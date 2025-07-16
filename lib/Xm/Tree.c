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
 *
 */

/************************************************************
*	INCLUDE FILES
*************************************************************/

#include <stdio.h>
#include "XmI.h"
#include "xmlist.h"
#include <Xm/TreeP.h>
#include <X11/Xutil.h>

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#define SUPERCLASS (&xmHierarchyClassRec)

#define INDENT_SPACE 30

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

static void Resize(Widget), ClassInit(void);
static void Realize(Widget, Mask *, XSetWindowAttributes *);
static void Redisplay(Widget, XEvent *, Region);
static void ClassPartInitialize(WidgetClass w_class);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ConstraintInitialize(Widget, Widget, ArgList, Cardinal *);
static void ConstraintDestroy(Widget);
static void TreeDestroy(Widget);
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

static Boolean CvtStringToConnectStyle(Display *, XrmValuePtr, Cardinal *,
				       XrmValuePtr, XrmValuePtr);
static Boolean CvtStringToCompressStyle(Display *, XrmValuePtr, Cardinal *,
				       XrmValuePtr, XrmValuePtr);
static Boolean CvtStringToLineStyle(Display *, XrmValuePtr, Cardinal *,
				       XrmValuePtr, XrmValuePtr);

static void ReleaseNodeGCs(Widget), GetNodeGCs(Widget);
static TreeConstraints GetNodeInfo(Widget);
static int GetExtraVertSpace(Widget);
static int GetExtraHorizSpace(Widget);

static void LineColorDefault(Widget, int, XrmValue *);
static void LineBackgroundColorDefault(Widget, int, XrmValue *);
static void HorizontalNodeSpaceDefault(Widget, int, XrmValue *);
static void VerticalNodeSpaceDefault(Widget, int, XrmValue *);

static void ChangeManaged(Widget), CalcMaxSize(Widget);
static void CalcLocations(Widget, Boolean);
static void LayoutChildren(Widget, Widget);
static Boolean GetNodeHeightAndWidth(Widget, TreeConstraints,
				     Cardinal *, Cardinal);
static void GetDesiredSize(Widget, Dimension *, Dimension *, Boolean);

static void _DrawLine(Widget, XRectangle *, TreeConstraints, TreeConstraints,
		      LadderPoint, LadderPoint *);
static void _PlaceNode(Widget, TreeConstraints);
static void _ResetPlacedFlag(TreeConstraints);

static void FindNodeLocations(Widget);
static void DrawTreeLine(Widget, XRectangle *, TreeConstraints);
static void RedrawTreeLines(Widget, XRectangle *);

static Boolean RequestNewSize(Widget);

static Bool CheckExpose(Display *, XEvent *, char *);

static Boolean LocInRect(XRectangle *, Widget, Position, Position);
static Boolean WidgetInRect(XRectangle *, Widget);
static Boolean CheckWidget(XRectangle *, TreeConstraints);
static void ProcessChildQueue(XmTreeWidget, XRectangle *);
static void MoveNode(XmTreeWidget, TreeConstraints, Position, Position,
		     Position, Position, Boolean);
static void ProcessNode(TreeConstraints);
static Boolean MoveNodesTimer(XtPointer);
static void UnmapNode(XmTreeWidget tw, TreeConstraints node);
static void UnmapAllExtraNodes(Widget w, HierarchyConstraints node);

static void LineStyle_confirm (Widget w, int value);

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

static XtResource resources[] =
{
  {
    XmNconnectStyle, XmCConnectStyle, XmRXmConnectStyle,
    sizeof(XmTreeConnectStyle), XtOffsetOf(XmTreeRec, tree.connect_style),
    XmRImmediate, (XtPointer) XmTreeDirect
  },

  {
    XmNorientation, XmCOrientation, XmROrientation,
    sizeof(unsigned char), XtOffsetOf(XmTreeRec, tree.orientation),
    XmRImmediate, (XtPointer) XmHORIZONTAL
  },

  {
    XmNcompressStyle, XmCCompressStyle, XmRXmCompressStyle,
    sizeof(XmTreeCompressStyle), XtOffsetOf(XmTreeRec, tree.compress_style),
    XmRImmediate, (XtPointer) XmTreeCompressLeaves
  },

  {
    XmNverticalDelta, XmCVerticalDelta, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmTreeRec, tree.vertical_delta),
    XmRImmediate, (XtPointer) 30
  },

  {
    XmNhorizontalDelta, XmCHorizontalDelta, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmTreeRec, tree.horizontal_delta),
    XmRImmediate, (XtPointer) 25
  },

  {
    XmNhorizontalNodeSpace, XmCDimension, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmTreeRec, tree.h_node_space),
    XmRCallProc, (XtPointer) HorizontalNodeSpaceDefault
  },

  {
    XmNverticalNodeSpace, XmCDimension, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmTreeRec, tree.v_node_space),
    XmRCallProc, (XtPointer) VerticalNodeSpaceDefault
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNhorizontalNodeSpace, sizeof(Dimension),
    XtOffsetOf(XmTreeRec, tree.h_node_space),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNverticalNodeSpace, sizeof(Dimension),
    XtOffsetOf(XmTreeRec, tree.v_node_space),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNverticalDelta, sizeof(Dimension),
    XtOffsetOf(XmTreeRec, tree.vertical_delta),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNhorizontalDelta, sizeof(Dimension),
    XtOffsetOf(XmTreeRec, tree.horizontal_delta),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

static XtResource constraints[] =
{
  {
    XmNopenClosePadding, XmCOpenClosePadding, XmRInt,
    sizeof(int), XtOffsetOf(XmTreeConstraintRec, tree.open_close_padding),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNlineColor, XmCForeground, XmRPixel,
    sizeof(Pixel), XtOffsetOf(XmTreeConstraintRec, tree.color),
    XmRCallProc, (XtPointer) LineColorDefault
  },

  {
    XmNlineBackgroundColor, XmCBackground, XmRPixel,
    sizeof(Pixel), XtOffsetOf(XmTreeConstraintRec, tree.background_color),
    XmRCallProc, (XtPointer) LineBackgroundColorDefault
  },

  {
    XmNlineWidth, XmCLineWidth, XmRInt,
    sizeof(int), XtOffsetOf(XmTreeConstraintRec, tree.line_width),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNlineStyle, XmCLineStyle, XmRXmLineStyle,
    sizeof(int), XtOffsetOf(XmTreeConstraintRec, tree.line_style),
    XmRImmediate, (XtPointer) LineSolid
  }
};

static XmSyntheticResource get_cons_resources[] =
{
  {
    XmNopenClosePadding, sizeof(int),
    XtOffsetOf(XmTreeConstraintRec, tree.open_close_padding),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

#undef offset

XmTreeClassRec xmTreeClassRec = {
  { /* core fields */
    /* superclass		*/	((WidgetClass) SUPERCLASS),
    /* class_name		*/	"XmTree",
    /* widget_size		*/	sizeof(XmTreeRec),
    /* class_initialize		*/	ClassInit,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	(XtResource*)resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	XtExposeCompressMultiple,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	TreeDestroy,
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
    /* resource list        */         (XtResource*)constraints,
    /* num resources        */         XtNumber(constraints),
    /* constraint size      */         sizeof(XmTreeConstraintRec),
    /* init proc            */         ConstraintInitialize,
    /* destroy proc         */         ConstraintDestroy,
    /* set values proc      */         ConstraintSetValues,
    /* extension            */         NULL,
   },
   {		/* manager_class fields */
    /* default translations   */      XtInheritTranslations,
    /* syn_resources          */      get_resources,
    /* num_syn_resources      */      XtNumber(get_resources),
    /* syn_cont_resources     */      get_cons_resources,
    /* num_syn_cont_resources */      XtNumber(get_cons_resources),
    /* parent_process         */      XmInheritParentProcess,
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
   { /* Tree fields */
      NULL                      /* extension    */
   }
};

WidgetClass xmTreeWidgetClass = (WidgetClass) &xmTreeClassRec;

/************************************************************
*	STATIC CODE
*************************************************************/

/*	Function Name: ClassInit
 *	Description:   Called to initialize information specific
 *                     to this widget class.
 *	Arguments:     none.
 *	Returns:       none.
 */
static void ClassInit(void)
{
    XtSetTypeConverter(XmRString, XmRXmConnectStyle,
		       (XtTypeConverter) CvtStringToConnectStyle,
		       NULL, (Cardinal) 0, XtCacheAll, (XtDestructor) NULL);
    XtSetTypeConverter(XmRString, XmRXmCompressStyle,
		       (XtTypeConverter) CvtStringToCompressStyle,
		       NULL, (Cardinal) 0, XtCacheAll, (XtDestructor) NULL);
    XtSetTypeConverter(XmRString, XmRXmLineStyle,
		       (XtTypeConverter) CvtStringToLineStyle,
		       NULL, (Cardinal) 0, XtCacheNone, (XtDestructor) NULL);
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmTREE_BIT);
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

static void
Initialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmTreeWidget tw = (XmTreeWidget) set;
    TreeConstraints top_node;

    top_node = ((TreeConstraints)
		XtRealloc((XtPointer) XmHierarchy_top_node(tw),
			  sizeof(TreeConstraintRec)));

    XmHierarchy_top_node(tw) = (HierarchyConstraints) top_node;

    XmTree_ul_point(tw).x = XmTree_ul_point(tw).y = 0;
    XmTree_lr_point(tw).x = XmTree_lr_point(tw).y = 0;

    XmTreeC_box_x(top_node) = XmTreeC_box_y(top_node) = 0;
    XmTreeC_bb_width(top_node) = XmTreeC_bb_height(top_node) = 0;
    XmTreeC_placed(top_node) = False;

    XmTree_child_op_list(tw) = _XmListInit();

    /*
     * Set the initial values for XmTreeC_max_width(tw) and XmTreeC_max_height(tw).
     */

    CalcMaxSize(set);
}

/*	Function Name: Realize
 *	Description:   Called to realize this widget.
 *	Arguments:     w - Widget to realize.
 *                     valueMask, attributes - attributes to use when creating
 *                     this widget's window.
 *	Returns:       none.
 *
 * This overrides the Manager's frobbing with various values.
 */

static void
Realize(Widget w, Mask *valueMask, XSetWindowAttributes * attributes)
{
   XtCreateWindow (w, InputOutput, CopyFromParent, *valueMask, attributes);
}

/*	Function Name: Redisplay
 *	Description:   This function redraws the tree lines.
 *	Arguments:     w - the Tree widget.
 *                     event - event that caused the exposure.
 *                     region - the region containing all the exposures.
 *	Returns:       none
 */

typedef struct _RedispInfo {
    Window window;
    Boolean found;
} RedispInfo;

static void
Redisplay(Widget w, XEvent * event, Region region)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    XEvent junk;
    RedispInfo info;
    int lrx, lry;		/* local variables fro lower left corner. */

    XmDropSiteStartUpdate(w);

    /*
     * Make sure that there are not more expose events pending in the queue
     * since only one is required to paint the tree lines.
     */

    info.window = XtWindow(w);
    info.found = False;
    XCheckIfEvent(XtDisplay(w), &junk, CheckExpose, (char *) &info);

    /*
     * Compute the maximum bounding rectangle for all expose events
     * that have yet to be processed.
     */

    if (event->xexpose.x < XmTree_ul_point(tw).x)
	XmTree_ul_point(tw).x = event->xexpose.x;
    if (event->xexpose.y < XmTree_ul_point(tw).y)
	XmTree_ul_point(tw).y = event->xexpose.y;

    lrx = event->xexpose.x + event->xexpose.width;
    lry = event->xexpose.y + event->xexpose.height;

    if (lrx > XmTree_lr_point(tw).x)
	XmTree_lr_point(tw).x = lrx;
    if (lry > XmTree_lr_point(tw).y)
	XmTree_lr_point(tw).y = lry;

    if (!info.found) {	/* No more expose events waiting - process these. */
	XRectangle rect;

	rect.x = XmTree_ul_point(tw).x;
	rect.y = XmTree_ul_point(tw).y;
	rect.width = XmTree_lr_point(tw).x - XmTree_ul_point(tw).x;
	rect.height = XmTree_lr_point(tw).y - XmTree_ul_point(tw).y;

	ProcessChildQueue((XmTreeWidget) w, &rect);
	RedrawTreeLines(w, &rect);

	/*
	 * Reset upper right and lower left points.
	 */

	XmTree_ul_point(tw).x = w->core.width;
	XmTree_ul_point(tw).y = w->core.height;
	XmTree_lr_point(tw).x = XmTree_lr_point(tw).y = 0;
    }

    XmDropSiteEndUpdate(w);
}

/*	Function Name: CheckExpose
 *	Description: Checks to see if there is an expose event on the queue.
 *	Arguments: disp - the X Display.
 *                 event - the event to check for.
 *                 info_ptr - a pointer to the redispInfo.
 *	Returns: Always False.
 */

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

/*	Function Name: Resize
 *	Description:   Called when this widget has been resized.
 *	Arguments:     w - Extended List Widget to resize.
 *	Returns:       none.
 */

static void
Resize(Widget w)
{
    XmTreeWidget tw = (XmTreeWidget) w;

    if (XmHierarchy_refigure_mode(tw)) {
	LayoutChildren(w, NULL);
	if (XtIsRealized((Widget)tw)) {
	    XClearArea(XtDisplay(tw), XtWindow(tw),
		       0, 0, tw->core.width, tw->core.height, True);
	}
    }
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
    GetDesiredSize(w, &(preferred->width), &(preferred->height), True);
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

static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    XmTreeWidget c_tree = (XmTreeWidget) current;
    XmTreeWidget tree = (XmTreeWidget) set;
    Boolean redisplay = False;
    Boolean layout = False;

    if ((XmHierarchy_v_margin(c_tree) != XmHierarchy_v_margin(tree)) ||
	(XmHierarchy_h_margin(c_tree) != XmHierarchy_h_margin(tree)) ||
	(XmTree_orientation(c_tree) != XmTree_orientation(tree)) ||
	((XmTree_compress_style(c_tree) != XmTree_compress_style(tree)) &&
         (XmTree_orientation(tree) == XmVERTICAL))                  ||
	(XmTree_horizontal_delta(c_tree) != XmTree_horizontal_delta(tree)) ||
	(XmTree_vertical_delta(c_tree) != XmTree_vertical_delta(tree)) ||
	(XmTree_v_node_space(c_tree) != XmTree_v_node_space(tree)) ||
	(XmTree_h_node_space(c_tree) != XmTree_h_node_space(tree)))
    {
	layout = redisplay = True;
    }

    if (XmTree_connect_style(c_tree) != XmTree_connect_style(tree))
	redisplay = True;

    if (XmHierarchy_refigure_mode(c_tree) != XmHierarchy_refigure_mode(tree))
	layout = redisplay = XmHierarchy_refigure_mode(tree);

    if (layout && XmHierarchy_refigure_mode(tree)) {
	CalcLocations(set, False);
	LayoutChildren(set, NULL);

	GetDesiredSize(set, &(set->core.width), &(set->core.height), False);
	redisplay = True;
    }

    return(redisplay);
}


/*    Function Name: Destroy
 *    Description:   Destroys all data allocated by the widget
 *    Arguments:     w - the widget.
 *    Returns:       none.
 */
static void TreeDestroy (Widget widget)
{
  XmTreeWidget tree = (XmTreeWidget) widget;

    _XmListFree(XmTree_child_op_list(tree));
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
    XmTreeWidget tw = (XmTreeWidget) w;

    if (XmHierarchy_refigure_mode(tw)) {
	CalcLocations(w, True);
	LayoutChildren(w, NULL);
	if (XtIsRealized((Widget)tw)) {
	    XClearArea(XtDisplay(tw), XtWindow(tw),
		       0, 0, tw->core.width, tw->core.height, True);
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

static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry * request,
		XtWidgetGeometry * result)
{
    Widget tw = XtParent(w);

    if (!(request->request_mode & (CWWidth | CWHeight | CWBorderWidth)))
	return(XtGeometryNo);

    if (!(request->request_mode & CWWidth)) {
	request->width = w->core.width;
	request->request_mode |= CWWidth;
    }

    if (!(request->request_mode & CWBorderWidth)) {
	request->border_width = w->core.border_width;
	request->request_mode |= CWBorderWidth;
    }

    if (!(request->request_mode & CWHeight)) {
	request->height = w->core.height;
	request->request_mode |= CWHeight;
    }

    if (request->request_mode & (CWX | CWY | CWStackMode | CWSibling)) {
	*result = *request;
	result->request_mode &= ~(CWX | CWY | CWStackMode | CWSibling);
	return(XtGeometryAlmost);
    }

    /*
     * Any width or height request is allowed, but others will be disallowed
     * or result in an almost that states only the width and height can
     * change.
     */

    if (request->request_mode & XtCWQueryOnly)
	return(XtGeometryYes);

    /*
     * A real allowed request, make the change.
     */

    _XmResizeWidget(w, request->width, request->height, request->border_width);

    if (XmHierarchy_refigure_mode((XmTreeWidget) tw)) {
	CalcLocations(tw, True);
	LayoutChildren(tw, w);
	if (XtIsRealized(tw)) {
	    XClearArea(XtDisplay(tw), XtWindow(tw),
		       0, 0, tw->core.width, tw->core.height, True);
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

static void
ConstraintInitialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    TreeConstraints node = GetNodeInfo(set);

    XmTreeC_box_x(node) = XmTreeC_box_y(node) = 0;
    XmTreeC_bb_width(node) = XmTreeC_bb_height(node) = 0;
    XmTreeC_placed(node) = False;
    XmTreeC_is_compressed(node) = False;

    LineStyle_confirm(set, LineSolid);

    if (XmHierarchyC_state(node) != XmNotInHierarchy)
	GetNodeGCs(set);
    else
 	XmTreeC_gc(node) = None;
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
    TreeConstraints node = GetNodeInfo(w);
    XmListElem *elem, *next;
    XmTreeWidget tw;

    if (XmHierarchyC_state(node) == XmNotInHierarchy)
    {
      return;
    }

    tw = (XmTreeWidget) XtParent(w);
    elem = XmListFirst(XmTree_child_op_list(tw));
    while(elem != NULL) {
	TreeConstraints info = (TreeConstraints) XmListElemData(elem);

	next = XmListElemNext(elem);

	if (XmHierarchyC_widget(info) == w) {
	    /*
	     * Each widget will only be in the list once.
	     */

	    _XmListRemove(XmTree_child_op_list(tw), elem);
	    break;
	}

	elem = next;
    }

    ReleaseNodeGCs(w);
}

/*	Function Name: GetNodeGCs(node)
 *	Description: Gets the gc's associated with a tree node.
 *	Arguments: w - the child who's gc we are getting.
 *	Returns: none
 */

static void
GetNodeGCs(Widget w)
{
    XtGCMask mask;
    XGCValues values;
    TreeConstraints node = GetNodeInfo(w);

    mask = GCForeground | GCLineWidth | GCLineStyle | GCBackground;
    values.foreground = XmTreeC_color(node);
    values.background = XmTreeC_background_color(node);
    values.line_width = XmTreeC_line_width(node);
    values.line_style = XmTreeC_line_style(node);
    XmTreeC_gc(node) = XtGetGC(w, mask, &values);
}

/*	Function Name: ReleaseNodeGCs(node)
 *	Description: Releases the gc's associated with a tree node.
 *	Arguments: w - the child who's gc we are releasing
 *	Returns: none
 */

static void
ReleaseNodeGCs(Widget w)
{
    TreeConstraints node = GetNodeInfo(w);

    if (XmTreeC_gc(node) != None)
	XtReleaseGC(w, XmTreeC_gc(node));
}

/*	Function Name: ConstraintSetValues
 *	Description:   Called a constraint is changed on my children.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

static Boolean
ConstraintSetValues(Widget current, Widget request, Widget set,
		    ArgList args, Cardinal * num_args)
{
    Widget tw = XtParent(set);
    TreeConstraints set_node = GetNodeInfo(set);
    TreeConstraints old_node = GetNodeInfo(current);
    Boolean redisplay = False;
    Boolean insert_change = False;
    int i;

    for (i = 0; i < *num_args; i++)
	if (streq(args[i].name, XmNinsertBefore)) {
	    insert_change = True;
	    break;
	}

    if ((XmTreeC_color(set_node) != XmTreeC_color(old_node)) ||
    	(XmTreeC_background_color(set_node) != XmTreeC_background_color(old_node)) ||
    	(XmTreeC_line_width(set_node) != XmTreeC_line_width(old_node)) ||
    	(XmTreeC_line_style(set_node) != XmTreeC_line_style(old_node)))
    {
	LineStyle_confirm(set, XmTreeC_line_style(old_node));
	ReleaseNodeGCs(current);
	GetNodeGCs(set);
	redisplay = True;
    }

    /*
     * Nothing below here needs to be done before we are realized.
     */

    if (!XtIsRealized(set))
	return(False);

    if ((XmHierarchyC_parent(set_node) != XmHierarchyC_parent(old_node)) ||
	(XmHierarchyC_state(set_node) != XmHierarchyC_state(old_node)) ||
	(insert_change) || (XmTreeC_open_close_padding(set_node) !=
			    XmTreeC_open_close_padding(old_node)))
    {
	 /*
	  * Other operations have already been performed by my superclass.
	  */

        if (XmHierarchy_refigure_mode((XmTreeWidget)tw)) {
	    CalcLocations(tw, True);
	    LayoutChildren(tw, NULL);
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

	 /*
	  * Since redisplay redraws the child, not the Tree widget we
	  * need to do the equilivent of a redisplay here.
	  */

	 redisplay = True;
     }

    if (XtIsRealized(tw) && redisplay &&
	(XmHierarchy_refigure_mode((XmTreeWidget)tw)))
    {
	XClearArea(XtDisplay(tw), XtWindow(tw),
		   0, 0, tw->core.width, tw->core.height, True);
    }

    return(False);
}

/************************************************************
 *
 * Hierarchy widget class proceedures.
 *
 ************************************************************/

/*	Function Name: ToggleNodeState
 *	Description:   Toggles the open/close state of a toggle button.
 *	Arguments:     w - the command button that activated this.
 *                     node_ptr - a pointer to the node info.
 *                     call_data - UNUSED.
 *	Returns:       none.
 */

static void
ToggleNodeState(Widget w, XtPointer node_ptr, XtPointer call_data)
{
    Widget tw = XtParent(w);
    XtCallbackProc toggle_node_state;

    _XmProcessLock();
    toggle_node_state = (SUPERCLASS->hierarchy_class.toggle_node_state);
    _XmProcessUnlock();

    (*toggle_node_state)(w, node_ptr, call_data);

    CalcLocations(tw, True);
    LayoutChildren(tw, NULL);

    /*
     * Could check for a size change and abort if we really wanted
     * to be clever.
     */

    if (XtIsRealized(tw)) {
	XClearArea(XtDisplay(tw), XtWindow(tw),
		   0, 0, tw->core.width, tw->core.height, True);
    }
}

/************************************************************
 *
 * Type Converters.
 *
 ************************************************************/

/*	Function Name: CvtStringToConnectStyle
 *	Description:   Converts a string to a connect style
 *	Arguments:     dpy - the X Display.
 *                     args, num_args - *** NOT USED ***
 *                     fromVal - contains the string to convert.
 *                     toVal - contains the converted node state.
 *	Returns:       True if the SetValues succeeds.
 */

static Boolean
CvtStringToConnectStyle(Display * dpy, XrmValuePtr args, Cardinal *num_args,
			XrmValuePtr fromVal, XrmValuePtr toVal)
{
    static XmTreeConnectStyle connect;
    static XrmQuark XtQELadder;
    static XrmQuark XtQEDirect;
    static Boolean haveQuarks = FALSE;
    XrmQuark q;
    char lowerName[BUFSIZ];

    if (!haveQuarks) {
	XtQELadder = XrmStringToQuark("ladder");
	XtQEDirect = XrmStringToQuark("direct");
	haveQuarks = TRUE;
    }

    XmCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
    q = XrmStringToQuark(lowerName);

    if ( (q == XtQELadder) || streq(lowerName,"treeladder") )
	connect = XmTreeLadder;
    else if ( (q == XtQEDirect) || streq(lowerName,"treedirect") )
	connect = XmTreeDirect;
    else {
	XtDisplayStringConversionWarning(dpy,fromVal->addr, XmRXmConnectStyle);
	return(FALSE);		/* Conversion failed. */
    }

    if (toVal->addr == NULL) {
	toVal->size = sizeof(XmTreeConnectStyle);
	toVal->addr = (XtPointer) &connect;
	return(TRUE);
    }

    if (toVal->size >= sizeof(XmTreeConnectStyle)) {
	XmTreeConnectStyle *loc = (XmTreeConnectStyle *)toVal->addr;

	*loc = connect;
	return(TRUE);
    }

    toVal->size = sizeof(XmTreeConnectStyle);
    return(FALSE);
}

/*      Function Name: CvtStringToCompressStyle
 *      Description:   Converts a string to a compress style
 *      Arguments:     dpy - the X Display.
 *                     args, num_args - *** NOT USED ***
 *                     fromVal - contains the string to convert.
 *                     toVal - contains the converted value
 *      Returns:       True if the SetValues succeeds.
 */

static Boolean
CvtStringToCompressStyle(Display * dpy, XrmValuePtr args, Cardinal *num_args,
                        XrmValuePtr fromVal, XrmValuePtr toVal)
{
    static XmTreeCompressStyle compress;
    static XrmQuark XtQECompressNone;
    static XrmQuark XtQECompressLeaves;
    static XrmQuark XtQECompressAll;
    static Boolean haveQuarks = FALSE;
    XrmQuark q;
    char lowerName[BUFSIZ];

    if (!haveQuarks) {
        XtQECompressNone = XrmStringToQuark("compressnone");
        XtQECompressLeaves = XrmStringToQuark("compressleaves");
        XtQECompressAll = XrmStringToQuark("compressall");
        haveQuarks = TRUE;
    }

    XmCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
    q = XrmStringToQuark(lowerName);

    if ((q == XtQECompressNone) || streq(lowerName,"none") || streq(lowerName,"treecompressnone") )
        compress = XmTreeCompressNone;
    else if ((q == XtQECompressLeaves) || streq(lowerName,"leaves") || streq(lowerName,"treecompressleaves") )
        compress = XmTreeCompressLeaves;
    else if ((q == XtQECompressAll) || streq(lowerName,"all") || streq(lowerName,"treecompressall") )
        compress = XmTreeCompressAll;
    else {
        XtDisplayStringConversionWarning(dpy,fromVal->addr, XmRXmCompressStyle);
        return(FALSE);          /* Conversion failed. */
    }

    if (toVal->addr == NULL) {
        toVal->size = sizeof(XmTreeCompressStyle);
        toVal->addr = (XtPointer) &compress;
        return(TRUE);
    }

    if (toVal->size >= sizeof(XmTreeCompressStyle)) {
        XmTreeCompressStyle *loc = (XmTreeCompressStyle *)toVal->addr;

        *loc = compress;
        return(TRUE);
    }

    toVal->size = sizeof(XmTreeCompressStyle);
    return(FALSE);
}

static Boolean
CvtStringToLineStyle(Display * dpy, XrmValuePtr args, Cardinal *num_args,
			XrmValuePtr fromVal, XrmValuePtr toVal)
{
    static int lineStyle = LineSolid;
    char lowerName[BUFSIZ];

    XmCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);

    if ( streq(lowerName, "linesolid") || streq(lowerName,"solid") )
	lineStyle = LineSolid;
    else if ( streq(lowerName, "lineonoffdash") || streq(lowerName,"onoffdash") )
	lineStyle = LineOnOffDash;
    else if ( streq(lowerName, "linedoubledash") || streq(lowerName,"doubledash") )
	lineStyle = LineDoubleDash;
    else {
	XtDisplayStringConversionWarning(dpy,fromVal->addr, XmRXmLineStyle);
	return(FALSE);		/* Conversion failed. */
    }

    if (toVal->addr == NULL) {
	toVal->size = sizeof(int);
	toVal->addr = (XtPointer) &lineStyle;
	return(TRUE);
    }

    if (toVal->size >= sizeof(int)) {
	int *loc = (int*)toVal->addr;

	*loc = lineStyle;
	return(TRUE);
    }

    toVal->size = sizeof(int);
    return(FALSE);
}

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

/*	Function Name: HorizontalNodeSpaceDefault
 *	Description: determines the default value for the
 *                 XmNhorizontalNodeSpace resource based on
 *                 the orientation.
 *	Arguments: w - the tree widget
 *                 offset - offset of the field in the widget record.
 *                 value - resource descriptor to return.
 *	Returns: none.
 */

static void
HorizontalNodeSpaceDefault(Widget widget, int offset, XrmValue *value)
{
    XmTreeWidget tw = (XmTreeWidget) widget;
    static Dimension default_val;

    if (XmTree_orientation(tw) == XmVERTICAL)
        default_val = 2;
    else
        default_val = 20;

    value->addr = (XtPointer) &default_val;
}

/*	Function Name: VerticalNodeSpaceDefault
 *	Description: determines the default value for the
 *                 XmNverticalNodeSpace resource based on
 *                 the orientation.
 *	Arguments: w - the tree widget
 *                 offset - offset of the field in the widget record.
 *                 value - resource descriptor to return.
 *	Returns: none.
 */

static void
VerticalNodeSpaceDefault(Widget widget, int offset, XrmValue *value)
{
    XmTreeWidget tw = (XmTreeWidget) widget;
    static Dimension default_val;

    if (XmTree_orientation(tw) == XmVERTICAL)
        default_val = 20;
    else
        default_val = 2;

    value->addr = (XtPointer) &default_val;
}

/*	Function Name: LineColorDefault
 *	Description: Sets the default line color to the parent's
 *                   foreground color.
 *	Arguments: w - the widget to set the line color on.
 *                 offset - offset of the field in the widget record.
 *                 value - resource descriptor to return.
 *	Returns: none.
 */

static void
LineColorDefault(Widget widget, int offset, XrmValue *value)
{
    XmTreeWidget tw = (XmTreeWidget) XtParent(widget);

    value->addr = (XtPointer) &(tw->manager.foreground);
}

static void
LineBackgroundColorDefault(Widget widget, int offset, XrmValue *value)
{
    XmTreeWidget tw = (XmTreeWidget) XtParent(widget);

    value->addr = (XtPointer) &(tw->core.background_pixel);
}

/*	Function Name: DrawExtraLadderLines
 *	Description: Draws the extra horizontal and vertical ladder lines
 *                   that connect elements of the tree.
 *	Arguments: w - the tree widget.
 *                 gc - gc of the parent node
 *                 first_kid - start points for vertical line
 *                 last_kid - end points for vertical line
 *                 parent_point - supplies y end points for horizontal line
 *	Returns: none
 */
static void
DrawExtraLadderLines( Widget w, GC gc, LadderPoint first_kid,
		      LadderPoint last_kid, LadderPoint parent_point )
{
  XmTreeWidget tw = (XmTreeWidget)w;

  if (XmTree_connect_style(tw) == XmTreeLadder)
    {

      /* First draw the line that starts at parent midpoint and goes
       *    to the ladder line connecting the children. Then draw the
       *    line connecting the children.
       */

      if (XmTree_orientation(tw) == XmHORIZONTAL)
      {
          XDrawLine(XtDisplay(w), XtWindow(w), gc, parent_point.x,
		parent_point.y, first_kid.x, parent_point.y );
          XDrawLine(XtDisplay(w), XtWindow(w), gc, first_kid.x, first_kid.y,
		first_kid.x, last_kid.y );
      }
      else    /* orientation == XmVERTICAL */
      {
          XDrawLine(XtDisplay(w), XtWindow(w), gc, parent_point.x,
		parent_point.y, parent_point.x, first_kid.y );
          XDrawLine(XtDisplay(w), XtWindow(w), gc, first_kid.x, first_kid.y,
		last_kid.x, first_kid.y );
      }
    }

}


/*	Function Name: RedrawTreeLines;
 *	Description: Redraws the lines that connect elements of the tree.
 *	Arguments: w - the tree widget.
 *                 rect - the rectangle to clip to.
 *	Returns: none
 */

static void
RedrawTreeLines(Widget w, XRectangle * rect)
{
    XmTreeWidget tw = (XmTreeWidget) w;

    if (XtIsRealized(w))
	DrawTreeLine(w, rect, (TreeConstraints) XmHierarchy_top_node(tw));
}

/*	Function Name: _CalcNodeMidPoint
 *	Description: Calculates the x and y origin of the
 *                   part of the ladder line that comes out of the
 *                   parent node
 *	Arguments:
 *                 node - The node to calculate points for
 *                 w - the tree widget
 *                 ret_point - return values for calculated points
 *	Returns: none
 */
static void
_CalcNodeMidPoint( TreeConstraints node, Widget w,
		   LadderPoint *ret_point )
{
  register int extra_space;
  XmTreeWidget tw = (XmTreeWidget)w;

  if (!XmHierarchyC_widget(node)) return;

  if (XmTree_orientation(tw) == XmHORIZONTAL)
  {
    ret_point->x = (XmTreeC_box_x(node) + XmTreeC_widget_offset(node) +
		  (XmHierarchyC_widget(node))->core.width +
		  XmHierarchy_h_margin(tw));

    extra_space = GetExtraVertSpace(w);

    ret_point->y = XmTreeC_box_y(node)+(int)(XmTreeC_bb_height(node)+extra_space)/2;
  }
  else    /* orientation == XmVERTICAL */
  {
    ret_point->y = (XmTreeC_box_y(node) + XmTreeC_widget_offset(node) +
		  (XmHierarchyC_widget(node))->core.height +
		  XmHierarchy_v_margin(tw));

    extra_space = GetExtraHorizSpace(w);

    ret_point->x = XmTreeC_box_x(node)+(int)(XmTreeC_bb_width(node)+extra_space)/2;
  }
}

/*	Function Name: DrawTreeLine
 *	Description: Draws a line between two elements in the tree.
 *	Arguments: w - the tree.
 *                 rect - the rectangle to clip to.
 *                 node - The node to draw lines on. (it also recurses to all
 *                        descendants.
 *	Returns: none
 */

static void
DrawTreeLine(Widget w, XRectangle *rect, TreeConstraints node)
{
    TreeConstraints * kids;
    register int i, num_kids;
    TreeConstraints from_node = node;
    LadderPoint from_node_point = {0, 0}, kid_point = {0, 0}, first_kid_point = {0, 0};
    LadderPoint last_kid_point = {0, 0};
    Boolean first_time=True;
    XmTreeWidget tw = (XmTreeWidget)w;

    if (XmHierarchyC_widget(node) != NULL && !XtIsManaged(XmHierarchyC_widget(node)))
	return;

    /*
     * Hunt up the tree until we find a non-hidden parent to be the from
     * node.
     */

    while ((XmHierarchyC_parent(from_node) != NULL) &&
	   (XmHierarchyC_state(from_node) == XmHidden))
    {
	from_node = GetNodeInfo(XmHierarchyC_parent(from_node));
    }

    num_kids = XmHierarchyC_num_children(node);
    kids = (TreeConstraints *) XmHierarchyC_children(node);

    /*
     * This only executes if the root is hidden.
     */

    if (XmHierarchyC_state(from_node) == XmHidden) {
	for (i = 0; i < num_kids; i++, kids++)
	    DrawTreeLine(w, rect, *kids);	/* recurse to descendants. */
	return;
    }

    if (XmHierarchyC_state(from_node) == XmClosed)
	return;

    _CalcNodeMidPoint( from_node, w, &from_node_point );

    for (i = 0; i < num_kids; i++, kids++) {
      if (XtIsManaged(XmHierarchyC_widget((*kids)))) {
	if (XmHierarchyC_state((*kids)) != XmHidden) {
	  _DrawLine(w, rect, from_node, *kids, from_node_point, &kid_point);

	  /* set up the points to draw ladder lines */

	  if (XmTree_connect_style(tw) == XmTreeLadder) {
	    last_kid_point.x = kid_point.x;
	    last_kid_point.y = kid_point.y;

	    if (first_time ){
	      first_kid_point.x = last_kid_point.x;
	      first_kid_point.y = last_kid_point.y;
	      first_time = False;
	    }
	  }
	}
	DrawTreeLine(w, rect, *kids);	/* recurse to descendants. */
      }
    }

    /* Draw extra ladder lines if necessary.
     * If first time False, then we know we've gone through the line
     * drawing loop at least twice
     */
    if ((num_kids > 1) && (!first_time) && (XmTree_connect_style(tw) == XmTreeLadder)) {
      DrawExtraLadderLines( w, XmTreeC_gc(node), first_kid_point,
			   last_kid_point, from_node_point );
    }
  }

/*	Function Name: _DrawLine
 *	Description: Draw a tree line between two nodes in the tree. If
 *                 child is an only child in XmTreeLadder, then draw a line
 *                 directly to it.
 *	Arguments: w - the tree.
 *                 rect - the rectangle to clip to.
 *                 parent - The parent.
 *                 child - The child to draw the line to.
 *                 from_ladder_point - The midpoint of parent where all
 *                      lines start from (whether ladder or direct)
 *                 to_ladder_point - RETURNs the midpoint of the kid - where
 *                      all lines end at (whether ladder or direct)
 *	Returns: none
 */

static void
_DrawLine(Widget w, XRectangle *rect, TreeConstraints parent,
	  TreeConstraints child, LadderPoint from_ladder_point,
	  LadderPoint *to_ladder_point )

{
    GC gc;
    XmTreeWidget tw = (XmTreeWidget) w;
    register int x2=0, y2=0, extra_space;
    register int rx2, ry2, cx1, cx2, cy1, cy2;

    /*
     * (from_ladder_point.x, from_ladder_point.y) are the coordinates
     *     of the parent's midpoint
     * (x2, y2) are the coordinates of the child midpoint
     *
     * We will end up drawing either a partial ladder line from
     *     (cx1, cy1) to (cx2, cy2) or a full line from
     *     (from_ladder_point.x, from_ladder_point.y) to (x2, y2)
     *
     * (rx2, ry2) are the coordinates of the lower right of clip rectangle
     */

    /*
     * Must always do the first node since it is what draws the root
     * of the ladder.
     */

    if (child != (TreeConstraints) XmHierarchyC_children(parent)[0] &&
	(!(XmHierarchyC_status(child) & IS_MAPPED) ||
	 (XmHierarchyC_status(child) & IS_COMPRESSED)))
    {
	return;
    }

    gc = XmTreeC_gc(child);

    if (XmTree_orientation(tw) == XmHORIZONTAL)
    {
      extra_space = GetExtraVertSpace(w);
      x2 = (XmTreeC_box_x(child) + XmHierarchy_h_margin(tw));
      y2 = XmTreeC_box_y(child) + (int)(XmTreeC_bb_height(child) + extra_space)/2;
    }
    else     /* orientation == XmVERTICAL */
    {
      extra_space = GetExtraHorizSpace(w);
      y2 = (XmTreeC_box_y(child) + XmHierarchy_v_margin(tw));
      x2 = XmTreeC_box_x(child) + (int)(XmTreeC_bb_width(child) + extra_space)/2;
    }

    cx1 = MIN(from_ladder_point.x, x2);
    cx2 = MAX(from_ladder_point.x, x2);

    cy1 = MIN(from_ladder_point.y, y2);
    cy2 = MAX(from_ladder_point.y, y2);

    /* If XmTreeLadder and more than one child, do funky ladder line calc's */

    if ((XmTree_connect_style(tw) == XmTreeLadder)&&(XmHierarchyC_num_children(parent) > 1))
    {
        if (XmTree_orientation(tw) == XmHORIZONTAL)
        {
	  cx1 += (cx2 - cx1)/2;
	  cy1 = cy2 = y2;
        }
        else     /* orientation == XmVERTICAL */
        {
          if (XmTreeC_is_compressed(child))
	      cy1 += (int)(cy2 - XmTree_vertical_delta(tw) - cy1)/(int)2;
          else
	      cy1 += (cy2 - cy1)/2;
	  cx1 = cx2 = x2;
        }
    }
    else /* XmTree_connect_style== XmTreeDirect or only has one child in XmTreeLadder */
    {
	/* If there is a small pixel difference, make the line straight
	 * or it will look wierd.
	 */

	if ((cy2-cy1) == 1)
	  y2 = from_ladder_point.y;

	if ((cx2-cx1) == 1)
	  x2 = from_ladder_point.x;
    }

    rx2 = rect->x + rect->width;
    ry2 = rect->y + rect->height;

    /*
     * Not very pretty, only checks to see if the rect containing the
     * line is partially within the exposed rectangle.
     */

    if (!((cx1 > rx2) || (cy1 > ry2) || (cx2 < rect->x) || (cy2 < rect->y)))
    {
	if ((XmTree_connect_style(tw) == XmTreeLadder)
	    &&(XmHierarchyC_num_children(parent) > 1))
	    XDrawLine(XtDisplay(w), XtWindow(w), gc, cx1, cy1, cx2, cy2);
	else
	    XDrawLine(XtDisplay(w), XtWindow(w), gc, from_ladder_point.x,
		      from_ladder_point.y, x2, y2);
    }

    /* This is sent back because the ladder lines get connected later */
    to_ladder_point->x = cx1;
    to_ladder_point->y = cy1;

}

/*	Function Name: CalcLocations
 *	Description:   Calculates the location of each widget in the tree.
 *	Arguments:     w - the tree widget.
 *                     Boolean - attempt resize?
 *	Returns:       none.
 */

static void
CalcLocations(Widget w, Boolean resize_it)
{
    Cardinal          current_index;
    XmTreeWidget      tw = (XmTreeWidget) w;
    TreeConstraints   node;
    XmTreeWidgetClass tc = (XmTreeWidgetClass) XtClass(w);
    register int      i;

    /*
     * Reset each node to be hidden;
     */
    for( i = 0; i < tw->composite.num_children; ++i )
    {
	node = GetNodeInfo(tw->composite.children[i]);
	XmHierarchyC_status(node) |= IS_COMPRESSED;
    }

    XmHierarchy_num_nodes(tw) = 0;
    (void)GetNodeHeightAndWidth(w, (TreeConstraints) XmHierarchy_top_node(tw),
			  &(XmHierarchy_num_nodes(tw)), 0);

    current_index = 0;
    {
        XmHierarchyBuildTableProc build_node_table;
        _XmProcessLock();
        build_node_table = tc->hierarchy_class.build_node_table;
        _XmProcessUnlock();
        (*build_node_table) (w, XmHierarchy_top_node(tw), &current_index);
    }
    CalcMaxSize(w);
    FindNodeLocations(w);		/* Finds the location for each node. */

    if (resize_it)
	RequestNewSize(w);

}

/*	Function Name: RequestNewSize
 *	Description:   Asks our parent for a new size.
 *	Arguments:     w - the data request tree widget.
 *	Returns:       True if resize happened.
 */

static Boolean
RequestNewSize(Widget w)
{
    Dimension width, height, rwidth, rheight;
    XtGeometryResult ret_val;

    GetDesiredSize(w, &width, &height, False);
    ret_val = XtMakeResizeRequest(w, width, height, &rwidth, &rheight);

    if (ret_val == XtGeometryAlmost)
	ret_val = XtMakeResizeRequest(w, rwidth, rheight, NULL, NULL);

    return(ret_val == XtGeometryYes);
}

/*	Function Name: GetDesiredSize
 *	Description:   Returns the desired size of the tree widget.
 *	Arguments:     w - the tree widget.
 *                     width - the desired width.
 *                     height - the desired height.
 *                     recalc - recalculate the new size if row height unset?
 *	Returns:       none.
 */

static void
GetDesiredSize(Widget w, Dimension *width, Dimension *height, Boolean recalc)
{
    XmTreeWidget tw = (XmTreeWidget) w;

    if (recalc)
	CalcLocations(w, False);

    *width = XmTree_max_width(tw);
    *height = XmTree_max_height(tw);
}

/*	Function Name: LayoutChildren
 *	Description:   all the fun stuff for positioning the children is here.
 *	Arguments:     w - the tree widget.
 *                     assign_child - A child to assign values to, rather
 *                                    than just moving it.
 *	Returns:       none.
 */

static void
LayoutChildren(Widget w, Widget assign_child)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    XmTreeWidgetClass tc = (XmTreeWidgetClass) XtClass(w);
    register HierarchyConstraints * node_table = XmHierarchy_node_table(tw);
    register Cardinal num_nodes = XmHierarchy_num_nodes(tw);
    register int i, extra_space;
    Boolean register_workproc = True;

    XmDropSiteStartUpdate(w);

    /*
     * Remove the old list, replace it with the new one.
     */

    if (XmListFirst(XmTree_child_op_list(tw)) != NULL) {
	if( XmHierarchy_work_proc_id(tw) != (XtWorkProcId) NULL )
	{
	    XtRemoveWorkProc(XmHierarchy_work_proc_id(tw));
	    XmHierarchy_work_proc_id(tw) = (XtWorkProcId) NULL;
	}
	_XmListFree(XmTree_child_op_list(tw));
	XmTree_child_op_list(tw) = _XmListInit();
	register_workproc = False;
    }

    /*
     * Unmap all nodes that no longer are visible.
     */

    {
        XmHierarchyExtraNodeProc unmap_all_extra_nodes;

        _XmProcessLock();
        unmap_all_extra_nodes = tc->hierarchy_class.unmap_all_extra_nodes;
        _XmProcessUnlock();
        (*unmap_all_extra_nodes) (w, XmHierarchy_top_node(tw));
    }
    /*
     * Go through all nodes that can possibly be displayed, putting those
     * that will be visible on the screen and unmapping all others.
     */

    for (i = 0; i < num_nodes; i++, node_table++) {
	TreeConstraints t_node = (TreeConstraints) *node_table;
	Widget child = XmHierarchyC_widget(t_node);
	Widget open_close = XmHierarchyC_open_close_button(t_node);
	register Dimension c_height, c_width;
	register Position y_loc, x_loc, oc_y_loc = 0, oc_x_loc = 0;

	c_width = child->core.width + 2 * child->core.border_width;
	c_height = child->core.height + 2 * child->core.border_width;

	if (XmTree_orientation(tw) == XmHORIZONTAL) /* DMS */
	{
		/*
		 * Nodes and open close buttons are centered vertically
		 * in the box that contains them.
		 */

    		extra_space = GetExtraVertSpace(w);
		y_loc = (XmTreeC_box_y(t_node) +
			 (int)(XmTreeC_bb_height(t_node) + extra_space - c_height) / 2);

		if (open_close != NULL) {
		    Dimension oc_height = (open_close->core.height +
					   2 * open_close->core.border_width);

		    oc_y_loc = y_loc + (int)(c_height - oc_height)/2;
		}

		oc_x_loc = XmTreeC_box_x(t_node) + XmHierarchy_h_margin(tw);

		x_loc = oc_x_loc + XmTreeC_widget_offset(t_node);

	} else /* orientation == XmVERTICAL */
	{
		/*
		 * Nodes and open close buttons are centered horizontally
		 * in the box that contains them.
		 */

    		extra_space = GetExtraHorizSpace(w);

		x_loc = (XmTreeC_box_x(t_node) +
			(int)(XmTreeC_bb_width(t_node) + extra_space - c_width) / 2);

		if (open_close != NULL) {
		    Dimension oc_width = (open_close->core.width +
					   2 * open_close->core.border_width);

		    oc_x_loc = x_loc + (int)(c_width - oc_width)/2;
		}

	        oc_y_loc = XmTreeC_box_y(t_node) + XmHierarchy_v_margin(tw);

		y_loc = oc_y_loc + XmTreeC_widget_offset(t_node);
	}

	if (child == assign_child) {
	    child->core.x = x_loc;
	    child->core.y = y_loc;
	}
	MoveNode(tw, t_node, x_loc, y_loc, oc_x_loc, oc_y_loc, True);
    }

    if (register_workproc) {
	XmHierarchy_work_proc_id(tw) =
	    XtAppAddWorkProc(XtWidgetToApplicationContext(w),
			     MoveNodesTimer, (XtPointer) w);
    }

    XmDropSiteEndUpdate(w);
}

/*	Function Name: GetExtraVertSpace
 *	Description: Returns the amount of unused vertical space in the
 *                   tree, given the current geometry information.
 *	Arguments: w - the tree widget.
 *	Returns: see description.
 */

static int
GetExtraVertSpace(Widget w)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    register int space, vmargin;
    TreeConstraints node = (TreeConstraints) XmHierarchy_top_node(tw);

    space = w->core.height - XmTreeC_bb_height(node);
    vmargin = 2 * XmHierarchy_v_margin(tw);

    return((space > vmargin) ? space : vmargin);
}

/*	Function Name: GetExtraHorizSpace
 *	Description: Returns the amount of unused horizontal space in the
 *                   tree, given the current geometry information.
 *	Arguments: w - the tree widget.
 *	Returns: see description.
 */

static int
GetExtraHorizSpace(Widget w)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    register int space, hmargin;
    TreeConstraints node = (TreeConstraints) XmHierarchy_top_node(tw);

    space = w->core.width - XmTreeC_bb_width(node);
    hmargin = 2 * XmHierarchy_h_margin(tw);

    return((space > hmargin) ? space : hmargin);
}

/*	Function Name: FindNodeLocations
 *	Description: Finds the location each node in the node table
 *                   should be placed at.
 *	Arguments:   w - the tree widget.
 *	Returns: none.
 */

static void
FindNodeLocations(Widget w)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    TreeConstraints * node;
    register int i, num_nodes;
    Widget *childP;

    _ResetPlacedFlag((TreeConstraints) XmHierarchy_top_node(tw));

    ForAllChildren(tw, childP)
	_ResetPlacedFlag(GetNodeInfo(*childP));

    num_nodes = XmHierarchy_num_nodes(tw);
    node = (TreeConstraints *) XmHierarchy_node_table(tw);
    for (i = 0; i < num_nodes; i++, node++)
	_PlaceNode(w, *node);
}

/*	Function Name: _ResetPlacedFlag
 *	Description: Resets the placed flag on all notes.
 *	Arguments: node - the node to place.
 *	Returns: none.
 */

static void
_ResetPlacedFlag(TreeConstraints node)
{
    register TreeConstraints *child;
    register int i, num;

    if (node == NULL)
	return;

    XmTreeC_placed(node) = False;

    child = (TreeConstraints *) XmHierarchyC_children(node);
    for (num = XmHierarchyC_num_children(node), i = 0; i < num; i++, child++)
	_ResetPlacedFlag(*child);
}

/*	Function Name: _PlaceNode
 *	Description: Actuall Places a node (this is a recursive call).
 *	Arguments: w - the tree widget.
 *                 node - the node to place.
 *	Returns: none.
 */

static void
_PlaceNode(Widget w, TreeConstraints node)
{
  XmTreeWidget tw = (XmTreeWidget) w;
  register TreeConstraints *child, prev_child, parent;
  register Widget pw = XmHierarchyC_parent(node);
  register int i, num, x_loc, y_loc, box_amount, boxy, boxx;

  if ((node == NULL) || XmTreeC_placed(node))	/* Already placed. */
    return;

  if (pw == NULL) {
    if (node == (TreeConstraints) XmHierarchy_top_node(tw)) {
      XmTreeC_placed(node) = TRUE;
      XmTreeC_box_x(node) = 0;
      XmTreeC_box_y(node) = 0;
      return;
    }
    else
      parent = (TreeConstraints) XmHierarchy_top_node(tw);
  }
  else
    parent = GetNodeInfo(pw);

  if (!XmTreeC_placed(parent))
    _PlaceNode(w, parent);

  /*
   * Place all the children of this node.
   */

  num = XmHierarchyC_num_children(parent);

  /*
   * Calculate how much room the children take up in the box
   */

  child = (TreeConstraints * )XmHierarchyC_children(parent);
  prev_child = NULL;
  box_amount = 0;

  if (XmTree_orientation(tw) == XmHORIZONTAL)
  {
    for (i = 0; i < num; child++, i++ ) {
      if ((child != NULL) && (XtIsManaged(XmHierarchyC_widget(*child)))){
        box_amount += XmTreeC_bb_height(*child);
        if (prev_child != NULL)
          box_amount += XmTree_v_node_space(tw);
        prev_child = *child;
      }
    }
    /* center in parent's bounding box */
    boxy = XmTreeC_box_y(parent)+((int)(XmTreeC_bb_height(parent)-box_amount)/2);

    /*
     * Calculate the positions of all the child bounding boxes
     */

    child = (TreeConstraints *) XmHierarchyC_children(parent);
    prev_child = NULL;

    x_loc = XmTreeC_box_x(parent);

    if (XmHierarchyC_state(parent) != XmHidden)
      x_loc += (pw->core.width + XmTreeC_widget_offset(parent) +
	      2 * pw->core.border_width + XmTree_h_node_space(tw));


    for (i = 0; i < num; i++, child++)
    {
      XmTreeC_placed(*child) = TRUE;
      XmTreeC_box_x(*child) = x_loc;

      /* Only calculate for children that have node widgets that are managed */

      if ((XmHierarchyC_widget(*child))&&
	  (XtIsManaged(XmHierarchyC_widget(*child))))
	{
	  if (prev_child == NULL)
	    XmTreeC_box_y(*child) = boxy;
	  else
	    XmTreeC_box_y(*child) = (XmTreeC_box_y(prev_child) +
				    XmTreeC_bb_height(prev_child) +
				    XmTree_v_node_space(tw));
	  prev_child = *child;
	}
    }
  } else   /* orientation == XmVERTICAL */
  {
    for (i = 0; i < num; child++, i++ ) {
      if ((child != NULL) && (XtIsManaged(XmHierarchyC_widget(*child)))){
        box_amount += XmTreeC_bb_width(*child);
        if ((XmTreeC_is_compressed(*child))
        ||  (prev_child && XmTreeC_is_compressed(prev_child)))
          box_amount -= XmTree_horizontal_delta(tw);
        if (prev_child != NULL)
          box_amount += XmTree_h_node_space(tw);
        prev_child = *child;
      }
    }
    /* center in parent's bounding box */
    boxx = XmTreeC_box_x(parent)+((int)(XmTreeC_bb_width(parent)-box_amount)/2);

    /*
     * Calculate the positions of all the child bounding boxes
     */

    child = (TreeConstraints *) XmHierarchyC_children(parent);
    prev_child = NULL;

    y_loc = XmTreeC_box_y(parent);

    if (XmHierarchyC_state(parent) != XmHidden)
      y_loc += (pw->core.height + XmTreeC_widget_offset(parent) +
	      2 * pw->core.border_width + XmTree_v_node_space(tw));

  for (i = 0; i < num; i++, child++)
    {
      XmTreeC_placed(*child) = TRUE;
      XmTreeC_box_y(*child) = y_loc;

      /* Only calculate for children that have node widgets that are managed */

      if ((XmHierarchyC_widget(*child))&&
	  (XtIsManaged(XmHierarchyC_widget(*child))))
	{
	  if (prev_child == NULL)
	    XmTreeC_box_x(*child) = boxx;
	  else
	    XmTreeC_box_x(*child) = (XmTreeC_box_x(prev_child) +
				    XmTreeC_bb_width(prev_child) +
				    XmTree_h_node_space(tw));

          /*
           * If we are doing tree compression, we may have to move this
           * widget down. If the compress_style is CompressAll
           * then we should compress every other node no matter what.
           * If the compress_style == CompressLeaves then we only compress
           * alternating nodes if they have no children.
           */

          if (XmTreeC_is_compressed(*child))
	  {
	      XmTreeC_box_y(*child) += XmTree_vertical_delta(tw);
	      XmTreeC_box_x(*child) -= XmTree_horizontal_delta(tw);
	  }
          else if (prev_child && XmTreeC_is_compressed(prev_child))
	      XmTreeC_box_x(*child) -= XmTree_horizontal_delta(tw);

          prev_child = *child;
	}
    }

  }

}

/*	Function Name: GetNodeHeightAndWidth
 *	Description:   Gets the size of each node.
 *	Arguments:     w - the drt widget.
 *                     node - the node to get the height and width of.
 *                     tree_depth - depth of the tree at this node.
 * IN/OUT              num - number of nodes.
 *                     sib_index - index of this node with relation
 *                          to siblings. Used for vertical compression.
 *	Returns:       none
 */

static Boolean
GetNodeHeightAndWidth(Widget w, TreeConstraints node,
        Cardinal * num, Cardinal sib_index)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    register int i, num_kids, l_width, l_height;

    if (node == NULL)
	return(False);

    l_width = l_height = 0;

    XmTreeC_bb_width(node) = XmTreeC_bb_height(node) = 0;

    if ((node == NULL) || ((XmHierarchyC_widget(node) != NULL) &&
			   !XtIsManaged(XmHierarchyC_widget(node))))
    {
	return(False);
    }

    if (XmHierarchyC_state(node) != XmHidden) {
	Widget child = XmHierarchyC_widget(node);
	Dimension bw = 2 * child->core.border_width;

	XmTreeC_bb_width(node) = child->core.width + bw;
	XmTreeC_bb_height(node) = child->core.height + bw;

	/*
	 * Leave space to place the open close button if it exists.
	 */

	if (XmHierarchyC_open_close_button(node) != NULL) {
	    Dimension width, height, border_width;

	    width = XmHierarchyC_open_close_button(node)->core.width;
	    height = XmHierarchyC_open_close_button(node)->core.height;
	    border_width = 2*XmHierarchyC_open_close_button(node)->core.border_width;

	    width += border_width;
	    height += border_width;

	    if (XmTree_orientation(tw) == XmHORIZONTAL)
	    {
	        width += XmTreeC_open_close_padding(node);

	        XmTreeC_bb_width(node) += width;
	        XmTreeC_widget_offset(node) = width;
	        if (height > XmTreeC_bb_height(node))
		    XmTreeC_bb_height(node) = height;
	    }
	    else /* orientation == XmVERTICAL */
	    {
	        height += XmTreeC_open_close_padding(node);

	        XmTreeC_bb_height(node) += height;
	        XmTreeC_widget_offset(node) = height;
	        if (width > XmTreeC_bb_width(node))
		    XmTreeC_bb_width(node) = width;
	    }

	}
	else
	    XmTreeC_widget_offset(node) = 0;

	(*num)++;
    }

    if (XmTree_orientation(tw) == XmHORIZONTAL)
    {
      num_kids = XmHierarchyC_num_children(node);
      if ((XmHierarchyC_state(node) != XmClosed) && (num_kids > 0)) {
	register TreeConstraints * child;
	register int num_managed=0;

 	child = (TreeConstraints *) XmHierarchyC_children(node);

	for(i = 0; i < num_kids; i++, child++) {

	  /* If node values were calculated for this child,
	   * and there is more than one child managed, add node
	   * space to bounding box size
	   *
	   * l_height = MAX(sum of N child bounding box heights +
	   *              (N-1)*v_node_space, our height);
	   * l_width = MAX(child bounding box widths) + 1*h_node_space +
	   *              our width;
	   */

	  if (GetNodeHeightAndWidth(w, *child, num, i)) {
	    num_managed++;
	    if (num_managed > 1)
	      l_height += XmTree_v_node_space(tw);
	  }

	  if ((int)l_width < (int)XmTreeC_bb_width(*child))
	    l_width = XmTreeC_bb_width(*child);

	  l_height += XmTreeC_bb_height(*child);

	}
	if (XmHierarchyC_state(node) != XmHidden)
	  l_width += XmTree_h_node_space(tw);
      }

      XmTreeC_bb_width(node) += l_width;

      if ((int)XmTreeC_bb_height(node) < (int)l_height)
        XmTreeC_bb_height(node) = l_height;
    }
    else /* orientation == XmVERTICAL */
    {
      num_kids = XmHierarchyC_num_children(node);
      if ((XmHierarchyC_state(node) != XmClosed) && (num_kids > 0)) {
  	register TreeConstraints * child;
  	register TreeConstraints prev_child = NULL;
  	register int num_managed=0;

 	child = (TreeConstraints *) XmHierarchyC_children(node);

	for(i = 0; i < num_kids; i++) {

	  /* If node values were calculated for this child,
	   * and there is more than one child managed, add node
	   * space to bounding box size
	   */

	  if (GetNodeHeightAndWidth(w, *child, num, i)) {
	    num_managed++;
	    if (num_managed > 1)
	      l_width += XmTree_h_node_space(tw);
	  }

          /*
           * If this child is compressed, add the verticalDelta to
           * it's height for purposes of calculating our own height
           * and subtract the horizontal delta from its width for
           * our own width.
           * If this child is not compressed, use just its height but
           * we still need to subtract the horizontal delta from our
           * width if the previous child is compressed.
           */
          if (XmTreeC_is_compressed(*child))
          {
	    if ((int)l_height < (int)XmTreeC_bb_height(*child)
                                + (int)XmTree_vertical_delta(tw))
	      l_height = XmTreeC_bb_height(*child) + XmTree_vertical_delta(tw);
	    l_width += XmTreeC_bb_width(*child) - XmTree_horizontal_delta(tw);
          }
          else
          {
	    if ((int)l_height < (int)XmTreeC_bb_height(*child))
	      l_height = XmTreeC_bb_height(*child);

            if (prev_child && XmTreeC_is_compressed(prev_child))
	        l_width += XmTreeC_bb_width(*child) - XmTree_horizontal_delta(tw);
            else
	        l_width += XmTreeC_bb_width(*child);
          }

          prev_child = *child;
          child++;
	}

	if (XmHierarchyC_state(node) != XmHidden)
	  l_height += XmTree_v_node_space(tw);
      }

      XmTreeC_bb_height(node) += l_height;

      if ((int)XmTreeC_bb_width(node) < (int)l_width)
        XmTreeC_bb_width(node) = l_width;

      if ( (((XmTree_compress_style(tw) == XmTreeCompressAll)
	        && ((sib_index % 2) == 1))
	        && (XmHierarchyC_parent(node) != NULL))
          ||   (((XmTree_compress_style(tw) == XmTreeCompressLeaves)
                && (num_kids == 0)
		&& (XmHierarchyC_parent(node) != NULL)
                &&   ((sib_index % 2) == 1))) )
      {
            XmTreeC_is_compressed(node) = True;
      }
      else
          XmTreeC_is_compressed(node) = False;
    }

    return( True );
}

/*	Function Name: GetNodeInfo
 *	Description:   Gets the node info associated with this widget.
 *	Arguments:     w - the widget.
 *	Returns:       the node info about this widget.
 */

static TreeConstraints
GetNodeInfo(Widget w)
{
    return((TreeConstraints) w->core.constraints);
}

/*	Function Name: CalcMaxSize
 *	Description:   Calculates the maximum width of the tree.
 *	Arguments:     w - the tree.
 *	Returns:       The max width the tree would like to be.
 *
 * NOTE:  This is calculating both the max width and the max_height
 */

static void
CalcMaxSize(Widget w)
{
    XmTreeWidget tw = (XmTreeWidget) w;
    register TreeConstraints node = (TreeConstraints) XmHierarchy_top_node(tw);

    XmTree_max_width(tw) = XmTreeC_bb_width(node) + 2 * XmHierarchy_h_margin(tw);
    XmTree_max_height(tw) = XmTreeC_bb_height(node) + 2 * XmHierarchy_v_margin(tw);
}

/************************************************************
 *
 *  Code for handling the node queue.
 *
 ************************************************************/

/*	Function Name: UnmapAllExtraNodes
 *	Description:   Correctly unmaps each node in the hierarchy that is
 *                     currently compresed out.
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
	UnmapNode((XmTreeWidget) w, (TreeConstraints) node);
    }

    ptr = XmHierarchyC_children(node);
    for (num = XmHierarchyC_num_children(node), i = 0; i < num; i++, ptr++)
	UnmapAllExtraNodes(w, *ptr);
}

/*	Function Name: MoveNode
 *	Description: This function adds a widget to the movment queue
 *                   but does not move it until it becomes visible.
 *	Arguments: tw - the tree widget.
 *                 node - the node to move.
 *                 x, y - the X and Y location.
 *                 oc_x, oc_y - the X and Y location of the open close button.
 *                 map - map this child?
 *	Returns: none.
 */

static void
MoveNode(XmTreeWidget tw, TreeConstraints node, Position x, Position y,
	   Position oc_x, Position oc_y, Boolean map)
{
    XmTreeC_new_x(node) = x;
    XmTreeC_new_y(node) = y;
    XmTreeC_oc_new_x(node) = oc_x;
    XmTreeC_oc_new_y(node) = oc_y;
    XmTreeC_map(node) = map;
    XmTreeC_move(node) = True;
    XmTreeC_unmap(node) = False;

    _XmListAddBefore(XmTree_child_op_list(tw), NULL, (XtPointer) node);
}

/*	Function Name: UnmapNode
 *	Description: This function adds a widget to the movment queue
 *                   telling it to be unmapped.
 *	Arguments: tw - the tree widget.
 *                 node - the node to move.
 *	Returns: none.
 */

static void
UnmapNode(XmTreeWidget tw, TreeConstraints node)
{
    XmTreeC_map(node) = False;
    XmTreeC_move(node) = False;
    XmTreeC_unmap(node) = True;

    _XmListAddBefore(XmTree_child_op_list(tw), NULL, (XtPointer) node);
}

/*	Function Name: ProcessChildQueue
 *	Description: Processes the child queue.
 *	Arguments: tw - the tree widget.
 *                 vis - the visible rect.
 *	Returns: none.
 */

static void
ProcessChildQueue(XmTreeWidget tw, XRectangle *vis)
{
    XmListElem *elem, *next;

    elem = XmListFirst(XmTree_child_op_list(tw));
    while(elem != NULL) {
	TreeConstraints info = (TreeConstraints) XmListElemData(elem);

	next = XmListElemNext(elem);

	if (CheckWidget(vis, info))
	    _XmListRemove(XmTree_child_op_list(tw), elem);

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
 *        on the screen, or if the new location is on the screen
 *        OR if the node's parent is on the screen
 */

static Boolean
CheckWidget(XRectangle * visible, TreeConstraints node)
{
    if (( ((XmHierarchyC_status(node) & IS_MAPPED) || XmTreeC_map(node)) &&
	 (WidgetInRect(visible, XmHierarchyC_open_close_button(node))    ||
	  WidgetInRect(visible, XmHierarchyC_widget(node))               ||
	  WidgetInRect(visible, XmHierarchyC_parent(node))))             ||
	LocInRect(visible, XmHierarchyC_open_close_button(node),
		  XmTreeC_oc_new_x(node), XmTreeC_oc_new_y(node))        ||
	LocInRect(visible, XmHierarchyC_widget(node),
		    XmTreeC_new_x(node), XmTreeC_new_y(node)))
    {
	ProcessNode(node);
	return(True);
    }
    return(False);
}

/*	Function Name: ProcessNode
 *	Description: Processes the request made of this tree node.
 *	Arguments: node - the Tree node to process.
 *	Returns: none.
 */

static void
ProcessNode(TreeConstraints node)
{
    XmTreeWidgetClass tc;
    Widget w = XmHierarchyC_widget(node);

    if (w == NULL)
	return;

    tc = (XmTreeWidgetClass) XtClass(XtParent(w));

    if (XmTreeC_move(node)) {
	_XmMoveWidget(XmHierarchyC_widget(node),
		      XmTreeC_new_x(node), XmTreeC_new_y(node));

	if (XmHierarchyC_open_close_button(node) != NULL)
	    _XmMoveWidget(XmHierarchyC_open_close_button(node),
			  XmTreeC_oc_new_x(node), XmTreeC_oc_new_y(node));
	XmTreeC_move(node) = False;
    }

    if (XmTreeC_map(node)) {
        XmHierarchyNodeProc map_node;

        _XmProcessLock();
        map_node = tc->hierarchy_class.map_node;
        _XmProcessUnlock();

	    (*map_node)((HierarchyConstraints) node);
    	XmTreeC_map(node) = False;
    }

    if (XmTreeC_unmap(node)) {
    XmHierarchyNodeProc unmap_node;

    _XmProcessLock();
    unmap_node = tc->hierarchy_class.unmap_node;
    _XmProcessUnlock();

	(*unmap_node)((HierarchyConstraints) node);
	XmTreeC_unmap(node) = False;
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

    if (w == NULL)
	return(False);

    x1 = x + w->core.width;
    y1 = y + w->core.height;

    x2 = rect->x + rect->width;
    y2 = rect->y + rect->height;

    return (!((x > x2) || (y > y2) || (x1 < rect->x) || (y1 < rect->y)));
}

/*	Function Name: MoveNodesTimer
 *	Description: processes one node from the movemont queue.
 *	Arguments: data - A Pointer to the tree widget.
 *                 id - th ext interval id.
 *	Returns: none
 */

static Boolean
MoveNodesTimer(XtPointer data)
{
    XmTreeWidget tw = (XmTreeWidget) data;
    XmListElem *elem = XmListFirst(XmTree_child_op_list(tw));

    if (elem != NULL) {
	TreeConstraints node = (TreeConstraints) XmListElemData(elem);
	ProcessNode(node);
	_XmListRemove(XmTree_child_op_list(tw), elem);
	return( False );
    }

    XmHierarchy_work_proc_id(tw) = (XtWorkProcId) NULL;
    return( True );
}

static void LineStyle_confirm (Widget w, int value)
{
	TreeConstraints node = GetNodeInfo(w);
	switch (XmTreeC_line_style(node))
	{
	case LineSolid:
	case LineOnOffDash:
	case LineDoubleDash:
		break;
	default:
		/* error */
		XmTreeC_line_style(node) = value;
		break;
	}
}

/************************************************************
 *
 *  Public functions.
 *
 ************************************************************/

/*	Function Name: XmCreateTree
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateTree(Widget parent, String name,
	     ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmTreeWidgetClass, parent, args, num_args));
}
