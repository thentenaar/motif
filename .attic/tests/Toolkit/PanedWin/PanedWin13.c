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
static char rcsid[] = "$XConsortium: PanedWin13.c /main/4 1995/07/13 19:06:32 drk $"
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

/*  Forward declarations  */


void doIt (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Arg args[1];
  int whichButton, scaleValue;
  static int toggleIsOn = 1;
  Dimension howBig;

  whichButton = (int)(long) client_data;
  printf ("doIt called with client_data %d\n", (int)(long) client_data);

  XtSetArg (args[0], XmNvalue, &scaleValue);
  XtGetValues (scale1, args, 1);

  switch (whichButton)
    {
    case 1 :   XtSetArg(args[0], XmNwidth, scaleValue);
      XtSetValues (push1, args, 1);
      /* check to see how wide the child thinks it is */
      XtSetArg(args[0], XmNwidth, &howBig);
      XtGetValues(push1, args, 1);
      printf("I wanna be %d, I am %d\n", scaleValue, howBig);
      break;

    case 2 :   XtSetArg(args[0], XmNwidth, scaleValue);
      XtSetValues (push2, args, 1);
      /* check to see how wide the child thinks it is */
      XtSetArg(args[0], XmNwidth, &howBig);
      XtGetValues(push2, args, 1);
      printf("I wanna be %d, I am %d\n", scaleValue, howBig);
      break;

    case 3 :   /* if the toggle's on, set Shell allowResize: False */
      if (toggleIsOn)
	{	XtSetArg (args[0], XmNallowShellResize, False);
		XtSetValues (Shell1, args, 1);
		toggleIsOn = 0;
	      }
      else /* toggleOn is false, so we making resizable */
	{	XtSetArg (args[0], XmNallowShellResize, True);
		XtSetValues (Shell1, args, 1);
		toggleIsOn = 1;
	      };
      break;

    case 4:    XtSetArg(args[0], XmNwidth, scaleValue);
      XtSetValues (form, args, 1);
      break;

    default:   printf ("How did I get here?\n");
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
   form = XmCreatePanedWindow (Shell1, "form", args, n);
   XtManageChild (form);

   /*  Arguments to the first frame.  */

   n = 0;
   XtSetArg(args[n], XmNallowResize, True); n++;
   push1 = XmCreatePushButton (form, "push1", args, n);
   XtManageChild (push1);

   n = 0;
   XtSetArg(args[n], XmNallowResize, True); n++;
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
    Widget	  pb1, pb2, pb3, tb1;
    Widget	  label1;
    Arg           args[MAX_ARGS];
    register int  n;
    char *startString;

    n = 0;
    XtSetArg (args[n], XmNdefaultPosition, False); n++;
    XtSetArg (args[n], XmNy, 500); n++;
    XtSetArg (args[n], XmNheight, 400); n++;
    XtSetArg (args[n], XmNwidth, 500); n++;
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

    n = 0;
    XtSetArg (args[n], XmNx, 20); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString,
     XmStringCreateLtoR ("Resize Paned Window", XmSTRING_DEFAULT_CHARSET)); n++;
    pb3 = XmCreatePushButton (BBD, "pb3", args, n);
    XtAddCallback (pb3, XmNactivateCallback, doIt, (XtPointer) 4);

    startString = "Set the scale to desired width and use the\npushbuttons to apply that width to the client's\nbuttons. Toggle XmNallowShellResize with the Shell\nResize toggle button. It is True by default.";

    n = 0;
    XtSetArg (args[n], XmNx, 20); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg (args[n], XmNeditable, False); n++;
    XtSetArg (args[n], XmNvalue, startString); n++;
    XtSetArg (args[n], XmNrows, 4); n++;
    XtSetArg (args[n], XmNcolumns, 50); n++;
    label1 = XmCreateText (BBD, "label1", args, n);
    XtManageChild (label1);


    XtManageChild (pb1);
    XtManageChild (pb2);
    XtManageChild (pb3);
    XtManageChild (tb1);

    XtManageChild (BBD); 

}
