/* $XConsortium: ClipWindowP.h /main/5 1995/07/15 20:48:39 drk $ */
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
#ifndef _XmClipWindowP_h
#define _XmClipWindowP_h

#include <Xm/DrawingAP.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmClipWindowWidgetClass;

typedef struct _XmClipWindowClassRec * XmClipWindowWidgetClass;
typedef struct _XmClipWindowRec      * XmClipWindowWidget;


#ifndef XmIsClipWindow
#define XmIsClipWindow(w)  (XtIsSubclass (w, xmClipWindowWidgetClass))
#endif


/*  New fields for the ClipWindow widget class record  */

typedef struct
{
   XtPointer extension;   
} XmClipWindowClassPart;


/* Full class record declaration */

typedef struct _XmClipWindowClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	XmDrawingAreaClassPart	drawing_area_class;
	XmClipWindowClassPart	clip_window_class;
} XmClipWindowClassRec;

externalref XmClipWindowClassRec xmClipWindowClassRec;


/* New fields for the ClipWindow widget record */

typedef struct
{
	unsigned char flags;
	Dimension old_width ;
} XmClipWindowPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XmClipWindowRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	XmDrawingAreaPart	drawing_area;
	XmClipWindowPart	clip_window;
} XmClipWindowRec;


/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmClipWindowP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
