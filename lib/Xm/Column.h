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

#ifndef __Xmcolumn_h__
#define __Xmcolumn_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <Xm/Xm.h>
#include <Xm/Ext.h>

extern WidgetClass xmColumnWidgetClass;

typedef struct _XmColumnClassRec * XmColumnWidgetClass;
typedef struct _XmColumnRec      * XmColumnWidget;

#ifndef XmIsColumn
#define XmIsColumn(w) (XtIsSubclass(w, xmColumnWidgetClass))
#endif

extern Widget XmCreateColumn(Widget, String, ArgList, Cardinal);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateColumn(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedColumn(
                        Widget parent,
                        char *name,
                        ...);
#ifdef __cplusplus
}
#endif

#endif /* __column_h__ */
