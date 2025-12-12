/* $XConsortium: PannerP.h /main/5 1995/07/15 20:41:19 drk $ */
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
#ifndef _ExmPannerP_h
#define _ExmPannerP_h

#include <Exm/Panner.h>
#include <Exm/SimpleP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ExmPannerClassPart {	/* new fields in widget class */
    XtPointer extension;
} ExmPannerClassPart;

typedef struct _ExmPannerClassRec {	/* Panner widget class */
    CoreClassPart        core_class;
    XmPrimitiveClassPart primitive_class;
    ExmSimpleClassPart   simple_class;
    ExmPannerClassPart   panner_class;
} ExmPannerClassRec;

typedef struct {			/* new fields in widget */
    /* resources... */
    XtCallbackList report_callbacks;	/* callback/Callback */
    Dimension canvas_width;		/* canvasWidth/CanvasWidth */
    Dimension canvas_height;		/* canvasHeight/CanvasHeight */
    Position slider_x;			/* sliderX/SliderX */
    Position slider_y;			/* sliderY/SliderY */
    Dimension slider_width;		/* sliderWidth/SliderWidth */
    Dimension slider_height;		/* sliderHeight/SliderHeight */

    /* private data... */
    GC xor_gc;				/* for doing XOR tmp graphics */
    double haspect, vaspect;		/* aspect ratio of core to canvas */
    Boolean rubber_band;		/* true = rubber band, false = move */
    struct {
	Boolean doing;			/* tmp graphics in progress */
	Boolean showing;		/* true if tmp graphics displayed */
	Position startx, starty;	/* initial position of slider */
	Position dx, dy;		/* offset loc for tmp graphics */
	Position x, y;			/* location for tmp graphics */
    } tmp;
    Position knob_x, knob_y;		/* real upper left of knob in canvas */
    Dimension knob_width, knob_height;	/* real size of knob in canvas */
    Position last_x, last_y;		/* previous location of knob */

    Mask dimMask ;                      /* navigator mask */
} ExmPannerPart;

typedef struct _ExmPannerRec {
    CorePart core;
    XmPrimitivePart primitive;
    ExmSimplePart simple;
    ExmPannerPart panner;
} ExmPannerRec;

/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmPannerIndex (ExmSimpleIndex + 1)

/*
 * external declarations
 */
externalref ExmPannerClassRec exmPannerClassRec;

/********    Private Function Declarations    ********/

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _ExmPannerP_h */
