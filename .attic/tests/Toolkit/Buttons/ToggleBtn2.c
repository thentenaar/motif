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
static char rcsid[] = "$XConsortium: ToggleBtn2.c /main/11 1995/07/13 17:32:34 drk $"
#endif
#endif

/****************************************************************************
 *
 * This program tests  a toggle pixmap, and the XmNindicatorOn resource.\
 *
 ****************************************************************************/
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <testlib.h>

#include "smiley.bmp"

#define XBMu_BITS smiley_bits
#define XBMu_WIDTH smiley_width
#define XBMu_HEIGHT smiley_height

int which_way;
Pixmap pix;

Widget mainw, rc, rc2;

/* forward refs */

void GrowBW (Widget w, XtPointer client_data, XtPointer call_data);

/*
** do something to the widgets
*/

void swap(Widget w, XtPointer client_data, XtPointer call_data)
{
  switch (which_way)
    {
    default:
    case 0:
      break;
    case 1:
      {
	Arg args[1];
	unsigned char labelType;
	XtSetArg(args[0],XmNlabelType, &labelType);
	XtGetValues(w,args,1);
	labelType = (labelType==XmSTRING?XmPIXMAP:XmSTRING);
	XtSetArg(args[0],XmNlabelType, labelType);
	XtSetValues(w,args,1);
	break;
      }
    }
}

void main (argc, argv)
unsigned int argc;
char **argv;
{
  Cardinal n;
  XmStringCharSet cset = "isolatin6";
  Arg args[MAX_ARGS];
  Widget w, tb[20];
  XmFontList fontlist;
  XFontStruct *newfont;
  XmString tcs;
  int i, j;
  char s[10];
  /* begin test for CR 3466 */
  static int widths[] = { 2, 5, 10 };
  /* end test for CR 3466 */

  CommonTestInit(argc, argv);

#ifdef MOTIF1_1
  n = 0;
  XtSetArg(args[n], XmNwidth, 10); n++;
  XtSetArg(args[n], XmNheight, 10); n++;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);
#endif
  XtRealizeWidget (Shell1);

  newfont = XLoadQueryFont(display, "6x13");

  /* Begin Test for PIR 2651 */

  XtManageChild(mainw = XmCreateMainWindow(Shell1, "mainw", NULL, 0));
  rc = XmCreateRowColumn(mainw, "rc", NULL, 0);
  XtManageChild(rc);

  XmMainWindowSetAreas(mainw, NULL, NULL, NULL, NULL, rc);

  fontlist = XmFontListCreate(newfont, cset);

  pix = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
				 XBMu_BITS, XBMu_WIDTH, XBMu_HEIGHT,
				 WhitePixel(display, DefaultScreen(display)),
				 BlackPixel(display, DefaultScreen(display)),
				 DefaultDepth(display,DefaultScreen(display)));
  n = 0;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 1", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb1", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 1", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg1", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 2", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb2", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 2", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg2", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 3", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb3", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 3", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg3", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 4", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb4", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 4", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg4", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 5", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb5", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 5", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg5", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 6", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb6", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 6", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg6", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 7", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb7", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 7", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg7", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0;
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Widget 8", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButton(rc, "tb8", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  n = 0; 
  XtSetArg(args[n], XmNindicatorSize, 12); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNfontList, fontlist); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  tcs = XmStringCreateLtoR("Toggle Button Gadget 8", cset);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNlabelPixmap, pix); n++;
  XtManageChild(w=XmCreateToggleButtonGadget(rc, "tbg8", args,n));
  XtAddCallback(w, XmNvalueChangedCallback, swap, NULL);

  XtRealizeWidget(Shell1);

  which_way=0;
  CommonPause();

  which_way=1;
  CommonPause();

  /*  
   * Test for PIR 4438; create 20 toggle buttons, update color, and
   *  see if a core dump occurs due to GC's being freed too many times 
   */

#ifdef BESTCASE
  XtDestroyWidget(rc);
#else
  XtUnmanageChild(rc);
#endif

  rc2 = XmCreateRowColumn(mainw, "rc2", NULL, 0);
  XtManageChild(rc2);

  XmMainWindowSetAreas(mainw, NULL, NULL, NULL, NULL, rc2);

  for (i=0; i<20; i++)
    {
      sprintf(s, "tbtn%d", i);
      tb[i] = XmCreateToggleButton(rc2, s, NULL, 0);
      XtManageChild(tb[i]);
    }

  n = 0;
  XtSetArg(args[n], XmNbackground, CommonGetColor("yellow")); n++;
  XtSetValues(tb[0], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("red")); n++;
  XtSetValues(tb[1], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("pink")); n++;
  XtSetValues(tb[2], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("green")); n++;
  XtSetValues(tb[3], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("lightgrey")); n++;
  XtSetValues(tb[4], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("orange")); n++;
  XtSetValues(tb[5], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("magenta")); n++;
  XtSetValues(tb[6], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("maroon")); n++;
  XtSetValues(tb[7], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("forestGreen")); n++;
  XtSetValues(tb[8], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("black")); n++;
  XtSetValues(tb[9], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("cadetBlue")); n++;
  XtSetValues(tb[10], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("orangered")); n++;
  XtSetValues(tb[11], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("lightgrey")); n++;
  XtSetValues(tb[12], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("darkorchid")); n++;
  XtSetValues(tb[13], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("navy")); n++;
  XtSetValues(tb[14], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("goldenrod")); n++;
  XtSetValues(tb[15], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("seagreen")); n++;
  XtSetValues(tb[16], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("wheat")); n++;
  XtSetValues(tb[17], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("brown")); n++;
  XtSetValues(tb[18], args, n);
  XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
  XtSetValues(tb[19], args, n);

  CommonPause();

  /* begin test for CR 3466 */

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNnumColumns, 2); n++;
  XtSetValues (rc2, args, n);

  for (i = 0; i < 20; i++) {
	XtSetArg (args[0], XmNborderWidth, widths[0]);
	XtSetValues (tb[i], args, 1);
     }

  CommonPause();       

  for (i = 0; i < 20; i++) {
	XtSetArg (args[0], XmNborderWidth, widths[1]);
	XtSetValues (tb[i], args, 1);
     }

  CommonPause();

  for (i = 0; i < 20; i++) {
	XtSetArg (args[0], XmNborderWidth, widths[2]);
	XtSetValues (tb[i], args, 1);
	XtAddCallback (tb[i], XmNvalueChangedCallback, GrowBW, NULL);
     }

  /* end test for CR 3466 */
 
  CommonPause();

  CommonPause(); /* exit */
  XtAppMainLoop(app_context);
}

void GrowBW (Widget w, XtPointer client_data, XtPointer call_data)
{
   Dimension orig, new;
   Arg args[1];

   XtSetArg (args[0], XmNborderWidth, &orig); 
   XtGetValues (w, args, 1);

   XtSetArg (args[0], XmNborderWidth, orig + 10); 
   XtSetValues (w, args, 1);

   XtSetArg (args[0], XmNborderWidth, &new); 
   XtGetValues (w, args, 1);

   printf ("Grow width of widget %s test %s\n", XtName (w),
	   (new == orig + 10 ? "PASSED" : "FAILED"));
   fflush (stdout);

}
