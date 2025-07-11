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
static char rcsid[] = "$XConsortium: AllocWidInfo.c /main/7 1995/07/14 11:24:11 drk $"
#endif
#endif
/***********************************************************************
  	@(#)AllocWidInfo.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/18/90 SJS add to SCCS
            06/21/90 SJS add being_destroyed & convenience parent
	Calls:

	Summary:
            Allocates a widget info record and connects the pointers in the
            widget info hierarchy appropriately. This function does not
            perform color resource set up. (See mvsSetupWidgetInfo() )

            INPUTS
                widget_class_info  - Pointer to the mvs widget class info
                                     record that
                                     you want this widget info record to point
                                     to

                parent_widget_info - Pointer to the parent widget info record

                widget             - The Xt widget id
  
            OUTPUTS
                none

            RETURNS
                Pointer to newly alloc'ed widget info record.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

MvsWidgetInfoRecord *mvsAllocWidgetInfo(wc_info, pinfo, widget)
MvsWidgetClassInfo *wc_info;
MvsWidgetInfoRecord *pinfo;
Widget widget;
{
    int i,j;
    MvsWidgetInfoRecord *winfo;

    for (i=1; i<=mvsNumWidgetInfos; i++) {
        if (i >= MAX_NUM_WIDGET_INFOS) {
            AutoMessage(_AutoMessages[WARNMSG120]);
            exit(-1);
        }

        if (!mvsWidgetInfos[i].in_use) {
            mvsWidgetInfos[i].in_use = 1;
            mvsWidgetInfos[i].being_destroyed = False;
            mvsWidgetInfos[i].c_parent = False;

            if (i == mvsNumWidgetInfos)
                mvsNumWidgetInfos++;

            mvsWidgetInfos[i].parent = pinfo;
            if (pinfo != NULL) {
                winfo = mvsWidgetInfos[i].parent->first_child;
                if (winfo == NULL)
                    mvsWidgetInfos[i].parent->first_child = &mvsWidgetInfos[i];
                else {
                    while (winfo->next_sibling != NULL)
                        winfo = winfo->next_sibling;
                    winfo->next_sibling = &mvsWidgetInfos[i];
                }
	    }

            mvsWidgetInfos[i].widget = widget;
            mvsWidgetInfos[i].widget_class_info = wc_info;
            mvsWidgetInfos[i].callbacks_setup = 0;
            mvsWidgetInfos[i].first_child = NULL;
            mvsWidgetInfos[i].next_sibling = NULL;


            if (wc_info != NULL) {
                for (j=0; j<wc_info->num_res_colors; j++) {
                    mvsWidgetInfos[i].unique_color_ref[j] 
                        	= mvsAllocUniqueColor(&mvsWidgetInfos[i],j);
/* Automation Added */
		    mvsWidgetInfos[i].widgetUniqueColorInfo[j].in_use 
				= True;
		    mvsWidgetInfos[i].widgetUniqueColorInfo[j].widget_info 
				= &mvsWidgetInfos[i];
		    mvsWidgetInfos[i].widgetUniqueColorInfo[j].color_ref 
				= j;
/* End Automation Added */
		}
            }

            break;
	}
    } /* End for(i<=mvsNumWidgetInfos) */

    return (&mvsWidgetInfos[i]);

} /* End mvsAllocWidgetInfo() */
