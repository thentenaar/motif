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
 *  SashP.h - Private definitions for Sash widget (Used by VPane Widget)
 *
 */

#ifndef _XmSashP_h
#define _XmSashP_h

#include <Xm/PrimitiveP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *
 * Sash Widget Private Data
 *
 *****************************************************************************/

/* New fields for the Sash widget class record */
typedef struct {
   XtPointer extension;   /* Pointer to extension record */
} XmSashClassPart;

/* Full Class record declaration */
typedef struct _XmSashClassRec {
    CoreClassPart         core_class;
    XmPrimitiveClassPart  primitive_class;
    XmSashClassPart    sash_class;
} XmSashClassRec;

typedef struct _XmSashClassRec *XmSashWidgetClass;

externalref XmSashClassRec xmSashClassRec;

/* New fields for the Sash widget record */
typedef struct {
  XtCallbackList sash_action;
  Boolean has_focus;
} XmSashPart;

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

typedef struct _XmSashRec {
   CorePart         core;
   XmPrimitivePart  primitive;
   XmSashPart       sash;
} XmSashRec;

typedef struct _XmSashRec      *XmSashWidget;

typedef struct {
  XEvent *event;		/* the event causing the SashAction */
  String *params;		/* the TranslationTable params */
  Cardinal num_params;		/* count of params */
} SashCallDataRec, *SashCallData;

/* Class Record Constant */

externalref WidgetClass xmSashWidgetClass;

#ifndef XmIsSash
#define XmIsSash(w)	XtIsSubclass(w, xmSashWidgetClass)
#endif /* XmIsSash */


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmSashP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
