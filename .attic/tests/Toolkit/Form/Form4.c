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
static char rcsid[] = "$XConsortium: Form4.c /main/4 1995/07/13 18:11:20 drk $"
#endif
#endif


/*
 *  Create three PushButton in a form.  The first being attached to the right
 *  and top sides of the form.  The second being attached to the left
 *  and bottom of the form and the left edge of PushButton1.
 *  The third being attached to the form on the top and on the left, its
 *  attached on the bottom by PushButton2 and on the right by PushButton1.
 *  The fourth PushButton is attached to the form on the right, attached 
 *  to the form on the bottom. It is attached to PushButton2 on the left and
 *  to PushButton1 on the top.
 */


#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

extern void FormCallback();

/*  main  */
void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget frame;
   Widget form;
   Widget PushButton1;
   Widget PushButton2;
   Widget PushButton3;
   Widget PushButton4;
   Arg args[10];
   int n;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);



   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   form = XmCreateForm (Shell1, "form", args, n);
   XtManageChild (form);
   XtAddCallback (form, XmNhelpCallback, FormCallback, NULL);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNwidth, 100);				n++;
   XtSetArg (args[n], XmNheight, 100);				n++;
   XtSetArg (args[n], XmNshadowThickness, 5);				n++;
   frame = XmCreateFrame (form, "frame", args, n);
   XtManageChild (frame);

   XtRealizeWidget (Shell1);

   CommonPause();

   XtDestroyWidget (frame);

   /*  Arguments to the first PushButton.  */

   n = 0;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   PushButton1 = XmCreatePushButton(form, "PushButton1", args, n);
   XtManageChild(PushButton1);

   CommonPause();

   /*  Arguments to the second PushButton.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNrightWidget, PushButton1);			n++;
   PushButton2 = XmCreatePushButton(form, "PushButton2", args, n);
   XtManageChild(PushButton2);

   CommonPause();

   /*  Arguments to the third PushButton.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNbottomWidget, PushButton2);		n++;
   XtSetArg (args[n], XmNrightWidget, PushButton1);		n++;
   PushButton3 = XmCreatePushButton(form, "PushButton3", args, n);
   XtManageChild(PushButton3);

   CommonPause();

   n = 0;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNleftWidget, PushButton2);		n++;
   XtSetArg (args[n], XmNtopWidget, PushButton1);		n++;
   PushButton4 = XmCreatePushButton(form, "PushButton4", args, n);
   XtManageChild(PushButton4);

   CommonPause();

   XtAppMainLoop(app_context);
}

void FormCallback (fw, closure, call_data)
XmFormWidget fw;
caddr_t closure;
caddr_t call_data;
{
   int reason;

   XmAnyCallbackStruct * call_value = (XmAnyCallbackStruct *) call_data;

   reason = call_value -> reason;

   switch (reason)
   {
      case XmCR_HELP:
         printf ("Reason = XmCR_HELP\n");
      break;

      default:
         printf ("Hit the default, incorrect reason sent!!\n");
      break;
   }
}












