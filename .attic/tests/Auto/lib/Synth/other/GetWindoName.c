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
static char rcsid[] = "$XConsortium: GetWindoName.c /main/7 1995/07/14 11:35:46 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetWindoName.c	1.3.1.2	Date:2/7/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns a pointer to a statically declared string naming the
        specified window.

        INPUTS:
            window       - window id
        
        OUTPUTS:
            none

        RETURNS:
            string window name
************************************************************************/
#include "xislib.h"
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Core.h>
#include <X11/CoreP.h>

char *xisGetWindowName(window)
Window window;
{
    char *name = NULL;
    Widget widg;
    static char other_string[] = "Other";
    CoreClassPart *class;

    widg = XtWindowToWidget(xisDisplay, window);

    while (widg != NULL && name == NULL) {
        if (window == XtWindow(widg)) {
            class = (CoreClassPart *)XtClass(widg);
            name = class->class_name;
	}
        widg = XtParent(widg);
    }

    if (name == NULL)
        return other_string;
    else
        return name;

} /* End xisGetWindowName() */
