#include <stdio.h>
#include <stdlib.h>

#include <png.h>
#include "PngI.h"

static double get_display_exponent(void);
static int png_process_image(Screen *screen,
                                XImage *ximage, int image_rowbytes,
                                int image_channels, unsigned char bg_red,
                                unsigned char bg_green,
                                unsigned char bg_blue,
                                unsigned char *image_data);
static int png_msb(unsigned long u32val);
static int png_load_file(FILE *infile, unsigned long *pWidth,
                         unsigned long *pHeight, unsigned char *red,
                         unsigned char *green, unsigned char *blue,
                         int *pChannels, unsigned long *pRowbytes,
                         unsigned char **image_data);

int
_XmPngGetImage(Screen *screen, FILE *infile, XColor *bg,
               XImage **ximage)
{
    int image_channels;
    unsigned char *image_data = NULL;
    unsigned long image_width, image_height, image_rowbytes;
    unsigned char *xdata;
    int pad;
    int rc;

    rc = png_load_file(infile,
                       &image_width, &image_height,
                       NULL, NULL, NULL,
                       &image_channels, &image_rowbytes, &image_data);
/* XXX if background is XmINSPECIFIED_PIXEL, we cat try
       to get background from the PNG image, but I can
       doesn't have this information.
        rc = readpng_load_file(infile,
	    &image_width, &image_height,
	    &bg_red, &bg_green, &bg_blue,
	    &image_channels, &image_rowbytes, &image_data);
*/
    if (rc) return rc;

    if (screen->root_depth == 24 || screen->root_depth == 32) {
        xdata = (unsigned char *) malloc(4 * image_width * image_height);
        pad = 32;
    } else if (screen->root_depth == 16) {
        xdata = (unsigned char *) malloc(2 * image_width * image_height);
        pad = 16;
    } else { /* depth == 8 */
        xdata = (unsigned char *) malloc(image_width * image_height);
        pad = 8;
    }

    if (!xdata)
        return 4;

    *ximage =
        XCreateImage(screen->display, screen->root_visual,
                     screen->root_depth, ZPixmap, 0, (char *) xdata,
                     image_width, image_height, pad, 0);

    if (!*ximage) {
        free(xdata);
        return 4;
    }

    (*ximage)->byte_order = MSBFirst;

    rc = png_process_image(screen, *ximage, image_rowbytes,
                              image_channels, bg->red, bg->green, bg->blue,
                              image_data);

    if (image_data) {
        free(image_data);
        image_data = NULL;
    }

    return rc;
}

static double
get_display_exponent(void)
{
    double LUT_exponent;        /* just the lookup table */
    double CRT_exponent = 2.2;  /* just the monitor */
    double default_display_exponent;    /* whole display system */
    char *p;
    double display_exponent;

    /* First set the default value for our display-system exponent, i.e.,
     * the product of the CRT exponent and the exponent corresponding to
     * the frame-buffer's lookup table (LUT), if any.  This is not an
     * exhaustive list of LUT values (e.g., OpenStep has a lot of weird
     * ones), but it should cover 99% of the current possibilities. */

    LUT_exponent = 1.0;         /* assume no LUT:  most PCs */

    /* the defaults above give 1.0, 1.3, 1.5 and 2.2, respectively: */
    default_display_exponent = LUT_exponent * CRT_exponent;


    /* If the user has set the SCREEN_GAMMA environment variable as suggested
     * (somewhat imprecisely) in the libpng documentation, use that; otherwise
     * use the default value we just calculated.  Either way, the user may
     * override this via a command-line option. */

    if ((p = getenv("SCREEN_GAMMA")) != NULL)
        display_exponent = atof(p);
    else
        display_exponent = default_display_exponent;

    return display_exponent;
}

/* return value = 0 for success, 1 for bad sig, 2 for bad struct (IHDR or kBGD),
   4 for no mem 5 if fails due to no bKGD chunk */

static int
png_load_file(FILE *infile, unsigned long *pWidth,
              unsigned long *pHeight, unsigned char *red,
              unsigned char *green, unsigned char *blue,
              int *pChannels, unsigned long *pRowbytes,
              unsigned char **image_data)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    int bit_depth, color_type;
    png_uint_32 width, height;
//    int rc;
    unsigned char sig[8];
    png_color_16p pBackground;
    double gamma;
    png_uint_32 i, rowbytes;
    png_bytepp row_pointers = NULL;

    fread(sig, 1, 8, infile);
    if (png_sig_cmp(sig, 0, 8))
        return 1;               /* bad signature */

    png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;               /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;               /* out of memory */
    }

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);      /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);   /* read all PNG info up to image data */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                 &color_type, NULL, NULL, NULL);
    if (pWidth)
        *pWidth = width;
    if (pHeight)
        *pHeight = height;

    if (red && green && blue) {
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD)) {

            png_get_bKGD(png_ptr, info_ptr, &pBackground);

            if (bit_depth == 16) {
                *red = pBackground->red >> 8;
                *green = pBackground->green >> 8;
                *blue = pBackground->blue >> 8;
            } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
                if (bit_depth == 1)
                    *red = *green = *blue = pBackground->gray ? 255 : 0;
                else if (bit_depth == 2)
                    *red = *green = *blue = (255 / 3) * pBackground->gray;
                else            /* bit_depth == 4 */
                    *red = *green = *blue = (255 / 15) * pBackground->gray;
            } else {
                *red = (unsigned char) pBackground->red;
                *green = (unsigned char) pBackground->green;
                *blue = (unsigned char) pBackground->blue;
            }
        }
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 16)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, get_display_exponent(), gamma);

    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *pChannels = (int) png_get_channels(png_ptr, info_ptr);

    if ((*image_data =
         (unsigned char *) malloc(rowbytes * height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 4;
    }
    if ((row_pointers =
         (png_bytepp) malloc(height * sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(*image_data);
        *image_data = NULL;
        return 4;
    }

    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0; i < height; ++i)
        row_pointers[i] = *image_data + i * rowbytes;

    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);


    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    free(row_pointers);

    png_read_end(png_ptr, NULL);

    if (png_ptr && info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }

    return 0;
}

static int
png_process_image(Screen *screen,
                     XImage *ximage, int image_rowbytes,
                     int image_channels,
                     unsigned char bg_red,
                     unsigned char bg_green,
                     unsigned char bg_blue, unsigned char *image_data)
{
    unsigned char *src;
    char *dest;
    unsigned char r, g, b, a;
    unsigned long i, row, lastrow = 0;
    unsigned long pixel;
    int ximage_rowbytes = ximage->bytes_per_line;
    static int RShift, GShift, BShift;
    static unsigned long RMask, GMask, BMask;
    unsigned short red, green, blue;

    RMask = screen->root_visual->red_mask;
    GMask = screen->root_visual->green_mask;
    BMask = screen->root_visual->blue_mask;
    if (screen->root_depth == 15 || screen->root_depth == 16) {
        RShift = 15 - png_msb(RMask);
        GShift = 15 - png_msb(GMask);
        BShift = 15 - png_msb(BMask);
    } else if (screen->root_depth > 16) {
        RShift = png_msb(RMask) - 7;
        GShift = png_msb(GMask) - 7;
        BShift = png_msb(BMask) - 7;
    }

    if (screen->root_depth == 24 || screen->root_depth == 32) {
        for (lastrow = row = 0; row < ximage->height; ++row) {
            src = image_data + row * image_rowbytes;
            dest = ximage->data + row * ximage_rowbytes;
            if (image_channels == 3) {
                for (i = ximage->width; i > 0; --i) {
                    red = *src++;
                    green = *src++;
                    blue = *src++;
#ifdef NO_24BIT_MASKS
                    pixel = (red << RShift) |
                        (green << GShift) | (blue << BShift);
                    /* recall that we set ximage->byte_order = MSBFirst above */
                    /* GRR BUG:  this assumes bpp == 32, but may be 24: */
                    *dest++ = (char) ((pixel >> 24) & 0xff);
                    *dest++ = (char) ((pixel >> 16) & 0xff);
                    *dest++ = (char) ((pixel >> 8) & 0xff);
                    *dest++ = (char) (pixel & 0xff);
#else
                    red = (RShift < 0) ? red << (-RShift) : red >> RShift;
                    green =
                        (GShift <
                         0) ? green << (-GShift) : green >> GShift;
                    blue =
                        (BShift < 0) ? blue << (-BShift) : blue >> BShift;
                    pixel =
                        (red & RMask) | (green & GMask) | (blue & BMask);
                    /* recall that we set ximage->byte_order = MSBFirst above */
                    *dest++ = (char) ((pixel >> 24) & 0xff);
                    *dest++ = (char) ((pixel >> 16) & 0xff);
                    *dest++ = (char) ((pixel >> 8) & 0xff);
                    *dest++ = (char) (pixel & 0xff);
#endif
                }
            } else {            /* if (image_channels == 4) */
                for (i = ximage->width; i > 0; --i) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
                    if (a == 255) {
                        red = r;
                        green = g;
                        blue = b;
                    } else if (a == 0) {
                        red = bg_red;
                        green = bg_green;
                        blue = bg_blue;
                    } else {
                        /* this macro (from png.h) composites the foreground
                         * and background values and puts the result into the
                         * first argument */
                        png_composite(red, r, a, bg_red);
                        png_composite(green, g, a, bg_green);
                        png_composite(blue, b, a, bg_blue);
                    }
                    pixel = (red << RShift) |
                        (green << GShift) | (blue << BShift);
                    /* recall that we set ximage->byte_order = MSBFirst above */
                    *dest++ = (char) ((pixel >> 24) & 0xff);
                    *dest++ = (char) ((pixel >> 16) & 0xff);
                    *dest++ = (char) ((pixel >> 8) & 0xff);
                    *dest++ = (char) (pixel & 0xff);
                }
            }
        }
    } else if (screen->root_depth == 16) {
        for (lastrow = row = 0; row < ximage->height; ++row) {
            src = image_data + row * image_rowbytes;
            dest = ximage->data + row * ximage_rowbytes;
            if (image_channels == 3) {
                for (i = ximage->width; i > 0; --i) {
                    red = ((unsigned short) (*src) << 8);
                    ++src;
                    green = ((unsigned short) (*src) << 8);
                    ++src;
                    blue = ((unsigned short) (*src) << 8);
                    ++src;
                    pixel = ((red >> RShift) & RMask) |
                        ((green >> GShift) & GMask) |
                        ((blue >> BShift) & BMask);
                    /* recall that we set ximage->byte_order = MSBFirst above */
                    *dest++ = (char) ((pixel >> 8) & 0xff);
                    *dest++ = (char) (pixel & 0xff);
                }
            } else {            /* if (image_channels == 4) */
                for (i = ximage->width; i > 0; --i) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
                    if (a == 255) {
                        red = ((unsigned short) r << 8);
                        green = ((unsigned short) g << 8);
                        blue = ((unsigned short) b << 8);
                    } else if (a == 0) {
                        red = ((unsigned short) bg_red << 8);
                        green = ((unsigned short) bg_green << 8);
                        blue = ((unsigned short) bg_blue << 8);
                    } else {
                        /* this macro (from png.h) composites the foreground
                         * and background values and puts the result back into
                         * the first argument (== fg byte here:  safe) */
                        png_composite(r, r, a, bg_red);
                        png_composite(g, g, a, bg_green);
                        png_composite(b, b, a, bg_blue);
                        red = ((unsigned short) r << 8);
                        green = ((unsigned short) g << 8);
                        blue = ((unsigned short) b << 8);
                    }
                    pixel = ((red >> RShift) & RMask) |
                        ((green >> GShift) & GMask) |
                        ((blue >> BShift) & BMask);
                    /* recall that we set ximage->byte_order = MSBFirst above */
                    *dest++ = (char) ((pixel >> 8) & 0xff);
                    *dest++ = (char) (pixel & 0xff);
                }
            }
        }
    } else { /* depth == 8 */
        /* XXX:  add 8-bit support */
    }

    return 0;
}

static int
png_msb(unsigned long u32val)
{
    int i;

    for (i = 31; i >= 0; --i) {
        if (u32val & 0x80000000L)
            break;
        u32val <<= 1;
    }
    return i;
}

