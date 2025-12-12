/* $XConsortium: ContainerT.h /main/6 1996/02/09 15:05:04 drk $ */
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

#ifndef _XmContainerT_H
#define _XmContainerT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Xm/Xm.h>

externalref XrmQuark XmQTcontainer;

/* Trait structures and typedefs, place typedefs first */

/* this one can be expanded in the future */
typedef struct _XmContainerDataRec {
    Mask valueMask ;

    Cardinal * detail_order ;
    Cardinal detail_order_count ;
    XmTabList detail_tablist ;
    Dimension first_column_width ;
    XtEnum  selection_mode ;    /* XmNORMAL_MODE,	XmADD_MODE */
    Pixel   select_color ;

} XmContainerDataRec, *XmContainerData;

#define ContAllValid             (0xFFFF)
#define ContDetailOrder	         (1L<<0)
#define ContDetailTabList        (1L<<1)
#define ContFirstColumnWidth     (1L<<2)
#define ContSelectionMode        (1L<<3)
#define ContSelectColor          (1L<<4)


typedef void (*XmContainerGetValuesProc)(Widget w, 
					XmContainerData contData);

/* Version 0: initial release. */

typedef struct _XmContainerTraitRec {
  int			   version;	/* 0 */
  XmContainerGetValuesProc getValues;
} XmContainerTraitRec, *XmContainerTrait;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmContainerT_H */
