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
static char rcsid[] = "$TOG: TrackingLocM1.c /main/9 1999/11/22 18:16:36 jff $"
#endif
#endif

#include <testlib.h>
#include <X11/cursorfont.h>

/* forward refs */
static void TrackCB();

Widget  Dialog;
Widget 	PanedW;
Widget	Toggle1, Toggle2, Toggle3;
Widget  List;
Widget	Push1, Push2, Push3, Push4;
Widget  Arrow1, Arrow2, Arrow3;
Widget 	w_id;

int   	cur_num = 1;
char 	begin[50] = "Help has arrived (via XmTrackingLocate) for ";


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Pixel         colorVal;
    XmString      list_items[2];
    

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg (args[n], XmNwidth, 400); n++;
    XtSetArg (args[n], XmNheight, 500); n++;
    XtSetArg (args[n], XmNmappedWhenManaged, False); n++;
    XtSetValues(Shell1, args, n);
    
    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg (args[n], XmNautoUnmanage, False); n++;
    XtSetArg (args[n], XmNgeometry, "+0+0"); n++;
    Dialog = XmCreateBulletinBoardDialog (Shell1, "Dialog", args, n);


    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 20);			n++;
    XtSetArg(args[n], XmNmarginHeight, 20);			n++;
    PanedW = XmCreatePanedWindow(Dialog, "PanedWindow", args, n);
    XtManageChild(PanedW);

#ifndef BESTCASE                            /* to get us around P2986 */
    XtManageChild (Dialog);
#endif

    n = 0;
    Push1 = XmCreatePushButton(PanedW, "Push1", args, n);
    XtManageChild(Push1);

    n = 0;
    Push2 = XmCreatePushButton(PanedW, "Push2", args, n);
    XtManageChild(Push2);

    n = 0;
    Push3 = XmCreatePushButton(PanedW, "Push3", args, n);
    XtManageChild(Push3);

    n = 0;
    Toggle1 = XmCreateToggleButton(PanedW, "Toggle1", args, n);
    XtManageChild(Toggle1);

    n = 0;
    Toggle2 = XmCreateToggleButton(PanedW, "Toggle2", args, n);
    XtManageChild(Toggle2);

    n = 0;
    Toggle3 = XmCreateToggleButton(PanedW, "Toggle3", args, n);
    XtManageChild(Toggle3);

    list_items[0] = XmStringCreateSimple("List item 1");
    list_items[1] = XmStringCreateSimple("List item 2");

    n = 0;
    XtSetArg(args[n], XmNx, 5), n++;
    XtSetArg(args[n], XmNy, 50), n++;
    XtSetArg(args[n], XmNitems, list_items), n++;
    XtSetArg(args[n], XmNitemCount, XtNumber(list_items)), n++;
    XtSetArg(args[n], XmNvisibleItemCount, 5), n++;
    List = XmCreateScrolledList(PanedW, "List", args, n);
    XtManageChild(List);

    n = 0;
    Arrow1 = XmCreateArrowButton(PanedW, "Arrow1", args, n);
    XtManageChild(Arrow1);

    n = 0;
    Arrow2 = XmCreateArrowButton(PanedW, "Arrow2", args, n);
    XtManageChild(Arrow2);

    n = 0;
    Arrow3 = XmCreateArrowButton(PanedW, "Arrow3", args, n);
    XtManageChild(Arrow3);

    tcs = XmStringCreateLtoR("Push To Turn on Tracking Locate", 
			XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("Yellow"));  	n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Push4 = XmCreatePushButton(PanedW, "Push4", args, n);
    XtManageChild(Push4);

    XtAddCallback(Push4, XmNactivateCallback, TrackCB, (XtPointer) 0);

    XmStringFree(tcs);

#ifdef BESTCASE
    XtManageChild (Dialog);
#endif

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    XtRemoveCallback(Push4, XmNactivateCallback, TrackCB, (XtPointer) 0);
    XtAddCallback(Push4, XmNactivateCallback, TrackCB, (XtPointer) 1);

    CommonPause();


    XtAppMainLoop(app_context);
}


static void TrackCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    Cursor 	cursor;
	

    switch (cur_num) {
	case 1: 	cursor = XCreateFontCursor (display, XC_pirate);
			break;
	case 2:		cursor = XCreateFontCursor (display, XC_heart);
			break;
	case 3:		cursor = XCreateFontCursor (display, XC_man);
			break;
	case 4:		cursor = XCreateFontCursor (display, XC_shuttle);
			break;
	default:	cursor = XCreateFontCursor (display, XC_arrow);
			break;
    };
    w_id = XmTrackingLocate(Dialog, cursor, FALSE);

    /* support two modes. Help() for regular testing; XtCallActionProc
       illustrates PIR 3133 */

    if ((long) client_data == 0)
       Help(w_id);
    else {
      if (w_id)
        XtCallActionProc (w_id, "Help", 
			  ((XmAnyCallbackStruct *)call_data)->event, NULL, 0);
    }

    cur_num++;
}


Help(widget)
Widget widget;
{
    int 	n = 0;
    Arg 	args[5];
    char	to[150];
    XmString	tcs;
    Widget 	MessageB;

    if (widget == NULL)
      return;
    else {
       strcpy(to, begin);
       strcat(to, XtName(widget));

       tcs = XmStringCreateLtoR(to, XmSTRING_DEFAULT_CHARSET);
       n = 0;
       XtSetArg(args[n], XmNx, 0);			n++;
       XtSetArg(args[n], XmNy, 0);			n++;
       XtSetArg(args[n], XmNmessageString, tcs);	n++;
       MessageB = XmCreateInformationDialog(Shell1, "MessageB", args, n);
       XtManageChild(MessageB);
    }
}
