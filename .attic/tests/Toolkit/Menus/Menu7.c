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
static char rcsid[] = "$XConsortium: Menu7.c /main/5 1995/07/13 18:46:13 drk $"
#endif
#endif


#include <testlib.h>
#include <X11/StringDefs.h>

Widget mainWindow = NULL, MenuBar, cascade1, cascade2, ob1, ob2, ob3, ob4;
Widget pb1, pb2, pb3, pb4, newkid1, newkid2, newkid3;
Widget submenu4, submenu6, pulldown1, pulldown2, w;

int main(argc, argv)
int argc;
char **argv;
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
  mainWindow = XmCreateMainWindow(Shell1, "mainWindow", NULL, 0);
  XtManageChild(mainWindow);

  MenuBar = XmCreateMenuBar(mainWindow, "MenuBar", NULL, 0);
  XtManageChild(MenuBar);

  pulldown1 = XmCreatePulldownMenu(MenuBar, "pulldown1", NULL, 0);

  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown1); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu1", nargs, n));

  pb1 = XmCreatePushButton(pulldown1, "PushButton1", args, 0);
  pb2 = XmCreatePushButton(pulldown1, "PushButton2", args, 0);
  pb3 = XmCreatePushButton(pulldown1, "PushButton3", args, 0);
  
  XtManageChild(pb1);
  XtManageChild(pb2);
  XtManageChild(pb3);

  pulldown2 = XmCreatePulldownMenu(MenuBar, "pulldown2", NULL, 0);
  n=0;
  XtSetArg(nargs[n],XmNsubMenuId, pulldown2); n++;
  XtManageChild(XmCreateCascadeButtonGadget(MenuBar, "Menu2", nargs, n));
  ob1 = XmCreatePushButton(pulldown2, "OtherButton1", args, 0);
  ob2 = XmCreatePushButton(pulldown2, "OtherButton2", args, 0);
  ob3 = XmCreatePushButton(pulldown2, "OtherButton3", args, 0);
  ob4 = XmCreatePushButton(pulldown2, "OtherButton4", args, 0);
  XtManageChild(ob1);
  XtManageChild(ob2);
  XtManageChild(ob3);
  XtManageChild(ob4);
  XtRealizeWidget(Shell1);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, XmLAST_POSITION); n++;
  XtSetValues(pb1, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 0); n++;
  XtSetValues(ob4, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 1); n++;
  XtSetValues(ob3, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 2); n++;
  XtSetValues(ob2, args, n);

  CommonPause();

  /* Try some illegal values */

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, -3); n++;
  XtSetValues(ob2, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 20); n++;
  XtSetValues(pb2, args, n);
  CommonPause();

  /* Add some to the middle */

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 2); n++;
  newkid1 = XmCreatePushButton(pulldown1, "NewKid1", args, n);
  XtManageChild(newkid1);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, XmLAST_POSITION); n++;
  newkid2 = XmCreatePushButton(pulldown2, "NewKid2", args, n);
  XtManageChild(newkid2);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 0); n++;
  newkid3 = XmCreatePushButton(pulldown2, "NewKid3", args, n);
  XtManageChild(newkid3);

  CommonPause();

  CommonPause(); /* Exit */
  XtAppMainLoop(app_context);
  return(1);
}












