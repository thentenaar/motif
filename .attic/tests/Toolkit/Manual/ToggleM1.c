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
static char rcsid[] = "$TOG: ToggleM1.c /main/8 1997/04/10 13:26:12 dbl $"
#endif
#endif


#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <testlib.h>


#include <smiley.bmp>

#define XBMu_BITS smiley_bits
#define XBMu_WIDTH smiley_width
#define XBMu_HEIGHT smiley_height

int which_way;
Pixmap pix;

char smileRaster[] =
{
   0xff, 0xff, 0xff, 0x01, 0x00, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0x3f, 0x00, 0x00, 0xe0, 0xff, 0xff,
   0xff, 0xff, 0x0f, 0xfc, 0xff, 0xc1, 0xff, 0xff,
   0xff, 0xff, 0x83, 0xff, 0xff, 0x0f, 0xff, 0xff,
   0xff, 0xff, 0xf0, 0xff, 0xff, 0x3f, 0xfc, 0xff,
   0xff, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0xf8, 0xff,
   0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff,
   0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff,
   0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff,
   0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff,
   0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff,
   0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff,
   0x7f, 0xfc, 0x1f, 0xfe, 0x7f, 0xf8, 0x3f, 0xff,
   0x7f, 0xfe, 0xe7, 0xf9, 0x9f, 0xe7, 0x3f, 0xff,
   0x3f, 0xff, 0xfb, 0xf7, 0xef, 0xdf, 0x7f, 0xfe,
   0x3f, 0xff, 0xfd, 0xef, 0xf7, 0xbf, 0x7f, 0xfe,
   0x3f, 0xff, 0xfd, 0xef, 0xf7, 0xbf, 0xff, 0xfc,
   0x9f, 0xff, 0xfe, 0xdf, 0xfb, 0x7f, 0xff, 0xfc,
   0x9f, 0xff, 0xfe, 0xdf, 0xfb, 0x7f, 0xff, 0xf9,
   0x9f, 0xff, 0xfe, 0xc1, 0xfb, 0x07, 0xff, 0xf9,
   0xcf, 0xff, 0xfe, 0xc0, 0xfb, 0x03, 0xff, 0xf9,
   0xcf, 0xff, 0x7d, 0xe0, 0xf7, 0x81, 0xff, 0xf3,
   0xcf, 0xff, 0x3d, 0xe0, 0xf7, 0x80, 0xff, 0xf3,
   0xcf, 0xff, 0x3b, 0xf0, 0xef, 0xc0, 0xff, 0xf3,
   0xcf, 0xff, 0x27, 0xf8, 0x9f, 0xe0, 0xff, 0xf3,
   0xcf, 0xff, 0x1f, 0xfe, 0x7f, 0xf8, 0xff, 0xf3,
   0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
   0xcf, 0xef, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf3,
   0xcf, 0xef, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf3,
   0xcf, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xf3,
   0xcf, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xf3,
   0xcf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfb,
   0xdf, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0xfb,
   0x9f, 0xff, 0xfc, 0xff, 0xff, 0x9f, 0xff, 0xf9,
   0x9f, 0xff, 0xfb, 0xff, 0xff, 0xcf, 0xff, 0xf9,
   0x3f, 0xff, 0xef, 0xff, 0xff, 0xf3, 0xff, 0xfc,
   0x3f, 0xff, 0x1f, 0xff, 0x7f, 0xf8, 0xff, 0xfc,
   0x3f, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xfe,
   0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff,
   0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff,
   0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff,
   0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff,
   0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff,
   0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff,
   0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff,
   0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff,
   0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff,
   0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff,
   0xff, 0x3f, 0xfe, 0xff, 0xff, 0x3f, 0xfe, 0xff,
   0xff, 0x7f, 0xfc, 0xff, 0xff, 0x1f, 0xff, 0xff,
   0xff, 0xff, 0xf0, 0xff, 0xff, 0xc3, 0xff, 0xff,
   0xff, 0xff, 0xe1, 0xff, 0xff, 0xe0, 0xff, 0xff,
   0xff, 0xff, 0x03, 0xff, 0x3f, 0xf8, 0xff, 0xff,
   0xff, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x00, 0xc0, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

Pixmap select_pixmap;

void main (argc, argv)
unsigned int argc;
char **argv;
{
  int n;
  XImage *pic;
  Widget toggle, bb, RowColumn1, RowColumn2, ToggleButtonG1, ToggleButtonG2,
          ToggleButtonG3, ToggleButtonG4, ToggleButtonG5, ToggleButtonG6;
  Widget history, ToggleButton1, ToggleButton2, ToggleButtonGadget1;
  Arg args[10];
  Screen *screen;
  Pixel black;
  Pixmap pixOn, pixOff;
  GC      gc;
  XGCValues  values;
  XtGCMask valueMask; 
  Drawable drawable;
  extern Pixel CommonGetColor();

  CommonTestInit(argc, argv);

  n = 0;
  bb = XmCreateBulletinBoard(Shell1, "bb", args, n);
 
  screen = XtScreen(Shell1);
  pic = XCreateImage(display, XDefaultVisualOfScreen(screen), 1, 
		     XYBitmap, 0, smileRaster, 64, 64,
		     BitmapPad(display), 8);

  pic->byte_order = MSBFirst;
  pic->bitmap_bit_order = LSBFirst;
  pic->bitmap_unit = 8;

  drawable = RootWindowOfScreen(XtScreen(Shell1));
  pixOn = XCreatePixmap(display, drawable, 64, 64,
			DefaultDepthOfScreen(screen));    
  pixOff = XCreatePixmap(display, drawable, 64, 64,
			 DefaultDepthOfScreen(screen));    

  values.foreground = XBlackPixel(display, 0);
  values.background = XWhitePixel(display, 0);
  valueMask = GCForeground | GCBackground;

  gc = XCreateGC(display, drawable, valueMask, &values);
  XPutImage(display, pixOn, gc, pic, 0, 0, 0, 0, 64, 64);
    
  values.foreground = XWhitePixel(display, 0);
  values.background = XBlackPixel(display, 0);
  valueMask = GCForeground | GCBackground;
  XChangeGC(display, gc, valueMask, &values);
  XPutImage(display, pixOff, gc, pic, 0, 0, 0, 0, 64, 64);
  
  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP);  n++;
  XtSetArg(args[n], XmNlabelPixmap, pixOff);  n++;  
  XtSetArg(args[n], XmNselectPixmap, pixOn); n++;
  XtSetArg(args[n], XmNlabelInsensitivePixmap, pixOn);  n++; 
  XtSetArg(args[n], XmNselectInsensitivePixmap, pixOff);  n++; 
  toggle = XmCreateToggleButton(bb, "toggle" , args, n); 

  XtManageChild (bb);
  XtManageChild (toggle);
  XtRealizeWidget (Shell1);

  CommonPause();			/* change button face */
  n=0;
  XtSetArg(args[n], XmNsensitive, False); n++;
  XtSetValues (toggle, args, n);

  CommonPause();

  XtDestroyWidget(toggle);
  n = 0;
  black = CommonGetColor("Black");
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  XtSetArg(args[n], XmNselectColor, black); n++;
  XtSetArg(args[n], XmNtopShadowColor, black); n++;
  XtSetArg(args[n], XmNbottomShadowColor, black); n++;
  toggle = XmCreateToggleButton(bb, "toggle", args, n);
  XtManageChild(toggle);

  CommonPause();

  XtDestroyWidget(toggle);

  /*
   *  Create a test for PIR 2430
   */

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg(args[n], XmNbackground, CommonGetColor("black")); n++;
  XtSetArg(args[n], XmNforeground, CommonGetColor("orange")); n++;
  RowColumn1 = XmCreateRadioBox(bb, "RowColumn1", args, n);
  XtManageChild (RowColumn1);

  XtSetArg(args[n], XmNx, 125); n++;   /* add an offset */
  RowColumn2 = XmCreateRadioBox(bb, "RowColumn2", args, n);
  XtManageChild (RowColumn2);
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  ToggleButtonG1 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG1",
					      args, n);
  ToggleButtonG4 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG4",
					      args, n);
  ToggleButtonG2 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG2",
					      args, n);
  ToggleButtonG5 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG5",
					      args, n);
  ToggleButtonG3 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG3",
					      args, n);
  ToggleButtonG6 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG6",
					      args, n);
  XtManageChild(ToggleButtonG1);
  XtManageChild(ToggleButtonG4);
  XtManageChild(ToggleButtonG2);
  XtManageChild(ToggleButtonG5);
  XtManageChild(ToggleButtonG3);
  XtManageChild(ToggleButtonG6);
  
  /* Now change the colors of the second radiobox */

  n = 0;
  XtSetArg(args[n], XmNforeground, CommonGetColor("blue")); n++;
  XtSetArg(args[n], XmNbackground, CommonGetColor("magenta")); n++;
  XtSetValues(RowColumn2, args, n);

  CommonPause();


  /* Begin Test Case for PIR 2963 */

  XtDestroyWidget(bb);
  XtUnrealizeWidget(Shell1); 
  
  n = 0;
  XtSetArg(args[n], XmNwidth, 100); n++;
  RowColumn1 = XmCreateRadioBox(Shell1, "RadioBox", args, n);

  ToggleButton1 = XmCreateToggleButton(RowColumn1, "ToggleButton1", NULL, 0);
  ToggleButton2 = XmCreateToggleButton(RowColumn1, "ToggleButton2", NULL, 0);
  XtManageChild(ToggleButton2);
  XtManageChild(ToggleButton1);
  XtManageChild(RowColumn1);

  XtSetArg(args[0], XmNmenuHistory, &history);
  XtGetValues(RowColumn1, args, 1);
  printf("Default menuHistory = %ld\n", (long int) history);

  XmToggleButtonSetState(ToggleButton1, True, False);

  XtSetArg(args[0], XmNmenuHistory, &history);
  XtGetValues(RowColumn1, args, 1);
  printf("menuHistory after SetState = %ld\n", (long int) history);

  XtRealizeWidget(Shell1);

  XtSetArg(args[0], XmNmenuHistory, &history);
  XtGetValues(RowColumn1, args, 1);
  printf("menuHistory after Shell's been realized = %ld\n", 
	 (long int) history);

  CommonPause();

  /* test for CR 5130 */

  CommonPause();

  XtDestroyWidget (ToggleButton1);
  XtDestroyWidget (ToggleButton2);
  ToggleButtonGadget1 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonGadget1", NULL, 0);
  XtManageChild (ToggleButtonGadget1);
  
  CommonPause();

  XtAppMainLoop(app_context);
}
