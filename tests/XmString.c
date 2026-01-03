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
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "XmStringI.h"
#include "suites.h"

static Display *display;

static void _init_xt(void)
{
	display = XtDisplay(init_xt("check_XmString"));
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

	{ XmSTRING_COMPONENT_LOCALE, 0,  "UTF-8",                 True  },
	{ XmSTRING_COMPONENT_LOCALE, 3,  "UTF-8",                 True  },
	{ XmSTRING_COMPONENT_LOCALE, 5,  "UTF-8",                 True  },
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

void xmstring_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmString");

	t = tcase_create("Create");
	tcase_add_test(t, create);
	tcase_add_test(t, create_tag);
	tcase_add_test(t, create_localized);
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
	tcase_add_test(t, compare_self);
	tcase_add_test(t, compare_same);
	tcase_add_test(t, compare_diff_text);
	tcase_add_test(t, compare_diff_tag);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Empty");
	tcase_add_test(t, empty_null);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

