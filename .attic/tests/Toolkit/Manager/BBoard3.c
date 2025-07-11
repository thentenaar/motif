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
static char rcsid[] = "$XConsortium: BBoard3.c /main/7 1995/07/13 18:25:45 drk $"
#endif
#endif


#include <testlib.h>

Widget	BulletinBoard1,
	PushButtonGadget1,
	PushButtonGadget2,
	PushButtonGadget3;

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    Boolean       trace = False;
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

    n = 0;
    BulletinBoard1 = XmCreateBulletinBoard(Shell1, "BulletinBoard1", args, n);
    XtManageChild(BulletinBoard1);

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 3);  n++;
    XtSetValues(BulletinBoard1, args, n);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("red"));  n++;
    XtSetValues(BulletinBoard1, args, n);

    CommonPause();

    n = 0;
    PushButtonGadget1 = XmCreatePushButtonGadget(BulletinBoard1,
                                                 "PushButtonGadget1", args, n);
    XtManageChild(PushButtonGadget1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    XtSetArg(args[n], XmNmarginWidth, 10);  n++;
    XtSetArg(args[n], XmNmarginHeight, 5);  n++;
    PushButtonGadget2 = XmCreatePushButtonGadget(BulletinBoard1,
                                                 "PushButtonGadget2", args, n);
    XtManageChild(PushButtonGadget2);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNx, 50);  n++;
    XtSetValues(PushButtonGadget1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNy, 50);  n++;
    XtSetValues(PushButtonGadget1, args, n);

    CommonPause();

    XtDestroyWidget(PushButtonGadget2);

    n = 0;
    XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
    XtSetArg(args[n], XmNhighlightThickness, 1);  n++;
    PushButtonGadget3 = XmCreatePushButtonGadget(BulletinBoard1,
                                                 "PushButtonGadget3", args, n);
    XtManageChild(PushButtonGadget3);

    CommonPause();

    XtDestroyWidget(BulletinBoard1);
    CommonPause();

    XtAppMainLoop(app_context);
}










