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
static char rcsid[] = "$XConsortium: CreateWidget.c /main/7 1995/07/14 11:38:03 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CreateWidget.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Call all InformExpectedAction functions associated with an
        expose event.

        INPUTS
            widget - the widget (assumed already created but not realized)

        OUTPUTS
            none

        RETURNS 
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisCreateWidget(widget)
Widget widget;
{
    XisObjectRecord *object;
    XisObjectRecord *current_object;

    object = (XisObjectRecord *)NULL;

    xisProcessObjects();

    object = xisFindObjectFromWidget(widget);
    if (object == NULL_Object)
      AutoMessage(_AutoMessages[WARNMSG94]);

    /* Call all relavent InformExpectedActions functions for EventExpose */
  
    current_object = object;
    xisInform.is_valid = 1;
    xisInform.action_obj = object;
    xisInform.pointer_obj = NULL;
    xisInform.modifier_key_status = 0;
    xisInform.button_num = 0;
    xisInform.key_code = 0;
    xisInform.num_clicks = 0;
    xisInform.edge_code = 0;
    xisInform.event_code = EventExpose;

    while (current_object != NULL) {

        if (current_object->proc_InformExpectedActions != NULL) {
            xisInform.current_obj = current_object;
            (*current_object->proc_InformExpectedActions)(EventExpose); 
        }
        current_object = current_object->parent;
    }

    xisInform.is_valid = 0;

} /* End xisCreateWidget() */
