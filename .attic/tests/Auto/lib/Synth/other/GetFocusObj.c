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
static char rcsid[] = "$XConsortium: GetFocusObj.c /main/7 1995/07/14 11:34:08 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetFocusObj.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/31/90 SJS change return of this function
	Calls:

	Summary:
            Returns the object that is currently in focus. It does this by first
        identifying which top-most shell widget is in focus (if any are) and
        then determining which item of which tab group under that shell is
        currently in focus.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
                Object that is in keyboard focus. If the current window is not a
            recognizable widget, it then returns NULL.  If the current window
            is the root window, then it return the constant PointerRoot.
                The object returned is used as the base for calling the
            SetExpectedActions procs. The object may or may not be a "real"
            widget. It can also be a gadget.
************************************************************************/
#include "xislib.h"

XisObjectRecord *xisGetFocusObject()
{
    Window pointer_window;
    XisObjectRecord *curr_focus_object;
    int rx,ry,wx,wy;
    unsigned int button_mask;

    xisGetPointerInfo(&pointer_window,&rx,&ry,&wx,&wy,&button_mask);

    curr_focus_object = xisFindObject(xisState.focus_widget,
                                      xisState.focus_object_type,
                                      xisState.focus_instance);

    return(curr_focus_object);

} /* End xisGetFocusObject() */
