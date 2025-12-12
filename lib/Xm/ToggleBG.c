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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ToggleBG.c /main/46 1999/12/06 18:10:15 samborn $"
#endif
#endif

#include <stdio.h>
#include "XmI.h"
#include <X11/ShellP.h>
#include <Xm/CareVisualT.h>
#include <Xm/CascadeB.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/ToggleBGP.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "ColorI.h"
#include "ExtObjectI.h"
#include "LabelGI.h"
#include "LabelI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "RepTypeI.h"
#include "SyntheticI.h"
#include "ToggleBGI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"


#define XmINVALID_TYPE		255	/* special flag for IndicatorType */
#define XmINVALID_BOOLEAN	 85	/* special flag for VisibleWhenOff */

#define MIN_GLYPH_SIZE		5	/* Threshold for rendering glyphs. */

#define PixmapOn(w)		(TBG_OnPixmap(w))
#define PixmapOff(w)		(LabG_Pixmap(w))
#define PixmapInd(w)		(TBG_IndeterminatePixmap(w))
#define PixmapInsenOn(w)	(TBG_InsenPixmap(w))
#define PixmapInsenOff(w)	(LabG_PixmapInsensitive(w))
#define PixmapInsenInd(w)	(TBG_IndeterminateInsensitivePixmap(w))
#define IsNull(p)		((p) == XmUNSPECIFIED_PIXMAP)
#define IsOn(w)			(TBG_VisualSet(w))

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
static void SecondaryObjectCreate(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
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
int _XmToggleBCacheCompare(
			   XtPointer A,
			   XtPointer B );
static void InputDispatch(
                        Widget wid,
                        XEvent *event,
                        Mask event_mask) ;
static void SetAndDisplayPixmap(
                        XmToggleButtonGadget w,
                        XEvent *event,
                        Region region) ;
static void Help(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void ToggleButtonCallback(
                        XmToggleButtonGadget data,
                        unsigned int reason,
                        unsigned int value,
                        XEvent *event) ;
static void Leave(
                        XmToggleButtonGadget w,
                        XEvent *event) ;
static void Enter(
                        XmToggleButtonGadget w,
                        XEvent *event) ;
static void Arm(
                        Widget w,
                        XEvent *event) ;
static void Select(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void Disarm(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void BtnDown(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void BtnUp(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void GetGC(
                        XmToggleButtonGadget tw) ;
static void GetUnselectGC(
			  XmToggleButtonGadget tw) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget w) ;
static void DrawToggle(
                        XmToggleButtonGadget w) ;
static void BorderHighlight(
                        Widget wid) ;
static void BorderUnhighlight(
                        Widget wid) ;
static void KeySelect(
                        XmToggleButtonGadget tb,
                        XEvent *event) ;
static void ComputeSpace(
                        XmToggleButtonGadget tb) ;
static void Redisplay(
                        Widget w,
                        XEvent *event,
                        Region region) ;
static void Resize(
                        Widget w) ;
static Boolean SetValuesPrehook(
                        Widget oldParent,
                        Widget refParent,
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPrehook(
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPosthook(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPosthook(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Cardinal GetToggleBGClassSecResData(
                        WidgetClass w_class,
                        XmSecondaryResourceData **data_rtn) ;
static XtPointer GetToggleBGClassSecResBase(
                        Widget widget,
                        XtPointer client_data) ;
static void DrawToggleLabel(
                        XmToggleButtonGadget tb) ;
static void DrawEtchedInMenu(
                        XmToggleButtonGadget tb) ;
static void DrawToggleShadow(
                        XmToggleButtonGadget tb) ;
static void SetToggleSize(
                        XmToggleButtonGadget newtbg) ;
static void NextState(unsigned char *state);
static void DrawBox(
		    XmToggleButtonGadget w,
		    GC top_gc,
		    GC bot_gc,
		    GC fillgc,
		    int x, int y, int edge,
		    Dimension margin);

static Boolean HandleRedraw (Widget kid,
		       Widget cur_parent,
		       Widget new_parent,
		       Mask visual_flag);

static Boolean ToggleBGCVTRedraw (Widget kid,
		       Widget cur_parent,
		       Widget new_parent,
		       Mask visual_flag);

static void DefaultSelectColor(Widget    widget,
			       int       offset,
			       XrmValue *value);
static void TBG_FixTearoff( XmToggleButtonGadget tb);

static unsigned char NormalizeIndOn(XmToggleButtonGadget tb);
static unsigned char NormalizeIndType(XmToggleButtonGadget tb);

/********    End Static Function Declarations    ********/

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

#define Offset(field)		XtOffsetOf(XmToggleButtonGadgetRec, field)
#define CacheOffset(field)	XtOffsetOf(XmToggleButtonGCacheObjRec, field)

static XtResource cache_resources[] =
{
  {
    XmNindicatorSize, XmCIndicatorSize,
    XmRVerticalDimension, sizeof(Dimension),
    CacheOffset(toggle_cache.indicator_dim),
    XmRImmediate, (XtPointer) XmINVALID_DIMENSION
  },

  {
    XmNindicatorType, XmCIndicatorType,
    XmRIndicatorType, sizeof(unsigned char),
    CacheOffset(toggle_cache.ind_type),
    XmRImmediate, (XtPointer) XmINVALID_TYPE
  },

  {
    XmNvisibleWhenOff, XmCVisibleWhenOff,
    XmRBoolean, sizeof(Boolean),
    CacheOffset(toggle_cache.visible),
    XmRImmediate, (XtPointer) XmINVALID_BOOLEAN
  },

  {
    XmNspacing, XmCSpacing,
    XmRHorizontalDimension, sizeof(Dimension),
    CacheOffset(toggle_cache.spacing),
    XmRImmediate, (XtPointer) 4
  },

  {
    XmNselectPixmap, XmCSelectPixmap,
    XmRDynamicPixmap, sizeof(Pixmap),
    CacheOffset(toggle_cache.on_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNselectInsensitivePixmap, XmCSelectInsensitivePixmap,
    XmRDynamicPixmap, sizeof(Pixmap),
    CacheOffset(toggle_cache.insen_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNindicatorOn, XmCIndicatorOn,
    XmRIndicatorOn, sizeof(unsigned char),
    CacheOffset(toggle_cache.ind_on),
    XmRImmediate, (XtPointer) XmINDICATOR_FILL
  },

  {
    XmNfillOnSelect, XmCFillOnSelect,
    XmRBoolean, sizeof(Boolean),
    CacheOffset(toggle_cache.fill_on_select),
    XmRImmediate, (XtPointer) XmINVALID_BOOLEAN
  },

  {
    XmNselectColor, XmCSelectColor,
    XmRSelectColor, sizeof(Pixel),
    CacheOffset(toggle_cache.select_color),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  },

  {
    XmNtoggleMode, XmCToggleMode,
    XmRToggleMode, sizeof(unsigned char),
    CacheOffset(toggle_cache.toggle_mode),
    XmRImmediate, (XtPointer) XmTOGGLE_BOOLEAN
  },

  {
    XmNindeterminatePixmap, XmCIndeterminatePixmap,
    XmRDynamicPixmap, sizeof(Pixmap),
    CacheOffset(toggle_cache.indeterminate_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNindeterminateInsensitivePixmap, XmCIndeterminateInsensitivePixmap,
    XmRDynamicPixmap, sizeof(Pixmap),
    CacheOffset(toggle_cache.indeterminate_insensitive_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNunselectColor, XmCUnselectColor,
    XmRPixel, sizeof(Pixel),
    CacheOffset(toggle_cache.unselect_color),
    XmRImmediate, (XtPointer) INVALID_PIXEL
  }
};


/************************************************
 * The uncached resources for ToggleButton
 ************************************************/


static XtResource resources[] =
{
  {
    XmNset, XmCSet,
    XmRSet, sizeof(unsigned char),
    Offset(toggle.set),
    XmRImmediate, (XtPointer) XmUNSET
  },

  {
    XmNvalueChangedCallback, XmCValueChangedCallback,
    XmRCallback, sizeof(XtCallbackList),
    Offset(toggle.value_changed_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNarmCallback, XmCArmCallback, XmRCallback,
    sizeof(XtCallbackList),
    Offset(toggle.arm_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNdisarmCallback, XmCDisarmCallback,
    XmRCallback, sizeof(XtCallbackList),
    Offset(toggle.disarm_CB),
    XmRPointer, (XtPointer)NULL
  },

  {
    XmNtraversalOn, XmCTraversalOn,
    XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmGadgetRec, gadget.traversal_on),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNhighlightThickness, XmCHighlightThickness,
    XmRHorizontalDimension, sizeof(Dimension),
    XtOffsetOf(struct _XmGadgetRec, gadget.highlight_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  },

   {
     XmNdetailShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     Offset(toggle.detail_shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   }
};

/* Resources that need special processing in get/set values. */

static XmSyntheticResource cache_syn_resources[] =
{
  {
    XmNspacing, sizeof(Dimension),
    CacheOffset(toggle_cache.spacing),
    XmeFromHorizontalPixels,
    XmeToHorizontalPixels
  },

  {
    XmNindicatorSize, sizeof(Dimension),
    CacheOffset(toggle_cache.indicator_dim),
    XmeFromVerticalPixels,
    XmeToVerticalPixels
  }
};

static XmSyntheticResource syn_resources[] =
{
  {
    XmNdetailShadowThickness,
    sizeof(Dimension),
    XtOffsetOf(XmToggleButtonGadgetRec, toggle.detail_shadow_thickness),
    XmeFromHorizontalPixels,XmeToHorizontalPixels
  }
};

#undef CacheOffset
#undef Offset

/*****************************************************************
 *
 *   Class Record definitions
 *
 ****************************************************************/

static XmCacheClassPart ToggleButtonClassCachePart =
{
  { NULL, 0, 0 },		/* head of class cache list */
  _XmCacheCopy,			/* Copy routine		    */
  _XmCacheDelete,		/* Delete routine  	    */
  _XmToggleBCacheCompare,	/* Comparison routine	    */
};

static XmBaseClassExtRec   ToggleBGClassExtensionRec =
{
    NULL,    				/* next_extension	  */
    NULLQUARK, 				/* record_type		  */
    XmBaseClassExtVersion,		/* version		  */
    sizeof(XmBaseClassExtRec),		/* record_size		  */
    InitializePrehook,		 	/* initializePrehook	  */
    SetValuesPrehook, 			/* setValuesPrehook	  */
    InitializePosthook, 		/* initializePosthook	  */
    SetValuesPosthook, 			/* setValuesPosthook	  */
    (WidgetClass)&xmToggleButtonGCacheObjClassRec, /* secondaryObjectClass */
    SecondaryObjectCreate,  	        /* secondaryObjectCreate  */
    GetToggleBGClassSecResData,         /* getSecResData	  */
    { 0 },           			/* Other Flags		  */
    GetValuesPrehook, 			/* getValuesPrehook	  */
    GetValuesPosthook, 			/* getValuesPosthook	  */
    (XtWidgetClassProc)NULL,            /* classPartInitPrehook	  */
    (XtWidgetClassProc)NULL,            /* classPartInitPosthook  */
    NULL,                               /* ext_resources	  */
    NULL,                               /* compiled_ext_resources */
    0,                                  /* num_ext_resources	  */
    FALSE,                              /* use_sub_resources	  */
    XmInheritWidgetNavigable,           /* widgetNavigable	  */
    XmInheritFocusChange,               /* focusChange		  */
    (XmWrapperData)NULL,		/* wrapperData		  */
};


/* ext rec static initialization */
externaldef(xmtogglebuttongcacheobjclassrec)
XmToggleButtonGCacheObjClassRec xmToggleButtonGCacheObjClassRec =
{
  {
      /* superclass         */    (WidgetClass) &xmLabelGCacheObjClassRec,
      /* class_name         */    "XmToggleButtonGadget",
      /* widget_size        */    sizeof(XmToggleButtonGCacheObjRec),
      /* class_initialize   */    (XtProc)NULL,
      /* chained class init */    (XtWidgetClassProc)NULL,
      /* class_inited       */    False,
      /* initialize         */    (XtInitProc)NULL,
      /* initialize hook    */    (XtArgsProc)NULL,
      /* realize            */    NULL,
      /* actions            */    NULL,
      /* num_actions        */    0,
      /* resources          */    cache_resources,
      /* num_resources      */    XtNumber(cache_resources),
      /* xrm_class          */    NULLQUARK,
      /* compress_motion    */    False,
      /* compress_exposure  */    False,
      /* compress enter/exit*/    False,
      /* visible_interest   */    False,
      /* destroy            */    (XtWidgetProc)NULL,
      /* resize             */    NULL,
      /* expose             */    NULL,
      /* set_values         */    (XtSetValuesFunc)NULL,
      /* set values hook    */    (XtArgsFunc)NULL,
      /* set values almost  */    NULL,
      /* get values hook    */    (XtArgsProc)NULL,
      /* accept_focus       */    NULL,
      /* version            */    XtVersion,
      /* callback offsetlst */    NULL,
      /* default trans      */    NULL,
      /* query geo proc     */    NULL,
      /* display accelerator*/    NULL,
      /* extension record   */    NULL,
  },

  {
      /* synthetic resources */   cache_syn_resources,
      /* num_syn_resources   */   XtNumber(cache_syn_resources),
      /* extension           */   NULL,
  }
};

/*************************************<->*************************************
 *
 *
 *   Description:  global class record for instances of class: ToggleButton
 *   -----------
 *
 *   Defines default field settings for this class record.
 *
 *************************************<->***********************************/

static XmGadgetClassExtRec _XmToggleBGadClassExtRec =
{
  NULL,				/* next_extension	 */
  NULLQUARK,			/* record_type		 */
  XmGadgetClassExtVersion,	/* version		 */
  sizeof(XmGadgetClassExtRec),	/* record_size		 */
  XmInheritBaselineProc,	/* widget_baseline	 */
  XmInheritDisplayRectProc,	/* widget_display_rect	 */
  XmInheritMarginsProc,         /* widget_margins */
};

externaldef(xmtogglebuttongadgetclassrec)
	XmToggleButtonGadgetClassRec xmToggleButtonGadgetClassRec =
{
  {
    /* superclass	  */	(WidgetClass) &xmLabelGadgetClassRec,
    /* class_name	  */	"XmToggleButtonGadget",
    /* widget_size	  */	sizeof(XmToggleButtonGadgetRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    (XtArgsProc)NULL,
    /* realize		  */	NULL,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
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
    /* accept_focus       */	NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator */   NULL,
    /* extension          */    (XtPointer)&ToggleBGClassExtensionRec
  },

  {        /* gadget class record */
    /* border_highlight    */	BorderHighlight,
    /* border_unhighlight  */	BorderUnhighlight,
    /* arm_and_activate    */	ArmAndActivate,
    /* input_dispatch      */	InputDispatch,
    /* visual_change       */	XmInheritVisualChange,
    /* syn resources       */	syn_resources,
    /* num_syn_resources   */   XtNumber(syn_resources),
    /* extension           */   &ToggleButtonClassCachePart,
    /* extension           */	(XtPointer)&_XmToggleBGadClassExtRec
  },

  {        /* label class record */
    /* SetOverrideCallback */	XmInheritWidgetProc,
    /* menu proc entry 	   */	XmInheritMenuProc,
    /* extension           */	NULL
  },

  {	    /* toggle class record */
    /* extension           */	NULL
  }
};

externaldef(xmtogglebuttongadgetclass)
  WidgetClass xmToggleButtonGadgetClass =
            (WidgetClass)&xmToggleButtonGadgetClassRec;


/* Menu Savvy trait record */
static XmMenuSavvyTraitRec MenuSavvyRecord =
{
  -1,				/* version		 */
  NULL,				/* disableCallback	 */
  NULL,				/* getAccelerator	 */
  NULL,				/* getMnemonic		 */
  _XmCBNameValueChanged		/* getActivateCBName	 */
};


/* Care about parent Visual trait record. */
static XmConst XmCareVisualTraitRec ToggleBGCVT =
{
  0,				/* version	 */
  HandleRedraw			/* redraw	 */
};

/***********************************************************
 *
 *  ClassInitialize
 *
 ************************************************************/

static void
ClassInitialize( void )
{
  Cardinal                    wc_num_res, sc_num_res;
  XtResource                  *merged_list;
  int                         i, j;
  XtResourceList              uncompiled;
  Cardinal                    num;

  /*
   * Label's and Togglebutton's resource lists are being merged into one
   * and assigned to xmToggleButtonGCacheObjClassRec. This is for performance
   * reasons, since, instead of two calls to XtGetSubResources XtGetSubvaluse
   * and XtSetSubvalues for both the superclass and the widget class, now
   * we have just one call with a merged resource list.
   * NOTE: At this point the resource lists for Label and Togglebutton do
   * have unique entries, but if there are resources in the superclass
   * that are being overwritten by the subclass then the merged_lists
   * need to be created differently.
   */

  wc_num_res = xmToggleButtonGCacheObjClassRec.object_class.num_resources;

  sc_num_res = xmLabelGCacheObjClassRec.object_class.num_resources;

  merged_list = (XtResource *)
    XtMalloc((sizeof(XtResource) * (wc_num_res + sc_num_res)));

  _XmTransformSubResources(xmLabelGCacheObjClassRec.object_class.resources,
                           sc_num_res, &uncompiled, &num);

  for (i = 0; i < num; i++)
    merged_list[i] = uncompiled[i];

  XtFree((char *)uncompiled);

  for (i = 0, j = num; i < wc_num_res; i++, j++)
    merged_list[j] = xmToggleButtonGCacheObjClassRec.object_class.resources[i];

  xmToggleButtonGCacheObjClassRec.object_class.resources = merged_list;
  xmToggleButtonGCacheObjClassRec.object_class.num_resources =
    wc_num_res + sc_num_res ;

  ToggleBGClassExtensionRec.record_type = XmQmotif;
}

/************************************************************************
 *
 * ClassPartInitialize
 *   Set up fast subclassing for the gadget.
 *
 ***********************************************************************/

static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmTOGGLE_BUTTON_GADGET_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabelG */
  _XmLabelGCloneMenuSavvy (wc, &MenuSavvyRecord);

  /* Install the careParentVisual trait for all subclasses as well. */
  XmeTraitSet((XtPointer)(WidgetClass) wc,
	      XmQTcareParentVisual, (XtPointer)&ToggleBGCVT);
}

/*******************************************************************
 *
 *  _XmToggleBCacheCompare
 *
 *******************************************************************/

int
_XmToggleBCacheCompare(
        XtPointer A,
        XtPointer B )
{
  XmToggleButtonGCacheObjPart *toggleB_inst =
    (XmToggleButtonGCacheObjPart *) A ;
  XmToggleButtonGCacheObjPart *toggleB_cache_inst =
    (XmToggleButtonGCacheObjPart *) B ;

  if ((toggleB_inst->ind_type == toggleB_cache_inst->ind_type) &&
      (toggleB_inst->visible == toggleB_cache_inst->visible) &&
      (toggleB_inst->spacing == toggleB_cache_inst->spacing) &&
      (toggleB_inst->indicator_dim == toggleB_cache_inst->indicator_dim) &&
      (toggleB_inst->on_pixmap == toggleB_cache_inst->on_pixmap) &&
      (toggleB_inst->insen_pixmap == toggleB_cache_inst->insen_pixmap) &&
      (toggleB_inst->ind_on == toggleB_cache_inst->ind_on) &&
      (toggleB_inst->fill_on_select == toggleB_cache_inst->fill_on_select) &&
      (toggleB_inst->select_color == toggleB_cache_inst->select_color) &&
      (toggleB_inst->select_GC == toggleB_cache_inst->select_GC) &&
      (toggleB_inst->unselect_GC == toggleB_cache_inst->unselect_GC) &&
      (toggleB_inst->unselect_color == toggleB_cache_inst->unselect_color) &&
      (toggleB_inst->indeterminate_pixmap ==
       toggleB_cache_inst->indeterminate_pixmap) &&
      (toggleB_inst->indeterminate_insensitive_pixmap ==
       toggleB_cache_inst->indeterminate_insensitive_pixmap) &&
      (toggleB_inst->indeterminate_GC ==
       toggleB_cache_inst->indeterminate_GC) &&
      (toggleB_inst->indeterminate_box_GC ==
       toggleB_cache_inst->indeterminate_box_GC) &&
      (toggleB_inst->toggle_mode == toggleB_cache_inst->toggle_mode) &&
      (toggleB_inst->reversed_select == toggleB_cache_inst->reversed_select) &&
      (toggleB_inst->background_gc == toggleB_cache_inst->background_gc) &&
      (toggleB_inst->ind_left_delta == toggleB_cache_inst->ind_left_delta) &&
      (toggleB_inst->ind_right_delta == toggleB_cache_inst->ind_right_delta) &&
      (toggleB_inst->ind_top_delta == toggleB_cache_inst->ind_top_delta) &&
      (toggleB_inst->ind_bottom_delta == toggleB_cache_inst->ind_bottom_delta))
    return 1;
  else
    return 0;
}

/************************************************************************
 *
 *  SecondaryObjectCreate
 *
 ************************************************************************/
/* ARGSUSED */
static void
SecondaryObjectCreate(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmBaseClassExt              *cePtr;
  XmWidgetExtData             extData;
  WidgetClass                 wc;
  Cardinal                    size;
  XtPointer                   newSec, reqSec;

  _XmProcessLock();
  cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);
  wc = (*cePtr)->secondaryObjectClass;
  size = wc->core_class.widget_size;

  newSec = _XmExtObjAlloc(size);
  reqSec = _XmExtObjAlloc(size);
  _XmProcessUnlock();


  /*
   * Update pointers in instance records now so references to resources
   * in the cache record will be valid for use in CallProcs.
   * CallProcs are invoked by XtGetSubresources().
   */

  LabG_Cache(new_w) = &(((XmLabelGCacheObject)newSec)->label_cache);
  LabG_Cache(req)   = &(((XmLabelGCacheObject)reqSec)->label_cache);
  TBG_Cache(new_w) = &(((XmToggleButtonGCacheObject)newSec)->toggle_cache);
  TBG_Cache(req)   = &(((XmToggleButtonGCacheObject)reqSec)->toggle_cache);

  /*
   * Since the resource lists for label and togglebutton were merged at
   * ClassInitialize time we need to make only one call to
   * XtGetSubresources()
   */

  XtGetSubresources(new_w,
                    newSec,
                    NULL, NULL,
                    wc->core_class.resources,
                    wc->core_class.num_resources,
                    args, *num_args );


  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;

  ((XmToggleButtonGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
  ((XmToggleButtonGCacheObject)newSec)->ext.logicalParent = new_w;

  _XmPushWidgetExtData(new_w, extData,
		       ((XmToggleButtonGCacheObject)newSec)->ext.extensionType);
  memcpy(reqSec, newSec, size);
}

/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/
/* ARGSUSED */
static void
InitializePrehook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  /* CR 2990: Use XmNbuttonFontList as the default. */
  if (LabG_Font(new_w) == NULL)
    LabG_Font(new_w) = XmeGetDefaultRenderTable (new_w, XmBUTTON_FONTLIST);
}

/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/

/* ARGSUSED */
static void
InitializePosthook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData     ext;
  XmToggleButtonGadget  tbw = (XmToggleButtonGadget)new_w;

  /*
   * - register parts in cache.
   * - update cache pointers
   * - and free req
   */

  _XmProcessLock();
  LabG_Cache(tbw) = (XmLabelGCacheObjPart *)
    _XmCachePart( LabG_ClassCachePart(tbw),
		 (XtPointer) LabG_Cache(tbw),
		 sizeof(XmLabelGCacheObjPart));

  TBG_Cache(tbw) = (XmToggleButtonGCacheObjPart *)
    _XmCachePart( TBG_ClassCachePart(tbw),
		 (XtPointer) TBG_Cache(tbw),
		 sizeof(XmToggleButtonGCacheObjPart));

  /*
   * might want to break up into per-class work that gets explicitly
   * chained. For right now, each class has to replicate all
   * superclass logic in hook routine
   */

  /*
   * free the req subobject used for comparisons
   */
  _XmPopWidgetExtData((Widget) tbw, &ext, XmCACHE_EXTENSION);
  _XmExtObjFree((XtPointer) ext->widget);
  _XmExtObjFree((XtPointer) ext->reqWidget);
  _XmProcessUnlock();
  XtFree( (char *) ext);
}

static Boolean
HandleRedraw (
	Widget kid,
	Widget cur_parent,
	Widget new_parent,
	Mask visual_flag)
{
  Boolean redraw = False;

  redraw = _XmLabelGCVTRedraw (kid,  cur_parent, new_parent, visual_flag);

  redraw = ToggleBGCVTRedraw (kid,  cur_parent, new_parent, visual_flag) ||
    redraw;

  if (redraw)
  {
    XtExposeProc expose;

    _XmProcessLock();
    expose = ((XmToggleButtonGadgetClassRec *)
	      (XtClass(kid)))->rect_class.expose;
    _XmProcessUnlock();

    if (! XtIsRealized(kid))
      return redraw;
    else
      (* (expose)) ((Widget)kid, NULL, (Region) NULL);
  }

  return False;
}

static Boolean
ToggleBGCVTRedraw (
	Widget kid,
	Widget cur_parent,
	Widget new_parent,
	Mask visual_flag)
{
  XmToggleButtonGadget tw = (XmToggleButtonGadget) kid ;
  XmManagerWidget mw = (XmManagerWidget) new_parent;
  XmManagerWidget curmw = (XmManagerWidget) cur_parent;
  Boolean redraw = False;

  XmToggleButtonGCacheObjPart  oldCopy;

  /*
   * For the purposes of backward compatibility the existance of
   * UnselectColor is obscured here.  That is it is made to follow
   * background.
   *
   * Unless of course someone sets it.
   */

  /* Deal with the Evil gadget cache */
  _XmProcessLock();
  _XmCacheCopy((XtPointer) TBG_Cache(tw), (XtPointer) &oldCopy,
	       sizeof(XmToggleButtonGCacheObjPart));
  _XmCacheDelete ((XtPointer) TBG_Cache(tw));
  _XmProcessUnlock();
  TBG_Cache(tw) = &oldCopy;

  if ((visual_flag & VisualBackgroundPixel) &&
      (TBG_UnselectColor(tw) == curmw->core.background_pixel))
    {
      XtReleaseGC (XtParent(tw), TBG_UnselectGC(tw));

      TBG_UnselectColor(tw) = mw->core.background_pixel;
      GetUnselectGC(tw);

      redraw = True;
    }

  _XmProcessLock();
  TBG_Cache(tw) = (XmToggleButtonGCacheObjPart *)
    _XmCachePart(TBG_ClassCachePart(tw), (XtPointer) TBG_Cache(tw),
		 sizeof(XmToggleButtonGCacheObjPart));
  _XmProcessUnlock();

  return redraw ;
}

/************************************************************************
 *
 *  InputDispatch
 *     This function catches input sent by a manager and dispatches it
 *     to the individual routines.
 *
 ************************************************************************/

static void
InputDispatch(
        Widget wid,
        XEvent *event,
        Mask event_mask )
{
  XmToggleButtonGadget tb = (XmToggleButtonGadget) wid ;
  if (event_mask & XmARM_EVENT)
    {
      if (LabG_IsMenupane(tb))
	BtnDown(tb, event);
      else
	Arm ((Widget) tb, event);
    }
  /* BEGIN OSF Fix pir 2097 */
  else if (event_mask & XmMULTI_ARM_EVENT)
    {
      if (! LabG_IsMenupane(tb))
	{
	  Arm ((Widget) tb, event);
	}
    }
  /* END OSF Fix pir 2097 */

  else if (event_mask & XmACTIVATE_EVENT)
    {
      if (LabG_IsMenupane(tb))
	{
	  if (event->type == ButtonRelease)
	    BtnUp(tb, event);
	  else /* Assume KeyPress or KeyRelease */
	    KeySelect (tb, event);
	}
      else
	{
	  Select(tb, event);
	  Disarm (tb, event);
	}
    }
  /* BEGIN OSF Fix pir 2097 */
  else if (event_mask & XmMULTI_ACTIVATE_EVENT)
    {
      if (! LabG_IsMenupane(tb))
	{
	  Select(tb, event);
	  Disarm (tb, event);
	}
    }
  /* END OSF Fix pir 2097 */

  else if (event_mask & XmHELP_EVENT)
    Help (tb, event);
  else if (event_mask & XmENTER_EVENT)
    Enter (tb, event);
  else if (event_mask & XmLEAVE_EVENT)
    Leave (tb, event);
  else if (event_mask & XmFOCUS_IN_EVENT)
    (*(((XmToggleButtonGadgetClass) XtClass( tb))
       ->gadget_class.border_highlight))( (Widget) tb) ;
  else if (event_mask & XmFOCUS_OUT_EVENT)
    (*(((XmToggleButtonGadgetClass) XtClass( tb))
       ->gadget_class.border_unhighlight))
      ( (Widget) tb) ;
  else if (event_mask & XmBDRAG_EVENT)
    _XmProcessDrag ((Widget) tb, event, NULL, NULL);
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
redisplayPixmap(XmToggleButtonGadget tb, XEvent *event, Region region)
{
  Pixmap todo;
  unsigned int onH = 0, onW = 0;
  int w, h;
  int x, y, offset;
  XRectangle saved_Text;
  LRectangle background_box;

  memcpy(&saved_Text, &LabG_TextRect(tb), sizeof(XRectangle));

  offset = tb->gadget.highlight_thickness + tb->gadget.shadow_thickness;

  x = offset + LabG_MarginWidth(tb) + LabG_MarginLeft(tb);

  y = offset + LabG_MarginHeight(tb) + LabG_MarginTop(tb);

  w = tb->rectangle.width - x - offset
    - LabG_MarginRight(tb) - LabG_MarginWidth(tb);

  ASSIGN_MAX(w, 0);

  h = tb->rectangle.height - y - offset - LabG_MarginBottom(tb)
    - LabG_MarginHeight(tb);

  ASSIGN_MAX(h, 0);

  x += tb->rectangle.x;

  y += tb->rectangle.y;

  /* CR 6023: As of release 2.0 the LabelG will clear it's own background. */
  background_box.x = x;
  background_box.y = y;
  background_box.width = w;
  background_box.height = h;

  /* CR 7041: ... but not when label.fill_bg_box is False! */
  if (!tb->label.fill_bg_box)
    XClearArea(XtDisplay(tb), XtWindow(tb), x, y, w, h, False);

  todo = tb->label.pixmap;

  if ((! XtIsSensitive((Widget) tb)) && tb->label.pixmap_insen)
    todo = tb->label.pixmap_insen;

  if (! IsNull(todo))
    XmeGetPixmapData(XtScreen(tb), todo,
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW, &onH);

  h = (tb->rectangle.height - onH) / 2 ;
  LabG_TextRect_y(tb) = MAX(h, 0);
  LabG_TextRect_height(tb) = onH;
  LabG_TextRect_width(tb) = onW;
  _XmRedisplayLabG ((Widget) tb, event, region, &background_box);

  memcpy(&LabG_TextRect(tb), &saved_Text, sizeof(XRectangle));
}

static void
HandlePixmap(XmToggleButtonGadget tb,
	     Pixmap pix,
	     Pixmap insen_pix,
	     XEvent * event,
	     Region region)
{
  if (XtIsSensitive((Widget) tb))
    {
      if (! IsNull (pix))
	{
	  Pixmap tempPix = LabG_Pixmap(tb);

	  LabG_Pixmap(tb) = pix;
	  redisplayPixmap(tb, event, region);
	  LabG_Pixmap(tb) = tempPix;
	}
      else
	redisplayPixmap(tb, event, region);
    }
  else
    {
      if (! IsNull (insen_pix))
	{
	  Pixmap tempPix = LabG_PixmapInsensitive(tb);

	  LabG_PixmapInsensitive(tb) = insen_pix;
	  redisplayPixmap(tb, event, region);
	  LabG_PixmapInsensitive(tb) = tempPix;
	}
      else
	redisplayPixmap(tb, event, region);
    }
}

/***********************************************************************
 *
 * SetAndDisplayPixmap
 *    Sets the appropriate on, off pixmap in label's pixmap field and
 *    calls label's Redisplay routine.
 *
 ***********************************************************************/
static void
SetAndDisplayPixmap(
        XmToggleButtonGadget w,
        XEvent *event,
        Region region )
{
  XmToggleButtonGadget tb = (XmToggleButtonGadget) w ;

  if (TBG_ToggleMode(tb) == XmTOGGLE_INDETERMINATE)
    {
      if (TBG_VisualSet(tb) == XmUNSET)
	HandlePixmap(tb, PixmapOff(tb), PixmapInsenOff(tb), event, region);
      else if (TBG_VisualSet(tb) == XmSET)
	HandlePixmap(tb, PixmapOn(tb), PixmapInsenOn(tb), event, region);
      else if (TBG_VisualSet(tb) == XmINDETERMINATE)
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
 *     on the gadget.
 *
 ************************************************************************/

static void
Help(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  Boolean is_menupane = LabG_IsMenupane(tb);
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->buttonPopdown(XtParent(tb), event);

  ToggleButtonCallback(tb, XmCR_HELP, TBG_Set(tb), event);

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->reparentToTearOffShell(XtParent(tb), event);
}

/*************************************************************************
 *
 * ToggleButtonCallback
 *    This is the widget's application callback routine
 *
 *************************************************************************/
static void
ToggleButtonCallback(
        XmToggleButtonGadget data,
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
      XtCallCallbackList ((Widget) data, TBG_ValueChangedCB(data), &temp);
      break;

    case XmCR_ARM:
      XtCallCallbackList ((Widget) data, TBG_ArmCB(data), &temp);
      break;

    case XmCR_DISARM:
      XtCallCallbackList ((Widget) data, TBG_DisarmCB(data), &temp);
      break;

    case XmCR_HELP:
      _XmSocorro( (Widget) data, event, NULL, NULL);
      break;
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

/* Update the toggle after an Enter or Leave action. */
static void
ActionDraw(XmToggleButtonGadget w,
	   XEvent              *event,
	   Boolean              leave)
{
  if (TBG_Armed(w))
    {
      /* CR 7301: We may have armed while outside the toggle. */
      if (leave)
	TBG_VisualSet(w) = TBG_Set(w);
      else if (TBG_ToggleMode(w) == XmTOGGLE_INDETERMINATE)
	NextState(&TBG_VisualSet(w));
      else
	IsOn(w) = !TBG_Set(w);

      if (TBG_IndOn(w))
	DrawToggle(w);
      else
	{
	  if (w->gadget.shadow_thickness > 0)
	    DrawToggleShadow(w);
	  if (TBG_FillOnSelect(w) && !LabG_IsPixmap(w))
	    DrawToggleLabel(w);
	}

      if (LabG_IsPixmap(w) || LabG_IsPixmapAndText(w))
	SetAndDisplayPixmap(w, event, NULL);
    }
}

/**************************************************************************
 *
 *   Leave
 *     This procedure is called when  the mouse button is pressed and  the
 *     cursor moves out of the widget's window. This procedure is used
 *     to change the visuals.
 *
 *************************************************************************/

static void
Leave(
        XmToggleButtonGadget w,
        XEvent *event )
{
  if (LabG_IsMenupane(w))
    {
      if (_XmGetInDragMode((Widget)w) && TBG_Armed(w))
	{
	  XmDisplay dpy;
	  Boolean etched_in;

	  dpy = (XmDisplay) XmGetXmDisplay(XtDisplayOfObject((Widget) w));
	  etched_in = dpy->display.enable_etched_in_menu;

	  ((XmManagerWidget) XtParent(w))->manager.active_child = NULL;

	  TBG_Armed(w) = FALSE;

	 if ((etched_in) &&
	     (TBG_IndOn(w) ||
	      (!(TBG_IndOn(w)) && !(TBG_FillOnSelect(w)))))
	    {
		DrawEtchedInMenu(w);
		if (TBG_IndOn(w))
		    DrawToggle(w);
	    }

	  XmeDrawHighlight(XtDisplay(w),
			   XtWindow(w),
			   LabG_BackgroundGC(w),
			   w->rectangle.x + w->gadget.highlight_thickness,
			   w->rectangle .y + w->gadget.highlight_thickness,
			   w->rectangle.width - 2 *
			   w->gadget.highlight_thickness,
			   w->rectangle.height - 2 *
			   w->gadget.highlight_thickness,
			   w->gadget.shadow_thickness);


	  if (TBG_DisarmCB(w))
	    {
	      XFlush (XtDisplay (w));
	      ToggleButtonCallback(w, XmCR_DISARM, TBG_Set(w), event);
	    }
	}
    }
  else
    {
      _XmLeaveGadget( (Widget) w, event, NULL, NULL);
      ActionDraw(w, event, TRUE);
    }
}

/**************************************************************************
 *
 *  Enter
 *    This procedure is called when the mouse button is pressed and the
 *    cursor reenters the widget's window. This procedure changes the visuals
 *    accordingly.
 *
 **************************************************************************/

static void
Enter(
        XmToggleButtonGadget w,
        XEvent *event )
{
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) w));
  Boolean etched_in = xm_dpy->display.enable_etched_in_menu;

  if (LabG_IsMenupane(w))
    {
      if ((((ShellWidget) XtParent(XtParent(w)))->shell.popped_up) &&
          _XmGetInDragMode((Widget)w))
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) w));
	  etched_in = dpy->display.enable_etched_in_menu;

	  if (TBG_Armed(w))
	    return;

	  /* So KHelp event is delivered correctly */
	  _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, TRUE);
	  XtSetKeyboardFocus(XtParent(XtParent(w)), (Widget)w);
	  _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, FALSE);

	  ((XmManagerWidget) XtParent(w))->manager.active_child = (Widget) w;

	  TBG_Armed(w) = TRUE;

	 if ((etched_in) &&
	     (TBG_IndOn(w) ||
	      (!(TBG_IndOn(w)) && !(TBG_FillOnSelect(w)))))
	   {
	       DrawEtchedInMenu(w);
	       if (TBG_IndOn(w))
		   DrawToggle(w);
	   }

	  XmeDrawShadows (XtDisplay (w), XtWindow (w),
			  LabG_TopShadowGC(w),
			  LabG_BottomShadowGC(w),
			  w->rectangle.x + w->gadget.highlight_thickness,
			  w->rectangle.y + w->gadget.highlight_thickness,
			  w->rectangle.width -
			  2 * w->gadget.highlight_thickness,
			  w->rectangle.height -
			  2 * w->gadget.highlight_thickness,
			  w->gadget.shadow_thickness,
			  etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	  if (TBG_ArmCB(w))
	    {
	      XFlush (XtDisplay (w));
	      ToggleButtonCallback(w, XmCR_ARM, TBG_Set(w), event);
	    }
	}
    }
  else
    {
      _XmEnterGadget( (Widget) w, event, NULL, NULL);
      ActionDraw(w, event, FALSE);
    }
}

/************************************************************************
 *
 *     Arm
 *        This function processes button 1 down occuring on the togglebutton.
 *        Mark the togglebutton as armed and display it armed.
 *        The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/

static void
Arm(
        Widget w,
        XEvent *event )
{
  XmToggleButtonGadget tb = (XmToggleButtonGadget)w;

  if (TBG_ToggleMode(tb) == XmTOGGLE_INDETERMINATE)
    {
      NextState(&TBG_VisualSet(tb));
      TBG_Armed(tb) = TRUE;
    }
  else
    {
      IsOn(tb) = (TBG_Set(tb) == XmSET) ? XmUNSET : XmSET;
      TBG_Armed(tb) = TRUE;
    }


  if (TBG_IndOn(tb))
    DrawToggle((XmToggleButtonGadget) w);
  else
    {
      if (tb->gadget.shadow_thickness> 0)  DrawToggleShadow (tb);
      if (TBG_FillOnSelect(w) && !LabG_IsPixmap(w)) DrawToggleLabel (tb);
    }

  if (LabG_IsPixmap(tb) || LabG_IsPixmapAndText(tb))
    SetAndDisplayPixmap(tb, event, NULL);

  if (TBG_ArmCB(tb))
    {
      XFlush(XtDisplay(tb));
      ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);
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

static void
Select(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  XmToggleButtonCallbackStruct call_value;
  Boolean hit;
  XmMenuSystemTrait menuSTrait;
  Boolean radio = TRUE, always = TRUE;

  if ( TBG_Armed(tb) == FALSE)
     return;

  TBG_Armed(tb) = FALSE;

  if (XmIsRowColumn(XtParent(tb))) {
    XtVaGetValues(XtParent(tb),
    XmNradioBehavior, &radio,
    XmNradioAlwaysOne, &always,
    NULL);
  }

  /* CR 8068: Verify that this is in fact a button event. */
  /* CR 9181: Consider clipping when testing visibility. */
  hit = (((event->xany.type == ButtonPress) ||
	  (event->xany.type == ButtonRelease)) &&
	 _XmGetPointVisibility((Widget) tb,
			       event->xbutton.x_root,
			       event->xbutton.y_root));

  if (hit)
    {
      if (TBG_ToggleMode(tb) == XmTOGGLE_INDETERMINATE)
	NextState(&TBG_Set(tb));
      else
	TBG_Set(tb) = !TBG_Set(tb);
    }

  /* CR 7803: Suppress redundant redraws. */
  if (tb->toggle.set != tb->toggle.visual_set)
    {
      XtExposeProc expose;
      _XmProcessLock();
      expose = ((XmToggleButtonGadgetClassRec *)(tb->object.widget_class))->
	  rect_class.expose;
      _XmProcessUnlock();
      /* Redisplay after changing state. */
      (* (expose)) ((Widget) tb, event, (Region) NULL);
    }

  if (hit)
    {
      /* UNDOING this fix ... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /*	reported correctly even if the entry callback resets it. */
      menuSTrait = (XmMenuSystemTrait)
	XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);

      if (menuSTrait != NULL)
	{
	  call_value.reason = XmCR_VALUE_CHANGED;
	  call_value.event = event;
	  call_value.set = TBG_Set(tb);
	  menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
	}

      if ((! LabG_SkipCallback(tb)) &&
	  (TBG_ValueChangedCB(tb)))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, TBG_Set(tb), event);
	}

    }
}

/**********************************************************************
 *
 *    Disarm
 *     The callbacks for XmNdisarmCallback are called..
 *
 ************************************************************************/

static void
Disarm(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  if (TBG_DisarmCB(tb))
    ToggleButtonCallback(tb, XmCR_DISARM, TBG_Set(tb), event);

  if (tb->toggle.set != tb->toggle.visual_set)
  {
    tb->toggle.visual_set = tb->toggle.set;
    Redisplay((Widget) tb, event, (Region) NULL);
  }
}

static void
TBG_FixTearoff( XmToggleButtonGadget tb)
{
	 if  (XmMENU_PULLDOWN == LabG_MenuType(tb))
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
 *    Modify: Current implementation does care to draw shadows if indicator
 *	     is set to false; This is being modified.
 ************************************************************************/

/*ARGSUSED*/
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmToggleButtonGadget tb = (XmToggleButtonGadget) wid ;
  XmToggleButtonCallbackStruct call_value;
  Boolean already_armed = TBG_Armed(tb);
  Boolean is_menupane = LabG_IsMenupane(tb);
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
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

  if (is_menupane && !XmIsMenuShell(XtParent(XtParent(tb))) &&
      menuSTrait != NULL)
    {
      if (XmeFocusIsInShell((Widget)tb))
	{
	  /* In case allowAcceleratedInsensitiveUnmanagedMenuItems is True */
	  if (!XtIsSensitive((Widget)tb) || (!XtIsManaged((Widget)tb)))
            return;
	  torn_has_focus = TRUE;
	}
    }

  TBG_Armed(tb) = FALSE;

  if (!no_change)
    {
      if (TBG_ToggleMode(tb) == XmTOGGLE_INDETERMINATE)
        {
          NextState(&TBG_VisualSet(tb));
          NextState(&TBG_Set(tb));
        }
      else
        {
          TBG_Set(tb) = (TBG_Set(tb) == TRUE) ? FALSE : TRUE;
          IsOn(tb) = TBG_Set(tb);
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
      if (TBG_IndOn(tb))
	DrawToggle(tb);
      else if (TBG_FillOnSelect(tb) && !LabG_IsPixmap(tb))
	DrawToggleLabel (tb);

      if (LabG_IsPixmap(tb) || LabG_IsPixmapAndText(tb))
	SetAndDisplayPixmap(tb, NULL, NULL);
    }
  else
    {
      if (TBG_IndOn(tb))
	DrawToggle(tb);
      else
	{
	  if (tb->gadget.shadow_thickness> 0)
	    DrawToggleShadow (tb);
	  if (TBG_FillOnSelect(tb) && !LabG_IsPixmap(tb))
	    DrawToggleLabel (tb);
	}

      if (LabG_IsPixmap(tb) || LabG_IsPixmapAndText(tb))
	SetAndDisplayPixmap(tb, event, NULL);
    }

  /* If the parent is menu system able, set the lastSelectToplevel before
   * the arm. It's ok if this is recalled later.
   */
  if (menuSTrait != NULL)
    menuSTrait->getLastSelectToplevel(XtParent(tb));

  if (TBG_ArmCB(tb) && !already_armed)
    ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);

  if (!no_change)
  {
    /* UNDOING this fix ... */
    /* CR 8904: Notify value_changed before entry so that state is */
    /*	reported correctly even if the entry callback resets it. */

    /* if the parent is menu system able, notify it about the select */
    if (menuSTrait != NULL)
      {
        call_value.reason = XmCR_VALUE_CHANGED;
        call_value.event = event;
        call_value.set = TBG_Set(tb);
        menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);
      }

    if ((! LabG_SkipCallback(tb)) &&
        (TBG_ValueChangedCB(tb)))
      {
        XFlush(XtDisplay(tb));
        ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, TBG_Set(tb), event);
      }
  }

  if (TBG_DisarmCB(tb))
    {
      XFlush(XtDisplay(tb));
      ToggleButtonCallback(tb, XmCR_DISARM, TBG_Set(tb), event);
    }

  if (is_menupane && menuSTrait != NULL)
    {
      if (torn_has_focus && XtIsSensitive(wid))
	{
	  TBG_Armed(tb) = TRUE;
	  if (TBG_ArmCB(tb))
	    {
	      XFlush(XtDisplay(tb));
	      ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);
	    }
	}
      else
        {
	menuSTrait->reparentToTearOffShell(XtParent(tb), event);
	TBG_FixTearoff(tb);
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

static void
BtnDown(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  Boolean already_armed;
  ShellWidget popup;
  XmMenuSystemTrait menuSTrait;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(tb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);
  if (menuSTrait == NULL) return;

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

  if (popup)
    {
      if (popup->shell.popped_up)
	menuSTrait->popdownEveryone((Widget) popup, event);
    }

  /* Set focus to this button.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal( (Widget) tb, XmTRAVERSE_CURRENT);
  /* get the location cursor - get consistent with Gadgets */

  already_armed = TBG_Armed(tb);

  TBG_Armed(tb) = TRUE;

  if ((etched_in) &&
      (TBG_IndOn(tb) ||
       (!(TBG_IndOn(tb)) && !(TBG_FillOnSelect(tb)))))
     {
	 DrawEtchedInMenu(tb);
	 if (TBG_IndOn(tb))
	     DrawToggle(tb);
     }

  XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
		  LabG_TopShadowGC(tb),
		  LabG_BottomShadowGC(tb),
		  tb->rectangle.x + tb->gadget.highlight_thickness,
		  tb->rectangle.y + tb->gadget.highlight_thickness,
		  tb->rectangle.width - 2 * tb->gadget.highlight_thickness,
		  tb->rectangle.height - 2 * tb->gadget.highlight_thickness,
		  tb->gadget.shadow_thickness,
		  etched_in ? XmSHADOW_IN : XmSHADOW_OUT);


  if (TBG_ArmCB(tb) && !already_armed)
    {
      XFlush (XtDisplay (tb));

      ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);
    }

  _XmRecordEvent (event);
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
static void
BtnUp(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  XmToggleButtonCallbackStruct call_value;
  Boolean popped_up;
  Boolean valid_event;
  Boolean is_menupane = LabG_IsMenupane(tb);
  Widget shell = XtParent(XtParent(tb));
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);
  if (menuSTrait == NULL) return;

  TBG_Armed(tb) = FALSE;


  /* We need to validate the event in case the XmMENU_POPDOWN restores the
   * submenu to the transient shell.  The tear off control becomes unmanaged
   * and the submenu's (and menu item children) layout/geometry changes.
   */
  /* CR 9181: Consider clipping when testing visibility. */
  valid_event = (((event->xany.type == ButtonPress) ||
		  (event->xany.type == ButtonRelease)) &&
		 _XmGetPointVisibility((Widget)tb,
				       event->xbutton.x_root,
				       event->xbutton.y_root));

  if (is_menupane && !XmIsMenuShell(shell))
    popped_up = menuSTrait->popdown((Widget) tb, event);
  else
    popped_up = menuSTrait->buttonPopdown((Widget) tb, event);

  _XmRecordEvent(event);

  if (popped_up)
    return;

  if (valid_event)
    {
      if (TBG_ToggleMode(tb) == XmTOGGLE_INDETERMINATE)
	{
	  NextState(&TBG_VisualSet(tb));
	  NextState(&TBG_Set(tb));
	}
      else
	{
	  TBG_Set(tb) = (TBG_Set(tb) == TRUE) ? FALSE : TRUE;
	  IsOn(tb) = TBG_Set(tb);
	}

      /* UNDOING this fix ... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /*	reported correctly even if the entry callback resets it. */
      /* notify the parent about the select */
      call_value.reason = XmCR_VALUE_CHANGED;
      call_value.event = event;
      call_value.set = TBG_Set(tb);
      menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);

      if ((! LabG_SkipCallback(tb)) &&
	  (TBG_ValueChangedCB(tb)))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, TBG_Set(tb), event);
	}

      if (TBG_DisarmCB(tb))
	ToggleButtonCallback(tb, XmCR_DISARM, TBG_Set(tb), event);

      /* If the original shell does not indicate an active menu, but rather a
       * tear off pane, leave the button in an armed state.
       */
      if (!XmIsMenuShell(shell))
	{
	  if (XtIsSensitive((Widget)tb))
	    {
	      TBG_Armed(tb) = TRUE;

	      if (TBG_IndOn(tb))
		DrawToggle(tb);
	      else if (TBG_FillOnSelect(tb) && !LabG_IsPixmap(tb))
		DrawToggleLabel (tb);

	      if (LabG_IsPixmap(tb) || LabG_IsPixmapAndText(tb))
		SetAndDisplayPixmap( tb, event, NULL);

	      if (TBG_ArmCB(tb))
		{
		  XFlush(XtDisplay(tb));
		  ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);
		}
	    }
	}
      else
	menuSTrait->reparentToTearOffShell(XtParent(tb), event);
    }

  _XmSetInDragMode((Widget)tb, False);

  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  if (!XmIsMenuShell(shell))
    XmProcessTraversal((Widget) tb, XmTRAVERSE_CURRENT);
  TBG_FixTearoff(tb);
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
        XmToggleButtonGadget tw )
{
  XGCValues values;
  XtGCMask  valueMask;

  valueMask = GCForeground | GCBackground | GCFillStyle | GCGraphicsExposures;
  values.foreground = TBG_UnselectColor(tw);
  values.background = LabG_Background(tw);
  values.fill_style = FillSolid;
  values.graphics_exposures = FALSE;

  TBG_UnselectGC(tw) = XtGetGC((Widget) tw, valueMask, &values);
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
        XmToggleButtonGadget tw )
{
  XmManagerWidget mw = (XmManagerWidget) XtParent(tw);
  XGCValues values;
  XtGCMask  valueMask;
  XFontStruct *fs = (XFontStruct *) NULL;
  Pixel sel_color, select_pixel;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) tw));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  /* Differentiate background and select colors on monochrome displays or
  ** others on which we have indistinguishable colors. */
  if ((DefaultDepthOfScreen (XtScreen (tw)) == 1) ||
      (LabG_Background(tw) == TBG_SelectColor(tw)))
    sel_color = LabG_Foreground(tw);
  else
    sel_color = TBG_SelectColor(tw);

  valueMask = 0;
  valueMask |= GCForeground, values.foreground = sel_color;
  valueMask |= GCBackground, values.background =  LabG_Background(tw);
  valueMask |= GCFillStyle, values.fill_style = FillSolid;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;

  TBG_SelectGC(tw) = XtAllocateGC((Widget) mw, 0, valueMask, &values, 0, 0);


  /* When foreground and select colors coincide, this GC is used
   * by XmLabel to draw the text.  It requires a font to pacify
   * the XmString draw functions.
   */
  valueMask = 0;
  if (XmeRenderTableGetDefaultFont(LabG_Font(tw), &fs))
    valueMask |= GCFont, values.font = fs->fid;

  valueMask |= GCForeground, values.foreground = LabG_Background(tw);
  valueMask |= GCBackground, values.background = LabG_Foreground(tw);
  valueMask |= GCFillStyle, values.fill_style = FillSolid;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;
  valueMask |= GCLineWidth, values.line_width = 1;

  TBG_BackgroundGC(tw) = XtGetGC((Widget) mw, valueMask, &values);

  valueMask = 0;
  valueMask |= GCFillStyle, values.fill_style = FillOpaqueStippled;
  valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;
  valueMask |= GCStipple, values.stipple =
    XmGetPixmapByDepth(XtScreen((Widget)(tw)), XmS50_foreground, 1, 0, 1);
  valueMask |= GCLineWidth, values.line_width = 1;

  TBG_IndeterminateGC(tw) = XtAllocateGC((Widget) tw, 0, valueMask, &values,
					 GCForeground | GCBackground, 0);

  /* The valueMask and values are inherited from above. */
  valueMask &= ~GCLineWidth;
  valueMask |= GCForeground, values.foreground = LabG_Background(tw);
  valueMask |= GCBackground, values.background = LabG_Foreground(tw);

  TBG_IndeterminateBoxGC(tw) = XtGetGC((Widget) tw, valueMask, &values);

  /* Create the ArmGC for filling in background if we are in a menu
     and enableEtchedInMenu is True. */
  if ((LabG_IsMenupane(tw)) && etched_in) {
      Widget wid = (Widget) XtParent(tw);
      XmGetColors(XtScreen(wid), wid->core.colormap,
		  wid->core.background_pixel,
		  NULL, NULL, NULL, &select_pixel);

      valueMask = 0;
      valueMask |= GCForeground, values.foreground = select_pixel;
      valueMask |= GCBackground, values.background = LabG_Foreground(tw);
      if (fs != NULL)
	  valueMask |= GCFont, values.font = fs->fid;
      valueMask |= GCGraphicsExposures, values.graphics_exposures = FALSE;

      TBG_ArmGC(tw) = XtGetGC((Widget) tw, valueMask, &values);
  }
}

/*************************************<->*************************************
 *
 *  Initialize
 *    If the rectangle height and width fields are set to 0, treat that as a
 *    flag, and compute the optimum size for this button.  Then using what ever
 *    the rectangle fields are set to, compute the text placement fields.
 *************************************<->***********************************/

/*ARGSUSED*/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmToggleButtonGadget request = (XmToggleButtonGadget) rw ;
  XmToggleButtonGadget new_w = (XmToggleButtonGadget) nw ;
  XtWidgetProc resize;


  TBG_Armed(new_w) = FALSE;

  /* if menuProcs is not set up yet, try again */
  _XmProcessLock();
  resize = xmLabelGadgetClassRec.rect_class.resize;
  if (xmLabelGadgetClassRec.label_class.menuProcs == (XmMenuProc)NULL)
    xmLabelGadgetClassRec.label_class.menuProcs =
      (XmMenuProc) _XmGetMenuProcContext();
  _XmProcessUnlock();


  if (LabG_IsMenupane(new_w))
    {
      /* If the shadow thickness hasn't been set yet, inherit it
	   from the menu parent, instead of a hard-coded 2, as before.
	   The only draw back is that is the parent has also 0, then
	   the toggle shadow is 0, which is not very good in a menu,
	   but at least consistent with the other buttons */
	if (new_w->gadget.shadow_thickness <= 0) {
	  Dimension  parent_st ;

	  XtVaGetValues(XtParent(nw), XmNshadowThickness, &parent_st, NULL);
	  new_w->gadget.shadow_thickness = parent_st;
      }

      if (TBG_Visible(new_w) == XmINVALID_BOOLEAN)
	TBG_Visible(new_w) = FALSE;

      new_w->gadget.traversal_on = TRUE;
    }
  else
    {
      if (TBG_Visible(new_w) == XmINVALID_BOOLEAN)
	TBG_Visible(new_w) = TRUE;
    }

  /*
   * if the indicatorType has not been set, then
   * find out if radio behavior is set for RowColumn parents and
   * then set indicatorType.  If radio behavior is true, default to
   * one of many, else default to n of many.
   */
  if ((TBG_IndType( new_w) == XmINVALID_TYPE) ||
      !XmRepTypeValidValue(XmRID_INDICATOR_TYPE,
			   TBG_IndType( new_w), (Widget) new_w))
    {
      Boolean radio = FALSE;

      if (XmIsRowColumn(XtParent(new_w)))
	{
	  XtVaGetValues(XtParent(new_w),
			XmNradioBehavior, &radio,
			NULL);
	}

      if (radio)
	TBG_IndType(new_w) = XmONE_OF_MANY;
      else
	TBG_IndType(new_w) = XmN_OF_MANY;
    }

  /*
   * This resource defaults to true if an indicator box is drawn.
   */
  if (TBG_FillOnSelect(new_w) == XmINVALID_BOOLEAN)
    {
      if (DRAWBOX(NormalizeIndOn(new_w)))
	TBG_FillOnSelect(new_w) = True;
      else if (IsOneOfMany(TBG_IndType(new_w)) &&
	       TBG_IndOn(new_w))
	TBG_FillOnSelect(new_w) = True;
      else
	TBG_FillOnSelect(new_w) = False;
    }

  /* Tristate buttons ain't allowed in one-of-many land. */
  if (IsOneOfMany(TBG_IndType(new_w)))
    TBG_ToggleMode(new_w) = XmTOGGLE_BOOLEAN;


  /* Use the On pixmaps if there are no indeterminate pixmaps. */
  if (IsNull(PixmapInd(new_w)) && !IsNull(PixmapOn(new_w)))
    PixmapInd(new_w) = PixmapOn(new_w);
  if (IsNull(PixmapInsenInd(new_w)) && !IsNull(PixmapInsenOn(new_w)))
    PixmapInsenInd(new_w) = PixmapInsenOn(new_w);

  /* Use the On pixmap if no Off pixmap is available. */
  if (IsNull(PixmapOff(new_w)) && !IsNull(PixmapOn(new_w)))
    {
      PixmapOff(new_w) = PixmapOn(new_w);
      if (request->rectangle.width == 0)
	new_w->rectangle.width = 0;
      if (request->rectangle.height == 0)
	new_w->rectangle.height = 0;

      _XmCalcLabelGDimensions(nw);
      (* resize)( (Widget) new_w);
    }

  /* Use the insensitive On pixmap if there is no insensitive Off pixmap. */
  if (IsNull(PixmapInsenOff(new_w)) && !IsNull(PixmapInsenOn(new_w)))
    {
      PixmapInsenOff(new_w) = PixmapInsenOn(new_w);
      if (request->rectangle.width == 0)
	new_w->rectangle.width = 0;
      if (request->rectangle.height == 0)
	new_w->rectangle.height = 0;

      _XmCalcLabelGDimensions(nw);
      (* resize)( (Widget) new_w);
    }

  /* BEGIN OSF Fix pir 1778 */
  if (LabG_IsPixmap(new_w) &&
      (!IsNull(PixmapOff(new_w)) || !IsNull(PixmapInsenOff(new_w)) ||
       !IsNull(PixmapOn(new_w)) || !IsNull(PixmapInsenOn(new_w)) ||
       !IsNull(PixmapInd(new_w)) || !IsNull(PixmapInsenInd(new_w))))
    {
      if (request->rectangle.width == 0)
	new_w->rectangle.width = 0;
      if (request->rectangle.height == 0)
	new_w->rectangle.height = 0;
      SetToggleSize(new_w);
    }
  /* END OSF Fix pir 1778 */

  if (TBG_IndicatorDim(new_w) == XmINVALID_DIMENSION) {
    if (TBG_IndOn(new_w))
      {
	/* Determine how high the toggle indicator should be. */
	if (LabG_IsPixmap(new_w) || LabG_IsPixmapAndText(new_w))
	  {
	    /* Set indicatorSize proportional to size of pixmap. */
            if (!IsNull(PixmapOff(new_w)) || !IsNull(PixmapInsenOff(new_w)) ||
             !IsNull(PixmapOn(new_w)) || !IsNull(PixmapInsenOn(new_w)) ||
             !IsNull(PixmapInd(new_w)) || !IsNull(PixmapInsenInd(new_w)))
	      if (LabG_TextRect(new_w).height < 13)
	        TBG_IndicatorDim(new_w) = LabG_TextRect(new_w).height;
	      else
	        TBG_IndicatorDim(new_w) = 13 + (LabG_TextRect(new_w).height/13);
	    else
	      TBG_IndicatorDim(new_w) = XmDEFAULT_INDICATOR_DIM;
	  }
	else /* Set indicatorSize proportional to size of font. */
	  {
	    Dimension height;
	    int line_count;

	    height = XmStringHeight (LabG_Font(new_w), LabG__label(new_w));
	    if ((line_count = XmStringLineCount (LabG__label(new_w))) < 1)
	      line_count = 1;

	    /* Shiz recommends toggles in menus have smaller indicators */
	    if (LabG_IsMenupane(new_w))
	      TBG_IndicatorDim(new_w) =
		MAX(XmDEFAULT_INDICATOR_DIM,
		    (height / ((Dimension)line_count)) * 2/3);
	    else
	      TBG_IndicatorDim(new_w) =
		MAX(XmDEFAULT_INDICATOR_DIM, height / ((Dimension)line_count));
	  }
      } else {
        TBG_IndicatorDim(new_w) = 0;
      }

    TBG_IndicatorSet(new_w) = FALSE;
  } else {
    TBG_IndicatorSet(new_w) = TRUE;
  }

  /* CR 2337: Maintain original margin values. */
  TBG_IndLeftDelta(new_w) = 0;
  TBG_IndRightDelta(new_w) = 0;
  TBG_IndTopDelta(new_w) = 0;
  TBG_IndBottomDelta(new_w) = 0;

  if (TBG_IndOn(new_w))
    {
      /*
       *   Enlarge the text rectangle if needed to accomodate the size of
       *   indicator button. Adjust the dimensions of superclass Label-Gadget
       *   so that the toggle-button may be accommodated in it.
       */
      int maxIndicatorSize;   /* Max Indicator size permissible */
      int delta;
      int boxSize;

      /* BEGIN OSF Fix pir 2480 */
      if (! LabG_IsMenupane(new_w))
	maxIndicatorSize = TBG_IndicatorDim(new_w) + 2 * Xm3D_ENHANCE_PIXEL;
      else
	maxIndicatorSize = TBG_IndicatorDim(new_w);
      /* END OSF Fix pir 2480 */

      boxSize = ((int) LabG_TextRect(new_w).height +
		 (int) LabG_MarginTop (new_w) +
		 (int) LabG_MarginBottom (new_w));

      if (maxIndicatorSize > boxSize)
	{
	  delta = (maxIndicatorSize - boxSize) / 2;
	  TBG_IndTopDelta(new_w) = delta;
	  LabG_MarginTop(new_w) += delta;
	  TBG_IndBottomDelta(new_w) = delta;
	  LabG_MarginBottom(new_w) += delta;
	}

      /* CR 2337: Make room for toggle indicator and spacing */
      if (LayoutIsRtoLG(new_w))
	{
	  delta = (TBG_IndicatorDim(new_w) + TBG_Spacing(new_w) -
		   LabG_MarginRight(new_w));
	  if (delta > 0)
	    {
	      TBG_IndRightDelta(new_w) = delta;
	      LabG_MarginRight(new_w) += delta;
	    }
	}
      else
	{
	  delta = (TBG_IndicatorDim(new_w) + TBG_Spacing(new_w) -
		   LabG_MarginLeft(new_w));
	  if (delta > 0)
	    {
	      TBG_IndLeftDelta(new_w) = delta;
	      LabG_MarginLeft(new_w) += delta;
	    }
	}
    }

  if (request->rectangle.width == 0)
    {
      new_w->rectangle.width = LabG_TextRect(new_w).width +
	2 * LabG_MarginHeight(new_w) +
	  LabG_MarginRight(new_w) + LabG_MarginLeft(new_w) +
	      2 * (new_w->gadget.highlight_thickness +
		   new_w->gadget.shadow_thickness);

      if (new_w->rectangle.width == 0)
	new_w->rectangle.width = 1;

      if ((LabG__acceleratorText(new_w) != NULL) && (TBG_IndOn(new_w)))
      {
	if (LayoutIsRtoLG(new_w))
        {
	  LabG_AccTextRect(new_w).x =   new_w->gadget.highlight_thickness +
	    new_w->gadget.shadow_thickness + LabG_MarginHeight(new_w);
      }
	else
        {
	  LabG_AccTextRect(new_w).x = new_w->rectangle.width -
	    new_w->gadget.highlight_thickness -
	      new_w->gadget.shadow_thickness -
		LabG_MarginHeight(new_w) -
		  LabG_MarginRight(new_w) + LABELG_ACC_PAD;
      }
      }
    }

  if (request->rectangle.height == 0)
    new_w->rectangle.height =
      MAX(TBG_IndicatorDim(new_w),
	  LabG_TextRect(new_w).height + 2 * LabG_MarginHeight(new_w) +
	  LabG_MarginTop(new_w) + LabG_MarginBottom(new_w)) +
	    2 * (new_w->gadget.highlight_thickness +
		 new_w->gadget.shadow_thickness);

  LabG_TextRect(new_w).y = new_w->gadget.highlight_thickness
    + new_w->gadget.shadow_thickness
      + LabG_MarginHeight(new_w) + LabG_MarginTop(new_w) +
	((new_w->rectangle.height - LabG_MarginTop(new_w)
	  - LabG_MarginBottom(new_w)
	  - (2 * (LabG_MarginHeight(new_w)
		  + new_w->gadget.highlight_thickness
		  + new_w->gadget.shadow_thickness))
	  - LabG_TextRect(new_w).height) / 2);

  if (new_w->rectangle.height == 0)
    new_w->rectangle.height = 1;


  /* Display toggles as set if XmNset is True initially. */
    IsOn(new_w) = TBG_Set(new_w);

  /* BEGIN OSF Fix pir 2097 */
  new_w->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT |
    XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT | XmHELP_EVENT |
      XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT |
	XmBDRAG_EVENT;
  /* END OSF Fix pir 2097 */

  (* (resize)) ((Widget) new_w);


  /* Unselect same as background unless set. */
  if (TBG_UnselectColor(new_w) == INVALID_PIXEL)
    TBG_UnselectColor(new_w) = LabG_Background(new_w);

  TBG_ReversedSelect(new_w) =
    (TBG_SelectColor(new_w) == XmREVERSED_GROUND_COLORS);

  if ((TBG_SelectColor(new_w) == INVALID_PIXEL) ||
      (TBG_SelectColor(new_w) == XmDEFAULT_SELECT_COLOR))
    {
      XrmValue value;
      value.size = sizeof(Pixel);

      DefaultSelectColor((Widget) new_w,
			 XtOffsetOf(XmToggleButtonGCacheObjRec,
				    toggle_cache.select_color),
			 &value);
      memcpy((char*) &TBG_SelectColor(new_w), value.addr, value.size);
    }
  else if (TBG_SelectColor(new_w) == XmREVERSED_GROUND_COLORS)
    {
      TBG_SelectColor(new_w) = LabG_Foreground(new_w);
    }
  else if (TBG_SelectColor(new_w) == XmHIGHLIGHT_COLOR)
    {
      TBG_SelectColor(new_w) = LabG_HighlightColor(new_w);
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
        Widget w )
{
  XmToggleButtonGadget tw = (XmToggleButtonGadget) w;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;


  XtReleaseGC (XtParent(tw), TBG_SelectGC(tw));
  XtReleaseGC (XtParent(tw), TBG_BackgroundGC(tw));
  XtReleaseGC (XtParent(tw), TBG_UnselectGC(tw));
  XtReleaseGC (XtParent(tw), TBG_IndeterminateGC(tw));
  XtReleaseGC (XtParent(tw), TBG_IndeterminateBoxGC(tw));
  if (LabG_IsMenupane(tw) && etched_in)
      XtReleaseGC ((Widget) tw, TBG_ArmGC(tw));

  _XmProcessLock();
  _XmCacheDelete( (XtPointer) TBG_Cache(tw));
  _XmProcessUnlock();
}

static void
DrawBox(XmToggleButtonGadget w,
	GC top_gc,
	GC bot_gc,
	GC fillgc,
	int x,
	int y,
	int edge,
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

  if (DefaultDepthOfScreen(XtScreen(w)) == 1 && DRAWGLYPH(NormalizeIndOn(w)))
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
 *  DrawToggle
 *     Depending on the state of this widget, draw the ToggleButton.
 *
 *************************************<->***********************************/
static void
DrawToggle(
        XmToggleButtonGadget w )
{
  int x, y, edge;
  Dimension margin;
  XGCValues values;
  Display *dpy = XtDisplay((Widget) w);
  Drawable drawable = XtWindow(XtParent((Widget) w));
  XmDisplay dpyxm = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean etched_in = dpyxm->display.enable_etched_in_menu;

  if (LabG_IsPixmap(w) || XmStringEmpty(LabG__label(w)))
    edge = TBG_IndicatorDim(w);
  else
    edge =
      MIN((int)TBG_IndicatorDim(w),
	  MAX(0,
	      (int)w->rectangle.height -
	      2 * (w->gadget.highlight_thickness + w->gadget.shadow_thickness +
		   (int)LabG_MarginHeight(w)) +
	      LabG_MarginTop(w) + LabG_MarginBottom(w)));

  if ((DefaultDepthOfScreen(XtScreen(w)) > 1) &&
      (LabG_TopShadowColor(w) != TBG_SelectColor(w)) &&
      (LabG_BottomShadowColor (w)!= TBG_SelectColor(w)))
    margin = 0;
  else
    margin = 1;

  if (LayoutIsRtoLG(w))
    x = w->rectangle.x + w->rectangle.width -
      w->gadget.highlight_thickness - w->gadget.shadow_thickness -
	LabG_MarginHeight(w) - TBG_IndicatorDim(w);
  else
    x = w->rectangle.x +
      w->gadget.highlight_thickness + w->gadget.shadow_thickness +
	LabG_MarginHeight(w);

  if (LabG_IsPixmap(w) || XmStringEmpty(LabG__label(w)))
    y = w->rectangle.y +
      (int)((w->rectangle.height - TBG_IndicatorDim(w)))/2;
  else
    {
      /* Center indicator vertically next to the first line of text. */
      int fudge = Xm3D_ENHANCE_PIXEL;
      Dimension text_height;
      int line_count, height_diff;

      text_height = XmStringHeight(LabG_Font(w), LabG__label(w));
      if ((line_count = XmStringLineCount(LabG__label(w))) < 1)
	line_count = 1;

      height_diff = ((int)text_height / line_count) - (int)TBG_IndicatorDim(w);
      if (height_diff < 0)
	height_diff = 0;

      y = w->rectangle.y + LabG_TextRect(w).y + (height_diff / 2);

      /* CR 2337: Keep large indicators inside the toggle. */
      /*	Is this definition of fudge right??? */
      if (TBG_IndTopDelta(w) > fudge)
	y -= (TBG_IndTopDelta(w) - fudge);
    }

  if (TBG_Visible(w) || (IsOn(w) == XmSET))
    {
      /* The toggle indicator should be visible. */
      GC top_gc, bot_gc, fill_gc, glyph_gc;
      unsigned char normal_ind_on = NormalizeIndOn(w);

      switch (TBG_VisualSet(w))
	{
	case XmUNSET:
	  top_gc = LabG_TopShadowGC(w);
	  bot_gc = LabG_BottomShadowGC(w);
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (LabG_IsMenupane(w) && etched_in && TBG_Armed(w))
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_UnselectGC(w) : TBG_ArmGC(w));
	  else
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_UnselectGC(w) : LabG_BackgroundGC(w));
	  glyph_gc = None;
	  break;

	case XmSET:
	  top_gc = LabG_BottomShadowGC(w);
	  bot_gc = LabG_TopShadowGC(w);
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (LabG_IsMenupane(w) && etched_in && TBG_Armed(w))
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_SelectGC(w) : TBG_ArmGC(w));
	  else
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_SelectGC(w) : LabG_BackgroundGC(w));
	  glyph_gc = ((TBG_ReversedSelect(w) && DRAWBOX(normal_ind_on)) ?
		      LabG_BackgroundGC(w) : LabG_NormalGC(w));

	  /* CR 9791: Label's normal_gc has a dynamic clip_mask. */
	  if (glyph_gc == LabG_NormalGC(w))
	    XSetClipMask(dpy, glyph_gc, None);
	  break;

	case XmINDETERMINATE:
	  top_gc = bot_gc = TBG_IndeterminateBoxGC(w);
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
	  if (LabG_IsMenupane(w) && etched_in && TBG_Armed(w))
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_IndeterminateGC(w) : TBG_ArmGC(w));
	  else
	      fill_gc = (TBG_FillOnSelect(w) ?
			 TBG_IndeterminateGC(w) : LabG_BackgroundGC(w));
	  glyph_gc = TBG_IndeterminateGC(w);
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

	    /* Adjust x,y to center the indicator relative to the label. */
	    y = y + ((edge - new_edge) / 2);
	    x = x + ((edge - new_edge) / 2);
	    edge = new_edge;

	    switch(TBG_VisualSet(w))
	      {
	      case XmUNSET:
		if (edge > 0)
		  {
		    if (w->label.fill_bg_box || TBG_FillOnSelect(w))
		      XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);
		    else
		      XClearArea(dpy, drawable, x, y, edge, edge, False);
		  }

		if (DRAW3DBOX(normal_ind_on))
		  DrawBox(w, top_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (DRAWFLATBOX(normal_ind_on))
		  DrawBox(w, bot_gc, bot_gc, fill_gc, x, y, edge, margin);
		break;

	      case XmSET:
		if (DRAW3DBOX(normal_ind_on))
		  DrawBox(w, top_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (DRAWFLATBOX(normal_ind_on))
		  DrawBox(w, top_gc, top_gc, fill_gc, x, y, edge, margin);
		else if (edge > 0)
		  {
		    if (w->label.fill_bg_box || TBG_FillOnSelect(w))
		      XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);
		    else
		      XClearArea(dpy, drawable, x, y, edge, edge, False);
		  }

		if (!DRAWBOX(normal_ind_on) ||
		    ((edge - 2 * box_margin) >= MIN_GLYPH_SIZE))
		  {
		    if (DRAWCHECK(normal_ind_on))
		      XmeDrawIndicator(dpy, drawable, glyph_gc,
				       x, y, edge, edge, box_margin,
				       normal_ind_on);
		    else if (DRAWCROSS(normal_ind_on))
		      XmeDrawIndicator(dpy, drawable, glyph_gc,
				       x, y, edge, edge, box_margin,
				       normal_ind_on);
		  }
		break;

	      case XmINDETERMINATE:
		if (TBG_FillOnSelect(w))
		  {
		    /* Fetch the select_color GetGC() actually used. */
		    XGetGCValues(dpy, TBG_SelectGC(w), GCForeground, &values);
		    values.background = values.foreground;
		    values.foreground = TBG_UnselectColor(w);
		    XChangeGC(dpy, fill_gc,
			      GCForeground|GCBackground, &values);
		  }
		else
		  {
		    /* This GC should have the right values already. */
		    fill_gc = TBG_IndeterminateBoxGC(w);
		  }

		if (DRAWBOX(normal_ind_on))
		  DrawBox(w, bot_gc, bot_gc, fill_gc, x, y, edge, margin);
		else if (edge > 0)
		  {
		    if (w->label.fill_bg_box || TBG_FillOnSelect(w))
		      XFillRectangle(dpy, drawable, fill_gc, x, y, edge, edge);
		    else
		      XClearArea(dpy, drawable, x, y, edge, edge, False);
		  }

		if (TBG_ReversedSelect(w))
		  {
		    values.foreground = LabG_Background(w);
		    values.background = LabG_Foreground(w);
		  }
		else
		  {
		    values.foreground = LabG_Foreground(w);
		    values.background = LabG_Background(w);
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
			 edge, edge, w->toggle.detail_shadow_thickness,margin);
	  break;

	case XmONE_OF_MANY_ROUND:
	  XmeDrawCircle(dpy, drawable, top_gc, bot_gc, fill_gc, x, y,
			edge, edge, w->toggle.detail_shadow_thickness, margin);
	  break;
	}
    }
  else
    {
      /* The toggle indicator should be invisibile. */
      if (edge > 0)
	{
	  if (LabG_IsMenupane(w) && etched_in) {
	      if (w->toggle.Armed)
	  /* use the arm GC in a menu if armed and enableEtchedInMenu is set */
		  XFillRectangle(dpy, drawable, TBG_ArmGC(w), x, y,
				 edge, edge);
	      else
		  XFillRectangle(dpy, drawable, TBG_BackgroundGC(w), x, y,
				 edge, edge);
	  }
	  else
	      if (w->label.fill_bg_box)
		  XFillRectangle(dpy, drawable, TBG_BackgroundGC(w), x, y,
				 edge, edge);
	      else
		  XClearArea(dpy, drawable, x, y, edge, edge, False);
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
  XmToggleButtonGadget tb = (XmToggleButtonGadget) wid ;
  XEvent * event = NULL;

  if (LabG_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      TBG_Armed(tb) = TRUE;

      if ((etched_in) &&
	  (TBG_IndOn(tb) ||
	   (!(TBG_IndOn(tb)) && !(TBG_FillOnSelect(tb)))))
	{
	    DrawEtchedInMenu(tb);
	    if (TBG_IndOn(tb))
		DrawToggle(tb);
	}

      XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
		      LabG_TopShadowGC(tb),
		      LabG_BottomShadowGC(tb),
		      tb->rectangle.x + tb->gadget.highlight_thickness,
		      tb->rectangle.y + tb->gadget.highlight_thickness,
		      tb->rectangle.width -
		      2 * tb->gadget.highlight_thickness,
		      tb->rectangle.height -
		      2 * tb->gadget.highlight_thickness,
		      tb->gadget.shadow_thickness,
		      etched_in ? XmSHADOW_IN : XmSHADOW_OUT);


      if (TBG_ArmCB(tb))
	{
	  XFlush (XtDisplay (tb));
	  ToggleButtonCallback(tb, XmCR_ARM, TBG_Set(tb), event);
	}
    }
  else
    {
      (*(xmLabelGadgetClassRec.gadget_class.border_highlight))((Widget) tb) ;
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
  XmToggleButtonGadget tb = (XmToggleButtonGadget) wid ;
  XEvent * event = NULL;

  if (LabG_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      if (!TBG_Armed(tb))
	return;

      TBG_Armed(tb) = FALSE;

      if ((etched_in) &&
	  (TBG_IndOn(tb) ||
	   (!(TBG_IndOn(tb)) && !(TBG_FillOnSelect(tb)))))
        {
	    DrawEtchedInMenu(tb);
	    if (TBG_IndOn(tb))
		DrawToggle(tb);
	}

      XmeClearBorder (XtDisplay (tb), XtWindow (tb),
		      tb->rectangle.x + tb->gadget.highlight_thickness,
		      tb->rectangle.y + tb->gadget.highlight_thickness,
		      tb->rectangle.width -
		      2 * tb->gadget.highlight_thickness,
		      tb->rectangle.height -
		      2 * tb->gadget.highlight_thickness,
		      tb->gadget.shadow_thickness);

      if (TBG_DisarmCB(tb))
	{
	  XFlush (XtDisplay (tb));
	  ToggleButtonCallback(tb, XmCR_DISARM, TBG_Set(tb), event);
	}
    }
  else
    {
      (*(xmLabelGadgetClassRec.gadget_class.border_unhighlight)) ((Widget) tb) ;
    }
}

/*************************************<->*************************************
 *
 *  KeySelect
 *    If the menu system traversal is enabled, do an activate and disarm
 *
 *************************************<->***********************************/

static void
KeySelect(
        XmToggleButtonGadget tb,
        XEvent *event )
{
  XmToggleButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;
  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(tb)), XmQTmenuSystem);
  if (menuSTrait == NULL) return;

  if (!_XmIsEventUnique(event))
    return;

  if (!_XmGetInDragMode((Widget)tb))
    {
      if (TBG_IndOn(tb))
	DrawToggle(tb);
      else if (TBG_FillOnSelect(tb) && !LabG_IsPixmap(tb))
	DrawToggleLabel (tb);

      if (LabG_IsPixmap(tb) || LabG_IsPixmapAndText(tb))
	SetAndDisplayPixmap( tb, NULL, NULL);

      TBG_Armed(tb) = FALSE;
      TBG_Set(tb) = (TBG_Set(tb) == TRUE) ? FALSE : TRUE;

      menuSTrait->buttonPopdown(XtParent(tb), event);

      _XmRecordEvent(event);

      /* UNDOING this fix ... */
      /* CR 8904: Notify value_changed before entry so that state is */
      /*	reported correctly even if the entry callback resets it. */

      /* Notify parent about the select. */
      call_value.reason = XmCR_VALUE_CHANGED;
      call_value.event = event;
      call_value.set = TBG_Set(tb);
      menuSTrait->entryCallback(XtParent(tb), (Widget) tb, &call_value);

      menuSTrait->reparentToTearOffShell(XtParent(tb), event);

      if ((! LabG_SkipCallback(tb)) &&
	  (TBG_ValueChangedCB(tb)))
	{
	  XFlush(XtDisplay(tb));
	  ToggleButtonCallback(tb, XmCR_VALUE_CHANGED, TBG_Set(tb), event);
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
        XmToggleButtonGadget tb )
{
  int needed_width;
  int needed_height;

  /* Compute space for drawing toggle. */
  needed_width = LabG_TextRect(tb).width +
    LabG_MarginLeft(tb) + LabG_MarginRight(tb) +
      (2 * (tb->gadget.shadow_thickness +
	    tb->gadget.highlight_thickness +
	    LabG_MarginWidth(tb)));

  needed_height = LabG_TextRect(tb).height +
    LabG_MarginTop(tb) + LabG_MarginBottom(tb) +
      (2 * (tb->gadget.shadow_thickness +
	    tb->gadget.highlight_thickness +
	    LabG_MarginHeight(tb)));

  if (needed_height > tb->rectangle.height)
    if (TBG_IndOn(tb))
      LabG_TextRect(tb).y = tb->gadget.shadow_thickness +
	tb->gadget.highlight_thickness +
	  LabG_MarginHeight(tb) +
	    LabG_MarginTop(tb) +
	      ((tb->rectangle.height - LabG_MarginTop(tb)
		- LabG_MarginBottom(tb)
		- (2 * (LabG_MarginHeight(tb)
			+ tb->gadget.highlight_thickness
			+ tb->gadget.shadow_thickness))
		- LabG_TextRect(tb).height) / 2);

  if (LayoutIsRtoLG(tb))
    {
      if ((needed_width > tb->rectangle.width) ||
	  ((LabG_Alignment(tb) == XmALIGNMENT_BEGINNING)
	   && (needed_width < tb->rectangle.width)) ||
	  ((LabG_Alignment(tb) == XmALIGNMENT_CENTER)
	   && (needed_width < tb->rectangle.width)
	   && (tb->rectangle.width - needed_width < LabG_MarginRight(tb))) ||
	  (needed_width == tb->rectangle.width))
	{
	  if (TBG_IndOn(tb))
	    LabG_TextRect(tb).x = tb->rectangle.x + tb->rectangle.width -
	      (tb->gadget.shadow_thickness +
	       tb->gadget.highlight_thickness +
	       LabG_MarginHeight(tb) +
	       LabG_MarginRight(tb) +
	       LabG_TextRect(tb).width);
	}
    }
  else
    {
      if ((needed_width > tb->rectangle.width) ||
	  ((LabG_Alignment(tb) == XmALIGNMENT_BEGINNING)
	   && (needed_width < tb->rectangle.width)) ||
	  ((LabG_Alignment(tb) == XmALIGNMENT_CENTER)
	   && (needed_width < tb->rectangle.width)
	   && (tb->rectangle.width - needed_width < LabG_MarginLeft(tb))) ||
	  (needed_width == tb->rectangle.width))
	{
	  if (TBG_IndOn(tb))
	    LabG_TextRect(tb).x =  tb->gadget.shadow_thickness +
	      tb->gadget.highlight_thickness +
		LabG_MarginHeight(tb) +
		  LabG_MarginLeft(tb);
	}
    }
}

/*************************************<->*************************************
 *
 *  Redisplay(w, event, region)
 *     Cause the widget, identified by w, to be redisplayed.
 *
 *************************************<->***********************************/
static void
Redisplay(
        Widget w,
        XEvent *event,
        Region region )
{
  register XmToggleButtonGadget tb = (XmToggleButtonGadget) w;

  /* Fix CR #4884, D. Rand 6/4/92 */
  if (! XtIsRealized(w) ) return;
  /* End Fix */

  if (LabG_IsMenupane(tb))
    {
      ShellWidget mshell = (ShellWidget) XtParent(XtParent(tb));
      if (! mshell->shell.popped_up)
	return;
    }

  ComputeSpace(tb);

  if (LabG_IsPixmap (tb) || LabG_IsPixmapAndText(tb))
    SetAndDisplayPixmap(tb, event, region);
  else
    {
      if (!TBG_IndOn(tb) && TBG_FillOnSelect(tb))
	DrawToggleLabel (tb);
      else {
	XtExposeProc expose;
	_XmProcessLock();
	expose = xmLabelGadgetClassRec.rect_class.expose;
	_XmProcessUnlock();
	(* expose) ((Widget)tb, event, region);
      }
    }


  if (TBG_IndOn(tb))
    {
      if (! TBG_Armed(tb))
	IsOn(tb) = TBG_Set(tb);
      DrawToggle(tb);
    }

  if (LabG_IsMenupane(tb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
      Boolean etched_in = dpy -> display.enable_etched_in_menu;

      if (TBG_Armed(tb))
	XmeDrawShadows(XtDisplay (tb), XtWindow (tb),
		       LabG_TopShadowGC(tb),
		       LabG_BottomShadowGC(tb),
		       tb->rectangle.x + tb->gadget.highlight_thickness,
		       tb->rectangle.y + tb->gadget.highlight_thickness,
		       tb->rectangle.width - 2*tb->gadget.highlight_thickness,
		       tb->rectangle.height - 2*tb->gadget.highlight_thickness,
		       tb->gadget.shadow_thickness,
		       etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
    }
  else
    {
      DrawToggleShadow (tb);
    }
}

/**************************************************************************
 * Resize(w, event)
 **************************************************************************/
static void
Resize(
        Widget w )
{
  register XmToggleButtonGadget tb = (XmToggleButtonGadget) w;

  if (LabG_IsPixmap(w))
    SetToggleSize(tb);
  else {
    XtWidgetProc resize;
    _XmProcessLock();
    resize = xmLabelGadgetClassRec.rect_class.resize;
    _XmProcessUnlock();
    (* resize)( (Widget) tb);
  }
}

/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/
 /* ARGSUSED */
static Boolean
SetValuesPrehook(
        Widget oldParent,
        Widget refParent,
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData             extData;
  XmBaseClassExt              *cePtr;
  WidgetClass                 ec;
  Cardinal			size;
  XmToggleButtonGCacheObject  newSec, reqSec;

  _XmProcessLock();
  cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
  ec = (*cePtr)->secondaryObjectClass;
  size = ec->core_class.widget_size;

  newSec = (XmToggleButtonGCacheObject)_XmExtObjAlloc(size);
  reqSec = (XmToggleButtonGCacheObject)_XmExtObjAlloc(size);
  _XmProcessUnlock();

  newSec->object.self = (Widget)newSec;
  newSec->object.widget_class = ec;
  newSec->object.parent = XtParent(newParent);
  newSec->object.xrm_name = newParent->core.xrm_name;
  newSec->object.being_destroyed = False;
  newSec->object.destroy_callbacks = NULL;
  newSec->object.constraints = NULL;

  newSec->ext.logicalParent = newParent;
  newSec->ext.extensionType = XmCACHE_EXTENSION;

  memcpy (&(newSec->label_cache),
	  LabG_Cache(newParent),
	  sizeof(XmLabelGCacheObjPart));

  memcpy(&(newSec->toggle_cache),
	 TBG_Cache(newParent),
	 sizeof(XmToggleButtonGCacheObjPart));

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;
  _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

  /*
   * Since the resource lists for label and togglebutton were merged at
   * ClassInitialize time we need to make only one call to
   * XtSetSubvalues()
   */

  XtSetSubvalues((XtPointer)newSec,
		 ec->core_class.resources,
		 ec->core_class.num_resources,
		 args, *num_args);

  memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

  LabG_Cache(newParent) = &(((XmLabelGCacheObject)newSec)->label_cache);
  LabG_Cache(refParent) = &(((XmLabelGCacheObject)
			     extData->reqWidget)->label_cache);

  TBG_Cache(newParent) =
    &(((XmToggleButtonGCacheObject)newSec)->toggle_cache);
  TBG_Cache(refParent) =
    &(((XmToggleButtonGCacheObject)extData->reqWidget)->toggle_cache);

  _XmExtImportArgs((Widget)newSec, args, num_args);

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (LabG_Font(newParent) == NULL)
    LabG_Font(newParent) = XmeGetDefaultRenderTable (newParent,
						     XmBUTTON_FONTLIST);

  return FALSE;
}

/************************************************************************
 *
 *  GetValuesPrehook
 *
 ************************************************************************/

/* ARGSUSED */
static void
GetValuesPrehook(
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData             extData;
  XmBaseClassExt              *cePtr;
  WidgetClass                 ec;
  Cardinal                    size;
  XmToggleButtonGCacheObject  newSec;

  _XmProcessLock();
  cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
  ec = (*cePtr)->secondaryObjectClass;
  size = ec->core_class.widget_size;

  newSec = (XmToggleButtonGCacheObject)_XmExtObjAlloc(size);
  _XmProcessUnlock();

  newSec->object.self = (Widget)newSec;
  newSec->object.widget_class = ec;
  newSec->object.parent = XtParent(newParent);
  newSec->object.xrm_name = newParent->core.xrm_name;
  newSec->object.being_destroyed = False;
  newSec->object.destroy_callbacks = NULL;
  newSec->object.constraints = NULL;

  newSec->ext.logicalParent = newParent;
  newSec->ext.extensionType = XmCACHE_EXTENSION;

  memcpy (&(newSec->label_cache),
	  LabG_Cache(newParent),
	  sizeof(XmLabelGCacheObjPart));

  memcpy (&(newSec->toggle_cache),
	  TBG_Cache(newParent),
	  sizeof(XmToggleButtonGCacheObjPart));

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

  /*
   * Note that if a resource is defined in the superclass's as well as a
   * subclass's resource list and if a NULL is passed in as the third
   * argument to XtSetArg, then when a GetSubValues() is done by the
   * superclass the NULL is replaced by a value. Now when the subclass
   * gets the arglist it doesn't see a NULL and thinks it's an address
   * it needs to stuff a value into and sure enough it breaks.
   * This means that we have to pass the same arglist with the NULL to
   * both the superclass and subclass and propagate the values up once
   * the XtGetSubValues() are done.
   */

  /*
   * Since the resource lists for label and togglebutton were merged at
   * ClassInitialize time we need to make only one call to
   * XtGetSubvalues()
   */

  XtGetSubvalues((XtPointer)newSec,
		 ec->core_class.resources,
		 ec->core_class.num_resources,
		 args, *num_args);

  _XmExtGetValuesHook((Widget)newSec, args, num_args);
}

/************************************************************************
 *
 *  GetValuesPosthook
 *
 ************************************************************************/

/*ARGSUSED*/
static void
GetValuesPosthook(
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmWidgetExtData             ext;

  _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

  _XmProcessLock();
  _XmExtObjFree((XtPointer) ext->widget);
  _XmProcessUnlock();
  XtFree( (char *) ext);
}

/************************************************************************
 *
 *  SetValuesPosthook
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
SetValuesPosthook(
        Widget current,
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmWidgetExtData             ext;

  /*
   * - register parts in cache.
   * - update cache pointers
   * - and free req
   */

  /* assign if changed! */
  _XmProcessLock();
  if (!_XmLabelCacheCompare((XtPointer)LabG_Cache(new_w),
			    (XtPointer)LabG_Cache(current)))
    {                         /* delete the old one */
      _XmCacheDelete( (XtPointer) LabG_Cache(current));
      LabG_Cache(new_w) = (XmLabelGCacheObjPart *)
	_XmCachePart(LabG_ClassCachePart(new_w),
		     (XtPointer) LabG_Cache(new_w),
		     sizeof(XmLabelGCacheObjPart));
    }
  else
    LabG_Cache(new_w) = LabG_Cache(current);

  /* assign if changed! */
  if (!_XmToggleBCacheCompare((XtPointer)TBG_Cache(new_w),
			      (XtPointer)TBG_Cache(current)))
    {                                      /* delete the old one */
      _XmCacheDelete( (XtPointer) TBG_Cache(current));
      TBG_Cache(new_w) = (XmToggleButtonGCacheObjPart *)
	_XmCachePart(TBG_ClassCachePart(new_w),
		     (XtPointer) TBG_Cache(new_w),
		     sizeof(XmToggleButtonGCacheObjPart));
    }
  else
    TBG_Cache(new_w) = TBG_Cache(current);

  _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

  _XmExtObjFree((XtPointer) ext->widget);
  _XmExtObjFree((XtPointer) ext->reqWidget);
  _XmProcessUnlock();
  XtFree( (char *) ext);

  return FALSE;
}

/***************************************************************************
 *
 *  SetValues(current, request, new_w)
 *     This is the set values procedure for the ToggleButton class.  It is
 *     called last (the set values rtnes for its superclasses are called
 *     first).
 *
 *************************************<->***********************************/

/*ARGSUSED*/
static Boolean
SetValues(
        Widget current,
        Widget request,
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmToggleButtonGadget curcbox = (XmToggleButtonGadget) current;
  XmToggleButtonGadget reqcbox = (XmToggleButtonGadget) request;
  XmToggleButtonGadget newcbox = (XmToggleButtonGadget) new_w;
  Boolean  flag = FALSE;    /* our return value */
  XtWidgetProc resize;


  /* CR 2337: Preserve the user's margins. */
  if (LabG_MarginRight(curcbox) != LabG_MarginRight(reqcbox))
    TBG_IndRightDelta(newcbox) = 0;
  if (LabG_MarginLeft(curcbox) != LabG_MarginLeft(reqcbox))
    TBG_IndLeftDelta(newcbox) = 0;
  if (LabG_MarginTop(curcbox) != LabG_MarginTop(reqcbox))
    TBG_IndTopDelta(newcbox) = 0;
  if (LabG_MarginBottom(curcbox) != LabG_MarginBottom(reqcbox))
    TBG_IndBottomDelta(newcbox) = 0;


  /**********************************************************************
   * Calculate the window size:  The assumption here is that if
   * the width and height are the same in the new and current instance
   * record that those fields were not changed with set values.  Therefore
   * its okay to recompute the necessary width and height.  However, if
   * the new and current do have different width/heights then leave them
   * alone because that's what the user wants.
   *********************************************************************/
   _XmProcessLock();
   resize = xmLabelGadgetClassRec.rect_class.resize;
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
      if ((LabG_RecomputeSize(newcbox)) &&
	  (request->core.width == current->core.width))
	new_w->core.width = 0;
      if ((LabG_RecomputeSize(newcbox)) &&
	  (request->core.height == current->core.height))
	new_w->core.height = 0;

      _XmCalcLabelGDimensions(new_w);
      (* resize)( (Widget) newcbox);
    }

  /* Use the insensitive On pixmap if no insensitive Off pixmap is found. */
  if (IsNull(PixmapInsenOff(newcbox)) && !IsNull(PixmapInsenOn(newcbox)))
    {
      PixmapInsenOff(newcbox) = PixmapInsenOn(newcbox);
      if ((LabG_RecomputeSize(newcbox)) &&
	  (request->core.width == current->core.width))
	new_w->core.width = 0;
      if ((LabG_RecomputeSize(newcbox)) &&
	  (request->core.height == current->core.height))
	new_w->core.height = 0;

      _XmCalcLabelGDimensions(new_w);
      (* resize)( (Widget) newcbox);
    }

  /* BEGIN OSF Fix pir 1778 */
  /* Have to reset the TextRect width because label's resize will have
     mucked with it. */
  if (LabG_IsPixmap(newcbox) &&
      (!IsNull(PixmapOff(newcbox)) || !IsNull(PixmapInsenOff(newcbox)) ||
       !IsNull(PixmapOn(newcbox)) || !IsNull(PixmapInsenOn(newcbox)) ||
       !IsNull(PixmapInd(newcbox)) || !IsNull(PixmapInsenInd(newcbox))))
    {
      if (LabG_RecomputeSize(newcbox))
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
  if (TBG_FillOnSelect(newcbox) != TBG_FillOnSelect(curcbox))
    {
      flag = TRUE;
    }

  if ((LabG__label(newcbox) != LabG__label(curcbox)) ||
      (PixmapOff(newcbox) != PixmapOff(curcbox)) ||
      (LabG_Font(newcbox) != LabG_Font(curcbox)) ||
      (TBG_Spacing(newcbox) != TBG_Spacing(curcbox)) ||
      (PixmapOn(newcbox) != PixmapOn(curcbox)) ||
      (PixmapInsenOn(newcbox) != PixmapInsenOn(curcbox)) ||
      (PixmapInd(newcbox) != PixmapInd(curcbox)) ||
      (PixmapInsenInd(newcbox) != PixmapInsenInd(curcbox)) ||
      (TBG_IndOn(newcbox) != TBG_IndOn(curcbox)) ||
      (TBG_IndicatorDim(newcbox) != TBG_IndicatorDim(curcbox)) ||
      (LabG_IsPixmap(newcbox) != LabG_IsPixmap(curcbox)) ||
      (LabG_IsPixmapAndText(newcbox) != LabG_IsPixmapAndText(curcbox)))
    {
      int right_delta = 0;	/* Our desired margin adjustments. */
      int left_delta = 0;
      int top_delta = 0;
      int bottom_delta = 0;

      if (LabG_RecomputeSize(newcbox))
	{
	  if (reqcbox->rectangle.width == curcbox->rectangle.width)
            newcbox->rectangle.width = 0;
	  if (reqcbox->rectangle.height == curcbox->rectangle.height)
            newcbox->rectangle.height = 0;
	}

      if (LabG_IsPixmap(newcbox) &&
	  ((PixmapOn(newcbox) != PixmapOn(curcbox)) ||
	  (PixmapInsenOn(newcbox) != PixmapInsenOn(curcbox)) ||
	  (PixmapInd(newcbox) != PixmapInd(curcbox)) ||
	  (PixmapInsenInd(newcbox) != PixmapInsenInd(curcbox))) )
	{
          _XmCalcLabelGDimensions(new_w);

	  /* OSF Fix 1778 */
          SetToggleSize(newcbox);
	}

	TBG_IndicatorSet(newcbox) = TBG_IndicatorSet(curcbox);

	if ( TBG_IndicatorDim(newcbox) !=  TBG_IndicatorDim(curcbox))
	  TBG_IndicatorSet(newcbox) = TRUE;

      /* CR 8415: Honor explicit requests for XmNindicatorSize. */
      if (!TBG_IndicatorSet(newcbox) &&
	  (TBG_IndicatorDim(newcbox) == TBG_IndicatorDim(curcbox)))
	{
	  if ((LabG_IsPixmap(newcbox) != LabG_IsPixmap(curcbox)) ||
	      (LabG_IsPixmapAndText(newcbox) != LabG_IsPixmapAndText(curcbox)) ||
	      (LabG__label(newcbox) != LabG__label(curcbox)) ||
	      (PixmapOff(newcbox) != PixmapOff(curcbox)) ||
	      (LabG_Font(newcbox) != LabG_Font(curcbox)) ||
	      (TBG_IndOn(newcbox) != TBG_IndOn(curcbox)))
	    {
	      if (LabG_IsPixmap(newcbox) || LabG_IsPixmapAndText(newcbox))
		{
                if (!IsNull(PixmapOff(newcbox)) || !IsNull(PixmapInsenOff(newcbox)) ||
                 !IsNull(PixmapOn(newcbox)) || !IsNull(PixmapInsenOn(newcbox)) ||
                 !IsNull(PixmapInd(newcbox)) || !IsNull(PixmapInsenInd(newcbox)))
		  if (LabG_TextRect(newcbox).height < 13)
		    TBG_IndicatorDim(newcbox) = LabG_TextRect(newcbox).height;
		  else
		    TBG_IndicatorDim(newcbox) = 13 +
		      (LabG_TextRect(newcbox).height/13);
                else
	          TBG_IndicatorDim(newcbox) = XmDEFAULT_INDICATOR_DIM;
		}
	      else
		{
		  Dimension height;
		  int line_count;

		  height = XmStringHeight(LabG_Font(newcbox),
					  LabG__label(newcbox));
		  line_count = XmStringLineCount (LabG__label(newcbox));
		  if (line_count < 1)
		    line_count = 1;

		  /*
		   * CR 5203 - Make the calculation for the
		   *     indicator_dim be the same as in the Initialize
		   *     procedure, i.e. Popup and Pulldown menus should
		   *     have smaller indicators
		   */
		  if (LabG_IsMenupane(new_w))
		    TBG_IndicatorDim(newcbox) =
		      MAX(XmDEFAULT_INDICATOR_DIM,
			  (height / ((Dimension)line_count))*2/3);
		  else
		    TBG_IndicatorDim(newcbox) =
		      MAX(XmDEFAULT_INDICATOR_DIM,
			  height / ((Dimension)line_count));
		}
	      /* End 5203 Fix */
	    }
	}


      if (TBG_IndOn(newcbox))
	{
	  /*
	   * Fix CR 5568 - If the indicator is on and the user has changed the
	   *             indicator dimension, calculate the new top and bottom
	   *             margins in a place where they can effect the core width
	   *             and height.
	   */
	  /*  Recompute the Top and bottom margins and the height of the text
	   *      rectangle to  accommodate the size of toggle indicator.
	   *  if (we are given a new toggleIndicator size)
	   *     { if (user has given new top or bottom margin)
	   *         { compute to accomodate new toggle button size; }
	   *       else (user has set new top/bottom margin)
	   *         { Recompute margin to accommodate new IndicatorSize }
	   *     }
	   */
	  if (TBG_IndicatorDim(newcbox) != TBG_IndicatorDim(curcbox))
	    {
	      int maxIndicatorSize =
		(int) (TBG_IndicatorDim(newcbox)) + 2*Xm3D_ENHANCE_PIXEL;

	      int boxSize = ((int) LabG_TextRect(newcbox).height +
			 (int) LabG_MarginTop(newcbox) +
			 (int) LabG_MarginBottom(newcbox));

	      top_delta = bottom_delta = (maxIndicatorSize - boxSize) / 2;
	    }

	  if (LayoutIsRtoLG(newcbox))
	    right_delta = (TBG_IndicatorDim(newcbox) +
			   TBG_Spacing(newcbox) -
			   LabG_MarginRight(newcbox));
          else
	    left_delta = (TBG_IndicatorDim(newcbox) +
			  TBG_Spacing(newcbox) -
			  LabG_MarginLeft(newcbox));
	}
      else if (TBG_IndOn(curcbox))
	{
	  /* CR 2337: Redisplay when the indicator is turned off. */
	  flag = TRUE;

	  top_delta = -TBG_IndTopDelta(newcbox);
	  bottom_delta = -TBG_IndBottomDelta(newcbox);

          if (LayoutIsRtoLG(newcbox))
	    right_delta = -TBG_IndRightDelta(newcbox);
          else
	    left_delta = -TBG_IndLeftDelta(newcbox);
	}

      /* CR 2337: Let the toggle button shrink if necessary. */
      if (right_delta || left_delta || top_delta || bottom_delta)
	{
	  flag = TRUE;

	  /* Adjust vertical margins based on the indicator. */
	  if ((int)TBG_IndTopDelta(newcbox) + top_delta > 0)
	    {
	      LabG_MarginTop(newcbox) += top_delta;
	      TBG_IndTopDelta(newcbox) += top_delta;
	    }
	  else
	    {
	      LabG_MarginTop(newcbox) -= TBG_IndTopDelta(newcbox);
	      TBG_IndTopDelta(newcbox) = 0;
	    }

	  if ((int)TBG_IndBottomDelta(newcbox) + bottom_delta > 0)
	    {
	      LabG_MarginBottom(newcbox) += bottom_delta;
	      TBG_IndBottomDelta(newcbox) += bottom_delta;
	    }
	  else
	    {
	      LabG_MarginBottom(newcbox) -= TBG_IndBottomDelta(newcbox);
	      TBG_IndBottomDelta(newcbox) = 0;
	    }

	  /* Adjust horizontal margins based on the indicator. */
	  if (LayoutIsRtoLG(newcbox))
	    {
	      if ((int)TBG_IndRightDelta(newcbox) + right_delta > 0)
		{
		  LabG_MarginRight(newcbox) += right_delta;
		  TBG_IndRightDelta(newcbox) += right_delta;
		}
	      else
		{
		  LabG_MarginRight(newcbox) -= TBG_IndRightDelta(newcbox);
		  TBG_IndRightDelta(newcbox) = 0;
		}
	    }
	  else
	    {
	      if ((int)TBG_IndLeftDelta(newcbox) + left_delta > 0)
		{
		  LabG_MarginLeft(newcbox) += left_delta;
		  TBG_IndLeftDelta(newcbox) += left_delta;
		}
	      else
		{
		  LabG_MarginLeft(newcbox) -= TBG_IndLeftDelta(newcbox);
		  TBG_IndLeftDelta(newcbox) = 0;
		}
	    }

	  /* Realign the label. */
	  if (! LabG_RecomputeSize(newcbox))
	    (* resize) ((Widget) new_w);
	}

      if (LabG_RecomputeSize(newcbox))
	{
	  if (reqcbox->rectangle.width == curcbox->rectangle.width)
            newcbox->rectangle.width = 0;
	  if (reqcbox->rectangle.height == curcbox->rectangle.height)
            newcbox->rectangle.height = 0;
	}

      if (newcbox->rectangle.width == 0)
	{
	  newcbox->rectangle.width =
	    LabG_TextRect(newcbox).width +
	      LabG_MarginLeft(newcbox) + LabG_MarginRight(newcbox) +
		2 * (newcbox->gadget.highlight_thickness +
		     newcbox->gadget.shadow_thickness +
		     LabG_MarginHeight(newcbox));

	  if (newcbox->rectangle.width == 0)
            newcbox->rectangle.width = 1;

	  flag = TRUE;
	}

      if (newcbox->rectangle.height == 0)
	{
	  newcbox->rectangle.height =
	    MAX(TBG_IndicatorDim(newcbox),
		LabG_TextRect(newcbox).height + 2*LabG_MarginHeight(newcbox) +
		LabG_MarginTop(newcbox) + LabG_MarginBottom(newcbox)) +
		  2 * (newcbox->gadget.highlight_thickness +
		       newcbox->gadget.shadow_thickness);

	  if (newcbox->rectangle.height == 0)
            newcbox->rectangle.height = 1;

	  flag = TRUE;
	}
    }

  if ((TBG_IndType(curcbox) != TBG_IndType(newcbox)) &&
      (!XmRepTypeValidValue(XmRID_INDICATOR_TYPE,
			    TBG_IndType( newcbox), (Widget) newcbox)))
    {
      TBG_IndType(newcbox) = TBG_IndType(curcbox);
    }

  if ((LabG_Foreground(newcbox) != LabG_Foreground(curcbox)) ||
      (LabG_Background(newcbox) != LabG_Background(curcbox)) ||
      (TBG_SelectColor(newcbox) != TBG_SelectColor(curcbox)))
    {
	XtReleaseGC (XtParent(newcbox), TBG_SelectGC(newcbox));
	XtReleaseGC (XtParent(newcbox), TBG_BackgroundGC(newcbox));
	XtReleaseGC (XtParent(newcbox), TBG_IndeterminateGC(newcbox));
	XtReleaseGC (XtParent(newcbox), TBG_IndeterminateBoxGC(newcbox));

	TBG_ReversedSelect(newcbox) =
	  (TBG_SelectColor(newcbox) == XmREVERSED_GROUND_COLORS);

	if ((TBG_SelectColor(newcbox) == INVALID_PIXEL) ||
	    (TBG_SelectColor(newcbox) == XmDEFAULT_SELECT_COLOR))
	  {
	    XrmValue value;
	    value.size = sizeof(Pixel);

	    DefaultSelectColor((Widget) newcbox,
			       XtOffsetOf(XmToggleButtonGCacheObjRec,
					  toggle_cache.select_color),
			       &value);
	    memcpy((char*) &TBG_SelectColor(newcbox), value.addr, value.size);
	  }
	else if (TBG_SelectColor(newcbox) == XmREVERSED_GROUND_COLORS)
	  {
	    TBG_SelectColor(newcbox) = LabG_Foreground(newcbox);
	  }
	else if (TBG_SelectColor(newcbox) == XmHIGHLIGHT_COLOR)
	  {
	    TBG_SelectColor(newcbox) = LabG_HighlightColor(newcbox);
	  }

	GetGC(newcbox);
	flag = TRUE;
      }

  if (TBG_UnselectColor(newcbox) != TBG_UnselectColor(curcbox))
    {
      XtReleaseGC (XtParent(curcbox), TBG_UnselectGC(curcbox));
      GetUnselectGC(newcbox);
      flag = TRUE;
    }

  if (TBG_Set(curcbox) != TBG_Set(newcbox))
    {
      if (TBG_ToggleMode(newcbox) == XmTOGGLE_BOOLEAN &&
	  TBG_Set(newcbox) == XmINDETERMINATE)
	TBG_Set(newcbox) = TBG_Set(curcbox);
      else
	{
	  IsOn(newcbox) = TBG_Set(newcbox);
	  if (flag == False && XtIsRealized((Widget)newcbox))
	     {
	        if (TBG_IndOn(newcbox))
			DrawToggle (newcbox);
		else
		   {
			if (newcbox->gadget.shadow_thickness > 0)
				DrawToggleShadow (newcbox);
			if (TBG_FillOnSelect(newcbox)&& !LabG_IsPixmap(newcbox))
				DrawToggleLabel (newcbox);
			if ((LabG_IsPixmap(newcbox)) || LabG_IsPixmapAndText(newcbox))
				{
				SetAndDisplayPixmap(newcbox, NULL, NULL);
				flag = True;
				}
		   }
	    }
	}
    }

  if ((TBG_IndType(curcbox) != TBG_IndType(newcbox)) ||
      ( (TBG_Visible(curcbox) != TBG_Visible(newcbox)) && (XmUNSET == TBG_Set(newcbox))))
    {
      flag = True;
    }

  /* One-of-many forces boolean mode. */
  if (IsOneOfMany(TBG_IndType(newcbox)) &&
      (TBG_ToggleMode(newcbox) == XmTOGGLE_INDETERMINATE))
    {
      TBG_ToggleMode(newcbox) = XmTOGGLE_BOOLEAN;
    }

  if (TBG_ToggleMode(curcbox) != TBG_ToggleMode(newcbox))
    {
      if ((TBG_ToggleMode(newcbox) == XmTOGGLE_BOOLEAN) &&
	  (TBG_Set(newcbox) == XmINDETERMINATE))
	{
	  TBG_VisualSet(newcbox) = TBG_Set(newcbox) = False;
	  flag = True;
	}
    }

    /* Fix br 1211:  we need to compare and set label cache of new_w
       if differs from current.*/
    if(!_XmLabelCacheCompare(LabG_Cache(new_w), LabG_Cache(current)))
    {
	_XmCacheCopy((XtPointer)LabG_Cache(new_w),
	    (XtPointer)LabG_Cache(current), sizeof(XmLabelGCacheObjPart));
    }
    /* End of fix br 1251 */

  /* BEGIN OSF Fix pir 2097 */
  newcbox->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT |
    XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT | XmHELP_EVENT |
      XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT |
	XmBDRAG_EVENT;
  /* END OSF Fix pir 2097 */

  return(flag);
}

/***************************************************************
 *
 * XmToggleButtonGadgetGetState
 *    This function gets the state of the toggle gadget.
 *
 ***************************************************************/

Boolean
XmToggleButtonGadgetGetState(
        Widget w )
{
  XmToggleButtonGadget tg = (XmToggleButtonGadget) w;
  Boolean ret_val;
  XtAppContext app = XtWidgetToApplicationContext(w);

  _XmAppLock(app);
  ret_val = TBG_Set(tg);
  _XmAppUnlock(app);

  return (ret_val);
}

/****************************************************************
 *
 * XmToggleButtonGadgetSetState
 *    This function sets the state of the toggle gadget.
 *
 ****************************************************************/

void
XmToggleButtonGadgetSetState(
        Widget w,
        Boolean bnewstate,
        Boolean notify )
{
  XmToggleButtonGadget tg = (XmToggleButtonGadget) w;
  XmMenuSystemTrait menuSTrait;
  XmToggleButtonState newstate;

  XtAppContext app = XtWidgetToApplicationContext(w);

  _XmAppLock(app);

  newstate = (bnewstate == XmSET)? XmSET : XmUNSET;

  if (TBG_Set(tg) != newstate)
    {
      TBG_Set(tg) = newstate;
      IsOn(tg) = newstate;
      if (XtIsRealized ((Widget)tg))
	{
	  if (TBG_IndOn(tg))
	    DrawToggle(tg);
	  else
	    {
	      if (tg->gadget.shadow_thickness> 0)
		DrawToggleShadow (tg);
	      if (TBG_FillOnSelect(tg) && !LabG_IsPixmap(tg))
		DrawToggleLabel (tg);
	    }
	  if (LabG_IsPixmap(tg) || LabG_IsPixmapAndText(tg))
	    SetAndDisplayPixmap(tg, NULL, NULL);
	}

      if (notify)
	{
	  /* UNDOING this fix ... */
	  /* CR 8904: Notify value_changed before entry so that state is */
	  /*	reported correctly even if the entry callback resets it. */
	  menuSTrait = (XmMenuSystemTrait)
	    XmeTraitGet((XtPointer) XtClass(XtParent(tg)), XmQTmenuSystem);

	  if (menuSTrait != NULL)
	    {
	      XmToggleButtonCallbackStruct call_value;

	      call_value.reason = XmCR_VALUE_CHANGED;
	      call_value.event = NULL;
	      call_value.set = TBG_Set(tg);
	      menuSTrait->entryCallback(XtParent(tg), (Widget)tg, &call_value);
	    }

	  if ((! LabG_SkipCallback(tg)) &&
	      (TBG_ValueChangedCB(tg)))
	    {
	      if (XtIsRealized ((Widget)tg))
		XFlush (XtDisplay (tg));
	      ToggleButtonCallback(tg, XmCR_VALUE_CHANGED, TBG_Set(tg), NULL);
	    }

	}
    }
  _XmAppUnlock(app);
}

/****************************************************************
 *
 * XmToggleButtonGadgetSetValue
 *    This function sets the state of the toggle gadget.
 *
 ****************************************************************/
Boolean
XmToggleButtonGadgetSetValue(
        Widget w,
        XmToggleButtonState newstate,
        Boolean notify )
{
  XmToggleButtonGadget tg = (XmToggleButtonGadget) w;
  XmMenuSystemTrait menuSTrait;
  XtAppContext app = XtWidgetToApplicationContext(w);

  _XmAppLock(app);

  /* Can't set third state if we aren't in three state mode */
  if ((newstate == XmINDETERMINATE) &&
      (TBG_ToggleMode(tg) != XmTOGGLE_INDETERMINATE)) {
    _XmAppUnlock(app);
    return False;
  }


  if (TBG_Set(tg) != newstate)
    {
      TBG_Set(tg) = newstate;
      IsOn(tg) = newstate;
      if (XtIsRealized ((Widget)tg))
	{
	  if (TBG_IndOn(tg))
	    DrawToggle(tg);
	  else
	    {
	      if (tg->gadget.shadow_thickness> 0)
		DrawToggleShadow (tg);
	      if (TBG_FillOnSelect(tg) && !LabG_IsPixmap(tg))
		DrawToggleLabel (tg);
	    }
	  if (LabG_IsPixmap(tg) || LabG_IsPixmapAndText(tg))
	    SetAndDisplayPixmap(tg, NULL, NULL);
	}
      if (notify)
	{
	  /* UNDOING this fix ... */
	  /* CR 8904: Notify value_changed before entry so that state is */
	  /*	reported correctly even if the entry callback resets it. */
	  menuSTrait = (XmMenuSystemTrait)
	    XmeTraitGet((XtPointer) XtClass(XtParent(tg)), XmQTmenuSystem);

	  if (menuSTrait != NULL)
	    {
	      XmToggleButtonCallbackStruct call_value;

	      call_value.reason = XmCR_VALUE_CHANGED;
	      call_value.event = NULL;
	      call_value.set = TBG_Set(tg);
	      menuSTrait->entryCallback(XtParent(tg), (Widget)tg, &call_value);
	    }

	  if ((! LabG_SkipCallback(tg)) &&
	      (TBG_ValueChangedCB(tg)))
	    {
	      if (XtIsRealized ((Widget)tg))
		XFlush (XtDisplay (tg));
	      ToggleButtonCallback(tg, XmCR_VALUE_CHANGED, TBG_Set(tg), NULL);
	    }

	}
    }
  _XmAppUnlock(app);
  return True;
}

/***********************************************************************
 *
 * XmCreateToggleButtonGadget
 *   Creates an instance of a togglebutton and returns the widget id.
 *
 ***********************************************************************/

Widget
XmCreateToggleButtonGadget(
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount )
{
  return XtCreateWidget(name,xmToggleButtonGadgetClass,parent,arglist,argCount);
}

Widget
XmVaCreateToggleButtonGadget(
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
                         xmToggleButtonGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}

Widget
XmVaCreateManagedToggleButtonGadget(
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
                         xmToggleButtonGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}

/*********************************************************
 *   Functions for manipulating Secondary Resources.
 *********************************************************/

/*
 * GetPushBGSecResData()
 *    Create a XmSecondaryResourceDataRec for each secondary resource;
 *    Put the pointers to these records in an array of pointers;
 *    Return the pointer to the array of pointers.
 */

/*ARGSUSED*/
static Cardinal
GetToggleBGClassSecResData(
        WidgetClass w_class,	/* unused */
        XmSecondaryResourceData **data_rtn )
{
  int arrayCount = 0;
  XmBaseClassExt  bcePtr;
  String  resource_class, resource_name;
  XtPointer  client_data;

  _XmProcessLock();
  bcePtr = &(  ToggleBGClassExtensionRec );
  client_data = NULL;
  resource_class = NULL;
  resource_name = NULL;
  arrayCount =
    _XmSecondaryResourceData ( bcePtr, data_rtn, client_data,
			      resource_name, resource_class,
			      GetToggleBGClassSecResBase) ;
  _XmProcessUnlock();
  return (arrayCount);
}

/*
 * GetToggleBGClassResBase ()
 *   return the address of the base of resources.
 */

/*ARGSUSED*/
static XtPointer
GetToggleBGClassSecResBase(
        Widget widget,
        XtPointer client_data )	/* unused */
{
  XtPointer  widgetSecdataPtr;
  int  labg_cache_size = sizeof (XmLabelGCacheObjPart);
  int  togglebg_cache_size = sizeof (XmToggleButtonGCacheObjPart);
  char *cp;

  widgetSecdataPtr = (XtPointer)
    (XtMalloc ( labg_cache_size + togglebg_cache_size + 1));

  _XmProcessLock();
  if (widgetSecdataPtr)
    {
      cp = (char *) widgetSecdataPtr;
      memcpy (cp, LabG_Cache(widget), labg_cache_size);
      cp += labg_cache_size;
      memcpy (cp, TBG_Cache(widget), togglebg_cache_size);
    }
  /* else Warning: error cannot allocate Memory */

  _XmProcessUnlock();
  return ( widgetSecdataPtr);
}

/*
 * DrawToggleLabel (tb)
 *    Called when XmNindicatorOn is TRUE and XmNfillOnSelect is FALSE.
 *    Fill toggle with selectColor or background depending on toggle
 *    value, and draw label.
 */

static void
DrawToggleLabel(
        XmToggleButtonGadget tb )
{
  Dimension margin =
    tb->gadget.highlight_thickness + tb->gadget.shadow_thickness;
  Position fx = tb->rectangle.x + margin;
  Position fy = tb->rectangle.y + margin;
  int fw = tb->rectangle.width - 2 * margin;
  int fh = tb->rectangle.height - 2 * margin;
  Boolean restore_gc = False;
  GC tmp_gc = NULL, fill_gc;

  if ((LabG_TopShadowColor(tb) == TBG_SelectColor(tb)) ||
      (LabG_BottomShadowColor(tb) == TBG_SelectColor(tb)))
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
      fill_gc = TBG_UnselectGC(tb);
      break;
    case XmSET:
      fill_gc = TBG_SelectGC(tb);
      break;
    case XmINDETERMINATE:
      {
	XGCValues values;

	/* Fetch the select_color GetGC() actually used. */
	XGetGCValues(XtDisplay(tb), TBG_SelectGC(tb), GCForeground, &values);
	values.background = TBG_UnselectColor(tb);
	XChangeGC(XtDisplay((Widget)tb), TBG_IndeterminateGC(tb),
		  GCForeground|GCBackground, &values);
	fill_gc = TBG_IndeterminateGC(tb);
	break;
      }
    default:
      assert(False);
      return;
    }

  XFillRectangle (XtDisplay(tb), XtWindow(tb), fill_gc, fx, fy, fw, fh);

  if (LabG_Foreground(tb) == TBG_SelectColor(tb) && (IsOn(tb) == XmSET))
    {
      tmp_gc =  LabG_NormalGC(tb);
      LabG_NormalGC(tb) = TBG_BackgroundGC(tb);
      restore_gc = True;
    }

  {
      XtExposeProc expose;
      _XmProcessLock();
      expose = xmLabelGadgetClassRec.rect_class.expose;
      _XmProcessUnlock();
      (* expose) ((Widget)tb, NULL, NULL);
  }

  if (restore_gc)
    {
      /* CR 9791: Label's normal_gc has a dynamic clip_mask. */
      XSetClipMask(XtDisplay(tb), TBG_BackgroundGC(tb), None);
      LabG_NormalGC(tb) = tmp_gc;
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
        XmToggleButtonGadget tb )
{
  Dimension margin =
    tb->gadget.highlight_thickness + tb->gadget.shadow_thickness;
  Position fx = tb->rectangle.x + margin;
  Position fy = tb->rectangle.y + margin;
  int fw = tb->rectangle.width - 2 * margin;
  int fh = tb->rectangle.height - 2 * margin;
  Boolean restore_gc = False;
  GC tmp_gc = NULL;
  Boolean restore_bgc = False;
  GC tmp_bgc = NULL;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) tb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if ((LabG_TopShadowColor(tb) == TBG_SelectColor(tb)) ||
      (LabG_BottomShadowColor(tb) == TBG_SelectColor(tb)))
    {
      fx += 1;
      fy += 1;
      fw -= 2;
      fh -= 2;
    }

  if (fw < 0 || fh < 0)
    return;

  XFillRectangle (XtDisplay(tb), XtWindow(tb),
		  TBG_Armed(tb) ? TBG_ArmGC(tb) : TBG_BackgroundGC(tb),
		  fx, fy, fw, fh);

  if (TBG_Armed(tb))
    {
	Pixel select_pix;
	Widget wid = XtParent(tb);

	XmGetColors(XtScreen(wid), wid->core.colormap,
		    wid->core.background_pixel,
		    NULL, NULL, NULL, &select_pix);

	if (LabG_Foreground(tb) == select_pix)
	  {
	      tmp_gc =  LabG_NormalGC(tb);
	      LabG_NormalGC(tb) = TBG_BackgroundGC(tb);
	      restore_gc = True;
	  }

	  {
	    XGCValues values;
	    /* Fetch the select_color GetGC() actually used. */
	    XGetGCValues(XtDisplay(tb), TBG_SelectGC(tb), GCBackground, &values);
	    /* Before label expose call we should change bgc
	     to correct color and then restore it.
	    */
	    if (values.background != select_pix)
	    {
		XChangeGC(XtDisplay((Widget)tb), TBG_SelectGC(tb),
		  GCBackground, &values);
		tmp_bgc = LabG_BackgroundGC(tb);
		LabG_BackgroundGC(tb) = TBG_SelectGC(tb);
		restore_bgc = True;
	    }
	 }
    }

  {
      XtExposeProc expose;
      _XmProcessLock();
      expose = xmLabelGadgetClassRec.rect_class.expose;
      _XmProcessUnlock();
      (* expose) ((Widget)tb, NULL, NULL);
  }

  if (restore_gc)
    {
      /* CR 9791: Label's normal_gc has a dynamic clip_mask. */
      XSetClipMask(XtDisplay(tb), TBG_BackgroundGC(tb), None);
      LabG_NormalGC(tb) = tmp_gc;
    }

  if (restore_bgc)
  {
      LabG_BackgroundGC(tb) = tmp_bgc;
  }
}

/*
 * DrawToggleShadow (tb)
 *   - Should be called only if ToggleShadow are to be drawn ;
 *	if the IndicatorOn resource is set to false top and bottom shadows
 *	will be switched depending on whether the Toggle is selected or
 *	unselected.
 */

static void
DrawToggleShadow(
        XmToggleButtonGadget tb )
{
  GC top_gc, bot_gc;
  int dx, dy, width, height;
  int hilite_thickness;

  if (TBG_IndOn(tb) || (IsOn(tb) == XmUNSET))
    {
      top_gc = LabG_TopShadowGC(tb);
      bot_gc = LabG_BottomShadowGC(tb);
    }
  else if (IsOn(tb) == XmINDETERMINATE)
    {
      top_gc = bot_gc = TBG_IndeterminateBoxGC(tb);
    }
  else
    {
      top_gc = LabG_BottomShadowGC(tb);
      bot_gc = LabG_TopShadowGC(tb);
    }

  hilite_thickness = tb->gadget.highlight_thickness;
  dx = (int)(tb->rectangle.x + hilite_thickness);
  dy = (int)(tb->rectangle.y + hilite_thickness);
  width = (int) ( tb->rectangle.width - (hilite_thickness << 1));
  height = (int) ( tb->rectangle.height - (hilite_thickness << 1));

  XmeDrawShadows (XtDisplay (tb), XtWindow (tb),
		  top_gc, bot_gc, dx, dy, width, height,
		  tb->gadget.shadow_thickness, XmSHADOW_OUT);
}

/* BEGIN OSF Fix pir 1778 */
/************************************************************************
 *
 * SetToggleSize(newtbg)
 * Set size properly when XmNselectPixmap or XmNselectInsensitivePixmaps
 * are set in addition to the corresponding labelPixmaps.  Have to pick
 * the largest dimensions.
 *
 ************************************************************************/

static void
SetToggleSize(
	XmToggleButtonGadget newtbg)
{
  unsigned int maxW, maxH, tmpW, tmpH;

  maxW = maxH = tmpW = tmpH = 0;

  /* We know it's a pixmap so find out how how big it is */
  if (XtIsSensitive ((Widget) newtbg))
    {
      if (!IsNull(PixmapOn(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapOn(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapOff(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapOff(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInd(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapInd(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}
    }
  else
    {
      if (!IsNull(PixmapInsenOn(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapInsenOn(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInsenOff(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapInsenOff(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}

      if (!IsNull(PixmapInsenInd(newtbg)))
	{
	  XmeGetPixmapData(XtScreen(newtbg), PixmapInsenInd(newtbg),
			   NULL, NULL, NULL, NULL, NULL, NULL,
			   &tmpW, &tmpH);
	  ASSIGN_MAX(maxW, tmpW);
	  ASSIGN_MAX(maxH, tmpH);
	}
    }

  LabG_TextRect(newtbg).width = (unsigned short) maxW;
  LabG_TextRect(newtbg).height = (unsigned short) maxH;

  /* Invoke Label's SetSize procedure. */
  {
      XtWidgetProc resize;
      _XmProcessLock();
      resize = xmLabelGadgetClassRec.rect_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) newtbg);
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
DefaultSelectColor(Widget    widget,
		   int       offset,
		   XrmValue *value)
{
  XmToggleButtonGadget tb = (XmToggleButtonGadget) widget;
  Boolean force_highlight = FALSE;
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(widget));
  Boolean enable_toggle_color = xm_dpy->display.enable_toggle_color;

  if (enable_toggle_color)
    {
      /* This code may misbehave for erroneous ind_type values. */
      if (IsOneOfMany(TBG_IndType(tb)))
	{
	  force_highlight = TRUE;
	}
      else if ((TBG_IndType(tb) == XmINVALID_TYPE) &&
	       XmIsRowColumn(XtParent(widget)))
	{
	  XtVaGetValues(XtParent(widget),
			XmNradioBehavior, &force_highlight,
			NULL);
	}
    }

  if (force_highlight)
    {
      value->size = sizeof(LabG_HighlightColor(tb));
      value->addr = (char *) &LabG_HighlightColor(tb);
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
NormalizeIndOn(XmToggleButtonGadget tb)
{
  unsigned char value = TBG_IndOn(tb);

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
NormalizeIndType(XmToggleButtonGadget tb)
{
  unsigned char value = TBG_IndType(tb);

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
