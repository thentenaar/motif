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
static char rcsid[] = "$XConsortium: Region.c /main/10 1995/07/13 17:46:45 drk $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*
 * The functions in this file implement the XmRegion abstraction
 * An XmRegion is simply an area, as the name
 * implies, and is implemented as a "y-x-banded" array of rectangles. To
 * explain: Each XmRegion is made up of a certain number of rectangles sorted
 * by y coordinate first, and then by x coordinate.
 *
 * Furthermore, the rectangles are banded such that every rectangle with a
 * given upper-left y coordinate (y1) will have the same lower-right y
 * coordinate (y2) and vice versa. If a rectangle has scanlines in a band, it
 * will span the entire vertical distance of the band. This means that some
 * areas that could be merged into a taller rectangle will be represented as
 * several shorter rectangles to account for shorter rectangles to its left
 * or right but within its "vertical scope".
 *
 * An added constraint on the rectangles is that they must cover as much
 * horizontal area as possible. E.g. no two rectangles in a band are allowed
 * to touch.
 *
 * Whenever possible, bands will be merged together to cover a greater vertical
 * distance (and thus reduce the number of rectangles). Two bands can be merged
 * only if the bottom of one touches the top of the other and they have
 * rectangles in the same places (of the same width, of course). This maintains
 * the y-x-banding that's so nice to have...
 */

#include <Xm/XmosP.h>  /* for memmove */
#include "XmI.h"
#include "RegionI.h"
#include "MessagesI.h"

#define MESSAGE1	_XmMMsgRegion_0000

typedef void (*XmOverlapProc)( XmRegion, XmRegionBox *, XmRegionBox *,
			       XmRegionBox *, XmRegionBox *,
                               short, short) ;
typedef void (*XmNonOverlapProc)( XmRegion, XmRegionBox *, XmRegionBox *,
                               short, short) ;


/********    Static Function Declarations    ********/

static void miSetExtents(
                        XmRegion pReg) ;
static void Compress(
                        XmRegion r,
                        XmRegion s,
                        XmRegion t,
                        unsigned dx,
                        int xdir,
                        int grow) ;
static void miIntersectO(
                        register XmRegion pReg,
                        register XmRegionBox *r1,
                        XmRegionBox *r1End,
                        register XmRegionBox *r2,
                        XmRegionBox *r2End,
                        short y1,
                        short y2) ;
static void miRegionCopy(
                        register XmRegion dstrgn,
                        register XmRegion rgn) ;
static long miCoalesce(
                        register XmRegion pReg,
                        long prevStart,
                        long curStart) ;
static void miRegionOp(
                        register XmRegion newReg,
                        XmRegion reg1,
                        XmRegion reg2,
                        XmOverlapProc overlapFunc,
                        XmNonOverlapProc nonOverlap1Func,
                        XmNonOverlapProc nonOverlap2Func) ;
static void miUnionNonO(
                        register XmRegion pReg,
                        register XmRegionBox *r,
                        XmRegionBox *rEnd,
                        register short y1,
                        register short y2) ;
static void miUnionO(
                        register XmRegion pReg,
                        register XmRegionBox *r1,
                        XmRegionBox *r1End,
                        register XmRegionBox *r2,
                        XmRegionBox *r2End,
                        register short y1,
                        register short y2) ;
static void miSubtractNonO1(
                        register XmRegion pReg,
                        register XmRegionBox *r,
                        XmRegionBox *rEnd,
                        register short y1,
                        register short y2) ;
static void miSubtractO(
                        register XmRegion pReg,
                        register XmRegionBox *r1,
                        XmRegionBox *r1End,
                        register XmRegionBox *r2,
                        XmRegionBox *r2End,
                        register short y1,
                        register short y2) ;
static void CreateLeftShadow(
                        XmRegionBox *here,
                        unsigned long mask,
                        XSegment **segml,
                        int *segmc,
                        int *segmi) ;
static void CreateRightShadow(
                        XmRegionBox *here,
                        unsigned long mask,
                        XSegment **segml,
                        int *segmc,
                        int *segmi) ;
static void CreateTopShadow(
                        Position start_x,
                        Position end_x,
                        XmRegionBox *here,
                        unsigned long mask,
                        XSegment **segml,
                        int *segmc,
                        int *segmi) ;
static void CreateBottomShadow(
                        Position start_x,
                        Position end_x,
                        XmRegionBox *here,
                        unsigned long mask,
                        XSegment **segml,
                        int *segmc,
                        int *segmi) ;
static void ShrinkRegion(
			XmRegion r,
			XmRegion s,
			XmRegion t,
			int dx,
			int dy) ;

/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *  _XmRegionCreateSize()
 *
 *  Creates a new, empty, XmRegion of specified size.
 ***********************************************************************/

XmRegion
_XmRegionCreateSize(
    long	size )
{
    XmRegion temp;

    if (!(temp = ( XmRegion )XtMalloc(sizeof (XmRegionRec))))
	return (XmRegion) NULL;
    if (!(temp->rects = (XmRegionBox *)
	XtMalloc((Cardinal) (sizeof(XmRegionBox)*size)))) {
	XtFree((char *) temp);
	return (XmRegion) NULL;
    }
    temp->numRects = 0;
    temp->extents.x1 = 0;
    temp->extents.y1 = 0;
    temp->extents.x2 = 0;
    temp->extents.y2 = 0;
    temp->size = size;
    return( temp );
}

/************************************************************************
 *
 *  _XmRegionCreate()
 *
 *  Creates a new, empty, XmRegion.
 ***********************************************************************/

XmRegion
_XmRegionCreate( void )
{
    return (_XmRegionCreateSize(1L));
}

/************************************************************************
 *
 *  _XmRegionDestroy ()
 *
 *  Frees the memory associated with an XmRegion.
 ***********************************************************************/

void
_XmRegionDestroy(
    XmRegion	r )
{
    XtFree( (char *) r->rects );
    XtFree( (char *) r );
}

/************************************************************************
 *
 *  _XmRegionClear ()
 *
 *  Empties an XmRegion.
 ***********************************************************************/

void
_XmRegionClear(
    XmRegion	r )
{
    r->numRects = 0;
    r->extents.x1 = 0;
    r->extents.y1 = 0;
    r->extents.x2 = 0;
    r->extents.y2 = 0;
}

/************************************************************************
 *
 *  _XmRegionIsEmpty ()
 *
 *  Check to see if an XmRegion is empty.
 ***********************************************************************/

Boolean
_XmRegionIsEmpty(
    XmRegion	r )
{
    return ISEMPTY(r);
}

/************************************************************************
 *
 *  _XmRegionEqual ()
 *
 *  Check to see if two XmRegions are equal.
 ***********************************************************************/

Boolean
_XmRegionEqual(
    register XmRegion	r1,
    register XmRegion	r2 )
{
    register int i;

    if (r1->numRects != r2->numRects) {
	return( False );
    }
    else if (ISEMPTY(r1)) {
	return( True );
    }

    /*
     *  R1 and r2 have the same nonzero number of rectangles.
     *  Check the extents.
     */

    else if (r1->extents.x1 != r2->extents.x1 ||
             r1->extents.x2 != r2->extents.x2 ||
             r1->extents.y1 != r2->extents.y1 ||
             r1->extents.y2 != r2->extents.y2) {
	return( False );
    }

    /*
     *  The extents match.  Check each rectangle.
     */

    else {
	for (i=0; i < r1->numRects; i++) {
    	    if (r1->rects[i].x1 != r2->rects[i].x1 ||
    	        r1->rects[i].x2 != r2->rects[i].x2 ||
    	        r1->rects[i].y1 != r2->rects[i].y1 ||
    	        r1->rects[i].y2 != r2->rects[i].y2) {
	        return( False );
	    }
	}
    }
    return( True );
}

/************************************************************************
 *
 *  _XmRegionPointInRegion ()
 *
 *  Check to see if a point is in a XmRegion.
 ***********************************************************************/

Boolean
_XmRegionPointInRegion(
    XmRegion	r,
    int		x,
    int		y )
{
    register int i;

    if (ISEMPTY(r)) {
        return(False);
    }
    if (!INBOX(r->extents, x, y)) {
        return(False);
    }
    for (i = 0; i < r->numRects; i++)
    {
        if (INBOX (r->rects[i], x, y))
	    return(True);
    }
    return(False);
}

/************************************************************************
 *
 *  _XmRegionOffset ()
 *
 *  Adds an (x, y) offset to an XmRegion.
 ***********************************************************************/

void
_XmRegionOffset(
    XmRegion	r,
    int		x,
    int		y )
{
    register long		nbox;
    register XmRegionBox	*pbox;

    pbox = r->rects;
    nbox = r->numRects;

    while(nbox--)
    {
	pbox->x1 += x;
	pbox->x2 += x;
	pbox->y1 += y;
	pbox->y2 += y;
	pbox++;
    }
    r->extents.x1 += x;
    r->extents.x2 += x;
    r->extents.y1 += y;
    r->extents.y2 += y;
}

/************************************************************************
 *
 *  _XmRegionGetExtents ()
 *
 *  Returns an XmRegion's extents in an XRectangle provided by the caller.
 *  Assumes the extents are current.
 ***********************************************************************/

void
_XmRegionGetExtents(
    XmRegion	r,
    XRectangle	*rect )
{
    rect->x = r->extents.x1;
    rect->y = r->extents.y1;
    rect->width = r->extents.x2 - r->extents.x1;
    rect->height = r->extents.y2 - r->extents.y1;
}

/************************************************************************
 *
 *  _XmRegionGetNumRectangles ()
 *
 *  Returns the number of rectangles used by an XmRegion.
 ***********************************************************************/

long
_XmRegionGetNumRectangles(
    XmRegion 	r )
{
    return (r->numRects);
}

/************************************************************************
 *
 *  _XmRegionGetRectangles ()
 *
 *  Allocates and returns a list and count of XRectangles representing
 *  an XmRegion.
 ***********************************************************************/

void
_XmRegionGetRectangles(
    XmRegion 	r,
    XRectangle	**rects,
    long	*nrects )
{
    register XmRegionBox	*pBox = r->rects;
    register XRectangle		*pRect;
    register long		count = r->numRects;

    if (!(*nrects = count)) {
	*rects = NULL;
	return;
    }

    pRect = *rects = (XRectangle *)
	XtMalloc ((Cardinal) (sizeof (XRectangle) * count));
    if (pRect) {
        for (; count; pBox++, pRect++, count--) {
	    pRect->x = pBox->x1;
	    pRect->y = pBox->y1;
	    pRect->width = (unsigned short) pBox->x2 - pBox->x1;
	    pRect->height = (unsigned short) pBox->y2 - pBox->y1;
        }
    }
}

/************************************************************************
 *
 *  _XmRegionSetGCRegion ()
 *
 *  Sets the clip-mask of a given GC to an XmRegion.
 *  Graphics output is disabled if the XmRegion is empty.
 ***********************************************************************/

void
_XmRegionSetGCRegion(
    Display		*dpy,
    GC			gc,
    int			x_origin,
    int			y_origin,
    XmRegion		r)
{
    XRectangle	*rects;
    long	nrects;

    _XmRegionGetRectangles (r, &rects, &nrects);

    if (rects || !nrects) {	/* not if XtMalloc failed */
	XSetClipRectangles(dpy, gc, x_origin, y_origin,
			   rects, (int) nrects, YXBanded);
    }
    XtFree ((char *) rects);
}

/************************************************************************
 *
 *  miSetExtents()
 *
 *	Computes and resets the extents of an XmRegion. Called by
 *	miSubtract and miIntersect b/c they can't figure it out along the
 *	way or do so easily, as miUnion can.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	The region's 'extents' structure is overwritten.
 *
 ***********************************************************************/

static void
miSetExtents(
    XmRegion  	pReg )
{
    register XmRegionBox	*pBox;
    register XmRegionBox	*pBoxEnd;
    register XmRegionBox	*pExtents;

    if (ISEMPTY(pReg))
    {
	pReg->extents.x1 = 0;
	pReg->extents.y1 = 0;
	pReg->extents.x2 = 0;
	pReg->extents.y2 = 0;
	return;
    }

    pExtents = &pReg->extents;
    pBox = pReg->rects;
    pBoxEnd = &pBox[pReg->numRects - 1];

    /*
     * Since pBox is the first rectangle in the region, it must have the
     * smallest y1 and since pBoxEnd is the last rectangle in the region,
     * it must have the largest y2, because of banding. Initialize x1 and
     * x2 from  pBox and pBoxEnd, resp., as good things to initialize them
     * to...
     */

    pExtents->x1 = pBox->x1;
    pExtents->y1 = pBox->y1;
    pExtents->x2 = pBoxEnd->x2;
    pExtents->y2 = pBoxEnd->y2;

    assert(pExtents->y1 < pExtents->y2);
    while (pBox <= pBoxEnd)
    {
	if (pBox->x1 < pExtents->x1)
	{
	    pExtents->x1 = pBox->x1;
	}
	if (pBox->x2 > pExtents->x2)
	{
	    pExtents->x2 = pBox->x2;
	}
	pBox++;
    }
    assert(pExtents->x1 < pExtents->x2);
}

/************************************************************************
 *
 *  _XmRegionComputeExtents ()
 *
 *  Compute and set the extents of an XmRegion.
 ***********************************************************************/

void
_XmRegionComputeExtents(
    XmRegion	r )
{
    miSetExtents (r);
}

/************************************************************************
 *
 *  miRegionCopy()
 *
 ***********************************************************************/

static void
miRegionCopy(
    register XmRegion	dstrgn,
    register XmRegion	rgn )
{
    if (dstrgn != rgn) /*  don't want to copy to itself */
    {
        if (dstrgn->size < rgn->numRects)
        {
            if (dstrgn->rects)
            {
                if (! (dstrgn->rects = (XmRegionBox *)
		       XtRealloc((char *) dstrgn->rects,
			 (Cardinal) rgn->numRects * (sizeof(XmRegionBox)))))
		    return;
            }
            dstrgn->size = rgn->numRects;
	}
        dstrgn->numRects = rgn->numRects;
        dstrgn->extents.x1 = rgn->extents.x1;
        dstrgn->extents.y1 = rgn->extents.y1;
        dstrgn->extents.x2 = rgn->extents.x2;
        dstrgn->extents.y2 = rgn->extents.y2;

	memmove((char *) dstrgn->rects, (char *) rgn->rects,
	      (int) (rgn->numRects * sizeof(XmRegionBox)));
    }
}

/*======================================================================
 *	    Generic Region Operator
 *====================================================================*/

/*-
 *-----------------------------------------------------------------------
 * miCoalesce --
 *	Attempt to merge the boxes in the current band with those in the
 *	previous one. Used only by miRegionOp.
 *
 * Results:
 *	The new index for the previous band.
 *
 * Side Effects:
 *	If coalescing takes place:
 *	    - rectangles in the previous band will have their y2 fields
 *	      altered.
 *	    - pReg->numRects will be decreased.
 *
 *-----------------------------------------------------------------------
 */
static long
miCoalesce(
    XmRegion	pReg,
    long    	  	prevStart,
    long    	  	curStart )
{
    register XmRegionBox	*pPrevBox; /* Current box in previous band */
    register XmRegionBox	*pCurBox;  /* Current box in current band */
    register XmRegionBox	*pRegEnd;  /* End of region */
    register long		curNumRects; /* Number of rectangles in
						current band */
    long    	  		prevNumRects; /* Number of rectangles in
						 previous band */
    long    	  		bandY1;	 /* Y1 coordinate for current band */

    pRegEnd = &pReg->rects[pReg->numRects];

    pPrevBox = &pReg->rects[prevStart];
    prevNumRects = curStart - prevStart;

    /*
     * Figure out how many rectangles are in the current band. Have to do
     * this because multiple bands could have been added in miRegionOp
     * at the end when one region has been exhausted.
     */
    pCurBox = &pReg->rects[curStart];
    bandY1 = pCurBox->y1;
    for (curNumRects = 0;
	 (pCurBox != pRegEnd) && (pCurBox->y1 == bandY1);
	 curNumRects++)
    {
	pCurBox++;
    }

    if (pCurBox != pRegEnd)
    {
	/*
	 * If more than one band was added, we have to find the start
	 * of the last band added so the next coalescing job can start
	 * at the right place... (given when multiple bands are added,
	 * this may be pointless -- see above).
	 */
	pRegEnd--;
	while (pRegEnd[-1].y1 == pRegEnd->y1)
	{
	    pRegEnd--;
	}
	curStart = pRegEnd - pReg->rects;
	pRegEnd = pReg->rects + pReg->numRects;
    }

    if ((curNumRects == prevNumRects) && (curNumRects != 0)) {
	pCurBox -= curNumRects;
	/*
	 * The bands may only be coalesced if the bottom of the previous
	 * matches the top scanline of the current.
	 */
	if (pPrevBox->y2 == pCurBox->y1)
	{
	    /*
	     * Make sure the bands have boxes in the same places. This
	     * assumes that boxes have been added in such a way that they
	     * cover the most area possible. I.e. two boxes in a band must
	     * have some horizontal space between them.
	     */
	    do
	    {
		if ((pPrevBox->x1 != pCurBox->x1) ||
		    (pPrevBox->x2 != pCurBox->x2))
		{
		    /*
		     * The bands don't line up so they can't be coalesced.
		     */
		    return (curStart);
		}
		pPrevBox++;
		pCurBox++;
		prevNumRects -= 1;
	    } while (prevNumRects != 0);

	    pReg->numRects -= curNumRects;
	    pCurBox -= curNumRects;
	    pPrevBox -= curNumRects;

	    /*
	     * The bands may be merged, so set the bottom y of each box
	     * in the previous band to that of the corresponding box in
	     * the current band.
	     */
	    do
	    {
		pPrevBox->y2 = pCurBox->y2;
		pPrevBox++;
		pCurBox++;
		curNumRects -= 1;
	    } while (curNumRects != 0);

	    /*
	     * If only one band was added to the region, we have to backup
	     * curStart to the start of the previous band.
	     *
	     * If more than one band was added to the region, copy the
	     * other bands down. The assumption here is that the other bands
	     * came from the same region as the current one and no further
	     * coalescing can be done on them since it's all been done
	     * already... curStart is already in the right place.
	     */
	    if (pCurBox == pRegEnd)
	    {
		curStart = prevStart;
	    }
	    else
	    {
		do
		{
		    *pPrevBox++ = *pCurBox++;
		} while (pCurBox != pRegEnd);
	    }

	}
    }
    return (curStart);
}

/*-
 *-----------------------------------------------------------------------
 * miRegionOp --
 *	Apply an operation to two regions. Called by miUnion, miInverse,
 *	miSubtract, miIntersect...
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	THE NEW REGION IS OVERWRITTEN.
 *
 * Notes:
 *	The idea behind this function is to view the two regions as sets.
 *	Together they cover a rectangle of area that this function divides
 *	into horizontal bands where points are covered only by one region
 *	or by both. For the first case, the nonOverlapFunc is called with
 *	each the band and the band's upper and lower extents. For the
 *	second, the overlapFunc is called to process the entire band. It
 *	is responsible for clipping the rectangles in the band, though
 *	this function provides the boundaries.
 *	At the end of each band, the new region is coalesced, if possible,
 *	to reduce the number of rectangles in the region.
 *
 *-----------------------------------------------------------------------
 */
static void
miRegionOp(
    register XmRegion 	newReg,
    XmRegion	  	reg1,
    XmRegion	  	reg2,
    XmOverlapProc    	overlapFunc,
    XmNonOverlapProc    nonOverlap1Func,
    XmNonOverlapProc  	nonOverlap2Func)
{
    register XmRegionBox	*r1; 	    	/* Pointer into first region */
    register XmRegionBox	*r2; 	    	/* Pointer into 2d region */
    XmRegionBox  	  	*r1End;	    	/* End of 1st region */
    XmRegionBox  	  	*r2End;	    	/* End of 2d region */
    register short  	ybot;	    	    	/* Bottom of intersection */
    register short  	ytop;	    	    	/* Top of intersection */
    XmRegionBox  	  	*oldRects;    	/* Old rects for newReg */
    long    	  		prevBand;    	/* Index of start of
						 * previous band in newReg */
    long    	  		curBand;    	/* Index of start of current
						 * band in newReg */
    register XmRegionBox 	*r1BandEnd;    	/* End of current band in r1 */
    register XmRegionBox 	*r2BandEnd;    	/* End of current band in r2 */
    short     	  	top;	    	    	/* Top of non-overlapping
						 * band */
    short     	  	bot;	    	    	/* Bottom of non-overlapping
						 * band */

    /*
     * Initialization:
     *	set r1, r2, r1End and r2End appropriately, preserve the important
     * parts of the destination region until the end in case it's one of
     * the two source regions.
     */
    r1 = reg1->rects;
    r2 = reg2->rects;
    r1End = r1 + reg1->numRects;
    r2End = r2 + reg2->numRects;

    oldRects = newReg->rects;

    /*
     * Mark the "new" region empty and
     * allocate a reasonable number of rectangles for it. The idea
     * is to allocate enough so the individual functions don't need to
     * reallocate and copy the array, which is time consuming, yet we don't
     * have to worry about using too much memory. I hope to be able to
     * nuke the XtRealloc() at the end of this function eventually.
     */

    newReg->numRects = 0;
    newReg->size = MAX(reg1->numRects,reg2->numRects) * 2;

    if (! (newReg->rects = (XmRegionBox *)
	   XtMalloc ((Cardinal) (sizeof(XmRegionBox) * newReg->size)))) {
	newReg->size = 0;
	return;
    }

    /*
     * Initialize ybot and ytop.
     * In the upcoming loop, ybot and ytop serve different functions depending
     * on whether the band being handled is an overlapping or non-overlapping
     * band.
     * 	In the case of a non-overlapping band (only one of the regions
     * has points in the band), ybot is the bottom of the most recent
     * intersection and thus clips the top of the rectangles in that band.
     * ytop is the top of the next intersection between the two regions and
     * serves to clip the bottom of the rectangles in the current band.
     *	For an overlapping band (where the two regions intersect), ytop clips
     * the top of the rectangles of both regions and ybot clips the bottoms.
     */
    if (reg1->extents.y1 < reg2->extents.y1)
	ybot = reg1->extents.y1;
    else
	ybot = reg2->extents.y1;

    /*
     * prevBand serves to mark the start of the previous band so rectangles
     * can be coalesced into larger rectangles. qv. miCoalesce, above.
     * In the beginning, there is no previous band, so prevBand == curBand
     * (curBand is set later on, of course, but the first band will always
     * start at index 0). prevBand and curBand must be indices because of
     * the possible expansion, and resultant moving, of the new region's
     * array of rectangles.
     */
    prevBand = 0;

    do
    {
	curBand = newReg->numRects;

	/*
	 * This algorithm proceeds one source-band (as opposed to a
	 * destination band, which is determined by where the two regions
	 * intersect) at a time. r1BandEnd and r2BandEnd serve to mark the
	 * rectangle after the last one in the current band for their
	 * respective regions.
	 */
	r1BandEnd = r1;
	while ((r1BandEnd != r1End) && (r1BandEnd->y1 == r1->y1))
	{
	    r1BandEnd++;
	}

	r2BandEnd = r2;
	while ((r2BandEnd != r2End) && (r2BandEnd->y1 == r2->y1))
	{
	    r2BandEnd++;
	}

	/*
	 * First handle the band that doesn't intersect, if any.
	 *
	 * Note that attention is restricted to one band in the
	 * non-intersecting region at once, so if a region has n
	 * bands between the current position and the next place it overlaps
	 * the other, this entire loop will be passed through n times.
	 */
	if (r1->y1 < r2->y1)
	{
	    top = MAX(r1->y1,ybot);
	    bot = MIN(r1->y2,r2->y1);

	    if ((top != bot) && nonOverlap1Func)
	    {
		(* nonOverlap1Func) (newReg, r1, r1BandEnd, top, bot);
	    }

	    ytop = r2->y1;
	}
	else if (r2->y1 < r1->y1)
	{
	    top = MAX(r2->y1,ybot);
	    bot = MIN(r2->y2,r1->y1);

	    if ((top != bot) && nonOverlap2Func)
	    {
		(* nonOverlap2Func) (newReg, r2, r2BandEnd, top, bot);
	    }

	    ytop = r1->y1;
	}
	else
	{
	    ytop = r1->y1;
	}

	/*
	 * If any rectangles got added to the region, try and coalesce them
	 * with rectangles from the previous band. Note we could just do
	 * this test in miCoalesce, but some machines incur a not
	 * inconsiderable cost for function calls, so...
	 */
	if (newReg->numRects != curBand)
	{
	    prevBand = miCoalesce (newReg, prevBand, curBand);
	}

	/*
	 * Now see if we've hit an intersecting band. The two bands only
	 * intersect if ybot > ytop
	 */
	ybot = MIN(r1->y2, r2->y2);
	curBand = newReg->numRects;
	if (ybot > ytop)
	{
	    (* overlapFunc) (newReg, r1, r1BandEnd, r2, r2BandEnd, ytop, ybot);

	}

	if (newReg->numRects != curBand)
	{
	    prevBand = miCoalesce (newReg, prevBand, curBand);
	}

	/*
	 * If we've finished with a band (y2 == ybot) we skip forward
	 * in the region to the next band.
	 */
	if (r1->y2 == ybot)
	{
	    r1 = r1BandEnd;
	}
	if (r2->y2 == ybot)
	{
	    r2 = r2BandEnd;
	}
    } while ((r1 != r1End) && (r2 != r2End));

    /*
     * Deal with whichever region still has rectangles left.
     */
    curBand = newReg->numRects;
    if (r1 != r1End)
    {
	if (nonOverlap1Func)
	{
	    do
	    {
		r1BandEnd = r1;
		while ((r1BandEnd < r1End) && (r1BandEnd->y1 == r1->y1))
		{
		    r1BandEnd++;
		}
		(* nonOverlap1Func) (newReg, r1, r1BandEnd,
				     MAX(r1->y1,ybot), r1->y2);
		r1 = r1BandEnd;
	    } while (r1 != r1End);
	}
    }
    else if ((r2 != r2End) && nonOverlap2Func)
    {
	do
	{
	    r2BandEnd = r2;
	    while ((r2BandEnd < r2End) && (r2BandEnd->y1 == r2->y1))
	    {
		 r2BandEnd++;
	    }
	    (* nonOverlap2Func) (newReg, r2, r2BandEnd,
				MAX(r2->y1,ybot), r2->y2);
	    r2 = r2BandEnd;
	} while (r2 != r2End);
    }

    if (newReg->numRects != curBand)
    {
	(void) miCoalesce (newReg, prevBand, curBand);
    }

    /*
     * A bit of cleanup. To keep regions from growing without bound,
     * we shrink the array of rectangles to match the new number of
     * rectangles in the region. This never goes to 0, however...
     *
     * Only do this stuff if the number of rectangles allocated is more than
     * twice the number of rectangles in the region (a simple optimization...).
     */
    if (newReg->numRects < (newReg->size >> 1))
    {
	if (newReg->numRects)
	{
	    XmRegionBox *prev_rects = newReg->rects;

	    newReg->size = newReg->numRects;
	    newReg->rects = (XmRegionBox *) XtRealloc ((char *) newReg->rects,
			     (Cardinal) (sizeof(XmRegionBox) * newReg->size));
	    if (! newReg->rects)
		newReg->rects = prev_rects;
	}
	else
	{
	    /*
	     * No point in doing the extra work involved in an XtRealloc if
	     * the region is empty
	     */
	    newReg->size = 1;
	    XtFree((char *) newReg->rects);
	    newReg->rects = (XmRegionBox *) XtMalloc(sizeof(XmRegionBox));
	}
    }
    XtFree ((char *) oldRects);
}

/*======================================================================
 *	    Region Intersection
 *====================================================================*/
/*-
 *-----------------------------------------------------------------------
 * miIntersectO --
 *	Handle an overlapping band for miIntersect.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Rectangles may be added to the region.
 *
 *-----------------------------------------------------------------------
 */
static void
miIntersectO(
    XmRegion		pReg,
    register XmRegionBox	*r1,
    XmRegionBox  	  	*r1End,
    register XmRegionBox	*r2,
    XmRegionBox  	  	*r2End,
    short    	  		y1,
    short    	  		y2 )
{
    register short  		x1;
    register short  		x2;
    register XmRegionBox	*pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    while ((r1 != r1End) && (r2 != r2End))
    {
	x1 = MAX(r1->x1,r2->x1);
	x2 = MIN(r1->x2,r2->x2);

	/*
	 * If there's any overlap between the two rectangles, add that
	 * overlap to the new region.
	 * There's no need to check for subsumption because the only way
	 * such a need could arise is if some region has two rectangles
	 * right next to each other. Since that should never happen...
	 */
	if (x1 < x2)
	{
	    assert(y1<y2);

	    MEMCHECK(pReg, pNextRect, pReg->rects);
	    pNextRect->x1 = x1;
	    pNextRect->y1 = y1;
	    pNextRect->x2 = x2;
	    pNextRect->y2 = y2;
	    pReg->numRects += 1;
	    pNextRect++;
	    assert(pReg->numRects <= pReg->size);
	}

	/*
	 * Need to advance the pointers. Shift the one that extends
	 * to the right the least, since the other still has a chance to
	 * overlap with that region's next rectangle, if you see what I mean.
	 */
	if (r1->x2 < r2->x2)
	{
	    r1++;
	}
	else if (r2->x2 < r1->x2)
	{
	    r2++;
	}
	else
	{
	    r1++;
	    r2++;
	}
    }
}

/************************************************************************
 *
 *  _XmRegionIntersect ()
 *
 *  Intersect two regions and place the result into a new region.
 *  The new region is overwritten.
 ***********************************************************************/

void
_XmRegionIntersect(
    XmRegion 	  	reg1,
    XmRegion	  	reg2,
    XmRegion 		newReg )
{
   /*
    *  Check for trivial reject.
    */

    if ( ISEMPTY(reg1) || ISEMPTY(reg2) ||
	(!EXTENTCHECK(&reg1->extents, &reg2->extents)))
        newReg->numRects = 0;
    else
	miRegionOp (newReg, reg1, reg2,
    		miIntersectO, NULL, NULL);

    /*
     * Can't alter newReg's extents before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the extents of those regions being the same. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */

    miSetExtents(newReg);
}

/************************************************************************
 *
 *  _XmRegionIntersectRectWithRegion ()
 *
 *  Intersect a rectangle with a region and place the result into a new
 *  region.  The new region is overwritten.
 ***********************************************************************/

void
_XmRegionIntersectRectWithRegion(
    XRectangle		*rect,
    XmRegion		source,
    XmRegion		dest )
{
    XmRegionRec	region;

    region.rects = &region.extents;
    region.numRects = 1;
    region.extents.x1 = rect->x;
    region.extents.y1 = rect->y;
    region.extents.x2 = rect->x + rect->width;
    region.extents.y2 = rect->y + rect->height;
    region.size = 1;

    _XmRegionIntersect(&region, source, dest);
}

/*======================================================================
 *	    Region Union
 *====================================================================*/

/*-
 *-----------------------------------------------------------------------
 * miUnionNonO --
 *	Handle a non-overlapping band for the union operation. Just
 *	Adds the rectangles into the region. Doesn't have to check for
 *	subsumption or anything.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	pReg->numRects is incremented and the final rectangles overwritten
 *	with the rectangles we're passed.
 *
 *-----------------------------------------------------------------------
 */
static void
miUnionNonO(
    XmRegion		pReg,
    register XmRegionBox	*r,
    XmRegionBox    		*rEnd,
    register short  		y1,
    register short  		y2 )
{
    register XmRegionBox	*pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    assert(y1 <= y2);

    while (r != rEnd)
    {
	assert(r->x1 < r->x2);
	MEMCHECK(pReg, pNextRect, pReg->rects);
	pNextRect->x1 = r->x1;
	pNextRect->y1 = y1;
	pNextRect->x2 = r->x2;
	pNextRect->y2 = y2;
	pReg->numRects += 1;
	pNextRect++;

	assert(pReg->numRects<=pReg->size);
	r++;
    }
}

/*-
 *-----------------------------------------------------------------------
 * miUnionO --
 *	Handle an overlapping band for the union operation. Picks the
 *	left-most rectangle each time and merges it into the region.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Rectangles are overwritten in pReg->rects and pReg->numRects will
 *	be changed.
 *
 *-----------------------------------------------------------------------
 */

static void
miUnionO(
    XmRegion		pReg,
    register XmRegionBox	*r1,
    XmRegionBox  	  	*r1End,
    register XmRegionBox	*r2,
    XmRegionBox  	  	*r2End,
    register short		y1,
    register short		y2 )
{
    register XmRegionBox	*pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

#define MERGERECT(r) \
    if ((pReg->numRects != 0) &&  \
	(pNextRect[-1].y1 == y1) &&  \
	(pNextRect[-1].y2 == y2) &&  \
	(pNextRect[-1].x2 >= r->x1))  \
    {  \
	if (pNextRect[-1].x2 < r->x2)  \
	{  \
	    pNextRect[-1].x2 = r->x2;  \
	    assert(pNextRect[-1].x1<pNextRect[-1].x2); \
	}  \
    }  \
    else  \
    {  \
	MEMCHECK(pReg, pNextRect, pReg->rects);  \
	pNextRect->y1 = y1;  \
	pNextRect->y2 = y2;  \
	pNextRect->x1 = r->x1;  \
	pNextRect->x2 = r->x2;  \
	pReg->numRects += 1;  \
        pNextRect += 1;  \
    }  \
    assert(pReg->numRects<=pReg->size);\
    r++;

    assert (y1<y2);
    while ((r1 != r1End) && (r2 != r2End))
    {
	if (r1->x1 < r2->x1)
	{
	    MERGERECT(r1);
	}
	else
	{
	    MERGERECT(r2);
	}
    }

    if (r1 != r1End)
    {
	do
	{
	    MERGERECT(r1);
	} while (r1 != r1End);
    }
    else while (r2 != r2End)
    {
	MERGERECT(r2);
    }
}

/************************************************************************
 *
 *  _XmRegionUnion ()
 *
 *  Union two regions and place the result into a new region.
 *  The new region is overwritten.
 ***********************************************************************/

void
_XmRegionUnion(
    XmRegion 	  reg1,
    XmRegion	  reg2,
    XmRegion 	  newReg )
{
    /*  checks all the simple cases */

    /*
     * XmRegion 1 and 2 are the same or region 1 is empty
     */
    if ( reg1 == reg2 || ISEMPTY(reg1) )
    {
	miRegionCopy(newReg, reg2);
	return;
    }

    /*
     * if nothing to union (region 2 empty)
     */
    if (ISEMPTY(reg2))
    {
	miRegionCopy(newReg, reg1);
	return;
    }

    /*
     * Region 1 completely subsumes region 2
     */
    if ((reg1->numRects == 1) &&
	(reg1->extents.x1 <= reg2->extents.x1) &&
	(reg1->extents.y1 <= reg2->extents.y1) &&
	(reg1->extents.x2 >= reg2->extents.x2) &&
	(reg1->extents.y2 >= reg2->extents.y2))
    {
	miRegionCopy(newReg, reg1);
	return;
    }

    /*
     * Region 2 completely subsumes region 1
     */
    if ((reg2->numRects == 1) &&
	(reg2->extents.x1 <= reg1->extents.x1) &&
	(reg2->extents.y1 <= reg1->extents.y1) &&
	(reg2->extents.x2 >= reg1->extents.x2) &&
	(reg2->extents.y2 >= reg1->extents.y2))
    {
	miRegionCopy(newReg, reg2);
	return;
    }

    miRegionOp (newReg, reg1, reg2, miUnionO,
    		miUnionNonO, miUnionNonO);

    newReg->extents.x1 = MIN(reg1->extents.x1, reg2->extents.x1);
    newReg->extents.y1 = MIN(reg1->extents.y1, reg2->extents.y1);
    newReg->extents.x2 = MAX(reg1->extents.x2, reg2->extents.x2);
    newReg->extents.y2 = MAX(reg1->extents.y2, reg2->extents.y2);
}

/************************************************************************
 *
 *  _XmRegionUnionRectWithRegion ()
 *
 *  Union a rectangle with a region and place the result into a new
 *  region.  The new region is overwritten.
 ***********************************************************************/

void
_XmRegionUnionRectWithRegion(
    XRectangle		*rect,
    XmRegion		source,
    XmRegion		dest )
{
    XmRegionRec	region;
    XmRegionBox box[1];

/*
    region.rects = &region.extents;
*/
    region.rects = box;
    region.numRects = 1;
    region.rects->x1  = region.extents.x1 = rect->x;
    region.rects->y1 = region.extents.y1 = rect->y;
    region.rects->x2 = region.extents.x2 = rect->x + rect->width;
    region.rects->y2 = region.extents.y2 = rect->y + rect->height;
    region.size = 1;

    _XmRegionUnion(&region, source, dest);
}

/*======================================================================
 * 	    	  Region Subtraction
 *====================================================================*/

/*-
 *-----------------------------------------------------------------------
 * miSubtractNonO --
 *	Deal with non-overlapping band for subtraction. Any parts from
 *	region 2 we discard. Anything from region 1 we add to the region.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	pReg may be affected.
 *
 *-----------------------------------------------------------------------
 */
static void
miSubtractNonO1(
    XmRegion		pReg,
    register XmRegionBox	*r,
    XmRegionBox  	  	*rEnd,
    register short  		y1,
    register short   		y2 )
{
    register XmRegionBox	*pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    assert(y1<y2);

    while (r != rEnd)
    {
	assert(r->x1<r->x2);
	MEMCHECK(pReg, pNextRect, pReg->rects);
	pNextRect->x1 = r->x1;
	pNextRect->y1 = y1;
	pNextRect->x2 = r->x2;
	pNextRect->y2 = y2;
	pReg->numRects += 1;
	pNextRect++;

	assert(pReg->numRects <= pReg->size);

	r++;
    }
}

/*-
 *-----------------------------------------------------------------------
 * miSubtractO --
 *	Overlapping band subtraction. x1 is the left-most point not yet
 *	checked.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	pReg may have rectangles added to it.
 *
 *-----------------------------------------------------------------------
 */
static void
miSubtractO(
    XmRegion		pReg,
    register XmRegionBox	*r1,
    XmRegionBox  	  	*r1End,
    register XmRegionBox	*r2,
    XmRegionBox  	  	*r2End,
    register short  		y1,
    register short  		y2 )
{
    register XmRegionBox	*pNextRect;
    register int  		x1;

    x1 = r1->x1;

    assert(y1<y2);
    pNextRect = &pReg->rects[pReg->numRects];

    while ((r1 != r1End) && (r2 != r2End))
    {
	if (r2->x2 <= x1)
	{
	    /*
	     * Subtrahend missed the boat: go to next subtrahend.
	     */
	    r2++;
	}
	else if (r2->x1 <= x1)
	{
	    /*
	     * Subtrahend preceeds minuend: nuke left edge of minuend.
	     */
	    x1 = r2->x2;
	    if (x1 >= r1->x2)
	    {
		/*
		 * Minuend completely covered: advance to next minuend and
		 * reset left fence to edge of new minuend.
		 */
		if (++r1 != r1End)
		{
		    x1 = r1->x1;
		}
	    }
	    else
	    {
		/*
		 * Subtrahend now used up since it doesn't extend beyond
		 * minuend
		 */
		r2++;
	    }
	}
	else if (r2->x1 < r1->x2)
	{
	    /*
	     * Left part of subtrahend covers part of minuend: add uncovered
	     * part of minuend to region and skip to next subtrahend.
	     */
	    assert(x1<r2->x1);
	    MEMCHECK(pReg, pNextRect, pReg->rects);
	    pNextRect->x1 = x1;
	    pNextRect->y1 = y1;
	    pNextRect->x2 = r2->x1;
	    pNextRect->y2 = y2;
	    pReg->numRects += 1;
	    pNextRect++;

	    assert(pReg->numRects<=pReg->size);

	    x1 = r2->x2;
	    if (x1 >= r1->x2)
	    {
		/*
		 * Minuend used up: advance to new...
		 */
		if (++r1 != r1End)
		{
		    x1 = r1->x1;
		}
	    }
	    else
	    {
		/*
		 * Subtrahend used up
		 */
		r2++;
	    }
	}
	else
	{
	    /*
	     * Minuend used up: add any remaining piece before advancing.
	     */
	    if (r1->x2 > x1)
	    {
		MEMCHECK(pReg, pNextRect, pReg->rects);
		pNextRect->x1 = x1;
		pNextRect->y1 = y1;
		pNextRect->x2 = r1->x2;
		pNextRect->y2 = y2;
		pReg->numRects += 1;
		pNextRect++;
		assert(pReg->numRects<=pReg->size);
	    }
	    if (++r1 != r1End)
	    {
		x1 = r1->x1;
	    }
	}
    }

    /*
     * Add remaining minuend rectangles to region.
     */
    while (r1 != r1End)
    {
	assert(x1<r1->x2);
	MEMCHECK(pReg, pNextRect, pReg->rects);
	pNextRect->x1 = x1;
	pNextRect->y1 = y1;
	pNextRect->x2 = r1->x2;
	pNextRect->y2 = y2;
	pReg->numRects += 1;
	pNextRect++;

	assert(pReg->numRects<=pReg->size);

	if (++r1 != r1End)
	{
	    x1 = r1->x1;
	}
    }
}

/************************************************************************
 *
 *  _XmRegionSubtract ()
 *
 *  Subtract one region from another and place the result into a new
 *  region.  The new region is overwritten.
 ***********************************************************************/

void
_XmRegionSubtract(
    XmRegion 	  	regM,
    XmRegion	  	regS,
    XmRegion		regD )
{
   /* check for trivial reject */
    if ( ISEMPTY(regM) || ISEMPTY(regS) ||
	(!EXTENTCHECK(&regM->extents, &regS->extents)) )
    {
	miRegionCopy(regD, regM);
        return;
    }

    miRegionOp (regD, regM, regS, miSubtractO,
    		miSubtractNonO1, NULL);

    /*
     * Can't alter newReg's extents before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the extents of those regions being the unaltered. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */

    miSetExtents (regD);
}

/************************************************************************
 *
 *  Utility procedure Compress:
 *
 * Replace r by the region r', where
 *   p in r' iff (Quantifer m <= dx) (p + m in r), and
 *   Quantifier is Exists if grow is TRUE, For all if grow is FALSE, and
 *   (x,y) + m = (x+m,y) if xdir is TRUE; (x,y+m) if xdir is FALSE.
 *
 * Thus, if xdir is TRUE and grow is FALSE, r is replaced by the region
 * of all points p such that p and the next dx points on the same
 * horizontal scan line are all in r.  We do this using by noting
 * that p is the head of a run of length 2^i + k iff p is the head
 * of a run of length 2^i and p+2^i is the head of a run of length
 * k. Thus, the loop invariant: s contains the region corresponding
 * to the runs of length shift.  r contains the region corresponding
 * to the runs of length 1 + dxo & (shift-1), where dxo is the original
 * value of dx.  dx = dxo & ~(shift-1).  As parameters, s and t are
 * scratch regions, so that we don't have to allocate them on every
 * call.
 ***********************************************************************/

static void
Compress(
    XmRegion		r,
    XmRegion		s,
    XmRegion		t,
    unsigned		dx,
    int			xdir,
    int			grow )
{
    register unsigned shift = 1;

    miRegionCopy (s, r);
    while (dx) {
        if (dx & shift) {

	    if (xdir) {
		_XmRegionOffset (r, -(int)shift, 0);
	    }
	    else {
		_XmRegionOffset (r, 0, -(int)shift);
	    }
	    if (grow) {
		_XmRegionUnion (r,s,r);
	    }
	    else {
		_XmRegionIntersect (r,s,r);
	    }

            dx -= shift;
            if (!dx) break;
        }
	miRegionCopy (t, s);
	if (xdir) {
	    _XmRegionOffset (s, -(int)shift, 0);
	}
	else {
	    _XmRegionOffset (s, 0, -(int)shift);
	}
	if (grow) {
	    _XmRegionUnion (s,t,s);
	}
	else {
	    _XmRegionIntersect (s,t,s);
	}
        shift <<= 1;
    }
}

/************************************************************************
 *
 *  ShrinkRegion ()
 *
 *  Shrink an XmRegion dx and dy in the x and y directions, respectively.
 *  The width and height are reduced by 2dx and 2dy, respectively.
 ***********************************************************************/

static void
ShrinkRegion(
    XmRegion	r,
    XmRegion	s,
    XmRegion	t,
    int		dx,
    int		dy )
{
    int	grow;

    if ((grow = (dx < 0)) != 0) {
	dx = -dx;
    }
    if (dx) {
	Compress (r, s, t, (unsigned) 2*dx, TRUE, grow);
    }
    if ((grow = (dy < 0)) != 0) {
	dy = -dy;
    }
    if (dy) {
        Compress (r, s, t, (unsigned) 2*dy, FALSE, grow);
    }
    _XmRegionOffset (r, dx, dy);
}

/************************************************************************
 *
 *  _XmRegionShrink ()
 *
 *  Shrink an XmRegion dx and dy in the x and y directions, respectively.
 *  The width and height are reduced by 2dx and 2dy, respectively.
 ***********************************************************************/

void
_XmRegionShrink(
    XmRegion	r,
    int		dx,
    int		dy )
{
    XmRegion	s, t;

    if (!dx && !dy) {
        return;
    }
    if (!(s = _XmRegionCreate())) {
	return;
    }
    if (!(t = _XmRegionCreate())) {
        _XmRegionDestroy (s);
	return;
    }

    ShrinkRegion (r, s, t, dx, dy);

    _XmRegionDestroy (s);
    _XmRegionDestroy (t);
}

/************************************************************************
 *
 *  CreateLeftShadow ()
 *
 *  Create a one-pixel thick left shadow segment for the current XY-
 *  banded rectangle.
 ***********************************************************************/

static void
CreateLeftShadow(
    XmRegionBox		*here,
    unsigned long	mask,
    XSegment		**segml,
    int			*segmc,
    int			*segmi )
{
    Position	start_y = here->y1 + 1;
    Position	end_y = here->y2;

    if (*segmi >= *segmc) {
	*segml = (XSegment *) XtRealloc ((char *)(*segml),
		 (Cardinal) ((sizeof(XSegment) << 1) * (*segmc)));
        if (*segml == NULL) {
	    XmeWarning(NULL, MESSAGE1);
	    *segmi = *segmc = 0;
	    return;
	}
	*segmc *= 2;
    }

    if (mask & BL_OPEN) {
        end_y--;
    }
    if (start_y <= end_y) {
	(*segml)[*segmi].x1 = (*segml)[*segmi].x2 = here->x1;
	(*segml)[*segmi].y1 = start_y;
	(*segml)[*segmi].y2 = end_y;
	(*segmi)++;
    }
}

/************************************************************************
 *
 *  CreateRightShadow ()
 *
 *  Create a one-pixel thick right shadow segment for the current XY-
 *  banded rectangle.
 ***********************************************************************/

static void
CreateRightShadow(
    XmRegionBox		*here,
    unsigned long	mask,
    XSegment		**segml,
    int			*segmc,
    int			*segmi )
{
    Position	start_y = here->y1;
    Position	end_y = here->y2;

    if (*segmi >= *segmc) {
	*segml = (XSegment *) XtRealloc ((char *)(*segml),
		 (Cardinal) ((sizeof(XSegment) << 1) * (*segmc)));
        if (*segml == NULL) {
	    XmeWarning(NULL, MESSAGE1);
	    *segmi = *segmc = 0;
	    return;
	}
	*segmc *= 2;
    }

    if (!(mask & TR_MATCH)) {
        start_y++;
    }
    if (!(mask & BR_OPEN)) {
        end_y--;
    }
    if (start_y <= end_y) {
        (*segml)[*segmi].x1 = (*segml)[*segmi].x2 = here->x2;
        (*segml)[*segmi].y1 = start_y;
        (*segml)[*segmi].y2 = end_y;
	(*segmi)++;
    }
}

/************************************************************************
 *
 *  CreateTopShadow ()
 *
 *  Create a one-pixel thick top shadow segment between two points.
 ***********************************************************************/

static void
CreateTopShadow(
    Position		start_x,
    Position		end_x,
    XmRegionBox		*here,
    unsigned long	mask,
    XSegment		**segml,
    int			*segmc,
    int			*segmi )
{
    if (*segmi >= *segmc) {
	*segml = (XSegment *) XtRealloc ((char *)(*segml),
		 (Cardinal) ((sizeof(XSegment) << 1) * (*segmc)));
        if (*segml == NULL) {
	    XmeWarning(NULL, MESSAGE1);
	    *segmi = *segmc = 0;
	    return;
	}
	*segmc *= 2;
    }

    if (mask & TL_OPEN) {
        start_x++;
    }
    if (start_x <= end_x) {
	(*segml)[*segmi].y1 = (*segml)[*segmi].y2 = here->y1;
	(*segml)[*segmi].x1 = start_x;
	(*segml)[*segmi].x2 = end_x;
	(*segmi)++;
    }
}

/************************************************************************
 *
 *  CreateBottomShadow ()
 *
 *  Create a one-pixel thick bottom shadow segment between two points.
 ***********************************************************************/

static void
CreateBottomShadow(
    Position		start_x,
    Position		end_x,
    XmRegionBox		*here,
    unsigned long	mask,
    XSegment		**segml,
    int			*segmc,
    int			*segmi )
{
    if (*segmi >= *segmc) {
	*segml = (XSegment *) XtRealloc ((char *)(*segml),
		 (Cardinal) ((sizeof(XSegment) << 1) * (*segmc)));
        if (*segml == NULL) {
	    XmeWarning(NULL, MESSAGE1);
	    *segmi = *segmc = 0;
	    return;
	}
	*segmc *= 2;
    }

    if (!(mask & BL_OPEN)) {
        start_x++;
    }
    if (start_x <= end_x) {
	(*segml)[*segmi].y1 = (*segml)[*segmi].y2 = here->y2;
	(*segml)[*segmi].x1 = start_x;
	(*segml)[*segmi].x2 = end_x;
	(*segmi)++;
    }
}

/************************************************************************
 *
 *  _XmRegionDrawShadow ()
 *
 *  Shadows or highlights a possibly nonrectangular XmRegion.
 *  This is done in multiple steps:
 *
 *  1. Remove any border region specified by a nonzero border_thick
 *     parameter by shrinking the XmRegion() by that amount in both width
 *     and height.
 *
 *  2. Create the shadow segment list by iteratively shadowing the XmRegion
 *     one pixel thick, then shrinking the XmRegion by one pixel in both
 *     width and height, until the shadow of specified thickness is
 *     obtained.  Each iteration scans the XmRegion's XY-banded rectangle
 *     list to determine where left, right, top, and bottom shadows should
 *     be drawn.  The number and characteristics of these shadows
 *     depend upon how the XY-banded rectangles overlap.
 *
 *  This algorithm will usually correctly shadow detail finer than twice
 *  the shadow thickness.  The reason why it does not always work as one
 *  might expect is that the ShrinkRegion() algorithm will drop XY-banded
 *  rectangles when they become one pixel wide or high, and in some cases
 *  the resulting shadow has "holes".  This can be seen in shadowed
 *  regions having curves not aligned horizontally or vertically.
 *
 *  To draw a highlight, use the same highlight GC as both top_gc and
 *  bottom_gc and specify border_thick equal to the XmRegion's
 *  borderWidth.
 *
 *  To draw a shadow, use appropraite top_gc and bottom_gc and specify
 *  border_thick equal to the sum of the XmRegion's borderWidth and
 *  highlightThickness.
 *
 ***********************************************************************/

void
_XmRegionDrawShadow(
    Display		*display,
    Drawable		d,
    GC			top_gc,
    GC			bottom_gc,
    XmRegion		region,
    Dimension		border_thick,
    Dimension		shadow_thick,
    unsigned int	shadow_type )
{
    XmRegion		workReg;
    XmRegion		scReg1, scReg2;
    XSegment		*topSegms;
    int			topSegmCount;
    int			curTopSeg;
    XSegment		*botSegms;
    int			botSegmCount;
    int			curBotSeg;
    register XmRegionBox *above, *here, *below;
    register XmRegionBox *end_above, *end_here, *end_below;
    XmRegionBox		*end_all;
    unsigned long	mask;
    Position		x1, x2, y;
    Position		start_x, end_x;
    Boolean		draw;
    GC  		tmp_gc;
    XmRegionBox		*rects;
    long		nrects = region->numRects;
    int min_y, i ;

    if (!d || ISEMPTY(region) || !shadow_thick) {
	return;
    }

    if (shadow_type == XmSHADOW_IN) {
        tmp_gc = top_gc ;
        top_gc = bottom_gc ;  /* switch top and bottom shadows */
        bottom_gc = tmp_gc ;
    }

    /*
     *  Create scratch regions and segment arrays.
     */

    if (!(scReg1 = _XmRegionCreate())) {
	XmeWarning(NULL, MESSAGE1);
	return;
    }
    if (!(scReg2 = _XmRegionCreate())) {
	XmeWarning(NULL, MESSAGE1);
	_XmRegionDestroy (scReg1);
	return;
    }
    if (!(workReg = _XmRegionCreateSize (nrects))) {
	XmeWarning(NULL, MESSAGE1);
	_XmRegionDestroy (scReg2);
	_XmRegionDestroy (scReg1);
	return;
    }
    miRegionCopy (workReg, region);

    /* Currently this code crashes when given negative y and
       some big thickness. (it crashes below, the line that accesses
       'above' in:   while (above < end_above && above->x2 <= x1) { ...
     The reason is not currently understood and in the mean time,
     we patch it so that negative y are not present in the region.
     Just before drawing the segments, at the end of this function, we
     reincorpate this negative offset. min_y is used as both flag
     and offset value . CR 9141 */
    for (i = 0, min_y = 0 ; i < nrects; i++) {
	min_y = MIN(min_y, workReg->rects[i].y1);
	min_y = MIN(min_y, workReg->rects[i].y2);
    }
    if (min_y < 0) {
	for (i = 0 ; i < nrects; i++) {
	    workReg->rects[i].y1 += -min_y;
	    workReg->rects[i].y2 += -min_y;
	}
    }

    topSegmCount = botSegmCount = (int) (nrects * shadow_thick << 1);

    if (!(topSegms = (XSegment *)
	    XtMalloc(sizeof (XSegment) * topSegmCount))) {
	XmeWarning(NULL, MESSAGE1);
	_XmRegionDestroy (workReg);
	_XmRegionDestroy (scReg2);
	_XmRegionDestroy (scReg1);
	return;
    }
    if (!(botSegms = (XSegment *)
	    XtMalloc(sizeof (XSegment) * botSegmCount))) {
	XmeWarning(NULL, MESSAGE1);
	XtFree((char *) topSegms);
	_XmRegionDestroy (workReg);
	_XmRegionDestroy (scReg2);
	_XmRegionDestroy (scReg1);
	return;
    }
    curTopSeg = curBotSeg = 0;

    /*
     *  Remove the border from workReg.
     */

    if (workReg->numRects && border_thick) {
	ShrinkRegion (workReg, scReg1, scReg2, border_thick, border_thick);
    }

    /*
     *  Draw the shadows one pixel wide at a time.
     */

    while ((nrects = workReg->numRects) && shadow_thick) {

	/*
	 *  Set up the initial banded rectangle pointers.
	 */

	rects = workReg->rects;
	end_all = &rects[nrects];
	end_above = NULL;
	end_below = here = end_here = rects;
	while (end_below != end_all && end_below->y1 == end_here->y1) {
            end_below++;
	}
	y = -1;

	/*
	 *  Scan through the bands
	 */

	while (end_here != end_all) {

	    /*
	     *  Shift down one band.
	     *  valid:  y, end_above, here, end_here, end_below, end_all
	     */

            above = end_above;		/* in case not there yet */
            below = end_below;		/* in case not there yet */

	    end_above = end_here;
            if (end_here->y1 != y) {	/* gap between bands */
	        above = end_above;
	    }

            y = here->y2;

	    end_here = end_below;
            while (end_below != end_all && end_below->y1 == end_here->y1) {
                end_below++;
            }

            if ((end_here == end_all) || (end_here->y1 != y)) { /* gap */
	        below = end_below;
	    }

            /*
	     *  Process this band.
	     */

            while (here < end_here) {
                x1 = here->x1;
                x2 = here->x2;
	        mask = 0;

	        /*
	         *  Determine the left shadow.
	         */

	        while (above < end_above && above->x2 <= x1) {
		    above++;
	        }
	        while (below < end_below && below->x2 <= x1) {
		    below++;
	        }

                if (above < end_above && above->x1 < x1) {
		    mask |= TL_OPEN;
	        }
                else if (above < end_above && above->x1 == x1) {
		    mask |= TL_MATCH;
	        }

                if (below < end_below && below->x1 < x1) {
		    mask |= BL_OPEN;
	        }
                else if (below < end_below && below->x1 == x1) {
		    mask |= BL_MATCH;
	        }

	        CreateLeftShadow (here, mask, &topSegms, &topSegmCount,
			          &curTopSeg);

	        /*
	         *  Determine the top shadow(s).
	         */

	        if (mask & (TL_OPEN | TL_MATCH)) {
	            draw = False;
	        }
	        else {
	            draw = True;
	        }
                start_x = x1;

	        while (above < end_above && above->x2 < x2) {
		    if (draw) {
	                CreateTopShadow (start_x, above->x1, here,
				         mask | TR_OPEN,
				         &topSegms, &topSegmCount, &curTopSeg);
		    }
		    mask &= ~TL_MATCH;
		    mask |= TL_OPEN;
	            draw = True;
                    start_x = above->x2;
		    above++;
	        }

	        if (above < end_above && above->x2 == x2) {
		    if (draw) {
	                CreateTopShadow (start_x, above->x1, here,
				         mask | TR_OPEN,
				         &topSegms, &topSegmCount, &curTopSeg);
		    }
		    mask |= TR_MATCH;
		    above++;
	        }
	        else {
		    if (above < end_above && above->x1 < x2) {
		        mask |= TR_OPEN;
		        end_x = above->x1;
	            }
	            else {
		        end_x = x2;
	            }
	            if (draw) {
	                CreateTopShadow (start_x, end_x, here, mask,
				         &topSegms, &topSegmCount, &curTopSeg);
		    }
	        }

	        /*
	         *  Determine the bottom shadow(s).
	         */

	        if (mask & (BL_OPEN | BL_MATCH)) {
	            draw = False;
	        }
	        else {
	            draw = True;
	        }
                start_x = x1;

	        while (below < end_below && below->x2 < x2) {
		    if (draw) {
	                CreateBottomShadow (start_x, below->x1, here,
					    mask | BR_OPEN, &botSegms,
					    &botSegmCount, &curBotSeg);
		    }
		    mask &= ~BL_MATCH;
		    mask |= BL_OPEN;
	            draw = True;
                    start_x = below->x2;
		    below++;
	        }

	        if (below < end_below && below->x2 == x2) {
		    if (draw) {
	                CreateBottomShadow (start_x, below->x1, here,
					    mask | BR_OPEN, &botSegms,
					    &botSegmCount, &curBotSeg);
		    }
		    mask |= BR_MATCH;
		    below++;
	        }
	        else {
	            if (below < end_below && below->x1 < x2) {
		        mask |= BR_OPEN;
		        end_x = below->x1;
	            }
	            else {
		        end_x = x2;
	            }
	            if (draw) {
	                CreateBottomShadow (start_x, end_x, here, mask,
				            &botSegms, &botSegmCount,
					    &curBotSeg);
		    }
	        }

	        /*
	         *  Determine the right shadow.
	         */

	        CreateRightShadow (here, mask, &botSegms, &botSegmCount,
			           &curBotSeg);
	        here++;
            }
        }

	ShrinkRegion (workReg, scReg1, scReg2, 1, 1);
	shadow_thick--;
    }

    /*
     *  Draw the segments.
     */

    /* reincorporate the negative offset taken away one page up. CR 9141 */
    if (min_y < 0) {
	for (i = 0 ; i < curTopSeg; i++) {
	    topSegms[i].y1 += min_y;
	    topSegms[i].y2 += min_y;
	}
	for (i = 0 ; i < curBotSeg; i++) {
	    botSegms[i].y1 += min_y;
	    botSegms[i].y2 += min_y;
	}
    }

    XDrawSegments (display, d, top_gc, topSegms, curTopSeg);
    XDrawSegments (display, d, bottom_gc, botSegms, curBotSeg);

    XtFree((char *) botSegms);
    XtFree((char *) topSegms);
    _XmRegionDestroy (workReg);
    _XmRegionDestroy (scReg2);
    _XmRegionDestroy (scReg1);
}

/*
 * _XmRegionFromImage:
 *   This function was ported from the X server ddx driver code in the
 *   mfb subsection.  It is a port of the function mfbPixmapToRegion()
 *   in mfbclip.c.  The function takes an image and converts the image
 *   into an XmRegion.
 */

XmRegion
_XmRegionFromImage(
    XImage	*image)
{
    register XmRegion	pReg;
    register int	width, x1, x2, y1, crects;
    int			irectPrevStart, irectLineStart;
    register XmRegionBox *prectO, *prectN;
    XmRegionBox		*FirstRect, *rects, *prectLineStart;
    Bool		fInBox, fSame;

    pReg = (XmRegion) XCreateRegion();

    if(!pReg)
	return NULL;
    FirstRect = REGION_BOXPTR(pReg);
    rects = FirstRect;
    width = image->width;
    pReg->extents.x1 = width - 1;
    pReg->extents.x2 = 0;
    irectPrevStart = -1;
    x1 = 0;
    fInBox = False;
    for(y1 = 0; y1 < image->height; y1++)
    {
	irectLineStart = rects - FirstRect;
	/* If the Screen left most bit of the word is set, we're starting in
	 * a box */
	for (x2 = 0; x2 < width; x2++)
	{
	    if (XGetPixel(image, x2, y1))
	    {
		if(!fInBox)
		{
		    x1 = x2;
		    /* start new box */
		    fInBox = TRUE;
		}
	    }
	    else
	    {
		if(fInBox)
		{
		    /* end box */
		    ADDRECT(pReg, rects, FirstRect, x1, y1, x2, y1 + 1);
		    fInBox = FALSE;
		}
	    }
	}
	/* If scanline ended with last bit set, end the box */
	if (fInBox)
	{
	   ADDRECT(pReg, rects, FirstRect, x1, y1, x2, y1 + 1);
	}
	/* if all rectangles on this line have the same x-coords as
	 * those on the previous line, then add 1 to all the previous  y2s and
	 * throw away all the rectangles from this line
	 */
	fSame = FALSE;
	if (irectPrevStart != -1)
	{
	    crects = irectLineStart - irectPrevStart;
	    if(crects == ((rects - FirstRect) - irectLineStart))
	    {
	        prectO = FirstRect + irectPrevStart;
	        prectN = prectLineStart = FirstRect + irectLineStart;
		fSame = TRUE;
	        while (prectO < prectLineStart)
		{
		    if((prectO->x1 != prectN->x1) || (prectO->x2 != prectN->x2))
		    {
			  fSame = FALSE;
			  break;
		    }
		    prectO++;
		    prectN++;
		}
		if (fSame)
		{
		    prectO = FirstRect + irectPrevStart;
		    while (prectO < prectLineStart)
		    {
			prectO->y2 += 1;
			prectO++;
		    }
		    rects -= crects;
		    pReg->numRects -= crects;
		}
	    }
	}
	if (!fSame)
	    irectPrevStart = irectLineStart;
    }
    return(pReg);
}

