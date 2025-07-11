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
static char rcsid[] = "$XConsortium: BBoard1.c /main/8 1995/07/13 18:25:20 drk $"
#endif
#endif


#include <testlib.h>

Widget	BulletinBoard1, BulletinBoard2, BulletinBoard3,
	PushButton1,
	PushButton2;
Widget	ScrollBar1,
	ArrowButton1,
	ArrowButton2;



void myHelpProc (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
printf ("A help callback was issued.\n");

}

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    Boolean       trace = False;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;

    static char translation1[] = "#augment \n\
                                  <Key>F1:     Help() ";
    /* if this is F1, the help callback is not made ! */

    static char translation2[]=  "Ctrl<Key>p:  ManagerGadgetActivate() ";

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);

    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg (args[n], XmNtranslations, XtParseTranslationTable(translation1));
    n++;
    BulletinBoard1 = XmCreateBulletinBoard(Shell1, "BulletinBoard1", args, n);
    XtManageChild(BulletinBoard1);
    XmAddTabGroup(BulletinBoard1);
    XtAddCallback (BulletinBoard1, XmNhelpCallback, myHelpProc, NULL);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 0);  n++;
    XtSetValues(BulletinBoard1, args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 0);  n++;
    XtSetValues(BulletinBoard1, args, n);
   
    n = 0;
    PushButton1 = XmCreatePushButtonGadget(BulletinBoard1, 
					   "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg(args[n], XmNx, 50);  n++;
    XtSetArg(args[n], XmNy, 150);  n++;
    PushButton2 = XmCreatePushButtonGadget(BulletinBoard1, 
					   "PushButton2", args, n);
    XtManageChild(PushButton2);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNy, 25);  n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
    ScrollBar1 = XmCreateScrollBar(BulletinBoard1, "ScrollBar1", args, n);
    XtManageChild(ScrollBar1);

    n = 0;
    XtSetArg(args[n], XmNx, 100);  n++;
    XtSetArg(args[n], XmNy, 10); n++;		/* leave room for big hilite */
    XtSetArg(args[n], XmNwidth, 50);  n++;
    XtSetArg(args[n], XmNheight, 50);  n++;
    ArrowButton1 = XmCreateArrowButton(BulletinBoard1, 
				       "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNx, 120);  n++;
    XtSetArg(args[n], XmNy, 230);  n++;
    ArrowButton2 = XmCreateArrowButton(BulletinBoard1, "ArrowButton2", args,
                                       n);
    XtManageChild(ArrowButton2);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
    XtSetArg(args[n], XmNy, 20);  n++;
    XtSetArg(args[n], XmNheight, 29);  n++;
    XtSetArg(args[n], XmNwidth, 40);  n++;
    XtSetValues(ArrowButton2, args, n);

    n = 0;
    XtSetArg(args[n], XmNheight, 25);  n++;
    XtSetArg(args[n], XmNvalue, 70);  n++;
    XtSetArg(args[n], XmNsliderSize, 30);  n++;
    XtSetValues(ScrollBar1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 150);  n++;
    XtSetArg(args[n], XmNheight, 42);  n++;
    XtSetValues(PushButton2, args, n);

    CommonPause();

    /* begin test for PIR 3256 */

    XtDestroyWidget (PushButton1);
    XtDestroyWidget (PushButton2);
    XtDestroyWidget (ScrollBar1);
    XtDestroyWidget (ArrowButton1);
    XtDestroyWidget (ArrowButton2);

    n = 0;
    XtSetArg (args[n], XmNshadowThickness, 10); n++;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    BulletinBoard2 = XmCreateBulletinBoard (BulletinBoard1, 
					    "BulletinBoard2", args, n);

    n = 0;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNx, 105); n++;
    BulletinBoard3 = XmCreateBulletinBoard (BulletinBoard1,
					    "BulletinBoard3", args, n);

  
    XtManageChild (BulletinBoard2);
    XtManageChild (BulletinBoard3);  
  
    CommonPause();

    XtDestroyWidget (BulletinBoard2);
    XtDestroyWidget (BulletinBoard3);

    n = 0;
    XtSetArg (args[n], XmNshadowThickness, 10); n++;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    BulletinBoard2 = XmCreateFrame (BulletinBoard1, 
				    "BulletinBoard2", args, n);

    n = 0;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNx, 105); n++;
    BulletinBoard3 = XmCreateFrame (BulletinBoard1,
				    "BulletinBoard3", args, n);

  
    XtManageChild (BulletinBoard2);
    XtManageChild (BulletinBoard3);  

    CommonPause();

    /* end test for PIR 3256 */

    XtAppMainLoop(app_context);
}










