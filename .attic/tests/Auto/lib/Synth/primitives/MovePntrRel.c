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
static char rcsid[] = "$XConsortium: MovePntrRel.c /main/7 1995/07/14 11:41:49 drk $"
#endif
#endif
/***********************************************************************
  	@(#)MovePntrRel.c	1.5.1.1	Date:1/22/91
  	Author:
	History:
		07/18/90 PSN Fixed wrong call to non-existing procedure. Not
			     calls xisCMovePointer() correctly.
	Calls:

	Summary:
            Simulates a user moving the mouse some delta-x delta-y distance.

        INPUTS:
            delta_x - x coord of relative offset from current position
            delta_y - y coord of relative offset from current position
            direction - MoveAny, MoveUp, MoveDown, MoveLeft, MoveRight

        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisMovePointerRelative(delta_x,delta_y)
int delta_x, delta_y;
{
    int curr_x,curr_y;
    XisObjectRecord *curr_object;
    XisObjectRecord *end_obj;
    Boolean valid = True;

    (*xisTraceMsg)("Got send_event_request = MovePointerRelative\n");


    /* Get current pointer location and current object over */

    xisGetPointerLocation(&curr_x,&curr_y);
    
    curr_object = xisFindObjectAtLocation(curr_x,curr_y);

/* Automation Added */

    end_obj = xisFindObjectAtLocation(curr_x + delta_x, curr_y + delta_y);

    if (end_obj == NULL) {
	AutoMessage(_AutoMessages[WARNMSG17]);
	valid = False;
    }
    /*
    else 
   	if (curr_object != end_obj)
	     AutoMessage(_AutoMessages[WARNMSG18]);
    */
	
    if (valid)
    	xisCMovePointer(curr_object, curr_x, curr_y,
			curr_x + delta_x, curr_y + delta_y);

/* End Automation Added */

} /* End xisMovePointerRelative() */
