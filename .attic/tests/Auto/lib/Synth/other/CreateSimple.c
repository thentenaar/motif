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
static char rcsid[] = "$XConsortium: CreateSimple.c /main/7 1995/07/14 11:32:43 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CreateSimple.c	1.2.1.1	Date:1/22/91
  	Author: SJS
	History:
            08/24/90 SJS let's make this its own routine
	Calls:

	Summary:

            Creates the window used for focus setting. This window is
        used as the focus window when focus is set out of the window
        being validated.

        INPUTS

        OUTPUTS
            none

        RETURNS
************************************************************************/

#include "xislib.h"

void xisCreateSimple()
{
    XWMHints wm_hints;
    XSizeHints size_hints;

    xisDummyFocusWindow =
               XCreateSimpleWindow( xisDisplay, xisRootWindow,
                                    DisplayWidth(xisDisplay,xisScreen) - 50,
                                    DisplayHeight(xisDisplay,xisScreen) - 50,
                                    50,50,2,BlackPixel(xisDisplay,xisScreen),
                                    WhitePixel(xisDisplay,xisScreen));

    wm_hints.flags = (InputHint | StateHint);
    wm_hints.input = False;
    wm_hints.initial_state = NormalState;
    XSetWMHints(xisDisplay,xisDummyFocusWindow,&wm_hints);
    size_hints.flags = (USPosition | USSize);
    size_hints.x = DisplayWidth(xisDisplay,xisScreen)-50;
    size_hints.y = DisplayHeight(xisDisplay,xisScreen)-50;
    size_hints.width = 50;
    size_hints.height = 50;
    XSetWMNormalHints(xisDisplay,xisDummyFocusWindow,&size_hints);
    XMapWindow(xisDisplay,xisDummyFocusWindow);

} /* End xisCreateSimple() */
