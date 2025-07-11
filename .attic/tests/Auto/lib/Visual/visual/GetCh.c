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
static char rcsid[] = "$XConsortium: GetCh.c /main/7 1995/07/14 11:50:04 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetCh.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            This function reads in a character from the visual hash
        file.
************************************************************************/

#include "vislib.h"

short mvsGetCh(fp,inbuf,ch)
FILE *fp;
unsigned char inbuf[];
unsigned char *ch;
{
    static int i,j;
    static num_bytes_in_buf;
    static done;

    if (fp == (FILE *)NULL) {
        i = 0;
        num_bytes_in_buf = 0;
        done = 0;
        return 1;
    }

    if (i >= num_bytes_in_buf) {
        i = 0;
        if (mvsBigChars) {
            for (j=0; !feof(fp) && j<INBUF_SIZE; j++) {
                inbuf[i++]=fgetc(fp);
            }
            num_bytes_in_buf = i;
            i = 0;
            done = feof(fp);
        }
        else {
            if (!feof(fp))
                num_bytes_in_buf = fread(inbuf,sizeof(char),INBUF_SIZE, fp);
            else
                done = 1;
        }
    }
    *ch = inbuf[i++];
    return (!done);

} /* End mvsGetCh() */
