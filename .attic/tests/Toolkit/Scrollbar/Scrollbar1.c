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
static char rcsid[] = "$XConsortium: Scrollbar1.c /main/6 1995/07/13 19:13:25 drk $"
#endif
#endif




/****************************************
*   Automatic test program generation   *
*              Version 1.2              *
*       Wed Apr  5 08:27:41 1989        *
****************************************/

#include <testlib.h>

Widget	BulletinBoard1, ScrollBar1, ScrollBar2, ScrollBar3;

void  main (argc, argv)
int argc;
char **argv;
{
  register int n;
  Arg args[MAX_ARGS];

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetValues(Shell1, args, n);
  XtRealizeWidget(Shell1);
  
  n = 0;
  BulletinBoard1 = XmCreateBulletinBoard(Shell1, "BulletinBoard1", args, n);
  XtManageChild(BulletinBoard1);

  n = 0;
  ScrollBar1 = XmCreateScrollBar(BulletinBoard1, "ScrollBar1", args, n);
  XtManageChild(ScrollBar1);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
  ScrollBar2 = XmCreateScrollBar(BulletinBoard1, "ScrollBar2", args, n);
  XtManageChild(ScrollBar2);

  CommonPause();
  n = 0;
  XtSetArg(args[n], XmNx, 50);  n++;
  XtSetValues(ScrollBar2, args, n);

  CommonPause();
  n = 0;
  XtSetArg(args[n], XmNx, 50);  n++;
  XtSetArg(args[n], XmNy, 100);  n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
  ScrollBar3 = XmCreateScrollBar(BulletinBoard1, "ScrollBar3", args, n);
  XtManageChild(ScrollBar3);

  CommonPause();

  XtDestroyWidget(ScrollBar3);
  XtDestroyWidget(ScrollBar2);
  XtDestroyWidget(ScrollBar1);
  XtDestroyWidget(BulletinBoard1);
  CommonPause();
  XtAppMainLoop(app_context);
}


















