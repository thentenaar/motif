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
static char rcsid[] = "$XConsortium: IsMapped.c /main/7 1995/07/14 11:36:20 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsMapped.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
            06/27/90 PSN add new function xisIsWidgetMapped()
            08/17/90 PSN add new function xisIsViewable()
	Calls:

	Summary:
            Determines if the specified window is mapped or not.

        INPUTS:
            window - window which may be mapped or not
        
        OUTPUTS:
            none

        RETURNS:
            True if window is mapped
            False otherwise
************************************************************************/
#include "xislib.h"

int xisIsMapped(window)
Window window;
{
    static char routine_name[] = "xisIsMapped()";
    XWindowAttributes wattr;

    xisUseSessionInfo(routine_name);

    XGetWindowAttributes(xisDisplay,window,&wattr);

    return(wattr.map_state != IsUnmapped); 

} /* End xisIsMapped() */


int xisIsViewable(window)
Window window;
{
    static char routine_name[] = "xisIsViewable()";
    XWindowAttributes wattr;

    xisUseSessionInfo(routine_name);

    XGetWindowAttributes(xisDisplay,window,&wattr);

    return(wattr.map_state == IsViewable); 

} /* End xisIsViewable() */


/*
 * Return status of widget being mapped or unmapped
 */
int xisIsWidgetMapped(widget)
Widget	widget;
{
    XisObjectRecord	*object = xisGetObjectFromWidget(widget);

    return (xisIsMapped(object->id.window));

} /* End xisIsWidgetMapped() */
