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
static char rcsid[] = "$XConsortium: ColorCache.c /main/7 1995/07/14 11:48:38 drk $"
#endif
#endif
/***********************************************************************
  	@(#)ColorCache.c	1.2.1.1	Date:1/22/91
  	Author: PSN
	History:
	   11/12/90 PSN Add to sccs tree
	Calls:

	Summary:
            This file contains the function visLookUpColorCache &
	visAddToColorCache which looks-up local color cache for 
	existance of already looked up pixel, if present use the cache
	value otherwise do XQueryColor for the given pixel and save that
	XColor returned.
	      
        INPUTS
            Pointer to XColor that contains correct pixel value for 
	visLookUpColorCache & visAddToColorCache; but also contains
	other fields correctly initialized for visAddToColorCache.

        OUTPUTS
            XColor initialized by visLookUpColorCache.
	    Cache updated by visAddToColorCache.
 
        RETURNS
            <0 failed, >=0 succeeded for visLookUpColorCache.
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"

int visLookUpColorCache(color)
XColor	*color;
{
    int	   i;
    Pixel  pixel = color->pixel;

    for (i=0; i<cache_pointer && i<MAX_COLORS; i++)
	if (color_cache[i].pixel == pixel) {
	   *color = color_cache[i];
	   return (i);
	}
    return(-1);

} /* End visLookUpColorCache() */

void visAddToColorCache(color)
XColor	*color;
{
    if (cache_pointer < MAX_COLORS)
	color_cache[cache_pointer++] = *color;
    else
      AutoMessage(_AutoMessages[WARNMSG60]);

} /* End visAddToColorCache() */

/* 
 * Clear cache pointer so that it effectively clears the color cache
 */
void visClearColorCache()
{
    cache_pointer = 0;
} /* End visClearColorCache() */



