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
static char rcsid[] = "$XConsortium: Form3.c /main/4 1995/07/13 18:10:55 drk $"
#endif
#endif

/*
 *  Create a push button in a form.  SetValues on a bunch of different
 *  things.  Used to hit branches in SetValues.
 */


#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Forward declarations  */

extern void PushBCallback();
extern char * UserData;

/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget form;
   Widget pushb;
   Arg args[10];
   int n;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   form = XmCreateForm (Shell1, "form", args, n);
   XtManageChild (form);

   /*  Arguments to the first frame.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNwidth, 100);				n++;
   XtSetArg (args[n], XmNheight, 100);				n++;
   pushb = XmCreatePushButton (form, "pushb", args, n);
   XtManageChild (pushb);

   XtAddCallback (pushb, XmNactivateCallback, PushBCallback, form);

   XtRealizeWidget (Shell1);

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
   static Widget pb2;

   if (count == 0)
   {
      printf ("Setting the fraction base to 0.\n");
      n = 0;
      XtSetArg (args[n], XmNfractionBase, n);		n++;
      XtSetValues (form, args, n);
   }      
   else if (count == 1)
   {
      printf ("Changing the size of the form to (200, 300).\n");
      n = 0;
      XtSetArg (args[n], XmNwidth, 200);		n++;
      XtSetArg (args[n], XmNheight, 300);		n++;
      XtSetValues (form, args, n);
   }
   else if (count == 2)
   {
      printf ("Changing the size of the form to (0, 0).\n");
      n = 0;
      XtSetArg (args[n], XmNwidth, 0);			n++;
      XtSetArg (args[n], XmNheight, 0);			n++;
      XtSetValues (form, args, n);
   }

   count++;
}





















