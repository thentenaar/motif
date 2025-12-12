/* $XConsortium: CxxSimple.cxx /main/4 1995/07/17 11:45:27 drk $ */
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
 * CxxSimple.cc - ExmCxxSimple widget.  This widget renders a rectangle or
 *                an oval. 
 *            The ExmCxxSimple widget demonstrates how to create a
 *            subclass of CxxPrimitive that can, itselve, serve as a superclass
 *            for other widgets.  ExmCxxSimple also demonstrates how to  
 *               * create a new representation type (ExmRSimpleShape).
 *               * install the XmQTcontainerItem trait and its trait methods.
 *               * install the XmQTcareParentVisual trait and methods.
 *               * access the trait methods of the XmQTcontainer trait. 
 *            See the "OSF/Motif Widget Writer's Guide" for more details.
 *
******************************************************************************/
// Include appropriate files.
#include "CxxSimple.h"		// header file of this widget
#include <Xm/DrawP.h>       	// header file for Xme drawing functions
#include <Xm/RepType.h>     	// header file for representation type facility
#include <Xm/Screen.h>      	// header file for screen information
#include <Xm/TraitP.h>      	// header file for installing traits

// The following section defines constants for the widget.  
// The IDEAL_SHAPE_SIZE constant defines the starting preferred width 
// and height for the widget visual (which is either a rectangle or an oval). 
static XmConst int IDEAL_SHAPE_SIZE  = 30;
static XmConst int FIND_NATURAL_SIZE = 0;

// Define the resources array.  Define this array just as you would in C. 
static XtResource resources[] = 
{
    {
	ExmNsimpleShape,  /* resource name */
	ExmCSimpleShape,  /* resource class */
	ExmRSimpleShape,  /* resource data type */
	sizeof (unsigned char), /* all representation types have this size */
	XtOffsetOf(ExmCxxSimple, simple.simple_shape), /* offset */
	XmRImmediate,  /* default value appears on next line */
	(XtPointer)ExmSHAPE_OVAL /* default shape is oval */ 
    },
    {
 	XmNmarginWidth, 
	XmCMarginWidth, 
	XmRHorizontalDimension, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxSimple, simple.margin_width), 
	XmRImmediate,
	(XtPointer) 4
    },
    {
	XmNmarginHeight, 
	XmCMarginHeight, 
	XmRVerticalDimension, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxSimple, simple.margin_height),
	XmRImmediate,
	(XtPointer) 4
    },
};


// Two of the three resources will also be handled as synthetic resources. 
static XmSyntheticResource syn_resources[] =
{
    { 
	XmNmarginWidth, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxSimple, simple.margin_width), 
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels 
    },
    { 
	XmNmarginHeight, 
	sizeof (Dimension),
	XtOffsetOf(ExmCxxSimple, simple.margin_height),
	XmeFromVerticalPixels, 
	XmeToVerticalPixels 
    },
};


/////////////////////////////////////
// Establish the widget metaclass. //          
/////////////////////////////////////
// Here is the constructor for the widget metaclass.
ExmCxxSimpleClass::ExmCxxSimpleClass(char*		name,
				     WidgetClass	parentClass,
				     Cardinal		widgetSize,
				     XtProc		class_init,
				     XtActionList	actions,
				     Cardinal		num_actions,
				     XtResourceList	resources,
				     Cardinal		num_resources,
				     XtInitProc		cxx_cast)
   : XmCxxPrimitiveClass(name, parentClass, widgetSize, class_init,
			 actions, num_actions, resources, num_resources, cxx_cast)
{
 // Specify the data member fields of Core that need overriding. 
 core_class.tm_table = XtInheritTranslations;
 core_class.compress_exposure = XtExposeCompressMaximal;

 // Specify the data member fields of XmPrimitive that need overriding.
 // syn_resources array and num_syn_resources should only be set for the
 // class implementing them and should not be set in subclasses. 
 if (parentClass == (WidgetClass) &xmCxxPrimitiveClassRec) {	
   primitive_class.syn_resources = syn_resources;
   primitive_class.num_syn_resources = XtNumber(syn_resources);
 }

 // Specify the data member fields of ExmSimple. 
 cxx_simple_extension		= NULL;
};


// Instantiate a widget metaclass.  The constructor for the widget
// metaclass requires eight arguments.
externaldef(exmCxxSimpleclassrec)
ExmCxxSimpleClass exmCxxSimpleClassRec("ExmCxxSimple",
				       (WidgetClass) &xmCxxPrimitiveClassRec,
				       sizeof(ExmCxxSimple),
				       ExmCxxSimpleClass::ClassInitialize,
				       NULL,	// actions
				       0,	// num_actions
				       resources,
				       XtNumber(resources),
				       ExmCxxSimple::_MakeCxxWidget);

// Associate the widget class name (two of them, actually) with the 
// widget metaclass.  When an application uses exmCxxSimpleWidgetClass
// or ExmCxxSimpleWidgetClass, the widget metaclass will be instantiated. 
externaldef(exmCxxSimplewidgetclass)
WidgetClass exmCxxSimpleWidgetClass = (WidgetClass) &exmCxxSimpleClassRec;

// Declare trait record variables.

// Declare a trait record variable for the XmQTcontainerItem trait. 
static XmConst XmContainerItemTraitRec simpleCIT = {
  0,			// version
  ExmCxxSimple::ContItemSetValues,
  ExmCxxSimple::ContItemGetValues,
};

// Declare a trait record variable for the XmQTcareParentVisual trait. 
static XmConst XmCareVisualTraitRec simpleCVT = {
  0,			// version
  ExmCxxSimple::HandleRedraw,
};


// Define global variables here. 
// The SimpleShapeNames variable holds some normalized values.
// The XmRepTypeRegister function will use these values to determine the
// legal values for a given representation type. 
static String SimpleShapeNames[] = {
	"simple_oval",
	"simple_rectangle"
};

static XmRepTypeId simpleShapeId;

/******************************************************************************
 *
 *  ClassInitialize:
 *      Called the first time a widget of this class is instantiated.
 *
 *****************************************************************************/
void ExmCxxSimpleClass::ClassInitialize(void)
{
 // The following statement is not required in the C version of this widget;
 // however, you must provide this statement in the C++ version.
   exmCxxSimpleClassRec.SetBaseClassExtensionQuark();

 // The ExmNsimpleShape resource requires a value of data type ExmRSimpleShape.
 // Since this is not a standard Motif data type, we must create this
 // data type just for the ExmSimple widget.
 // We want ExmRSimpleShape to hold an enumerated value, either 
 // ExmSIMPLE_OVAL or ExmSIMPLE_RECTANGLE.  The best way to accomplish this
 // is to register the new data type with the representation type facility. 
   simpleShapeId = XmRepTypeRegister (ExmRSimpleShape, SimpleShapeNames,
				      NULL, XtNumber(SimpleShapeNames));
}

/******************************************************************************
 *
 *  class_part_initialize
 *
 *****************************************************************************/
void ExmCxxSimpleClass::class_part_initialize(void)
{
  // Before anything else, do the downward chaining;
  XmCxxPrimitiveClass::class_part_initialize();

  // Install the containerItem trait on ExmSimple and all its subclasses.
  XmeTraitSet((XtPointer) this, XmQTcontainerItem, (XtPointer) &simpleCIT);

  // Install the careParentVisual trait on ExmSimple and all subclasses. 
  XmeTraitSet((XtPointer) this, XmQTcareParentVisual, (XtPointer) &simpleCVT);
}

/******************************************************************************
 *
 *  Initialize
 *
 *****************************************************************************/
void ExmCxxSimple::initialize (Widget request_w, 
			       ArgList args, 
                               Cardinal *num_args)
{
 ExmCxxSimple *rw = (ExmCxxSimple *) request_w;
  
 // Simulate chaining by calling the initialize method of ExmCxxSimple's
 // superclass (XmCxxPrimitive).
 XmCxxPrimitive::initialize(request_w, args, num_args);

 // Look at the value assigned to the ExmNsimpleShape resource.  If 
 // value assigned is neither ExmSHAPE_OVAL nor ExmSHAPE_RECTANGLE, then
 // issue a warning message and set ExmNsimpleShape to ExmSHAPE_OVAL.
 if (!XmRepTypeValidValue (simpleShapeId, simple.simple_shape, 
			   (Widget) this))
   simple.simple_shape = ExmSHAPE_OVAL;

 // Call a function that creates the GC's needed by the widget.
 create_gc();
  
 // If the widget's width is set to 0, that's a trigger for the widget 
 // to calculate its width.  We are not actually going to calculate
 // its width here, rather we are going to set the need_to_compute_width
 // flag appropriately. 
 if (rw->core.width == FIND_NATURAL_SIZE) {
   simple.need_to_compute_width = True; 
 } else {
   simple.need_to_compute_width = False;
   simple.pref_width = rw->core.width;
   core.width = rw->core.width;
 }

 // Same for the height 
 if (rw->core.height == FIND_NATURAL_SIZE) {
   simple.need_to_compute_height = True;
 } else {
   simple.need_to_compute_height = False;
   simple.pref_height = rw->core.height;
   core.height = rw->core.height;
 }

 // mark us as not selected to start with
 simple.saved_foreground = XmUNSPECIFIED_PIXEL ;

 // In order not to resize the widget at each level of the Initialize
 // chain, the actual class is passed to this method
 reconfigure(exmCxxSimpleWidgetClass, NULL);
}

/************************************************************************
 *
 * _MakeCxxWidget:
 *     Call 'new' to make each widget a C++ object.
 * 
 ************************************************************************/
void ExmCxxSimple::_MakeCxxWidget(Widget,
                                  Widget new_w, 
				  ArgList, Cardinal*) 
{
  ExmCxxSimpleWidget dummy = IDLE_NEW(new_w) ExmCxxSimple;
}


/************************************************************************
 *
 *  destructor
 *      The destroy method of C widgets is implemented in C++ as a 
 *      destructor.
 *
 ************************************************************************/
ExmCxxSimple::~ExmCxxSimple()
{
  // The ExmSimple widget allocates two internal GC's.  In order to prevent
  // memory leaks, we must destroy these GC's.  
  destroy_gc(); 
}

/*****************************************************************************
 * 
 * realize
 *      Called by the Intrinsics to create the window for the widget.  This
 *      class's realize method creates a propagating window for this 
 *      exact class,  but uses the default window otherwise 
 *
 *****************************************************************************/
void ExmCxxSimple::realize(XtValueMask *p_valueMask,
			   XSetWindowAttributes *attributes )
{
   Mask valueMask = *p_valueMask;

   /* First call Primitive's method */
   XmCxxPrimitive::realize(p_valueMask, attributes);

   /* ExmSimple wants to propagate all unused events to its hierarchy, which
      Primitive.Realize doesn't do.  So if this is an ExmSimple widget, 
      we fix the do_not_propagate window attribute */
   if (XtClass((Widget) this) == exmCxxSimpleWidgetClass) {
     Mask adjustMask;
     XSetWindowAttributes xswa;

     adjustMask = CWDontPropagate;
     xswa.do_not_propagate_mask = NoEventMask;
     XChangeWindowAttributes(XtDisplay((Widget) this), 
			     XtWindow((Widget) this), 
			     adjustMask, &xswa);
   }
}

/************************************************************************
 *
 *  resize
 *
 ************************************************************************/
void ExmCxxSimple::resize(void)
{
 Dimension  mw, mh;
 Dimension  window_decoration_thickness;
 Dimension  total_target_widget_width, total_target_widget_height;

 // Calculate the size of the visual. 
 calc_visual_size();

 // Calculate some sizes that will be useful later on. 
 window_decoration_thickness = primitive.highlight_thickness  +
                               primitive.shadow_thickness;

 mw = window_decoration_thickness + simple.margin_width;
 total_target_widget_width = (2 * mw) + simple.visual.width;

 // When there is not enough space to display the entire widget, we
 // must clip part of the widget, but which part? 

 if (core.width >= total_target_widget_width) {
   /* We have enough space to display everything (the visual, the margins, 
      and the border decorations). */ 
   simple.visual.x = mw;
   simple.visual.width = core.width - (2 * mw);
 } else if (core.width > ((2 * window_decoration_thickness) + 
                               simple.visual.width)) {
   /* We do not have enough space to display everything, but we do have
      enough space to display the visual and the border decorations.
      The left and right margins will have to be reduced. */ 
   simple.visual.x = (core.width - simple.visual.width)/2;
 } else if (core.width > 2 * window_decoration_thickness) {
   /* Space is very tight.  We will eliminate the left and right margins
      all together.  Furthermore, we will reduce the size of the visual.  */  
   simple.visual.x = window_decoration_thickness;
   simple.visual.width = core.width - (2 * window_decoration_thickness);
 } else {
   /* We do not have enough space to display even one pixel of the visual. */ 
   simple.visual.width = 0;
 }

 /* Now do the same for the vertical dimension. */
 mh = window_decoration_thickness + simple.margin_height;
 total_target_widget_height = (2 * mh) + simple.visual.height;

 if (core.height >= total_target_widget_height) {
   /* We have enough space to display everything (the visual, the margins, 
      and the border decorations). */ 
   simple.visual.y = mh;
   simple.visual.height = core.height - (2 * mh);
 } else if (core.height > 
	    ((2 * window_decoration_thickness) + simple.visual.height)) {
   /* We do not have enough space to display everything, but we do have
      enough space to display the visual and the border decorations.
      The top and bottom margins will have to be reduced. */ 
   simple.visual.y = (core.height - simple.visual.height)/2;
 } else if (core.height > 2 * window_decoration_thickness) {
   /* Space is very tight.  We will eliminate the top and right margins
      all together.  Furthermore, we will reduce the size of the visual.  */  
   simple.visual.y = window_decoration_thickness;
   simple.visual.height = core.height - (2 * window_decoration_thickness);
 } else {
   /* We do not have enough space to display even one pixel of the visual. */ 
   simple.visual.height = 0;
 }
}

/************************************************************************
 *
 *  redisplay
 *     Called by the Intrinsics whenever a portion of the widget that was
 *     obscured becomes exposed.
 *
 ***********************************************************************/
void ExmCxxSimple::expose(XEvent *event, Region region)
{
   draw_visual();  // Call the function that draws the widget visual.

   draw_shadow();  // Call the function that draws the widget shadow.

 // Envelop our superclass expose method.  The superclass expose
 // method knows how to draw the border highlight. 
   XmCxxPrimitive::expose(event, region);
}

/******************************************************************************
 *
 *  setvalues
 *
 *****************************************************************************/
Boolean ExmCxxSimple::set_values (Widget old_w, 
				  Widget request_w, 
				  ArgList args, 
				  Cardinal *num_args)
{
 ExmCxxSimple *cw = (ExmCxxSimple *)old_w;
 ExmCxxSimple *rw = (ExmCxxSimple *)request_w;
 Boolean redisplayFlag = False;

 // Simulate chaining by explicitly calling the set_values method of 
 // XmCxxPrimitive.  (XmCxxPrimitive is the superclass of ExmCxxSimple.)
 XmCxxPrimitive::set_values(old_w, request_w, args, num_args); 

 // Validate the value of ExmNsimpleShape by calling XmRepTypeValidValue.
 if (simple.simple_shape != cw->simple.simple_shape) {
   if (!XmRepTypeValidValue (simpleShapeId, simple.simple_shape, 
			     (Widget) this))
     simple.simple_shape = cw->simple.simple_shape;
   else
     redisplayFlag = True;
 }

 // Redisplay on change in sensitivity 
 if (XtIsSensitive((Widget) this) != XtIsSensitive(old_w)) 
   redisplayFlag = True;

 /* If the widget's foreground or background color changes, 
    then we must update the GC. */ 
 if (primitive.foreground != primitive.foreground ||
     core.background_pixel != core.background_pixel) {
   destroy_gc();
   create_gc();
   redisplayFlag = True;
 }

 /* Check for application geometry settings. '0' means 'ideal size' */
 if (rw->core.width == FIND_NATURAL_SIZE) {
   core.width = FIND_NATURAL_SIZE;
   simple.need_to_compute_width = True;
 } else if (rw->core.width != cw->core.width) {
   core.width = rw->core.width;
   simple.pref_width = rw->core.width;
   simple.need_to_compute_width = False;
 }
 
 if (rw->core.height == FIND_NATURAL_SIZE) {
   core.height = FIND_NATURAL_SIZE;
   simple.need_to_compute_height = True;
 }
 else if (rw->core.height != cw->core.height) {
   core.height = rw->core.height;
   simple.pref_height = rw->core.height;
   simple.need_to_compute_height = False;
 }

 // Check for any changes in total widget set, margin size, or 
 // window decoration size.  If any are found, call Reconfigure. 
 simple.need_to_reconfigure = False;
 if (core.width != cw->core.width ||
     core.height != cw->core.height ||
     simple.margin_width != cw->simple.margin_width ||
     simple.margin_height != cw->simple.margin_height ||
     primitive.shadow_thickness != cw->primitive.shadow_thickness ||
     primitive.highlight_thickness != cw->primitive.highlight_thickness
     ) 
   reconfigure (exmCxxSimpleWidgetClass, old_w);

 return (redisplayFlag);
}

/************************************************************************
 *
 *  QueryGeometry
 *     Called by the Intrinsics in response to a proposed changed in geometry.
 *
 ************************************************************************/
XtGeometryResult ExmCxxSimple::query_geometry (XtWidgetGeometry *request, 
                                               XtWidgetGeometry *reply)

{
  Widget w = (Widget) this;

  if (!XtIsRealized(w)) { /* Simple hasn't been realized yet. */ 
    reply->width  = XtWidth(w);   /* might be 0 */
    reply->height = XtHeight(w);  /* might be 0 */
  } else {
    /* Widget has already been realized.  Setting the width and height to '0' 
       means to compute the natural size of the widget. */
    reply->width  = simple.pref_width;
    reply->height = simple.pref_height;
  }    
  
  // Return our preferred size. 
  return XmeReplyToQueryGeometry(w, request, reply);
}

/******************************************************************************
 *
 *  DrawVisual:
 *     Called by ExmSimple whenever it needs to draw or redraw the
 *     widget visual (either an oval or a rectangle).
 *
 *****************************************************************************/
void ExmCxxSimple::draw_visual(void)
{
 Widget w = (Widget)this;

 // Draw the oval or the rectangle, space permitting. 
   if (simple.visual.width > 0 && simple.visual.height > 0) {
     switch (simple.simple_shape) {
	    case(ExmSHAPE_OVAL):
	    default:
		XFillArc (XtDisplay(w), XtWindow(w),
                          select_gc(),
			  simple.visual.x, simple.visual.y,
			  simple.visual.width, simple.visual.height,
			  0, 360 * 64);
		break;
	    case(ExmSHAPE_RECTANGLE):
		XFillRectangle (XtDisplay(w), XtWindow(w),
                           select_gc(),
                           simple.visual.x, simple.visual.y,
                           simple.visual.width, simple.visual.height);
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
void ExmCxxSimple::draw_shadow(void)
{
  Widget w = (Widget)this;

  // If there is enough room in the widget to draw a shadow, 
  // and if the shadow thickness resource is nonzero, then draw a 
  // Motif-style shadow in the appropriate place around the widget's border. 
  if (core.width  > (2 * primitive.highlight_thickness) &&
      core.height > (2 * primitive.highlight_thickness) &&
      primitive.shadow_thickness > 0) 
    XmeDrawShadows (XtDisplay(w), XtWindow(w),
		    primitive.top_shadow_GC,
		    primitive.bottom_shadow_GC,
		    primitive.highlight_thickness,
		    primitive.highlight_thickness,
		    core.width - (2 * primitive.highlight_thickness),
		    core.height - (2 * primitive.highlight_thickness),
		    primitive.shadow_thickness, 
		    XmSHADOW_ETCHED_OUT);
}


/************************************************************************
 *
 *  CreateGC:
 *       Called by Initialize and by SetValues.
 *
 ************************************************************************/
void ExmCxxSimple::create_gc(void)
{
 XGCValues   values;
 XtGCMask    valueMask;
 Arg         args[2];
 Pixmap      insensitiveStippleBitmap;

 /* This function creates two GC's: one to render a sensitive widget
    and the other to render an insensitive widget. */


 /* First, create the sensitive GC (named normal_GC). */ 
 valueMask = GCForeground | GCBackground | GCGraphicsExposures;
 values.foreground = primitive.foreground;
 values.background = core.background_pixel;
 values.graphics_exposures = False;
 simple.normal_gc = XtGetGC ((Widget) this, valueMask, &values);

 /* Gather the Motif-appropriate insensitive stipple bitmap. */
 XtSetArg(args[0], XmNinsensitiveStippleBitmap, &insensitiveStippleBitmap); 
 XtGetValues(XmGetXmScreen(XtScreen((Widget) this)), args, 1);
 values.stipple = insensitiveStippleBitmap;

 simple.insensitive_gc = XtGetGC((Widget) this, valueMask, &values);
}



/************************************************************************
 *
 *  DestroyGC:
 *      Called by the destructor.
 *
 ************************************************************************/
void ExmCxxSimple::destroy_gc(void)
{
 Widget w = (Widget)this;

 // Destroy GC's.
 XtReleaseGC (w, simple.normal_gc);
 XtReleaseGC (w, simple.insensitive_gc);
}

/************************************************************************
 *
 *  SelectGC
 *
 ************************************************************************/
GC ExmCxxSimple::select_gc(void)
{
  Widget w = (Widget)this;
  GC drawGC;
  
  // Select between the normal (sensitive) GC and the insensitive GC.
  drawGC = XtIsSensitive(w) ? simple.normal_gc : simple.insensitive_gc;
  
  return (drawGC);
}



/************************************************************************
 *
 *  CalcVisualSize:
 *       Called by CalcWidgetSize. 
 *
 ************************************************************************/
void ExmCxxSimple::calc_visual_size(void)
{
  // Calculate ideal size of class's visual. 
  simple.visual.width = IDEAL_SHAPE_SIZE;
  simple.visual.height = IDEAL_SHAPE_SIZE;
}

/******************************************************************************
 *
 *  CalcWidgetSize:
 *      Called by Reconfigure. 
 *
 *****************************************************************************/
void ExmCxxSimple::calc_widget_size(void)
{

 calc_visual_size();  
     
 // Compute the widget's width if asked to.  Otherwise, set the
 // widget's width to the preferred width. 
 if (simple.need_to_compute_width)
   core.width = simple.visual.width + (2 * (simple.margin_width +
					    primitive.shadow_thickness +
					    primitive.highlight_thickness));
   else
     core.width = simple.pref_width;

 // Compute the widget's height if asked to.  Otherwise, set the
 // widget's height to the preferred height. 
 if (simple.need_to_compute_height)
   core.height = simple.visual.height + (2 * (simple.margin_height +
					      primitive.shadow_thickness +
					      primitive.highlight_thickness));
   else
     core.height = simple.pref_height;
}

/******************************************************************************
 *
 *  WidgetDisplayRect:
 *      Called by several Motif managers to determine how to align the visuals 
 *      drawn by primitives.  In addition, an application can access this 
 *      method by calling XmWidgetGetDisplayRect. 
 *
 *****************************************************************************/
Boolean  ExmCxxSimple::widget_display_rect(XRectangle  *displayrect)
{
   if ((simple.visual.width > 0) && 
       (simple.visual.height > 0)) {
   // The widget is currently displaying a visual.  Write the dimensions
   // of the visual's bounding box into displayrect. 
     displayrect->x =       simple.visual.x; 
     displayrect->y =       simple.visual.y; 
     displayrect->width =   simple.visual.width; 
     displayrect->height =  simple.visual.height; 
     return True;  /* Yes, this widget contains something displayable. */
   }
   else  {
     /* The widget is not currently displaying a visual. */
     return False; 
   }
}

/************************************************************************
 *
 *  Reconfigure
 *
 ************************************************************************/
void ExmCxxSimple::reconfigure(WidgetClass wc, 
                              Widget old_w)
{
 ExmCxxSimple *sw = (ExmCxxSimple *) old_w;
 Widget w = (Widget)this;

 // If Reconfigure is called from a leaf chained method, the following code 
 // calls CalcWidgetSize.   If Reconfigure is not called from a leaf chained
 // method, wait until the final leaf method is called.  (That is because
 // the subclass CalcWidgetSize method may need information derived in its 
 // SetValues method.) 

  if (wc == XtClass(w)) {
    calc_widget_size();

    simple.pref_width = core.width;
    simple.pref_height = core.height;

    if (sw == NULL ||
	(core.width == sw->core.width &&
	 core.height == sw->core.height)) {
      resize();
    }
  } 
  else 
    simple.need_to_reconfigure = True;
}


/******--*********************** Trait Methods ****************--***********/

/******************************************************************************
 *
 *  ContItemSetValues 
 *      This is a trait method of the XmQTcontainterItem trait. 
 *
 *****************************************************************************/
void ExmCxxSimple::ContItemSetValues(Widget w, 
				     XmContainerItemData contItemData)
{
  ExmCxxSimple *sw = (ExmCxxSimple *) w;

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
void ExmCxxSimple::ContItemGetValues(Widget w, 
				     XmContainerItemData contItemData)
{    
    ExmCxxSimple *sw = (ExmCxxSimple *) w;

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
void ExmCxxSimple::SetSelectedVisual (Widget wid)
{
  /* our parent is notifying us that its select color has changed
     so let's try to get the new value using the Container trait
     getValue method and then change our foreground - somehow, there 
     is the  assumption that if this method gets called with SelectColor
     on, then it must be a Container, which is ok in 2.0 */
  XmContainerDataRec container_data ;
  XmContainerTrait container_trait ;
  Pixel select_color = XmUNSPECIFIED_PIXEL;
  ExmCxxSimple *sw = (ExmCxxSimple *) wid;
  
  /* get the container trait record */
  container_trait = (XmContainerTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTcontainer) ;
  
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
Boolean  ExmCxxSimple::HandleRedraw (Widget kid, Widget cur_parent,
				     Widget new_parent, Mask visual_flag)
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
 *  ExmCreateCxxSimple
 *
 *****************************************************************************/
Widget ExmCreateCxxSimple (Widget parent, char *name,
			   ArgList args, Cardinal argCount)
{
 // Convenience function to instantiate an ExmSimple widget. 
   return (XtCreateWidget (name, exmCxxSimpleWidgetClass, parent, 
			   args, argCount));
}
