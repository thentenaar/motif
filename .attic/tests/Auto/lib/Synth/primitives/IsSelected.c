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
static char rcsid[] = "$XConsortium: IsSelected.c /main/7 1995/07/14 11:40:17 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsSelected.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Returns True if selected object (xisState.selected_obj) matches this
        object type

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = selected object matches specified object type
            False = no match
************************************************************************/


#include <AutoMessages.h>
#include "xislib.h"

int xisIsSelectedRegion(object_type_in)
int object_type_in;
{
    XisObjectRecord *object = xisFindObject(xisState.selected_widget,
                                            xisState.selected_object_type,
                                            xisState.selected_instance);

    int object_type, cum_object_type = object_type_in;
    int tmp;
    int succeed = 1;

    if (!xisInform.is_valid)
        AutoMessage(_AutoMessages[WARNMSG55]);

    if (object == NULL)
        return 0;

    while (object != NULL && cum_object_type > 0) {

        tmp = cum_object_type / xisNumObjectTypes;
        
        object_type = cum_object_type - (tmp*xisNumObjectTypes);
     
        if (object->id.object_type != object_type) {
            succeed = 0;
            break;
	}

        cum_object_type = tmp;

        object = object->parent;
    }

    return (object == xisInform.current_obj && succeed);

} /* End xisIsSelectedRegion() */
