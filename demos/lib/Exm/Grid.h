/* $XConsortium: Grid.h /main/5 1995/07/15 20:40:49 drk $ */
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
 * Grid.h: The widget public header file for the ExmGrid demonstration
 *         widget.
 *
 ******************************************************************************/


/* Ensure that the file be included only once. */
#ifndef _ExmGrid_h
#define _ExmGrid_h


/* Include appropriate files. */
#include <Xm/Xm.h> /* widget public header file for XmManager */


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Define the widget class and widget record. */
externalref WidgetClass exmGridWidgetClass;

typedef struct _ExmGridClassRec * ExmGridWidgetClass;
typedef struct _ExmGridRec      * ExmGridWidget;


/* Define an IsSubclass macro. */
#ifndef ExmIsGrid
#define ExmIsGrid(w) XtIsSubclass(w, exmGridWidgetClass)
#endif


/* Define string equivalents of new resource names. */
#define ExmNgridMarginWidthWithinCell "gridMarginWidthWithinCell"
#define ExmNgridMarginHeightWithinCell "gridMarginHeightWithinCell"

#define ExmCGridMarginWidthWithinCell  "GridMarginWidthWithinCell"
#define ExmCGridMarginHeightWithinCell "GridMarginHeightWithinCell"


/* Specify the API for this widget. */
extern Widget ExmCreateGrid(
			Widget parent,
			char   *name,
			ArgList arglist,
			Cardinal argcount);
extern Widget ExmCreateGridDialog(
			Widget parent,
			char   *name,
			ArgList arglist,
			Cardinal argcount);

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmGrid_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

