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
static char rcsid[] = "$XConsortium: CMovePointer.c /main/7 1995/07/14 11:37:30 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CMovePointer.c	1.13.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            07/16/90 SJS correct initialization
            08/04/90 SJS grabs, motion, Enter/Leave events, etc
            09/04/90 SJS modify Enter/Leaves for Menus (*always* do them)
	Calls:

	Summary:
            Common code between MovePointerTo and MovePointerRelative.  This
        function calls the various InfromExpected procedures with Enter/Leave
        events and then proceeds to move the mouse pointer to the target
        location.

        INPUTS:
            target_x
            target_y

        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include "xislib.h"

void xisCMovePointer(curr_object, curr_x, curr_y, target_x, target_y)
XisObjectRecord *curr_object;
int curr_x, curr_y;
int target_x, target_y;
{
    register XisObjectRecord *tmp_object=NULL;
    register XisObjectRecord *new_object=NULL;
    XisObjectRecord *object=NULL;
    int x,y,old_x,old_y;
    int distance_int;
    double a,b,tmp,distance;
    int i;

    /* Simulate the pointer motion by checking which objects it passes */
    /* over by moving it one pixel at a time and calling the appropriate    */
    /* proc_InformExpectedActions with Enter/Leave events.                  */

    a = target_y - curr_y;
    b = -(target_x - curr_x);
    distance = sqrt((double)(a*a + b*b));
    distance_int = distance + 0.6;

    if (xisPointerGrabbed && xisWindowExists(xisGrabPointerWindow)) {
        object = xisFindObject(xisState.selected_widget,
                               xisState.selected_object_type,
                               xisState.selected_instance);
    }
    else {
        object = curr_object;
    }

    tmp_object = object;
    x = curr_x;
    y = curr_y;

    xisInform.modifier_key_status = 0;
    xisInform.button_num = 0;
    xisInform.key_code = 0;
    xisInform.num_clicks = 0;

    /* Send Pointer Motion Event */

    xisInform.is_valid = TRUE;
    xisInform.event_code = EventPointerMotion;
    xisInform.action_obj = curr_object;
    xisInform.pointer_obj = curr_object;
    xisInform.current_obj = curr_object;
    xisInform.del_x = target_x - curr_x;
    xisInform.del_y = target_y - curr_y;
    while (tmp_object != NULL) {
        if (tmp_object->proc_InformExpectedActions != NULL) {
            (*tmp_object->proc_InformExpectedActions)
                                      (xisInform.event_code);
        }
        tmp_object = tmp_object->parent;
        xisInform.current_obj = tmp_object;
    }
    xisInform.is_valid = FALSE;

    /* Loop to send Enter/Leave events for all objects along path. Begin
       with the object we are currently over. */

    object = curr_object;
    for (i=1; i<=distance_int; i++) {
        tmp = i / distance;
        old_x = x;
        old_y = y;

        x = (int) (-b * tmp + curr_x + 0.5);
        y = (int) (a * tmp + curr_y + 0.5);

        new_object = xisFindObjectAtLocation(x,y);
        xisInform.is_valid = True;

        if (!xisPointerGrabbed || !xisWindowExists(xisGrabPointerWindow)) {
            if (new_object != object) {
                if (xisIsAncestorObject(object,new_object)) {
                    xisInform.event_code = EventPointerEnter;
                    xisInform.action_obj = new_object;
                    xisInform.pointer_obj = new_object;
                    xisInform.current_obj = new_object;
                    xisInform.edge_code = xisGetEdgeCode(new_object,old_x,
                                                         old_y,x,y);
                    tmp_object = new_object;
                    if (new_object->proc_InformExpectedActions == NULL) {
                        tmp_object = new_object->parent;
                        xisInform.current_obj = tmp_object;
                    }
                    if (tmp_object != NULL && 
                        tmp_object->proc_InformExpectedActions != NULL) {

                        (*tmp_object->proc_InformExpectedActions)
                                                      (xisInform.event_code);
                    }
                }
                else {
                    xisInform.event_code = EventPointerLeave;
                    xisInform.action_obj = object;
                    xisInform.pointer_obj = object;
                    xisInform.current_obj = object;
                    xisInform.enter_obj = new_object;
                    xisInform.leave_obj = object;
                    xisInform.edge_code = xisGetEdgeCode(object,old_x,old_y,
                                                         x,y);
                    tmp_object = object;
                    if (object->proc_InformExpectedActions == NULL) {
                        tmp_object = object->parent;
                        xisInform.current_obj = tmp_object;
                    }
                    if (tmp_object != NULL && 
                        tmp_object->proc_InformExpectedActions != NULL) {
                        (*tmp_object->proc_InformExpectedActions)
                                                    (xisInform.event_code);
                    }
                    if (!xisIsAncestorObject(new_object,object)) {
                        xisInform.event_code = EventPointerEnter;
                        xisInform.action_obj = new_object;
                        xisInform.pointer_obj = new_object;
                        xisInform.current_obj = new_object;
                        xisInform.enter_obj = new_object;
                        xisInform.leave_obj = object;
                        xisInform.edge_code = xisGetEdgeCode(new_object,old_x,
                                                             old_y,x,y);
                        tmp_object = new_object;
                        if (new_object->proc_InformExpectedActions == NULL) {
                            tmp_object = new_object->parent;
                            xisInform.current_obj = new_object;
                        }
                        if (tmp_object != NULL &&
                            tmp_object->proc_InformExpectedActions != NULL) {

                            (*tmp_object->proc_InformExpectedActions)
                                                      (xisInform.event_code);
                        }
                    }
                } /* End if (xisIsAncestorObject(object,new_object)) */
                object = new_object;
            } /* End if (new_object != object) */
        } /* End !xisPointerGrabbed */
        else {
            if(new_object != object) {
                xisInform.event_code = EventPointerLeave;
                xisInform.action_obj = object;
                xisInform.pointer_obj = object;
                xisInform.enter_obj = new_object;
                xisInform.leave_obj = object;
                xisInform.edge_code = xisGetEdgeCode(object,old_x, old_y,
                                                     x,y);
                if(!object->id.window) { /* This is a region */
                    tmp_object = object->parent;
                }
                else {
                    tmp_object = object;
                }
                while (tmp_object != NULL &&
                            tmp_object->proc_InformExpectedActions != NULL) {

                    xisInform.current_obj = tmp_object;
                    (*tmp_object->proc_InformExpectedActions)
                                                      (xisInform.event_code);
                    tmp_object = tmp_object->parent;
                }
                xisInform.event_code = EventPointerEnter;
                xisInform.action_obj = new_object;
                xisInform.pointer_obj = new_object;
                xisInform.enter_obj = new_object;
                xisInform.leave_obj = object;
                xisInform.edge_code = xisGetEdgeCode(new_object,old_x, old_y,
                                                     x,y);
                if(!new_object->id.window) { /* This is a region */
                    tmp_object = new_object->parent;
                }
                else {
                    tmp_object = new_object;
                }
                while (tmp_object != NULL &&
                            tmp_object->proc_InformExpectedActions != NULL) {

                    xisInform.current_obj = tmp_object;
                    (*tmp_object->proc_InformExpectedActions)
                                                      (xisInform.event_code);
                    tmp_object = tmp_object->parent;
                }
            } /* End if(new_object != object) */
            object = new_object;
          /* Save this damn thing for Enter/Leaves later */
        } /* End if(!xisPointerGrabbed || xisWindowExists) */
        xisInform.is_valid = False;
    } /* End for() */
        
    /* Now move pointer on the screen */
    xisMovePointer(target_x, target_y);

    /* And process all subsequent events */
    xisProcessEvents(NULL,0);

} /* End xisMovePointerTo() */
