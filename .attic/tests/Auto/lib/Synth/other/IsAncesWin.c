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
static char rcsid[] = "$XConsortium: IsAncesWin.c /main/7 1995/07/14 11:36:06 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsAncesWin.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
           05/24/90 SJS add to sccs
	Calls:

	Summary:
            Determines if the first specified window is an ancestor of the 2nd
        but not the root. It returns true if this is in fact a true statement.

        INPUTS:
            window1 - window which may be ancestor of window2 
            window2 - any window
        
        OUTPUTS:
            none

        RETURNS:
            True if window1 is not the root but is an ancestor of window2
            False otherwise
************************************************************************/
#include "xislib.h"

int xisIsAncestorWindow(window1,window2)
Window window1,window2;
{
    static char routine_name[] = "xisIsAncestorWindow():";
    Window tmp_window;
    Window root_return, parent_return;
    Window *children_return;
    unsigned int n_children;

    xisUseSessionInfo(routine_name);

    if (window1 == xisRootWindow)
        return(False);

    if((!window1) || (!window2))
        return(False);

    if((!xisWindowExists(window1)) || (!xisWindowExists(window2)))
        return(False);

    tmp_window = window2;
    while (tmp_window != xisRootWindow && tmp_window != window1) {
        if (!XQueryTree(xisDisplay,tmp_window,&root_return,&parent_return,
                        &children_return,&n_children)) {
            (*xisErrorMsg)("%s XQueryTree() failure\n", routine_name);
            exit(-1);
	}
        if (children_return != NULL)
            XFree( (caddr_t)children_return);

        tmp_window = parent_return;
    }

    if (tmp_window == window1)
        return(True);
    else
        return(False);

} /* End xisIsAncestorWindow() */
