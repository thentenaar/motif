/* $TOG: DNDScroll.c /main/5 1999/11/11 15:42:21 jff $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <testlib.h>


Widget  ScrolledWin1;
Widget  vertSB, horizSB;
Widget	Clip1;

#define SLIDER_SIZE 10
#define	SCROLL_DELAY 50


void CreateScrollBars(parent)
Widget parent;
{
    Arg args[10];
    int n;

    n = 0;
    XtSetArg (args[n], XmNorientation, XmVERTICAL);  n++;
    XtSetArg (args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg (args[n], XmNmaximum, 60);  n++;
    XtSetArg (args[n], XmNminimum, 0);  n++;
    XtSetArg (args[n], XmNtraversalOn, True);  n++;
    XtSetArg (args[n], XmNhighlightOnEnter, True);  n++;
    XtSetArg (args[n], XmNhighlightThickness, 2);  n++;
    XtSetArg (args[n], XmNsliderSize, SLIDER_SIZE);  n++;
    XtSetArg (args[n], XmNinitialDelay, SCROLL_DELAY);  n++;
    XtSetArg (args[n], XmNrepeatDelay, SCROLL_DELAY);  n++;
    vertSB = XmCreateScrollBar (parent, "vertSB", args, n);
    XtManageChild (vertSB);

    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg (args[n], XmNmaximum, 59);  n++;
    XtSetArg (args[n], XmNminimum, 0);  n++;
    XtSetArg (args[n], XmNtraversalOn, True);  n++;
    XtSetArg (args[n], XmNhighlightOnEnter, True);  n++;
    XtSetArg (args[n], XmNhighlightThickness, 2);  n++;
    XtSetArg (args[n], XmNsliderSize, SLIDER_SIZE);  n++;
    XtSetArg (args[n], XmNinitialDelay, SCROLL_DELAY);  n++;
    XtSetArg (args[n], XmNrepeatDelay, SCROLL_DELAY);  n++;
    horizSB = XmCreateScrollBar (parent, "horizSB", args, n);

    XtManageChild (horizSB);
}




void  main(argc, argv)
    int     argc;
    char  **argv;
{
    int  n,i;
    Arg args[MAX_ARGS];
    XmString tcs;
    Pixel colorVal;
    Widget  ScrolledWin1;
    Widget RowCol1;
    Widget PushB[15];
    char name[16];


    CommonTestInit(argc, argv);
    

    XtRealizeWidget(Shell1);



    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmBOTTOM_RIGHT);  n++;
    XtSetArg(args[n], XmNautoDragModel, XmAUTO_DRAG_ENABLED); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    ScrolledWin1 = XmCreateScrolledWindow(Shell1, "ScrolledWin1", args, n);
    XtManageChild(ScrolledWin1);



    n=0;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg (args[n], XmNnumColumns, 2); n++;
    RowCol1 = XmCreateRowColumn (ScrolledWin1, "RowCol1", args, n);
    XtManageChild (RowCol1);

    for (i=0; i < 15; i++)
      {
	n=0;
	sprintf (name, "PushB%d", i);
	PushB[i] = XmCreatePushButton (RowCol1, name, args, n);
      }
    XtManageChildren (PushB, 15);

    CreateScrollBars (ScrolledWin1);
    XmScrolledWindowSetAreas (ScrolledWin1, horizSB, vertSB, RowCol1);



    CommonPause();
    CommonPause();
    CommonPause();

    n=0;
    XtSetArg (args[n], XmNautoDragModel, XmAUTO_DRAG_DISABLED); n++;
    XtSetValues (ScrolledWin1, args, n);

    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}

