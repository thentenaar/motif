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
*/
#ifndef _XmTextF_h
#define _XmTextF_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************
 * type defines   *
 ******************/
typedef struct _XmTextFieldClassRec *XmTextFieldWidgetClass;
typedef struct _XmTextFieldRec *XmTextFieldWidget;

/******************
 * extern class   *
 ******************/
externalref WidgetClass       xmTextFieldWidgetClass;


/************************
 * fast subclass define *
 ************************/
#ifndef XmIsTextField
#define XmIsTextField(w)     XtIsSubclass(w, xmTextFieldWidgetClass)
#endif /* XmIsTextField */


/********************
 * public functions *
 ********************/

/********    Public Function Declarations    ********/

char * XmTextFieldGetString(
                 Widget w);
int XmTextFieldGetSubstring(
                 Widget widget,
                 XmTextPosition start,
                 int num_chars,
                 int buf_size,
                 char *buffer);
wchar_t * XmTextFieldGetStringWcs(
                 Widget w);
int XmTextFieldGetSubstringWcs(
                 Widget widget,
                 XmTextPosition start,
                 int num_chars,
                 int buf_size,
                 wchar_t *buffer);
XmTextPosition XmTextFieldGetLastPosition(
                 Widget w);
void XmTextFieldSetString(
                 Widget w,
                 char *value);
void XmTextFieldSetStringWcs(
                 Widget w,
                 wchar_t *wc_value);
void XmTextFieldReplace(
                 Widget w,
                 XmTextPosition from_pos,
                 XmTextPosition to_pos,
                 char *value);
void XmTextFieldReplaceWcs(
                 Widget w,
                 XmTextPosition from_pos,
                 XmTextPosition to_pos,
                 wchar_t *wc_value);
void XmTextFieldInsert(
                 Widget w,
                 XmTextPosition position,
                 char *value);
void XmTextFieldInsertWcs(
                 Widget w,
                 XmTextPosition position,
                 wchar_t *wcstring);
void XmTextFieldSetAddMode(
                        Widget w,
                        Boolean state);
Boolean XmTextFieldGetAddMode(
                 Widget w);
Boolean XmTextFieldGetEditable(
                 Widget w);
void XmTextFieldSetEditable(
                        Widget w,
                        Boolean editable);
int XmTextFieldGetMaxLength(
                 Widget w);
void XmTextFieldSetMaxLength(
                 Widget w,
                 int max_length);
XmTextPosition XmTextFieldGetCursorPosition(
                 Widget w);
XmTextPosition XmTextFieldGetInsertionPosition(
                 Widget w);
void XmTextFieldSetCursorPosition(
                 Widget w,
                 XmTextPosition position);
void XmTextFieldSetInsertionPosition(
                 Widget w,
                 XmTextPosition position);
Boolean XmTextFieldGetSelectionPosition(
                 Widget w,
                 XmTextPosition *left,
                 XmTextPosition *right);
char * XmTextFieldGetSelection(
                 Widget w);
wchar_t * XmTextFieldGetSelectionWcs(
                 Widget w);
Boolean XmTextFieldRemove(
                 Widget w);
Boolean XmTextFieldCopy(
                 Widget w,
                 Time clip_time);
Boolean XmTextFieldCopyLink(
                 Widget w,
                 Time clip_time);
Boolean XmTextFieldCut(
                 Widget w,
                 Time clip_time);
Boolean XmTextFieldPaste(
                 Widget w);
Boolean XmTextFieldPasteLink(
                 Widget w);
void XmTextFieldClearSelection(
                 Widget w,
                 Time sel_time);
void XmTextFieldSetSelection(
                 Widget w,
                 XmTextPosition first,
                 XmTextPosition last,
                 Time sel_time);
XmTextPosition XmTextFieldXYToPos(
                        Widget w,
                        Position x,
                        Position y);
Boolean XmTextFieldPosToXY(
                 Widget w,
                 XmTextPosition position,
                 Position *x,
                 Position *y);
void XmTextFieldShowPosition(
                 Widget w,
                 XmTextPosition position);
void XmTextFieldSetHighlight(
                 Widget w,
                 XmTextPosition left,
                 XmTextPosition right,
                 XmHighlightMode mode);
int XmTextFieldGetBaseline(
                 Widget w);
Widget XmCreateTextField(
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
Widget XmVaCreateTextField(
                        Widget parent,
                        char *name,
                        ...);
Widget XmVaCreateManagedTextField(
                        Widget parent,
                        char *name,
                        ...);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextF_h */
