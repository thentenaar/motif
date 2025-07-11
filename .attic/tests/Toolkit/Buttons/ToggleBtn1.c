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
static char rcsid[] = "$XConsortium: ToggleBtn1.c /main/9 1995/07/13 17:32:22 drk $"
#endif
#endif


 

/****************************************
*   Automatic test program generation   *
*       Fri Mar 17 14:54:45 1989        *
****************************************/
#include <testlib.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>
#include "smiley.bmp"

#define XBMs_BITS        smiley_bits
#define XBMs_WIDTH       smiley_width
#define XBMs_HEIGHT      smiley_height
Pixmap px_select;

#include "circles1.bmp"
#define XBMu_BITS        circles1_bits
#define XBMu_WIDTH       circles1_width
#define XBMu_HEIGHT      circles1_height
Pixmap px_unselect;

void toggleCB();

static XtCallbackRec 
  valuechangedCR[] = {{ toggleCB, NULL}, {NULL, NULL},},
  armCR[] = {{ toggleCB, NULL},  {NULL, NULL},},
  disarmCR[] = {{ toggleCB, NULL}, {NULL, NULL},};

/*
**  Globals
*/

XWindowAttributes  info;
Boolean            trace = False;
int                delay = 0;
Arg                args[20];
int                n;
XmString           tcs;
XEvent             DummyEvent;

Widget VPaned1 = NULL, ToggleButton1 = NULL;
Widget ToggleButton2 = NULL, ToggleButton3 = NULL, ToggleButton4 = NULL;
Widget ToggleButton5 = NULL, ToggleButton6 = NULL;

void toggleCB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
  printf("Toggle Callback Structure:\n    Reason:   ");
  
  switch( ((XmToggleButtonCallbackStruct*)call_data)->reason) {
  case XmCR_VALUE_CHANGED: 
    printf("XmCR_VALUE_CHANGED\n"); 
    break;
  case XmCR_ARM: 		
    printf("XmCR_ARM\n"); 
    break;
  case XmCR_DISARM: 	
    printf("XmCR_DISARM\n"); 
    break;
  default: 		
    printf("Oops - unknown callback reason!\n"); 
    break;
    }
}

void  main(argc, argv)
int     argc;
char  **argv;
{

  XmFontList fontlist; 
  XmStringCharSet  cset = "isolatin6";
  XFontStruct *newfont;
  
  CommonTestInit(argc, argv);
    
  XmRegisterConverters();

  newfont = XLoadQueryFont (display, "9x15");


  n = 0;
  VPaned1 = XmCreatePanedWindow(Shell1, "VPaned1", args, n);
  XtManageChild(VPaned1);

  n = 0;
  fontlist = XmFontListCreate (newfont, cset);
  tcs = XmStringLtoRCreate("Toggle1", cset);
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
  XtSetArg(args[n], XmNarmCallback, armCR);  n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
  ToggleButton1 = XmCreateToggleButton(VPaned1, "ToggleButton1",
				       args, n);
  if(tcs) XmStringFree(tcs);
  XtManageChild(ToggleButton1);

  n = 0;
  XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY);  n++;
  XtSetArg(args[n], XmNspacing, 10);  n++;
  XtSetArg(args[n], XmNfillOnSelect, False);  n++;
  ToggleButton2 = XmCreateToggleButton(VPaned1, "ToggleButton2",
				       args, n);
  XtManageChild(ToggleButton2);

  n = 0;
  XtSetArg(args[n], XmNvisibleWhenOff, False);  n++;
  ToggleButton3 = XmCreateToggleButton(VPaned1, "ToggleButton3",
				       args, n);
  XtManageChild(ToggleButton3);

  n = 0;
  XtSetArg(args[n], XmNset, True);  n++;
  XtSetArg(args[n], XmNfillOnSelect, True);  n++;
  ToggleButton4 = XmCreateToggleButton(VPaned1, "ToggleButton4",
				       args, n);
  XtManageChild(ToggleButton4);

  px_unselect =
    XCreatePixmapFromBitmapData(display, 
				DefaultRootWindow(display),
				XBMu_BITS, XBMu_WIDTH,XBMu_HEIGHT,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));
  px_select = 
    XCreatePixmapFromBitmapData(display, 
				DefaultRootWindow(display),
				XBMs_BITS, XBMs_WIDTH,XBMs_HEIGHT,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));
  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP);  n++;
  XtSetArg(args[n], XmNselectPixmap, px_select);  n++;
  XtSetArg(args[n], XmNlabelPixmap, px_unselect);  n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  ToggleButton5 = XmCreateToggleButton(VPaned1, "ToggleButton5",
				       args, n);
  XtManageChild(ToggleButton5);
  
  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False);  n++;
  XtSetArg(args[n], XmNshadowThickness, 4); n++;
  ToggleButton6 = XmCreateToggleButton(VPaned1, "ToggleButton6",
				       args, n);
  XtManageChild(ToggleButton6);

  n = 0;
  tcs = XmStringLtoRCreate("This is\nToggle2", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetValues(ToggleButton2, args, n);
  if(tcs) XmStringFree(tcs);

  printf("Toggle1 state = %d\n", XmToggleButtonGetState(ToggleButton1));
  XmToggleButtonSetState(ToggleButton1, TRUE, FALSE);
  printf("Toggle1 state = %d\n", XmToggleButtonGetState(ToggleButton1));
  XmToggleButtonSetState(ToggleButton1, FALSE, TRUE);
  printf("Toggle1 state = %d\n", XmToggleButtonGetState(ToggleButton1));
  XtRealizeWidget(Shell1);

  for (n = 0; n < 5; n++)
    CommonPause();                  

  XmToggleButtonSetState(ToggleButton5, TRUE, FALSE);
  CommonPause();

  XmToggleButtonSetState(ToggleButton5, FALSE, TRUE);
  CommonPause();

  n = 0;
  XtSetArg (args[n], XmNbackgroundPixmap, px_select);	n++;
  XtSetValues (ToggleButton5, args, n);

  CommonPause();

  n = 0;
  XtSetArg (args[n], XmNbackgroundPixmap, XmUNSPECIFIED_PIXMAP);   n++;
  XtSetValues (ToggleButton5, args, n);

  CommonPause();
  /* END */

  XtDestroyWidget(VPaned1);
  CommonPause();

  XtAppMainLoop(app_context);
}

