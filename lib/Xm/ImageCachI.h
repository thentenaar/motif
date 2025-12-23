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
/* $XConsortium: ImageCachI.h /main/7 1996/01/29 13:19:43 daniel $ */
#ifndef _XmImageCacheI_h
#define _XmImageCacheI_h

#include <Xm/XmP.h>

/* this name is used by XmeGetPixmapData to cache a pixmap in the
   pixmap cache with no associated name. _XmCachePixmap knows about it
   and will not add this one in the pixmap_data cache because it is a
   _name_ based pixmap cache used during conversion */
#define DIRECT_PIXMAP_CACHED ""

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for ImageCache.c    ********/

extern Boolean _XmInstallImage(
                        XImage *image,
                        char *image_name,
                        int hot_x,
                        int hot_y) ;
extern Boolean _XmGetImage(
                        Screen *screen,
                        char *image_name,
                        XImage **image) ;
extern Boolean _XmCachePixmap(
			      Pixmap pixmap,
			      Screen *screen,
			      char *image_name,
			      Pixel foreground,
			      Pixel background,
			      int depth,
			      Dimension width,
			      Dimension height) ;
extern Pixmap _XmGetColoredPixmap(Screen *screen,
				  char *image_name,
				  XmAccessColorData acc_color,
				  int depth,
				  Boolean only_if_exists) ;

extern Boolean _XmGetPixmapData(
		   Screen *screen,
		   Pixmap pixmap,
		   char **image_name,
		   int *depth,
		   Pixel *foreground,
		   Pixel *background,
		   int *hot_x,
		   int *hot_y,
		   unsigned int *width,
		   unsigned int *height) ;
extern Boolean _XmInImageCache(
			       String image_name);

extern Pixmap _XmGetScaledPixmap(
    Screen *screen,
    Widget widget,
    char *image_name,
    XmAccessColorData acc_color,
    int depth,
    Boolean only_if_exists,
    double scaling_ratio,
    int desired_w,
    int desired_h);

extern void _XmPutScaledImage(Screen *screen, Display *display, Drawable d,
                              int depth, GC gc, XImage *src, int sx, int sy,
                              int sw, int sh, int dx, int dy, int dw, int dh);

extern void _XmCleanPixmapCache(Screen * screen);

/* for Xm.h */
extern Pixmap XmGetScaledPixmap(
    Widget widget,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth,
    double scaling_ratio);

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}
#endif

#endif /* _XmImageCacheI_h */
