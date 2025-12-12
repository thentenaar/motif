/* $XConsortium: CxxGrid.cxx /main/4 1995/07/17 11:45:15 drk $ */
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
/*
 * HISTORY
 */

/********************************** WARNING **********************************
 *
 * ExmGrid is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way
 *********************************** WARNING *********************************/


/******************************************************************************
 *
 * Grid.c - ExmGrid widget.  This widget manages its children as an MxN matrix.
 *          The ExmGrid widget demonstrates how to 
 *             * create a relatively easy Motif manager widget.
 *             * install and use the XmQTspecifyRenderTable trait.
 *             * install and use the XmQTdialogShellSavvy trait. 
 *          See the "OSF/Motif Widget Writer's Guide" for details.
 *
******************************************************************************/

/* Include appropriate header files. */
#include "CxxGrid.h"
#include <Xm/GadgetP.h> /* for gadget management functions */
#include <Xm/TraitP.h>  /* for trait access functions */
#include <Xm/DialogSavvyT.h> /* for XmQTdialogSavvy trait */
#include <Xm/SpecRenderT.h> /* for XmQTspecifyRenderTable trait */

/* Define macros. */
#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define WARNING_TOO_MANY_ROWS "Too many rows specified for ExmCxxGrid.\n" 
#define WARNING_TOO_MANY_COLUMNS "Too many columns specified for ExmCxxGrid.\n"

static XtResource resources[] =
{
    {
	XmNrows, 
	XmCRows, 
	XmRShort, 
	sizeof (short),
	XtOffsetOf(ExmCxxGrid, grid.rows),
	XmRImmediate, (XtPointer) 4
    },
    {
	XmNcolumns, 
	XmCColumns, 
	XmRShort, 
	sizeof (short),
	XtOffsetOf(ExmCxxGrid, grid.columns),
	XmRImmediate, (XtPointer) 4
    },
    {
	XmNmarginWidth, 
	XmCMarginWidth, 
	XmRHorizontalDimension, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxGrid, grid.margin_width),
	XmRImmediate, (XtPointer) 10
    },
    {
	XmNmarginHeight, 
	XmCMarginHeight, 
	XmRVerticalDimension,
	sizeof (Dimension),
	XtOffsetOf(ExmCxxGrid, grid.margin_height),
	XmRImmediate, (XtPointer) 10
    },
    {	
	XmNmapCallback,
	XmCCallback, 
	XmRCallback, 
	sizeof (XtCallbackList),
	XtOffsetOf(ExmCxxGrid, grid.map_callback),
	XmRImmediate, 
	(XtPointer) NULL
    },
    {	
	XmNunmapCallback,
	XmCCallback, 
	XmRCallback, 
	sizeof (XtCallbackList),
	XtOffsetOf(ExmCxxGrid, grid.unmap_callback),
	XmRImmediate, 
	(XtPointer) NULL
    },
    {	
	XmNdefaultPosition,
	XmCDefaultPosition, 
	XmRBoolean, sizeof (Boolean),
	XtOffsetOf(ExmCxxGrid, grid.default_position),
	XmRImmediate,
	(XtPointer) True
    },
    {	
	XmNbuttonRenderTable,
	XmCButtonRenderTable, 
	XmRButtonRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmCxxGrid, grid.button_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNlabelRenderTable,
	XmCLabelRenderTable, 
	XmRLabelRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmCxxGrid, grid.label_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNtextRenderTable,
	XmCTextRenderTable, 
	XmRTextRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmCxxGrid, grid.text_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNdialogTitle,
	XmCDialogTitle, 
	XmRXmString, 
	sizeof (XmString),
	XtOffsetOf(ExmCxxGrid, grid.dialog_title),
	XmRImmediate, (XtPointer) NULL
    },

};

/* Three of the preceding resources will be handled as synthetic 
   resources. */
static XmSyntheticResource syn_resources[] = 
{
    {
	XmNmarginWidth, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxGrid, grid.margin_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
    },
    {
	XmNmarginHeight, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxGrid, grid.margin_height),
	XmeFromVerticalPixels,
	XmeToVerticalPixels
    },
    {	
	XmNdialogTitle,
	sizeof (XmString),
	XtOffsetOf(ExmCxxGrid, grid.dialog_title),
	ExmCxxGrid::GetDialogTitle,
	NULL
     }
};

static XtResource constraints[] =
{
    {
	ExmNgridMarginWidthWithinCell,
	ExmCGridMarginWidthWithinCell,
	XmRHorizontalDimension, sizeof (Dimension),
	XtOffsetOf( struct ExmCxxGridConstraintRec, 
		    grid.grid_margin_width_within_cell),
	XmRImmediate, (XtPointer) 0
    },
    {
	ExmNgridMarginHeightWithinCell,
	ExmCGridMarginHeightWithinCell,
	XmRVerticalDimension,sizeof (Dimension),
	XtOffsetOf( struct ExmCxxGridConstraintRec,
		    grid.grid_margin_height_within_cell),
	XmRImmediate, (XtPointer) 0
    },
};

static XmSyntheticResource syn_constraints[] =
{
    {
        ExmNgridMarginWidthWithinCell,
	sizeof (Dimension),
	XtOffsetOf( struct ExmCxxGridConstraintRec,
		    grid.grid_margin_width_within_cell),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
    },
    {
	ExmNgridMarginHeightWithinCell,
	sizeof (Dimension),
	XtOffsetOf( struct ExmCxxGridConstraintRec,
		    grid.grid_margin_height_within_cell),
	XmeFromVerticalPixels,
	XmeToVerticalPixels
    },
};


/*********************************
 * Class declaration for CxxGrid *
 *********************************/

ExmCxxGridClass::ExmCxxGridClass(char*		name,
				 WidgetClass	parentClass,
				 Cardinal	widgetSize,
				 XtProc		class_init,
				 XtActionList	actions,
				 Cardinal	num_actions,
				 XtResourceList	res,
				 Cardinal	num_res,
				 XtInitProc	cxx_cast)
    : XmCxxManagerClass(name, parentClass, widgetSize, class_init,
			actions, num_actions, res, num_res, cxx_cast)
{
    /*
     * Core class part
     */
    core_class.compress_motion		= TRUE;
    core_class.compress_exposure	= XtExposeCompressMaximal;
    core_class.compress_enterleave	= TRUE;
    /*
     * constraint class part
     * .resources array and .num_resources should only be set for the class
     * implementing them and should not be set in subclasses (Xt job).
     * On the other hand, .constraint_size MUST be set for subclasses, even
     * when they do not define new constraints.
     */
    if (parentClass == (WidgetClass) &xmCxxManagerClassRec) {	
	constraint_class.resources	= constraints;
	constraint_class.num_resources	= XtNumber(constraints);
    }
    constraint_class.constraint_size	= sizeof(ExmCxxGridConstraintRec);

    /*
     * manager class part
     * syn_[constraint_]resources array and num_syn_[constraint_]resources
     * should only be set for the class implementing them and should not be
     * set in subclasses (Xm job).
     */
    manager_class.translations			= XtInheritTranslations;
    if (parentClass == (WidgetClass) &xmCxxManagerClassRec) {	
	manager_class.syn_resources		= syn_resources;
	manager_class.num_syn_resources		= XtNumber(syn_resources);
	manager_class.syn_constraint_resources	= syn_constraints;
	manager_class.num_syn_constraint_resources = XtNumber(syn_constraints);
    }
    manager_class.parent_process = XmInheritParentProcess;
}

externaldef(exmcxxgridclassrec)
ExmCxxGridClass exmCxxGridClassRec("ExmCxxGrid",
				   (WidgetClass) &xmCxxManagerClassRec,
				   sizeof(ExmCxxGrid),
			           ExmCxxGridClass::ClassInitialize,
				   NULL		/*actions*/,
				   0		/*num_actions*/,
				   resources,
				   XtNumber(resources),
			           ExmCxxGrid::_MakeCxxWidget);

externaldef(exmcxxgridwidgetclass)
WidgetClass exmCxxGridWidgetClass = (WidgetClass) &exmCxxGridClassRec;

/*********************************************************************
 *
 * Trait structures
 *
 *********************************************************************/

/* Define trait record variables. */

/* Here is the trait record variable for the XmQTdialogSavvy trait. */ 
static XmConst XmDialogSavvyTraitRec gridDST = {
  0,            /* version */
  ExmCxxGrid::CallMapUnmap, /* trait method */        
};


/* Here is the trait record variable for the XmQTspecifyRenderTable trait. */ 
static XmConst XmSpecRenderTraitRec gridSRTT = {
  0,		/* version */
  ExmCxxGrid::GetTable,     /* trait method */
};

/****************************************************************************
 *
 *  GetDialogTitle
 *      Copy the XmString in XmNdialogTitle before returning it to the user.
 *
 ****************************************************************************/
void ExmCxxGrid::GetDialogTitle(Widget wid, int, XtArgVal *value)
{
   *value = (XtArgVal)XmStringCopy(((ExmCxxGrid *) wid)->grid.dialog_title);
}

			/****************************
			 *                          *
			 * Intrinsics class methods *
			 *                          *
			 ****************************/


/*********************************************************************
 *
 * ClassInitialize
 *
 ********************************************************************/
void ExmCxxGridClass::ClassInitialize() {
    /* set up base_class extension quark */
    exmCxxGridClassRec.SetBaseClassExtensionQuark();
}

/****************************************************************************
 *
 *  ClassPartInitialize:
 *      Called when this widget or a subclass of this widget is instantiated.
 *
 ****************************************************************************/

void ExmCxxGridClass::class_part_initialize()
{
  // Before anything else, do the downward chaining;
  XmCxxManagerClass::class_part_initialize();

  // Install the XmQTdialogShellSavyy trait on this class and on
  // all its future subclasses.
  XmeTraitSet((XtPointer) this, XmQTdialogShellSavvy, (XtPointer) &gridDST);

  // Install the XmQTspecifyRenderTable trait on this class and on
  // all its future subclasses.
  XmeTraitSet((XtPointer) this, XmQTspecifyRenderTable, (XtPointer) &gridSRTT);
}

/************************************************************************
 *
 *  _MakeCxxWidget
 *	Call `new' to make each widget a C++ object
 *
 ************************************************************************/
void ExmCxxGrid::_MakeCxxWidget(Widget /*req_w*/, Widget new_w,
			       ArgList, Cardinal*) {
    ExmCxxGridWidget dummy = IDLE_NEW(new_w) ExmCxxGrid;
}

/************************************************************************
 *
 *  initialize : virtual
 *     Downward chained Xt method :
 *     each class re-implementing initialize() should start by
 *     calling <superclass>::initialize()
 *
 *     Ensure that the width and height are not 0.
 *
 ************************************************************************/
void ExmCxxGrid::initialize(Widget reqw, ArgList args, Cardinal* nargs) {
  ExmCxxGridWidget rw = (ExmCxxGridWidget)reqw;

  /*
   * Before anything else, do the downward chaining;
   */
  XmCxxManager::initialize(reqw, args, nargs);

  /* Initialize one of the internal fields of the ExmGrid widget. */ 
  grid.processing_constraints = False;
    
  /* Ensure that user doesn't specify too many rows. */ 
  if (rw->grid.rows > EXM_GRID_MAX_NUMBER_OF_ROWS) {
    XmeWarning((Widget)rw, WARNING_TOO_MANY_ROWS);
    grid.rows = EXM_GRID_MAX_NUMBER_OF_ROWS;
  }
    
  /* Ensure that user doesn't specify too many columns. */ 
  if (rw->grid.columns > EXM_GRID_MAX_NUMBER_OF_COLUMNS) {
    XmeWarning((Widget)rw, WARNING_TOO_MANY_COLUMNS);
    grid.columns = EXM_GRID_MAX_NUMBER_OF_COLUMNS;
  }

  /* Copy in the dialog title XmString and update our shell */
  if (grid.dialog_title) {   
    grid.dialog_title = XmStringCopy(rw->grid.dialog_title) ;
    XmeSetWMShellTitle(grid.dialog_title, XtParent((Widget) this)) ;
  }
}

/****************************************************************************
 *
 *  Destructor
 *      Called when the widget is destroyed.
 *
 ****************************************************************************/

ExmCxxGrid::~ExmCxxGrid()
{   
  // Free dialog title memory
  XmStringFree(grid.dialog_title);
}


/************************************************************************
 *
 *  Resize 
 *
 ************************************************************************/
void ExmCxxGrid::resize()
{
  /* Configure using current size */
  layout(NULL);
}

/************************************************************************
 *
 *  Redisplay
 *
 ************************************************************************/
void ExmCxxGrid::expose(XEvent *event, Region region)
{
      /* Pass exposure down to gadget children */

  XmeRedisplayGadgets ((Widget) this, event, region);
}

/************************************************************************
 *
 *  set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values() should start by
 *     calling <superclass>::set_values()
 *
 *     Adjust the size of the manager based on shadow thickness changes.
 *
 ************************************************************************/
Boolean ExmCxxGrid::set_values(Widget old_w, Widget request_w, 
			      ArgList args, Cardinal *num_args)
{
    ExmCxxGridWidget cw = (ExmCxxGridWidget)old_w;
    ExmCxxGridWidget rw = (ExmCxxGridWidget)request_w;
    Boolean redisplay = False;
    Boolean need_relayout_flag;

    /*
     * Before anything else, do the downward chaining;
     */
    redisplay = XmCxxManager::set_values(old_w, request_w, args, num_args);


    /* Ensure that user doesn't specify too many rows. */ 
    if (rw->grid.rows > EXM_GRID_MAX_NUMBER_OF_ROWS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_ROWS);
      grid.rows = EXM_GRID_MAX_NUMBER_OF_ROWS;
    }
    
    /* Ensure that user doesn't specify too many rows. */ 
    if (rw->grid.columns > EXM_GRID_MAX_NUMBER_OF_COLUMNS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_COLUMNS);
      grid.columns = EXM_GRID_MAX_NUMBER_OF_COLUMNS;
    }
    
    
    /* See if any class or subclass resources have changed. */
    need_relayout_flag = need_relayout(old_w);

  /* If any geometry resources changed and a new size wasn't specified,
     recalculate a new ideal size. */ 
    if (need_relayout_flag) {
      /* Reset the widget size so that CalcSize can affect them. */
      if (core.width == cw->core.width) 
	core.width = 0;
      if (core.height == cw->core.height) 
	core.height = 0;
	
      /* Call CalcSize. */
      calc_size(NULL, &core.width, &core.height);
	
      /* If the geometry resources have changed but the size hasn't, 
	 we need to relayout manually, because Xt won't generate a 
	 Resize at this point. */
      if ((core.width == cw->core.width) &&
          (core.height == cw->core.height)) {
	layout(NULL); 
	redisplay = True ;
      }
    }

    /* ExmGrid installs the XmQTdialogShellSavvy trait.  Therefore, ExmGrid
       has to process the Xm_DIALOG_SAVVY_FORCE_ORIGIN case, which is as
       follows.  A DialogShell always mimics the child position on itself.
       That is, the "current" position of an ExmGrid within a DialogShell is
       always 0.  Therefore, if an application tries to set ExmGrid's x or
       y position to 0, the Intrinsics will not detect a position change and
       wll not trigger a geometry request.  ExmGrid has to detect this special 
       request and set core.x and core.y to the special value, 
       XmDIALOG_SAVVY_FORCE_ORIGIN.  That is, XmDIALOG_SAVVY_FORCE_ORIGIN
       tells DialogShell that ExmGrid really does want to move to an x or y
       position of 0. */
 
   if (XmIsDialogShell(XtParent((Widget) this)))
     {   /* Is parent a DialogShell? */
       Cardinal i ;

       /* We have to look in the arglist since old_w->core.x is always 0, and 
	  if new_w->core.x is also set to 0, we see no change. */
       for (i=0; i<*num_args; i++) {
         if (strcmp (args[i].name, XmNx) == 0) {
	   if ((args[i].value == 0) && (core.x == 0))
             core.x = XmDIALOG_SAVVY_FORCE_ORIGIN;
	 } 
	 if (strcmp (args[i].name, XmNy) == 0) {
           if ((args[i].value == 0) && (core.y == 0))
             core.y = XmDIALOG_SAVVY_FORCE_ORIGIN;
         }
       } /* end for */
     } /* end of if */

   /* Update wm shell title if it has changed */
   if(grid.dialog_title != cw->grid.dialog_title ) {  
     XmStringFree(cw->grid.dialog_title) ;
     grid.dialog_title = XmStringCopy(rw->grid.dialog_title) ;
     XmeSetWMShellTitle(grid.dialog_title, XtParent((Widget) this)) ;
   }

   return (redisplay);
}

/*************************************************************************
 *
 *  SetValuesAlmost:
 *       Called by the Intrinsics when an XtMakeGeometryRequest call
 *       returns either XmGeometryAlmost or XtGeometryNo.  
 *
 *************************************************************************/
void ExmCxxGrid::set_values_almost(Widget,
				   XtWidgetGeometry* request,
				   XtWidgetGeometry* reply) {
  /* ExmCxxGrid's parent said XtGeometryNo to ExmCxxGrid's geometry request. 
     Therefore, we need to relayout because this request
     was due to a change in internal geometry resource of the ExmCxxGrid */
  if (!reply->request_mode)
    layout(NULL);

  *request = *reply;
}

/************************************************************************
 *
 *  QueryGeometry
 *  	return width X height based on the children preferred sizes
 *
 ************************************************************************/
XtGeometryResult ExmCxxGrid::query_geometry(XtWidgetGeometry *request,
					   XtWidgetGeometry *reply) {
  Widget w = (Widget) this;

  /* If ExmGrid's parent calls XtQueryGeometry before ExmGrid has been 
     realized, use the current size of ExmGrid as the preferred size. */ 
  /* Deal with user initial size setting */
    if (!XtIsRealized(w))  {  /* Widget is not yet realized. */
	reply->width = XtWidth(w) ;    /* might be 0 */
	reply->height = XtHeight(w) ;  /* might be 0 */
    } else {	    /* Widget is realized. */
	/* always computes natural size afterwards */
	reply->width = 0;
	reply->height = 0; 
    }

  /* Call CalcSize to figure out what the preferred size really is. */ 
  calc_size(NULL, &reply->width, &reply->height);

  /* This function handles CWidth and CHeight */
  return XmeReplyToQueryGeometry(w, request, reply) ;
}

/************************************************************************
 *
 *  Geometry Manager
 *	Take the requested geometry, calculate the needed size for
 *	the grid and make a request to the parent.
 *      Requests to change x, y position are always denied.
 *
 ************************************************************************/
XtGeometryResult ExmCxxGrid::geometry_manager(Widget w,
					     XtWidgetGeometry *request,
					     XtWidgetGeometry *) {
  ExmCxxGrid *gw = (ExmCxxGrid *) XtParent(w);
  XtWidgetGeometry parentRequest;
  XtGeometryResult result;
  Dimension curWidth, curHeight, curBW;
  
  /* If the request was caused by ConstraintSetValues reset the flag */
  if (gw->grid.processing_constraints) {
    gw->grid.processing_constraints = False;
    /* The ConstraintSetValues added one to border_width; 
       This is the Xt trick used to fire the GM when a non core 
       geometry resource (like a constraint) changes.
       now take it away. */
    request->border_width -= 1;
  }
  
  /* Save the original child resources. */
  curWidth = w->core.width;
  curHeight = w->core.height;
  curBW = w->core.border_width;
  
  /* Deny any requests for a new position. */
  if ((request->request_mode & CWX) || (request->request_mode & CWY))
    return XtGeometryNo ;
  
  if (request->request_mode & CWWidth) 
    w->core.width = request->width;
  if (request->request_mode & CWHeight) 
    w->core.height = request->height;
  if (request->request_mode & CWBorderWidth) 
    w->core.border_width = request->border_width;

  /* Calculate a new ideal size based on these requests. */ 
  /* Setting width and height to 0 tells CalcSize to override these
     fields with the calculated width and height. */ 
  parentRequest.width = 0;
  parentRequest.height = 0;
  gw -> calc_size(w, &parentRequest.width, &parentRequest.height);
    
  /* Ask the Grid's parent if new calculated size is acceptable. */
  parentRequest.request_mode = CWWidth | CWHeight;
  if (request->request_mode & XtCWQueryOnly)
    parentRequest.request_mode |= XtCWQueryOnly;
  result = XtMakeGeometryRequest ((Widget)gw, &parentRequest, NULL);

  /*  Turn XtGeometryAlmost into XtGeometryNo. */ 
  if (result == XtGeometryAlmost) 
    result = XtGeometryNo;

  if (result == XtGeometryNo || 
      request->request_mode & XtCWQueryOnly) { 
    /* Restore original geometry. */
    w->core.width = curWidth;
    w->core.height = curHeight;
    w->core.border_width = curBW;
  } else {    
    /* result == XtGeometryYes and this wasn't just a query */
    gw -> layout(w); /* Layout with this child as the instigator,
			so that we don't resize this child. */
  }

  return (result);
}

/************************************************************************
 *
 *  ChangeManaged
 *	Process a changed in managed state of the child.  If its
 *	size is out of sync with the parent, make a resize request
 *	to change the size of the grid
 *	Note: called before ConstraintDestroy.
 *
 ************************************************************************/
void ExmCxxGrid::change_managed() {
    Dimension gridWidth, gridHeight;
    Widget w = (Widget) this;

    /* If you get an initial (C) size from the user or application, keep it.  
       Otherwise, just force width and height to 0 so that CalcSize will
       overwrite the appropriate fields. */ 
    if (!XtIsRealized(w))  {
	/* The first time, only attempts to change non specified sizes */
	gridWidth = XtWidth(w) ;   /* might be 0 */
	gridHeight = XtHeight(w) ; /* might be 0 */
    } else {
	gridWidth = 0 ;
	gridHeight = 0 ;
    }

    /* Determine the ideal size of Grid. */
    calc_size(NULL, &gridWidth, &gridHeight);

    /* Ask parent if new size is acceptable */
    while (XtMakeResizeRequest ((Widget) this, gridWidth, gridHeight,
			       &gridWidth, &gridHeight) == XtGeometryAlmost);

    /* Reconfigure children with agreed size */
    layout(NULL);

    /* Update keyboard traversal */
    XmeNavigChangeManaged ((Widget) this);
}

/************************************************************************
 *
 *  constraint_set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing constraint_set_values() should start by
 *     calling <superclass>::constraint_set_values()
 *
 ************************************************************************/
Boolean ExmCxxGrid::constraint_set_values(Widget cw, Widget rw, Widget nw,
					 ArgList args, Cardinal* nargs) 
{
    ExmCxxGridConstraintPart *nc;
    ExmCxxGridConstraintPart *cc;

    /*
     * Before anything else, do the downward chaining;
     */
    Boolean redisplay =
      XmCxxManager::constraint_set_values(cw, rw, nw, args, nargs);

    if (!XtIsRectObj ((Widget) nw)) return (False);

    nc = GridConstraint((Widget) this);
    cc = GridConstraint(cw);


    /* Check for change in ExmNgridMarginWidth or ExmNgridMarginHeight */
    if ((nc->grid_margin_width_within_cell != 
	 cc->grid_margin_width_within_cell ||
	 nc->grid_margin_height_within_cell != 
	 cc->grid_margin_height_within_cell) && 
	XtIsManaged (nw)) {
	/* Tell the Intrinsics and the GeometryManager method that a 
	   reconfigure is needed. */
	grid.processing_constraints = True;
	/* A trick: by altering one of the core geometry fields, Xt will
	   call the parent's geometry_manager method. */ 
	nw->core.border_width += 1;
    }

    return (False);
}

/************************************************************************
 *
 *  Layout
 *
 ************************************************************************/
void ExmCxxGrid::layout(Widget instigator)
{
  Dimension mw = grid.margin_width;
  Dimension mh = grid.margin_height;
  Dimension TotalWidthOfGridWidget  = core.width;
  Dimension TotalWidthOfGridMargins, TotalHeightOfGridMargins;
  Dimension TotalHeightOfGridWidget = core.height;
  Dimension AvailWidthForChildren = 1, AvailHeightForChildren = 1; 
  Dimension WidthAllottedEachChild, HeightAllottedEachChild;
  int i, row, column;

  /* Lay out the children that ExmGrid is currently managing.  
     Each child will be placed somewhere on the rowxcolumn grid. */
  TotalWidthOfGridMargins = 2 * mw;
  if (TotalWidthOfGridWidget > TotalWidthOfGridMargins) 
    AvailWidthForChildren = TotalWidthOfGridWidget - 
      TotalWidthOfGridMargins; 
  
  WidthAllottedEachChild = AvailWidthForChildren / grid.columns;
  
  TotalHeightOfGridMargins = 2 * mh;
  if (TotalHeightOfGridWidget > TotalHeightOfGridMargins) 
    AvailHeightForChildren = TotalHeightOfGridWidget - 
      TotalHeightOfGridMargins; 
  HeightAllottedEachChild = AvailHeightForChildren / grid.rows;
  
  /* Now that we know how much space is allotted for each child, we
     can lay them all out. */
  row = 0;
  column = 0;
  for (i = 0; i < composite.num_children; i++) {
    Widget ic = composite.children[i];
    ExmCxxGridConstraintPart *glc = GridConstraint (ic);
    Dimension gmw = glc->grid_margin_width_within_cell;
    Dimension gmh = glc->grid_margin_height_within_cell;
    Position ChildsStartingX, ChildsStartingY; 
    Dimension ChildsActualWidth, ChildsActualHeight, cb;
    
    if (!XtIsManaged(ic))  
      continue;  /* ignored unmanaged children */
    
    cb = ic->core.border_width;  
    
    /* Calculate the position and the size of the child.
       During the layout, the children are all resized 
       to exactly fit the cell size minus the cell margin */ 
    
    ChildsActualWidth = WidthAllottedEachChild - (2 * (gmw + cb));
    ChildsStartingX = mw + (column * WidthAllottedEachChild) + gmw;
    ChildsStartingY = mh + (row * HeightAllottedEachChild) + gmh;
    ChildsActualHeight = HeightAllottedEachChild - 2 * (gmh + cb);
    
    /* If layout is instigated by the GeometryManager don't 
       configure the requesting child, just set its geometry and 
       let Xt configure it.   */
    if (ic != instigator) {
      XmeConfigureObject (ic, ChildsStartingX, ChildsStartingY, 
			  ChildsActualWidth, ChildsActualHeight, cb);
    }
    else {
      ic->core.x = ChildsStartingX; 
      ic->core.y = ChildsStartingY; 
      ic->core.width = ChildsActualWidth; 
      ic->core.height = ChildsActualHeight; 
      ic->core.border_width = cb;
    }
    
    /* Advance the column counter until we reach the right edge.  
       When we reach the right edge, reset the column 
       counter back to 0 (left edge) and advance the row counter. */ 
    column += 1;
    
    if (column == grid.columns) {
      column = 0;
      row += 1;
    }
  } 
}

/************************************************************************
 *
 *  CalcSize
 *
 ************************************************************************/
void ExmCxxGrid::calc_size(Widget instigator, 
			   Dimension *TotalWidthOfGridWidget,
			   Dimension *TotalHeightOfGridWidget)
{
  Dimension maxWidth = 1;
  Dimension maxHeight = 1;
  int i;
  
  /* Examine each of Grid's children.  Find the biggest child.  The 
     ideal size of the Grid will be large enough to accomodate the
     largest child. */ 
  for (i = 0; i < composite.num_children; i++) {
    Widget ic = composite.children[i];
    ExmCxxGridConstraintPart *glc = GridConstraint (ic);
    Dimension width, height;
    Dimension cw, ch, cb;
    XtWidgetGeometry reply;
    
    if (!XtIsManaged(ic)) 
      continue ;
    
    /* Get child's preferred geometry if not the instigator. */
    if (ic != instigator) {
      XtQueryGeometry (ic, NULL, &reply);
      cw = (reply.request_mode & CWWidth) ? reply.width :
      ic->core.width;
      ch = (reply.request_mode & CWHeight) ? reply.height :
      ic->core.height;
    } else {
      cw = ic->core.width;
      ch = ic->core.height;
    }
    cb = ic->core.border_width;
    
    width = cw + 2 * (cb + glc->grid_margin_width_within_cell);
    height = ch + 2 * (cb + glc->grid_margin_height_within_cell);
    
    maxWidth  = Max (width, maxWidth);
    maxHeight = Max (height, maxHeight);
  } 
  
  
  /* The total width of the grid widget should be set to the width of 
     the largest child widget times the number of columns. */
  if (!*TotalWidthOfGridWidget) {
    *TotalWidthOfGridWidget = maxWidth * grid.columns +
      (2 * (grid.margin_width));
  }
  
  /* The total height of the grid widget should be set to the height of 
     the largest child widget times the number of columns. */
  if (!*TotalHeightOfGridWidget) {
    *TotalHeightOfGridWidget = maxHeight * grid.rows +
      (2 * (grid.margin_height));
  }
}


/****************************************************************************
 *
 *  need_relayout:
 *     Called by SetValues. 
 *     Returns True if a relayout is needed.  
 *     based on this class and all superclass resources' changes. 
 *
 ***************************************************************************/
Boolean ExmCxxGrid::need_relayout(Widget old_w)
{
  ExmCxxGrid *cw = (ExmCxxGrid *) old_w;

    if (grid.margin_width != cw -> grid.margin_width ||
	grid.margin_height != cw -> grid.margin_height ||
	grid.rows != cw -> grid.rows ||
	grid.columns != cw -> grid.columns) {
	return True ;
    } else
	return False ;
}

/*-- Trait methods --*/

/****************************************************************
 *
 * Trait method for XmQTdialogShellSavvy trait. 
 *
 **************************************************************/
void ExmCxxGrid::CallMapUnmap(Widget wid, Boolean map_unmap)
{
    ExmCxxGrid *grid = (ExmCxxGrid *) wid ;
    XmAnyCallbackStruct call_data;	

    call_data.reason = (map_unmap)? XmCR_MAP : XmCR_UNMAP;
    call_data.event  = NULL;			

    if (map_unmap) {
	XtCallCallbackList (wid, grid -> grid.map_callback, 
			    &call_data);
    } else {
	XtCallCallbackList (wid, grid -> grid.unmap_callback, 
			    &call_data);
    }
}

/*****************************************************************
 *
 * Trait method for XmQTspecifyRenderTable.   
 *
 *****************************************************************/
XmRenderTable ExmCxxGrid::GetTable(Widget wid, XtEnum type)
{
    ExmCxxGrid *grid = (ExmCxxGrid *) wid ;

    switch(type) {
    case XmLABEL_RENDER_TABLE : return grid->grid.label_render_table ;
    case XmBUTTON_RENDER_TABLE : return grid->grid.button_render_table ;
    case XmTEXT_RENDER_TABLE : return grid->grid.text_render_table ;
    }
   
    return NULL ;
}


/******************************************************************************
 *
 *  ExmCreateCxxGrid:
 *      Called by an application. 
 *
 *****************************************************************************/
Widget ExmCreateCxxGrid (Widget parent, char *name, 
			 ArgList arglist, Cardinal argcount)
{
 /* This is a convenience function to instantiate an ExmGrid widget. */
   return (XtCreateWidget (name, exmCxxGridWidgetClass, parent, 
                           arglist, argcount));
}

/******************************************************************************
 *
 *  ExmCreateCxxGridDialog
 *      Called by an application to create an ExmGrid managed by a
 *      DialogShell. 
 *
 *****************************************************************************/
Widget ExmCreateCxxGridDialog (Widget parent, char *name,
			       ArgList arglist, Cardinal argcount)
{
   return XmeCreateClassDialog (exmCxxGridWidgetClass,
                                parent, name, arglist, argcount) ; 

}
