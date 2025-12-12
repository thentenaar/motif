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
#ifndef _XmScaleP_h
#define _XmScaleP_h


#include <Xm/Scale.h>
#include <Xm/ManagerP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constraint part record for Scale widget */

typedef struct _XmScaleConstraintPart
{
   char unused;
} XmScaleConstraintPart, * XmScaleConstraint;


/*  New fields for the Scale widget class record  */

typedef struct
{
   XtPointer extension;   /* Pointer to extension record */
} XmScaleClassPart;


/* Full class record declaration */

typedef struct _XmScaleClassRec
{
   CoreClassPart       core_class;
   CompositeClassPart  composite_class;
   ConstraintClassPart constraint_class;
   XmManagerClassPart  manager_class;
   XmScaleClassPart    scale_class;
} XmScaleClassRec;

externalref XmScaleClassRec xmScaleClassRec;


/* New fields for the Scale widget record */

typedef struct
{
   int            value;
   int            maximum;
   int            minimum;
   unsigned char  orientation;
   unsigned char  processing_direction;
   XmString       title; 
   XmFontList     font_list;
   XFontStruct  * font_struct;
   Boolean        show_value;
   short          decimal_points;
   Dimension      scale_width;
   Dimension      scale_height;
   Dimension      highlight_thickness;
   Boolean        highlight_on_enter;
   XtCallbackList drag_callback;
   XtCallbackList value_changed_callback;

   /* this field is unused since 1.2 and
      has a new meaning in 2.0: a bitfield
      that carries instance states: FROM_SET_VALUE, etc
      The field will be referenced as scale.state_flags
      using a define in the .c file */
   /* Note: Instead, last_value is now being used to resolve between
    * XmRenderTable & XmFontList when setting up the resource table
    */
   int last_value;

   int slider_size;
   GC  foreground_GC;
   int show_value_x;
   int show_value_y;
   int show_value_width;
   int show_value_height;
   int scale_multiple;

   XtEnum sliding_mode;
   XtEnum slider_visual;
   XtEnum slider_mark;
   XtEnum show_arrows;
   Boolean editable;

   XtCallbackList   convert_callback;       /* Selection convert callback */

   Region value_region;
} XmScalePart;


#define FROM_SET_VALUE (1<<0)

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XmScaleRec
{
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;
    XmScalePart    scale;
} XmScaleRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScaleP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
