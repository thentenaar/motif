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
#ifndef _XmLabel_h
#define _XmLabel_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Widget class and record definitions  */

externalref WidgetClass xmLabelWidgetClass;

typedef struct _XmLabelClassRec     * XmLabelWidgetClass;
typedef struct _XmLabelRec      * XmLabelWidget;

/*fast subclass define */
#ifndef XmIsLabel
#define XmIsLabel(w)     XtIsSubclass(w, xmLabelWidgetClass)
#endif /* XmIsLabel */


/********    Public Function Declarations    ********/

extern Widget XmCreateLabel( 
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount) ;
extern Widget XmVaCreateLabel(
        Widget parent,
        char *name,
        ...);
extern Widget XmVaCreateManagedLabel(
        Widget parent,
        char *name,
        ...);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmLabel_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
