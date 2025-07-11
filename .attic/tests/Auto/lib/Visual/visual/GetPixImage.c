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
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: GetPixImage.c /main/7 1995/07/14 11:50:23 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetPixImage.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns the specified pixmap as an image.  This routine goes beyond
        X's XGetImage() by figuring out the pixmaps dimensions for you as well.

        INPUTS
            pixmap - X id of pixmap you wish returned as an X image

        OUTPUTS
            none
 
        RETURNS
            A pointer to a dynamically allocated image (use XFree to free it).
************************************************************************/
static int error_occurred;

#include <AutoMessages.h>
#include "vislib.h"

static int myhandler(display,myerr)
Display *display;
XErrorEvent *myerr;
{
    if ((myerr->error_code == BadMatch)&&(myerr->request_code == X_GetImage))
        error_occurred = TRUE;
}


XImage *mvsGetPixmapImage(pixmap)
Pixmap pixmap;
{
    int i;
    int x,y,width,height;
    static int first_time = 1;

    XImage *image;

    if (first_time) {
        first_time = 0;
        mvsNumAvailPixmaps = 0;
    }

    for (i=0; i<mvsNumAvailPixmaps; i++) {
        if (pixmap == mvsAvailPixmap[i])
            return mvsAvailImage[i];
    }

    XSetErrorHandler(myhandler);

    x=0;
    error_occurred = FALSE;
    while (error_occurred == FALSE) {
        image = XGetImage(visDisplay,pixmap,x++,0,1,1,AllPlanes,ZPixmap);
        if (image != 0)
            XDestroyImage(image);
    }
    width = x-1;

    y=0;

    error_occurred = FALSE;
    while (error_occurred == FALSE) {
        image = XGetImage(visDisplay,pixmap,0,y++,1,1,AllPlanes,ZPixmap);
        if (image != 0)
            XDestroyImage(image);
    }
    height = y-1;

    XSetErrorHandler(NULL);

    if (mvsNumAvailPixmaps > MAX_PIXMAPS)
        AutoMessage(_AutoMessages[VISMSG40]);
    else {
        mvsAvailPixmap[mvsNumAvailPixmaps] = pixmap;

        mvsAvailImage[mvsNumAvailPixmaps] = XGetImage(visDisplay,pixmap,0,0,
                                                      width,
                                                      height,AllPlanes,ZPixmap);
        mvsNumAvailPixmaps++;
    }

} /* End mvsGetPixmapImage() */
