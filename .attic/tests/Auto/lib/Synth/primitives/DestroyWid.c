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
static char rcsid[] = "$XConsortium: DestroyWid.c /main/7 1995/07/14 11:38:14 drk $"
#endif
#endif
/***********************************************************************
  	@(#)DestroyWid.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            07/18/90 PSN reset the xisState's focus info to null if widget being
			 destroyed is the focus widget.
	Calls:

	Summary:
            Call all InformExpectedAction functions associated with an expose event.

        INPUTS
            widget - the widget (not yet destroyed)

        OUTPUTS
            none

        RETURNS 
            nothing
************************************************************************/

#include "xislib.h"

void xisDestroyWidget(widget)
Widget widget;
{
    XisObjectRecord *object = xisFindObjectFromWidget(widget);
    XisObjectRecord *current_object;

    if (object == NULL_Object) {
        /* This should really be an error case, but the semi-random nature of     */
        /* when MenuShells and their children actually go away makes it difficult */
        /* to correctly maintain the hierarchy in a *completely* accurate state.  */
        /* We know when objects are being destroyed, we don't know when the       */
        /* process is complete. (Only for Menu-related conveniences)              */

        (*xisTraceMsg)("xisDestroyWidget called with non-existent widget\n");
        return;
    }

    /* Call all relavent InformExpectedActions functions for EventDestroy */
  
    current_object = object;
    xisInform.is_valid = TRUE;
    xisInform.action_obj = object;
    xisInform.modifier_key_status = 0;
    xisInform.button_num = 0;
    xisInform.key_code = 0;
    xisInform.num_clicks = 0;
    xisInform.edge_code = 0;
    xisInform.event_code = EventDestroy;

    while (current_object != NULL) {

        if (current_object->proc_InformExpectedActions != NULL) {
            xisInform.current_obj = current_object;
            (*current_object->proc_InformExpectedActions)(EventDestroy); 
        }
        current_object = current_object->parent;
    }

    xisInform.is_valid = FALSE;

    /* Check whether the widget being destroyed is currently having focus?
     * If yes, change the focus to different widget.
     */
    if ( widget == xisState.focus_widget ) {
	xisState.focus_window = xisDummyFocusWindow;
	xisState.focus_widget = (Widget)NULL;
	xisState.focus_object_type = 0;
	xisState.focus_instance = 0;
    }

} /* End xisDestroyWidget() */
