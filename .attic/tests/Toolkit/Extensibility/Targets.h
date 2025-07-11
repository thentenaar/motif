/* $XConsortium: Targets.h /main/4 1995/07/15 21:04:01 drk $ */
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
/*
 * HISTORY
 */

/*******************************************************************************
 *
 * Targets.h: The widget public header file for the ExmTargets
 *                uniform transfer model demonstration widget.
 *
 ******************************************************************************/


/* Ensure that the file be included only once. */
#ifndef _ExmTargets_h
#define _ExmTargets_h


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Include appropriate files. */
#include <Exm/String.h>  /* public header file for the ExmString widget */


/* Define the widget class and widget record. */
externalref WidgetClass exmTargetsWidgetClass;

typedef struct _ExmTargetsClassRec *ExmTargetsWidgetClass;
typedef struct _ExmTargetsRec *ExmTargetsWidget;


/* Define an IsSubclass macro. */
#ifndef ExmIsTargets
#define ExmIsTargets(w) XtIsSubclass(w, exmTargetsWidgetClass)
#endif


/* No string equivalents of new resource names. */


/* Specify the API for this widget. */
extern Widget ExmCreateTargets();


/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmTargets_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
