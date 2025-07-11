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
static char rcsid[] = "$XConsortium: ScrollWin1.c /main/8 1995/07/13 19:47:41 drk $"
#endif
#endif


#include <testlib.h>

Widget	PushButton1, PushButton2, PushButton3, PushButton4, PushButton5,
	PushButton6, PushButton7, PushButton8, PushButton9, PushButton10,
	PushButton11,PushButton12,PushButton13,PushButton14,PushButton15,
	ScrolledWindow1, RowColumn2;


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetValues(Shell1, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNshadowThickness, 4);  n++;
    ScrolledWindow1 = XmCreateScrolledWindow(Shell1, "ScrolledWindow1", 
	args, n);

    if (ScrolledWindow1 != NULL) 
	XtManageChild(ScrolledWindow1);
    else
	printf ("ScrolledWindow zero\n");

    XtRealizeWidget(Shell1);

    CommonPause();
 
    n = 0;
    RowColumn2 = XmCreateRowColumn(ScrolledWindow1, "RowColumn2", args, n);
    XtManageChild(RowColumn2);

    n = 0;
    PushButton1 = XmCreatePushButton(RowColumn2, "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    PushButton2 = XmCreatePushButton(RowColumn2, "PushButton2", args, n);
    XtManageChild(PushButton2);

    n = 0;
    PushButton3 = XmCreatePushButton(RowColumn2, "PushButton3", args, n);
    XtManageChild(PushButton3);

    n = 0;
    PushButton4 = XmCreatePushButton(RowColumn2, "PushButton4", args, n);
    XtManageChild(PushButton4);

    n = 0;
    PushButton5 = XmCreatePushButton(RowColumn2, "PushButton5", args, n);
    XtManageChild(PushButton5);

    n = 0;
    PushButton6 = XmCreatePushButton(RowColumn2, "PushButton6", args, n);
    XtManageChild(PushButton6);

    n = 0;
    PushButton7 = XmCreatePushButton(RowColumn2, "PushButton7", args, n);
    XtManageChild(PushButton7);

    n = 0;
    PushButton8 = XmCreatePushButton(RowColumn2, "PushButton8", args, n);
    XtManageChild(PushButton8);

    n = 0;
    PushButton9 = XmCreatePushButton(RowColumn2, "PushButton9", args, n);
    XtManageChild(PushButton9);

    n = 0;
    PushButton10 = XmCreatePushButton(RowColumn2, "PushButton10", args, n);
    XtManageChild(PushButton10);

    n = 0;
    PushButton11 = XmCreatePushButton(RowColumn2, "PushButton11", args, n);
    XtManageChild(PushButton11);

    n = 0;
    PushButton12 = XmCreatePushButton(RowColumn2, "PushButton12", args, n);
    XtManageChild(PushButton12);

    n = 0;
    PushButton13 = XmCreatePushButton(RowColumn2, "PushButton13", args, n);
    XtManageChild(PushButton13);

    n = 0;
    PushButton14 = XmCreatePushButton(RowColumn2, "PushButton14", args, n);
    XtManageChild(PushButton14);

    n = 0;
    PushButton15 = XmCreatePushButton(RowColumn2, "PushButton15", args, n);
    XtManageChild(PushButton15);


    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}
