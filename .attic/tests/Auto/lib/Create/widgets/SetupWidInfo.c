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
static char rcsid[] = "$XConsortium: SetupWidInfo.c /main/7 1995/07/14 11:25:11 drk $"
#endif
#endif
/***********************************************************************
  	@(#)SetupWidInfo.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   06/17/90 TAT Add to sccs tree
	   07/31/90 PSN Fix for memory leaks
	Calls:

	Summary:
            Allocates and sets up an additional widget info record.

            INPUTS
                parent_info      - parent widget info
                widget           - Xt widget
                class_info       - class_info of widget

            OUTPUTS
                none

            RETURNS
                widget_info_record
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

#define NUMARGS	50

MvsWidgetInfoRecord *mvsSetupNewWidgetInfo(parent_info,widget,class_info)
MvsWidgetInfoRecord *parent_info;
Widget widget;
MvsWidgetClassInfo *class_info;
{
    MvsWidgetInfoRecord *widget_info;
    MvsResourceInfoRecord *resource_info;
    short *res_color_ref;
    short *res_pixmap_ref;
    short *unique_color_ref;
    int num_res_colors;
    MvsArgVal *mvs_resources;
    int widget_class_code;

    Arg args[NUMARGS];
    int nargs;
    int i,j;

    if (widget == NULL || parent_info == NULL)
        return NULL;

    for (i=0; i<NUMARGS; i++)	args[i].value = 0L;

    widget_info = mvsAllocWidgetInfo(class_info,parent_info,widget);
    widget_info->window = XtWindowOfObject(widget);
    resource_info = class_info->resource_info;
    res_color_ref = class_info->res_color_ref;
    res_pixmap_ref = class_info->res_pixmap_ref;
    unique_color_ref = widget_info->unique_color_ref;
    num_res_colors = class_info->num_res_colors;
    mvs_resources = widget_info->mvs_resources;

    mvsLoadResources(widget_info,mvs_resources);

    nargs = 0;
    if (mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS) {
        for (j=0; j<num_res_colors; j++) {
            if (res_pixmap_ref[j] >= 0 && 
                mvs_resources[res_pixmap_ref[j]] != XmUNSPECIFIED_PIXMAP) {
/* Automation Fix */
/*                XtSetArg(args[nargs],resource_info[res_pixmap_ref[j]].name,
                         XmUNSPECIFIED_PIXMAP);*/
/* End Automation Fix */
/*                nargs++;*/
	    }
	    if (res_color_ref[j] >= 0) {
		XtSetArg(args[nargs],resource_info[res_color_ref[j]].name,
			 unique_color_ref[j]);
		nargs++;
	    }
	}

        /* Make mvsSetLocalValues store color arguments locally */
        mvsSetVisualMode(VISUAL_MODE_DONT_CHECK);
        mvsSetValuesNoCheck(widget_info,args,nargs);
        mvsSetVisualMode(VISUAL_MODE_GENERATE_PIXMAPS);
    }
    else {
        for (j=0; j<num_res_colors; j++) {
	    if (res_color_ref[j] >= 0)
		if (!mvsAllocColor(mvs_resources[res_color_ref[j]])) {
		    AutoMessage(_AutoMessages[WARNMSG39]);
		}
        }
    }    

    return widget_info;

} /* End mvsSetupNewWidgetInfo() */
