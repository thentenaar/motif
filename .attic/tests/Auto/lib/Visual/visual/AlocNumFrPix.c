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
static char rcsid[] = "$XConsortium: AlocNumFrPix.c /main/7 1995/07/14 11:45:59 drk $"
#endif
#endif
/***********************************************************************
  	@(#)AlocNumFrPix.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Lookup the associated color allocation number given the pixel value.

        INPUTS
            pixel - the X colormap index (pixel) value

        OUTPUTS
            none
 
        RETURNS
            The index into the mvsColorAllocInfo table.
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"

int mvsAllocNumFromPixel(pixel)
Pixel pixel;
{
    int i;
    char msg_string[125];

    for(i=0; i<MAX_COLORS &&
       (mvsColorAllocInfo[i].alloc_cnt==0 || mvsColorAllocInfo[i].pixel!=pixel);
       i++);

    if (i == MAX_COLORS) {
        sprintf (msg_string, _AutoMessages[VISMSG21], pixel);
	AutoMessage(_AutoMessages[WARNMSG21]);
	AutoMessage(_AutoMessages[WARNMSG22]);
	i=0;
    }

    return(i);

} /* End mvsAllocNumFromPixel() */
