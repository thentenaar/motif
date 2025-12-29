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
 *
 */

/**
 * TabStack.c - Source code for the XmTabStack Widget Class.
 *
 * This file contains the source code that implements the XmTabStack
 * WidgetClass. The XmTabStack is a manager widget where each child of
 * the TabStack is associated with a label and/or pixmap that is
 * positioned on the side to simulate a folder tab.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "XmI.h"
#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/BulletinBP.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/TabStackP.h>
#include <Xm/TabBoxP.h>
#include <Xm/VaSimpleP.h>

#undef TEAR_OFF_TABS

#ifdef TEAR_OFF_TABS
#include <Xm/DragCP.h>
#include <Xm/DragOverSP.h>
#include <Xm/DragDrop.h>
#include <Xm/DragIcon.h>
#endif

#ifdef _ARGS
#undef _ARGS
#endif
#define _ARGS(a) a

extern int _XiGetTabIndex _ARGS((Widget, int, int));
extern int _GetTabWidth _ARGS((Widget, int));
extern int _XmTabBoxGetTabHeight _ARGS((Widget, int));
extern Widget _XmTabBoxCanvas _ARGS((Widget));
extern void _XmTabBoxGetNumRowColumns _ARGS((Widget, int, int*, int*));
extern int _XmTabBoxGetMaxTabWidth _ARGS((Widget));
extern int _XmTabBoxGetMaxTabHeight _ARGS((Widget));
extern void _XmTabBoxSelectTab _ARGS((Widget, int));
extern void _XmTabBoxGetNumRowsColumns _ARGS((Widget, int, int *, int *));
extern void _XmTabBoxStackedGeometry _ARGS((XmTabBoxWidget, int, XRectangle *));
extern int _XmTabbedStackListCount _ARGS((XmTabbedStackList));

static void ClassInitialize(void);

#ifdef TEAR_OFF_TABS
static void ClassPartInitialize _ARGS((WidgetClass));
#endif
static void Initialize _ARGS((Widget, Widget, ArgList, Cardinal*));
static void Destroy _ARGS((Widget));
static void Resize _ARGS((Widget));
static void Realize _ARGS((Widget, XtValueMask *, XSetWindowAttributes *));
static void Redisplay _ARGS((Widget, XEvent*, Region));
static Boolean SetValues _ARGS((Widget, Widget, Widget, ArgList, Cardinal*));
static XtGeometryResult QueryGeometry _ARGS((Widget, XtWidgetGeometry*,
					    XtWidgetGeometry*));

static XtGeometryResult GeometryManager _ARGS((Widget, XtWidgetGeometry*,
					       XtWidgetGeometry*));
static void ChangeManaged _ARGS((Widget));

static void ConstraintInitialize _ARGS((Widget, Widget, ArgList, Cardinal*));
static void ConstraintDestroy _ARGS((Widget));
static Boolean ConstraintSetValues _ARGS((Widget, Widget, Widget, ArgList,
					  Cardinal*));

static Boolean CvtStringToXmTabSide _ARGS((Display*, XrmValue*, Cardinal*,
					   XrmValue*, XrmValue*, XtPointer*));
static Boolean CvtStringToXmPixmapPlacement _ARGS((Display*, XrmValue*,
						   Cardinal*, XrmValue*,
						   XrmValue*, XtPointer*));
static Boolean CvtStringToXiPixel _ARGS((Display*, XrmValue*, Cardinal*,
					 XrmValue*, XrmValue*, XtPointer*));
static Boolean CvtStringToXiPixmap _ARGS((Display*, XrmValue*, Cardinal*,
					  XrmValue*, XrmValue*, XtPointer*));

static void PickSizes _ARGS((XmTabStackWidget, int, int, XRectangle*,
			     XRectangle*));
static void Layout _ARGS((XmTabStackWidget));
static void DrawShadows _ARGS((XmTabStackWidget, GC, GC, int, int, int, int));
static void DrawStackedShadows _ARGS((XmTabStackWidget, GC, GC, int, int,
				      int, int));

static void TabSelectedCallback _ARGS((Widget, XtPointer, XtPointer));
static void CheckSetRenderTable(Widget wid, int offs, XrmValue *value);
#ifdef TEAR_OFF_TABS
static void TearOffCallback _ARGS((Widget, XtPointer, XtPointer));
static void XmTabStackMenu _ARGS((Widget, XEvent*, String*, Cardinal*));
static void BuildMenu _ARGS((XmTabStackWidget));
static void XmTabBoxDragTab _ARGS((Widget, XEvent*, String*, Cardinal*));
static Atom TabAtom _ARGS((Widget));
static void HandleTabDrop _ARGS((Widget, XtPointer, XtPointer));
static void TabTransferProc _ARGS((Widget, XtPointer, Atom*, Atom*,
				   XtPointer, unsigned long*, int));
static Boolean TabConvertProc _ARGS((Widget, Atom*, Atom*, Atom*,
				     XtPointer*, unsigned long*, int*,
				     unsigned long*, XtPointer, XtRequestId));
static void XiMoveTabPanel _ARGS((Widget, Widget));
#define WidgetAtom(w) XInternAtom(XtDisplay(w), "WIDGET", False)
#endif


#define ValidPixmap(p) ((p) != (Pixmap)NULL && \
			(p) != (Pixmap)XmUNSPECIFIED_PIXMAP && \
			(p) != (Pixmap)XmPIXMAP_DYNAMIC)

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

#define XiBackgroundSpecified(c) \
	(XmTabStackC_tab_background(c) != XmCOLOR_DYNAMIC || \
	 ValidPixmap(XmTabStackC_tab_background_pixmap(c)))

#define SetChildGC(c,g) \
{ \
      if( ValidPixmap(XmTabStackC_tab_background_pixmap(c)) ) \
      { \
	    SetTiledGC(XtDisplay(c), (g), XmTabStackC_tab_background_pixmap(c)); \
      } \
      else \
      { \
	    SetSolidGC(XtDisplay(c), (g), XmTabStackC_tab_background(c)); \
      }\
}

#define XiSelectSpecified(t) \
	(XmTabStack_select_color((t)) != XmCOLOR_DYNAMIC || \
	 ValidPixmap(XmTabStack_select_pixmap((t))))

#define SetSelectGC(t,g) \
{ \
      if( ValidPixmap(XmTabStack_select_pixmap((t))) ) \
      { \
	    SetTiledGC(XtDisplay(t), (g), XmTabStack_select_pixmap((t))); \
      } \
      else \
      { \
	    SetSolidGC(XtDisplay(t), (g), XmTabStack_select_color((t))); \
      }\
}

#ifndef AssignMax
#define AssignMax(x,y) if((y)>(x)) (x)=(y); else
#endif
#ifndef AssignMin
#define AssignMin(x,y) if((y)<(x)) (x)=(y); else
#endif
#ifndef Max
#define Max(x,y) (((x)>(y))?(x):(y))
#endif

#define LocalTabBox(t) (XtParent(XmTabStack_tab_box((t))) == (Widget)(t))
#define IsTabBox(t,k) (XmTabStack_tab_box((t)) == (k))
#define IsValidChild(t, k) (XtIsManaged(k) && !(k)->core.being_destroyed && \
			    !IsTabBox(t,k))

//#define BBPart(w) (*(XmBulletinBoardPart*)((char*)(w) + XmTabStack_offsets[XmBulletinBoardIndex]))
#define BBPart(w) (((XmBulletinBoardWidget)(w))->bulletin_board)



#ifdef TEAR_OFF_TABS
static XtActionsRec actions[] = {
    { "XmTabStackMenu",		  (XtActionProc) XmTabStackMenu           },
    { "XmTabBoxDragTab",	  (XtActionProc) XmTabBoxDragTab          },
};
static char drag_translations[] =
    "<Btn2Down>:   XmTabBoxArmTab() XmTabBoxDragTab()\n\
     <Btn3Down>:   XmTabStackMenu()\n\
     <Key>osfMenu: XmTabStackMenu()";
#endif

static XtConvertArgRec XmColorConvertArgs[] = {
    {XtWidgetBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.self),
	 sizeof(Widget)},
};

static String tab_stack_filter[] =
    { XmNx, XmNy, XmNwidth, XmNheight, XmNdestroyCallback, XmNsensitive,
      XmNuserData, XmNnavigationType, XmNmarginWidth, XmNmarginHeight,
      XmNtabList, XmNtabOrientation, XmNorientation, XmNtabEdge,
      XmNselectCallback, XmNunselectCallback, NULL };

#ifdef TEAR_OFF_TABS
/*
 * Bitmap used for Drag Icon
 */
#define tab_pix_width 32
#define tab_pix_height 32
#define tab_pix_x_hot 0
#define tab_pix_y_hot 0
static unsigned char tab_pix_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0xa9, 0xea, 0x03, 0x00, 0x54, 0xd5,
   0xfb, 0xff, 0xff, 0xbf, 0x00, 0x00, 0x00, 0xe0, 0xaa, 0xaa, 0xaa, 0xba,
   0x54, 0x55, 0x55, 0xf5, 0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5,
   0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5, 0xaa, 0xaa, 0xaa, 0xba,
   0x54, 0x55, 0x55, 0xf5, 0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5,
   0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5, 0xaa, 0xaa, 0xaa, 0xba,
   0x54, 0x55, 0x55, 0xf5, 0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5,
   0xaa, 0xaa, 0xaa, 0xba, 0x54, 0x55, 0x55, 0xf5, 0xfe, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static unsigned char tab_mask_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfe, 0x1f, 0xec, 0xff, 0xff, 0x3f,
  0xfc, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

/*
static unsigned char invalid_bits[] = {
  0x00, 0xf0, 0x0f, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x01, 0x80, 0x00,
  0xff, 0x00, 0x00, 0xe3, 0x3f, 0xe0, 0x07, 0xec, 0x13, 0x1c, 0x3c, 0xd8,
  0x0b, 0xfe, 0x7f, 0xb0, 0x08, 0x01, 0x20, 0xf0, 0x86, 0xaa, 0x1a, 0xa0,
  0x42, 0x55, 0x0d, 0xc2, 0xa2, 0xaa, 0x06, 0xc7, 0x62, 0x55, 0x83, 0xc5,
  0xa1, 0xaa, 0xc1, 0x86, 0x51, 0xd5, 0x60, 0x8d, 0xb1, 0x6a, 0xb0, 0x8a,
  0x51, 0x35, 0x58, 0x8d, 0xb1, 0x1a, 0xac, 0x8a, 0x51, 0x0d, 0x56, 0x8d,
  0xb1, 0x06, 0xab, 0x8a, 0x61, 0x83, 0x55, 0x85, 0xa2, 0xc1, 0xaa, 0xc6,
  0xe2, 0x60, 0x55, 0xc5, 0x42, 0xb0, 0xaa, 0xc2, 0x04, 0x58, 0x55, 0xe1,
  0x0a, 0xac, 0xaa, 0xb0, 0x0c, 0x56, 0x55, 0xf0, 0x1e, 0xfc, 0x3f, 0xf8,
  0x20, 0xe0, 0x07, 0x04, 0xc0, 0x00, 0x00, 0x03, 0x00, 0x01, 0x80, 0x00,
  0x00, 0x0e, 0x70, 0x00, 0x00, 0xf0, 0x0f, 0x00, };

static unsigned char invalid_mask_bits[] = {
  0x00, 0xf0, 0x0f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00,
  0xc0, 0xff, 0xff, 0x1f, 0xe0, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x3f,
  0xfc, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x3f,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x3f,
  0xe0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x00,
  0x00, 0xfe, 0x7f, 0x00, 0x00, 0xf0, 0x0f, 0x00, };
*/
static unsigned char invalid_bits[] = {
  0x00, 0xf0, 0x0f, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x01, 0x80, 0x00,
  0xc0, 0x00, 0x00, 0x03, 0x20, 0xe0, 0x07, 0x04, 0x10, 0x1c, 0x38, 0x08,
  0x08, 0x02, 0x40, 0x10, 0x08, 0x01, 0x20, 0x10, 0x86, 0x00, 0x10, 0x20,
  0x42, 0x00, 0x08, 0x42, 0x22, 0x00, 0x04, 0x45, 0x22, 0x00, 0x82, 0x44,
  0x21, 0x00, 0x41, 0x84, 0x11, 0x80, 0x20, 0x88, 0x11, 0x40, 0x10, 0x88,
  0x11, 0x20, 0x08, 0x88, 0x11, 0x10, 0x04, 0x88, 0x11, 0x08, 0x02, 0x88,
  0x11, 0x04, 0x01, 0x88, 0x21, 0x82, 0x00, 0x84, 0x22, 0x41, 0x00, 0x44,
  0xa2, 0x20, 0x00, 0x44, 0x42, 0x10, 0x00, 0x42, 0x04, 0x08, 0x00, 0x21,
  0x08, 0x04, 0x80, 0x10, 0x08, 0x02, 0x40, 0x10, 0x10, 0x1c, 0x38, 0x08,
  0x20, 0xe0, 0x07, 0x04, 0xc0, 0x00, 0x00, 0x03, 0x00, 0x01, 0x80, 0x00,
  0x00, 0x0e, 0x70, 0x00, 0x00, 0xf0, 0x0f, 0x00, };

static unsigned char invalid_mask_bits[] = {
  0x00, 0xf0, 0x0f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00,
  0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07, 0xf0, 0x1f, 0xf8, 0x0f,
  0xf8, 0x03, 0xc0, 0x1f, 0xf8, 0x01, 0xe0, 0x1f, 0xfe, 0x00, 0xf0, 0x3f,
  0x7e, 0x00, 0xf8, 0x7f, 0x3e, 0x00, 0xfc, 0x7d, 0x3e, 0x00, 0xfe, 0x7c,
  0x3f, 0x00, 0x7f, 0xfc, 0x1f, 0x80, 0x3f, 0xf8, 0x1f, 0xc0, 0x1f, 0xf8,
  0x1f, 0xe0, 0x0f, 0xf8, 0x1f, 0xf0, 0x07, 0xf8, 0x1f, 0xf8, 0x03, 0xf8,
  0x1f, 0xfc, 0x01, 0xf8, 0x3f, 0xfe, 0x00, 0xfc, 0x3e, 0x7f, 0x00, 0x7c,
  0xbe, 0x3f, 0x00, 0x7c, 0xfe, 0x1f, 0x00, 0x7e, 0xfc, 0x0f, 0x00, 0x3f,
  0xf8, 0x07, 0x80, 0x1f, 0xf8, 0x03, 0xc0, 0x1f, 0xf0, 0x1f, 0xf8, 0x0f,
  0xe0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x00,
  0x00, 0xfe, 0x7f, 0x00, 0x00, 0xf0, 0x0f, 0x00, };
#endif

static XtResource resources[] =
{
  /* Inherit (but changed default) resources */
  {
    XmNshadowThickness, XmCShadowThickness, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNautoUnmanage, XmCAutoUnmanage, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmBulletinBoardRec, bulletin_board.auto_unmanage),
    XmRImmediate, (XtPointer) False
  },

  /* TabBox Resources */
#ifdef TEAR_OFF_TABS
  {
    XmNallowTearOffTabs, XmCAllowTearOffTabs, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.allow_tear_offs),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtearOffLabelString, XmCTearOffLabelString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmTabStackRec, tab_stack.tear_off_label),
    XmRString, (XtPointer) "Tear Off Tab"
  },
#endif
  {
    XmNtabAutoSelect, XmCTabAutoSelect, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.tab_auto_select),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtabStyle, XmCTabStyle, XmRTabStyle,
    sizeof(XmTabStyle), XtOffsetOf(XmTabStackRec, tab_stack.tab_style),
    XmRImmediate, (XtPointer) XmTABS_BEVELED
  },

  {
    XmNtabMode, XmCTabMode, XmRTabMode,
    sizeof(XmTabMode), XtOffsetOf(XmTabStackRec, tab_stack.tab_mode),
    XmRImmediate, (XtPointer) XmTABS_BASIC
  },

  {
    XmNtabMarginWidth, XmCMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmTabStackRec, tab_stack.tab_margin_width),
    XmRImmediate, (XtPointer) 3
  },

  {
    XmNtabMarginHeight, XmCMarginHeight, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmTabStackRec, tab_stack.tab_margin_height),
    XmRImmediate, (XtPointer) 3
  },

  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.check_set_render_table),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof (XmFontList), XtOffsetOf(XmTabStackRec, tab_stack.font_list),
    XmRCallProc, (XtPointer) CheckSetRenderTable
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmTabStackRec, tab_stack.font_list),
    XmRCallProc, (XtPointer) CheckSetRenderTable
  },

  {
    XmNhighlightThickness, XmCHighlightThickness, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmTabStackRec, tab_stack.highlight_thickness),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNtabSide, XmCTabSide, XmRXmTabSide,
    sizeof(XmTabSide), XtOffsetOf(XmTabStackRec, tab_stack.tab_side),
    XmRImmediate, (XtPointer) XmTABS_ON_TOP
  },

  {
    XmNtabOrientation, XmCTabOrientation, XmRTabOrientation,
    sizeof(XmTabOrientation), XtOffsetOf(XmTabStackRec, tab_stack.tab_orientation),
    XmRImmediate, (XtPointer) XmTAB_ORIENTATION_DYNAMIC
  },

  {
    XmNuniformTabSize, XmCUniformTabSize, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.uniform_tab_size),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtabSelectColor, XmCTabSelectColor, XmRXmPixel,
    sizeof(Pixel), XtOffsetOf(XmTabStackRec, tab_stack.select_color),
    XmRImmediate, (XtPointer) XmCOLOR_DYNAMIC
  },

  {
    XmNtabSelectPixmap, XmCTabSelectPixmap, XmRXmPixmap,
    sizeof(Pixmap), XtOffsetOf(XmTabStackRec, tab_stack.select_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNstackedEffect, XmCStackedEffect, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.stacked_effect),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtabCornerPercent, XmCTabCornerPercent, XmRInt,
    sizeof(int), XtOffsetOf(XmTabStackRec, tab_stack.tab_corner_percent),
    XmRImmediate, (XtPointer) 40
  },

  {
    XmNtabLabelSpacing, XmCTabLabelSpacing, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmTabStackRec, tab_stack.tab_label_spacing),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNtabOffset, XmCTabOffset, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmTabStackRec, tab_stack.tab_offset),
    XmRImmediate, (XtPointer) 10
  },

  {
    XmNuseImageCache, XmCUseImageCache, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackRec, tab_stack.use_image_cache),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtabSelectedCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmTabStackRec, tab_stack.tab_select_callback),
    XmRImmediate, (XtPointer) NULL
  },

  /* appears to be intentionally undocumented; interface not yet public */
  {
    XmNtabBoxWidget, XmCWidget, XmRWidget,
    sizeof(Widget), XtOffsetOf(XmTabStackRec, tab_stack.tab_box),
    XmRImmediate, (XtPointer) NULL
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNtabMarginWidth, sizeof(Dimension),
    XtOffsetOf(XmTabStackRec, tab_stack.tab_margin_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNtabMarginHeight, sizeof(Dimension),
    XtOffsetOf(XmTabStackRec, tab_stack.tab_margin_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNtabLabelSpacing, sizeof(Dimension),
    XtOffsetOf(XmTabStackRec, tab_stack.tab_label_spacing),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNtabOffset, sizeof(Dimension),
    XtOffsetOf(XmTabStackRec, tab_stack.tab_offset),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

static XtResource constraint_resources[] =
{
  {
    XmNtabLabelString, XmCTabLabelString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_label_string),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNtabAlignment, XmCAlignment, XmRAlignment,
    sizeof(unsigned char), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_alignment),
    XmRImmediate, (XtPointer) XmALIGNMENT_CENTER
  },

  {
    XmNtabStringDirection, XmCStringDirection, XmRStringDirection,
    sizeof(unsigned char), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_string_direction),
    XmRImmediate, (XtPointer) XmSTRING_DIRECTION_DEFAULT
  },

  {
    XmNtabLabelPixmap, XmCTabLabelPixmap, XmRManForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_label_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNtabPixmapPlacement, XmCTabPixmapPlacement, XmRXmPixmapPlacement,
    sizeof(XmPixmapPlacement), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_pixmap_placement),
    XmRImmediate, (XtPointer) XmPIXMAP_RIGHT
  },

  {
    XmNfreeTabPixmap, XmCFreeTabPixmap, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackConstraintRec, tab_stack.free_tab_pixmap),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNtabBackground, XmCBackground, XmRXmPixel,
    sizeof(Pixel), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_background),
    XmRImmediate, (XtPointer) XmCOLOR_DYNAMIC
  },

  {
    XmNtabBackgroundPixmap, XmCBackgroundPixmap, XmRXmPixmap,
    sizeof(Pixmap), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_background_pixmap),
    XmRImmediate, (XtPointer) XmPIXMAP_DYNAMIC
  },

  {
    XmNtabForeground, XmCForeground, XmRXmPixel,
    sizeof(Pixel), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_foreground),
    XmRImmediate, (XtPointer) XmCOLOR_DYNAMIC
  },
#ifdef TEAR_OFF_TABS
  {
    XmNtabTearOffEnabled, XmCTabTearOffEnabled, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmTabStackConstraintRec, tab_stack.tear_off_enabled),
    XmRImmediate, (XtPointer) True
  }
#endif
};

static void Get_tabLabelString(Widget, int, XtArgVal *);
static XmSyntheticResource cont_get_resources[] =
{
  {
    XmNtabLabelString, sizeof(XmString),
    XtOffsetOf(XmTabStackConstraintRec, tab_stack.tab_label_string),
    Get_tabLabelString, (XmImportProc) NULL
  }
};

static void Get_tabLabelString (Widget widget, int offset, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmTabStackC_tab_label_string(widget));
}

XmTabStackClassRec xmTabStackClassRec = {
  { /* Core Fields */
    /* superclass	  */	(WidgetClass) &xmBulletinBoardClassRec,
    /* class_name	  */	"XmTabStack",
    /* widget_size	  */	sizeof(XmTabStackRec),
    /* class_initialize   */    ClassInitialize,
#ifdef TEAR_OFF_TABS
    /* class_part_initial */	ClassPartInitialize,
#else
    /* class_part_initial */	NULL,
#endif
    /* class_inited       */	False,
    /* initialize	  */	Initialize,
    /* initialize hook    */    NULL,
    /* realize		  */	Realize,
#ifdef TEAR_OFF_TABS
    /* actions		  */	actions,
    /* num_actions	  */	XtNumber(actions),
#else
    /* actions		  */	NULL,
    /* num_actions	  */	0,
#endif
    /* resources	  */	(XtResource*)resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	True,
    /* compress_exposure  */	XtExposeCompressMultiple,
    /* compress enter/exit*/    True,
    /* visible_interest	  */	False,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay,
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
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    (XtPointer) NULL },
  { /* Constraint Fields */
    /* resources          */    constraint_resources,
    /* num_resources      */    XtNumber(constraint_resources),
    /* constraint_size    */    sizeof (XmTabStackConstraintRec),
    /* initialize         */    ConstraintInitialize,
    /* destroy            */    ConstraintDestroy,
    /* set_values         */    ConstraintSetValues,
    /* extension          */    (XtPointer) NULL },
  { /* OSF/Motif Manager Fields */
    /* translations       */    XtInheritTranslations,
    /* syn_resources      */    get_resources,
    /* num_syn_resources  */    XtNumber(get_resources),
    /* syn_resources      */    cont_get_resources,
    /* num_syn_resources  */    XtNumber(cont_get_resources),
    /* parent_process     */    XmInheritParentProcess,
    /* extension          */    (XtPointer) NULL },
  { /* OSF/Motif BulletinBoard */
    /* always_install_acc */    False,
    /* geo_matrix_create  */    XmInheritGeoMatrixCreate,
    /* focus_moved_proc   */    XmInheritFocusMovedProc,
    /* extension          */    (XtPointer) NULL },
  { /* EPak Stack Box Fields */
#ifdef TEAR_OFF_TABS
    /* drag_translations  */    drag_translations,
#else
    /* drag_translations  */    "",
#endif
    /* extension          */    (XtPointer) NULL }
};

WidgetClass xmTabStackWidgetClass = (WidgetClass) &xmTabStackClassRec;

/*
 * Note these aren't static, even though they should be.  TabBox.c
 * uses the instance data too.
 */

/*
 * Function:
 *	ClassInitialize(void)
 * Description:
 *	This function sets all the type converters needed for the
 *	XmTabStack widget class.
 * Input:
 *	None.
 * Output:
 *	None.
 */
static void ClassInitialize(void)
{
    XmTabStackClassRec* wc = &xmTabStackClassRec;

    /*
     * Initialize the XmTabBox class to add its type converters which we
     * also use.
     */
    XtInitializeWidgetClass(xmTabBoxWidgetClass);
    XtSetTypeConverter(XmRString, XmRXmTabSide,
		       CvtStringToXmTabSide, NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRXmPixmapPlacement,
		       CvtStringToXmPixmapPlacement, NULL, 0, XtCacheNone,
		       NULL);
    XtSetTypeConverter(XmRString, XmRXmPixel,
		       CvtStringToXiPixel,
		       XmColorConvertArgs, XtNumber(XmColorConvertArgs),
		       XtCacheNone, NULL);
    XtSetTypeConverter(XmRString, XmRXmPixmap,
		       CvtStringToXiPixmap,
		       XmColorConvertArgs, XtNumber(XmColorConvertArgs),
		       XtCacheNone, NULL);
}

#ifdef TEAR_OFF_TABS
static void ClassPartInitialize(WidgetClass widget_class)
{
    XmTabStackWidgetClass tab = (XmTabStackWidgetClass) widget_class;

    _XmFastSubclassInit (w_class, XmTABSTACK_BIT);

    if( XmTabStack_class(tab).drag_translations != NULL )
    {
	if( (String)XmTabStack_class(tab).drag_translations ==
	    XtInheritTranslations )
	{
	    XmTabStack_class(tab).drag_translations = (String)
		((XmTabStackWidgetClass) tab->core_class.superclass)->
		    tab_stack_class.drag_translations;
	}
	else
	{
	    XmTabStack_class(tab).drag_translations = (String)
		XtParseTranslationTable((String) XmTabStack_class(tab).
					drag_translations);
	}
    }
}
#endif

/*
 * Function:
 *	Initialize(request, set, arg_list, arg_cnt)
 * Description:
 *	This function initializes the widget data structure for an
 *	instance of the XmTabStack.
 * Input:
 *	request  : Widget    - the user's requested resources
 *	set      : Widget    - the initial resource values
 *	arg_list : ArgList   - the argument list passed in with creation
 *	arg_cnt  : Cardinal* - the number of arguments
 * Output:
 *	None.
 */
static void Initialize(Widget request, Widget set, ArgList arg_list, Cardinal *arg_cnt)
{
    XmTabStackWidget  ts = (XmTabStackWidget) set;
    Arg               args[50];
    ArgList           filtered_args, merged_args;
    Cardinal          n = 0, num_filtered_args;
    XmTabOrientation  orientation = XmTabStack_tab_orientation(ts);

    XmTabStack__inited(ts) = False;

    /* By default call XmNtabSelectedCallback callbacks when tab is selected */
    XmTabStack_do_notify(ts) = True;

    /*
     * Lets make sure that our font list is not NULL.  If it is we will
     * set it to the appropriate default.
     */
    if( XmTabStack_font_list(ts) == NULL )
    {
	XmTabStack_font_list(ts) = XmeGetDefaultRenderTable(set, XmLABEL_FONTLIST);
    }
    XmTabStack_font_list(ts) = XmFontListCopy(XmTabStack_font_list(ts));
    XmTabStack__gc(ts) = NULL;

    /*
     * For our stack we need to create a TabBox, to do this we need to set
     * up some arguments depending on which side the caller wants the tabs
     * on.
     */
    switch( XmTabStack_tab_side(ts) )
    {
    case XmTABS_ON_TOP:
    default:
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); ++n;
	if( orientation == XmTAB_ORIENTATION_DYNAMIC )
	{
	    XtSetArg(args[n], XmNtabOrientation, XmTABS_LEFT_TO_RIGHT); ++n;
	}
	else
	{
	    XtSetArg(args[n], XmNtabOrientation, orientation); ++n;
	}
	XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_BOTTOM_RIGHT); ++n;
	break;
    case XmTABS_ON_BOTTOM:
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); ++n;
	if( orientation == XmTAB_ORIENTATION_DYNAMIC )
	{
	    XtSetArg(args[n], XmNtabOrientation, XmTABS_LEFT_TO_RIGHT); ++n;
	}
	else
	{
	    XtSetArg(args[n], XmNtabOrientation, orientation); ++n;
	}
	XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_TOP_LEFT); ++n;
	break;
    case XmTABS_ON_RIGHT:
	XtSetArg(args[n], XmNorientation, XmVERTICAL); ++n;
	if( orientation == XmTAB_ORIENTATION_DYNAMIC )
	{
	    XtSetArg(args[n], XmNtabOrientation, XmTABS_TOP_TO_BOTTOM); ++n;
	}
	else
	{
	    XtSetArg(args[n], XmNtabOrientation, orientation); ++n;
	}
	XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_TOP_LEFT); ++n;
	break;
    case XmTABS_ON_LEFT:
	XtSetArg(args[n], XmNorientation, XmVERTICAL); ++n;
	if( orientation == XmTAB_ORIENTATION_DYNAMIC )
	{
	    XtSetArg(args[n], XmNtabOrientation, XmTABS_BOTTOM_TO_TOP); ++n;
	}
	else
	{
	    XtSetArg(args[n], XmNtabOrientation, orientation); ++n;
	}
	XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_BOTTOM_RIGHT); ++n;
	break;
    }

    /*
     * Lets do a real quick check here.  If we are in a stacked mode
     * and XmNuniformTabSize is False, then lets set it to true.
     *
     * We have to add the XtSetArg for the default value of XmNuniformTabSize
     * here because we may override it later.
     */
    XtSetArg(args[n], XmNuniformTabSize, XmTabStack_uniform_tab_size(ts)); ++n;
    if( !XmTabStack_uniform_tab_size(ts) &&
        (XmTabStack_tab_mode(ts) == XmTABS_STACKED ||
	 XmTabStack_tab_mode(ts) == XmTABS_STACKED_STATIC) )
    {
	/*
	 * Well it appears that we need to issue a warning here, because
	 * the user tried to set XmNuniformTabSize to False, while in a
	 * stacked mode.
	 */
	XmTabStack_uniform_tab_size(ts) = True;
	XmeWarning(set, XmNillegalUniformTabSizeMsg);
	XtSetArg(args[n], XmNuniformTabSize, True); ++n;
    }

    /*
     * Lets make sure we pass certain resource down to our children, like
     * color and font specification. This way if they were set via app
     * defaults the correct thing will happen.
     */
    XtSetArg(args[n], XmNrenderTable, XmTabStack_font_list(ts)); ++n;
    XtSetArg(args[n], XmNbackground, ts->core.background_pixel); ++n;
    XtSetArg(args[n], XmNshadowThickness, ts->manager.shadow_thickness); ++n;
    XtSetArg(args[n], XmNtabMode, XmTabStack_tab_mode(ts)); ++n;
    XtSetArg(args[n], XmNtabStyle, XmTabStack_tab_style(ts)); ++n;
    XtSetArg(args[n], XmNtabAutoSelect, XmTabStack_tab_auto_select(ts)); ++n;
    XtSetArg(args[n], XmNtabSelectColor, XmTabStack_select_color(ts)); ++n;
    XtSetArg(args[n], XmNtabSelectPixmap, XmTabStack_select_pixmap(ts));++n;
    XtSetArg(args[n], XmNtabMarginWidth, XmTabStack_tab_margin_width(ts)); ++n;
    XtSetArg(args[n], XmNtabMarginHeight, XmTabStack_tab_margin_height(ts));++n;
    XtSetArg(args[n], XmNtabLabelSpacing, XmTabStack_tab_label_spacing(ts));++n;
    XtSetArg(args[n], XmNtabCornerPercent,
	     XmTabStack_tab_corner_percent(ts)); ++n;
    XtSetArg(args[n], XmNtabOffset, XmTabStack_tab_offset(ts)); ++n;
    XtSetArg(args[n], XmNhighlightThickness,
	     XmTabStack_highlight_thickness(ts)); ++n;
    XtSetArg(args[n], XmNuseImageCache, XmTabStack_use_image_cache(ts)); ++n;
    XtSetArg(args[n], XmNunitType, XmPIXELS); ++n;

    /*
     * We also want to filter the other arguments that the user has
     * assigned to us.  We will then merge these with the argument
     * list we created and pass this to the children we create.
     */
    _XmFilterArgs(arg_list, *arg_cnt, tab_stack_filter,
		  &filtered_args, &num_filtered_args);
    merged_args = XtMergeArgLists(filtered_args, num_filtered_args,
				  args, n);


    if( XmTabStack_tab_box(ts) == NULL )
    {
	XmTabStack_tab_box(ts) =
	    XtCreateManagedWidget("tabBox", xmTabBoxWidgetClass, set,
				  merged_args, n + num_filtered_args);
    }
    else
    {
	XtSetValues(XmTabStack_tab_box(ts),
		    merged_args, n + num_filtered_args);
    }

#ifdef TEAR_OFF_TABS
    if( XmTabStack_tear_off_label(ts) != NULL )
    {
	XmTabStack_tear_off_label(ts)
	  = XmStringCopy(XmTabStack_tear_off_label(ts));
    }

    canvas = XmTabBox__canvas(XmTabStack_tab_box((XmTabBoxWidget)ts));

    n = 0;
    XmTabStack__menu(ts) = XmCreatePopupMenu(canvas, "tabMenu", args, n);

    XtOverrideTranslations(canvas,
	 (XtTranslations)((XmTabStackWidgetClass)(ts->core.widget_class))
	 ->tab_stack_class.drag_translations);

    target = TabAtom(set);
    n = 0;
    XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_ACTIVE); ++n;
    XtSetArg(args[n], XmNdropSiteOperations, XmDROP_MOVE); ++n;
    XtSetArg(args[n], XmNdropSiteType, XmDROP_SITE_COMPOSITE); ++n;
    XtSetArg(args[n], XmNimportTargets, &target); ++n;
    XtSetArg(args[n], XmNnumImportTargets, 1); ++n;
    XtSetArg(args[n], XmNdropProc, HandleTabDrop); ++n;
    XmDropSiteRegister(set, args, n); n = 0;

    XtSetArg(args[n], XmNlabelString, XmTabStack_tear_off_label(ts)); ++n;
    XmTabStack__tear_off_button(ts) =
	XtCreateManagedWidget("tearOffTab", xmPushButtonWidgetClass,
			      XmTabStack__menu(ts), args, n);
    XtAddCallback(XmTabStack__tear_off_button(ts), XmNactivateCallback,
		  TearOffCallback, NULL);
    n = 0;
    (void) XtCreateManagedWidget("separator", xmSeparatorWidgetClass,
				 XmTabStack__menu(ts), NULL, 0);
#endif

    /*
     * Add a callback to the XmTabBox so that we know when to display
     * the correct child.
     */
    XtAddCallback(XmTabStack_tab_box(ts), XmNselectCallback,
		  TabSelectedCallback, NULL);

    /*
     * Now that we are done with these argument list lets deallocate
     * them.
     */
    XtFree((XtPointer)filtered_args);
    XtFree((XtPointer)merged_args);

    /*
     * Lets initialize some of the instances values.
     */
    XmTabStack__size(ts).x = -1;
    XmTabStack__active_child(ts) = (Widget) NULL;
    XmTabStack__tab_list(ts) = (XmTabbedStackList) NULL;
    XmTabStack__source_icon(ts) = NULL;
    XmTabStack__invalid_icon(ts) = NULL;
    XmTabStack__source_pixmap(ts) = XmUNSPECIFIED_PIXMAP;
    XmTabStack__source_mask(ts) = XmUNSPECIFIED_PIXMAP;
    XmTabStack__invalid_pixmap(ts) = XmUNSPECIFIED_PIXMAP;
    XmTabStack__invalid_mask(ts) = XmUNSPECIFIED_PIXMAP;
    XmTabStack__set_tab_list(ts) = False;

    /*
     * Finally if the caller did not assign us a geometry lets pick
     * an appropriate default.
     */
    if( request->core.width == 0 )
    {
	ts->core.width = 2 * (ts->manager.shadow_thickness +
			      BBPart(ts).margin_width) + 50;
    }
    if( request->core.height == 0 )
    {
	ts->core.height = 2 * (ts->manager.shadow_thickness +
			       BBPart(ts).margin_height) + 50;
    }

    /* Used to select tabs before they are realized */
    XmTabStack__selected_tab(ts) = NULL;
    XmTabStack__selected_notify(ts) = False;

    /*
     * Now that the widget is initialized lets flag that fact.
     */
    XmTabStack__inited(ts) = True;
}


/*
 * Function:
 *	Destroy(widget)
 * Description:
 *	Deallocates all the memory allocated during the lifetime of the
 * 	given widget instance.
 * Input:
 *	widget : Widget - the widget being destroyed.
 * Output:
 *	None.
 */
static void Destroy(Widget widget)
{
    XmTabStackWidget tab = (XmTabStackWidget) widget;

    /*
     * If we have a tab list lets deallocate it.
     */
    if( XmTabStack__tab_list(tab) != NULL )
    {
	XmTabbedStackListFree(XmTabStack__tab_list(tab));
    }

    XmFontListFree(XmTabStack_font_list(tab));

    if( XmTabStack__gc(tab) != NULL )
    {
	XFreeGC(XtDisplay(tab), XmTabStack__gc(tab));
    }

#ifdef TEAR_OFF_TABS
    if( XmTabStack_tear_off_label(tab) != NULL )
    {
	XmStringFree(XmTabStack_tear_off_label(tab));
    }

    if( ValidPixmap(XmTabStack__source_pixmap(tab)) )
    {
	XFreePixmap(XtDisplay(widget), XmTabStack__source_pixmap(tab));
    }

    if( ValidPixmap(XmTabStack__source_mask(tab)) )
    {
	XFreePixmap(XtDisplay(widget), XmTabStack__source_mask(tab));
    }

    if( ValidPixmap(XmTabStack__invalid_pixmap(tab)) )
    {
	XFreePixmap(XtDisplay(widget), XmTabStack__invalid_pixmap(tab));
    }

    if( ValidPixmap(XmTabStack__invalid_mask(tab)) )
    {
	XFreePixmap(XtDisplay(widget), XmTabStack__invalid_mask(tab));
    }
#endif
}

/*
 * Function:
 *	Realize(widget, XtValueMask *, XSetWindowAttributes *)
 * Description:
 *	A realize method is used to address a timing issue that blocks
 *	XmTabStackSelectTab() from working before a selected tab widget
 *	has been realized...
 * Input:
 *	widget : Widget - the widget being resized.
 *
 *
 * Output:
 *	None.
 */
static void Realize(Widget w, XtValueMask *mask, XSetWindowAttributes *attr)
{
    XmTabStackWidget tab = (XmTabStackWidget) w;

    /* Call superclass realize method... */
    XtRealizeProc realize;

    _XmProcessLock();
    realize = xmTabStackWidgetClass->core_class.superclass->core_class.realize;
    _XmProcessUnlock();

    (*realize) (w, mask, attr);

    /* Now that we are realized, let's select that tab, finally... */
    if ((XmTabStack__selected_tab(tab) != NULL) &&
	!XmTabStack__selected_tab(tab)->core.being_destroyed)
    {
	XmTabStackSelectTab(XmTabStack__selected_tab(tab),
			    XmTabStack__selected_notify(tab));
    }

    /*
     * Lets create a GC that we will use for drawing.
     */
    XmTabStack__gc(tab) = XCreateGC(XtDisplay(tab), XtWindow(tab),
				   0, NULL);
}

/*
 * Function:
 *	Resize(widget)
 * Description:
 *	Handles everything when the XmTabStack is resized.  This means
 *	that is shifts the children around to their new locations and
 *	sizes.
 * Input:
 *	widget : Widget - the widget being resized.
 * Output:
 *	None.
 */
static void Resize(Widget widget)
{
    XmTabStackWidget tab = (XmTabStackWidget) widget;
    Widget           active = XmTabStack__active_child(tab);
    XRectangle       *size = &(XmTabStack__size(tab));
    Boolean          stacked = False;
    int		     cnt = _XmTabbedStackListCount(XmTabStack__tab_list(tab));
    GC     	     gc = tab->manager.background_GC;

    if( XmTabStack_tab_mode(tab) == XmTABS_STACKED ||
	XmTabStack_tab_mode(tab) == XmTABS_STACKED_STATIC )
    {
	stacked = True;
    }

    /*
     * First lets relayout all our children.
     */
    Layout(tab);

    /*
     * If we are realized then we want to do something about our
     * shadows, like erase the old one and redraw them at their
     * new location.
     */
    if( XtIsRealized(widget) )
    {
	if( stacked || size->x != -1 )
	{
	    if( active != NULL && XiBackgroundSpecified(active) )
	    {
		gc = XmTabStack__gc(tab);
		SetChildGC(active, gc);
	    }
	    if( (!stacked || cnt < 1) && size->x != -1 )
	    {
		DrawShadows(tab, gc, gc,
			    size->x, size->y, size->width, size->height);
	    }
	}

	if( stacked && active != NULL && cnt > 0 )
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   active->core.x - BBPart(tab).margin_width,
			   active->core.y - BBPart(tab).margin_height,
			   XtWidth(active) +
			   2*BBPart(tab).margin_width,
			   XtHeight(active)+
			   2*BBPart(tab).margin_height);
	}
	else
	{
	    DrawShadows(tab, tab->manager.top_shadow_GC,
			tab->manager.bottom_shadow_GC, 0, 0,
			XtWidth(widget), XtHeight(widget));
	}

	size->x = size->y = 0;
	size->width = XtWidth(widget);
	size->height = XtHeight(widget);
    }
}

/*
 * Function:
 *	Redisplay(widget, event, region)
 * Description:
 *	Draws all the visuals for the TabStack.
 * Input:
 *	widget : Widget  - the widget that needs redisplayed
 *	event  : XEvent* - the event causing the redisplay
 *	region : Region  - the region that is being redisplayed.
 * Output:
 *	None.
 */
static void
Redisplay(Widget widget, XEvent *event, Region region)
{
    XmTabStackWidget tab = (XmTabStackWidget) widget;
    XRectangle             *size = &(XmTabStack__size(tab));
    Widget                 active = XmTabStack__active_child(tab);
    int                    cnt = _XmTabbedStackListCount(XmTabStack__tab_list(tab));
    Boolean		   stacked = False;

    if( XmTabStack_stacked_effect(tab) && active != NULL &&
        (XmTabStack_tab_mode(tab) == XmTABS_STACKED ||
	 XmTabStack_tab_mode(tab) == XmTABS_STACKED_STATIC) )
    {
	stacked = True;
    }

    /*
     * Now lets look at our active child and if they have a background
     * set lets color ourselves that background.
     */

    if( (active != NULL) &&
        (XiBackgroundSpecified(active) || XiSelectSpecified(tab)) )
    {
	GC        gc = XmTabStack__gc(tab);

	if( XiSelectSpecified(tab) )
	{
	    SetSelectGC(tab, gc);
	}
	else
	{
	    SetChildGC(active, gc);
	}

	if( stacked )
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   active->core.x - BBPart(tab).margin_width,
			   active->core.y - BBPart(tab).margin_height,
			   XtWidth(active) +
			   2 * (BBPart(tab).margin_width),
			   XtHeight(active) +
			   2 * (BBPart(tab).margin_height));
	}
	else
	{
	    if( event == NULL || event->xany.type != Expose )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			       0, 0, XtWidth(tab), XtHeight(tab));
	    }
	    else
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			       event->xexpose.x, event->xexpose.y,
			       event->xexpose.width, event->xexpose.height);
	    }
	}
    }

    /*
     * next if we have any gadget children lets be sure to
     * redisplay them.
     */
    XmeRedisplayGadgets(widget, event, region);

    /*
     * Now lets redray our shadows.
     */
    if( stacked && cnt > 0 )
    {
	DrawStackedShadows(tab, tab->manager.top_shadow_GC,
			   tab->manager.bottom_shadow_GC,
			   0, 0, XtWidth(active) +
			   (2 * BBPart(tab).margin_width),
			   XtHeight(active) +
			   (2 * BBPart(tab).margin_height));
    }
    else
    {
	DrawShadows(tab, tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC, 0, 0, XtWidth(widget),
		    XtHeight(widget));
    }

    size->x = size->y = 0;
    size->width = XtWidth(widget);
    size->height = XtHeight(widget);
}

/*
 * Function:
 *	SetValues(current, request, set, arg_list, arg_cnt)
 * Description:
 *	Handles changes to the widget induced by resource settings.
 * Input:
 *	current  : Widget    - the current state of the widget
 *	request  : Widget    - the request made by the caller
 *	set      : Widget    - the widget as it will appear
 *	arg_list : ArgList   - the argument list to implement the changes
 *	arg_cnt  : Cardinal* - the number of arguments
 * Output:
 *	Boolean - True if a redisplay is needed, else False.
 */
#define rfield(f) (XmTabStack_##f(r_tab))
#define cfield(f) (XmTabStack_##f(c_tab))
#define sfield(f) (XmTabStack_##f(s_tab))
#define bcfield(f) \
    (((XmBulletinBoardWidget) c_tab)->bulletin_board.f)
#define bsfield(f) \
    (((XmBulletinBoardWidget) s_tab)->bulletin_board.f)

static Boolean
SetValues(Widget current, Widget request, Widget set, ArgList arg_list,
	  Cardinal *arg_cnt)
{
    XmTabStackWidget c_tab = (XmTabStackWidget) current,
                     s_tab = (XmTabStackWidget) set;
    Arg              args[25];
    ArgList          filtered_args, merged_args;
    int 		n = 0;
    Cardinal         num_filtered_args;
    Boolean          need_redraw = False, need_resize = False,
                     need_layout = False;

    if( c_tab->manager.shadow_thickness != s_tab->manager.shadow_thickness )
    {
	need_resize = True;
	need_layout = True;
    }

    if( bcfield(margin_width) != bsfield(margin_width) ||
        bcfield(margin_height) != bsfield(margin_height) ||
        cfield(tab_side) != sfield(tab_side) ||
        cfield(tab_orientation) != sfield(tab_orientation) )
    {
	need_layout = True;
	need_resize = True;
    }

    if( cfield(select_color) != sfield(select_color) ||
        cfield(select_pixmap) != sfield(select_pixmap) )
    {
	need_redraw = True;
    }

    if( cfield(font_list) != sfield(font_list) )
    {
	XmFontListFree(cfield(font_list));
	if( sfield(font_list) == NULL )
	{
	    sfield(font_list) = XmeGetDefaultRenderTable(set, XmLABEL_FONTLIST);
	}
	sfield(font_list) = XmFontListCopy(sfield(font_list));
    }

    if( cfield(tab_side) != sfield(tab_side) ||
        cfield(tab_orientation) != sfield(tab_orientation) )
    {
	switch( sfield(tab_side) )
	{
	case XmTABS_ON_TOP:
	default:
	    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); ++n;
	    XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_BOTTOM_RIGHT); ++n;
	    if( sfield(tab_orientation) == XmTAB_ORIENTATION_DYNAMIC )
	    {
		XtSetArg(args[n], XmNtabOrientation, XmTABS_LEFT_TO_RIGHT);
	    }
	    else
	    {
		XtSetArg(args[n], XmNtabOrientation, sfield(tab_orientation));
	    }
	    n++;
	    break;
	case XmTABS_ON_BOTTOM:
	    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); ++n;
	    XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_TOP_LEFT); ++n;
	    if( sfield(tab_orientation) == XmTAB_ORIENTATION_DYNAMIC )
	    {
		XtSetArg(args[n], XmNtabOrientation, XmTABS_LEFT_TO_RIGHT);
	    }
	    else
	    {
		XtSetArg(args[n], XmNtabOrientation, sfield(tab_orientation));
	    }
	    n++;
	    break;
	case XmTABS_ON_RIGHT:
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); ++n;
	    XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_TOP_LEFT); ++n;
	    if( sfield(tab_orientation) == XmTAB_ORIENTATION_DYNAMIC )
	    {
		XtSetArg(args[n], XmNtabOrientation, XmTABS_TOP_TO_BOTTOM);
	    }
	    else
	    {
		XtSetArg(args[n], XmNtabOrientation, sfield(tab_orientation));
	    }
	    n++;
	    break;
	case XmTABS_ON_LEFT:
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); ++n;
	    XtSetArg(args[n], XmNtabEdge, XmTAB_EDGE_BOTTOM_RIGHT); ++n;
	    if( sfield(tab_orientation) == XmTAB_ORIENTATION_DYNAMIC )
	    {
		XtSetArg(args[n], XmNtabOrientation, XmTABS_BOTTOM_TO_TOP);
	    }
	    else
	    {
		XtSetArg(args[n], XmNtabOrientation, sfield(tab_orientation));
	    }
	    n++;
	    break;
	}
	need_layout = True;
	need_resize = True;
	need_redraw = True;
    }

    if( !sfield(uniform_tab_size) &&
        sfield(uniform_tab_size) != cfield(uniform_tab_size) &&
        (XmTabStack_tab_mode(s_tab) == XmTABS_STACKED ||
	 XmTabStack_tab_mode(s_tab) == XmTABS_STACKED_STATIC) )
    {
	XmeWarning(set, XmNillegalUniformTabSizeMsg);
	XtSetArg(args[n], XmNuniformTabSize, True); ++n;
    }

    if( (sfield(tab_mode) == XmTABS_STACKED ||
	 sfield(tab_mode) == XmTABS_STACKED_STATIC) &&
        sfield(tab_mode) != cfield(tab_mode) &&
        !sfield(uniform_tab_size) )
    {
	XmeWarning(set, XmNillegalUniformTabSizeMsg);
	XtSetArg(args[n], XmNuniformTabSize, True); ++n;
    }

    if( XmTabStack__inited(s_tab) && (XmTabStack_tab_box(s_tab) != NULL ))
    {
	_XmFilterArgs(arg_list, *arg_cnt, tab_stack_filter,
		      &filtered_args, &num_filtered_args);
	merged_args = XtMergeArgLists(filtered_args, num_filtered_args,
				      args, n);
	XtSetValues(XmTabStack_tab_box(s_tab), merged_args,
		    n + num_filtered_args); n = 0;
	XtFree((XtPointer)filtered_args);
	XtFree((XtPointer)merged_args);
    }

#ifdef TEAR_OFF_TABS
    if( cfield(tear_off_label) != sfield(tear_off_label) )
    {
	if( cfield(tear_off_label) != NULL )
	{
	    XmStringFree(cfield(tear_off_label));
	}

	if( sfield(tear_off_label) != NULL )
	{
	    sfield(tear_off_label) = XmStringCopy(sfield(tear_off_label));
	}
	XtVaSetValues(sfield(_tear_off_button),
		      XmNlabelString, sfield(tear_off_label),
		      NULL);
    }
#endif /* TEAR_OFF_TABS */

    if( need_layout )
    {
	Resize(set);
    }

    if( need_redraw )
    {
	Widget canvas = _XmTabBoxCanvas(XmTabStack_tab_box(s_tab));

	if( XtIsRealized(canvas) )
	{
	    XClearArea(XtDisplay(set), XtWindow(canvas), 0, 0, 0, 0, True);
	}
    }

    if( need_resize )
    {
	XtWidgetGeometry unused, allowed;

	unused.request_mode = 0;
	QueryGeometry(set, &unused, &allowed);

	if( BBPart(s_tab).resize_policy != XmRESIZE_NONE )
	{

	    /*
	     * Lets just adjust the values depending on our resize
	     * policy.
	     */
	    if( BBPart(s_tab).resize_policy == XmRESIZE_GROW )
	    {
		if( allowed.width < s_tab->core.width )
		{
		    allowed.width = s_tab->core.width;
		}
		if( allowed.height < s_tab->core.height )
		{
		    allowed.height = s_tab->core.height;
		}
	    }

	    if( XtWidth(request) == XtWidth(current) )

	    {
		s_tab->core.width = allowed.width;
	    }
	    if( XtHeight(request) == XtHeight(current) )
	    {
		s_tab->core.height = allowed.height;
	    }
	}
    }

    return( need_redraw || need_layout || need_resize );
}
#undef cfield
#undef rfield
#undef sfield
#undef bcfield
#undef bsfield

static XtGeometryResult
QueryGeometry(Widget widget, XtWidgetGeometry *request,
	      XtWidgetGeometry *allowed)
{
    XmTabStackWidget tab = (XmTabStackWidget)widget;
    WidgetList       kid;
    XtWidgetGeometry tab_wanted;
    Cardinal         i;
    Boolean	     have_width, have_height;
    int              kid_width = 0, kid_height = 0, width, height;

    /*
     * First lets check to see if the caller is interested in the
     * same things we are, i.e. if the caller does not care about
     * the width or height then we will accept whatever they
     * give us.
     */
    if( request->request_mode != 0 &&
        !(request->request_mode & (CWWidth | CWHeight)) )
    {
	return( XtGeometryYes );
    }

    *allowed = *request;
    allowed->request_mode = CWWidth | CWHeight;

    /*
     * If we get this far we know that they care about our Dimensions
     * so lets grab the desired width/height of our kids and see what
     * they want.  We calculate our kids width by taking the max of
     * all the widths and heights and then adding room for the margins.
     */
    for( i = 0, kid = tab->composite.children;
	 i < tab->composite.num_children; ++i )
    {
	if( !IsValidChild(tab, kid[i]) ) continue;

	AssignMax(kid_width, (int)XmTabStackC_width(kid[i]));
	AssignMax(kid_height, (int)XmTabStackC_height(kid[i]));
    }

    /*
     * Now that we know the bounding box of all the children lets add the
     * margin to that area.
     */
    width = kid_width += 2 * BBPart(tab).margin_width +
			      tab->manager.shadow_thickness;

    height = kid_height += 2 * BBPart(tab).margin_height +
				tab->manager.shadow_thickness;
    if (XmTabStack_tab_side(tab) == XmTABS_ON_TOP
            || XmTabStack_tab_side(tab) == XmTABS_ON_BOTTOM)
    {
        width += tab->manager.shadow_thickness;
    } else /* if (XmTabStack_tab_side(tab) == XmTABS_ON_LEFT
            || XmTabStack_tab_side(tab) == XmTABS_ON_RIGHT) */
    {
        height += tab->manager.shadow_thickness;
    }

    if( LocalTabBox(tab) )
    {
	/*
	 * Now lets find out what size the TabBox wants to be.
	 */
	if( XmTabStack_tab_mode(tab) == XmTABS_STACKED ||
	   XmTabStack_tab_mode(tab) == XmTABS_STACKED_STATIC )
	{
	    XRectangle tab_rect;

	    switch( XmTabStack_tab_side(tab) )
	    {
	    case XmTABS_ON_TOP:
	    case XmTABS_ON_BOTTOM:
		_XmTabBoxStackedGeometry((XmTabBoxWidget)XmTabStack_tab_box(tab),
					 width, &tab_rect);
		break;
	    case XmTABS_ON_RIGHT:
	    case XmTABS_ON_LEFT:
		_XmTabBoxStackedGeometry((XmTabBoxWidget)XmTabStack_tab_box(tab),
					 height, &tab_rect);
		break;
	    }
	    tab_wanted.width = tab_rect.width;
	    tab_wanted.height = tab_rect.height;
	}
	else
	{
	    XtQueryGeometry(XmTabStack_tab_box(tab), NULL, &tab_wanted);
	}


	/*
	 * Now we need to look at the placement of the TabBox and
	 * determine how we should merge its desired size in with the
	 * size of the children.
	 */
	switch( XmTabStack_tab_side(tab) )
	{
	case XmTABS_ON_TOP:
	case XmTABS_ON_BOTTOM:
	default:
	    /*
	     * For these cases we need to take the max of the width of the
	     * children and the TabBox and then add the TabBox's height
	     * to the children's.
	     */
	    AssignMax(width, (int)tab_wanted.width);
	    height += tab_wanted.height;
	    break;
	case XmTABS_ON_RIGHT:
	case XmTABS_ON_LEFT:
	    /*
	     * For these cases we need to take the max of the height of the
	     * children and the TabBox and then add the TabBox's width
	     * to the children's.
	     */
	    width += tab_wanted.width;
	    AssignMax(height, (int)tab_wanted.height);
	    break;
	}
    }

    /*
     * Lets first check the simple case.  This is the case where the
     * caller is interested in the geometry that we would like to
     * be.
     */
    if( request->request_mode == 0 )
    {
	/*
	 * Well it seems that the caller is just interested in the
	 * size that we want to be, so lets stuff that away to our
	 * return structure and send that back.
	 */
	allowed->width = width;
	allowed->height = height;

	/*
	 * Before we give a result lets see if the geometry that
	 * we would give back is our current geometry and if it is
	 * we will return XtGeometryNo to indicate that we want to
	 * stay the way we are.
	 */
	if( XmCompareXtWidgetGeometryToWidget(allowed, widget) )
	{
	    return( XtGeometryNo );
	}

	return( XtGeometryYes );
    }

    /*
     * If we got here that means that the caller wants to specify
     * either our width or height and see how we would react to such
     * a change.  So lets what they want.
     */
    have_width = ((request->request_mode & CWWidth) != 0);
    have_height = ((request->request_mode & CWHeight) != 0);

    if( have_width && have_height )
    {
	/*
	 * It seems that the caller wants to specify our with and
	 * height.  If this is the case then we want to tell them that
	 * we prefer our desired geometry.
	 */
	allowed->width = width;
	allowed->height = height;
	if( XmCompareXtWidgetGeometryToWidget(allowed, widget) )
	{
	    return( XtGeometryNo );
	}

	if( XmCompareXtWidgetGeometry(request, allowed) )
	{
	    return( XtGeometryYes );
	}

	return( XtGeometryAlmost );
    }

    /*
     * Finally the hard part.  Here it seems that the caller wants to
     * freeze either our width or height and have us adjust the other
     * dimension.
     */
    if( have_width )
    {
	/*
	 * Ok it seems that the caller wants to freeze our width so lets
	 * find out what we can do about this.  This case is only interesting
	 * if the tabs are on the top or the bottom.  Because if they are
	 * on the sides this case diverts to the we want our preferred geomtry
	 * case because we don't want to shrink our kids below their
	 * preferred size and we don't want to shrink the tabs height.
	 */
	switch( XmTabStack_tab_side(tab) )
	{
	case XmTABS_ON_RIGHT:
	case XmTABS_ON_LEFT:
	    /*
	     * This is the boring case and degrades to the "we want our
	     * preferred geometry" case.
	     */
	    allowed->width = width;
	    allowed->height = height;
	    break;
	case XmTABS_ON_TOP:
	case XmTABS_ON_BOTTOM:
	default:
	    /*
	     * Here is the interesting case.  The uses has frozen our width
	     * but not our height.  Now as long as the width they specified
	     * is greater than the width of the kids we are in business, else
	     * we degrade to give me what I want.
	     */
	    if( (int)request->width < kid_width )
	    {
		/*
		 * The degraded case: "Give me what I want"
		 */
		allowed->width = width;
		allowed->height = height;
	    }
	    else
	    {
		XtWidgetGeometry ask, got;
		/*
		 * We actually get to have some fun now.  What we now
		 * do is ask the TabBox what it is willing to do
		 * "heightwise" if we freeze its width.
		 */
		ask.request_mode = CWWidth;
		ask.width = request->width;
		got.request_mode = 0;
		switch( XtQueryGeometry(XmTabStack_tab_box(tab), &ask, &got) )
		{
		case XtGeometryYes:
		case XtGeometryAlmost:
		default:
		    if( !(got.request_mode & CWWidth) )
		    {
			got.width = tab_wanted.width;
		    }
		    if( !(got.request_mode & CWHeight) )
		    {
			got.height = tab_wanted.height;
		    }

		    /*
		     * Now we have the height that the TabBox is willing
		     * to take if we freeze its width so lets
		     * store this information back into our return
		     * structure.
		     */
		    allowed->width = Max(kid_width, (int)got.width);
		    allowed->height = kid_height + got.height;
		    break;
		case XtGeometryNo:
		    allowed->width = width;
		    allowed->height = height;
		    break;
		}
	    }
	    break;
	}
    }
    else if( have_height )
    {
	/*
	 * It seems that the caller want to freeze our height and see how
	 * we react "widthwise".  This case degrades to the default if
	 * the tabs are on the right or left.
	 */
	switch( XmTabStack_tab_side(tab) )
	{
	case XmTABS_ON_TOP:
	case XmTABS_ON_BOTTOM:
	default:
	    allowed->width = width;
	    allowed->height = height;
	    break;
	case XmTABS_ON_RIGHT:
	case XmTABS_ON_LEFT:

	    /*
	     * First we will do a quick check to make sure that the height
	     * they are asking about will fit all the children at least, and
	     * if not we will reject it out of hand.
	     */
	    if( (int)request->height < kid_height )
	    {
		allowed->width = width;
		allowed->height = height;
	    }
	    else
	    {
		XtWidgetGeometry ask, got;

		/*
		 * Now letws see how the TabBox reacts to the new size.
		 */
		ask.request_mode = CWHeight;
		ask.request_mode = request->height;
		got.request_mode = 0;
		switch( XtQueryGeometry(XmTabStack_tab_box(tab), &ask, &got) )
		{
		case XtGeometryYes:
		case XtGeometryAlmost:
		default:
		    if( !(got.request_mode & CWWidth) )
		    {
			got.width = tab_wanted.width;
		    }
		    if( !(got.request_mode & CWHeight) )
		    {
			got.height = tab_wanted.height;
		    }

		    /*
		     * Now we have the info we need so lets stuff that back
		     * into our return structure.
		     */
		    allowed->width = kid_width + got.width;
		    allowed->height = Max(kid_height, (int)got.height);
		    break;
		case XtGeometryNo:
		    allowed->width = width;
		    allowed->height = height;
		    break;
		}
	    }
	    break;
	}
    }

    if( XmCompareXtWidgetGeometryToWidget(allowed, widget) )
    {
	return( XtGeometryNo );
    }
    else if( XmCompareXtWidgetGeometry(request, allowed) )
    {
	return( XtGeometryYes );
    }

    return( XtGeometryNo );
}

#if 0
#define XiReturn(i,r) \
    printf("%d: RESULT: XtGeometry", i); \
    switch( r ) \
    { \
    case XtGeometryYes: \
    default: \
	printf("Yes"); \
	break; \
    case XtGeometryNo: \
	printf("No"); \
	break; \
    case XtGeometryAlmost: \
	printf("Almost"); \
	break; \
    case XtGeometryDone: \
	printf("Done"); \
	break; \
    } \
    if( request->request_mode & XtCWQueryOnly ) printf("(QUERY)\n"); \
    else printf("\n"); \
    printf("  WIDGET: %s\n", XtName(widget)); \
    printf("  X      : "); \
    if( allowed->request_mode & CWX ) printf("%d\n", allowed->x); \
    else printf("N/A\n"); \
    printf("  Y      : "); \
    if( allowed->request_mode & CWY ) printf("%d\n", allowed->y); \
    else printf("N/A\n"); \
    printf("  WIDTH  : "); \
    if( allowed->request_mode & CWWidth ) printf("%d\n", allowed->width); \
    else printf("N/A\n"); \
    printf("  HEIGHT : "); \
    if( allowed->request_mode & CWHeight ) printf("%d\n", allowed->height); \
    else printf("N/A\n"); \
    printf("  BORDER : "); \
    if( allowed->request_mode & CWBorderWidth ) \
	printf("%d\n", allowed->border_width); \
    else printf("N/A\n"); \
    printf("\n"); \
    return(r);
#else
#define XiReturn(i,r) return( r )
#endif

static XtGeometryResult
GeometryNo(XmTabStackWidget tab, Widget widget, XtWidgetGeometry *request,
	   XtWidgetGeometry *allowed)
{
    XRectangle box, kid;

    /*
     * If we are not dealing with the tab box child then we really want
     * to return No, so lets to that.
     */
    if( widget != XmTabStack_tab_box(tab) )
    {
	return( XtGeometryNo );
    }

    /*
     * If we got here that means that we know we are dealing with our
     * tab box child, and we are about to deny a size request from that
     * child.  The problem is that we do not want to do this. Actually
     * we want to resize the tab box child to fix in the space provided.
     */
    PickSizes(tab, XtWidth(tab), XtHeight(tab), &box, &kid);

    allowed->request_mode = CWWidth | CWHeight;
    allowed->width = box.width;
    allowed->height = box.height;

    if( XmCompareXtWidgetGeometryToWidget(allowed,
					  XmTabStack_tab_box(tab)) )
    {
	return( XtGeometryNo );
    }
    if( XmCompareXtWidgetGeometry(request, allowed) )
    {
	if( !(request->request_mode & XtCWQueryOnly) )
	{
	    Resize((Widget)tab);
	    if( XtIsRealized((Widget)tab) ) Redisplay((Widget)tab,NULL,False);
	}
	return( XtGeometryYes );
    }

    return( XtGeometryAlmost );
}

/*
 * Function:
 *	GeometryManager(widget, request, allowed)
 * Description:
 *	This function handles geometry requests. These requests are
 *	sent from the children.
 * Input:
 *	widget  : Widget            - the widget making the request.
 *	request : XtWidgetGeometry* - the size the child wants
 *	allowed : XtWidgetGeometry* - the size the child gets
 * Output:
 *	XtGeometryResult - specifies information about the allowed geometry
 *			   result.
 */
static XtGeometryResult
GeometryManager(Widget widget, XtWidgetGeometry *request,
		XtWidgetGeometry *allowed)
{
    XmTabStackWidget tab = (XmTabStackWidget) XtParent(widget);
    int              save_width, save_height, save_border;
    Dimension        child_save_width, child_save_height;
    XtWidgetGeometry want, got;
    XRectangle       box, kids;

#if 0
    printf("REQUEST: ");
    if( request->request_mode & XtCWQueryOnly ) printf("QUERY");
    printf("\n");
    printf("  WIDGET: %s\n", XtName(widget));
    printf("  X      : ");
    if( request->request_mode & CWX ) printf("%d\n", request->x);
    else printf("N/A\n");
    printf("  Y      : ");
    if( request->request_mode & CWY ) printf("%d\n", request->y);
    else printf("N/A\n");
    printf("  WIDTH  : ");
    if( request->request_mode & CWWidth ) printf("%d\n", request->width);
    else printf("N/A\n");
    printf("  HEIGHT : ");
    if( request->request_mode & CWHeight ) printf("%d\n", request->height);
    else printf("N/A\n");
    printf("  BORDER : ");
    if( request->request_mode & CWBorderWidth )
	printf("%d\n", request->border_width);
    else printf("N/A\n");
    printf("\n");
#endif

    if( XmTabStack__set_tab_list(tab) &&
        (request->request_mode & CWBorderWidth) )
    {
	XmTabStack__set_tab_list(tab) = False;
	XtVaSetValues(XmTabStack_tab_box(tab),
		      XmNtabList, XmTabStack__tab_list(tab),
		      NULL);
#ifdef TEAR_OFF_TABS
	BuildMenu(tab);
#endif
	XiReturn( 1, XtGeometryNo );
    }

    *allowed = *request;
    /*
     * Ok playing with geometry is real easy.  This is because we only
     * let our children change their dimensions and not their location.
     * So if the caller wants to muck with their X or Y we ignore it.
     *
     * Also stacking order does not matter because only one child is
     * visible at a time.
     */
    allowed->request_mode &= ~(CWX | CWY | CWStackMode | CWSibling);

    /*
     * If the child wants does not want to change its dimensions (width,
     * height or border width) then we are done.
     */
    if( !(allowed->request_mode & (CWWidth | CWHeight | CWBorderWidth)) )
    {
	/*
	 * It turns out that we have weeded out everything from their
	 * request which means that we are not excepting any of their
	 * changes and we can just return XtGeometryNo.
	 */
	XiReturn( 2, XtGeometryNo );
    }

    /*
     * Lets find out what size we want to be if we accept this kids
     * request.
     */
    save_width = XtWidth(widget);
    save_height = XtHeight(widget);
    save_border = widget->core.border_width;
    if( allowed->request_mode & CWWidth )
    {
	widget->core.width = allowed->width;
	XmTabStackC_width(widget) = allowed->width;
    }
    if( allowed->request_mode & CWHeight )
    {
	widget->core.height = allowed->height;
        XmTabStackC_height(widget) = allowed->height;
    }
    if( allowed->request_mode & CWBorderWidth )
    {
	widget->core.border_width = allowed->border_width;
    }

    child_save_width = widget->core.width;
    child_save_height = widget->core.height;

    want.request_mode = 0;
    QueryGeometry((Widget)tab, &want, &got);
    widget->core.width = save_width;
    widget->core.height = save_height;
    widget->core.border_width = save_border;

    XmTabStackC_width(widget) = save_width;
    XmTabStackC_height(widget) = save_height;

    /*
     * Lets store away the width and height we want.
     */
    save_width = XtWidth(tab);
    save_height = XtHeight(tab);
    if( got.request_mode & CWWidth )
    {
	save_width = got.width;
    }
    if( got.request_mode & CWHeight )
    {
	save_height = got.height;
    }

    /*
     * Lets first check to see if we can satify this request without
     * changing our size and if so we will just return yes.  We will
     * also use this chance to check our resize policy.
     */
#ifdef DKB
    if( save_width == XtWidth(tab) && save_height == XtHeight(tab) )
    {
	/*
	 * We can satify this request without changing our size so
	 * lets retrun XtGeometryNo, to let the kid know it should
	 * stay the same size.
	 */
	XiReturn( 3, GeometryNo(tab, widget, request, allowed) );
    }
#endif

    /*
     * If we got here that means that the our child wants us to change
     * our size, so lets see if our resize polify will allow this
     * chnage.
     */
    if( BBPart(tab).resize_policy == XmRESIZE_NONE ||
        (BBPart(tab).resize_policy == XmRESIZE_GROW &&
	 (save_width < (int)XtWidth(tab) ||save_height < (int)XtHeight(tab))) )
    {
	/*
	 * Well we cannot allow this change so what we want is our child
	 * to stay the same size it is, so lets return XtGeometryNo.
	 */
	XiReturn( 4, GeometryNo(tab, widget, request, allowed) );
    }

    /*
     * Now we know what size we want to be if we accept this kids request
     * so now lets if we can be this size.
     */
    want.request_mode = CWWidth | CWHeight | XtCWQueryOnly;
    want.width = save_width;
    want.height = save_height;

    switch( XtMakeGeometryRequest((Widget)tab, &want, &got) )
    {
    case XtGeometryYes:
    default:
	/*
	 * Well our parent said yes, so we can accept the request from
	 * our kid, so what we will do is now check if they really want
	 * us to do it or if this was a query.
	 */

	if( (request->request_mode & XtCWQueryOnly) == 0 )
	{
	    /*
	     * This was not a query so we now really want to make the
	     * request to our parent.  We don't care about the return
	     * value from this call because the parent already said that
	     * they would accept this.
	     */
	    XtMakeResizeRequest((Widget)tab, save_width, save_height,
				NULL, NULL);

	    XmTabStackC_width(widget) = child_save_width;
	    XmTabStackC_height(widget) = child_save_height;

	    Resize((Widget)tab);
	    if( XtIsRealized((Widget)tab) ) Redisplay((Widget)tab,NULL,False);
	}
	XiReturn( 5, XtGeometryYes );
    case XtGeometryNo:
	/*
	 * Our parent said no, so we must also say no.
	 */
	XiReturn( 6, GeometryNo(tab, widget, request, allowed) );
    case XtGeometryAlmost:
	/*
	 * Out parent said almost, so we need to figure out how this
	 * affects our children, i.e. what is our children's dimensions
	 * with this change.
	 */
	break;
    case XtGeometryDone:
	XmTabStackC_width(widget) = child_save_width;
	XmTabStackC_height(widget) = child_save_height;

	XiReturn( 99, XtGeometryDone );
	break;
    }

    /*
     * This is the almost case. To get here we have had to ask our parent for
     * a size change, and our parent said we could not be the size that
     * we want to be, but we could be a "close" size. So what we have to
     * do is figure out how this affects our children and return the
     * proper response.
     */

    /*
     * First lets see if we can allow the size change that our parent
     * has said OK to.  This means that we have fake a layout to find
     * out what size our tabbox and children will be.
     */
    if( BBPart(tab).resize_policy == XmRESIZE_GROW &&
        (got.width < XtWidth(tab) || got.height < XtHeight(tab)) )
    {
	/*
	 * It turns out that our resize policy want allow us to make
	 * this compromise so lets just return no.
	 */
	XiReturn( 7, GeometryNo(tab, widget, request, allowed) );
    }

    /*
     * Ok we could accept this compromise, so lets see how it effects
     * the size of our kids.
     */
    PickSizes(tab, got.width, got.height, &box, &kids);

    allowed->request_mode |= (CWWidth | CWHeight);
    if( widget == XmTabStack_tab_box(tab) )
    {
	/*
	 * In this case we are dealing with our tab box child, so
	 * lets set the almost result to size we picked.
	 */
	allowed->width = box.width;
	allowed->height = box.height;
    }
    else
    {
	/*
	 * In this case we are dealing with our children, so lets
	 * set the almost return value.
	 */
	allowed->width = kids.width;
	allowed->height = kids.height;
    }

    XiReturn( 8, XtGeometryAlmost );
}

static void
ChangeManaged(Widget widget)
{
    XmTabStackWidget  tab = (XmTabStackWidget) widget;
    WidgetList        kid;
    Widget	      active;
    int		      idx = 0;
    Cardinal          i;
    XmTabbedStackList         tl = NULL;
    XmTabAttributeRec attr;
    XtWidgetGeometry  request, geom;
    XmString          xmstr;
    Boolean           changed_size = False;
    Boolean           select_new_kid = False;

    if( !XmTabStack__inited(tab) ) return;

    /*
     * Lets first find out if our active child is still managed, and
     * if it is not then lets NULLify our active child.
     */
    if( XmTabStack__active_child(tab) != NULL &&
        !XtIsManaged(XmTabStack__active_child(tab)) )
    {
	XmTabStack__active_child(tab) = (Widget) NULL;
    }

    /*
     * If we do not have an active child then lets find the first
     * managed child and make it active.  We will walk the whole list
     * of children and make sure that the active child is the only
     * child that is visible.
     */
    for( i = 0, kid = tab->composite.children;
	 i < tab->composite.num_children; ++i, ++kid )
    {
	if( IsTabBox(tab, *kid) ) continue;

	if( !XtIsManaged(*kid) )
	{
	    /*
	     * If this child is not managed then lets zero out
	     * the widgets managed width and height so that
	     * when it becomes managed again we will grab the
	     * new managed width/height.
	     */
	    XmTabStackC_width(*kid) = 0;
	    XmTabStackC_height(*kid) = 0;
	    XmTabStackC_index(*kid) = -1;
	    continue;
	}

	XmTabStackC_index(*kid) = idx++;

	if( XmTabStack__active_child(tab) == NULL )
	{
	    select_new_kid = True;

	    XmTabStack__active_child(tab) = *kid;
	}

	/*
	 * Now that we know that we are dealing with a managed child
	 * lets add them to the tab list we are building up to
	 * pass off to the TabBox.
	 */
	if( tl == NULL )
	{
	    tl = XmTabbedStackListCreate();
	}

	if( XmTabStackC_tab_label_string(*kid) == NULL )
	{
	    xmstr = attr.label_string = XmStringCreateLocalized(XtName(*kid));
	    attr.value_mode = XmTAB_VALUE_COPY;
	}
	else
	{
	    xmstr = NULL;
	    attr.label_string = XmTabStackC_tab_label_string(*kid);
	    attr.value_mode = XmTAB_VALUE_SHARE;
	}
	attr.string_direction = XmTabStackC_tab_string_direction(*kid);
	attr.label_pixmap = XmTabStackC_tab_label_pixmap(*kid);
	attr.label_alignment = XmTabStackC_tab_alignment(*kid);
	attr.pixmap_placement = XmTabStackC_tab_pixmap_placement(*kid);
	attr.foreground = XmTabStackC_tab_foreground(*kid);
	attr.background = XmTabStackC_tab_background(*kid);
	attr.background_pixmap = XmTabStackC_tab_background_pixmap(*kid);
	attr.sensitive = XtIsSensitive(*kid);
	XmTabbedStackListAppend(tl, XmTAB_ALL_FLAGS, &attr);
	if( xmstr != NULL ) XmStringFree(xmstr);

	/*
	 * Next lets check if this kid has its managed width and
	 * height set and if not lets set it.
	 */
	if( XmTabStackC_width(*kid) == 0 )
	{
	    XmTabStackC_width(*kid) = XtWidth(*kid);
	    XmTabStackC_height(*kid) = XtHeight(*kid);
	}
    }

    if( (active = XmTabStack__active_child(tab)) != NULL )
    {
	XtVaSetValues(XmTabStack_tab_box(tab),
		      XmNtabList, tl,
		      XmNselectedIndex, XmTabStackC_index(active),
		      XmNtraversalIndex, XmTabStackC_index(active),
		      NULL);
    }
    else
    {
	XtVaSetValues(XmTabStack_tab_box(tab),
		      XmNtabList, tl,
		      NULL);
    }
    if( XmTabStack__tab_list(tab) != NULL )
    {
	XmTabbedStackListFree(XmTabStack__tab_list(tab));
    }
    XmTabStack__tab_list(tab) = tl;

#ifdef TEAR_OFF_TABS
    BuildMenu(tab);
#endif

    /*
     * Now lets see how this new kid affects our geometry and request
     * a new size.
     */
    request.request_mode = 0;
    QueryGeometry((Widget)tab, &request, &geom);

    /*
     * Now that we know the size that we want to be lets see if we are
     * that size and if not lets request a new size.  But before we do this
     * lets check our resize policy to see if we want to change size.
     */
    if( (geom.width != XtWidth(tab) || geom.height != XtHeight(tab)) &&
        BBPart(tab).resize_policy != XmRESIZE_NONE )
    {
	Dimension width, height;

	if( BBPart(tab).resize_policy != XmRESIZE_GROW ||
	    (geom.width >= XtWidth(tab) && geom.height >= XtHeight(tab)) )
	{
	    switch( XtMakeResizeRequest((Widget)tab, geom.width, geom.height,
					&width, &height) )
	    {
	    case XtGeometryYes:
		changed_size = True;
	    case XtGeometryNo:
	    default:
		break;
	    case XtGeometryAlmost:
		/*
		 * Our parent gave us an alternate suggestion so lets do what
		 * mom or dad says and take it.
		 */
		if( BBPart(tab).resize_policy != XmRESIZE_GROW ||
		    (width >= XtWidth(tab) && height >= XtHeight(tab)) )
		{
		    XtMakeResizeRequest((Widget)tab, width, height, NULL,
					NULL);
		    changed_size = True;
		}
		break;
	    }
	}
    }
    Resize((Widget)tab);
    if( !changed_size && XtIsRealized((Widget)tab) )
    {
	Redisplay((Widget)tab, NULL, False);
    }

    /*
     * If new tab is selected as a result of a manage set changed, then notify
     * the next tab stack widget through callbacks that it is now selected...
     */
    if (select_new_kid)
    {
	XmTabStackCallbackStruct	cbdata;

	cbdata.reason = XmCR_TAB_SELECTED;
	cbdata.event = NULL;
	cbdata.selected_child = XmTabStack__active_child(tab);

	XtCallCallbackList((Widget)tab, XmTabStack_tab_select_callback(tab),
			   (XtPointer) &cbdata);
    }
}

static void
ConstraintInitialize(Widget request, Widget set, ArgList arg_list,
		     Cardinal *arg_cnt)
{
    XmTabStackWidget tab = (XmTabStackWidget) XtParent(set);

    if( XmTabStackC_tab_label_string(set) != NULL )
    {
	XmTabStackC_tab_label_string(set) = XmStringCopy(XmTabStackC_tab_label_string(set));
    }

    if( XmTabStackC_tab_string_direction(set) == XmSTRING_DIRECTION_DEFAULT )
    {
	XmTabStackC_tab_string_direction(set) = tab->manager.string_direction;
    }

    XmTabStackC_width(set) = 0;
    XmTabStackC_height(set) = 0;
    XmTabStackC_index(set) = -1;
}

static void
ConstraintDestroy(Widget widget)
{
    if( XmTabStackC_tab_label_string(widget) != NULL )
    {
	XmStringFree(XmTabStackC_tab_label_string(widget));
    }
    if( XmTabStackC_free_tab_pixmap(widget) && ValidPixmap(XmTabStackC_tab_label_pixmap(widget)) )
    {
	XFreePixmap(XtDisplay(widget), XmTabStackC_tab_label_pixmap(widget));
    }
}

#define cfield(f) (XmTabStackC_##f(current))
#define rfield(f) (XmTabStackC_##f(request))
#define sfield(f) (XmTabStackC_##f(set))

static Boolean
ConstraintSetValues(Widget current, Widget request, Widget set,
		    ArgList arg_list, Cardinal *arg_cnt)
{
    XmTabStackWidget  tab = (XmTabStackWidget) XtParent(set);
    XmTabAttributeRec attr;
    WidgetList        kid;
    Cardinal          i;

    /*
     * If any of the following have changed that means we need to
     * build a new TabList and send it to our child.
     */
    if( cfield(tab_label_string) != sfield(tab_label_string) ||
        cfield(tab_alignment) != sfield(tab_alignment) ||
        cfield(tab_string_direction ) != sfield(tab_string_direction) ||
        cfield(tab_label_pixmap ) != sfield(tab_label_pixmap) ||
        cfield(tab_pixmap_placement ) != sfield(tab_pixmap_placement) ||
        cfield(tab_background ) != sfield(tab_background) ||
        cfield(tab_background_pixmap) != sfield(tab_background_pixmap) ||
        cfield(tab_foreground ) != sfield(tab_foreground) ||
	XtIsSensitive(current) != XtIsSensitive(set) )
    {
	XmTabbedStackList tl = NULL;
	XmString  xmstr;

	if( cfield(tab_label_string) != sfield(tab_label_string) )
	{
	    if( cfield(tab_label_string) != NULL )
	    {
		XmStringFree(cfield(tab_label_string));
	    }

	    sfield(tab_label_string) = XmStringCopy(sfield(tab_label_string));
	}

	for( i = 0, kid = tab->composite.children;
	     i < tab->composite.num_children; ++i, ++kid )
	{
	    if( !IsValidChild(tab, *kid) ) continue;

	    if( tl == NULL ) tl = XmTabbedStackListCreate();

	    if( XmTabStackC_tab_label_string(*kid) == NULL )
	    {
		xmstr = attr.label_string =
		    XmStringCreateLocalized(XtName(*kid));
		attr.value_mode = XmTAB_VALUE_COPY;
	    }
	    else
	    {
		xmstr = NULL;
		attr.label_string = XmTabStackC_tab_label_string(*kid);
		attr.value_mode = XmTAB_VALUE_SHARE;
	    }
	    attr.string_direction = XmTabStackC_tab_string_direction(*kid);
	    attr.label_pixmap = XmTabStackC_tab_label_pixmap(*kid);
	    attr.label_alignment = XmTabStackC_tab_alignment(*kid);
	    attr.pixmap_placement = XmTabStackC_tab_pixmap_placement(*kid);
	    attr.foreground = XmTabStackC_tab_foreground(*kid);
	    attr.background = XmTabStackC_tab_background(*kid);
	    attr.background_pixmap = XmTabStackC_tab_background_pixmap(*kid);
	    attr.sensitive = XtIsSensitive(*kid);
	    XmTabbedStackListAppend(tl, XmTAB_ALL_FLAGS, &attr);
	    if( xmstr != NULL ) XmStringFree(xmstr);
	}

	if( XmTabStack__tab_list(tab) != NULL )
	{
	    XmTabbedStackListFree(XmTabStack__tab_list(tab));
	}
	XmTabStack__tab_list(tab) = tl;

	if( XtIsManaged(set) && XtIsRealized(set) )
	{
	    set->core.border_width++;
	    XmTabStack__set_tab_list(tab) = True;
	}
    }

    /*
     * If the background color for our tab changes and this is the
     * active tab then we need cause a refresh to happen in the stack.
     */
    if( (cfield(tab_background ) != sfield(tab_background) ||
	 cfield(tab_background_pixmap) != sfield(tab_background_pixmap)) &&
         XtIsRealized((Widget)tab) )
    {
	XClearArea(XtDisplay(tab), XtWindow(tab), 0, 0, 0, 0, True);
    }

    return( False );
}
#undef cfield
#undef rfield
#undef sfield

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
CvtStringToXiPixel(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		   XrmValue *from, XrmValue *to, XtPointer *data)
{
    static Pixel result = XmCOLOR_DYNAMIC;
    String	 str = (String) from->addr;
    Widget       widget;

    widget = *((Widget*) arg_list[0].addr);

    if( XmCompareISOLatin1(str, "color_dynamic") == 0 ||
        XmCompareISOLatin1(str, "dynamic") == 0 )
    {
	XiCvtDone(Pixel, result);
    }

    return( XtConvertAndStore(widget, XmRString, from, XmRPixel, to) );
}

static Boolean
CvtStringToXiPixmap(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		    XrmValue *from, XrmValue *to, XtPointer *data)
{
    static Pixmap result = XmPIXMAP_DYNAMIC;
    String	  str = (String) from->addr;
    Widget        widget;

    widget = *((Widget*) arg_list[0].addr);

    if( XmCompareISOLatin1(str, "pixmap_dynamic") == 0 ||
        XmCompareISOLatin1(str, "dynamic") == 0 )
    {
	XiCvtDone(Pixel, result);
    }

    return( XtConvertAndStore(widget, XmRString, from, XmRPrimForegroundPixmap, to) );
}

static Boolean
CvtStringToXmTabSide(Display *dpy, XrmValue *arg_list, Cardinal *arg_cnt,
		     XrmValue *from, XrmValue *to, XtPointer *data)
{
    static int result = XmTABS_ON_TOP;
    String     str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "TOP") == 0 ||
        XmCompareISOLatin1(str, "XmTABS_ON_TOP") == 0 )
    {
	result = XmTABS_ON_TOP;
    }
    else if( XmCompareISOLatin1(str, "BOTTOM") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_BOTTOM") == 0 )
    {
	result = XmTABS_ON_BOTTOM;
    }
    else if( XmCompareISOLatin1(str, "LEFT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_LEFT") == 0 )
    {
	result = XmTABS_ON_LEFT;
    }
    else if( XmCompareISOLatin1(str, "RIGHT") == 0 ||
	     XmCompareISOLatin1(str, "XmTABS_ON_RIGHT") == 0 )
    {
	result = XmTABS_ON_RIGHT;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRXmTabSide);
	return( False );
    }

    XiCvtDone(int, result);
}

static Boolean
CvtStringToXmPixmapPlacement(Display *dpy, XrmValue *arg_list,
			     Cardinal *arg_cnt, XrmValue *from, XrmValue *to,
			     XtPointer *data)
{
    static XmPixmapPlacement result = XmPIXMAP_RIGHT;
    String                   str = (String) (from->addr);

    if( XmCompareISOLatin1(str, "TOP") == 0 ||
        XmCompareISOLatin1(str, "XmPIXMAP_TOP") == 0 )
    {
	result = XmPIXMAP_TOP;
    }
    else if( XmCompareISOLatin1(str, "BOTTOM") == 0 ||
	     XmCompareISOLatin1(str, "XmPIXMAP_BOTTOM") == 0 )
    {
	result = XmPIXMAP_BOTTOM;
    }
    else if( XmCompareISOLatin1(str, "RIGHT") == 0 ||
	     XmCompareISOLatin1(str, "XmPIXMAP_RIGHT") == 0 )
    {
	result = XmPIXMAP_RIGHT;
    }
    else if( XmCompareISOLatin1(str, "LEFT") == 0 ||
	     XmCompareISOLatin1(str, "XmPIXMAP_LEFT") == 0 )
    {
	result = XmPIXMAP_LEFT;
    }
    else if( XmCompareISOLatin1(str, "NONE") == 0 ||
	     XmCompareISOLatin1(str, "XmPIXMAP_NONE") == 0 )
    {
	result = XmPIXMAP_NONE;
    }
    else if( XmCompareISOLatin1(str, "ONLY") == 0 ||
	     XmCompareISOLatin1(str, "XmPIXMAP_ONLY") == 0 )
    {
	result = XmPIXMAP_ONLY;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, str, XmRXmPixmapPlacement);
	return( False );
    }

    XiCvtDone(XmPixmapPlacement, result);
}

static void
DrawShadows(XmTabStackWidget tab, GC top_GC, GC bottom_GC, int x, int y,
	    int width, int height)
{
    XRectangle             top_rects[2], bottom_rects[2];
    XPoint                 bevel;
    int                    num_top_rects = 0, num_bottom_rects = 0,
                           shadow = tab->manager.shadow_thickness;
    /*
     * The only bit of redisplaying that we really have to do is draw
     * the three sides of the shadow that the XmTabBox does not draw.
     */

    switch( XmTabStack_tab_side(tab) )
    {
    case XmTABS_ON_TOP:
    default:
	top_rects[0].x = x;
	top_rects[0].y = y;
	top_rects[0].width = shadow;
	top_rects[0].height = height;
	num_top_rects = 1;

	bottom_rects[0].x = x;
	bottom_rects[0].y = y + height - shadow;
	bottom_rects[0].width = width;
	bottom_rects[0].height = shadow;

	bottom_rects[1].x = x + width - shadow;
	bottom_rects[1].y = y;
	bottom_rects[1].width = shadow;
	bottom_rects[1].height = height;
	num_bottom_rects = 2;

	bevel.x = x;
	bevel.y = y + height - shadow;
	break;
    case XmTABS_ON_BOTTOM:
	top_rects[0].x = x;
	top_rects[0].y = y;
	top_rects[0].width = shadow;
	top_rects[0].height = height;

	top_rects[1].x = x;
	top_rects[1].y = y;
	top_rects[1].width = width;
	top_rects[1].height = shadow;
	num_top_rects = 2;

	bottom_rects[0].x = x + width - shadow;
	bottom_rects[0].y = y;
	bottom_rects[0].width = shadow;
	bottom_rects[0].height = height;
	num_bottom_rects = 1;

	bevel.x = x + width - shadow;
	bevel.y = y;
	break;
    case XmTABS_ON_LEFT:
	top_rects[0].x = x;
	top_rects[0].y = y;
	top_rects[0].width = width;
	top_rects[0].height = shadow;
	num_top_rects = 1;

	bottom_rects[0].x = x;
	bottom_rects[0].y = y + height - shadow;
	bottom_rects[0].width = width;
	bottom_rects[0].height = shadow;

	bottom_rects[1].x = x + width - shadow;
	bottom_rects[1].y = y;
	bottom_rects[1].width = shadow;
	bottom_rects[1].height = height;
	num_bottom_rects = 2;

	bevel.x = x + width - shadow;
	bevel.y = y;
	break;
    case XmTABS_ON_RIGHT:
	top_rects[0].x = x;
	top_rects[0].y = y;
	top_rects[0].width = shadow;
	top_rects[0].height = height;

	top_rects[1].x = x;
	top_rects[1].y = y;
	top_rects[1].width = width;
	top_rects[1].height = shadow;
	num_top_rects = 2;

	bottom_rects[0].x = x;
	bottom_rects[0].y = y + height - shadow;
	bottom_rects[0].width = width;
	bottom_rects[0].height = shadow;
	num_bottom_rects = 1;

	bevel.x = x;
	bevel.y = y + height - shadow;
	break;
    }

    XFillRectangles(XtDisplay(tab), XtWindow(tab), top_GC, top_rects,
		    num_top_rects);
    XFillRectangles(XtDisplay(tab), XtWindow(tab), bottom_GC, bottom_rects,
		    num_bottom_rects);
    XmDrawBevel(XtDisplay(tab), XtWindow(tab), top_GC, bottom_GC,
		bevel.x, bevel.y, shadow, XmBEVEL_BOTH);
}

static void
PickSizes(XmTabStackWidget tab, int tab_width, int tab_height, XRectangle *box,
	  XRectangle *kid)
{
    int              max, width, height, row_width, offset = 0,
	             num_rows, num_cols;
    XtWidgetGeometry ask, got;
    Boolean          stacked = False;

    /*
     * The first thing that we need to do is find out how much space we
     * will have for our children.  This is the space that is not
     * taken up by our XmTabBox Child.  We give first priority to the
     * XmTabBoxChild.
     */
    switch( XmTabStack_tab_side(tab) )
    {
    case XmTABS_ON_TOP:
    case XmTABS_ON_BOTTOM:
    default:
	/*
	 * For this case the amount of width is fixed so we want to know,
	 * given a fixed width what height are you interested in.
	 */
	ask.request_mode = CWWidth | CWHeight;
	ask.width = tab_width;
	ask.height = 0;
	XtQueryGeometry(XmTabStack_tab_box(tab), &ask, &got);

	/*
	 * Now that we have a response back from the kid we need to make
	 * sure that the height it wants is not larger than we can
	 * accomidate.  If it is then we will scale it down.
	 */
	max = tab_height - 2 * BBPart(tab).margin_height -
	    tab->manager.shadow_thickness;

	if( (int)got.height > max ) got.height = max;

	width = tab_width - 2 * ((int)BBPart(tab).margin_width+
				 (int)tab->manager.shadow_thickness);
	height = max - (int)got.height;

	AssignMax(width, 1);
	AssignMax(height, 1);
	break;
    case XmTABS_ON_RIGHT:
    case XmTABS_ON_LEFT:
	/*
	 * For this case the amount of height is fixed so we want to know,
	 * given a fixed height what width are you interested in.
	 */
	ask.request_mode = CWWidth | CWHeight;
	ask.width = 0;
	ask.height = tab_height;
	XtQueryGeometry(XmTabStack_tab_box(tab), &ask, &got);

	/*
	 * Now that we have a response back from the kid we need to make
	 * sure that the width it wants is not larger than we can
	 * accomidate.  If it is then we will scale it down.
	 */
	max = tab_width - 2 * BBPart(tab).margin_width -
	    tab->manager.shadow_thickness;

	if( (int)got.width > max ) got.height = max;

	height = tab_height - 2 *
	    ((int)BBPart(tab).margin_height +
	     (int)tab->manager.shadow_thickness);
	width = max - (int)got.width;

	AssignMax(width, 1);
	AssignMax(height, 1);
	break;
    }

    if( XmTabStack_stacked_effect(tab) &&
        (XmTabStack_tab_mode(tab) == XmTABS_STACKED ||
	 XmTabStack_tab_mode(tab) == XmTABS_STACKED_STATIC) )
    {
	Dimension doffset;

	stacked = True;
	XtVaGetValues(XmTabStack_tab_box(tab),
		      XmNtabOffset, &doffset,
		      NULL);
	offset = doffset;
    }

    /*
     * Now that we know the size of the XmTabBox we can now layout it and
     * our other children.  We will start out with the XmTabBox.
     */
    switch( XmTabStack_tab_side(tab) )
    {
    case XmTABS_ON_TOP:
    default:
	box->x = 0;
	box->y = 0;
	box->width = tab_width;
	box->height = got.height;
	kid->x =
	    BBPart(tab).margin_width + tab->manager.shadow_thickness;
	kid->y = got.height + BBPart(tab).margin_height;
	break;
    case XmTABS_ON_BOTTOM:
	box->x = 0;
	box->y = tab_height - (int)got.height;
	box->width = tab_width;
	box->height = got.height;
	kid->x =
	    BBPart(tab).margin_width + tab->manager.shadow_thickness;
	kid->y =
	    BBPart(tab).margin_height + tab->manager.shadow_thickness;
	break;
    case XmTABS_ON_LEFT:
	box->x = 0;
	box->y = 0;
	box->width = got.width;
	box->height = tab_height;
	kid->x = got.width + BBPart(tab).margin_height;
	kid->y =
	    BBPart(tab).margin_height + tab->manager.shadow_thickness;
	break;
    case XmTABS_ON_RIGHT:
	box->x = tab_width - (int)got.width;
	box->y = 0;
	box->width = got.width;
	box->height = tab_height;
	kid->x =
	    BBPart(tab).margin_width + tab->manager.shadow_thickness;
	kid->y =
	    BBPart(tab).margin_height + tab->manager.shadow_thickness;
	break;
    }

    /*
     * If we are doing a stacked layout we need to change the size of
     * our children to make room for some special effects. "Hey, Special
     * Effects ..."
     */
    if( stacked )
    {
	/*
	 * What we need to do is make the size of our child, the size of
	 * a row of tabs minus the shadows.
	 */
	switch( XmTabStack_tab_side(tab) )
	{
	case XmTABS_ON_TOP:
	case XmTABS_ON_BOTTOM:
	default:
	    _XmTabBoxGetNumRowsColumns(XmTabStack_tab_box(tab), tab_width,
				       &num_rows, &num_cols);

	    if( num_rows <= 1 || num_cols < 1 )
	    {
		width = tab_width - 2 * (BBPart(tab).margin_width +
					 tab->manager.shadow_thickness);
	    }
	    else
	    {
	        int tmp;
		if (LayoutIsRtoL(tab))
		    kid->x += offset * (num_rows-1);
		tmp = (((int)tab_width) - ((num_rows-1) * offset))/
		    num_cols;

		row_width = num_cols * tmp;
		width = row_width - 2 * (BBPart(tab).margin_width +
					 tab->manager.shadow_thickness);
	    }
	    break;
	case XmTABS_ON_LEFT:
	case XmTABS_ON_RIGHT:
	    _XmTabBoxGetNumRowsColumns(XmTabStack_tab_box(tab), tab_height,
				       &num_rows, &num_cols);

	    if( num_rows <= 1 || num_cols < 1 )
	    {
		height = tab_height -
		    2 * (BBPart(tab).margin_height +
			 tab->manager.shadow_thickness);
	    }
	    else
	    {
		int tmp = (((int)tab_height) - ((num_rows-1) * offset))/
		    num_cols;

		row_width = num_cols * tmp;
		height = row_width - 2 * (BBPart(tab).margin_height +
					  tab->manager.shadow_thickness);
	    }
	    break;
	}
    }

    kid->width = width;
    kid->height = height;
}

static void Layout(XmTabStackWidget tab)
{
    XRectangle box, child;
    WidgetList kid;
    int        i, neg_x;

    /*
     * Lets start by finding out what size everything should be.
     */
    PickSizes(tab, XtWidth(tab), XtHeight(tab), &box, &child);

    /*
     * Now that we know the sizes lets start by placing the tab box.
     */
    if( LocalTabBox(tab) )
    {
	_XmConfigureWidget(XmTabStack_tab_box(tab),
			   box.x, box.y, box.width, box.height, 0);
    }

    /*
     * ... And now place the kiddies.
     */
    for( i = 0, kid = tab->composite.children; i < tab->composite.num_children;
	 ++i, ++kid )
    {
	if( *kid == XmTabStack_tab_box(tab) || !XtIsManaged(*kid) ) continue;

	if( XmTabStack__active_child(tab) == *kid )
	{
	    _XmConfigureWidget(*kid, child.x, child.y,
			       child.width, child.height,
			       (*kid)->core.border_width);
	}
	else
	{
	    neg_x = -((int)child.width) - 2 *
		(int)((*kid)->core.border_width);

	    _XmConfigureWidget(*kid, (Position)neg_x, child.y, child.width,
			       child.height, (*kid)->core.border_width);
	}
    }
}

/*
 * Function:
 *	TabSelectedCallback(widget, client, cbdata)
 * Description:
 *	This callback is activated when the user selects a tab.  This
 *	callback brings the active child to the front and hides the
 *	old active child.
 * Input:
 *	widget : Widget    - the XmTabBox Widget
 *	client : XtPointer - unused
 *	cbdata : XtPointer - the callback data from the XmTabBox
 * Output:
 *	None.
 */
static void
TabSelectedCallback(Widget widget, XtPointer client, XtPointer cbdata)
{
    XmTabBoxCallbackStruct   *info = (XmTabBoxCallbackStruct*) cbdata;
    XmTabStackWidget         tab = (XmTabStackWidget) XtParent(widget);
    XmTabStackCallbackStruct data;
    WidgetList               kid = tab->composite.children;
    Widget                   old, active = NULL;
    Cardinal                 i, idx = 0;
    int                      neg_x, x, y, width, height;

    /*
     * It seems that a tab has been selected so we need to swap our
     * active widgets.  Lets start by grabbing a pointer to our soon
     * to be active widget.  To do this we have to walk our child list
     * and find our info->tab_index managed widget.
     */
    for( i = 0; i < tab->composite.num_children; ++i, ++kid )
    {
	if( !XtIsManaged(*kid) || IsTabBox(tab, *kid) ) continue;

	if( idx == info->tab_index )
	{
	    active = *kid;
	    break;
	}
	idx++;
    }

    /*
     * Lets do a quick check here to make sure we found something.  If not
     * something is probably seriously wrong so we will just bail.
     */
    if( active == NULL )
    {
	return;
    }
    old = XmTabStack__active_child(tab);

    /*
     * We know that our new active widget is going to be placed at the
     * same location and of the same dimensions as our currently active
     * child so lets more the new active child to those coords.
     */
    _XmConfigureWidget(active, old->core.x, old->core.y,
		       XtWidth(old), XtHeight(old), active->core.border_width);

    neg_x = -((int)XtWidth(old)) - 2 * (int)((*kid)->core.border_width);
    _XmMoveWidget(old, (Position)neg_x, old->core.y);

    /*
     * Now lets set the active child.
     */
    XmTabStack__active_child(tab) = active;
    x = active->core.x - BBPart(tab).margin_width;
    y = active->core.y - BBPart(tab).margin_height;
    width = XtWidth(active) + (2 * BBPart(tab).margin_width);
    height = XtHeight(active) + (2 * BBPart(tab).margin_height);

    /*
     * Now if the active child has a background specified, or if the old
     * active child had one, then we need to redraw our background.
     */
    if( XiBackgroundSpecified(old) ||
        XiBackgroundSpecified(active) ||
        XiSelectSpecified(tab) )
    {
	if( XiSelectSpecified(tab) )
	{
	    SetSelectGC(tab, XmTabStack__gc(tab));
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), XmTabStack__gc(tab),
			   x, y, width, height);
	}
	else if( XiBackgroundSpecified(active) )
	{
	    SetChildGC(active, XmTabStack__gc(tab));
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), XmTabStack__gc(tab),
			   x, y, width, height);
	}
	else
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab),
			   tab->manager.background_GC,
			   x, y, width, height);
	}
    }

    /*
     * Now, if we are stacked, lets check to see if are old child
     * and our newly selected child are on the same row, and if
     * not, and we are not a perfect square (all rows and columns filled)
     * then we need to redraw our stacked shadows.
     */
    if( ((XmTabStack_tab_mode(tab) == XmTABS_STACKED ||
	  XmTabStack_tab_mode(tab) == XmTABS_STACKED_STATIC) &&
          XmTabStack_stacked_effect(tab)) &&
        (info->old_index >= 0 && info->tab_index >= 0 &&
	 XmTabBoxGetTabRow(XmTabStack_tab_box(tab), info->old_index) !=
	 XmTabBoxGetTabRow(XmTabStack_tab_box(tab), info->tab_index)) )
    {
	DrawStackedShadows(tab, tab->manager.top_shadow_GC,
			   tab->manager.bottom_shadow_GC,
			   0, 0, XtWidth(active) +
			   (2 * BBPart(tab).margin_width),
			   XtHeight(active) +
			   (2 * BBPart(tab).margin_height));
    }

    /*
     * And finally lets call the select callback for the XmTabStack
     * so that the user knows something has changed.
     */
    if (XmTabStack_do_notify(tab))
    {
	data.reason = XmCR_TAB_SELECTED;
	data.event = info->event;
	data.selected_child = active;

	XtCallCallbackList((Widget)tab, XmTabStack_tab_select_callback(tab), (XtPointer)&data);
    }
}

/*
 * XmRCallProc routine for checking font_list before setting it to NULL
 * If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */

static void
CheckSetRenderTable(Widget wid,
		    int offs,
		    XrmValue *value)
{
  XmTabStackWidget lw = (XmTabStackWidget)wid;

  /* Check if been here before */
  if (lw->tab_stack.check_set_render_table)
      value->addr = NULL;
  else {
      lw->tab_stack.check_set_render_table = True;
      value->addr = (char*)&(lw->tab_stack.font_list);
  }

}

#ifdef TEAR_OFF_TABS
static Widget IndexToTab(XmTabStackWidget tab, int idx)
{
    WidgetList kid = tab->composite.children;
    int i;

    if( tab == NULL || idx < 0 ) return( (Widget)NULL );

    for( i = 0; kid != NULL; kid++ )
    {
	if( !IsValidChild(tab, *kid) ) continue;
	if( i == idx )
	{
	    return( *kid );
	}
	++i;
    }
    return( (Widget)NULL );
}
#endif /* TEAR_OFF_TABS */

static int
TabToIndex(XmTabStackWidget tab, Widget child)
{
    WidgetList kid;
    int i;

    if( tab == NULL || child == NULL ) return( -1 );

    kid = tab->composite.children;

    for( i = 0; kid != NULL; kid++ )
    {
	if( !IsValidChild(tab, *kid) ) continue;
	if( *kid == child ) return( i );
	++i;
    }
    return( -1 );
}

Widget
XmCreateTabStack(Widget parent, String name, ArgList arg_list,
		 Cardinal arg_cnt)
{
    return( XtCreateWidget(name, xmTabStackWidgetClass, parent, arg_list,
			   arg_cnt) );
}

Widget
XmVaCreateTabStack(
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
                         xmTabStackWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedTabStack(
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
                         xmTabStackWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

void XmTabStackSelectTab(Widget widget, Boolean notify)
{
    XmTabStackWidget tab = (XmTabStackWidget) XtParent(widget);

    _XmWidgetToAppContext(widget);
    _XmAppLock(app);

    if (!XmIsTabStack((Widget)tab))
      {
	_XmAppUnlock(app);
	return;
      }

    if (!XtIsRealized((Widget)tab))
    {
	/* wherehouse the selected widget id to use later in realize proc */
	XmTabStack__selected_tab(tab) = widget;
	XmTabStack__selected_notify(tab) = notify;
    }
    else
    {
	/*
	 * _XmTabBoxSelectTab will call our internal callback routine,
	 * TabSelectedCallback() which in turn calls the XmNtabSelectedCallback
	 * callbacks if the "do_notify" flag is set...  The do_notify flag is
	 * normally TRUE, but can be switched off through this interface.
	 */
	XmTabStack_do_notify(tab) = notify;

	_XmTabBoxSelectTab(XmTabStack_tab_box(tab), TabToIndex(tab, widget));

	XmTabStack_do_notify(tab) = True;
    }

    _XmAppUnlock(app);
}

static void
DrawStackedShadows(XmTabStackWidget tab, GC top_GC, GC bottom_GC, int x, int y,
		   int base_width, int base_height)
{
    Widget     parent = XtParent(tab);
    int        i, num_rows, num_cols, offset, shadow, y2, x2, idx;
    Dimension  doffset;
    GC         gc;
    XRectangle rts[2];
    Pixel      pixel = XmUNSPECIFIED_PIXEL;
    Pixmap     pixmap = None;

    shadow = tab->manager.shadow_thickness;
    num_rows = XmTabBoxGetNumRows(XmTabStack_tab_box(tab));
    num_cols = XmTabBoxGetNumColumns(XmTabStack_tab_box(tab));
    XtVaGetValues(XmTabStack_tab_box(tab),
		  XmNtabOffset, &doffset,
		  NULL);
    offset = doffset;

    if( !XmIsManager(parent) )
    {
	XtVaGetValues(parent,
		      XmNbackground, &pixel,
		      XmNbackgroundPixmap, &pixmap,
		      NULL);
    }

    switch( XmTabStack_tab_side(tab) )
    {
    case XmTABS_ON_TOP:
    default:
	if (LayoutIsRtoLM(tab)) {
	    x = 0;
	    y = XmTabStack_tab_box(tab)->core.height;

	    XFillRectangle(XtDisplay(tab), XtWindow(tab), bottom_GC,
		       x + shadow + offset * (num_rows - 1), y + base_height,
		       base_width + shadow, shadow);

	    x += offset * (num_rows - 1);
	    x2 = x + offset;
	    y2 = y + base_height;

	    for( i = 0; i < num_rows; ++i )
	    {
	    	if (i == 0)
		{
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.top_shadow_GC,
			       x, y, shadow, y2 - y);
		    XmDrawBevel(XtDisplay(tab), XtWindow(tab), top_GC, bottom_GC,
				   x, y2, shadow, XmBEVEL_BOTH);
		    y2 -= offset;
		    x -= offset;
		    x2 -= offset;
		}
		else
		{
		    if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
					  (num_cols-1))) >= 0 )
		    {
			Widget child = XmTabStackIndexToWidget((Widget)tab, idx);

			if( child == NULL ||
				!XiBackgroundSpecified(child) )
			{
			    gc = tab->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    SetChildGC(child, gc);
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);

			if( XmIsManager(parent) )
			{
			    gc = ((XmManagerWidget)parent)->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    if( ValidPixmap(pixmap) )
			    {
				SetTiledGC(XtDisplay(tab), gc, pixmap);
			    }
		    	    else
			    {
				SetSolidGC(XtDisplay(tab), gc, pixel);
			    }
		        }
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y2, x2 - x, (int)XtHeight(tab) - y2);

			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.top_shadow_GC,
				   x, y, shadow, y2 - y);
			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x, y2 - shadow, x2 - x, shadow);
			XmDrawBevel(XtDisplay(tab), XtWindow(tab), top_GC, bottom_GC,
				   x, y2 - shadow, shadow, XmBEVEL_BOTH);
			x2 -= offset;
		    }
		    x -= offset;
		    y2 -= offset;
		}
	    } /* for */
	    if( XmIsManager(parent) )
	    {
		gc = ((XmManagerWidget)parent)->manager.background_GC;
	    }
	    else
	    {
		gc = XmTabStack__gc(tab);
		if( ValidPixmap(pixmap) )
		{
		    SetTiledGC(XtDisplay(tab), gc, pixmap);
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, pixel);
		}
	    }
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			offset * (num_rows-1) + base_width + 2*shadow, 0,
			XtWidth(tab) - offset * (num_rows-1) + base_width + 2*shadow, XtHeight(tab));
	    if( _XiGetTabIndex(XmTabStack_tab_box(tab), num_rows-1,
		    num_cols-1) < 0 )
	    {
	        XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			0, 0, offset, XtHeight(tab));
	    }
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), bottom_GC,
		       offset * (num_rows - 1) + base_width + shadow, y,
		       shadow, base_height + shadow);
	} else {
	    x = 0;
	    y = XmTabStack_tab_box(tab)->core.height;

	    XFillRectangle(XtDisplay(tab), XtWindow(tab), top_GC,
		       x, y, shadow, base_height + shadow);
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), bottom_GC,
		       x + shadow, y + base_height, base_width + shadow,
		       shadow);
	    XmDrawBevel(XtDisplay(tab), XtWindow(tab), top_GC, bottom_GC,
		    x, y + base_height, shadow, XmBEVEL_BOTTOM);

	    x += base_width + shadow;
	    x2 = x + offset;
	    y2 = y + base_height;
	    for( i = 0; i < num_rows; ++i )
	    {
		if( i == 0 )
		{
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.bottom_shadow_GC,
			       x, y, shadow, y2 - y);
		    x += shadow;
		    x2 = x + offset;
		}
		else
		{
		    if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
					  (num_cols-1))) >= 0 )
		    {
			Widget child = XmTabStackIndexToWidget((Widget)tab, idx);

			if( child == NULL || !XiBackgroundSpecified(child) )
			{
			    gc = tab->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    SetChildGC(child, gc);
			}
		        XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);

			if( XmIsManager(parent) )
			{
			    gc = ((XmManagerWidget)parent)->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    if( ValidPixmap(pixmap) )
			    {
				SetTiledGC(XtDisplay(tab), gc, pixmap);
			    }
			    else
			    {
				SetSolidGC(XtDisplay(tab), gc, pixel);
			    }
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y2, x2 - x, (int)XtHeight(tab) - y2);

			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x2 - shadow, y, shadow, y2 - y);
			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x, y2 - shadow, x2 - x, shadow);
			x = x2;
		    }
		    x2 += offset;
		}
	        y2 -= offset;
	    }


	    if( XmIsManager(parent) )
	    {
		gc = ((XmManagerWidget)parent)->manager.background_GC;
	    }
	    else
	    {
		gc = XmTabStack__gc(tab);
		if( ValidPixmap(pixmap) )
		{
		    SetTiledGC(XtDisplay(tab), gc, pixmap);
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, pixel);
		}
	    }
	    if( x < (int)XtWidth(tab) )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   x, 0, (int)XtWidth(tab) - x, XtHeight(tab));
	    }
	}
	break;
    case XmTABS_ON_BOTTOM:
	if( LayoutIsRtoL(tab) )
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), bottom_GC,
		    offset * (num_rows - 1) + base_width + shadow, 0, shadow, XtHeight(tab));

	    XFillRectangle(XtDisplay(tab), XtWindow(tab), top_GC,
		    offset * (num_rows - 1), 0,
		    base_width + shadow, shadow);

	    x = offset * (num_rows - 1);
	    y = 0;
	    x2 = x + offset;
	    y2 = y + offset;

	    for( i = 0; i < num_rows; ++i )
	    {
	    	if (i == 0)
		{
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.top_shadow_GC,
			       x, y + shadow, shadow, base_height);
		    x -= offset;
		    x2 -= offset;
		}
		else
		{
		    if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
			(num_cols-1))) >= 0 )
		    {
			Widget child = XmTabStackIndexToWidget((Widget)tab, idx);

			if( child == NULL || !XiBackgroundSpecified(child) )
			{
			    gc = tab->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    SetChildGC(child, gc);
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y2 + shadow, x2 - x, base_height - y2);

			if( XmIsManager(parent) )
			{
			    gc = ((XmManagerWidget)parent)->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    if( ValidPixmap(pixmap) )
			    {
				SetTiledGC(XtDisplay(tab), gc, pixmap);
			    }
			    else
			    {
				SetSolidGC(XtDisplay(tab), gc, pixel);
			    }
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);
			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.top_shadow_GC,
				   x, y2 + shadow, shadow, base_height - y2);
			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.top_shadow_GC,
				   x, y2, x2 - x, shadow);

			x2 -= offset;
		    }
		    x -= offset;
		    y2 += offset;
		}
	    }

	    if( XmIsManager(parent) )
	    {
		gc = ((XmManagerWidget)parent)->manager.background_GC;
	    }
	    else
	    {
		gc = XmTabStack__gc(tab);
		if( ValidPixmap(pixmap) )
		{
		    SetTiledGC(XtDisplay(tab), gc, pixmap);
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, pixel);
		}
	    }
	    i = offset * (num_rows-1) + base_width + shadow*2;
	    if( i < (int)XtWidth(tab) )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   i, 0, (int)XtWidth(tab) - i, XtHeight(tab));
	    }
	    if( _XiGetTabIndex(XmTabStack_tab_box(tab), num_rows-1,
		    num_cols-1) < 0 )
	    {
	        XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			0, 0, offset, XtHeight(tab));
	    }
	}
	else
	{
	    rts[0].x = 0;
	    rts[0].y = 0;
	    rts[0].width = shadow;
	    rts[0].height = XtHeight(tab);

	    rts[1].x = shadow;
	    rts[1].y = 0;
	    rts[1].width = base_width + shadow;
	    rts[1].height = shadow;
	    XFillRectangles(XtDisplay(tab), XtWindow(tab), top_GC, rts, 2);
	    x += base_width + shadow;

	    XmDrawBevel(XtDisplay(tab), XtWindow(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    x, 0, shadow, XmBEVEL_BOTTOM);

	    x2 = x + offset;
	    y += shadow;
	    y2 = y + base_height;
	    for( i = 0; i < num_rows; ++i )
	    {
		if( i == 0 )
		{
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.bottom_shadow_GC,
			       x, y, shadow, y2 - y);

		    x += shadow;
		    x2 = x + offset;
	        }
		else
		{
		    if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
					  (num_cols-1))) >= 0 )
		    {
			Widget child =
				XmTabStackIndexToWidget((Widget)tab, idx);

			if( child == NULL || !XiBackgroundSpecified(child) )
			{
			    gc = tab->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    SetChildGC(child, gc);
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);


			if( XmIsManager(parent) )
			{
			    gc = ((XmManagerWidget)parent)->manager.background_GC;
			}
			else
			{
			    gc = XmTabStack__gc(tab);
			    if( ValidPixmap(pixmap) )
			    {
				SetTiledGC(XtDisplay(tab), gc, pixmap);
			    }
			    else
			    {
				SetSolidGC(XtDisplay(tab), gc, pixel);
			    }
			}
			XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, 0, x2 - x, y);

			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x2 - shadow, y, shadow, y2 - y);
			XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.top_shadow_GC,
				   x, y, x2 - x, shadow);
			XmDrawBevel(XtDisplay(tab), XtWindow(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				x2 - shadow, y, shadow, XmBEVEL_BOTTOM);
			x = x2;
		    }
		    x2 += offset;
		}
		y += offset;
	    }
	    if( XmIsManager(parent) )
	    {
		gc = ((XmManagerWidget)parent)->manager.background_GC;
	    }
	    else
	    {
		gc = XmTabStack__gc(tab);
		if( ValidPixmap(pixmap) )
		{
		    SetTiledGC(XtDisplay(tab), gc, pixmap);
		}
		else
		{
		    SetSolidGC(XtDisplay(tab), gc, pixel);
		}
	    }
	    if( x < (int)XtWidth(tab) )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   x, 0, (int)XtWidth(tab) - x, XtHeight(tab));
	    }
	}
	break;
    case XmTABS_ON_RIGHT:
	rts[0].x = 0;
	rts[0].y = 0;
	rts[0].width = XtWidth(tab);
	rts[0].height = shadow;

	rts[1].x = 0;
	rts[1].y = shadow;
	rts[1].width = shadow;
	rts[1].height = base_width + shadow;
	XFillRectangles(XtDisplay(tab), XtWindow(tab), top_GC, rts, 2);
	y += base_height + shadow;

	XmDrawBevel(XtDisplay(tab), XtWindow(tab),
		    tab->manager.top_shadow_GC,
		    tab->manager.bottom_shadow_GC,
		    0, y, shadow, XmBEVEL_BOTTOM);
	y2 = y + offset;
	x += shadow;
	x2 = y + base_width;
	for( i = 0; i < num_rows; ++i )
	{
	    if( i == 0 )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.bottom_shadow_GC,
			       x, y, x2 - x, shadow);
		y += shadow;
		y2 = y + offset;
	    }
	    else
	    {
		if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
					  (num_cols-1))) >= 0 )
		{
		    Widget child = XmTabStackIndexToWidget((Widget)tab, idx);

		    if( child == NULL || !XiBackgroundSpecified(child) )
		    {
			gc = tab->manager.background_GC;
		    }
		    else
		    {
			gc = XmTabStack__gc(tab);
			SetChildGC(child, gc);
		    }
		    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);

		    if( XmIsManager(parent) )
		    {
			gc = ((XmManagerWidget)parent)->manager.background_GC;
		    }
		    else
		    {
			gc = XmTabStack__gc(tab);
			if( ValidPixmap(pixmap) )
			{
			    SetTiledGC(XtDisplay(tab), gc, pixmap);
			}
			else
			{
			    SetSolidGC(XtDisplay(tab), gc, pixel);
			}
		    }

		    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   0, y, x, y2 - y);
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x, y2 - shadow, x2 - x, shadow);
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.top_shadow_GC,
				   x, y, shadow, y2 - y);
		    XmDrawBevel(XtDisplay(tab), XtWindow(tab),
				tab->manager.top_shadow_GC,
				tab->manager.bottom_shadow_GC,
				x, y2 - shadow, shadow, XmBEVEL_BOTTOM);
		    y = y2;
		}
		y2 += offset;
	    }
	    x += offset;
	}
	if( XmIsManager(parent) )
	{
	    gc = ((XmManagerWidget)parent)->manager.background_GC;
	}
	else
	{
	    gc = XmTabStack__gc(tab);
	    if( ValidPixmap(pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, pixel);
	    }
	}
	if( x < (int)XtWidth(tab) )
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   0, y, XtWidth(tab), (int)XtHeight(tab) - y);
	}
	break;
    case XmTABS_ON_LEFT:
	y = 0;
	x = XtWidth(XmTabStack_tab_box(tab));

	XFillRectangle(XtDisplay(tab), XtWindow(tab), top_GC,
		       x, y, base_width + shadow, shadow);
	XFillRectangle(XtDisplay(tab), XtWindow(tab), bottom_GC,
		       x + base_width, y + shadow, shadow,
		       base_height + shadow);
	XmDrawBevel(XtDisplay(tab), XtWindow(tab), top_GC, bottom_GC,
		    x + base_width, y, shadow, XmBEVEL_BOTTOM);

	y += base_height + shadow;
	y2 = y + offset;
	x2 = x + base_width;
	for( i = 0; i < num_rows; ++i )
	{
	    if( i == 0 )
	    {
		XFillRectangle(XtDisplay(tab), XtWindow(tab),
			       tab->manager.bottom_shadow_GC,
			       x, y, x2 - x, shadow);
		y += shadow;
		y2 = y + offset;
	    }
	    else
	    {
		if( (idx = _XiGetTabIndex(XmTabStack_tab_box(tab), i,
					  (num_cols-1))) >= 0 )
		{
		    Widget child = XmTabStackIndexToWidget((Widget)tab, idx);

		    if( child == NULL || !XiBackgroundSpecified(child) )
		    {
			gc = tab->manager.background_GC;
		    }
		    else
		    {
			gc = XmTabStack__gc(tab);
			SetChildGC(child, gc);
		    }
		    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x, y, x2 - x, y2 - y);


		    if( XmIsManager(parent) )
		    {
			gc = ((XmManagerWidget)parent)->manager.background_GC;
		    }
		    else
		    {
			gc = XmTabStack__gc(tab);
			if( ValidPixmap(pixmap) )
			{
			    SetTiledGC(XtDisplay(tab), gc, pixmap);
			}
			else
			{
			    SetSolidGC(XtDisplay(tab), gc, pixel);
			}
		    }
		    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
				   x2, y, (int)XtWidth(tab) - x2, y2 - y);
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x, y2 - shadow, x2 - x, shadow);
		    XFillRectangle(XtDisplay(tab), XtWindow(tab),
				   tab->manager.bottom_shadow_GC,
				   x2 - shadow, y, shadow, y2 - y);
		    y = y2;
		}
		y2 += offset;
	    }
	    x2 -= offset;
	}
	if( XmIsManager(parent) )
	{
	    gc = ((XmManagerWidget)parent)->manager.background_GC;
	}
	else
	{
	    gc = XmTabStack__gc(tab);
	    if( ValidPixmap(pixmap) )
	    {
		SetTiledGC(XtDisplay(tab), gc, pixmap);
	    }
	    else
	    {
		SetSolidGC(XtDisplay(tab), gc, pixel);
	    }
	}
	if( x < (int)XtWidth(tab) )
	{
	    XFillRectangle(XtDisplay(tab), XtWindow(tab), gc,
			   0, y, XtWidth(tab), (int)XtHeight(tab) - y);
	}
	break;
    }
}


/* note: intentionally undocumented. It may be here for historical reasons
** so keep it around; but the function isn't useful. It's used internally
** and was probably intended not to be publicized.
*/
Widget XmTabStackIndexToWidget(Widget widget, int idx)
{
    XmTabStackWidget tab = (XmTabStackWidget) widget;
    Cardinal         i, cnt;
    WidgetList       kid;

    _XmWidgetToAppContext(widget);
    _XmAppLock(app);

    if( !XmIsTabStack(widget) || idx < 0 )
    {
	_XmAppUnlock(app);
	return( NULL );
    }

    for( i = 0, cnt = 0, kid = tab->composite.children;
	 i < tab->composite.num_children; ++i, ++kid )
    {
	if( IsTabBox(tab, *kid) || !XtIsManaged(*kid) ) continue;
	if( cnt++ == idx )
	  {
	    _XmAppUnlock(app);
	    return( *kid );
	  }
    }
    _XmAppUnlock(app);
    return( NULL );
}

Widget XmTabStackGetSelectedTab(Widget widget)
{
    XmTabStackWidget tab = (XmTabStackWidget) widget;

    _XmWidgetToAppContext(widget);
    _XmAppLock(app);

    if (!XmIsTabStack(widget))
    {
	_XmAppUnlock(app);
	return(NULL);
    }

    if (!XtIsRealized((Widget)tab) && XmTabStack__selected_tab(tab))
      {
	_XmAppUnlock(app);
	return (XmTabStack__selected_tab(tab));
      }

    _XmAppUnlock(app);
    return (XmTabStack__active_child(tab));
}

#ifdef TEAR_OFF_TABS

static void
TearOffCallback(Widget widget, XtPointer client, XtPointer cbdata)
{
    XtPointer data;

    XtVaGetValues(widget, XmNuserData, &data, NULL);
    if( data == NULL ) return;
}

static void
MenuSelectCallback(Widget widget, XtPointer client, XtPointer cbdata)
{
    XtPointer data;

    XtVaGetValues(widget,
		  XmNuserData, &data,
		  NULL);

    XmTabStackSelectTab((Widget)data);
}

static void
BuildMenu(XmTabStackWidget tab)
{
    WidgetList kids, buttons;
    Cardinal   kidCnt, validKids, i, j, buttonCnt, validButtons;
    Widget     menu = XmTabStack__menu(tab), tmp;
    Boolean    done;

    /*
     * First thing we need to do is get the list of child for the
     * menu.
     */
    XtVaGetValues(tab,
		  XmNchildren, &kids,
		  XmNnumChildren, &kidCnt,
		  NULL);

    for( i = 0, validKids = 0; i < kidCnt; ++i )
    {
	if( IsValidChild(tab, kids[i]) ) validKids++;
    }

    XtVaGetValues(menu,
		  XmNchildren, &buttons,
		  XmNnumChildren, &buttonCnt,
		  NULL);

    for( i = 0, validButtons = 0; i < buttonCnt; ++i )
    {
	if( XmIsPushButton(buttons[i]) ) validButtons++;
    }

    for( i = validButtons; i <= validKids; ++i )
    {
	tmp =
	    XtVaCreateWidget("menuItem", xmPushButtonWidgetClass, menu, NULL);
	XtAddCallback(tmp, XmNactivateCallback, MenuSelectCallback, NULL);
    }

    XtVaGetValues(menu,
		  XmNchildren, &buttons,
		  XmNnumChildren, &buttonCnt,
		  NULL);

    for( j = 0; j < buttonCnt; ++j )
    {
	if( !XmIsSeparator(buttons[j]) &&
	    buttons[j] != XmTabStack__tear_off_button(tab) ) break;
    }
    for( i = 0, done = False; i < kidCnt;  ++i)
    {
	if( !IsValidChild(tab, kids[i]) ) continue;

	XtVaSetValues(buttons[j],
		      XmNlabelString, XmTabStackC_tab_label_string(kids[i]),
		      XmNuserData, (XtPointer) kids[i],
		      NULL);

	XtManageChild(buttons[j]);
	for( j++; j < buttonCnt; ++j )
	{
	    if( !XmIsSeparator(buttons[j]) &&
	        buttons[j] != XmTabStack__tear_off_button(tab) ) break;
	}
    }

    for( ; j < buttonCnt; ++j )
    {
	if( XmIsSeparator(buttons[j]) ) continue;

	XtUnmanageChild(buttons[j]);
    }
}

static void
TabTransferProc(Widget widget, XtPointer closure, Atom *selType,
		Atom *type, XtPointer value, unsigned long *length,
		int format)
{
    Widget   child, tmp, tab = (Widget)closure;
    Colormap c_cmap, p_cmap;
    Cardinal c_depth, p_depth;

    if( *length != sizeof(Widget) ||
        *type != WidgetAtom(widget) )
    {
	XtVaSetValues(widget,
		      XmNtransferStatus, XmTRANSFER_FAILURE,
		      NULL);
	return;
    }

    child = *((Widget*)value);

    /*
     * Lets just do a quick validity check here. Also if the child is
     * already a child of the tab stack then lets fail.
     */
    if( child->core.self != child || tab->core.self != tab ||
        XtParent(child) == tab )
    {
	XtVaSetValues(widget,
		      XmNtransferStatus, XmTRANSFER_FAILURE,
		      NULL);
	return;
    }

    /*
     * Now lets check some specific values to make sure that the
     * transfer will not cause an X Error.
     */
    XtVaGetValues(child,
		  XmNcolormap, &c_cmap,
		  XmNdepth, &c_depth,
		  NULL);

    XtVaGetValues(tab,
		  XmNcolormap, &p_cmap,
		  XmNdepth, &p_depth,
		  NULL);

    if( c_cmap != p_cmap || c_depth != p_depth )
    {
	XtVaSetValues(widget,
		      XmNtransferStatus, XmTRANSFER_FAILURE,
		      NULL);
	return;
    }

    /*
     * The final safty check is to see if we are trying to move a parent
     * into a child.
     */
    for( tmp = tab; tmp != NULL && tmp != child; tmp = XtParent(tmp) );
    if( tmp == child )
    {
	XtVaSetValues(widget,
		      XmNtransferStatus, XmTRANSFER_FAILURE,
		      NULL);
	return;
    }

    /*
     * If we got here that means that we really want to move this
     * panel so lets do it.
     */
    XiMoveTabPanel(child, tab);
    XtVaSetValues(widget,
		  XmNtransferStatus, XmTRANSFER_SUCCESS,
		  NULL);
}

static void
HandleTabDrop(Widget widget, XtPointer client, XtPointer cbdata)
{
    XmDropProcCallback     drop = (XmDropProcCallback) cbdata;
    XmDropTransferEntryRec transferEntries[2];
    XmDropTransferEntry    transferList;
    Arg                    args[10];
    int                    n = 0;

    if( drop->dropAction != XmDROP ||
        drop->operation != XmDROP_MOVE )
    {
	XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); ++n;
    }
    else
    {
	transferEntries[0].target = TabAtom(widget);
	transferEntries[0].client_data = (XtPointer) widget;
	transferList = transferEntries;
	XtSetArg(args[n], XmNdropTransfers, transferList); ++n;
	XtSetArg(args[n], XmNnumDropTransfers, 1); ++n;
	XtSetArg(args[n], XmNtransferProc, TabTransferProc); ++n;
	XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_SUCCESS); ++n;
    }
    XmDropTransferStart(drop->dragContext, args, n);
}

static Boolean
TabConvertProc(Widget widget, Atom *selection, Atom *target, Atom *typeRtn,
	       XtPointer *valueRtn, unsigned long *lengthRtn, int *formatRtn,
	       unsigned long *max_lengthRtn, XtPointer client_data,
	       XtRequestId request_id)
{
    Widget    child, *passed_child;
    XtPointer child_data;

    XtVaGetValues(widget,
		  XmNclientData, &child_data,
		  NULL);
    child = (Widget) child_data;

    if( *target != TabAtom(widget) ) return( False );

    passed_child = XtNew(Widget);
    *passed_child = child;

    *typeRtn = WidgetAtom(widget);
    *valueRtn = (XtPointer) passed_child;
    *lengthRtn = sizeof(Widget);
    *formatRtn = 8;

    return( True );
}

static Atom TabAtom(Widget widget)
{
    char pid_buf[128];

    sprintf(pid_buf, "ICS_TAB_PID_%d\n", (int)getpid());
    return XInternAtom(XtDisplay(widget), pid_buf, False);
}

Widget XmTabStackXYToWidget(Widget widget, int x, int y)
{
    Widget tab_stack, tab_box;

    _XmWidgetToAppContext(widget);
    _XmAppLock(app);

    if( XmIsTabStack(widget) )
    {
	tab_stack = widget;
	tab_box = XmTabStack_tab_box((XmTabStackWidget)widget);

    }
    else if( XmIsTabBox(widget) )
    {
	tab_stack = XtParent(widget);
	tab_box = widget;
    }
    else if( XtParent(widget) != NULL && XmIsTabBox(XtParent(widget)) )
    {

	tab_box = XtParent(widget);
	tab_stack = XtParent(tab_box);
    }
    else
    {
      _XmAppUnlock(app);
      return( NULL );
    }

    _XmAppUnlock(app);
    return( XmTabStackIndexToWidget(tab_stack,
				    XmTabBoxXYToIndex(tab_box, x, y)) );
}

#define XiDeleteChild(c) \
{ \
     _XmProcessLock(); \
     (*((CompositeWidgetClass)((c)->core.parent->core.widget_class))-> \
      composite_class.delete_child)(c); \
    _XmProcessUnlock(); \
}


#define XiInsertChild(p,c) \
{ \
     (c)->core.parent = (p); \
     _XmProcessLock(); \
     (*((CompositeWidgetClass)(p)->core.widget_class)-> \
      composite_class.insert_child)(c); \
     _XmProcessUnlock(); \
}


static void XiMoveTabPanel(Widget panel, Widget to)
{
    Widget from = XtParent(panel);
    int    width, height;

    /*
     * The object here is to move the panel from "from" to "to", so lets
     * start by removing the panel from "from".  This is done by first
     * unmanaging the panel and then calling "from"s delete method.
     */
    XmDropSiteStartUpdate(from);
    XmDropSiteStartUpdate(to);
    width = XmTabStackC_width(panel);
    height = XmTabStackC_height(panel);
    XtUnmanageChild(panel);
    XtVaSetValues(panel,
		  XmNwidth, width,
		  XmNheight, height,
		  NULL);
    XiDeleteChild(panel);
    XtUnrealizeWidget(panel);

    /*
     * And now lets add that child to "to" by calling its insert method
     * and then managing the puppy.
     */
    XiInsertChild(to, panel);
    XtManageChild(panel);
    XmDropSiteEndUpdate(from);
    XmDropSiteEndUpdate(to);
}

static void
DragCallback(Widget widget, XtPointer client, XtPointer cbdata)
{
    XmAnyCallbackStruct            *info = (XmAnyCallbackStruct*)cbdata;
    XmTabStackWidget               tab = (XmTabStackWidget) client;

    switch( info->reason )
    {
    case XmCR_DRAG_DROP_FINISH:
	break;
    case XmCR_DRAG_MOTION:
	break;
    case XmCR_DROP_FINISH:
	break;
    case XmCR_DROP_SITE_ENTER:
        {
	    XmDropSiteEnterCallback enter = (XmDropSiteEnterCallback)cbdata;

	    switch( enter->dropSiteStatus )
	    {
	    case XmINVALID_DROP_SITE:
	    default:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, XmTabStack__invalid_icon(tab),
			      NULL);
		break;
	    case XmVALID_DROP_SITE:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, NULL,
			      NULL);
		break;
	    }
	}
	break;
    case XmCR_DROP_SITE_LEAVE:
	XtVaSetValues(widget,
		      XmNstateCursorIcon, XmTabStack__invalid_icon(tab),
		      NULL);
	break;
    case XmCR_DROP_START:
        {
	    XmDropSiteEnterCallback enter = (XmDropSiteEnterCallback)cbdata;

	    switch( enter->dropSiteStatus )
	    {
	    case XmINVALID_DROP_SITE:
	    default:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, XmTabStack__invalid_icon(tab),
			      NULL);
		break;
	    case XmVALID_DROP_SITE:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, NULL,
			      NULL);
		break;
	    }
	}
	break;
    case XmCR_OPERATION_CHANGED:
        {
	    XmOperationChangedCallback change =
		(XmOperationChangedCallback) cbdata;

	    switch( change->operation )
	    {
	    case XmDROP_MOVE:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, NULL,
			      NULL);
		break;
	    default:
		XtVaSetValues(widget,
			      XmNstateCursorIcon, XmTabStack__invalid_icon(tab),
			      NULL);
		break;
	    }
	}
	break;
    case XmCR_TOP_LEVEL_ENTER:
	break;
    case XmCR_TOP_LEVEL_LEAVE:
	break;
    default:
	break;
    }
}

static void
XmTabStackMenu(Widget widget, XEvent *event, String *params,
	       Cardinal *num_params)
{
    Widget           parent = XtParent(widget), child;
    XmTabStackWidget tab = (XmTabStackWidget)XtParent(parent);

    /*
     * Lets start by finding out if we are popping this menu up from
     * a tab.
     */
    child = XmTabStackXYToWidget((Widget) tab, event->xbutton.x,
				 event->xbutton.y);

    /*
     * If we are not popping this menu up, and we are not in a tab
     * then we want to desensitise the tear off option.
     */
    XtSetSensitive(XmTabStack__tear_off_button(tab), (child != NULL));
    XtVaSetValues(XmTabStack__tear_off_button(tab),
		  XmNuserData, child, NULL);

    XmMenuPosition(XmTabStack__menu(tab), (XButtonPressedEvent*)event);
    XtManageChild(XmTabStack__menu(tab));
}

static void
XmTabBoxDragTab(Widget widget, XEvent *event, String *params,
		Cardinal *num_params)
{
    Widget           parent = XtParent(widget), drag, child, source_icon,
                     invalid_icon;
    XmTabStackWidget tab = (XmTabStackWidget)XtParent(parent);
    Arg              args[10];
    Cardinal         n = 0;
    Atom             target;
    int              idx;
    Pixmap           source_pixmap, source_mask, invalid_pixmap, invalid_mask;

#ifdef TEAR_OFF_TABS
    if( !XmTabStack_allow_tear_offs(tab) ||
        event == NULL || event->xany.type != ButtonPress ||
        (child = IndexToTab(tab,
			    XmTabBoxGetIndex(parent, event->xbutton.x,
					     event->xbutton.y))) == NULL ||
        !XmTabStackC_tear_off_enabled(child) )
    {
	return;
    }
#endif /* TEAR_OFF_TABS */

    if( XmTabStack__source_icon(tab) == NULL )
    {
	source_pixmap = XCreateBitmapFromData(XtDisplay(widget),
					      XtWindow(widget),
					      (char*)tab_pix_bits,
					      tab_pix_width,
					      tab_pix_height);
	source_mask = XCreateBitmapFromData(XtDisplay(widget),
					    XtWindow(widget),
					    (char*)tab_mask_bits,
					    tab_pix_width,
					    tab_pix_height);
	invalid_pixmap = XCreateBitmapFromData(XtDisplay(widget),
					       XtWindow(widget),
					       (char*)invalid_bits,
					       tab_pix_width,
					       tab_pix_height);
	invalid_mask = XCreateBitmapFromData(XtDisplay(widget),
					     XtWindow(widget),
					     (char*)invalid_mask_bits,
					     tab_pix_width,
					     tab_pix_height);

	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNwidth, tab_pix_width); n++;
	XtSetArg(args[n], XmNheight, tab_pix_height); n++;
	XtSetArg(args[n], XmNpixmap, source_pixmap); n++;
	XtSetArg(args[n], XmNmask, source_mask); n++;
	source_icon = XmCreateDragIcon(parent, "tabSourceIcon", args, n); n=0;

	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNwidth, tab_pix_width); n++;
	XtSetArg(args[n], XmNheight, tab_pix_height); n++;
	XtSetArg(args[n], XmNpixmap, invalid_pixmap); n++;
	XtSetArg(args[n], XmNmask, invalid_mask); n++;
	invalid_icon = XmCreateDragIcon(parent, "tabInvalidIcon", args, n);n=0;

	XmTabStack__source_pixmap(tab) = source_pixmap;
	XmTabStack__source_mask(tab) = source_mask;
	XmTabStack__invalid_pixmap(tab) = invalid_pixmap;
	XmTabStack__invalid_mask(tab) = invalid_mask;
	XmTabStack__source_icon(tab) = source_icon;
	XmTabStack__invalid_icon(tab) = invalid_icon;
    }
    else
    {
	source_icon = XmTabStack__source_icon(tab);
	invalid_icon = XmTabStack__invalid_icon(tab);
    }

    target = TabAtom(widget);
    XtSetArg(args[n], XmNblendModel, XmBLEND_ALL); ++n;
    XtSetArg(args[n], XmNsourceCursorIcon, source_icon); ++n;
    XtSetArg(args[n], XmNsourcePixmapIcon, source_icon); ++n;
    XtSetArg(args[n], XmNdragOperations, XmDROP_MOVE); ++n;
    XtSetArg(args[n], XmNexportTargets, &target); ++n;
    XtSetArg(args[n], XmNnumExportTargets, 1); ++n;
    XtSetArg(args[n], XmNconvertProc, TabConvertProc); ++n;
    XtSetArg(args[n], XmNclientData, child); ++n;
    drag = XmDragStart(parent, event, args, n); n = 0;

    XtAddCallback(drag, XmNdragDropFinishCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNdragMotionCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNdropFinishCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNdropSiteEnterCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNdropSiteLeaveCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNdropStartCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNoperationChangedCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNtopLevelEnterCallback,
		  DragCallback, (XtPointer) tab);
    XtAddCallback(drag, XmNtopLevelLeaveCallback,
		  DragCallback, (XtPointer) tab);
}

#endif
