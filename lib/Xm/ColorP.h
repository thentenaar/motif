/* $XConsortium: ColorP.h /main/4 1995/07/15 20:49:10 drk $ */
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

#ifndef _XmColorP_h
#define _XmColorP_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Default Color Allocation proc */

#define DEFAULT_ALLOCCOLOR_PROC       (XAllocColor)

/*  Defines and functions for processing dynamic defaults  */

#define XmMAX_SHORT 	65535

#define XmCOLOR_PERCENTILE (XmMAX_SHORT / 100)
#define BoundColor(value)\
	((value < 0) ? 0 : (((value > XmMAX_SHORT) ? XmMAX_SHORT : value)))

/* Contributions of each primary to overall luminosity, sum to 1.0 */

#define XmRED_LUMINOSITY 	0.30
#define XmGREEN_LUMINOSITY 	0.59
#define XmBLUE_LUMINOSITY 	0.11

/* Percent effect of intensity, light, and luminosity & on brightness,
   sum to 100 */

#define XmINTENSITY_FACTOR  75
#define XmLIGHT_FACTOR       0
#define XmLUMINOSITY_FACTOR 25

/* LITE color model
   percent to interpolate RGB towards black for SEL, BS, TS */

#define XmCOLOR_LITE_SEL_FACTOR  15
#define XmCOLOR_LITE_BS_FACTOR   40
#define XmCOLOR_LITE_TS_FACTOR   20

/* DARK color model
   percent to interpolate RGB towards white for SEL, BS, TS */

#define XmCOLOR_DARK_SEL_FACTOR  15
#define XmCOLOR_DARK_BS_FACTOR   30
#define XmCOLOR_DARK_TS_FACTOR   50

/* STD color model
   percent to interpolate RGB towards black for SEL, BS
   percent to interpolate RGB towards white for TS
   HI values used for high brightness (within STD)
   LO values used for low brightness (within STD)
   Interpolate factors between HI & LO values based on brightness */

#define XmCOLOR_HI_SEL_FACTOR  15
#define XmCOLOR_HI_BS_FACTOR   40
#define XmCOLOR_HI_TS_FACTOR   60

#define XmCOLOR_LO_SEL_FACTOR  15
#define XmCOLOR_LO_BS_FACTOR   60
#define XmCOLOR_LO_TS_FACTOR   50


/* For the default color calculation and caching */

#define XmLOOK_AT_SCREEN          (1<<0)
#define XmLOOK_AT_CMAP            (1<<1)
#define XmLOOK_AT_BACKGROUND      (1<<2)
#define XmLOOK_AT_FOREGROUND      (1<<3)
#define XmLOOK_AT_TOP_SHADOW      (1<<4)
#define XmLOOK_AT_BOTTOM_SHADOW   (1<<5)
#define XmLOOK_AT_SELECT          (1<<6)

#define XmBACKGROUND     ((unsigned char) (1<<0))
#define XmFOREGROUND     ((unsigned char) (1<<1))
#define XmTOP_SHADOW     ((unsigned char) (1<<2))
#define XmBOTTOM_SHADOW  ((unsigned char) (1<<3))
#define XmSELECT         ((unsigned char) (1<<4))
#define XmHIGHLIGHT      ((unsigned char) (1<<5))

/*  Structure used to hold color schemes  */
typedef struct _XmColorData
{  Screen * screen;
   Colormap color_map;
   unsigned char allocated;
   XColor background;
   XColor foreground;
   XColor top_shadow;
   XColor bottom_shadow;
   XColor select;
} XmColorData;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _ColorP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
