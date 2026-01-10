/**
 * Motif
 *
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

/************************************************************
*	INCLUDE FILES
*************************************************************/

#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include <Xm/TraitP.h>
#include <Xm/ActivatableT.h>
#include <stdio.h>
#include <Xm/IconButtonP.h>
#include <X11/bitmaps/gray>
#include <Xm/ExtP.h>
#include "PrimitiveI.h"
#include "XmStrDefsI.h"
#include "xmlist.h"
#include "RepTypeI.h"

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#define SUPERCLASS ((WidgetClass) &xmPrimitiveClassRec)

#define FILL_SPACE(iw)		((iw)->primitive.highlight_thickness + \
                                 (iw)->primitive.shadow_thickness)

#define H_SPACE(iw)		(FILL_SPACE(iw) + XmIconButton_h_space(iw))
#define V_SPACE(iw)		(FILL_SPACE(iw) + XmIconButton_v_space(iw))

#define ALLOC_INCREMENT         5
#define DELAY			100

#define NO_TIMER		0

#define NEW_LINE  		('\n')
#define EOL       		('\0')

typedef struct _PixCacheEntry {
    Display *display;
    Pixmap pixmap;
    long count;			/*The tree widget uses the folder icon A LOT*/
    Dimension width, height, depth;
} PixCacheEntry;

/************************************************************
*	MACROS
*************************************************************/

#define streq(a, b) (((a) != NULL) && ((b) != NULL) && (strcmp((a), (b)) == 0))
#define ValidPixmap(p) ((p)!=None&&(p)!=XmUNSPECIFIED_PIXMAP)

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

extern void _XmSelectColorDefault(Widget widget, int offset, XrmValue *value);

static XmList pix_cache_list = NULL;

/************************************************************
*	STATIC FUNCTION DECLARATIONS
*************************************************************/

static void Resize(Widget), Destroy(Widget), ClassInit(void);
static void ClassPartInitialize(WidgetClass w_class);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static XtGeometryResult QueryGeometry(Widget,
				      XtWidgetGeometry *, XtWidgetGeometry *);

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void SetValuesAlmost(Widget, Widget,
			    XtWidgetGeometry *,  XtWidgetGeometry *);

static void GetValues_labelString ( Widget w, int n, XtArgVal *value) ;
static void ChangeCB(Widget w, XtCallbackProc activCB, XtPointer closure, Boolean setunset);

/************************
 * Actions and callbacks.
 ************************/

static void ToggleState(Widget, XEvent *, String *, Cardinal *);
static void Notify(Widget, XEvent *, String *, Cardinal *);
static void DoubleNotify(Widget, XEvent *, String *, Cardinal *);
static void ArmAndActivate(Widget, XEvent *, String *, Cardinal *);
static void ButtonUp(Widget, XEvent *, String *, Cardinal *);

/*********************
 * Internal Routines.
 *********************/

static XmListElem * GetCacheElem(Display *, Pixmap);
static Boolean CheckPixCache(Display *, Pixmap,
			     unsigned int *, unsigned int *, unsigned int *);
static void AddPixCache(Display *, Pixmap,
			unsigned int, unsigned int, unsigned int);
static void IncPixCache(Display *, Pixmap);
static void DecPixCache(Display *, Pixmap);

static void Deactivate(XtPointer, XtIntervalId *);
static void GetDesiredSize(Widget, Dimension *, Dimension *);
static void RequestNewSize(Widget, Dimension, Dimension);
static void CalcLocations(Widget), DrawTextAndImage(Widget, GC, GC, GC);
static void CreateGCs(Widget), DestroyGCs(Widget w);

static void FromPaddingPixels(Widget, int, XtArgVal *);
static XmImportOperator ToPaddingPixels(Widget, int, XtArgVal *);
static XmString CreateXmString(Widget, String);
static void CheckSetRenderTable(Widget wid, int offs, XrmValue *value);

/******************
 * Type Converters
 ******************/

static Boolean CvtStringToIconPlacement(Display *, XrmValuePtr, Cardinal *,
					XrmValuePtr, XrmValuePtr);

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

/*
 * It seems redundant to have both Btn1Up and BtnUp bound to
 * EndDrag, but for some reason BtnUp does not cause the event to
 * fire on button 1, must be some Translations magic that I don't
 * understand.
 *
 * CDP - 5/11/91.
 */

typedef struct _StippleInfo {
    struct _StippleInfo *next;
    Pixmap stipple;
    Display *disp;
    Screen *screen;
} StippleInfo;

/*
 * ||| It would be nice to put this in the widget class.
 */

static StippleInfo *stipple_cache; /* Only one of these for each app. */

static char defaultTranslations[] =
    "<Btn1Down>,<Btn1Up>:		XiToggle() XmNotify() XiButtonUp()\n\
     <Btn1Down>:			XiGetFocus() XiToggle() \n\
     <Key>osfSelect:  			XiArmAndActivate()\n\
     <Key>osfActivate:  		XiArmAndActivate()\n\
     None<Key>space:			XiArmAndActivate()\n\
     None<Key>Return:  			XiArmAndActivate()\n\
     <Btn1Down>,<Leave>:		XiToggle()";

/*
 * These are the primitive translations that we need, this list
 * adds enter and leave to the primitive defaults.
 */

static char traverseTranslations[] =
"<Key>osfBeginLine: PrimitiveTraverseHome()\n\
 <Key>osfHelp:	PrimitiveHelp()\n\
 <Key>osfUp:	PrimitiveTraverseUp()\n\
 <Key>osfDown:	PrimitiveTraverseDown()\n\
 <Key>osfLeft:	PrimitiveTraverseLeft()\n\
 <Key>osfRight:	PrimitiveTraverseRight()\n\
 Shift ~Meta ~Alt <Key>Tab: PrimitivePrevTabGroup()\n\
 ~Meta ~Alt <Key>Tab: PrimitiveNextTabGroup()\n\
 <FocusIn>:	PrimitiveFocusIn()\n\
 <FocusOut>:	PrimitiveFocusOut()\n\
 <Enter>:	PrimitiveEnter()\n\
 <Leave>:	PrimitiveLeave()\n\
 <Unmap>:	PrimitiveUnmap()";

static XtActionsRec actionsList[] =
{
    {"XiArmAndActivate",		ArmAndActivate},
    {"XiToggle",			ToggleState},
    {"XmNotify",			Notify},
    {"XiGetFocus",			_XmGetFocus},
    {"XiDoubleNotify",			DoubleNotify},
    {"XiButtonUp",			ButtonUp},
    /*
     * Why aren't these in primitive?
     */
    { "PrimitiveEnter",		(XtActionProc) _XmPrimitiveEnter },
    { "PrimitiveLeave",		(XtActionProc) _XmPrimitiveLeave },
};

static XtResource resources[] =
{
  {
    XmNlabel, XmCLabel, XmRString,
    sizeof(String), XtOffsetOf(XmIconButtonRec, icon.label),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNlabelString, XmCLabelString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmIconButtonRec, icon.label_string),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNpixmap, XmCPixmap, XmRPrimForegroundPixmap,
    sizeof(Pixmap), XtOffsetOf(XmIconButtonRec, icon.pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmIconButtonRec, icon.check_set_render_table),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmIconButtonRec, icon.font_list),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmIconButtonRec, icon.font_list),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNalignment, XmCAlignment, XmRAlignment,
    sizeof(unsigned char), XtOffsetOf(XmIconButtonRec, icon.alignment),
    XmRImmediate,(XtPointer) XmALIGNMENT_BEGINNING
  },

  {
    XmNstringDirection, XmCStringDirection, XmRStringDirection,
    sizeof(unsigned char), XtOffsetOf(XmIconButtonRec, icon.string_direction),
    XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
  },

  {
    XmNiconPlacement, XmCIconPlacement, XmRXmIconPlacement,
    sizeof(XmIconPlacement), XtOffsetOf(XmIconButtonRec, icon.icon_placement),
    XmRImmediate, (XtPointer) XmIconTop
  },

  {
    XmNrecomputeSize, XmCBoolean , XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmIconButtonRec, icon.recompute),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNarmColor, XmCArmColor, XmRPixel,
    sizeof(Pixel), XtOffsetOf(XmIconButtonRec, icon.arm_color),
    XmRCallProc, (XtPointer) _XmSelectColorDefault
  },

  {
    XmNset, XmCBoolean , XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmIconButtonRec, icon.set),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNverticalMargin, XmCSpace, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.v_space),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNhorizontalMargin, XmCSpace, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.h_space),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNiconTextPadding, XmCSpace, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.icon_text_padding),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNactivateCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmIconButtonRec, icon.activate_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNdoubleClickCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmIconButtonRec, icon.double_click_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNpixmapWidth, XmCDimension, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.pix_width),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNpixmapHeight, XmCDimension, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.pix_height),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNpixmapDepth, XmCDimension, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmIconButtonRec, icon.pix_depth),
    XmRImmediate, (XtPointer) 0
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNhorizontalMargin, sizeof(Dimension),
    XtOffsetOf(XmIconButtonRec, icon.h_space),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },
  {
    XmNverticalMargin, sizeof(Dimension),
    XtOffsetOf(XmIconButtonRec, icon.v_space),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },
  {
    XmNiconTextPadding, sizeof(Dimension),
    XtOffsetOf(XmIconButtonRec, icon.icon_text_padding),
    FromPaddingPixels, (XmImportProc) ToPaddingPixels
  },
  {
    XmNlabelString, sizeof(XmString),
    XtOffsetOf(XmIconButtonRec, icon.label_string),
    GetValues_labelString, NULL
  }
};

XmIconButtonClassRec xmIconButtonClassRec = {
  { /* core fields */
    /* superclass		*/	SUPERCLASS,
    /* class_name		*/	XM_ICON_BUTTON_CLASS_NAME,
    /* widget_size		*/	sizeof(XmIconButtonRec),
    /* class_initialize		*/	ClassInit,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actionsList,
    /* num_actions		*/	XtNumber(actionsList),
    /* resources		*/	(XtResourceList)resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	XtExposeCompressMultiple,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	SetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	(XtGeometryHandler) QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* Xmprimitive fields */
      (XtWidgetProc)_XtInherit, /* border_highlight   */
      (XtWidgetProc)_XtInherit, /* border_unhighlight */
      traverseTranslations,	/* translations       */
      NULL,                     /* arm_and_activate   */
      get_resources,   	    	/* syn resources      */
      XtNumber(get_resources),	/* num syn_resources  */
      NULL                      /* extension          */
  },
  { /* Icon Button class fields */
      NULL                      /* extension          */
  }
};

WidgetClass xmIconButtonWidgetClass = (WidgetClass)&xmIconButtonClassRec;

/* Trait record for iconButton */
static const XmActivatableTraitRec iconButtonAT =
{
  0,				/* version	*/
  ChangeCB			/* changeCB	*/
};

/************************************************************
*	STATIC CODE
*************************************************************/

/*	Function Name: ClassInit
 *	Description:   Called to initialize information specific
 *                     to this widget class.
 *	Arguments:     none.
 *	Returns:       none.
 */
static void ClassInit(void)
{
    XmIconButtonClassRec *wc = &xmIconButtonClassRec;

    XtSetTypeConverter(XmRString, XmRXmIconPlacement,
		       (XtTypeConverter) CvtStringToIconPlacement,
		       NULL, (Cardinal) 0, XtCacheAll, (XtDestructor) NULL);
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmICONBUTTON_BIT );

    /* Install the activatable trait for all subclasses */
    XmeTraitSet((XtPointer)w_class, XmQTactivatable, (XtPointer) &iconButtonAT);
}


/*	Function Name: Initialize
 *	Description:   Called to initialize information specific
 *                     to this widget.
 *	Arguments:     req - what was originally requested.
 *                     set - what will be created (our superclassed have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */
static void
Initialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) set;
    Boolean resetPixmapValues = True;

    XmIconButton_unset_timer(iw) = NO_TIMER;
    XmIconButton_time(iw) = 0L;

    /*
     * If the label string is specified then it wins.  If only the label
     * is specified the it is converted into an XmString and stored
     * in the label_string.
     */

    XmIconButton_label_from_name(iw) = False;
    if (XmIconButton_label_string(iw) == NULL) {
	if (XmIconButton_label(iw) == NULL)
	{
	    XmIconButton_label_string(iw) = CreateXmString(set, XtName(set));
	    XmIconButton_label_from_name(iw) = True;
	}
	else {
	    if ((XmIconButton_label_string(iw) =
		 CreateXmString(set, XmIconButton_label(iw))) == NULL)
	    {
		XmIconButton_label_string(iw) = CreateXmString(set, XtName(set));
		XmIconButton_label_from_name(iw) = True;
	    }
	}
    }
    else {
	XmIconButton_label_string(iw) =
	  XmStringCopy(XmIconButton_label_string(iw));
    }
    if (XmIconButton_label(iw))
	    XmIconButton_label(iw) = XtNewString(XmIconButton_label(iw));

    if (ValidPixmap(XmIconButton_pixmap(iw)))
    {
	/* here's what we enforce: all must be non-zero */
	if (	(0 != XmIconButton_pix_width(iw)) 	&&
		(0 != XmIconButton_pix_height(iw)) 	&&
		(0 != XmIconButton_pix_depth(iw))	)
		resetPixmapValues = False;
    }

    if (resetPixmapValues)
	{
	XmIconButton_pix_width(iw) = 0;
	XmIconButton_pix_height(iw) = 0;
	XmIconButton_pix_depth(iw) = 0;
   	}
    else
    {
	/* we have a validPixmap and valid values */
	AddPixCache(XtDisplay(set), XmIconButton_pixmap(iw),
		    XmIconButton_pix_width(iw),
		    XmIconButton_pix_height(iw),
		    XmIconButton_pix_depth(iw));
        IncPixCache(XtDisplay(set), XmIconButton_pixmap(iw));
    }

    /* If layout_direction is set, it overrides string_direction.
     * If string_direction is set, but not layout_direction, use
     *	string_direction value.
     * If neither is set, get from parent
     */
    if (XmPrim_layout_direction(iw) != XmDEFAULT_DIRECTION) {
	if (XmIconButton_string_direction(iw) == XmDEFAULT_DIRECTION)
	    XmIconButton_string_direction(iw) =
		XmDirectionToStringDirection(XmPrim_layout_direction(iw));
    } else if (XmIconButton_string_direction(iw) != XmDEFAULT_DIRECTION) {
	XmPrim_layout_direction(iw) =
	    XmStringDirectionToDirection(XmIconButton_string_direction(iw));
    } else {
	XmPrim_layout_direction(iw) = _XmGetLayoutDirection(XtParent(set));
	XmIconButton_string_direction(iw) =
	    XmDirectionToStringDirection(XmPrim_layout_direction(iw));
    }

    if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION,
	    XmIconButton_string_direction(iw), (Widget) iw))
    {
	XmIconButton_string_direction(iw) = XmSTRING_DIRECTION_L_TO_R;
	XmPrim_layout_direction(iw) =
	    XmStringDirectionToDirection(XmIconButton_string_direction(iw));
    }

    if (XmIconButton_font_list(iw) == NULL)
    {
        XmIconButton_font_list(iw) = XmeGetDefaultRenderTable((Widget) iw,
						   XmBUTTON_FONTLIST);
    }

    /* Make a local copy of the font list */
    XmIconButton_font_list(iw) = XmFontListCopy( XmIconButton_font_list(iw));

    if ((req->core.width == 0) || (req->core.height == 0)) {
	Dimension width, height;

	GetDesiredSize(set, &width,  &height);
	RequestNewSize(set, width, height);
    }

    CalcLocations(set);
    CreateGCs(set);
}

/*	Function Name: Resize
 *	Description:   Called when this widget has been resized.
 *	Arguments:     w - the widget to resize.
 *	Returns:       none.
 */

static void
Resize(Widget w)
{
    CalcLocations(w);
}

/*	Function Name: Redisplay
 *	Description:   This function redraws the contents of the widget.
 *	Arguments:     w - the widget.
 *                     event - event that caused the exposure.
 *                     region - the region containing all the exposures.
 *	Returns:       none
 */
static void
Redisplay(Widget w, XEvent * event, Region region)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    int dx, dy, width, height;
    GC topgc, bottomgc, text_gc, icon_gc, icon_stippled_gc;

    dx = iw->primitive.highlight_thickness;
    dy = dx;

    width = iw->core.width - (2 * dx);
    height = iw->core.height - (2 * dy);

    if (XmIconButton_set(iw)) {
	Dimension fill_width, fill_height;

	if ((int)iw->core.width > (int)(2 * H_SPACE(iw)))
	    fill_width = iw->core.width - 2 * FILL_SPACE(iw);
	else
	    fill_width = 0;

	if ((int)iw->core.height > (int)(2 * V_SPACE(iw)))
	    fill_height = iw->core.height - 2 * FILL_SPACE(iw);
	else
	    fill_height = 0;

	if ((fill_width != 0) && (fill_height != 0))
	    XFillRectangle(XtDisplay(w), XtWindow(w), XmIconButton_fill_gc(iw),
			   FILL_SPACE(iw), FILL_SPACE(iw),
			   fill_width, fill_height);

	icon_gc = XmIconButton_pixmap_fill_gc(iw);
	icon_stippled_gc = XmIconButton_stippled_set_gc(iw);
	topgc    = iw->primitive.bottom_shadow_GC;
	bottomgc = iw->primitive.top_shadow_GC;
    }
    else {
	icon_gc  = XmIconButton_gc(iw);
	icon_stippled_gc = XmIconButton_stippled_unset_gc(iw);
	topgc     = iw->primitive.top_shadow_GC;
	bottomgc  = iw->primitive.bottom_shadow_GC;
    }

    if (XtIsSensitive(w)) {
	if (XmIconButton_set(iw))
	    text_gc = XmIconButton_pixmap_fill_gc(iw);
	else
	    text_gc = XmIconButton_gc(iw);

	icon_stippled_gc = None;
    }
    else {
	if (XmIconButton_set(iw))
	    text_gc = XmIconButton_stippled_set_text_gc(iw);
	else
	    text_gc = XmIconButton_stippled_text_gc(iw);
    }

    DrawTextAndImage(w, text_gc, icon_gc, icon_stippled_gc);

    XmeDrawShadows(XtDisplay(w), XtWindow(w), topgc, bottomgc,
		   dx, dy, width, height, iw->primitive.shadow_thickness,
		   XmSHADOW_OUT);

    if (iw->primitive.highlighted)
	_XmExtHighlightBorder(w);
    else
	_XmExtUnhighlightBorder(w);
}

/*	Function Name: SetValuesAlmost
 *	Description: Handles the case of our geom request being denied.
 *	Arguments: old - the old state of the widget.
 *                 set - the new state of the widget.
 *                 request - the requested new geometry.
 *                 reply - the compromise from our parent.
 *	Returns: none
 */
static void
SetValuesAlmost(Widget old, Widget set,
		XtWidgetGeometry * request,  XtWidgetGeometry * reply)
{
    if (request->request_mode != 0) /* XtGeometryAlmost, make new request */
	*request = *reply;

    CalcLocations(set);
}

/*	Function Name: SetValues
 *	Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */
static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    Boolean resetGCs, recalc, redisplay, reinit_l, reinit_ls;
    XmIconButtonWidget old_iw = (XmIconButtonWidget) current;
    XmIconButtonWidget set_iw = (XmIconButtonWidget) set;
    int i;

    Boolean pixmapChanged = False;
    Boolean pixmapGeoChanged = False;
    Boolean resetPixmapValues = False;

    reinit_l = reinit_ls = resetGCs = recalc = redisplay = False;

    for (i = 0; i < *num_args; i++) {
	String name = args[i].name;

	if (streq(XmNlabel, name))
	    reinit_l = resetGCs = recalc = redisplay = TRUE;

	if (streq(XmNlabelString, name))
	    reinit_ls = resetGCs = recalc = redisplay = TRUE;

	if (streq(XmNpixmap, name))
	{
	    DecPixCache(XtDisplay(current), XmIconButton_pixmap(old_iw));
	    recalc = redisplay = TRUE;
	    pixmapChanged = True;
	}

	if ( streq(XmNpixmapWidth, name) || streq(XmNpixmapHeight, name) || streq(XmNpixmapDepth, name) )
	{
	    pixmapGeoChanged = True;
	    /* for now, we naively assume that the user knows that all of
	    ** these resources should be changed at the same time
	    */
	}

	if (streq(XmNset, name)) {
	    /*
	     * Remove the timer since programmer has changed value.
	     */
	    if (XmIconButton_unset_timer(set_iw) != NO_TIMER) {
		XtRemoveTimeOut(XmIconButton_unset_timer(set_iw));
		XmIconButton_unset_timer(set_iw) = NO_TIMER;
	    }
	}
    }

    /* ----------------------------------------------------------------- */

    /* now make sense of the pixmap changes */
    if (pixmapChanged && !pixmapGeoChanged)
    {
	/* for backward compatibility, reset values and fall through to
	** GetDesiredSize
	*/
	resetPixmapValues = True;
    }
    else if (pixmapGeoChanged && !pixmapChanged)
    {
	/* we don't allow this case */
	resetPixmapValues = True;
    }
    /* else if neither changed, then we don't worry about anything */
    /* else if both changed, we use new values */

    if (pixmapGeoChanged)
    {
	/* here's what we enforce: all must be non-zero */
	if (!(	(0 != XmIconButton_pix_width(set_iw)) 	&&
		(0 != XmIconButton_pix_height(set_iw)) 	&&
		(0 != XmIconButton_pix_depth(set_iw))	))
		resetPixmapValues = True;
    }

    /* for backward-compatibility, preserve old behavior in which pixmap
    ** can change without sizes changing when recomputeSize is not set, in
    ** which case we restore the values rather than use the new ones
    */
    if (resetPixmapValues)
    {
    	if (XmIconButton_recompute(set_iw))
	{
		XmIconButton_pix_width(set_iw) = 0;
		XmIconButton_pix_height(set_iw) =
		XmIconButton_pix_depth(set_iw) = 0;
	}
	else
	{
		XmIconButton_pix_width(set_iw) = XmIconButton_pix_width(old_iw);
		XmIconButton_pix_height(set_iw) = XmIconButton_pix_height(old_iw);
		XmIconButton_pix_depth(set_iw) = XmIconButton_pix_depth(old_iw);
	}
    }

    /* ----------------------------------------------------------------- */

    if ((old_iw->primitive.foreground != set_iw->primitive.foreground) ||
	(old_iw->core.background_pixel != set_iw->core.background_pixel) ||
	(XmIconButton_arm_color(old_iw) != XmIconButton_arm_color(set_iw)))
    {
	resetGCs = redisplay = True;
    }

    if( XtIsSensitive(current) != XtIsSensitive(set) )
    {
	redisplay = True;
    }

    if( XmIconButton_font_list(old_iw) != XmIconButton_font_list(set_iw) ) {
	if( XmIconButton_font_list(old_iw)  != NULL )
	{
	    XmFontListFree (XmIconButton_font_list(old_iw));
	}
	if( XmIconButton_font_list(set_iw) == NULL )
	{
	    XmIconButton_font_list(set_iw) = XmeGetDefaultRenderTable(set,
							   XmBUTTON_FONTLIST);
	}
	XmIconButton_font_list(set_iw) = XmFontListCopy(XmIconButton_font_list(set_iw));
	recalc = redisplay = True;
    }

    if ((XmIconButton_icon_placement(old_iw) != XmIconButton_icon_placement(set_iw)) ||
	(XmIconButton_v_space(old_iw) != XmIconButton_v_space(set_iw)) ||
	(XmIconButton_h_space(old_iw) != XmIconButton_h_space(set_iw)) ||
	(XmIconButton_icon_text_padding(old_iw) != XmIconButton_icon_text_padding(set_iw)))
    {
	recalc = redisplay = TRUE;
    }

    if ((XmIconButton_set(old_iw) != XmIconButton_set(set_iw)) ||
	(XmIconButton_alignment(old_iw) != XmIconButton_alignment(set_iw)) ||
	(XmIconButton_string_direction(old_iw) != XmIconButton_string_direction(set_iw)) ||
        (XmPrim_layout_direction(old_iw) != XmPrim_layout_direction(set_iw)))
    {
	redisplay = TRUE;
    }

    if (XmIconButton_recompute(old_iw) != XmIconButton_recompute(set_iw)) {
	if (XmIconButton_recompute(set_iw))
	    redisplay = recalc = TRUE;
    }

    if (reinit_l || reinit_ls)
    {
	/*
	 * The order here makes sure that label string overrides label
	 * when both are set. Fix up data first, then check what we
	 * should be displaying. If both have changed, deal only with
	 * the label string case.
	 */

	/* this has the desired behavior, except in this case: only
	 * XmNlabel is set at the beginning, then it migrates to
	 * XmNlabelString; then XmNlabel is reset to NULL. What value
	 * should be used?
	*/
	/* Boolean	was_from_name; */
	if (reinit_ls) {
		XmStringFree(XmIconButton_label_string(old_iw));
		if (XmIconButton_label_string(set_iw))
			XmIconButton_label_string(set_iw) = XmStringCopy(XmIconButton_label_string(set_iw));
	    }
	    if (reinit_l) {
		XtFree(XmIconButton_label(old_iw));
		if (XmIconButton_label(set_iw))
			XmIconButton_label(set_iw) = XtNewString(XmIconButton_label(set_iw));
	    }
	    /* was_from_name = XmIconButton_label_from_name(set_iw); */
	    XmIconButton_label_from_name(set_iw) = False;
	    if (reinit_ls) {
		if (XmIconButton_label_string(set_iw) == NULL) {
		    if (XmIconButton_label(set_iw) == NULL)
		    {
			XmIconButton_label_string(set_iw) = CreateXmString(set, XtName(set));
			XmIconButton_label_from_name(set_iw) = True;
		    }
		    else
		    {
			if ((XmIconButton_label_string(set_iw) =
			     CreateXmString(set, XmIconButton_label(set_iw))) == NULL)
			{
			    XmIconButton_label_string(set_iw) =
				CreateXmString(set, XtName(set));
			    XmIconButton_label_from_name(set_iw) = True;
			}
		    }
		}
	    }
	    else
	    if (reinit_l) {
		/* spec says, if labelString is set it is used, which
		 * isn't enough to really work with; I think it really
		 * means that if both are set, XmNlabelString is used,
		 * which is a much different thing
		 */
		/* now I'm confused as to why this was ever necessary */
		/* if (was_from_name)	*/
		{
			XmStringFree(XmIconButton_label_string(set_iw));
			if (XmIconButton_label(set_iw) == NULL)
			{
				XmIconButton_label_string(set_iw) = CreateXmString(set, XtName(set));
				XmIconButton_label_from_name(set_iw) = True;
			}
			else
			{
				if ((XmIconButton_label_string(set_iw) =
					 CreateXmString(set, XmIconButton_label(set_iw))) == NULL)
				{
					XmIconButton_label_string(set_iw) =
						CreateXmString(set, XtName(set));
					XmIconButton_label_from_name(set_iw) = True;
				}
			}
		}
	    }
    }

    if (recalc) {
	if (XmIconButton_recompute(set_iw)) {
	    Dimension width, height;

	    GetDesiredSize(set, &width,  &height);

	    /*
	     * SetValues will ask for a new size.
	     */

	    set->core.height = height;
	    set->core.width = width;
	}

	CalcLocations(set);
    }

    if (resetGCs) {
	DestroyGCs(current);
	CreateGCs(set);
    }

    return(redisplay);
}

/*	Function Name: Destroy
 *	Description:   Cleans up after the widget.
 *	Arguments:     w - the widget.
 *	Returns:       none.
 */
static void
Destroy(Widget w)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    XtFree(XmIconButton_label(iw));
    XmStringFree(XmIconButton_label_string(iw));
    if (XmIconButton_font_list(iw)  != NULL) XmFontListFree (XmIconButton_font_list(iw));
    DestroyGCs(w);

    if (XmIconButton_unset_timer(w) != NO_TIMER) {
	XtRemoveTimeOut(XmIconButton_unset_timer(w));
	XmIconButton_unset_timer(w) = NO_TIMER;
    }
}

/*	Function Name: QueryGeometry
 *	Description:   Called when my parent wants to know what size
 *                     I would like to be.
 *	Arguments:     w - the widget to check.
 *                     indended - constriants imposed by the parent.
 *                     preferred - what I would like.
 *	Returns:       See Xt Manual.
 */

static XtGeometryResult
QueryGeometry(Widget w,XtWidgetGeometry *intended, XtWidgetGeometry *preferred)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    if (XmIconButton_recompute(iw))
	GetDesiredSize(w, &(preferred->width), &(preferred->height));
    else {
	preferred->width = w->core.width;
	preferred->height = w->core.height;
    }

    return(_XmHWQuery(w, intended, preferred));
}

/************************************************************
 *
 * Type Converters.
 *
 ************************************************************/

/*	Function Name: CvtStringToIconPlacement
 *	Description:   Converts a string to an Icon Placement
 *	Arguments:     dpy - the X Display.
 *                     args, num_args - *** NOT USED ***
 *                     fromVal - contains the string to convert.
 *                     toVal - contains the converted node state.
 *	Returns:       True if the SetValues succeeds.
 */
static Boolean
CvtStringToIconPlacement(Display * dpy, XrmValuePtr args, Cardinal *num_args,
			 XrmValuePtr fromVal, XrmValuePtr toVal)
{
    static XmIconPlacement type;
    static XrmQuark XtQETop;
    static XrmQuark XtQELeft;
    static XrmQuark XtQERight;
    static XrmQuark XtQEBottom;
    static XrmQuark XtQEIconOnly;
    static XrmQuark XtQEIconNone;
    static Boolean haveQuarks = FALSE;
    XrmQuark q;
    char lowerName[BUFSIZ];

    if (!haveQuarks) {
	XtQETop = XrmStringToQuark("top");
	XtQELeft = XrmStringToQuark("left");
	XtQERight = XrmStringToQuark("right");
	XtQEBottom = XrmStringToQuark("bottom");
	XtQEIconOnly = XrmStringToQuark("icononly");
	XtQEIconNone = XrmStringToQuark("none");
	haveQuarks = TRUE;
    }

    XmCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
    q = XrmStringToQuark(lowerName);

    if ( (q == XtQETop) || streq(lowerName, "icontop") )
	type = XmIconTop;
    else if ( (q == XtQELeft) || streq(lowerName, "iconleft") )
	type = XmIconLeft;
    else if ( (q == XtQERight) || streq(lowerName, "iconright") )
	type = XmIconRight;
    else if ( (q == XtQEBottom) || streq(lowerName, "iconbottom") )
	type = XmIconBottom;
    else if (q == XtQEIconOnly)
	type = XmIconOnly;
    else if ( (q == XtQEIconNone) || streq(lowerName, "iconnone") )
	type = XmIconNone;
    else {
	XtDisplayStringConversionWarning(dpy,
					 fromVal->addr, XmRXmIconPlacement);
	return(FALSE);		/* Conversion failed. */
    }

    if (toVal->addr == NULL) {
	toVal->size = sizeof(XmIconPlacement);
	toVal->addr = (XtPointer) &type;
	return(TRUE);
    }

    if (toVal->size >= sizeof(XmIconPlacement)) {
	XmIconPlacement *loc = (XmIconPlacement *)toVal->addr;

	*loc = type;
	return(TRUE);
    }

    toVal->size = sizeof(XmIconPlacement);
    return(FALSE);
}

/************************************************************
 *
 * Actions and Callbacks.
 *
 ************************************************************/

/*	Function Name: ToggleState
 *	Description:   Toggles the state of the icon button.
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */
static void
ToggleState(Widget w, XEvent * event, String * params, Cardinal * num_params)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    Arg args[1];

    XtSetArg(args[0], XmNset, !(XmIconButton_set(iw)));
    XtSetValues(w, args, (Cardinal) 1);
}

/*	Function Name: DoubleNotify
 *	Description:   Calls all functions on the icon button's
 *                     doubleClickCallback list
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */
static void
DoubleNotify(Widget w, XEvent * event, String * params, Cardinal * num_params)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    XmIconButtonCallbackInfo info;

    info.state = XmIconButton_set(iw);
    info.event = event;

    XtCallCallbackList(w, XmIconButton_double_click_callback(iw), &info);
}

/*	Function Name: Notify
 *	Description:   Calls all functions on the icon button's
 *                     activateCallback list
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */
static void
Notify(Widget w, XEvent * event, String * params, Cardinal * num_params)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    XmIconButtonCallbackInfo info;
    Boolean dclick;

    if ((event->type == ButtonPress) || (event->type == ButtonRelease))
	dclick = ((event->xbutton.time - XmIconButton_time(iw)) <=
		  XtGetMultiClickTime(XtDisplay(w)));
    else
	dclick = False;

    info.state = XmIconButton_set(iw);
    info.event = event;

    if (dclick)
	XtCallCallbackList(w, XmIconButton_double_click_callback(iw), &info);
    else
	XtCallCallbackList(w, XmIconButton_activate_callback(iw), &info);
}

/*	Function Name: ArmAndActivate
 *	Description:   Arms the button, notifies it of the action, then
 *                     disarms the button after a small delay.
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */
static void
ArmAndActivate(Widget w,
	       XEvent * event, String * params, Cardinal * num_params)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    Arg args[1];

    if (!XmIconButton_set(iw)) {
	XtSetArg(args[0], XmNset, True);
	XtSetValues(w, args, (Cardinal) 1);
    }

    XmIconButton_unset_timer(iw) = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					   (unsigned long) DELAY,
					   Deactivate, (XtPointer) w);

    Notify(w, event, params, num_params);
}

/*	Function Name: Deactivate
 *	Description: Called when the icon button should be deactivated.
 *	Arguments: data - the client data.
 *                 timer - the timer that fired.
 *	Returns: none.
 */
static void
Deactivate(XtPointer data, XtIntervalId *timer)
{
    Arg args[1];
    Widget w = (Widget) data;
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    XtSetArg(args[0], XmNset, False);
    XtSetValues(w, args, (Cardinal) 1);

    XmIconButton_unset_timer(iw) = NO_TIMER;
}

/*	Function Name: ButtonUp
 *	Description:   Saves the timestamp of the ButtonUp event
 *                     for later double click processing
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */
static void
ButtonUp(Widget w,
	       XEvent * event, String * params, Cardinal * num_params)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    XmIconButton_time(iw) = event->xbutton.time;
}

/************************************************************
 *
 * Internal routines.
 *
 ************************************************************/

/*	Function Name: GetCacheElem
 *	Description: Gets an element in the cache.
 *	Arguments: disp, pixmap - The keys into the cache.
 *	Returns: elem - The list elem for this element.
 */

static XmListElem *
GetCacheElem(Display *disp, Pixmap pix)
{
    XmListElem *elem;

    if (pix_cache_list == NULL)
	return(NULL);

    for (elem = XmListFirst(pix_cache_list); elem != NULL;
	 elem = XmListElemNext(elem))
    {
	PixCacheEntry * entry = (PixCacheEntry *) XmListElemData(elem);

	if ((entry->display == disp) && (entry->pixmap == pix))
	{
	    return(elem);
	}
    }

    return(NULL);
}

/*	Function Name: CheckPixCache
 *	Description: Checks the cache to see if this pixmap is in it.
 *	Arguments: disp, pixmap - The keys into the cache.
 *                 width, height and depth - data about the pixmap.
 *	Returns: True if a match found.
 */

static Boolean
CheckPixCache(Display *disp, Pixmap pixmap,
	      unsigned int *width, unsigned int *height, unsigned int *depth)
{
    XmListElem * elem = GetCacheElem(disp, pixmap);
    PixCacheEntry * entry;

    if (elem == NULL)
	return(False);

    entry = (PixCacheEntry *) XmListElemData(elem);

    *width = entry->width;
    *height = entry->height;
    *depth = entry->depth;

    return(True);
}

/*	Function Name: AddPixCache
 *	Description: Adds or updates data in the pixmap cache.
 *	Arguments: disp, pixmap - The keys into the cache.
 *                 width, height and depth - data about the pixmap.
 *	Returns: True if a match found.
 */

static void
AddPixCache(Display *disp, Pixmap pixmap,
	    unsigned int width, unsigned int height, unsigned int depth)
{
    XmListElem * elem = GetCacheElem(disp, pixmap);
    PixCacheEntry * entry;

    if (elem == NULL)
    {
	entry = (PixCacheEntry *) XtCalloc(sizeof(PixCacheEntry), 1);
	entry->display = disp;
	entry->pixmap = pixmap;

	if (pix_cache_list == NULL)
	    pix_cache_list = _XmListInit();
	_XmListAddBefore(pix_cache_list, NULL, (XtPointer) entry);
    }
    else
	entry = (PixCacheEntry *) XmListElemData(elem);

    entry->width = width;
    entry->height = height;
    entry->depth = depth;
}

/*	Function Name: IncPixCache
 *	Description: Increments the count in the pixmap cache.
 *	Arguments: disp - The current display.
 *                 pix - The pixmap to increment.
 *	Returns: none.
 */

static void
IncPixCache(Display *disp, Pixmap pix)
{
    XmListElem * elem = GetCacheElem(disp, pix);

    if (elem != NULL)
	(((PixCacheEntry *) XmListElemData(elem))->count)++;
}

/*	Function Name: DecPixCache
 *	Description: Decrements the count in the pixmap cache.
 *	Arguments: disp - The current display.
 *                 pix - The pixmap to increment.
 *	Returns: none.
 */

static void
DecPixCache(Display *disp, Pixmap pix)
{
    XmListElem * elem = GetCacheElem(disp, pix);
    PixCacheEntry * entry;

    if (elem == NULL)
	return;

    entry = (PixCacheEntry *) XmListElemData(elem);

    if (entry->count > 0)
	(entry->count)--;

    if (entry->count == 0) {
	_XmListRemove(pix_cache_list, elem);
	XtFree((char *) entry);
    }
}

static void GetValues_labelString ( Widget w, int n, XtArgVal *value)
{
	(*value) = (XtArgVal) XmStringCopy(XmIconButton_label_string(w));
}

/*	Function Name: FromPaddingPixels
 *	Description: Converts from pixels to current unit type
 *                   does either horiz or vert depending on icon placement.
 *	Arguments: widget - the icon button widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

static void
FromPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) widget;

    switch(XmIconButton_icon_placement(iw)) {
    case XmIconTop:
    case XmIconBottom:
	XmeFromVerticalPixels(widget, offset, value);
	break;
    default:			/* everything else is horiz. */
	XmeFromHorizontalPixels(widget, offset, value);
	break;
    }
}

/*	Function Name: ToPaddingPixels
 *	Description: Converts to pixels from current unit type
 *                   does either horiz or vert depending on icon placement.
 *	Arguments: widget - the icon button widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: the import order from _XmTo{Horizontal, Vertical}Pixels.
 */

static XmImportOperator
ToPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) widget;

    switch(XmIconButton_icon_placement(iw)) {
    case XmIconTop:
    case XmIconBottom:
	return(XmeToVerticalPixels(widget, offset, value));
    default:
	return(XmeToHorizontalPixels(widget, offset, value));
    }
}

/*	Function Name: GetDesiredSize
 *	Description:   Gets the size that this widget would like to be.
 *	Arguments:     w - the icon widget.
 *  RETURNED           width, height - the desired size.
 *	Returns:       none.
 */

static void
GetDesiredSize(Widget w, Dimension * width, Dimension * height)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    Dimension total_width, total_height;

    XmStringExtent(XmIconButton_font_list(iw), XmIconButton_label_string(iw),
		   &total_width, &total_height);

    if( (XmIconButton_icon_placement(iw) != XmIconNone) && ValidPixmap(XmIconButton_pixmap(iw)) )
    {
	Window root;
	int x, y;
	unsigned int local_width, local_height, bw, depth;

	/*
	 * If we have an icon of unknown size then ask the server,
	 * be sure to not make a round trip if not needed.
	 */

	if ((XmIconButton_pix_width(iw) == 0) || (XmIconButton_pix_height(iw) == 0) ||
	    (XmIconButton_pix_depth(iw) == 0))
	{
	    if (!CheckPixCache(XtDisplay(w), XmIconButton_pixmap(iw),
			       &local_width, &local_height, &depth))
	    {
		XGetGeometry(XtDisplay(w), XmIconButton_pixmap(iw), &root, &x, &y,
			     &local_width, &local_height, &bw, &depth);

		AddPixCache(XtDisplay(w), XmIconButton_pixmap(iw),
			    local_width, local_height, depth);
	    }

	    IncPixCache(XtDisplay(w), XmIconButton_pixmap(iw));

	    XmIconButton_pix_width(iw) = local_width;
	    XmIconButton_pix_height(iw) = local_height;
	    XmIconButton_pix_depth(iw) = depth;
	}
	else {
	    local_width = XmIconButton_pix_width(iw);
	    local_height = XmIconButton_pix_height(iw);
	    depth = XmIconButton_pix_depth(iw);
	}

	switch (XmIconButton_icon_placement(iw)) {
	case XmIconTop:
	case XmIconBottom:
	    total_height += local_height + XmIconButton_icon_text_padding(iw);

	    if ( local_width > total_width )
		total_width = local_width;

	    break;
	case XmIconOnly:
	    total_height = local_height;
	    total_width = local_width;
	    break;
	case XmIconLeft:
	case XmIconRight:
	    total_width += local_width + XmIconButton_icon_text_padding(iw);

	    if (local_height > total_height)
		total_height = local_height;

	    break;
	default:
	    break;
	}
    }
    else {
	XmIconButton_pix_width(iw) = XmIconButton_pix_height(iw) = 0;
	XmIconButton_pix_depth(iw) = 0;
    }


    *height = total_height + 2 * V_SPACE(iw);
    *width = total_width + 2 * H_SPACE(iw);
}

/*	Function Name: RequestNewSize
 *	Description:   Asks our parent for a new size.
 *	Arguments:     w - the Icon Button widget.
 *                     width, height - the width and height we want to be.
 *	Returns:       none.
 */

static void
RequestNewSize(Widget w, Dimension width, Dimension height)
{
    Dimension w_ret, h_ret;
    XtGeometryResult ret_val;

    ret_val = XtMakeResizeRequest(w, width, height, &w_ret, &h_ret);

    if (ret_val == XtGeometryAlmost)
	(void) XtMakeResizeRequest(w, w_ret, h_ret, NULL, NULL);

    Resize(w);			/* May not have been called. */
}

/*	Function Name: CalcLocations
 *	Description:   Calculates where the icon and strings should be
 *                     painted.
 *	Arguments:     w - the icon button widget.
 *	Returns:       none.
 */

static void
CalcLocations(Widget w)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    Dimension width, height, d_width, d_height;
    Dimension extra_y_space;
    Boolean   have_pixmap = True;

    width = w->core.width;
    height = w->core.height;

    GetDesiredSize(w, &d_width, &d_height);

    if (height > d_height)
	extra_y_space = (int)(height - d_height)/2;
    else
	extra_y_space = 0;

    XmIconButton_max_text_width(iw) = width - 2 * H_SPACE(iw);
    XmIconButton_max_text_height(iw) = height - 2 * V_SPACE(iw);
    XmIconButton_text_x(iw) = H_SPACE(iw);
    XmIconButton_text_y(iw) = V_SPACE(iw);

    if( XmIconButton_icon_placement(iw) != XmIconNone && ValidPixmap(XmIconButton_pixmap(iw)) )
    {
	/*
	 * Here we actually have a pixmap so lets figure out the
	 * location of the pixmap.  First we will loook at the
	 * horizontal position.
	 */
	switch (XmIconButton_icon_placement(iw)) {
	case XmIconLeft: /* Flush Left */
	    XmIconButton_pix_x(iw) = (Position) H_SPACE(iw);
	    break;
	case XmIconRight: /* Flush Right */
	    XmIconButton_pix_x(iw) = (Position) (width - H_SPACE(iw) -
					 XmIconButton_pix_width(iw));
	    break;
	default: /* Centered */
	    XmIconButton_pix_x(iw) = ((Position) width -
			      (Position) XmIconButton_pix_width(iw))/2;
	    break;
	}

	/*
	 * Now lets deside on the the vertical position
	 */
	switch (XmIconButton_icon_placement(iw)) {
	case XmIconTop: /* Centered At the top */
	    XmIconButton_pix_y(iw) = V_SPACE(iw) + extra_y_space;
	    break;
	case XmIconBottom: /* Centered at the bottom */
	    XmIconButton_pix_y(iw) = ((Position) height -
			      ((Position) V_SPACE(iw) +
			       (Position) extra_y_space +
			       (Position) XmIconButton_pix_height(iw)));
	    break;
	default: /* Centered */
	    XmIconButton_pix_y(iw) = ((Position) height -
			      (Position) XmIconButton_pix_height(iw))/2;
	    break;
	}
    }
    else
    {
	/*
	 * If we get here that means that we do not have a pixmap
	 * to play with so lets just zero the x and y locations of the
	 * pixmap.
	 */
	XmIconButton_pix_x(iw) = XmIconButton_pix_y(iw) = 0;
	have_pixmap = False;
    }

    /*
     * Now that we know the location of the pixmap, lets go and try to
     * find out what we want to do with the XmString if there is one.
     */
    if( XmIconButton_icon_placement(iw) != XmIconOnly ) {
	Dimension text_width, text_height;
	int       tmp;

	/*
	 * First let find out what the size of the string we have is
	 */
	XmStringExtent(XmIconButton_font_list(iw), XmIconButton_label_string(iw),
		       &text_width, &text_height);
	XmIconButton_max_text_height(iw) = text_height;

	switch (XmIconButton_icon_placement(iw)) {
	case XmIconLeft: /* Aligned to Right of Icon. */
	    /*
	     * If the pixmap is to our left then we can use all the
	     * space to the right of the pixmap.
	     */
	    if( have_pixmap )
	    {
		XmIconButton_text_x(iw) = XmIconButton_pix_x(iw) + XmIconButton_pix_width(iw) +
		    XmIconButton_icon_text_padding(iw);
		if( (tmp = (int)width - (int)H_SPACE(iw) -
		     (int)XmIconButton_text_x(iw)) < 0 )
		{
		    tmp = 0;
		}

		XmIconButton_max_text_width(iw) = tmp;
	    }
	    break;
	case XmIconRight: /* Aligned to left of Icon. */
	    if( have_pixmap )
	    {
		XmIconButton_text_x(iw) = H_SPACE(iw);
		if( (tmp = XmIconButton_pix_x(iw) - XmIconButton_text_x(iw)) < 0 ) tmp = 0;
		XmIconButton_max_text_width(iw) = tmp;
	    }
	    break;
	default:
	    break;
	}

	switch (XmIconButton_icon_placement(iw)) {
	case XmIconTop: /* Along Bottom. */
	    XmIconButton_text_y(iw) = height - (V_SPACE(iw) + (Position)text_height);
	    if( have_pixmap )
	    {
		tmp = XmIconButton_pix_y(iw) + XmIconButton_pix_height(iw) +
		    XmIconButton_icon_text_padding(iw);
		if( XmIconButton_text_y(iw) < tmp )
		{
		    XmIconButton_text_y(iw) = tmp;
		    if( (tmp = height - V_SPACE(iw) - XmIconButton_text_y(iw)) < 0 )
			tmp = 0;
		    XmIconButton_max_text_height(iw) = tmp;
		}
	    }
	    break;
	case XmIconBottom: /* Along Top. */
	    XmIconButton_text_y(iw) = V_SPACE(iw);
	    if( have_pixmap )
	    {
		tmp = XmIconButton_pix_y(iw) - XmIconButton_icon_text_padding(iw) -
		    XmIconButton_text_y(iw);
		if( tmp < 0 ) tmp = 0;
		XmIconButton_max_text_height(iw) = tmp;
	    }
	    break;
	default:
	    /* Centered. */
	    XmIconButton_text_y(iw) = ((Position) height - (Position) text_height)/2;
	    break;
	}
    }

    /*
     * Else
     *
     * Text_x and Text_y are garbage, but never used.
     */
}

/*	Function Name: DrawTextAndImage
 *	Description:   Renders the Text and the image.
 *	Arguments:     w - the icon button widget.
 *                     text_gc - gc to use when drawing the text.
 *                     icon_gc - gc to use when drawing the icon.
 *                     icon_stippled_gc - gc to use to fake a stipple
 *                                       on the image.
 *	Returns:       none.
 */

static void
    DrawTextAndImage(Widget w, GC text_gc, GC icon_gc, GC icon_stippled_gc)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    if( XmIconButton_icon_placement(iw) != XmIconNone && ValidPixmap(XmIconButton_pixmap(iw)) )
    {
	if (XmIconButton_pix_depth(iw) == 1)
	{
	    XCopyPlane(XtDisplay(w), XmIconButton_pixmap(iw), XtWindow(w), icon_gc,
		       0, 0, XmIconButton_pix_width(iw), XmIconButton_pix_height(iw),
		       XmIconButton_pix_x(iw), XmIconButton_pix_y(iw), 1L);
	}
	else
	{
	    XCopyArea(XtDisplay(w), XmIconButton_pixmap(iw), XtWindow(w), icon_gc,
		      0, 0, XmIconButton_pix_width(iw), XmIconButton_pix_height(iw),
		      XmIconButton_pix_x(iw), XmIconButton_pix_y(iw));
	}

	/*
	 * Stipple the background color over the top of the image
	 * since XCopyPlace and Area do not use the tile or stipple.
	 */

	if( icon_stippled_gc != None )
	{
	    XFillRectangle(XtDisplay(w), XtWindow(w), icon_stippled_gc,
			   XmIconButton_pix_x(iw), XmIconButton_pix_y(iw),
			   XmIconButton_pix_width(iw), XmIconButton_pix_height(iw));
	}
    }

    if( XmIconButton_icon_placement(iw) != XmIconOnly )
    {
	int        size, tmp;
	XRectangle clip;

	clip.x = XmIconButton_text_x(iw);
	clip.y = XmIconButton_text_y(iw);
	clip.width = XmIconButton_max_text_width(iw);
	clip.height = XmIconButton_max_text_height(iw);

	if( (int)clip.x < (int)H_SPACE(iw) ) clip.x = H_SPACE(iw);
	if( (int)clip.y < (int)V_SPACE(iw) ) clip.y = V_SPACE(iw);

	size = XtWidth(iw) - H_SPACE(iw);
	if( (int) (clip.x + clip.width) > size )
	{
	    tmp = size - (int)clip.x;
	    if( tmp < 0 ) tmp = 0;
	    clip.width = tmp;
	}

	size = XtHeight(iw) - V_SPACE(iw);
	if( (int)(clip.y + clip.height) > size )
	{
	    tmp = size - (int)clip.y;
	    if( tmp < 0 ) tmp = 0;
	    clip.height = tmp;
	}

	if( clip.width != 0 && clip.height != 0 )
	{
	    /*
	     * Set the clip rectangle so things do not
	     * overlap or go out of bounds.
	     */
	    XSetClipRectangles(XtDisplay(w), text_gc,
			       0, 0, &clip, 1, Unsorted);

		/*Draw shadow for insensitive text*/
		if (!XtIsSensitive(w)) {
			XmStringDraw(XtDisplay(w), XtWindow(w), XmIconButton_font_list(iw),
				XmIconButton_label_string(iw), XmIconButton_shadow_gc(iw),
				XmIconButton_text_x(iw)+1, XmIconButton_text_y(iw)+1,
				XmIconButton_max_text_width(iw), XmIconButton_alignment(iw),
				XmPrim_layout_direction(iw), NULL);
		}

	    XmStringDraw(XtDisplay(w), XtWindow(w), XmIconButton_font_list(iw),
			 XmIconButton_label_string(iw), text_gc,
			 XmIconButton_text_x(iw), XmIconButton_text_y(iw),
			 XmIconButton_max_text_width(iw), XmIconButton_alignment(iw),
			 XmPrim_layout_direction(iw), NULL);
	    XSetClipMask(XtDisplay(w), text_gc, None);
	}
    }
}

/*
 * There is almost always only 1 display, and certainly only a few, therefore
 * there is no need to be clever here, just make sure it works for one
 * fast, and doesn't break when using many.
 *
 * Would be nice to refocunt and remove...
 */

static Pixmap
GetGreyStipple(Widget w)
{
  StippleInfo *set, *ptr;

  for (ptr = stipple_cache; ptr != NULL; ptr = ptr->next) {
    /* Check for both screen and display, such that it displays */
    /* correctly on multi-headed X-servers.                     */
    /* Change Request #: CR03619                                */
    if (ptr->disp == XtDisplay(w) && ptr->screen == XtScreen(w)) {
      return (ptr->stipple);
    }
  }

  set = (StippleInfo *) XtMalloc(sizeof(StippleInfo));
  set->stipple = XCreateBitmapFromData(XtDisplay(w),
				       RootWindowOfScreen(XtScreen(w)),
				       gray_bits, gray_width, gray_height);
  set->disp = XtDisplay(w);
  set->screen = XtScreen(w);
  set->next = NULL;

  if (stipple_cache == NULL)
    stipple_cache = set;
  else
    for (ptr = stipple_cache; ptr != NULL; ptr = ptr->next)
      if (ptr->next == NULL) {
	ptr->next = set;
	break;
      }

  return(set->stipple);
}

/*	Function Name: CreateGCs
 *	Description:   Creates the Graphics contexts
 *	Arguments:     w - the icon button widget.
 *	Returns:       none
 */

static void
CreateGCs(Widget w)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;
    XtGCMask mask, smask;
    XGCValues values;
    Pixel fg, bg;
    Pixmap stipple;
    Arg args[2];
    Cardinal num_args = 0;
    XFontStruct *fs = NULL;

    XtSetArg(args[num_args], XmNforeground, &fg); num_args++;
    XtSetArg(args[num_args], XmNbackground, &bg); num_args++;
    XtGetValues(w, args, num_args);

    stipple = GetGreyStipple(w);

    XmeRenderTableGetDefaultFont(XmIconButton_font_list(iw), &fs);

    values.foreground = fg;
    values.background = bg;
    values.graphics_exposures = False;
    values.stipple = stipple;
    values.fill_style = FillStippled;

    mask = GCForeground | GCBackground | GCGraphicsExposures;
	smask = mask | GCFillStyle;

    if (fs) {
        values.font = fs->fid;
        mask |= GCFont;
    }

    XmIconButton_gc(iw) = XtGetGC(w, mask, &values);

    /*generally gray insensitive foreground (instead stipple)*/
    values.foreground = _XmAssignInsensitiveColor((Widget)iw);
    XmIconButton_insensitive_text_gc(iw) = XtGetGC(w, smask, &values);

    /*light shadow for insensitive text (instead stipple)*/
    values.foreground = iw->primitive.top_shadow_color;
    XmIconButton_shadow_gc(iw) = XtGetGC(w, smask, &values);

    /*
     * HACK ALERT: !!! Motif hack for monochrome displays. !!!
     *             If fg and arm color are the same then replace fg color
     *             with bg color.
     */

    if (values.foreground == XmIconButton_arm_color(iw))
	values.foreground = bg;

    /*
     * else just use the same fg color we used to be using.
     */

    values.background = XmIconButton_arm_color(iw);
    XmIconButton_pixmap_fill_gc(iw) = XtGetGC(w, mask, &values);
    XmIconButton_stippled_set_text_gc(iw) = XtGetGC(w, smask, &values);

    mask = GCForeground;
    values.foreground = XmIconButton_arm_color(iw);

    XmIconButton_fill_gc(iw) = XtGetGC(w, mask, &values);

    values.foreground = bg;
    smask = mask | GCStipple | GCFillStyle;
    XmIconButton_stippled_unset_gc(iw) = XtGetGC(w, smask, &values);

    values.foreground = XmIconButton_arm_color(iw);
    XmIconButton_stippled_set_gc(iw) = XtGetGC(w, smask, &values);
}

/*	Function Name: DestroyGCs
 *	Description:   Destroys all GC's needed by icon button.
 *	Arguments:     w - the icon button widget.
 *	Returns:       none.
 */

static void
DestroyGCs(Widget w)
{
    XmIconButtonWidget iw = (XmIconButtonWidget) w;

    XtReleaseGC(w, XmIconButton_gc(iw));
    XtReleaseGC(w, XmIconButton_fill_gc(iw));
    XtReleaseGC(w, XmIconButton_pixmap_fill_gc(iw));
    XtReleaseGC(w, XmIconButton_stippled_set_gc(iw));
    XtReleaseGC(w, XmIconButton_stippled_unset_gc(iw));
    XtReleaseGC(w, XmIconButton_shadow_gc(iw));
    XtReleaseGC(w, XmIconButton_insensitive_text_gc(iw));
    XtReleaseGC(w, XmIconButton_stippled_set_text_gc(iw));
}

/*	Function Name: CreateXmString
 *	Description: Given a char * create an Xm String.
 *                   an remember to put in the new lines.
 *	Arguments: str - the String.
 *	Returns: The Motif string for this string.
 */

static XmString
CreateXmString(Widget w, String str)
{
    XrmValue		fromVal, toVal;
    XmString xmstr;

    fromVal.size = strlen(str) + 1; /* space for NULL. */
    fromVal.addr = str;

    toVal.size = sizeof(XmString);
    toVal.addr = (XtPointer) &xmstr;

    if (XtConvertAndStore(w, XmRString, &fromVal, XmRXmString, &toVal)) {
	xmstr = XmStringCopy(xmstr); /* because I free it later. */
	return(xmstr);
    }
    return(NULL);
}

/*
 * XmRCallProc routine for checking font_list before setting it to NULL
 * If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offs,
		    XrmValue *value)
{
  XmIconButtonWidget lw = (XmIconButtonWidget)wid;

  /* Check if been here before */
  if (lw->icon.check_set_render_table)
      value->addr = NULL;
  else {
      lw->icon.check_set_render_table = True;
      value->addr = (char*)&(lw->icon.font_list);
  }

}

/*	Function name: ChangeCB
 *	Description: add or remove the activate callback list.
 *	Arguments:   w - the child widget ha� its list of callbacks modified
 *	             activCB - the callback to add or remove from
 *	             closure - additional data to be passed to the callback
 *	             setunset - set/unset flag
 *	Returns: none
 */

static void
ChangeCB(
	 Widget w,
	 XtCallbackProc activCB,
	 XtPointer closure,
	 Boolean setunset)
{
  if (setunset)
    XtAddCallback (w, XmNactivateCallback, activCB, closure);
  else
    XtRemoveCallback (w, XmNactivateCallback, activCB, closure);
}

/************************************************************
 *
 * Public routines.
 *
 ************************************************************/

/*	Function Name: XmCreateIconButton
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateIconButton(Widget parent, String name,
		   ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmIconButtonWidgetClass,
			  parent, args, num_args));
}

