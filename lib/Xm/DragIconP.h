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

#ifndef _XmDragIconP_h
#define _XmDragIconP_h

#include <Xm/VendorSEP.h>
#include <Xm/DragIcon.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*XmCloneVisualProc) (XmDragIconObject, Widget, Widget);
typedef void (*XmMovePixmapProc)  (XmDragIconObject,
				   XmDragIconObject,
				   XmDragIconObject,
				   Position, Position);

typedef struct {
  XtPointer		extension;
} XmDragIconClassPart;

typedef struct _XmDragIconClassRec{
  RectObjClassPart		rectangle_class;
  XmDragIconClassPart		dragIcon_class;
} XmDragIconClassRec;

typedef struct {
  Cardinal	depth;
  Pixmap	pixmap;
  Dimension	width, height;
  Pixmap	mask;
  Position	hot_x, hot_y;
  Position	offset_x, offset_y;
  unsigned char	attachment;
  Boolean	isDirty;
  Region        region;
  Region        restore_region;
  Position	x_offset, y_offset;
} XmDragIconPart, *XmDragIconPartPtr;

externalref XmDragIconClassRec 	xmDragIconClassRec;

typedef struct _XmDragIconRec{
  ObjectPart		object;
  RectObjPart		rectangle;
  XmDragIconPart	drag;
} XmDragIconRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmDragIconP_h */
