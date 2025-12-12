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

#ifndef _SLIDECP_H
#define _SLIDECP_H

#include <Xm/SlideC.h>
#include <X11/IntrinsicP.h>
#include <X11/ObjectP.h>
#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XmUNSPECIFIED_POSITION
#define XmUNSPECIFIED_POSITION (-1)
#endif

typedef struct {
    XtPointer extension;
} XmSlideContextClassPart;

typedef struct _XmSlideContextClassRec {
	ObjectClassPart object_class;
	XmSlideContextClassPart slide_class;
} XmSlideContextClassRec;

extern XmSlideContextClassRec xmSlideContextClassRec;

typedef struct _XmSlideContextPart {
	XtIntervalId id;
	XtCallbackList slideFinishCallback;
	XtCallbackList slideMotionCallback;
	Widget slide_widget;
	unsigned long interval;
	Dimension dest_width;
	Dimension dest_height;
	Position dest_x;
	Position dest_y;
} XmSlideContextPart;

typedef struct _XmSlideContextRec {
	ObjectPart object;
	XmSlideContextPart slide;
} XmSlideContextRec;

#define Slide_Id(w) (((XmSlideContextWidget)w)->slide.id)
#define Slide_Widget(w) (((XmSlideContextWidget)w)->slide.slide_widget)
#define Slide_Interval(w) (((XmSlideContextWidget)w)->slide.interval)
#define Slide_DestWidth(w) (((XmSlideContextWidget)w)->slide.dest_width)
#define Slide_DestHeight(w) (((XmSlideContextWidget)w)->slide.dest_height)
#define Slide_DestX(w) (((XmSlideContextWidget)w)->slide.dest_x)
#define Slide_DestY(w) (((XmSlideContextWidget)w)->slide.dest_y)
#define Slide_FinishCallback(w) (((XmSlideContextWidget)w)->slide.slideFinishCallback)
#define Slide_MotionCallback(w) (((XmSlideContextWidget)w)->slide.slideMotionCallback)

#ifdef __cplusplus
}
#endif

#endif
