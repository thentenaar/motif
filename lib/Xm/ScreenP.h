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
#ifndef _XmScreenP_h
#define _XmScreenP_h

#include <Xm/DesktopP.h>
#include <Xm/Screen.h>
#include <Xm/DragIcon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmScreenClassPart {
	XtPointer extension;
} XmScreenClassPart, *XmScreenClassPartPtr;

typedef struct _XmScreenClassRec {
	CoreClassPart      core_class;
	XmDesktopClassPart desktop_class;
	XmScreenClassPart  screen_class;
} XmScreenClassRec;

typedef struct _XmDragCursorRec {
	struct _XmDragCursorRec *next;
	Cursor cursor;
	XmDragIconObject stateIcon;
	XmDragIconObject opIcon;
	XmDragIconObject sourceIcon;
	Boolean dirty;
} XmDragCursorRec, *XmDragCursorCache;

typedef struct _XmScratchPixmapKeyRec *XmScratchPixmapKey;

typedef struct _XmScratchPixmapKeyRec {
	Cardinal  depth;
	Dimension width;
	Dimension height;
} XmScratchPixmapKeyRec;

typedef struct {
	Boolean mwmPresent;
	unsigned short numReparented;
	int darkThreshold;
	int foregroundThreshold;
	int lightThreshold;
	XmDragIconObject defaultNoneCursorIcon;
	XmDragIconObject defaultValidCursorIcon;
	XmDragIconObject defaultInvalidCursorIcon;
	XmDragIconObject defaultMoveCursorIcon;
	XmDragIconObject defaultCopyCursorIcon;
	XmDragIconObject defaultLinkCursorIcon;
	XmDragIconObject defaultSourceCursorIcon;

	Cursor nullCursor;
	XmDragCursorRec *cursorCache;
	Cardinal maxCursorWidth;
	Cardinal maxCursorHeight;

	Cursor menuCursor;
	unsigned char unpostBehavior;
	XFontStruct *font_struct;
	int h_unit;
	int v_unit;
	XtPointer scratchPixmaps;
	unsigned char moveOpaque;
	XmScreenColorProc color_calc_proc;
	XmAllocColorProc  color_alloc_proc;
	XtEnum bitmap_conversion_model;

	/* to save internally-created XmDragIcons */
	XmDragIconObject xmStateCursorIcon;
	XmDragIconObject xmMoveCursorIcon;
	XmDragIconObject xmCopyCursorIcon;
	XmDragIconObject xmLinkCursorIcon;
	XmDragIconObject xmSourceCursorIcon;

	/* These four fields are obsolete */
	GC imageGC;
	int imageGCDepth;
	Pixel imageForeground;
	Pixel imageBackground;

	XtPointer screenInfo; /* extension */
	XtPointer user_data;
	Pixmap insensitive_stipple_bitmap;
	XtPointer inUsePixmaps;
} XmScreenPart, *XmScreenPartPtr;

typedef struct _XmScreenInfo {
	/* so much for information hiding */
	XtPointer menu_state;         /* MenuUtil.c */
	Boolean destroyCallbackAdded; /* ImageCache.c */
} XmScreenInfo;

externalref XmScreenClassRec 	xmScreenClassRec;

typedef struct _XmScreenRec {
	CorePart      core;
	XmDesktopPart desktop;
	XmScreenPart  screen;
} XmScreenRec;

#ifdef __cplusplus
}
#endif
#endif /* _XmScreenP_h */
