/* $XConsortium: autopopups.c /main/5 1995/07/15 20:45:56 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <Xm/XmP.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawingA.h>

XtAppContext context;
#define APP_CLASS "AutomaticPopups"

Widget exitpb;

void QuitAppl(Widget, char *, XmPushButtonCallbackStruct *);
void ExposeCB(Widget, char *, XmDrawingAreaCallbackStruct *);
void ButtonEventHandler(Widget, XtPointer, XEvent*);
void PopupHandlerCB(Widget, XtPointer, XmPopupHandlerCallbackStruct *);
void addButton(Widget, char*);
void addChildren(Widget, char*);

int circlex, circley, circled;
int rectx, recty, recth, rectw;

Widget	popup1, popup2, popup3, popup4, popup5;

int main(int argc, char **argv)
{
  Widget        shell;
  Widget        frame1, frame2;
  Widget	da1, da2;
  Widget	btn1, btn2, label1, label2, btn3, sbtn1;
  Widget	explain;
  Arg		args[10];
  int		count;
  XmString	str, tmp1, tmp2;

  shell     = XtVaAppInitialize(&context, APP_CLASS, NULL, 0, &argc, argv,
				NULL, NULL);

  frame1 = XmCreateFrame(shell, "outerframe", NULL, 0);
  XtManageChild(frame1);

  count = 0;
  XtSetArg(args[count], XmNpopupEnabled, XmPOPUP_AUTOMATIC_RECURSIVE); count++;
  popup1 = XmCreatePopupMenu(frame1, "popupOF", args, count);

  addChildren(popup1, "OuterFrameMenu");

  str = XmStringCreateLocalized("OuterFrame");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNframeChildType, XmFRAME_TITLE_CHILD); count++;
  label1 = XmCreateLabel(frame1, "label1", args, count);
  XtManageChild(label1);
  XmStringFree(str);

  count = 0;
  XtSetArg(args[count], XmNheight, 500); count++;
  XtSetArg(args[count], XmNwidth, 500); count++;
  XtSetArg(args[count], XmNframeChildType, XmFRAME_WORKAREA_CHILD); count++;
  XtSetArg(args[count], XmNresizePolicy, XmRESIZE_NONE); count++;
  da1 = XmCreateDrawingArea(frame1, "da1", args, count);
  XtManageChild(da1);

  count = 0;
  XtSetArg(args[count], XmNx, 100); count++;
  XtSetArg(args[count], XmNy, 100); count++;
  frame2 = XmCreateFrame(da1, "innerframe", args, count);
  XtManageChild(frame2);

  count = 0;
  XtSetArg(args[count], XmNpopupEnabled, XmPOPUP_AUTOMATIC_RECURSIVE); count++;
  popup2 = XmCreatePopupMenu(frame2, "popupIF", args, count);

  addChildren(popup2, "InnerFrameMenu");
  addButton(popup2, "Make me different");

  str = XmStringCreateLocalized("InnerFrame");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNframeChildType, XmFRAME_TITLE_CHILD); count++;
  label2 = XmCreateLabel(frame2, "label2", args, count);
  XtManageChild(label2);
  XmStringFree(str);

  count = 0;
  XtSetArg(args[count], XmNheight, 300); count++;
  XtSetArg(args[count], XmNwidth, 300); count++;
  XtSetArg(args[count], XmNframeChildType, XmFRAME_WORKAREA_CHILD); count++;
  XtSetArg(args[count], XmNresizePolicy, XmRESIZE_NONE); count++;
  da2 = XmCreateDrawingArea(frame2, "da2", args, count);
  XtManageChild(da2);
  XtAddCallback(da2, XmNexposeCallback, (XtCallbackProc) ExposeCB, NULL);
  XtAddCallback(da2, XmNpopupHandlerCallback, 
		(XtCallbackProc) PopupHandlerCB, NULL);

  count = 0;
  str = XmStringCreateLocalized(
	"Click Button3 anywhere to cause a popup menu");
  tmp1 = XmStringConcat(str, XmStringSeparatorCreate());
  XmStringFree(str);
  tmp2 = XmStringCreateLocalized(
         "to be posted.  Note that the posted menu");
  str = XmStringConcat(tmp1, tmp2);
  XmStringFree(tmp1);
  XmStringFree(tmp2);
  tmp1 = XmStringConcat(str, XmStringSeparatorCreate());
  XmStringFree(str);
  tmp2 = XmStringCreateLocalized(
         "differs depending on where you click.");
  str = XmStringConcat(tmp1, tmp2);
  XmStringFree(tmp1);
  XmStringFree(tmp2);

  XtSetArg(args[count], XmNx, 10); count++;
  XtSetArg(args[count], XmNy, 200); count++;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNalignment, XmALIGNMENT_BEGINNING); count++;
  explain = XmCreateLabelGadget(da2, "Explain", args, count);
  XtManageChild(explain);
  XmStringFree(str);

  circlex = 40; circley = 120; circled = 30;
  rectx = 80; recty = 120; recth = 30; rectw = 60;

  str = XmStringCreateLocalized("Quit");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNx, 40); count++;
  XtSetArg(args[count], XmNy, 40); count++;
  btn1 = XmCreatePushButton(da2, "pb1", args, count);
  XtManageChild(btn1);
  XtAddCallback(btn1, XmNactivateCallback, 
		(XtCallbackProc) QuitAppl, NULL);

  str = XmStringCreateLocalized("Another Button With no Menu");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNx, 40); count++;
  XtSetArg(args[count], XmNy, 80); count++;
  btn3 = XmCreatePushButton(da2, "pb3", args, count);
  XtManageChild(btn3);

  count = 0;
  XtSetArg(args[count], XmNpopupEnabled, XmPOPUP_AUTOMATIC); count++;
  popup4 = XmCreatePopupMenu(btn1, "popupBtn1", args, count);

  addChildren(popup4, "SharedMenu");
  addButton(popup4, "Do something more");
  addButton(popup4, "What else?");


  str = XmStringCreateLocalized("PushMe");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNx, 40); count++;
  XtSetArg(args[count], XmNy, 40); count++;
  btn2 = XmCreatePushButton(da1, "pb2", args, count);
  XtManageChild(btn2);

  str = XmStringCreateLocalized("I share a menu");
  count = 0;
  XtSetArg(args[count], XmNlabelString, str); count++;
  XtSetArg(args[count], XmNx, 240); count++;
  XtSetArg(args[count], XmNy, 40); count++;
  sbtn1 = XmCreatePushButton(da1, "sharepb1", args, count);
  XtManageChild(sbtn1);

  /* Associate a menu with this button */
  XmAddToPostFromList(popup4, sbtn1);

  count = 0;
  XtSetArg(args[count], XmNpopupEnabled, XmPOPUP_AUTOMATIC); count++;
  popup3 = XmCreatePopupMenu(btn2, "popupBtn2", args, count);

  addChildren(popup3, "Button2Menu");
  addButton(popup3, "Do something more");
  
  count = 0;
  XtSetArg(args[count], XmNpopupEnabled, True); count++;
  popup4 = XmCreatePopupMenu(da2, "PopupObjects1", args, count);

  addChildren(popup4, "ObjectMenu");
  addButton(popup4, "Flip");
  addButton(popup4, "Size");
  addButton(popup4, "Stretch");
  
  XtRealizeWidget(shell);

  XtAppMainLoop(context);

  return 0;    /* make compiler happy */
}


void 
QuitAppl(Widget w, char *i, XmPushButtonCallbackStruct *e)
{
  exit(0);
}

void 
addChildren(Widget popup, char* label)
{
  XmString str;
  Arg args[2];
  Widget child;

  str = XmStringCreateLocalized(label);
  XtSetArg(args[0], XmNlabelString, str);
  child = XmCreateLabelGadget(popup, "Title", args, 1);
  XtManageChild(child);
  XmStringFree(str);

  child = XmCreateSeparatorGadget(popup, "Sep", NULL, 0);
  XtManageChild(child);

  addButton(popup, "Cut");
  addButton(popup, "Copy");
  addButton(popup, "Paste");

}

void 
addButton(Widget parent, char* label) 
{
  Widget ch;
  XmString str;
  Arg args[2];

  str = XmStringCreateLocalized(label);
  XtSetArg(args[0], XmNlabelString, str);
  ch = XmCreatePushButtonGadget(parent, label, args, 1);
  XtManageChild(ch);
  XmStringFree(str);

}

GC drawGC = NULL;

void 
ExposeCB(Widget da, char *ig, XmDrawingAreaCallbackStruct *cbs)
{
  Arg args[2];
  XGCValues gcv;
  Pixel fg;
  Pixel bg;

  if (drawGC == NULL) {
    XtSetArg (args[0], XmNforeground, &fg);
    XtSetArg (args[1], XmNbackground, &bg);
    XtGetValues (da, args, 2);
    gcv.foreground = fg;
    gcv.background = bg;
    gcv.line_width = 2;
    drawGC = XtGetGC (da, GCForeground | GCBackground | GCLineWidth, &gcv);
  }

  XDrawArc(XtDisplay(da), XtWindow(da), drawGC,
	   circlex, circley, circled, circled, 
	   0, (360 * 64));
  XDrawRectangle(XtDisplay(da), XtWindow(da), drawGC, 
		 rectx, recty, rectw, recth);

}

void 
PopupHandlerCB(Widget da, XtPointer ignore, XmPopupHandlerCallbackStruct *phcs)
{
  int radius;
  int dx, dy;
  XButtonEvent *bevent = (XButtonEvent *) phcs -> event;
  int eventx, eventy;
  Position wox, woy;

  /* Translate event coordinates to this widget */
  /* First get origin of this widget */
  XtTranslateCoords(da, 0, 0, &wox, &woy);

  /* Now calculate relative offset */
  eventx = bevent -> x_root - wox;
  eventy = bevent -> y_root - woy;
  
  /* Check for hit in circle or rectangle to post special 
     menus */

  dx = eventx - rectx;
  dy = eventy - recty;

  if (dx > 0 && dx < rectw &&
      dy > 0 && dy < recth) {
    phcs -> menuToPost = popup4;
    return;
  }

  dx = eventx - circlex;
  dy = eventy - circley;

  dx -= circled / 2;
  dy -= circled / 2;

  radius = (dx * dx) + (dy * dy);

  if (radius < ((circled * circled) / 4))
    phcs -> menuToPost = popup4;
}
