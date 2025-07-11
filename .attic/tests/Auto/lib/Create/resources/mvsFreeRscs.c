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
static char rcsid[] = "$XConsortium: mvsFreeRscs.c /main/7 1995/07/14 11:20:51 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsFreeRscs.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
           05/11/90 TAT Moved body of free code into mvsFreeResource()
           05/29/90 SJS minor fixes
           07/29/90 PSN parameter changes
	Calls:

	Summary:
		This function frees up all the resources associated with
	the current widget state. Only the resources which are pointers
	require any processing. The memory pointed to by the resources
	must be freed because it is dynamically allocated as each new
	widget state is entered.
************************************************************************/

#include "mvslib.h"

void mvsFreeResources(widget_class_info, resources)
MvsWidgetClassInfo *widget_class_info;
MvsArgVal resources[];
{
    int i;
    short num_resources;

    num_resources = widget_class_info->num_resources;

    for (i=0; i<num_resources; i++)
        mvsFreeResource(widget_class_info, &resources[i], i);

} /* End mvsFreeResources() */
