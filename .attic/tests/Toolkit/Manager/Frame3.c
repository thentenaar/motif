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
static char rcsid[] = "$XConsortium: Frame3.c /main/7 1995/07/13 18:32:08 drk $"
#endif
#endif

#include <testlib.h>

Widget    RowColumn1;
Widget    Frame1;
Widget    ArrowButton1;
Widget    Frame2;
Widget    Label1;
Widget    Frame3;
Widget    PushButton1;
Widget    Frame4;
Widget    ToggleButton1;
Widget    Frame5;
Widget    LabelGadget1;


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
    XtSetArg(args[n], XmNwidth,  400);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++;
    XtSetValues(Shell1, args, n);
    

#ifndef REALIZE_AFTER
    XtRealizeWidget(Shell1);
#endif 

    n = 0;
    RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);
    XtManageChild(RowColumn1);

    n = 0;
    Frame1 = XmCreateFrame(RowColumn1, "Frame1", args, n);
    XtManageChild(Frame1);

    n = 0;
    ArrowButton1 = XmCreateArrowButton(Frame1, "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 50);  			n++;
    XtSetArg(args[n], XmNmarginHeight, 50);  			n++;
    Frame2 = XmCreateFrame(RowColumn1, "Frame2", args, n);
    XtManageChild(Frame2);

    tcs = XmStringLtoRCreate("LABEL1", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);  			n++;
    Label1 = XmCreateLabel(Frame2, "Label1", args, n);
    XtManageChild(Label1);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 30);  			n++;
    XtSetArg(args[n], XmNmarginHeight, 10);  			n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);  	n++;
    Frame3 = XmCreateFrame(RowColumn1, "Frame3", args, n);
    XtManageChild(Frame3);

    n = 0;
    PushButton1 = XmCreatePushButton(Frame3, "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 10);  			n++;
    XtSetArg(args[n], XmNmarginHeight, 10);  			n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_IN);  		n++;
    Frame4 = XmCreateFrame(RowColumn1, "Frame4", args, n);
    XtManageChild(Frame4);

    n = 0;
    ToggleButton1 = XmCreateToggleButton(Frame4, "ToggleButton1", args, n);
    XtManageChild(ToggleButton1);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 10);  			n++;
    XtSetArg(args[n], XmNmarginHeight, 10);  			n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);  		n++;
    Frame5 = XmCreateFrame(RowColumn1, "Frame5", args, n);
    XtManageChild(Frame5);

    tcs = XmStringLtoRCreate("LABELGADGET1", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);  			n++;
    LabelGadget1 = XmCreateLabelGadget(Frame5, "LabelGadget1", args, n);
    XtManageChild(LabelGadget1);

    XmStringFree(tcs);

#ifdef REALIZE_AFTER
    XtRealizeWidget(Shell1);
#endif

    CommonPause();

    XtAppMainLoop(app_context);
}
