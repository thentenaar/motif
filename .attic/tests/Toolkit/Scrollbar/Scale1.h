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
/*   $XConsortium: Scale1.h /main/6 1995/07/13 19:10:57 drk $ */

Boolean   wait;
Widget    ContinuePopup = NULL;

/*  Private Functions  */
static void            Pause();
static void            FlushEvents();
static Widget          CreateContinuePopup();
static XtCallbackProc  ContinueCB();
static XtCallbackProc  QuitCB();

static char *Pause_events[] = 
   {
"\
1) A client with a scale should appear. Press and hold MB1 on\n\
   scale slider. Drag slider up and down the scale. The\n\
   number to the left of the scale should change from 0 at\n\
   the bottom to 100 at the top. Release MB1.\n\
2) Continue to have client window change size.",
"\
1) Resize the client window.\n\
2) Press and hold MB1 on scale slider. Drag slider\n\
   up and down the scale. Number to the left of\n\
   the scale should change from -1.00 at bottom to\n\
   1.00 at the top. Release MB1.\n\
3) Resize the client window.\n\
4) Iconify and Normalize the client window.\n\
5) Shrink the client window to the smallest possible size.\n\
6) Re-expand the client window.",
/* PIR2808 - use Italics font to show bug */
"\
1) Press and hold MB1 on scale slider. Slowly drag slider left\n\
   and right. The number on the top of the scale should change\n\
   from 100 at left to 0 at the right. As the slider is\n\
   moved, the number should be drawn and redrawn cleanly\n\
   (leaving no garbage pixels).\n\
2) Test Finished -- Exit Please"
   };

int Pause_cnt = 0;
XtAppContext  app_context;
