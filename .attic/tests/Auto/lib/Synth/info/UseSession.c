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
static char rcsid[] = "$XConsortium: UseSession.c /main/7 1995/07/14 11:32:22 drk $"
#endif
#endif
/***********************************************************************
  	@(#)UseSession.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Checks if display info is ok for use.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#include "xislib.h"

void  xisUseSessionInfo(routine_name)
char *routine_name;
{
    if (xisDisplay == NULL || xisAppContext == NULL)
        (*xisWarningMsg)("Session info being used in %s without prior BeginSession call!\n",
            routine_name);
} /* End xisUseSessionInfo() */
