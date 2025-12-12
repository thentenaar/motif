/* $XConsortium: CommandBP.h /main/4 1995/07/15 20:40:31 drk $ */
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
 * CommandBP.h: The widget private header file for the ExmCommandButton 
 *              demonstration widget.
 *
 ******************************************************************************/

/* Ensure that this file be included only once. */
#ifndef _ExmCommandBP_h
#define _ExmCommandBP_h

/* Include appropriate files. */
#include <Exm/CommandB.h> /* public header file for ExmCommandButton */
#include <Exm/ExmStringP.h>  /* private header file for ExmSimple */

/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif

/* Don't define any inheritable methods. */


/* Define the widget class part. */ 
typedef struct _ExmCommandButtonClassPart
{
        XtPointer		extension;
} ExmCommandButtonClassPart;


/* Define the full class record. */
typedef struct _ExmCommandButtonClassRec
{
	CoreClassPart		core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
	ExmCommandButtonClassPart command_button_class;
} ExmCommandButtonClassRec;

/* Make the full class record exteranlly accessible. */
externalref ExmCommandButtonClassRec exmCommandButtonClassRec;

/* Define the widget instance part. */
typedef struct _ExmCommandButtonPart
{
	XtCallbackList		activate_callback;
	Boolean			show_as_default;

 /* armed is a flag.  If its value is True, then the user has selected the
    widget.  */
	Boolean			armed;

 /* visual_armed is a flag.  If its value is True, then the widget is to
    visually represent its armed state by drawing a certain kind of shadow. */ 
	Boolean			visual_armed;

 /* visual_timer holds an integral value representing the time (in milliseconds)
    between when armed is set to True and visual_armed is set to True. */ 
	XtIntervalId		visual_timer;
} ExmCommandButtonPart;

/* Define the full instance record. */
typedef struct _ExmCommandButtonRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
	ExmCommandButtonPart	command_button;
} ExmCommandButtonRec;

/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmCommandButtonIndex (ExmStringIndex + 1)

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* Ensure that the file be included only once. */
#endif /* _ExmCommandBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
