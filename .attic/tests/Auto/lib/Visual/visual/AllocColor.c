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
static char rcsid[] = "$XConsortium: AllocColor.c /main/7 1995/07/14 11:45:12 drk $"
#endif
#endif
/***********************************************************************
  	@(#)AllocColor.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/24/90 TAT access global color info
	   11/15/90 PSN make this routine to use local color cache and
			not use time consuming XQueryColor routines.
	Calls:

	Summary:
            This file contains the function mvsAllocColor which
        allocates one unique RGB color from the local color cache.
	      
        INPUTS
            pixel - X colormap index (pixel) value

        OUTPUTS
            none
 
        RETURNS
            Status where 0=failed, ~0=succeeded
************************************************************************/

#include "vislib.h"

Status mvsAllocColor(pixel)
Pixel pixel;
{
    XColor color;
    int	   result;

    color.pixel = pixel;
    if (visLookUpColorCache(&color) < 0) {
	XQueryColor(visDisplay,mvsVisualInfo.colormap_id,&color);
	visAddToColorCache(&color);
    }
    return(mvsAllocColorStruct(color));

} /* End mvsAllocColor() */
