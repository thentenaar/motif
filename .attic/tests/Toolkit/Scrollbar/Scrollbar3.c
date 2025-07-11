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
static char rcsid[] = "$XConsortium: Scrollbar3.c /main/6 1995/07/13 19:14:33 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.3              *
*       Thu Apr  6 08:30:55 1989        *
****************************************/

#include <testlib.h>

Widget	Form1, ScrollBar1, ScrollBar2, ScrollBar3, ScrollBar4, ScrollBar5;

void  main(argc, argv)
int     argc;
char  **argv;
{
  register int  n;
  Arg args[MAX_ARGS];

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
  XtSetValues(Shell1, args, n);
  XtRealizeWidget(Shell1);

  n = 0;
  Form1 = XmCreateForm(Shell1, "Form1", args, n);
  XtManageChild(Form1);

  n = 0;
  XtSetArg(args[n], XmNvalue, 100);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar1 = XmCreateScrollBar(Form1, "ScrollBar1", args, n);
  XtManageChild(ScrollBar1);

  n = 0;
  XtSetArg(args[n], XmNx, 50);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_TOP);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar2 = XmCreateScrollBar(Form1, "ScrollBar2", args, n);
  XtManageChild(ScrollBar2);

  n = 0;
  XtSetArg(args[n], XmNx, 50);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar3 = XmCreateScrollBar(Form1, "ScrollBar3", args, n);
  XtManageChild(ScrollBar3);

  XtDestroyWidget(ScrollBar3);

  n = 0;
  XtSetArg(args[n], XmNx, 100);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar3 = XmCreateScrollBar(Form1, "ScrollBar3", args, n);
  XtManageChild(ScrollBar3);

  n = 0;
  XtSetArg(args[n], XmNx, 150);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar4 = XmCreateScrollBar(Form1, "ScrollBar4", args, n);
  XtManageChild(ScrollBar4);

  n = 0;
  XtSetArg(args[n], XmNx, 200);  n++;
  XtSetArg(args[n], XmNsliderSize, 20);  n++;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 25);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 5);  n++;
  ScrollBar5 = XmCreateScrollBar(Form1, "ScrollBar5", args, n);
  XtManageChild(ScrollBar5);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNvalue, 10);  n++;
  XtSetArg(args[n], XmNminimum, 20);  n++;
  XtSetValues(ScrollBar1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNminimum, 0);  n++;
  XtSetValues(ScrollBar1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpageIncrement, 5);  n++;
  XtSetValues(ScrollBar1, args, n);
  CommonPause();
  
  n = 0;
  XtSetArg(args[n], XmNinitialDelay, 5);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 25);  n++;
  XtSetValues(ScrollBar1, args, n);

  n = 0;
  XtSetArg(args[n], XmNminimum, 50);  n++;
  XtSetValues(ScrollBar1, args, n);
  CommonPause();
  
  n = 0;
  XtSetArg(args[n], XmNmaximum, 20);  n++;
  XtSetValues(ScrollBar1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNshowArrows, FALSE);  n++;
  XtSetValues(ScrollBar2, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNvalue, 0);  n++;
  XtSetValues(ScrollBar2, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNincrement, -1);  n++;
  XtSetArg(args[n], XmNpageIncrement, -1);  n++;
  XtSetArg(args[n], XmNinitialDelay, -1);  n++;
  XtSetArg(args[n], XmNrepeatDelay, -1);  n++;
  XtSetValues(ScrollBar3, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNincrement, 10);  n++;
  XtSetArg(args[n], XmNpageIncrement, 20);  n++;
  XtSetArg(args[n], XmNinitialDelay, 0);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 0);  n++;
  XtSetValues(ScrollBar3, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNinitialDelay, 1);  n++;
  XtSetArg(args[n], XmNrepeatDelay, 1);  n++;
  XtSetValues(ScrollBar3, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_TOP);  n++;
  XtSetValues(ScrollBar4, args, n);
  CommonPause();
  
  n = 0;
  XtSetArg(args[n], XmNvalue, 10);  n++;
  XtSetValues(ScrollBar4, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNminimum, 20);  n++;
  XtSetValues(ScrollBar4, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNvalue, 30);  n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_TOP);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNvalue, 60);  n++;
  XtSetArg(args[n], XmNsliderSize, 50);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 100);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 99);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNsliderSize, 101);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNminimum, 20);  n++;
  XtSetValues(ScrollBar5, args, n);
  CommonPause();

  XtDestroyWidget(ScrollBar5);
  XtDestroyWidget(ScrollBar4);
  XtDestroyWidget(ScrollBar3);
  XtDestroyWidget(ScrollBar2);
  XtDestroyWidget(ScrollBar1);
  XtDestroyWidget(Form1);
  CommonPause();

  XtAppMainLoop(app_context);
}


















