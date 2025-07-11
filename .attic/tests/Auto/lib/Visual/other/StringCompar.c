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
static char rcsid[] = "$XConsortium: StringCompar.c /main/7 1995/07/14 11:45:02 drk $"
#endif
#endif
/***********************************************************************
  	@(#)StringCompar.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Compare two strings in a case insensitive way.

            INPUTS
                str1 - 1st string
                str2 - 2nd string

            OUTPUTS
                none

            RETURNS
                0 if equal, >0 if str1 > str2, <0 if str1 < str2
************************************************************************/
#include "vislib.h"

int mvsStringCompare(str1,str2)
String str1,str2;
{
    char *p1, *p2;
 
    for (p1=str1, p2=str2; 
         *p1 && *p2 && tolower(*p1) == tolower(*p2);
         p1++, p2++);

    if (*p1 == *p2)
        return(0);
 
    if ((*p2 == 0) || (*p1 > *p2))
        return(1);
    else
        return(-1);

} /* End mvsStringCompare() */
