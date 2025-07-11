/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ToggleB.c /main/43 1999/12/06 18:09:38 samborn $"
#endif
#endif

#include <stdio.h>
#include "XmI.h"
#include <X11/ShellP.h>
#include <Xm/BaseClassP.h>
#include <Xm/CascadeB.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleBP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "LabelI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"


#define XmINVALID_TYPE		255	/* default flag for IndicatorType */
#define XmINVALID_BOOLEAN	 85	/* default flag for VisibleWhenOff */
#define XmINVALID_PIXEL    ((Pixel) -1)	/* default flag for unselectColor */

#define MIN_GLYPH_SIZE		5	/* Threshold for rendering glyphs. */

#define PixmapOn(w)		((w)->toggle.on_pixmap)
#define PixmapOff(w)		((w)->label.pixmap)
#define PixmapInd(w)		((w)->toggle.indeterminate_pixmap)
#define PixmapInsenOn(w)	((w)->toggle.insen_pixmap)
#define PixmapInsenOff(w)	((w)->label.pixmap_insen)
#define PixmapInsenInd(w)	((w)->toggle.indeterminate_insensitive_pixmap)
#define IsNull(p)		((p) == XmUNSPECIFIED_PIXMAP)
#define IsOn(w)			((w)->toggle.visual_set)

#define IsOneOfMany(ind_type)	(((ind_type) == XmONE_OF_MANY) || \
				 ((ind_type) == XmONE_OF_MANY_ROUND) || \
				 ((ind_type) == XmONE_OF_MANY_DIAMOND))

/* Constants used to decompose XmNindicatorOn values. */
#define XmINDICATOR_BOX_MASK		0x0f
#define XmINDICATOR_GLYPH_MASK		0xf0

/* The indicator value should already have been normalized! */
#define DRAW3DBOX(ind_on)	((ind_on) & XmINDICATOR_3D_BOX)
#define DRAWFLATBOX(ind_on)	((ind_on) & XmINDICATOR_FLAT_BOX)
#define DRAWBOX(ind_on)		((ind_on) & XmINDICATOR_BOX_MASK)
#define DRAWCHECK(ind_on)	((ind_on) & XmINDICATOR_CHECK_GLYPH)
#define DRAWCROSS(ind_on)	((ind_on) & XmINDICATOR_CROSS_GLYPH)
#define DRAWGLYPH(ind_on)	((ind_on) & XmINDICATOR_GLYPH_MASK)


/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void InitializePrehook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void SetAndDisplayPixmap(
                        XmToggleButtonWidget tb,
                        XEvent *event,
                        Region region) ;
static void Help(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ToggleButtonCallback(
                        XmToggleButtonWidget data,
                        unsigned int reason,
                        unsigned int value,
                        XEvent *event) ;
static void Leave(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Enter(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Arm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Select(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Disarm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void BtnDown(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void BtnUp(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void GetGC(
                        XmToggleButtonWidget tw) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget wid) ;
static void DrawToggle(
                        XmToggleButtonWidget w) ;
static void BorderHighlight(
                        Widget wid) ;
static void BorderUnhighlight(
                        Widget wid) ;
static void KeySelect(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void ComputeSpace(
                        XmToggleButtonWidget tb) ;
static void Redisplay(
                        Widget w,
                        XEvent *event,
                        Region region) ;
static void Resize(
                        Widget w) ;
static Boolean SetValuesPrehook(
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void DrawToggleShadow(
                        XmToggleButtonWidget tb) ;
static void DrawToggleLabel(
                        XmToggleButtonWidget tb) ;
static void DrawEtchedInMenu(
                        XmToggleButtonWidget tb) ;
static void SetToggleSize(
                        XmToggleButtonWidget newtb) ;
static void NextState(unsigned char *state);
static void DrawBox(
		    XmToggleButtonWidget w,
		    GC top_gc,
		    GC bot_gc,
		    GC fillgc,
		    int x, int y, int edge,
		    Dimension margin
		    );

static void DefaultSelectColor(Widget    widget,
			       int       offset,
			       XrmValue *value);

static unsigned char NormalizeIndOn(XmToggleButtonWidget tb);
static unsigned char NormalizeIndType(XmToggleButtonWidget tb);
static void TB_FixTearoff( XmToggleButtonWidget tb);


/********    End Static Function Declarations    ********/



/*************************************<->*************************************
 *
 *
 *   Description:  default translation table for class: ToggleButton
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/
static XtTranslations default_parsed;

#define defaultTranslations	_XmToggleB_defaultTranslations

static XtTranslations menu_parsed;

#define menuTranslations	_XmToggleB_menuTranslations

/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: ToggleButton
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  {"Arm", 	     Arm            },
  {"ArmAndActivate", ArmAndActivate },
  {"Disarm", 	     Disarm         },
  {"Select", 	     Select         },
  {"Enter", 	     Enter          },
  {"Leave", 	     Leave          },
  {"BtnDown",        BtnDown        },
  {"BtnUp",          BtnUp          },
  {"ButtonTakeFocus", _XmButtonTakeFocus },
  {"MenuButtonTakeFocus", _XmMenuButtonTakeFocus },
  {"MenuButtonTakeFocusUp", _XmMenuButtonTakeFocusUp },
  {"KeySelect",      KeySelect      },
  {"Help",           Help},
};




/*************************************<->*************************************
 *
 *
 *   Description:  resource list for class: ToggleButton
 *   -----------
 *
 *   Provides default resource settings for instances of this class.
 *   To get full set of default settings, examine resouce list of super
 *   classes of this class.
 *
 *************************************<->***********************************/

#define Offset(field)	(XtOffsetOf(XmToggleButtonRec, field))

static XtResource resources[] = {
  {
    XmNindicatorSize, XmCIndicatorSize, XmRVerticalDimension,
    sizeof(Dimension), Offset(toggle.indicator_dim),
    XmRImmediate, (XtPointer) XmINVALID_DIMENSION
  },

  {
    XmNindicatorType, XmCIndicatorType, XmRIndicatorType,
    sizeof(unsigned char), Offset(toggle.ind_type),
    XmRImmediate, (XtPointer) XmINVALID_TYPE
  },

  {
    XmNvisibleWhenOff, XmCVisibleWhenOff, XmRBoolean,
    sizeof(Boolean), Offset(toggle.visible),
    XmRImmediate, (XtPointer) XmINVALID_BOOLEAN
  },

  {
    XmNspacing, XmCSpacing, XmRHorizontalDimension,
    sizeof(Dimension), Offset(toggle.spacing),
    XmRImmediate, (XtPointer) 4
  },

  {
    XmNselectPixmap, XmCSelectPixmap, XmRDynamicPixmap,
    sizeof(Pixmap), Offset(toggle.on_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNselectInsensitivePixmap, XmCSelectInsensitivePixmap, XmRDynamicPixmap,
    sizeof(Pixmap), Offset(toggle.insen_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNset, XmCSet, XmRSet,
    sizeof(unsigned char), Offset(toggle.set),
    XmRImmediate, (XtPointer) XmUNSET
  },

  {
    XmNindicatorOn, XmCIndicatorOn, XmRIndicatorOn,
    sizeof (unsigned char), Offset(toggle.ind_on),
    XmRImmediate, (XtPointer) XmINDICATOR_FILL
  },

  {
    XmNfillOnSelect, XmCFillOnSelect, XmRBoolean,
    sizeof(Boolean), Offset(toggle.fill_on_select),
    XmRImmediate, (XtPointer) XmINVALID_BOOLEAN
  },

  {
    XmNselectColor, XmCSelectColor, XmRSelectColor,
    sizeof(Pixel), Offset(toggle.select_color),
    XmRCallProc, (XtPointer) DefaultSelectColor
  },

  {
    XmNvalueChangedCallback, XmCValueChangedCallback, XmRCallback,
    sizeof (XtCallbackList), Offset(toggle.value_changed_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNarmCallback, XmCArmCallback, XmRCallback,
    sizeof(XtCallbackList), Offset(toggle.arm_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNdisarmCallback, XmCDisarmCallback, XmRCallback,
    sizeof (XtCallbackList), Offset(toggle.disarm_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNtraversalOn, XmCTraversalOn, XmRBoolean,
    sizeof(Boolean), Offset(primitive.traversal_on),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(primitive.highlight_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  },

  {
    XmNtoggleMode, XmCToggleMode, XmRToggleMode,
    sizeof(unsigned char), Offset(toggle.toggle_mode),
    XmRImmediate, (XtPointer) XmTOGGLE_BOOLEAN
  },

  {
    XmNindeterminatePixmap, XmCIndeterminatePixmap, XmRDynamicPixmap,
    sizeof(Pixmap), Offset(toggle.indeterminate_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNindeterminateInsensitivePixmap, XmCIndeterminateInsensitivePixmap,
    XmRDynamicPixmap,
    sizeof(Pixmap), Offset(toggle.indeterminate_insensitive_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNunselectColor, XmCUnselectColor, XmRPixel,
    sizeof(Pixel), Offset(toggle.unselect_color),
    XmRImmediate, (XtPointer) XmINVALID_PIXEL
  },

  {
    XmNdetailShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(toggle.detail_shadow_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  }
};

/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
   {
     XmNspacing,
     sizeof(Dimension), Offset(toggle.spacing),
     XmeFromHorizontalPixels, XmeToHorizontalPixels
   },

   {
     XmNindicatorSize,
     sizeof(Dimension), Offset(toggle.indicator_dim),
     XmeFromVerticalPixels, XmeToVerticalPixels
   },

   {
     XmNdetailShadowThickness,
     sizeof(Dimension), Offset(toggle.detail_shadow_thickness),
     XmeFromHorizontalPixels, XmeToHorizontalPixels
   }
};

#undef Offset

/*************************************<->*************************************
 *
 *
 *   Description:  global class record for instances of class: ToggleButton
 *   -----------
 *
 *   Defines default field settings for this class record.
 *
 *************************************<->***********************************/
static XmBaseClassExtRec       toggleBBaseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    InitializePrehook,                        /* initialize prehook   */
    SetValuesPrehook,                         /* set_values prehook   */
    InitializePosthook,                       /* initialize posthook  */
    XmInheritSetValuesPosthook,               /* set_values posthook  */
    XmInheritClass,                           /* secondary class      */
    XmInheritSecObjectCreate,                 /* creation proc        */
    XmInheritGetSecResData,                   /* getSecResData        */
    {0},                                      /* fast subclass        */
    XmInheritGetValuesPrehook,                /* get_values prehook   */
    XmInheritGetValuesPosthook,               /* get_values posthook  */
    (XtWidgetClassProc)NULL,                  /* classPartInitPrehook */
    (XtWidgetClassProc)NULL,                  /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
  };


externaldef(xmtogglebuttonclassrec)
	XmToggleButtonClassRec xmToggleButtonClassRec = {
   {
    /* superclass	  */	(WidgetClass) &xmLabelClassRec,
    /* class_name	  */	"XmToggleButton",
    /* widget_size	  */	sizeof(XmToggleButtonRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    (XtArgsProc)NULL,
    /* realize		  */	XmInheritRealize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressMaximal,
    /* compress_enterlv   */    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    (XtArgsFunc)NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	(XtArgsProc)NULL,
    /* accept_focus       */    (XtAcceptFocusProc)NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/    (XtStringProc)NULL,
    /* extension record   */    (XtPointer)&toggleBBaseClassExtRec,
   },

   {
    /* Primitive border_highlight   */	BorderHighlight,
    /* Primitive border_unhighlight */  BorderUnhighlight,
    /* translations                 */ 	XtInheritTranslations,
    /* arm_and_activate             */  ArmAndActivate,
    /* syn resources                */  syn_resources,
    /* num syn_resources            */  XtNumber(syn_resources),
    /* extension                    */  NULL,
   },

   {
    /* SetOverrideCallback     */    XmInheritWidgetProc,
    /* menu procedures         */    XmInheritMenuProc,
    /* menu traversal xlation  */    XtInheritTranslations,
    /* extension               */    NULL,
   },

   {
    /* extension               */    (XtPointer) NULL,
   }
};

externaldef(xmtogglebuttonwidgetclass)
   WidgetClass xmToggleButtonWidgetClass = (WidgetClass)&xmToggleButtonClassRec;

/* Menu Savvy trait record */
static XmMenuSavvyTraitRec MenuSavvyRecord = {
    /* version: */
    -1,
    NULL,
    NULL,
    NULL,
    _XmCBNameValueChanged,
};

/*************************************<->*************************************
 *
 *  ClassInitialize
 *
 *************************************<->***********************************/
static void
ClassInitialize( void )
{
  /* Parse the various translation tables. */
  default_parsed = XtParseTranslationTable(defaultTranslations);
  menu_parsed    = XtParseTranslationTable(menuTranslations);

  /* Set up base class extension quark */
  toggleBBaseClassExtRec.record_type = XmQmotif;
}

/*****************************************************************************
 *
 * ClassPartInitialize
 *   Set up fast subclassing for the widget.
 *
 ****************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmTOGGLE_BUTTON_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabel */
  _XmLabelCloneMenuSavvy (wc, &MenuSavvyRecord);
}

/************************************************************
 *
 * InitializePrehook
 *
 * Put the proper translations in core_class tm_table so that
 * the data is massaged correctly
 *
 ************************************************************/
/*ARGSUSED*/
static void
InitializePrehook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  unsigned char type;
  XmMenuSystemTrait menuSTrait;
  XmToggleButtonWidget bw = (XmToggleButtonWidget) new_w ;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(new_w)), XmQTmenuSystem);

  _XmSaveCoreClassTranslations (new_w);

  if (menuSTrait != NULL)
    type = menuSTrait->type(XtParent(new_w));
  else
    type = XmWORK_AREA;

  _XmProcessLock();
  if ((type == XmMENU_PULLDOWN) ||
      (type == XmMENU_POPUP))
    new_w->core.widget_class->core_class.tm_table = (String) menu_parsed;
  else
    new_w->core.widget_class->core_class.tm_table = (String) default_parsed;
  _XmProcessUnlock();

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (bw->label.font == NULL)
    bw->label.font = XmeGetDefaultRenderTable (new_w, XmBUTTON_FONTLIST);
}

/************************************************************
 *
 * InitializePosthook
 *
 * restore core class translations
 *
 ************************************************************/
/*ARGSUSED*/
static void
InitializePosthook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  _XmRestoreCoreClassTranslations (new_w);
}

/*********************************************************************
 *
 * redisplayPixmap
 *   does the apropriate calculations based on the toggle button's
 *   current pixmap and calls label's Redisplay routine.
 *
 * This routine was added to fix CR 4839 and CR 4838
 * D. Rand 7/6/92
 *
 ***********************************************************************/

static void
redisplayPixmap(XmToggleButtonWidget tb,
		XEvent *event,
		Region region)
{
  Pixmap todo;
  unsigned int onH = 0, onW = 0;
  int w, h;
  int x, y, offset;
  short saveY;
  unsigned short saveWidth, saveHeight;

  offset = tb->primitive.highlight_thickness + tb->primitive.shadow_thickness;

  x = offset + tb->label.margin_width + tb->label.margin_left;

  y = offset + tb->label.margin_height + tb->label.margin_top;

  w = XtWidth(tb) - x - offset - tb->label.margin_right
    - tb->label.margin_width;

  ASSIGN_MAX(w, 0);

  h = XtHeight(tb) - y - offset - tb->label.margin_bottom
    - tb->label.margin_height;

  ASSIGN_MAX(h, 0);

  XClearArea(XtDisplay(tb), XtWindow(tb), x, y, w, h, False);

  todo = tb->label.pixmap;

  if ((! XtIsSensitive((Widget) tb)) && tb->label.pixmap_insen)
    todo = tb->label.pixmap_insen;

  if (! IsNull(todo))
    XmeGetPixmapData(XtScreen(tb), todo,
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW, &onH);

  saveY = Lab_TextRect_y(tb);
  saveWidth = Lab_TextRect_width(tb);
  saveHeight = Lab_TextRect_height(tb);

  h = (XtHeight(tb) - onH) / 2;
  Lab_TextRect_y(tb) = MAX(h, 0);
  Lab_TextRect_height(tb) = onH;
  Lab_TextRect_width(tb) = onW;
  {
    XtExposeProc expose;
    _XmProcessLock();
    expose = xmLabelClassRec.core_class.expose;
    _XmProcessUnlock();
    (* expose) ((Widget) tb, event, region);
  }

  Lab_TextRect_y(tb) = saveY;
  Lab_TextRect_width(tb) = saveWidth;
  Lab_TextRect_height(tb) = saveHeight;
}

static void
HandlePixmap(XmToggleButtonWidget tb,
	     Pixmap pix,
	     Pixmap insen_pix,
	     XEvent * event,
	     Region region)
{
  if (XtIsSensitive((Widget) tb))
    {
      if (! IsNull (pix))
	{
	  Pixmap tempPix = tb->label.pixmap;

	  tb->label.pixmap = pix;
	  redisplayPixmap(tb, event, region);
	  tb->label.pixmap = tempPix;
	}
      else
	redisplayPixmap(tb, event, region);
    }
  else
    {
      if (! IsNull (insen_pix))
	{
	  Pixmap tempPix = tb->label.pixmap_insen;

	  tb->label.pixmap_insen = insen_pix;
	  redisplayPixmap(tb, event, region);
	  tb->label.pixmap_insen = tempPix;
	}
      else
	redisplayPixmap(tb, event, region);
    }
}

/***********************************************************************
 *
 * SetAndDisplayPixmap
 *   Sets the appropriate on, off pixmap in label's pixmap field and
 *   calls redisplayPixmap
 *
 ***********************************************************************/
static void
SetAndDisplayPixmap(
        XmToggleButtonWidget tb,
        XEvent *event,
        Region region )
{
  if (tb->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
    {
      if (tb->toggle.visual_set == XmUNSET)
	HandlePixmap(tb, PixmapOff(tb), PixmapInsenOff(tb), event, region);
      else if (tb->toggle.visual_set == XmSET)
	HandlePixmap(tb, PixmapOn(tb), PixmapInsenOn(tb), event, region);
      else if (tb->toggle.visual_set == XmINDETERMINATE)
	HandlePixmap(tb, PixmapInd(tb), PixmapInsenInd(tb), event, region);
    }
  else
    {
      if (IsOn (tb) == XmSET)
	HandlePixmap(tb, PixmapOn(tb), PixmapInsenOn(tb), event, region);
      else
	redisplayPixmap(tb, event, region);
    }
}

/*************************************************************************
 *
 *  Help
 *     This routine is called if the user has made a help selection
 *     on the widget.
 *
 ************************************************************************/
static void
Help(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
   Boolean is_menupane = Lab_IsMenupane(tb);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

   if (is_menupane && menuSTrait != NULL)
     menuSTrait->buttonPopdown(XtParent(tb), event);

   _XmPrimitiveHelp( (Widget) tb, event, params, num_params);

   if (is_menupane && menuSTrait != NULL)
     menuSTrait->reparentToTearOffShell(XtParent(tb), event);
}

/*************************************************************************
 *
 * ToggleButtonCallback
 *   This is the widget's application callback routine
 *
 *************************************************************************/
static void
ToggleButtonCallback(
        XmToggleButtonWidget data,
        unsigned int reason,
        unsigned int value,
        XEvent *event )
{
  XmToggleButtonCallbackStruct temp;

  temp.reason = reason;
  temp.set = value;
  temp.event = event;

  switch (reason)
    {
    case XmCR_VALUE_CHANGED:
      XtCallCallbackList ((Widget) data, data->toggle.value_changed_CB, &temp);
      break;

    case XmCR_ARM:
      XtCallCallbackList ((Widget) data, data->toggle.arm_CB, &temp);
      break;

    case XmCR_DISARM:
      XtCallCallbackList ((Widget) data, data->toggle.disarm_CB, &temp);
      break;
    }
}

/* Update the toggle after an Enter or Leave action. */
static void
ActionDraw(XmToggleButtonWidget w,
	   XEvent              *event,
	   Boolean              leave)
{
  if (w->toggle.Armed)
    {
      /* CR 7301: We may have armed while outside the toggle. */
      if (leave)
	w->toggle.visual_set = w->toggle.set;
      else if (w->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
	NextState(&w->toggle.visual_set);
      else
	IsOn(w) = (w->toggle.set == XmSET)? XmUNSET : XmSET;

      if (w->toggle.ind_on)
	DrawToggle(w);
      else
	{
	  if (w->primitive.shadow_thickness > 0)
	    DrawToggleShadow(w);
	  if (w->toggle.fill_on_select && !Lab_IsPixmap(w))
	    DrawToggleLabel(w);
	}

      if (Lab_IsPixmap(w) || Lab_IsPixmapAndText(w))
	SetAndDisplayPixmap(w, event, NULL);
    }
}

/**************************************************************************
 *
 * Leave
 *  This procedure is called when  the mouse button is pressed and  the
 *  cursor moves out of the widget's window. This procedure is used
 *  to change the visuals.
 *
*************************************************************************/
static void
Leave(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget w = (XmToggleButtonWidget) wid;

  if (Lab_IsMenupane(w))
    {
      if (_XmGetInDragMode((Widget)w) && w->toggle.Armed &&
	  (/* !ActiveTearOff || */ event->xcrossing.mode == NotifyNormal))
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	  Boolean etched_in = dpy->display.enable_etched_in_menu;

	  w->toggle.Armed = FALSE;

	  ((XmManagerWidget) XtParent(wid))->manager.active_child = NULL;

	  if ((etched_in) &&
	      ((w->toggle.ind_on) ||
	       (!(w->toggle.ind_on) && !(w->toggle.fill_on_select))))
	    {
		DrawEtchedInMenu(w);
		if (w->toggle.ind_on)
		    DrawToggle(w);
	    }

	  XmeClearBorder(XtDisplay (w), XtWindow (w),
			 w->primitive.highlight_thickness,
			 w->primitive.highlight_thickness,
			 w->core.width - 2 * w->primitive.highlight_thickness,
			 w->core.height - 2 * w->primitive.highlight_thickness,
			 w->primitive.shadow_thickness);

	  if (w->toggle.disarm_CB)
	    {
	      XFlush (XtDisplay (w));
	      ToggleButtonCallback(w, XmCR_DISARM, w->toggle.set, event);
	    }
	}
    }
  else
    {
      _XmPrimitiveLeave( (Widget) w,  event, params, num_params);
      ActionDraw(w, event, TRUE);
    }
}

/**************************************************************************
 *
 * Enter
 *   This procedure is called when the mouse button is pressed and the
 *   cursor reenters the widget's window. This procedure changes the visuals
 *   accordingly.
 *
 **************************************************************************/
static void
Enter(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   XmToggleButtonWidget w = (XmToggleButtonWidget) wid ;

   if (Lab_IsMenupane(w))
   {
      if ((((ShellWidget) XtParent(XtParent(w)))->shell.popped_up) &&
	  _XmGetInDragMode((Widget)w))
      {
	 XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	 Boolean etched_in = dpy->display.enable_etched_in_menu;

	 if (w->toggle.Armed)
	    return;

	 /* So KHelp event is delivered correctly */
	 _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, TRUE);
	 XtSetKeyboardFocus(XtParent(XtParent(w)), (Widget)w);
	 _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, FALSE);

	 ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	 w->toggle.Armed = TRUE;

	 if ((etched_in) &&
	     ((w->toggle.ind_on) ||
	      (!(w->toggle.ind_on) && !(w->toggle.fill_on_select))))
	   {
	       DrawEtchedInMenu(w);
	       if (w->toggle.ind_on)
		   DrawToggle(w);
	   }

	 XmeDrawShadows (XtDisplay (w), XtWindow (w),
		w->primitive.top_shadow_GC,
		w->primitive.bottom_shadow_GC,
		w->primitive.highlight_thickness,
		w->primitive.highlight_thickness,
		w->core.width - 2 * w->primitive.highlight_thickness,
		w->core.height - 2 * w->primitive.highlight_thickness,
		w->primitive.shadow_thickness,
		etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	 if (w->toggle.arm_CB)
	 {
	    XFlush (XtDisplay (w));
	    ToggleButtonCallback(w, XmCR_ARM, w->toggle.set, event);
	 }
      }
   }
   else
   {
      _XmPrimitiveEnter( (Widget) w, event, params, num_params);
      ActionDraw(w, event, FALSE);
    }
}

static void
NextState(
    unsigned char *state)
{
  switch(*state)
    {
    case XmUNSET:
      *state = XmSET;
      break;
    case XmSET:
      *state = XmINDETERMINATE;
      break;
    case XmINDETERMINATE:
      *state = XmUNSET;
      break;
    }
}

/****************************************************************************
 *
 *     Arm
 *       This function processes button down occuring on the togglebutton.
 *       Mark the togglebutton as armed and display it armed.
 *       The callbacks for XmNarmCallback are called.
 *
 ***************************************************************************/

/* ARGSUSED */
static void
Arm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;

  (void)XmProcessTraversal( (Widget) tb, XmTRAVERSE_CURRENT);

  if (tb->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
    NextState(&tb->toggle.visual_set);
  else
    IsOn(tb) = (tb->toggle.set == XmSET)? XmUNSET : XmSET;

  tb->toggle.Armed = TRUE;

  if (tb->toggle.ind_on)
    DrawToggle(tb);
  else
    {
      if (tb->primitive.shadow_thickness > 0)
	DrawToggleShadow (tb);
      if (tb->toggle.fill_on_select && !Lab_IsPixmap(tb))
	DrawToggleLabel(tb);
    }

  if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
    SetAndDisplayPixmap(tb, event, NULL);

  if (tb->toggle.arm_CB)
    {
      XFlush(XtDisplay(tb));

      ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
    }
}

/************************************************************************
 *
 *     Select
 *       Mark the togglebutton as unarmed (i.e. inactive).
 *       If the button release occurs inside of the ToggleButton, the
 *       callbacks for XmNvalueChangedCallback are called.
 *
 ************************************************************************/
/* ARGSUSED */
static void
Select(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XmToggleButtonCallbackStruct call_value;
  Boolean hit;
  XmMenuSystemTrait menuSTrait;
  Boolean radio = TRUE, always = TRUE;

  if (tb->toggle.Armed == FALSE)
    return;

  tb->toggle.Armed = FALSE;

  if (XmIsRowColumn(XtParent(tb))) {
    XtVaGetValues(XtParent(tb),
    XmNradioBehavior, &radio,
    XmNradioAlwaysOne, &always,
    NULL);
  }

  /* CR 8068: Verify that this is in fact a button event. */
  /* CR 9181: Consider clipping when testing visibility. */
  /* Check to see if BtnUp is inside the widget */
  hit = ((event->xany.type == ButtonPress ||
	  event->xany.type == ButtonRelease) &&
	 _XmGetPointVisibility(wid,
			       event->xbutton.x_root,
			       event->xbutton.y_root));

  if (hit)
    {
      if (tb->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
	NextState(&tb->toggle.set);
      else
	tb->toggle.set = !tb->toggle.set;
    }

  /* CR 7803: Suppress redundant redraws. */
  if (tb->toggle.set != tb->toggle.visual_set)
    {
      /* Redisplay after changing state. */
      XtExposeProc expose;
      _XmProcessLock();
      expose = ((WidgetClass)XtClass(tb))->core_class.expose;
      _XmProcessUnlock();
      (* (expose)) (wid, event, NULL);
    }

  if (hit)
    {
      /* UNDOING this fix .... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /*	reported correctly even if the entry callback resets it. */
      menuSTrait = (XmMenuSystemTrait)
	XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

      if (menuSTrait != NULL)
	{
	  call_value.reason = XmCR_VALUE_CHANGED;
	  call_value.event = event;
	  call_value.set = tb->toggle.set;
	  menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
	}

      if ((! tb->label.skipCallback) &&
	  (tb->toggle.value_changed_CB))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, tb->toggle.set, event);
	}

    }
}

/**********************************************************************
 *
 *    Disarm
 *      The callbacks for XmNdisarmCallback are called..
 *
 ************************************************************************/
/* ARGSUSED */
static void
Disarm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;

  if (tb->toggle.disarm_CB)
    ToggleButtonCallback(tb, XmCR_DISARM, tb->toggle.set, event);

/* BEGIN OSF Fix pir 2826 */

  /* CR 7803:  Suppress redundant redraws. */
  if (tb->toggle.set != tb->toggle.visual_set)
    Redisplay((Widget) tb, event, (Region) NULL);

/* END OSF Fix pir 2826 */
}

static void
TB_FixTearoff( XmToggleButtonWidget tb)
{
	 if  (XmMENU_PULLDOWN == tb->label.menu_type)
	 {
		Widget mwid = XmGetPostedFromWidget(XtParent(tb));
		if (mwid && XmIsRowColumn(mwid)
			&& (XmMENU_OPTION == RC_Type(mwid))
			&& _XmIsActiveTearOff(XtParent(tb)))
			XmProcessTraversal((Widget) tb, XmTRAVERSE_CURRENT);
	 }
}

/************************************************************************
 *
 *     ArmAndActivate
 *       This routine arms and activates a ToggleButton. It is called on
 *       <Key> Return and a <Key> Space, as well as when a mnemonic or
 *       button accelerator has been activated.
 *
 ************************************************************************/
/* ARGSUSED */
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XmToggleButtonCallbackStruct call_value;
  Boolean already_armed = tb->toggle.Armed;
  Boolean is_menupane = Lab_IsMenupane(tb);
  Boolean torn_has_focus = FALSE;
  XmMenuSystemTrait menuSTrait;
  Boolean radio = TRUE, always = TRUE, no_change;

  if (XmIsRowColumn(XtParent(tb))) {
    XtVaGetValues(XtParent(tb),
    XmNradioBehavior, &radio,
    XmNradioAlwaysOne, &always,
    NULL);
  }

  no_change = False;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

  if (is_menupane && !XmIsMenuShell(XtParent(XtParent(tb))))
    {
      if (XmeFocusIsInShell((Widget)tb))
	{
	  /* In case allowAcceleratedInsensitiveUnmanagedMenuItems is True */
	  if (!XtIsSensitive((Widget)tb) || (!XtIsManaged((Widget)tb)))
            return;
	  torn_has_focus = TRUE;
	}
    }

  tb->toggle.Armed = FALSE;

  if (!no_change) /* skip toggle for buttons already turned on */
    {
      if (tb->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
        {
          NextState(&tb->toggle.visual_set);
          NextState(&tb->toggle.set);
        }
      else
        {
          tb->toggle.set = (tb->toggle.set == XmSET)? XmUNSET : XmSET;
          IsOn(tb) = tb->toggle.set;
        }
    }

  if (is_menupane && menuSTrait != NULL)
    {
      /* CR 7799: Torn off menus shouldn't be shared, so don't reparent. */
      if (torn_has_focus)
	menuSTrait->popdown(XtParent(tb), event);
      else
	menuSTrait->buttonPopdown(XtParent(tb), event);

      if (torn_has_focus)
	XmProcessTraversal((Widget) tb, XmTRAVERSE_CURRENT);

      /* Draw the toggle indicator in case of tear off */
      if (tb->toggle.ind_on)
	DrawToggle(tb);
      else if (tb->toggle.fill_on_select && !Lab_IsPixmap(tb))
	DrawToggleLabel(tb);

      if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
	SetAndDisplayPixmap( tb, event, NULL);
    }
  else
    {
      if (tb->toggle.ind_on)
	DrawToggle(tb);
      else
	{
	  if (tb->primitive.shadow_thickness > 0)
	    DrawToggleShadow (tb);
	  if (tb->toggle.fill_on_select && !Lab_IsPixmap(tb))
	    DrawToggleLabel (tb);
	}
      if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
	SetAndDisplayPixmap( tb, event, NULL);
    }

  /* If the parent is menu system able, set the lastSelectToplevel before
   * the arm. It's ok if this is recalled later.
   */
  if (menuSTrait != NULL)
    menuSTrait->getLastSelectToplevel(XtParent(tb));

  if (tb->toggle.arm_CB && !already_armed)
    {
      XFlush(XtDisplay(tb));
      ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
    }

  if (!no_change)
  {
  /* UNDOING this fix .... */
  /* CR 8904: Notify value_changed before entry so that state is */
  /* 	reported correctly even if the entry callback resets it. */
  /* if the parent is menu system able, notify it about the select */

  if (menuSTrait != NULL)
    {
      call_value.reason = XmCR_VALUE_CHANGED;
      call_value.event = event;
      call_value.set = tb->toggle.set;
      menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
    }

  if ((! tb->label.skipCallback) &&
      (tb->toggle.value_changed_CB))
    {
      XFlush(XtDisplay(tb));
      ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, tb->toggle.set, event);
    }
  }

  if (tb->toggle.disarm_CB)
    {
      XFlush(XtDisplay(tb));
      ToggleButtonCallback(tb, XmCR_DISARM, tb->toggle.set, event);
    }

  if (is_menupane)
    {
      if (torn_has_focus && XtIsSensitive(wid))
	{
	  tb->toggle.Armed = TRUE;
	  if (tb->toggle.arm_CB)
	    {
	      XFlush(XtDisplay(tb));
	      ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
	    }
	}
      else if (menuSTrait != NULL)
	{
	  menuSTrait->reparentToTearOffShell(XtParent(tb), event);
	  TB_FixTearoff(tb);
	}
    }
}

/************************************************************************
 *
 *     BtnDown
 *       This function processes a button down occuring on the togglebutton
 *       when it is in a popup, pulldown, or option menu.
 *       Popdown the posted menu.
 *       Turn parent's traversal off.
 *       Mark the togglebutton as armed (i.e. active).
 *       The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/
/* ARGSUSED */
static void
BtnDown(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  Boolean validButton = False;
  Boolean already_armed;
  ShellWidget popup;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

  if (menuSTrait == NULL) return;

  /* Support menu replay, free server input queue until next button event */
  XAllowEvents(XtDisplay(tb), SyncPointer, CurrentTime);

  already_armed = tb->toggle.Armed;

  tb->toggle.Armed = TRUE;

  if (event && (event->type == ButtonPress))
    {
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;

	if ((etched_in)  &&
	    ((tb->toggle.ind_on) ||
	     (!(tb->toggle.ind_on) && !(tb->toggle.fill_on_select))))
          {
	      DrawEtchedInMenu(tb);
	      if (tb->toggle.ind_on)
		  DrawToggle(tb);
	  }

	validButton = menuSTrait->verifyButton(XtParent(tb), event);
    }

  if (!validButton)
    return;

  _XmSetInDragMode((Widget)tb, True);

  /* Popdown other popups that may be up */
  if (!(popup = (ShellWidget)_XmGetRC_PopupPosted(XtParent(tb))))
    {
      if (!XmIsMenuShell(XtParent(XtParent(tb))))
	{
	  /* In case tear off not armed and no grabs in place, do it now.
	   * Ok if already armed and grabbed - nothing done.
	   */
	  menuSTrait->tearOffArm(XtParent(tb));
	}
    }

  if  (popup)
    {
      /* Widget w; */

      if (popup->shell.popped_up)
	menuSTrait->popdownEveryone((Widget) popup, event);

      /* If the active_child is a cascade (highlighted), then unhighlight it.*/
      /*
       * w = ((XmManagerWidget)XtParent(tb))->manager.active_child;
       * if (w && (XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w)))
       *   XmCascadeButtonHighlight (w, FALSE);
       */
    }

  /* Set focus to this button.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal( (Widget) tb, XmTRAVERSE_CURRENT);
  /* get the location cursor - get consistent with Gadgets */

  if (tb->toggle.arm_CB && !already_armed)
    {
      XFlush (XtDisplay (tb));

      ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
    }

  _XmRecordEvent(event);
}

/************************************************************************
 *
 *     BtnUp
 *       This function processes a button up occuring on the togglebutton
 *       when it is in a popup, pulldown, or option menu.
 *       Mark the togglebutton as unarmed (i.e. inactive).
 *       The callbacks for XmNvalueChangedCallback are called.
 *       The callbacks for XmNdisarmCallback are called.
 *
 ************************************************************************/
/* ARGSUSED */
static void
BtnUp(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XmToggleButtonCallbackStruct call_value;
  Boolean validButton = False;
  Boolean popped_up;
  Boolean is_menupane = Lab_IsMenupane(tb);
  Widget shell = XtParent(XtParent(tb));
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

  if (menuSTrait == NULL) return;

  if (event && (event->type == ButtonRelease))
    validButton = menuSTrait->verifyButton(XtParent(tb), event);

  if (!validButton || (tb->toggle.Armed == FALSE))
    return;

  tb->toggle.Armed = FALSE;

  if (is_menupane && !XmIsMenuShell(shell))
    popped_up = menuSTrait->popdown((Widget) tb, event);
  else
    popped_up = menuSTrait->buttonPopdown((Widget) tb, event);

  _XmRecordEvent(event);

  if (popped_up)
    return;

  /* Check to see if BtnUp is inside the widget */
  /* CR 9181: Consider clipping when testing visibility. */
  if ((event->xany.type == ButtonPress || event->xany.type == ButtonRelease) &&
      _XmGetPointVisibility(wid, event->xbutton.x_root, event->xbutton.y_root))
    {
      if (tb->toggle.toggle_mode == XmTOGGLE_INDETERMINATE)
	{
	  NextState(&tb->toggle.visual_set);
	  NextState(&tb->toggle.set);
	}
      else
	{
	  tb->toggle.set = (tb->toggle.set == XmSET)? XmUNSET : XmSET;
	  IsOn(tb) = tb->toggle.set;
	}

      /* UNDOING this fix ... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /* 	reported correctly even if the entry callback resets it. */
      /* if the parent is menu system able, notify it about the select */
      if (menuSTrait != NULL)
	{
	  call_value.reason = XmCR_VALUE_CHANGED;
	  call_value.event = event;
	  call_value.set = tb->toggle.set;
	  menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
	}

      if ((! tb->label.skipCallback) &&
	  (tb->toggle.value_changed_CB))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, tb->toggle.set, event);
	}

      if (tb->toggle.disarm_CB)
	ToggleButtonCallback(tb, XmCR_DISARM, tb->toggle.set, event);

      if (is_menupane)
	{
	  if (!XmIsMenuShell(shell))
	    {
	      if (XtIsSensitive((Widget)tb))
		{
		  tb->toggle.Armed = TRUE;

		  if (tb->toggle.ind_on)
		    DrawToggle(tb);
		  else if (tb->toggle.fill_on_select && !Lab_IsPixmap(tb))
		    DrawToggleLabel(tb);

		  if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
		    SetAndDisplayPixmap( tb, event, NULL);

		  if (tb->toggle.arm_CB)
		    {
		      XFlush(XtDisplay(tb));
		      ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
		    }
		}
	    }
	  else
	    menuSTrait->reparentToTearOffShell(XtParent(tb), event);
	}
    }

  _XmSetInDragMode((Widget)tb, False);

  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  if (!XmIsMenuShell(shell))
    XmProcessTraversal((Widget) tb, XmTRAVERSE_CURRENT);
  TB_FixTearoff(tb);
}

/************************************************************************
 *
 *  GetUnselectGC
 *	Get the graphics context to be used to fill the interior of
 *	a square or diamond when unselected.
 *
 ************************************************************************/
static void
GetUnselectGC(
        XmToggleButtonWidget tw )
{
  XGCValues values;
  XtGCMask  valueMask;

  valueMask = GCForeground | GCBackground | GCFillStyle | GCGraphicsExposures;
  values.foreground = tw->toggle.unselect_color;
  values.background = tw->core.background_pixel;
  values.fill_style = FillSolid;
  values.graphics_exposures = FALSE;

  tw->toggle.unselect_GC = XtGetGC((Widget) tw, valueMask, &values);
}

/************************************************************************
 *
 *  GetGC
 *	Get the graphics context to be used to fill the interior of
 *	a square or diamond when selected, and the arm GC used in a
 *      menu if enableEtchedInMenu is True.
 *
 ************************************************************************/

static void
GetGC(
        XmToggleButtonWidget tw )
{
  XGCValues values;
  XtGCMask  valueMask;
  XFontStruct *fs = (XFontStruct *) NULL;
  Pixel sel_color, select_pixel;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(tw));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  /* Differentiate background and select colors on monochrome displays or
  ** others on which we have indistinguishable colors. */
  if ((DefaultDepthOfScreen(XtScreen(tw)) == 1) ||
      (tw->core.background_pixel == tw->toggle.select_color))
    sel_color = tw->primitive.foreground;
  else
    sel_color = tw->toggle.select_color;

  valueMask = 0;
  valueMask |= GCForeground, values.foreground = sel_color;
  valueMask |= GCBackground, values.background = tw->core.background_pixel;
  valueMask |= GCFillStyle, values.fill_style = FillSolid;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;

  tw->toggle.select_GC = XtAllocateGC((Widget)tw, 0, valueMask, &values, 0, 0);

  /* When foreground and select colors coincide, this GC is used
   * by XmLabel to draw the text.  It requires a font to pacify
   * the XmString draw functions.
   */
  valueMask = 0;

  if (XmeRenderTableGetDefaultFont(tw->label.font, &fs))
    valueMask |= GCFont, values.font = fs->fid;

  valueMask |= GCForeground, values.foreground = tw->core.background_pixel;
  valueMask |= GCBackground, values.background = tw->primitive.foreground;
  valueMask |= GCFillStyle, values.fill_style = FillSolid;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;
  valueMask |= GCLineWidth, values.line_width = 1;

  tw->toggle.background_gc = XtGetGC((Widget) tw, valueMask, &values);

  valueMask = 0;
  valueMask |= GCFillStyle, values.fill_style = FillOpaqueStippled;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;
  valueMask |= GCStipple, values.stipple =
    XmGetPixmapByDepth(XtScreen((Widget)(tw)), XmS50_foreground, 1, 0, 1);
  valueMask |= GCLineWidth, values.line_width = 1;

  tw->toggle.indeterminate_GC = XtAllocateGC((Widget)tw, 0, valueMask, &values,
					     GCForeground | GCBackground, 0);

  /* The valueMask and values are inherited from above. */
  valueMask &= ~GCLineWidth;
  valueMask |= GCForeground, values.foreground = tw->core.background_pixel;
  valueMask |= GCBackground, values.background = tw->primitive.foreground;

  tw->toggle.indeterminate_box_GC = XtGetGC((Widget) tw, valueMask, &values);

  /* Create the ArmGC for filling in background if we are in a menu
     and enableEtchedInMenu is True. */
  if ((Lab_IsMenupane(tw)) && etched_in) {
      XmGetColors(XtScreen(tw), tw->core.colormap, tw->core.background_pixel,
		  NULL, NULL, NULL, &select_pixel);

      valueMask = 0;
      valueMask |= GCForeground, values.foreground = select_pixel;
      valueMask |= GCBackground, values.background = tw->primitive.foreground;
      if (fs != NULL)
	  valueMask |= GCFont, values.font = fs->fid;
      valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;

      tw->toggle.arm_GC = XtGetGC((Widget) tw, valueMask, &values);
  }
}



/*************************************<->*************************************
 *
 *  Initialize
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
  XmToggleButtonWidget request = (XmToggleButtonWidget) rw ;
  XmToggleButtonWidget new_w = (XmToggleButtonWidget) nw ;
  XtWidgetProc resize;


  new_w->toggle.Armed = FALSE;

  /* if menuProcs is not set up yet, try again */
  _XmProcessLock();
  resize = xmLabelClassRec.core_class.resize;
  if (xmLabelClassRec.label_class.menuProcs == (XmMenuProc)NULL)
    xmLabelClassRec.label_class.menuProcs = (XmMenuProc)_XmGetMenuProcContext();
  _XmProcessUnlock();

  if (Lab_IsMenupane(new_w))
    {
	/* If the shadow thickness hasn't been set yet, inherit it
	   from the menu parent, instead of a hard-coded 2, as before.
	   The only draw back is that is the parent has also 0, then
	   the toggle shadow is 0, which is not very good in a menu,
	   but at least consistent with the other buttons */
      if (new_w->primitive.shadow_thickness <= 0) {
	  Dimension  parent_st ;

	  XtVaGetValues(XtParent(nw), XmNshadowThickness, &parent_st, NULL);
	  new_w->primitive.shadow_thickness = parent_st;
      }

      if (new_w->toggle.visible == XmINVALID_BOOLEAN)
	new_w->toggle.visible = FALSE;

      new_w->primitive.traversal_on = TRUE;
    }
  else if (new_w->toggle.visible == XmINVALID_BOOLEAN)
    new_w->toggle.visible = TRUE;


  /*
   * If the indicatorType has not been set, then
   * find out if radio behavior is set for RowColumn parents and
   * then set indicatorType.  If radio behavior is true, default to
   * one of many, else default to n of many.
   */
  if ((new_w->toggle.ind_type == XmINVALID_TYPE) ||
     !XmRepTypeValidValue (XmRID_INDICATOR_TYPE,
			   new_w->toggle.ind_type, (Widget) new_w))
    {
      Boolean radio = FALSE;

      if (XmIsRowColumn(XtParent(new_w)))
	{
	  XtVaGetValues(XtParent(new_w),
			XmNradioBehavior, &radio,
			NULL);
	}

      if (radio)
	new_w->toggle.ind_type = XmONE_OF_MANY;
      else
	new_w->toggle.ind_type = XmN_OF_MANY;
    }

  /*
   * This resource defaults to true if an indicator box is drawn.
   */
  if (new_w->toggle.fill_on_select == XmINVALID_BOOLEAN)
    {
      if (DRAWBOX(NormalizeIndOn(new_w)))
	new_w->toggle.fill_on_select = True;
      else if (IsOneOfMany(new_w->toggle.ind_type) &&
	       new_w->toggle.ind_on)
	new_w->toggle.fill_on_select = True;
      else
	new_w->toggle.fill_on_select = False;
    }

  /* Tristate buttons ain't allowed in one-of-many land. */
  if (IsOneOfMany(new_w->toggle.ind_type))
    new_w->toggle.toggle_mode = XmTOGGLE_BOOLEAN;


  /* If necessary use the On pixmaps in place of the Indeterminate ones. */
  if (IsNull(PixmapInd(new_w)) && !IsNull(PixmapOn(new_w)))
    PixmapInd(new_w) = PixmapOn(new_w);
  if (IsNull(PixmapInsenInd(new_w)) && !IsNull(PixmapInsenOn(new_w)))
    PixmapInsenInd(new_w) = PixmapInsenOn(new_w);

  /* If necessary use PixmapOn in place of PixmapOff. */
  if (IsNull(PixmapOff(new_w)) && !IsNull(PixmapOn(new_w)))
    {
      PixmapOff(new_w) = PixmapOn(new_w);
      if (request->core.width == 0)
	new_w->core.width = 0;
      if (request->core.height == 0)
	new_w->core.height = 0;

      _XmCalcLabelDimensions((Widget) new_w);
      (* resize)( (Widget) new_w);
    }

  /* If necessary use PixmapInsenOn in place of PixmapInsenOff. */
  if (IsNull(PixmapInsenOff(new_w)) && !IsNull(PixmapInsenOn(new_w)))
    {
      PixmapInsenOff(new_w) = PixmapInsenOn(new_w);
      if (request->core.width == 0)
	new_w->core.width = 0;
      if (request->core.height == 0)
	new_w->core.height = 0;

      _XmCalcLabelDimensions((Widget) new_w);
      (* resize)( (Widget) new_w);
    }


  /* BEGIN OSF Fix pir 1778 */
  if (Lab_IsPixmap(new_w) &&
      (!IsNull(PixmapOff(new_w)) || !IsNull(PixmapInsenOff(new_w)) ||
       !IsNull(PixmapOn(new_w)) || !IsNull(PixmapInsenOn(new_w)) ||
       !IsNull(PixmapInd(new_w)) || !IsNull(PixmapInsenInd(new_w))))
    {
      if (request->core.width == 0)
	new_w->core.width = 0;
      if (request->core.height == 0)
	new_w->core.height = 0;
      SetToggleSize(new_w);
    }
  /* END OSF Fix pir 1778 */

  if (new_w->toggle.indicator_dim == XmINVALID_DIMENSION)  {
    if (new_w->toggle.ind_on)
      {
	/* Determine how high the toggle indicator should be. */
	if (Lab_IsPixmap(new_w) || Lab_IsPixmapAndText(new_w))
	  {
	    /* Set indicator size proportional to size of pixmap. */
            if ( !IsNull(PixmapOn(new_w)) || !IsNull(PixmapInsenOn(new_w)) ||
		!IsNull(PixmapOff(new_w)) || !IsNull(PixmapInsenOff(new_w)) ||
		!IsNull(PixmapInd(new_w)) || !IsNull(PixmapInsenInd(new_w)))
	       if (new_w->label.TextRect.height < 13)
	         new_w->toggle.indicator_dim = new_w->label.TextRect.height;
	       else
	         new_w->toggle.indicator_dim =
 		   13 + (new_w->label.TextRect.height/13);
	    else
	      new_w->toggle.indicator_dim = XmDEFAULT_INDICATOR_DIM;
	  }
	else
	  {
	    /* Set indicator size proportional to size of font. */
	    Dimension height;
	    int line_count;

	    height = XmStringHeight (new_w->label.font, new_w->label._label);
	    if ((line_count = XmStringLineCount (new_w->label._label)) < 1)
	      line_count = 1;

	    /* Shiz recommends toggles in menus have smaller indicators */
	    if (Lab_IsMenupane(new_w))
	      new_w->toggle.indicator_dim =
		MAX(XmDEFAULT_INDICATOR_DIM,
		    (height / ((Dimension)line_count))*2/3);
	    else
	      new_w->toggle.indicator_dim =
		MAX(XmDEFAULT_INDICATOR_DIM, height / ((Dimension)line_count));
	  }
      } else {
	new_w->toggle.indicator_dim = 0;
      }

    new_w->toggle.indicator_set = FALSE;
  } else {
    new_w->toggle.indicator_set = TRUE;
  }

  /* CR 2337: Maintain original margin values. */
  new_w->toggle.ind_left_delta = 0;
  new_w->toggle.ind_right_delta = 0;
  new_w->toggle.ind_top_delta = 0;
  new_w->toggle.ind_bottom_delta = 0;

  if (new_w->toggle.ind_on)
    {
      /*
       *   Enlarge the text rectangle if needed to accomodate the size of
       *   indicator button. Adjust the dimensions of superclass Label-Gadget
       *   so that the toggle-button may be accommodated in it.
       */
      int maxIndicatorSize;
      int delta;
      int boxSize;

      /* BEGIN OSF Fix pir 2480 */
      if (! Lab_IsMenupane(new_w))
	maxIndicatorSize = new_w->toggle.indicator_dim + 2*Xm3D_ENHANCE_PIXEL;
      else
	maxIndicatorSize = new_w->toggle.indicator_dim;
      /* END OSF Fix pir 2480 */

      boxSize = ((int) new_w->label.TextRect.height  +
		 (int) new_w->label.margin_top +
		 (int) new_w->label.margin_bottom);

      if (maxIndicatorSize > boxSize)
	{
	  delta = (maxIndicatorSize - boxSize) / 2;
	  new_w->toggle.ind_top_delta = delta;
	  new_w->label.margin_top += new_w->toggle.ind_top_delta;
	  new_w->toggle.ind_bottom_delta = delta;
	  new_w->label.margin_bottom += new_w->toggle.ind_bottom_delta;
	}

      /* CR 2337: Make room for toggle indicator and spacing */
      if (LayoutIsRtoLP(new_w))
	{
	  delta = (new_w->toggle.indicator_dim + new_w->toggle.spacing -
		   new_w->label.margin_right);
	  if (delta > 0)
	    {
	      new_w->toggle.ind_right_delta = delta;
	      new_w->label.margin_right += delta;
	    }
	}
      else
	{
	  delta = (new_w->toggle.indicator_dim + new_w->toggle.spacing -
		   new_w->label.margin_left);
	  if (delta > 0)
	    {
	      new_w->toggle.ind_left_delta = delta;
	      new_w->label.margin_left += delta;
	    }
	}
    }

  if (request->core.width == 0)
    {
      new_w->core.width = new_w->label.TextRect.width +
	2 * new_w->label.margin_width + new_w->label.margin_right +
	    new_w->label.margin_left +
	      2 * (new_w->primitive.highlight_thickness +
		   new_w->primitive.shadow_thickness);

      if (new_w->core.width == 0)
	new_w->core.width = 1;

      if ((new_w->label._acc_text != NULL) && (new_w->toggle.ind_on))
      {
	if (LayoutIsRtoLP(new_w))
        {
	  new_w->label.acc_TextRect.x = new_w->primitive.highlight_thickness +
	    new_w->primitive.shadow_thickness + new_w->label.margin_width;
      }
	else
        {
	  new_w->label.acc_TextRect.x = new_w->core.width -
	    new_w->primitive.highlight_thickness -
	      new_w->primitive.shadow_thickness - new_w->label.margin_width -
		  new_w->label.margin_right + LABEL_ACC_PAD;
      }
    }
    }

  if (request->core.height == 0)
    new_w->core.height =
      MAX(new_w->toggle.indicator_dim,
	  new_w->label.TextRect.height + 2 * new_w->label.margin_height +
	  new_w->label.margin_top + new_w->label.margin_bottom)  +
	    2 * (new_w->primitive.highlight_thickness +
		 new_w->primitive.shadow_thickness);

  new_w->label.TextRect.y =  (short) new_w->primitive.highlight_thickness
    + new_w->primitive.shadow_thickness + new_w->label.margin_height +
      new_w->label.margin_top +
	((new_w->core.height - new_w->label.margin_top
	  - new_w->label.margin_bottom
	  - (2 * (new_w->label.margin_height
		  + new_w->primitive.highlight_thickness
		  + new_w->primitive.shadow_thickness))
	  - new_w->label.TextRect.height) / 2);

  if (new_w->core.height == 0)
    new_w->core.height = 1;

  new_w->toggle.visual_set = new_w->toggle.set;

  /* Display as set if XmNset is TRUE when the toggle first comes up. */
  if (XmSET == new_w->toggle.set)
    IsOn(new_w) = XmSET;
  else
    IsOn(new_w) = XmUNSET;

  {
       _XmProcessLock();
       resize = new_w->core.widget_class->core_class.resize;
       _XmProcessUnlock();

      (* (resize)) ((Widget) new_w);
  }

  /* unselect same as background unless set. */
  if (new_w->toggle.unselect_color == XmINVALID_PIXEL)
    new_w->toggle.unselect_color = new_w->core.background_pixel;

  /* Deal with selectColor */
  new_w->toggle.reversed_select =
    (new_w->toggle.select_color == XmREVERSED_GROUND_COLORS);
  if (new_w->toggle.select_color == XmDEFAULT_SELECT_COLOR)
    {
      /* CR 9923: Copy all bytes of the resulting pixel. */
      XrmValue value;
      _XmSelectColorDefault((Widget)new_w,
			    XtOffsetOf(XmToggleButtonRec,
				       toggle.select_color),
			    &value);
      assert(value.size == sizeof(Pixel));
      new_w->toggle.select_color = *((Pixel*) value.addr);
    }
  else if (new_w->toggle.select_color == XmREVERSED_GROUND_COLORS)
    {
      new_w->toggle.select_color = new_w->primitive.foreground;
    }
  else if (new_w->toggle.select_color == XmHIGHLIGHT_COLOR)
    {
      new_w->toggle.select_color = new_w->primitive.highlight_color;
    }

  GetGC (new_w);
  GetUnselectGC(new_w);
}

/************************************************************************
 *
 *  Destroy
 *	Free toggleButton's graphic context.
 *
 ************************************************************************/
static void
Destroy(
        Widget wid )
{
  XmToggleButtonWidget tw = (XmToggleButtonWidget) wid ;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  XtReleaseGC ((Widget) tw, tw->toggle.select_GC);
  XtReleaseGC ((Widget) tw, tw->toggle.background_gc);
  XtReleaseGC ((Widget) tw, tw->toggle.unselect_GC);
  XtReleaseGC ((Widget) tw, tw->toggle.indeterminate_GC);
  XtReleaseGC ((Widget) tw, tw->toggle.indeterminate_box_GC);
  if (Lab_IsMenupane(tw) && etched_in)
      XtReleaseGC ((Widget) tw, tw->toggle.arm_GC);
}

static void
DrawBox(XmToggleButtonWidget w,
	GC top_gc, GC bot_gc, GC fillgc, int x, int y, int edge,
	Dimension margin)
{
  int shadow = w->toggle.detail_shadow_thickness;

  XmeDrawShadows(XtDisplay ((Widget) w),
		 XtWindow ((Widget) w),
		 top_gc,
		 bot_gc,
		 x, y, edge, edge,
		 shadow, XmSHADOW_OUT);

  /* Don't fill the background on mono screens if we're going to */
  /* draw a glyph */

  if ((DefaultDepthOfScreen(XtScreen(w)) == 1) && DRAWGLYPH(NormalizeIndOn(w)))
    return;

  shadow += margin;

  if (edge > (shadow * 2))
    XFillRectangle (XtDisplay ((Widget) w),
		    XtWindow ((Widget) w),
		    fillgc,
		    x + shadow,
		    y + shadow,
		    edge - (shadow * 2),
		    edge - (shadow * 2));
}

/*************************************<->*************************************
 *
 *  DrawToggle(w)
 *     Depending on the state of this widget, draw the Toggle.
 *     That is draw the True/False indicator next to the label.
 *
 *************************************<->***********************************/
static void
DrawToggle
        (XmToggleButtonWidget w )
{
  int x, y, edge;
  Dimension margin;
  XGCValues values;
  Display *dpy = XtDisplay((Widget) w);
  Drawable drawable = XtWindow((Widget) w);
  XmDisplay dpyxm = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean etched_in = dpyxm->display.enable_etched_in_menu;

  /* Get size of indicator i.e. bounding box */
  if (Lab_IsPixmap(w) || XmStringEmpty(w->label._label))
    edge = w->toggle.indicator_dim;
  else
    edge = MIN((int)w->toggle.indicator_dim,
	       MAX(0,
		   ((int)w->core.height - 2*(w->primitive.highlight_thickness +
					     w->primitive.shadow_thickness +
					     (int)w->label.margin_height) +
		    w->label.margin_top + w->label.margin_bottom)));

  /* Touch up the appearance of filled background. */
  if ((DefaultDepthOfScreen(XtScreen(w)) > 1) &&
      (w->primitive.top_shadow_color != w->toggle.select_color) &&
      (w->primitive.bottom_shadow_color != w->toggle.select_color))
    margin = 0;
  else
    margin = 1;

  if (LayoutIsRtoLP(w))
    x = (int)w->core.width - w->primitive.highlight_thickness -
      w->primitive.shadow_thickness - w->label.margin_width -
	w->toggle.indicator_dim;
  else
    x = w->primitive.highlight_thickness + w->primitive.shadow_thickness +
      w->label.margin_width;

  if (Lab_IsPixmap(w)  || XmStringEmpty(w->label._label))
    y = (int)((w->core.height - w->toggle.indicator_dim))/2;
  else
    {
      /* Center indicator vertically next to the first line of text. */
      int fudge = Xm3D_ENHANCE_PIXEL;
      Dimension text_height;
      int line_count, height_diff;

      text_height = XmStringHeight(w->label.font, w->label._label);
      if ((line_count = XmStringLineCount(w->label._label)) < 1)
	line_count = 1;

      height_diff = ((int)text_height / line_count) -
                    (int)w->toggle.indicator_dim;
      if (height_diff < 0)
	height_diff = 0;

      y = w->label.TextRect.y + (height_diff / 2);

      /* CR 2337: Keep large indicators inside the toggle. */
      /*	Is this definition of fudge right??? */
      if (w->toggle.ind_top_delta > fudge)
	y -= (w->toggle.ind_top_delta - fudge);
    }

  if (w->toggle.visible || (w->toggle.visual_set != XmUNSET))
    {
      /* The toggle indicator should be visible. */
      GC top_gc, bot_gc, fill_gc, glyph_gc;
      unsigned char normal_ind_on = NormalizeIndOn(w);

      switch (w->toggle.visual_set)
	{
	case XmUNSET:
	  top_gc = w->primitive.top_shadow_GC;
	  bot_gc = w->primitive.bottom_shadow_GC;
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (Lab_IsMenupane(w) && etched_in && w->toggle.Armed)
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.unselect_GC : w->toggle.arm_GC);
	  else
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.unselect_GC : w->toggle.background_gc);
	  glyph_gc = None;
	  break;

	case XmSET:
	  top_gc = w->primitive.bottom_shadow_GC;
	  bot_gc = w->primitive.top_shadow_GC;
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (Lab_IsMenupane(w) && etched_in && w->toggle.Armed)
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.select_GC : w->toggle.arm_GC);
	  else
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.select_GC : w->toggle.background_gc);
	  glyph_gc = ((w->toggle.reversed_select && DRAWBOX(normal_ind_on)) ?
		      w->toggle.background_gc : w->label.normal_GC);

	  /* CR 9791: Label's normal_gc has a dynamic clip_mask. */
	  if (glyph_gc == w->label.normal_GC)
	    XSetClipMask(dpy, glyph_gc, None);
	  break;

	case XmINDETERMINATE:
	  top_gc = bot_gc = w->toggle.indeterminate_box_GC;
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (Lab_IsMenupane(w) && etched_in && w->toggle.Armed)
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.indeterminate_GC : w->toggle.arm_GC);
	  else
	      fill_gc = (w->toggle.fill_on_select ?
			 w->toggle.indeterminate_GC : w->toggle.background_gc);
	  glyph_gc = w->toggle.indeterminate_GC;
	  break;

	default:
	  assert(False);
	  return;
	}

      switch (NormalizeIndType(w))
	{
	case XmN_OF_MANY:
	  {
	    /* If the toggle indicator is square shaped then adjust the
	     * indicator width and height, so that it looks proportional
	     * to a diamond shaped indicator of the same width and height
	     */
	    int new_edge;
	    Dimension box_margin = (DRAWBOX(normal_ind_on) ?
				    w->toggle.detail_shadow_thickness : 0);

	    /* Subtract 3 pixels + 10% from the width and height. */
	    new_edge = edge - 3 - ((edge - 10)/10);

	    /* Adjust x,y to center the indicator relative to the label */
	    y = y + ((edge - new_edge) / 2);
	    x = x + ((edge - new_edge) / 2);
	    edge = new_edge;

	    switch(w->toggle.visual_set)
	      {
	      case XmUNSET:
		if (DRAW3DBOX(normal_ind_on))
		  DrawBox(w, top_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (DRAWFLATBOX(normal_ind_on))
		  DrawBox(w, bot_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (edge > 0)
		  XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);
		break;

	      case XmSET:
		if (DRAW3DBOX(normal_ind_on))
		  DrawBox(w, top_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (DRAWFLATBOX(normal_ind_on))
		  DrawBox(w, top_gc, top_gc, fill_gc, x, y, edge, margin);
		else if (edge > 0)
		  XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);

		if (!DRAWBOX(normal_ind_on) ||
		    ((edge - 2 * box_margin) >= MIN_GLYPH_SIZE))
		  {
		    if (DRAWCHECK(normal_ind_on))
		      XmeDrawIndicator(dpy, drawable, glyph_gc, x, y,
				       edge, edge, box_margin,
				       normal_ind_on);
		    else if (DRAWCROSS(normal_ind_on))
		      XmeDrawIndicator(dpy, drawable, glyph_gc, x, y,
				       edge, edge, box_margin,
				       normal_ind_on);
		  }
		break;

	      case XmINDETERMINATE:
		if (w->toggle.fill_on_select)
		  {
		    /* Fetch the select_color GetGC() actually used. */
		    XGetGCValues(dpy, w->toggle.select_GC,
				 GCForeground, &values);
		    values.background = values.foreground;
		    values.foreground = w->toggle.unselect_color;
		    XChangeGC(dpy, fill_gc,
			      GCForeground|GCBackground, &values);
		  }
		else if (DRAWBOX(normal_ind_on))
		  {
		    /* This GC should have the right values already. */
		    fill_gc = w->toggle.indeterminate_box_GC;
		  }

		if (DRAWBOX(normal_ind_on))
		  DrawBox(w, bot_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (edge > 0)
		  XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);

		if (w->toggle.reversed_select)
		  {
		    values.foreground = w->core.background_pixel;
		    values.background = w->primitive.foreground;
		  }
		else
		  {
		    values.foreground = w->primitive.foreground;
		    values.background = w->core.background_pixel;
		  }

		if (!DRAWBOX(normal_ind_on) ||
		    ((edge - 2 * box_margin) >= MIN_GLYPH_SIZE))
		  {
		    if (DRAWCHECK(normal_ind_on))
		      {
			XChangeGC(dpy, glyph_gc,
				  GCForeground|GCBackground, &values);
			XmeDrawIndicator(dpy, drawable, glyph_gc, x, y,
					 edge, edge, box_margin,
					 normal_ind_on);
		      }
		    else if (DRAWCROSS(normal_ind_on))
		      {
			XChangeGC(dpy, glyph_gc,
				  GCForeground|GCBackground, &values);
			XmeDrawIndicator(dpy, drawable, glyph_gc, x, y,
					 edge, edge, box_margin,
					 normal_ind_on);
		      }
		  }
		break;
	      }
	    break;
	  }

	case XmONE_OF_MANY:
	  /* This value should have been normalized away! */
	  assert(FALSE);

	case XmONE_OF_MANY_DIAMOND:
	  XmeDrawDiamond(dpy, drawable, top_gc, bot_gc, fill_gc, x, y,
			 edge,edge, w->toggle.detail_shadow_thickness, margin);
	  break;

	case XmONE_OF_MANY_ROUND:
#ifdef FIX_1402
	  XmeDrawCircle(dpy, drawable, top_gc, bot_gc, fill_gc, x, y,
			edge, edge, w->toggle.detail_shadow_thickness, margin);
#else
	  XmeDrawCircle(dpy, drawable, top_gc, bot_gc, fill_gc, x, y,
			edge, edge, w->toggle.detail_shadow_thickness, 1);
#endif
	  break;
	}
    }
  else
    {
      /* The toggle indicator should be invisible. */
      if (edge > 0)
	{
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (Lab_IsMenupane(w) && etched_in && w->toggle.Armed)
	      XFillRectangle(dpy, drawable, w->toggle.arm_GC,
			     x, y, edge + 4, edge + 2);
	  else
	      XFillRectangle(dpy, drawable, w->toggle.background_gc,
			     x, y, edge + 4, edge + 2);
      }
    }
}

/*************************************<->*************************************
 *
 *  BorderHighlight
 *
 *************************************<->***********************************/
static void
BorderHighlight(
        Widget wid )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XEvent * event = NULL;

  if (Lab_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;
      Boolean already_armed =  tb->toggle.Armed;

      tb->toggle.Armed = True;

      if ((etched_in) &&
	  ((tb->toggle.ind_on) ||
	   (!(tb->toggle.ind_on) && !(tb->toggle.fill_on_select))))
        {
	    DrawEtchedInMenu(tb);
	    if (tb->toggle.ind_on)
		DrawToggle(tb);
        }

      XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
		      tb->primitive.top_shadow_GC,
		      tb->primitive.bottom_shadow_GC,
		      tb->primitive.highlight_thickness,
		      tb->primitive.highlight_thickness,
		      tb->core.width - 2 * tb->primitive.highlight_thickness,
		      tb->core.height - 2 * tb->primitive.highlight_thickness,
		      tb->primitive.shadow_thickness,
		      etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

      if (!already_armed &&  tb->toggle.arm_CB)
	{
	  XFlush (XtDisplay (tb));
	  ToggleButtonCallback(tb, XmCR_ARM, tb->toggle.set, event);
	}
    }
  else
    {
      (*(xmLabelClassRec.primitive_class.border_highlight))((Widget) tb) ;
    }
}

/*************************************<->*************************************
 *
 *  BorderUnhighlight
 *
 *************************************<->***********************************/
static void
BorderUnhighlight(
        Widget wid )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XEvent * event = NULL;

  if (Lab_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;
      Boolean already_armed = tb->toggle.Armed;

      tb -> toggle.Armed = FALSE;

      if ((etched_in) &&
	      ((tb->toggle.ind_on) ||
	       (!(tb->toggle.ind_on) && !(tb->toggle.fill_on_select))))
        {
	  DrawEtchedInMenu(tb);
	  if (tb->toggle.ind_on)
	      DrawToggle(tb);
	}

      XmeClearBorder (XtDisplay (tb), XtWindow (tb),
		      tb->primitive.highlight_thickness,
		      tb->primitive.highlight_thickness,
		      tb->core.width - 2 * tb->primitive.highlight_thickness,
		      tb->core.height - 2 * tb->primitive.highlight_thickness,
		      tb->primitive.shadow_thickness);

      if (tb->toggle.Armed && tb->toggle.disarm_CB)
	{
	  XFlush (XtDisplay (tb));
	  ToggleButtonCallback(tb, XmCR_DISARM, tb->toggle.set, event);
	}
    }
  else
    {
      (*(xmLabelClassRec.primitive_class.border_unhighlight))((Widget) tb) ;
    }
}

/*  spb This action does not seem to be used anywhere */
/* Tue Apr 27 17:31:48 1993 */
/*************************************<->*************************************
 *
 *  KeySelect
 *    If the menu system traversal is enabled, do an activate and disarm
 *
 *************************************<->***********************************/
/* ARGSUSED */
static void
KeySelect(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) wid ;
  XmToggleButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

  if (menuSTrait == NULL) return;

  if (!_XmIsEventUnique(event))
    return;

  if (!_XmGetInDragMode((Widget)tb))
    {
      if (tb->toggle.ind_on)
	DrawToggle(tb);
      else if (tb->toggle.fill_on_select && !Lab_IsPixmap(tb))
	DrawToggleLabel(tb);

      if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
	SetAndDisplayPixmap( tb, event, NULL);

      tb->toggle.Armed = FALSE;
      tb->toggle.set = (tb->toggle.set == TRUE) ? FALSE : TRUE;

      if (menuSTrait != NULL)
	menuSTrait->buttonPopdown(XtParent(tb), event);

      _XmRecordEvent(event);

      /* UNDOING this fix ... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /* 	reported correctly even if the entry callback resets it. */

      /* If the parent is menu system able, notify it about the select. */
      if (menuSTrait != NULL)
	{
	  call_value.reason = XmCR_VALUE_CHANGED;
	  call_value.event = event;
	  call_value.set = tb->toggle.set;
	  menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
	}

      if (menuSTrait != NULL)
	menuSTrait->reparentToTearOffShell(XtParent(tb), event);

      if ((! tb->label.skipCallback) &&
	  (tb->toggle.value_changed_CB))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, tb->toggle.set, event);
	}
    }
}

/************************************************************************
 *
 * Compute Space
 *
 ***********************************************************************/
static void
ComputeSpace(
        XmToggleButtonWidget tb )
{
  int needed_width;
  int needed_height;

  /* Compute space for drawing toggle. */

  needed_width = tb->label.TextRect.width +
    tb->label.margin_left + tb->label.margin_right +
      (2 * (tb->primitive.shadow_thickness +
	    tb->primitive.highlight_thickness +
	    tb->label.margin_width));

  needed_height = tb->label.TextRect.height +
    tb->label.margin_top + tb->label.margin_bottom +
      (2 * (tb->primitive.shadow_thickness +
	    tb->primitive.highlight_thickness +
	    tb->label.margin_height));

  if (needed_height > tb->core.height)
    if (tb->toggle.ind_on)
      tb->label.TextRect.y = tb->primitive.shadow_thickness +
	tb->primitive.highlight_thickness +
	  tb->label.margin_height +
	    tb->label.margin_top +
	      ((tb->core.height - tb->label.margin_top
		- tb->label.margin_bottom
		- (2 * (tb->label.margin_height
			+ tb->primitive.highlight_thickness
			+ tb->primitive.shadow_thickness))
		- tb->label.TextRect.height) / 2);

  if (LayoutIsRtoLP(tb))
    {
      if ((needed_width > tb->core.width) ||
	  ((tb->label.alignment == XmALIGNMENT_BEGINNING)
	   && (needed_width < tb->core.width)) ||
	  ((tb->label.alignment == XmALIGNMENT_CENTER)
	   && (needed_width < tb->core.width)
	   && (tb->core.width - needed_width < tb->label.margin_right)) ||
	  (needed_width == tb->core.width))
	{
	  if (tb->toggle.ind_on)
	    tb->label.TextRect.x = tb->core.width -
	      (tb->primitive.shadow_thickness +
	       tb->primitive.highlight_thickness +
	       tb->label.margin_width +
	       tb->label.margin_right +
	       tb->label.TextRect.width);
	}
    }
  else
    {
      if ((needed_width > tb->core.width) ||
	  ((tb->label.alignment == XmALIGNMENT_BEGINNING)
	   && (needed_width < tb->core.width)) ||
	  ((tb->label.alignment == XmALIGNMENT_CENTER)
	   && (needed_width < tb->core.width)
	   && (tb->core.width - needed_width < tb->label.margin_left)) ||
	  (needed_width == tb->core.width))
	{
	  if (tb->toggle.ind_on)
	    tb->label.TextRect.x = tb->primitive.shadow_thickness +
	      tb->primitive.highlight_thickness +
		tb->label.margin_width +
		  tb->label.margin_left;
	}
    }
}

/*************************************<->*************************************
 *
 *  Redisplay (w, event, region)
 *     Cause the widget, identified by w, to be redisplayed.
 *
 *************************************<->***********************************/
/*ARGUSED*/
static void
Redisplay(
        Widget w,
        XEvent *event,
        Region region )
{
  register XmToggleButtonWidget tb = (XmToggleButtonWidget) w;

  if (! XtIsRealized(w) ) return;    /* Fix CR #4884, D. Rand 6/4/92 */

  ComputeSpace (tb);

  if (Lab_IsPixmap(tb) || Lab_IsPixmapAndText(tb))
    SetAndDisplayPixmap(tb, event, region);
  else
    if (!tb->toggle.ind_on && tb->toggle.fill_on_select)
    DrawToggleLabel (tb);
  else
  {
    XtExposeProc expose;
    _XmProcessLock();
    expose = xmLabelClassRec.core_class.expose;
    _XmProcessUnlock();
    (* expose) (w, event, region);
  }

  if (tb->toggle.ind_on)
    {
      if (!(tb->toggle.Armed))
	IsOn(tb) = tb->toggle.set;
      DrawToggle(tb);
    }

  if (Lab_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
      Boolean etched_in = dpy -> display.enable_etched_in_menu;

      if ((tb->toggle.Armed) &&
	  (tb->primitive.shadow_thickness > 0))
	XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
			tb->primitive.top_shadow_GC,
			tb->primitive.bottom_shadow_GC,
			tb ->primitive.highlight_thickness,
			tb ->primitive.highlight_thickness,
			(int)tb->core.width-2*tb->primitive.highlight_thickness,
			(int)tb->core.height-2*tb->primitive.highlight_thickness,
			tb->primitive.shadow_thickness,
			etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
    }

  else
    {
      DrawToggleShadow (tb);
    }
}

/**************************************************************************
 *
 * Resize(w, event)
 *
 **************************************************************************/
static void
Resize(
        Widget w )
{
  register XmToggleButtonWidget tb = (XmToggleButtonWidget) w;

  /* BEGIN OSF Fix pir 1778 */
  if (Lab_IsPixmap(w))
    SetToggleSize(tb);
  else {
    XtWidgetProc resize;
    _XmProcessLock();
    resize = xmLabelClassRec.core_class.resize;
    _XmProcessUnlock();
    (* resize)( (Widget) tb);
  }
  /* END OSF Fix pir 1778 */
}

/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
SetValuesPrehook(
        Widget cw,		/* unused */
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmToggleButtonWidget new_w = (XmToggleButtonWidget) nw ;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (new_w->label.font == NULL)
    new_w->label.font = XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST);

  return False;
}

/***************************************************************************
 *
 *  SetValues(current, request, new_w)
 *     This is the set values procedure for the ToggleButton class.  It is
 *     called last (the set values rtnes for its superclasses are called
 *     first).
 *
 *************************************<->***********************************/
/* ARGSUSED */
static Boolean
SetValues(
        Widget current,
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmToggleButtonWidget curcbox = (XmToggleButtonWidget) current;
  XmToggleButtonWidget reqcbox = (XmToggleButtonWidget) request;
  XmToggleButtonWidget newcbox = (XmToggleButtonWidget) new_w;
  Boolean  flag = FALSE;    /* our return value */
  XtWidgetProc resize;

  /* CR 2337: Preserve the user's margins. */
  if (curcbox->label.margin_right != reqcbox->label.margin_right)
    newcbox->toggle.ind_right_delta = 0;
  if (curcbox->label.margin_left != reqcbox->label.margin_left)
    newcbox->toggle.ind_left_delta = 0;
  if (curcbox->label.margin_top != reqcbox->label.margin_top)
    newcbox->toggle.ind_top_delta = 0;
  if (curcbox->label.margin_bottom != reqcbox->label.margin_bottom)
    newcbox->toggle.ind_bottom_delta = 0;


  /**********************************************************************
   * Calculate the window size:  The assumption here is that if
   * the width and height are the same in the new and current instance
   * record that those fields were not changed with set values.  Therefore
   * its okay to recompute the necessary width and height.  However, if
   * the new and current do have different width/heights then leave them
   * alone because that's what the user wants.
   *********************************************************************/

  _XmProcessLock();
  resize = xmLabelClassRec.core_class.resize;
  _XmProcessUnlock();

  /* Use the On pixmaps if no Indeterminate pixmaps are found. */
  if (IsNull(PixmapInd(newcbox)) && !IsNull(PixmapOn(newcbox)))
    PixmapInd(newcbox) = PixmapOn(newcbox);
  if (IsNull(PixmapInsenInd(newcbox)) && !IsNull(PixmapInsenOn(newcbox)))
    PixmapInsenInd(newcbox) = PixmapInsenOn(newcbox);

  /* Use the On pixmap if no Off pixmap is found. */
  if (IsNull(PixmapOff(newcbox)) && !IsNull(PixmapOn(newcbox)))
    {
      PixmapOff(newcbox) = PixmapOn(newcbox);
      if ((newcbox->label.recompute_size) &&
	  (request->core.width == current->core.width))
	new_w->core.width = 0;
      if ((newcbox->label.recompute_size) &&
	  (request->core.height == current->core.height))
	new_w->core.height = 0;

      _XmCalcLabelDimensions(new_w);
      (* resize)( (Widget) new_w);
    }

  /* Use the insensitive On pixmap if no insensitive Off pixmap is found. */
  if (IsNull(PixmapInsenOff(newcbox)) && !IsNull(PixmapInsenOn(newcbox)))
    {
      PixmapInsenOff(newcbox) = PixmapInsenOn(newcbox);
      if ((newcbox->label.recompute_size) &&
	  (request->core.width == current->core.width))
	new_w->core.width = 0;
      if ((newcbox->label.recompute_size) &&
	  (request->core.height == current->core.height))
	new_w->core.height = 0;

      _XmCalcLabelDimensions(new_w);
      (* resize)( (Widget) new_w);
    }

  /* BEGIN OSF Fix pir 1778 */
  /* Have to reset the TextRect width because label's resize will have
   * mucked with it. */
  if (Lab_IsPixmap(newcbox) &&
      (!IsNull(PixmapOff(newcbox)) || !IsNull(PixmapInsenOff(newcbox)) ||
       !IsNull(PixmapOn(newcbox)) || !IsNull(PixmapInsenOn(newcbox)) ||
       !IsNull(PixmapInd(newcbox)) || !IsNull(PixmapInsenInd(newcbox))))
    {
      if ((newcbox->label.recompute_size))
	{
	  if (request->core.width == current->core.width)
	    new_w->core.width = 0;
	  if (request->core.height == current->core.height)
	    new_w->core.height = 0;
	}

      SetToggleSize(newcbox);
    }
  /* END OSF Fix pir 1778 */

  /* CR 9922: Changing fillOnSelect requires a redraw. */
  if (newcbox->toggle.fill_on_select != curcbox->toggle.fill_on_select)
    {
      flag = TRUE;
    }

  if ((newcbox->label._label != curcbox->label._label) ||
      (PixmapOff(newcbox) != PixmapOff(curcbox)) ||
      (newcbox->label.font != curcbox->label.font) ||
      (newcbox->toggle.spacing != curcbox->toggle.spacing) ||
      (PixmapOn(newcbox) != PixmapOn(curcbox)) ||
      (PixmapInsenOn(newcbox) != PixmapInsenOn(curcbox)) ||
      (PixmapInd(newcbox) != PixmapInd(curcbox)) ||
      (PixmapInsenInd(newcbox) != PixmapInsenInd(curcbox)) ||
      (newcbox->toggle.ind_on != curcbox->toggle.ind_on) ||
      (newcbox->toggle.indicator_dim != curcbox->toggle.indicator_dim) ||
      (Lab_IsPixmap(newcbox) != Lab_IsPixmap(curcbox)) ||
      (Lab_IsPixmapAndText(newcbox) != Lab_IsPixmapAndText(curcbox)))
    {
      int right_delta = 0;	/* Our desired margin adjustments. */
      int left_delta = 0;
      int top_delta = 0;
      int bottom_delta = 0;

      if (newcbox->label.recompute_size)
	{
	  if (request->core.width == current->core.width)
            new_w->core.width = 0;
	  if (request->core.height == current->core.height)
            new_w->core.height = 0;
	}

      if (Lab_IsPixmap(newcbox) &&
	  ((PixmapOn(newcbox) != PixmapOn(curcbox)) ||
	  (PixmapInsenOn(newcbox) != PixmapInsenOn(curcbox)) ||
	  (PixmapInd(newcbox) != PixmapInd(curcbox)) ||
	  (PixmapInsenInd(newcbox) != PixmapInsenInd(curcbox))) )
	{
	  _XmCalcLabelDimensions(new_w);

	  /* OSF Fix pir 1778 */
	  SetToggleSize(newcbox);
	}

	newcbox->toggle.indicator_set = curcbox->toggle.indicator_set;

if (newcbox->toggle.indicator_dim != curcbox->toggle.indicator_dim)
	{
	newcbox->toggle.indicator_set = TRUE;
	}


      /* CR 8415: Honor explicit requests for XmNindicatorSize. */
      if (!(newcbox->toggle.indicator_set) &&
	  (newcbox->toggle.indicator_dim == curcbox->toggle.indicator_dim))
	{
	  if ((Lab_IsPixmap(newcbox) != Lab_IsPixmap(curcbox)) ||
	      (Lab_IsPixmapAndText(newcbox) != Lab_IsPixmapAndText(curcbox)) ||
	      (newcbox->label._label != curcbox->label._label) ||
	      (PixmapOff(newcbox) != PixmapOff(curcbox)) ||
	      (newcbox->label.font != curcbox->label.font) ||
	      (newcbox->toggle.ind_on != curcbox->toggle.ind_on))
	    {
	      if (Lab_IsPixmap(new_w) || Lab_IsPixmapAndText(new_w))
		{
                if (!IsNull(PixmapOff(newcbox)) || !IsNull(PixmapInsenOff(newcbox)) ||
                 !IsNull(PixmapOn(newcbox)) || !IsNull(PixmapInsenOn(newcbox)) ||
                 !IsNull(PixmapInd(newcbox)) || !IsNull(PixmapInsenInd(newcbox)))
		  if (newcbox->label.TextRect.height < 13)
		    newcbox->toggle.indicator_dim =
		      newcbox->label.TextRect.height;
		  else
		    newcbox->toggle.indicator_dim = 13 +
		      (newcbox->label.TextRect.height/13);
		else
		  newcbox->toggle.indicator_dim = XmDEFAULT_INDICATOR_DIM;
		}
	      else
		{
		  Dimension height;
		  int line_count;

		  height = XmStringHeight (newcbox->label.font,
					    newcbox->label._label);
		  line_count = XmStringLineCount (newcbox->label._label);

		  /*
		   * CR 5203 - Make the calculation for the
		   *     indicator_dim be the same as in the Initialize
		   *     procedure, i.e. Popup and Pulldown menus should
		   *     have smaller indicators.
		   */
		  if (line_count < 1)
		    line_count = 1;
		  if (Lab_IsMenupane(newcbox))
		    newcbox->toggle.indicator_dim =
		      MAX(XmDEFAULT_INDICATOR_DIM,
			  (height / ((Dimension)line_count)) * 2/3);
		  else
		    newcbox->toggle.indicator_dim =
		      MAX(XmDEFAULT_INDICATOR_DIM,
			  height / ((Dimension)line_count));
		  /* End 5203 Fix */
		}
	    }
	}

      if (newcbox->toggle.ind_on)
	{
	  /*
	   * Fix CR 5568 - If the indicator is on and the user has changed the
	   *             indicator dimension, calculate the new top and bottom
	   *             margins in a place where they can effect the core width
	   *             and height.
	   */
	  /*  Recompute the Top and bottom margins and the height of the text
	   *  rectangle to  accommodate the size of toggle indicator.
	   *  if (we are given a new toggleIndicator size)
	   *    { if (user has given new top or bottom margin)
	   *        { compute to accomodate new toggle button size; }
	   *      else (user has set new top/bottom margin)
	   *        { Recompute margin to accommodate new IndicatorSize; }
	   *    }
	   */
	  if (newcbox->toggle.indicator_dim != curcbox->toggle.indicator_dim)
	    {
	      int maxIndicatorSize =
		(int) (newcbox->toggle.indicator_dim) + 2*Xm3D_ENHANCE_PIXEL;

	      int boxSize = ((int) newcbox->label.TextRect.height +
			     (int) newcbox->label.margin_top +
			     (int) newcbox->label.margin_bottom);

	      top_delta = bottom_delta = (maxIndicatorSize - boxSize) / 2;
	    }
	  /*  End fix CR 5568 */

          if (LayoutIsRtoLP(newcbox))
	    right_delta = (newcbox->toggle.indicator_dim +
			   newcbox->toggle.spacing -
			   newcbox->label.margin_right);
          else
	    left_delta = (newcbox->toggle.indicator_dim +
			  newcbox->toggle.spacing -
			  newcbox->label.margin_left);
	}
      else if (curcbox->toggle.ind_on)
	{
	  /* CR 2337: Redisplay when the indicator is turned off. */
	  flag = TRUE;

	  top_delta = -newcbox->toggle.ind_top_delta;
	  bottom_delta = -newcbox->toggle.ind_bottom_delta;

          if (LayoutIsRtoLP(newcbox))
	    right_delta = -newcbox->toggle.ind_right_delta;
          else
	    left_delta = -newcbox->toggle.ind_left_delta;
	}

      /* CR 2337: Let the toggle button shrink if necessary. */
      if (right_delta || left_delta || top_delta || bottom_delta)
	{
	  flag = TRUE;

	  /* Adjust vertical margins based on the indicator. */
	  if ((int)newcbox->toggle.ind_top_delta + top_delta > 0)
	    {
	      newcbox->label.margin_top += top_delta;
	      newcbox->toggle.ind_top_delta += top_delta;
	    }
	  else
	    {
	      newcbox->label.margin_top -= newcbox->toggle.ind_top_delta;
	      newcbox->toggle.ind_top_delta = 0;
	    }

	  if ((int)newcbox->toggle.ind_bottom_delta + bottom_delta > 0)
	    {
	      newcbox->label.margin_bottom += bottom_delta;
	      newcbox->toggle.ind_bottom_delta += bottom_delta;
	    }
	  else
	    {
	      newcbox->label.margin_bottom -= newcbox->toggle.ind_bottom_delta;
	      newcbox->toggle.ind_bottom_delta = 0;
	    }

	  /* Adjust horizontal margins based on the indicator. */
	  if (LayoutIsRtoLP(newcbox))
	    {
	      if ((int)newcbox->toggle.ind_right_delta + right_delta > 0)
		{
		  newcbox->label.margin_right += right_delta;
		  newcbox->toggle.ind_right_delta += right_delta;
		}
	      else
		{
		  newcbox->label.margin_right -=
		    newcbox->toggle.ind_right_delta;
		  newcbox->toggle.ind_right_delta = 0;
		}
	    }
	  else
	    {
	      if ((int)newcbox->toggle.ind_left_delta + left_delta > 0)
		{
		  newcbox->label.margin_left += left_delta;
		  newcbox->toggle.ind_left_delta += left_delta;
		}
	      else
		{
		  newcbox->label.margin_left -= newcbox->toggle.ind_left_delta;
		  newcbox->toggle.ind_left_delta = 0;
		}
	    }

	  /* Realign the label. */
	  if (!newcbox->label.recompute_size)
	    (* resize) ((Widget) new_w);
	}

      if (newcbox->label.recompute_size)
	{
	  if (request->core.width == current->core.width)
            new_w->core.width = 0;
	  if (request->core.height == current->core.height)
            new_w->core.height = 0;
	}

      if (new_w->core.width == 0)
	{
	  newcbox->core.width =
	    newcbox->label.TextRect.width +
	      newcbox->label.margin_left + newcbox->label.margin_right +
		2 * (newcbox->primitive.highlight_thickness +
		     newcbox->primitive.shadow_thickness +
		     newcbox->label.margin_width);

	  if (newcbox->core.width == 0)
	    newcbox->core.width = 1;

	  flag = TRUE;
	}

      if (new_w->core.height == 0)
	{
	  newcbox->core.height =
	    MAX(newcbox->toggle.indicator_dim,
		newcbox->label.TextRect.height +
		2 * newcbox->label.margin_height +
		newcbox->label.margin_top + newcbox->label.margin_bottom) +
		  2 * (newcbox->primitive.highlight_thickness +
		       newcbox->primitive.shadow_thickness);

	  if (newcbox->core.height == 0)
	    newcbox->core.height = 1;

	  flag = TRUE;
	}
    }

  if ((newcbox->primitive.foreground != curcbox->primitive.foreground) ||
      (newcbox->core.background_pixel != curcbox->core.background_pixel) ||
      (newcbox->toggle.select_color != curcbox->toggle.select_color))
    {
      XtReleaseGC( (Widget) curcbox, curcbox->toggle.select_GC);
      XtReleaseGC( (Widget) curcbox, curcbox->toggle.background_gc);
      XtReleaseGC( (Widget) curcbox, curcbox->toggle.indeterminate_GC);
      XtReleaseGC( (Widget) curcbox, curcbox->toggle.indeterminate_box_GC);

      newcbox->toggle.reversed_select =
	(newcbox->toggle.select_color == XmREVERSED_GROUND_COLORS);
      if (newcbox->toggle.select_color == XmDEFAULT_SELECT_COLOR)
	{
	  /* CR 9923: Copy all bytes of the resulting pixel. */
	  XrmValue value;
	  DefaultSelectColor((Widget)newcbox,
			     XtOffsetOf(XmToggleButtonRec,toggle.select_color),
			     &value);
	  assert(value.size == sizeof(Pixel));
	  newcbox->toggle.select_color = *((Pixel*) value.addr);
	}
      else if (newcbox->toggle.select_color == XmREVERSED_GROUND_COLORS)
	{
	  newcbox->toggle.select_color = newcbox->primitive.foreground;
	}
      else if (newcbox->toggle.select_color == XmHIGHLIGHT_COLOR)
	{
	  newcbox->toggle.select_color = newcbox->primitive.highlight_color;
	}

      GetGC(newcbox);
      flag = TRUE;
    }

  if (newcbox->toggle.unselect_color != curcbox->toggle.unselect_color)
    {
      XtReleaseGC ((Widget) curcbox, curcbox->toggle.unselect_GC);
      GetUnselectGC(newcbox);
      flag = TRUE;
    }


  if ((curcbox->toggle.ind_type != newcbox->toggle.ind_type) &&
      (!XmRepTypeValidValue(XmRID_INDICATOR_TYPE,
			    newcbox->toggle.ind_type, (Widget) newcbox)))
    {
      newcbox->toggle.ind_type = curcbox->toggle.ind_type;
    }

  if (curcbox->toggle.set != newcbox->toggle.set)
    {
      if ((newcbox->toggle.toggle_mode == XmTOGGLE_BOOLEAN) &&
	  (newcbox->toggle.set == XmINDETERMINATE))
	{
	  newcbox->toggle.set = curcbox->toggle.set;
	}
      else
	{
	  IsOn(newcbox) = newcbox->toggle.set;
	  if (flag == False && XtIsRealized((Widget)newcbox))
	    {
	      if (newcbox->toggle.ind_on)
		{
		  DrawToggle (newcbox);
		  if (Lab_IsPixmap(newcbox) || Lab_IsPixmapAndText(newcbox))
		    SetAndDisplayPixmap(newcbox, NULL, NULL);
		}
	      else
		{
		  /* Begin fixing OSF 5946 */
		  if (newcbox->primitive.shadow_thickness > 0)
		    DrawToggleShadow (newcbox);
		  if (newcbox->toggle.fill_on_select && !Lab_IsPixmap(newcbox))
		    DrawToggleLabel (newcbox);
		  if (Lab_IsPixmap(newcbox) || Lab_IsPixmapAndText(newcbox))
		    {
		    SetAndDisplayPixmap(newcbox, NULL, NULL);
		    flag = True;
		    }
		  /* End fixing OSF 5946 */
		}
	    }
	}
    }

  if ((curcbox->toggle.ind_type != newcbox->toggle.ind_type) ||
      ( (curcbox->toggle.visible != newcbox->toggle.visible) && (XmUNSET == newcbox->toggle.set)) )
    {
      flag = True;
    }

  /* One-of-many forces boolean mode. */
  if (IsOneOfMany(newcbox->toggle.ind_type) &&
      (newcbox->toggle.toggle_mode == XmTOGGLE_INDETERMINATE))
    {
      newcbox->toggle.toggle_mode = XmTOGGLE_BOOLEAN;
    }

  /*
   * Transition between True/False is easy.  Transition from
   * indetermine is done by setting the toggle to False.
   */
  if ((curcbox->toggle.toggle_mode != newcbox->toggle.toggle_mode) &&
      (newcbox->toggle.toggle_mode == XmTOGGLE_BOOLEAN) &&
      (newcbox->toggle.set == XmINDETERMINATE))
    {
      newcbox->toggle.visual_set = newcbox->toggle.set = XmUNSET;
      flag =  True;
    }

  return(flag);
}

/***************************************************************
 *
 * XmToggleButtonGetState
 *   This function gets the state of the toggle widget.
 *
 ***************************************************************/
Boolean
XmToggleButtonGetState(
        Widget w )
{
  XmToggleButtonWidget tw = (XmToggleButtonWidget) w;
  Boolean ret_val;
  XtAppContext app = XtWidgetToApplicationContext(w);

  if (XmIsGadget(w))
    return XmToggleButtonGadgetGetState(w);

  _XmAppLock(app);
  ret_val = tw->toggle.set;
  _XmAppUnlock(app);

  return (ret_val);
}

/****************************************************************
 *
 * XmTogglebuttonSetState
 *   This function sets the state of the toggle widget.
 *
 ****************************************************************/
void
XmToggleButtonSetState(
        Widget w,
#if NeedWidePrototypes
        int bnewstate,
        int notify )
#else
        Boolean bnewstate,
        Boolean notify )
#endif /* NeedWidePrototypes */
{
  XmToggleButtonWidget tw = (XmToggleButtonWidget) w;
  XmMenuSystemTrait menuSTrait;
  XtAppContext app = XtWidgetToApplicationContext(w);
  XmToggleButtonState newstate;

  if (XmIsGadget(w)) {
    XmToggleButtonGadgetSetState(w, bnewstate, notify);
    return;
  }

  _XmAppLock(app);

  /* toggel.set is enum type, not Boolean */
  newstate = (bnewstate == XmSET)? XmSET : XmUNSET;

  if (tw->toggle.set != newstate)
    {
      tw->toggle.set = newstate;
      IsOn(tw) = newstate;
      if (XtIsRealized ((Widget)tw))
	{
	  if (tw->toggle.ind_on)
            DrawToggle(tw);
	  else
	    {
	      if (tw->primitive.shadow_thickness > 0)
		DrawToggleShadow (tw);
	      if (tw->toggle.fill_on_select && !Lab_IsPixmap(tw))
		DrawToggleLabel (tw);
	    }

	  if (Lab_IsPixmap(tw) || Lab_IsPixmapAndText(tw))
            SetAndDisplayPixmap( tw, NULL, NULL);
	}

      if (notify)
	{
	  /* UNDOING this fix ... */
	  /* CR 8904: Notify value_changed before entry so that state is */
	  /* 	reported correctly even if the entry callback resets it. */
	  menuSTrait = (XmMenuSystemTrait)
	    XmeTraitGet((XtPointer) XtClass(XtParent(tw)), XmQTmenuSystem);

          if (menuSTrait != NULL)
	    {
	      XmToggleButtonCallbackStruct call_value;

	      call_value.reason = XmCR_VALUE_CHANGED;
	      call_value.event = NULL;
	      call_value.set = tw->toggle.set;

	      menuSTrait->entryCallback(XtParent(tw), (Widget)tw, &call_value);
	    }

          if ((! tw->label.skipCallback) &&
              (tw->toggle.value_changed_CB))
	    {
	      XFlush(XtDisplay(tw));
	      ToggleButtonCallback(tw, XmCR_VALUE_CHANGED,
				   tw->toggle.set, NULL);
	    }

	}
    }
  _XmAppUnlock(app);
}

/****************************************************************
 *
 * XmToggleButtonSetValue
 *   This function sets the state of the toggle widget.
 *
 ****************************************************************/
Boolean
XmToggleButtonSetValue(
        Widget w,
#if NeedWidePrototypes
        int newstate,
        int notify )
#else
        XmToggleButtonState newstate,
        Boolean notify )
#endif /* NeedWidePrototypes */
{
  XmToggleButtonWidget tw = (XmToggleButtonWidget) w;
  XtAppContext app = XtWidgetToApplicationContext(w);

  if (XmIsGadget(w))
    return XmToggleButtonGadgetSetValue(w, newstate, notify);

  _XmAppLock(app);

  /* Can't set third state if we aren't in three state mode. */
  if ((newstate == XmINDETERMINATE) &&
      (tw->toggle.toggle_mode != XmTOGGLE_INDETERMINATE)) {
    _XmAppUnlock(app);
    return False;
  }

  if (tw->toggle.set != newstate)
    {
      tw->toggle.set = tw->toggle.visual_set = newstate;
      if (XtIsRealized ((Widget)tw))
	{
	  if (tw->toggle.ind_on)
            DrawToggle(tw);
	  else
	    {
	      if (tw->primitive.shadow_thickness > 0)
		DrawToggleShadow (tw);
	      if (tw->toggle.fill_on_select && !Lab_IsPixmap(tw))
		DrawToggleLabel (tw);
	    }
	  if (Lab_IsPixmap(tw) || Lab_IsPixmapAndText(tw))
            SetAndDisplayPixmap( tw, NULL, NULL);
	}

      if (notify)
	{
          /* If the parent is a RowColumn, notify it about the select. */
          if (XmIsRowColumn(XtParent(tw)))
	    {
	      XmToggleButtonCallbackStruct call_value;
	      call_value.reason = XmCR_VALUE_CHANGED;
	      call_value.event = NULL;
	      call_value.set = tw->toggle.set;
	      (* xmLabelClassRec.label_class.menuProcs)
		(XmMENU_CALLBACK, XtParent(tw), FALSE, tw, &call_value);
	    }

          if ((! tw->label.skipCallback) &&
              (tw->toggle.value_changed_CB))
	    {
	      XFlush(XtDisplay(tw));
	      ToggleButtonCallback(tw, XmCR_VALUE_CHANGED,
				   tw->toggle.set, NULL);
	    }
	}
    }

  _XmAppUnlock(app);
  return True;
}

/***********************************************************************
 *
 * XmCreateToggleButton
 *   Creates an instance of a togglebutton and returns the widget id.
 *
 ************************************************************************/
Widget
XmCreateToggleButton(
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount )
{
  return XtCreateWidget(name, xmToggleButtonWidgetClass, parent,
			arglist, argCount);
}

Widget
XmVaCreateToggleButton(
        Widget parent,
        char *name,
        ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmToggleButtonWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedToggleButton(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmToggleButtonWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}

/*
 * DrawToggleShadow (tb)
 *   - Should be called only if ToggleShadow are to be drawn ;
 *	if the IndicatorOn resource is set to false top and bottom shadows
 *	will be switched depending on whether the Toggle is selected or
 *	unselected.
 *   No need to call the routine if shadow_thickness is 0.
 */
static void
DrawToggleShadow(
        XmToggleButtonWidget tb )
{
  GC top_gc, bot_gc;
  int width, height;
  int hilite_thickness;

  if (tb->toggle.ind_on || (IsOn(tb) == XmUNSET))
    {
      top_gc = tb->primitive.top_shadow_GC;
      bot_gc = tb->primitive.bottom_shadow_GC;
    }
  else if (IsOn(tb) == XmINDETERMINATE)
    {
      top_gc = bot_gc = tb->toggle.indeterminate_box_GC;
    }
  else
    {
      top_gc = tb->primitive.bottom_shadow_GC;
      bot_gc = tb->primitive.top_shadow_GC;
    }

  hilite_thickness = tb->primitive.highlight_thickness;
  width = (int) (tb->core.width - (hilite_thickness << 1));
  height = (int) (tb->core.height - (hilite_thickness << 1));

  XmeDrawShadows (XtDisplay (tb), XtWindow (tb), top_gc, bot_gc,
		  hilite_thickness, hilite_thickness, width, height,
		  tb->primitive.shadow_thickness, XmSHADOW_OUT);
}

/*
 * DrawToggleLabel (tb)
 *    Called when XmNindicatorOn is FALSE and XmNfillOnSelect is TRUE.
 *    Fill toggle with selectColor or background depending on toggle
 *    value, and draw label.
 */
static void
DrawToggleLabel(
        XmToggleButtonWidget tb )
{
  Dimension margin = (tb->primitive.highlight_thickness +
		      tb->primitive.shadow_thickness);
  Position fx = margin;
  Position fy = margin;
  int fw = tb->core.width - 2 * margin;
  int fh = tb->core.height - 2 * margin;
  Boolean restore_gc = False;
  GC tmp_gc = None, fill_gc;

  if (tb->primitive.top_shadow_color == tb->toggle.select_color ||
      tb->primitive.bottom_shadow_color == tb->toggle.select_color)
    {
      fx += 1;
      fy += 1;
      fw -= 2;
      fh -= 2;
    }

  if (fw < 0 || fh < 0)
    return;

  switch (tb->toggle.visual_set)
    {
    case XmUNSET:
      fill_gc = tb->toggle.unselect_GC;
      break;
    case XmSET:
      fill_gc = tb->toggle.select_GC;
      break;
    case XmINDETERMINATE:
      {
	XGCValues values;

	/* Fetch the select_color GetGC() actually used. */
	XGetGCValues(XtDisplay(tb), tb->toggle.select_GC,
		     GCForeground, &values);
	values.background = tb->toggle.unselect_color;
	XChangeGC(XtDisplay((Widget)tb), tb->toggle.indeterminate_GC,
		  GCForeground|GCBackground, &values);
	fill_gc = tb->toggle.indeterminate_GC;
	break;
      }
    default:
      assert(False);
      return;
    }

  XFillRectangle (XtDisplay(tb), XtWindow(tb), fill_gc, fx, fy, fw, fh);


  if ((tb->primitive.foreground == tb->toggle.select_color) &&
      (IsOn(tb) == XmSET))
    {
      tmp_gc = tb->label.normal_GC;
      tb->label.normal_GC = tb->toggle.background_gc;
      restore_gc = True;
    }

  {

	XtExposeProc expose;
#ifdef FIX_1395
       Pixel tmpc = tb->core.background_pixel; /* Save bg color */

       /* Changing label background color if button toggled */
       if (tb->toggle.visual_set == XmSET && !Lab_IsMenupane(tb))
        XSetWindowBackground(XtDisplay(tb), XtWindow(tb), tb->toggle.select_color);
#endif
        _XmProcessLock();
        expose = xmLabelClassRec.core_class.expose;
        _XmProcessUnlock();
        (* expose) ((Widget) tb, NULL, NULL);
#ifdef FIX_1395
	/* restore default bg color */
	if (tb->toggle.visual_set == XmSET && !Lab_IsMenupane(tb) )
	    XSetWindowBackground(XtDisplay(tb), XtWindow(tb), tmpc);
#endif
   }

  if (restore_gc)
    {
      /* CR 9791: Label's normal_gc has a dynamic clip_mask. */
      XSetClipMask(XtDisplay(tb), tb->toggle.background_gc, None);
      tb->label.normal_GC = tmp_gc;
    }
}

/*
 * DrawEtchedInMenu (tb)
 *    Called when in a Menu and EtchedInMenu is TRUE.
 *    And when XmNindicatorOn is FALSE and XmNfillOnSelect is FALSE;
 *    or when XmNindicatorOn is TRUE.
 *    Fill background with the arm_gc and draw label.
 */
static void
DrawEtchedInMenu(
        XmToggleButtonWidget tb )
{
  Dimension margin = (tb->primitive.highlight_thickness +
		      tb->primitive.shadow_thickness);
  Position fx = margin;
  Position fy = margin;
  int fw = tb->core.width - 2 * margin;
  int fh = tb->core.height - 2 * margin;
  Boolean restore_gc = False;
  GC tmp_gc = None;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(tb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;
  Pixel select_pix, tmpc;

  if (tb->primitive.top_shadow_color == tb->toggle.select_color ||
      tb->primitive.bottom_shadow_color == tb->toggle.select_color)
    {
      fx += 1;
      fy += 1;
      fw -= 2;
      fh -= 2;
    }

  if (fw < 0 || fh < 0)
    return;

  XFillRectangle (XtDisplay(tb), XtWindow(tb),
		  tb->toggle.Armed ? tb->toggle.arm_GC :
		                     tb->toggle.background_gc,
		  fx, fy, fw, fh);

  if (tb->toggle.Armed)
    {
	tmpc = tb->core.background_pixel;

	XmGetColors(XtScreen(tb), tb->core.colormap,
		    tb->core.background_pixel,
		    NULL, NULL, NULL, &select_pix);

	if (tb->primitive.foreground == select_pix)
	  {
	      tmp_gc = tb->label.normal_GC;
	      tb->label.normal_GC = tb->toggle.background_gc;
	      restore_gc = True;
	  }
    }

  {
       XtExposeProc expose;
#ifdef FIX_1395
	if (tb->toggle.Armed)
	{
	    XSetWindowBackground(XtDisplay(tb), XtWindow(tb), select_pix);
	}
	else
	{
	    XSetWindowBackground(XtDisplay(tb), XtWindow(tb), tb->core.background_pixel);
	}
#endif
	_XmProcessLock();
	expose = xmLabelClassRec.core_class.expose;
	_XmProcessUnlock();
	(* expose) ((Widget) tb, NULL, NULL);
   }

  if (restore_gc)
    {
      XSetClipMask(XtDisplay(tb), tb->toggle.background_gc, None);
      tb->label.normal_GC = tmp_gc;
    }
}

/* BEGIN OSF Fix pir 1778 */
/*************************************************************************
 *
 * SetToggleSize(newtb)
 * Set size properly when XmNselectPixmap or XmNselectInsensitivePixmaps
 * are set in addition to the corresponding labelPixmaps.  Have to pick
 * the largest dimensions.
 *
 ************************************************************************/

static void
SetToggleSize(
     XmToggleButtonWidget newtb)
{
  unsigned int maxW, maxH;
  unsigned int tmpW, tmpH;

  maxW = maxH = tmpW = tmpH = 0;

  /* We know it's a pixmap so find out how how big it is */
  if (XtIsSensitive((Widget) newtb))
    {
      if (!IsNull(PixmapOn(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapOn(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapOff(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapOff(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInd(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapInd(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}
    }
  else
    {
      if (!IsNull(PixmapInsenOn(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapInsenOn(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInsenOff(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapInsenOff(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInsenInd(newtb)))
	{
	  XmeGetPixmapData(XtScreen(newtb), PixmapInsenInd(newtb),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}
    }

  newtb->label.TextRect.width = (unsigned short) maxW;
  newtb->label.TextRect.height = (unsigned short) maxH;

  /* Invoke Label's SetSize procedure. */
  {
      XtWidgetProc resize;
      _XmProcessLock();
      resize = xmLabelClassRec.core_class.resize;
      _XmProcessUnlock();

      (* resize) ((Widget) newtb);
  }
}
/* END OSF Fix pir 1778 */

/*
 * DefaultSelectColor - an XtResourceDefaultProc for generating the
 *	default select color.  This may require examining the
 *	XmNindicatorType and XmNhighlightColor resources, which will
 *	happen to have real values only because they appear before
 *	XmNselectColor in the resource list.
 */
static void
DefaultSelectColor(Widget widget,
		   int offset,
		   XrmValue *value)
{
  XmToggleButtonWidget tb = (XmToggleButtonWidget) widget;
  Boolean force_highlight = FALSE;
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(widget));
  Boolean enable_toggle_color = xm_dpy->display.enable_toggle_color;

  if (enable_toggle_color)
    {
      /* This code may misbehave for erroneous ind_type values. */
      if (IsOneOfMany(tb->toggle.ind_type))
	{
	  force_highlight = TRUE;
	}
      else if ((tb->toggle.ind_type == XmINVALID_TYPE) &&
	       XmIsRowColumn(XtParent(widget)))
	{
	  XtVaGetValues(XtParent(widget),
			XmNradioBehavior, &force_highlight,
			NULL);
	}
    }

  if (force_highlight)
    {
      value->size = sizeof(tb->primitive.highlight_color);
      value->addr = (char *) &tb->primitive.highlight_color;
    }
  else
    _XmSelectColorDefault(widget, offset, value);
}

/*
 * NormalizeIndOn - return the normalized value of XmNindicatorOn,
 *	replacing XmINDICATOR_FILL and XmINDICATOR_BOX with the proper
 *	absolute values.
 */
static unsigned char
NormalizeIndOn(XmToggleButtonWidget tb)
{
  unsigned char value = tb->toggle.ind_on;

  /* Convert XmINDICATOR_FILL to XmINDICATOR_CHECK_BOX? */
  if (value == XmINDICATOR_FILL)
    {
      /* This routine may be called frequently, so reach directly into */
      /* the XmDisplay rather than calling XtGetValues. */
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) tb));

      if (xm_dpy->display.enable_toggle_visual)
	value = XmINDICATOR_CHECK_BOX;
    }

  /* Convert XmINDICATOR_BOX to XmINDICATOR_3D_BOX (XmINDICATOR_FILL). */
  else if (value == XmINDICATOR_BOX)
    {
      value = XmINDICATOR_3D_BOX;
    }

  return value;
}

/*
 * NormalizeIndType - return the normalized value of XmNindicatorType,
 *	replacing XmONE_OF_MANY with the proper absolute value.
 */
static unsigned char
NormalizeIndType(XmToggleButtonWidget tb)
{
  unsigned char value = tb->toggle.ind_type;

  if (value == XmONE_OF_MANY)
    {
      /* This routine may be called frequently, so reach directly into */
      /* the XmDisplay rather than calling XtGetValues. */
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) tb));

      if (xm_dpy->display.enable_toggle_visual)
	value = XmONE_OF_MANY_ROUND;
      else
	value = XmONE_OF_MANY_DIAMOND;
    }

  return value;
}
