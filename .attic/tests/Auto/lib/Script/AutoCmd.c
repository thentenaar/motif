/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: AutoCmd.c /main/14 1999/04/23 18:21:47 jff $"

#endif
#endif
#include <sys/types.h>

#include <mvslib.h>
#include <xislib.h>

#include <X11/IntrinsicP.h>
#include <Xm/MessageB.h>
#include <Xm/ScaleP.h>
#include <Xm/ScrollBarP.h>
#include <testlib.h>
#include <AutoMessages.h>

#include "AutoToken.h"
#include "Automation.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#define	SCROLLBAR_CHILD		1
#define SYNCWIDTH               50
#define SYNCHEIGHT              50

extern void xisKeyPress();
extern void xisKeyRelease();
extern void xisClickKey();

extern   Boolean  MonitorOn;
Boolean  SyncWidgetCreated = False;
Boolean  SyncWidgetPoppedup = False;
Widget   SyncWidget;

extern  Display *display;
extern  Widget  Shell1;

Boolean AutoManualMode;
Boolean AutoTimeOut;

#define ManString	"** - YOU ARE NOW IN MANUAL MODE - **\n\n** - Click on\
 OK to get back into AUTOMATED Mode - **"


static Pixel GetColor(
			char *colorstr);
static void UnPostDialogCB(
			Widget widget, 
			XtPointer call_data, 
			XtPointer client_data);
static void CalcSliderRect(
                        XmScrollBarWidget sbw,
                        short *slider_x,
                        short *slider_y,
                        short *slider_width,
                        short *slider_height,
                        int value);
static void CalcScrollBarData(
                        XmScaleWidget sw,
                        int valuein,
                        int *value,
                        int *slider_size);
static void AutoTimeoutProc(
			XtPointer data,
			XtIntervalId *id);



void
AutoMoveMaxOrMinText(
        Widget widget,
        int location )
{
    int widget_class_code, n;
    XisObjectRecord   *object;
    char msg_string[125];
    Arg	args[5];
    int x, y, width, height, move_x, move_y;
    Dimension shadow, border, highlight, margin_width, margin_height;

    xisProcessObjects();
    xisUpdateObjectAttributes();

    

    widget_class_code  = mvsGetClassCode(widget);
    if (widget_class_code != mvsXmTextWidgetClass &&
	widget_class_code != mvsXmTextFieldWidgetClass) {
	AutoMessage(_AutoMessages[SCRMSG1]);
	return;
    }

    object = xisFindObjectFromWidget(widget);

    x = object->x;
    y = object->y;
    width = object->width;
    height = object->height;

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, &shadow);		n++;
    XtSetArg(args[n], XmNborderWidth, &border);			n++;
    XtSetArg(args[n], XmNhighlightThickness, &highlight);	n++;
    XtSetArg(args[n], XmNmarginWidth, &margin_width);		n++;
    XtSetArg(args[n], XmNmarginHeight, &margin_height);		n++;
    XtGetValues(widget, args, n);

    if (location == AUTOMAX) {
    	move_x = (x + width) 
			- (shadow + border + highlight + margin_width) - 1;
    	move_y = (y + height) 
			- (shadow + border + highlight + margin_height) - 1;
    }
    else {
	move_x = x + (shadow + border + highlight + margin_width + 1);
	move_y = y + (shadow + border + highlight + margin_height + 1);
    }

    AutoProcessAllEvents();
    AutoLocatePointerAbs(move_x, move_y);

}


int
AutoGetMaxListPosition(
        Widget widget )
{
    int widget_class_code, n;
    int visible_count, item_count;
    char msg_string[125];
    Arg	args[2];



    widget_class_code  = mvsGetClassCode(widget);

    switch (widget_class_code) {
	case mvsXmTextWidgetClass:	break;
	case mvsXmListWidgetClass:	n = 0;
					XtSetArg(args[n], XmNvisibleItemCount, 
						 &visible_count);	n++;
					XtSetArg(args[n], XmNitemCount, 
						 &item_count);	n++;
					XtGetValues(widget, args, n);
					if (item_count < visible_count)
						return(item_count - 1);
					else
						return(visible_count - 1);
	default:			AutoMessage(_AutoMessages[SCRMSG1]);
					return(0);
    }
}


/* 
   Need to do system process manually and not call system() directly
   because some systems still use old fork() process of coping entire
   image. vfork() will use pointers to process.
*/

void
AutoSystem(
        char *command_name,
        char **command_args )
{
	pid_t	chl_pid;
	char	msg_string[125];

#ifdef PORT_NOVFORK
	chl_pid = fork();
#else
	chl_pid = vfork();
#endif
	if (chl_pid == -1)
		AutoError(_AutoMessages[WARNMSG1]);
	else 
	{
		if (!(chl_pid))
		{ /* 0 return is child process image */
#ifdef DEBUG
			printf("[child]: id=%ld\n", (long)getpid());
#endif
			if (execvp(command_name, command_args) == -1) {
				sprintf(msg_string, _AutoMessages[WARNMSG122], errno);
				  
				AutoMessage(msg_string);
				exit(1);
			}
		}
		else
		{ /* pid return is parent process image (pid is child's pid) */
#ifdef DEBUG
			printf("[parent]: child's id=%ld\n", (long)chl_pid);
#endif
			waitpid(chl_pid, NULL, 0);
#ifdef DEBUG
			printf("[parent]: after wait\n");
#endif
		}
	}
}
    

void
AutoExit( void )
{

    xisRestorePointer();
    mvsCloseVisual();

}

static Boolean
AutoWindowMapped (Widget widget)
{
  XWindowAttributes window_attributes;

  /* use AutoError if Get fails  and add into AutoMessages new message */
  XGetWindowAttributes(display, XtWindow(widget), &window_attributes);

  if (window_attributes.map_state == IsViewable)
    return (True);
  else
    return (False);
}




void
PopdownSyncCallBack(
	     Widget widget,
	     XtPointer call_data,
	     XtPointer client_data)
{

  enum WHICH_TIME get_which_time;
  extern void AutoGetTime();
  extern void AutoPrintDurationTime();
  extern struct   timeval StartTime, EndTime;

  while (AutoWindowMapped(SyncWidget) == True);
  get_which_time = END_TIME;
  AutoGetTime(get_which_time);
  AutoPrintDurationTime(&StartTime, &EndTime);
  Command_In_Progress = False;
  SyncWidgetPoppedup = False;
}

static XtCallbackRec PopdownCR[] = 
{ 
  {PopdownSyncCallBack, NULL}, 
  {NULL, NULL}
};

void
AutoInvokePerformMonitor()
{
  enum WHICH_TIME get_which_time;
  extern void AutoGetTime();
  Widget PB;
  Arg args[MAX_ARGS];
  int n;


  if (MonitorOn == True)  {
    Command_In_Progress = True;
    /* Use SyncWidget to synchronize the timing of the performance
       monitor wrt the events in the queue.
     */
    if (SyncWidgetCreated != True)  {
      n = 0;
      XtSetArg(args[n], XtNwidth, SYNCWIDTH);                                n++;
      XtSetArg(args[n], XtNheight, SYNCHEIGHT);                              n++;
      XtSetArg(args[n], XtNy, (XHeightOfScreen(screen) - (SYNCHEIGHT+5)));   n++;
      XtSetArg(args[n], XtNx, (XWidthOfScreen(screen) - (SYNCWIDTH+5)));     n++;
      XtSetArg(args[n], XmNpopdownCallback, PopdownCR);                      n++;
      SyncWidget = XtCreatePopupShell ("SyncWidget", topLevelShellWidgetClass,
				       Shell1, args, n);
      XtRealizeWidget(SyncWidget);
      XtPopup(SyncWidget, XtGrabNone);
      SyncWidgetCreated = True;
    }
    else if (SyncWidgetPoppedup == False)  {
      XtPopup(SyncWidget, XtGrabNone);
    }
    SyncWidgetPoppedup = True;
    get_which_time = START_TIME;
    AutoGetTime(get_which_time);
  }
}

void
AutoProcessAllEvents( void )
{

    XSync(display, False);
    xisProcessEvents(NULL,0);
    XSync(display, False);

}


void
AutoProcessKey(
        int key,
        int mask,
        int func_type )
{
    void            (*keyproc)();

    switch (func_type) {
	case CLICKKEY:		keyproc = xisClickKey;
				break;
	case PRESSKEY:		keyproc = xisKeyPress;
				break;
	case RELEASEKEY:	keyproc = xisKeyRelease;
				break;
	default:		AutoError(_AutoMessages[WARNMSG87]);
				break;
    }

    switch (key) {
	case BACKSPACE:
		(*keyproc)(mask | KeyBackSpace);
		break;
	case INSERT:
		(*keyproc)(mask | KeyInsert);
		break;
	case DELETE:
		(*keyproc)(mask | KeyDelete);
		break;
	case COPY:
		(*keyproc)(mask | KeyCopy);
		break;
	case CUT:
		(*keyproc)(mask | KeyCut);
		break;
	case PASTE:
		(*keyproc)(mask | KeyPaste);
		break;
	case ADDMODE:
		(*keyproc)(mask | KeyAddMode);
		break;
	case PRIMARYPASTE:
		(*keyproc)(mask | KeyPrimaryPaste);
		break;
	case QUICKPASTE:
		(*keyproc)(mask | KeyQuickPaste);
		break;
	case PAGEUP:
		(*keyproc)(mask | KeyPageUp);
		break;
	case PAGEDOWN:
		(*keyproc)(mask | KeyPageDown);
		break;
	case ENDLINE:
		(*keyproc)(mask | KeyEndLine);
		break;
	case BEGINLINE:
		(*keyproc)(mask | KeyBeginLine);
		break;
	case ACTIVATE:
		(*keyproc)(mask | KeyActivate);
		break;
	case MENUBAR:
		(*keyproc)(mask | KeyMenuBar);
		break;
	case CLEAR:
		(*keyproc)(mask | KeyClear);
		break;
	case CANCEL:
		(*keyproc)(mask | KeyCancel);
		break;
	case HELP:
		(*keyproc)(mask | KeyHelp);
		break;
	case MENU:
		(*keyproc)(mask | KeyMenu);
		break;
	case SELECT:
		(*keyproc)(mask | KeySelect);
		break;
	case UNDO:
		(*keyproc)(mask | KeyUndo);
		break;
	case UPK:
		(*keyproc)(mask | KeyUp);
		break;
	case RIGHTK:
		(*keyproc)(mask | KeyRight);
		break;
	case DOWNK:
		(*keyproc)(mask | KeyDown);
		break;
	case LEFTK:
		(*keyproc)(mask | KeyLeft);
		break;
	case BACKTAB:
		(*keyproc)(mask | KeyBackTab);
		break;
	case BEGINDATA:
		(*keyproc)(mask | KeyBeginData);
		break;
	case DESELECTALL:
		(*keyproc)(mask | KeyDeselectAll);
		break;
	case ENDDATA:
		(*keyproc)(mask | KeyEndData);
		break;
	case ENTER:
		(*keyproc)(mask | KeyEnter);
		break;
	case ESCAPE:
		(*keyproc)(mask | KeyEscape);
		break;
	case EXTEND:
		(*keyproc)(mask | KeyExtend);
		break;
	case NEXTFIELD:
		(*keyproc)(mask | KeyNextField);
		break;
	case NEXTMENU:
		(*keyproc)(mask | KeyNextMenu);
		break;
	case PAGERIGHT:
		(*keyproc)(mask | KeyPageRight);
		break;
	case PAGELEFT:
		(*keyproc)(mask | KeyPageLeft);
		break;
	case PREVFIELD:
		(*keyproc)(mask | KeyPrevField);
		break;
	case PREVMENU:
		(*keyproc)(mask | KeyPrevMenu);
		break;
	case PRIMARYCOPY:
		(*keyproc)(mask | KeyPrimaryCopy);
		break;
	case PRIMARYCUT:
		(*keyproc)(mask | KeyPrimaryCut);
		break;
	case QUICKCOPY:
		(*keyproc)(mask | KeyQuickCopy);
		break;
	case QUICKCUT:
		(*keyproc)(mask | KeyQuickCut);
		break;
	case QUICKEXTEND:
		(*keyproc)(mask | KeyQuickExtend);
		break;
	case RESELECT:
		(*keyproc)(mask | KeyReselect);
		break;
	case RESTORE:
		(*keyproc)(mask | KeyRestore);
		break;
	case SELECTALL:
		(*keyproc)(mask | KeySelectAll);
		break;
	case SPACE:
		(*keyproc)(mask | KeySpace);
		break;
	case TAB:
		(*keyproc)(mask | KeyTab);
		break;
	case KEYA:
		(*keyproc)(mask | Keya);
		break;
	case KEYB:
		(*keyproc)(mask | Keyb);
		break;
	case KEYC:
		(*keyproc)(mask | Keyc);
		break;
	case KEYD:
		(*keyproc)(mask | Keyd);
		break;
	case KEYE:
		(*keyproc)(mask | Keye);
		break;
	case KEYF:
		(*keyproc)(mask | Keyf);
		break;
	case KEYG:
		(*keyproc)(mask | Keyg);
		break;
	case KEYH:
		(*keyproc)(mask | Keyh);
		break;
	case KEYI:
		(*keyproc)(mask | Keyi);
		break;
	case KEYJ:
		(*keyproc)(mask | Keyj);
		break;
	case KEYK:
		(*keyproc)(mask | Keyk);
		break;
	case KEYL:
		(*keyproc)(mask | Keyl);
		break;
	case KEYM:
		(*keyproc)(mask | Keym);
		break;
	case KEYN:
		(*keyproc)(mask | Keyn);
		break;
	case KEYO:
		(*keyproc)(mask | Keyo);
		break;
	case KEYP:
		(*keyproc)(mask | Keyp);
		break;
	case KEYQ:
		(*keyproc)(mask | Keyq);
		break;
	case KEYR:
		(*keyproc)(mask | Keyr);
		break;
	case KEYS:
		(*keyproc)(mask | Keys);
		break;
	case KEYT:
		(*keyproc)(mask | Keyt);
		break;
	case KEYU:
		(*keyproc)(mask | Keyu);
		break;
	case KEYV:
		(*keyproc)(mask | Keyv);
		break;
	case KEYW:
		(*keyproc)(mask | Keyw);
		break;
	case KEYX:
		(*keyproc)(mask | Keyx);
		break;
	case KEYY:
		(*keyproc)(mask | Keyy);
		break;
	case KEYZ:
		(*keyproc)(mask | Keyz);
		break;
	case KEYSLASH:
		(*keyproc)(mask | KeySlash);
		break;
	case KEYBACKSLASH:
		(*keyproc)(mask | KeyBackSlash);
		break;
	case KEYBRACERIGHT:
		(*keyproc)(mask | KeyBraceRight);
		break;
	case KEYBRACELEFT:
		(*keyproc)(mask | KeyBraceLeft);
		break;
	case KEYBRACKETRIGHT:
		(*keyproc)(mask | KeyBracketRight);
		break;
	case KEYBRACKETLEFT:
		(*keyproc)(mask | KeyBracketLeft);
		break;
	case KEYRETURN:
		(*keyproc)(mask | KeyReturn);
		break;
	case KEY1:
		(*keyproc)(mask | Key1);
		break;
	case KEY2:
		(*keyproc)(mask | Key2);
		break;
	case KEY3:
		(*keyproc)(mask | Key3);
		break;
	case KEY4:
		(*keyproc)(mask | Key4);
		break;
	case KEY5:
		(*keyproc)(mask | Key5);
		break;
	case KEY6:
		(*keyproc)(mask | Key6);
		break;
	case KEY7:
		(*keyproc)(mask | Key7);
		break;
	case KEY8:
		(*keyproc)(mask | Key8);
		break;
	case KEY9:
		(*keyproc)(mask | Key9);
		break;
	case KEY0:
		(*keyproc)(mask | Key0);
		break;
	case KEYEXCLAM:
		(*keyproc)(mask | KeyExclamation);
		break;
	case KEYAT:
		(*keyproc)(mask | KeyAt);
		break;
	case KEYNUMBERSIGN:
		(*keyproc)(mask | KeyNumberSign);
		break;
	case KEYDOLLAR:
		(*keyproc)(mask | KeyDollar);
		break;
	case KEYPERCENT:
		(*keyproc)(mask | KeyPercent);
		break;
	case KEYCARET:
		(*keyproc)(mask | KeyCaret);
		break;
	case KEYAMPERSAND:
		(*keyproc)(mask | KeyAmpersand);
		break;
	case KEYASTERISK:
		(*keyproc)(mask | KeyAsterisk);
		break;
	case KEYPARENLEFT:
		(*keyproc)(mask | KeyParenLeft);
		break;
	case KEYPARENRIGHT:
		(*keyproc)(mask | KeyParenRight);
		break;
	case KEYNONE:
		(*keyproc)(mask | KeyNone);
		break;
	default:
		AutoMessage(_AutoMessages[WARNMSG2]);
		break;
    }
}


void
AutoInputString(
        char *istring )
{
    int 	i;
    int 	code;
    Boolean 	AddShift, valid;
    int 	KeyStart = KEYA;
    int 	NumberStart = KEY0;
    char	msg_string[125];

    for (i = 1; istring[i] != '"'; i++) {
	AddShift = False;
	valid = False;
	/* Character in string is a number */
	if (isdigit(istring[i])) {
		code = (istring[i] - '0') + NumberStart;
		valid = True;
	}
	/* Character in string is a letter */
	if (isalpha(istring[i]))
		if (isupper(istring[i])) {
			AddShift = True;
			code = (tolower(istring[i]) - 'a') + KeyStart;
			valid = True;
		}
		else {
			code = (istring[i] - 'a') + KeyStart;
			valid = True;
		}
	/* Special Cases */
	if (istring[i] == '!') {
		code = KEYEXCLAM;
		valid = True;
	}
	if (istring[i] == '@') {
		code = KEYAT;
		valid = True;
	}
	if (istring[i] == '#') {
		code = KEYNUMBERSIGN;
		valid = True;
	}
	if (istring[i] == '$') {
		code = KEYDOLLAR;
		valid = True;
	}
	if (istring[i] == '%') {
		code = KEYPERCENT;
		valid = True;
	}
	if (istring[i] == '^') {
		code = KEYCARET;
		valid = True;
	}
	if (istring[i] == '&') {
		code = KEYAMPERSAND;
		valid = True;
	}
	if (istring[i] == '*') {
		code = KEYASTERISK;
		valid = True;
	}
	if (istring[i] == '(') {
		code = KEYPARENLEFT;
		valid = True;
	}
	if (istring[i] == ')') {
		code = KEYPARENRIGHT;
		valid = True;
	}
	if (istring[i] == '/') {
		code = KEYSLASH;
		valid = True;
	}
	if (istring[i] == '{') {
		code = KEYBRACKETLEFT;
		valid = True;
	}
	if (istring[i] == '}') {
		code = KEYBRACKETRIGHT;
		valid = True;
	}
	if (istring[i] == '[') {
		code = KEYBRACELEFT;
		valid = True;
	}
	if (istring[i] == ']') {
		code = KEYBRACERIGHT;
		valid = True;
	}
	if (istring[i] == ' ') {
		code = SPACE;
		valid = True;
	}
	if (istring[i] == '\\') {
		if (istring[i + 1] == 'n') {
			code = KEYRETURN;
			valid = True;
			i++;
		}
		else {
			if (istring[i + 1] == 't') {
				code = TAB;
				valid = True;
				i++;
			}
			else {
				code = KEYBACKSLASH;
				valid = True;
			}
		}
	}

	if (istring[i] == '\t') {
		code = TAB;
		valid = True;
	}
	if (istring[i] == '\n') {
		code = KEYRETURN;
		valid = True;
	}

	if (! valid) {
		sprintf(msg_string, _AutoMessages[SCRMSG38], istring[i]);
		AutoMessage(msg_string);
	}
	else
		if (AddShift)
			AutoProcessKey(code, ShiftMask, CLICKKEY);
		else
			AutoProcessKey(code, NoModifierKeys, CLICKKEY);
    }

}
		

void
AutoContinueOrEnd(
        Widget widget,
        int Component )
{

    XSync(display, False);
    AutoLocatePointer(widget, Component, 0, MoveAny);
    AutoProcessAllEvents();
    AutoClickMB(NoModifierKeys, Button1, 0);
    XSync(display, False);

}


void
AutoDrag(
        Widget widget,
        int Component,
        int Location,
        int mask,
        int XButton )
{

    xisPressMouseButton(mask, XButton);
    xisMovePointerTo(widget, Component, Location, MoveAny);
    xisReleaseMouseButton(mask, XButton);

}


void
AutoDragAbs(
        int XCoord,
        int YCoord,
        int mask,
        int XButton )
{

    xisPressMouseButton(mask, XButton);
    xisMovePointer(XCoord, YCoord);
    xisReleaseMouseButton(mask, XButton);

}


void
AutoDragRelative(
        int location,
        int XCoord,
        int YCoord,
        int mask,
        int XButton )
{
    int		x, y;
    Boolean	invalid = False;

    switch(location) {
	case UP:	y = -XCoord;
			x = 0;
			break;
	case DOWN:	y = XCoord;
			x = 0;
			break;
	case LEFT:	y = 0;
			x = -XCoord;
			break;
	case RIGHT:	y = 0;
			x = XCoord;
			break;
	case ANY:	x = XCoord;
			y = YCoord;
			break;
	case OFF:
	default:	invalid = True;
			AutoMessage(_AutoMessages[SCRMSG2]);
			break;
    }
		
    if (!invalid) {
	xisPressMouseButton(mask, XButton);
	xisMovePointerRelative(x, y);
	xisReleaseMouseButton(mask, XButton);
    }

}


void
AutoDragSliderValue(
        int location,
        int XCoord,
        int YCoord,
        int mask,
        int XButton )
{
    int 		x, y, widget_class_code, n;
    int			maximum, minimum, value;
    Boolean		invalid = False;
    XisObjectRecord 	*object;
    Widget		widget, tmp_widget;
    CompositeWidget 	cw;
    XmScrollBarWidget   sb_widget;
    Boolean		IsScale = False;
    Arg			args[5];
    short 		s_x1, s_y1, s_width1, s_height1;
    short 		s_x2, s_y2, s_width2, s_height2;
    short		change_x, change_y;
    int 		value1, value2, slider_size, slidersize;

    xisGetPointerLocation(&x,&y);
    object = xisFindObjectAtLocation(x, y);

    widget = (Widget) object->id.widget;
    if (widget == NULL)
	AutoError(_AutoMessages[SCRMSG18]);

    tmp_widget = XtParent(widget);
    widget_class_code  = mvsGetClassCode(tmp_widget);

    if (widget_class_code == mvsXmScaleWidgetClass) {
	widget = tmp_widget;
	IsScale = True;
    }

    widget_class_code  = mvsGetClassCode(widget);
    if (widget_class_code != mvsXmScaleWidgetClass)
	if (widget_class_code != mvsXmScrollBarWidgetClass) {
		AutoMessage(_AutoMessages[SCRMSG19]);
		return;
	}

    n = 0;
    XtSetArg(args[n], XmNvalue, &value);				n++;
    XtSetArg(args[n], XmNminimum, &minimum);				n++;
    XtSetArg(args[n], XmNmaximum, &maximum);				n++;
    XtSetArg(args[n], XmNsliderSize, &slidersize);		n++;
    XtGetValues(widget, args, n);

    if (! IsScale && XCoord == AUTOMAX)
	XCoord = maximum - slidersize;
    else
	if (! IsScale && XCoord != AUTOMIN)
		XCoord = (int) (((maximum - slidersize) * XCoord) / 100);

    if (XCoord == AUTOMAX && IsScale)
	XCoord = maximum;

    if (XCoord == AUTOMIN)
	XCoord = minimum;

    /* We know that the second composite child is the Scrollbar child */

    if (IsScale) {
    	cw = (CompositeWidget) widget;
    	sb_widget = (XmScrollBarWidget) cw->composite.children[SCROLLBAR_CHILD];

    	widget_class_code  = mvsGetClassCode((Widget) sb_widget);
    	if (widget_class_code != mvsXmScrollBarWidgetClass)
		AutoError(_AutoMessages[SCRMSG20]);
		     

    	CalcScrollBarData((XmScaleWidget)widget, value, &value1, &slider_size);
    	CalcScrollBarData((XmScaleWidget)widget, XCoord, &value2, &slider_size);

    	CalcSliderRect(sb_widget, &s_x1, &s_y1, &s_width1, &s_height1, value1);
    	CalcSliderRect(sb_widget, &s_x2, &s_y2, &s_width2, &s_height2, value2);
    }
    else {
	CalcSliderRect((XmScrollBarWidget)widget, &s_x1, &s_y1, 
			&s_width1, &s_height1, value);
    	CalcSliderRect((XmScrollBarWidget)widget, &s_x2, &s_y2, 
			&s_width2, &s_height2, XCoord);
    }

    change_x = abs(s_x2 - s_x1);
    change_y = abs(s_y2 - s_y1);

    switch(location) {
   	case DOWN:     	x = 0;
			y = change_y;
			break;
	case UP:	x = 0;
			y = -change_y;
			break;
	case RIGHT:     y = 0;
			x = change_x;
			break;
      	case LEFT:      y = 0;
			x = -change_x;
			break;
	default:        invalid = True;
			AutoMessage(_AutoMessages[SCRMSG3]);
			break;
    }

    if (!invalid) {
	xisPressMouseButton(mask, XButton);
	xisMovePointerRelative(x, y);
	xisReleaseMouseButton(mask, XButton);
    }

}


void
AutoPressMB(
        int mask,
        int XButton )
{

    xisPressMouseButton(mask, XButton);

}


void
AutoReleaseMB(
        int mask,
        int XButton )
{

    xisReleaseMouseButton(mask, XButton);

}


void
AutoClickMB(
        int mask,
        int XButton,
        int multi_click )
{

    if (multi_click > 0)
	xisMultiClickMouseButton(mask, XButton, multi_click);
    else {
	xisPressMouseButton(mask, XButton);
	xisReleaseMouseButton(mask, XButton);
    }

}


void
AutoSetFocus(
        Widget widget,
        int Component )
{
    int 	focus_mode = True;

    mvsSetFocus(widget, oUserDefined, 0, focus_mode);

}

void
AutoWait( int count )
{

    AutoTimeOut = True;

    XtAppAddTimeOut(XtWidgetToApplicationContext(Shell1),
		    count * 1000,
		    AutoTimeoutProc,
		    NULL);

    while (AutoTimeOut == True)
	xisSynchronize();

}


static void
AutoTimeoutProc(
        XtPointer data,
        XtIntervalId *id )
{

    AutoTimeOut = False;

}


void
AutoCompareVisual(
        MvsWidgetInfoRecord *w_info )
{

    mvsCheckVisual(w_info);

}


void
AutoCompareTempVisual(
        MvsWidgetInfoRecord *w_info,
        char *identifier )
{

    mvsCheckCompareTempVisual(w_info, identifier);

}

void
AutoStoreVisual(
        MvsWidgetInfoRecord *w_info,
        char *identifier )
{

    mvsCheckStoreVisual(w_info, identifier);

}


void
AutoLocatePointerAbs(
        int XCoord,
        int YCoord )
{

    xisMovePointer(XCoord, YCoord);

}


void
AutoLocatePointerRel(
        Widget widget,
        int Component,
        int Location,
        int XCoord,
        int YCoord )
{
    XisObjectRecord *target_object;

    target_object = xisFindObject(widget, Component, Location);
    xisMovePointer(target_object->x + XCoord, target_object->y + YCoord);

}


void
AutoLocatePointer(
        Widget widget,
        int Component,
        int Location,
        int direction )
{

    xisMovePointerTo(widget, Component, Location, direction);

}


void
AutoDelayCycle(
        int delay_num )
{

    AutoWait(delay_num);

} 


void
AutoGoManual( void )
{
    static Boolean 	first_time = True;
    int    	   	n;
    Arg		   	args[10];
    XmString		tcs;
    static Widget	Dialog;
    int			save_x, save_y;

    save_x = xisSavePointerX;
    save_y = xisSavePointerY;
    xisSavePointer();

    if (first_time) {

	tcs = XmStringCreateLtoR(ManString, XmSTRING_DEFAULT_CHARSET);

    	n = 0;
	XtSetArg(args[n], XmNx, 500);					n++;
	XtSetArg(args[n], XmNy, 0);					n++;
    	XtSetArg(args[n], XmNmessageString, tcs);			n++;
    	XtSetArg(args[n], XmNbackground, GetColor("#d0d0d8"));		n++;
	XtSetArg(args[n], XmNautoUnmanage, True);			n++;
	XtSetArg(args[n], XmNdefaultPosition, False);			n++;
	XtSetArg(args[n], XmNshadowThickness, 10);			n++;
	XtSetArg(args[n], XmNmarginWidth, 15);				n++;
	XtSetArg(args[n], XmNmarginHeight, 15);                     	n++;
	Dialog = XmCreateInformationDialog(Shell1, "Dialog", args, n);

	XtAddCallback(Dialog, XmNokCallback, UnPostDialogCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNsensitive, False);				n++;
	XtSetValues(XmMessageBoxGetChild(Dialog, XmDIALOG_HELP_BUTTON),
		    args, n);

	XmStringFree(tcs);

	first_time = False;
    }

    XtManageChild(Dialog);

    XtUnmanageChild(XmMessageBoxGetChild(Dialog, XmDIALOG_CANCEL_BUTTON));

    while (AutoManualMode == True)
	xisSynchronize();

    xisProcessObjects();
    xisUpdateObjectAttributes();

    xisRestorePointer();
    xisSavePointerX = save_x;
    xisSavePointerY = save_y;


}



static void
UnPostDialogCB(Widget widget, XtPointer call_data, XtPointer client_data)
{

    AutoManualMode = False;

}


static
Pixel GetColor(char *colorstr)

{
   XrmValue from, to;

   from.size = strlen(colorstr) +1;
   if (from.size < sizeof(String)) from.size = sizeof(String);
   from.addr = colorstr;
   to.addr = NULL;
   XtConvert(Shell1, XmRString, &from, XmRPixel, &to);
    if (to.addr != NULL)
      return ((Pixel) *((Pixel *) to.addr));
    else
      return ((Pixel) NULL);

}


static void
CalcSliderRect(
        XmScrollBarWidget sbw,
        short *slider_x,
        short *slider_y,
        short *slider_width,
        short *slider_height,
        int value )
{
   float userSize;
   float trueSize;
   float factor;
   float slideStart;
   float slideSize;
   int arrowWidth;
   int minSliderWidth;
   int minSliderHeight;

   userSize = sbw->scrollBar.maximum - sbw->scrollBar.minimum;

   if (sbw->scrollBar.orientation == XmHORIZONTAL)
   {
      arrowWidth = sbw->scrollBar.arrow_width;
      trueSize =  sbw->scrollBar.slider_area_width;
   }
   else
   {
      arrowWidth = sbw->scrollBar.arrow_height;
      trueSize = sbw->scrollBar.slider_area_height;
   }


   if (arrowWidth == 0)
      arrowWidth -= 1;

   arrowWidth += sbw->primitive.highlight_thickness + 
                 sbw->primitive.shadow_thickness + 1;


   factor = trueSize / userSize;

   slideStart = (float)(value - sbw->scrollBar.minimum) * factor + arrowWidth;

   slideSize = (float) (sbw->scrollBar.slider_size) * factor;

   if (sbw->scrollBar.orientation == XmHORIZONTAL)
   {
      *slider_x = (int) (slideStart + 0.5);
      *slider_y = sbw->scrollBar.slider_area_y;
      *slider_width = (int) (slideSize + 0.5);
      *slider_height = sbw->scrollBar.slider_area_height;
      minSliderWidth = MIN_SLIDER_LENGTH;
      minSliderHeight = MIN_SLIDER_THICKNESS;
   }
   else
   {
      *slider_x = sbw->scrollBar.slider_area_x;
      *slider_y = (int) (slideStart + 0.5);
      *slider_width = sbw->scrollBar.slider_area_width;
      *slider_height = (int) (slideSize + 0.5);
      minSliderWidth = MIN_SLIDER_THICKNESS;
      minSliderHeight = MIN_SLIDER_LENGTH;
   }

   if (*slider_width < minSliderWidth)
   {
      *slider_width = minSliderWidth;
      if (sbw->scrollBar.orientation == XmHORIZONTAL &&
          *slider_x + *slider_width > 
          sbw->scrollBar.slider_area_x + sbw->scrollBar.slider_area_width)
         *slider_x = sbw->scrollBar.slider_area_x + 
                     sbw->scrollBar.slider_area_width - *slider_width;
   }

   if (*slider_height < minSliderHeight)
   {
      *slider_height = minSliderHeight;
      if (sbw->scrollBar.orientation == XmVERTICAL &&
          *slider_y + *slider_height > 
          sbw->scrollBar.slider_area_y + sbw->scrollBar.slider_area_height)
         *slider_y = sbw->scrollBar.slider_area_y + 
                     sbw->scrollBar.slider_area_height - *slider_height;
   }
}

#define SCROLLBAR_MAX   1000000000
#define SCROLLBAR_MIN   0
#define SLIDER_SIZE     30


static void
CalcScrollBarData(
        XmScaleWidget sw,
        int valuein,
        int *value,
        int *slider_size )
{
	Dimension scrollbar_size;
	int size ;
	float sb_value;

	if (sw->scale.orientation == XmHORIZONTAL)
		scrollbar_size = sw->composite.children[1]->core.width;
	else
		scrollbar_size = sw->composite.children[1]->core.height;

	size = scrollbar_size - 2 * (sw->scale.highlight_thickness
			+ sw->manager.shadow_thickness);

	if (size <= 0)
	    scrollbar_size = 1;
	else
	    scrollbar_size -= 2 * (sw->scale.highlight_thickness
			+ sw->manager.shadow_thickness);

	*slider_size = ((SCROLLBAR_MAX - SCROLLBAR_MIN) / scrollbar_size)
		* SLIDER_SIZE;

	if (*slider_size < 0)
	{
		*slider_size = SCROLLBAR_MAX;
	}
	else if (*slider_size < 1)
	{
		*slider_size = 1;
	}
	else if (*slider_size < SCROLLBAR_MIN)
	{
		*slider_size = SCROLLBAR_MIN;
	}
	else if (*slider_size > SCROLLBAR_MAX)
	{
		*slider_size = SCROLLBAR_MAX;
	}


	sb_value = (float) (valuein - sw->scale.minimum) / 
		(float) (sw->scale.maximum - sw->scale.minimum);
	sb_value = sb_value * 
		(float) (SCROLLBAR_MAX - *slider_size - SCROLLBAR_MIN);

	*value = (int) sb_value;

	if (*value > SCROLLBAR_MAX - *slider_size)
		*value = SCROLLBAR_MAX - *slider_size;
	if (*value < SCROLLBAR_MIN)
		*value = SCROLLBAR_MIN;
}
