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
static char rcsid[] = "$XConsortium: UnmapWidget.c /main/7 1995/07/14 11:43:21 drk $"
#endif
#endif
/***********************************************************************
  	@(#)UnmapWidget.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/21/90 TAT add to sccs
	Calls:

	Summary:
            Unmaps the widget by calling XtUnmanageChild(). You should
            pass the child of the Menu or Dialog shell to this routine.

        INPUTS:
            widget - the widget to unmap.
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisUnmapWidget(widget)
Widget widget;
{
    XisObjectRecord *object;
    XisObjectRecord *current_object;

    if (widget == NULL) 
      AutoMessage(_AutoMessages[WARNMSG22]);

    else {
        object = xisFindObjectFromWidget(widget);

        (*xisTraceMsg)("Got send_event_request = MapWidget for %s\n",
                       xisGetObjectName(object));
    
        /* Call all relavent InformExpectedActions functions */
  
        current_object = object;
    
        xisInform.is_valid = 1;
        xisInform.action_obj = current_object;
        xisInform.modifier_key_status = 0;
        xisInform.button_num = 0;
        xisInform.key_code = 0;
        xisInform.edge_code = 0;
        xisInform.num_clicks = 0;
        xisInform.event_code = EventUnmap;

        while (current_object != NULL) {

            if (current_object->proc_InformExpectedActions != NULL) {
                xisInform.current_obj = current_object;
                (*current_object->proc_InformExpectedActions)(EventUnmap); 
            }
            current_object = current_object->parent;
	}

        xisInform.is_valid = 0;

        XtUnmanageChild(widget);

        xisProcessEvents(NULL,0);
    }
} /* End xisUnmapWidget() */


