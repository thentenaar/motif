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
static char rcsid[] = "$XConsortium: PrintEvent.c /main/7 1995/07/14 11:30:35 drk $"
#endif
#endif
/***********************************************************************
  	@(#)PrintEvent.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
	Calls:

	Summary:
            Prints out a brief description of the event (mostly on one line)
           
        INPUTS:
            event         - the event structure to be briefly printed out
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisPrintEvent(event)
XEvent *event;
{
    Window window;
    char msg_string[125];

    static char *event_name[] = { 
        "","","KeyPress","KeyRelease","ButtonPress","ButtonRelease",
        "MotionNotify","EnterNotify","LeaveNotify","FocusIn","FocusOut",
        "KeymapNotify","Expose","GraphicsExpose","NoExpose",
        "VisibilityNotify","CreateNotify","DestroyNotify","UnmapNotify",
        "MapNotify","MapRequest","ReparentNotify","ConfigureNotify",
        "ConfigureRequest","GravityNotify","ResizeRequest","CirculateNotify",
        "CirculateRequest","PropertyNotify","SelectionClear","SelectionRequest",
        "SelectionNotify","ColormapNotify","ClientMessage","MappingNotify" };

    static char *crossing_mode[] = {"NotifyNormal","NotifyGrab","NotifyUngrab",
                                    "NotifyWhileGrabbed"};

    static char *crossing_detail[] = {"NotifyAncestor","NotifyVirtual",
                                      "NotifyInferior","NotifyNonlinear",
                                      "NotifyNonlinearVirtual","NotifyPointer",
                                      "NotifyPointerRoot","NotifyDetailNone" };

    switch (event->type) {
      case MotionNotify:
          window = event->xmotion.window;
          break;
      case ButtonPress:
      case ButtonRelease:
          window = event->xbutton.window;
          break;
      case KeyPress:
      case KeyRelease:
          window = event->xkey.window;
          break;
      case EnterNotify:
      case LeaveNotify:
          window = event->xcrossing.window;
          break;
      case FocusIn:
      case FocusOut:
          window = event->xfocus.window;
          break;
      case Expose:
          window = event->xexpose.window;
          break;
      case GraphicsExpose:
          window = event->xgraphicsexpose.drawable;
          break;
      case NoExpose:
          window = event->xnoexpose.drawable;
          break;
      case VisibilityNotify:
          window = event->xvisibility.window;
          break;
      case CreateNotify:
        window = event->xcreatewindow.window;
        break;
      case DestroyNotify:
        window = event->xdestroywindow.window;
        break;
      case UnmapNotify:
        window = event->xunmap.window;
        break;
      case MapNotify:
        window = event->xmap.window;
        break;
      case MapRequest:
        window = event->xmaprequest.window;
        break;
      case ReparentNotify:
        window = event->xreparent.window;
        break;
      case ConfigureNotify:
        window = event->xconfigure.window;
        break;
      case GravityNotify:
        window = event->xgravity.window;
        break;
      case ResizeRequest:
        window = event->xresizerequest.window;
        break;
      case ConfigureRequest:
        window = event->xconfigurerequest.window;
        break;
      case CirculateNotify:
        window = event->xcirculate.window;
        break;
      case CirculateRequest:
        window = event->xcirculaterequest.window;
        break;
      case PropertyNotify:
        window = event->xproperty.window;
        break;
      case SelectionClear:
        window = event->xselectionclear.window;
        break;
      case SelectionRequest:
        window = event->xselectionrequest.owner;
        break;
      case SelectionNotify:
        window = event->xselection.requestor;
        break;
      case ColormapNotify:
        window = event->xcolormap.window;
        break;
      case ClientMessage:
        window = event->xclient.window;
        break;
      case MappingNotify:
        window = event->xmapping.window;
        break;
      case KeymapNotify:
        window = event->xkeymap.window;
        break;
      default:
        AutoMessage(_AutoMessages[WARNMSG125]);
        window = xisRootWindow;
        break;
    }

    (*xisTraceMsg)("Received %s event\n\t\t\t from %s, send_event=%d\n",
                   event_name[event->type],
                   xisGetObjectName(xisFindObjectFromWindow(window)),
                   event->xany.send_event);

    if (event->type == EnterNotify || event->type == LeaveNotify) {
        (*xisTraceMsg)("with crossing_mode=%s, detail=%s\n",
                       crossing_mode[event->xcrossing.mode],
                       crossing_detail[event->xcrossing.detail] );
    }

    else if (event->type == KeyPress || event->type == KeyRelease) {
        char buf[20];
        char outbuf[80];
        int buf_size;
        KeySym keysym_return;
        buf_size = XLookupString((XKeyEvent *)event,buf,20,&keysym_return,
                                 (XComposeStatus *)NULL);
        buf[buf_size] = '\0';
        if (isprint(buf[0]))
            sprintf(outbuf,"'%s'",buf);
        else if (iscntrl(buf[0]))
            sprintf(outbuf,"'^%c'",buf[0]+'A'-1);
        else 
            sprintf(outbuf,"non-printable");
            
        (*xisTraceMsg)("Keycode = %d (%s)\n",buf[0],outbuf);
    }

} /* End xisPrintEvent() */
