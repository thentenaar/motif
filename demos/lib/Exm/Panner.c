/* $XConsortium: Panner.c /main/5 1995/07/15 20:41:11 drk $ */
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

/************************************ WARNING **********************************
 *
 * ExmPanner is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way
 *
 ************************************ WARNING *********************************/


/*************************************************************************
 *
 * Panner.c - ExmPanner widget.  This widget lets a user pan a
 *            ScrolledWindow (or any other scrollFrame widget)
 *            in two-dimensions.  The ExmPanner widget is similar
 *            to the Athena Panner widget.  ExmPanner demonstrates:
 *               * how to install the XmQTnavigator trait.
 *
 *********************************************************************/

#include <Xm/XmP.h>
#include <Xm/TraitP.h>
#include <Xm/NavigatorT.h>
#include <Xm/DrawP.h>
#include <Xm/RepType.h>
#include <Exm/PannerP.h>
#include <stdlib.h>
#include <ctype.h>


/* Define macros and constants. */
static XmConst int DEFAULT_WIDTH=100;
static XmConst int DEFAULT_HEIGHT=100;

static XmConst int DEFAULT_CANVAS_WIDTH=100;
static XmConst int DEFAULT_CANVAS_HEIGHT=100;

static XmConst int PANNER_OUTOFRANGE = (-30000);

#define PANNER_HSCALE(pw,val) ((pw)->panner.haspect * ((double) (val)))
#define PANNER_VSCALE(pw,val) ((pw)->panner.vaspect * ((double) (val)))

static void ClassPartInitialize(
    WidgetClass wc );
static void Initialize(
    Widget greq,
    Widget gnew,
    ArgList args,
    Cardinal *num_args);
static void Resize (
    Widget gw);
static Boolean SetValues(
    Widget gcur,
    Widget greq,
    Widget gnew,
    ArgList args,
    Cardinal *num_args);
static void DrawVisual(
    Widget gw);
static void CreateGC(
    Widget w);
static void DestroyGC(
    Widget w);
static void DrawRubberBand(
    ExmPannerWidget pw);
static void CheckKnob(
    ExmPannerWidget pw,
    Boolean knob);
static void ScaleKnob (
    ExmPannerWidget pw,
    Boolean location,
    Boolean size);
static void ExmPannerStart (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void ExmPannerStop (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void ExmPannerAbort (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void ExmPannerMove (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void ExmPannerNotify (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void ExmPannerSet (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static Boolean GetEventXY (
    ExmPannerWidget pw,
    XEvent *event,
    int *x,
    int *y);
static int ParsePageString (
    char *s,
    int pagesize,
    int canvassize,
    Boolean *relative);
static void ExmPannerPage (
    Widget gw,
    XEvent *event,
    String *params,
    Cardinal *num_params);
static void NavigChangeMoveCB(
    Widget nav,
    XtCallbackProc moveCB,
    XtPointer closure,
    Boolean setunset);
static void NavigSetValue(
    Widget nav,
    XmNavigatorData nav_data,
    Boolean notify);
static void NavigGetValue(
    Widget nav,
    XmNavigatorData nav_data);


/* Define the translations. */
static char defaultTranslations[] =
"<Unmap>:			PrimitiveUnmap()\n\
<Enter>:			PrimitiveEnter()\n\
<Leave>:			PrimitiveLeave()\n\
<FocusIn>:			PrimitiveFocusIn()\n\
<FocusOut>:			PrimitiveFocusOut()\n\
:<Key>osfActivate:		PrimitiveParentActivate()\n\
:<Key>osfHelp:	        	PrimitiveHelp()\n\
:<Key>osfCancel:	        ExmPannerAbort()\n\
~s ~c ~m ~a <Btn1Down>:		ExmPannerStart(1) \n\
~s ~c ~m ~a <Btn1Motion>:	ExmPannerMove() \n\
~s ~c ~m ~a <Btn1Up>:		ExmPannerNotify() ExmPannerStop() \n\
~s ~c ~m ~a <Btn2Down>:         ExmPannerStart(2) \n\
~s ~c ~m ~a <Btn2Motion>:    	ExmPannerMove() \n\
~s ~c ~m ~a <Btn2Up>:           ExmPannerNotify() ExmPannerStop() \n\
:<Key>space:         		ExmPannerSet(rubberband,toggle) \n\
:<Key>osfPageUp:     		ExmPannerPage(+0, -1p) \n\
:c <Key>osfPageUp:     		ExmPannerPage(+0, -1c) \n\
:<Key>osfPageDown:   		ExmPannerPage(+0, +1p) \n\
:c <Key>osfPageDown:   		ExmPannerPage(+0, +1c) \n\
:<Key>osfPageLeft:   		ExmPannerPage(-1p, +0) \n\
:c <Key>osfPageLeft:   		ExmPannerPage(-1c, +0) \n\
:<Key>osfPageRight:  		ExmPannerPage(+1p, +0) \n\
:c <Key>osfPageRight:  		ExmPannerPage(+1c, +0) \n\
:<Key>osfLeft:     		ExmPannerPage(-1, +0) \n\
:c <Key>osfLeft:     		ExmPannerPage(-1p, +0) \n\
:<Key>osfRight:    		ExmPannerPage(+1, +0) \n\
:c <Key>osfRight:    		ExmPannerPage(+1p, +0) \n\
:<Key>osfUp:       		ExmPannerPage(+0, -1) \n\
:c <Key>osfUp:       		ExmPannerPage(+0, -1p) \n\
:<Key>osfDown:     		ExmPannerPage(+0, +1) \n\
:c <Key>osfDown:     		ExmPannerPage(+0, +1p) \n\
:<Key>osfEndLine:     		ExmPannerPage(+1c, +1c) \n\
:<Key>osfBeginLine:     	ExmPannerPage(0,0)\n\
~s ~m ~a <Key>Return:   	PrimitiveParentActivate()\n\
s ~m ~a <Key>Tab:	   	PrimitivePrevTabGroup()\n\
~m ~a <Key>Tab:	   		PrimitiveNextTabGroup()";


/* Define the actions array. */
static XtActionsRec actions[] = {
  {"ExmPannerStart", ExmPannerStart},  /* start tmp graphics */
  {"ExmPannerStop",  ExmPannerStop},   /* stop tmp graphics */
  {"ExmPannerAbort", ExmPannerAbort},  /* abort panning */
  {"ExmPannerMove",  ExmPannerMove},   /* move tmp graphics on Motion event */
  {"ExmPannerPage",  ExmPannerPage},   /* page around usually from keyboard */
  {"ExmPannerNotify",ExmPannerNotify}, /* callback new position */
  {"ExmPannerSet",   ExmPannerSet}     /* set various parameters */
};


/* Define the resources. */
#define poff(field) XtOffsetOf(ExmPannerRec, panner.field)

static XtResource resources[] =
{
    {
	ExmNreportCallback,
	ExmCReportCallback,
	XtRCallback,
	sizeof(XtPointer),
	poff(report_callbacks),
	XtRCallback,
	(XtPointer) NULL
    },
    {
	ExmNrubberBand,
	ExmCRubberBand,
	XtRBoolean,
	sizeof(Boolean),
	poff(rubber_band),
	XtRImmediate,
	(XtPointer) FALSE
    },
    {
	ExmNcanvasWidth,
	ExmCCanvasWidth,
	XmRHorizontalDimension,
	sizeof(Dimension),
	poff(canvas_width),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	ExmNcanvasHeight,
	ExmCCanvasHeight,
        XmRVerticalDimension,
	sizeof(Dimension),
	poff(canvas_height),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	ExmNsliderX,
	ExmCSliderX,
        XmRHorizontalPosition,
	sizeof(Position),
	poff(slider_x),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	ExmNsliderY,
	ExmCSliderY,
        XmRVerticalPosition,
	sizeof(Position),
	poff(slider_y),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	ExmNsliderWidth,
	ExmCSliderWidth,
	XmRHorizontalDimension,
	sizeof(Dimension),
	poff(slider_width),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	ExmNsliderHeight,
	ExmCSliderHeight,
	XmRVerticalDimension,
	sizeof(Dimension),
	poff(slider_height),
	XtRImmediate,
	(XtPointer) 0
    },
    {
	XmNnavigationType,
	XmCNavigationType,
	XmRNavigationType,
	sizeof(unsigned char),
	XtOffsetOf(ExmPannerRec, primitive.navigation_type),
	XmRImmediate,
        (XtPointer) XmSTICKY_TAB_GROUP
    },
};


/* 6 of the 8 resources have corresponding synthetic resources. */
static XmSyntheticResource syn_resources[] =
{
   {	ExmNcanvasWidth,
	sizeof (Dimension),
	poff(canvas_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
   },
   { 	ExmNcanvasHeight,
	sizeof (Dimension),
	poff(canvas_height),
     	XmeFromVerticalPixels,
	XmeToVerticalPixels
   },
   {	ExmNsliderX,
	sizeof (Position),
	poff(slider_x),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
   },
   {	ExmNsliderY,
	sizeof (Position),
	poff(slider_y),
	XmeFromVerticalPixels,
	XmeToVerticalPixels
   },
   {	ExmNsliderWidth,
	sizeof (Dimension),
	poff(slider_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels
   },
   {	ExmNsliderHeight,
	sizeof (Dimension),
	poff(slider_height),
	XmeFromVerticalPixels,
        XmeToVerticalPixels
   },
};
#undef poff


/* Define the widget class record. */
externaldef(exmpannerclassrec) ExmPannerClassRec exmPannerClassRec = {
  { /* Following is the Core class record. */
    /* superclass		*/	(WidgetClass) &exmSimpleClassRec,
    /* class_name		*/	"ExmPanner",
    /* widget_size		*/	sizeof(ExmPannerRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	XtInheritExpose,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* Following is the XmPrimitive class record. */
    /* border_highlight */              XmInheritWidgetProc,
    /* border_unhighlight */            XmInheritWidgetProc,
    /* translations */                  NULL,
    /* arm_and_activate */              (XtActionProc)NULL,
    /* syn_resources */                 syn_resources,
    /* num_syn_resources */             XtNumber(syn_resources),
    /* extension */                     NULL,
   },
  { /* Following is the ExmSimple class record. */
    /* draw_visual */                   DrawVisual,
    /* draw_shadow */                   NULL,
    /* create_gc */                     CreateGC,
    /* destroy_gc */                    DestroyGC,
    /* select_gc */                     ExmInheritSelectGC,
    /* calc_visual_size */              ExmInheritCalcVisualSize,
    /* calc_widget_size */              ExmInheritCalcWidgetSize,
    /* extension */                     NULL,
  },
  { /* Following is the ExmPanner class record. */
    /* extension */                     NULL
  }
};

externaldef(exmpannerwidgetclass)
    WidgetClass exmPannerWidgetClass = (WidgetClass) &exmPannerClassRec;



/* Navigator Trait record for Panner */

static XmConst XmNavigatorTraitRec pannerNT = {
  0,         /* version */
  NavigChangeMoveCB,
  NavigSetValue,
  NavigGetValue,
};





/*********************************************************************
 *
 *  ClassPartInitialize
 *     Initialize navigator trait
 *
 *********************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
    /* Install the navigator trait. */
    XmeTraitSet((XtPointer)wc, XmQTnavigator, (XtPointer) &pannerNT);
}





/*********************************************************************
 *
 *  Initialize
 *
 *********************************************************************/
static void
Initialize(
    Widget greq,
    Widget gnew,
    ArgList args,
    Cardinal *num_args)
{
 ExmPannerWidget req = (ExmPannerWidget) greq,
                 new = (ExmPannerWidget) gnew;

    if (req->panner.canvas_width < 1)
	new->panner.canvas_width = DEFAULT_CANVAS_WIDTH;
    if (req->panner.canvas_height < 1)
	new->panner.canvas_height = DEFAULT_CANVAS_HEIGHT;

    if (greq->core.width == 0) gnew->core.width = DEFAULT_WIDTH;
    if (greq->core.height == 0) gnew->core.height = DEFAULT_HEIGHT;

    new->panner.tmp.doing = FALSE;
    new->panner.tmp.showing = FALSE;

    /* call the resize method to get an initial size */
    (* (gnew->core.widget_class->core_class.resize)) (gnew);

    /* Simple Initialize already created the GCs */
}



/*********************************************************************
 *
 *  Resize
 *
 *********************************************************************/
static void
Resize (
    Widget gw)
{
 ExmPannerWidget pw = (ExmPannerWidget)gw;

    int hpad = pw->simple.margin_width * 2;
    int vpad = pw->simple.margin_height * 2;

    if (pw->panner.canvas_width < 1)
      pw->panner.canvas_width = pw->core.width;
    if (pw->panner.canvas_height < 1)
      pw->panner.canvas_height = pw->core.height;

    if ((int)pw->core.width <= hpad) hpad = 0;
    if ((int)pw->core.height <= vpad) vpad = 0;

    pw->panner.haspect = ((double) pw->core.width - hpad) /
			  (double) pw->panner.canvas_width;
    pw->panner.vaspect = ((double) pw->core.height - vpad) /
			  (double) pw->panner.canvas_height;

    ScaleKnob (pw, TRUE, TRUE);
}

/*********************************************************************
 *
 *  SetValues
 *
 *********************************************************************/
static Boolean
SetValues(
    Widget gcur,
    Widget greq,
    Widget gnew,
    ArgList args,
    Cardinal *num_args)
{
 ExmPannerWidget cur = (ExmPannerWidget) gcur;
 ExmPannerWidget new = (ExmPannerWidget) gnew;
 Boolean redisplay = FALSE;

    if (cur->panner.rubber_band != new->panner.rubber_band) {
	if (new->panner.tmp.doing) redisplay = TRUE;
    }

    if (cur->panner.canvas_width != new->panner.canvas_width ||
	cur->panner.canvas_height != new->panner.canvas_height ||
	cur->simple.margin_width != new->simple.margin_width ||
	cur->simple.margin_height != new->simple.margin_height) {
	Resize (gnew);			/* does a ScaleKnob as well */
	redisplay = TRUE;
    } else {
	Boolean loc = (cur->panner.slider_x != new->panner.slider_x ||
		       cur->panner.slider_y != new->panner.slider_y);
	Boolean siz = (cur->panner.slider_width !=
		       new->panner.slider_width ||
		       cur->panner.slider_height !=
		       new->panner.slider_height);
	if (loc || siz) {
	    ScaleKnob (new, loc, siz);
	    redisplay = TRUE;
	}
    }

    return redisplay;
}



/*********************************************************************
 *
 *  DrawVisual
 *      an ExmSimple class method
 *
 *********************************************************************/
static void
DrawVisual(
    Widget gw)
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(gw);
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 int kx = pw->panner.knob_x + pw->simple.margin_width;
 int ky = pw->panner.knob_y + pw->simple.margin_height;
 int lx = pw->panner.last_x + pw->simple.margin_width;
 int ly = pw->panner.last_y + pw->simple.margin_height;

    /* Clear rubberband */
    if (pw->panner.tmp.doing && pw->panner.rubber_band && pw->panner.tmp.showing)
      DrawRubberBand (pw);

    /* Only clear areas that need clearing. This minimizes flickering while
       dragging. */
    if (pw->panner.last_y != PANNER_OUTOFRANGE &&
	pw->panner.last_x != PANNER_OUTOFRANGE) {
      int height = 0;
      /* Clear horizontal areas */
      if (ly < ky) {
	/* Clear area above new panner position */
	height = ky - ly;
	XClearArea (XtDisplay(gw), XtWindow(gw),
		    lx, ly,
		    (unsigned int) pw->panner.knob_width,
		    (unsigned int) height,
		    False);
      } else if (ky < ly) {
	/* Clear area under new panner */
	height = ly - ky;
	XClearArea (XtDisplay(gw), XtWindow(gw),
		    lx, ky + pw->panner.knob_height - 1,
		    (unsigned int) pw->panner.knob_width,
		    (unsigned int) height,
		    False);
      }
      /* Clear vertical areas. Try not to clear the same area twice. */
      height = pw->panner.knob_height - height;
      if (height > 0) {
	if (lx < kx) {
	/* Clear area to the left of new panner position */
	  XClearArea (XtDisplay(gw), XtWindow(gw),
		      lx, ly > ky ? ly : ky,
		      (unsigned int) kx - lx,
		      (unsigned int) height,
		      False);
	} else if (kx < lx) {
	  /* Clear area to the right of new panner */
	  XClearArea (XtDisplay(gw), XtWindow(gw),
		      kx + pw->panner.knob_width - 1, ly > ky ? ly : ky,
		      (unsigned int) lx - kx,
		      (unsigned int) height,
		      False);
	}
      }
    }

    pw->panner.last_x = pw->panner.knob_x;
    pw->panner.last_y = pw->panner.knob_y;

    /* use Simple class method to fetch either a normal GC or an
       insensitive GC. Draw a rectangle inside the shadow. */
    XFillRectangle (XtDisplay(gw), XtWindow(gw),
		    wc->simple_class.select_gc(gw),
		    kx + pw->primitive.shadow_thickness,
		    ky + pw->primitive.shadow_thickness,
		    pw->panner.knob_width - 2 * pw->primitive.shadow_thickness - 1,
		    pw->panner.knob_height - 2 * pw->primitive.shadow_thickness - 1);

    XmeDrawShadows (XtDisplay (gw), XtWindow(gw),
		    pw->primitive.top_shadow_GC,
		    pw->primitive.bottom_shadow_GC,
		    kx, ky,
		    pw->panner.knob_width - 1, pw->panner.knob_height - 1,
		    pw->primitive.shadow_thickness,
		    XmSHADOW_OUT);

    /* if in the middle of rubber_banding, redisplay it as well */
    if (pw->panner.tmp.doing && pw->panner.rubber_band)
      DrawRubberBand (pw);
}


/*********************************************************************
 *
 *  CreateGC
 *      an ExmSimple class method
 *
 *********************************************************************/
static void
CreateGC(
   Widget w)
{
    ExmPannerWidget pw = (ExmPannerWidget) w ;
    XtGCMask valuemask = (GCForeground | GCFunction);
    XGCValues values;

    /* Envelop our superclass method to get the normal
       and the insensitive GC and then add our rubber_band GC */
    (*(exmSimpleClassRec.simple_class.create_gc))(w);

    values.foreground = pw->primitive.foreground ^
	pw->core.background_pixel;
    values.function = GXxor;
    pw->panner.xor_gc = XtGetGC ((Widget) pw, valuemask, &values);
}


/*********************************************************************
 *
 *  DestroyGC
 *      an ExmSimple class method
 *
 *********************************************************************/
static void
DestroyGC(
   Widget w)
{
 ExmPannerWidget pw = (ExmPannerWidget) w ;

  /* Envelop our superclass method to destroy the normal
     and the insensitive GC, and then release our Xor GC */
    (*(exmSimpleClassRec.simple_class.destroy_gc))(w);

    XtReleaseGC ((Widget) pw, pw->panner.xor_gc);
}


/*********************************************************************
 *
 *  DrawRubberBand
 *      Draw the rubberband rectangle when rubberband dragging is used.
 *
 *********************************************************************/

static void
DrawRubberBand(
    ExmPannerWidget pw)
{
  XDrawRectangle (XtDisplay(pw), XtWindow(pw),
		  pw->panner.xor_gc,
		  (int) (pw->panner.tmp.x - 1 + pw->simple.margin_width),
		  (int) (pw->panner.tmp.y - 1 + pw->simple.margin_height),
		  (unsigned int) (pw->panner.knob_width),
		  (unsigned int) (pw->panner.knob_height));
  pw->panner.tmp.showing = !pw->panner.tmp.showing;
}


/*********************************************************************
 *
 *  CheckKnob
 *      Called by several action methods.
 *
 *********************************************************************/
static void
CheckKnob(
    ExmPannerWidget pw,
    Boolean knob)
{
 Position maxx = (((Position) pw->core.width) -
		     pw->simple.margin_width * 2 -
		     ((Position) pw->panner.knob_width));
 Position maxy = (((Position) pw->core.height) -
		     pw->simple.margin_height * 2 -
		     ((Position) pw->panner.knob_height));
 Position *x = (knob ? &pw->panner.knob_x : &pw->panner.tmp.x);
 Position *y = (knob ? &pw->panner.knob_y : &pw->panner.tmp.y);

    /* Note that positions are already normalized (i.e. margins
       have been subtracted out).  */
    if (*x < 0) *x = 0;
    if (*x > maxx) *x = maxx;

    if (*y < 0) *y = 0;
    if (*y > maxy) *y = maxy;

    if (knob) {
	pw->panner.slider_x = (Position) (((double) pw->panner.knob_x) /
					  pw->panner.haspect + 0.5);
	pw->panner.slider_y = (Position) (((double) pw->panner.knob_y) /
					  pw->panner.vaspect + 0.5);
	pw->panner.last_x = pw->panner.last_y = PANNER_OUTOFRANGE;
    }
}


/*********************************************************************
 *
 *  ScaleKnob
 *      Called by Resize and SetValues.  This function sets the knob
 *      size and/or its location.
 *
 *********************************************************************/
static void
ScaleKnob (
    ExmPannerWidget pw,
    Boolean location,
    Boolean size)
{
  if (location) {
    pw->panner.knob_x = (Position) PANNER_HSCALE(pw, pw->panner.slider_x);
    pw->panner.knob_y = (Position) PANNER_VSCALE(pw, pw->panner.slider_y);
  }

  if (size) {
    Dimension width, height;

      if (pw->panner.slider_width < 1) {
        pw->panner.slider_width = pw->panner.canvas_width;
      }

      if (pw->panner.slider_height < 1) {
        pw->panner.slider_height = pw->panner.canvas_height;
      }

      width = (pw->panner.slider_width < pw->panner.canvas_width)?
      pw->panner.slider_width : pw->panner.canvas_width;
      height = (pw->panner.slider_height < pw->panner.canvas_height)?
      pw->panner.slider_height : pw->panner.canvas_height ;

      pw->panner.knob_width = (Dimension) PANNER_HSCALE(pw, width);
      pw->panner.knob_height = (Dimension) PANNER_VSCALE(pw, height);
  }

  CheckKnob (pw, TRUE);
}



/***************************************************************************
 *                                                                          *
 * 		      Panner action procs                                   *
 *                                                                          *
 **************************************************************************/




/************************************************************************
 *
 *  ExmPannerStart
 *
 *
 ************************************************************************/
static void
ExmPannerStart (
    Widget gw,
    XEvent *event,
    String *params,	  /* unused */
    Cardinal *num_params  /* unused */ )
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 int x, y;

    if (!GetEventXY (pw, event, &x, &y)) {
	XBell (XtDisplay(gw), 0);	/* should do error message */
	return;
    }

    if (*num_params != 1) {
	XBell (XtDisplay(gw), 0);
	return;
    }

    if (XmeNamesAreEqual (params[0], "1"))
	(void) XmProcessTraversal (gw, XmTRAVERSE_CURRENT);

    pw->panner.tmp.doing = TRUE;
    pw->panner.tmp.startx = pw->panner.knob_x;
    pw->panner.tmp.starty = pw->panner.knob_y;
    pw->panner.tmp.dx = (((Position) x) - pw->panner.knob_x);
    pw->panner.tmp.dy = (((Position) y) - pw->panner.knob_y);
    pw->panner.tmp.x = pw->panner.knob_x;
    pw->panner.tmp.y = pw->panner.knob_y;
    if (pw->panner.rubber_band)
      DrawRubberBand (pw);
}


/************************************************************************
 *
 *  ExmPannerStop
 *
 *
 ************************************************************************/
static void
ExmPannerStop (
    Widget gw,
    XEvent *event,
    String *params,	 /* unused */
    Cardinal *num_params /* unused */)
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 int x, y;

    if (GetEventXY (pw, event, &x, &y)) {
      pw->panner.tmp.x = ((Position) x) - pw->panner.tmp.dx;
      pw->panner.tmp.y = ((Position) y) - pw->panner.tmp.dy;
      CheckKnob (pw, FALSE);
    }

    if (pw->panner.rubber_band && pw->panner.tmp.showing)
      DrawRubberBand(pw);

    pw->panner.tmp.doing = FALSE;
}



/************************************************************************
 *
 *  ExmPannerAbort:
 *       Restores slider to the position it had at ExmPannerStart.
 *
 ************************************************************************/
static void
ExmPannerAbort (
    Widget gw,
    XEvent *event,
    String *params,			/* unused */
    Cardinal *num_params		/* unused */)
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;

    if (!pw->panner.tmp.doing)
      return;

    if (pw->panner.rubber_band && pw->panner.tmp.showing)
      DrawRubberBand (pw);

    if (!pw->panner.rubber_band) {	/* restore old position */
      pw->panner.tmp.x = pw->panner.tmp.startx;
      pw->panner.tmp.y = pw->panner.tmp.starty;
      ExmPannerNotify (gw, event, params, num_params);
    }
    pw->panner.tmp.doing = FALSE;
}


/************************************************************************
 *
 *  ExmPannerMove
 *
 *
 ************************************************************************/
static void
ExmPannerMove (
    Widget gw,
    XEvent *event,			/* must be a motion event */
    String *params,			/* unused */
    Cardinal *num_params 		/* unused */)
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 int x, y;

    if (!pw->panner.tmp.doing)
      return;

    if (!GetEventXY (pw, event, &x, &y)) {
	XBell (XtDisplay(gw), 0);	/* should do error message */
	return;
    }

    if (pw->panner.rubber_band && pw->panner.tmp.showing)
      DrawRubberBand (pw);

    pw->panner.tmp.x = ((Position) x) - pw->panner.tmp.dx;
    pw->panner.tmp.y = ((Position) y) - pw->panner.tmp.dy;

    if (!pw->panner.rubber_band) {
	ExmPannerNotify (gw, event, params, num_params);  /* does a check */
    } else {
	CheckKnob (pw, FALSE);
	DrawRubberBand (pw);
    }
}



/************************************************************************
 *
 *  ExmPannerNotify
 *
 *
 ************************************************************************/
static void
ExmPannerNotify (
    Widget gw,
    XEvent *event,			/* unused */
    String *params,			/* unused */
    Cardinal *num_params		/* unused */)
{
 ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(gw);
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 Position tmp;


    if (!pw->panner.tmp.doing)
      return;

    CheckKnob (pw, FALSE);
    pw->panner.knob_x = pw->panner.tmp.x;
    pw->panner.knob_y = pw->panner.tmp.y;

    pw->panner.slider_x = (Position) (((double) pw->panner.knob_x) /
				      pw->panner.haspect + 0.5);
    pw->panner.slider_y = (Position) (((double) pw->panner.knob_y) /
				      pw->panner.vaspect + 0.5);
    /* check knob */
    if (pw->panner.slider_x >
	(tmp = (((Position) pw->panner.canvas_width) -
		((Position) pw->panner.slider_width))))
	  pw->panner.slider_x = tmp;
    if (pw->panner.slider_x < 0) pw->panner.slider_x = 0;
    if (pw->panner.slider_y >
	(tmp = (((Position) pw->panner.canvas_height) -
		((Position) pw->panner.slider_height))))
	pw->panner.slider_y = tmp;
    if (pw->panner.slider_y < 0) pw->panner.slider_y = 0;

    /* call callback if needed */
    if (pw->panner.last_x != pw->panner.knob_x ||
	pw->panner.last_y != pw->panner.knob_y) {
	ExmPannerCallbackStruct rep;

	wc->core_class.expose (gw, (XEvent*) NULL, (Region) NULL);
	rep.changed = (ExmPRSliderX | ExmPRSliderY);
	rep.slider_x = pw->panner.slider_x;
	rep.slider_y = pw->panner.slider_y;
	rep.slider_width = pw->panner.slider_width;
	rep.slider_height = pw->panner.slider_height;
	rep.canvas_width = pw->panner.canvas_width;
	rep.canvas_height = pw->panner.canvas_height;
	XtCallCallbackList (gw, pw->panner.report_callbacks,
			    (XtPointer) &rep);
    }
}


/************************************************************************
 *
 *  ExmPannerSet
 *
 *
 ************************************************************************/
static void
ExmPannerSet (
    Widget gw,
    XEvent *event,			/* unused */
    String *params,
    Cardinal *num_params)
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 Boolean rb;

    if (*num_params < 2 ||
	!XmeNamesAreEqual (params[0], "rubberband")) {
	XBell (XtDisplay(gw), 0);
	return;
    }

    if (XmeNamesAreEqual (params[1], "on")) {
	rb = TRUE;
    } else if (XmeNamesAreEqual (params[1], "off")) {
	rb = FALSE;
    } else if (XmeNamesAreEqual (params[1], "toggle")) {
	rb = !pw->panner.rubber_band;
    } else {
	XBell (XtDisplay(gw), 0);
	return;
    }

    if (rb != pw->panner.rubber_band) {
	XtVaSetValues (gw, ExmNrubberBand, rb, NULL);
    }
}


/************************************************************************
 *
 *  GetEventXY
 *
 *
 ************************************************************************/
static Boolean
GetEventXY (
    ExmPannerWidget pw,
    XEvent *event,
    int *x,
    int *y)
{
    switch (event->type) {
      case ButtonPress:
      case ButtonRelease:
	*x = event->xbutton.x - pw->simple.margin_width;
	*y = event->xbutton.y - pw->simple.margin_height;
	return TRUE;

      case KeyPress:
      case KeyRelease:
	*x = event->xkey.x - pw->simple.margin_width;
	*y = event->xkey.y - pw->simple.margin_height;
	return TRUE;

      case EnterNotify:
      case LeaveNotify:
	*x = event->xcrossing.x - pw->simple.margin_width;
	*y = event->xcrossing.y - pw->simple.margin_height;
	return TRUE;

      case MotionNotify:
	*x = event->xmotion.x - pw->simple.margin_width;
	*y = event->xmotion.y - pw->simple.margin_height;
	return TRUE;
    }

    return FALSE;
}


/************************************************************************
 *
 *  ParsePageString
 *
 *
 ************************************************************************/
static int
ParsePageString (
    char *s,
    int pagesize,
    int canvassize,
    Boolean *relative)
{
 char *cp;
 int val = 1;
 Boolean rel = FALSE;

    /* Syntax:    spaces [+-] number spaces [pc\0] spaces  */

    for (; isascii(*s) && isspace(*s); s++) ;	/* skip white space */

    if (*s == '+' || *s == '-') {	/* deal with signs */
	rel = TRUE;
	if (*s == '-') val = -1;
	s++;
    }

    if (!*s) {				/* if null then return nothing */
	*relative = TRUE;
	return 0;
    }

					/* skip over numbers */
    for (cp = s; isascii(*s) && isdigit(*s); s++) ;
    val *= atoi (cp);

					/* skip blanks */
    for (; isascii(*s) && isspace(*s); s++) ;

    if (*s) {				/* if units */
	switch (s[0]) {
	  case 'p': case 'P':
	    val *= pagesize;
	    break;

	  case 'c': case 'C':
	    val *= canvassize;
	    break;
	}
    }
    *relative = rel;
    return (val);
}




/************************************************************************
 *
 *  ExmPannerPage
 *
 *
 ************************************************************************/
static void
ExmPannerPage (
    Widget gw,
    XEvent *event,			/* unused */
    String *params,
    Cardinal *num_params		/* unused */)
{
 ExmPannerWidget pw = (ExmPannerWidget) gw;
 Cardinal zero = 0;
 Boolean isin = pw->panner.tmp.doing;
 int x, y;
 Boolean relx, rely;

    if (*num_params != 2) {
	XBell (XtDisplay(gw), 0);
	return;
    }

    x = ParsePageString (params[0], (int) pw->panner.knob_width,
			   ((int) pw->core.width) -
			 pw->simple.margin_width*2, &relx);
    y = ParsePageString (params[1], (int) pw->panner.knob_height,
			   ((int) pw->core.height) -
			 pw->simple.margin_height*2, &rely);

    if (relx) x += pw->panner.knob_x;
    if (rely) y += pw->panner.knob_y;

    if (isin) {				/* if in, then use move */
	XEvent ev;
	ev.xbutton.type = ButtonPress;
	ev.xbutton.x = x;
	ev.xbutton.y = y;
	ExmPannerMove (gw, &ev, (String *) NULL, &zero);
    } else {				/* else just do it */
	pw->panner.tmp.doing = TRUE;
	pw->panner.tmp.x = x;
	pw->panner.tmp.y = y;
	ExmPannerNotify (gw, event, (String *) NULL, &zero);
	pw->panner.tmp.doing = FALSE;
    }
}


/***************************************************************************
 *                                                                         *
 * 		      XmQTnavigator Trait Methods                          *
 *                                                                         *
 **************************************************************************/


/************************************************************************
 *
 *  NavigChangeMoveCB
 *	store or remove the callback list to be called on any move.
 *
 ************************************************************************/
static void
NavigChangeMoveCB(
           Widget nav,
	   XtCallbackProc moveCB,
           XtPointer closure,
           Boolean setunset)
{
    if (setunset)
	XtAddCallback (nav, ExmNreportCallback, moveCB, closure);
    else
	XtRemoveCallback (nav, ExmNreportCallback, moveCB, closure);
}




/************************************************************************
 *
 *  NavigSetValue
 *	change the value and possibly call the callbacks
 *
 ************************************************************************/
static void
NavigSetValue(
           Widget nav,
	   XmNavigatorData nav_data,
           Boolean notify)
{
 ExmPannerWidget pw = (ExmPannerWidget) nav;
 Arg arglist[6];
 Cardinal n ;
 int hsave_value, vsave_value ;

    if (nav_data->valueMask & NavDimMask) {
	pw->panner.dimMask = nav_data->dimMask ;
    }

    /* Panner is a two dimensional navigator, it expects
       one or two dimension to be set, either case should match
       the ones registered using the NavDimMask flag as aboce. */

    if (!(pw->panner.dimMask & nav_data->dimMask)) return ;

    hsave_value = pw->panner.slider_x ;
    vsave_value = pw->panner.slider_y ;

    n = 0;
    if (nav_data->dimMask & NavigDimensionX) {
	if ((nav_data->valueMask & NavValue) &&
	    (pw->panner.slider_x != nav_data->value.x)) {
	    XtSetArg (arglist[n], ExmNsliderX, nav_data->value.x);	n++;
	}
	if ((nav_data->valueMask & NavMaximum) &&
	    (pw->panner.canvas_width != nav_data->maximum.x)) {
	    XtSetArg (arglist[n], ExmNcanvasWidth,nav_data->maximum.x); n++;
	}
	if ((nav_data->valueMask & NavSliderSize) &&
	    (pw->panner.slider_width != nav_data->slider_size.x) &&
	    (nav_data->slider_size.x != 0)) {
	    XtSetArg (arglist[n], ExmNsliderWidth, nav_data->slider_size.x);
	    n++;
	}
    }

    if (nav_data->dimMask & NavigDimensionY) {
	if ((nav_data->valueMask & NavValue) &&
	    (pw->panner.slider_y != nav_data->value.y)) {
	    XtSetArg (arglist[n], ExmNsliderY, nav_data->value.y);	n++;
	}
	if ((nav_data->valueMask & NavMaximum) &&
	    (pw->panner.canvas_height != nav_data->maximum.y)) {
	    XtSetArg (arglist[n], ExmNcanvasHeight, nav_data->maximum.y);n++;
	}
	if ((nav_data->valueMask & NavSliderSize) &&
	    (pw->panner.slider_height != nav_data->slider_size.y) &&
	    (nav_data->slider_size.y != 0)) {
	    XtSetArg (arglist[n], ExmNsliderHeight, nav_data->slider_size.y);
	    n++;
	}
    }

    if (n) XtSetValues (nav, arglist, n);

    if (notify && ((pw->panner.slider_x != hsave_value) ||
		   (pw->panner.slider_y != vsave_value))) {
	ExmPannerCallbackStruct rep;

	rep.changed = (ExmPRSliderX | ExmPRSliderY); /* not always both */
	rep.slider_x = pw->panner.slider_x;
	rep.slider_y = pw->panner.slider_y;
	rep.slider_width = pw->panner.slider_width;
	rep.slider_height = pw->panner.slider_height;
	rep.canvas_width = pw->panner.canvas_width;
	rep.canvas_height = pw->panner.canvas_height;
	XtCallCallbackList (nav, pw->panner.report_callbacks,
			    (XtPointer) &rep);
    }
}


/************************************************************************
 *
 *  NavigGetValue
 *	reports the all the data for this navigator scrollbar
 *
 ************************************************************************/
static void
NavigGetValue(
    Widget nav,
    XmNavigatorData nav_data)
{
    ExmPannerWidget pw = (ExmPannerWidget) nav;

    nav_data->dimMask =  pw->panner.dimMask;

    if (nav_data->valueMask &
	(NavValue|NavMinimum|NavMaximum|
	 NavSliderSize|NavIncrement|NavPageIncrement)) {

	nav_data->value.x = pw->panner.slider_x ;
	nav_data->value.y = pw->panner.slider_y ;
	nav_data->maximum.x = pw->panner.canvas_width ;
	nav_data->maximum.y = pw->panner.canvas_height ;
	nav_data->slider_size.x = pw->panner.slider_width ;
	nav_data->slider_size.y = pw->panner.slider_height ;

    }
}



/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/


/************************************************************************
 *
 *  ExmCreatePanner
 *	Create an instance of a Panner and return the widget id.
 *
 ************************************************************************/
Widget
ExmCreatePanner(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, exmPannerWidgetClass,
                           parent, arglist, argcount));
}



/************************************************************************
 *
 *  ExmPannerSetValues
 *	Set some Panner values.
 *
 ************************************************************************/
void
ExmPannerSetValues(
        Widget w,
        int hvalue,
        int vvalue,
#if NeedWidePrototypes
        int notify )
#else
        Boolean notify )
#endif /* NeedWidePrototypes */
{
 ExmPannerWidget pw = (ExmPannerWidget) w;
 int       hsave_value, vsave_value;

   hsave_value = pw->panner.slider_x ;
   vsave_value = pw->panner.slider_y ;

   XtVaSetValues (w, ExmNsliderX, hvalue, ExmNsliderY, vvalue, NULL);

   if (notify && ((pw->panner.slider_x != hsave_value) ||
		  (pw->panner.slider_y != vsave_value))) {
       ExmPannerCallbackStruct rep;

       rep.changed = (ExmPRSliderX | ExmPRSliderY); /* not always both..*/
       rep.slider_x = pw->panner.slider_x;
       rep.slider_y = pw->panner.slider_y;
       rep.slider_width = pw->panner.slider_width;
       rep.slider_height = pw->panner.slider_height;
       rep.canvas_width = pw->panner.canvas_width;
       rep.canvas_height = pw->panner.canvas_height;
       XtCallCallbackList (w, pw->panner.report_callbacks, (XtPointer) &rep);
    }
}

