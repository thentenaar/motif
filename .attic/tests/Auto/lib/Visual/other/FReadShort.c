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
static char rcsid[] = "$XConsortium: FReadShort.c /main/7 1995/07/14 11:44:18 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FReadShort.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Read a short integer from a file in an architecture independent way.

            INPUTS
                fd - file descriptor

            OUTPUTS
                none

            RETURNS
                short interger value
************************************************************************/

#include "vislib.h"

unsigned short fread_short(fd)
FILE *fd;
{
    unsigned short value1,value2;
    char *ch1 = (char *)&value1;
    char *ch2 = (char *)&value2;

    fread(&value1,sizeof(short),1,fd);

    if (LEAST_SIGNIFICANT_BYTE_FIRST) {
        ch2[0] = ch1[1];
        ch2[1] = ch1[0];
    }
    else
        value2 = value1;

    return(value2);

} /* End fread_short() */
