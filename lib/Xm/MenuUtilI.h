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
/* $XConsortium: MenuUtilI.h /main/6 1996/07/23 16:45:49 pascale $ */
#ifndef _XmMenuUtilI_h
#define _XmMenuUtilI_h

#include <Xm/MenuUtilP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern void _XmMenuHelp(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern Boolean _XmIsActiveTearOff (
                         Widget w) ;
extern int _XmGrabPointer(
                        Widget widget,
                        int owner_events,
                        unsigned int event_mask,
                        int pointer_mode,
                        int keyboard_mode,
                        Window confine_to,
                        Cursor cursor,
                        Time time) ;
extern int _XmGrabKeyboard(
                        Widget widget,
                        int owner_events,
                        int pointer_mode,
                        int keyboard_mode,
                        Time time) ;
extern int _XmMenuGrabKeyboardAndPointer(
                        Widget widget,
			Time time) ;
extern void _XmMenuSetInPMMode(
			Widget wid,
                        Boolean flag) ;
extern void _XmSetMenuTraversal(
                        Widget wid,
                        Boolean traversalOn) ;
extern void _XmLeafPaneFocusOut(
                        Widget wid) ;
extern void _XmMenuTraverseLeft(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseRight(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseUp(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseDown(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuEscape(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmRC_GadgetTraverseDown(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseUp(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseLeft(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseRight(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraversalHandler(
                        Widget w,
                        Widget pw,
                        XmTraversalDirection direction) ;
/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMenuUtilI_h */
