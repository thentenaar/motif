/* $XConsortium: CommText4.c /main/6 1995/07/15 21:11:14 drk $ */
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

static char string1[] =	"One fish\nTwo fish\nRed fish\nBlue fish"; 
static char string2[] = "1234567890";

static void ShowPosition( Widget w, XtPointer client_data,
                                XtPointer call_data)
{
  printf ("The new cursor position is %d\n",
	  ((XmTextVerifyCallbackStruct *)call_data)->newInsert); 
}


void main_text4 (argc, argv)
     int argc;
     char **argv;
{
	Widget PanedWindow1;
	Widget Text1, Text2;
	Widget RowColumn1;
	Widget PButton1, PButton2, PButton3;
	Pixel red_pixel;
	Arg args[MAX_ARGS];
	int n;
        XmString tcs;

	/*  initialize toolkit  */
	CommonTestInit(argc, argv);
	 
	n = 0;
	PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1", args, n);
	XtManageChild(PanedWindow1);

	n = 0;
	XtSetArg(args[n], XmNallowResize, True); n++;
	XtSetArg(args[n], XmNborderWidth, 0); n++;
	RowColumn1 = XmCreateRowColumn(PanedWindow1, "RowColumn1", args, n);
	XtManageChild(RowColumn1);

	red_pixel = CommonGetColor("red");

	n = 0;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
	XtSetArg(args[n], XmNallowResize, True); n++;
	XtSetArg(args[n], XmNblinkRate, 0);  n++;
	XtSetArg(args[n], XmNresizeWidth, True); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 5); n++;
	XtSetArg(args[n], XmNvalue, string1 ); n++;
	XtSetArg(args[n], XmNcursorPosition, 5); n++;
	Text1 = XmCreateText(PanedWindow1, "Text1", args, n);
	XtManageChild(Text1);

	XtAddCallback(Text1, XmNlosingFocusCallback, CommonGenericCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
	XtSetArg(args[n], XmNallowResize, True); n++;
	XtSetArg(args[n], XmNvalue, string2 );n++;
	Text2 = XmCreateText(PanedWindow1, "Text2", args, n);
	XtManageChild(Text2);

	XtAddCallback(Text2, XmNlosingFocusCallback, CommonGenericCB, NULL);
	XtAddCallback(Text2, XmNmotionVerifyCallback, ShowPosition, 
                             NULL);

	n = 0;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	PButton1 = XmCreatePushButton(RowColumn1, "PButton1", args, n);
	XtManageChild(PButton1);

	n = 0;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	PButton2 = XmCreatePushButton(RowColumn1, "PButton2", args, n);
	XtManageChild(PButton2);

	n = 0;
	XtSetArg(args[n], XmNhighlightThickness, 2); n++;
	XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
	XtSetArg(args[n], XmNtraversalOn, True); n++;
	PButton3 = XmCreatePushButton(RowColumn1, "PButton3", args, n);
	XtManageChild(PButton3);

	XmAddTabGroup(Text1);
	XmAddTabGroup(Text2);
	XmAddTabGroup(RowColumn1);

	XtRealizeWidget(Shell1);

	CommonPause();	/* tab groups, losingFocus, test for PIR 3143 */
	CommonPause();	/* grow text widget 1 */

	XtAppMainLoop(app_context);
}
