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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: GeoUtils.c /main/13 1996/08/15 17:11:25 pascale $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "XmI.h"
#include "GeoUtilsI.h"
#include "GMUtilsI.h"

/********    Static Function Declarations    ********/

static XtGeometryResult QueryAnyPolicy(
                        XmGeoMatrix geoSpec,
                        XtWidgetGeometry *parentRequestRtn) ;
static XtGeometryResult QueryGrowPolicy(
                        XmGeoMatrix geoSpec,
                        XtWidgetGeometry *parentRequestRtn) ;
static XtGeometryResult QueryNonePolicy(
                        XmGeoMatrix geoSpec,
                        XtWidgetGeometry *parentRequestRtn) ;
static Dimension _XmGeoStretchVertical(
                        XmGeoMatrix geoSpec,
                        Dimension actualH,
                        Dimension desiredH) ;
static Dimension _XmGeoFillVertical(
                        XmGeoMatrix geoSpec,
                        Dimension actualH,
                        Dimension desiredH) ;
static void _XmGeoCalcFill(
                        Dimension fillSpace,
                        Dimension margin,
                        unsigned int numBoxes,
                        Dimension endSpec,
                        Dimension betweenSpec,
                        Dimension *pEndSpace,
                        Dimension *pBetweenSpace) ;
static int boxWidthCompare(
                        XmConst void *boxPtr1,
                        XmConst void *boxPtr2) ;
static void FitBoxesAveraging(
                        XmKidGeometry rowPtr,
                        unsigned int numBoxes,
                        Dimension boxWidth,
                        int amtOffset) ;
static void FitBoxesProportional(
                        XmKidGeometry rowPtr,
                        unsigned int numBoxes,
                        Dimension boxWidth,
                        int amtOffset) ;
static void SegmentFill(
                        XmKidGeometry rowBoxes,
                        unsigned int numBoxes,
                        XmGeoRowLayout layoutPtr,
                        Position x,
                        Dimension width,
                        Dimension marginW,
                        Position endX,
                        Position maxX,
                        Dimension endSpace,
                        Dimension betweenSpace) ;
static Position _XmGeoLayoutWrap(
                        XmKidGeometry rowPtr,
                        XmGeoRowLayout layoutPtr,
                        Position x,
                        Position y,
                        Dimension endSpace,
                        Dimension betweenSpace,
                        Position maxX,
                        Dimension width,
                        Dimension marginW) ;
static Position _XmGeoLayoutSimple(
                        XmKidGeometry rowPtr,
                        XmGeoRowLayout layoutPtr,
                        Position x,
                        Position y,
                        Position maxX,
                        Dimension endSpace,
                        Dimension betweenSpace) ;
static Position _XmGeoArrangeList(
                        XmKidGeometry rowBoxes,
                        XmGeoRowLayout layoutPtr,
                        Position x,
                        Position y,
                        Dimension width,
                        Dimension marginW,
                        Dimension marginH) ;

/********    End Static Function Declarations    ********/

/****************************************************************/


#ifdef DEBUG_GEOUTILS

void PrintBox( char * hdr, XmKidGeometry box) ;
void PrintList( char * hdr, XmKidGeometry listPtr) ;
void PrintMatrix( char * hdr, XmGeoMatrix spec) ;

#endif /* DEBUG_GEOUTILS */


/****************************************************************/
XtGeometryResult
_XmHandleQueryGeometry(
        Widget widget,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired,
        unsigned char resize_policy,
        XmGeoCreateProc createMatrix)
{
    Dimension       width = 0 ;
    Dimension       height = 0 ;
    XmGeoMatrix     geoSpec ;

    /* first determine what is the desired size, using the resize_policy. */
    if (resize_policy == XmRESIZE_NONE) {
	desired->width = XtWidth(widget) ;
	desired->height = XtHeight(widget) ;
    } else {
	if (GMode( intended) & CWWidth) width = intended->width;
	if (GMode( intended) & CWHeight) height = intended->height;

	geoSpec = (*createMatrix)( widget, NULL, NULL) ;
	_XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;
	_XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0,
			   &width, &height) ;
	_XmGeoMatrixFree( geoSpec) ;
	if ((resize_policy == XmRESIZE_GROW) &&
	    ((width < XtWidth(widget)) ||
	     (height < XtHeight(widget)))) {
	    desired->width = XtWidth(widget) ;
	    desired->height = XtHeight(widget) ;
	} else {
	    desired->width = width ;
	    desired->height = height ;
	}
    }

    /* deal with user initial size setting */
    if (!XtIsRealized(widget))  {
	if (XtWidth(widget) != 0) desired->width = XtWidth(widget) ;
	if (XtHeight(widget) != 0) desired->height = XtHeight(widget) ;
    }

    return XmeReplyToQueryGeometry(widget, intended, desired) ;
}


/****************************************************************/
XtGeometryResult
_XmHandleGeometryManager(
        Widget wid,
        Widget instigator,
        XtWidgetGeometry *desired,
        XtWidgetGeometry *allowed,
        unsigned char policy,
        XmGeoMatrix *cachePtr,
        XmGeoCreateProc createMatrix)
{
            XmGeoMatrix     geoSpec ;
            XtWidgetGeometry parentRequest ;
            XtGeometryResult queryResult ;
            XtGeometryResult result ;
/****************/

    if(    !cachePtr    )
    {
        /* Almost replies are not entertained unless caching is supported.
        */
        allowed = NULL ;
        }
    else
    {   geoSpec = *cachePtr ;

        if(    geoSpec    )
        {
            if(    (geoSpec->composite == wid)
                && (geoSpec->instigator == instigator)
                && _XmGeometryEqual( instigator, geoSpec->in_layout, desired)    )
            {
                /* This is a successive geometry request which matches the
                *   cached geometry record.
                */
                if(    GMode( desired) & XtCWQueryOnly    )
                {   return( XtGeometryYes) ;
                    }
                else
                {   /* If we get here, we should have already verified that
                    *   the current layout is acceptable to the parent, so
                    *   we will ignore the result of the request.
                    */
                    if(    geoSpec->parent_request.request_mode    )
                    {
                        geoSpec->parent_request.request_mode &= ~XtCWQueryOnly ;

                        XtMakeGeometryRequest( wid, &geoSpec->parent_request,
                                                                            NULL) ;
                        }
                    _XmGeoMatrixSet( geoSpec) ;

                    _XmGeoMatrixFree( geoSpec) ;
                    *cachePtr = NULL ;

                    return( XtGeometryYes) ;
                    }
                }
            else
            {   /* Cached geometry is different than current request, so clear
                *   existing cache record and allow request to be processed.
                */
                _XmGeoMatrixFree( geoSpec) ;
                *cachePtr = NULL ;
                }
            }
        }
    /*	Get box list and arrange boxes according to policy.
    */
    geoSpec = (*createMatrix)( wid, instigator, desired) ;

    if(    geoSpec->no_geo_request
        && (*geoSpec->no_geo_request)( geoSpec)    )
    {
        _XmGeoMatrixFree( geoSpec) ;
        return( XtGeometryNo) ;
        }

    /* The following Query routines only respond with XtGeometryYes or
    *    XtGeometryNo.  All requests made to the parent are strictly
    *    queries.
    *  A return value (from these routines!) of XtGeometryNo means that
    *    the composite widget would need to change size in order to
    *    entertain the child's request, and that the parent said "no"
    *    to the request.  A XtGeometryNo leaves no alternatives to the
    *    child's geometry request.
    *  A return value of XtGeometryYes means that either the composite
    *    widget does not need to change size to entertain the child's
    *    request, or that negotiation with the parent yielded a viable
    *    geometry layout.  If the composite widget does not need to
    *    change size, then request_mode field of the returned geometry
    *    structure will contain zero.  Otherwise, the returned geometry
    *    structure will contain a request which is guaranteed to be
    *    accepted by a subsequent request to the parent.
    *  A return value of XtGeometryYes always loads the return geometry
    *    structure with valid data.
    */
    switch(    policy    )
    {
        case XmRESIZE_GROW:
        {   queryResult = QueryGrowPolicy( geoSpec, &parentRequest) ;
	    break;
            }
	case XmRESIZE_NONE:
        {   queryResult = QueryNonePolicy( geoSpec, &parentRequest) ;
            break ;
            }
	case XmRESIZE_ANY:
        default:
        {   queryResult = QueryAnyPolicy( geoSpec, &parentRequest) ;
            break ;
            }
        }
    result = XtGeometryNo ; /* Setup default response. */

    /* If parent replies XtGeometryYes, then build appropriate reply for
    *   instigator.  Otherwise, it is not reasonable to try to find a child
    *   size which would result in an acceptable overall size, so just say no.
    */
    if(    queryResult == XtGeometryYes    )
    {
        if(    _XmGeoReplyYes( instigator, desired, geoSpec->in_layout)    )
        {
            /* Reply Yes since desired geometry is same as the
            *   instigator geometry in this layout.
            */
            if(    GMode( desired) & XtCWQueryOnly    )
            {
                geoSpec->parent_request = parentRequest ;
                result = XtGeometryYes ;
                }
            else
            {   /* Don't need almost reply and this is not a query, so do it!
                */
                if(    parentRequest.request_mode    )
                {
                    /* The geometry request in parentRequest has already
                    *   been tested by a query to the parent, so should
                    *   always be honored.
                    */
                    parentRequest.request_mode &= ~XtCWQueryOnly ;

                    XtMakeGeometryRequest( wid, &parentRequest, NULL) ;
                    }
                _XmGeoMatrixSet( geoSpec) ;

                result = XtGeometryYes ;
                }
            }
        else
        {   /* If allowed and not an exception then reply Almost, since
            *   desired geometry is different than geometry in this layout.
            */
            if(    allowed
                && (    !geoSpec->almost_except
                     || !(*(geoSpec->almost_except))(geoSpec))    )
            {
                geoSpec->parent_request = parentRequest ;
                result = XtGeometryAlmost ;
                }
            }
        }
    switch(    result    )
    {
        case XtGeometryAlmost:
        {
            /* Cache "almost" replies.  Variables cachePtr and allowed are
            *   guaranteed to be non-null since almost replies are prevented
            *   if either is null.
            */
            if(geoSpec->in_layout) {
		*cachePtr = geoSpec ;
		*allowed = *(geoSpec->in_layout) ;
            } else /* For fixing OSF CR 5956 */ {
		allowed = NULL ;
		*cachePtr = NULL ;
		result = XtGeometryNo ;
	    }
            break ;
	}
        case XtGeometryYes:
        {
            /* This must be a query-only request, or the response would
            *   be XtGeometryYes.  Cache this reply if caching is
            *   supported.  Otherwise, drop through and free geoSpec.
            */
            if(    cachePtr    )
            {   *cachePtr = geoSpec ;
                break ;
                }
            }
        default:
        {   _XmGeoMatrixFree( geoSpec) ;
            break ;
            }
        }
    return( result) ;
    }

/****************************************************************
 * Handle geometry request for XmRESIZE_ANY resize policy.
 * Accept request allowed by parent.
 * Reject request to change both width and height if both
 *   are disallowed by parent, but return almost if one is
 *   allowed.
 ****************/
static XtGeometryResult
QueryAnyPolicy(
        XmGeoMatrix geoSpec,
        XtWidgetGeometry *parentRequestRtn )
{
            Widget          wid ;
            Dimension       layoutW ;
            Dimension       layoutH ;
            XtWidgetGeometry parentResponse ;
            XtGeometryResult queryResult ;
            Dimension       almostW ;
            Dimension       almostH ;
/****************/

    wid = geoSpec->composite ;

    _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;

    layoutW = 0 ;
    layoutH = 0 ;
    _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &layoutW,
                                                                    &layoutH) ;
    /*	Load request.
    */
    parentRequestRtn->request_mode = CWWidth | CWHeight ;
    parentRequestRtn->width = layoutW ;
    parentRequestRtn->height = layoutH ;

    /*	Query parent only if necessary.
    */
    if(    (layoutW == XtWidth( wid))
        && (layoutH == XtHeight( wid))    )
    {
        parentRequestRtn->request_mode = 0 ;
        queryResult = XtGeometryYes ;
        }
    else
    {   parentRequestRtn->request_mode |= XtCWQueryOnly ;

        queryResult = XtMakeGeometryRequest( wid, parentRequestRtn,
                                                             &parentResponse) ;
        if(    queryResult == XtGeometryAlmost    )
        {
            if(    (parentResponse.request_mode & (CWWidth | CWHeight))
                                                   != (CWWidth | CWHeight)    )
            {   queryResult = XtGeometryNo ;
                }
            else
            {   /* The protocol guarantees an XtGeometryYes reply for
                *   for an immediately subsequent request which is
                *   identical to the XtGeometryAlmost reply.
                */
                *parentRequestRtn = parentResponse ;
                queryResult = XtGeometryYes ;

                almostW = parentResponse.width ;
                almostH = parentResponse.height ;

                if(    (almostW != layoutW)  ||  (almostH != layoutH)    )
                {
                    /* Response to geometry request was different than
                    *   requested geometry in fields that we care about.
                    *   So, try a new arrangement with the area being
                    *   offered by the parent.
                    */
                    _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;
                    layoutW = almostW ;
                    layoutH = almostH ;
                    _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0,
                                                          &layoutW, &layoutH) ;
                    if(    (almostW != layoutW)  ||  (almostH != layoutH)    )
                    {
                        /* The children cannot be laid-out in the area offered
                        *   by the parent, so parent result is No.
                        */
                        queryResult = XtGeometryNo ;
                        }
                    }
                }
            }
        }
    return( queryResult) ;
    }

/****************************************************************
 * Handle geometry request for XmRESIZE_GROW resize policy.
 * Accept request which would increase or maintain current size.
 * Reject request which would decrease both preferred width
 *   and preferred height, but return almost if only one
 *   would decrease.
 ****************/
static XtGeometryResult
QueryGrowPolicy(
        XmGeoMatrix geoSpec,
        XtWidgetGeometry *parentRequestRtn )
{
            Widget          wid ;
            Dimension       layoutW ;
            Dimension       layoutH ;
            XtWidgetGeometry parentResponse ;
            XtGeometryResult queryResult ;
            Dimension       almostW ;
            Dimension       almostH ;
/****************/

    wid = geoSpec->composite ;

    _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;

    if(    geoSpec->instig_request.request_mode & CWWidth    )
    {   layoutW = 0 ;               /* Let the layout routine choose a width.*/
        }
    else
    {   layoutW = XtWidth( wid) ;   /* All changes will be reflected in      */
        }                           /*   vertical dimension.                 */
    layoutH = XtHeight( wid) ;  /* Layout routine will grow vert., if needed.*/

    _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &layoutW,
                                                                    &layoutH) ;
    if(    layoutW < XtWidth( wid)    )
    {
        /* Try again, this time passing the width to _XmGeoArrangeBoxes.
        */
        _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;

        layoutW = XtWidth( wid) ;
        layoutH = XtHeight( wid) ;
        _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &layoutW,
                                                                    &layoutH) ;
        }
    /*	Load request.
    */
    parentRequestRtn->request_mode = CWWidth | CWHeight ;
    parentRequestRtn->width = layoutW ;
    parentRequestRtn->height = layoutH ;

    /*	Query parent only if necessary.
    */
    if(    (layoutW == XtWidth( wid))
        && (layoutH == XtHeight( wid))    )
    {
        parentRequestRtn->request_mode = 0 ;
        queryResult = XtGeometryYes ;
        }
    else
    {   parentRequestRtn->request_mode |= XtCWQueryOnly ;

        queryResult = XtMakeGeometryRequest( wid, parentRequestRtn,
                                                             &parentResponse) ;
        if(    queryResult == XtGeometryAlmost    )
        {
            if(    (parentResponse.request_mode & (CWWidth | CWHeight))
                                                   != (CWWidth | CWHeight)    )
            {   queryResult = XtGeometryNo ;
                }
            else
            {   /* The protocol guarantees an XtGeometryYes reply for
                *   for an immediately subsequent request which is
                *   identical to the XtGeometryAlmost reply.
                */
                *parentRequestRtn = parentResponse ;
                queryResult = XtGeometryYes ;

                almostW = parentResponse.width ;
                almostH = parentResponse.height ;

                if(    (almostW < XtWidth( wid))
                    || (almostH < XtHeight( wid))    )
                {
                    queryResult = XtGeometryNo ;
                    }
                else
                {   if(    (almostW != layoutW)  ||  (almostH != layoutH)    )
                    {
                        /* Response to geometry request was different than
                        *   requested geometry in fields that we care about.
                        *   So, try a new arrangement with the area being
                        *   offered by the parent.
                        */
                        _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;
                        layoutW = almostW ;
                        layoutH = almostH ;
                        _XmGeoArrangeBoxes( geoSpec, (Position) 0,
                                            (Position) 0, &layoutW, &layoutH) ;
                        if(    (almostW != layoutW)  ||  (almostH != layoutH) )
                        {
                            /* The children cannot be laid-out in the area
                            *   offered by the parent, so parent result is No.
                            */
                            queryResult = XtGeometryNo ;
                            }
                        }
                    }
                }
            }
        }
    return( queryResult) ;
    }
/****************************************************************
 * Handle geometry request for XmRESIZE_NONE resize policy.
 * Accept request which would not change preferred size and
 *   allowed by parent.
 * Reject request which would change both preferred width
 *   and preferred height, but return almost if only one
 *   would change and parent allows the other.
 ****************/
static XtGeometryResult
QueryNonePolicy(
        XmGeoMatrix geoSpec,
        XtWidgetGeometry *parentRequestRtn )
{
            Widget          wid ;
            Dimension       layoutW ;
            Dimension       layoutH ;
/****************/

    wid = geoSpec->composite ;

    _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;

    layoutW = XtWidth( wid) ;
    layoutH = XtHeight( wid) ;
    _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0,
                                                          &layoutW, &layoutH) ;
    parentRequestRtn->request_mode = 0 ;

    if(    (layoutW != XtWidth( wid))  ||  (layoutH != XtHeight( wid))    )
    {   return( XtGeometryNo) ;
        }

    return( XtGeometryYes) ;
    }

/****************************************************************/
void
_XmHandleSizeUpdate(
        Widget wid,
        unsigned char policy,
        XmGeoCreateProc createMatrix)
{
            XmGeoMatrix     geoSpec ;
            Dimension       w ;
            Dimension       h ;
            Dimension       r_w ;
            Dimension       r_h ;
            XtGeometryResult parentResult = XtGeometryNo ;
/****************/

    geoSpec = (*createMatrix)( wid, NULL, NULL) ;

    _XmGeoMatrixGet( geoSpec, XmGET_PREFERRED_SIZE) ;

    switch(    policy    )
    {
        case XmRESIZE_NONE:
        {
            w = XtWidth( wid) ;
            h = XtHeight( wid) ;
            _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &w, &h) ;

            break ;
            }
        case XmRESIZE_GROW:
        {
            w = 0 ;
            h = XtHeight( wid) ;
            _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &w, &h) ;

            if(    w < XtWidth( wid)    )
            {   w = XtWidth( wid) ;
                h = XtHeight( wid) ;
                _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0,
                                                                      &w, &h) ;
                }
            break ;
            }
        case XmRESIZE_ANY:
        default:
        {
            w = 0 ;
            h = 0 ;
            _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0, &w, &h) ;

            break ;
            }
        }

    if(    ((w == XtWidth( wid))  &&  (h == XtHeight( wid)))    )
    {   parentResult = XtGeometryYes ;
        }
    else
    {   if(    policy != XmRESIZE_NONE    )
        {
            parentResult = XtMakeResizeRequest( wid, w, h, &r_w, &r_h) ;

            if(    parentResult == XtGeometryAlmost    )
            {
                if(    (policy == XmRESIZE_GROW)
                    && (   (r_w < XtWidth( wid))
                        || (r_h < XtHeight( wid)))    )
                {
                    parentResult = XtGeometryNo ;
                    }
                else
                {   w = r_w ;
                    h = r_h ;
                    _XmGeoArrangeBoxes( geoSpec, (Position) 0, (Position) 0,
                                                                      &w, &h) ;
                    if(    (w == r_w)  &&  (h == r_h)    )
                    {   XtMakeResizeRequest( wid, w, h, NULL, NULL) ;
                        }
                    else
                    {   parentResult = XtGeometryNo ;
                        }
                    }
                }
            }
        }
    if(    parentResult != XtGeometryNo    )
    {   _XmGeoMatrixSet( geoSpec) ;
        }
    _XmGeoMatrixFree( geoSpec) ;
    return ;
    }

/****************************************************************
 * This routine allocates and initializes the data structure used
 *   to describe a matrix of geometry boxes.  Supplemental initialization
 *   may be required for some of the fields of the data structure, if
 *   the user uses these fields in its supplied co-routines.
 * Rows of the GeoMatrix are lists of kid boxes which are terminated with
 *   a NULL in the kid widget field of the box structure.  This routine
 *   automatically allocates extra boxes to use to mark the end of each
 *   row list.
 * This routine initializes all fields to NULL.
 * The pointer returned by this routine should be freed by the user
 *   using the _XmGeoMatrixFree() routine.
 ****************/
XmGeoMatrix
_XmGeoMatrixAlloc(
        unsigned int numRows,       /* Number of rows of widgets to layout.*/
        unsigned int numBoxes,      /* Total number of widgets of matrix.*/
        unsigned int extSize )      /* Extension record size (bytes).*/
{
            XmGeoMatrix     geoSpecPtr ;
            unsigned int    matrixRecSize ;
            unsigned int    layoutRecSize ;
            unsigned int    kidGeoRecSize ;
            unsigned int    layoutSize ;
            unsigned int    boxesSize ;
            unsigned int    totalSize ;
/****************/

    /* Get sizes of the various components of the GeoMatrix.  Round up to
    *   prevent alignment problems.
    */
    matrixRecSize = sizeof( XmGeoMatrixRec) ;
    if(    matrixRecSize & 0x03    )
    {   matrixRecSize = (matrixRecSize + 4) & ~((unsigned int) 0x03) ;
        }
    layoutRecSize = sizeof( XmGeoRowLayoutRec) ;
    if(    layoutRecSize & 0x03    )
    {   layoutRecSize = (layoutRecSize + 4) & ~((unsigned int) 0x03) ;
        }
    kidGeoRecSize = sizeof( XmKidGeometryRec) ;
    if(    kidGeoRecSize & 0x03    )
    {   kidGeoRecSize = (kidGeoRecSize + 4) & ~((unsigned int) 0x03) ;
        }
    layoutSize = (numRows + 1) * layoutRecSize ;
    /* Extra boxes are used to mark the end of each row.
    */
    boxesSize = (numBoxes + numRows) * kidGeoRecSize ;
    totalSize = matrixRecSize + layoutSize + boxesSize + extSize ;

    geoSpecPtr = (XmGeoMatrix) XtCalloc( 1, totalSize) ;   /* Must be zeroed.*/

    /* Set locations of arrays of row layout, box, and extension records.
    */
    geoSpecPtr->layouts = (XmGeoMajorLayout) (((char *) geoSpecPtr)
                                                             + matrixRecSize) ;
    geoSpecPtr->boxes = (XmKidGeometry) (((char *) geoSpecPtr)
                                                + matrixRecSize + layoutSize) ;
    if(    extSize    )
    {   geoSpecPtr->extension = (XtPointer) (((char *) geoSpecPtr)
                                    + matrixRecSize + layoutSize + boxesSize) ;
        }
    return( geoSpecPtr) ;
    }
void
_XmGeoMatrixFree(
	XmGeoMatrix geo_spec)
{
  if(    geo_spec->ext_destructor    )
    {
      (*(geo_spec->ext_destructor))( geo_spec->extension) ;
    }
  XtFree( (char *) geo_spec) ;
}

/****************************************************************
 * If the widget specified by the "kidWid" parameter is non-NULL and is managed,
 *   its value is copied into the appropriate field of the kid geometry
 *   structure provided by the "geo" parameter and TRUE is returned.
 * Otherwise, nothing is done and FALSE is returned.
 ****************/
Boolean
_XmGeoSetupKid(
        XmKidGeometry geo,          /* Must be non-NULL.*/
        Widget kidWid )
{
/****************/
    if(    !kidWid  ||  !XtIsManaged( kidWid)    )
    {   return( FALSE) ;
        }
    /* The widget ID will be used for subsequent "get" operation.
    */
    geo->kid = (Widget) kidWid;

    /* Return TRUE so the user knows that the box record was filled with
    *   a widget ID and can then increment the box pointer for the next
    *   managed widget to be setup.
    */
    return( TRUE) ;
    }

/****************************************************************
 * This routine goes through the widget matrix and retrieves the appropriate
 *   values for the KidGeometry boxes.  Field values of the boxes may be
 *   altered according to requirements specified in each row structure of
 *   the geoSpec.
 * If the widget id within the matrix matches the instigator field of the
 *   geoSpec, then the value for the box is taken from the request field of
 *   the geoSpec or the widget itself as appropriate.
 ****************/
void
_XmGeoMatrixGet(
        XmGeoMatrix geoSpec,
        int geoType )               /* XmGET_PREFERRED_SIZE or */
{
    register XmKidGeometry   boxPtr ;
            XmKidGeometry   rowPtr ;
            XmGeoRowLayout  layoutPtr ;
            XtWidgetGeometry * request ;
            Widget          instigator ;
/****************/

    request = &geoSpec->instig_request ;
    instigator = geoSpec->instigator ;
    rowPtr = geoSpec->boxes ;
    layoutPtr = &(geoSpec->layouts->row) ;

    while(    !(layoutPtr->end)    )
    {
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {
            _XmGeoLoadValues( boxPtr->kid, geoType, instigator, request,
                                                              &(boxPtr->box)) ;
            if(    boxPtr->kid == instigator    )
            {   geoSpec->in_layout = &(boxPtr->box) ;
                }
            ++boxPtr ;
            }
        if(    layoutPtr->fix_up    )
        {
            (*(layoutPtr->fix_up))( geoSpec, geoType,
				        (XmGeoMajorLayout) layoutPtr, rowPtr) ;
            }
        rowPtr = boxPtr + 1 ;   /* Skip over NULL box marking the end of row.*/
        ++layoutPtr ;           /* Go to next row layout record.*/
        }
#ifdef DEBUG_GEOUTILS
    PrintMatrix( "(get) ", geoSpec) ;
#endif
    return ;
    }

/****************************************************************
 * The XtConfigureWidget routine is called on all widgets of the geoSpec
 *   matrix as needed (when the geometry values of the box have changed).
 *   If a widget ID matches that of the instigator field of the geoSpec,
 *   then that widget is not configured.
 * Any layout "fixup" routines which are specified in the row structure
 *   of the geoSpec are called before and after the call
 *   to XmeConfigureObject, with appropriate parameter values.
 ****************/
void
_XmGeoMatrixSet(
        XmGeoMatrix geoSpec )
{
    register XmKidGeometry   rowPtr ;
    register XmGeoRowLayout  layoutPtr ;
            Boolean         fixUps = FALSE ;
/****************/

#ifdef DEBUG_GEOUTILS
    PrintMatrix( "(set) ", geoSpec) ;
#endif

    /* Give the user a chance to avoid setting the widgets to box values.
    */
    if(    !geoSpec->set_except  ||  !(*geoSpec->set_except)( geoSpec)    )
    {
        /* Give the user a chance to modify box sizes before setting
        *   the widget to the values defined in the box record.
        */
        layoutPtr = &(geoSpec->layouts->row) ;
        rowPtr = geoSpec->boxes ;
        while(    !(layoutPtr->end)    )
        {
            if(    layoutPtr->fix_up    )
            {
                /* Call the user's routine which may modify boxes of this row.
                */
                (*(layoutPtr->fix_up))( geoSpec, XmGEO_PRE_SET,
                                        (XmGeoMajorLayout) layoutPtr, rowPtr) ;
                fixUps = TRUE ;
                }
            rowPtr += layoutPtr->box_count + 1 ;         /* Skip to next row.*/
            ++layoutPtr ;
            }
        /* Now set the widgets to the values in the boxes.
        */
        layoutPtr = &(geoSpec->layouts->row) ;
        rowPtr = geoSpec->boxes ;
        while(    !(layoutPtr->end)    )
        {
            _XmSetKidGeo( rowPtr, geoSpec->instigator) ;

            rowPtr += layoutPtr->box_count + 1 ;         /* Skip to next row.*/
            ++layoutPtr ;
            }
        if(    fixUps    )
        {
            /* Now call the fix_up routines again, to give the user a chance to
            *   undo the chances to the boxes in order to keep consistency for
            *   subsequent layout operations.
            */
            layoutPtr = &(geoSpec->layouts->row) ;
            rowPtr = geoSpec->boxes ;
            while(    !(layoutPtr->end)    )
            {
                if(    layoutPtr->fix_up    )
                {
                    (*(layoutPtr->fix_up))( geoSpec, XmGEO_POST_SET,
                                        (XmGeoMajorLayout) layoutPtr, rowPtr) ;
                    }
                rowPtr += layoutPtr->box_count + 1 ;     /* Skip to next row.*/
                ++layoutPtr ;
                }
            }
        }
    return ;
    }

/****************************************************************
 * This routine adjusts boxes according to policies regarding border size
 *   and even-sized boxes.  Box dimensions are altered appropriately if
 *   even_width or even_height parameters are set.  Borders are set if
 *   uniform_border is TRUE.
 ****************/
void
_XmGeoAdjustBoxes(
        XmGeoMatrix geoSpec )
{
    register XmKidGeometry   rowPtr ;
    register XmKidGeometry   boxPtr ;
            XmGeoRowLayout  layoutPtr ;
            Dimension       globalSetBorder ;
            Dimension       globalBorder ;
            Dimension       borderValue ;
/****************/

    globalSetBorder = geoSpec->uniform_border ;
    globalBorder = geoSpec->border ;

    rowPtr = geoSpec->boxes ;
    layoutPtr = &(geoSpec->layouts->row) ;

    while(    !(layoutPtr->end)    )
    {
        if(    layoutPtr->even_width    )
        {   _XmGeoBoxesSameWidth( rowPtr, layoutPtr->even_width) ;
            }
        if(    layoutPtr->even_height    )
        {   _XmGeoBoxesSameHeight( rowPtr, layoutPtr->even_height) ;
            }
        if(    globalSetBorder  ||  layoutPtr->uniform_border    )
        {   if(    globalSetBorder    )
            {   borderValue = globalBorder ;
                }
            else
            {   borderValue = layoutPtr->border ;
                }
            boxPtr = rowPtr ;
            while(    boxPtr->kid    )
            {   boxPtr->box.border_width = borderValue ;
                ++boxPtr ;
                }
            }
        while(    (rowPtr++)->kid    )  /* Go to next row of boxes. */
        { /*EMPTY*/  }
        ++layoutPtr ;           /* Go to next row layout record.*/
        }
    return ;
    }

/****************************************************************
 * This routine traverses the matrix and collects data regarding the
 *   sizes of boxes, the minimum fill area expected, and various other
 *   parameters which are used during the layout process.
 ****************/
void
_XmGeoGetDimensions(
        XmGeoMatrix geoSpec )
{
    register XmKidGeometry   rowPtr ;
    register XmKidGeometry   boxPtr ;
            XmGeoRowLayout  layoutPtr ;
            Dimension       boxH ;
            Dimension       rowH ;
            Dimension       rowW ;
            Dimension       matrixFillH ;
            Dimension       matrixBoxesH ;
            Dimension       matrixW ;
            Dimension       endSpaceW ;
            unsigned int    numBoxes ;
            Dimension       marginW ;
            Dimension       marginH ;
/****************/

    marginH = geoSpec->margin_h ;
    marginW = geoSpec->margin_w ;
    rowPtr = geoSpec->boxes ;
    layoutPtr = &(geoSpec->layouts->row) ;
    matrixW = 0 ;
    matrixBoxesH = 0 ;
    matrixFillH = layoutPtr->space_above ;
    if(    matrixFillH < marginH    )
    {   matrixFillH = 0 ;
        }
    else
    {   matrixFillH -= marginH ;  /* This dimension does not include margins.*/
        }
    geoSpec->stretch_boxes = FALSE ;
    while(    !(layoutPtr->end)    )
    {
        /* Gather information about the height, width, and number of boxes
        *   in the row.
        */
        rowW = 0 ;
        rowH = 0 ;
        numBoxes = 0 ;
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {   rowW += boxPtr->box.width + (boxPtr->box.border_width << 1) ;
            boxH = boxPtr->box.height + (boxPtr->box.border_width << 1) ;
            ASSIGN_MAX( rowH, boxH) ;   /* The tallest box is the row height.*/
            ++numBoxes ;
            ++boxPtr ;
            }
        /* Fill row layout record with info about row.
        */
        layoutPtr->max_box_height = rowH ;/* Tallest box in row, with border.*/
        layoutPtr->boxes_width = rowW ; /* Sum of box widths and borders.*/
        layoutPtr->box_count = numBoxes ;

        /* Check for vertical stetch row.
        */
        if(    layoutPtr->stretch_height    )
        {   if(    layoutPtr->fit_mode != XmGEO_WRAP    )
            {   geoSpec->stretch_boxes = TRUE ;
                }
            else
            {   layoutPtr->stretch_height = FALSE ;
                }
            }
        /* Compute row width to generate matrix width.  Exclude margins.
        */
        if(    layoutPtr->space_end > marginW    )
        {   endSpaceW = layoutPtr->space_end - marginW ;
            }
        else
        {   endSpaceW = 0 ;
            }
        /* Fill width is the minimum spacing between (borders of) boxes plus
        *   any extra space required at ends.  Margins are not included.
        */
        layoutPtr->fill_width = (endSpaceW << 1)
                                + ((numBoxes - 1) * layoutPtr->space_between) ;
        /* Maximum row width is the overall matrix width, less margins.  Add
        *   box width to fill width for total width this row.
        */
        rowW += layoutPtr->fill_width ;
        ASSIGN_MAX( matrixW, rowW) ;
        rowPtr = boxPtr + 1 ;   /* Skip over NULL box marking the end of row.*/
        ++layoutPtr ;           /* Go to next row layout record.*/
        /* Accumulate heights of each row.
        */
        matrixFillH += layoutPtr->space_above ;
        matrixBoxesH += rowH ;
        }
    /* The matrixFillH variable already has fill space included from the final
    *   row layout record.  This must be reduced by the amount of the margin,
    *   or a smaller amount if the amount specified was less than the margin.
    */
    if(    layoutPtr->space_above < marginH    )
    {   matrixFillH -= layoutPtr->space_above ;
        }
    else
    {   matrixFillH -= marginH ;
        }
    geoSpec->max_major = matrixW ;         /* Widest row, excluding margins. */
    geoSpec->boxes_minor = matrixBoxesH ;  /* Sum of tallest box in each row.*/
    geoSpec->fill_minor = matrixFillH ;    /* Sum of vertical fill spacing.  */
    }

/****************************************************************
 * After the boxes have been layed-out according to the minimum vertical fill
 *   requirements of the matrix, this routine stretches the layout to fill
 *   any extra space required by the managing widget.
 * Returns new height after extra spacing is inserted.
 ****************/
static Dimension
_XmGeoStretchVertical(
        XmGeoMatrix geoSpec,
        Dimension actualH,
        Dimension desiredH )
{
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   rowPtr ;
            int             fillOffset ;
            int             stretchableSpace ;
            int             deltaY ;
            int             deltaH ;
/****************/

    layoutPtr = &(geoSpec->layouts->row) ;
    stretchableSpace = 0 ;
    fillOffset = ((int) desiredH) - ((int) actualH) ;
    if(    fillOffset < 0    )
    {   /* Must shrink stretchable boxes.
        */
        while(    !layoutPtr->end    )
        {   if(    layoutPtr->stretch_height
                    && (layoutPtr->max_box_height > layoutPtr->min_height)    )
            {
                stretchableSpace += layoutPtr->max_box_height
                                                      - layoutPtr->min_height ;
                }
            ++layoutPtr ;
            }
        if(    -fillOffset > stretchableSpace    )
        {   fillOffset = -stretchableSpace ;
            }
        }
    else
    {   /* Must grow stretchable boxes.
        */
        while(    !layoutPtr->end    )
        {   if(    layoutPtr->stretch_height    )
            {   stretchableSpace += layoutPtr->max_box_height ;
                }
            ++layoutPtr ;
            }
        }
    if(    !stretchableSpace    )
    {   /* No stretchable boxes, so return with current height.
        */
        return( actualH) ;
        }
    deltaY = 0 ;
    rowPtr = geoSpec->boxes ;
    layoutPtr = &(geoSpec->layouts->row) ;
    while(    !layoutPtr->end    )
    {   if(    layoutPtr->stretch_height    )
        {
            if(    fillOffset < 0    )
            {   if(    layoutPtr->max_box_height > layoutPtr->min_height    )
                {   deltaH = (((int) (layoutPtr->max_box_height
                                        - layoutPtr->min_height)) * fillOffset)
                                                           / stretchableSpace ;
                    }
                else
                {   deltaH = 0 ;
                    }
                /* deltaH is now <= 0.
                */
                while(    rowPtr->kid    )
                {
		    int boxCorrection = layoutPtr->max_box_height
                                                         - rowPtr->box.height ;
		    if(    boxCorrection > -deltaH    )
		    {   boxCorrection = -deltaH ;
			}
		    rowPtr->box.height += deltaH + boxCorrection ;
		    rowPtr->box.y += deltaY - (boxCorrection >> 1) ;
                    ++rowPtr ;
                    }
                }
            else /* fillOffset >= 0 */
            {
                deltaH = (layoutPtr->max_box_height * fillOffset)
                                                           / stretchableSpace ;
                while(    rowPtr->kid    )
                {   rowPtr->box.height += deltaH ;
                    rowPtr->box.y += deltaY ;
                    ++rowPtr ;
                    }
                }
            deltaY += deltaH ;
            }
        else
        {   while(    rowPtr->kid    )
            {   rowPtr->box.y += deltaY ;
                ++rowPtr ;
                }
            }
        ++rowPtr ;
        ++layoutPtr ;
        }
    return( actualH + deltaY) ;                    /* Return new height.*/
    }

/****************************************************************
 * After the boxes have been layed-out according to the minimum vertical fill
 *   requirements of the matrix, this routine stretches the layout to fill
 *   any extra space required by the managing widget.
 * Returns new height after extra spacing is inserted.
 ****************/
static Dimension
_XmGeoFillVertical(
        XmGeoMatrix geoSpec,
        Dimension actualH,
        Dimension desiredH )
{
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   rowPtr ;
            unsigned long   fillAmount ;
            unsigned long   totalSpecSpace ;
            Dimension       marginH ;
            Dimension       firstSpecSpace ;
            Dimension       lastSpecSpace ;
            Dimension       currentFirstSpace ;
            Dimension       currentLastSpace ;
            Dimension       newFirstSpace ;
            Dimension       newLastSpace ;
            int             deltaY ;
/****************/

    /* Need to accumulate specified spacing factors, saving first and last
    *   ends separately.
    */
    layoutPtr = &(geoSpec->layouts->row) ;
    totalSpecSpace = 0 ;
    firstSpecSpace = layoutPtr->space_above ;
    while(    !(++layoutPtr)->end    )
    {   totalSpecSpace += layoutPtr->space_above ;
        }
    lastSpecSpace = layoutPtr->space_above ;
    totalSpecSpace += firstSpecSpace + lastSpecSpace ;
    if(    !totalSpecSpace    )
    {   /* Zero spacing specified, so just return as is.
        */
        return( actualH) ;
        }
    /* Must reconstruct the actual spacing, which is the specified minimum.
    * Save current end spacing separately, since everything done here is
    *   relative to the actual coordinates of the matrix and it will be
    *   needed later.
    */
    marginH = geoSpec->margin_h ;
    currentFirstSpace = (firstSpecSpace < marginH) ? marginH : firstSpecSpace ;
    currentLastSpace  = (lastSpecSpace < marginH) ? marginH : lastSpecSpace ;

    /* Fill amount includes the current fill plus margins and extra
    *   spacing.
    */
    fillAmount = (desiredH - actualH) + geoSpec->fill_minor
                                       + currentFirstSpace + currentLastSpace ;
    newFirstSpace = (Dimension) ((fillAmount * (unsigned long) firstSpecSpace)
                                                            / totalSpecSpace) ;
    newLastSpace = (Dimension) ((fillAmount * (unsigned long) lastSpecSpace)
                                                            / totalSpecSpace) ;
    if(    newFirstSpace < marginH    )
    {   fillAmount -= marginH ;
        totalSpecSpace -= firstSpecSpace ;
        newFirstSpace = marginH ;
        }
    if(    newLastSpace < marginH    )
    {   fillAmount -= marginH ;
        totalSpecSpace -= lastSpecSpace ;
        newLastSpace = marginH ;
        }
    /* Now traverse the matrix, offsetting all y-ccordinates according to
    *   additional spacing.  Wrapped lines receive no extra spacing between
    *   them.
    */
    deltaY = newFirstSpace - currentFirstSpace ;
    rowPtr = geoSpec->boxes ;
    layoutPtr = &(geoSpec->layouts->row) ;
    for(;;)
    {   while(    rowPtr->kid    )
        {   rowPtr->box.y += deltaY ;
            ++rowPtr ;
            }
        ++rowPtr ;
        ++layoutPtr ;
        if(    layoutPtr->end    )
        {   break ;
            }
        deltaY += (int) (((((unsigned long) layoutPtr->space_above)
                    * fillAmount) / totalSpecSpace) - layoutPtr->space_above) ;
        }
    deltaY += newLastSpace - currentLastSpace ;

    return( actualH + deltaY) ;                    /* Return new height.*/
    }

/****************************************************************
 * Calculates and returns appropriate fill factor from given layout
 *   parameters.  Also returns appropriate spacing for ends.
 * The fill factor returned is for use with all spacing between boxes, but
 *   not the ends (use provided spacing).
 ****************/
static void
_XmGeoCalcFill(
        Dimension fillSpace,        /* Fill space, including margins.*/
        Dimension margin,           /* Margin (included in fillSpace).*/
        unsigned int numBoxes,
        Dimension endSpec,
        Dimension betweenSpec,
        Dimension *pEndSpace,       /* Receives end spacing.*/
        Dimension *pBetweenSpace )  /* Receives between spacing.*/
{
            Dimension       totalSpecSpace ;/* Sum of specified spacing.*/
/****************/

    if(    !endSpec    )
    {   if(    numBoxes == 1    )
        {   endSpec = 1 ;
            }
        else
        {   if(    !betweenSpec    )
            {   betweenSpec = (Dimension) (numBoxes - 1) ;
                }
            }
        }
    totalSpecSpace = (betweenSpec * (numBoxes - 1)) + (endSpec << 1) ;
    *pEndSpace = (endSpec * fillSpace) / totalSpecSpace ;

    if(    *pEndSpace < margin    )
    {
        if(    (endSpec << 1) < totalSpecSpace    )
        {   totalSpecSpace -= endSpec << 1 ;
            }
        else
        {   totalSpecSpace = 1 ;
            }
        if(    (margin << 1) < fillSpace    )
        {   fillSpace -= margin << 1 ;
            }
        else
        {   fillSpace = 0 ;
            }
        *pEndSpace = margin ;
        }
    *pBetweenSpace = (betweenSpec * fillSpace) / totalSpecSpace ;
    return ;
    }

/****************************************************************
 * The x, y, width, and height fields of the boxes in the geoSpec matrix
 *   are modified with values appropriate for the layout parameters specified
 *   by x, y, pW, and pH.
 * The overall width and height dimensions at the specified locations
 *   of pW and pH must be initially set to their desired values, or zero
 *   for the default layout.
 * The actual values of the width and height (after layout) are returned at
 *   the locations pW and pH.
 ****************/
void
_XmGeoArrangeBoxes(
        XmGeoMatrix geoSpec,        /* Array of box lists (rows).*/
        Position x,                 /* X coordinate of composite.*/
        Position y,                 /* Y coordinate of composite.*/
        Dimension *pW,              /* Initial value is minimum width.*/
        Dimension *pH )             /* Initial value is minimum height.*/
{
            Dimension       marginW ;   /* Margin between sides and boxes.*/
            Dimension       marginH ;   /* Margin between top/bot and boxes.*/
            XmKidGeometry   rowPtr ;
            XmGeoRowLayout  layoutPtr ;
            Dimension       actualW ;
            Dimension       actualH ;
            Dimension       initY ;
/****************/

    if(    geoSpec->arrange_boxes
       &&  (geoSpec->arrange_boxes != _XmGeoArrangeBoxes)    )
      {
	(*(geoSpec->arrange_boxes))( geoSpec, x, y, pW, pH) ;
	return ;
      }
    /* Fix box dimensions according to even_height/width and uniform_border
    *   specifications.
    */
    _XmGeoAdjustBoxes( geoSpec) ;

    /* Compute layout dimensions.
    */
    _XmGeoGetDimensions( geoSpec) ;

    /* Initialize global layout dimensions.
    */
    marginW = geoSpec->margin_w ;
    marginH = geoSpec->margin_h ;
    actualW = geoSpec->max_major + (marginW << 1) ;

/****** the value of this assignment is never used **********
    actualH = geoSpec->boxes_minor + geoSpec->fill_minor + (marginH << 1) ;
*************************************************************/

    /* Adjust layout dimensions to requested dimensions.
    */
    if(    *pW    )
    {   actualW = *pW ;
        }

/******* the value assigned to actualH is never used *************
    if(    *pH    )
    {   actualH = *pH ;
        }
******************************************************************/

    /* Save initial Y coordinate for later computation of height.
    */
    initY = y ;

    /* Layout horizontal position of each box in row, one row at a time.
    */
    layoutPtr = &(geoSpec->layouts->row) ;
    rowPtr = geoSpec->boxes ;
    /* Add first end spacing.
    */
    if(    layoutPtr->space_above > marginH    )
    {   y += layoutPtr->space_above ;
        }
    else
    {   y += marginH ;
        }
    while(    !(layoutPtr->end)    )
    {
        /* Arrange one row of boxes at a time.
        */
        y = _XmGeoArrangeList( rowPtr, layoutPtr, x, y, actualW,
                                                            marginW, marginH) ;
        rowPtr += layoutPtr->box_count + 1 ;             /* Skip to next row.*/
        ++layoutPtr ;

        /* Add between-row spacing.
        */
        y += layoutPtr->space_above ;
        }
    if(    layoutPtr->space_above < marginH    )
    {   /* Fill out to the minimum margin if previous spacing is less
        *   than margin.
        */
        y += marginH - layoutPtr->space_above ;
        }
    actualH = y - initY ;
    if(    *pH  &&  (actualH != *pH)    )
    {   if(    geoSpec->stretch_boxes    )
        {   /* Has stretchable boxes, so grow or shrink using stretch.
            */
            actualH = _XmGeoStretchVertical( geoSpec, actualH, *pH) ;
            }
        else
        {   if(    actualH < *pH    )
            {   /* Layout is smaller than specified height, so fill vertically.
                */
                actualH = _XmGeoFillVertical( geoSpec, actualH, *pH) ;
                }
            }
        }
    /* Set return values of actual width and height of matrix.
    */
    geoSpec->width = actualW ;
    if(    *pW < actualW    )
    {   *pW = actualW ;
        }
    geoSpec->height = actualH ;
    if(    *pH < actualH    )
    {   *pH = actualH ;
        }
    return ;
    }

/****************************************************************/
static int
boxWidthCompare(
        XmConst void * boxPtr1,
        XmConst void * boxPtr2 )
{
/****************/

    return( (*((XmKidGeometry *) boxPtr1))->box.width
                                 > (*((XmKidGeometry *) boxPtr2))->box.width) ;
    }
/****************************************************************
 * This routine alters box sizes such that the composite width is reduced
 *   by the offset amount specified.  The boxWidth parameter is assumed to
 *   contain the sum of the all box widths, including borders.
 * The algorithm used by this routine tends to average the width of all boxes.
 *   In other words, to achieve the desired width reduction, the largest boxes
 *   are reduced first, possibly until all boxes are the same width
 *   (thereafter reducing all boxes evenly).
 ****************/
static void
FitBoxesAveraging(
        XmKidGeometry rowPtr,
        unsigned int numBoxes,
        Dimension boxWidth,
        int amtOffset )
{
            unsigned int    Index ;
            XmKidGeometry * sortedBoxes ;
/****************/

    /* Get memory to use for sorting the list of boxes.
    */
    sortedBoxes = (XmKidGeometry *) XtMalloc( numBoxes
                                                    * sizeof( XmKidGeometry)) ;
    /* Enter the boxes into the array and sort.
    */
    Index = 0 ;
    while(    Index < numBoxes    )
    {   sortedBoxes[Index] = &rowPtr[Index] ;
        /* Need to remove the border_width component from boxWidth.
        */
        boxWidth -= (rowPtr[Index].box.border_width << 1) ;
        ++Index ;
        }
    qsort( (void *) sortedBoxes, (size_t) numBoxes, sizeof( XmKidGeometry),
                                                             boxWidthCompare) ;
    /* Now sorted with smallest box first.
    */
    Index = 0 ;
    while(    Index < numBoxes    )
    {
        /* The right-hand side of the comparison represents the amount of
        *   area that would be truncated if all boxes were the same width
        *   as sortedBoxes[Index].  The loop will break when the Index
        *   points to the smallest box in the list to be truncated.
        */
        if(   amtOffset >= ((int) (boxWidth - ((sortedBoxes[Index]->box.width)
                                                   * (numBoxes - Index))))    )
        {   break ;
            }
        /* This keeps the above comparison simple.
        */
        boxWidth -= sortedBoxes[Index]->box.width ;
        ++Index ;
        }
    if(    Index < numBoxes    )
    {
        if(    (int) boxWidth > amtOffset    )
        {
            boxWidth = (boxWidth - amtOffset) / (numBoxes - Index) ;

            if(    !boxWidth    )
            {   boxWidth = 1 ;
                }
            }
        else
        {   boxWidth = 1 ;
            }
        /* boxWidth is now the truncated width of the remaining boxes
        *   in the sorted list.  Set these boxes appropriately.
        */
        while(    Index < numBoxes    )
        {   sortedBoxes[Index]->box.width = boxWidth ;
            ++Index ;
            }
        }
    XtFree( (char *) sortedBoxes) ;

    return ;
    }

/****************************************************************
 * This routine alters the width of boxes in proportion to the width of each
 *   box such that the total change is equal to amtOffset.  If amtOffset is
 *   greater than zero, the total width is reduced (a "fit").  Otherwise,
 *   the total width is increased (a "fill").
 ****************/
static void
FitBoxesProportional(
        XmKidGeometry rowPtr,
        unsigned int numBoxes,
        Dimension boxWidth,
        int amtOffset )
{
            int             deltaX ;
            int             deltaW ;
/****************/


    if(    boxWidth >= numBoxes    )
    {
        deltaX = 0 ;
        while(    rowPtr->kid    )
        {
            deltaW = (amtOffset * (int)(rowPtr->box.width
                       + (rowPtr->box.border_width << 1))) / ((int) boxWidth) ;
            if(    deltaW < ((int) rowPtr->box.width)    )
            {   rowPtr->box.width -= deltaW ;
                }
            else
            {   rowPtr->box.width = 1 ;
                }
            rowPtr->box.x += deltaX ;
            deltaX -= deltaW ;
            ++rowPtr ;
            }
        }
    else /* boxWidth < numBoxes */
    {
        if(    (-amtOffset) > numBoxes    )
        {
            boxWidth = (-amtOffset) / numBoxes ;
            }
        else
        {   boxWidth = 1 ;
            }
        deltaX = 0 ;
        while(    rowPtr->kid    )
        {
            rowPtr->box.width = boxWidth ;
            rowPtr->box.x += deltaX ;
            deltaX += boxWidth ;
            ++rowPtr ;
            }

        }
    return ;
    }

/****************************************************************/
static void
SegmentFill(
        XmKidGeometry rowBoxes,
        unsigned int numBoxes,
        XmGeoRowLayout layoutPtr,
        Position x,
        Dimension width,
        Dimension marginW,
        Position endX,
        Position maxX,
        Dimension endSpace,
        Dimension betweenSpace )
{
            Widget          holdEnd ;
            Dimension       spacedWidth ;
            Dimension       boxWidth ;
            Dimension       sumW ;
            int             amtOffset ;
            Dimension       totalFill ;
            Position        rowX ;
            XmKidGeometry   rowPtr ;
/****************/

    holdEnd = rowBoxes[numBoxes].kid ;
    rowBoxes[numBoxes].kid = NULL ;

    spacedWidth = (betweenSpace * (numBoxes - 1)) + (endSpace << 1) ;
    amtOffset = ((int) spacedWidth + (maxX - endX)) ;
    if(    (amtOffset > 0)  &&  (amtOffset < width)    )
    {   boxWidth = width - amtOffset ;
        }
    else
    {   boxWidth = 1 ;
        }
    sumW = boxWidth + spacedWidth ;

    amtOffset = ((int) sumW) - ((int) width) ;
    /* Setup the default spacing.
    */
    betweenSpace = layoutPtr->space_between ;
    endSpace = (layoutPtr->space_end < marginW)
                                             ? marginW : layoutPtr->space_end ;
    switch(    layoutPtr->fill_mode    )
    {   case XmGEO_CENTER:
        {
            /* Compute new spacing values to result in a centered
            *   layout when passed to the simple layout routine.
            */
            if(    width > sumW    )
            {   totalFill = (spacedWidth + width) - sumW ;
                }
            else
            {   totalFill = marginW << 1 ;
                }
                _XmGeoCalcFill(totalFill, marginW, numBoxes,
                               layoutPtr->space_end, layoutPtr->space_between,
                               &endSpace, &betweenSpace) ;
            break ;
            }
        case XmGEO_PACK:
        {   /* For a packed layout, just layout with extra space
            *   at the end of the row.
            */
            break ;
            }
        case XmGEO_EXPAND:
        default:
        {   /* FitBoxesProportional will fill if amtOffset < 0,
            *    as it is here.
            */
            FitBoxesProportional( rowBoxes, numBoxes, boxWidth, amtOffset) ;
            break ;
            }
        }
    rowX = x + endSpace ;
    rowPtr = rowBoxes ;
    while(    rowPtr->kid    )
    {
        rowPtr->box.x = rowX ;
        rowX += rowPtr->box.width + (rowPtr->box.border_width << 1)
                                                               + betweenSpace ;
        ++rowPtr ;
        }
    rowBoxes[numBoxes].kid = holdEnd ;
    return ;
    }

/****************************************************************
 * This routine lays out the row of boxes with the spacing specified in
 *   the endSpace and betweenSpace parameters.  If the width of a row
 *   which contains more than one box causes the right edge of the
 *   row to be greater than maxX, then the boxes will wrap to the next
 *   line.
 * The Y coordinate of the space following the layout is returned.
 ****************/
static Position
_XmGeoLayoutWrap(
        XmKidGeometry rowPtr,
        XmGeoRowLayout layoutPtr,
        Position x,
        Position y,
        Dimension endSpace,
        Dimension betweenSpace,
        Position maxX,
        Dimension width,
        Dimension marginW )
{
            Position        rowX ;
            Dimension       rowH ;
            Position        boxMaxX ;
            unsigned int    numBoxes ;
            Dimension       boxH ;
            int             deltaW ;
            XmKidGeometry   rowBegin ;
    register XmKidGeometry  boxPtr ;
            Position        endX ;
/****************/

    rowX = x + endSpace ;
    rowH = layoutPtr->max_box_height ;
    numBoxes = 0 ;
    rowBegin = rowPtr ;
    boxPtr = rowPtr ;
    while(    boxPtr->kid    )
    {   boxMaxX = rowX + boxPtr->box.width + (boxPtr->box.border_width << 1) ;

        if(    (boxMaxX > maxX)  &&  numBoxes    )
        {   /* Wrap the line.  Also adjust preceding segment according to
            *   fill policy.
            */
            endX = rowX - betweenSpace ;
            SegmentFill( rowBegin, numBoxes, layoutPtr, x, width,
                                 marginW, endX, maxX, endSpace, betweenSpace) ;
            numBoxes = 0 ;
            rowX = x + endSpace ;
            y += rowH ;
            rowBegin = boxPtr ;
            boxMaxX = rowX + boxPtr->box.width
                                            + (boxPtr->box.border_width << 1) ;
            }
        if(    boxMaxX > maxX    )
        {   /* Since it wasn't wrapped, there must be only one box in this
            *   segment.  It is too wide, so simply truncate it.
            */
            deltaW = ((int) (endSpace + boxMaxX)) - ((int) (maxX + marginW)) ;
            if(    (deltaW < ((int) boxPtr->box.width))  &&  (deltaW > 0)    )
            {   boxPtr->box.width -= deltaW ;
                }
            else
            {   boxPtr->box.width = 1 ;
                }
            boxMaxX = rowX + boxPtr->box.width
                                            + (boxPtr->box.border_width << 1) ;
            }
        boxPtr->box.x = rowX ;
        boxPtr->box.y = y ;
        boxH = boxPtr->box.height + (boxPtr->box.border_width << 1) ;
        if(    boxH != rowH    )
        {   /* If box height is not the same as the maximum box height
            *   of the row, then adjust y to center the box in the row.
            */
            boxPtr->box.y += (((int) rowH - (int) boxH) >> 1) ;
            }
        rowX = boxMaxX + betweenSpace ;
        ++numBoxes ;
        ++boxPtr ;
        }
    endX = rowX - betweenSpace ;
    SegmentFill( rowBegin, numBoxes, layoutPtr, x, width,
                                 marginW, endX, maxX, endSpace, betweenSpace) ;
    if(    layoutPtr->sticky_end    )
    {
        boxPtr = &rowPtr[layoutPtr->box_count - 1] ;
        endX = maxX - (boxPtr->box.width + (boxPtr->box.border_width << 1)) ;
        if(    endX > boxPtr->box.x    )
        {   boxPtr->box.x = endX ;
            }
        }
    return( y + rowH) ;
    }
/****************************************************************
 * This routine does a simple layout of the boxes in the row.  It assumes
 *   that all boxes have been conditioned to fit appropriately with the
 *   spacing specified by the endSpace and betweenSpace parameters.
 * The Y coordinate of the space following the layout is returned.
 ****************/
static Position
_XmGeoLayoutSimple(
        XmKidGeometry rowPtr,
        XmGeoRowLayout layoutPtr,
        Position x,
        Position y,
        Position maxX,
        Dimension endSpace,
        Dimension betweenSpace )
{
            Position        rowX ;
            Position        newX ;
            Dimension       rowH ;
            Dimension       boxH ;
/****************/

    rowH = layoutPtr->max_box_height ;
    rowX = x + endSpace ;
    while(    rowPtr->kid    )
    {
        rowPtr->box.x = rowX ;
        rowPtr->box.y = y ;
        boxH = rowPtr->box.height + (rowPtr->box.border_width << 1) ;
        if(    boxH != rowH    )
        {   /* If box height is not the same as the maximum box height
            *   of the row, then adjust y to center the box in the row.
            */
            rowPtr->box.y += ((rowH - boxH) >> 1) ;
            }
        rowX += rowPtr->box.width + (rowPtr->box.border_width << 1)
                                                               + betweenSpace ;
        ++rowPtr ;
        }
    if(    layoutPtr->sticky_end    )
    {
        --rowPtr ;
        newX = maxX - (rowPtr->box.width + (rowPtr->box.border_width << 1)) ;
        if(    newX > rowPtr->box.x    )
        {   rowPtr->box.x = newX ;
            }
        }
    return( y + rowH) ;
    }


/****************************************************************
 * This routines lays out the boxes in this row according to the specified
 *   paramaters and the policies specified in the layout record at layoutPtr.
 ****************/
static Position
_XmGeoArrangeList(
        XmKidGeometry rowBoxes,
        XmGeoRowLayout layoutPtr,
        Position x,
        Position y,
        Dimension width,
        Dimension marginW,
        Dimension marginH )	/* unused */
{
            Dimension       sumW ;
            unsigned int    numBoxes ;
            Dimension       betweenBoxes ;
            Dimension       endsOfBoxes ;
            int             amtOffset ;
            Dimension       boxWidth ;
            Position        maxX ;
            Dimension       totalFill ;
/****************/

    numBoxes = layoutPtr->box_count ;
    boxWidth = layoutPtr->boxes_width ;
    sumW = boxWidth + layoutPtr->fill_width + (marginW << 1) ;
    amtOffset = ((int) sumW) - ((int) width) ;
    /* Setup the default spacing.
    */
    betweenBoxes = layoutPtr->space_between ;
    endsOfBoxes = (layoutPtr->space_end < marginW)
                                             ? marginW : layoutPtr->space_end ;
    maxX = x + width - marginW ;

    if(    (sumW > width)  &&  (layoutPtr->fit_mode == XmGEO_WRAP)    )
    {   /* Wrapping is required, so fill routines and other policy decisions
        *   are not needed.  Do the layout using the wrap routine and we're
        *   done.
        */
        y = _XmGeoLayoutWrap( rowBoxes, layoutPtr, x, y, endsOfBoxes,
                                          betweenBoxes, maxX, width, marginW) ;
        }
    else
    {   if(    sumW > width    )
        {   switch(    layoutPtr->fit_mode    )
            {   case XmGEO_AVERAGING:
                {   FitBoxesAveraging( rowBoxes, numBoxes, boxWidth,
                                                                   amtOffset) ;
                    break ;
                    }
                case XmGEO_PROPORTIONAL:
                default:
                {   FitBoxesProportional( rowBoxes, numBoxes, boxWidth,
                                                                   amtOffset) ;
                    }
                }
            }
        else
        {   if(    sumW < width    )
            {   switch(    layoutPtr->fill_mode    )
                {   case XmGEO_CENTER:
                    {
                        /* Compute new spacing values to result in a centered
                        *   layout when passed to the simple layout routine.
                        */
                        totalFill = (marginW << 1) + layoutPtr->fill_width
                                                               + width - sumW ;
                        _XmGeoCalcFill( totalFill, marginW, numBoxes,
                                layoutPtr->space_end, layoutPtr->space_between,
                                                 &endsOfBoxes, &betweenBoxes) ;
                        break ;
                        }
                    case XmGEO_PACK:
                    {   /* For a packed layout, just layout with extra space
                        *   at the end of the row.
                        */
                        break ;
                        }
                    case XmGEO_EXPAND:
                    default:
                    {   /* FitBoxesProportional will fill if amtOffset < 0,
                        *    as it is here.
                        */
                        FitBoxesProportional( rowBoxes, numBoxes, boxWidth,
                                                                   amtOffset) ;
                        break ;
                        }
                    }
                }
            }
        y = _XmGeoLayoutSimple( rowBoxes, layoutPtr, x, y, maxX,
                                                   endsOfBoxes, betweenBoxes) ;
        }
    return( y) ;
    }

/****************************************************************
 * Changes boxes in the kid geometry list to have desired width.
 * If width > 1, then use the specified width.
 * If width == 1, then use the width of the widest box.
 * If width == 0, then do not change the boxes but return the width of
 *   the widest box.
 * Returns the value of the width actually used.
 ****************/
Dimension
_XmGeoBoxesSameWidth(
        XmKidGeometry rowPtr,
        Dimension width )
{
    register XmKidGeometry   boxPtr ;
    register Dimension       useW ;
/****************/

    useW = width ;  /* Setup default width of each box in row, as specified.*/

    if(    width <= 1    )
    {
        /* If user specified width parameter of zero or one, then find the
        *   width of the widest box.
        */
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {   ASSIGN_MAX( useW, boxPtr->box.width) ;
            ++boxPtr ;
            }
        }
    if(    width    )
    {
        /* If width parameter is non-zero, then set the boxes appropriately.
        */
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {   boxPtr->box.width = useW ;
            ++boxPtr ;
            }
        }
    return( useW) ;
    }
/****************************************************************
 * Changes boxes in the kid geometry list to have desired height.
 * If height > 1, then use the specified height.
 * If height == 1, then use the height of the tallest box.
 * If height == 0, then do not change the boxes but return the height of
 *   the tallest box.
 * Returns the value of the height actually used.
 ****************/
Dimension
_XmGeoBoxesSameHeight(
        XmKidGeometry rowPtr,
        Dimension height )
{
    register XmKidGeometry   boxPtr ;
    register Dimension       useH ;
/****************/

    useH = height ; /* Setup default height of each box in row, as specified.*/

    if(    height <= 1    )
    {
        /* If user specified height parameter of zero or one, then find the
        *   height of the tallest box.
        */
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {   ASSIGN_MAX( useH, boxPtr->box.height) ;
            ++boxPtr ;
            }
        }
    if(    height    )
    {
        /* If height parameter is non-zero, then set the boxes appropriately.
        */
        boxPtr = rowPtr ;
        while(    boxPtr->kid    )
        {   boxPtr->box.height = useH ;
            ++boxPtr ;
            }
        }
    return( useH) ;
    }

/****************************************************************
 * This routine is a fixup routine which can be used for rows which consist
 *   of a single separator widget.  The effect of this routine is to have
 *   the separator ignore the margin width.
 ****************/
void
_XmSeparatorFix(
        XmGeoMatrix geoSpec,
        int action,
        XmGeoMajorLayout layoutPtr, /* unused */
        XmKidGeometry rowPtr )
{
    register Dimension       marginW ;
    register Dimension       twoMarginW ;
/****************/

    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;

    switch(    action    )
    {
        case XmGEO_PRE_SET:
        {   rowPtr->box.x -= marginW ;
            rowPtr->box.width += twoMarginW ;
            break ;
            }
        default:
        {   if(    rowPtr->box.width > twoMarginW    )
            {
                /* Avoid subtracting a margin from box width which would
                *   result in underflow.
                */
                rowPtr->box.x += marginW ;
                rowPtr->box.width -= twoMarginW ;
                }
            if(    action == XmGET_PREFERRED_SIZE    )
            {
                /* Set width to some small value so it does not
                *   effect total width of matrix.
                */
                rowPtr->box.width = 1 ;
                }
            break ;
            }
        }
    return ;
    }


/****************************************************************
 * This routine is a fixup routine which can be used for rows which consist
 *   of a single MenuBar RowColumn.  The effect of this routine is to have
 *   the RowColumn ignore the margin width and height.
 ****************/
void
_XmMenuBarFix(
        XmGeoMatrix geoSpec,
        int action,
        XmGeoMajorLayout layoutPtr, /* unused */
        XmKidGeometry rowPtr )
{
    register Dimension       marginW ;
    register Dimension       marginH ;
    register Dimension       twoMarginW ;
/****************/

    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;
    marginH = geoSpec->margin_h ;

    switch(    action    )
    {
        case XmGEO_PRE_SET:
        {   rowPtr->box.x -= marginW ;
            rowPtr->box.width += twoMarginW ;
            rowPtr->box.y -= marginH ;
            break ;
            }
        default:
        {   if(    rowPtr->box.width > twoMarginW    )
            {
                /* Avoid subtracting a margin from box width which would
                *   result in underflow.
                */
                rowPtr->box.x += marginW ;
                rowPtr->box.width -= twoMarginW ;
                }
            if(    action == XmGET_PREFERRED_SIZE    )
            {
                /* Set width to some small value so it does not
                *   effect total width of matrix.
                */
                rowPtr->box.width = 1 ;
                }
            break ;
            }
        }
    return ;
    }

/****************************************************************/
void
_XmGeoLoadValues(
        Widget wid,
        int geoType,
        Widget instigator,
        XtWidgetGeometry *request,
        XtWidgetGeometry *geoResult )
{
            XtWidgetGeometry reply ;
            XtWidgetGeometry * geoSource ;
/****************/

    if(    wid == instigator    )
    {   /* If this widget is making the request, then use the request info.
        */
        geoSource = request ;
        }
    else
    {   geoSource = &reply ;

        switch(    geoType    )
        {
            case XmGET_PREFERRED_SIZE:
            {   XtQueryGeometry( wid, NULL, &reply) ;
                break ;
                }
            case XmGET_ACTUAL_SIZE:
            default:
            {   reply.request_mode = 0 ;  /* Will cause geoSpec to be loaded.*/
                break ;
                }
            }
        }
    geoResult->x = IsX( geoSource) ? geoSource->x : XtX( wid) ;
    geoResult->y = IsY( geoSource) ? geoSource->y : XtY( wid) ;
    geoResult->width = IsWidth( geoSource) ? geoSource->width : XtWidth( wid) ;
    geoResult->height = IsHeight( geoSource)
                                         ? geoSource->height : XtHeight( wid) ;
    geoResult->border_width = IsBorder( geoSource)
                              ? geoSource->border_width : XtBorderWidth( wid) ;
    geoResult->request_mode = CWX | CWY | CWWidth | CWHeight | CWBorderWidth ;
    return ;
    }

/****************************************************************
 * Get a count of the managed kids of a parent, it is assumed that all
 *   gadgets are always managed
 ****************/
int
_XmGeoCount_kids(
        register CompositeWidget c )
{
    register int i, n = 0 ;
/****************/

    for(    i = 0 ; i < c->composite.num_children ; i++    )
    {
        if(    c->composite.children[i]->core.managed    )
        {   n++ ;
            }
        }
    return( n) ;
    }

/****************************************************************
 * Assemble a kid box for each child widget and gadget, fill in data about
 *   each widget and optionally set up uniform border widths.
 * Returns a list of records, last one has a 'kid' field of NULL.  This memory
 *   for this list should eventually be freed with a call to XtFree().
 ****************/
XmKidGeometry
_XmGetKidGeo(
        Widget wid,                     /* Widget w/ children. */
        Widget instigator,              /* May point to a child who */
        XtWidgetGeometry *request,      /*   is asking to change. */
        int uniform_border,             /* T/F, enforce it. */
        Dimension border,               /* Value to use if enforcing.*/
        int uniform_width_margins,      /* unused.  T/F, enforce it. */
        int uniform_height_margins,     /* unused.  T/F, enforce it. */
        Widget help,                    /* May point to a help kid. */
        int geo_type )                  /* Actual or preferred. */
{
            CompositeWidget c = (CompositeWidget) wid ;
            XmKidGeometry   geo ;
            Widget          kidWid ;
            int             i ;
            int             j = 0 ;
            Boolean         helpFound = FALSE ;
/****************/

    geo = (XmKidGeometry) XtMalloc(
                            (_XmGeoCount_kids (c) + 1) * sizeof (XmKidGeometryRec)) ;
    /* load all managed kids */
    for(    i = 0 ; i < c->composite.num_children ; i++    )
    {
        kidWid = c->composite.children[i] ;
        if(    XtIsManaged( kidWid)    )
        {   if(    kidWid == help    )
            {   /* Save to put help widget at the end of the widget list.*/
                helpFound = TRUE ;
                }
            else
            {   geo[j].kid = kidWid ;

                _XmGeoLoadValues( kidWid, geo_type, instigator, request,
                                                               &(geo[j].box)) ;
                if(    uniform_border    )     /* if asked override border */
                {   geo[j].box.border_width = border ;
                    }
                j++ ;
                }
            }
        }
    if(    helpFound    )                 /* put help guy into list */
    {
        geo[j].kid = help ;

        _XmGeoLoadValues( help, geo_type, instigator, request, &(geo[j].box)) ;

        if(    uniform_border    )         /* if asked override border */
        {   geo[j].box.border_width = border ;
            }
        j++ ;
        }
    geo[j].kid = NULL ;                /* signal end of list */

    return( geo) ;
    }

/****************************************************************/
void
_XmGeoClearRectObjAreas(
        RectObj r,
        XWindowChanges *old )
{
            Widget          parent = XtParent( r) ;
            int             bw2 ;
/****************/

    bw2 = old->border_width << 1;
    XClearArea( XtDisplay( parent), XtWindow( parent), old->x, old->y,
                                   old->width + bw2, old->height + bw2, TRUE) ;

    bw2 = r->rectangle.border_width << 1;
    XClearArea( XtDisplay( parent), XtWindow( parent), (int) r->rectangle.x,
               (int) r->rectangle.y, (unsigned int) (r->rectangle.width + bw2),
                            (unsigned int) (r->rectangle.height + bw2), TRUE) ;
    return ;
    }

/****************************************************************
 * Take the kid geometry array and change each kid to match them.
 *   remember not to do the resize of the instigator.
 * The kid geometry "kg" is assumed to be fully specified.
 ****************/
void _XmSetKidGeo(
        XmKidGeometry kg,
        Widget instigator )
{
    Widget          w ;
    XtWidgetGeometry * b ;
    int             i ;
/****************/

    for(    i=0 ; kg[i].kid != NULL ; i++    )  {
        w = (Widget) kg[i].kid ;
        b = &(kg[i].box) ;

        if(    w != instigator    ) {
	    XmeConfigureObject( w, b->x, b->y,
			       b->width, b->height, b->border_width) ;
	}  else {
	    XtX( w) = b->x ;
	    XtY( w) = b->y ;
	    XtWidth( w) = b->width ;
	    XtHeight( w) = b->height ;
	    XtBorderWidth( w) = b->border_width ;
	}
    }
    return ;
}

/****************************************************************
 * Returns TRUE if all specified geometries of geoA are equal to either the
 *   specified geometries of geoB or to the geometry of the widget, and
 *   vice versa.  The XtCWQueryOnly bit is ignored.
 ****************/
Boolean
_XmGeometryEqual(
        Widget wid,
        XtWidgetGeometry *geoA,
        XtWidgetGeometry *geoB )
{
/****************/
    if(!geoA){ /* For fixing OSF CR 5956 */
         return(False);
    }

    if(    IsWidth( geoA)  ||  IsWidth( geoB)    )
    {
        if(    IsWidth( geoA)  &&  IsWidth( geoB)    )
        {   if(    geoA->width != geoB->width    )
            {   return( FALSE) ;
                }
            }
        else
        {   if(    IsWidth( geoA)    )
            {   if(    geoA->width != XtWidth( wid)    )
                {   return( FALSE) ;
                    }
                }
            else
            {   if(    IsWidth( geoB)    )
                {   if(    geoB->width != XtWidth( wid)    )
                    {   return( FALSE) ;
                        }
                    }
                }
            }
        }
    if(    IsHeight( geoA)  ||  IsHeight( geoB)    )
    {
        if(    IsHeight( geoA)  &&  IsHeight( geoB)    )
        {   if(    geoA->height != geoB->height    )
            {   return( FALSE) ;
                }
            }
        else
        {   if(    IsHeight( geoA)    )
            {   if(    geoA->height != XtHeight( wid)    )
                {   return( FALSE) ;
                    }
                }
            else
            {   if(    IsHeight( geoB)    )
                {   if(    geoB->height != XtHeight( wid)    )
                    {   return( FALSE) ;
                        }
                    }
                }
            }
        }
    if(    IsBorder( geoA)  ||  IsBorder( geoB)    )
    {
        if(    IsBorder( geoA)  &&  IsBorder( geoB)    )
        {   if(    geoA->border_width != geoB->border_width    )
            {   return( FALSE) ;
                }
            }
        else
        {   if(    IsBorder( geoA)    )
            {   if(    geoA->border_width != XtBorderWidth( wid)    )
                {   return( FALSE) ;
                    }
                }
            else
            {   if(    IsBorder( geoB)    )
                {   if(    geoB->border_width != XtBorderWidth( wid)    )
                    {   return( FALSE) ;
                        }
                    }
                }
            }
        }
    if(    IsX( geoA)  ||  IsX( geoB)    )
    {
        if(    IsX( geoA)  &&  IsX( geoB)    )
        {   if(    geoA->x != geoB->x    )
            {   return( FALSE) ;
                }
            }
        else
        {   if(    IsX( geoA)    )
            {   if(    geoA->x != XtX( wid)    )
                {   return( FALSE) ;
                    }
                }
            else
            {   if(    IsX( geoB)    )
                {   if(    geoB->x != XtX( wid)    )
                    {   return( FALSE) ;
                        }
                    }
                }
            }
        }
    if(    IsY( geoA)  ||  IsY( geoB)    )
    {
        if(    IsY( geoA)  &&  IsY( geoB)    )
        {   if(    geoA->y != geoB->y    )
            {   return( FALSE) ;
                }
            }
        else
        {   if(    IsY( geoA)    )
            {   if(    geoA->y != XtY( wid)    )
                {   return( FALSE) ;
                    }
                }
            else
            {   if(    IsY( geoB)    )
                {   if(    geoB->y != XtY( wid)    )
                    {   return( FALSE) ;
                        }
                    }
                }
            }
        }
    return( TRUE) ;
    }

/****************************************************************
 * Returns TRUE if all specified geometries of "desired" correspond to
 *   specified geometries of "response" and are equal to them.
 * The XtCWQueryOnly bit is ignored.
 ****************/
Boolean
_XmGeoReplyYes(
        Widget wid,		/* unused */
        XtWidgetGeometry *desired,
        XtWidgetGeometry *response )
{
/****************/
    if(!response){ /* For fixing OSF CR 5956 */
         return(False);
    }
    if(    IsWidth( desired)    )
    {
        if(    !IsWidth( response)
            || (desired->width != response->width)    )
        {
            return( FALSE) ;
            }
        }
    if(    IsHeight( desired)    )
    {
        if(    !IsHeight( response)
            || (desired->height != response->height)    )
        {
            return( FALSE) ;
            }
        }
    if(    IsBorder( desired)    )
    {
        if(    !IsBorder( response)
            || (desired->border_width != response->border_width)    )
        {
            return( FALSE) ;
            }
        }
    if(    IsX( desired)    )
    {
        if(    !IsX( response)
            || (desired->x != response->x)    )
        {
            return( FALSE) ;
            }
        }
    if(    IsY( desired)    )
    {
        if(    !IsY( response)
            || (desired->y != response->y)    )
        {
            return( FALSE) ;
            }
        }
    return( TRUE) ;
    }

/****************************************************************
 * This routine calls the geometry manager and accept the almost
 ****************/
XtGeometryResult
_XmMakeGeometryRequest(
        Widget w,
        XtWidgetGeometry *geom )
{
  XtWidgetGeometry allowed ;
  XtGeometryResult answer ;
/****************/

  answer = XtMakeGeometryRequest( w, geom, &allowed) ;

  /* On an almost, accept the returned value and make
   *   a second request to get an XtGeometryYes returned.
   */
  if(    answer == XtGeometryAlmost    )
    {
      /* The Intrinsics protocol guarantees a Yes response
       * to a request with identical geometry to that which
       * was returned by a previous request returning almost.
       */
      *geom = allowed ;
      answer = XtMakeGeometryRequest( w, geom, &allowed) ;
    }
  return answer ;
}

/****************************************************************/
#ifdef DEBUG_GEOUTILS
/****************************************************************/
void
PrintBox(
            char *          hdr,
            XmKidGeometry   box)
/****************
 *
 ****************/
{
/****************/
    printf( "%sw: %X, m: 0x%X, x: %d, y: %d, w: %d, h: %d, b: %d\n",
                  hdr, box->kid, box->box.request_mode, box->box.x, box->box.y,
                      box->box.width, box->box.height, box->box.border_width) ;
    return ;
    }
/****************************************************************/
void
PrintList(
            char *          hdr,
            XmKidGeometry   listPtr)
/****************
 *
 ****************/
{
            int             num ;
            char            subhdr[256] ;
/****************/

    num = 0 ;
    while(    listPtr->kid    )
    {   sprintf( subhdr, "%si: %d ", hdr, num) ;
        PrintBox( subhdr, listPtr) ;
        ++num ;
        ++listPtr ;
        }
    return ;
    }

/****************************************************************/
void
PrintMatrix(
            char *          hdr,
            XmGeoMatrix     spec)
/****************
 *
 ****************/
{
            int             row ;
            int             col ;
            XmKidGeometry   boxPtr ;
            XmGeoRowLayout  layoutPtr ;
            char            subhdr[256] ;
/****************/
    row = 1 ;
    boxPtr = spec->boxes ;
    layoutPtr = spec->layouts.row ;
    while(    !(layoutPtr->end)    )
    {   col = 1 ;
        while(    boxPtr->kid    )
        {   sprintf( subhdr, "%srow: %d, col: %d, ", hdr, row, col) ;
            PrintBox( subhdr, boxPtr) ;
            ++col ;
            ++boxPtr ;
            }
        ++row ;
        ++boxPtr ;
        ++layoutPtr ;
        }
    return ;
    }
/****************************************************************/
#endif /* DEBUG_GEOUTILS */
/****************************************************************/
