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
static char rcsid[] = "$XConsortium: PushBGad4.c /main/8 1995/07/13 17:29:58 drk $"
#endif
#endif

/*
   Test was written for PIR629
   PIR748,
   PIR903,
   and PIR637
*/

#include <testlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>


/*  Global Variables  */
Widget    PBGadget1;

/*  Private Functions  */

void	       ActCB();
static void  DestroyCB();

void  main (argc, argv)
int     argc;
char  **argv;
{
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           tcs;
  Pixel 	       red_pixel, blue_pixel;
  Widget	       Form1, PushB1, Cascade1, PushButton1;
  Widget	       Pulldown1, MenuBar1, PushButton2, PushButton3;

  CommonTestInit(argc, argv);
  
  blue_pixel = CommonGetColor("cadetblue");
  red_pixel = CommonGetColor("maroon");

  n = 0;
  XtSetArg (args[n], XmNmarginWidth, 2);		n++;
  XtSetArg (args[n], XmNmarginHeight, 2);		n++;
  XtSetArg (args[n], XmNbackground, blue_pixel); 	n++;
  XtSetArg (args[n], XmNwidth, 150);			n++;
  XtSetArg (args[n], XmNheight, 175);			n++;
  Form1 = XmCreateForm (Shell1, "Form1", args, n); 
  XtManageChild(Form1);

  n = 0;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); 	n++;
  XtSetArg (args[n], XmNtraversalOn, True);  n++;
  MenuBar1 = XmCreateMenuBar (Form1, "MenuBar1", args, n);
  XtManageChild (MenuBar1);
  
  n = 0;
  Pulldown1 = XmCreatePulldownMenu (MenuBar1, "Pulldown1", args, n);
  
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, Pulldown1);  n++;
  Cascade1 = XmCreateCascadeButton(MenuBar1, "Cascade1", args, n);
  XtManageChild (Cascade1);

  n = 0;
  tcs = XmStringLtoRCreate("Destroy PushButton", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs); 	n++;
  XtSetArg (args[n], XmNwidth, 150);		n++;
  XtSetArg (args[n], XmNheight, 50);		n++;
  PushButton1 = XmCreatePushButton(Pulldown1, "PushButton1", args, n);
  XmStringFree (tcs);
  XtManageChild (PushButton1);

  n = 0;
  tcs = XmStringLtoRCreate("Destroy Form", XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs); 	n++;
  XtSetArg (args[n], XmNwidth, 150);		n++;
  XtSetArg (args[n], XmNheight, 50);		n++;
  PushButton2 = XmCreatePushButton(Pulldown1, "PushButton2", args, n);
  XmStringFree (tcs);
  XtManageChild (PushButton2);

  n = 0;
  tcs = XmStringLtoRCreate("Destroy PushButtonGadget",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg (args[n], XmNlabelString, tcs); 	n++;
  XtSetArg (args[n], XmNwidth, 150);		n++;
  XtSetArg (args[n], XmNheight, 50);		n++;
  PushButton3 = XmCreatePushButton(Pulldown1, "PushButton3", args, n);
  XmStringFree (tcs);
  XtManageChild (PushButton3);

  XtManageChild (PushButton2);
  n = 0;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, MenuBar1); n++;
  XtSetArg (args[n], XmNtopOffset, 5);	      n++;
  XtSetArg (args[n], XmNbackground, red_pixel);	n++;
  XtSetArg (args[n], XmNtraversalOn, True);	n++;
  XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
  XtSetArg (args[n], XmNhighlightThickness, 2);		n++;
  XtSetArg (args[n], XmNhighlightColor, CommonGetColor("orangered"));	n++;
  XtSetArg (args[n], XmNheight, 75);   n++;
  PushB1 = XmCreatePushButton(Form1, "PushB1", args, n);
  XtManageChild(PushB1);

  XmAddTabGroup(PushB1);

  /* Add Tab Group again to test PIR637 */

  XmAddTabGroup(PushB1);

  n = 0;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 		n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 		n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); 		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 		n++;
  XtSetArg (args[n], XmNtopWidget, PushB1);				n++;
  XtSetArg (args[n], XmNtopOffset, 5);  				n++;
  XtSetArg (args[n], XmNtraversalOn, True);				n++;
  XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
  XtSetArg (args[n], XmNhighlightThickness, 2);				n++;
  PBGadget1 = XmCreatePushButtonGadget (Form1, "PBGadget1", args, n);
  XtManageChild(PBGadget1);

  XmAddTabGroup(PBGadget1);	

  XtAddCallback(PBGadget1, XmNactivateCallback, ActCB, "PushButtonGadget");
  XtAddCallback(PushB1, XmNactivateCallback, ActCB, "PushButton"); 
  XtAddCallback(PushButton1, XmNactivateCallback, DestroyCB, PushB1);
  XtAddCallback(PushButton2, XmNactivateCallback, DestroyCB, Form1);
  XtAddCallback(PushButton3, XmNactivateCallback, DestroyCB, PBGadget1);
    
  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(MenuBar1);
  XtDestroyWidget(Form1);
  XtDestroyWidget(Cascade1);
  XtDestroyWidget(Pulldown1);
  XtDestroyWidget(PushButton1);
  XtDestroyWidget(PushButton2);
  XtDestroyWidget(PushButton3);
  XtDestroyWidget(PushB1);
  XtDestroyWidget(PBGadget1);
}

void  ActCB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
  Arg 	args[5];
  int 	n = 0;
  Pixel yellow, green;
  char	*name;
  Widget parent;

  parent = XtParent(w);
  name = (char *) client_data;

  printf("...Setting the background color of the Form to Yellow\n");
  yellow = CommonGetColor("Yellow");
  XtSetArg(args[n], XmNbackground, yellow); n++;
  XtSetValues(parent, args, n);

  /* CR 1139 fix begin */
  XmChangeColor(w, yellow);
  /* CR 1139 fix end */
  
  green = CommonGetColor("forestgreen");

  printf("...Setting the background color of %s to Green\n\n", name);
  XtSetArg (args[0], XmNbackground, green);
  XtSetValues (w, args, 1);
}

static void  DestroyCB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
  Widget	PushB;

  PushB = (Widget) client_data;

  XtDestroyWidget(PushB);
}


