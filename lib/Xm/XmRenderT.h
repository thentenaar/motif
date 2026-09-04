/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#ifndef _XM_RENDERT_H
#define _XM_RENDERT_H

#include <X11/Intrinsic.h>

/* Motif-branded minimal slant / weight constants */
#define XmSLANT_ROMAN   0
#define XmSLANT_ITALIC  100
#define XmSLANT_OBLIQUE 110

#define XmWEIGHT_REGULAR  80
#define XmWEIGHT_MEDIUM   100
#define XmWEIGHT_DEMIBOLD 180
#define XmWEIGHT_BOLD     200

/**
 * Color for rendering a rendition
 */
typedef struct XmRenditionColor {
	Pixel pixel;

	/* Reserved for future use */
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	unsigned short alpha;
} XmRenditionColor;

typedef struct _XmRenditionStyle {
	unsigned char underline;
	unsigned char strikethru;
	unsigned char fg_state;
	unsigned char bg_state;
	GC gc;
	XmRenditionColor fg;
	XmRenditionColor bg;
} *XmRenditionStyle;

/**
 * Create and initialize a XmRenditionStyle
 */
XmRenditionStyle XmRenditionStyleCreate(void);

/**
 * Duplicate a XmRenditionStyle
 */
XmRenditionStyle XmRenditionStyleDup(const XmRenditionStyle style);

/**
 * Cascade style properties from b to a.
 *
 * Returns True if modifications were made to a, False otherwise
 */
Boolean XmRenditionStyleMerge(XmRenditionStyle a, const XmRenditionStyle b);

/**
 * Free a XmRenditionStyle
 */
void XmRenditionStyleFree(XmRenditionStyle style);

#endif /* _XM_RENDERT_H */
