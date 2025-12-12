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
/*
 * HISTORY
 */
/* $XConsortium: TextFI.h /main/5 1995/07/13 18:05:32 drk $ */
#ifndef _XmTextFI_h
#define _XmTextFI_h

#include <Xm/TextFP.h>

#ifdef __cplusplus
extern "C" {
#endif


/********    Private Function Declarations    ********/

extern int _XmTextFieldCountBytes(
                        XmTextFieldWidget tf,
                        wchar_t *wc_value,
                        int num_chars) ;
extern void _XmTextFToggleCursorGC(
                        Widget widget) ;
extern void _XmTextFieldDrawInsertionPoint(
                        XmTextFieldWidget tf,
                        Boolean turn_on) ;
extern void _XmTextFieldSetClipRect(
                        XmTextFieldWidget tf) ;
extern void _XmTextFieldSetCursorPosition(
                        XmTextFieldWidget tf,
                        XEvent *event,
                        XmTextPosition position,
                        Boolean adjust_flag,
                        Boolean call_cb) ;
extern Boolean _XmTextFieldReplaceText(
                        XmTextFieldWidget tf,
                        XEvent *event,
                        XmTextPosition replace_prev,
                        XmTextPosition replace_next,
                        char *insert,
                        int insert_length,
                        Boolean move_cursor) ;
extern void _XmTextFieldDeselectSelection(
                        Widget w,
                        Boolean disown,
                        Time sel_time) ;
extern Boolean _XmTextFieldSetDestination(
                        Widget w,
                        XmTextPosition position,
                        Time set_time) ;
extern void _XmTextFieldStartSelection(
                        XmTextFieldWidget tf,
                        XmTextPosition left,
                        XmTextPosition right,
                        Time sel_time) ;
extern void _XmTextFieldSetSel2(
                        Widget w,
                        XmTextPosition left,
                        XmTextPosition right,
                        Boolean disown,
                        Time sel_time) ;
extern void _XmTextFieldHandleSecondaryFinished(Widget w,
						XEvent *event);
extern int _XmTextFieldCountCharacters(XmTextFieldWidget tf,
				       char *ptr,
				       int n_bytes);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextFI_h */
