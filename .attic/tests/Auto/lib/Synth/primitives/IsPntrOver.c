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
static char rcsid[] = "$XConsortium: IsPntrOver.c /main/7 1995/07/14 11:39:44 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsPntrOver.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            10/16/90 SJS check for ancestor relationship
	Calls:

	Summary:
            Returns True if pointer_obj matches specified object type.The object
	type passed in is a concatenation of the object types from the parent on
	down to the one we are interested in. Thus, from the keyboard region
	we go upwards from object to object. As we travel up the ancestor
	chain, each object type is compared against the corresponding "piece"
	of the concatenated type.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = pointer_obj is of specified type
            False = otherwise
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsPointerOverRegion(object_type_in)
int object_type_in;
{
    XisObjectRecord *object = xisInform.pointer_obj;
    int object_type, cum_object_type = object_type_in;
    int tmp;
    int succeed = 1;

    if (!xisInform.is_valid)
         AutoMessage(_AutoMessages[WARNMSG53]);

    if (object == NULL)
        return(FALSE);

    while (object != NULL && cum_object_type > 0) {

        tmp = cum_object_type / xisNumObjectTypes;
        object_type = cum_object_type - (tmp*xisNumObjectTypes);
     
        if (object->id.object_type != object_type) { /* Does not match */
            succeed = 0;
            break;
	}

        cum_object_type = tmp;
        object = object->parent;
    }

    /* This should be a straight equality */
    return (xisIsAncestorObject(xisInform.current_obj, object) && succeed);

} /* End xisIsPointerOverRegion() */
