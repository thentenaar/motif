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
#ifndef __XmTabStack_h__
#define __XmTabStack_h__

#include <Xm/Ext.h>
#include <Xm/TabBox.h>
#include <Xm/TabList.h>
#include <Xm/DrawUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmTabStackRec      *XmTabStackWidget;
typedef struct _XmTabStackClassRec *XmTabStackWidgetClass;
extern WidgetClass                 xmTabStackWidgetClass;

#ifndef XmIsTabStack
#define XmIsTabStack(w) XtIsSubclass(w, xmTabStackWidgetClass)
#endif /* XmIsTabStack */

extern Widget XmCreateTabStack(Widget, String, ArgList, Cardinal);
extern Widget XmTabStackGetSelectedTab(Widget);
extern void   XmTabStackSelectTab(Widget, Boolean);
extern Widget XmTabStackIndexToWidget(Widget, int);
extern Widget XmTabStackXYToWidget(Widget, int, int);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateTabStack(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedTabStack(
                        Widget parent,
                        char *name,
                        ...);

#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif


#endif /* __TabStack_h__ */
