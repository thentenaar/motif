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
 *
 */

#ifndef _XmPaned_h
#define _XmPaned_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/Ext.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define XmPanedAskChild 0

typedef struct _XmPanedClassRec	*XmPanedWidgetClass;
typedef struct _XmPanedRec	*XmPanedWidget;

/************************************************************
 *
 *  Public Procedures
 *
 ************************************************************/

/*	Function Name: XmPanedGetPanes
 *	Description: Returns the number of panes in the paned widget.
 *	Arguments: w - the paned widget.
 *                 panes - the list of all panes contained in this widget.
 *                 num - the number of panes.
 *	Returns: the number of panes in the paned widget.
 */

extern int XmPanedGetPanes(
    Widget			/* w */,
    WidgetList *		/* panes */,
    int *			/* num */
);

/*	Function Name: XmCreatePaned
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */

Widget XmCreatePaned(Widget, String, ArgList, Cardinal);

/* Class record constant */
extern WidgetClass xmPanedWidgetClass;

#if defined(__cplusplus)
}
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* _XmPaned_h --  DON'T ADD STUFF AFTER THIS #endif */
