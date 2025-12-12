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
/****************************************************************
 *
 * Vertical Paned Widget (SubClass of CompositeClass)
 *
 ****************************************************************/
#ifndef _XmPanedW_h
#define _XmPanedW_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Class record constant */
externalref WidgetClass xmPanedWindowWidgetClass;

#ifndef XmIsPanedWindow
#define XmIsPanedWindow(w)	XtIsSubclass(w, xmPanedWindowWidgetClass)
#endif /* XmIsPanedWindow */

typedef struct _XmPanedWindowClassRec  *XmPanedWindowWidgetClass;
typedef struct _XmPanedWindowRec	*XmPanedWindowWidget;


/********    Public Function Declarations    ********/

extern Widget XmCreatePanedWindow( 
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount) ;

/*
 * Variable argument list functions
 */

extern Widget XmVaCreatePanedWindow(
                        Widget parent,
                        char *name,
                        ...);

extern Widget XmVaCreateManagedPanedWindow(
                        Widget parent,
                        char *name,
                        ...);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPanedWindow_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
