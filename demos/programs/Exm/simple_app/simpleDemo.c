/* $XConsortium: simpleDemo.c /main/5 1995/07/15 20:44:25 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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


/****************************************************************************
 ****************************************************************************
 **
 **   File:     simpleDemo.c
 **
 **   Description: C program to demonstrate the ExmSimple widget. 
 **
 ****************************************************************************
 ****************************************************************************/


#include <sys/file.h>

/*  Standard C headers  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  X headers  */
#include <X11/IntrinsicP.h>

/*  Xm headers  */
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/CascadeBG.h>
#include <Exm/Simple.h>
#include <Exm/ExmString.h>
#include <Exm/Grid.h>
#include <Exm/MenuB.h>

/* Definitions. */
#define      APP_CLASS  "XMdemo"

/*******************************************************************************
exercise_ExmSimple: Realize four ExmSimple widgets. 
*******************************************************************************/
void 
exercise_ExmSimple(Widget parent)
{
 Widget   simpleWidget1, simpleWidget2, simpleWidget3, simpleWidget4;

 /* Instantiate a default ExmSimple widget; the default shape is an oval. */
  simpleWidget1 = XtVaCreateManagedWidget("Simple1", 
                               exmSimpleWidgetClass, parent, 
                               NULL); 

 /* Override the default shape; draw a rectangle instead. */ 
  simpleWidget2 = XtVaCreateManagedWidget("Simple2", 
                               exmSimpleWidgetClass, parent, 
                               ExmNsimpleShape,      ExmSHAPE_RECTANGLE,
                               NULL);

 /* Override the default shadow thickness and margin width. */ 
  simpleWidget3 = XtVaCreateManagedWidget("Simple3", 
                               exmSimpleWidgetClass, parent,
                               ExmNsimpleShape,     ExmSHAPE_OVAL,
                               XmNshadowThickness,  14,
                               XmNmarginWidth,      9,
                               NULL);

 /* Experiment with a different unit type. */
  simpleWidget4 = XtVaCreateManagedWidget("Simple4", 
                               exmSimpleWidgetClass, parent, 
                               ExmNsimpleShape,     ExmSHAPE_RECTANGLE,
                               XmNunitType,         Xm100TH_MILLIMETERS,
                               XmNmarginHeight,     1000,
                               NULL);
}


/*******************************************************************************
QuitCB: Exit the program when the user selects 'Quit'
*******************************************************************************/
void
QuitCB(Widget w, XtPointer ignore, XtPointer cbstruct)
{
  exit(0);
}



/*******************************************************************************
main: Set up the application, including the mainWindow and menuBar.
*******************************************************************************/
int 
main (int argc, char **argv)
{
  Display	*display;
  Widget       	shell, mainWindow, workRegion, menuBar, cascade, menuitem,
           	menu;
  XtAppContext 	AppContext;
  XmString	str;

 /* Do standard Motif application start-up. */
  XtToolkitInitialize();
  AppContext = XtCreateApplicationContext();
  if ((display = XtOpenDisplay (AppContext, NULL, argv[0], APP_CLASS,
				NULL, 0, &argc, argv)) == NULL)
    {
      fprintf (stderr,"\n%s:  Can't open display\n", argv[0]);
      exit(1);
    }

  shell = XtVaAppCreateShell(argv[0], APP_CLASS, applicationShellWidgetClass,
			     display, XmNallowShellResize, True, NULL);
			     

  mainWindow = XtVaCreateManagedWidget("myMainWindow", xmMainWindowWidgetClass, 
                                       shell, NULL);

 /* Create a menu bar with one simple menu. */
  menuBar = XmCreateMenuBar(mainWindow, "menuBar", NULL, 0);
  XtManageChild(menuBar);

  cascade = XmCreateCascadeButtonGadget(menuBar, "File", NULL, 0);
  XtManageChild(cascade);

  menu = XmCreatePulldownMenu(menuBar, "FileMenu", NULL, 0);

  XtVaSetValues(cascade, XmNsubMenuId, menu, NULL);

  str = XmStringCreateLocalized("Quit");
  menuitem = XtVaCreateManagedWidget("item", xmPushButtonWidgetClass,
				     menu,
				     XmNlabelString, str,
				     NULL);
  XmStringFree(str);
  XtAddCallback(menuitem, XmNactivateCallback, QuitCB, NULL);

 /* Create a XmRowColumn widget; the ExmSimple widgets will ultimately be
    managed by this XmRowColumn. */
  workRegion = XtVaCreateManagedWidget("myWorkRegion", xmRowColumnWidgetClass,
                                       mainWindow, 
                                       NULL);
  
  exercise_ExmSimple(workRegion);

  XtRealizeWidget(shell);

  XtAppMainLoop(AppContext);

  return 0;    /* make compiler happy */
}
