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
*/ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: ReadImage.c /main/15 1996/10/21 11:40:15 cde-osf $"
#endif
#endif

#include "XmI.h"		/* for _XmCreateImage() */
#include "ReadImageI.h"

/************************************************************************
 *
 *  _XmReadImageAndHotSpotFromFile
 *	Given a filename, extract and create an image from the file data.
 *      This one takes a Display.
 ************************************************************************/
XImage * 
_XmReadImageAndHotSpotFromFile(
        Display * display,
        char *filename,
	int *hot_x, 
	int *hot_y)
{
   unsigned int width; 
   unsigned int height;
   unsigned char * data;

   if (BitmapSuccess == XReadBitmapFileData(filename, &width, &height, &data,
			       hot_x, hot_y))
   {
      XImage * image;
      _XmCreateImage(image, display, (char*)data, width, height, LSBFirst);

      return (image);
   }

   return (NULL);
}


