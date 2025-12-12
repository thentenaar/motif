/* $XConsortium: StringTrans.h /main/5 1995/07/15 20:41:54 drk $ */
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
/*******************************************************************************
 *
 * StringTrans.h: The widget public header file for the ExmStringTransfer
 *                uniform transfer model demonstration widget.
 *
 ******************************************************************************/


/* Ensure that the file be included only once. */
#ifndef _ExmStringTrans_h
#define _ExmStringTrans_h


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Include appropriate files. */
#include <Exm/ExmString.h>  /* public header file for the ExmString widget */


/* Define the widget class and widget record. */
externalref WidgetClass exmStringTransferWidgetClass;

typedef struct _ExmStringTransferClassRec *ExmStringTransferWidgetClass;
typedef struct _ExmStringTransferRec *ExmStringTransferWidget;


/* Define an IsSubclass macro. */
#ifndef ExmIsStringTransfer
#define ExmIsStringTransfer(w) XtIsSubclass(w, exmStringTransferWidgetClass)
#endif


/* No string equivalents of new resource names. */


/* Specify the API for this widget. */
extern Widget ExmCreateStringTransfer(Widget    parent,
                                      char     *name,
                                      Arg      *arglist,
                                      Cardinal  argCount);


/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _ExmStringTrans_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

