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
/*   $XConsortium: Scale5.h /main/6 1995/07/13 19:12:09 drk $ */

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
1) A client with a vertical scale will appear. The scale\n\
   will have two labels that say '0' and '100'. Both labels\n\
   will have thin black borders and the '100' will be at the bottom.\n\
2) Press MB1 on the slider and drag the slider. The callback\n\
   reason and value should be printed in the term window.\n\
3) Press MB1 in the area above the slider. The callback reason and\n\
   value should be printed in the term window.\n\
4) Resize the client window larger. The labels should spread out\n\
   and remain on the left side, the scale area should span the right side.\n\
5) Resize the client window smaller.\n\
6) Continue to have the scale change orientation to HORIZONTAL.\n\
",
"\
1) A client with a horizontal scale will appear. The scale\n\
   will have two labels that say '0' and '100'. Both labels\n\
   will have thin black borders and the '100' will be on the right.\n\
2) Press MB1 on the slider and drag the slider. The callback\n\
   reason and value should be printed in the term window\n\
3) Press MB1 in area to the left of the slider. The callback\n\
   reason and value should be printed in the term window.\n\
4) Resize the client window larger. The label should spread out\n\
   and remain on the top, the scale area should span the bottom.\n\
5) Resize the client window smaller.\n\
6) Test Finished -- Exit Please\
"
      };
	
int Pause_cnt = 0;
XtAppContext  app_context;
