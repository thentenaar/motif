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
static char rcsid[] = "$XConsortium: ScrollWin2.c /main/8 1995/07/13 19:48:29 drk $"
#endif
#endif


#include <testlib.h>

Widget	ScrolledWindow1, RowColumn1,
	PushButtonGadget1, PushButtonGadget2, PushButtonGadget3, 
	PushButtonGadget4, PushButtonGadget5, PushButtonGadget6, 
        ScrolledWindow1, DrawingArea1, 
#ifdef NOCODE
	PushButtonGadget7, PushButtonGadget8, PushButtonGadget9, 
	PushButtonGadget10,PushButtonGadget11,PushButtonGadget12,
	PushButtonGadget13,PushButtonGadget14,PushButtonGadget15,
#endif
	PushButton1, PushButton2, PushButton3, PushButton4, PushButton5,
	PushButton6, PushButton7, PushButton8,
	ArrowButton1, ArrowButton2, ArrowButton3, ArrowButton4, ArrowButton5,
	ArrowButtonGadget1, ArrowButtonGadget2, ArrowButtonGadget3,
	ArrowButtonGadget4, ArrowButtonGadget5;


void  main(argc, argv)
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

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNshadowThickness, 2);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmTOP_LEFT); n++;
    ScrolledWindow1 = XmCreateScrolledWindow(Shell1, "ScrolledWindow1",
        args, n);

    if (ScrolledWindow1 != NULL)
    {
    	XtManageChild(ScrolledWindow1);
    }
    else
	printf ("ScrolledWindow zero \n");

    n = 0;
    DrawingArea1 =
	XmCreateDrawingArea(ScrolledWindow1, "DrawingArea1", args, n); 

    XmScrolledWindowSetAreas (ScrolledWindow1, NULL, NULL, DrawingArea1);

    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2);  n++;
    RowColumn1 = XmCreateRowColumn(ScrolledWindow1, "RowColumn1", args, n);
    XtManageChild(RowColumn1);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget1 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget1", args, n);
    XtManageChild(PushButtonGadget1);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget2 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget2", args, n);
    XtManageChild(PushButtonGadget2);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget3 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget3", args, n);
    XtManageChild(PushButtonGadget3);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget4 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget4", args, n);
    XtManageChild(PushButtonGadget4);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget5 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget5", args, n);
    XtManageChild(PushButtonGadget5);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget6 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget6", args, n);
    XtManageChild(PushButtonGadget6);

#ifdef NOCODE
    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget7 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget7", args, n);
    XtManageChild(PushButtonGadget7);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget8 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget8", args, n);
    XtManageChild(PushButtonGadget8);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget9 = XmCreatePushButtonGadget(RowColumn1,
                                                 "PushButtonGadget9", args, n);
    XtManageChild(PushButtonGadget9);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget10 = XmCreatePushButtonGadget(RowColumn1,
                                             "PushButtonGadget10", args, n);
    XtManageChild(PushButtonGadget10);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget11 = XmCreatePushButtonGadget(RowColumn1,
                                              "PushButtonGadget11", args, n);
    XtManageChild(PushButtonGadget11);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget12 = XmCreatePushButtonGadget(RowColumn1,
                                              "PushButtonGadget12", args, n);
    XtManageChild(PushButtonGadget12);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget13 = XmCreatePushButtonGadget(RowColumn1,
                                              "PushButtonGadget13", args, n);
    XtManageChild(PushButtonGadget13);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget14 = XmCreatePushButtonGadget(RowColumn1,
                                              "PushButtonGadget14", args, n);
    XtManageChild(PushButtonGadget14);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButtonGadget15 = XmCreatePushButtonGadget(RowColumn1,
                                              "PushButtonGadget15", args, n);
    XtManageChild(PushButtonGadget15);
#endif /* NOCODE */

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton1 = XmCreatePushButton(RowColumn1, "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton2 = XmCreatePushButton(RowColumn1, "PushButton2", args, n);
    XtManageChild(PushButton2);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton3 = XmCreatePushButton(RowColumn1, "PushButton3", args, n);
    XtManageChild(PushButton3);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton4 = XmCreatePushButton(RowColumn1, "PushButton4", args, n);
    XtManageChild(PushButton4);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton5 = XmCreatePushButton(RowColumn1, "PushButton5", args, n);
    XtManageChild(PushButton5);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton6 = XmCreatePushButton(RowColumn1, "PushButton6", args, n);
    XtManageChild(PushButton6);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton7 = XmCreatePushButton(RowColumn1, "PushButton7", args, n);
    XtManageChild(PushButton7);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("green"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    PushButton8 = XmCreatePushButton(RowColumn1, "PushButton8", args, n);
    XtManageChild(PushButton8);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButton1 = XmCreateArrowButton(RowColumn1, "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButton2 = XmCreateArrowButton(RowColumn1, "ArrowButton2", args, n);
    XtManageChild(ArrowButton2);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButton3 = XmCreateArrowButton(RowColumn1, "ArrowButton3", args, n);
    XtManageChild(ArrowButton3);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButton4 = XmCreateArrowButton(RowColumn1, "ArrowButton4", args, n);
    XtManageChild(ArrowButton4);

    n = 0;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButton5 = XmCreateArrowButton(RowColumn1, "ArrowButton5", args, n);
    XtManageChild(ArrowButton5);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButtonGadget1 = XmCreateArrowButtonGadget(RowColumn1,
                                               "ArrowButtonGadget1", args, n);
    XtManageChild(ArrowButtonGadget1);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButtonGadget2 = XmCreateArrowButtonGadget(RowColumn1,
                                               "ArrowButtonGadget2", args, n);
    XtManageChild(ArrowButtonGadget2);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButtonGadget3 = XmCreateArrowButtonGadget(RowColumn1,
                                               "ArrowButtonGadget3", args, n);
    XtManageChild(ArrowButtonGadget3);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButtonGadget4 = XmCreateArrowButtonGadget(RowColumn1,
                                               "ArrowButtonGadget4", args, n);
    XtManageChild(ArrowButtonGadget4);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 2);  n++;
    ArrowButtonGadget5 = XmCreateArrowButtonGadget(RowColumn1,
                                               "ArrowButtonGadget5", args, n);
    XtManageChild(ArrowButtonGadget5);

    CommonPause();
    CommonPause();
    CommonPause();

    CommonPause();

    XtAppMainLoop(app_context);
}
