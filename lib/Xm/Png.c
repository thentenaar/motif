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
#include <stdlib.h>
#include <X11/Xlibint.h>
#include <png.h>

#include "PngI.h"

/**
 * Load a PNG image from a file, returning 0 on success and yielding a
 * 32-bit ARGB ZPixmap. If the background color is specified, it is
 * blended in automatically, and the resulting image is fully opaque.
 */
int _XmPngGetImage(FILE *fp, XColor *bg, XImage **ximage)
{
	long pos;
	XImage *img = NULL;
	png_color_16 bgcolor;
	png_uint_32 w, h, i;
	png_structp pngp = NULL;
	png_infop infop  = NULL;
	png_bytep *rows  = NULL;
	unsigned char header[8], *data = NULL;

	if (!ximage || !fp)
		return 1;

	rewind(fp);
	if (fread(header, 1, sizeof header, fp) != sizeof header ||
        png_sig_cmp(header, 0, sizeof header))
		return 1;

	*ximage = NULL;
	if (!(pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) ||
	    !(infop = png_create_info_struct(pngp))) {
		if (pngp) png_destroy_read_struct(&pngp, NULL, NULL);
		return 2;
	}

	if (setjmp(png_jmpbuf(pngp))) {
		free(data);
		free(rows);
		if (img) XFree(img);
		*ximage = NULL;
		png_destroy_read_struct(&pngp, &infop, NULL);
		return 3;
	}

	pos = ftell(fp);
	png_init_io(pngp, fp);
	png_set_sig_bytes(pngp, sizeof header);

	/* Peek at the width / height and reinitialize */
	png_read_info(pngp, infop);
	png_get_IHDR(pngp, infop, &w, &h, NULL, NULL, NULL, NULL, NULL);
	png_destroy_read_struct(&pngp, &infop, NULL);
	if (!(pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) ||
	    !(infop = png_create_info_struct(pngp))) {
		if (pngp) png_destroy_read_struct(&pngp, NULL, NULL);
		return 4;
	}
	fseek(fp, pos, SEEK_SET);
	png_init_io(pngp, fp);
	png_set_sig_bytes(pngp, sizeof header);

	/**
	 * Allocate the row pointers, so that the [A]RGB pixels are nice and
	 * contiguous in our buffer.
	 */
	if (!(data = Xmalloc(w * h * 4))) {
		png_destroy_read_struct(&pngp, &infop, NULL);
		return 5;
	}

	if (!(rows = malloc(h * sizeof(*rows)))) {
		XFree(data);
		png_destroy_read_struct(&pngp, &infop, NULL);
		return 6;
	}

	for (i = 0; i < h; i++)
		rows[i] = data + i * w * 4;

	png_set_rows(pngp, infop, rows);
	png_set_palette_to_rgb(pngp);
	png_set_filler(pngp, 0xff, PNG_FILLER_BEFORE);

	/* If we've specified a background color, use that in lieu of alpha */
	if (bg) {
		bgcolor.red   = bg->red   * 257;
		bgcolor.green = bg->green * 257;
		bgcolor.blue  = bg->blue  * 257;
		png_set_background(pngp, &bgcolor, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1);
	} else png_set_alpha_mode(pngp, PNG_ALPHA_STANDARD, PNG_DEFAULT_sRGB);

	png_read_png(pngp, infop,
	             PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_GRAY_TO_RGB |
	             PNG_TRANSFORM_SCALE_16   | PNG_TRANSFORM_EXPAND, NULL);
	png_destroy_read_struct(&pngp, &infop, NULL);
	free(rows);

	/* Create our XImage */
	if (!(img = Xmalloc(sizeof *img))) {
		XFree(data);
		return 7;
	}

	img->data             = (char *)data;
	img->obdata           = NULL;
	img->width            = w;
	img->height           = h;
	img->xoffset          = 0;
	img->depth            = 32;
	img->format           = ZPixmap;
	img->byte_order       = MSBFirst;
	img->red_mask         = 0x00ff0000;
	img->green_mask       = 0x0000ff00;
	img->blue_mask        = 0x000000ff;
	img->bitmap_unit      = 8;
	img->bitmap_bit_order = MSBFirst;
	img->bitmap_pad       = 32;
	img->bits_per_pixel   = 32;
	img->bytes_per_line   = w * 4;
	XInitImage(img);

	*ximage = img;
	return 0;
}

