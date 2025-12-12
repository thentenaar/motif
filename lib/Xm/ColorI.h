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
/* $XConsortium: ColorI.h /main/6 1995/07/14 10:15:47 drk $ */
#ifndef _XmColorI_h
#define _XmColorI_h

#include <Xm/ColorP.h>
#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for Color.c    ********/

extern void _XmForegroundColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmHighlightColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmBackgroundColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmTopShadowColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmBottomShadowColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmSelectColorDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern Boolean _XmSearchColorCache(
                        unsigned int which,
                        XmColorData *values,
                        XmColorData **ret) ;
extern XmColorData * _XmAddToColorCache(
                        XmColorData *new_rec) ;
extern Pixmap _XmConvertToBW(
                        Widget w,
                        Pixmap pm) ;
/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmColorI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
