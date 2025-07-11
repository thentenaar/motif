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
static char rcsid[] = "$XConsortium: Scale3.c /main/6 1995/07/13 19:11:24 drk $"
#endif
#endif

#include <testlib.h>

Widget	RowColumn1,
	Scale1,
	Scale2,
	Scale3;


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
    

#ifdef DEFECT
    XtRealizeWidget(Shell1);
#endif

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);  n++;
    RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);
    XtManageChild(RowColumn1);

    n = 0;
    XtSetArg(args[n], XmNminimum, -100);  n++;
    tcs = XmStringLtoRCreate("Scale 1", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetArg(args[n], XmNshowValue, TRUE);  n++;
    XtSetArg(args[n], XmNdecimalPoints, 2);  n++;
    XtSetArg(args[n], XmNwidth, 60);  n++;
    XtSetArg(args[n], XmNheight, 200);  n++;
    XtSetArg(args[n], XmNscaleWidth, 30);  n++;
    XtSetArg(args[n], XmNscaleHeight, 200);  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    Scale1 = XmCreateScale(RowColumn1, "Scale1", args, n);
    XtManageChild(Scale1);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNminimum, -100);  n++;
    tcs = XmStringLtoRCreate("Scale 2", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetArg(args[n], XmNshowValue, TRUE);  n++;
    XtSetArg(args[n], XmNdecimalPoints, 2);  n++;
    XtSetArg(args[n], XmNwidth, 60);  n++;
    XtSetArg(args[n], XmNheight, 200);  n++;
    XtSetArg(args[n], XmNscaleWidth, 30);  n++;
    XtSetArg(args[n], XmNscaleHeight, 200);  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    Scale2 = XmCreateScale(RowColumn1, "Scale2", args, n);
    XtManageChild(Scale2);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNminimum, -100);  n++;
    tcs = XmStringLtoRCreate("Scale 3", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetArg(args[n], XmNshowValue, TRUE);  n++;
    XtSetArg(args[n], XmNdecimalPoints, 2);  n++;
    XtSetArg(args[n], XmNwidth, 60);  n++;
    XtSetArg(args[n], XmNheight, 200);  n++;
    XtSetArg(args[n], XmNscaleWidth, 30);  n++;
    XtSetArg(args[n], XmNscaleHeight, 200);  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    Scale3 = XmCreateScale(RowColumn1, "Scale3", args, n);
    XtManageChild(Scale3);

    XmStringFree(tcs);

#ifndef DEFECT
    XtRealizeWidget(Shell1);
#endif

    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}
