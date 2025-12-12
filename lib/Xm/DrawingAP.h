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
#ifndef _XmDrawingAreaP_h
#define _XmDrawingAreaP_h

#include <Xm/ManagerP.h>
#include <Xm/DrawingA.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XmRESIZE_SWINDOW	10


/* Constraint part record for DrawingArea widget */

typedef struct _XmDrawingAreaConstraintPart
{
   char unused;
} XmDrawingAreaConstraintPart, * XmDrawingAreaConstraint;

/*  New fields for the DrawingArea widget class record  */

typedef struct
{
   XtPointer extension;   /* Pointer to extension record */
} XmDrawingAreaClassPart;


/* Full class record declaration */

typedef struct _XmDrawingAreaClassRec
{
  CoreClassPart		core_class;
  CompositeClassPart	composite_class;
  ConstraintClassPart	constraint_class;
  XmManagerClassPart	manager_class;
  XmDrawingAreaClassPart	drawing_area_class;
} XmDrawingAreaClassRec;

externalref XmDrawingAreaClassRec xmDrawingAreaClassRec;


/* New fields for the DrawingArea widget record */

typedef struct
{
  Dimension		margin_width;
  Dimension		margin_height;

  XtCallbackList	resize_callback;
  XtCallbackList	expose_callback;
  XtCallbackList	input_callback;

  unsigned char		resize_policy;
  
#ifndef XM_PART_BC
  XtCallbackList	convert_callback;
  XtCallbackList	destination_callback;
#endif
} XmDrawingAreaPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XmDrawingAreaRec
{
  CorePart		core;
  CompositePart		composite;
  ConstraintPart	constraint;
  XmManagerPart		manager;
  XmDrawingAreaPart	drawing_area;
} XmDrawingAreaRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDrawingAreaP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
