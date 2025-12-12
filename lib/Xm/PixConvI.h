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
/* $XConsortium: PixConvI.h /main/6 1996/01/29 10:08:12 daniel $ */
#ifndef _XmPixConvI_h
#define _XmPixConvI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for PixConv.c    ********/

extern void _XmRegisterPixmapConverters( void ) ;
extern void _XmTopShadowPixmapDefault( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmHighlightPixmapDefault( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern GC _XmGetPixmapBasedGC(
			      Widget w,
			      Pixel foreground,
			      Pixel background,
			      Pixmap pixmap);

/********    End Private Function Declarations    ********/


/* tmp - go to XmStrDef */
#define XmRNoScalingBitmap "NoScalingBitmap"
#define XmRNoScalingDynamicPixmap "NoScalingDynamicPixmap"



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPixConvI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
