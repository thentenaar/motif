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
static char rcsid[] = "$XConsortium: ToggleBGad4.c /main/9 1995/07/13 17:31:50 drk $"
#endif
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <testlib.h>
#include "bitmaps/circles1.bmp"

#define XBMu_BITS        circles1_bits
#define XBMu_WIDTH       circles1_width
#define XBMu_HEIGHT      circles1_height
#define mtn_width 50
#define mtn_height 50

Widget WidgetRC;
Widget StateButton1, StateButton2;
Widget TButton1, TButton2, TButton3;
Widget ToggleW1, ToggleW2;

void  override_msg();

static XtCallbackRec Override_CB[] = {
   {override_msg, NULL},
   {NULL, NULL}};

Pixmap px_unselect;

static char mtn_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
   0x00, 0x00, 0x70, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0xfc,
   0x00, 0x01, 0x00, 0x00, 0xfc, 0x01, 0xfc, 0x80, 0x03, 0x00, 0x00, 0xfc,
   0x01, 0xfc, 0x81, 0x03, 0x00, 0x00, 0xfc, 0x01, 0xfe, 0xc1, 0x07, 0x00,
   0x00, 0xf8, 0x00, 0xfe, 0xe1, 0x0f, 0x00, 0x00, 0x70, 0x00, 0xff, 0x73,
   0x1f, 0x00, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x3c, 0x00, 0x00, 0x00, 0x80,
   0xff, 0x05, 0x78, 0x00, 0xc0, 0x00, 0x80, 0xfd, 0x0c, 0x88, 0x00, 0xc0,
   0x01, 0x80, 0x78, 0x08, 0x08, 0x01, 0xe0, 0x01, 0x40, 0x30, 0x08, 0x10,
   0x02, 0xe0, 0x03, 0x40, 0x20, 0x10, 0x20, 0x02, 0xe0, 0x03, 0x20, 0x20,
   0x10, 0x20, 0x00, 0xf0, 0x07, 0x20, 0x20, 0x20, 0x40, 0x00, 0xf0, 0x07,
   0x30, 0x20, 0x20, 0x00, 0x00, 0xf0, 0x09, 0x08, 0x20, 0xc0, 0x00, 0x00,
   0x70, 0x19, 0x0c, 0x60, 0x80, 0x00, 0x00, 0x10, 0x31, 0x04, 0x40, 0x80,
   0x00, 0x00, 0x18, 0x61, 0x04, 0xc0, 0x00, 0x03, 0x00, 0x08, 0xc1, 0x02,
   0x80, 0x00, 0x06, 0x00, 0x0c, 0x01, 0x03, 0x80, 0x00, 0x0c, 0x00, 0x04,
   0x02, 0x03, 0x00, 0x03, 0x08, 0x00, 0x04, 0x06, 0x06, 0x00, 0x02, 0x10,
   0x00, 0x04, 0x04, 0x0c, 0x00, 0x02, 0x10, 0x00, 0x02, 0x04, 0x10, 0x00,
   0x02, 0x10, 0x00, 0x03, 0x0c, 0x00, 0x00, 0x06, 0x20, 0x00, 0x01, 0x00,
   0x20, 0x00, 0x04, 0x60, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0xc0, 0x00,
   0x00, 0x10, 0x00, 0x01, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,
   0x00, 0x00, 0x00, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

static XtCallbackProc activate_msg(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 
{
  printf("activate callback\n");
}

/* extern void XtManageChild(); */
static XtCallbackProc ManageSB1 (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{
  XtManageChild (StateButton1);
}

static XtCallbackProc ManageSB2 (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{
  XtManageChild (StateButton2);
}

void ToggleGStates (widget)
/* switch ON toggles to OFF and vice-versa*/
Widget widget;
{
  Boolean state;
  state = XmToggleButtonGadgetGetState(widget);
  if (state == True) 
    XmToggleButtonGadgetSetState(widget, False, False);
  else
    XmToggleButtonGadgetSetState(widget, True, False);
}

void ToggleWStates (w)
Widget w;
{
  Boolean state;
   
  state = XmToggleButtonGetState(w);
  if (state == True)
    XmToggleButtonSetState (w, False, False);
  else
    XmToggleButtonSetState (w, True, False);
}

static XtCallbackProc ToggleWidgets (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{
  ToggleWStates (ToggleW1);
  ToggleWStates (ToggleW2);
}

static XtCallbackProc ManageWidgetRC (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{
  XmString tcs;
  int n;
  Arg args[1];

  if (!XtIsManaged (WidgetRC)) 
    {  
      tcs = 
	XmStringCreateLtoR ("Toggle Gadget States", XmSTRING_DEFAULT_CHARSET);
      n = 0;
      XtSetArg (args[n], XmNlabelString, tcs); n++;
      XtSetValues (StateButton1, args, n);
      XtManageChild (WidgetRC);
    }
  else 
    {
      ToggleGStates(TButton1); 
      ToggleGStates(TButton2);
      ToggleGStates(TButton3);
    }
}

static XtCallbackProc arm_msg(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
  printf("arm callback\n");
}

void override_msg(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
  printf("override callback\n");
}

static XtCallbackProc Modified(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmTextVerifyCallbackStruct *call_data;
{
  XmTextVerifyCallbackStruct *cb;

  cb = call_data;
  if (strncmp(cb->text->ptr, "q", 1) == 0)
    cb->doit = False;
  printf("modifyVerifyCallback Recieved.\n");
}

static XtCallbackProc CursorMoved(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmTextVerifyCallbackStruct *call_data;
{
  XmTextVerifyCallbackStruct *cb;

  cb = call_data;
  if (cb->newInsert > 20)
         cb->doit = False;
     printf("motionVerifyCallback Recieved. newInsert = %d\n", cb->newInsert);
}

static XtCallbackProc FocusLost(w, client_data, call_data)
Widget w;
caddr_t client_data;
XmTextVerifyCallbackStruct *call_data;
{
     XmTextVerifyCallbackStruct *cb;

     cb = call_data;
      
     printf("losingFocusCallback Recieved.\n");
}

void main (argc, argv)
int argc;
char **argv;
{
   Widget PanedWindow1;
   Widget RowColumn1;

   Pixel red_pixel;
   Pixmap pixmap;
   XmString tcs;
   Arg args[20];
   int n;

   /*  initialize toolkit  */
    CommonTestInit(argc, argv);

   pixmap = 
     XCreatePixmapFromBitmapData(display,
				 DefaultRootWindow (display),
				 mtn_bits, mtn_width, mtn_height,
				 WhitePixel(display,DefaultScreen(display)),
				 BlackPixel(display,DefaultScreen(display)),
				 DefaultDepth(display,DefaultScreen(display)));

   px_unselect =
     XCreatePixmapFromBitmapData(display,
				 DefaultRootWindow(display),
				 XBMu_BITS, XBMu_WIDTH,XBMu_HEIGHT,
				 WhitePixel(display,DefaultScreen(display)),
				 BlackPixel(display,DefaultScreen(display)),
				 DefaultDepth(display,DefaultScreen(display)));

   n = 0;
   XtSetArg(args[n], XmNwidth, 300); n++;
   PanedWindow1 = 
     XmCreatePanedWindow(Shell1, "PanedWindow1", args, n);
   XtManageChild(PanedWindow1);

   red_pixel = CommonGetColor("red");

   n = 0;
   XtSetArg(args[n], XmNhighlightColor, red_pixel); n++;
   XtSetArg(args[n], XmNallowResize, True); n++;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNborderWidth, 0); n++;

   RowColumn1 = XmCreateRowColumn( PanedWindow1, "RowColumn1", args, n);
   XtManageChild(RowColumn1);

   n = 0;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
   XtSetArg(args[n], XmNlabelPixmap, px_unselect); n++;
   XtSetArg(args[n], XmNselectPixmap, pixmap); n++;
   TButton1 = XmCreateToggleButtonGadget(RowColumn1, "TButton1", args, n);
   XtManageChild (TButton1);

   XtAddCallback(TButton1, XmNvalueChangedCallback, 
		 (XtCallbackProc) activate_msg, (caddr_t) NULL);
   XtAddCallback(TButton1, XmNarmCallback, (XtCallbackProc) arm_msg,
                 (caddr_t) NULL);
   n = 0;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   TButton2 = XmCreateToggleButtonGadget(RowColumn1, "TButton2", args, n);
   XtManageChild (TButton2); 
   XtAddCallback(TButton2, XmNvalueChangedCallback, 
		 (XtCallbackProc) activate_msg, (caddr_t) NULL);
   XtAddCallback(TButton2, XmNarmCallback, (XtCallbackProc) arm_msg,
                 (caddr_t) NULL);

   n = 0;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   TButton3 = XmCreateToggleButtonGadget(RowColumn1, "TButton3", args, n);
   XtAddCallback(TButton3, XmNvalueChangedCallback, 
		 (XtCallbackProc) ManageSB1, (caddr_t) NULL);
   XtManageChild(TButton3);
   XmAddTabGroup(RowColumn1);

   tcs = XmStringCreateLtoR ("More widgets...", XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg (args[n], XmNlabelString, tcs); n++;
   XtSetArg (args[n], XmNshadowThickness, 2); n++;
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   XtSetArg (args[n], XmNhighlightThickness, 4); n++;
   XtSetArg (args[n], XmNhighlightColor, red_pixel); n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg (args[n], XmNwidth, 200); n++;
   StateButton1 = XmCreatePushButton(PanedWindow1, "StateButton1", args, n);
   if (XtIsManaged(StateButton1))
	XtUnmanageChild(StateButton1);
   XtAddCallback(StateButton1, XmNactivateCallback, 
		 (XtCallbackProc) ManageWidgetRC, (caddr_t) NULL);

   /* and this will manage a row column with the following Toggle widgets */

   n = 0;
   WidgetRC = XmCreateRadioBox(PanedWindow1, "WidgetRC", args, n);
   if (XtIsManaged(WidgetRC))
	XtUnmanageChild(WidgetRC);
   
   tcs = XmStringCreateLtoR ("Toggle Widget 1", XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg (args[n], XmNlabelString, tcs); n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   XtSetArg (args[n], XmNhighlightThickness, 2); n++;
   XtSetArg (args[n], XmNhighlightColor, red_pixel); n++;
   ToggleW1 = XmCreateToggleButton (WidgetRC, "ToggleW1", args, n);
   XtManageChild (ToggleW1);
   XtAddCallback(ToggleW1, XmNvalueChangedCallback, (XtCallbackProc) ManageSB2, (caddr_t) NULL);

   tcs = XmStringCreateLtoR ("Toggle Widget 2", XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg (args[n], XmNlabelString, tcs); n++;
   XtSetArg (args[n], XmNindicatorOn, False); n++;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY); n++;
   XtSetArg (args[n], XmNshadowThickness, 2); n++;
   XtSetArg (args[n], XmNhighlightColor, red_pixel); n++;
   XtSetArg (args[n], XmNhighlightThickness, 4); n++;
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   ToggleW2 = XmCreateToggleButton (WidgetRC, "ToggleW2", args, n);
   XtManageChild (ToggleW2);

   /* button 1 above manages StateButton2*/
   tcs = 
     XmStringCreateLtoR ("Toggle Widget States...", XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg (args[n], XmNlabelString, tcs); n++;
   XtSetArg (args[n], XmNshadowThickness, 2); n++;
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   XtSetArg (args[n], XmNhighlightThickness, 4); n++;
   XtSetArg (args[n], XmNhighlightColor, red_pixel); n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg (args[n], XmNwidth, 200); n++;
   StateButton2 = XmCreatePushButton(PanedWindow1, "StateButton2", args, n);
   if (XtIsManaged(StateButton2))
	XtUnmanageChild(StateButton2);
   XtAddCallback(StateButton2, XmNactivateCallback, 
		 (XtCallbackProc) ToggleWidgets, (caddr_t) NULL);
   XmAddTabGroup (StateButton2);
	
   XmAddTabGroup (WidgetRC);
   XmAddTabGroup (PanedWindow1);
   
   XtRealizeWidget(Shell1);

   CommonPause();                             /* tab group */
   CommonPause();                             /* functions */
   CommonPause();
   CommonPause();

   /*
    * Exit
    */

   XtDestroyWidget(PanedWindow1);

   CommonPause();				/* exit */

   XtAppMainLoop(app_context);
}










