/* $XConsortium: DrTog.c /main/6 1995/10/25 20:00:29 cde-sun $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"
#include <Xm/DrawP.h>

/**************************** DrTog module ***************************
 *
 * Draw API used by Toggle Button only.
 *
 ***************************************************************************/

/********    Static Function Declarations    ********/
static void DrawCheckMark(Display *display,
			  Drawable d,
			  GC gc,
			  Position x,
			  Position y,
			  Dimension width,
			  Dimension height,
			  Dimension margin);

static void DrawCross(Display *display,
		      Drawable d,
		      GC gc,
		      Position x,
		      Position y,
		      Dimension width,
		      Dimension height,
		      Dimension margin);

/********    End Static Function Declarations    ********/


#define CHECK_TEMPLATE_WIDTH	32
#define CHECK_TEMPLATE_HEIGHT	32

static const XPoint check_template[] = {
  {  0, 15 },
  {  6,  9 },
  { 14, 17 },
  { 31,  0 },
  { 31,  3 },
  { 21, 17 },
  { 16, 31 },
  {  0, 15 }
};

static void
DrawCheckMark(Display *display,
	      Drawable d,
	      GC gc,
	      Position x,
	      Position y,
	      Dimension width,
	      Dimension height,
	      Dimension margin)
{
  XGCValues old_values, new_values;
  XtGCMask mask;
  XPoint check[XtNumber(check_template)];
  float scale_x = (width - 2 * margin - 1) / (float)CHECK_TEMPLATE_WIDTH;
  float scale_y = (height - 2 * margin - 1) / (float)CHECK_TEMPLATE_HEIGHT;
  int npoints = XtNumber(check_template);
  int i;

  /* Scale and translate the glyph to the desired area. */
  for (i = 0; i < npoints; i++)
    {
      check[i].x = (Position)(check_template[i].x*scale_x + 0.5) + x + margin;
      check[i].y = (Position)(check_template[i].y*scale_y + 0.5) + y + margin;
    }

  /* CR 9656: Force line_width so test results are not platform-dependent. */
  mask = 0;
  new_values.line_width = 1, mask |= GCLineWidth;
  XGetGCValues(display, gc, mask, &old_values);
  XChangeGC(display, gc, mask, &new_values);

  /* Draw the check mark. */
  XFillPolygon(display, d, gc, check, npoints - 1, Nonconvex, CoordModeOrigin);
  XDrawLines(display, d, gc, check, npoints, CoordModeOrigin);

  XChangeGC(display, gc, mask, &old_values);
}

static void
DrawCross(Display *display,
	  Drawable d,
	  GC gc,
	  Position x,
	  Position y,
	  Dimension width,
	  Dimension height,
	  Dimension margin)
{
  Position left   = x + margin;
  Position right  = x + width - margin - 1;
  Position top    = y + margin;
  Position bottom = y + height - margin - 1;

  XSegment segs[6];
  Cardinal nsegs = 0;

  segs[nsegs].x1 = left;
  segs[nsegs].y1 = top + 1;
  segs[nsegs].x2 = right - 1;
  segs[nsegs].y2 = bottom;
  nsegs++;

  segs[nsegs].x1 = left;
  segs[nsegs].y1 = top;
  segs[nsegs].x2 = right;
  segs[nsegs].y2 = bottom;
  nsegs++;

  segs[nsegs].x1 = left + 1;
  segs[nsegs].y1 = top;
  segs[nsegs].x2 = right;
  segs[nsegs].y2 = bottom - 1;
  nsegs++;

  segs[nsegs].x1 = left;
  segs[nsegs].y1 = bottom - 1;
  segs[nsegs].x2 = right - 1;
  segs[nsegs].y2 = top;
  nsegs++;

  segs[nsegs].x1 = left;
  segs[nsegs].y1 = bottom;
  segs[nsegs].x2 = right;
  segs[nsegs].y2 = top;
  nsegs++;

  segs[nsegs].x1 = left + 1;
  segs[nsegs].y1 = bottom;
  segs[nsegs].x2 = right;
  segs[nsegs].y2 = top + 1;
  nsegs++;

  assert(nsegs <= XtNumber(segs));

  XDrawSegments(display, d, gc, segs, nsegs);
}


/***********************XmeDrawDiamond**********************************/
void XmeDrawDiamond(Display *display, Drawable d,
                    GC top_gc, GC bottom_gc, GC center_gc,
                    Position x, Position y,
                    Dimension width,
       		    Dimension height,
                    Dimension shadow_thick,
	            Dimension margin)
{
   XSegment seg[12];
   XPoint   pt[4];
   int midX, midY;
   int delta;
   _XmDisplayToAppContext(display);

   (void)height;
   if (!d || !width) return ;

   _XmAppLock(app);
   if (width % 2 == 0) width--;

   if (width == 1) {
       XDrawPoint(display, d, top_gc, x,y);
       _XmAppUnlock(app);
       return ;
   } else
   if (width == 3) {
       seg[0].x1 = x;
       seg[0].y1 = seg[0].y2 = y + 1;
       seg[0].x2 = x + 2;

       seg[1].x1 = seg[1].x2 = x + 1;
       seg[1].y1 = y ;
       seg[1].y2 = y + 2;
       XDrawSegments (display, d, top_gc, seg, 2);
       _XmAppUnlock(app);
       return ;
   } else   {        /* NORMAL SIZED ToggleButtonS : initial width >= 5 */
       midX = x + (width + 1) / 2;
       midY = y + (width + 1) / 2;
       /*  The top shadow segments  */
       seg[0].x1 = x;                   /*  1  */
       seg[0].y1 = midY - 1;
       seg[0].x2 = midX - 1;            /*  2  */
       seg[0].y2 = y;

       seg[1].x1 = x + 1;               /*  3  */
       seg[1].y1 = midY - 1;
       seg[1].x2 = midX - 1;            /*  4  */
       seg[1].y2 = y + 1;

       seg[2].x1 = x + 2;               /*  3  */
       seg[2].y1 = midY - 1;
       seg[2].x2 = midX - 1;            /*  4  */
       seg[2].y2 = y + 2;

       seg[3].x1 = midX - 1;            /*  5  */
       seg[3].y1 = y;
       seg[3].x2 = x + width - 1;       /*  6  */
       seg[3].y2 = midY - 1;

       seg[4].x1 = midX - 1;            /*  7  */
       seg[4].y1 = y + 1;
       seg[4].x2 = x + width - 2;       /*  8  */
       seg[4].y2 = midY - 1;

       seg[5].x1 = midX - 1;            /*  7  */
       seg[5].y1 = y + 2;
       seg[5].x2 = x + width - 3;       /*  8  */
       seg[5].y2 = midY - 1;

       /*  The bottom shadow segments  */
       seg[6].x1 = x;                   /*  9  */
       seg[6].y1 = midY - 1;
       seg[6].x2 = midX - 1;            /*  10  */
       seg[6].y2 = y + width - 1;

       seg[7].x1 = x + 1;               /*  11  */
       seg[7].y1 = midY - 1;
       seg[7].x2 = midX - 1;            /*  12  */
       seg[7].y2 = y + width - 2;

       seg[8].x1 = x + 2;               /*  11  */
       seg[8].y1 = midY - 1;
       seg[8].x2 = midX - 1;            /*  12  */
       seg[8].y2 = y + width - 3;

       seg[9].x1 = midX - 1;            /*  13  */
       seg[9].y1 = y + width - 1;
       seg[9].x2 = x + width - 1;       /*  14  */
       seg[9].y2 = midY - 1;

       seg[10].x1 = midX - 1;           /*  15  */
       seg[10].y1 = y + width - 2;
       seg[10].x2 = x + width - 2;      /*  16  */
       seg[10].y2 = midY - 1;

       seg[11].x1 = midX - 1;           /*  15  */
       seg[11].y1 = y + width - 3;
       seg[11].x2 = x + width - 3;      /*  16  */
       seg[11].y2 = midY - 1;
   }

   XDrawSegments (display, d, top_gc, &seg[3], 3);
   XDrawSegments (display, d, bottom_gc, &seg[6], 6);
   XDrawSegments (display, d, top_gc, &seg[0], 3);

   if (width == 5 || !center_gc) { _XmAppUnlock(app); return ; }   /* <= 5 in fact */

   if (shadow_thick == 0)
     delta = -3 ;
   else if (shadow_thick == 1)
     delta = -1 ;
   else
     delta = margin;

   pt[0].x = x + 3 + delta;
   pt[0].y = pt[2].y = midY - 1;
   pt[1].x = pt[3].x = midX - 1 ;
   pt[1].y = y + 2 + delta;
   pt[2].x = x + width - 3 - delta;
   pt[3].y = y + width - 3 - delta;

   XFillPolygon (display, d, center_gc, pt, 4, Convex, CoordModeOrigin);
   _XmAppUnlock(app);
}

/******************************XmeDrawIndicator**********************/
void
XmeDrawIndicator(Display *display,
		 Drawable d,
		 GC gc,
                 Position x, Position y,
                 Dimension width, Dimension height,
                 Dimension margin,
                 XtEnum type)
{
  _XmDisplayToAppContext(display);

  _XmAppLock(app);
  switch(type & 0xf0)
    {
    case XmINDICATOR_CHECK:
      DrawCheckMark(display, d, gc, x, y, width, height, margin);
      break;

    case XmINDICATOR_CROSS:
      DrawCross(display, d, gc, x, y, width, height, margin);
      break;
    }
  _XmAppUnlock(app);
}

void
XmeDrawCircle(Display *display,
	      Drawable d,
	      GC top_gc,
	      GC bottom_gc,
	      GC center_gc,
	      Position x,
	      Position y,
	      Dimension width,
	      Dimension height,
	      Dimension shadow_thick,
	      Dimension margin)
{
  int line_width = MIN(shadow_thick, MIN(width, height) / 2);
  _XmDisplayToAppContext(display);

  if ((width <= 0) || (height <= 0))
    return;

  _XmAppLock(app);
  if (shadow_thick > 0)
    {
      /* Force the GCs to use our values. */
      XGCValues top_values, bottom_values, new_values;
      XtGCMask mask;

      mask = 0;
      new_values.line_width = line_width, mask |= GCLineWidth;

      XGetGCValues(display, top_gc, mask, &top_values);
      XGetGCValues(display, bottom_gc, mask, &bottom_values);

      XChangeGC(display, top_gc, mask, &new_values);
      XChangeGC(display, bottom_gc, mask, &new_values);

      if (center_gc != NULL) {
    	  int delta = MIN(line_width + margin, MIN(width, height) / 2) -1;
    	  XFillArc(display, d, center_gc,
    			  x + delta, y + delta,
    			  MAX(width - 2 * delta, 1),
    			  MAX(height - 2 * delta, 1),
    			  0, 360 * 64);
      }

      XDrawArc(display, d, top_gc,
	       x + line_width/2, y + line_width/2,
	       MAX(width - line_width, 1),
	       MAX(height - line_width, 1),
	       45 * 64, 180 * 64);
      XDrawArc(display, d, bottom_gc,
	       x + line_width/2, y + line_width/2,
	       MAX(width - line_width, 1),
	       MAX(height - line_width, 1),
	       45 * 64, -180 * 64);

      XChangeGC(display, top_gc, mask, &top_values);
      XChangeGC(display, bottom_gc, mask, &bottom_values);
    } else {
	  if (center_gc != NULL) {
		  int delta = MIN(line_width + margin, MIN(width, height) / 2);
		  XFillArc(display, d, center_gc,
				  x + delta, y + delta,
				  MAX(width - 2 * delta, 1),
				  MAX(height - 2 * delta, 1),
				  0, 360 * 64);
	  }
  }
  _XmAppUnlock(app);
}

