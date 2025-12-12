/* $XConsortium: GetPixData.c /main/6 1995/10/25 20:05:46 cde-sun $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/XmP.h>
#include "ImageCachI.h"
#include "XmI.h"

/*******************************************************************
 *
 * XmeGetPixmapData.
 *   see if this pixmap is in the cache. If it is then return all the
 *   gory details about it. If not put it in the cache first.
 *   The RETURN pointers can be NULL.
 *   This function never fails (unless pixmap bad id) and always returns
 *   valid information for depth, with, height, etc .
 *   It returns True if the pixmap was in the cache to start with
 *   and True if it had to fetch it to the server first.
 *
 *******************************************************************/
Boolean 
XmeGetPixmapData(
    Screen *screen,
    Pixmap pixmap,
    char **image_name,
    int *depth,
    Pixel *foreground,
    Pixel *background,
    int *hot_x,
    int *hot_y,
    unsigned int *width,
    unsigned int *height)
{
    char *loc_image_name;
    int loc_depth;
    Pixel loc_foreground;
    Pixel loc_background;
    int loc_hot_x;
    int loc_hot_y;
    unsigned int loc_width = 0;
    unsigned int loc_height;
    XtAppContext app;
    
    app = XtDisplayToApplicationContext(DisplayOfScreen(screen));
    
    _XmAppLock(app);
    
    /* support passed NULL argument */
    if (!image_name) image_name = &loc_image_name ;
    if (!depth) depth = &loc_depth ;
    if (!background) background = &loc_background ;
    if (!foreground) foreground = &loc_foreground ;
    if (!hot_x) hot_x = &loc_hot_x ;
    if (!hot_y) hot_y = &loc_hot_y ;
    if (!width) width = &loc_width ;
    if (!height) height = &loc_height ;

    if (_XmGetPixmapData(screen, pixmap, image_name, depth, foreground,
			 background, hot_x, hot_y, width, height)) {
	_XmAppUnlock(app);
	return True ;
    }

    
    /* not in the cache, generate an incomplete entry in the pixmap cache */
    /* Use a magic name, which will have _XmCachePixmap not cache this
       one in the pixmap_data name based cache */

    if (_XmCachePixmap(pixmap, screen, DIRECT_PIXMAP_CACHED, 1, 0, 0, 0, 0)) {
	/* and query again */
	_XmGetPixmapData(screen, pixmap, image_name, depth, foreground,
			 background, hot_x, hot_y, width, height);
    }

    _XmAppUnlock(app);

    return (False);
}

