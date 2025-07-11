/* $XConsortium: XmeQueryGeo.c /main/5 1995/07/15 21:04:58 drk $ */
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

/*  Standard C headers  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* QATS headers */
#include <testlib.h>

/*  X headers  */
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/Xm.h>

/*  Exm headers  */
#include <Exm/CommandB.h>
#include <Exm/Grid.h>

static int ProtoError();

Widget		butt1, butt2;
Widget		xmDisplay;
XmString	default_str1, default_str2;

int 
main (int argc, char **argv)
{
  register int       n;
  Arg                args[MAX_ARGS];
  Widget       	     grid;
  unsigned char      alignment;
  XtGeometryResult   geo_result;
  XtWidgetGeometry   intended, preferred;
  Dimension	     width, height;

  XSetErrorHandler(ProtoError);

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  50);  n++;
  XtSetArg(args[n], XmNheight, 50);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  grid = XtVaCreateManagedWidget("Grid",
				 exmGridWidgetClass, Shell1, 
				 XmNwidth,  50,
				 XmNheight, 50,
				 XmNrows, 1,
				 XmNcolumns, 2,
				 NULL); 

  default_str1 = XmStringCreateLocalized("ONE");
  butt1 = XtVaCreateManagedWidget("butt1",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 50,
				   XmNheight, 50,
				   XmNalignment, XmALIGNMENT_CENTER,
				   ExmNcompoundString, default_str1,
				   NULL);

  default_str2 = XmStringCreateLocalized("TWO");
  butt2 = XtVaCreateManagedWidget("butt2",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 50,
				   XmNheight, 50,
				   ExmNcompoundString, default_str2,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);

  /* pause here to let the user read the description*/
  CommonPause();

  intended.height = 200;
  intended.width = 200;
  intended.request_mode = CWWidth | CWHeight;
  geo_result = XtQueryGeometry(grid, &intended, &preferred);

  if (geo_result != XtGeometryNo)
    printf("ERROR: unexpected result from query_geometry method\n");
  CommonPause();

  intended.height = 70;
  intended.width = 120;
  intended.request_mode = CWWidth | CWHeight;
  geo_result = XtQueryGeometry(grid, &intended, &preferred);

  if (geo_result != XtGeometryYes)
    printf("ERROR: unexpected result from query_geometry method\n");
  CommonPause();

  /* force new size on the grid widget */
  n = 0;
  XtSetArg(args[n], XmNheight, 100);  n++;
  XtSetArg(args[n], XmNwidth, 200);  n++;
  XtSetValues(grid, args, n);

  intended.height = 170;
  intended.width = 230;
  intended.request_mode = CWWidth | CWHeight;
  geo_result = XtQueryGeometry(grid, &intended, &preferred);

  if (geo_result != XtGeometryAlmost)
    printf("ERROR: unexpected result from query_geometry method\n");
  CommonPause();

  /* clean up */
  XmStringFree(default_str1);
  XmStringFree(default_str2);

  CommonPause(); /* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}

static int ProtoError(dpy, ev)
Display		*dpy;
XErrorEvent	*ev;
{
	fprintf(stderr, "Proto Error:");
	switch (ev->error_code) {
	case BadAccess:
	  fprintf(stderr,"\tBadAccess\n"); break;
	case BadAlloc:
	  fprintf(stderr,"\tBadAlloc\n"); break;
	case BadAtom:
	  fprintf(stderr,"\tBadAtom\n"); break;
	case BadColor:
	  fprintf(stderr,"\tBadColor\n"); break;
	case BadCursor:
	  fprintf(stderr,"\tBadCursor\n"); break;
	case BadDrawable:
	  fprintf(stderr,"\tBadDrawable\n"); break;
	case BadFont:
	  fprintf(stderr,"\tBadFont\n"); break;
	case BadGC:
	  fprintf(stderr,"\tBadGC\n"); break;
	case BadIDChoice:
	  fprintf(stderr,"\tBadIDChoice\n"); break;
	case BadImplementation:
	  fprintf(stderr,"\tBadImplementation\n"); break;
	case BadLength:
	  fprintf(stderr,"\tBadLength\n"); break;
	case BadMatch:
	  fprintf(stderr,"\tBadMatch\n"); break;
	case BadName:
	  fprintf(stderr,"\tBadName\n"); break;
	case BadPixmap:
	  fprintf(stderr,"\tBadPixmap\n"); break;
	case BadRequest:
	  fprintf(stderr,"\tBadRequest\n"); break;
        case BadValue:
	  fprintf(stderr,"\tBadValue\n"); break;
	case BadWindow:
	  fprintf(stderr,"\tBadWindow\n"); break;
	default:
	  fprintf(stderr,"\tUnknown error_code\n"); break;
	}
#ifdef DEBUG
	fflush(outfp);
	fclose(outfp);
#endif
	exit(0);
}
