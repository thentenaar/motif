/* $XConsortium: HorPanedWin1.c /main/5 1995/07/15 21:08:07 drk $ */
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


Widget  PanedWindow1 = NULL;
Widget  PushButton1 = NULL;
Widget  PushButton2 = NULL;
Widget  PushButton3 = NULL;

/*
 * This is a test for Horizontal PanedWindow
 */

void  
main (int argc, char **argv)
{
    int  n;
    Arg  args[MAX_ARGS];
    XmString           tcs = NULL;


    /*  
     * initialize toolkit  
     */

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 400);  n++;
    XtSetArg(args[n], XmNheight,300);  n++;
    XtSetArg(args[n], XmNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);
  

    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg (args[n], XmNmarginWidth, 20); n++;
    XtSetArg (args[n], XmNmarginHeight, 20); n++;
    XtSetArg (args[n], XmNsashWidth, 10); n++;
    XtSetArg (args[n], XmNsashHeight, 10); n++;
    XtSetArg (args[n], XmNsashIndent, 15); n++;
    XtSetArg (args[n], XmNsashShadowThickness, 5); n++;
    XtSetArg (args[n], XmNseparatorOn, True); n++;
    XtSetArg (args[n], XmNspacing, 25); n++;
    
    
    PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1",
                    args, n);
    XtManageChild(PanedWindow1);

    n = 0;
    PushButton1 = XmCreatePushButton(PanedWindow1, "PushButton1",
                    args, n);
    XtManageChild(PushButton1);

    n = 0;
    PushButton2 = XmCreatePushButton(PanedWindow1, "PushButton2",
                    args, n);
    XtManageChild(PushButton2);
    XmStringFree(tcs);

    n = 0;
    PushButton3 = XmCreatePushButton(PanedWindow1, "PushButton3",
                    args, n);
    XtManageChild(PushButton3);
    XmStringFree(tcs);

    XtRealizeWidget(Shell1);


    CommonPause();
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNallowResize, True);  n++;
    XtSetValues(PushButton1, args, n);

    n = 0;
    XtSetArg(args[n], XmNpaneMinimum, 100);  n++;
    XtSetArg(args[n], XmNpaneMaximum, 150);  n++;
    XtSetValues(PushButton2, args, n);

    n = 0;
    XtSetArg(args[n], XmNskipAdjust, True);  n++;
    XtSetValues(PushButton3, args, n);


    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 250); n++;
    XtSetArg(args[n], XmNheight, 250); n++;
    XtSetValues(PushButton1, args, n);

    CommonPause();

    XtAppMainLoop(app_context);
  }

