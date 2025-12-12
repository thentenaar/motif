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
static char rcsid[] = "$TOG: Primitive.c /main/25 1999/01/27 16:08:04 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/AccColorT.h>
#include <Xm/ActivatableT.h>
#include <Xm/CareVisualT.h>
#include <Xm/DrawP.h>
#include <Xm/DisplayP.h>
#include <Xm/GadgetP.h>
#include <Xm/LayoutT.h>
#include <Xm/ManagerP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/UnitTypeT.h>
#include <Xm/UnhighlightT.h>
#include "BaseClassI.h"
#include "ColorI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "ResConverI.h"
#include "ResIndI.h"
#include "SyntheticI.h"
#include "ToolTipI.h"
#include "TraitI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"


#define MESSAGE1	_XmMMsgPrimitive_0000


/********    Static Function Declarations    ********/

static void GetXFromShell(
			 Widget wid,
			 int resource_offset,
			 XtArgVal *value) ;
static void GetYFromShell(
			 Widget wid,
			 int resource_offset,
			 XtArgVal *value) ;

static void ClassInitialize( void ) ;
static void BuildPrimitiveResources(
			 WidgetClass c ) ;
static void ClassPartInitialize(
			 WidgetClass w) ;
static void Initialize(
			 Widget rw,
			 Widget nw,
			 ArgList args,
			 Cardinal *num_args) ;
static void Realize(
			 register Widget w,
			 XtValueMask *p_valueMask,
			 XSetWindowAttributes *attributes) ;
static void Destroy(
			 Widget w) ;
static void Redisplay (
			 Widget w,
			 XEvent *event,
			 Region region);
static Boolean SetValues(
			 Widget current,
			 Widget request,
			 Widget new_w,
			 ArgList args,
			 Cardinal *num_args) ;
static void HighlightBorder(
			 Widget w) ;
static void UnhighlightBorder(
			 Widget w) ;
static XmNavigability WidgetNavigable(
			 Widget wid) ;
static void FocusChange(
			 Widget wid,
			 XmFocusChange change) ;
static Boolean Redraw (Widget kid,
			Widget cur_parent,
			Widget new_parent,
			Mask visual_flag);
static XmDirection GetDirection(Widget);
static void GetColors(Widget widget,
		       XmAccessColorData color_data);
static unsigned char GetUnitType(Widget);
static void GetToolTipString(
                         Widget wid,
                         int resource, /* unused */
                         XtArgVal * value);
static XmImportOperator SetToolTipString(
                         Widget wid,
                         int resource, /* unused */
                         XtArgVal * value);

/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *   Default translation table
 *	These translations will be compiled at class initialize.  When
 *	a subclass of primitive is created then these translations will
 *	be used to augment the translations of the subclass IFF
 *	traversal is on.  The SetValues routine will also augment
 *	a subclass's translations table IFF traversal goes from off to on.
 *	Since we are augmenting it should not be a problem when
 *	traversal goes from off to on to off and on again.
 *
 ************************************************************************/

#define defaultTranslations	_XmPrimitive_defaultTranslations


/************************************************************************
 *
 *   Action list.
 *
 *************************************<->***********************************/

static XtActionsRec actions[] =
{
  {"PrimitiveFocusIn",         _XmPrimitiveFocusIn},
  {"PrimitiveFocusOut",        _XmPrimitiveFocusOut},
  {"PrimitiveUnmap",           _XmPrimitiveUnmap},
  {"PrimitiveHelp",            _XmPrimitiveHelp},
  {"PrimitiveEnter",           _XmPrimitiveEnter},
  {"PrimitiveLeave",           _XmPrimitiveLeave},
  {"PrimitiveTraverseLeft",    _XmTraverseLeft},
  {"PrimitiveTraverseRight",   _XmTraverseRight},
  {"PrimitiveTraverseUp",      _XmTraverseUp },
  {"PrimitiveTraverseDown",    _XmTraverseDown },
  {"PrimitiveTraverseNext",    _XmTraverseNext },
  {"PrimitiveTraversePrev",    _XmTraversePrev },
  {"PrimitiveTraverseHome",    _XmTraverseHome },
  {"PrimitiveNextTabGroup",    _XmTraverseNextTabGroup },
  {"PrimitivePrevTabGroup",    _XmTraversePrevTabGroup },
  {"PrimitiveParentActivate",  _XmPrimitiveParentActivate },
  {"PrimitiveParentCancel",    _XmPrimitiveParentCancel },
  {"unmap",                    _XmPrimitiveUnmap},      /* Motif 1.0 BC. */
  {"Help",                     _XmPrimitiveHelp},       /* Motif 1.0 BC. */
  {"enter",                    _XmPrimitiveEnter},      /* Motif 1.0 BC. */
  {"leave",                    _XmPrimitiveLeave},      /* Motif 1.0 BC. */
  {"PrevTabGroup",	       _XmTraversePrevTabGroup},/* Motif 1.0 BC. */
  {"NextTabGroup",	       _XmTraverseNextTabGroup},/* Motif 1.0 BC. */
};


/*****************************************/
/*  Resource definitions for XmPrimitive */

static XtResource resources[] =
{
   {
     XmNunitType, XmCUnitType, XmRUnitType,
     sizeof (unsigned char), XtOffsetOf(XmPrimitiveRec, primitive.unit_type),
     XmRCallProc, (XtPointer) _XmUnitTypeDefault
   },

   {
     XmNx, XmCPosition, XmRHorizontalPosition,
     sizeof(Position), XtOffsetOf(WidgetRec, core.x),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNy, XmCPosition, XmRVerticalPosition,
     sizeof(Position), XtOffsetOf(WidgetRec, core.y),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNwidth, XmCDimension, XmRHorizontalDimension,
     sizeof(Dimension), XtOffsetOf(WidgetRec, core.width),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNheight, XmCDimension, XmRVerticalDimension,
     sizeof(Dimension), XtOffsetOf(WidgetRec, core.height),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNborderWidth, XmCBorderWidth, XmRHorizontalDimension,
     sizeof(Dimension), XtOffsetOf(WidgetRec, core.border_width),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNforeground, XmCForeground, XmRPixel,
     sizeof (Pixel), XtOffsetOf(XmPrimitiveRec, primitive.foreground),
     XmRCallProc, (XtPointer) _XmForegroundColorDefault
   },

   {
     XmNbackground, XmCBackground, XmRPixel,
     sizeof (Pixel), XtOffsetOf(WidgetRec, core.background_pixel),
     XmRCallProc, (XtPointer) _XmBackgroundColorDefault
   },

   {
     XmNtraversalOn, XmCTraversalOn, XmRBoolean,
     sizeof (Boolean), XtOffsetOf(XmPrimitiveRec, primitive.traversal_on),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNhighlightOnEnter, XmCHighlightOnEnter, XmRBoolean,
     sizeof(Boolean),
     XtOffsetOf(XmPrimitiveRec, primitive.highlight_on_enter),
     XmRImmediate, (XtPointer) False
   },

   {
     XmNnavigationType, XmCNavigationType, XmRNavigationType,
     sizeof (unsigned char),
     XtOffsetOf(XmPrimitiveRec, primitive.navigation_type),
     XmRImmediate, (XtPointer) XmNONE
   },

   {
     XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNhighlightColor, XmCHighlightColor, XmRPixel,
     sizeof (Pixel), XtOffsetOf(XmPrimitiveRec, primitive.highlight_color),
     XmRCallProc, (XtPointer) _XmHighlightColorDefault
   },

   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmPrimitiveRec, primitive.top_shadow_color),
     XmRCallProc, (XtPointer) _XmTopShadowColorDefault
   },

   {
     XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmPrimitiveRec, primitive.bottom_shadow_color),
     XmRCallProc, (XtPointer) _XmBottomShadowColorDefault
   },

   {
     XmNbackgroundPixmap, XmCPixmap, XmRPixmap,
     sizeof (Pixmap), XtOffsetOf(WidgetRec, core.background_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {
     XmNhighlightPixmap, XmCHighlightPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap), XtOffsetOf(XmPrimitiveRec, primitive.highlight_pixmap),
     XmRCallProc, (XtPointer) _XmHighlightPixmapDefault
   },

   {
     XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmPrimitiveRec, primitive.top_shadow_pixmap),
     XmRCallProc, (XtPointer) _XmTopShadowPixmapDefault
   },

   {
     XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmPrimitiveRec, primitive.bottom_shadow_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {
     XmNhelpCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList),
     XtOffsetOf(XmPrimitiveRec, primitive.help_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNuserData, XmCUserData, XmRPointer,
     sizeof(XtPointer),
     XtOffsetOf(XmPrimitiveRec, primitive.user_data),
     XmRImmediate, (XtPointer) NULL
   },

#ifndef XM_PART_BC
   {
     XmNpopupHandlerCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList),
     XtOffsetOf(XmPrimitiveRec, primitive.popup_handler_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
      XmNconvertCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
      XtOffsetOf(XmPrimitiveRec, primitive.convert_callback),
      XmRCallback, NULL
   },
   {
     XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
     sizeof(XmDirection),
     XtOffsetOf(XmPrimitiveRec, primitive.layout_direction),
     XmRCallProc, (XtPointer) _XmDirectionDefault
   },
#endif
};

#ifdef XM_PART_BC
XmDirection XmPrimLayoutDir = XmDEFAULT_DIRECTION ;
#endif


/***************************************/
/*  Definition for synthetic resources */

static XmSyntheticResource syn_resources[] =
{
   { XmNx,
     sizeof (Position), XtOffsetOf(WidgetRec, core.x),
     GetXFromShell, XmeToHorizontalPixels },

   { XmNy,
     sizeof (Position), XtOffsetOf(WidgetRec, core.y),
     GetYFromShell, XmeToVerticalPixels },

   { XmNwidth,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNheight,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.height),
     XmeFromVerticalPixels, XmeToVerticalPixels },

   { XmNborderWidth,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.border_width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNhighlightThickness,
     sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNshadowThickness,
     sizeof (Dimension),
     XtOffsetOf(XmPrimitiveRec, primitive.shadow_thickness),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNtoolTipString,
     0,
     0,
     GetToolTipString, SetToolTipString }
};


/*******************************************/
/*  Declaration of class extension records */

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,				/* InitializePrehook	*/
    NULL,				/* SetValuesPrehook	*/
    NULL,				/* InitializePosthook	*/
    NULL,				/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    NULL,				/* secondaryCreate	*/
    NULL,		                /* getSecRes data	*/
    { 0 },				/* fastSubclass flags	*/
    NULL,				/* get_values_prehook	*/
    NULL,				/* get_values_posthook	*/
    NULL,                               /* classPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    FocusChange,                        /* focusChange          */
};

static XmPrimitiveClassExtRec primClassExtRec = {
    NULL,
    NULLQUARK,
    XmPrimitiveClassExtVersion,
    sizeof(XmPrimitiveClassExtRec),
    NULL,                               /* widget_baseline */
    NULL,                               /* widget_display_rect */
    NULL                                /* widget_margins */
};


/*******************************************/
/*  The Primitive class record definition  */

externaldef(xmprimitiveclassrec) XmPrimitiveClassRec xmPrimitiveClassRec =
{
   {
      (WidgetClass) &widgetClassRec,    /* superclass	         */
      "XmPrimitive",                    /* class_name	         */
      sizeof(XmPrimitiveRec),           /* widget_size	         */
      ClassInitialize,                  /* class_initialize      */
      ClassPartInitialize,              /* class_part_initialize */
      False,                            /* class_inited          */
      Initialize,                       /* initialize	         */
      NULL,                             /* initialize_hook       */
      Realize,                          /* realize	         */
      actions,                          /* actions               */
      XtNumber(actions),                /* num_actions	         */
      resources,                        /* resources	         */
      XtNumber(resources),              /* num_resources         */
      NULLQUARK,                        /* xrm_class	         */
      True,                             /* compress_motion       */
      XtExposeCompressMaximal |		/* compress_exposure     */
	  XtExposeNoRegion,
      True,                             /* compress_enterleave   */
      False,                            /* visible_interest      */
      Destroy,                          /* destroy               */
      NULL,                             /* resize                */
      Redisplay,                        /* expose                */
      SetValues,                        /* set_values	         */
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      _XmPrimitiveGetValuesHook,        /* get_values_hook       */
      NULL,                             /* accept_focus	         */
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,				/* display_accelerator   */
      (XtPointer)&baseClassExtRec,      /* extension             */
   },

   {
      HighlightBorder,		        /* border_highlight   */
      UnhighlightBorder,		/* border_unhighlight */
      defaultTranslations,		/* translations       */
      NULL,				/* arm_and_activate   */
      syn_resources,			/* syn resources      */
      XtNumber(syn_resources),		/* num_syn_resources  */
      (XtPointer)&primClassExtRec,      /* extension        */
   }
};

externaldef(xmprimitivewidgetclass) WidgetClass xmPrimitiveWidgetClass =
				     (WidgetClass) &xmPrimitiveClassRec;



static XmConst XmSpecifyLayoutDirectionTraitRec primLDT = {
  0,			/* version */
  GetDirection
};


/* Care visual Trait record for Primitive */

static XmConst XmCareVisualTraitRec primCVT = {
    0,		/* version */
    Redraw,
};

/* Access Colors Trait record for Primitive */

static XmConst XmAccessColorsTraitRec primACT = {
  0,			/* version */
  GetColors
};

/* Unit Type Trait record for Primitive */

static XmConst XmSpecUnitTypeTraitRec primUTT = {
  0,			/* version */
  GetUnitType
};


/**************************************************************************
**
** Synthetic resource hooks function section
**
**************************************************************************/

static void
GetXFromShell(
	 Widget wid,
	 int resource_offset,
	 XtArgVal *value )
{
    /* return the x in the child's unit type; for children of shell, return
     ** the parent's x relative to the origin, in pixels
     */

    Position	rootx, rooty;
    Widget parent = XtParent(wid);

    if (XtIsShell(parent))
    {
	 XtTranslateCoords( (Widget) wid,
		 (Position) 0, (Position) 0, &rootx, &rooty) ;
	 *value = (XtArgVal) rootx;
    }
    else
    {
	 *value = (XtArgVal) wid->core.x ;
	 XmeFromHorizontalPixels(wid,  resource_offset, value);
    }
}

static void
GetYFromShell(
	 Widget wid,
	 int resource_offset,
	 XtArgVal *value )
{
    /* return the y in the child's unit type; for children of shell, return
     ** the parent's y relative to the origin, in pixels
     */

    Position	rootx, rooty;
    Widget parent = XtParent(wid);

    if (XtIsShell(parent))
    {
	 XtTranslateCoords( (Widget) wid,
		 (Position) 0, (Position) 0, &rootx, &rooty) ;
	 *value = (XtArgVal) rooty;
    }
    else
    {
	 *value = (XtArgVal) wid->core.y ;
	 XmeFromVerticalPixels(wid,  resource_offset, value);
    }
}

/************************************************************************
 *
 *  ClassInitialize
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
   /* These routines are called for each base classes,
	they just returned if it has been done already */
   _XmRegisterConverters();
   _XmRegisterPixmapConverters();
   _XmInitializeExtensions();
   _XmInitializeTraits();

   baseClassExtRec.record_type = XmQmotif;

}


/**********************************************************************
 *
 *  BuildPrimitiveResources
 *	Build up the primitive's synthetic resource processing
 *      list by combining the super classes with this class.
 *
 **********************************************************************/

static void
BuildPrimitiveResources(
	 WidgetClass c )
{
    XmPrimitiveWidgetClass wc = (XmPrimitiveWidgetClass) c ;
    XmPrimitiveWidgetClass sc = (XmPrimitiveWidgetClass)
	 wc->core_class.superclass;

    _XmInitializeSyntheticResources(wc->primitive_class.syn_resources,
				    wc->primitive_class.num_syn_resources);

    if (sc == (XmPrimitiveWidgetClass) widgetClass) return;

    _XmBuildResources (&(wc->primitive_class.syn_resources),
			&(wc->primitive_class.num_syn_resources),
			sc->primitive_class.syn_resources,
			sc->primitive_class.num_syn_resources);
}



/************************************************************************
 *
 *  ClassPartInitialize
 *    Set up the inheritance mechanism for the routines exported by
 *    primitives class part.
 *
 ************************************************************************/
static void
ClassPartInitialize(
	 WidgetClass w )
{
    static Boolean first_time = TRUE;
    XmPrimitiveWidgetClass wc = (XmPrimitiveWidgetClass) w;
    XmPrimitiveWidgetClass super =
	(XmPrimitiveWidgetClass) wc->core_class.superclass;
    XmPrimitiveClassExt              *wcePtr, *scePtr;


    _XmFastSubclassInit (w, XmPRIMITIVE_BIT);

    /*** first deal with inheritance of regular class method */

    if (wc->primitive_class.border_highlight == XmInheritWidgetProc)
	wc->primitive_class.border_highlight =
	    super->primitive_class.border_highlight;

    if (wc->primitive_class.border_unhighlight == XmInheritWidgetProc)
	wc->primitive_class.border_unhighlight =
	    super->primitive_class.border_unhighlight;

    if (wc->primitive_class.translations == XtInheritTranslations)
	wc->primitive_class.translations =
	    super->primitive_class.translations;
    else if (wc->primitive_class.translations)
	wc->primitive_class.translations = (String)
	    XtParseTranslationTable(wc->primitive_class.translations);

    if (wc->primitive_class.arm_and_activate == XmInheritArmAndActivate)
        wc->primitive_class.arm_and_activate =
	    super->primitive_class.arm_and_activate;

    /* synthetic resource management */
    BuildPrimitiveResources((WidgetClass) wc);

    /*** then look at the extension.
      if it's NULL, create a new one with inherit everywhere,
      then do the inheritance. */

    wcePtr = _XmGetPrimitiveClassExtPtr(wc, NULLQUARK);

    if (*wcePtr == NULL) {
	*wcePtr = (XmPrimitiveClassExt) XtCalloc(1,
					 sizeof(XmPrimitiveClassExtRec)) ;
	(*wcePtr)->next_extension = NULL;
	(*wcePtr)->record_type 	= NULLQUARK;
	(*wcePtr)->version	= XmPrimitiveClassExtVersion ;
	(*wcePtr)->record_size	= sizeof(XmPrimitiveClassExtRec);
	(*wcePtr)->widget_baseline = XmInheritBaselineProc;
	(*wcePtr)->widget_display_rect  = XmInheritDisplayRectProc ;
	(*wcePtr)->widget_margins = XmInheritMarginsProc ;
    }

    if ((WidgetClass)wc != xmPrimitiveWidgetClass) {

	scePtr = _XmGetPrimitiveClassExtPtr(super, NULLQUARK);

	if ((*wcePtr)->widget_baseline == XmInheritBaselineProc)
	    (*wcePtr)->widget_baseline = (*scePtr)->widget_baseline;

	if ((*wcePtr)->widget_display_rect == XmInheritDisplayRectProc)
	    (*wcePtr)->widget_display_rect  = (*scePtr)->widget_display_rect;

	if ((*wcePtr)->widget_margins == XmInheritMarginsProc)
	    (*wcePtr)->widget_margins  = (*scePtr)->widget_margins;
    }

    /*** Carry this ugly non portable code that deal with Xt internals.
       Maintain first_time because we want to do that only once
       Object ClassPartInit has been called */
    if (first_time) {
        _XmReOrderResourceList(xmPrimitiveWidgetClass, XmNunitType, NULL);
        _XmReOrderResourceList(xmPrimitiveWidgetClass,
			       XmNforeground, XmNbackground);
        first_time = FALSE;
    }



    /*** setting up traits for all subclasses as well.*/

    XmeTraitSet((XtPointer)wc, XmQTspecifyLayoutDirection,(XtPointer)&primLDT);
    XmeTraitSet((XtPointer)wc, XmQTcareParentVisual, (XtPointer)&primCVT);
    XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&primACT);
    XmeTraitSet((XtPointer)wc, XmQTspecifyUnitType, (XtPointer)&primUTT);
}





/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    XmPrimitiveWidget request = (XmPrimitiveWidget) rw ;
    XmPrimitiveWidget pw = (XmPrimitiveWidget) nw ;
    XtTranslations translations ;
    XmString tool_tip_string;

    static XtResource subresources[] =
    {
        {
            XmNtoolTipString, XmCToolTipString, XmRXmString,
            sizeof(XmString), 0,
            XmRImmediate, (XtPointer) NULL
        },
    };

    _XmProcessLock();
    translations = (XtTranslations) ((XmPrimitiveClassRec *) XtClass( pw))
	->primitive_class.translations ;
    _XmProcessUnlock();

    XtGetSubresources(nw, &tool_tip_string, NULL, NULL,
         subresources, XtNumber(subresources),
	 args, *num_args);

    XmSetToolTipString(nw, (XmString) tool_tip_string);

    if(    pw->primitive.traversal_on
       && translations  &&  pw->core.tm.translations
       && !XmIsLabel( pw)    )
	{
	    /*  If this widget is requesting traversal then augment its
	     * translation table with some additional events.
	     * We will only augment translations for a widget which
	     * already has some translations defined; this allows widgets
	     * which want to set different translations (i.e. menus) to
	     * it at a later point in time.
	     * We do not override RowColumn and Label subclasses, these
	     * are handled by those classes.
	     */
	    XtOverrideTranslations( (Widget) pw, translations) ;
	}

    pw->primitive.have_traversal = FALSE ;
    pw->primitive.highlighted = FALSE ;
    pw->primitive.highlight_drawn = FALSE ;

    if((pw->primitive.navigation_type != XmDYNAMIC_DEFAULT_TAB_GROUP)
       && !XmRepTypeValidValue(XmRID_NAVIGATION_TYPE,
                               pw->primitive.navigation_type,
			       (Widget) pw))
	{   pw->primitive.navigation_type = XmNONE ;
	}
    _XmNavigInitialize( (Widget) request, (Widget) pw, args, num_args);

    if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE,
				pw->primitive.unit_type, (Widget) pw)    )
	{
	    pw->primitive.unit_type = XmPIXELS;
	}


    /*  Convert the fields from unit values to pixel values  */

    _XmPrimitiveImportArgs( (Widget) pw, args, num_args);

    /*  Check the geometry information for the widget  */

    if (request->core.width == 0)
	pw->core.width += pw->primitive.highlight_thickness * 2 +
	    pw->primitive.shadow_thickness * 2;

    if (request->core.height == 0)
	pw->core.height += pw->primitive.highlight_thickness * 2 +
	    pw->primitive.shadow_thickness * 2;

    /*  Get the graphics contexts for the border drawing  */

    pw->primitive.highlight_GC =
	_XmGetPixmapBasedGC (nw,
			     pw->primitive.highlight_color,
			     pw->core.background_pixel,
			     pw->primitive.highlight_pixmap);
    pw->primitive.top_shadow_GC =
	_XmGetPixmapBasedGC (nw,
			     pw->primitive.top_shadow_color,
			     pw->core.background_pixel,
			     pw->primitive.top_shadow_pixmap);
    pw->primitive.bottom_shadow_GC =
	_XmGetPixmapBasedGC (nw,
			     pw->primitive.bottom_shadow_color,
			     pw->core.background_pixel,
			     pw->primitive.bottom_shadow_pixmap);

}




/************************************************************************
 *
 *  Realize
 *	General realize procedure for primitive widgets.  Lets the bit
 *	gravity default to Forget.
 *
 ************************************************************************/
static void
Realize(
        register Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
   Mask valueMask = *p_valueMask;

   valueMask |= CWDontPropagate;
   attributes->do_not_propagate_mask =
      ButtonPressMask | ButtonReleaseMask |
      KeyPressMask | KeyReleaseMask | PointerMotionMask;

   XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);
}


/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/
/* ARGSUSED */
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    XmPrimitiveWidget pw = (XmPrimitiveWidget) wid ;

    if (pw->primitive.highlighted) {
	(*(((XmPrimitiveWidgetClass) XtClass(wid))
	   ->primitive_class.border_highlight))(wid) ;
    } else {
	(*(((XmPrimitiveWidgetClass) XtClass(wid))
	   ->primitive_class.border_unhighlight))(wid) ;
    }
}


/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/
static void Destroy(Widget w)
{
   XmPrimitiveWidget pw = (XmPrimitiveWidget) w ;

   _XmNavigDestroy(w);
   _XmToolTipRemove(w);
   XtReleaseGC( w, pw->primitive.top_shadow_GC);
   XtReleaseGC( w, pw->primitive.bottom_shadow_GC);
   XtReleaseGC( w, pw->primitive.highlight_GC);
}

/************************************************************************
 *
 *  SetValues
 *     Perform and updating necessary for a set values call.
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget current,
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
   XmPrimitiveWidget curpw = (XmPrimitiveWidget) current;
   XmPrimitiveWidget newpw = (XmPrimitiveWidget) new_w;
   Boolean returnFlag = False;

    if (XtIsSensitive(current) != XtIsSensitive(new_w))
    {
    	if (!XtIsSensitive(new_w))
    	{
	    _XmToolTipLeave(new_w, NULL, NULL, NULL);
    	}
    }

    if (XmGetToolTipString(new_w) != XmGetToolTipString(current))
    {
        XmSetToolTipString(current, XmGetToolTipString (new_w));
    }

   /* CR 7124: XmNlayoutDirection is a CG resource. */
   if (XmPrim_layout_direction(curpw) != XmPrim_layout_direction(newpw))
     {
       XmeWarning(new_w, MESSAGE1);
       XmPrim_layout_direction(newpw) = XmPrim_layout_direction(curpw);
     }

   if(    newpw->primitive.traversal_on
       && (newpw->primitive.traversal_on != curpw->primitive.traversal_on)
       && newpw->core.tm.translations
       && ((XmPrimitiveClassRec *) XtClass( newpw))
                                            ->primitive_class.translations
       && !XmIsLabel(newpw)    ) {
       _XmProcessLock();
       XtOverrideTranslations( (Widget) newpw, (XtTranslations)
			      ((XmPrimitiveClassRec *) XtClass( newpw))
			      ->primitive_class.translations) ;
       _XmProcessUnlock();
       }
   if(    curpw->primitive.navigation_type
      != newpw->primitive.navigation_type    )
     {
	 if(    !XmRepTypeValidValue( XmRID_NAVIGATION_TYPE,
				     newpw->primitive.navigation_type,
				     (Widget) newpw)    )
	 {
	     newpw->primitive.navigation_type
		 = curpw->primitive.navigation_type ;
	 }
     }
   returnFlag = _XmNavigSetValues( current, request, new_w, args, num_args);

   /*  Validate changed data.  */

   if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE,
                               newpw->primitive.unit_type,
			       (Widget) newpw)    )
       {
       newpw->primitive.unit_type = curpw->primitive.unit_type;
   }


   /*  Convert the necessary fields from unit values to pixel values  */

   _XmPrimitiveImportArgs( (Widget) newpw, args, num_args);

   /*  Check for resize conditions  */

   if (curpw->primitive.shadow_thickness !=
       newpw->primitive.shadow_thickness ||
       curpw->primitive.highlight_thickness !=
       newpw->primitive.highlight_thickness)
      returnFlag = True;

   if (curpw->primitive.highlight_color != newpw->primitive.highlight_color ||
       curpw->primitive.highlight_pixmap != newpw->primitive.highlight_pixmap)
   {
       XtReleaseGC ((Widget) newpw, newpw->primitive.highlight_GC);
       newpw->primitive.highlight_GC =
	   _XmGetPixmapBasedGC (new_w,
				newpw->primitive.highlight_color,
				newpw->core.background_pixel,
				newpw->primitive.highlight_pixmap);
       returnFlag = True;
   }

   if (curpw->primitive.top_shadow_color !=
       newpw->primitive.top_shadow_color ||
       curpw->primitive.top_shadow_pixmap !=
       newpw->primitive.top_shadow_pixmap)
       {
       XtReleaseGC ((Widget) newpw, newpw->primitive.top_shadow_GC);
       newpw->primitive.top_shadow_GC =
	   _XmGetPixmapBasedGC (new_w,
				newpw->primitive.top_shadow_color,
				newpw->core.background_pixel,
				newpw->primitive.top_shadow_pixmap);
       returnFlag = True;
   }

   if (curpw->primitive.bottom_shadow_color !=
       newpw->primitive.bottom_shadow_color ||
       curpw->primitive.bottom_shadow_pixmap !=
       newpw->primitive.bottom_shadow_pixmap)
   {
      XtReleaseGC( (Widget) newpw, newpw->primitive.bottom_shadow_GC);
      newpw->primitive.bottom_shadow_GC =
	  _XmGetPixmapBasedGC (new_w,
			       newpw->primitive.bottom_shadow_color,
			       newpw->core.background_pixel,
			       newpw->primitive.bottom_shadow_pixmap);
      returnFlag = True;
   }

   if(    newpw->primitive.highlight_drawn
      &&  (    !XtIsSensitive( (Widget) newpw)
	   ||  (    (curpw->primitive.highlight_on_enter)
		&&  !(newpw->primitive.highlight_on_enter)
		&&  (_XmGetFocusPolicy( (Widget) newpw) == XmPOINTER)))    )
     {
       if(    ((XmPrimitiveWidgetClass) XtClass( newpw))
	  ->primitive_class.border_unhighlight    )
	 {
	     (*(((XmPrimitiveWidgetClass) XtClass( newpw))
		->primitive_class.border_unhighlight))( (Widget) newpw) ;
	 }
     }

   /*  Return a flag which may indicate that a redraw needs to occur.  */

   return (returnFlag);
}


/************************************************************************
 *
 *  HighlightBorder
 *
 ************************************************************************/
static void
HighlightBorder(
        Widget w )
{
    XmPrimitiveWidget pw = (XmPrimitiveWidget) w ;

    pw->primitive.highlighted = True ;
    pw->primitive.highlight_drawn = True ;

    if(XtWidth( pw) == 0 || XtHeight( pw) == 0
       || pw->primitive.highlight_thickness == 0) return ;


    XmeDrawHighlight( XtDisplay( pw), XtWindow( pw),
		     pw->primitive.highlight_GC, 0, 0,
		     XtWidth( pw), XtHeight( pw),
		     pw->primitive.highlight_thickness) ;
}


/************************************************************************
 *
 *  UnhighlightBorder
 *
 ************************************************************************/
static void
UnhighlightBorder(
        Widget w )
{
    XmPrimitiveWidget pw = (XmPrimitiveWidget) w ;
    XmSpecifyUnhighlightTrait UnhighlightT;
    GC manager_background_GC;

    pw->primitive.highlighted = False ;
    pw->primitive.highlight_drawn = False ;

    if ( XtWidth( w) == 0
	|| XtHeight( w) == 0
        || pw->primitive.highlight_thickness == 0)
	{
	return ;
	}


    if(XmIsManager( pw->core.parent))
	{
	/* If unhighlight trait in parent use specified GC */
	if (((UnhighlightT=(XmSpecifyUnhighlightTrait)XmeTraitGet((XtPointer)
		XtClass(pw->core.parent), XmQTspecifyUnhighlight)) != NULL)
	    && (UnhighlightT->getUnhighlightGC != NULL))
	    {
	    manager_background_GC =
			    UnhighlightT->getUnhighlightGC( pw->core.parent, w);
	    }
	/* otherwise, use parent's background GC */
	else
	    {
	    manager_background_GC = ((XmManagerWidget)(pw->core.parent))
			    ->manager.background_GC;
	    }
	XmeDrawHighlight( XtDisplay( pw), XtWindow( pw),
			    manager_background_GC,
			    0, 0, XtWidth( w), XtHeight( w),
			    pw->primitive.highlight_thickness) ;
	}
    else
	XmeClearBorder( XtDisplay (pw), XtWindow (pw), 0, 0, XtWidth( w),
		       XtHeight( w) , pw->primitive.highlight_thickness) ;
}


/************************************************************************
 *
 *  WidgetNavigable
 *
 ************************************************************************/
static XmNavigability
WidgetNavigable(
        Widget wid)
{
    if(    XtIsSensitive(wid)
       &&  ((XmPrimitiveWidget) wid)->primitive.traversal_on    )
	{
	    XmNavigationType nav_type = ((XmPrimitiveWidget) wid)
		->primitive.navigation_type ;
	    if(    (nav_type == XmSTICKY_TAB_GROUP)
	       ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	       ||  (    (nav_type == XmTAB_GROUP)
		    &&  !_XmShellIsExclusive( wid))    )
		{
		    return XmTAB_NAVIGABLE ;
		}
	    return XmCONTROL_NAVIGABLE ;
	}
    return XmNOT_NAVIGABLE ;
}


/************************************************************************
 *
 *  FocusChange
 *
 ************************************************************************/
static void
FocusChange(
        Widget wid,
        XmFocusChange change)
{
    /* Enter/Leave is called only in pointer mode,
     * Focus in/out only called in explicit mode.
     */
    switch(    change    )
	{
	case XmENTER:
	    if(!(((XmPrimitiveWidget) wid)->primitive.highlight_on_enter))
		{
		    break ;
		}
	    /* Drop through. */
	case XmFOCUS_IN:
	    if(change == XmFOCUS_IN    ) /* Because of drop-though. */ {
		((XmPrimitiveWidget) wid)->primitive.have_traversal = TRUE ;
	    }
	    if(    ((XmPrimitiveWidgetClass) XtClass( wid))
	       ->primitive_class.border_highlight    )
		{
		    (*(((XmPrimitiveWidgetClass) XtClass( wid))
		       ->primitive_class.border_highlight))( wid) ;
		}
	    break ;
	case XmLEAVE:
	    if(!(((XmPrimitiveWidget) wid)->primitive.highlight_on_enter))
		{
		    break ;
		}
	    /* Drop through. */
	case XmFOCUS_OUT:
	    if(change == XmFOCUS_OUT    ) /* Because of drop-though. */{

		((XmPrimitiveWidget) wid)->primitive.have_traversal = FALSE ;
	    }
	    if(    ((XmPrimitiveWidgetClass) XtClass( wid))
	       ->primitive_class.border_unhighlight    )
		{
		    (*(((XmPrimitiveWidgetClass) XtClass( wid))
		       ->primitive_class.border_unhighlight))( wid) ;
		}
	    break ;
	}
    return ;
}


/****************************************/
/******------ trait methods -------******/
/****************************************/



static XmDirection
GetDirection(Widget w)
{
  return XmPrim_layout_direction(((XmPrimitiveWidget)(w)));
}



/*ARGSUSED*/
static Boolean
Redraw (
	Widget kid,
	Widget cur_parent,	/* unused */
	Widget new_parent,	/* unused */
	Mask visual_flag)
{
    XmPrimitiveWidget pw = (XmPrimitiveWidget) kid ;

    /* primitive only cares about background info */
    /* Menu buttons "redefine" unhighlight on a per instance basis,
       so they'll get this trait method anyway and do nothing
       in their unhighlight method on a per instance basis */
    /* Separators, Lists, have a null unhighlight, so they'll be caught
       here, they could redefine the trait, kindof uninstall it... */

    if (visual_flag & (VisualBackgroundPixel|VisualBackgroundPixmap)) {
	/* do the unhighlight if necessary, using class method */
	/* parent manager background gc already set up */
	if (!pw->primitive.highlighted) {
            XmPrimitiveClassRec* kid_class = (XmPrimitiveClassRec*) XtClass(kid);

	    if (! XtIsRealized(kid)) return True;

	    if (kid_class->primitive_class.border_unhighlight)
	      kid_class-> primitive_class.border_unhighlight(kid);
	}
    }

    return False ;
}

static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
    XmPrimitiveWidget pw = (XmPrimitiveWidget) w ;

    color_data->valueMask = AccessForeground | AccessBackgroundPixel |
	AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
    color_data->background = pw->core.background_pixel;
    color_data->foreground = pw->primitive.foreground;
    color_data->highlight_color = pw->primitive.highlight_color;
    color_data->top_shadow_color = pw->primitive.top_shadow_color;
    color_data->bottom_shadow_color = pw->primitive.bottom_shadow_color;
}


static unsigned char
GetUnitType(Widget w)
{
    return ((XmPrimitiveWidget) w)->primitive.unit_type ;
}


/****************************************************************
 ****************************************************************
 **
 ** External functions, both _Xm and Xm.
 ** First come the action procs and then the other external entry points.
 **
 ****************************************************************
 ****************************************************************/




/************************************************************************
 *
 *  The traversal event processing routines.
 *    The following set of routines are the entry points invoked from
 *    each primitive widget when one of the traversal event conditions
 *    occur.  These routines are externed in XmP.h.
 *
 ************************************************************************/
/*ARGSUSED*/
void
_XmTraverseLeft(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  _XmMgrTraversal(w, XmTRAVERSE_LEFT);
}

/*ARGSUSED*/
void
_XmTraverseRight(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_RIGHT);
}

/*ARGSUSED*/
void
_XmTraverseUp(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_UP);
}

/*ARGSUSED*/
void
_XmTraverseDown(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_DOWN);
}

/*ARGSUSED*/
void
_XmTraverseNext(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_NEXT);
}

/*ARGSUSED*/
void
_XmTraversePrev(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_PREV);
}

/*ARGSUSED*/
void
_XmTraverseHome(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   _XmMgrTraversal (w, XmTRAVERSE_HOME);
}

/*ARGSUSED*/
void
_XmTraverseNextTabGroup(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean button_tab = xm_dpy->display.enable_button_tab;

  if (button_tab)
    _XmMgrTraversal(w, XmTRAVERSE_GLOBALLY_FORWARD);
  else
    _XmMgrTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}

/*ARGSUSED*/
void
_XmTraversePrevTabGroup(
        Widget w,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  Boolean button_tab = xm_dpy->display.enable_button_tab;

  if (button_tab)
    _XmMgrTraversal(w, XmTRAVERSE_GLOBALLY_BACKWARD);
  else
    _XmMgrTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
}



/*ARGSUSED*/
void
_XmPrimitiveHelp(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   if (!_XmIsEventUnique(event))
      return;

   _XmSocorro( wid, event, NULL, NULL);

   _XmRecordEvent(event);
}



void
_XmPrimitiveParentActivate(
        Widget pw,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmParentInputActionRec  pp_data ;

    pp_data.process_type = XmINPUT_ACTION ;
    pp_data.action = XmPARENT_ACTIVATE ;
    pp_data.event = event ;
    pp_data.params = params ;
    pp_data.num_params = num_params ;

    _XmParentProcess( XtParent( pw), (XmParentProcessData) &pp_data) ;
}

void
_XmPrimitiveParentCancel(
        Widget pw,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmParentInputActionRec  pp_data ;

    pp_data.process_type = XmINPUT_ACTION ;
    pp_data.action = XmPARENT_CANCEL ;
    pp_data.event = event ;
    pp_data.params = params ;
    pp_data.num_params = num_params ;

    _XmParentProcess( XtParent( pw), (XmParentProcessData) &pp_data) ;
}

/**********************************************************************
 *
 * _XmButtonTakeFocus
 *
 *********************************************************************/
/*ARGSUSED*/
void
_XmButtonTakeFocus(
        Widget wid,
        XEvent *event,		/* unused */
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmProcessTraversal(wid, XmTRAVERSE_CURRENT);
}

static void
GetToolTipString(Widget wid,
                 int resource, /* unused */
                 XtArgVal * value)
{
    XmString string = XmGetToolTipString (wid);
    *value = (XtArgVal) string;
}

XmImportOperator
SetToolTipString(Widget wid,
                 int resource, /* unused */
                 XtArgVal * value)
{
    XmSetToolTipString (wid, (XmString)*value);
    return XmSYNTHETIC_NONE;
}
