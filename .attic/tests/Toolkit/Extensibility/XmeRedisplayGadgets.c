/* $XConsortium: XmeRedisplayGadgets.c /main/4 1995/07/15 21:05:02 drk $ */
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
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  int                status;
  Arg                args[MAX_ARGS];
  String             dirname;
  Widget             rc, g1, g2, g3, g4;
  Pixel              black;
  XEvent             event;
  Display            *display;
  Window             window;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

printf("creating widget\n");
  n = 0;
/*  black=BlackPixel(display, DefaultScreen(display));
  XtSetArg(args[n], XmNbackground, black);  n++;*/
  rc = XmCreateRowColumn(Shell1, "rc", args, n);

printf("creating gadgets\n");
  n = 0;
  g1 = XmCreatePushButtonGadget(rc, "Gadget1", args, n);
  g2 = XmCreatePushButtonGadget(rc, "Gadget2", args, n);
  g3 = XmCreatePushButtonGadget(rc, "Gadget3", args, n);
  g4 = XmCreatePushButtonGadget(rc, "Gadget4", args, n);

printf("managing widgets and gadgets\n");
  XtManageChild(rc);
  display = XtDisplay(Shell1);
  window = XtWindow(rc);
  XtManageChild(g1);
  XtManageChild(g2);
  XtManageChild(g3);
  XtManageChild(g4);
  CommonPause();

printf("clear the window\n");
  status = XClearWindow(display, window);
  if ( status == BadMatch)
    printf("ERROR: XClearWindow failed - BadMatch\n");
  else if ( status == BadWindow)
    printf("ERROR: XClearWindow failed - BadWindow\n");
  CommonPause();

printf("XmeRedisplayGadgets\n");
  event.type = Expose;
  event.xexpose.display = display;
  event.xexpose.window = window;
  event.xexpose.send_event = False;
  event.xexpose.x = 0;
  event.xexpose.y = 0;
  event.xexpose.width = 100;
  event.xexpose.height = 100;
  XmeRedisplayGadgets(rc, &event, NULL);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
