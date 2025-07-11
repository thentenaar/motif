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
static char rcsid[] = "$XConsortium: KeyRelease.c /main/8 1995/07/14 11:41:16 drk $"
#endif
#endif
/***********************************************************************
  	@(#)KeyRelease.c	1.6.1.2	Date:2/8/91
  	Author: SJS
	History:
            05/30/90 SJS add to sccs
            05/31/90 SJS xisGetFocusObject is changed()
            10/17/90 SJS xisState.focus_window refers to X focus window
	Calls:

	Summary:
            Simulates a user releasing a keyboard key.

        INPUTS:
            key - key code (i.e., KeyReturn, KeyUp, KeyRight, KeySpace, etc.)
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include "xislib.h"
#include <Xm/ScrolledW.h>

/*
  When an X-server gets a key press from the real keyboard, this is how it 
  determines the appropriate event window to send it to:

    If ActiveGrabInProgress (keyboard)
        Send to client window that grabbed it (if visible)

    Else
        Search visible ancestors of window from root down for a key grab
        If found, 
            Activate grab (call XGrabKeyboard) on that window (pg 144 Xlib)
            Send event to that same window

        Else If focus_window = None
            Ignore Event
 
        Else If focus_window = PointerRoot    
            Starting from lowest ancestor window containing pointer, go up tree
               (up meaning to ancestors) looking for window that has this event 
               selected.
            Send event to that window (with subwindow set to child in direction
               of window containing pointer).

        Else (focus_window = some specific window)
            If the lowest window containing the pointer is an inferior of
            the focus_window, 
                Start searching from there up the hierarchy for a window that 
                has selected this event
            Else
                Start searching from the focus window up the hierarchy for a
                    window that has selected this event.

  For keypress and keyrelease events, basically keyboard grabs preside overall.
  These can be actively set via a call to XGrabKeyboard or passively set via
  a call to XGrabKey followed by the key press action (staying grabbed for the 
  duration of the keypress).

  If there are no active or passive grabs, then the focus settings come into 
  play. These are either set to None, PointerRoot, or to a specific window.  
  Basically, if the window manager has a pointer-driven approach to input focus
  it will call XSetInputFocus with a PointerRoot mode setting which means to 
  let the input focus go to which ever window the pointer is currently in.  
  If it has an explicit keyboard focus policy (meaning a "click-to-type" style)
  than it calls XSetInputFocus with the application window which the pointer 
  is in only when the primary mouse button is pressed.  If for some reason 
  either the window manager or the application calls XSetInputFocus with the 
  window set to None, then all subsequent keyboard events are ignored until 
  the focus is changed.

  For our purposes (like with button presses) we have no way of knowing which
  keys have been grabbed by which windows.  Furthermore, we have no way of
  knowing whether a current grab is in effect or not.
*/

extern Boolean SendToClipWindow;	/* 
					   Calculated in Script lib to see
					   if the event should be possibly
					   passed to clip window. This is 
					   because of the Grabs going on
					   in the ScrolledWindow code. 
					 */

static XisObjectRecord *GetClipWindowObject();

void xisKeyRelease (modifier_keys, key_code)
unsigned int modifier_keys;
int key_code;
{
    XisObjectRecord *focus_object,*send_object;
    Window subwindow = 0L;
    Window send_window = 0L;
    int root_x,root_y;
    int pursue = 1;
    XisObjectRecord *current_object;
    Time time;

    (*xisTraceMsg)("Got send_event_request = %s\n",xisGetKeyName(key_code));

    /* Get current focus widget */

    focus_object = xisGetFocusObject();    

    xisUpdateObjectAttributes();

    /* If it exists and is known to this application context... */

    if (focus_object != NULL) {

        /* Call all appropriate InformExpectedActions functions */

        current_object = focus_object;
        xisInform.is_valid = 1;
        send_window = current_object->id.window;
        xisInform.action_obj = current_object;
       /* rsg was here */
        xisInform.modifier_key_status = modifier_keys;
        xisInform.key_code = key_code;
        xisInform.edge_code = 0;
        xisInform.num_clicks = 0;
        xisInform.button_num = 0;
        xisInform.edge_code = 0;
        xisInform.event_code = EventKeyRelease;

        while (current_object != NULL) {
            if (current_object->proc_InformExpectedActions != NULL) {
                xisInform.current_obj = current_object;

                (*current_object->proc_InformExpectedActions)
                                                     (xisInform.event_code);
	    }
            current_object = current_object->parent;
        }

        xisInform.is_valid = 0;


        /* Find window (from xis) that has KeyRelease event selected... */

        subwindow = None;
        send_object = xisFindObjectFromWindow(send_window);

        while (send_object != NULL) {

            if ((send_object->your_event_mask&KeyPressMask) && 
                (send_object->visibility_state != IsUnmapped)     ) {
                pursue = 1;
                break;
            }

            if ((send_object->do_not_propagate_mask&KeyPressMask) &&
                (send_object->visibility_state != IsUnmapped)     ) {
                pursue = 0;
                break;
	    }

            subwindow = send_object->id.window;

            send_object= send_object->parent;
        }
 
        if (!subwindow)
            subwindow = None;

        if (pursue && send_object != NULL) {

	    if (SendToClipWindow) {
		send_object = GetClipWindowObject(send_object);
		subwindow = send_object->id.window;
	    }

	    xisLastEventTime += 500;
            time = xisGetServerTime(0); 

            xisGetPointerLocation(&root_x,&root_y);

            xisSendEvent( send_object->id.window, None,
                          KeyRelease,
                          time+1,
                          root_x - send_object->x,
                          root_y - send_object->y,
                          root_x, root_y,
                          modifier_keys,(unsigned int)key_code);  

            xisProcessEvents(NULL,0);
        }
    }

} /* End xisKeyRelease() */


static XisObjectRecord *GetClipWindowObject(object)
XisObjectRecord *object;
{
   XisObjectRecord *return_object;
   Widget widget, clip_window;
   unsigned char scrolling_policy;
   int n;
   Arg args[2];

   widget = object->id.widget;

   while (widget != NULL && ! XmIsScrolledWindow(widget))
	widget = XtParent(widget);

   if (widget == NULL)
	return(object);
   else {
	n = 0;
	XtSetArg(args[n], XmNclipWindow, &clip_window);			n++;
	XtSetArg(args[n], XmNscrollingPolicy, &scrolling_policy); 	n++;
	XtGetValues(widget, args, n);

	if (scrolling_policy != XmAUTOMATIC)
		return(object);
	else {
		if (clip_window != NULL) {
			return_object = xisFindObjectFromWidget(clip_window);
			return(return_object);
		}
		else
			return(object);
	}
   }

}
