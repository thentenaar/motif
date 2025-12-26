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
static char rcsid[] = "$XConsortium: Sash.c /main/12 1995/07/13 17:51:55 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/cursorfont.h>
#include <Xm/SashP.h>
#include <Xm/TransltnsP.h>
#include <Xm/DrawP.h>
#include <Xm/DisplayP.h>
#include <Xm/Screen.h>

#include "XmI.h"
#include "TraversalI.h"

#define SASHSIZE 10

static void ClassPartInitialize(WidgetClass wc);
static void ClassInitialize(void);
static void Initialize(Widget rw, Widget nw, ArgList args, Cardinal *num_args);
static void HighlightSash(Widget w);
static void UnhighlightSash(Widget w);
static XmNavigability WidgetNavigable(Widget w);
static void SashFocusIn(Widget w, XEvent *event, String *params,
                        Cardinal *num_params);
static void SashFocusOut(Widget w, XEvent *event, String *params,
                         Cardinal *num_params);
static void SashAction(Widget widget, XEvent *event, String *params,
                       Cardinal *num_params);
static void Realize(Widget w, XtValueMask *p_valueMask,
                    XSetWindowAttributes *attributes);
static void SashDisplayDestroyCallback(Widget w, XtPointer client, XtPointer call);
static void Redisplay(Widget w, XEvent *event, Region region);

/***************************************************************************
 *
 * Resources
 *
 ***************************************************************************/
#define Offset(x) (XtOffsetOf(XmSashRec, x))
static XtResource resources[] = {
	{
		XmNborderWidth, XmCBorderWidth, XmRHorizontalDimension,
		sizeof(Dimension), Offset(core.border_width),
		XmRImmediate, NULL
	},
	{
		XmNcallback, XmCCallback, XmRCallback,
		sizeof(XtCallbackList), Offset(sash.sash_action),
		XmRPointer, NULL
	},
	{
		XmNnavigationType, XmCNavigationType, XmRNavigationType,
		sizeof(char), Offset(primitive.navigation_type),
		XmRImmediate, (XtPointer)XmSTICKY_TAB_GROUP
	},

};
#undef Offset

static XtActionsRec actions[] = {
	{ "SashAction",   SashAction   },
	{ "SashFocusIn",  SashFocusIn  },
	{ "SashFocusOut", SashFocusOut }
};

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
	NULL,                 /* InitializePrehook      */
	NULL,                 /* SetValuesPrehook       */
	NULL,                 /* InitializePosthook     */
	NULL,                 /* SetValuesPosthook      */
	NULL,                 /* secondaryObjectClass   */
	NULL,                 /* secondaryCreate        */
	NULL,                 /* getSecRes data         */
	{ 0 },                /* fastSubclass flags     */
	NULL,                 /* getValuesPrehook       */
	NULL,                 /* getValuesPosthook      */
	NULL,                 /* classPartInitPrehook   */
	NULL,                 /* classPartInitPosthook  */
	NULL,                 /* ext_resources          */
	NULL,                 /* compiled_ext_resources */
	0,                    /* num_ext_resources      */
	FALSE,                /* use_sub_resources      */
	WidgetNavigable,      /* widgetNavigable        */
	XmInheritFocusChange, /* focusChange            */
	NULL                  /* wrapperData            */
};

externaldef(xmsashclassrec) XmSashClassRec xmSashClassRec = {
{ /* core */
	(WidgetClass)&xmPrimitiveClassRec,  /* superclass          */
	"XmSash",                           /* class_name          */
	sizeof(XmSashRec),                  /* size                */
	ClassInitialize,                    /* Class Initializer   */
	ClassPartInitialize,                /* class_part_init     */
	FALSE,                              /* Class init'ed ?     */
	Initialize,                         /* initialize          */
	NULL,                               /* initialize_notify   */
	Realize,                            /* realize             */
	actions,                            /* actions             */
	XtNumber(actions),                  /* num_actions         */
	resources,                          /* resources           */
	XtNumber(resources),                /* resource_count      */
	NULLQUARK,                          /* xrm_class           */
	TRUE,                               /* compress_motion     */
	XtExposeCompressMaximal,            /* compress_exposure   */
	TRUE,                               /* compress_enterleave */
	FALSE,                              /* visible_interest    */
	NULL,                               /* destroy             */
	NULL,                               /* resize              */
	Redisplay,                          /* expose              */
	NULL,                               /* set_values          */
	NULL,                               /* set_values_hook     */
	XtInheritSetValuesAlmost,           /* set_values_almost   */
	NULL,                               /* get_values_hook     */
	NULL,                               /* accept_focus        */
	XtVersion,                          /* intrinsics version  */
	NULL,                               /* callback offsets    */
	_XmSash_defTranslations,            /* tm_table            */
	NULL,                               /* query_geometry      */
	NULL,                               /* screen_accelerator  */
	(XtPointer)&baseClassExtRec,        /* extension           */
},
{ /* primitive */
	XmInheritWidgetProc,                /* border_highlight    */
	XmInheritWidgetProc,                /* border_unhighlight  */
	NULL,                               /* translations        */
	NULL,                               /* arm_and_activate    */
	NULL,                               /* syn_resources       */
	0,                                  /* num_syn_resources   */
	NULL,                               /* extension           */
},
{ /* sash */
	NULL,                               /* extension           */
}};

externaldef(xmsashwidgetclass) WidgetClass xmSashWidgetClass =
	(WidgetClass)&xmSashClassRec;

/************************************************************************
 *
 *  ClassPartInitialize
 *    Set up the fast subclassing for the widget.
 *
 ************************************************************************/
static void ClassPartInitialize(WidgetClass wc)
{
	_XmFastSubclassInit(wc, XmSASH_BIT);
}

/************************************************************************
 *
 *  ClassInitialize
 *    Initialize the primitive part of class structure with
 *    routines to do special highlight & unhighlight for Sash.
 *
 ************************************************************************/
static void ClassInitialize(void)
{
	xmSashClassRec.primitive_class.border_highlight   = HighlightSash;
	xmSashClassRec.primitive_class.border_unhighlight = UnhighlightSash;
	baseClassExtRec.record_type                       = XmQmotif;
}

static void Initialize(Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
	Dimension size;
	XmSashWidget req = (XmSashWidget)rw;
	XmSashWidget new = (XmSashWidget)nw;
	(void)args;
	(void)num_args;

	size = (Dimension)(4 + 6 * XmScreenDpi(XmScreenOfObject(new))/96.);
	if (!req->core.width)  new->core.width  = size;
	if (!req->core.height) new->core.height = size;
	new->sash.has_focus = False;
}

static void HighlightSash(Widget w)
{
	int x, y;
	XmSashWidget sash = (XmSashWidget)w;

	x = y = sash->primitive.shadow_thickness;
	XFillRectangle(XtDisplay(w), XtWindow(w), sash->primitive.highlight_GC,
	               x, y, sash->core.width - (2 * x), sash->core.height - (2 * y));
}

static void UnhighlightSash(Widget w)
{
	int x, y;
	XmSashWidget sash = (XmSashWidget)w;

	x = y = sash->primitive.shadow_thickness;
	XClearArea(XtDisplay(w), XtWindow(w), x, y,
	           sash->core.width - (2 * x), sash->core.height - (2 * y),
	           False);
}

static XmNavigability WidgetNavigable(Widget w)
{
	XmNavigationType nav;
	XmPrimitiveWidget prim = (XmPrimitiveWidget)w;

	/* Preserve 1.0 behavior.  (Why?  Don't ask me!) */
	if (_XmShellIsExclusive(w))
		return XmNOT_NAVIGABLE;

	if (!XtIsSensitive(w) || !prim->primitive.traversal_on)
		return XmNOT_NAVIGABLE;

	nav = prim->primitive.navigation_type;
	if (nav == XmSTICKY_TAB_GROUP || nav == XmEXCLUSIVE_TAB_GROUP || nav == XmTAB_GROUP)
		return XmTAB_NAVIGABLE;

	return XmNOT_NAVIGABLE;
}

static void SashFocusIn(Widget w, XEvent *event, String *params,
                        Cardinal *num_params)
{
	XmSashWidget sash = (XmSashWidget)w;

	if (event->xany.type != FocusIn || !event->xfocus.send_event)
		return;

	if (_XmGetFocusPolicy(w) == XmEXPLICIT)
		HighlightSash(w);

	XmeDrawShadows(XtDisplay(w), XtWindow(w),
	               sash->primitive.top_shadow_GC,
	               sash->primitive.bottom_shadow_GC,
	               0, 0, w->core.width, w->core.height,
	               sash->primitive.shadow_thickness, XmSHADOW_OUT);
	sash->sash.has_focus = True;
}

static void SashFocusOut(Widget w, XEvent *event, String *params,
                         Cardinal *num_params)
{
	XmSashWidget sash = (XmSashWidget)w;

	if (event->xany.type != FocusOut || !event->xfocus.send_event)
		return;

	if (_XmGetFocusPolicy(w) == XmEXPLICIT)
		UnhighlightSash(w);

	XmeDrawShadows(XtDisplay(w), XtWindow(w),
	               sash->primitive.top_shadow_GC,
	               sash->primitive.bottom_shadow_GC,
	               0, 0, w->core.width, w->core.height,
	               sash->primitive.shadow_thickness, XmSHADOW_OUT);
	sash->sash.has_focus = False;
}

static void SashAction(Widget widget, XEvent *event, String *params,
                       Cardinal *num_params)
{
	struct XmSashCallbackData call_data;
	XmSashWidget sash = (XmSashWidget)widget;

	call_data.event      = event;
	call_data.params     = params;
	call_data.num_params = num_params ? *num_params : 0;
	XtCallCallbackList(widget, sash->sash.sash_action, (XtPointer)&call_data);
}

static void Realize(Widget w, XtValueMask *p_valueMask, XSetWindowAttributes *attributes)
{
	Display *d        = XtDisplay(w);
	XmDisplay dd      = (XmDisplay)XmGetXmDisplay(d);
	Cursor SashCursor = ((XmDisplayInfo *)(dd->display.displayInfo))->SashCursor;

	_XmDisplayToAppContext(d);
	if (!SashCursor) {
		/**
		 * create some data shared among all instances on this
		 * display; the first one along can create it, and
		 * any one can remove it; note no reference count
		 */
		SashCursor = XCreateFontCursor(XtDisplay(w), XC_crosshair);
		XtAddCallback((Widget)dd, XtNdestroyCallback, SashDisplayDestroyCallback, NULL);

		_XmAppLock(app);
		((XmDisplayInfo *)(dd->display.displayInfo))->SashCursor = SashCursor;
		_XmAppUnlock(app);
	}

	attributes->cursor = SashCursor;
	XtCreateWindow(w, InputOutput, CopyFromParent, *p_valueMask | CWCursor, attributes);
}

static void SashDisplayDestroyCallback(Widget w, XtPointer client, XtPointer call)
{
	Cursor cursor;
	Display *d   = XtDisplay(w);
	XmDisplay dd = (XmDisplay)XmGetXmDisplay(d);
	(void)client;
	(void)call;

	_XmDisplayToAppContext(d);
	if (dd && (cursor = ((XmDisplayInfo *)(dd->display.displayInfo))->SashCursor)) {
		_XmAppLock(app);
		((XmDisplayInfo *)(dd->display.displayInfo))->SashCursor = None;
		XFreeCursor(d, cursor);
		_XmAppUnlock(app);
	}
}

/*************************************<->*************************************
 *
 *  Redisplay (w, event)
 *
 *   Description:
 *   -----------
 *     Cause the widget, identified by w, to be redisplayed.
 *
 *
 *   Inputs:
 *   ------
 *     w = widget to be redisplayed;
 *     event = event structure identifying need for redisplay on this
 *             widget.
 *
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *************************************<->***********************************/
static void Redisplay(Widget w, XEvent *event, Region region)
{
	XmSashWidget sash = (XmSashWidget)w;

	XmeDrawShadows(XtDisplay(w), XtWindow(w),
	               sash->primitive.top_shadow_GC,
	               sash->primitive.bottom_shadow_GC,
	               0, 0, w->core.width, w->core.height,
	               sash->primitive.shadow_thickness, XmSHADOW_OUT);

	if (sash->sash.has_focus)
		HighlightSash(w);
}

