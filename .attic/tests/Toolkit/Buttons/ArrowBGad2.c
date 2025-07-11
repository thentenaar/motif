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
static char rcsid[] = "$XConsortium: ArrowBGad2.c /main/9 1995/07/13 17:27:29 drk $"
#endif
#endif

/*
 *  Create a drawing area with a set of arrow gadgets within it.
 *  Turn the highlight_thickness and highlight_on_enter resources on.
 *  Change the highlight color of the manager to see if it gets reflected
 *  in the gadgets.
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
  Widget arrow[16];
  Arg args[10];
  Arg args2[10];
  int n, i;
  int x, y;
  char buf[32];
  Pixel top_shadow_color;


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

  for (y = 10; y <= 175; y += 50) {
    for (x = 10; x <= 175; x += 50)	{
	n = 0;
	XtSetArg (args[n], XmNx, x); n++;
	XtSetArg (args[n], XmNy, y); n++;
	XtSetArg (args[n], XmNhighlightThickness, 4); n++;
	XtSetArg (args[n], XmNhighlightOnEnter, True); n++;
	sprintf(buf, "arrowButton%d", i );
	arrow[i] = XmCreateArrowButtonGadget(drawarea, buf, args, n);
	XtManageChild(arrow[i]);
        i = i + 1;
      }
  }	

  n = 0;
  XtSetArg (args[n], XmNtopShadowColor, &top_shadow_color);	n++;
  XtGetValues (drawarea, args, n);
  
  n = 0;
  XtSetArg (args2[n], XmNhighlightColor, top_shadow_color); 	n++;
  XtSetValues (drawarea, args2, n);

  CommonPause();

  XtAppMainLoop(app_context);

}
