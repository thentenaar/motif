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
#ifndef _XmList_h
#define _XmList_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmListWidgetClass;

#define XmINITIAL 	0
#define XmADDITION	1
#define XmMODIFICATION	2

#ifndef XmIsList
#define XmIsList(w)	XtIsSubclass(w, xmListWidgetClass)
#endif /* XmIsList */

typedef struct _XmListClassRec * XmListWidgetClass;
typedef struct _XmListRec      * XmListWidget;

/********    Public Function Declarations    ********/

extern void XmListAddItem(
                        Widget w,
                        XmString item,
                        int pos) ;
extern void XmListAddItems(
                        Widget w,
                        XmString *items,
                        int item_count,
                        int pos) ;
extern void XmListAddItemsUnselected(
                        Widget w,
                        XmString *items,
                        int item_count,
                        int pos) ;
extern void XmListAddItemUnselected(
                        Widget w,
                        XmString item,
                        int pos) ;
extern void XmListDeleteItem(
                        Widget w,
                        XmString item) ;
extern void XmListDeleteItems(
                        Widget w,
                        XmString *items,
                        int item_count) ;
extern void XmListDeletePositions(
                        Widget    w,
                        int      *position_list,
                        int       position_count ) ;
extern void XmListDeletePos(
                        Widget w,
                        int pos) ;
extern void XmListDeleteItemsPos(
                        Widget w,
                        int item_count,
                        int pos) ;
extern void XmListDeleteAllItems(
                        Widget w) ;
extern void XmListReplaceItems(
                        Widget w,
                        XmString *old_items,
                        int item_count,
                        XmString *new_items) ;
extern void XmListReplaceItemsPos(
                        Widget w,
                        XmString *new_items,
                        int item_count,
                        int position) ;
extern void XmListReplaceItemsUnselected(
                        Widget w,
                        XmString *old_items,
                        int item_count,
                        XmString *new_items) ;
extern void XmListReplaceItemsPosUnselected(
                        Widget w,
                        XmString *new_items,
                        int item_count,
                        int position) ;
extern void XmListReplacePositions(
                        Widget    w,
                        int      *position_list,
                        XmString *item_list,
                        int       item_count ) ;
extern void XmListSelectItem(
                        Widget w,
                        XmString item,
                        Boolean notify) ;
extern void XmListSelectPos(
                        Widget w,
                        int pos,
                        Boolean notify) ;
extern void XmListDeselectItem(
                        Widget w,
                        XmString item) ;
extern void XmListDeselectPos(
                        Widget w,
                        int pos) ;
extern void XmListDeselectAllItems(
                        Widget w) ;
extern void XmListSetPos(
                        Widget w,
                        int pos) ;
extern void XmListSetBottomPos(
                        Widget w,
                        int pos) ;
extern void XmListSetItem(
                        Widget w,
                        XmString item) ;
extern void XmListSetBottomItem(
                        Widget w,
                        XmString item) ;
extern void XmListSetAddMode(
                        Widget w,
                        Boolean add_mode) ;
extern Boolean XmListItemExists(
                        Widget w,
                        XmString item) ;
extern int XmListItemPos(
                        Widget w,
                        XmString item) ;
extern int XmListGetKbdItemPos(
                        Widget w) ;
extern Boolean XmListSetKbdItemPos(
                        Widget w,
                        int    pos ) ;
extern int XmListYToPos(
                        Widget w,
                        Position y) ;
extern Boolean XmListPosToBounds(
                        Widget w,
                        int         position,
                        Position   *x,
                        Position   *y,
                        Dimension  *width,
                        Dimension  *height) ;
extern Boolean XmListGetMatchPos(
                        Widget w,
                        XmString item,
                        int **pos_list,
                        int *pos_count) ;
extern Boolean XmListGetSelectedPos(
                        Widget w,
                        int **pos_list,
                        int *pos_count) ;
extern void XmListSetHorizPos(
                        Widget w,
                        int position) ;
extern void XmListUpdateSelectedList(
                        Widget w) ;
extern Boolean XmListPosSelected(
			Widget 	w,
			int 	pos);
extern Widget XmCreateList(
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount) ;
extern Widget XmCreateScrolledList(
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount) ;
extern Widget XmVaCreateList(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedList(
                        Widget parent,
                        char *name,
                        ...);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmList_h */
