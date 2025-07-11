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
static char rcsid[] = "$XConsortium: IsWidgSelect.c /main/7 1995/07/14 11:40:46 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsWidgSelect.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
            Returns True if widget is selected. This means that the mouse
        button was pressed while over the widget

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = current widget selected
            False = current widget not selected
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsWidgetSelected(bool)
int bool;
{
   int result;

   if (!xisInform.is_valid) 
       AutoMessage(_AutoMessages[WARNMSG58]);

   if (xisInform.current_obj->id.widget == xisState.selected_widget &&
       xisInform.current_obj->id.object_type == xisState.selected_object_type &&
       xisInform.current_obj->id.instance == xisState.selected_instance)
       
       result = bool;
   else
       result = (!bool);

   return (result);

} /* End xisIsWidgetSelected() */
