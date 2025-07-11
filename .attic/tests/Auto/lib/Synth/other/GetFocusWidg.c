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
static char rcsid[] = "$XConsortium: GetFocusWidg.c /main/8 1995/07/14 11:34:18 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetFocusWidg.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
            07/31/90 SJS the _XmFocusDataRec is changed (I don't much
                         like this dependence, but I guess we are stuck.
            08/23/90 SJS call to XtWindowToWidget() was incorrect
	Calls:

	Summary:
            Returns the widget that is currently in focus. It does this by first
        identifying which top-most shell widget is in focus (if any are) and
        then determining which item of which tab group under that shell is
        currently in focus.

        INPUTS
            none

        OUTPUTS
            none

        RETURNS
            Widget that is in keyboard focus. If the current window is not a 
            recognizable widget, it then returns NULL.  If the current window
            is the root window, then it return the constant PointerRoot.
************************************************************************/
#include <Xm/Xm.h>
#include "xislib.h"

Widget xisGetFocusWidget()
{
    Window server_focus_window,pointer_window;
    Widget server_focus_widget,tmp_widget;
    int revert;
    int rx,ry,wx,wy;
    unsigned int button_mask;

    XGetInputFocus(xisDisplay, &server_focus_window, &revert);
    xisGetPointerInfo(&pointer_window,&rx,&ry,&wx,&wy,&button_mask);

    if ( (server_focus_window == PointerRoot) || 
         (xisIsAncestorWindow(server_focus_window,pointer_window)) )
        server_focus_window = pointer_window;

    if (server_focus_window == None)
        return (Widget)NULL;

    server_focus_widget = XtWindowToWidget(xisDisplay, server_focus_window);

    if (server_focus_widget == NULL)
        return (Widget)NULL;

    tmp_widget = xisGetShellWidget(server_focus_widget);

    if (tmp_widget == NULL)
        return (Widget)NULL;

    return XmGetFocusWidget(tmp_widget);

} /* End xisGetFocusWidget() */
