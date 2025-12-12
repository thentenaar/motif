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
#ifndef _XmScale_h
#define _XmScale_h


#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Class record constants */

externalref WidgetClass xmScaleWidgetClass;

/* fast XtIsSubclass define */
#ifndef XmIsScale
#define XmIsScale(w) XtIsSubclass (w, xmScaleWidgetClass)
#endif

typedef struct _XmScaleClassRec * XmScaleWidgetClass;
typedef struct _XmScaleRec      * XmScaleWidget;


/********    Public Function Declarations    ********/

void XmScaleSetValue( 
                 Widget w,
                 int value) ;
void XmScaleGetValue( 
                 Widget w,
                 int *value) ;
Widget XmCreateScale( 
                 Widget parent,
                 char *name,
                 ArgList arglist,
                 Cardinal argcount) ;
Widget XmVaCreateScale(
                 Widget parent,
                 char *name,
                 ...);
Widget XmVaCreateManagedScale(
                 Widget parent,
                 char *name,
                 ...);
void XmScaleSetTicks(
                     Widget scale,
                     int big_every,
		     Cardinal num_med,
		     Cardinal num_small, 
		     Dimension  size_big,
		     Dimension  size_med,
		     Dimension  size_small);
/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScale_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
