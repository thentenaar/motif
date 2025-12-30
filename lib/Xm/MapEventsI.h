/**
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

/* $XConsortium: MapEventsI.h /main/5 1995/07/13 17:35:36 drk $ */
#ifndef _XmMapEventsI_h
#define _XmMapEventsI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for MapEvents.c    ********/

extern Boolean _XmMapBtnEvent(
                        String str,
                        int *eventType,
                        unsigned int *button,
                        Modifiers *modifiers) ;
extern int _XmMapKeyEvents(
                        String str,
                        int **eventType,
                        KeySym **keysym,
                        Modifiers **modifiers) ;
extern Boolean _XmMatchBtnEvent(
                        XEvent *event,
                        int eventType,
                        unsigned int button,
                        Modifiers modifiers) ;
extern Boolean _XmMatchKeyEvent(
                        XEvent *event,
                        int eventType,
                        unsigned int key,
                        Modifiers modifiers) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}
#endif
#endif /* _XmMapEventsI_h */

