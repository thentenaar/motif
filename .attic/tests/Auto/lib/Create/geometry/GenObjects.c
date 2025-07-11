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
static char rcsid[] = "$XConsortium: GenObjects.c /main/7 1995/07/14 10:58:23 drk $"
#endif
#endif
/***********************************************************************
        @(#)GenObjects.c	1.5.1.1     Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS Add tp sccs
            06/15/90 SJS fix propagation of events problem (call to 
                         mvsGetSubObjects()
            06/22/90 SJS Really do a depth first search to generate
	Calls:

	Summary:
            Generate the xis object hierarchy.

            INPUTS
                root_obj - Pointer to the root object

            OUTPUTS
                root_obj - Pointer to the root object with new hierarchy 

            RETURNS
                none
************************************************************************/

#include "mvslib.h"

void mvsGenerateObjects(root_obj)
XisObjectRecord *root_obj;
{
    MvsWidgetInfoRecord *winfo = mvsRootWidgetInfo->first_child;
    int stack_top=0;
    MvsWidgetInfoRecord *object_stack[50];

    /* Re-initialize next_avail_id_code to 0 */

    mvsGetSubObjects(NULL,NULL,0,0,False);

    /* Perform Depth-First Search */

    while (winfo != NULL) {

        if(!winfo->being_destroyed)
            mvsGetSubObjects(xisFindObjectFromWidget(winfo->parent->widget),
                                                     winfo->widget,0,0,False);

        if (winfo->first_child != NULL) {
            object_stack[stack_top++] = winfo;
            winfo = winfo->first_child;
        }
        else if (winfo->next_sibling != NULL) {
            winfo = winfo->next_sibling;
	}
        else {

            winfo = NULL;
            while (stack_top > 0 && winfo == NULL) {
                winfo = object_stack[--stack_top]->next_sibling;
            }
        }       
    }
} /* End mvsGenerateObjects() */
