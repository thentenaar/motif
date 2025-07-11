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
static char rcsid[] = "$XConsortium: MenuM1.c /main/7 1995/07/13 18:36:21 drk $"
#endif
#endif

#include <testlib.h>

#include <X11/cursorfont.h>

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
unsigned long 	fg, bg;

static void CreateMenubar();

static void ChangeCursor(Widget tb, XtPointer client_data, XtPointer call_data)
{
  Cursor cursor;
  cursor = (Cursor) client_data;

  if (XmToggleButtonGadgetGetState(tb) == XmSET) {
      XmSetMenuCursor(display, cursor);
  
      if (XmGetMenuCursor(display) != cursor)
	  fprintf(stdout,"Error: problem occurred while changing menu cursor\n");
  }
}

static void CreateMenuCursors()
{
  sailboat = XCreateFontCursor(display, XC_sailboat);
  heart = XCreateFontCursor(display, XC_heart);
  man = XCreateFontCursor(display, XC_man);
  pirate =  XCreateFontCursor(display, XC_pirate);
  shuttle = XCreateFontCursor(display, XC_shuttle);
  arrow = XCreateFontCursor(display, XC_arrow);
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
  XtManageChild(button1);

  button2 = XmCreatePushButtonGadget(rc1, "button2", args, n);
  XtManageChild(button2);

  button3 = XmCreatePushButtonGadget(rc1, "button3", args, n);
  XtManageChild(button3);

  button4 = XmCreatePushButtonGadget(rc1, "button4", args, n);
  XtManageChild(button4);

  button5 = XmCreatePushButtonGadget(rc1, "button5", args, n);
  XtManageChild(button5);

  button6 = XmCreatePushButtonGadget(rc1, "button6", args, n);
  XtManageChild(button6);

  XmAddTabGroup(rc1);

  /*
   * Realize the widget hierarchy
   */

  XtRealizeWidget(Shell1);

  CommonPause();
  XtDestroyWidget(mainw);
  
  CommonPause();
  XtAppMainLoop(app_context);
}

static void CreateMenubar()
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
  child[1] = XmCreatePushButtonGadget(submenu1,"option2a", NULL, 0);
  child[2] = XmCreatePushButtonGadget(submenu1,"option3a", NULL, 0);
  XtManageChildren(child, 3);

  /*
   * XmPulldownMenu: submenu2
   */

  submenu2 = XmCreatePulldownMenu(submenu1, "submenu2", NULL, 0);

  /*
   * XmPushButtonGadget: child[345]
   */

  child[3] = XmCreatePushButtonGadget(submenu2, "option1b", NULL, 0);
  child[4] = XmCreatePushButtonGadget(submenu2, "option2b", NULL, 0);
  child[5] = XmCreatePushButtonGadget(submenu2, "option3b", NULL, 0);
  XtManageChildren(&(child[3]), 3);

  /*
   * XmPulldownMenu: submenu4
   */

  submenu4 = XmCreatePulldownMenu(submenu1, "submenu4", args, 0);

  /*
   * XmPushButtonGadget: child[678]
   */

  child[6] = XmCreatePushButtonGadget(submenu4, "option1d", NULL, 0);
  child[7] = XmCreatePushButtonGadget(submenu4, "option2d", NULL, 0);

  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  child[8] = XmCreatePushButtonGadget(submenu4, "option3d", args, n);

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
  XtAddCallback(child[9], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) sailboat);

  child[10] = 
    special2 = XmCreateToggleButtonGadget(submenu5,"heart", NULL, 0);
  XtAddCallback(child[10], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) heart);

  child[11] = XmCreateToggleButtonGadget(submenu5,"man", NULL, 0);
  XtAddCallback(child[11], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) man);

  child[12] = XmCreateToggleButtonGadget(submenu5,"pirate", NULL, 0);
  XtAddCallback(child[12], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) pirate);

  child[13] = XmCreateToggleButtonGadget(submenu5,"shuttle", NULL, 0);
  XtAddCallback(child[13], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) shuttle);
  
  child[14] = XmCreateToggleButtonGadget(submenu5,"arrow", NULL, 0);
  XtAddCallback(child[14], XmNvalueChangedCallback, 
		ChangeCursor, (XtPointer) arrow);

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

  /*
   * XmCascadeButtonGadget: child[19]
   */

  n = 0; 
  XtSetArg(args[n], XmNsubMenuId, submenu4); n++;
  child[19] = XmCreateCascadeButtonGadget(submenu1, "more2", args, n);
  XtManageChild(child[19]);
}
