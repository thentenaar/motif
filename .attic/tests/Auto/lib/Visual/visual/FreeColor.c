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
static char rcsid[] = "$XConsortium: FreeColor.c /main/7 1995/07/14 11:49:33 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FreeColor.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            This function frees up an allocated pixel value
************************************************************************/
#include <AutoMessages.h>
#include "vislib.h"

void mvsFreeColor(pixel)
Pixel pixel;
{
    int j;
    Pixel pixels[1];
    char msg_string[125];
 
    for (j=0; j<MAX_COLORS && mvsColorAllocInfo[j].pixel != pixel; j++);

    if (j < MAX_COLORS && j > 1 /* first 2 are black and white */) {
        mvsColorAllocInfo[j].alloc_cnt--;
        if (mvsColorAllocInfo[j].alloc_cnt < 0) {
	    sprintf(msg_string, _AutoMessages[VISMSG48], pixel);
	    AutoMessage(msg_string);
            mvsColorAllocInfo[j].alloc_cnt = 0;
        }
        else if (mvsColorAllocInfo[j].alloc_cnt == 0) {
            pixels[0] = pixel;
            XFreeColors(visDisplay,mvsVisualInfo.colormap_id,pixels,1,0);
        }
    }
   

} /* End mvsFreeColor() */

