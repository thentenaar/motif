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
static char rcsid[] = "$XConsortium: MClickButton.c /main/7 1995/07/14 11:41:26 drk $"
#endif
#endif
/***********************************************************************
  	@(#)MClickButton.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
        Simulate a user clicking the mouse button multiple times

        INPUTS:
            modifier_keys - ShiftMask, LockMask, ControlMask, Mod1Mask, etc.
            button_num    - Mouse button number 1 through 5
            num_clicks    - Number of times to click the mouse button
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include "xislib.h"

void xisMultiClickMouseButton(modifier_keys,button_num,num_clicks)
unsigned int modifier_keys;
int button_num;
int num_clicks;
{
    xisCommonButtonPress(modifier_keys,button_num,num_clicks);

} /* End xisMultiClickMouseButton() */
