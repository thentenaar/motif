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
static char rcsid[] = "$XConsortium: mvsGetRscNum.c /main/7 1995/07/14 11:21:14 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsGetRscNum.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS Changes for composites
	   06/15/90 PSN add function to get index of given resnam from given 
			args array.
	   09/20/90 SJS use constant NON_EXISTENT
	Calls:

	Summary:
		This function returns the index number of a resource
	within the resource_info structure. A value of "NON_EXISTENT"
        (currently -1) is returned if the requested resource name was
        not located.
************************************************************************/

#include "mvslib.h"

int mvsGetResourceNum(widget_class_info, resource_name)
MvsWidgetClassInfo *widget_class_info;
char resource_name[];
{
    int j;
    short num_resources;
    MvsResourceInfoRecord *resource_info;

    num_resources = widget_class_info->num_resources;
    resource_info = widget_class_info->resource_info;

    for (j=0;
         (j<num_resources)&&strcmp(resource_name,resource_info[j].name); j++);

    return (j<num_resources ? j : (NON_EXISTENT));
        
} /* End mvsGetResourceNum() */

int mvsGetResNumInArgs(args, nargs, resource_name)
Arg	args[];
int	nargs;
char	resource_name[];
{
    int j;

    for ( j=0 ; (j<nargs)&&strcmp(resource_name,args[j].name) ; j++ )
	;

    return (j<nargs ? j : (-1));
        
} /* End mvsGetResNumInArgs() */
