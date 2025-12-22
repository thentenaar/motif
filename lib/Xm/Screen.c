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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Screen.c /main/16 1997/06/18 17:41:50 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <X11/Xatom.h>
#include <Xm/Xm.h>		/* To make cpp on Sun happy. CR 5943 */
#include <Xm/AtomMgr.h>
#include <Xm/DisplayP.h>

#include "DragIconI.h"
#include "HashI.h"
#include "ImageCachI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "PixConvI.h"

#if XM_WITH_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#if XM_WITH_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#define DEFAULT_QUAD_WIDTH 10
#define RESOURCE_DEFAULT  (-1)
#define SCREEN_MISMATCH   _XmMMsgScreen_0000
#define CANT_FIND_DISPLAY _XmMMsgScreen_0001

/********    Static Function Declarations    ********/
static void ClassPartInitialize(WidgetClass wc);
static void ClassInitialize(void);
static void GetUnitFromFont(Display *display, XFontStruct *fst,
                            int *ph_unit, int *pv_unit);
static void Initialize(Widget requested_widget, Widget new_widget,
                       ArgList args, Cardinal *num_args);
static Boolean SetValues(Widget current, Widget requested, Widget new_w,
                         ArgList args, Cardinal *num_args);
static void Destroy(Widget widget);
static Boolean MatchPixmap(XmHashKey a, XmHashKey b);
static XmHashValue HashPixmap(XmHashKey x);
static Boolean FreePixmap(XmHashKey k, XtPointer p, XtPointer client_data);

#define Offset(x) (XtOffsetOf(XmScreenRec, x))
static XtResource resources[] = {
	{
		XmNdarkThreshold, XmCDarkThreshold, XmRInt,
		sizeof(int), Offset(screen.darkThreshold),
		XmRImmediate, NULL,
	},
	{
		XmNlightThreshold, XmCLightThreshold, XmRInt,
		sizeof(int), Offset(screen.lightThreshold),
		XmRImmediate, NULL,
	},
	{
		XmNforegroundThreshold, XmCForegroundThreshold, XmRInt,
		sizeof(int), Offset(screen.foregroundThreshold),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultNoneCursorIcon, XmCDefaultNoneCursorIcon, XmRWidget,
		sizeof(Widget), Offset(screen.defaultNoneCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultValidCursorIcon, XmCDefaultValidCursorIcon,
		XmRWidget, sizeof(Widget), Offset(screen.defaultValidCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultInvalidCursorIcon, XmCDefaultInvalidCursorIcon, XmRWidget,
		sizeof(Widget), Offset(screen.defaultInvalidCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultMoveCursorIcon, XmCDefaultMoveCursorIcon, XmRWidget,
		sizeof(Widget), Offset(screen.defaultMoveCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultLinkCursorIcon, XmCDefaultLinkCursorIcon,
		XmRWidget, sizeof(Widget), Offset(screen.defaultLinkCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultCopyCursorIcon, XmCDefaultCopyCursorIcon, XmRWidget,
		sizeof(Widget), Offset(screen.defaultCopyCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNdefaultSourceCursorIcon, XmCDefaultSourceCursorIcon, XmRWidget,
		sizeof(Widget), Offset(screen.defaultSourceCursorIcon),
		XmRImmediate, NULL,
	},
	{
		XmNmenuCursor, XmCCursor, XmRCursor,
		sizeof(Cursor), Offset(screen.menuCursor),
		XmRString, "arrow",
	},
	{
		XmNunpostBehavior, XmCUnpostBehavior, XmRUnpostBehavior,
		sizeof(unsigned char), Offset(screen.unpostBehavior),
		XmRImmediate, (XtPointer)XmUNPOST_AND_REPLAY,
	},
	{
		XmNfont, XmCFont, XmRFontStruct,
		sizeof(XFontStruct *), Offset(screen.font_struct),
		XmRString, XmDEFAULT_FONT,
	},
	{
		XmNhorizontalFontUnit, XmCHorizontalFontUnit, XmRInt,
		sizeof(int), Offset(screen.h_unit),
		XmRImmediate, (XtPointer)RESOURCE_DEFAULT,
	},
	{
		XmNverticalFontUnit, XmCVerticalFontUnit, XmRInt,
		sizeof(int), Offset(screen.v_unit),
		XmRImmediate, (XtPointer)RESOURCE_DEFAULT,
	},
	{
    	XmNmoveOpaque, XmCMoveOpaque, XmRBoolean,
    	sizeof(unsigned char), Offset(screen.moveOpaque),
    	XmRImmediate, (XtPointer)False,
	},
	{
    	XmNcolorCalculationProc, XmCColorCalculationProc, XmRProc,
    	sizeof(XtProc), Offset(screen.color_calc_proc),
    	XmRImmediate, NULL,
	},
	{
    	XmNcolorAllocationProc, XmCColorAllocationProc, XmRProc,
    	sizeof(XtProc), Offset(screen.color_alloc_proc),
    	XmRImmediate, NULL,
	},
	{
    	XmNbitmapConversionModel, XmCBitmapConversionModel,
    	XmRBitmapConversionModel,
    	sizeof(XtEnum), Offset(screen.bitmap_conversion_model),
    	XmRImmediate, (XtPointer)XmMATCH_DEPTH,
	},
	{
    	XmNuserData, XmCUserData, XmRPointer,
    	sizeof(XtPointer), Offset(screen.user_data),
    	XmRPointer, NULL
	},
	{
		XmNinsensitiveStippleBitmap, XmCInsensitiveStippleBitmap,
		XmRNoScalingBitmap,
		sizeof(Pixmap), Offset(screen.insensitive_stipple_bitmap),
		XmRString, XmS50_foreground
	}
};
#undef Offset

/***************************************************************************
 *
 * Class Record
 *
 ***************************************************************************/
static XmBaseClassExtRec baseClassExtRec = {
	NULL,
	NULLQUARK,
	XmBaseClassExtVersion,
	sizeof(XmBaseClassExtRec),
	NULL,   /* InitializePrehook      */
	NULL,   /* SetValuesPrehook       */
	NULL,   /* InitializePosthook     */
	NULL,   /* SetValuesPosthook      */
	NULL,   /* secondaryObjectClass   */
	NULL,   /* secondaryCreate        */
	NULL,   /* getSecRes data         */
	{ 0 },  /* fastSubclass flags     */
	NULL,   /* getValuesPrehook       */
	NULL,   /* getValuesPosthook      */
	NULL,   /* classPartInitPrehook   */
	NULL,   /* classPartInitPosthook  */
	NULL,   /* ext_resources          */
	NULL,   /* compiled_ext_resources */
	0,      /* num_ext_resources      */
	FALSE,  /* use_sub_resources      */
	NULL,   /* widgetNavigable        */
	NULL,   /* focusChange            */
	NULL    /* wrapperData            */
};

externaldef(xmscreenclassrec) XmScreenClassRec xmScreenClassRec = {
{ /* core */
	(WidgetClass)&coreClassRec,  /* superclass          */
	"XmScreen",                  /* class_name          */
	sizeof(XmScreenRec),         /* size                */
	ClassInitialize,             /* Class Initializer   */
	ClassPartInitialize,         /* class_part_init     */
	FALSE,                       /* Class init'ed ?     */
	Initialize,                  /* initialize          */
	NULL,                        /* initialize_notify   */
	NULL,                        /* realize             */
	NULL,                        /* actions             */
	0,                           /* num_actions         */
	resources,                   /* resources           */
	XtNumber(resources),         /* resource_count      */
	NULLQUARK,                   /* xrm_class           */
	FALSE,                       /* compress_motion     */
	XtExposeNoCompress,          /* compress_exposure   */
	FALSE,                       /* compress_enterleave */
	FALSE,                       /* visible_interest    */
	Destroy,                     /* destroy             */
	NULL,                        /* resize              */
	NULL,                        /* expose              */
	SetValues,                   /* set_values          */
	NULL,                        /* set_values_hook     */
	NULL,                        /* set_values_almost   */
	NULL,                        /* get_values_hook     */
	NULL,                        /* accept_focus        */
	XtVersion,                   /* intrinsics version  */
	NULL,                        /* callback offsets    */
	NULL,                        /* tm_table            */
	NULL,                        /* query_geometry      */
	NULL,                        /* screen_accelerator  */
	(XtPointer)&baseClassExtRec, /* extension           */
},
{ /* desktop */
	NULL,                        /* child_class         */
	NULL,                        /* insert_child        */
	NULL,                        /* delete_child        */
	NULL,                        /* extension           */
},
{ /* screen */
	NULL,
}};

externaldef(xmscreenclass) WidgetClass xmScreenClass =
	(WidgetClass)&xmScreenClassRec;

externaldef(xmscreenquark) XrmQuark _XmInvalidCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmValidCursorIconQuark   = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmNoneCursorIconQuark    = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmDefaultDragIconQuark   = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmMoveCursorIconQuark    = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmCopyCursorIconQuark    = NULLQUARK;
externaldef(xmscreenquark) XrmQuark _XmLinkCursorIconQuark    = NULLQUARK;

/* Context used to memoize Screen -> XmScreen association */
static XContext screen_context = None;

static void ClassInitialize(void)
{
	baseClassExtRec.record_type = XmQmotif;

	_XmInvalidCursorIconQuark = XrmPermStringToQuark("defaultInvalidCursorIcon");
	_XmValidCursorIconQuark   = XrmPermStringToQuark("defaultValidCursorIcon");
	_XmNoneCursorIconQuark    = XrmPermStringToQuark("defaultNoneCursorIcon");
	_XmDefaultDragIconQuark   = XrmPermStringToQuark("defaultSourceCursorIcon");
	_XmMoveCursorIconQuark    = XrmPermStringToQuark("defaultMoveCursorIcon");
	_XmCopyCursorIconQuark    = XrmPermStringToQuark("defaultCopyCursorIcon");
	_XmLinkCursorIconQuark    = XrmPermStringToQuark("defaultLinkCursorIcon");

	XtInitializeWidgetClass(xmDesktopClass);
	xmScreenClassRec.desktop_class.insert_child = xmDesktopClassRec.desktop_class.insert_child;
	xmScreenClassRec.desktop_class.delete_child = xmDesktopClassRec.desktop_class.delete_child;
	screen_context = XUniqueContext();
}

static void ClassPartInitialize(WidgetClass wc)
{
	_XmFastSubclassInit(wc, XmSCREEN_BIT);
}

/************************************************************************
 *
 *  GetUnitFromFont
 *
 ************************************************************************/
static void GetUnitFromFont(Display *display, XFontStruct *fst,
                            int *ph_unit, int *pv_unit)
{
	enum { XmAAVERAGE_WIDTH, XmAPIXEL_SIZE, XmARESOLUTION_Y, NUM_ATOMS };
	static char *atom_names[NUM_ATOMS] = { XmIAVERAGE_WIDTH, XmIPIXEL_SIZE, XmIRESOLUTION_Y };
	Atom atoms[XtNumber(atom_names)];
	unsigned long pixel_s, avg_w, point_s, res_y;
	unsigned long font_unit_return;

	if (!fst) {
		if (ph_unit) *ph_unit = DEFAULT_QUAD_WIDTH;
		if (pv_unit) *pv_unit = DEFAULT_QUAD_WIDTH;
		return;
	}

	XInternAtoms(display, atom_names, XtNumber(atom_names), TRUE, atoms);

	/* Horizontal units */
	if (ph_unit) {
		if (atoms[XmAAVERAGE_WIDTH] && XGetFontProperty(fst, atoms[XmAAVERAGE_WIDTH], &avg_w))
			*ph_unit = (int)(avg_w / 10.0 + 0.5);
		else if (XGetFontProperty(fst, XA_QUAD_WIDTH, &font_unit_return))
			*ph_unit = font_unit_return;
		else *ph_unit = (int)((fst->min_bounds.width + fst->max_bounds.width) / 2.3 + 0.5);
	}

	/* Vertical units */
	if (pv_unit) {
		if (XGetFontProperty(fst, atoms[XmAPIXEL_SIZE], &pixel_s))
			*pv_unit = (int)(pixel_s / 1.8 + 0.5);
		else if (XGetFontProperty(fst, XA_POINT_SIZE, &point_s) &&
		         XGetFontProperty(fst, atoms[XmARESOLUTION_Y], &res_y)) {
			*pv_unit = (int)(((point_s * (float)res_y) / 1400.0) + 0.5);
		} else *pv_unit = (int)((fst->max_bounds.ascent + fst->max_bounds.descent) / 2.2 + 0.5);
	}
}

#ifdef DEBUG
static void dump_monitors(XmScreen scr)
{
	Cardinal i;

	fputs("\n===============================================\n", stderr);
	fprintf(stderr, "Got %u monitor(s)\n", scr->screen.n_monitors);
	for (i = 0; i < scr->screen.n_monitors; i++) {
		fprintf(stderr, "[%u] (%d,%d) %dpx (%d mm) x %dpx (%d mm) %g dpi \"%s\"\n", i,
		        scr->screen.monitors[i].x, scr->screen.monitors[i].y,
		        scr->screen.monitors[i].width, scr->screen.monitors[i].width_mm,
		        scr->screen.monitors[i].height, scr->screen.monitors[i].height_mm,
		        scr->screen.monitors[i].dpi,
		        scr->screen.monitors[i].name ? scr->screen.monitors[i].name : "(none)");
	}
	fputs("===============================================\n", stderr);
}
#endif /* DEBUG */

/**
 * Canonical way for users to specify their desired DPI. This being
 * set implies that Xft's default DPI calculation (which is based
 * particularly on the X-provided Screen dimensions) isn't good
 * enough to satisfy the user; likely because the size in millimeters
 * X provides isn't correctly configured.
 */
static float user_dpi(Display *d)
{
	double dd = SCREEN_DEFAULT_DPI;
	char *def, *end = NULL;

	if ((def = XGetDefault(d, "Xft", "dpi"))) {
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		dd = strtod(def, &end);
		if (!end || !isfinite(dd) || dd == HUGE_VAL)
			return SCREEN_DEFAULT_DPI;
#else
		(void)end;
		dd = atof(def);
		if (dd != dd || dd < -DBL_MAX || dd > DBL_MAX)
			return SCREEN_DEFAULT_DPI;
#endif
	}

	return (float)dd;
}

/**
 * Approximate our diagonal pixels per inch
 */
static float calc_dpi(int w, int h, int w_mm, int h_mm)
{
	double d, nd, diag_px, diag_mm;
	if (!w || !h || !w_mm || !h_mm)
		return SCREEN_DEFAULT_DPI;

	diag_px = sqrt(w * w + h * h);
	diag_mm = sqrt(w_mm * w_mm + h_mm * h_mm);
	d = floor((diag_px * 25.4) / diag_mm);
	return (float)((d <= SCREEN_DEFAULT_DPI) ? SCREEN_DEFAULT_DPI : d);
}

/**
 * Set the logical DPI for the screen
 *
 * We prefer the user-specified DPI if it's set to something larger than
 * the default.
 */
static void set_dpi(XmScreenPart *screen)
{
	if ((screen->dpi = screen->user_dpi) <= SCREEN_DEFAULT_DPI)
		screen->dpi = calc_dpi(screen->width, screen->height,
		                       screen->width_mm, screen->height_mm);
}

#if XM_WITH_XRANDR
/**
 * Xt's default dispatcher ignores extension events. So, to use extension
 * events with Xt, a custom dispatcher is needed.
 */
static Boolean dispatch_randr(XEvent *event)
{
	Widget w = XtWindowToWidget(event->xany.display, event->xany.window);

	return XFilterEvent(event, XtWindow(w)) ||
	       XtDispatchEventToWidget(w, event);
}

/**
 * Allow Xt to select for Xrandr events
 *
 * We only need ScreenChange and CrtcChangeNotify, but we'll select
 * the entire gamut for the benefit of other applications that may
 * want to utilize XRandR, saving them the event dispatching pain.
 */
static void select_randr(Widget w, int *types, XtPointer *sel,
                        int count, XtPointer client)
{
	(void)types;
	(void)sel;
	(void)count;
	(void)client;

	XRRSelectInput(XtDisplay(w), XtWindow(w),
	               RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask |
	               RROutputChangeNotifyMask | RROutputPropertyNotifyMask |
	               RRProviderChangeNotifyMask | RRProviderPropertyNotifyMask |
	               RRResourceChangeNotifyMask | RRLeaseNotifyMask);
}

/**
 * Update the monitor info using Xrandr's monitor API
 */
static void monitors_randr(Display *d, Window root, XmScreen screen)
{
	float dp;
	int j;
	Cardinal i, n;
	XRRMonitorInfo *info;

	_XmDisplayToAppContext(d);
	_XmAppLock(app);

	for (i = 0; i < screen->screen.n_monitors; i++)
		XFree((XPointer)screen->screen.monitors[i].name);
	XtFree((XtPointer)screen->screen.monitors);
	screen->screen.monitors   = NULL;
	screen->screen.n_monitors = 0;

	if (!(info = XRRGetMonitors(d, root, True, &j))) {
		_XmAppUnlock(app);
		return;
	}

	if ((n = (Cardinal)j)) {
		screen->screen.n_monitors = n;
		screen->screen.monitors   = XtMallocArray(n, sizeof *screen->screen.monitors);
	}

	for (i = 0; i < n; i++) {
		dp = calc_dpi(info[i].width, info[i].height,
		              info[i].mwidth, info[i].mheight);
		screen->screen.monitors[i].x         = info[i].x;
		screen->screen.monitors[i].y         = info[i].y;
		screen->screen.monitors[i].width     = info[i].width;
		screen->screen.monitors[i].height    = info[i].height;
		screen->screen.monitors[i].width_mm  = info[i].mwidth;
		screen->screen.monitors[i].height_mm = info[i].mheight;
		screen->screen.monitors[i].dpi       = dp;
		screen->screen.monitors[i].name      = XGetAtomName(d, info[i].name);
	}

	if (n == 1) {
		screen->screen.width_mm  = info[0].mwidth;
		screen->screen.height_mm = info[0].mheight;
		set_dpi(&screen->screen);
	}

	XRRFreeMonitors(info);
	_XmAppUnlock(app);
}
#endif /* XM_WITH_XRANDR */

#if XM_WITH_XINERAMA
/**
 * Update the monitor info using Xinerama's API
 */
static void monitors_xinerama(Display *d, Window root, XmScreen screen)
{
	int i, n;
	XineramaScreenInfo *info;

	_XmDisplayToAppContext(d);
	_XmAppLock(app);

	for (i = 0; i < screen->screen.n_monitors; i++)
		XFree((XPointer)screen->screen.monitors[i].name);
	XtFree((XtPointer)screen->screen.monitors);
	screen->screen.monitors   = NULL;
	screen->screen.n_monitors = 0;

	if (!(info = XineramaQueryScreens(d, &n))) {
		_XmAppUnlock(app);
		return;
	}

	if (n) {
		screen->screen.n_monitors = n;
		screen->screen.monitors   = XtMallocArray(n, sizeof *screen->screen.monitors);
	}

	for (i = 0; i < n; i++) {
		screen->screen.monitors[i].x         = info[i].x_org;
		screen->screen.monitors[i].y         = info[i].y_org;
		screen->screen.monitors[i].width     = info[i].width;
		screen->screen.monitors[i].height    = info[i].height;
		screen->screen.monitors[i].width_mm  = 0;
		screen->screen.monitors[i].height_mm = 0;
		screen->screen.monitors[i].dpi       = screen->screen.dpi;
		screen->screen.monitors[i].name      = NULL;
	}

	XFree(info);
	_XmAppUnlock(app);
}
#endif /* XM_WITH_XINERAMA */

/**
 * If we don't have anything else, at least we have a root window
 */
static void monitor_rootwin(Display *d, Window root, XmScreen screen)
{
	float dp;
	Cardinal i;
	_XmDisplayToAppContext(d);
	_XmAppLock(app);

	for (i = 0; i < screen->screen.n_monitors; i++)
		XFree((XPointer)screen->screen.monitors[i].name);

	screen->screen.n_monitors = 1;
	if (!screen->screen.monitors)
		screen->screen.monitors = (XmMonitorInfo *)XtNew(XmMonitorInfo);

	dp = calc_dpi(screen->screen.width, screen->screen.height,
	              screen->screen.width_mm, screen->screen.height_mm);

	screen->screen.monitors->x         = 0;
	screen->screen.monitors->y         = 0;
	screen->screen.monitors->width     = screen->screen.width;
	screen->screen.monitors->height    = screen->screen.height;
	screen->screen.monitors->width_mm  = screen->screen.width_mm;
	screen->screen.monitors->height_mm = screen->screen.height_mm;
	screen->screen.monitors->dpi       = dp;
	screen->screen.monitors->name      = NULL;
	_XmAppUnlock(app);
}

/**
 * Update dimensions on our monitor(s) and root window
 */
static void monitor_cb(Widget w, XtPointer data, XEvent *ev, Boolean *cont)
{
#if XM_WITH_XRANDR
	XRRNotifyEvent *notif;
	XRRScreenChangeNotifyEvent *change;
#endif

	Display *d     = XtDisplayOfObject(w);
	Screen *screen = XtScreenOfObject(w);
	Window root    = RootWindowOfScreen(screen);
	XmScreen scr   = XmScreenOfObject(w);

	_XmDisplayToAppContext(d);
	*cont = True;

#if XM_WITH_XRANDR
	if (scr->screen.randr_base != -1) {
		switch (ev->type - scr->screen.randr_base) {
		case RRScreenChangeNotify:
			change = (XRRScreenChangeNotifyEvent *)ev;
			XRRUpdateConfiguration(ev);

			_XmAppLock(app);
			scr->screen.width     = change->width;
			scr->screen.height    = change->height;
			scr->screen.width_mm  = change->mwidth;
			scr->screen.height_mm = change->mheight;
			set_dpi(&scr->screen);
			_XmAppUnlock(app);

		case RRNotify:
			notif = (XRRNotifyEvent *)ev;
			XRRUpdateConfiguration(ev);
			if (notif->subtype == RRNotify_CrtcChange ||
			    notif->subtype == RRNotify_OutputChange)
				monitors_randr(d, root, scr);
			break;
		}
	}
#endif

	if (ev->type != ConfigureNotify)
		goto done;

#if XM_WITH_XRANDR
	XRRUpdateConfiguration(ev);
#endif

#if XM_WITH_XINERAMA
	if (scr->screen.xinerama_base != -1 && XineramaIsActive(d))
		monitors_xinerama(d, root, scr);
#endif

	/**
	 * Handle size changes for the root window
	 */
	_XmAppLock(app);
	if (scr->screen.width != ev->xconfigure.width ||
	    scr->screen.height != ev->xconfigure.height) {
		scr->screen.width  = ev->xconfigure.width;
		scr->screen.height = ev->xconfigure.height;
	}
	_XmAppUnlock(app);

done:
#if XM_WITH_XRANDR || XM_WITH_XINERAMA
	if (!scr->screen.n_monitors)
#endif
		monitor_rootwin(d, root, scr);
}

/************************************************************************
 *
 *  ScreenInitialize
 *
 ************************************************************************/
static void Initialize(Widget requested_widget, Widget new_widget,
                       ArgList args, Cardinal *num_args)
{
#if XM_WITH_XRANDR || XM_WITH_XINERAMA
	int i, j, base;
#endif

	XmScreenInfo *info;
	XmScreen xmScreen;
	Display *display = XtDisplay(new_widget);
	Screen *screen   = XtScreen(new_widget);
	Window root      = RootWindowOfScreen(screen);

	if (XFindContext(display, root, screen_context, (XPointer *)&xmScreen) == XCSUCCESS) {
		XtError("Refusing to create an additional XmScreen");
		return;
	}

	xmScreen = (XmScreen)new_widget;
	XQueryBestCursor(display, root, 1024, 1024,
	                 &xmScreen->screen.maxCursorWidth,
	                 &xmScreen->screen.maxCursorHeight);

	xmScreen->desktop.parent = NULL;
	xmDesktopClass->core_class.initialize(NULL, new_widget, NULL, NULL);
	xmScreen->screen.screenInfo         = NULL;
	xmScreen->screen.nullCursor         = None;
	xmScreen->screen.cursorCache        = NULL;
	xmScreen->screen.scratchPixmaps     = _XmAllocHashTable(20, MatchPixmap, HashPixmap);
	xmScreen->screen.inUsePixmaps       = _XmAllocHashTable(20, NULL, NULL);
	xmScreen->screen.xmStateCursorIcon  = NULL;
	xmScreen->screen.xmMoveCursorIcon   = NULL;
	xmScreen->screen.xmCopyCursorIcon   = NULL;
	xmScreen->screen.xmLinkCursorIcon   = NULL;
	xmScreen->screen.xmSourceCursorIcon = NULL;
	xmScreen->screen.mwmPresent         = XmIsMotifWMRunning(new_widget);
	xmScreen->screen.numReparented      = 0;
	xmScreen->screen.width              = WidthOfScreen(screen);
	xmScreen->screen.height             = HeightOfScreen(screen);
	xmScreen->screen.width_mm           = WidthMMOfScreen(screen);
	xmScreen->screen.height_mm          = HeightMMOfScreen(screen);
	xmScreen->screen.user_dpi           = user_dpi(display);
	xmScreen->screen.randr_base         = -1;
	xmScreen->screen.xinerama_base      = -1;
	xmScreen->screen.n_monitors         = 0;
	xmScreen->screen.monitors           = NULL;
	set_dpi(&xmScreen->screen);

	if (!XmRepTypeValidValue(XmRID_UNPOST_BEHAVIOR,
	                         xmScreen->screen.unpostBehavior, new_widget))
		xmScreen->screen.unpostBehavior = XmUNPOST_AND_REPLAY;

	/**
	 * font unit stuff, priority to actual unit values, if they haven't
	 * been set yet, compute from the font, otherwise, stay unchanged
	 */
	if (xmScreen->screen.h_unit == RESOURCE_DEFAULT)
		GetUnitFromFont(display, xmScreen->screen.font_struct,
		                &xmScreen->screen.h_unit, NULL);

	if (xmScreen->screen.v_unit == RESOURCE_DEFAULT)
		GetUnitFromFont(display, xmScreen->screen.font_struct,
		                NULL, &xmScreen->screen.v_unit);

	info = (XmScreenInfo *)XtNew(XmScreenInfo);
	info->menu_state            = NULL;
	info->destroyCallbackAdded  = False;
	xmScreen->screen.screenInfo = info;

	/* Assume the identity of our root window for dispatching */
	xmScreen->core.window = root;
	XtRegisterDrawable(display, root, new_widget);
	XSaveContext(display, root, screen_context, (XPointer)xmScreen);

#if XM_WITH_XRANDR
	/**
	 * We're only interested in Xrandr >= 1.5, which was released nearly
	 * a decade ago. The monitors API keeps things simple.
	 */
	if (XRRQueryExtension(display, &base, &i)) {
		XRRQueryVersion(display, &i, &j);
		if ((i * 100 + j) < 105)
			base = -1;
	}

	if ((xmScreen->screen.randr_base = base) != -1) {
		XtSetEventDispatcher(display, base + RRNotify,             dispatch_randr);
		XtSetEventDispatcher(display, base + RRScreenChangeNotify, dispatch_randr);

		XtRegisterExtensionSelector(display,
		                            base + RRScreenChangeNotify,
		                            base + RRNotify, select_randr, NULL);
		XtInsertEventTypeHandler(new_widget, base + RRScreenChangeNotify,
		                         NULL, monitor_cb, NULL, XtListHead);
		XtInsertEventTypeHandler(new_widget, base + RRNotify,
		                         NULL, monitor_cb, NULL, XtListHead);
		monitors_randr(display, root, xmScreen);
	}
#endif

#if XM_WITH_XINERAMA
	/**
	 * Use Xinerama as a fallback if we lack Xrandr for some reason
	 */
	if (xmScreen->screen.randr_base == -1 && XineramaQueryExtension(display, &i, &j)) {
	    xmScreen->screen.xinerama_base = i;
		if (XineramaIsActive(display))
			monitors_xinerama(display, root, xmScreen);
	}
#endif

	if (!xmScreen->screen.n_monitors)
		monitor_rootwin(display, root, xmScreen);

#ifdef DEBUG
	dump_monitors(xmScreen);
#endif

	/* Install our glorious event handler */
	XtInsertEventHandler(new_widget, StructureNotifyMask, False,
	                     monitor_cb, NULL, XtListHead);
}

/**
 * Prevent modification of the icon if the new icon would be
 * on a Screen other than the current one.
 */
static void validate_icon_assignment(Widget w,    XmDragIconObject *new,
                                     Screen *scr, XmDragIconObject *old)
{
	if (*new && *new != *old && XtScreenOfObject(XtParent(new)) != scr) {
		XmeWarning(w, SCREEN_MISMATCH);
		*new = *old;
	}
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean SetValues(Widget current, Widget requested, Widget new_w,
                         ArgList args, Cardinal *num_args)
{
	Screen *scr      = XtScreen(new_w);
	XmScreen new     = (XmScreen)new_w;
	XmScreen old     = (XmScreen)current;
	Display *display = XtDisplay(new_w);

	if(!XmRepTypeValidValue(XmRID_UNPOST_BEHAVIOR,
	                        new->screen.unpostBehavior, new_w))
		new->screen.unpostBehavior = XmUNPOST_AND_REPLAY;

	validate_icon_assignment(new_w, &new->screen.defaultNoneCursorIcon,
	                         scr,   &old->screen.defaultNoneCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultValidCursorIcon,
	                         scr,   &old->screen.defaultValidCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultInvalidCursorIcon,
	                         scr,   &old->screen.defaultInvalidCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultMoveCursorIcon,
	                         scr,   &old->screen.defaultMoveCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultCopyCursorIcon,
	                         scr,   &old->screen.defaultCopyCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultLinkCursorIcon,
	                         scr,   &old->screen.defaultLinkCursorIcon);

	validate_icon_assignment(new_w, &new->screen.defaultSourceCursorIcon,
	                         scr,   &old->screen.defaultSourceCursorIcon);

	/**
	 * font unit stuff, priority to actual unit values, if the
	 * font has changed but not the unit values, report the change,
	 * otherwise, use the unit value - i.e do nothing
	 */
	if (new->screen.font_struct->fid != old->screen.font_struct->fid) {
		if (new->screen.h_unit == old->screen.h_unit) {
			GetUnitFromFont(display, new->screen.font_struct,
			                &new->screen.h_unit, NULL);
		}

		if (new->screen.v_unit == old->screen.v_unit) {
			GetUnitFromFont(display, new->screen.font_struct,
			                &new->screen.v_unit, NULL);
		}
	}

	new->screen.width      = old->screen.width;
	new->screen.height     = old->screen.height;
	new->screen.width_mm   = old->screen.width_mm;
	new->screen.height_mm  = old->screen.height_mm;
	new->screen.dpi        = old->screen.dpi;
	new->screen.n_monitors = old->screen.n_monitors;
	new->screen.monitors   = old->screen.monitors;
	return False;
}

/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
static void Destroy(Widget widget)
{
	Cardinal i;
	XmScreen xscr = (XmScreen)widget;
	XmDragCursorCache cache, prev;
	XmHashTable ht;
	Window w = xscr->core.window;

	/* Remove our callback */
#if XM_WITH_XRANDR
	XtRemoveEventTypeHandler(widget,
	                         xscr->screen.randr_base + RRScreenChangeNotify,
	                         NULL, monitor_cb, NULL);
	XtRemoveEventTypeHandler(widget,
	                         xscr->screen.randr_base + RRNotify,
	                         NULL, monitor_cb, NULL);
#endif
	XtRemoveEventHandler(widget, StructureNotifyMask, False, monitor_cb, NULL);
	XtUnregisterDrawable(XtDisplay(w), w);
	xscr->core.window = None;

	/* destroy any default icons created by Xm */
	_XmDestroyDefaultDragIcon(xscr->screen.xmStateCursorIcon);
	_XmDestroyDefaultDragIcon(xscr->screen.xmMoveCursorIcon);
	_XmDestroyDefaultDragIcon(xscr->screen.xmCopyCursorIcon);
	_XmDestroyDefaultDragIcon(xscr->screen.xmLinkCursorIcon);
	_XmDestroyDefaultDragIcon(xscr->screen.xmSourceCursorIcon);

	/* free the cursorCache and the pixmapCache */
	cache = xscr->screen.cursorCache;
	while (cache) {
		prev = cache;
		if (cache->cursor != None)
			XFreeCursor(XtDisplay(xscr), cache->cursor);
		cache = cache->next;
		XtFree((XtPointer)prev);
	}

	_XmProcessLock();
	ht = (XmHashTable)xscr->screen.scratchPixmaps;
	_XmMapHashTable(ht, FreePixmap, xscr);
	_XmFreeHashTable(ht);
	_XmFreeHashTable((XmHashTable)xscr->screen.inUsePixmaps);
	_XmProcessUnlock();

	/* need to remove pixmap and GC related to this screen */
	_XmCleanPixmapCache(XtScreen(xscr), NULL);
	XtFree((XtPointer)xscr->screen.screenInfo);
	xmDesktopClass->core_class.destroy(widget);

	for (i = 0; i < xscr->screen.n_monitors; i++)
		XFree((XPointer)xscr->screen.monitors[i].name);
	XtFree((XtPointer)xscr->screen.monitors);

	if (XFindContext(XtDisplay(w), w, screen_context, (XPointer *)&xscr) == XCSUCCESS)
		XDeleteContext(XtDisplay(w), w, screen_context);
}

/************************************************************************
 *
 *  _XmScreenRemoveFromCursorCache
 *
 *  Mark any cursor cache reference to the specified icon as deallocated.
 ************************************************************************/
void _XmScreenRemoveFromCursorCache(XmDragIconObject icon)
{
	XmScreen scr = XmScreenOfObject(icon);
	XmDragCursorCache i    = scr->screen.cursorCache;
	XmDragCursorCache prev, next;

	prev = i;
	while (i) {
		next = i->next;
		if (i->sourceIcon == icon || i->stateIcon == icon || i->opIcon == icon) {
			if (i->cursor)
				XFreeCursor(XtDisplay(icon), i->cursor);

			if (i == scr->screen.cursorCache)
				scr->screen.cursorCache = next;
			else prev->next = i->next;
			XtFree((XtPointer)i);
		} else prev = i;

		i = next;
	}
}

static Boolean FreePixmap(XmHashKey k, XtPointer p, XtPointer client)
{
	XtFree((XtPointer)k);
	XFreePixmap(XtDisplay((Widget)client), (Pixmap)p);
	return False;
}

/************************************************************************
 *
 *  _XmScreenGetOperationIcon ()
 *
 *  Returns one of the three XmScreen operation cursor types. These aren't
 *  created ahead of time in order to let the client specify its own.
 *  If they haven't by now (a drag is in process) then we create our
 *  own. The name of the OperatonIcon can cause the built-in cursor data
 *  to get loaded in (if not specified in the resource file).
 ************************************************************************/
XmDragIconObject _XmScreenGetOperationIcon(Widget w, unsigned char operation)
{
	XrmQuark name = NULLQUARK;
	XmScreen scr  = XmScreenOfObject(w);
	XmDragIconObject *p1 = NULL, *p2 = NULL;

	switch (operation) {
	case XmDROP_MOVE:
		p1   = &scr->screen.defaultMoveCursorIcon;
		p2   = &scr->screen.xmMoveCursorIcon;
		name = _XmMoveCursorIconQuark;
		break;
	case XmDROP_COPY:
		p1   = &scr->screen.defaultCopyCursorIcon;
		p2   = &scr->screen.xmCopyCursorIcon;
		name = _XmCopyCursorIconQuark;
		break;
	case XmDROP_LINK:
		p1   = &scr->screen.defaultLinkCursorIcon;
		p2   = &scr->screen.xmLinkCursorIcon;
		name = _XmLinkCursorIconQuark;
		break;
	default:
		return NULL;
	}

	if (!*p1) {
		if (!*p2) {
			*p2 = (XmDragIconObject)XmCreateDragIcon(
				(Widget)scr, XrmQuarkToString(name), NULL, 0
			);
		}

		*p1 = *p2;
	}

    return *p1;
}

/************************************************************************
 *
 *  _XmScreenGetStateIcon ()
 *
 *  Returns one of the three XmScreen state cursor types. These aren't
 *  created ahead of time in order to let the client specify its own.
 *  If they haven't by now (a drag is in process) then we create our
 *  own. The name of the StateIcon can cause the built-in cursor data
 *  to get loaded in (if not specified in the resource file).
 *  The default state cursors are the same XmDragIcon object.
 ************************************************************************/
XmDragIconObject _XmScreenGetStateIcon(Widget w, unsigned char state)
{
	XrmQuark name = NULLQUARK;
	XmScreen scr  = XmScreenOfObject(w);
	XmDragIconObject icon = NULL, tmp;

	switch (state) {
	default:
	case XmNO_DROP_SITE:
		icon = scr->screen.defaultNoneCursorIcon;
		name = _XmNoneCursorIconQuark;
		break;
	case XmVALID_DROP_SITE:
		icon = scr->screen.defaultValidCursorIcon;
		name = _XmValidCursorIconQuark;
		break;
	case XmINVALID_DROP_SITE:
		icon = scr->screen.defaultInvalidCursorIcon;
		name = _XmInvalidCursorIconQuark;
		break;
	}

	if (icon)
		return icon;

	/**
	 * We need to create the default state icons.
	 * Set all uncreated state icons to the same XmDragIcon object.
	 */
	if (!(icon = scr->screen.xmStateCursorIcon)) {
		icon = (XmDragIconObject)XmCreateDragIcon(
			(Widget)scr, XrmQuarkToString(name), NULL, 0
		);
	}

	scr->screen.xmStateCursorIcon = icon;
	if (!scr->screen.defaultNoneCursorIcon)
		scr->screen.defaultNoneCursorIcon = icon;
	if (!scr->screen.defaultValidCursorIcon)
		scr->screen.defaultValidCursorIcon = icon;
	if (!scr->screen.defaultInvalidCursorIcon)
		scr->screen.defaultInvalidCursorIcon = icon;

	return icon;
}

/************************************************************************
 *
 *  _XmScreenGetSourceIcon ()
 *
 *  Returns the XmScreen source cursor types.  This isn't created ahead of
 *  time in order to let the client specify its own.  If it hasn't by now
 *  (a drag is in process) then we create our own.
 ************************************************************************/
XmDragIconObject _XmScreenGetSourceIcon(Widget w)
{
	XmDragIconObject icon;
	XmScreen scr = XmScreenOfObject(w);

	if (scr->screen.defaultSourceCursorIcon)
		return scr->screen.defaultSourceCursorIcon;

	if (!scr->screen.xmSourceCursorIcon) {
		icon = (XmDragIconObject)XmCreateDragIcon(
			(Widget)scr, XrmQuarkToString(_XmDefaultDragIconQuark),
			NULL, 0
		);
		scr->screen.xmSourceCursorIcon = icon;
	}

	scr->screen.defaultSourceCursorIcon = scr->screen.xmSourceCursorIcon;
	return scr->screen.defaultSourceCursorIcon;
}

/************************************************************************
 *
 *  _XmAllocScratchPixmap
 *
 ************************************************************************/
static Boolean MatchPixmap(XmHashKey a, XmHashKey b)
{
	XmScratchPixmapKey ka = (XmScratchPixmapKey)a;
	XmScratchPixmapKey kb = (XmScratchPixmapKey)b;
	return (ka->height == kb->height &&
	        ka->width  == kb->width  &&
	        ka->depth  == kb->depth);
}

static XmHashValue HashPixmap(XmHashKey x)
{
	XmScratchPixmapKey k = (XmScratchPixmapKey)x;
	return k->height + k->width + k->depth;
}

Pixmap _XmAllocScratchPixmap(XmScreen scr, Cardinal depth,
                             Dimension width, Dimension height)
{
	Pixmap p = None;
	XmHashTable scratch = (XmHashTable)scr->screen.scratchPixmaps;
	XmHashTable in_use  = (XmHashTable)scr->screen.inUsePixmaps;
	XmScratchPixmapKey key;
	XmScratchPixmapKeyRec k;

	k.width  = width;
	k.height = height;
	k.depth  = depth;

	_XmProcessLock();
	if ((p = (Pixmap)_XmGetHashEntry(scratch, &k)) == None) {
		key = (XmScratchPixmapKey)XtNew(XmScratchPixmapKeyRec);
		key->width  = width;
		key->height = height;
		key->depth  = depth;
		p = XCreatePixmap(XtDisplay(scr),
		                  RootWindowOfScreen(XtScreen(scr)),
		                  width, height, depth);
	} else key = (XmScratchPixmapKey)_XmRemoveHashEntry(scratch, &k);

	/* Place key in in_use table */
	_XmAddHashEntry(in_use,(XmHashKey)p, key);
	_XmProcessUnlock();
	return p;
}

/************************************************************************
 *
 *  _XmFreeScratchPixmap
 *
 ************************************************************************/
void _XmFreeScratchPixmap(XmScreen scr, Pixmap pixmap)
{
	XmScratchPixmapKey key;
	XmHashTable scratch = (XmHashTable)scr->screen.scratchPixmaps;
	XmHashTable in_use  = (XmHashTable)scr->screen.inUsePixmaps;

	_XmProcessLock();
	if (!(key = (XmScratchPixmapKey)_XmGetHashEntry(in_use, (XmHashKey)pixmap))) {
		_XmProcessUnlock();
		return;
	}

	_XmRemoveHashEntry(in_use, (XmHashKey)pixmap);
	_XmAddHashEntry(scratch, key, (XtPointer)pixmap);
	_XmProcessUnlock();
}

/************************************************************************
 *
 *  _XmGetDragCursorCachePtr ()
 *
 ************************************************************************/
XmDragCursorCache *_XmGetDragCursorCachePtr(XmScreen scr)
{
	return &scr->screen.cursorCache;
}

/************************************************************************
 *
 *  XmeQueryBestCursorSize()
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

static const char null_bits[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

/************************************************************************
 *
 *  XmeGetNullCursor ()
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

/*
 * The following set of functions support the menu cursor functionality.
 * They have moved from MenuUtil to here.
 */

/* Obsolete global per-display menu cursor manipulation functions */
/* Programs have to use XtSet/GetValues on the XmScreen object instead */
void XmSetMenuCursor(Display *display, Cursor cursorId)
{
	int i;

	_XmDisplayToAppContext(display);
	_XmAppLock(app);

	/**
	 * This function has no screen parameter, so we have to set the
	 * menucursor for _all_ the xmscreen available on this display. why?
	 * because when RowColumn will be getting a menucursor for a particular
	 * screen, it will have to get what the application has set using
	 * this function, not the default for that particular screen (which is
	 * what will happen if we were only setting the default display here)
	 */
	for (i = 0; i < ScreenCount(display); i++)
		XmScreenOfScreen(ScreenOfDisplay(display, i))->screen.menuCursor = cursorId;
	_XmAppUnlock(app);
}

Cursor XmGetMenuCursor(Display *display)
{
	Cursor c;
	XmScreen scr;

	_XmDisplayToAppContext(display);
	_XmAppLock(app);

	/**
	 * get the default screen menuCursor since there is no
	 * other information available to this function
	 */
	scr = XmScreenOfScreen(DefaultScreenOfDisplay(display));
	c   = scr->screen.menuCursor;
	_XmAppUnlock(app);
	return c;
}

/* a convenience for RowColumn */
Cursor _XmGetMenuCursorByScreen(Screen *screen)
{
	return XmScreenOfScreen(screen)->screen.menuCursor;
}

Boolean _XmGetMoveOpaqueByScreen(Screen *screen)
{
	return XmScreenOfScreen(screen)->screen.moveOpaque;
}

/* a convenience for RowColumn */
unsigned char _XmGetUnpostBehavior(Widget w)
{
	return XmScreenOfObject(w)->screen.unpostBehavior;
}

/**********************************************************************
 **********************************************************************

      Font unit handling functions

 **********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  XmSetFontUnits
 *    Set the font_unit value for all screens.  These values can
 *    then be used later to process the font unit conversions.
 *
 **********************************************************************/
static void _XmSetFontUnits(Display *display, int h_value, int v_value)
{
	int i;
	Screen *scr;
	XmScreen xscr;

	_XmDisplayToAppContext(display);
	_XmAppLock(app);

	/**
	 * This function has no screen parameter, so we have to set the
	 * fontunit for _all_ the xmscreen available on this display. why?
	 * because when someone will be getting fontunits for a particular
	 * screen, it will have to get what the application has set using
	 * this function, not the default for that particular screen (which is
	 * what will happen if we were only setting the default display here)
	 */
	for (i = 0; i < ScreenCount(display); i++) {
		scr  = ScreenOfDisplay(display, i);
		xscr = XmScreenOfScreen(scr);
		xscr->screen.h_unit = h_value;
		xscr->screen.v_unit = v_value;
	}

	_XmAppUnlock(app);
}

/* DEPRECATED */
void XmSetFontUnits(Display *display, int h_value, int v_value)
{
	_XmSetFontUnits(display, h_value, v_value);
}

/* DEPRECATED */
void XmSetFontUnit(Display *display, int value)
{
	_XmSetFontUnits(display, value, value);
}

/**********************************************************************
 *
 *  _XmGetFontUnit
 *
 **********************************************************************/
int _XmGetFontUnit(Screen *screen, int dimension)
{
	XmScreen scr = XmScreenOfScreen(screen);
	return (dimension == XmHORIZONTAL) ? scr->screen.h_unit : scr->screen.v_unit;
}

/**********************************************************************
 *
 *  _XmGetColorCalculationProc
 *     Here there is no point of supporting the old function as
 *     place holder for the new color proc since the signature are
 *     different. The old color proc will still be managed by Visual.c
 *     in its own way.
 **********************************************************************/
XmScreenColorProc _XmGetColorCalculationProc(Screen *screen)
{
    return XmScreenOfScreen(screen)->screen.color_calc_proc;
}

/**********************************************************************
 *
 *  _XmGetColorAllocationProc
 *
 **********************************************************************/
XmAllocColorProc _XmGetColorAllocationProc(Screen *screen)
{
    return XmScreenOfScreen(screen)->screen.color_alloc_proc;
}

/**********************************************************************
 *
 *  _XmGetBitmapConversionModel
 *
 **********************************************************************/
XtEnum _XmGetBitmapConversionModel(Screen *screen)
{
    return XmScreenOfScreen(screen)->screen.bitmap_conversion_model;
}

/************************************************************************
 *
 * _XmGetInsensitiveStippleBitmap
 *
 * Returns the insensitive_stipple_bitmap field of the XmScreenPart
 ************************************************************************/
Pixmap _XmGetInsensitiveStippleBitmap(Widget w)
{
    return XmScreenOfObject(w)->screen.insensitive_stipple_bitmap;
}

/*********************************************************************
 *
 *  XmGetXmScreen
 *
 *********************************************************************/
Widget XmGetXmScreen(Screen *screen)
{
	int i;
	Arg arg;
	Cardinal c;
	XmDisplay d;
	Widget w, child;
	WidgetList children;
	char name[32];

	_XmDisplayToAppContext(DisplayOfScreen(screen));
	_XmAppLock(app);

	if (screen_context != None && /* class might be yet uninitialized */
	    XFindContext(DisplayOfScreen(screen), RootWindowOfScreen(screen),
	                 screen_context, (XPointer *)&child) == XCSUCCESS) {
		_XmAppUnlock(app);
		return child;
	}

	if (!(d = (XmDisplay)XmGetXmDisplay(DisplayOfScreen(screen)))) {
		XmeWarning(NULL, CANT_FIND_DISPLAY);
		_XmAppUnlock(app);
		return NULL;
	}

	for (c = 0; c < d->composite.num_children; c++) {
		child = d->composite.children[c];
		if (XmIsScreen(child) && screen == XtScreen(child)) {
			_XmAppUnlock(app);
			return child;
		}
	}

	/* If we can't find it, create it */
	for (i = 0; i < ScreenCount(XtDisplay(d)); i++) {
		if (screen == ScreenOfDisplay(XtDisplay(d), i))
			break;
	}

	sprintf(name, "screen%d", i);
	XtSetArg(arg, XmNscreen, screen);
	w = XtCreateWidget(name, xmScreenClass, (Widget)d, &arg, 1);
	_XmAppUnlock(app);
	return w;
}

/**
 * Returns a copy of the XmMonitorInfo for the monitor that contains
 * the given point; or NULL if the given point is off-screen.
 *
 * The returned info must be freed with FreeXmMonitorInfo().
 */
XmMonitorInfo *XmGetMonitorInfoAt(XmScreen screen, Position x, Position y)
{
	Cardinal i;
	XmMonitorInfo *s, *out;

	for (i = 0; i < screen->screen.n_monitors; i++) {
		s = screen->screen.monitors + i;
		if (x < s->x || x >= s->x + s->width ||
		    y < s->y || y >= s->y + s->height)
			continue;

		out = XtNew(XmMonitorInfo);
		memcpy(out, s, sizeof *s);

		if (s->name) {
			out->name = XtMalloc(strlen(s->name) + 1);
			strcpy(out->name, s->name);
		}

	    return out;
	}

	return NULL;
}

void FreeXmMonitorInfo(XmMonitorInfo *info)
{
	XtFree((XtPointer)info->name);
	XtFree((XtPointer)info);
}

