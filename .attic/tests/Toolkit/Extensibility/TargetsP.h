/* $XConsortium: TargetsP.h /main/4 1995/07/15 21:04:08 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
 * TargetsP.h: The widget private header file for the ExmTargets
 *                 uniform transfer model demonstration widget.
 *
 ******************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmTargetsP_h
#define _ExmTargetsP_h

/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Include appropriate files. */
#include "Targets.h" /* public header file for the ExmTargets widget */
#include <Exm/StringP.h> /* private header file for the ExmString widget */

/* Data structure to hold data transfer snapshots */
typedef struct _ExmTargetsSnapshotStruct
{
  Atom                                      distinguisher;
  XmString                                  value;
  struct _ExmTargetsSnapshotStruct   *prev;
  struct _ExmTargetsSnapshotStruct   *next;
} ExmTargetsSnapshotStruct;

/* No new inheritable methods. */

/* Define the widget class part. */
typedef struct _ExmTargetsClassPart
{
	XtPointer		extension;
} ExmTargetsClassPart;


/* Define the full class record. */
typedef struct _ExmTargetsClassRec
{
	CoreClassPart		core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
	ExmTargetsClassPart	targets_class;
} ExmTargetsClassRec;


/* Make the full class record externally accessible. */
externalref ExmTargetsClassRec exmTargetsClassRec;

/* Define the widget instance part. */
typedef struct _ExmTargetsPart
{
  /* Fields and resources for ExmTargets */
        int                                    snap_counter;
        ExmTargetsSnapshotStruct        *snapshots;
	XtCallbackList		               destination_callback;

  /* private state variables for selections */
	int			select_start_x;
	int			select_start_y;
	int                     select_end_x;
	int                     select_end_y;
	int			selected_item_count;
	Boolean			have_clip;
	Boolean			have_primary;
	Boolean			have_secondary;
} ExmTargetsPart;

/* Define the full instance record. */
typedef struct _ExmTargetsRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
	ExmTargetsPart	targets;
} ExmTargetsRec;

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* Ensure that the file be included only once. */
#endif /* _ExmTargetsP_h */
