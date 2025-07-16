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

#ifndef _XmRegionI_h
#define _XmRegionI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISEMPTY(r) ((r)->numRects == 0)

/*  1 if two Boxs overlap.
 *  0 if two Boxs do not overlap.
 *  Remember, x2 and y2 are not in the region
 */
#define EXTENTCHECK(r1, r2) \
	((r1)->x2 > (r2)->x1 && \
	 (r1)->x1 < (r2)->x2 && \
	 (r1)->y2 > (r2)->y1 && \
	 (r1)->y1 < (r2)->y2)

/*
 *  update region extents
 */
#define EXTENTS(r,idRect){\
            if((r)->x1 < (idRect)->extents.x1)\
              (idRect)->extents.x1 = (r)->x1;\
            if((r)->y1 < (idRect)->extents.y1)\
              (idRect)->extents.y1 = (r)->y1;\
            if((r)->x2 > (idRect)->extents.x2)\
              (idRect)->extents.x2 = (r)->x2;\
            if((r)->y2 > (idRect)->extents.y2)\
              (idRect)->extents.y2 = (r)->y2;\
        }

/*
 *   Check to see if there is enough memory in the present region.
 */
#define MEMCHECK(reg, rect, firstrect){\
        if ((reg)->numRects >= ((reg)->size - 1)){\
          (firstrect) = (XmRegionBox *) XtRealloc \
          ((char *)(firstrect), \
	  (unsigned) (2*(sizeof(XmRegionBox))*((reg)->size)));\
          if ((firstrect) == 0)\
            return;\
          (reg)->size *= 2;\
          (rect) = &(firstrect)[(reg)->numRects];\
         }\
       }

/*  this routine checks to see if the previous rectangle is the same
 *  or subsumes the new rectangle to add.
 */

#define CHECK_PREVIOUS(Reg, R, Rx1, Ry1, Rx2, Ry2)\
               (!(((Reg)->numRects > 0)&&\
                  ((R-1)->y1 == (Ry1)) &&\
                  ((R-1)->y2 == (Ry2)) &&\
                  ((R-1)->x1 <= (Rx1)) &&\
                  ((R-1)->x2 >= (Rx2))))

/*  add a rectangle to the given XmRegion */
#define ADDRECT(reg, r, fr, rx1, ry1, rx2, ry2){\
    if (((rx1) < (rx2)) && ((ry1) < (ry2)) && \
        CHECK_PREVIOUS((reg), (r), (rx1), (ry1), (rx2), (ry2))){\
              if ((reg)->numRects == (reg)->size){\
                 if (!(reg)->size) (reg)->size = 1;\
                 else (reg)->size += (reg)->numRects;\
                 (reg)->rects = (XmRegionBox *) realloc((reg)->rects,\
                                                        sizeof(XmRegionBox) *\
                                                        (reg)->size);\
                 fr = REGION_BOXPTR(reg);\
                 r = fr + (reg)->numRects;\
              }\
              (r)->x1 = (rx1);\
              (r)->y1 = (ry1);\
              (r)->x2 = (rx2);\
              (r)->y2 = (ry2);\
              EXTENTS((r), (reg));\
              (reg)->numRects++;\
              (r)++;\
            }\
        }



/*  add a rectangle to the given XmRegion */
#define ADDRECTNOX(reg, r, rx1, ry1, rx2, ry2){\
            if ((rx1 < rx2) && (ry1 < ry2) &&\
                CHECK_PREVIOUS((reg), (r), (rx1), (ry1), (rx2), (ry2))){\
              (r)->x1 = (rx1);\
              (r)->y1 = (ry1);\
              (r)->x2 = (rx2);\
              (r)->y2 = (ry2);\
              (reg)->numRects++;\
              (r)++;\
            }\
        }

#define INBOX(r, x, y) \
      ( ( ((r).x2 >= x)) && \
        ( ((r).x1 <= x)) && \
        ( ((r).y2 >= y)) && \
        ( ((r).y1 <= y)) )

/*
 * used by _XmRegionDrawShadow
 */

#define TL_OPEN		(1 << 0)
#define BL_OPEN		(1 << 1)
#define TR_OPEN		(1 << 2)
#define BR_OPEN		(1 << 3)
#define TL_MATCH	(1 << 4)
#define BL_MATCH	(1 << 5)
#define TR_MATCH	(1 << 6)
#define BR_MATCH	(1 << 7)

/*
 * The following macro were ported from the X server include file regionstr.h
 */
#define REGION_BOXPTR(reg) ((XmRegionBox *)((reg)->rects))


/********    Private Function Declarations for Region.c    ********/

extern XmRegion _XmRegionCreate( void ) ;
extern XmRegion _XmRegionCreateSize(
			long size) ;
extern void _XmRegionComputeExtents(
			XmRegion r) ;
extern void _XmRegionGetExtents(
                        XmRegion r,
                        XRectangle *rect) ;
extern void _XmRegionUnionRectWithRegion(
                        XRectangle *rect,
                        XmRegion source,
                        XmRegion dest) ;
extern void _XmRegionIntersectRectWithRegion(
                        XRectangle *rect,
                        XmRegion source,
                        XmRegion dest) ;
extern long _XmRegionGetNumRectangles(
			XmRegion r) ;
extern void _XmRegionGetRectangles(
                        XmRegion r,
                        XRectangle **rects,
                        long *nrects) ;
extern void _XmRegionSetGCRegion(
                        Display *dpy,
                        GC gc,
			int x_origin,
			int y_origin,
                        XmRegion r) ;
extern void _XmRegionDestroy(
                        XmRegion r) ;
extern void _XmRegionOffset(
                        XmRegion pRegion,
                        int x,
                        int y) ;
extern void _XmRegionIntersect(
                        XmRegion reg1,
                        XmRegion reg2,
                        XmRegion newReg) ;
extern void _XmRegionUnion(
                        XmRegion reg1,
                        XmRegion reg2,
                        XmRegion newReg) ;
extern void _XmRegionSubtract(
                        XmRegion regM,
                        XmRegion regS,
                        XmRegion regD) ;
extern Boolean _XmRegionIsEmpty(
                        XmRegion r) ;
extern Boolean _XmRegionEqual(
                        XmRegion r1,
                        XmRegion r2) ;
extern Boolean _XmRegionPointInRegion(
                        XmRegion pRegion,
                        int x,
                        int y) ;
extern void _XmRegionClear(
			XmRegion r ) ;
extern void _XmRegionShrink(
			XmRegion r,
			int dx,
                        int dy) ;
extern void _XmRegionDrawShadow(
			Display	*display,
			Drawable d,
			GC top_gc,
			GC bottom_gc,
			XmRegion region,
			Dimension border_thick,
			Dimension shadow_thick,
			unsigned int shadow_type ) ;

extern XmRegion _XmRegionFromImage(
			XImage *image );

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmRegionI_h */
