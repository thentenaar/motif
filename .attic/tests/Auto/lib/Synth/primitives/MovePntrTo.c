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
static char rcsid[] = "$XConsortium: MovePntrTo.c /main/8 1995/07/14 11:41:58 drk $"
#endif
#endif
/***********************************************************************
  	@(#)MovePntrTo.c	1.11.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/07/90 SJS check for existance of object (can not check
                         for visibility because of regions. maybe later)
            06/11/90 SJS break out of for loop(s) when done becomes TRUE.
            07/19/90 SJS same as last change, but for more cases. Put in
                         comments.
	Calls:

	Summary:
            Simulates a user moving the mouse such that the pointer ends up
        "over" or on top of the specified widget's instance of an object. 

        INPUTS:
            widget      - Xt widget 
            object_type - Object type code 
            instance    - occurance number of char, item, or sash (start with 0)
            direction   - MoveAny, MoveUp, MoveDown, MoveLeft, MoveRight

        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"
#include "mvslib.h"

#define MAX_DIRECTIONS 4

void xisMovePointerTo(widget,object_code,instance,direction)
Widget widget;
int object_code;
int instance;
int direction;
{
    int curr_x,curr_y;
    XisObjectRecord *curr_object, *target_object;
    int target_x,target_y;
    int region_x,region_y,region_width,region_height;
    int x,y,itmp;
    int half_width,half_height;
    int done,i;
    int class_code = -1;
    static int recursive_calls=0; /* prevent infinite loop for any */
    static int last_direction=0;

    (*xisTraceMsg)("Got send_event_request = MovePointerTo\n");
    xisProcessObjects();
    xisUpdateObjectAttributes();

    /* Get current pointer location and current object over */

    xisGetPointerLocation(&curr_x,&curr_y);
    curr_object = xisFindObjectAtLocation(curr_x,curr_y);

    /* Get target pointer object and location */
 
    if (widget != NULL)
    	class_code = mvsGetClassCode(widget);

    target_object = xisFindObject(widget,object_code,instance);

    if (target_object == NULL &&
	(class_code == mvsXmTextWidgetClass ||
	 class_code == mvsXmListWidgetClass))
		target_object = xisFindObject
				    (XtParent(widget),object_code,instance);
    if(target_object == NULL) {
        AutoMessage(_AutoMessages[SCRMSG12]);
        return;
    }

    if (target_object == curr_object)
        return;

    if(recursive_calls > MAX_DIRECTIONS) {
        AutoMessage(_AutoMessages[SCRMSG11]);
        return;
    }
    
    /* When the object being moved to is not an ancestor of the current   */
    /* object, try for the center of the object we are interested in. If  */
    /* this location is not the object we want (it may be obscured by     */
    /* another object), move out towards the edges in all directions. As  */
    /* soon as the object is found at a particular location, use that     */
    /* location as our destination. (A part of the "cross-hair" must be   */
    /* visible or we won't find the object. This isn't perfect, but will  */
    /* have to do)                                                        */

    if (!xisIsAncestorObject(target_object,curr_object)) {

        target_x = target_object->x + target_object->width/2;
        target_y = target_object->y + target_object->height/2;

	/*
	 * Do not test for object at location if object is an
	 * OutlineButton because it is OUTSIDE of the parent object
	 */
        if ((object_code != oOutlineBtn) &&
	    (xisFindObjectAtLocation(target_x,target_y) != target_object)) {
            half_width = target_object->width / 2;
            half_height = target_object->height / 2;
            if (half_width > half_height)
                itmp = half_width;
            else
                itmp = half_height;

            done = False;

            for (i=0; i<itmp; i++) {
                x = target_x + i + 1;
                if (x <= target_x + half_width) {  /* Try right */
                    y = target_y;
                    if (xisFindObjectAtLocation(x-1,y) == target_object)
                        if (xisFindObjectAtLocation(x,y) == target_object) {
                            done = True;
                            break;
                        }
		}
                if (!done) {
                    x = target_x - i - 1;
                    if (x >= (target_x - half_width)) {  /* Try left */
                        y = target_y;
                        if (xisFindObjectAtLocation(x+1,y) == target_object)
                            if (xisFindObjectAtLocation(x,y) == target_object) {
                                done = True;
                                break;
                            }
		    }
                }
                if (!done) {
                    x = target_x;
                    y = target_y + i + 1;
                    if (y <= target_y + half_height) {    /* Try down */
                        if (xisFindObjectAtLocation(x,y-1) == target_object)
                            if (xisFindObjectAtLocation(x,y) == target_object) {
                                done = True;
                                break;
                            }
		    }
                }
                if (!done) {
                    x = target_x;
                    y = target_y - i - 1;
                    if (y >= (target_y - half_height)) {    /* Try up */
                        if (xisFindObjectAtLocation(x,y+1) == target_object)
                            if (xisFindObjectAtLocation(x,y) == target_object) {
                                done = True;
                                break;
                            }
		    }
                }
            }
            if (!done) {
                AutoMessage(_AutoMessages[SCRMSG39]);
                return;
	    }
            target_x = x;
            target_y = y;
        }
    }
    else {
    /* This is an ancestor, just move off of the current object until the */
    /* requested object is reached. If the requested object is not        */
    /* visible in a particular direction, try another. However, it is     */
    /* necessary to break out after all directions have been tried.       */

	if (direction == MoveAny) {
	    direction = ++last_direction;
	    last_direction  = direction;
	}

        done = False;
        switch (direction) {

	    case MoveUp:
                region_x = curr_object->x;
                region_y = target_object->y;
                region_width = curr_object->width;
                region_height = curr_object->y - region_y;
   
                half_width = region_width/2;
                for (y=region_y+region_height; !done && y>region_y; y--) {
                    for (i=0; i<half_width; i++) {
                        x = region_x + half_width - i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x,y-1)== target_object) {
                                done = True;
                                break;
			    }
                        x = region_x + half_width + i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x,y-1)== target_object) {
                                done = True;
                                break;
                            }
                    }
                }
                target_x = x;
                target_y = y-1;
                break;

            case MoveDown:
                region_x = curr_object->x;
                region_y = curr_object->y + curr_object->height;
                region_width = curr_object->width;
                region_height = target_object->y + target_object->height 
                                - region_y;
                half_width = region_width/2;
                for (y=region_y; !done && y<region_y+region_height; y++) {
                    for (i=0; i<half_width; i++) {
                        x = region_x + half_width - i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x,y+1)== target_object) {
                                done = True;
                                break;
			    }
                        x = region_x + half_width + i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x,y+1)== target_object) {
                                done = True;
                                break;
                            }
                    }
                }
                target_x = x;
                target_y = y+1;
                break;

            case MoveLeft:
                region_x = target_object->x;
                region_y = curr_object->y;
                region_width = curr_object->x - region_x;
                region_height = curr_object->height;

                half_height = region_height/2;
                for (x=region_x + region_width; !done && x>region_x; x--) {
                    for (i=0; i<half_height; i++) {
                        y = region_y + half_height - i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x-1,y)== target_object) {
                                done = True;
                                break;
			    }
                        y = region_y + half_height + i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x-1,y)== target_object) {
                                done = True;
                                break;
                            }
                    }
                }
                target_x = x-1;
                target_y = y;
                break;

	    default:
                AutoMessage(_AutoMessages[WARNMSG19]);
                AutoMessage(_AutoMessages[WARNMSG20]);

            case MoveRight:
                region_x = curr_object->x + curr_object->width;
                region_y = curr_object->y;
                region_width = target_object->x + target_object->width
                               - region_x;
                region_height = curr_object->height;

                half_height = region_height/2;
                for (x=region_x; !done && x<region_x + region_width; x++) {
                    for (i=0; i<half_height; i++) {
                        y = region_y + half_height - i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x+1,y)== target_object) {
                                done = True;
                                break;
			    }
                        y = region_y + half_height + i;
                        if (xisFindObjectAtLocation(x,y) == target_object)
                            if(xisFindObjectAtLocation(x+1,y)== target_object) {
                                done = True;
                                break;
                            }
                    }
                }
                target_x = x+1;
                target_y = y;
                break;
        } /* End switch(direction) */
        if (!done) { /* try again in another direction */
            recursive_calls++;
            xisMovePointerTo(widget,object_code,instance,MoveAny);
            recursive_calls--;
            return;
        }
    } /* End if (!xisIsAncestorObject()) */

    xisCMovePointer(curr_object, curr_x, curr_y, target_x, target_y);
    
} /* End xisMovePointerTo() */
