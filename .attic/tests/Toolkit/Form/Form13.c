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
static char rcsid[] = "$XConsortium: Form13.c /main/4 1995/07/13 18:02:05 drk $"
#endif
#endif


/*
 *  Create three arrows in a form such that they horizontally span
 *  the form; 10% - 30%, 40 - 60% and 70% to 90%.
 */


#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Forward declarations  */

extern void FormCallback();

/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget frame;
   Widget form;
   Widget arrow1;
   Widget arrow2;
   Widget arrow3;
   Arg args[10];
   int n;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   frame = XmCreateFrame(Shell1, "frame",  NULL, 0);
   XtManageChild(frame);
   
   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   XtSetArg (args[n], XmNwidth, 200);		n++;
   XtSetArg (args[n], XmNheight, 100);		n++;
   form = XmCreateForm(frame, "form", args, n);
   XtManageChild (form);
   XtAddCallback (form, XmNhelpCallback, FormCallback, NULL);

   /*  Arguments to the first arrow.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 10);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 30);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNtopPosition, 25);			n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNbottomPosition, 75);			n++;
   arrow1 = XmCreateArrowButtonGadget(form, "arrow1", args, n);
   XtManageChild(arrow1);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 40);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 60);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNtopPosition, 25);			n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNbottomPosition, 75);			n++;
   arrow2 = XmCreateArrowButtonGadget(form, "arrow2", args, n);
   XtManageChild(arrow2);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 70);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 90);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNtopPosition, 25);			n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNbottomPosition, 75);			n++;
   arrow3 = XmCreateArrowButtonGadget(form, "arrow3", args, n);
   XtManageChild(arrow3);

   XtRealizeWidget (Shell1);

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












