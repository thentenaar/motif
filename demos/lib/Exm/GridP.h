/* $XConsortium: GridP.h /main/5 1995/07/15 20:40:53 drk $ */
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

/*******************************************************************************
 *
 * GridP.h: The widget private header file for the ExmGrid widget.
 *
 ******************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmGridP_h
#define _ExmGridP_h


/* Include appropriate files. */
#include <Exm/Grid.h>  /* public header file for ExmGrid */ 
#include <Xm/ManagerP.h> /* private header file for XmManager */


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Make the following two methods inheritable by subclasses of ExmGrid. */ 
#define ExmInheritLayout   ((ExmLayoutProc) _XtInherit)
#define ExmInheritCalcSize ((ExmCalcSizeProc) _XtInherit)
#define ExmInheritNeedRelayout ((ExmNeedRelayoutProc) _XtInherit)


/* Define new data types for these two inheritable methods. */
typedef void (*ExmLayoutProc)(
			Widget,
			Widget);
typedef void (*ExmCalcSizeProc)(
                        Widget,
                        Widget,
                        Dimension *,
                        Dimension *);
typedef Boolean (*ExmNeedRelayoutProc)(
                        Widget,
                        Widget);

/* Define the widget class part. */
typedef struct
{
	ExmLayoutProc           layout;
        ExmCalcSizeProc         calc_size;
        ExmNeedRelayoutProc     need_relayout;
	XtPointer		extension;
} ExmGridClassPart;


/* Define the full class record. */
typedef struct _ExmGridClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	ExmGridClassPart	grid_class;
} ExmGridClassRec;

externalref ExmGridClassRec exmGridClassRec;


/* Define the widget instance part. */
typedef struct
{
 /* Provide space for the values of the four resources of ExmGrid. */
	short			rows;
	short			columns;
	Dimension		margin_width;
	Dimension		margin_height;
	XtCallbackList	        map_callback;
	XtCallbackList	        unmap_callback;
	Boolean		        default_position;
	XmRenderTable           button_render_table ;
	XmRenderTable           label_render_table ;
	XmRenderTable           text_render_table ;
        XmString	        dialog_title;

 /* processing_constraints is a flag.  If its value is True, then 
    it means that the ConstraintSetValues method is requesting a 
    geometry change. */
	Boolean			processing_constraints;

} ExmGridPart;

/* Establish an arbitrary limit */
#define EXM_GRID_MAX_NUMBER_OF_ROWS 100
#define EXM_GRID_MAX_NUMBER_OF_COLUMNS 100

/* Define the full instance record. */
typedef struct _ExmGridRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	ExmGridPart		grid;
} ExmGridRec;

/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmGridIndex (XmManagerIndex + 1)

/* Define the constraint part structure. */
typedef struct _ExmGridConstraintPart
{
	Dimension		grid_margin_width_within_cell;
	Dimension		grid_margin_height_within_cell;
} ExmGridConstraintPart, * ExmGridConstraint;


/* Define the full constraint structure. */
typedef struct _ExmGridConstraintRec
{
	XmManagerConstraintPart	manager;
	ExmGridConstraintPart	grid;
} ExmGridConstraintRec, * ExmGridConstraintPtr;


/* Define macros for this class. */
#define ExmGridCPart(w) \
        (&((ExmGridConstraintPtr) (w)->core.constraints)->grid)


/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmGridP_h */
