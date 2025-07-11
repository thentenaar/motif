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
static char rcsid[] = "$XConsortium: Frame1.c /main/7 1995/07/13 18:30:46 drk $"
#endif
#endif

#include <testlib.h>

/*  Global Variables  */
Widget    RowColumn1;
Widget    Frame1;
Widget    ArrowButton1;
Widget    Frame2;
Widget    PushButton1;
Widget    Frame3;
Widget    PushButton2;

Widget rowcol,buttons[6],da[6];

void  main(argc, argv)
int     argc;
char  **argv;
{
    register int  n, i;
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
    XtSetArg(args[n], XmNwidth,  10);  				n++;
    XtSetArg(args[n], XmNheight, 10);  				n++;
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
    XtSetArg(args[n], XmNmarginHeight, 10);  			n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);  	n++;
    Frame2 = XmCreateFrame(RowColumn1, "Frame2", args, n);
    XtManageChild(Frame2);

    n = 0;
    PushButton1 = XmCreatePushButton(Frame2, "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("orange"));  	n++;
    XtSetArg(args[n], XmNborderColor, CommonGetColor("brown"));  	n++;
    XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  		n++;
    XtSetArg(args[n], XmNhighlightColor, CommonGetColor("white"));  	n++;
    XtSetArg(args[n], XmNshadowThickness, 5);  				n++;
    XtSetArg(args[n], XmNtopShadowColor, CommonGetColor("blue"));  	n++;
    XtSetArg(args[n], XmNbottomShadowColor, CommonGetColor("green"));  	n++;
    XtSetArg(args[n], XmNmarginWidth, 5);  				n++;
    XtSetArg(args[n], XmNmarginHeight, 5);  				n++;
    Frame3 = XmCreateFrame(RowColumn1, "Frame3", args, n);
    XtManageChild(Frame3);

    n = 0;
    PushButton2 = XmCreatePushButton(Frame3, "PushButton2", args, n);
    XtManageChild(PushButton2);

#ifdef REALIZE_AFTER
    XtRealizeWidget(Shell1);
#endif

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 20);  			n++;
    XtSetArg(args[n], XmNmarginHeight, 20);  			n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);  	n++;
    XtSetValues(Frame1, args, n);

    CommonPause();

    XtDestroyWidget(Frame1);

    CommonPause();

    XtDestroyWidget(RowColumn1);

    /*  Begin Test for PIR 3095 */

    n=0;
    XtSetArg(args[n],XmNnumColumns,3); n++;
    XtSetArg(args[n],XmNorientation,XmHORIZONTAL); n++;
    XtSetArg(args[n],XmNpacking,XmPACK_COLUMN); n++;
    rowcol = XmCreateRowColumn(Shell1,"rc",args,n);
    XtManageChild(rowcol);

    for (i=0; i< 5; i++){
      n=0;
      XtSetArg(args[n],XmNborderWidth,4); n++;
      buttons[i] = XmCreateFrame(rowcol,"Push Me",args,n);
      XtManageChild(buttons[i]);
    }

    for (i=0; i<5; i++){
      n=0;
      XtSetArg(args[n],XmNwidth,25); n++;
      XtSetArg(args[n],XmNheight,25); n++;
      da[i] = XmCreateDrawnButton(buttons[i],"DA",args,n);
      XtManageChild(da[i]);
    }

    CommonPause();

    XtAppMainLoop(app_context);
}
