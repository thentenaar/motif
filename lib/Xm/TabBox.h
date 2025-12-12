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
#ifndef _XmTabBox_h_
#define _XmTabBox_h_

#include <Xm/Ext.h>
#include <Xm/DrawUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmTabBoxRec      *XmTabBoxWidget;
typedef struct _XmTabBoxClassRec *XmTabBoxWidgetClass;

extern WidgetClass               xmTabBoxWidgetClass;

#ifndef XmIsTabBox
#define XmIsTabBox(w) XtIsSubclass(w, xmTabBoxWidgetClass)
#endif /* XmIsTabBox */

Widget XmCreateTabBox(Widget, String, ArgList, Cardinal);
int XmTabBoxGetIndex(Widget, int, int);
int XmTabBoxGetNumRows(Widget);
int XmTabBoxGetNumColumns(Widget);
int XmTabBoxGetNumTabs(Widget);
int XmTabBoxGetTabRow(Widget, int);
int XmTabBoxXYToIndex(Widget, int, int);

#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* __TabBox_h__ */
