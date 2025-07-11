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
static char rcsid[] = "$XConsortium: mvsGetLcRsc.c /main/7 1995/07/14 11:21:03 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsGetLcRsc.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS add composite changes
	   07/10/90 PSN Add new function mvsGetParentResource()
	Calls:

	Summary
		This function obtains the value of a resource according
	to what MVS believes it currently to be. This may be different
	than what is really true.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

MvsArgVal mvsGetLocalResource(widget_info, resource_name)
MvsWidgetInfoRecord *widget_info;
char resource_name[];
{
    int i;
    short num_resources;
    MvsResourceInfoRecord *resource_info;
    char msg_string[125];

    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;

    for(i=0;(i<num_resources)&&strcmp(resource_name,resource_info[i].name);i++);

    if (i >= num_resources) {
        sprintf(msg_string, _AutoMessages[WARNMSG116], resource_name);
	AutoMessage (msg_string);
        return(0);
    }
    return( widget_info->mvs_resources[i] );        

} /* End mvsGetLocalResource() */


/************************************************************************
        mvsGetParentResource()	Date:1/22/91
  	Author: PSN
	History:
	   07/10/90 PSN Add new function mvsGetParentResource()
	Calls:

	Summary:
           Gets the resource specified from the parent of the current
        widget info. Returns zero on failure
************************************************************************/

MvsArgVal mvsGetParentResource(widget_info, resource_name)
MvsWidgetInfoRecord *widget_info;
char resource_name[];
{
    MvsWidgetInfoRecord *pwidget_info = widget_info->parent;
    char msg_string[125];

    if (pwidget_info == NULL) {
        sprintf (msg_string, _AutoMessages[WARNMSG70], resource_name);
	AutoMessage(msg_string);
        return(0);
    }

    return( mvsGetLocalResource(pwidget_info, resource_name) );

} /* End mvsGetParentResource() */
