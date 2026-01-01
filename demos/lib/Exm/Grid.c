/* $XConsortium: Grid.c /main/5 1995/07/15 20:40:45 drk $ */
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
#include <Exm/GridP.h>  /* private header file for the ExmGrid widget */
#include <Xm/GadgetP.h> /* for gadget management functions */
#include <Xm/TraitP.h>  /* for trait access functions */
#include <Xm/DialogSavvyT.h> /* for XmQTdialogSavvy trait */
#include <Xm/SpecRenderT.h> /* for XmQTspecifyRenderTable trait */


/* Define macros. */
#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define WARNING_TOO_MANY_ROWS "Too many rows specified for ExmGrid.\n" 
#define WARNING_TOO_MANY_COLUMNS "Too many columns specified for ExmGrid.\n"


/* Declare static functions. */
static void GetDialogTitle( 
                        Widget bb,
                        int resource,
                        XtArgVal *value) ;
static void ClassPartInitialize(
                        WidgetClass widgetClass);
static void Initialize(
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void Destroy( 
                        Widget wid) ;
static void Resize (
                        Widget w );
static void Redisplay (
                        Widget w,
                        XEvent *event,
                        Region region);
static Boolean SetValues (
                        Widget old_w,
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void SetValuesAlmost( 
                        Widget cw,
                        Widget nw,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static XtGeometryResult QueryGeometry (
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply);
static XtGeometryResult GeometryManager (
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply);
static void ChangeManaged (
                        Widget w);
static Boolean ConstraintSetValues (
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void Layout (
                        Widget wid,
                        Widget instigator);
static void CalcSize (
                        Widget wid,
                        Widget instigator,
                        Dimension *gridWidth,
                        Dimension *gridHeight);
static Boolean NeedRelayout (
                        Widget new,
                        Widget cur);
static void CallMapUnmap(Widget wid,
			 Boolean map_unmap) ;
static XmRenderTable GetTable(Widget wid,
			   XtEnum type);


/* No translations and no actions. */


/* Define the resources for the ExmGrid widget. */ 
static XtResource resources[] =
{
    {
	XmNrows, 
	XmCRows, 
	XmRShort, 
	sizeof (short),
	XtOffsetOf( ExmGridRec, grid.rows),
	XmRImmediate, 
        (XtPointer) 4
    },
    {
	XmNcolumns, 
	XmCColumns, 
	XmRShort, 
	sizeof (short),
	XtOffsetOf( ExmGridRec, grid.columns),
	XmRImmediate, 
        (XtPointer) 4
    },
    {
	XmNmarginWidth, 
	XmCMarginWidth, 
	XmRHorizontalDimension, 
	sizeof (Dimension),
	XtOffsetOf( ExmGridRec, grid.margin_width),
	XmRImmediate, 
        (XtPointer) 10
    },
    {
	XmNmarginHeight, 
	XmCMarginHeight, 
	XmRVerticalDimension,
	sizeof (Dimension),
	XtOffsetOf(ExmGridRec, grid.margin_height),
	XmRImmediate, 
        (XtPointer) 10
    },
    {	
	XmNmapCallback,
	XmCCallback, 
	XmRCallback, 
	sizeof (XtCallbackList),
	XtOffsetOf(ExmGridRec, grid.map_callback),
	XmRImmediate, 
	(XtPointer) NULL
    },
    {	
	XmNunmapCallback,
	XmCCallback, 
	XmRCallback, 
	sizeof (XtCallbackList),
	XtOffsetOf(ExmGridRec, grid.unmap_callback),
	XmRImmediate, 
	(XtPointer) NULL
    },
    {	
	XmNdefaultPosition,
	XmCDefaultPosition, 
	XmRBoolean, sizeof (Boolean),
	XtOffsetOf(ExmGridRec, grid.default_position),
	XmRImmediate,
	(XtPointer) True
    },
    {	
	XmNbuttonRenderTable,
	XmCButtonRenderTable, 
	XmRButtonRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmGridRec, grid.button_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNlabelRenderTable,
	XmCLabelRenderTable, 
	XmRLabelRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmGridRec, grid.label_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNtextRenderTable,
	XmCTextRenderTable, 
	XmRTextRenderTable, 
	sizeof (XmRenderTable),
	XtOffsetOf(ExmGridRec, grid.text_render_table),
	XmRCallProc, (XtPointer) NULL
    },
    {	
	XmNdialogTitle,
	XmCDialogTitle, 
	XmRXmString, 
	sizeof (XmString),
	XtOffsetOf(ExmGridRec, grid.dialog_title),
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
	XtOffsetOf( ExmGridRec, grid.margin_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
    },
    {
	XmNmarginHeight, 
	sizeof (Dimension),
	XtOffsetOf( ExmGridRec, grid.margin_height),
	XmeFromVerticalPixels,
	XmeToVerticalPixels
    },
    {	
	XmNdialogTitle,
	sizeof (XmString),
	XtOffsetOf( ExmGridRec, grid.dialog_title),
	GetDialogTitle,
	NULL
     }
};


/* Define the two constraints of ExmGrid. */
static XtResource constraints[] =
{
    {
        ExmNgridMarginWidthWithinCell,
        ExmCGridMarginWidthWithinCell,
        XmRHorizontalDimension, 
        sizeof (Dimension),
        XtOffsetOf( ExmGridConstraintRec, grid.grid_margin_width_within_cell),
        XmRImmediate, 
        (XtPointer) 0
    },
    {
        ExmNgridMarginHeightWithinCell,
        ExmCGridMarginHeightWithinCell,
        XmRVerticalDimension,
        sizeof (Dimension),
        XtOffsetOf( ExmGridConstraintRec, grid.grid_margin_height_within_cell),
        XmRImmediate, 
        (XtPointer) 0
    },
};


/* Both of the preceding constraints will be handled as synthetic 
   constraints. */
static XmSyntheticResource syn_constraints[] =
{
    {
        ExmNgridMarginWidthWithinCell,
        sizeof (Dimension),
        XtOffsetOf( ExmGridConstraintRec, grid.grid_margin_width_within_cell),
        XmeFromHorizontalPixels,
        XmeToHorizontalPixels
    },
    {
        ExmNgridMarginHeightWithinCell,
        sizeof (Dimension),
        XtOffsetOf( ExmGridConstraintRec, grid.grid_margin_height_within_cell),
        XmeFromVerticalPixels,
        XmeToVerticalPixels
    },
};



/* Define the widget class record.  See Chapter 4 of the 
   "OSF/Motif Widget Writer's Guide" for details. */ 
externaldef(exmgridclassrec) ExmGridClassRec exmGridClassRec = 
{ 
  { /* Here is the Core class record. */ 
    /* superclass */                 (WidgetClass) &xmManagerClassRec, 
    /* class_name */                 "ExmGrid",
    /* widget_size */                sizeof(ExmGridRec),
    /* class_initialize */           NULL,
    /* class_part_initialize */      ClassPartInitialize, 
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    NULL,
    /* num_actions */                0,
    /* resources */                  resources,
    /* num_resources */              XtNumber(resources),
    /* xrm_class */                  NULLQUARK,
    /* compress_motion */            TRUE,
    /* compress_exposure */          XtExposeCompressMaximal,
    /* compress_enterleave */        TRUE,
    /* visible_interest */           FALSE,
    /* destroy */                    Destroy,
    /* resize */                     Resize,
    /* expose */                     Redisplay,
    /* set_values */                 SetValues,
    /* set_values_hook */            NULL,
    /* set_values_almost */          SetValuesAlmost,
    /* get_values_hook */            NULL,
    /* accept_focus */               NULL,
    /* version */                    XtVersion,
    /* callback_private */           NULL,
    /* tm_table */                   XtInheritTranslations,
    /* query_geometry */             QueryGeometry,
    /* display_accelerator */        NULL,
    /* extension */                  NULL,
  },    
  { /* Here is the Composite class record. */ 
    /* geometry_manager */           GeometryManager,
    /* change_managed */             ChangeManaged,
    /* insert_child */               XtInheritInsertChild,
    /* delete_child */               XtInheritDeleteChild,
    /* extension */                  NULL,
  },    
  { /* Here is the Constaint class record. */ 
    /* constraint_resources */       constraints,
    /* constraint_num_resources */   XtNumber(constraints),
    /* constraint_size */            sizeof(ExmGridConstraintRec), 
    /* constraint_initialize */      NULL,
    /* constraint_destroy */         NULL,
    /* constraint_set_values */      ConstraintSetValues,
    /* extension */                  NULL,
  },    
  { /* Here is the XmManager class record. */ 
    /* translations */               XtInheritTranslations,
    /* syn_resources */              syn_resources,
    /* num_syn_resources */          XtNumber(syn_resources),
    /* syn_constraint_resources */   syn_constraints,
    /* num_syn_constraint_resources */ XtNumber(syn_constraints),
    /* parent_process */             XmInheritParentProcess,
    /* extension */                  NULL,
  },    
  { /* Here is the ExmGrid class record. */ 
    /* layout */                     Layout,
    /* calc_size */                  CalcSize,
    /* need_relayout */              NeedRelayout,
    /* extension */                  NULL,
  }    
};    

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */
externaldef(exmgridwidgetclass) WidgetClass exmGridWidgetClass =
    (WidgetClass) &exmGridClassRec;


             /* Define trait record variables. */

/* Here is the trait record variable for the XmQTdialogSavvy trait. */ 
static const XmDialogSavvyTraitRec gridDST = {
  0,            /* version */
  CallMapUnmap, /* trait method */        
};


/* Here is the trait record variable for the XmQTspecifyRenderTable trait. */ 
static const XmSpecRenderTraitRec gridSRTT = {
  0,		/* version */
  GetTable,     /* trait method */
};


/****************************************************************************
 *
 *  GetDialogTitle
 *      Copy the XmString in XmNdialogTitle before returning it to the user.
 *
 ****************************************************************************/
static void
GetDialogTitle(
        Widget wid,
        int resource,		
        XtArgVal *value)
{
   *value = (XtArgVal)XmStringCopy(((ExmGridWidget) wid)->grid.dialog_title);
}


/****************************************************************************
 *
 *  ClassPartInitialize:
 *      Called when this widget or a subclass of this widget is instantiated.
 *
 ****************************************************************************/
static void
ClassPartInitialize (
        WidgetClass widgetClass
                    )
{
 ExmGridWidgetClass wc = (ExmGridWidgetClass)widgetClass;
 ExmGridWidgetClass sc = (ExmGridWidgetClass)wc->core_class.superclass;

  /* The following code allows subclasses of ExmGrid to inherit three of  
     ExmGrid's methods. */
    if (wc->grid_class.layout == ExmInheritLayout)
	wc->grid_class.layout = sc->grid_class.layout;
    if (wc->grid_class.calc_size == ExmInheritCalcSize)
	wc->grid_class.calc_size = sc->grid_class.calc_size;
    if (wc->grid_class.need_relayout == ExmInheritNeedRelayout)
	wc->grid_class.need_relayout = sc->grid_class.need_relayout;
   
  /* Install the XmQTdialogShellSavyy trait on this class and on
     all its future subclasses. */
    XmeTraitSet(widgetClass, XmQTdialogShellSavvy, (XtPointer) &gridDST);

  /* Install the XmQTspecifyRenderTable trait on this class and on
     all its future subclasses. */
    XmeTraitSet(widgetClass, XmQTspecifyRenderTable, (XtPointer) &gridSRTT);
}


/**************************************************************************
 *
 *  Initialize:
 *      Called when this widget is first instantiated.
 *
 ***************************************************************************/
static void 
Initialize (
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
           )
{
 ExmGridWidget rw = (ExmGridWidget)request_w;
 ExmGridWidget nw = (ExmGridWidget)new_w;
    
  /* Initialize one of the internal fields of the ExmGrid widget. */ 
    nw->grid.processing_constraints = False;
    
  /* Ensure that user doesn't specify too many rows. */ 
    if (rw->grid.rows > EXM_GRID_MAX_NUMBER_OF_ROWS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_ROWS);
      nw->grid.rows = EXM_GRID_MAX_NUMBER_OF_ROWS;
    }
    
  /* Ensure that user doesn't specify too many columns. */ 
    if (rw->grid.columns > EXM_GRID_MAX_NUMBER_OF_COLUMNS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_COLUMNS);
      nw->grid.columns = EXM_GRID_MAX_NUMBER_OF_COLUMNS;
    }

  /* Copy in the dialog title XmString and update our shell */
    if (nw->grid.dialog_title) {   
      nw->grid.dialog_title = XmStringCopy(rw->grid.dialog_title) ;
      XmeSetWMShellTitle(nw->grid.dialog_title, XtParent(new_w)) ;
    }
}


/****************************************************************************
 *
 *  Destroy:
 *      Called when the widget is destroyed.
 *
 ****************************************************************************/
static void 
Destroy(
        Widget wid )
{   
 ExmGridWidget grid = (ExmGridWidget)wid;

  /* Deallocate the dynamic memory that holds the dialog_title. */ 
    XmStringFree(grid->grid.dialog_title) ;
}


/****************************************************************************
 *
 *  Resize:
 *
 ****************************************************************************/
static void  
Resize (
        Widget w
       )
{
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass(w);
	
  /* Configure the children by calling Layout. */
    if (gwc->grid_class.layout)
      (*(gwc->grid_class.layout))(w, NULL); 
    else 
      Layout (w, NULL); 
}




/****************************************************************************
 *
 *  Redisplay:
 *      Called by the Intrinsics in response to an exposure event.
 *
 ***************************************************************************/
static void 
Redisplay (
        Widget w,
        XEvent *event,
        Region region
          )
{
  /* Pass exposure event down to gadget children. */
    XmeRedisplayGadgets (w, event, region);
}



/*****************************************************************************
 *
 *  SetValues:
 *      Called by the Intrinsics whenever any of the resource values change.
 *
 ****************************************************************************/
static Boolean 
SetValues (
        Widget old_w,
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args 
          )
{
 ExmGridWidget cw = (ExmGridWidget)old_w;
 ExmGridWidget rw = (ExmGridWidget)request_w;
 ExmGridWidget nw = (ExmGridWidget)new_w;
 Boolean redisplay = False; 
 Boolean need_relayout;
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass(new_w);
	    
  /* Ensure that user doesn't specify too many rows. */ 
    if (rw->grid.rows > EXM_GRID_MAX_NUMBER_OF_ROWS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_ROWS);
      nw->grid.rows = EXM_GRID_MAX_NUMBER_OF_ROWS;
    }
    
  /* Ensure that user doesn't specify too many rows. */ 
    if (rw->grid.columns > EXM_GRID_MAX_NUMBER_OF_COLUMNS) {
      XmeWarning((Widget)rw, WARNING_TOO_MANY_COLUMNS);
      nw->grid.columns = EXM_GRID_MAX_NUMBER_OF_COLUMNS;
    }
    
  /* See if any class or subclass resources have changed. */
    if (gwc->grid_class.need_relayout)
      need_relayout = (*(gwc->grid_class.need_relayout))(old_w, new_w); 
    else 
      need_relayout = NeedRelayout (old_w, new_w);

  /* If any geometry resources changed and a new size wasn't specified,
     recalculate a new ideal size. */ 
    if (need_relayout) {
      /* Reset the widget size so that CalcSize can affect them. */
	if (nw->core.width == cw->core.width) 
          nw->core.width = 0;
	if (nw->core.height == cw->core.height) 
          nw->core.height = 0;
	
      /* Call CalcSize. */
        if (gwc->grid_class.calc_size)
          (*(gwc->grid_class.calc_size))(new_w, NULL,
                                         &nw->core.width, &nw->core.height); 
        else 
          CalcSize (new_w, NULL, &nw->core.width, &nw->core.height);

	
      /* If the geometry resources have changed but the size hasn't, 
	 we need to relayout manually, because Xt won't generate a 
	 Resize at this point. */
	if ((nw->core.width == cw->core.width) &&
          (nw->core.height == cw->core.height)) {

         /* Call Layout to configure the children. */
           if (gwc->grid_class.layout)
             (*(gwc->grid_class.layout))(new_w, NULL); 
           else 
             Layout(new_w, NULL); 
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
 
   if (XmIsDialogShell(XtParent(new_w)))  {   /* Is parent a DialogShell? */
     Cardinal i ;

    /* We have to look in the arglist since old_w->core.x is always 0, and 
       if new_w->core.x is also set to 0, we see no change. */
      for (i=0; i<*num_args; i++) {
         if (strcmp (args[i].name, XmNx) == 0) {
	   if ((args[i].value == 0) && (new_w->core.x == 0))
             new_w->core.x = XmDIALOG_SAVVY_FORCE_ORIGIN;
	 } 
	 if (strcmp (args[i].name, XmNy) == 0) {
           if ((args[i].value == 0) && (new_w->core.y == 0))
             new_w->core.y = XmDIALOG_SAVVY_FORCE_ORIGIN;
         }
      } /* end for */
   } /* end of if */

   /* Update wm shell title if it has changed */
   if(nw->grid.dialog_title != cw->grid.dialog_title ) {  
        XmStringFree(cw->grid.dialog_title) ;
        nw->grid.dialog_title = XmStringCopy(rw->grid.dialog_title) ;
        XmeSetWMShellTitle(nw->grid.dialog_title, XtParent(new_w)) ;
   }

   return (redisplay);
}



/*************************************************************************
 *
 *  SetValuesAlmost:
 *       Called by the Intrinsics when an XtMakeGeometryRequest call
 *       returns either XmGeometryAlmost or XtGeometryNo.  
 *
 ***************************************************************************/
static void 
SetValuesAlmost(
        Widget cw,		/* unused */
        Widget nw,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{  
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass(nw);
	
  /* ExmGrid's parent said XtGeometryNo to ExmGrid's geometry request. 
     Therefore, we need to relayout because this request
     was due to a change in internal geometry resource of the ExmGrid */
    if (!reply->request_mode) {
	if (gwc->grid_class.layout)
	    (*(gwc->grid_class.layout))(nw, NULL); 
	else 
	    Layout(nw, NULL);
    }

    *request = *reply;
}


/*************************************************************************
 *
 *  QueryGeometry:
 *       Called by a parent of Grid when the parent needs to find out Grid's
 *       preferred size.  QueryGeometry calls CalcSize to do find the
 *       preferred size. 
 *
 ***************************************************************************/
static XtGeometryResult 
QueryGeometry (
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply 
              )
{
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass(w);
	
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
    if (gwc->grid_class.calc_size)
      (*(gwc->grid_class.calc_size))(w, NULL, &reply->width, &reply->height); 
    else 
      CalcSize (w, NULL, &reply->width, &reply->height);

  /* This function handles CWidth and CHeight */
    return XmeReplyToQueryGeometry(w, request, reply) ;
}


/****************************************************************************
 *
 *  GeometryManager:
 *       Called by Intrinsics in response to a geometry change request from 
 *       one of the children of ExmGrid.
 *
 ***************************************************************************/
static XtGeometryResult 
GeometryManager (
        Widget w,  /* instigator */
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply
                )
{
 ExmGridWidget gw = (ExmGridWidget) XtParent(w);
 XtWidgetGeometry parentRequest;
 XtGeometryResult result;
 Dimension curWidth, curHeight, curBW;
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass((Widget)gw);

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
    if (gwc->grid_class.calc_size)
	(*(gwc->grid_class.calc_size))((Widget)gw, w,
				       &parentRequest.width, 
				       &parentRequest.height); 
    else 
	CalcSize ((Widget)gw, w, &parentRequest.width, &parentRequest.height);
    
    
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
       if (gwc->grid_class.layout)
	   (*(gwc->grid_class.layout))((Widget)gw, w); 
       else 
	   Layout ((Widget)gw, w); /* Layout with this child as the instigator,
			              so that we don't resize this child. */
   }

   return (result);
}



/**************************************************************************
 *
 *  ChangeManaged:
 *      Called by the Intrinsics whenever either of the following happens:
 *           * a managed child becomes unmanaged.
 *           * an unmanaged child becomes managed.
 *
 *************************************************************************/
static void 
ChangeManaged(
        Widget w
             )
{
 Dimension gridWidth, gridHeight;
 ExmGridWidgetClass gwc = (ExmGridWidgetClass) XtClass(w);
	
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
    if (gwc->grid_class.calc_size)
	(*(gwc->grid_class.calc_size))(w, NULL, &gridWidth, &gridHeight); 
    else 
	CalcSize (w, NULL, &gridWidth, &gridHeight);

 /* Ask parent of Grid if Grid's new size is acceptable.  Keep asking until
    parent returns either XtGeometryYes or XtGeometryNo. */
    while (XtMakeResizeRequest (w, gridWidth, gridHeight,
   		                &gridWidth, &gridHeight) == XtGeometryAlmost);

    /* Now that we have a size for the Grid, we can layout the children
       of the grid. */ 
    if (gwc->grid_class.layout)
	(*(gwc->grid_class.layout))(w, NULL); 
    else 
	Layout (w, NULL);
    
    /* Update keyboard traversal */
    XmeNavigChangeManaged (w);
}




/**************************************************************************
 *
 *  ConstraintSetValues:
 *      Called by Intrinsics if there is any change in any of the constraint
 *      resources. 
 *
 **************************************************************************/
static Boolean 
ConstraintSetValues (
    Widget cw,
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num_args 
                    )
{
 ExmGridConstraint nc;
 ExmGridConstraint cc;
 ExmGridWidget gw;

    if (!XtIsRectObj (nw)) 
	return (False);

    gw = (ExmGridWidget)XtParent(nw);
    nc = ExmGridCPart(nw);
    cc = ExmGridCPart(cw);

    /* Check for change in ExmNgridMarginWidth or ExmNgridMarginHeight */
    if ((nc->grid_margin_width_within_cell != 
	 cc->grid_margin_width_within_cell ||
	 nc->grid_margin_height_within_cell != 
	 cc->grid_margin_height_within_cell) && 
	XtIsManaged (nw)) {
	/* Tell the Intrinsics and the GeometryManager method that a 
	   reconfigure is needed. */
	gw->grid.processing_constraints = True;
     /* A trick: by altering one of the core geometry fields, Xt will
        call the parent's geometry_manager method. */ 
	nw->core.border_width += 1;
    }

    return (False);
}


/***************************************************************************
 *
 *  Layout:
 *     Does all the placement of children.
 *     Instigator tells whether or not to resize all children.
 *
 *************************************************************************/
static void 
Layout (
        Widget wid,
        Widget instigator
       )
{
 ExmGridWidget gw = (ExmGridWidget) wid ;
 Dimension mw = gw->grid.margin_width;
 Dimension mh = gw->grid.margin_height;
 Dimension TotalWidthOfGridWidget  = gw->core.width;
 Dimension TotalWidthOfGridMargins, TotalHeightOfGridMargins;
 Dimension TotalHeightOfGridWidget = gw->core.height;
 Dimension AvailWidthForChildren = 1, AvailHeightForChildren = 1; 
 Dimension WidthAllottedEachChild, HeightAllottedEachChild;
 int i, row, column;

    /* Lay out the children that ExmGrid is currently managing.  
       Each child will be placed somewhere on the rowxcolumn grid. */
    TotalWidthOfGridMargins = 2 * mw;
    if (TotalWidthOfGridWidget > TotalWidthOfGridMargins) 
	AvailWidthForChildren = TotalWidthOfGridWidget - 
	    TotalWidthOfGridMargins; 

    WidthAllottedEachChild = AvailWidthForChildren / gw->grid.columns;

    TotalHeightOfGridMargins = 2 * mh;
    if (TotalHeightOfGridWidget > TotalHeightOfGridMargins) 
	AvailHeightForChildren = TotalHeightOfGridWidget - 
	    TotalHeightOfGridMargins; 
    HeightAllottedEachChild = AvailHeightForChildren / gw->grid.rows;
    
    /* Now that we know how much space is allotted for each child, we
       can lay them all out. */
    row = 0;
    column = 0;
    for (i = 0; i < gw->composite.num_children; i++) {
	Widget ic = gw->composite.children[i];
	ExmGridConstraint glc = ExmGridCPart (ic);
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

	if (column == gw->grid.columns) {
	    column = 0;
	    row += 1;
	}
    } 
}



/******************************************************************************
 *
 *  CalcSize:
 *     Called by QueryGeometry, SetValues, GeometryManager, and ChangeManaged.
 *     Calculate the ideal size of the ExmGrid widget. 
 *     Only affects the returned size if it is 0.
 *
 ****************************************************************************/
static void 
CalcSize (
        Widget wid,
        Widget instigator,
        Dimension *TotalWidthOfGridWidget,
        Dimension *TotalHeightOfGridWidget
         )
{
 ExmGridWidget gw = (ExmGridWidget) wid ;
 Dimension maxWidth = 1;
 Dimension maxHeight = 1;
 int i;

  /* Examine each of Grid's children.  Find the biggest child.  The 
     ideal size of the Grid will be large enough to accomodate the
     largest child. */ 
    for (i = 0; i < gw->composite.num_children; i++) {
	Widget ic = gw->composite.children[i];
	ExmGridConstraint glc = ExmGridCPart (ic);
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
       *TotalWidthOfGridWidget = maxWidth * gw->grid.columns +
   			         (2 * (gw->grid.margin_width));
   }

 /* The total height of the grid widget should be set to the height of 
    the largest child widget times the number of columns. */
   if (!*TotalHeightOfGridWidget) {
       *TotalHeightOfGridWidget = maxHeight * gw->grid.rows +
   			          (2 * (gw->grid.margin_height));
   }
}


/****************************************************************************
 *
 *  NeedRelayout:
 *     Called by SetValues. 
 *     Returns True if a relayout is needed.  
 *     based on this class and all superclass resources' changes. 
 *
 ***************************************************************************/
static Boolean
NeedRelayout (
        Widget old_w,
        Widget new_w 
         )
{
 ExmGridWidget cw = (ExmGridWidget)old_w;
 ExmGridWidget nw = (ExmGridWidget)new_w;

    if (nw->grid.margin_width != cw->grid.margin_width ||
	nw->grid.margin_height != cw->grid.margin_height ||
	nw->grid.rows != cw->grid.rows ||
	nw->grid.columns != cw->grid.columns) {
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
static void
CallMapUnmap(
	 Widget wid,
	 Boolean map_unmap)
{
    ExmGridWidget grid = (ExmGridWidget) wid ;
    XmAnyCallbackStruct call_data;	

    call_data.reason = (map_unmap)? XmCR_MAP : XmCR_UNMAP;
    call_data.event  = NULL;			

    if (map_unmap) {
	XtCallCallbackList (wid, grid->grid.map_callback, 
			    &call_data);
    } else {
	XtCallCallbackList (wid, grid->grid.unmap_callback, 
			    &call_data);
    }
}

/*****************************************************************
 *
 * Trait method for XmQTspecifyRenderTable.   
 *
*****************************************************************/

static XmRenderTable
GetTable(
	 Widget wid,
	 XtEnum type)
{
    ExmGridWidget grid = (ExmGridWidget) wid ;

    switch(type) {
    case XmLABEL_RENDER_TABLE : return grid->grid.label_render_table ;
    case XmBUTTON_RENDER_TABLE : return grid->grid.button_render_table ;
    case XmTEXT_RENDER_TABLE : return grid->grid.text_render_table ;
    }
   
    return NULL ;
}


/*******************************************************************************
 *
 *  ExmCreateGrid:
 *      Called by an application. 
 *
 ******************************************************************************/
Widget 
ExmCreateGrid (
        Widget parent,
        char *name,
        ArgList arglist, 
        Cardinal argcount
              )
{
 /* This is a convenience function to instantiate an ExmGrid widget. */
   return (XtCreateWidget (name, exmGridWidgetClass, parent, 
                           arglist, argcount));
}


/*******************************************************************************
 *
 *  ExmCreateGridDialog
 *      Called by an application to create an ExmGrid managed by a
 *      DialogShell. 
 *
 ******************************************************************************/
Widget 
ExmCreateGridDialog (
        Widget parent,
        char *name,
        ArgList arglist, 
        Cardinal argcount
              )
{
   return XmeCreateClassDialog (exmGridWidgetClass,
                                parent, name, arglist, argcount) ; 

}
