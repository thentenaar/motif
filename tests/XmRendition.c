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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "XmI.h"
#include "ScreenI.h"
#include "SharedPtrI.h"
#include "XmRenderTI.h"
#include <check.h>

#include "suites.h"

static Display *display;

/* X font props */
static String x_font    = NULL;
static String x_foundry = NULL;
static String x_family  = NULL;
static int x_weight     = 0;
static int x_slant      = 0;
static int x_pointsz    = 0;
static int x_pixelsz    = 0;
static int x_width      = 0;
static int x_inkwidth   = 0;
static int x_ascent     = 0;
static int x_descent    = 0;

/* Xft font props */
static String xft_font    = NULL;
static String xft_foundry = NULL;
static String xft_family  = NULL;
static int xft_weight     = 0;
static int xft_slant      = 0;
static int xft_pointsz    = 0;
static int xft_pixelsz    = 0;
static int xft_width      = 0;
static int xft_ascent     = 0;
static int xft_descent    = 0;

/**
 * Ignore X errors, in case of BadAtom
 */
static int ignore_x_errors(Display *disp, XErrorEvent *event)
{
	(void)disp;
	(void)event;
    return 0;
}

static void _init_xt(void)
{
	int count, i;
	double d;
	char **names         = NULL;
	XFontStruct *finfo   = NULL;
	XErrorHandler olderr = NULL;
	String slant = NULL, weight = NULL;
	Atom FOUNDRY, WEIGHT_NAME, SLANT;
#if USE_XFT
	FcPattern *p, *p2;
	FcObjectSet *os;
	FcFontSet *fs;
	FcChar8 *s;
	FcResult res;
	XftFont *f;
#endif

	setenv("LANG", "C", 1);
	display = XtDisplay(init_xt("check_XmRendition"));
	d = DpiOfXmScreen(XmScreenOfScreen(DefaultScreenOfDisplay(display)));

	/**
	 * Probe for a usable X font and load its props
	 */
	names = XListFontsWithInfo(display, "*", 1, &count, &finfo);
	if (!names || !count)
		goto xft;
	x_font = XtNewString(names[0]);

	/* Just in case we get some bad atoms... */
	olderr = XSetErrorHandler(ignore_x_errors);

	FOUNDRY     = XInternAtom(display, "FOUNDRY", False);
	WEIGHT_NAME = XInternAtom(display, "WEIGHT_NAME", False);
	SLANT       = XInternAtom(display, "SLANT", False);

	/* Grab our properties */
	for (i = 0; i < finfo[0].n_properties; i++) {
		if (finfo[0].properties[i].name == FOUNDRY)
			x_foundry = XGetAtomName(display, finfo[0].properties[i].card32);
		if (finfo[0].properties[i].name == XA_FAMILY_NAME)
			x_family = XGetAtomName(display, finfo[0].properties[i].card32);
		if (finfo[0].properties[i].name == WEIGHT_NAME)
			weight = XGetAtomName(display, finfo[0].properties[i].card32);
		if (finfo[0].properties[i].name == SLANT)
			slant = XGetAtomName(display, finfo[0].properties[i].card32);
		if (finfo[0].properties[i].name == XA_POINT_SIZE)
			x_pointsz = (int)(finfo[0].properties[i].card32 / 10);
	}

	if (weight) {
		switch (*weight) {
		case 'm': case 'M': x_weight = 100; break; /* Medium   */
		case 'd': case 'D': x_weight = 180; break; /* Demibold */
		case 'b': case 'B': x_weight = 200; break; /* Bold     */
		default: x_weight = 80; /* Regular */
		}

		XFree(weight);
	}

	if (slant) {
		switch (*slant) {
		case 'r': case 'R': x_slant = 0;   break; /* Roman   */
		case 'i': case 'I': x_slant = 100; break; /* Italic  */
		case 'o': case 'O': x_slant = 110; break; /* Oblique */
		default: x_slant = 0;
		}

		XFree(slant);
	}

	x_pixelsz = (int)(x_pointsz * (d / 72.));
	x_inkwidth = finfo[0].max_bounds.width;
	x_width    = finfo[0].max_bounds.rbearing -
	             finfo[0].max_bounds.lbearing;
	x_ascent   = finfo[0].ascent;
	x_descent  = finfo[0].descent;
	XSetErrorHandler(olderr);
	XFreeFontInfo(names, finfo, count);

xft:
#if USE_XFT
	/**
	 * Probe for a usable Xft font
	 */
	p  = FcPatternCreate();
	os = FcObjectSetBuild(FC_FOUNDRY, FC_FAMILY, FC_WEIGHT, FC_SLANT,
	                      FC_SIZE, FC_PIXEL_SIZE, NULL);
	fs = FcFontList(NULL, p, os);

	for (i = 0; i < fs->nfont; i++) {
		p2 = XftFontMatch(display, 0, fs->fonts[i], &res);
		if (res != FcResultMatch) {
			FcPatternDestroy(p2);
			continue;
		}

		f        = XftFontOpenPattern(display, p2);
		xft_font = (String)FcNameUnparse(f->pattern);
		if (FcPatternGetString(f->pattern, FC_FOUNDRY, 0, &s) == FcResultMatch)
			xft_foundry = XtNewString((String)s);
		if (FcPatternGetString(f->pattern, FC_FAMILY, 0, &s) == FcResultMatch)
			xft_family = XtNewString((String)s);
		if (FcPatternGetInteger(f->pattern, FC_WEIGHT, 0, &i) == FcResultMatch)
			xft_weight = i;
		if (FcPatternGetInteger(f->pattern, FC_SLANT, 0, &i) == FcResultMatch)
			xft_slant = i;
		if (FcPatternGetDouble(f->pattern, FC_SIZE, 0, &d) == FcResultMatch)
			xft_pointsz = (int)d;
		if (FcPatternGetDouble(f->pattern, FC_PIXEL_SIZE, 0, &d) == FcResultMatch)
			xft_pixelsz = (int)d;

		xft_width   = f->max_advance_width;
		xft_ascent  = f->ascent;
		xft_descent = f->descent;
		XftFontClose(display, f);
		break;
	}

	FcFontSetDestroy(fs);
	FcPatternDestroy(p);
	FcObjectSetDestroy(os);
#endif

	return;
}

static void cleanup(void)
{
	if (x_font)    XtFree(x_font);
	if (x_foundry) XFree(x_foundry);
	if (x_family)  XFree(x_family);
	x_font = x_foundry = x_family = NULL;

	if (xft_font)    XtFree(xft_font);
	if (xft_foundry) XtFree(xft_foundry);
	if (xft_family)  XtFree(xft_family);
	xft_font = xft_foundry = xft_family = NULL;

	uninit_xt();
}

START_TEST(create_null_widget)
{
	XmRendition rend;
	const struct __XmRenditionRec *r;

	rend = XmRenditionCreate(NULL, "test", NULL, 0);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL result");
	ck_assert_msg(r->display == _XmGetDefaultDisplay(), "Expected r->display to be the default");
	XmSharedPtrFree(rend);
}

START_TEST(create_default_tag)
{
	XmRendition rend;
	const struct __XmRenditionRec *r;

	rend = XmRenditionCreate(NULL, XmFONTLIST_DEFAULT_TAG, NULL, 0);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL result");
	ck_assert_msg(!strcmp(r->tag, XmFONTLIST_DEFAULT_TAG),
	              "Expected tag (%s) to be XmFONTLIST_DEFAULT_TAG", r->tag);
	XmSharedPtrFree(rend);
}
END_TEST

START_TEST(create_null_tag)
{
	XmRendition rend;
	const struct __XmRenditionRec *r;

	rend = XmRenditionCreate(NULL, NULL, NULL, 0);
	ck_assert_msg(!XmSharedPtrGet(rend), "Expected NULL result");
	XmSharedPtrFree(rend);
}
END_TEST

START_TEST(create_sets_pattern)
{
	Arg arg[3];
	String spec = NULL;
	XmRendition rend;
	XmFontType ft;
	const struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (x_font) {
		ft   = XmFONT_IS_FONT;
		spec = x_font;
	} else if (xft_font) {
		ft   = XmFONT_IS_XFT;
		spec = xft_font;
	}

	if (!spec)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_LAZY);
	XtSetArg(arg[1], XmNfontType,  ft);
	XtSetArg(arg[2], XmNfontName,  spec);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL result");
	ck_assert_msg(!strcmp(r->pattern, spec), "Expected pattern to equal spec");
	XmRenditionFree(rend);
}
END_TEST

static const XmLoadModel non_loaded_models[3] = {
	XmLOAD_LAZY, XmLOAD_DEFERRED, XmUNSPECIFIED_LOAD_MODEL
};

START_TEST(create_non_loaded_models)
{
	Arg arg[3];
	String spec = NULL;
	XmRendition rend;
	XmFontType ft;
	const struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (x_font) {
		ft   = XmFONT_IS_FONT;
		spec = x_font;
	} else if (xft_font) {
		ft   = XmFONT_IS_XFT;
		spec = xft_font;
	}

	if (!spec)
		return;

	XtSetArg(arg[0], XmNloadModel, non_loaded_models[_i]);
	if (non_loaded_models[_i] == XmUNSPECIFIED_LOAD_MODEL) {
		XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
		XtSetArg(arg[2], XmNfont, (void *)0x1234);
	}
	else {
		XtSetArg(arg[1], XmNfontType,  ft);
		XtSetArg(arg[2], XmNfontName,  spec);
	}

	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL result");
	ck_assert_msg(!r->xftFont, "Expected NULL xft font");

	if (non_loaded_models[_i] == XmUNSPECIFIED_LOAD_MODEL)
		ck_assert_msg(r->font == (void *)0x1234, "Expected user-supplied font");
	else ck_assert_msg(!r->font, "Expected NULL font");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_null_rendition)
{
	ck_assert_msg(!XmRenditionLoad(NULL, False),
	              "Expected False for NULL rendition");
}
END_TEST

START_TEST(load_unspecified_model)
{
	Arg arg[3];
	XmRendition rend;

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg(rend, "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False),
	              "Expected True for UNSPECIFIED_LOAD_MODEL");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_null_display)
{
	Arg arg[3];
	XmRendition rend;
	Display *d;
	struct __XmRenditionRec *r;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	d = r->display;
	r->display = NULL;
	ck_assert_msg(!XmRenditionLoad(rend, False), "Expected False for NULL display");
	r->display = d;
	r->font    = NULL;
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_null_pattern)
{
	Arg arg[3];
	String spec = NULL;
	XmRendition rend;
	XmFontType ft;
	String p;
	struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (x_font) {
		ft   = XmFONT_IS_FONT;
		spec = x_font;
	} else if (xft_font) {
		ft   = XmFONT_IS_XFT;
		spec = xft_font;
	}

	if (!spec)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_LAZY);
	XtSetArg(arg[1], XmNfontType,  ft);
	XtSetArg(arg[2], XmNfontName,  spec);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	p = r->pattern;
	r->pattern = NULL;
	ck_assert_msg(!XmRenditionLoad(rend, False), "Expected False for NULL pattern");
	r->pattern = p;
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_empty_pattern)
{
	Arg arg[3];
	String spec = NULL;
	XmRendition rend;
	XmFontType ft;
	String p;
	struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (x_font) {
		ft   = XmFONT_IS_FONT;
		spec = x_font;
	} else if (xft_font) {
		ft   = XmFONT_IS_XFT;
		spec = xft_font;
	}

	if (!spec)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_LAZY);
	XtSetArg(arg[1], XmNfontType,  ft);
	XtSetArg(arg[2], XmNfontName,  spec);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	p = r->pattern;
	r->pattern = XmS;
	ck_assert_msg(!XmRenditionLoad(rend, False), "Expected False for empty pattern");
	r->pattern = p;
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_invalid_font_type)
{
	Arg arg[3];
	XmRendition rend;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, 0xdeadbeef);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg(rend, "Expected non-NULL rendition");
	ck_assert_msg(!XmRenditionLoad(rend, False), "Expected False for invalid fontType");
	XmRenditionFree(rend);
}
END_TEST

static volatile Boolean no_font_called;
static void no_font_callback(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
	no_font_called = True;
}

START_TEST(load_calls_no_font_callback)
{
	Arg arg[3];
	XmRendition rend;
	Widget xd;
	struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_LAZY);
	XtSetArg(arg[1], XmNfontType,  XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName,  "-not-a-font-*");
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(!r->font, "Font should not have been loaded");

	xd = XmGetXmDisplay(r->display);
	no_font_called = False;
	XtAddCallback(xd, XmNnoFontCallback, no_font_callback, NULL);
	XmRenditionLoad(rend, True);
	ck_assert_msg(no_font_called, "Expected NO_FONT callback to be called");

	no_font_called = False;
	XmRenditionLoad(rend, False);
	ck_assert_msg(!no_font_called, "Expected NO_FONT callback not to be called");
	XtRemoveCallback(xd, XmNnoFontCallback, no_font_callback, NULL);

	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_user_supplied)
{
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(r->font == (void *)0x1234, "Unexpected font value");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_xfont)
{
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(r->font, "Expected non-NULL font");
	ck_assert_msg(r->fontFoundry && !strcmp(r->fontFoundry, x_foundry),
	              "Expected foundry (%s) to be %s", r->fontFoundry, x_foundry);
	ck_assert_msg(r->fontFamily && !strcmp(r->fontFamily, x_family),
	              "Expected family (%s) to be %s", r->fontFamily, x_family);
	ck_assert_msg(r->fontWeight == x_weight,
	              "Expected weight (%d) to equal (%d)", r->fontWeight, x_weight);
	ck_assert_msg(r->fontSlant == x_slant,
	              "Expected slant (%d) to equal (%d)", r->fontSlant, x_slant);
	ck_assert_msg(r->fontSize == x_pointsz,
	              "Expected size (%d) to equal %d", r->fontSize, x_pointsz);
	ck_assert_msg(r->pixelSize == x_pixelsz,
	              "Expected pixelSize (%d) to equal %d", r->pixelSize, x_pixelsz);
	XmRenditionFree(rend);
}
END_TEST

START_TEST(load_xft)
{
#if USE_XFT
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!xft_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNfontName, xft_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(r->xftFont, "Expected non-NULL xftFont");
	ck_assert_msg(r->fontFoundry && !strcmp(r->fontFoundry, xft_foundry),
	              "Expected foundry (%s) to be %s", r->fontFoundry, xft_foundry);
	ck_assert_msg(r->fontFamily && !strcmp(r->fontFamily, xft_family),
	              "Expected family (%s) to be %s", r->fontFamily, xft_family);
	ck_assert_msg(r->fontWeight == xft_weight,
	              "Expected weight (%d) to equal %d", r->fontWeight, xft_weight);
	ck_assert_msg(r->fontSlant == xft_slant,
	              "Expected slant (%d) to equal %d", r->fontSlant, xft_slant);
	ck_assert_msg(r->fontSize == xft_pointsz,
	              "Expected size (%d) to equal %d", r->fontSize, xft_pointsz);
	ck_assert_msg(r->pixelSize == xft_pixelsz,
	              "Expected pixelSize (%d) to equal %d",
	              r->pixelSize, xft_pixelsz);
	XmRenditionFree(rend);
#endif
}
END_TEST

START_TEST(load_already_loaded)
{
	Arg arg[3];
	XmRendition rend;
	XmFontType ft;
	XtPointer font;
	const struct __XmRenditionRec *r;

	/* Skip if we lack a font */
	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType,  XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName,  x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected to load the font");

	font = r->font;
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected second load to succeed");
	ck_assert_msg(r->font == font, "Expected the font to be equal");
	XmRenditionFree(rend);
}
END_TEST

/* Unload shouldn't touch the font props in this case */
START_TEST(unload_user_supplied)
{
	Arg arg[3];
	XmRendition rend;
	struct __XmRenditionRec *r;

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");

	r->fontFoundry = XtNewString("culo");
	r->fontFamily  = XtNewString("meo");
	XmRenditionUnload(rend);
	ck_assert_msg(r->fontFoundry, "Expected non-NULL foundry");
	ck_assert_msg(r->fontFamily,  "Expected non-NULL family");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(unload_xfont)
{
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");

	XmRenditionUnload(rend);
	ck_assert_msg(!r->fontFoundry, "Expected NULL foundry");
	ck_assert_msg(!r->fontFamily,  "Expected NULL family");
	ck_assert_msg(!r->fontStyle,   "Expected NULL style");
	ck_assert_msg(!r->fontWeight,  "Expected 0 weight");
	ck_assert_msg(!r->fontSlant,   "Expected 0 slant");
	ck_assert_msg(!r->fontSize,    "Expected 0 size");
	ck_assert_msg(!r->fontSpacing, "Expected 0 spacing");
	ck_assert_msg(!r->pixelSize,   "Expected 0 pixel size");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(unload_xft)
{
#if USE_XFT
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!xft_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNfontName, xft_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");

	XmRenditionUnload(rend);
	ck_assert_msg(!r->fontFoundry, "Expected NULL foundry");
	ck_assert_msg(!r->fontFamily,  "Expected NULL family");
	ck_assert_msg(!r->fontStyle,   "Expected NULL style");
	ck_assert_msg(!r->fontWeight,  "Expected 0 weight");
	ck_assert_msg(!r->fontSlant,   "Expected 0 slant");
	ck_assert_msg(!r->fontSize,    "Expected 0 size");
	ck_assert_msg(!r->fontSpacing, "Expected 0 spacing");
	ck_assert_msg(!r->pixelSize,   "Expected 0 pixel size");
	XmRenditionFree(rend);
#endif
}
END_TEST

START_TEST(has_codepoint_null_rend)
{
	ck_assert(!XmRenditionHasCodepoint(NULL, (XmCodepoint)'a'));
}
END_TEST

START_TEST(has_codepoint_non_xft_font)
{
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(XmRenditionHasCodepoint(rend, (XmCodepoint)'a'),
	              "Expected True for non-Xft font");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(fallback_no_rendertable)
{
	Arg arg[3];
	XmRendition rend;
	const struct __XmRenditionRec *r;

	if (!xft_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNfontName, xft_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(!XmRenditionFallbackForCodepoint(NULL, rend, (XmCodepoint)'a'),
	              "Expected NULL for NULL rendertable");
	XmRenditionFree(rend);
}
END_TEST

START_TEST(fallback_non_xft_font)
{
	Arg arg[3];
	XmRendition rend;
	XmRenderTable rt;
	const struct __XmRenditionRec *r;

	if (!x_font)
		return;

	rt = XmRenderTableCreate(NULL);
	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	ck_assert_msg(XmRenditionLoad(rend, False), "Expected load to succeed");
	ck_assert_msg(!XmRenditionFallbackForCodepoint(rt, rend, (XmCodepoint)'a'),
	              "Expected NULL for non-Xft font");
	XmRenditionFree(rend);
	XmRenderTableFree(rt);
}
END_TEST

/**
 * When requesting the font, since this is a deferred font,
 * XmRenditionLoad will be called to load it.
 */
START_TEST(getvalues_xfont)
{
	Arg arg[7];
	String foundry, family;
	int weight, slant, pointsz, pixelsz;
	XmRendition rend;
	XtPointer f = (void *)0x2468;
	const struct __XmRenditionRec *r;

	if (!x_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	XtSetArg(arg[0], XmNfont, &f);
	XtSetArg(arg[1], XmNfontFoundry, &foundry);
	XtSetArg(arg[2], XmNfontFamily, &family);
	XtSetArg(arg[3], XmNfontWeight, &weight);
	XtSetArg(arg[4], XmNfontSlant, &slant);
	XtSetArg(arg[5], XmNfontSize, &pointsz);
	XtSetArg(arg[6], XmNfontPixelSize, &pixelsz);
	XmRenditionGetValues(rend, arg, 7);
	ck_assert_msg(f && f != (void *)0x2468, "Expected f to be overwritten");
	ck_assert_msg(foundry, "Expected foundry to be non-NULL");
	ck_assert_msg(foundry != x_foundry,
	              "foundary should not be the same as x_foundry");
	ck_assert_msg(!strcmp(foundry, x_foundry),
	              "foundry (%s) should equal x_foundry (%s)",
	              foundry, x_foundry);
	ck_assert_msg(family, "Expected family to be non-NULL");
	ck_assert_msg(family != x_family,
	              "foundary should not be the same as x_family");
	ck_assert_msg(!strcmp(family, x_family),
	              "family (%s) should equal x_family (%s)",
	              family, x_family);
	ck_assert_msg(weight == x_weight,
	              "weight (%d) should equal x_weight (%d)",
	              weight, x_weight);
	ck_assert_msg(slant == x_slant,
	              "slant (%d) should equal x_slant (%d)",
	              slant, x_slant);
	ck_assert_msg(pointsz == x_pointsz,
	              "size (%d) should equal x_pointsz (%d)",
	              pointsz, x_pointsz);
	ck_assert_msg(pixelsz == x_pixelsz,
	              "pixelSize (%d) should equal x_pixelsz (%d)",
	              pixelsz, x_pixelsz);
	ck_assert_msg(r->width == x_width,
	              "Expected r->width (%d) to equal x_width (%d)",
	              r->width, x_width);
	ck_assert_msg(r->ink_width == x_inkwidth,
	              "Expected r->ink_width (%d) to equal x_inkwidth (%d)",
	              r->ink_width, x_inkwidth);
	ck_assert_msg(r->ascent == x_ascent,
	              "Expected r->ascent (%d) to equal x_ascent (%d)",
	              r->ascent, x_ascent);
	ck_assert_msg(r->descent == x_descent,
	              "Expected r->descent (%d) to equal x_descent (%d)",
	              r->descent, x_descent);
	XmRenditionFree(rend);
}
END_TEST

START_TEST(getvalues_xft)
{
	Arg arg[7];
	String foundry, family;
	int weight, slant, pointsz, pixelsz;
	XmRendition rend;
	XtPointer f = (void *)0x2468;
	const struct __XmRenditionRec *r;

	if (!xft_font)
		return;

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNfontName, xft_font);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	XtSetArg(arg[0], XmNxftFont, &f);
	XtSetArg(arg[1], XmNfontFoundry, &foundry);
	XtSetArg(arg[2], XmNfontFamily, &family);
	XtSetArg(arg[3], XmNfontWeight, &weight);
	XtSetArg(arg[4], XmNfontSlant, &slant);
	XtSetArg(arg[5], XmNfontSize, &pointsz);
	XtSetArg(arg[6], XmNfontPixelSize, &pixelsz);
	XmRenditionGetValues(rend, arg, 7);
	ck_assert_msg(f && f != (void *)0x2468, "Expected f to be overwritten");
	ck_assert_msg(foundry, "Expected foundry to be non-NULL");
	ck_assert_msg(foundry != xft_foundry,
	              "foundary should not be the same as xft_foundry");
	ck_assert_msg(!strcmp(foundry, xft_foundry),
	              "foundry (%s) should equal xft_foundry (%s)",
	              foundry, xft_foundry);
	ck_assert_msg(family, "Expected family to be non-NULL");
	ck_assert_msg(family != xft_family,
	              "foundary should not be the same as xft_family");
	ck_assert_msg(!strcmp(family, xft_family),
	              "family (%s) should equal xft_family (%s)",
	              family, xft_family);
	ck_assert_msg(weight == xft_weight,
	              "weight (%d) should equal xft_weight (%d)",
	              weight, xft_weight);
	ck_assert_msg(slant == xft_slant,
	              "slant (%d) should equal xft_slant (%d)",
	              slant, xft_slant);
	ck_assert_msg(pointsz == xft_pointsz,
	              "size (%d) should equal xft_pointsz (%d)",
	              pointsz, xft_pointsz);
	ck_assert_msg(pixelsz == xft_pixelsz,
	              "pixelSize (%d) should equal xft_pixelsz (%d)",
	              pixelsz, xft_pixelsz);
	ck_assert_msg(r->width == xft_width,
	              "Expected r->width (%d) to equal xft_width (%d)",
	              r->width, xft_width);
	ck_assert_msg(r->ink_width == xft_width,
	              "Expected r->ink_width (%d) to equal xft_width (%d)",
	              r->ink_width, xft_width);
	ck_assert_msg(r->ascent == xft_ascent,
	              "Expected r->ascent (%d) to equal xft_ascent (%d)",
	              r->ascent, xft_ascent);
	ck_assert_msg(r->descent == xft_descent,
	              "Expected r->descent (%d) to equal xft_descent (%d)",
	              r->descent, xft_descent);
	XmRenditionFree(rend);
}
END_TEST

/* In this case, the font values are passthru */
START_TEST(getvalues_user_specified)
{
	Arg arg[3];
	XmRendition rend;
	XtPointer f = (void *)0x2468;
	const struct __XmRenditionRec *r;

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNxftFont, f);
	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");
	XtSetArg(arg[0], XmNxftFont, &f);
	XmRenditionGetValues(rend, arg, 1);
	ck_assert_msg(f && f == (void *)0x2468, "Unexpected value for f (%p)", f);
	XmRenditionFree(rend);
}
END_TEST

/**
 * When requesting the font, since this is a deferred font,
 * XmRenditionLoad will be called to load it.
 */
START_TEST(setvalues_xfont)
{
	Arg arg[3];
	XmRendition rend;
	struct __XmRenditionRec *r;

	if (!x_font)
		return;

	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, NULL, 0);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfontName, x_font);
	XmRenditionSetValues(rend, arg, 3);

	ck_assert_msg(r->loadModel == XmLOAD_DEFERRED,
	              "Expected r->loadModel to be XmLOAD_DEFERRED");
	ck_assert_msg(r->pattern != x_font, "Expected r->pattern to differ from x_font");
	ck_assert_msg(!strcmp(r->pattern, x_font),
	              "Expected r->pattern (%s) to compare equal to x_font (%s)",
	              r->pattern, x_font);
	ck_assert_msg(r->font, "Expected r->font to not be NULL");
	ck_assert_msg(r->fontFoundry, "Expected r->fontFoundry to not be NULL");
	ck_assert_msg(!strcmp(r->fontFoundry, x_foundry),
	              "Expected r->fontFoundry (%s) to compare equal to x_foundry (%s)",
	              r->fontFoundry, x_foundry);
	ck_assert_msg(r->fontFamily, "Expected r->fontFamily to not be NULL");
	ck_assert_msg(!strcmp(r->fontFamily, x_family),
	              "Expected r->fontFamily (%s) to compare equal to x_family (%s)",
	              r->fontFamily, x_family);
	ck_assert_msg(r->fontWeight == x_weight,
	              "Expected r->fontWeight (%d) to equal x_weight (%d)",
	              r->fontWeight, x_weight);
	ck_assert_msg(r->fontSlant == x_slant,
	              "Expected r->fontSlant (%d) to equal x_slant (%d)",
	              r->fontSlant, x_slant);
	ck_assert_msg(r->fontSize == x_pointsz,
	              "Expected r->fontSize (%d) to equal x_pointsz (%d)",
	              r->fontSize, x_pointsz);
	ck_assert_msg(r->pixelSize == x_pixelsz,
	              "Expected r->pixelSize (%d) to equal x_pixelsz (%d)",
	              r->pixelSize, x_pixelsz);
	ck_assert_msg(r->width == x_width,
	              "Expected r->width (%d) to equal x_width (%d)",
	              r->width, x_width);
	ck_assert_msg(r->ink_width == x_inkwidth,
	              "Expected r->ink_width (%d) to equal x_inkwidth (%d)",
	              r->ink_width, x_inkwidth);
	ck_assert_msg(r->ascent == x_ascent,
	              "Expected r->ascent (%d) to equal x_ascent (%d)",
	              r->ascent, x_ascent);
	ck_assert_msg(r->descent == x_descent,
	              "Expected r->descent (%d) to equal x_descent (%d)",
	              r->descent, x_descent);
	XmRenditionFree(rend);
}
END_TEST

START_TEST(setvalues_xft)
{
	Arg arg[3];
	XmRendition rend;
	struct __XmRenditionRec *r;

	if (!xft_font)
		return;

	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, NULL, 0);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	XtSetArg(arg[0], XmNloadModel, XmLOAD_DEFERRED);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(arg[2], XmNfontName, xft_font);
	XmRenditionSetValues(rend, arg, 3);

	ck_assert_msg(r->loadModel == XmLOAD_DEFERRED,
	              "Expected r->loadModel to be XmLOAD_DEFERRED");
	ck_assert_msg(r->pattern != xft_font, "Expected r->pattern to differ from xft_font");
	ck_assert_msg(!strcmp(r->pattern, xft_font),
	              "Expected r->pattern (%s) to compare equal to xft_font (%s)",
	              r->pattern, xft_font);
	ck_assert_msg(r->xftFont, "Expected r->xftFont to not be NULL");
	ck_assert_msg(r->fontFoundry, "Expected r->fontFoundry to not be NULL");
	ck_assert_msg(!strcmp(r->fontFoundry, xft_foundry),
	              "Expected r->fontFoundry (%s) to compare equal to xft_foundry (%s)",
	              r->fontFoundry, xft_foundry);
	ck_assert_msg(r->fontFamily, "Expected r->fontFamily to not be NULL");
	ck_assert_msg(!strcmp(r->fontFamily, xft_family),
	              "Expected r->fontFamily (%s) to compare equal to xft_family (%s)",
	              r->fontFamily, xft_family);
	ck_assert_msg(r->fontWeight == xft_weight,
	              "Expected r->fontWeight (%d) to equal xft_weight (%d)",
	              r->fontWeight, xft_weight);
	ck_assert_msg(r->fontSlant == xft_slant,
	              "Expected r->fontSlant (%d) to equal xft_slant (%d)",
	              r->fontSlant, xft_slant);
	ck_assert_msg(r->fontSize == xft_pointsz,
	              "Expected r->fontSize (%d) to equal xft_pointsz (%d)",
	              r->fontSize, xft_pointsz);
	ck_assert_msg(r->pixelSize == xft_pixelsz,
	              "Expected r->pixelSize (%d) to equal xft_pixelsz (%d)",
	              r->pixelSize, xft_pixelsz);
	ck_assert_msg(r->width == xft_width,
	              "Expected r->width (%d) to equal xft_width (%d)",
	              r->width, xft_width);
	ck_assert_msg(r->ink_width == xft_width,
	              "Expected r->ink_width (%d) to equal xft_width (%d)",
	              r->ink_width, xft_width);
	ck_assert_msg(r->ascent == xft_ascent,
	              "Expected r->ascent (%d) to equal xft_ascent (%d)",
	              r->ascent, xft_ascent);
	ck_assert_msg(r->descent == xft_descent,
	              "Expected r->descent (%d) to equal xft_descent (%d)",
	              r->descent, xft_descent);
	XmRenditionFree(rend);
}
END_TEST

/* The properties should get loaded from the supplied font */
START_TEST(setvalues_user_specified)
{
	Arg arg[3];
	XmRendition rend;
	XFontStruct *f;
	struct __XmRenditionRec *r;

	if (!x_font)
		return;

	ck_assert_msg((f = XLoadQueryFont(display, x_font)),
	              "Failed to load X font");

	rend = XmRenditionCreate(NULL, XmSTRING_DEFAULT_CHARSET, NULL, 0);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected non-NULL rendition");

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, f);
	XmRenditionSetValues(rend, arg, 3);

	ck_assert_msg(r->loadModel == XmUNSPECIFIED_LOAD_MODEL,
	              "Expected r->loadModel to be XmUNSPECIFIED_LOAD_MODEL");
	ck_assert_msg(!r->pattern, "Expected r->pattern to be NULL");
	ck_assert_msg(r->font, "Expected r->font to not be NULL");
	ck_assert_msg(r->fontFoundry, "Expected r->fontFoundry to not be NULL");
	ck_assert_msg(!strcmp(r->fontFoundry, x_foundry),
	              "Expected r->fontFoundry (%s) to compare equal to x_foundry (%s)",
	              r->fontFoundry, x_foundry);
	ck_assert_msg(r->fontFamily, "Expected r->fontFamily to not be NULL");
	ck_assert_msg(!strcmp(r->fontFamily, x_family),
	              "Expected r->fontFamily (%s) to compare equal to x_family (%s)",
	              r->fontFamily, x_family);
	ck_assert_msg(r->fontWeight == x_weight,
	              "Expected r->fontWeight (%d) to equal x_weight (%d)",
	              r->fontWeight, x_weight);
	ck_assert_msg(r->fontSlant == x_slant,
	              "Expected r->fontSlant (%d) to equal x_slant (%d)",
	              r->fontSlant, x_slant);
	ck_assert_msg(r->fontSize == x_pointsz,
	              "Expected r->fontSize (%d) to equal x_pointsz (%d)",
	              r->fontSize, x_pointsz);
	ck_assert_msg(r->pixelSize == x_pixelsz,
	              "Expected r->pixelSize (%d) to equal x_pixelsz (%d)",
	              r->pixelSize, x_pixelsz);
	XmRenditionFree(rend);
	XFreeFont(display, f);
}
END_TEST

void xmrendition_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmRendition");

	t = tcase_create("Create");
	tcase_add_test(t, create_null_widget);
	tcase_add_test(t, create_default_tag);
	tcase_add_test(t, create_null_tag);
	tcase_add_test(t, create_sets_pattern);
	tcase_add_loop_test(t, create_non_loaded_models, 0, XtNumber(non_loaded_models));
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Load");
	tcase_add_test(t, load_null_rendition);
	tcase_add_test(t, load_unspecified_model);
	tcase_add_test(t, load_null_display);
	tcase_add_test(t, load_null_pattern);
	tcase_add_test(t, load_empty_pattern);
	tcase_add_test(t, load_invalid_font_type);
	tcase_add_test(t, load_calls_no_font_callback);
	tcase_add_test(t, load_user_supplied);
	tcase_add_test(t, load_xfont);
	tcase_add_test(t, load_xft);
	tcase_add_test(t, load_already_loaded);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Unload");
	tcase_add_test(t, unload_user_supplied);
	tcase_add_test(t, unload_xfont);
	tcase_add_test(t, unload_xft);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("HasCodepoint");
	tcase_add_test(t, has_codepoint_null_rend);
	tcase_add_test(t, has_codepoint_non_xft_font);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("FallbackForCodepoint");
	tcase_add_test(t, fallback_no_rendertable);
	tcase_add_test(t, fallback_non_xft_font);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetValues");
	tcase_add_test(t, getvalues_xfont);
	tcase_add_test(t, getvalues_xft);
	tcase_add_test(t, getvalues_user_specified);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("SetValues");
	tcase_add_test(t, setvalues_xfont);
	tcase_add_test(t, setvalues_xft);
	tcase_add_test(t, setvalues_user_specified);
	tcase_add_checked_fixture(t, _init_xt, cleanup);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

