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
#ifndef _XmTextOutI_h
#define _XmTextOutI_h

#include <Xm/TextOutP.h>

#ifdef __cplusplus
extern "C" {
#endif


/********    Private Function Declarations    ********/

extern void _XmTextFreeContextData(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
extern void _XmTextResetClipOrigin(
                        XmTextWidget tw,
                        XmTextPosition position,
                        Boolean clip_mask_reset) ;
extern void _XmTextAdjustGC(
                        XmTextWidget tw) ;
extern Boolean _XmTextShouldWordWrap(
                        XmTextWidget widget) ;
extern Boolean _XmTextScrollable(
                        XmTextWidget widget) ;
extern XmTextPosition _XmTextFindLineEnd(
                        XmTextWidget widget,
                        XmTextPosition position,
                        LineTableExtra *extra) ;
extern void _XmTextOutputGetSecResData(
                        XmSecondaryResourceData *secResDataRtn) ;
extern int _XmTextGetNumberLines(
                        XmTextWidget widget) ;
extern void _XmTextMovingCursorPosition(
                        XmTextWidget tw,
                        XmTextPosition position) ;
extern void _XmTextChangeBlinkBehavior(
                        XmTextWidget widget,
                        Boolean newvalue) ;
extern void _XmTextOutputCreate(
                        Widget wid,
                        ArgList args,
                        Cardinal num_args) ;
extern Boolean _XmTextGetBaselines(
                        Widget widget,
                        Dimension **baselines,
                        int *line_count) ;
extern Boolean _XmTextGetDisplayRect(
                        Widget w,
                        XRectangle *display_rect) ;
extern void _XmTextMarginsProc(
                        Widget w,
                        XmBaselineMargins *margins_rec) ;
extern void _XmTextChangeHOffset(
                        XmTextWidget widget,
                        int length) ;
extern void _XmTextChangeVOffset(
                        XmTextWidget widget,
                        int length) ;
extern void _XmTextToggleCursorGC(
                        Widget widget) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextOutI_h */
