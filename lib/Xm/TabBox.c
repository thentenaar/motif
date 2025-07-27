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
/*
 * COPYRIGHT NOTICE
 * Copyright (c) 1995 Integrated Computer Solutions
 */
/*
 * SYMBOLS
 * =======
 *
 * DEMO             - Controls if this is a demo version or not
 * SCROLLED_LAYOUT  - Controls if the scrolled layout features are supported
 * OVERLAYED_LAYOUT - Controls if the overlayed layout is supported
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "XmI.h"
#include <Xm/TabBoxP.h>
#ifdef SCROLLED_LAYOUT
#include <Xm/ArrowB.h>
#endif
#include <Xm/PrimitiveP.h>
#include <Xm/TabStackP.h>
#include <Xm/DrawUtils.h>
#include <Xm/XmP.h>

#if USE_XFT
#include <X11/Xft/Xft.h>
#endif

#define _XiBoolean Boolean

typedef enum {XiQUAD_1, XiQUAD_2, XiQUAD_3, XiQUAD_4} XiQuadrant;
typedef enum {gc_normal, gc_insensitive, gc_shadow} GC_type;

typedef struct _XmTabRect {
    int    x, y;
    int    width, height;
    int    row, column;
} XiTabRect;

typedef struct _XmCache {
    XImage *pixmap;
    XImage *label;
    Boolean sensitive;
} XiCache;

#ifdef _ARGS
#undef _ARGS
#endif
#define _ARGS(a) a

extern XmTabAttributes _XmTabbedStackListGet(XmTabbedStackList, int);
extern XmTabAttributes _XmTabbedStackListArray(XmTabbedStackList);
extern int             _XmTabbedStackListCount(XmTabbedStackList);

static void ClassInitialize (void);
static void ClassPartInitialize(WidgetClass w_class);
static void TabCanvasClassInitialize (void);
static void Initialize _ARGS((Widget, Widget, ArgList, Cardinal*));
static void Destroy _ARGS((Widget));
static void Resize _ARGS((Widget));
static Boolean SetValues _ARGS((Widget, Widget, Widget, ArgList, Cardinal*));
static XtGeometryResult QueryGeometry _ARGS((Widget, XtWidgetGeometry*,
					     XtWidgetGeometry*));

static void Realize _ARGS((Widget, XtValueMask*, XSetWindowAttributes*));

static void BorderHighlight _ARGS((Widget));
static void BorderUnhighlight _ARGS((Widget));

static void XmTabBoxArmAndActivate _ARGS((Widget, XEvent*, String*,Cardinal*));

static void _XmTabBoxTraverseRight _ARGS((Widget, XEvent*, String*,Cardinal*));
static void _XmTabBoxTraverseLeft _ARGS((Widget, XEvent*, String*, Cardinal*));
static void _XmTabBoxTraverseUp _ARGS((Widget, XEvent*, String*, Cardinal*));
static void _XmTabBoxTraverseDown _ARGS((Widget, XEvent*, String*, Cardinal*));

static void XmTabBoxArmTab _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxSelectTab _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxTraverseRight _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxTraverseLeft _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxTraverseUp _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxTraverseDown _ARGS((Widget, XEvent*, String*, Cardinal*));
static void XmTabBoxTraversePrevious _ARGS((Widget, XEvent*, String*,
					    Cardinal*));
static void XmTabBoxTraverseNext _ARGS((Widget, XEvent*, String*, Cardinal*));

static void CalcTabSize _ARGS((XmTabBoxWidget, XmTabAttributes,
			       XmTabOrientation, XmFontList, int, int, int,
			       int, int, int, Dimension*, Dimension*));

static void CalcStackedGeometry _ARGS((XmTabBoxWidget, XRectangle*));
static void HorizontalStackedLayout _ARGS((XmTabBoxWidget, _XiBoolean));
static void HorizontalStackedRedisplay _ARGS((XmTabBoxWidget));
static void VerticalStackedRedisplay _ARGS((XmTabBoxWidget));
static void VerticalStackedLayout _ARGS((XmTabBoxWidget, _XiBoolean));

static void CalcUnlimitedGeometry _ARGS((XmTabBoxWidget, XRectangle*));
static void CalcGeometry _ARGS((XmTabBoxWidget, XRectangle*));

static void HorizontalBasicLayout _ARGS((XmTabBoxWidget));
static void VerticalBasicLayout _ARGS((XmTabBoxWidget));
static void Layout _ARGS((XmTabBoxWidget));
static void Redisplay _ARGS((Widget, XEvent*, Region));
static void RedisplayTabs _ARGS((XmTabBoxWidget, Region));

static void DrawTab _ARGS((XmTabBoxWidget, XmTabAttributes, XiTabRect*,
			   _XiBoolean, _XiBoolean));

static Boolean CvtStringToTabOrientation _ARGS((Display*, XrmValue*, Cardinal*,
						XrmValue*, XrmValue*,
						XtPointer*));
static Boolean CvtStringToTabSide _ARGS((Display*, XrmValue*, Cardinal*,
					 XrmValue*, XrmValue*,
					 XtPointer*));
static Boolean CvtStringToTabStyle _ARGS((Display*, XrmValue*, Cardinal*,
					  XrmValue*, XrmValue*, XtPointer*));
static Boolean CvtStringToTabMode _ARGS((Display*, XrmValue*, Cardinal*,
					 XrmValue*, XrmValue*, XtPointer*));
static Boolean CvtStringToTabEdge _ARGS((Display*, XrmValue*, Cardinal*,
					 XrmValue*, XrmValue*, XtPointer*));

#ifdef SCROLLED_LAYOUT
static Boolean CvtStringToArrowPlacement _ARGS((Display*, XrmValue*, Cardinal*,
						XrmValue*, XrmValue*,
						XtPointer*));
#endif
static Boolean CvtStringToTabList _ARGS((Display*, XrmValue*, Cardinal*,
					 XrmValue*, XrmValue*, XtPointer*));
static void CvtDestroyTabList _ARGS((XtAppContext, XrmValue*, XtPointer,
				     XrmValue*, Cardinal*));

static void XiDrawCorner _ARGS((Display*, Drawable, GC, GC, int, int,
				unsigned int, unsigned int, unsigned int,
				XiQuadrant));


static void HorizontalBasicRedisplay _ARGS((XmTabBoxWidget));
static void VerticalBasicRedisplay _ARGS((XmTabBoxWidget));

static XImage *XiRotateImage _ARGS((XmTabBoxWidget, XImage*, int));

static void CalcCornerSize _ARGS((XmTabBoxWidget));
static int XiXYtoTab _ARGS((XmTabBoxWidget, int, int));

static void CalcTabGeometry _ARGS((XmTabBoxWidget));
static int CalcGeometryMinor _ARGS((XmTabBoxWidget, int));
static int CalcGeometryMajor _ARGS((XmTabBoxWidget, int));

static void CallCallbacks _ARGS((XmTabBoxWidget, XEvent*, int, int));

static void SetRightGC(XmTabBoxWidget tab, GC gc, GC_type);
static void DrawVerticalTab _ARGS((XmTabBoxWidget, XmTabAttributes, GC,
				   _XiBoolean, int, int, int, _XiBoolean, int,
				   int, XRectangle*, _XiBoolean, _XiBoolean));
static void DrawLeftToRightTab _ARGS((XmTabBoxWidget, XmTabAttributes, GC,
				      _XiBoolean, int, int, int, _XiBoolean,
				      int, int, XRectangle*));
static void DrawRightToLeftTab _ARGS((XmTabBoxWidget, XmTabAttributes, GC,
				      _XiBoolean, int, int, int, _XiBoolean,
				      int, int, XRectangle*, _XiBoolean));
static void DrawTopToBottomTab _ARGS((XmTabBoxWidget, XmTabAttributes, GC,
				      _XiBoolean, int, int, int, _XiBoolean,
				      int, int, XRectangle*, _XiBoolean));
static void DrawBottomToTopTab _ARGS((XmTabBoxWidget, XmTabAttributes, GC,
				      _XiBoolean, int, int, int, _XiBoolean,
				      int, int, XRectangle*, _XiBoolean));

static void FillRoundedTab _ARGS((XmTabBoxWidget, GC, XiTabRect*, XmTabEdge));
static void FillBeveledTab _ARGS((XmTabBoxWidget, GC, XiTabRect*, XmTabEdge));

static XmTabAttributes GetTabInfo _ARGS((XmTabBoxWidget, int, int));
static int GetTabIndex _ARGS((XmTabBoxWidget, int, int));
static void SelectTab _ARGS((XmTabBoxWidget, XEvent*, int, int));
static Boolean IsTabSensitive _ARGS((XmTabBoxWidget, int));
static Widget XiGCParent _ARGS((XmTabBoxWidget));

static void ResetImageCache _ARGS((XmTabBoxWidget));
static void FreeImageCache _ARGS((XmTabBoxWidget));
static int InfoToIndex _ARGS((XmTabBoxWidget, XmTabAttributes));

static Visual* GetShellVisual _ARGS((Widget));

static void CheckSetRenderTable(Widget wid, int offs, XrmValue *value);

#define CachePixmap(t,i) XmTabBox__cache((t))[InfoToIndex(t,i)].pixmap
#define CacheLabel(t,i)  XmTabBox__cache((t))[InfoToIndex(t,i)].label
#define CacheSensitive(t,i)  XmTabBox__cache((t))[InfoToIndex(t,i)].sensitive

#ifndef AssignMax
#define AssignMax(x,y) if((y)>(x)) (x)=(y); else
#endif
#ifndef AssignMin
#define AssignMin(x,y) if((y)<(x)) (x)=(y); else
#endif
#ifndef Max
#define Max(x,y) (((x)>(y))?(x):(y))
#endif

#define XiRectInRegion(r,x,y,w,h) ((r) == False ? RectangleIn : \
				   XRectInRegion(r,x,y,w,h))
#define XiCanvas(x) (XtWindow(XmTabBox__canvas((x))))
#define XiTabParent(x) ((XmTabBoxWidget) XtParent(x))
#define XImageWidth(i) (i)->width
#define XImageHeight(i) (i)->height
#define XImageDepth(i) (i)->depth;
#define XiTabDegree(x) ((x) == XmTABS_RIGHT_TO_LEFT ? 180 \
			: ((x) == XmTABS_TOP_TO_BOTTOM ? 90 \
			   : ((x) == XmTABS_BOTTOM_TO_TOP ? 270 : 0)))

#define ValidPixmap(p) ((p) != (Pixmap)NULL && \
			(p) != (Pixmap)XmUNSPECIFIED_PIXMAP && \
			(p) != XmPIXMAP_DYNAMIC )
#define XiCalcTabSize(t,i,w,h) \
	CalcTabSize((t),(i), XmTabBox_tab_orientation((t)), \
		    XmTabBox_font_list((t)), (t)->manager.shadow_thickness, \
		    XmTabBox_highlight_thickness((t)), \
		    XmTabBox_tab_margin_width((t)), \
		    XmTabBox_tab_margin_height((t)), \
		    XmTabBox_tab_label_spacing((t)), \
		    XmTabBox__corner_size((t)), (w), (h))

#define SetSolidGC(d,g,p) \
{ \
      XGCValues _macro_gc_values; \
      _macro_gc_values.foreground = (p); \
      _macro_gc_values.fill_style = FillSolid; \
      XChangeGC((d),(g), GCForeground | GCFillStyle, &_macro_gc_values); \
}

#define SetTiledGC(d,g,p) \
{ \
      XGCValues _macro_gc_values; \
      _macro_gc_values.tile = (p); \
      _macro_gc_values.fill_style = FillTiled; \
      XChangeGC((d),(g), GCTile | GCFillStyle, &_macro_gc_values); \
}

#define SetTiledXYGC(d,g,p,x,y) \
{ \
      XGCValues _macro_gc_values; \
      _macro_gc_values.tile = (p); \
      _macro_gc_values.fill_style = FillTiled; \
      _macro_gc_values.ts_x_origin = (x); \
      _macro_gc_values.ts_y_origin = (y); \
      XChangeGC((d),(g), GCTile | GCFillStyle | GCTileStipXOrigin | \
		GCTileStipYOrigin, &_macro_gc_values); \
}

#define SetStippledGC(d,g,p) \
{ \
      XGCValues _macro_gc_values; \
      _macro_gc_values.stipple = (p); \
      _macro_gc_values.fill_style = FillStippled; \
      _macro_gc_values.ts_x_origin = (x); \
      _macro_gc_values.ts_y_origin = (y); \
      XChangeGC((d),(g), GCStipple | GCFillStyle | GCTileStipXOrigin | \
		GCTileStipYOrigin, &_macro_gc_values); \
}

#define SetStippledXYGC(d,g,p,x,y) \
{ \
      XGCValues _macro_gc_values; \
      _macro_gc_values.stipple = (p); \
      _macro_gc_values.fill_style = FillStippled; \
      _macro_gc_values.ts_x_origin = (x); \
      _macro_gc_values.ts_y_origin = (y); \
      XChangeGC((d),(g), GCStipple | GCFillStyle | GCTileStipXOrigin | \
		GCTileStipYOrigin, &_macro_gc_values); \
}

#define XiBackgroundSpecified(i) \
	((i)->background != XmCOLOR_DYNAMIC || \
	 ValidPixmap((i)->background_pixmap))

#define SetBackgroundGC(t,i,g) \
{ \
      if( ValidPixmap((i)->background_pixmap) ) \
      { \
	    SetTiledGC(XtDisplay(t), (g), (i)->background_pixmap); \
      } \
      else \
      { \
	    SetSolidGC(XtDisplay(t), (g), (i)->background); \
      }\
}

#define XiSelectSpecified(t) \
	(XmTabBox_select_color((t)) != XmCOLOR_DYNAMIC || \
	 ValidPixmap(XmTabBox_select_pixmap((t))))

#define SetSelectGC(t,g) \
{ \
      if( ValidPixmap(XmTabBox_select_pixmap((t))) ) \
      { \
	    SetTiledGC(XtDisplay(t), (g), XmTabBox_select_pixmap((t))); \
      } \
      else \
      { \
	    SetSolidGC(XtDisplay(t), (g), XmTabBox_select_color((t))); \
      }\
}

#define XiChildSpecified(c) \
	(XmTabStackC_tab_background(c) != XmCOLOR_DYNAMIC || \
	 ValidPixmap(XmTabStackC_tab_background_pixmap(c)))

#define RemoveStipple(d,g) XSetFillStyle((d),(g), FillSolid)

static XtActionsRec actions[] = {
    { "XmTabBoxArmAndActivate",	  (XtActionProc) XmTabBoxArmAndActivate   },
    { "XmTabBoxArmTab",           (XtActionProc) XmTabBoxArmTab           },
    { "XmTabBoxSelectTab",        (XtActionProc) XmTabBoxSelectTab        },
    { "XmTabBoxTraverseRight",    (XtActionProc) XmTabBoxTraverseRight    },
    { "XmTabBoxTraverseLeft",     (XtActionProc) XmTabBoxTraverseLeft     },
    { "XmTabBoxTraverseUp",       (XtActionProc) XmTabBoxTraverseUp       },
    { "XmTabBoxTraverseDown",     (XtActionProc) XmTabBoxTraverseDown     },
    { "XmTabBoxTraversePrevious", (XtActionProc) XmTabBoxTraversePrevious },
    { "XmTabBoxTraverseNext",     (XtActionProc) XmTabBoxTraverseNext     },
};

static char traversal_translations[] =
    "<Key>osfRight:    XmTabBoxTraverseRight()\n\
     <Key>osfLeft:     XmTabBoxTraverseLeft()\n\
     <Key>osfUp:       XmTabBoxTraverseUp()\n\
     <Key>osfDown:     XmTabBoxTraverseDown()\n\
     ~c s <Key>Tab:    PrimitivePrevTabGroup()\n\
     ~c ~s <Key>Tab:   PrimitiveNextTabGroup()\n\
     c s <Key>Tab:     PrimitivePrevTabGroup()\n\
     c ~s <Key>Tab:    PrimitiveNextTabGroup()\n\
     <Key>osfHelp:     PrimitiveHelp()\n\
     <FocusIn>:        PrimitiveFocusIn()\n\
     <FocusOut>:       PrimitiveFocusOut()\n\
     <Enter>:	       PrimitiveEnter()\n\
     <Leave>:          PrimitiveLeave()\n\
     <Unmap>:          PrimitiveUnmap()";

static char canvas_translations[] =
    "<Btn1Down>:       XmTabBoxArmTab()\n\
     <Btn1Up>:         XmTabBoxSelectTab()\n\
     <Key>osfActivate: XmTabBoxArmAndActivate()\n\
     <Key>space:       XmTabBoxArmAndActivate()";

/* because this is a private widget for the TabStack, the resources are
** assumed already to be in pixels
*/

static XtResource resources[] =
{
  /* Inherit (but changed default) resources */
  {
    XmNshadowThickness, XmCShadowThickness, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
    XmRImmediate, (XtPointer) 2
  },

  /* TabBox Resources */
  {
    XmNtabStyle, XmCTabStyle, XmRTabStyle,
    sizeof(XmTabStyle), XtOffsetOf(XmTabBoxRec, tab_box.tab_style),
    XmRImmediate, (XtPointer) XmTABS_BEVELED
  },

  {
    XmNtabMode, XmCTabMode, XmRTabMode,
    sizeof(XmTabMode), XtOffsetOf(XmTabBoxRec, tab_box.tab_mode),
    XmRImmediate, (XtPointer) XmTABS_BASIC
  },

  {
    XmNtabList, XmCTabList, XmRTabList,
    sizeof(XmTabbedStackList), XtOffsetOf(XmTabBoxRec, tab_box.tab_list),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNtabOrientation, XmCTabOrientation, XmRTabOrientation,
    sizeof(XmTabOrientation), XtOffsetOf(XmTabBoxRec, tab_box.tab_orientation),
    XmRImmediate, (XtPointer) XmTABS_LEFT_TO_RIGHT
  },

  {
    XmNtabAutoSelect, XmCTabAutoSelect, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabBoxRec, tab_box.tab_auto_select),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNorientation, XmCOrientation, XmROrientation,
    sizeof(unsigned char), XtOffsetOf(XmTabBoxRec, tab_box.orientation),
    XmRImmediate, (XtPointer) XmHORIZONTAL
  },

  {
      XmNuniformTabSize, XmCUniformTabSize, XmRBoolean,
      sizeof(Boolean), XtOffsetOf(XmTabBoxRec, tab_box.uniform_tab_size),
      XmRImmediate, (XtPointer) True
  },

  {
    XmNtabSelectColor, XmCTabSelectColor, XmRXmPixel,
    sizeof(Pixel), XtOffsetOf(XmTabBoxRec, tab_box.select_color),
    XmRImmediate, (XtPointer) XmCOLOR_DYNAMIC
  },

  {
    XmNtabSelectPixmap, XmCTabSelectPixmap, XmRXmPixmap,
    sizeof(Pixel), XtOffsetOf(XmTabBoxRec, tab_box.select_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNtabMarginWidth, XmCMarginWidth, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmTabBoxRec, tab_box.tab_margin_width),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNtabMarginHeight, XmCMarginHeight, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmTabBoxRec, tab_box.tab_margin_height),
    XmRImmediate, (XtPointer) 2
  },

  {
      XmNtabLabelSpacing, XmCTabLabelSpacing, XmRDimension,
      sizeof(Dimension), XtOffsetOf(XmTabBoxRec, tab_box.tab_label_spacing),
      XmRImmediate, (XtPointer) 2
  },

  {
    XmNtabCornerPercent, XmCTabCornerPercent, XmRInt,
    sizeof(int), XtOffsetOf(XmTabBoxRec, tab_box.tab_corner_percent),
    XmRImmediate, (XtPointer) 40
  },

  {
      XmNnumStacks, XmCNumStacks, XmRInt,
      sizeof(int), XtOffsetOf(XmTabBoxRec, tab_box.num_stacks),
      XmRImmediate, (XtPointer) 5
  },
#ifdef SCROLLED_LAYOUT
  {
    XmNtabArrowPlacement, XmCTabArrowPlacement, XmRTabArrowPlacement,
    sizeof(int), XtOffsetOf(XmTabBoxRec, tab_box.arrow_placement),
    XmRImmediate, (XtPointer) XmTAB_ARROWS_ON_LEFT
  },
#endif
  {
    XmNtabEdge, XmCTabEdge, XmRTabEdge,
    sizeof(XmTabEdge), XtOffsetOf(XmTabBoxRec, tab_box.tab_edge),
    XmRImmediate, (XtPointer) XmTAB_EDGE_BOTTOM_RIGHT
  },

  {
    XmNtabOffset, XmCTabOffset, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmTabBoxRec, tab_box.tab_offset),
    XmRImmediate, (XtPointer) 10
  },

  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabBoxRec, tab_box.check_set_render_table),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmTabBoxRec, tab_box.font_list),
    XmRCallProc, (XtPointer) CheckSetRenderTable
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmTabBoxRec, tab_box.font_list),
    XmRCallProc, (XtPointer) CheckSetRenderTable
  },

  {
    XmNhighlightThickness, XmCHighlightThickness, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmTabBoxRec, tab_box.highlight_thickness),
     XmRImmediate, (XtPointer) 2
  },

  {
    XmNstackedEffect, XmCStackedEffect, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabBoxRec, tab_box.stacked_effect),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNuseImageCache, XmCUseImageCache, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabBoxRec, tab_box.use_image_cache),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNselectedIndex, XmCSelectedIndex, XmRInt,
    sizeof(int), XtOffsetOf(XmTabBoxRec, tab_box.selected_index),
    XmRImmediate, (XtPointer) -1
  },

  {
    XmNtraversalIndex, XmCTraversalIndex, XmRInt,
    sizeof(int), XtOffsetOf(XmTabBoxRec, tab_box.traversal_index),
    XmRImmediate, (XtPointer) -1
  },

  {
    XmNselectCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmTabBoxRec, tab_box.select_callback),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNunselectCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmTabBoxRec, tab_box.unselect_callback),
    XmRImmediate, (XtPointer) NULL
  }
};

typedef struct _XiTabCanvasClassPart {
    XtPointer extension;
} XiTabCanvasClassPart;

typedef struct _XiTabCanvasClassRec {
    CoreClassPart        core_class;
    XmPrimitiveClassPart primitive_class;
    XiTabCanvasClassPart canvas_class;
} XiTabCanvasClassRec;

typedef struct _XiTabCanvasPart {
    XtPointer extension;
} XiTabCanvasPart;

typedef struct _XiTabCanvasRec {
    CorePart        core;
    XmPrimitivePart primitive;
    XiTabCanvasPart canvas;
} XiTabCanvasRec;

XiTabCanvasClassRec xmTabCanvasClassRec = {
  { /* Core Fields */
    /* superclass	  */	(WidgetClass) &xmPrimitiveClassRec,
    /* class_name	  */	"XiTabCanvas",
    /* widget_size	  */	sizeof(XiTabCanvasRec),
    /* class_initialize   */    TabCanvasClassInitialize,
    /* chained class init */	ClassPartInitialize,
    /* class_inited       */	False,
    /* initialize	  */	NULL,
    /* initialize hook    */    NULL,
    /* realize		  */	XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	NULL,
    /* num_resources	  */	0,
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	True,
    /* compress_exposure  */	XtExposeCompressMultiple,
    /* compress enter/exit*/    True,
    /* visible_interest	  */	False,
    /* destroy		  */	NULL,
    /* resize		  */	NULL,
    /* expose		  */	Redisplay,
    /* set_values	  */	NULL,
    /* set values hook    */    NULL,
    /* set values almost  */    XtInheritSetValuesAlmost,
    /* get values hook    */    NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback offsetlst */    NULL,
    /* default trans      */    canvas_translations,
    /* query geo proc	  */	QueryGeometry,
    /* display accelerator*/	NULL,
    /* extension record   */    (XtPointer) NULL },
  { /* OSF/Motif Primitive Fields */
    /* border_highlight   */	BorderHighlight,
    /* border_unhighlight */	BorderUnhighlight,
    /* translations       */	traversal_translations,
    /* arm_and_activate   */	NULL,
    /* syn resources      */    NULL,
    /* num syn_resources  */	0,
    /* extension          */    NULL
  },
  { /* EPak Tab Canvas Fields */
    /* extension          */	NULL
  }
};

XmTabBoxClassRec xmTabBoxClassRec = {
  { /* Core Fields */
    /* superclass	  */	(WidgetClass) &xmManagerClassRec,
    /* class_name	  */	"XmTabBox",
    /* widget_size	  */	sizeof(XmTabBoxRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initial */	NULL,
    /* class_inited       */	False,
    /* initialize	  */	Initialize,
    /* initialize hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	actions,
    /* num_actions	  */	XtNumber(actions),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	True,
    /* compress_exposure  */	XtExposeCompressMultiple,
    /* compress enter/exit*/    True,
    /* visible_interest	  */	False,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	NULL,
    /* set_values	  */	SetValues,
    /* set values hook    */    NULL,
    /* set values almost  */    XtInheritSetValuesAlmost,
    /* get values hook    */    NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback offsetlst */    NULL,
    /* default trans      */    XtInheritTranslations,
    /* query geo proc	  */	QueryGeometry,
    /* display accelerator*/	NULL,
    /* extension record   */    (XtPointer) NULL },
  { /* Composite Fields */
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    (XtPointer) NULL },
  { /* Constraint Fields */
    /* resources          */    NULL,
    /* num_resources      */    0,
    /* constraint_size    */    sizeof (XmManagerConstraintRec),
    /* initialize         */    NULL,
    /* destroy            */    NULL,
    /* set_values         */    NULL,
    /* extension          */    (XtPointer) NULL },
  { /* OSF/Motif Manager Fields */
    /* translations       */    XtInheritTranslations,
    /* syn_resources      */    NULL,
    /* num_syn_resources  */    0,
    /* syn_resources      */    NULL,
    /* num_syn_resources  */    0,
    /* parent_process     */    XmInheritParentProcess,
    /* extension          */    (XtPointer) NULL },
  { /* EPak Tab Box Fields */
    /* extension          */    (XtPointer) NULL }
};

WidgetClass xmTabCanvasWidgetClass = (WidgetClass) &xmTabCanvasClassRec;
WidgetClass xmTabBoxWidgetClass = (WidgetClass) &xmTabBoxClassRec;

static void TabCanvasClassInitialize(void)
{
  /* do nothing */
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmTABBOX_BIT);
}

static void ClassInitialize(void)
{
    XtSetTypeConverter(XmRString, XmRTabOrientation,
		       CvtStringToTabOrientation, NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRTabStyle,
		       CvtStringToTabStyle, NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRTabMode,
		       CvtStringToTabMode, NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRTabEdge,
		       CvtStringToTabEdge, NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRTabSide,
		       CvtStringToTabSide, NULL, 0, XtCacheNone, NULL);
#ifdef SCROLLED_LAYOUT
    XtSetTypeConverter(XmRString, XmRTabArrowPlacement,
		       CvtStringToArrowPlacement, NULL, 0, XtCacheNone, NULL);
#endif
    XtSetTypeConverter(XmRString, XmRTabList,
		       CvtStringToTabList, NULL, 0, XtCacheNone,
		       CvtDestroyTabList);
}

static void
Initialize(Widget request, Widget set, ArgList arg_list, Cardinal *arg_cnt)
{
    XmTabBoxWidget   st = (XmTabBoxWidget) set;
    XRectangle       want;
    ArgList          filtered_args;
    Cardinal	     num_filtered_args;

    XmTabBox__inited(st) = False;

    /*
     * Lets start by first making a copy of our TabList.  We do this so
     * that we are not sharing memory with the user.
     */
    XmTabBox_tab_list(st) = XmTabbedStackListCopy(XmTabBox_tab_list(st));

    /*
     * Now lets see if the creator set our font list and if not lets
     * grab the default.
     */
    if( XmTabBox_font_list(st) == NULL )
    {
	XmTabBox_font_list(st) = XmeGetDefaultRenderTable((Widget) st,
						      XmLABEL_FONTLIST);
    }
    XmTabBox_font_list(st) = XmFontListCopy(XmTabBox_font_list(st));


    _XmFilterArgs(arg_list, *arg_cnt, xm_std_filter,
		  &filtered_args, &num_filtered_args);
    XmTabBox__canvas(st) =
	XtCreateManagedWidget("canvas", xmTabCanvasWidgetClass, set,
			      filtered_args, num_filtered_args);

#ifdef SCROLLED_LAYOUT
    XmTabBox__left_arrow(st) =
	XtCreateWidget("leftArrow", xmArrowButtonWidgetClass, set,
		       filtered_args, num_filtered_args);
    XmTabBox__right_arrow(st) =
	XtCreateWidget("rightArrow", xmArrowButtonWidgetClass, set,
		       filtered_args, num_filtered_args);
#endif

    XtFree((XtPointer)filtered_args);

    XmTabBox__bitmap(st) = XmUNSPECIFIED_PIXMAP;
    XmTabBox__bitmap_width(st) = XmTabBox__bitmap_height(st) = 0;
    XmTabBox__zero_GC(st) = NULL;
    XmTabBox__one_GC(st) = NULL;
    XmTabBox__tab_GC(st) = NULL;
    XmTabBox__text_GC(st) = NULL;
    XmTabBox__wanted(st) = (XRectangle*) NULL;
    XmTabBox__num_wanted(st) = 0;
    XmTabBox__actual(st) = (XiTabRect*) NULL;
    XmTabBox__num_actual(st) = 0;
    XmTabBox__scroll_x(st) = 0;
    XmTabBox__selected(st) = -1;
    XmTabBox__keyboard(st) = -1;
    XmTabBox__armed_tab(st) = -1;
    XmTabBox__num_columns(st) = 0;
    XmTabBox__num_rows(st) = 0;
    XmTabBox__gray_stipple(st) = XmGetPixmapByDepth(XtScreen(st),
						   XmS50_foreground, 1, 0, 1);

    XmTabBox__cache(st) = NULL;
    XmTabBox__cache_size(st) = 0;

    CalcCornerSize(st);

    CalcGeometry(st, &want);

    if( XtWidth(request) == 0 )
    {
	set->core.width = want.width;
    }
    if( XtHeight(request) == 0 )
    {
	set->core.height = want.height;
    }

    Resize((Widget) st);

    XmTabBox__inited(st) = True;
}

static void
Realize(Widget widget, XtValueMask *value_mask,
	XSetWindowAttributes *attributes)
{
    XmTabBoxWidget   tb = (XmTabBoxWidget) widget;
    XGCValues        gcValues;
    XFontStruct      *font = NULL;
    XtGCMask         gcMask;

    XtRealizeProc realize;

    _XmProcessLock();
    realize = xmTabBoxWidgetClass->core_class.superclass->core_class.realize;
    _XmProcessUnlock();

    (*realize) (widget, value_mask, attributes);

    /*
     * Now lets create a GC that we will use for drawing.  We create
     * this GC as opposed to share it because we will be changing
     * attributes of it a, what seems like, random.
     */
    XmeRenderTableGetDefaultFont(XmTabBox_font_list(tb), &font);
    gcValues.background = tb->core.background_pixel;
    gcMask = GCBackground;
    if (font) {
        gcValues.font = font->fid;
	gcMask |= GCFont;
    }
    /* CR03128 */
    XmTabBox__tab_GC(tb) = XmTabBox__text_GC(tb) = XtGetGC(widget, gcMask, &gcValues);
}

static void Destroy(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    XmTabbedStackListFree(XmTabBox_tab_list(tab));
    XmFontListFree(XmTabBox_font_list(tab));
    if( ValidPixmap(XmTabBox__bitmap(tab)) )
    {
	XFreePixmap(XtDisplay(tab), XmTabBox__bitmap(tab));
    }
    if( XmTabBox__zero_GC(tab) != NULL )
    {
	XFreeGC(XtDisplay(tab), XmTabBox__zero_GC(tab));
    }
    if( XmTabBox__one_GC(tab) != NULL )
    {
	XFreeGC(XtDisplay(tab), XmTabBox__one_GC(tab));
    }
    if( ValidPixmap(XmTabBox__gray_stipple(tab)) )
    {
	XmDestroyPixmap(XtScreen(tab), XmTabBox__gray_stipple(tab));
    }
    if( XmTabBox__cache(tab) != NULL )
    {
	FreeImageCache(tab);
    }
    /* CR03218 */
    if(XmTabBox__tab_GC(tab)) XtReleaseGC((Widget)tab, XmTabBox__tab_GC(tab));

    XtFree((XtPointer) XmTabBox__actual(tab));
    XtFree((XtPointer) XmTabBox__wanted(tab));
}

static void Resize(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    Layout(tab);

    XtConfigureWidget(XmTabBox__canvas(tab), 0, 0,
		      XtWidth(tab), XtHeight(tab), 0);

    if( XtIsRealized(XmTabBox__canvas(tab)) )
    {
	XClearWindow(XtDisplay(widget), XiCanvas(tab));
	Redisplay(XmTabBox__canvas(tab), NULL, False);
    }
}

static void Redisplay(Widget widget, XEvent *event, Region region)
{
    Widget           parent;
    XmTabBoxWidget   tab = (XmTabBoxWidget) XtParent(widget);
    XmTabbedStackList        list = XmTabBox_tab_list(tab);
    int              count = _XmTabbedStackListCount(list),
                     shadow = tab->manager.shadow_thickness;
    GC               gc = XmTabBox__tab_GC(tab);
    Pixel            pixel= 0 ;
    Pixmap           pixmap= 0 ;
    XGCValues gcValues; /* CR03218 begin */
    Boolean getNewGC = False;
    if(gc)
    {
      XGetGCValues(XtDisplay(widget), gc, GCBackground, &gcValues);
      if (tab->core.background_pixel != gcValues.background)
      {
        XtReleaseGC(widget, gc);
        getNewGC = True;
      }
    }
    else getNewGC = True;
    if (getNewGC)
    {
      XFontStruct *font = NULL;
      XtGCMask    gcMask;
      XmeRenderTableGetDefaultFont(XmTabBox_font_list(tab), &font);
      gcValues.background = tab->core.background_pixel;
      gcMask = GCBackground;
      if (font) {
          gcValues.font = font->fid;
	  gcMask |= GCFont;
      }
      XmTabBox__tab_GC(tab) = XmTabBox__text_GC(tab) = XtGetGC((Widget)tab, gcMask, &gcValues);
    } /* CR03218 end */
    if( XmTabBox__inited(tab) == False ) return;

    /*
     * The first thing we want to do is fill in our background with the
     * same color/pixmap our parent has.  This will give us a "see-through"
     * appearance.
     */
    parent = XtParent(tab);

    /*
     * If our parent happens to be a tab stack then we know that we
     * actually want to look at the background of its parent.
     */
    if( XmIsTabStack(parent) )
    {
	parent = XtParent(parent);
	pixmap = parent->core.background_pixmap;
	pixel = parent->core.background_pixel;

	/*
	 * If we are the grand child of a tab stack then we want to use
	 * the background color of the active child.
	 */
	if( XmIsTabStack(parent) )
	{
	    XmTabStackWidget ts = (XmTabStackWidget) parent;
	    Widget           child;

	    if( (child = ts->tab_stack._active_child) != NULL &&
	        XiChildSpecified(child) )
	    {
		pixel = XmTabStackC_tab_background(child);
		if( ValidPixmap(XmTabStackC_tab_background_pixmap(child)) )
		{
		    pixmap = XmTabStackC_tab_background_pixmap(child);
		}
		else
		{
		    pixmap = XmUNSPECIFIED_PIXMAP;
		}
	    }
	}
    }
    if( ValidPixmap(pixmap) )
    {
	Widget kid;
	int    x = 0, y = 0;

	/*
	 * What we need to do here is find out the offset for the
	 * tile.  To do this we need to walk up our hierarchy until
	 * we come accross our parent.
	 */
	for( kid = widget; kid != parent; kid = XtParent(kid) )
	{
	    x += kid->core.x;
	    y += kid->core.y;
	}
	SetTiledXYGC(XtDisplay(tab), gc, pixmap, -x, -y);
    }
    else
    {
	SetSolidGC(XtDisplay(tab), gc, pixel);
    }
    /*
     * Well if we have a region then lets set our clipping region so we
     * confine our drawing to that area.
     */
    if( region != False )
    {
	XSetRegion(XtDisplay(tab), XmTabBox__tab_GC(tab), region);
	XSetRegion(XtDisplay(tab), tab->manager.bottom_shadow_GC, region);
	XSetRegion(XtDisplay(tab), tab->manager.top_shadow_GC, region);
	XSetRegion(XtDisplay(tab), tab->manager.background_GC, region);
    }
    else
    {
	XSetClipMask(XtDisplay(tab), gc, None);
	XSetClipMask(XtDisplay(tab), tab->manager.bottom_shadow_GC, None);
	XSetClipMask(XtDisplay(tab), tab->manager.top_shadow_GC, None);
	XSetClipMask(XtDisplay(tab), tab->manager.background_GC, None);
    }

    if( XmTabBox_tab_mode(tab) != XmTABS_STACKED &&
        XmTabBox_tab_mode(tab) != XmTABS_STACKED_STATIC )
    {
	if( event == NULL || event->xany.type != Expose )
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc, 0, 0,
			   XtWidth(widget), XtHeight(widget));
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc, event->xexpose.x,
			   event->xexpose.y, event->xexpose.width,
			   event->xexpose.height);
	}
    }

    /*
     * Lets check something real quick first.  If we do not have any
     * tabs then what we want to do is just draw a shadow line as an
     * edge.
     */
    if( count == 0 )
    {
	/*
	 * It seems that we have no tabs so lets draw just the shadow
	 * line with the correct GC in the correct place.
	 */
	switch( XmTabBox_tab_edge(tab) )
	{
	case XmTAB_EDGE_BOTTOM_RIGHT:
	default:
	    /*
	     * Well if appears that we need to draw the shadow line either
	     * on the bottom or the right so lets check our orientation
	     * to decide which one.
	     */
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		/*
		 * Here we need to draw a shadow along the bottom
		 * edge of our window and a beveled corner on the
		 * right side.
		 */
		XFillRectangle(XtDisplay(widget), XtWindow(widget),
			       tab->manager.top_shadow_GC,
			       0, (int)XtHeight(widget) - shadow,
			       XtWidth(widget), shadow);
		XmDrawBevel(XtDisplay(widget), XtWindow(widget),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    (int)XtWidth(widget) - shadow,
			    (int)XtHeight(widget) - shadow,
			    shadow, XmBEVEL_BOTH);
	    }
	    else
	    {
		/*
		 * Here we need to draw a shadow along the right side and
		 * a beveled corner on the bottom.
		 */
		XFillRectangle(XtDisplay(widget), XtWindow(widget),
			       tab->manager.top_shadow_GC,
			       (int)XtWidth(widget) - shadow, 0,
			       shadow, XtHeight(widget));
		XmDrawBevel(XtDisplay(widget), XtWindow(widget),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    (int)XtWidth(widget) - shadow,
			    (int)XtHeight(widget) - shadow,
			    shadow, XmBEVEL_BOTH);

	    }
	    break;
	case XmTAB_EDGE_TOP_LEFT:
	    /*
	     * Well if appears that we need to draw the shadow line either
	     * on the top or the left side so lets check our orientation
	     * to decide which one.
	     */
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		/*
		 * Here we need to draw a line across the top of our
		 * window and a corner in the left.
		 */
		XFillRectangle(XtDisplay(widget), XtWindow(widget),
			       tab->manager.bottom_shadow_GC,
			       0, 0, XtWidth(widget), shadow);
		XmDrawBevel(XtDisplay(widget), XtWindow(widget),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    0, 0, shadow, XmBEVEL_BOTH);
	    }
	    else
	    {
		/*
		 * Here we need a shadow along our windows left edge and
		 * a coner towards the top.
		 */
		XFillRectangle(XtDisplay(widget), XtWindow(widget),
			       tab->manager.bottom_shadow_GC,
			       0, 0, shadow, XtHeight(widget));
		XmDrawBevel(XtDisplay(widget), XtWindow(widget),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    0, 0, shadow, XmBEVEL_BOTH);
	    }
	    break;
	}

	if( region != False )
	{
	    XSetClipMask(XtDisplay(tab), XmTabBox__tab_GC(tab), None);
	    XSetClipMask(XtDisplay(tab), tab->manager.bottom_shadow_GC, None);
	    XSetClipMask(XtDisplay(tab), tab->manager.top_shadow_GC, None);
	    XSetClipMask(XtDisplay(tab), tab->manager.background_GC, None);
	}
	return;
    }

    /*
     * Well if we have a region then lets set our clipping region so we
     * confine our drawing to that area.
     */
    if( region != False )
    {
	XSetRegion(XtDisplay(tab), XmTabBox__tab_GC(tab), region);
	XSetRegion(XtDisplay(tab), tab->manager.bottom_shadow_GC, region);
	XSetRegion(XtDisplay(tab), tab->manager.top_shadow_GC, region);
	XSetRegion(XtDisplay(tab), tab->manager.background_GC, region);
    }

    /*
     * Call this handy dandy tab redisplay routine. This is kindof the
     * equivalent to XmRedisplayGadgets.
     */
    RedisplayTabs(tab, region);

    /*
     * Now lets do the orientation/mode dependant redisplay part.
     */
    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	switch( XmTabBox_tab_mode(tab) )
	{
	case XmTABS_BASIC:
	    HorizontalBasicRedisplay(tab);
	    break;
	case XmTABS_STACKED:
	case XmTABS_STACKED_STATIC:
	    HorizontalStackedRedisplay(tab);
	    break;
	case XmTABS_OVERLAYED:
	    break;
	case XmTABS_SCROLLED:
	    break;
	}
    }
    else
    {
	switch( XmTabBox_tab_mode(tab) )
	{
	case XmTABS_BASIC:
	    VerticalBasicRedisplay(tab);
	    break;
	case XmTABS_STACKED:
	case XmTABS_STACKED_STATIC:
	    VerticalStackedRedisplay(tab);
	    break;
	case XmTABS_OVERLAYED:
	    break;
	case XmTABS_SCROLLED:
	    break;
	}
    }

    /*
     * If we set a clipping are, be sure to unset it since we are probably
     * using shared GCs.
     */
    if( region != False )
    {
	XSetClipMask(XtDisplay(tab), XmTabBox__tab_GC(tab), None);
	XSetClipMask(XtDisplay(tab), tab->manager.bottom_shadow_GC, None);
	XSetClipMask(XtDisplay(tab), tab->manager.top_shadow_GC, None);
	XSetClipMask(XtDisplay(tab), tab->manager.background_GC, None);
    }
}

#define cfield(f) XmTabBox_##f(c_tab)
#define rfield(f) XmTabBox_##f(r_tab)
#define sfield(f) XmTabBox_##f(s_tab)

static Boolean
SetValues(Widget current, Widget request, Widget set, ArgList arg_list,
	  Cardinal *arg_cnt)
{
    XmTabBoxWidget c_tab = (XmTabBoxWidget) current,
                   s_tab = (XmTabBoxWidget) set;

    Boolean        need_layout = False, need_resize = False,
                   clear_cache = False;
    ArgList        filtered_args = NULL;
    Cardinal       num_filtered_args = 0;

    /*
     * First lets start by passing down our resources to our children.
     */
    if( XmTabBox__inited(s_tab) && XmTabBox__canvas(s_tab) != NULL )
    {
	_XmFilterArgs(arg_list, *arg_cnt, xm_std_filter,
		      &filtered_args, &num_filtered_args);
	XtSetValues(XmTabBox__canvas(s_tab), filtered_args, num_filtered_args);
#ifdef SCROLLED_LAYOUT
	XtSetValues(XmTabBox__left_arrow(s_tab), filtered_args,
		    num_filtered_args);
	XtSetValues(XmTabBox__right_arrow(s_tab), filtered_args,
		    num_filtered_args);
#endif
	XtFree((XtPointer)filtered_args);
    }

    if( XmTabBox_tab_list(c_tab) != XmTabBox_tab_list(s_tab) )
    {
	if( XmTabbedStackListCompare(XmTabBox_tab_list(s_tab),
			     XmTabBox_tab_list(c_tab)) == XmTAB_CMP_SIZE )
	{
	    clear_cache = True;
	    need_resize = True;
	}

	XmTabbedStackListFree(XmTabBox_tab_list(c_tab));
	XmTabBox_tab_list(c_tab) = (XmTabbedStackList) NULL;
	XmTabBox_tab_list(s_tab) = XmTabbedStackListCopy(XmTabBox_tab_list(s_tab));

	CalcTabGeometry(s_tab);
	need_layout = True;
    }

    if( sfield(_selected) > _XmTabbedStackListCount(sfield(tab_list)) )
    {
	sfield(_selected) = -1;
	if( _XmTabbedStackListCount(sfield(tab_list)) > 0 )
	{
	    sfield(_selected) = 0;
	}
	need_layout = True;
    }

    if( sfield(_keyboard) > _XmTabbedStackListCount(sfield(tab_list)) )
    {
	sfield(_keyboard) = -1;
	if( _XmTabbedStackListCount(sfield(tab_list)) > 0 )
	{
	    sfield(_keyboard) = 0;
	}
	need_layout = True;
    }

    if( sfield(selected_index) != -1 || sfield(traversal_index) != -1 )
    {
	sfield(_selected) = sfield(selected_index);
	sfield(_keyboard) = sfield(traversal_index);

	sfield(selected_index) = -1;
	sfield(traversal_index) = -1;

	need_layout = True;
    }

    if( cfield(font_list) != sfield(font_list) )
    {
	XmFontListFree(cfield(font_list));
	cfield(font_list) = (XmFontList) NULL;
	if( sfield(font_list) == NULL )
	{
	    sfield(font_list) = XmeGetDefaultRenderTable(set, XmLABEL_FONTLIST);
	}
	sfield(font_list) == XmFontListCopy(sfield(font_list));
	need_layout = True;
	need_resize = True;
    }

    if( c_tab->manager.shadow_thickness != s_tab->manager.shadow_thickness ||
        cfield(tab_style) != sfield(tab_style) ||
        cfield(tab_mode) != sfield(tab_mode) ||
        cfield(tab_orientation) != sfield(tab_orientation) ||
        cfield(orientation) != sfield(orientation) ||
        cfield(uniform_tab_size) != sfield(uniform_tab_size) ||
        cfield(tab_margin_width) != sfield(tab_margin_width) ||
        cfield(tab_margin_height) != sfield(tab_margin_height) ||
        cfield(tab_label_spacing) != sfield(tab_label_spacing) ||
        cfield(tab_corner_percent) != sfield(tab_corner_percent) ||
#ifdef SCROLLED_LAYOUT
        cfield(arrow_placement) != sfield(arrow_placement) ||
#endif
        cfield(tab_edge) != sfield(tab_edge) ||
        cfield(tab_offset) != sfield(tab_offset) ||
        cfield(highlight_thickness) != sfield(highlight_thickness) )
    {
	need_layout = True;
	need_resize = True;
    }

    if( cfield(tab_orientation) != sfield(tab_orientation) )
    {
	clear_cache = True;
    }

    if( cfield(use_image_cache) != sfield(use_image_cache) )
    {
	if( sfield(use_image_cache) )
	{
	    clear_cache = True;
	}
	else
	{
	    FreeImageCache(s_tab);
	    clear_cache = False;
	}
    }
    if( clear_cache ) ResetImageCache(s_tab);

    if( need_layout )
    {
	Resize(set);
    }

    /*
     * Now lets check to see if we are supposed to pick a new size, and
     * if we are lets find out that size.
     */
    if( need_resize )
    {
	XRectangle geom;

	CalcGeometry(s_tab, &geom);

	/*
	 * Now that we know the size that we want to be lets set the
	 * width and height for ourselves so the geometry can be
	 * passed up to our parent.  If that size is accepted we
	 * should get a resize callback.
	 */
        if( XtWidth(request) == XtWidth(current) )
	{
	    s_tab->core.width = geom.width;
	}
	if( XtHeight(request) == XtHeight(current) )
	{
	    s_tab->core.height = geom.height;
	}
    }

    return( need_layout || need_resize );
}
#undef cfield
#undef rfield
#undef sfield

static XtGeometryResult
QueryGeometry(Widget widget, XtWidgetGeometry *request,
	      XtWidgetGeometry *allowed)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;
    XRectangle     rect;

    /*
     * Lets start with the easy case.  If they gave us nothing then
     * lets return to them the geometry that we really want.
     */
    if( request == NULL || request->request_mode == 0 )
    {
	CalcGeometry(tab, &rect);

	allowed->request_mode = CWWidth | CWHeight;
	allowed->width = rect.width;
	allowed->height = rect.height;

	return( XtGeometryAlmost );
    }

    /*
     * If we got here that means that they are asking us something so
     * lets see what they want.  We will first check if they are
     * inquering about something other then width and height, because
     * we only care about width and height.  If they give us something
     * else we will gladly accept it.
     */
    if( !(request->request_mode & (CWWidth | CWHeight)) )
    {
	/*
	 * Ok it appears that they only care about attributes that we
	 * do not care about so lets just return them a big YES.
	 */
	return( XtGeometryYes );
    }

    /*
     * Now comes the big work.  It appears that they gave us a partial
     * or suggested geometry and they want to know how we will react to
     * that geometry.  Since we have an orientation we will take our
     * major dimension and give that priority.
     */
    *allowed = *request;
    allowed->request_mode |= (CWWidth | CWHeight);
    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	if( request->request_mode & CWWidth )
	{
	    /*
	     * They gave us a width so lets take that width and figure
	     * out our minor dimension using that width.
	     */
	    allowed->height = CalcGeometryMinor(tab, (int)request->width);
	}
	else if( request->request_mode & CWHeight )
	{
	    /*
	     * Well it looks like they gave us the minor geometry they
	     * want us to fit into so lets calculate the major.
	     */
	    allowed->width = CalcGeometryMinor(tab, (int)request->height);
	}
    }
    else
    {
	if( request->request_mode & CWHeight )
	{
	    /*
	     * They gave us a height so lets take that height and figure
	     * out our minor dimension using that height.
	     */
	    allowed->width = CalcGeometryMinor(tab, (int)request->height);
	}
	else if( request->request_mode & CWWidth )
	{
	    /*
	     * Well it looks like they gave us the minor geometry they
	     * want us to fit into so lets calculate the major.
	     */
	    allowed->height = CalcGeometryMajor(tab, request->width);
	}
    }

    if( XmCompareXtWidgetGeometryToWidget(allowed, widget) )
    {
	return( XtGeometryNo );
    }
    if( XmCompareXtWidgetGeometry(request, allowed) ) return( XtGeometryYes );
    return( XtGeometryAlmost );
}

#define XmTAB_HIGHLIGHT_RECT 0
#define XmTAB_TEXT_RECT 1

static XRectangle*
GetTabRectangle(XmTabBoxWidget tab, int type, XiTabRect *draw)
{
    static XRectangle rect;
    int               highlight = XmTabBox_highlight_thickness(tab),
                      shadow = tab->manager.shadow_thickness,
                      margin_height = XmTabBox_tab_margin_height(tab),
                      margin_width = XmTabBox_tab_margin_width(tab),
                      spacing = XmTabBox_tab_label_spacing(tab),
                      size, horiz, vert;

    if( draw == NULL )
    {
	if( XmTabBox__keyboard(tab) >= 0 )
	{
	    draw = &(XmTabBox__actual(tab)[XmTabBox__keyboard(tab)]);
	}
	else
	{
	    rect.x = rect.y = 0;
	    rect.width = rect.height = 0;
	}
    }

    size = XmTabBox__corner_size(tab);
    if( draw->width < draw->height )
    {
	AssignMin(size,(int)(draw->width/2));
    }
    else
    {
	AssignMin(size,(int)draw->height/2);
    }

    margin_width += shadow;
    margin_height += shadow;
    if( type == XmTAB_HIGHLIGHT_RECT )
    {
	horiz = Max(size, margin_width);
	vert = Max(size, margin_height);
    }
    else
    {
	horiz = Max(size, margin_width) + highlight + spacing;
	vert = Max(size, margin_height) + highlight + spacing;
    }

    switch( XmTabBox_tab_orientation(tab) )
    {
    case XmTABS_LEFT_TO_RIGHT:
    case XmTABS_RIGHT_TO_LEFT:
    default:
	break;
    case XmTABS_TOP_TO_BOTTOM:
    case XmTABS_BOTTOM_TO_TOP:
        {
	    int tmp = horiz;
	    horiz = vert;
	    vert = tmp;
	}
	break;
    }

    rect.x = draw->x + horiz;
    rect.y = draw->y + vert;
    rect.width = draw->width - (2 * horiz);
    rect.height = draw->height - (2 * vert);

    return( &rect );
}

static void
DrawBorder(XmTabBoxWidget tab, GC gc, int idx)
{
    int             highlight = XmTabBox_highlight_thickness(tab);
    XiTabRect       *geometry;
    XRectangle      *draw, rect[4];
    XmTabAttributes info;

    if( idx < 0 )
    {
	if( XmTabBox__keyboard(tab) < 0 ) return;
	geometry = &(XmTabBox__actual(tab)[XmTabBox__keyboard(tab)]);
	idx = XmTabBox__keyboard(tab);
    }
    else
    {
	geometry = &(XmTabBox__actual(tab)[idx]);
    }
    if( gc == tab->manager.background_GC &&
        (((info = _XmTabbedStackListGet(XmTabBox_tab_list(tab), idx)) != NULL &&
	  XiBackgroundSpecified(info)) ||
	 (idx == XmTabBox__selected(tab) && XiSelectSpecified(tab))) )
    {
	gc = XmTabBox__tab_GC(tab);

	if( idx == XmTabBox__selected(tab) && XiSelectSpecified(tab) )
	{
	    SetSelectGC(tab, gc);
	}
	else
	{
	    SetBackgroundGC(tab, info, gc);
	}
    }

    draw = GetTabRectangle(tab, XmTAB_HIGHLIGHT_RECT, geometry);

    rect[0].x = draw->x;
    rect[0].y = draw->y;
    rect[0].width = Max((int)draw->width, 1);
    rect[0].height = highlight;

    rect[1].x = rect[0].x;
    rect[1].y = rect[0].y;
    rect[1].width = highlight;
    rect[1].height = Max((int)draw->height, 1);

    rect[2].x = rect[0].x + (int)draw->width - highlight;
    rect[2].y = rect[0].y;
    rect[2].width = highlight;
    rect[2].height = Max((int)draw->height, 1);

    rect[3].x = rect[0].x;
    rect[3].y = rect[0].y + (int)draw->height - highlight;
    rect[3].width = Max((int)draw->width, 1);
    rect[3].height = highlight;

    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, 4);
}

static void
BorderHighlight(Widget widget)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    DrawBorder(tab, tab->manager.highlight_GC, -1);
}

static void
BorderUnhighlight(Widget widget)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    DrawBorder(tab, tab->manager.background_GC, -1);
}

/*
 * Function:
 *	XmTabBoxArmAndActivate(widget, event, params, num_params)
 * Description:
 *	This action is activate when keyboard traversal is used to
 *	select a tab.  This action selects the tabs and calls the
 *	unselect and select callbacks.
 * Input:
 *	widget     : Widget    - the XiTabCanvas
 *	event      : XEvent*   - unused
 *	params     : String*   - unused
 *	num_params : Cardinal* - unused
 * Output:
 *	None.
 */
static void
XmTabBoxArmAndActivate(Widget widget, XEvent *event, String *params,
		       Cardinal *num_params)
{
    XmTabBoxWidget         tab = XiTabParent(widget);
    int                    old = XmTabBox__selected(tab),
                           idx = XmTabBox__keyboard(tab);

    SelectTab(tab, event, old, idx);
}

static void
XmTabBoxArmTab(Widget widget, XEvent *event, String *params,
	       Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            old, idx;

    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);

    if( event == NULL || event->xany.type != ButtonPress ) return;

    idx = XmTabBox__armed_tab(tab) = XiXYtoTab(tab, event->xbutton.x,
					      event->xbutton.y);

    if( !IsTabSensitive(tab, idx) ) return;

    XmTabBox__armed_tab(tab) = idx;

    old = XmTabBox__keyboard(tab);
    if( idx != -1 && idx != old )
    {
	DrawBorder(tab, tab->manager.background_GC, old);
	DrawBorder(tab, tab->manager.highlight_GC, idx);
	XmTabBox__keyboard(tab) = idx;
    }
}

/*
 * Function:
 *	XmTabBoxSelected(widget, event, params, num_params)
 * Description:
 *	This action selects a tab and calls the select and unselect
 *	callbacks.
 * Input:
 *	widget     : Widget    - the XiTabCanvas
 *	event      : XEvent*   - the event that triggered the action
 *	params     : String*   - unused
 *	num_params : Cardinal* - unused
 * Output:
 *	None.
 */
static void
XmTabBoxSelectTab(Widget widget, XEvent *event, String *params,
		  Cardinal *num_params)
{
    XmTabBoxWidget         tab = XiTabParent(widget);
    int                    idx;

    /*
     * First lets do a quick check to make sure that we have an event we
     * can deal with and that we have an armed tab.
     */
    if( event == NULL || event->xany.type != ButtonRelease ||
        XmTabBox__armed_tab(tab) == -1 ) return;

    /*
     * Lets find out which tab was activated by converting the event
     * x, y to a tab index.
     */
    idx = XiXYtoTab(tab, event->xbutton.x, event->xbutton.y);

    /*
     * First lets make sure that we are trying to select the right tab
     * we do this my making sure it is not already selected and by
     * making sure it is the currently armed tab.
     */
    if( idx == XmTabBox__selected(tab) || idx != XmTabBox__armed_tab(tab) ||
        !IsTabSensitive(tab, idx) )
    {
	XmTabBox__armed_tab(tab) = -1;
	return;
    }
    XmTabBox__armed_tab(tab) = -1;
    SelectTab(tab, event, XmTabBox__selected(tab), idx);

}

static void
_XmTabBoxTraverseRight(Widget widget, XEvent *event, String *params,
		       Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            old, old_selected, set, col;

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_SCROLLED:
    case XmTABS_OVERLAYED:
    default:
	XmTabBoxTraverseNext(widget, event, params, num_params);
	return;
	break;
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	old = XmTabBox__keyboard(tab);
	col = XmTabBox__actual(tab)[old].column;
	do
	{
	    set = GetTabIndex(tab, XmTabBox__actual(tab)[old].row, ++col);
	    if( set < 0 ) return;
	} while( !IsTabSensitive(tab, set) );

	if( set < 0 || set == old ) return;

	old_selected = XmTabBox__selected(tab);

	XmTabBox__keyboard(tab) = set;
	DrawBorder(tab, tab->manager.background_GC, old);

	if( XmTabBox_tab_auto_select(tab) )
	{
	    SelectTab(tab, event, old_selected, set);
	}
	else
	{
	    DrawBorder(tab, tab->manager.highlight_GC, set);
	}

	break;
    }
}

static void
XmTabBoxTraverseRight(Widget widget, XEvent *event, String *params,
		      Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    if( XmTabBox_orientation(tab) == XmVERTICAL )
    {
	_XmTabBoxTraverseDown(widget, event, params, num_params);
    }
    else
    {
	_XmTabBoxTraverseRight(widget, event, params, num_params);
    }
}

static void
_XmTabBoxTraverseLeft(Widget widget, XEvent *event, String *params,
		      Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            old, old_selected, set, col;

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_SCROLLED:
    case XmTABS_OVERLAYED:
    default:
	XmTabBoxTraversePrevious(widget, event, params, num_params);
	return;
	break;
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	old = XmTabBox__keyboard(tab);
	col = XmTabBox__actual(tab)[old].column;
	do
	{
	    set = GetTabIndex(tab, XmTabBox__actual(tab)[old].row, --col);
	    if( set < 0 ) return;
	} while( !IsTabSensitive(tab, set) );

	if( set < 0 || set == old ) return;

	old_selected = XmTabBox__selected(tab);
	XmTabBox__keyboard(tab) = set;
	DrawBorder(tab, tab->manager.background_GC, old);

	if( XmTabBox_tab_auto_select(tab) )
	{
	    SelectTab(tab, event, old_selected, set);
	}
	else
	{
	    DrawBorder(tab, tab->manager.highlight_GC, set);
	}
	break;
    }
}

static void
XmTabBoxTraverseLeft(Widget widget, XEvent *event, String *params,
		     Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    if( XmTabBox_orientation(tab) == XmVERTICAL )
    {
	_XmTabBoxTraverseUp(widget, event, params, num_params);
    }
    else
    {
	_XmTabBoxTraverseLeft(widget, event, params, num_params);
    }
}

static void
_XmTabBoxTraverseUp(Widget widget, XEvent *event, String *params,
		    Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            old, old_selected, set, old_row, new_row;

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_SCROLLED:
    case XmTABS_OVERLAYED:
    default:
	XmTabBoxTraversePrevious(widget, event, params, num_params);
	return;
	break;
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	old = XmTabBox__keyboard(tab);
	new_row = old_row = XmTabBox__actual(tab)[old].row;
	do
	{
	    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	    {
		do
		{
		    new_row++;
		    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED &&
		       XmTabBox_tab_auto_select(tab) )
		    {
			if( new_row >= XmTabBox__num_rows(tab) )
			{
			    new_row = 0;
			}
		    }
		    set = GetTabIndex(tab, new_row,
				      XmTabBox__actual(tab)[old].column);
		} while( set >= 0 &&
			!IsTabSensitive(tab, set) && new_row != old_row );
	    }
	    else
	    {
		do
		{
		    new_row--;
		    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED &&
		       XmTabBox_tab_auto_select(tab) )
		    {
			if( new_row < 0 )
			{
			    new_row = XmTabBox__num_rows(tab) - 1;
			}
		    }
		    set = GetTabIndex(tab, new_row,
				      XmTabBox__actual(tab)[old].column);
		} while( set >= 0 &&
			!IsTabSensitive(tab, set) && new_row != old_row );
	    }
	    set = GetTabIndex(tab, new_row,
			      XmTabBox__actual(tab)[old].column);
	} while( new_row >= 0 && new_row < XmTabBox__num_rows(tab) &&
		set < 0 );

	if( set < 0 || set == old || new_row == old_row ) return;

	old_selected = XmTabBox__selected(tab);

	XmTabBox__keyboard(tab) = set;
	DrawBorder(tab, tab->manager.background_GC, old);

	if( XmTabBox_tab_auto_select(tab) )
	{
	    SelectTab(tab, event, old_selected, set);
	}
	else
	{
	    DrawBorder(tab, tab->manager.highlight_GC, set);
	}
	break;
    }
}

static void
XmTabBoxTraverseUp(Widget widget, XEvent *event, String *params,
		   Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    if( XmTabBox_orientation(tab) == XmVERTICAL )
    {
	_XmTabBoxTraverseLeft(widget, event, params, num_params);
    }
    else
    {
	_XmTabBoxTraverseUp(widget, event, params, num_params);
    }
}

static void
_XmTabBoxTraverseDown(Widget widget, XEvent *event, String *params,
		      Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            old, old_selected, set, new_row, old_row;

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_SCROLLED:
    case XmTABS_OVERLAYED:
    default:
	XmTabBoxTraverseNext(widget, event, params, num_params);
	return;
	break;
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	old = XmTabBox__keyboard(tab);
	new_row = old_row = XmTabBox__actual(tab)[old].row ;

	do
	{
	    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	    {
		do
		{
		    new_row--;
		    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED &&
		        XmTabBox_tab_auto_select(tab) )
		    {
			if( new_row < 0 )
			{
			    new_row = XmTabBox__num_rows(tab) - 1;
			}
		    }
		    set = GetTabIndex(tab, new_row,
				      XmTabBox__actual(tab)[old].column);
		} while( set >= 0 &&
			 !IsTabSensitive(tab, set) && new_row != old_row );
	    }
	    else
	    {
		do
		{
		    new_row++;
		    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED &&
		        XmTabBox_tab_auto_select(tab) )
		    {
			if( new_row >= XmTabBox__num_rows(tab) )
			{
			    new_row = 0;
			}
		    }
		    set = GetTabIndex(tab, new_row,
				      XmTabBox__actual(tab)[old].column);
		} while( set >= 0 &&
			 !IsTabSensitive(tab, set) && new_row != old_row );
	    }
	    set = GetTabIndex(tab, new_row, XmTabBox__actual(tab)[old].column);
	} while( new_row >= 0 && new_row < XmTabBox__num_rows(tab) && set < 0 );

	if( set < 0 || set == old || new_row == old_row ) return;

	old_selected = XmTabBox__selected(tab);

	XmTabBox__keyboard(tab) = set;
	DrawBorder(tab, tab->manager.background_GC, old);

	if( XmTabBox_tab_auto_select(tab) )
	{
	    SelectTab(tab, event, old_selected, set);
	}
	else
	{
	    DrawBorder(tab, tab->manager.highlight_GC, set);
	}
	break;
    }
}

static void
XmTabBoxTraverseDown(Widget widget, XEvent *event, String *params,
		     Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);

    if( XmTabBox_orientation(tab) == XmVERTICAL )
    {
	_XmTabBoxTraverseRight(widget, event, params, num_params);
    }
    else
    {
	_XmTabBoxTraverseDown(widget, event, params, num_params);
    }
}

static void
XmTabBoxTraversePrevious(Widget widget, XEvent *event, String *params,
			 Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), idx, old, tmp,
                   old_selected;

    if( cnt == 0 ) return;

    old = XmTabBox__keyboard(tab);
    old_selected = XmTabBox__selected(tab);

    idx = old;
    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
        XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC )
    {
	do {
	    tmp = idx;
	    idx = tmp - 1;
	    if( idx < 0 ) idx = cnt - 1;
	    if( idx == tmp ) return;

	    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
	       XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC )
	    {
		if( XmTabBox__actual(tab)[idx].row !=
		    XmTabBox__actual(tab)[tmp].row &&
		    XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
		{
		    int row = XmTabBox__actual(tab)[tmp].row + 1, col;
		    if ( row >= XmTabBox__num_rows(tab) ) row = 0;
		    for( col = XmTabBox__num_columns(tab) - 1;
			col > 0 && (idx = GetTabIndex(tab, row, col)) < 0;
			--col );
		}
	    }
	} while( !IsTabSensitive(tab, idx) && idx != old );
    }
    else
    {
	do {
	    idx = idx - 1;
	    if( idx < 0 ) idx = cnt - 1;
	} while( !IsTabSensitive(tab, idx) && idx != old );
    }

    if( idx < 0 || idx == old ) return;

    DrawBorder(tab, tab->manager.background_GC, old);

    XmTabBox__keyboard(tab) = idx;
    if( XmTabBox_tab_auto_select(tab) )
    {
	SelectTab(tab, event, old_selected, idx);
    }
    else
    {
	DrawBorder(tab, tab->manager.highlight_GC, idx);
    }
}

static void
XmTabBoxTraverseNext(Widget widget, XEvent *event, String *params,
		     Cardinal *num_params)
{
    XmTabBoxWidget tab = XiTabParent(widget);
    int            cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), idx, old, tmp,
                   old_selected;

    if( cnt == 0 ) return;

    old = XmTabBox__keyboard(tab);
    old_selected = XmTabBox__selected(tab);

    idx = old;
    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
        XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC )
    {
	do {
	    tmp = idx;
	    idx = (idx + 1) % cnt;
	    if( XmTabBox__actual(tab)[idx].row !=
	        XmTabBox__actual(tab)[tmp].row &&
	        XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	    {
		int row = XmTabBox__actual(tab)[tmp].row - 1;
		if( row < 0 ) row = XmTabBox__num_rows(tab) - 1;
		idx = GetTabIndex(tab, row, 0);
	    }
	} while( !IsTabSensitive(tab, idx) && idx != old );
    }
    else
    {
	do {
	    idx = (idx + 1) % cnt;
	} while( !IsTabSensitive(tab, idx) && idx != old );
    }

    if( idx < 0 || idx == old ) return;

    DrawBorder(tab, tab->manager.background_GC, old);
    XmTabBox__keyboard(tab) = idx;

    if( XmTabBox_tab_auto_select(tab) )
    {
	SelectTab(tab, event, old_selected, idx);
    }
    else
    {
	DrawBorder(tab, tab->manager.highlight_GC, idx);
    }
}

static void
CalcTabSize(XmTabBoxWidget tab, XmTabAttributes info,
	    XmTabOrientation orientation, XmFontList font_list,
	    int shadow_thickness, int highlight_thickness,
	    int margin_width, int margin_height,
	    int spacing, int corner_size,
	    Dimension *width, Dimension *height)
{
    Boolean   have_label = False, have_pixmap = False;
    Dimension _width = 0, _height = 0;

    /*
     * First lets take the quick out if we were not given a valid
     * tab.
     */
    if( info == NULL )
    {
	*width = *height = 0;
	return;
    }

    /*
     * The size of a tab is determined by the combined geometry of ...
     *
     * 		- Label (if visible)
     *		- Pixmap (if visible)
     *		- highlight border (width/height)
     *		- margins (width/height)
     *		- shadows (width/height)
     *		- corner size
     *
     */


    /*
     * Lets first see if we have a visible label string and if so lets
     * add it to the size of the tab.
     */
    if( info->label_string != NULL &&
        info->pixmap_placement != XmPIXMAP_ONLY )
    {
	Dimension string_width = 0, string_height = 0;

	/*
	 * We have a label String so lets find out its dimensions and
	 * add them to the size of the tab.
	 */
	XmStringExtent(font_list, info->label_string,
		       &string_width, &string_height);

	_width += string_width;
	AssignMax(_height, string_height);
	have_label = True;
    }

    /*
     * Now lets check to see if we have to deal with a pixmap, and if
     * so lets do that.
     */
    if( ValidPixmap(info->label_pixmap) &&
        info->pixmap_placement != XmPIXMAP_NONE )
    {
	Window       window_unused;
	int          int_unused;
	unsigned int uint_unused, width_return, height_return;

	/*
	 * We have a pixmap, so lets see what the size of this pixmap
	 * is.
	 */
	XGetGeometry(XtDisplay(tab), info->label_pixmap, &window_unused,
		     &int_unused, &int_unused, &width_return, &height_return,
		     &uint_unused, &uint_unused);

	/*
	 * We will add the geometry for the pixmap to the overall geometry
	 * based on the placement of the pixmap in relation to the
	 * label string.
	 */
	switch( info->pixmap_placement )
	{
	case XmPIXMAP_TOP:
	case XmPIXMAP_BOTTOM:
	    _height += height_return;
	    AssignMax(_width, width_return);
	    break;
	case XmPIXMAP_RIGHT:
	case XmPIXMAP_LEFT:
	case XmPIXMAP_ONLY:
	    _width += width_return;
	    AssignMax(_height, height_return);
	    break;
	case XmPIXMAP_NONE:
	default:
	    /* notreached */
	    break;
	}
	have_pixmap = True;
    }

    /*
     * If we have both a label and a pixmap we need to accomidate some
     * spacing between them so lets check if we have them and add the
     * space to the appropriate place.
     */
    if( have_label && have_pixmap )
    {
	switch( info->pixmap_placement )
	{
	case XmPIXMAP_TOP:
	case XmPIXMAP_BOTTOM:
	    _height += spacing;
	    break;
	case XmPIXMAP_LEFT:
	case XmPIXMAP_RIGHT:
	    _width += spacing;
	    break;
	case XmPIXMAP_NONE:
	case XmPIXMAP_ONLY:
	default:
	    /* notreached */
	    break;
	}
    }

    /*
     * Lets add shadow thickness to our margin, because we are going to
     * consider both of them to see if we should use them or the
     * corner size.
     */
    margin_width += shadow_thickness;
    margin_height += shadow_thickness;

    /*
     * Now lets add room for the traversal highlight and margins
     * around the label/pixmap.
     */
    AssignMax(margin_width, corner_size);
    AssignMax(margin_height, corner_size);
    _height += 2 * (highlight_thickness + margin_height + spacing);
    _width += 2 * (highlight_thickness + margin_width + spacing);

    /*
     * Now lets set the result depending on the orientation of the
     * tab.
     */
    switch( orientation )
    {
    case XmTABS_TOP_TO_BOTTOM:
    case XmTABS_BOTTOM_TO_TOP:
	*width = _height;
	*height = _width;
	break;
    default:
	*width = _width;
	*height = _height;
	break;
    }
}

static void
CalcUnlimitedGeometry(XmTabBoxWidget tab, XRectangle *geometry)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    XmTabAttributes info;
    int             i, count = _XmTabbedStackListCount(list),
                    width, height, max_width, max_height,
                    tab_width, tab_height;
    Boolean         uniform_size;

    /*
     * Start by initializing the geometry variables.  Although we are
     * calling these things width and height the may be revered depending
     * on the orientation of this thing.
     */
    width = height = max_width = max_height = 0;

    /*
     * Now lets cashe some values so we don't have to look them up
     * or calculate them.
     */
    uniform_size = XmTabBox_uniform_tab_size(tab);

    for( i = 0; i < count; ++i )
    {
	Dimension d_width = 0, d_height = 0;
	/*
	 * First the easy part, lets grabs tab and calculate its size.
	 */
	info = _XmTabbedStackListGet(list, i);
	XiCalcTabSize(tab, info, &d_width, &d_height);
	tab_width = (int) d_width;
	tab_height = (int) d_height;

	/*
	 * Now that we know the size of this tab, we need to check what
	 * that means.  Because if we are going to a uniform look part
	 * of that geometry may be droped.
	 */
	if( uniform_size )
	{
	    /*
	     * If we are calculating for unform sized tabs then lets
	     * just capture the max width and height of all the tabs
	     * to use in calculations later.
	     */
	    AssignMax(max_width, tab_width);
	    AssignMax(max_height, tab_height);
	}
	else
	{
	    /*
	     * If we are not doing uniform tabs that means that the
	     * major dimension of the tabs can vary.
	     */
	    XmTabBox__wanted(tab)[i].width = tab_width;
	    XmTabBox__wanted(tab)[i].height = tab_height;

	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		width += tab_width;
		AssignMax(height, tab_height);
	    }
	    else
	    {
		height += tab_height;
		AssignMax(width, tab_width);
	    }
	}
    }

    /*
     * Now the big check.  If we are dealing with uniform size tabs
     * then we want to calculate the width/height that we need based
     * on the max_width/max_height of all the tabs.
     */
    if( uniform_size )
    {
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    width = count * max_width;
	    height = max_height;
	}
	else
	{
	    height = count * max_height;
	    width = max_width;
	}

	for( i = 0; i < count; ++i )
	{
	    XmTabBox__wanted(tab)[i].width = max_width;
	    XmTabBox__wanted(tab)[i].height = max_height;
	}
    }

    geometry->width = width;
    geometry->height = height;
}

/*
 * Function:
 *	CalcGeometry(tab, geometry)
 * Description:
 *	Calculates the desired geometry given no size constraints.
 * Input:
 *	tab      : XmTabBoxWidget - the tab box to work with
 *	geometry : XRectangle*    - returns the desired geometry
 * Output:
 *	None.
 */
static void
CalcGeometry(XmTabBoxWidget tab, XRectangle *geometry)
{
    int                    count = _XmTabbedStackListCount(XmTabBox_tab_list(tab));

    /*
     * Lets start with the easy case.  This is the case where we do not
     * have any tabs.  In this case our desired size is simple the
     * room we need to draw single line shadow.
     */
    if( count == 0 )
    {
	/*
	 * We have no tabs so the size we want is twice our shadow thinkness
	 * in our major dimension and our shadow thickness in our minor
	 * dimension.
	 */
	geometry->x = geometry->y = 0;
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    geometry->width = 2 * tab->manager.shadow_thickness;
	    geometry->height = tab->manager.shadow_thickness;
	}
	else
	{
	    geometry->width = tab->manager.shadow_thickness;
	    geometry->height = 2 * tab->manager.shadow_thickness;
	}

	/*
	 * One thing before we leave.  If we have no tabs lets make
	 * sure to "zero" out the index to the tab with keyboard
	 * traversal and selected.
	 */
	XmTabBox__keyboard(tab) = -1;
	XmTabBox__selected(tab) = -1;

	/*
	 * Since this is the easy case we are done, so lets just hop
	 * out of this routine.
	 */
	return;
    }

    /*
     * The first thing we will do is calculate the size of each of the
     * tabs.  We do this via this handy dandy Tab Calculation routine.
     */
    CalcTabGeometry(tab);

    /*
     * Now lets make sure that we have a tab marked for both keyboard
     * traversal and selected.
     */
    if( XmTabBox__keyboard(tab) < 0 ) XmTabBox__keyboard(tab) = 0;
    if( XmTabBox__selected(tab) < 0 ) XmTabBox__selected(tab) = 0;

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	CalcStackedGeometry(tab, geometry);
	break;
    case XmTABS_BASIC:
    case XmTABS_SCROLLED:
    case XmTABS_OVERLAYED:
    default:
	CalcUnlimitedGeometry(tab, geometry);
	break;
    }

    if( geometry->width == 0 ) geometry->width = 20;
    if( geometry->height == 0 ) geometry->height = 20;

}

static void
DrawSegments(XmTabBoxWidget tab, XmTabAttributes info, XiTabRect *geometry,
	     XmTabEdge edge, int corner_size, int shadow, int selected)
{
    int        size, cnt = 2;
    XRectangle rect[3];
    Boolean    stacked;
    GC         gc = (GC)NULL;

    stacked = (XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
	       XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC);
    /*
     * Lets set up the GC used to draw the shadow that may or may not be
     * visible depending on the selected state.
     */
    if( selected )
    {
	if( XiSelectSpecified(tab) )
	{
	    gc = XmTabBox__tab_GC(tab);
	    SetSelectGC(tab, gc);
	}
	else
	{
	    if( XiBackgroundSpecified(info) )
	    {
		gc = XmTabBox__tab_GC(tab);
		SetBackgroundGC(tab, info, gc);
	    }
	    else
	    {
		gc = tab->manager.background_GC;
	    }
	}
    }

    size = corner_size;

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.top_shadow_GC, geometry->x,
			   geometry->y,
			   shadow, (int) geometry->height - size);

	    rect[0].x = geometry->x + geometry->width - shadow;
	    rect[0].y = geometry->y;
	    rect[0].width = shadow;
	    rect[0].height = (int) geometry->height - size;

	    rect[1].x = geometry->x + size;
	    rect[1].y = geometry->y + (int)geometry->height - shadow;
	    rect[1].width = (int)geometry->width - (2 * size);
	    rect[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		if( (geometry->column == 0 && !LayoutIsRtoLP(tab)) ||
			((geometry->column == XmTabBox__num_columns(tab)-1) &&
			LayoutIsRtoLP(tab)) )
		{
		    rect[2].x = geometry->x + shadow;
		    rect[2].width = (int)geometry->width - shadow;
		}
		else
		{
		    rect[2].x = geometry->x;
		    rect[2].width = geometry->width;
		}
		rect[2].y = geometry->y;
		rect[2].height = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.bottom_shadow_GC, rect, cnt);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + shadow,
			       geometry->y,
			       (int)geometry->width - 2*shadow,
			       shadow);

		if( geometry->row == 0 &&
			((geometry->column > 0 && !LayoutIsRtoLP(tab)) ||
			((geometry->column < XmTabBox__num_columns(tab)-1) &&
			LayoutIsRtoLP(tab))) )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTH);
		}
	    }
	    else if(geometry->row == 0 &&
		    ((geometry->column == 0 && !LayoutIsRtoLP(tab)) ||
		    ((geometry->column == XmTabBox__num_columns(tab)-1) &&
		    LayoutIsRtoLP(tab))) )
	    {
		XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    geometry->x, geometry->y, shadow,
			    XmBEVEL_BOTH);
	    }
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.top_shadow_GC,
			   geometry->x, geometry->y,
			   (int)geometry->width - size, shadow);

	    rect[0].x = geometry->x;
	    rect[0].y = geometry->y + (int)geometry->height - shadow;
	    rect[0].width = (int)geometry->width - size;
	    rect[0].height = shadow;

	    rect[1].x = geometry->x + (int)geometry->width - shadow;
	    rect[1].y = geometry->y + size;
	    rect[1].width = shadow;
	    rect[1].height = (int)geometry->height - (2 * size);

	    if( !selected && geometry->row == 0 )
	    {
		if( geometry->column == 0 )
		{
		    rect[2].y = geometry->y + shadow;
		    rect[2].height = (int)geometry->height - shadow;
		}
		else
		{
		    rect[2].y = geometry->y;
		    rect[2].height = geometry->height;
		}
		rect[2].x = geometry->x;
		rect[2].width = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.bottom_shadow_GC, rect, cnt);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x, geometry->y + shadow,
			       shadow,
			       (int)geometry->height - 2*shadow);
		if( geometry->column != 0 )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTH);
		}
	    }
	    else if( geometry->row == 0 && geometry->column == 0 )
	    {
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTTOM);
	    }
	}
	break;
    case XmTAB_EDGE_BOTTOM_RIGHT:

	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.bottom_shadow_GC,
			   geometry->x + (int)geometry->width - shadow,
			   geometry->y + size, shadow,
			   (int)geometry->height - size);

	    rect[0].x = geometry->x;
	    rect[0].y = geometry->y + size;
	    rect[0].width = shadow;
	    rect[0].height = (int) geometry->height - size;

	    rect[1].x = geometry->x + size;
	    rect[1].y = geometry->y;
	    rect[1].width = (int)geometry->width - (2 * size);
	    rect[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		rect[2].x = geometry->x;
		rect[2].y = geometry->y + (int)geometry->height - shadow;
		rect[2].width = geometry->width;
		rect[2].height = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC, rect, cnt);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + shadow,
			       geometry->y + (int)geometry->height - shadow,
			       (int)geometry->width - 2*shadow,
			       shadow);

		if( (!stacked &&
			(((geometry->column != XmTabBox__num_columns(tab) - 1) &&
			!LayoutIsRtoLP(tab)) ||
			(geometry->column != 0 && LayoutIsRtoLP(tab)))) ||
			(stacked && geometry->row == 0 &&
			(((geometry->column != XmTabBox__num_columns(tab) - 1) &&
			!LayoutIsRtoLP(tab)) ||
			XmTabBox__num_rows(tab) == 1 ||
			!XmTabBox_stacked_effect(tab))) )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTTOM);
		}
	    }
	    else if( (stacked && XmTabBox_stacked_effect(tab) &&
		     XmTabBox__num_rows(tab) > 1 &&
		     geometry->row == 0 &&
		     ((geometry->column == XmTabBox__num_columns(tab) - 1 &&
		     !LayoutIsRtoLP(tab)) ||
		     (geometry->column == 0 && LayoutIsRtoLP(tab)))) ||
		     (!stacked &&
		     ((geometry->column == XmTabBox__num_columns(tab) - 1 &&
		     !LayoutIsRtoLP(tab)) ||
		     (geometry->column == 0 && LayoutIsRtoLP(tab)))))
	    {
		XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    geometry->x + (int)geometry->width - shadow,
			    geometry->y + (int)geometry->height - shadow,
			    shadow, XmBEVEL_BOTTOM);
	    }
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.bottom_shadow_GC,
			   geometry->x + size,
			   geometry->y + (int)geometry->height - shadow,
			   (int)geometry->width - size, shadow);

	    rect[0].x = geometry->x + size;
	    rect[0].y = geometry->y;
	    rect[0].width = (int)geometry->width - size;
	    rect[0].height = shadow;

	    rect[1].x = geometry->x;
	    rect[1].y = geometry->y + size;
	    rect[1].width = shadow;
	    rect[1].height = (int)geometry->height - (2 * size);

	    if( !selected && geometry->row == 0 )
	    {
		rect[2].x = geometry->x + (int)geometry->width - shadow;
		rect[2].y = geometry->y;
		rect[2].width = shadow;
		rect[2].height = geometry->height;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC, rect, cnt);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + (int)geometry->width - shadow,
			       geometry->y + shadow,
			       shadow,
			       (int)geometry->height - 2*shadow);

		if( !stacked ||
		    (stacked && geometry->row == 0 &&
		    (geometry->column != XmTabBox__num_columns(tab) - 1 ||
		    XmTabBox__num_rows(tab) == 1 ||
		    !XmTabBox_stacked_effect(tab))) )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTTOM);
		}
	    }
	    else if( stacked && XmTabBox_stacked_effect(tab) &&
		     XmTabBox__num_rows(tab) > 1 &&
		     geometry->row == 0 &&
		     geometry->column == XmTabBox__num_columns(tab) - 1 )
	    {
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTTOM);
	    }
	}
	break;
    }
}

static void
DrawSquareShadows(XmTabBoxWidget tab, XmTabAttributes info,
		  XiTabRect *geometry,
		  Boolean selected, XmTabEdge edge, Dimension shadow)
{
    XRectangle rt[3];
    GC         gc = (GC)NULL;
    Boolean    stacked;
    int        cnt = 2;

    stacked = (XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
	       XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC);

    /*
     * Lets set up the GC used to draw the shadow that may or may not be
     * visible depending on the selected state.
     */
    if( selected )
    {
	if( XiSelectSpecified(tab) )
	{
	    gc = XmTabBox__tab_GC(tab);
	    SetSelectGC(tab, gc);
	}
	else
	{
	    if( XiBackgroundSpecified(info) )
	    {
		gc = XmTabBox__tab_GC(tab);
		SetBackgroundGC(tab, info, gc);
	    }
	    else
	    {
		gc = tab->manager.background_GC;
	    }
	}
    }

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.top_shadow_GC,
			   geometry->x, geometry->y,
			   shadow, (int)geometry->height - shadow);

	    rt[0].x = geometry->x + (int)geometry->width - shadow;
	    rt[0].y = geometry->y;
	    rt[0].width = shadow;
	    rt[0].height = geometry->height;

	    rt[1].x = geometry->x + shadow;
	    rt[1].y = geometry->y + (int) geometry->height - shadow;
	    rt[1].width = (int)geometry->width - shadow;
	    rt[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		if( (geometry->column == 0 && !LayoutIsRtoL(tab)) ||
			((geometry->column == XmTabBox__num_columns(tab)-1) &&
			LayoutIsRtoLP(tab)))
		{
		    rt[2].x = geometry->x + shadow;
		    rt[2].width = (int)geometry->width - shadow;
		}
		else
		{
		    rt[2].x = geometry->x;
		    rt[2].width = geometry->width;
		}
		rt[2].y = geometry->y;
		rt[2].height = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.bottom_shadow_GC, rt, cnt);

	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			tab->manager.top_shadow_GC,
			tab->manager.bottom_shadow_GC,
			geometry->x, geometry->y +
			(int)geometry->height - shadow, shadow,
			XmBEVEL_BOTH);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + shadow,
			       geometry->y,
			       (int)geometry->width - 2*shadow,
			       shadow);

		if (geometry->column == 0)
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTH);
		}
	    }
	    else if( geometry->row == 0 )
	    {
		if (geometry->column == 0 && !LayoutIsRtoLP(tab))
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    geometry->x, geometry->y, shadow,
			    XmBEVEL_BOTTOM);
		}
		else if (geometry->column == XmTabBox__num_columns(tab)-1 &&
		     LayoutIsRtoLP(tab))
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    geometry->x, geometry->y, shadow,
			    XmBEVEL_BOTTOM);
			    return;
		}

	    }
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.top_shadow_GC,
			   geometry->x, geometry->y,
			   (int)geometry->width - shadow, shadow);

	    rt[0].x = geometry->x + (int)geometry->width - shadow;
	    rt[0].y = geometry->y + shadow;
	    rt[0].width = shadow;
	    rt[0].height = geometry->height - shadow;

	    rt[1].x = geometry->x;
	    rt[1].y = geometry->y + (int) geometry->height - shadow;
	    rt[1].width = (int)geometry->width - shadow;
	    rt[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		if( geometry->column == 0 )
		{
		    rt[2].y = geometry->y + shadow;
		    rt[2].height = (int)geometry->height - shadow;
		}
		else
		{
		    rt[2].y = geometry->y;
		    rt[2].height = geometry->height;
		}
		rt[2].x = geometry->x;
		rt[2].width = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.bottom_shadow_GC, rt, cnt);

	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			tab->manager.top_shadow_GC,
			tab->manager.bottom_shadow_GC,
			geometry->x + (int)geometry->width - shadow,
			geometry->y, shadow, XmBEVEL_BOTH);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x, geometry->y + shadow,
			       shadow,
			       (int)geometry->height - 2*shadow);
		if( geometry->column != 0 )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTH);
		}
	    }
	    else if( geometry->row == 0 && geometry->column == 0 )
	    {
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				geometry->x, geometry->y, shadow,
				XmBEVEL_BOTTOM);
	    }
	}

	break;
    case XmTAB_EDGE_BOTTOM_RIGHT:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.bottom_shadow_GC,
			   geometry->x + (int)geometry->width - shadow,
			   geometry->y,
			   shadow,
			   (int)geometry->height);

	    rt[0].x = geometry->x;
	    rt[0].y = geometry->y;
	    rt[0].width = shadow;
	    rt[0].height = geometry->height;

	    rt[1].x = geometry->x;
	    rt[1].y = geometry->y;
	    rt[1].width = (int)geometry->width - shadow;
	    rt[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		rt[2].x = geometry->x;
		rt[2].y = geometry->y + (int)geometry->height - shadow;
		rt[2].width = geometry->width;
		rt[2].height = shadow;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC, rt, cnt);

	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			tab->manager.top_shadow_GC,
			tab->manager.bottom_shadow_GC,
			geometry->x + (int)geometry->width - shadow,
			geometry->y, shadow, XmBEVEL_TOP);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + shadow,
			       geometry->y + (int)geometry->height - shadow,
			       (int)geometry->width - 2*shadow,
			       shadow);

		if (LayoutIsRtoLP(tab))
		{
		    if (geometry->column != 0 && geometry->row == 0)
		    {
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTH);
		    }
		}
		else
		{
		    if( (!stacked &&
			((((geometry->column != XmTabBox__num_columns(tab) - 1) &&
			!LayoutIsRtoLP(tab))) ||
			(geometry->column != 0 && LayoutIsRtoLP(tab)))) ||
			(stacked && geometry->row == 0 &&
			(geometry->column != XmTabBox__num_columns(tab) - 1 ||
			(XmTabBox__num_rows(tab) == 1) ||
			!XmTabBox_stacked_effect(tab))) )
		    {
			XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTH);
		    }
		}
	    }
	    else if( (stacked && XmTabBox_stacked_effect(tab) &&
		     XmTabBox__num_rows(tab) > 1 &&
		     geometry->row == 0 &&
		     ((geometry->column == XmTabBox__num_columns(tab) - 1 &&
		     !LayoutIsRtoLP(tab)) ||
		     (geometry->column == 0 && LayoutIsRtoLP(tab)))) ||
		     (!stacked &&
		     ((geometry->column == XmTabBox__num_columns(tab) - 1 &&
		     !LayoutIsRtoLP(tab)) ||
		     (geometry->column == 0 && LayoutIsRtoLP(tab)))))
	    {
		XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC,
			    tab->manager.bottom_shadow_GC,
			    geometry->x + (int)geometry->width - shadow,
			    geometry->y + (int)geometry->height - shadow,
			    shadow, XmBEVEL_BOTTOM);
	    }
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.bottom_shadow_GC,
			   geometry->x + shadow,
			   geometry->y + (int)geometry->height - shadow,
			   (int)geometry->width - shadow, shadow);

	    rt[0].x = geometry->x;
	    rt[0].y = geometry->y;
	    rt[0].width = shadow;
	    rt[0].height = (int)geometry->height - shadow;

	    rt[1].x = geometry->x;
	    rt[1].y = geometry->y;
	    rt[1].width = geometry->width;
	    rt[1].height = shadow;

	    if( !selected && geometry->row == 0 )
	    {
		rt[2].x = geometry->x + (int)geometry->width - shadow;
		rt[2].y = geometry->y;
		rt[2].width = shadow;
		rt[2].height = geometry->height;
		cnt = 3;
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab),
			    tab->manager.top_shadow_GC, rt, cnt);

	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
			tab->manager.top_shadow_GC,
			tab->manager.bottom_shadow_GC,
			geometry->x,
			geometry->y + (int)geometry->height - shadow,
			shadow, XmBEVEL_BOTH);

	    if( selected )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + (int)geometry->width - shadow,
			       geometry->y + shadow,
			       shadow,
			       (int)geometry->height - 2*shadow);

		if( !stacked ||
		    (stacked && geometry->row == 0 &&
		    (geometry->column != XmTabBox__num_columns(tab) - 1 ||
		    XmTabBox__num_rows(tab) == 1 ||
		    !XmTabBox_stacked_effect(tab))) )
		{
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.bottom_shadow_GC,
				tab->manager.top_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTH);
		}
	    }
	    else if( stacked && XmTabBox_stacked_effect(tab) &&
		     XmTabBox__num_rows(tab) > 1 &&
		     geometry->row == 0 &&
		     geometry->column == XmTabBox__num_columns(tab) - 1 )
	    {
		    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				geometry->x + (int)geometry->width - shadow,
				geometry->y + (int)geometry->height - shadow,
				shadow, XmBEVEL_BOTTOM);
	    }
	}
	break;
    }
}

static void
DrawRoundedShadows(XmTabBoxWidget tab, XmTabAttributes info,
		   XiTabRect *geometry,
		   Boolean selected, XmTabEdge edge,
		   int shadow)
{
    int    size;

    size = XmTabBox__corner_size(tab);
    if( geometry->width < geometry->height )
    {
	if( size > (int)geometry->width/2 ) size = (int)geometry->width/2;
    }
    else
    {
	if( size > (int)geometry->height/2 ) size = (int)geometry->height/2;
    }

    DrawSegments(tab, info, geometry, edge, size, shadow, selected);

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x, geometry->y +
			 (int)geometry->height - size,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_3);
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x + (int)geometry->width - size,
			 geometry->y + (int)geometry->height - size,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_4);
	}
	else
	{
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x + (int)geometry->width - size,
			 geometry->y,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_1);
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x + (int)geometry->width - size,
			 geometry->y + (int)geometry->height - size,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_4);
	}
	break;
    case XmTAB_EDGE_BOTTOM_RIGHT:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x + (int)geometry->width - size,
			 geometry->y, size, size,
			 tab->manager.shadow_thickness, XiQUAD_1);
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x, geometry->y,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_2);
	}
	else
	{
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x,
			 geometry->y, size, size,
			 tab->manager.shadow_thickness, XiQUAD_2);
	    XiDrawCorner(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 tab->manager.bottom_shadow_GC,
			 geometry->x,
			 geometry->y + (int)geometry->height - size,
			 size, size, tab->manager.shadow_thickness,
			 XiQUAD_3);
	}
	break;
    }
}

static void
DrawBeveledShadows(XmTabBoxWidget tab, XmTabAttributes info,
		   XiTabRect *geometry,
		   Boolean selected, XmTabEdge edge,
		   int shadow)
{
    int    size;
    XPoint pt[4];

    size = XmTabBox__corner_size(tab);
    if( geometry->width < geometry->height )
    {
	if( size > (int)geometry->width/2 ) size = (int)geometry->width/2;
    }
    else
    {
	if( size > (int)geometry->height/2 ) size = (int)geometry->height/2;
    }

    DrawSegments(tab, info, geometry, edge, size, shadow, selected);

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    pt[0].x = geometry->x;
	    pt[0].y = geometry->y + (int)geometry->height - size;
	    pt[1].x = geometry->x + size;
	    pt[1].y = geometry->y + (int)geometry->height;
	    pt[2].x = geometry->x + size;
	    pt[2].y = geometry->y + (int)geometry->height - shadow;
	    pt[3].x = geometry->x + shadow;
	    pt[3].y = geometry->y + (int)geometry->height - size;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);

	    pt[0].x = geometry->x + (int)geometry->width;
	    pt[0].y = geometry->y + (int)geometry->height - size;
	    pt[1].x = geometry->x + (int)geometry->width - size;
	    pt[1].y = geometry->y + (int)geometry->height;
	    pt[2].x = geometry->x + (int)geometry->width - size;
	    pt[2].y = geometry->y + (int)geometry->height - shadow;
	    pt[3].x = geometry->x + (int)geometry->width - shadow;
	    pt[3].y = geometry->y + (int)geometry->height - size;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.bottom_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);
	}
	else
	{
	    pt[0].x = geometry->x + (int)geometry->width - size;
	    pt[0].y = geometry->y;
	    pt[1].x = geometry->x + (int)geometry->width;
	    pt[1].y = geometry->y + size;
	    pt[2].x = pt[1].x - shadow;
	    pt[2].y = pt[1].y;
	    pt[3].x = pt[0].x;
	    pt[3].y = pt[0].y + shadow;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.bottom_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);

	    pt[0].x = geometry->x + (int)geometry->width - size;
	    pt[0].y = geometry->y + (int)geometry->height;
	    pt[1].x = geometry->x + (int)geometry->width;
	    pt[1].y = geometry->y + (int)geometry->height - size;
	    pt[2].x = pt[1].x - shadow;
	    pt[2].y = pt[1].y;
	    pt[3].x = pt[0].x;
	    pt[3].y = pt[0].y - shadow;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.bottom_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);
	}
	break;
    case XmTAB_EDGE_BOTTOM_RIGHT:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    pt[0].x = geometry->x;
	    pt[0].y = geometry->y + size;
	    pt[1].x = geometry->x + size;
	    pt[1].y = geometry->y;
	    pt[2].x = geometry->x + size;
	    pt[2].y = geometry->y + shadow;
	    pt[3].x = geometry->x + shadow;
	    pt[3].y = geometry->y + size;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);

	    pt[0].x = geometry->x + (int)geometry->width;
	    pt[0].y = geometry->y + size;
	    pt[1].x = geometry->x + (int)geometry->width - size;
	    pt[1].y = geometry->y;
	    pt[2].x = geometry->x + (int)geometry->width - size;
	    pt[2].y = geometry->y + shadow;
	    pt[3].x = geometry->x + (int)geometry->width - shadow;
	    pt[3].y = geometry->y + size;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.bottom_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);
	}
	else
	{
	    pt[0].x = geometry->x + size;
	    pt[0].y = geometry->y;
	    pt[1].x = geometry->x;
	    pt[1].y = geometry->y + size;
	    pt[2].x = pt[1].x + shadow;
	    pt[2].y = pt[1].y;
	    pt[3].x = pt[0].x;
	    pt[3].y = pt[0].y + shadow;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);

	    pt[0].x = geometry->x + size;
	    pt[0].y = geometry->y + (int)geometry->height;
	    pt[1].x = geometry->x;
	    pt[1].y = geometry->y + (int)geometry->height - size;
	    pt[2].x = pt[1].x + shadow;
	    pt[2].y = pt[1].y;
	    pt[3].x = pt[0].x;
	    pt[3].y = pt[0].y - shadow;
	    XFillPolygon(XtDisplay(tab), XiCanvas(tab),
			 tab->manager.top_shadow_GC,
			 pt, 4, Convex, CoordModeOrigin);
	}
	break;
    }
}

static void
DrawTab(XmTabBoxWidget tab, XmTabAttributes info, XiTabRect *geometry,
	Boolean selected, Boolean keyboard)
{
    XmTabEdge        edge = XmTabBox_tab_edge(tab);
    Widget           canvas = XmTabBox__canvas(tab);
    XmFontList       font_list = XmTabBox_font_list(tab);
    Dimension        shadow_thickness = tab->manager.shadow_thickness,
                     margin_width = XmTabBox_tab_margin_width(tab),
                     margin_height = XmTabBox_tab_margin_width(tab),
                     size = XmTabBox__corner_size(tab);
    XRectangle       *clip;
    int              pix_width = 0, pix_height = 0, pix_depth = 0,
                     label_width = 0, label_height = 0, row;
    Boolean          have_pixmap = False, have_label = False;
    GC               gc = XmTabBox__tab_GC(tab);
    XmTabAttributes  above;

    /*
     * Lets do the quick check here.  If we are not realized then there is
     * really nothing to draw so lets just leave.
     */
    if( !XtIsRealized(canvas) || info == NULL ) return;

    if( (XmTabBox_tab_mode(tab) == XmTABS_STACKED ||
	 XmTabBox_tab_mode(tab) == XmTABS_STACKED_STATIC) &&
         XmTabBox_tab_style(tab) != XmTABS_SQUARED )
    {
	row = geometry->row;
	do
	{
	    if( LayoutIsRtoLP(tab) )
		above = GetTabInfo(tab, ++row, geometry->column-1);
	    else
		above = GetTabInfo(tab, ++row, geometry->column);
	} while( above == NULL && row < XmTabBox__num_rows(tab) );

	if( above != NULL )
	{
	    /*
	     * Ok, what is happening here is that we are in a stacked layout
	     * and we have a tab that is above us.  This means that we
	     * need to fill in our upper corner with this tabs color.
	     */
	    if( XiBackgroundSpecified(above) )
	    {
		SetBackgroundGC(tab, above, gc);
	    }
	    else
	    {
		if( ValidPixmap(tab->core.background_pixmap) )
		{
		    SetTiledXYGC(XtDisplay(tab), gc,
				 tab->core.background_pixmap,
				 -(tab->core.x), -(tab->core.y));
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
		}
	    }
	}
	else
	{
	    Widget gcParent = XiGCParent(tab);
	    Pixel  pixel;
	    Pixmap pixmap;

	    XtVaGetValues(gcParent,
			  XmNbackground, &pixel,
			  XmNbackgroundPixmap, &pixmap,
			  NULL);

	    if( ValidPixmap(pixmap) )
	    {
		SetTiledXYGC(XtDisplay(tab), gc, pixmap,
			     -(tab->core.x), -(tab->core.y));
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, pixel);
	    }

	}
	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + geometry->width - size,
			       geometry->y, size, size);
	    }
	    else
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x,
			       geometry->y + geometry->height - size,
			       size, size);
	    }
	}
	else
	{
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + geometry->width - size,
			       geometry->y + geometry->height - size,
			       size, size);
	    }
	    else
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + geometry->width - size,
			       geometry->y + geometry->height - size,
			       size, size);
	    }
	}

	row = geometry->row;
	do
	{
	    if( LayoutIsRtoLP(tab) )
		above = GetTabInfo(tab, ++row, geometry->column);
	    else
		above = GetTabInfo(tab, ++row, geometry->column - 1);
	} while( above == NULL && row < XmTabBox__num_rows(tab) );

	if( above != NULL )
	{
	    /*
	     * Ok, what is happening here is that we are in a stacked layout
	     * and we have a tab that is above us.  This means that we
	     * need to fill in our upper corner with this tabs color.
	     */
	    if( XiBackgroundSpecified(above) )
	    {
		SetBackgroundGC(tab, above, gc);
	    }
	    else
	    {
		if( ValidPixmap(tab->core.background_pixmap) )
		{
		    SetTiledXYGC(XtDisplay(tab), gc,
				 tab->core.background_pixmap,
				 -(tab->core.x), -(tab->core.y));
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
		}
	    }
	}
	else
	{
	    Widget gcParent = XiGCParent(tab);
	    Pixel  pixel;
	    Pixmap pixmap;

	    XtVaGetValues(gcParent,
			  XmNbackground, &pixel,
			  XmNbackgroundPixmap, &pixmap,
			  NULL);

	    if( ValidPixmap(pixmap) )
	    {
		SetTiledXYGC(XtDisplay(tab), gc, pixmap,
			     -(tab->core.x), -(tab->core.y));
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, pixel);
	    }

	}
	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x, geometry->y, size, size);
	    }
	    else
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x, geometry->y, size, size);
	    }
	}
	else
	{
	    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x,
			       geometry->y + geometry->height - size,
			       size, size);
	    }
	    else
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab), gc,
			       geometry->x + geometry->width - size,
			       geometry->y, size, size);
	    }
	}
    }

    if( selected && XiSelectSpecified(tab) )
    {
	SetSelectGC(tab, XmTabBox__tab_GC(tab));
    }
    else
    {
	/*
	 * Lets see if we need to fill in the background color of this tab, or
	 * if it wants the same color as its parent.
	 */
	if( XiBackgroundSpecified(info) )
	{
	    /*
	     * It appears that this tab wants its background filled in
	     * to a specific color so lets do that.
	     */
	    SetBackgroundGC(tab, info, XmTabBox__tab_GC(tab));
	}
	else
	{
	    if( ValidPixmap(tab->core.background_pixmap) )
	    {
		SetTiledXYGC(XtDisplay(tab), gc, tab->core.background_pixmap,
			     -(tab->core.x), -(tab->core.y));
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
	    }
	}
    }
    switch( XmTabBox_tab_style(tab) )
    {
    case XmTABS_SQUARED:
	XFillRectangle(XtDisplay(tab), XiCanvas(tab), XmTabBox__tab_GC(tab), geometry->x,
		       geometry->y, geometry->width, geometry->height);
	break;
    case XmTABS_ROUNDED:
	FillRoundedTab(tab, XmTabBox__tab_GC(tab), geometry, edge);
	break;
    case XmTABS_BEVELED:
	FillBeveledTab(tab, XmTabBox__tab_GC(tab), geometry, edge);
	break;
    }

    /*
     * The first thing that we will draw is the shadows around the tab.
     * We have a lot of nifty routines to do this.  Maybe will add more
     * someday.
     */

    switch( XmTabBox_tab_style(tab) )
    {
    case XmTABS_SQUARED:
	DrawSquareShadows(tab, info, geometry, selected, edge,
			  shadow_thickness);
	break;
    case XmTABS_ROUNDED:
	DrawRoundedShadows(tab, info, geometry, selected, edge,
			   shadow_thickness);
	break;
    case XmTABS_BEVELED:
	DrawBeveledShadows(tab, info, geometry, selected, edge,
			   shadow_thickness);
	break;
    default:
	break;
    }

    gc = XmTabBox__text_GC(tab);
    /*
     * Now comes the hard part we need to draw the label.  The label
     * consists of an XmString and a pixmap, both of which are optional.
     *
     * First lets calculate the size of both the label and the pixmap
     * so that we can calculate the amount of space we will need to
     * display the entire tab label.
     */
    if( ValidPixmap(info->label_pixmap) &&
        info->pixmap_placement != XmPIXMAP_NONE )
    {
	Window       window_unused;
	int          int_unused;
	unsigned int uint_unused, width_return, height_return, depth_return;

	have_pixmap = True;

	XGetGeometry(XtDisplay(tab), info->label_pixmap, &window_unused,
		     &int_unused, &int_unused, &width_return, &height_return,
		     &uint_unused, &depth_return);

	pix_depth = depth_return;
	pix_width = width_return;
	pix_height = height_return;
    }
    else
    {
	pix_depth = 0;
	pix_width = 0;
	pix_height = 0;
    }

    if( info->label_string != NULL &&
        info->pixmap_placement != XmPIXMAP_ONLY )
    {
	Dimension string_width, string_height;
	have_label = True;

	XmStringExtent(font_list, info->label_string,
		       &string_width, &string_height);
	label_width = string_width;
	label_height = string_height;
    }
    else
    {
	label_width = 0;
	label_height = 0;
    }

    SetSolidGC(XtDisplay(tab), gc,
	       (info->foreground == XmCOLOR_DYNAMIC
		? tab->manager.foreground
		: info->foreground));

    margin_width += shadow_thickness;
    margin_height += shadow_thickness;
    AssignMax(margin_width, size);
    AssignMax(margin_height, size);

    /*
     * Now lets calculate the cliping rectangle for the area in which
     * text and images can be drawn.  And since all we have left
     * to draw is text and images lets assign this rectangle as the
     * cliping area for our drawing GC.
     */
    clip = GetTabRectangle(tab, XmTAB_TEXT_RECT, geometry);
    XSetClipRectangles(XtDisplay(tab), gc, 0, 0, clip, 1, YXBanded);

    switch( XmTabBox_tab_orientation(tab) )
    {
    case XmTABS_LEFT_TO_RIGHT:
    default:
	DrawLeftToRightTab(tab, info, gc, have_pixmap, pix_width, pix_height,
			   pix_depth, have_label, label_width, label_height,
			   clip);
	break;
    case XmTABS_RIGHT_TO_LEFT:
	DrawRightToLeftTab(tab, info, gc, have_pixmap, pix_width, pix_height,
			   pix_depth, have_label, label_width, label_height,
			   clip, selected);
	break;
    case XmTABS_TOP_TO_BOTTOM:
	DrawTopToBottomTab(tab, info, gc, have_pixmap, pix_width, pix_height,
			   pix_depth, have_label, label_width, label_height,
			   clip, selected);
	break;
    case XmTABS_BOTTOM_TO_TOP:
	DrawBottomToTopTab(tab, info, gc, have_pixmap, pix_width, pix_height,
			   pix_depth, have_label, label_width, label_height,
			   clip, selected);
	break;
    }
    /*
     * The final bit of draw we have to do is to add the highlight
     * border to the tab if it currently has keyboard traversal.
     */
    if( keyboard && Prim_HaveTraversal(XmTabBox__canvas(tab)) )
    {
	BorderHighlight(XmTabBox__canvas(tab));
    }

    /*
     * Now that our drawing is done lets be sure to zero out the
     * clipping area for our GC.
     */
    XSetClipMask(XtDisplay(tab), gc, None);
}

#define XiCvtDone(type, value) 				\
    {							\
	if( to->addr != NULL )				\
	{						\
	    if( to->size < sizeof(type) )		\
	    {						\
		to->size = sizeof(type);		\
		return( False );			\
	    }						\
	    *(type*)(to->addr) = (value);		\
	}						\
	else						\
	{						\
	    static type static_val;			\
	    static_val = (value);			\
	    to->addr = (XtPointer) &static_val;		\
	}						\
	to->size = sizeof(type);			\
	return( True );					\
    }

static Boolean
CvtStringToTabOrientation(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
			  XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTABS_TOP_TO_BOTTOM;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "ORIENTATION_DYNAMIC") == 0 ||
        XmCompareISOLatin1(str, "XiTABS_ORIENTATION_DYNAMIC") == 0 )
    {
	result = XmTAB_ORIENTATION_DYNAMIC;
    }
    else if( XmCompareISOLatin1(str, "TOP_TO_BOTTOM") == 0 ||
        XmCompareISOLatin1(str, "XmTABS_TOP_TO_BOTTOM") == 0 )
    {
	result = XmTABS_TOP_TO_BOTTOM;
    }
    else if( XmCompareISOLatin1(str, "BOTTOM_TO_TOP") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_BOTTOM_TO_TOP") == 0 )
    {
	result = XmTABS_BOTTOM_TO_TOP;
    }
    else if( XmCompareISOLatin1(str, "LEFT_TO_RIGHT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_LEFT_TO_RIGHT") == 0 )
    {
	result = XmTABS_LEFT_TO_RIGHT;
    }
    else if( XmCompareISOLatin1(str, "RIGHT_TO_LEFT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_RIGHT_TO_LEFT") == 0 )
    {
	result = XmTABS_RIGHT_TO_LEFT;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabOrientation);
	return( False );
    }

    XiCvtDone(int, result);
}

static Boolean
CvtStringToTabSide(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		    XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTABS_ON_TOP;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "ON_TOP") == 0 ||
        XmCompareISOLatin1(str, "XmTABS_ON_TOP") == 0 )
    {
	result = XmTABS_ON_TOP;
    }
    else if( XmCompareISOLatin1(str, "ON_BOTTOM") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_BOTTOM") == 0 )
    {
	result = XmTABS_ON_BOTTOM;
    }
    else if( XmCompareISOLatin1(str, "ON_LEFT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_LEFT") == 0 )
    {
	result = XmTABS_ON_LEFT;
    }
    else if( XmCompareISOLatin1(str, "ON_RIGHT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_RIGHT") == 0 )
    {
	result = XmTABS_ON_RIGHT;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabSide);
	return( False );
    }

    XiCvtDone(int, result);
}

static Boolean
CvtStringToTabStyle(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		    XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTABS_BEVELED;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "SQUARED") == 0 ||
        XmCompareISOLatin1(str, "XmTABS_SQUARED") == 0 )
    {
	result = XmTABS_SQUARED;
    }
    else if( XmCompareISOLatin1(str, "ROUNDED") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ROUNDED") == 0 )
    {
	result = XmTABS_ROUNDED;
    }
    else if( XmCompareISOLatin1(str, "BEVELED") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_BEVELED") == 0 )
    {
	result = XmTABS_BEVELED;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabStyle);
	return( False );
    }

    XiCvtDone(int, result);
}

static Boolean
CvtStringToTabMode(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		   XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTABS_BASIC;
    String     str = (String) from->addr;

    if( XmCompareISOLatin1(str, "BASIC") == 0 ||
        XmCompareISOLatin1(str, "XmTABS_BASIC") == 0 )
    {
    }
    else if( XmCompareISOLatin1(str, "STACKED") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_STACKED") == 0 )
    {
	result = XmTABS_STACKED;
    }
    else if( XmCompareISOLatin1(str, "STACKED_STATIC") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_STACKED_STATIC") == 0 )
    {
	result = XmTABS_STACKED_STATIC;
    }
#ifdef SCROLLED_LAYOUT
    else if( XmCompareISOLatin1(str, "SCROLLED") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_SCROLLED") == 0 )
    {
	result = XmTABS_SCROLLED;
    }
#endif
#ifdef OVERLAYED_LAYOUT
    else if( XmCompareISOLatin1(str, "OVERLAYED") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_OVERLAYED") == 0 )
    {
	result = XmTABS_OVERLAYED;
    }
#endif
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabMode);
	return( False );
    }

    XiCvtDone(int, result);
}

static Boolean
CvtStringToTabEdge(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		   XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTAB_EDGE_BOTTOM_RIGHT;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "EDGE_TOP_LEFT") == 0 ||
        XmCompareISOLatin1(str, "XmTAB_EDGE_TOP_LEFT") == 0 )
    {
	result = XmTAB_EDGE_TOP_LEFT;
    }
    else if( XmCompareISOLatin1(str, "EDGE_BOTTOM_RIGHT") == 0 ||
        XmCompareISOLatin1(str, "XiTABS_EDGE_BOTTOM_RIGHT") == 0 )
    {
	result = XmTAB_EDGE_BOTTOM_RIGHT;
    }
    else if( XmCompareISOLatin1(str, "bottom") == 0 )
    {
	result = XmTAB_EDGE_TOP_LEFT;
    }
    else if( XmCompareISOLatin1(str, "right") == 0 )
    {
	result = XmTAB_EDGE_BOTTOM_RIGHT;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabEdge);
	return( False );
    }

    XiCvtDone(int, result);
}

#ifdef SCROLLED_LAYOUT
static Boolean
CvtStringToArrowPlacement(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
			  XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTAB_ARROWS_ON_LEFT;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "left") == 0 ||
        XmCompareISOLatin1(str, "tab_arrows_on_left") == 0 )
    {
	result = XmTAB_ARROWS_ON_LEFT;
    }
    else if( XmCompareISOLatin1(str, "right") == 0 ||
             XmCompareISOLatin1(str, "tab_arrows_on_right") == 0 )
    {
	result = XmTAB_ARROWS_ON_RIGHT;
    }
    else if( XmCompareISOLatin1(str, "split") == 0 ||
             XmCompareISOLatin1(str, "tab_arrows_split") == 0 )
    {
	result = XmTAB_ARROWS_SPLIT;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRTabArrowPlacement);
	return( False );
    }

    XiCvtDone(int, result);
}
#endif /* SCROLLED_LAYOUT */

static Boolean
CvtStringToTabList(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		   XrmValue *from, XrmValue *to, XtPointer *data)
{
    return( False );
}

static void
CvtDestroyTabList(XtAppContext app_context, XrmValue *to, XtPointer data,
			      XrmValue *args, Cardinal *num_args)
{
}

static short XiCosSinData[][2] = {
    { 1000,    0 },    {  998,   52 },    {  994,  104 },    {  987,  156 },
    {  978,  207 },    {  965,  258 },    {  951,  309 },    {  933,  358 },
    {  913,  406 },    {  891,  453 },    {  866,  499 },    {  838,  544 },
    {  809,  587 },    {  777,  629 },    {  743,  669 },    {  707,  707 },
    {  669,  743 },    {  629,  777 },    {  587,  809 },    {  544,  838 },
    {  500,  866 },    {  453,  891 },    {  406,  913 },    {  358,  933 },
    {  309,  951 },    {  258,  965 },    {  207,  978 },    {  156,  987 },
    {  104,  994 },    {   52,  998 },    {    0, 1000 }};

#define NUM_PTS    31
#define NUM_PTSx2  62
#define HALF_PTS   16
#define HALF_PTSx2 32
#define NUM_PTSp1  32

static void
XiDrawCorner(Display *dpy, Drawable d, GC top_gc, GC bottom_gc, int x, int y,
	     unsigned int width, unsigned int height, unsigned int size,
	     XiQuadrant quadrant)
{
    XPoint pt[NUM_PTSx2];
    int    i, xrad1, xrad2, yrad1, yrad2, tmp;

    xrad1 = width;
    xrad2 = width - size;
    yrad1 = height;
    yrad2 = height - size;

    switch( quadrant )
    {
    case XiQUAD_1:
	y += height;
	for( i = 0; i < HALF_PTS; ++i )
	{
	    pt[i].x = x + XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y - XiCosSinData[i][1] * yrad1 / 1000;

	    pt[HALF_PTSx2-1-i].x = x + XiCosSinData[i][0] * xrad2 / 1000;
	    pt[HALF_PTSx2-1-i].y = y - XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, bottom_gc, pt, HALF_PTSx2, Nonconvex,
		     CoordModeOrigin);
	for( tmp = 0, i = HALF_PTS-1; i < NUM_PTS; ++i, ++tmp )
	{
	    pt[tmp].x = x + XiCosSinData[i][0] * xrad1 / 1000;
	    pt[tmp].y = y - XiCosSinData[i][1] * yrad1 / 1000;

	    pt[HALF_PTSx2-1-tmp].x = x + XiCosSinData[i][0] * xrad2 / 1000;
	    pt[HALF_PTSx2-1-tmp].y = y - XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, top_gc, pt, HALF_PTSx2, Nonconvex,
		     CoordModeOrigin);
	break;
    case XiQUAD_2:
	x += width;
	y += height;

	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x - XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y - XiCosSinData[i][1] * yrad1 / 1000;

	    pt[NUM_PTSx2-1-i].x = x - (int)XiCosSinData[i][0] * xrad2 / 1000;
	    pt[NUM_PTSx2-1-i].y =  y - (int)XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, top_gc, pt, NUM_PTSx2, Nonconvex,
		     CoordModeOrigin);
	break;
    case XiQUAD_3:
	x += width;
	for( i = 0; i < HALF_PTS; ++i )
	{
	    pt[i].x = x - XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y + XiCosSinData[i][1] * yrad1 / 1000;

	    pt[HALF_PTSx2-1-i].x = x - XiCosSinData[i][0] * xrad2 / 1000;
	    pt[HALF_PTSx2-1-i].y = y + XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, top_gc, pt, HALF_PTSx2, Nonconvex,
		     CoordModeOrigin);

	for( tmp = 0, i = HALF_PTS-1; i < NUM_PTS; ++i, ++tmp )
	{
	    pt[tmp].x = x - XiCosSinData[i][0] * xrad1 / 1000;
	    pt[tmp].y = y + XiCosSinData[i][1] * yrad1 / 1000;

	    pt[HALF_PTSx2-1-tmp].x = x - XiCosSinData[i][0] * xrad2 / 1000;
	    pt[HALF_PTSx2-1-tmp].y = y + XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, bottom_gc, pt, HALF_PTSx2, Nonconvex,
		     CoordModeOrigin);
	break;
    case XiQUAD_4:
	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x + XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y + XiCosSinData[i][1] * yrad1 / 1000;

	    pt[NUM_PTSx2-1-i].x = x + XiCosSinData[i][0] * xrad2 / 1000;
	    pt[NUM_PTSx2-1-i].y = y + XiCosSinData[i][1] * yrad2 / 1000;
	}
	XFillPolygon(dpy, d, bottom_gc, pt, NUM_PTSx2, Nonconvex,
		     CoordModeOrigin);
	break;
    default:
	break;
    }
}

static void
XiFillCorner(Display *dpy, Drawable d, GC gc, int x, int y,
	     unsigned int width, unsigned int height, XiQuadrant quadrant)
{
    XPoint pt[NUM_PTSp1];
    int    i, xrad1, yrad1;

    xrad1 = width;
    yrad1 = height;

    switch( quadrant )
    {
    case XiQUAD_1:
	y += height;
	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x + XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y - XiCosSinData[i][1] * yrad1 / 1000;
	}
	pt[i].x = x;
	pt[i++].y = y + height;
	XFillPolygon(dpy, d, gc, pt, i, Nonconvex, CoordModeOrigin);
	break;
    case XiQUAD_2:
	x += width;
	y += height;

	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x - XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y - XiCosSinData[i][1] * yrad1 / 1000;
	}
	pt[i].x = x + width;
	pt[i++].y = y + height;
	XFillPolygon(dpy, d, gc, pt, i, Nonconvex, CoordModeOrigin);
	break;
    case XiQUAD_3:
	x += width;
	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x - XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y + XiCosSinData[i][1] * yrad1 / 1000;
	}
	pt[i].x = x + width;
	pt[i++].y = y;
	XFillPolygon(dpy, d, gc, pt, i, Nonconvex, CoordModeOrigin);
	break;
    case XiQUAD_4:
	for( i = 0; i < NUM_PTS; ++i )
	{
	    pt[i].x = x + XiCosSinData[i][0] * xrad1 / 1000;
	    pt[i].y = y + XiCosSinData[i][1] * yrad1 / 1000;
	}
	pt[i].x = x;
	pt[i++].y = y;
	XFillPolygon(dpy, d, gc, pt, i, Nonconvex, CoordModeOrigin);
	break;
    default:
	break;
    }
}

static void
HorizontalBasicLayout(XmTabBoxWidget tab)
{
    int        i, x, height, cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    XiTabRect  *actual = XmTabBox__actual(tab);
    XRectangle *wanted = XmTabBox__wanted(tab);

    x = LayoutIsRtoLP(tab) ? (tab->core.width - wanted[0].width) : 0;
    height = XtHeight(tab);
    for( i = 0; i < cnt; ++i )
    {
	actual[i].x = x;
	actual[i].y = 0;
	actual[i].width = wanted[i].width;
	actual[i].height = height;
	actual[i].row = 0;
	actual[i].column = i;
	if (i < cnt-1)
	    x += LayoutIsRtoLP(tab) ? -actual[i+1].width : actual[i].width;
    }
    XmTabBox__num_rows(tab) = 1;
    XmTabBox__num_columns(tab) = i;
}

static void
VerticalBasicLayout(XmTabBoxWidget tab)
{
    int        i, y, width, cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    XiTabRect  *actual = XmTabBox__actual(tab);
    XRectangle *wanted = XmTabBox__wanted(tab);

    y = 0;
    width = XtWidth(tab);
    for( i = 0; i < cnt; ++i )
    {
	actual[i].x = 0;
	actual[i].y = y;
	actual[i].width = width;
	actual[i].height = wanted[i].height;
	actual[i].row = 0;
	actual[i].column = i;
	y += actual[i].height;
    }
    XmTabBox__num_rows(tab) = 1;
    XmTabBox__num_columns(tab) = i;
}

static void
Layout(XmTabBoxWidget tab)
{
    XmTabbedStackList  list = XmTabBox_tab_list(tab);
    int        count = _XmTabbedStackListCount(list);
    XRectangle geometry;

    if( count == 0 )
    {
	XmTabBox__num_columns(tab) = 0;
	XmTabBox__num_rows(tab) = 0;
	return;
    }

    if( count > XmTabBox__num_actual(tab) )
    {
	XmTabBox__num_actual(tab) = count;
	XmTabBox__actual(tab) = (XiTabRect*)
	    XtRealloc((XtPointer) XmTabBox__actual(tab),
		      sizeof(XiTabRect) * count);
    }

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
	CalcGeometry(tab, &geometry);
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    HorizontalBasicLayout(tab);
	}
	else
	{
	    VerticalBasicLayout(tab);
	}
	break;
    case XmTABS_STACKED:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    HorizontalStackedLayout(tab, False);
	}
	else
	{
	    VerticalStackedLayout(tab, False);
	}
	break;
    case XmTABS_STACKED_STATIC:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    HorizontalStackedLayout(tab, True);
	}
	else
	{
	    VerticalStackedLayout(tab, True);
	}
	break;
    case XmTABS_SCROLLED:
	break;
    case XmTABS_OVERLAYED:
	break;
    }
}

/*
 * Function:
 *	HorizontalBasicRedisplay(tab)
 * Description:
 *	This function performs the basic tab redisplay in the horizontal
 *	direction.
 * Input:
 *	tab   : XtTabBoxWidget - the tab box to redisplay
 * Output:
 *	None.
 */
static void
HorizontalBasicRedisplay(XmTabBoxWidget tab)
{
    XiTabRect       *geom;
    int             count = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), x,
                    shadow = tab->manager.shadow_thickness;

    geom = XmTabBox__actual(tab);
    x = geom[count-1].x + geom[count-1].width;

    /*
     * Now that we are done with the tabs we need to complete the line
     * from the last tab to the edge of the widget.
     */
    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
    {
	if (LayoutIsRtoLP(tab))
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.top_shadow_GC,
		       0, (int)XtHeight(tab) - shadow,
		       geom[count-1].x, shadow);
	else
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.top_shadow_GC,
		       x, (int)XtHeight(tab) - shadow,
		       (int)XtWidth(tab) - x, shadow);
	XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    (int)XtWidth(tab) - shadow,
		    (int)XtHeight(tab) - shadow,
		    shadow, XmBEVEL_BOTTOM);
    }
    else
    {
	if (LayoutIsRtoLP(tab))
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.bottom_shadow_GC,
		       0, 0, geom[count-1].x, shadow);
	else
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.bottom_shadow_GC,
		       x, 0, (int)XtWidth(tab) - x, shadow);
    }
}

/*
 * Function:
 *	VerticalBasicRedisplay(tab)
 * Description:
 *	This function performs the basic tab redisplay in the Vertical
 *	direction.
 * Input:
 *	tab   : XtTabBoxWidget - the tab box to redisplay
 * Output:
 *	None.
 */
static void
VerticalBasicRedisplay(XmTabBoxWidget tab)
{
    XiTabRect       *geom;
    int             count = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), y,
                    shadow = tab->manager.shadow_thickness;

    geom = XmTabBox__actual(tab);
    y = geom[count-1].y + geom[count-1].height;

    /*
     * Now that we are done with the tabs we need to complete the line
     * from the last tab to the edge of the widget.
     */
    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
    {
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.top_shadow_GC,
		       (int)XtWidth(tab) - shadow, y, shadow,
		       (int)XtHeight(tab) - y);
	XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.top_shadow_GC, tab->manager.bottom_shadow_GC,
		    (int)XtWidth(tab) - shadow, (int)XtHeight(tab) - shadow,
		    shadow, XmBEVEL_BOTTOM);
    }
    else
    {
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.bottom_shadow_GC,
		       0, y, shadow, (int)XtHeight(tab) - y);
    }
}

/*
 * Function:
 *	XiRotateImage(tab, src, degree)
 * Description:
 *	Returns a new XImage which is the given image rotate by either
 *	0 (a strait copy), 90, 180, or 270 degrees.
 * Input:
 *	tab    : XmTabBoxWidget - used for reference
 *	src    : XImage*        - the image to rotate
 *	degree : int            - how many degrees to rotate "floored"
 *			          to the nearest 90
 * Output:
 *	XImage* - and new allocated, rotated copy of the image
 */
static XImage*
XiRotateImage(XmTabBoxWidget tab, XImage *src, int degree)
{
    XImage *dst;
    char   *data;
    int    x, y, width, height, depth, tmp;

    if( src == NULL ) return( NULL );

    width = XImageWidth(src);
    height = XImageHeight(src);
    depth = XImageDepth(src);

    if( depth < 8 )
    {
	int cnt = 8/depth;

	if( degree == 0 || degree == 180 )
	{
	    tmp = (width/cnt) + ((width % cnt != 0) ? 1 : 0);
	    tmp = tmp * height;
	}
	else
	{
	    tmp = (height/cnt) + ((height % cnt != 0) ? 1 : 0);
	    tmp = tmp * width;
	}
    }
    else
    {
	tmp = width * height * depth;
    }
    data = (char*) XtMalloc(tmp);

    if( degree == 0 || degree == 180 )
    {
	dst = XCreateImage(XtDisplay(tab),
			   GetShellVisual((Widget)tab),
			   depth, (depth == 1 ? XYBitmap : XYPixmap),
			   0, data, width, height,
			   8, 0);

	if( degree == 180 )
	{
	    for( y = 0; y < height; ++y )
	    {
		for( x = 0; x < width; ++x )
		{
		    XPutPixel(dst, (width-1-x), (height-1-y),
			      XGetPixel(src, x, y));
		}
	    }
	}
	else
	{
	    for( y = 0; y < height; ++y )
	    {
		for( x = 0; x < width; ++x )
		{
		    XPutPixel(dst, x, y, XGetPixel(src, x, y));
		}
	    }
	}
    }
    else
    {
	dst = XCreateImage(XtDisplay(tab),
			   GetShellVisual((Widget)tab),
			   depth, (depth == 1 ? XYBitmap : XYPixmap),
			   0, data, height, width, 8, 0);

	if( degree == 90 )
	{
	    for( y = 0; y < height; ++y )
	    {
		for( x = 0; x < width; ++x )
		{
		    XPutPixel(dst, (height-1-y), x,
			      XGetPixel(src, x, y));
		}
	    }
	}
	else
	{
	    for( y = 0; y < height; ++y )
	    {
		for( x = 0; x < width; ++x )
		{
		    XPutPixel(dst, y, (width-1-x),
			      XGetPixel(src, x, y));
		}
	    }
	}
    }

    return( dst );
}

static void CalcCornerSize(XmTabBoxWidget tab)
{
    XmFontContext   fc;
    XmFontListEntry entry;
    XmFontType      font_type;
    XtPointer       value;
    int             tmp, size = 0;

    XmFontListInitFontContext(&fc, XmTabBox_font_list(tab));

    while( (entry = XmFontListNextEntry(fc)) != NULL )
    {
	value = (XtPointer) XmFontListEntryGetFont(entry, &font_type);

	if( font_type == XmFONT_IS_FONT )
	{
	    XFontStruct     *font;

	    font = (XFontStruct*) value;
	    tmp = font->ascent + font->descent;
	    AssignMax(size, tmp);
	}
#if USE_XFT
        else if (font_type == XmFONT_IS_XFT)
	{
	    tmp = ((XftFont*)value)->ascent + ((XftFont*)value)->descent;
	    AssignMax(size, tmp);
	}
#endif
	else
	{
	    XFontSetExtents *extents;

	    extents = XExtentsOfFontSet((XFontSet) value);
	    tmp = extents->max_logical_extent.height;
	    AssignMax(size, tmp);
	}
    }
    XmFontListFreeFontContext(fc);


    XmTabBox__corner_size(tab) = size * XmTabBox_tab_corner_percent(tab)/ 100;
}

static int XiXYtoTab(XmTabBoxWidget tab, int x, int y)
{
    int        i, cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), row = -1, idx;
    XiTabRect  *area = XmTabBox__actual(tab);

    for( i = 0; i < cnt; ++i )
    {
	if( x < area[i].x || x > (area[i].x + (int)area[i].width) ||
	    y < area[i].y || y > (area[i].y + (int)area[i].height) )
	{
	    continue;
	}
	return( i );
    }

    /*
     * If we are in a stacked mode then we will do some more checking,
     * however if we are not in a stacked mode then we are done.
     */
    if( XmTabBox_tab_mode(tab) != XmTABS_STACKED &&
        XmTabBox_tab_mode(tab) != XmTABS_STACKED_STATIC )
    {
	return( -1 );
    }

    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	/*
	 * Since we are in a stacked mode it is possible that we clicked in
	 * a partial row.  So lets start by finding out which partial row
	 * we clicked in.  This is done by just looking at the height
	 * component of the tabs.
	 */
	for( i = 0; i < cnt; ++i )
	{
	    if( y >= area[i].y && y <= (area[i].y + (int)area[i].height) )
	    {
		/*
		 * We found the row we care about.
		 */
		row = area[i].row;
		break;
	    }
	}

	/*
	 * If we did not click in a row, or if that row is the last row we are
	 * done.
	 */
	if( row == -1 || row == (XmTabBox__num_rows(tab) - 1) ) return( -1 );

	/*
	 * Now we want to walk through all the columns of the previous row and
	 * see if the x position is in those columns.  And if we find a match
	 * we found a winner.
	 */
	for( i = 0, ++row, idx = 0;
	     idx >= 0 && i < XmTabBox__num_columns(tab); ++i )
	{
	    idx = GetTabIndex(tab, row, i);
	    if( idx < 0 || x < area[i].x || x > (area[i].x + area[i].width) )
	    {
		continue;
	    }
	    return( idx );
	}
    }
    else
    {
	/*
	 * Since we are in a stacked mode it is possible that we clicked in
	 * a partial row.  So lets start by finding out which partial row
	 * we clicked in.  This is done by just looking at the height
	 * component of the tabs.
	 */
	for( i = 0; i < cnt; ++i )
	{
	    if( x >= area[i].x && x <= (area[i].x + (int)area[i].width) )
	    {
		/*
		 * We found the row we care about.
		 */
		row = area[i].row;
		break;
	    }
	}

	/*
	 * If we did not click in a row, or if that row is the last row we are
	 * done.
	 */
	if( row == -1 || row == (XmTabBox__num_rows(tab) - 1) ) return( -1 );

	/*
	 * Now we want to walk through all the columns of the previous row and
	 * see if the x position is in those columns.  And if we find a match
	 * we found a winner.
	 */
	for( i = 0, ++row, idx = 0;
	     idx >= 0 && i < XmTabBox__num_columns(tab); ++i )
	{
	    idx = GetTabIndex(tab, row, i);
	    if( idx < 0 || y < area[i].y || y > (area[i].y + area[i].height) )
	    {
		continue;
	    }
	    return( idx );
	}
    }

    /*
     * If we get here we really did not find a match.
     */
    return( -1 );
}

/*
 * Function:
 *	CalcTabGeometry(XmTabBoxWidget tab)
 * Description:
 *	This function calculates and caches the geometry for all the tabs,
 *	taking into account the XmNuniformTabSize resource.
 * Input:
 *	tab : XmTabBoxWidget - the tab box who needs the geometries
 *                             calculated.
 * Output:
 *	None.
 */
static void CalcTabGeometry(XmTabBoxWidget tab)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    XmTabAttributes info;
    Dimension       width, height, max_width = 0, max_height = 0;
    int             count = _XmTabbedStackListCount(XmTabBox_tab_list(tab)), i;
    XRectangle      *geom;

    /*
     * First lets see if we have enough room to cache all the geometries
     * and if not lets increase our cache so there is room.
     */
    if( count > XmTabBox__num_wanted(tab) )
    {
	XmTabBox__num_wanted(tab) = count;
	XmTabBox__wanted(tab) = (XRectangle*)
	    XtRealloc((XtPointer)XmTabBox__wanted(tab),
		      sizeof(XRectangle) * count);
    }
    geom = XmTabBox__wanted(tab);

    /*
     * Now that we have enough space to cache all the Tab sizes we
     * need to walk through and calculate the size of all the tabs.
     */
    for( i = 0; i < count; ++i )
    {
	/*
	 * Now lets get the info for a tab and send it off to this handy
	 * dandy tab size calculator.
	 */
	info = _XmTabbedStackListGet(list, i);
	XiCalcTabSize(tab, info, &width, &height);

	/*
	 * Now lets do a quick check here to see if we are going to be
	 * making the tabs uniform and if so lets capture the maximum
	 * width and height of the tabs.
	 */
	if( XmTabBox_uniform_tab_size(tab) )
	{
	    AssignMax(max_width, width);
	    AssignMax(max_height, height);
	}
	else
	{
	    /*
	     * Since we are not doing uniform sized tabs here we can
	     * assign the values from our size calculation to the
	     * geometry cache.
	     */
	    geom[i].width = width;
	    geom[i].height = height;
	}
    }

    /*
     * Well if we are doing uniform tab sizes then we need to make
     * one more pass over the tab geometry cache assigning the
     * same width and height to each tab.
     */
    if( XmTabBox_uniform_tab_size(tab) )
    {
	for( i = 0; i < count; ++i )
	{
	    geom[i].width = max_width;
	    geom[i].height = max_height;
	}
    }
}

/*
 * Function:
 *	CalcGeometryMinor(tab, major_d)
 * Description:
 *	Given the major Dimension calculates the needed minor dimension to
 *	display correctly.
 * Input:
 *	tab     : XmTabBoxWidget - the tab box to work with
 *	major_d : int            - the major dimension
 * Output:
 *	int - the desired minor dimension given the major dimension
 */
static int CalcGeometryMinor(XmTabBoxWidget tab, int major_d)
{
    XmTabbedStackList  list = XmTabBox_tab_list(tab);
    int        count = _XmTabbedStackListCount(list), i, max;
    XRectangle *geom;

    /*
     * First lets check if we have any tabs, because if we do not, then
     * our minor dimension becomes real easy ... all it is is our
     * shadow thickness.
     */
    if( count == 0 )
    {
	/*
	 * We do not have any tabs so we want to be the hieght of the
	 * shadow.
	 */
	return( tab->manager.shadow_thickness );
    }

    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	/*
	 * Ok, what is happening here is that we have a width and given this
	 * width we want to see what height we will be.  We will start with
	 * the easy check, i.e do we have any tabs.
	 */

	/*
	 * We have kids, so what we need to do is fake a given layout with
	 * the specified width but allowing our height to grow.
	 */
	switch( XmTabBox_tab_mode(tab) )
	{
	case XmTABS_BASIC:
	case XmTABS_OVERLAYED:
	case XmTABS_SCROLLED:
	default:
	    /*
	     * This is the simple layout case where all the tabs are in
	     * one row.  So for these layouts all we have to do is find
	     * the maximum height of all the tabs.
	     */
	    for( i = 0, max = 0, geom = XmTabBox__wanted(tab); i < count; ++i )
	    {
		AssignMax(max, (int)geom[i].height);
	    }
	    return( max );
	case XmTABS_STACKED:
	case XmTABS_STACKED_STATIC:
 	    {
		int per_line, num_rows, tmp, tab_width, tab_height,
		    offset = XmTabBox_tab_offset(tab);

		tab_height = XmTabBox__wanted(tab)[0].height;
		tab_width = XmTabBox__wanted(tab)[0].width;

		per_line = count;
		num_rows = (int)(count/per_line) +
		    (count % per_line > 0 ? 1 : 0);
		tmp = (per_line * tab_width) + (num_rows * offset);
		while( per_line > 1 && tmp > major_d )
		{
		    per_line--;
		    num_rows = (int)(count/per_line) +
			(count % per_line > 0 ? 1 : 0);
		    tmp = (per_line * tab_width) + (num_rows * offset);
		}
		return( num_rows * tab_height );
	    }
	}
    }

    /* orientation == XmVERTICAL */

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_OVERLAYED:
    case XmTABS_SCROLLED:
    default:
	for( i = 0, max = 0, geom = XmTabBox__wanted(tab); i < count; ++i )
	{
	    AssignMax(max, (int)geom[i].width);
	}
	return( max );
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
        {
	    int per_line, num_rows, tmp, tab_width, tab_height,
	    offset = XmTabBox_tab_offset(tab);

	    tab_height = XmTabBox__wanted(tab)[0].height;
	    tab_width = XmTabBox__wanted(tab)[0].width;

	    per_line = count;
	    num_rows = (int)(count/per_line) +
		(count % per_line > 0 ? 1 : 0);
	    tmp = (per_line * tab_height) + (num_rows * offset);
	    while( per_line > 1 && tmp > major_d )
	    {
		per_line--;
		num_rows = (int)(count/per_line) +
		    (count % per_line > 0 ? 1 : 0);
		tmp = (per_line * tab_height) + (num_rows * offset);
	    }
	    return( num_rows * tab_width );
	}
    }
}

static int
CalcGeometryMajor(XmTabBoxWidget tab, int minor_d)
{
    XmTabbedStackList  list = XmTabBox_tab_list(tab);
    int        count = _XmTabbedStackListCount(list), i, max, total, num_rows;
    int        tmp=0, offset = XmTabBox_tab_offset(tab), num_cols;
    XRectangle *geom;

    if( count == 0 )
    {
	/*
	 * We don't have any children so our major dimension is simply
	 * our shadow.
	 */
	return( tab->manager.shadow_thickness );
    }

    geom = XmTabBox__wanted(tab);
    /*
     * Here the taks is to calculate the major dimension given the
     * minor one.
     */
    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	switch( XmTabBox_tab_mode(tab) )
	{
	case XmTABS_BASIC:
	case XmTABS_OVERLAYED:
	case XmTABS_SCROLLED:
	default:
	    /*
	     * For this case the major dimension is simply the length
	     * of all the tabs in one row, reguardless of what the minor
	     * dimension is so lets figure this out.
	     */
	    for( i = 0, max = 0, total = 0; i < count; ++i )
	    {
		total += geom[i].width;
		AssignMax(max, (int)geom[i].width);
	    }

	    if( XmTabBox_uniform_tab_size(tab) )
	    {
		return( max * count );
	    }
	    return( total );
	case XmTABS_STACKED:
	case XmTABS_STACKED_STATIC:
	    /*
	     * This case is a little harder here. What we want to do is
	     * say given this height how wide do we want to be.  So the
	     * first thing we need to do is find the height of our rows
	     * so we know how many we can fit.
	     */
	    for( i = 0, max = 1; i < count; ++i )
	    {
		AssignMax(max, (int)geom[i].height);
		AssignMax(tmp, (int)geom[i].width);
	    }
	    num_rows = minor_d/ max;
	    AssignMax(num_rows, 1);

	    /*
	     * Now that we know how many rows we are allowed all we
	     * have to do is partition the tabs between the rows and
	     * add on room for the stagger (if needed).
	     */
	    num_cols = (int)(count/num_rows) + (count % num_rows > 0 ? 1 : 0);

	    total = num_cols * tmp + (num_rows - 1) * offset;

	    return( total );
	}
    }

    /* Vertical Orientation */

    switch( XmTabBox_tab_mode(tab) )
    {
    case XmTABS_BASIC:
    case XmTABS_OVERLAYED:
    case XmTABS_SCROLLED:
    default:
	/*
	 * For this case the major dimension is simply the length
	 * of all the tabs in one row, reguardless of what the minor
	 * dimension is so lets figure this out.
	 */
	for( i = 0, max = 0, total = 0; i < count; ++i )
	{
	    total += geom[i].height;
	    AssignMax(max, (int)geom[i].height);
	}

	if( XmTabBox_uniform_tab_size(tab) )
	{
	    return( max * count );
	}
	return( total );
    case XmTABS_STACKED:
    case XmTABS_STACKED_STATIC:
	/*
	 * This case is a little harder here. What we want to do is
	 * say given this height how wide do we want to be.  So the
	 * first thing we need to do is find the height of our rows
	 * so we know how many we can fit.
	 */
	for( i = 0, max = 1; i < count; ++i )
	{
	    AssignMax(max, (int)geom[i].width);
	    AssignMax(tmp, (int)geom[i].height);
	}
	num_rows = minor_d/ max;
	AssignMax(num_rows, 1);

	/*
	 * Now that we know how many rows we are allowed all we
	 * have to do is partition the tabs between the rows and
	 * add on room for the stagger (if needed).
	 */
	num_cols = (int)(count/num_rows) + (count % num_rows > 0 ? 1 : 0);

	total = num_cols * tmp + (num_rows - 1) * offset;

	return( total );
    }
}

/*
 * Function:
 *	CallCallbacks(tab, event, from, to)
 * Description:
 *	Calls the unselect and select callbacks for the XmTabBox
 * Input:
 *	tab   : XmTabBoxWidget - the tab box widget
 *	event : XEvent*        - passed through
 *	from  : int            - the old selected tab
 *	to    : int	       - the new selected tab
 * Output:
 *	None.
 */
static void
CallCallbacks(XmTabBoxWidget tab, XEvent *event, int from, int to)
{
    XmTabBoxCallbackStruct cbdata;

    cbdata.reason    = XmCR_TAB_UNSELECTED;
    cbdata.event     = event;
    cbdata.tab_index = from;
    cbdata.old_index = from;
    XtCallCallbackList((Widget)tab, XmTabBox_unselect_callback(tab), (XtPointer)&cbdata);

    cbdata.reason    = XmCR_TAB_SELECTED;
    cbdata.event     = event;
    cbdata.tab_index = to;
    cbdata.old_index = from;
    XtCallCallbackList((Widget)tab, XmTabBox_select_callback(tab), (XtPointer)&cbdata);
}

static void
SetRightGC(XmTabBoxWidget tab, GC gc, GC_type gc_type)
{
    XGCValues       values;
    XtGCMask        valueMask;
    static Pixel 	p = 0;
    static GC_type 	last = gc_normal;
    valueMask = GCForeground;
    values.foreground = 0;
    switch( gc_type )
      {
        case gc_normal:
          if (last != gc_normal)
            {
              values.foreground = p;
                XChangeGC(XtDisplay(tab),gc, valueMask, &values);
            }
          last = gc_normal;
          break;
        case gc_insensitive:
          if (last == gc_normal) p=values.foreground;
            values.foreground = tab->manager.bottom_shadow_color;
            XChangeGC(XtDisplay(tab),gc, valueMask, &values);
            last = gc_insensitive;
          break;
        case gc_shadow:
          if (last == gc_normal) p=values.foreground;
            values.foreground = tab->manager.top_shadow_color;
            XChangeGC(XtDisplay(tab),gc, valueMask, &values);
            last = gc_shadow;
          break;
        default:
          break;
      }
}

/* ARGSUSED */
static void
DrawLeftToRightTab(XmTabBoxWidget tab, XmTabAttributes info, GC gc,
		   Boolean have_pixmap, int pix_width, int pix_height,
		   int pix_depth, Boolean have_label, int label_width,
		   int label_height, XRectangle *clip)
{
    XRectangle draw;
    XmFontList font_list = XmTabBox_font_list(tab);
    int        x, y, tmp, spacing = XmTabBox_tab_label_spacing(tab);
    Boolean    sensitive;

    x = draw.x = clip->x;
    y = draw.y = clip->y;
    draw.width = clip->width;
    draw.height = clip->height;
    sensitive = XtIsSensitive((Widget)tab) && info->sensitive;

    /*
     * If we are drawing both a pixmap and a label then we want to
     * give priority to the pixmap and align the text in the
     * rest of the mess.
     */
    if( have_pixmap )
    {
	/*
	 * We have a pixmap, so lets figure out where we want the
	 * pixmap to be placed.
	 */
	switch( info->pixmap_placement )
	{
	case XmPIXMAP_TOP:
	case XmPIXMAP_BOTTOM:
	case XmPIXMAP_ONLY:
	default:
	    /*
	     * In these cases we want to look at the alignment of the
	     * tab to determine where we want to place the pixmap.
	     */
	    switch( info->label_alignment )
	    {
	    case XmALIGNMENT_BEGINNING:
		/*
		 * For this alignment we want to slam the pixmap
		 * to the left of the cliping area.
		 */
		x = clip->x;
		break;
	    case XmALIGNMENT_CENTER:
	    default:
		/*
		 * For this alignment we want to center the pixmap
		 * in the cliping area.
		 */
		x = clip->x + ((int)clip->width - pix_width)/ 2;
		break;
	    case XmALIGNMENT_END:
		/*
		 * for this alignment we want to slam the pixmap
		 * to the right of the cliping area.
		 */
		x = clip->x + (int)clip->width - pix_width;
		break;
	    }

	    /*
	     * Since the pixmap will affect the amount of vertical
	     * space that I have to place my text lets calculate
	     * what space I have left over (if any) for my text.
	     */
	    if( (tmp = (int)draw.height - (pix_height + spacing)) < 0 )
	    {
		draw.height = 0;
	    }
	    else
	    {
		draw.height = tmp;
	    }

	    /*
	     * OK now that we know how we want this pixmap positioned
	     * horizontally lets see what we want to do with it
	     * vertically.  If we have no label then just center it.
	     */
	    if( info->label_string == NULL )
	    {
		y = clip->y + ((int)clip->height - pix_height)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_TOP:
		default:
		    y = clip->y;
		    draw.y = y + pix_height + spacing;
		    break;
		case XmPIXMAP_BOTTOM:
		    y = clip->y + (int)clip->height - pix_height;
		    break;
		case XmPIXMAP_ONLY:
		    y = clip->y + ((int)clip->height - pix_height)/2;
		    break;
		}
	    }
	    break;
	case XmPIXMAP_LEFT:
	case XmPIXMAP_RIGHT:
	    /*
	     * For this case we want to slam the pixmap to the
	     * left of the clipping area and center it in the
	     * vertical space.
	     */
	    if( info->label_string == NULL )
	    {
		x = clip->x + ((int)clip->width - pix_width)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_LEFT:
		    x = clip->x;
		    draw.x = x + pix_width + spacing;
		    break;
		case XmPIXMAP_RIGHT:
		    x = clip->x + (int)clip->width - pix_width;
		    break;
		default:
		    break;
		}
	    }
	    y = clip->y + ((int)clip->height - pix_height)/2;

	    /*
	     * Since the pixmap will affect the amount of vertical
	     * space that I have to place my text lets calculate
	     * what space I have left over (if any) for my text.
	     */
	    if( (tmp = (int)draw.width - (pix_width + spacing)) < 0 )
	    {
		draw.width = 0;
	    }
	    else
	    {
		draw.width = tmp;
	    }
	    break;
	case XmPIXMAP_NONE:
	    /* notreached */
	    break;
	}
    }

    /*
     * Now that we know where we are going to draw the pixmap, lets
     * go ahead and draw the bloody thing.  We do a quick check here
     * to see if we are dealing with a bitmap or a pixmap to
     * see if we should be copying a plane or and area.  Also be sure
     * to to an XCopyArea if the depth of the pixmap matches the
     * depth of the canvas.
     */
    if( ValidPixmap(info->label_pixmap) &&
       info->pixmap_placement != XmPIXMAP_NONE )
    {
	if( pix_depth == 1 )
	{
	    XCopyPlane(XtDisplay(tab), info->label_pixmap,
		       XiCanvas(tab), gc, 0, 0, pix_width, pix_height,
		       x, y, 1);
	}
	else if( pix_depth == XmTabBox__canvas(tab)->core.depth )
	{
	    XCopyArea(XtDisplay(tab), info->label_pixmap,
		      XiCanvas(tab), gc, 0, 0, pix_width, pix_height,
		      x, y);
	}
    }
    if( !sensitive )
    {
	/*
	 * Since we are working with a non-sensitve tab, then we need to
	 * stipple the the pixmap, and the only way to do that is
	 * to copy a chess board pattern (the gray stipple) over the pixmap
	 * we just drew.
	 */
	SetStippledGC(XtDisplay(tab), tab->manager.background_GC,
		      XmTabBox__gray_stipple(tab));
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.background_GC, x, y, pix_width,
		       pix_height);
	RemoveStipple(XtDisplay(tab), tab->manager.background_GC);
    }

    /*
     * Now that we are done drawing the pixmap (if we have one) we
     * need to draw the textual label (if we have one).
     */
    if( have_label )
    {
	/*
	 * First if we have a pixmap we need to reset our clipping
	 * area to account for the space the pixmap took up.
	 */
	if( have_pixmap )
	{
	    XSetClipRectangles(XtDisplay(tab), gc, 0, 0, &draw, 1,
			       YXBanded);
	}

	/*
	 * Now that we know the rectangle that we are going to
	 * draw the text in lets calculate that x and y position
	 * at which we are going to draw the text.  For text it is
	 * real easy we are always going to center the text in the
	 * vertical space and align it according to the alignment
	 * specfified.
	 */
	y = draw.y + ((int)draw.height - label_height)/2;

	/*
	 * Now that we know where the text is going to go, lets
	 * go ahead and draw it.
	 */

	if( !sensitive )
	{
	  /*Draw shadow for insensitive text*/
	  SetRightGC(tab, gc, gc_shadow);
	  XmStringDraw(XtDisplay(tab), XiCanvas(tab), font_list,
	  	info->label_string, gc, draw.x+1, (Position)y+1, draw.width,
	  	info->label_alignment, info->string_direction,
	  	NULL);
	  SetRightGC(tab, gc, gc_insensitive);
	}
	else
	{
	  SetRightGC(tab, gc, gc_normal);
	}
	XmStringDraw(XtDisplay(tab), XiCanvas(tab), font_list,
		     info->label_string, gc, draw.x, (Position)y, draw.width,
		     info->label_alignment, info->string_direction,
		     NULL);
    }
}

/*
 * Function:
 *	DrawRightToLeftTab(tab, info, gc, have_pixmap, pix_width, pix_height,
 *			   pix_depth, have_label, label_width, label_height,
 *			   clip);
 * Description:
 *	Draw the pixmap and text for a tab withing the clipping rectangle
 *	passed in as "clip".
 * Input:
 *	tab          : XmTabBoxWidget - the tab box that drawing
 *	gc           : GC             - the GC to draw with
 *	have_pixmap  : Boolean        - do we need to draw a pixmap ?
 *	pix_width,
 *      pix_height,
 *      pix_depth    : int            - pixmap information
 *	have_label   : Boolean        - do we need to draw a label string ?
 *	label_width,
 *	label_height : int            - label string information
 *	clip         : XRectangle*    - the clip rectangle to draw into
 * Output:
 *	None.
 */
/* ARGSUSED */
static void
DrawRightToLeftTab(XmTabBoxWidget tab, XmTabAttributes info, GC gc,
		   Boolean have_pixmap, int pix_width, int pix_height,
		   int pix_depth, Boolean have_label, int label_width,
		   int label_height, XRectangle *clip, Boolean selected)
{
    XRectangle draw;
    XmFontList font_list = XmTabBox_font_list(tab);
    int        x, y, tmp, spacing = XmTabBox_tab_label_spacing(tab);
    XImage     *src_ximage, *dst_ximage;
    Pixmap     bitmap;
    Boolean    sensitive;

    x = draw.x = clip->x;
    y = draw.y = clip->y;
    draw.width = clip->width;
    draw.height = clip->height;
    sensitive = XtIsSensitive((Widget)tab) && info->sensitive;

    /*
     * First lets check if we have a pixmap to draw and if we do
     * lets rotate and draw it.
     */
    if( have_pixmap )
    {
	/*
	 * We do have a pixmap so we will start by rotating the pixmap
	 * to the correct orientation.
	 */
	src_ximage = NULL;
	if( !XmTabBox_use_image_cache(tab) ||
	    (dst_ximage = CachePixmap(tab,info)) == NULL )
	{
	    src_ximage = XGetImage(XtDisplay(tab), info->label_pixmap,
				   0, 0, pix_width, pix_height,
				   AllPlanes, XYPixmap);
	    dst_ximage = XiRotateImage(tab, src_ximage,
				    XiTabDegree(XmTabBox_tab_orientation(tab)));
	    if( XmTabBox_use_image_cache(tab) )
	    {
		CachePixmap(tab,info) = dst_ximage;
	    }
	}

	/*
	 * Now that we have the rotated image lets snarf the width and height
	 * back into local cache.
	 */
	pix_width = XImageWidth(dst_ximage);
	pix_height = XImageHeight(dst_ximage);

	/*
	 * Now lets look at the pixmap placement to determine where we want
	 * to place the pixmap.
	 */
	switch( info->pixmap_placement )
	{
	case XmPIXMAP_TOP:
	case XmPIXMAP_BOTTOM:
	case XmPIXMAP_ONLY:
	default:
	    /*
	     * Now that we have rotate the ximage lets cache the width
	     * and hieght of that image away for use.
	     */
	    pix_width = XImageWidth(dst_ximage);
	    pix_height = XImageHeight(dst_ximage);

	    /*
	     * In these cases we want to look at the alignment of the
	     * tab to determine where we want to place the pixmap
	     * vertically.
	     */
	    switch( info->label_alignment )
	    {
	    case XmALIGNMENT_BEGINNING:
		/*
		 * For this alignement we want to slam the pixmap
		 * to the top of the clipping area.
		 */
		x = clip->x + (int)clip->width - pix_width;
		break;
	    case XmALIGNMENT_CENTER:
	    default:
		/*
		 * For this case we want to center the pixmap in
		 * the vertical space.
		 */
		x = clip->x + ((int)clip->width - pix_width)/2;
		break;
	    case XmALIGNMENT_END:
		/*
		 * For this alignment we want to slam the pixmap
		 * to the bottom of the clipping area.
		 */
		x = clip->x;
		break;
	    }

	    /*
	     * Since the pixmap will affect the amount of vertical
	     * space to place the text lets calculate what space is
	     * left for the text.
	     */
	    if( (tmp = (int)draw.height - (pix_height + spacing)) < 0 )
	    {
		draw.height = 0;
	    }
	    else
	    {
		draw.height = tmp;
	    }

	    /*
	     * Ok now that we have positioned the pixmap horizontally
	     * lets see what we want to do with it vertically.  If
	     * we have not label then center it.
	     */
	    if( info->label_string == NULL )
	    {
		y = clip->y + ((int)clip->height - pix_height)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_TOP:
		default:
		    y = clip->y + (int)clip->height - pix_height;
		    break;
		case XmPIXMAP_BOTTOM:
		    y = clip->y;
		    draw.y = y+ pix_height + spacing;
		    break;
		case XmPIXMAP_ONLY:
		    y = clip->y + ((int)clip->height - pix_height)/2;
		    break;
		}
	    }
	    break;
	case XmPIXMAP_LEFT:
	case XmPIXMAP_RIGHT:
	    /*
	     * For this case we want to slam the pixmap to either the
	     * top or bottom of the clipping area and center it in
	     * the horizontal space.
	     */
	    if( info->label_string == NULL )
	    {
		x = clip->x + ((int)clip->width - pix_width)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_LEFT:
		    x = clip->x + (int)clip->width - pix_width;
		    break;
		case XmPIXMAP_RIGHT:
		    x = clip->x;
		    draw.x = x + pix_width + spacing;
		    break;
		default:
		    break;
		}
	    }
	    y = clip->y + ((int)clip->height - pix_width)/2;

	    /*
	     * Since the pixmap will affect the amount of
	     * vertical space that the text will have lets
	     * calculate what space is left.
	     */
	    if( (tmp = (int)draw.width - (pix_width + spacing)) < 0 )
	    {
		draw.width = 0;
	    }
	    else
	    {
		draw.width = tmp;
	    }
	    break;
	case XmPIXMAP_NONE:
	    /* notreached */
	    break;
	}

	/*
	 * Now that we know where the image is supposed to go, lets
	 * draw the thing.
	 */
	XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, pix_width, pix_height);

	if( !sensitive )
	{
	    /*
	     * Since we are working with a non-sensitive tab, then we need to
	     * stipple the the pixmap, and the only way to do that is
	     * to copy a chess board pattern (the gray stipple) over the pixmap
	     * we just drew.
	     */
	    SetStippledGC(XtDisplay(tab), tab->manager.background_GC,
			  XmTabBox__gray_stipple(tab));
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.background_GC, x, y, pix_width,
			   pix_height);
	    RemoveStipple(XtDisplay(tab), tab->manager.background_GC);
	}

	/*
	 * We are done with the XImages now so lets destroy the
	 * ximages we used to rotate the pixmap.
	 */
	if( src_ximage != NULL ) XDestroyImage(src_ximage);
	if( !XmTabBox_use_image_cache(tab) && dst_ximage != NULL )
	{
	    XDestroyImage(dst_ximage);
	}
    }

    /*
     * Now that we have drawn the pixmap it is time to deal with the
     * label string.
     */
    if( !have_label || info->label_string == NULL ) return;

    /*
     * If we have a pixmap then the clipping area was modified so
     * we need to set the clip rectangle to that for the text.
     */
    if( have_pixmap )
    {
	XSetClipRectangles(XtDisplay(tab), gc, 0, 0, &draw, 1, YXBanded);
    }

    /*
     * Now we have to render and rotate the label string.  To do this
     * we need to first set up a few things like the pixmap to render
     * into and GC used to draw into a bitmap.  We will start by
     * making sure that we have a pixmap large enough to
     * accomidate the string we are about to render.
     */
    src_ximage = NULL;
    if( !XmTabBox_use_image_cache(tab) ||
    	(sensitive != CacheSensitive(tab,info)) ||
        (dst_ximage = CacheLabel(tab, info)) == NULL )
    {
	if( ValidPixmap(XmTabBox__bitmap(tab)) &&
	   (XmTabBox__bitmap_width(tab) < label_width ||
	    XmTabBox__bitmap_height(tab) < label_height) )
	{
	    /*
	     * As it turns out our pixmap is not large enough to
	     * hold the rendered text. So we will destroy the
	     * current pixmap so that we will allocate a pixmap
	     * of the correct size below.
	     */
	    XFreePixmap(XtDisplay(tab), XmTabBox__bitmap(tab));
	    XmTabBox__bitmap(tab) = XmUNSPECIFIED_PIXMAP;
	}

	/*
	 * If we do not have a bitmap to render the text into
	 * lets create one now.
	 */
	if( !ValidPixmap(XmTabBox__bitmap(tab)) )
	{
	    XmTabBox__bitmap(tab) = XCreatePixmap(XtDisplay(tab), XiCanvas(tab),
						 label_width, label_height, 1);

	    XmTabBox__bitmap_width(tab) = label_width;
	    XmTabBox__bitmap_height(tab) = label_height;
	}
	bitmap = XmTabBox__bitmap(tab);

	/*
	 * Now that we have a bitmap to render the text into lets make sure
	 * that we have GC that we can use to render the text.  We need two
	 * GC, one to clear the pixmap the other to draw into it.
	 */
	if( XmTabBox__zero_GC(tab) == NULL )
	{
	    /*
	     * We do not yet have either of the GC created so lets go
	     * ahead and create them now.
	     */
	    XFontStruct *font = NULL;
	    XGCValues   gcValues;
	    unsigned long gcMask;

	    gcValues.background = tab->core.background_pixel;
	    gcValues.foreground = tab->core.background_pixel;
	    XmTabBox__zero_GC(tab) = XCreateGC(XtDisplay(tab), bitmap,
					      GCForeground | GCBackground,
					      &gcValues);

	    XmeRenderTableGetDefaultFont(font_list, &font);
	    gcValues.foreground = 1;
	    gcValues.background = 0;
	    gcMask = GCForeground | GCBackground;
	    if (font) {
	        gcValues.font = font->fid;
	        gcMask |= GCFont;
	    }
	    XmTabBox__one_GC(tab) = XCreateGC(XtDisplay(tab), bitmap, gcMask,
	                                      &gcValues);
	}

	/*
	 * Now we have all that we need to render the label string, so now
	 * lets do it.  What we need to do if first zero the pixmap and
	 * then render the text.
	 */
	XFillRectangle(XtDisplay(tab), bitmap, XmTabBox__zero_GC(tab),
		       0, 0, label_width, label_height);
	if( !sensitive )
	{
	  /*Draw shadow for insensitive text*/
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_shadow);
	  XmStringDraw(XtDisplay(tab), bitmap, font_list, info->label_string,
	  XmTabBox__one_GC(tab), -1, -1, (Dimension)label_width,
	  info->label_alignment, info->string_direction, NULL);
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_insensitive);
	}
	else
	{
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_normal);
	}
	XmStringDraw(XtDisplay(tab), bitmap, font_list, info->label_string,
		     XmTabBox__one_GC(tab), 0, 0, (Dimension)label_width,
		     info->label_alignment, info->string_direction, NULL);

	/*
	 * Now that the label string is rendered we need to grab it back
	 * into an XImage so that we can rotate it.
	 */
	src_ximage = XGetImage(XtDisplay(tab), bitmap, 0, 0, label_width,
			       label_height, 1, XYPixmap);
	dst_ximage = XiRotateImage(tab, src_ximage,
				   XiTabDegree(XmTabBox_tab_orientation(tab)));

	if( XmTabBox_use_image_cache(tab) )
	{
	    CacheLabel(tab, info) = dst_ximage;
	    CacheSensitive(tab,info) = sensitive;
	}
    }

    /*
     * Now that the text is rotated lets snarf the label width and height
     * back from the image.
     */
    label_width = XImageWidth(dst_ximage);
    label_height = XImageHeight(dst_ximage);

    /*
     * Now we need to find out where we want to but the text.  We know that
     * we want to center the text in the horizontal space that we have left,
     * then we want to use the alignment value to determine what to
     * do with the vertical space.
     */
    y = draw.y + ((int)draw.height - label_height)/2;

    switch( info->label_alignment )
    {
    case XmALIGNMENT_BEGINNING:
	/*
	 * Here we want to slam the text to the top of the clipping
	 * area.
	 */
	x = draw.x + (int)draw.width - label_width;
	break;
    case XmALIGNMENT_CENTER:
    default:
	/*
	 * Here we want to center the text in the clipping area.
	 */
	x = draw.x + ((int)draw.width - label_width)/2;
	break;
    case XmALIGNMENT_END:
	/*
	 * Here we want to slam the text to the bottom of the
	 * clipping area.
	 */
	x = draw.x;
	break;
    }

    /*
     * Now that we know where we want the text lets push that
     * XImage over the wire.  But first we need to check if we should
     * mask out the text so that the background pixmap show through.
     */
    if( ValidPixmap(tab->core.background_pixmap) ||
        XiBackgroundSpecified(info) ||
        (selected && XiSelectSpecified(tab)) )
    {
	/*
	 * It appears that we have a background pixmap so what we
	 * want to do is set the clip mask such that we only draw the
	 * text and not the rectangle around it.
	 *
	 * What we are going to do is create a bitmap and draw the text
	 * into this pixmap to create a GC clipping mask.
	 */
	Pixmap pix = XCreatePixmap(XtDisplay(tab),
				   (XtIsRealized((Widget)tab)
				    ? XtWindow(tab)
				    : RootWindowOfScreen(XtScreen(tab))),
				   label_width, label_height, 1);

	/*
	 * Left first clear out the clipping mask and then draw the text
	 * into it.
	 */
	XFillRectangle(XtDisplay(tab), pix, XmTabBox__zero_GC(tab), 0, 0,
		       label_width, label_height);
	XPutImage(XtDisplay(tab), pix, XmTabBox__one_GC(tab), dst_ximage, 0, 0,
		  0, 0, label_width, label_height);

	/*
	 * Now that we have the clipping mask lets set the mask to the GC
	 * and then put the image that is the text to the display using this
	 * clipping mask.
	 */
	XSetClipMask(XtDisplay(tab), gc, pix);
	XSetClipOrigin(XtDisplay(tab), gc, x, y);
	XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, label_width, label_height);

	/*
	 * We are done with this cliping mask so lets destroy the pixmap.
	 */
	XFreePixmap(XtDisplay(tab), pix);
    }
    else
    {
	XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, label_width, label_height);
    }

    /*
     * Now that we are done with the XImages used for rotating the
     * text lets delete them.
     */
    if( src_ximage != NULL ) XDestroyImage(src_ximage);
    if( !XmTabBox_use_image_cache(tab) && dst_ximage != NULL )
    {
	XDestroyImage(dst_ximage);
    }
}

/*
 * Function:
 *	DrawBottomToTopTab(tab, info, gc, have_pixmap, pix_width, pix_height,
 *			   pix_depth, have_label, label_width, label_height,
 *			   clip);
 * Description:
 *	Draw the pixmap and text for a tab withing the clipping rectangle
 *	passed in as "clip".
 * Input:
 *	tab          : XmTabBoxWidget - the tab box that drawing
 *	gc           : GC             - the GC to draw with
 *	have_pixmap  : Boolean        - do we need to draw a pixmap ?
 *	pix_width,
 *      pix_height,
 *      pix_depth    : int            - pixmap information
 *	have_label   : Boolean        - do we need to draw a label string ?
 *	label_width,
 *	label_height : int            - label string information
 *	clip         : XRectangle*    - the clip rectangle to draw into
 * Output:
 *	None.
 */
static void
DrawBottomToTopTab(XmTabBoxWidget tab, XmTabAttributes info, GC gc,
		   Boolean have_pixmap, int pix_width, int pix_height,
		   int pix_depth, Boolean have_label, int label_width,
		   int label_height, XRectangle *clip, Boolean selected)
{
    DrawVerticalTab(tab, info, gc, have_pixmap, pix_width, pix_height,
		    pix_depth, have_label, label_width, label_height, clip,
		    False, selected);
}

/*
 * Function:
 *	DrawTopToBottomTab(tab, info, gc, have_pixmap, pix_width, pix_height,
 *			   pix_depth, have_label, label_width, label_height,
 *			   clip);
 * Description:
 *	Draw the pixmap and text for a tab withing the clipping rectangle
 *	passed in as "clip".
 * Input:
 *	tab          : XmTabBoxWidget - the tab box that drawing
 *	gc           : GC             - the GC to draw with
 *	have_pixmap  : Boolean        - do we need to draw a pixmap ?
 *	pix_width,
 *      pix_height,
 *      pix_depth    : int            - pixmap information
 *	have_label   : Boolean        - do we need to draw a label string ?
 *	label_width,
 *	label_height : int            - label string information
 *	clip         : XRectangle*    - the clip rectangle to draw into
 * Output:
 *	None.
 */
static void

DrawTopToBottomTab(XmTabBoxWidget tab, XmTabAttributes info, GC gc,
		   Boolean have_pixmap, int pix_width, int pix_height,
		   int pix_depth, Boolean have_label, int label_width,
		   int label_height, XRectangle *clip, Boolean selected)
{
    DrawVerticalTab(tab, info, gc, have_pixmap, pix_width, pix_height,
		    pix_depth, have_label, label_width, label_height, clip,
		    True, selected);
}

/*
 * Function:
 *	DrawVerticalTab(tab, info, gc, have_pixmap, pix_width, pix_height,
 *			pix_depth, have_label, label_width, label_height,
 *			clip, top_to_bottom);
 * Description:
 *	Draw the pixmap and text for a tab withing the clipping rectangle
 *	passed in as "clip".
 * Input:
 *	tab          : XmTabBoxWidget - the tab box that drawing
 *	gc           : GC             - the GC to draw with
 *	have_pixmap  : Boolean        - do we need to draw a pixmap ?
 *	pix_width,
 *      pix_height,
 *      pix_depth    : int            - pixmap information
 *	have_label   : Boolean        - do we need to draw a label string ?
 *	label_width,
 *	label_height : int            - label string information
 *	clip         : XRectangle*    - the clip rectangle to draw into
 *	top_to_bottom : Boolean       - drawing top to bottom(True) or
 *				        bottom to top(False)
 * Output:
 *	None.
 */
/* ARGSUSED */
static void
DrawVerticalTab(XmTabBoxWidget tab, XmTabAttributes info, GC gc,
		Boolean have_pixmap, int pix_width, int pix_height,
		int pix_depth, Boolean have_label, int label_width,
		int label_height, XRectangle *clip, Boolean top_to_bottom,
		Boolean selected)
{
    XRectangle draw;
    XmFontList font_list = XmTabBox_font_list(tab);
    int        x, y, tmp, spacing = XmTabBox_tab_label_spacing(tab);
    XImage     *src_ximage, *dst_ximage;
    Pixmap     bitmap;
    Boolean    sensitive;

    x = draw.x = clip->x;
    y = draw.y = clip->y;
    draw.width = clip->width;
    draw.height = clip->height;
    sensitive = XtIsSensitive((Widget)tab) && info->sensitive;

    /*
     * First lets check if we have a pixmap to draw and if we do
     * lets rotate and draw it.
     */
    if( have_pixmap )
    {
	/*
	 * We do have a pixmap so we will start by rotating the pixmap
	 * to the correct orientation.
	 */
	src_ximage = NULL;
	if( !XmTabBox_use_image_cache(tab) ||
	    (dst_ximage = CachePixmap(tab,info)) == NULL )
	{
	    src_ximage = XGetImage(XtDisplay(tab), info->label_pixmap,
				   0, 0, pix_width, pix_height,
				   AllPlanes, XYPixmap);
	    dst_ximage = XiRotateImage(tab, src_ximage,
				  XiTabDegree(XmTabBox_tab_orientation(tab)));

	    if( XmTabBox_use_image_cache(tab) )
	    {
		CachePixmap(tab,info) = dst_ximage;
	    }
	}

	/*
	 * Now that we have the rotated image lets snarf the width and height
	 * back into local cache.
	 */
	pix_width = XImageWidth(dst_ximage);
	pix_height = XImageHeight(dst_ximage);

	/*
	 * Now lets look at the pixmap placement to determine where we want
	 * to place the pixmap.
	 */
	switch( info->pixmap_placement )
	{
	case XmPIXMAP_TOP:
	case XmPIXMAP_BOTTOM:
	case XmPIXMAP_ONLY:
	default:
	    /*
	     * Now that we have rotate the ximage lets cache the width
	     * and hieght of that image away for use.
	     */
	    pix_width = XImageWidth(dst_ximage);
	    pix_height = XImageHeight(dst_ximage);

	    /*
	     * In these cases we want to look at the alignment of the
	     * tab to determine where we want to place the pixmap
	     * vertically.
	     */
	    switch( info->label_alignment )
	    {
	    case XmALIGNMENT_BEGINNING:
		/*
		 * For this alignement we want to slam the pixmap
		 * to the top of the clipping area.
		 */
		if( top_to_bottom )
		{
		    y = clip->y;
		}
		else
		{
		    y = clip->y + (int)clip->height - pix_height;
		}
		break;
	    case XmALIGNMENT_CENTER:
	    default:
		/*
		 * For this case we want to center the pixmap in
		 * the vertical space.
		 */
		y = clip->y + ((int)clip->height - pix_height)/2;
		break;
	    case XmALIGNMENT_END:
		/*
		 * For this alignment we want to slam the pixmap
		 * to the bottom of the clipping area.
		 */
		if( top_to_bottom )
		{
		    y = clip->y + (int)clip->height - pix_height;
		}
		else
		{
		    y = clip->y;
		}
		break;
	    }

	    /*
	     * Since the pixmap will affect the amount of horizontal
	     * space to place the text lets calculate what space is
	     * left for the text.
	     */
	    if( (tmp = (int)draw.width - (pix_width + spacing)) < 0 )
	    {
		draw.width = 0;
	    }
	    else
	    {
		draw.width = tmp;
	    }

	    /*
	     * Ok now that we have positioned the pixmap vertically
	     * lets see what we want to do with it horizontally.  If
	     * we have not label then center it.
	     */
	    if( info->label_string == NULL )
	    {
		x = clip->x + ((int)clip->width - pix_width)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_TOP:
		default:
		    if( top_to_bottom )
		    {
			x = clip->x + (int)clip->width - pix_width;
		    }
		    else
		    {
			x = clip->x;
			draw.x = x + pix_width + spacing;
		    }
		    break;
		case XmPIXMAP_BOTTOM:
		    if( top_to_bottom )
		    {
			x = clip->x;
			draw.x = x + pix_width + spacing;
		    }
		    else
		    {
			x = clip->x + (int)clip->width - pix_width;
		    }
		    break;
		case XmPIXMAP_ONLY:
		    x = clip->x + ((int)clip->width - pix_width)/2;
		    break;
		}
	    }
	    break;
	case XmPIXMAP_LEFT:
	case XmPIXMAP_RIGHT:
	    /*
	     * For this case we want to slam the pixmap to either the
	     * top or bottom of the clipping area and center it in
	     * the horizontal space.
	     */
	    if( info->label_string == NULL )
	    {
		y = clip->y + ((int)clip->height - pix_height)/2;
	    }
	    else
	    {
		switch( info->pixmap_placement )
		{
		case XmPIXMAP_LEFT:
		    if( top_to_bottom )
		    {
			y = clip->y;
			draw.y = y + pix_height + spacing;
		    }
		    else
		    {
			y = clip->y + (int)clip->height - pix_height;
		    }
		    break;
		case XmPIXMAP_RIGHT:
		    if( top_to_bottom )
		    {
			y = clip->y + (int)clip->height - pix_height;
		    }
		    else
		    {
			y = clip->y;
			draw.y = y + pix_height + spacing;
		    }
		    break;
		default:
		    break;
		}
	    }
	    x = clip->x + ((int)clip->width - pix_width)/2;

	    /*
	     * Since the pixmap will affect the amount of
	     * vertical space that the text will have lets
	     * calculate what space is left.
	     */
	    if( (tmp = (int)draw.height - (pix_height + spacing)) < 0 )
	    {
		draw.height = 0;
	    }
	    else
	    {
		draw.height = tmp;
	    }
	    break;
	case XmPIXMAP_NONE:
	    /* notreached */
	    break;
	}

	/*
	 * Now that we know where the image is supposed to go, lets
	 * draw the thing.
	 */
	XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, pix_width, pix_height);

	if( !sensitive )
	{
	    /*
	     * Since we are working with a non-sensitive tab, then we need to
	     * stipple the the pixmap, and the only way to do that is
	     * to copy a chess board pattern (the gray stipple) over the pixmap
	     * we just drew.
	     */
	    SetStippledGC(XtDisplay(tab), tab->manager.background_GC,
			  XmTabBox__gray_stipple(tab));
	    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			   tab->manager.background_GC, x, y, pix_width,
			   pix_height);
	    RemoveStipple(XtDisplay(tab), tab->manager.background_GC);
	}

	/*
	 * We are done with the XImages now so lets destroy the
	 * ximages we used to rotate the pixmap.
	 */
	if( src_ximage != NULL ) XDestroyImage(src_ximage);
	if( !XmTabBox_use_image_cache(tab) && dst_ximage != NULL )
	{
	    XDestroyImage(dst_ximage);
	}
    }

    /*
     * Now that we have drawn the pixmap it is time to deal with the
     * label string.
     */
    if( !have_label || info->label_string == NULL ) return;

    /*
     * If we have a pixmap then the clipping area was modified so
     * we need to set the clip rectangle to that for the text.
     */
    if( have_pixmap )
    {
	XSetClipRectangles(XtDisplay(tab), gc, 0, 0, &draw, 1, YXBanded);
    }

    src_ximage = NULL;
    if( !XmTabBox_use_image_cache(tab) ||
    	(sensitive != CacheSensitive(tab,info)) ||
        (dst_ximage = CacheLabel(tab, info)) == NULL )
    {
	/*
	 * Now we have to render and rotate the label string.  To do this
	 * we need to first set up a few things like the pixmap to render
	 * into and GC used to draw into a bitmap.  We will start by
	 * making sure that we have a pixmap large enough to
	 * accomidate the string we are about to render.
	 */
	if( ValidPixmap(XmTabBox__bitmap(tab)) &&
	   (XmTabBox__bitmap_width(tab) < label_width ||
	    XmTabBox__bitmap_height(tab) < label_height) )
	{
	    /*
	     * As it turns out our pixmap is not large enough to
	     * hold the rendered text. So we will destroy the
	     * current pixmap so that we will allocate a pixmap
	     * of the correct size below.
	     */
	    XFreePixmap(XtDisplay(tab), XmTabBox__bitmap(tab));
	    XmTabBox__bitmap(tab) = XmUNSPECIFIED_PIXMAP;
	}

	/*
	 * If we do not have a bitmap to render the text into
	 * lets create one now.
	 */
	if( !ValidPixmap(XmTabBox__bitmap(tab)) )
	{
	    XmTabBox__bitmap(tab) = XCreatePixmap(XtDisplay(tab), XiCanvas(tab),
						 label_width, label_height, 1);
	    XmTabBox__bitmap_width(tab) = label_width;
	    XmTabBox__bitmap_height(tab) = label_height;
	}
	bitmap = XmTabBox__bitmap(tab);

	/*
	 * Now that we have a bitmap to render the text into lets make sure
	 * that we have GC that we can use to render the text.  We need two
	 * GC, one to clear the pixmap the other to draw into it.
	 */
	if( XmTabBox__zero_GC(tab) == NULL )
	{
	    /*
	     * We do not yet have either of the GC created so lets go
	     * ahead and create them now.
	     */
	    XFontStruct *font = NULL;
	    XGCValues   gcValues;
	    unsigned long gcMask;

	    gcValues.background = tab->core.background_pixel;
	    gcValues.foreground = tab->core.background_pixel;
	    XmTabBox__zero_GC(tab) = XCreateGC(XtDisplay(tab), bitmap,
					      GCForeground | GCBackground,
					      &gcValues);

	    XmeRenderTableGetDefaultFont(font_list, &font);
	    gcValues.foreground = 1;
	    gcValues.background = 0;
	    gcMask = GCForeground | GCBackground;
	    if (font) {
	        gcValues.font = font->fid;
		gcMask |= GCFont;
	    }
	    XmTabBox__one_GC(tab) = XCreateGC(XtDisplay(tab), bitmap, gcMask,
	                                      &gcValues);
	}

	/*
	 * Now we have all that we need to render the label string, so now
	 * lets do it.  What we need to do if first zero the pixmap and
	 * then render the text.
	 */
	XFillRectangle(XtDisplay(tab), bitmap, XmTabBox__zero_GC(tab),
		       0, 0, label_width, label_height);
	if( !sensitive )
	{
	  /*Draw shadow for insensitive text*/
	  /*text will be rotated below but shadow should be always under the text*/
	  if (XmTabBox_tab_orientation(tab)==XmTABS_TOP_TO_BOTTOM){
	    x=1; y=-1;
	  }
	  else
	  {
	    x=-1; y=1;
	  }
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_shadow);
	  XmStringDraw(XtDisplay(tab), bitmap, font_list, info->label_string,
	  		XmTabBox__one_GC(tab), x, y, (Dimension)label_width,
			info->label_alignment, info->string_direction, NULL);
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_insensitive);
	}
	else
	{
	  SetRightGC(tab, XmTabBox__one_GC(tab), gc_normal);
	}
	XmStringDraw(XtDisplay(tab), bitmap, font_list, info->label_string,
		     XmTabBox__one_GC(tab), 0, 0, (Dimension)label_width,
		     info->label_alignment, info->string_direction, NULL);

	/*
	 * Now that the label string is rendered we need to grab it back
	 * into an XImage so that we can rotate it.
	 */
	src_ximage = XGetImage(XtDisplay(tab), bitmap, 0, 0, label_width,
			       label_height, 1, XYPixmap);
	dst_ximage = XiRotateImage(tab, src_ximage,
				   XiTabDegree(XmTabBox_tab_orientation(tab)));

	if( XmTabBox_use_image_cache(tab) )
	{
	    CacheLabel(tab, info) = dst_ximage;
	    CacheSensitive(tab,info) = sensitive;
	}
    }

    /*
     * Now that the text is rotated lets snarf the label width and height
     * back from the image.
     */
    if (dst_ximage)
    {
        label_width = XImageWidth(dst_ximage);
        label_height = XImageHeight(dst_ximage);
    }
    else
    {
        label_width = 0;
        label_height = 0;
    }

    /*
     * Now we need to find out where we want to but the text.  We know that
     * we want to center the text in the horizontal space that we have left,
     * then we want to use the alignment value to determine what to
     * do with the vertical space.
     */
    x = draw.x + ((int)draw.width - label_width)/2;

    switch( info->label_alignment )
    {
    case XmALIGNMENT_BEGINNING:
	/*
	 * Here we want to slam the text to the top of the clipping
	 * area.
	 */
	if( top_to_bottom )
	{
	    y = draw.y;
	}
	else
	{
	    y = draw.y + (int)draw.height - label_height;
	}
	break;
    case XmALIGNMENT_CENTER:
    default:
	/*
	 * Here we want to center the text in the clipping area.
	 */
	y = draw.y + ((int)draw.height - label_height)/2;
	break;
    case XmALIGNMENT_END:
	/*
	 * Here we want to slam the text to the bottom of the
	 * clipping area.
	 */
	if( top_to_bottom )
	{
	    y = draw.y + (int)draw.height - label_height;
	}
	else
	{
	    y = draw.y;
	}
	break;
    }

    /*
     * Now that we know where we want the text lets push that
     * XImage over the wire.  But first we need to check if we should
     * mask out the text so that the background pixmap show through.
     */
    if( ValidPixmap(tab->core.background_pixmap) ||
        XiBackgroundSpecified(info) ||
        (selected && XiSelectSpecified(tab)) )
    {
	/*
	 * It appears that we have a background pixmap so what we
	 * want to do is set the clip mask such that we only draw the
	 * text and not the rectangle around it.
	 *
	 * What we are going to do is create a bitmap and draw the text
	 * into this pixmap to create a GC clipping mask.
	 */
	Pixmap pix = XCreatePixmap(XtDisplay(tab),
				   (XtIsRealized((Widget)tab)
				    ? XtWindow(tab)
				    : RootWindowOfScreen(XtScreen(tab))),
				   label_width, label_height, 1);

	/*
	 * Left first clear out the clipping mask and then draw the text
	 * into it.
	 */
	XFillRectangle(XtDisplay(tab), pix, XmTabBox__zero_GC(tab), 0, 0,
		       label_width, label_height);
	XPutImage(XtDisplay(tab), pix, XmTabBox__one_GC(tab), dst_ximage, 0, 0,
		  0, 0, label_width, label_height);

	/*
	 * Now that we have the clipping mask lets set the mask to the GC
	 * and then put the image that is the text to the display using this
	 * clipping mask.
	 */
	XSetClipMask(XtDisplay(tab), gc, pix);
	XSetClipOrigin(XtDisplay(tab), gc, x, y);
	XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, label_width, label_height);

	/*
	 * We are done with this cliping mask so lets destroy the pixmap.
	 */
	XFreePixmap(XtDisplay(tab), pix);
    }
    else
    {
        if (dst_ximage)
            XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,
		  x, y, label_width, label_height);
    }

    /*
     * Now that we are done with the XImages used for rotating the
     * text lets delete them.
     */
    if( src_ximage != NULL ) XDestroyImage(src_ximage);
    if( !XmTabBox_use_image_cache(tab) && dst_ximage != NULL )
    {
	XDestroyImage(dst_ximage);
    }
}

static void

FillRoundedTab(XmTabBoxWidget tab, GC gc, XiTabRect *geometry, XmTabEdge edge)
{
    XRectangle rect[2];
    int        x = geometry->x, y = geometry->y,
               width = geometry->width, height = geometry->height, side;

    side = XmTabBox__corner_size(tab);
    if( width < height )
    {
	AssignMin(side, (width/2));
    }
    else
    {
	AssignMin(side, (height/2));
    }

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    rect[0].x = x;
	    rect[0].y = y;
	    rect[0].width = width;
	    rect[0].height = height - side;
	    rect[1].x = x + side;
	    rect[1].y = y + height - side;
	    rect[1].width = width - (2 * side);
	    rect[1].height = side;

	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x, y + height - side, side, side, XiQUAD_3);
	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x + width - side, y + height - side, side,
			 side, XiQUAD_4);
	}
	else
	{
	    rect[0].x = x;
	    rect[0].y = y;
	    rect[0].width = width - side;
	    rect[0].height = height;
	    rect[1].x = x + width - side;
	    rect[1].y = y + side;
	    rect[1].width = side;
	    rect[1].height = height - (2 * side);

	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x + width - side, y, side,
			 side, XiQUAD_1);
	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x + width - side, y + height - side, side,
			 side, XiQUAD_4);
	}
        break;
    case XmTAB_EDGE_BOTTOM_RIGHT:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    rect[0].x = x;
	    rect[0].y = y + side;
	    rect[0].width = width;
	    rect[0].height = height - side;
	    rect[1].x = x + side;
	    rect[1].y = y;
	    rect[1].width = width - (2 * side);
	    rect[1].height = side;

	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x + width - side, y, side,
			 side, XiQUAD_1);
	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc, x, y, side,
			 side, XiQUAD_2);
	}
	else
	{
	    rect[0].x = x + side;
	    rect[0].y = y;
	    rect[0].width = width - side;
	    rect[0].height = height;
	    rect[1].x = x;
	    rect[1].y = y + side;
	    rect[1].width = side;
	    rect[1].height = height - (2 * side);

	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc, x, y, side,
			 side, XiQUAD_2);
	    XiFillCorner(XtDisplay(tab), XiCanvas(tab), gc,
			 x, y + height - side, side, side, XiQUAD_3);
	}
	break;
    }

    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, 2);
}

static void
FillBeveledTab(XmTabBoxWidget tab, GC gc, XiTabRect *geometry, XmTabEdge edge)
{
    XPoint pt[6];
    int    x = geometry->x, y = geometry->y,
           width = geometry->width, height = geometry->height, side;

    side = XmTabBox__corner_size(tab);
    if( width < height )
    {
	AssignMin(side, (width/2));
    }
    else
    {
	AssignMin(side, (height/2));
    }

    switch( edge )
    {
    case XmTAB_EDGE_TOP_LEFT:
    default:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    pt[0].x = x;
	    pt[0].y = y;
	    pt[1].x = x;
	    pt[1].y = y + height - side;
	    pt[2].x = x + side;
	    pt[2].y = y + height;
	    pt[3].x = x + width - side;
	    pt[3].y = y + height;
	    pt[4].x = x + width;
	    pt[4].y = y + height - side;
	    pt[5].x = x + width;
	    pt[5].y = y;
	}
	else
	{
	    pt[0].x = x;
	    pt[0].y = y;
	    pt[1].x = x + width - side;
	    pt[1].y = y;
	    pt[2].x = x + width;
	    pt[2].y = y + side;
	    pt[3].x = x + width;
	    pt[3].y = y + height - side;
	    pt[4].x = x + width - side;
	    pt[4].y = y + height;
	    pt[5].x = x;
	    pt[5].y = y + height;
	}
	break;
    case XmTAB_EDGE_BOTTOM_RIGHT:
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    pt[0].x = x;
	    pt[0].y = y + height;
	    pt[1].x = x;
	    pt[1].y = y + side;
	    pt[2].x = x + side;
	    pt[2].y = y;
	    pt[3].x = x + width - side;
	    pt[3].y = y;
	    pt[4].x = x + width;
	    pt[4].y = y + side;
	    pt[5].x = x + width;
	    pt[5].y = y + height;
	}
	else
	{
	    pt[0].x = x + width;
	    pt[0].y = y + height;
	    pt[1].x = x + side;
	    pt[1].y = y + height;
	    pt[2].x = x;
	    pt[2].y = y + height - side;
	    pt[3].x = x;
	    pt[3].y = y + side;
	    pt[4].x = x + side;
	    pt[4].y = y;
	    pt[5].x = x + width;
	    pt[5].y = y;
	}
	break;
    }
    XFillPolygon(XtDisplay(tab), XiCanvas(tab), gc, pt, 6, Nonconvex,
		 CoordModeOrigin);
}

int
XmTabBoxGetIndex(Widget widget, int x, int y)
{
    return( XiXYtoTab((XmTabBoxWidget)widget, x, y) );
}

static void
CalcStackedGeometry(XmTabBoxWidget tab, XRectangle *rect)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    XmTabAttributes info;
    int             i, count = _XmTabbedStackListCount(list),
                    max_width, max_height,
                    tab_width, tab_height,
                    num_stacks = XmTabBox_num_stacks(tab);
    Dimension       d_width, d_height;
    /*
     * Calculating the geometry we want for the stack layout turns
     * out to be straight forward.  What we do is find calculate the
     * size of the largest tab and then use that to calculate the
     * geometry we want.
     */
    max_width = max_height = 0;
    for( i = 0; i < count; ++i )
    {
	info = _XmTabbedStackListGet(list, i);
	XiCalcTabSize(tab, info, &d_width, &d_height);
	AssignMax(max_width, (int)d_width);
	AssignMax(max_height, (int)d_height);

	XmTabBox__wanted(tab)[i].width = d_width;
	XmTabBox__wanted(tab)[i].height = d_height;
    }

    /*
     * Since we are in stacked modes, all tabs must be the same size.
     */
    for( i = 0; i < count; ++i )
    {
	XmTabBox__wanted(tab)[i].width = max_width;
	XmTabBox__wanted(tab)[i].height = max_height;
    }

    /*
     * Now that we know what the max tab's size is so lets use that and
     * the XmNnumStacks resource to figure out the size that we want to
     * be.
     */
    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	tab_width = num_stacks * max_width;
	tab_height = ((int)(count/num_stacks) + (count%num_stacks ? 1 : 0)) *
	    max_height;
    }
    else
    {
	tab_height = num_stacks * max_height;
	tab_width =  ((int)(count/num_stacks) + (count%num_stacks ? 1 : 0)) *
	    max_width;
    }

    rect->width = tab_width;
    rect->height = tab_height;
}

static void
HorizontalStackedLayout(XmTabBoxWidget tab, Boolean is_static)
{
    int        i, x, y, width, per_line, num_rows, tab_width, tab_height,
               tmp, on_line, start_x, offset, row, idx,
               cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    XiTabRect  *actual = XmTabBox__actual(tab);
    XRectangle *wanted = XmTabBox__wanted(tab);

    if( cnt == 0 ) return;

    offset = XmTabBox_tab_offset(tab);

    width = XtWidth(tab);
    tab_width = 0;
    for( i = 0; i < cnt; ++i )
    {
	AssignMax(tab_width, (int)wanted[i].width);
    }

    /*
     * Since we are doing a stacked layout we know that uniform tab
     * size are a must, so lets find out how many tabs we can fit on
     * a line.
     */
    per_line = cnt;
    num_rows = (int)(cnt/per_line) + (cnt % per_line > 0 ? 1 : 0);
    tmp = (per_line * tab_width) + (num_rows * offset);
    while( per_line > 1 && tmp > width )
    {
	per_line--;
	num_rows = (int)(cnt/per_line) + (cnt % per_line > 0 ? 1 : 0);
	tmp = (per_line * tab_width) + (num_rows * offset);
    }
    AssignMax(num_rows, 1);
    AssignMax(per_line, 1);

    tab_height = ((int)XtHeight(tab))/ num_rows;
    if( num_rows > 1 )
    {
	tab_width = (((int)XtWidth(tab)) - ((num_rows-1) * offset))/per_line;
    }
    AssignMax(tab_height, 1);
    AssignMax(tab_width, 1);

    /*
     * Now that we know what size each tab is going to be, we need to
     * the location for each tab. This is done differently depending
     * if we are doing a static or dynamic stacked layout. For the
     * static layout the tabs are always in the same location so we
     * can choose a row/column as we place them. For the dynamic we
     * will first choose a row column and then a pixel location.
     */

    if( is_static )
    {
	start_x = x = (LayoutIsRtoLP(tab)
		? (per_line-1) * tab_width + offset * (num_rows-1)
		: 0);

	on_line = 0;
	row = 0;
	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    y = (int) XtHeight(tab) - tab_height;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[i].x = x;
		actual[i].y = y;
		actual[i].width = tab_width;
		actual[i].height = tab_height;
		actual[i].row = row;
		actual[i].column = on_line;
		if( ++on_line >= per_line )
		{
		    on_line = 0;
		    start_x += (LayoutIsRtoLP(tab) ? -offset : offset);
		    x = start_x;
		    y -= tab_height;
		    row++;
		}
		else
		{
		    x += (LayoutIsRtoLP(tab) ? -tab_width : tab_width);
		}
	    }
	}
	else
	{
	    y = 0;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[i].x = x;
		actual[i].y = y;
		actual[i].width = tab_width;
		actual[i].height = tab_height;
		actual[i].row = row;
		actual[i].column = on_line;
		if( ++on_line >= per_line )
		{
		    on_line = 0;
		    start_x += (LayoutIsRtoLP(tab) ? -offset : offset);
		    x = start_x;
		    y += tab_height;
		    row++;
		}
		else
		{
		    x += (LayoutIsRtoLP(tab) ? -tab_width : tab_width);
		}
	    }
	}
	XmTabBox__num_rows(tab) = num_rows;
	XmTabBox__num_columns(tab) = per_line;
    }
    else
    {
	/*
	 * The challenge here is to make sure that the selected tab is
	 * always on row 0. So the first thing we need to do is break up
	 * the tabs into rows.
	 */
	start_x = x = (LayoutIsRtoLP(tab)
		? (per_line-1)*tab_width + offset*(num_rows-1)
		: 0);
	on_line = 0;
	row = 0;

	if( XmTabBox__selected(tab) < 0 )
	{
	    idx = 0;
	}
	else
	{
	    idx = (int)(XmTabBox__selected(tab)/ per_line) * per_line;
	}

	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    y = (int) XtHeight(tab) - tab_height;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[idx].x = x;
		actual[idx].y = y;
		actual[idx].width = tab_width;
		actual[idx].height = tab_height;
		actual[idx].row = row;
		actual[idx++].column = on_line;
		if( ++on_line >= per_line || idx >= cnt )
		{
		    on_line = 0;
		    start_x += LayoutIsRtoLP(tab) ? -offset : offset;
		    x = start_x;
		    y -= tab_height;
		    row++;
		}
		else
		{
		    x += LayoutIsRtoLP(tab) ? -tab_width : tab_width;
		}
		if( idx >= cnt ) idx = 0;
	    }
	}
	else
	{
	    y = 0;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[idx].x = x;
		actual[idx].y = y;
		actual[idx].width = tab_width;
		actual[idx].height = tab_height;
		actual[idx].row = row;
		actual[idx++].column = on_line;
		if( ++on_line >= per_line || idx >= cnt )
		{
		    on_line = 0;
		    start_x += LayoutIsRtoLP(tab) ? -offset : offset;
		    x = start_x;
		    y += tab_height;
		    row++;
		}
		else
		{
		    x += LayoutIsRtoLP(tab) ? -tab_width : tab_width;
		}
		if( idx >= cnt ) idx = 0;
	    }
	}
	XmTabBox__num_rows(tab) = num_rows;
	XmTabBox__num_columns(tab) = per_line;
    }
}

static void
VerticalStackedLayout(XmTabBoxWidget tab, Boolean is_static)
{
    int        i, x, y, height, per_line, num_rows, tab_width, tab_height,
               tmp, on_line, start_y, offset, row, idx,
               cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    XiTabRect  *actual = XmTabBox__actual(tab);
    XRectangle *wanted = XmTabBox__wanted(tab);

    if( cnt == 0 ) return;

    offset = XmTabBox_tab_offset(tab);

    height = XtHeight(tab);
    tab_height = 0;
    for( i = 0; i < cnt; ++i )
    {
	AssignMax(tab_height, (int)wanted[i].height);
    }

    per_line = cnt;
    num_rows = (int)(cnt/per_line) + (cnt % per_line > 0 ? 1 : 0);
    tmp = (per_line * tab_height) + (num_rows * offset);
    while( per_line > 1 && tmp > height )
    {
	per_line--;
	num_rows = (int)(cnt/per_line) + (cnt % per_line > 0 ? 1 : 0);
	tmp = (per_line * tab_height) + (num_rows * offset);
    }
    AssignMax(num_rows, 1);
    AssignMax(per_line, 1);

    if( num_rows > 1 )
    {
	tab_height = (((int)XtHeight(tab)) - ((num_rows-1) * offset))/per_line;
    }
    tab_width = ((int)XtWidth(tab))/num_rows;

    AssignMax(tab_height, 1);
    AssignMax(tab_width, 1);

    /*
     * Now that we know what size each tab is going to be, we need to
     * the location for each tab. This is done differently depending
     * if we are doing a static or dynamic stacked layout. For the
     * static layout the tabs are always in the same location so we
     * can choose a row/column as we place them. For the dynamic we
     * will first choose a row column and then a pixel location.
     */
    if( is_static )
    {
	start_y = y = 0;
	on_line = 0;
	row = 0;
	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    x = (int)XtWidth(tab) - tab_width;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[i].x = x;
		actual[i].y = y;
		actual[i].width = tab_width;
		actual[i].height = tab_height;
		actual[i].row = row;
		actual[i].column = on_line;
		if( ++on_line >= per_line )
		{
		    on_line = 0;
		    start_y += offset;
		    y = start_y;
		    x -= tab_width;
		    row++;
		}
		else
		{
		    y += tab_height;
		}
	    }
	}
	else
	{
	    x = 0;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[i].x = x;
		actual[i].y = y;
		actual[i].width = tab_width;
		actual[i].height = tab_height;
		actual[i].row = row;
		actual[i].column = on_line;
		if( ++on_line >= per_line )
		{
		    on_line = 0;
		    start_y += offset;
		    y = start_y;
		    x += tab_width;
		    row++;
		}
		else
		{
		    y += tab_height;
		}
	    }
	}
	XmTabBox__num_rows(tab) = num_rows;
	XmTabBox__num_columns(tab) = per_line;
    }
    else
    {
	/*
	 * The challenge here is to make sure that the selected tab is
	 * always on row 0. So the first thing we need to do is break up
	 * the tabs into rows.
	 */
	start_y = y = 0;
	on_line = 0;
	row = 0;

	if( XmTabBox__selected(tab) < 0 )
	{
	    idx = 0;
	}
	else
	{
	    idx = (int)(XmTabBox__selected(tab)/ per_line) * per_line;
	}

	if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
	{
	    x = (int)XtWidth(tab) - tab_width;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[idx].x = x;
		actual[idx].y = y;
		actual[idx].width = tab_width;
		actual[idx].height = tab_height;
		actual[idx].row = row;
		actual[idx++].column = on_line;
		if( ++on_line >= per_line || idx >= cnt )
		{
		    on_line = 0;
		    start_y += offset;
		    y = start_y;
		    x -= tab_width;
		    row++;
		}
		else
		{
		    y += tab_height;
		}
		if( idx >= cnt ) idx = 0;
	    }
	}
	else
	{
	    x = 0;
	    for( i = 0; i < cnt; ++i )
	    {
		actual[idx].x = x;
		actual[idx].y = y;
		actual[idx].width = tab_width;
		actual[idx].height = tab_height;
		actual[idx].row = row;
		actual[idx++].column = on_line;
		if( ++on_line >= per_line || idx >= cnt )
		{
		    on_line = 0;
		    start_y += offset;
		    y = start_y;
		    x += tab_width;
		    row++;
		}
		else
		{
		    y += tab_height;
		}
		if( idx >= cnt ) idx = 0;
	    }
	}
	XmTabBox__num_rows(tab) = num_rows;
	XmTabBox__num_columns(tab) = per_line;
    }
}

/*
 * Function:
 *	RedisplayTabs(tab, region)
 * Description:
 *	Walks the list of tabs and redisplays any tab that intersets with
 *	the given region. If the region specified is "False" then all tabs
 *	are exposed (see macro XiRectInRegion).
 * Input:
 *	tab    : XmTabBoxWidget - tab box, whose tabs need redisplayed
 *	region : Region         - the region to expose or False for all
 * Output:
 *	None.
 */
static void
RedisplayTabs(XmTabBoxWidget tab, Region region)
{
    XiTabRect       *geom;
    int             i, count = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    XmTabAttributes info;

    /*
     * Now lets walk through our list of tabs and redisplay any tab that
     * lies in the region.
     */
    geom = XmTabBox__actual(tab);
    for( i = 0; i < count; ++i )
    {
	if( XiRectInRegion(region, geom[i].x, geom[i].y, geom[i].width,
			   geom[i].height) )
	{
	    info = _XmTabbedStackListGet(XmTabBox_tab_list(tab), i);

	    DrawTab(tab, info, &(geom[i]),
		    (Boolean)(XmTabBox__selected(tab) == i),
		    (Boolean)(XmTabBox__keyboard(tab) == i));
	}
    }
}

static void
HorizontalStackedBottomEdgeRedisplay(XmTabBoxWidget tab)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    int             i, count = _XmTabbedStackListCount(list), row = 0, col,
                    shadow = tab->manager.shadow_thickness, x = 0,
                    height, offset, idx, below, corner, cnt, first, last,
                    x1, x2;
    XmTabAttributes info;
    XiTabRect       *geom;
    GC		    gc = XmTabBox__tab_GC(tab);
    Boolean	    do_top, do_bottom;
#define _NUM_RECTS 10
    XRectangle      rect[_NUM_RECTS], top = {0, 0, 0, 0}, bottom = {0, 0, 0, 0};

    /*
     * What we need to do in this mode is to extend the right most tab
     * on each row to the bottom of the widget(minus the shadow). Besides
     * the bcakground of the widget we also need to extend the shadow.
     */
    offset = XmTabBox_tab_offset(tab);
    geom = XmTabBox__actual(tab);

    /*
     * First the quick check. If we only have one row then all we need to do
     * is draw the base line from the end of the last tab to the right edge.
     */
    if( XmTabBox__num_rows(tab) == 1 &&
        (idx = GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1)) >= 0 )
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}

	x = geom[idx].x + geom[idx].width;
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.top_shadow_GC,
		       x, (int)XtHeight(tab) - shadow,
		       (int)XtWidth(tab) - x, shadow);
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       gc, x, 0, (int)XtWidth(tab) - x,
		       (int)XtHeight(tab) - shadow);
	XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    (int)XtWidth(tab) - shadow,
		    (int)XtHeight(tab) - shadow,
		    shadow, XmBEVEL_BOTTOM);
	return;
    }

    for( i = 0; i < count; ++i )
    {
	corner = GetTabIndex(tab, geom[i].row-1, geom[i].column+1);
	below = GetTabIndex(tab, geom[i].row-1, geom[i].column);
	if( geom[i].row > 0 && (corner < 0 || below < 0) )
	{
	    cnt = 0;
	    do_top = do_bottom = False;
	    info = _XmTabbedStackListGet(list, i);

	    if( XiBackgroundSpecified(info) )
	    {
		SetBackgroundGC(tab, info, gc);
	    }
	    else if( ValidPixmap(tab->core.background_pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, tab->core.background_pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
	    }

	    for( height = geom[i].height, row = geom[i].row - 2;
  		 row >= 0 && GetTabIndex(tab, row, geom[i].column+1) < 0;
		 row--, height += geom[i].height );

	    if( corner < 0 )
	    {
		if (LayoutIsRtoLP(tab))
		{
		    rect[cnt].x = geom[i].x;
		    rect[cnt].y = geom[i].y + geom[i].height;
		    rect[cnt].width = offset;
		    rect[cnt++].height = height;

		    bottom.x = geom[i].x;
		    bottom.y = geom[i].y + geom[i].height;
		}
		else
		{
		    rect[cnt].x = geom[i].x + geom[i].width - offset;
		    rect[cnt].y = geom[i].y + geom[i].height;
		    rect[cnt].width = offset;
		    rect[cnt++].height = height;

		    bottom.x = geom[i].x + geom[i].width - shadow;
		    bottom.y = geom[i].y + geom[i].height;
		}
		bottom.width = shadow;
		bottom.height = height;
		do_bottom = True;
	    }

	    if( below < 0 )
	    {
	        if( LayoutIsRtoLP(tab) )
		{
		rect[cnt].x = geom[i].x + offset;
		}
		else
		{
		rect[cnt].x = geom[i].x;
		}
		rect[cnt].y = geom[i].y + geom[i].height;
		rect[cnt].width = geom[i].width - offset;
		rect[cnt].height = geom[i].height;
		cnt++;

		if( LayoutIsRtoLP(tab) )
		{
		    top.x = geom[i].x + geom[i].width - shadow;
		}
		else
		{
		    top.x = geom[i].x;
		}
		top.y = geom[i].y + geom[i].height;
		top.width = shadow;
		top.height = geom[i].height;
		do_top = True;


		if( geom[i].row > 1 &&
		    geom[i].column == XmTabBox__num_columns(tab) - 1 )
		{
		    if( LayoutIsRtoLP(tab) )
		    {
		    rect[cnt].x = offset;
		    }
		    else
		    {
		    rect[cnt].x = geom[i].x + geom[i].width - (2*offset);
		    }
		    rect[cnt].y = geom[i].y + geom[i].height;
		    rect[cnt].width = offset;
		    rect[cnt].height = (int)XtHeight(tab) - rect[cnt].y;
		    cnt++;
		}
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    if( do_bottom )
	    {
		if( LayoutIsRtoLP(tab) )
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.top_shadow_GC, bottom.x,
			       bottom.y, bottom.width, bottom.height);
		else
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.bottom_shadow_GC, bottom.x,
			       bottom.y, bottom.width, bottom.height);
	    }
	    if( do_top )
	    {
	    	if( LayoutIsRtoLP(tab) )
		{
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.bottom_shadow_GC, top.x,
			       top.y, top.width, top.height);
		}
		else
		{
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.top_shadow_GC, top.x,
			       top.y, top.width, top.height);
		}
	    }
	}
    }

    /*
     * Now that all the tab stuff is done, lets clear all the background
     * areas to our parents background color.
     */
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}
    }

    cnt = 0;
    if (LayoutIsRtoLP(tab))
    {
	for( i = 1; i < XmTabBox__num_rows(tab); ++i, x -= offset )
	{
	    if( (idx = GetTabIndex(tab, i, 0)) < 0 ) continue;

	    rect[cnt].x = geom[idx].x + geom[idx].width;
	    rect[cnt].y = geom[idx].y;
	    rect[cnt].width = XtWidth(tab) - geom[idx].x;
	    rect[cnt++].height = geom[idx].height;

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    } else {
	x = offset;
	for( i = 1; i < XmTabBox__num_rows(tab); ++i, x += offset )
	{
	    if( (idx = GetTabIndex(tab, i, 0)) < 0 ) continue;

	    rect[cnt].x = 0;
	    rect[cnt].y = geom[idx].y;
	    rect[cnt].width = x;
	    rect[cnt++].height = geom[idx].height;

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    }

    /*
     * We need to clear any area on the right side of the tabs. To do this
     * we find the tab in the upper right corner and clear down to the
     * bottom and then check the top row to see if it is full.
     */
    last = -1;
    col = XmTabBox__num_columns(tab) - 1;
    for( i = 0; i < XmTabBox__num_rows(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, i, col)) >= 0 )
        {
	    last = idx;
	}
    }
    if( last != -1 )
    {
	if( LayoutIsRtoLP(tab) )
	{
	    rect[cnt].x = 0;
	    rect[cnt].y = 0;
	    rect[cnt].width = geom[last].x;
	    rect[cnt++].height = XtHeight(tab);
	    last = GetTabIndex(tab, 0, 0);
	}
	rect[cnt].x = geom[last].x + geom[last].width;
	rect[cnt].y = 0;
	rect[cnt].width = (int)XtWidth(tab) - rect[cnt].x;
	rect[cnt++].height = XtHeight(tab);
	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }

    row = XmTabBox__num_rows(tab) - 1;
    col = XmTabBox__num_columns(tab) - 1;
    last = -1;
    if( GetTabIndex(tab, row, col) < 0 )
    {
	for( i = col; i >= 0 && last < 0; last = GetTabIndex(tab, row, --i) );

	if( last >= 0 )
	{
	    if (LayoutIsRtoLP(tab)) {
		rect[cnt].x = 0;
		rect[cnt].width = geom[last].x;
	    } else {
		rect[cnt].x = geom[last].x + geom[last].width;
		rect[cnt].width = (int)XtWidth(tab) - rect[cnt].x;
	    }
	    rect[cnt].y = 0;
	    rect[cnt++].height = geom[last].height;

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    }

    if( cnt > 0 )
    {
	XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	cnt=0;
    }


    /*
     * If the users wants the special stacked effect then we are done so
     * lets get out of here.
     */
    if( GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1) >= 0 &&
        XmTabBox_stacked_effect(tab) ) return;

    /*
     * Now that we have all the tabs drawn we want to complete the
     * shadow line from the last tab in the first row to the last
     * tab in the last row.  Lets start by finding the last tab in the
     * first row and the last tab in the last row.
     */
    first = GetTabIndex(tab, 0, 0);;
    for( i = 0; i < XmTabBox__num_columns(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, 0, i)) < 0 ) break;
	first = idx;
    }

    if( XmTabBox_stacked_effect(tab) )
    {
	if( first < 0 ) return;

	/*
	 * If we got here that means that we are doing the stacked effect
	 * and that row 0 is not complete. What this means is that we
	 * need to do is draw a line from the last tab in row 0 to the
	 * location where the row should end.
	 */
	if( LayoutIsRtoLP(tab) )
	{
	    x1 = offset * ((XmTabBox__num_rows(tab) - 1));
	    x2 = geom[first].x;
	}
	else
	{
	    x1 = geom[first].x + geom[first].width;
	    x2 = XmTabBox__num_columns(tab) * geom[first].width;
	}
    }
    else
    {
	x1 = geom[first].x + geom[first].width;
	x2 = XtWidth(tab);
    }


    /*
     * Now that we know what we are dealing with all we have to do
     * is draw the line.
     */

    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		   tab->manager.top_shadow_GC,
		   x1, (int)XtHeight(tab) - shadow,
		   x2 - x1, shadow);

    if( !LayoutIsRtoLP(tab) )
    {
	XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		tab->manager.top_shadow_GC,
		tab->manager.bottom_shadow_GC,
		x2 - shadow, (int)XtHeight(tab) - shadow,
		shadow, XmBEVEL_BOTTOM);
    }
}

static void
HorizontalStackedTopEdgeRedisplay(XmTabBoxWidget tab)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    int             i, count = _XmTabbedStackListCount(list), row = 0, col,
                    shadow = tab->manager.shadow_thickness, x,
                    height, offset, idx, below, corner, cnt, first, last,
                    x1, x2;
    XmTabAttributes info;
    XiTabRect       *geom;
    GC		    gc = XmTabBox__tab_GC(tab);
    Boolean	    do_top, do_bottom;
#define _NUM_RECTS 10
    XRectangle      rect[_NUM_RECTS], top = {0, 0, 0, 0}, bottom = {0, 0, 0, 0};

    offset = XmTabBox_tab_offset(tab);
    geom = XmTabBox__actual(tab);

    /*
     * First the quick check. If we only have one row then all we need to do
     * is draw the base line from the end of the last tab to the right edge.
     */
    if( XmTabBox__num_rows(tab) == 1 &&
        (idx = GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1)) >= 0 )
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}

	x = geom[idx].x + geom[idx].width;
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.bottom_shadow_GC,
		       x, 0, (int)XtWidth(tab) - x, shadow);
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       gc, x, shadow, (int)XtWidth(tab) - x,
		       (int)XtHeight(tab) - shadow);
	return;
    }

    for( i = 0; i < count; ++i )
    {
	corner = GetTabIndex(tab, geom[i].row-1, geom[i].column+1);
	below = GetTabIndex(tab, geom[i].row-1, geom[i].column);
	if( geom[i].row > 0 && (corner < 0 || below < 0) )
	{
	    cnt = 0;
	    do_top = do_bottom = False;
	    info = _XmTabbedStackListGet(list, i);

	    if( XiBackgroundSpecified(info) )
	    {
		SetBackgroundGC(tab, info, gc);
	    }
	    else if( ValidPixmap(tab->core.background_pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, tab->core.background_pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
	    }

	    for( height = geom[i].height, row = geom[i].row - 2;
  		 row >= 0 && GetTabIndex(tab, row, geom[i].column+1) < 0;
		 row--, height += geom[i].height );

	    if( corner < 0 )
	    {
		if( LayoutIsRtoLP(tab) )
		{
		    rect[cnt].x = geom[i].x;
		    bottom.x = geom[i].x;
		}
		else
		{
		    rect[cnt].x = geom[i].x + geom[i].width - offset;
		    bottom.x = geom[i].x + geom[i].width - shadow;
		}
		rect[cnt].y = geom[i].y - height;
		rect[cnt].width = offset;
		rect[cnt++].height = height;

		bottom.y = geom[i].y - height;
		bottom.width = shadow;
		bottom.height = height;
		do_bottom = True;
	    }

	    if( below < 0 )
	    {
	    	if( LayoutIsRtoLP(tab) )
		{
		    rect[cnt].x = geom[i].x + offset;
		}
		else
		{
		    rect[cnt].x = geom[i].x;
		}
		rect[cnt].y = geom[i].y - geom[i].height;
		rect[cnt].width = geom[i].width - offset;
		rect[cnt++].height = geom[i].height;

	    	if( LayoutIsRtoLP(tab) )
		{
		    top.x = geom[i].x + geom[i].width - shadow;
		}
		else
		{
		    top.x = geom[i].x;
		}
		top.y = geom[i].y - geom[i].height;
		top.width = shadow;
		top.height = geom[i].height;
		do_top = True;

		if( geom[i].row > 1 &&
		    geom[i].column == XmTabBox__num_columns(tab) - 1 )
		{
		    if( LayoutIsRtoLP(tab) )
		    {
			rect[cnt].x = offset;
		    }
		    else
		    {
			rect[cnt].x = geom[i].x + geom[i].width - (2*offset);
		    }
		    rect[cnt].y = 0;
		    rect[cnt].width = offset;
		    rect[cnt].height = geom[i].y;
		    cnt++;
		}
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);

	    if( do_bottom )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       LayoutIsRtoLP(tab)
			           ? tab->manager.top_shadow_GC
				   : tab->manager.bottom_shadow_GC,
			       bottom.x, bottom.y, bottom.width, bottom.height);
	    }
	    if( do_top )
	    {
		if( LayoutIsRtoLP(tab) )
		{
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.bottom_shadow_GC, top.x,
			       top.y, top.width, top.height);
		}
		else
		{
		    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.top_shadow_GC, top.x,
			       top.y, top.width, top.height);
		}
	    }
	}
    }

    /*
     * Now that all the tab stuff is done, lets clear all the background
     * areas to our parents background color.
     */
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}
    }

    x = offset;
    cnt = 0;
    for( i = 1; i < XmTabBox__num_rows(tab); ++i, x += offset )
    {
	if( (idx = GetTabIndex(tab, i, 0)) < 0 ) continue;

	rect[cnt].x = LayoutIsRtoLP(tab) ? geom[idx].x + geom[idx].width : 0;
	rect[cnt].y = geom[idx].y;
	rect[cnt].width = x;
	rect[cnt++].height = geom[idx].height;

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }
    /*
     * We need to clear any area on the right side of the tabs. To do this
     * we find the tab in the upper right corner and clear down to the
     * bottom and then check the top row to see if it is full.
     */
    last = -1;
    col = XmTabBox__num_columns(tab) - 1;
    for( i = 0; i < XmTabBox__num_rows(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, i, col)) >= 0 )
	{
	    last = idx;
	}
    }

    if( last != -1 )
    {
	if( LayoutIsRtoLP(tab) )
	{
	    rect[cnt].x = 0;
	    rect[cnt].y = 0;
	    rect[cnt].width = geom[last].x;
	    rect[cnt++].height = XtHeight(tab);
	    last = GetTabIndex(tab, 0, 0);
	}
	rect[cnt].x = geom[last].x + geom[last].width;
	rect[cnt].y = 0;
	rect[cnt].width = (int)XtWidth(tab) - rect[cnt].x;
	rect[cnt++].height = XtHeight(tab);

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }

    row = XmTabBox__num_rows(tab) - 1;
    col = XmTabBox__num_columns(tab) - 1;
    last = -1;
    if( GetTabIndex(tab, row, col) < 0 )
    {
	for( i = col; i >= 0 && last < 0; last = GetTabIndex(tab, row, --i) );

	if( last >= 0 )
	{
	    if( LayoutIsRtoLP(tab) )
	    {
		rect[cnt].x = 0;
		rect[cnt].y = (int)XtHeight(tab) - geom[last].height;
		rect[cnt].width = geom[last].x;
		rect[cnt++].height = geom[last].height;
	    }
	    else
	    {
		rect[cnt].x = geom[last].x + geom[last].width;
		rect[cnt].y = (int)XtHeight(tab) - geom[last].height;
		rect[cnt].width = (int)XtWidth(tab) - rect[cnt].x;
		rect[cnt++].height = geom[last].height;
	    }

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    }

    if( cnt > 0 )
    {
	XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	cnt=0;
    }

    /*
     * If the users wants the special stacked effect then we are done so
     * lets get out of here.
     */
    if( GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1) >= 0 &&
        XmTabBox_stacked_effect(tab) ) return;

    /*
     * Now that we have all the tabs drawn we want to complete the
     * shadow line from the last tab in the first row to the last
     * tab in the last row.  Lets start by finding the last tab in the
     * first row and the last tab in the last row.
     */
    first = GetTabIndex(tab, 0, 0);;
    for( i = 0; i < XmTabBox__num_columns(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, 0, i)) < 0 ) break;
	first = idx;
    }

    if( XmTabBox_stacked_effect(tab) )
    {
	if( first < 0 ) return;

	/*
	 * If we got here that means that we are doing the stacked effect
	 * and that row 0 is not complete. What this means is that we
	 * need to do is draw a line from the last tab in row 0 to the
	 * location where the row should end.
	 */
	if( LayoutIsRtoLP(tab) )
	{
	    x1 = offset * (XmTabBox__num_rows(tab) - 1) + shadow;
	    x2 = geom[first].x;
	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    x1 - shadow, 0,
		    shadow, XmBEVEL_BOTH);
	    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.bottom_shadow_GC,
		    tab->manager.top_shadow_GC,
		    x2, 0,
		    shadow, XmBEVEL_BOTH);
	}
	else
	{
	    x1 = geom[first].x + geom[first].width;
	    x2 = XmTabBox__num_columns(tab) * geom[first].width;
	}
    }
    else
    {
	x1 = geom[first].x + geom[first].width;
	x2 = XtWidth(tab);
    }

    /*
     * Now that we know what we are dealing with all we have to do
     * is draw the line.
     */

    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		   tab->manager.bottom_shadow_GC,
		   x1, 0, x2 - x1, shadow);
}

static void
HorizontalStackedRedisplay(XmTabBoxWidget tab)
{
    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
    {
	HorizontalStackedBottomEdgeRedisplay(tab);
    }
    else
    {
	HorizontalStackedTopEdgeRedisplay(tab);
    }
}

static void
VerticalStackedRightEdgeRedisplay(XmTabBoxWidget tab)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    int             i, count = _XmTabbedStackListCount(list), row = 0, col,
                    shadow = tab->manager.shadow_thickness, y, width,
                    offset, idx, below, corner, cnt = 0, first, last,
                    y1, y2;
    XmTabAttributes info;
    XiTabRect       *geom;
    GC		    gc = XmTabBox__tab_GC(tab);
    Boolean	    do_top, do_bottom;
#define _NUM_RECTS 10
    XRectangle      rect[_NUM_RECTS], top = {0, 0, 0, 0}, bottom = {0, 0, 0, 0};

    /*
     * What we need to do in this mode is to extend the right most tab
     * on each row to the bottom of the widget(minus the shadow). Besides
     * the bcakground of the widget we also need to extend the shadow.
     */
    offset = XmTabBox_tab_offset(tab);
    geom = XmTabBox__actual(tab);

    /*
     * First the quick check. If we only have one row then all we need to do
     * is draw the base line from the end of the last tab to the right edge.
     */
    if( XmTabBox__num_rows(tab) == 1 &&
        (idx = GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1)) >= 0 )
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}

	y = geom[idx].y + geom[idx].height;
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.top_shadow_GC,
		       (int)XtWidth(tab) - shadow, y,
		       shadow, (int)XtHeight(tab) - y);
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       gc, 0, y, (int)XtWidth(tab) - shadow,
		       (int)XtHeight(tab) - y);
	XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    (int)XtWidth(tab) - shadow,
		    (int)XtHeight(tab) - shadow,
		    shadow, XmBEVEL_BOTTOM);
	return;
    }

    for( i = 0; i < count; ++i )
    {
	corner = GetTabIndex(tab, geom[i].row-1, geom[i].column+1);
	below = GetTabIndex(tab, geom[i].row-1, geom[i].column);
	if( geom[i].row > 0 && (corner < 0 || below < 0) )
	{
	    cnt = 0;
	    do_top = do_bottom = False;
	    info = _XmTabbedStackListGet(list, i);

	    if( XiBackgroundSpecified(info) )
	    {
		SetBackgroundGC(tab, info, gc);
	    }
	    else if( ValidPixmap(tab->core.background_pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, tab->core.background_pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
	    }

	    for( width = geom[i].width, row = geom[i].row - 2;
		 row >= 0 && GetTabIndex(tab, row, geom[i].column+1) < 0;
		 row--, width += geom[i].width );

	    if( corner < 0 )
	    {
		rect[cnt].x = geom[i].x + geom[i].width;
		rect[cnt].y = geom[i].y + geom[i].height- offset;
		rect[cnt].width = width;
		rect[cnt++].height = offset;

		bottom.x = geom[i].x + geom[i].width;
		bottom.y = geom[i].y + geom[i].height - shadow;
		bottom.width = width;
		bottom.height = shadow;
		do_bottom = True;
	    }

	    if( below < 0 )
	    {
		rect[cnt].x = geom[i].x + geom[i].width;
		rect[cnt].y = geom[i].y;
		rect[cnt].width = geom[i].width;
		rect[cnt++].height = geom[i].height - offset;

		top.x = geom[i].x + geom[i].width;
		top.y = geom[i].y;
		top.width = geom[i].width;
		top.height = shadow;
		do_top = True;

		if( geom[i].row > 1 &&
		    geom[i].column == XmTabBox__num_columns(tab) - 1 )
		{
		    rect[cnt].x = geom[i].x + geom[i].width;
		    rect[cnt].y = geom[i].y + geom[i].height - (2*offset);
		    rect[cnt].width = (int)XtWidth(tab) - rect[cnt].x;
		    rect[cnt].height = offset;
		    cnt++;
		}
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    if( do_bottom )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.bottom_shadow_GC, bottom.x,
			       bottom.y, bottom.width, bottom.height);
	    }
	    if( do_top )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.top_shadow_GC, top.x,
			       top.y, top.width, top.height);
	    }
	}
    }

    /*
     * Now that all the tab stuff is done, lets clear all the background
     * areas to our parents background color.
     */
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}
    }

    y = offset;
    cnt = 0;
    for( i = 1; i < XmTabBox__num_rows(tab); ++i, y += offset )
    {
	if( (idx = GetTabIndex(tab, i, 0)) < 0 ) continue;

	rect[cnt].x = geom[idx].x;
	rect[cnt].y = 0;
	rect[cnt].width = geom[idx].width;
	rect[cnt++].height = y;

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }
    /*
     * We need to clear any area on the right side of the tabs. To do this
     * we find the tab in the upper right corner and clear down to the
     * bottom and then check the top row to see if it is full.
     */
    last = -1;
    col = XmTabBox__num_columns(tab) - 1;
    for( i = 0; i < XmTabBox__num_rows(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, i, col)) >= 0 )
	{
	    last = idx;
	}
    }

    if( last != -1 )
    {
	rect[cnt].x = 0;
	rect[cnt].y = geom[last].y + geom[last].height;
	rect[cnt].width = XtWidth(tab);
	rect[cnt].height = (int)XtHeight(tab) - rect[cnt].y;
	cnt++;

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }

    row = XmTabBox__num_rows(tab) - 1;
    col = XmTabBox__num_columns(tab) - 1;
    last = -1;
    if( GetTabIndex(tab, row, col) < 0 )
    {
	for( i = col; i >= 0 && last < 0; last = GetTabIndex(tab, row, --i) );

	if( last >= 0 )
	{
	    rect[cnt].x = 0;
	    rect[cnt].y = geom[last].y + geom[last].height;
	    rect[cnt].width = geom[last].width;
	    rect[cnt].height = (int)XtHeight(tab) - rect[cnt].y;
	    cnt++;

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    }

    if( cnt > 0 )
    {
	XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	cnt=0;
    }


    /*
     * If the users wants the special stacked effect then we are done so
     * lets get out of here.
     */
    if( GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1) >= 0 &&
        XmTabBox_stacked_effect(tab) ) return;

    /*
     * Now that we have all the tabs drawn we want to complete the
     * shadow line from the last tab in the first row to the last
     * tab in the last row.  Lets start by finding the last tab in the
     * first row and the last tab in the last row.
     */
    first = GetTabIndex(tab, 0, 0);;
    for( i = 0; i < XmTabBox__num_columns(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, 0, i)) < 0 ) break;
	first = idx;
    }

    if( XmTabBox_stacked_effect(tab) )
    {
	if( first < 0 ) return;

	/*
	 * If we got here that means that we are doing the stacked effect
	 * and that row 0 is not complete. What this means is that we
	 * need to do is draw a line from the last tab in row 0 to the
	 * location where the row should end.
	 */
	y1 = geom[first].y + geom[first].height;
	y2 = XmTabBox__num_columns(tab) * geom[first].height;
    }
    else
    {
	y1 = geom[first].y + geom[first].height;
	y2 = XtHeight(tab);
    }


    /*
     * Now that we know what we are dealing with all we have to do
     * is draw the line.
     */

    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		   tab->manager.top_shadow_GC,
		   (int)XtWidth(tab) - shadow, y1,
		   shadow, y2 - y1);
    XmDrawBevel(XtDisplay(tab), XiCanvas(tab),
		tab->manager.top_shadow_GC,
		tab->manager.bottom_shadow_GC,
		(int)XtWidth(tab) - shadow, y2 - shadow,
		shadow, XmBEVEL_BOTTOM);
}

static void
VerticalStackedLeftEdgeRedisplay(XmTabBoxWidget tab)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    int             i, count = _XmTabbedStackListCount(list), row = 0, col,
                    shadow = tab->manager.shadow_thickness, y, width,
                    offset, idx, below, corner, cnt = 0, first, last,
                    y1, y2;
    XmTabAttributes info;
    XiTabRect       *geom;
    GC		    gc = XmTabBox__tab_GC(tab);
    Boolean	    do_top, do_bottom;
#define _NUM_RECTS 10
    XRectangle      rect[_NUM_RECTS], top = {0, 0, 0, 0}, bottom = {0, 0, 0, 0};

    offset = XmTabBox_tab_offset(tab);
    geom = XmTabBox__actual(tab);

    if( XmTabBox__num_rows(tab) == 1 &&
        (idx = GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1)) >= 0 )
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}

	y = geom[idx].y + geom[idx].height;
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       tab->manager.bottom_shadow_GC,
		       0, y, shadow, (int)XtHeight(tab) - y);
	XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		       gc, shadow, y, (int)XtWidth(tab) - shadow,
		       (int)XtHeight(tab) - y);
	return;
    }

    for( i = 0; i < count; ++i )
    {
	corner = GetTabIndex(tab, geom[i].row-1, geom[i].column+1);
	below = GetTabIndex(tab, geom[i].row-1, geom[i].column);
	if( geom[i].row > 0 && (corner < 0 || below < 0) )
	{
	    cnt = 0;
	    do_top = do_bottom = False;
	    info = _XmTabbedStackListGet(list, i);

	    if( XiBackgroundSpecified(info) )
	    {
		SetBackgroundGC(tab, info, gc);
	    }
	    else if( ValidPixmap(tab->core.background_pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, tab->core.background_pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, tab->core.background_pixel);
	    }

	    for( width = geom[i].width, row = geom[i].row - 2;
  		 row >= 0 && GetTabIndex(tab, row, geom[i].column+1) < 0;
		 row--, width += geom[i].width );

	    if( corner < 0 )
	    {
		rect[cnt].x = geom[i].x - width;
		rect[cnt].y = geom[i].y + geom[i].height - offset;
		rect[cnt].width = width;
		rect[cnt++].height = offset;

		bottom.x = geom[i].x - width;
		bottom.y = geom[i].y + geom[i].height - shadow;
		bottom.width = width;
		bottom.height = shadow;
		do_bottom = True;
	    }

	    if( below < 0 )
	    {
		rect[cnt].x = geom[i].x - geom[i].width;
		rect[cnt].y = geom[i].y;
		rect[cnt].width = geom[i].width;
		rect[cnt++].height = geom[i].height - offset;

		top.x = geom[i].x - geom[i].width;
		top.y = geom[i].y;
		top.width = geom[i].width;
		top.height = shadow;
		do_top = True;

		if( geom[i].row > 1 &&
		    geom[i].column == XmTabBox__num_columns(tab) - 1 )
		{
		    rect[cnt].x = 0;
		    rect[cnt].y = geom[i].y + geom[i].height - (2*offset);
		    rect[cnt].width = geom[i].x;
		    rect[cnt].height = offset;
		    cnt++;
		}
	    }

	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);

	    if( do_bottom )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.bottom_shadow_GC, bottom.x,
			       bottom.y, bottom.width, bottom.height);
	    }
	    if( do_top )
	    {
		XFillRectangle(XtDisplay(tab), XiCanvas(tab),
			       tab->manager.top_shadow_GC, top.x,
			       top.y, top.width, top.height);
	    }
	}
    }
    /*
     * Now that all the tab stuff is done, lets clear all the background
     * areas to our parents background color.
     */
    {
	Pixel  pixel;
	Pixmap pixmap;

	XtVaGetValues(XiGCParent(tab),
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);

	if( ValidPixmap(pixmap) )
	{
	    SetTiledGC(XtDisplay(tab), gc, pixmap);
	}
	else
	{
	    SetSolidGC(XtDisplay(tab), gc, pixel);
	}
    }

    y = offset;
    cnt = 0;
    for( i = 1; i < XmTabBox__num_rows(tab); ++i, y += offset )
    {
	if( (idx = GetTabIndex(tab, i, 0)) < 0 ) continue;

	rect[cnt].x = geom[idx].x;
	rect[cnt].y = 0;
	rect[cnt].width = geom[idx].width;
	rect[cnt++].height = y;

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }
    /*
     * We need to clear any area on the right side of the tabs. To do this
     * we find the tab in the upper right corner and clear down to the
     * bottom and then check the top row to see if it is full.
     */
    last = -1;
    col = XmTabBox__num_columns(tab) - 1;
    for( i = 0; i < XmTabBox__num_rows(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, i, col)) >= 0 )
	{
	    last = idx;
	}
    }

    if( last != -1 )
    {
	rect[cnt].x = 0;
	rect[cnt].y = geom[last].y + geom[last].height;
	rect[cnt].width = XtWidth(tab);
	rect[cnt].height = (int)XtHeight(tab) - rect[cnt].y;
	cnt++;

	if( cnt >= _NUM_RECTS )
	{
	    XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	    cnt=0;
	}
    }

    row = XmTabBox__num_rows(tab) - 1;
    col = XmTabBox__num_columns(tab) - 1;
    last = -1;
    if( GetTabIndex(tab, row, col) < 0 )
    {
	for( i = col; i >= 0 && last < 0; last = GetTabIndex(tab, row, --i) );

	if( last >= 0 )
	{
	    rect[cnt].x = (int)XtWidth(tab) - geom[last].width;
	    rect[cnt].y = geom[last].y + geom[last].height;
	    rect[cnt].width = geom[last].width;
	    rect[cnt].height = (int)XtHeight(tab) - rect[cnt].y;
	    cnt++;

	    if( cnt >= _NUM_RECTS )
	    {
		XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
		cnt=0;
	    }
	}
    }

    if( cnt > 0 )
    {
	XFillRectangles(XtDisplay(tab), XiCanvas(tab), gc, rect, cnt);
	cnt=0;
    }


    /*
     * If the users wants the special stacked effect then we are done so
     * lets get out of here.
     */
    if( GetTabIndex(tab, 0, XmTabBox__num_columns(tab) - 1) >= 0 &&
        XmTabBox_stacked_effect(tab) ) return;

    /*
     * Now that we have all the tabs drawn we want to complete the
     * shadow line from the last tab in the first row to the last
     * tab in the last row.  Lets start by finding the last tab in the
     * first row and the last tab in the last row.
     */
    first = GetTabIndex(tab, 0, 0);;
    for( i = 0; i < XmTabBox__num_columns(tab); ++i )
    {
	if( (idx = GetTabIndex(tab, 0, i)) < 0 ) break;
	first = idx;
    }

    if( XmTabBox_stacked_effect(tab) )
    {
	if( first < 0 ) return;

	/*
	 * If we got here that means that we are doing the stacked effect
	 * and that row 0 is not complete. What this means is that we
	 * need to do is draw a line from the last tab in row 0 to the
	 * location where the row should end.
	 */
	y1 = geom[first].y + geom[first].height;
	y2 = XmTabBox__num_columns(tab) * geom[first].height;
    }
    else
    {
	y1 = geom[first].y + geom[first].height;
	y2 = XtHeight(tab);
    }


    /*
     * Now that we know what we are dealing with all we have to do
     * is draw the line.
     */

    XFillRectangle(XtDisplay(tab), XiCanvas(tab),
		   tab->manager.bottom_shadow_GC,
		   0, y1, shadow, y2 - y1);
}

static void
VerticalStackedRedisplay(XmTabBoxWidget tab)
{
    if( XmTabBox_tab_edge(tab) == XmTAB_EDGE_BOTTOM_RIGHT )
    {
	VerticalStackedRightEdgeRedisplay(tab);
    }
    else
    {
	VerticalStackedLeftEdgeRedisplay(tab);
    }
}

static XmTabAttributes
GetTabInfo(XmTabBoxWidget tab, int row, int column)
{
    int idx = GetTabIndex(tab, row, column);

    if( idx < 0 ) return( NULL );

    return( _XmTabbedStackListGet(XmTabBox_tab_list(tab), idx) );
}

static int
GetTabIndex(XmTabBoxWidget tab, int row, int column)
{
    XmTabbedStackList list = XmTabBox_tab_list(tab);
    int       i, count = _XmTabbedStackListCount(list);
    XiTabRect *rect = XmTabBox__actual(tab);

    if( row < 0 || column < 0 || count < 1 ) return( -1 );

    for( i = 0; i < count; ++i )
    {
	if( row == rect[i].row && column == rect[i].column )
	{
	    return( i );
	}
    }
    return( -1 );
}

static void
SelectTab(XmTabBoxWidget tab, XEvent *event, int old, int set)
{
    int old_row, set_row;

    if( old == set ) return;

    XmTabBox__selected(tab) = set;
    old_row = set_row = XmTabBox__actual(tab)[set].row;
    if( old >= 0 )
    {
	old_row = XmTabBox__actual(tab)[old].row;
    }
    if( XmTabBox_tab_mode(tab) == XmTABS_STACKED && old_row != set_row )
    {
	Layout(tab);
	if( XtIsRealized((Widget)tab) )
	{
	    Redisplay(XmTabBox__canvas(tab), NULL, False);
	}
    }
    else
    {
	if( old != -1 )
	{
	    DrawTab(tab, _XmTabbedStackListGet(XmTabBox_tab_list(tab), old),
		    &(XmTabBox__actual(tab)[old]), False, False);
	}
	DrawTab(tab, _XmTabbedStackListGet(XmTabBox_tab_list(tab), set),
		&(XmTabBox__actual(tab)[set]), True, True);
    }
    CallCallbacks(tab, event, old, set);
}

void
_XmTabBoxSelectTab(Widget widget, int idx)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) || idx < 0 ||
        idx >=_XmTabbedStackListCount(XmTabBox_tab_list(tab)) ) return;

    SelectTab(tab, NULL, XmTabBox__selected(tab), idx);
}

static Boolean
IsTabSensitive(XmTabBoxWidget tab, int idx)
{
    XmTabAttributes info = _XmTabbedStackListGet(XmTabBox_tab_list(tab), idx);

    if( info != NULL ) return( info->sensitive );

    return( False );
}

void
_XmTabBoxStackedGeometry(XmTabBoxWidget tab, Dimension size,
			 XRectangle *rect)
{
    XmTabbedStackList       list = XmTabBox_tab_list(tab);
    XmTabAttributes info;
    int             i, count = _XmTabbedStackListCount(list),
                    max_width, max_height, tab_width, tab_height,
                    num_cols, num_rows, offset;
    Dimension       d_width, d_height;

    if( count == 0 )
    {
	if( XmTabBox_orientation(tab) == XmHORIZONTAL )
	{
	    rect->width = 2 * tab->manager.shadow_thickness;
	    rect->height = tab->manager.shadow_thickness;
	}
	else
	{
	    rect->width = tab->manager.shadow_thickness;
	    rect->height = 2 * tab->manager.shadow_thickness;
	}

	if( rect->width < 1 ) rect->width = 1;
	if( rect->height < 1 ) rect->height = 1;
	return;
    }

    /*
     * Calculating the geometry we want for the stack layout turns
     * out to be straight forward.  What we do is find calculate the
     * size of the largest tab and then use that to calculate the
     * geometry we want.
     */
    max_width = max_height = 1;
    for( i = 0; i < count; ++i )
    {
	info = _XmTabbedStackListGet(list, i);
	XiCalcTabSize(tab, info, &d_width, &d_height);
	AssignMax(max_width, (int)d_width);
	AssignMax(max_height, (int)d_height);
    }

    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	/*
	 * Now that we know how big the tabs are lets see how many can fit in
	 * the available space.
	 */
	num_cols = (int)size / max_width;
	AssignMax(num_cols, 1);

	/*
	 * Now we know how many fit per row, so what we now need to do is
	 * how many rows we need.
	 */
	num_rows = (count/ num_cols) + ((count % num_cols == 0) ? 0 : 1);

	/*
	 * Now that we know how many rows and columns we need we can calculate
	 * overall size.
	 */

	offset = XmTabBox_tab_offset(tab);
	tab_width = size + ((num_rows - 1) * offset);
	tab_height = num_rows * max_height;
    }
    else
    {
	num_cols = (int)size/ max_height;
	AssignMax(num_cols, 1);
	num_rows = (count/ num_cols) + ((count % num_cols == 0) ? 0 : 1);
	offset = XmTabBox_tab_offset(tab);
	tab_width = num_rows * max_width;
	tab_height = size + ((num_rows - 1) * offset);
    }

    rect->width = tab_width;
    rect->height = tab_height;
}

int
XmTabBoxGetNumRows(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) ) return( 0 );

    return( XmTabBox__num_rows(tab) );
}

int
XmTabBoxGetNumColumns(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) ) return( 0 );

    return( XmTabBox__num_columns(tab) );
}

void
_XmTabBoxGetNumRowsColumns(Widget widget, int size, int *num_rows,
			   int *num_cols)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;
    int            i, max, _num_rows, _num_cols, tmp, offset, cnt;
    XRectangle     *wanted = XmTabBox__wanted(tab);

    *num_rows = 0;
    *num_cols = 0;
    if( !XmIsTabBox(widget) ||
        (cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab))) == 0 ||
        (XmTabBox_tab_mode(tab) != XmTABS_STACKED &&
	 XmTabBox_tab_mode(tab) != XmTABS_STACKED_STATIC) )
    {
	return;
    }

    offset = XmTabBox_tab_offset(tab);
    max = 0;

    if( XmTabBox_orientation(tab) == XmHORIZONTAL )
    {
	for( i = 0; i < cnt; ++i )
	{
	    AssignMax(max, (int)wanted[i].width);
	}
    }
    else
    {
	for( i = 0; i < cnt; ++i )
	{
	    AssignMax(max, (int)wanted[i].height);
	}
    }

    _num_cols = cnt;
    _num_rows = (int)(cnt/ _num_cols) + (cnt % _num_cols > 0 ? 1 : 0);
    tmp = (_num_cols * max) + (_num_rows * offset);
    while( _num_cols > 1 && tmp > size )
    {
	_num_cols--;
	_num_rows = (int)(cnt/ _num_cols) + (cnt % _num_cols > 0 ? 1 : 0);
	tmp = (_num_cols * max) + (_num_rows * offset);
    }
    *num_rows = _num_rows;
    *num_cols = _num_cols;
}

int XmTabBoxGetNumTabs(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) ) return( 0 );

    return( _XmTabbedStackListCount(XmTabBox_tab_list(tab)) );
}

int _XiGetTabIndex(Widget tab, int row, int column)
{
    if( !XmIsTabBox(tab) ) return( -1 );

    return( GetTabIndex((XmTabBoxWidget)tab, row, column) );
}

int _XmTabBoxGetTabWidth(Widget widget, int idx)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) || idx < 0 ||
        idx > _XmTabbedStackListCount(XmTabBox_tab_list(tab)) ) return( -1 );

    return( XmTabBox__actual(tab)[idx].width );
}

int _XmTabBoxGetTabHeight(Widget widget, int idx)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) || idx < 0 ||
        idx > _XmTabbedStackListCount(XmTabBox_tab_list(tab)) ) return( -1 );

    return( XmTabBox__actual(tab)[idx].height );
}

static Widget XiGCParent(XmTabBoxWidget tab)
{
    Widget parent = XtParent(tab);

    if( XmIsTabStack(parent) )
    {
	return( XtParent(parent) );
    }
    return( parent );
}

int XmTabBoxGetTabRow(Widget widget, int idx)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) || idx < 0 ||
        idx > _XmTabbedStackListCount(XmTabBox_tab_list(tab)) ) return( -1 );

    return( XmTabBox__actual(tab)[idx].row );
}

static void ResetImageCache(XmTabBoxWidget tab)
{
    int i, cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    /*
     * First thing we will do is clear any existing cache.
     */
    if( XmTabBox__cache_size(tab) > 0 )
    {
	for( i = 0; i < XmTabBox__cache_size(tab); ++i )
	{
	    if( XmTabBox__cache(tab)[i].pixmap != NULL )
	    {
		XDestroyImage(XmTabBox__cache(tab)[i].pixmap);
	    }
	    if( XmTabBox__cache(tab)[i].label != NULL )
	    {
		XDestroyImage(XmTabBox__cache(tab)[i].label);
	    }
	}
    }

    /*
     * If we have no tabs then we will free the cache.
     */
    if( XmTabBox_tab_list(tab) == NULL || cnt == 0 )
    {
	XtFree((XtPointer) XmTabBox__cache(tab));
	XmTabBox__cache(tab) = NULL;
	XmTabBox__cache_size(tab) = 0;
	return;
    }

    /*
     * If we got here that means that we actually have some items so lets
     * size our cache to the correct size.
     */
    if( cnt != XmTabBox__cache_size(tab) )
    {
	XmTabBox__cache_size(tab) = cnt;
	XmTabBox__cache(tab) = (XiCache*)
	    XtRealloc((XtPointer)XmTabBox__cache(tab),
		      sizeof(XiCache) * cnt);
    }

    /*
     * Then lets zero the cache out.
     */
    for( i = 0; i < cnt; ++i )
    {
	XmTabBox__cache(tab)[i].pixmap = NULL;
	XmTabBox__cache(tab)[i].label = NULL;
	XmTabBox__cache(tab)[i].sensitive = False;
    }
}

static void FreeImageCache(XmTabBoxWidget tab)
{
    int i;

    /*
     * First thing we will do is clear any existing cache.
     */
    if( XmTabBox__cache_size(tab) > 0 )
    {
	for( i = 0; i < XmTabBox__cache_size(tab); ++i )
	{
	    if( XmTabBox__cache(tab)[i].pixmap != NULL )
	    {
		XDestroyImage(XmTabBox__cache(tab)[i].pixmap);
	    }
	    if( XmTabBox__cache(tab)[i].label != NULL )
	    {
		XDestroyImage(XmTabBox__cache(tab)[i].label);
	    }
	}
    }

    XtFree((XtPointer) XmTabBox__cache(tab));
    XmTabBox__cache(tab) = NULL;
    XmTabBox__cache_size(tab) = 0;
}

static int InfoToIndex(XmTabBoxWidget tab, XmTabAttributes info)
{
    return( (int)(info - _XmTabbedStackListArray(XmTabBox_tab_list(tab))) );
}

Widget _XmTabBoxCanvas(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;

    if( !XmIsTabBox(widget) ) return( NULL );

    return( XmTabBox__canvas(tab) );
}

int
_XmTabBoxGetMaxTabWidth(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;
    int            i, max = 0, cnt;
    XRectangle     *wanted;

    if( !XmIsTabBox(widget) ) return( 0 );

    cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    wanted = XmTabBox__wanted(tab);

    for( i = 0; i < cnt; ++i )
    {
	AssignMax(max, (int)wanted[i].width);
    }

    return( max );
}

int
_XmTabBoxGetMaxTabHeight(Widget widget)
{
    XmTabBoxWidget tab = (XmTabBoxWidget) widget;
    int            i, max = 0, cnt;
    XRectangle     *wanted;

    if( !XmIsTabBox(widget) ) return( 0 );

    cnt = _XmTabbedStackListCount(XmTabBox_tab_list(tab));
    wanted = XmTabBox__wanted(tab);

    for( i = 0; i < cnt; ++i )
    {
	AssignMax(max, (int)wanted[i].height);
    }

    return( max );
}

int XmTabBoxXYToIndex(Widget widget, int x, int y)
{
    if( !XmIsTabBox(widget) ) return( -1 );

    return( XiXYtoTab((XmTabBoxWidget)widget, x, y) );
}

static Visual*
GetShellVisual(Widget widget)
{
    Visual *visual;

    if( widget == NULL )
    {
	return( NULL );
    }
    if( XtIsShell(widget) )
    {

	XtVaGetValues(widget,
		      XmNvisual, &visual,
		      NULL);

	return( visual );
    }

    visual = GetShellVisual(XtParent(widget));

    if( visual == NULL )
    {
	return( DefaultVisualOfScreen(XtScreen(widget)) );
    }
    return( visual );
}

/*
 * XmRCallProc routine for checking font_list before setting it to NULL
 * If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */

/*ARGSUSED*/
static void
CheckSetRenderTable(Widget wid, int offs, XrmValue *value)
{
  XmTabBoxWidget tb = (XmTabBoxWidget)wid;

  /* Check if been here before */
  if (tb->tab_box.check_set_render_table)
      value->addr = NULL;
  else {
      tb->tab_box.check_set_render_table = True;
      value->addr = (char*)&(tb->tab_box.font_list);
  }

}

Widget
XmCreateTabBox(Widget parent, String name, ArgList arg_list,
		 Cardinal arg_cnt)
{
    return( XtCreateWidget(name, xmTabBoxWidgetClass, parent, arg_list,
			   arg_cnt) );
}
