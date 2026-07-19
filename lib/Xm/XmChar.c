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

#include "XmChar.h"
#include "XmCharI.h"

/**
 * Determine the length of a UTF-8 character based on it's initial
 * four bits (0 for contiuation bytes).
 */
static const unsigned int utf8_len[16] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 2, 2, 3, 4
};

/**
 * Get the length (in bytes) of the given XmChar
 */
unsigned int XmCharLen(const XmChar c)
{
	return utf8_len[(*c & 0xf0) >> 4];
}

/**
 * Decode a XmChar to a Unicode codepoint
 */
XmCodepoint XmCharToCodepoint(const XmChar c)
{
	unsigned int i, len;
	XmCodepoint cp = 0;

	if (!c || !*c)
		return 0;

	if ((len = XmCharLen(c)) == 1)
		return c[0];

	switch (len) {
	case 2: return ((c[0] & 0x1f) << 6) | (c[1] & 0x3f);
	case 3:
		cp = ((c[0] & 0x0f) << 12) | ((c[1] & 0x3f) << 6) | (c[2] & 0x3f);
		if (cp >= 0xd800 && cp <= 0xdfff)
			return XM_INVALID_CODEPOINT;
		return cp;
	default:
		/* Ensure we have the mandatory zero bit */
		if ((c[0] & 0xf8) != 0xf0)
			return XM_INVALID_CODEPOINT;
		return ((c[0] & 0x07) << 18) | ((c[1] & 0x1f) << 12) |
		        ((c[2] & 0x3f) << 6) | (c[3] & 0x3f);
	}
}

/**
 * Encode a Unicode codepoint to XmChar.
 *
 * Note: The resulting XmChar must be freed via XtFree().
 */
XmChar XmCodepointToChar(XmCodepoint cp)
{
	XmChar c;
	unsigned int len = 4;

	/* Use a non-character for out-of-range values / UTF-16 surrogates */
	if (cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff))
		cp = XM_INVALID_CODEPOINT;

	if (cp < 0x80)         len = 1;
	else if (cp < 0x800)   len = 2;
	else if (cp < 0x10000) len = 3;
	c = (XmChar)XtMalloc(len);

	switch (len) {
	case 1:
		c[0] = cp & 0xff;
		break;
	case 2:
		c[0] = 0xc0 | (cp >> 6);
		c[1] = 0x80 | (cp & 0x3f);
		break;
	case 3:
		c[0] = 0xe0 | (cp  >> 12);
		c[1] = 0x80 | ((cp >> 6) & 0x3f);
		c[2] = 0x80 | (cp & 0x3f);
		break;
	default:
		c[0] = 0xf0 | (cp  >> 18);
		c[1] = 0x80 | ((cp >> 12) & 0x3f);
		c[2] = 0x80 | ((cp >> 6)  & 0x3f);
		c[3] = 0x80 | (cp & 0x3f);
	}

	return c;
}

