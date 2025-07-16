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
*/
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Draw.c /main/12 1995/10/25 20:02:15 cde-sun $"
#endif
#endif


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/DrawP.h>


/*********************************************************************
 *    Only goes in this module the Draw API used by everybody in Xm.
 *    Anything specific to a class should go in its own Dr*.c module.
 *********************************************************************/

/********    Static Function Declarations    ********/

static void DrawSimpleShadow(Display *display,
			     Drawable d,
			     GC top_gc,
			     GC bottom_gc,
			     Position x,
			     Position y,
			     Dimension width,
			     Dimension height,
			     Dimension shadow_thick,
			     Dimension cor);
/********    End Static Function Declarations    ********/


/*-------------------- Private functions ----------------------*/
/*-------------------------------------------------------------*/

static void
DrawSimpleShadow (Display *display,
		  Drawable d,
		  GC top_gc,
		  GC bottom_gc,
		  Position x,
		  Position y,
		  Dimension width,
		  Dimension height,
		  Dimension shadow_thick,
		  Dimension cor)
/* New implementation (1.2 vs 1.1) uses XSegments instead of XRectangles. */
/* Used for the simple shadow, the etched shadow and the separators */
/* Segment has been faster than Rectangles in all my benches, either
   on Hp, Sun or Pmax. Lines has been slower, that I don't understand... */
{
  static XSegment * segms = NULL;
  static int segm_count = 0;

  register int i, size2, size3;

  if (!d) return;
  ASSIGN_MIN(shadow_thick, (width >> 1));
  ASSIGN_MIN(shadow_thick, (height >> 1));
  if (shadow_thick <= 0) return;

  size2 = (shadow_thick << 1);
  size3 = size2 + shadow_thick;

  _XmProcessLock();
  if (segm_count < shadow_thick) {
    segms = (XSegment *) XtRealloc((char*)segms,
				   sizeof (XSegment) * (size2 << 1));
    segm_count = shadow_thick;
  }

  for (i = 0; i < shadow_thick; i++) {
    /*  Top segments  */
    segms[i].x1 = x;
    segms[i].y2 = segms[i].y1 = y + i;
    segms[i].x2 = x + width - i - 1;
    /*  Left segments  */
    segms[i + shadow_thick].x2 = segms[i + shadow_thick].x1 = x + i;
    segms[i + shadow_thick].y1 = y + shadow_thick;
    segms[i + shadow_thick].y2 = y + height - i - 1;

    /*  Bottom segments  */
    segms[i + size2].x1 = x + i + ((cor)?0:1) ;
    segms[i + size2].y2 = segms[i + size2].y1 = y + height - i - 1;
    segms[i + size2].x2 = x + width - 1 ;
    /*  Right segments  */
    segms[i + size3].x2 = segms[i + size3].x1 = x + width - i - 1;
    segms[i + size3].y1 = y + i + 1 - cor;
    segms[i + size3].y2 = y + height - 1 ;
  }

  XDrawSegments (display, d, top_gc, &segms[0], size2);
  XDrawSegments (display, d, bottom_gc, &segms[size2], size2);
  _XmProcessUnlock();
}

/**************************** Public functions *************************/
/***********************************************************************/

/****************************XmeDrawShadows****************************/
void XmeDrawShadows(Display *display, Drawable d,
                  GC top_gc, GC bottom_gc,
                                           Position x, Position y,
                  Dimension width, Dimension height, Dimension shad_thick,
                  unsigned int shad_type)
{
    GC  tmp_gc ;
    XtAppContext app;

    if(!d) return ;

    app = XtDisplayToApplicationContext(display);

    _XmAppLock(app);

    if ((shad_type == XmSHADOW_IN) || (shad_type == XmSHADOW_ETCHED_IN)) {
        tmp_gc = top_gc ;
        top_gc = bottom_gc ;  /* switch top and bottom shadows */
        bottom_gc = tmp_gc ;
    }

    if ((shad_type == XmSHADOW_ETCHED_IN ||
         shad_type == XmSHADOW_ETCHED_OUT) && (shad_thick != 1)) {
        DrawSimpleShadow (display, d, top_gc, bottom_gc, x, y,
                            width, height, shad_thick/2, 1);
        DrawSimpleShadow (display, d, bottom_gc, top_gc,
                            x + shad_thick/2, y + shad_thick/2,
                            width - (shad_thick/2)*2,
                            height - (shad_thick/2)*2, shad_thick/2, 1);
    } else
        DrawSimpleShadow (display, d, top_gc, bottom_gc, x, y,
                            width, height, shad_thick, 0);
    _XmAppUnlock(app);
}


/*****************************XmeClearBorder*********************************/
void XmeClearBorder (Display *display, Window w,
                                                 Position x, Position y,
                    Dimension width, Dimension height, Dimension shadow_thick)
{
    XtAppContext app;

    if (!w || !shadow_thick || !width || !height) return ;

    app = XtDisplayToApplicationContext(display);
    _XmAppLock(app);

    XClearArea (display, w, x, y, width, shadow_thick, FALSE);
    XClearArea (display, w, x, y + height - shadow_thick, width,
                shadow_thick, FALSE);
    XClearArea (display, w, x, y, shadow_thick, height, FALSE);
    XClearArea (display, w, x + width - shadow_thick, y, shadow_thick,
                height, FALSE);
    _XmAppUnlock(app);
}





/****************************XmeDrawHighlight*************************/
void XmeDrawHighlight(Display *display, Drawable d,
			    GC gc,
                            Position x, Position y,
                            Dimension width, Dimension height,
                            Dimension highlight_thickness)
{
    XRectangle rect[4] ;
    XtAppContext app;

    if (!d || !highlight_thickness || !width || !height) return ;

    app = XtDisplayToApplicationContext(display);

    _XmAppLock(app);

    rect[0].x = rect[1].x = rect[2].x = x ;
    rect[3].x = x + width - highlight_thickness ;
    rect[0].y = rect[2].y = rect[3].y = y ;
    rect[1].y = y + height - highlight_thickness ;
    rect[0].width = rect[1].width = width ;
    rect[2].width = rect[3].width = highlight_thickness ;
    rect[0].height = rect[1].height = highlight_thickness ;
    rect[2].height = rect[3].height = height ;

    XFillRectangles (display, d, gc, rect, 4);

    _XmAppUnlock(app);

}



/******************************XmeDrawSeparator**********************/
void XmeDrawSeparator(Display *display, Drawable d,
                     GC top_gc, GC bottom_gc, GC separator_gc,
                     Position x, Position y,
                     Dimension width, Dimension height,
                     Dimension shadow_thick,
                     Dimension margin, unsigned char orientation,
                     unsigned char separator_type)
{
   Position center;
   XSegment segs[2];
   GC   tmp_gc;
   int i, ndash, shadow_dash_size ;
   XtAppContext app;

   if (!d || (separator_type == XmNO_LINE)) return ;

   app = XtDisplayToApplicationContext(display);
   _XmAppLock(app);

   if (orientation == XmHORIZONTAL) {
       center = y + height / 2;
   } else {
       center = x + width / 2;
   }

   if (separator_type == XmSINGLE_LINE ||
       separator_type == XmSINGLE_DASHED_LINE) {
       if (orientation == XmHORIZONTAL) {
           segs[0].x1 = x + margin;
           segs[0].y1 = segs[0].y2 = center;
           segs[0].x2 = x + width - margin - 1;
       } else {
           segs[0].y1 = y + margin;
           segs[0].x1 = segs[0].x2 = center;
           segs[0].y2 = y + height - margin - 1;
       }
       XDrawSegments (display, d, separator_gc, segs, 1);
       _XmAppUnlock(app);
       return;
   }

   if (separator_type == XmDOUBLE_LINE ||
       separator_type == XmDOUBLE_DASHED_LINE) {
       if (orientation == XmHORIZONTAL) {
           segs[0].x1 = segs[1].x1 = x + margin;
           segs[0].x2 = segs[1].x2 = x + width - margin - 1;
           segs[0].y1 = segs[0].y2 = center - 1;
           segs[1].y1 = segs[1].y2 = center + 1;
       } else {
           segs[0].y1 = segs[1].y1 = y + margin;
           segs[0].y2 = segs[1].y2 = y + height - margin - 1;
           segs[0].x1 = segs[0].x2 = center - 1;
           segs[1].x1 = segs[1].x2 = center + 1;
       }
       XDrawSegments (display, d, separator_gc, segs, 2);
       _XmAppUnlock(app);
       return;
   }

   /* only shadowed stuff in the following, so shadowThickness has to be
      something real */
   if (!shadow_thick) { _XmAppUnlock(app); return ; }

   /* do the in/out effect now */
   if (separator_type == XmSHADOW_ETCHED_IN ||
       separator_type == XmSHADOW_ETCHED_IN_DASH) {
       tmp_gc = top_gc ;
       top_gc = bottom_gc ;
       bottom_gc = tmp_gc ;
   }

   /* In the following, we need to special case the shadow_thick = 2 or 3 case,
      since : it's the default and we don't like changes in visual here,
      and also it looks non symetrical the way it is without special code:
                                    ......
	                            .,,,,,

	and you really want to have ......
                                    ,,,,,,
      So we won't use DrawSimpleShadow in this case, painful but hey..
   */


   /* Now the regular shadowed cases, in one pass with one looong dash
      for the non dashed case */

   if (separator_type == XmSHADOW_ETCHED_IN_DASH ||
       separator_type == XmSHADOW_ETCHED_OUT_DASH)
   /* for now, shadowed dash use three time the shadow thickness as a
      dash size, and worried about the shadow_thick odd values as well */
       shadow_dash_size = (shadow_thick/2)*2*3 ;
   else

   /* regular case, only one dash, the length of the separator */
       shadow_dash_size = (orientation == XmHORIZONTAL)?
           (width - 2*margin):(height - 2*margin) ;

   /* special case for shadowThickness = 1 */
   if (shadow_dash_size == 0) shadow_dash_size = 5 ;

   /* ndash value will be 1 for the regular shadow case (not dashed) */
   if (orientation == XmHORIZONTAL) {
       ndash = ((width - 2*margin)/shadow_dash_size + 1)/2 ;
       for (i=0; i<ndash; i++)
           if (shadow_thick < 4) {
	       XDrawLine(display, d, top_gc,
			 x + margin + 2*i*shadow_dash_size,
			 center - shadow_thick/2,
			 x + margin + (2*i + 1)*shadow_dash_size -1,
			 center - shadow_thick/2);
	       if (shadow_thick > 1)
		   XDrawLine(display, d, bottom_gc,
			 x + margin + 2*i*shadow_dash_size,
			 center,
			 x + margin + (2*i + 1)*shadow_dash_size -1,
			 center);
	   } else {
	       DrawSimpleShadow(display, d, top_gc, bottom_gc,
				  x + margin + i*2*shadow_dash_size,
				  center - shadow_thick/2,
				  shadow_dash_size, (shadow_thick/2)*2,
				  shadow_thick/2, 0);
	   }
       /* draw the last dash, with possibly a different size */
       if (i*2*shadow_dash_size < (width - 2*margin))
       {
           if (shadow_thick < 4)
           {
	       XDrawLine(display, d, top_gc,
			 x + margin + 2*i*shadow_dash_size,
			 center - shadow_thick/2,
			 x + (width - 2*margin),
			 center - shadow_thick/2);
	       if (shadow_thick > 1)
               {
		   XDrawLine(display, d, bottom_gc,
			 x + margin + 2*i*shadow_dash_size,
			 center,
			 x + (width - 2*margin),
			 center);
               }
	   } else {
	       DrawSimpleShadow(display, d, top_gc, bottom_gc,
				  x + margin + i*2*shadow_dash_size,
				  center - shadow_thick/2,
				  (width - 2*margin) - i*2*shadow_dash_size,
				  (shadow_thick/2)*2,
				  shadow_thick/2, 0);
	   }
       }
   } else {
       ndash = ((height - 2*margin)/shadow_dash_size + 1)/2 ;
       for (i=0; i<ndash; i++)
           if (shadow_thick < 4) {
	       XDrawLine(display, d, top_gc,
			 center - shadow_thick/2,
			 y + margin + 2*i*shadow_dash_size,
			 center - shadow_thick/2,
			 y + margin + (2*i + 1)*shadow_dash_size -1);
	       if (shadow_thick > 1)
		   XDrawLine(display, d, bottom_gc,
			 center,
			 y + margin + 2*i*shadow_dash_size,
			 center,
			 y + margin + (2*i + 1)*shadow_dash_size -1);
	   } else {
	       DrawSimpleShadow(display, d, top_gc, bottom_gc,
				  center - shadow_thick/2,
				  y + margin + i*2*shadow_dash_size,
				  (shadow_thick/2)*2, shadow_dash_size,
				  shadow_thick/2, 0);
	   }
       if (i*2*shadow_dash_size < (height - 2*margin))
       {
           if (shadow_thick < 4)
           {
	       XDrawLine(display, d, top_gc,
			 center - shadow_thick/2,
			 y + margin + 2*i*shadow_dash_size,
			 center - shadow_thick/2,
			 y + (height - 2*margin));
	       if (shadow_thick > 1)
		   XDrawLine(display, d, bottom_gc,
			 center,
			 y + margin + 2*i*shadow_dash_size,
			 center,
			 y + (height - 2*margin));
	   } else {
	       DrawSimpleShadow(display, d, top_gc, bottom_gc,
				  center - shadow_thick/2,
				  y + margin + i*2*shadow_dash_size,
				  (shadow_thick/2)*2,
				  (height - 2*margin) - i*2*shadow_dash_size,
				  shadow_thick/2, 0);
           }
        }
   }
   _XmAppUnlock(app);
}

