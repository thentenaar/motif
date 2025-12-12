/* $XConsortium: Panner.h /main/5 1995/07/15 20:41:15 drk $ */
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
/* Ensure that this file be included only once. */
#ifndef _ExmPanner_h
#define _ExmPanner_h

/* Include appropriate header file(s). */
#include <Exm/Simple.h>  /* public header file for the ExmSimple widget */


/* Allow for compilation by C++. */
#ifdef __cplusplus
extern "C" {
#endif

#define ExmNcanvasWidth "canvasWidth"
#define ExmCCanvasWidth "CanvasWidth"
#define ExmNcanvasHeight "canvasHeight"
#define ExmCCanvasHeight "CanvasHeight"
#define ExmNrubberBand "rubberBand"
#define ExmCRubberBand "RubberBand"
#define ExmNsliderX "sliderX"
#define ExmCSliderX "SliderX"
#define ExmNsliderY "sliderY"
#define ExmCSliderY "SliderY"
#define ExmNsliderWidth "sliderWidth"
#define ExmCSliderWidth "SliderWidth"
#define ExmNsliderHeight "sliderHeight"
#define ExmCSliderHeight "SliderHeight"
#define ExmNreportCallback "reportCallback"
#define ExmCReportCallback "reportCallback"


/*
 * ExmPannerReport - this structure is used by the reportCallback of the
 * Panner, Porthole, Viewport, and Scrollbar widgets to report its position.
 * All fields must be filled in, although the changed field may be used as
 * a hint as to which fields have been altered since the last report.
 */
typedef struct {
    int     reason;
    XEvent  *event;
    Mask changed;		/* mask, see below */
    Position slider_x, slider_y;	/* location of slider within outer */
    Dimension slider_width, slider_height;  /* size of slider */
    Dimension canvas_width, canvas_height;  /* size of canvas */
} ExmPannerCallbackStruct;

#define ExmPRSliderX		(1 << 0)
#define ExmPRSliderY		(1 << 1)
#define ExmPRSliderWidth	(1 << 2)
#define ExmPRSliderHeight	(1 << 3)
#define ExmPRCanvasWidth	(1 << 4)
#define ExmPRCanvasHeight	(1 << 5)

/* external declarations */
externalref WidgetClass exmPannerWidgetClass;

typedef struct _ExmPannerClassRec *ExmPannerWidgetClass;
typedef struct _ExmPannerRec      *ExmPannerWidget;


/* Define an IsSubclass macro. */
#ifndef ExmIsPanner
#define ExmIsPanner(w)	XtIsSubclass(w, exmPannerWidgetClass)
#endif 

/********    Public Function Declarations    ********/

extern Widget ExmCreatePanner( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern void ExmPannerSetValues( 
                        Widget w,
                        int hvalue,
                        int vvalue,
#if NeedWidePrototypes
                        int notify) ;
#else
                        Boolean notify) ;
#endif /* NeedWidePrototypes */

/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _ExmPanner_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
