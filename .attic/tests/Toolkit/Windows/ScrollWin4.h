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
/*   $XConsortium: ScrollWin4.h /main/7 1995/07/13 19:50:16 drk $ */


#include <X11/Intrinsic.h>

Boolean   wait;
Widget    ContinuePopup = NULL;

static void            Pause();
static void            FlushEvents();
static Widget          CreateContinuePopup();
static XtCallbackProc  ResizeDrawAreaCB();
static XtCallbackProc  ContinueCB();
static XtCallbackProc  actCB();
static XtCallbackProc  QuitCB();

static char *Pause_events[] = 
      {
      "1) A Scrolled Window with horizontal and vertical scrollbars should\n\
   appear. The scrollbars should be on top and on the left. The background\n\
   of the ScrolledWindow should be white. The work area of the ScrolledWindow\n\
   should contain a DrawingArea with two ArrowButtons. The ArrowButtons\n\
   should have coral and violet backgrounds respectively. XmNspacing should\n\
   be set at 20 for the ScrolledWindow.\n\
2) Move the slider on the vertical scrollbar until the second violet colored\n\
   ArrowButton appears.\n\
3) Press Shift <Tab> until the hightlight is in the work area with the Arrow Buttons.\n\
4) Press the KSelect key (Space Bar). An Activate Callback message should be\n\
   printed in the term window.\n\
5) Press the Down Arrow Key. Then press KSelect again. Another message should\n\
   be printed in the term window.\n\
6) Move the horizontal scrollbars slider all the way to the right. The Drawing\n\
   Area and it's contents should scroll to the right.\n\
7) Continue for more testing.\n",
      "1) The size of the ArrowButtons should have been increased to 500x500 for\n\
   each PushButton.\n\
2) Move the slider on the vertical scrollbar until the second PushButton\n\
   is visible.\n\
3) Press MB1 in PushButton1 (Coral colored). An activate Callback message\n\
   should be printed in the term window.\n\
4) Press Shift <Tab> until the hightlight is in the work area with the Arrow\n\
   Buttons.\n\
5) Press <KBeginData>. The ScrolledWindow should scroll all the way to the top.\n\
6) Press <KEndData>. The ScrolledWindow should scroll to the end of the window.\n\
7) Continue for more testing.\n",
      "1) PushButton1 (Coral) should now be Unmanaged and should not be visible.\n\
2) Use the vertical scrollbars to scroll to the Violet PushButton.\n\
3) Continue for more testing.\n",
      "1) PushButton2 (Violet) and the DrawingArea should now be Unmanaged and\n\
   nothing should be visible.\n\
2) Continue for more testing.\n",
      "1) All widgets should be managed again, and everthing should be visible.\n\
2) Use the scrollbars to \"Look At\" all of the widgets.\n\
3) Press Shift <Tab> until the hightlight is in the work area with the Arrow\n\
   Buttons.\n\
4) Press KPageUp. The ScrolledWindow should scroll upwards one page.\n\
5) Press KPageDown. The ScrolledWindow should scroll downwards one page.\n\
6) Continue for more testing.\n",
      "1) The width and the height of the ArrowButtons should now change to 175.\n\
   This should cause the horizontal scrollbar to disappear.\n\
2) Use the vertical scrollbar to  \"Look At\" all of the widgets.\n\
3) Continue for more testing.\n",
      "1) The width and height of the ScrolledWindow should now be 125. This\n\
   should cause the window to shrink and the horizontal scrollbar to reappear.\n\
2) Use the horizontal and vertical scrollbars to \"Look At\" all of the widgets.\n\
3) Continue for more testing.\n",
      "1) The width and height of the ScrolledWindow should now be 250. This\n\
   should cause the window to expand to the point that no scrollbars should\n\
   be present.\n\
2) Press Kselect. An Activate Callback message will be printed in the term\n\
   window.\n\
3) Press the Down Arrow Key. Then press KSelect. Another Activate Callback\n\
   message will be printed in the term window.\n\
4) Continue for more testing\n",
      "1) The Drawing Area should have been UnMapped. No widgets\n\
   should be visible.\n\
2) Continue for more testing\n",
      "1) The Drawing Area should have been Mapped again. All widgets\n\
   should be visible.\n\
2) Test Finished -- Exit Please"
};
	
int Pause_cnt = 0;
XtAppContext  app_context;






