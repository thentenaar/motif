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
static char rcsid[] = "$XConsortium: FindObjAtLoc.c /main/7 1995/07/14 11:33:04 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FindObjAtLoc.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
            07/19/90 SJS correct Null sibling/first_child cases
            07/24/90 SJS use some register variables
            09/12/90 SJS check for the Map state of the objects
	Calls:

	Summary:
            Looks up the object lowest in the hierarchy and highest in
        stacking order that contains the given x,y location. This object
	must also be mapped (possibly should be viewable, but not sure yet)

        INPUTS
            x,y - the location relative to the root window

        OUTPUTS
            none

        RETURNS
            Pointer to object which contains the given x,y location
************************************************************************/
#include <Xm/Xm.h>
#include "xislib.h"
#include <X11/IntrinsicP.h>

XisObjectRecord *xisFindObjectAtLocation(x,y)
int x,y;
{
    register int stack_top = 0;
    register int done = False;
    register XisObjectRecord *object;
    register XisObjectRecord *save_object=NULL_Object;
    XisObjectRecord *object_stack[100];
    int border_width;

    /* Check to see if the current l & y is valid */

    if ( x < 0 || y < 0 || 
         x > DisplayWidth(xisDisplay,xisScreen) || 
         y > DisplayHeight(xisDisplay,xisScreen)   ) {

        return(NULL);
    }

    object = (&xisObjects[0]);  /* Start at the Root object */

    /* Perform Depth-First Search to find inner-most containing object */
    /* As we go along create a stack of the objects we are in   (!?!)  */

    while (!done) {

	border_width = 0;

	if (object->id.widget != NULL) {
		border_width = 
			(int) object->id.widget->core.border_width;
	}

        /* Check to see if the current x & y is within the object */
        if ( ((object->x - border_width) <= x) &&
             ((object->y - border_width) <= y) &&
             ((object->x + object->width + border_width) > x) &&
             ((object->y + object->height + border_width) > y) &&
             (object->visibility_state != IsUnmapped)   ) {
        
            save_object = object;
	}

        if ((save_object == object) && (object->first_child != NULL) ) {
            object_stack[stack_top++] = object;
            object = object->first_child;
        }
        else if (object->next_sibling != NULL) {
            if ((object == save_object)) {
                done = True;
            }
            else
                object = object->next_sibling;
        }
        else {
            if ((object == save_object) ) {
                done = True;
            }
            else {
                object = NULL;
                while (stack_top > 0) {
                    object = object_stack[--stack_top];
                    if (object == save_object)
                        done = True;
                    object = object->next_sibling;
                    if (object != NULL) 
                        break;
	        }
                if (object == NULL)
                    done = True;
            }
        }
    } /* End while(!done) */

    return(save_object);

} /* End xisFindObjectAtLocation() */
