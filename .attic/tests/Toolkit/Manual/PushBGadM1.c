/* $XConsortium: PushBGadM1.c /main/5 1995/07/15 21:06:23 drk $ */
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


#include <testlib.h>

/*  Global Variables  */
Widget    Frame1;
Widget    PBGadget1;
Widget    PBWidget1;



static void ArmCB(Widget, XtPointer, XtPointer);
static void DisarmCB(Widget, XtPointer, XtPointer);
static void ActCB(Widget, XtPointer, XtPointer);


void  
main (int argc, char **argv)
{

  register int n;
  Arg args[MAX_ARGS];
  XmString tcs;
  XmString string;

  CommonTestInit(argc, argv);
    
  


  n = 0;
  Frame1 = XmCreateFrame(Shell1, "Frame1", args, n); 
  XtManageChild(Frame1);

  /* tests for CR 4891, 5130 */

  n = 0;
  XtSetArg(args[n], XmNfillOnArm, False); n++;
  string = XmStringLtoRCreate("Push Gadget Here", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelType, XmSTRING); n++; 
  XtSetArg(args[n], XmNlabelString, string); n++; 
  XtSetArg(args[n], XmNheight, 200); n++; 
  XtSetArg(args[n], XmNwidth, 200); n++; 
  XtSetArg (args[n], XmNhighlightThickness, 25); n++;
  PBGadget1 = XmCreatePushButtonGadget(Frame1, "PBGadget", args, n);
  XtManageChild(PBGadget1);

  /* add callbacks for select, arm, disarm */
  XtAddCallback(PBGadget1, XmNactivateCallback, ActCB, NULL);
  XtAddCallback(PBGadget1, XmNarmCallback, ArmCB, NULL);
  XtAddCallback(PBGadget1, XmNdisarmCallback, DisarmCB, NULL);

  XtRealizeWidget(Shell1);
 
  CommonPause();
  CommonPause();

  XtDestroyWidget(PBGadget1);

  /* test for CR 5130. */

  n = 0;
  XtSetArg(args[n], XmNfillOnArm, False); n++;
  string = XmStringLtoRCreate("Push Widget Here", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelType, XmSTRING); n++; 
  XtSetArg(args[n], XmNlabelString, string); n++; 
  XtSetArg(args[n], XmNheight, 200); n++; 
  XtSetArg(args[n], XmNwidth, 200); n++; 
  XtSetArg (args[n], XmNhighlightThickness, 25); n++;
  PBWidget1 = XmCreatePushButton(Frame1, "PBWidget", args, n);
  XtManageChild(PBWidget1);

  CommonPause();


  XtAppMainLoop(app_context);

}

static void  
ActCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("PushButtonGadget Activated\n");
}


static void  
ArmCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("PushButtonGadget Armed\n");

}


static void  
DisarmCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("PushButtonGadget Disarmed\n");
    printf("*********************\n");
}

