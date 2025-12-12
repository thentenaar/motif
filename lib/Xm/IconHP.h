/* $XConsortium: IconHP.h /main/4 1995/07/15 20:52:25 drk $ */
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
#ifndef _XmIconHP_h
#define _XmIconHP_h

#include <Xm/XmP.h>
#include <Xm/IconGP.h>
#include <Xm/IconH.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IconHeader class record */
typedef struct _XmIconHeaderClassPart
	{
	XtPointer extension ;
	} 	XmIconHeaderClassPart;


/* Full class record declaration */
typedef struct _XmIconHeaderClassRec
	{
	RectObjClassPart	rect_class;
	XmGadgetClassPart	gadget_class;
	XmIconGadgetClassPart	icong_class;
	XmIconHeaderClassPart	iconh_class;
	} 	XmIconHeaderClassRec;

extern	XmIconHeaderClassRec 	xmIconHeaderClassRec;

/* IconHeader instance record */
typedef struct _XmIconHeaderPart
	{
	Widget container_ID;	                /* XmNcontainerID */
	} 	XmIconHeaderPart;

/* Full instance record declaration */
typedef struct _XmIconHeaderRec
	{
	ObjectPart	object;
	RectObjPart	rectangle;
	XmGadgetPart	gadget;
	XmIconGadgetPart icong;
	XmIconHeaderPart iconh;
	} 	XmIconHeaderRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmIconHP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
