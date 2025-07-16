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
/***********************************************************************
 *
 * Toggle Widget
 *
 ***********************************************************************/
#ifndef _XmToggle_h
#define _XmToggle_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmToggleButtonWidgetClass;

typedef struct _XmToggleButtonClassRec *XmToggleButtonWidgetClass;
typedef struct _XmToggleButtonRec      *XmToggleButtonWidget;

/*fast subclass define */
#ifndef XmIsToggleButton
#define XmIsToggleButton(w)     XtIsSubclass(w, xmToggleButtonWidgetClass)
#endif /* XmIsToggleButton */


/********    Public Function Declarations    ********/

Boolean XmToggleButtonGetState(
                        Widget w);
void XmToggleButtonSetState(
                        Widget w,
                        Boolean newstate,
                        Boolean notify);

Boolean XmToggleButtonSetValue(
        Widget w,
        XmToggleButtonState newstate,
        Boolean notify );
Widget XmCreateToggleButton(
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount);
Widget XmVaCreateToggleButton(
                        Widget parent,
                        char *name,
                        ...);
Widget XmVaCreateManagedToggleButton(
                        Widget parent,
                        char *name,
                        ...);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmToggle_h */
