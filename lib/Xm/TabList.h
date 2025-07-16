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
 *
 */
#ifndef _Xm_TabList_h_
#define _Xm_TabList_h_

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XmCOLOR_DYNAMIC ((Pixel)-1)
#define XmPIXMAP_DYNAMIC ((Pixmap) 3)
#define XmTAB_LAST_POSITION -1
#define XmTAB_NOT_FOUND -1

#define XmTAB_LABEL_STRING      (1L<<0)
#define XmTAB_LABEL_PIXMAP      (1L<<1)
#define XmTAB_PIXMAP_PLACEMENT  (1L<<2)
#define XmTAB_BACKGROUND        (1L<<3)
#define XmTAB_FOREGROUND        (1L<<4)
#define XmTAB_VALUE_MODE        (1L<<5)
#define XmTAB_LABEL_ALIGNMENT   (1L<<6)
#define XmTAB_STRING_DIRECTION  (1L<<7)
#define XmTAB_BACKGROUND_PIXMAP	(1L<<8)
#define XmTAB_SENSITIVE         (1L<<9)
#define XmTAB_ALL_FLAGS         (XmTAB_LABEL_STRING|XmTAB_LABEL_PIXMAP|\
				 XmTAB_PIXMAP_PLACEMENT|XmTAB_BACKGROUND|\
				 XmTAB_FOREGROUND|XmTAB_VALUE_MODE|\
				 XmTAB_LABEL_ALIGNMENT|XmTAB_STRING_DIRECTION|\
				 XmTAB_BACKGROUND_PIXMAP|XmTAB_SENSITIVE)

typedef struct _XmTabAttributeRec {
    XmString          label_string;	/* default: NULL                     */
    XmStringDirection string_direction; /* default: XmSTRING_DIRECTION_L_TO_R*/
    Pixmap            label_pixmap;	/* default: XmUNSPECIFIED_PIXMAP     */
    int               label_alignment;  /* default: XmALIGNEMENT_CENTER      */
    XmPixmapPlacement pixmap_placement; /* default: XmPIXMAP_RIGHT           */
    Pixel             foreground;       /* default: XmCOLOR_DYNAMIC          */
    Pixel             background;       /* default: XmCOLOR_DYNAMIC          */
    Pixmap            background_pixmap;/* default: XmPIXMAP_DYNAMIC         */
    Boolean	      sensitive;        /* default: True                     */
    XmTabValueMode    value_mode;       /* default: XmTAB_VALUE_COPY         */
} XmTabAttributeRec, * XmTabAttributes;

typedef struct _XmTabbedStackListRec *XmTabbedStackList;

XmTabbedStackList XmTabbedStackListCreate(void);

XmTabbedStackList XmTabbedStackListCopy(XmTabbedStackList);

void XmTabbedStackListFree(XmTabbedStackList);

void XmTabbedStackListRemove(XmTabbedStackList, int);

int XmTabbedStackListInsert(XmTabbedStackList, int, XtValueMask, XmTabAttributes);

int XmTabbedStackListAppend(XmTabbedStackList, XtValueMask, XmTabAttributes);

void XmTabbedStackListModify(XmTabbedStackList, int, XtValueMask, XmTabAttributes);

void XmTabbedStackListQuery(XmTabbedStackList, int, XmTabAttributes);

int XmTabbedStackListFind(XmTabbedStackList, XmString);

void XmTabbedStackListSimpleRemove(XmTabbedStackList, XmString);

int XmTabbedStackListSimpleInsert(XmTabbedStackList, int, XmString);

int XmTabbedStackListSimpleAppend(XmTabbedStackList, XmString);

void XmTabbedStackListSimpleModify(XmTabbedStackList, int, XmString);

XmString XmTabbedStackListSimpleQuery(XmTabbedStackList, int);

XmTabResult XmTabbedStackListCompare(XmTabbedStackList, XmTabbedStackList);

void XmTabAttibutesFree(XmTabAttributes);

#ifdef __cplusplus
}	/* Closes scope of 'extern "C"' declaration */
#endif

#endif /* _Xm_TabList_h_ */
