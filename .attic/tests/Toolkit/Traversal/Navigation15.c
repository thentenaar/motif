/* $XConsortium: Navigation15.c /main/5 1995/07/15 21:11:49 drk $ */
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

void FocusEvent (Widget w, XtPointer client_data, XEvent *event, Boolean *c)
{
    if (event->type == FocusIn)
	printf ("A FocusIn Event happened for %s\n", (char*)client_data);
}


main (int argc, char ** argv)
{
   Arg args[10];
   int n;
   Widget form, drawarea1, textfield1, textfield2, drawarea2, sep;

   /* initialize Toolkit */
   CommonTestInit(argc, argv);

   /* malloc space for pkids */

   n = 0;
   XtSetArg(args[n], XmNheight, 300); n++;
   XtSetArg(args[n], XmNwidth, 100); n++;
   form = XmCreateForm(Shell1, "form", args, n);
  
   n = 0;
   XtSetArg(args[n], XmNheight, 150); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   drawarea1 = XmCreateDrawingArea(form, "drawarea1", args, n);
  
   n = 0;
   XtSetArg(args[n], XmNx, 20); n++;
   XtSetArg(args[n], XmNy, 40); n++;
   XtSetArg(args[n], XmNheight, 30); n++;
   XtSetArg(args[n], XmNwidth, 50); n++;
   textfield1 = XmCreateTextField(drawarea1, "textfield1", args, n);

   n = 0;
   XtSetArg(args[n], XmNx, 20); n++;
   XtSetArg(args[n], XmNy, 80); n++;
   XtSetArg(args[n], XmNheight, 30); n++;
   XtSetArg(args[n], XmNwidth, 50); n++;
   textfield2 = XmCreateTextField(drawarea1, "textfield2", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, drawarea1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   sep = XmCreateSeparator(form, "sep", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, sep); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   drawarea2 = XmCreateDrawingArea(form, "drawarea2", args, n);
  
   XtManageChild(textfield1);
   XtManageChild(textfield2);
   XtManageChild(drawarea1);
   XtManageChild(form);
   XtManageChild(sep);
   XtManageChild(drawarea2);
   XtManageChild(form);

   n = 0;
   XtSetArg(args[n], XmNheight, 300); n++;
   XtSetArg(args[n], XmNwidth, 100); n++;
   XtSetValues(form, args, n);

   XtRealizeWidget(Shell1);

   CommonPause ();

   XtAddEventHandler(form, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"form"); 
   XtAddEventHandler(drawarea1, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"drawarea1"); 
   XtAddEventHandler(textfield1, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"textfield1"); 
   XtAddEventHandler(textfield2, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"textfield2"); 
   XtAddEventHandler(sep, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"seperator"); 
   XtAddEventHandler(drawarea2, FocusChangeMask, FALSE, FocusEvent,
		     (XtPointer)"drawarea2"); 
   n = 0;
   XtSetArg(args[n], XmNinitialFocus, NULL); n++;
   XtSetValues(drawarea1, args, n);
   XtSetValues(drawarea2, args, n);

   CommonPause ();

   n = 0;
   XtSetArg(args[n], XmNinitialFocus, 1); n++;
   XtSetValues(drawarea1, args, n);
   XtSetValues(drawarea2, args, n);

   CommonPause ();

   XtAppMainLoop (app_context);
}



