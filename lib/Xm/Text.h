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
#ifndef _XmText_h
#define _XmText_h

#include <Xm/Xm.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------- *
 *   type defines *
 * -------------- */
typedef struct _XmTextSourceRec *XmTextSource;
typedef struct _XmTextClassRec *XmTextWidgetClass;
typedef struct _XmTextRec *XmTextWidget;

/* -------------- *
 * extern class   *
 * -------------- */
externalref WidgetClass       xmTextWidgetClass;


/* --------------------------------------- *
 *  text widget fast subclassing fallback  *
 * --------------------------------------- */

#ifndef XmIsText
#define XmIsText(w)	XtIsSubclass(w, xmTextWidgetClass)
#endif /* XmIsText */


/* ----------------------------------- *
 *   text widget public functions      *
 * ----------------------------------- */

/********    Public Function Declarations    ********/

extern void XmTextSetHighlight(
                        Widget w,
                        XmTextPosition left,
                        XmTextPosition right,
                        XmHighlightMode mode);
extern Widget XmCreateScrolledText(
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateText(
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmVaCreateText(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedText(
                        Widget parent,
                        char *name,
                        ...);
extern int XmTextGetSubstring(
                        Widget widget,
                        XmTextPosition start,
                        int num_chars,
                        int buf_size,
                        char *buffer);
extern int XmTextGetSubstringWcs(
                        Widget widget,
                        XmTextPosition start,
                        int num_chars,
                        int buf_size,
                        wchar_t *buffer);
extern char * XmTextGetString(
                        Widget widget);
extern wchar_t * XmTextGetStringWcs(
                        Widget widget);
extern XmTextPosition XmTextGetLastPosition(
                        Widget widget);
extern void XmTextSetString(
                        Widget widget,
                        char *value);
extern void XmTextSetStringWcs(
                        Widget widget,
                        wchar_t *wc_value);
extern void XmTextReplace(
                        Widget widget,
                        XmTextPosition frompos,
                        XmTextPosition topos,
                        char *value);
extern void XmTextReplaceWcs(
                        Widget widget,
                        XmTextPosition frompos,
                        XmTextPosition topos,
                        wchar_t *value);
extern void XmTextInsert(
                        Widget widget,
                        XmTextPosition position,
                        char *value);
extern void XmTextInsertWcs(
                        Widget widget,
                        XmTextPosition position,
                        wchar_t *wc_value);
extern void XmTextSetAddMode(
                        Widget widget,
                        Boolean state);
extern Boolean XmTextGetAddMode(
                        Widget widget);
extern Boolean XmTextGetEditable(
                        Widget widget);
extern void XmTextSetEditable(
                        Widget widget,
                        Boolean editable);
extern int XmTextGetMaxLength(
                        Widget widget);
extern void XmTextSetMaxLength(
                        Widget widget,
                        int max_length);
extern XmTextPosition XmTextGetTopCharacter(
                        Widget widget);
extern void XmTextSetTopCharacter(
                        Widget widget,
                        XmTextPosition top_character);
extern XmTextPosition XmTextGetCursorPosition(
                        Widget widget);
extern XmTextPosition XmTextGetInsertionPosition(
                        Widget widget);
extern void XmTextSetInsertionPosition(
                        Widget widget,
                        XmTextPosition position);
extern void XmTextSetCursorPosition(
                        Widget widget,
                        XmTextPosition position);
extern Boolean XmTextRemove(
                        Widget widget);
extern Boolean XmTextCopy(
                        Widget widget,
                        Time copy_time);
extern Boolean XmTextCopyLink(
                        Widget widget,
                        Time copy_time);
extern Boolean XmTextCut(
                        Widget widget,
                        Time cut_time);
extern Boolean XmTextPaste(
                        Widget widget);
extern Boolean XmTextPasteLink(
                        Widget widget);
extern char * XmTextGetSelection(
                        Widget widget);
extern wchar_t * XmTextGetSelectionWcs(
                        Widget widget);
extern void XmTextSetSelection(
                        Widget widget,
                        XmTextPosition first,
                        XmTextPosition last,
                        Time set_time);
extern void XmTextClearSelection(
                        Widget widget,
                        Time clear_time);
extern Boolean XmTextGetSelectionPosition(
                        Widget widget,
                        XmTextPosition *left,
                        XmTextPosition *right);
extern XmTextPosition XmTextXYToPos(
                        Widget widget,
                        Position x,
                        Position y);
extern Boolean XmTextPosToXY(
                        Widget widget,
                        XmTextPosition position,
                        Position *x,
                        Position *y);
extern XmTextSource XmTextGetSource(
                        Widget widget);
extern void XmTextSetSource(
                        Widget widget,
                        XmTextSource source,
                        XmTextPosition top_character,
                        XmTextPosition cursor_position);
extern void XmTextShowPosition(
                        Widget widget,
                        XmTextPosition position);
extern void XmTextScroll(
                        Widget widget,
                        int n);
extern int XmTextGetBaseline(
                        Widget widget);
extern int XmTextGetCenterline(
                        Widget widget);
extern void XmTextDisableRedisplay(
                        Widget widget);
extern void XmTextEnableRedisplay(
                        Widget widget);
extern Boolean XmTextFindString(
                        Widget w,
                        XmTextPosition start,
                        char *search_string,
                        XmTextDirection direction,
                        XmTextPosition *position);
extern Boolean XmTextFindStringWcs(
                        Widget w,
                        XmTextPosition start,
                        wchar_t *wc_string,
                        XmTextDirection direction,
                        XmTextPosition *position);

/********    End Public Function Declarations    ********/

/* tmp: go to XmStrDefs */
#define XmNtotalLines "totalLines"
#define XmCTotalLines "TotalLines"

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmText_h */
