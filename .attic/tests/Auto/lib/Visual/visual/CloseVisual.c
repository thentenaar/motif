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
static char rcsid[] = "$XConsortium: CloseVisual.c /main/7 1995/07/14 11:47:57 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CloseVisual.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Closes the image data file.

        INPUTS
            none
  
        OUTPUTS 
            none

        RETURNS
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"

void mvsCloseVisual()
{
    int i;
    long end_of_image_file;

    switch (mvsGetVisualMode()) {

        case VISUAL_MODE_GENERATE_PIXMAPS:
            end_of_image_file = ftell(mvsImageFd);

            VIS_trace("Writing %d record hashes out to .dat file\n",
                      mvsNumRecIds);

            fwrite_short(mvsImageFd,(unsigned int)mvsNumRecIds);
            for (i=0; i<mvsNumRecIds; i++) {
                fwrite_short(mvsImageFd,(unsigned int)mvsRecIdRecordStartCode);
                fwrite_short(mvsImageFd,(unsigned int)mvsRecIdLength[i]);
                fwrite_long(mvsImageFd,mvsFilePos[i]);
                fwrite(mvsRecIdTable[i],sizeof(char),mvsRecIdLength[i],mvsImageFd);
            }
            if (fseek(mvsImageFd,mvsBeginningOfImageFile,0) != 0) 
                AutoMessage(_AutoMessages[VISMSG33]);
            else
                fwrite_long(mvsImageFd,(unsigned long)end_of_image_file);

            fclose(mvsImageFd);
            break;

        case VISUAL_MODE_BATCH_COMPARE:
        case VISUAL_MODE_INTERACTIVE_COMPARE:

            fclose(mvsImageFd);
            break;

        default:
            /* Do absolutely nothing */
            break;
    }

} /* End mvsCloseVisual() */
