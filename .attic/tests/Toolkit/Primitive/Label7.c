/* $XConsortium: Label7.c /main/4 1995/07/15 21:08:12 drk $ */
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
#include "testlib.h"


main(argc, argv)
int argc;
char **argv;
{
    XtAppContext apc;
    char *font = "timi12";
    Widget  Label1, Label2, BBoard1;
    int     n;
    Arg     args[10];
    XmString string;


    CommonTestInit( argc, argv );

    n = 0;
    XtSetArg( args[n], XmNwidth, 350 ); n++;
    XtSetArg( args[n], XmNheight, 80 ); n++;
    BBoard1 = XmCreateForm( Shell1, "BBoard1", args, n );
    XtManageChild(BBoard1);

    string = XmStringCreate("(H - XmStringCreate", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg( args[n], XmNfontList, CommonGetFontList(font)); n++;
    XtSetArg( args[n], XmNlabelString, string ); n++;
    XtSetArg( args[n], XmNx, 10); n++;
    XtSetArg( args[n], XmNy, 0); n++;
    XtSetArg( args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    Label1 = XmCreateLabel(BBoard1, "Label1", args, n ); 

    XmStringFree( string );

    XtManageChild( Label1 );


    XtSetLanguageProc(NULL, NULL, NULL);
    string = XmStringCreateLocalized("(H - XmStringCreateLocalized");

    n = 0;
    XtSetArg( args[n], XmNfontList, CommonGetFontList(font)); n++;
    XtSetArg( args[n], XmNlabelString, string ); n++;
    XtSetArg( args[n], XmNx, 10); n++;
    XtSetArg( args[n], XmNy, 40); n++;
    XtSetArg( args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    Label2 = XmCreateLabel(BBoard1, "Label2", args, n ); 

    XmStringFree( string );

    XtManageChild(Label2);

    XtRealizeWidget(Shell1);

   /* Test CR 5216 */
    CommonPause();

    XtAppMainLoop(app_context);
}

