/* $XConsortium: XmeGetTextualDragIcon.c /main/4 1995/07/15 21:04:49 drk $ */
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
static char rcsid[] = ""
#endif
#endif

#include <testlib.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/DragIconP.h>

void  main (argc, argv)
int argc;
char **argv;
{
  register int       n;
  Arg                args[MAX_ARGS];
  Widget             label, drag_icon;
  Pixmap             drag_pixmap;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  100);  n++;
  XtSetArg(args[n], XmNheight, 100);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  /* create label widget for the test */
  n =0;
  XtSetArg(args[n], XmNrecomputeSize, False);  n++;
  label = XmCreateLabel(Shell1, "label", args, n);

  drag_icon = XmeGetTextualDragIcon(label);
  if (drag_icon == NULL)
    printf("ERROR: XmeGetTextualDragIcon has failed\n");

  n = 0;
  XtSetArg(args[n], XmNpixmap, &drag_pixmap); n++;
  XtGetValues(drag_icon, args, n);

  XtManageChild(label);

  /* Place the "source icon" pixmap on the label */
  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
  XtSetArg(args[n], XmNlabelPixmap, drag_pixmap); n++;
  XtSetValues(label, args, n);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
