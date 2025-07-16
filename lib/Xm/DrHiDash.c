/* $XConsortium: DrHiDash.c /main/5 1995/07/15 20:50:42 drk $ */
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
 *
 */
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include "DrawI.h"

/****************************_XmDrawHighlight***************************
 *
 * This function modifies the given gc, which therefore needs to be created
 *   using XCreateGC or XtAllocateGC.
 *
 ***************************************************************************/
void _XmDrawHighlight(Display *display, Drawable d,
			  GC gc,
                          Position x, Position y,
			  Dimension width, Dimension height,
			  Dimension highlight_thickness,
                   int line_style)
{
   XSegment seg[4];
   register Dimension half_hl = highlight_thickness/2 ;
   register Dimension cor = highlight_thickness % 2 ;
   XGCValues gcvalues;

   if (!d || !highlight_thickness || !width || !height) return ;

   /* the XmList dash case relies on this particular order of X segments */

   seg[0].x1 = seg[2].x1 = x ;
   seg[0].y1 = seg[0].y2 = y + half_hl ;
   seg[0].x2 = x + width - highlight_thickness ;
   seg[1].x1 = seg[1].x2 = x + width - half_hl - cor;
   seg[1].y1 = seg[3].y1 = y ;
   seg[3].y2 = y + height - half_hl;
   seg[2].y1 = seg[2].y2 = y + height - half_hl - cor;
   seg[3].x1 = seg[3].x2 = x + half_hl ;
   seg[2].x2 = x + width ;
   seg[1].y2 = y + height ;

   /* first save the current values we want to change */
   XGetGCValues(display, gc,
		GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,
		&gcvalues);
   /* change them and draw the lines */
   XSetLineAttributes(display, gc,  highlight_thickness, line_style,
		      CapButt, JoinMiter);
   XDrawSegments (display, d, gc, seg, 4);

   /* put them back */
   XSetLineAttributes(display, gc,
		      gcvalues.line_width, gcvalues.line_style,
		      gcvalues.cap_style, gcvalues.join_style);

   /** note that the above is a hack, a read-only GC shoudl not
     be modified, period */
}
