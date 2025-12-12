/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
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
#include <setjmp.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <jpeglib.h>
#include <jerror.h>

#include "JpegI.h"

/**
 * Error handling context info
 */
struct jerr {
	struct jpeg_error_mgr jpeg_error;
	jmp_buf jmp;
};

/**
 * Error handling routine
 */
static void on_jpeg_error(j_common_ptr errinfo)
{
	int ret;
	struct jerr *err = (struct jerr *)errinfo->err;

	switch (errinfo->err->msg_code) {
	case JERR_NO_SOI:        ret = 1; break; /* Invalid JPEG: No start of image */
	case JERR_OUT_OF_MEMORY: ret = 5; break;
	default:
		ret = -1;
	}

	longjmp(err->jmp, ret);
}

int _XmJpegGetImage(FILE *fp, XImage **ximage)
{
	int ret = 0;
	XImage *img = NULL;
	int w1, h1;
	unsigned int i, j, w, h;
	unsigned char *data = NULL, *gp;
	struct jerr err;
	struct jpeg_decompress_struct jpeg;
	JSAMPARRAY rows = NULL;

	if (!fp || !ximage)
		return 1;

	/* Setup error handling */
	*ximage = NULL;
	memset(&err, 0, sizeof err);
	jpeg.err = jpeg_std_error(&err.jpeg_error);
	err.jpeg_error.error_exit = on_jpeg_error;

	if ((ret = setjmp(err.jmp))) {
		if (data) XFree(data);
		if (rows) XFree(rows);
		jpeg_destroy_decompress(&jpeg);
		return ret;
	}

	/* Initialize the JPEG struct */
	jpeg_create_decompress(&jpeg);
	jpeg_stdio_src(&jpeg, fp);
	jpeg_read_header(&jpeg, True);
	jpeg_calc_output_dimensions(&jpeg);

	/* Allocate our data buffer */
	w = jpeg.output_width;
	h = jpeg.output_height;
	if (!(data = Xmalloc(w * h * 3))) {
		jpeg_destroy_decompress(&jpeg);
		return 2;
	}

	/* Setup our row pointers */
	if (!(rows = Xmalloc(h * sizeof(*rows)))) {
		XFree(data);
		jpeg_destroy_decompress(&jpeg);
		return 3;
	}

	for (i = 0; i < h; i++)
		rows[i] = data + w * i;

	/* Read scanlines */
	jpeg_start_decompress(&jpeg);
	do {
		jpeg_read_scanlines(&jpeg, rows + jpeg.output_scanline, h - jpeg.output_scanline);
	} while (jpeg.output_scanline < h);

	/* Do grayscale expansion if needed */
	if (jpeg.out_color_space == JCS_GRAYSCALE) {
		gp = data + 3 * w * h;
		for (h1 = (h - 1) * h; h1 >= 0; h1 -= h) {
			for (w1 = w - 1; w1 >= 0; --w1) {
				*(--gp) = data[h1 + w1];
				*(--gp) = data[h1 + w1];
				*(--gp) = data[h1 + w1];
			}
		}
	}

	jpeg_finish_decompress(&jpeg);
	jpeg_destroy_decompress(&jpeg);
	XFree(rows);

	/* Create our XImage */
	if (!(img = Xmalloc(sizeof *img))) {
		XFree(data);
		return 4;
	}

	img->data             = (char *)data;
	img->obdata           = NULL;
	img->width            = w;
	img->height           = h;
	img->xoffset          = 0;
	img->depth            = 24;
	img->format           = ZPixmap;
	img->byte_order       = MSBFirst;
	img->red_mask         = 0xff0000;
	img->green_mask       = 0x00ff00;
	img->blue_mask        = 0x0000ff;
	img->bitmap_unit      = 8;
	img->bitmap_bit_order = MSBFirst;
	img->bitmap_pad       = 32;
	img->bits_per_pixel   = 24;
	img->bytes_per_line   = w * 3;
	XInitImage(img);

	*ximage = img;
	return 0;
}

