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
static char rcsid[] = "$XConsortium: Label3.c /main/7 1995/07/13 19:08:41 drk $"
#endif
#endif

/*
 *  Use XmGetPixmap to get a set of pixmaps created from images read from
 *  a file.  Put the images into labels for display.
 */


#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

void main (argc, argv)
unsigned int argc;
char **argv;

{
  Screen *screen;
  Widget drawarea;
  Widget label1, label2, label3, label4;
  Pixel foreground, background;
  Pixmap pixmap1, pixmap2, pixmap3, pixmap4;
  Arg args[10];
  int n;

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg (args[n], XmNwidth, 400);			n++;
  XtSetArg (args[n], XmNheight, 400);			n++;
  XtSetValues(Shell1, args, n);
    
   /*  The drawing area creation.  Contained in an outer frame.  */

  n = 0;
  XtSetArg (args[n], XmNwidth, 400);	n++;
  XtSetArg (args[n], XmNheight, 400);	n++;
  drawarea = XmCreateDrawingArea (Shell1, "drawarea", args, n);
  XtManageChild (drawarea);

  n = 0;
  XtSetArg (args[n], XmNforeground, &foreground);	n++;
  XtSetArg (args[n], XmNbackground, &background);	n++;
  XtGetValues (drawarea, args, n);

  XtRealizeWidget (Shell1);

  /*  Create a set of pixmaps to copy the images to.  */

  screen = XtScreen (Shell1);
  pixmap1 = XmGetPixmap (screen, "tie_fighter", foreground, background);
  pixmap2 = XmGetPixmap (screen, "woman", foreground, background);
  pixmap3 = XmGetPixmap (screen, "right_ptr", foreground, background);
  pixmap4 = XmGetPixmap (screen, "target", foreground, background);

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
  
  XtDestroyWidget(label1);
  XtDestroyWidget(label2);
  XtDestroyWidget(label3);
  XtDestroyWidget(label4);
  XtDestroyWidget(drawarea);
  CommonPause();

  XtAppMainLoop(app_context);
}








