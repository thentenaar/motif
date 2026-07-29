/* $TOG: TxtPropCv.c /main/15 1997/06/18 17:46:05 samborn $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "XmI.h"
#include "TxtPropCv.h"

/**
 * Get the Atom that represents the current locale encoding
 * by converting a simple XPCS string to a text property.
 */
static Atom GetLocaleAtom(Display *d)
{
	int ret = 0;
	XTextProperty prop;
	static const char *xpcs = "ABC";
	Atom atom;

	memset(&prop, 0, sizeof prop);
	ret  = XmbTextListToTextProperty(d, (char **)&xpcs, 1, XTextStyle, &prop);
	atom = (ret >= Success) ? prop.encoding : None;
	XFree(prop.value);
	return atom;
}

/**
 * Convert an array of XmString (XmStringTable) to a TextProperty list
 * according to the given \a style.
 *
 * \param count Number of entries in \a string_table
 * \param style One of the XmSTYLE_* constants (i.e. XmSTYLE_UTF8_STRING)
 * \param prop  XTextProperty to fill in
 *
 * This function returns Success on success, and on failure returns
 * XNoMemory, XConverterNotFound, or XLocaleNotSupported.
 *
 * The pointer returned in prop->value must be freed with XFree().
 */
int XmCvtXmStringTableToTextProperty(Display *d, const XmStringTable string_table,
                                     int count, XmICCEncodingStyle style,
                                     XTextProperty *prop)
{
	int i, ret;
	XTextProperty tmp;
	Atom COMPOUND_TEXT, UTF8_STRING, _MOTIF_COMPOUND_STRING;
	_XmDisplayToAppContext(d);

	if (!count) return Success;
	if (!prop || !string_table || !*string_table)
		return XConverterNotFound;

	_XmAppLock(app);
	prop->value  = NULL;
	prop->nitems = 0;
	prop->format = 8;
	memset(&tmp, 0, sizeof tmp);
	tmp.format = 8;
	COMPOUND_TEXT          = XInternAtom(d, XmSCOMPOUND_TEXT, False);
	UTF8_STRING            = XInternAtom(d, XmSUTF8_STRING, False);
	_MOTIF_COMPOUND_STRING = XInternAtom(d, XmS_MOTIF_COMPOUND_STRING, False);

	/* Determine the encoding to use based on the specified style */
	switch (style) {
	case XmSTYLE_COMPOUND_STRING:
		tmp.encoding = _MOTIF_COMPOUND_STRING;
		break;
	case XmSTYLE_UTF8_STRING:
		tmp.encoding = UTF8_STRING;
		break;
	case XmSTYLE_COMPOUND_TEXT:
		tmp.encoding = COMPOUND_TEXT;
		break;
	case XmSTYLE_LOCALE:
	case XmSTYLE_TEXT:
		tmp.encoding = GetLocaleAtom(d);
		break;
	default:
		tmp.encoding = XA_STRING;
	}
	prop->encoding = tmp.encoding;

	/**
	 * Convert each string in our list, adding a null terminator.
	 */
	for (i = 0; i < count; i++) {
		tmp.value  = NULL;
		tmp.nitems = 0;

		if ((ret = XmCvtXmStringToTextProperty(d, string_table[i], &tmp)) == Success) {
			prop->value = realloc((XtPointer)prop->value, prop->nitems + tmp.nitems + 1);
			memcpy(prop->value + prop->nitems, tmp.value, tmp.nitems);
			prop->nitems += tmp.nitems;
			prop->value[prop->nitems++] = '\0';
			XtFree((XtPointer)tmp.value);
		} else {
			XFree(prop->value);
			prop->value  = NULL;
			prop->nitems = 0;

			/**
			 * The previous incarnation falls back to COMPOUND_TEXT in
			 * this case, so we'll preserve that dubious behavior despite
			 * it being unlikely.
			 */
			if (ret == XLocaleNotSupported && prop->encoding != COMPOUND_TEXT) {
				i = -1;
				prop->encoding = tmp.encoding = COMPOUND_TEXT;
				continue;
			}

			_XmAppUnlock(app);
			return ret;
		}
	}

	/**
	 * The list must be terminated by a null byte, which by convention
	 * is not included in nitems. The last item in the list need not be
	 * null terminated.
	 */
	if (prop->nitems && (count > 1 || prop->value[prop->nitems] == '\0')) {
		if (!--prop->nitems) {
			XtFree((XtPointer)prop->value);
			prop->value = NULL;
		}
	} else if (prop->nitems) {
		prop->value = (unsigned char *)XtRealloc((XtPointer)prop->value, prop->nitems + 1);
		prop->value[prop->nitems + 1] = '\0';
	}

	_XmAppUnlock(app);
	return Success;
}

/**
 * Convert a TextProperty list to XmStringTable
 *
 * Each null-delimited entry from \a prop is placed into \a string_table
 * as a separate entry, with the count of entries in the table returned
 * in \a count.
 *
 * Success is returned if the conversion was successful (or if the
 * TextProperty was empty.) XConverterNotFound is returned if the
 * TextProperty could not be converted, or if invalid parameters were
 * given.
 *
 * If the TextProperty was empty (nitems = 0), the value returned in
 * \a count will be zero, and the value returned in \a string_table will
 * be NULL.
 *
 * The memory for each entry (and \a string_table itself) must be freed
 * with XtFree().
 */
int XmCvtTextPropertyToXmStringTable(Display *d, XTextProperty *prop,
                                     XmStringTable *string_table,
                                     int *count)
{
	unsigned long i, c;
	int j, listlen, ret = Success;
	char **list     = NULL;
	XmStringTag tag = (XmStringTag)"UTF-8";
	XmTextType type = XmCHARSET_TEXT;
	Atom _MOTIF_COMPOUND_STRING;
	_XmDisplayToAppContext(d);

	if (!prop || !string_table || !count)
		return XConverterNotFound;

	*count        = 0;
	*string_table = NULL;
	if (!prop->nitems || !prop->value || (prop->nitems == 1 && !*prop->value))
		return Success;
	_MOTIF_COMPOUND_STRING = XInternAtom(d, XmS_MOTIF_COMPOUND_STRING, False);
	_XmAppLock(app);

	/**
	 * For XmStrings, we just need to unserialize
	 */
	if (prop->encoding == _MOTIF_COMPOUND_STRING) {
		c = 0;
		i = 0;
		while (i < prop->nitems && prop->value[i] != '\0') {
			*string_table = (XmStringTable)XtRealloc((XtPointer)*string_table, ++c * sizeof(XmString));
			(*string_table)[c - 1] = XmStringUnserialize(prop->value + i);
			i += XmStringSerializedLength(prop->value + i) + 1;
		}

		*count = (int)c;
		_XmAppUnlock(app);
		return Success;
	}

	/**
	 * Else, we need to convert to a text/string list
	 */
	if (prop->encoding == GetLocaleAtom(d)) {
		tag  = (XmStringTag)_MOTIF_DEFAULT_LOCALE;
		type = XmMULTIBYTE_TEXT;
		if ((ret = XmbTextPropertyToTextList(d, prop, &list, &listlen)) < Success)
			goto out;
	} else if (prop->encoding == XA_STRING) {
		tag = (XmStringTag)XmFALLBACK_CHARSET;
		ret = XTextPropertyToStringList(prop, &list, &listlen) ? Success : XLocaleNotSupported;
		if (ret < Success)
			goto out;
	}

	if (!list && (ret = Xutf8TextPropertyToTextList(d, prop, &list, &listlen)) < Success)
		goto out;

	/* Finally, generate the XmStringTable */
	*count        = listlen;
	*string_table = (XmStringTable)XtRealloc((XtPointer)*string_table, listlen * sizeof(XmString));
	for (j = 0; j < listlen; j++)
		(*string_table)[j] = XmStringGenerate(list[j], tag, type, NULL);

out:
	_XmAppUnlock(app);
	return ret;
}

/**
 * Convert an XmString to XTextProperty
 *
 * Converts the string specified by \a s into the given \a prop. The
 * encoding field of \a prop should be set to the desired property
 * encoding, which should be one of the following atoms:
 * COMPOUND_TEXT, STRING, UTF8_STRING, or _MOTIF_COMPOUND_STRING.
 *
 * The caller must free the value member of \a prop with XFree()
 * when done with it.
 *
 * This function returns Success on success, and on failure returns
 * XNoMemory, XConverterNotFound, or XLocaleNotSupported as reported
 * by Xutf8TextListToTextProperty.
 */
int XmCvtXmStringToTextProperty(Display *d, const XmString s, XTextProperty *prop)
{
	int ret;
	char *list = NULL;
	XICCEncodingStyle style = XStringStyle;
	Atom COMPOUND_TEXT, UTF8_STRING, _MOTIF_COMPOUND_STRING;

	if (!d || !s || !prop)
		return XConverterNotFound;

	COMPOUND_TEXT          = XInternAtom(d, XmSCOMPOUND_TEXT, False);
	UTF8_STRING            = XInternAtom(d, XmSUTF8_STRING, False);
	_MOTIF_COMPOUND_STRING = XInternAtom(d, XmS_MOTIF_COMPOUND_STRING, False);

	prop->value = NULL;
	if (prop->encoding == _MOTIF_COMPOUND_STRING) {
		prop->nitems = XmStringSerialize(s, (unsigned char **)&list);
		prop->format = 8;
		if (list && prop->nitems) {
			prop->value = (unsigned char *)list;
			return Success;
		}

		return XNoMemory;
	}

	if (prop->encoding == COMPOUND_TEXT) style = XCompoundTextStyle;
	if (prop->encoding == UTF8_STRING)   style = XUTF8StringStyle;
	list = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmUTF8_TEXT);
	ret  = Xutf8TextListToTextProperty(d, &list, 1, style, prop);
	XtFree(list);
	return ret >= Success ? Success : ret;
}

/**
 * Convert a XmTextProperty to XmString
 *
 * Returns the XmString representation of the given \a prop, or NULL
 * if \a prop could not be converted to XmString.
 *
 * Remember to free the resulting XmString with XmStringFree() when
 * done with it.
 */
XmString XmCvtTextPropertyToXmString(Display *d, XTextProperty *prop)
{
	int i, listlen = 0;
	char **list      = NULL;
	XmString out     = NULL;
	XmStringTag tag  = (XmStringTag)"UTF-8";
	XmTextType type  = XmCHARSET_TEXT;
	Atom _MOTIF_COMPOUND_STRING;
	unsigned long j = 0;

	if (!d || !prop || !prop->value || !prop->nitems)
		return NULL;

	_MOTIF_COMPOUND_STRING = XInternAtom(d, XmS_MOTIF_COMPOUND_STRING, False);
	if (prop->encoding == _MOTIF_COMPOUND_STRING) {
		while (j < prop->nitems && prop->value[j] != '\0') {
			out = XmStringConcatAndFree(out, XmStringUnserialize(prop->value + j));
			j  += XmStringSerializedLength(prop->value) + 1;
		}

		return out;
	}

	if (prop->encoding == GetLocaleAtom(d)) {
		tag  = (XmStringTag)_MOTIF_DEFAULT_LOCALE;
		type = XmMULTIBYTE_TEXT;
		if (XmbTextPropertyToTextList(d, prop, &list, &listlen) < Success)
			goto out;
	} else if (prop->encoding == XA_STRING) {
		tag = (XmStringTag)XmFALLBACK_CHARSET;
		if (!XTextPropertyToStringList(prop, &list, &listlen))
			goto out;
	}

	if (!list && Xutf8TextPropertyToTextList(d, prop, &list, &listlen) < Success)
		goto out;

	for (i = 0; i < listlen; i++)
		out = XmStringConcatAndFree(out, XmStringGenerate(list[i], tag, type, NULL));

out:
	XFreeStringList(list);
	return out;
}

