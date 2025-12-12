/*
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: TravAct.c /main/14 1999/05/27 13:58:09 mgreess $"
#endif
#endif

#include "TraversalI.h"
#include "TravActI.h"
#include <Xm/GadgetP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ManagerP.h>
#include <Xm/VendorSEP.h>
#include <Xm/MenuShellP.h>
#include "RepTypeI.h"
#include <Xm/VirtKeysP.h>
#include <Xm/DisplayP.h>
#include <Xm/ScrolledWP.h>

#include "ToolTipI.h"


#define EVENTS_EQ(ev1, ev2) \
  ((((ev1)->type == (ev2)->type) &&\
    ((ev1)->serial == (ev2)->serial) &&\
    ((ev1)->time == (ev2)->time) &&\
    ((ev1)->x == (ev2)->x) &&\
    ((ev1)->y == (ev2)->y)) ? TRUE : FALSE)


/********    Static Function Declarations    ********/

static Boolean UpdatePointerData(Widget w, XEvent *event);
static void FlushPointerData(Widget w, XEvent *event);
static void DispatchGadgetInput(XmGadget g, XEvent *event, Mask mask);

/********    End Static Function Declarations    ********/


/*
 * The following functions are used by the widgets to query or modify one
 * of the display dependent global variabled used by traversal mechanism.
 */

unsigned short
_XmGetFocusFlag(Widget w,
		unsigned int mask)
{
  XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(w));

  return ((unsigned short)((XmDisplayInfo *)
	   (dd->display.displayInfo))->resetFocusFlag & mask);
}


void
_XmSetFocusFlag(Widget w,
		unsigned int mask,
     		Boolean value)
{
  XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(w));

  if (value)
     ((XmDisplayInfo *)
	 (dd->display.displayInfo))->resetFocusFlag |= mask;
  else
     ((XmDisplayInfo *)
	 (dd->display.displayInfo))->resetFocusFlag &= ~mask;
}


static Boolean
UpdatePointerData(Widget w,
		  XEvent *event)
{
  XmFocusData focusData;

  if ((focusData = _XmGetFocusData(w)) != NULL)
    {
      XCrossingEvent *lastEvent = &(focusData->lastCrossingEvent);

      focusData->needToFlush = TRUE;

      if (!EVENTS_EQ(lastEvent, (XCrossingEvent *)event))
	{
	  focusData->old_pointer_item = focusData->pointer_item;
	  focusData->pointer_item = w;
	  focusData->lastCrossingEvent = *(XCrossingEvent *) event;
	  return TRUE;
	}
    }

  return FALSE;
}

static void
FlushPointerData(Widget w,
		 XEvent *event)
{
  XmFocusData focusData = _XmGetFocusData(w);

  if (focusData && focusData->needToFlush)
    {
      XCrossingEvent	lastEvent;

      lastEvent = focusData->lastCrossingEvent;

      focusData->needToFlush = FALSE;
      /*
       * We are munging data into the event to fake out the focus
       * code when Mwm is trying to catch up with the pointer.
       * This event that we are munging might already have been
       * munged by XmDispatchGadgetInput from a motion event to a
       * crossing event !!!!!
       */

      lastEvent.serial = event->xany.serial;
      if ( (LeaveNotify == event->type) || (EnterNotify == event->type) )
	      lastEvent.time = event->xcrossing.time;
      else
		/* Approximation; the code appears to need even Focus events,
		** so make up a time and try to continue, rather than limit
		** lastEvent to XCrossingEvents. (It is a flaw in the X
		** Protocol that Focus events do not have timestamps.)
		*/
		lastEvent.time = XtLastTimestampProcessed(XtDisplay(w));
      lastEvent.focus = True;
      XtDispatchEvent((XEvent *) &lastEvent);
    }
}

/************************************************************************
 *
 *  _XmTrackShellFocus
 *
 *  This handler is added by ShellExt initialize to the front of the
 * queue
 *
 ************************************************************************/

void
_XmTrackShellFocus(Widget widget,
		   XtPointer client_data,
		   XEvent *event,
		   Boolean *dontSwallow)
{
  XmVendorShellExtObject ve = (XmVendorShellExtObject) client_data;
  XmFocusData		 focusData;
  XmGeneology		 oldFocalPoint;
  XmGeneology		 newFocalPoint;

  if (widget->core.being_destroyed)
    {
      *dontSwallow = False;
      return;
    }

  if ((focusData = ve->vendor.focus_data) == NULL)
    return;

  oldFocalPoint = newFocalPoint = focusData->focalPoint;

  switch(event->type)
    {
    case EnterNotify:
    case LeaveNotify:
      /*
       * If operating in a focus driven model, then enter and
       * leave events do not affect the keyboard focus.
       */
      if ((event->xcrossing.detail != NotifyInferior) &&
	  (event->xcrossing.focus))
	{
	  switch (oldFocalPoint)
	    {
	    case XmUnrelated:
	      if (event->type == EnterNotify)
		newFocalPoint = XmMyAncestor;
	      break;
	    case XmMyAncestor:
	      if (event->type == LeaveNotify)
		newFocalPoint = XmUnrelated;
	      break;
	    case XmMyDescendant:
	    case XmMyCousin:
	    case XmMySelf:
	    default:
	      break;
	    }
	}
      break;

    case FocusIn:
      switch (event->xfocus.detail)
	{
	case NotifyNonlinear:
	case NotifyAncestor:
	case NotifyInferior:
	  newFocalPoint = XmMySelf;
	  break;
	case NotifyNonlinearVirtual:
	case NotifyVirtual:
	  newFocalPoint = XmMyDescendant;
	  break;
	case NotifyPointer:
	  newFocalPoint = XmMyAncestor;
	  break;
	}
      break;

    case FocusOut:
      switch (event->xfocus.detail)
	{
	case NotifyPointer:
	case NotifyNonlinear:
	case NotifyAncestor:
	case NotifyNonlinearVirtual:
	case NotifyVirtual:
	  newFocalPoint = XmUnrelated;
	  break;
	case NotifyInferior:
	  return;
	}
      break;
    }

  if (newFocalPoint == XmUnrelated)
    {
      focusData->old_focus_item = NULL;

      if (focusData->trav_graph.num_alloc)
	{
	  /* Free traversal graph, since focus is leaving hierarchy. */
	  _XmFreeTravGraph(&(focusData->trav_graph));
	}
    }

  if ((focusData->focus_policy == XmEXPLICIT) &&
      (oldFocalPoint != newFocalPoint) &&
      focusData->focus_item)
    {
      if (oldFocalPoint == XmUnrelated)
	_XmCallFocusMoved(NULL, focusData->focus_item, event);
      else if (newFocalPoint == XmUnrelated)
	_XmCallFocusMoved(focusData->focus_item, NULL, event);
    }

  focusData->focalPoint = newFocalPoint;
}

/************************************************************************
 *
 *  Enter & Leave
 *      Enter and leave event processing routines.
 *
 ************************************************************************/

void
_XmPrimitiveEnter(Widget wid,
		  XEvent *event,
		  String *params,	/* unused */
		  Cardinal *num_params)	/* unused */
{
  _XmToolTipEnter(wid, event, params, num_params);
  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      if (event->xcrossing.focus)
        {
	  _XmCallFocusMoved(XtParent(wid), wid, event);
	  _XmWidgetFocusChange(wid, XmENTER);
	}

      UpdatePointerData(wid, event);
    }
}

void
_XmPrimitiveLeave(Widget wid,
		  XEvent *event,
		  String *params,	/* unused */
		  Cardinal *num_params)	/* unused */
{
  _XmToolTipLeave(wid, event, params, num_params);
  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      if (event->xcrossing.focus)
        {
	  _XmCallFocusMoved(wid, XtParent(wid), event);
	  _XmWidgetFocusChange(wid, XmLEAVE);
	}
    }
}

/************************************************************************
 *
 *  Focus In & Out
 *
 ************************************************************************/

void
_XmPrimitiveFocusInInternal(Widget wid,
			    XEvent *event,
			    String *params,		/* unused */
			    Cardinal *num_params)	/* unused */
{
  if (!(event->xfocus.send_event) ||
      _XmGetFocusFlag(wid, XmFOCUS_IGNORE))
    return;

  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      /* Maybe Mwm trying to catch up with us. */
      if (XtIsShell(XtParent(wid)))
	FlushPointerData(wid, event);
    }
  else
    {
      /* We should only be recieving the focus from a traversal request. */
      if (!_XmGetActiveTabGroup(wid))
	_XmMgrTraversal(_XmFindTopMostShell(wid), XmTRAVERSE_NEXT_TAB_GROUP);
      else
	_XmWidgetFocusChange(wid, XmFOCUS_IN);
    }
}

void
_XmPrimitiveFocusOut(Widget wid,
		     XEvent *event,
		     String *params,		/* unused */
		     Cardinal *num_params)	/* unused */
{
  if (event->xfocus.send_event &&
      !(wid->core.being_destroyed) &&
      (_XmGetFocusPolicy(wid) == XmEXPLICIT))
    {
      _XmWidgetFocusChange(wid, XmFOCUS_OUT);
    }
}

void
_XmPrimitiveFocusIn(Widget pw,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  _XmPrimitiveFocusInInternal(pw, event, params, num_params);
}

/************************************************************************
 *
 *  _XmEnterGadget
 *     This function processes enter window conditions occuring in a gadget
 *
 ************************************************************************/

void
_XmEnterGadget(Widget wid,
	       XEvent *event,
	       String *params,		/* unused */
	       Cardinal *num_params)	/* unused */
{
  if (XmIsGadget(wid) && ((XmGadget)wid)->gadget.traversal_on)
  {
      _XmToolTipEnter(wid, event, params, num_params);
  }
  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      XmFocusData focusData = _XmGetFocusData(wid);

      /* We may be getting called as a result of Mwm catching up
       * with the pointer and setting input focus to the shell
       * which then gets forwarded to us.
       */
      if (focusData && (focusData->focalPoint != XmUnrelated))
        {
	  _XmCallFocusMoved(XtParent(wid), wid, event);
	  _XmWidgetFocusChange(wid, XmENTER);
        }
    }
}

/************************************************************************
 *
 *  DispatchGadgetInput
 *	This routine is used instead of _XmDispatchGadgetInput due to
 *	the fact that it needs to dispatch to unmanaged gadgets
 *
 ************************************************************************/
static void
DispatchGadgetInput(XmGadget g,
		    XEvent *event,
		    Mask mask)
{
   if ((g->gadget.event_mask & mask) && XtIsSensitive((Widget)g))
     {
       (*(((XmGadgetClass) (g->object.widget_class))->
	  gadget_class.input_dispatch)) ((Widget) g, event, mask);
     }
}

/************************************************************************
 *
 *  _XmLeaveGadget
 *     This function processes leave window conditions occuring in a gadget
 *
 ************************************************************************/

void
_XmLeaveGadget(Widget wid,
	       XEvent *event,
	       String *params,		/* unused */
	       Cardinal *num_params)	/* unused */
{
  if (XmIsGadget(wid) && ((XmGadget)wid)->gadget.traversal_on)
  {
      _XmToolTipLeave(wid, event, params, num_params);
  }

  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      _XmCallFocusMoved(wid, XtParent(wid), event);
      _XmWidgetFocusChange(wid, XmLEAVE);
    }
}

/************************************************************************
 *
 *  _XmFocusInGadget
 *     This function processes focusIn conditions occuring in a gadget
 *

 ************************************************************************/
void
_XmFocusInGadget(Widget wid,
		 XEvent *event,		/* unused */
		 String *params,	/* unused */
		 Cardinal *num_params)	/* unused */
{
  if (_XmGetFocusPolicy(wid) == XmEXPLICIT)
    _XmWidgetFocusChange(wid, XmFOCUS_IN);
}

/************************************************************************
 *
 *  _XmFocusOutGadget
 *     This function processes FocusOut conditions occuring in a gadget
 *
 ************************************************************************/

void
_XmFocusOutGadget(Widget wid,
		  XEvent *event,	/* unused */
		  String *params,	/* unused */
		  Cardinal *num_params)	/* unused */
{
  if (_XmGetFocusPolicy(wid) == XmEXPLICIT)
    _XmWidgetFocusChange(wid, XmFOCUS_OUT);
}

/************************************************************************
 *
 *  Enter, FocusIn and Leave Window procs
 *
 *     These two procedures handle traversal activation and deactivation
 *     for manager widgets. They are invoked directly throught the
 *     the action table of a widget.
 *
 ************************************************************************/

/************************************************************************
 *
 *  _XmManagerEnter
 *     This function handles both focusIn and Enter. Don't ask me why
 *     :-(
 *
 ************************************************************************/

void
_XmManagerEnter(Widget wid,
		XEvent *event_in,
		String *params,		/* unused */
		Cardinal *num_params)	/* unused */
{
  XmManagerWidget mw = (XmManagerWidget) wid;
  XCrossingEvent *event = (XCrossingEvent *) event_in;

  if (_XmGetFocusPolicy((Widget) mw) == XmPOINTER)
    {
      if (UpdatePointerData((Widget) mw, event_in) && event->focus)
	{
	  Widget old;

	  if (event->detail == NotifyInferior)
	    old = XtWindowToWidget(event->display, event->subwindow);
	  else
	    old = XtParent(mw);

	  _XmCallFocusMoved(old, (Widget) mw, (XEvent *) event);
	  _XmWidgetFocusChange((Widget) mw, XmENTER);
	}
    }
}

void
_XmManagerLeave(Widget wid,
		XEvent *event,
		String *params,		/* unused */
		Cardinal *num_params)	/* unused */
{
  /*
   * This code is inefficient since it is called twice for each
   * internal move in the hierarchy |||
   */
  if (event->type == LeaveNotify)
    {
      if (_XmGetFocusPolicy(wid) == XmPOINTER)
	{
	  Widget new_wid;

	  if (event->xcrossing.detail == NotifyInferior)
	    new_wid = XtWindowToWidget(event->xcrossing.display,
				       event->xcrossing.subwindow);
	  else
	    new_wid = XtParent(wid);

	  if (UpdatePointerData(wid, event) && event->xcrossing.focus)
	    {
	      _XmCallFocusMoved(wid, new_wid, event);
	      _XmWidgetFocusChange(wid, XmLEAVE);
	    }
	}
    }
}

void
_XmManagerFocusInInternal(Widget wid,
			  XEvent *event,
			  String *params,	/* unused */
			  Cardinal *num_params)	/* unused */
{
  Widget child;

  /*
   * Managers ignore all focus events which have been generated by the
   * window system; only those sent to us by a window manager or the
   * Xtk focus code is accepted.
   * Bail out if the focus policy is not set to explicit
   */
  if (!(event->xfocus.send_event) ||
      _XmGetFocusFlag(wid, XmFOCUS_RESET | XmFOCUS_IGNORE))
    return;

  if (_XmGetFocusPolicy(wid) == XmPOINTER)
    {
      FlushPointerData(wid, event);
    }
  else if (!_XmGetActiveTabGroup(wid))
    {
      /* If the heirarchy doesn't have an active tab group give it one. */
      _XmMgrTraversal(_XmFindTopMostShell(wid), XmTRAVERSE_NEXT_TAB_GROUP);
    }
  else if ((child = ((XmManagerWidget) wid)->manager.active_child) &&
	   XmIsGadget(child))
    {
      /* If focus went to a gadget, then force it to highlight */
      DispatchGadgetInput((XmGadget) child, event, XmFOCUS_IN_EVENT);
    }
  else
    {
      _XmWidgetFocusChange(wid, XmFOCUS_IN);
    }
}

/*
 * Non-menu widgets use this entry point, so that they will ignore focus
 * events during menu activities.
 */
void
_XmManagerFocusIn(Widget mw,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  _XmManagerFocusInInternal(mw, event, params, num_params);
}

/*
 * If the manager widget received a FocusOut while it is processing its
 * FocusIn event, then it knows that the focus has been successfully moved
 * to one of its children.  However, if no FocusOut is received, then the
 * manager widget must manually force the child to take the focus.
 */

void
_XmManagerFocusOut(Widget wid,
		   XEvent *event,
		   String *params,		/* unused */
		   Cardinal *num_params)	/* unused */
{
  Widget child;

  if (!event->xfocus.send_event)
    return;

  if (_XmGetFocusPolicy(wid) == XmEXPLICIT)
    {
      /* If focus is in a gadget, then force it to unhighlight. */
      if ((child = ((XmManagerWidget) wid)->manager.active_child) &&
	  XmIsGadget(child))
        {
	  DispatchGadgetInput((XmGadget) child, event, XmFOCUS_OUT_EVENT);
	}
      else
	{
	  _XmWidgetFocusChange(wid, XmFOCUS_OUT);
	}
    }
}

void
_XmManagerUnmap(Widget mw,
		XEvent *event,		/* unused */
		String *params,		/* unused */
		Cardinal *num_params)	/* unused */
{
  /* This functionality is bogus, since a good implementation
   * requires more code (hooks for mapping of widgets) than it's
   * worth.  To move focus away from a widget when it is unmapped
   * implies the ability to recover from the case when the last
   * traversable widget in a hierarchy is unmapped and then re-mapped.
   * Since we don't have the hooks in place for the mapping of these
   * widgets, and since the old code only worked some of the time,
   * and since it is arguable that the focus should never be
   * changed in response to a widget being unmapped, we should choose
   * to do NO traversal in response to the unmapping of a widget.
   * However, historical precedent again defeats good design.
   */
  _XmValidateFocus(mw);
}

void
_XmPrimitiveUnmap(Widget pw,
		  XEvent *event,	/* unused */
		  String *params,	/* unused */
		  Cardinal *num_params)	/* unused */
{
  _XmValidateFocus(pw);
}
