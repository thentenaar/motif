/* $XConsortium: TabB.c /main/5 1995/07/15 20:42:02 drk $ */
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
 * ExmTabButton is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way
 *********************************** WARNING *********************************/


/******************************************************************************
 *
 * TabB.c - ExmTabButton widget.  This widget can serve as a Notebook tab. 
 *          This widget can affix itself to any parent widget that knows how
 *          to call the appropriate XmQTjoinSide trait methods.
 *          The ExmTabButton widget demonstrates how to 
 *               * install the XmQTjoinSide trait. 
 *               * use XmeDrawHighlight 
 *               * use XmeClearBorder
 *               * use XmeDrawShadows
 *               * use XmeResolvePartOffsets
 *          See the "OSF/Motif Widget Writer's Guide" for details.
 *
******************************************************************************/

#include <Xm/XmP.h>         /* private header file for XmPrimitive */
#include <Exm/TabBP.h>      /* private header file for ExmTabButton widget */
#include <Xm/DrawP.h>       /* for XmeDraw functions */
#include <Xm/TraitP.h>      /* for XmeTrait functions */
#include <Xm/RepType.h>     /* for representation type facility */
#include <Xm/SpecRenderT.h> /* defines RENDER_TABLE types. */
#include <Xm/JoinSideT.h>   /* for XmQTjoinSide trait */
#include <X11/extensions/shape.h> 

#define WARNING_SHAPE_CHANGED "Widget not allowed to change shape"
#define WARNING_NO_SHAPE_EXTENSION "No Shape extension, style = rectangular"

/* Define the macros that we want to go thru offset. */
/* No need to offset primitive or core fields; the Intrinsics will not break 
   binary compatibility. */
#define OpenSide(w) \
    XmField(w, offsets, ExmTabButton, open_side, XtEnum)
#define JoinShadowThickness(w) \
    XmField(w, offsets, ExmTabButton, join_shadow_thickness, Dimension)
#define VisualArmed(w) \
    XmField(w, offsets, ExmCommandButton, visual_armed, Boolean)
#define SimpleShape(w) \
    XmField(w, offsets, ExmSimple, simple_shape, unsigned char)
#define NeedToReconfigure(w) \
    XmField(w, offsets, ExmSimple, need_to_reconfigure, Boolean)

#define DEFAULT_CORNER_ROUND_PERCENT 40 
/* corber size based on percent of the smaller dimension of the button */
#define CORNER_SIZE(w) \
   ((((w->core.width < w->core.height) ? w->core.width \
     : w->core.height) * DEFAULT_CORNER_ROUND_PERCENT) / 100)

/* Declare all static functions. */
static void ClassInitialize(void) ;
static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void FixVisualPosition(
		    Widget w );
static void Resize(
		    Widget w );
static void FillOpenRoundedRectangle(
				     Display		*dpy,
				     Drawable		draw,
				     GC			top_gc, 
				     GC                 bottom_gc, 
				     GC                 bg_gc,
				     int		x, 
				     int                y, 
				     int                w, 
				     int                h, 
				     int                shad_thick, 
				     int                corner_size,
				     unsigned char       open_side);
static void ShapeButton(
		    Widget w);
static void Realize(
		    Widget w,
		    XtValueMask *p_valueMask,
		    XSetWindowAttributes *attributes );
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetHighlightRectangle (
			ExmTabButtonWidget tb,
			XRectangle * hrect);
static void BorderHighlight( 
                        Widget wid) ;
static void BorderUnhighlight( 
                        Widget wid) ;
static void DrawShadow(
		        Widget wid);
static void JoinSideSetValue(Widget tab,
                        unsigned char  join_side, 
                        Dimension join_thickness);
static unsigned char JoinSideGetValue(Widget tab,
                        Dimension *join_thickness);


/* ExmTabButton provides no new translations.  All of its translations
   are inherited from ExmCommandButton. */


/* ExmTabButton provides only one new resource. 
   Use the resolve part offset type of resource */ 
static XmPartResource resources[] = {
    {
     ExmNopenSide, 
     ExmCOpenSide,  
     ExmROpenSide, sizeof (XtEnum),
     XmPartOffset (ExmTabButton, open_side),
     XmRImmediate, 
     (XtPointer) XmLEFT
   },
};

/* No synthetic resources. */

/* Define the widget class record.  See Chapter 3 of the 
   "OSF/Motif Widget Writer's Guide for details. */

externaldef(exmtabbuttonclassrec) 
ExmTabButtonClassRec exmTabButtonClassRec = {
  { /* Here is the Core class record. */
    /* superclass */	             (WidgetClass) &exmCommandButtonClassRec,
    /* class_name */	             "ExmTabButton",
    /* widget_size */	             sizeof(ExmTabButtonPart),
    /* class_initialize */           ClassInitialize,
    /* class_part_init */            ClassPartInitialize,
    /* class_inited */	             FALSE,
    /* initialize */	             Initialize,
    /* initialize_hook */            (XtArgsProc)NULL,
    /* realize */	             Realize,
    /* actions */	             NULL,
    /* num_actions */	             0,
    /* resources */	             (XtResourceList) resources,
    /* num_resources */	             XtNumber(resources),
    /* xrm_class */	             NULLQUARK,
    /* compress_motion */	     TRUE,
    /* compress_exposure */	     XtExposeCompressMaximal,
    /* compress_enterlv */           TRUE,
    /* visible_interest */	     FALSE,
    /* destroy */	             NULL,
    /* resize */	             Resize,
    /* expose */	             XtInheritExpose,
    /* set_values */	             SetValues,
    /* set_values_hook */            (XtArgsFunc)NULL,
    /* set_values_almost */          XtInheritSetValuesAlmost,
    /* get_values_hook */	     (XtArgsProc)NULL,
    /* accept_focus */	             (XtAcceptFocusProc)NULL,
    /* version */	             XtVersionDontCheck,
    /* callback_private */           NULL,
    /* tm_table */                   XtInheritTranslations,
    /* query_geometry */	     XtInheritQueryGeometry, 
    /* display_accelerator */        (XtStringProc)NULL,
    /* extension record */           NULL,
  },
  { /* Here is the XmPrimitive class record. */ 
    /* border_highlight */	     BorderHighlight,
    /* border_unhighlight */	     BorderUnhighlight,
    /* translations */               XtInheritTranslations,
    /* arm_and_activate */           XmInheritArmAndActivate,
    /* get resources */              NULL,
    /* num get_resources */          0,
    /* extension */                  NULL,
  },
  { /* Here is the ExmSimple class record. */
    /* draw_visual */                ExmInheritDrawVisual,
    /* draw_shadow */                DrawShadow,
    /* create_gc */                  ExmInheritCreateGC,
    /* destroy_gc */                 ExmInheritDestroyGC,
    /* select_gc */                  ExmInheritSelectGC,
    /* calc_visual_size */           ExmInheritCalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */                ExmInheritReconfigure,
    /* extension */                  NULL,
  },
  { /* Here is the ExmString class record. */ 
    /* default_render_table_type */  XmBUTTON_RENDER_TABLE,           
    /* extension */                  NULL,
  },
  { /* Here is the ExmCommandButton class record. */ 
    /* extension */                  NULL,
  },
  { /* Here is the ExmTabButton class record. */
    /* extension */                  NULL,
  },
};


/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */ 
externaldef(exmtabbuttonwidgetclass)
   WidgetClass exmTabButtonWidgetClass = (WidgetClass)&exmTabButtonClassRec;


/* Define static representation type variables here.  ExmTabButton
   introduces one new representation type. */ 
static String OpenSideNames[] = {
    "none", "left", "right", "top", "bottom"
};

static XmRepTypeId openSideId;


/* Define trait structure variables here. */
/* This widget will install the XmQTjoinSide trait. */ 
static const XmJoinSideTraitRec tabButtonOST = {
  0,		/* version */
  JoinSideSetValue,
  JoinSideGetValue,
};


/* Part Offset table for XmResolvePartOffsets */
static XmOffsetPtr offsets; 


/*************************************************************************
 *
 *  ClassInitialize
 *      Called by the Intrinsics the first time a widget of this class is
 *      instantiated.
 *
 ************************************************************************/
static void ClassInitialize()
{
 /* Register new representation types.  These new representation types
    will hold the names of the valid values of the ExmNopenSide resource. */

   openSideId = XmRepTypeRegister (ExmROpenSide, OpenSideNames,
				   NULL, XtNumber(OpenSideNames));

 /* resolve the offsets so that our XmField macro works fine */
   XmeResolvePartOffsets(exmTabButtonWidgetClass, &offsets, NULL);
}



/************************************************************************
 *
 *  ClassPartInitialize
 *      Called by the Intrinsics when this widget or a subclass of this
 *      widget is intantiated. 
 *
 ************************************************************************/
static void 
ClassPartInitialize(
        WidgetClass wc )
{
 /* Install the XmQTjoinSide trait on the ExmTabButton class and on all
    its subclasses. */
   XmeTraitSet((XtPointer) wc, XmQTjoinSide, (XtPointer) &tabButtonOST);
}


/*****************************************************************************
 *
 *  Initialize 
 *      Called when the widget is instantiated.
 *
 ***************************************************************************/
static void 
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
 ExmTabButtonWidgetClass wc = (ExmTabButtonWidgetClass)XtClass(nw);
 int shape_event_base, shape_error_base;

  /* Validate the value of ExmNopenSide.  If ExmNopenSide does not
     have a valid value, set its value to XmLEFT. */
   if (!XmRepTypeValidValue(openSideId, OpenSide(nw), nw)) 
     OpenSide(nw) = XmLEFT ;

 /* check if the shape extention is on the server */
  if (SimpleShape(nw) != ExmSHAPE_RECTANGLE
      && !XShapeQueryExtension(XtDisplay(nw), &shape_event_base, 
			       &shape_error_base)) {
      XmeWarning(nw, WARNING_NO_SHAPE_EXTENSION);
      SimpleShape(nw) = ExmSHAPE_RECTANGLE;
  }

   /* default the shadow join to the shadow */
    JoinShadowThickness(nw) = Prim_ShadowThickness(nw);

   if (wc->simple_class.reconfigure)
     (*(wc->simple_class.reconfigure))(exmTabButtonWidgetClass, nw, NULL);
}


/*****************************************************************************
 * 
 * FillOpenRoundedRectangle
 *      Xlib style API for round edge graphics.
 *      This is used for both the bitmap used as a shape and the rendering
 *      of the shaped shadows.
 *
 *****************************************************************************/
static void
FillOpenRoundedRectangle(
			 Display		*dpy,
			 Drawable		draw,
			 GC			top_gc, 
			 GC                 bottom_gc, 
			 GC                 bg_gc,
			 int		x, 
			 int                y, 
			 int                w, 
			 int                h, 
			 int                shad_thick, 
			 int                corner_size,
			 unsigned char       open_side)
{
    XArc	arcs[4];
    XRectangle  rects[2];
    XGCValues top_gcvalues, bottom_gcvalues;
    Dimension demi_shad = (shad_thick > 1)? (shad_thick/2):shad_thick;

    if (corner_size > w/2) corner_size = w/2;
    if (corner_size > h/2) corner_size = h/2;

   /* first save the current line attributes (have to do all of them
      because of the weak Xlib API).
      The default in the GC is 1, so do not change the line width there */
    if (shad_thick != 1) {
	XGetGCValues(dpy, top_gc,
		     GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,
		     &top_gcvalues);
	/* change the line width and draw the lines */
	XSetLineAttributes(dpy, top_gc,  shad_thick, 
			   LineSolid, CapButt, JoinMiter);

	if (bottom_gc != top_gc) {
	    XGetGCValues(dpy, bottom_gc,
			 GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,
			 &bottom_gcvalues);
	    XSetLineAttributes(dpy, bottom_gc,  shad_thick, 
			       LineSolid, CapButt, JoinMiter);
	}
    }
 

    /* this code is rather long and boring, but there is nothing
       complex here: we use arcs even for the vertical and horizontal
       lines, so that the requests are minimized. There are some
       known visual glitch with shadow = 1. */
    if (open_side == XmLEFT) {
	arcs[0].x = x ;
	arcs[0].y = y + demi_shad;
	arcs[0].width = w - corner_size;
	arcs[0].height = 0;
	arcs[0].angle1 = 180*64; 
	arcs[0].angle2 = -180*64;
	arcs[1].x = x + w - 2*corner_size + demi_shad;
	arcs[1].y = y + demi_shad;
	arcs[1].width = 2*corner_size - 2*demi_shad;
	arcs[1].height = 2*corner_size - 2*demi_shad;
	arcs[1].angle1 = 90*64; 
	arcs[1].angle2 = -50*64;
	if (shad_thick) XDrawArcs (dpy, draw, top_gc, arcs, 2);
	if (bg_gc) XFillArcs (dpy, draw, bg_gc, arcs+1, 1);

	arcs[0].x = x + w - 2*corner_size + demi_shad;
	arcs[0].y = y + demi_shad;
	arcs[0].width = 2*corner_size - 2*demi_shad;
	arcs[0].height = 2*corner_size - 2*demi_shad;
	arcs[0].angle1 = 40*64; 
	arcs[0].angle2 = -40*64;
	arcs[1].x = x + w - demi_shad;
	arcs[1].y = y + corner_size;
	arcs[1].width = 0;
	arcs[1].height = h - 2*corner_size;
	arcs[1].angle1 = 90*64; 
	arcs[1].angle2 = -180*64;
	arcs[2].x = x + w - 2*corner_size + demi_shad;
	arcs[2].y = y + h - 2*corner_size + demi_shad ;
	arcs[2].width = 2*corner_size - 2*demi_shad;
	arcs[2].height = 2*corner_size - 2*demi_shad;
	arcs[2].angle1 = 0*64; 
	arcs[2].angle2 = -90*64;
	arcs[3].x = x ;
	arcs[3].y = y + h - demi_shad;
	arcs[3].width = w - corner_size;
	arcs[3].height = 0 ;
	arcs[3].angle1 = 180*64; 
	arcs[3].angle2 = -180*64;
	if (shad_thick) XDrawArcs (dpy, draw, bottom_gc, arcs, 4);

	if (bg_gc) {
	    XFillArcs (dpy, draw, bg_gc, arcs, 3);
	    /* fill middle with rectangle */
	    rects[0].x = x;  rects[0].y = y;
	    rects[0].width = w - corner_size;
	    rects[0].height = h;
	    rects[1].x = x + w - corner_size;
	    rects[1].y = y + corner_size;
	    rects[1].width = corner_size;
	    rects[1].height = h - corner_size*2;
	    XFillRectangles (dpy, draw, bg_gc, rects, 2);
	}  
    } else
    if (open_side == XmBOTTOM) {
	arcs[0].x = x + demi_shad;
	arcs[0].y = y + corner_size;
	arcs[0].width = 0;
	arcs[0].height = h - corner_size;
	arcs[0].angle1 = 90*64; 
	arcs[0].angle2 = -180*64;
	arcs[1].x = x + demi_shad;
	arcs[1].y = y + demi_shad;
	arcs[1].width = 2*corner_size - 2*demi_shad;
	arcs[1].height = 2*corner_size - 2*demi_shad;
	arcs[1].angle1 = 180*64; 
	arcs[1].angle2 = -90*64;
	arcs[2].x = x + corner_size ;
	arcs[2].y = y + demi_shad ;
	arcs[2].width = w - 2*corner_size ;
	arcs[2].height = 0;
	arcs[2].angle1 = 180*64; 
	arcs[2].angle2 = -180*64;
	arcs[3].x = x + w - 2*corner_size + demi_shad;
	arcs[3].y = y + demi_shad;
	arcs[3].width = 2*corner_size - 2*demi_shad;
	arcs[3].height = 2*corner_size - 2*demi_shad ;
	arcs[3].angle1 = 90*64; 
	arcs[3].angle2 = -50*64;
	if (shad_thick) XDrawArcs (dpy, draw, top_gc, arcs, 4);
	if (bg_gc) XFillArcs (dpy, draw, bg_gc, arcs+1, 3);

	arcs[0].x = x + w - 2*corner_size + demi_shad;
	arcs[0].y = y + demi_shad;
	arcs[0].width = 2*corner_size - 2*demi_shad;
	arcs[0].height = 2*corner_size - 2*demi_shad;
	arcs[0].angle1 = 40*64; 
	arcs[0].angle2 = -40*64;
	arcs[1].x = x + w - demi_shad;
	arcs[1].y = y + corner_size;
	arcs[1].width = 0;
	arcs[1].height = h - corner_size;
	arcs[1].angle1 = 90*64; 
	arcs[1].angle2 = -180*64;
 	if (shad_thick) XDrawArcs (dpy, draw, bottom_gc, arcs, 2);

	if (bg_gc) {
	    XFillArcs (dpy, draw, bg_gc, arcs, 1);
	    /* fill middle with rectangle */
	    rects[0].x = x + corner_size;  rects[0].y = y;
	    rects[0].width = w - 2*corner_size;
	    rects[0].height = corner_size;
	    rects[1].x = x ;
	    rects[1].y = y + corner_size;
	    rects[1].width = w;
	    rects[1].height = h - corner_size;
	    XFillRectangles (dpy, draw, bg_gc, rects, 2);
	}  
    } else
    if (open_side == XmTOP) {
	arcs[0].x = x + demi_shad;
	arcs[0].y = y ;
	arcs[0].width = 0;
	arcs[0].height = h - corner_size;
	arcs[0].angle1 = 90*64; 
	arcs[0].angle2 = -180*64;
	arcs[1].x = x + demi_shad;
	arcs[1].y = y + h - 2*corner_size + demi_shad;
	arcs[1].width = 2*corner_size - 2*demi_shad;
	arcs[1].height = 2*corner_size - 2*demi_shad;
	arcs[1].angle1 = 180*64; 
	arcs[1].angle2 = 50*64;
	if (shad_thick) XDrawArcs (dpy, draw, top_gc, arcs, 2);
	if (bg_gc) XFillArcs (dpy, draw, bg_gc, arcs+1, 1);

	arcs[0].x = x + demi_shad;
	arcs[0].y = y + h - 2*corner_size + demi_shad;
	arcs[0].width = 2*corner_size - 2*demi_shad;
	arcs[0].height = 2*corner_size - 2*demi_shad;
	arcs[0].angle1 = 230*64; 
	arcs[0].angle2 = 40*64;
	arcs[1].x = x + corner_size;
	arcs[1].y = y + h - demi_shad;
	arcs[1].width = w - 2*corner_size;
	arcs[1].height = 0;
	arcs[1].angle1 = 180*64; 
	arcs[1].angle2 = -180*64;
	arcs[2].x = x + w - 2*corner_size + demi_shad;
	arcs[2].y = y + h - 2*corner_size + demi_shad ;
	arcs[2].width = 2*corner_size - 2*demi_shad;
	arcs[2].height = 2*corner_size - 2*demi_shad;
	arcs[2].angle1 = 270*64; 
	arcs[2].angle2 = 90*64;
	arcs[3].x = x + w - demi_shad;
	arcs[3].y = y ;
	arcs[3].width = 0;
	arcs[3].height = h - corner_size ;
	arcs[3].angle1 = 90*64; 
	arcs[3].angle2 = -180*64;
	if (shad_thick) XDrawArcs (dpy, draw, bottom_gc, arcs, 4);

	if (bg_gc) {
	    XFillArcs (dpy, draw, bg_gc, arcs, 3);
	    /* fill middle with rectangle */
	    rects[0].x = x;  rects[0].y = y;
	    rects[0].width = w ;
	    rects[0].height = h - corner_size;
	    rects[1].x = x + corner_size;
	    rects[1].y = y + h - corner_size;
	    rects[1].width = w - 2*corner_size;
	    rects[1].height = corner_size;
	    XFillRectangles (dpy, draw, bg_gc, rects, 2);
	}  
    }  else
    if (open_side == XmRIGHT) {
	arcs[0].x = x + corner_size;
	arcs[0].y = y + demi_shad;
	arcs[0].width = w - corner_size;
	arcs[0].height = 0;
	arcs[0].angle1 = 180*64; 
	arcs[0].angle2 = -180*64;
	arcs[1].x = x + demi_shad;
	arcs[1].y = y + demi_shad;
	arcs[1].width = 2*corner_size - 2*demi_shad;
	arcs[1].height = 2*corner_size - 2*demi_shad;
	arcs[1].angle1 = 180*64; 
	arcs[1].angle2 = -90*64;
	arcs[2].x = x + demi_shad ;
	arcs[2].y = y + corner_size ;
	arcs[2].width = 0;
	arcs[2].height = h - 2*corner_size;
	arcs[2].angle1 = 90*64; 
	arcs[2].angle2 = -180*64;
	arcs[3].x = x + demi_shad;
	arcs[3].y = y + h - 2*corner_size + demi_shad;
	arcs[3].width = 2*corner_size - 2*demi_shad;
	arcs[3].height = 2*corner_size - 2*demi_shad ;
	arcs[3].angle1 = 180*64; 
	arcs[3].angle2 = 50*64;
	if (shad_thick) XDrawArcs (dpy, draw, top_gc, arcs, 4);
	if (bg_gc) XFillArcs (dpy, draw, bg_gc, arcs+1, 3);

	arcs[0].x = x + demi_shad;
	arcs[0].y = y + h - 2*corner_size + demi_shad;
	arcs[0].width = 2*corner_size - 2*demi_shad;
	arcs[0].height = 2*corner_size - 2*demi_shad;
	arcs[0].angle1 = 230*64; 
	arcs[0].angle2 = 40*64;
	arcs[1].x = x + corner_size;
	arcs[1].y = y + h - demi_shad;
	arcs[1].width = w - corner_size;
	arcs[1].height = 0;
	arcs[1].angle1 = 180*64; 
	arcs[1].angle2 = -180*64;
 	if (shad_thick) XDrawArcs (dpy, draw, bottom_gc, arcs, 2);

	if (bg_gc) {
	    XFillArcs (dpy, draw, bg_gc, arcs, 1);
	    /* fill middle with rectangle */
	    rects[0].x = x ;  rects[0].y = y + corner_size;
	    rects[0].width = corner_size;
	    rects[0].height = h - 2*corner_size;
	    rects[1].x = x + corner_size;
	    rects[1].y = y ;
	    rects[1].width = w - corner_size;
	    rects[1].height = h ;
	    XFillRectangles (dpy, draw, bg_gc, rects, 2);
	}  
    }

   /* put the line attributes back */
    if (shad_thick != 1) {
	XSetLineAttributes(dpy, top_gc,  
			   top_gcvalues.line_width, top_gcvalues.line_style, 
			   top_gcvalues.cap_style, top_gcvalues.join_style);
	if (bottom_gc != top_gc)
	    XSetLineAttributes(dpy, bottom_gc,  
			       bottom_gcvalues.line_width, 
			       bottom_gcvalues.line_style, 
			       bottom_gcvalues.cap_style, 
			       bottom_gcvalues.join_style);
    }
}



/*****************************************************************************
 * 
 * ShapeButton
 *      Call on a realized window from Realize and Resize.
 *      Style is rounded edge at this point.
 *
 *****************************************************************************/
static void
ShapeButton(
Widget w)
{
    Pixmap p = XCreatePixmap(XtDisplay(w), XtWindow(w), 
			     XtWidth(w), XtHeight(w), 1 );
    XGCValues values;
    GC gc;

    /* create a gc width 0 to blank the pixmap and then with 1
       for the mask */
    values.foreground = 0;
    gc = XCreateGC (XtDisplay(w), p, GCForeground, &values );
    XFillRectangle( XtDisplay(w), p, gc, 0, 0, XtWidth(w), XtHeight(w));
    XSetForeground (XtDisplay(w), gc, 1);

    FillOpenRoundedRectangle(XtDisplay(w), p, gc, gc, gc, 0, 0, 
			     XtWidth(w), XtHeight(w),
			     Prim_ShadowThickness(w),
			     CORNER_SIZE(w), OpenSide(w));

    XShapeCombineMask(XtDisplay(w), XtWindow(w), ShapeBounding, 
		      0, 0, p, ShapeSet );
    XShapeCombineMask(XtDisplay(w), XtWindow(w),ShapeClip, 
		      0, 0, p, ShapeSet );

    XFreePixmap(XtDisplay(w), p );
    XFreeGC (XtDisplay(w), gc);
}


/*****************************************************************************
 * 
 * Realize
 *      Called by the Intrinsics to create the window for the widget.  This
 *      class's realize method creates a shaped window for this 
 *      exact class,  but uses the default window otherwise 
 *
 *****************************************************************************/
static void 
Realize(Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
    /* First call Primitive's method */
    xmPrimitiveClassRec.core_class.realize(w, p_valueMask, attributes);

    /* then reshape the button if needed */
    if (SimpleShape(w) != ExmSHAPE_RECTANGLE) ShapeButton(w);
}


/*****************************************************************************
 *
 *  FixVisualPosition: tabs are asymmetric, so we need to recenter
 *                     the string so it looks right.
 *
 ***************************************************************************/
static void 
FixVisualPosition(
    Widget w)
{
    /* apply some little correction to the string position as the
       shadow is only on one side for the TabButton */
    if (OpenSide(w) == XmLEFT)
	((ExmSimpleWidget)w)->simple.visual.x -= Prim_ShadowThickness(w) ;
    else
    if (OpenSide(w) == XmTOP)
	((ExmSimpleWidget)w)->simple.visual.y -= Prim_ShadowThickness(w) ;
    else
    if (OpenSide(w) == XmRIGHT)
	((ExmSimpleWidget)w)->simple.visual.x += Prim_ShadowThickness(w) ;
    else
    if (OpenSide(w) == XmBOTTOM)
	((ExmSimpleWidget)w)->simple.visual.y += Prim_ShadowThickness(w) ;
}

/*****************************************************************************
 *
 *  Resize
 *      Must reshape the window.
 *
 ***************************************************************************/
static void 
Resize(
    Widget w)
{
    if (XtIsRealized(w) && SimpleShape(w) != ExmSHAPE_RECTANGLE) 
	ShapeButton(w);

    /* call superclass Resize */
    (*exmCommandButtonClassRec.core_class.superclass->core_class.resize)(w);

    FixVisualPosition(w);
}

/*****************************************************************************
 *
 *  SetValues
 *      Called by the Intrinsics when the application tries to set a value
 *      of one of the resources.
 *
 ***************************************************************************/
static Boolean 
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    ExmSimpleWidgetClass wc = (ExmSimpleWidgetClass)XtClass(nw);
    Boolean expose_flag = False ;

    /* Make sure that the new value of ExmNopenSide is a valid one. */  
    if (OpenSide(nw) != OpenSide(cw)) {
	if (!XmRepTypeValidValue(openSideId, OpenSide(nw), nw)) 
	    OpenSide(nw) = OpenSide(cw);
	else {
	    /* call Resize so the shape and the visual position be reset */
	    if (wc->core_class.resize) 
		(*(wc->core_class.resize))(nw);
	    expose_flag = True ;
	}
    }

    /* Do no allow change in shape */
    if (SimpleShape(cw) != SimpleShape(nw)) {
	SimpleShape(nw) = SimpleShape(cw);
	XmeWarning(nw, WARNING_SHAPE_CHANGED);
    }

    /* must call our Resize for the shape update */
    if (SimpleShape(nw) == ExmSHAPE_OVAL && NeedToReconfigure(nw)) {
	if (wc->simple_class.reconfigure)
	    (*(wc->simple_class.reconfigure))(exmTabButtonWidgetClass, nw, cw);
    }

    return expose_flag ;
}


/*****************************************************************************
 *
 *  BorderHighlight 
 *
 ***************************************************************************/
static void 
BorderHighlight(
        Widget wid )
{
    XRectangle hrect ;
    ExmTabButtonWidget tb = (ExmTabButtonWidget) wid;

    tb->primitive.highlighted = True ;

  /* If there is no need to draw a border highlight, then don't bother. */
    if (XtWidth(wid) == 0 || 
        XtHeight( wid) == 0 || 
        tb->primitive.highlight_thickness == 0) 
      return;

    if (SimpleShape(wid) == ExmSHAPE_OVAL) {
	/* Render the label using the highlight color for a change.
	   The app must be sure the highlight color and the background and
	   different */
	if (tb->string.compound_string &&
	    (tb->simple.visual.width != 0) &&
	    (tb->simple.visual.height != 0)) {
	    XmStringDraw (XtDisplay(tb), XtWindow(tb),
			  tb->string.render_table, 
			  tb->string.compound_string,
			  tb->primitive.highlight_GC,
			  tb->simple.visual.x, tb->simple.visual.y,
			  tb->simple.visual.width, tb->string.alignment,
			  tb->primitive.layout_direction, NULL); 
	}
    } else {
	/* Determine where the highlight should be drawn and then draw it. */  
	GetHighlightRectangle(tb, &hrect);

	XmeDrawHighlight(XtDisplay( wid), XtWindow( wid), 
			 tb->primitive.highlight_GC,
			 hrect.x, hrect.y, hrect.width, hrect.height, 
			 tb->primitive.highlight_thickness);
    }
}


/*****************************************************************************
 *
 *  BorderUnhighlight
 *
 ***************************************************************************/
static void 
BorderUnhighlight(
        Widget wid )
{
    XRectangle hrect ;
    ExmTabButtonWidget tb = (ExmTabButtonWidget) wid;
    
  /* This flag tells XmPrimitive what the state of the highlights is. 
     By turning it to False, we are alerting XmPrimitive that the
     highlight is off. */
    tb->primitive.highlighted = False ;

  /* If there is no need to undraw a border highlight, then don't bother. */
    if (XtWidth(wid) == 0 || 
        XtHeight(wid) == 0 || 
        tb->primitive.highlight_thickness == 0) 
      return;

    if (SimpleShape(wid) == ExmSHAPE_OVAL) {
	/* Undo the render label using the highlight color */
	if (tb->string.compound_string &&
	    (tb->simple.visual.width != 0) &&
	    (tb->simple.visual.height != 0)) {
	    XmStringDraw (XtDisplay(tb), XtWindow(tb),
			  tb->string.render_table, 
			  tb->string.compound_string,
			  tb->simple.normal_gc,
			  tb->simple.visual.x, tb->simple.visual.y,
			  tb->simple.visual.width, tb->string.alignment,
			  tb->primitive.layout_direction, NULL); 
	}	
    } else {
	/* Determine where the highlight should be drawn and then draw it. */  
	GetHighlightRectangle(tb, &hrect);

	/* Erase the border highlights.  The border highlights of ExmTabButton 
	   are located inside the widget; therefore, we can XmeClearBorder 
	   rather than XmeDrawHighlight (which is used with the parent color
	   in Motif). */ 
	XmeClearBorder(XtDisplay (wid), XtWindow (wid), 
		       hrect.x, hrect.y, hrect.width, hrect.height, 
		       tb->primitive.highlight_thickness) ;	
    }
}


/************************************************************************
 *
 *  DrawShadow 
 *
 ************************************************************************/
static void 
DrawShadow (
        Widget wid)
{
    XPoint pts[3] ;
    XRectangle hrect ;
    GC tune_gc = 0;
    ExmTabButtonWidget tb = (ExmTabButtonWidget) wid;

  /* ExmTabButton cannot inherit the draw_shadow routine of ExmSimple.
     This is because ExmSimple's draw_shadow method assumes rectangular 
     shadows; however, ExmTabButton will require both rectangular
     and curved shadows.  That is, the shadows in an ExmTabButton will
     be partly rectangular and partly curved.  The rectangular portions
     will be drawn with XmeDrawShadows and the curved portions with
     XFillPolygon.  The resulting shadow will be drawn just outside the
     border highlight.  The shadow will have one open side and a 3D   
     appearance.  */

    if (SimpleShape(wid) == ExmSHAPE_RECTANGLE) {
	    
	/* Find the dimensions of the border highlight. */
	GetHighlightRectangle (tb, &hrect) ;

	/* Apply a correction to the returned border highlight dimensions 
	   since we're drawing the shadow this time around. */
	hrect.x -= tb->primitive.shadow_thickness ;
	hrect.y -= tb->primitive.shadow_thickness ;
	hrect.width += 2*tb->primitive.shadow_thickness ;
	hrect.height += 2*tb->primitive.shadow_thickness ;

	/* Draw the rectangular portion of the shadows. 
	   We're drawing a square shadow, but because the position are
	   negative on the open side, it'll look like an open shadow */
	XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
			tb->primitive.top_shadow_GC, 
			tb->primitive.bottom_shadow_GC,
			hrect.x, hrect.y, hrect.width, hrect.height,
			tb->primitive.shadow_thickness, 
			VisualArmed(tb) ? XmSHADOW_IN: XmSHADOW_OUT);
    } else {
	 FillOpenRoundedRectangle(XtDisplay(wid), XtWindow (wid), 
				  VisualArmed(tb) ?
				  tb->primitive.bottom_shadow_GC: 
				  tb->primitive.top_shadow_GC, 
				  VisualArmed(tb) ?
				  tb->primitive.top_shadow_GC: 
				  tb->primitive.bottom_shadow_GC, 
				  NULL, 0, 0, 
				  XtWidth(wid), XtHeight(wid),
				  tb->primitive.shadow_thickness,
				  CORNER_SIZE(wid), OpenSide(wid));
     }
	
    /* Now draw the shadow join triangle */
    /* Determine the dimensions of the curved portions of the shadow. */
    if (OpenSide(wid) == XmLEFT) {
	pts[0].x = 0 ; 
	pts[1].x = JoinShadowThickness(tb) ;
	pts[2].x = 0 ; 
	if (!VisualArmed(tb)) {
	    /* top part starts with half bottom gc to join clean */
	    pts[0].y = 0 ;
	    pts[1].y = 0 ;
	    pts[2].y = tb->primitive.shadow_thickness ;
	} else {
	    /* bottom part starts with half bottom gc to join clean */
	    pts[0].y = tb->core.height - tb->primitive.shadow_thickness ;
	    pts[1].y = tb->core.height ;
	    pts[2].y = tb->core.height ;
	}
	tune_gc = tb->primitive.bottom_shadow_GC ;
    } else 
    if (OpenSide(wid) == XmRIGHT) {
	pts[0].x = tb->core.width - JoinShadowThickness(tb) ;
	pts[1].x = tb->core.width ; 
	pts[2].x = tb->core.width ; 
	if (!VisualArmed(tb)) {
	    /* bottom part ends with half top gc to join clean */
	    pts[0].y = tb->core.height ;
	    pts[1].y = tb->core.height - tb->primitive.shadow_thickness ;
	    pts[2].y = tb->core.height ;
	} else {
	    /* top part ends with half top gc to join clean */
	    pts[0].y = 0 ;
	    pts[1].y = tb->primitive.shadow_thickness ;
	    pts[2].y = 0 ;
	}
	tune_gc = tb->primitive.top_shadow_GC ;
    } else 
    if (OpenSide(wid) == XmTOP) {
	pts[0].y = 0 ;
	pts[1].y = 0 ;
	pts[2].y = JoinShadowThickness(tb) ;
	if (!VisualArmed(tb)) {
	    /* right part starts with half bottom gc to join clean */
	    pts[0].x = 0 ; 
	    pts[1].x = tb->primitive.shadow_thickness ; 
	    pts[2].x = 0 ; 
	} else {
	    /* left part ends with half bottom gc to join clean */
	    pts[0].x = tb->core.width - tb->primitive.shadow_thickness ; 
	    pts[1].x = tb->core.width ; 
	    pts[2].x = tb->core.width ; 
	}
	tune_gc = tb->primitive.bottom_shadow_GC ;
    } else 
    if (OpenSide(wid) == XmBOTTOM) {
	pts[0].y = tb->core.height - JoinShadowThickness(tb) ;
	pts[1].y = tb->core.height ;
	pts[2].y = tb->core.height ;
	if (!VisualArmed(tb)) {
	    /* left part ends with half top gc to join clean */
	    pts[0].x = tb->core.width ; 
	    pts[1].x = tb->core.width - tb->primitive.shadow_thickness ; 
	    pts[2].x = tb->core.width ; 
	} else {
	    /* right part ends with half top gc to join clean */
	    pts[0].x = 0 ; 
	    pts[1].x = tb->primitive.shadow_thickness ; 
	    pts[2].x = 0 ; 
	}
	tune_gc = tb->primitive.top_shadow_GC ;
    } 
    /* else if (OpenSide(wid) == XmNONE) do nothing */
 


  /* Draw the curved portion of the shadows, unless join thickness is zero */
    if ((OpenSide(wid) != XmNONE) && (JoinShadowThickness(wid) > (Dimension)0))
	XFillPolygon (XtDisplay (tb), XtWindow (tb), 
		      tune_gc, pts, 3, Convex, CoordModeOrigin);
}



/*****************************************************************************
 *
 *  GetHighlightRectangle
 *      Called by BorderHighlight, BorderUnhighlight, and DrawShadow. ,
 *      in the case a rectangular tab only.
 *
 ***************************************************************************/
static void 
GetHighlightRectangle (
    ExmTabButtonWidget tb,
    XRectangle *hrect)
{
  /* This function writes the dimensions of the highlight rectangle 
     into *hrect.  These dimensions depend on the value of ExmNopenSide. */ 
    if (OpenSide(tb) == XmLEFT) {
	hrect->x = 0 ;
	hrect->y = tb->primitive.shadow_thickness ;
	hrect->width = XtWidth( tb) - tb->primitive.shadow_thickness ; 
	hrect->height = XtHeight(tb) - 2* tb->primitive.shadow_thickness ;
    } else 
    if (OpenSide(tb) == XmRIGHT) {		
	hrect->x = tb->primitive.shadow_thickness ;
	hrect->y = tb->primitive.shadow_thickness ;
	hrect->width = XtWidth(tb) - tb->primitive.shadow_thickness ; 
	hrect->height = XtHeight(tb) - 2* tb->primitive.shadow_thickness ;
    } else 
    if (OpenSide(tb) == XmTOP) {	
	hrect->x = tb->primitive.shadow_thickness ;
	hrect->y = 0 ;
	hrect->width = XtWidth(tb) - 2* tb->primitive.shadow_thickness ; 
	hrect->height = XtHeight(tb) - tb->primitive.shadow_thickness ;
    } else 
    if (OpenSide(tb) == XmBOTTOM) {		
	hrect->x = tb->primitive.shadow_thickness ;
	hrect->y = tb->primitive.shadow_thickness ;
	hrect->width = XtWidth(tb) - 2* tb->primitive.shadow_thickness ; 
	hrect->height = XtHeight(tb) - tb->primitive.shadow_thickness ;
    } else 
    if (OpenSide(tb) == XmNONE) {		
	hrect->x = tb->primitive.shadow_thickness ;
	hrect->y = tb->primitive.shadow_thickness ;
	hrect->width = XtWidth(tb) - 2* tb->primitive.shadow_thickness ; 
	hrect->height = XtHeight(tb) - 2*tb->primitive.shadow_thickness ;
    }
}



/***************************************************************
 * Trait methods for XmQTjoinSide trait 
 ***************************************************************/

/***************************************************************
 *
 * JoinSideSetValue:
 *      The setValue trait method of XmQTjoinSide. 
 *
 ***************************************************************/
static void 
JoinSideSetValue(Widget tab,
		 unsigned char join_side, 
		 Dimension join_thickness)
{
   ExmTabButtonWidget tb = (ExmTabButtonWidget) tab;
   Arg args[6];
   int n = 0;

   if (OpenSide(tb) != join_side)
	{
	XtSetArg (args[n], ExmNopenSide, join_side), n++;
	XtSetValues (tab, args, n);
	}

   if (JoinShadowThickness(tb) != join_thickness)
	{
	JoinShadowThickness(tb) = join_thickness;
	/* redraw, in the case XtSetValues will not */
	if (XtIsRealized(tab))
	   DrawShadow (tab);
	}

}


/***************************************************************
 *
 * JoinSideGetValue:
 *      The getValue trait method of XmQTjoinSide. 
 *
 ***************************************************************/
static unsigned char 
JoinSideGetValue(Widget tab,
		 Dimension  * join_thickness)
{
    ExmTabButtonWidget tb = (ExmTabButtonWidget) tab;
    
    *join_thickness = JoinShadowThickness(tb);

    return OpenSide(tb) ;
}


/************************************************************************
 *		Application Accessible External Functions
 ************************************************************************/

/************************************************************************
 *
 *  ExmCreateTabButton
 *	Create an instance of a TabButton and return its widget id.
 *
 ************************************************************************/
Widget 
ExmCreateTabButton(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, exmTabButtonWidgetClass, 
                           parent, arglist, argcount));
}
