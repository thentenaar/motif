/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "suites.h"

#define N_FONTS 2

static Display *display;
static XFontStruct *font[N_FONTS];
static char * const font_name[N_FONTS] = {
	"fixed",
	"8x13bold"
};

static char * const font_tag[N_FONTS] = {
	XmSTRING_DEFAULT_CHARSET,
	"ISOLatin1"
};

static void _init_xt(void)
{
	display = XtDisplay(init_xt("check_XmFontListEntry"));
}

static void load_fonts(void)
{
	int i;

	memset(font, 0, sizeof font);
	for (i = 0; i < N_FONTS; i++) {
		ck_assert_msg(
			font[i] = XLoadQueryFont(display, font_name[i]),
			"Failed to load font \"%s\"", font_name[i]
		);
	}
}

static void unload_fonts(void)
{
	int i;

	for (i = 0; i < N_FONTS; i++) {
		if (font[i])
			XFreeFont(display, font[i]);
	}
}

START_TEST(create_entry_with_invalid_tag)
{
	XmFontListEntry e;

	ck_assert_msg(
		!(e = XmFontListEntryCreate(NULL, XmFONT_IS_FONT, "fixed")),
		"Shouldn't be able to create a font list entry with a NULL tag"
	);

	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(create_entry_with_invalid_type)
{
	XmFontListEntry e;

	ck_assert_msg(
		!(e = XmFontListEntryCreate(font_tag[0], -1, NULL)),
		"Shouldn't be able to create a font list entry with an invalid type"
	);

	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(create_entry_with_invalid_font)
{
	XmFontListEntry e;

	ck_assert_msg(
		!(e = XmFontListEntryCreate(font_tag[0], XmFONT_IS_FONT, NULL)),
		"Shouldn't be able to create a font list entry with a NULL font"
	);

	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(create_entry_from_font)
{
	XmFontListEntry e;

	ck_assert_msg((e = XmFontListEntryCreate(font_tag[0], XmFONT_IS_FONT, font[0])),
	              "Failed to create a font list entry");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(create_entry_from_fontset)
{
	XmFontListEntry e;

	ck_assert_msg((e = XmFontListEntryCreate(font_tag[0], XmFONT_IS_FONTSET, "*medium-14-*")),
	              "Failed to create a font list entry");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(load_entry_invalid_type)
{
	XmFontListEntry e;
	ck_assert_msg(!(e = XmFontListEntryLoad(display, font_name[1], -1, font_tag[0])),
	              "Shouldn't be able to load an entry with an invalid type");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(load_entry_invalid_tag)
{
	XmFontListEntry e;
	ck_assert_msg(!(e = XmFontListEntryLoad(display, font_name[1], XmFONT_IS_FONT, NULL)),
	              "Shouldn't be able to load an entry with an invalid tag");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

/* Crashes on upstream libXm */
START_TEST(load_entry_invalid_name)
{
	XmFontListEntry e;
	ck_assert_msg(!(e = XmFontListEntryLoad(display, NULL, XmFONT_IS_FONT, font_tag[0])),
	              "Shouldn't be able to load an entry with a NULL name");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(load_entry)
{
	XmFontListEntry e;
	ck_assert_msg((e = XmFontListEntryLoad(display, font_name[1], XmFONT_IS_FONT, font_tag[1])),
	              "Failed to load a font list entry");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(load_entry_xft)
{
	XmFontListEntry e;

	ck_assert_msg((e = XmFontListEntryLoad(display, "Misc Fixed-14:weight=200", XmFONT_IS_XFT, font_tag[0])),
	              "Failed to load a font list entry (xft)");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(get_font_invalid_entry)
{
	XmFontType type = -1;

	ck_assert_msg(!XmFontListEntryGetFont(NULL, &type),
	              "Shouldn't get a font for a NULL entry");
	ck_assert_msg((int)type == -1, "Type shouldn't have changed");
}
END_TEST

START_TEST(get_font)
{
	XmFontType type;
	XmFontListEntry e;

	ck_assert_msg((e = XmFontListEntryCreate(font_tag[1], XmFONT_IS_FONT, font[0])),
	              "Failed to create a font list entry");
	ck_assert_msg(XmFontListEntryGetFont(e, &type) == font[0],
	              "Unexpected result getting entry font");
	ck_assert_msg(type == XmFONT_IS_FONT, "Unexpected entry type");
	if (e) XmFontListEntryFree(&e);
}
END_TEST

START_TEST(get_tag_invalid_entry)
{
	char *tag = NULL;
	ck_assert_msg(!(tag = XmFontListEntryGetTag(NULL)),
	              "Shouldn't be able to get a tag from a NULL entry");
	if (tag) XtFree(tag);
}
END_TEST

START_TEST(get_tag)
{
	char *tag = NULL;
	XmFontListEntry e;

	ck_assert_msg((e = XmFontListEntryCreate(font_tag[1], XmFONT_IS_FONT, font[0])),
	              "Failed to create a font list entry");
	ck_assert_msg((tag = XmFontListEntryGetTag(e)),
	              "Failed to get the tag from the entry");
	ck_assert_msg(tag && !strcmp(tag, font_tag[1]), "Tag mismatch");

	if (tag) XtFree(tag);
	if (e)   XmFontListEntryFree(&e);
}
END_TEST

void xmfontlistentry_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmFontListEntry");

	t = tcase_create("Create entry");
	tcase_add_test(t, create_entry_with_invalid_type);
	tcase_add_test(t, create_entry_with_invalid_tag);
	tcase_add_test(t, create_entry_with_invalid_font);
	tcase_add_test(t, create_entry_from_font);
	tcase_add_test(t, create_entry_from_fontset);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_add_checked_fixture(t, load_fonts, unload_fonts);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Load entry");
	tcase_add_test(t, load_entry_invalid_type);
	tcase_add_test(t, load_entry_invalid_tag);
	tcase_add_test(t, load_entry_invalid_name);
	tcase_add_test(t, load_entry);
	tcase_add_test(t, load_entry_xft);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_add_checked_fixture(t, load_fonts, unload_fonts);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Get entry properties");
	tcase_add_test(t, get_font_invalid_entry);
	tcase_add_test(t, get_font);
	tcase_add_test(t, get_tag_invalid_entry);
	tcase_add_test(t, get_tag);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_add_checked_fixture(t, load_fonts, unload_fonts);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

