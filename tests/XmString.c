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

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "XmStringI.h"
#include "suites.h"

static void _init_xt(void)
{
	setenv("LANG", "C", 1);
	init_xt("check_XmString");
}

/**
 * Create a string consisting of a singular LOCALE_TEXT component
 */
START_TEST(create)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	ck_assert_msg(s, "Resulting string is NULL");

	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TAG,
	              "First component should be the tag");
	ck_assert_msg(len == strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2,
	              "Unexpected length");
	ck_assert_msg(val && !memcmp(val, XmFONTLIST_DEFAULT_TAG_STRING + 2,
	                             strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2),
	              "Tag should be FONTLIST_DEFAULT_TAG_STRING");
	XtFree((XtPointer)val);

	/* 2. text */
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TEXT,
	              "Second component should be TEXT");
	ck_assert_msg(len == 4, "Length should be 4");
	ck_assert_msg(val && !memcmp(val, "test", 4), "Value should be 'test'");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should have exactly two components");
	XmStringFree(s);
	XtFree((XtPointer)val);
	XmStringFreeContext(ctx);
}
END_TEST

/**
 * Create a string with a given tag
 */
START_TEST(create_tag)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringCreate("test", "UTF-99");
	ck_assert_msg(s, "Resulting string is NULL");

	/* 1. tag */
	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TAG,
	              "First component should be the tag");
	ck_assert_msg(len == 6, "Length should be 6");
	ck_assert_msg(val && !memcmp(val, "UTF-99", 6),
	              "Tag should be 'UTF-99'");
	XtFree((XtPointer)val);

	/* 2. text */
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TEXT,
	              "Second component should be TEXT");
	ck_assert_msg(len == 4, "Length should be 4");
	ck_assert_msg(val && !memcmp(val, "test", 4), "Value should be 'test'");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should have exactly two components");
	XmStringFree(s);
	XtFree((XtPointer)val);
	XmStringFreeContext(ctx);
}
END_TEST

/**
 * Create a locale-encoded string
 */
START_TEST(create_localized)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Resulting string is NULL");

	/* 1. tag */
	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TAG,
	              "First component should be the tag");
	ck_assert_msg(len == strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2,
	              "Unexpected length");
	ck_assert_msg(val && !memcmp(val, XmFONTLIST_DEFAULT_TAG_STRING + 2,
	                             strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2),
	              "Tag should be FONTLIST_DEFAULT_TAG_STRING");
	XtFree((XtPointer)val);

	/* 2. text */
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_TEXT,
	              "Second component should be TEXT");
	ck_assert_msg(len == 4, "Length should be 4");
	ck_assert_msg(val && !memcmp(val, "test", 4), "Value should be 'test'");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should have exactly two components");
	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

/**
 * Create a wide string
 */
START_TEST(create_wide)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringCreateWide(L"test", NULL);
	ck_assert_msg(s, "Resulting string is NULL");

	/* 1. tag */
	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_LOCALE,
	              "First component should be the tag (locale)");
	ck_assert_msg(len == strlen(_MOTIF_DEFAULT_LOCALE),
	              "Unexpected length");
	ck_assert_msg(val && !memcmp(val, _MOTIF_DEFAULT_LOCALE,
	                             strlen(_MOTIF_DEFAULT_LOCALE)),
	              "Tag should be _MOTIF_DEFAULT_LOCALE");
	XtFree((XtPointer)val);

	/* 2. text */
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_WIDECHAR_TEXT,
	              "Second component should be WIDECHAR_TEXT");
	ck_assert_msg(len == 16, "Length should be 16");
	ck_assert_msg(val && !wcscmp((wchar_t *)val, L"test"),
	              "Value should be 'test'");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should have exactly two components");
	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

/**
 * Create a multibyte string
 */
extern void _XmStringSetLocaleTag(const char *lang);
START_TEST(create_multibyte)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateMultibyte("test", NULL);
	ck_assert_msg(s, "Resulting string is NULL");

	/* 1. tag */
	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_LOCALE,
	              "First component should be the tag (locale)");
	ck_assert_msg(len == strlen(_MOTIF_DEFAULT_LOCALE),
	              "Unexpected length");
	ck_assert_msg(val && !memcmp(val, _MOTIF_DEFAULT_LOCALE,
	                             strlen(_MOTIF_DEFAULT_LOCALE)),
	              "Tag should be _MOTIF_DEFAULT_LOCALE");
	XtFree((XtPointer)val);

	/* 2. text */
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_LOCALE_TEXT,
	              "Second component should be LOCALE_TEXT");
	ck_assert_msg(len == 4, "Length should be 4");
	ck_assert_msg(val && !memcmp(val, "test", 4),
	              "Value should be 'test'");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should have exactly two components");
	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

static const struct _xmstring_direction_params {
	XmStringDirection dir;
	XmStringComponentType t;
} directions[] = {
	{ XmSTRING_DIRECTION_L_TO_R,  XmSTRING_COMPONENT_DIRECTION },
	{ XmSTRING_DIRECTION_R_TO_L,  XmSTRING_COMPONENT_DIRECTION },
	{ XmSTRING_DIRECTION_UNSET,   XmSTRING_COMPONENT_END       },
	{ XmSTRING_DIRECTION_DEFAULT, XmSTRING_COMPONENT_END       }
};

START_TEST(create_direction)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringDirectionCreate(directions[_i].dir);
	ck_assert_msg(s, "Resulting string is NULL");
	ck_assert_msg(s == XmStringDirectionCreate(directions[_i].dir),
	              "Resulting string should be cached");

	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == directions[_i].t, "Unexpected component value");
	if (t != XmSTRING_COMPONENT_END) {
		ck_assert_msg(len == sizeof(XmStringDirection), "Length should be 1");
		ck_assert_msg(*(XmStringDirection *)val == directions[_i].dir,
		              "Unexpected direction value");
	}

	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should only have one component");
	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

START_TEST(create_separator)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringSeparatorCreate();
	ck_assert_msg(s, "Resulting string is NULL");
	ck_assert_msg(s == XmStringSeparatorCreate(),
	              "Resulting string should be cached");

	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == XmSTRING_COMPONENT_SEPARATOR,
	              "Component should be SEPARATOR");
	ck_assert_msg(!len, "Length should be 0");
	ck_assert_msg(XmStringPeekNextTriple(ctx) == XmSTRING_COMPONENT_END,
	              "String should only have one component");
	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

static const struct _xmstring_component_params {
	XmStringComponentType t;
	unsigned int length;
	XtPointer value;
	Boolean null_result;
} components[] = {
	{ XmSTRING_COMPONENT_TAG, 0, NULL,    True  }, /* No data == NULL */
	{ XmSTRING_COMPONENT_TAG, 3, NULL,    True  }, /* No data == NULL */
	{ XmSTRING_COMPONENT_TAG, 1, "",      True  },
	{ XmSTRING_COMPONENT_TAG, 0, "UTF-8", True  },
	{ XmSTRING_COMPONENT_TAG, 3, "UTF-8", True  }, /* Bad length == NULL */
	{ XmSTRING_COMPONENT_TAG, 5, "UTF-8", False },

	/* This doesn't check length? */
	{ XmSTRING_COMPONENT_TEXT, 4, "test", False },

	{ XmSTRING_COMPONENT_DIRECTION, 0,                         &directions[0].dir, True  },
	{ XmSTRING_COMPONENT_DIRECTION, sizeof(directions[0].dir), &directions[0].dir, False },

	/* Just calls XmStringSeparatorCreate() */
	{ XmSTRING_COMPONENT_SEPARATOR, 0, NULL, False },

	/* This also doesn't return NULL if !length? */
	{ XmSTRING_COMPONENT_LOCALE_TEXT, 4, "test", False },

	{ XmSTRING_COMPONENT_LOCALE, 1, "",                       True  },
	{ XmSTRING_COMPONENT_LOCALE, 0,  "UTF-8",                 True  },
	{ XmSTRING_COMPONENT_LOCALE, 3,  "UTF-8",                 True  },
	{ XmSTRING_COMPONENT_LOCALE, 5,  "UTF-8",                 False },
	{ XmSTRING_COMPONENT_LOCALE, 21, "_MOTIF_DEFAULT_LOCALE", False },

    /* Returns NULL unless value is NULL */
	{ XmSTRING_COMPONENT_LAYOUT_POP, 0, "not null", True  },
	{ XmSTRING_COMPONENT_LAYOUT_POP, 0, NULL,       False },

	{ XmSTRING_COMPONENT_TAB,     0,"not-null",  True  },
	{ XmSTRING_COMPONENT_TAB,     0, NULL,       False },
	{ XmSTRING_COMPONENT_END,     0, "not-null", True  },
	{ XmSTRING_COMPONENT_END,     0, NULL,       False },
	{ XmSTRING_COMPONENT_UNKNOWN, 0, NULL,       True  }
};

START_TEST(create_component)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len   = 0;
	unsigned char *val = NULL;

	s = XmStringComponentCreate(components[_i].t, components[_i].length, components[_i].value);
	if (components[_i].null_result) {
		ck_assert_msg(!s, "Expected null result");
		return;
	} else ck_assert_msg(s, "Expected non-null result");

	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == components[_i].t, "Unexpected component value");

	if (components[_i].value) {
		ck_assert_msg(len == components[_i].length,
		              "Unexpected length");
		ck_assert_msg(val && !memcmp(val, components[_i].value, len),
		              "Unexpected value");
	}

	XmStringFree(s);
	XmStringFreeContext(ctx);
	XtFree((XtPointer)val);
}
END_TEST

/**
 * refcount is 6 bits for an "optimized" string, 8 bits for an unoptimized
 * string
 */
START_TEST(copy_makes_copy)
{
	XmString a, b;
	unsigned char cnt;

	a = XmStringCreateLocalized("test");
	_XmStrRefCountSet(a, 0);
	_XmStrRefCountDec(a);
	cnt = _XmStrRefCountGet(a);
	b = XmStringCopy(a);

	ck_assert_msg(a != b, "Copy should make a copy when refcount overflows");
	ck_assert_msg(_XmStrRefCountGet(b) == 1, "The copy should have 1 reference");
	ck_assert_msg(_XmStrRefCountGet(a) == cnt,
	              "The original refcount should be unchanged");
	_XmStrRefCountSet(a, 1);
	XmStringFree(a);
	XmStringFree(b);
}
END_TEST

START_TEST(copy_null)
{
	ck_assert_msg(!XmStringCopy(NULL), "Copy should return NULL");
}
END_TEST

START_TEST(copy_same)
{
	XmString s, c;

	s = XmStringCreateLocalized("motif");
	c = XmStringCopy(s);
	ck_assert_msg(c == s, "Copy should return the same string");
	XmStringFree(c);
	XmStringFree(s);
}
END_TEST

START_TEST(compare_both_null)
{
	ck_assert_msg(XmStringCompare(NULL, NULL),
	              "A NULL string should equal a NULL string");
}
END_TEST

START_TEST(compare_one_null)
{
	XmString s;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Test string shouldn't be NULL");
	ck_assert_msg(!XmStringCompare(NULL, s),
	              "A NULL string can't be equal to a non-NULL string");
	XmStringFree(s);
}
END_TEST

START_TEST(compare_null_empty)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmStringCompare(s, NULL), "NULL and empty are equivalent");
	ck_assert_msg(XmStringCompare(NULL, s), "NULL and empty are equivalent");
	XmStringFree(s);
}
END_TEST

START_TEST(compare_self)
{
	XmString s;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Test string shouldn't be NULL");
	ck_assert_msg(XmStringCompare(s, s), "String should equal itself");
	XmStringFree(s);
}
END_TEST

START_TEST(compare_same)
{
	XmString a, b;

	a = XmStringCreateLocalized("test");
	b = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	ck_assert_msg(a && b, "Test string(s) shouldn't be NULL");
	ck_assert_msg(XmStringCompare(a, b), "The test strings should be equal");
	XmStringFree(a);
	XmStringFree(b);
}
END_TEST

START_TEST(compare_diff_text)
{
	XmString a, b;

	a = XmStringCreateLocalized("test");
	b = XmStringCreateLocalized("diff");
	ck_assert_msg(a && b, "Test string(s) shouldn't be NULL");
	ck_assert_msg(!XmStringCompare(a, b), "The test strings shouldn't be equal");
	XmStringFree(a);
	XmStringFree(b);
}
END_TEST

START_TEST(compare_diff_tag)
{
	XmString a, b;

	a = XmStringCreate("test", "UTF-8");
	b = XmStringCreate("test", "ISO8859-1");
	ck_assert_msg(a && b, "Test string(s) shouldn't be NULL");
	ck_assert_msg(!XmStringCompare(a, b), "The test strings shouldn't be equal");
	XmStringFree(a);
	XmStringFree(b);
}
END_TEST

START_TEST(empty_null)
{
	ck_assert_msg(XmStringEmpty(NULL), "A NULL string should be empty");
}
END_TEST

START_TEST(empty_is_empty)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmStringEmpty(s), "Empty strings are empty");
	XmStringFree(s);
}
END_TEST

START_TEST(empty_not_empty)
{
	XmString s;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(!XmStringEmpty(s), "Strings with text aren't empty");
	XmStringFree(s);
}
END_TEST

/**
 * These XmStringGenerate() tests also exercise XmStringParseText()
 */
START_TEST(generate_null_text)
{
	ck_assert_msg(!XmStringGenerate(NULL, NULL, XmCHARSET_TEXT, NULL),
	              "Expected NULL result for NULL text input");
}
END_TEST

static XmTextType tag_text_type[] = { XmCHARSET_TEXT, XmMULTIBYTE_TEXT, XmWIDECHAR_TEXT };
START_TEST(generate_tag)
{
	char *val;
	unsigned int len;
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t, ext;

	ext = (tag_text_type[_i] == XmCHARSET_TEXT) ? XmSTRING_COMPONENT_TAG : XmSTRING_COMPONENT_LOCALE;
	ck_assert_msg(s = XmStringGenerate("text", "tag", tag_text_type[_i], NULL),
	              "Expected non-NULL result for tag");
	XmStringInitContext(&ctx, s);
	t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
	ck_assert_msg(t == ext, "Unexpected tag type");
	ck_assert_msg(len == 3, "Unexpected tag length");
	XmStringFree(s);
	XmStringFreeContext(ctx);
}
END_TEST

/**
 * Only XmCHARSET_TEXT, XmWIDECHAR_TEXT, and XmMULTIBYTE_TEXT are valid
 */
START_TEST(generate_invalid_type)
{
	ck_assert_msg(!XmStringGenerate("text", NULL, XmNO_TEXT, NULL),
	              "Expected NULL result for bad type");
}
END_TEST

START_TEST(generate_charset)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len;
	char *val;
	Cardinal i;

	static XmStringComponentType extype[] = {
		XmSTRING_COMPONENT_TAG, XmSTRING_COMPONENT_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_TEXT, XmSTRING_COMPONENT_TAB, XmSTRING_COMPONENT_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_TEXT,
		XmSTRING_COMPONENT_END
	};

	static unsigned int exlen[] = {
		27, 4, 0, 2, 0, 1, 0, 4, 0
	};

	s = XmStringGenerate("this\nis\ta\ntest", NULL, XmCHARSET_TEXT, NULL);
	ck_assert_msg(s, "Expected generate to succeed");

	XmStringInitContext(&ctx, s);
	for (i = 0; i < XtNumber(extype); i++) {
		t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
		ck_assert_msg(t   == extype[i], "Unexpected component type");
		ck_assert_msg(len == exlen[i],  "Unexpected component length");
	}

	XmStringFreeContext(ctx);
	XmStringFree(s);
}
END_TEST

START_TEST(generate_widechar)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len;
	char *val;
	Cardinal i;

	/* TAG is for CHARSET_TEXT, LOCALE for mb/wide */
	static XmStringComponentType extype[] = {
		XmSTRING_COMPONENT_LOCALE,
		XmSTRING_COMPONENT_WIDECHAR_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_WIDECHAR_TEXT, XmSTRING_COMPONENT_TAB, XmSTRING_COMPONENT_WIDECHAR_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_WIDECHAR_TEXT,
		XmSTRING_COMPONENT_END
	};

	static unsigned int exlen[] = {
		21, 16, 0, 8, 0, 4, 0, 16, 0
	};

	s = XmStringGenerate(L"this\nis\ta\ntest", NULL, XmWIDECHAR_TEXT, NULL);
	ck_assert_msg(s, "Expected generate to succeed");

	XmStringInitContext(&ctx, s);
	for (i = 0; i < XtNumber(extype); i++) {
		t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
		ck_assert_msg(t   == extype[i], "Unexpected component type");
		ck_assert_msg(len == exlen[i],  "Unexpected component length");
	}

	XmStringFreeContext(ctx);
	XmStringFree(s);
}
END_TEST

START_TEST(generate_multibyte)
{
	XmString s;
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len;
	char *val;
	Cardinal i;

	/* TAG is for CHARSET_TEXT, LOCALE for mb/wide */
	static XmStringComponentType extype[] = {
		XmSTRING_COMPONENT_LOCALE,
		XmSTRING_COMPONENT_LOCALE_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_LOCALE_TEXT, XmSTRING_COMPONENT_TAB, XmSTRING_COMPONENT_LOCALE_TEXT,
		XmSTRING_COMPONENT_SEPARATOR,
		XmSTRING_COMPONENT_LOCALE_TEXT,
		XmSTRING_COMPONENT_END
	};

	static unsigned int exlen[] = {
		21, 6, 0, 12, 0, 3, 0, 6, 0
	};

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringGenerate("这是\n玉米超人\t的\n测试", NULL, XmMULTIBYTE_TEXT, NULL);
	ck_assert_msg(s, "Expected generate to succeed");

	XmStringInitContext(&ctx, s);
	for (i = 0; i < XtNumber(extype); i++) {
		t = XmStringGetNextTriple(ctx, &len, (XtPointer *)&val);
		ck_assert_msg(t   == extype[i], "Unexpected component type");
		ck_assert_msg(len == exlen[i],  "Unexpected component length");
	}

	XmStringFreeContext(ctx);
	XmStringFree(s);
}
END_TEST

/**
 * Default (set in _init_xt is "C")
 */
START_TEST(getcharset_default)
{
	XmStringTag cset;

	cset = XmStringGetCharset();
	ck_assert_msg(cset && !strcmp(cset, XmFALLBACK_CHARSET),
	              "Expected XmFALLBACK_CHARSET");

	if (cset)
		XtFree(cset);
}
END_TEST

START_TEST(getcharset_from_lang)
{
	XmStringTag cset;

	_XmStringSetLocaleTag("C.KOI8-U");
	cset = XmStringGetCharset();
	ck_assert_msg(cset && !strcmp(cset, "KOI8-U"), "Expected KOI8-U");

	if (cset)
		XtFree(cset);
}
END_TEST

START_TEST(getcharset_no_lang)
{
	XmStringTag cset;

	_XmStringSetLocaleTag("");
	cset = XmStringGetCharset();
	ck_assert_msg(cset && !strcmp(cset, XmFALLBACK_CHARSET),
	              "Expected XmFALLBACK_CHARSET");

	if (cset)
		XtFree(cset);
}
END_TEST

START_TEST(getcharset_utf8)
{
	XmStringTag cset;

	_XmStringSetLocaleTag("POSIX.UTF8");
	cset = XmStringGetCharset();
	ck_assert_msg(cset && !strcmp(cset, "UTF-8"), "Expected UTF-8");

	if (cset)
		XtFree(cset);
}
END_TEST

START_TEST(string_null)
{
	XmString ss;

	ss = XmStringCreateLocalized("sub");
	ck_assert_msg(!XmStringHasSubstring(NULL, ss), "NULL strings cannot have substrings");
	XmStringFree(ss);
}
END_TEST

START_TEST(string_empty)
{
	XmString s, ss;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ss = XmStringCreateLocalized("sub");
	ck_assert_msg(!XmStringHasSubstring(s, ss), "Empty strings cannot have substrings");
	XmStringFree(ss);
	XmStringFree(s);
}
END_TEST

START_TEST(substr_null)
{
	XmString s;

	s = XmStringCreateLocalized("str");
	ck_assert_msg(!XmStringHasSubstring(s, NULL), "Strings cannot have NULL substrings");
	XmStringFree(s);
}
END_TEST

START_TEST(substr_empty)
{
	XmString s, ss;

	s  = XmStringCreateLocalized("str");
	ss = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(!XmStringHasSubstring(s, ss), "Strings cannot have empty substrings");
	XmStringFree(ss);
	XmStringFree(s);
}
END_TEST

START_TEST(substr_simple)
{
	XmString s, ss;

	s  = XmStringCreateLocalized("cornholio");
	ss = XmStringCreateLocalized("hol");
	ck_assert_msg(_XmStrOptimized(s),  "String should be optimized");
	ck_assert_msg(_XmStrOptimized(ss), "Substring should be optimized");
	ck_assert_msg(XmStringHasSubstring(s, ss), "'cornholio' contains 'hol'");
	XmStringFree(ss);
	XmStringFree(s);
}
END_TEST

START_TEST(valid_null)
{
	ck_assert_msg(!XmeStringIsValid(NULL), "NULL strings aren't valid");
}
END_TEST

START_TEST(valid_refcnt_zero)
{
	XmString s;

	s = XmStringCreateLocalized("test");
	_XmStrRefCountSet(s, 0);
	ck_assert_msg(!XmeStringIsValid(s), "Refcount can't be zero");
	_XmStrRefCountSet(s, 1);
	XmStringFree(s);
}
END_TEST

START_TEST(valid_empty)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmeStringIsValid(s), "Empty strings are valid");
	XmStringFree(s);
}
END_TEST

START_TEST(valid_separator_only)
{
	XmString s;

	s = XmStringSeparatorCreate();
	ck_assert_msg(XmeStringIsValid(s), "Separator-only strings are valid");
	XmStringFree(s);
}
END_TEST

/**
 * The memory region we allocate is much smaller than the header size,
 * so this should trigger a memory access error which our signal handler
 * should catch.
 */
START_TEST(valid_bad_ptr)
{
	XmString s = (XmString)XtMalloc(1);
	memset(s, 0xaa, 1);
	ck_assert_msg(!XmeStringIsValid(s), "Small 'strings' we can't grok aren't valid");
	XtFree((XtPointer)s);
}
END_TEST

START_TEST(valid_bad_data)
{
	XmString s = (XmString)XtMalloc(8);
	memset(s, 0x3a, 8);
	ck_assert_msg(!XmeStringIsValid(s), "Strings we can't grok aren't valid");
	XtFree((XtPointer)s);
}
END_TEST

START_TEST(valid_string)
{
	XmString s = XmStringCreateLocalized("test");
	ck_assert_msg(XmeStringIsValid(s), "Strings are valid");
	XmStringFree(s);
}
END_TEST

START_TEST(void_null)
{
	ck_assert_msg(XmStringIsVoid(NULL), "NULL is void, void is NULL");
}
END_TEST

START_TEST(void_empty)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmStringIsVoid(s), "Empty strings are void");
	XmStringFree(s);
}
END_TEST

START_TEST(void_text)
{
	XmString s = XmStringCreateLocalized("test");
	ck_assert_msg(!XmStringIsVoid(s), "Strings with text are not void");
	XmStringFree(s);
}
END_TEST

START_TEST(void_tab)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
	ck_assert_msg(!XmStringIsVoid(s), "Strings with tabs are not void");
	XmStringFree(s);
}
END_TEST

START_TEST(void_separator)
{
	XmString s;

	s = XmStringSeparatorCreate();
	ck_assert_msg(!XmStringIsVoid(s), "Strings with separators are not void");
	XmStringFree(s);
}
END_TEST

START_TEST(line_count_null)
{
	ck_assert_msg(XmStringLineCount(NULL) == 0, "NULL should have no lines");
}
END_TEST

/**
 * One separator == one line
 */
START_TEST(line_count_empty)
{
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmStringLineCount(s) == 1, "Empty strings have one line");
	XmStringFree(s);
}
END_TEST

START_TEST(line_count_separators)
{
	int i;
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	for (i = 0; i < _i; i++)
		s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	ck_assert_msg(XmStringLineCount(s) == _i + 1, "Should have _i + 1 lines");
	XmStringFree(s);
}
END_TEST

/**
 * Optimized strings have one line only
 */
START_TEST(line_count_optimized)
{
	XmString s;

	s = XmStringCreateLocalized("motif");
	ck_assert_msg(_XmStrOptimized(s), "String should be optimized");
	ck_assert_msg(XmStringLineCount(s) == 1, "Optimized strings have one line");
	XmStringFree(s);
}
END_TEST

START_TEST(line_count_unoptimized)
{
	XmString s, x;

	s = XmStringCreateLocalized("motif");
	s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	ck_assert_msg(!_XmStrOptimized(s), "String should be unoptimized");
	ck_assert_msg(XmStringLineCount(s) == 2, "String should have two lines");
	XmStringFree(s);
}
END_TEST

START_TEST(tostream_null)
{
	unsigned char *ret = NULL;
	ck_assert_msg(!XmCvtXmStringToByteStream(NULL, &ret),
	              "Converting a NULL string should yield a length of 0");
	ck_assert_msg(!ret, "The return pointer should be NULL");
}
END_TEST

START_TEST(tostream_empty)
{
	XmString s;
	unsigned char *ret = NULL;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	ck_assert_msg(XmCvtXmStringToByteStream(s, &ret) == 4,
	              "Converting an empty string should yield just the header");
	ck_assert_msg(ret, "No data was returned");
	ck_assert_msg(ret[0] == 0xdf, "The first header byte should be 0xdf");
	ck_assert_msg(ret[1] == 0x80, "The second header byte should be 0x80");
	ck_assert_msg(ret[2] == 0x06, "The third header byte should be 0x06");
	ck_assert_msg(ret[3] == 0x00, "The length field should be 0");
	XmStringFree(s);
	XtFree((XtPointer)ret);
}
END_TEST

START_TEST(tostream_optimized)
{
	XmString s;
	unsigned char *ret = NULL;

	s = XmStringCreateLocalized("motif");
	ck_assert_msg(XmCvtXmStringToByteStream(s, &ret) == 11 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "BER representation should be 11 + length of "
	              "XmFONTLIST_DEFAULT_TAG_STRING bytes long");
	ck_assert_msg(ret, "No data was returned");
	ck_assert_msg(ret[0] == 0xdf, "The first header byte should be 0xdf");
	ck_assert_msg(ret[1] == 0x80, "The second header byte should be 0x80");
	ck_assert_msg(ret[2] == 0x06, "The third header byte should be 0x00");
	ck_assert_msg(ret[3] == 7 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "The length field should be 7 + length of XmFONTLIST_DEFAULT_TAG_STRING");
	ck_assert_msg(ret[4] == XmSTRING_COMPONENT_TAG, "The tag should be TAG");
	ck_assert_msg(ret[5] == strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2,
	              "Tag length should be that of XmFONTLIST_DEFAULT_TAG_STRING without Xm");
	ck_assert_msg(!memcmp(ret + 6, XmFONTLIST_DEFAULT_TAG_STRING + 2,
	              strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2),
	              "The tag should be XmFONTLIST_DEFAULT_TAG_STRING without Xm");
	ck_assert_msg(ret[4 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == XmSTRING_COMPONENT_TEXT,
	              "The second component should be TEXT");
	ck_assert_msg(ret[5 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == 5,
	              "The text length should be 5");
	ck_assert_msg(!memcmp(ret + 6 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "motif", 5), "The text should be 'motif'");
	XmStringFree(s);
	XtFree((XtPointer)ret);
}
END_TEST

START_TEST(tostream_unoptimized)
{
	XmString s;
	unsigned char *ret = NULL;

	s = XmStringCreateLocalized("motif");
	s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	ck_assert_msg(XmCvtXmStringToByteStream(s, &ret) == 13 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "BER representation should be 13 + length of "
	              "XmFONTLIST_DEFAULT_TAG_STRING bytes long");
	ck_assert_msg(ret, "No data was returned");
	ck_assert_msg(ret[0] == 0xdf, "The first header byte should be 0xdf");
	ck_assert_msg(ret[1] == 0x80, "The second header byte should be 0x80");
	ck_assert_msg(ret[2] == 0x06, "The third header byte should be 0x00");
	ck_assert_msg(ret[3] == 9 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "The length field should be 9 + length of XmFONTLIST_DEFAULT_TAG_STRING");
	ck_assert_msg(ret[4] == XmSTRING_COMPONENT_TAG, "The tag should be TAG");
	ck_assert_msg(ret[5] == strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2,
	              "Tag length should be that of XmFONTLIST_DEFAULT_TAG_STRING without Xm");
	ck_assert_msg(!memcmp(ret + 6, XmFONTLIST_DEFAULT_TAG_STRING + 2,
	              strlen(XmFONTLIST_DEFAULT_TAG_STRING) - 2),
	              "The tag should be XmFONTLIST_DEFAULT_TAG_STRING without Xm");
	ck_assert_msg(ret[4 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == XmSTRING_COMPONENT_TEXT,
	              "The second component should be TEXT");
	ck_assert_msg(ret[5 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == 5,
	              "The text length should be 5");
	ck_assert_msg(!memcmp(ret + 6 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "motif", 5), "The text should be 'motif'");
	ck_assert_msg(ret[11 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == XmSTRING_COMPONENT_SEPARATOR,
	              "The third component should be SEPARATOR");
	ck_assert_msg(ret[12 + strlen(XmFONTLIST_DEFAULT_TAG_STRING)] == 0x00,
	              "The third length should be 0");
	XmStringFree(s);
	XtFree((XtPointer)ret);
}
END_TEST

START_TEST(tostream_wide)
{
	XmString s;
	unsigned char *ret = NULL;

	s = XmStringCreateWide(L"motif", NULL);
	ck_assert_msg(XmCvtXmStringToByteStream(s, &ret) == 8 + (sizeof(wchar_t) * 5) + strlen(_MOTIF_DEFAULT_LOCALE),
	              "BER representation should be 8 + sizeof(wchar_t) * 5 "
	              "+ length of _MOTIF_DEFAULT_LOCALE bytes long");
	ck_assert_msg(ret, "No data was returned");
	ck_assert_msg(ret[0] == 0xdf, "The first header byte should be 0xdf");
	ck_assert_msg(ret[1] == 0x80, "The second header byte should be 0x80");
	ck_assert_msg(ret[2] == 0x06, "The third header byte should be 0x00");
	ck_assert_msg(ret[3] == 4 + (5 * sizeof(wchar_t)) + strlen(_MOTIF_DEFAULT_LOCALE),
	              "The length field should be 4 + sizeof(wchar_t) * 5 + "
	              "length of _MOTIF_DEFAULT_LOCALE");
	ck_assert_msg(ret[4] == XmSTRING_COMPONENT_LOCALE, "The tag should be LOCALE");
	ck_assert_msg(ret[5] == strlen(_MOTIF_DEFAULT_LOCALE),
	              "Tag length should be that of _MOTIF_DEFAULT_LOCALE");
	ck_assert_msg(!memcmp(ret + 6, _MOTIF_DEFAULT_LOCALE,
	              strlen(_MOTIF_DEFAULT_LOCALE)),
	              "The tag should be _MOTIF_DEFAULT_LOCALE");
	ck_assert_msg(ret[6 + strlen(_MOTIF_DEFAULT_LOCALE)] == XmSTRING_COMPONENT_WIDECHAR_TEXT,
	              "The second component should be WIDECHAR_TEXT");
	ck_assert_msg(ret[7 + strlen(_MOTIF_DEFAULT_LOCALE)] == 5 * sizeof(wchar_t),
	              "The text length should be 5 * sizeof(wchar_t)");
	ck_assert_msg(!wcsncmp((wchar_t *)(ret + 8 + strlen(_MOTIF_DEFAULT_LOCALE)),
	              L"motif", 5), "The text should be 'motif'");
	XmStringFree(s);
	XtFree((XtPointer)ret);
}
END_TEST

START_TEST(fromstream_null)
{
	ck_assert_msg(!XmCvtByteStreamToXmString(NULL),
	              "Converting a NULL bytestream should yield a NULL string");
}
END_TEST

START_TEST(fromstream_empty)
{
	XmString s, s2 = NULL;
	const unsigned char stream[] = {
		0xdf, 0x80, 0x06, 0x00
	};

	s  = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	s2 = XmCvtByteStreamToXmString(stream);
	ck_assert_msg(XmStringCompare(s, s2), "Strings should compare equal");
	XmStringFree(s);
	XmStringFree(s2);
}
END_TEST

START_TEST(fromstream_optimized)
{
	XmString s, s2;
	unsigned char *stream = NULL;

	s = XmStringCreateLocalized("motif");
	XmCvtXmStringToByteStream(s, &stream);

	s2 = XmCvtByteStreamToXmString(stream);
	ck_assert_msg(XmStringCompare(s, s2), "Strings should compare equal");
	XmStringFree(s);
	XmStringFree(s2);
	XtFree((XtPointer)stream);
}
END_TEST

START_TEST(fromstream_unoptimized)
{
	XmString s, s2;
	unsigned char *stream = NULL;

	s = XmStringCreateLocalized("motif");
	s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	XmCvtXmStringToByteStream(s, &stream);

	s2 = XmCvtByteStreamToXmString(stream);
	ck_assert_msg(!_XmStrOptimized(s2),   "String should not be optimized");
	ck_assert_msg(XmStringCompare(s, s2), "Strings should compare equal");
	XmStringFree(s);
	XmStringFree(s2);
	XtFree((XtPointer)stream);
}
END_TEST

START_TEST(fromstream_wide)
{
	XmString s, s2;
	unsigned char *stream = NULL;

	s = XmStringCreateWide(L"motif", NULL);
	XmCvtXmStringToByteStream(s, &stream);

	s2 = XmCvtByteStreamToXmString(stream);
	ck_assert_msg(XmStringCompare(s, s2), "Strings should compare equal");
	XmStringFree(s);
	XmStringFree(s2);
	XtFree((XtPointer)stream);
}
END_TEST

START_TEST(streamlen_null)
{
	ck_assert_msg(!XmStringByteStreamLength(NULL),
	              "A NULL string results in a 0-length stream");
}
END_TEST

START_TEST(streamlen_empty)
{
	XmString s;
	unsigned char *stream = NULL;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	XmCvtXmStringToByteStream(s, &stream);
	ck_assert_msg(XmStringByteStreamLength(stream) == 4,
	              "An empty string is just a header");
	XmStringFree(s);
	XtFree((XtPointer)s);
}
END_TEST

START_TEST(streamlen_optimized)
{
	XmString s;
	unsigned char *stream = NULL;

	s = XmStringCreateLocalized("motif");
	XmCvtXmStringToByteStream(s, &stream);
	ck_assert_msg(XmStringByteStreamLength(stream) == 11 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "The bytestream should be 11 + length of "
	              "XmFONTLIST_DEFAULT_TAG_STRING bytes long");
	XmStringFree(s);
	XtFree((XtPointer)stream);
}
END_TEST

START_TEST(streamlen_unoptimized)
{
	XmString s;
	unsigned char *stream = NULL;

	s = XmStringCreateLocalized("motif");
	s = XmStringConcatAndFree(s, XmStringSeparatorCreate());
	XmCvtXmStringToByteStream(s, &stream);
	ck_assert_msg(XmStringByteStreamLength(stream) == 13 + strlen(XmFONTLIST_DEFAULT_TAG_STRING),
	              "The bytestream should be 13 + length of "
	              "XmFONTLIST_DEFAULT_TAG_STRING bytes long");
	XmStringFree(s);
	XtFree((XtPointer)stream);
}
END_TEST

START_TEST(unparse_null_charset)
{
	XtPointer t;

	t = XmStringUnparse(NULL, NULL, XmCHARSET_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
}
END_TEST

START_TEST(unparse_null_wide)
{
	XtPointer t;

	t = XmStringUnparse(NULL, NULL, XmWIDECHAR_TEXT,
	                    XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(wchar_t *)t, "Expected empty string");
	XtFree(t);
}
END_TEST

START_TEST(unparse_null_multibyte)
{
	XtPointer t;

	t = XmStringUnparse(NULL, NULL, XmMULTIBYTE_TEXT,
	                    XmMULTIBYTE_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
}
END_TEST

START_TEST(unparse_null_notext)
{
	XtPointer t;

	t = XmStringUnparse(NULL, NULL, XmMULTIBYTE_TEXT,
	                    XmNO_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
}
END_TEST

START_TEST(unparse_empty_charset)
{
	XtPointer t;
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	t = XmStringUnparse(NULL, NULL, XmCHARSET_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_empty_wide)
{
	XtPointer t;
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	t = XmStringUnparse(NULL, NULL, XmWIDECHAR_TEXT,
	                    XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(wchar_t *)t, "Expected empty string");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_empty_multibyte)
{
	XtPointer t;
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	t = XmStringUnparse(NULL, NULL, XmMULTIBYTE_TEXT,
	                    XmMULTIBYTE_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_empty_notext)
{
	XtPointer t;
	XmString s;

	s = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
	t = XmStringUnparse(NULL, NULL, XmMULTIBYTE_TEXT,
	                    XmNO_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!*(char *)t, "Expected empty string");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_charset)
{
	XmString s;
	XtPointer t;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Expected to create a localized string");
	t = XmStringUnparse(s, NULL, XmCHARSET_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_charset_conversion)
{
	XmString s;
	XtPointer t;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("en_US.UTF-8");
	s = XmStringCreate("\xd3\xf1\xc3\xd7\xb3\xac\xc8\xcb", "GB18030");
	ck_assert_msg(s, "Expected to create a string");
	t = XmStringUnparse(s, NULL, XmCHARSET_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "玉米超人"), "Expected '玉米超人'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_wide)
{
	XmString s;
	XtPointer t;

	s = XmStringCreateWide(L"test", NULL);
	ck_assert_msg(s, "Expected to create a wide string");
	t = XmStringUnparse(s, NULL, XmWIDECHAR_TEXT,
	                    XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!wcscmp((wchar_t *)t, L"test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_multibyte)
{
	XmString s;
	XtPointer t;
	wchar_t buf[5];

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateMultibyte("test", NULL);
	ck_assert_msg(s, "Expected to create a multibyte string");
	t = XmStringUnparse(s, NULL, XmMULTIBYTE_TEXT,
	                    XmMULTIBYTE_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_charset_to_wide)
{
	XmString s;
	XtPointer t;

	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Expected to create a localized string");
	t = XmStringUnparse(s, XmSTRING_DEFAULT_CHARSET, XmCHARSET_TEXT,
	                    XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!wcscmp((wchar_t *)t, L"test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_charset_to_multibyte)
{
	XmString s;
	XtPointer t;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateLocalized("test");
	ck_assert_msg(s, "Expected to create a localized string");

	t = XmStringUnparse(s, XmSTRING_DEFAULT_CHARSET, XmCHARSET_TEXT,
	                    XmMULTIBYTE_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_wide_to_charset)
{
	XmString s;
	XtPointer t;

	s = XmStringCreateWide(L"test", NULL);
	ck_assert_msg(s, "Expected to create a wide string");
	t = XmStringUnparse(s, NULL, XmWIDECHAR_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_wide_to_multibyte)
{
	XmString s;
	XtPointer t;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateWide(L"test", NULL);
	ck_assert_msg(s, "Expected to create a wide string");

	t = XmStringUnparse(s, NULL, XmWIDECHAR_TEXT,
	                    XmMULTIBYTE_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_multibyte_to_charset)
{
	XmString s;
	XtPointer t;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateMultibyte("test", NULL);
	ck_assert_msg(s, "Expected to create a multibyte string");

	t = XmStringUnparse(s, NULL, XmMULTIBYTE_TEXT,
	                    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!strcmp(t, "test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

START_TEST(unparse_multibyte_to_wide)
{
	XmString s;
	XtPointer t;

	setlocale(LC_CTYPE, "en_US.UTF-8");
	_XmStringSetLocaleTag("C");
	s = XmStringCreateMultibyte("test", NULL);
	ck_assert_msg(s, "Expected to create a multibyte string");

	t = XmStringUnparse(s, NULL, XmMULTIBYTE_TEXT,
	                    XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_msg(t, "Expected unparse to succeed");
	ck_assert_msg(!wcscmp((wchar_t *)t, L"test"), "Expected 'test'");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

void xmstring_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmString");

	t = tcase_create("Create");
	tcase_add_test(t, create);
	tcase_add_test(t, create_tag);
	tcase_add_test(t, create_localized);
	tcase_add_test(t, create_wide);
	tcase_add_test(t, create_multibyte);
	tcase_add_loop_test(t, create_direction, 0, 4);
	tcase_add_test(t, create_separator);
	tcase_add_loop_test(t, create_component, 0, XtNumber(components));

	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Copy");
	tcase_add_test(t, copy_makes_copy);
	tcase_add_test(t, copy_null);
	tcase_add_test(t, copy_same);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Compare");
	tcase_add_test(t, compare_both_null);
	tcase_add_test(t, compare_one_null);
	tcase_add_test(t, compare_null_empty);
	tcase_add_test(t, compare_self);
	tcase_add_test(t, compare_same);
	tcase_add_test(t, compare_diff_text);
	tcase_add_test(t, compare_diff_tag);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Empty");
	tcase_add_test(t, empty_null);
	tcase_add_test(t, empty_is_empty);
	tcase_add_test(t, empty_not_empty);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Generate");
	tcase_add_test(t, generate_null_text);
	tcase_add_loop_test(t, generate_tag, 0, XtNumber(tag_text_type));
	tcase_add_test(t, generate_invalid_type);
	tcase_add_test(t, generate_charset);
	tcase_add_test(t, generate_widechar);
	tcase_add_test(t, generate_multibyte);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetCharset");
	tcase_add_test(t, getcharset_default);
	tcase_add_test(t, getcharset_from_lang);
	tcase_add_test(t, getcharset_no_lang);
	tcase_add_test(t, getcharset_utf8);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("HasSubstring");
	tcase_add_test(t, string_null);
	tcase_add_test(t, string_empty);
	tcase_add_test(t, substr_null);
	tcase_add_test(t, substr_empty);
	tcase_add_test(t, substr_simple);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("IsValid");
	tcase_add_test(t, valid_null);
	tcase_add_test(t, valid_refcnt_zero);
	tcase_add_test(t, valid_empty);
	tcase_add_test(t, valid_separator_only);
	tcase_add_test(t, valid_bad_ptr);
	tcase_add_test(t, valid_bad_data);
	tcase_add_test(t, valid_string);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("IsVoid");
	tcase_add_test(t, void_null);
	tcase_add_test(t, void_empty);
	tcase_add_test(t, void_text);
	tcase_add_test(t, void_tab);
	tcase_add_test(t, void_separator);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("LineCount");
	tcase_add_test(t, line_count_null);
	tcase_add_test(t, line_count_empty);
	tcase_add_loop_test(t, line_count_separators, 1, 5);
	tcase_add_test(t, line_count_optimized);
	tcase_add_test(t, line_count_unoptimized);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("StringToByteStream");
	tcase_add_test(t, tostream_null);
	tcase_add_test(t, tostream_empty);
	tcase_add_test(t, tostream_optimized);
	tcase_add_test(t, tostream_unoptimized);
	tcase_add_test(t, tostream_wide);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("ByteStreamToString");
	tcase_add_test(t, fromstream_null);
	tcase_add_test(t, fromstream_empty);
	tcase_add_test(t, fromstream_optimized);
	tcase_add_test(t, fromstream_unoptimized);
	tcase_add_test(t, fromstream_wide);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("ByteStreamLength");
	tcase_add_test(t, streamlen_null);
	tcase_add_test(t, streamlen_empty);
	tcase_add_test(t, streamlen_optimized);
	tcase_add_test(t, streamlen_unoptimized);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Unparse");
	tcase_add_test(t, unparse_null_charset);
	tcase_add_test(t, unparse_null_wide);
	tcase_add_test(t, unparse_null_multibyte);
	tcase_add_test(t, unparse_null_notext);
	tcase_add_test(t, unparse_empty_charset);
	tcase_add_test(t, unparse_empty_wide);
	tcase_add_test(t, unparse_empty_multibyte);
	tcase_add_test(t, unparse_empty_notext);
	tcase_add_test(t, unparse_charset);
	tcase_add_test(t, unparse_charset_conversion);
	tcase_add_test(t, unparse_wide);
	tcase_add_test(t, unparse_multibyte);
	tcase_add_test(t, unparse_charset_to_wide);
	tcase_add_test(t, unparse_charset_to_multibyte);
	tcase_add_test(t, unparse_wide_to_charset);
	tcase_add_test(t, unparse_wide_to_multibyte);
	tcase_add_test(t, unparse_multibyte_to_charset);
	tcase_add_test(t, unparse_multibyte_to_wide);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

