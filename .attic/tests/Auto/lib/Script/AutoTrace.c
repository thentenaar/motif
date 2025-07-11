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
static char rcsid[] = "$XConsortium: AutoTrace.c /main/9 1996/10/07 15:01:12 drk $"
#endif
#endif
#include <xislib.h>
#include <mvslib.h>
#include <testlib.h>
#include "AutoToken.h"
#include "Automation.h"
#include "AutoMessages.h"


static void AutoTraceInstance(int instance);
static void AutoTraceMask(int mask[5], int num_mask);
static void AutoTraceButton(int button);
static void AutoTraceComponent(int component);



void
AutoTraceButtonAction(
        int button,
        int mask[5],
	int num_mask,
        int multi_click,
        int func_type,
        int line )
{
    
    switch (func_type) {
	case PRESSMB:		printf("TRACE:(%d) PressMB", line);
				break;
	case RELEASEMB:		printf("TRACE:(%d) ReleaseMB", line);
				break;
	case CLICKMB:		printf("TRACE:(%d) ClickMB", line);
				break;
    }

    printf(" ");

    AutoTraceMask(mask, num_mask);

    AutoTraceButton(button);

    printf(" ");

    if (multi_click > 0)
	printf("%d\n", multi_click);
    else
	 printf("\n");

}


void
AutoTraceKeyAction(
        int key,
        int mask[5],
	int num_mask,
        int func_type,
        int line )
{

    switch (func_type) {
	case CLICKKEY:		printf("TRACE:(%d) ClickKey", line);
				break;
	case PRESSKEY:		printf("TRACE:(%d) PressKey", line);
				break;
	case RELEASEKEY:	printf("TRACE:(%d) ReleaseKey", line);
				break;
	default:		AutoError("Bad function in AutoTraceKeyAction");
				break;
    }

    printf(" ");

    AutoTraceMask(mask, num_mask);

    switch (key) {
	case BACKSPACE:
		printf("KBackSpace");
		break;
	case INSERT:
		printf("KInsert");
		break;
	case DELETE:
		printf("KDelete");
		break;
	case COPY:
		printf("KCopy");
		break;
	case CUT:
		printf("KCut");
		break;
	case PASTE:
		printf("KPaste");
		break;
	case ADDMODE:
		printf("KAddMode");
		break;
	case PRIMARYPASTE:
		printf("KPrimaryPaste");
		break;
	case QUICKPASTE:
		printf("KQuickPaste");
		break;
	case PAGEUP:
		printf("KPageUp");
		break;
	case PAGEDOWN:
		printf("KPageDown");
		break;
	case ENDLINE:
		printf("KEndLine");
		break;
	case BEGINLINE:
		printf("KBeginLine");
		break;
	case ACTIVATE:
		printf("KActivate");
		break;
	case MENUBAR:
		printf("KMenuBar");
		break;
	case CLEAR:
		printf("KClear");
		break;
	case CANCEL:
		printf("KCancel");
		break;
	case HELP:
		printf("KHelp");
		break;
	case MENU:
		printf("KMenu");
		break;
	case SELECT:
		printf("KSelect");
		break;
	case UNDO:
		printf("KUndo");
		break;
	case UPK:
		printf("KUp");
		break;
	case RIGHTK:
		printf("KRight");
		break;
	case DOWNK:
		printf("KDown");
		break;
	case LEFTK:
		printf("KLeft");
		break;
	case BACKTAB:
		printf("KBackTab");
		break;
	case BEGINDATA:
		printf("KBeginData");
		break;
	case DESELECTALL:
		printf("KDeselectAll");
		break;
	case ENDDATA:
		printf("KEndData");
		break;
	case ENTER:
		printf("KEnter");
		break;
	case ESCAPE:
		printf("KEscape");
		break;
	case EXTEND:
		printf("KExtend");
		break;
	case NEXTFIELD:
		printf("KNextField");
		break;
	case NEXTMENU:
		printf("KNextMenu");
		break;
	case PAGERIGHT:
		printf("KPageRight");
		break;
	case PAGELEFT:
		printf("KPageLeft");
		break;
	case PREVFIELD:
		printf("KPrevField");
		break;
	case PREVMENU:
		printf("KPrevMenu");
		break;
	case PRIMARYCOPY:
		printf("KPrimaryCopy");
		break;
	case PRIMARYCUT:
		printf("KPrimaryCut");
		break;
	case QUICKCOPY:
		printf("KQuickCopy");
		break;
	case QUICKCUT:
		printf("KQuickCut");
		break;
	case QUICKEXTEND:
		printf("KQuickExtend");
		break;
	case RESELECT:
		printf("KReselect");
		break;
	case RESTORE:
		printf("KRestore");
		break;
	case SELECTALL:
		printf("KSelectAll");
		break;
	case SPACE:
		printf("KSpace");
		break;
	case TAB:
		printf("KTab");
		break;
	case KEYA:
		printf("Ka");
		break;
	case KEYB:
		printf("Kb");
		break;
	case KEYC:
		printf("Kc");
		break;
	case KEYD:
		printf("Kd");
		break;
	case KEYE:
		printf("Ke");
		break;
	case KEYF:
		printf("Kf");
		break;
	case KEYG:
		printf("Kg");
		break;
	case KEYH:
		printf("Kh");
		break;
	case KEYI:
		printf("Ki");
		break;
	case KEYJ:
		printf("Kj");
		break;
	case KEYK:
		printf("Kk");
		break;
	case KEYL:
		printf("Kl");
		break;
	case KEYM:
		printf("Km");
		break;
	case KEYN:
		printf("Kn");
		break;
	case KEYO:
		printf("Ko");
		break;
	case KEYP:
		printf("Kp");
		break;
	case KEYQ:
		printf("Kq");
		break;
	case KEYR:
		printf("Kr");
		break;
	case KEYS:
		printf("Ks");
		break;
	case KEYT:
		printf("Kt");
		break;
	case KEYU:
		printf("Ku");
		break;
	case KEYV:
		printf("Kv");
		break;
	case KEYW:
		printf("Kw");
		break;
	case KEYX:
		printf("Kx");
		break;
	case KEYY:
		printf("Ky");
		break;
	case KEYZ:
		printf("Kz");
		break;
	case KEYSLASH:
		printf("KSlash");
		break;
	case KEYBACKSLASH:
		printf("KBackSlash");
		break;
	case KEYBRACERIGHT:
		printf("KBraceRight");
		break;
	case KEYBRACELEFT:
		printf("KBraceLeft");
		break;
	case KEYBRACKETRIGHT:
		printf("KBracketRight");
		break;
	case KEYBRACKETLEFT:
		printf("KBracketLeft");
		break;
	case KEYRETURN:
		printf("KReturn");
		break;
	case KEYEXCLAM:
		printf("KExclamation");
		break;
	case KEYAT:
		printf("KeyAt");
		break;
	case KEYNUMBERSIGN:
		printf("KNumberSign");
		break;
	case KEYDOLLAR:
		printf("KDollar");
		break;
	case KEYPERCENT:
		printf("KPercent");
		break;
	case KEYCARET:
		printf("KCaret");
		break;
	case KEYAMPERSAND:
		printf("KAmpersand");
		break;
	case KEYASTERISK:
		printf("KAsterisk");
		break;
	case KEYPARENLEFT:
		printf("KParenLeft");
		break;
	case KEYPARENRIGHT:
		printf("KParenRight");
		break;
	case KEY1:
		printf("K1");
		break;
	case KEY2:
		printf("K2");
		break;
	case KEY3:
		printf("K3");
		break;
	case KEY4:
		printf("K4");
		break;
	case KEY5:
		printf("K5");
		break;
	case KEY6:
		printf("K6");
		break;
	case KEY7:
		printf("K7");
		break;
	case KEY8:
		printf("K8");
		break;
	case KEY9:
		printf("K9");
		break;
	case KEY0:
		printf("K0");
		break;
	default:
		break;
    }

    printf("\n");
}


void
AutoTraceLocateAction(
        char *widget,
        int component,
        int instance,
        int x,
        int y,
        int func_type,
        int line )
{

    switch (func_type) {
	case LOCATEPOINTERABS:	printf("TRACE:(%d) LocatePointerXYAbs %d %d\n", 
				       line, x, y);
				return;
	case LOCATEPOINTERREL:	printf("TRACE:(%d) LocatePointerXYRel %d %d ", 
				       line, x, y);
				break;
	case LOCATEPOINTER:	printf("TRACE:(%d) LocatePointer ", line);
				break;
   }

   printf("%s ", widget);

   AutoTraceComponent(component);

   printf(" ");

   AutoTraceInstance(instance);

   printf("\n");

}


void
AutoPerformanceAction(
		      int on_off_switch, 
		      int line)
{

     char msg_string[75];



     switch (on_off_switch)  {
     case ON:
       printf("TRACE** : (%d) AutoPerformMonitor On\n", line);
       /* COMMENTED OUT UNTIL BUG IN REPORT GEN IS FIXED
       sprintf(msg_string, _AutoMessages[TRACEMSG148], line);
       AutoMessage(msg_string);
        */
       break;
     case OFF:
       printf("TRACE** : (%d) AutoPerformMonitor Off\n", line);
       /* COMMENTED OUT UNTIL BUG IN REPORT GEN IS FIXED
       sprintf(msg_string, _AutoMessages[TRACEMSG149], line);
       AutoMessage(msg_string);
        */
       break;
     }
   }
   


void
AutoTraceCompareAction(
        char *widget,
        int component,
        int line )
{

    printf("TRACE:(%d) CompareVisual %s ", line, widget);

    AutoTraceComponent(component);

    printf("\n");

}


void
AutoTraceStoreAction (
	char *widget,
        int component,
	char *identifier,
        int line )
{

    printf("TRACE:(%d) StoreVisual %s, %s ", line, widget, identifier);

    AutoTraceComponent(component);

    printf("\n");

}


void
AutoTraceCompareStoreAction (
	char *widget,
        int component,
	char *identifier,
        int line )
{

    printf("TRACE:(%d) CompareStoreVisual %s, %s ", line, widget, identifier);

    AutoTraceComponent(component);

    printf("\n");

}



void
AutoTraceSystemAction(
        char *command,
        char *args[CommandSize],
        int num_args,
        int line )
{
    int i;

    printf("TRACE:(%d) System %s ", line, command);

    for(i = 0; i < num_args ; i++)
	printf("%s ", args[i]);

    printf("\n");

}


void
AutoTraceInputStringAction(
        char *string,
        int line )
{

    printf("TRACE:(%d) InputString %s\n", line, string);

}


void
AutoTraceWindowAction(
        char *widget,
        int func_type,
        int line )
{

    switch (func_type) {
	case WINDOWRAISE:	printf("TRACE:(%d) WindowRaise ", line);
				break;
	case WINDOWMAXIMIZE:	printf("TRACE:(%d) WindowMaximize ", line);
				break;
	case WINDOWICONIFY:	printf("TRACE:(%d) WindowIconify ", line);
				break;
	case WINDOWDEICONIFY:	printf("TRACE:(%d) WindowDeiconify ", line);
				break;
	case WINDOWFOCUS:	printf("TRACE:(%d) WindowFocus ", line);
				break;
	case WINDOWCLICK:	printf("TRACE:(%d) WindowClick ", line);
				break;
	case WINDOWMENUUNPOST:	printf("TRACE:(%d) WindowMenuUnpost ", line);
				break;
	case ICONMENUUNPOST:	printf("TRACE:(%d) IconMenuUnpost ", line);
				break;
    }

    printf("%s\n", widget);

}


void
AutoTracePostAction(
        char *widget,
	int keyboard,
        int func_type,
        int line )
{

    switch (func_type) {
	case WINDOWMENUPOST:	printf("TRACE:(%d) WindowMenuPost ", line);
				break;
	case ICONMENUPOST:	printf("TRACE:(%d) IconMenuPost ", line);
				break;
    }

    printf("%s", widget);

    if (keyboard != -1)
	printf(" Keyboard\n");
    else 
	printf("\n");

}


void
AutoTraceControlAction(
        int func_type,
        int line )
{

    switch(func_type) {
	case MANUAL:	printf("TRACE:(%d) Manual\n", line);
			break;
	case CONTINUE:	printf("TRACE:(%d) Continue\n", line);
			break;
	case END:	printf("TRACE:(%d) Exit\n", line);
			break;
    }
}


void
AutoTraceDragAction(
        int mask[5],
        int num_mask,
        char *widget,
        int component,
        int dest,
        int button,
        int x,
        int y,
        int value,
        int func_type,
        int line )
{

    switch (func_type) {
	case DRAG:	
			printf("TRACE:(%d) DragComponent %s ", line, widget);
			AutoTraceComponent(component); printf(" ");
			AutoTraceInstance(dest); printf(" ");
			AutoTraceMask(mask, num_mask);
			AutoTraceButton(button);
			printf("\n");
			break;
	case DRAGSLIDERVAL:
			printf("TRACE:(%d) DragSliderValue ", line);
			AutoTraceInstance(dest); printf(" ");
			AutoTraceInstance(value); printf(" ");
			AutoTraceMask(mask, num_mask);
			AutoTraceButton(button);
			printf("\n");
			break;
	case DRAGRELATIVE:
			printf("TRACE:(%d) DragXYRelative ", line);
			AutoTraceInstance(dest);
			if (dest == ANY)
				printf(" %d %d ", x, y);
			else
				printf(" %d ", x);
			AutoTraceMask(mask, num_mask);
			AutoTraceButton(button);
			printf("\n");
			break;
	case DRAGABS:
			printf("TRACE:(%d) DragXYAbs %d %d ", line, x, y);
			AutoTraceMask(mask, num_mask);
			AutoTraceButton(button);
			printf("\n");
			break;
	default:	break;
    }
}


void
AutoTraceMoveAction(
        char *widget,
        int x,
        int y,
        int gravity,
        int func_type,
        int line )
{

    switch (func_type) {
	case ICONMOVE:
			printf("TRACE:(%d) IconMove %s %d %d\n", line, widget,
								 x, y);
			return;
	case WINDOWMOVE:
			printf("TRACE:(%d) WindowMove %s %d %d\n", line, widget,
								   x, y);
			return;
	case WINDOWRESIZE:
			printf("TRACE:(%d) WindowResize %s ", line, widget);
			break;
	default:	break;
    }

    switch (gravity) {
	case NORTHWEST:
			printf("NorthWest ");
			break;
	case NORTH:
			printf("North ");
			break;
	case NORTHEAST:
			printf("NorthEast ");
			break;
	case WEST:
			printf("West ");
			break;
	case EAST:
			printf("East ");
			break;
	case SOUTHWEST:
			printf("SouthWest ");
			break;
	case SOUTH:
			printf("South ");
			break;
	case SOUTHEAST:
			printf("SouthEast ");
			break;
	default:	break;
    }

    if (y == -1)
	printf("%d\n", x);
    else
	printf("%d %d\n", x, y);
}


void
AutoTraceSelectAction(
        char *widget,
        int item_num,
        char *item_name,
	int keyboard,
        int func_type,
        int line )
{

    switch (func_type) {
	case WINDOWMENUSELECT:
			printf("TRACE:(%d) WindowMenuSelect %s", line, widget);
			break;
	case ICONMENUSELECT:
			printf("TRACE:(%d) IconMenuSelect %s", line, widget);
			break;
    }

    if (item_num == -1)
	printf(" %s", item_name);
    else
	printf(" %d", item_num);

    if (keyboard != -1)
	printf(" Keyboard\n");
    else
	printf("\n");

}


void
AutoTraceWaitAction(
        int count,
	int line)
{

    printf("TRACE:(%d) Wait %d\n", line, count);

}


void AutoPerformanceOn()
{

    printf("AUTOMESSAGE** : Performance measuring turned on\n");
    /* COMMENTED OUT UNTIL BUG IN REPORT GEN IS FIXED
    AutoMessage(_AutoMessages[INFOMSG2]);
     */

}


void AutoPerformanceOff()
{
    printf("AUTOMESSAGE** : Performance measuring turned off\n");
    /* COMMENTED OUT UNTIL BUG IN REPORT GEN IS FIXED
    AutoMessage(_AutoMessages[INFOMSG3]);
     */

}


static void
AutoTraceInstance(
        int instance )
{

    switch (instance) {
	case OFF: 
		printf("Off");
		break;
	case RIGHT: 
		printf("Right");
		break;
	case LEFT:
		printf("Left");
		break;
	case UP:
		printf("Up");
		break;
	case DOWN:
		printf("Down");
		break;
	case AUTOMIN:
		printf("Min");
		break;
	case AUTOMAX:
		printf("Max");
		break;
	case -1:  
		break;
	default:  
		printf("%d", instance);
		break;
    }
}



static void
AutoTraceMask(
        int mask[5],
        int num_mask )
{
    int i;

    for (i = 0 ; i < num_mask ; i++) {
	    switch (mask[i]) {
		case SHIFTMASK:
			printf("<Shift>");
			break;
		case LOCKMASK:
			printf("<Lock>");
			break;
		case CTRLMASK:
			printf("<Ctrl>");
			break;
		case METAMASK:
			printf("<Mod1>");
			break;
		case ALTMASK:
			printf("<Alt>");
			break;
		case MOD1MASK:
			printf("<Mod1>");
			break;
		case MOD2MASK:
			printf("<Mod2>");
			break;
		case MOD3MASK:
			printf("<Mod3>");
			break;
		case MOD4MASK:
			printf("<Mod4>");
			break;
		case MOD5MASK:
			printf("<Mod5>");
			break;
		default:
			break;
	    }
    printf(" ");
    }
}


static void
AutoTraceButton(
        int button )
{
    switch (button) {
	case BTN1:
		printf("Btn1");
		break;
	case BTN2:
		printf("Btn2");
		break;
	case BTN3:
		printf("Btn3");
		break;
	case BTN4:
		printf("Btn4");
		break;
	case BTN5:
		printf("Btn5");
		break;
	default:
		break;
    }
}


static void
AutoTraceComponent(
        int component )
{

    switch (component) {
	case SEPARATOR:
		printf("RSeparator");
		break;
	case LABEL:
		printf("RLabel");
		break;
	case LISTITEM:
		printf("RListItem");
		break;
	case FILELISTITEM:
		printf("RFileListItem");
		break;
	case DIRLISTITEM:
		printf("RDirListItem");
		break;
	case DIRFILTERLABEL:
		printf("RDirFilterLabel");
		break;
	case DIRFILTERTEXT:
		printf("RDirFilterText");
		break;
	case DIRFILTERTEXTCHAR:
		printf("RDirFilterTextChar");
		break;
	case SELECTIONLISTITEM:
		printf("RSelectionListItem");
		break;
	case TEXTCHAR:
		printf("RTextChar");
		break;
	case SLIDER:
		printf("RSlider");
		break;
	case COMBOBOXDROPDOWNBUTTON:
		printf("RComboBoxDropDownButton");
		break;
	case OUTLINEBTN:
		printf("ROutlineBtn");
		break;
	case INCREMENTBTN:
		printf("RIncrementBtn");
		break;
	case DECREMENTBTN:
		printf("RDecrementBtn");
		break;
	case PAGESCROLLER:
		printf("RPageScroller");
		break;
	case MAJORTABSCROLLERNEXT:
		printf("RMajorTabScrollerNext");
		break;
	case MAJORTABSCROLLERPREV:
		printf("RMajorTabScrollerPrev");
		break;
	case MINORTABSCROLLERNEXT:
		printf("RMinorTabScrollerNext");
		break;
	case MINORTABSCROLLERPREV:
		printf("RMinorTabScrollerPrev");
		break;
	case UPORLEFTARROW:
		printf("RUpOrLeftArrow");
		break;
	case DOWNORRIGHTARROW:
		printf("RDownOrRightArrow");
		break;
	case UPORLEFTSLIDERAREA:
		printf("RUpOrLeftSliderArea");
		break;
	case DOWNORRIGHTSLIDERAREA:
		printf("RDownOrRightSliderArea");
		break;
	case SCROLLBAR:
		printf("RScrollBar");
		break;
	case HORIZSCROLLBAR:
		printf("RHorizScrollBar");
		break;
	case HORIZSBLEFTARROW:
		printf("RHorizScrollBarUpOrLeftArrow");
		break;
	case HORIZSBRIGHTARROW:
		printf("RHorizScrollBarDownOrRightArrow");
		break;
	case HORIZSBLEFTSLIDERAREA:
		printf("RHorizScrollBarUpOrLeftSliderArea");
		break;
	case HORIZSBRIGHTSLIDERAREA:
		printf("RHorizScrollBarDownOrRightSliderArea");
		break;
	case HORIZSBSLIDER:
		printf("RHorizScrollBarSlider");
		break;
	case VERTSCROLLBAR:
		printf("RVertScrollBar");
		break;
	case VERTSBUPARROW:
		printf("RVertScrollBarUpOrLeftArrow");
		break;
	case VERTSBDOWNARROW:
		printf("RVertScrollBarDownOrRightArrow");
		break;
	case VERTSBUPSLIDERAREA:
		printf("RVertScrollBarUpOrLeftSliderArea");
		break;
	case VERTSBDOWNSLIDERAREA:
		printf("RVertScrollBarDownOrRightSliderArea");
		break;
	case VERTSBSLIDER:
		printf("RVertScrollBarSlider");
		break;
	case MENUBAR:
		printf("RMenuBar");
		break;
	case SASH:
		printf("RSash");
		break;
	case CLOSEBUTTON:
		printf("RCloseButton");
		break;
	case HELPBUTTON:
		printf("RHelpButton");
		break;
	case OKBUTTON:
		printf("ROkButton");
		break;
	case FILTERBUTTON:
		printf("RFilterButton");
		break;
	case CANCELBUTTON:
		printf("RCancelButton");
		break;
	case APPLYBUTTON:
		printf("RApplyButton");
		break;
	case MESSAGELABEL:
		printf("RMessageLabel");
		break;
	case SYMBOLLABEL:
		printf("RSymbolLabel");
		break;
	case SELECTIONTEXT:
		printf("RSelectionText");
		break;
	case SELECTIONTEXTCHAR:
		printf("RSelectionTextChar");
		break;
	case SELECTIONLABEL:
		printf("RSelectionLabel");
		break;
	case SELECTIONLIST:
		printf("RSelectionList1");
		break;
	case SELECTIONLISTLABEL:
		printf("RSelectionListLabel");
		break;
	case FILELIST:
		printf("RFileList1");
		break;
	case FILELISTLABEL:
		printf("RFileListLabel");
		break;
	case DIRLIST:
		printf("RDirList1");
		break;
	case DIRLISTLABEL:
		printf("RDirListLabel");
		break;
	case FILESELECTIONTEXT:
		printf("RFileSelectionText");
		break;
	case FILESELECTIONTEXTCHAR:
		printf("RFileSelectionTextChar");
		break;
	case FILESELECTIONLABEL:
		printf("RFileSelectionLabel");
		break;
	case FILEFILTERTEXT:
		printf("RFileFilterText");
		break;
	case FILEFILTERTEXTCHAR:
		printf("RFileFilterTextChar");
		break;
	case FILEFILTERLABEL:
		printf("RFileFilterLabel");
		break;
	case WORKAREA:
		printf("RWorkArea");
		break;
	case COMMANDWINDOW:
		printf("RCommandWindow");
		break;
	case MESSAGEWINDOW:
		printf("RMessageWindow");
		break;
	case WORKWINDOW:
		printf("RWorkWindow");
		break;
	case SCALE:
		printf("RScale");
		break;
	case SCALELABEL:
		printf("RScaleLabel");
		break;
	case OPTIONBUTTON:
		printf("ROptionButton");
		break;
	case SCROLLEDWINDOW:
		printf("RScrolledWindow");
		break;
	case SCROLLBARSLIDER:
		printf("RScrollBarSlider");
		break;
	case SCROLLBARUPORLEFTARROW:
		printf("RScrollBarUpOrLeftArrow");
		break;
	case SCROLLBARDOWNORRIGHTARROW:
		printf("RScrollBarDownOrRightArrow");
		break;
	case SCROLLBARUPORLEFTSLIDERAREA:
		printf("RScrollBarUpOrLeftSliderArea");
		break;
	case SCROLLBARDOWNORRIGHTSLIDERAREA:
		printf("RScrollBarDownOrRightSliderArea");
		break;
	case FLHSBUPORLEFTARROW:
		printf("RFileListHorizScrollBarUpOrLeftArrow");
		break;
	case FLHSBDOWNORRIGHTARROW:
		printf("RFileListHorizScrollBarDownOrRightArrow");
		break;
	case FLHSBUPORLEFTSLIDERAREA:
		printf("RFileListHorizScrollBarUpOrLeftSliderArea");
		break;
	case FLHSBDOWNORRIGHTSLIDERAREA:
		printf("RFileListHorizScrollBarDownOrRightSliderArea");
		break;
	case FLHSBSLIDER:
		printf("RFileListHorizScrollBarSlider");
		break;
	case FLVSBUPORLEFTARROW:
		printf("RFileListVertScrollBarUpOrLeftArrow");
		break;
	case FLVSBDOWNORRIGHTARROW:
		printf("RFileListVertScrollBarDownOrRightArrow");
		break;
	case FLVSBUPORLEFTSLIDERAREA:
		printf("RFileListVertScrollBarUpOrLeftSliderArea");
		break;
	case FLVSBDOWNORRIGHTSLIDERAREA:
		printf("RFileListVertScrollBarDownOrRightSliderArea");
		break;
	case FLVSBSLIDER:
		printf("RFileListVertScrollBarSlider");
		break;
	case DLHSBUPORLEFTARROW:
		printf("RDirListHorizScrollBarUpOrLeftArrow");
		break;
	case DLHSBDOWNORRIGHTARROW:
		printf("RDirListHorizScrollBarDownOrRightArrow");
		break;
	case DLHSBUPORLEFTSLIDERAREA:
		printf("RDirListHorizScrollBarUpOrLeftSliderArea");
		break;
	case DLHSBDOWNORRIGHTSLIDERAREA:
		printf("RDirListHorizScrollBarDownOrRightSliderArea");
		break;
	case DLHSBSLIDER:
		printf("RDirListHorizScrollBarSlider");
		break;
	case DLVSBUPORLEFTARROW:
		printf("RDirListVertScrollBarUpOrLeftArrow");
		break;
	case DLVSBDOWNORRIGHTARROW:
		printf("RDirListVertScrollBarDownOrRightArrow");
		break;
	case DLVSBUPORLEFTSLIDERAREA:
		printf("RDirListVertScrollBarUpOrLeftSliderArea");
		break;
	case DLVSBDOWNORRIGHTSLIDERAREA:
		printf("RDirListVertScrollBarDownOrRightSliderArea");
		break;
	case DLVSBSLIDER:
		printf("RDirListVertScrollBarSlider");
		break;
	case SLHSBUPORLEFTARROW:
		printf("RSelectionListHorizScrollBarUpOrLeftArrow");
		break;
	case SLHSBDOWNORRIGHTARROW:
		printf("RSelectionListHorizScrollBarDownOrRightArrow");
		break;
	case SLHSBUPORLEFTSLIDERAREA:
		printf("RSelectionListHorizScrollBarUpOrLeftSliderArea");
		break;
	case SLHSBDOWNORRIGHTSLIDERAREA:
		printf("RSelectionListHorizScrollBarDownOrRightSliderArea");
		break;
	case SLHSBSLIDER:
		printf("RSelectionListHorizScrollBarSlider");
		break;
	case SLVSBUPORLEFTARROW:
		printf("RSelectionListVertScrollBarUpOrLeftArrow");
		break;
	case SLVSBDOWNORRIGHTARROW:
		printf("RSelectionListVertScrollBarDownOrRightArrow");
		break;
	case SLVSBUPORLEFTSLIDERAREA:
		printf("RSelectionListVertScrollBarUpOrLeftSliderArea");
		break;
	case SLVSBDOWNORRIGHTSLIDERAREA:
		printf("RSelectionListVertScrollBarDownOrRightSliderArea");
		break;
	case SLVSBSLIDER:
		printf("RSelectionListVertScrollBarSlider");
		break;
	case -1: 
		break;
	default:
		break;
    }
}
