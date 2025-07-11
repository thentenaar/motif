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
static char rcsid[] = "$XConsortium: CButtonPress.c /main/7 1995/07/14 11:37:22 drk $"
#endif
#endif
/***********************************************************************
	Calls:

	Summary:
            Simulate a user pressing then releasing a mouse button one or
        more times.

        INPUTS:
            modifier_keys       - ShiftMask, LockMask, ControlMask, Mod1Mask,
                                  etc.
            button_num          - Mouse button number 1 through 5
            clicks              - Number of button multi clicks
            hold_time_intervals - Number of Xt timer intervals to press the
                                  button
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <X11/IntrinsicP.h>
#include "xislib.h"
#include <AutoMessages.h>

/* 
  When an X-server gets a button press from the real mouse device, this is 
  how it determines the appropriate event window to send it to:

    If ActiveGrabInProgress
        Send to client window that grabbed it (if visible)

    Else
        Search visible ancestors of window from root down for a passive button 
            grab

        If found, 
            Activate grab (call XGrabPointer) on that window (pg 144 Xlib)
            Send event to that same window
        Else
            Starting from lowest ancestor window containing pointer, go up tree
               (up meaning to ancestors) looking for window that has this event 
               selected.
            Send event to that window (with subwindow set to child in direction
               of window containing pointer).


  Since we have no way of determining which window currently has the pointer
  grabbed nor do we have a way of determining which windows have passive button
  grabs, we will skip these two cases and implement them in special ways when
  they come up. This appears feasible since button grabs are not used anywhere
  in Xm except in menus.

  Note: Actually, I do know of a way to determe if the pointer is currently
        grabbed: open a second display connection and try to grab the pointer
        with this new display.  If you get back an AlreadyGrabbed flag, then
        you know its grabbed.  The problem is, which window grabbed the pointer?
        You could rely on Enter/Leave Notify events with Grab/Ungrab modes
        but you would have to be sure these events were selected for every
        window at all times (not very feasible).
*/

#define GOOD_GRAB_BITS  \
		    ButtonPressMask | ButtonReleaseMask | EnterWindowMask | \
                    LeaveWindowMask | PointerMotionMask | \
		    PointerMotionHintMask | Button1MotionMask | \
		    Button2MotionMask | Button3MotionMask | \
                    Button4MotionMask | Button5MotionMask | \
		    ButtonMotionMask | KeymapStateMask

void xisCommonButtonPress(modifier_keys,button_num,clicks)
unsigned int modifier_keys;
int button_num;
int clicks;
{
    static char routine_name[] = "xisCommonButtonPress():";
    Window subwindow,send_window;
    short pursue;
    XisObjectRecord *action_object, *current_object, *send_object;
    int root_x,root_y;
    Time time;
    int i, multi = 0;
    char msg_string[125];
    
    xisUseSessionInfo(routine_name);
    xisUpdateObjectAttributes();
    
    if (clicks > 1)
	multi++;

    /* First make sure this button is not already pressed */

    if ( xisState.mod_button_state & xisMouseButtonMask[button_num] ) {
            sprintf(msg_string,_AutoMessages[WARNMSG46], routine_name,button_num); 
            AutoMessage(msg_string);
        xisReleaseMouseButton(NoModifierKeys,button_num);
    }

    /* Then look up which object the pointer is over or is already grabbed */

    xisGetPointerLocation(&root_x,&root_y);
    xisInform.pointer_obj = xisFindObjectAtLocation(root_x,root_y);

    if (xisState.mod_button_state != 0 && xisPointerGrabbed) {
        send_window = xisGrabPointerWindow;
        send_object = xisFindObject(xisState.selected_widget,
                                    xisState.selected_object_type,
                                    xisState.selected_instance);
        action_object = send_object;
        pursue = TRUE;
    }
    else {
        if (xisPointerGrabbed) {
	  sprintf(msg_string, _AutoMessages[WARNMSG48], routine_name);
	  AutoMessage(msg_string);
	}
           

        action_object = xisInform.pointer_obj;

        xisState.selected_widget = action_object->id.widget;
        xisState.selected_object_type = action_object->id.object_type;
        xisState.selected_instance = action_object->id.instance;

        /* Find window (object) which has elected to receive these events */

        send_object = action_object;    
        subwindow = 0L;

        while (!send_object->id.window) 
            send_object = send_object->parent;

        while (send_object != NULL) {

            if ((send_object->your_event_mask&ButtonPressMask) && 
                (send_object->visibility_state != IsUnmapped) ) {
                pursue = 1;
                send_window = send_object->id.window;
                break;
            }
            if ((send_object->do_not_propagate_mask&ButtonPressMask) &&
                (send_object->visibility_state != IsUnmapped) ) {
                pursue = 0;
                break;
	    }

            subwindow = send_object->id.window;
            send_object = send_object->parent;
        } /* End while() */

        if (!subwindow)
            subwindow = None; /* convert to X's terminology */

        xisGrabPointerSubwindow = subwindow;
    } /* End if (xisState.mod_button_state != 0 && xisPointerGrabbed) */

    if ((send_object != NULL) && pursue) {
    
        if (clicks == 0) 
            xisInform.event_code = EventMouseButtonDown;
        else 
            xisInform.event_code = EventMouseButtonMultiClick;

        /* Call all relavent InformExpectedActions functions */    

        xisInform.is_valid = 1;
        xisInform.action_obj = action_object;
        xisInform.modifier_key_status = modifier_keys;
        xisInform.button_num = button_num;
        xisInform.key_code = 0;
        xisInform.edge_code = 0;
        xisInform.num_clicks = clicks;
    
        current_object = action_object;

        while (current_object != NULL) {
            if (current_object->proc_InformExpectedActions != NULL) {
                xisInform.current_obj = current_object;
                (*current_object->proc_InformExpectedActions)
                                                  (xisInform.event_code);
	    }
            current_object = current_object->parent;
	}

        xisInform.is_valid = 0;

        /* Send synthesized Button Press event */

        (*xisTraceMsg)("    Action_obj=%s\n",xisGetObjectName(action_object));
        (*xisTraceMsg)("    Send_obj = %s\n",xisGetObjectName(send_object));

        if (clicks == 0)
            clicks = 1;

        time = xisGetServerTime(xisMultiClickTime); 

        for (i=0; i<clicks; i++) {
            xisSendEvent(send_window, xisGrabPointerSubwindow,
                         ButtonPress, xisUseCurrentTime && !multi ?
			 CurrentTime : time + 4*i + 1,
                         root_x - send_object->x,
                         root_y - send_object->y, 
                         root_x, root_y, modifier_keys,
                         xisMouseButtonDetail[button_num]);

            xisState.mod_button_state |= xisMouseButtonMask[button_num] |
                                         modifier_keys;

            xisProcessEvents(NULL,0);

            if (!xisPointerGrabbed) {
                /**NoteGrab: after xisProcessEvents now for menus **/
                xisPointerGrabbed = 1;
                xisGrabPointerWindow = send_window;
	 
   
	        if (xisPointerGrabMode == POINTER_GRAB_CONTROLLED) {

                   while (!send_object->id.window)
                        send_object = send_object->parent;

                   XGrabPointer(xisDisplay,send_object->id.window,
                             send_object->your_event_mask&OwnerGrabButtonMask,
                             (send_object->your_event_mask & GOOD_GRAB_BITS) |
                             (ButtonPressMask|ButtonReleaseMask),
                             GrabModeAsync,GrabModeAsync,None,None,
                             xisUseCurrentTime && !multi ? CurrentTime : 
							   time + 4*i + 1 );

                /**NoteGrab***/
              }
            }

            if (clicks > 1) { /* Doing a Multi-click */
                xisSendEvent(send_window, xisGrabPointerSubwindow, 
                             ButtonRelease, xisUseCurrentTime && !multi ?
			     CurrentTime : time + 4*i +2,
                             root_x - send_object->x,
                             root_y - send_object->y, 
                             root_x, root_y, modifier_keys,
                             xisMouseButtonDetail[button_num]);

                xisState.mod_button_state &= (~xisMouseButtonMask[button_num]) &
                                             (~modifier_keys);

                if (xisState.mod_button_state == 0) {
	            if (xisPointerGrabMode == POINTER_GRAB_CONTROLLED)
                       XUngrabPointer(xisDisplay, xisUseCurrentTime && !multi ? 
						  CurrentTime : 
						  time + 4*i + 3);
                    xisPointerGrabbed = 0;
                    xisGrabPointerWindow = 0;
	        }

                xisProcessEvents(NULL,0);
	    }
            else
                clicks = 0;
	}
        if (xisUseSyntheticTime)
            xisSyntheticTime += 4*i;
    } /* End if ((send_object != NULL) && pursue) */

    if (clicks == 0)
        xisState.mod_button_state |= xisMouseButtonMask[button_num] |
                                     modifier_keys;
    else {
        xisState.mod_button_state &= (~xisMouseButtonMask[button_num]);
        xisState.selected_widget = 0;
        xisState.selected_object_type = 0;
        xisState.selected_instance = 0;
    }

} /* End xisCommonButtonPress() */
