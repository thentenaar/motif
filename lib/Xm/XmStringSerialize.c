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
 *	The ASN.1 version of XmString is:
 *
 *	COMPOUND_STRING		4 or 3 + n bytes (see description below)
 *
 *	    component tag		1 byte
 *	    length			1 to m bytes
 *	    value			n bytes
 *
 *	    component tag		1 byte
 *	    length			1 to m bytes
 *	    value			n bytes
 *
 * ASN.1 header for compound string - 3 byte header, followed by length
 * which is at least one byte, and at most n bytes.
 *
 * The first byte defines the ASN.1 space:  (0xdf)
 *              1 1      0      1 1 1 1 1
 *              class    form   ID code
 *
 *    class is private, form is primitive (not constructed from other
 *    forms), and the ID code value means the actual ID code value is
 *    extended into one or more octets.
 *
 * The second and third bytes define the actual ID code value.  The
 * value used for 1.2 is the inverse of the original XUI value.
 *     second byte:  (0x80)
 *               1       0000000
 *              MSB      high seven bits of ID code
 *
 *     third byte:   (0x06 [version 1])
 *               0       0000110
 *              LSB      low seven bits of ID code
 *
 * The low seven bits of the ID code are used further used to indicate
 * the version of bytestream seralization used. The first version (0x06)
 * is used up to and including Motif 2.4.0. Version 2 was introduced
 * in Motif 2.5.0, signifying that all text components are UTF-8
 * encoded within the stream, and should be portable across locales.
 *
 * The length field of the ASN.1 conformant compound string header
 * is dynamically constructed.  There are two possible forms depending
 * upon the length of the string.  Note that this length excludes the
 * header bytes.
 *
 *    Short Form: range 0 .. 127
 *    one byte
 *                  0         nnnnnnn
 *                 short       7 bit length
 *
 *    Long Form: range 128 .. 2**n-1
 *    three bytes
 *    first:        1         nnnnnnn
 *                 long       number of bytes to follow
 *
 *    second:
 *                  nnnnnnnn
 *                  MSB of length
 *
 *    third:
 *                  nnnnnnnn
 *                  LSB of length
 *
 * This process for constructing the length field will also be
 * used to construct the length field within individual tag-length-value
 * triplets.
 */

/**
 * Bytestream version marker
 */
#define ASN_VERSION(X) ((X) + 5)

/**
 * Maximum length value: 2 ^ 32 - 1, for compatibility with a
 * 32-bit unsigned int.
 */
#define ASN_MAXLEN       ((size_t)0xffffffff)

/**
 * Set \a b to the number of bytes needed to hold the length
 * \a x.
 */
#define ASN_LENGTH_BYTES(b, x) do { ++(b); } while ((x) >>= 8)

/**
 * Write the BER length field, allowing a flexible number of bytes
 * for length. Note that the bytes are in big-endian order.
 *
 * Returns a pointer to the byte following the newly-written length.
 */
static unsigned char *write_length(unsigned char *p, size_t len)
{
	size_t i, x = len, bytes = 0;

	if (len <= 0x7f) {
		*p++ = (unsigned char)len;
		return p;
	}

	ASN_LENGTH_BYTES(bytes, x);
	*p++ = 0x80 | bytes;

	for (i = bytes - 1; i <= bytes; i--) {
		p[i] = len & 0xff;
		len >>= 8;
	}

	return p + bytes;
}

/**
 * Write out the BER header
 */
static unsigned char *write_header(unsigned char *p, size_t len)
{
	if (!p)
		return NULL;

	*p++ = 0xdf;
	*p++ = 0x80;
	*p++ = ASN_VERSION(2);
	return write_length(p, len);
}

/**
 * Serialize a component in TLV format, and return a pointer to the
 * byte following the newly-written bytes.
 */
static unsigned char *write_component(unsigned char *p, XmStringComponentType type,
                                      size_t len, const unsigned char *value)
{
	if (len > ASN_MAXLEN) {
		XmeWarning(NULL, "write_component: Truncating overlong length");
		len = ASN_MAXLEN;
	}

	*p++ = (unsigned char)type;
	p    = write_length(p, len);
	memcpy(p, value, len);
	return p + len;
}

/**
 * Check and skip the header, optionally returning the stream version.
 *
 * Returns the length of the header on success, zero on failure.
 */
static int check_header(const unsigned char **stream, unsigned char *version)
{
	unsigned char v;
	const unsigned char *s;

	if (!stream || !(s = *stream) || *s++ != 0xdf || *s++ != 0x80)
		return 0;

	v = *s++ - ASN_VERSION(0);
	if (version)
		*version = v;
	*stream = s;
	return 3;
}

/**
 * Read the length of the BER stream, returning 0 if the
 * stream is empty or invalid; assuming that *stream points
 * to the first length byte.
 *
 * The stream pointer is moved to point to the first byte
 * after the length.
 */
static size_t read_length(const unsigned char **stream)
{
	size_t i, sz, len = 0;
	const unsigned char *s;

	if (!stream || !(s = *stream))
		return 0;

	sz = *s++;
	if (!(sz & 0x80)) {
		len = sz & 0x7f;
		goto done;
	}

	sz &= 0x7f;
	do {
		len = (len << 8) | *s++;
	} while (--sz);

done:
	*stream = s;
	return len;
}

/**
 * Reconstitute a component from the bytestream
 */
static XmString read_component(const unsigned char **stream,
                               const unsigned char *end,
                               unsigned char version)
{
	size_t len;
	XmString str = NULL;
	XmTextType text_type;
	XmStringComponentType type;
	const unsigned char *s, *x;
	unsigned char *data;

	if (!stream || !(s = *stream))
		return NULL;

	type = (XmStringComponentType)*s++;
	len  = read_length(&s);

	if (len > ASN_MAXLEN || s + len > end) {
		*stream = end;
		return NULL;
	}

	/* For version 1, we need to convert text components to UTF-8 */
	if (version == 1) {
		switch (type) {
		case XmSTRING_COMPONENT_TEXT:          text_type = XmCHARSET_TEXT;   break;
		case XmSTRING_COMPONENT_LOCALE_TEXT:   text_type = XmMULTIBYTE_TEXT; break;
		case XmSTRING_COMPONENT_WIDECHAR_TEXT: text_type = XmWIDECHAR_TEXT;  break;
		default: text_type = XmUTF8_TEXT;
		}

		if (text_type != XmUTF8_TEXT) {
			data = (unsigned char *)XtCalloc(1, len + sizeof(wchar_t));
			memcpy(data, s, len);
			str = XmStringParseText(data, NULL, NULL, text_type, NULL, 0, NULL);
			XtFree((XtPointer)data);
			data = XmStringUngenerate(str, NULL, text_type, XmUTF8_TEXT);
			XmStringFree(str);
			str  = XmStringComponentCreate(type, strlen((char *)data), data);
			XtFree((XtPointer)data);
		}
	}

	if (!str) str = XmStringComponentCreate(type, len, len ? (XtPointer)s : NULL);
	s += len;
	*stream = s;

	/* The locale (mb/wc tag) component defaults to multibyte */
	if (type == XmSTRING_COMPONENT_LOCALE && *s == XmSTRING_COMPONENT_WIDECHAR_TEXT) {
		if (_XmStrOptimized(str))
			_XmStrTextType(str) = XmWIDECHAR_TEXT;
		else _XmEntryTextTypeSet(_XmStrEntry(str)[0], XmWIDECHAR_TEXT);
	}

	return str;
}

/**
 * Iteratively compute the total size of the serialized stream
 */
static size_t compute_stream_size(const XmString string, size_t *data_sz)
{
	XtPointer val;
	size_t len = 0, n, bytes;
	unsigned int sz;
	XmStringContext ctx;

	if (!XmStringIsValid(string))
		return 0;

	XmStringInitContext(&ctx, string);
	XmStringContextWantUtf8Text(ctx, True);
	while (XmeStringGetComponent(ctx, True, False, &sz, &val) != XmSTRING_COMPONENT_END) {
		if (sz > ASN_MAXLEN) {
			XmeWarning(NULL, "compute_stream_size: Truncating overlong length");
			sz = ASN_MAXLEN;
		}

		n     = sz;
		bytes = 0;
		ASN_LENGTH_BYTES(bytes, n);
		len += 1 + bytes + sz;
	}

	*data_sz = len;
	XmStringFreeContext(ctx);
	n = len; bytes = 0;
	ASN_LENGTH_BYTES(bytes, n);
	return 3 + !!(len > 0x7f) + bytes + len;
}

/**
 * Given a bytestream in BER form, return the size of the stream,
 * including the header; or zero if the stream is invalid.
 */
size_t XmStringSerializedLength(const unsigned char *stream)
{
	size_t hs, len, n, bytes = 0;

	_XmProcessLock();
	if (!XmStringSerializedIsValid(stream) ||
	    !(hs = check_header(&stream, NULL))) {
		_XmProcessUnlock();
		return 0;
	}

	n = len = read_length(&stream);
	ASN_LENGTH_BYTES(bytes, n);
	_XmProcessUnlock();
	return hs + !!(len > 0x7f) + bytes + len;
}

/**
 * Unserialize an XmString
 */
XmString XmStringUnserialize(const unsigned char *stream)
{
	size_t len;
	unsigned char version;
	const unsigned char *end;
	XmString s = NULL;

	_XmProcessLock();
	if (!XmStringSerializedIsValid(stream) ||
	    !(len = check_header(&stream, &version))) {
		_XmProcessUnlock();
		return NULL;
	}

	if (!(len = read_length(&stream))) {
		_XmProcessUnlock();
		return XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	}

	end = stream + len;
	while (stream < end)
		s = XmStringConcatAndFree(s, read_component(&stream, end, version));
	_XmProcessUnlock();
	return s;
}

/**
 * Serialize a XmString
 *
 * if ret is NULL, computes and returns the size only
 */
size_t XmStringSerialize(const XmString string, unsigned char **stream_out)
{
	size_t len, data_len;
	unsigned int sz;
	unsigned char *out = NULL, *val;
	XmStringContext ctx;
	XmStringComponentType t;

	if (!XmStringIsValid(string)) {
		if (stream_out) *stream_out = NULL;
		return 0;
	}

	_XmProcessLock();
	len = compute_stream_size(string, &data_len);
	if (!stream_out || !len) {
		if (stream_out) *stream_out = NULL;
		_XmProcessUnlock();
		return len;
	}

	*stream_out = (unsigned char *)XtMalloc(len);
	out         = write_header(*stream_out, data_len);

	if (data_len) {
		XmStringInitContext(&ctx, string);
		XmStringContextWantUtf8Text(ctx, True);
		while ((t = XmeStringGetComponent(ctx, True, False, &sz, (XtPointer *)&val)) != XmSTRING_COMPONENT_END)
			out = write_component(out, t, sz, (unsigned char *)val);
		XmStringFreeContext(ctx);
	}

	_XmProcessUnlock();
	return len;
}

