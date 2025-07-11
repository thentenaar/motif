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
/*   $XConsortium: StringDisp.h /main/6 1995/07/13 19:55:02 drk $ */

/*
 *	file: StringDisp.h
 */

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/MenuShell.h>


/*  Private Functions  */
static XtCallbackProc  QuitCB();

Widget 			toplevel;
XtAppContext  	app_context;

