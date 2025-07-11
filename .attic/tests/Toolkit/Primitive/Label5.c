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
static char rcsid[] = "$TOG: Label5.c /main/8 1999/01/29 13:48:09 jff $"
#endif
#endif

/*
 *  Read a set of pixmaps using XmGetPixmap, then create a drawing area
 *  with a set of labels containing the pixmaps.
 */

#include <testlib.h>

static char *bitmapNames[8] = {
	"../bitmaps/tie_fighter",
	"../bitmaps/gnu.bmp",
	"../bitmaps/warning",
	"../bitmaps/mtn.bmp",
	"../bitmaps/smiley.bmp",
	"../bitmaps/dp.bmp",
	"../bitmaps/circles1.bmp",
	""
};

void main (argc, argv)
unsigned int argc;
char **argv;

{
  Widget drawarea;
  Widget separator;
  static char *label_names[15] = {"label1","label2","label3","label4",
		            "label5","label6","label7","label8",
		            "label9","label10","label11","label12",
		            "label13","label14","label15"};
  static Widget labels[15] = 
    {NULL, NULL,NULL, NULL,NULL, NULL,NULL, NULL,NULL, NULL,
       NULL, NULL,NULL, NULL,NULL};
  Pixmap pixmap;
  Arg args[10];
  int j = 0, n;
  XmString tcs;
  Pixel foreground, background;

  CommonTestInit(argc, argv);

  /*  The drawing area creation.  Contained in an outer frame.  */

  n = 0;
  XtSetArg (args[n], XmNmarginWidth, 10);	n++;
  XtSetArg (args[n], XmNmarginHeight, 10); 	n++;
  drawarea = XmCreateRowColumn (Shell1, "drawarea", args, n);
  XtManageChild (drawarea);
  XtRealizeWidget (Shell1);

  n = 0;
  XtSetArg (args[n], XmNforeground, &foreground);	n++;
  XtSetArg (args[n], XmNbackground, &background); 	n++;
  XtGetValues(drawarea, args, n);

  while (bitmapNames[j][0]) {

    n = 0;
    tcs = XmStringLtoRCreate("Oops! error", XmSTRING_DEFAULT_CHARSET);
    XtSetArg (args[n], XmNlabelString, tcs);	n++;
    labels[j] = XmCreateLabel (drawarea, label_names[j], args, n);
    XtManageChild (labels[j]);
    XmStringFree (tcs);
    
    printf ("Reading image %s\n", bitmapNames[j]);

    if ((pixmap = XmGetPixmap (XtScreen(drawarea), bitmapNames[j],
				foreground, background)) == NULL)
      {
	printf ("No pixmap returned from XmGetPixmap.\n");
	j++;
	continue;
      }
   
    n = 0;
    XtSetArg (args[n], XmNx, j * 20 + 5);	n++;
    XtSetArg (args[n], XmNlabelType, XmPIXMAP);	n++;
    XtSetArg (args[n], XmNlabelPixmap, pixmap);	n++;
    XtSetValues (labels[j], args, n);
    XtManageChild (labels[j]);

    n = 0;
    XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);	n++;
    separator = XmCreateSeparatorGadget (drawarea, "separator", args, n);
    XtManageChild (separator);

    CommonPause();

    j++; 
  }

  j = 0;

  while (labels[j] != NULL)
    {
      XtDestroyWidget(labels[j]);
      j++;
    }

  XmDestroyPixmap(XtScreen(drawarea), pixmap);
 
  CommonPause();

  XtAppMainLoop(app_context);
}
