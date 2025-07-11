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
static char rcsid[] = "$XConsortium: FormDia1.c /main/4 1995/07/13 18:12:47 drk $"
#endif
#endif

/*
 *  Create 2 push buttons in a form.  SetValues on the constraints to
 *  try out the difference attachment types.  Also type some invalid
 *  conditions.
 */

#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>

Widget form;
Widget push1;
Widget push2;

extern void PushBCallback();

void main (argc, argv)
unsigned int argc;
char **argv;
{
   Arg args[10];
   int n;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   XtSetArg (args[n], XmNheight, 35); n++;
   XtSetArg (args[n], XmNwidth, 50); n++;
   form = XmCreateForm (Shell1, "form", args, n);
   XtManageChild (form);

   /*  Arguments to the first frame.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   push1 = XmCreatePushButton (form, "push1", args, n);
   XtManageChild (push1);

   XtAddCallback (push1, XmNactivateCallback, PushBCallback, form);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopOffset, 10);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, push1);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   push2 = XmCreatePushButton (form, "push2", args, n);
   XtManageChild (push2);

   /* XtAddCallback (push2, XmNactivateCallback, PushBCallback, form); */

   XtRealizeWidget (Shell1);

   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();

   XtAppMainLoop(app_context);
}

void PushBCallback (pw, form, closure)
Widget pw;
Widget form;
caddr_t closure;

{
   static count = 0;
   Arg args[10];
   int n;
   unsigned char top_attachment;

   if (count == 0)
   {
      printf ("Change push2's attach widget to NULL.\n");
      n = 0;
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_NONE);	n++;
      XtSetArg (args[n], XmNtopWidget, NULL);			n++;
      XtSetValues (push2, args, n);

      n = 0;
      XtSetArg (args[n], XmNtopAttachment, &top_attachment);	n++;
      XtGetValues (push2, args, n);
      printf ("The attachment of pushbutton 2 expected 0, now %d\n", 
		(int) top_attachment);

   }      
   else if (count == 1)
   {
      printf ("change push2's top attachment to opposite of push 1\n");
      n = 0;
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
      XtSetArg (args[n], XmNtopWidget, push1);				n++;
      XtSetValues (push2, args, n);
   }
   else if (count == 2)
   {
      printf ("Change push2's attach widget to NULL.\n");
      n = 0;
      XtSetArg (args[n], XmNtopWidget, NULL);			n++;
      XtSetValues (push2, args, n);
   

   }
   else if (count == 3)
   {
      printf ("change the top attachment to attach widget.\n");

      n = 0;
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
      XtSetArg (args[n], XmNtopWidget, push1);			n++;
      XtSetValues (push2, args, n);
      n = 0;
      XtSetArg (args[n], XmNwidth, 200);			n++;
      XtSetValues (push1, args, n);
   

   }
   else if (count == 4)
   {
      printf ("set resizable to False and change the width of the child.\n");

      n = 0;
      XtSetArg (args[n], XmNallowShellResize, False);		n++;
      XtSetValues (Shell1, args, n);

      n = 0;
      XtSetArg (args[n], XmNwidth, 300);			n++;
      XtSetValues (push2, args, n);
   }
   else if (count == 5)
   {
      printf ("Set push2 to attach opposite on left and right.\n");

      n = 0;
      XtSetArg (args[n], XmNallowShellResize, True);		n++;
      XtSetValues (Shell1, args, n);

      n = 0;
      XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
      XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
      XtSetArg (args[n], XmNleftWidget, push1);				n++;
      XtSetArg (args[n], XmNrightWidget, push1);			n++;
      XtSetValues (push2, args, n);
   }
   else if (count == 6)
   {
      Widget form_dialog;

      printf ("Create a form dialog\n");

      n = 0;

      form_dialog = XmCreateFormDialog (Shell1, "form_dialog", args, n);
      XtManageChild (form_dialog);
   }

   count++;
}












