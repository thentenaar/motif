/* $XConsortium: XmeConfigureObject.c /main/5 1995/07/15 21:04:12 drk $ */
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


/*#include <sys/file.h>*/

/*  Standard C headers  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* QATS headers */
#include <testlib.h>

/*  X headers  */
#include <X11/IntrinsicP.h>

/*  Xm headers  */
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>

int 
main (int argc, char **argv)
{
  Widget             workRegion, simpleWidget, simpleGadget;
  register int       n;
  Arg                args[MAX_ARGS];
  Position           x, y;
  Dimension          width, height, border_width;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  /* set up a manager */
  n = 0;
  XtSetArg(args[n],XmNnoResize, True);  n++;
  XtSetArg(args[n],XmNresizePolicy, XmRESIZE_NONE);  n++;
  XtSetArg(args[n],XmNwidth, 200);  n++;
  XtSetArg(args[n],XmNheight, 200);  n++;
  workRegion = XmCreateBulletinBoard(Shell1,"WorkRegion", args, n);

  XtManageChild(workRegion);

  /* Instantiate a PushButton widget. */
  n = 0;
  XtSetArg(args[n],XmNrecomputeSize, False);  n++;
  XtSetArg(args[n],XmNshadowThickness, 5);  n++;
  XtSetArg(args[n],XmNmarginWidth, 5);  n++;
  XtSetArg(args[n],XmNx, 5);  n++;
  XtSetArg(args[n],XmNy, 5);  n++;
  XtSetArg(args[n],XmNborderWidth, 5);  n++;
  XtSetArg(args[n],XmNwidth, 90);  n++;
  XtSetArg(args[n],XmNheight, 50);  n++;
  simpleWidget = XmCreatePushButton(workRegion,"Widget", args, n);
  XtManageChild(simpleWidget);
  CommonPause();

  /* move the child widget */
  XmeConfigureObject(simpleWidget, 30, 30, 90, 50, 5);
  XtVaGetValues(simpleWidget,
		XmNx, &x,
		XmNy, &y,
		NULL);
  if (x != 30 || y !=30)
    printf("ERROR: incorrect position values\n");
  CommonPause();

  /* change the child widget's border */
  XmeConfigureObject(simpleWidget, 30, 30, 90, 50, 25);
  XtVaGetValues(simpleWidget,
		XmNborderWidth, &border_width,
		NULL);
  if (border_width != 25)
    printf("ERROR: incorrect border value\n");
  CommonPause();

  /* change the size of the widget */
  XmeConfigureObject(simpleWidget, 30, 30, 150, 100, 5);
  XtVaGetValues(simpleWidget,
		XmNheight, &height,
		XmNwidth, &width,
		NULL);
  if (width != 150 || height != 100)
    printf("ERROR: incorrect size values\n");
  CommonPause();

  /* Instantiate a PushButton gadget. */
  XtDestroyWidget(simpleWidget);
  n = 0;
  XtSetArg(args[n],XmNrecomputeSize, False);  n++;
  XtSetArg(args[n],XmNshadowThickness, 5);  n++;
  XtSetArg(args[n],XmNmarginWidth, 5);  n++;
  XtSetArg(args[n],XmNx, 5);  n++;
  XtSetArg(args[n],XmNy, 5);  n++;
  XtSetArg(args[n],XmNborderWidth, 5);  n++;
  XtSetArg(args[n],XmNwidth, 90);  n++;
  XtSetArg(args[n],XmNheight, 50);  n++;
  simpleGadget = XmCreatePushButtonGadget(workRegion,"Gadget", args, n);
  XtManageChild(simpleGadget);
  CommonPause();

  /* move the child gadget */
  XmeConfigureObject(simpleGadget, 80, 30, 90, 50, 5);
  XtVaGetValues(simpleGadget,
		XmNx, &x,
		XmNy, &y,
		NULL);
  if (x != 80 || y !=30)
    printf("ERROR: incorrect position values\n");
  CommonPause();

  /* change the child gadget's border */
  XmeConfigureObject(simpleGadget, 30, 30, 90, 50, 25);
  XtVaGetValues(simpleGadget,
		XmNborderWidth, &border_width,
		NULL);
  if (border_width != 25)
    printf("ERROR: incorrect border value: %d\n",border_width);
  CommonPause();

  /* change the size of the gadget */
  XmeConfigureObject(simpleGadget, 30, 30, 150, 100, 5);
  XtVaGetValues(simpleGadget,
		XmNheight, &height,
		XmNwidth, &width,
		NULL);
  if (width != 150 || height != 100)
    printf("ERROR: incorrect size values\n");
  CommonPause();

  CommonPause(); /* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
