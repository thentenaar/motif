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
static char rcsid[] = "$XConsortium: ScrollWin5.c /main/8 1995/07/13 19:50:28 drk $"
#endif
#endif

#include <testlib.h>

static XtCallbackProc  ResizeDrawAreaCB();

Widget  DrawingArea1;
Widget  ScrolledWindow1;
Widget  ArrowB1, ArrowB2, ArrowB3, ArrowB4;
Widget  Form;



void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;

    CommonTestInit(argc, argv);

#ifdef MOTIF1_1
#ifdef AUTOMATION
    n = 0;
    XtSetArg(args[n], XmNwidth, 200); n++;
    XtSetArg(args[n], XmNheight, 200); n++;
    XtSetValues(Shell1, args, n);
#endif
#endif

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);			n++;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE);			n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmBOTTOM_LEFT); 		n++; 
    XtSetArg(args[n], XmNbackground, CommonGetColor("MediumAquamarine")); n++;
    XtSetArg(args[n], XmNwidth, 200);					n++;
    XtSetArg(args[n], XmNheight, 200);					n++;
    ScrolledWindow1 = XmCreateScrolledWindow
				(Shell1, "ScrolledWindow1", args, n);
    XtManageChild(ScrolledWindow1);

    n = 0;
    Form = XmCreateForm(ScrolledWindow1, "Form", args, n);
    XtManageChild(Form);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNheight, 200);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("coral"));	n++;
    ArrowB1 = XmCreateArrowButton(Form, "ArrowButton1", args, n);
    XtManageChild(ArrowB1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, ArrowB1);			n++;
    XtSetArg(args[n], XmNleftOffset, 10);			n++;
    XtSetArg(args[n], XmNwidth, 200);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("violet"));	n++;
    ArrowB2 = XmCreateArrowButton(Form, "ArrowButton2", args, n);
    XtManageChild(ArrowB2);

    n = 0;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++; 
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
    XtSetArg(args[n], XmNleftWidget, ArrowB1);      		n++;
    XtSetArg(args[n], XmNleftOffset, 10);	      		n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ArrowB2);			n++;
    XtSetArg(args[n], XmNtopOffset, 10);			n++;
    XtSetArg(args[n], XmNwidth, 150);				n++;
    XtSetArg(args[n], XmNheight, 150);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Yellow"));	n++;
    ArrowB3 = XmCreateArrowButton(Form, "ArrowButton3", args, n);
    XtManageChild(ArrowB3);

    XtRealizeWidget(Shell1);
 
    CommonPause();  

    n = 0;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_LEFT);		n++;
    XtSetArg(args[n], XmNbottomOffset, 25);			n++;
    XtSetArg(args[n], XmNtopOffset, 25);			n++;
    XtSetValues(ArrowB3, args, n);

    CommonPause();

    XtUnmanageChild(ArrowB3);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;
    XtSetValues(ArrowB3, args, n);

    n = 0;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, ArrowB3);			n++;
    XtSetArg(args[n], XmNtopOffset, 10);			n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("White"));	n++;
    ArrowB4 = XmCreateArrowButton(Form, "ArrowButton4", args, n);
    XtManageChild(ArrowB4);

    CommonPause();

    XtManageChild(ArrowB3);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 300);				n++;
    XtSetArg(args[n], XmNheight, 300);				n++;
    XtSetValues(ScrolledWindow1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 400);				n++;
    XtSetArg(args[n], XmNheight, 400);				n++;
    XtSetValues(Form, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNx, 50);				n++;
    XtSetArg(args[n], XmNy, 50);				n++;
    XtSetValues(Form, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);				n++;
    XtSetArg(args[n], XmNheight, 100);				n++;
    XtSetValues(ScrolledWindow1, args, n);

    CommonPause();

    XtAppMainLoop(app_context);
}
