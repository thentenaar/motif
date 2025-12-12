/* $XConsortium: ContItemT.h /main/5 1995/07/15 20:49:36 drk $ */
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
#ifndef _XmContainerItemT_H
#define _XmContainerItemT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTcontainerItem;

/* Trait structures and typedefs, place typedefs first */

/* this one can be expanded in the future */
typedef struct _XmContainerItemDataRec {
    Mask valueMask ;        /* on setValues, give the information on
			     what to change in the Icon, on getValues,
			     on what to put in the record returned */
    unsigned char view_type;
    unsigned char visual_emphasis;
    Dimension icon_width ;    /* get value */
    Cardinal detail_count;   /* get value */
} XmContainerItemDataRec, *XmContainerItemData;

#define ContItemAllValid             (0xFFFF)
#define ContItemViewType	     (1L<<0)
#define ContItemVisualEmphasis	     (1L<<1)
#define ContItemIconWidth            (1L<<2)
#define ContItemDetailCount          (1L<<3)


typedef void (*XmContainerItemSetValuesProc)(Widget w, 
					XmContainerItemData contItemData);
typedef void (*XmContainerItemGetValuesProc)(Widget w, 
					XmContainerItemData contItemData);

/* Version 0: initial release. */

typedef struct _XmContainerItemTraitRec {
  int			       version;		/* 0 */
  XmContainerItemSetValuesProc setValues;
  XmContainerItemGetValuesProc getValues;
} XmContainerItemTraitRec, *XmContainerItemTrait;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmContainerItemT_H */
