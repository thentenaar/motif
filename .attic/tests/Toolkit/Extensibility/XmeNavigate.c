/* $XConsortium: XmeNavigate.c /main/5 1995/07/15 21:04:54 drk $ */
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

Widget		butt1, butt2, butt3, butt4, butt5, butt6;
Widget		xmDisplay;
XmString	default_str1, default_str2, default_str3, 
  default_str4, default_str5, default_str6;

int 
main (int argc, char **argv)
{
  register int       n;
  Arg                args[MAX_ARGS];
  Widget       	     grid;
  char               name[80];
  XmString           tcs;
  unsigned char      alignment;

  sprintf(name, "%s.out", argv[0]);

#ifdef DEBUG
  if ((outfp = fopen(name, "w+")) == NULL) {
    fprintf(stderr, "Can not open output file %s.out. Trying in /tmp\n",
	    argv[0]);
    sprintf(name, "/tmp/%s.out", argv[0]);
    if ((outfp = fopen(name, "w+")) == NULL) {
      fprintf(stderr, "Can not open output file /tmp/%s.out. Exiting.\n",
	      argv[0]);
      exit(0);
    }
  }    
#endif

  XSetErrorHandler(ProtoError);

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  300);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  n = 0;
  grid = XtVaCreateManagedWidget("Grid",
				 exmGridWidgetClass, Shell1, 
				 XmNrows, 2,
				 XmNcolumns, 3,
				 NULL); 

  default_str1 = XmStringCreateLocalized("ONE");
  butt1 = XtVaCreateManagedWidget("butt1",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   XmNalignment, XmALIGNMENT_CENTER,
				   ExmNcompoundString, default_str1,
				   NULL);

  default_str2 = XmStringCreateLocalized("TWO");
  butt2 = XtVaCreateManagedWidget("butt2",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   ExmNcompoundString, default_str2,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);

  default_str3 = XmStringCreateLocalized("THREE");
  butt3 = XtVaCreateManagedWidget("butt3",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   ExmNcompoundString, default_str3,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);

  default_str4 = XmStringCreateLocalized("FOUR");
  butt4 = XtVaCreateManagedWidget("butt4",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   ExmNcompoundString, default_str4,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);

  default_str5 = XmStringCreateLocalized("FIVE");
  butt5 = XtVaCreateManagedWidget("butt5",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   ExmNcompoundString, default_str5,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);

  default_str6 = XmStringCreateLocalized("SIX");
  butt6 = XtVaCreateManagedWidget("butt6",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   ExmNcompoundString, default_str6,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);
  /* clean up */
  XmStringFree(default_str1);
  XmStringFree(default_str2);
  XmStringFree(default_str3);
  XmStringFree(default_str4);
  XmStringFree(default_str5);
  XmStringFree(default_str6);
  CommonPause();

  XtRealizeWidget(Shell1);
  CommonPause();

  XtUnmanageChild(butt2);
  XtUnmanageChild(butt3);
  XtUnmanageChild(butt4);
  CommonPause();

  XtManageChild(butt2);
  XtManageChild(butt3);
  XtManageChild(butt4);
  CommonPause();

  XtUnmanageChild(butt1);
  XtUnmanageChild(butt2);
  XtUnmanageChild(butt3);
  XtUnmanageChild(butt4);
  XtUnmanageChild(butt5);
  XtUnmanageChild(butt6);
  CommonPause();

  XtManageChild(butt1);
  XtManageChild(butt2);
  XtManageChild(butt3);
  XtManageChild(butt4);
  XtManageChild(butt5);
  XtManageChild(butt6);
  CommonPause();

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
