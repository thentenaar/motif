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
static char rcsid[] = "$XConsortium: ClickButton.c /main/7 1995/07/14 11:37:40 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ClickButton.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Simulate a user pressing a mouse button and releasing it after
        some time interval.

        INPUTS:
            modifier_keys - ShiftMask, LockMask, ControlMask, Mod1Mask, etc.
            button_num    - Mouse button number 1 through 5
            hold_time_interval - Number of Xt timer callbacks to wait for before
                                 releasing the button.
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#include "xislib.h"


void xisClickMouseButton(modifier_keys,button_num)
unsigned int modifier_keys;
int button_num;
{
    xisCommonButtonPress(modifier_keys, button_num, 1);

} /* End xisClickMouseButton() */
