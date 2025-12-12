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
/* $XConsortium: LabelI.h /main/5 1995/07/13 17:31:52 drk $ */
#ifndef _XmLabelI_h
#define _XmLabelI_h

#include <Xm/LabelP.h>
#include <Xm/MenuT.h>

#ifdef __cplusplus
extern "C" {
#endif


/********    Private Function Declarations    ********/

extern void _XmCalcLabelDimensions(Widget wid) ;
extern void _XmLabelCloneMenuSavvy(WidgetClass, XmMenuSavvyTrait);
extern char* _XmCBNameActivate(void);
extern char* _XmCBNameValueChanged(void);
extern void _XmLabelCalcTextRect(Widget wid) ;


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmLabelI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
