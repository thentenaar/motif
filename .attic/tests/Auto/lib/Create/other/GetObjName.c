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
static char rcsid[] = "$XConsortium: GetObjName.c /main/7 1995/07/14 11:16:38 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetObjName.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to SCCS
	Calls:

	Summary:
            Returns the name of the object region to be one of those listed
            in mvslib.h

            INPUTS
                object_code  - integer code from 0 to MAX_OBJECT (i.e.,
                LIST_ITEM_OBJECT)

            OUTPUTS
                none

            RETURNS
                statically define string containing name of object region
************************************************************************/

#include "mvslib.h"

char *mvsGetObjectRegionName(object_code)
int object_code;
{
    if (object_code > MAX_OBJECT || object_code < 0)
        object_code = MAX_OBJECT;

    return mvsObjectName[object_code];
} /* End mvsGetObjectRegionName() */
