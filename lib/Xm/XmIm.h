/* $XConsortium: XmIm.h /main/7 1996/05/21 12:13:36 pascale $ */
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
 */
/*
 * HISTORY
 */
#ifndef _XmIm_h
#define _XmIm_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Public Function Declarations    ********/

extern void XmImRegister(
                        Widget w,
			unsigned int reserved) ;
extern void XmImUnregister(
                        Widget w) ;
extern void XmImSetFocusValues(
                        Widget w,
                        ArgList args,
                        Cardinal num_args) ;
extern void XmImSetValues(
                        Widget w,
                        ArgList args,
                        Cardinal num_args) ;
extern void XmImUnsetFocus(
                        Widget w) ;
extern XIM XmImGetXIM(
                        Widget w) ;
extern void XmImCloseXIM(
                        Widget w) ;

extern int XmImMbLookupString(
                        Widget w,
                        XKeyPressedEvent *event,
                        char *buf,
                        int nbytes,
                        KeySym *keysym,
                        int *status) ;
extern void XmImVaSetFocusValues(
                        Widget w,
                        ...) ;
extern void XmImVaSetValues(
                        Widget w,
                        ...) ;
extern XIC XmImGetXIC(
		        Widget 		w,
		        XmInputPolicy	input_policy,
		        ArgList		args,
		        Cardinal	num_args) ;
extern XIC XmImSetXIC(
			Widget w,
			XIC    input_context) ;
extern void XmImFreeXIC(
			Widget w,
			XIC    input_context) ;

extern void XmImMbResetIC(
			Widget w,
			char **mb);

extern XIMResetState XmImGetXICResetState(
			Widget w);

/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmIm_h */
