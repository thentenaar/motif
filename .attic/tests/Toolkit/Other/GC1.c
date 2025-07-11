/* $XConsortium: GC1.c /main/4 1995/07/15 21:08:02 drk $ */
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
#include <Xm/PushBP.h>
#include <Xm/FormP.h>

Widget mainw, rc, drawpb, erasepb, highlightpb;
Arg    args[30];
int    n;
GC     rectangle_gc;

static void DrawRectangle();
static void EraseRectangle();
static void ChangeHighlight();

main(argc, argv)
int argc;
char *argv[];
{

  CommonTestInit (argc,argv);

  n = 0; 
  mainw = XmCreateMainWindow(Shell1, "mainw", args, n);
  XtManageChild(mainw);

  n = 0;
  XtSetArg (args[n], XmNhorizontalSpacing, 30); n++;
  XtSetArg (args[n], XmNwidth, 800); n++;
  XtSetArg (args[n], XmNheight, 300); n++;
  rc = XmCreateForm(mainw, "rc", args, n);
  XtManageChild(rc);

  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg (args[n], XmNleftPosition, 50); n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNheight, 100); n++;
  drawpb = XmCreatePushButton(rc, "Draw Rectangle", args, n);
  XtManageChild(drawpb);

  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, drawpb); n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNheight, 100); n++;
  erasepb = XmCreatePushButton(rc, "Erase Rectangle", args, n);
  XtManageChild(erasepb);

  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, erasepb); n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNheight, 100); n++;
  XtSetArg (args[n], XmNhighlightThickness, 20); n++;
  highlightpb = XmCreatePushButton(rc, "Change Highlight", args, n);
  XtManageChild(highlightpb);

  XtAddCallback(drawpb, XmNactivateCallback, DrawRectangle, NULL);
  XtAddCallback(erasepb, XmNactivateCallback, EraseRectangle, NULL);
  XtAddCallback(highlightpb, XmNactivateCallback, ChangeHighlight, NULL);

  XtRealizeWidget(Shell1);

  rectangle_gc = ((XmPushButtonWidget)highlightpb)->primitive.highlight_GC;
  XSetLineAttributes(XtDisplay(highlightpb), 
                     rectangle_gc, 5, 
                     LineSolid, CapButt, JoinMiter);

  CommonPause();
  XtAppMainLoop(app_context);

}

static void DrawRectangle(widget, data, callData)
Widget widget;
XtPointer data;
XtPointer callData;
{
  XDrawRectangle(XtDisplay(rc), XtWindow(rc), rectangle_gc, 25, 100, 100, 100);
}

static void EraseRectangle(widget, data, callData)
Widget widget;
XtPointer data;
XtPointer callData;
{
  XFillRectangle(XtDisplay(rc), XtWindow(rc), 
                 ((XmFormWidget)rc)->manager.background_GC, 
                 10, 85, 130, 130);
}

static void ChangeHighlight(widget, data, callData)
Widget widget;
XtPointer data;
XtPointer callData;
{
  int n;
  Arg args[2];

  n = 0;
  XtSetArg (args[n], XmNhighlightThickness, 20); n++;
  XtSetValues(highlightpb, args, n);
}

