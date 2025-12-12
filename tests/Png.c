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
#include <X11/Xlib.h>
#include <PngI.h>
#include <check.h>

#include "suites.h"

START_TEST(load_invalid_header)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/invalid_header.png", "rb"), "Failed to open png/invalid_header.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(ret == 1 && !img, "Failed to recognize invalid header");
	if (img) XDestroyImage(img);
}
END_TEST

START_TEST(load_rgb24)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/rgb24.png", "rb"), "Failed to open png/rgb24.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 24-bit RGB test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0xff, "Expected pixel 4 to be black (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0x00, "Expected pixel 4 to be black (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0x00, "Expected pixel 4 to be black (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0x00, "Expected pixel 4 to be black (b)");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_rgba32)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/rgba32.png", "rb"), "Failed to open png/rgba32.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 32-bit RGBA test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be opaque red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be opaque red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be opaque red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be opaque red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be opaque green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be opaque green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be opaque green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be opaque green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be opaque blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be opaque blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be opaque blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be opaque blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0x00, "Expected pixel 4 to be transparent (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0x00, "Expected pixel 4 to be transparent (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0x00, "Expected pixel 4 to be transparent (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0x00, "Expected pixel 4 to be transparent (b)");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_rgba32_bgcolor)
{
	FILE *fp;
	XImage *img = NULL;
	XColor bg;
	int ret;

	bg.red   = 0xcc;
	bg.green = 0xcc;
	bg.blue  = 0xcc;

	ck_assert_msg(fp = fopen("png/rgba32.png", "rb"), "Failed to open png/rgba32.png");
	ret = _XmPngGetImage(fp, &bg, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 32-bit RGBA test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0xff, "Expected pixel 4 to be bg (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0xcc, "Expected pixel 4 to be bg (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0xcc, "Expected pixel 4 to be bg (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0xcc, "Expected pixel 4 to be bg (b)");
	XDestroyImage(img);
}
END_TEST

void png_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("Png");

	t = tcase_create("Load PNG images");
	tcase_add_test(t, load_invalid_header);
	tcase_add_test(t, load_rgb24);
	tcase_add_test(t, load_rgba32);
	tcase_add_test(t, load_rgba32_bgcolor);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

