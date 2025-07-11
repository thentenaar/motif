/* $XConsortium: RIForm2.c /main/5 1995/07/15 21:08:46 drk $ */
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
 * Revision 1.6.4.2  1993/05/05  14:17:10  shobana
 * 	Added a call to XtRealizeWidget so that option menu would be completely visible
 * 	[1993/05/05  14:16:47  shobana]
 *
 * Revision 1.6.2.2  1992/05/05  17:59:34  qauser
 * 	Shows PIR 3514 is still open
 * 	[1992/05/04  17:22:09  qauser]
 * 
 * Revision 1.6  1992/03/13  17:20:18  devsrc
 * 	Converted to ODE
 * 
*/ 
/*
 *  Create a push button in a form.  Add a callback to the push button
 *  which resizes it.  See what the form does.
 */


#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>

extern void PushBCallback();
static void activate_callback();
void CreateSrc();	
static void activateCB_pushButton1();

Widget form, drawingArea1_shell, drawingArea1, pushButton1;
Widget Push0, Push1, Push2, pushb, form;

/*  main  */
void main (argc, argv)
unsigned int argc;
char **argv;

{
  Arg args[10];
  int n;
  XmString tcs;

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);
    
  XtSetArg(args[0], XmNallowShellResize, True);
  XtSetValues(Shell1, args, 1);

  /* Begin Test for PIR 3514 */

  n = 0;
  XtSetArg(args[n], XmNx, 490); n++;
  XtSetArg(args[n], XmNy, 40); n++;
  XtSetArg(args[n], XmNwidth, 440); n++;
  XtSetArg(args[n], XmNheight, 400); n++;
  drawingArea1_shell = XmCreateBulletinBoard(Shell1, "drawingArea1_shell",
					     args, n);
  XtManageChild(drawingArea1_shell);

  n = 0;
  XtSetArg(args[n], XmNheight, 380); n++;
  XtSetArg(args[n], XmNwidth, 430); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
  drawingArea1 = XmCreateDrawingArea(drawingArea1_shell, "drawingArea1",
				     args, n);
  XtManageChild(drawingArea1);

  n = 0;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  XtSetArg(args[n], XmNheight, 150); n++;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNy, 20); n++;
  XtSetArg(args[n], XmNx, 70); n++;
  XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
  form = XmCreateForm(drawingArea1, "form", args, n);
  XtManageChild(form);
  
  n = 0;
  tcs = XmStringCreateLtoR("Create Option Menu", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNheight, 50); n++;
  XtSetArg(args[n], XmNwidth, 320); n++;
  XtSetArg(args[n], XmNy, 310); n++;
  XtSetArg(args[n], XmNx, 70); n++;
  pushButton1 = XmCreatePushButton(drawingArea1,"pushButton1", args, n);
  XtManageChild(pushButton1);
  XtAddCallback(pushButton1, XmNactivateCallback,activateCB_pushButton1, NULL);

  XtRealizeWidget(Shell1);

  CommonPause();

  XtDestroyWidget(drawingArea1_shell);

   /*  The form creation.  Contained in an outer frame.  */

  n = 0;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  form = XmCreateForm(Shell1, "form", args, n);
  XtManageChild(form);

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

   XtAddCallback (pushb, XmNactivateCallback, PushBCallback, NULL);

   n = 0;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++; 
   XtSetValues(form, args, n);

   CommonPause();

   /* Begin Test for Pir2389 */

   XtDestroyWidget(form);

   n = 0;
   form = XmCreateForm(Shell1, "form", args, n);
   XtManageChild(form);
    
   /* upper left widget */
   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); 		n++;
   XtSetArg(args[n], XmNwidth, 100); 				n++;
   Push0 = XmCreatePushButton(form, "Push0", args, n);
   XtManageChild(Push0);

   /* lower widget */
   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
   XtSetArg(args[n], XmNtopWidget, Push0); 			n++;
   XtSetArg(args[n], XmNwidth, 300); 				n++;
   Push2 = XmCreatePushButton(form, "Push2", args, n);
   XtManageChild(Push2);

   /* upper right widget, that will resize */
   n = 0;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); 		n++;
   XtSetArg(args[n], XmNwidth, 100); 				n++; 
   Push1 = XmCreatePushButton(form, "Push1", args, n);
   XtManageChild(Push1);

   XtAddCallback(Push1, XmNactivateCallback, activate_callback, NULL);

   n = 0;
   XtSetArg(args[n], XmNallowShellResize, False);		n++;
   XtSetValues(Shell1, args, n);
    
   CommonPause(); 

   /* End Test for Pir2389 */
  CommonPause();  /* Exit */

  XtAppMainLoop(app_context);
}

void PushBCallback(Widget pw, caddr_t closure, caddr_t call_data)
{
  Arg args[10];
  int n;
  static Dimension width, height;
  static short firstTime = 1;

  if (firstTime)
    {
      n = 0;
      XtSetArg (args[n], XmNwidth, &width);		n++;
      XtSetArg (args[n], XmNheight, &height);		n++;
      XtGetValues (pw, args, n);
      firstTime = 0;
    }

  printf ("GetValues  --  width = %d,    height = %d\n", 
	  width, height);
  printf ("Setting Values to  --  width = %d,    height = %d\n\n", 
	  width + 20, height + 20);

  width += 20;
  height += 20;
 
  n = 0;
  XtSetArg (args[n], XmNwidth, width); n++;
  XtSetArg (args[n], XmNheight, height); n++;
  XtSetValues (pw, args, 2);

}

static void activate_callback(Widget widget, XtPointer tag,
			      XmDrawingAreaCallbackStruct *dacb)
{
    Arg args[10] ;
    Cardinal n;
    n = 0;
    XtSetArg(args[n], XmNwidth, 150); n++;
    XtSetValues(widget, args, n);
}

/* ARGSUSED */

static void
activateCB_pushButton1(Widget UxWidget, XtPointer UxClientData, 
		       XtPointer UxCallbackArg)
{
  CreateSrc(form);
}

void CreateSrc(Widget frm)
{
  Widget menu, panel, pb;
  XmString xms;
  int n;
  Arg args[10];

  panel = XmCreatePulldownMenu(frm, "panel", NULL, 0);

  xms = XmStringCreateSimple("Option Menu Item");
  n = 0;
  XtSetArg(args[n], XmNlabelString, xms); n++;
  pb = XmCreatePushButtonGadget(panel, "pb", args, n);
  XmStringFree(xms);
  XtManageChild(pb);

  n = 0;
  XtSetArg(args[n], XmNborderWidth, 0);                    n++;
  XtSetArg(args[n], XmNmarginWidth, 0);                    n++;
  XtSetArg(args[n], XmNsubMenuId, panel);                  n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);    n++;
  menu= XmCreateOptionMenu(frm, "menu", args, n);
  XtRealizeWidget (menu);
  XtManageChild(menu);
}

