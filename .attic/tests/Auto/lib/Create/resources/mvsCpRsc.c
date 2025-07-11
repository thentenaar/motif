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
static char rcsid[] = "$XConsortium: mvsCpRsc.c /main/7 1995/07/14 11:20:13 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsCpRsc.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree.
	   05/29/90 SJS changes for composites
	   07/29/90 PSN parameter changes, memory fixes
	Calls:

	Summary:
		This file contains function which copy resources.
	The method of copying is based on the type of each resource and
	a pointer/value is returned based on this type.
************************************************************************/

#include "mvslib.h"

/************************************************************************
*   This function copies One resource. The resource is copied           *
*   based on its type and a pointer/value is returned.                  *
************************************************************************/
MvsArgVal mvsCopyResource(widget_class_info, src_resource, i) 
MvsWidgetClassInfo *widget_class_info;
MvsArgVal src_resource; 
int i;
{ 
    MvsArgVal dst_resource;
    MvsResourceInfoRecord *resource_info;

    resource_info = widget_class_info->resource_info;

    switch (mvsTypeInfo[resource_info[i].type_code].ref_type) {

	    case RefXmStringTable: /* Here until handling of compound strings */
	                           /* becomes clear */
	    case RefValue: 	 
            case RefGeneric:
            case RefBoolean: 
                dst_resource = src_resource; 
                break;

	    case RefXmString:
	    case RefString: 
            { 
                char *str = (char *)src_resource; 
                if (str != NULL) { 
                    dst_resource = (MvsArgVal)XtMalloc(strlen(str)+1);
                    strcpy((char *)dst_resource,str); 
                } 
                else dst_resource = 0L; 
                break; 	 
            }

	    case RefCallbackList: 	 
            { 
                XtCallbackList cb_list = (XtCallbackList)src_resource; 
                int j=0; 
                if (cb_list != NULL) { 
                    while (cb_list->callback != NULL) { 
                        cb_list++; j++; 
                    } 
                    cb_list = (XtCallbackList)src_resource; 
                    dst_resource = (MvsArgVal)XtMalloc(sizeof(XtCallbackRec)*(j+1));
                    memcpy((char *)dst_resource,cb_list,sizeof(XtCallbackRec)*(j+1));
                }
                else dst_resource = 0L;
                break;
   	    } 

	    default: 
                dst_resource = src_resource; 
                msg_error("bad resource type code=%d in mvsCopyResource\n",
                          mvsTypeInfo[resource_info[i].type_code].ref_type);
                break;
    }

    return (dst_resource);
} /* End mvsCopyResource() */


/*************************************************************************
*  This function copies all resources. The entire list of resources is   *
* looped through and each resource is copied using mvsCopyResource().    *
*************************************************************************/
void mvsCopyResources(widget_class_info, dst_resource,src_resource)
MvsWidgetClassInfo *widget_class_info;
MvsArgVal dst_resource[],
src_resource[]; 
{ 
    int i;

    for (i=0; i<widget_class_info->num_resources; i++)
        dst_resource[i] = mvsCopyResource(widget_class_info, src_resource[i],i);

} /* End mvsCopyResource() */

