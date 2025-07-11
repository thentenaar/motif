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
/*   $XConsortium: ScrollWin2.h /main/7 1995/07/13 19:48:50 drk $ */

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
	"1) A client with a scrolled window should appear. Ten and\n   a half pushbutton gadgets and pushbuttons should be visible.\n   A vertical scrollbar should appear at the left side of the client.\n2) Continue for more testing.\n",
	"1) Move the client to the top of the screen. Resize\n   the bottom of the client window down to the bottom of the screen.\n   Most or all of the following should be visible:\n      PushButtonGadget1 - PushButtonGadget6 (black foregrounds)\n      PushButton1 - PushButton8 (green foregrounds)\n      five ArrowButtonGadgets (red foregrounds)\n      five ArrowButtons (black foregrounds)\n   If all are visible, scrollbars should disappear.\n2) Continue for more testing.\n",
	"1) Resize the right edge of the client window left so that\n   widgets and gadgets are only partially visible. The\n   horizontal scrollbar should appear at the top of the client.\n2) Press and hold MB1 on the horizontal slider. Drag the slider to\n   the far right. The pushbutton gadget numbers should be visible.\n3) Resize the bottom of the client up so that some widgets and\n   gadgets are clipped. The vertical scrollbar should reappear\n   at on the left of the client.\n4) Continue for more testing.\n",
	"1) Press and hold MB1 on the vertical slider. Drag the slider \n   to the bottom. Release MB1.\n2) Press and hold MB1 on the horizontal slider. Drag the slider\n   to the middle. Release MB1. Depending on how small you resized the window,\n   some number of the green and red arrow buttons should be visible.\n3) Iconify the client. Normalize the client.\n4) Test Finished -- Exit Please"
      };
	
int Pause_cnt = 0;
XtAppContext  app_context;
