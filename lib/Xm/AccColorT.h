/* $XConsortium: AccColorT.h /main/5 1995/07/15 20:47:59 drk $ */
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

#ifndef _XmAccessColorsT_H
#define _XmAccessColorsT_H

#include <Xm/Xm.h>
#include <X11/Xresource.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTaccessColors;

/* this one can be expanded in the future */
typedef struct _XmAccessColorDataRec {
    Mask  valueMask ;
    Pixel foreground  ;
    Pixel background  ;
    Pixel highlight_color  ;
    Pixel top_shadow_color  ;
    Pixel bottom_shadow_color ;
    Pixel select_color ;
} XmAccessColorDataRec, *XmAccessColorData;

typedef void (*XmAccessColorsGetProc)(Widget widget, 
				      XmAccessColorData color_data);
typedef void (*XmAccessColorsSetProc)(Widget widget, 
				      XmAccessColorData color_data);

/* Trait structures and typedefs, place typedefs first */

/* Version 0: initial release. */

typedef struct _XmAccessColorsTraitRec {
  int			version;	/* 0 */
  XmAccessColorsGetProc getColors;
  XmAccessColorsGetProc setColors;
} XmAccessColorsTraitRec, *XmAccessColorsTrait;

#define AccessColorInvalid         0L
#define AccessForeground           (1L<<0)  
#define AccessBackgroundPixel      (1L<<1)   
#define AccessHighlightColor       (1L<<2)   
#define AccessTopShadowColor       (1L<<3)   
#define AccessBottomShadowColor    (1L<<4)   
#define AccessSelectColor          (1L<<5)   

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* to do:

 add it to PushB/G and ToggleB/G so that they can report their
   select color
 implement the setValues ?

*/

#endif /* _XmAccessColorsT_H */
