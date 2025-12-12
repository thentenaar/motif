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
static char rcsid[] = "$TOG: SeparatoG.c /main/18 1999/01/26 15:32:56 mgreess $"
#endif
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/AccColorT.h>
#include <Xm/CareVisualT.h>
#include <Xm/DrawP.h>
#include <Xm/LabelGP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "ColorI.h"
#include "ExtObjectI.h"
#include "PixConvI.h"
#include "RepTypeI.h"
#include "SeparatoGI.h"
#include "SyntheticI.h"
#include "XmI.h"


/********    Static Function Declarations    ********/

static void SetTopShadowPixmapDefault(
				      Widget widget,
				      int offset,
				      XrmValue * value);
static void InputDispatch( 
                        Widget sg,
                        XEvent *event,
                        Mask event_mask) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void SecondaryObjectCreate( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
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
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetSeparatorGC( 
                        XmSeparatorGadget sg) ;
static void GetBackgroundGC(
                        XmSeparatorGadget sg) ;
static void Redisplay( 
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Destroy( 
                        Widget sg) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Help( 
                        Widget sg,
                        XEvent *event) ;
static Cardinal GetSeparatorGClassSecResData( 
                        WidgetClass w_class,
                        XmSecondaryResourceData **data_rtn) ;
static XtPointer GetSeparatorGClassSecResBase( 
                        Widget widget,
                        XtPointer client_data) ;
static Boolean HandleRedraw (Widget kid, 
		       Widget cur_parent,
		       Widget new_parent,
		       Mask visual_flag);
static void 
InitNewColorBehavior(
		     XmSeparatorGadget sg);
static void DealWithColors(
			   XmSeparatorGadget sg);
static void DealWithPixmaps(
			   XmSeparatorGadget sg);
static void InitNewPixmapBehavior(
			   XmSeparatorGadget sg);

static void GetColors(Widget widget, 
		      XmAccessColorData color_data);

/********    End Static Function Declarations    ********/

/* Menu Savvy trait record */
static XmConst XmMenuSavvyTraitRec MenuSavvySeparatorRecord = {
  0,				/* version */
  NULL,				/* disableCallback */
  NULL,				/* getAccelerator */
  NULL,				/* getMnemonic */
  NULL,				/* getActivateCBName */
};


static XmConst XmCareVisualTraitRec SeparatoGCVT = {
    0,		/* version */
    HandleRedraw,
};


/* Access Colors Trait record for separator */

static XmConst XmAccessColorsTraitRec sepACT = {
  0,			/* version */
  GetColors
};

  

/*  Resource list for Separator  */

static XtResource resources[] =
{
   {
     XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof (Boolean),
     XtOffsetOf( struct _XmGadgetRec, gadget.traversal_on),
     XmRImmediate, (XtPointer) False
   },
   {
     XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf( struct _XmGadgetRec, gadget.highlight_thickness),
     XmRImmediate, (XtPointer) 0
   },
   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf( struct _XmGadgetRec, gadget.shadow_thickness),
     XmRImmediate, (XtPointer) 2
   },
};


static XtResource cache_resources[] = 
{
   {
      XmNseparatorType,
      XmCSeparatorType,
      XmRSeparatorType,
      sizeof (unsigned char),
      XtOffsetOf( struct _XmSeparatorGCacheObjRec,
		 separator_cache.separator_type),
      XmRImmediate, (XtPointer) XmSHADOW_ETCHED_IN
   },

   {
      XmNmargin, 
      XmCMargin, 
      XmRHorizontalDimension, 
      sizeof (Dimension),
      XtOffsetOf( struct _XmSeparatorGCacheObjRec, separator_cache.margin),
      XmRImmediate, (XtPointer)  0
   },

   {
      XmNorientation,
      XmCOrientation,
      XmROrientation,
      sizeof (unsigned char),
      XtOffsetOf( struct _XmSeparatorGCacheObjRec,
		 separator_cache.orientation),
      XmRImmediate, (XtPointer) XmHORIZONTAL
   },
   
   
   {
     XmNbackground, XmCBackground, XmRPixel, 
     sizeof (Pixel), XtOffsetOf(struct _XmSeparatorGCacheObjRec,
				separator_cache.background),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },
   {
     XmNforeground, XmCForeground, XmRPixel, 
     sizeof (Pixel),
     XtOffsetOf(struct _XmSeparatorGCacheObjRec, separator_cache.foreground),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },
   
   {
     XmNtopShadowColor, XmCTopShadowColor, XmRPixel, 
     sizeof (Pixel),
     XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		separator_cache.top_shadow_color),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },
   {
     XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap),
     XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		separator_cache.top_shadow_pixmap),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },

   {
     XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel, 
     sizeof (Pixel),
     XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		separator_cache.bottom_shadow_color),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },

   {
     XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
     sizeof (Pixmap),
     XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		separator_cache.bottom_shadow_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },
};

static XmSyntheticResource cache_syn_resources[] = 
{
   {
      XmNmargin, 
      sizeof (Dimension),
      XtOffsetOf( struct _XmSeparatorGCacheObjRec, separator_cache.margin),
      XmeFromHorizontalPixels,
      XmeToHorizontalPixels,
   },
};

/* ext rec static initialization */
externaldef(xmseparatorgcacheobjclassrec)
XmSeparatorGCacheObjClassRec xmSeparatorGCacheObjClassRec =
{
  {
    /* superclass         */    (WidgetClass) &xmExtClassRec,
    /* class_name         */    "XmSeparatorGadget",
    /* widget_size        */    sizeof(XmSeparatorGCacheObjRec),
    /* class_initialize   */    NULL,
    /* chained class init */    NULL,
    /* class_inited       */    False,
    /* initialize         */    NULL,
    /* initialize hook    */    NULL,
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
    /* destroy            */    NULL,
    /* resize             */    NULL,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set values hook    */    NULL,
    /* set values almost  */    NULL,
    /* get values hook    */    NULL,
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

static XmBaseClassExtRec   separatorBaseClassExtRec = {
    NULL,    					/* next_extension        */
    NULLQUARK,					/* record_typ            */
    XmBaseClassExtVersion,			/* version               */
    sizeof(XmBaseClassExtRec),			/* record_size           */
    XmInheritInitializePrehook,			/* initializePrehook     */
    SetValuesPrehook,				/* setValuesPrehook      */
    InitializePosthook,				/* initializePosthook    */
    SetValuesPosthook,				/* setValuesPosthook     */
    (WidgetClass)&xmSeparatorGCacheObjClassRec,	/* secondaryObjectClass  */
    SecondaryObjectCreate,		        /* secondaryObjectCreate */
    GetSeparatorGClassSecResData,	        /* getSecResData */
    {0},			                /* Other Flags           */
    GetValuesPrehook,				/* getValuesPrehook      */
    GetValuesPosthook,				/* getValuesPosthook     */
};

static XmCacheClassPart SeparatorClassCachePart = {
    {NULL, 0, 0},        /* head of class cache list */
    _XmCacheCopy,       /* Copy routine     */
    _XmCacheDelete,     /* Delete routine   */
    _XmSeparatorCacheCompare,    /* Comparison routine   */
};


/*  The Separator class record definition  */

externaldef(xmseparatorgadgetclassrec) XmSeparatorGadgetClassRec xmSeparatorGadgetClassRec =

{
   {
      (WidgetClass) &xmGadgetClassRec,  /* superclass            */
      "XmSeparatorGadget",              /* class_name	         */
      sizeof(XmSeparatorGadgetRec),     /* widget_size	         */
      ClassInitialize,         		/* class_initialize      */
      ClassPartInitialize,              /* class_part_initialize */
      FALSE,                            /* class_inited          */
      Initialize,                       /* initialize	         */
      NULL,                             /* initialize_hook       */
      NULL,	                        /* realize	         */
      NULL,                             /* actions               */
      0,			        /* num_actions    	 */
      resources,                        /* resources	         */
      XtNumber(resources),		/* num_resources         */
      NULLQUARK,                        /* xrm_class	         */
      TRUE,                             /* compress_motion       */
      TRUE,                             /* compress_exposure     */
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      Destroy,                          /* destroy               */	
      NULL,                             /* resize                */
      Redisplay,                        /* expose                */	
      SetValues,                        /* set_values	         */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,				/* display_accelerator   */
      (XtPointer)&separatorBaseClassExtRec, /* extension         */
   },

   {
      NULL, 			/* border highlight   */
      NULL,      		/* border_unhighlight */
      NULL,			/* arm_and_activate   */
      InputDispatch,		/* input dispatch     */
      XmInheritVisualChange,	/* visual_change      */
      NULL,			/* syn_resources      */
      0,  			/* num_syn_resources  */
      &SeparatorClassCachePart, /* class cache part   */
      NULL,         		/* extension          */
   },

   {
      NULL,         		/* extension */
   }
};

externaldef(xmseparatorgadgetclass) WidgetClass xmSeparatorGadgetClass = 
   (WidgetClass) &xmSeparatorGadgetClassRec;


/*ARGSUSED*/
static void 
SetTopShadowPixmapDefault(
        Widget widget,
        int offset,		/* unused */
        XrmValue *value )
{
   XmSeparatorGadget sg = (XmSeparatorGadget) widget;
   XmManagerWidget mw = (XmManagerWidget)XtParent(sg);
   static Pixmap pixmap;

   pixmap = XmUNSPECIFIED_PIXMAP;

   value->addr = (char *) &pixmap;
   value->size = sizeof (Pixmap);

   if (SEPG_TopShadowColor(sg) == SEPG_Background(sg))
      pixmap = XmGetPixmapByDepth (XtScreen (sg), XmS50_foreground,
                            SEPG_TopShadowColor(sg),
                            SEPG_Foreground(sg),
                            mw->core.depth);

   else if (DefaultDepthOfScreen (XtScreen (widget)) == 1)
      pixmap = XmGetPixmapByDepth (XtScreen (sg), XmS50_foreground,
                            SEPG_TopShadowColor(sg),
                            SEPG_Background(sg),
                            mw->core.depth);
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
        Widget sg,
        XEvent *event,
        Mask event_mask )
{
   if (event_mask & XmHELP_EVENT) Help (sg, event);
}

/*******************************************************************
 *
 *  _XmSeparatorCacheCompare
 *
 *******************************************************************/
int 
_XmSeparatorCacheCompare(
        XtPointer A,
        XtPointer B )
{
        XmSeparatorGCacheObjPart *separator_inst =
                                               (XmSeparatorGCacheObjPart *) A ;
        XmSeparatorGCacheObjPart *separator_cache_inst =
                                               (XmSeparatorGCacheObjPart *) B ;
    if((separator_inst->margin == separator_cache_inst->margin) &&
       (separator_inst->orientation == separator_cache_inst->orientation) &&
       (separator_inst->separator_type == separator_cache_inst->separator_type) &&
       (separator_inst-> separator_GC == separator_cache_inst->separator_GC) &&
       (separator_inst-> background_GC == separator_cache_inst->background_GC) &&
       (separator_inst-> top_shadow_GC == separator_cache_inst->top_shadow_GC) &&
       (separator_inst-> bottom_shadow_GC == separator_cache_inst->bottom_shadow_GC) &&
       (separator_inst-> background == separator_cache_inst->background) &&
       (separator_inst-> top_shadow_color == separator_cache_inst->top_shadow_color) &&
       (separator_inst-> top_shadow_pixmap == separator_cache_inst->top_shadow_pixmap) &&
       (separator_inst-> bottom_shadow_color ==
	separator_cache_inst->bottom_shadow_color) &&
       (separator_inst-> bottom_shadow_pixmap ==
	                 separator_cache_inst->bottom_shadow_pixmap))
       return 1;
    else
       return 0;
 }

/***********************************************************
*
*  ClassInitialize
*
************************************************************/
static void 
ClassInitialize( void )
{
    separatorBaseClassExtRec.record_type = XmQmotif;

    /* Install the menu savvy trait. */
    XmeTraitSet((XtPointer) xmSeparatorGadgetClass, XmQTmenuSavvy,
		(XtPointer) &MenuSavvySeparatorRecord);
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
   _XmFastSubclassInit (wc, XmSEPARATOR_GADGET_BIT);
  
   /* Install the careParentVisual trait for all subclasses as well. */
   XmeTraitSet((XtPointer)wc, XmQTcareParentVisual, (XtPointer)&SeparatoGCVT);

   /* Install the accessColors trait for all subclasses as well. */
   XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&sepACT);
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
 *  Update pointers in instance records now so references to resources
 * in the cache record will be valid for use in CallProcs.
 */
 
  SEPG_Cache(new_w) = &(((XmSeparatorGCacheObject)newSec)->separator_cache);
  SEPG_Cache(req) = &(((XmSeparatorGCacheObject)reqSec)->separator_cache);

    /*
     * fetch the resources in superclass to subclass order
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

  ((XmSeparatorGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
  ((XmSeparatorGCacheObject)newSec)->ext.logicalParent = new_w;

  _XmPushWidgetExtData(new_w, extData,
                      ((XmSeparatorGCacheObject)newSec)->ext.extensionType);
   memcpy(reqSec, newSec, size);
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
    XmSeparatorGadget sw = (XmSeparatorGadget)new_w;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    _XmProcessLock();
    SEPG_Cache(sw) = (XmSeparatorGCacheObjPart *)
      _XmCachePart( SEPG_ClassCachePart(sw),
                    (XtPointer) SEPG_Cache(sw),
                    sizeof(XmSeparatorGCacheObjPart));

    /*
     * might want to break up into per-class work that gets explicitly
     * chained. For right now, each class has to replicate all
     * superclass logic in hook routine
     */

    /*
     * free the req subobject used for comparisons
     */
    _XmPopWidgetExtData((Widget) sw, &ext, XmCACHE_EXTENSION);
    _XmExtObjFree((XtPointer) ext->widget);
    _XmExtObjFree((XtPointer) ext->reqWidget);
    _XmProcessUnlock();
    XtFree( (char *) ext);

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
    XmSeparatorGCacheObject     newSec, reqSec;
    Cardinal                    size;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmSeparatorGCacheObject)_XmExtObjAlloc(size);
    reqSec = (XmSeparatorGCacheObject)_XmExtObjAlloc(size);
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

    memcpy( &(newSec->separator_cache),
            SEPG_Cache(newParent),
            sizeof(XmSeparatorGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    XtSetSubvalues((XtPointer)newSec,
                   ec->core_class.resources,
                   ec->core_class.num_resources,
                   args, *num_args);

    memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

    SEPG_Cache(newParent) = &(((XmSeparatorGCacheObject)newSec)->separator_cache);
    SEPG_Cache(refParent) =
	      &(((XmSeparatorGCacheObject)extData->reqWidget)->separator_cache);

    _XmExtImportArgs((Widget)newSec, args, num_args);

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
    XmSeparatorGCacheObject     newSec;
    Cardinal                    size;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmSeparatorGCacheObject)_XmExtObjAlloc(size);
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

    memcpy( &(newSec->separator_cache), 
            SEPG_Cache(newParent),
            sizeof(XmSeparatorGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

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
/* ARGSUSED */
static void 
GetValuesPosthook(
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData ext;

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
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             ext;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    /* assign if changed! */
    _XmProcessLock();
    if (!_XmSeparatorCacheCompare((XtPointer) SEPG_Cache(new_w),
				  (XtPointer) SEPG_Cache(current)))
    {
          _XmCacheDelete( (XtPointer) SEPG_Cache(current));  /* delete the old one */
          SEPG_Cache(new_w) = (XmSeparatorGCacheObjPart *)
            _XmCachePart(SEPG_ClassCachePart(new_w),
                         (XtPointer) SEPG_Cache(new_w),
                         sizeof(XmSeparatorGCacheObjPart));
     } else
           SEPG_Cache(new_w) = SEPG_Cache(current);

    _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

    _XmExtObjFree((XtPointer) ext->widget);
    _XmExtObjFree((XtPointer) ext->reqWidget);
    _XmProcessUnlock();

    XtFree( (char *) ext);

    return FALSE;
}


      
/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    XmSeparatorGadget request = (XmSeparatorGadget) rw ;
    XmSeparatorGadget new_w = (XmSeparatorGadget) nw ;
   new_w -> gadget.traversal_on = FALSE;

   /* Force highlightThickness to zero if in a menu. */
   if (XmIsRowColumn(XtParent(new_w)) &&
       ((RC_Type(XtParent(new_w)) == XmMENU_PULLDOWN) ||
        (RC_Type(XtParent(new_w)) == XmMENU_POPUP)))
     new_w->gadget.highlight_thickness = 0;

    if(    !XmRepTypeValidValue( XmRID_SEPARATOR_TYPE,
                                   SEPG_SeparatorType( new_w), (Widget) new_w)    )
   {
      SEPG_SeparatorType(new_w) = XmSHADOW_ETCHED_IN;
   }

   if(    !XmRepTypeValidValue( XmRID_ORIENTATION,
                                     SEPG_Orientation( new_w), (Widget) new_w)    )
   {
      SEPG_Orientation(new_w) = XmHORIZONTAL;
   }

   if (SEPG_Orientation(new_w) == XmHORIZONTAL)
   {
      if (request -> rectangle.width == 0)
	 new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness +2;

      if (request -> rectangle.height == 0)
      {
	 new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness;

	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.height += 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> rectangle.height += new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE)
	    new_w -> rectangle.height += 5;
	 else
	    if (new_w -> rectangle.height == 0)
	       new_w -> rectangle.height = 1;
      }
   }

   if (SEPG_Orientation(new_w) == XmVERTICAL)
   {
      if (request -> rectangle.height == 0)
	 new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness +2;

      if (request -> rectangle.width == 0)
      {
	 new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness;

	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.width += 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> rectangle.width += new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE)
	    new_w -> rectangle.width += 5;
	 else
	    if (new_w -> rectangle.width == 0)
	       new_w -> rectangle.width = 1;
      }
   }



   DealWithColors(new_w);
   DealWithPixmaps(new_w);
   /*  Get the drawing graphics contexts.  */

    GetSeparatorGC (new_w);
    GetBackgroundGC (new_w);
    SEPG_TopShadowGC(new_w) =
	_XmGetPixmapBasedGC (XtParent(nw), 
			     SEPG_TopShadowColor(new_w),
			     SEPG_Background(new_w),
			     SEPG_TopShadowPixmap(new_w));
    SEPG_BottomShadowGC(new_w) =
	_XmGetPixmapBasedGC (XtParent(nw), 
			     SEPG_BottomShadowColor(new_w),
			     SEPG_Background(new_w),
			     SEPG_BottomShadowPixmap(new_w));


   /* only want help input events */

   new_w->gadget.event_mask = XmHELP_EVENT;

}



 /*
  *
  * DealWithColors
  *
  * Deal with compatibility.  
  * 
  *
  */
static void
DealWithColors(
        XmSeparatorGadget sg)
    {
    
    XmManagerWidget mw = (XmManagerWidget) XtParent(sg);
    
    
    /*
      If the gadget color is set to the tag value or it is the
      same as the manager color; bc mode is enabled otherwise
      initialize like a widget.
    */
    if((SEPG_Background(sg) == INVALID_PIXEL ||
       SEPG_Background(sg) == mw->core.background_pixel) &&
       (SEPG_Foreground(sg) == INVALID_PIXEL ||
       SEPG_Foreground(sg) == mw->manager.foreground) &&
   (SEPG_TopShadowColor(sg) == INVALID_PIXEL ||
   SEPG_TopShadowColor(sg) == mw->manager.top_shadow_color) &&
(SEPG_BottomShadowColor(sg) == INVALID_PIXEL || 
SEPG_BottomShadowColor(sg) == mw->manager.bottom_shadow_color)
       )
	{
	SEPG_Background(sg) = mw->core.background_pixel;
	SEPG_Foreground(sg) = mw->manager.foreground;
	SEPG_TopShadowColor(sg) = mw->manager.top_shadow_color;
	SEPG_BottomShadowColor(sg) = mw->manager.bottom_shadow_color;
	}
    else
	{
	InitNewColorBehavior(sg);
	}
    }

/*
 * InitNewColorBehavior
 *
 * Initialize colors like a widget.  These are CallProcs so
 * they should be called with a correct offset. However offset
 * isn't used by these functions.  Even so I supply offset.
 * You make the call.
 *
 */
 
static void
InitNewColorBehavior(
        XmSeparatorGadget sg)
    {
    XrmValue value;
    
    value.size = sizeof(Pixel);
    
    if(SEPG_Background(sg) == INVALID_PIXEL)
	{
	_XmBackgroundColorDefault((Widget)sg,
        XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		   separator_cache.background),
				  &value);
	memcpy((char*) &SEPG_Background(sg), value.addr, value.size);
	}
    
    if(SEPG_Foreground(sg) == INVALID_PIXEL)
	{
	_XmForegroundColorDefault((Widget)sg,
        XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		   separator_cache.foreground),
				  &value);
	memcpy((char*) &SEPG_Foreground(sg), value.addr, value.size);
	}
	
    if(SEPG_TopShadowColor(sg) == INVALID_PIXEL)
	{
	_XmTopShadowColorDefault((Widget)sg,
         XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		    separator_cache.top_shadow_color),
				 &value);
	memcpy((char*) &SEPG_TopShadowColor(sg), value.addr, value.size);
	}
    
    if(SEPG_BottomShadowColor(sg) == INVALID_PIXEL)
	{
	_XmBottomShadowColorDefault((Widget)sg,
        XtOffsetOf(struct _XmSeparatorGCacheObjRec,
		   separator_cache.bottom_shadow_color),
				    &value);
	memcpy((char*) &SEPG_BottomShadowColor(sg), value.addr, value.size);
	}
    }

 /*
  *
  * DealWithPixmaps
  *
  * Deal with compatibility.  
  *  If any resource is set initialize like a widget otherwise get
  * everything from the parent.
  * 
  *
  */
static void
DealWithPixmaps(
        XmSeparatorGadget sg)
    {
    
    XmManagerWidget mw = (XmManagerWidget) XtParent(sg);
    
    
    if(SEPG_TopShadowPixmap(sg) == INVALID_PIXMAP ||
       SEPG_TopShadowPixmap(sg) == mw->manager.top_shadow_pixmap
       )
	{
	SEPG_TopShadowPixmap(sg) = mw->manager.top_shadow_pixmap;
	}	
    else
	{
	InitNewPixmapBehavior(sg);
	}
    }
/*
 * InitNewPixmapBehavior
 *
 * Initialize colors like a widget.  These are CallProcs so
 * they should be called with a correct offset. However offset
 * isn't used by these functions.  Even so I supply offset.
 * You make the call.
 *
 */
 
static void
InitNewPixmapBehavior(
        XmSeparatorGadget sg)
    {
    XrmValue value;
    
    value.size = sizeof(Pixmap);
    
	
    if(SEPG_TopShadowPixmap(sg) == INVALID_PIXMAP)
	{
	SetTopShadowPixmapDefault((Widget)sg,
				  0,
				  &value);
	
	SEPG_TopShadowColor(sg) = *value.addr;
	memcpy((char*) &SEPG_TopShadowPixmap(sg), value.addr, value.size);
	}
    }

		     


/************************************************************************
 *
 *  GetBackgroundGC
 *     Get the graphics context used for drawing the separator.
 *
 ************************************************************************/
static void 
GetBackgroundGC(
        XmSeparatorGadget sg )
{
   XGCValues values;
   XtGCMask  valueMask;
   XmManagerWidget mw;
   
   mw = (XmManagerWidget) XtParent(sg);

   sg->separator.fill_bg_box = 
     ((mw->core.background_pixel != SEPG_Background(sg)) &&
      (mw->core.background_pixmap == XmUNSPECIFIED_PIXMAP));

   if (sg->separator.fill_bg_box)
     {
       valueMask = GCForeground | GCBackground;

       values.foreground = SEPG_Background(sg);
       values.background = SEPG_Foreground(sg);

       SEPG_BackgroundGC(sg) = XtGetGC ((Widget) mw, valueMask, &values);
     }
   else
     {
       /* CR 7650: At least initialize the field. */
       SEPG_BackgroundGC(sg) = (GC) None;
     }
}


/************************************************************************
 *
 *  GetSeparatorGC
 *     Get the graphics context used for drawing the separator.
 *
 ************************************************************************/
static void 
GetSeparatorGC(
        XmSeparatorGadget sg )
{
   XGCValues values;
   XtGCMask  valueMask;
   XmManagerWidget mw;
   
   mw = (XmManagerWidget) XtParent(sg);

   valueMask = GCForeground | GCBackground;

   values.foreground = SEPG_Foreground(sg);
   values.background = SEPG_Background(sg);

   if (SEPG_SeparatorType(sg) == XmSINGLE_DASHED_LINE ||
       SEPG_SeparatorType(sg) == XmDOUBLE_DASHED_LINE)
   {
      valueMask = valueMask | GCLineStyle;
      values.line_style = LineDoubleDash;
   }

   SEPG_SeparatorGC(sg) = XtGetGC ((Widget) mw, valueMask, &values);
}




/************************************************************************
 *
 *  Redisplay
 *     Invoke the application exposure callbacks.
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
    {
    XmSeparatorGadget sg = (XmSeparatorGadget) wid ;
    int background_x_offset, background_y_offset, background_width,
    background_height;

     if (XmIsRowColumn(XtParent(sg))) {
       Widget rowcol = XtParent(sg);

       if ((RC_Type(rowcol) == XmMENU_PULLDOWN ||
            RC_Type(rowcol) == XmMENU_POPUP)    &&
           (! ((ShellWidget)XtParent(rowcol))->shell.popped_up)) {
           /* in a menu system that is not yet popped up, ignore */
           return;
       }
     }
   
    background_height = (int) sg->rectangle.height - 2 *
	sg->gadget.highlight_thickness;
						  
    background_width = (int) sg->rectangle.width - 2 *
	sg->gadget.highlight_thickness;

    background_x_offset = sg->rectangle.x + sg->gadget.highlight_thickness;
    
    background_y_offset = sg->rectangle.y + sg->gadget.highlight_thickness;
   

    if (sg->separator.fill_bg_box)  {
      XFillRectangle(XtDisplay(sg),
		     XtWindow((Widget) sg),
		     SEPG_BackgroundGC(sg),
		     background_x_offset,
		     background_y_offset,
		     background_width,
		     background_height);
    }

    XmeDrawSeparator(XtDisplay((Widget) sg), XtWindow((Widget) sg),
                  SEPG_TopShadowGC(sg),
                  SEPG_BottomShadowGC(sg),
                  SEPG_SeparatorGC(sg),
                  sg->rectangle.x + sg->gadget.highlight_thickness,
                  sg->rectangle.y + sg->gadget.highlight_thickness ,
                  sg->rectangle.width - 2*sg->gadget.highlight_thickness,
                  sg->rectangle.height - 2*sg->gadget.highlight_thickness,
                  sg->gadget.shadow_thickness,
                  SEPG_Margin(sg),
                  SEPG_Orientation(sg),
                  SEPG_SeparatorType(sg));
}




/************************************************************************
 *
 *  Destroy
 *	Remove the callback lists.
 *
 ************************************************************************/
static void 
Destroy(
        Widget sg )
{
   XmManagerWidget mw = (XmManagerWidget) XtParent(sg);

   XtReleaseGC( (Widget) mw, SEPG_SeparatorGC(sg));
   if (((XmSeparatorGadget) sg)->separator.fill_bg_box)
     XtReleaseGC( (Widget) mw, SEPG_BackgroundGC(sg));
   XtReleaseGC( (Widget) mw, SEPG_TopShadowGC(sg));
   XtReleaseGC( (Widget) mw, SEPG_BottomShadowGC(sg));

   _XmProcessLock();
   _XmCacheDelete( (XtPointer) SEPG_Cache(sg));
   _XmProcessUnlock();
}



static Boolean 
HandleRedraw (
	Widget kid, 	       
	Widget cur_parent,
	Widget new_parent,
	Mask visual_flag)
    {
    XmSeparatorGadget sg = (XmSeparatorGadget) kid ;
    XmManagerWidget mw = (XmManagerWidget) new_parent;
    XmManagerWidget curmw = (XmManagerWidget) cur_parent;
    Boolean redraw = False;
    XmSeparatorGCacheObjPart oldCopy;
    
	
    _XmProcessLock();
    _XmCacheCopy((XtPointer) SEPG_Cache(sg), (XtPointer) &oldCopy,
		 sizeof(XmSeparatorGCacheObjPart));
    _XmCacheDelete ((XtPointer) SEPG_Cache(sg));
    SEPG_Cache(sg) = &oldCopy;
    _XmProcessUnlock();

    if ((visual_flag & VisualBackgroundPixel) &&
	(SEPG_Background(sg) == curmw->core.background_pixel))
	{
	XtReleaseGC (XtParent(sg), SEPG_SeparatorGC(sg));
	if (sg->separator.fill_bg_box)
	  XtReleaseGC (XtParent(sg), SEPG_BackgroundGC(sg));

	SEPG_Background(sg) = mw->core.background_pixel;
	
	GetSeparatorGC((XmSeparatorGadget)sg);
	GetBackgroundGC((XmSeparatorGadget)sg);
      
	
	redraw = True;
	}

    if (visual_flag & VisualBackgroundPixmap)
	{
	if (sg->separator.fill_bg_box)
	  XtReleaseGC (XtParent(sg), SEPG_BackgroundGC(sg));

	GetBackgroundGC((XmSeparatorGadget)sg);
	
	redraw = True;
	}
    
    if ((visual_flag & VisualForeground) &&
	(SEPG_Foreground(sg) == curmw->manager.foreground))
	{
	XtReleaseGC (XtParent(sg), SEPG_SeparatorGC(sg));
	XtReleaseGC (XtParent(sg), SEPG_TopShadowGC(sg));

	SEPG_Foreground(sg) = mw->manager.foreground;
	
	GetSeparatorGC((XmSeparatorGadget)sg);
	SEPG_TopShadowGC(sg) =
	    _XmGetPixmapBasedGC (XtParent(sg), 
				 SEPG_TopShadowColor(sg),
				 SEPG_Background(sg),
				 SEPG_TopShadowPixmap(sg));
	redraw = True;
	}
    
    if (visual_flag & (VisualTopShadowColor | VisualTopShadowPixmap))
	{
	XtReleaseGC (XtParent(sg), SEPG_TopShadowGC(sg));

	if(SEPG_TopShadowColor(sg) == curmw->manager.top_shadow_color)
	    SEPG_TopShadowColor(sg) = mw->manager.top_shadow_color;
	
	if(SEPG_TopShadowPixmap(sg) == curmw->manager.top_shadow_pixmap &&
	   (SEPG_TopShadowPixmap(sg) != XmUNSPECIFIED_PIXMAP
	    || SEPG_TopShadowColor(sg) == curmw->manager.top_shadow_color))
	    SEPG_TopShadowPixmap(sg) = mw->manager.top_shadow_pixmap;
	    
	SEPG_TopShadowGC(sg) =
	    _XmGetPixmapBasedGC (XtParent(sg), 
				 SEPG_TopShadowColor(sg),
				 SEPG_Background(sg),
				 SEPG_TopShadowPixmap(sg));
	
	redraw = True;
	}

    if (visual_flag & (VisualBottomShadowColor | VisualBottomShadowPixmap))
	{
	XtReleaseGC (XtParent(sg), SEPG_BottomShadowGC(sg));

	if(SEPG_BottomShadowColor(sg) == curmw->manager.bottom_shadow_color)
	    SEPG_BottomShadowColor(sg) = mw->manager.bottom_shadow_color;
	
	if(SEPG_BottomShadowPixmap(sg) == curmw->manager.bottom_shadow_pixmap &&
	   (SEPG_BottomShadowPixmap(sg) != XmUNSPECIFIED_PIXMAP 
	   || SEPG_BottomShadowColor(sg) == curmw->manager.bottom_shadow_color))
	    SEPG_BottomShadowPixmap(sg) = mw->manager.bottom_shadow_pixmap;
	    
	SEPG_BottomShadowGC(sg) =
	    _XmGetPixmapBasedGC (XtParent(sg), 
				 SEPG_BottomShadowColor(sg),
				 SEPG_Background(sg),
				 SEPG_BottomShadowPixmap(sg));
	
	redraw = True;
	}
    
    _XmProcessLock();
    SEPG_Cache(sg) = (XmSeparatorGCacheObjPart *)
	_XmCachePart(SEPG_ClassCachePart(sg), (XtPointer) SEPG_Cache(sg),
		     sizeof(XmSeparatorGCacheObjPart));
    _XmProcessUnlock();
		     
    return redraw ;
    }


/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean 
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
        XmSeparatorGadget current = (XmSeparatorGadget) cw ;
        XmSeparatorGadget request = (XmSeparatorGadget) rw ;
        XmSeparatorGadget new_w = (XmSeparatorGadget) nw ;
   Boolean flag = FALSE;   
   XmManagerWidget new_mw = (XmManagerWidget) XtParent(new_w);

   /*
    * We never allow our traversal flags to be changed during SetValues();
    * this is enforced by our superclass.
    */
   /*  Force traversal_on to FALSE */
   new_w -> gadget.traversal_on = FALSE;
 
   /* Force highlightThickness to zero if in a menu. */
   if (XmIsRowColumn(XtParent(new_w)) &&
       ((RC_Type(XtParent(new_w)) == XmMENU_PULLDOWN) ||
        (RC_Type(XtParent(new_w)) == XmMENU_POPUP)))
     new_w->gadget.highlight_thickness = 0;

   if(    !XmRepTypeValidValue( XmRID_SEPARATOR_TYPE,
                                   SEPG_SeparatorType( new_w), (Widget) new_w)    )
   {
      SEPG_SeparatorType(new_w) = SEPG_SeparatorType(current);
   }

   if(    !XmRepTypeValidValue( XmRID_ORIENTATION,
                                     SEPG_Orientation( new_w), (Widget) new_w)    )
   {
      SEPG_Orientation(new_w) = SEPG_Orientation(current);
   }

   if (SEPG_Orientation(new_w) == XmHORIZONTAL)
   {
      if (request -> rectangle.width == 0)
	 new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness + 2;

      if (request -> rectangle.height == 0)
      {
	 new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness;

	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.height += 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH) 
	    new_w -> rectangle.height += new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE)
	    new_w -> rectangle.height += 5;
	 else
	    if (new_w -> rectangle.height == 0)
	       new_w -> rectangle.height = 1;
      }

      if ((SEPG_SeparatorType(new_w) != SEPG_SeparatorType(current) ||
           new_w->gadget.shadow_thickness != current->gadget.shadow_thickness ||
           new_w->gadget.highlight_thickness != current->gadget.highlight_thickness) && 
	   request -> rectangle.height == current -> rectangle.height)
      {
	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness + 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT || 
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH) 
	    new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness +
				       new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE) 
	    new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness + 5;
         } 
   }

   if (SEPG_Orientation(new_w) == XmVERTICAL)
   {
      if (request -> rectangle.height == 0)
	 new_w -> rectangle.height = 2 * new_w -> gadget.highlight_thickness + 2;

      if (request -> rectangle.width == 0)
      {
	 new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness;

	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.width += 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH) 
	    new_w -> rectangle.width += new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE)
	    new_w -> rectangle.width += 5;
	 else
	    if (new_w -> rectangle.width == 0)
	       new_w -> rectangle.width = 1;
      }

      if ((SEPG_SeparatorType(new_w) != SEPG_SeparatorType(current) ||
           new_w->gadget.shadow_thickness != current->gadget.shadow_thickness ||
           new_w->gadget.highlight_thickness != current->gadget.highlight_thickness) &&
	   request -> rectangle.width == current -> rectangle.width)
      {
	 if (SEPG_SeparatorType(new_w) == XmSINGLE_LINE ||
	     SEPG_SeparatorType(new_w) == XmSINGLE_DASHED_LINE)
	    new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness + 3;
	 else if (SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT || 
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_IN_DASH ||
		  SEPG_SeparatorType(new_w) == XmSHADOW_ETCHED_OUT_DASH) 
	    new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness +
				       new_w -> gadget.shadow_thickness;
	 else if (SEPG_SeparatorType(new_w) == XmDOUBLE_LINE ||
		  SEPG_SeparatorType(new_w) == XmDOUBLE_DASHED_LINE) 
	    new_w -> rectangle.width = 2 * new_w -> gadget.highlight_thickness + 5;
         } 
   }
  
   if (SEPG_Orientation(new_w) != SEPG_Orientation(current) ||
       SEPG_Margin(new_w) != SEPG_Margin(current) ||
       new_w -> gadget.shadow_thickness != current -> gadget.shadow_thickness)
      flag = TRUE;


   if (SEPG_SeparatorType(new_w) != SEPG_SeparatorType(current) ||
       SEPG_Background(new_w)!= SEPG_Background(current) ||
       SEPG_Foreground(new_w)!= SEPG_Foreground(current))
   {
      XtReleaseGC( (Widget) new_mw, SEPG_SeparatorGC(new_w));
      GetSeparatorGC (new_w);
      flag = TRUE;
   }
  
   if (SEPG_Background(new_w)!= SEPG_Background(current))
   {
     if (new_w->separator.fill_bg_box)
       XtReleaseGC( (Widget) new_mw, SEPG_BackgroundGC(new_w));
      GetBackgroundGC (new_w);
      flag = TRUE;
   }
   
   if (SEPG_TopShadowColor(new_w)!= SEPG_TopShadowColor(current) ||
      (SEPG_TopShadowPixmap(new_w)!= SEPG_TopShadowPixmap(current)))
   {
      XtReleaseGC( (Widget) new_mw, SEPG_TopShadowGC(new_w));
      SEPG_TopShadowGC(new_w) =
	  _XmGetPixmapBasedGC ((Widget) new_mw, 
			       SEPG_TopShadowColor(new_w),
			       SEPG_Background(new_w),
			       SEPG_TopShadowPixmap(new_w));
      flag = TRUE;
   }

   if (SEPG_BottomShadowColor(new_w)!= SEPG_BottomShadowColor(current) ||
      (SEPG_BottomShadowPixmap(new_w)!= SEPG_BottomShadowPixmap(current)))
   {
      XtReleaseGC( (Widget) new_mw, SEPG_BottomShadowGC(new_w));
      SEPG_BottomShadowGC(new_w) =
	  _XmGetPixmapBasedGC ((Widget) new_mw, 
			       SEPG_BottomShadowColor(new_w),
			       SEPG_Background(new_w),
			       SEPG_BottomShadowPixmap(new_w));
      flag = TRUE;
   }

       
   /* SPB Why is this here? */
   /* Initialize the interesting input types */
   new_w->gadget.event_mask = XmHELP_EVENT;

   return (flag);
}


/************************************************************************
 *
 *  Help
 *
 ************************************************************************/
static void 
Help(
        Widget sg,
        XEvent *event )
{
   XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(sg);

   if (XmIsRowColumn(parent))
   {
      if (RC_Type(parent) == XmMENU_POPUP ||
	  RC_Type(parent) == XmMENU_PULLDOWN)
      {
	 (* ((XmRowColumnWidgetClass) parent->core.widget_class)->
	  row_column_class.menuProcedures)
	     (XmMENU_POPDOWN, XtParent(sg), NULL, event, NULL);
      }
   }

   _XmSocorro( (Widget) sg, event, NULL, NULL);
}


/************************************************************************
 *
 *  XmCreateSeparatorGadget
 *	Create an instance of a separator and return the widget id.
 *
 ************************************************************************/
Widget 
XmCreateSeparatorGadget(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, xmSeparatorGadgetClass, 
                           parent, arglist, argcount));
}
Widget 
XmVaCreateSeparatorGadget(
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
                         xmSeparatorGadgetClass, 
                         parent, False, 
                         var, count);
    va_end(var);   
    return w;
    
}
Widget 
XmVaCreateManagedSeparatorGadget(
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
                         xmSeparatorGadgetClass, 
                         parent, True, 
                         var, count);
    va_end(var);   
    return w;
    
}

/****************************************************
 *   Functions for manipulating Secondary Resources.
 *********************************************************/
/*
 * GetSeparatorGClassSecResData()
 *    Create a XmSecondaryResourceDataRec for each secondary resource;
 *    Put the pointers to these records in an array of pointers;
 *    Return the pointer to the array of pointers.
 *	client_data = Address of the structure in the class record which
 *	  represents the (template of ) the secondary data.
 */
/*ARGSUSED*/
static Cardinal 
GetSeparatorGClassSecResData(
        WidgetClass w_class,	/* unused */
        XmSecondaryResourceData **data_rtn )
{   int arrayCount;
    XmBaseClassExt  bcePtr;
    String  resource_class, resource_name;
    XtPointer  client_data;

    _XmProcessLock();
    bcePtr = &(separatorBaseClassExtRec );
    client_data = NULL;
    resource_class = NULL;
    resource_name = NULL;
    arrayCount =
      _XmSecondaryResourceData ( bcePtr, data_rtn, client_data,
                resource_name, resource_class,
                GetSeparatorGClassSecResBase); 
    _XmProcessUnlock();
    return (arrayCount);

}

/*
 * GetSeparatorGClassResBase ()
 *   retrun the address of the base of resources.
 *  If client data is the same as the address of the secndary data in the
 *	class record then send the base address of the cache-resources for this
 *	instance of the widget. 
 * Right now we  do not try to get the address of the cached_data from
 *  the Gadget component of this instance - since Gadget class does not
 *	have any cached_resources defined. If later secondary resources are
 *	defined for Gadget class then this routine will have to change.
 */
/*ARGSUSED*/
static XtPointer 
GetSeparatorGClassSecResBase(
        Widget widget,
        XtPointer client_data )	/* unused */
{	XtPointer  widgetSecdataPtr; 
  
	_XmProcessLock();
	widgetSecdataPtr = (XtPointer) (SEPG_Cache(widget));
	_XmProcessUnlock();


    return (widgetSecdataPtr);
}


static void
GetColors(Widget w, 
	  XmAccessColorData color_data)
{
    color_data->valueMask = AccessForeground | AccessBackgroundPixel |
	AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
    color_data->background = SEPG_Background(w);
    color_data->foreground = SEPG_Foreground(w);
    color_data->highlight_color = SEPG_Foreground(w);
    color_data->top_shadow_color = SEPG_TopShadowColor(w);
    color_data->bottom_shadow_color = SEPG_BottomShadowColor(w);
}

