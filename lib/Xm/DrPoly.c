/* $XConsortium: DrPoly.c /main/6 1995/10/25 20:00:09 cde-sun $ */
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
 *
 */
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/DrawP.h>
#include "RegionI.h"
#include "XmI.h"

/****************************XmeDrawPolygonShadow***************************/
void
XmeDrawPolygonShadow (
		      Display *dpy,
		      Drawable d,
		      GC topGC,
		      GC bottomGC,
		      XPoint *points,
		      int n_points,
		      Dimension shadowThickness,
		      unsigned char shadowType)
{
    Region  xregion;
    XtAppContext app;

    app = XtDisplayToApplicationContext(dpy);

    _XmAppLock(app);
    xregion = XPolygonRegion(points, n_points, /* FillRule */ WindingRule);

    _XmRegionDrawShadow (dpy, d,
			 topGC, bottomGC,
			 (XmRegion)xregion,
			 /* border_width */ 0, shadowThickness,
			 shadowType);

    XDestroyRegion(xregion);
    _XmAppUnlock(app);
}
