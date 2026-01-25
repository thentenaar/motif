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
#include <Xm/DesktopP.h>
#include <check.h>

#include "suites.h"

static Widget shell;

static void _init_xt(void)
{
	shell = init_xt("check_XmDesktopObject");
}

START_TEST(initialize)
{
	Widget w;
	XmDesktopObject d;

	w = XtCreateWidget("desktop", xmDesktopClass, shell, NULL, 0);
	ck_assert_msg(w, "Failed to create a XmDesktopObject");
	if ((d = (XmDesktopObject)w)) {
		ck_assert_msg(!(d->desktop.num_slots),    "Should have 0 slots");
		ck_assert_msg(!(d->desktop.num_children), "Should have 0 children");
		ck_assert_msg(!(d->desktop.children),     "Should have no child storage");
		XtDestroyWidget(w);
	}
}
END_TEST

START_TEST(add_child)
{
	Widget w, x;
	XmDesktopObject dw;
	XmDesktopObjectClass c;

	w = XtCreateWidget("desktop", xmDesktopClass, shell, NULL, 0);
	ck_assert_msg(w, "Failed to create a XmDesktopObject");

	x = XtCreateWidget("child", xmDesktopClass, w, NULL, 0);
	ck_assert_msg(w, "Failed to create child XmDesktopObject");

	c  = (XmDesktopObjectClass)XtClass(w);
	dw = (XmDesktopObject)w;
	((XmDesktopObject)x)->desktop.parent = w;

	c->desktop_class.insert_child(x);
	ck_assert_msg(dw->desktop.num_slots    == 2, "Should have 2 slots");
	ck_assert_msg(dw->desktop.num_children == 1, "Should have 1 child");
	ck_assert_msg(dw->desktop.children && dw->desktop.children[0] == x,
	              "Child[0] should be child");

	XtDestroyWidget(x);
	XtDestroyWidget(w);
}
END_TEST

START_TEST(delete_child)
{
	Widget w, x;
	XmDesktopObject dw;
	XmDesktopObjectClass c;

	w = XtCreateWidget("desktop", xmDesktopClass, shell, NULL, 0);
	ck_assert_msg(w, "Failed to create a XmDesktopObject");

	x = XtCreateWidget("child", xmDesktopClass, w, NULL, 0);
	ck_assert_msg(w, "Failed to create child XmDesktopObject");

	c  = (XmDesktopObjectClass)XtClass(w);
	dw = (XmDesktopObject)w;
	((XmDesktopObject)x)->desktop.parent = w;

	c->desktop_class.insert_child(x);
	c->desktop_class.delete_child(x);
	ck_assert_msg(dw->desktop.num_slots == 2,  "Should have 2 slots");
	ck_assert_msg(!(dw->desktop.num_children), "Should have 0 children");
	ck_assert_msg(!dw->desktop.children[0],    "The former child should be NULL");
	XtDestroyWidget(x);
	XtDestroyWidget(w);
}
END_TEST

START_TEST(delete_one_of_many)
{
	int i;
	Widget w, children[3];
	XmDesktopObject d;
	XmDesktopObjectClass c;
	char buf[7];

	w = XtCreateWidget("desktop", xmDesktopClass, shell, NULL, 0);
	ck_assert_msg(w, "Failed to create a XmDesktopObject");

	d = (XmDesktopObject)w;
	c = (XmDesktopObjectClass)XtClass(w);
	for (i = 0; i < 3; i++) {
		sprintf(buf, "child%d", i);
		children[i] = XtCreateWidget(buf, xmDesktopClass, w, NULL, 0);
		ck_assert_msg(children[i], "Failed to create a child XmDesktopObject");
		((XmDesktopObject)children[i])->desktop.parent = w;
		c->desktop_class.insert_child(children[i]);
	}

	ck_assert_msg(d->desktop.num_slots    == 5,           "Should have 5 slots");
	ck_assert_msg(d->desktop.num_children == 3,           "Should have 3 children");
	ck_assert_msg(d->desktop.children[0]  == children[0], "Unexpected child[0]");
	ck_assert_msg(d->desktop.children[1]  == children[1], "Unexpected child[1]");
	ck_assert_msg(d->desktop.children[2]  == children[2], "Unexpected child[2]");

	c->desktop_class.delete_child(children[1]);
	ck_assert_msg(d->desktop.num_slots    == 5,           "Should have 2 slots");
	ck_assert_msg(d->desktop.num_children == 2,           "Should have 2 children");
	ck_assert_msg(d->desktop.children[0]  == children[0], "Unexpected child[0]");
	ck_assert_msg(d->desktop.children[1]  == children[2], "Unexpected child[1]");
	for (i = 0; i < 3; i++) { XtDestroyWidget(children[i]); }
	XtDestroyWidget(w);
}
END_TEST

START_TEST(delete_child_when_empty)
{
	Widget w;
	XmDesktopObject dw;

	w = XtCreateWidget("desktop", xmDesktopClass, shell, NULL, 0);
	ck_assert_msg(w, "Failed to create a XmDesktopObject");

	dw = (XmDesktopObject)w;
	dw->desktop.parent = w;

	((XmDesktopObjectClass)XtClass(w))->desktop_class.delete_child(w);
	ck_assert_msg(!(dw->desktop.num_slots),    "Should have 0 slots");
	ck_assert_msg(!(dw->desktop.num_children), "Should have 0 children");
	ck_assert_msg(!(dw->desktop.children),     "Should have no child storage");
	XtDestroyWidget(w);
}
END_TEST

void xmdesktopobject_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmDesktopObject");

	t = tcase_create("Init");
	tcase_add_test(t, initialize);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Add");
	tcase_add_test(t, add_child);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Delete");
	tcase_add_test(t, delete_child);
	tcase_add_test(t, delete_one_of_many);
	tcase_add_test(t, delete_child_when_empty);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

