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

#include <stdlib.h>
#include <locale.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "suites.h"

static Display *display;

static void _init_xt(void)
{
	Widget shell;

	setenv("LC_ALL", "C", 1);
	setlocale(LC_ALL, "C");
	shell   = init_xt("check_TxtPropCv");
	display = XtDisplay(shell);
}

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

START_TEST(null_to_text_property)
{
	XTextProperty prop;
	XmString s;

	s = XmStringCreateLocalized("tp indigeo pro culo meo");
	ck_assert_msg(XmCvtXmStringToTextProperty(NULL, s, &prop) == XConverterNotFound,
	              "Expected XConverterNotFound for NULL display");
	ck_assert_msg(XmCvtXmStringToTextProperty(display, NULL, &prop) == XConverterNotFound,
	              "Expected XConverterNotFound for NULL string");
	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, NULL) == XConverterNotFound,
	              "Expected XConverterNotFound for NULL prop");
	XmStringFree(s);
}
END_TEST

START_TEST(xmstring_to_compound_string)
{
	size_t len;
	XmString s;
	XTextProperty prop;
	unsigned char *serialized;
	Atom _MOTIF_COMPOUND_STRING = XInternAtom(display, XmS_MOTIF_COMPOUND_STRING, False);

	s   = XmStringCreateLocalized("test");
	len = XmStringSerialize(s, &serialized);
	memset(&prop, 0, sizeof prop);
	prop.encoding = _MOTIF_COMPOUND_STRING;

	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, &prop) == Success,
	              "Failed to convert XmString to _MOTIF_COMPOUND_STRING");
	ck_assert_msg(prop.format == 8, "Expected 8-bit format");
	ck_assert_msg(prop.nitems == len, "Expected len to be %lu (got %lu)",
	              len, prop.nitems);
	ck_assert_msg(!memcmp(prop.value, serialized, len),
	              "Expected bytes to compare equal");
	XFree(prop.value);
	XtFree((XtPointer)serialized);
	XmStringFree(s);
}
END_TEST

START_TEST(xmstring_to_utf8_string)
{
	XmString s;
	XTextProperty prop;
	Atom UTF8_STRING = XInternAtom(display, XmSUTF8_STRING, False);

	s = XmStringGenerate("\xf0\x9d\x95\xa5\x65st", (XmStringTag)"UTF-8",
	                     XmCHARSET_TEXT, NULL);
	memset(&prop, 0, sizeof prop);
	prop.encoding = UTF8_STRING;

	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, &prop) == Success,
	              "Failed to convert XmString to UTF8_STRING");
	ck_assert_msg(prop.format == 8, "Expected 8-bit format");
	ck_assert_msg(prop.nitems == 7, "Expected len to be 7 (got %lu)", prop.nitems);
	ck_assert_msg(!memcmp(prop.value, "\xf0\x9d\x95\xa5\x65st", prop.nitems),
	              "Expected bytes to compare equal");
	XFree(prop.value);
	XmStringFree(s);
}
END_TEST

START_TEST(xmstring_to_compound_text)
{
	XmString s;
	XTextProperty prop;
	Atom COMPOUND_TEXT = XInternAtom(display, XmSCOMPOUND_TEXT, False);

	static const unsigned char expected[13] = {
		0x1b, 0x25, 0x47, 0xf0, 0x9d, 0x95, 0xa5,
		0x1b, 0x25, 0x40, 0x65, 0x73, 0x74
	};

	s = XmStringGenerate("\xf0\x9d\x95\xa5\x65st", (XmStringTag)"UTF-8",
	                     XmCHARSET_TEXT, NULL);
	memset(&prop, 0, sizeof prop);
	prop.encoding = COMPOUND_TEXT;

	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, &prop) == Success,
	              "Failed to convert XmString to COMPOUND_TEXT");
	ck_assert_msg(prop.format == 8, "Expected 8-bit format");
	ck_assert_msg(prop.nitems == sizeof expected, "Expected len to be %lu (got %lu)", sizeof expected, prop.nitems);
	ck_assert_msg(!memcmp(prop.value, expected, prop.nitems),
	              "Expected bytes to compare equal");
	XFree(prop.value);
	XmStringFree(s);
}
END_TEST

START_TEST(xmstring_to_locale)
{
	XmString s;
	XTextProperty prop;

	s = XmStringCreateLocalized("local");
	memset(&prop, 0, sizeof prop);
	prop.encoding = GetLocaleAtom(display);

	ck_assert_msg(prop.encoding != None, "Failed to get locale Atom");
	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, &prop) == Success,
	              "Failed to convert XmString to locale");
	ck_assert_msg(prop.format == 8, "Expected 8-bit format");
	ck_assert_msg(prop.nitems == 5, "Expected len to be 5 (got %lu)", prop.nitems);
	ck_assert_msg(!memcmp(prop.value, "local", prop.nitems),
	              "Expected bytes to compare equal");
	XFree(prop.value);
	XmStringFree(s);
}
END_TEST

START_TEST(xmstring_to_string)
{
	XmString s;
	XTextProperty prop;

	s = XmStringCreateLocalized("Carthago delenda est");
	memset(&prop, 0, sizeof prop);
	prop.encoding = XA_STRING;

	ck_assert_msg(XmCvtXmStringToTextProperty(display, s, &prop) == Success,
	              "Failed to convert XmString to string");
	ck_assert_msg(prop.format == 8, "Expected 8-bit format");
	ck_assert_msg(prop.nitems == 20, "Expected len to be 20 (got %lu)", prop.nitems);
	ck_assert_msg(!memcmp(prop.value, "Carthago delenda est", prop.nitems),
	              "Expected bytes to compare equal");
	XFree(prop.value);
	XmStringFree(s);
}
END_TEST

START_TEST(null_text_property)
{
	XTextProperty prop;

	memset(&prop, 0, sizeof prop);
	prop.nitems = 1;

	ck_assert_msg(!XmCvtTextPropertyToXmString(NULL, &prop),
	              "Expected NULL for NULL display");
	ck_assert_msg(!XmCvtTextPropertyToXmString(display, NULL),
	              "Expected NULL for NULL prop");
	ck_assert_msg(!XmCvtTextPropertyToXmString(display, &prop),
	              "Expected NULL for NULL value");

	prop.nitems = 0;
	ck_assert_msg(!XmCvtTextPropertyToXmString(display, &prop),
	              "Expected NULL for 0 nitems");
}
END_TEST

START_TEST(compound_string_to_xmstring)
{
	size_t len;
	XmString s, x;
	XTextProperty prop;
	unsigned char *serialized;
	Atom _MOTIF_COMPOUND_STRING = XInternAtom(display, XmS_MOTIF_COMPOUND_STRING, False);

	s   = XmStringCreateLocalized("test");
	len = XmStringSerialize(s, &serialized);
	memset(&prop, 0, sizeof prop);
	prop.encoding = _MOTIF_COMPOUND_STRING;
	prop.format   = 8;
	prop.nitems   = len;
	prop.value    = serialized;

	ck_assert_msg(x = XmCvtTextPropertyToXmString(display, &prop),
	              "Failed to convert _MOTIF_COMPOUND_STRING to XmString");
	ck_assert_msg(XmStringCompare(x, s), "Expected strings to compare equal");
	XtFree((XtPointer)serialized);
	XmStringFree(x);
	XmStringFree(s);
}
END_TEST

START_TEST(utf8_string_to_xmstring)
{
	XmString s, x;
	XTextProperty prop;
	Atom UTF8_STRING = XInternAtom(display, XmSUTF8_STRING, False);

	s = XmStringGenerate("\xf0\x9d\x95\xa5\x65st", (XmStringTag)"UTF-8",
	                     XmCHARSET_TEXT, NULL);
	memset(&prop, 0, sizeof prop);
	prop.encoding = UTF8_STRING;
	prop.format   = 8;
	prop.nitems   = 7;
	prop.value    = (unsigned char *)"\xf0\x9d\x95\xa5\x65st";

	ck_assert_msg(x = XmCvtTextPropertyToXmString(display, &prop),
	              "Failed to convert UTF8_STRING to XmString");
	ck_assert_msg(XmStringCompare(x, s), "Expected strings to compare equal");
	XmStringFree(x);
	XmStringFree(s);
}
END_TEST

START_TEST(compound_text_to_xmstring)
{
	XmString s, x;
	XTextProperty prop;
	Atom COMPOUND_TEXT = XInternAtom(display, XmSCOMPOUND_TEXT, False);

	static const unsigned char expected[13] = {
		0x1b, 0x25, 0x47, 0xf0, 0x9d, 0x95, 0xa5,
		0x1b, 0x25, 0x40, 0x65, 0x73, 0x74
	};

	s = XmStringGenerate("\xf0\x9d\x95\xa5\x65st", (XmStringTag)"UTF-8",
	                     XmCHARSET_TEXT, NULL);
	memset(&prop, 0, sizeof prop);
	prop.encoding = COMPOUND_TEXT;
	prop.format   = 8;
	prop.nitems   = sizeof expected;
	prop.value    = (unsigned char *)expected;

	ck_assert_msg(x = XmCvtTextPropertyToXmString(display, &prop),
	              "Failed to convert COMPOUND_TEXT to XmString");
	ck_assert_msg(XmStringCompare(s, x), "Expected strings to compare equal");
	XmStringFree(s);
	XmStringFree(x);
}
END_TEST

START_TEST(locale_to_xmstring)
{
	XmString s, x;
	XTextProperty prop;

	s = XmStringCreateMultibyte("local", _MOTIF_DEFAULT_LOCALE);
	memset(&prop, 0, sizeof prop);
	prop.encoding = GetLocaleAtom(display);
	prop.format   = 8;
	prop.nitems   = 5;
	prop.value    = (unsigned char *)"local";

	ck_assert_msg(prop.encoding != None, "Failed to get locale Atom");
	ck_assert_msg(x = XmCvtTextPropertyToXmString(display, &prop),
	              "Failed to convert locale to XmString");
	ck_assert_msg(XmStringCompare(s, x), "Expected strings to compare equal");
	XmStringFree(s);
	XmStringFree(x);
}
END_TEST

START_TEST(string_to_xmstring)
{
	XmString s, x;
	XTextProperty prop;

	if (GetLocaleAtom(display) == XA_STRING)
		s =  XmStringCreateMultibyte("Carthago delenda est", _MOTIF_DEFAULT_LOCALE);
	else s = XmStringCreate("Carthago delenda est", XmFALLBACK_CHARSET);

	memset(&prop, 0, sizeof prop);
	prop.encoding = XA_STRING;
	prop.format   = 8;
	prop.nitems   = 20;
	prop.value    = (unsigned char *)"Carthago delenda est";

	ck_assert_msg(x = XmCvtTextPropertyToXmString(display, &prop),
	              "Failed to convert string to XmString");
	ck_assert_msg(XmStringCompare(s, x), "Expected strings to compare equal");
	XmStringFree(s);
	XmStringFree(x);
}
END_TEST

void txtpropcv_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("TxtPropCv");

	t = tcase_create("XmStringToTextProp");
	tcase_add_test(t, null_to_text_property);
	tcase_add_test(t, xmstring_to_compound_string);
	tcase_add_test(t, xmstring_to_utf8_string);
	tcase_add_test(t, xmstring_to_compound_text);
	tcase_add_test(t, xmstring_to_locale);
	tcase_add_test(t, xmstring_to_string);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("TextPropToXmString");
	tcase_add_test(t, null_text_property);
	tcase_add_test(t, compound_string_to_xmstring);
	tcase_add_test(t, utf8_string_to_xmstring);
	tcase_add_test(t, compound_text_to_xmstring);
	tcase_add_test(t, locale_to_xmstring);
	tcase_add_test(t, string_to_xmstring);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

