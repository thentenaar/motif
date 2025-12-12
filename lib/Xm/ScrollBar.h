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
#ifndef _XmScrollBar_h
#define _XmScrollBar_h


#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif


/*  ScrollBar Widget  */

externalref WidgetClass xmScrollBarWidgetClass;

typedef struct _XmScrollBarClassRec * XmScrollBarWidgetClass;
typedef struct _XmScrollBarRec      * XmScrollBarWidget;

/* ifndef for Fast Subclassing  */

#ifndef XmIsScrollBar
#define XmIsScrollBar(w)	XtIsSubclass(w, xmScrollBarWidgetClass)
#endif  /* XmIsScrollBar */


/********    Public Function Declarations    ********/

extern Widget XmCreateScrollBar(
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern void XmScrollBarGetValues(
                        Widget w,
                        int *value,
                        int *slider_size,
                        int *increment,
                        int *page_increment) ;
extern void XmScrollBarSetValues(
                        Widget w,
                        int value,
                        int slider_size,
                        int increment,
                        int page_increment,
                        Boolean notify) ;

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateScrollBar(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedScrollBar(
                        Widget parent,
                        char *name,
                        ...);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScrollBar_h */
