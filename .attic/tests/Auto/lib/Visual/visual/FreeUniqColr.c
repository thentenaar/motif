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
static char rcsid[] = "$XConsortium: FreeUniqColr.c /main/7 1995/07/14 11:49:43 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FreeUniqColr.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Frees up one of the unique color index values for pixmap
        generate mode.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
            nothing
************************************************************************/
#include <AutoMessages.h>
#include "vislib.h"

/* 
   Changed for Automation to look at widgetUniqueColorInfo from the
   widget (via the widget_info struct, and not the global array
   (now not used)
*/

void mvsFreeUniqueColor(winfo, pixel)
MvsWidgetInfoRecord *winfo;
Pixel pixel;
{
  char msg_string[125];
  
    if (pixel >= MAX_COLORS) {
       sprintf(msg_string, _AutoMessages[VISMSG36], pixel);
       AutoMessage(msg_string); } 
    else {
        if (winfo->widgetUniqueColorInfo[pixel].in_use == 0){
       sprintf(msg_string, _AutoMessages[VISMSG37], pixel);
       AutoMessage(msg_string); } 

        winfo->widgetUniqueColorInfo[pixel].in_use = 0;
        winfo->widgetUniqueColorInfo[pixel].widget_info = NULL;
        winfo->widgetUniqueColorInfo[pixel].color_ref = 0;
    }
} /* End mvsFreeUniqueColor() */
