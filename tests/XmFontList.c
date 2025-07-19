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

static Display *display;

static void _init_xt(void)
{
	display = XtDisplay(init_xt("check_XmFontList"));
}

START_TEST(create_from_invalid_entry)
{
	XmFontList fl;

	ck_assert_msg(!(fl = XmFontListAppendEntry(NULL, NULL)),
	              "Adding a NULL entry should fail");
	if (fl) XmFontListFree(fl);
}
END_TEST

START_TEST(create_from_valid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg((e && (fl = XmFontListAppendEntry(NULL, e))),
	              "Failed to create a list from a valid entry");

	if (fl) XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(add_invalid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);

	ck_assert_msg(fl, "Failed to create the font list");
	ck_assert_msg((XmFontListAppendEntry(fl, NULL) == fl),
	              "Shouldn't be able to add a NULL entry");
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(add_valid_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);

	ck_assert_msg(fl, "Failed to create the font list");
	ck_assert_msg(e,  "Failed to create font list entry");
	ck_assert_msg((fl && e2 && (fx = XmFontListAppendEntry(fl, e2))),
	              "Failed to add a valid entry");
	ck_assert(fx && fx != fl);

	if (fx) XmFontListFree(fx);
	else XmFontListFree(fl);
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
}
END_TEST

START_TEST(remove_entry_invalid_list)
{
	XmFontListEntry e;

	e = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg(!XmFontListRemoveEntry(NULL, e), "Unexpected result");
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(remove_entry_invalid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListRemoveEntry(fl, NULL) == fl, "Unexpected result");
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(remove_entry_not_in_list)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e2)) == fl,
	              "Unexpected return value");
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
}
END_TEST

START_TEST(remove_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(fl, e2);
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e2)) != fl,
	              "Unexpected return value");
	XmFontListFree(fx);
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
}
END_TEST

START_TEST(remove_sole_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e)) != fl,
	              "Unexpected return value");
	if (fx) XmFontListFree(fx);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(copy_list_invalid)
{
	XmFontList fl;
	ck_assert_msg(!(fl = XmFontListCopy(NULL)), "Expected result to be NULL");
	if (fl) XmFontListFree(fl);
}
END_TEST

START_TEST(copy_list)
{
	XmFontList fl, fx;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(fl != (fx = XmFontListCopy(fl)) && fx, "Unexpected result");
	if (fx) XmFontListFree(fx);
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(font_context_null_return)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(!XmFontListInitFontContext(NULL, fl),
	              "Should fail if fc is NULL");
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(font_context_empty_list)
{
	XmFontContext fc = NULL;

	ck_assert_msg(!XmFontListInitFontContext(&fc, NULL) && !fc,
	              "Should fail if the list is empty");
	if (fc) XmFontListFreeFontContext(fc);
}
END_TEST

START_TEST(enum_null_context)
{
	ck_assert_msg(!XmFontListNextEntry(NULL), "Should fail if no context");
}
END_TEST

START_TEST(enum_first_entry)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");

	e2 = XmFontListNextEntry(fc);
	ck_assert_msg(e2 && !strcmp(XmFontListEntryGetTag(e2), XmFontListEntryGetTag(e)),
	              "Failed to get first entry");
	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(enum_second_entry)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e, e2, e3;

	e  = XmFontListEntryLoad(display, "fixed",    XmFONT_IS_FONT, "FIRST-ENTRY");
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, "SECOND-ENTRY");
	fl = XmFontListAppendEntry(XmFontListAppendEntry(NULL, e), e2);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");
	e3 = XmFontListNextEntry(fc);
	ck_assert_msg(e3 && !strcmp(XmFontListEntryGetTag(e3), XmFontListEntryGetTag(e)),
	              "Failed to get first entry");
	e3 = XmFontListNextEntry(fc);
	ck_assert_msg(e2 && !strcmp(XmFontListEntryGetTag(e3), XmFontListEntryGetTag(e2)),
	              "Failed to get second entry");

	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
}
END_TEST

START_TEST(enum_beyond_the_end)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, "FIRST-ENTRY");
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");
	ck_assert_msg(!strcmp(XmFontListEntryGetTag(XmFontListNextEntry(fc)), XmFontListEntryGetTag(e)),
	              "Failed to get first entry");
	ck_assert_msg(!XmFontListNextEntry(fc),
	              "Should get NULL beyond the end of the list");
	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
	XmFontListEntryFree(&e);
}
END_TEST

void xmfontlist_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmFontList");

	t = tcase_create("Create font list");
	tcase_add_test(t, create_from_invalid_entry);
	tcase_add_test(t, create_from_valid_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Add an entry to a font list");
	tcase_add_test(t, add_invalid_entry);
	tcase_add_test(t, add_valid_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Remove an entry from a font list");
	tcase_add_test(t, remove_entry_invalid_list);
	tcase_add_test(t, remove_entry_invalid_entry);
	tcase_add_test(t, remove_entry_not_in_list);
	tcase_add_test(t, remove_entry);
	tcase_add_test(t, remove_sole_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Copy a font list");
	tcase_add_test(t, copy_list_invalid);
	tcase_add_test(t, copy_list);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Enumerate a font list");
	tcase_add_test(t, font_context_null_return);
	tcase_add_test(t, font_context_empty_list);
	tcase_add_test(t, enum_null_context);
	tcase_add_test(t, enum_first_entry);
	tcase_add_test(t, enum_second_entry);
	tcase_add_test(t, enum_beyond_the_end);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

