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
static char rcsid[] = "$XConsortium: GetPntrInfo.c /main/7 1995/07/14 11:35:07 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetPntrInfo.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns the current mouse pointer location relative to the root and
        the lowest containing window in the window heirarchy.
           
        INPUTS:
            none
        
        OUTPUTS:
            ptr_window        - window id of lowest containing window in
                                heirarchy
            root_x,root_y     - position of pointer relative to the root window
            window_x,window_y - position of pointer relative to ptr_window
            button_mask       - current state of mouse buttons using
                                Button1Mask,etc.

        RETURNS:
            void
************************************************************************/
#include "xislib.h"

void xisGetPointerInfo(ptr_window,root_x,root_y,window_x,window_y,button_mask)

Window *ptr_window;
int *root_x,*root_y,*window_x,*window_y;
unsigned int *button_mask;
{
    static char routine_name[] = "xisGetPointerInfo";
    Window root_return, tmp_window = xisRootWindow, save_window = xisRootWindow;

    xisUseSessionInfo(routine_name);

    while (tmp_window != None) {

        save_window = tmp_window;

        if (!XQueryPointer(xisDisplay,save_window,&root_return,
                           &tmp_window,root_x,root_y,
                           window_x,window_y,button_mask)) {
            (*xisErrorMsg)("Pointer not on root window anymore!!\n");
            exit(-1);
        }
    }

    *button_mask = xisState.mod_button_state;

    *ptr_window = save_window;

} /* End xisGetPointerInfo() */
