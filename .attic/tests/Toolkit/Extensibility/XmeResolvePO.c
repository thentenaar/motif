/* $XConsortium: XmeResolvePO.c /main/5 1995/07/15 21:05:13 drk $ */
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

/*  Xm headers  */
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/JoinSideT.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/CascadeBG.h>

/*  Exm headers  */
#include <Exm/CommandB.h>
#include <Exm/Simple.h>
#include <Exm/String.h>
#include <Exm/TabB.h>

void
DoQuit(Widget w, XtPointer ignore, XtPointer cbstruct)
{
  exit(0);
}


int 
main (int argc, char **argv)
{
  register int       n;
  Arg                args[MAX_ARGS];
  Display	     *display;
  Widget       	     tab_button, mainWindow;
  XtEnum	     open_side;
  

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  mainWindow = XtVaCreateManagedWidget("myMainWindow",
				       xmMainWindowWidgetClass, Shell1,
				       NULL);

  /* Instantiate an ExmTabButton widget. */
  tab_button = XtVaCreateManagedWidget("tabButton", exmTabButtonWidgetClass,
				   mainWindow,
				   XmNheight, 100,
				   XmNwidth, 140,
				   XmNshadowThickness, 15,
				   NULL);
  n = 0;
  XtSetArg(args[n], ExmNopenSide, &open_side);  n++;
  XtGetValues(tab_button, args, n);

  if (open_side != XmLEFT)
      printf("ERROR: open_side default is incorrect\n");
  CommonPause();

  /* Set the the open side resource and test that it works */
  n = 0;
  XtSetArg(args[n], ExmNopenSide, XmTOP);  n++;
  XtSetValues(tab_button, args, n);

  n = 0;
  XtSetArg(args[n], ExmNopenSide, &open_side);  n++;
  XtGetValues(tab_button, args, n);

  if (open_side != XmTOP)
      printf("ERROR: open_side resource has not been set properly\n");
  CommonPause();

  /* Reset the the open side resource and retest it */
  n = 0;
  XtSetArg(args[n], ExmNopenSide, XmRIGHT);  n++;
  XtSetValues(tab_button, args, n);

  n = 0;
  XtSetArg(args[n], ExmNopenSide, &open_side);  n++;
  XtGetValues(tab_button, args, n);

  if (open_side != XmRIGHT)
      printf("ERROR: open_side resource has not been set properly\n");
  CommonPause();

  CommonPause(); /* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
