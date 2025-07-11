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
static char rcsid[] = "$XConsortium: GetXisState.c /main/7 1995/07/14 11:50:40 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetXisState.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	Calls:

	Summary:
            Used to generate a compressed representation of the current
        input synthesis 
        state.

          INPUTS
            none

          OUTPUTS
            hash - array of bytes used to fill with xis state information
            num_bytes - number of bytes returned

          RETURNS
            nothing
************************************************************************/
  
#include "mvslib.h"

void mvsGetXisStateHash(hash,num_bytes)
char hash[];
short *num_bytes;
{
    int bit = 0;
    int byte = 0;
    XisStateRecord *xis_state = xisGetStateInfo();
    int root_x,root_y;
    XisObjectRecord *pointer_obj, *focus_obj, *selected_obj;
    short pointer_id_code,focus_id_code,selected_id_code;
    
    xisGetPointerLocation(&root_x,&root_y);
    pointer_obj = xisFindObjectAtLocation(root_x,root_y);
    if (pointer_obj == NULL_Object)
        pointer_id_code = 0;
    else
        pointer_id_code = pointer_obj->id_code;

    if (xis_state->focus_window == 0)
        focus_id_code = 0;
    else {
        focus_obj = xisFindObjectFromWindow(xis_state->focus_window);
        if (focus_obj == NULL_Object)
            focus_id_code = 0;
        else
            focus_id_code = focus_obj->id_code;
    }

    if (xis_state->selected_widget == 0)
        selected_id_code = 0;
    else {
        selected_obj = xisFindObject(xis_state->selected_widget,
                                     xis_state->selected_object_type,
                                     xis_state->selected_instance);
        if (selected_obj == NULL_Object)
            selected_id_code = 0;
        else
            selected_id_code = selected_obj->id_code;
    
    }

    mvsPushHashBits(hash,&byte,&bit,pointer_id_code,8);

    mvsPushHashBits(hash,&byte,&bit,focus_id_code,8);

    mvsPushHashBits(hash,&byte,&bit,selected_id_code,8);

    mvsPushHashBits(hash,&byte,&bit,xis_state->mod_button_state,16);

    if (bit == 0)
        *num_bytes = byte;
    else
        *num_bytes = byte+1;

} /* end mvsGetXisStateHash() */
