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
static char rcsid[] = "$XConsortium: Form16.c /main/4 1995/07/13 18:03:00 drk $"
#endif
#endif


/*
 *  Create three PushButton in a form.  
 */

#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>
#
/*  Forward declarations  */

Widget frame;
Widget form;
Widget PushButton1;
Widget PushButton2;
Widget PushButton3;
static void PushButtonCallback();

/*  main  */

void main (argc, argv)
unsigned int  argc;
char         *argv[];
{
  Arg args[MAX_ARGS];
  unsigned int n;
  
  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth, 1);                    n++;
  XtSetArg(args[n], XmNheight, 1);                   n++;
  XtSetValues(Shell1, args, n);
  
  XtRealizeWidget (Shell1);
  
  /* Create frame for Form. */
  frame = XmCreateFrame(Shell1,"frame", NULL, 0);

  /* The form creation. Contained in an outer frame. */
  n = 0;
  XtSetArg (args[n], XmNverticalSpacing, 15);	n++;
  form = XmCreateForm (frame, "form", args, n);
  
  /*  Create first PushButton.  */
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton1 = XmCreatePushButton(form, "PushButton1", args, n);
  XtAddCallback(PushButton1, XmNactivateCallback, PushButtonCallback, NULL);
  
  
  /*  Create second PushButton.  */
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNleftWidget, PushButton1);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton2 = XmCreatePushButton(form, "PushButton2", args, n);
  XtAddCallback(PushButton2, XmNactivateCallback, PushButtonCallback, NULL);

  
  /*  Create third PushButton.  */
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNleftWidget, PushButton2); 	        n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton3 = XmCreatePushButton(form, "PushButton3", args, n);
  XtAddCallback(PushButton3, XmNactivateCallback, PushButtonCallback, NULL);

  /* Manage widgets. */
  XtManageChild(frame);
  XtManageChild(form);
  XtManageChild(PushButton1);
  XtManageChild(PushButton2);
  XtManageChild(PushButton3);


    
  CommonPause();
  CommonPause();
  CommonPause();

  XtDestroyWidget(form);

  /*
   *  Create a generic form
   */

  n = 0;
  XtSetArg(args[n], XmNmarginHeight, 25); n++;
  XtSetArg(args[n], XmNmarginWidth, 25); n++;
  form = XmCreateForm (frame, "form", args, n);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  PushButton1 = XmCreatePushButton(form, "PushButton1", args, n);

  XtManageChild(PushButton1);
  XtManageChild(form);
  CommonPause();

  /*
   *  Now try to update verticalSpacing/horizontalSpacing. Should
   *  have no effect.
   */
   
  n = 0;
  XtSetArg(args[n], XmNverticalSpacing, 50); n++;
  XtSetArg(args[n], XmNhorizontalSpacing, 50); n++;
  XtSetValues(form, args,  n);

  CommonPause();

  /*
   *  Update marginHeight, marginWidth.  This should have an
   *	effect.
   */

  n = 0;
  XtSetArg(args[n], XmNmarginHeight, 70); n++;
  XtSetArg(args[n], XmNmarginWidth, 70); n++;
  XtSetValues(form, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNrightOffset, 20); n++;
  XtSetArg(args[n], XmNleftOffset, 20); n++;
  XtSetValues(PushButton1, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNverticalSpacing, 50); n++;
  XtSetArg(args[n], XmNhorizontalSpacing, 50); n++;
  XtSetValues(form, args,  n);

  CommonPause();

  XtAppMainLoop(app_context);
}

static void PushButtonCallback(pb, client_data, call_data)
Widget pb;
caddr_t client_data;
caddr_t call_data;
{
  XtDestroyWidget(pb);
}











