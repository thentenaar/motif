/* $XConsortium: XmeReplyToQueryGeometry.c /main/4 1995/07/15 21:05:08 drk $ */
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
#include <Xm/BulletinB.h>
#include <Xm/XmP.h>

void  main (argc, argv)
int argc;
char **argv;
{
  register int       n;
  Arg                args[MAX_ARGS];
  Widget             bboard;
  XtWidgetGeometry   *desired, *intended;
  XtGeometryResult   geo_result;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  n = 0;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE);  n++;
  bboard = XmCreateBulletinBoard(Shell1, "BBoard", args, n);

  XtManageChild(bboard);

  /* set up desired XtWidgetGeometry struct */
  desired = (XtWidgetGeometry *) XtMalloc (sizeof (XtWidgetGeometry));
  desired->width = (Dimension) 50;
  desired->height = (Dimension) 50;
  desired->request_mode = (CWWidth | CWHeight);

  intended = (XtWidgetGeometry *) XtMalloc (sizeof (XtWidgetGeometry));
  intended->width = (Dimension) 50;
  intended->height = (Dimension) 50;
  intended->request_mode = (CWWidth | CWHeight);

  /* check if 1) the desired and intended values match and 
              2) the widget intends to use these values (request_mode
	         is set for width and height) */
  if (XmeReplyToQueryGeometry(bboard, intended, desired) == XtGeometryYes)
    printf("Case1: passed\n");
  else
    printf("ERROR: incorrect result returned\n");
  CommonPause();

  /* check if the desired and intended values are the same. */
  desired->width = (Dimension) 200;
  desired->height = (Dimension) 200;
  if (XmeReplyToQueryGeometry(bboard, intended, desired) == XtGeometryNo)
    printf("Case2: passed\n");
  else
    printf("ERROR: incorrect result returned\n");
  CommonPause();

  /* Check the remaining case when the previous cases don't apply. */
  desired->width = (Dimension) 100;
  if (XmeReplyToQueryGeometry(bboard, intended, desired) == XtGeometryAlmost)
    printf("Case3: passed\n");
  else
    printf("ERROR: incorrect result returned\n");
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
