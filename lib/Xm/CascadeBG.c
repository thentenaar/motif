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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: CascadeBG.c /main/28 1999/02/01 18:47:11 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "XmI.h"
#include <X11/ShellP.h>
#include <X11/keysymdef.h>
#include <Xm/CacheP.h>
#include <Xm/CascadeBP.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuStateP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "CascadeBI.h"
#include "CascadeBGI.h"
#include "ExtObjectI.h"
#include "GadgetUtiI.h"
#include "LabelI.h"
#include "LabelGI.h"
#include "MessagesI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "RCMenuI.h"
#include "ScreenI.h"
#include "SyntheticI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"

#define CASCADE_PIX_SPACE     4	/* pixels between label and bit map */
#define MAP_DELAY_DEFAULT   180
#define EVENTS              ((unsigned int) (ButtonPressMask | \
			       ButtonReleaseMask | EnterWindowMask | \
			       LeaveWindowMask))

#define WRONGPARENT	_XmMMsgCascadeB_0000
#define WRONGSUBMENU	_XmMMsgCascadeB_0001
#define WRONGMAPDELAY	_XmMMsgCascadeB_0002

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
static int _XmCascadeBCacheCompare(
                        XtPointer A,
                        XtPointer B) ;
static void BorderHighlight(
                        Widget wid) ;
static void BorderUnhighlight(
                        Widget wid) ;
static void DrawShadow(
                        XmCascadeButtonGadget cb) ;
static void DrawCascade(
                        XmCascadeButtonGadget cb) ;
static void position_cascade(
                        XmCascadeButtonGadget cascadebtn) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void InputDispatch(
                        Widget wid,
                        XEvent *event,
                        Mask event_mask) ;
static void Arm(
                        XmCascadeButtonGadget cb) ;
static void ArmAndPost(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Disarm(
                        XmCascadeButtonGadget cb,
                        Boolean unpost) ;
static void PostTimeout(
                        XtPointer closure,
                        XtIntervalId *id) ;
static void DelayedArm(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void CheckDisarm(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void StartDrag(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void Select(
                        XmCascadeButtonGadget cb,
                        XEvent *event,
                        Boolean doCascade) ;
static void DoSelect(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void KeySelect(
                        XmCascadeButtonGadget cb,
                        XEvent *event) ;
static void MenuBarSelect(
                        Widget wid,
                        XEvent *event) ;
static void MenuBarEnter(
                        Widget wid,
                        XEvent *event) ;
static void MenuBarLeave(
                        Widget wid) ;
static void size_cascade(
                        XmCascadeButtonGadget cascadebtn) ;
static void setup_cascade(
                        XmCascadeButtonGadget cascadebtn,
                        Boolean adjustWidth,
                        Boolean adjustHeight) ;
static void Destroy(
                        Widget wid) ;
static void Resize(
                        Widget wid) ;
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
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetArmGC(
                        XmCascadeButtonGadget cb) ;
static void GetBackgroundGC(
                        XmCascadeButtonGadget cb) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static Cardinal GetCascadeBGClassSecResData(
                        WidgetClass w_class,
                        XmSecondaryResourceData **data_rtn) ;
static XtPointer GetCascadeBGClassSecResBase(
                        Widget widget,
                        XtPointer client_data) ;

/********    End Static Function Declarations    ********/

static XtResource resources[] =
{
    {   XmNactivateCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmCascadeButtonGadgetRec, cascade_button.activate_callback),
        XmRCallback,
        NULL
    },

    {   XmNcascadingCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmCascadeButtonGadgetRec,
		   cascade_button.cascade_callback),
        XmRCallback,
        NULL
    },

    {	XmNsubMenuId,
	XmCMenuWidget,				/* submenu */
	XmRMenuWidget,
	sizeof (Widget),
	XtOffsetOf( struct _XmCascadeButtonGadgetRec, cascade_button.submenu),
	XmRMenuWidget,
	(XtPointer) 0
    },
    {
        XmNshadowThickness,
        XmCShadowThickness,
        XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmCascadeButtonGadgetRec, gadget.shadow_thickness),
        XmRCallProc,
	(XtPointer) _XmSetThickness
    },

    {
        XmNtraversalOn,
        XmCTraversalOn,
        XmRBoolean,
        sizeof (Boolean),
        XtOffsetOf( struct _XmGadgetRec, gadget.traversal_on),
        XmRImmediate,
        (XtPointer) True
    },

    {
        XmNhighlightThickness,
        XmCHighlightThickness,
        XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmGadgetRec, gadget.highlight_thickness),
        XmRCallProc,
	(XtPointer) _XmSetThickness
    },
};


static XtResource cache_resources[] =
{

   {   XmNcascadePixmap,
       XmCPixmap,
       XmRDynamicPixmap,
       sizeof(Pixmap),
       XtOffsetOf( struct _XmCascadeButtonGCacheObjRec,
                 cascade_button_cache.cascade_pixmap),
       XmRImmediate,
       (XtPointer) XmUNSPECIFIED_PIXMAP
   },

   {   XmNmappingDelay,
       XmCMappingDelay,
       XmRInt,
       sizeof (int),
       XtOffsetOf( struct _XmCascadeButtonGCacheObjRec,
	         cascade_button_cache.map_delay),
       XmRImmediate,
       (XtPointer) MAP_DELAY_DEFAULT
   },
};


static XmCacheClassPart CascadeButtonClassCachePart = {
    {NULL, 0, 0},        /* head of class cache list */
    _XmCacheCopy,       /* Copy routine     */
    _XmCacheDelete,     /* Delete routine   */
    _XmCascadeBCacheCompare,    /* Comparison routine   */
};

static XmBaseClassExtRec   CascadeBGClassExtensionRec = {
    NULL,    					    /* next_extension        */
    NULLQUARK,    				    /* record_typ  	     */
    XmBaseClassExtVersion,      		    /* version  	     */
    sizeof(XmBaseClassExtRec),  		    /* record_size  	     */
    InitializePrehook,				    /* initializePrehook     */
    SetValuesPrehook,   			    /* setValuesPrehook	     */
    InitializePosthook,   			    /* initializePosthook    */
    SetValuesPosthook,   			    /* setValuesPosthook     */
    (WidgetClass)&xmCascadeButtonGCacheObjClassRec, /* secondaryObjectClass  */
    SecondaryObjectCreate,                 	    /* secondaryObjectCreate */
    GetCascadeBGClassSecResData,                    /* getSecResData         */
    {0},           				    /* fast subclass         */
    GetValuesPrehook,				    /* getValuesPrehook      */
    GetValuesPosthook,				    /* getValuesPosthook     */
    NULL,                                     /* classPartInitPrehook */
    NULL,                                     /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};

/* ext rec static initialization */
externaldef(xmcascadebuttongcacheobjclassrec)
XmCascadeButtonGCacheObjClassRec xmCascadeButtonGCacheObjClassRec =
{
  {
      /* superclass         */    (WidgetClass) &xmLabelGCacheObjClassRec,
      /* class_name         */    "XmCascadeButtonGadget",
      /* widget_size        */    sizeof(XmCascadeButtonGCacheObjRec),
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
      /* synthetic resources */   NULL,
      /* num_syn_resources   */   0,
      /* extension           */   NULL,
   },
};


/*
 * static initialization of the cascade button widget class record,
 * must do each field
 */

static XmGadgetClassExtRec _XmCascadeBGadClassExtRec = {
     NULL,
     NULLQUARK,
     XmGadgetClassExtVersion,
     sizeof(XmGadgetClassExtRec),
     XmInheritBaselineProc,                  /* widget_baseline */
     XmInheritDisplayRectProc,               /* widget_display_rect */
     XmInheritMarginsProc,                   /* widget_margins */
};

externaldef(xmcascadebuttongadgetclassrec) XmCascadeButtonGadgetClassRec
	xmCascadeButtonGadgetClassRec =
{
    {
	(WidgetClass) &xmLabelGadgetClassRec,	/* superclass ptr	*/
	"XmCascadeButtonGadget",		/* class_name	*/
	sizeof (XmCascadeButtonGadgetRec),	/* size of Pulldown widget */
	ClassInitialize,			/* class init proc */
	ClassPartInitialize,			/* chained class init */
	FALSE,					/* class is not init'ed */
	Initialize,				/* widget init proc */
	NULL,					/* init_hook proc */
    	NULL,					/* widget realize proc */
    	NULL,					/* class action table */
	0,
	resources,			       /* this class's resource list */
	XtNumber (resources),			/*  "	  " resource_count */
    	NULLQUARK,				/* xrm_class	        */
    	TRUE,					/* do compress motion */
    	XtExposeCompressMaximal,		/* do compress exposure */
	TRUE,				       /* don't compress enter-leave */
   	FALSE,					/* no VisibilityNotify */
	Destroy,				/* class destroy proc */
	Resize,					/* class resize proc */
	Redisplay,				/* expose proc */
	SetValues,				/* set_value proc */
	NULL,					/* set_value_hook proc */
	XtInheritSetValuesAlmost,		/* set_value_almost proc */
	NULL,					/* get_values_hook */
	NULL,					/* class accept focus proc */
	XtVersion,				/* current version */
    	NULL,					/* callback offset list */
    	NULL,					/* default translation table */
	XtInheritQueryGeometry,			/* query geo proc */
	NULL,				        /* display accelerator*/
	(XtPointer)&CascadeBGClassExtensionRec,	/* extension */
    },
    {
			/* Gadget Class record */
	BorderHighlight,			/* border_highlight */
	BorderUnhighlight,			/* border_uhighlight */
	ArmAndActivate,				/* arm_and_activate */
	InputDispatch,				/* input_dispatch routine */
	XmInheritVisualChange,			/* visual_change routine */
	NULL,					/* syn_resources */
	0,					/* num_syn_resources */
	&CascadeButtonClassCachePart,		/* class cache_part   */
	(XtPointer)&_XmCascadeBGadClassExtRec,	/* extension */
    },
    {			/* Label Class record */
	XmInheritWidgetProc,	        	/* set override callback */
	XmInheritMenuProc,      		/* menu procedures       */
	NULL,					/* extension */
    },
    {			/* cascade_button class record */
        NULL,  				        /* extension          */
    }
};

/*
 * now make a public symbol that points to this class record
 */

externaldef(xmcascadebuttongadgetclass) WidgetClass
	xmCascadeButtonGadgetClass =
                          (WidgetClass) &xmCascadeButtonGadgetClassRec;

/*
 * use caching to share arrow data
 */
static XmCacheClassPart ArrowPixmapCache = {
   {NULL, 0, 0},                /* head of class cache list */
   _XmCacheCopy,                /* Copy routine     */
   _XmArrowPixmapCacheDelete,   /* Delete routine   */
   _XmArrowPixmapCacheCompare,  /* Comparison routine   */
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

static void
ClassInitialize( void )
{
  Cardinal                    wc_num_res, sc_num_res;
  XtResource                  *merged_list;
  int                         i, j;
  XtResourceList              uncompiled;
  Cardinal                    num;

/**************************************************************************
   Label's and Cascadebutton's resource lists are being merged into one
   and assigned to xmCascadeButtonGCacheObjClassRec. This is for performance
   reasons, since, instead of two calls to XtGetSubResources() XtGetSubvaluse()
   and XtSetSubvalues() for both the superclass and the widget class, now
   we have just one call with a merged resource list.
   NOTE: At this point the resource lists for Label and Cascadebutton do
         have unique entries, but if there are resources in the superclass
         that are being overwritten by the subclass then the merged_lists
         need to be created differently.
****************************************************************************/

  wc_num_res = xmCascadeButtonGCacheObjClassRec.object_class.num_resources;

  sc_num_res = xmLabelGCacheObjClassRec.object_class.num_resources;

  merged_list = (XtResource *)XtMalloc((sizeof(XtResource) * (wc_num_res +
                                                                 sc_num_res)));

  _XmTransformSubResources(xmLabelGCacheObjClassRec.object_class.resources,
                           sc_num_res, &uncompiled, &num);

  for (i = 0; i < num; i++)
  {

  merged_list[i] = uncompiled[i];

  }
  XtFree((char *)uncompiled);

  for (i = 0, j = num; i < wc_num_res; i++, j++)
  {
   merged_list[j] =
        xmCascadeButtonGCacheObjClassRec.object_class.resources[i];
  }

  _XmProcessLock();
  xmCascadeButtonGCacheObjClassRec.object_class.resources = merged_list;
  xmCascadeButtonGCacheObjClassRec.object_class.num_resources =
                wc_num_res + sc_num_res ;
  _XmProcessUnlock();

  CascadeBGClassExtensionRec.record_type = XmQmotif;
}

/*
 * set up fast subclassing
 */
static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmCASCADE_BUTTON_GADGET_BIT);

  _XmLabelGCloneMenuSavvy (wc, &MenuSavvyRecord);
}

/************************************************************************
*
*  SecondaryObjectCreate
*
************************************************************************/
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
     * Since the resource lists for label and cascadebutton were merged at
     * ClassInitialize time we need to make only one call to
     * XtGetSubresources()
     */

/*
 *  Update pointers in instance records now so references to resources
 * in the cache record will be valid for use in CallProcs.
 * CallProcs are invoked by XtGetSubresources().
 */

  LabG_Cache(new_w) = &(((XmLabelGCacheObject)newSec)->label_cache);
    LabG_Cache(req) = &(((XmLabelGCacheObject)reqSec)->label_cache);
   CBG_Cache(new_w) = &(((XmCascadeButtonGCacheObject)newSec)->
			cascade_button_cache);
     CBG_Cache(req) = &(((XmCascadeButtonGCacheObject)reqSec)->
			cascade_button_cache);

  XtGetSubresources(new_w,
                    newSec,
                    NULL, NULL,
                    wc->core_class.resources,
                    wc->core_class.num_resources,
                    args, *num_args );

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;

  ((XmCascadeButtonGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
  ((XmCascadeButtonGCacheObject)newSec)->ext.logicalParent = new_w;

  _XmPushWidgetExtData(new_w, extData,
		       ((XmCascadeButtonGCacheObject)newSec)->
		                                   ext.extensionType);
  memcpy(reqSec, newSec, size);
}

/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/
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
static void
InitializePosthook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData     ext;
    XmCascadeButtonGadget  cbw = (XmCascadeButtonGadget)new_w;

    /*
    * - register parts in cache.
    * - update cache pointers
    * - and free req
    */

    _XmProcessLock();

    LabG_Cache(cbw) = (XmLabelGCacheObjPart *)
      _XmCachePart( LabG_ClassCachePart(cbw),
                    (XtPointer) LabG_Cache(cbw),
                    sizeof(XmLabelGCacheObjPart));

    CBG_Cache(cbw) = (XmCascadeButtonGCacheObjPart *)
           _XmCachePart( CBG_ClassCachePart(cbw),
                         (XtPointer) CBG_Cache(cbw),
                         sizeof(XmCascadeButtonGCacheObjPart));

   /*
    * might want to break up into per-class work that gets explicitly
    * chained. For right now, each class has to replicate all
    * superclass logic in hook routine
    */

   /*
    * free the req subobject used for comparisons
    */
    _XmPopWidgetExtData((Widget) cbw, &ext, XmCACHE_EXTENSION);
    _XmExtObjFree((XtPointer) ext->widget);
    _XmExtObjFree((XtPointer) ext->reqWidget);
    XtFree( (char *) ext);
    _XmProcessUnlock();

}

/*******************************************************************
 *
 *  _XmCascadeBCacheCompare
 *
 *******************************************************************/
static int
_XmCascadeBCacheCompare(
        XtPointer A,
        XtPointer B )
    {
    XmCascadeButtonGCacheObjPart *cascadeB_inst =
	(XmCascadeButtonGCacheObjPart *) A ;
    XmCascadeButtonGCacheObjPart *cascadeB_cache_inst =
	(XmCascadeButtonGCacheObjPart *) B ;
   if((cascadeB_inst->cascade_pixmap == cascadeB_cache_inst->cascade_pixmap) &&
      (cascadeB_inst->map_delay == cascadeB_cache_inst->map_delay) &&
      (cascadeB_inst->armed_pixmap == cascadeB_cache_inst->armed_pixmap) &&
      (cascadeB_inst->arm_gc == cascadeB_cache_inst->arm_gc) &&
      (cascadeB_inst->background_gc == cascadeB_cache_inst->background_gc))
        return 1;
   else
        return 0;

}

/*******************************************************************
 * _XmArrowPixmapCacheCompare()
 *******************************************************************/
int
_XmArrowPixmapCacheCompare(
        XtPointer A,
        XtPointer B )
{
   XmArrowPixmap *arrowpix_rec = (XmArrowPixmap *) A ;
   XmArrowPixmap *arrowpix_cache_rec = (XmArrowPixmap *) B ;

   if ((arrowpix_rec->height == arrowpix_cache_rec->height) &&
       (arrowpix_rec->width == arrowpix_cache_rec->width) &&
       (arrowpix_rec->screen == arrowpix_cache_rec->screen) &&
       (arrowpix_rec->depth == arrowpix_cache_rec->depth) &&
       (arrowpix_rec->direction == arrowpix_cache_rec->direction) &&
    (arrowpix_rec->top_shadow_color ==
          arrowpix_cache_rec->top_shadow_color) &&
       (arrowpix_rec->bottom_shadow_color ==
          arrowpix_cache_rec->bottom_shadow_color) &&
       (arrowpix_rec->foreground_color ==
          arrowpix_cache_rec->foreground_color))
      return 1;
   else
      return 0;
}

/*******************************************************************
 * _XmArrowPixmapCacheDelete()
 *******************************************************************/
void
_XmArrowPixmapCacheDelete(
        XtPointer data )
{
        Pixmap pixmap = (Pixmap) data ;
   XmGadgetCachePtr ptr;
   XmArrowPixmap *arrowpix_rec;

   ptr =  (XmGadgetCachePtr)(ClassCacheHead(&ArrowPixmapCache)).next;
   while (ptr)
   {
     arrowpix_rec = (XmArrowPixmap *)(CacheDataPtr(ptr));
     if (pixmap == arrowpix_rec->pixmap)
     {
        if (--ptr->ref_count <= 0)
        {
          (ptr->prev)->next = ptr->next;
          if (ptr->next)                    /* not the last record */
            (ptr->next)->prev = ptr->prev;
          XmDestroyPixmap(arrowpix_rec->screen, arrowpix_rec->pixmap);
          XtFree( (char *) ptr);
        }
        return;
     }
     else
        ptr = (XmGadgetCachePtr)ptr->next;
   }
}

/*
 * Border highlighting is only allowed for option menus.  Otherwise
 * the button is armed (does not pop up submenus).
 */
static void
BorderHighlight(
        Widget wid )
{
        XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
   if (LabG_MenuType(cb) == XmMENU_OPTION)
      (* ((XmGadgetClass) xmGadgetClass)->
             gadget_class.border_highlight) ((Widget) cb);

   else
      Arm (cb);
}



/*
 * Border unhighlighting only done in option menus.  Otherwise the button
 * is disarmed (does not pop down submenus).
 */
static void
BorderUnhighlight(
        Widget wid )
{
        XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
   if (LabG_MenuType(cb) == XmMENU_OPTION)
      (* ((XmGadgetClass) xmGadgetClass)->
             gadget_class.border_unhighlight) ((Widget) cb);

   else
      Disarm (cb, False);
}


/*
 * Draw the 3D shadow around the widget if its in an option menu or if the
 * widget is armed.
 */
static void
DrawShadow(
        XmCascadeButtonGadget cb )
{
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) cb));
  Boolean etched_in;

  etched_in = dpy -> display.enable_etched_in_menu
    && (! (LabG_MenuType(cb) == XmMENU_OPTION)) ;

  if (CBG_IsArmed(cb) ||
      (LabG_MenuType(cb) == XmMENU_OPTION))
    {
      if (XtIsRealized(XtParent(cb)))
      {
	 XmeDrawShadows (XtDisplay (cb), XtWindow (XtParent(cb)),
			LabG_TopShadowGC(cb),
			LabG_BottomShadowGC(cb),
			cb->gadget.highlight_thickness + cb->rectangle.x,
			cb->gadget.highlight_thickness + cb->rectangle.y,
			 cb->rectangle.width - 2 *
			cb->gadget.highlight_thickness,
			 cb->rectangle.height - 2 *
			cb->gadget.highlight_thickness,
			cb->gadget.shadow_thickness,
			etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
      }
   }
}

static void
DrawCascade(
        XmCascadeButtonGadget cb )
{
   if ((CBG_HasCascade(cb)) && (CBG_Cascade_width(cb) != 0))
   {
      /* draw the casacade */
      if ((LabG_MenuType(cb) == XmMENU_OPTION) &&
	  (CBG_CascadePixmap(cb) == XmUNSPECIFIED_PIXMAP))
      {
	 Dimension height, width;
	 Dimension offset_y;
         Dimension offset_x;

	 switch(CBG_Cascade_height(cb) - 2*G_ShadowThickness(cb)) {
	    case 5:
	    case 6:
	       height = 1;
	       width =  CBG_Cascade_width(cb) - 3;
	       break;
	    case 7:
	    case 8:
	    case 9:
	       height = 2;
	       width =  CBG_Cascade_width(cb) - 4;
	       break;
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	       height = 3;
	       width =  CBG_Cascade_width(cb) - 5;
	       break;
	    default:
	       height = 4;
	       width =  CBG_Cascade_width(cb) - 6;
	       break;
	 }
	 width -= 2*G_ShadowThickness(cb);
	 offset_y =  (CBG_Cascade_height(cb)- height)/2;

	 if (LayoutIsRtoLG(cb))
	   {
	     offset_x = (CBG_Cascade_width(cb) - width - G_ShadowThickness(cb));
	     XFillRectangle(XtDisplay(cb), XtWindow(XtParent(cb)),
			    LabG_BackgroundGC(cb),
			    cb->rectangle.x + CBG_Cascade_x(cb) + offset_x,
			    cb->rectangle.y + CBG_Cascade_y(cb) + offset_y,
			    width, height);

	     XmeDrawShadows(XtDisplay(cb), XtWindow(XtParent(cb)),
			    LabG_TopShadowGC(cb), LabG_BottomShadowGC(cb),
			    cb->rectangle.x + CBG_Cascade_x(cb) + offset_x -
			    G_ShadowThickness(cb),
			    cb->rectangle.y + CBG_Cascade_y(cb) + offset_y -
			    G_ShadowThickness(cb),
			    width + (2* G_ShadowThickness(cb)),
			    height +  (2* G_ShadowThickness(cb)),
			    G_ShadowThickness(cb),
			    XmSHADOW_OUT);
	   }
	 else
	   {
	     XFillRectangle(XtDisplay(cb), XtWindow(XtParent(cb)),
			    LabG_BackgroundGC(cb),
			    cb->rectangle.x + CBG_Cascade_x(cb) +
			    G_ShadowThickness(cb),
			    cb->rectangle.y + CBG_Cascade_y(cb) + offset_y,
			    width, height);

	     XmeDrawShadows(XtDisplay(cb), XtWindow(XtParent(cb)),
			    LabG_TopShadowGC(cb), LabG_BottomShadowGC(cb),
			    cb->rectangle.x + CBG_Cascade_x(cb),
			    cb->rectangle.y + CBG_Cascade_y(cb) + offset_y -
			    G_ShadowThickness(cb),
			    width + (2* G_ShadowThickness(cb)),
			    height +  (2* G_ShadowThickness(cb)),
			    G_ShadowThickness(cb), XmSHADOW_OUT);
	   }
      }
      else {
	  Pixmap pixmap ;
	  int depth ;

	  pixmap = CBG_IsArmed(cb) &&
	      (CBG_ArmedPixmap(cb) != XmUNSPECIFIED_PIXMAP) ?
		  CBG_ArmedPixmap(cb) : CBG_CascadePixmap(cb) ;

	  XmeGetPixmapData(XtScreen(cb),
			   pixmap,
			   NULL,
			   &depth,
			   NULL, NULL,
			   NULL, NULL,
			   NULL, NULL);

	  if (depth == XtParent(cb)->core.depth)
	      XCopyArea (XtDisplay(cb), pixmap,
			 XtWindow(XtParent(cb)),
			 LabG_NormalGC(cb), 0, 0,
			 CBG_Cascade_width(cb), CBG_Cascade_height(cb),
			 cb->rectangle.x + CBG_Cascade_x(cb),
			 cb->rectangle.y + CBG_Cascade_y(cb));
	 else
	 if (depth == 1)
	     XCopyPlane (XtDisplay(cb), pixmap,
			 XtWindow(XtParent(cb)),
			 LabG_NormalGC(cb), 0, 0,
			 CBG_Cascade_width(cb), CBG_Cascade_height(cb),
			 cb->rectangle.x + CBG_Cascade_x(cb),
			 cb->rectangle.y + CBG_Cascade_y(cb), 1);
      }
   }
}


/*
 * set up the cascade position.
 */
static void
position_cascade(
        XmCascadeButtonGadget cascadebtn )
{
   Dimension buffer;

   if (CBG_HasCascade(cascadebtn))
   {
      if (LayoutIsRtoLG(cascadebtn))
         CBG_Cascade_x(cascadebtn) = cascadebtn->gadget.highlight_thickness +
                                     cascadebtn->gadget.shadow_thickness +
                                     LabG_MarginWidth(cascadebtn);
      else
	CBG_Cascade_x(cascadebtn) = cascadebtn->rectangle.width -
	                            cascadebtn->gadget.highlight_thickness -
	                            cascadebtn->gadget.shadow_thickness -
	                            LabG_MarginWidth(cascadebtn) -
		                    CBG_Cascade_width(cascadebtn);

      buffer = cascadebtn->gadget.highlight_thickness +
             cascadebtn->gadget.shadow_thickness +
             LabG_MarginHeight(cascadebtn);

      CBG_Cascade_y(cascadebtn) = buffer +
                               ((cascadebtn->rectangle.height -  2*buffer) -
                                CBG_Cascade_height(cascadebtn)) / 2;
   }
   else
   {
      CBG_Cascade_y(cascadebtn) = 0;
      CBG_Cascade_x(cascadebtn) = 0;
   }
}

/*
 * redisplay the widget
 */
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
        XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
	XtExposeProc expose;

    if (XtIsRealized ((Widget)cb))
    {
       XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
       Boolean etched_in = dpy->display.enable_etched_in_menu;
       GC tmpGC = NULL;

       if (LabG_IsMenupane(cb))
       {
	  XmMenuShellWidget mshell = (XmMenuShellWidget)XtParent(XtParent(cb));

	  if (!mshell->shell.popped_up)
	      return;
       }

       /*
	* This might be necessary in an option menu to keep the glyph from
	* moving when it's items vary in size.
	*/
       if (LabG_MenuType(cb) == XmMENU_OPTION)
          position_cascade(cb);

	if (etched_in) {
	    XFillRectangle (XtDisplay(cb), XtWindow(XtParent(cb)),
	                CBG_IsArmed(cb) ? CBG_ArmGC(cb) : CBG_BackgroundGC(cb),
			    cb->rectangle.x, cb->rectangle.y,
			    cb->rectangle.width, cb->rectangle.height);
	}

	if (etched_in && CBG_IsArmed(cb)) {
	    Pixel	junk, select_pix;
	    XmManagerWidget mw = (XmManagerWidget) XtParent(cb);
	    Boolean replaceGC = False;
	    XGCValues values;
	    GC tmp_bgc = NULL;

	    XmGetColors(XtScreen(mw), mw->core.colormap,
		mw->core.background_pixel,
                &junk, &junk, &junk, &select_pix);

	    if (select_pix == mw->manager.foreground) {
		/* mw->core.background_pixel */
		replaceGC = True;
		tmpGC = LabG_NormalGC(cb);
		LabG_NormalGC(cb) = CBG_BackgroundGC(cb);
	    }

	    /* Fetch the select_color GetGC() actually used. */
	    XGetGCValues(XtDisplay(cb), LabG_BackgroundGC(cb), GCBackground, &values);
	    if (values.background != select_pix)
	    {
			values.background = select_pix;
			XChangeGC(XtDisplay((Widget)cb), LabG_BackgroundGC(cb), GCBackground, &values);
		}
	    tmp_bgc = LabG_BackgroundGC(cb);
	    LabG_BackgroundGC(cb) = CBG_ArmGC(cb);

	    _XmProcessLock();
	    expose = xmLabelGadgetClassRec.rect_class.expose;
	    _XmProcessUnlock();
	    (* expose)((Widget)cb, event, region);

	    /* Restore default bg GC*/
	    LabG_BackgroundGC(cb) = tmp_bgc;

	    if (replaceGC)
		LabG_NormalGC(cb) = tmpGC;
	}
	else {

	    /* Label class does most of the work */
	    _XmProcessLock();
	    expose = xmLabelGadgetClassRec.rect_class.expose;
	    _XmProcessUnlock();
	    (* expose)((Widget)cb, event, region);
	}

	DrawCascade(cb);
	DrawShadow (cb);
    }
}


/*
 * Input sent by a manager is dispatched here.  The gadget handles Arm,
 * Activate, Enter, Leave, FocusIn, FocusOut and Help events.
 */
static void
InputDispatch(
        Widget wid,
        XEvent *event,
        Mask event_mask )
{
        XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
   if (event_mask & XmARM_EVENT)
   {
      if (LabG_MenuType(cb) == XmMENU_OPTION)
         ArmAndPost (cb, event);

      else if (LabG_MenuType(cb) == XmMENU_BAR)
	  MenuBarSelect ((Widget) cb, event);

      else
         StartDrag (cb, event);
   }

   else if (event_mask & XmBDRAG_EVENT)
   {
      _XmProcessDrag ((Widget) cb, event, NULL, NULL);
   }

   else if (event_mask & XmACTIVATE_EVENT)
   {
      if ((LabG_MenuType(cb) == XmMENU_PULLDOWN) ||
          (LabG_MenuType(cb) == XmMENU_POPUP) ||
	  (LabG_MenuType(cb) == XmMENU_BAR))
      {
          if (event->type == ButtonRelease)
             DoSelect (cb, event);

          else if (event->type == KeyPress)
             KeySelect (cb, event);

          CBG_SetWasPosted(cb, FALSE);
      }
      /* else option menu - do nothing if menu was not posted on btndown */
   }

   else if (event_mask & XmENTER_EVENT)
   {
      if (LabG_MenuType(cb) == XmMENU_BAR)
	  MenuBarEnter ((Widget) cb, event);

      else if (LabG_MenuType(cb) == XmMENU_OPTION)
	  _XmEnterGadget ((Widget) cb, event, NULL, NULL);

      else
	  DelayedArm (cb, event);
   }

   else if (event_mask & XmLEAVE_EVENT)
   {
      if (LabG_MenuType(cb) == XmMENU_BAR)
	  MenuBarLeave ((Widget) cb);
      else if (LabG_MenuType(cb) == XmMENU_OPTION)
	  _XmLeaveGadget( (Widget) cb, event, NULL, NULL);
      else
	  CheckDisarm (cb, event);
   }

   else if (event_mask & XmFOCUS_IN_EVENT)
      (* (((XmCascadeButtonGadgetClassRec *)(cb->object.widget_class))->
		gadget_class.border_highlight)) ((Widget) cb);

   else if (event_mask & XmFOCUS_OUT_EVENT)
   {
      if (LabG_IsMenupane(cb) &&
          (((XmManagerWidget)XtParent(cb))->manager.active_child == wid) &&
	  CBG_Submenu(cb))
      {
	  XmMenuShellWidget mshell =
	      (XmMenuShellWidget) XtParent(CBG_Submenu(cb));

	  if ((mshell->composite.children[0] == CBG_Submenu(cb)) &&
	      (XmIsMenuShell(mshell)) &&
	      (mshell->shell.popped_up))
	  {
	    return;
	  }
      }

      (* (((XmCascadeButtonGadgetClassRec *)(cb->object.widget_class))->
		gadget_class.border_unhighlight)) ((Widget) cb);
    }
    else if (event_mask & XmHELP_EVENT)
       _XmCBHelp((Widget) cb, event, NULL, NULL);
}


/*
 * Arming the CascadeButtonGadget consists of setting the armed bit
 * and drawing the 3D shadow.  CascadeButtonGadgets in
 * option menus are never armed since they will never get the event
 * to cause it to unarm.
 */
static void
Arm(
        XmCascadeButtonGadget cb )
{
   if ((LabG_MenuType(cb) != XmMENU_OPTION) &&
       (! CBG_IsArmed(cb)))
   {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      CBG_SetArmed(cb, TRUE);

      if (etched_in)
	  Redisplay((Widget) cb, NULL, NULL);
      else {
	  DrawCascade(cb);
	  DrawShadow (cb);
      }
   }
   XmProcessTraversal((Widget) cb, XmTRAVERSE_CURRENT);
}



/*
 * Post any submenus and then arm the gadget.  The order is important for
 * performance.
 */
static void
ArmAndPost(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
   XmMenuState mst = _XmGetMenuState((Widget)cb);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(cb)), XmQTmenuSystem);

   if (menuSTrait == NULL) return;

   if (!CBG_IsArmed(cb))
   {
      if ((LabG_MenuType(cb) == XmMENU_OPTION) &&
	  (XtParent(cb) == mst->RC_ReplayInfo.toplevel_menu) &&
	  (event->xbutton.time == mst->RC_ReplayInfo.time))
	 return;

      _XmCascadingPopup ((Widget) cb, event, TRUE);
      Arm(cb);

      /*
       * Option menus must be armed since the post just arms the
       * submenu
       */
      if (LabG_MenuType(cb) == XmMENU_OPTION)
      {
	menuSTrait -> arm((Widget) cb);
      }

      /*
       * Record so spring loaded DispatchEvent() doesn't handle this event
       */
      if (event)
	  _XmRecordEvent(event);
   }
}

/*
 * class function to cause the cascade button to be armed and selected
 */
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
   XmAnyCallbackStruct cback;
   XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(cb);
   Time _time = _XmGetDefaultTime(wid, event);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

   /* check if event has been processed */
   if (event && !_XmIsEventUnique(event))
      return;

   if (menuSTrait == NULL) return;

   switch (LabG_MenuType(cb))
   {
    case XmMENU_OPTION:
    {
       ArmAndPost (cb, event);
       if (CBG_Submenu(cb))
       {
          /*
           * if XmProcessTraversal() fails, it's possible that the pane
           * has no traversable children, so reset the focus to the pane.
           */
          if (!XmProcessTraversal(CBG_Submenu(cb), XmTRAVERSE_CURRENT))
	  {
	     /* Must clear focus path first for shared menushells.
	      * Otherwise, moving the focus back will have old stale
	      * (old) focus_item.
	      */
	     _XmClearFocusPath(CBG_Submenu(cb));
             XtSetKeyboardFocus(XtParent(CBG_Submenu(cb)), CBG_Submenu(cb));
	  }
       }
       break;
    }

    case XmMENU_PULLDOWN:
    case XmMENU_POPUP:
    {
       /* In case the tear off is active but not armed or grabbed */
       menuSTrait -> tearOffArm((Widget) parent);

       Select (cb, event, TRUE);
       if (CBG_Submenu(cb))
       {
          /*
           * if XmProcessTraversal() fails, it's possible that the pane
           * has no traversable children, so reset the focus to the pane.
           */
          if (!XmProcessTraversal(CBG_Submenu(cb), XmTRAVERSE_CURRENT))
          {
             /* Must clear focus path first for shared menushells.
              * Otherwise, moving the focus back will have old stale
              * (old) focus_item.
              */
             _XmClearFocusPath(CBG_Submenu(cb));
             XtSetKeyboardFocus(XtParent(CBG_Submenu(cb)), CBG_Submenu(cb));
	  }
       }
       break;
    }

    case XmMENU_BAR:
    {
       ShellWidget myShell = NULL;

       /* Shared menupanes require some additional checks */
       if (CBG_Submenu(cb))
	   myShell = (ShellWidget)XtParent(CBG_Submenu(cb));

       if (myShell &&
	   XmIsMenuShell(myShell) &&         /* not torn ?! */
           (myShell->shell.popped_up) &&
	   (myShell->composite.children[0] == CBG_Submenu(cb)) &&
	   (cb == (XmCascadeButtonGadget)RC_CascadeBtn(CBG_Submenu(cb))))
       {
	  menuSTrait -> popdown((Widget) parent, event);
	  Disarm (cb, FALSE);
       }

       else
       {
	  /* call the cascading callbacks first thing */
	  cback.reason = XmCR_CASCADING;
	  cback.event = event;
	  XtCallCallbackList ((Widget) cb, CBG_CascadeCall(cb), &cback);

	  /*
	   * check if the traversing flag is set true.  This indicates
	   * that we are in a traverse and don't want to activate if
	   * there is no submenu attached.  Set during KDown in menubar.
	   */
	  if (CBG_Traversing(cb) && !CBG_Submenu(cb))
	      return;

	  if (! RC_IsArmed (parent))
	  {
	     _XmMenuFocus((Widget) parent, XmMENU_BEGIN, _time);

	     if (CBG_Submenu (cb) && menuSTrait != NULL)
	       menuSTrait -> arm((Widget) cb);
	  }
	  else
	     menuSTrait -> menuBarCleanup((Widget) parent);

	  /* do the select without calling the cascading callbacks again */
	  Select (cb, event, FALSE);

          /* To support menu replay, keep the pointer in sync mode */
          XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

	  if (CBG_Submenu(cb))
	  {
             /*
              * if XmProcessTraversal() fails, it's possible that the pane
              * has no traversable children, so reset the focus to the pane.
              */
             if (!XmProcessTraversal(CBG_Submenu(cb), XmTRAVERSE_CURRENT))
	     {
		/* Must clear focus path first for shared menushells.
		 * Otherwise, moving the focus back will have old stale
		 * (old) focus_item.
		 */
		_XmClearFocusPath(CBG_Submenu(cb));
                XtSetKeyboardFocus(XtParent(CBG_Submenu(cb)), CBG_Submenu(cb));
	     }
	  }
	  else
	  {
 	     menuSTrait -> disarm((Widget) parent);
	     _XmMenuFocus(XtParent(cb), XmMENU_END, _time);
	     XtUngrabPointer(XtParent(cb), _time);
	  }
       }

       break;
    }
   }
   /* Record so spring loaded DispatchEvent() doesn't recall this routine.  */
   if (event)
      _XmRecordEvent(event);
}


/*
 * Disarm the menu.  This may include popping down any submenu that is up
 * and removing the timeout to post a submenu.
 */
static void
Disarm(
        XmCascadeButtonGadget cb,
        Boolean unpost )
{
   Widget rowcol = XtParent(cb);

   if (CBG_IsArmed(cb))
   {
      CBG_SetArmed(cb,FALSE);

      /* popdown  any posted submenus */
      if ((unpost) &&  (RC_PopupPosted(rowcol)))
      {
	  (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	     menu_shell_class.popdownEveryone))
	      (RC_PopupPosted(rowcol),NULL, NULL, NULL);
      }

      /* if a delayed arm is pending, remove it */
      if (CBG_Timer(cb))
      {
         XtRemoveTimeOut (CBG_Timer(cb));
         CBG_Timer(cb) = 0;
      }

      /* if the shadow is drawn and the menupane is not going down, erase it */
      if ((! RC_PoppingDown(rowcol)) || RC_TornOff(rowcol))
      {
	 if (XtIsRealized(rowcol))
	 {
	     /* etched in menu button */
	     XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
	     Boolean etched_in = dpy->display.enable_etched_in_menu;

	     if (etched_in)
		 Redisplay((Widget) cb, NULL, NULL);
	     else
		 XmeDrawHighlight(XtDisplay(cb),
				  XtWindow(cb),
				  LabG_BackgroundGC(cb),
			    cb->gadget.highlight_thickness + cb->rectangle.x,
			    cb->gadget.highlight_thickness + cb->rectangle.y,
				  cb->rectangle.width - 2 *
				  cb->gadget.highlight_thickness,
				  cb->rectangle.height - 2 *
				  cb->gadget.highlight_thickness,
				  cb->gadget.shadow_thickness);
	 }
      }
      DrawCascade(cb);
   }
}


/*
 * called when the post delay timeout occurs.
 */
static void
PostTimeout(
	XtPointer closure,
	XtIntervalId *id)	/* unused */
{
        XmCascadeButtonGadget cb = (XmCascadeButtonGadget) closure ;

   if (CBG_Timer(cb))
   {
      CBG_Timer(cb) = 0;

      _XmCascadingPopup ((Widget) cb, NULL, TRUE);

   }
}



/*
 * set the timer to post the submenu if a leave event does
 * not occur first.
 */
static void
DelayedArm(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
   if ((! CBG_IsArmed(cb)) &&
       (((XmMenuShellWidget) XtParent(XtParent(cb)))->shell.popped_up) &&
       _XmGetInDragMode((Widget) cb))

   {
      if (CBG_MapDelay(cb) <= 0)
      {
	 /* don't delay, just do it */
	 ArmAndPost (cb, event);
      }
      else
      {
 	 /* To fix CR 8172,  the following two lines were reversed.
	    Because calling Arm seems to cause a focus change (temporary)
	    out of the widget,  the timer was incorrectly removed and
	    the menu wouldn't post. */
         Arm(cb);
	 /* NOTE!! XtAppAddTimeOut returns XtIntervalId (unsigned long)
	  * but the timer field was declared as an int in this gadget.
	  *
	  */
         CBG_Timer(cb) =
	   XtAppAddTimeOut(XtWidgetToApplicationContext( (Widget) cb),
			   (unsigned long) CBG_MapDelay(cb),
			   PostTimeout, (XtPointer) cb) ;
      }
   }
}


/*
 * if traversal is not on and the mouse
 * has not entered its cascading submenu, disarm the
 * CascadeButtonGadget.
 */
static void
CheckDisarm(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
   XmMenuShellWidget submenushell;
   XMotionEvent * entEvent = (XMotionEvent *) event;

   if (_XmGetInDragMode((Widget) cb))
   {
      if ((CBG_IsArmed(cb)) &&
          (CBG_Submenu(cb)))
      {
         submenushell = (XmMenuShellWidget) XtParent (CBG_Submenu(cb));

         if (submenushell->shell.popped_up)
         {
            if ((entEvent->x_root >= submenushell->core.x) &&
                (entEvent->x_root <  submenushell->core.x +
                                     submenushell->core.width +
                                     (submenushell->core.border_width << 1)) &&
                (entEvent->y_root >= submenushell->core.y) &&
                (entEvent->y_root <  submenushell->core.y +
                                     submenushell->core.height +
	   			     (submenushell->core.border_width << 1)))

  	        /* then we are in the cascading submenu, don't disarm */
 	        return;
         }
      }
      Disarm (cb, TRUE);
   }
}


/*
 * post submenu and disable traversal.  These functions must be called
 * in this order.
 */
static void
StartDrag(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
   XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(cb);
   XmMenuShellWidget mshell = (XmMenuShellWidget) XtParent(parent);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(cb)), XmQTmenuSystem);

   /* Start with posted submenu bit reset */
   CBG_SetWasPosted(cb, FALSE);

   if (CBG_Submenu(cb) &&
       RC_IsArmed ((XmRowColumnWidget) CBG_Submenu(cb))) {
     CBG_SetWasPosted(cb, TRUE);
   }

   if (LabG_IsMenupane(cb) &&
       ! mshell->shell.popped_up)
   {
      return;
   }

   /* In case the tear off is active but not armed or grabbed */
   if (menuSTrait != NULL) menuSTrait -> tearOffArm((Widget) parent);

   _XmSetInDragMode((Widget) cb, True);

   _XmCascadingPopup ((Widget) cb, event, TRUE);
   Arm (cb);

   /* record event so MenuShell does not process it */
   _XmRecordEvent (event);
}


/*
 * do the popup and if there is not a submenu, bring down the menu system.
 */
static void
Select(
        XmCascadeButtonGadget cb,
        XEvent *event,
        Boolean doCascade )
{
   XmAnyCallbackStruct cback;
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(cb)), XmQTmenuSystem);
   if (menuSTrait == NULL) return;

   if (CBG_WasPosted(cb)) {
     Disarm(cb, TRUE);
     if ((CBG_Submenu(cb) != NULL) && (LabG_MenuType(cb) == XmMENU_BAR))
       _XmMenuPopDown(XtParent((Widget) cb), event, NULL);
     return;
   }

   _XmCascadingPopup ((Widget) cb, event, doCascade);

   /*
    * check if there is a submenu here in case this changed during
    * the cascadeing callbacks
    */
   if (CBG_Submenu(cb) == NULL)
   {
      if (menuSTrait != NULL)
	menuSTrait -> popdown(XtParent(cb), event);

      Disarm (cb, FALSE);

      if (menuSTrait != NULL)
	menuSTrait -> disarm(XtParent(cb));

      cback.event = event;
      cback.reason = XmCR_ACTIVATE;

      if (menuSTrait != NULL)
	menuSTrait -> entryCallback(XtParent(cb), (Widget) cb,
					     &cback);

      if ((! LabG_SkipCallback(cb)) &&
	  (CBG_ActivateCall(cb)))
      {
	 XtCallCallbackList ((Widget) cb, CBG_ActivateCall(cb), &cback);
      }
   }
   else
   {
      Arm (cb);
   }
}



/*
 * if there is a submenu, enable traversal.
 * call select to do the work
 */
static void
DoSelect(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
  Time _time = _XmGetDefaultTime((Widget) cb, event);

   if ((LabG_MenuType(cb) == XmMENU_BAR) &&
       ! RC_IsArmed(XtParent(cb)))
       return;

   /*
    * make sure the shell is popped up, this takes care of a corner case
    * that can occur with rapid pressing of the mouse button
    */
   if (LabG_IsMenupane(cb) &&
       (!((XmMenuShellWidget) XtParent(XtParent(cb)))->shell.popped_up))
   {
      return;
   }

   Select(cb, event, (Boolean)(CBG_Submenu(cb) != NULL));

   /*
    * don't let the menu shell widget process this event
    */
   _XmRecordEvent (event);

   _XmSetInDragMode((Widget) cb, False);

   if (CBG_Submenu(cb))
   {
      /*
       * if XmProcessTraversal() fails, it's possible that the pane has
       * no traversable children, so reset the focus to the pane.
       */
      if (!XmProcessTraversal(CBG_Submenu(cb), XmTRAVERSE_CURRENT))
      {
	 /* Must clear focus path first for shared menushells.
	  * Otherwise, moving the focus back will have old stale
	  * (old) focus_item.
	  */
	 _XmClearFocusPath(CBG_Submenu(cb));
         XtSetKeyboardFocus(XtParent(CBG_Submenu(cb)), CBG_Submenu(cb));
      }
   }
   else
   {
      /* *** Move this call to Select() ***
       *
       * (* xmLabelGadgetClassRec.label_class.menuProcs)
       *    (XmMENU_DISARM, XtParent(cb));
       */

      if (LabG_MenuType(cb) == XmMENU_BAR)
      {
	 _XmMenuFocus(XtParent(cb), XmMENU_END, _time);
	 XtUngrabPointer(XtParent(cb), _time);
      }
   }
}


/*
 * if the menu system traversal is enabled, do a select
 */
static void
KeySelect(
        XmCascadeButtonGadget cb,
        XEvent *event )
{
   XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(cb);
   XmMenuSystemTrait menuSTrait;

   /* check if event has been processed */
   if (!_XmIsEventUnique(event))
      return;

   if (!_XmGetInDragMode((Widget) cb) && RC_IsArmed(parent))
   {
      if (LabG_MenuType(cb) == XmMENU_BAR) {
	menuSTrait = (XmMenuSystemTrait)
	  XmeTraitGet((XtPointer) XtClass(XtParent(cb)), XmQTmenuSystem);
	if (menuSTrait != NULL)
	  menuSTrait -> menuBarCleanup((Widget) parent);
      }

      Select(cb, event, TRUE);

      if (CBG_Submenu(cb))
      {
         XmProcessTraversal(CBG_Submenu(cb), XmTRAVERSE_CURRENT);
      }
   }

   /* record so that menuShell does not process this event */
   _XmRecordEvent(event);
}


/*
 * If the menu system is not active, arm it and arm this cascadebutton
 * else start the drag mode
 */
static void
MenuBarSelect(
        Widget wid,
        XEvent *event )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   Boolean validButton;
   Time _time = _XmGetDefaultTime(wid, event);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

   if (menuSTrait == NULL) return;

   CBG_SetWasPosted(cb, FALSE);

   if (RC_IsArmed ((XmRowColumnWidget) XtParent(cb)))
   {
      /* Cleanup the PM menubar mode, if enabled */
      menuSTrait -> menuBarCleanup(XtParent(cb));

      if (!CBG_Submenu(cb))
      {
	 _XmMenuFocus(XtParent(cb), XmMENU_MIDDLE, _time);
      }

      StartDrag ((XmCascadeButtonGadget) cb, event);

   }

   else
   {
      validButton = menuSTrait -> verifyButton(XtParent(cb), event);

      if (validButton)
      {
         /* Don't post the menu if the menu cannot control grabs! */

         if (_XmMenuGrabKeyboardAndPointer(XtParent(cb), _time) != GrabSuccess)
         {
            _XmRecordEvent (event);
            return;
	 }

         _XmMenuFocus(XtParent(cb), XmMENU_BEGIN, _time);

	 menuSTrait -> arm((Widget) cb);

         _XmSetInDragMode((Widget) cb, True);

         _XmCascadingPopup ((Widget) cb, event, TRUE);

	 /* To support menu replay, keep the pointer in sync mode */
	 XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

	 if (!CBG_Submenu(cb))
	 {
	    /*
	     * since no submenu is posted, check if the grab has occured
	     * and if not, do the pointer grab now.
	     */
	    if (RC_BeingArmed(XtParent(cb)))
	    {
	       Cursor cursor;

	       cursor = _XmGetMenuCursorByScreen(XtScreen(cb));

               _XmGrabPointer(XtParent(cb), True, EVENTS,
                  GrabModeAsync, GrabModeAsync, None, cursor, _time);

	       RC_SetBeingArmed(XtParent(cb), False);
	    }
	 }

	 /* record so that menuShell doesn't process this event */
	 _XmRecordEvent (event);
      }
   }
}


/*
 * If the menu is active, post submenu and arm.
 */
static void
MenuBarEnter(
        Widget wid,
        XEvent *event )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   XmRowColumnWidget rc = (XmRowColumnWidget)XtParent(cb);

   if ((RC_IsArmed (rc)) && (! CBG_IsArmed(cb))
       && _XmGetInDragMode((Widget) cb))
   {
      if (!CBG_Submenu(cb))
      {
	 _XmMenuFocus( (Widget) rc, XmMENU_MIDDLE,
		      _XmGetDefaultTime(wid, event));
      }

      _XmCascadingPopup ((Widget) cb, event, TRUE);
      Arm((XmCascadeButtonGadget) cb);
   }
}


/*
 * unless our submenu is posted or traversal is on, disarm
 */
static void
MenuBarLeave(
        Widget wid )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   XmMenuShellWidget submenuShell;

   if (RC_IsArmed (XtParent (cb)))
   {
      /* Reset this bit so that we don't unpost if the user
	 reenters the cascade button */
      CBG_SetWasPosted(cb, FALSE);

      if (CBG_Submenu(cb))
      {
         submenuShell = (XmMenuShellWidget) XtParent(CBG_Submenu(cb));

         if (submenuShell->shell.popped_up)
            return;
      }

      if (_XmGetInDragMode((Widget) cb))
         Disarm ((XmCascadeButtonGadget) cb, TRUE);
   }
}


/*
 * Create the CB and CBG 3d arrows.
 * The Pixmaps must be unspecified going into this routine.  This helps to
 * make sure arrow cache is sync'd up as well as not accidentally overwriting
 * application's pixmap arrow if set.
 */
void
_XmCreateArrowPixmaps(
        Widget wid )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   XmArrowPixmap cpart;
   XmArrowPixmap *armed_arrow, *unarmed_arrow;
   GC gc, tsGC, bsGC;
   XGCValues values;
   Pixmap pixmap;
   int ht, st;
   Pixel tsc, bsc, bg;
   Dimension side;
   Screen *screen;
   int depth;
   unsigned char arrow_direction;
   unsigned short text_height;
   GC armGC = NULL;
   XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
   Boolean etched_in = dpy->display.enable_etched_in_menu;

   screen = XtScreen(cb);
   if (XmIsGadget(cb))
   {
      if ((CBG_CascadePixmap(cb) != XmUNSPECIFIED_PIXMAP) &&
          (CBG_ArmedPixmap(cb) != XmUNSPECIFIED_PIXMAP))
        return;

      ht = G_HighlightThickness(cb);
      st = G_ShadowThickness(cb);
      tsc = LabG_TopShadowColor(cb);
      bsc = LabG_BottomShadowColor(cb);
      bg = LabG_Background(cb);
      tsGC = LabG_TopShadowGC(cb);
      bsGC = LabG_BottomShadowGC(cb);
      arrow_direction = (LayoutIsRtoLP(cb)) ? XmARROW_LEFT : XmARROW_RIGHT;
      text_height = LabG_TextRect_height(cb);
      depth = XtParent(cb)->core.depth;

      if (etched_in)
          armGC = CBG_ArmGC(wid);

   }
   else
   {
      Pixel       select_pixel;

      if ((CB_CascadePixmap(cb) != XmUNSPECIFIED_PIXMAP) &&
          (CB_ArmedPixmap(cb) != XmUNSPECIFIED_PIXMAP))
        return;

      XmGetColors(XtScreen(wid), wid->core.colormap,
		  wid -> core.background_pixel,
		  NULL, NULL, NULL, &select_pixel);

      if (etched_in)
      {
          values.foreground = values.background = select_pixel;
          values.graphics_exposures = False;
          armGC = XtGetGC ((Widget) wid,
              GCForeground | GCBackground | GCGraphicsExposures, &values);
      }

      ht = cb->primitive.highlight_thickness;
      st = cb->primitive.shadow_thickness;
      tsc =  cb->primitive.top_shadow_color;
      bsc =  cb->primitive.bottom_shadow_color;
      bg = cb->core.background_pixel;
      tsGC = cb->primitive.top_shadow_GC;
      bsGC = cb->primitive.bottom_shadow_GC;
      arrow_direction = (LayoutIsRtoLP(cb)) ? XmARROW_LEFT : XmARROW_RIGHT;
      text_height = Lab_TextRect_height(cb);
      depth = cb->core.depth;
   }

   side = MAX( (text_height * 2 / 3) + 2 * (ht + st),
	      (2*(ht + (st-1) +1)) +1 );       /* see _XmGetArrowDrawRects() */

   cpart.height = cpart.width = side;
   cpart.depth = depth;
   cpart.direction = arrow_direction;
   cpart.top_shadow_color = tsc;
   cpart.bottom_shadow_color = bsc;
   cpart.foreground_color = bg;
   cpart.display = XtDisplay(cb);
   cpart.screen = screen;
   cpart.pixmap = XmUNSPECIFIED_PIXMAP;

   /*
    * Create or get an existing arrow cache record for the unarmed arrow
    */
   _XmProcessLock();
   unarmed_arrow = (XmArrowPixmap *)
       _XmCachePart(&ArrowPixmapCache, (XtPointer) &cpart,
		    sizeof(XmArrowPixmap));

   /*
    * Create or get an existing arrow cache record for the armed arrow
    */
   cpart.top_shadow_color = bsc;
   cpart.bottom_shadow_color = tsc;

   armed_arrow =  (XmArrowPixmap *)
      _XmCachePart(&ArrowPixmapCache, (XtPointer) &cpart,
		   sizeof(XmArrowPixmap));
   _XmProcessUnlock();

   if ((unarmed_arrow->pixmap == XmUNSPECIFIED_PIXMAP) ||
       (armed_arrow->pixmap == XmUNSPECIFIED_PIXMAP))
   {
      values.foreground = values.background = bg;
      values.graphics_exposures = False;
      gc = XtGetGC ((Widget) cb,
                   GCForeground | GCBackground | GCGraphicsExposures, &values);

      /* armed arrow */
      if (armed_arrow->pixmap == XmUNSPECIFIED_PIXMAP)
      {
         pixmap = XCreatePixmap(XtDisplay(cb), RootWindowOfScreen(screen),
            side, side, depth);

         armed_arrow->pixmap = pixmap;

         XFillRectangle(XtDisplay(cb), pixmap, etched_in? armGC : gc,
			0, 0, side, side);
	 XmeDrawArrow(XtDisplay((Widget)cb), pixmap,
		bsGC, tsGC, gc,
		ht + st - 1,
		ht + st - 1,
		side - 2*(ht + st - 1),
		side - 2*(ht + st - 1),
		st, arrow_direction);
      }

      /* standard (unarmed) pixmap */
      if (unarmed_arrow->pixmap == XmUNSPECIFIED_PIXMAP)
      {
         pixmap = XCreatePixmap(XtDisplay(cb),  RootWindowOfScreen(screen),
            side, side, depth);

         unarmed_arrow->pixmap = pixmap;

         XFillRectangle(XtDisplay(cb), pixmap, gc, 0, 0, side, side);
	 XmeDrawArrow(XtDisplay((Widget)cb), pixmap,
	 tsGC, bsGC, gc,
	 	ht + st - 1,
	 	ht + st - 1,
	 	side - 2*(ht + st - 1),
	 	side - 2*(ht + st - 1),
	 	st, arrow_direction);
      }
      XtReleaseGC( (Widget) cb, gc);
   }
   if (XmIsGadget(cb))
   {
      CBG_ArmedPixmap(cb) = armed_arrow->pixmap;
      CBG_CascadePixmap(cb) = unarmed_arrow->pixmap;
   }
   else
   {
      CB_ArmedPixmap(cb) = armed_arrow->pixmap;
      CB_CascadePixmap(cb) = unarmed_arrow->pixmap;
   }
}


/*
 * get the cascade size set up
 */
static void
size_cascade(
        XmCascadeButtonGadget cascadebtn )
{
    Window rootwin;
    int x,y;					/* must be int */
    unsigned int width, height, border, depth;  /* must be unsigned int */

    if (CBG_CascadePixmap(cascadebtn) != XmUNSPECIFIED_PIXMAP)
    {
       XGetGeometry(XtDisplay(cascadebtn), CBG_CascadePixmap(cascadebtn),
		    &rootwin, &x, &y, &width, &height,
		    &border, &depth);

       CBG_Cascade_width(cascadebtn) = (Dimension) width;
       CBG_Cascade_height(cascadebtn) = (Dimension) height;
    }
    else
    {
       if (LabG_MenuType(cascadebtn) == XmMENU_OPTION)
       {
	  CBG_Cascade_width(cascadebtn) =
	     CBG_Cascade_height(cascadebtn) =
		MAX(LabG_TextRect(cascadebtn).height,
		    LabG_AccTextRect(cascadebtn).height) +
	        2 * cascadebtn->gadget.shadow_thickness;     /* glyph shadow */

       }
       else
       {
	  CBG_Cascade_width(cascadebtn) = 0;
	  CBG_Cascade_height(cascadebtn) = 0;
       }
    }
}


/*
 * set up the cascade size and location
 */
static void
setup_cascade(
        XmCascadeButtonGadget cascadebtn,
        Boolean adjustWidth,
        Boolean adjustHeight )
{
   Dimension delta;

   if (CBG_HasCascade(cascadebtn))
   {
      /*
       *  modify the size of the CascadeButtonGadget to acommadate the
       *  cascade, if needed.  The cascade should fit inside MarginRight.
       */
      if (LayoutIsRtoLG(cascadebtn))
      {
        if ((CBG_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE) >
             LabG_MarginLeft(cascadebtn))
        {
            delta = CBG_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE -
                LabG_MarginLeft(cascadebtn);
            LabG_MarginLeft(cascadebtn) += delta;

            if (adjustWidth)
                cascadebtn->rectangle.width += delta;

            else
            {
               if (LabG_Alignment(cascadebtn) == XmALIGNMENT_BEGINNING)
                   LabG_TextRect_x(cascadebtn) += delta;
               else if (LabG_Alignment(cascadebtn) == XmALIGNMENT_CENTER)
                   LabG_TextRect_x(cascadebtn) += delta/2;
            }
        }

      }
      else
      {

	if ((CBG_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE) >
	    LabG_MarginRight(cascadebtn))
	  {
	    delta = CBG_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE -
	      LabG_MarginRight(cascadebtn);
	    LabG_MarginRight(cascadebtn) = LabG_MarginRight(cascadebtn) +
	      delta;

	    if (adjustWidth)
	      cascadebtn->rectangle.width += delta;

	    else
	      {
		if (LabG_Alignment(cascadebtn) == XmALIGNMENT_END)
		  LabG_TextRect_x(cascadebtn) -= delta;
	    else if (LabG_Alignment(cascadebtn) == XmALIGNMENT_CENTER)
	      LabG_TextRect_x(cascadebtn) -= delta/2;

	      }
	  }
      }

      /*
       * the cascade height should fit inside of
       * TextRect + marginTop + marginBottom
       */
      delta = CBG_Cascade_height(cascadebtn) +
                   2 * (LabG_MarginHeight(cascadebtn) +
		         cascadebtn->gadget.shadow_thickness +
		         cascadebtn->gadget.highlight_thickness);

      if (delta > cascadebtn->rectangle.height)
      {
	 delta -= cascadebtn->rectangle.height;
	 LabG_MarginTop(cascadebtn) = LabG_MarginTop(cascadebtn) +
	   (delta/2);
	 LabG_TextRect_y(cascadebtn) += delta/2;
	 LabG_MarginBottom(cascadebtn) = LabG_MarginBottom(cascadebtn) +
	   delta - (delta/2);

	 if (adjustHeight)
	     cascadebtn->rectangle.height += delta;
      }
   }

   position_cascade(cascadebtn);
}


/*
 * Destroy the widget
 */
static void
Destroy(
        Widget wid )
{
    XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid ;
    XmRowColumnWidget submenu = (XmRowColumnWidget) CBG_Submenu(cb);
    XmMenuSystemTrait menuSTrait;
    XmManagerWidget mw = (XmManagerWidget) XtParent(cb);

    menuSTrait = (XmMenuSystemTrait)
      XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

    /*
     * break the submenu link
     */
    if (submenu != NULL && menuSTrait != NULL)
      menuSTrait -> recordPostFromWidget((Widget) submenu, (Widget) cb, FALSE);

    if (CBG_Timer(cb))
         XtRemoveTimeOut (CBG_Timer(cb));

    _XmProcessLock();
    if (CBG_ArmedPixmap(cb) != XmUNSPECIFIED_PIXMAP)
    {
       _XmArrowPixmapCacheDelete((XtPointer) CBG_ArmedPixmap(cb));
       _XmArrowPixmapCacheDelete((XtPointer) CBG_CascadePixmap(cb));
    }

   /* Release the GCs */
   XtReleaseGC ((Widget) mw, CBG_ArmGC(cb));
   XtReleaseGC ((Widget) mw, CBG_BackgroundGC(cb));

    _XmCacheDelete((XtPointer) CBG_Cache(cb));
    _XmProcessUnlock();
}


/*
 * routine to resize a cascade button, called by the parent
 * geometery manager
 */
static void
Resize(
        Widget wid )
{
  XmCascadeButtonGadget cb = (XmCascadeButtonGadget) wid;
  XtWidgetProc resize;
  if (cb)
    {
      /* Label class does it's work */
      _XmProcessLock();
      resize = xmLabelGadgetClassRec.rect_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) cb);

      /* move the cascade too */
      position_cascade (cb);
    }
}


/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/
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
    Cardinal                    size;
    XmCascadeButtonGCacheObject newSec, reqSec;

    _XmProcessLock();

    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmCascadeButtonGCacheObject)_XmExtObjAlloc(size);
    reqSec = (XmCascadeButtonGCacheObject)_XmExtObjAlloc(size);

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

    memcpy( &(newSec->label_cache),
            LabG_Cache(newParent),
            sizeof(XmLabelGCacheObjPart));

    memcpy( &(newSec->cascade_button_cache),
            CBG_Cache(newParent),
            sizeof(XmCascadeButtonGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    /*
     * Since the resource lists for label and cascadebutton were merged at
     * ClassInitialize time we need to make only one call to
     * XtSetSubvalues()
     */

    XtSetSubvalues((XtPointer)newSec,
                    ec->core_class.resources,
                    ec->core_class.num_resources,
                    args, *num_args);

    memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

    LabG_Cache(newParent) = &(((XmLabelGCacheObject)newSec)->label_cache);
    LabG_Cache(refParent) = &(((XmLabelGCacheObject)extData->reqWidget)->
			      label_cache);

    CBG_Cache(newParent) =
        &(((XmCascadeButtonGCacheObject)newSec)->cascade_button_cache);
    CBG_Cache(refParent) =
        &(((XmCascadeButtonGCacheObject)extData->reqWidget)->
	  cascade_button_cache);

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
static void
GetValuesPrehook(
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             extData;
    XmBaseClassExt              *cePtr;
    WidgetClass                 ec;
    Cardinal 			size;
    XmCascadeButtonGCacheObject newSec;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmCascadeButtonGCacheObject)_XmExtObjAlloc(size);
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

    memcpy( &(newSec->label_cache),
            LabG_Cache(newParent),
            sizeof(XmLabelGCacheObjPart));

    memcpy( &(newSec->cascade_button_cache),
            CBG_Cache(newParent),
            sizeof(XmCascadeButtonGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    /* Note that if a resource is defined in the superclass's as well as a
   subclass's resource list and if a NULL is passed in as the third
   argument to XtSetArg, then when a GetSubValues() is done by the
   superclass the NULL is replaced by a value. Now when the subclass
   gets the arglist it doesn't see a NULL and thinks it's an address
   it needs to stuff a value into and sure enough it breaks.
   This means that we have to pass the same arglist with the NULL to
   both the superclass and subclass and propagate the values up once
   the XtGetSubValues() are done.*/

    /*
     * Since the resource lists for label and cascadebutton were merged at
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
static void
GetValuesPosthook(
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
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
static Boolean
SetValuesPosthook(
        Widget current,
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
   XmWidgetExtData                  ext;
   /*
    * - register parts in cache.
    * - update cache pointers
    * - and free req
    */

  /* assign if changed! */
  _XmProcessLock();
  if (!_XmLabelCacheCompare((XtPointer)LabG_Cache(new_w),
			    (XtPointer)LabG_Cache(current)))
  {
  _XmCacheDelete((XtPointer) LabG_Cache(current));  /* delete the old one */
  LabG_Cache(new_w) = (XmLabelGCacheObjPart *)
      _XmCachePart(LabG_ClassCachePart(new_w),
		   (XtPointer) LabG_Cache(new_w),
		   sizeof(XmLabelGCacheObjPart));
  }
  else
       LabG_Cache(new_w) = LabG_Cache(current);

  /* assign if changed! */
  if (!_XmCascadeBCacheCompare((XtPointer)CBG_Cache(new_w),
                              (XtPointer)CBG_Cache(current)))
      {
      _XmCacheDelete((XtPointer) CBG_Cache(current));  /* delete the old one */
      CBG_Cache(new_w) = (XmCascadeButtonGCacheObjPart *)
	  _XmCachePart(CBG_ClassCachePart(new_w),
		       (XtPointer) CBG_Cache(new_w),
		       sizeof(XmCascadeButtonGCacheObjPart));
      }
  else
      CBG_Cache(new_w) = CBG_Cache(current);

  _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

  _XmExtObjFree((XtPointer) ext->widget);
  _XmExtObjFree((XtPointer) ext->reqWidget);

  _XmProcessUnlock();

  XtFree( (char *) ext);

  return FALSE;
}

/*
 * Set Values
 */
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmCascadeButtonGadget old = (XmCascadeButtonGadget) cw ;
    XmCascadeButtonGadget requested = (XmCascadeButtonGadget) rw ;
    XmCascadeButtonGadget new_w = (XmCascadeButtonGadget) nw ;
    Boolean flag = FALSE;
    Boolean adjustWidth = FALSE;
    Boolean adjustHeight = FALSE;
    XmMenuSystemTrait menuSTrait;

    menuSTrait = (XmMenuSystemTrait)
      XmeTraitGet((XtPointer) XtClass(XtParent(cw)), XmQTmenuSystem);

    if ((CBG_Submenu(new_w)) &&
	((! XmIsRowColumn(CBG_Submenu(new_w))) ||
	 (RC_Type(CBG_Submenu(new_w)) != XmMENU_PULLDOWN)))
    {
       CBG_Submenu(new_w) = NULL;
       XmeWarning( (Widget) new_w, WRONGSUBMENU);
    }

    if (CBG_MapDelay(new_w) < 0)
    {
       CBG_MapDelay(new_w) = CBG_MapDelay(old);
       XmeWarning( (Widget) new_w, WRONGMAPDELAY);
    }

    /* if there is a change to submenu, notify menu system */
    if (CBG_Submenu(old) != CBG_Submenu(new_w))
    {
       /* We must pass nw as the parameter to recordPostFromWidget
	* because old is a copy!  The call to recordPostFromWidget() does
	* a widget ID comparison and we must pass the real widget (nw).
	*/
       if (CBG_Submenu(old) && menuSTrait != NULL)
	 menuSTrait -> recordPostFromWidget(CBG_Submenu(old), nw, FALSE);

       if (CBG_Submenu(new_w) && menuSTrait != NULL)
	 menuSTrait -> recordPostFromWidget(CBG_Submenu(new_w), nw, TRUE);
    }

    if (LabG_MenuType(new_w) == XmMENU_BAR)
	new_w->gadget.traversal_on = TRUE;

    /* handle the cascade pixmap indicator */
    else if (LabG_MenuType(new_w) == XmMENU_PULLDOWN ||
	     LabG_MenuType(new_w) == XmMENU_POPUP ||
	     LabG_MenuType(new_w) == XmMENU_OPTION)
    {
       /* don't let traversal change */
       if (LabG_MenuType(new_w) != XmMENU_OPTION)
          new_w->gadget.traversal_on = TRUE;

       if (LabG_RecomputeSize(new_w) || (requested->rectangle.width <= 0))
	  adjustWidth = TRUE;

       if (LabG_RecomputeSize(new_w) || (requested->rectangle.height <= 0))
	  adjustHeight = TRUE;

       /* get new pixmap size */
       if (CBG_CascadePixmap (old) != CBG_CascadePixmap (new_w))
       {
          if (CBG_ArmedPixmap(old) != XmUNSPECIFIED_PIXMAP)
          {
	     _XmProcessLock();
             _XmArrowPixmapCacheDelete((XtPointer) CBG_ArmedPixmap(old));
             _XmArrowPixmapCacheDelete((XtPointer) CBG_CascadePixmap(old));
	     _XmProcessUnlock();
          }
          CBG_ArmedPixmap(new_w) = XmUNSPECIFIED_PIXMAP;
	  size_cascade (new_w);
       } else
	   if ( (LabG_MenuType(new_w) != XmMENU_OPTION) &&
	       (((CBG_CascadePixmap(new_w) ==  XmUNSPECIFIED_PIXMAP) &&
		 (!CBG_Submenu(old) && CBG_Submenu(new_w))) ||
	        ((CBG_ArmedPixmap(old) != XmUNSPECIFIED_PIXMAP) &&
		 (LabG_TextRect_height(old) != LabG_TextRect_height(new_w)))))
          {
	        _XmProcessLock();
                _XmArrowPixmapCacheDelete((XtPointer) CBG_ArmedPixmap(old));
                _XmArrowPixmapCacheDelete((XtPointer) CBG_CascadePixmap(old));
                CBG_ArmedPixmap(new_w) = XmUNSPECIFIED_PIXMAP;
                CBG_CascadePixmap(new_w) = XmUNSPECIFIED_PIXMAP;
                _XmCreateArrowPixmaps((Widget) new_w);
		_XmProcessUnlock();
                size_cascade (new_w);
          }

       /*
        * resize gadget if cascade appeared or disappeared, or if the
	* cascade pixmap changed size.
	*/
       if ((CBG_CascadePixmap (old) != CBG_CascadePixmap (new_w))  ||
	    (LabG_LabelType(old) != LabG_LabelType(new_w)) ||
	    (CBG_Submenu(old) != CBG_Submenu(new_w)))
       {
	  setup_cascade (new_w, adjustWidth, adjustHeight);

	  /* if there wasn't a cascade, and still isn't, don't redraw */
	  if (CBG_Submenu(old) || CBG_Submenu(new_w))
	      flag = TRUE;
       }

       /* make sure that other changes did not scrunch our pixmap */
       else if (CBG_Submenu(new_w))
       {
	  if ((new_w->gadget.highlight_thickness !=
	       old->gadget.highlight_thickness)                     ||
	      (new_w->gadget.shadow_thickness !=
	       old->gadget.shadow_thickness)                        ||
	      (LabG_MarginRight (new_w) != LabG_MarginRight (old))    ||
	      (LabG_MarginHeight (new_w) != LabG_MarginHeight (old))  ||
	      (LabG_MarginTop (new_w) != LabG_MarginTop (old))	    ||
	      (LabG_MarginBottom (new_w) != LabG_MarginBottom (old)))
	  {

             setup_cascade (new_w, adjustWidth, adjustHeight);
	     flag = TRUE;
	  }

	  else if ((LabG_MarginWidth(new_w) != LabG_MarginWidth(old)) ||
		   (new_w->rectangle.width != old->rectangle.width)   ||
		   (new_w->rectangle.height != old->rectangle.height))

	  {
	     position_cascade (new_w);
	     flag = TRUE;
	  }
       }
    }

    /* don't allow this to change */
    new_w->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT |
                             XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT |
                             XmENTER_EVENT | XmLEAVE_EVENT | XmHELP_EVENT |
			     XmBDRAG_EVENT ;

    return (flag);
}



/************************************************************************
 *
 *  GetArmGC
 *     Get the graphics context used for filling in background of the
 *     cascade button when armed.
 *
 ************************************************************************/

static void
GetArmGC(
        XmCascadeButtonGadget cb )
{
  XGCValues values;
  XtGCMask  valueMask;
  Pixel     junk, select_pixel;
  XmManagerWidget mw = (XmManagerWidget) XtParent(cb);

  XmGetColors(XtScreen(mw), mw->core.colormap, mw->core.background_pixel,
	      &junk, &junk, &junk, &select_pixel);

  valueMask = GCForeground | GCBackground | GCGraphicsExposures;

  values.foreground = select_pixel;
  values.background = select_pixel;
  values.graphics_exposures = False;

  CBG_ArmGC(cb) = XtGetGC ((Widget) mw, valueMask, &values);
}

/************************************************************************
 *
 *  GetBackgroundGC
 *     Get the graphics context used for filling in background of the
 *     cascade button when not armed.
 *
 ************************************************************************/

static void
GetBackgroundGC(
        XmCascadeButtonGadget cb )
{
  XGCValues values;
  XtGCMask  valueMask;
  XFontStruct  *fs;

  XmManagerWidget mw = (XmManagerWidget) XtParent(cb);

  valueMask = GCForeground | GCBackground | GCFont | GCGraphicsExposures;
  values.foreground = mw -> core.background_pixel;
  values.background = mw -> core.background_pixel;
  values.graphics_exposures = False;

  if (XmeRenderTableGetDefaultFont(LabG_Font(cb), &fs))
      values.font = fs->fid;
  else
      valueMask &= ~GCFont;

  CBG_BackgroundGC(cb) = XtGetGC( (Widget) mw, valueMask, &values);
}

/*
 * Initialize
 */
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
   XmCascadeButtonGadget req = (XmCascadeButtonGadget) rw ;
   XmCascadeButtonGadget new_w = (XmCascadeButtonGadget) nw ;
   Boolean adjustWidth = FALSE;
   Boolean adjustHeight = FALSE;
   XmMenuSystemTrait menuSTrait;
   XmRowColumnWidget    submenu = (XmRowColumnWidget) CBG_Submenu (new_w);

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(nw)), XmQTmenuSystem);

   if (! (LabG_MenuType(new_w) == XmMENU_BAR ||
	  LabG_MenuType(new_w) == XmMENU_PULLDOWN ||
	  LabG_MenuType(new_w) == XmMENU_POPUP    ||
	  LabG_MenuType(new_w) == XmMENU_OPTION))
   {
      XmeWarning( (Widget) new_w, WRONGPARENT);
   }

   /* Initialize GCs for armed button select and background only */
   GetArmGC (new_w);
   GetBackgroundGC (new_w);

   /* if menuProcs is not set up yet, try again */

   _XmProcessLock();
   if (xmLabelGadgetClassRec.label_class.menuProcs == NULL)
       xmLabelGadgetClassRec.label_class.menuProcs =
	   (XmMenuProc) _XmGetMenuProcContext();
   _XmProcessUnlock();

   CBG_ArmedPixmap(new_w) =  XmUNSPECIFIED_PIXMAP;

   /*
    * if the user did not specify a margin width, and we are
    * in a menuBar, set up the default.  First, find out what was the
    * request value (not in request since this is in the cached data)
    */
   if (LabG_MenuType(new_w) == XmMENU_BAR)
   {
      Dimension requestedMarginWidth;
      XtResource request_resources;

      request_resources.resource_name = XmNmarginWidth;
      request_resources.resource_class = XmCMarginWidth;
      request_resources.resource_type = XmRHorizontalDimension;
      request_resources.resource_size = sizeof (Dimension);
      request_resources.default_type = XmRImmediate;
      request_resources.resource_offset = 0;
      request_resources.default_addr = (XtPointer) XmINVALID_DIMENSION;

      XtGetSubresources(XtParent(new_w), &requestedMarginWidth,
			XtName((Widget)new_w),
			new_w->object.widget_class->core_class.class_name,
			&request_resources, 1, args, *num_args);

      if (requestedMarginWidth == XmINVALID_DIMENSION)
      {
	 LabG_MarginWidth(new_w) = 6;
      }
   }

   /* for other menu types, we may need to initialize the cascade pixmap */
   else
   if ((LabG_MenuType(new_w) != XmMENU_OPTION))
   {
      if (submenu && CBG_CascadePixmap(new_w) == XmUNSPECIFIED_PIXMAP) {
	 _XmProcessLock();
	 _XmCreateArrowPixmaps((Widget) new_w);
	 _XmProcessUnlock();
      }
   }

   /* Clear before setting  */
   CBG_Armed(new_w) = 0;
   CBG_SetArmed(new_w, FALSE);
   CBG_SetTraverse (new_w, FALSE);
   CBG_SetWasPosted (new_w, FALSE);
   CBG_Timer(new_w) = 0;

   if ((submenu) &&
       ((! XmIsRowColumn(submenu)) ||
	(RC_Type(submenu) != XmMENU_PULLDOWN)))
   {
      submenu = NULL;
      XmeWarning( (Widget) new_w, WRONGSUBMENU);
   }

   if (CBG_MapDelay(new_w) < 0)
   {
      CBG_MapDelay(new_w) = MAP_DELAY_DEFAULT;
      XmeWarning( (Widget) new_w, WRONGMAPDELAY);
   }

   /* call submenu's class function to set the link  */
   if (submenu != NULL && menuSTrait != NULL)
     menuSTrait -> recordPostFromWidget(CBG_Submenu(new_w), (Widget) new_w, TRUE);

   if (LabG_MenuType(new_w) == XmMENU_PULLDOWN	||
       LabG_MenuType(new_w) == XmMENU_POPUP ||
       LabG_MenuType(new_w) == XmMENU_OPTION)
   {
      if (req->rectangle.width <= 0)
	  adjustWidth = TRUE;

      if (req->rectangle.height <= 0)
	  adjustHeight = TRUE;

      /* get pixmap size and set up gadget to allow room for it */
      size_cascade (new_w);
      setup_cascade (new_w, adjustWidth, adjustHeight);
   }

   if (LabG_MenuType(new_w) == XmMENU_BAR ||
       LabG_MenuType(new_w) == XmMENU_PULLDOWN	||
       LabG_MenuType(new_w) == XmMENU_POPUP)
   {
      new_w->gadget.traversal_on = TRUE;
   }

   /*
    * initialize the input types
    */
   new_w->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT |
       XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT |
	   XmENTER_EVENT | XmLEAVE_EVENT | XmHELP_EVENT;

}


/*
 *************************************************************************
 *
 * Public Routines
 *
 *************************************************************************
 */
Widget
XmCreateCascadeButtonGadget(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    Widget cb;

    cb = XtCreateWidget(name, xmCascadeButtonGadgetClass, parent, al, ac);

    return (cb);
}

Widget
XmVaCreateCascadeButtonGadget(
        Widget parent,
        char *name,
        ...)
{
    Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmCascadeButtonGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedCascadeButtonGadget(
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
                         xmCascadeButtonGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/*
 * Arm or disarm the gadget.  This routine does not pop up or down submenus
 */
void
XmCascadeButtonGadgetHighlight(
        Widget wid,
        Boolean highlight )
{
   XmCascadeButtonGadget cb  = (XmCascadeButtonGadget) wid;
   _XmWidgetToAppContext(wid);

   _XmAppLock(app);
   if ((cb) && XmIsCascadeButtonGadget(cb))
   {
      if (highlight)
         Arm (cb);

      else
         Disarm (cb, FALSE);
   }
   _XmAppUnlock(app);
}

/****************************************************
 *   Functions for manipulating Secondary Resources.
 *********************************************************/
/*
 * GetCascadeBGSecResData()
 *    Create a XmSecondaryResourceDataRec for each secondary resource;
 *    Put the pointers to these records in an array of pointers;
 *    Return the pointer to the array of pointers.
 */
static Cardinal
GetCascadeBGClassSecResData(
        WidgetClass w_class,
        XmSecondaryResourceData **data_rtn )
{   int arrayCount /* = 0 */;
    XmBaseClassExt  bcePtr;
    String  resource_class, resource_name;
    XtPointer  client_data;

    _XmProcessLock();
    bcePtr = &(CascadeBGClassExtensionRec );
    client_data = NULL;
    resource_class = NULL;
    resource_name = NULL;
    arrayCount =
      _XmSecondaryResourceData ( bcePtr, data_rtn, client_data,
                                resource_name, resource_class,
                                GetCascadeBGClassSecResBase) ;
    _XmProcessUnlock();
    return (arrayCount);
}

/*
 * GetCascadeBGClassResBase ()
 *   retrun the address of the base of resources.
 */
static XtPointer
GetCascadeBGClassSecResBase(
        Widget widget,
        XtPointer client_data )	/* unused */
{       XtPointer  widgetSecdataPtr;
    XtPointer ret_val;
    size_t labg_cache_size = sizeof (XmLabelGCacheObjPart);
    size_t cascadebg_cache_size = sizeof (XmCascadeButtonGCacheObjPart);
        char *cp;

    _XmProcessLock();
    widgetSecdataPtr = (XtPointer)
                      (XtMalloc ( labg_cache_size + cascadebg_cache_size + 1));

    if (widgetSecdataPtr)
          { cp = (char *) widgetSecdataPtr;
            memcpy( cp, LabG_Cache(widget), labg_cache_size);
            cp += labg_cache_size;
            memcpy( cp, CBG_Cache(widget), cascadebg_cache_size);
          }
/* else Warning: error cannot allocate Memory */
/*     widgetSecdataPtr = (XtPointer) ( LabG_Cache(widget)); */

	ret_val = widgetSecdataPtr;

	_XmProcessUnlock();
        return (widgetSecdataPtr);
}
