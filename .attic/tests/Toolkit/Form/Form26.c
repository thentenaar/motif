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
static char rcsid[] = "$XConsortium: Form26.c /main/4 1995/07/13 18:09:14 drk $"
#endif
#endif

#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Forward declarations  */

extern void FormCallback();
static void report();


void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget 	form;
   Display 	*display;
   Widget 	Scroll_Text;
   Widget	mess_dia;
   Arg 		args[20];
   int  	n;
   char         *string = "This is a Scrolled Text in\na Form with left and top attachments\nIt should be displayed on\nthree lines\n";

   CommonTestInit(argc, argv);

   n = 0;
   XtSetArg (args[n], XmNx, 0);					n++;
   XtSetArg (args[n], XmNy, 0);					n++;
   form = XmCreateForm(Shell1,"form", NULL, 0);
   XtManageChild(form);

   n = 0;
   XtSetArg(args[n], XmNgeometry, "+0+500"); n++;
   XtSetArg(args[n], XmNdefaultPosition, False);		n++;
   mess_dia = XmCreateMessageDialog(form, "MessageDia", args, n);
   XtManageChild(mess_dia);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);		n++;
   XtSetArg (args[n], XmNbackground, CommonGetColor("ForestGreen"));	n++;
   XtSetArg (args[n], XmNcolumns, 20);				n++;
   XtSetArg (args[n], XmNborderWidth, 8);			n++;
   XtSetArg (args[n], XmNvalue, string);			n++;
   XtSetArg (args[n], XmNwordWrap, True);			n++;
   XtSetArg (args[n], XmNrows, 3);				n++;
   XtSetArg (args[n], XmNscrollLeftSide, True);			n++;
   XtSetArg (args[n], XmNshadowThickness, 8);			n++;
   XtSetArg (args[n], XmNblinkRate, 0);				n++;
   Scroll_Text = XmCreateScrolledText(form, "ScrollText1", args, n);
   XtManageChild(Scroll_Text);

   XtAddCallback(Scroll_Text, XmNfocusCallback, report, "focusCallback");
   XtAddCallback(Scroll_Text, XmNvalueChangedCallback, report, "valueChanged");

   XtRealizeWidget (Shell1);

   CommonPause();

   /* begin test for PIR 2486 */
   n = 0;
   XtSetArg(args[n], XmNwidth, 200); n++;
   XtSetArg(args[n], XmNheight, 50); n++;
   XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
   XtSetValues(Scroll_Text, args, n);

   /* end test for PIR 2486 */

   CommonPause();
   CommonPause(); /* exit */

   XtAppMainLoop(app_context);
}


void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
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














static void report( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
	printf("%s callback made\n", tag);
}
