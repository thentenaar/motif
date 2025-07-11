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
static char rcsid[] = "$XConsortium: Form15.c /main/4 1995/07/13 18:02:35 drk $"
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
Widget scale1;
Widget push1;
Widget push2;

static void CreateBBPopup();

void doIt (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Arg args[1];
  int whichButton, scaleValue;
  static int toggleIsOn = 1;

  whichButton = (int) (long)client_data;

  XtSetArg (args[0], XmNvalue, &scaleValue);
  XtGetValues (scale1, args, 1);

  switch (whichButton)
    {
    case 1 :   XtSetArg(args[0], XmNwidth, scaleValue);
      XtSetValues(push1, args, 1);
      break;

    case 2 :   XtSetArg(args[0], XmNwidth, scaleValue);
      XtSetValues(push2, args, 1);
      break;

    case 3 :   /* if the toggle's on, set Shell allowResize: False */
      if (toggleIsOn)
	{	XtSetArg(args[0], XmNallowShellResize, False);
		XtSetValues(Shell1, args, 1);
		toggleIsOn = 0;
	      }
      else /* toggleOn is false, so we making resizable */
	{	XtSetArg(args[0], XmNallowShellResize, True);
		XtSetValues(Shell1, args, 1);
		toggleIsOn = 1;
	      };
      break;

    default:   printf("How did I get here?\n");
      break;

    }
}

/*  main  */

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
   /* XtSetArg (args[n], XmNheight, 35); n++;
      XtSetArg (args[n], XmNwidth, 50); n++; */

   form = XmCreateForm (Shell1, "form", args, n);
   XtManageChild (form);

   /*  Arguments to the first frame.  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   push1 = XmCreatePushButton (form, "push1", args, n);
   XtManageChild (push1);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopOffset, 10);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, push1);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   push2 = XmCreatePushButton (form, "push2", args, n);
   XtManageChild (push2);

   XtRealizeWidget (Shell1);

   CreateBBPopup();
   CommonPause();

   XtAppMainLoop(app_context);
}

static void CreateBBPopup()
{
    Widget        BBD;
    Widget	  pb1, pb2, tb1;
    Arg           args[MAX_ARGS];
    register int  n;

    n = 0;
    XtSetArg (args[n], XmNdefaultPosition, False); n++;
    XtSetArg (args[n], XmNy, 500); n++;
    XtSetArg (args[n], XmNheight, 400); n++;
    XtSetArg (args[n], XmNwidth, 400); n++;
    XtSetArg (args[n], XmNautoUnmanage, False); n++;
    BBD = XmCreateBulletinBoardDialog(Shell1, "BBD", args, n);

    n = 0; 
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg (args[n], XmNshowValue, True); n++;
    XtSetArg (args[n], XmNminimum, 1); n++;
    XtSetArg (args[n], XmNmaximum, 600); n++;
    XtSetArg (args[n], XmNvalue, 200); n++;
    XtSetArg (args[n], XmNwidth, 200); n++;
    XtSetArg (args[n], XmNx, 20); n++;
    XtSetArg (args[n], XmNy, 10); n++;
    scale1 = XmCreateScale (BBD, "scale1", args, n);
    XtManageChild (scale1);

    n = 0;
    XtSetArg (args[n], XmNx, 20); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
     XmStringCreateLtoR ("Resize PB1", XmSTRING_DEFAULT_CHARSET)); n++;
    pb1 = XmCreatePushButton (BBD, "pb1", args, n);
    XtAddCallback (pb1, XmNactivateCallback, doIt, (XtPointer) 1);

    n = 0;
    XtSetArg (args[n], XmNx, 100); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
     XmStringCreateLtoR ("Resize PB2", XmSTRING_DEFAULT_CHARSET)); n++;
    pb2 = XmCreatePushButton (BBD, "pb2", args, n);
    XtAddCallback (pb2, XmNactivateCallback, doIt, (XtPointer) 2);

    n = 0;
    XtSetArg (args[n], XmNx, 200); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNset, True); n++;
    XtSetArg (args[n], XmNlabelString,
     XmStringCreateLtoR ("Shell Resizable", XmSTRING_DEFAULT_CHARSET)); n++;
    tb1 = XmCreateToggleButton (BBD, "tb1", args, n);
    XtAddCallback (tb1, XmNvalueChangedCallback, doIt, (XtPointer) 3);

    XtManageChild (pb1);
    XtManageChild (pb2);
    XtManageChild (tb1);

    XtManageChild (BBD); 
}








