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

#include <X11/Intrinsic.h>
#include <Xm/Screen.h>
#include <Xm/ScreenP.h>
#include <Xm/DesktopP.h>
#include <check.h>

#include "suites.h"

static Widget shell;

static void _init_xt(void)
{
	shell = init_xt("check_XmScreen");
}

START_TEST(get_xmscreen)
{
	XmScreen s;

	ck_assert_msg((s = XmScreenOfObject(shell)), "Failed to create screen");
	ck_assert_msg(XmScreenOfObject(shell) == s,  "Failed to get screen");
}
END_TEST

START_TEST(initialize)
{
	XmScreen s;

	ck_assert_msg((s = XmScreenOfObject(shell)), "Failed to get screen");
	ck_assert_msg(xmScreenClassRec.desktop_class.insert_child ==
	              xmDesktopClassRec.desktop_class.insert_child,
	              "Incorrect insert_child callback");
	ck_assert_msg(xmScreenClassRec.desktop_class.delete_child ==
	              xmDesktopClassRec.desktop_class.delete_child,
	              "Incorrect delete_child callback");
}
END_TEST

void xmscreen_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmScreen");

	t = tcase_create("Get/Create an XmScreen object");
	tcase_add_test(t, get_xmscreen);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("XmDesktopObject compatibility");
	tcase_add_test(t, initialize);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

