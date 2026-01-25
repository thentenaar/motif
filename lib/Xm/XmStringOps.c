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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <string.h>

#include "XmI.h"
#include "XmStringI.h"

/**
 * Determine the length of a UTF-8 character based on it's initial
 * four bits (0 for contiuation bytes).
 */
static const unsigned int utf8_len[16] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 2, 2, 3, 4
};

/**
 * Get the length of a UTF-8 string
 *
 * If \a byte_count is 0, we assume the string is NULL-terminated
 */
size_t _Xmstrlen(const unsigned char *s, size_t byte_count)
{
	size_t chars = 0, len;
	const unsigned char *end;

	if (!s || !*s)
		return 0;

	end = s + byte_count;
	while ((!byte_count || s < end) && *s) {
		/* Make sure we don't have an errant contiunation byte */
		if (!(len = utf8_len[(*s & 0xf0) >> 4]))
			break;

		s += len;
		++chars;
	}

	return chars;
}

/**
 * Advance \a n characters and return the offset from \a s in bytes
 */
static size_t advance(const unsigned char *s, size_t n)
{
	size_t len, bytes = 0;

	if (!s || !n)
		return 0;

	while (*s && n) {
		if (!(len = utf8_len[(*s & 0xf0) >> 4]))
			break;

		s += len;
		bytes += len;
		n--;
	}

	return bytes;
}

/**
 * Create a tag suitable for the given component type, based on the
 * latest tag in the context
 */
static XmString create_tag(const XmStringContext ctx, XmStringComponentType t)
{
	XmString out;

	out = XmStringComponentCreate(
			t == XmSTRING_COMPONENT_TEXT
			? XmSTRING_COMPONENT_TAG : XmSTRING_COMPONENT_LOCALE,
			strlen(_XmStrContTag(ctx)), _XmStrContTag(ctx));

	if (t  == XmSTRING_COMPONENT_WIDECHAR_TEXT) {
		if (_XmStrOptimized(out))
			_XmStrTextType(out) = XmWIDECHAR_TEXT;
		else _XmEntryTextTypeSet(_XmStrEntry(out)[0], XmWIDECHAR_TEXT);
	}

	return out;
}

/**
 * Return a set of RENDITION_ENDs for each rendition active in s
 * at the end of the string (or from the current context if supplied)
 */
static XmString terminate_renditions(const XmString s, XmStringContext ctx)
{
	short i;
	unsigned int len;
	XtPointer val;
	XmString tmp, out = NULL;
	XmStringContext c;
	XmStringComponentType t = XmSTRING_COMPONENT_UNKNOWN;

	/* Run the context over the string to accumulate the renditions */
	if (!ctx) {
		XmStringInitContext(&c, s);
		XmStringContextWantUtf8Text(ctx, True);
		while (XmeStringGetComponent(c, True, False, &len, &val) != XmSTRING_COMPONENT_END);
	}

	if (_XmStrContRendCount(ctx ? ctx : c)) {
		/* Terminate */
		for (i = _XmStrContRendIndex(ctx ? ctx : c); i >= 0; i--) {
			tmp = XmStringComponentCreate(
				XmSTRING_COMPONENT_RENDITION_END,
				strlen(_XmStrContRendTags(ctx ? ctx : c)[i]),
				_XmStrContRendTags(ctx ? ctx : c)[i]
			);
			out = XmStringConcatAndFree(out, tmp);
		}
	}

	if (!ctx) XmStringFreeContext(c);
	return out;
}

/**
 * Return a set of RENDITION_BEGINs for each currently active rendition
 * in the given context
 */
static XmString current_renditions(const XmStringContext ctx)
{
	short i;
	XmString tmp, out = NULL;

	if (!ctx)
		return NULL;

	for (i = 0; i < _XmStrContRendCount(ctx); i++) {
		tmp = XmStringComponentCreate(
			XmSTRING_COMPONENT_RENDITION_BEGIN,
			strlen(_XmStrContRendTags(ctx)[i]),
			_XmStrContRendTags(ctx)[i]
		);
		out = XmStringConcatAndFree(out, tmp);
	}

	return out;
}

/**
 * Separators and tabs count for one character each, other non-text
 * components (e.g., control characters) have no length.
 */
size_t XmStringLen(const XmString s)
{
	XtPointer val;
	unsigned int len;
	size_t c_len = 0;
	XmStringContext ctx;
	XmStringComponentType t = XmSTRING_COMPONENT_UNKNOWN;

	if (!XmStringIsValid(s) || XmStringEmpty(s))
		return 0;

	_XmProcessLock();
	XmStringInitContext(&ctx, s);
	XmStringContextWantUtf8Text(ctx, True);
	while (XmeStringGetComponent(ctx, True, False, &len, &val) != XmSTRING_COMPONENT_END);
	c_len = _XmStrContError(ctx) ? 0 : _XmStrContPos(ctx);
	XmStringFreeContext(ctx);
	_XmProcessUnlock();
	return c_len;
}

/**
 * Slice an XmString, yielding a substring
 */
XmString XmStringSubstr(const XmString s, XmTextPosition pos, size_t length)
{
	XtPointer val;
	unsigned int len;
	size_t mid_len;
	Boolean first = False;
	XmString out = NULL;
	XmStringContext ctx;
	XmStringComponentType t = XmSTRING_COMPONENT_UNKNOWN;

	if (XmStringEmpty(s) || !length)
		return NULL;

	_XmProcessLock();
	XmStringInitContext(&ctx, s);
	XmStringContextWantUtf8Text(ctx, True);

	while (t != XmSTRING_COMPONENT_END) {
		t = XmeStringGetComponent(ctx, True, False, &len, &val);
		if (_XmStrContNextPos(ctx) <= (size_t)pos)
			continue;
		if (_XmStrContPos(ctx) > (size_t)pos + length)
			break;

		if (!first) {
			/* Output currently active renditions on our first component */
			out = XmStringConcatAndFree(out, current_renditions(ctx));
			first = True;
		}

		if (t == XmSTRING_COMPONENT_TEXT        ||
		    t == XmSTRING_COMPONENT_LOCALE_TEXT ||
		    t == XmSTRING_COMPONENT_WIDECHAR_TEXT)
			out = XmStringConcatAndFree(out, create_tag(ctx, t));

		/* We're starting inside the component */
		if (_XmStrContPos(ctx) < (size_t)pos) {
			mid_len = advance(val, (size_t)pos - _XmStrContPos(ctx));
			val  = (XtPointer)((char *)val + mid_len);
			len -= mid_len;
		}

		/* The whole component fits */
		if (_XmStrContNextPos(ctx) <= (size_t)pos + length) {
			out = XmStringConcatAndFree(out, XmStringComponentCreate(t, len, val));
			continue;
		}

		/* Partial fit */
		if (_XmStrContPos(ctx) <= (size_t)pos)
			mid_len = advance(val, length);
		else mid_len = advance(val, (size_t)pos + length - _XmStrContPos(ctx));
		out = XmStringConcatAndFree(out, XmStringComponentCreate(t, mid_len, val));
	}

	XmStringFreeContext(ctx);
	_XmProcessUnlock();
	return out;
}

/**
 * Insert an XmString at the given position in \a s
 */
XmString XmStringInsert(const XmString s, XmTextPosition pos, const XmString addition)
{
	XtPointer val;
	unsigned int len;
	size_t mid_len;
	Boolean done = False;
	XmString out = NULL, tmp;
	XmStringContext ctx;
	XmStringComponentType t = XmSTRING_COMPONENT_UNKNOWN;

	pos = pos < 0 ? 0 : pos;
	if (XmStringEmpty(s))
		return XmStringCopy(addition);

	if (XmStringEmpty(addition))
		return XmStringCopy(s);

	_XmProcessLock();
	XmStringInitContext(&ctx, s);
	XmStringContextWantUtf8Text(ctx, True);

	while (t != XmSTRING_COMPONENT_END) {
		switch ((t = XmeStringGetComponent(ctx, True, False, &len, &val))) {
		/* These are handled by the context */
		case XmSTRING_COMPONENT_TAG:
		case XmSTRING_COMPONENT_LOCALE:
		case XmSTRING_COMPONENT_RENDITION_BEGIN:
		case XmSTRING_COMPONENT_RENDITION_END:
			break;
		case XmSTRING_COMPONENT_END:
			if (!done)
				out = XmStringConcat(s, addition);
			break;

		case XmSTRING_COMPONENT_LAYOUT_POP:
		case XmSTRING_COMPONENT_SEPARATOR:
		case XmSTRING_COMPONENT_TAB:
			/* ComponentCreate requires these be NULL/0 */
			val  = NULL;
			len  = 0;
			goto copy;

		case XmSTRING_COMPONENT_TEXT:
		case XmSTRING_COMPONENT_LOCALE_TEXT:
		case XmSTRING_COMPONENT_WIDECHAR_TEXT:
			if (done)
				goto copy;

			if (_XmStrContNextPos(ctx) <= (size_t)pos)
				goto tag;

			if ((size_t)pos == _XmStrContPos(ctx)) {
				/* Pre-component insertion */
				tmp = out;
				out = XmStringConcat(out, addition);
				out = XmStringConcatAndFree(out, terminate_renditions(addition, NULL));
				XmStringFree(tmp);
				done = True;
				goto tag;
			}

			/**
			 * Intra-component insertion
			 *
			 * Insert at the requested position, ensuring that all
			 * inserted renditions are terminated after the inserted
			 * string ends.
			 */
			mid_len = advance(val, (size_t)pos - _XmStrContPos(ctx));
			tmp     = XmStringComponentCreate(t, mid_len, val);
			out     = XmStringConcatAndFree(out, create_tag(ctx, t));
			out     = XmStringConcatAndFree(out, tmp);
			out     = XmStringConcat(out, addition);
			out     = XmStringConcatAndFree(out, terminate_renditions(addition, NULL));

			/* Restore the pre-insertion tag */
			if (_XmStrContTag(ctx))
				out = XmStringConcatAndFree(out, create_tag(ctx, t));

			/* Now, rejoin the remainder of the split component */
			tmp   = XmStringComponentCreate(t, len - mid_len, (char *)val + mid_len);
			out   = XmStringConcatAndFree(out, tmp);
			done  = True;
			break;

		tag:
			out = XmStringConcatAndFree(out, create_tag(ctx, t));

		default:
		copy:
			/* Everything else is passthrough */
			tmp = XmStringComponentCreate(t, len, val);
			out = XmStringConcatAndFree(out, tmp);
		}
	}

	XmStringFreeContext(ctx);
	_XmProcessUnlock();
	return out;
}

