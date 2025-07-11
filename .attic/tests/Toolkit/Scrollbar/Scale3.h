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
/*   $XConsortium: Scale3.h /main/6 1995/07/13 19:11:35 drk $ */

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
	"1) A client window with three scales will appear.\n2) Continue for more testing.\n",
	"1) Resize the client window.\n2) Press and hold MB1 on each scale slider. Drag each\n   slider up and down the scale. Number to the left of each scale\n   should change from -1.00 at bottom to 1.00 at top. Release MB1\n   so each slider is at a different position.\n3) Resize bottom of the client window down.\n4) Test Finished -- Exit Please"
      };
	
int Pause_cnt = 0;
static XtCallbackProc  QuitCB();
XtAppContext  app_context;
