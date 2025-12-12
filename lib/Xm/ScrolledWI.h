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
/* $XConsortium: ScrolledWI.h /main/5 1995/07/13 17:56:39 drk $ */
#ifndef _XmScrolledWI_h
#define _XmScrolledWI_h

#include <Xm/ScrolledWP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SWCEPTR(wc)  ((XmScrolledWindowClassExt *)(&(((XmScrolledWindowWidgetClass)(wc))->swindow_class.extension)))

#define _XmGetScrolledWindowClassExtPtr(wc, owner) \
  ((*SWCEPTR(wc) && (((*SWCEPTR(wc))->record_type) == owner))\
   ? SWCEPTR(wc) \
   :((XmScrolledWindowClassExt *) _XmGetClassExtensionPtr(((XmGenericClassExt *)SWCEPTR(wc)), owner)))


/********    Private Function Declarations    ********/

extern void _XmSWNotifyGeoChange(Widget sw,
				 Widget child,
				 XtWidgetGeometry *request);
extern Boolean _XmSWGetClipArea(Widget widget, 
				XRectangle *rect);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScrolledWI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
