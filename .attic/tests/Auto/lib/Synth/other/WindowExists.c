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
static char rcsid[] = "$XConsortium: WindowExists.c /main/7 1995/07/14 11:37:16 drk $"
#endif
#endif
/***********************************************************************
  	@(#)WindowExists.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Determines if the specified window exists.

        INPUTS:
            window - window which may or may not exist
        
        OUTPUTS:
            none

        RETURNS:
            True if window exists
            False otherwise
************************************************************************/
#include "xislib.h"

static int weBadWindowFlag;

/**** ERROR HANDLER FOR WINDOW_EXISTS BELOW *****/

static int weErrorHandler(display_in,err_event)
Display *display_in;
XErrorEvent *err_event;
{
  if ((display_in == xisDisplay)&&(err_event->request_code == X_GetGeometry)&&
      ((err_event->error_code == BadDrawable)||(err_event->error_code == BadWindow)) )
      weBadWindowFlag = 1;
        
} /* End weErrorHandler() */

/**** WINDOW_EXISTS *****/

int xisWindowExists(window)
Window window;
{
    static char routine_name[] = "xisWindowExists";
    Window root_return;
    int x,y;
    unsigned int w,h,bw;
    unsigned int depth;

    xisUseSessionInfo(routine_name);

    weBadWindowFlag = 0;
    XSetErrorHandler(weErrorHandler);

    XGetGeometry(xisDisplay,window,&root_return,&x,&y,&w,&h,&bw,&depth);
    XSync(xisDisplay,False);

    XSetErrorHandler(NULL);

    return (!weBadWindowFlag);

} /* end xisWindowExists() */
