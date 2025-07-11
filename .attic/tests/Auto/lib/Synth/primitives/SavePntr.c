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
static char rcsid[] = "$XConsortium: SavePntr.c /main/7 1995/07/14 11:42:53 drk $"
#endif
#endif
/***********************************************************************
  	@(#)SavePntr.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90/SJS add to sccs
            08/07/90/SJS do not allow pointer to start off the
                         display (ie. x or y < 0)
	Calls:

	Summary:
            Initializes xisSavePointerX and xisSavePointerY with the start
        position of the pointer.  This routine should be called at the
        beginning of the application and xisRestorePointer() should be
        called at the end of an input synthesis session. We need to start
        the validation out with the pointer over a valid object. The
        object chosen is the root window.

        INPUTS:
            none
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include "xislib.h"

void xisSavePointer()
{
    static char routine_name[] = "xisSavePointer";
    int x,y;
    unsigned int button_mask;
    Window root_return,tmp_window;

    /* NOTE: Some window managers may change focus to our application
       at application window creation time because of the use of a
       particular focus policy. */

    xisUseSessionInfo(routine_name);

    XGetInputFocus(xisDisplay,&xisSaveFocusWindow,&xisSaveFocusRevert);

    if (!XQueryPointer(xisDisplay,xisRootWindow,&root_return,
                       &tmp_window,&xisSavePointerX,&xisSavePointerY,
                       &x,&y,&button_mask)) {
        (*xisErrorMsg)("Pointer not on root window anymore!!\n");
        exit(-1);
    }

    if((xisSaveFocusWindow == None) || (xisSaveFocusWindow == PointerRoot))
        xisSaveFocusWindow = xisRootWindow;

    xisPointerSaved = True;

    /* On the DecStations, it is possible to move the pointer "off" the
       display. This is a problem because we do not have any objects off
       the display. So, let's move the pointer on the display.
    */
    if( (xisSavePointerX < 0) || (xisSavePointerY < 0) ) {
        xisSavePointerX = 0;
        xisSavePointerY = 0;
        XWarpPointer(xisDisplay,None,xisRootWindow,0,0,0,0, 0, 0);
    }

} /* End xisSavePointer() */
