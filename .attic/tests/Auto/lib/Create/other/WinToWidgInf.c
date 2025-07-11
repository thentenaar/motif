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
static char rcsid[] = "$XConsortium: WinToWidgInf.c /main/7 1995/07/14 11:18:02 drk $"
#endif
#endif
/***********************************************************************
  	@(#)WinToWidgInf.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to sccs
	Calls:

	Summary:
            Looks up the widget_info record associated with the specified window

            INPUTS
                window  - window to match

            OUTPUTS
                none

            RETURNS
                Pointer to widget_info record associated with the specified
                window
************************************************************************/

#include "mvslib.h"

MvsWidgetInfoRecord *mvsWindowToWidgetInfo(window)
Window window;
{
    MvsWidgetInfoRecord *widget_info = mvsRootWidgetInfo->first_child;
    MvsWidgetInfoRecord *widget_info_stack[100];
    short stack_top = 0;

    while ( widget_info != mvsRootWidgetInfo && 
            window != XtWindowOfObject(widget_info->widget) ) {

        if (widget_info->first_child != NULL) {
            widget_info_stack[stack_top++] = widget_info;
            widget_info = (MvsWidgetInfoRecord *)widget_info->first_child;
        }
        else 
            if (widget_info->next_sibling != NULL)
                widget_info = (MvsWidgetInfoRecord *)widget_info->next_sibling;
            else {
                widget_info = NULL;
                while (stack_top > 0 && widget_info == NULL)
                    widget_info = (MvsWidgetInfoRecord *)
                                  widget_info_stack[--stack_top]->next_sibling;

                if (stack_top == 0)
                    break;
            } 
    } /* End while() */

    return widget_info;

} /* End mvsWindowToWidgetInfo() */
