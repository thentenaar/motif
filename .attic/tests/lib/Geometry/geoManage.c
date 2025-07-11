/* $XConsortium: geoManage.c /main/4 1995/07/15 21:13:27 drk $ */
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

#include "geoTattler.h"

static String XtNinvalidChild = "invalidChild";
static String XtNxtUnmanageChildren = "xtUnmanageChildren";
static String XtNxtManageChildren = "xtManageChildren";

void XtUnmanageChildren(children, num_children)
    WidgetList children;
    Cardinal num_children;
{
    register CompositeWidget	parent;
    register Widget		child;
    register Cardinal		num_unique_children, i;
	     XtWidgetProc	change_managed;
	     Bool		parent_realized;

    if (num_children == 0) return;
    if (children[0] == NULL) {
	XtWarningMsg(XtNinvalidChild,XtNxtUnmanageChildren,XtCXtToolkitError,
                  "Null child passed to XtUnmanageChildren",
		  (String *)NULL, (Cardinal *)NULL);
	return;
    }
    parent = (CompositeWidget) children[0]->core.parent;
    if (parent->core.being_destroyed) return;

    if (XtIsComposite((Widget) parent)) {
        change_managed = ((CompositeWidgetClass) parent->core.widget_class)
		    ->composite_class.change_managed;
	parent_realized = XtIsRealized((Widget)parent);
    }

    num_unique_children = 0;
    for (i = 0; i < num_children; i++) {
	child = children[i];
	if (child == NULL) {
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		  XtNinvalidChild,XtNxtUnmanageChildren,XtCXtToolkitError,
                  "Null child passed to XtUnmanageChildren",
		  (String *)NULL, (Cardinal *)NULL);
	    return;
	}
        if ((CompositeWidget) child->core.parent != parent) {
	   XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		   "ambiguousParent",XtNxtUnmanageChildren,XtCXtToolkitError,
           "Not all children have same parent in XtUnmanageChildren",
             (String *)NULL, (Cardinal *)NULL);
	} else
        if (child->core.managed) {
            num_unique_children++;
	    _GeoPrintTrace(child,"Child \"%s\" is marked unmanaged\n", XtName(child));
	    child->core.managed = FALSE;
            if (XtIsWidget(child)
		&& XtIsRealized(child)
		&& child->core.mapped_when_managed)
                    XtUnmapWidget(child);
            else
	    { /* RectObj child */
		Widget pw = child->core.parent;
		RectObj r = (RectObj) child;
		while ((pw!=NULL) && (!XtIsWidget(pw))) pw = pw->core.parent;
		if ((pw!=NULL) && XtIsRealized (pw))
		    XClearArea (XtDisplay (pw), XtWindow (pw),
			r->rectangle.x, r->rectangle.y,
			r->rectangle.width + (r->rectangle.border_width << 1),
			r->rectangle.height + (r->rectangle.border_width << 1),
			TRUE);
	    }

        }
    }
    if (num_unique_children != 0 && change_managed != NULL && parent_realized) {
	_GeoPrintTrace((Widget)parent,"Call parent: \"%s\"[%d,%d]'s changemanaged proc\n", XtName((Widget)parent),parent->core.width,parent->core.height);
	(*change_managed) ((Widget)parent);
    }
} /* XtUnmanageChildren */


void XtUnmanageChild(child)
    Widget child;
{
    XtUnmanageChildren(&child, (Cardinal)1);
} /* XtUnmanageChild */


void XtManageChildren(children, num_children)
    WidgetList  children;
    Cardinal    num_children;
{
#define MAXCHILDREN 100
    register CompositeWidget    parent;
    register Widget		child;
    register Cardinal		num_unique_children, i;
	     XtWidgetProc	change_managed;
    register WidgetList		unique_children;
	     Widget		cache[MAXCHILDREN];
	     Bool		parent_realized;

    if (num_children == 0) return;
    if (children[0] == NULL) {
	XtWarningMsg(XtNinvalidChild,XtNxtManageChildren,XtCXtToolkitError,
         "null child passed to XtManageChildren",
	 (String *)NULL, (Cardinal *)NULL);
	return;
    } 
    parent = (CompositeWidget) children[0]->core.parent;
    if (XtIsComposite((Widget) parent)) {
        change_managed = ((CompositeWidgetClass) parent->core.widget_class)
		    ->composite_class.change_managed;
	parent_realized = XtIsRealized((Widget)parent);

    } else {
	XtAppErrorMsg(XtWidgetToApplicationContext((Widget)parent),
		"invalidParent",XtNxtManageChildren, XtCXtToolkitError,
	    "Attempt to manage a child when parent is not Composite",
	    (String *) NULL, (Cardinal *) NULL);
    }
    if (parent->core.being_destroyed) return;

    /* Construct new list of children that really need to be operated upon. */
    if (num_children <= MAXCHILDREN) {
	unique_children = cache;
    } else {
	unique_children = (WidgetList) XtMalloc(num_children * sizeof(Widget));
    }
    num_unique_children = 0;
    for (i = 0; i < num_children; i++) {
	child = children[i];
	if (child == NULL) {
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		XtNinvalidChild,XtNxtManageChildren,XtCXtToolkitError,
		"null child passed to XtManageChildren",
		(String *)NULL, (Cardinal *)NULL);
	    if (unique_children != cache) XtFree((char *) unique_children);
	    return;
	}
#ifdef DEBUG
	if (!XtIsRectObj(child)) {
	    String params[2];
	    Cardinal num_params = 2;
	    params[0] = XtName(child);
	    params[1] = child->core.widget_class->core_class.class_name;
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
			    "notRectObj",XtNxtManageChildren,XtCXtToolkitError,
			    "child \"%s\", class %s is not a RectObj",
			    params, &num_params);
	    continue;
	}
#endif /*DEBUG*/
        if ((CompositeWidget) child->core.parent != parent) {
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		    "ambiguousParent",XtNxtManageChildren,XtCXtToolkitError,
		"Not all children have same parent in XtManageChildren",
		(String *)NULL, (Cardinal *)NULL);
	} else if (! child->core.managed && !child->core.being_destroyed) {
	    unique_children[num_unique_children++] = child;
	    child->core.managed = TRUE;
	    _GeoPrintTrace(child,"Child \"%s\"[%d,%d] is marked managed\n", XtName(child),child->core.width,child->core.height);
    
    	}
    }

    if (num_unique_children != 0 && parent_realized) {
	/* Compute geometry of new managed set of children. */
	
	if (change_managed != NULL) {
	    _GeoPrintTrace((Widget)parent,"Call parent: \"%s\"[%d,%d]'s changemanaged\n", XtName((Widget)parent), parent->core.width,parent->core.height);
	    (*change_managed) ((Widget)parent);
	}

	/* Realize each child if necessary, then map if necessary */
	for (i = 0; i < num_unique_children; i++) {
	    child = unique_children[i];
	    if (XtIsWidget(child)) {
		if (! XtIsRealized(child)) {
		    XtRealizeWidget(child);
		}
		if (child->core.mapped_when_managed) XtMapWidget(child);
	    } else { /* RectObj child */
		Widget pw = child->core.parent;
		RectObj r = (RectObj) child;
		while ((pw!=NULL) && (!XtIsWidget(pw)))
		    pw = pw->core.parent;
		if (pw != NULL)
		    XClearArea (XtDisplay (pw), XtWindow (pw),
		    r->rectangle.x, r->rectangle.y,
		    r->rectangle.width + (r->rectangle.border_width << 1),
		    r->rectangle.height + (r->rectangle.border_width << 1),
		    TRUE);
            }
        }
    }

    if (unique_children != cache) XtFree((char *) unique_children);
} /* XtManageChildren */


void XtManageChild(child)
    Widget child;
{
    XtManageChildren(&child, (Cardinal) 1);
} /* XtManageChild */


#if NeedFunctionPrototypes
void XtSetMappedWhenManaged(
    register Widget widget,
    _XtBoolean	    mapped_when_managed
    )
#else
void XtSetMappedWhenManaged(widget, mapped_when_managed)
    register Widget widget;
    Boolean	    mapped_when_managed;
#endif
{
    if (widget->core.mapped_when_managed == mapped_when_managed) return;
    widget->core.mapped_when_managed = mapped_when_managed;
    if (! XtIsManaged(widget)) return;

    if (mapped_when_managed) {
	/* Didn't used to be mapped when managed.		*/
	if (XtIsRealized(widget)) XtMapWidget(widget);
    } else {
	/* Used to be mapped when managed.			*/
	if (XtIsRealized(widget)) XtUnmapWidget(widget);
    }
} /* XtSetMappedWhenManaged */


