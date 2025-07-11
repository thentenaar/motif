/* $TOG: RIFormM1.c /main/5 1999/01/14 18:11:03 jff $ */
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
/*
 * Original History:
 *
 * Revision 1.1.2.3  1992/12/10  18:14:41  shobana
 * 	merged another test in
 * 	[1992/12/10  18:13:51  shobana]
 *
 * Revision 1.1.2.2  1992/12/10  16:59:03  shobana
 * 	Test for CR 3203
 * 	[1992/12/10  16:58:48  shobana]
 * 
 */
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
   Widget frame;
   Widget pushb;
   Widget scrollbar1;
   Widget scrollbar2;
   Arg args[10];
   int n;
   int offset;
   Dimension height;
   Dimension width;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);


   if (UserData == NULL) {
	printf ("Usage: %s -u [early middle late]\n", argv[0]);
	printf ("Assuming late realize.\n");
   }
    
   frame = XmCreateFrame(Shell1, "frame", NULL, 0);
   XtManageChild(frame);

   if (UserData && (strcmp (UserData, "early") == 0)) {
	printf ("Early realize.\n");
	fflush (stdout);
	XtRealizeWidget (Shell1);
   }

   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   XtSetArg (args[n], XmNwidth, 4000);			n++;
   XtSetArg (args[n], XmNheight, 3000);			n++;
   XtSetArg (args[n], XmNunitType, Xm1000TH_INCHES);	n++;
   form = XmCreateForm (frame, "form", args, n);
   XtManageChild (form);

   if (UserData && (strcmp (UserData, "middle") == 0)) {
	printf ("Middle realize.\n");
	fflush (stdout);
	XtRealizeWidget (Shell1);
   }

   /*  Arguments to the first scrollbar.  */

   n = 0;
   XtSetArg (args[n], XmNheight, 750);				n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightOffset, 500);			n++;
   XtSetArg (args[n], XmNunitType, Xm1000TH_INCHES);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   scrollbar1 = XmCreateScrollBar (form, "scrollbar1", args, n);
   XtManageChild (scrollbar1);

   /*  Arguments to the second scrollbar.  */

   n = 0;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNunitType, Xm1000TH_INCHES);		n++;
   XtSetArg (args[n], XmNrightWidget, scrollbar1);		n++;
   scrollbar2 = XmCreateScrollBar (form, "scrollbar2", args, n);
   XtManageChild (scrollbar2);

   /*  Set the bottom offset for scrollbar2 to 1/2 inch  */

   n = 0;
   XtSetArg (args[n], XmNbottomOffset, 500);	n++;
   XtSetValues (scrollbar2, args, n);


   if (!UserData || (strcmp (UserData, "late") == 0)) {
	printf ("Late realize.\n");
	fflush (stdout);
	XtRealizeWidget (Shell1);
   }

   /*  Get Values out of the form and scrollbars and print the results.  */

   printf ("Width\tHeight\tOffset1\tOffset2\tMode\n");

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);	n++;
   XtSetArg (args[n], XmNheight, &height);	n++;
   XtGetValues (form, args, n);
   printf ("%d\t%d\t", width, height);

   n = 0;
   XtSetArg (args[n], XmNrightOffset, &offset);	n++;
   XtGetValues (scrollbar1, args, n);
   printf ("%d\t", offset);

   n = 0;
   XtSetArg (args[n], XmNbottomOffset, &offset);n++;
   XtGetValues (scrollbar2, args, n);
   printf ("%d\t", offset);

   printf ("%s\n", (UserData ? UserData : "late"));


   CommonPause();

   XtDestroyWidget (frame);


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
      /* need to create a second child to see effect of [vert,horiz]spacing */
      /* begin test for PIR 3203 */
      n = 0;
      if (UserData != NULL)
	if (strcmp (UserData, "default") == 0) {
	  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); 
	  n++;
	}
      pb2 = XmCreatePushButtonGadget (form, "pb2", args, n);
      XtManageChild (pb2);

      printf ("Setting the unit type to inches and setting the spacing.\n");
      n = 0;
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
      XtSetArg (args[n], XmNtopWidget, pb2);                    n++;
      XtSetValues (pw, args, n);

      n = 0;
      XtSetArg (args[n], XmNunitType, Xm1000TH_INCHES);		n++;
      XtSetArg (args[n], XmNverticalSpacing, 2000);		n++;
      XtSetArg (args[n], XmNhorizontalSpacing, 2000);		n++;
      XtSetValues (form, args, n);

   }
   else if (count == 1) {
     n = 0;
     XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);      n++;
     XtSetValues (pw, args, n);

     n = 0;
     XtSetArg (args[n], XmNverticalSpacing, 200); n++;
     XtSetArg (args[n], XmNhorizontalSpacing, 200); n++;
     XtSetValues (form, args, n);

     XtDestroyWidget (pb2);
   }
   /* end test for PIR 3203 */

   count++;
}





















