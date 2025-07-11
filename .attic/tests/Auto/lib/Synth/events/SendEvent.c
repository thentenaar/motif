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
static char rcsid[] = "$XConsortium: SendEvent.c /main/8 1995/07/14 11:30:58 drk $"
#endif
#endif
/***********************************************************************
  	@(#)SendEvent.c	1.12.1.2	Date:2/8/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
            11/27/90 PSN quick fix for time stamp that was failing the
			 List and Text class of widgets. Use CurrentTime
			 instead of time passed in.

	Calls:

	Summary:
            Sets up and sends one event to the specified window on the display
        specified by the global variable display (local to this module only).

        INPUTS:
            window    - window to send the event to
            subwindow - child window in direction of descendent where event
                        happened
            type      - event type (ButtonPress,ButtonRelease,KeyPress,
                        KeyRelease,etc)
            time      - event time in milliseconds (use xisGetServerTime())
            wx,wy     - location of pointer when event occured relative to
                        window
            rx,ry     - location of pointer when event occured relative to root
            state     - key or button mask (Button1Mask, etc)
            detail    - Button that changed state (Button1,etc) 
                        or Key pressed (KeyReturn,KeySpace, etc)
        
        OUTPUTS:
            none

        RETURNS:
            void
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisSendEvent(window,subwindow,type,time,wx,wy,rx,ry,state,detail)
Window window,subwindow;
int type;
Time time;
int wx,wy,rx,ry;
unsigned int state,detail;
{
    char msg_string[125];
    static char routine_name[] = "xisSendEvent";

    static unsigned int event_mask[] = 
       { 0,0,KeyPressMask,KeyPressMask|KeyReleaseMask,
	 ButtonPressMask,
	 ButtonPressMask|ButtonReleaseMask,
         PointerMotionMask|Button1MotionMask|
         Button2MotionMask|Button3MotionMask|Button4MotionMask|
         Button5MotionMask|ButtonMotionMask,EnterWindowMask,LeaveWindowMask };

    XEvent event;

    xisUseSessionInfo(routine_name);

    /* Hack for List and Text tests that does not get correct time from X11r3
     * server and crashes.
     */
    if (xisUseCurrentTime && 
	(xisInform.event_code != EventMouseButtonMultiClick ||
	 xisInform.num_clicks < 2) )
	time = CurrentTime;

    event.type = type;
    event.xany.display = xisDisplay;
    event.xany.window = window;

    switch (type) {
      case KeyPress:
      case KeyRelease:
        event.xkey.root = xisRootWindow;
        event.xkey.subwindow = subwindow;
        event.xkey.x = wx;
        event.xkey.y = wy;
        event.xkey.x_root = rx;
        event.xkey.y_root = ry;
        event.xkey.state = state | xisKeySymName[detail].modifier;
        event.xkey.keycode =  xisKeySymName[detail].code;
        event.xkey.same_screen = True;
        event.xkey.time = time;
  /*
   *     msg_info(" we are sending key code %d:%d that represents ]%s[ \n",
   *          xisKeySymName[detail].code,state,xisKeySymName[detail].key_name);
   */
        break;

    case ButtonPress:
    case ButtonRelease:
        event.xbutton.root = xisRootWindow;
        event.xbutton.subwindow = subwindow;
        event.xbutton.x = wx;
        event.xbutton.y = wy;
        event.xbutton.x_root = rx;
        event.xbutton.y_root = ry;
        event.xbutton.state = state;
        event.xbutton.button = detail;
        event.xbutton.same_screen = True;
        event.xbutton.time = time;
        break;

    case MotionNotify:
        event.xmotion.root = xisRootWindow;
        event.xmotion.subwindow = subwindow;
        event.xmotion.x = wx;
        event.xmotion.y = wy;
        event.xmotion.x_root = rx;
        event.xmotion.y_root = ry;
        event.xmotion.state = state;
        event.xmotion.is_hint = detail;
        event.xmotion.same_screen = True;
        event.xmotion.time = time;
        break;
      
      default:
        sprintf (msg_string, _AutoMessages[WARNMSG47], type);
        AutoMessage(msg_string);
        return;
        break;
    }

    XSendEvent(xisDisplay,window,True,event_mask[type],&event);

} /* End xisSendEvent() */










