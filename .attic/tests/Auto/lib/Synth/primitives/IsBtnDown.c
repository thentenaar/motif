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
static char rcsid[] = "$XConsortium: IsBtnDown.c /main/7 1995/07/14 11:39:20 drk $"
#endif
#endif
/******************************************************************************
	@(#)IsBtnDown.c	1.4.1.1		Date: 1/22/91

	History:
            06/07/90 PSN first draft

	Calls:

	Summary:
            Returns True if mouse button is in pressed state. It does not 
	have to be the most recent event.

        INPUTS:
	    modifier_keys - modifier key mask
            button_num - the mouse button in question (Button1, Button2, etc.)
        
        OUTPUTS:
            none

        RETURNS:
            True = specified mouse button is down or not
            False = otherwise
************************************************************************/

#include "xislib.h"

int xisIsMouseButtonDown ( modifier_keys , button_num )
unsigned int modifier_keys;
int button_num;
{
    return ((xisMouseButtonMask[button_num] | modifier_keys) ==
						xisState.mod_button_state);
} /* End xisIsMouseButtonDown() */

