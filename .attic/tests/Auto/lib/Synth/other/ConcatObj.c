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
static char rcsid[] = "$XConsortium: ConcatObj.c /main/7 1995/07/14 11:32:33 drk $"
#endif
#endif
/***********************************************************************
  	@(#)Concat.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
        CONCATENATE OBJECT TYPES

            Returns a new object type that consists of the last two object types 
        concatenated together.

        INPUTS
            cum_object_type - pointer to an object record
            new_object_type - object code of child

        OUTPUTS
            none

        RETURNS
            A concatenated object type code.
************************************************************************/
#include "xislib.h"

long xisConcat(cum_object_type,new_object_type)
long cum_object_type;
long new_object_type;
{
    return (cum_object_type * xisNumObjectTypes + new_object_type);

} /* End xisConcat() */
