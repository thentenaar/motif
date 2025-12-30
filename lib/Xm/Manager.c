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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Manager.c /main/22 1999/01/27 16:07:30 mgreess $"
#endif
#endif

#include <Xm/AccColorT.h>
#include <Xm/CareVisualT.h>
#include <Xm/DrawP.h>
#include <Xm/LayoutT.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/UnitTypeT.h>
#include "BaseClassI.h"
#include "CareVisualTI.h"
#include "ColorI.h"
#include "DisplayP.h"
#include "GadgetI.h"
#include "GadgetUtiI.h"
#include "ImageCachI.h"
#include "ManagerI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "RepTypeI.h"
#include "ResConverI.h"
#include "ResIndI.h"
#include "SyntheticI.h"
#include "TraitI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"

#define MESSAGE1 _XmMMsgManager_0000
#define MESSAGE2 _XmMMsgManager_0001

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
static CompositeClassExtension FindCompClassExtension(
                        WidgetClass widget_class) ;
static void BuildManagerResources(
			WidgetClass c) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void Initialize(
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Realize(
                        Widget w,
                        XtValueMask *p_valueMask,
                        XSetWindowAttributes *attributes) ;
static void Destroy(
                        Widget w) ;
static Boolean SetValues(
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InsertChild(
                        Widget child) ;
static void DeleteChild(
                        Widget child) ;
static void ManagerMotion(
                        Widget wid,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static void ManagerEnter(
                        Widget wid,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static void ManagerLeave(
                        Widget wid,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static void AddMotionHandlers(
                        XmManagerWidget mw) ;
static void ConstraintInitialize(
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void CheckRemoveMotionHandlers(
                        XmManagerWidget mw) ;
static void ConstraintDestroy(
                        Widget w) ;
static Boolean ConstraintSetValues(
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean ManagerParentProcess(
                        Widget widget,
                        XmParentProcessData data) ;
static XmNavigability WidgetNavigable(
                        Widget wid) ;
static Widget ObjectAtPoint(
			      Widget wid,
			      Position x, Position y);
static XmDirection GetDirection(Widget);
static void GetColors(Widget widget,
		      XmAccessColorData color_data);
static unsigned char GetUnitType(Widget);

/********    End Static Function Declarations    ********/



/***********************************/
/*  Default actions for XmManager  */

static XtActionsRec actions[] = {
	{ "ManagerGadgetTraverseCurrent",  _XmGadgetTraverseCurrent },
	{ "ManagerEnter",               _XmManagerEnter },
	{ "ManagerLeave",               _XmManagerLeave },
	{ "ManagerFocusIn",             _XmManagerFocusIn },
	{ "ManagerFocusOut",            _XmManagerFocusOut },
	{ "ManagerGadgetPrevTabGroup",  _XmGadgetTraversePrevTabGroup},
	{ "ManagerGadgetNextTabGroup",  _XmGadgetTraverseNextTabGroup},
	{ "ManagerGadgetTraversePrev",  _XmGadgetTraversePrev },
	{ "ManagerGadgetTraverseNext",  _XmGadgetTraverseNext },
	{ "ManagerGadgetTraverseLeft",  _XmGadgetTraverseLeft },
	{ "ManagerGadgetTraverseRight", _XmGadgetTraverseRight },
	{ "ManagerGadgetTraverseUp",    _XmGadgetTraverseUp },
	{ "ManagerGadgetTraverseDown",  _XmGadgetTraverseDown },
	{ "ManagerGadgetTraverseHome",  _XmGadgetTraverseHome },
	{ "ManagerGadgetSelect",        _XmGadgetSelect },
	{ "ManagerParentActivate",      _XmManagerParentActivate },
	{ "ManagerParentCancel",        _XmManagerParentCancel },
	{ "ManagerGadgetButtonMotion",  _XmGadgetButtonMotion },
	{ "ManagerGadgetKeyInput",      _XmGadgetKeyInput },
	{ "ManagerGadgetHelp",          _XmManagerHelp },
        { "ManagerGadgetArm",           _XmGadgetArm },
        { "ManagerGadgetDrag",          _XmGadgetDrag },
	{ "ManagerGadgetActivate",      _XmGadgetActivate },
	{ "ManagerGadgetMultiArm",      _XmGadgetMultiArm },
	{ "ManagerGadgetMultiActivate", _XmGadgetMultiActivate },
        { "Enter",           _XmManagerEnter },         /* Motif 1.0 BC. */
        { "FocusIn",         _XmManagerFocusIn },       /* Motif 1.0 BC. */
        { "Help",            _XmManagerHelp },          /* Motif 1.0 BC. */
        { "Arm",             _XmGadgetArm },            /* Motif 1.0 BC. */
	{ "Activate",        _XmGadgetActivate },       /* Motif 1.0 BC. */

};


/****************************************/
/*  Resource definitions for XmManager  */

static XtResource resources[] =
{
   {
     XmNunitType, XmCUnitType, XmRUnitType,
     sizeof (unsigned char), XtOffsetOf(XmManagerRec, manager.unit_type),
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
     sizeof (Pixel), XtOffsetOf(XmManagerRec, manager.foreground),
     XmRCallProc, (XtPointer) _XmForegroundColorDefault
   },

   {
     XmNbackground, XmCBackground, XmRPixel,
     sizeof (Pixel), XtOffsetOf(WidgetRec, core.background_pixel),
     XmRCallProc, (XtPointer) _XmBackgroundColorDefault
   },

   {
     XmNbackgroundPixmap, XmCPixmap, XmRPixmap,
     sizeof (Pixmap), XtOffsetOf(WidgetRec, core.background_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {
     XmNhighlightColor, XmCHighlightColor, XmRPixel,
     sizeof (Pixel), XtOffsetOf(XmManagerRec, manager.highlight_color),
     XmRCallProc, (XtPointer) _XmHighlightColorDefault
   },

   {
     XmNhighlightPixmap, XmCHighlightPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap), XtOffsetOf(XmManagerRec, manager.highlight_pixmap),
     XmRCallProc, (XtPointer) _XmHighlightPixmapDefault
   },

   {
     XmNnavigationType, XmCNavigationType, XmRNavigationType,
     sizeof (unsigned char),
     XtOffsetOf(XmManagerRec, manager.navigation_type),
     XmRImmediate, (XtPointer) XmTAB_GROUP,
   },

   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
     sizeof (Pixel), XtOffsetOf(XmManagerRec, manager.top_shadow_color),
     XmRCallProc, (XtPointer) _XmTopShadowColorDefault
   },

   {
     XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap), XtOffsetOf(XmManagerRec, manager.top_shadow_pixmap),
     XmRCallProc, (XtPointer) _XmTopShadowPixmapDefault
   },

   {
     XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
     sizeof (Pixel), XtOffsetOf(XmManagerRec, manager.bottom_shadow_color),
     XmRCallProc, (XtPointer) _XmBottomShadowColorDefault
   },

   {
     XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap), XtOffsetOf(XmManagerRec, manager.bottom_shadow_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {
     XmNhelpCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList), XtOffsetOf(XmManagerRec, manager.help_callback),
     XmRPointer, (XtPointer) NULL
   },

#ifndef XM_PART_BC
   {
     XmNpopupHandlerCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList),
     XtOffsetOf(XmManagerRec, manager.popup_handler_callback),
     XmRPointer, (XtPointer) NULL
   },
#endif

   {
     XmNuserData, XmCUserData, XmRPointer,
     sizeof(XtPointer), XtOffsetOf(XmManagerRec, manager.user_data),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNtraversalOn, XmCTraversalOn, XmRBoolean,
     sizeof(Boolean), XtOffsetOf(XmManagerRec, manager.traversal_on),
     XmRImmediate, (XtPointer) TRUE
   },
   {
     XmNstringDirection, XmCStringDirection, XmRStringDirection,
     sizeof(XmStringDirection),
     XtOffsetOf(XmManagerRec, manager.string_direction),
     XmRImmediate, (XtPointer) XmSTRING_DIRECTION_DEFAULT
   },
   {
     XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
     sizeof(XmDirection),
     XtOffsetOf(XmManagerRec, manager.string_direction),
     XmRCallProc, (XtPointer) _XmDirectionDefault
   },
   {
     XmNinitialFocus, XmCInitialFocus, XmRWidget,
     sizeof(Widget), XtOffsetOf(XmManagerRec, manager.initial_focus),
     XmRImmediate, NULL
   }
};

/***************************************/
/*  Definition for synthetic resources */

static XmSyntheticResource syn_resources[] =
{
   { XmNx,
     sizeof (Position), XtOffsetOf(WidgetRec, core.x),
     GetXFromShell, XmeToHorizontalPixels },

   { XmNy,
     sizeof (Position), XtOffsetOf(WidgetRec, core.y),
     GetYFromShell,  XmeToVerticalPixels },

   { XmNwidth,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNheight,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.height),
     XmeFromVerticalPixels, XmeToVerticalPixels },

   { XmNborderWidth,
     sizeof (Dimension), XtOffsetOf(WidgetRec, core.border_width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNshadowThickness,
     sizeof (Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNstringDirection,
     sizeof(XmStringDirection),
     XtOffsetOf(XmManagerRec, manager.string_direction),
     _XmFromLayoutDirection, _XmToLayoutDirection }

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
    NULL,               		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    NULL,				/* getValuesPrehook	*/
    NULL,				/* getValuesPosthook	*/
    NULL,                               /* ClassPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    NULL                                /* focusChange          */
};

static CompositeClassExtensionRec compositeClassExtRec = {
    NULL,
    NULLQUARK,
    XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec),
    TRUE,                               /* accepts_objects */
};

static XmManagerClassExtRec managerClassExtRec = {
    NULL,
    NULLQUARK,
    XmManagerClassExtVersion,
    sizeof(XmManagerClassExtRec),
    NULL,                               /* traversal_children */
    ObjectAtPoint                       /* object_at_point */
};

/******************************************/
/*  The Manager class record definition.  */

externaldef(xmmanagerclassrec) XmManagerClassRec xmManagerClassRec =
{
   {
      (WidgetClass) &constraintClassRec,     /* superclass            */
     "XmManager",		             /* class_name	      */
      sizeof(XmManagerRec),                  /* widget_size	      */
      ClassInitialize,                       /* class_initialize      */
      ClassPartInitialize,                   /* class part initialize */
      False,                                 /* class_inited          */
      Initialize,                            /* initialize	      */
      NULL,                                  /* initialize hook       */
      Realize,                               /* realize	              */
      actions,                               /* actions               */
      XtNumber(actions),                     /* num_actions	      */
      resources,                             /* resources	      */
      XtNumber(resources),                   /* num_resources         */
      NULLQUARK,                             /* xrm_class	      */
      True,                                  /* compress_motion       */
      XtExposeCompressMaximal,               /* compress_exposure     */
      True,                                  /* compress enterleave   */
      False,                                 /* visible_interest      */
      Destroy,                               /* destroy               */
      NULL,                                  /* resize                */
      NULL,                                  /* expose                */
      SetValues,                             /* set_values	      */
      NULL,                                  /* set_values_hook       */
      XtInheritSetValuesAlmost,              /* set_values_almost     */
      _XmManagerGetValuesHook,               /* get_values_hook       */
      NULL,                                  /* accept_focus	      */
      XtVersion,                             /* version               */
      NULL,                                  /* callback private      */
      _XmManager_defaultTranslations,        /* tm_table              */
      NULL,                                  /* query geometry        */
      NULL,                                  /* display_accelerator   */
      (XtPointer)&baseClassExtRec,           /* extension             */
   },
   {					     /* composite class   */
      NULL,                                  /* Geometry Manager  */
      NULL,                                  /* Change Managed    */
      InsertChild,                           /* Insert Child      */
      DeleteChild,	                     /* Delete Child      */
      (XtPointer)&compositeClassExtRec,      /* extension         */
   },

   {						/* constraint class	*/
      NULL,					/* resources		*/
      0,					/* num resources	*/
      sizeof (XmManagerConstraintRec),	        /* constraint record	*/
      ConstraintInitialize,			/* initialize		*/
      ConstraintDestroy,			/* destroy		*/
      ConstraintSetValues,			/* set values		*/
      NULL,					/* extension		*/
   },

   {						/* manager class	  */
      _XmManager_managerTraversalTranslations,  /* default translations   */
      syn_resources,				/* syn resources      	  */
      XtNumber(syn_resources),			/* num_syn_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_syn_cont_resources */
      ManagerParentProcess,                     /* parent_process         */
      (XtPointer)&managerClassExtRec,		/* extension		  */
   },
};

externaldef(xmmanagerwidgetclass) WidgetClass xmManagerWidgetClass =
                                 (WidgetClass) &xmManagerClassRec;



static XmConst XmSpecifyLayoutDirectionTraitRec manLDT = {
  0,			/* version */
  GetDirection
};


/* Access Colors Trait record for Manager */

static XmConst XmAccessColorsTraitRec manACT = {
  0,			/* version */
  GetColors
};

/* Unit Type Trait record for Manager */

static XmConst XmSpecUnitTypeTraitRec manUTT = {
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
     * the parent's x relative to the origin, in pixels */

    Widget parent = XtParent(wid);

    if (XtIsShell(parent)) {
	/* at the moment menuShell doesn't reset x,y values to 0, so
	** we'll have them counted twice if we use XtTranslateCoords
	*/
        *value = (XtArgVal) parent->core.x;
    } else {
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
     * the parent's y relative to the origin, in pixels */

    Widget parent = XtParent(wid);

    if (XtIsShell(parent)) {
	/* at the moment menuShell doesn't reset x,y values to 0, so
	** we'll have them counted twice if we use XtTranslateCoords
	*/
        *value = (XtArgVal) parent->core.y;
    } else {
	*value = (XtArgVal) wid->core.y ;
	XmeFromVerticalPixels(wid,  resource_offset, value);
    }
}

/*********************************************************************
 *
 * ClassInitialize
 *
 *********************************************************************/
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


static CompositeClassExtension
FindCompClassExtension(
        WidgetClass widget_class )
{
    CompositeClassExtension ext;
    ext = (CompositeClassExtension)((CompositeWidgetClass)widget_class)
	       ->composite_class.extension;
    while ((ext != NULL) && (ext->record_type != NULLQUARK))
      ext = (CompositeClassExtension)ext->next_extension;

    if (ext != NULL) {
	/* fix for 9640: check for older version of the extension as well */
	if (  ext->version <= XtCompositeExtensionVersion &&
	      ext->record_size <= sizeof(CompositeClassExtensionRec)) {
	    /*EMPTY*/
	    /* continue */
	} else {
	    String params[1];
	    Cardinal num_params = 1;
	    params[0] = widget_class->core_class.class_name;
	    XtErrorMsg( "invalidExtension", "ManagerClassPartInitialize",
		        "XmToolkitError",
		       MESSAGE1, params, &num_params);
	}
    }
    return ext;
}



/**********************************************************************
 *
 *  BuildManagerResources
 *	Build up the manager's synthetic and constraint synthetic
 *	resource processing list by combining the super classes with
 *	this class.
 *
 **********************************************************************/
static void
BuildManagerResources(
        WidgetClass c )
{
    XmManagerWidgetClass wc = (XmManagerWidgetClass) c ;
    XmManagerWidgetClass sc;

    sc = (XmManagerWidgetClass) wc->core_class.superclass;

    _XmInitializeSyntheticResources(wc->manager_class.syn_resources,
				    wc->manager_class.num_syn_resources);

    _XmInitializeSyntheticResources(
			wc->manager_class.syn_constraint_resources,
			wc->manager_class.num_syn_constraint_resources);

    if (sc == (XmManagerWidgetClass) constraintWidgetClass) return;

    _XmBuildResources (&(wc->manager_class.syn_resources),
		       &(wc->manager_class.num_syn_resources),
		       sc->manager_class.syn_resources,
		       sc->manager_class.num_syn_resources);

    _XmBuildResources (&(wc->manager_class.syn_constraint_resources),
		       &(wc->manager_class.num_syn_constraint_resources),
		       sc->manager_class.syn_constraint_resources,
		       sc->manager_class.num_syn_constraint_resources);
}




/*********************************************************************
 *
 * ClassPartInitialize
 *
 *********************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
    static Boolean first_time = TRUE;
    XmManagerWidgetClass mw = (XmManagerWidgetClass) wc;
    XmManagerWidgetClass super = (XmManagerWidgetClass)
	                        mw->core_class.superclass;
    XmManagerClassExt *mext = (XmManagerClassExt *)
	_XmGetClassExtensionPtr((XmGenericClassExt *)
				&(mw->manager_class.extension), NULLQUARK) ;



    _XmFastSubclassInit (wc, XmMANAGER_BIT);

    /*** start by doing the inheritance for Xt... */
    if (FindCompClassExtension(wc) == NULL) {
	CompositeClassExtension comp_ext ;

	XtPointer *comp_extP
	    = &((CompositeWidgetClass)wc)->composite_class.extension;
	comp_ext = XtNew(CompositeClassExtensionRec);
	memcpy(comp_ext, FindCompClassExtension(wc->core_class.superclass),
	       sizeof(CompositeClassExtensionRec));
	comp_ext->next_extension = *comp_extP;
	*comp_extP = (XtPointer)comp_ext;
    }

    /*** deal with inheritance for regular methods */
    if (mw->manager_class.translations == XtInheritTranslations)
	mw->manager_class.translations = super->manager_class.translations;
    else if (mw->manager_class.translations)
	mw->manager_class.translations = (String)
	    XtParseTranslationTable(mw->manager_class.translations);

    if (mw->manager_class.parent_process == XmInheritParentProcess)
	mw->manager_class.parent_process =
	    super->manager_class.parent_process;

    /* synthetic resource management */
    BuildManagerResources((WidgetClass) wc);


    /*** then deal with class extension inheritance.
      if it's NULL, create a new one with inherit everywhere,
      then do the inheritance*/

    if (*mext == NULL) {
	*mext = (XmManagerClassExt) XtCalloc(1, sizeof(XmManagerClassExtRec));
	(*mext)->record_type     = NULLQUARK ;
	(*mext)->version = XmManagerClassExtVersion ;
	(*mext)->record_size     = sizeof( XmManagerClassExtRec) ;
	(*mext)->traversal_children = XmInheritTraversalChildrenProc ;
	(*mext)->object_at_point = XmInheritObjectAtPointProc ;
    }

    if ((WidgetClass)mw != xmManagerWidgetClass) {

	XmManagerClassExt *smext = (XmManagerClassExt *)
	    _XmGetClassExtensionPtr( (XmGenericClassExt *)
				    &(super->manager_class.extension),
				    NULLQUARK) ;

	if ((*mext)->traversal_children == XmInheritTraversalChildrenProc) {
	    (*mext)->traversal_children = (*smext)->traversal_children ;
	}
	if ((*mext)->object_at_point == XmInheritObjectAtPointProc) {
	    (*mext)->object_at_point = (*smext)->object_at_point ;
	}
    }


   /*** Carry this ugly non portable code that deal with Xt internals.
      first_time because we want to do that onlt once but
      after Object ClassPartInit has been called */
    if (first_time) {
        _XmReOrderResourceList(xmManagerWidgetClass, XmNunitType, NULL);
	_XmReOrderResourceList(xmManagerWidgetClass,
			       XmNforeground, XmNbackground);
        first_time = FALSE;
    }


    /*** setting up traits for all subclasses as well. */

    XmeTraitSet((XtPointer)wc, XmQTspecifyLayoutDirection, (XtPointer)&manLDT);
    XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&manACT);
    XmeTraitSet((XtPointer)wc, XmQTspecifyUnitType, (XtPointer)&manUTT);
}



/************************************************************************
 *
 *  Initialize
 *
 ************************************************************************/
static void
Initialize(
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmManagerWidget mw = (XmManagerWidget) new_w ;
    XtTranslations translations ;

   /*  Initialize manager and composite instance data  */

   mw->manager.selected_gadget = NULL;
   mw->manager.highlighted_widget = NULL;
   mw->manager.event_handler_added = False;
   mw->manager.active_child = NULL;
   mw->manager.keyboard_list = NULL;
   mw->manager.num_keyboard_entries = 0;
   mw->manager.size_keyboard_list = 0;
   mw->manager.has_focus = False;

   _XmProcessLock();
   translations = (XtTranslations) ((XmManagerClassRec *)XtClass( mw))
                                      ->manager_class.translations ;
   _XmProcessUnlock();

   if(    mw->manager.traversal_on
       && translations  &&  mw->core.tm.translations
       && !XmIsRowColumn( mw)    )
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
       XtOverrideTranslations( (Widget) mw, translations) ;
       }

   if(    (mw->manager.navigation_type != XmDYNAMIC_DEFAULT_TAB_GROUP)
       && !XmRepTypeValidValue( XmRID_NAVIGATION_TYPE,
                        mw->manager.navigation_type, (Widget) mw)    )
   {   mw->manager.navigation_type = XmNONE ;
       }
   _XmNavigInitialize( request, new_w, args, num_args);

   /*  Verify resource data  */

   if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE, mw->manager.unit_type,
                                          (Widget) mw)    )
   {
      mw->manager.unit_type = XmPIXELS;
   }

   /*  Convert the fields from unit values to pixel values  */

   _XmManagerImportArgs( (Widget) mw, args, num_args);

    if (mw->manager.string_direction == XmSTRING_DIRECTION_DEFAULT) {
      /* This indicates that layoutDirection was set in the arglist, but
         stringDirection defaulting overwrote the value */
      int i;
      for (i=0; i < *num_args; i++)
	if (strcmp(args[i].name, XmNlayoutDirection) == 0)
	  mw->manager.string_direction = (XmDirection)args[i].value;
    }

   /*  Get the shadow drawing GC's  */

    mw->manager.background_GC =
	_XmGetPixmapBasedGC (new_w,
			     mw->core.background_pixel,
			     mw->manager.foreground,
			     mw->core.background_pixmap);
    mw->manager.highlight_GC =
	_XmGetPixmapBasedGC (new_w,
			     mw->manager.highlight_color,
			     mw->core.background_pixel,
			     mw->manager.highlight_pixmap);
    mw->manager.top_shadow_GC =
	_XmGetPixmapBasedGC (new_w,
			     mw->manager.top_shadow_color,
			     mw->core.background_pixel,
			     mw->manager.top_shadow_pixmap);
    mw->manager.bottom_shadow_GC =
	_XmGetPixmapBasedGC (new_w,
			     mw->manager.bottom_shadow_color,
			     mw->core.background_pixel,
			     mw->manager.bottom_shadow_pixmap);

   /* Copy accelerator widget from parent or set to NULL.
    */
    {
      XmManagerWidget p = (XmManagerWidget) XtParent(mw);

      if (XmIsManager(p) && p->manager.accelerator_widget)
         mw->manager.accelerator_widget = p->manager.accelerator_widget;
      else
         mw->manager.accelerator_widget = NULL;
    }
}




/*************************************************************************
 *
 *  Realize
 *
 *************************************************************************/
static void
Realize(
        Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
   Mask valueMask = *p_valueMask;

    /*	Make sure height and width are not zero.
    */
   if (!XtWidth(w)) XtWidth(w) = 1 ;
   if (!XtHeight(w)) XtHeight(w) = 1 ;

   valueMask |= CWBitGravity | CWDontPropagate;
   attributes->bit_gravity = NorthWestGravity;
   attributes->do_not_propagate_mask =
      ButtonPressMask | ButtonReleaseMask |
      KeyPressMask | KeyReleaseMask | PointerMotionMask;

   XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);
}



/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
static void
Destroy(
        Widget w )
{
   XmManagerWidget	mw = (XmManagerWidget) w;

   _XmNavigDestroy(w);

   XtReleaseGC (w, mw->manager.background_GC);
   XtReleaseGC (w, mw->manager.top_shadow_GC);
   XtReleaseGC (w, mw->manager.bottom_shadow_GC);
   XtReleaseGC (w, mw->manager.highlight_GC);
}




/************************************************************************
 *
 *  SetValues
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
    Boolean returnFlag = False;
    Mask visualFlag = NoVisualChange;
    XmManagerWidget curmw = (XmManagerWidget) current;
    XmManagerWidget newmw = (XmManagerWidget) new_w;

    /*  Process the change in values */

    /* CR 7124: XmNlayoutDirection and XmNstringDirection are CG resources. */
    if (curmw->manager.string_direction != newmw->manager.string_direction)
      {
	XmeWarning(new_w, MESSAGE2);
	newmw->manager.string_direction = curmw->manager.string_direction;
      }

    /* If traversal has been turned on, then augment the translations
    *    of the new widget.
    */
    if(    newmw->manager.traversal_on
        && (newmw->manager.traversal_on != curmw->manager.traversal_on)
        && newmw->core.tm.translations) {

    	   XtTranslations translations;
	   _XmProcessLock();
	   translations = (XtTranslations)
	       ((XmManagerClassRec *) XtClass( newmw))
		   ->manager_class.translations;
	   _XmProcessUnlock();

	   if (translations)
        	XtOverrideTranslations( (Widget) newmw, translations);
        }
    if(    newmw->manager.initial_focus != curmw->manager.initial_focus    )
      {
	_XmSetInitialOfTabGroup( (Widget) newmw,
				newmw->manager.initial_focus) ;
      }

    if( curmw->manager.navigation_type != newmw->manager.navigation_type   )
      {
	if(    !XmRepTypeValidValue( XmRID_NAVIGATION_TYPE,
			 newmw->manager.navigation_type, (Widget) newmw)    )
	  {
	    newmw->manager.navigation_type = curmw->manager.navigation_type ;
	  }
      }

    returnFlag = _XmNavigSetValues(current, request, new_w, args, num_args);

    /*  Validate changed data.  */

    if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE, newmw->manager.unit_type,
                                                        (Widget) newmw)    )
    {
       newmw->manager.unit_type = curmw->manager.unit_type;
    }

   /*  Convert the necessary fields from unit values to pixel values  */

   _XmManagerImportArgs( (Widget) newmw, args, num_args);

    /*  Checking for valid Direction */
    if(    !XmRepTypeValidValue( XmRID_DIRECTION,
                       newmw->manager.string_direction, (Widget) newmw)    )
    {
        newmw->manager.string_direction = curmw->manager.string_direction ;
    }

   /*  If either of the background, shadow, or highlight colors or  */
   /*  pixmaps have changed, destroy and recreate the gc's.         */

   if (curmw->core.background_pixel != newmw->core.background_pixel ||
       curmw->core.background_pixmap != newmw->core.background_pixmap)
   {
      XtReleaseGC ( (Widget) newmw, newmw->manager.background_GC);
      newmw->manager.background_GC =
	  _XmGetPixmapBasedGC (new_w,
			       newmw->core.background_pixel,
			       newmw->manager.foreground,
			       newmw->core.background_pixmap);
      returnFlag = True;
      visualFlag |= (VisualBackgroundPixel|VisualBackgroundPixmap);
   }

   if (curmw->manager.top_shadow_color != newmw->manager.top_shadow_color ||
       curmw->manager.top_shadow_pixmap != newmw->manager.top_shadow_pixmap)
   {
      XtReleaseGC ((Widget) newmw, newmw->manager.top_shadow_GC);
      newmw->manager.top_shadow_GC =
	_XmGetPixmapBasedGC (new_w,
			     newmw->manager.top_shadow_color,
			     newmw->core.background_pixel,
			     newmw->manager.top_shadow_pixmap);
      returnFlag = True;
      visualFlag |= (VisualTopShadowColor|VisualTopShadowPixmap);
   }

   if (curmw->manager.bottom_shadow_color !=
          newmw->manager.bottom_shadow_color ||
       curmw->manager.bottom_shadow_pixmap !=
          newmw->manager.bottom_shadow_pixmap)
   {
      XtReleaseGC ((Widget) newmw, newmw->manager.bottom_shadow_GC);
      newmw->manager.bottom_shadow_GC =
	  _XmGetPixmapBasedGC (new_w,
			       newmw->manager.bottom_shadow_color,
			       newmw->core.background_pixel,
			       newmw->manager.bottom_shadow_pixmap);
      returnFlag = True;
      visualFlag |= (VisualBottomShadowColor|VisualBottomShadowPixmap);
   }

   if (curmw->manager.highlight_color != newmw->manager.highlight_color ||
       curmw->manager.highlight_pixmap != newmw->manager.highlight_pixmap)
   {
      XtReleaseGC ((Widget) newmw, newmw->manager.highlight_GC);
      newmw->manager.highlight_GC =
	_XmGetPixmapBasedGC (new_w,
			     newmw->manager.highlight_color,
			     newmw->core.background_pixel,
			     newmw->manager.highlight_pixmap);
      returnFlag = True;
      visualFlag |= (VisualHighlightColor|VisualHighlightPixmap);
   }

   if (curmw->manager.foreground != newmw->manager.foreground)
      visualFlag |= VisualForeground ;


   /*  Inform children of possible visual changes  */

   if (visualFlag)
      returnFlag |= _XmNotifyChildrenVisual (current, new_w, visualFlag);


   /*  Return flag to indicate if redraw is needed.  */

   return (returnFlag);
}





/*********************************************************************
 *
 * InsertChild
 *
 *********************************************************************/
static void
InsertChild(
        Widget child )
{
    CompositeClassRec *cc = (CompositeClassRec *) compositeWidgetClass;
    XtWidgetProc insert_child;

    if (!XtIsRectObj(child))
	return;

    _XmProcessLock();
    insert_child = cc->composite_class.insert_child;
    _XmProcessUnlock();
    (*insert_child)(child);
}

/*********************************************************************
 *
 * DeleteChild
 *
 *********************************************************************/
static void
DeleteChild(
        Widget child )
{
    XmManagerWidget mw = (XmManagerWidget) child->core.parent;
    Widget tab_group ;
    CompositeClassRec *cc = (CompositeClassRec *) compositeWidgetClass;
    XtWidgetProc delete_child;

    if (!XtIsRectObj(child))
	return;

    if (mw->manager.selected_gadget == (XmGadget) child)
	mw->manager.selected_gadget = NULL;

    if(    mw->manager.initial_focus == child    )
	{
	    mw->manager.initial_focus = NULL ;
	}
    if(    mw->manager.active_child == child    )
	{
	    mw->manager.active_child = NULL;
	}
    if(    (tab_group = XmGetTabGroup( child))
       &&  (tab_group != (Widget) mw)
       &&  XmIsManager( tab_group)
       &&  (((XmManagerWidget) tab_group)->manager.active_child == child) )
	{
	    ((XmManagerWidget) tab_group)->manager.active_child = NULL ;
	}

    _XmProcessLock();
    delete_child = cc->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)(child);
}




/************************************************************************
 *
 *  ManagerMotion
 *	This function handles the generation of motion, enter, and leave
 *	window events for gadgets and the dispatching of these events to
 *	the gadgets.
 *
 ************************************************************************/
static void
ManagerMotion(
        Widget wid,
        XtPointer closure,
        XEvent *event,
        Boolean *cont)
{
   XmManagerWidget mw = (XmManagerWidget) wid ;
   XmGadget gadget;
   XmGadget oldGadget;

   /*  Event on the managers window and not propagated from a child  */

   /* Old comment from 1.1:
    * ManagerMotion() creates misleading Enter/Leave events.  A race condition
    * exists such that it's possible that when ManagerMotion() is called, the
    * manager does not yet have the focus.  Dropping the Enter on the floor
    * caused ManagerMotion() to translate the first subsequent motion event
    * into an enter to dispatch to the gadget.  Subsequently button gadget
    * (un)highlighting on enter/leave was unreliable.  This problem requires
    * additional investigation.
    * The quick fix, currently, is for ManagerEnter()
    * and ManagerLeave() to use the event whether or not the manager has the
    * focus.
    * In addition, in dispatching enter/leaves to gadgets here in this
    * routine, ManagerMotion(), bear in mind that we are passing a
    * XPointerMovedEvent and should probably be creating a synthethic
    * XCrossingEvent instead.
    *
    * if ((event->subwindow != 0) || !mw->manager.has_focus)
    */

    /* CR 9362:
     * ManagerMotion() was not keeping track of Enter/Leave events.
     * Pointer motion on a gadget which extended beyond the manager's
     * geometry was not being tracked correctly.  Thus we now use the
     * has_focus flag to toggle between enter/leave states. Use of this
     * flag prevents us from having to use _XmGetPointVisibility which is
     * expensive.
     * Prior to 1.1, xcrossing.focus was used, but this proved incorrect,
     * as the 1.1 comment above describes.
     */
   (void)closure;
   (void)cont;
   if (event->xmotion.subwindow != 0 || !mw->manager.has_focus)
      return;

   gadget = _XmInputForGadget((Widget) mw, event->xmotion.x,
			      event->xmotion.y);
   oldGadget = (XmGadget) mw->manager.highlighted_widget;


   /*  Dispatch motion events to the child  */

   if (gadget != NULL)
   {
      if (gadget->gadget.event_mask & XmMOTION_EVENT)
         _XmDispatchGadgetInput((Widget) gadget, event, XmMOTION_EVENT);
   }


   /*  Check for and process a leave window condition  */

   if (oldGadget != NULL && gadget != oldGadget)
   {
      if (oldGadget->gadget.event_mask & XmLEAVE_EVENT)
         _XmDispatchGadgetInput( (Widget) oldGadget, event, XmLEAVE_EVENT);

      mw->manager.highlighted_widget = NULL;
   }


   /*  Check for and process an enter window condition  */

   if (gadget != NULL && gadget != oldGadget)
   {
      if (gadget->gadget.event_mask & XmENTER_EVENT)
      {
         _XmDispatchGadgetInput( (Widget) gadget, event, XmENTER_EVENT);
         mw->manager.highlighted_widget = (Widget) gadget;
      }
      else
         mw->manager.highlighted_widget = NULL;
   }
}




/************************************************************************
 *
 *  ManagerEnter
 *	This function handles the generation of motion and enter window
 *	events for gadgets and the dispatching of these events to the
 *	gadgets.
 *
 ************************************************************************/
static void
ManagerEnter(
        Widget wid,
        XtPointer closure,
        XEvent *event,
        Boolean *cont)
{
   XmManagerWidget mw = (XmManagerWidget) wid ;
   XmGadget gadget;

   /* Old comment from 1.1:
    * See ManagerMotion()
    * if (!(mw->manager.has_focus = (Boolean) event->xcrossing.focus))
    *    return;
    */

    /* Toggle to entered state */
    (void)closure;
    (void)cont;
    mw->manager.has_focus = True;

   /*
    * call the traversal action in order to synch things up. This
    * should be cleaned up into a single module |||
    */
   _XmManagerEnter((Widget) mw, event, NULL, NULL);

   gadget = _XmInputForGadget( (Widget) mw, event->xcrossing.x,
                                           event->xcrossing.y);
   /*  Dispatch motion and enter events to the child  */

   if (gadget != NULL)
   {
      if (gadget->gadget.event_mask & XmMOTION_EVENT)
         _XmDispatchGadgetInput( (Widget) gadget, event, XmMOTION_EVENT);

      if (gadget->gadget.event_mask & XmENTER_EVENT)
      {
         _XmDispatchGadgetInput( (Widget) gadget, event, XmENTER_EVENT);
         mw->manager.highlighted_widget = (Widget) gadget;
      }
      else
         mw->manager.highlighted_widget = NULL;

   }
}




/************************************************************************
 *
 *  ManagerLeave
 *	This function handles the generation of leave window events for
 *	gadgets and the dispatching of these events to the gadgets.
 *
 ************************************************************************/
static void
ManagerLeave(
        Widget wid,
        XtPointer closure,
        XEvent *event,
        Boolean *cont)
{
   XmManagerWidget mw = (XmManagerWidget) wid ;
   XmGadget oldGadget;

   /* See ManagerMotion()
    * if (!(mw->manager.has_focus = (Boolean) event->xcrossing.focus))
    *    return;
    */

    /* Toggle to leave state */
    (void)closure;
    (void)cont;
    mw->manager.has_focus = False;

   oldGadget = (XmGadget) mw->manager.highlighted_widget;

   if (oldGadget != NULL)
   {
      if (oldGadget->gadget.event_mask & XmLEAVE_EVENT)
         _XmDispatchGadgetInput( (Widget) oldGadget, event, XmLEAVE_EVENT);
      mw->manager.highlighted_widget = NULL;
   }
   /*
    * call the traversal action in order to synch things up. This
    * should be cleaned up into a single module |||
    */
   _XmManagerLeave( (Widget) mw, event, NULL, NULL);

}



/************************************************************************
 *
 *  AddMotionHandlers
 *	Add the event handlers necessary to synthisize motion events
 *	for gadgets.
 *
 ************************************************************************/
static void
AddMotionHandlers(
        XmManagerWidget mw )
{
   mw->manager.event_handler_added = True;
#if 1
     /* for tool tips */
     XtAddEventHandler ((Widget) mw, PointerMotionMask, False,
			ManagerMotion, NULL);
#else
   /* The first version in this #ifdef is superior because it
      involves lower network traffic,  but causes problems in
      VTS and automation (CR 8943).  We can reexamine this later */
   if( _XmGetFocusPolicy( (Widget) mw) != XmEXPLICIT ) {
     XtAddEventHandler ((Widget) mw, PointerMotionMask, False,
			ManagerMotion, NULL);
   } else {
     XtAddEventHandler ((Widget) mw, ButtonMotionMask, False,
			ManagerMotion, NULL);
   }
#endif

   XtAddEventHandler ((Widget) mw, EnterWindowMask, False,
		      ManagerEnter, NULL);
   XtAddEventHandler ((Widget) mw, LeaveWindowMask, False,
		      ManagerLeave, NULL);
}


/************************************************************************
 *
 *  ConstraintInitialize
 *	The constraint destroy procedure checks to see if a gadget
 *	child is being destroyed.  If so, the managers motion processing
 *	event handlers are checked to see if they need to be removed.
 *
 ************************************************************************/
static void
ConstraintInitialize(
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal * num_args)
{
   XmGadget g;
   XmManagerWidget parent;

   (void)request;
   (void)args;
   (void)num_args;
   if (!XtIsRectObj(new_w)) return;

   parent = (XmManagerWidget) new_w->core.parent;

   if (XmIsGadget (new_w))
   {
      g = (XmGadget) new_w;


      if ((g->gadget.event_mask &
           (XmENTER_EVENT | XmLEAVE_EVENT | XmMOTION_EVENT)) &&
           parent->manager.event_handler_added == False)
         AddMotionHandlers (parent);
   }
   else if (XtIsWidget(new_w))
     {
       if (parent->manager.accelerator_widget)
         {
           XtInstallAccelerators (new_w, parent->manager.accelerator_widget);
         }
       /* else was the DoMagicCompatibility */
     }
   /* else non-widget non-gadget RectObj */
}





/************************************************************************
 *
 *  CheckRemoveMotionHandlers
 *	This function loops through the child set checking each gadget
 *	to see if the need motion events or not.  If no gadget's need
 *	motion events and the motion event handlers have been added,
 *	then remove the event handlers.
 *
 ************************************************************************/
static void
CheckRemoveMotionHandlers(
        XmManagerWidget mw )
{
   int i;
   Widget child;


   /*  If there are any gadgets which need motion events, return.  */

   if (!mw->core.being_destroyed)
   {
      for (i = 0; i < mw->composite.num_children; i++)
      {
         child = mw->composite.children[i];

         if (XmIsGadget(child))
         {
            if (((XmGadget) child)->gadget.event_mask &
                (XmENTER_EVENT | XmLEAVE_EVENT | XmMOTION_EVENT))
            return;
         }
      }
   }


   /*  Remove the motion event handlers  */

   mw->manager.event_handler_added = False;

   XtRemoveEventHandler ((Widget) mw, PointerMotionMask, False,
			 ManagerMotion, NULL);
   XtRemoveEventHandler ((Widget) mw, EnterWindowMask, False,
			 ManagerEnter, NULL);
   XtRemoveEventHandler ((Widget) mw, LeaveWindowMask, False,
			 ManagerLeave, NULL);
}




/************************************************************************
 *
 *  ConstraintDestroy
 *	The constraint destroy procedure checks to see if a gadget
 *	child is being destroyed.  If so, the managers motion processing
 *	event handlers are checked to see if they need to be removed.
 *
 ************************************************************************/
static void
ConstraintDestroy(
        Widget w )
{
   XmGadget g;
   XmManagerWidget parent;

   if (!XtIsRectObj(w)) return;

   if (XmIsGadget (w))
   {
      g = (XmGadget) w;
      parent = (XmManagerWidget) w->core.parent;

      if (g->gadget.event_mask &
          (XmENTER_EVENT | XmLEAVE_EVENT | XmMOTION_EVENT))
         CheckRemoveMotionHandlers (parent);

      if (parent->manager.highlighted_widget == w)
         parent->manager.highlighted_widget = NULL;

      if (parent->manager.selected_gadget == g)
         parent->manager.selected_gadget = NULL;
   }
}



/************************************************************************
 *
 *  ConstraintSetValues
 *	Make sure the managers event handler is set appropriately for
 *	gadget event handling.
 *
 ************************************************************************/
static Boolean
ConstraintSetValues(
        Widget current,
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args)
{
   XmGadget currentg, newg;
   XmManagerWidget parent;
   unsigned int motion_events;

   (void)request;
   (void)args;
   (void)num_args;
   if (!XtIsRectObj(new_w)) return False;

   /*  If the child is a gadget and its event mask has changed with  */
   /*  respect to the event types which need motion events on the    */
   /*  parent.                                                       */

   if (XmIsGadget (new_w))
   {
      currentg = (XmGadget) current;
      newg = (XmGadget) new_w;
      parent = (XmManagerWidget) new_w->core.parent;

      motion_events = XmENTER_EVENT | XmLEAVE_EVENT | XmMOTION_EVENT;

      if ((newg->gadget.event_mask & motion_events) !=
          (currentg->gadget.event_mask & motion_events))
      {
         if ((newg->gadget.event_mask & motion_events) &&
             parent->manager.event_handler_added == False)
            AddMotionHandlers (parent);

         if ((~(newg->gadget.event_mask & motion_events)) &&
             parent->manager.event_handler_added == True)
            CheckRemoveMotionHandlers (parent);
      }
   }

   return (False);
}

/****************************************************************/
static Boolean
ManagerParentProcess(
        Widget widget,
        XmParentProcessData data )
{

    return( _XmParentProcess( XtParent( widget), data)) ;
}

/************************************************************************
 *
 *  ObjectAtPoint method
 *	Given a composite widget and an (x, y) coordinate, see if the
 *	(x, y) lies within one of the gadgets contained within the
 *	composite.  Return the gadget if found, otherwise return NULL.
 *
 ************************************************************************/
static Widget
ObjectAtPoint(
        Widget wid,
        Position  x,
        Position  y )
{
    CompositeWidget cw = (CompositeWidget) wid ;
    int i;
    Widget widget;

   /* For the case of overlapping gadgets, the last one in the
    * composite list will be the visible gadget (see order of
    * redisplay in XmeRedisplayGadgets).  So, search the child
    * list from the tail to the head to get this visible gadget
    * as the one to get the input.
    */
    i = cw->composite.num_children ;
    while( i-- ) {
	widget = cw->composite.children[i];

	if (XmIsGadget(widget) && XtIsManaged (widget)) {
	    if (x >= widget->core.x && y >= widget->core.y &&
		x < widget->core.x + widget->core.width    &&
		y < widget->core.y + widget->core.height)
		return (widget);
	}
    }

    return (NULL);
}



static XmNavigability
WidgetNavigable(
        Widget wid)
{
    if(    XtIsSensitive(wid)
       &&  ((XmManagerWidget) wid)->manager.traversal_on    )
	{
	    XmNavigationType nav_type = ((XmManagerWidget) wid)
		->manager.navigation_type ;
	    if(    (nav_type == XmSTICKY_TAB_GROUP)
	       ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	       ||  (    (nav_type == XmTAB_GROUP)
		    &&  !_XmShellIsExclusive( wid))    )
		{
		    return XmDESCENDANTS_TAB_NAVIGABLE ;
		}
	    return XmDESCENDANTS_NAVIGABLE ;
	}
    return XmNOT_NAVIGABLE ;
}


/****************************************************************
 ****************************************************************
 **
 ** Trait methods
 **
 ****************************************************************
 ****************************************************************/

static XmDirection
GetDirection(Widget w)
{
  return (XmDirection)((XmManagerWidget)(w))->manager.string_direction;
}

static unsigned char
GetUnitType(Widget w)
{
    return ((XmManagerWidget) w)->manager.unit_type ;
}


static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
    XmManagerWidget mw = (XmManagerWidget) w ;

    color_data->valueMask = AccessForeground | AccessBackgroundPixel |
	AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
    color_data->background = w->core.background_pixel;
    color_data->foreground = mw->manager.foreground;
    color_data->highlight_color = mw->manager.highlight_color;
    color_data->top_shadow_color = mw->manager.top_shadow_color;
    color_data->bottom_shadow_color = mw->manager.bottom_shadow_color;
}



/****************************************************************
 ****************************************************************
 **
 ** External functions, both _Xm and Xm.
 ** First come the action procs and then the other external entry points.
 **
 ****************************************************************
 ****************************************************************/
void
_XmGadgetTraversePrevTabGroup(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  Widget ref_wid = ((XmManagerWidget) wid)->manager.active_child ;
  Boolean button_tab;
  XmDisplay xm_dpy;

  (void)event;
  (void)params;
  (void)num_params;
  if (!ref_wid)
    ref_wid = wid ;

  xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplayOfObject(ref_wid));
  button_tab = xm_dpy->display.enable_button_tab;

  if (button_tab)
    _XmMgrTraversal(ref_wid,  XmTRAVERSE_GLOBALLY_BACKWARD);
  else
    _XmMgrTraversal(ref_wid,  XmTRAVERSE_PREV_TAB_GROUP);
}

void
_XmGadgetTraverseNextTabGroup(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  Widget ref_wid = ((XmManagerWidget) wid)->manager.active_child ;
  Boolean button_tab;
  XmDisplay xm_dpy;

  (void)event;
  (void)params;
  (void)num_params;
  if (!ref_wid)
    ref_wid = wid;

  xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplayOfObject(ref_wid));
  button_tab = xm_dpy->display.enable_button_tab;

  if (button_tab)
    _XmMgrTraversal(ref_wid, XmTRAVERSE_GLOBALLY_FORWARD);
  else
    _XmMgrTraversal(ref_wid, XmTRAVERSE_NEXT_TAB_GROUP);
}

void
_XmGadgetTraverseCurrent(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget child;

	(void)params;
	(void)num_params;
	child = (Widget)_XmInputForGadget(wid, event->xbutton.x, event->xbutton.y);
	XmProcessTraversal(child, XmTRAVERSE_CURRENT);
}

void
_XmGadgetTraverseLeft(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_LEFT);
}

void
_XmGadgetTraverseRight(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_RIGHT);
}

void
_XmGadgetTraverseUp(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_UP);
}

void
_XmGadgetTraverseDown(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_DOWN);
}

void
_XmGadgetTraverseNext(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_NEXT);
}

void
_XmGadgetTraversePrev(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_PREV);
}

void
_XmGadgetTraverseHome(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	Widget ref_wid = ((XmManagerWidget)wid)->manager.active_child;

	(void)event;
	(void)params;
	(void)num_params;

	if (!ref_wid)
		ref_wid = wid;
	_XmMgrTraversal(ref_wid, XmTRAVERSE_HOME);
}

void
_XmGadgetSelect(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
   XmManagerWidget mw = (XmManagerWidget) wid ;
            Widget child ;

    (void)params;
    (void)num_params;
    if(    _XmGetFocusPolicy( (Widget) mw) == XmEXPLICIT    )
    {
        child = mw->manager.active_child ;
        if(    child  &&  !XmIsGadget( child)    )
        {   child = NULL ;
            }
        }
    else /* FocusPolicy == XmPOINTER */
    {   child = (Widget) _XmInputForGadget( (Widget) mw, event->xkey.x, event->xkey.y) ;
        }
    if(    child
        && (((XmGadgetClass)XtClass( child))->gadget_class.arm_and_activate)  )
    {
        (*(((XmGadgetClass)XtClass( child))->gadget_class.arm_and_activate))(
                                                    child, event, NULL, NULL) ;
        }
    return ;
    }

void
_XmManagerParentActivate(
        Widget mw,
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

    _XmParentProcess( mw, (XmParentProcessData) &pp_data);
}

void
_XmManagerParentCancel(
        Widget mw,
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

    _XmParentProcess( mw, (XmParentProcessData) &pp_data) ;
    }

void
_XmGadgetButtonMotion(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
   XmManagerWidget mw = (XmManagerWidget) wid ;
            Widget child ;

    (void)params;
    (void)num_params;
    if(    _XmGetFocusPolicy( (Widget) mw) == XmEXPLICIT    )
    {
        child = mw->manager.active_child ;
        if(    child  &&  !XmIsGadget( child)    )
        {   child = NULL ;
            }
        }
    else /* FocusPolicy == XmPOINTER */
    {   child = (Widget) _XmInputForGadget( (Widget) mw, event->xmotion.x,
					   event->xmotion.y) ;
        }
    if(    child    )
    {   _XmDispatchGadgetInput( child, event, XmMOTION_EVENT);
        }
    return ;
    }

void
_XmGadgetKeyInput(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
            XmManagerWidget mw = (XmManagerWidget) wid ;
            Widget child ;

    (void)params;
    (void)num_params;
    if(    _XmGetFocusPolicy( (Widget) mw) == XmEXPLICIT    )
    {
        child = mw->manager.active_child ;
        if(    child  &&  !XmIsGadget( child)    )
        {   child = NULL ;
            }
        }
    else /* FocusPolicy == XmPOINTER */
	{
	    child = (Widget) _XmInputForGadget( (Widget) mw,
					       event->xkey.x, event->xkey.y) ;
        }
    if(    child    )
    {   _XmDispatchGadgetInput( child, event, XmKEY_EVENT);
        }
    return ;
    }

void
_XmGadgetArm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
    XmManagerWidget mw = (XmManagerWidget) wid ;
    XmGadget gadget;

    (void)params;
    (void)num_params;
    if ((gadget = _XmInputForGadget( (Widget) mw, event->xbutton.x,
				    event->xbutton.y)) != NULL)
    {
	XmProcessTraversal( (Widget) gadget, XmTRAVERSE_CURRENT);
        _XmDispatchGadgetInput( (Widget) gadget, event, XmARM_EVENT);
        mw->manager.selected_gadget = gadget;
    }
    else
      {
        if(    _XmIsNavigable( wid)    )
          {
            XmProcessTraversal( wid, XmTRAVERSE_CURRENT) ;
          }
      }

    mw->manager.eligible_for_multi_button_event = NULL;
}

void
_XmGadgetDrag(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
    XmManagerWidget mw = (XmManagerWidget) wid ;
    XmGadget gadget;

    /* CR 5141: Don't let multi-button drags cause confusion. */
    (void)params;
    (void)num_params;
    if ( !(event->xbutton.state &
         ~((Button1Mask >> 1) << event->xbutton.button) &
         (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))
	&& (gadget = _XmInputForGadget((Widget) mw, event->xbutton.x,
				    event->xbutton.y)) != NULL)
    {
        _XmDispatchGadgetInput( (Widget) gadget, event, XmBDRAG_EVENT);
        mw->manager.selected_gadget = gadget;
    }

    mw->manager.eligible_for_multi_button_event = NULL;
}

void
_XmGadgetActivate(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
        XmManagerWidget mw = (XmManagerWidget) wid ;
        XmGadget gadget;

    /* we emulate automatic grab with owner_events = false by sending
     * the button up to the button down gadget
     */
    (void)params;
    (void)num_params;
    if ((gadget = mw->manager.selected_gadget) != NULL)
    {
        _XmDispatchGadgetInput( (Widget) gadget, event, XmACTIVATE_EVENT);
        mw->manager.selected_gadget = NULL;
        mw->manager.eligible_for_multi_button_event = gadget;
        }
}


void
_XmManagerHelp(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
        XmManagerWidget mw = (XmManagerWidget) wid ;
	Widget widget;

    (void)params;
    (void)num_params;
	if (!_XmIsEventUnique(event))
	   return;

        if (_XmGetFocusPolicy( (Widget) mw) == XmEXPLICIT)
        {
          if ((widget = mw->manager.active_child) != NULL)
             _XmDispatchGadgetInput(widget, event, XmHELP_EVENT);
          else
             _XmSocorro( (Widget) mw, event, NULL, NULL);
        }
        else
        {
	    if ((widget = XmObjectAtPoint( (Widget) mw,
					  event->xkey.x,
					  event->xkey.y)) != NULL)
               _XmDispatchGadgetInput(widget, event, XmHELP_EVENT);
          else
               _XmSocorro( (Widget) mw, event, NULL, NULL);
        }

	_XmRecordEvent(event);
}


void
_XmGadgetMultiArm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmManagerWidget mw = (XmManagerWidget) wid ;
    XmGadget gadget;

    gadget = _XmInputForGadget( (Widget) mw, event->xbutton.x,
			       event->xbutton.y);
    /*
     * If we're not set up for multi_button events, check to see if the
     * input gadget has changed from the active_child.  This means that the
     * user is quickly clicking between gadgets of this manager widget.
     * If so, arm the gadget as if it were the first button press.
     */
    if (mw->manager.eligible_for_multi_button_event &&
	((gadget = _XmInputForGadget( (Widget) mw, event->xbutton.x,
				     event->xbutton.y)) ==
	  mw->manager.eligible_for_multi_button_event))
    {
        _XmDispatchGadgetInput( (Widget) gadget, event, XmMULTI_ARM_EVENT);
	    mw->manager.selected_gadget = gadget;
    }
    else
       if (gadget && (gadget != (XmGadget)mw->manager.active_child))
	   _XmGadgetArm( (Widget) mw, event, params, num_params);
       else
	   mw->manager.eligible_for_multi_button_event = NULL;
}

void
_XmGadgetMultiActivate(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmManagerWidget mw = (XmManagerWidget) wid ;
    XmGadget gadget;

    /*
     * If we're not set up for multi_button events, call _XmGadgetActivate
     * in case we're quickly selecting a new gadget in which it should
     * be activated as if it were the first button press.
     */
    if (mw->manager.eligible_for_multi_button_event &&
	   ((gadget = mw->manager.selected_gadget) ==
	      mw->manager.eligible_for_multi_button_event))
    {
        _XmDispatchGadgetInput((Widget) gadget, event,
		   XmMULTI_ACTIVATE_EVENT);
    }
    else
       _XmGadgetActivate( (Widget) mw, event, params, num_params);
}

