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
static char rcsid[] = "$XConsortium: CreateShell.c /main/8 1995/07/14 11:24:02 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CreateShell.c	1.2.1.1	Date:1/22/91
  	Author: SJS
	History:
            07/17/90 SJS create
	Calls:

	Summary:
            This file creates the function used to create an
        Application Shell. It would be best if the name did not begin
        with Xm, but for now this is generated automatically and a better
        solution than hand modifying the generated file.
************************************************************************/

#include "mvslib.h"

Widget XmCreateApplicationShell(parent, name, args, nargs, xtclass)
Widget parent;
char *name;
Arg args[];
int nargs;
WidgetClass xtclass;
{
    Widget Shell;

    Shell = XtAppCreateShell(name, "XMcommon" , xtclass,
                             mvsDisplay, args, nargs);

    return(Shell);

} /* End XmCreateApplicationShell() */



Widget XmCreatePopupShell(parent, name, args, nargs, xtclass)
Widget parent;
char *name;
Arg args[];
int nargs;
WidgetClass xtclass;
{
    Widget Shell;

    Shell = XtCreatePopupShell(name, topLevelShellWidgetClass,
                               parent, args, nargs);

    return(Shell);

} /* End XmCreateApplicationShell() */
