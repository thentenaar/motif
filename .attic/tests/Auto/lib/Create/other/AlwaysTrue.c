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
static char rcsid[] = "$XConsortium: AlwaysTrue.c /main/7 1995/07/14 11:15:21 drk $"
#endif
#endif
/***********************************************************************
	@(#)AlwaysTrue.c	1.2.1.1 			Date : 1/22/91
  	Author: PSN
	History:
            06/27/90 PSN add to SCCS

	Calls:

	Summary:
		  Always return true!!!

            OUTPUTS
                none
  
            RETURNS
		  Always return true!!!
************************************************************************/

#include "mvslib.h"

int
mvsIsAlwaysTrue(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    return( 1 == 1 );

} /* End mvsIsAlwaysTrue() */
