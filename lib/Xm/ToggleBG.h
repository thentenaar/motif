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
/***********************************************************************
 *
 * Toggle Gadget
 *
 ***********************************************************************/
#ifndef _XmToggleG_h
#define _XmToggleG_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif


externalref WidgetClass xmToggleButtonGadgetClass;

typedef struct _XmToggleButtonGadgetClassRec     *XmToggleButtonGadgetClass;
typedef struct _XmToggleButtonGadgetRec          *XmToggleButtonGadget;
typedef struct _XmToggleButtonGCacheObjRec       *XmToggleButtonGCacheObject;


/*fast subclass define */
#ifndef XmIsToggleButtonGadget
#define XmIsToggleButtonGadget(w)     XtIsSubclass(w, xmToggleButtonGadgetClass)
#endif /* XmIsToggleButtonGadget */


/********    Public Function Declarations    ********/

Boolean XmToggleButtonGadgetGetState(
                        Widget w);
void XmToggleButtonGadgetSetState(
                        Widget w,
                        Boolean newstate,
                        Boolean notify);

Boolean XmToggleButtonGadgetSetValue(
					    Widget w,
					    XmToggleButtonState newstate,
					    Boolean notify);

Widget XmCreateToggleButtonGadget(
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount);
Widget XmVaCreateToggleButtonGadget(
        Widget parent,
        char *name,
        ...);
Widget XmVaCreateManagedToggleButtonGadget(
        Widget parent,
        char *name,
        ...);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmToggleG_h */
