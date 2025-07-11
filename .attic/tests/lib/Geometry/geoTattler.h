/* $XConsortium: geoTattler.h /main/4 1995/07/15 21:13:47 drk $ */
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
/*
 * HISTORY
 */

#define XtNgeoTattler "geoTattler"
#define XtCGeoTattler "GeoTattler"

/* not found anywhere in Xt, so define this one */
#define XtClassName(w) XtClass(w)->core_class.class_name



#if NeedFunctionPrototypes
extern Boolean _GeoIsTattled (Widget widget);
extern Boolean _GeoIsCached (Widget widget, Boolean* is_geotattled);
extern void _GeoChangeCache (Widget widget, Boolean geo_tat);
extern void _GeoCache (Widget widget, Boolean geo_tat);
extern void _GeoTabTrace (void);
extern void _GeoUnTabTrace (void);
extern void _GeoPrintTab (void);
#else
extern Boolean _GeoIsTattled ();
extern Boolean _GeoIsCached ();
extern void _GeoChangeCache ();
extern void _GeoCache ();
extern void _GeoTabTrace ();
extern void _GeoUnTabTrace ();
extern void _GeoPrintTab ();
#endif

#if NeedVarargsPrototypes
extern void _GeoPrintTrace (Widget widget, ...);
#else
extern void _GeoPrintTrace ();
#endif



/* some useful macros */

#define GMode(g)	    ((g)->request_mode)
#define IsX(g)		    (GMode (g) & CWX)
#define IsY(g)		    (GMode (g) & CWY)
#define IsWidth(g)	    (GMode (g) & CWWidth)
#define IsHeight(g)	    (GMode (g) & CWHeight)
#define IsBorder(g)	    (GMode (g) & CWBorderWidth)
#define IsWidthHeight(g)    ((GMode (g) & CWWidth) || (GMode (g) & CWHeight))
#define IsQueryOnly(g)      (GMode (g) & XtCWQueryOnly)
