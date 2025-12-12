/*
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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: PushB.c /main/29 1999/01/27 16:08:33 mgreess $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/ActivatableT.h>
#include <Xm/BaseClassP.h>
#include <Xm/CareVisualT.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/PushBP.h>
#include <Xm/TakesDefT.h>
#include <Xm/TearOffBP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "LabelI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "PrimitiveI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"
#include "VaSimpleI.h"
#include "ExtP.h"

#define XmINVALID_MULTICLICK	255
#define DELAY_DEFAULT		100

/********    Static Function Declarations    ********/

static void Arm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void MultiArm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Activate(
                        Widget wid,
                        XEvent *buttonEvent,
                        String *params,
                        Cardinal *num_params) ;
static void MultiActivate(
                        Widget wid,
                        XEvent *buttonEvent,
                        String *params,
                        Cardinal *num_params) ;
static void ActivateCommon(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ArmTimeout(
                        XtPointer data,
                        XtIntervalId *id) ;
static void Disarm(
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
static void Enter(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Leave(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void BorderHighlight(
                        Widget wid) ;
static void DrawBorderHighlight(
                        Widget wid) ;
static void BorderUnhighlight(
                        Widget wid) ;
static void KeySelect(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
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
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetFillGC(
                        XmPushButtonWidget pb) ;
static void GetBackgroundGC(
                        XmPushButtonWidget pb) ;
static Boolean SetValuesPrehook(
			Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Help(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Destroy(
                        Widget w) ;
static void Resize(
                        Widget w) ;
static void EraseDefaultButtonShadow(
                        XmPushButtonWidget pb) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void DrawPushButtonBackground(
                        XmPushButtonWidget pb) ;
static void DrawPushButtonLabel(
                        XmPushButtonWidget pb,
                        XEvent *event,
                        Region region) ;
static void DrawPushButtonShadows(
                        XmPushButtonWidget pb) ;
static void ComputePBLabelArea(
                        XmPushButtonWidget pb,
                        XRectangle *box) ;
static void DrawPBPrimitiveShadows(
                        XmPushButtonWidget pb) ;
static void DrawDefaultButtonShadows(
                        XmPushButtonWidget pb) ;
static XmImportOperator ShowAsDef_ToHorizPix(
                        Widget widget,
                        int offset,
                        XtArgVal *value) ;
static int AdjustHighLightThickness(
                        XmPushButtonWidget new_w,
                        XmPushButtonWidget current) ;
static void ExportHighlightThickness(
                        Widget widget,
                        int offset,
                        XtArgVal *value) ;
static void FillBorderWithParentColor(
                        XmPushButtonWidget pb,
                        int borderwidth,
                        int dx,
                        int dy,
                        int rectwidth,
                        int rectheight) ;
static void SetPushButtonSize(
                        XmPushButtonWidget newtb) ;
static void ChangeCB(Widget w,
		     XtCallbackProc activCB,
		     XtPointer closure,
		     Boolean setunset) ;
static void ShowAsDefault(Widget w,
			  XtEnum state) ;
static Boolean ParentVisualChanged(Widget kid,
				   Widget cur_parent,
				   Widget new_parent,
				   Mask visual_flag);
static void PB_FixTearoff(XmPushButtonWidget pb);

/********    End Static Function Declarations    ********/

/*************************************<->*************************************
 *
 *
 *   Description:   translation tables for class: PushButton
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/

static XtTranslations default_parsed;

#define defaultTranslations	_XmPushB_defaultTranslations

static XtTranslations menu_parsed;

#define menuTranslations	_XmPushB_menuTranslations


/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: PushButton
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *   Note that Primitive will register additional event handlers
 *   for traversal.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  { "Arm", 			Arm			 },
  { "MultiArm", 		MultiArm		 },
  { "Activate", 		Activate		 },
  { "MultiActivate",		MultiActivate		 },
  { "ArmAndActivate", 		ArmAndActivate		 },
  { "Disarm", 			Disarm			 },
  { "BtnDown", 			BtnDown			 },
  { "BtnUp", 			BtnUp			 },
  { "Enter", 			Enter			 },
  { "Leave",			Leave			 },
  { "ButtonTakeFocus",       	_XmButtonTakeFocus	 },
  { "MenuButtonTakeFocus",   	_XmMenuButtonTakeFocus	 },
  { "MenuButtonTakeFocusUp", 	_XmMenuButtonTakeFocusUp },
  { "KeySelect",		KeySelect		 },
  { "Help",			Help			 },
};



/* The resource list for Push Button. */

static XtResource resources[] =
{
   {
     XmNmultiClick, XmCMultiClick,
     XmRMultiClick, sizeof (unsigned char),
     XtOffsetOf(XmPushButtonRec, pushbutton.multiClick),
     XmRImmediate, (XtPointer) XmINVALID_MULTICLICK
   },

   {
     XmNfillOnArm, XmCFillOnArm,
     XmRBoolean, sizeof (Boolean),
     XtOffsetOf(XmPushButtonRec, pushbutton.fill_on_arm),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNarmColor, XmCArmColor,
     XmRPixel, sizeof (Pixel),
     XtOffsetOf(XmPushButtonRec, pushbutton.arm_color),
     XmRCallProc, (XtPointer) _XmSelectColorDefault
   },

   {
     XmNarmPixmap, XmCArmPixmap,
     XmRDynamicPixmap, sizeof (Pixmap),
     XtOffsetOf(XmPushButtonRec, pushbutton.arm_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {
     XmNshowAsDefault, XmCShowAsDefault,
     XmRBooleanDimension, sizeof (Dimension),
     XtOffsetOf(XmPushButtonRec, pushbutton.show_as_default),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNactivateCallback, XmCCallback,
     XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf(XmPushButtonRec, pushbutton.activate_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNarmCallback, XmCCallback,
     XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf(XmPushButtonRec, pushbutton.arm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNdisarmCallback, XmCCallback,
     XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf(XmPushButtonRec, pushbutton.disarm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNshadowThickness, XmCShadowThickness,
     XmRHorizontalDimension, sizeof(Dimension),
     XtOffsetOf(XmPushButtonRec, primitive.shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNdefaultButtonShadowThickness, XmCDefaultButtonShadowThickness,
     XmRHorizontalDimension, sizeof (Dimension),
     XtOffsetOf(XmPushButtonRec, pushbutton.default_button_shadow_thickness),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNtraversalOn, XmCTraversalOn,
     XmRBoolean, sizeof(Boolean),
     XtOffsetOf(XmPrimitiveRec, primitive.traversal_on),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNhighlightThickness, XmCHighlightThickness,
     XmRHorizontalDimension, sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },
};

/* Synthetic resources list */

static XmSyntheticResource syn_resources[] =
{
  {
     XmNshowAsDefault, sizeof (Dimension),
     XtOffsetOf(XmPushButtonRec, pushbutton.show_as_default),
     XmeFromHorizontalPixels,
     ShowAsDef_ToHorizPix
  },

  {
     XmNdefaultButtonShadowThickness, sizeof (Dimension),
     XtOffsetOf(XmPushButtonRec, pushbutton.default_button_shadow_thickness),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels
  },

   {
     XmNhighlightThickness, sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
     ExportHighlightThickness,
     XmeToHorizontalPixels
   },
};


/*************************************<->*************************************
 *
 *
 *   Description:  global class record for instances of class: PushButton
 *   -----------
 *
 *   Defines default field settings for this class record.
 *
 *************************************<->***********************************/

static XmBaseClassExtRec       pushBBaseClassExtRec = {
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

externaldef(xmpushbuttonclassrec) XmPushButtonClassRec xmPushButtonClassRec =
{
  { /* core_class record */
    /* superclass	  */	(WidgetClass) &xmLabelClassRec,
    /* class_name	  */	"XmPushButton",
    /* widget_size	  */	sizeof(XmPushButtonRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    (XtArgsProc)NULL,
    /* realize		  */	XtInheritRealize,
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
    /* accept_focus	  */	(XtAcceptFocusProc)NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/    (XtStringProc)NULL,
    /* extension record   */    (XtPointer)&pushBBaseClassExtRec,
  },

  { /* primitive_class record       */
    /* Primitive border_highlight   */	BorderHighlight,
    /* Primitive border_unhighlight */	BorderUnhighlight,
    /* translations		    */  XtInheritTranslations,
    /* arm_and_activate		    */  ArmAndActivate,
    /* get resources		    */  syn_resources,
    /* num get_resources	    */  XtNumber(syn_resources),
    /* extension		    */  NULL,
  },

  { /* label_class record */
    /* setOverrideCallback*/	XmInheritWidgetProc,
    /* menu procedures    */	XmInheritMenuProc,
    /* menu traversal xlation */ XtInheritTranslations,
    /* extension	  */	(XtPointer) NULL,
  },

  { /* pushbutton_class record */
    /* extension	  */	(XtPointer) NULL,
  }
};


externaldef(xmpushbuttonwidgetclass)
   WidgetClass xmPushButtonWidgetClass = (WidgetClass)&xmPushButtonClassRec;



/* Activatable Trait record for pushButton */
static XmConst XmActivatableTraitRec pushButtonAT =
{
  0,		/* version */
  ChangeCB,
};

/* Care visual Trait record for pushButton */
static XmConst XmCareVisualTraitRec pushButtonCVT = {
  0,		/* version */
  ParentVisualChanged,
};

/* TakesDefault Trait record for pushButton */
static XmConst XmTakesDefaultTraitRec pushButtonTDT =
{
  0,		/* version */
  ShowAsDefault,
};

/* Menu Savvy trait record */
static XmMenuSavvyTraitRec MenuSavvyRecord = {
    /* version: */
    -1,
    NULL,
    NULL,
    NULL,
    _XmCBNameActivate,
};


/*************************************<->*************************************
 *
 *  Synthetic hooks
 *
 *************************************<->***********************************/

static XmImportOperator
ShowAsDef_ToHorizPix(
        Widget widget,
        int offset,
        XtArgVal *value )
{
  XtArgVal        oldValue ;
  XmImportOperator returnVal ;

  oldValue = *value ;
  returnVal = XmeToHorizontalPixels (widget, offset, value) ;

  if (oldValue  &&  !*value)
    *value = (XtArgVal) 1;

  return (returnVal) ;
}

static void
ExportHighlightThickness(
        Widget widget,
        int offset,
        XtArgVal *value )
{
  XmPushButtonWidget pbw = (XmPushButtonWidget) widget;
  if (pbw->pushbutton.show_as_default ||
      pbw->pushbutton.default_button_shadow_thickness)
    {
      if ((int)*value >= Xm3D_ENHANCE_PIXEL)
	*value -= Xm3D_ENHANCE_PIXEL;
    }

  XmeFromHorizontalPixels (widget, offset, value);
}

/*************************************<->*************************************
 *
 *  ClassInitialize
 *
 *************************************<->***********************************/

static void
ClassInitialize( void )
{
  /* parse the various translation tables */
  menu_parsed    = XtParseTranslationTable(menuTranslations);
  default_parsed = XtParseTranslationTable(defaultTranslations);

  /* set up base class extension quark */
  pushBBaseClassExtRec.record_type = XmQmotif;
}



/************************************************************************
 *
 *  ClassPartInitialize
 *     Set up the fast subclassing for the widget
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmPUSH_BUTTON_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabel */
  _XmLabelCloneMenuSavvy (wc, &MenuSavvyRecord);

  /* Install the activatable trait for all subclasses */
  XmeTraitSet((XtPointer) wc, XmQTactivatable, (XtPointer) &pushButtonAT);

  /* Install the takesDefault trait for all subclasses */
  XmeTraitSet((XtPointer) wc, XmQTtakesDefault, (XtPointer) &pushButtonTDT);

  /* Override primitive's careParentVisual trait for all subclasses. */
  XmeTraitSet((XtPointer) wc, XmQTcareParentVisual, (XtPointer)&pushButtonCVT);
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
  XmPushButtonWidget bw = (XmPushButtonWidget) new_w;
  unsigned char type;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(new_w)), XmQTmenuSystem);

  _XmSaveCoreClassTranslations (new_w);

  if (menuSTrait != NULL)
    type = menuSTrait->type(XtParent(new_w));
  else
    type = XmWORK_AREA;

  _XmProcessLock();
  if (type == XmMENU_PULLDOWN ||
      type == XmMENU_POPUP)
    new_w->core.widget_class->core_class.tm_table = (String) menu_parsed;
  else
    new_w->core.widget_class->core_class.tm_table = (String) default_parsed;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (bw->label.font == NULL)
    bw->label.font = XmeGetDefaultRenderTable (new_w, XmBUTTON_FONTLIST);
  _XmProcessUnlock();
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

/************************************************************************
 *
 *  GetFillGC
 *     Get the graphics context used for filling in background of button.
 *
 ************************************************************************/

static void
GetFillGC(
        XmPushButtonWidget pb )
{
  XGCValues values;
  XtGCMask  valueMask;

  valueMask = GCForeground | GCBackground | GCFillStyle;

  values.foreground = pb->pushbutton.arm_color;
  values.background = pb->core.background_pixel;
  values.fill_style = FillSolid;

  pb->pushbutton.fill_gc = XtGetGC ((Widget) pb, valueMask, &values);
}

/************************************************************************
 *
 *  GetBackgroundGC
 *     Get the graphics context used for filling in background of
 *     the pushbutton when not armed.
 *
 ************************************************************************/
static void
GetBackgroundGC(
        XmPushButtonWidget pb )
{
  XGCValues       values;
  XtGCMask        valueMask;
  XFontStruct     *fs;

  valueMask = GCForeground | GCBackground | GCFont | GCGraphicsExposures;

  values.foreground = pb->core.background_pixel;
  values.background = pb->primitive.foreground;
  values.graphics_exposures = False;

  if (XmeRenderTableGetDefaultFont(pb->label.font, &fs))
    values.font = fs->fid;
  else
    valueMask &= ~GCFont;

  /* add background_pixmap to GC */
  if (pb->core.background_pixmap != XmUNSPECIFIED_PIXMAP)
    {
      values.tile = pb->core.background_pixmap;
      values.fill_style = FillTiled;
      valueMask |= (GCTile | GCFillStyle);
    }

  pb->pushbutton.background_gc = XtGetGC ((Widget) pb,valueMask,&values);
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
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmPushButtonWidget request = (XmPushButtonWidget) rw ;
  XmPushButtonWidget new_w = (XmPushButtonWidget) nw ;
  int		     increase;
  int		     adjustment = 0;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(new_w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if (new_w->pushbutton.multiClick == XmINVALID_MULTICLICK)
    {
      if (Lab_IsMenupane(new_w))
	new_w->pushbutton.multiClick = XmMULTICLICK_DISCARD;
      else
	new_w->pushbutton.multiClick = XmMULTICLICK_KEEP;
    }

  /* if menuProcs is not set up yet, try again */
  _XmProcessLock();
  if (xmLabelClassRec.label_class.menuProcs == NULL)
    xmLabelClassRec.label_class.menuProcs =
      (XmMenuProc) _XmGetMenuProcContext();
  _XmProcessUnlock();

  /*
   * Fix to introduce Resource XmNdefaultBorderWidth and compatibility
   *  variable.
   *  if defaultBorderWidth > 0, the program knows about this resource
   *  and is therefore a Motif 1.1 program; otherwise it is a Motif 1.0
   *      program and old semantics of XmNshowAsDefault prevails.
   */
  if (new_w->pushbutton.default_button_shadow_thickness > 0)
    new_w->pushbutton.compatible = False;
  else
    new_w->pushbutton.compatible = True;

  /*
   * showAsDefault as boolean if compatibility is false (Motif 1.1) else
   *  treat it to indicate the thickness of defaultButtonShadow.
   */
  if (new_w->pushbutton.compatible)
    new_w->pushbutton.default_button_shadow_thickness =
      new_w->pushbutton.show_as_default;

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen(nw), NULL, NULL) !=
      XmUNSPECIFIED_PIXMAP)
    {
      new_w->pushbutton.compatible = False;
      new_w->pushbutton.default_button_shadow_thickness = 0 ;
    }
#endif

  new_w->pushbutton.armed = FALSE;
  new_w->pushbutton.timer = 0;

  /* No unarm_pixmap but do have an arm_pixmap, use that. */
  if ((new_w->label.pixmap == XmUNSPECIFIED_PIXMAP) &&
      (new_w->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP))
    {
      XtWidgetProc resize;
      new_w->label.pixmap = new_w->pushbutton.arm_pixmap;
      if (request->core.width == 0)
	new_w->core.width = 0;
      if (request->core.height == 0)
	new_w->core.height = 0;

      _XmCalcLabelDimensions(nw);
      _XmProcessLock();
      resize = xmLabelClassRec.core_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) new_w);
    }

    if ((new_w->label.label_type == XmPIXMAP ||
       new_w->label.label_type == XmPIXMAP_AND_STRING) &&
       (new_w->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP))
    {
      if (request->core.width == 0)
	new_w->core.width = 0;
      if (request->core.height == 0)
	new_w->core.height = 0;
      SetPushButtonSize(new_w);
    }

  new_w->pushbutton.unarm_pixmap = new_w->label.pixmap;

  if (new_w->pushbutton.default_button_shadow_thickness)
    {
      /*
       * Special hack for 3d enhancement of location cursor highlight.
       *  - Make the box bigger. During drawing of location cursor
       *    make it smaller.  See in Primitive.c
       *  Maybe we should use the macro: G_HighLightThickness(pbgadget);
       */
      new_w->primitive.highlight_thickness += Xm3D_ENHANCE_PIXEL;
      adjustment = Xm3D_ENHANCE_PIXEL;
      increase =  (2 * new_w->pushbutton.default_button_shadow_thickness +
		   new_w->primitive.shadow_thickness + adjustment);

      /* Add the increase to the core to compensate for extra space */
      if (increase != 0)
	{
	  Lab_MarginLeft(new_w) += increase;
	  Lab_MarginRight(new_w) += increase;
	  Lab_TextRect_x(new_w) += increase ;
	  new_w->core.width += (increase << 1);

	  Lab_MarginTop(new_w) += increase;
	  Lab_MarginBottom(new_w) += increase;
	  Lab_TextRect_y(new_w) += increase ;
	  new_w->core.height += (increase << 1);
	}
    }

  if (Lab_IsMenupane(new_w))
    {
      new_w->primitive.traversal_on = TRUE;
    }

  if (etched_in || !Lab_IsMenupane(new_w)) {
      /* Initialize GCs for fill and background */
      GetFillGC (new_w);
      GetBackgroundGC (new_w);
  }

}

#ifdef DEFAULT_GLYPH_PIXMAP

/*
 * DrawDefaultGlyphPixmap (pb)
 */

static void
DrawDefaultGlyphPixmap(
        XmPushButtonWidget pb )
{
  int dx, dy, width, height ;
  Pixmap def_pixmap ;
  unsigned int def_pixmap_width, def_pixmap_height ;

  def_pixmap = _XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)),
					&def_pixmap_width,
					&def_pixmap_height) ;

  /* we draw in the margin right area here */
  dx = pb->core.width -
    (Lab_MarginRight(pb) + Lab_MarginWidth(pb) +
     pb->primitive.highlight_thickness + pb->primitive.shadow_thickness) ;
  dy = pb->primitive.highlight_thickness +
    pb->primitive.shadow_thickness + Lab_MarginTop(pb) + Lab_MarginHeight(pb) +
      (MAX(Lab_TextRect_height(pb),
	   pb->label.acc_TextRect.height) - def_pixmap_height)/2;
  width = MIN(def_pixmap_width, Lab_MarginRight(pb));
  height = MIN(def_pixmap_height,
	       MAX(Lab_TextRect_height(pb), pb->label.acc_TextRect.height));
  XCopyPlane (XtDisplay (pb), def_pixmap, XtWindow (pb),
	      pb->label.normal_GC, 0, 0, width, height, dx, dy, 1);
}
#endif /* DEFAULT_GLYPH_PIXMAP */

#ifdef DEFAULT_GLYPH_PIXMAP

/*
 * EraseDefaultGlyphPixmap (pb)
 */

static void
EraseDefaultGlyphPixmap(
        XmPushButtonWidget pb )
{
  int dx, dy, width, height ;

  /* we clear the margin right area here */
  dx = pb->core.width -
    (Lab_MarginRight(pb) + Lab_MarginWidth(pb) +
     pb->primitive.highlight_thickness + pb->primitive.shadow_thickness) ;
  dy = pb->primitive.highlight_thickness +
    pb->primitive.shadow_thickness + Lab_MarginTop(pb) + Lab_MarginHeight(pb) ;

  width = Lab_MarginRight(pb) ;
  height = MAX(Lab_TextRect_height(pb), pb->label.acc_TextRect.height) ;

  XClearArea (XtDisplay (pb), XtWindow (pb),
	      dx, dy, width, height, False);
}
#endif /* DEFAULT_GLYPH_PIXMAP */

/*
 * EraseDefaultButtonShadow (pb)
 *  - Called from SetValues() - effort to optimize shadow drawing.
 */

static void
EraseDefaultButtonShadow(
        XmPushButtonWidget pb )
{
  int size, x, y, width, height, delta;
  XtEnum default_button_emphasis;

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
      XmUNSPECIFIED_PIXMAP)
    {
      EraseDefaultGlyphPixmap( pb) ;
      return ;
    }
#endif

  size = pb->pushbutton.default_button_shadow_thickness;

  if (size > 0)
    {
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

      default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmEXTERNAL_HIGHLIGHT:
	  delta = pb->primitive.highlight_thickness;
	  break;

	case XmINTERNAL_HIGHLIGHT:
	  delta = Xm3D_ENHANCE_PIXEL;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      size += Xm3D_ENHANCE_PIXEL;
      x = y = delta;
      width = pb->core.width - (2 * delta);
      height = pb->core.height - (2 * delta);

      FillBorderWithParentColor(pb, size, x, y, width, height);
    }
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
  XmPushButtonWidget new_w = (XmPushButtonWidget) nw ;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (new_w->label.font == NULL)
    new_w->label.font = XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST);

  return False;
}

/*************************************<->*************************************
 *
 *  SetValues(current, request, new_w)
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmPushButtonWidget current = (XmPushButtonWidget) cw ;
  XmPushButtonWidget request = (XmPushButtonWidget) rw ;
  XmPushButtonWidget new_w = (XmPushButtonWidget) nw ;
  int increase;
  Boolean  flag = FALSE;    /* our return value */
  int adjustment;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(new_w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  /*
   * Fix to introduce Resource XmNdefaultBorderWidth and compatibility
   *      variable.
   *  if the XmNdefaultBorderWidth resource in the current differ from the
   *  one in "new_w", then the programmer is setting this resource - so this
   *  is known to the programmer and hence it is a Motif1.1 program.
   *  If they are same then either it is a Motif 1.0 program or there has been
   *  no change in the resource (Motif 1.1 program). If it is a Motif 1.0
   *  program, then we should copy the value of XmNshowAsDefault to the
   *  XmNdefaultBorderWidth. If it is  Motif 1.1 program  (Compatible
   *  flag is false) - then we should not do the copy.
   *  This logic will maintain the semantics of the  XmNshowAsDefault of Motif
   *  1.0. For a full explanation see the Design architecture document.
   */

  if ((current->pushbutton.default_button_shadow_thickness) !=
      (new_w->pushbutton.default_button_shadow_thickness))
    new_w->pushbutton.compatible = False;

  if (new_w->pushbutton.compatible)
    new_w->pushbutton.default_button_shadow_thickness =
      new_w->pushbutton.show_as_default;

  adjustment = AdjustHighLightThickness (new_w, current);

  /*
   * Compute size change.
   */
  if (new_w->pushbutton.default_button_shadow_thickness !=
      current->pushbutton.default_button_shadow_thickness)
    {
      if (new_w->pushbutton.default_button_shadow_thickness >
	  current->pushbutton.default_button_shadow_thickness)
	{
	  increase = (2 * new_w->pushbutton.default_button_shadow_thickness +
		      new_w->primitive.shadow_thickness);
	  if (current->pushbutton.default_button_shadow_thickness > 0)
            increase -=
	      (2 * current->pushbutton.default_button_shadow_thickness +
	       current->primitive.shadow_thickness);
	}
      else
	{
	  increase = -(2 * current->pushbutton.default_button_shadow_thickness +
		       current->primitive.shadow_thickness);
	  if (new_w->pushbutton.default_button_shadow_thickness > 0)
	    increase += (2 * new_w->pushbutton.default_button_shadow_thickness +
			 new_w->primitive.shadow_thickness);
	}

      increase += adjustment ;

      if (new_w->label.recompute_size || request->core.width == 0)
	{
	  Lab_MarginLeft(new_w) += increase;
	  Lab_MarginRight(new_w) += increase;
	  new_w->core.width += (increase << 1);
	  flag = TRUE;
	}
      else if (increase != 0)
	{
	  /* Add the change due to default button to the core */
	  Lab_MarginLeft(new_w) += increase;
	  Lab_MarginRight(new_w) += increase;
	  new_w->core.width += (increase << 1);
	  flag = TRUE;
	}

      if (new_w->label.recompute_size || request->core.height == 0)
	{
	  Lab_MarginTop(new_w) += increase;
	  Lab_MarginBottom(new_w) += increase;
	  new_w->core.height += (increase << 1);
	  flag = TRUE;
	}
      else if (increase != 0)
	{
	  /* Add the change due to default button to the core */
	  Lab_MarginTop(new_w) += increase;
	  Lab_MarginBottom(new_w) += increase;
	  new_w->core.height += (increase << 1);
	  flag = TRUE;
	}
    }

  if ((new_w->pushbutton.arm_pixmap != current->pushbutton.arm_pixmap) &&
      (new_w->label.label_type == XmPIXMAP) && (new_w->pushbutton.armed))
    flag = TRUE;

  /* No unarm_pixmap but do have an arm_pixmap, use that. */
  if ((new_w->label.pixmap == XmUNSPECIFIED_PIXMAP) &&
      (new_w->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP))
    {
      XtWidgetProc resize;
      new_w->label.pixmap = new_w->pushbutton.arm_pixmap;
      if (new_w->label.recompute_size &&
          request->core.width == current->core.width)
	new_w->core.width = 0;
      if (new_w->label.recompute_size &&
          request->core.height == current->core.height)
	new_w->core.width = 0;

      _XmCalcLabelDimensions(nw);
      _XmProcessLock();
      resize = xmLabelClassRec.core_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) new_w);
    }

  if (new_w->label.pixmap != current->label.pixmap)
    {
      new_w->pushbutton.unarm_pixmap = new_w->label.pixmap;
      if ((new_w->label.label_type == XmPIXMAP) && (!new_w->pushbutton.armed))
	flag = TRUE;
    }

  if ((new_w->label.label_type == XmPIXMAP ||
      new_w->label.label_type == XmPIXMAP_AND_STRING) &&
      (new_w->pushbutton.arm_pixmap != current->pushbutton.arm_pixmap))
    {
      if ((new_w->label.recompute_size))
	{
          if (request->core.width == current->core.width)
	    new_w->core.width = 0;
          if (request->core.height == current->core.height)
	    new_w->core.height = 0;
	}
      SetPushButtonSize(new_w);
      flag = TRUE;
    }

  if ((new_w->pushbutton.fill_on_arm != current->pushbutton.fill_on_arm) &&
      (new_w->pushbutton.armed == TRUE))
    flag = TRUE;


  if (! Lab_IsMenupane(new_w) || etched_in) {
      /*  See if the GC need to be regenerated and widget redrawn.  */
      if (new_w->pushbutton.arm_color != current->pushbutton.arm_color)
	{
	    if (new_w->pushbutton.armed) flag = TRUE;  /* see PIR 5091 */
	    XtReleaseGC ((Widget) new_w, new_w->pushbutton.fill_gc);
	    GetFillGC (new_w);
	}

      if (new_w->core.background_pixel != current->core.background_pixel ||
	  (new_w->core.background_pixmap != XmUNSPECIFIED_PIXMAP &&
	   new_w->core.background_pixmap != current->core.background_pixmap))
	{
	    flag = TRUE;  /* label will cause redisplay anyway */
	    XtReleaseGC ((Widget) new_w, new_w->pushbutton.background_gc);
	    GetBackgroundGC (new_w);
	}
  }

  /* OSF Fix pir 3469 */
  if (flag == False && XtIsRealized((Widget)new_w))
    {
      /* Size is unchanged - optimize the shadow drawing  */
      if ((current->pushbutton.show_as_default != 0) &&
	  (new_w->pushbutton.show_as_default == 0))
	EraseDefaultButtonShadow (new_w);

      if ((current->pushbutton.show_as_default == 0) &&
	  (new_w->pushbutton.show_as_default != 0))
	DrawDefaultButtonShadows (new_w);
    }

  return flag;
}

/**************************************************************************
 *
 * Resize(w)
 *
 **************************************************************************/

static void
Resize(
        Widget w )
{
  register XmPushButtonWidget tb = (XmPushButtonWidget) w;

  if (Lab_IsPixmap(w) || Lab_IsPixmapAndText(w))
    SetPushButtonSize((XmPushButtonWidget) tb);
  else {
    XtWidgetProc    resize;
    _XmProcessLock();
    resize = xmLabelClassRec.core_class.resize;
    _XmProcessUnlock();
    (* resize)((Widget) tb);
  }
}

/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void
Destroy(
        Widget w )
{
  XmPushButtonWidget pb = (XmPushButtonWidget) w ;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if (pb->pushbutton.timer)
    XtRemoveTimeOut (pb->pushbutton.timer);

  if (!Lab_IsMenupane(pb) || etched_in) {
    XtReleaseGC ((Widget) pb, pb->pushbutton.fill_gc);
    XtReleaseGC ((Widget) pb, pb->pushbutton.background_gc);
  }
}

/*************************************<->*************************************
 *
 *  Redisplay (pb, event, region)
 *   Completely rewritten to accommodate defaultButtonShadowThickness
 *   Description:
 *   -----------
 *     Cause the widget, identified by pb, to be redisplayed.
 *     If XmNfillOnArm is True and the pushbutton is not in a menu,
 *     the background will be filled with XmNarmColor.
 *     If XmNinvertOnArm is True and XmNLabelType is XmPIXMAP,
 *     XmNarmPixmap will be used in the label.
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;

  if (XtIsRealized((Widget)pb))
    {
      if (Lab_IsMenupane(pb))
	{
	  DrawPushButtonLabel (pb, event, region);

	  /* CR 5991:  Refresh border highlight too. */
	  if (pb->pushbutton.armed)
	    (*(((XmPushButtonWidgetClass) XtClass (pb))
	       ->primitive_class.border_highlight)) ((Widget) pb) ;
	}
      else
	{
	  DrawPushButtonBackground (pb);
	  DrawPushButtonLabel (pb, event, region);
	  DrawPushButtonShadows (pb);
	}
    }
}

/*
 * DrawPushButtonBackground ()
 *  - Compute the area allocated to the pushbutton and fill it with
 *    the background or the fill gc;
 */

static void
DrawPushButtonBackground(
        XmPushButtonWidget pb )
{
  XRectangle box;
  GC  tmp_gc;

  ComputePBLabelArea (pb, &box);

  if ((pb->pushbutton.armed) && (pb->pushbutton.fill_on_arm))
    tmp_gc = pb->pushbutton.fill_gc;
  else
    tmp_gc = pb->pushbutton.background_gc;
  /* really need to fill with background if not armed ? */

  if (tmp_gc)
    XFillRectangle (XtDisplay(pb), XtWindow(pb), tmp_gc,
		    box.x, box.y, box.width, box.height);
}

/*
 * DrawPushButtonLabel (pb, event, region)
 * Draw the label contained in the pushbutton.
 */

static void
DrawPushButtonLabel(
        XmPushButtonWidget pb,
        XEvent *event,
        Region region )
{
  GC tmp_gc = NULL;
  Boolean replaceGC = False;
  Boolean replaceBg = False;
  Boolean deadjusted = False;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if (pb->pushbutton.armed &&
      ((! Lab_IsMenupane(pb) && pb->pushbutton.fill_on_arm) ||
       (Lab_IsMenupane(pb) && etched_in)))
    {
      XSetWindowBackground(XtDisplay(pb), XtWindow(pb), pb->pushbutton.arm_color);
      replaceBg = True;
      if ((pb->label.label_type == XmSTRING ||
           pb->label.label_type == XmPIXMAP_AND_STRING) &&
	  (pb->pushbutton.arm_color == pb->primitive.foreground))
	{
	  tmp_gc = pb->label.normal_GC;
	  pb->label.normal_GC = pb->pushbutton.background_gc;
	  replaceGC = True;
	}
    }

  if (pb->label.label_type == XmPIXMAP ||
     pb->label.label_type == XmPIXMAP_AND_STRING)
    {
	if (pb->pushbutton.armed)
	  {
	      if (pb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		  pb->label.pixmap = pb->pushbutton.arm_pixmap;
	      else
		  pb->label.pixmap = pb->pushbutton.unarm_pixmap;
	  }
	else   /* pushbutton is unarmed */
	    pb->label.pixmap = pb->pushbutton.unarm_pixmap;
    }

  /*
   * Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment")
   *           from the margin values, so we don't confuse Label.
   */
  if (pb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_MarginLeft(pb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginRight(pb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginTop(pb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(pb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;
	_XmProcessLock();
	expose = xmLabelClassRec.core_class.expose;
	_XmProcessUnlock();
	(* expose) ((Widget) pb, event, region) ;
    }

  if (deadjusted)
    {
	Lab_MarginLeft(pb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginRight(pb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginTop(pb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(pb) += Xm3D_ENHANCE_PIXEL;
    }

  if (replaceGC)
      pb->label.normal_GC = tmp_gc;
  if (replaceBg) {
      XSetWindowBackground(XtDisplay(pb), XtWindow(pb), XtBackground(pb));
      if (pb->core.background_pixmap != XmUNSPECIFIED_PIXMAP) {
          XSetWindowBackgroundPixmap(XtDisplay(pb), XtWindow(pb),
	      pb->core.background_pixmap);
      }
  }
}



/*
 * DrawPushButtonShadows()
 *  Note: PushButton has two types of shadows: primitive-shadow and
 *	default-button-shadow.
 *  If pushbutton is in a menu only primitive shadows are drawn.
 */

static void
DrawPushButtonShadows(
        XmPushButtonWidget pb )
{
  XRectangle box;
  XtEnum default_button_emphasis;
  int adjust;
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

  default_button_emphasis = xm_dpy->display.default_button_emphasis;

  switch (default_button_emphasis)
    {
    case XmEXTERNAL_HIGHLIGHT:
      adjust = (pb->primitive.highlight_thickness -
		(pb->pushbutton.default_button_shadow_thickness ?
		 Xm3D_ENHANCE_PIXEL : 0));
      break;

    case XmINTERNAL_HIGHLIGHT:
      adjust = 0;
      break;

    default:
      assert(FALSE);
      return;
    }

  /* Clear the area not occupied by label with parents background color. */
  /* Label will invoke BorderUnhighlight() on the highlight_thickness	 */
  /* area, which is redundant when XmEXTERNAL_HIGHLIGHT default button	 */
  /* shadow emphasis is used.						 */

  ComputePBLabelArea (pb, &box);
  if (box.x > adjust)
    {
      FillBorderWithParentColor(pb,
				box.x - adjust,
				adjust,
				adjust,
				pb->core.width - 2 * adjust,
				pb->core.height - 2 * adjust);

      switch (default_button_emphasis)
	{
	case XmINTERNAL_HIGHLIGHT:
	  /* The call above erases the border highlighting. */
	  if (pb->primitive.highlight_drawn)
	    (*(((XmPushButtonWidgetClass) XtClass (pb))
	       ->primitive_class.border_highlight)) ((Widget) pb) ;
	  break;

	default:
	  break;
	}
    }


  if (pb->pushbutton.default_button_shadow_thickness
#ifdef DEFAULT_GLYPH_PIXMAP
      || (_XmGetDefaultGlyphPixmap(XtScreen((Widget)pb), NULL, NULL) !=
	  XmUNSPECIFIED_PIXMAP)
#endif
      )
    {
      if (pb->pushbutton.show_as_default)
	DrawDefaultButtonShadows (pb);
    }

  if (pb->primitive.shadow_thickness)
    DrawPBPrimitiveShadows (pb);
}

/*ARGSUSED*/
static Boolean
ParentVisualChanged(Widget kid,
		    Widget cur_parent,	/* unused */
		    Widget new_parent,	/* unused */
		    Mask visual_flag)
{
  XmPushButtonWidget pb = (XmPushButtonWidget) kid ;

  /* CR 9333: The primitive Redraw procedure only redraws the */
  /*	highighlight area, but push buttons needs to redraw the */
  /*	default button shadow emphasis area too. */

  if (visual_flag & (VisualBackgroundPixel|VisualBackgroundPixmap))
    {
      XmPrimitiveClassRec* kid_class = (XmPrimitiveClassRec*) XtClass(kid);

      if (! XtIsRealized(kid)) return True;

      if (!pb->primitive.highlighted &&
	  kid_class->primitive_class.border_unhighlight)
	kid_class->primitive_class.border_unhighlight(kid);

      DrawPushButtonShadows(pb);
    }

  return False;
}

/*
 * ComputePBLabelArea()
 *	Compute the area allocated to the label of the pushbutton;
 * fill in the dimensions in the box.
 */

static void
ComputePBLabelArea(
        XmPushButtonWidget pb,
        XRectangle *box )
{
  int dx, adjust;
  short fill = 0;

  if ((pb->pushbutton.arm_color == pb->primitive.top_shadow_color) ||
      (pb->pushbutton.arm_color == pb->primitive.bottom_shadow_color))
    fill = 1;

  if (pb->pushbutton.compatible)
    adjust = pb->pushbutton.show_as_default;
  else
    adjust = pb->pushbutton.default_button_shadow_thickness;

  if (adjust > 0)
    {
      adjust = adjust + pb->primitive.shadow_thickness;
      adjust = (adjust << 1);
      dx = pb->primitive.highlight_thickness + adjust + fill;
    }
  else
    dx = (pb->primitive.highlight_thickness +
	  pb->primitive.shadow_thickness + fill);

  box->x = dx;
  box->y = dx;
  adjust = (dx << 1);
  box->width  = pb->core.width - adjust;
  box->height = pb->core.height - adjust;
}

/*
 * DrawPBPrimitiveShadow (pb)
 *   - Should be called only if PrimitiveShadowThickness > 0
 */

static void
DrawPBPrimitiveShadows(
        XmPushButtonWidget pb )
{
  GC top_gc, bottom_gc;
  int dx, adjust, shadow_thickness;

  if (pb->pushbutton.armed)
    {
      bottom_gc = pb->primitive.top_shadow_GC;
      top_gc = pb->primitive.bottom_shadow_GC;
    }
  else
    {
      bottom_gc = pb->primitive.bottom_shadow_GC;
      top_gc = pb->primitive.top_shadow_GC;
    }


  shadow_thickness = pb->primitive.shadow_thickness;
  /*
   * This might have to be modified.
   *  - this is where dependency on compatibility with 1.0
   *    and defaultButtonShadowThickness etc. will showup.
   *  NOTE: defaultButtonShadowThickness is not supported in
   *   RowColumn children.
   *  1. Compute (x,y,width,height) for the rectangle within which
   *	  the shadow is to be drawn.
   */

  if ((shadow_thickness > 0) && (top_gc) && (bottom_gc))
    {
      if (pb->pushbutton.compatible)
	adjust = pb->pushbutton.show_as_default;
      else
	adjust = pb->pushbutton.default_button_shadow_thickness;

      if (adjust > 0)
	{
	  adjust = (adjust << 1);
	  dx = (pb->primitive.highlight_thickness +
		adjust + pb->primitive.shadow_thickness);
	}
      else
	dx = pb->primitive.highlight_thickness ;

      /* CR 7115: Deal with degenerate cases. */
      if ((pb->core.width > 2 * dx) &&
	  (pb->core.height > 2 * dx))
	XmeDrawShadows (XtDisplay (pb), XtWindow (pb), top_gc, bottom_gc,
			dx, dx,
			pb->core.width - 2 * dx,
			pb->core.height - 2 * dx,
			shadow_thickness, XmSHADOW_OUT);
    }
}

/*
 * DrawDefaultButtonShadows()
 *  - get the topShadowColor and bottomShadowColor from the parent;
 *    use those colors to construct top and bottom gc; use these
 *	  GCs to draw the shadows of the button.
 *  - Should not be called if pushbutton is in a row column or in a menu.
 *  - Should be called only if a defaultbuttonshadow is to be drawn.
 */

static void
DrawDefaultButtonShadows(
        XmPushButtonWidget pb )
{
  GC	 top_gc, bottom_gc;
  int	 default_button_shadow_thickness;
  int	 x, y, width, height, delta;
  Widget parent;
  XtEnum default_button_emphasis;
  XmDisplay xm_dpy;

  if (pb->pushbutton.compatible &&
      (pb->pushbutton.show_as_default == 0))
    return;

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
      XmUNSPECIFIED_PIXMAP)
    {
      DrawDefaultGlyphPixmap( pb) ;
      return ;
    }
#endif

  if (!pb->pushbutton.compatible &&
      (pb->pushbutton.default_button_shadow_thickness == 0))
    return ;

  /*
   * May need more complex computation for getting the GCs.
   */
  parent = XtParent(pb);
  if (XmIsManager(parent))
    {
      /* CR 5894:  Use the parent's GC so monochrome works. */
      bottom_gc = XmParentTopShadowGC(pb);
      top_gc = XmParentBottomShadowGC(pb);
    }
  else
    {
      /* Use your own pixel for drawing. */
      bottom_gc = pb->primitive.top_shadow_GC;
      top_gc = pb->primitive.bottom_shadow_GC;
    }

  if ((bottom_gc == None) || (top_gc == None))
    return;


  if (pb->pushbutton.compatible)
    default_button_shadow_thickness = pb->pushbutton.show_as_default;
  else
    default_button_shadow_thickness =
      pb->pushbutton.default_button_shadow_thickness;


  /*
   * Compute location of bounding box to contain the defaultButtonShadow.
   */
  xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
  default_button_emphasis = xm_dpy->display.default_button_emphasis;

  switch (default_button_emphasis)
    {
    case XmEXTERNAL_HIGHLIGHT:
      delta = pb->primitive.highlight_thickness;
      break;

    case XmINTERNAL_HIGHLIGHT:
      delta = Xm3D_ENHANCE_PIXEL;
      break;

    default:
      assert(FALSE);
      return;
    }

  x = y = delta;
  width = pb->core.width - 2 * delta;
  height = pb->core.height - 2 * delta;

  if ((width > 0) && (height > 0))
    XmeDrawShadows(XtDisplay(pb), XtWindow(pb),
		   top_gc, bottom_gc, x, y, width, height,
		   default_button_shadow_thickness, XmSHADOW_OUT);
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
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XEvent * event = NULL;

  if (Lab_IsMenupane(pb))
    {
       XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
       Boolean etched_in = dpy->display.enable_etched_in_menu;
       Boolean already_armed = pb->pushbutton.armed;

       pb->pushbutton.armed = TRUE;

       if (etched_in && !XmIsTearOffButton(pb) )
	 {
	     XFillRectangle (XtDisplay(pb), XtWindow(pb),
			     pb->pushbutton.fill_gc,
			     0, 0, pb->core.width, pb->core.height);

	     DrawPushButtonLabel (pb, event, NULL);
	 }

       if ((pb->core.width > 2 * pb->primitive.highlight_thickness) &&
	   (pb->core.height > 2 * pb->primitive.highlight_thickness))
	   XmeDrawShadows(XtDisplay (pb), XtWindow (pb),
		       pb->primitive.top_shadow_GC,
		       pb->primitive.bottom_shadow_GC,
		       pb->primitive.highlight_thickness,
		       pb->primitive.highlight_thickness,
		       pb->core.width - 2 * pb->primitive.highlight_thickness,
		       pb->core.height - 2 * pb->primitive.highlight_thickness,
		       pb->primitive.shadow_thickness,
		       etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

       if (!already_armed && pb->pushbutton.arm_callback)
	 {
	     XFlush (XtDisplay (pb));

	     call_value.reason = XmCR_ARM;
	     call_value.event = event;
	     XtCallCallbackList ((Widget) pb, pb->pushbutton.arm_callback,
				 &call_value);
	 }
   }
  else
    {
      DrawBorderHighlight ((Widget) pb) ;
    }
}

static void
DrawBorderHighlight(
        Widget wid)
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  int x, y, width, height, delta;
  Dimension highlight_width ;
  XtEnum default_button_emphasis;

  if (!XtWidth (pb) || !XtHeight (pb))
    return;

  pb->primitive.highlighted = True;
  pb->primitive.highlight_drawn = True;

  if (pb->pushbutton.default_button_shadow_thickness)
    highlight_width = pb->primitive.highlight_thickness - Xm3D_ENHANCE_PIXEL;
  else
    highlight_width = pb->primitive.highlight_thickness;

  if (highlight_width > 0)
    {
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

      default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmEXTERNAL_HIGHLIGHT:
	  delta = 0;
	  break;

	case XmINTERNAL_HIGHLIGHT:
	  if (pb->pushbutton.default_button_shadow_thickness)
	    delta = Xm3D_ENHANCE_PIXEL +
	      2 * (pb->pushbutton.compatible ?
		   pb->pushbutton.show_as_default :
		   pb->pushbutton.default_button_shadow_thickness);
	  else
	    delta = 0;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      x = y = delta;
      width = pb->core.width - 2 * delta;
      height = pb->core.height - 2 * delta;

      XmeDrawHighlight(XtDisplay(pb), XtWindow(pb), pb->primitive.highlight_GC,
		       x, y, width, height, highlight_width);
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
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XEvent * event = NULL;

  if (Lab_IsMenupane(pb))
    {
     XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
     Boolean etched_in = dpy->display.enable_etched_in_menu;
     Boolean already_armed = pb->pushbutton.armed;

     pb->pushbutton.armed = FALSE;

     if (etched_in && !XmIsTearOffButton(pb) ) {
	  XFillRectangle (XtDisplay(pb), XtWindow(pb),
			  pb->pushbutton.background_gc,
			  0, 0, pb->core.width, pb->core.height);
	  DrawPushButtonLabel (pb, event, NULL);
     }
      else
	  XmeClearBorder (
		  XtDisplay (pb), XtWindow (pb),
		  pb->primitive.highlight_thickness,
		  pb->primitive.highlight_thickness,
		  pb->core.width - 2 * pb->primitive.highlight_thickness,
		  pb->core.height - 2 * pb->primitive.highlight_thickness,
		  pb->primitive.shadow_thickness);


      if (already_armed && pb->pushbutton.disarm_callback)
	{
	  XFlush (XtDisplay (pb));

	  call_value.reason = XmCR_DISARM;
	  call_value.event = event;
	  XtCallCallbackList ((Widget) pb, pb->pushbutton.disarm_callback,
			      &call_value);
	}
    }
  else
    {
      /* PushButton is not in a menu - parent may be a shell or manager */
      int border = pb->primitive.highlight_thickness - Xm3D_ENHANCE_PIXEL;
      XtEnum default_button_emphasis;
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

      default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmINTERNAL_HIGHLIGHT:
	  if (pb->pushbutton.default_button_shadow_thickness && (border > 0))
	    {
	      int x, y, width, height, delta;

	      pb->primitive.highlighted = False;
	      pb->primitive.highlight_drawn = False;

	      delta = Xm3D_ENHANCE_PIXEL +
		2 * (pb->pushbutton.compatible ?
		     pb->pushbutton.show_as_default :
		     pb->pushbutton.default_button_shadow_thickness);

	      x = y = delta;
	      width = pb->core.width - 2 * delta;
	      height = pb->core.height - 2 * delta;

	      FillBorderWithParentColor(pb, border, x, y, width, height);
	      break;
	    }
	  /* else fall through to XmEXTERNAL_HIGHLIGHT. */

	case XmEXTERNAL_HIGHLIGHT:
	  (*(xmLabelClassRec.primitive_class.border_unhighlight)) (wid) ;
	  break;

	default:
	  assert(FALSE);
	  return;
	}
    }
}

/*
 * AdjustHighLightThickness ()
 *  HighlightThickness has a dependency on default_button-shadow-thickness;
 *  This routine (called from SetValues) adjust for that dependency.
 *  Applications should be aware that
 *  if a pushbutton gadget has  with (default_button-shadow-thickness == 0)
 * - then if through a XtSetValue it sets (default_button-shadow-thickness > 0)
 *  the application-specified highlight-thickness is internally increased by
 *  Xm3D_ENHANCE_PIXEL to enhance the 3D-appearance of the defaultButton
 *  Shadow. Similarly if a pushbutton gadget has  (default_button-shadow_
 *  thickness > 0), and it resets the (default_button-shadow-thickness = 0)
 *  through a XtSetValue , then the existing highlight-thickness is decreased
 *  by Xm3D_ENHANCE_PIXEL.
 *  The border-highlight when drawn is however is always of the same
 *  thickness as specified by the application since compensation is done
 *  in the drawing routine (see BorderHighlight).
 */

static int
AdjustHighLightThickness(
        XmPushButtonWidget new_w,
        XmPushButtonWidget current )
{
  int adjustment = 0;


  if (new_w->pushbutton.default_button_shadow_thickness > 0)
    {
      if (current->pushbutton.default_button_shadow_thickness == 0)
	{
	  new_w->primitive.highlight_thickness += Xm3D_ENHANCE_PIXEL;
	  adjustment = Xm3D_ENHANCE_PIXEL;
	}
      else if (new_w->primitive.highlight_thickness !=
	       current->primitive.highlight_thickness)
	{
	  new_w->primitive.highlight_thickness += Xm3D_ENHANCE_PIXEL;
	  adjustment = Xm3D_ENHANCE_PIXEL;
	}
    }
  else
    {
      if (current->pushbutton.default_button_shadow_thickness > 0)
        /* default_button_shadow_thickness was > 0 and is now being set to 0,
         * so take away the adjustment for enhancement.
         */
	{
	  if (new_w->primitive.highlight_thickness ==
	      current->primitive.highlight_thickness)
	    {
	      new_w->primitive.highlight_thickness -= Xm3D_ENHANCE_PIXEL;
	      adjustment -= Xm3D_ENHANCE_PIXEL;
	    }
	  /*
	   * This will appear to be a bug if in a XtSetValues the application
	   * removes the default_button_shadow_thickness and also
	   * sets the high-light-thickness to a value of
	   * (old-high-light-thickness (from previous XtSetValue) +
	   *  Xm3D_ENHANCE_PIXEL) at the same time.
	   * This will be documented.
	   */
	}
    }

  return (adjustment);
}

static void
FillBorderWithParentColor(
        XmPushButtonWidget pb,
        int borderwidth,
        int dx,
        int dy,
        int rectwidth,
        int rectheight )
{
  if (XmIsManager(XtParent(pb)))
    {
      /* CR 5551: Use the manager gc rather than creating a new one. */
      XmeDrawHighlight(XtDisplay(pb), XtWindow(pb), XmParentBackgroundGC(pb),
		       dx, dy, rectwidth, rectheight, borderwidth);
    }
  else
    {
      /* CR 6038: This is wrong, but is the way Label (by calling
       *	Primitive.c:UnhighlightBorder) clears borders in shells.
       */
      XmeClearBorder(XtDisplay(pb), XtWindow(pb),
		     dx, dy, rectwidth, rectheight, borderwidth);
    }
}

/*************************************************************************
 *
 * SetPushButtonSize(newpb)
 *  Picks the larger dimension when the armPixmap is a
 *  different size than the label pixmap(i.e the unarm pixmap).
 *
 ************************************************************************/

static void
SetPushButtonSize(
     XmPushButtonWidget newpb)
{
  XmLabelPart *lp = &(newpb->label);
  unsigned int onW = 0, onH = 0, onW2 = 0, onH2 = 0;
  XtWidgetProc resize;

  if (newpb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
    {
      if (newpb->label.pixmap != XmUNSPECIFIED_PIXMAP)
      XmeGetPixmapData(XtScreen(newpb), newpb->label.pixmap,
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW, &onH);
      XmeGetPixmapData(XtScreen(newpb), newpb->pushbutton.arm_pixmap,
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW2, &onH2);
      newpb->label.PixmapRect.width = MAX(onW2, onW);
      newpb->label.PixmapRect.height = MAX(onH2, onH);
      _XmLabelCalcTextRect((Widget)newpb);
    }


  /* Let Label do the rest. */
  _XmProcessLock();
  resize = xmLabelClassRec.core_class.resize;
  _XmProcessUnlock();
  (* resize) ((Widget) newpb);
}

/************************************************************************
 *
 *    Actions -----------
 *
 ************************************************************************/

/************************************************************************
 *
 *     Arm
 *
 *     This function processes button 1 down occuring on the pushbutton.
 *     Mark the pushbutton as armed (i.e. active).
 *     The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
Arm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XtExposeProc expose;

  (void) XmProcessTraversal ((Widget) pb, XmTRAVERSE_CURRENT);

  pb->pushbutton.armed = TRUE;

  if (event != NULL &&
      (event->xany.type == ButtonPress || event->xany.type == ButtonRelease))
    pb->pushbutton.armTimeStamp = event->xbutton.time;
  else
    pb->pushbutton.armTimeStamp = 0;

  _XmProcessLock();
  expose = XtClass(pb)->core_class.expose;
  _XmProcessUnlock();
  (* expose)(wid, event, (Region) NULL);

  if (pb->pushbutton.arm_callback)
    {
      XFlush(XtDisplay (pb));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget) pb, pb->pushbutton.arm_callback, &call_value);
    }
}

/*ARGSUSED*/
static void
MultiArm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;

  if (pb->pushbutton.multiClick == XmMULTICLICK_KEEP)
    Arm ((Widget) pb, event, NULL, NULL);
}

/************************************************************************
 *
 *     Activate
 *
 *     Mark the pushbutton as unarmed (i.e. inactive).
 *     If the button release occurs inside of the PushButton, the
 *     callbacks for XmNactivateCallback are called.
 *
 ************************************************************************/
static void
Activate(
        Widget wid,
        XEvent *buttonEvent,
        String *params,
        Cardinal *num_params )
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;

  if (pb->pushbutton.armed == FALSE)
    return;

  pb->pushbutton.click_count = 1;
  ActivateCommon ((Widget) pb, buttonEvent, params, num_params);
}

static void
MultiActivate(
        Widget wid,
        XEvent *buttonEvent,
        String *params,
        Cardinal *num_params )
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;

  /* When a multi click sequence occurs and the user Button Presses and
   * holds for a length of time, the final release should look like a
   * new_w/separate activate.
   */
  if (pb->pushbutton.multiClick == XmMULTICLICK_KEEP)
    {
      if ((buttonEvent->xbutton.time - pb->pushbutton.armTimeStamp) >
	  XtGetMultiClickTime(XtDisplay(pb)))
	pb->pushbutton.click_count = 1;
      else
        pb->pushbutton.click_count++;

      ActivateCommon ((Widget) pb, buttonEvent, params, num_params);
      Disarm ((Widget) pb, buttonEvent, params, num_params);
    }
}

/*ARGSUSED*/
static void
ActivateCommon(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;
  XtExposeProc expose;

  pb->pushbutton.armed = FALSE;

  _XmProcessLock();
  expose = ((WidgetClass)XtClass(pb))->core_class.expose;
  _XmProcessUnlock();

  (* expose)(wid, event, (Region) NULL);

  /* CR 9181: Consider clipping when testing visibility. */
  if ((event->xany.type == ButtonPress || event->xany.type == ButtonRelease) &&
      _XmGetPointVisibility(wid, event->xbutton.x_root, event->xbutton.y_root))
    {
      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;
      call_value.click_count = pb->pushbutton.click_count;

      if ((pb->pushbutton.multiClick == XmMULTICLICK_DISCARD) &&
	  (call_value.click_count > 1))
	return;

      menuSTrait = (XmMenuSystemTrait)
	XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);

      /* if the parent is menu system able, notify it about the select */
      if (menuSTrait != NULL)
	menuSTrait->entryCallback(XtParent(pb), (Widget) pb, &call_value);

      if ((! pb->label.skipCallback) &&
	  (pb->pushbutton.activate_callback))
	{
	  XFlush (XtDisplay (pb));
	  XtCallCallbackList ((Widget) pb, pb->pushbutton.activate_callback,
			      &call_value);
	}
    }
}



static void
PB_FixTearoff( XmPushButtonWidget pb)
{
	 if  (XmMENU_PULLDOWN == pb->label.menu_type)
	 {
		Widget mwid = XmGetPostedFromWidget(XtParent(pb));
		if (mwid && XmIsRowColumn(mwid)
			&& (XmMENU_OPTION == RC_Type(mwid))
			&& _XmIsActiveTearOff(XtParent(pb)))
			XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);
	 }
}
/************************************************************************
 *
 *     ArmAndActivate
 *
 ************************************************************************/

/*ARGSUSED*/
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  Boolean already_armed = pb->pushbutton.armed;
  XmPushButtonCallbackStruct call_value;
  Boolean is_menupane = Lab_IsMenupane(pb);
  Boolean torn_has_focus = FALSE;	/* must be torn! */
  XmMenuSystemTrait menuSTrait;

  if (is_menupane && !XmIsMenuShell(XtParent(XtParent(pb))))
    {
      /* Because the pane is torn and the parent is a transient shell,
       * the shell's focal point from _XmGetFocusData should be valid
       * (as opposed to getting it from a MenuShell).
       */
      if (XmeFocusIsInShell((Widget)pb))
	{
	  /* In case allowAcceleratedInsensitiveUnmanagedMenuItems is True */
	  if (!XtIsSensitive((Widget)pb) || (!XtIsManaged((Widget)pb)))
            return;
	  torn_has_focus = TRUE;
	}
    }

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass((Widget)XtParent(pb)), XmQTmenuSystem);
  if (is_menupane && menuSTrait != NULL)
    {
      pb->pushbutton.armed = FALSE;

      /* CR 7799: Torn off menus shouldn't be shared, so don't reparent. */
      if (torn_has_focus)
	menuSTrait->popdown(XtParent(pb), event);
      else
	menuSTrait->buttonPopdown(XtParent(pb), event);

      /* if its in a torn off menu pane, show depressed button briefly */
      if (torn_has_focus)
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
	  Boolean etched_in = dpy->display.enable_etched_in_menu;

	  /* Set the focus here. */
	  XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);

	  if ((pb->core.width > 2 * pb->primitive.highlight_thickness) &&
	      (pb->core.height > 2 * pb->primitive.highlight_thickness))
	    XmeDrawShadows
	      (XtDisplay (pb), XtWindow (pb),
	       pb->primitive.bottom_shadow_GC, pb->primitive.top_shadow_GC,
	       pb->primitive.highlight_thickness,
	       pb->primitive.highlight_thickness,
	       pb->core.width - 2 * pb->primitive.highlight_thickness,
	       pb->core.height - 2 * pb->primitive.highlight_thickness,
	       pb->primitive.shadow_thickness,
	       etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
	}
    }
  else
    {
      XtExposeProc expose;
      pb->pushbutton.armed = TRUE;
      _XmProcessLock();
      expose = XtClass(pb)->core_class.expose;
      _XmProcessUnlock();
      (* expose)(wid, event, (Region) NULL);
    }

  XFlush (XtDisplay (pb));

  /* If the parent is menu system able, set the lastSelectToplevel before
   * the arm. It's ok if this is recalled later.
   */
  if (menuSTrait != NULL)
    menuSTrait->getLastSelectToplevel(XtParent(pb));

  if (pb->pushbutton.arm_callback && !already_armed)
    {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget)pb, pb->pushbutton.arm_callback, &call_value);
    }

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = 1;	           /* always 1 in kselect */

  /* if the parent is menu system able, notify it about the select */
  if (menuSTrait != NULL)
    menuSTrait->entryCallback(XtParent(pb), (Widget)pb, &call_value);
	pb->label.pixmap = pb->pushbutton.unarm_pixmap;
  if ((! pb->label.skipCallback) && (pb->pushbutton.activate_callback))
    {
      XFlush (XtDisplay (pb));
      XtCallCallbackList ((Widget) pb, pb->pushbutton.activate_callback,
			  &call_value);
    }

  pb->pushbutton.armed = FALSE;

  if (pb->pushbutton.disarm_callback)
    {
      XFlush (XtDisplay (pb));
      call_value.reason = XmCR_DISARM;
      XtCallCallbackList ((Widget) pb, pb->pushbutton.disarm_callback,
			  &call_value);
    }

  if (is_menupane && menuSTrait != NULL)
    {
      if (torn_has_focus && XtIsSensitive(wid))
	{
	  /* Leave the focus widget in an armed state */
	  pb->pushbutton.armed = TRUE;

	  if (pb->pushbutton.arm_callback)
	    {
	      XFlush (XtDisplay (pb));
	      call_value.reason = XmCR_ARM;
	      XtCallCallbackList ((Widget) pb, pb->pushbutton.arm_callback,
				  &call_value);
	    }
	}
      else
	{
	  menuSTrait->reparentToTearOffShell(XtParent(pb), event);
	  PB_FixTearoff(pb);
	}
    }

  /*
   * If the button is still around, show it released, after a short delay.
   * This is done if the button is outside of a menus, or if in a torn
   * off menupane.
   */

  if (!is_menupane || torn_has_focus)
    {
      if ((pb->core.being_destroyed == False) && (!pb->pushbutton.timer))
        pb->pushbutton.timer =
	  XtAppAddTimeOut (XtWidgetToApplicationContext((Widget)pb),
			   (unsigned long) DELAY_DEFAULT,
			   ArmTimeout,
			   (XtPointer)(pb));
    }
}

/*ARGSUSED*/
static void
ArmTimeout(
        XtPointer data,
        XtIntervalId *id )
{
  XmPushButtonWidget pb = (XmPushButtonWidget) data;

  pb->pushbutton.timer = 0;
  if (XtIsRealized ((Widget)pb) && XtIsManaged ((Widget)pb))
    {
      if (Lab_IsMenupane(pb))
	{
	  /* When rapidly clicking, the focus may have moved away from this
	   * widget, so check before changing the shadow.
	   */
	  if (XmeFocusIsInShell((Widget)pb) &&
	      (XmGetFocusWidget((Widget)pb) == (Widget)pb))
	    {
	      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
	      Boolean etched_in = dpy->display.enable_etched_in_menu;

	      /* in a torn off menu, redraw shadows */
	      if ((pb->core.width > 2 * pb->primitive.highlight_thickness) &&
		  (pb->core.height > 2 * pb->primitive.highlight_thickness))
		XmeDrawShadows
		  (XtDisplay (pb), XtWindow (pb),
		   pb->primitive.top_shadow_GC,
		   pb->primitive.bottom_shadow_GC,
		   pb->primitive.highlight_thickness,
		   pb->primitive.highlight_thickness,
		   pb->core.width - 2 * pb->primitive.highlight_thickness,
		   pb->core.height - 2 * pb->primitive.highlight_thickness,
		   pb->primitive.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
	    }
	}
      else
	{
	  XtExposeProc expose;
	  _XmProcessLock();
	  expose = XtClass(pb)->core_class.expose;
	  _XmProcessUnlock();
	  (* expose) ((Widget) pb, NULL, (Region) NULL);
	}

      XFlush (XtDisplay (pb));
    }
}

/************************************************************************
 *
 *    Disarm
 *
 *     Mark the pushbutton as unarmed (i.e. active).
 *     The callbacks for XmNdisarmCallback are called..
 *
 ************************************************************************/

/*ARGSUSED*/
static void
Disarm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XtExposeProc expose;

  /* BEGIN OSF Fix pir 2826 */
  if (pb->pushbutton.armed == TRUE)
    {
      pb->pushbutton.armed = FALSE;
      Redisplay((Widget) pb, event, (Region)NULL);
      _XmProcessLock();
      expose = XtClass(pb)->core_class.expose;
      _XmProcessUnlock();
      if (expose)
        (* expose)((Widget)(pb), event, (Region)NULL);

    }
  /* END OSF Fix pir 2826 */

  if (pb->pushbutton.disarm_callback)
    {
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, pb->pushbutton.disarm_callback,
			  &call_value);
    }
}

/************************************************************************
 *
 *     BtnDown
 *
 *     This function processes a button down occuring on the pushbutton
 *     when it is in a popup, pulldown, or option menu.
 *     Popdown the posted menu.
 *     Turn parent's traversal off.
 *     Mark the pushbutton as armed (i.e. active).
 *     The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
BtnDown(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  Boolean validButton = False;
  Boolean already_armed;
  ShellWidget popup;
  XmMenuSystemTrait menuSTrait;

  /* Support menu replay, free server input queue until next button event */
  XAllowEvents(XtDisplay(pb), SyncPointer, CurrentTime);

  /* If no menu system trait then parent isn't a menu as it should be. */
  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);
  if (menuSTrait == NULL)
    return;

  if (event && (event->type == ButtonPress))
    validButton = menuSTrait->verifyButton(XtParent(pb), event);

  if (!validButton)
    return;

  _XmSetInDragMode((Widget)pb, True);

  /* Popdown other popus that may be up */
  if (!(popup = (ShellWidget)_XmGetRC_PopupPosted(XtParent(pb))))
    {
      if (!XmIsMenuShell(XtParent(XtParent(pb))))
	{
	  /* In case tear off not armed and no grabs in place, do it now.
	   * Ok if already armed and grabbed - nothing done.
	   */
	  menuSTrait->tearOffArm(XtParent(pb));
	}
    }

  if (popup)
    {
      if (popup->shell.popped_up)
	menuSTrait->popdownEveryone((Widget) popup, event);
    }

  /* Set focus to this pushbutton.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal ((Widget) pb, XmTRAVERSE_CURRENT);
  /* get the location cursor - get consistent with Gadgets */

  already_armed = pb->pushbutton.armed;
  pb->pushbutton.armed = TRUE;

  if (pb->pushbutton.arm_callback && !already_armed)
    {
      XFlush (XtDisplay (pb));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget) pb, pb->pushbutton.arm_callback, &call_value);
    }
  _XmRecordEvent (event);
}

/************************************************************************
 *
 *     BtnUp
 *
 *     This function processes a button up occuring on the pushbutton
 *     when it is in a popup, pulldown, or option menu.
 *     Mark the pushbutton as unarmed (i.e. inactive).
 *     The callbacks for XmNactivateCallback are called.
 *     The callbacks for XmNdisarmCallback are called.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
BtnUp(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  Widget parent =  XtParent(pb);
  XmPushButtonCallbackStruct call_value;
  Boolean flushDone = False;
  Boolean validButton = False;
  Boolean popped_up;
  Boolean is_menupane = Lab_IsMenupane(pb);
  Widget shell = XtParent(XtParent(pb));
  XmMenuSystemTrait menuSTrait;

  /* If no menu system trait then parent isn't a menu as it should be. */
  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);
  if (menuSTrait == NULL)
    return;

  if (event && (event->type == ButtonRelease))
    validButton = menuSTrait->verifyButton(parent, event);

  if (!validButton || (pb->pushbutton.armed == FALSE))
    return;

  pb->pushbutton.armed = FALSE;

  if (is_menupane && !XmIsMenuShell(shell))
    popped_up = menuSTrait->popdown((Widget) pb, event);
  else
    popped_up = menuSTrait->buttonPopdown((Widget) pb, event);

  _XmRecordEvent(event);

  /* XmMENU_POPDOWN left the menu posted on button click - don't activate! */
  if (popped_up)
    return;

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = 1;

  /* if the parent is menu system able, notify it about the select */
  if (menuSTrait != NULL)
    {
      menuSTrait->entryCallback(parent, (Widget) pb, &call_value);
      flushDone = True;
    }

  if ((! pb->label.skipCallback) &&
      (pb->pushbutton.activate_callback))
    {
      XFlush (XtDisplay (pb));
      flushDone = True;
      XtCallCallbackList ((Widget) pb, pb->pushbutton.activate_callback,
			  &call_value);
    }
  if (pb->pushbutton.disarm_callback)
    {
      if (!flushDone)
	XFlush (XtDisplay (pb));
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, pb->pushbutton.disarm_callback,
			  &call_value);
    }

  /* If the original shell does not indicate an active menu, but rather a
   * tear off pane, leave the button in an armed state.  Also, briefly
   * display the button as depressed to give the user some feedback of
   * the selection.
   */

  if (is_menupane) /* necessary check? */
    {
      if (!XmIsMenuShell(shell))
	{
	  if (XtIsSensitive((Widget)pb))
	    {
	      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
	      Boolean etched_in = dpy->display.enable_etched_in_menu;

	      if ((pb->core.width > 2 * pb->primitive.highlight_thickness) &&
		  (pb->core.height > 2 * pb->primitive.highlight_thickness))
		XmeDrawShadows
		  (XtDisplay (pb), XtWindow (pb),
		   pb->primitive.bottom_shadow_GC,
		   pb->primitive.top_shadow_GC,
		   pb->primitive.highlight_thickness,
		   pb->primitive.highlight_thickness,
		   pb->core.width - 2 * pb->primitive.highlight_thickness,
		   pb->core.height - 2 * pb->primitive.highlight_thickness,
		   pb->primitive.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	      XFlush (XtDisplay (pb));

	      if (pb->core.being_destroyed == False)
		{
		  if (!pb->pushbutton.timer)
		    pb->pushbutton.timer =
		      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)pb),
				      (unsigned long) DELAY_DEFAULT,
				      ArmTimeout,
				      (XtPointer)(pb));
		}

	      pb->pushbutton.armed = TRUE;
	      if (pb->pushbutton.arm_callback)
		{
		  call_value.reason = XmCR_ARM;
		  call_value.event = event;
		  XtCallCallbackList ((Widget) pb, pb->pushbutton.arm_callback,
				      &call_value);
		}
	    }
	}
      else
	menuSTrait->reparentToTearOffShell(XtParent(pb), event);
    }

  _XmSetInDragMode((Widget)pb, False);

  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  if (!XmIsMenuShell(shell))
    XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);
  PB_FixTearoff(pb);
}

/************************************************************************
 *
 *  Enter
 *
 ************************************************************************/

/*ARGSUSED*/
static void
Enter(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;

  if (Lab_IsMenupane(pb))
    {
      if ((((ShellWidget) XtParent(XtParent(pb)))->shell.popped_up) &&
          _XmGetInDragMode((Widget)pb))
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	  Boolean etched_in = dpy->display.enable_etched_in_menu;

	  if (pb->pushbutton.armed)
	    return;

	  /* So KHelp event is delivered correctly */
	  _XmSetFocusFlag (XtParent(XtParent(pb)), XmFOCUS_IGNORE, TRUE);
	  XtSetKeyboardFocus(XtParent(XtParent(pb)), (Widget)pb);
	  _XmSetFocusFlag (XtParent(XtParent(pb)), XmFOCUS_IGNORE, FALSE);

	  pb -> pushbutton.armed = TRUE;

	  ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	  /* etched in menu button */
	  if (etched_in && !XmIsTearOffButton(pb) ) {

	      XFillRectangle (XtDisplay(pb), XtWindow(pb),
			      pb->pushbutton.fill_gc,
			      0, 0, pb->core.width, pb->core.height);
	      DrawPushButtonLabel( pb, event, NULL );
	  }

	  if ((pb->core.width > 2 * pb->primitive.highlight_thickness) &&
	      (pb->core.height > 2 * pb->primitive.highlight_thickness))
	    XmeDrawShadows
	      (XtDisplay (pb), XtWindow (pb),
	       pb->primitive.top_shadow_GC,
	       pb->primitive.bottom_shadow_GC,
	       pb->primitive.highlight_thickness,
	       pb->primitive.highlight_thickness,
	       pb->core.width - 2 * pb->primitive.highlight_thickness,
	       pb->core.height - 2 * pb->primitive.highlight_thickness,
	       pb->primitive.shadow_thickness,
	       etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	  if (pb->pushbutton.arm_callback)
	    {
	      XFlush (XtDisplay (pb));

	      call_value.reason = XmCR_ARM;
	      call_value.event = event;
	      XtCallCallbackList ((Widget) pb,
				  pb->pushbutton.arm_callback, &call_value);
	    }
	}
    }
  else
    {
      XtExposeProc expose;
      _XmPrimitiveEnter ((Widget) pb, event, NULL, NULL);
      if (pb->pushbutton.armed == TRUE) {
	_XmProcessLock();
	expose = XtClass(pb)->core_class.expose;
	_XmProcessUnlock();
	(* expose)(wid, event, (Region) NULL);
      }
    }
}

/************************************************************************
 *
 *  Leave
 *
 ************************************************************************/

/*ARGSUSED*/
static void
Leave(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;

  if (Lab_IsMenupane(pb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      if (_XmGetInDragMode((Widget)pb) && pb->pushbutton.armed &&
          (/* !ActiveTearOff || */ event->xcrossing.mode == NotifyNormal))
	{
	  pb->pushbutton.armed = FALSE;

	  ((XmManagerWidget) XtParent(wid))->manager.active_child = NULL;

	  if (etched_in && !XmIsTearOffButton(pb) ) {
	      XFillRectangle (XtDisplay(pb), XtWindow(pb),
			      pb->pushbutton.background_gc,
			      0, 0, pb->core.width, pb->core.height);
	      DrawPushButtonLabel (pb, event, NULL);
	  }
	  else
	     XmeClearBorder
		 (XtDisplay (pb), XtWindow (pb),
		  pb->primitive.highlight_thickness,
		  pb->primitive.highlight_thickness,
		  pb->core.width - 2 * pb->primitive.highlight_thickness,
		  pb->core.height - 2 * pb->primitive.highlight_thickness,
		  pb->primitive.shadow_thickness);

	  if (pb->pushbutton.disarm_callback)
	    {
	      XFlush (XtDisplay (pb));

	      call_value.reason = XmCR_DISARM;
	      call_value.event = event;
	      XtCallCallbackList ((Widget) pb,
				  pb->pushbutton.disarm_callback, &call_value);
	    }
	}
    }
  else
    {
      _XmPrimitiveLeave ((Widget) pb, event, NULL, NULL);

      if (pb->pushbutton.armed == TRUE)
	{
	  XtExposeProc expose;
	  pb->pushbutton.armed = FALSE;
	  _XmProcessLock();
	  expose = XtClass(pb)->core_class.expose;
	  _XmProcessUnlock();
	  (* expose)(wid, event, (Region) NULL);
	  pb->pushbutton.armed = TRUE;
	}
    }
}

/*************************************<->*************************************
 *
 *  KeySelect
 *
 *  If the menu system traversal is enabled, do an activate and disarm
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static void
KeySelect(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  XmPushButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;

  if (!_XmIsEventUnique(event))
    return;

  if (!_XmGetInDragMode((Widget)pb))
    {
      menuSTrait = (XmMenuSystemTrait)
	XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);
      pb->pushbutton.armed = FALSE;

      if (menuSTrait != NULL)
	menuSTrait->buttonPopdown(XtParent(pb), event);

      _XmRecordEvent(event);

      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;

      /* if the parent is menu system able, notify it about the select */
      if (menuSTrait != NULL)
	menuSTrait->entryCallback(XtParent(pb), (Widget) pb, &call_value);

      if ((! pb->label.skipCallback) &&
	  (pb->pushbutton.activate_callback))
	{
	  XFlush (XtDisplay (pb));
	  XtCallCallbackList ((Widget) pb, pb->pushbutton.activate_callback,
			      &call_value);
	}

      if (menuSTrait != NULL)
	menuSTrait->reparentToTearOffShell(XtParent(pb), event);
    }
}

/************************************************************************
 *
 *  Help
 *     This function processes Function Key 1 press occuring on the PushButton.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
Help(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonWidget pb = (XmPushButtonWidget) wid ;
  Boolean is_menupane = Lab_IsMenupane(pb);
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->buttonPopdown(XtParent(pb), event);

  _XmPrimitiveHelp ((Widget) pb, event, NULL, NULL);

  /***
   * call_value.reason = XmCR_HELP;
   * call_value.event = event;
   * XtCallCallbackList ((Widget) pb, pb->primitive.help_callback, &call_value);
   ***/

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->reparentToTearOffShell(XtParent(pb), event);
}

/************************************************************************
 *
 *  Trait methods --------
 *
 ************************************************************************/

/************************************************************************
 *
 *  ChangeCB
 *	add or remove the activate callback list.
 *
 ************************************************************************/

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

/************************************************************************
 *
 *  ShowAsDefault
 *	set up the default visual
 *
 ************************************************************************/

static void
ShowAsDefault(Widget w,
	      XtEnum state)
{
  XmPushButtonWidget pb = (XmPushButtonWidget) w ;
  Dimension dbShadowTh ;

  switch (state)
    {
    case XmDEFAULT_READY:
      {
	/* We have pixels, but the button unit type might not be
	 * pixel, so save it and restore it after the setvalues */
	unsigned char saved_unit_type =
	  ((XmPrimitiveWidget)w)->primitive.unit_type ;

#ifdef DEFAULT_GLYPH_PIXMAP
	unsigned int def_pixmap_width ;
	if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)),
				     &def_pixmap_width, NULL) !=
	    XmUNSPECIFIED_PIXMAP)
	  {
	    /* We will use the margin right area, so increase it */
	    pb->pushbutton.compatible = False;
	    ((XmPrimitiveWidget)w)->primitive.unit_type = XmPIXELS;
	    XtVaSetValues(w, XmNmarginRight, def_pixmap_width, NULL);
	    ((XmPrimitiveWidget)w)->primitive.unit_type = saved_unit_type;
	  }
	else
#endif
	  if (!pb->pushbutton.default_button_shadow_thickness)
	    {
	      if (pb->primitive.shadow_thickness > 1)
		dbShadowTh = pb->primitive.shadow_thickness >> 1 ;
	      else
		dbShadowTh = pb->primitive.shadow_thickness ;

	      /* CR 7474: Disable pushbutton compatibility mode. */
	      pb->pushbutton.compatible = False;
	      ((XmPrimitiveWidget)w)->primitive.unit_type = XmPIXELS;
	      XtVaSetValues(w, XmNdefaultButtonShadowThickness,
			    dbShadowTh, NULL);
	      ((XmPrimitiveWidget)w)->primitive.unit_type = saved_unit_type;
	    }
      }
      break ;

    case XmDEFAULT_ON :
      /* CR 7474: Disable pushbutton compatibility mode. */
      pb->pushbutton.compatible = False;
      XtVaSetValues(w, XmNshowAsDefault, True, NULL);
      break ;

    case XmDEFAULT_OFF :
      XtVaSetValues(w, XmNshowAsDefault, False, NULL);
      break ;

    case XmDEFAULT_FORGET :
    default:
#ifdef DEFAULT_GLYPH_PIXMAP
      if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
	  XmUNSPECIFIED_PIXMAP)
	XtVaSetValues(w, XmNmarginRight, 0, NULL);
      else
#endif
    	if (!pb->pushbutton.default_button_shadow_thickness)
	  XtVaSetValues(w, XmNdefaultButtonShadowThickness, 0, NULL);
    }
}

/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/


/************************************************************************
 *
 *  XmCreatePushButton
 *	Create an instance of a pushbutton and return the widget id.
 *
 ************************************************************************/

Widget
XmCreatePushButton(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
  return XtCreateWidget(name, xmPushButtonWidgetClass, parent,
			arglist, argcount);
}

Widget
XmVaCreatePushButton(
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
                         xmPushButtonWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedPushButton(
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
                         xmPushButtonWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}
