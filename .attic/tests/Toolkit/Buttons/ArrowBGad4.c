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
static char rcsid[] = "$XConsortium: ArrowBGad4.c /main/8 1995/07/13 17:27:56 drk $"
#endif
#endif

/*
 *  Create a drawing area with a set of arrow gadgets within it.
 *  Move and resize the last gadget repetitively.
 */

#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>

static void flushEvents();

void main (argc, argv)
unsigned int argc;
char **argv;
{
  Widget drawarea;
  Widget arrow_set[50];
  int arrow_count = 0;
  Arg args[10];
  Arg args2[10];
  int n;
  int i;
  int x, y;
  int width, height;
  char buf[32];

  CommonTestInit(argc, argv);

  /* Set the focus-policy to pointer mode for the shell */
  n = 0;
  XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);   n++;  
  XtSetValues (Shell1, args, n);

  /*  The drawing area creation.  Contained in an outer frame.  */
  i = 0;
  n = 0;
  XtSetArg (args[n], XmNwidth, 400);	n++;
  XtSetArg (args[n], XmNheight, 400);	n++;
  drawarea = XmCreateDrawingArea (Shell1, "drawarea", args, n);
  XtManageChild (drawarea);

  XtRealizeWidget (Shell1);

  /*  Create a matrix of arrows  */

  for (y = 10; y <= 175; y += 50)
    {
      for (x = 10; x <= 175; x += 50)
	{
	  i = i + 1;
	  n = 0;
	  XtSetArg (args[n], XmNx, x);			n++;
	  XtSetArg (args[n], XmNy, y);			n++;
	  XtSetArg (args[n], XmNhighlightThickness, 4);	n++;
	  XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
	  sprintf(buf, "arrowButtonGadget%d", i);
	  arrow_set[arrow_count] =
            XmCreateArrowButtonGadget(drawarea, buf, args, n);
	  XtManageChild(arrow_set[arrow_count]);
	  arrow_count++;
	}
    }
  CommonPause();

  flushEvents (XtDisplay(Shell1), 0);
  width = height = 20;

  for (i = 0; i < 5; i++)
    {
      n = 0;
      x += 10;
      y += 10;
      XtSetArg (args[n], XmNx, x);	n++;
      XtSetArg (args[n], XmNy, y);	n++;
      XtSetValues (arrow_set[arrow_count - 1], args, n);

      flushEvents (XtDisplay(Shell1), 1);

      n = 0;
      width += 5;
      height += 5;
      XtSetArg (args[n], XmNwidth, width);	n++;
      XtSetArg (args[n], XmNheight, height);	n++;
      XtSetValues (arrow_set[arrow_count - 1], args, n);

      flushEvents (XtDisplay(Shell1), 1);

      n = 0;
      x += 10;
      y += 10;
      width += 5;
      height += 5;
      XtSetArg (args[n], XmNx, x);	n++;
      XtSetArg (args[n], XmNy, y);	n++;
      XtSetArg (args[n], XmNwidth, width);	n++;
      XtSetArg (args[n], XmNheight, height);	n++;
      XtSetValues (arrow_set[arrow_count - 1], args, n);

      flushEvents (XtDisplay(Shell1), 1);
   }

  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(drawarea);
  i = 0;
  for (y = 10; y <= 175; y += 50)
    {
      for (x = 10; x <= 175; x += 50)
	{
	  XtDestroyWidget(arrow_set[i]);
	  i++;
	}
    }
}

static void flushEvents(display, delay)
Display * display;
int delay;
{
   XEvent event;
   while (XPending(display)) {
      XNextEvent(display, &event);
      XtDispatchEvent(&event);
   }
   sleep(delay);
}

