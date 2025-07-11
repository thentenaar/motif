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
static char rcsid[] = "$XConsortium: AllocColorSt.c /main/7 1995/07/14 11:45:23 drk $"
#endif
#endif
/***********************************************************************
  	@(#)AllocColorSt.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   09/17/90 SJS split library
	Calls:

	Summary:
            Allocate one unqiue RGB color from the locol color cache.

        INPUTS
            color  - requested RGB color value

        OUTPUTS
            none
 
        RETURNS
            Status where 0=failed, ~0=succeeded
************************************************************************/
#include <AutoMessages.h>
#include "vislib.h"

Status mvsAllocColorStruct(color)
XColor color;
{
    int i;
    Status status;
    
    /* record pixel choice in current_color_info array */
    /* see if pixel is already in color_alloc array */

    for (i=0;
             i<MAX_COLORS &&
             ( mvsColorAllocInfo[i].alloc_cnt == 0     ||
               mvsColorAllocInfo[i].pixel != color.pixel );
         i++);

    if (i != MAX_COLORS) /* Yes it exists. Just increment its allocation cnt */ {
        mvsColorAllocInfo[i].alloc_cnt++;
        status = 1;
    }

    else { /* No, it does not exist. Create a new entry for it */

        /* find next available color_alloc slot */

        for (i=0; i<MAX_COLORS && mvsColorAllocInfo[i].alloc_cnt != 0; i++);
        if (i == MAX_COLORS) {
            AutoMessage(_AutoMessages[VISMSG1]);
            return 0;
        }

        /* transfer info to this slot */

        mvsColorAllocInfo[i].pixel = color.pixel;
        mvsColorAllocInfo[i].red = color.red;
        mvsColorAllocInfo[i].green = color.green;
        mvsColorAllocInfo[i].blue = color.blue;
        mvsColorAllocInfo[i].alloc_cnt = 1;

        status = XAllocColor(visDisplay,mvsVisualInfo.colormap_id,&color);
    }
    return status;

} /* End mvsAllocColorStruct() */













