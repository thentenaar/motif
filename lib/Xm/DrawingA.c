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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: DrawingA.c /main/16 1999/10/13 16:16:41 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "DrawingAI.h"
#include "GadgetUtiI.h"
#include "GMUtilsI.h"
#include "RepTypeI.h"
#include "TraversalI.h"
#include "XmI.h"


#define	MARGIN_DEFAULT		10

#define defaultTranslations	_XmDrawingA_defaultTranslations
#define traversalTranslations	_XmDrawingA_traversalTranslations


/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass w_class) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Redisplay( 
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Resize( 
                        Widget wid) ;
static XtGeometryResult GeometryManager( 
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged( 
                        Widget wid) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static XtGeometryResult QueryGeometry( 
                        Widget wid,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *desired) ;
static XmNavigability WidgetNavigable( 
                        Widget wid) ;

/********    End Static Function Declarations    ********/


static XtActionsRec actionsList[] =
{
   { "DrawingAreaInput", _XmDrawingAreaInput },
};


/*  Resource definitions for DrawingArea
 */

static XtResource resources[] =
{
    {	XmNmarginWidth,
	    XmCMarginWidth, XmRHorizontalDimension, sizeof (Dimension),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.margin_width),
	    XmRImmediate, (XtPointer) MARGIN_DEFAULT
	},

    {	XmNmarginHeight,
	    XmCMarginHeight, XmRVerticalDimension, sizeof (Dimension),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.margin_height),
	    XmRImmediate, (XtPointer) MARGIN_DEFAULT
	},

   {	XmNresizeCallback,
	    XmCCallback, XmRCallback, sizeof (XtCallbackList),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.resize_callback),
	    XmRImmediate, (XtPointer) NULL
	},

   {	XmNexposeCallback,
	    XmCCallback, XmRCallback, sizeof (XtCallbackList),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.expose_callback),
	    XmRImmediate, (XtPointer) NULL
	},

   {	XmNinputCallback,
	    XmCCallback, XmRCallback, sizeof (XtCallbackList),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.input_callback),
	    XmRImmediate, (XtPointer) NULL
	},
#ifndef XM_PART_BC
   {	XmNconvertCallback,
	    XmCCallback, XmRCallback, sizeof (XtCallbackList),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.convert_callback),
	    XmRImmediate, (XtPointer) NULL
	},

   {	XmNdestinationCallback,
	    XmCCallback, XmRCallback, sizeof (XtCallbackList),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.destination_callback),
	    XmRImmediate, (XtPointer) NULL
	},
#endif

{	XmNresizePolicy,
	    XmCResizePolicy, XmRResizePolicy, sizeof (unsigned char),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.resize_policy),
	    XmRImmediate, (XtPointer) XmRESIZE_ANY
	},

};


static XmSyntheticResource syn_resources[] =
{
   {	XmNmarginWidth,
	    sizeof (Dimension),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.margin_width),
	    XmeFromHorizontalPixels, XmeToHorizontalPixels
	},

   {	XmNmarginHeight,
	    sizeof (Dimension),
	    XtOffsetOf(XmDrawingAreaRec, drawing_area.margin_height),
	    XmeFromVerticalPixels, XmeToVerticalPixels
	},
};



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,				/* InitializePrehook	*/
    NULL,				/* SetValuesPrehook	*/
    NULL,				/* InitializePosthook	*/
    NULL,				/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    NULL,				/* secondaryCreate	*/
    NULL,               		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    NULL,				/* getValuesPrehook	*/
    NULL,				/* getValuesPosthook	*/
    NULL,                               /* classPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    NULL                                /* focusChange          */
};

externaldef( xmdrawingareaclassrec) XmDrawingAreaClassRec
                                   xmDrawingAreaClassRec =
{
   {			/* core_class fields      */
      (WidgetClass) &xmManagerClassRec,		/* superclass         */
      "XmDrawingArea",				/* class_name         */
      sizeof(XmDrawingAreaRec),			/* widget_size        */
      ClassInitialize,	        		/* class_initialize   */
      ClassPartInitialize,			/* class_part_init    */
      FALSE,					/* class_inited       */
      Initialize,      			        /* initialize         */
      NULL,					/* initialize_hook    */
      XtInheritRealize,				/* realize            */
      actionsList,				/* actions	      */
      XtNumber(actionsList),			/* num_actions	      */
      resources,				/* resources          */
      XtNumber(resources),			/* num_resources      */
      NULLQUARK,				/* xrm_class          */
      TRUE,					/* compress_motion    */
      FALSE,					/* compress_exposure  */
      TRUE,					/* compress_enterlv   */
      FALSE,					/* visible_interest   */
      NULL,			                /* destroy            */
      Resize,           			/* resize             */
      Redisplay,	        		/* expose             */
      SetValues,                		/* set_values         */
      NULL,					/* set_values_hook    */
      XtInheritSetValuesAlmost,	        	/* set_values_almost  */
      NULL,					/* get_values_hook    */
      NULL,					/* accept_focus       */
      XtVersion,				/* version            */
      NULL,					/* callback_private   */
      defaultTranslations,			/* tm_table           */
      QueryGeometry,                    	/* query_geometry     */
      NULL,             	                /* display_accelerator*/
      (XtPointer)&baseClassExtRec,              /* extension          */
   },
   {		/* composite_class fields */
      GeometryManager,    	                /* geometry_manager   */
      ChangeManaged,	                	/* change_managed     */
      XtInheritInsertChild,			/* insert_child       */
      XtInheritDeleteChild,     		/* delete_child       */
      NULL,                                     /* extension          */
   },

   {		/* constraint_class fields */
      NULL,					/* resource list        */   
      0,					/* num resources        */   
      sizeof(XmManagerConstraintRec),		/* constraint size      */   
      NULL,					/* init proc            */   
      NULL,					/* destroy proc         */   
      NULL,					/* set values proc      */   
      NULL,                                     /* extension            */
   },

   {		/* manager_class fields */
      traversalTranslations,			/* translations           */
      syn_resources,				/* syn_resources      	  */
      XtNumber (syn_resources),			/* num_get_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           */    
   },

   {          /* drawingArea class */
      (XtPointer) NULL,                         /* extension pointer */
   }	
};

externaldef( xmdrawingareawidgetclass) WidgetClass xmDrawingAreaWidgetClass
                                       = (WidgetClass) &xmDrawingAreaClassRec ;



/****************************************************************/
static void 
ClassInitialize( void )
{   
  baseClassExtRec.record_type = XmQmotif ;
}


/****************************************************************/
static void 
ClassPartInitialize(
        WidgetClass w_class )
{   

    _XmFastSubclassInit( w_class, XmDRAWING_AREA_BIT) ;

}


/****************************************************************
 * Initialize. Check resizePolicy resource value
 ****************/
/*ARGSUSED*/
static void 
Initialize(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmDrawingAreaWidget new_w = (XmDrawingAreaWidget) nw ;

    if(new_w->drawing_area.resize_policy != XmRESIZE_SWINDOW
       && !XmRepTypeValidValue(XmRID_RESIZE_POLICY,
                            new_w->drawing_area.resize_policy, 
			    (Widget) new_w)    ) {   
	new_w->drawing_area.resize_policy = XmRESIZE_ANY ;
    } 
}



/****************************************************************
 * General redisplay function called on exposure events.
 ****************/
static void 
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid ;
    XmDrawingAreaCallbackStruct cb;
/****************/
   
    cb.reason = XmCR_EXPOSE;
    cb.event = event;
    cb.window = XtWindow (da);

    XtCallCallbackList ((Widget) da, da->drawing_area.expose_callback, &cb);

    XmeRedisplayGadgets( (Widget) da, event, region);
}


/****************************************************************
 * Invoke the application resize callbacks.
 ****************/
static void 
Resize(
        Widget wid )
{
    XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid ;
    XmDrawingAreaCallbackStruct cb;

    cb.reason = XmCR_RESIZE;
    cb.event = NULL;
    cb.window = XtWindow (da);

    XtCallCallbackList ((Widget) da, da->drawing_area.resize_callback, &cb);
}

static Widget 
ObjectAtPoint(
        Widget wid,
        Position  x,
        Position  y )
{
    CompositeWidget cw = (CompositeWidget) wid ;
    register int i;
    register Widget widget;

    i = cw->composite.num_children ;
    while( i-- ) {
	widget = cw->composite.children[i];

	/* do not care for gadget only for the DA input */
	if (XmIsGadget(widget) && XtIsManaged (widget)) {
	    if (x >= widget->core.x && y >= widget->core.y && 
		x < widget->core.x + widget->core.width    && 
		y < widget->core.y + widget->core.height)
		return (widget);
	}
    }

    return (NULL);
}


/****************************************************************
 * This function processes key and button presses and releases
 *   belonging to the DrawingArea.
 ****************/
/*ARGSUSED*/
void 
_XmDrawingAreaInput(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{   
    XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid ;
    XmDrawingAreaCallbackStruct cb ;
    int x, y ;
    Boolean button_event = True, input_on_gadget ;

    if ((event->type == ButtonPress) || 
	(event->type == ButtonRelease)) {
	x = event->xbutton.x ;
	y = event->xbutton.y ;
    } else 
    if (event->type == MotionNotify) {
	x = event->xmotion.x ;
	y = event->xmotion.y ;
    } else
    if ((event->type == KeyPress) || 
	(event->type == KeyRelease)) {
	x = event->xkey.x ;
	y = event->xkey.y ;
	button_event = False ;
    } else return ; 
	    /* Unrecognized event (cannot determine x, y of pointer).*/
	
    input_on_gadget = (ObjectAtPoint((Widget)da, x, y) != NULL);
	    
    if (!input_on_gadget && (!da->manager.active_child || button_event)) {
	cb.reason = XmCR_INPUT ;
	cb.event = event ;
	cb.window = XtWindow( da) ;
	XtCallCallbackList ((Widget) da,
			    da->drawing_area.input_callback, &cb) ;

    }
}


/****************************************************************/
static XtGeometryResult 
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
            XmDrawingAreaWidget da;
/*            XtGeometryHandler manager ; */
/****************/

    da = (XmDrawingAreaWidget) w->core.parent;

    /* function shared with Bulletin Board */
    return(_XmGMHandleGeometryManager((Widget)da, w, request, reply, 
                                    da->drawing_area.margin_width, 
                                    da->drawing_area.margin_height, 
                                    da->drawing_area.resize_policy,
                                    True)); /* no overlap checking */
}

/****************************************************************
 * Re-layout children.
 ****************/
static void 
ChangeManaged(
        Widget wid )
{
    XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid ;

    /* function shared with Bulletin Board */
    _XmGMEnforceMargin((XmManagerWidget)da,
                     da->drawing_area.margin_width,
                     da->drawing_area.margin_height, 
                     False); /* use movewidget, not setvalue */

    /* The first time, reconfigure only if explicit size were not given */

    if (XtIsRealized((Widget)da) || (!XtWidth(da)) || (!XtHeight(da))) {

      /* function shared with Bulletin Board */
      (void)_XmGMDoLayout((XmManagerWidget)da,
                          da->drawing_area.margin_width,
                          da->drawing_area.margin_height,
                          da->drawing_area.resize_policy,
                          False);  /* queryonly not specified */
        }
  
    XmeNavigChangeManaged((Widget) da) ;
}

/****************************************************************/
/*ARGSUSED*/
static Boolean 
SetValues(
        Widget cw,
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmDrawingAreaWidget current = (XmDrawingAreaWidget) cw ;
    XmDrawingAreaWidget new_w = (XmDrawingAreaWidget) nw ;

    if(new_w->drawing_area.resize_policy != XmRESIZE_SWINDOW
       && !XmRepTypeValidValue(XmRID_RESIZE_POLICY,
			    new_w->drawing_area.resize_policy, 
			    (Widget) new_w)  ) {   
	new_w->drawing_area.resize_policy = 
	    current->drawing_area.resize_policy ;
    } 


    /* If new margins, re-enforce them using movewidget, 
       then update the width and height so that XtSetValues does
       the geometry request */
    if (XtIsRealized((Widget) new_w) &&
	(((new_w->drawing_area.margin_width != 
	  current->drawing_area.margin_width) ||
	 (new_w->drawing_area.margin_height !=
	  current->drawing_area.margin_height)))) {
	    
	/* move the child around if necessary */
	_XmGMEnforceMargin((XmManagerWidget)new_w,
			   new_w->drawing_area.margin_width,
			   new_w->drawing_area.margin_height,
			   False); /* use movewidget, no request */
	_XmGMCalcSize ((XmManagerWidget)new_w, 
		       new_w->drawing_area.margin_width, 
		       new_w->drawing_area.margin_height, 
		       &new_w->core.width, &new_w->core.height);
    }

    return( False) ;
}

   
/****************************************************************
 * Handle query geometry requests
 ****************/
static XtGeometryResult 
QueryGeometry(
        Widget wid,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    XmDrawingAreaWidget da = (XmDrawingAreaWidget) wid ;

     /* function shared with Bulletin Board */
     return(_XmGMHandleQueryGeometry(wid, intended, desired, 
                                   da->drawing_area.margin_width, 
                                   da->drawing_area.margin_height, 
                                   da->drawing_area.resize_policy));
}


/****************************************************************
 * Xm private class method 
 ****************/


static XmNavigability
WidgetNavigable(
        Widget wid)
{   
  if(    XtIsSensitive(wid)
     &&  ((XmManagerWidget) wid)->manager.traversal_on    )
    {   
      XmNavigationType nav_type = ((XmManagerWidget) wid)
	                                            ->manager.navigation_type ;
      Widget *children = ((XmManagerWidget) wid)->composite.children ;
      unsigned idx = 0 ;

      /* If a Drawing Area has a navigable child, and initial focus
	 is either NULL or point to this child, allow navigation to it.
       */
        while(idx < ((XmManagerWidget) wid)->composite.num_children) {
	  if  (_XmGetNavigability(children[idx]) &&
	       ((((XmManagerWidget) wid)->manager.initial_focus == NULL) ||
		(((XmManagerWidget) wid)->manager.initial_focus == 
		                                          children[idx]))){
	      if(    (nav_type == XmSTICKY_TAB_GROUP)
		 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
		 ||  (    (nav_type == XmTAB_GROUP)
		      &&  !_XmShellIsExclusive( wid))) {
		  return XmDESCENDANTS_TAB_NAVIGABLE ;
	      }
	      return XmDESCENDANTS_NAVIGABLE ;
	  }
	  ++idx ;
      }

      /* else just return the DA itself */

      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	 ||  (    (nav_type == XmTAB_GROUP)
	      &&  !_XmShellIsExclusive( wid))    )
	{
	  return XmTAB_NAVIGABLE ;
	}
      return XmCONTROL_NAVIGABLE ;
    }
  return XmNOT_NAVIGABLE ;
}




/****************************************************************
 * This function creates and returns a DrawingArea widget.
 ****************/
Widget 
XmCreateDrawingArea(
        Widget p,
        String name,
        ArgList args,
        Cardinal n )
{
/****************/

    return( XtCreateWidget( name, xmDrawingAreaWidgetClass, p, args, n)) ;
}

Widget 
XmVaCreateDrawingArea(
        Widget parent,
        char *name,
        ...)
{
    register Widget w;
    va_list var;
    int count;
    
    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    
    Va_start(var, name);
    w = XmeVLCreateWidget(name, 
                         xmDrawingAreaWidgetClass,
                         parent, False, 
                         var, count);
    va_end(var);   
    return w;
}

Widget
XmVaCreateManagedDrawingArea(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;
    
    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);
    
    Va_start(var, name);
    w = XmeVLCreateWidget(name, 
                         xmDrawingAreaWidgetClass,
                         parent, True, 
                         var, count);
    va_end(var);   
    return w;
}
