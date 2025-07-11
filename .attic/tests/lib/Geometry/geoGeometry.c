/* $XConsortium: geoGeometry.c /main/5 1995/07/15 21:13:17 drk $ */
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

#include "IntrinsicI.h"
#include "ShellP.h"

#include "geoTattler.h"

static void ClearRectObjAreas(r, old)
    RectObj r;
    XWindowChanges* old;
{
    Widget pw = _XtWindowedAncestor((Widget)r);
    int bw2;

    bw2 = old->border_width << 1;
    XClearArea( XtDisplay(pw), XtWindow(pw),
		old->x, old->y,
		old->width + bw2, old->height + bw2,
		TRUE );

    bw2 = r->rectangle.border_width << 1;
    XClearArea( XtDisplay(pw), XtWindow(pw),
		(int)r->rectangle.x, (int)r->rectangle.y,
		(unsigned int)(r->rectangle.width + bw2),
	        (unsigned int)(r->rectangle.height + bw2),
		TRUE );
}

/*
 * Internal function used by XtMakeGeometryRequest and XtSetValues.
 * Returns more data than the public interface.  Does not convert
 * XtGeometryDone to XtGeometryYes.
 *
 * clear_rect_obj - *** RETURNED ***  
 *		    TRUE if the rect obj has been cleared, false otherwise.
 */

XtGeometryResult 
_XtMakeGeometryRequest (widget, request, reply, clear_rect_obj)
    Widget widget;
    XtWidgetGeometry *request, *reply;
    Boolean * clear_rect_obj;
{
    XtWidgetGeometry    junk;
    XtGeometryHandler manager;
    XtGeometryResult returnCode;
    Widget parent = widget->core.parent;
    XtGeometryMask	changeMask;
    Boolean managed, parentRealized, rgm = False;
    XWindowChanges changes;

    *clear_rect_obj = FALSE;

    _GeoPrintTrace(widget,
	"\"%s\" is making a %sgeometry request to its parent \"%s\".\n",
		   XtName(widget),
		   ((GMode (request) & XtCWQueryOnly))? "query only ":"",
		   (XtParent(widget))?XtName(XtParent(widget)):"Root");
    _GeoTabTrace();

    if (XtIsShell(widget)) {
	ShellClassExtension ext;
	for (ext = (ShellClassExtension)((ShellWidgetClass)XtClass(widget))
		   ->shell_class.extension;
	     ext != NULL && ext->record_type != NULLQUARK;
	     ext = (ShellClassExtension)ext->next_extension);

	if (ext != NULL) {
	    if (  ext->version == XtShellExtensionVersion
		  && ext->record_size == sizeof(ShellClassExtensionRec)) {

		manager = ext->root_geometry_manager;
		rgm = True;
	    } else {
		String params[1];
		Cardinal num_params = 1;
		params[0] = XtClass(widget)->core_class.class_name;
		XtAppErrorMsg(XtWidgetToApplicationContext(widget),
		     "invalidExtension", "xtMakeGeometryRequest",
		     XtCXtToolkitError,
		     "widget class %s has invalid ShellClassExtension record",
		     params, &num_params);
	    }
	} else {
	    XtAppErrorMsg(XtWidgetToApplicationContext(widget),
			  "internalError", "xtMakeGeometryRequest",
			  XtCXtToolkitError,
			  "internal error; ShellClassExtension is NULL",
			  NULL, NULL);
	}
	managed = True;
	parentRealized = TRUE;
    } else if (parent == NULL) {
	XtAppErrorMsg(XtWidgetToApplicationContext(widget),
		      "invalidParent","xtMakeGeometryRequest",XtCXtToolkitError,
		      "non-shell has no parent in XtMakeGeometryRequest",
		      (String *)NULL, (Cardinal *)NULL);
    } else /* not shell */ {
	managed = XtIsManaged(widget);

	if (XtIsComposite(parent)) {
	    parentRealized = XtIsRealized(parent);
	    manager = ((CompositeWidgetClass) (parent->core.widget_class))
		    ->composite_class.geometry_manager;
	} else if (managed) {
	    /* Should never happen - XtManageChildren should have checked */
	    XtAppErrorMsg(XtWidgetToApplicationContext(widget),
			  "invalidParent", "xtMakeGeometryRequest",
			  XtCXtToolkitError,
			  "XtMakeGeometryRequest - parent not composite",
			  (String *)NULL, (Cardinal *)NULL);
	} else {
	    /* no need to waste time checking if parent is actually realized
	     * at this point; since the child is unmanaged we need to perform
	     * the configure iff the child is realized, so we dummy the
	     * parentRealized checks below.
	     */
	    parentRealized = TRUE;
	}
    }

    if (managed && manager == (XtGeometryHandler) NULL) {
	XtErrorMsg("invalidGeometryManager","xtMakeGeometryRequest",
                 XtCXtToolkitError,
                 "XtMakeGeometryRequest - parent has no geometry manager",
                  (String *)NULL, (Cardinal *)NULL);
    }

    if (widget->core.being_destroyed) {
	_GeoUnTabTrace();
	_GeoPrintTrace(widget,
		       "It is being destroyed, just return XtGeometryNo.\n");
	return XtGeometryNo;
    }

    /* see if requesting anything to change */
    changeMask = 0;
    if (request->request_mode & CWStackMode
	&& request->stack_mode != XtSMDontChange) {
	    changeMask |= CWStackMode;
	    _GeoPrintTrace(widget,"Asking for a change in StackMode!\n");
	    if (request->request_mode & CWSibling) {
		XtCheckSubclass(request->sibling, rectObjClass,
				"XtMakeGeometryRequest");
		changeMask |= CWSibling;
	    }
    }
    if (request->request_mode & CWX
	&& widget->core.x != request->x) {
	_GeoPrintTrace(widget,"Asking for a change in x: from %d to %d.\n",
		       widget->core.x, request->x);
	changeMask |= CWX;
    }
    if (request->request_mode & CWY
	&& widget->core.y != request->y) {
	_GeoPrintTrace(widget,"Asking for a change in y: from %d to %d.\n",
		       widget->core.y, request->y);
	changeMask |= CWY;
    }
    if (request->request_mode & CWWidth
	&& widget->core.width != request->width) {
	_GeoPrintTrace(widget,"Asking for a change in width: from %d to %d.\n",
		       widget->core.width, request->width);
	changeMask |= CWWidth;
    }
    if (request->request_mode & CWHeight
	&& widget->core.height != request->height) {
	_GeoPrintTrace(widget,"Asking for a change in height: from %d to %d.\n",
		       widget->core.height, request->height);
	changeMask |= CWHeight;
    }
    if (request->request_mode & CWBorderWidth
	&& widget->core.border_width != request->border_width){
	_GeoPrintTrace(widget,
		       "Asking for a change in border_width: from %d to %d.\n",
		       widget->core.border_width, request->border_width);
	changeMask |= CWBorderWidth;
    }
    if (! changeMask) {
	_GeoPrintTrace(widget,
		       "Asking for nothing new,\n");
	_GeoUnTabTrace();
	_GeoPrintTrace(widget,
		       "just return XtGeometryYes.\n");
	return XtGeometryYes;
    }

    changeMask |= (request->request_mode & XtCWQueryOnly);

    if ( !(changeMask & XtCWQueryOnly) && XtIsRealized(widget) ) {
	/* keep record of the current geometry so we know what's changed */
	changes.x = widget->core.x ;
	changes.y = widget->core.y ;
	changes.width = widget->core.width ;
	changes.height = widget->core.height ;
	changes.border_width = widget->core.border_width ;
    }

    if (!managed || !parentRealized) {
	if (!managed) {
	    _GeoPrintTrace(widget,"\"%s\" is not managed yet.\n",XtName(widget));
	}
	if (!parentRealized) {
	    _GeoPrintTrace(widget,"Parent \"%s\" is not realized yet.\n",
			   (XtParent(widget))?XtName(XtParent(widget)):"Root");
	}
	/* Don't get parent's manager involved--assume the answer is yes */
	if (changeMask & XtCWQueryOnly) {
	    /* He was just asking, don't change anything, just tell him yes */
	    _GeoPrintTrace(widget,"QueryOnly request\n");
	    _GeoUnTabTrace();
	    _GeoPrintTrace(widget,"just return XtGeometryYes.\n");
	    return XtGeometryYes;
	} else {
	    _GeoPrintTrace(widget,"Copy values from request to widget.\n");
	    /* copy values from request to widget */
	    if (request->request_mode & CWX)
		widget->core.x = request->x;
	    if (request->request_mode & CWY)
		widget->core.y = request->y;
	    if (request->request_mode & CWWidth)
		widget->core.width = request->width;
	    if (request->request_mode & CWHeight)
		widget->core.height = request->height;
	    if (request->request_mode & CWBorderWidth)
		widget->core.border_width = request->border_width;
	    if (!parentRealized) {
		_GeoUnTabTrace();
		_GeoPrintTrace(widget,"and return XtGeometryYes.\n");
		return XtGeometryYes;
	    }
	    else returnCode = XtGeometryYes;
	}
    } else {
	/* go ask the widget's geometry manager */
	_GeoPrintTrace(widget,"Go ask the parent geometry manager.\n");
	if (reply == (XtWidgetGeometry *) NULL) {
	    returnCode = (*manager)(widget, request, &junk);
	} else {
	    returnCode = (*manager)(widget, request, reply);
	}
    }

    /*
     * If Unrealized, not a XtGeometryYes, or a query-only then we are done.
     */

    if ((returnCode != XtGeometryYes) || 
	(changeMask & XtCWQueryOnly) || !XtIsRealized(widget)) {

	switch(returnCode){
	case XtGeometryNo:
	    _GeoUnTabTrace();
	    _GeoPrintTrace(widget,"\"%s\" returns XtGeometryNo.\n",
			     (XtParent(widget))?XtName(XtParent(widget)):"Root");
            /* check for no change */
            break ;
	case XtGeometryDone:
	    _GeoUnTabTrace();
	    _GeoPrintTrace(widget,"\"%s\" returns XtGeometryDone.\n",
			     (XtParent(widget))?XtName(XtParent(widget)):"Root");
            /* check for no change in queryonly */
            break ;
	case XtGeometryAlmost:
	    _GeoUnTabTrace();
	    _GeoPrintTrace(widget,"\"%s\" returns XtGeometryAlmost.\n",
			     (XtParent(widget))?XtName(XtParent(widget)):"Root");
	    _GeoTabTrace();
	    _GeoPrintTrace(widget,"Proposal: width %d height %d.\n",
			   (reply)?reply->width:junk.width,
			   (reply)?reply->height:junk.height);
	    _GeoUnTabTrace();
	    
            /* check for no change */
            break ;
	case XtGeometryYes:
	    if (changeMask & XtCWQueryOnly) {
		_GeoPrintTrace(widget,"QueryOnly specified, no configuration.\n");
	    }
            if (!XtIsRealized(widget)) {
		_GeoPrintTrace(widget,"\"%s\" not realized, no configuration.\n",
				 XtName(widget));
	    }
	    _GeoUnTabTrace();
            _GeoPrintTrace(widget,"\"%s\" returns XtGeometryYes.\n",
			     (XtParent(widget))?XtName(XtParent(widget)):"Root");
	    break ;
	}

	return returnCode;
    }

    _GeoUnTabTrace();
    _GeoPrintTrace(widget,"\"%s\" returns XtGeometryYes.\n",
		   (XtParent(widget))?XtName(XtParent(widget)):"Root");

    
    if (XtIsWidget(widget)) {	/* reconfigure the window (if needed) */

	if (rgm) return returnCode;	

	if (changes.x != widget->core.x) {
 	    changeMask |= CWX;
 	    changes.x = widget->core.x;
 	}
 	if (changes.y != widget->core.y) {
 	    changeMask |= CWY;
 	    changes.y = widget->core.y;
 	}
 	if (changes.width != widget->core.width) {
 	    changeMask |= CWWidth;
 	    changes.width = widget->core.width;
 	}
 	if (changes.height != widget->core.height) {
 	    changeMask |= CWHeight;
 	    changes.height = widget->core.height;
 	}
 	if (changes.border_width != widget->core.border_width) {
 	    changeMask |= CWBorderWidth;
 	    changes.border_width = widget->core.border_width;
 	}
	if (changeMask & CWStackMode) {
	    changes.stack_mode = request->stack_mode;
	    if (changeMask & CWSibling)
 			if (XtIsWidget(request->sibling))
 		    	changes.sibling = XtWindow(request->sibling);
 			else
 		    	changeMask &= ~(CWStackMode | CWSibling);
	}

        if (changeMask) {
	    _GeoPrintTrace(widget,"XConfigure \"%s\"'s window.\n",XtName(widget));
	} else {
	    _GeoPrintTrace(widget,"No window configuration needed for \"%s\".\n",
			     XtName(widget));
	}

	XConfigureWindow(XtDisplay(widget), XtWindow(widget),
			 changeMask, &changes);
    }
    else {			/* RectObj child of realized Widget */
	*clear_rect_obj = TRUE;

	_GeoPrintTrace(widget,"ClearRectObj on \"%s\".\n",XtName(widget));

	ClearRectObjAreas((RectObj)widget, &changes);
    }

    return returnCode;
} /* _XtMakeGeometryRequest */


/* Public routines */

XtGeometryResult XtMakeGeometryRequest (widget, request, reply)
    Widget         widget;
    XtWidgetGeometry *request, *reply;
{
    Boolean junk;
    XtGeometryResult returnCode;

    returnCode = _XtMakeGeometryRequest(widget, request, reply, &junk);

    return ((returnCode == XtGeometryDone) ? XtGeometryYes : returnCode);
}

#if NeedFunctionPrototypes
XtGeometryResult XtMakeResizeRequest(
    Widget	widget,
    _XtDimension width,
    _XtDimension height,
    Dimension	*replyWidth,
    Dimension	*replyHeight
    )
#else
XtGeometryResult XtMakeResizeRequest
	(widget, width, height, replyWidth, replyHeight)
    Widget	widget;
    Dimension	width, height;
    Dimension	*replyWidth, *replyHeight;
#endif
{
    XtWidgetGeometry request, reply;
    XtGeometryResult r;

    request.request_mode = CWWidth | CWHeight;
    request.width = width;
    request.height = height;
    r = XtMakeGeometryRequest(widget, &request, &reply);
    if (replyWidth != NULL)
	if (r == XtGeometryAlmost && reply.request_mode & CWWidth)
	    *replyWidth = reply.width;
	else
	    *replyWidth = width;
    if (replyHeight != NULL)
	if (r == XtGeometryAlmost && reply.request_mode & CWHeight)
	    *replyHeight = reply.height;
	else
	    *replyHeight = height;
    return r;
} /* XtMakeResizeRequest */

void XtResizeWindow(w)
    Widget w;
{
    if (XtIsRealized(w)) {
	XWindowChanges changes;
	changes.width = w->core.width;
	changes.height = w->core.height;
	changes.border_width = w->core.border_width;
	XConfigureWindow(XtDisplay(w), XtWindow(w),
	    (unsigned) CWWidth | CWHeight | CWBorderWidth, &changes);
    }
} /* XtResizeWindow */


#if NeedFunctionPrototypes
void XtConfigureWidget(
    Widget w,
    _XtPosition x,
    _XtPosition y,
    _XtDimension width,
    _XtDimension height,
    _XtDimension borderWidth
    )
#else
void XtConfigureWidget(w, x, y, width, height, borderWidth)
    Widget w;
    Position x, y;
    Dimension width, height, borderWidth;
#endif
{
    XWindowChanges changes, old;
    Cardinal mask = 0;

    _GeoPrintTrace(w,"\"%s\" is being configured by its parent \"%s\"\n",
		XtName(w), XtName(XtParent(w)));
    _GeoTabTrace();
    
    if ((old.x = w->core.x) != x) {
	_GeoPrintTrace(w,"x move from %d to %d\n",w->core.x, x);
	changes.x = w->core.x = x;
	mask |= CWX;
    }

    if ((old.y = w->core.y) != y) {
	_GeoPrintTrace(w,"y move from %d to %d\n",w->core.y, y);
	changes.y = w->core.y = y;
	mask |= CWY;
    }

    if ((old.width = w->core.width) != width) {
	_GeoPrintTrace(w,"width move from %d to %d\n",w->core.width, width);
	changes.width = w->core.width = width;
	mask |= CWWidth;
    }

    if ((old.height = w->core.height) != height) {
	_GeoPrintTrace(w,"height move from %d to %d\n",w->core.height, height);
	changes.height = w->core.height = height;
	mask |= CWHeight;
    }

    if ((old.border_width = w->core.border_width) != borderWidth) {
	_GeoPrintTrace(w,"border_width move from %d to %d\n",
		    w->core.border_width,borderWidth );
	changes.border_width = w->core.border_width = borderWidth;
	mask |= CWBorderWidth;
    }

    if (mask != 0) {
	if (XtIsRealized(w)) {
	    if (XtIsWidget(w)) {
		_GeoPrintTrace(w,"XConfigure \"%s\"'s window\n",XtName(w));
		XConfigureWindow(XtDisplay(w), XtWindow(w), mask, &changes);
	    }
	    else {
		_GeoPrintTrace(w,"ClearRectObj called on \"%s\"\n",XtName(w));
		ClearRectObjAreas((RectObj)w, &old);
	    }
	} else {
	    _GeoPrintTrace(w,"\"%s\" not Realized\n",XtName(w));
	}

	if ((mask & (CWWidth | CWHeight)) &&
	      XtClass(w)->core_class.resize != (XtWidgetProc) NULL) {

	    _GeoPrintTrace(w,"Resize proc is called.\n");

	    (*(w->core.widget_class->core_class.resize))(w);
	} else {
	    _GeoPrintTrace(w,"Resize proc is not called.\n");
	}
	    
    } else {
	_GeoPrintTrace(w,"No change in configuration\n");
    }

    _GeoUnTabTrace();

} /* XtConfigureWidget */


#if NeedFunctionPrototypes
void XtResizeWidget(
    Widget w,
    _XtDimension width,
    _XtDimension height,
    _XtDimension borderWidth
    )
#else
void XtResizeWidget(w, width, height, borderWidth)
    Widget w;
    Dimension width, height, borderWidth;
#endif
{
    XtConfigureWidget(w, w->core.x, w->core.y, width, height, borderWidth);
    
} /* XtResizeWidget */


#if NeedFunctionPrototypes
void XtMoveWidget(
    Widget w,
    _XtPosition x,
    _XtPosition y
    )
#else
void XtMoveWidget(w, x, y)
    Widget w;
    Position x, y;
#endif
{
    XtConfigureWidget(w, x, y, w->core.width, w->core.height,
		      w->core.border_width);
    
} /* XtMoveWidget */

#if NeedFunctionPrototypes
void XtTranslateCoords(
    register Widget w,
    _XtPosition x,
    _XtPosition y,
    register Position *rootx,	/* return */
    register Position *rooty	/* return */
    )
#else
void XtTranslateCoords(w, x, y, rootx, rooty)
    register Widget w;
    Position x, y;
    register Position *rootx, *rooty;	/* return */
#endif
{
    Position garbagex, garbagey;
    Widget passed = w;

    if (rootx == NULL) rootx = &garbagex;
    if (rooty == NULL) rooty = &garbagey;

    *rootx = x;
    *rooty = y;

    for (; w != NULL && ! XtIsShell(w); w = w->core.parent) {
	*rootx += w->core.x + w->core.border_width;
	*rooty += w->core.y + w->core.border_width;
    }

    if (w == NULL)
        XtAppWarningMsg(XtWidgetToApplicationContext(passed),
		"invalidShell","xtTranslateCoords",XtCXtToolkitError,
                "Widget has no shell ancestor",
		(String *)NULL, (Cardinal *)NULL);
    else {
	Position x, y;
	extern void _XtShellGetCoordinates();
	_XtShellGetCoordinates( w, &x, &y );
	*rootx += x + w->core.border_width;
	*rooty += y + w->core.border_width;
    }
}

XtGeometryResult XtQueryGeometry(widget, intended, reply)
    Widget widget;
    register XtWidgetGeometry *intended; /* parent's changes; may be NULL */
    XtWidgetGeometry *reply;	/* child's preferred geometry; never NULL */
{
    XtWidgetGeometry null_intended;
    XtGeometryHandler query = XtClass(widget)->core_class.query_geometry;
    XtGeometryResult result;

    _GeoPrintTrace(widget,"\"%s\" is asking its preferred geometry to \"%s\".\n",
		     (XtParent(widget))?XtName(XtParent(widget)):"Root",
		     XtName(widget));
    _GeoTabTrace();

    reply->request_mode = 0;
    if (query != NULL) {
	if (intended == NULL) {
	    null_intended.request_mode = 0;
	    intended = &null_intended;

	    _GeoPrintTrace(widget,"without any constraint.\n");
	    
	} else {
	    _GeoPrintTrace(widget,"with the following constraints:\n");

	    if (intended->request_mode & CWX) {
		_GeoPrintTrace(widget," x = %d\n",intended->x);
	    }
	    if (intended->request_mode & CWY) {
		_GeoPrintTrace(widget," y = %d\n",intended->y);
	    }
	    if (intended->request_mode & CWWidth) {
		_GeoPrintTrace(widget," width = %d\n",intended->width);
	    }
	    if (intended->request_mode & CWHeight) {
		_GeoPrintTrace(widget," height = %d\n",intended->height);
	    }
	    if (intended->request_mode & CWBorderWidth) {
		_GeoPrintTrace(widget," border_width = %d\n",intended->border_width);
	    }

	}

	result = (*query) (widget, intended, reply);
    }
    else {
	_GeoPrintTrace(widget,"\"%s\" has no QueryGeometry proc, return the current state\n",XtName(widget));

	result = XtGeometryYes;
    }

#define FillIn(mask, field, str_field) \
	if (!(reply->request_mode & mask)) {\
	      reply->field = widget->core.field;\
	      _GeoPrintTrace(widget," using core %s = %d.\n", str_field,\
			                               widget->core.field);\
	} else {\
	      _GeoPrintTrace(widget," replied %s = %d\n", str_field,\
			                           reply->field);\
	} 

    FillIn(CWX, x, "x");
    FillIn(CWY, y, "y");
    FillIn(CWWidth, width, "width");
    FillIn(CWHeight, height, "height");
    FillIn(CWBorderWidth, border_width, "border_width");

    _GeoUnTabTrace();

#undef FillIn

    if (!reply->request_mode & CWStackMode) reply->stack_mode = XtSMDontChange;

    _GeoPrintTrace(widget,"\"%s\" returns %s to \"%s\".\n",
		     XtName(widget),
		     (result == XtGeometryYes)?"XtGeometryYes":
		     ((result == XtGeometryNo)?"XtGeometryNo":
		      "XtGeometryAlmost"),
		     (XtParent(widget))?XtName(XtParent(widget)):"Root");
		     
    return result;
  }
