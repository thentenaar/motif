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
 * DrawnButton Widget
 *
 ***********************************************************************/

#ifndef _XmDButton_h
#define _XmDButton_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XmIsDrawnButton
#define XmIsDrawnButton(w) XtIsSubclass(w, xmDrawnButtonWidgetClass)
#endif /* XmIsDrawnButton */

/* DrawnButon Widget */

externalref WidgetClass xmDrawnButtonWidgetClass;

typedef struct _XmDrawnButtonClassRec *XmDrawnButtonWidgetClass;
typedef struct _XmDrawnButtonRec      *XmDrawnButtonWidget;


/********    Public Function Declarations    ********/

extern Widget XmCreateDrawnButton( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget XmVaCreateDrawnButton(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedDrawnButton(
                        Widget parent,
                        char *name,
                        ...);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDButton_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
