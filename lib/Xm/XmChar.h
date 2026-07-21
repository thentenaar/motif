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
#ifndef XM_CHAR_H
#define XM_CHAR_H

#include <limits.h>
#include <X11/Intrinsic.h>

/* A non-character to represent invalid codepoints */
#define XM_INVALID_CODEPOINT 0xfffd

/* Maximum valid codepoint value */
#define XM_CODEPOINT_MAX 0x10ffff

/* A single UTF-8 encoded character */
typedef unsigned char *XmChar;

/* A single Unicode codepoint */
#if UINT_MAX >= (1 << 32) - 1
typedef unsigned int XmCodepoint;
#else
typedef unsigned long XmCodepoint;
#endif

/**
 * Get the length (in bytes) of the given XmChar
 */
unsigned int XmCharLen(const XmChar c);

/**
 * Decode a XmChar to a Unicode codepoint
 */
XmCodepoint XmCharToCodepoint(const XmChar c);

/**
 * Encode a Unicode codepoint to XmChar.
 *
 * Note: The resulting XmChar must be freed via XtFree().
 */
XmChar XmCodepointToChar(XmCodepoint cp);

#endif /* XM_CHAR_H */
