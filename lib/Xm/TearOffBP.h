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
 *  TearOffBP.h - Private definitions for TearOffButton widget 
 *  (Used by RowColumn Tear Off Menupanes)
 *
 */

#ifndef _XmTearOffBP_h
#define _XmTearOffBP_h

#include <Xm/PushBP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *
 * TearOffButton Widget Private Data
 *
 *****************************************************************************/

/* New fields for the TearOffButton widget class record */
typedef struct _XmTearOffButtonClassPart
{
    String translations;
} XmTearOffButtonClassPart;

/* Full Class record declaration */
typedef struct _XmTearOffButtonClassRec {
    CoreClassPart         core_class;
    XmPrimitiveClassPart  primitive_class;
    XmLabelClassPart      label_class;
    XmPushButtonClassPart pushbutton_class;
    XmTearOffButtonClassPart    tearoffbutton_class;
} XmTearOffButtonClassRec;

typedef struct _XmTearOffButtonClassRec *XmTearOffButtonWidgetClass;

externalref XmTearOffButtonClassRec xmTearOffButtonClassRec;

/* New fields for the TearOffButton widget record */
typedef struct {
   Dimension      margin;
   unsigned char  orientation;
   unsigned char separator_type;
   GC separator_GC;
   Boolean 	set_recompute_size;
} XmTearOffButtonPart;

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

typedef struct _XmTearOffButtonRec {
   CorePart         core;
   XmPrimitivePart  primitive;
   XmLabelPart      label;
   XmPushButtonPart pushbutton;
   XmTearOffButtonPart tear_off_button;
} XmTearOffButtonRec;

typedef struct _XmTearOffButtonRec      *XmTearOffButtonWidget;

/* Class Record Constant */

externalref WidgetClass xmTearOffButtonWidgetClass;

#ifndef XmIsTearOffButton
#define XmIsTearOffButton(w)	XtIsSubclass(w, xmTearOffButtonWidgetClass)
#endif /* XmIsTearOffButton */


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTearOffButtonP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
