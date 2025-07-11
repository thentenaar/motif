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
static char rcsid[] = "$XConsortium: IsPtrOvrWidg.c /main/7 1995/07/14 11:40:03 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsPtrOvrWidg.c	1.3.1.1	Date:1/22/91
  	Author:
	History:
	Calls:

	Summary:
            Returns True if pointer is over widget.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            True = pointer over widget or child of widget
            False = pointer not over widget
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsPointerOverWidget(bool)
int bool;
{
    int result;

    if (!xisInform.is_valid)
      AutoMessage(_AutoMessages[WARNMSG54]);

    if (xisInform.pointer_obj == xisInform.current_obj)
        result = bool;
    else
        result = (!bool);

    return (result);
} /* End xisIsPointerOverWidget() */
