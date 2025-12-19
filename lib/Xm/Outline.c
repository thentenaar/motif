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
 *
 */

/************************************************************
*	INCLUDE FILES
*************************************************************/
#include <stdio.h>
#include <Xm/OutlineP.h>
#include <Xm/DropSMgr.h>
#include "XmI.h"

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#define SUPERCLASS (&xmHierarchyClassRec)
#define DEF_INDENT_SPACE 30

#define MOVE_TIME    200

typedef struct {
  int x;
  int y;
} LadderPoint;

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

/************************************************************
*	STATIC FUNCTION DECLARATIONS
*************************************************************/

static void ClassInitialize();
static void Resize(Widget), ClassPartInitialize(WidgetClass);
static void Redisplay(Widget, XEvent *, Region);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget widget);
static void ConstraintInitialize(Widget, Widget, ArgList, Cardinal *);
static void ConstraintDestroy(Widget);
static void ToggleNodeState(Widget, XtPointer, XtPointer);

static XtGeometryResult QueryGeometry(Widget,
				      XtWidgetGeometry *, XtWidgetGeometry *);

static Boolean ConstraintSetValues(Widget, Widget, Widget, ArgList, Cardinal*);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal*);

static XtGeometryResult GeometryManager(Widget,
				      XtWidgetGeometry *, XtWidgetGeometry *);

/************************
 * Actions and callbacks.
 ************************/

/*********************
 * Internal Routines.
 *********************/

static Bool CheckExpose(Display *, XEvent *, char *);
static OutlineConstraints GetNodeInfo(Widget);
static int CalcMaxWidth(Widget);
static void ChangeManaged(Widget);
static void RequestNewSize(Widget), CalcLocations(Widget, Boolean);
static void GetNodeDimensions(Widget, OutlineConstraints,
			      Cardinal, Cardinal *);
static void GetNodeHeightAndWidth(Widget, OutlineConstraints,
				  Cardinal, Boolean, Cardinal *);
static void GetDesiredSize(Widget, Dimension*, Dimension*,
			   Dimension *, Dimension *,
			   Boolean, Boolean);
static void LayoutChildren(Widget, Widget);
static Boolean LocInRect(XRectangle *, Widget, Position, Position);
static Boolean WidgetInRect(XRectangle *, Widget);
static Boolean CheckWidget(XRectangle *, OutlineConstraints);
static void ProcessChildQueue(XmOutlineWidget, XRectangle *);
static void MoveNode(XmOutlineWidget, OutlineConstraints, Position,
		     Position, Position, Position, Boolean);
static void ProcessNode(OutlineConstraints);
static Boolean MoveNodesTimer(XtPointer);
static void UnmapNode(XmOutlineWidget ow, OutlineConstraints node);
static void UnmapAllExtraNodes(Widget w, HierarchyConstraints node);
static void NegotiateNodeWidth(Widget w, OutlineConstraints node);

static void RedrawOutlineLines(Widget, XRectangle *);
static void DrawOutlineLine(Widget, XRectangle *, OutlineConstraints);
static void _CalcNodeMidPoint( OutlineConstraints node, Widget w, LadderPoint *ret_point );
static void _OutlineDrawLine(Widget w, XRectangle *rect, OutlineConstraints parent,
	  OutlineConstraints child, LadderPoint from_ladder_point,
	  LadderPoint *to_ladder_point );
static void CreateGC(XmOutlineWidget ow);

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/
static XtResource resources[] =
{
  {
    XmNindentSpace, XmCIndentSpace, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmOutlineRec, outline.indent_space),
    XmRImmediate, (XtPointer)DEF_INDENT_SPACE
  },

  {
    XmNconstrainWidth, XmCConstrainWidth, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmOutlineRec, outline.constrain_width),
    XmRImmediate, (XtPointer)False
  },

  {
    XmNconnectNodes, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmOutlineRec, outline.connect_nodes),
    XmRImmediate, (XtPointer)False
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNindentSpace, sizeof(Dimension),
    XtOffsetOf(XmOutlineRec, outline.indent_space),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

XmOutlineClassRec xmOutlineClassRec = {
  { /* core fields */
    /* superclass		*/	((WidgetClass) SUPERCLASS),
    /* class_name		*/	"XmOutline",
    /* widget_size		*/	sizeof(XmOutlineRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	(XtResource*)resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	XtExposeCompressMultiple,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	XtInheritTranslations,
    /* query_geometry		*/	(XtGeometryHandler) QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
   {		/* composite_class fields */
    /* geometry_manager   */      GeometryManager,
    /* change_managed     */      ChangeManaged,
    /* insert_child       */      XtInheritInsertChild,
    /* delete_child       */      XtInheritDeleteChild,
    /* extension          */      NULL,
   },
   {		/* constraint_class fields */
    /* resource list        */         NULL,
    /* num resources        */         0,
    /* constraint size      */         sizeof(XmOutlineConstraintRec),
    /* init proc            */         ConstraintInitialize,
    /* destroy proc         */         ConstraintDestroy,
    /* set values proc      */         ConstraintSetValues,
    /* extension            */         NULL,
   },
   {		/* manager_class fields */
    /* default translations   */      XtInheritTranslations,
    /* syn_resources          */      get_resources,
    /* num_syn_resources      */      XtNumber(get_resources),
    /* syn_cont_resources     */      NULL,
    /* num_syn_cont_resources */      0,
    /* parent_process         */      NULL,
    /* extension	      */      NULL,
   },
   { /* Hierarchy fields */
      XtInheritChangeNodeState,	/* The function for changing the node state. */
      XtInheritMapNode,			/* Maps a given node. */
      XtInheritUnmapNode,		/* Unmaps a given node. */
      UnmapAllExtraNodes,		/* Unmaps all extra nodes. */
      XtInheritBuildNodeTable,		/* Builds up the node table. */
      XtInheritResetOpenCloseButton, 	/* Resets the the o/c button. */
      ToggleNodeState,		/* Called to toggle the state of node. */
      NULL,			/* extension          */
   },
   { /* Outline fields */
      CalcMaxWidth,		/* calculates the max width of the outline. */
      CalcLocations,		/* calculates were children will be placed. */
      NULL                      /* extension    */
   }
};

WidgetClass xmOutlineWidgetClass = (WidgetClass) &xmOutlineClassRec;

/************************************************************
*	STATIC CODE
*************************************************************/

/*      Function Name: ClassInitialize
 *      Description:   Initializes class-specific data (offsets)
 *      Arguments:     none
 *      Returns:       nothing
 */
static void
ClassInitialize()
{
  /* do nothing */
}

/*	Function Name: ClassPartInitialize
 *	Description: handles inheritance of class functions.
 *	Arguments: class - the widget class of this widget.
 *	Returns: none.
 */

static void
ClassPartInitialize(WidgetClass class)
{
    XmOutlineWidgetClass wc = (XmOutlineWidgetClass) class;
    XmOutlineWidgetClass superC;

    _XmProcessLock();

    superC = (XmOutlineWidgetClass) wc->core_class.superclass;

/*
 * We don't need to check for NULL super since we'll get to The functions
 * defined by the Outline class eventually.
 */

    if (wc->outline_class.calc_max_width == XtInheritCalcMaxWidth)
	wc->outline_class.calc_max_width= superC->outline_class.calc_max_width;

    if (wc->outline_class.calc_locations == XtInheritCalcLocations)
	wc->outline_class.calc_locations= superC->outline_class.calc_locations;

    _XmProcessUnlock();
}

/*	Function Name: Initialize
 *	Description:   Called to initialize information specific
 *                     to this widget.
 *	Arguments:     req - what was originally requested.
 *                     set - what will be created (our superclassed have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */

/*ARGSUSED*/
static void
Initialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmOutlineWidget ow = (XmOutlineWidget) set;

    XmHierarchy_top_node(ow) = ((HierarchyConstraints)
			      XtRealloc((XtPointer) XmHierarchy_top_node(ow),
					sizeof(OutlineConstraintRec)));

    XmOutline_top_node_of_display(ow) = NULL;

    XmOutline_max_width(ow) = 0;
    XmOutline_max_widget_width(ow) = 0;
    XmOutline_ul_point(ow).x = ow->core.width;
    XmOutline_ul_point(ow).y = ow->core.height;
    XmOutline_lr_point(ow).x = 0;
    XmOutline_lr_point(ow).y = 0;

    XmOutline_child_op_list(ow) = _XmListInit();

    CreateGC(ow);
}

static void CreateGC(XmOutlineWidget ow)
{
    	XGCValues values;
	values.foreground = ow->manager.foreground;
	XmOutline_draw_gc(ow) = XtGetGC((Widget)ow, GCForeground, &values);
}

/*    Function Name: Destroy
 *    Description:   Destroys all data allocated by the widget
 *    Arguments:     w - the widget.
 *    Returns:       none.
 */
static void
Destroy(Widget widget)
{
  XmOutlineWidget ow = (XmOutlineWidget) widget;

  _XmListFree(XmOutline_child_op_list(ow));
  XtReleaseGC(widget,XmOutline_draw_gc(ow));
}

/*	Function Name: Resize
 *	Description:   Called when this widget has been resized.
 *	Arguments:     w - Extended List Widget to resize.
 *	Returns:       none.
 */

static void
Resize(Widget w)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;

    if (XmHierarchy_refigure_mode(ow)) {
	if(XmOutline_constrain_width(ow))
	    CalcLocations(w, False);

	LayoutChildren(w, NULL);
	if (XtIsRealized((Widget)ow)) {
	    XClearArea(XtDisplay(ow), XtWindow(ow),
		       0, 0, ow->core.width, ow->core.height, True);
	}
    }
}

/*	Function Name: Redisplay
 *	Description:   This function deals with any pending moves/maps/unmaps
 *                     in the queue before redisplay
 *	Arguments:     w - the Outline widget.
 *                     event - event that caused the exposure.
 *                     region - the region containing all the exposures.
 *	Returns:       none
 */

typedef struct _RedispInfo {
    Window window;
    Boolean found;
} RedispInfo;

/* ARGSUSED */
static void
Redisplay(Widget w, XEvent * event, Region region)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;
    XEvent junk;
    RedispInfo info;
    int lrx, lry;		/* local variables for lower left corner. */


    XmDropSiteStartUpdate(w);

    /*
     * Make sure that there are not more expose events pending in the queue
     */

    info.window = XtWindow(w);
    info.found = False;
    XCheckIfEvent(XtDisplay(w), &junk, CheckExpose, (char *) &info);

    /*
     * Compute the maximum bounding rectangle for all expose events
     * that have yet to be processed.
     */

    if (event->xexpose.x < XmOutline_ul_point(ow).x)
	XmOutline_ul_point(ow).x = event->xexpose.x;
    if (event->xexpose.y < XmOutline_ul_point(ow).y)
	XmOutline_ul_point(ow).y = event->xexpose.y;

    lrx = event->xexpose.x + event->xexpose.width;
    lry = event->xexpose.y + event->xexpose.height;

    if (lrx > XmOutline_lr_point(ow).x)
	XmOutline_lr_point(ow).x = lrx;
    if (lry > XmOutline_lr_point(ow).y)
	XmOutline_lr_point(ow).y = lry;

    if (!info.found) {	/* No more expose events waiting - process these. */
	XRectangle rect;

	rect.x = XmOutline_ul_point(ow).x;
	rect.y = XmOutline_ul_point(ow).y;
	rect.width = XmOutline_lr_point(ow).x - XmOutline_ul_point(ow).x;
	rect.height = XmOutline_lr_point(ow).y - XmOutline_ul_point(ow).y;

	ProcessChildQueue((XmOutlineWidget) w, &rect);
	if (XmOutline_connect_nodes(w))
		RedrawOutlineLines(w,&rect);

	/*
	 * Reset upper right and lower left points.
	 */

	XmOutline_ul_point(ow).x = w->core.width;
	XmOutline_ul_point(ow).y = w->core.height;
	XmOutline_lr_point(ow).x = XmOutline_lr_point(ow).y = 0;
    }


    XmDropSiteEndUpdate(w);

}

/*    Function Name: CheckExpose
 *    Description: Checks to see if there is an expose event on the queue.
 *    Arguments: disp - the X Display.
 *                 event - the event to check for.
 *                 info_ptr - a pointer to the redispInfo.
 *    Returns: Always False.
 */

/* ARGSUSED */
static Bool
CheckExpose(Display *disp, XEvent *event, char *info_ptr)
{
    RedispInfo *info = (RedispInfo *) info_ptr;

    if (info->found || event->xany.type != Expose)
      return(False);

    if (event->xexpose.window == info->window)
      info->found = True;

    return(False);
}

/*	Function Name: QueryGeometry
 *	Description:   Called when my parent wants to know what size
 *                     I would like to be.
 *	Arguments:     w - the drt widget.
 *                     indended - constriants imposed by the parent.
 *                     preferred - what I would like.
 *	Returns:       See Xt Manual.
 */

static XtGeometryResult
QueryGeometry(Widget w,XtWidgetGeometry *intended, XtWidgetGeometry *preferred)
{
    Dimension *width_intended = NULL;
    Dimension *height_intended = NULL;

    if(!(intended->request_mode & XtCWQueryOnly)) {
	if(intended->request_mode & CWWidth)
	    width_intended = &(intended->width);
	if(intended->request_mode & CWHeight)
	    height_intended = &(intended->height);
    }

    GetDesiredSize(w, width_intended, height_intended,
		   &(preferred->width), &(preferred->height), True, False);
    preferred->request_mode = CWWidth | CWHeight;

    return(_XmHWQuery(w, intended, preferred));
}

/*	Function Name: SetValues
 *	Description:   Called a resources is changed.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

/* ARGSUSED */
static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    XmOutlineWidget c_outline = (XmOutlineWidget) current;
    XmOutlineWidget outline = (XmOutlineWidget) set;
    Boolean layout = False;
    Boolean retval = False;

    if ((XmHierarchy_v_margin(c_outline) != XmHierarchy_v_margin(outline)) ||
	(XmHierarchy_h_margin(c_outline) != XmHierarchy_h_margin(outline)))
    {
	layout = True;
    }

    if (XmOutline_indent_space(c_outline) != XmOutline_indent_space(outline))
    {
	layout = True;
    }

    if (XmOutline_constrain_width(c_outline)
	!= XmOutline_constrain_width(outline))
    {
	layout = True;
    }

    if (XmHierarchy_refigure_mode(c_outline) != XmHierarchy_refigure_mode(outline))
	layout = XmHierarchy_refigure_mode(outline);

    if (XmOutline_connect_nodes(c_outline) != XmOutline_connect_nodes(outline))
	retval = True;

    if (layout) {
	XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(set);
	XmOutlineCalcLocationProc calc_locations;

        _XmProcessLock();
	calc_locations = oc->outline_class.calc_locations;
        _XmProcessUnlock();
	(*calc_locations)(set, True);

	LayoutChildren(set, NULL);
	retval = True;
    }

    if (c_outline->manager.foreground != outline->manager.foreground)
	{
  	XtReleaseGC(current,XmOutline_draw_gc(c_outline));
	CreateGC(outline);
	retval = True;
	}

    return(retval);
}

/************************************************************
 *
 *  Composite Widget class procedures.
 *
 ************************************************************/

/*	Function Name: ChangeManaged
 *	Description:   When a management change has occured...
 *	Arguments:     w - the icon box widget.
 *	Returns:       none.
 */

static void
ChangeManaged(Widget w)
{
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(w);
    XmOutlineWidget ow = (XmOutlineWidget) w;

    if (XmHierarchy_refigure_mode(ow)) {
	XmOutlineCalcLocationProc calc_locations;

	_XmProcessLock();
	calc_locations = oc->outline_class.calc_locations;
	_XmProcessUnlock();
	(*calc_locations)(w, True);

	LayoutChildren(w, NULL);
	if (XtIsRealized((Widget)ow)) {
	    XClearArea(XtDisplay(ow), XtWindow(ow),
		       0, 0, ow->core.width, ow->core.height, True);
	}
    }

   XmeNavigChangeManaged(w);	/* for Motif navigation */
}

/*	Function Name: GeometryManager
 *	Description:   handles requests from children for a size change.
 *	Arguments:     child - the child to change.
 *                     request - the geometry that the child wants.
 *                     return - what we will allow if this is an almost.
 *	Returns:       status.
 */

/*ARGSUSED*/
static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry * request,
		XtWidgetGeometry * result)
{
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(XtParent(w));
    XmOutlineWidget ow = (XmOutlineWidget)(XtParent(w));

    result->request_mode = 0;

    /* if nothing we're interested in, say No */
    if (!(request->request_mode & (CWWidth | CWHeight | CWBorderWidth)))
	return(XtGeometryNo);

    if((XmOutline_constrain_width(ow)) &&
       (request->request_mode & CWWidth) &&
       (request->width > w->core.width))
    {
	result->request_mode = CWWidth;
	result->width = w->core.width;
	return XtGeometryAlmost;
    }

    /* else we have something that we're interested in. Do we also have
    ** something that we're not granting? If so, say that we'll grant the
    ** stuff that we're interested in but not the other items
    */
    if (request->request_mode & (CWX | CWY | CWStackMode | CWSibling))
	{
	result->request_mode = request->request_mode & (CWWidth | CWHeight | CWBorderWidth);
	result->width = request->width;
	result->height = request->height;
	result->border_width = request->border_width;
	return(XtGeometryAlmost);
	}

    if (!(request->request_mode & CWWidth)) {
	request->width = w->core.width;
	result->request_mode |= CWWidth;
    }

    if (!(request->request_mode & CWBorderWidth)) {
	request->border_width = w->core.border_width;
	result->request_mode |= CWBorderWidth;
    }

    if (!(request->request_mode & CWHeight)) {
	request->height = w->core.height;
	result->request_mode |= CWHeight;
    }

    *result = *request;

    if (request->request_mode & XtCWQueryOnly)
	return(XtGeometryYes);

    /*
     * Correct for stupid children
     */
    if(result->width  <= 0) result->width  = 1;
    if(result->height <= 0) result->height = 1;

    /*
     * A real allowed request, make the change.
     */
    _XmResizeWidget(w, result->width, result->height, result->border_width);

    if (XmHierarchy_refigure_mode(ow)) {
	XmOutlineCalcLocationProc calc_locations;

	_XmProcessLock();
	calc_locations = oc->outline_class.calc_locations;
	_XmProcessUnlock();
	(*calc_locations)(XtParent(w), True);

	LayoutChildren(XtParent(w), w);
	if (XtIsRealized((Widget)ow)) {
	    XClearArea(XtDisplay(ow), XtWindow(ow),
		       0, 0, ow->core.width, ow->core.height, True);
	}
    }

    return(XtGeometryYes);
}

/************************************************************
 *
 * Constraint widget class procedures.
 *
 ************************************************************/

/*	Function Name: ConstraintInitialize
 *	Description:   Called when a childs constriaints need initializing.
 *	Arguments:     req - the widget being requested.
 *                     set - what this will become.
 *                     args, num_args - the argument list.
 *	Returns:       none.
 */

/*ARGSUSED*/
static void
ConstraintInitialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmOutlineC_widget_x(set->core.constraints)
      = XmOutlineC_open_close_x(set->core.constraints) = 0;
}

/*	Function Name: ConstraintSetValues
 *	Description:   Called a constraint is changed on my children.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

/*ARGSUSED*/
static Boolean
ConstraintSetValues(Widget current, Widget request, Widget set,
		    ArgList args, Cardinal * num_args)
{
    Widget ow = XtParent(set);
    OutlineConstraints set_node = GetNodeInfo(set);
    OutlineConstraints old_node = GetNodeInfo(current);
    Boolean insert_change = False, redisplay = False;
    int i;

    for (i = 0; i < *num_args; i++)
	if (streq(args[i].name, XmNinsertBefore)) {
	    insert_change = True;
	    break;
	}

    /*
     * Nothing below here needs to be done before we are realized.
     */

    if (!XtIsRealized(set))
	return(False);

    if ((XmHierarchyC_parent(set_node) != XmHierarchyC_parent(old_node)) ||
	(XmHierarchyC_state(set_node) != XmHierarchyC_state(old_node)) ||
	(insert_change))
    {
	/*
	 * Other operations have already been performed by my superclass.
	 */

	if (XmHierarchy_refigure_mode((XmOutlineWidget)ow)) {
	    CalcLocations(ow, True);
	    LayoutChildren(ow, NULL);
	    redisplay = True;
	}

	 /*
	  * Since Layout children has (possibily) moved this widget
	  * to a new location.  The current state of the widget
	  * must be updated so that neither the intrinsics nor
	  * any sub-classes of this widget attempt to move it
	  * to the new location, since it is already there.
	  */

	 current->core.x = set->core.x;
	 current->core.y = set->core.y;
     }

    if (XtIsRealized(ow) && redisplay &&
        (XmHierarchy_refigure_mode((XmOutlineWidget)ow)))
    {
	XClearArea(XtDisplay(ow), XtWindow(ow),
		   0, 0, ow->core.width, ow->core.height, True);
    }

    return(False);
}

/*	Function Name: ConstraintDestroy
 *	Description:   Destroys all data allocated by the constraint
 *                     record.
 *	Arguments:     w - the widget.
 *	Returns:       none.
 *
 * Removes the destroyed children from the list of children that still
 * need to be moved.
 */

static void
ConstraintDestroy(Widget w)
{
    OutlineConstraints node = GetNodeInfo(w);
    XmListElem *elem, *next;
    XmOutlineWidget ow;

    if (XmHierarchyC_state(node) == XmNotInHierarchy)
	return;

    ow = (XmOutlineWidget) XtParent(w);

    elem = XmListFirst(XmOutline_child_op_list(ow));
    while(elem != NULL) {
	OutlineConstraints info = (OutlineConstraints) XmListElemData(elem);

	next = XmListElemNext(elem);

	if (XmHierarchyC_widget(info) == w) {
	    /*
	     * Each widget will only be in the list once.
	     */

	    _XmListRemove(XmOutline_child_op_list(ow), elem);
	    break;
	}

	elem = next;
    }
}

/************************************************************
 *
 *  Hierarchy Widget class proceedures.
 *
 ************************************************************/

/*	Function Name: ToggleNodeState
 *	Description:   Toggles the open/close state of a toggle button.
 *	Arguments:     w - the command button that activated this.
 *                     node_ptr - a pointer to the node info.
 *                     call_data - UNUSED.
 *	Returns:       none.
 */

/*ARGSUSED*/
static void
ToggleNodeState(Widget w, XtPointer node_ptr, XtPointer call_data)
{
    Widget ow = XtParent(w);
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(ow);

    {
	XtCallbackProc toggle_node_state;

	_XmProcessLock();
	toggle_node_state = SUPERCLASS->hierarchy_class.toggle_node_state;
	_XmProcessUnlock();
	(*toggle_node_state)(w, node_ptr, call_data);
    }

    {
	XmOutlineCalcLocationProc calc_locations;

	_XmProcessLock();
	calc_locations = oc->outline_class.calc_locations;
	_XmProcessUnlock();
	(*calc_locations)(ow, True);
    }
    LayoutChildren(ow, NULL);

    /*
     * Could check for a size change and abort if we really wanted
     * to be clever.
     */

    if (XtIsRealized(ow)) {
	XClearArea(XtDisplay(ow), XtWindow(ow),
		   0, 0, ow->core.width, ow->core.height, True);
    }
}

/************************************************************
 *
 *  Outline widget class proceedures.
 *
 ************************************************************/

/************************************************************
 *
 * Actions and Callbacks.
 *
 ************************************************************/

/************************************************************
 *
 * Internal routines.
 *
 ************************************************************/

/*	Function Name: CalcLocations
 *	Description:   Calculates the location of each widget in the outline.
 *	Arguments:     w - the outline widget.
 *                     allow_resize - allow shell to resize?
 *	Returns:       none.
 */

static void
CalcLocations(Widget w, Boolean allow_resize)
{
    Cardinal             outline_depth, current_index;
    XmOutlineWidget      ow = (XmOutlineWidget) w;
    OutlineConstraints   node;
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(w);
    register int         i;
    unsigned int         num_nodes;

    if (!XmHierarchy_refigure_mode(ow))
	return;

    XmOutline_max_widget_width(ow) = 0;	/* reset max_width. */

    /*
     * Reset each node to be hidden;
     */

    for( i = 0; i < ow->composite.num_children; ++i )
    {
	node = GetNodeInfo(ow->composite.children[i]);
	XmHierarchyC_status(node) |= IS_COMPRESSED;
    }

    num_nodes = 0;
    outline_depth = 0;

    GetNodeDimensions(w, (OutlineConstraints) XmHierarchy_top_node(ow),
		      outline_depth, &num_nodes);

    {
	XmOutlineMaxWidthProc calc_max_width;

	_XmProcessLock();
	calc_max_width = oc->outline_class.calc_max_width;
	_XmProcessUnlock();
	XmOutline_max_width(ow) = (*calc_max_width)(w);
    }

    XmHierarchy_num_nodes(ow) = num_nodes;

    current_index = 0;
    {
	XmHierarchyBuildTableProc build_node_table;

	_XmProcessLock();
	build_node_table = oc->hierarchy_class.build_node_table;
	_XmProcessUnlock();
	(*build_node_table)(w, XmHierarchy_top_node(ow), &current_index);
    }

    if (num_nodes != 0) {
	XmOutline_top_node_of_display(ow) =
	                     (OutlineConstraints) XmHierarchy_node_table(ow)[0];
    }
    else
	XmOutline_top_node_of_display(ow) = NULL;

    if (allow_resize)
	RequestNewSize(w);
}

/*	Function Name:	GetNodeDimensions
	Description:	Calls GetNodeHeightAndWidth, but first resets maximum
	Arguments:	w - outline widget
			node - the starting node
			depth - outline depth
			num_nodes - like num_nuts... hahaha
	Returns:	none

 */
static void
GetNodeDimensions(Widget w, OutlineConstraints node,
		  Cardinal depth, Cardinal *num)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;

    XmOutline_max_widget_width(ow) = 2 * XmHierarchy_h_margin(ow);

    GetNodeHeightAndWidth(w, node, depth, TRUE, num);
}

/*	Function Name: RequestNewSize
 *	Description:   Asks our parent for a new size.
 *	Arguments:     w - the data request outline widget.
 *	Returns:       none.
 */

static void
RequestNewSize(Widget w)
{
    Dimension width, height, rwidth, rheight;
    XtGeometryResult ret_val;

    GetDesiredSize(w, NULL, NULL, &width, &height, False, True);

    /*
     * It is just barely possible that our height or width could go to
     * zero.  If this happens then do no request a new size because the
     * X server will generate a protocol error.  It is best to just
     * stay where we are.
     */

    if ((width == 0) || (height == 0))
	return;

    ret_val = XtMakeResizeRequest(w, width, height, &rwidth, &rheight);

    while (ret_val == XtGeometryAlmost) {
	Dimension fwidth, fheight;  /* Final values */
	if(XmOutline_constrain_width(w)) {
	    GetDesiredSize(w, &rwidth, &rheight, &fwidth, &fheight,
			   False, True);
	    ret_val = XtMakeResizeRequest(w, fwidth, fheight,
					  &rwidth, &rheight);
	}
	else {
	    ret_val = XtMakeResizeRequest(w, rwidth, rheight, NULL, NULL);
	}
    }
}

/*	Function Name: GetDesiredSize
 *	Description:   Returns the desired size of the outline widget.
 *	Arguments:     w - the outline widget.
 *                     width - the intended width
 *                     height - the intended height
 *                     width_ret - the desired width.
 *                     height_ret - the desired height.
 *                     recalc - recalculate the new size if row height unset?
 *                     allow_resize - allow the outline to attempt a resize?
 *	Returns:       none.
 */

static void
GetDesiredSize(Widget w, Dimension *width, Dimension *height,
	       Dimension *width_ret, Dimension *height_ret,
	       Boolean recalc, Boolean allow_resize)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;
    register int i, num, temp_height;
    OutlineConstraints * node;
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(w);

    if (recalc) {
	if(XmOutline_constrain_width(w)) {
	    Dimension tmp_width, tmp_height;
	    tmp_height = w->core.height;
	    tmp_width = w->core.width;

	    if(width)
		w->core.width = *width;
	    if(height)
		w->core.height = *height;

	    {
		XmOutlineCalcLocationProc calc_locations;

		_XmProcessLock();
		calc_locations = oc->outline_class.calc_locations;
		_XmProcessUnlock();
		(*calc_locations)(w, allow_resize);
	    }

	    if(width)
		w->core.width = tmp_width;
	    if(height)
		w->core.height = tmp_height;
	} else {
	    {
		XmOutlineCalcLocationProc calc_locations;

		_XmProcessLock();
		calc_locations = oc->outline_class.calc_locations;
		_XmProcessUnlock();
		(*calc_locations)(w, allow_resize);
	    }
	}
    }

    if(XmOutline_constrain_width(ow) && width) {
	*width_ret = *width;
    } else {
	*width_ret = XmOutline_max_width(ow);
    }
    temp_height = 0;
    node = (OutlineConstraints *) XmHierarchy_node_table(ow);
    num = XmHierarchy_num_nodes(ow);
    for (i = 0; i < num; i++, node++)
	temp_height += XmOutlineC_height(*node) + XmHierarchy_v_margin(ow);

    *height_ret = temp_height + XmHierarchy_v_margin(ow);
}

/*	Function Name: LayoutChildren
 *	Description:   all the fun stuff for positioning the children is here.
 *	Arguments:     w - the outline widget.
 *                     assign_child - The child to assign values to rather
 *                                    than _XiMove.
 *	Returns:       none.
 */

static void
LayoutChildren(Widget w, Widget assign_child)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;
    XmOutlineWidgetClass oc = (XmOutlineWidgetClass) XtClass(w);
    register OutlineConstraints disp_top = XmOutline_top_node_of_display(ow);
    register HierarchyConstraints * node_table = XmHierarchy_node_table(ow);
    register Cardinal num_nodes = XmHierarchy_num_nodes(ow);
    register Position cur_y;
    register int cur_node, v_margin;
    int oc_x = 0, oc_y = 0;
    Boolean register_workproc = True;
    Widget _w;

    if (!XmHierarchy_refigure_mode(ow))
	return;

    XmDropSiteStartUpdate(w);


    /*
     * Remove the old list, replace it with the new one.
     */

    if (XmListFirst(XmOutline_child_op_list(ow)) != NULL) {
	if( XmHierarchy_work_proc_id(ow) != (XtWorkProcId) NULL )
	{
	    XtRemoveWorkProc(XmHierarchy_work_proc_id(ow));
	    XmHierarchy_work_proc_id(ow) = (XtWorkProcId) NULL;
	}
	_XmListFree(XmOutline_child_op_list(ow));
	XmOutline_child_op_list(ow) = _XmListInit();
	register_workproc = False;
    }

    /*
     * Unmap all nodes that no longer are visible.
     */

    {
	XmHierarchyExtraNodeProc unmap_all_extra_nodes;
	_XmProcessLock();
	unmap_all_extra_nodes = oc->hierarchy_class.unmap_all_extra_nodes;
	_XmProcessUnlock();
	(*unmap_all_extra_nodes)(w, XmHierarchy_top_node(ow));
    }

    /*
     * Find the first node to be displayed, and unmap all nodes that would be
     * above that one.
     */

    for (cur_node = 0; cur_node < num_nodes; cur_node++, node_table++) {
	if (*node_table == (HierarchyConstraints) disp_top)
	    break;

	UnmapNode(ow, (OutlineConstraints) *node_table);
    }

    cur_y = (Position) (v_margin = XmHierarchy_v_margin(ow));

    while (
	   ((int)cur_node < (int)num_nodes)
	   &&
	   (XmOutline_connect_nodes(ow) || ((int)cur_y < (int)ow->core.height) )
		)
    {
	OutlineConstraints node = (OutlineConstraints) *node_table;

	if (XmHierarchyC_open_close_button(node) != NULL) {
	    Position offset;

	    _w = XmHierarchyC_open_close_button(node);
	    offset = (XmOutlineC_height(node) -
		      (_w->core.height + 2 * _w->core.border_width));

	    oc_x = XmOutlineC_open_close_x(node);
	    oc_y = cur_y + offset/2;
	}

	_w = XmHierarchyC_widget(node);
	if (assign_child == w) {
	    _w->core.x = XmOutlineC_widget_x(node);
	    _w->core.y = cur_y;
	}

	MoveNode(ow, node, XmOutlineC_widget_x(node), cur_y, oc_x, oc_y, True);
	cur_y += XmOutlineC_height(node) + v_margin;
	cur_node++;
	node_table++;
    }

    /*
     * We have used all the window, unmap all other nodes.
     */

    while(cur_node < num_nodes) {
	UnmapNode(ow, (OutlineConstraints) *node_table);

	cur_node++;
	node_table++;
    }

    if (register_workproc) {
	XmHierarchy_work_proc_id(ow) =
	    XtAppAddWorkProc(XtWidgetToApplicationContext(w),
			     MoveNodesTimer, (XtPointer) w);
    }


    XmDropSiteEndUpdate(w);

}

/*	Function Name: GetNodeHeightAndWidth
 *	Description:   Gets the size of each node.
 *	Arguments:     w - the drt widget.
 *                     node - the node to get the height and width of.
 *                     outline_depth - depth of the outline at this node.
 *                     recurse - recurse all subnodes?.
 * IN/OUT              num - number of nodes.
 *	Returns:       none
 */

static void
GetNodeHeightAndWidth(Widget w, OutlineConstraints node,
		      Cardinal outline_depth, Boolean recurse, Cardinal * num)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;
    register int i;
    XtWidgetGeometry geom_pref;

    if ((node == NULL) || ((XmHierarchyC_widget(node) != NULL) &&
			   !XtIsManaged(XmHierarchyC_widget(node))))
    {
	return;
    }

    if (XmHierarchyC_state(node) != XmHidden) {
	Arg args[5];
	Cardinal num_args;
	Dimension width, open_height, node_height, width2, border_width;

	if (XmHierarchyC_open_close_button(node) != NULL) {
	    num_args = 0;
	    XtSetArg(args[num_args], XmNwidth, &width); num_args++;
	    XtSetArg(args[num_args], XmNheight, &open_height); num_args++;
	    XtSetArg(args[num_args], XmNborderWidth, &border_width);num_args++;
	    XtGetValues(XmHierarchyC_open_close_button(node), args, num_args);

	    border_width *= 2;
	    width += border_width;
	    open_height += border_width;
	}
	else {
	    width = 0;
	    open_height = 0;
	}

	num_args = 0;
/* 	XtSetArg(args[num_args], XmNwidth, &width2); num_args++; */
/* 	XtSetArg(args[num_args], XmNheight, &node_height); num_args++; */
	XtSetArg(args[num_args], XmNborderWidth, &border_width); num_args++;
	XtGetValues(XmHierarchyC_widget(node), args, num_args);

	XtQueryGeometry(XmHierarchyC_widget(node), NULL, &geom_pref);
	width2 = geom_pref.width;
	node_height = geom_pref.height;

	border_width *= 2;
	width2 += border_width;
	node_height += border_width;

	/*
	 * indent_level = outline_depth;
	 */

	XmOutlineC_open_close_x(node) = ((outline_depth *XmOutline_indent_space(ow)) +
				      XmHierarchy_h_margin(ow));
	if (XmHierarchyC_open_close_button(node) == NULL)
	    XmOutlineC_widget_x(node) = XmOutlineC_open_close_x(node);
	else
	    XmOutlineC_widget_x(node) = (XmOutlineC_open_close_x(node) +
				      width + XmHierarchy_h_margin(ow));

	width2 += XmOutlineC_widget_x(node) + XmHierarchy_h_margin(ow);
	if ( width2 > XmOutline_max_widget_width(ow) )
	    XmOutline_max_widget_width(ow) = width2;

	/*
	 * Now, if we are set to constrain our childrens' widths, we need
	 * to do a geometry negotiation and reset the width to whatever we
	 * decide.
	 */
	if(XmOutline_constrain_width(w))
	{
	    NegotiateNodeWidth(w, node);
	    num_args = 0;
	    XtSetArg(args[num_args], XmNheight, &node_height); num_args++;
	    XtGetValues(XmHierarchyC_widget(node), args, num_args);
	}

	XmOutlineC_height(node) = MAX(node_height, open_height);

	(*num)++;
    }

    if (!recurse || (XmHierarchyC_state(node) == XmClosed))
	return;

    /*
     * Hidden nodes don't count as another level down the outline.
     */

    if (XmHierarchyC_state(node) != XmHidden)
	outline_depth++;

    for (i = 0; i < XmHierarchyC_num_children(node); i++) {
	GetNodeHeightAndWidth(w,
			      (OutlineConstraints) XmHierarchyC_children(node)[i],
			      outline_depth, TRUE, num);
    }

    if (XmHierarchyC_state(node) != XmHidden)
	outline_depth--;
}

/*	Function Name: GetNodeInfo
 *	Description:   Gets the node info associated with this widget.
 *	Arguments:     w - the widget.
 *	Returns:       the node info about this widget.
 */

static OutlineConstraints
GetNodeInfo(Widget w)
{
    return((OutlineConstraints) w->core.constraints);
}

/*	Function Name: CalcMaxWidth
 *	Description:   Calculates the maximum width of the outline.
 *	Arguments:     w - the outline.
 *	Returns:       The max width the outline would like to be.
 */

static int
CalcMaxWidth(Widget w)
{
    return XmOutline_max_widget_width(w) + 2 * XmHierarchy_h_margin(w);
}

/************************************************************
 *
 *  Code for handling the node queue.
 *
 ************************************************************/

/*	Function Name: UnmapAllExtraNodes
 *	Description:   Correctly unmaps each node in the hierarchy that is
 *                     currently compressed out.
 *	Arguments:     w - the ow.
 *                     node - node to work one.
 *	Returns:       none
 */

static void
UnmapAllExtraNodes(Widget w, HierarchyConstraints node)
{
    register int i, num;
    register HierarchyConstraints * ptr;

    if ((XmHierarchyC_status(node) & IS_COMPRESSED) &&
	(XmHierarchyC_status(node) & IS_MAPPED))
    {
	UnmapNode((XmOutlineWidget) w, (OutlineConstraints) node);
    }

    ptr = XmHierarchyC_children(node);
    for (num = XmHierarchyC_num_children(node), i = 0; i < num; i++, ptr++)
	UnmapAllExtraNodes(w, *ptr);
}

/*	Function Name: MoveNode
 *	Description: This function adds a widget to the movment queue
 *                   but does not move it until it becomes visible.
 *	Arguments: ow - the outline widget.
 *                 node - the node to move.
 *                 x, y - the X and Y location.
 *                 oc_x, oc_y - the X and Y location of the open close button.
 *                 map - map this child?
 *	Returns: none.
 */

static void
MoveNode(XmOutlineWidget ow, OutlineConstraints node, Position x, Position y,
	   Position oc_x, Position oc_y, Boolean map)
{
    XmOutlineC_new_x(node) = x;
    XmOutlineC_new_y(node) = y;
    XmOutlineC_oc_new_x(node) = oc_x;
    XmOutlineC_oc_new_y(node) = oc_y;
    XmOutlineC_map(node) = map;
    XmOutlineC_move(node) = True;
    XmOutlineC_unmap(node) = False;

    _XmListAddBefore(XmOutline_child_op_list(ow), NULL, (XtPointer) node);
}

/*	Function Name: UnmapNode
 *	Description: This function adds a widget to the movment queue
 *                   telling it to be unmapped.
 *	Arguments: ow - the outline widget.
 *                 node - the node to move.
 *	Returns: none.
 */

static void
UnmapNode(XmOutlineWidget ow, OutlineConstraints node)
{
    XmOutlineC_map(node) = False;
    XmOutlineC_move(node) = False;
    XmOutlineC_unmap(node) = True;

    _XmListAddBefore(XmOutline_child_op_list(ow), NULL, (XtPointer) node);
}

/*	Function Name: ProcessChildQueue
 *	Description: Processes the child queue.
 *	Arguments: ow - the outline widget.
 *                 vis - the visible rect.
 *	Returns: none.
 */

static void
ProcessChildQueue(XmOutlineWidget ow, XRectangle *vis)
{
    XmListElem *elem, *next;

    elem = XmListFirst(XmOutline_child_op_list(ow));
    while(elem != NULL) {
	OutlineConstraints info = (OutlineConstraints) XmListElemData(elem);

	next = XmListElemNext(elem);

	if (CheckWidget(vis, info))
	    _XmListRemove(XmOutline_child_op_list(ow), elem);

	elem = next;
    }
}

/*	Function Name: CheckWidget
 *	Description: This function checks a widget to see if it
 *                   needs to be operated on.
 *	Arguments:  visible - visible rect.
 *                  info - the child op info.
 *	Returns: True if operation complete.
 *
 * NOTES: A widget will need to be operated on if it is mapped an will show
 *        on the screen, or if the new location is on the screen.
 */

static Boolean
CheckWidget(XRectangle * visible, OutlineConstraints node)
{
    if (( ((XmHierarchyC_status(node) & IS_MAPPED) || XmOutlineC_map(node))  &&
	( WidgetInRect(visible, XmHierarchyC_open_close_button(node))        ||
	  WidgetInRect(visible, XmHierarchyC_parent(node))                   ||
	  WidgetInRect(visible, XmHierarchyC_widget(node)) ) )               ||
	LocInRect(visible, XmHierarchyC_open_close_button(node),
		    XmOutlineC_oc_new_x(node), XmOutlineC_oc_new_y(node))               ||
	LocInRect(visible, XmHierarchyC_widget(node),
		    XmOutlineC_new_x(node), XmOutlineC_new_y(node)) )
    {
	ProcessNode(node);
	return(True);
    }
    return(False);
}

/*	Function Name: ProcessNode
 *	Description: Processes the request made of this outline node.
 *	Arguments: node - the Outline node to process.
 *	Returns: none.
 */

static void
ProcessNode(OutlineConstraints node)
{
    XmOutlineWidgetClass tc;
    Widget w = XmHierarchyC_widget(node);

    if (w == NULL)
	return;

    tc = (XmOutlineWidgetClass) XtClass(XtParent(w));

    if (XmOutlineC_move(node)) {
	_XmMoveWidget(XmHierarchyC_widget(node),
		      XmOutlineC_new_x(node), XmOutlineC_new_y(node));

	if (XmHierarchyC_open_close_button(node) != NULL)
	    _XmMoveWidget(XmHierarchyC_open_close_button(node),
			  XmOutlineC_oc_new_x(node), XmOutlineC_oc_new_y(node));
	XmOutlineC_move(node) = False;
    }

    if (XmOutlineC_map(node)) {
	{
	    XmHierarchyNodeProc map_node;
	    _XmProcessLock();
	    map_node = tc->hierarchy_class.map_node;
	    _XmProcessUnlock();
	    (*map_node)((HierarchyConstraints) node);
	}
	XmOutlineC_map(node) = False;
    }

    if (XmOutlineC_unmap(node)) {
	{
	    XmHierarchyNodeProc	unmap_node;
	    _XmProcessLock();
	    unmap_node = tc->hierarchy_class.unmap_node;
	    _XmProcessUnlock();
	    (*unmap_node)((HierarchyConstraints) node);
	}
	XmOutlineC_unmap(node) = False;
    }
}

/*	Function Name: WidgetInRect
 *	Description: Checks to see if widget is in the rect specified.
 *	Arguments: rect - the rect to check.
 *                 w - the widget to check.
 *	Returns: True if the widget is in the regien specified.
 */

static Boolean
WidgetInRect(XRectangle *rect, Widget w)
{
    if (w == NULL)
	return(False);

    return(LocInRect(rect, w, w->core.x, w->core.y));
}

/*	Function Name: LocInRect
 *	Description: Checks to see if widget is in the rect specified.
 *                   given new X and Y locations.
 *	Arguments: rect - the rectangle to check.
 *                 w - the widget to check.
 *                 x, y - the new x and y locations.
 *	Returns: True if the widget is in the regien specified.
 */

static Boolean
LocInRect(XRectangle *rect, Widget w, Position x, Position y)
{
    register int x1, x2;
    register int y1, y2;

    if (w == NULL) return(False);

    x1 = x + w->core.width;
    y1 = y + w->core.height;

    x2 = rect->x + rect->width;
    y2 = rect->y + rect->height;

    return (!((x > x2) || (y > y2) || (x1 < rect->x) || (y1 < rect->y)));
}

/*	Function Name: MoveNodesTimer
 *	Description: processes one node from the movement queue.
 *	Arguments: data - A Pointer to the outline widget.
 *                 id - th ext interval id.
 *	Returns: none
 */

/* ARGSUSED */
static Boolean
MoveNodesTimer(XtPointer data)
{
    XmOutlineWidget ow = (XmOutlineWidget) data;
    XmListElem *elem = XmListFirst(XmOutline_child_op_list(ow));

    if (elem != NULL) {
	OutlineConstraints node = (OutlineConstraints) XmListElemData(elem);
	ProcessNode(node);
	_XmListRemove(XmOutline_child_op_list(ow), elem);
	return (False);
    }
    XmHierarchy_work_proc_id(ow) = (XtWorkProcId) NULL;
    return (True);
}

/*	Function Name: NegotiateNodeWidth
 *	Description:   Does a geometry negoation with the child
 *                     specified by node with the intent of fitting
 *                     the child within the existing width of the
 *                     Outline widget.
 *	Arguments:     w    - The XmOutline widget
 *                     node - The constraint record for the child in
 *                            question
 *	Returns:       The width negotiated
 */
static void
NegotiateNodeWidth(Widget w, OutlineConstraints node)
{
    Dimension width_avail;
    XtWidgetGeometry geom_asked, geom_agreed;
    XtGeometryResult result;
    Dimension curr_width, curr_height;

    width_avail = w->core.width - XmOutlineC_widget_x(node);

    curr_width = XmHierarchyC_widget(node)->core.width;
    curr_height = XmHierarchyC_widget(node)->core.height;

    geom_asked.request_mode = XtCWQueryOnly;
    result = XtQueryGeometry(XmHierarchyC_widget(node),
			     &geom_asked, &geom_agreed);

    if(geom_agreed.width  < 1) geom_agreed.width  = 1;
    if(geom_agreed.height < 1) geom_agreed.height = 1;

    if(geom_agreed.width <= width_avail) {
	/*
	 * If it's the right size already, just return
	 */
	if(curr_width  == geom_agreed.width &&
	   curr_height == geom_agreed.height)
	{
	    return;
	}
	else
	{
	    _XmResizeWidget(XmHierarchyC_widget(node),
			    geom_agreed.width,
			    geom_agreed.height,
			    geom_agreed.border_width);
	    return;
	}
    }

    /*
     * Otherwise tell the widget to size itself to the right width and
     * see what we get
     */
    geom_asked.request_mode = CWWidth;
    geom_asked.width        = width_avail;
    result = XtQueryGeometry(XmHierarchyC_widget(node),
			     &geom_asked, &geom_agreed);

    if(geom_agreed.width  < 1) geom_agreed.width  = 1;
    if(geom_agreed.height < 1) geom_agreed.height = 1;

    if(result == XtGeometryYes)
    {
	_XmResizeWidget(XmHierarchyC_widget(node),
			width_avail,
			geom_agreed.height,
			geom_agreed.border_width);
	return;
    }
    else if(result == XtGeometryAlmost)
    {
	/*
	 * See if it lied and set the width correctly anyway
	 */
	if(geom_agreed.width <= width_avail) {
	    _XmResizeWidget(XmHierarchyC_widget(node),
			    geom_agreed.width,
			    geom_agreed.height,
			    geom_agreed.border_width);
	    return;
	}
    }
    /*
     * If we get here, the sucker tried to deny the request, so just
     * clip the thing.  Thbbbt!
     */
    _XmResizeWidget(XmHierarchyC_widget(node),
		    width_avail,
		    geom_agreed.height,
		    geom_agreed.border_width);
}


/* drawing on expose; taken from XmTree in horizontal mode with
** ladder drawing
** Note: it looks not so good when indentSpace is very small
*/

static void
RedrawOutlineLines(Widget w, XRectangle * rect)
{
    XmOutlineWidget ow = (XmOutlineWidget) w;

    if (XtIsRealized(w))	/* && has children */
	DrawOutlineLine(w, rect, (OutlineConstraints) XmHierarchy_top_node(ow));
}

static void
DrawOutlineLine(Widget w, XRectangle *rect, OutlineConstraints node)
{

    OutlineConstraints from_node = node;

    if (XmHierarchyC_widget(node) != NULL && !XtIsManaged(XmHierarchyC_widget(node)))
	return;

    /*
     * Hunt up the list until we find a non-hidden parent to be the from
     * node.
     */

    while ((XmHierarchyC_parent(from_node) != NULL) &&
	   (XmHierarchyC_state(from_node) == XmHidden))
    {
	from_node = GetNodeInfo(XmHierarchyC_parent(from_node));
    }

   {
    OutlineConstraints * kids;
    register int i, num_kids;
    Boolean anyKidManaged = False; /* CR03730 Support Case 22066 */
    LadderPoint from_node_point ={0, 0}, kid_point = {0, 0};
    LadderPoint last_kid_point ={0, 0};
    XmOutlineWidget ow = (XmOutlineWidget)w;

    num_kids = XmHierarchyC_num_children(node);
    kids = (OutlineConstraints *) XmHierarchyC_children(node);

    /*
     * This only executes if the root is hidden.
     */

    if (XmHierarchyC_state(from_node) == XmHidden) {
	for (i = 0; i < num_kids; i++, kids++)
	    DrawOutlineLine(w, rect, *kids);	/* recurse to descendants. */
	return;
    }

    if (XmHierarchyC_state(from_node) == XmClosed)
	return;

    _CalcNodeMidPoint( from_node, w, &from_node_point );

    for (i = 0; i < num_kids; i++, kids++) {
      if (XtIsManaged(XmHierarchyC_widget((*kids)))) {
	anyKidManaged = True; /* CR03730 Support Case 22066 */
	if (XmHierarchyC_state((*kids)) != XmHidden) {
	  _OutlineDrawLine(w, rect, from_node, *kids, from_node_point, &kid_point);

	  /* set up the points to draw ladder lines */

	    last_kid_point.x = kid_point.x;
	    last_kid_point.y = kid_point.y;

	}
	DrawOutlineLine(w, rect, *kids);	/* recurse to descendants. */
      }
    }

    /* Draw extra ladder lines if necessary.
     */
    if (num_kids > 0 && anyKidManaged)
	{ /* CR03730 Support Case 22066 anyKidManaged added to prevent draw needless
	     (x & y are not initialized if no kid managed) line */
          XDrawLine(XtDisplay(w), XtWindow(w), XmOutline_draw_gc(ow), from_node_point.x, from_node_point.y,
		from_node_point.x, last_kid_point.y );
	}
   }
}

static void
_CalcNodeMidPoint( OutlineConstraints node, Widget w, LadderPoint *ret_point )
{
  if (!XmHierarchyC_widget(node)) return;

  {
  XmOutlineWidget ow = (XmOutlineWidget)w;
  Widget which;
  int value;

  if (XmHierarchyC_open_close_button(node))
	value = (int)XtWidth(which=XmHierarchyC_open_close_button(node));
  else
	{
	value = (int)XmOutline_indent_space(ow);
	which = XmHierarchyC_widget(node);
	}

    ret_point->x = XmOutlineC_open_close_x(node) + value/2;
    ret_point->y = XtY(which)+XtHeight(which);	/* plus possible pad */
  }
}

static void
_OutlineDrawLine(Widget w, XRectangle *rect, OutlineConstraints parent,
	  OutlineConstraints child, LadderPoint from_ladder_point,
	  LadderPoint *to_ladder_point )

{
    GC gc;
    XmOutlineWidget ow = (XmOutlineWidget) w;
    register int x2, y2;
    register int rx2, ry2, cx1, cx2, cy1, cy2;

    /*
     * (from_ladder_point.x, from_ladder_point.y) are the coordinates
     *     of the parent's midpoint
     * (x2, y2) are the coordinates of the child midpoint
     *
     * We will end up drawing a partial ladder line from
     *     (cx1, cy1) to (cx2, cy2)
     *
     * (rx2, ry2) are the coordinates of the lower right of clip rectangle
     */

    /*
     * Must always do the first node since it is what draws the root
     * of the ladder.
     */

    if (child != (OutlineConstraints) XmHierarchyC_children(parent)[0] &&
	(!(XmHierarchyC_status(child) & IS_MAPPED) ||
	 (XmHierarchyC_status(child) & IS_COMPRESSED)))
    {
	return;
    }

    gc = XmOutline_draw_gc(ow);

      x2 = XmOutlineC_open_close_x(child);
      y2 = XtY(XmHierarchyC_widget(child)) + XtHeight(XmHierarchyC_widget(child))/2;

    cx1 = MIN(from_ladder_point.x, x2);
    cx2 = MAX(from_ladder_point.x, x2);

    cy1 = MIN(from_ladder_point.y, y2);
    cy2 = MAX(from_ladder_point.y, y2);

    cy1 = cy2 = y2;

    rx2 = rect->x + rect->width;
    ry2 = rect->y + rect->height;

    /*
     * Not very pretty, only checks to see if the rect containing the
     * line is partially within the exposed rectangle.
     */

    if (!((cx1 > rx2) || (cy1 > ry2) || (cx2 < rect->x) || (cy2 < rect->y)))
	    XDrawLine(XtDisplay(w), XtWindow(w), gc, cx1, cy1, cx2, cy2);

    /* This is sent back because the ladder lines get connected later */
    to_ladder_point->x = cx1;
    to_ladder_point->y = cy1;
}

/************************************************************
 *
 *  Public functions.
 *
 ************************************************************/

/*	Function Name: XmCreateOutline
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateOutline(Widget parent, String name,
		ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmOutlineWidgetClass,
			  parent, args, num_args));
}
