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
 */

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ArrowBG.c /main/20 1999/01/26 15:30:15 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <Xm/ArrowBGP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/TraitP.h>
#include <Xm/AccColorT.h>
#include <Xm/ActivatableT.h>
#include <Xm/CareVisualT.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "PixConvI.h"
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmI.h"

#define DELAY_DEFAULT	100
#define INVALID_PIXEL	((Pixel) -1)
#define INVALID_PIXMAP	((Pixmap) -1)

/********    Static Function Declarations    ********/

static Pixmap GetHighlightPixmapDefault(XmArrowButtonGadget ag);
static Pixmap GetTopShadowPixmapDefault(XmArrowButtonGadget ag);

static void ClassPartInitialize(WidgetClass wc);
static void Initialize(Widget    rw,
		       Widget    nw,
		       ArgList   args,
		       Cardinal *num_args);
static void GetArrowGC(XmArrowButtonGadget ag);
static void GetBackgroundGC(XmArrowButtonGadget ag);
static void Redisplay(Widget w, XEvent *event, Region region);
static void Destroy(Widget w);
static Boolean SetValues(Widget    cw,
			 Widget    rw,
			 Widget    nw,
			 ArgList   args,
			 Cardinal *num_args);
static void HighlightBorder(Widget w);
static void InputDispatch(Widget wid, XEvent *event, Mask event_mask);
static void Arm(XmArrowButtonGadget aw, XEvent *event);
static void Activate(Widget    wid,
		     XEvent   *event,
		     String   *params,
		     Cardinal *num_params);
static void ArmAndActivate(Widget    w,
			   XEvent   *event,
			   String   *params,
			   Cardinal *num_params);
static void ArmTimeout(XtPointer data, XtIntervalId *id);
static void Disarm(XmArrowButtonGadget aw, XEvent *event);
static void Enter(XmArrowButtonGadget aw, XEvent *event);
static void Leave(XmArrowButtonGadget aw, XEvent *event);
static void Help(XmArrowButtonGadget aw, XEvent *event);
static void ActivateCommonG(XmArrowButtonGadget ag,
			    XEvent             *event,
			    Mask                event_mask);

static void ChangeCB(Widget         w,
		     XtCallbackProc activCB,
		     XtPointer      closure,
		     Boolean        setunset);
static Boolean HandleRedraw(Widget kid,
			    Widget cur_parent,
			    Widget new_parent,
			    Mask   visual_flag);
static void InitNewColorBehavior(XmArrowButtonGadget ag);
static void DealWithColors(XmArrowButtonGadget ag);
static void DealWithPixmaps(XmArrowButtonGadget ag);
static void InitNewPixmapBehavior(XmArrowButtonGadget ag);

static void DrawArrowG(XmArrowButtonGadget ag,
		       GC                  top_gc,
		       GC                  bottom_gc,
		       GC                  center_gc);
static void GetColors(Widget widget, XmAccessColorData color_data);

/********    End Static Function Declarations    ********/

/*  Resource list for Arrow  */

static XtResource resources[] =
{
  {
    XmNmultiClick, XmCMultiClick, XmRMultiClick,
    sizeof(unsigned char),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.multiClick),
    XmRImmediate, (XtPointer) XmMULTICLICK_KEEP
  },

  {
    XmNarrowDirection, XmCArrowDirection, XmRArrowDirection,
    sizeof(unsigned char),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.direction),
    XmRImmediate, (XtPointer) XmARROW_UP
  },

  {
    XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.activate_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.arm_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNdisarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.disarm_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNbackground, XmCBackground, XmRPixel,
    sizeof(Pixel),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.background),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNforeground, XmCForeground, XmRPixel,
    sizeof(Pixel),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.foreground),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
    sizeof(Pixel),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.top_shadow_color),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
    sizeof(Pixmap),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.top_shadow_pixmap),
    XmRImmediate, (XtPointer) INVALID_PIXMAP
  },

  {
    XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
    sizeof(Pixel),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.bottom_shadow_color),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
    sizeof(Pixmap),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.bottom_shadow_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNhighlightColor, XmCHighlightColor, XmRPixel,
    sizeof(Pixel),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.highlight_color),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNhighlightPixmap, XmCHighlightPixmap, XmRNoScalingDynamicPixmap,
    sizeof(Pixmap),
    XtOffsetOf(struct _XmArrowButtonGadgetRec, arrowbutton.highlight_pixmap),
    XmRImmediate, (XtPointer) INVALID_PIXMAP
  },
   {
     XmNdetailShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf(XmArrowButtonGadgetRec, arrowbutton.detail_shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   }
};

static XmSyntheticResource syn_resources[] =
{
    {
        XmNdetailShadowThickness,
        sizeof(Dimension),
        XtOffsetOf(XmArrowButtonGadgetRec, arrowbutton.detail_shadow_thickness),
        XmeFromHorizontalPixels,XmeToHorizontalPixels
    }
};


/*  The Arrow class record definition  */

externaldef(xmarrowbuttongadgetclassrec) XmArrowButtonGadgetClassRec
	xmArrowButtonGadgetClassRec =
{
  { /* object fields */
    (WidgetClass) &xmGadgetClassRec,	/* superclass            */
    "XmArrowButtonGadget",		/* class_name	         */
    sizeof(XmArrowButtonGadgetRec),	/* widget_size	         */
    (XtProc)NULL,			/* class_initialize      */
    ClassPartInitialize,		/* class_part_initialize */
    FALSE,				/* class_inited          */
    Initialize,				/* initialize	         */
    (XtArgsProc)NULL,			/* initialize_hook       */
    NULL,				/* realize	         */
    NULL,				/* actions               */
    0,			        	/* num_actions    	 */
    resources,				/* resources	         */
    XtNumber(resources),		/* num_resources         */
    NULLQUARK,				/* xrm_class	         */
    TRUE,				/* compress_motion       */
    XtExposeCompressMaximal,		/* compress_exposure     */
    TRUE,				/* compress_enterleave   */
    FALSE,				/* visible_interest      */
    Destroy,				/* destroy               */
    (XtWidgetProc)NULL,			/* resize                */
    Redisplay,				/* expose                */
    SetValues,				/* set_values	         */
    (XtArgsFunc)NULL,			/* set_values_hook       */
    XtInheritSetValuesAlmost,		/* set_values_almost     */
    (XtArgsProc)NULL,			/* get_values_hook       */
    NULL,				/* accept_focus          */
    XtVersion,				/* version               */
    (XtPointer)NULL,			/* callback private      */
    (String)NULL,			/* tm_table              */
    (XtGeometryHandler)NULL,		/* query_geometry        */
    NULL,				/* display_accelerator   */
    (XtPointer)NULL			/* extension             */
  },

  { /* XmGadget fields */
    HighlightBorder,			/* border highlight   */
    XmInheritBorderUnhighlight,		/* border_unhighlight */
    ArmAndActivate,			/* arm_and_activate   */
    InputDispatch,			/* input dispatch     */
    XmInheritVisualChange,		/* visual_change      */
    syn_resources,         		/* syn_resources */
    XtNumber(syn_resources),	        /* num_syn_resources */
    (XmCacheClassPartPtr)NULL,		/* class cache part   */
    (XtPointer)NULL			/* extension          */
  },

  { /* XmArrowButtonGadget fields */
    (XtPointer)NULL			/* extension	      */
  }
};

externaldef(xmarrowbuttongadgetclass) WidgetClass xmArrowButtonGadgetClass =
   (WidgetClass) &xmArrowButtonGadgetClassRec;


/* Trait record for arrowBG */
static const XmActivatableTraitRec arrowBGAT =
{
  0,				/* version	*/
  ChangeCB			/* changeCB	*/
};

static const XmCareVisualTraitRec arrowBGCVT =
{
  0,				/* version	*/
  HandleRedraw			/* redraw	*/
};

/* Access Colors Trait record for arrow button gadget */
static const XmAccessColorsTraitRec arrowBGACT =
{
  0,				/* version	*/
  GetColors,			/* getColors	*/
  NULL				/* setColors	*/
};

static Pixmap
GetHighlightPixmapDefault(XmArrowButtonGadget ag)
{
  XmManagerWidget mw = (XmManagerWidget)XtParent(ag);
  Pixmap result = XmUNSPECIFIED_PIXMAP;

  if (ag->arrowbutton.highlight_color == ag->arrowbutton.background)
    result = XmGetPixmapByDepth(XtScreen(ag), XmS50_foreground,
				ag->arrowbutton.highlight_color,
				ag->arrowbutton.foreground,
				mw->core.depth);

  return result;
}

static Pixmap
GetTopShadowPixmapDefault(XmArrowButtonGadget ag)
{
  XmManagerWidget mw = (XmManagerWidget)XtParent(ag);
  Pixmap result = XmUNSPECIFIED_PIXMAP;

  if (ag->arrowbutton.top_shadow_color == ag->arrowbutton.background)
    result = XmGetPixmapByDepth(XtScreen(ag), XmS50_foreground,
				ag->arrowbutton.top_shadow_color,
				ag->arrowbutton.foreground,
				mw->core.depth);

  else if (DefaultDepthOfScreen(XtScreen(ag)) == 1)
    result = XmGetPixmapByDepth(XtScreen(ag), XmS50_foreground,
				ag->arrowbutton.top_shadow_color,
				ag->arrowbutton.background,
				mw->core.depth);

  return result;
}

/************************************************************************
 *
 *  ClassPartInitialize
 *     Set up the fast subclassing for the widget
 *
 ************************************************************************/

static void
ClassPartInitialize(
        WidgetClass wc)
{
  _XmFastSubclassInit (wc, XmARROW_BUTTON_GADGET_BIT);

  /* Install the activatable trait for all subclasses */
  XmeTraitSet((XtPointer)wc, XmQTactivatable, (XtPointer)&arrowBGAT);

  /* Install the careParentVisual trait for all subclasses as well. */
  XmeTraitSet((XtPointer)wc, XmQTcareParentVisual, (XtPointer)&arrowBGCVT);

  /* Install the accessColors trait for all subclasses as well. */
  XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&arrowBGACT);
}

/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args)
{
  XmArrowButtonGadget request = (XmArrowButtonGadget) rw;
  XmArrowButtonGadget new_w = (XmArrowButtonGadget) nw;

  /*
   *  Check the data put into the new widget from .Xdefaults
   *  or through the arg list.
   */
  (void)args;
  (void)num_args;
  if (!XmRepTypeValidValue(XmRID_ARROW_DIRECTION,
			   new_w->arrowbutton.direction, (Widget) new_w))
    {
      new_w->arrowbutton.direction = XmARROW_UP;
    }


  /*  Set up a geometry for the widget if it is currently 0.  */
  if (request->rectangle.width == 0)
    new_w->rectangle.width += 15;
  if (request->rectangle.height == 0)
    new_w->rectangle.height += 15;

  /*  Set the internal arrow variables */
  new_w->arrowbutton.timer = 0;
  new_w->arrowbutton.selected = False;

  /*  Get the drawing graphics contexts.  */
  DealWithColors(new_w);
  DealWithPixmaps(new_w);

  GetArrowGC (new_w);
  GetBackgroundGC (new_w);
  new_w->arrowbutton.highlight_GC =
    _XmGetPixmapBasedGC (XtParent(nw),
			 new_w->arrowbutton.highlight_color,
			 new_w->arrowbutton.background,
			 new_w->arrowbutton.highlight_pixmap);

  new_w->arrowbutton.top_shadow_GC =
    _XmGetPixmapBasedGC (XtParent(nw),
			 new_w->arrowbutton.top_shadow_color,
			 new_w->arrowbutton.background,
			 new_w->arrowbutton.top_shadow_pixmap);

  new_w->arrowbutton.bottom_shadow_GC =
    _XmGetPixmapBasedGC (XtParent(nw),
			 new_w->arrowbutton.bottom_shadow_color,
			 new_w->arrowbutton.background,
			 new_w->arrowbutton.bottom_shadow_pixmap);

  /*  Initialize the interesting input types.  */
  new_w->gadget.event_mask =  XmARM_EVENT | XmACTIVATE_EVENT | XmHELP_EVENT |
    XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT
      | XmMULTI_ARM_EVENT|  XmMULTI_ACTIVATE_EVENT;
}

/************************************************************************
 *
 *  GetBackgroundGC
 *     Get the graphics context used for drawing the background.
 *
 ************************************************************************/

static void
GetBackgroundGC(
        XmArrowButtonGadget ag)
{
  XGCValues values;
  XtGCMask  valueMask;
  XmManagerWidget mw = (XmManagerWidget) XtParent(ag);

  ag->arrowbutton.fill_bg_box =
    ((mw->core.background_pixel != ag->arrowbutton.background) &&
     (mw->core.background_pixmap == XmUNSPECIFIED_PIXMAP));

  if (!ag->arrowbutton.fill_bg_box)
    return;

  valueMask = GCForeground | GCBackground;
  values.foreground = ag->arrowbutton.background;
  values.background = ag->arrowbutton.foreground;

  ag->arrowbutton.background_GC = XtGetGC ((Widget) mw, valueMask, &values);
}

/************************************************************************
 *
 *  GetArrowGC
 *     Get the graphics context used for drawing the arrowbutton.
 *
 ************************************************************************/

static void
GetArrowGC(
        XmArrowButtonGadget ag)
{
  XGCValues values;
  XtGCMask  valueMask, unusedMask;
  XmManagerWidget mw = (XmManagerWidget) XtParent(ag);

  valueMask = GCForeground | GCBackground | GCGraphicsExposures;
  unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;

  values.foreground = ag->arrowbutton.foreground;
  values.background = ag->arrowbutton.background;
  values.graphics_exposures = False;

  ag->arrowbutton.arrow_GC = XtAllocateGC ((Widget) mw, 0, valueMask, &values,
					   GCClipMask, unusedMask);

  valueMask |= GCFillStyle | GCStipple;
  values.fill_style = FillOpaqueStippled;
  values.stipple = _XmGetInsensitiveStippleBitmap((Widget) ag);

  ag->arrowbutton.insensitive_GC = XtAllocateGC((Widget) mw, 0, valueMask,
						&values, GCClipMask,
						unusedMask);
}

/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/
static void
Redisplay(
        Widget w,
        XEvent *event,
        Region region)
{
  XmArrowButtonGadget aw = (XmArrowButtonGadget) w;
  int iwidth, iheight;
  int background_x_offset, background_y_offset, background_width,
  background_height;

  iwidth = (int) aw->rectangle.width - 2 * aw->gadget.highlight_thickness;
  iheight = (int) aw->rectangle.height - 2 * aw->gadget.highlight_thickness;

  background_x_offset = (aw->rectangle.x +
			 aw->gadget.highlight_thickness +
			 aw->gadget.shadow_thickness);

  background_y_offset = (aw->rectangle.y +
			 aw->gadget.highlight_thickness +
			 aw->gadget.shadow_thickness);

  background_height = iheight - 2 * aw->gadget.shadow_thickness;
  background_width = iwidth - 2 * aw->gadget.shadow_thickness;

  if (aw->arrowbutton.fill_bg_box)
    XFillRectangle(XtDisplay(aw),
		   XtWindow((Widget) aw),
		   aw->arrowbutton.background_GC,
		   background_x_offset,
		   background_y_offset,
		   background_width,
		   background_height);

  /*  Draw the arrow  */
  if ((iwidth > 0) && (iheight > 0))
    {
      if (aw->gadget.shadow_thickness > 0)
	XmeDrawShadows (XtDisplay (aw), XtWindow (aw),
			aw->arrowbutton.top_shadow_GC,
			aw->arrowbutton.bottom_shadow_GC,
			aw->rectangle.x + aw->gadget.highlight_thickness,
			aw->rectangle.y + aw->gadget.highlight_thickness,
			aw->rectangle.width - 2 *
			aw->gadget.highlight_thickness,
			aw->rectangle.height-2 *
			aw->gadget.highlight_thickness,
			aw->gadget.shadow_thickness, XmSHADOW_OUT);

      if (aw->arrowbutton.selected && XtIsSensitive(w))
	DrawArrowG(aw, aw->arrowbutton.bottom_shadow_GC,
		   aw->arrowbutton.top_shadow_GC, aw->arrowbutton.arrow_GC);
      else
	DrawArrowG(aw, aw->arrowbutton.top_shadow_GC,
		   aw->arrowbutton.bottom_shadow_GC,
		   (XtIsSensitive(w) ?
		    aw->arrowbutton.arrow_GC : aw->arrowbutton.insensitive_GC));
    }

  if (aw->gadget.highlighted)
    {
      (*(xmArrowButtonGadgetClassRec.gadget_class.border_highlight)) (w);
    }
}

 /*
  *
  * DealWithColors
  *
  * Deal with compatibility.
  *  If any resource is set initialize like a widget otherwise get
  * everything from the parent.
  *
  *
  */
static void
DealWithColors(
        XmArrowButtonGadget ag)
{
  XmManagerWidget mw = (XmManagerWidget) XtParent(ag);

  /*
   * If the gadget color is set to the tag value or it is the
   * same as the manager color; bc mode is enabled otherwise
   * initialize like a widget.
   */
  if ((ag->arrowbutton.background == INVALID_PIXEL ||
       ag->arrowbutton.background == mw->core.background_pixel) &&
      (ag->arrowbutton.foreground == INVALID_PIXEL ||
       ag->arrowbutton.foreground == mw->manager.foreground) &&
      (ag->arrowbutton.top_shadow_color == INVALID_PIXEL ||
       ag->arrowbutton.top_shadow_color == mw->manager.top_shadow_color) &&
      (ag->arrowbutton.bottom_shadow_color == INVALID_PIXEL ||
       ag->arrowbutton.bottom_shadow_color == mw->manager.bottom_shadow_color) &&
      (ag->arrowbutton.highlight_color == INVALID_PIXEL ||
       ag->arrowbutton.highlight_color == mw->manager.highlight_color))
    {
      ag->arrowbutton.background = mw->core.background_pixel;
      ag->arrowbutton.foreground = mw->manager.foreground;
      ag->arrowbutton.top_shadow_color = mw->manager.top_shadow_color;
      ag->arrowbutton.bottom_shadow_color = mw->manager.bottom_shadow_color;
      ag->arrowbutton.highlight_color = mw->manager.highlight_color;
    }
  else
    {
      InitNewColorBehavior(ag);
    }
}

static void
InitNewColorBehavior(
        XmArrowButtonGadget ag)
{
  XrmValue value;

  value.size = sizeof(Pixel);

  if (ag->arrowbutton.background == INVALID_PIXEL)
    {
      _XmBackgroundColorDefault ((Widget)ag,
				 XtOffsetOf(struct _XmArrowButtonGadgetRec,
					    arrowbutton.background),
				 &value);
      memcpy((char*) &ag->arrowbutton.background, value.addr, value.size);
    }

  if (ag->arrowbutton.foreground == INVALID_PIXEL)
    {
      _XmForegroundColorDefault ((Widget)ag,
				 XtOffsetOf(struct _XmArrowButtonGadgetRec,
					    arrowbutton.foreground),
				 &value);
      memcpy((char*) &ag->arrowbutton.foreground, value.addr, value.size);
    }

  if (ag->arrowbutton.top_shadow_color == INVALID_PIXEL)
    {
      _XmTopShadowColorDefault ((Widget)ag,
				XtOffsetOf(struct _XmArrowButtonGadgetRec,
					   arrowbutton.top_shadow_color),
				&value);
      memcpy((char*)&ag->arrowbutton.top_shadow_color, value.addr,value.size);
    }

  if (ag->arrowbutton.bottom_shadow_color == INVALID_PIXEL)
    {
      _XmBottomShadowColorDefault((Widget)ag,
				  XtOffsetOf(struct _XmArrowButtonGadgetRec,
					     arrowbutton.bottom_shadow_color),
				  &value);
      memcpy((char*)&ag->arrowbutton.bottom_shadow_color, value.addr, value.size);
    }

  if (ag->arrowbutton.highlight_color == INVALID_PIXEL)
    {
      _XmHighlightColorDefault((Widget)ag,
			       XtOffsetOf(struct _XmArrowButtonGadgetRec,
					  arrowbutton.highlight_color),
			       &value);
      memcpy((char*) &ag->arrowbutton.highlight_color, value.addr,value.size);
    }
}

 /*
  *
  * DealWithPixmaps
  *
  * Deal with compatibility.
  *  If any resource is set initialize like a widget otherwise get
  * everything from the parent.
  *
  */

static void
DealWithPixmaps(
        XmArrowButtonGadget ag)
{
  XmManagerWidget mw = (XmManagerWidget) XtParent(ag);

  if ((ag->arrowbutton.top_shadow_pixmap == INVALID_PIXMAP ||
       ag->arrowbutton.top_shadow_pixmap == mw->manager.top_shadow_pixmap) &&
      (ag->arrowbutton.highlight_pixmap == INVALID_PIXMAP  ||
       ag->arrowbutton.highlight_pixmap == mw->manager.highlight_pixmap))
    {
      ag->arrowbutton.top_shadow_pixmap = mw->manager.top_shadow_pixmap;
      ag->arrowbutton.highlight_pixmap = mw->manager.highlight_pixmap;
    }
  else
    {
      InitNewPixmapBehavior(ag);
    }
}

/*
 * InitNewPixmapBehavior
 *
 * Initialize colors like a widget.
 */

static void
InitNewPixmapBehavior(
        XmArrowButtonGadget ag)
{
  if (ag->arrowbutton.top_shadow_pixmap == INVALID_PIXMAP)
    {
      ag->arrowbutton.top_shadow_pixmap = GetTopShadowPixmapDefault(ag);
    }

  if (ag->arrowbutton.highlight_pixmap == INVALID_PIXMAP)
    {
      ag->arrowbutton.highlight_pixmap = GetHighlightPixmapDefault(ag);
    }
}

/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void
Destroy(
        Widget w)
{
  XmArrowButtonGadget aw = (XmArrowButtonGadget) w;
  XmManagerWidget mw = (XmManagerWidget) XtParent(aw);

  if (aw->arrowbutton.timer)
    XtRemoveTimeOut (aw->arrowbutton.timer);

  XtReleaseGC ((Widget) mw, aw->arrowbutton.arrow_GC);
  XtReleaseGC ((Widget) mw, aw->arrowbutton.insensitive_GC);
  if (aw->arrowbutton.fill_bg_box)
    XtReleaseGC ((Widget) mw, aw->arrowbutton.background_GC);
  XtReleaseGC ((Widget) mw, aw->arrowbutton.top_shadow_GC);
  XtReleaseGC ((Widget) mw, aw->arrowbutton.bottom_shadow_GC);
  XtReleaseGC ((Widget) mw, aw->arrowbutton.highlight_GC);
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args)
{
  XmArrowButtonGadget current = (XmArrowButtonGadget) cw;
  XmArrowButtonGadget new_w = (XmArrowButtonGadget) nw;
  Boolean returnFlag = False;

  /*  Check the data put into the new widget.  */
  (void)rw;
  (void)args;
  (void)num_args;
  if (!XmRepTypeValidValue(XmRID_ARROW_DIRECTION,
			   new_w->arrowbutton.direction, (Widget) new_w))
    {
      new_w->arrowbutton.direction = current->arrowbutton.direction;
    }


  /*  ReInitialize the interesting input types.  */

  new_w->gadget.event_mask |=  XmARM_EVENT | XmACTIVATE_EVENT | XmHELP_EVENT |
    XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT
      | XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT;

  if (new_w->arrowbutton.direction != current->arrowbutton.direction ||
      XtIsSensitive(nw) != XtIsSensitive(cw) ||
      new_w->gadget.highlight_thickness !=
      current->gadget.highlight_thickness ||
      new_w->gadget.shadow_thickness != current->gadget.shadow_thickness)
    {
      returnFlag = TRUE;
    }

  if (new_w->arrowbutton.foreground != current->arrowbutton.foreground ||
      new_w->arrowbutton.background != current->arrowbutton.background)
    {
      XtReleaseGC (XtParent(current), new_w->arrowbutton.arrow_GC);
      XtReleaseGC (XtParent(current), new_w->arrowbutton.insensitive_GC);
      if (new_w->arrowbutton.fill_bg_box)
	XtReleaseGC (XtParent(current), new_w->arrowbutton.background_GC);
      GetArrowGC (new_w);
      GetBackgroundGC (new_w);
      return (True);
    }

  if (current->arrowbutton.top_shadow_color !=
      new_w->arrowbutton.top_shadow_color ||
      current->arrowbutton.top_shadow_pixmap !=
      new_w->arrowbutton.top_shadow_pixmap)
    {
      XtReleaseGC ((Widget) new_w, new_w->arrowbutton.top_shadow_GC);
      new_w->arrowbutton.top_shadow_GC =
	_XmGetPixmapBasedGC (XtParent(nw),
			     new_w->arrowbutton.top_shadow_color,
			     new_w->arrowbutton.background,
			     new_w->arrowbutton.top_shadow_pixmap);
      returnFlag = True;
    }


  if (current->arrowbutton.bottom_shadow_color !=
      new_w->arrowbutton.bottom_shadow_color ||
      current->arrowbutton.bottom_shadow_pixmap !=
      new_w->arrowbutton.bottom_shadow_pixmap)
    {
      XtReleaseGC ((Widget) new_w, new_w->arrowbutton.bottom_shadow_GC);
      new_w->arrowbutton.bottom_shadow_GC =
	_XmGetPixmapBasedGC (XtParent(nw),
			     new_w->arrowbutton.bottom_shadow_color,
			     new_w->arrowbutton.background,
			     new_w->arrowbutton.bottom_shadow_pixmap);
      returnFlag = True;
    }

  if (current->arrowbutton.highlight_color !=
      new_w->arrowbutton.highlight_color ||
      current->arrowbutton.highlight_pixmap !=
      new_w->arrowbutton.highlight_pixmap)
    {
      XtReleaseGC ((Widget) new_w, new_w->arrowbutton.highlight_GC);
      new_w->arrowbutton.highlight_GC =
	_XmGetPixmapBasedGC (XtParent(nw),
			     new_w->arrowbutton.highlight_color,
			     new_w->arrowbutton.background,
			     new_w->arrowbutton.highlight_pixmap);

      returnFlag = True;
    }


  return (returnFlag);
}

static void
HighlightBorder(
        Widget w)
{
  XmArrowButtonGadget ag = (XmArrowButtonGadget) w;

  if (ag->rectangle.width == 0 ||
      ag->rectangle.height == 0 ||
      ag->gadget.highlight_thickness == 0)
    {
      return;
    }

  ag->gadget.highlighted = True;
  ag->gadget.highlight_drawn = True;

  /* CR 7330: Use XmeDrawHighlight, not _XmDrawHighlight. */
  XmeDrawHighlight(XtDisplay((Widget) ag), XtWindow((Widget) ag),
		   ag->arrowbutton.highlight_GC,
		   ag->rectangle.x, ag->rectangle.y,
		   ag->rectangle.width, ag->rectangle.height,
		   ag->gadget.highlight_thickness);
}

static Boolean
HandleRedraw (
	Widget kid,
	Widget cur_parent,
	Widget new_parent,
	Mask visual_flag)
{
  XmArrowButtonGadget ag = (XmArrowButtonGadget) kid;
  XmManagerWidget mw = (XmManagerWidget) new_parent;
  XmManagerWidget curmw = (XmManagerWidget) cur_parent;
  Boolean redraw, do_bg, do_arrow, do_highlight, do_top, do_bot;

  redraw = do_bg = do_arrow = do_highlight = do_top = do_bot = False;

  if ((visual_flag & VisualBackgroundPixel) &&
      (ag->arrowbutton.background == curmw->core.background_pixel))

    {
      redraw = do_bg = do_arrow = do_highlight = do_top = do_bot = True;
      ag->arrowbutton.background = mw->core.background_pixel;
    }

  if ((visual_flag & VisualForeground) &&
      (ag->arrowbutton.foreground == curmw->manager.foreground))
    {
      redraw = do_arrow = True;
      ag->arrowbutton.foreground = mw->manager.foreground;
    }

  if (visual_flag & VisualBackgroundPixmap)
    {
      redraw = do_bg = True;
    }

  if (visual_flag & (VisualTopShadowColor | VisualTopShadowPixmap))
    {
      redraw = do_top = True;

      if (ag->arrowbutton.top_shadow_color == curmw->manager.top_shadow_color)
	ag->arrowbutton.top_shadow_color = mw->manager.top_shadow_color;

      if ((ag->arrowbutton.top_shadow_pixmap ==
	   curmw->manager.top_shadow_pixmap) &&
	  (ag->arrowbutton.top_shadow_pixmap != XmUNSPECIFIED_PIXMAP ||
	   ag->arrowbutton.top_shadow_color == curmw->manager.top_shadow_color))
	ag->arrowbutton.top_shadow_pixmap = mw->manager.top_shadow_pixmap;
    }

  if (visual_flag & (VisualBottomShadowColor | VisualBottomShadowPixmap))
    {
      redraw = do_bot = True;

      if (ag->arrowbutton.bottom_shadow_color ==
	  curmw->manager.bottom_shadow_color)
	ag->arrowbutton.bottom_shadow_color = mw->manager.bottom_shadow_color;

      if ((ag->arrowbutton.bottom_shadow_pixmap ==
	   curmw->manager.bottom_shadow_pixmap) &&
	  (ag->arrowbutton.bottom_shadow_pixmap != XmUNSPECIFIED_PIXMAP ||
	   ag->arrowbutton.bottom_shadow_color == curmw->manager.bottom_shadow_color))
	ag->arrowbutton.bottom_shadow_pixmap = mw->manager.bottom_shadow_pixmap;
    }

  if (visual_flag & (VisualHighlightColor | VisualHighlightPixmap))
    {
      redraw = do_highlight = True;

      if (ag->arrowbutton.highlight_color == curmw->manager.highlight_color)
	ag->arrowbutton.highlight_color = mw->manager.highlight_color;

      if (ag->arrowbutton.highlight_pixmap == curmw->manager.highlight_pixmap &&
	  (ag->arrowbutton.highlight_pixmap != XmUNSPECIFIED_PIXMAP ||
	   ag->arrowbutton.highlight_color != curmw->manager.highlight_color))
	ag->arrowbutton.highlight_pixmap = mw->manager.highlight_pixmap;

      ag->arrowbutton.highlight_GC =
	_XmGetPixmapBasedGC (XtParent(ag),
			     ag->arrowbutton.highlight_color,
			     ag->arrowbutton.background,
			     ag->arrowbutton.highlight_pixmap);
    }

  if (do_bg)
    {
      if (ag->arrowbutton.fill_bg_box)
	XtReleaseGC ((Widget)mw, ag->arrowbutton.background_GC);
      GetBackgroundGC (ag);
    }

  if (do_arrow)
    {
      XtReleaseGC ((Widget)mw, ag->arrowbutton.arrow_GC);
      XtReleaseGC ((Widget)mw, ag->arrowbutton.insensitive_GC);
      GetArrowGC (ag);
    }

  if (do_highlight)
    {
      XtReleaseGC ((Widget)mw, ag->arrowbutton.highlight_GC);
      ag->arrowbutton.highlight_GC =
	_XmGetPixmapBasedGC (XtParent(ag),
			     ag->arrowbutton.highlight_color,
			     ag->arrowbutton.background,
			     ag->arrowbutton.highlight_pixmap);
    }

  if (do_top)
    {
      XtReleaseGC ((Widget)mw, ag->arrowbutton.top_shadow_GC);
      ag->arrowbutton.top_shadow_GC =
	_XmGetPixmapBasedGC (XtParent(ag),
			     ag->arrowbutton.top_shadow_color,
			     ag->arrowbutton.background,
			     ag->arrowbutton.top_shadow_pixmap);
    }

  if (do_bot)
    {
      XtReleaseGC ((Widget)mw, ag->arrowbutton.bottom_shadow_GC);
      ag->arrowbutton.bottom_shadow_GC =
	_XmGetPixmapBasedGC (XtParent(ag),
			     ag->arrowbutton.bottom_shadow_color,
			     ag->arrowbutton.background,
			     ag->arrowbutton.bottom_shadow_pixmap);
    }

  return redraw;
}

/************************************************************************
 *
 *  InputDispatch
 *     This function catches input sent by a manager and dispatches it
 *     to the individual routines.
 *
 ************************************************************************/

static void
InputDispatch(
        Widget wid,
        XEvent *event,
        Mask event_mask)
{
  XmArrowButtonGadget ag = (XmArrowButtonGadget) wid;

  if ((event_mask & XmARM_EVENT) ||
      ((ag->arrowbutton.multiClick == XmMULTICLICK_KEEP) &&
       (event_mask & XmMULTI_ARM_EVENT)))
    Arm (ag, event);

  else if (event_mask & XmACTIVATE_EVENT)
    {
      ag->arrowbutton.click_count = 1;
      ActivateCommonG (ag, event, event_mask);
    }

  else if (event_mask & XmMULTI_ACTIVATE_EVENT)
    {
      /* if XmNMultiClick resource is set to DISCARD - do nothing
       * else call ActivateCommon() and increment clickCount;
       */
      if (ag->arrowbutton.multiClick == XmMULTICLICK_KEEP)
	{
	  ag->arrowbutton.click_count++;
	  ActivateCommonG (ag, event, event_mask);
	}
    }

  else if (event_mask & XmHELP_EVENT)
    Help (ag, event);

  else if (event_mask & XmENTER_EVENT)
    Enter (ag, event);

  else if (event_mask & XmLEAVE_EVENT)
    Leave (ag, event);

  else if (event_mask & XmFOCUS_IN_EVENT)
    _XmFocusInGadget ((Widget) ag, event, NULL, NULL);

  else if (event_mask & XmFOCUS_OUT_EVENT)
    _XmFocusOutGadget ((Widget)ag, event, NULL, NULL);
}

/************************************************************************
 *
 *  Arm
 *     This function processes button 1 down occuring on the arrowbutton.
 *
 ************************************************************************/
static void
Arm(
        XmArrowButtonGadget aw,
        XEvent *event)
{
  XmArrowButtonCallbackStruct call_value;

  aw->arrowbutton.selected = True;

  DrawArrowG(aw, aw->arrowbutton.bottom_shadow_GC,
	     aw->arrowbutton.top_shadow_GC, NULL);

  if (aw->arrowbutton.arm_callback)
    {
      XFlush(XtDisplay(aw));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) aw, aw->arrowbutton.arm_callback,
			  &call_value);
    }
}

/************************************************************************
 *
 *  Activate
 *     This function processes button 1 up occuring on the arrowbutton.
 *     If the button 1 up occurred inside the button the activate
 *     callbacks are called.
 *
 ************************************************************************/
static void
Activate(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonGadget aw = (XmArrowButtonGadget)wid;
  XButtonPressedEvent *buttonEvent = (XButtonPressedEvent *)event;
  XmPushButtonCallbackStruct call_value;

  (void)params;
  (void)num_params;
  aw->arrowbutton.selected = False;

  DrawArrowG(aw, aw->arrowbutton.top_shadow_GC,
	     aw->arrowbutton.bottom_shadow_GC, NULL);

  /* CR 9181: Consider clipping when testing visibility. */
  if ((buttonEvent->type == ButtonPress ||
       buttonEvent->type == ButtonRelease) &&
      _XmGetPointVisibility(wid, buttonEvent->x_root, buttonEvent->y_root) &&
      (aw->arrowbutton.activate_callback))
    {
      XFlush(XtDisplay(aw));

      call_value.reason = XmCR_ACTIVATE;
      call_value.event = (XEvent *) buttonEvent;
      call_value.click_count = aw->arrowbutton.click_count;
      XtCallCallbackList ((Widget) aw,
			  aw->arrowbutton.activate_callback, &call_value);
    }
}

/************************************************************************
 *
 *     ArmAndActivate
 *
 ************************************************************************/
static void
ArmAndActivate(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonGadget ab = (XmArrowButtonGadget) w;
  XmPushButtonCallbackStruct call_value;
  XtExposeProc expose;

  (void)params;
  (void)num_params;
  ab->arrowbutton.selected = True;
  ab->arrowbutton.click_count = 1;
  _XmProcessLock();
  expose = ab->object.widget_class->core_class.expose;
  _XmProcessUnlock();
  (*(expose))
    ((Widget) ab, event, FALSE);

  XFlush (XtDisplay (ab));

  if (ab->arrowbutton.arm_callback)
    {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      call_value.click_count = ab->arrowbutton.click_count;
      XtCallCallbackList ((Widget) ab, ab->arrowbutton.arm_callback,
			  &call_value);
    }

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = 1;          /* always 1 in kselect */

  if (ab->arrowbutton.activate_callback)
    {
      XFlush (XtDisplay (ab));
      XtCallCallbackList ((Widget) ab, ab->arrowbutton.activate_callback,
			  &call_value);
    }

  ab->arrowbutton.selected = FALSE;

  if (ab->arrowbutton.disarm_callback)
    {
      XFlush (XtDisplay (ab));
      call_value.reason = XmCR_DISARM;
      XtCallCallbackList ((Widget) ab, ab->arrowbutton.disarm_callback,
			  &call_value);
    }

  /* If the button is still around, show it released, after a short delay */

  if (ab->object.being_destroyed == False)
    {
      ab->arrowbutton.timer =
	XtAppAddTimeOut(XtWidgetToApplicationContext((Widget) ab),
			(unsigned long) DELAY_DEFAULT,
			ArmTimeout, (XtPointer)ab);
    }
}

static void
ArmTimeout(
        XtPointer data,
        XtIntervalId *id)
{
  XmArrowButtonGadget ab = (XmArrowButtonGadget) data;

  ab->arrowbutton.timer = 0;

  if (XtIsRealized((Widget)ab) && XtIsManaged((Widget)ab))
    {
      Redisplay ((Widget) ab, NULL, FALSE);
      XFlush (XtDisplay (ab));
    }
}

/************************************************************************
 *
 *  Disarm
 *     This function processes button 1 up occuring on the arrowbutton.
 *
 ************************************************************************/
static void
Disarm(
        XmArrowButtonGadget aw,
        XEvent *event)
{
  XmArrowButtonCallbackStruct call_value;

  call_value.reason = XmCR_DISARM;
  call_value.event = event;
  XtCallCallbackList ((Widget) aw, aw->arrowbutton.disarm_callback,
		      &call_value);
}

/************************************************************************
 *
 *  Enter
 *
 ************************************************************************/

static void
Enter(
        XmArrowButtonGadget aw,
        XEvent *event)
{
  _XmEnterGadget ((Widget) aw, event, NULL, NULL);

  if (aw->arrowbutton.selected && XtIsSensitive((Widget) aw))
    DrawArrowG(aw, aw->arrowbutton.bottom_shadow_GC,
	       aw->arrowbutton.top_shadow_GC, NULL);
}

/************************************************************************
 *
 *  Leave
 *
 ************************************************************************/

static void
Leave(
        XmArrowButtonGadget aw,
        XEvent *event)
{
  _XmLeaveGadget ((Widget) aw, event, NULL, NULL);

  if (aw->arrowbutton.selected && XtIsSensitive((Widget) aw))
    DrawArrowG(aw, aw->arrowbutton.top_shadow_GC,
	       aw->arrowbutton.bottom_shadow_GC, NULL);
}

/************************************************************************
 *
 *  Help
 *     This function processes Function Key 1 press occuring on
 *     the arrowbutton.
 *
 ************************************************************************/

static void
Help(
        XmArrowButtonGadget aw,
        XEvent *event)
{
   _XmSocorro((Widget) aw, event, NULL, NULL);
}

/************************************************************************
 *
 *  ChangeCB
 *	add or remove the activate callback list.
 *
 ************************************************************************/

static void
ChangeCB(
	 Widget w,
	 XtCallbackProc activCB,
	 XtPointer closure,
	 Boolean setunset)
{
  if (setunset)
    XtAddCallback (w, XmNactivateCallback, activCB, closure);
  else
    XtRemoveCallback (w, XmNactivateCallback, activCB, closure);
}

static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
  color_data->valueMask = AccessForeground | AccessBackgroundPixel |
    AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
  color_data->background = ArrowBG_Background(w);
  color_data->foreground = ArrowBG_Foreground(w);
  color_data->highlight_color = ArrowBG_HighlightColor(w);
  color_data->top_shadow_color = ArrowBG_TopShadowColor(w);
  color_data->bottom_shadow_color = ArrowBG_BottomShadowColor(w);
}

/************************************************************************
 *
 *  XmCreateArrowButtonGadget
 *	Create an instance of an arrowbutton and return the widget id.
 *
 ************************************************************************/

Widget
XmCreateArrowButtonGadget(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount)
{
  return XtCreateWidget(name, xmArrowButtonGadgetClass, parent,
			arglist, argcount);
}

Widget
XmVaCreateArrowButtonGadget(
        Widget parent,
        char *name,
        ...)
{
    Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmArrowButtonGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedArrowButtonGadget(
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
                         xmArrowButtonGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}

static void
ActivateCommonG(
        XmArrowButtonGadget ag,
        XEvent *event,
        Mask event_mask)
{
  if (event->type == ButtonRelease)
    {
      Activate((Widget) ag, event, NULL, NULL);
      Disarm (ag, event);
    }

  else  /* assume KeyPress or KeyRelease */
    (* (((XmArrowButtonGadgetClassRec *)(ag->object.widget_class))->
	gadget_class.arm_and_activate))
      ((Widget) ag, event, NULL, NULL);
}

/* Wrapper around XmeDrawArrow to calculate sizes. */
static void
DrawArrowG(XmArrowButtonGadget ag,
	   GC		       top_gc,
	   GC		       bottom_gc,
	   GC		       center_gc)
{
  Position x, y;
  Dimension width, height;
  Dimension margin =
    ag->gadget.highlight_thickness + ag->gadget.shadow_thickness;

  /* Don't let large margins cause confusion. */
  if (margin <= (ag->rectangle.width / 2))
    {
      x = ag->rectangle.x + margin;
      width = ag->rectangle.width - (margin * 2);
    }
  else
    {
      x = ag->rectangle.x + ag->rectangle.width / 2;
      width = 0;
    }

  if (margin <= (ag->rectangle.height / 2))
    {
      y = ag->rectangle.y + margin;
      height = ag->rectangle.height - (margin * 2);
    }
  else
    {
      y = ag->rectangle.y + ag->rectangle.height / 2;
      height = 0;
    }

  /* The way we currently handle 1 shadowThickness in XmeDrawArrow
     is by drawing the center a little bit bigger, so the center_gc has
     to be present. Kinda hacky... */
  if (!center_gc &&
      ag->arrowbutton.detail_shadow_thickness == 1)
      center_gc = ag->arrowbutton.arrow_GC ;
  if (center_gc)
    XSetClipMask(XtDisplay((Widget) ag), center_gc, None);

  XmeDrawArrow (XtDisplay ((Widget) ag), XtWindow ((Widget) ag),
		top_gc, bottom_gc, center_gc,
		x, y, width, height,
		ag->arrowbutton.detail_shadow_thickness,
		ag->arrowbutton.direction);
}

