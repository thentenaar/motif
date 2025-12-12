/* $XConsortium: Simple.c /main/5 1995/07/15 20:41:24 drk $ */
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
/*********************************** WARNING **********************************
 *
 * ExmSimple is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way
 *********************************** WARNING *********************************/


/******************************************************************************
 *
 * Simple.c - ExmSimple widget.  This widget renders a rectangle or an oval. 
 *            The ExmSimple widget demonstrates how to create a
 *            subclass of XmPrimitive that can, itselve, serve as a superclass
 *            for other widgets.  ExmSimple also demonstrates how to  
 *               * create a new representation type (ExmRSimpleShape).
 *               * install the XmQTcontainerItem trait and its trait methods.
 *               * install the XmQTcareParentVisual trait and methods.
 *               * access the trait methods of the XmQTcontainer trait. 
 *            See the "OSF/Motif Widget Writer's Guide" for more details.
 *
******************************************************************************/

/* Include appropriate header files. */ 
#include <Exm/SimpleP.h>    /* private header file for the ExmSimple widget */
#include <Xm/DrawP.h>       /* header file for Xme drawing functions */
#include <Xm/RepType.h>     /* header file for representation type facility */
#include <Xm/Screen.h>      /* header file for screen information */
#include <Xm/TraitP.h>      /* header file for installing traits */
#include <Xm/CareVisualT.h> /* header file for XmQTcareParentVisual trait */
#include <Xm/ContItemT.h>   /* header file for XmQTcontainerItem trait */
#include <Xm/ContainerT.h>  /* header file for XmQTcontainer trait */


/* The following section defines constants for the widget.  
   The IDEAL_SHAPE_SIZE constant defines the starting preferred width 
   and height for the widget visual (which is either a rectangle or an oval). 
*/
static XmConst int IDEAL_SHAPE_SIZE  = 30;
static XmConst int FIND_NATURAL_SIZE = 0;


/* Declare all static functions. */ 
static void ClassInitialize(void);
static void ClassPartInitialize (
                        WidgetClass widgetClass );
static void Initialize(
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void Destroy (
                        Widget w);
static void Realize(
		    Widget w,
		    XtValueMask *p_valueMask,
		    XSetWindowAttributes *attributes );
static void Resize (
                        Widget w);
static void Redisplay (
                        Widget w,
                        XEvent *event,
                        Region region);
static Boolean SetValues (
                        Widget old_w,
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static XtGeometryResult QueryGeometry (
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply);
static void DrawVisual (
                        Widget w);
static void DrawShadow (
                        Widget w);
static void CreateGC (
                        Widget w);
static void DestroyGC (
                        Widget w);
static GC SelectGC (
                        Widget w);
static void CalcVisualSize (
                        Widget w);
static void CalcWidgetSize (
                        Widget w);
static Boolean WidgetDisplayRect(
                        Widget       w,
                        XRectangle  *displayrect);
static void Reconfigure (
                        WidgetClass  class,
                        Widget       new_w,
                        Widget       old_w);
static	void ContItemSetValues(Widget w, 
			       XmContainerItemData contItemData);

static	void ContItemGetValues(Widget w, 
			       XmContainerItemData contItemData);

static  Boolean HandleRedraw (Widget kid, 
			      Widget cur_parent,
			      Widget new_parent,
			      Mask visual_flag);
static void SetSelectedVisual (Widget wid) ;




/* Define the resources for the ExmSimple widget.  This widget supports
   three new resources: ExmNsimpleShape, XmNmarginWidth, and XmNmarginHeight. 
   Naturally, ExmSimple also supports the resources of its two superclasses:
   Core and XmPrimitive. */
static XtResource resources[] = 
{
    {
	ExmNsimpleShape,   /* resource name */
	ExmCSimpleShape,   /* resource class */
	ExmRSimpleShape,   /* resource data type */
	sizeof (unsigned char),  /* all representation types have this size */
	XtOffsetOf( ExmSimpleRec, simple.simple_shape),  /* offset */
	XmRImmediate,     /* default value appears on next line */
	(XtPointer)ExmSHAPE_OVAL /* default shape is oval */
    },
    {
 	XmNmarginWidth, 
	XmCMarginWidth, 
	XmRHorizontalDimension, 
	sizeof (Dimension),
	XtOffsetOf( ExmSimpleRec, simple.margin_width), 
	XmRImmediate,
	(XtPointer) 4
    },
    {
	XmNmarginHeight, 
	XmCMarginHeight, 
	XmRVerticalDimension, 
	sizeof (Dimension),
	XtOffsetOf( ExmSimpleRec, simple.margin_height),
	XmRImmediate,
	(XtPointer) 4
    },
};

/* Two of the three resources will also be handled as synthetic resources. */ 
static XmSyntheticResource syn_resources[] =
{
    { 
	XmNmarginWidth, 
	sizeof (Dimension),
	XtOffsetOf( ExmSimpleRec, simple.margin_width), 
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels 
    },
    { 
	XmNmarginHeight, 
	sizeof (Dimension),
	XtOffsetOf( ExmSimpleRec, simple.margin_height),
	XmeFromVerticalPixels, 
	XmeToVerticalPixels 
    },
};



/* Define the widget class record.  See Chapter 3 of the
   "OSF/Motif Widget Writer's Guide" for details. */
 
/* Here is the XmPrimitive class extension record. */
static XmPrimitiveClassExtRec primClassExtRec = {
    /* next_extension */             NULL,
    /* record_type */                NULLQUARK,
    /* version */                    XmPrimitiveClassExtVersion,
    /* record_size */                sizeof(XmPrimitiveClassExtRec),
    /* widget_baseline */            NULL,
    /* widget_display_rect */        WidgetDisplayRect,
    /* widget_margins */             NULL,
};    
    
externaldef (exmsimpleclassrec) ExmSimpleClassRec exmSimpleClassRec = {
  { /* Here is the core class record. */
    /* superclass */                 (WidgetClass)&xmPrimitiveClassRec,
    /* class_name */                 "ExmSimple",
    /* widget_size */                sizeof(ExmSimpleRec),
    /* class_initialize */           ClassInitialize,
    /* class_part_initialize */      ClassPartInitialize,
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    Realize,
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
    /* set_values_almost */          XtInheritSetValuesAlmost,
    /* get_values_hook */            NULL,
    /* accept_focus */               NULL,
    /* version */                    XtVersion,
    /* callback_private */           NULL,
    /* tm_table */                   XtInheritTranslations,
    /* query_geometry */             QueryGeometry,
    /* display_accelerator */        NULL,
    /* extension */                  NULL,
  },    
  { /* Here is the XmPrimitive class record. */
    /* border_highlight */           XmInheritBorderHighlight,
    /* border_unhighlight */         XmInheritBorderUnhighlight,
    /* translations */               XtInheritTranslations,
    /* arm_and_activate */           NULL,
    /* syn_resources */              syn_resources,
    /* num_syn_resources */          XtNumber(syn_resources),
    /* extension */                  (XtPointer)&primClassExtRec,
  },    
  { /* Here is the ExmSimple class record. */
    /* draw_visual */                DrawVisual,
    /* draw_shadow */                DrawShadow,
    /* create_gc */                  CreateGC,
    /* destroy_gc */                 DestroyGC,
    /* select_gc */                  SelectGC,
    /* calc_visual_size */           CalcVisualSize,
    /* calc_widget_size */           CalcWidgetSize,
    /* reconfigure */                Reconfigure,
    /* extension */                  NULL,
  }    
};    
/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */ 
  externaldef( exmsimplewidgetclass) WidgetClass exmSimpleWidgetClass =
                                (WidgetClass) &exmSimpleClassRec;


/* Declare trait record variables. */

/* Declare a trait record variable for the XmQTcontainerItem trait. */ 
static XmConst XmContainerItemTraitRec simpleCIT = {
  0,		/* version */
  ContItemSetValues,
  ContItemGetValues,
};

/* Declare a trait record variable for the XmQTcareParentVisual trait. */ 
static XmConst XmCareVisualTraitRec simpleCVT = {
    0,		/* version */
    HandleRedraw,
};


/* Declare any global static variables. */
/* The SimpleShapeNames variable holds some normalized values.  
   The XmRepTypeRegister function will use these values to determine the 
   legal values for a given representation type. */
static String SimpleShapeNames[] = {
	"simple_oval",
	"simple_rectangle"
};

static XmRepTypeId simpleShapeId;

/******************************************************************************
 *
 *  ClassInitialize:
 *     Called the first time a widget of this class is instantiated. 
 *
 *****************************************************************************/
static void 
ClassInitialize( void )
{
 /* The ExmNsimpleShape resource requires a value of data type ExmRSimpleShape.
    Since this is not a standard Motif data type, we must create this
    data type just for the ExmSimple widget.
    We want ExmRSimpleShape to hold an enumerated value, either 
    XmSIMPLE_OVAL or XmSIMPLE_RECTANGLE.  The best way to accomplish
    this is to register the new data type with representation type facility. */ 
   simpleShapeId = XmRepTypeRegister (ExmRSimpleShape, SimpleShapeNames,
				      NULL, XtNumber(SimpleShapeNames));
}



/******************************************************************************
 *
 *  ClassPartInitialize:
 *      Called when this widget or a subclass of this widget is instantiated.
 *
 *****************************************************************************/
static void 
ClassPartInitialize (
        WidgetClass widgetClass
                    )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)widgetClass;
 ExmSimpleWidgetClass sc = (ExmSimpleWidgetClass) wc->core_class.superclass;

 /* The following code allows subclasses of ExmSimple to inherit certain 
    methods of ExmSimple. */ 
   if (wc->simple_class.draw_visual == ExmInheritDrawVisual)
       wc->simple_class.draw_visual = sc->simple_class.draw_visual;
   if (wc->simple_class.draw_shadow == ExmInheritDrawShadow)
       wc->simple_class.draw_shadow = sc->simple_class.draw_shadow;
   if (wc->simple_class.create_gc == ExmInheritCreateGC)
       wc->simple_class.create_gc = sc->simple_class.create_gc;
   if (wc->simple_class.destroy_gc == ExmInheritDestroyGC)
       wc->simple_class.destroy_gc = sc->simple_class.destroy_gc;
   if (wc->simple_class.select_gc == ExmInheritSelectGC)
       wc->simple_class.select_gc = sc->simple_class.select_gc;
   if (wc->simple_class.calc_visual_size == ExmInheritCalcVisualSize)
       wc->simple_class.calc_visual_size = sc->simple_class.calc_visual_size;
   if (wc->simple_class.calc_widget_size == ExmInheritCalcWidgetSize)
       wc->simple_class.calc_widget_size = sc->simple_class.calc_widget_size;
   if (wc->simple_class.reconfigure == ExmInheritReconfigure)
       wc->simple_class.reconfigure = sc->simple_class.reconfigure;

  /* Install the containerItem trait on ExmSimple and all its subclasses. */ 
    XmeTraitSet((XtPointer) wc, XmQTcontainerItem, (XtPointer) &simpleCIT);

  /* Install the careParentVisual trait on ExmSimple and all subclasses. */ 
    XmeTraitSet((XtPointer) wc, XmQTcareParentVisual, (XtPointer) &simpleCVT);
}



/******************************************************************************
 *
 *  Initialize:
 *     Called by the Intrinsics when this widget is instantiated. 
 *
 *****************************************************************************/

static void 
Initialize(
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(new_w);
 ExmSimpleWidget rw = (ExmSimpleWidget)request_w;
 ExmSimpleWidget nw = (ExmSimpleWidget)new_w;

 /* Look at the value assigned to the ExmNsimpleShape resource.  If the
    value assigned is neither ExmSHAPE_OVAL nor ExmSHAPE_RECTANGLE, then
    issue a warning message and set ExmNsimpleShape to ExmSHAPE_OVAL. */
   if (!XmRepTypeValidValue (simpleShapeId, 
                             nw->simple.simple_shape, (Widget)nw))
     nw->simple.simple_shape = ExmSHAPE_OVAL;

 /* Call a function that creates the GC's needed by the widget. */ 
   if (wc->simple_class.create_gc) 
     (*(wc->simple_class.create_gc))((Widget)nw);

 /* If the widget's width is set to 0, that's a trigger for the widget 
    to calculate its width.  We are not actually going to calculate
    its width here, rather we are going to set the need_to_compute_width
    flag appropriately. */ 
   if (rw->core.width == FIND_NATURAL_SIZE) {
     nw->simple.need_to_compute_width = True; 
   }
   else {
     nw->simple.need_to_compute_width = False;
     nw->simple.pref_width = rw->core.width;
     nw->core.width = rw->core.width;
   }

   if (rw->core.height == FIND_NATURAL_SIZE) 
     nw->simple.need_to_compute_height = True;
   else {
     nw->simple.need_to_compute_height = False;
     nw->simple.pref_height = rw->core.height;
     nw->core.height = rw->core.height;
   }

 /* mark us as not selected to start with */
   nw->simple.saved_foreground = XmUNSPECIFIED_PIXEL ;
    
 /* In order not to resize the widget at each level of the Initialize
    chain, the actual class is passed to this method */
   if (wc->simple_class.reconfigure) 
     (*(wc->simple_class.reconfigure))(exmSimpleWidgetClass, new_w, NULL);
}


/******************************************************************************
 *
 *  Destroy: 
 *      Called by the Intrinsics whenever this widget is deallocated. 
 *
 *****************************************************************************/
static void
Destroy (Widget w)
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass) XtClass(w);

 /* The ExmSimple widget allocates two internal GC's.  In order to prevent 
    memory leaks, we must destroy these GC's.  */ 
   if (wc->simple_class.destroy_gc) 
     (*(wc->simple_class.destroy_gc))(w);
}

/*****************************************************************************
 * 
 * Realize
 *      Called by the Intrinsics to create the window for the widget.  This
 *      class's realize method creates a propagating window for this 
 *      exact class,  but uses the default window otherwise 
 *
 *****************************************************************************/
static void 
Realize(Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
   Mask valueMask = *p_valueMask;

   /* First call Primitive's method */
   xmPrimitiveClassRec.core_class.realize(w, p_valueMask, attributes);

   /* ExmSimple wants to propagate all unused events to its hierarchy, which
      Primitive.Realize doesn't do.  So if this is an ExmSimple widget, 
      we fix the do_not_propagate window attribute */
   if (XtClass(w) == exmSimpleWidgetClass) {
     Mask adjustMask;
     XSetWindowAttributes xswa;

     adjustMask = CWDontPropagate;
     xswa.do_not_propagate_mask = NoEventMask;
     XChangeWindowAttributes(XtDisplay(w), XtWindow(w), adjustMask, &xswa);
   }
}

/******************************************************************************
 *
 *  Resize: 
 *      Called by the Intrinsics whenever a parent resizes a child.
 *      Also called by the Reconfigure method. 
 *
 *****************************************************************************/
static void 
Resize (
        Widget w
       ) 
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(w);
 ExmSimpleWidget sw = (ExmSimpleWidget)w;
 Dimension  mw, mh;
 Dimension  window_decoration_thickness;
 Dimension  total_target_widget_width, total_target_widget_height;

 /* Call CalcVisualSize */
   if (wc->simple_class.calc_visual_size)
     (*(wc->simple_class.calc_visual_size))((Widget)sw);

   window_decoration_thickness = sw->primitive.highlight_thickness  +
                                 sw->primitive.shadow_thickness;

   mw = window_decoration_thickness + sw->simple.margin_width;
   total_target_widget_width = (2 * mw) + sw->simple.visual.width;

 /* When there is not enough space to display the entire widget, we
    must clip part of the widget, but which part? */ 

   if (sw->core.width >= total_target_widget_width) {
     /* We have enough space to display everything (the visual, the margins, 
        and the border decorations). */ 
     sw->simple.visual.x = mw;
     sw->simple.visual.width = sw->core.width - (2 * mw);
   } else if (sw->core.width > ((2 * window_decoration_thickness) + 
				sw->simple.visual.width)) {
     /* We do not have enough space to display everything, but we do have
        enough space to display the visual and the border decorations.
        The left and right margins will have to be reduced. */ 
     sw->simple.visual.x = (sw->core.width - sw->simple.visual.width)/2;
   } else if (sw->core.width > 2 * window_decoration_thickness) {
    /* Space is very tight.  We will eliminate the left and right margins
       all together.  Furthermore, we will reduce the size of the visual.  */  
     sw->simple.visual.x = window_decoration_thickness;
     sw->simple.visual.width = sw->core.width - 
	 (2 * window_decoration_thickness);
   } else {
     /* We do not have enough space to display even one pixel 
	of the visual. */ 
     sw->simple.visual.width = 0;
   }

 /* Now do the same for the vertical dimension. */
 mh = window_decoration_thickness + sw->simple.margin_height;
 total_target_widget_height = (2 * mh) + sw->simple.visual.height;

 if (sw->core.height >= total_target_widget_height) {
   /* We have enough space to display everything (the visual, the margins, 
      and the border decorations). */ 
   sw->simple.visual.y = mh;
   sw->simple.visual.height = sw->core.height - (2 * mh);
 } else if (sw->core.height > ((2 * window_decoration_thickness) 
			       + sw->simple.visual.height)) {
   /* We do not have enough space to display everything, but we do have
      enough space to display the visual and the border decorations.
      The top and bottom margins will have to be reduced. */ 
   sw->simple.visual.y = (sw->core.height - sw->simple.visual.height)/2;
 } else if (sw->core.height > 2 * window_decoration_thickness) {
   /* Space is very tight.  We will eliminate the top and right margins
      all together.  Furthermore, we will reduce the size of the visual.  */  
   sw->simple.visual.y = window_decoration_thickness;
   sw->simple.visual.height = sw->core.height - 
     (2 * window_decoration_thickness);
 } else {
   /* We do not have enough space to display even one pixel of the visual. */ 
   sw->simple.visual.height = 0;
 }
}

/******************************************************************************
 *
 *  Redisplay:
 *     Called by the Intrinsics whenever a portion of the widget that was
 *     obscured becomes exposed.   
 *
 *****************************************************************************/
static void 
Redisplay (
        Widget w,
        XEvent *event,
        Region region 
          ) 
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(w);

 /* Call the function that draws the widget visual. */ 
   if (wc->simple_class.draw_visual) 
     (*wc->simple_class.draw_visual) (w);

 /* Call the function that draws the widget shadow. */
   if (wc->simple_class.draw_shadow) 
     (*wc->simple_class.draw_shadow) (w);

 /* Envelop our superclass expose method.  The superclass expose
    method of XmPrimitive knows how to draw the border highlight. */ 
   (*(xmPrimitiveClassRec.core_class.expose))(w, event, region);
}



/******************************************************************************
 *
 *  SetValues:
 *     Called by the Intrinsics when an application attempts to
 *     change the value of a resource.  
 *
 *****************************************************************************/
static Boolean 
SetValues (
        Widget old_w,
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
          )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(new_w);
 ExmSimpleWidget cw = (ExmSimpleWidget)old_w;
 ExmSimpleWidget rw = (ExmSimpleWidget)request_w;
 ExmSimpleWidget nw = (ExmSimpleWidget)new_w;
 Boolean redisplayFlag = False;

 /* Validate the value of ExmNsimpleShape by calling XmRepTypeValidValue. */
   if (nw->simple.simple_shape != cw->simple.simple_shape) {
     if (!XmRepTypeValidValue (simpleShapeId, nw->simple.simple_shape, 
                               (Widget)nw))
       nw->simple.simple_shape = cw->simple.simple_shape;
     else
       redisplayFlag = True;
   }

 /* Redisplay on change in sensitivity */
 if (XtIsSensitive(new_w) != XtIsSensitive(old_w))
   redisplayFlag = True;

 /* If the widget's foreground or background color changes, 
    then we must update the GC. */ 
   if (nw->primitive.foreground != cw->primitive.foreground ||
       nw->core.background_pixel != cw->core.background_pixel) {
     if (wc->simple_class.destroy_gc)
       (*(wc->simple_class.destroy_gc))((Widget)cw);
     if (wc->simple_class.create_gc)
       (*(wc->simple_class.create_gc))((Widget)nw);
     redisplayFlag = True;
   }

 /* Check for application geometry settings. '0' means 'ideal size' */
   if (rw->core.width == FIND_NATURAL_SIZE) {
     nw->core.width = FIND_NATURAL_SIZE;
     nw->simple.need_to_compute_width = True;
   }
   else if (rw->core.width != cw->core.width) {
     nw->core.width = rw->core.width;
     nw->simple.pref_width = rw->core.width;
     nw->simple.need_to_compute_width = False;
   }

   if (rw->core.height == FIND_NATURAL_SIZE) {
     nw->core.height = FIND_NATURAL_SIZE;
     nw->simple.need_to_compute_height = True;
   }
   else if (rw->core.height != cw->core.height) {
     nw->core.height = rw->core.height;
     nw->simple.pref_height = rw->core.height;
     nw->simple.need_to_compute_height = False;
   }

 /* Check for any changes in total widget set, margin size, or 
    window decoration size.  If any are found, call Reconfigure. */ 
   nw->simple.need_to_reconfigure = False;
   if (nw->core.width != cw->core.width ||
       nw->core.height != cw->core.height ||
       nw->simple.margin_width != cw->simple.margin_width ||
       nw->simple.margin_height != cw->simple.margin_height ||
       nw->primitive.shadow_thickness != cw->primitive.shadow_thickness ||
       nw->primitive.highlight_thickness != cw->primitive.highlight_thickness
      ) 
   Reconfigure (exmSimpleWidgetClass, new_w, old_w);

   return (redisplayFlag);
}



/******************************************************************************
 *
 *  QueryGeometry:
 *     Called by the Intrinsics in response to a proposed changed in geometry.
 *
 *****************************************************************************/
static XtGeometryResult 
QueryGeometry (
        Widget widget,
        XtWidgetGeometry *parent_request,
        XtWidgetGeometry *child_reply
              ) 
{
 ExmSimpleWidget sw = (ExmSimpleWidget) widget;

   if (!XtIsRealized(widget)) {   /* Simple has not yet been realized. */ 
     child_reply->width  = XtWidth(widget);   /* might be 0 */
     child_reply->height = XtHeight(widget);  /* might be 0 */
   } else {                       /* Simple has been realized. */
     child_reply->width  = sw->simple.pref_width;
     child_reply->height = sw->simple.pref_height;
   }

 /* Return Simple's preferred size */
   return XmeReplyToQueryGeometry(widget, parent_request, child_reply);
}



/******************************************************************************
 *
 *  DrawVisual:
 *     Called by ExmSimple whenever it needs to draw or redraw the
 *     widget visual (either an oval or a rectangle).
 *
 *****************************************************************************/
static void 
DrawVisual (
        Widget w
           )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(w);
 ExmSimpleWidget sw = (ExmSimpleWidget)w;

 /* Draw the oval or the rectangle, space permitting. */
   if (sw->simple.visual.width > 0 && sw->simple.visual.height > 0) {
     switch (sw->simple.simple_shape) {
	    case(ExmSHAPE_OVAL):
	    default:
		XFillArc (XtDisplay(w), XtWindow(w),
			  wc->simple_class.select_gc(w),
			  sw->simple.visual.x, sw->simple.visual.y,
			  sw->simple.visual.width, sw->simple.visual.height,
			  0, 360 * 64);
		break;
	    case(ExmSHAPE_RECTANGLE):
		XFillRectangle (XtDisplay(w), XtWindow(w),
                          wc->simple_class.select_gc(w),
                          sw->simple.visual.x, sw->simple.visual.y,
                          sw->simple.visual.width, sw->simple.visual.height);
		break;
     }
   }
}



/******************************************************************************
 *
 *  DrawShadow:
 *      Called by Redisplay.
 *
 *****************************************************************************/
static void 
DrawShadow (
        Widget w
           )
{
 ExmSimpleWidget sw = (ExmSimpleWidget)w;

 /* If there is enough room in the widget to draw a shadow, 
    and if the shadow thickness resource is nonzero, then draw a 
    Motif-style shadow in the appropriate place around the widget's border. */ 
   if (sw->core.width  > (2 * sw->primitive.highlight_thickness) &&
       sw->core.height > (2 * sw->primitive.highlight_thickness) &&
       sw->primitive.shadow_thickness > 0) 
     XmeDrawShadows (XtDisplay (sw), XtWindow (sw),
      		     sw->primitive.top_shadow_GC,
      		     sw->primitive.bottom_shadow_GC,
      		     sw->primitive.highlight_thickness,
      		     sw->primitive.highlight_thickness,
      		     sw->core.width - (2 * sw->primitive.highlight_thickness),
      		     sw->core.height - (2 * sw->primitive.highlight_thickness),
      		     sw->primitive.shadow_thickness, 
                     XmSHADOW_ETCHED_OUT);
}



/******************************************************************************
 *
 *  CreateGC:
 *      Called by Initialize and by SetValues.
 *
 *****************************************************************************/
static void 
CreateGC (
        Widget w
         )
{
 ExmSimpleWidget sw = (ExmSimpleWidget)w;
 XGCValues   values;
 XtGCMask    valueMask;
 Arg         args[2];
 Pixmap      insensitiveStippleBitmap;

 /* This function creates two GC's: one to render a sensitive widget
    and the other to render an insensitive widget. */


 /* First, create the sensitive GC (named normal_GC). */ 
   valueMask = GCForeground | GCBackground | GCGraphicsExposures;
   values.foreground = sw->primitive.foreground;
   values.background = sw->core.background_pixel;
   values.graphics_exposures = False;
   sw->simple.normal_gc = XtGetGC ((Widget)sw, valueMask, &values);

 /* Next, create the insensitive GC.  This GC will share the same
    foreground, background, and graphics exposures as the sensitive
    GC, but will hold a different fill style and stipple pattern. 
    The stipple pattern is stored in the XmNinsensitiveStippleBitmap
    resource of XmScreen. */
   valueMask |= GCFillStyle | GCStipple;
   values.fill_style = FillStippled;

 /* Gather the Motif-appropriate insensitive stipple bitmap. */
   XtSetArg(args[0], XmNinsensitiveStippleBitmap, &insensitiveStippleBitmap); 
   XtGetValues(XmGetXmScreen(XtScreen(w)), args, 1);
   values.stipple = insensitiveStippleBitmap;

   sw->simple.insensitive_gc = XtGetGC((Widget)sw, valueMask, &values);
}



/******************************************************************************
 *
 *  DestroyGC:
 *      Called by the Destroy method.
 *
 *****************************************************************************/
static void 
DestroyGC ( 
        Widget w
          )
{
 ExmSimpleWidget sw = (ExmSimpleWidget)w;

 /* Deallocate the resources used by the two GC's. */ 
   XtReleaseGC ((Widget)sw, sw->simple.normal_gc);
   XtReleaseGC ((Widget)sw, sw->simple.insensitive_gc);
}



/******************************************************************************
 *
 *  SelectGC:
 *     Called by DrawVisual. 
 *
 *****************************************************************************/
static GC 
SelectGC (
        Widget w
         )
{
 ExmSimpleWidget sw = (ExmSimpleWidget)w;
 GC drawGC;

 /* Select between the normal (sensitive) GC and the insensitive GC. */ 
   drawGC = XtIsSensitive(w)  ? sw->simple.normal_gc 
                              : sw->simple.insensitive_gc;
   return (drawGC);
}



/******************************************************************************
 *
 *  CalcVisualSize: 
 *      Called by CalcWidgetSize.
 *
 *****************************************************************************/
static void 
CalcVisualSize (
        Widget w
               )
{
 ExmSimpleWidget sw = (ExmSimpleWidget)w;

 /* Calculate the ideal size of class's visual */
   sw->simple.visual.width = IDEAL_SHAPE_SIZE;
   sw->simple.visual.height = IDEAL_SHAPE_SIZE;
}



/******************************************************************************
 *
 *  CalcWidgetSize:
 *      Called by Reconfigure 
 *
 *****************************************************************************/
static void 
CalcWidgetSize(
        Widget w
              )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(w);
 ExmSimpleWidget sw = (ExmSimpleWidget)w;

 /* Call CalcVisualSize */
   if (wc->simple_class.calc_visual_size)
     (*(wc->simple_class.calc_visual_size))((Widget)sw);

 /* Compute the widget's width if asked to.  Otherwise, set the
    widget's width to the preferred width. */ 
   if (sw->simple.need_to_compute_width == True)
     sw->core.width = sw->simple.visual.width + 
                     (2 * (sw->simple.margin_width +
		           sw->primitive.shadow_thickness +
		           sw->primitive.highlight_thickness));
   else
     sw->core.width = sw->simple.pref_width;

 /* Compute the widget's height if asked to.  Otherwise, set the
    widget's height to the preferred height. */ 
   if (sw->simple.need_to_compute_height == True)
     sw->core.height = sw->simple.visual.height + 
                      (2 * (sw->simple.margin_height +
		            sw->primitive.shadow_thickness +
		            sw->primitive.highlight_thickness));
   else
     sw->core.height = sw->simple.pref_height;
}



/******************************************************************************
 *
 *  WidgetDisplayRect:
 *      Called by several Motif managers to determine how to align the visuals 
 *      drawn by primitives.  In addition, an application can access this 
 *      method by calling XmWidgetGetDisplayRect. 
 *
 *****************************************************************************/
static Boolean  
WidgetDisplayRect(
        Widget       w,
        XRectangle  *displayrect
                 )
{
 ExmSimpleWidget  my_widget = (ExmSimpleWidget) w;

   if ((my_widget->simple.visual.width > 0) && 
       (my_widget->simple.visual.height > 0)) {
   /* The widget is currently displaying a visual.  Write the dimensions
      of the visual's bounding box into displayrect. */ 
     displayrect->x =       my_widget->simple.visual.x; 
     displayrect->y =       my_widget->simple.visual.y; 
     displayrect->width =   my_widget->simple.visual.width; 
     displayrect->height =  my_widget->simple.visual.height; 
     return True;  /* Yes, this widget contains something displayable. */
   }
   else  {
     /* The widget is not currently displaying a visual. */
     return False; 
   }
}

/******************************************************************************
 *
 *  Reconfigure:
 *      Called by the Initialize and SetValues methods.  
 *
 *****************************************************************************/
static void
Reconfigure (
        WidgetClass class,
        Widget new_w,
        Widget old_w
                          )
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)class;
 ExmSimpleWidget nw = (ExmSimpleWidget)new_w;
 ExmSimpleWidget cw = (ExmSimpleWidget)old_w;

 /* If Reconfigure is called from a leaf chained method, the following code 
    calls CalcWidgetSize.   If Reconfigure is not called from a leaf chained
    method, wait until the final leaf method is called.  (That is because
    the subclass CalcWidgetSize method may need information derived in its 
    SetValues method.) */

 /* The Reconfigure method can only reconfigure an ExmSimple widget
    or an ExmSimple subclass. */
   if (!ExmIsSimple((Widget)nw)) 
     return;

   if (class == XtClass(nw)) {
     if (wc->simple_class.calc_widget_size)
       (*(wc->simple_class.calc_widget_size))((Widget)nw);

     nw->simple.pref_width  = nw->core.width;
     nw->simple.pref_height = nw->core.height;

   if (cw == NULL ||
      (nw->core.width  == cw->core.width && 
       nw->core.height == cw->core.height)
      ) {
     if (wc->core_class.resize) 
       (*(wc->core_class.resize))((Widget)nw);
     }
   }
   else 
     nw->simple.need_to_reconfigure = True;
}


/******--*********************** Trait Methods ****************--***********/

/******************************************************************************
 *
 *  ContItemSetValues 
 *      This is a trait method of the XmQTcontainterItem trait. 
 *
 *****************************************************************************/
static	void
ContItemSetValues(Widget w, 
		  XmContainerItemData contItemData)
{
    ExmSimpleWidget sw = (ExmSimpleWidget)w;

    /* A Container widget calls this trait method to set new visual attributes 
       on ExmSimple. */

    if (contItemData->valueMask & ContItemViewType) {
      /* We will treat SMALL_ICON as one shape (oval) and LARGE_ICON as 
         a different shape (rectangle). */
	if (contItemData->view_type == XmSMALL_ICON)
	    XtVaSetValues(w, ExmNsimpleShape, ExmSHAPE_OVAL, NULL);
	else
	    XtVaSetValues(w, ExmNsimpleShape, ExmSHAPE_RECTANGLE, NULL);
    }

    if (contItemData->valueMask & ContItemVisualEmphasis) {
	if (contItemData->visual_emphasis == XmSELECTED) {
	    /* ok, we are forced selected, so save current foreground
	       and then fetch the new select color and set it */

	    /* only save the foreground the first time thru */
	    if (sw->simple.saved_foreground == XmUNSPECIFIED_PIXEL)
		sw->simple.saved_foreground = sw->primitive.foreground ;

	    SetSelectedVisual  (w) ;

	} else {
	    /* put back our old foreground and forget saved_foreground.
	       The assumption is that the app will not set the foreground
	       during the time the widget is selected. */
	    XtVaSetValues(w, XmNforeground, sw->simple.saved_foreground, NULL);
	    sw->simple.saved_foreground = XmUNSPECIFIED_PIXEL ;
	}

    }
}


/******************************************************************************
 *
 *  ContItemGetValues 
 *      This is a trait method of the XmQTcontainterItem trait. 
 *
 *****************************************************************************/
static	void
ContItemGetValues(Widget w, 
		  XmContainerItemData contItemData)
{    
    ExmSimpleWidget sw = (ExmSimpleWidget)w;

    if (contItemData->valueMask & ContItemViewType) {
	if (sw->simple.simple_shape == ExmSHAPE_OVAL)
	    contItemData->view_type = XmSMALL_ICON;
	else 
	    contItemData->view_type = XmLARGE_ICON;
    }  

    if (contItemData->valueMask & ContItemVisualEmphasis) {
	/* we know awe are in a selected state if our foreground
	   is currently saved */
	if (sw->simple.saved_foreground == XmUNSPECIFIED_PIXEL)
	    contItemData->visual_emphasis = XmNOT_SELECTED;
	else 
	    contItemData->visual_emphasis = XmSELECTED;
    }

    if (contItemData->valueMask & ContItemIconWidth) {
	contItemData->icon_width = sw->core.width - sw->core.x;
    }

    if (contItemData->valueMask & ContItemDetailCount) {
	contItemData->detail_count = 0;
    }

}


/******************************************************************************
 *
 *  SetSelectedVisual
 *      Get the select color of the parent and set our foreground 
 *
 *****************************************************************************/

static void
SetSelectedVisual (
	Widget wid)
{
    /* our parent is notifying us that its select color has changed
       so let's try to get the new value using the Container trait
       getValue method and then change our foreground - somehow, there 
       is the  assumption that if this method gets called with SelectColor
       on, then it must be a Container, which is ok in 2.0 */
    XmContainerDataRec container_data ;
    XmContainerTrait container_trait ;
    Pixel select_color = XmUNSPECIFIED_PIXEL;
    ExmSimpleWidget sw = (ExmSimpleWidget)wid;

    /* get the container trait record */
    container_trait = (XmContainerTrait) 
	XmeTraitGet((XtPointer)
		    XtClass(XtParent(wid)), 
		    XmQTcontainer) ;

    if (container_trait) {
	/* get the container information */
	container_data.valueMask = ContSelectColor ;
	container_trait->getValues(XtParent(wid), &container_data);

	if (container_data.valueMask & ContSelectColor) {
	    /* if the Container uses reverse ground, convert that
	       to using black pixel */
	    if (container_data.select_color == XmREVERSED_GROUND_COLORS)
		select_color = BlackPixelOfScreen(XtScreen(wid));
	    else
		select_color = container_data.select_color;
	} 
    }

    /* only set the foreground of the one selected */
    if ((select_color != XmUNSPECIFIED_PIXEL) &&
	(sw->simple.saved_foreground != XmUNSPECIFIED_PIXEL))
	XtVaSetValues(wid, XmNforeground, select_color, NULL) ;

}


/******************************************************************************
 *
 *  HandleRedraw 
 *      This is a trait method of the XmQTcareParentVisual trait. 
 *
 *****************************************************************************/

static Boolean 
HandleRedraw (
	Widget kid, 	       
	Widget cur_parent,
	Widget new_parent,
	Mask visual_flag)
{
    Boolean redraw = False;
    XmCareVisualTrait care_visual ;

    if (visual_flag & VisualSelectColor) {

	SetSelectedVisual (kid) ;

	/* no need to set redraw to True since SetSelectedVisual calls
	   XtSetValues which already re-exposes when foreground changes */
    }

    /* at this point we need to envelop our superclass trait method
       since we are overiding it but we still want its job to be done */
    care_visual = (XmCareVisualTrait)
	XmeTraitGet(xmPrimitiveWidgetClass, XmQTcareParentVisual) ;

    /* care_visual never NULL on Primitive ! */
    redraw |= care_visual->redraw(kid, cur_parent, new_parent, visual_flag) ;

    return redraw ;
}



/******************************************************************************
 *
 *  ExmCreateSimple:
 *       Called by an application.
 *
 *****************************************************************************/
Widget
ExmCreateSimple (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                       )
{
 /* Convenience function to instantiate an ExmSimple widget. */
   return (XtCreateWidget (name, exmSimpleWidgetClass, parent, 
                           arglist, argCount));
}

