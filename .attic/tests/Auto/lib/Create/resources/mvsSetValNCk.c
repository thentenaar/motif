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
static char rcsid[] = "$XConsortium: mvsSetValNCk.c /main/7 1995/07/14 11:23:18 drk $"
#endif
#endif
/***********************************************************************
	Calls:

	Summary:
		This function sets the requested resources to the
	desired values. No checking is done here. (In other words,
	there is no check to determine if the changes were actually made)
************************************************************************/
#include "mvslib.h"

void mvsSetValuesNoCheck(widget_info,args,nargs)
MvsWidgetInfoRecord *widget_info;
ArgList args;
Cardinal nargs;
{
    int i,j;
    XtCallbackList cb_list;
    int t;
    short num_resources;
    MvsResourceInfoRecord *resource_info;
    Widget widget;
    MvsArgVal *mvs_resources;

    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;
    widget = widget_info->widget;
    mvs_resources = widget_info->mvs_resources;

    for(i=0; i<nargs; i++) {
        for (j=0;
             (j<num_resources)&&strcmp(args[i].name,resource_info[j].name);
             j++);

        if (j<num_resources) {
            t = resource_info[j].type_code;
            switch (mvsTypeInfo[t].ref_type) {

            case RefCallbackList:
                break;

	    case RefValue:
                break;

	    default:
                break;
	    }
        }
    } /* for() */

    XtSetValues(widget,args,nargs);

    mvsSetLocalValues(widget_info,args,&nargs);

} /* End mvsSetValuesNoCheck() */
