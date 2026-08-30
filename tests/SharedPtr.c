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

#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "SharedPtrI.h"
#include "suites.h"

struct _XmSharedPtr {
	void *ptr;
	_Atomic unsigned long ref;     /**< Ref count on ptr         */
	_Atomic unsigned long weakref; /**< Ref count on this struct */
	void (*free)(void *);          /**< Free proc for ptr        */
	void *(*dup)(void *);          /**< Duplicate proc for ptr   */
};

struct dummy {
	Boolean freed;
	Boolean dupd;
};

static void _init_xt(void)
{
	init_xt("check_SharedPtr");
}

static void _destroy(void *ptr)
{
	((struct dummy *)ptr)->freed = True;
}

static void *_dup(void *ptr)
{
	((struct dummy *)ptr)->dupd = True;
	return XtCalloc(1, sizeof(struct dummy));
}

START_TEST(shared_ptr_create)
{
	XmSharedPtr sp;
	struct dummy d;

	memset(&d, 0, sizeof d);
	sp = XmSharedPtrCreate(&d, _destroy, _dup);
	ck_assert_msg(sp, "Failed to create shared pointer");
	ck_assert_msg(*(struct dummy **)sp == &d, "Failed to resolve pointer");
	ck_assert_msg(sp->ref     == 1, "Refcount should be 1");
	ck_assert_msg(sp->weakref == 1, "Weakref count should be 1");
	ck_assert_msg(!d.dupd,  "Underlying object should not have been duplicated");
	ck_assert_msg(!d.freed, "Underlying object should not have been freed");

	XmSharedPtrFree(sp);
	ck_assert_msg(!d.dupd, "Underlying object should not have been duplicated");
	ck_assert_msg(d.freed, "Expected underlying object to be freed");
}
END_TEST

START_TEST(shared_ptr_create_null)
{
	ck_assert_msg(!XmSharedPtrCreate(NULL, NULL, NULL), "Expected NULL result");
}
END_TEST

START_TEST(shared_ptr_copy)
{
	XmSharedPtr p, p2;
	struct dummy d;

	memset(&d, 0, sizeof d);
	p  = XmSharedPtrCreate(&d, _destroy, _dup);
	p2 = XmSharedPtrCopy(p, False);
	ck_assert_msg(p2 == p,        "Expected the pointers to be equal");
	ck_assert_msg(p->ref == 2,    "Expected the refcount to be 2");
	ck_assert_msg(p->weakref == 1, "Expected the weak refcount to be 1");
	XmSharedPtrFree(p);
}
END_TEST

START_TEST(shared_ptr_copy_duplicate)
{
	XmSharedPtr p1, p2;
	struct dummy d;

	memset(&d, 0, sizeof d);
	p1 = XmSharedPtrCreate(&d, _destroy, _dup);
	p2 = XmSharedPtrCopy(p1, True);
	ck_assert_msg(p2,       "Expected a non-NULL result");
	ck_assert_msg(p2 != p1, "Expected p2 to not equal p1");
	ck_assert_msg(d.dupd,   "Expected d to have been duplicated");
	ck_assert_msg(!d.freed, "d should not have been freed");
	ck_assert_msg(p1->ref == 1, "p1's refcount should be 1");
	ck_assert_msg(p2->ref == 1, "p2's refcount should be 1");
	XtFree((XtPointer)p2->ptr);
	XtFree((XtPointer)p2);
	XtFree((XtPointer)p1);
}
END_TEST

START_TEST(shared_ptr_copy_overflow)
{
	XmSharedPtr p1, p2;
	struct dummy d;

	memset(&d, 0, sizeof d);
	p1 = XmSharedPtrCreate(&d, _destroy, _dup);
	p1->ref = ULONG_MAX;

	p2 = XmSharedPtrCopy(p1, False);
	ck_assert_msg(!p2,      "Expected a NULL result");
	ck_assert_msg(!d.dupd,  "Expected d to have been duplicated");
	ck_assert_msg(!d.freed, "d should not have been freed");
	ck_assert_msg(p1->ref == ULONG_MAX, "p1's refcount should be ULONG_MAX");
	XtFree((XtPointer)p1);
}
END_TEST

START_TEST(shared_ptr_get)
{
	XmSharedPtr x = (XmSharedPtr)(void *)XtCalloc(1, sizeof *x);
	x->ptr = (void *)1234;
	ck_assert_msg(!XmSharedPtrGet(NULL), "Get on a NULL pointer should return NULL");
	ck_assert_msg(!XmSharedPtrGet(x),    "Get on a pointer with no references should return NULL");

	x->ref = 1;
	ck_assert_msg(XmSharedPtrGet(x) == (void *)1234, "Unexpected pointer value");
	XtFree((XtPointer)x);
}
END_TEST

START_TEST(shared_ptr_get_null)
{
	ck_assert_msg(!XmSharedPtrGet(NULL), "Expected NULL result");
}
END_TEST

START_TEST(weak_ptr_create_null)
{
	ck_assert_msg(!XmWeakPtrCreate(NULL), "Expected NULL result");
}
END_TEST

START_TEST(weak_ptr_create)
{
	XmSharedPtr x;
	XmWeakPtr w;
	struct dummy d;

	memset(&d, 0, sizeof d);
	x = XmSharedPtrCreate(&d, _destroy, _dup);
	w = XmWeakPtrCreate(x);
	ck_assert_msg(w, "Expected to create a weak pointer");
	ck_assert_msg(*(void **)w == x, "Expected weak pointer to wrap the shared pointer");
	ck_assert_msg(x->weakref == 2, "Expected weakref count to be 2");

	XmWeakPtrFree(w);
	ck_assert_msg(x->weakref == 1, "Expected weakref count to be 1");

	XmSharedPtrFree(x);
	ck_assert_msg(d.freed, "Expected d to be freed");
}
END_TEST

START_TEST(weak_ptr_create_overflow)
{
	XmSharedPtr x;
	XmWeakPtr w;
	struct dummy d;

	memset(&d, 0, sizeof d);
	x = XmSharedPtrCreate(&d, _destroy, _dup);
	x->weakref = ULONG_MAX;
	w = XmWeakPtrCreate(x);
	ck_assert_msg(!w, "Expected weak pointer creation to fail");
	ck_assert_msg(x->weakref == ULONG_MAX, "Expected weak refcount to be ULONG_MAX");
	x->weakref = 1;
	XmSharedPtrFree(x);
	ck_assert_msg(d.freed, "Expected d to be freed");
}
END_TEST

START_TEST(weak_ptr_lock)
{
	XmSharedPtr x, y;
	XmWeakPtr w;
	struct dummy d;

	memset(&d, 0, sizeof d);
	x = XmSharedPtrCreate(&d, _destroy, _dup);
	w = XmWeakPtrCreate(x);
	y = XmWeakPtrLock(w);
	ck_assert_msg(y == x,          "Expected y to be a copy of x");
	ck_assert_msg(x->weakref == 2, "Expected weak refcount to be 2");
	ck_assert_msg(x->ref == 2,     "Expected refcount to be 2");
	XmSharedPtrFree(y);
	XmWeakPtrFree(w);
}
END_TEST

START_TEST(weak_ptr_lock_null)
{
	ck_assert_msg(!XmWeakPtrLock(NULL), "Expected NULL result");
}
END_TEST

START_TEST(weak_ptr_lives)
{
	XmSharedPtr x;
	XmWeakPtr w;
	struct dummy d;

	memset(&d, 0, sizeof d);
	x = XmSharedPtrCreate(&d, _destroy, _dup);
	w = XmWeakPtrCreate(x);
	ck_assert_msg(XmWeakPtrLives(w), "Expected w to be alive");
	XmWeakPtrFree(w);
	XmSharedPtrFree(x);
}
END_TEST

START_TEST(weak_ptr_survives)
{
	XmSharedPtr x;
	XmWeakPtr w;
	struct dummy d;

	memset(&d, 0, sizeof d);
	x = XmSharedPtrCreate(&d, _destroy, _dup);
	w = XmWeakPtrCreate(x);
	XmSharedPtrFree(x);
	ck_assert_msg(d.freed, "Expected d to have been freed");
	ck_assert_msg(!XmWeakPtrLives(w), "Expected w to be dead");
	ck_assert_msg(x->weakref == 1, "Expected weak refcount to be 1");
	XmWeakPtrFree(w);
}
END_TEST

void sharedptr_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("SharedPtr");

	t = tcase_create("Strong");
	tcase_add_test(t, shared_ptr_create);
	tcase_add_test(t, shared_ptr_create_null);
	tcase_add_test(t, shared_ptr_copy);
	tcase_add_test(t, shared_ptr_copy_duplicate);
	tcase_add_test(t, shared_ptr_copy_overflow);
	tcase_add_test(t, shared_ptr_get);
	tcase_add_test(t, shared_ptr_get_null);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Weak");
	tcase_add_test(t, weak_ptr_create);
	tcase_add_test(t, weak_ptr_create_null);
	tcase_add_test(t, weak_ptr_create_overflow);
	tcase_add_test(t, weak_ptr_lock);
	tcase_add_test(t, weak_ptr_lock_null);
	tcase_add_test(t, weak_ptr_lives);
	tcase_add_test(t, weak_ptr_survives);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

