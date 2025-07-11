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
static char rcsid[] = "$XConsortium: IsAction.c /main/7 1995/07/14 11:38:34 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsAction.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Returns True if action_obj matches specified object type

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = action_obj is of specified type
            False = otherwise
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsActionRegion(object_type_in)
int object_type_in;
{
    XisObjectRecord *object = xisInform.action_obj;
    int object_type, cum_object_type = object_type_in;
    int tmp;
    int succeed = 1;

    if (!xisInform.is_valid)
        AutoMessage(_AutoMessages[WARNMSG127]); 

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

}  /* End xisIsActionRegion() */
