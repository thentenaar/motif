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
static char rcsid[] = "$XConsortium: mvsCpRepArgs.c /main/7 1995/07/14 11:19:55 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsCpRepArgs.c	1.7.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   07/31/90 SJS Add type cast
	Calls:

	Summary:
		This function takes an array of named resources and
	adds them to the arg array used in calls to XtSetValues().
	Any new resources are added to the list, any that already exist
	have the values replaced. The number of arguments is updated
	as required.
************************************************************************/
#include "mvslib.h"

void mvsCopyReplaceArgs(w_class_info, orig_args, orig_argcnt, new_args, 
			new_argcnt)
MvsWidgetClassInfo	*w_class_info;
Arg orig_args[];
Cardinal *orig_argcnt;
Arg new_args[];
Cardinal new_argcnt;
{
    register int i;

    if (new_args == NULL || new_argcnt == 0)
	return;
    for (i=0; i<new_argcnt; i++) {
	mvsCopyReplace1Arg(w_class_info, orig_args, (int *)orig_argcnt, 
			new_args[i].name, new_args[i].value);
    } /* End for(i<new_argcnt) */

} /* End mvsCopyReplaceArgs() */
