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
#ifndef _XmFrameP_h
#define _XmFrameP_h

#include <Xm/Frame.h>
#include <Xm/ManagerP.h>

#ifdef __cplusplus
extern "C" {
#endif



/* Full class records */

typedef struct
{
   XtPointer extension;
} XmFrameClassPart;

typedef struct _XmFrameClassRec
{
   CoreClassPart       core_class;
   CompositeClassPart  composite_class;
   ConstraintClassPart constraint_class;
   XmManagerClassPart  manager_class;
   XmFrameClassPart    frame_class;
} XmFrameClassRec;

externalref XmFrameClassRec xmFrameClassRec;


/*  Frame instance records  */

typedef struct
{
   Dimension margin_width;
   Dimension margin_height;
   unsigned char shadow_type;
   Dimension old_width;
   Dimension old_height;
   Dimension old_shadow_thickness;
   Position old_shadow_x;
   Position old_shadow_y;
   Widget work_area;
   Widget title_area;
   Boolean processing_constraints;
} XmFramePart;

typedef struct _XmFrameRec
{
    CorePart	   core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;
    XmFramePart    frame;
} XmFrameRec;


/*  Frame constraint records  */

typedef struct _XmFrameConstraintPart
{
   /* "unused" is actually being used in the CheckSetChildType defaultproc ! */
   int unused;
   unsigned char child_type;
   unsigned char child_h_alignment;
   Dimension child_h_spacing;
   unsigned char child_v_alignment;
} XmFrameConstraintPart, * XmFrameConstraint;

typedef struct _XmFrameConstraintRec
{
   XmManagerConstraintPart manager;
   XmFrameConstraintPart   frame;
} XmFrameConstraintRec, * XmFrameConstraintPtr;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmFrameP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
