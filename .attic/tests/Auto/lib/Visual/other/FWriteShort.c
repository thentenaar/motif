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
static char rcsid[] = "$XConsortium: FWriteShort.c /main/7 1995/07/14 11:44:36 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FWriteShort.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Write a short integer out to a file in an architecture
            independent way.

            INPUTS
                fd - file descriptor
                value1 - short integer value

            OUTPUTS
                none

            RETURNS
                nothing
************************************************************************/
#include "vislib.h"

void fwrite_short(fd,value_in)
FILE *fd;
unsigned int value_in;
{
    unsigned short value1=(unsigned short)value_in;
    unsigned short value2;
    char *ch1 = (char *)&value1;
    char *ch2 = (char *)&value2;

    /* force into most signif byte first format */

    if (LEAST_SIGNIFICANT_BYTE_FIRST) {
        ch2[1] = ch1[0];   /* swap bytes */
        ch2[0] = ch1[1];
    }
    else
        value2 = value1;

    fwrite(&value2,sizeof(short),1,fd);

} /* End fwrite_short() */
