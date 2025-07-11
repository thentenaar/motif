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
static char rcsid[] = "$XConsortium: PushHashBits.c /main/7 1995/07/14 11:44:53 drk $"
#endif
#endif
/***********************************************************************
  	@(#)PushHashBits.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to sccs
	Calls:

	Summary:
            Used in GetStateHash to append a long value (bits) of arbitrary
            precision (num_bits) onto an array of bytes (hash) of arbitrary
            precision.  

          INPUTS
            hash - list of bytes to append more bits to
            byte_num - current appendable byte number (0...) in hash byte list
            bit_num - current bit to append next (0...7)
            bits - long value of arbitrary precision to append onto hash list
            num_bits - precision of bits

          OUTPUTS
            hash - altered by appending bits
            byte_num - altered to represent new current byte num
            bit_num - altered to represent new current bit num

          RETURNS
            nothing
************************************************************************/

#include "vislib.h"

void mvsPushHashBits(hash,byte_num,bit_num,bits,num_bits)
char hash[];
int *byte_num;
int *bit_num;
long bits;
int num_bits;
{
    int i;

    for (i=0; i<num_bits; i++) {
        hash[*byte_num] <<= 1;
        hash[*byte_num] |= bits & 1;
        *bit_num = (*bit_num) + 1;
        if (*bit_num == 8) {
            *bit_num = 0;
            *byte_num = *byte_num + 1;
        }
        bits >>= 1;
    }

} /* End mvsPushHashBits() */
