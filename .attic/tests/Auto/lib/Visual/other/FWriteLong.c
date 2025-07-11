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
static char rcsid[] = "$XConsortium: FWriteLong.c /main/7 1995/07/14 11:44:27 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FWriteLong.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Write a 4-byte integer to a file in an architecture independent way.

            INPUTS
                fd - file descriptor
                value - 4-byte (or more) value

            OUTPUTS
                none

            RETURNS
                nothing
************************************************************************/
#include "vislib.h"

void fwrite_long(fd,value)
FILE *fd;
unsigned long value;
{
    unsigned short value1,value2;

    /* split long into 2 shorts */

    value1 = (value>>16) & 0xFFFF;
    value2 = value & 0xFFFF;

    /* force into most signif byte first format */

    fwrite_short(fd,(unsigned int)value1); /* most sig word first */
    fwrite_short(fd,(unsigned int)value2); 

} /* End fwrite_long() */
