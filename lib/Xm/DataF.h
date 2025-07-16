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
#ifndef _XmDataF_h
#define _XmDataF_h

#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/Ext.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _XmDataFieldClassRec    *XmDataFieldWidgetClass;
typedef struct _XmDataFieldRec         *XmDataFieldWidget;

/*      Function Name: XmCreateDataField
 *      Description: Creation Routine for UIL and ADA.
 *      Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *      Returns: The Widget created.
 */
Widget XmCreateDataField(Widget, String, ArgList, Cardinal);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateDataField(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedDataField(
                        Widget parent,
                        char *name,
                        ...);

Boolean _XmDataFieldReplaceText(XmDataFieldWidget, XEvent*, XmTextPosition, XmTextPosition, char*, int, Boolean);

extern void XmDataFieldSetString(Widget, char *);

extern char * XmDataFieldGetString(Widget);

extern wchar_t * XmDataFieldGetStringWcs(Widget);

void _XmDataFieldSetClipRect(XmDataFieldWidget);

void _XmDataFieldDrawInsertionPoint(XmDataFieldWidget, Boolean);

void XmDataFieldSetHighlight(Widget, XmTextPosition, XmTextPosition, XmHighlightMode);

void XmDataFieldSetAddMode(Widget, Boolean);

char * XmDataFieldGetSelection(Widget);

void XmDataFieldSetSelection(Widget, XmTextPosition, XmTextPosition, Time);

void _XmDataFieldSetSel2(Widget, XmTextPosition, XmTextPosition, Boolean, Time);

Boolean XmDataFieldGetSelectionPosition(Widget, XmTextPosition *, XmTextPosition *);

XmTextPosition XmDataFieldXYToPos(Widget, Position, Position);

void XmDataFieldShowPosition(Widget, XmTextPosition);

Boolean XmDataFieldCut(Widget, Time);

Boolean XmDataFieldCopy(Widget, Time);

Boolean XmDataFieldPaste(Widget);

void XmDataFieldSetEditable(Widget, Boolean);

void XmDataFieldSetInsertionPosition(Widget, XmTextPosition);

extern WidgetClass xmDataFieldWidgetClass;

typedef struct _XmDataFieldCallbackStruct {
    Widget   w;			/* The XmDataField */
    String   text;		/* Proposed string */
    Boolean  accept;		/* Accept return value, for validation */
} XmDataFieldCallbackStruct;

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* _XmDataF_h */
