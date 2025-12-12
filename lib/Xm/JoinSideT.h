/* $XConsortium: JoinSideT.h /main/5 1995/07/15 20:52:34 drk $ */
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
#ifndef _XmJoinSideT_H
#define _XmJoinSideT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTjoinSide;

/* Trait structures and typedefs, place typedefs first */

typedef void (*XmJoinSideSetValueProc)(Widget tab,
				       unsigned char join_side, 
				       Dimension join_thickness) ;
typedef unsigned char (*XmJoinSideGetValueProc)(Widget tab,
						Dimension * join_thickness);


/* Version 0: initial release. */

typedef struct _XmJoinSideTraitRec {
  int			 version;	/* 0 */
  XmJoinSideSetValueProc setValue;
  XmJoinSideGetValueProc getValue;
} XmJoinSideTraitRec, *XmJoinSideTrait;

enum {/* XmNONE already defined in Xm.h */ 
	 XmLEFT = 1, XmRIGHT, XmTOP, XmBOTTOM} ;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmJoinSideT_H */
