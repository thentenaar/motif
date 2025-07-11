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
static char rcsid[] = "$XConsortium: ArrowBGad1.c /main/7 1995/07/13 17:27:08 drk $"
#endif
#endif


#include <testlib.h>
#include "Xm/XmP.h"

Widget PanedWindow1, ArrowButton1, ArrowButtonGadget1; 
Widget ArrowButtonGadget2, ArrowButtonGadget3, ArrowButtonGadget4;

void main(argc, argv)
int argc;
char **argv;
{
  register int n;
  Arg args[MAX_ARGS];
  XmString tcs;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
  XtSetValues(Shell1, args, n);

  n = 0;
  PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1", args, n);
  XtManageChild(PanedWindow1);

  n = 0;
  ArrowButton1 = XmCreateArrowButton(PanedWindow1, "ArrowButton1", args, n);
  XtManageChild(ArrowButton1);

  /*
   * We must realize the top level shell here for arrow to size correctly.
   */

  XtRealizeWidget(Shell1);
  CommonPause();

  n = 0;
  ArrowButtonGadget1 = 
    XmCreateArrowButtonGadget(PanedWindow1,
			      "ArrowButtonGadget1", args, n);
  XtManageChild(ArrowButtonGadget1);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
  ArrowButtonGadget2 = 
    XmCreateArrowButtonGadget(PanedWindow1,
			      "ArrowButtonGadget2", args, n);
  XtManageChild(ArrowButtonGadget2);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_LEFT);  n++;
  ArrowButtonGadget3 = 
    XmCreateArrowButtonGadget(PanedWindow1,
			      "ArrowButtonGadget3", args, n);
  XtManageChild(ArrowButtonGadget3);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
  ArrowButtonGadget4 = 
    XmCreateArrowButtonGadget(PanedWindow1,
			      "ArrowButtonGadget4", args, n);
  XtManageChild(ArrowButtonGadget4);

  CommonPause();
  CommonPause();
  CommonPause();
  XtAppMainLoop(app_context);

  /*
   * Clean up to test more easily for memory leaks
   */

  XtDestroyWidget(ArrowButtonGadget1);
  XtDestroyWidget(ArrowButtonGadget2);
  XtDestroyWidget(ArrowButtonGadget3);
  XtDestroyWidget(ArrowButtonGadget4);
  XtDestroyWidget(ArrowButton1);
  XtDestroyWidget(PanedWindow1);
}
