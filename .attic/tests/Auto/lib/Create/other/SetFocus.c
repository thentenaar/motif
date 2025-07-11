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
static char rcsid[] = "$XConsortium: SetFocus.c /main/7 1995/07/14 11:17:35 drk $"
#endif
#endif
/***********************************************************************
  	@(#)SetFocus.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	Calls:

	Summary:
		This functions sets the current focus into or out of
	the current widget
************************************************************************/
#include "mvslib.h"

void mvsSetFocus(widget,object_type,instance,focus_mode)
Widget widget;
int object_type;
int instance;
int focus_mode;

{
    Widget Null_widget = (Widget)NULL;

    if (focus_mode)
        xisSetKeyboardFocus(widget, object_type, instance);
    else
        xisSetKeyboardFocus(Null_widget, oUserDefined, 0);

} /* End mvsSetFocus() */
