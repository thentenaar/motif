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
/* $XConsortium: SelectioBI.h /main/5 1995/07/13 17:57:50 drk $ */
#ifndef _XmSelectioBI_h
#define _XmSelectioBI_h

#include <Xm/SelectioBP.h>

#ifdef __cplusplus
extern "C" {
#endif


/********    Private Function Declarations    ********/

extern void _XmSelectionBoxCreateListLabel(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateSelectionLabel(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateList(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateText(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateSeparator(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateOkButton(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateApplyButton(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateCancelButton(
                        XmSelectionBoxWidget sel) ;
extern void _XmSelectionBoxCreateHelpButton(
                        XmSelectionBoxWidget sel) ;
extern XmGeoMatrix _XmSelectionBoxGeoMatrixCreate(
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *desired) ;
extern Boolean _XmSelectionBoxNoGeoRequest(
                        XmGeoMatrix geoSpec) ;
extern void _XmSelectionBoxGetSelectionLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetListLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetTextColumns(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetTextString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetListItems(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetListItemCount(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetListVisibleItemCount(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetOkLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetApplyLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetCancelLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxGetHelpLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
extern void _XmSelectionBoxUpOrDown(
                        Widget wid,
                        XEvent *event,
                        String *argv,
                        Cardinal *argc) ;
extern void _XmSelectionBoxRestore(
                        Widget wid,
                        XEvent *event,
                        String *argv,
                        Cardinal *argc) ;

Widget _XmSelectionBoxGetChild(Widget sb, unsigned char which);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmSelectioBI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
