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
static char rcsid[] = "$XConsortium: CompresWrite.c /main/7 1995/07/14 11:49:15 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CompresWrite.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            This function writes out visual info in compressed form
************************************************************************/

#include "vislib.h"

void mvsCompressWrite(fp,buffer,size)
FILE *fp;
unsigned char buffer[];
int size;
{
    unsigned char *bp = buffer;
    unsigned char oldchar;
    long cnt;
    unsigned char *end_of_buffer = &buffer[size];
    unsigned char outbuf[OUTBUF_SIZE];
    int  i,j;

    oldchar = *bp++;
    cnt = 1;
    i = 0;

    /* Loop for pointer bp from beginning of buffer until 'size' bytes passed */

    while (bp < end_of_buffer) {

        /* Count number of times oldchar reappears consecutively */

        while ((bp < end_of_buffer) && (*bp == oldchar)) {
            bp++; cnt++;
        }

        /* Write out character that repeats */

        outbuf[i++] = oldchar;

        /* Then write out repeat count */
        /* Use variable length format where most-sig-bit is set in each byte  */
        /* to indicate that the next byte contains more bits.  Of course this */
        /* means that we can only use the 7 least sig bits per byte for data. */

        while (cnt > 127) {
            outbuf[i++] = (cnt & 0x7f)|(0x80); /* Force MSBit to 1 */
            cnt >>= 7;
        }
        outbuf[i++] = cnt;  /* This one is gauranteed to have MSBit set to 0 */

        if (i > OUTBUF_SIZE - 10) {  /* If buffer full, flush out to file */
            if (mvsBigChars) {
                for (j=0; j<i; j++)
                    fputc(outbuf[j],fp);
            }
            else {
                fwrite(outbuf,sizeof(char),i,fp);
            }
            i = 0;
        }
        oldchar = *bp++;  /* Get next char, store in oldchar */
        cnt = 1;          /* Reset char count to 1 */
    }

    /* Flush buffer out to file one last time */

    if (mvsBigChars) {
        for (j=0; j<i; j++)
            fputc(outbuf[j],fp);
    }
    else {
        fwrite(outbuf,sizeof(char),i,fp);
    }

} /* End mvsCompressWrite() */



