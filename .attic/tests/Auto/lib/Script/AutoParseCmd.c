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
static char rcsid[] = "$XConsortium: AutoParseCmd.c /main/12 1995/07/14 11:28:20 drk $"
#endif
#endif

#include <xislib.h>
#include <mvslib.h>
#include <testlib.h>
#include "AutoToken.h"
#include "Automation.h"
#include "AutoMwm.h"
#include <AutoMessages.h>


char 				*sysargs[CommandSize];
extern Widget 			Shell1;
extern MvsWidgetInfoRecord      *Shell_info;
extern Boolean			HaveScrolledWindow;
AutoCommandStructPtr		Command;
Boolean				SendToClipWindow = False;
Boolean				keyboard;
Boolean                         Command_In_Progress;
Boolean                         MonitorOn = False;


static int	GetMask(int num_mask, int mask_array[5]);
static int 	GetXMask(int mask);
static int 	GetButton(int button);
static int 	GetComponent(int component);
static Widget 	GetWidgetID(char *widget_string);
static Widget 	GetTopShell(Widget widget);
static void 	CheckKey(int key);
static void 	AutoUpdateWindow(void);



/* 
  Go through the Command List and Generate the appropriate
  Automation command to execute the commmand.
  */

void
AutoParseAndGenerateCommands( void )
{
  
  MvsWidgetInfoRecord		*w_info;
  XisObjectRecord   		*object;
  char			        *item_name;
  Widget			widget;
  Window			window;
  Boolean			MoreInput = True;
  Boolean			null_move = False;
  Boolean			NextScreen = False;
  Boolean			use_object = False;
  static Boolean		parsed = False;
  static int			command_num = 1;
  char		                *inputstring, *name;
  int				item_number, gravity, XCoord, YCoord;
  int				i, multi_click,	mask, XButton, group;
  int				Component, location, Location, direction;
  int				widget_class_code = 0;
  register int                  n;
  Arg                           args[MAX_ARGS];
  int                           PerformanceOnOff;
  
  /* Parse input, creating command structure */
  
  if (!parsed) {
    yyparse();
    parsed = True;
    if (AutoCommandRoot->next == NULL) 
      AutoError(_AutoMessages[WARNMSG88]);
    else
      Command = AutoCommandRoot->next;
  }

  Command_In_Progress = FALSE;
  
  while (MoreInput) {

    /* 
      Wait until the previous command is not in progress
      when doing performance measurment 
     */
    while (Command_In_Progress == True);
    
    switch (Command->CommandName) {
    case PRESSMB:
      if (AutoTrace)
	AutoTraceButtonAction(Command->ButtonNumber,
			      Command->Mask,
			      Command->NumMask,
			      0, PRESSMB, command_num);
      mask = GetMask(Command->NumMask, Command->Mask);
      XButton = GetButton(Command->ButtonNumber);
      AutoInvokePerformMonitor();
      AutoPressMB(mask, XButton);
      break;
    case RELEASEMB:
      if (AutoTrace)
	AutoTraceButtonAction(Command->ButtonNumber,
			      Command->Mask,
			      Command->NumMask,
			      0, RELEASEMB,
			      command_num);
      mask = GetMask(Command->NumMask, Command->Mask);
      XButton = GetButton(Command->ButtonNumber);
      AutoInvokePerformMonitor();
      AutoReleaseMB(mask, XButton);
      break;
    case CLICKMB:
      if (AutoTrace)
	AutoTraceButtonAction(Command->ButtonNumber,
			      Command->Mask,
			      Command->NumMask,
			      Command->KeyOrButtonCount,
			      CLICKMB,
			      command_num);
      multi_click = Command->KeyOrButtonCount;
      mask = GetMask(Command->NumMask, Command->Mask);
      XButton = GetButton(Command->ButtonNumber);
      AutoInvokePerformMonitor();
      AutoClickMB(mask, XButton, multi_click);
      break;
    case CLICKKEY:
      if (AutoTrace)
	AutoTraceKeyAction(Command->Key, Command->Mask,
			   Command->NumMask, CLICKKEY,
			   command_num);
      CheckKey(Command->Key);
      mask = GetMask(Command->NumMask, Command->Mask);
      AutoInvokePerformMonitor();
      AutoProcessKey(Command->Key, mask, CLICKKEY);
      break;
    case PRESSKEY:
      if (AutoTrace)
	AutoTraceKeyAction(Command->Key, Command->Mask,
			   Command->NumMask, PRESSKEY,
			   command_num);
      CheckKey(Command->Key);
      mask = GetMask(Command->NumMask, Command->Mask);
      AutoInvokePerformMonitor();
      AutoProcessKey(Command->Key, mask, PRESSKEY);
      break;
    case RELEASEKEY:
      if (AutoTrace)
	AutoTraceKeyAction(Command->Key, Command->Mask,
			   Command->NumMask, RELEASEKEY,
			   command_num);
      CheckKey(Command->Key);
      mask = GetMask(Command->NumMask, Command->Mask);
      AutoInvokePerformMonitor();
      AutoProcessKey(Command->Key, mask, RELEASEKEY);
      break;
    case LOCATEPOINTERABS:
      if (AutoTrace)
	AutoTraceLocateAction(NULL, 0, 0, 
			      Command->XCoord,
			      Command->YCoord,
			      LOCATEPOINTERABS,
			      command_num);
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      AutoInvokePerformMonitor();
      AutoLocatePointerAbs(XCoord, YCoord);
      break;
    case LOCATEPOINTERREL:
      if (AutoTrace)
	AutoTraceLocateAction(Command->WidgetName,
			      Command->WidgetComponent,
			      Command->Location,
			      Command->XCoord,
			      Command->YCoord,
			      LOCATEPOINTERREL,
			      command_num);
      widget = GetWidgetID(Command->WidgetName);
      Component = GetComponent(Command->WidgetComponent);
      location = Command->Location;
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      AutoInvokePerformMonitor();
      AutoLocatePointerRel(widget, Component, location, XCoord, YCoord);
      break;
    case LOCATEPOINTER:
      if (AutoTrace)
	AutoTraceLocateAction(Command->WidgetName,
			      Command->WidgetComponent,
			      Command->Location,
			      0, 0, LOCATEPOINTER,
			      command_num);
      widget = GetWidgetID(Command->WidgetName);
      Component = GetComponent(Command->WidgetComponent);
      location = Command->Location;
      switch (location) {
      case OFF: 
	direction = MoveAny; 
	Location = 0;
	null_move = True;
	break;
      case RIGHT: 
	direction = MoveRight; 
	Location = 0;
	null_move = True;
	break;
      case LEFT:
	direction = MoveLeft; 
	Location = 0;
	null_move = True;
	break;
      case UP:
	direction = MoveUp; 
	Location = 0;
	null_move = True;
	break;
      case DOWN:
	direction = MoveDown; 
	Location = 0;
	null_move = True;
	break;
      case AUTOMIN:
	Location = 0;
	break;
      case -1:  
	Location = 0;
	break;
      default:  
	Location = location;
	break;
      }
      widget_class_code = mvsGetClassCode(widget);
      if (widget_class_code == mvsXmTextWidgetClass ||
	  widget_class_code == mvsXmTextFieldWidgetClass)
	use_object = False;
      else {
	use_object = True;
	object = xisFindObject(widget, Component, 0);
	if (object == NULL && (location == AUTOMAX ||
			       location == AUTOMIN))
		AutoError (_AutoMessages[WARNMSG89]);
	else {
	  if (location == AUTOMAX || 
	      location == AUTOMIN) 
	    widget_class_code  = 
	      mvsGetClassCode
		(object->id.widget);
	}
      }
      if (location == AUTOMAX && widget_class_code == 
	  mvsXmListWidgetClass) {
	if (use_object)  {
	  AutoInvokePerformMonitor();
	  Location = 
	    AutoGetMaxListPosition
	      (object->id.widget);
	}
	else  {
	  AutoInvokePerformMonitor();
	  Location =
	    AutoGetMaxListPosition
	      (widget);
	}
      }
      AutoInvokePerformMonitor();
      if ((location == AUTOMAX  || location == AUTOMIN) && 
	  (widget_class_code == mvsXmTextWidgetClass ||
	   widget_class_code == mvsXmTextFieldWidgetClass)) {
	if (use_object)  {
	  AutoMoveMaxOrMinText(object->id.widget, 
			       location);
	}
	else  {
	  AutoMoveMaxOrMinText(widget, location);
	}
      }
      else {
	if (! null_move)  {
	  AutoLocatePointer(widget, Component, 
			    Location, MoveAny);
	}
	else  {
	  AutoLocatePointer((Widget) NULL,
			    oUserDefined,
			    Location, direction);
	}
      }
      null_move = False;
      break;
    case COMPAREVISUAL:
      if (AutoTrace)
	AutoTraceCompareAction(Command->WidgetName,
			       Command->WidgetComponent,
			       command_num);
      widget = GetWidgetID(Command->WidgetName);
      /* take picture of a widget not a gadget */
      while (XmIsGadget(widget))
	widget = XtParent(widget);
      w_info = mvsWidgetToWidgetInfo(widget);
      /* 
	If widget is a ScrolledText or a ScrolledList
	widget, compare the ScrolledWindow parent and
	not the List or Text widget. This way, scrollbars
	are compared.
	*/
      widget_class_code = mvsGetClassCode(w_info->widget);
      if (widget_class_code == mvsXmTextWidgetClass ||
	  widget_class_code == mvsXmListWidgetClass) {
	widget_class_code = 
	  mvsGetClassCode(w_info->parent->widget);
	if (widget_class_code == 
	    mvsXmScrolledWindowWidgetClass)
	  w_info = w_info->parent;
      }
      AutoInvokePerformMonitor();
      AutoCompareVisual(w_info);
      break;
    case STOREVISUAL:
      if (AutoTrace)
	AutoTraceStoreAction(Command->WidgetName,
			     Command->WidgetComponent,
			     Command->Identifier,
			     command_num);
      widget = GetWidgetID(Command->WidgetName);
      /* take picture of a widget not a gadget */
      while (XmIsGadget(widget))
	widget = XtParent(widget);
      w_info = mvsWidgetToWidgetInfo(widget);
      /* 
	If widget is a ScrolledText or a ScrolledList
	widget, compare the ScrolledWindow parent and
	not the List or Text widget. This way, scrollbars
	are compared.
	*/
      widget_class_code = mvsGetClassCode(w_info->widget);
      if (widget_class_code == mvsXmTextWidgetClass ||
	  widget_class_code == mvsXmListWidgetClass) {
	widget_class_code = 
	  mvsGetClassCode(w_info->parent->widget);
	if (widget_class_code == 
	    mvsXmScrolledWindowWidgetClass)
	  w_info = w_info->parent;
      }
      AutoInvokePerformMonitor();
      AutoStoreVisual(w_info, Command->Identifier);
      break;
    case COMPARESTOREDVISUAL:
      if (AutoTrace)
	AutoTraceCompareStoreAction(Command->WidgetName,
				    Command->WidgetComponent,
				    Command->Identifier,
				    command_num);
      widget = GetWidgetID(Command->WidgetName);
      /* take picture of a widget not a gadget */
      while (XmIsGadget(widget))
	widget = XtParent(widget);
      w_info = mvsWidgetToWidgetInfo(widget);
      /* 
	If widget is a ScrolledText or a ScrolledList
	widget, compare the ScrolledWindow parent and
	not the List or Text widget. This way, scrollbars
	are compared.
	*/
      widget_class_code = mvsGetClassCode(w_info->widget);
      if (widget_class_code == mvsXmTextWidgetClass ||
	  widget_class_code == mvsXmListWidgetClass) {
	widget_class_code = 
	  mvsGetClassCode(w_info->parent->widget);
	if (widget_class_code == 
	    mvsXmScrolledWindowWidgetClass)
	  w_info = w_info->parent;
      }
      AutoInvokePerformMonitor();
      AutoCompareTempVisual(w_info, Command->Identifier);
      break;
    case SYSTEM:
      if (AutoTrace)
	AutoTraceSystemAction(Command->SystemCommand,
			      Command->SystemArgs,
			      Command->SystemNumArgs,
			      command_num);
      sysargs[0] = Command->SystemCommand;
      for (i = 1; i <= Command->SystemNumArgs; i++) 
	sysargs[i] =  Command->SystemArgs[i - 1];
      i++;
      sysargs[i] = (char *) 0;
      AutoInvokePerformMonitor();
      AutoSystem(Command->SystemCommand, sysargs);
      for (i = 0;  i < Command->SystemNumArgs; i++)
	sysargs[i] = (char *) 0;
      break;
    case DRAG:
      if (AutoTrace)
	AutoTraceDragAction(Command->Mask,
			    Command->NumMask,
			    Command->WidgetName,
			    Command->WidgetComponent,
			    Command->Location,
			    Command->ButtonNumber,
			    0, 0, 0, DRAG,
			    command_num);
      mask = GetMask(Command->NumMask, Command->Mask);
      widget = GetWidgetID(Command->WidgetName);
      location = Command->Location;
      switch (location) {
      case AUTOMIN:
      case -1:	Location = 0;
	break;
      case AUTOMAX:	Location = 
	AutoGetMaxListPosition(widget);
	break;
      default:	Location = location;
	break;
      }
      if (Command->ButtonNumber != 0)
	XButton = GetButton(Command->ButtonNumber);
      else 
	XButton = Button1;
      Component = GetComponent(Command->WidgetComponent);
      AutoInvokePerformMonitor();
      AutoDrag(widget, Component, Location, mask, XButton);
      break;
    case DRAGSLIDERVAL:
    case DRAGRELATIVE:
    case DRAGABS:
      mask = GetMask(Command->NumMask, Command->Mask);
      location = Command->Location;
      if (Command->ButtonNumber != 0)
	XButton = GetButton(Command->ButtonNumber);
      else 
	XButton = Button1;
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      switch (Command->CommandName) {
      case DRAGSLIDERVAL:	
	if (AutoTrace)
	  AutoTraceDragAction(
			      Command->Mask,
			      Command->NumMask,
			      NULL, 0, location,
			      Command->ButtonNumber,
			      0, 0, XCoord, 
			      DRAGSLIDERVAL,
			      command_num);
	AutoInvokePerformMonitor();
	AutoDragSliderValue(location, XCoord,
			    YCoord, mask,
			    XButton);
	break;
      case DRAGRELATIVE:
	if (AutoTrace)
	  AutoTraceDragAction(
			      Command->Mask,
			      Command->NumMask,
			      NULL, 0, location,
			      Command->ButtonNumber,
			      XCoord, YCoord,
			      0, DRAGRELATIVE,
			      command_num);
	AutoInvokePerformMonitor();
	AutoDragRelative(location, XCoord,
			 YCoord, mask,
			 XButton);
	break;
      case DRAGABS:
	if (AutoTrace)
	  AutoTraceDragAction(
			      Command->Mask,
			      Command->NumMask,
			      NULL, 0, 0,
			      Command->ButtonNumber,
			      XCoord, YCoord,
			      0, DRAGABS,
			      command_num);
	AutoInvokePerformMonitor();
	AutoDragAbs(XCoord, YCoord, mask, 
		    XButton);
	break;
      }
      break;
    case INPUTSTRING:
      if (AutoTrace)
	AutoTraceInputStringAction(Command->WidgetName,
				   command_num);
      inputstring = Command->WidgetName;
      AutoInvokePerformMonitor();
      AutoInputString(inputstring);
      break;
  case WINDOWRAISE:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWRAISE,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      if (widget != NULL) {
        window = XtWindowOfObject(GetTopShell(widget));
        AutoInvokePerformMonitor();
#ifndef AUTOMATION
        AutoWmRaise(window);
#else
        if (! AutoWmRaise(window))
            AutoMessage(_AutoMessages[SCRMSG41]);
#endif
      }
      break;
    case WINDOWMAXIMIZE:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWMAXIMIZE,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
   window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmMaximize(window);
#else
      if (! AutoWmMaximize(window))
        AutoMessage(_AutoMessages[SCRMSG22]);
#endif
      AutoUpdateWindow();
      break;
    case WINDOWNORMALIZE:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWNORMALIZE,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      /* call AutoWmMaximize() - same operation to 
         normalize a window */
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmNormalize(window);
#else
    if (! AutoWmMaximize(window))
        AutoMessage(_AutoMessages[SCRMSG23]);
#endif
      AutoUpdateWindow();
      break;
    case WINDOWICONIFY:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWICONIFY,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmIconify(window);
#else
    if (! AutoWmIconify(window))
        AutoMessage(_AutoMessages[SCRMSG24]);
#endif
      break;
    case WINDOWDEICONIFY:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWDEICONIFY,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmDeIconify(window);
#else
      if (! AutoWmDeIconify(window))
        AutoMessage(_AutoMessages[SCRMSG25]);
#endif
      break;
    case WINDOWFOCUS:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWFOCUS,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
#ifndef AUTOMATION
      AutoWmFocus (window);
#else
      if (! AutoWmFocus(window))
        AutoMessage(_AutoMessages[SCRMSG26]);
#endif
      AutoInvokePerformMonitor();
      AutoSetFocus(GetTopShell(widget), oUserDefined);
      break;
  case WINDOWCLICK:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWCLICK,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmFocus (window);
#else
      if (! AutoWmFocus(window))
        AutoMessage(_AutoMessages[SCRMSG27]);
#endif
      break;
    case WINDOWMENUPOST:
      if (AutoTrace)
        AutoTracePostAction(Command->WidgetName,
                            Command->Key,
                            WINDOWMENUPOST,
                            command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
     if (Command->Key == -1)
        keyboard = False;
      else
        keyboard = True;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmMenuPost(window, keyboard);
#else
      if (! AutoWmMenuPost(window, keyboard))
        AutoMessage(_AutoMessages[SCRMSG28]);
#endif
      break;
    case WINDOWMENUUNPOST:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              WINDOWMENUUNPOST,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmMenuUnpost(window);
#else
      if (! AutoWmMenuUnpost(window))
        AutoMessage(_AutoMessages[SCRMSG29]);
#endif
      break;
    case ICONMENUPOST:
      if (AutoTrace)
        AutoTracePostAction(Command->WidgetName,
                            Command->Key,
                            ICONMENUPOST,
                            command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      if (Command->Key == -1)
        keyboard = False;
      else
        keyboard = True;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmIconMenuPost (window,keyboard);
#else
      if (! AutoWmIconMenuPost(window, keyboard))
        AutoMessage(_AutoMessages[SCRMSG30]);
#endif
      break;
    case ICONMENUUNPOST:
      if (AutoTrace)
        AutoTraceWindowAction(Command->WidgetName,
                              ICONMENUUNPOST,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmIconMenuUnpost(window);
#else
      if (! AutoWmIconMenuUnpost(window))
        AutoMessage(_AutoMessages[SCRMSG31]);
#endif
      break;
    case WINDOWMENUSELECT:
      if (AutoTrace)
        AutoTraceSelectAction(Command->WidgetName,
                              Command->KeyOrButtonCount,
                              Command->SystemCommand,
                              Command->Key,
         WINDOWMENUSELECT,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      item_number = Command->KeyOrButtonCount;
      item_name = Command->SystemCommand;
      if (Command->Key == -1)
        keyboard = False;
      else
        keyboard = True;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmMenuSelect(window, item_number, 
                             item_name, keyboard);
#else
      if (! AutoWmMenuSelect(window, item_number, 
                             item_name, keyboard))
        AutoMessage(_AutoMessages[SCRMSG32]);
#endif
      break;
    case ICONMENUSELECT:
      if (AutoTrace)
        AutoTraceSelectAction(Command->WidgetName,
     Command->KeyOrButtonCount,
                              Command->SystemCommand,
                              Command->Key,
                              ICONMENUSELECT,
                              command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      item_number = Command->KeyOrButtonCount;
      item_name = Command->SystemCommand;
      if (Command->Key == -1)
        keyboard = False;
      else
        keyboard = True;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmIconMenuSelect(window, item_number,
                           item_name, keyboard);
#else

      if (! AutoWmIconMenuSelect(window, item_number,
                                 item_name, keyboard))
        AutoMessage(_AutoMessages[SCRMSG33]);
#endif
      break;
   case WINDOWRESIZE:
      if (AutoTrace)
        AutoTraceMoveAction(Command->WidgetName,
                            Command->XCoord,
                            Command->YCoord,
                            Command->Gravity,
                            WINDOWRESIZE,
                            command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      gravity = Command->Gravity - NORTHWEST;
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmResize(window, gravity, XCoord, YCoord);
        
#else
      if (! AutoWmResize(window, gravity, XCoord, YCoord))
        AutoMessage(_AutoMessages[SCRMSG34]);
        
#endif
      AutoUpdateWindow();
    break;
    case WINDOWMOVE:
      if (AutoTrace)
        AutoTraceMoveAction(Command->WidgetName,
                            Command->XCoord,
                            Command->YCoord,
                            0,
                            WINDOWMOVE,
                            command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmMove(window, XCoord, YCoord);
#else
      if (! AutoWmMove(window, XCoord, YCoord))
        AutoMessage(_AutoMessages[SCRMSG35]);
#endif
      AutoUpdateWindow();
      break;
    
    case WINDOWICHECK:
    widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      group = Command->Gravity - SYSTEMMENU;
      name = Command->SystemCommand;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
     AutoWmItemCheck(window, group, name);
#else
      if (! AutoWmItemCheck(window, group, name))
        AutoMessage(_AutoMessages[SCRMSG36]);
#endif
      break;
    case ICONMOVE:
      if (AutoTrace)
        AutoTraceMoveAction(Command->WidgetName,
                            Command->XCoord,
                            Command->YCoord,
                            0,
                            ICONMOVE,
                            command_num);
      widget = GetWidgetID(Command->WidgetName);
      window = XtWindowOfObject(GetTopShell(widget));
      XCoord = Command->XCoord;
      YCoord = Command->YCoord;
      AutoInvokePerformMonitor();
#ifndef AUTOMATION
      AutoWmIconMove(window, XCoord, YCoord);
#else
      if (! AutoWmIconMove(window, XCoord, YCoord))
        AutoMessage(_AutoMessages[SCRMSG37]);
#endif
      break;



    case WAIT:
      if (AutoTrace)
	AutoTraceWaitAction(Command->ButtonNumber,
			    command_num);
      AutoInvokePerformMonitor();
      AutoWait(Command->ButtonNumber);
      break;
    case MANUAL:
      if (AutoTrace)
	AutoTraceControlAction(MANUAL, command_num);
      AutoInvokePerformMonitor();
      AutoManualMode = True;
      AutoGoManual();
      break;
    case CONTINUE:
      if (AutoTrace)
	AutoTraceControlAction(CONTINUE, command_num);
      NextScreen = True;
      widget = GetWidgetID("InstructionBox");
      if (widget != NULL) {
	window = XtWindowOfObject(GetTopShell(widget));
	AutoWmRaise(window);
      }

      /*
       * This is a hack for CR 9650. Basically, we need to have enought time
       * for the raise to happen before we locate the pointer to the OK
       * button, so we force a delay
       */

      AutoDelayCycle (1);
      Component = GetComponent(OKBUTTON);
      AutoInvokePerformMonitor();
      AutoContinueOrEnd(widget, Component);
      break;
    case AUTOPERFORMMONITOR:
      if (AutoTrace)
	AutoPerformanceAction(Command->OnOffSwitch, command_num);
      PerformanceOnOff = Command->OnOffSwitch;
      n = 0;
      switch (PerformanceOnOff)  {
      case ON:
	MonitorOn = True;
	break;
      case OFF:
	MonitorOn = False;
	break;
      }
      break;
    case END:
      if (AutoTrace)
	AutoTraceControlAction(END, command_num);
      NextScreen = True;
      widget = GetWidgetID("InstructionBox");
      if (widget != NULL) {
	window = XtWindowOfObject(GetTopShell(widget));
	AutoWmRaise(window);
      }
      /*
       * This is a hack for CR 9650. Basically, we need to have enought time
       * for the raise to happen before we locate the pointer to the Exit
       * button, so we force a delay
       */

      AutoDelayCycle (1);
      Component = GetComponent(CANCELBUTTON);
      AutoContinueOrEnd(widget, Component);
      break;
    default:
	AutoMessage(_AutoMessages[WARNMSG3]);
      break;
    }
    
    if (Command->next == NULL || NextScreen) {
      MoreInput = False;
      if (Command->next != NULL)
	Command = Command->next;
    }
    else 
      Command = Command->next;
    
    AutoProcessAllEvents();
    
    if (AutoDelay)
      AutoDelayCycle(AutoDelayCycles);
    
    command_num++;
  }
}


static int 
GetMask(int num_mask, int mask_array[5])
{
  
  int 	i;
  int 	mask;
  
  if (num_mask == 0)
    return(NoModifierKeys);
  else {
    mask = GetXMask(mask_array[0]);
    for (i = 1 ; i < num_mask; i++)
      mask = mask | GetXMask(mask_array[i]);
    return(mask);
  }
  
}


static int
GetXMask(
	 int mask )
{
  
  int XMask;
  
  switch (mask) {
  case SHIFTMASK:
    XMask = ShiftMask;
    break;
  case LOCKMASK:
    XMask = LockMask;
    break;
  case CTRLMASK:
    XMask = ControlMask;
    break;
  case METAMASK:
    XMask = Mod1Mask;
    break;
  case ALTMASK:
    XMask = Mod1Mask;
    break;
  case MOD1MASK:
    XMask = Mod1Mask;
    break;
  case MOD2MASK:
    XMask = Mod2Mask;
    break;
  case MOD3MASK:
    XMask = Mod3Mask;
    break;
  case MOD4MASK:
    XMask = Mod4Mask;
    break;
  case MOD5MASK:
    XMask = Mod5Mask;
    break;
  default:
    AutoMessage(_AutoMessages[WARNMSG4]);
    break;
  }
  
  return(XMask);
  
}


static int
GetButton(
	  int button )
{
  
  int XButton;
  
  switch (button) {
  case BTN1:
    XButton = Button1;
    break;
  case BTN2:
    XButton = Button2;
    break;
  case BTN3:
    XButton = Button3;
    break;
  case BTN4:
    XButton = Button4;
    break;
  case BTN5:
    XButton = Button5;
    break;
  default:
    AutoWarning("GetButton: Unknown Button");
    break;
  }
  
  return(XButton);
  
}


static int
GetComponent(
	     int component )
{
  
  int Component;
  
  if (component == -1)
    return(oUserDefined);
  
  switch (component) {
  case SEPARATOR:
    Component = oSeparator;
    break;
  case LABEL:
    Component = oLabel;
    break;
  case LISTITEM:
    Component = oListItem;
    break;
  case FILELISTITEM:
    Component = oFileListItem;
    break;
  case DIRLISTITEM:
    Component = oDirListItem;
    break;
  case SELECTIONLISTITEM:
    Component = oSelectionListItem;
    break;
  case TEXTCHAR:
    Component = oTextChar;
    break;
  case SLIDER:
    Component = oSlider;
    break;
  case COMBOBOXDROPDOWNBUTTON:
    Component = oComboBoxDropDownButton;
    break;
  case OUTLINEBTN:
    Component = oOutlineBtn;
    break;
  case INCREMENTBTN:
    Component = oIncrementBtn;
    break;
  case DECREMENTBTN:
    Component = oDecrementBtn;
    break;
  case PAGESCROLLER:
    Component = oPageScroller;
    break;
  case MAJORTABSCROLLERNEXT:
    Component = oMajorTabScrollerNext;
    break;
  case MAJORTABSCROLLERPREV:
    Component = oMajorTabScrollerPrev;
    break;
  case MINORTABSCROLLERNEXT:
    Component = oMinorTabScrollerNext;
    break;
  case MINORTABSCROLLERPREV:
    Component = oMinorTabScrollerPrev;
    break;
  case UPORLEFTARROW:
    Component = oUpOrLeftArrow;
    break;
  case DOWNORRIGHTARROW:
    Component = oDownOrRightArrow;
    break;
  case UPORLEFTSLIDERAREA:
    Component = oUpOrLeftSliderArea;
    break;
  case DOWNORRIGHTSLIDERAREA:
    Component = oDownOrRightSliderArea;
    break;
  case SCROLLBAR:
    Component = oScrollBar;
    break;
  case HORIZSCROLLBAR:
    Component = oHorizScrollBar;
    break;
  case HORIZSBLEFTARROW:
    Component = oHorizScrollBarUpOrLeftArrow;
    break;
  case HORIZSBRIGHTARROW:
    Component = oHorizScrollBarDownOrRightArrow;
    break;
  case HORIZSBLEFTSLIDERAREA:
    Component = oHorizScrollBarUpOrLeftSliderArea;
    break;
  case HORIZSBRIGHTSLIDERAREA:
    Component = oHorizScrollBarDownOrRightSliderArea;
    break;
  case HORIZSBSLIDER:
    Component = oHorizScrollBarSlider;
    break;
  case VERTSCROLLBAR:
    Component = oVertScrollBar;
    break;
  case VERTSBUPARROW:
    Component = oVertScrollBarUpOrLeftArrow;
    break;
  case VERTSBDOWNARROW:
    Component = oVertScrollBarDownOrRightArrow;
    break;
  case VERTSBUPSLIDERAREA:
    Component = oVertScrollBarUpOrLeftSliderArea;
    break;
  case VERTSBDOWNSLIDERAREA:
    Component = oVertScrollBarDownOrRightSliderArea;
    break;
  case VERTSBSLIDER:
    Component = oVertScrollBarSlider;
    break;
  case MENUBAR:
    Component = oMenuBar;
    break;
  case SASH:
    Component = oSash;
    break;
  case CLOSEBUTTON:
    Component = oCloseButton;
    break;
  case HELPBUTTON:
    Component = oHelpButton;
    break;
  case OKBUTTON:
    Component = oOkButton;
    break;
  case FILTERBUTTON:
    Component = oFilterButton;
    break;
  case CANCELBUTTON:
    Component = oCancelButton;
    break;
  case APPLYBUTTON:
    Component = oApplyButton;
    break;
  case MESSAGELABEL:
    Component = oMessageLabel;
    break;
  case SYMBOLLABEL:
    Component = oSymbolLabel;
    break;
  case SELECTIONTEXT:
    Component = oSelectionText;
    break;
  case SELECTIONTEXTCHAR:
    Component = oSelectionTextChar;
    break;
  case SELECTIONLABEL:
    Component = oSelectionLabel;
    break;
  case SELECTIONLIST:
    Component = oSelectionList1;
    break;
  case SELECTIONLISTLABEL:
    Component = oSelectionListLabel;
    break;
  case FILELIST:
    Component = oFileList1;
    break;
  case FILELISTLABEL:
    Component = oFileListLabel;
    break;
  case DIRLIST:
    Component = oDirList1;
    break;
  case DIRLISTLABEL:
    Component = oDirListLabel;
    break;
  case DIRFILTERLABEL:
    Component = oDirFilterLabel;
    break;
  case DIRFILTERTEXT:
    Component = oDirFilterText;
    break;
  case DIRFILTERTEXTCHAR:
    Component = oDirFilterTextChar;
    break;
  case FILESELECTIONTEXT:
    Component = oFileSelectionText;
    break;
  case FILESELECTIONTEXTCHAR:
    Component = oFileSelectionTextChar;
    break;
  case FILESELECTIONLABEL:
    Component = oFileSelectionLabel;
    break;
  case FILEFILTERTEXT:
    Component = oFileFilterText;
    break;
  case FILEFILTERTEXTCHAR:
    Component = oFileFilterTextChar;
    break;
  case FILEFILTERLABEL:
    Component = oFileFilterLabel;
    break;
  case WORKAREA:
    Component = oWorkArea;
    break;
  case COMMANDWINDOW:
    Component = oCommandWindow;
    break;
  case MESSAGEWINDOW:
    Component = oMessageWindow;
    break;
  case WORKWINDOW:
    Component = oWorkWindow;
    break;
  case SCALE:
    Component = oScale;
    break;
  case SCALELABEL:
    Component = oScaleLabel;
    break;
  case OPTIONBUTTON:
    Component = oOptionButton;
    break;
  case SCROLLEDWINDOW:
    Component = oScrolledWindow;
    break;
  case SCROLLBARSLIDER:
    Component = oScrollBarSlider;
    break;
  case SCROLLBARUPORLEFTARROW:
    Component = oScrollBarUpOrLeftArrow;
    break;
  case SCROLLBARDOWNORRIGHTARROW:
    Component = oScrollBarDownOrRightArrow;
    break;
  case SCROLLBARUPORLEFTSLIDERAREA:
    Component = oScrollBarUpOrLeftSliderArea;
    break;
  case SCROLLBARDOWNORRIGHTSLIDERAREA:
    Component = oScrollBarDownOrRightSliderArea;
    break;
  case FLHSBUPORLEFTARROW:
    Component = oFileListHorizScrollBarUpOrLeftArrow;
    break;
  case FLHSBDOWNORRIGHTARROW:
    Component = oFileListHorizScrollBarDownOrRightArrow;
    break;
  case FLHSBUPORLEFTSLIDERAREA:
    Component = oFileListHorizScrollBarUpOrLeftSliderArea;
    break;
  case FLHSBDOWNORRIGHTSLIDERAREA:
    Component = oFileListHorizScrollBarDownOrRightSliderArea;
    break;
  case FLHSBSLIDER:
    Component = oFileListHorizScrollBarSlider;
    break;
  case FLVSBUPORLEFTARROW:
    Component = oFileListVertScrollBarUpOrLeftArrow;
    break;
  case FLVSBDOWNORRIGHTARROW:
    Component = oFileListVertScrollBarDownOrRightArrow;
    break;
  case FLVSBUPORLEFTSLIDERAREA:
    Component = oFileListVertScrollBarUpOrLeftSliderArea;
    break;
  case FLVSBDOWNORRIGHTSLIDERAREA:
    Component = oFileListVertScrollBarDownOrRightSliderArea;
    break;
  case FLVSBSLIDER:
    Component = oFileListVertScrollBarSlider;
    break;
  case DLHSBUPORLEFTARROW:
    Component = oDirListHorizScrollBarUpOrLeftArrow;
    break;
  case DLHSBDOWNORRIGHTARROW:
    Component = oDirListHorizScrollBarDownOrRightArrow;
    break;
  case DLHSBUPORLEFTSLIDERAREA:
    Component = oDirListHorizScrollBarUpOrLeftSliderArea;
    break;
  case DLHSBDOWNORRIGHTSLIDERAREA:
    Component = oDirListHorizScrollBarDownOrRightSliderArea;
    break;
  case DLHSBSLIDER:
    Component = oDirListHorizScrollBarSlider;
    break;
  case DLVSBUPORLEFTARROW:
    Component = oDirListVertScrollBarUpOrLeftArrow;
    break;
  case DLVSBDOWNORRIGHTARROW:
    Component = oDirListVertScrollBarDownOrRightArrow;
    break;
  case DLVSBUPORLEFTSLIDERAREA:
    Component = oDirListVertScrollBarUpOrLeftSliderArea;
    break;
  case DLVSBDOWNORRIGHTSLIDERAREA:
    Component = oDirListVertScrollBarDownOrRightSliderArea;
    break;
  case DLVSBSLIDER:
    Component = oDirListVertScrollBarSlider;
    break;
  case SLHSBUPORLEFTARROW:
    Component = oSelectionListHorizScrollBarUpOrLeftArrow;
    break;
  case SLHSBDOWNORRIGHTARROW:
    Component = oSelectionListHorizScrollBarDownOrRightArrow;
    break;
  case SLHSBUPORLEFTSLIDERAREA:
    Component = oSelectionListHorizScrollBarUpOrLeftSliderArea;
    break;
  case SLHSBDOWNORRIGHTSLIDERAREA:
    Component = oSelectionListHorizScrollBarDownOrRightSliderArea;
    break;
  case SLHSBSLIDER:
    Component = oSelectionListHorizScrollBarSlider;
    break;
  case SLVSBUPORLEFTARROW:
    Component = oSelectionListVertScrollBarUpOrLeftArrow;
    break;
  case SLVSBDOWNORRIGHTARROW:
    Component = oSelectionListVertScrollBarDownOrRightArrow;
    break;
  case SLVSBUPORLEFTSLIDERAREA:
    Component = oSelectionListVertScrollBarUpOrLeftSliderArea;
    break;
  case SLVSBDOWNORRIGHTSLIDERAREA:
    Component = oSelectionListVertScrollBarDownOrRightSliderArea;
    break;
  case SLVSBSLIDER:
    Component = oSelectionListVertScrollBarSlider;
    break;
  case TEAROFFBUTTON:
    Component = oTearOffButton;
    break;
  default:
    AutoWarning("GetComponent: Unknown component");
    break;
  }
  
  return(Component);
  
}


static Widget
GetWidgetID(
	    char *widget_string )
{
  
  Widget 	widget;
  char		matchstring[25];
  char		err_string[100];
  
  if (strcmp(widget_string, "Shell1") == 0)
    return(Shell1);
  sprintf(matchstring, "*%s", widget_string);
  widget = XtNameToWidget(Shell1, matchstring);
  if (widget == NULL) {
    sprintf(err_string, 
	    _AutoMessages[SCRMSG21], matchstring);
    AutoError(err_string);
    return(NULL);
  }
  else
    return(widget);
  
}


static Widget
GetTopShell(
	    Widget widget )
{
  
  Widget topWidget;
  
  topWidget = widget;
  while (!XtIsShell(topWidget))
    topWidget = XtParent(topWidget);
  return(topWidget);
}


static void
CheckKey(
	 int key )
{
  /* 
    Check to see if the key to be clicked, pressed, or released is
    one that needs to be sent to the clipWindow of the ScrolledWindow.
    If it is set SendToClipWindow.
    */
  switch (key) {
  case PAGERIGHT:
  case PAGELEFT:
  case PAGEUP:
  case PAGEDOWN:
  case BEGINLINE:
  case ENDLINE:
  case BEGINDATA:
  case ENDDATA:
  case HELP:		SendToClipWindow = HaveScrolledWindow;
    break;
  default:		break;
    
  }
}


static void
AutoUpdateWindow( void )
{
  
  xisProcessObjects();
  xisUpdateObjectAttributes();
  
}
