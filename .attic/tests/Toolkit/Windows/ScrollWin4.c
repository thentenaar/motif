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
static char rcsid[] = "$XConsortium: ScrollWin4.c /main/10 1995/07/13 19:49:47 drk $"
#endif
#endif

#include <testlib.h>

Widget DrawingArea1;
Widget ScrolledWindow1;
Widget ArrowB1, ArrowB2;
Widget vert, horiz;
Widget Frame1;

static void actCB(Widget w, XtPointer client_data,
	XtPointer call_data)
{
	printf("Activate Callback made for %s\n", (char *)client_data);
}

void main(argc, argv)
int argc;
char **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    
    CommonTestInit(argc, argv);

#ifdef	MOTIF1_1
    n = 0;
    XtSetArg(args[n], XmNwidth, 10); n++;
    XtSetArg(args[n], XmNheight, 10); n++;
    XtSetValues(Shell1, args, n);
#endif
    XtRealizeWidget(Shell1);

    n = 0;
    Frame1 = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame1);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  	n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNshadowThickness, 2);  			n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmTOP_LEFT); 	n++;
    XtSetArg(args[n], XmNspacing, 20);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("white"));	n++;
    XtSetArg(args[n], XmNwidth,  300);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++;
    ScrolledWindow1 = XmCreateScrolledWindow(Frame1,
	"ScrolledWindow1", args, n);

    XtRealizeWidget(ScrolledWindow1);
    XtManageChild(ScrolledWindow1);

/* test case for PIR 4276 */
/* 
 * Removed explicit calls to set traversalOn to True
 * that were added at the beginning of 1.2 - shouldn't
 * need them.
 */
/* end of test case for PIR 4276 */

    n = 0;
    DrawingArea1 = XmCreateDrawingArea
				(ScrolledWindow1, "DrawingArea", args, n); 
    XtManageChild(DrawingArea1);

    n = 0;
    XtSetArg(args[n], XmNwidth, 250);				n++;
    XtSetArg(args[n], XmNheight, 250);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("coral"));	n++;
    XtSetArg(args[n], XmNforeground, CommonGetColor("black"));	n++;
    ArrowB1 = XmCreateArrowButton(DrawingArea1, "ArrowButton1", args, n);
    XtManageChild(ArrowB1);

    XtAddCallback(ArrowB1, XmNactivateCallback, actCB, 
		  (XtPointer) "ArrowB1");

    n = 0;
    XtSetArg(args[n], XmNy, 260);				n++;
    XtSetArg(args[n], XmNwidth, 250);				n++;
    XtSetArg(args[n], XmNheight, 250);				n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("violet"));	n++;
    XtSetArg(args[n], XmNforeground, CommonGetColor("black"));	n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);		n++;
    ArrowB2 = XmCreateArrowButton(DrawingArea1, "ArrowButton2", args, n);
    XtManageChild(ArrowB2);

    XtAddCallback(ArrowB2, XmNactivateCallback, (XtCallbackProc) actCB, 
		  (XtPointer) "ArrowB2");
 
    CommonPause();  

    n = 0;
    XtSetArg(args[n], XmNwidth, 500);			n++;
    XtSetArg(args[n], XmNheight, 500);			n++;
    XtSetValues(ArrowB1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 500);			n++;
    XtSetArg(args[n], XmNheight, 500);			n++;
    XtSetArg(args[n], XmNy, 510);			n++;
    XtSetValues(ArrowB2, args, n);			

    CommonPause();  

    XtUnmanageChild(ArrowB1);

    CommonPause();

    XtUnmanageChild(ArrowB2);
    XtUnmanageChild(DrawingArea1);

    CommonPause();

    XtManageChild(DrawingArea1);
    XtManageChild(ArrowB1);
    XtManageChild(ArrowB2);
  
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 175);		n++;
    XtSetArg(args[n], XmNheight, 175);		n++;
    XtSetValues(ArrowB1, args, n);

    XtSetArg(args[n], XmNy, 185);		n++;
    XtSetValues(ArrowB2, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 125);		n++;
    XtSetArg(args[n], XmNheight, 125);		n++;
    XtSetValues(ScrolledWindow1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 250);		n++;
    XtSetArg(args[n], XmNheight, 250);		n++;
    XtSetValues(ScrolledWindow1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);		n++;
    XtSetArg(args[n], XmNheight, 100);		n++;
    XtSetValues(ArrowB1, args, n);

    XtSetArg(args[n], XmNy, 110);		n++;
    XtSetValues(ArrowB2, args, n);

    CommonPause();

    XtUnmapWidget(DrawingArea1);

    CommonPause();

    XtMapWidget(DrawingArea1);

    CommonPause();

    /* test case for PIR 3580 */
    n = 0;
    XtSetArg(args[n], XmNwidth, 300);           n++;
    XtSetArg(args[n], XmNheight, 300);          n++;
    XtSetValues(ArrowB1, args, n);

    XtSetArg(args[n], XmNy, 310);		n++;
    XtSetValues(ArrowB2, args, n);

    n = 0;
    XtSetArg(args[n], XmNverticalScrollBar, &vert);		n++;
    XtSetArg(args[n], XmNhorizontalScrollBar, &horiz);		n++;
    XtGetValues(ScrolledWindow1, args, n);

    n = 0;
    XtSetArg(args[n], XmNtraversalOn, False);	n++;
    XtSetValues(vert, args, n);
    XtSetValues(horiz, args, n);

    CommonPause();

    /* end of test case for PIR 3580 */


    XtAppMainLoop(app_context);
}
