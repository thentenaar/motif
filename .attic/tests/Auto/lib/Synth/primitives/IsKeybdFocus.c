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
static char rcsid[] = "$XConsortium: IsKeybdFocus.c /main/7 1995/07/14 11:39:32 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsKeybdFocus.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            09/23/90 SJS the focus region only need be a descendant.
	Calls:

	Summary:
            Returns True if current focus object matches the specified object
        type (relative to the current object). The objecttype passed in is a
        concatenation of the object types from the parent on down to the one we
        are interested in. Thus, from the keyboard region we go upwards from
        object to object. As we travel up the ancestor chain, each object type
        is compared against the corresponding "piece" of the concatenated type.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = focus object matches specified object type
            False = no match
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsKeyboardFocusRegion(object_type_in)
int object_type_in;
{
    XisObjectRecord *object = xisFindObject(xisState.focus_widget,
                                            xisState.focus_object_type,
                                            xisState.focus_instance);
    int object_type, cum_object_type = object_type_in;
    int tmp;
    int succeed = 1;

    if (!xisInform.is_valid)
        AutoMessage(_AutoMessages[WARNMSG52]);

    if (object == NULL) {
        return(FALSE);
    }
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

    /* This should be a straight equality */
    return (xisIsAncestorObject(xisInform.current_obj, object) && succeed);

}  /* End xisIsKeyboardFocusRegion() */
