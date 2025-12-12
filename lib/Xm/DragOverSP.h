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
#ifndef _XmDragOverSP_h
#define _XmDragOverSP_h

#include <X11/IntrinsicP.h>

#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <Xm/XmP.h>
#include <Xm/DragIconP.h>
#include <Xm/DragOverS.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOExpose(do) \
	((XtClass(do))->core_class.expose) ((Widget)(do), NULL, NULL)

/* 
 * DRAGOVER SHELL
 */
typedef struct 
{
    XtPointer				extension;
} XmDragOverShellClassPart;

/* Full class record declaration */

typedef struct _XmDragOverShellClassRec 
{
    CoreClassPart 		core_class;
    CompositeClassPart 		composite_class;
    ShellClassPart 		shell_class;
    WMShellClassPart	        wm_shell_class;
    VendorShellClassPart 	vendor_shell_class;
    XmDragOverShellClassPart 	dragOver_shell_class;
} XmDragOverShellClassRec;

externalref XmDragOverShellClassRec xmDragOverShellClassRec;

typedef struct _XmBackingRec{
    Position	x, y;
    Pixmap	pixmap;
}XmBackingRec, *XmBacking;

typedef struct _XmDragOverBlendRec{
    XmDragIconObject		sourceIcon;	/* source icon */
    Position			sourceX;	/* source location in blend */
    Position			sourceY;	/* source location in blend */
    XmDragIconObject		mixedIcon;	/* blended icon */
    GC				gc;		/* appropriate depth */
}XmDragOverBlendRec, *XmDragOverBlend;

typedef struct _XmDragOverShellPart{
    Position		hotX;		/* current hotX */
    Position		hotY;		/* current hotY */
    unsigned char	cursorState;	/* current cursor state */
    unsigned char	mode;
    unsigned char	activeMode;

    Position		initialX;	/* initial hotX */
    Position		initialY;	/* initial hotY */

    XmDragIconObject	stateIcon;	/* current state icon */
    XmDragIconObject	opIcon;		/* current operation icon */

    XmDragOverBlendRec	cursorBlend;	/* cursor blending */
    XmDragOverBlendRec	rootBlend;	/* pixmap or window blending */
    Pixel		cursorForeground;
    Pixel		cursorBackground;
    Cursor		ncCursor;	/* noncached cursor */
    Cursor		activeCursor;	/* the current cursor */

    XmBackingRec	backing; 	/* backing store for pixdrag */
    Pixmap		tmpPix;		/* temp storage for pixdrag */
    Pixmap		tmpBit;		/* temp storage for pixdrag */
    Boolean             isVisible;	/* shell is visible */

    /* Added for ShapedWindow dragging */
    /* Resources */
    Boolean		installColormap;/* Install the colormap */

    /* locals */
    Boolean		holePunched;	/* true if hole is punched */

    /* the following variables are used to make sure the correct colormap */
    /* is installed.  colormapWidget is initially the parent widget, but */
    /* can be changed by calling DragShellColormapWidget.		*/
    Widget		colormapWidget;	/* The widget I'm dragging from */
    Widget		colormapShell;	/* It's shell, install colormap here */
    Boolean		colormapOverride; /* shell is override rediirect */
    Colormap*		savedColormaps;	/* used with override redirect */
    int			numSavedColormaps;
}XmDragOverShellPart;

typedef  struct _XmDragOverShellRec{
    CorePart	 	core;
    CompositePart 	composite;
    ShellPart 		shell;
    WMShellPart		wm;
    VendorShellPart	vendor;
    XmDragOverShellPart	drag;
} XmDragOverShellRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDragOverSP_h */
