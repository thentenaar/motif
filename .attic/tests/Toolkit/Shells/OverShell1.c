/* $XConsortium: OverShell1.c /main/5 1995/07/15 21:09:30 drk $ */
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

#include <testlib.h>

/* This is a test for CR 5195 */

void 
main (int argc, char **argv)
{
    Widget TestShell, PB;
    Arg args[10];
    int n;

    CommonTestInit (argc, argv);

    n=0;
    XtSetArg (args[n], XmNx, 100); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    TestShell=XtCreatePopupShell ("popup", overrideShellWidgetClass, Shell1, 
				   args, n);

 
    n=0;
    PB = XmCreatePushButton (TestShell, "PB", args ,n);
    XtManageChild (PB);
 
    XtPopup (TestShell, XtGrabNone);

    CommonPause();
     
    XtAppMainLoop (app_context);
}
