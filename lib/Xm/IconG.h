/* $XConsortium: IconG.h /main/5 1995/07/15 20:52:04 drk $ */
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
/*
 * HISTORY
 */
#ifndef _XmIconG_h
#define _XmIconG_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif



/* Class record constants */
extern	WidgetClass	xmIconGadgetClass;

typedef struct _XmIconGadgetClassRec * XmIconGadgetClass;
typedef struct _XmIconGadgetRec      * XmIconGadget;

#ifndef XmIsIconGadget
#define XmIsIconGadget(w) XtIsSubclass(w, xmIconGadgetClass)
#endif /* XmIsIconGadget */

/********    Public Function Declarations    ********/
extern Widget XmCreateIconGadget(
                        Widget parent,
                        String name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget XmVaCreateIconGadget(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedIconGadget(
                        Widget parent,
                        char *name,
                        ...);
/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmIconG_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
