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
/*   $XConsortium: Scale10.h /main/6 1995/07/13 19:11:16 drk $ */

Boolean   wait;
Widget    ContinuePopup = NULL;

/*  Private Functions  */
static void            Pause();
static void            FlushEvents();
static void 	       Quit();
static Widget          CreateContinuePopup();
static XtCallbackProc  ContinueCB();
static XtCallbackProc  QuitCB();

static char *Pause_events[] = 
   {
"\
1) A client with two Scales and four PushButtons contained\n\
   in a Bulletin Board should appear.  The horizontal scale\n\
   is 375 pixels wide and 30 pixels high and has an initial\n\
   value of 44.  The vertical scale is 30 pixels wide and 375\n\
   pixels high and has an initial value of 66.\n\
2) Throughout all of the following operations, the value label\n\
   for each scale widget should remain directly above (horizontal\n\
   scale) or to the left (vertical scale) of the slider.\n\
3) Click MB1 in the PushButton labeled \"Horiz scaleHeight to 50\".\n\
   The horizontal scale should grow to a scaleHeight of 50.\n\
4) Click MB1 in the PushButton labeled \"Vert scaleHeight to 50\".\n\
   The vertical scale should shrink to a scaleHeight of 50.\n\
5) Click MB1 in the PushButton labeled \"Horiz scaleWidth to 50\".\n\
   The horizontal scale should shrink to a scaleWidth of 50.\n\
6) Click MB1 in the PushButton labeled \"Vert scaleWidth to 50\".\n\
   The vertical scale should grow to a scaleWidth of 50.\n\
7) Move the horizontal scale slider several times and look at the\n\
   values printed in the stdout window.  The old and new values\n\
   should reflect the direction and amount of movement.\n\
8) Click MB1 on the slider of the horizontal scale without\n\
   moving it.  The old and new values printed should be the same.\n\
9) Test Finished -- Exit Please"
   };

int Pause_cnt = 0;
XtAppContext  app_context;
