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

#ifndef _XmExt18List_h_
#define _XmExt18List_h_

#if defined(__cplusplus)
extern "C" {
#endif

#include <Xm/MultiList.h>
#include <Xm/XmP.h>

XM_ALTERNATIVE(Use XmMultiList instead)
externalref WidgetClass xmExt18ListWidgetClass;

enum {
    XmEXT18LIST_FOUND = XmMULTILIST_FOUND,
    XmEXT18LIST_NOT_FOUND = XmMULTILIST_NOT_FOUND
};

typedef XmMultiListWidgetClass *XmExt18ListWidgetClass;
typedef XmMultiListWidget *XmExt18ListWidget;
typedef XmMultiListRowInfo Xm18RowInfo;
typedef XmMultiListCallbackStruct XmExt18ListCallbackStruct;

/*
 * Function Name: XmExt18ListGetSelectedRows
 * Description:   Takes an Extended List and returns a NULL terminated array
 *                of pointers to selected rows from the internal list
 * Arguments:     w - the extended list widget
 * Returns:       Xm18RowInfo **
 */
XM_DEPRECATED Xm18RowInfo **XmExt18ListGetSelectedRows(Widget w);

/*	Function Name: XmCreateExtended18List
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The Widget created.
 */
XM_ALTERNATIVE(Use XmMultiList instead)
extern Widget XmCreateExtended18List(Widget, String, ArgList, Cardinal);

XM_ALTERNATIVE(Use XmMultiList instead)
extern Widget XmCreateExt18List(Widget, String, ArgList, Cardinal);

XM_ALTERNATIVE(Use XmMultiList instead)
extern Widget XmVaCreateExt18List(Widget parent, char *name, ...);

XM_ALTERNATIVE(Use XmMultiList instead)
extern Widget XmVaCreateManagedExt18List(Widget parent, char *name, ...);

/*  Function Name: XmExt18ListUnselectAllItems
 *  Description:   Unselects all rows
 *  Arguments:     w - the ilist widget.
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListUnselectAllItems(Widget w);

/*  Function Name: XmExt18ListUnselectItem
 *  Description:   Unselects the row passed in
 *  Arguments:     w - the ilist widget.
 *                 row_info - ptr to the row passed in
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListUnselectItem(Widget, Xm18RowInfo *);

/*  Function Name: XmExt18ListToggleRow
 *  Description:   Toggles the selection state of a specified row
 *  Arguments:     w - the extended list widget
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListToggleRow(Widget, short);

/*  Function Name: XmExt18ListSelectItems
 *  Description:   Set selection state by matching column entries to XmString
 *  Arguments:     w - the extended list widget
 *		   item - XmString to use as selection key
 *		   column - column number (0 - N) to match (or XmANY_COLUMN)
 *		   notify - if True, call XmNsingleSelectionCallback
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListSelectItems(Widget, XmString, int, Boolean);

/*  Function Name: XmExt18ListDeselectItems
 *  Description:   Set selection state by matching column entries to XmString
 *  Arguments:     w - the extended list widget
 *		   item - XmString to use as selection key
 *		   column - column number (0 - N) to match (or XmANY_COLUMN)
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListDeselectItems(Widget, XmString, int);

/*  Function Name: XmExt18ListSelectAllItems
 *  Description:   Set selection state on all rows
 *  Arguments:     w - the extended list widget
 *		   notify - if True, call XmNsingleSelectionCallback for each
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListSelectAllItems(Widget, Boolean);

/*  Function Name: XmExt18ListSelectRow
 *  Description:   Set selection state on all rows
 *  Arguments:     w - the extended list widget
 *		   row - the row to select
 *		   notify - if True, call XmNsingleSelectionCallback
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListSelectRow(Widget, int, Boolean);

/*  Function Name: XmExt18ListDeselectRow
 *  Description:   Set selection state on all rows
 *  Arguments:     w - the extended list widget
 *		   row - the row to select
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListDeselectRow(Widget, int);

/*
 * Function Name: XmExt18ListGetSelectedRowArray
 * Description:   Takes an Extended List and returns a NULL terminated array
 *                of pointers to selected rows from the internal list
 * Arguments:     w - the extended list widget
 *		  num_rows - pointer to the number of rows
 * Returns:       array of integer (selected) row numbers
 */
XM_DEPRECATED extern int *XmExt18ListGetSelectedRowArray(Widget, int *);

/*  Function Name: XmExt18ListMakeRowVisible
 *  Description:   Shifts the visible extended list rows as desired
 *  Arguments:     w - the extended list widget
 *		   row - the row number wished to be made visible
 *  Returns:       none
 */
XM_DEPRECATED extern void XmExt18ListMakeRowVisible(Widget, int);

#if defined(__cplusplus)
}
#endif
#endif /* _XmExt18List_h_ */
