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
static char rcsid[] = "$XConsortium: GetEdgeCode.c /main/7 1995/07/14 11:33:58 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetEdgeCode.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Return either TopEdge, BottomEdge, LeftEdge, or RightEdge to
        satisfy the definition of xisInform.edge_code (see xislib.h).  This
        field refers to the edge of the rectangular object that caused a
        pointer Enter or Leave event to occur.

        INPUTS:
            object - pointer to object record
            old_x
            old_y
            new_x
            new_y

        OUTPUTS:
            none

        RETURNS:
            TopEdge, BottomEdge, LeftEdge, or RightEdge
************************************************************************/
#include "xislib.h"

int xisGetEdgeCode(object,old_x,old_y,new_x,new_y)
XisObjectRecord *object;
int old_x,old_y;
int new_x,new_y;
{
    int itmp;

    if ( (old_x < object->x && new_x >= object->x)||
         (old_x >= object->x && new_x < object->x)  )
        return LeftEdge;

    else if ( (old_x >= (itmp = object->x + object->width) && new_x < itmp) ||
              (old_x < itmp && new_x >= itmp) )
        return RightEdge;

    else if ( (old_y < object->y && new_y >= object->y) ||
              (old_y >= object->y && new_y < object->y)   )
        return TopEdge;

    else if ( (old_y >= (itmp = object->y + object->width) && new_y < itmp) ||
              (old_y < itmp && new_y >= itmp) )
        return BottomEdge;

    else
        return UndefinedEdge;

} /* End xisGetEdgeCode() */
