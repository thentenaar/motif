/* $XConsortium: StringTransP.h /main/4 1995/07/15 20:41:58 drk $ */
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
 * StringTransP.h: The widget private header file for the ExmStringTransfer 
 *                 uniform transfer model demonstration widget.
 *
 ******************************************************************************/

/* Ensure that the file be included only once. */
#ifndef _ExmStringTransP_h
#define _ExmStringTransP_h

/* Allow for C++ compilation. */
#ifdef __cplusplus
extern "C" {
#endif


/* Include appropriate files. */
#include <Exm/StringTrans.h> /* public header file for the ExmStringTransfer widget */
#include <Exm/ExmStringP.h> /* private header file for the ExmString widget */

/* No new inheritable methods. */

/* Define the widget class part. */
typedef struct _ExmStringTransferClassPart
{
	XtPointer		extension;
} ExmStringTransferClassPart;


/* Define the full class record. */
typedef struct _ExmStringTransferClassRec
{
	CoreClassPart		core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
	ExmStringTransferClassPart	string_transfer_class;
} ExmStringTransferClassRec;


/* Make the full class record externally accessible. */
externalref ExmStringTransferClassRec exmStringTransferClassRec;

/* Define the widget instance part. */
typedef struct _ExmStringTransferPart
{
  /* Fields and resources for ExmStringTransfer. */
	XtCallbackList		               destination_callback;
	Boolean				       own_primary;
} ExmStringTransferPart;

/* Define the full instance record. */
typedef struct _ExmStringTransferRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
	ExmStringTransferPart	string_transfer;
} ExmStringTransferRec, *ExmStringTransfer;


/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmStringTransferIndex (ExmStringIndex + 1)

/* Allow for C++ compilation. */
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* Ensure that the file be included only once. */
#endif /* _ExmStringTransP_h */
