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
/*   $XConsortium: ScrollWin3.h /main/7 1995/07/13 19:49:31 drk $ */


#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

Boolean   wait;
Widget    ContinuePopup = NULL;
Widget  Arrow1;
Widget  DrawingArea1;
Widget  DrawingArea2;
Widget  ScrolledWindow1; 
Widget  ScrolledWindow2;
Widget  vertSB, horizSB;
Widget	Shell1;
Widget	Text1;

static void            Pause();
static void            FlushEvents();
static Widget          CreateContinuePopup();
static XtCallbackProc  ResizeDrawAreaCB();
static XtCallbackProc  ChangeCallback();
static XtCallbackProc  ContinueCB();
static XtCallbackProc  QuitCB();

static char *Pause_events[] = 
      {
"\
1) A client containing a ScrolledWindow will appear. The ScrolledWindow\n\
   will contain a DrawingArea which will contain a Text widget. The\n\
   DrawingArea will be green in color, while the ScrolledWindow will be\n\
   AquaMarine.  The ScrolledWindow will have a scrollingPolicy of\n\
   XmAPPLICATION_DEFINED, and the scrollbars will be created independently.\n\
2) Press the continue button to continue this test.\n",
"\
1) Click MB1 in the trough area of the horizontal scrollbar. The cursor\n\
   will move with each click.  When the cursor reaches the end of\n\
   the Text area, the Text widget will scroll to the right.\n\
2) Click MB1 in the trough area of the vertical scrollbar. The cursor\n\
   will move to the next line in the Text widget.  When the cursor is on\n\
   the last visible line, it will be able to scroll down two more lines\n\
   of text.\n\
3) Press <Tab>.  The highlight will now be on the horizontal scrollbar.\n\
   Use the Arrow keys to scroll through the entire Text area horizontally.\n\
4) Press <Shift> <Tab>.  The highlight now will be in the vertical\n\
   scrollbar.  Use the Arrow keys again to scroll through the entire Text\n\
   area vertically.\n\
5) Press the continue button to continue this test.\n",
"\
1) Two Warning messages will be printed in the term window.  The messages\n\
   will state:\n\
   \"Cannot set a visual policy of CONSTANT in APPLICATION_DEFINED mode\",\n\
   \"Cannot set AS_NEEDED scrollbar policy with a\n\
      visual policy of VARIABLE\".\n\
2) The scrollbars will now be found on the top and on the right of the Text\n\
   widget.\n\
3) Use the scrollbars to scroll through the entire Text area.\n\
4) Press the continue button to continue this test.\n",
"\
1) The scrollbars will now be found on the left and on the bottom of the\n\
   Text widget.  Use the scrollbars to scroll through the entire Text area.\n\
2) Press the continue button to continue this test.\n",
"\
1) Automatic and static scrollbars appear on the bottom and on the left\n\
   of the Text widget.\n\
2) Resize the window smaller so that the scrollbars are not completely\n\
   filled.  Press <Tab> to put the focus in the text area, and press\n\
   the PageDown and PageUp keys.  The text should scroll.\n\
3) Test Finished -- Exit Please"
};
	
int Pause_cnt = 0;
XtAppContext  app_context;
