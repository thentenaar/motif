/* $XConsortium: StringP.h /main/4 1995/07/15 20:41:45 drk $ */
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
 * StringP.h: The widget private header file for the ExmString demonstration
 *            widget.
 *
 ******************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmStringP_h
#define _ExmStringP_h

/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Include appropriate files. */
#include <Exm/ExmString.h>  /* public header file for the ExmString widget */
#include <Exm/SimpleP.h> /* private header file for the ExmSimple widget */

/* No new inheritable methods. */

/* Define the widget class part. */
typedef struct _ExmStringClassPart
{
	unsigned char 		default_render_table_type;
	XtPointer		extension;
} ExmStringClassPart;


/* Define the full class record. */
typedef struct _ExmStringClassRec
{
	CoreClassPart		core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
} ExmStringClassRec;


/* Make the full class record externally accessible. */
externalref ExmStringClassRec exmStringClassRec;

/* Define a new enumerated type. */
typedef enum { ExmCENTER_STRING, ExmSTART_STRING_LEFT_SIDE,
                     ExmSTART_STRING_RIGHT_SIDE} ExmHowAligned;


/* Define the widget instance part. */
typedef struct _ExmStringPart
{
 /* Provide space for the five resources of ExmString. */
	XmString		compound_string;
	unsigned char		alignment;
        XmRenderTable           render_table;
        Boolean                 recompute_size;
        ExmHowAligned           text_starts_here; 
} ExmStringPart;

/* Define the full instance record. */
typedef struct _ExmStringRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
} ExmStringRec;

/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmStringIndex (ExmSimpleIndex + 1)

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* Ensure that the file be included only once. */
#endif /* _ExmStringP_h */
