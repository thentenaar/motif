/* $XConsortium: Container.h /main/4 1995/07/15 20:47:08 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
/*****************************************************************************
 *
 * Container.h: The widget public header file.
 *
 ****************************************************************************/


/* Ensure that the file be included only once. */
#ifndef _ExmContainer_h
#define _ExmContainer_h


/* Include appropriate files. */
#include <Xm/Xm.h> /* widget public header file for XmManager */


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Define the widget class and widget record. */
externalref WidgetClass exmContainerWidgetClass;

typedef struct _ExmContainerClassRec * ExmContainerWidgetClass;
typedef struct _ExmContainerRec      * ExmContainerWidget;


/* Define an IsSubclass macro. */
#ifndef ExmIsContainer
#define ExmIsContainer(w) XtIsSubclass(w, exmContainerWidgetClass)
#endif


/* Define string equivalents of new resource names. */
#define ExmNfirstColumnWidth "firstColumnWidth"
#define ExmCFirstColumnWidth "FirstColumnWidth"

#define ExmNdetailOrder "detailOrder"
#define ExmCDetailOrder "DetailOrder"

#define ExmNdetailOrderCount "detailOrderCount"
#define ExmCDetailOrderCount "DetailOrderCount"

#define ExmNdetailTabList "detailTabList"
#define ExmCDetailTabList "DetailTabList"

#define ExmNdetailHeader "detailHeader"
#define ExmCDetailHeader "DetailHeader"

#define ExmNdetailHeaderCount "detailHeaderCount"
#define ExmCDetailHeaderCount "DetailHeaderCount"

#define ExmNselectColor "selectColor"
#define ExmCSelectColor "SelectColor"

#define ExmNlayoutType "layoutType"
#define ExmCLayoutType "LayoutType"


/* Specify the API for this widget. */
extern Widget ExmCreateContainer();


/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmContainer_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

