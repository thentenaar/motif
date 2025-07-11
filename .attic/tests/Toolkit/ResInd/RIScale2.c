/* $XConsortium: RIScale2.c /main/4 1995/07/15 21:09:16 drk $ */
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
/* 
 * Original History:
 *
 * Revision 1.3.2.2  1993/04/27  17:24:48  shobana
 * 	HaL fix for CR 5523
 * 	[1993/04/27  17:24:29  shobana]
 *
 * Revision 1.3  1992/03/13  17:44:26  devsrc
 * 	Converted to ODE
 * 
*/ 
#include <testlib.h>

Widget  Scale1;

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
    XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);

    tcs = XmStringLtoRCreate("Noise Level", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 5);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 3);  n++;
    /*
    XtSetArg(args[n], XmNunitType, Xm100TH_MILLIMETERS);  n++;
    */
    XtSetArg(args[n], XmNshowValue, TRUE);  n++;
    XtSetArg(args[n], XmNvalue, -10);  n++;
    XtSetArg(args[n], XmNmaximum, 0);  n++;
    XtSetArg(args[n], XmNminimum, 100);  n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_TOP);  n++;
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetArg(args[n], XmNscaleWidth, 30);  n++;
    XtSetArg(args[n], XmNscaleHeight, 250);  n++;
    XtSetArg(args[n], XmNshadowThickness, 0);  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    Scale1 = XmCreateScale(Shell1, "Scale1", args, n);
    XtManageChild(Scale1);

    XmStringFree(tcs);

    CommonPause();

    /* test case for PIR 2688 */
    n = 0;
    XtSetArg(args[n], XmNtitleString, &tcs);	n++;
    XtGetValues(Scale1, args, n);

    printf("XmNtitleString = %s\n", CommonCsToRs(tcs));
/* Fix for CR 5523 */
    XmStringFree(tcs);
/* End Fix for CR 5523 */
    fflush(stdout);

    /* end of test case for PIR 2688 */

    CommonPause();

    tcs = XmStringLtoRCreate("Decibels", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNmaximum, 0);  n++;
    XtSetArg(args[n], XmNminimum, 100);  n++;
    XtSetArg(args[n], XmNvalue, -10);  n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM);  n++;
    XtSetArg(args[n], XmNunitType, Xm100TH_MILLIMETERS);  n++;
    XtSetArg(args[n], XmNscaleWidth, 500); n++;
    XtSetArg(args[n], XmNscaleHeight, 2000); n++;
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetValues(Scale1, args, n);

    XmStringFree(tcs);

    CommonPause();

    XtDestroyWidget(Scale1);

    CommonPause();

    XtAppMainLoop(app_context);
}
