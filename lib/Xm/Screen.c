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
#include <string.h>
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

/************************************************************************
 *
 *  ScreenInitialize
 *
 ************************************************************************/
static void Initialize(Widget requested_widget, Widget new_widget,
                       ArgList args, Cardinal *num_args)
{
	XmScreenInfo *info;
	XmScreen xmScreen = (XmScreen)new_widget;
	Display *display  = XtDisplay(new_widget);

	((XmDesktopObject)xmScreen)->desktop.parent = NULL;
	xmDesktopClass->core_class.initialize(NULL, new_widget, NULL, NULL);
	XQueryBestCursor(display, RootWindowOfScreen(XtScreen(xmScreen)),
	                 1024, 1024, &xmScreen->screen.maxCursorWidth,
	                 &xmScreen->screen.maxCursorHeight);

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

	if (!(info = (XmScreenInfo *)XtNew(XmScreenInfo)))
		return;

	info->menu_state            = NULL;
	info->destroyCallbackAdded  = False;
	xmScreen->screen.screenInfo = info;
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

	return FALSE;
}

/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
static void Destroy(Widget widget)
{
	XmScreen xmScreen = (XmScreen)widget;
	XmDragCursorCache cache, prev;
	XmHashTable ht;

	/* destroy any default icons created by Xm */
	_XmDestroyDefaultDragIcon(xmScreen->screen.xmStateCursorIcon);
	_XmDestroyDefaultDragIcon(xmScreen->screen.xmMoveCursorIcon);
	_XmDestroyDefaultDragIcon(xmScreen->screen.xmCopyCursorIcon);
	_XmDestroyDefaultDragIcon(xmScreen->screen.xmLinkCursorIcon);
	_XmDestroyDefaultDragIcon(xmScreen->screen.xmSourceCursorIcon);

	/* free the cursorCache and the pixmapCache */
	cache = xmScreen->screen.cursorCache;
	while (cache) {
		prev = cache;
		if (cache->cursor != None)
			XFreeCursor(XtDisplay(xmScreen), cache->cursor);
		cache = cache->next;
		XtFree((XtPointer)prev);
	}

	_XmProcessLock();
	ht = (XmHashTable)xmScreen->screen.scratchPixmaps;
	_XmMapHashTable(ht, FreePixmap, xmScreen);
	_XmFreeHashTable(ht);
	_XmFreeHashTable((XmHashTable)xmScreen->screen.inUsePixmaps);
	_XmProcessUnlock();

	/* need to remove pixmap and GC related to this screen */
	_XmCleanPixmapCache(XtScreen(widget), NULL);
	XtFree((XtPointer)xmScreen->screen.screenInfo);
	xmDesktopClass->core_class.destroy(widget);
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

