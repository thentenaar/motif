/* $XConsortium: TabBP.h /main/4 1995/07/15 20:42:11 drk $ */
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
 * TabBP.h: The widget private header file for the ExmTabButton demonstration
 *          widget.
 *
 ******************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmTabBP_h
#define _ExmTabBP_h

/* Include appropriate files. */
#include <Exm/TabB.h> /* public header file for ExmTabButton */
#include <Exm/CommandBP.h> /* private header file for ExmCommandButton */

/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif

/* This widget does not provide any inheritable methods. */


/* Define the widget class part. */
typedef struct _ExmTabButtonClassPart
{
        XtPointer		extension;
} ExmTabButtonClassPart;


/* Define the full class record. */
typedef struct _ExmTabButtonClassRec
{
	CoreClassPart		core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
	ExmCommandButtonClassPart command_button_class;
	ExmTabButtonClassPart   tab_button_class;
} ExmTabButtonClassRec;

/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmTabButtonIndex (ExmCommandButtonIndex + 1)

/* Make the full class record externally accessible. */
externalref ExmTabButtonClassRec exmTabButtonClassRec;


/* Define the widget instance part. */
typedef struct _ExmTabButtonPart
{
 /* Provide space for the one resource of ExmTabButton. */
	XtEnum  open_side;
 /* Provide space for the size of the join shadow_thickness */
	Dimension join_shadow_thickness;
} ExmTabButtonPart;


/* Define the full instance record. */
typedef struct _ExmTabButtonRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
	ExmCommandButtonPart	command_button;
	ExmTabButtonPart	tab_button;
} ExmTabButtonRec;

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


/* Ensure that the file be included only once. */
#endif /* _ExmTabBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
