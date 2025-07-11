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
static char rcsid[] = "$XConsortium: RestorePntr.c /main/7 1995/07/14 11:42:43 drk $"
#endif
#endif
/***********************************************************************
  	@(#)RestorePntr.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
	Calls:

	Summary:
  Restores the pointer location to where it was found at the beginning of
  the input synthesized application run (when xisSavePointer() was called).

  INPUTS:
      none
        
  OUTPUTS:
      none

  RETURNS:
      nothing
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

void xisRestorePointer()
{
    static char routine_name[] = "xisRestorePointer";

    xisUseSessionInfo(routine_name);

    if (!xisPointerSaved)
        AutoMessage(_AutoMessages[SCRMSG40]);
    else {
        xisMovePointer(xisSavePointerX,xisSavePointerY);

        if (!xisIsMapped(xisSaveFocusWindow))
            xisSaveFocusWindow = xisRootWindow;

        XSetInputFocus(xisDisplay,xisSaveFocusWindow,xisSaveFocusRevert,CurrentTime);
    }

} /* End xisRestorePointer() */
