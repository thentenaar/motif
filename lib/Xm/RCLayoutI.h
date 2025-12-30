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

/* $XConsortium: RCLayoutI.h /main/5 1995/07/13 17:44:58 drk $ */
#ifndef _XmRCLayoutI_h
#define _XmRCLayoutI_h

#include <Xm/RCLayoutP.h>

#ifdef __cplusplus
extern "C" {
#endif


/********    Private Function Declarations    ********/

extern void _XmRCDoMarginAdjustment(
                        XmRowColumnWidget m) ;
extern void _XmRCThinkAboutSize(
                        XmRowColumnWidget m,
                        Dimension *w,
                        Dimension *h,
                        Widget instigator,
                        XtWidgetGeometry *request) ;
extern void _XmRCPreferredSize(
                        XmRowColumnWidget m,
                        Dimension *w,
                        Dimension *h) ;
extern void _XmRCAdaptToSize(
                        XmRowColumnWidget m,
                        Widget instigator,
                        XtWidgetGeometry *request) ;
extern XmRCKidGeometry _XmRCGetKidGeo(
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *request,
                        int uniform_border,
                        Dimension border,
                        int uniform_width_margins,
                        int uniform_height_margins,
                        Widget help,
			Widget toc,
                        int geo_type) ;
extern void _XmRCSetKidGeo(
                        XmRCKidGeometry kg,
                        Widget instigator) ;

extern void _XmRC_SetOrGetTextMargins(
                        Widget wid,
                        unsigned char op,
                        XmBaselineMargins *textMargins) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}
#endif
#endif /* _XmRCLayoutI_h */

