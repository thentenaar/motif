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
static char rcsid[] = "$XConsortium: IsActiveWid.c /main/7 1995/07/14 11:39:03 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsActiveWid.c	1.2.1.1	Date:1/22/91
  	Author: TAT
	History:
            06/21/90 TAT add to sccs
	Calls:

	Summary:
            Returns True if the current widget is the action widget 
            (that is, the widget that produced the event).

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = current widget is the action widget
            False = otherwise
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsActiveWidget()
{
    if (!xisInform.is_valid)
        AutoMessage(_AutoMessages[WARNMSG50]);

    return (xisInform.current_obj == xisInform.action_obj);

} /* End xisIsActiveWidget() */
