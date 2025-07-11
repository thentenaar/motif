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
static char rcsid[] = "$XConsortium: GetDragDist.c /main/7 1995/07/14 11:33:48 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetDragDist.c	1.2.1.1	Date:1/22/91
  	Author: PSN
	History:
            06/05/90 PSN add to sccs
	Calls:

	Summary:
		This function provides the drag distance (deltas) in x & y
	directions and the current widget id. 
************************************************************************/

#include "xislib.h"

int xisGetDragDistance(pwidget, pdel_x, pdel_y)
int 	*pdel_x, *pdel_y;
Widget	*pwidget;
{
    register XisObjectRecord	*current_obj;

    if (!xisInform.is_valid)
	{
            (*xisWarningMsg)("xisGetDragDistance() called with invalid Inform record\n");
	    return (-1);
	}

    current_obj = xisInform.current_obj;
    *pwidget = current_obj->id.widget;
    *pdel_x = xisInform.del_x;
    *pdel_y = xisInform.del_y;

    return (0);

} /* End xisGetDragDistance() */
