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
#ifndef _XmDrawP_h
#define _XmDrawP_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------*/
/*   Functions used by Xm widgets for the Motif visual drawing   */
/*---------------------------------------------------------------*/
/* All these functions have an Xlib draw like API:
      a Display*, a Drawable, then GCs, Positions and Dimensions
      and finally some specific paramaters */

/******** The Draw.c file has been split in several module for
          a better link profile *********/

/*---------------------------------------------------------------
  XmeDrawShadows,
       use in place of the 1.1 _XmDrawShadow and _XmDrawShadowType
       with changes to the interface (widget vs window, offsets, new order)
       and in the implementation (uses XSegments instead of XRectangles).
       Both etched and regular shadows use now a single private routine
       xmDrawSimpleShadow.
    XmeDrawHighlight.
       Implementation using FillRectangles, for solid highlight only.
    _XmDrawHighlight.
       Highlight using wide lines, so that dash mode works.
    XmeClearBorder,
       new name for _XmEraseShadow  (_XmClearShadowType, which clear half a
       shadow with a 'widget' API stays in Manager.c )
       XmClearBorder is only usable on window, not on drawable.
    XmeDrawSeparator,
       use in place of the duplicate redisplay method of both separator and
       separatorgadget (highlight_thickness not used, must be incorporated
       in the function call parameters). use xmDrawSimpleShadow.
       Has 2 new separator types for dash shadowed lines.
    XmeDrawDiamond,
       new interface for _XmDrawDiamondButton (_XmDrawSquareButton is
       really a simple draw shadow and will be in the widget file as is).
    XmeDrawArrow,
       same algorithm as before but in one function that re-uses the malloced
       rects and does not store anything in the wigdet instance.
    XmeDrawPolygonShadow,
       new one that use the RegionDrawShadow API to implement an Xme call
    XmeDrawCircle,
       new one for toggle visual
    XmeDrawIndicator
       new one for toggle drawing
---------------------------------------------------------------------------*/


/********    Private Function Declarations    ********/

extern void XmeDrawShadows(
                        Display *display,
                        Drawable d,
                        GC top_gc,
                        GC bottom_gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shad_thick,
                        unsigned int shad_type);
extern void XmeClearBorder(
                        Display *display,
                        Window w,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shadow_thick);
extern void XmeDrawSeparator(
                        Display *display,
                        Drawable d,
                        GC top_gc,
                        GC bottom_gc,
                        GC separator_gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shadow_thick,
                        Dimension margin,
                        unsigned char orientation,
                        unsigned char separator_type);
extern void XmeDrawDiamond(
                        Display *display,
                        Drawable d,
                        GC top_gc,
                        GC bottom_gc,
                        GC center_gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shadow_thick,
                        Dimension margin);

extern void XmeDrawCircle(
                        Display *display,
                        Drawable d,
                        GC top_gc,
                        GC bottom_gc,
                        GC center_gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shadow_thick,
                        Dimension margin);

extern void XmeDrawHighlight(
                        Display *display,
                        Drawable d,
                        GC gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension highlight_thick
                        );
extern void XmeDrawArrow(
                        Display *display,
                        Drawable d,
                        GC top_gc,
                        GC bot_gc,
                        GC cent_gc,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension shadow_thick,
                        unsigned char direction);

extern void XmeDrawPolygonShadow(
		      Display *dpy,
		      Drawable d,
		      GC topGC,
		      GC bottomGC,
		      XPoint *points,
		      int n_points,
		      Dimension shadowThickness,
		      unsigned char shadowType);

extern void XmeDrawIndicator(Display *display,
		 Drawable d,
		 GC gc,
                 Position x, Position y,
                 Dimension width, Dimension height,
		 Dimension margin,
                 XtEnum type);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmDrawP_h */
