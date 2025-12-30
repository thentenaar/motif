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

/* $XConsortium: GeoUtilsI.h /main/5 1995/07/13 17:28:18 drk $ */
#ifndef _XmGeoUtilsI_h
#define _XmGeoUtilsI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for GeoUtils.c    ********/

extern XtGeometryResult _XmHandleQueryGeometry(
                        Widget wid,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *desired,
                        unsigned char policy,
                        XmGeoCreateProc createMatrix) ;
extern XtGeometryResult _XmHandleGeometryManager(
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *desired,
                        XtWidgetGeometry *allowed,
                        unsigned char policy,
                        XmGeoMatrix *cachePtr,
                        XmGeoCreateProc createMatrix) ;
extern void _XmHandleSizeUpdate(
                        Widget wid,
                        unsigned char policy,
                        XmGeoCreateProc createMatrix) ;
extern XmGeoMatrix _XmGeoMatrixAlloc(
                        unsigned int numRows,
                        unsigned int numBoxes,
                        unsigned int extSize) ;
extern void _XmGeoMatrixFree(
                        XmGeoMatrix geo_spec) ;
extern Boolean _XmGeoSetupKid(
                        XmKidGeometry geo,
                        Widget kidWid) ;
extern void _XmGeoMatrixGet(
                        XmGeoMatrix geoSpec,
                        int geoType) ;
extern void _XmGeoMatrixSet(
                        XmGeoMatrix geoSpec) ;
extern void _XmGeoAdjustBoxes(
                        XmGeoMatrix geoSpec) ;
extern void _XmGeoGetDimensions(
                        XmGeoMatrix geoSpec) ;
extern void _XmGeoArrangeBoxes(
                        XmGeoMatrix geoSpec,
                        Position x,
                        Position y,
                        Dimension *pW,
                        Dimension *pH) ;
extern Dimension _XmGeoBoxesSameWidth(
                        XmKidGeometry rowPtr,
                        Dimension width) ;
extern Dimension _XmGeoBoxesSameHeight(
                        XmKidGeometry rowPtr,
                        Dimension height) ;
extern void _XmSeparatorFix(
                        XmGeoMatrix geoSpec,
                        int action,
                        XmGeoMajorLayout layoutPtr,
                        XmKidGeometry rowPtr) ;
extern void _XmMenuBarFix(
                        XmGeoMatrix geoSpec,
                        int action,
                        XmGeoMajorLayout layoutPtr,
                        XmKidGeometry rowPtr) ;
extern void _XmGeoLoadValues(
                        Widget wid,
                        int geoType,
                        Widget instigator,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *geoResult) ;
extern int _XmGeoCount_kids(
                        CompositeWidget c) ;
extern XmKidGeometry _XmGetKidGeo(
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *request,
                        int uniform_border,
                        Dimension border,
                        int uniform_width_margins,
                        int uniform_height_margins,
                        Widget help,
                        int geo_type) ;
extern void _XmGeoClearRectObjAreas(
                        RectObj r,
                        XWindowChanges *old) ;
extern void _XmSetKidGeo(
                        XmKidGeometry kg,
                        Widget instigator) ;
extern Boolean _XmGeometryEqual(
                        Widget wid,
                        XtWidgetGeometry *geoA,
                        XtWidgetGeometry *geoB) ;
extern Boolean _XmGeoReplyYes(
                        Widget wid,
                        XtWidgetGeometry *desired,
                        XtWidgetGeometry *response) ;
extern XtGeometryResult _XmMakeGeometryRequest(
                        Widget w,
                        XtWidgetGeometry *geom) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmGeoUtilsI_h */
