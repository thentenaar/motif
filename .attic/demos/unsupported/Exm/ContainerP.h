/* $XConsortium: ContainerP.h /main/4 1995/07/15 20:47:12 drk $ */
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

/****************************************************************************
 *
 * ContainerP.h: The widget private header file for the ExmContainer widget.
 *
 ****************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmContainerP_h
#define _ExmContainerP_h


/* Include appropriate files. */
#include <Exm/Container.h>  /* public header file for ExmContainer */ 
#include <Exm/GridP.h>      /* private header file for Grid */


/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Define the widget class part. */
typedef struct
{
	XtPointer		extension;
} ExmContainerClassPart;


/* Define the full class record. */
typedef struct _ExmContainerClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	ExmGridClassPart	grid_class;
	ExmContainerClassPart	container_class;
} ExmContainerClassRec;

externalref ExmContainerClassRec exmContainerClassRec;


/* Define the widget instance part. */
typedef struct
{
 /* Provide space for the values of the resources of ExmContainer. */
	Dimension		first_column_width;
	Cardinal *		detail_order;
	Cardinal 		detail_order_count;
	XmTabList               detail_tablist ;
	XmStringTable           detail_header ;
	Cardinal                detail_header_count;
	Pixel                   select_color;
	unsigned char           layout_type;

	Widget                  icon_header;

} ExmContainerPart;


/* Define the full instance record. */
typedef struct _ExmContainerRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	ExmGridPart	        grid;
	ExmContainerPart	container;
} ExmContainerRec;


/* Define the constraint part structure. */
typedef struct _ExmContainerConstraintPart
{
	char unused ;
} ExmContainerConstraintPart, * ExmContainerConstraint;


/* Define the full constraint structure. */
typedef struct _ExmContainerConstraintRec
{
	XmManagerConstraintPart	        manager;
	ExmGridConstraintPart	        grid;
	ExmContainerConstraintPart	container;
} ExmContainerConstraintRec, * ExmContainerConstraintPtr;


/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmContainerP_h */
