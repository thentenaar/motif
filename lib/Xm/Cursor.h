/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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

#ifndef _XmCursor_h
#define _XmCursor_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load a cursor given a name, which can be a cursor name (e.g., "arrow")
 * or a Xcursor file (if Xcursor is supported.)
 */
extern Cursor XmeLoadCursor(Display *display, Screen *screen, const char *name);

/**
 * Load a cursor given a filename (i.e., file.svg -- or simply file)
 * which will be loaded as a cursor via Xrender if such file exists
 * and we have an ARGB visual. The cursor will be scaled in the same
 * manner as Xcursor cursors. SVG and PNG images are supported.
 *
 * The hot_x and hot_y parameters indicate the hot coordinate of the
 * cursor in its original size.
 *
 * Returns None if the image cannot be found or loaded, or we don't have
 * a 32-bit TrueColor visual.
 *
 * This can be used in a XmRCursor resource by specifying "name:hot_x,hot_y"
 * as the string to convert from.
 */
extern Cursor XmeLoadCursorImage(Display *display, Screen *screen, const char *name,
                                 int hot_x, int hot_y);

#ifdef __cplusplus
}
#endif
#endif /* _XmCursor_h */

