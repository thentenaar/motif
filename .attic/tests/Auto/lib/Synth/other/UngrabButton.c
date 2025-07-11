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
static char rcsid[] = "$XConsortium: UngrabButton.c /main/7 1995/07/14 11:37:07 drk $"
#endif
#endif
/***********************************************************************
  	@(#)UngrabButton.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Simulates the user releasing the current mouse button.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#include "xislib.h"

void xisUngrabMouseButton()
{

    (*xisTraceMsg)("Performing ungrab of mouse button\n");

    if (xisPointerGrabbed) {
        xisPointerGrabbed = 0;   /* release causes grab to be released */
        xisState.mod_button_state = 0;
/********NoteGrab****/
        XUngrabPointer(xisDisplay,CurrentTime);
/******/
    }

} /* End  xisUngrabMouseButton() */
