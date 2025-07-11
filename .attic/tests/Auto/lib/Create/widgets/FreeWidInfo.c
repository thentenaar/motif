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
static char rcsid[] = "$XConsortium: FreeWidInfo.c /main/8 1995/07/14 11:24:34 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FreeWidInfo.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/18/90 SJS add to SCCS
            07/25/90 SJS free resources associated with the widget
                         info
            07/29/90 PSN parameter changes
	Calls:

	Summary:
            Frees a widget info record and disconnects the pointers in the
            widget info hierarchy appropriately.

            INPUTS
                widget_info  - Pointer to the mvs widget info record that
                               is to be freed

            OUTPUTS
                none

            RETURNS
                noting
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

void mvsFreeWidgetInfo(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    int i;
    MvsWidgetInfoRecord *winfo;
    
    if (widget_info == NULL || !widget_info->in_use) {
      AutoMessage(_AutoMessages[WARNMSG38]);
    }
    else {
        mvsFreeResources(widget_info->widget_class_info, widget_info->mvs_resources);
        widget_info->in_use = 0;
        if (widget_info->widget_class_info != NULL) {
            for (i=0; i<widget_info->widget_class_info->num_res_colors; i++) {
              mvsFreeUniqueColor(widget_info, i);
            }
        }
  
        if( (widget_info->parent == NULL) ||
            (widget_info->parent->first_child == NULL)) {
            AutoMessage(_AutoMessages[WARNMSG38]);
        }
        else {        
            winfo = widget_info->parent->first_child;
            if (winfo == widget_info) {
                widget_info->parent->first_child = widget_info->next_sibling;
            }
            else {
                while ( winfo->next_sibling != NULL && 
                        winfo->next_sibling != widget_info )
                    winfo = winfo->next_sibling;

                if (winfo == NULL || winfo->next_sibling != widget_info) {
                   AutoMessage(_AutoMessages[WARNMSG86]);
                }
                else {
                    winfo->next_sibling = widget_info->next_sibling;
                }
            }
            for ( winfo=widget_info->first_child; 
                  winfo != NULL; 
                  winfo = winfo->next_sibling ) {
                
                mvsFreeWidgetInfo(winfo);
	    }
	} /* End if (parent == NULL) || parent->first_child == NULL)) */

    } /* End if((widget_info == NULL || !widget_info->in_use) */

} /* End mvsFreeWidgetInfo() */
