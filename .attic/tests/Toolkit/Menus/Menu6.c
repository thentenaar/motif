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
static char rcsid[] = "$XConsortium: Menu6.c /main/10 1995/07/13 18:45:52 drk $"
#endif
#endif


#include <testlib.h>
#include <X11/StringDefs.h>

#define ARRANGE_MENU_SIZE 8
#define LAYOUT_MENU_SIZE 6
#define SUBMENU4SIZE 6

static char *arrangeMenuNames[] = 
{ 
  "Align Top Sides",
  "Align Bottom Sides",
  "Align Left Sides",
  "Align Right Sides",
  "Center",
  "Align to Form",
  "Align to Parent",
  "Align Diagonally"
  };

static char *layoutMenuNames[] = 
{ 
  "Square and Circle",
  "Circle and Rectangle",
  "Square and Oval",
  "Oval and Square",
  "Two Squares",
  "Two Ovals"
  };

static char *submenu4Names[] = 
{ 
  "Circle and Square",
  "Square and Square",
  "Rectangle and Square",
  "Square and Oval",
  "Oval and Rectangle",
  "Arc and Square"
  };

Widget mainWindow = NULL, MenuBar;
Widget CascadeBtn1, CascadeBtn2, CascadeBtn3, CascadeBtn4;
Widget ArrangeMenu, LayoutMenu;
Widget submenu4, submenu6, pulldown1, pulldown2, pulldown3, pulldown4, w;
Widget arrangeMenuButtons[ARRANGE_MENU_SIZE];
Widget submenu4Buttons[SUBMENU4SIZE];
Widget layoutMenuButtons[ARRANGE_MENU_SIZE];
int which_way;

void remove_text(Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[2];
  int i=0;
  XmString tcs;
  switch(which_way)
    {
    case 0:
      i=0;
      XtSetArg(args[i], XmNacceleratorText, NULL); i++;
      XtSetArg(args[i], XmNaccelerator, NULL); i++;
      XtSetValues(w, args,i);
      break;
    case 1:
      i=0;
      XtSetArg(args[i],XmNacceleratorText,tcs=XmStringCreateSimple("Alt+R")); 
      i++;
      XtSetValues(w, args,i);
      XmStringFree(tcs);
      break;
    }
}

static void activate_msg(Widget w, XtPointer client_dat, 
			 XtPointer call_data)
{
  fprintf(stdout, "widget cascade callback called\n");
}

int main(int argc, char **argv)
{
  register int  n;
  Arg args[MAX_ARGS];
  Arg nargs[MAX_ARGS];
  int i;
  char buf[32];
  XmString string, tcs;

  CommonTestInit(argc, argv);

  /* 
   * The parent of all this will be a main window
   */

  n = 0;
  XtSetArg(args[n], XmNborderWidth, 2); n++;
  mainWindow = XmCreateMainWindow(Shell1, "mainWindow", args, n);

  /* 
   * now the menu bar 
   */

  n = 0;
  MenuBar = XmCreateMenuBar(mainWindow, "MenuBar", args, n);
  XtManageChild(MenuBar);

  /* 
   * The cascade button children of the menu bar 
   */

  n = 0;
  string = XmStringCreate("File", XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNlabelString, string); n++;
  CascadeBtn1 = XmCreateCascadeButtonGadget(MenuBar, "CascadeBtn1", args, n);
  XtAddCallback(CascadeBtn1, XmNcascadingCallback, activate_msg, NULL);
  XtManageChild(CascadeBtn1);
  XmStringFree(string);

  n = 0;
  string = XmStringCreate("Edit", XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNlabelString, string); n++;
  CascadeBtn2 = XmCreateCascadeButtonGadget(MenuBar, "CascadeBtn2", args, n);
  XtAddCallback(CascadeBtn2, XmNcascadingCallback, activate_msg, NULL);
  XtManageChild(CascadeBtn2);
  XmStringFree(string);

  n = 0;
  string = XmStringCreate("Arrange", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, string); n++;
  CascadeBtn3 = XmCreateCascadeButtonGadget(MenuBar, "CascadeBtn3", args, n);
  XtManageChild(CascadeBtn3);
  XmStringFree(string);

  n = 0;
  string = XmStringCreate("Layout", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, string); n++;
  CascadeBtn4 = XmCreateCascadeButtonGadget(MenuBar, "CascadeBtn4", args, n);
  XtManageChild(CascadeBtn4);
  XmStringFree(string);

  /* 
   *  The Arrange subMenu will be 2D, ie 2 cols 
   */

  n = 0;
  XtSetArg(args[n], XmNadjustLast, False); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 2); n++;
  ArrangeMenu = XmCreatePulldownMenu(MenuBar, "ArrangeMenu", args, n);

  /* 
   * ArrangeMenu is the submenu of CascadeBtn3 
   */

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, ArrangeMenu); n++;
  XtSetValues(CascadeBtn3, args, n);

  /* 
   * We need several button children, let's try this 
   */

  for (i = 0; i <= (ARRANGE_MENU_SIZE - 1); i++)
    { 
      n = 0;
      string = XmStringCreate(arrangeMenuNames[i], XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, string); n++;
      sprintf(buf, "arrangeButton%d", i);
      arrangeMenuButtons[i] = XmCreateCascadeButtonGadget(ArrangeMenu,
							  buf, args, n);
      XtManageChild(arrangeMenuButtons[i]);
      XmStringFree(string);
    };
    
  /*
   *  some of the button children, say, 4 and 6, need submenus
   *  submenu 4 will be a 2D menu as well, with six children;
   *  we'll use the same scheme as above 
   */

  n = 0;
  XtSetArg(args[n], XmNadjustLast, False); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 2); n++;
  submenu4 = XmCreatePulldownMenu(ArrangeMenu, "submenu4", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, submenu4); n++;
  XtSetValues(arrangeMenuButtons[3], args, n);
  
  for (i = 0; i <= (SUBMENU4SIZE - 1); i++)
    { 
      n = 0;
      string = XmStringCreate(submenu4Names[i], XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, string); n++;
      sprintf(buf, "submenu4Button%d", i);
      submenu4Buttons[i] = XmCreatePushButtonGadget(submenu4, buf, args, n);
      XtManageChild(submenu4Buttons[i]);
      XmStringFree(string);
    };

  /*
   *  create the layout menu. It will hang off of CascadeBtn4 
   */

  n = 0;
  XtSetArg(args[n], XmNadjustLast, False); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg(args[n], XmNnumColumns, 2); n++;
  LayoutMenu = XmCreatePulldownMenu(MenuBar, "LayoutMenu", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, LayoutMenu); n++;
  XtSetValues(CascadeBtn4, args, n);

  /* 
   * children of the Layout menu 
   */

  for (i = 0; i <= (LAYOUT_MENU_SIZE - 1); i++)
    { 
      n = 0;
      string = XmStringCreate(layoutMenuNames[i], XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, string); n++;
      sprintf(buf, "layoutButton%d", i);
      layoutMenuButtons[i] = XmCreateCascadeButtonGadget(LayoutMenu,
							 buf, args, n);
      XtManageChild(layoutMenuButtons[i]);
      XmStringFree(string);
    };

  /* 
   * set up the main window areas
   */
  
  XmMainWindowSetAreas(mainWindow, MenuBar, NULL, NULL, NULL, NULL);
    
  XtManageChild(mainWindow);
  XtRealizeWidget(Shell1);

  CommonPause();

#ifndef MOTIF1_1
  CommonPause();
#endif

  XtDestroyWidget(MenuBar);
  XtDestroyWidget(mainWindow);

  mainWindow = XmCreateMainWindow(Shell1, "mainWindow", NULL, 0);
  XtManageChild(mainWindow);

  MenuBar = XmCreateMenuBar(mainWindow, "MenuBar", NULL, 0);
  XtManageChild(MenuBar);

  i=0;
  XtSetArg(args[i],XmNacceleratorText, tcs=XmStringCreateSimple("^R")); i++;

  pulldown1 = XmCreatePulldownMenu(MenuBar, "pulldown1", NULL, 0);

  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown1); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu1", nargs, n));

  XtManageChild(w = XmCreatePushButton(pulldown1, "PushButton", args, i));
  XtAddCallback(w, XmNactivateCallback, remove_text, NULL);

  pulldown2 = XmCreatePulldownMenu(MenuBar, "pulldown2", NULL, 0);
  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown2); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu2", nargs, n));
  XtManageChild(w = XmCreatePushButtonGadget(pulldown2,
					     "PushButtonGadget", args, i));
  XtAddCallback(w, XmNactivateCallback, remove_text, NULL);

  pulldown3 = XmCreatePulldownMenu(MenuBar, "pulldown3", NULL, 0);
  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown3); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu3", nargs, n));
  XtManageChild(w = XmCreateToggleButton(pulldown3, "ToggleButton", args, i));
  XtAddCallback(w, XmNvalueChangedCallback, remove_text, NULL);

  pulldown4 = XmCreatePulldownMenu(MenuBar, "pulldown4", NULL, 0);
  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown4); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu4", nargs, n));
  XtManageChild(w = XmCreateToggleButtonGadget(pulldown4, 
					       "ToggleButtonGadget", 
					       args, i));
  XtAddCallback(w, XmNvalueChangedCallback, remove_text, NULL);

  XmStringFree (tcs);

  XtRealizeWidget(Shell1);

  which_way=0;
  CommonPause();
  which_way=1;
  CommonPause();
  CommonPause(); /* Exit */
  XtAppMainLoop(app_context);
  return(1);
}












