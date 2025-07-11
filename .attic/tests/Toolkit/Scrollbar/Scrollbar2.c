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
static char rcsid[] = "$XConsortium: Scrollbar2.c /main/6 1995/07/13 19:14:15 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.3              *
*       Thu Apr  6 07:43:20 1989        *
****************************************/

#include <testlib.h>

Widget Form1, ScrollBar1, ScrollBar2;

void  main(argc, argv)
int argc;
char **argv;
{
  register int  n;
  Arg args[MAX_ARGS];

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetValues(Shell1, args, n);
  XtRealizeWidget(Shell1);

  n = 0;
  Form1 = XmCreateForm(Shell1, "Form1", args, n);
  XtManageChild(Form1);

  n = 0;
  ScrollBar1 = XmCreateScrollBar(Form1, "ScrollBar1", args, n);
  XtManageChild(ScrollBar1);

  n = 0;
  XtSetArg(args[n], XmNx, 50);  n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
  ScrollBar2 = XmCreateScrollBar(Form1, "ScrollBar2", args, n);
  XtManageChild(ScrollBar2);

  n = 0;
  XtSetArg(args[n], XmNvalue, 0);  n++;
  XtSetValues(ScrollBar1, args, n);
  XtDestroyWidget(ScrollBar1);

  CommonPause();

  n = 0;
  ScrollBar1 = XmCreateScrollBar(Form1, "ScrollBar1", args, n);
  XtManageChild(ScrollBar1);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNminimum, -5);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 100);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 105);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNvalue, -5);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNincrement, 0);  n++;
  XtSetArg(args[n], XmNpageIncrement, 0);  n++;
  XtSetArg(args[n], XmNinitialDelay, 0);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 0);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNincrement, 100);  n++;
  XtSetArg(args[n], XmNpageIncrement, 200);  n++;
  XtSetArg(args[n], XmNinitialDelay, 10);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  XtSetValues(ScrollBar1, args, n);

  n = 0;
  XtSetArg(args[n], XmNvalue, 0);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();
  n = 0;
  XtSetArg(args[n], XmNvalue, -10);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();
  n = 0;
  XtSetArg(args[n], XmNvalue, 0);  n++;
  XtSetValues(ScrollBar1, args, n);

  n = 0;
  XtSetArg(args[n], XmNminimum, 0);  n++;
  XtSetValues(ScrollBar1, args, n);

  CommonPause();

  XtDestroyWidget(ScrollBar2);
  XtDestroyWidget(ScrollBar1);
  XtDestroyWidget(Form1);
  CommonPause();
  XtAppMainLoop(app_context);
}


















