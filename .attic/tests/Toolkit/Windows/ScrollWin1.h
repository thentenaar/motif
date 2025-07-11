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
/*   $XConsortium: ScrollWin1.h /main/7 1995/07/13 19:48:11 drk $ */

Boolean   wait;
Widget    ContinuePopup = NULL;

static void            Pause();
static void            FlushEvents();
static Widget          CreateContinuePopup();
static XtCallbackProc  ContinueCB();
static XtCallbackProc  QuitCB();

static char *Pause_events[] = 
      {
	"1) A empty client will appear. This window is a ScrolledWindow\n   without setting the work window.\n2) Continue to set a work window.\n",
	"1) A client window containing a scrolled window with pushbuttons\n   should appear. Ten of the fifteen pushbuttons should be visible.\n   A vertical scrollbar should appear at the right side of the client.\n2) Continue for more testing.\n",
	"1) Push some pushbuttons and scroll up and down.\n2) Resize the bottom of the client window down to expose all\n   fifteen pushbuttons. The vertical scrollbar should disappear.\n3) Resize the right edge of the client left so that the pushbuttons\n   are only partially visible. A horizontal scrollbar should appear at\n   the bottom of the client window.\n4) Press MB1 on the horizontal slider. Drag the slider to the far right.\n   The pushbutton numbers should be visible.\n5) Continue for more testing.\n",
	"1) Resize bottom of the client up so that some pushbuttons\n   are clipped. The vertical scrollbar should reappear at the right\n   side of the the client window.\n2) Press and hold MB1 on the horizontal slider. Drag the slider to the\n   far right. The pushbutton numbers should be visible.\n   Release MB1.\n3) Press and hold MB1 on the vertical slider. Drag the slider to the bottom.\n   The last pushbutton numbers through 15 should be visible.\n   Release MB1.\n4) Test Finished -- Exit Please"
      };
	
int Pause_cnt = 0;
XtAppContext  app_context;
