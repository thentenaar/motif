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
static char rcsid[] = "$XConsortium: DragUnder.c /main/12 1995/07/14 10:26:51 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/DrawP.h>
#include "XmI.h"
#include "DragCI.h"
#include "DragICCI.h"
#include "DragOverSI.h"
#include "DragUnderI.h"
#include "DropSMgrI.h"
#include "GadgetUtiI.h"
#include "MessagesI.h"
#include "RegionI.h"
#include "ScreenI.h"

#define MESSAGE1	_XmMMsgDragUnder_0000
#define MESSAGE2	_XmMMsgDragUnder_0001

/********    Static Function Declarations    ********/

static XmAnimationSaveData CreateAnimationSaveData(
                        XmDragContext dc,
                        XmAnimationData aData,
                        XmDragProcCallbackStruct *dpcb) ;
static void FreeAnimationData(
                        XmAnimationSaveData aSaveData) ;
static Boolean SaveAll(
                        XmAnimationSaveData aSaveData,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height) ;
static Boolean SaveSegments(
                        XmAnimationSaveData aSaveData,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension height,
                        Dimension *thickness) ;
static void DrawHighlight(
                        XmAnimationSaveData aSaveData) ;
static void DrawShadow(
                        XmAnimationSaveData aSaveData) ;
static void DrawPixmap(
                        XmAnimationSaveData aSaveData) ;
static void AnimateExpose(Widget w, XmAnimationSaveData aSaveData,
                          XEvent *event,
                          Boolean *cont);
static void AnimateEnter(
                        XmDropSiteManagerObject dsm,
                        XmAnimationData aData,
                        XmDragProcCallbackStruct *dpcb) ;
static void AnimateLeave(
                        XmDropSiteManagerObject dsm,
                        XmAnimationData aData,
                        XmDragProcCallbackStruct *dpcb) ;

/********    End Static Function Declarations    ********/


/*****************************************************************************
 *
 *  CreateAnimationSaveData ()
 *
 *  Create and fill an XmAnimationSaveData structure containing the data
 *  needed to animate the dropsite.
 ***************************************************************************/
static XmAnimationSaveData
CreateAnimationSaveData(
        XmDragContext dc,
        XmAnimationData aData,
        XmDragProcCallbackStruct *dpcb ) /* unused */
{
    XmAnimationSaveData		aSaveData;
    XGCValues			v;
    unsigned long		vmask;
    XmDropSiteVisuals		dsv;
    int				ac;
    Arg				al[5];
    Window			junkWin;
    int				junkInt;
    unsigned int		junkUInt;
    unsigned char		activeMode;

    aSaveData = (XmAnimationSaveData)
	XtMalloc (sizeof (XmAnimationSaveDataRec));

    aSaveData->dragOver = aData->dragOver;
    aSaveData->display = XtDisplay (dc);
    aSaveData->xmScreen = XmScreenOfScreen(aData->screen);

    aSaveData->window = aData->window;
    aSaveData->windowX = aData->windowX;
    aSaveData->windowY = aData->windowY;

    if (aSaveData->dragOver) {
        aSaveData->xmScreen = XmScreenOfObject(aSaveData->dragOver);
    }
    else {
        aSaveData->xmScreen = XmScreenOfObject(dc);
    }

    /*
     *  Get the window depth.
     */

    if (!XGetGeometry (aSaveData->display, aSaveData->window,
		       &junkWin, &junkInt, &junkInt,
		       &junkUInt, &junkUInt, &junkUInt,
                       &(aSaveData->windowDepth))) {
	XmeWarning ((Widget) dc, MESSAGE1);
        aSaveData->windowDepth = 0;
    }

    aSaveData->clipRegion = aData->clipRegion;
    aSaveData->dropSiteRegion = aData->dropSiteRegion;

    dsv = XmDropSiteGetActiveVisuals ((Widget) dc);
    aSaveData->background = dsv->background;
    aSaveData->foreground = dsv->foreground;
    aSaveData->topShadowColor = dsv->topShadowColor;
    aSaveData->topShadowPixmap = dsv->topShadowPixmap;
    aSaveData->bottomShadowColor = dsv->bottomShadowColor;
    aSaveData->bottomShadowPixmap = dsv->bottomShadowPixmap;
    aSaveData->shadowThickness = dsv->shadowThickness;
    aSaveData->highlightThickness = dsv->highlightThickness;
    aSaveData->highlightColor = dsv->highlightColor;
    aSaveData->highlightPixmap = dsv->highlightPixmap;
    aSaveData->borderWidth = dsv->borderWidth;
    XtFree ((char *)dsv);

    ac = 0;
    XtSetArg (al[ac], XmNanimationStyle, &(aSaveData->animationStyle)); ac++;
    XtSetArg (al[ac], XmNanimationMask, &(aSaveData->animationMask)); ac++;
    XtSetArg (al[ac], XmNanimationPixmap, &(aSaveData->animationPixmap)); ac++;
    XtSetArg (al[ac], XmNanimationPixmapDepth,
	      &(aSaveData->animationPixmapDepth)); ac++;
    XmDropSiteRetrieve ((Widget) dc, al, ac);

    if (aSaveData->animationStyle == XmDRAG_UNDER_PIXMAP &&
	aSaveData->animationPixmap != None &&
        aSaveData->animationPixmap != XmUNSPECIFIED_PIXMAP &&
        aSaveData->animationPixmapDepth != 1 &&
        aSaveData->animationPixmapDepth != aSaveData->windowDepth) {

	XmeWarning ((Widget) dc, MESSAGE2);
        aSaveData->animationPixmap = XmUNSPECIFIED_PIXMAP;
    }

    /*
     *  Create the draw GC.
     */

    v.foreground = aSaveData->foreground;
    v.background = aSaveData->background;
    v.graphics_exposures = False;
    v.subwindow_mode = IncludeInferiors;
    vmask = GCGraphicsExposures|GCSubwindowMode|GCForeground|GCBackground;
    aSaveData->drawGC =
	XCreateGC (aSaveData->display, aSaveData->window, vmask, &v);

    if (aSaveData -> dragOver != (Widget) NULL) {
      /* Save info on active drag over mode */
      XtSetArg(al[0], XmNdragOverActiveMode, &activeMode);
      XtGetValues((Widget) aSaveData -> dragOver, al, 1);
      aSaveData->activeMode = activeMode;
    } else {
      /* XmCURSOR is as good as any other value for here.  We only
	 check this against XmDRAG_WINDOW */
      aSaveData->activeMode = XmCURSOR;
    }

    /* initialize savedPixmaps list */
    aSaveData->savedPixmaps = NULL;
    aSaveData->numSavedPixmaps = 0;

    return (aSaveData);
}

/*****************************************************************************
 *
 *  FreeAnimationData ()
 *
 *  Free an XmAnimationSaveData structure.
 ***************************************************************************/
static void
FreeAnimationData(
        XmAnimationSaveData aSaveData )
{
    Cardinal	i;

    switch (aSaveData->animationStyle)
    {
        case XmDRAG_UNDER_SHADOW_IN:
        case XmDRAG_UNDER_SHADOW_OUT:
            XFreeGC (aSaveData->display, aSaveData->topShadowGC);
            XFreeGC (aSaveData->display, aSaveData->bottomShadowGC);
            XFreeGC (aSaveData->display, aSaveData->drawGC);
        break;

        case XmDRAG_UNDER_HIGHLIGHT:
            XFreeGC (aSaveData->display, aSaveData->highlightGC);
            XFreeGC (aSaveData->display, aSaveData->drawGC);
        break;

        case XmDRAG_UNDER_PIXMAP:
            XFreeGC (aSaveData->display, aSaveData->drawGC);

        case XmDRAG_UNDER_NONE:
        default:
        break;
    }

    if (aSaveData->numSavedPixmaps) {
        for (i = 0; i < aSaveData->numSavedPixmaps; i++) {
	    _XmFreeScratchPixmap (aSaveData->xmScreen,
				  aSaveData->savedPixmaps[i].pixmap);
        }
        XtFree ((char *)aSaveData->savedPixmaps);
    }

    XtFree ((char *)aSaveData);
}

/*****************************************************************************
 *
 *  SaveAll ()
 *
 *  Save the original contents of a dropsite window that will be overwritten
 *  by dropsite animation into a rectangular backing store.
 ***************************************************************************/
static Boolean
SaveAll(
        XmAnimationSaveData aSaveData,
        Position x,
        Position y,
        Dimension width,
        Dimension height )
{
    DragPixmapData	*pData;

    if (width <= 0 || height <= 0) {
	return (False);
    }

    aSaveData->numSavedPixmaps = 1;
    aSaveData->savedPixmaps = pData =
        (DragPixmapData *) XtMalloc (sizeof(DragPixmapData));
    if (!pData) {
	return (False);
    }

    pData->x = x;
    pData->y = y;
    pData->width = width;
    pData->height = height;
    pData->pixmap =
	_XmAllocScratchPixmap (aSaveData->xmScreen,
			       (Cardinal) aSaveData->windowDepth,
		               pData->width, pData->height);
    XCopyArea (aSaveData->display, aSaveData->window,
    	       pData->pixmap, aSaveData->drawGC,
               pData->x, pData->y,
	       pData->width, pData->height, 0, 0);

    return (True);
}

/*****************************************************************************
 *
 *  SaveSegments ()
 *
 *  Save the original contents of a dropsite window that will be overwritten
 *  by dropsite highlighting or shadowing of indicated thickness.  This will
 *  save 0, 1, or 4 strips into backing store, depending on the dimensions
 *  of the dropsite and the animation thickness.
 ***************************************************************************/
static Boolean
SaveSegments(
        XmAnimationSaveData aSaveData,
        Position x,
        Position y,
        Dimension width,
        Dimension height,
        Dimension *thickness )
{
    DragPixmapData	*pData;
    Boolean		save_all = False;

    if (width <= 0 || height <= 0 || *thickness <= 0) {
        return (False);
    }
    if (*thickness > (width >> 1)) {
        *thickness = (width >> 1);
        save_all = True;
    }
    if (*thickness > (height >> 1)) {
        *thickness = (height >> 1);
        save_all = True;
    }

    if (save_all) {
        return (SaveAll (aSaveData, x, y, width, height));
    }

    aSaveData->numSavedPixmaps = 4;
    aSaveData->savedPixmaps = pData =
	    (DragPixmapData *) XtMalloc (sizeof(DragPixmapData) * 4);
    if (!pData) {
	    return (False);
    }

    pData->x = x;
    pData->y = y;
    pData->width = width;
    pData->height = *thickness;
    pData->pixmap =
	_XmAllocScratchPixmap (aSaveData->xmScreen,
			       (Cardinal) aSaveData->windowDepth,
		               pData->width, pData->height);
    XCopyArea (aSaveData->display, aSaveData->window,
    	       pData->pixmap, aSaveData->drawGC,
               pData->x, pData->y,
	       pData->width, pData->height, 0, 0);

    pData++;
    pData->x = x;
    pData->y = y + *thickness;
    pData->width = *thickness;
    pData->height = height - (*thickness << 1);
    pData->pixmap =
	_XmAllocScratchPixmap (aSaveData->xmScreen,
			       (Cardinal) aSaveData->windowDepth,
		               pData->width, pData->height);
    XCopyArea (aSaveData->display, aSaveData->window,
    	       pData->pixmap, aSaveData->drawGC,
               pData->x, pData->y,
	       pData->width, pData->height, 0, 0);

    pData++;
    pData->x = x;
    pData->y = y + height - *thickness;
    pData->width = width;
    pData->height = *thickness;
    pData->pixmap =
	_XmAllocScratchPixmap (aSaveData->xmScreen,
			       (Cardinal) aSaveData->windowDepth,
		               pData->width, pData->height);
    XCopyArea (aSaveData->display, aSaveData->window,
    	       pData->pixmap, aSaveData->drawGC,
               pData->x, pData->y,
	       pData->width, pData->height, 0, 0);

    pData++;
    pData->x = x + width - *thickness;
    pData->y = y + *thickness;
    pData->width = *thickness;
    pData->height = height - (*thickness << 1);
    pData->pixmap =
	_XmAllocScratchPixmap (aSaveData->xmScreen,
			       (Cardinal) aSaveData->windowDepth,
		               pData->width, pData->height);
    XCopyArea (aSaveData->display, aSaveData->window,
    	       pData->pixmap, aSaveData->drawGC,
               pData->x, pData->y,
	       pData->width, pData->height, 0, 0);

    return (True);
}

/*****************************************************************************
 *
 *  DrawHighlight ()
 *
 *  Draws a highlight around the indicated region.
 ***************************************************************************/
static void
DrawHighlight(
        XmAnimationSaveData aSaveData )
{
    XGCValues		v;
    unsigned long	vmask;
    Dimension		offset;
    Position		x;
    Position		y;
    Dimension		width;
    Dimension		height;
    XRectangle		extents;

    /*
     *  Create the highlightGC
     */

    v.foreground = aSaveData->highlightColor;
    v.background = aSaveData->background;
    v.graphics_exposures = False;
    v.subwindow_mode = IncludeInferiors;
    vmask = GCGraphicsExposures|GCSubwindowMode|GCForeground|GCBackground;

    if (aSaveData->highlightPixmap != None &&
	aSaveData->highlightPixmap != XmUNSPECIFIED_PIXMAP) {
	int depth ;

	XmeGetPixmapData(XtScreen(aSaveData->xmScreen),
			 aSaveData->highlightPixmap,
			 NULL,
			 &depth,
			 NULL, NULL, NULL, NULL, NULL, NULL);

	if (depth == 1) {
	   v.fill_style = FillStippled;
	   v.stipple = aSaveData->highlightPixmap;
	   vmask |= GCStipple | GCFillStyle;
       } else {
	   v.fill_style = FillTiled;
	   v.tile = aSaveData->highlightPixmap;
	   vmask |= GCTile | GCFillStyle;
       }
    }

    aSaveData->highlightGC =
	XCreateGC(aSaveData->display, aSaveData->window, vmask, &v);

    _XmRegionSetGCRegion (aSaveData->display, aSaveData->highlightGC,
			  0, 0, aSaveData->clipRegion);

    /* draw highlight */

    _XmRegionGetExtents (aSaveData->dropSiteRegion, &extents);
    offset = aSaveData->borderWidth;

    if (_XmRegionGetNumRectangles(aSaveData->dropSiteRegion) == 1L) {

        x = extents.x + offset;
        y = extents.y + offset;
        width = extents.width - (offset << 1);
        height = extents.height - (offset << 1);

        if (SaveSegments (aSaveData, x, y, width, height,
                          &aSaveData->highlightThickness)) {
            XmeDrawHighlight (aSaveData->display, aSaveData->window,
				    aSaveData->highlightGC,
				    x, y, width, height,
				    aSaveData->highlightThickness);
        }
    }
    else {
        if (SaveAll (aSaveData, extents.x, extents.y, extents.width,
		     extents.height)) {
            _XmRegionDrawShadow (aSaveData->display, aSaveData->window,
		                 aSaveData->highlightGC, aSaveData->highlightGC,
                                 aSaveData->dropSiteRegion,
                                 offset, aSaveData->highlightThickness,
				 XmSHADOW_OUT);
	}
    }
}

/*****************************************************************************
 *
 *  DrawShadow ()
 *
 *  Draws a 3-D shadow around the indicated region.
 ***************************************************************************/
static void
DrawShadow(
        XmAnimationSaveData aSaveData )
{
    XGCValues		v;
    unsigned long	vmask;
    Dimension		offset;
    Position		x;
    Position		y;
    Dimension		width;
    Dimension		height;
    XRectangle		extents;

    /*
     *  Create the topShadowGC
     */

    v.foreground = aSaveData->topShadowColor;
    v.background = aSaveData->foreground;
    v.graphics_exposures = False;
    v.subwindow_mode = IncludeInferiors;
    vmask = GCGraphicsExposures|GCSubwindowMode|GCForeground|GCBackground;

    if (aSaveData->topShadowPixmap != None &&
        aSaveData->topShadowPixmap != XmUNSPECIFIED_PIXMAP) {
	int depth ;

	XmeGetPixmapData(XtScreen(aSaveData->xmScreen),
			 aSaveData->topShadowPixmap,
			 NULL,
			 &depth,
			 NULL, NULL, NULL, NULL, NULL, NULL);

	if (depth == 1) {
	   v.fill_style = FillStippled;
	   v.stipple = aSaveData->topShadowPixmap;
	   vmask |= GCStipple | GCFillStyle;
       } else {
	   v.fill_style = FillTiled;
	   v.tile = aSaveData->topShadowPixmap;
	   vmask |= GCTile | GCFillStyle;
       }
    }

    aSaveData->topShadowGC =
	XCreateGC(aSaveData->display, aSaveData->window, vmask, &v);

    _XmRegionSetGCRegion (aSaveData->display, aSaveData->topShadowGC,
			  0, 0, aSaveData->clipRegion);

    /*
     *  Create the bottomShadowGC
     */

    v.foreground = aSaveData->bottomShadowColor;
    v.background = aSaveData->foreground;
    v.graphics_exposures = False;
    v.subwindow_mode = IncludeInferiors;
    vmask = GCGraphicsExposures|GCSubwindowMode|GCForeground|GCBackground;

    if (aSaveData->bottomShadowPixmap != None &&
        aSaveData->bottomShadowPixmap != XmUNSPECIFIED_PIXMAP) {
		int depth ;

	XmeGetPixmapData(XtScreen(aSaveData->xmScreen),
			 aSaveData->bottomShadowPixmap,
			 NULL,
			 &depth,
			 NULL, NULL, NULL, NULL, NULL, NULL);

	if (depth == 1) {
	   v.fill_style = FillStippled;
	   v.stipple = aSaveData->bottomShadowPixmap;
	   vmask |= GCStipple | GCFillStyle;
       } else {
	   v.fill_style = FillTiled;
	   v.tile = aSaveData->bottomShadowPixmap;
	   vmask |= GCTile | GCFillStyle;
       }
    }


    aSaveData->bottomShadowGC =
	XCreateGC(aSaveData->display, aSaveData->window, vmask, &v);

    _XmRegionSetGCRegion (aSaveData->display, aSaveData->bottomShadowGC,
			  0, 0, aSaveData->clipRegion);

    /*
     *  Draw the shadows.
     */

    _XmRegionGetExtents (aSaveData->dropSiteRegion, &extents);
    offset = aSaveData->borderWidth + aSaveData->highlightThickness;

    if (_XmRegionGetNumRectangles(aSaveData->dropSiteRegion) == 1L) {

        x = extents.x + offset;
        y = extents.y + offset;
        width = extents.width - (offset << 1);
        height = extents.height - (offset << 1);

        if (SaveSegments (aSaveData, x, y, width, height,
                          &aSaveData->shadowThickness)) {
            XmeDrawShadows (aSaveData->display, aSaveData->window,
		             aSaveData->topShadowGC,
                             aSaveData->bottomShadowGC,
                             x, y, width, height,
		             aSaveData->shadowThickness,
                             (aSaveData->animationStyle ==
				 XmDRAG_UNDER_SHADOW_IN) ?
		                     XmSHADOW_IN : XmSHADOW_OUT);
        }
    }
    else {
        if (SaveAll (aSaveData, extents.x, extents.y,
		     extents.width, extents.height)) {
            _XmRegionDrawShadow (aSaveData->display, aSaveData->window,
		                 aSaveData->topShadowGC,
				 aSaveData->bottomShadowGC,
                                 aSaveData->dropSiteRegion,
		                 offset, aSaveData->shadowThickness,
                                 (aSaveData->animationStyle ==
				     XmDRAG_UNDER_SHADOW_IN) ?
		                         XmSHADOW_IN : XmSHADOW_OUT);
	}
    }
}

/*****************************************************************************
 *
 *  DrawPixmap ()
 *
 *  Copy an animationPixmap, possibly masked, to the dropsite window.
 ***************************************************************************/
static void
DrawPixmap(
        XmAnimationSaveData aSaveData )
{
    Position		x;
    Position		y;
    Dimension		width;
    Dimension		height;
    XRectangle		extents;
    XGCValues		v;
    unsigned long       vmask;
    Pixmap		mask = XmUNSPECIFIED_PIXMAP;
    GC			maskGC = NULL;

    if (aSaveData->animationPixmap == None ||
        aSaveData->animationPixmap == XmUNSPECIFIED_PIXMAP) {
	return;
    }

    /*
     *  Determine the destination location and dimensions -- the
     *  dropsite's bounding box.
     */

    _XmRegionGetExtents (aSaveData->dropSiteRegion, &extents);
    x = extents.x;
    y = extents.y;
    width = extents.width;
    height = extents.height;

    /*
     *  Save the original window contents.
     *  Draw the DrawUnder pixmap into the window.
     *  Assume correct depth -- checked in CreateAnimationSaveData().
     */

    if (SaveAll (aSaveData, x, y, width, height)) {

	if (aSaveData->animationMask != None &&
	    aSaveData->animationMask != XmUNSPECIFIED_PIXMAP) {

	    /*
	     *  AnimationMask specified:  create a composite mask consisting
	     *  of both the clipping region and the animationMask to use for
	     *  copying the animationPixmap into the dropSite.
	     *
	     *    Create a mask and maskGC.
	     *    Set the composite mask to 0's.
	     *    Or the animationMask into it through the ClipRegion.
	     *    Set the drawGC's ClipMask to the composite mask.
	     */

            mask = _XmAllocScratchPixmap (aSaveData->xmScreen, 1,
					  width, height);

	    v.background = 0;
	    v.foreground = 1;
	    v.function = GXclear;
	    v.graphics_exposures = False;
	    v.subwindow_mode = IncludeInferiors;
	    vmask = GCGraphicsExposures|GCSubwindowMode|
	            GCBackground|GCForeground|GCFunction;
	    maskGC = XCreateGC (aSaveData->display, mask, vmask, &v);

	    XFillRectangle (aSaveData->display, mask, maskGC,
		            0, 0, width, height);

	    XSetFunction (aSaveData->display, maskGC, GXor);
	    _XmRegionSetGCRegion (aSaveData->display, maskGC,
				  -x, -y, aSaveData->clipRegion);
	    XCopyArea (aSaveData->display,
		       aSaveData->animationMask,
    	               mask, maskGC,
                       0, 0, width, height, 0, 0);

	    XSetClipOrigin (aSaveData->display, aSaveData->drawGC, x, y);
	    XSetClipMask (aSaveData->display, aSaveData->drawGC, mask);

	    XFreeGC (aSaveData->display, maskGC);
	}
	else {
	    _XmRegionSetGCRegion (aSaveData->display, aSaveData->drawGC,
				  0, 0, aSaveData->clipRegion);
	}

	/*
	 *  Copy the animationPixmap to the window.
	 *  If the animationPixmapDepth is 1 we treat the animationPixmap
	 *  as a bitmap and use XCopyPlane.  For 1-deep dropsite windows,
	 *  this may not be the same as treating the animationPixmap as a
	 *  1-deep pixmap and using XCopyArea.
	 */

	if (aSaveData->animationPixmapDepth == 1) {
	    XCopyPlane (aSaveData->display,
			aSaveData->animationPixmap,
    	                aSaveData->window, aSaveData->drawGC,
                        0, 0, width, height, x, y, 1L);
	}
	else {
	    XCopyArea (aSaveData->display,
		       aSaveData->animationPixmap,
    	               aSaveData->window, aSaveData->drawGC,
                       0, 0, width, height, x, y);
	}
	if (mask != XmUNSPECIFIED_PIXMAP) {
	    _XmFreeScratchPixmap (aSaveData->xmScreen, mask);
	}
    }
}

/*****************************************************************************
 *
 *  AnimateExpose ()
 *
 ***************************************************************************/
static void
AnimateExpose(Widget w,		/* unused */
	      XmAnimationSaveData aSaveData,
	      XEvent *event,	/* unused */
	      Boolean *cont)	/* unused */
{
  /*
   *  If dragging a pixmap or window, hide it while drawing the
   *  animation.
   */

  if (aSaveData->dragOver && aSaveData->activeMode != XmDRAG_WINDOW) {
    _XmDragOverHide (aSaveData->dragOver,
		     aSaveData->windowX, aSaveData->windowY,
		     aSaveData->clipRegion);
  }

  /* Draw the visuals. */
  switch(aSaveData->animationStyle) {
  default:
  case XmDRAG_UNDER_HIGHLIGHT:
    DrawHighlight (aSaveData);
    break;

  case XmDRAG_UNDER_SHADOW_IN:
  case XmDRAG_UNDER_SHADOW_OUT:
    DrawShadow (aSaveData);
    break;

  case XmDRAG_UNDER_PIXMAP:
    DrawPixmap (aSaveData);
    break;

  case XmDRAG_UNDER_NONE:
    break;
  }

  /*
   *  If dragging a pixmap or window, show it.
   */

  if (aSaveData->dragOver && aSaveData->activeMode != XmDRAG_WINDOW) {
    _XmDragOverShow (aSaveData->dragOver,
		     aSaveData->windowX, aSaveData->windowY,
		     aSaveData->clipRegion);
  }
}

/*****************************************************************************
 *
 *  AnimateEnter ()
 *
 ***************************************************************************/
static void
AnimateEnter(
        XmDropSiteManagerObject dsm, /* unused */
        XmAnimationData aData,
        XmDragProcCallbackStruct *dpcb )
{
    Widget dc = dpcb->dragContext;
    XmAnimationSaveData	aSaveData;
    Widget dswidget = GetDSWidget((XmDSInfo) (dsm->dropManager.curInfo));
    Boolean dummy;

    /*
     *  Create and fill an XmAnimationSaveData structure containing the
     *  data needed to animate the dropsite.  Save it for AnimateLeave().
     */

    aSaveData = CreateAnimationSaveData ((XmDragContext) dc, aData, dpcb);
    *((XtPointer *) aData->saveAddr) = (XtPointer) aSaveData;

    /* Show the visual */
    AnimateExpose(dswidget, aSaveData, NULL, &dummy);

    /* Save the dragunder widget */
    aSaveData->dragUnder = dswidget;

    if (aSaveData->activeMode == XmDRAG_WINDOW) {
      /* Install the event handler to redo visual on Exposure */
      Widget hwidget = dswidget;
      if (XmIsGadget(hwidget))
	hwidget = XtParent(hwidget);
      XtInsertEventHandler(hwidget, ExposureMask, False,
			   (XtEventHandler) AnimateExpose,
			   (XtPointer) aSaveData, XtListTail);
    }
}

/*****************************************************************************
 *
 *  AnimateLeave ()
 *
 ***************************************************************************/
static void
AnimateLeave(
        XmDropSiteManagerObject dsm, /* unused */
        XmAnimationData aData,
        XmDragProcCallbackStruct *dpcb ) /* unused */
{
    Cardinal i;
    DragPixmapData *pData;
    XmAnimationSaveData aSaveData =
	(XmAnimationSaveData) *((XtPointer *) aData->saveAddr);

	(void)dpcb;
	if (!aSaveData)
	  return;

	/**
	 * Ignore this event if we have stale dragOver data. Likely, the
	 * DragOver shell has already been destroyed.
	 */
	if (aData->dragOver && aData->dragOver != aSaveData->dragOver) {
	  FreeAnimationData(aSaveData);
	  dsm->dropManager.dragUnderData = NULL;
	  return;
	}

	/* Move to here to avoid crashes when aSaveData already zeroed */
	if (aSaveData->activeMode == XmDRAG_WINDOW) {
	  /* Remove the event handler to redo visual on Exposure */
	  Widget hwidget = aSaveData -> dragUnder;
	  if (XmIsGadget(hwidget))
	    hwidget = XtParent(hwidget);
	  XtRemoveEventHandler(hwidget, ExposureMask, False,
			       (XtEventHandler) AnimateExpose,
			       (XtPointer) aSaveData);
	}

	/*
	 *  If dragging a pixmap or window, hide it while erasing the
	 *  animation.
	 */

        if (aSaveData->dragOver) {
	    _XmDragOverHide (aSaveData->dragOver,
    			     aSaveData->windowX, aSaveData->windowY,
			     aSaveData->clipRegion);
	}

	/*
	 *  Copy any saved segments back into the window.
	 *  Be sure GCRegion is set properly here.
	 */

        _XmRegionSetGCRegion (aSaveData->display, aSaveData->drawGC,
			      0, 0, aSaveData->clipRegion);
        for (pData = aSaveData->savedPixmaps, i = aSaveData->numSavedPixmaps;
	     i; pData++, i--) {
            XCopyArea (aSaveData->display,
		       pData->pixmap,
		       aSaveData->window,
		       aSaveData->drawGC,
                       0, 0,
		       pData->width,
		       pData->height,
		       pData->x,
		       pData->y);
        }

	/*
	 *  If dragging a pixmap or window, show it.
         *  Free the XmAnimationSaveData structure created in AnimateEnter().
	 */

        if (aSaveData->dragOver) {
	    _XmDragOverShow (aSaveData->dragOver,
    			     aSaveData->windowX, aSaveData->windowY,
			     aSaveData->clipRegion);
	}

	FreeAnimationData(aSaveData);
	*((XtPointer *)aData->saveAddr) = NULL;
}

/*****************************************************************************
 *
 *  _XmDragUnderAnimation ()
 *
 ***************************************************************************/
void
_XmDragUnderAnimation(
    Widget w,
    XtPointer clientData,
    XtPointer callData )
{
    XmDropSiteManagerObject dsm = (XmDropSiteManagerObject) w;
    XmDragProcCallbackStruct *dpcb = (XmDragProcCallbackStruct *) callData;
    XmAnimationData aData = (XmAnimationData) clientData;

    switch(dpcb->reason)
    {
        case XmCR_DROP_SITE_LEAVE_MESSAGE:
            AnimateLeave(dsm, aData, dpcb);
        break;

        case XmCR_DROP_SITE_ENTER_MESSAGE:
            AnimateEnter(dsm, aData, dpcb);
        break;

        default:
        break;
    }
}

