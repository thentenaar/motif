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
static char rcsid[] = "$XConsortium: FindParFrWin.c /main/7 1995/07/14 11:33:37 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FindParFrWin.c	1.2.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/30/90 TAT create
	Calls:

	Summary:
            Looks up the object highest in the hierarchy that has the 
        given window in its children_windows list.

        INPUTS
            window - X window id to match

        OUTPUTS
            none

        RETURNS
            Pointer to object which matches given window id
************************************************************************/
#include "xislib.h"

XisObjectRecord *xisFindParentObjectFromWindow(window)
Window window;
{
    XisObjectRecord *object;
    XisObjectRecord *object_stack[100];
    short stack_top = 0;
    int i;

    object = &xisObjects[0];

    /* Perform Depth-First Search */

    while (1) {

        for (i=0; i < object->id.num_children; i++) {
            if (object->id.children_windows[i] == window)
                break;
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
            while (stack_top > 0 && object == NULL)
                object = object_stack[--stack_top]->next_sibling;

            if (stack_top == 0)
                break;
        }       
    }

    return object;

} /* End xisFindParentFromWindow() */
