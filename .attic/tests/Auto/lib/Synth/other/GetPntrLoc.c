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
static char rcsid[] = "$XConsortium: GetPntrLoc.c /main/7 1995/07/14 11:35:24 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetPntrLoc.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns the current mouse pointer location relative to the root.
           
        INPUTS:
            none
        
        OUTPUTS:
            root_x,root_y     - position of pointer relative to the root window

        RETURNS:
            void
************************************************************************/
#include "xislib.h"

void xisGetPointerLocation(root_x,root_y)
int *root_x,*root_y;
{
    static char routine_name[] = "xisGetPointerLocation";
    Window tmp_window,root_return;
    int window_x,window_y;
    unsigned int button_mask;

    xisUseSessionInfo(routine_name);

    if (!XQueryPointer(xisDisplay,xisRootWindow,&root_return,
                       &tmp_window,root_x,root_y,
                       &window_x,&window_y,&button_mask)) {
        (*xisErrorMsg)("Pointer not on root window anymore!!\n");
        exit(-1);
    }
} /* End xisGetPointerLocation() */

