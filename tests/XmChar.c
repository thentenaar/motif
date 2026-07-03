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
#include <limits.h>
#include <locale.h>
#include <iconv.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "suites.h"

static void _init_xt(void)
{
	init_xt("check_XmChar");
}

/**
 * Simple cases for length / conversion
 */
static const struct _xmchar_simple {
	unsigned char c[4];
	XmCodepoint cp;
	unsigned int len;
} simple[] = {
	{ { '\0', 0,    0,    0    }, '\0',    1 }, /* '\0' */
	{ { 'a',  0,    0,    0    }, 'a',     1 }, /* 'a' */
	{ { 0xcf, 0x80, 0,    0    }, 0x03c0,  2 }, /*  π  */
	{ { 0xe4, 0xb8, 0xad, 0    }, 0x4e2d,  3 }, /* 中  */
	{ { 0xf0, 0x9d, 0x84, 0x9e }, 0x1d11e, 4 }  /* 𝄞   */
};

START_TEST(len)
{
	ck_assert_msg(XmCharLen(simple[_i].c) == simple[_i].len,
	              "Incorrect length");
}
END_TEST

START_TEST(char_to_codepoint)
{
	ck_assert_msg(XmCharToCodepoint((XmChar)simple[_i].c) == simple[_i].cp,
	              "Unexpected codepoint value");
}
END_TEST

START_TEST(codepoint_to_char)
{
	XmChar x;

	x = XmCodepointToChar(simple[_i].cp);
	ck_assert_msg(!memcmp(simple[_i].c, x, simple[_i].len),
	              "Unexpected char value");
	XtFree((XtPointer)x);
}
END_TEST

START_TEST(invalid_char)
{
	static const unsigned char missing_zero_bit[4] = {0xf8, 0x80, 0x80, 0x80};
	ck_assert_msg(!XmCharToCodepoint(NULL),
	              "Expected NULL to yield codepoint 0");
	ck_assert_msg(!XmCharToCodepoint((XmChar)""),
	              "Expected empty string to yield codepoint 0");
	ck_assert_msg(XmCharToCodepoint((XmChar)missing_zero_bit) == XM_INVALID_CODEPOINT,
	              "Expected invalid codepoint (missing zero bit)");
}
END_TEST

/* Out-of-range / UTF-16 surrogates */
static const XmCodepoint invalid_cp[] = { 0x110000, 0xd800, 0xdffff, 0xdc5a };
static const unsigned char invalid_cp_char[] = { 0xef, 0xbf, 0xbd };

START_TEST(invalid_codepoint)
{
	XmChar x;

	x = XmCodepointToChar(invalid_cp[_i]);
	ck_assert_msg(XmCharLen(x) == 3 && !memcmp(x, invalid_cp_char, 3),
	              "Expected XM_INVALID_CODEPOINT");
	XtFree((XtPointer)x);
}
END_TEST

void xmchar_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmChar");

	t = tcase_create("Simple Cases");
	tcase_add_loop_test(t, len, 0, 5);
	tcase_add_loop_test(t, char_to_codepoint, 0, 5);
	tcase_add_loop_test(t, codepoint_to_char, 0, 5);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Edge Cases");
	tcase_add_test(t, invalid_char);
	tcase_add_test(t, invalid_codepoint);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

