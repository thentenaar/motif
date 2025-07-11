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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: GetEventTime.c /main/7 1995/07/14 11:29:59 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetEventTime.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
	Calls:

	Summary:
            Extract the time field from the X event
           
        INPUTS:
            event         - the pointer to the X event structure
        
        OUTPUTS:
            none

        RETURNS:
            The X Time found in the event data structure if it exists. 
            If it doesn't exist, it returns zero.
************************************************************************/

#include "xislib.h"

Time xisGetEventTime(event)
XEvent *event;
{
    Time time;

    switch (event->type) {
      case MotionNotify:
          time = event->xmotion.time;
          break;
      case ButtonPress:
      case ButtonRelease:
          time = event->xbutton.time;
          break;
      case KeyPress:
      case KeyRelease:
          time = event->xkey.time;
          break;
      case EnterNotify:
      case LeaveNotify:
          time = event->xcrossing.time;
          break;
      case PropertyNotify:
          time = event->xproperty.time;
          break;
      default:
          time = 0;
          break;
    }

    return time;

} /* End xisGetEventTime() */
