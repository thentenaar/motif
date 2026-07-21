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
#include <limits.h>
#include <errno.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <XmCharI.h>
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
	static const unsigned char s[3]  = {0xed, 0xa0, 0x80};
	static const unsigned char s2[3] = {0xed, 0xbf, 0xbf};

	ck_assert_msg(!XmCharToCodepoint(NULL),
	              "Expected NULL to yield codepoint 0");
	ck_assert_msg(!XmCharToCodepoint((XmChar)""),
	              "Expected empty string to yield codepoint 0");
	ck_assert_msg(XmCharToCodepoint((XmChar)missing_zero_bit) == XM_INVALID_CODEPOINT,
	              "Expected invalid codepoint (missing zero bit)");
	ck_assert_msg(XmCharToCodepoint((XmChar)missing_zero_bit) == XM_INVALID_CODEPOINT,
	              "Expected invalid codepoint (missing zero bit)");
	ck_assert_msg(XmCharToCodepoint((XmChar)missing_zero_bit) == XM_INVALID_CODEPOINT,
	              "Expected invalid codepoint (missing zero bit)");
	ck_assert_msg(XmCharToCodepoint((XmChar)s) == XM_INVALID_CODEPOINT,
	              "Surrogates (0xd000) should yield XM_INVALID_CODEPOINT");
	ck_assert_msg(XmCharToCodepoint((XmChar)s2) == XM_INVALID_CODEPOINT,
	              "Surrogates (0xdfff) should yield XM_INVALID_CODEPOINT");
}
END_TEST

START_TEST(invalid_codepoint)
{
	XmChar x;

	/* Out-of-range / UTF-16 surrogates */
	static const XmCodepoint invalid_cp[] = { 0x110000, 0xd800, 0xdffff, 0xdc5a };
	static const unsigned char invalid_cp_char[] = { 0xef, 0xbf, 0xbd };

	x = XmCodepointToChar(invalid_cp[_i]);
	ck_assert_msg(XmCharLen(x) == 3 && !memcmp(x, invalid_cp_char, 3),
	              "Expected XM_INVALID_CODEPOINT");
	XtFree((XtPointer)x);
}
END_TEST

#define N_WB_CASES 51
static const struct wb_case {
	XmCodepoint a;
	XmCodepoint b;
	Boolean result;
} wb_cases[N_WB_CASES] = {
	{ 0x00000d, 0x00000a, False }, /* WB3: CR x LF */
	{ 0x00000d, 0x00000d, True  }, /* WB3a-b: CR / CR */
	{ 0x00000d, 0x000085, True  }, /* WB3a-b: CR / NEL */
	{ 0x000085, 0x00000d, True  }, /* WB3a-b: NEL / CR */
	{ 0x000085, 0x000085, True  }, /* WB3a-b: NEL / NEL */
	{ 0x000061, 0x000085, True  }, /* WB3a-b: ALetter / NEL */
	{ 0x000085, 0x000061, True  }, /* WB3a-b: NEL / ALetter */
	{ 0x00200d, 0x01f595, False }, /* WB3c: ZWJ x Extended_Pictographic */
	{ 0x000020, 0x000020, False }, /* WB3d: WSegSpace x WSegSpace */
	{ 0x000300, 0x000308, False }, /* WB4: Extend x Extend */
	{ 0x000308, 0x0000ad, False }, /* WB4: Extend x Format */
	{ 0x000308, 0x00200d, False }, /* WB4: Extend x ZWJ */
	{ 0x000061, 0x000061, False }, /* WB5: ALetter x ALetter */
	{ 0x0005d0, 0x0005d0, False }, /* WB5: HebrewLetter x HebrewLetter */
	{ 0x0005d0, 0x000061, False }, /* WB5: HebrewLetter x ALetter */
	{ 0x000061, 0x0005d0, False }, /* WB5: ALetter x HebrewLetter */
	{ 0x000061, 0x000033, False }, /* WB6: ALetter x MidLetter */
	{ 0x0005d0, 0x00ff1a, False }, /* WB6: HebrewLetter x MidLetter */
	{ 0x00ff1a, 0x000062, False }, /* WB7: MidLetter x ALetter */
	{ 0x00ff1a, 0x0005d0, False }, /* WB7: MidLetter x HebrewLetter */
	{ 0x0005d0, 0x000027, False }, /* WB7a: HebrewLetter x SingleQuote */
	{ 0x0005d0, 0x000022, False }, /* WB7b: HebrewLetter x DoubleQuote */
	{ 0x000022, 0x0005d0, False }, /* WB7c: DoubleQuote x HebrewLetter */
	{ 0x000030, 0x000039, False }, /* WB8: Numeric x Numeric */
	{ 0x000061, 0x000033, False }, /* WB9: ALetter x Numeric */
	{ 0x0005d0, 0x000033, False }, /* WB9: HebrewLetter x Numeric */
	{ 0x000030, 0x000061, False }, /* WB10: Numeric x ALetter */
	{ 0x000061, 0x0005d0, False }, /* WB10: Numeric x HebrewLetter */
	{ 0x00002c, 0x000033, False }, /* WB11: MidNum x Numeric */
	{ 0x002024, 0x000033, False }, /* WB11: MidNumLet x Numeric */
	{ 0x000027, 0x000033, False }, /* WB11: Single_Quote x Numeric */
	{ 0x000033, 0x00002c, False }, /* WB12: Numeric x MidNum */
	{ 0x000033, 0x002024, False }, /* WB12: Numeric x MidNumLet */
	{ 0x000033, 0x000027, False }, /* WB12: Numeric x Single_Quote */
	{ 0x003031, 0x003031, False }, /* WB13: Katakana x Katakana */
	{ 0x000061, 0x00202f, False }, /* WB13a: ALetter x ExtendNumLet */
	{ 0x0005d0, 0x00202f, False }, /* WB13a: HebrewLetter x ExtendNumLet */
	{ 0x000030, 0x00202f, False }, /* WB13a: Numeric x ExtendNumLet */
	{ 0x003031, 0x00202f, False }, /* WB13a: Katakana x ExtendNumLet */
	{ 0x00202f, 0x00202f, False }, /* WB13a: ExtendNumLet x ExtendNumLet */
	{ 0x00202f, 0x000061, False }, /* WB13b: ExtendNumLet x ALetter */
	{ 0x00202f, 0x0005d0, False }, /* WB13b: ExtendNumLet x HebrewLetter */
	{ 0x00202f, 0x000030, False }, /* WB13b: ExtendNumLet x Numeric */
	{ 0x00202f, 0x003031, False }, /* WB13b: ExtendNumLet x Katakana */
	{ 0x01f1e6, 0x01f1e6, False }, /* WB15/16: RI x RI */
	{ 0x00000a, 0x003031, True  }, /* LF / Katakana */
	{ 0x00002c, 0x000308, False }, /* MidNum x Extend */
	{ 0x000027, 0x002060, False }, /* Single_Quote x Format */
	{ 0x01f1e6, 0x000041, True  }, /* RI / ALetter */
	{ 0x01f1e6, 0x0005d0, True  }, /* RI / HebrewLetter */
	{ 0x01f1e6, 0x000308, False }  /* RI x Extend */
};

START_TEST(word_boundary)
{
	ck_assert_msg(XmCodepointIsWordBoundary(wb_cases[_i].a, wb_cases[_i].b) == wb_cases[_i].result,
	              "Case %u failed", _i);
}
END_TEST

START_TEST(compose_invalid_params)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(XM_CODEPOINT_MAX + 1, XM_CODEPOINT_MAX);
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");

	cp = XmCodepointCompose(XM_CODEPOINT_MAX, XM_CODEPOINT_MAX + 1);
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");

	cp = XmCodepointCompose(XM_CODEPOINT_MAX + 1, XM_CODEPOINT_MAX + 1);
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * L + L is invalid
 */
START_TEST(compose_hangul_LL)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00110b, 0x00110b);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * L + T is invalid
 */
START_TEST(compose_hangul_LT)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00110b, 0x0011bf);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * V + L is invalid
 */
START_TEST(compose_hangul_VL)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x001167, 0x00110b);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * V + T is invalid
 */
START_TEST(compose_hangul_VT)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x001167, 0x0011bf);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * V + V is invalid
 */
START_TEST(compose_hangul_VV)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x001167, 0x001167);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * L + V = LV
 */
START_TEST(compose_hangul_LV)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00110b, 0x001167);
	ck_assert_msg(cp == 0x00c5ec, "Expected LV YEO, got U+%X", cp);
}
END_TEST

/**
 * LV + T = LVT
 */
START_TEST(compose_hangul_LVT)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00c5ec, 0x0011bf);
	ck_assert_msg(cp == 0x00c604, "Expected LVT YEOK, got U+%X", cp);
}
END_TEST

/**
 * LV + L is invalid
 */
START_TEST(compose_hangul_LVL)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00c604, 0x00110b);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * LV + V is invalid
 */
START_TEST(compose_hangul_LVV)
{
	XmCodepoint cp;

	cp = XmCodepointCompose(0x00c604, 0x001167);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * If we don't have a composition for a set of codepoints, we should get
 * XM_INVALID_CODEPOINT / ENOTSUPP
 */
START_TEST(compose_no_composition)
{
	XmCodepoint cp;

	/* Q + combining Titlo should not combine */
	cp = XmCodepointCompose(0x000051, 0x000483);
	ck_assert_msg(errno == ENOTSUP, "Expected errno to be ENOTSUP");
	ck_assert_msg(cp == XM_INVALID_CODEPOINT, "Expected XM_INVALID_CODEPOINT");
}
END_TEST

/**
 * Ensure correct behavior when the composition list is traversed.
 */
START_TEST(compose)
{
	XmCodepoint cp;

	/* U + Combining Hook Above is pretty far down the combination list */
	cp = XmCodepointCompose(0x000055, 0x000309);
	ck_assert_msg(cp == 0x001ee6, "Expected U+1EE6, got U+%X", cp);
}
END_TEST

START_TEST(decompose_invalid_params)
{
	size_t ret;
	XmCodepoint buf[5];

	/* Null buf */
	ret = XmCodepointDecompose(0x001ee6, NULL, sizeof buf / sizeof *buf);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");

	/* 0 size */
	ret = XmCodepointDecompose(0x001ee6, buf, 0);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");

	/* Codepoint out of range */
	ret = XmCodepointDecompose(XM_CODEPOINT_MAX + 1, buf, sizeof buf / sizeof *buf);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == EINVAL, "Expected errno to be EINVAL");
}
END_TEST

#define N_NODECOMP 4
static const XmCodepoint nodecomp[N_NODECOMP] = {
	0x00110b, /* Hangul L */
	0x001167, /* Hangul V */
	0x0011bf, /* Hangul T */
	0x000051  /* U */
};

/**
 * If it can't decompose, it should yield the given codepoint.
 */
START_TEST(decompose_no_decomposition)
{
	size_t ret;
	XmCodepoint buf[5];

	memset(buf, 0, sizeof buf);
	ret = XmCodepointDecompose(nodecomp[_i], buf, sizeof buf / sizeof *buf);
	ck_assert_msg(ret == 1, "Expected ret == 1");
	ck_assert_msg(buf[0] == nodecomp[_i], "Expected buf[0] == U+%X", nodecomp[_i]);
}
END_TEST

/**
 * LV should decompose into <L, V>
 */
START_TEST(decompose_hangul_LV)
{
	size_t ret;
	XmCodepoint buf[3];

	/* Insufficient space */
	ret = XmCodepointDecompose(0x00c5ec, buf, 1);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == ENOSPC, "Expected errno to be ENOSPC");

	/* Sufficient space */
	ret = XmCodepointDecompose(0x00c5ec, buf, 2);
	ck_assert_msg(ret == 2, "Expected ret == 2");
	ck_assert_msg(buf[0] == 0x00110b, "Expected buf[0] == U+110B (U+%X)", buf[0]);
	ck_assert_msg(buf[1] == 0x001167, "Expected buf[1] == U+1167 (U+%X)", buf[1]);
}
END_TEST

/**
 * LVT should fully decompose into <L, V, T>
 */
START_TEST(decompose_hangul_LVT)
{
	size_t ret;
	XmCodepoint buf[3];

	/* Insufficient space */
	ret = XmCodepointDecompose(0x00d4db, buf, 1);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == ENOSPC, "Expected errno to be ENOSPC");

	/* 2 is expected for LV, but this is LVT */
	ret = XmCodepointDecompose(0x00d4db, buf, 2);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == ENOSPC, "Expected errno to be ENOSPC");

	/* Sufficient space */
	ret = XmCodepointDecompose(0x00d4db, buf, 3);
	ck_assert_msg(ret == 3, "Expected ret == 3");
	ck_assert_msg(buf[0] == 0x001111, "Expected buf[0] == U+1111 (U+%X)", buf[0]);
	ck_assert_msg(buf[1] == 0x001171, "Expected buf[1] == U+1171 (U+%X)", buf[1]);
	ck_assert_msg(buf[2] == 0x0011b6, "Expected buf[2] == U+11B6 (U+%X)", buf[2]);
}
END_TEST

START_TEST(decompose)
{
	size_t ret;
	XmCodepoint buf[3];

	/* Insufficient space */
	ret = XmCodepointDecompose(0x001ff2, buf, 1);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == ENOSPC, "Expected errno to be ENOSPC");

	ret = XmCodepointDecompose(0x001ff2, buf, 2);
	ck_assert_msg(!ret, "Expected ret == 0");
	ck_assert_msg(errno == ENOSPC, "Expected errno to be ENOSPC");

	/* Sufficient space */
	ret = XmCodepointDecompose(0x001ff2, buf, 3);
	ck_assert_msg(ret == 3, "Expected ret == 3");
	ck_assert_msg(buf[0] == 0x0003c9, "Expected buf[0] == U+03C9 (U+%X)", buf[0]);
	ck_assert_msg(buf[1] == 0x000300, "Expected buf[1] == U+0300 (U+%X)", buf[1]);
	ck_assert_msg(buf[2] == 0x000345, "Expected buf[2] == U+0345 (U+%X)", buf[2]);
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

	t = tcase_create("Unicode");
	tcase_add_loop_test(t, word_boundary, 0, N_WB_CASES);
	tcase_add_test(t, compose_invalid_params);
	tcase_add_test(t, compose_hangul_LL);
	tcase_add_test(t, compose_hangul_LT);
	tcase_add_test(t, compose_hangul_VL);
	tcase_add_test(t, compose_hangul_VT);
	tcase_add_test(t, compose_hangul_VV);
	tcase_add_test(t, compose_hangul_LV);
	tcase_add_test(t, compose_hangul_LVT);
	tcase_add_test(t, compose_hangul_LVL);
	tcase_add_test(t, compose_hangul_LVV);
	tcase_add_test(t, compose_no_composition);
	tcase_add_test(t, compose);
	tcase_add_test(t, decompose_invalid_params);
	tcase_add_loop_test(t, decompose_no_decomposition, 0, N_NODECOMP);
	tcase_add_test(t, decompose_hangul_LV);
	tcase_add_test(t, decompose_hangul_LVT);
	tcase_add_test(t, decompose);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

