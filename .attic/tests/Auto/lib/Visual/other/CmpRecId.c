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
static char rcsid[] = "$XConsortium: CmpRecId.c /main/7 1995/07/14 11:43:58 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CmpRecId.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to sccs
	Calls:

	Summary:
             This function compares two record ids
************************************************************************/

int mvsCmpRecId(str1,len1,str2,len2)
unsigned char str1[], str2[];
short len1,len2;
{
    int i;

    if (len1 != len2)
        return(1);

    for (i=0; i<len1; i++)
        if (str1[i] != str2[i])
            return(1);

    return(0);

} /* End mvsCmpRecId() */
