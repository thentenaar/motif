/* $XConsortium: CareVisualT.h /main/5 1995/07/15 20:48:21 drk $ */
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
#ifndef _XmCareVisualT_H
#define _XmCareVisualT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTcareParentVisual;

/* Trait structures and typedefs, place typedefs first */

typedef Boolean (*XmCareVisualRedrawProc)(Widget kid, 
					  Widget cur_parent,
					  Widget new_parent,
					  Mask visual_flag);

/* Version 0: initial release. */

typedef struct _XmCareVisualTraitRec {
  int 			 version;	/* 0 */
  XmCareVisualRedrawProc redraw;
} XmCareVisualTraitRec, *XmCareVisualTrait;


#define NoVisualChange                    0L
#define VisualForeground                  (1L<<0)  
#define VisualHighlightPixmap             (1L<<1)                              
#define VisualHighlightColor              (1L<<2)   
#define VisualBottomShadowPixmap          (1L<<3)   
#define VisualBottomShadowColor           (1L<<4)   
#define VisualTopShadowPixmap             (1L<<5)   
#define VisualTopShadowColor              (1L<<6)   
#define VisualBackgroundPixel             (1L<<7)   
#define VisualBackgroundPixmap            (1L<<8)   
#define VisualSelectColor                 (1L<<9)   


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCareVisualT_H */
