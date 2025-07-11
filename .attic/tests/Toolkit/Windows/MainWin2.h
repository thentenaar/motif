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
/*   $XConsortium: MainWin2.h /main/7 1995/07/13 19:47:23 drk $ */

#define MAX_ARGS 	100

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
Main Window\n\
   \n\
   A Main Window will appear, containing a work area and a\n\
   button box. The button box should be above the work area.\n\
",
"\
   A message area should appear beneath the work area.\n\
",
"\
   The button box should have moved to beneath the work area.\n\
   \n\
   Exit now please\n\
"
};	
int Pause_cnt = 0;
XtAppContext  app_context;
