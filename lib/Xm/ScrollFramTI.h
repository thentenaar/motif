/* $XConsortium: ScrollFramTI.h /main/5 1995/07/15 20:55:16 drk $ */
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
#ifndef _XmScrollFrameTI_h
#define _XmScrollFrameTI_h

#include <Xm/Xm.h>
#include <Xm/ScrollFrameT.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern void _XmSFAddNavigator(
			      Widget sf,
			      Widget nav,
			      Mask dimMask,
			      XmScrollFrameData scroll_frame_data);
extern void _XmSFRemoveNavigator(
				 Widget sf,
				 Widget nav,
				 XmScrollFrameData scroll_frame_data);
extern void _XmSFUpdateNavigatorsValue(
				       Widget sf,
				       XmNavigatorData nav_data,
				       Boolean notify);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScrollFrameTI_h */
