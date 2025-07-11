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
/*   $XConsortium: Protocol.h /main/6 1995/07/13 19:20:52 drk $ */

#define MAX_ARGS	100

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
	"1) A client with a menubar containing three entries will appear.\n   The entries will be labeled 'short menu', 'long menu' and\n   'no menu'.\n2) Click MB1 on 'short menu'.\n3) Select the window menu.  It should contain an additional entry,\n   'entry_1'.\n4) Choose 'entry_1' from the window menu.  The message \"entry 1\n   selected\" will be displayed in the xterm window.\n5) Click MB1 on 'short menu' again.\n6) Choose 'entry_1' from the window menu.  Nothing will be displayed\n   in the xterm window.\n7) Press the Continue button.\n",
         "1) Click MB1 on 'long menu'.  Two more entries, 'entry_2' and\n   'entry_3', will be added to the window menu.\n2) Choose 'entry_2' from the window menu.  The message \"entry 2\n   selected\" will be displayed in the xterm window.\n3) Choose 'entry_3' from the window menu.  The message \"entry 3\n   selected\" will be displayed in the xterm window.\n4) Click MB1 on 'long menu' again.  The three added entries in\n   the window menu will be deactivated (no longer boldface.)\n5) Click MB1 on 'long menu' again.  The three added entries in\n   the window menu will be reactivated (boldface.)\n6) Press the Continue button.\n",
         "1) Click MB1 on 'no menu'.  The three added entries in the window\n   menu will disappear.\n2) Test Finished -- Exit Please",
      };

int Pause_cnt = 0;
XtAppContext  app_context;
