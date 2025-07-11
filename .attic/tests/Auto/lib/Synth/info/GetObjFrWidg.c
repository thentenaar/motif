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
static char rcsid[] = "$XConsortium: GetObjFrWidg.c /main/7 1995/07/14 11:32:00 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetObjFrWidg.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
            08/23/90 SJS do not check instance
            09/21/90 SJS do not check object_type
	Calls:

	Summary:
            Looks up the object record associated with the given widget
	The definition of what this function *SHOULD* do needs to be
	addressed. Not passing in the object_type or instance is
	questionable in my mind
	THIS FUNCTION DOES SOMETHING SIMILAR TO xisFindObjectFromWidget().
	This one does brute force for loop, other does depth-first search.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
            Pointer to statically allocated object record.
************************************************************************/
#include "xislib.h"

XisObjectRecord *xisGetObjectFromWidget(widget)
Widget widget;
{
    int i;
   
    for (i=0; i < xisMaxObjectNum; i++) {
        if ( (xisObjects[i].in_use) && (xisObjects[i].id.widget == widget))
            return (&xisObjects[i]);
    }

    return(NULL_Object);

} /* End xisGetObjectFromWidget() */
