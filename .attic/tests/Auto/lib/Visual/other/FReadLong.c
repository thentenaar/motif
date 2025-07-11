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
static char rcsid[] = "$XConsortium: FReadLong.c /main/7 1995/07/14 11:44:08 drk $"
#endif
#endif
/***********************************************************************
  	@(#)FReadLong.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
            Reads a 4-byte integer from a file in an architecture independent
            way.

            INPUTS
                fd - file descriptor

            OUTPUTS
                none

            RETURNS
                value - 4-byte (or more) value that was read from the file
************************************************************************/

#include "vislib.h"

unsigned long fread_long(fd)
FILE *fd;
{
    unsigned long value;
    unsigned short value1,value2;

    value1 = fread_short(fd);
    value2 = fread_short(fd);

    value = ((unsigned long)value1<<16) | (unsigned long)value2;
    
    return(value);

} /* End fread_long() */
