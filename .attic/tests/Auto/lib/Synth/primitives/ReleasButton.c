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
static char rcsid[] = "$XConsortium: ReleasButton.c /main/8 1995/07/14 11:42:29 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ReleasButton.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/07/90 SJS add event mask for release event. This determines
                         if the send_object should get the event or not.
	Calls:

	Summary:
            Simulates the user releasing the specified mouse button. 

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include <X11/IntrinsicP.h>
#include "xislib.h"
#include "mvslib.h"

void xisReleaseMouseButton(modifier_keys,button_num)
unsigned int modifier_keys;
int button_num;
{
    static char routine_name[] = "xisReleaseMouseButton";
    XisObjectRecord *object, *current_object, *send_object, *action_object;
    int root_x,root_y;        /* Pointer position relative to root and window */
    Time time;
    short pursue;
    int x, y;
    XisObjectRecord *menu_object;
    Widget menu_widget, orig_widget;
    char *menu_class = NULL;
    MvsWidgetInfoRecord *menu_info;
    Boolean menu_manager = False;
    Boolean menu_release = False;
    Boolean menu_invalid = False;
    char msg_string[125];


    (*xisTraceMsg)("Got send_event_request = ReleaseMouseButton\n");
    xisUseSessionInfo(routine_name);
    xisUpdateObjectAttributes();

    /* First make sure this button is not already released */

    if ( !(xisState.mod_button_state & xisMouseButtonMask[button_num])) {
        sprintf (msg_string, _AutoMessages[WARNMSG59], button_num);
	AutoMessage(msg_string);

        return;
    }

    /* Then look up which object the pointer is over or is already grabbed */

    xisGetPointerLocation(&root_x,&root_y);
    xisInform.pointer_obj = xisFindObjectAtLocation(root_x,root_y);

    /* If Grab Window still exists, then ... */

    if (xisPointerGrabbed && xisWindowExists(xisGrabPointerWindow)) {
        
        /* Call all relavent InformExpectedActions functions */
  
        object = xisFindObject(xisState.selected_widget,
                               xisState.selected_object_type,
                               xisState.selected_instance);

        if (object == NULL) 
            object = xisFindObjectFromWindow(xisGrabPointerWindow);

        if (object == NULL)
            AutoMessage(_AutoMessages[WARNMSG21]);

        xisInform.is_valid = 1;        
        xisInform.action_obj = object;
        xisInform.modifier_key_status = modifier_keys;
        xisInform.button_num = button_num;
        xisInform.edge_code = 0;
        xisInform.key_code = 0;
        xisInform.event_code = EventMouseButtonUp;
        xisInform.num_clicks = 0;

        current_object = object;

        while (current_object != NULL) {

            if (current_object->proc_InformExpectedActions != NULL) {
                xisInform.current_obj = current_object;

                (*current_object->proc_InformExpectedActions)(EventMouseButtonUp); 
	    }
            current_object = current_object->parent;
	}

        xisInform.is_valid = 0;

        /* Send Synthesized Button Release Event */

        send_object = action_object = object;

/* Automation Added */

	xisGetPointerLocation(&x,&y);
	menu_object = xisFindObjectAtLocation(x,y);

	menu_widget = (Widget) menu_object->id.widget;
	orig_widget = menu_widget;

	while(! menu_invalid && menu_widget != NULL) {
		if (XmIsManager(menu_widget)) {
			menu_manager = True;
			menu_invalid = True;
		}
		if (XtIsShell(menu_widget)) {
			menu_invalid = True;
			menu_manager = False;
		}
		else 
			if (! menu_invalid)
				menu_widget = XtParent(menu_widget);
	}

	if (menu_manager) {
		menu_info = mvsWidgetToWidgetInfo(menu_widget);
		menu_class = menu_info->widget_class_info->class_name;
	}

	if (menu_manager &&
	    ((strcmp(menu_class, "XmPulldownMenu") == 0) 	||
	    (strcmp(menu_class, "XmPopupMenu") == 0)		||
	    (strcmp(menu_class, "XmMenuBar") == 0)		||
	    (strcmp(menu_class, "XmCascadeButton") == 0)	||
	    (strcmp(menu_class, "XmCascadeButtonGadget") == 0)	||
	    (strcmp(menu_class, "XmOptionMenu") == 0))) {
		/* insure that the object is not a gadget, if you use
		   parent as send_object since it has a valid window */
		if (XmIsGadget(orig_widget))
			menu_object = menu_object->parent;
		send_object = menu_object;
		menu_release = True;
	}
	else {
		menu_release = False;
		send_object = action_object = object;
	}

/* End Automation Added */

        /* Use parent object if in a region. Regions do not have windows */

        while (send_object != (XisObjectRecord *) NULL &&
	       send_object->id.window == (Window) NULL)
            send_object = send_object->parent;

        while (send_object != NULL) {
            if (((send_object->your_event_mask&ButtonReleaseMask) ||
		 menu_release)					  &&
                (send_object->visibility_state != IsUnmapped)) {
                pursue = 1;
                break;
            }

            if ((send_object->do_not_propagate_mask&ButtonReleaseMask) &&
                (send_object->visibility_state != IsUnmapped)            ) {
                pursue = 0;
                break;
            }
            send_object = send_object->parent;
        }

        if( (send_object != NULL) && pursue) {
            (*xisTraceMsg)("    Action_object = %s\n",
                           xisGetObjectName(action_object));
            (*xisTraceMsg)("    Send_object = %s\n",
                           xisGetObjectName(send_object));

            time = xisGetServerTime(xisMultiClickTime);
	    /* Won't perform any time delay */

/* Automation Added */

	    if (menu_release)
            	xisSendEvent(XtWindow(send_object->id.widget),
			  xisGrabPointerSubwindow,
               	          ButtonRelease,
               	          time,
               	          root_x - send_object->x,
               	          root_y - send_object->y, 
               	          root_x, root_y, modifier_keys,
               	          xisMouseButtonDetail[button_num]);
	    else

/* Automation end Added */

            	xisSendEvent(xisGrabPointerWindow,
			  xisGrabPointerSubwindow,
               	          ButtonRelease,
               	          time,
               	          root_x - send_object->x,
               	          root_y - send_object->y, 
               	          root_x, root_y, modifier_keys,
               	          xisMouseButtonDetail[button_num]);
		
        }

        /* Alter local info about which mouse buttons are down */

        xisState.mod_button_state &= (~xisMouseButtonMask[button_num]) &
                                     (~modifier_keys);

        /* If all mouse buttons are released, then ungrab pointer */

        if (xisState.mod_button_state == 0 &&
	    xisPointerGrabMode == POINTER_GRAB_CONTROLLED) {
/*****NoteGrab****/
            XUngrabPointer(xisDisplay,CurrentTime);
/*****/
            xisGrabPointerWindow = 0;
            xisPointerGrabbed = 0;
	}

        /* And, finally, process all related events */

        xisProcessEvents(NULL,0);
    }

    /* Make sure no matter if the pointer is grabbed or not that this
       buttons bit is released.                                       */

    xisState.mod_button_state &= (~xisMouseButtonMask[button_num]) &
                                 (~modifier_keys);
    xisPointerGrabbed = 0;
    xisGrabPointerWindow = 0;
    xisState.selected_widget = 0;
    xisState.selected_object_type = 0;
    xisState.selected_instance = 0;

} /* End xisReleaseMouseButton() */
