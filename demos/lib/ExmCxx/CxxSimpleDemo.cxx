/* $XConsortium: CxxSimpleDemo.cxx /main/4 1995/07/17 11:45:34 drk $ */
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
 **   Description: C program to instantiate the ExmSimple widget. 
 **
 ****************************************************************************
 ****************************************************************************/


#include <sys/file.h>

/*  Standard C headers  */
#include <stdio.h>
#include <stdlib.h>

/*  X headers  */
#include <X11/IntrinsicP.h>

/*  Xm headers  */
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Xm/Label.h>
#include <XmCxx/CxxXmString.h>
#include "CxxSimple.h"
#include "CxxString.h"
#include "CxxGrid.h"
#include "CxxCommand.h"

static void TickleCB(Widget, XtPointer, XtPointer);

void TickleCB(
   Widget w,
   XtPointer,
   XtPointer
              )
{
 XmCxxString  stop_that("Stop that!");
 Arg       arg[2];
 Widget    MsgBox1;
 printf("In TickleCB\n");

 XtSetArg(arg[0], XmNmessageString, (XmString) stop_that);
 
 MsgBox1 = XmCreateMessageDialog(w, "MsgBox1",  arg, 1);

 XtManageChild(MsgBox1);
} 


void make_simple_widgets(Widget parent)
{
  Widget       simpleWidget1, simpleWidget2, stringWidget,
    commandButtonWidget1;
  XmCxxString  compound_string1, compound_string2;
  Arg          args[8];
  Cardinal     argcount;

  simpleWidget1 = XtCreateManagedWidget("Simple1", 
					exmCxxSimpleWidgetClass, parent, 
					NULL, 0);

  argcount = 0;
  XtSetArg(args[argcount], ExmNsimpleShape, ExmSHAPE_RECTANGLE); argcount++;
  XtSetArg(args[argcount], XmNmarginWidth, 20); argcount++;

  simpleWidget2 = XtCreateManagedWidget("Simple2", 
					exmCxxSimpleWidgetClass, parent, 
					args, argcount);

  compound_string1 = "ExmString";
  compound_string1 = compound_string1 + XmCxxString::Separator() + "widget";

  argcount = 0;
  XtSetArg(args[argcount], ExmNcompoundString, (XmString) compound_string1);
  argcount++;
  stringWidget = XtCreateManagedWidget("String", 
				       exmCxxStringWidgetClass, parent, 
				       args, argcount);

  compound_string2 = "ExmCommandButton";
  compound_string2 = compound_string2 + XmCxxString::Separator() + "Push me";

  argcount = 0;
  XtSetArg(args[argcount], ExmNcompoundString, (XmString) compound_string2);
  argcount++;
  commandButtonWidget1 = XtCreateManagedWidget("CommandButtonWidget1", 
					       exmCxxCommandWidgetClass, 
					       parent, args, argcount);

  XtAddCallback(commandButtonWidget1, XmNactivateCallback, TickleCB, NULL);
}

#define      APP_CLASS  "C++ Widgets demo"

int main (int argc, char **argv)
{
  Display     *display;
  Widget       shell, mainWindow, workRegion;
  XtAppContext AppContext;
  Arg args[8];
  Cardinal argcount = 0;


  XtToolkitInitialize();
  AppContext = XtCreateApplicationContext();
  if ((display = XtOpenDisplay (AppContext, NULL, argv[0], APP_CLASS,
				NULL, 0, &argc, argv)) == NULL)
    {
      fprintf (stderr,"\n%s:  Can't open display\n", argv[0]);
      exit(1);
    }

  argcount = 0;
  XtSetArg(args[argcount], XmNallowShellResize, True); argcount++;

  shell = XtAppCreateShell(argv[0], APP_CLASS, applicationShellWidgetClass,
			   display, args, argcount);
			     

  mainWindow = XtCreateManagedWidget("myMainWindow", xmMainWindowWidgetClass, 
				     shell, NULL, 0);

  argcount = 0;
  XtSetArg(args[argcount], XmNrows, 2); argcount++;
  XtSetArg(args[argcount], XmNcolumns, 2); argcount++;
  workRegion = XtCreateManagedWidget("myWorkRegion", exmCxxGridWidgetClass,
				     mainWindow, args, argcount); 

  make_simple_widgets(workRegion);

  XmMainWindowSetAreas(mainWindow, NULL, NULL, NULL, NULL, workRegion);

  XtRealizeWidget(shell);

  XtAppMainLoop(AppContext);
}
