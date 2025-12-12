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
/* $XConsortium: TextInI.h /main/5 1995/07/13 18:07:40 drk $ */
#ifndef _XmTextInI_h
#define _XmTextInI_h

#include <Xm/TextInP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern Boolean _XmTextHasDestination(
                        Widget w) ;
extern Boolean _XmTextSetDestinationSelection(
                        Widget w,
                        XmTextPosition position,
                        Boolean disown,
                        Time set_time) ;
extern Boolean _XmTextSetSel2(
                        XmTextWidget tw,
                        XmTextPosition left,
                        XmTextPosition right,
                        Time set_time) ;
extern Boolean _XmTextGetSel2(
                        XmTextWidget tw,
                        XmTextPosition *left,
                        XmTextPosition *right) ;
extern void _XmTextInputGetSecResData(
                        XmSecondaryResourceData *secResDataRtn) ;
extern void _XmTextInputCreate(
                        Widget wid,
                        ArgList args,
                        Cardinal num_args) ;
extern void _XmTextHandleSecondaryFinished(Widget w,
					   XEvent *event);


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextInI_h */
