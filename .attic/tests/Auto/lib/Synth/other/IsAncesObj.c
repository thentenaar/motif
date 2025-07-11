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
static char rcsid[] = "$XConsortium: IsAncesObj.c /main/7 1995/07/14 11:35:57 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsAncesObj.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Determines if the first specified object is an ancestor of the 2nd.
        It returns true if this is in fact a true statement.

        INPUTS:
            object1 - object which may be ancestor of object2 
            object2 - any object
        
        OUTPUTS:
            none

        RETURNS:
            True if object1 is an ancestor of object2
            False otherwise
************************************************************************/
#include "xislib.h"

int xisIsAncestorObject(object1,object2)
XisObjectRecord  *object1,*object2;
{
    XisObjectRecord *tmp_object;

    if (object1 == NULL)
        return(False);

    if (object2 == NULL)
        return(False);

    tmp_object = object2;

    while (tmp_object != NULL && tmp_object != object1)
        tmp_object = tmp_object->parent;

    if (tmp_object == object1)
        return(True);
    else
        return(False);
} /* End xisIsAncestorObject */





