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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>

#include <Xm/ScreenP.h>
#include "SvgI.h"
#include "XmI.h"
#include "XmosI.h"
#include "Cursor.h"

#if XM_WITH_PNG
#include "PngI.h"
#endif

#if XM_WITH_XCURSOR
#include <X11/Xcursor/Xcursor.h>
#endif

static const char null_bits[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

#if XM_WITH_XRENDER
#include <X11/extensions/Xrender.h>

/**
 * Render an ARGB image (SVG or PNG) to the specified size
 */
static Pixmap render_cursor(Display *display, Screen *screen, const char *filename,
                            int size, int *hot_x, int *hot_y)
{
	GC gc;
	FILE *fp;
	int x, y, ox, oy;
	unsigned char *new_data;
	XImage *img = NULL, *old;
	Pixmap p = XmUNSPECIFIED_PIXMAP;

	if (!(fp = fopen(filename, "rb")))
		return p;

	if (_XmSvgGetImage(fp, &img)) {
#if XM_WITH_PNG
		if (_XmPngGetImage(fp, NULL, &img))
#endif
			goto done;
	}

	/* Rasterize */
	if (XImageIsSVG(img)) {
		old = img;
		img = img->f.sub_image(img, 0, 0, size, size);
		XDestroyImage(old);
	} else {
#if !XM_WITH_PNG
		goto done;
#else
		/* Scale the PNG to size */
		new_data = (unsigned char *)XtCalloc(size * size, sizeof(img->bits_per_pixel >> 3));

		for (y = 0; y < size; y++) {
			for (x = 0; x < size; x++) {
				ox = (int)(x / (double)size * img->width);
				oy = (int)(y / (double)size * img->height);

				/* Anything out of bounds is transparent */
				if (ox > img->width || oy > img->height)
					continue;

				/* Copy the target pixel */
				memcpy(new_data  + (y * size + x)          * (img->bits_per_pixel >> 3),
				       img->data + (oy * img->height + ox) * (img->bits_per_pixel >> 3),
				       img->bits_per_pixel >> 3);
			}
		}

		XFree(img->data);
		img->data           = (char *)new_data;
		img->width          = size;
		img->height         = size;
		img->bytes_per_line = size * (img->bits_per_pixel >> 3);
#endif /* XM_WITH_PNG */
	}

	*hot_x = (int)(*hot_x / (double)size * img->width);
	*hot_y = (int)(*hot_y / (double)size * img->height);
	p  = XCreatePixmap(display, RootWindowOfScreen(screen), size, size, img->depth);
	gc = XCreateGC(display, p, 0, NULL);
	XPutImage(display, p, gc, img, 0, 0, 0, 0, size, size);
	XFreeGC(display, gc);

done:
	if (img) XDestroyImage(img);
	fclose(fp);
	return p;
}
#endif /* XM_WITH_XRENDER */

/**
 * Load a cursor given a filename (i.e., file.svg -- or simply file)
 * which will be loaded as a cursor via Xrender if such file exists
 * and we have an ARGB visual. The cursor will be scaled in the same
 * manner as Xcursor cursors. SVG and PNG images are supported.
 *
 * The hot_x and hot_y parameters indicate the hot coordinate of the
 * cursor in its original size.
 *
 * Returns None if the image cannot be found or loaded, or we don't have
 * a 32-bit TrueColor visual.
 *
 * This can be used in a XmRCursor resource by specifying "name:hot_x,hot_y"
 * as the string to convert from.
 */
Cursor XmeLoadCursorImage(Display *display, Screen *screen, const char *name,
                          int hot_x, int hot_y)
{
	Cursor c = None;

#if XM_WITH_XRENDER
	const char *def, *filename = NULL;
	int i,j,size = 0;
	Pixmap p;
	XVisualInfo vis;
	String path;
	XmScreen s;
	Picture pic;
	XRenderPictFormat *format;
	SubstitutionRec pathsub;

	_XmDisplayToAppContext(display);
	_XmAppLock(app);

	/* See if we ain't got a 32-bit TrueColor visual */
	if (!XMatchVisualInfo(display, XScreenNumberOfScreen(screen), 32, TrueColor, &vis)) {
		_XmAppUnlock(app);
		return None;
	}

	/* Our default paths expect %P */
	pathsub.match = 'P';
	pathsub.substitution = name;
	path = _XmOSInitPath((String)name, (String)"XMCURSORSEARCHPATH", NULL);

	if (!filename) {
		filename = XtResolvePathname(display, "cursors", NULL, ".svg",
		                             path, &pathsub, 1, NULL);
	}

#if XM_WITH_PNG
	if (!filename) {
		filename = XtResolvePathname(display, "cursors", NULL, ".png",
		                             path, &pathsub, 1, NULL);
	}
#endif

	XtFree(path);
	if (!filename) {
		_XmAppUnlock(app);
		return None;
	}

	/* Treat Xcursor.size just like we do Xft.dpi */
	s = XmScreenOfScreen(screen);
	if ((def = getenv("XCURSOR_SIZE")))
		size = atoi(def);
	if (!size && (def = XGetDefault(display, "Xcursor", "size")))
		size = atoi(def);
	if (!size) /* Use the same ratio as Xcursor */
		size = (int)(XmScreenDpi(s) * 16. / 72.);

	p = render_cursor(display, screen, filename, size, &hot_x, &hot_y);
	XtFree((char *)filename);
	if (p == XmUNSPECIFIED_PIXMAP) {
		_XmAppUnlock(app);
		return None;
	}

	/* XRender 0.5+ ftw */
	if (XRenderQueryExtension(display, &i, &j) && (i * 100 + j) >= 5) {
		format  = XRenderFindStandardFormat(display, PictStandardARGB32);
		pic     = XRenderCreatePicture(display, p, format, 0, NULL);
		c       = XRenderCreateCursor(display, pic, hot_x, hot_y);
		XRenderFreePicture(display, pic);
	}

	XFreePixmap(display, p);
	_XmAppUnlock(app);
#endif /* XM_WITH_XRENDER */

	return c;
}

#if XM_WITH_XCURSOR
/**
 * For just weeding out things we know aren't Xcursor files,
 * checking for XCURSOR_MAGIC is enough.
 */
static Boolean test_xcursor_file(String path)
{
	Boolean ok = False;
	char header[4];
	FILE *fp = NULL;

	if (path && (fp = fopen(path, "r"))) {
		if (fread(header, 1, 4, fp) == 4)
			ok = !memcmp(header, "Xcur", 4);
	}

	if (fp) fclose(fp);
	return ok;
}

/**
 * Custom Xcursor file
 */
static Cursor cursor_xcursor_file(Display *display, const char *name)
{
	String path;
	char *filename;
	Cursor c = None;
	SubstitutionRec pathsub;

	/* Our default paths expect %P */
	pathsub.match = 'P';
	pathsub.substitution = (char *)name;

	path     = _XmOSInitPath((char *)name, "XMCURSORSEARCHPATH", NULL);
	filename = XtResolvePathname(display, "cursors", NULL, NULL,
	                             path, &pathsub, 1,
	                             (XtFilePredicate)test_xcursor_file);

	if (filename)
		c = XcursorFilenameLoadCursor(display, filename);

	XtFree(path);
	XtFree(filename);
	return c;
}
#endif /* XM_WITH_XCURSOR */

/**
 * Good ol' cursorfont
 */
static Cursor cursor_font(Display *display, const char *name)
{
	Cardinal i;

	/* "Borrowed" from Xt */
	static const struct _CursorName {
		const char      *name;
		unsigned int    shape;
	} cursor_names[] = {
		{"X_cursor",            XC_X_cursor},
		{"arrow",               XC_arrow},
		{"based_arrow_down",    XC_based_arrow_down},
		{"based_arrow_up",      XC_based_arrow_up},
		{"boat",                XC_boat},
		{"bogosity",            XC_bogosity},
		{"bottom_left_corner",  XC_bottom_left_corner},
		{"bottom_right_corner", XC_bottom_right_corner},
		{"bottom_side",         XC_bottom_side},
		{"bottom_tee",          XC_bottom_tee},
		{"box_spiral",          XC_box_spiral},
		{"center_ptr",          XC_center_ptr},
		{"circle",              XC_circle},
		{"clock",               XC_clock},
		{"coffee_mug",          XC_coffee_mug},
		{"cross",               XC_cross},
		{"cross_reverse",       XC_cross_reverse},
		{"crosshair",           XC_crosshair},
		{"diamond_cross",       XC_diamond_cross},
		{"dot",                 XC_dot},
		{"dotbox",              XC_dotbox},
		{"double_arrow",        XC_double_arrow},
		{"draft_large",         XC_draft_large},
		{"draft_small",         XC_draft_small},
		{"draped_box",          XC_draped_box},
		{"exchange",            XC_exchange},
		{"fleur",               XC_fleur},
		{"gobbler",             XC_gobbler},
		{"gumby",               XC_gumby},
		{"hand1",               XC_hand1},
		{"hand2",               XC_hand2},
		{"heart",               XC_heart},
		{"icon",                XC_icon},
		{"iron_cross",          XC_iron_cross},
		{"left_ptr",            XC_left_ptr},
		{"left_side",           XC_left_side},
		{"left_tee",            XC_left_tee},
		{"leftbutton",          XC_leftbutton},
		{"ll_angle",            XC_ll_angle},
		{"lr_angle",            XC_lr_angle},
		{"man",                 XC_man},
		{"middlebutton",        XC_middlebutton},
		{"mouse",               XC_mouse},
		{"pencil",              XC_pencil},
		{"pirate",              XC_pirate},
		{"plus",                XC_plus},
		{"question_arrow",      XC_question_arrow},
		{"right_ptr",           XC_right_ptr},
		{"right_side",          XC_right_side},
		{"right_tee",           XC_right_tee},
		{"rightbutton",         XC_rightbutton},
		{"rtl_logo",            XC_rtl_logo},
		{"sailboat",            XC_sailboat},
		{"sb_down_arrow",       XC_sb_down_arrow},
		{"sb_h_double_arrow",   XC_sb_h_double_arrow},
		{"sb_left_arrow",       XC_sb_left_arrow},
		{"sb_right_arrow",      XC_sb_right_arrow},
		{"sb_up_arrow",         XC_sb_up_arrow},
		{"sb_v_double_arrow",   XC_sb_v_double_arrow},
		{"shuttle",             XC_shuttle},
		{"sizing",              XC_sizing},
		{"spider",              XC_spider},
		{"spraycan",            XC_spraycan},
		{"star",                XC_star},
		{"target",              XC_target},
		{"tcross",              XC_tcross},
		{"top_left_arrow",      XC_top_left_arrow},
		{"top_left_corner",     XC_top_left_corner},
		{"top_right_corner",    XC_top_right_corner},
		{"top_side",            XC_top_side},
		{"top_tee",             XC_top_tee},
		{"trek",                XC_trek},
		{"ul_angle",            XC_ul_angle},
		{"umbrella",            XC_umbrella},
		{"ur_angle",            XC_ur_angle},
		{"watch",               XC_watch},
		{"xterm",               XC_xterm},
	};

	for (i = 0; i < XtNumber(cursor_names); i++)
		if (!strcmp(cursor_names[i].name, name))
			break;

	if (i < XtNumber(cursor_names))
		return XCreateFontCursor(display, cursor_names[i].shape);
	return None;
}

/**
 * Load a cursor given a name, which can be a cursor name (e.g., "arrow")
 * or a Xcursor file (if Xcursor is supported.)
 */
Cursor XmeLoadCursor(Display *display, Screen *screen, const char *name)
{
	Cursor c = None;
	_XmDisplayToAppContext(display);
	_XmAppLock(app);

#if XM_WITH_XCURSOR
	if (c == None)
		c = cursor_xcursor_file(display, name);

	if (c == None)
		c = XcursorLibraryLoadCursor(display, name);
#endif

	if (c == None)
		c = cursor_font(display, name);

	_XmAppUnlock(app);
	return c;
}

/************************************************************************
 *
 *  XmeQueryBestCursorSize
 *
 ************************************************************************/
void XmeQueryBestCursorSize(Widget w, Dimension *width, Dimension *height)
{
	XmScreen scr;
	_XmWidgetToAppContext(w);
	_XmAppLock(app);
	scr     = XmScreenOfObject(w);
	*width  = (Dimension)scr->screen.maxCursorWidth;
	*height = (Dimension)scr->screen.maxCursorHeight;
	_XmAppUnlock(app);
}

/************************************************************************
 *
 *  XmeGetNullCursor
 *
 ************************************************************************/
Cursor XmeGetNullCursor(Widget w)
{
	XmScreen scr;
	Pixmap p;
	Cursor c;
	XColor fg, bg;

	_XmWidgetToAppContext(w);
	_XmAppLock(app);
	scr = XmScreenOfObject(w);
	if (scr->screen.nullCursor) {
		c = scr->screen.nullCursor;
		_XmAppUnlock(app);
		return c;
	}

	fg.pixel = bg.pixel = 0;
	p = XCreatePixmapFromBitmapData(XtDisplayOfObject(w),
	                                RootWindowOfScreen(XtScreenOfObject(w)),
	                                (char *)null_bits, 4, 4, 0, 0, 1);
	c = XCreatePixmapCursor(XtDisplayOfObject(w), p, p, &fg, &bg, 0, 0);
	XFreePixmap(XtDisplayOfObject(w), p);
	scr->screen.nullCursor = c;
	_XmAppUnlock(app);
	return c;
}

