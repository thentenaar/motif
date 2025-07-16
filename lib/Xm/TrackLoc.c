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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: TrackLoc.c /main/12 1995/10/25 20:24:41 cde-sun $"
#endif
#endif

#include <Xm/XmP.h>
#include "XmI.h"
#include "MessagesI.h"

#define GRABPTRERROR    _XmMMsgCascadeB_0003


/********    Static Function Declarations    ********/

static Widget _XmInputInWidget(
                        Widget w,
                        Position x,
                        Position y) ;

/********    End Static Function Declarations    ********/



/******************************************************/
/* copy from XmeGadgetAtPoint, buts works for widget too,
   only used in this module so far, so let it static */
static Widget
_XmInputInWidget(
        Widget w,
        Position x,
        Position y)
{
   int i;
   Widget child;
   CompositeWidget cw = (CompositeWidget) w;

   /* loop over the child list to find if there is one at x,y */
   /* well, overlapping won't really work, since I have no standard way to
      check visibility */
   for (i = 0; i < cw->composite.num_children; i++) {
      child = cw->composite.children[i];
      if (XtIsManaged (child)) {
         if (x >= child->core.x && y >= child->core.y &&
             x < child->core.x + child->core.width    &&
             y < child->core.y + child->core.height)
            return (child);
      }
   }
   return (NULL);
}





/* new tracking locate function that returns the event that trigerred
   the return + this function uses XNextEvent instead of just XMaskEvent
   which was only eating some events and thus causing problem when
   back to the application */

Widget
XmTrackingEvent(
        Widget widget,
        Cursor cursor,
        Boolean confineTo,
	XEvent * pev)
{
    Window      w, confine_to = None;
    Time        lastTime;
    Widget      child;
    Boolean     key_has_been_pressed= False;
    Widget      target ;
    Position x, y ;
    XtAppContext app;

    if (widget == NULL) return(NULL);

    app = XtWidgetToApplicationContext(widget);
    _XmAppLock(app);

    w = XtWindowOfObject(widget);
    if (confineTo) confine_to = w;

    lastTime = XtLastTimestampProcessed(XtDisplay(widget));
    XmUpdateDisplay(widget);

    if (XtGrabPointer(widget, True,
          /* The following truncation of masks is due to a bug in the Xt API.*/
		      (unsigned int) (ButtonPressMask | ButtonReleaseMask),
		      GrabModeAsync, GrabModeAsync,
		      confine_to, cursor, lastTime) != GrabSuccess) {
	XmeWarning(widget, GRABPTRERROR);
	_XmAppUnlock(app);
	return NULL ;
    }

    while (True) {
	/* eat all events, not just button's */
        XNextEvent(XtDisplay(widget), pev);
	/* track only button1release and non first keyrelease */
        if (((pev->type == ButtonRelease) &&
	     (pev->xbutton.button & Button1)) ||
	    ((pev->type == KeyRelease) && key_has_been_pressed)) {

            if ((!confineTo) && (pev->xbutton.window == w)) {
		/* this is the case where we are not confine, so the user
		   can click outside the window, but we want to return
		   NULL */
		if ((pev->xbutton.x < 0) || (pev->xbutton.y < 0) ||
		    (pev->xbutton.x > widget->core.width) ||
		    (pev->xbutton.y > widget->core.height))
		    {
			XtUngrabPointer(widget, lastTime);
			_XmAppUnlock(app);
			return(NULL);
		    }
	    }

	    target = XtWindowToWidget(pev->xbutton.display,
				      pev->xbutton.window);

/* New algorithm that solves the problem of mouse insensitive widgets:
    ( i.e you can't get help on Label.)
   When we get the Btn1Up event with the window in which the event ocurred,
   and convert it to a widget.  If that widget is a primitive, return it.
   Otherwise, it is a composite or a shell, and we do the following:

   Walk down the child list (for gadgets AND WIDGETS), looking for one which
   contains the x,y.  If none is found, return the manager itself,
   If a primitive or gadget is found, return the object found.
   If a manager is found, execute the above algorithm recursively with
   that manager. */

	    if (target) {
		/* do not change the original pev coordinates */
		x = pev->xbutton.x ;
		y = pev->xbutton.y ;
		/* do not enter the loop for primitive */
		while (XtIsComposite(target) || XtIsShell(target)) {
		    if ((child = _XmInputInWidget(target, x,y)) != NULL) {
			target = child;
			/* if a gadget or a primitive is found, return */
			if (!XtIsComposite(child)) break ;

			/* otherwise, loop */
			x = x - XtX(child) ;
			y = y - XtY(child) ;
		    } else break ;
		    /* no child found at this place: return the manager */
		}
	    }
            break ;
        } else if (pev->type == KeyPress) key_has_been_pressed = True ;
	/* to avoid exiting on the first keyreleased coming from
	   the keypress activatation of the function itself */
    }

    XtUngrabPointer(widget, lastTime);
    _XmAppUnlock(app);
    return(target);
}



/* reimplementation of the old function using the new one and a dummy event */
Widget
XmTrackingLocate(
        Widget widget,
        Cursor cursor,
        Boolean confineTo )
{
    XEvent ev ;

    return XmTrackingEvent(widget, cursor, confineTo, &ev);
}



/***********************************************************************
 * Certain widgets, such as those in a menu, would like the application
 * to look non-scraged (i.e. all exposure events have been processed)
 * before invoking a callback which takes a long time to do its thing.
 * This function grabs all exposure events off the queue, and processes
 * them.
 ***********************************************************************/

void
XmUpdateDisplay(
        Widget w )
{
   XEvent event;
   Display * display = XtDisplay(w);
   _XmWidgetToAppContext(w);

   _XmAppLock(app);
   XSync (display, 0);

   while (XCheckMaskEvent(display, ExposureMask, &event))
      XtDispatchEvent(&event);
   _XmAppUnlock(app);
}
