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
static char rcsid[] = "$XConsortium: Scale1.c /main/6 1995/07/13 19:10:45 drk $"
#endif
#endif

#include <testlib.h>

Widget  Scale1;



void  main(argc, argv)
    int     argc;
    char  **argv;
{
    Boolean       trace = False;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    XmFontList newfontlist;
    XFontStruct *newfont;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);

    n = 0;

    /* test case for PIR 2808 - use Italics font to show bug */
    XtSetArg(args[n], XmNfontList, CommonGetFontList("helvo18")); n++;
    /* end of test case for PIR 2808 */

    XtSetArg(args[n], XmNborderWidth, 5);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 3);  n++;
    tcs = XmStringLtoRCreate("Decibels", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtitleString, tcs);  n++;
    XtSetArg(args[n], XmNshowValue, TRUE);  n++;
    XtSetArg(args[n], XmNscaleWidth, 30);  n++;
    XtSetArg(args[n], XmNscaleHeight, 250);  n++;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    Scale1 = XmCreateScale(Shell1, "Scale1", args, n);
    XtManageChild(Scale1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 400); n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetValues(Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 140);  n++;
    XtSetArg(args[n], XmNheight, 250);  n++;
    XtSetArg(args[n], XmNdecimalPoints, 2);  n++;
    XtSetArg(args[n], XmNminimum, -100); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetValues(Scale1, args, n);

    
    CommonPause();

    /* test case for PIR 2808 - use Italics font to show bug HORIZONTAL */
    n = 0;
    XtSetArg(args[n], XmNdecimalPoints, 0);  n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetValues(Scale1, args, n);

    CommonPause();

    XtAppMainLoop(app_context);
}
