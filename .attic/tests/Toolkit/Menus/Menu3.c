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
static char rcsid[] = "$XConsortium: Menu3.c /main/10 1995/07/13 18:44:44 drk $"
#endif
#endif

#include <testlib.h>

#include <Xm/XmP.h>
#include <Xm/RowColumnP.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Vendor.h>

#include "armPixft.h"
#include "disarmPixft.h"
#include "arrowPixft.h"

Widget mainw, menubar, separator;
Widget rc0, rc1;
Widget special, special2;
Widget frame1;
Widget button1, button2, button3, button4, button5, button6;
Widget popup[5];
Widget submenu;
Widget submenu1, submenu2, submenu3, submenu4, submenu5;
Widget title, separator2, popupmenu, help;
Widget child[20];

Cursor sailboat, heart, man, pirate, shuttle, arrow;
Boolean	CallbacksEnabled = False;
int WhichButton = 2;
Pixmap arm = None;
Pixmap disarm = None;
Pixmap arrowPix = None;
unsigned long 	fg, bg;

void CreatePopup();
void CreateMenubar();
void CreatePixmaps();

static void DArmCB(Widget w, XtPointer client_data, XtPointer call_data);
static void ArmCB(Widget w, XtPointer client_data, XtPointer call_data);

static XtTranslations translation1;

void _XmRegisterHelpEvent(String event)
{
  static String	action1 = ": Help()";
  static String	action2 = ": help()";
  String bufr;

  if ((event == NULL) || (event[0] == '\0'))
    {
      fprintf(stdout,
	      "_XmRegisterHelpEvent(): NULL event string specified\n");
      return;
    }

  bufr = XtMalloc(strlen(event) + strlen(action1) + 1);
  strcpy(bufr, event);
  strcat(bufr, action1);
  translation1 = XtParseTranslationTable(bufr);
  XtFree(bufr);
}

void _XmBindHelpAction(Widget widget, void (*func)(), XtPointer client_data,
		       Boolean recursive)
{
  int i;
  if (widget == NULL || (!XtIsWidget(widget) && !XmIsGadget(widget)))
    {
      return;
    }

  if (XmIsGadget(widget))
    {
      XtAddCallback(widget, XmNhelpCallback, func, client_data);
      return;
    }

  if (!XtIsShell(widget))
    {
      XtOverrideTranslations(widget, translation1);
      XtAddCallback(widget, XmNhelpCallback, func, client_data);
    }

  if (recursive)
    {
      if (XtIsComposite(widget))
	{
	  XmManagerWidget mw = (XmManagerWidget)widget;

	  for(i=0 ; i<mw->composite.num_children ; i++)
	    {
	      _XmBindHelpAction(mw->composite.children[i], func,
				client_data, recursive);
	    }
	}
      for(i=0 ; i<widget->core.num_popups ; i++)
	{
	  _XmBindHelpAction(widget->core.popup_list[i], func, client_data, 
			    recursive);
	}
    }
}

void PrintName(Widget w,
XtPointer d1,
XtPointer d2)
{
  fprintf(stdout, "Widget %s was activated\n",
	  XrmQuarkToString(w->core.xrm_name));
}

void ChangeCursor(Widget rc, XtPointer client_data,
		  XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  Cursor cursor;
	if (call_data->set) {
  cursor = (Cursor) client_data;
  XmSetMenuCursor(display, cursor);
  
  if (XmGetMenuCursor(display) != cursor)
    {
      fprintf(stdout,"Error: problem occurred while changing menu cursor\n");
    }
	}
}

Help(Widget w)
{
  fprintf(stdout, "Help requested for widget %s\n", 
	  XrmQuarkToString(w->core.xrm_name));
}

void PostIt(Widget w, XtPointer popup, XEvent *event, Boolean *c)
{
  Widget lpopup = (Widget) popup ;
  Boolean popupEnabled;
  Arg args[1];
  int n;

  if (lpopup == NULL)
    {
      return;
    }

  if ((event->type == ButtonPress) && (event->xbutton.button != WhichButton))
    {
      return;
    }

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, &popupEnabled); n++;
  XtGetValues(lpopup, args, n);
  
  if (popupEnabled)
    {
      XmMenuPosition(lpopup, (XButtonPressedEvent *)event);
      XtManageChild(lpopup);
    }
}

CreateMenuCursors()
{
  sailboat = XCreateFontCursor(display, XC_sailboat);
  heart = XCreateFontCursor(display, XC_heart);
  man = XCreateFontCursor(display, XC_man);
  pirate =  XCreateFontCursor(display, XC_pirate);
  shuttle = XCreateFontCursor(display, XC_shuttle);
  arrow = XCreateFontCursor(display, XC_arrow);
}

void DeleteMenuPane(Widget w,
XtPointer d1,
XtPointer d2)
{
  XtDestroyWidget(submenu4);
  submenu4 = NULL;
  XtSetSensitive(w, False);
}

void DeleteHelpItem(Widget w,
XtPointer d1,
XtPointer d2)
{
  XtDestroyWidget(help);
  help = NULL;
  XtSetSensitive(w, False);
}

void DeleteMenuItem(Widget w,
XtPointer d1,
XtPointer d2)
{
  XtDestroyWidget(special);
  child[0] = NULL;
  XtSetSensitive(w, False);
}


/* ARGSUSED */

void TogglePopupAccelerator(Widget w,
XtPointer d1,
XtPointer d2)
{
  static Boolean	set = True;
  static XmString	accel = NULL;
  Arg			args[2];
  int			n;

  n = 0;
  if (set == False)
    {
      XtSetArg(args[n], XmNmenuAccelerator, "Ctrl<Key>g"); n++;
      set = True;
    }
  else
    {
      XtSetArg(args[0], XmNmenuAccelerator, "Ctrl<Key>h"); n++;
      set = False;
    }
  XtSetValues(popupmenu, args, n);
}

void MoveSubmenu(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[10];
  int n;
   
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, NULL); n++;
  XtSetValues(child[15], args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, submenu1); n++;
  XtSetValues(child[17], args, n);
  XtSetSensitive(w, False);
}

void DisconnectMenupane(Widget w)
{
  Arg args[1];
  int n;

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, NULL); n++;
  XtSetValues(child[16], args, n);
  XtSetSensitive(w, False);
}

void ChangeItemSensitivity(Widget w,
XtPointer d1,
XtPointer d2)
{
  Boolean sensitive;
  Arg args[1];
  int n;

  n = 0;
  XtSetArg(args[n], XmNsensitive, &sensitive); n++;
  XtGetValues(special2, args, n);

  if (sensitive)
    {
      XtSetSensitive(special2, False);
    }
  else
    {
      XtSetSensitive(special2, True);
    }
}

void main(unsigned int argc, char *argv[])
{
  Arg args[4];
  int n;

  CommonTestInit(argc, argv);
  
  /*
   * Create menu cursors
   */

  CreateMenuCursors();

  /*
   * XmMainWindow: mainw
   */

  mainw = XmCreateMainWindow(Shell1, "mainw", NULL, 0);
  XtManageChild(mainw);

  /*
   * Create label pixmaps
   */

  CreatePixmaps();

  /*
   * Create the menubar
   */

  CreateMenubar();

  /*
   * XmRowColumn: rc0
   */

  rc0 = XmCreateRowColumn(mainw, "rc0", (ArgList) NULL, 0);
  XtManageChild(rc0);

  n = 0;
  XtSetArg(args[n], XmNworkWindow, rc0); n++;
  XtSetArg(args[n], XmNmenuBar, menubar); n++;
  XtSetValues(mainw, args, n);

  /*
   * XmFrame: frame1
   */

  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  frame1 = XmCreateFrame(rc0, "frame1", args, n);
  XtManageChild(frame1);

  /*
   * XmRowColumn: rc1
   */

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 3); n++;
  rc1 = XmCreateRowColumn(frame1, "rc1", (ArgList) args, n);
  XtManageChild(rc1);

  /*
   * Set attributes for all XmPushButton widgets
   */

  n = 0;
  XtSetArg(args[n], XmNmarginTop, 3); n++;
  XtSetArg(args[n], XmNmarginBottom, 3); n++;
  XtSetArg(args[n], XmNmarginLeft, 6); n++;
  XtSetArg(args[n], XmNmarginRight, 6); n++;

  /*
   * XmPushButtons: button[123456]
   */

  button1 = XmCreatePushButtonGadget(rc1, "button1", args, n);
  XtAddCallback(button1, XmNactivateCallback, DeleteMenuPane, NULL);
  XtManageChild(button1);

  button2 = XmCreatePushButtonGadget(rc1, "button2", args, n);
  XtAddCallback(button2, XmNactivateCallback, DeleteMenuItem, NULL);
  XtManageChild(button2);

  button3 = XmCreatePushButtonGadget(rc1, "button3", args, n);
  XtAddCallback(button3, XmNactivateCallback, DeleteHelpItem, NULL);
  XtManageChild(button3);

  button4 = XmCreatePushButtonGadget(rc1, "button4", args, n);
  XtAddCallback(button4, XmNactivateCallback, TogglePopupAccelerator, NULL);
  XtManageChild(button4);

  button5 = XmCreatePushButtonGadget(rc1, "button5", args, n);
  XtAddCallback(button5, XmNactivateCallback, MoveSubmenu, NULL);
  XtManageChild(button5);

  button6 = XmCreatePushButtonGadget(rc1, "button6", args, n);
  XtAddCallback(button6, XmNactivateCallback, ChangeItemSensitivity, NULL);
  XtManageChild(button6);


  /*
   * Create the popup menu
   */

  CreatePopup();


  XmAddTabGroup(rc1);

  /*
   * Realize the widget hierarchy
   */

  XtRealizeWidget(Shell1);

  /*
   * Bind F8 to be the help button
   */

/*    _XmRegisterHelpEvent("<Key>F8");
    _XmBindHelpAction(Shell1, Help, NULL, True); */

  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();

/*
  if (submenu4 != NULL)
    {
      XtDestroyWidget(child[8]);
      XtDestroyWidget(child[7]);
      XtDestroyWidget(child[6]);
      XtDestroyWidget(submenu4);
    }

  if (child[0] != NULL)
    XtDestroyWidget(child[0]);
  XtDestroyWidget(child[1]);
  XtDestroyWidget(child[2]);
  XtDestroyWidget(child[18]);
  XtDestroyWidget(child[19]);
  XtDestroyWidget(child[3]);
  XtDestroyWidget(child[4]);
  XtDestroyWidget(child[5]);
  XtDestroyWidget(child[9]);
  XtDestroyWidget(child[10]);
  XtDestroyWidget(child[11]);
  XtDestroyWidget(child[12]);
  XtDestroyWidget(child[13]);
  XtDestroyWidget(child[14]);
  XtDestroyWidget(submenu2);
  XtDestroyWidget(submenu5);
  XtDestroyWidget(submenu1);
  XtDestroyWidget(child[15]);
  XtDestroyWidget(child[16]);
  XtDestroyWidget(child[17]);
  if (help != NULL)
    XtDestroyWidget(help);
  XtDestroyWidget(menubar);
  XtDestroyWidget(popup[4]);
  XtDestroyWidget(popup[3]);
  XtDestroyWidget(popup[2]);
  XtDestroyWidget(popup[1]);
  XtDestroyWidget(popup[0]);
  XtDestroyWidget(popupmenu);
  XtDestroyWidget(button1);
  XtDestroyWidget(button2);
  XtDestroyWidget(button3);
  XtDestroyWidget(button4);
  XtDestroyWidget(button5);
  XtDestroyWidget(button6);
  XtDestroyWidget(rc1);
  XtDestroyWidget(frame1);
  XtDestroyWidget(rc0);
*/
  XtDestroyWidget(mainw);
  
  CommonPause();
  XtAppMainLoop(app_context);
}

void CreatePixmaps()
{
  unsigned int	depth;
  Arg args[2];
  int n;
  Drawable drawable;
    
  n = 0;
  XtSetArg(args[n], XmNforeground, &fg); n++;
  XtSetArg(args[n], XmNbackground, &bg); n++;
  XtGetValues(mainw, args, n);

  depth = DefaultDepthOfScreen(Shell1->core.screen);
  drawable = RootWindowOfScreen(XtScreen(Shell1));
       
  arm = XCreatePixmapFromBitmapData(display,
				    drawable,
				    armPix_bits,
				    armPix_width,
				    armPix_height,
				    fg, bg,
				    depth);

  arrowPix = XCreatePixmapFromBitmapData(display, 
					 drawable,
					 arrowPix_bits,
					 arrowPix_width,
					 arrowPix_height,
					 fg, bg,
					 depth);
}

void CreatePopup()
{
  Arg args[1];
  int n;

  /*
   * XmPopupMenu: popup
   */

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
  popupmenu = XmCreatePopupMenu(rc1, "popupmenu", args, n);

  XtAddEventHandler(rc1, ButtonPressMask, False, PostIt, popup);

  /*
   * XmLabelGadget: popup[0]
   */

  popup[0] = XmCreateLabelGadget(popupmenu, "popup0", NULL, 0);

  /*
   * XmSeparatorGadget: popup[1]
   */

  popup[1] = XmCreateSeparatorGadget(popupmenu, "popup1", NULL, 0);

  /*
   * XmPushButtonGadget: popup[234]
   */

  popup[2] = XmCreatePushButtonGadget(popupmenu, "option1e", NULL, 0);
  XtAddCallback(popup[2], XmNactivateCallback, PrintName, NULL);

  popup[3] = XmCreatePushButtonGadget(popupmenu, "option2e", NULL, 0);
  XtAddCallback(popup[3], XmNactivateCallback, PrintName, NULL);

  popup[4] = XmCreatePushButtonGadget(popupmenu, "option3e", NULL, 0);
  XtAddCallback(popup[4], XmNactivateCallback, PrintName, NULL);
  XtManageChildren(&(popup[0]), 5);

  /*
   * Add the accelerator
   */

  TogglePopupAccelerator(NULL, NULL, NULL);
}

void CreateMenubar()
{
  Arg  args[3];
  int n;

  /*
   * XmMenuBar: menubar
   */

  menubar = XmCreateMenuBar(mainw, "menubar", NULL, 0);
  XtManageChild(menubar);

  /*
   * XmCascadeButton: help
   */

  help = XmCreateCascadeButton(menubar, "help", NULL, 0);
  XtAddCallback(help, XmNactivateCallback, PrintName, NULL);
  XtManageChild(help);

  n = 0; 
  XtSetArg(args[n], XmNmenuHelpWidget, help); n++;
  XtSetValues(menubar, args, n);

  /*
   * XmPulldownMenu: submenu1
   */

  submenu1 = XmCreatePulldownMenu(menubar, "submenu1", NULL, 0);
    
  /*
   * XmPushButtonGadget: child[012]
   */

  child[0] = 
    special = XmCreatePushButtonGadget(submenu1, "option1a", NULL, 0);
  XtAddCallback(child[0], XmNactivateCallback, PrintName, NULL);

  child[1] = XmCreatePushButtonGadget(submenu1,"option2a", NULL, 0);
  XtAddCallback(child[1], XmNactivateCallback, PrintName, NULL);

  child[2] = XmCreatePushButtonGadget(submenu1,"option3a", NULL, 0);
  XtAddCallback(child[2], XmNactivateCallback, PrintName, NULL);
  XtAddCallback(child[2], XmNarmCallback, ArmCB, NULL);
  XtAddCallback(child[2], XmNdisarmCallback, DArmCB, NULL);

  XtManageChildren(child, 3);

  /*
   * XmPulldownMenu: submenu2
   */

  submenu2 = XmCreatePulldownMenu(submenu1, "submenu2", NULL, 0);

  /*
   * XmPushButtonGadget: child[345]
   */

  child[3] = XmCreatePushButtonGadget(submenu2, "option1b", NULL, 0);
  XtAddCallback(child[3], XmNactivateCallback, PrintName, NULL);
    
  child[4] = XmCreatePushButtonGadget(submenu2, "option2b", NULL, 0);
  XtAddCallback(child[4], XmNactivateCallback, PrintName, NULL);

  child[5] = XmCreatePushButtonGadget(submenu2, "option3b", NULL, 0);
  XtAddCallback(child[5], XmNactivateCallback, PrintName, NULL);

  XtManageChildren(&(child[3]), 3);

  /*
   * XmPulldownMenu: submenu4
   */

  submenu4 = XmCreatePulldownMenu(submenu1, "submenu4", args, 0);

  /*
   * XmPushButtonGadget: child[678]
   */

  child[6] = XmCreatePushButtonGadget(submenu4, "option1d", NULL, 0);
  XtAddCallback(child[6], XmNactivateCallback, PrintName, NULL);

  child[7] = XmCreatePushButtonGadget(submenu4, "option2d", NULL, 0);
  XtAddCallback(child[7], XmNactivateCallback, PrintName, NULL);

  n = 0;
  XtSetArg(args[n], XmNarmPixmap, arm); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  child[8] = XmCreatePushButtonGadget(submenu4, "option3d", args, n);
  XtAddCallback(child[8], XmNactivateCallback, PrintName, NULL);

  XtManageChildren(&(child[6]), 3);

  /*
   * XmPulldownMenu: submenu5
   */

  n = 0;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  submenu5 = XmCreatePulldownMenu(menubar, "submenu5", args, n);

  /*
   * XmPushButtonGadget: child[9-10-11-12-13-14]
   */

  child[9] = XmCreateToggleButtonGadget(submenu5,"sailboat", NULL, 0);
  XtAddCallback(child[9], XmNvalueChangedCallback, ChangeCursor, (XtPointer) sailboat);

  child[10] = 
    special2 = XmCreateToggleButtonGadget(submenu5,"heart", NULL, 0);
  XtAddCallback(child[10], XmNvalueChangedCallback, ChangeCursor, (XtPointer) heart);

  child[11] = XmCreateToggleButtonGadget(submenu5,"man", NULL, 0);
  XtAddCallback(child[11], XmNvalueChangedCallback, ChangeCursor, (XtPointer) man);

  child[12] = XmCreateToggleButtonGadget(submenu5,"pirate", NULL, 0);
  XtAddCallback(child[12], XmNvalueChangedCallback, ChangeCursor, (XtPointer) pirate);

  child[13] = XmCreateToggleButtonGadget(submenu5,"shuttle", NULL, 0);
  XtAddCallback(child[13], XmNvalueChangedCallback, ChangeCursor, (XtPointer) shuttle);
  
  child[14] = XmCreateToggleButtonGadget(submenu5,"arrow", NULL, 0);
  XtAddCallback(child[14], XmNvalueChangedCallback, ChangeCursor, (XtPointer) arrow);

  XtManageChildren(&(child[9]), 6);

  /*
   * XmCascadeButton: child[15-16-17] 
   */

  n = 0; 
  XtSetArg(args[n], XmNsubMenuId, submenu1); n++;
  child[15] = XmCreateCascadeButton(menubar, "pdbtn1", args, n);
  
  n = 0; 
  XtSetArg(args[n], XmNsubMenuId, submenu5); n++;
  child[16] = XmCreateCascadeButton(menubar, "pdbtn2", args, n);

  child[17] = XmCreateCascadeButton(menubar, "pdbtn3", NULL, 0);
  XtManageChildren(&(child[15]), 3);

  /*
   * XmCascadeButtonGadget: child[18]
   */

  n = 0; 
  XtSetArg(args[n], XmNsubMenuId, submenu2); n++;
  child[18] = XmCreateCascadeButtonGadget(submenu1, "more", args, n);
  XtManageChild(child[18]);

  n = 0; 
  XtSetArg(args[n], XmNcascadePixmap, arrowPix); n++;
  XtSetValues(child[18], args, 1);

  /*
   * XmCascadeButtonGadget: child[19]
   */

  n = 0; 
  XtSetArg(args[n], XmNsubMenuId, submenu4); n++;
  child[19] = XmCreateCascadeButtonGadget(submenu1, "more2", args, n);
  XtManageChild(child[19]);
}

static void ArmCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("Arm Callback called\n");
}

static void DArmCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("Disarm Callback called\n");
}
