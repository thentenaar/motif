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
static char rcsid[] = "$XConsortium: GetShellWidg.c /main/7 1995/07/14 11:35:34 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetShellWidg.c	1.3.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Returns the shell of the specified widget.

        INPUTS
            widget

        OUTPUTS
            none

        RETURNS
            widget's shell widget
************************************************************************/
#include "xislib.h"
#include <X11/Shell.h>

Widget xisGetShellWidget(widget)
Widget widget;
{
    Widget tmp_widget;

    if (widget == NULL || widget == None)
        return (Widget)NULL;

    tmp_widget = widget;

    while (tmp_widget != NULL && !XtIsShell(tmp_widget))
        tmp_widget = XtParent(tmp_widget);

    return tmp_widget;    

} /* End xisGetShellWidget() */
