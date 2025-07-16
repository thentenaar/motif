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
#include <Xm/IconBoxP.h>
#include <Xm/ExtP.h>

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#define SUPERCLASS ((WidgetClass) &xmManagerClassRec)

/************************************************************
*	MACROS
*************************************************************/
#define GetIconInfo(w) ((IconInfo*) \
           &(((XmIconBoxConstraintsRec*)((char*)((w)->core.constraints)))->icon))


/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

/************************************************************
*	STATIC FUNCTION DECLARATIONS
*************************************************************/

static void ClassInitialize();
static void ClassPartInitialize(WidgetClass w_class);
static void Realize(Widget, Mask *, XSetWindowAttributes *);
static void Resize(Widget), ChangeManaged(Widget), InsertChild(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ConstraintInitialize(Widget, Widget, ArgList, Cardinal *);
static Boolean ConstraintSetValues(Widget, Widget, Widget, ArgList, Cardinal*);

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
					XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget,
				      XtWidgetGeometry *, XtWidgetGeometry *);

/************************
 * Actions and callbacks.
 ************************/

/*********************
 * Internal Routines.
 *********************/

static void FindNearestCellLocation(Widget, Position *, Position *);
static void GetMinCells(Widget, Cardinal *, Cardinal *);
static void PlaceChildren(Widget, Widget);
static void GetMaxCellSize(Widget, Widget, Dimension *, Dimension *);
static void GetCellFromXY(Widget, Position, Position, Position *, Position *);
static void GetXYFromCell(Widget, IconInfo *, Position *, Position *);
static void CalcCellSizes(Widget, Widget,
			  Boolean, Boolean, Dimension *, Dimension *);

static Boolean SetToEmptyCell(Widget);

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

static XtResource resources[] =
{
  {
    XmNminimumVerticalCells, XmCDefaultCells, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.min_v_cells),
    XmRImmediate, (XtPointer) 2
  },
  {
    XmNminimumHorizontalCells, XmCDefaultCells, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.min_h_cells),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNminimumCellWidth, XmCMinimumCellSize, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.min_cell_width),
    XmRImmediate, (XtPointer) 20
  },

  {
    XmNminimumCellHeight, XmCMinimumCellSize, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.min_cell_height),
    XmRImmediate, (XtPointer) 10
  },

  {
    XmNverticalMargin, XmCMargin, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.v_margin),
    XmRImmediate, (XtPointer) 4
  },

  {
    XmNhorizontalMargin, XmCMargin, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconBoxRec, box.h_margin),
    XmRImmediate, (XtPointer) 4
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNhorizontalMargin, sizeof(Dimension),
    XtOffsetOf(XmIconBoxRec, box.h_margin),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNverticalMargin, sizeof(Dimension),
    XtOffsetOf(XmIconBoxRec, box.v_margin),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNminimumCellWidth, sizeof(Dimension),
    XtOffsetOf(XmIconBoxRec, box.min_cell_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNminimumCellHeight, sizeof(Dimension),
    XtOffsetOf(XmIconBoxRec, box.min_cell_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  }
};

static short G_any_cell = XmIconBoxAnyCell;

static XtResource constraints[] =
{
  {
    XmNcellX, XmCCellX, XmRShort,
    sizeof(short), XtOffsetOf(XmIconBoxConstraintsRec, icon.cell_x),
    XmRShort, (XtPointer) &G_any_cell
  },

  {
    XmNcellY, XmCCellY, XmRShort,
    sizeof(short), XtOffsetOf(XmIconBoxConstraintsRec, icon.cell_y),
    XmRShort, (XtPointer) &G_any_cell
  }
};

XmIconBoxClassRec xmIconBoxClassRec = {
  { /* core fields */
    /* superclass		*/	SUPERCLASS,
    /* class_name		*/	"XmIconBox",
    /* widget_size		*/	sizeof(XmIconBoxRec),
    /* class_initialize		*/	ClassInitialize,
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
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	NULL,
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
    /* insert_child       */      InsertChild,
    /* delete_child       */      XtInheritDeleteChild,
    /* extension          */      NULL,
   },
   {		/* constraint_class fields */
    /* resource list        */         (XtResource*)constraints,
    /* num resources        */         XtNumber(constraints),
    /* constraint size      */         sizeof(XmIconBoxConstraintsRec),
    /* init proc            */         ConstraintInitialize,
    /* destroy proc         */         NULL,
    /* set values proc      */         ConstraintSetValues,
    /* extension            */         NULL,
   },
   {		/* manager_class fields */
    /* default translations   */      XtInheritTranslations,
    /* syn_resources          */      get_resources,
    /* num_syn_resources      */      XtNumber(get_resources),
    /* syn_cont_resources     */      NULL,
    /* num_syn_cont_resources */      0,
    /* parent_process         */      XmInheritParentProcess,
    /* extension	      */      NULL,
   },
  { /* Icon Box fields */
      NULL                      /* extension          */
  }
};

WidgetClass xmIconBoxWidgetClass = (WidgetClass)&xmIconBoxClassRec;

/************************************************************
*	STATIC CODE
*************************************************************/

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
ClassInitialize()
{
  /* do nothing */
}

static void
Initialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) set;

    /*
     * This is needed so that the right thing happens if an icon box is
     * created w/o any children.
     */

    CalcCellSizes(set, NULL, FALSE, FALSE,
		  &(XmIconBox_cell_width(ibw)), &(XmIconBox_cell_height(ibw)));
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
Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
    XtCreateWindow (w, InputOutput, CopyFromParent, *valueMask, attributes);
}

/*	Function Name: Resize
 *	Description:   Called when this widget has been resized.
 *	Arguments:     w - the widget to resize.
 *	Returns:       none.
 */

static void
Resize(Widget w)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    CalcCellSizes(w, NULL, TRUE, FALSE,
		  &(XmIconBox_cell_width(ibw)), &(XmIconBox_cell_height(ibw)));
    PlaceChildren(w, NULL);
}

/*	Function Name: SetValues
 *	Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the set values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    XmIconBoxWidget old_ibw = (XmIconBoxWidget) current;
    XmIconBoxWidget set_ibw = (XmIconBoxWidget) set;

    if ((XmIconBox_min_v_cells(old_ibw) != XmIconBox_min_v_cells(set_ibw))           ||
	(XmIconBox_min_h_cells(old_ibw) != XmIconBox_min_h_cells(set_ibw))           ||
	(XmIconBox_min_cell_width(old_ibw) != XmIconBox_min_cell_width(set_ibw))     ||
	(XmIconBox_min_cell_height(old_ibw) != XmIconBox_min_cell_height(set_ibw))   ||
	(XmIconBox_v_margin(old_ibw) != XmIconBox_v_margin(set_ibw))                 ||
	(XmIconBox_h_margin(old_ibw) != XmIconBox_h_margin(set_ibw)) )
    {
	CalcCellSizes(set, NULL, FALSE, FALSE,
		      &(XmIconBox_cell_width(set_ibw)), &(XmIconBox_cell_height(set_ibw)));
	PlaceChildren(set, NULL);
    }

    return(FALSE);
}

/*	Function Name: QueryGeometry
 *	Description:   Called when my parent wants to know what size
 *                     I would like to be.
 *	Arguments:     w - the widget to check.
 *                     indended - constriants imposed by the parent.
 *                     preferred - what I would like.
 *	Returns:       See Xt Manual.
 */

static XtGeometryResult
QueryGeometry(Widget w,XtWidgetGeometry *intended, XtWidgetGeometry *preferred)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;
    Cardinal min_x, min_y;
    Dimension max_w, max_h;

    GetMinCells(w, &min_x, &min_y);
    GetMaxCellSize(w, NULL, &max_w, &max_h);

    min_x++;
    min_y++;

    preferred->width = XmIconBox_h_margin(ibw) + min_x * (max_w + XmIconBox_h_margin(ibw));
    preferred->height= XmIconBox_v_margin(ibw) + min_y * (max_h + XmIconBox_v_margin(ibw));

    return(_XmHWQuery(w, intended, preferred));
}

/************************************************************
 *
 * Composite and Constraint Information.
 *
 ************************************************************/

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
    Dimension cwidth, cheight;
    XmIconBoxWidget ibw = (XmIconBoxWidget) XtParent(w);
    IconInfo * info = GetIconInfo(w);
    Boolean w_req = (request->request_mode & CWWidth);
    Boolean h_req = (request->request_mode & CWHeight);
    Boolean x_req = (request->request_mode & CWX);
    Boolean y_req = (request->request_mode & CWY);

    if (!(request->request_mode & (CWWidth | CWHeight | CWX | CWY)))
	return(XtGeometryNo);

    result->request_mode = 0;

    if (w_req || h_req) {
	CalcCellSizes((Widget) ibw, w, FALSE, TRUE, &cwidth, &cheight);

	if(w_req)
	    ASSIGN_MAX(cwidth, request->width);
	else
	    ASSIGN_MAX(cwidth, w->core.width);

	if(h_req)
	    ASSIGN_MAX(cheight, request->height);
	else
	    ASSIGN_MAX(cheight, w->core.height);

	/*
	 * Do not allow an x, y width and height request at the same time.
	 * since it is unclear what this would mean.  I want to place the
	 * widget in the cell the center of it is over.  With a multiple
	 * request like this is it tough to find out which cell to use
	 * since there are two reasonable values for height and width.
	 */

	result->x = w->core.x;
	result->y = w->core.y;
	result->width  = cwidth;
	result->height = cheight;
	result->request_mode |= CWX | CWY | CWHeight | CWWidth;
    }
    else if ( x_req || y_req ) {
	Position x, y;
	short cell_x, cell_y;

	if (x_req)
	    x = request->x;
	else
	    x = w->core.x;

	if (y_req)
	    y = request->y;
	else
	    y = w->core.y;

	FindNearestCellLocation((Widget) ibw, &x, &y);

	GetCellFromXY((Widget) ibw, x, y, &cell_x, &cell_y);
	if (XmIconBoxIsCellEmpty((Widget) ibw, cell_x, cell_y, w)) {
	    result->x = x;
	    result->y = y;
	    result->request_mode |= CWX | CWY;
	}
	else			/* Cell is full, return NO. */
	    return(XtGeometryNo);
    }

    if (((request->x == result->x) || !x_req) &&
	((request->y == result->y) || !y_req) &&
	((request->width == result->width) || !w_req) &&
	((request->height == result->height) || !h_req))
    {
	if (request->request_mode &
	    (CWBorderWidth | CWStackMode | CWSibling))
	{
	    return(XtGeometryAlmost);
	}

	if (request->request_mode & XtCWQueryOnly)
	    return(XtGeometryYes);

	if (w_req || h_req) {
	    if (w_req)
		info->pref_width = w->core.width = request->width;

	    if (h_req)
		info->pref_height = w->core.height = request->height;
	}
	else {
	    /*
	     * NOTE: We are assuming here that the cell height/width
	     * did not change.  This is valid because this code is
	     * only executed if w_req and h_req are false.
	     */

	    GetCellFromXY((Widget) ibw, result->x, result->y,
			  &(info->cell_x), &(info->cell_y));
	}

	CalcCellSizes((Widget) ibw, NULL, FALSE, FALSE,
		      &(XmIconBox_cell_width(ibw)), &(XmIconBox_cell_height(ibw)));

	PlaceChildren((Widget) ibw, w);
	return(XtGeometryYes);
    }

    info->pref_width = info->pref_height = 0; /* invalidate cache. */

    if (((request->x == result->x) || !x_req)         ||
	((request->y == result->y) || !y_req)         ||
	((request->width == result->width) || !w_req) ||
	((request->height == result->height) || !h_req))
    {
	return(XtGeometryAlmost);
    }
    else
	return(XtGeometryNo);
}

/*	Function Name: InsertChild
 *	Description: called when a new child is to be added.
 *	Arguments: w - the new child.
 *	Returns: none.
 *
 * This routine simply makes sure that no gadgets are added.
 */

static void
InsertChild(Widget w)
{
   if (_XmGadgetWarning(w))
       return;

   {
      XtWidgetProc insert_child;

      _XmProcessLock();
      insert_child = ( (CompositeWidgetClass) SUPERCLASS)->composite_class.insert_child;
      _XmProcessUnlock();

      (*insert_child)(w);
   }
}

/*	Function Name: ChangeManaged
 *	Description:   When a management change has occured...
 *	Arguments:     w - the icon box widget.
 *	Returns:       none.
 */

static void
ChangeManaged(Widget w)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;
    Widget * childp;

    CalcCellSizes(w, NULL, FALSE, TRUE,
		  &(XmIconBox_cell_width(ibw)), &(XmIconBox_cell_height(ibw)));

    ForAllChildren(ibw, childp) {
	IconInfo * info = GetIconInfo(*childp);

	if ((info->cell_x != XmIconBoxAnyCell) &&
	    (info->cell_y != XmIconBoxAnyCell) &&
	    !XmIconBoxIsCellEmpty((Widget) ibw,
				  info->cell_x, info->cell_y, *childp))
	{
	    static String params[1];
	    Cardinal num = 1;
	    char buf[BUFSIZ];

	    params[0] = buf;
	    snprintf(buf, BUFSIZ, "(%d, %d)", info->cell_x, info->cell_y);

	    _XmWarningMsg(w, XmNcellNotEmpty,
		    XmNcellNotEmptyMsg, params, num);
	    /*
	     * tell it to reset this to an empty cell.
	     */

	    info->cell_y = XmIconBoxAnyCell;
	}

	if ((info->cell_x == XmIconBoxAnyCell) ||
	    (info->cell_y == XmIconBoxAnyCell))
	{
	    Position x = (*childp)->core.x;
	    Position y = (*childp)->core.y;
	    Position cell_x, cell_y;

	    /*
	     * If the cell location is not specified try to find the
	     * cell nearest the X and Y coords specified.
	     */

	    FindNearestCellLocation((Widget) ibw, &x, &y);
	    GetCellFromXY((Widget) ibw, x, y, &cell_x, &cell_y);

	    if (XmIconBoxIsCellEmpty((Widget) ibw, cell_x, cell_y, w))
	    {
		info->cell_x = cell_x;
		info->cell_y = cell_y;
	    }
	    /*
	     * If this cell is full the just find any empty cell.
	     */
	    else if (!SetToEmptyCell(*childp)) {
		XmeWarning(w, XmNnoEmptyCellsMsg);
	    }
	}
    }

    CalcCellSizes(w, NULL, FALSE, FALSE,
		  &(XmIconBox_cell_width(ibw)), &(XmIconBox_cell_height(ibw)));

    PlaceChildren(w, NULL);

   XmeNavigChangeManaged(w);	/* For Motif navigation */
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmICONBOX_BIT);
}

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
    IconInfo * info = GetIconInfo(set);

    info->pref_width = req->core.width;
    info->pref_height = req->core.height;
}

/*	Function Name: ConstraintSetValues
 *	Description:   Called when some constraint data needs to be modified
 *                     on-the-fly.
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
    IconInfo * set_info = GetIconInfo(set);
    IconInfo * old_info = GetIconInfo(current);

    if (set->core.width != set_info->pref_width)
	set_info->pref_width = 0;

    if (set->core.height != set_info->pref_height)
	set_info->pref_height = 0;

    if ((set_info->cell_x != old_info->cell_x) ||
	(set_info->cell_y != old_info->cell_y))
    {
	if ( XmIconBoxIsCellEmpty(XtParent(set),
				  set_info->cell_x, set_info->cell_y, set))
	{
	    GetXYFromCell(XtParent(set),
			  set_info, &(set->core.x), &(set->core.y));
	}
	else {
	    static String params[1];
	    Cardinal num = 1;
	    char buf[BUFSIZ];

	    params[0] = buf;
	    snprintf(buf, BUFSIZ, "(%d, %d)", set_info->cell_x, set_info->cell_y);

	    _XmWarningMsg(set, XmNcellNotEmpty,
		    XmNcellNotEmptyMsg, params, num);

	    set_info->cell_x = old_info->cell_x;
	    set_info->cell_y = old_info->cell_y;
	}
    }

    return(False);
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

/*	Function Name: GetCellFromXY
 *	Description:   Gets the cell located at this location.
 *	Arguments:     w - the icon box.
 *                     x, y - the coordinates in X space.
 * RETURNED            cell_x, cell_y - the corrdinates in cell space.
 *	Returns:       none.
 */

static void
GetCellFromXY(Widget w,
	      Position x, Position y, Position * cell_x, Position * cell_y)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    *cell_x = (int)x / (int)(XmIconBox_cell_width(ibw) + XmIconBox_h_margin(ibw));
    *cell_y = (int)y / (int)(XmIconBox_cell_height(ibw) + XmIconBox_v_margin(ibw));
}

/*	Function Name: Find Nearest Cell Location
 *	Description: Finds the nearest cell
 *	Arguments: ibw - the icon box widget.
 * IN/OUT          x, y - Used as an input for the current location
 *                        is output with the new location.
 *	Returns: none
 */

static void
FindNearestCellLocation(Widget w, Position *x, Position *y)
{
    IconInfo temp;
    Dimension width, height;
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    width = XmIconBox_cell_width(ibw);
    height = XmIconBox_cell_height(ibw);

    GetCellFromXY(w, *x + width/2,
		  *y + height/2, &(temp.cell_x), &(temp.cell_y));

    GetXYFromCell(w, &temp, x, y);
}

/*	Function Name: GetXYFromCell
 *	Description:   Gets the X and Y position for this cell.
 *	Arguments:     info - the cell info.
 * RETURNED            x, y - the coordinates in X space.
 *	Returns:       none.
 */

static void
GetXYFromCell(Widget w, IconInfo * info, Position * x, Position * y)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;
    Position x_temp = (info->cell_x < 0) ? 0 : info->cell_x;
    Position y_temp = (info->cell_y < 0) ? 0 : info->cell_y;

    *x = XmIconBox_h_margin(ibw) +
	 x_temp * (XmIconBox_cell_width(ibw) + XmIconBox_h_margin(ibw));

    *y = XmIconBox_v_margin(ibw) +
	 y_temp * (XmIconBox_cell_height(ibw) + XmIconBox_v_margin(ibw));
}

/*	Function Name: PlaceChildren
 *	Description:   Places all managed children correctly.
 *	Arguments:     w - the icon box widget.
 *                     child - set attributes rather than configure this child.
 *	Returns:       none.
 */

static void
PlaceChildren(Widget w, Widget child)
{
    Widget * childp;
    Position x, y;
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    ForAllChildren(ibw, childp) {
	if (!XtIsManaged(*childp))
	    continue;

	GetXYFromCell(w, GetIconInfo(*childp), &x, &y);

	if (*childp == child) {
	    child->core.x = x;
	    child->core.y = y;
	    child->core.width = XmIconBox_cell_width(ibw);
	    child->core.height = XmIconBox_cell_height(ibw);
	}
	else {
	    _XmConfigureWidget(*childp, x, y,
			       XmIconBox_cell_width(ibw), XmIconBox_cell_height(ibw),
			       (*childp)->core.border_width);
	}
    }
}

/*	Function Name: CalcCellSizes
 *	Description:   Calculates the height and width of each cell.
 *	Arguments:     w - the icon box widget.
 *                     ignore - ignore this child when calculating cell sizes.
 *                     noresize - If true then never attempt a resize.
 *                     query_only - only ask, don't change anything.
 *                     cell_width, cell_height - new size of each cell.
 *	Returns:       none.
 */

static void
CalcCellSizes(Widget w, Widget ignore, Boolean noresize, Boolean query_only,
	      Dimension * cell_width, Dimension * cell_height)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    Cardinal min_x, min_y;
    Dimension max_w, max_h, d_width, d_height, width, height;

    GetMinCells(w, &min_x, &min_y);
    GetMaxCellSize(w, ignore, &max_w, &max_h);

    min_x++;
    min_y++;

    d_width = XmIconBox_h_margin(ibw) + min_x * (max_w + XmIconBox_h_margin(ibw));
    d_height = XmIconBox_v_margin(ibw) + min_y * (max_h + XmIconBox_v_margin(ibw));

    if (noresize ||
	(_XmRequestNewSize(w, query_only, d_width, d_height,
			   &width, &height) != XtGeometryYes))
    {
	if (noresize) {
	    width = w->core.width;
	    height = w->core.height;
	}

	/*
	 * We may need to adjust the cell size.
	 */

	if (width < d_width)
	    max_w = (width - XmIconBox_h_margin(ibw))/ min_x - XmIconBox_h_margin(ibw);
	if (height < d_height)
	    max_h = (height - XmIconBox_v_margin(ibw))/ min_y - XmIconBox_v_margin(ibw);
    }

    *cell_width = max_w;
    *cell_height = max_h;
}

/*	Function Name: GetMinCells
 *	Description:   Returns the minimum number of cells that should
 *                     be displayed in each direction.
 *	Arguments:     w - the Icon Box widget.
 * RETURN              min_x, min_y - minimum number of cells needed
 *                                    in each direction.
 *
 *	Returns:
 */

static void
GetMinCells(Widget w, Cardinal * min_x, Cardinal * min_y)
{
    Widget * childp;
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;

    *min_x = XmIconBox_min_h_cells(ibw) - 1;
    *min_y = XmIconBox_min_v_cells(ibw) - 1;

    ForAllChildren(ibw, childp) {
	Position x, y;
	IconInfo * info;

	if (!XtIsManaged(*childp))
	    continue;

	info = GetIconInfo(*childp);

	x = (info->cell_x < 0) ? 0 : info->cell_x;
	y = (info->cell_y < 0) ? 0 : info->cell_y;

	if (x > *min_x)
	    *min_x = x;

	if (y > *min_y)
	    *min_y = y;
    }
}

/*	Function Name: GetMaxCellSize
 *	Description:   Gets the maximum size of each cell.
 *	Arguments:     w - the Icon Box widget.
 *                     ignore - ignore this child.
 *                     max_w, max_h - the maximum size of each cell.
 *	Returns:       none.
 */

static void
GetMaxCellSize(Widget w, Widget ignore, Dimension * max_w, Dimension * max_h)
{
    Widget * childp;
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;
    XtWidgetGeometry preferred;
    register Dimension temp;

    *max_w = XmIconBox_min_cell_width(ibw);
    *max_h = XmIconBox_min_cell_height(ibw);

    ForAllChildren(ibw, childp) {
	IconInfo * info = GetIconInfo(*childp);

	if (!XtIsManaged(*childp) || (*childp == ignore))
	    continue;

	if ((info->pref_width != 0) && (info->pref_height != 0)) {
	    preferred.width = info->pref_width;
	    preferred.height = info->pref_height;
	    preferred.border_width = (*childp)->core.border_width;
	}
	else {
	    (void) XtQueryGeometry(*childp, NULL, &preferred);
	    info->pref_width = preferred.width;
	    info->pref_height = preferred.height;
	}

	temp = preferred.width + 2 * preferred.border_width;
	ASSIGN_MAX(*max_w, temp);

	temp = preferred.height + 2 * preferred.border_width;
	ASSIGN_MAX(*max_h, temp);
    }
}

/*	Function Name: SetToEmptyCell
 *	Description:   Puts the widget passed into an empty node as
 *                     close to 0, 0 as possible.
 *	Arguments:     child - the child.
 *	Returns:       True if an empty cell was found.
 */

static Boolean
SetToEmptyCell(Widget child)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) XtParent(child);

    register Position x, y, cur_x, cur_y;
    register unsigned long square, cur_square;
    Cardinal max_x, max_y;

    GetMinCells((Widget) ibw, &max_x, &max_y);

    cur_x = cur_y = XmIconBoxAnyCell;
    cur_square = max_x * max_x + max_y * max_y;

    for (y = 0; y <= max_y; y++)
	for (x = 0; x <= max_x; x++) {
	    square = x * x + y * y;

	    if (square <= cur_square &&
		XmIconBoxIsCellEmpty(XtParent(child), x, y, NULL)) {
		cur_square = square;
		cur_x = x;
		cur_y = y;
		break;
	    }
	    else if (square >= cur_square)
		continue;
	}

    if (cur_x == XmIconBoxAnyCell) {
	IconInfo * info = GetIconInfo(child);

	info->cell_x = 0;
	info->cell_y = max_y + 1;
    }
    else {
	IconInfo * info = GetIconInfo(child);

	info->cell_x = cur_x;
	info->cell_y = cur_y;
    }

    return(TRUE);
}

/************************************************************
 *
 * Public Functions.
 *
 ************************************************************/

/*	Function Name: XmIconBoxIsCellEmpty
 *	Description:   Returns true if this cell is unused.
 *	Arguments:     w - the icon box.
 *                     x, y - cell to check.
 *                     ignore - ignore this widget when checking.
 *	Returns:       Returns true if this cell is unused.
 */

Boolean
XmIconBoxIsCellEmpty(Widget w, Position x, Position y, Widget ignore)
{
    XmIconBoxWidget ibw = (XmIconBoxWidget) w;
    Widget * childp;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    ForAllChildren(ibw, childp) {
	IconInfo * info;

	if (!XtIsManaged(*childp) ||
	    (*childp == ignore) || (*childp)->core.being_destroyed)
	{
	    continue;
	}

	info = GetIconInfo(*childp);
	if ((x == info->cell_x) && (y == info->cell_y))
	  {
	    _XmAppUnlock(app);
	    return(False);
	  }
    }

    _XmAppUnlock(app);
    return(True);
}

/*	Function Name: XmCreateIconBox
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateIconBox(Widget parent, String name,
		ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmIconBoxWidgetClass, parent, args, num_args));
}
