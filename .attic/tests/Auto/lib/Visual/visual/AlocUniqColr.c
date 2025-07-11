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
static char rcsid[] = "$XConsortium: AlocUniqColr.c /main/7 1995/07/14 11:46:20 drk $"
#endif
#endif
/***********************************************************************
  	@(#)AlocUniqColr.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            This function is used for providing simple "hard-wired" color index
        values for use during the visual generate run of a test.  It keeps 
        track of the lowest unique color index values and which ones are 
        already in use so that new additional color index values may
        be obtained at any time.

        INPUTS
            none
 
        OUTPUTS
            none
 
        RETURNS
            none
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"

#define COLOR	256
#define MONO 	2
#define GRAY	5

Pixel mvsAllocUniqueColor(widget_info,color_ref)
MvsWidgetInfoRecord *widget_info;
int color_ref;
{
    Pixel i;

/* 
   Changed for Automation. No longer allocate into global array
   mvsUniqueColorAllocInfo, now each widget will carry around a
   representation of this array and the comparison and generation
   routines will be modified.

 */

/*

    for (i=0; i<MAX_COLORS; i++) {
       	 if (mvsUniqueColorAllocInfo[i].in_use == 0)
            break;
    }

    if (i == MAX_COLORS) {
        AutoError(_AutoMessages[VISMSG23]);
        
    }

    mvsUniqueColorAllocInfo[i].in_use = 1;
    mvsUniqueColorAllocInfo[i].widget_info = widget_info;
    mvsUniqueColorAllocInfo[i].color_ref = color_ref;

*/

/* 
   Check for MonoChrome server an if so only return 0 or 1. If
   color return anything up to 256 and then cycle through colors
   again. If GRAY scale, retrun 0-5 and cycle through the rest.
*/


    if (mvsVisualInfo.depth == 1) 
    	return(color_ref % MONO);    
    else if ((mvsVisualInfo.class == StaticGray) ||
             (mvsVisualInfo.class == GrayScale))
	return(color_ref % GRAY);
    else
	return(color_ref % COLOR);


} /* End mvsAllocUniqueColor() */
