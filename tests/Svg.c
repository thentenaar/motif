/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <SvgI.h>
#include <check.h>

#include "suites.h"

START_TEST(load_invalid_file)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("svg/invalid_file.svg", "rb"), "Failed to open svg/invalid_file.svg");
	ret = _XmSvgGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(ret == 1 && !img, "Failed to recognize invalid header");
	if (img) XDestroyImage(img);
}
END_TEST

START_TEST(load_test_image)
{
	FILE *fp;
	XImage *img = NULL, *rast = NULL;
	int ret;

	ck_assert_msg(fp = fopen("svg/test.svg", "rb"), "Failed to open svg/test.svg");
	ret = _XmSvgGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load RGB test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(XImageIsSVG(img), "Expected XImageIsSVG");
	XDestroyImage(img);
}
END_TEST

START_TEST(rasterize_image)
{
	FILE *fp;
	XImage *img = NULL, *rast = NULL;
	int ret;

	ck_assert_msg(fp = fopen("svg/test.svg", "rb"), "Failed to open svg/test.svg");
	ret = _XmSvgGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load RGB test image");
	ck_assert_msg(rast = img->f.sub_image(img, 0, 0, img->width, img->height),
	              "Expected to rasterize image");
	ck_assert_msg(rast->depth == 32, "Expected 32-bit rasterized depth");
	ck_assert_msg(rast->width == 4 && rast->height == 1, "Expected 4x1 rasterized image");
	ck_assert_msg(!XImageIsSVG(rast), "Expected !XImageIsSVG");
	ck_assert_msg(rast->red_mask   = (0xff << 16), "Expected rasterized red_mask @ <<16");
	ck_assert_msg(rast->green_mask = (0xff << 8),  "Expected rasterized green_mask @ <<8");
	ck_assert_msg(rast->blue_mask  = 0xff,         "Expected rasterized blue_mask @ 0");
	ck_assert_msg((unsigned char)rast->data[0]  == 0xff, "Expected pixel 1 to be red (a)");
	ck_assert_msg((unsigned char)rast->data[1]  == 0xff, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)rast->data[2]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)rast->data[3]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)rast->data[4]  == 0xff, "Expected pixel 2 to be green (a)");
	ck_assert_msg((unsigned char)rast->data[5]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)rast->data[6]  == 0xff, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)rast->data[7]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)rast->data[8]  == 0xff, "Expected pixel 3 to be blue (a)");
	ck_assert_msg((unsigned char)rast->data[9]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)rast->data[10] == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)rast->data[11] == 0xff, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)rast->data[12] == 0x00, "Expected pixel 4 to be transparent (a)");
	XDestroyImage(rast);
	XDestroyImage(img);
}
END_TEST

void svg_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("SVG");

	t = tcase_create("Load SVG images");
	tcase_add_test(t, load_invalid_file);
	tcase_add_test(t, load_test_image);
	tcase_add_test(t, rasterize_image);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

