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
#ifndef _XmScreen_h
#define _XmScreen_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The tranditional default for DPI
 */
#define SCREEN_DEFAULT_DPI 96.0

#ifndef XmIsScreen
#define XmIsScreen(w) XtIsSubclass((w), xmScreenClass)
#endif

/**
 * Convenience macros akin to XtScreen() / XtScreenOfObject() which wrap
 * XmGetXmScreen().
 */
#define XmScreenOfScreen(s) ((XmScreen)XmGetXmScreen((s)))
#define XmScreenOfObject(w) XmScreenOfScreen(XtScreenOfObject((Widget)(w)))

/**
 * Dimension and position information for monitors associated with
 * a XmScreen
 */
typedef struct _XmMonitorInfo {
	Position x;
	Position y;
	Dimension width;
	Dimension height;
	Dimension width_mm;
	Dimension height_mm;
	float dpi;
	const char *name;
} XmMonitorInfo;

/* Class record constants */
typedef struct _XmScreenRec *XmScreen;
typedef struct _XmScreenClassRec *XmScreenClass;
externalref WidgetClass xmScreenClass;

/********    Public Function Declarations    ********/
typedef void (*XmScreenColorProc)(Screen *screen, XColor *bg_color,
                                  XColor *fg_color, XColor *sel_color,
                                  XColor *ts_color, XColor *bs_color);
typedef Status (*XmAllocColorProc)(Display *display, Colormap colormap,
                                   XColor *screen_in_out);
extern Widget XmGetXmScreen(Screen *screen);
extern XmMonitorInfo *XmGetMonitorInfoAt(XmScreen screen, Position x,
                                         Position y);
extern void FreeXmMonitorInfo(XmMonitorInfo *info);
/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}
#endif
#endif /* _XmScreen_h */

