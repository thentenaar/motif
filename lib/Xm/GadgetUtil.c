/**
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: GadgetUtil.c /main/16 1996/10/23 15:00:52 cde-osf $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <Xm/XmP.h>
#include <Xm/GadgetP.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <Xm/DropSMgr.h>
#include "GadgetUtiI.h"
#include "XmI.h"

/************************************************************************
 *
 *  _XmInputForGadget
 *	This routine is a front-end for XmObjectAtPoint which returns a
 *      gadget or NULL if XmbjectAtPoint is not sensitive.
 *
 ************************************************************************/
XmGadget
_XmInputForGadget(
        Widget wid,
        int x,
        int y )
{
    Widget widget;

    widget = XmObjectAtPoint (wid, x, y);

    if (!widget  ||  !XtIsSensitive (widget))
	return ((XmGadget) NULL);

   return ((XmGadget) widget);
}




/************************************************************************
 *
 *  XmConfigureObject
 *	Wrapper around Xt equivalent + DropSite update.
 *
 ************************************************************************/
void
XmeConfigureObject(
        Widget wid,
        Position x,
        Position y,
        Dimension width,
        Dimension height,
        Dimension border_width )
{
    _XmWidgetToAppContext(wid);
    XmDropSiteStartUpdate(wid);

    _XmAppLock(app);

    if (!width && !height) {
	XtWidgetGeometry   desired, preferred ;
	desired.request_mode = 0;
        XtQueryGeometry(wid, &desired, &preferred);
	width = preferred.width;
        height = preferred.height;
    }
    if (!width)  width++;
    if (!height) height++;
    XtConfigureWidget(wid, x, y, width, height, border_width);

    XmDropSiteEndUpdate(wid);
    _XmAppUnlock(app);
}




/************************************************************************
 *
 *  XmeRedisplayGadgets
 *	Redisplay any gadgets contained within the manager mw which
 *	are intersected by the region.
 *
 ************************************************************************/
void
XmeRedisplayGadgets(
        Widget w,
        XEvent *event,
        Region region )
{
   CompositeWidget mw = (CompositeWidget) w ;
   int i;
   Widget child;
   XtExposeProc expose;

   _XmWidgetToAppContext(w);

   _XmAppLock(app);
   for (i = 0; i < mw->composite.num_children; i++)
   {
      child = mw->composite.children[i];
      if (XmIsGadget(child) && XtIsManaged(child))
      {
         if (region == NULL)
         {
            if (child->core.x < event->xexpose.x + event->xexpose.width      &&
                child->core.x + child->core.width > event->xexpose.x &&
                child->core.y < event->xexpose.y + event->xexpose.height     &&
                child->core.y + child->core.height > event->xexpose.y)
            {

	       _XmProcessLock();
	       expose = child->core.widget_class->core_class.expose;
	       _XmProcessUnlock();

               if (expose)
                  (*(expose))
                     (child, event, region);
            }
         }
         else
         {
            if (XRectInRegion (region, child->core.x, child->core.y,
                               child->core.width, child->core.height))
            {
 	      _XmProcessLock();
	      expose = child->core.widget_class->core_class.expose;
	      _XmProcessUnlock();

              if (expose)
                  (*(expose))
                     (child, event, region);
            }
         }
      }
   }
   _XmAppUnlock(app);
}




/************************************************************************
 *
 *  _XmDispatchGadgetInput
 *	Call the gadgets class function and send the desired data to it.
 *
 ************************************************************************/
void
_XmDispatchGadgetInput(
        Widget wid,
        XEvent *event,
        Mask mask )
{
        XmGadget g = (XmGadget) wid ;
   if ((g->gadget.event_mask & mask) &&
       XtIsSensitive ((Widget)g) && XtIsManaged ((Widget)g))
   {
      if (event != NULL)
      {
         XEvent synth_event;

#define CopyEvent(source, dest, type) \
    source.type = dest->type

         switch(mask) {
	   case XmENTER_EVENT:
                   CopyEvent(synth_event, event, xcrossing);
		   if (event->type != EnterNotify) {
		      synth_event.type = EnterNotify;
                   }
                   break;
	   case XmLEAVE_EVENT:
                   CopyEvent(synth_event, event, xcrossing);
		   if (event->type != LeaveNotify) {
		      synth_event.type = LeaveNotify;
                   }
                   break;
	   case XmFOCUS_IN_EVENT:
                   CopyEvent(synth_event, event, xfocus);
		   if (event->type != FocusIn) {
		      synth_event.type = FocusIn;
		   }
		   break;
	   case XmFOCUS_OUT_EVENT:
                   CopyEvent(synth_event, event, xfocus);
		   if (event->type != FocusIn) {
		      synth_event.type = FocusOut;
		   }
		   break;
	   case XmMOTION_EVENT:
                   CopyEvent(synth_event, event, xmotion);
		   if (event->type != MotionNotify) {
		      event->type = MotionNotify;
		   }
		   break;
	   case XmARM_EVENT:
                   CopyEvent(synth_event, event, xkey);
		   if (event->type != ButtonPress &&
		       event->type != KeyPress) {
		      synth_event.type = ButtonPress;
		   }
		   break;
	   case XmACTIVATE_EVENT:
                   CopyEvent(synth_event, event, xkey);
		   if (event->type != ButtonRelease &&
		       event->type != KeyPress) {
		      synth_event.type = ButtonRelease;
		   }
		   break;
	   case XmKEY_EVENT:
                   CopyEvent(synth_event, event, xkey);
		   if (event->type != KeyPress &&
		       event->type != ButtonPress) {
		      synth_event.type = KeyPress;
		   }
		   break;
	   case XmHELP_EVENT:
                   CopyEvent(synth_event, event, xkey);
		   if (event->type != KeyPress) {
		      synth_event.type = KeyPress;
		   }
		   break;
           default:
		   memcpy((char*)&synth_event, (char*)event,
		      (size_t)sizeof(synth_event));
		   break;
         }

         (*(((XmGadgetClass) (g->object.widget_class))->
             gadget_class.input_dispatch)) ((Widget) g,
                                               (XEvent *) &synth_event, mask) ;
      }
      else
      {
         (*(((XmGadgetClass) (g->object.widget_class))->
             gadget_class.input_dispatch)) ((Widget) g,
                                                  (XEvent *) event, mask) ;
      }
   }
}

