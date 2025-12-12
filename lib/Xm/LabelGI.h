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
/* $XConsortium: LabelGI.h /main/5 1995/07/13 17:31:31 drk $ */
#ifndef _XMLABELGI_H
#define _XMLABELGI_H

#include <Xm/LabelGP.h>
#include <Xm/MenuT.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern int _XmLabelCacheCompare(
                        XtPointer A,
                        XtPointer B) ;
extern void _XmCalcLabelGDimensions(
                        Widget wid) ;
extern void _XmReCacheLabG(
                        Widget wid) ;
extern void _XmAssignLabG_MarginHeight(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmLabelGCalcTextRect(
                        Widget wid) ;
extern void _XmAssignLabG_MarginWidth(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmAssignLabG_MarginLeft(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmAssignLabG_MarginRight(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmAssignLabG_MarginTop(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmAssignLabG_MarginBottom(
                        XmLabelGadget lw,
                        Dimension value) ;
extern void _XmProcessDrag(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern Boolean _XmLabelGCVTRedraw (Widget kid,
				   Widget cur_parent,
				   Widget new_parent,
				   Mask visual_flag);

extern void _XmRedisplayLabG (Widget      w,
			      XEvent     *event,
			      Region      region,
			      LRectangle *background_box);

extern void _XmLabelGCloneMenuSavvy(WidgetClass, XmMenuSavvyTrait);

extern void _XmLabelSetBackgroundGC(XmLabelGadget lw);
extern void _XmLabelGCalcTextRect(Widget wid);
/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XMLABELGI_H */
