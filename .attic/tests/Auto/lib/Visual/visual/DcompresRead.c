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
static char rcsid[] = "$XConsortium: DcompresRead.c /main/7 1995/07/14 11:49:24 drk $"
#endif
#endif
/***********************************************************************
  	@(#)DcompresRead.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            This function reads in information about visual images
        that was previously written to the data file. This information
        was in compressed form and needs to be decompressed
************************************************************************/

#include "vislib.h"

void mvsDecompressRead(fp,buffer,size)
FILE *fp;
unsigned char buffer[];
int size;
{
    unsigned char *bp = buffer;
    unsigned char byte,curchar;
    long cnt;
    unsigned char cnt_comp[10];
    short num_cnt_comps;
    unsigned char *end_of_buffer = &buffer[size];
    unsigned char inbuf[INBUF_SIZE];
    int  i,j;

    mvsGetCh((FILE *)0, (unsigned char *)0,
             (unsigned char *)0); /* initialize mvsGetCh */

    /* Loop to read one byte at a time from buffer until 'size' bytes read */

    while ((bp < end_of_buffer) && (mvsGetCh(fp,inbuf,&curchar))) {

        /* New character read in while loop above */

        /* Now read the variable length count below */

        cnt = 0;
        if (mvsGetCh(fp,inbuf,&byte)) {

            num_cnt_comps = 0;

            /* Repeatedly read bytes int cnt_comp[] until MSBit is 0 */

            do {
                cnt_comp[num_cnt_comps++] = (byte & 0x7f);
            } while ((byte & 0x80) && (mvsGetCh(fp,inbuf,&byte)));

            /* Reconstruct count; start with MSbyte last back to LSbyte first */

            for (j=num_cnt_comps-1; j>=0; j--) {
                cnt <<=7;
                cnt |= cnt_comp[j];
            }
        }

        /* Now output that many instances of curchar in the image buffer */

        for (i=0; i<cnt; i++) {
            *bp++ = curchar;
        }
    } /* End while() */

} /* End mvsDecompressRead() */
