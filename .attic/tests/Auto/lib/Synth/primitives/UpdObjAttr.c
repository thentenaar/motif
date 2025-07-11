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
static char rcsid[] = "$XConsortium: UpdObjAttr.c /main/7 1995/07/14 11:43:30 drk $"
#endif
#endif
/***********************************************************************
  	@(#)UpdObjAttr.c	1.9.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/06/90 TAT/SJS add in gadget check for getting window attributes
            08/02/90 SJS clear out propagate & event masks of gadgets/regions
            09/12/90 SJS set the visibility_state of regions to be equal to
	                 their parent's visibility_state
	Calls:

	Summary:
            Updates the object hierarchy with recent object window 
        attributes.

        INPUTS

        OUTPUTS

        RETURNS
            none
************************************************************************/

#include "xislib.h"

void xisUpdateObjectAttributes()
{
    register XisObjectRecord *object;
    XisObjectRecord *object_stack[100];
    short stack_top = 0;
    XWindowAttributes wattr;

    object = &xisObjects[0];

    /* Perform Depth-First Search */

    while (1) {

    /* If the id.window is NULL then this is a Region and does not have
       window attributes. We also check to see if we have a gadget to
       determine the event mask. (NULL parent means it's not a gadget or a
       window different from the parent means it's not a gadget) */

        if (object->id.window) {

            XGetWindowAttributes(xisDisplay,object->id.window,&wattr);

            if ((object->parent == NULL) ||
                (object->parent->id.window != object->id.window)) {

                object->your_event_mask = wattr.your_event_mask;
                object->do_not_propagate_mask = wattr.do_not_propagate_mask;
                object->visibility_state = wattr.map_state;
            }
            else {
                object->your_event_mask = 0;
                object->do_not_propagate_mask = 0;
		if (object->id.widget != NULL &&
		    			!XtIsManaged(object->id.widget))
			object->visibility_state = IsUnmapped;
		else
			object->visibility_state = wattr.map_state;
            }

        } /* End if (object->id.window != NULL) */
        else {
            object->visibility_state = object->parent->visibility_state;
        }
        if (object->first_child != NULL) {
            object_stack[stack_top++] = object;
            object = object->first_child;
        }
        else if (object->next_sibling != NULL) {
            object = object->next_sibling;
	}
        else {
            object = NULL;
            while (stack_top > 0 && object == NULL) {
                object = object_stack[--stack_top]->next_sibling;
            }
            if (stack_top == 0)
                break;
        }       
    } /* End while(1) */

} /* End xisUpdateObjectAttributes() */
