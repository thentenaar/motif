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
static char rcsid[] = "$XConsortium: MessageBM1.c /main/5 1995/07/13 18:36:42 drk $"
#endif
#endif


/* 
 * Create a message box with menu bar and button children, as detailed in
 *  the Motif 1.2 specification.  Then create a MessageBox of Type:
 *  XmDIALOG_TEMPLATE.
 */

#include <signal.h>
#include <testlib.h>
#include "smiley.bmp"
#include "mtn.bmp"

Widget mbox, rowcol2, label3;
Widget pushButton, toggleButton, arrowButton, drawnButton;
Widget menuBar, cascButton1, cascButton2, pulldown1, pulldown2;
Widget buttonsa[3], buttonsb[4], rowcol, labelfrito;
Widget drawingArea, scale1, radbox1, radbox2, panedWindow;

/* callbacks */

static void OkCB();

static XtCallbackRec ok_cb[] = {
  { OkCB, NULL},
  { NULL, NULL}
};

static void CancelCB();

static XtCallbackRec cancel_cb[] = {
  { CancelCB, NULL},
  { NULL, NULL}
};
static void HelpCB();

static XtCallbackRec help_cb[] = {
  { HelpCB, NULL},
  { NULL, NULL}
};


/* ARGSUSED */
static void OkCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("OK Callback received\n");
}	

/* ARGSUSED */
static void HelpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("Help Callback received\n");
}

/* ARGSUSED */
static void CancelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("Cancel Callback received\n");
}

/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
  Arg args[10];       
  register int n;    
  XmString tcs, tcs1;
  Widget Frame;
  Pixmap smilemap, mtnmap;
  XmString simplearray[4], simplearray1[4];

#ifndef MOTIF1_1

  /*  initialize toolkit */

  CommonTestInit(argc, argv);

  n = 0;
  Frame = XmCreateFrame(Shell1, "Frame1", args, n);
  XtManageChild(Frame);
  
  /* create message dialog */

  tcs = XmStringLtoRCreate("This is a Message Box with multiple children.",
			   XmSTRING_DEFAULT_CHARSET);

  n = 0;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION); n++;
  XtSetArg(args[n], XmNokCallback, ok_cb); n++;
  XtSetArg(args[n], XmNhelpCallback, help_cb); n++;
  XtSetArg(args[n], XmNautoUnmanage, False);  n++;
  XtSetArg(args[n], XmNminimizeButtons, False); n++;
  XtSetArg(args[n], XmNmessageString, tcs); n++;
  mbox = XmCreateMessageBox(Frame, "mbox", args, n);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 200); n++;
  XtSetArg(args[n], XmNheight, 200); n++;
  XtSetArg(args[n], XmNborderWidth, 4); n++;
  drawingArea = XmCreateDrawingArea(mbox, "Drawarea", args, n);
  XtManageChild(drawingArea);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  scale1 = XmCreateScale(drawingArea, "Scale", args, n);
  XtManageChild(scale1);

  n = 0;
  pushButton = XmCreatePushButton(mbox, "PushMe1", args, n);
  XtManageChild(pushButton);

  tcs = XmStringLtoRCreate("ToggleMe2", XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggleButton = XmCreateToggleButton(mbox, "ToggleMe2", args, n);
  XmStringFree(tcs);
  XtManageChild(toggleButton);

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_UP); n++;
  arrowButton = XmCreateArrowButton(mbox, "PointMe3", args, n);
  XtManageChild(arrowButton);

  n = 0;
  tcs = XmStringLtoRCreate("DrawnButton", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  drawnButton = XmCreateDrawnButton(mbox, "drawnbutton", args, n);
  XmStringFree(tcs);
  XtManageChild(drawnButton);

  n = 0;
  menuBar = XmCreateMenuBar(mbox, "menuBar", args, n);
  XtManageChild(menuBar);

  n = 0;
  pulldown1 = XmCreatePulldownMenu(menuBar, "Pulldown1", args, n);
  pulldown2 = XmCreatePulldownMenu(menuBar, "Pulldown2", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pulldown1); n++;
  cascButton1 = XmCreateCascadeButton(menuBar, "Cascade1", args, n);
  XtManageChild(cascButton1);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pulldown2); n++;
  cascButton2 = XmCreateCascadeButton(menuBar, "Cascade2", args, n);
  XtManageChild(cascButton2);

  buttonsa[0] = XmCreatePushButton(pulldown1, "button11", NULL, 0);
  buttonsa[1] = XmCreatePushButton(pulldown1, "button12", NULL, 0);
  buttonsa[2] = XmCreatePushButton(pulldown1, "button13", NULL, 0);
  buttonsb[0] = XmCreatePushButton(pulldown2, "button21", NULL, 0);
  buttonsb[1] = XmCreateSeparator(pulldown2, "separator22", NULL, 0);
  buttonsb[2] = XmCreatePushButton(pulldown2, "button31", NULL, 0);
  XtManageChildren(buttonsa, 3);
  XtManageChildren(buttonsb, 3);

  XtManageChild(mbox);
  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();

  /*  Destroy the message box so we can create another one  */
  XtDestroyWidget(mbox);

  /*  
   * Create an XmDIALOG_TEMPLATE type MessageBox widget via
   *  XmCreateTemplateDialog
   */

  n = 0;
  tcs = XmStringLtoRCreate(
          "This is a test of the XmCreateTemplateDialog function", 
			   XmSTRING_DEFAULT_CHARSET);

  mtnmap = XCreatePixmapFromBitmapData(display,
	        DefaultRootWindow(display),
	   	mtn_bits, mtn_width,mtn_height,
		BlackPixel(display,DefaultScreen(display)),
		WhitePixel(display,DefaultScreen(display)),
		DefaultDepth(display,DefaultScreen(display)));

  XtSetArg(args[n], XmNsymbolPixmap, mtnmap); n++;
  XtSetArg(args[n], XmNokCallback, ok_cb); n++;
  XtSetArg(args[n], XmNcancelCallback, cancel_cb); n++;
  XtSetArg(args[n], XmNhelpCallback, help_cb); n++;
  XtSetArg(args[n], XmNautoUnmanage, False);  n++;
  XtSetArg(args[n], XmNminimizeButtons, False); n++;
  XtSetArg(args[n], XmNmessageString, tcs); n++;
  mbox = XmCreateTemplateDialog(Frame, "mbox", args, n);
  XmStringFree(tcs);

  n = 0;
  rowcol = XmCreateRowColumn(mbox, "rowcol", args, n);

  n = 0;
  tcs = XmStringLtoRCreate("Eye, Yi Yi Yi... I am a Frito Bandito",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  labelfrito = XmCreateLabel(rowcol, "frito", args, n);

  XtManageChild(labelfrito);
  XtManageChild(rowcol);

  n = 0;
  menuBar = XmCreateMenuBar(mbox, "menuBar", args, n);
  XtManageChild(menuBar);

  n = 0;
  pulldown1 = XmCreatePulldownMenu(menuBar, "pd1", args, n);
  pulldown2 = XmCreatePulldownMenu(menuBar, "pd2", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pulldown1); n++;
  cascButton1 = XmCreateCascadeButton(menuBar,"Rocky Mountain", args, n);
  XtManageChild(cascButton1);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pulldown2); n++;
  cascButton2 = XmCreateCascadeButton(menuBar, "Hilly Mountain", args, n);
  XtManageChild(cascButton2);

  buttonsa[0] = XmCreatePushButton(pulldown1, "High", NULL, 0);
  buttonsa[1] = XmCreatePushButton(pulldown1, "Low", NULL, 0);
  buttonsa[2] = XmCreatePushButton(pulldown1, "Middle", NULL, 0);
  buttonsb[0] = XmCreatePushButton(pulldown2, "Over the River", NULL, 0); 
  buttonsb[1] = XmCreateSeparator(pulldown2, "hmmmmm", NULL, 0);
  buttonsb[2] = XmCreatePushButton(pulldown2, "and through the", NULL, 0);
  buttonsb[3] = XmCreatePushButton(pulldown2, "woods", NULL, 0);
  XtManageChildren(buttonsa, 3);
  XtManageChildren(buttonsb, 4);

  n = 0;
  pushButton = XmCreatePushButton(mbox, "Ticonderoga", args, n);
  XtManageChild(pushButton);

  tcs = XmStringLtoRCreate("Guide Dogs?", XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggleButton = XmCreateToggleButton(mbox, "Guide Dogs", args, n);
  XmStringFree(tcs);
  XtManageChild(toggleButton);

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_UP); n++;
  arrowButton = XmCreateArrowButton(mbox, "Higher?", args, n);
  XtManageChild(arrowButton);

  n = 0;
  tcs = XmStringLtoRCreate("Oh Susanna", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  drawnButton = XmCreateDrawnButton(mbox, "Oh Susanna", args, n);
  XmStringFree(tcs);
  XtManageChild(drawnButton);
  XtManageChild(mbox);

  CommonPause();

  /*  Destroy the message box so we can create another one  */
  XtDestroyWidget(mbox);

  /*
   * Create a MessageBox widget with XmDIALOG_TEMPLATE set as
   *  the XmNdialogType.
   */

  n = 0;
  tcs = XmStringLtoRCreate(
          "This is a XmDIALOG_TEMPLATE XmNdialogType MessageBox!  SMILE!!", 
			   XmSTRING_DEFAULT_CHARSET);

  smilemap = XCreatePixmapFromBitmapData(display,
	        DefaultRootWindow(display),
	   	smiley_bits, smiley_width,smiley_height,
		BlackPixel(display,DefaultScreen(display)),
		WhitePixel(display,DefaultScreen(display)),
		DefaultDepth(display,DefaultScreen(display)));

  tcs1 = XmStringCreate("Otay 'Panky", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNsymbolPixmap, smilemap); n++;
  XtSetArg(args[n], XmNdialogType, XmDIALOG_TEMPLATE); n++;
  XtSetArg(args[n], XmNokCallback, ok_cb); n++;
  XtSetArg(args[n], XmNokLabelString, tcs1); n++;
  XtSetArg(args[n], XmNcancelCallback, cancel_cb); n++;
  XtSetArg(args[n], XmNhelpCallback, help_cb); n++;
  XtSetArg(args[n], XmNautoUnmanage, False);  n++;
  XtSetArg(args[n], XmNminimizeButtons, False); n++;
  XtSetArg(args[n], XmNmessageString, tcs); n++;
  mbox = XmCreateMessageBox(Frame, "mbox", args, n);
  XtRealizeWidget(mbox);
  XmStringFree(tcs);

  n = 0;
  rowcol2 = XmCreateRowColumn(mbox, "rowcol2", args, n);
  XtRealizeWidget(rowcol2);

  n = 0;
  XtSetArg(args[n], XmNwidth, 200); n++;
  panedWindow = XmCreatePanedWindow(rowcol2, "Panedw", args, n);
  XtRealizeWidget(panedWindow);

  n = 0;
  tcs = XmStringCreate("I got a Nikon camera,  I can take a photograph,  Mama dont take my Kodachrome awayieieie", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 300); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  label3 = XmCreateLabel(rowcol2, "label3", args, n);
  XtRealizeWidget(label3);
  XmStringFree(tcs);
  XtManageChild(label3);

  simplearray[0] = XmStringCreate("Minolta", XmSTRING_DEFAULT_CHARSET);
  simplearray[1] = XmStringCreate("Fuji", XmSTRING_DEFAULT_CHARSET);
  simplearray[2] = XmStringCreate("Kodak", XmSTRING_DEFAULT_CHARSET);
  simplearray[3] = XmStringCreate("Canon", XmSTRING_DEFAULT_CHARSET);

  n = 0;
  XtSetArg(args[n], XmNbuttonCount, 4); n++;
  XtSetArg(args[n], XmNbuttonSet, 2); n++;
  XtSetArg(args[n], XmNbuttons, simplearray); n++;
  radbox1 = XmCreateSimpleRadioBox(panedWindow, "radbox1", args, n);
  XtRealizeWidget(radbox1);


  simplearray1[0] = XmStringCreate("Still Photos", XmSTRING_DEFAULT_CHARSET);
  simplearray1[1] = XmStringCreate("Action Shots", XmSTRING_DEFAULT_CHARSET);
  simplearray1[2] = XmStringCreate("Fake Poses", XmSTRING_DEFAULT_CHARSET);
  simplearray1[3] = XmStringCreate("Genuine Grins", XmSTRING_DEFAULT_CHARSET);

  n = 0;
  XtSetArg(args[n], XmNbuttonCount, 4); n++;
  XtSetArg(args[n], XmNbuttonSet, 3); n++;
  XtSetArg(args[n], XmNbuttons, simplearray1);n++;
  radbox2 = XmCreateSimpleRadioBox(panedWindow, "radbox2", args, n);
  XtRealizeWidget(radbox2);

  XtManageChild(radbox1);
  XtManageChild(radbox2);
  /* running with -u realize, we see PIR 4173 */
if (UserData && (strcmp (UserData, "realize") == 0)) 
     XtRealizeWidget (panedWindow);
  XtManageChild(panedWindow);
  XtManageChild(rowcol2);

  n = 0;
  tcs = XmStringLtoRCreate("Watch the birdie!", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  drawnButton = XmCreateDrawnButton(mbox, "Watch the birdie!", args, n);
  XmStringFree(tcs);
  XtManageChild(drawnButton);

  n = 0;
  tcs = XmStringLtoRCreate("Say Cheese...", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushButton = XmCreatePushButton(mbox, "Say Cheese...", args, n);
  XmStringFree(tcs);
  XtManageChild(pushButton);

  /* attempt to fix P4001 */ 
  XtRealizeWidget (mbox); 
  XtManageChild(mbox);
  
  CommonPause();

  /*  process events  */

  XtDestroyWidget (mbox);
  n = 0;
  XtSetArg (args[n], XmNwidth, 400); n++;
  XtSetArg (args[n], XmNheight, 400); n++;
  mbox = XmCreateTemplateDialog (Frame, "mbox", args, n);
  XtManageChild (mbox);

  CommonPause();

  CommonPause(); /* exit */
  XtAppMainLoop(app_context);

#else

  printf("This test is for 1.2 only\n");
  exit(0);

#endif
}
