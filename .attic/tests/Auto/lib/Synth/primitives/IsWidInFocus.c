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
static char rcsid[] = "$XConsortium: IsWidInFocus.c /main/7 1995/07/14 11:40:35 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsWidInFocus.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Returns True if widget is in keyboard focus.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = widget is in keyboard focus
            False = widget is not in keyboard focus
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsWidgetInKeyboardFocus(bool)
int bool;
{
    int result;

    XisObjectRecord *focus_object = xisGetFocusObject();

    if (!xisInform.is_valid) 
       AutoMessage (_AutoMessages[WARNMSG56]);

    if (focus_object->id.widget == xisInform.current_obj->id.widget)
        result = bool;
    else
        result = (!bool);

    return (result);

} /* End xisIsWidgetInKeyboardFocus() */

/***********************************************************************
  	Author: PSN
	History:
            08/29/90 PSN first cut
	Calls:

	Summary:
            Returns True if widget's parent is in keyboard focus.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = widget's parent is in keyboard focus
            False = widget's parent is not in keyboard focus
************************************************************************/

int xisIsParentWidgetInKeyboardFocus(bool)
int bool;
{
    int result;

    XisObjectRecord *focus_object = xisGetFocusObject();

    if (!xisInform.is_valid) 
       AutoMessage(_AutoMessages[WARNMSG57]);

    if (focus_object->id.widget == XtParent(xisInform.current_obj->id.widget))
        result = bool;
    else
        result = (!bool);

    return (result);

} /* End xisIsParentWidgetInKeyboardFocus() */
