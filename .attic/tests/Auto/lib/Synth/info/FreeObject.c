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
static char rcsid[] = "$XConsortium: FreeObject.c /main/7 1995/07/14 11:31:36 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FreeObject.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            Frees the specified object and all its children.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
            Pointer to statically allocated object record.
************************************************************************/

#include "xislib.h"

void xisFreeObject(object)
XisObjectRecord *object;
{
    XisObjectRecord *child_obj, *parent_obj;
   
    if (object->parent != NULL) {
        parent_obj = object->parent;
        child_obj = parent_obj->first_child;
        if (child_obj == object)
            parent_obj->first_child = object->next_sibling;
        else {
            while (child_obj->next_sibling != NULL_Object && 
                   child_obj->next_sibling != object) {
                child_obj = object->next_sibling;
            }
            if (child_obj == NULL_Object)
                (*xisWarningMsg)("xisFreeObject():Did not find child in parent list\n");
            else {
                child_obj->next_sibling = object->next_sibling;
            }
        }
    }

    while (object->first_child != NULL_Object) {
        xisFreeObject(object->first_child);
    }

    object->in_use = 0;

    if (object == &xisObjects[xisMaxObjectNum])
        xisMaxObjectNum--;

} /* End xisFreeObject() */
