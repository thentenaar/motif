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
static char rcsid[] = "$XConsortium: GetKeyName.c /main/7 1995/07/14 11:34:30 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetKeyName.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns a pointer to a statically declared string naming the
        specified key.

        INPUTS:
            key - key code (i.e., KeyReturn, KeySpace, KeyUp, KeyTab, etc.)
        
        OUTPUTS:
            none

        RETURNS:
            key name string pointer
************************************************************************/
#include "xislib.h"

char *xisGetKeyName(key)
int key;
{
    static char *name;
    static char unknown_string[] = "KeyUnknown";
    static char other_string[] = "Name needs to be added to key struct";

    if (key >= 0 && key < _MaxKey)
        name = other_string;
    else
        name = unknown_string;

    return name;

} /* End xisGetKeyName */
