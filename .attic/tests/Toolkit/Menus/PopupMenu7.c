/* $XConsortium: PopupMenu7.c /main/5 1995/07/15 21:07:00 drk $ */
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


static void PopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (((XmPopupHandlerCallbackStruct*)call_data)->reason == XmCR_POST)
	printf("%s Posting Popup.\n", (char*)client_data);
    else
	printf("%s Unposting Popup.\n", (char*)client_data);
}

static void PostMenu(Widget w, XtPointer popup, XEvent *event, Boolean *c)
{
    XButtonEvent *ev = (XButtonEvent *)event;
    if (ev->button != Button3)
	return;
    XmMenuPosition((Widget)popup, ev);
    XtManageChild((Widget)popup);
}



int main(int argc, char *argv[])
{
  Arg args[20];
  int n;
  XmString str;
  Widget drawarea;
  Widget popup1;
  Widget title1;
  Widget text1;

Widget popup2;
XmString str2;
Widget title2;
XEvent event;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 200); n++;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);

  n = 0;
  drawarea = XmCreateDrawingArea(Shell1, "drawarea", args, n);
  XtManageChild(drawarea);

  n = 0; 
  XtSetArg(args[n], XmNx, 175); n++;
  XtSetArg(args[n], XmNy, 100); n++;
  XtSetArg(args[n], XmNwidth, 50); n++;
  text1 = XmCreateTextField(drawarea, "text1", args, n);
  XtManageChild(text1);

  XtRealizeWidget(Shell1);

  n = 0;
  XtSetArg(args[n], XmNmnemonic, "P"); n++;
  popup1 = XmCreatePopupMenu(drawarea, "popup1", args, n);

  n = 0;
  str = XmStringCreateSimple("title1");
  XtSetArg(args[n], XmNlabelString, str); n++;
  title1 = XmCreateLabel(popup1, "title1", args, n);
  XtManageChild(title1);
  XmStringFree(str);

  CommonPause();
 
  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC_RECURSIVE); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_DISABLED); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_KEYBOARD); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  /* Install a PopupHandler on the DrawArea and TextField. */

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC); n++;
  XtSetValues(popup1, args, n);
  XtAddCallback(drawarea, XmNpopupHandlerCallback,
		(XtCallbackProc)PopupCB, "DrawArea");
  XtAddCallback(text1, XmNpopupHandlerCallback,
		(XtCallbackProc)PopupCB, "TextField");
  XtAddCallback(popup1, XmNpopupHandlerCallback,
		(XtCallbackProc)PopupCB, "Popup");


  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC_RECURSIVE); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_DISABLED); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_KEYBOARD); n++;
  XtSetValues(popup1, args, n);

  CommonPause();

XtDestroyWidget(drawarea);

n = 0;
drawarea = XmCreateDrawingArea(Shell1, "drawarea", args, n);
XtManageChild(drawarea);

n = 0; 
XtSetArg(args[n], XmNx, 175); n++;
XtSetArg(args[n], XmNy, 100); n++;
XtSetArg(args[n], XmNwidth, 50); n++;
text1 = XmCreateTextField(drawarea, "text1", args, n);
XtManageChild(text1);

n = 0;
XtSetArg(args[n], XmNmnemonic, "P"); n++;
popup1 = XmCreatePopupMenu(drawarea, "popup1", args, n);
popup2 = XmCreatePopupMenu(text1, "popup2", args, n);

n = 0;
str2 = XmStringCreateSimple("Text Field");
XtSetArg(args[n], XmNlabelString, str2); n++;
title2 = XmCreateLabel(popup2, "title2", args, n);
XtManageChild(title2);
XmStringFree(str2);

n = 0;
str = XmStringCreateSimple("Drawing Area");
XtSetArg(args[n], XmNlabelString, str); n++;
title1 = XmCreateLabel(popup1, "title1", args, n);
XtManageChild(title1);
XmStringFree(str);

n = 0;
XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC); n++;
XtSetValues(popup1, args, n);
XtSetValues(popup2, args, n);
XtAddCallback(drawarea, XmNpopupHandlerCallback,
	      (XtCallbackProc)PopupCB, "DrawArea");
XtAddCallback(text1, XmNpopupHandlerCallback,
	      (XtCallbackProc)PopupCB, "TextField");
XtAddCallback(popup1, XmNpopupHandlerCallback,
	      (XtCallbackProc)PopupCB, "Popup1");
XtAddCallback(popup2, XmNpopupHandlerCallback,
	      (XtCallbackProc)PopupCB, "Popup2");

CommonPause();

n = 0;
XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_AUTOMATIC_RECURSIVE); n++;
XtSetValues(popup1, args, n);
XtSetValues(popup2, args, n);

CommonPause();

n = 0;
XtSetArg(args[n], XmNmenuPost, "<Btn3Down>"); n++;
XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_DISABLED); n++;
XtSetValues(popup1, args, n);
XtSetValues(popup2, args, n);
XtAddEventHandler(drawarea, ButtonPressMask, False, PostMenu, popup1);
XtAddEventHandler(text1, ButtonPressMask, False, PostMenu, popup2);

CommonPause();

n = 0;
XtSetArg(args[n], XmNpopupEnabled, XmPOPUP_KEYBOARD); n++;
XtSetValues(popup1, args, n);
XtSetValues(popup2, args, n);

CommonPause();

  XtAppMainLoop(app_context);
  return(1);
}
