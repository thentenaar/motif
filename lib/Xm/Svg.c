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
#include <stdlib.h>
#include <errno.h>

#include <X11/Xlibint.h>
#include "SvgI.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

/* same as xlib's _XDestroyImage */
static int destroy(XImage *image)
{
	nsvgDelete((NSVGimage *)image->obdata);
	return 1;
}

/**
 * sub_image: Rasterize the svg to the target size
 */
static XImage *rasterize(XImage *src, int x, int y, unsigned int w, unsigned int h)
{
	XImage *img;
	unsigned char *data;
	float scale, x_scale = 1.0, y_scale = 1.0;
	NSVGrasterizer *rast;

	(void)x;
	(void)y;
	if (!w || !h || !(rast = nsvgCreateRasterizer()))
		return NULL;

	if (!(data = Xcalloc(4, w * h))) {
		nsvgDeleteRasterizer(rast);
		return NULL;
	}

	x_scale = w / (float)src->width;
	y_scale = h / (float)src->height;
	scale   = (x_scale < y_scale) ? x_scale : y_scale;
	nsvgRasterize(rast, (NSVGimage *)src->obdata, x * scale, y * scale,
	              scale, data, w, h, w * 4);
	nsvgDeleteRasterizer(rast);

	if (!(img = Xcalloc(1, sizeof *img))) {
		XFree(data);
		return NULL;
	}

	img->data             = (char *)data;
	img->obdata           = NULL;
	img->width            = w;
	img->height           = h;
	img->xoffset          = 0;
	img->format           = ZPixmap;
	img->byte_order       = MSBFirst;
	img->red_mask         = 0x00ff0000;
	img->green_mask       = 0x0000ff00;
	img->blue_mask        = 0x000000ff;
	img->bitmap_unit      = 8;
	img->bitmap_bit_order = MSBFirst;
	img->bitmap_pad       = 32;
	img->bits_per_pixel   = 32;
	img->depth            = 32;
	XInitImage(img);
	return img;
}

static unsigned long get_pixel(XImage *img, int x, int y)
{
	(void)img;
	(void)x;
	(void)y;
	return 0;
}

static int put_pixel(XImage *img, int x, int y, unsigned long pixel)
{
	(void)img;
	(void)x;
	(void)y;
	(void)pixel;
	return 1;
}

static int add_pixel(XImage *img, long pixel)
{
	(void)img;
	(void)pixel;
	return 1;
}

int _XmSvgGetImage(FILE *fp, XImage **out)
{
	char *data;
	long size;
	XImage *img;
	NSVGimage *svg;

	*out = NULL;
	errno = 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	if (errno || !(data = Xmalloc((size_t)size + 1)))
		return 1;

	if (!fread(data, 1, (size_t)size, fp)) {
		XFree(data);
		return 1;
	}

	data[size] = '\0';
	svg = nsvgParse(data, "px", 96);
	XFree(data);

	if (!svg)
		return 1;

	if (!svg->width || !svg->height) {
		nsvgDelete(svg);
		return 1;
	}

	/* Create our XImage */
	if (!(img = Xmalloc(sizeof *img))) {
		nsvgDelete(svg);
		return 2;
	}

	img->data             = NULL;
	img->obdata           = (XPointer)svg;
	img->width            = (int)svg->width;
	img->height           = (int)svg->height;
	img->xoffset          = 0;
	img->depth            = 32;
	img->format           = ZPixmap;
	img->byte_order       = MSBFirst;
	img->red_mask         = 0x00000000;
	img->green_mask       = 0x00000000;
	img->blue_mask        = 0x00000000;
	img->bitmap_unit      = 8;
	img->bitmap_bit_order = MSBFirst;
	img->bitmap_pad       = 32;
	img->bits_per_pixel   = 32;
	img->bytes_per_line   = 0;
	img->f.create_image   = NULL;
	img->f.sub_image      = rasterize;
	img->f.destroy_image  = destroy;
	img->f.get_pixel      = get_pixel;
	img->f.put_pixel      = put_pixel;
	img->f.add_pixel      = add_pixel;
	*out = img;
	return 0;
}

