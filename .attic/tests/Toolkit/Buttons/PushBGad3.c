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
static char rcsid[] = "$XConsortium: PushBGad3.c /main/8 1995/07/13 17:29:42 drk $"
#endif
#endif

#include <X11/Xlib.h>
#include <testlib.h>

static void  arrow1CB();
static void  arrow2CB();

Widget bb;
Widget rc1, rc2;
Widget arrow1, arrow2, arrow3, arrow4, arrow5, arrow6;

AddAllTabGroups()
{
  /* Add in correct order */
  XmAddTabGroup(rc1);
  XmAddTabGroup(rc2);
}

/*******************************************************/

void main (argc, argv)
unsigned int argc;
char **argv;
{
  Arg args[15];
  int n;

  CommonTestInit(argc, argv);
   
  bb = XmCreateBulletinBoard(Shell1, "bb", (ArgList) args, 0);
  XtManageChild(bb);

  XtSetArg(args[0], XmNborderWidth, 4); 
  rc1 = XmCreateRowColumn(bb, "rc1", (ArgList) args, 1);
  XtManageChild(rc1);
  n = 0;
  XtSetArg (args[0], XmNborderWidth, 4); n++;
  XtSetArg (args[n], XmNx, 100); n++;
  XtSetArg (args[n], XmNy, 100); n++;
  rc2 = XmCreateRowColumn(bb, "rc2", (ArgList) args, n);
  XtManageChild(rc2);

  /* resources common to all buttons */
  n = 0;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNhighlightThickness, 2); n++;
  XtSetArg(args[n], XmNhighlightOnEnter, True); n++;

  arrow1 = XmCreatePushButtonGadget(rc1, "arrow1", args, n);
  XtManageChild(arrow1);
  XtAddCallback(arrow1, XmNactivateCallback, arrow1CB, NULL);

  arrow2 = XmCreatePushButtonGadget(rc1, "arrow2", args, n);
  XtManageChild(arrow2);
  XtAddCallback(arrow2, XmNactivateCallback, arrow2CB, NULL);

  arrow3 = XmCreatePushButtonGadget(rc1, "arrow3", args, n);
  XtManageChild(arrow3);

  arrow4 = XmCreatePushButtonGadget(rc2, "arrow4", args, n);
  XtManageChild(arrow4);

  arrow5 = XmCreatePushButtonGadget(rc2, "arrow5", args, n);
  XtManageChild(arrow5);

  arrow6 = XmCreatePushButtonGadget(rc2, "arrow6", args, n);
  XtManageChild(arrow6);

  AddAllTabGroups();
  XtRealizeWidget (Shell1);
  CommonPause ();
  CommonPause ();
  CommonPause (); /* end */
  XtAppMainLoop(app_context);
  XtDestroyWidget(arrow1);
  XtDestroyWidget(arrow2);
  XtDestroyWidget(arrow3);
  XtDestroyWidget(arrow4);
  XtDestroyWidget(arrow5);
  XtDestroyWidget(arrow6);
  XtDestroyWidget(bb);
  XtDestroyWidget(rc1);
  XtDestroyWidget(rc2);
}

static void  arrow1CB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
  XtManageChild(arrow2);
}

static void  arrow2CB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
  XtUnmanageChild(arrow2);
}





