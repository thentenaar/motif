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
static char rcsid[] = "$TOG: Label2.c /main/8 1999/01/29 11:07:07 jff $"
#endif
#endif

/*
 *  Read a set of images using XmGetPixmap, then create a drawing area
 *  with a set of labels containing the pixmaps.
 */


#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Forward declarations  */

void main (argc, argv)
unsigned int argc;
char **argv;

{
  Widget drawarea;
  Widget label1, label2, label3, label4;
  Pixmap pixmap1, pixmap2, pixmap3, pixmap4;
  Arg args[10];
  Pixel foreground, background;
  int n;

  CommonTestInit(argc, argv);
    
  /*  The drawing area creation.  Contained in an outer frame.  */
  
  n = 0;
  XtSetArg (args[n], XmNwidth, 400);	n++;
  XtSetArg (args[n], XmNheight, 400);	n++;
  drawarea = XmCreateDrawingArea (Shell1, "drawarea", args, n);
  XtManageChild (drawarea);

  XtRealizeWidget (Shell1);

  n = 0;
  XtSetArg (args[n], XmNforeground, &foreground);	n++;
  XtSetArg (args[n], XmNbackground, &background);	n++;
  XtGetValues(drawarea, args, n);

  /*  Create the set of pixmaps  */

  pixmap1 = XmGetPixmap (XtScreen(drawarea), "tie_fighter",
					foreground, background);
  pixmap2 = XmGetPixmap (XtScreen(drawarea), "woman", foreground, background);
  pixmap3 = XmGetPixmap (XtScreen(drawarea), "right_ptr",
					foreground, background);
  pixmap4 = XmGetPixmap (XtScreen(drawarea), "target", foreground, background);

  /*  Create 4 labels in the drawing area  */

  n = 0;
  XtSetArg (args[n], XmNx, 25);		n++;
  XtSetArg (args[n], XmNy, 25);		n++;
  XtSetArg (args[n], XmNlabelType, XmPIXMAP);	n++;
  XtSetArg (args[n], XmNlabelPixmap, pixmap1);	n++;
  label1 = XmCreateLabel (drawarea, "label1", args, n);
  XtManageChild (label1);

  n = 0;
  XtSetArg (args[n], XmNx, 250); n++;
  XtSetArg (args[n], XmNy, 25);	n++;
  XtSetArg (args[n], XmNlabelType, XmPIXMAP);	n++;
  XtSetArg (args[n], XmNlabelPixmap, pixmap2);	n++;
  label2 = XmCreateLabel (drawarea, "label2", args, n);
  XtManageChild (label2);

  n = 0;
  XtSetArg (args[n], XmNx, 25);		n++;
  XtSetArg (args[n], XmNy, 250);		n++;
  XtSetArg (args[n], XmNlabelType, XmPIXMAP);	n++;
  XtSetArg (args[n], XmNlabelPixmap, pixmap3);	n++;
  label3 = XmCreateLabel (drawarea, "label3", args, n);
  XtManageChild (label3);

  n = 0;
  XtSetArg (args[n], XmNx, 250);		n++;
  XtSetArg (args[n], XmNy, 250);		n++;
  XtSetArg (args[n], XmNlabelType, XmPIXMAP);	n++;
  XtSetArg (args[n], XmNlabelPixmap, pixmap4);	n++;
  label4 = XmCreateLabel (drawarea, "label4", args, n);
  XtManageChild (label4);

  CommonPause();

  /* END */

  /* Clean up */

  XtDestroyWidget(label1);
  XtDestroyWidget(label2);
  XtDestroyWidget(label3);
  XtDestroyWidget(label4);
  XmDestroyPixmap(XtScreen(drawarea) ,pixmap1);
  XmDestroyPixmap(XtScreen(drawarea) ,pixmap2);
  XmDestroyPixmap(XtScreen(drawarea) ,pixmap3);
  XmDestroyPixmap(XtScreen(drawarea) ,pixmap4);
  XtDestroyWidget(drawarea);
  CommonPause();
  XtAppMainLoop(app_context);
}
