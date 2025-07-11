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
static char rcsid[] = "$XConsortium: postSW3.c /main/7 1995/07/14 12:03:32 drk $"
#endif
#endif
/*
 *	file: postSW3.c
 *
 *	Program posting a scrolled window with a Drawing Area child having
 *	an arrow button as *its* child.
 */

#include <testlib.h>


Widget  ScrolledWindow1;
Widget  ArrowB1;
Widget	dialog;


static void postit(w,cad,cld)
Widget w;
XtPointer cad;
XtPointer cld;
{
	XtManageChild(ScrolledWindow1);
	XtManageChild(dialog);
}


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
	Widget		  pb;

    
    CommonTestInit(argc, argv);

	n = 0;
	pb = XmCreatePushButton(Shell1, "pb", args, n);
	XtManageChild(pb);
	XtAddCallback(pb, XmNactivateCallback, postit, NULL);

	n = 0;
	dialog = XmCreateBulletinBoardDialog(Shell1, "BBDialog", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  	n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNshadowThickness, 2);  			n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmTOP_LEFT); 	n++;
    XtSetArg(args[n], XmNdefaultPosition, False);		n++;
    XtSetArg(args[n], XmNspacing, 20);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("white"));	n++;
    XtSetArg(args[n], XmNwidth,  300);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++;
    ScrolledWindow1 = XmCreateScrolledWindow
				(dialog, "ScrolledWindow1", args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 500);				n++;
    XtSetArg(args[n], XmNheight, 500);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("coral"));	n++;
    XtSetArg(args[n], XmNforeground, CommonGetColor("black"));	n++;
    ArrowB1 = XmCreateArrowButton(ScrolledWindow1, "ArrowButton1", args, n);
    XtManageChild(ArrowB1);

    XtRealizeWidget(Shell1);
 
    XtAppMainLoop(app_context);
}
