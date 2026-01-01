/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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
#include <Xm/Cursor.h>
#include <check.h>
#include "suites.h"

#include <config.h>

static Widget shell;
static Display *display;
static Screen *screen;

static void _init_xt(void)
{
	shell   = init_xt("check_Cursor");
	display = XtDisplay(shell);
	screen  = DefaultScreenOfDisplay(display);
}

START_TEST(load_image_noimage)
{
	Cursor c;

	c = XmeLoadCursorImage(display, screen, RESOURCE(cursor/nx_image), 0, 0);
	ck_assert_msg(c == None, "Should fail to load non-existent image");

	if (c != None)
		XFreeCursor(display, c);
}
END_TEST

START_TEST(load_image_svg)
{
#if XM_WITH_XRENDER
	Cursor c = None;
	XVisualInfo vis;

	/* Unfortunately, check lacks a skip mechanism */
	if (XMatchVisualInfo(display, XScreenNumberOfScreen(screen), 32, TrueColor, &vis)) {
		c = XmeLoadCursorImage(display, screen, RESOURCE(cursor/svg), 0, 0);
		ck_assert_msg(c != None, "Should load an SVG cursor image");
	} else puts("Skipping test (no 32-bit TrueColor visual)");

	if (c != None)
		XFreeCursor(display, c);
#else
	puts("Skipping test (no Xrender support)");
#endif /* XM_WITH_RENDER */
}
END_TEST

START_TEST(load_image_png)
{
#if XM_WITH_XRENDER && XM_WITH_PNG
	Cursor c = None;
	XVisualInfo vis;

	if (XMatchVisualInfo(display, XScreenNumberOfScreen(screen), 32, TrueColor, &vis)) {
		c = XmeLoadCursorImage(display, screen, RESOURCE(cursor/png), 0, 0);
		ck_assert_msg(c != None, "Should load an PNG cursor image");
	} else puts("Skipping test (no 32-bit TrueColor visual)");

	if (c != None)
		XFreeCursor(display, c);
#else
	puts("Skipping test (no Xrender or PNG support)");
#endif
}
END_TEST

START_TEST(load_xcursor_file)
{
#if XM_WITH_XRENDER && XM_WITH_XCURSOR
	Cursor c = None;
	XVisualInfo vis;

	if (XMatchVisualInfo(display, XScreenNumberOfScreen(screen), 32, TrueColor, &vis)) {
		c = XmeLoadCursor(display, screen, RESOURCE(cursor/left_ptr.xcur));
		ck_assert_msg(c != None, "Should load a XCursor file");
	} else puts("Skipping test (no 32-bit TrueColor visual)");

	if (c != None)
		XFreeCursor(display, c);
#else
	puts("Skipping test (no Xrender or Xcursor support)");
#endif
}
END_TEST

START_TEST(load_non_xcursor_file)
{
#if XM_WITH_XRENDER && XM_WITH_XCURSOR
	Cursor c = None;
	XVisualInfo vis;

	if (XMatchVisualInfo(display, XScreenNumberOfScreen(screen), 32, TrueColor, &vis)) {
		c = XmeLoadCursor(display, screen, RESOURCE(cursor/left_ptr.svg));
		ck_assert_msg(c == None, "Should fail to load a non-XCursor file");
	} else puts("Skipping test (no 32-bit TrueColor visual)");

	if (c != None)
		XFreeCursor(display, c);
#else
	puts("Skipping test (no Xrender or Xcursor support)");
#endif
}
END_TEST

START_TEST(non_existent_cursor)
{
	Cursor c;

	c = XmeLoadCursor(display, screen, "non-existent-cursor");
	ck_assert_msg(c == None, "Should fail to load non existent cursor");
	if (c != None)
		XFreeCursor(display, c);
}
END_TEST

START_TEST(empty_cursor)
{
	Cursor c;

	c = XmeLoadCursor(display, screen, "");
	ck_assert_msg(c == None, "Should fail to load empty cursor");
	if (c != None)
		XFreeCursor(display, c);
}
END_TEST

START_TEST(null_cursor)
{
	Cursor c;

	c = XmeLoadCursor(display, screen, NULL);
	ck_assert_msg(c == None, "Should fail to load null cursor");
	if (c != None)
		XFreeCursor(display, c);
}
END_TEST

START_TEST(left_ptr)
{
	Cursor c;

	c = XmeLoadCursor(display, screen, "left_ptr");
	ck_assert_msg(c != None, "Should load left_ptr");
	if (c != None)
		XFreeCursor(display, c);
}
END_TEST

void cursor_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("Cursor");

	t = tcase_create("Image");
	tcase_add_test(t, load_image_noimage);
	tcase_add_test(t, load_image_svg);
	tcase_add_test(t, load_image_png);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("XCursor");
	tcase_add_test(t, load_xcursor_file);
	tcase_add_test(t, load_non_xcursor_file);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Cursor");
	tcase_add_test(t, non_existent_cursor);
	tcase_add_test(t, empty_cursor);
	tcase_add_test(t, null_cursor);
	tcase_add_test(t, left_ptr);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

