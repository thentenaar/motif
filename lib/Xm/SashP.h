/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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

#ifndef _XmSashP_h
#define _XmSashP_h

#include <Xm/PrimitiveP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	XtPointer extension;
} XmSashClassPart;

typedef struct _XmSashClassRec {
	CoreClassPart        core_class;
	XmPrimitiveClassPart primitive_class;
	XmSashClassPart      sash_class;
} XmSashClassRec;

externalref XmSashClassRec xmSashClassRec;

typedef struct {
  XtCallbackList sash_action;
  Boolean        has_focus;
  Cursor         cursor;
} XmSashPart;

typedef struct _XmSashRec {
	CorePart        core;
	XmPrimitivePart primitive;
	XmSashPart      sash;
} XmSashRec;

typedef struct _XmSashRec *XmSashWidget;

struct XmSashCallbackData {
	XEvent *event;       /* the event causing the SashAction */
	String *params;      /* the TranslationTable params      */
	Cardinal num_params; /* count of params                  */
};

externalref WidgetClass xmSashWidgetClass;

#ifndef XmIsSash
#define XmIsSash(w)	XtIsSubclass(w, xmSashWidgetClass)
#endif /* XmIsSash */

#ifdef __cplusplus
}
#endif

#endif /* _XmSashP_h */

