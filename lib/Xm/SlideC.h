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

#ifndef _SLIDEC_H
#define _SLIDEC_H

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XmIsSlideContext
#define XmIsSlideContext(w) XtIsSubclass(w,xmSlideContextClass)
#endif
#ifndef XmNslideFinishCallback
#define XmNslideFinishCallback "slideFinishCallback"
#endif
#ifndef XmCSlideFinishCallback
#define XmCSlideFinishCallback "SlideFinishCallback"
#endif
#ifndef XmNslideMotionCallback
#define XmNslideMotionCallback "slideMotionCallback"
#endif
#ifndef XmCSlideMotionCallback
#define XmCSlideMotionCallback "SlideMotionCallback"
#endif
#ifndef XmNslideWidget
#define XmNslideWidget "slideWidget"
#endif
#ifndef XmCSlideWidget
#define XmCSlideWidget "SlideWidget"
#endif
#ifndef XmNslideInterval
#define XmNslideInterval "slideInterval"
#endif
#ifndef XmCSlideInterval
#define XmCSlideInterval "SlideInterval"
#endif
#ifndef XmNslideDestWidth
#define XmNslideDestWidth "slideDestWidth"
#endif
#ifndef XmCSlideDestWidth
#define XmCSlideDestWidth "SlideDestWidth"
#endif
#ifndef XmNslideDestHeight
#define XmNslideDestHeight "slideDestHeight"
#endif
#ifndef XmCSlideDestHeight
#define XmCSlideDestHeight "SlideDestHeight"
#endif
#ifndef XmNslideDestX
#define XmNslideDestX "slideDestX"
#endif
#ifndef XmCSlideDestX
#define XmCSlideDestX "SlideDestX"
#endif
#ifndef XmNslideDestY
#define XmNslideDestY "slideDestY"
#endif
#ifndef XmCSlideDestY
#define XmCSlideDestY "SlideDestY"
#endif

extern WidgetClass xmSlideContextWidgetClass;

typedef struct _XmSlideContextRec *XmSlideContextWidget;
typedef struct _XmSlideContextClassRec *XmSlideContextWidgetClass;

typedef struct _XmSlideStruct {
	Widget w;
	XtWidgetGeometry dest;
	unsigned long interval;
	XtIntervalId id;
} XmSlideStruct, *XmSlidePtr;

void XmSlide(XmSlidePtr slide_info);

#ifdef __cplusplus
}
#endif
#endif
