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
static char rcsid[] = "$XConsortium: FindCorrRsc.c /main/7 1995/07/14 11:44:44 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FindCorrRsc.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
	Calls:

	Summary:
            This function locates the resource that corresponds to the
        resource that is refered to by index i in the widget's
        resource_info array.

	The index of the corresponding resource in the resource_info 
        array is returned if found. Otherwise, return (-1)
************************************************************************/
#include "vislib.h"

int mvsFindCorrespondingResource(widget_class_info, i, remove_str, add_str)
MvsWidgetClassInfo *widget_class_info;
int i;
char remove_str[], add_str[];
{
    int k;
    char name[MAX_RESOURCE_NAME_LENGTH];
    MvsResourceInfoRecord *resource_info;
    int name_length,remove_length;

    resource_info = widget_class_info->resource_info;
    name_length = strlen(resource_info[i].name);
    remove_length = strlen(remove_str);

    /* Grab root resource name (without suffix) */
    k = name_length - remove_length;
    if (strcmp(&resource_info[i].name[k],remove_str) != 0) {
        msg_warning("mvsFindCorrespondinResource():Remove_string '%s' not found in '%s'\n",
                    remove_str,resource_info[i].name);
        k = name_length;
    }
    strncpy(name,resource_info[i].name,k);
    name[k] = '\0';

    /* Instead, concatenate add_str */
    strcat(name,add_str);

    /* Find this resource number */
    return( mvsGetResourceNum(widget_class_info, name));

} /* End mvsFindCorrespondingResource() */
