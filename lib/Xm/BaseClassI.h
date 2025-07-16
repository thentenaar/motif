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
/*
 * HISTORY
 */
/* $XConsortium: BaseClassI.h /main/6 1995/07/14 10:10:58 drk $ */
#ifndef _XmBaseClassI_h
#define _XmBaseClassI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern void _XmPushWidgetExtData(
                        Widget widget,
                        XmWidgetExtData data,
                        unsigned char extType) ;
extern void _XmPopWidgetExtData(
                        Widget widget,
                        XmWidgetExtData *dataRtn,
                        unsigned char extType) ;
extern XmWidgetExtData _XmGetWidgetExtData(
                        Widget widget,
                        unsigned char extType) ;
extern void _XmInitializeExtensions( void ) ;
extern void _XmTransformSubResources(
                        XtResourceList comp_resources,
                        Cardinal num_comp_resources,
                        XtResourceList *resources,
                        Cardinal *num_resources) ;

extern Cardinal _XmSecondaryResourceData(
                        XmBaseClassExt bcePtr,
                        XmSecondaryResourceData **secResDataRtn,
                        XtPointer client_data,
                        String name,
                        String class_name,
                        XmResourceBaseProc basefunctionpointer) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmBaseClassI_h */
