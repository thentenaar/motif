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
static char rcsid[] = "$XConsortium: ChkCmpTmpVis.c /main/5 1995/07/14 11:47:16 drk $"
#endif
#endif
/***********************************************************************
  	Author: REC
	History:
	    04/15/93 Created by REC
	Calls:

	Summary:
	    Called to determine the current mode when a compare temp 
	    visaul command is executed.  Compare the current imagage
	    against a stored temp image only if running in a compare 
	    (batch or interactive) mode.

        INPUTS:
            widget - the widget whose image needs comparing or capturing
	    identifier - the tag assocated with an image
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#include <AutoMessages.h>
#include "vislib.h"

void mvsCheckCompareTempVisual(widget_info, identifier)
MvsWidgetInfoRecord *widget_info;
char *identifier;
{

    char msg_string[125];
  
    if (XmIsGadget(widget_info->widget))
        widget_info = widget_info->parent;

    switch (mvsVisualMode) {
        case VISUAL_MODE_DONT_CHECK:
        case VISUAL_MODE_GENERATE_PIXMAPS:
            /* mvsImageFd = 0; */
            break;

        case VISUAL_MODE_BATCH_COMPARE:
	    mvsCompareStoredPixmapBatch(widget_info, identifier);
	    break;

        case VISUAL_MODE_INTERACTIVE_COMPARE:
	    mvsCompareStoredPixmapInt(widget_info, identifier);
            break;

	default:
	    sprintf(msg_string, _AutoMessages[VISMSG45], mvsVisualMode);
	    AutoMessage(msg_string);
    }

} /* End mvsCheckCompareTempVisual() */
