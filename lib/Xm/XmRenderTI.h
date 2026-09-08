/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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
/*   $XConsortium: XmRenderTI.h /main/5 1995/07/13 18:24:12 drk $ */
#ifndef _XmRenderTI_h
#define _XmRenderTI_h

#include <Xm/XmP.h>
#include <Xm/XmRenderT.h>
#if USE_XFT
#include <X11/Xft/Xft.h>
#endif

#include "HashI.h"

/* Internal types for XmRenderTable.c */

/*
 * Macros for Rendition data structure access
 */

#define _XmRendLoadModel(r)	((_XmRendition)*(r))->loadModel
#define _XmRendTag(r)		((_XmRendition)*(r))->tag
#define _XmRendFontName(r)	((_XmRendition)*(r))->pattern
#define _XmRendFontType(r)	((_XmRendition)*(r))->fontType
#define _XmRendFont(r)		((_XmRendition)*(r))->font
#define _XmRendDisplay(r)	((_XmRendition)*(r))->display
#define _XmRendTabs(r)		((_XmRendition)*(r))->tabs
#define _XmRendFontStyle(r)     ((_XmRendition)*(r))->fontStyle
#define _XmRendFontFoundry(r)   ((_XmRendition)*(r))->fontFoundry
#define _XmRendFontSize(r)      ((_XmRendition)*(r))->fontSize
#define _XmRendPixelSize(r)     ((_XmRendition)*(r))->pixelSize
#define _XmRendFontSlant(r)     ((_XmRendition)*(r))->fontSlant
#define _XmRendFontSpacing(r)   ((_XmRendition)*(r))->fontSpacing
#define _XmRendFontWeight(r)    ((_XmRendition)*(r))->fontWeight
#if USE_XFT
#define _XmRendXftFont(r)       ((_XmRendition)*(r))->xftFont
#else
#define _XmRendXftFont(r)       (NULL)
#endif

typedef struct __XmRenditionRec
{
	XmLoadModel loadModel;
	XmStringTag tag;
	XmFontType fontType;
	XtPointer font;
	Display *display;
	XmTabList tabs;

	String pattern; /**< Pattern string used to load the font */
	String fontFoundry;
	String fontFamily;
	String fontStyle;
	int fontSize;
	int pixelSize;
	int fontSlant;
	int fontWeight;
	int fontSpacing;
	int ascent;
	int descent;
	int width;
	int ink_width;
	struct _XmRenditionStyle style;

#if USE_XFT
	XftFont *xftFont;
#else
	XtPointer xftFont;
#endif
} _XmRenditionRec, *_XmRendition;

/* Accessor macros. */

#define _XmRTCount(rt)		((_XmRenderTable)*(rt))->count
#define _XmRTRenditions(rt)	((_XmRenderTable)*(rt))->renditions
#define _XmRTDisplay(rt)	((_XmRenderTable)*(rt))->display

typedef struct __XmRenderTableRec
{
	Cardinal count;
	Display *display;
	XmRendition *renditions;
	XmHashTable ht;
} _XmRenderTableRec, *_XmRenderTable;

/********    Private Function Declarations for XmRenderTable.c    ********/

/* Used by ResConvert */
XmRendition _XmRenditionCreate(Display *display, Widget widget, String resname,
                               String resclass, XmStringTag tag, ArgList args,
                               Cardinal count, Boolean *in_db);

/* Used by Mrm / wml */
Widget _XmCreateRenderTable(Widget parent, String name, ArgList args, Cardinal count);
Widget _XmCreateRendition(Widget parent, String name, ArgList args, Cardinal count);

/* Used by Mrm */
extern Display *_XmRenderTableDisplay(XmRenderTable table);

extern XmRendition _XmRenditionMerge(Display *d,
				     XmRendition *scr,
				     XmRendition base_rend,
				     XmRenderTable rt,
				     XmStringTag base_tag,
				     XmStringTag *tags,
				     unsigned short tag_count,
                     Boolean copy
				     );
extern Boolean _XmRenderTableFindFallback(XmRenderTable ,
					  XmStringTag tag,
					  Boolean cached_tag,
					  XmRendition *rend_ptr) ;
extern Boolean _XmRenderTableFindFirstFont(XmRenderTable rendertable,
					   XmRendition *rend_ptr);

#if USE_XFT
/*
 * XftDraw cache functions
 */
XftDraw * _XmXftDrawCreate(Display *display, Window window);

void _XmXftDrawDestroy(Display *display, Window window, XftDraw *d);

void _XmXftDrawString(Display *display, Window window, XmRendition rend,
                      XmRenditionStyle style, int bpc, Position x,
                      Position y, char *s, int len, Boolean image);

void _XmXftDrawString2(Display *display, Window window, GC gc, XftFont *font, int bpc,
                Position x, Position y,
                char *s, int len);

void _XmXftSetClipRectangles(Display *display, Window window, Position x, Position y, XRectangle *rects, int n);

XftColor _XmXftGetXftColor(Display *display, Pixel color);

void _XmXftFontAverageWidth(Widget w, XtPointer f, int *width);
#endif

/********    End Private Function Declarations    ********/

#endif /* _XmRenderTI_h */

