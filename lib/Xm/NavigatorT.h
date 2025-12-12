/* $XConsortium: NavigatorT.h /main/5 1995/07/15 20:53:08 drk $ */
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
#ifndef _XmNavigatorT_H
#define _XmNavigatorT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTnavigator;

/* Trait structures and typedefs, place typedefs first */


/* this structure is equivalent to an XPoint but in int,
   not in Position, which are short */
typedef struct _TwoDInt {
    int x;
    int y;
} TwoDIntRec, *TwoDInt;


/* this one can be expanded in the future */
typedef struct _XmNavigatorDataRec {
    Mask valueMask ;
    Mask dimMask ;
    TwoDIntRec value;
    TwoDIntRec minimum;
    TwoDIntRec maximum;
    TwoDIntRec slider_size;
    TwoDIntRec increment;
    TwoDIntRec page_increment;
} XmNavigatorDataRec, *XmNavigatorData;

#define NavAllValid             (OxFFFF)
#define NavDimMask		(1L<<0)
#define NavValue  		(1L<<1)
#define NavMinimum              (1L<<2)
#define NavMaximum		(1L<<3)
#define NavSliderSize		(1L<<4)
#define NavIncrement            (1L<<5)
#define NavPageIncrement	(1L<<6)



typedef void (*XmNavigatorMoveCBProc)(Widget nav, 
				      XtCallbackProc moveCB,
				      XtPointer closure,
				      Boolean setunset);
typedef void (*XmNavigatorSetValueProc)(Widget nav, 
					XmNavigatorData nav_data,
					Boolean notify);
typedef void (*XmNavigatorGetValueProc)(Widget nav, 
					XmNavigatorData nav_data);



/* Version 0: initial release. */

typedef struct _XmNavigatorTraitRec {
  int			  version;		/* 0 */
  XmNavigatorMoveCBProc   changeMoveCB;
  XmNavigatorSetValueProc setValue;
  XmNavigatorGetValueProc getValue;
} XmNavigatorTraitRec, *XmNavigatorTrait;


#define NavigDimensionX			(1L<<0)  
#define NavigDimensionY			(1L<<1)  

/* convenience Macros */
#define ACCESS_DIM(mask,field) ((mask & NavigDimensionX)?(field.x):(field.y))

#define ASSIGN_DIM(mask,field,val)	\
  {					\
    if (mask & NavigDimensionX)		\
      (field.x)=(val);			\
    else				\
      (field.y)=(val);			\
  }


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmNavigatorT_H */
