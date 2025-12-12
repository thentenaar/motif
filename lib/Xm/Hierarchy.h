/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#ifndef _XmHierarchy_h
#define _XmHierarchy_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/Ext.h>

/************************************************************
*	INCLUDE FILES
*************************************************************/

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _XmHierarchyClassRec	*XmHierarchyWidgetClass;
typedef struct _XmHierarchyRec		*XmHierarchyWidget;

typedef struct _XmHierarchyNodeStateData {
    Widget widget;
    XmHierarchyNodeState state;
} XmHierarchyNodeStateData;

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL FUNCTION DECLARATIONS
*************************************************************/

/*	Function Name: XmHierarchyOpenAllAncestors
 *	Description: This function opens all ancestors of the given node.
 *	Arguments: nw - the node (widget) that will be changed.
 *	Returns: none
 */

void XmHierarchyOpenAllAncestors(Widget);

WidgetList XmHierarchyGetChildNodes(Widget);

extern WidgetClass xmHierarchyWidgetClass;

#if defined(__cplusplus)
}
#endif

#endif /* _Hierarchy_h */
