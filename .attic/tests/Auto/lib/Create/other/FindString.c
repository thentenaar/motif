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
static char rcsid[] = "$XConsortium: FindString.c /main/7 1995/07/14 11:15:50 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FindString.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Finds a substring in a string

            INPUTS
                in_string     - The string to search in 
                search_string - The string to match in in_string

            OUTPUTS
                none

            RETURNS
                nothing
************************************************************************/

int findstr(in_string,search_string)
char in_string[],search_string[];
{
    char *ch, *sch = search_string;
    char *save_ch = in_string;

    while (*sch && *save_ch) {
        for (ch=save_ch, sch=search_string; 
             *sch && *ch && *ch == *sch; 
             ch++, sch++);
        if (*sch)
            save_ch++;
        ch = save_ch;
    }
    return (int)(save_ch - in_string);

} /* End findstr() */
