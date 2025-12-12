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
static char rcsid[] = "$TOG: PushBG.c /main/29 1999/05/27 14:18:33 mgreess $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/ActivatableT.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/PushBGP.h>
#include <Xm/RowColumnP.h>
#include <Xm/ScreenP.h>
#include <Xm/TakesDefT.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "ColorI.h"
#include "ExtObjectI.h"
#include "GadgetUtiI.h"
#include "LabelI.h"
#include "LabelGI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "SyntheticI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"

#define DELAY_DEFAULT		100
#define XmINVALID_MULTICLICK	255


/********    Static Function Declarations    ********/

static int _XmPushBCacheCompare(
                        XtPointer A,
                        XtPointer B);
static void InputDispatch(
                        Widget wid,
                        XEvent *event,
                        Mask event_mask);
static void Arm(
                        XmPushButtonGadget pb,
                        XEvent *event);
static void Activate(
                        XmPushButtonGadget pb,
                        XEvent *event);
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ArmTimeout(
                        XtPointer data,
                        XtIntervalId *id);
static void Disarm(
                        XmPushButtonGadget pb,
                        XEvent *event);
static void BtnDown(
                        XmPushButtonGadget pb,
                        XEvent *event);
static void BtnUp(
                        Widget wid,
                        XEvent *event);
static void Enter(
                        Widget wid,
                        XEvent *event);
static void Leave(
                        Widget wid,
                        XEvent *event);
static void BorderHighlight(
                        Widget wid);
static void DrawBorderHighlight(
                        Widget wid);
static void BorderUnhighlight(
                        Widget wid);
static void KeySelect(
                        Widget wid,
                        XEvent *event);
static void ClassInitialize( void );
static void ClassPartInitialize(
                        WidgetClass wc);
static void SecondaryObjectCreate(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void InitializePrehook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void InitializePosthook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void GetFillGC(
                        XmPushButtonGadget pb);
static Boolean SetValuesPrehook(
                        Widget oldParent,
                        Widget refParent,
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesPrehook(
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesPosthook(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesPosthook(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void Help(
                        XmPushButtonGadget pb,
                        XEvent *event);
static void Destroy(
                        Widget wid);
static void Resize(
                        Widget wid);
static void ActivateCommonG(
                        XmPushButtonGadget pb,
                        XEvent *event,
                        Mask event_mask);
static Cardinal GetPushBGClassSecResData(
                        WidgetClass w_class,
                        XmSecondaryResourceData **data_rtn);
static XtPointer GetPushBGClassSecResBase(
                        Widget widget,
                        XtPointer client_data);
static void EraseDefaultButtonShadow(
                        XmPushButtonGadget pb);
static void DrawDefaultButtonShadow(
                        XmPushButtonGadget pb);
static int AdjustHighLightThickness(
                        XmPushButtonGadget new_w,
                        XmPushButtonGadget current);
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region);
static void DrawPushButtonLabelGadget(
                        XmPushButtonGadget pb,
                        XEvent *event,
                        Region region);
static void DrawLabelGadget(
                        XmPushButtonGadget pb,
                        XEvent *event,
                        Region region) ;
static void DrawPushButtonGadgetShadows(
                        XmPushButtonGadget pb);
static void DrawPBGadgetShadows(
                        XmPushButtonGadget pb);
static void EraseDefaultButtonShadows(
                        XmPushButtonGadget pb);
static void DrawDefaultButtonShadows(
                        XmPushButtonGadget pb);
static XmImportOperator ShowAsDef_ToHorizPix(
                        Widget widget,
                        int offset,
                        XtArgVal *value);
static Boolean ComputePBLabelArea(
                        XmPushButtonGadget pb,
                        LRectangle *box);
static void ExportHighlightThickness(
                        Widget widget,
                        int offset,
                        XtArgVal *value);
static void SetPushButtonSize(
                        XmPushButtonGadget newpb);
static void ChangeCB(Widget w,
		     XtCallbackProc activCB,
		     XtPointer closure,
		     Boolean setunset);
static void ShowAsDefault(Widget w,
			  XtEnum state);
static void PBG_FixTearoff( XmPushButtonGadget pb);

/********    End Static Function Declarations    ********/

/************************************************
 The uncached resources for Push Button
 ************************************************/

static XtResource resources[] =
{
   {
     XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf (struct _XmPushButtonGadgetRec, pushbutton.activate_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf (struct _XmPushButtonGadgetRec, pushbutton.arm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNdisarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf (struct _XmPushButtonGadgetRec, pushbutton.disarm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof(Dimension),
     XtOffsetOf (struct _XmPushButtonGadgetRec, gadget.shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof (Boolean),
     XtOffsetOf (struct _XmGadgetRec, gadget.traversal_on),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf (struct _XmGadgetRec, gadget.highlight_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNshowAsDefault, XmCShowAsDefault, XmRBooleanDimension,
     sizeof (Dimension),
     XtOffsetOf (struct _XmPushButtonGadgetRec, pushbutton.show_as_default),
     XmRImmediate, (XtPointer) 0
   },
};

static XmSyntheticResource syn_resources[] =
{
   {
     XmNshowAsDefault,
     sizeof (Dimension),
     XtOffsetOf (struct _XmPushButtonGadgetRec, pushbutton.show_as_default),
     XmeFromHorizontalPixels,
     ShowAsDef_ToHorizPix
   },
   {
     XmNhighlightThickness,
     sizeof (Dimension),
     XtOffsetOf (struct _XmGadgetRec, gadget.highlight_thickness),
     ExportHighlightThickness,
     XmeToHorizontalPixels
   },

};

/**********************************************
 Cached resources for PushButton Gadget
 **********************************************/

static XtResource cache_resources[] =
{
   {
     XmNmultiClick, XmCMultiClick, XmRMultiClick,
     sizeof (unsigned char),
     XtOffsetOf (struct _XmPushButtonGCacheObjRec, pushbutton_cache.multiClick),
     XmRImmediate, (XtPointer) XmINVALID_MULTICLICK
   },

   {
     XmNdefaultButtonShadowThickness, XmCDefaultButtonShadowThickness,
     XmRHorizontalDimension, sizeof (Dimension),
     XtOffsetOf(struct _XmPushButtonGCacheObjRec,
		pushbutton_cache.default_button_shadow_thickness),
     XmRImmediate, (XtPointer) 0
   },

   {
     XmNfillOnArm, XmCFillOnArm, XmRBoolean, sizeof (Boolean),
     XtOffsetOf(struct _XmPushButtonGCacheObjRec, pushbutton_cache.fill_on_arm),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNarmColor, XmCArmColor, XmRPixel, sizeof (Pixel),
     XtOffsetOf(struct _XmPushButtonGCacheObjRec, pushbutton_cache.arm_color),
     XmRImmediate, (XtPointer) INVALID_PIXEL
   },

   {
     XmNarmPixmap, XmCArmPixmap, XmRDynamicPixmap, sizeof (Pixmap),
     XtOffsetOf(struct _XmPushButtonGCacheObjRec, pushbutton_cache.arm_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },
};

/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource cache_syn_resources[] =
{
   {
     XmNdefaultButtonShadowThickness, sizeof (Dimension),
     XtOffsetOf(struct _XmPushButtonGCacheObjRec,
		pushbutton_cache.default_button_shadow_thickness),
     XmeFromHorizontalPixels,
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

static XmCacheClassPart PushButtonClassCachePart = {
    {NULL, 0, 0},            /* head of class cache list */
    _XmCacheCopy,            /* Copy routine     */
    _XmCacheDelete,          /* Delete routine   */
    _XmPushBCacheCompare,    /* Comparison routine   */
};

static XmBaseClassExtRec   PushBGClassExtensionRec = {
    NULL,   		 			/* next_extension        */
    NULLQUARK,    				/* record_typ            */
    XmBaseClassExtVersion,			/* version               */
    sizeof(XmBaseClassExtRec), 			/* record_size           */
    InitializePrehook,         			/* initializePrehook     */
    SetValuesPrehook,   			/* setValuesPrehook      */
    InitializePosthook,				/* initializePosthook    */
    SetValuesPosthook, 				/* setValuesPosthook     */
    (WidgetClass)&xmPushButtonGCacheObjClassRec,/* secondaryObjectClass  */
    SecondaryObjectCreate, 		        /* secondaryObjectCreate */
    GetPushBGClassSecResData,                   /* getSecResData         */
    {0},           		                /* fast subclass         */
    GetValuesPrehook,   			/* getValuesPrehook      */
    GetValuesPosthook,   			/* getValuesPosthook     */
    (XtWidgetClassProc)NULL,                    /* classPartInitPrehook  */
    (XtWidgetClassProc)NULL,                    /* classPartInitPosthook */
    (XtResourceList)NULL,                       /* ext_resources         */
    (XtResourceList)NULL,                       /* compiled_ext_resources*/
    0,                                          /* num_ext_resources     */
    FALSE,                                      /* use_sub_resources     */
    XmInheritWidgetNavigable,                   /* widgetNavigable       */
    XmInheritFocusChange,                       /* focusChange           */
};

/* ext rec static initialization */
externaldef(xmpushbuttongcacheobjclassrec)
XmPushButtonGCacheObjClassRec xmPushButtonGCacheObjClassRec =
{
  {
      /* superclass         */    (WidgetClass) &xmLabelGCacheObjClassRec,
      /* class_name         */    "XmPushButtonGadget",
      /* widget_size        */    sizeof(XmPushButtonGCacheObjRec),
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

/*  The PushButton class record definition  */

static XmGadgetClassExtRec _XmPushBGadClassExtRec = {
    NULL,
    NULLQUARK,
    XmGadgetClassExtVersion,
    sizeof(XmGadgetClassExtRec),
    XmInheritBaselineProc,                  /* widget_baseline */
    XmInheritDisplayRectProc,               /* widget_display_rect */
    XmInheritMarginsProc,                   /* widget_margins */
};

externaldef(xmpushbuttongadgetclassrec)
	XmPushButtonGadgetClassRec xmPushButtonGadgetClassRec =
{
  {
      (WidgetClass) &xmLabelGadgetClassRec,  	/* superclass            */
      "XmPushButtonGadget",             	/* class_name	         */
      sizeof(XmPushButtonGadgetRec),    	/* widget_size	         */
      ClassInitialize,         			/* class_initialize      */
      ClassPartInitialize,              	/* class_part_initialize */
      FALSE,                            	/* class_inited          */
      Initialize,          	                /* initialize	         */
      (XtArgsProc)NULL,                         /* initialize_hook       */
      NULL,                     		/* realize	         */
      NULL,                             	/* actions               */
      0,			        	/* num_actions    	 */
      resources,                        	/* resources	         */
      XtNumber(resources),              	/* num_resources         */
      NULLQUARK,                        	/* xrm_class	         */
      TRUE,                             	/* compress_motion       */
      XtExposeCompressMaximal,          	/* compress_exposure     */
      TRUE,                             	/* compress_enterleave   */
      FALSE,                            	/* visible_interest      */
      Destroy,           	                /* destroy               */
      Resize,	                		/* resize                */
      Redisplay,        		        /* expose                */
      SetValues,      	                        /* set_values	         */
      (XtArgsFunc)NULL,                       	/* set_values_hook       */
      XtInheritSetValuesAlmost,         	/* set_values_almost     */
      (XtArgsProc)NULL,                        	/* get_values_hook       */
      NULL,                 			/* accept_focus	         */
      XtVersion,                        	/* version               */
      NULL,                             	/* callback private      */
      NULL,                             	/* tm_table              */
      XtInheritQueryGeometry,           	/* query_geometry        */
      NULL,					/* display_accelerator   */
      (XtPointer)&PushBGClassExtensionRec,	/* extension             */
   },

   {	/* gadget class record */
      BorderHighlight,			  /* border highlight   */
      BorderUnhighlight,		  /* border_unhighlight */
      ArmAndActivate,			  /* arm_and_activate   */
      InputDispatch,			  /* input dispatch     */
      XmInheritVisualChange,		  /* visual_change      */
      syn_resources,   			  /* syn resources      */
      XtNumber(syn_resources),  	  /* num syn_resources  */
      &PushButtonClassCachePart,	  /* class cache part   */
      (XtPointer)&_XmPushBGadClassExtRec, /* extension          */
   },

   { 	/* label_class record */
      XmInheritWidgetProc,	/* setOverrideCallback */
      XmInheritMenuProc,	/* menu proc's entry   */
      NULL,			/* extension	       */
   },

   {	/* pushbutton class record */
      NULL,			/* extension 	 */
   }
};

externaldef(xmpushbuttongadgetclass) WidgetClass xmPushButtonGadgetClass =
   (WidgetClass) &xmPushButtonGadgetClassRec;


/* Trait record for pushButtonG */

static XmConst XmActivatableTraitRec pushButtonGAT = {
  0,		/* version */
  ChangeCB,
};

/* TakesDefault Trait record for pushButtonG */

static XmConst XmTakesDefaultTraitRec pushButtonGTDT = {
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

/*****************************************************************
 *  SPECIAL PROPERTIES OF PUSHBUTTON GADGET INSIDE A MENU:
 *   When a PushButton (widget/gadget) is incorporated in a Menu
 *   (Pulldownor Popup) - its properties get modified in these ways:
 *   (1) The redisplay routine should not draw its background nor draw
 *	     its shadows.  It should draw only the label. To comply with
 *	     this means that the arm-color and background color are not
 *	     of any use. As a result the values in the FillGC and BackgroundGC
 *	     are not initialized and are likely to be bogus. This causes
 *	     special casing of Initialize and SetValues routines.
 *   (2) PushButton does not show its depressed appearance in the
 *       menu. This will cause Arm(), DisArm(), ArmAndActivate routines
 *	     to have special cases.
 *   In short the properties of Pushbutton in a menu are so different
 *	  that practically all major routines in this widget will have to
 *	  special-cased to accommodate this difference as if two different
 *	  classes are being glued to one class.
 *******************************************************************/

/*******************************************************************
 *
 *  _XmPushBCacheCompare
 *
 *******************************************************************/

static int
_XmPushBCacheCompare(
        XtPointer A,
        XtPointer B )
{
  XmPushButtonGCacheObjPart *a_inst = (XmPushButtonGCacheObjPart *) A;
  XmPushButtonGCacheObjPart *b_inst = (XmPushButtonGCacheObjPart *) B;

  if ((a_inst->fill_on_arm	== b_inst->fill_on_arm) &&
      (a_inst->arm_color	== b_inst->arm_color) &&
      (a_inst->arm_pixmap	== b_inst->arm_pixmap) &&
      (a_inst->unarm_pixmap	== b_inst->unarm_pixmap) &&
      (a_inst->fill_gc		== b_inst->fill_gc) &&
      (a_inst->background_gc	== b_inst->background_gc) &&
      (a_inst->multiClick	== b_inst->multiClick) &&
      (a_inst->default_button_shadow_thickness
       				== b_inst->default_button_shadow_thickness) &&
      (a_inst->timer		== b_inst->timer) &&
      (a_inst->timer_widget	== b_inst->timer_widget)
     )
    return 1;
  else
    return 0;
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
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;

  if ((event_mask & XmARM_EVENT) ||
      ((PBG_MultiClick(pb) == XmMULTICLICK_KEEP) &&
       (event_mask & XmMULTI_ARM_EVENT)))
    {
      if (LabG_IsMenupane(pb))
	BtnDown (pb, event);
      else
	Arm (pb, event);
    }

  else if (event_mask & XmACTIVATE_EVENT)
    {
      PBG_ClickCount (pb) = 1;
      /* pb->pushbutton.click_count = 1; */
      ActivateCommonG (pb, event, event_mask);
    }

  else if (event_mask & XmMULTI_ACTIVATE_EVENT)
    {
      /* If XmNMultiClick resource is set to DISCARD - do nothing
       * else increment clickCount and call ActivateCommonG.
       */
      if (PBG_MultiClick(pb) == XmMULTICLICK_KEEP)
	{
	  (PBG_ClickCount (pb))++;
	  ActivateCommonG (pb, event, event_mask);
	}
    }

  else if (event_mask & XmHELP_EVENT)
    Help (pb, event);

  else if (event_mask & XmENTER_EVENT)
    Enter ((Widget) pb, event);

  else if (event_mask & XmLEAVE_EVENT)
    Leave ((Widget) pb, event);

  else if (event_mask & XmFOCUS_IN_EVENT)
    _XmFocusInGadget((Widget) pb, event, NULL, NULL);

  else if (event_mask & XmFOCUS_OUT_EVENT)
    _XmFocusOutGadget((Widget) pb, event, NULL, NULL);

  else if (event_mask & XmBDRAG_EVENT)
    _XmProcessDrag ((Widget) pb, event, NULL, NULL);
}

/************************************************************************
 *
 *     Arm
 *
 *     This function processes button 1 down occuring on the pushbutton.
 *     Mark the pushbutton as armed (i.e. active).
 *     The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/

static void
Arm(
        XmPushButtonGadget pb,
        XEvent *event )
{
  XmPushButtonCallbackStruct call_value;
  XtExposeProc    expose;

  PBG_Armed(pb) = TRUE;

  _XmProcessLock();
  expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
      rect_class.expose;
  _XmProcessUnlock();

  (* (expose)) ((Widget) pb, event, (Region) NULL);

  if (PBG_ArmCallback(pb))
    {
      XFlush(XtDisplay (pb));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, PBG_ArmCallback(pb), &call_value);
    }
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
        XmPushButtonGadget pb,
        XEvent *event )
{
  XmPushButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;
  XtExposeProc    expose;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);

  PBG_Armed(pb) = FALSE;

   _XmProcessLock();
   expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
       rect_class.expose;
   _XmProcessUnlock();
  (* (expose)) ((Widget) pb, event, (Region) NULL);

  /* CR 9181: Consider clipping when testing visibility. */
  if ((event->xany.type == ButtonPress || event->xany.type == ButtonRelease) &&
      _XmGetPointVisibility((Widget)pb,
			    event->xbutton.x_root, event->xbutton.y_root))
    {
      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;
      call_value.click_count = PBG_ClickCount(pb);

      /* _XmRecordEvent(event); */       /* Fix CR 3407 DRand 6/16/92 */

      /* If the parent is menu system able, notify it about the select. */
      if (menuSTrait != NULL)
	menuSTrait->entryCallback (XtParent(pb), (Widget) pb, &call_value);

      if ((! LabG_SkipCallback(pb)) &&
	  (PBG_ActivateCallback(pb)))
	{
	  XFlush (XtDisplay (pb));
	  XtCallCallbackList ((Widget) pb, PBG_ActivateCallback(pb),
			      &call_value);
	}
    }
}

static void
PBG_FixTearoff( XmPushButtonGadget pb)
{
	 if  (XmMENU_PULLDOWN == LabG_MenuType(pb))
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
 *  - Called if the PushButtonGadget is being selected via keyboard
 *    i.e. by pressing <return> or <space-bar>.
 *  - Called by SelectionBox and FileSelectionBox code when they receive
 *    a default-action callback on their embedded XmList widgets.
 ************************************************************************/

/*ARGSUSED*/
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  Boolean already_armed = PBG_Armed(pb);
  Boolean is_menupane = LabG_IsMenupane(pb);
  Boolean torn_has_focus = FALSE;    /* must be torn! */
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

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

  if (is_menupane && menuSTrait != NULL)
    {
      PBG_Armed(pb) = FALSE;

      /* CR 7799: Torn off menus shouldn't be shared, so don't reparent. */
      if (torn_has_focus)
	menuSTrait->popdown(XtParent(pb), event);
      else
	menuSTrait->buttonPopdown(XtParent(pb), event);

      /* If its in a torn off menu pane, show depressed button briefly. */
      if (torn_has_focus)
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
	  Boolean etched_in = dpy->display.enable_etched_in_menu;

	  /* Set the focus here. */
	  XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);

	  if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
	      (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
	    XmeDrawShadows
	      (XtDisplay (pb), XtWindow (pb),
	       LabG_BottomShadowGC(pb), LabG_TopShadowGC(pb),
	       pb->rectangle.x + pb->gadget.highlight_thickness,
	       pb->rectangle.y + pb->gadget.highlight_thickness,
	       pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
	       pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
	       pb->gadget.shadow_thickness,
	       etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
	}
    }
  else
    {
      XtExposeProc    expose;
      /* We have no idea what the event is, so don't pass it through,
       * in case some future subclass is smarter about reexposure.
       */
      PBG_Armed(pb) = TRUE;
      _XmProcessLock();
      expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
		rect_class.expose;
      _XmProcessUnlock();
      (* (expose)) ((Widget) pb, (XEvent *)NULL, (Region) NULL);
    }

  XFlush (XtDisplay (pb));

  if (event)
    {
      if (event->type == KeyPress)
	PBG_ClickCount (pb) = 1;
    }

  /* If the parent is menu system able, set the lastSelectToplevel before
   * the arm. It's ok if this is recalled later.
   */
  if (menuSTrait != NULL)
    menuSTrait->getLastSelectToplevel (XtParent(pb));

  if (PBG_ArmCallback(pb) && !already_armed)
    {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      call_value.click_count = PBG_ClickCount (pb);
      XtCallCallbackList ((Widget) pb, PBG_ArmCallback(pb), &call_value);
    }

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = PBG_ClickCount (pb);

  /* If the parent is menu system able, notify it about the select */
  if (menuSTrait != NULL)
    menuSTrait->entryCallback (XtParent(pb), (Widget) pb, &call_value);


  LabG_Pixmap(pb) = PBG_UnarmPixmap(pb);	/*Added for fix bug 1375*/
  if ((! LabG_SkipCallback(pb)) && (PBG_ActivateCallback(pb)))
    {
      XFlush (XtDisplay (pb));
      XtCallCallbackList ((Widget) pb, PBG_ActivateCallback(pb), &call_value);
    }

  PBG_Armed(pb) = FALSE;

  if (PBG_DisarmCallback(pb))
    {
      XFlush (XtDisplay (pb));
      call_value.reason = XmCR_DISARM;
      XtCallCallbackList ((Widget) pb, PBG_DisarmCallback(pb), &call_value);
    }

  if (is_menupane)
    {
      if (torn_has_focus && XtIsSensitive(wid))
	{
	  /* Leave the focus widget in an armed state */
	  PBG_Armed(pb) = TRUE;

	  if (PBG_ArmCallback(pb))
	    {
	      XFlush (XtDisplay (pb));
	      call_value.reason = XmCR_ARM;
	      XtCallCallbackList((Widget) pb, PBG_ArmCallback(pb), &call_value);
	    }
	}
      else if (menuSTrait != NULL)
	{
	menuSTrait->reparentToTearOffShell(XtParent(pb), event);
	PBG_FixTearoff(pb);
	}
    }

  /*
   * If the button is still around, show it released, after a short delay.
   * This is done if the button is outside of a menus, or if in a torn
   * off menupane.
   */

  if (!is_menupane || torn_has_focus)
    {
      if ((pb->object.being_destroyed == False) && (!(PBG_Timer(pb))))
	{
	PBG_Timer(pb) =
	  XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)pb),
			  (unsigned long) DELAY_DEFAULT, ArmTimeout,
			  (XtPointer)(pb));
	((XmPushButtonGadget)(pb))->pushbutton.cache->timer_widget = wid;
	}
    }
}

/*ARGSUSED*/
static void
ArmTimeout(
        XtPointer data,
        XtIntervalId *id )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) data;

  PBG_Timer(pb) = 0;
  if (XtIsRealized ((Widget)pb) && XtIsManaged ((Widget)pb))
    {
      if (LabG_IsMenupane(pb))
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
	      if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
		  (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
		XmeDrawShadows
		  (XtDisplay (pb), XtWindow (pb),
		   LabG_TopShadowGC(pb),
		   LabG_BottomShadowGC(pb),
		   pb->rectangle.x + pb->gadget.highlight_thickness,
		   pb->rectangle.y + pb->gadget.highlight_thickness,
		   pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		   pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		   pb->gadget.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
	    }
	}
      else
	{
	  XtExposeProc    expose;
	  _XmProcessLock();
	  expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
		    rect_class.expose;
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

static void
Disarm(
        XmPushButtonGadget pb,
        XEvent *event )
{
  XmPushButtonCallbackStruct call_value;

  PBG_Armed(pb) = FALSE;

  if (PBG_DisarmCallback(pb))
    {
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, PBG_DisarmCallback(pb), &call_value);
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

static void
BtnDown(
        XmPushButtonGadget pb,
        XEvent *event )
{
  XmPushButtonCallbackStruct call_value;
  Boolean already_armed;
  ShellWidget popup;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);

  /* Popdown other popus that may be up */
  if (!(popup = (ShellWidget)_XmGetRC_PopupPosted(XtParent(pb))))
    {
      if (!XmIsMenuShell(XtParent(XtParent(pb))) && menuSTrait != NULL)
	{
	  /* In case tear off not armed and no grabs in place, do it now.
	   * Ok if already armed and grabbed - nothing done.
	   */
	  menuSTrait->tearOffArm(XtParent(pb));
	}
    }

  if (popup)
    {
      if (popup->shell.popped_up && menuSTrait != NULL)
	menuSTrait->popdownEveryone((Widget) popup, event);
    }

  /* Set focus to this button.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);
  /* get the location cursor - get consistent with Gadgets */

  already_armed = PBG_Armed(pb);
  PBG_Armed(pb) = TRUE;

   if (etched_in) {
       Redisplay((Widget) pb, NULL, NULL);
   }
   else
       if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
	   (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
	   XmeDrawShadows (
		    XtDisplay (pb), XtWindow (pb),
		    LabG_TopShadowGC(pb),
		    LabG_BottomShadowGC(pb),
		    pb->rectangle.x + pb->gadget.highlight_thickness,
		    pb->rectangle.y + pb->gadget.highlight_thickness,
		    pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		    pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		    pb->gadget.shadow_thickness, XmSHADOW_OUT);

  if (PBG_ArmCallback(pb) && !already_armed)
    {
      XFlush (XtDisplay (pb));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, PBG_ArmCallback(pb), &call_value);
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

static void
BtnUp(
        Widget wid,
        XEvent *event )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  Boolean flushDone = False;
  Boolean popped_up = False;
  Boolean is_menupane = LabG_IsMenupane(pb);
  Widget shell = XtParent(XtParent(pb));
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

  PBG_Armed(pb) = FALSE;

  if (menuSTrait != NULL)
    {
      if (is_menupane && !XmIsMenuShell(shell))
	popped_up = menuSTrait->popdown((Widget) pb, event);
      else
	popped_up = menuSTrait->buttonPopdown((Widget) pb, event);
    }

  _XmRecordEvent(event);

  if (popped_up)
    return;

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = 1;

  /* if the parent is menu system able, notify it about the select */
  if (menuSTrait != NULL)
    {
      menuSTrait->entryCallback(XtParent(pb), (Widget) pb, &call_value);
      flushDone = True;
    }

  if ((! LabG_SkipCallback(pb)) &&
      (PBG_ActivateCallback(pb)))
    {
      XFlush (XtDisplay (pb));
      flushDone = True;
      XtCallCallbackList ((Widget) pb, PBG_ActivateCallback(pb), &call_value);
    }

  if (PBG_DisarmCallback(pb))
    {
      if (!flushDone)
	XFlush (XtDisplay (pb));
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) pb, PBG_DisarmCallback(pb), &call_value);
    }

  /* If the original shell does not indicate an active menu, but rather a
   * tear off pane, leave the button in an armed state.  Also, briefly
   * display the button as depressed to give the user some feedback of
   * the selection.
   */
  if (is_menupane)
    {
      if (!XmIsMenuShell(shell))
	{
	  if (XtIsSensitive((Widget)pb))
	    {
	      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
	      Boolean etched_in = dpy->display.enable_etched_in_menu;

	      if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
		  (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
		XmeDrawShadows
		  (XtDisplay (pb), XtWindow (pb),
		   LabG_BottomShadowGC(pb),
		   LabG_TopShadowGC(pb),
		   pb->rectangle.x + pb->gadget.highlight_thickness,
		   pb->rectangle.y + pb->gadget.highlight_thickness,
		   pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		   pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		   pb->gadget.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	      XFlush (XtDisplay (pb));

	      /* set timer to redraw the shadow out again */
	      if (pb->object.being_destroyed == False)
		{
		  if (!(PBG_Timer(pb)))
		   {
		    PBG_Timer(pb) =
		      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)pb),
				      (unsigned long) DELAY_DEFAULT,
				      ArmTimeout, (XtPointer)(pb));
			((XmPushButtonGadget)(pb))->pushbutton.cache->timer_widget = wid;
		   }
		}

	      PBG_Armed(pb) = TRUE;
	      if (PBG_ArmCallback(pb))
		{
		  call_value.reason = XmCR_ARM;
		  call_value.event = event;
		  XtCallCallbackList((Widget)pb, PBG_ArmCallback(pb),
				     &call_value);
		}
	    }
	}
      else if (menuSTrait != NULL)
	menuSTrait->reparentToTearOffShell(XtParent(pb), event);
    }

  _XmSetInDragMode((Widget) pb, False);

  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  if (!XmIsMenuShell(shell))
    XmProcessTraversal((Widget) pb, XmTRAVERSE_CURRENT);
  PBG_FixTearoff(pb);
}

/************************************************************************
 *
 *  Enter
 *
 ************************************************************************/

static void
Enter(
        Widget wid,
        XEvent *event )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if (LabG_IsMenupane(pb))
    {
      if ((((ShellWidget) XtParent(XtParent(pb)))->shell.popped_up) &&
          _XmGetInDragMode((Widget) pb))
	{
          if (PBG_Armed(pb))
	    return;

	  /* So KHelp event is delivered correctly */
	  _XmSetFocusFlag(XtParent(XtParent(pb)), XmFOCUS_IGNORE, TRUE);
	  XtSetKeyboardFocus(XtParent(XtParent(pb)), (Widget)pb);
	  _XmSetFocusFlag(XtParent(XtParent(pb)), XmFOCUS_IGNORE, FALSE);

	  PBG_Armed(pb) = TRUE;

	  ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	  if (etched_in) {
	      Redisplay((Widget) pb, NULL, NULL);
	  }
	  else
	      if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
		  (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
		 XmeDrawShadows
		    (XtDisplay (pb), XtWindow (pb),
		     LabG_TopShadowGC(pb),
		     LabG_BottomShadowGC(pb),
		     pb->rectangle.x + pb->gadget.highlight_thickness,
		     pb->rectangle.y + pb->gadget.highlight_thickness,
		     pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		     pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		     pb->gadget.shadow_thickness,
		     etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	  if (PBG_ArmCallback(pb))
	    {
	      XFlush (XtDisplay (pb));

	      call_value.reason = XmCR_ARM;
	      call_value.event = event;
	      XtCallCallbackList((Widget) pb, PBG_ArmCallback(pb), &call_value);
	    }
	}
    }
  else
    {
      XtExposeProc expose;
      _XmEnterGadget((Widget) pb, event, NULL, NULL);
      if (PBG_Armed(pb) == TRUE) {
	_XmProcessLock();
	expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
	    rect_class.expose;
	_XmProcessUnlock();
	(* (expose)) ((Widget) pb, event, (Region) NULL);
       }
    }
}

/************************************************************************
 *
 *  Leave
 *
 ************************************************************************/
static void
Leave(
        Widget wid,
        XEvent *event )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;

  if (LabG_IsMenupane(pb))
    {
      if (_XmGetInDragMode((Widget) pb) && PBG_Armed(pb))
	{
	   XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	   Boolean etched_in = dpy->display.enable_etched_in_menu;

	  PBG_Armed(pb) = FALSE;

	  ((XmManagerWidget) XtParent(wid))->manager.active_child = NULL;

	  if (etched_in) {
	      Redisplay((Widget) pb, NULL, NULL);
	  }

	  XmeDrawHighlight
	    (XtDisplay(pb), XtWindow(pb),
	     LabG_BackgroundGC(pb),
	     pb->rectangle.x + pb->gadget.highlight_thickness,
	     pb->rectangle.y + pb->gadget.highlight_thickness,
	     pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
	     pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
	     pb->gadget.shadow_thickness);

	  if (PBG_DisarmCallback(pb))
	    {
	      XFlush (XtDisplay (pb));

	      call_value.reason = XmCR_DISARM;
	      call_value.event = event;
	      XtCallCallbackList ((Widget) pb, PBG_DisarmCallback(pb),
				  &call_value);
	    }
	}
    }
  else
    {
      _XmLeaveGadget((Widget) pb, event, NULL, NULL);

      if (PBG_Armed(pb) == TRUE)
	{
	  XtExposeProc expose;
	  PBG_Armed(pb) = FALSE;
	  _XmProcessLock();
	  expose = ((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
	      rect_class.expose;
	  _XmProcessUnlock();
	  (* (expose)) ((Widget) pb, event, (Region) NULL);
	  PBG_Armed(pb) = TRUE;
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
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  XEvent * event = NULL;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  Boolean etched_in = dpy->display.enable_etched_in_menu;
  Boolean already_armed = PBG_Armed(pb);

  if (LabG_IsMenupane(pb))
    {
      PBG_Armed(pb) = TRUE;

      if (etched_in) {
	  Redisplay((Widget) pb, NULL, NULL);
      }
      else
	  if ((pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
	      (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
	      XmeDrawShadows
		  (XtDisplay (pb), XtWindow (pb),
		   LabG_TopShadowGC(pb), LabG_BottomShadowGC(pb),
		   pb->rectangle.x + pb->gadget.highlight_thickness,
		   pb->rectangle.y + pb->gadget.highlight_thickness,
		   pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		   pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		   pb->gadget.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

      if (!already_armed && PBG_ArmCallback(pb))
	{
	  XFlush (XtDisplay (pb));

	  call_value.reason = XmCR_ARM;
	  call_value.event = event;
	  XtCallCallbackList ((Widget) pb, PBG_ArmCallback(pb), &call_value);
	}

    }
  else
    {
      DrawBorderHighlight((Widget) pb);
    }
}


static void
DrawBorderHighlight(
        Widget wid)
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  int x, y, width, height, delta;
  Dimension highlight_width;
  XtEnum default_button_emphasis;

  if (!pb->rectangle.width || !pb->rectangle.height)
    return;

  pb->gadget.highlighted = True;
  pb->gadget.highlight_drawn = True;

  if ((PBG_DefaultButtonShadowThickness(pb) > 0))
    highlight_width = pb->gadget.highlight_thickness - Xm3D_ENHANCE_PIXEL;
  else
    highlight_width = pb->gadget.highlight_thickness;

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
	  if (PBG_DefaultButtonShadowThickness(pb))
	    delta = Xm3D_ENHANCE_PIXEL +
	      2 * (PBG_Compatible(pb) ?
		   PBG_ShowAsDefault(pb) :
		   PBG_DefaultButtonShadowThickness(pb));
	  else
	    delta = 0;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      x = pb->rectangle.x + delta;
      y = pb->rectangle.y + delta;
      width = pb->rectangle.width - 2 * delta;
      height = pb->rectangle.height - 2 * delta;

      XmeDrawHighlight(XtDisplay(pb), XtWindow(pb), LabG_HighlightGC(pb),
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
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  XEvent * event = NULL;

  if (LabG_IsMenupane(pb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      if (!PBG_Armed(pb))
	return;

      PBG_Armed(pb) = FALSE;

      if (etched_in) {
	  Redisplay((Widget) pb, NULL, NULL);
      }

      XmeClearBorder(XtDisplay (pb), XtWindow (pb),
		     pb->rectangle.x + pb->gadget.highlight_thickness,
		     pb->rectangle.y + pb->gadget.highlight_thickness,
		     pb->rectangle.width - 2 * pb->gadget.highlight_thickness,
		     pb->rectangle.height - 2 * pb->gadget.highlight_thickness,
		     pb->gadget.shadow_thickness);

      if (PBG_DisarmCallback(pb))
	{
	  XFlush (XtDisplay (pb));

	  call_value.reason = XmCR_DISARM;
	  call_value.event = event;
	  XtCallCallbackList((Widget) pb, PBG_DisarmCallback(pb), &call_value);
	}
    }
  else
    {
      int border = pb->gadget.highlight_thickness - Xm3D_ENHANCE_PIXEL;
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
      XtEnum default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmINTERNAL_HIGHLIGHT:
	  if (PBG_DefaultButtonShadowThickness(pb) && (border > 0))
	    {
	      int x, y, width, height, delta;

	      pb->gadget.highlighted = False;
	      pb->gadget.highlight_drawn = False;

	      delta = Xm3D_ENHANCE_PIXEL +
		2 * (PBG_Compatible(pb) ?
		     PBG_ShowAsDefault(pb) :
		     PBG_DefaultButtonShadowThickness(pb));

	      x = pb->rectangle.x + delta;
	      y = pb->rectangle.y + delta;
	      width = pb->rectangle.width - 2 * delta;
	      height = pb->rectangle.height - 2 * delta;

	      XmeClearBorder(XtDisplay(pb), XtWindow(pb),
			     x, y, width, height, border);
	      break;
	    }
	  /* else fall through to XmEXTERNAL_HIGHLIGHT. */

	case XmEXTERNAL_HIGHLIGHT:
	  (*(xmGadgetClassRec.gadget_class.border_unhighlight)) (wid);
	  break;

	default:
	  assert(FALSE);
	  return;
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

static void
KeySelect(
        Widget wid,
        XEvent *event )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmPushButtonCallbackStruct call_value;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

  if (!_XmIsEventUnique(event))
    return;

  if (!_XmGetInDragMode((Widget) pb))
    {

      PBG_Armed(pb) = FALSE;

      if (menuSTrait != NULL)
	menuSTrait->buttonPopdown(XtParent(pb), event);

      _XmRecordEvent(event);

      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;

      /* if the parent is menu system able, notify it about the select */
      if (menuSTrait != NULL)
	{
	  menuSTrait->entryCallback(XtParent(pb), (Widget) pb, &call_value);
	}

      if ((! LabG_SkipCallback(pb)) &&
	  (PBG_ActivateCallback(pb)))
	{
	  XFlush (XtDisplay (pb));
	  XtCallCallbackList ((Widget) pb, PBG_ActivateCallback(pb),
			      &call_value);
	}

      if (menuSTrait != NULL)
	menuSTrait->reparentToTearOffShell(XtParent(pb), event);
    }
}

/***********************************************************
 *
 *  ClassInitialize
 *
 ************************************************************/

static void
ClassInitialize( void )
{
  Cardinal	 wc_num_res, sc_num_res;
  XtResource	 *merged_list;
  int		 i, j;
  XtResourceList uncompiled;
  Cardinal	 num;

  /*
   * Label's and Pushbutton's resource lists are being merged into one
   * and assigned to xmPushButtonGCacheObjClassRec.  This is for performance
   * reasons, since instead of two calls to XtGetSubResources(),
   * XtGetSubvaluse() and XtSetSubvalues() for both the superclass and
   * the widget class, now we have just one call with a merged resource list.
   * NOTE: At this point the resource lists for Label and Pushbutton do
   * have unique entries, but if there are resources in the superclass
   * that are being overwritten by the subclass then the merged_lists
   * need to be created differently.
   */

  wc_num_res = xmPushButtonGCacheObjClassRec.object_class.num_resources;
  sc_num_res = xmLabelGCacheObjClassRec.object_class.num_resources;

  merged_list = (XtResource *)
    XtMalloc((sizeof(XtResource) * (wc_num_res + sc_num_res)));

  _XmTransformSubResources(xmLabelGCacheObjClassRec.object_class.resources,
                           sc_num_res, &uncompiled, &num);

  for (i = 0; i < num; i++)
    merged_list[i] = uncompiled[i];
  XtFree((char *)uncompiled);

  for (i = 0, j = num; i < wc_num_res; i++, j++)
    merged_list[j] = xmPushButtonGCacheObjClassRec.object_class.resources[i];

  xmPushButtonGCacheObjClassRec.object_class.resources = merged_list;
  xmPushButtonGCacheObjClassRec.object_class.num_resources =
    wc_num_res + sc_num_res;

  PushBGClassExtensionRec.record_type = XmQmotif;
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
  _XmFastSubclassInit (wc, XmPUSH_BUTTON_GADGET_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabelG */
  _XmLabelGCloneMenuSavvy (wc, &MenuSavvyRecord);

  /* Install the activatable trait for all subclasses */
  XmeTraitSet((XtPointer) wc, XmQTactivatable, (XtPointer) &pushButtonGAT);

  /* Install the takesDefault trait for all subclasses */
  XmeTraitSet((XtPointer) wc, XmQTtakesDefault, (XtPointer) &pushButtonGTDT);
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
  XmBaseClassExt *cePtr;
  XmWidgetExtData extData;
  WidgetClass	  wc;
  Cardinal	  size;
  XtPointer	  newSec, reqSec;

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
   * CallProcs are invoked by XtGetSubresources().
   */

  LabG_Cache(new_w) = &(((XmLabelGCacheObject)newSec)->label_cache);
  LabG_Cache(req)   = &(((XmLabelGCacheObject)reqSec)->label_cache);
  PBG_Cache(new_w)  = &(((XmPushButtonGCacheObject)newSec)->pushbutton_cache);
  PBG_Cache(req)    = &(((XmPushButtonGCacheObject)reqSec)->pushbutton_cache);


  /*
   * Since the resource lists for label and pushbutton were merged at
   * ClassInitialize time we need to make only one call to
   * XtGetSubresources()
   */

  XtGetSubresources (new_w, newSec, NULL, NULL,
		     wc->core_class.resources,
		     wc->core_class.num_resources,
		     args, *num_args);


  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;

  ((XmPushButtonGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
  ((XmPushButtonGCacheObject)newSec)->ext.logicalParent = new_w;

  _XmPushWidgetExtData(new_w, extData,
		       ((XmPushButtonGCacheObject)newSec)->ext.extensionType);
  memcpy(reqSec, newSec, size);
}

/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/

/*ARGSUSED*/
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

/*ARGSUSED*/
static void
InitializePosthook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData     ext;
  XmPushButtonGadget  pbw = (XmPushButtonGadget)new_w;

  /*
   * - register parts in cache.
   * - update cache pointers
   * - and free req
   */

  _XmProcessLock();
  LabG_Cache(pbw) = (XmLabelGCacheObjPart *)
    _XmCachePart(LabG_ClassCachePart(pbw),
		 (XtPointer) LabG_Cache(pbw),
		 sizeof(XmLabelGCacheObjPart));

  PBG_Cache(pbw) = (XmPushButtonGCacheObjPart *)
    _XmCachePart(PBG_ClassCachePart(pbw),
		 (XtPointer) PBG_Cache(pbw),
		 sizeof(XmPushButtonGCacheObjPart));

  /*
   * We might want to break up into per-class work that gets explicitly
   * chained. For right now, each class has to replicate all
   * superclass logic in hook routine.
   */

  /*
   * free the req subobject used for comparisons
   */
  _XmPopWidgetExtData ((Widget) pbw, &ext, XmCACHE_EXTENSION);
  _XmExtObjFree ((XtPointer)ext->widget);
  _XmExtObjFree ((XtPointer)ext->reqWidget);
  _XmProcessUnlock();
  XtFree((char *) ext);
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
  XmPushButtonGadget request = (XmPushButtonGadget) rw;
  XmPushButtonGadget new_w = (XmPushButtonGadget) nw;
  XmGadgetPart	    *pbgadget;
  int 		     increase;
  int		     adjustment = 0;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(new_w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  if (PBG_MultiClick(new_w) == XmINVALID_MULTICLICK)
    {
      if (LabG_IsMenupane(new_w))
	PBG_MultiClick(new_w) = XmMULTICLICK_DISCARD;
      else
	PBG_MultiClick(new_w) = XmMULTICLICK_KEEP;
    }

  /* If menuProcs is not set up yet, try again. */
  _XmProcessLock();
  if (xmLabelGadgetClassRec.label_class.menuProcs == (XmMenuProc)NULL)
    xmLabelGadgetClassRec.label_class.menuProcs =
      (XmMenuProc) _XmGetMenuProcContext();
  _XmProcessUnlock();


  PBG_Armed(new_w) = FALSE;
  PBG_Timer(new_w) = 0;
  ((XmPushButtonGadget)(new_w))->pushbutton.cache->timer_widget = (Widget)NULL;

  /*
   * Fix to introduce Resource XmNdefaultBorderWidth and compatibility
   *	variable.
   *  if defaultBorderWidth > 0, the program knows about this resource
   *	and is therefore a Motif 1.1 program; otherwise it is a Motif 1.0
   *      program and old semantics of XmNshowAsDefault prevails.
   *  - Sankar 2/1/90.
   */

  if (PBG_DefaultButtonShadowThickness(new_w) > 0)
    PBG_Compatible (new_w) = False;
  else
    PBG_Compatible (new_w) = True;

  if (PBG_Compatible (new_w))
    PBG_DefaultButtonShadowThickness(new_w) = PBG_ShowAsDefault(new_w);

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen(nw),NULL,NULL) != XmUNSPECIFIED_PIXMAP)
    {
      PBG_Compatible (new_w) = False;
      PBG_DefaultButtonShadowThickness(new_w) = 0;
    }
#endif

  /* No unarm_pixmap but do have an arm_pixmap, use that. */
  if ((LabG_Pixmap(new_w) == XmUNSPECIFIED_PIXMAP) &&
      (PBG_ArmPixmap(new_w) != XmUNSPECIFIED_PIXMAP))
    {
      XtWidgetProc resize;
      LabG_Pixmap(new_w) = PBG_ArmPixmap(new_w);
      if (request->rectangle.width == 0)
	new_w->rectangle.width = 0;
      if (request->rectangle.height == 0)
	new_w->rectangle.height = 0;

      _XmCalcLabelGDimensions((Widget) new_w);
      _XmProcessLock();
      resize = xmLabelGadgetClassRec.rect_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) new_w);
    }

    if ((LabG_LabelType(new_w) == XmPIXMAP) &&
       (PBG_ArmPixmap(new_w) != XmUNSPECIFIED_PIXMAP))
    {
      if (request->rectangle.width == 0)
	new_w->rectangle.width = 0;
      if (request->rectangle.height == 0)
	new_w->rectangle.height = 0;
      SetPushButtonSize(new_w);
    }

  PBG_UnarmPixmap(new_w) = LabG_Pixmap(new_w);


  if (PBG_DefaultButtonShadowThickness(new_w))
    {
      /*
       * Special hack for 3d enhancement of location cursor highlight.
       *  - Make the box bigger. During drawing of location cursor
       *    make it smaller.  See in Primitive.c
       *  May be we should use the macro: G_HighLightThickness(pbgadget);
       */

      pbgadget = (XmGadgetPart *) (&(new_w->gadget));
      pbgadget->highlight_thickness += Xm3D_ENHANCE_PIXEL;
      adjustment += Xm3D_ENHANCE_PIXEL;

      increase =  2 * PBG_DefaultButtonShadowThickness(new_w) +
	new_w->gadget.shadow_thickness + adjustment;

      /* Add the increase to the rectangle to compensate for extra space */
      if (increase != 0)
	{
	  LabG_MarginLeft(new_w) += increase;
	  LabG_MarginRight(new_w) += increase;
	  LabG_TextRect_x(new_w) += increase;
	  new_w->rectangle.width += (increase  << 1);

	  LabG_MarginTop(new_w)   += increase;
	  LabG_MarginBottom(new_w) += increase;
	  LabG_TextRect_y(new_w) += increase;
	  new_w->rectangle.height += (increase  << 1);
	}
    }

  if (LabG_IsMenupane(new_w))
    {
      new_w->gadget.traversal_on = TRUE;
    }


  if (PBG_ArmColor(new_w) == INVALID_PIXEL)
    {
      XrmValue value;

      value.size = sizeof(Pixel);

      _XmSelectColorDefault ((Widget)new_w,
			     XtOffsetOf(struct _XmPushButtonGCacheObjRec,
					pushbutton_cache.arm_color),
			    &value);
      memcpy((char*) &PBG_ArmColor(new_w), value.addr, value.size);
    }

  /* Get the background fill GC */
  if (! LabG_IsMenupane(new_w) || etched_in)
    {
	GetFillGC (new_w);
	new_w->label.fill_bg_box = _XmALWAYS_FILL_BG_BOX;
	if (LabG_BackgroundGC(new_w))
          XtReleaseGC (XtParent (new_w), LabG_BackgroundGC(new_w));
	_XmLabelSetBackgroundGC((XmLabelGadget) new_w);
    }
  else
      PBG_FillGc(new_w) = 0;

  /* Set to zero so _XmPushBCacheCompare will behave correctly. */
  PBG_BackgroundGc(new_w) = 0;

  /*  Initialize the interesting input types.  */
  new_w->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT | XmHELP_EVENT |
    XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT
      | XmMULTI_ARM_EVENT |  XmMULTI_ACTIVATE_EVENT | XmBDRAG_EVENT;
}

/************************************************************************
 *
 *  GetFillGC
 *     Get the graphics context used for filling in background of button.
 *
 ************************************************************************/

static void
GetFillGC(
        XmPushButtonGadget pb )
{
  XGCValues values;
  XtGCMask  valueMask;
  XmManagerWidget mw = (XmManagerWidget) XtParent(pb);

  valueMask = GCForeground | GCBackground | GCFillStyle;

  values.foreground = PBG_ArmColor(pb);
  values.background = LabG_Background(pb);
  values.fill_style = FillSolid;

  PBG_FillGc(pb) = XtGetGC ((Widget) mw, valueMask, &values);
}

/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/

/*ARGSUSED*/
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
  XmPushButtonGCacheObject    newSec, reqSec;

  _XmProcessLock();
  cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
  ec = (*cePtr)->secondaryObjectClass;
  size = ec->core_class.widget_size;

  newSec = (XmPushButtonGCacheObject)_XmExtObjAlloc(size);
  reqSec = (XmPushButtonGCacheObject)_XmExtObjAlloc(size);
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

  memcpy(&(newSec->label_cache),
	 LabG_Cache(newParent),
	 sizeof(XmLabelGCacheObjPart));

  memcpy(&(newSec->pushbutton_cache),
	 PBG_Cache(newParent),
	 sizeof(XmPushButtonGCacheObjPart));

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;
  _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

  /*
   * Since the resource lists for label and pushbutton were merged at
   * ClassInitialize time we need to make only one call to
   * XtSetSubvalues()
   */

  XtSetSubvalues((XtPointer)newSec,
		 ec->core_class.resources,
		 ec->core_class.num_resources,
		 args, *num_args);


  memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

  LabG_Cache(newParent) = &(((XmLabelGCacheObject)newSec)->label_cache);
  LabG_Cache(refParent) =
    &(((XmLabelGCacheObject)extData->reqWidget)->label_cache);

  PBG_Cache(newParent) =
    &(((XmPushButtonGCacheObject)newSec)->pushbutton_cache);
  PBG_Cache(refParent) =
    &(((XmPushButtonGCacheObject)extData->reqWidget)->pushbutton_cache);

  _XmExtImportArgs((Widget)newSec, args, num_args);

  /* CR 2990: Use XmNbuttonFontList as the default. */
  if (LabG_Font(newParent) == NULL)
    LabG_Font(newParent) =
      XmeGetDefaultRenderTable (newParent, XmBUTTON_FONTLIST);

  return FALSE;
}

/************************************************************************
 *
 *  GetValuesPrehook
 *
 ************************************************************************/

/*ARGSUSED*/
static void
GetValuesPrehook(
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData             extData;
  XmBaseClassExt              *cePtr;
  WidgetClass                 ec;
  XmPushButtonGCacheObject    newSec;
  Cardinal                    size;

  _XmProcessLock();
  cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
  ec = (*cePtr)->secondaryObjectClass;
  size = ec->core_class.widget_size;

  newSec = (XmPushButtonGCacheObject)_XmExtObjAlloc(size);
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

  memcpy(&(newSec->label_cache),
	 LabG_Cache(newParent),
	 sizeof(XmLabelGCacheObjPart));

  memcpy(&(newSec->pushbutton_cache),
	 PBG_Cache(newParent),
	 sizeof(XmPushButtonGCacheObjPart));

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

  /* Note that if a resource is defined in the superclass's as well as a
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
   * Since the resource lists for label and pushbutton were merged at
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
        ArgList args,
        Cardinal *num_args )
{
  XmWidgetExtData ext;

  _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

  _XmProcessLock();
  _XmExtObjFree((XtPointer)ext->widget);
  _XmProcessUnlock();
  XtFree((char *) ext);
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
      _XmCacheDelete((XtPointer) LabG_Cache(current));
      LabG_Cache(new_w) = (XmLabelGCacheObjPart *)
	_XmCachePart(LabG_ClassCachePart(new_w),
		     (XtPointer) LabG_Cache(new_w),
		     sizeof(XmLabelGCacheObjPart));
    }
  else
    LabG_Cache(new_w) = LabG_Cache(current);


  /* assign if changed! */
  if (!_XmPushBCacheCompare((XtPointer)PBG_Cache(new_w),
		            (XtPointer)PBG_Cache(current)))
    {
      _XmCacheDelete((XtPointer) PBG_Cache(current));
      PBG_Cache(new_w) = (XmPushButtonGCacheObjPart *)
	_XmCachePart(PBG_ClassCachePart(new_w),
		     (XtPointer) PBG_Cache(new_w),
		     sizeof(XmPushButtonGCacheObjPart));
    }
  else
    PBG_Cache(new_w) = PBG_Cache(current);

  _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

  _XmExtObjFree((XtPointer)ext->widget);
  _XmExtObjFree((XtPointer)ext->reqWidget);
  _XmProcessUnlock();

  XtFree((char *) ext);

  return FALSE;
}

/*************************************<->*************************************
 *
 *  SetValues(current, request, new_w)
 *
 *   Description:
 *   -----------
 *     This is the set values procedure for the pushbutton class.  It is
 *     called last (the set values rtnes for its superclasses are called
 *     first).
 *
 *
 *   Inputs:
 *   ------
 *    current = original widget;
 *    request = original copy of request;
 *    new_w = copy of request which reflects changes made to it by
 *          set values procedures of its superclasses;
 *    last = TRUE if this is the last set values procedure to be called.
 *
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
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
  XmPushButtonGadget current = (XmPushButtonGadget) cw;
  XmPushButtonGadget request = (XmPushButtonGadget) rw;
  XmPushButtonGadget new_w = (XmPushButtonGadget) nw;
  int		     increase;
  Boolean	     flag = FALSE;    /* our return value */
  XmManagerWidget    newmw = (XmManagerWidget) XtParent(new_w);
  int		     adjustment;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(new_w));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

  /*
   * Fix to introduce Resource XmNdefaultBorderWidth and compatibility
   *      variable.
   *  if  defaultBorderWidth of the current and new are different, then
   *  the programmer is setting the resource XmNdefaultBorderWidth; i.e. it
   *  defaultBorderWidth > 0, the program knows about this resource
   *  a Motif 1.1 program; otherwise it is a Motif 1.0
   *      program and old semantics of XmNshowAsDefault prevails.
   *     Note if (PBG_ShowAsDefault(gadget) == 0) then we are NOT currently
   *      drawing defaultBorderWidth; if it is > 0, we should be drawing
   *	    the shadow in defaultorderWidth;
   *  - Sankar 2/1/90.
   */


  if (PBG_DefaultButtonShadowThickness(new_w) !=
      PBG_DefaultButtonShadowThickness(current))
    PBG_Compatible (new_w) = False;

  if (PBG_Compatible (new_w))
    PBG_DefaultButtonShadowThickness(new_w) = PBG_ShowAsDefault(new_w);

  adjustment = AdjustHighLightThickness (new_w, current);

  if (PBG_DefaultButtonShadowThickness(new_w) !=
      PBG_DefaultButtonShadowThickness(current))
    {
      if (PBG_DefaultButtonShadowThickness(new_w) >
	  PBG_DefaultButtonShadowThickness(current))
	{
	  if (PBG_DefaultButtonShadowThickness(current) > 0)
            increase =  (2 * PBG_DefaultButtonShadowThickness(new_w) +
                         new_w->gadget.shadow_thickness) -
			   (2 * PBG_DefaultButtonShadowThickness(current) +
			    current->gadget.shadow_thickness);
	  else
            increase =  (2 * PBG_DefaultButtonShadowThickness(new_w) +
                         new_w->gadget.shadow_thickness);
	}
      else
	{
	  if (PBG_DefaultButtonShadowThickness(new_w) > 0)
            increase = - ((2 * PBG_DefaultButtonShadowThickness(current) +
                           current->gadget.shadow_thickness) -
                          (2 * PBG_DefaultButtonShadowThickness(new_w) +
                           new_w->gadget.shadow_thickness));
	  else
            increase = - (2 * PBG_DefaultButtonShadowThickness(current) +
                          current->gadget.shadow_thickness);
	}

      increase += adjustment;

      if (LabG_RecomputeSize(new_w) || request->rectangle.width == 0)
	{
	  LabG_MarginLeft(new_w) += increase;
	  LabG_MarginRight(new_w) += increase;
	  new_w->rectangle.width += (increase << 1);
	  flag = TRUE;
	}
      else if (increase != 0)
	{
	  /* add the change to the rectangle */
	  LabG_MarginLeft(new_w) += increase;
	  LabG_MarginRight(new_w) += increase;
	  new_w->rectangle.width += (increase << 1);
	  flag = TRUE;
	}

      if (LabG_RecomputeSize(new_w) || request->rectangle.height == 0)
	{
	  LabG_MarginTop(new_w) += increase;
	  LabG_MarginBottom(new_w) += increase;
	  new_w->rectangle.height +=  (increase << 1);
	  flag = TRUE;
	}
      else if (increase != 0)
	{
	  /* add the change to the rectangle */
	  LabG_MarginTop(new_w)  += increase;
	  LabG_MarginBottom(new_w) += increase;
	  new_w->rectangle.height += (increase << 1);
	  flag = TRUE;
	}

#ifndef XTHREADS
      _XmReCacheLabG((Widget) new_w);
#endif
    }

  if ((PBG_ArmPixmap(new_w) != PBG_ArmPixmap(current)) &&
      (LabG_LabelType(new_w) == XmPIXMAP) && (PBG_Armed(new_w)))
    flag = TRUE;

  /* No unarm_pixmap but do have an arm_pixmap, use that. */
  if ((LabG_Pixmap(new_w) == XmUNSPECIFIED_PIXMAP) &&
      (PBG_ArmPixmap(new_w) != XmUNSPECIFIED_PIXMAP))
    {
      LabG_Pixmap(new_w) = PBG_ArmPixmap(new_w);
      if (LabG_RecomputeSize(new_w) &&
          request->rectangle.width == current->rectangle.width)
	new_w->rectangle.width = 0;
      if (LabG_RecomputeSize(new_w) &&
          request->rectangle.height == current->rectangle.height)
	new_w->rectangle.width = 0;

      _XmCalcLabelGDimensions((Widget) new_w);
      {
	XtWidgetProc resize;
	_XmProcessLock();
	resize = xmLabelGadgetClassRec.rect_class.resize;
	_XmProcessUnlock();
	(* resize) ((Widget) new_w);
      }
    }

  if (LabG_Pixmap(new_w) != LabG_Pixmap(current))
    {
      PBG_UnarmPixmap(new_w) = LabG_Pixmap(new_w);
      if ((LabG_LabelType(new_w) == XmPIXMAP) && (!PBG_Armed(new_w)))
	flag = TRUE;
    }
  if ((LabG_LabelType(new_w) == XmPIXMAP) &&
      (PBG_ArmPixmap(new_w) != PBG_ArmPixmap(current)))
    {
      if ((LabG_RecomputeSize(new_w)))
	{
          if (request->rectangle.width == current->rectangle.width)
	    new_w->rectangle.width = 0;
          if (request->rectangle.height == current->rectangle.height)
	    new_w->rectangle.height = 0;
	}
      SetPushButtonSize(new_w);
      flag = TRUE;
    }

  if ((PBG_FillOnArm(new_w) != PBG_FillOnArm(current)) &&
      (PBG_Armed(new_w) == TRUE))
    flag = TRUE;

  if (! LabG_IsMenupane(new_w) || etched_in) {
      /* See if the GC need to be regenerated and widget redrawn. */
      if (PBG_ArmColor(new_w) != PBG_ArmColor(current))
	{
	    if (PBG_Armed(new_w)) flag = TRUE;  /* see PIR 5091 */
	    XtReleaseGC ((Widget) newmw, PBG_FillGc(new_w));
	    GetFillGC (new_w);
	}

      /* Sun Apr 18 17:50:27 1993
       * Currently not using PBG_BackgroundGc().
       */
  }

  /* Initialize the interesting input types. */
  new_w->gadget.event_mask = XmARM_EVENT | XmACTIVATE_EVENT | XmHELP_EVENT |
    XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT
      | XmMULTI_ARM_EVENT | XmMULTI_ACTIVATE_EVENT | XmBDRAG_EVENT;

  /* OSF Fix pir 3469 */
  if ((flag == False) && XtIsRealized((Widget) new_w))
    {
      /* No size change has taken place. */
      if ((PBG_ShowAsDefault(current) != 0) &&
	  (PBG_ShowAsDefault(new_w) == 0))
	EraseDefaultButtonShadow (new_w);

      if ((PBG_ShowAsDefault(current) == 0) &&
	  (PBG_ShowAsDefault(new_w) != 0))
	DrawDefaultButtonShadow (new_w);
    }

  return flag;
}

/************************************************************************
 *
 *  Help
 *     This function processes Function Key 1 press occuring on the PushButton.
 *
 ************************************************************************/

static void
Help(
        XmPushButtonGadget pb,
        XEvent *event )
{
  Boolean is_menupane = LabG_IsMenupane(pb);
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(pb)), XmQTmenuSystem);

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->buttonPopdown(XtParent(pb), event);

  _XmSocorro((Widget) pb, event, NULL, NULL);

  if (is_menupane && menuSTrait != NULL)
    menuSTrait->reparentToTearOffShell(XtParent(pb), event);
}

/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void
Destroy(
        Widget wid )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;
  XmManagerWidget mw = (XmManagerWidget) XtParent(pb);
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  Boolean etched_in = dpy->display.enable_etched_in_menu;

   if (PBG_Timer(pb) &&
       ((XmPushButtonGadget)(pb))->pushbutton.cache->timer_widget == wid)
       {
       XtRemoveTimeOut (PBG_Timer(pb));
       PBG_Timer(pb) = (XtIntervalId)0;
       }

  /* BEGIN OSF Fix pir 2746 */
  if (!LabG_IsMenupane(pb) || etched_in) {
      XtReleaseGC ((Widget) mw, PBG_FillGc(pb));

      /*
       *  Sun Apr 18 17:51:25 1993
       *  Currently not using PBG_BackgroundGc
       */

      /* END OSF Fix pir 2746 */
  }

  _XmProcessLock();
  _XmCacheDelete((XtPointer) PBG_Cache(pb));
  _XmProcessUnlock();
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
  register XmPushButtonGadget pb = (XmPushButtonGadget) w;

  if (LabG_IsPixmap(w))
    SetPushButtonSize(pb);
  else {
    XtWidgetProc resize;
    _XmProcessLock();
    resize = xmLabelGadgetClassRec.rect_class.resize;
    _XmProcessUnlock();
    (* resize) ((Widget) pb);
  }
}

/*ARGSUSED*/
static void
ActivateCommonG(
        XmPushButtonGadget pb,
        XEvent *event,
        Mask event_mask )
{
  if (LabG_IsMenupane(pb))
    {
      if (event->type == ButtonRelease)
	BtnUp ((Widget) pb, event);
      else  /* assume KeyRelease */
	KeySelect ((Widget) pb, event);
    }
  else
    {
      if (event->type == ButtonRelease)
	{
	  Activate (pb, event);
	  Disarm (pb, event);
	}
      else  /* assume KeyPress or KeyRelease */
        (* (((XmPushButtonGadgetClassRec *)(pb->object.widget_class))->
	    gadget_class.arm_and_activate))
	  ((Widget) pb, event, NULL, NULL);
    }
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
GetPushBGClassSecResData(
        WidgetClass w_class,
        XmSecondaryResourceData **data_rtn )
{
  int		 arrayCount;
  XmBaseClassExt bcePtr;

  _XmProcessLock();
  bcePtr = &(PushBGClassExtensionRec);
  arrayCount =
    _XmSecondaryResourceData (bcePtr, data_rtn, NULL, NULL, NULL,
			      GetPushBGClassSecResBase);
  _XmProcessUnlock();

  return (arrayCount);
}

/*
 * GetPushBGClassResBase ()
 *   retrun the address of the base of resources.
 */

/*ARGSUSED*/
static XtPointer
GetPushBGClassSecResBase(
        Widget widget,
        XtPointer client_data )	/* unused */
{
  XtPointer widgetSecdataPtr;
  size_t    labg_cache_size = sizeof (XmLabelGCacheObjPart);
  size_t    pushbg_cache_size = sizeof (XmPushButtonGCacheObjPart);
  char *cp;

  widgetSecdataPtr = (XtPointer)
    XtMalloc(labg_cache_size + pushbg_cache_size + 1);

  _XmProcessLock();
  if (widgetSecdataPtr)
    {
      cp = (char *) widgetSecdataPtr;
      memcpy(cp, LabG_Cache(widget), labg_cache_size);
      cp += labg_cache_size;
      memcpy(cp, PBG_Cache(widget), pushbg_cache_size);
    }
  /* else Warning: error cannot allocate Memory */
  /*     widgetSecdataPtr = (XtPointer) (LabG_Cache(widget)); */

  _XmProcessUnlock();
  return (widgetSecdataPtr);
}

#ifdef DEFAULT_GLYPH_PIXMAP
/*
 * DrawDefaultGlyphPixmap (pb)
 */

static void
DrawDefaultGlyphPixmap(
        XmPushButtonGadget pb )
{
  int	       dx, dy, width, height;
  Pixmap       def_pixmap;
  unsigned int def_pixmap_width, def_pixmap_height;

  def_pixmap = _XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)),
					&def_pixmap_width,
					&def_pixmap_height);

  /* We draw in the margin right area here. */
  dx = pb->rectangle.x + pb->rectangle.width -
    (LabG_MarginRight(pb) + LabG_MarginWidth(pb) +
     pb->gadget.highlight_thickness + pb->gadget.shadow_thickness);
  dy = pb->rectangle.y + pb->gadget.highlight_thickness +
    pb->gadget.shadow_thickness + LabG_MarginTop(pb) + LabG_MarginHeight(pb) +
      (MAX(LabG_TextRect(pb).height,
	   LabG_AccTextRect(pb).height) - def_pixmap_height)/2;

  width = MIN(def_pixmap_width, LabG_MarginRight(pb));
  height = MIN(def_pixmap_height,
	       MAX(LabG_TextRect(pb).height, LabG_AccTextRect(pb).height));

  XCopyPlane (XtDisplay (pb), def_pixmap,
	      XtWindow (XtParent(pb)),
	      LabG_NormalGC(pb), 0, 0, width, height, dx, dy, 1);
}
#endif /* DEFAULT_GLYPH_PIXMAP */

#ifdef DEFAULT_GLYPH_PIXMAP
/*
 * EraseDefaultGlyphPixmap (pb)
 */

static void
EraseDefaultGlyphPixmap(
        XmPushButtonGadget pb )
{
  int dx, dy, width, height;

  /* we clear the margin right area here */
  dx = pb->rectangle.x + pb->rectangle.width -
    (LabG_MarginRight(pb) + LabG_MarginWidth(pb) +
     pb->gadget.highlight_thickness + pb->gadget.shadow_thickness);
  dy = pb->rectangle.y + pb->gadget.highlight_thickness +
    pb->gadget.shadow_thickness + LabG_MarginTop(pb) + LabG_MarginHeight(pb);

  width = LabG_MarginRight(pb);
  height = MAX(LabG_TextRect(pb).height, LabG_AccTextRect(pb).height);

  XClearArea (XtDisplay (pb), XtWindow (XtParent(pb)),
	      dx, dy, width, height, False);
}
#endif /* DEFAULT_GLYPH_PIXMAP */

/*
 * EraseDefaultButtonShadow (pb)
 *  - Called from SetValues() - effort to optimize shadow drawing.
 */

static void
EraseDefaultButtonShadow(
        XmPushButtonGadget pb )
{
  int size, x, y, width, height, delta;
  XtEnum default_button_emphasis;

  if (!XtIsRealized((Widget)pb) || !XtIsManaged((Widget)pb))
    return;

  if (LabG_IsMenupane(pb))
    {
      ShellWidget mshell = (ShellWidget) XtParent(XtParent(pb));
      if (!mshell->shell.popped_up)
	return;
    }

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
      XmUNSPECIFIED_PIXMAP)
    {
      EraseDefaultGlyphPixmap(pb);
      return;
    }
#endif

  size = PBG_DefaultButtonShadowThickness(pb);
  if (size > 0)
    {
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

      default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmEXTERNAL_HIGHLIGHT:
	  delta = pb->gadget.highlight_thickness;
	  break;

	case XmINTERNAL_HIGHLIGHT:
	  delta = Xm3D_ENHANCE_PIXEL;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      size += Xm3D_ENHANCE_PIXEL;
      x = pb->rectangle.x + delta;
      y = pb->rectangle.y + delta;
      width = pb->rectangle.width - (2 * delta);
      height = pb->rectangle.height - (2 * delta);

      XmeClearBorder(XtDisplay(pb), XtWindow(pb), x, y, width, height, size);
    }
}

/*
 * DrawDefaultButtonShadow (pb)
 *  - Called from SetValues() - effort to optimize shadow drawing.
 */

static void
DrawDefaultButtonShadow(
        XmPushButtonGadget pb )
{
  if (!(XtIsRealized((Widget)pb)))
    return;

  if (LabG_IsMenupane(pb))
    {
      ShellWidget mshell = (ShellWidget)XtParent(XtParent(pb));
      if (!mshell->shell.popped_up)
	return;
    }

  DrawDefaultButtonShadows(pb);
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
 *  Shadow. Similarly if a pushbutton gadget has (default_button-shadow_
 *  thickness > 0), and it resets the (default_button-shadow-thickness = 0)
 *  through a XtSetValue , then the existing highlight-thickness is decreased
 *  by Xm3D_ENHANCE_PIXEL.
 *  The border-highlight when drawn is however is always of the same
 *  thickness as specified by the application since compensation is done
 *  in the drawing routine (see BorderHighlight).
 */

static int
AdjustHighLightThickness(
        XmPushButtonGadget new_w,
        XmPushButtonGadget current )
{
  XmGadgetPart  *pbnew, *pbcurrent;
  int adjustment = 0;

  pbnew = (XmGadgetPart *) (&(new_w->gadget));
  pbcurrent = (XmGadgetPart *) (&(current->gadget));

  if (PBG_DefaultButtonShadowThickness(new_w))
    {
      if (!(PBG_DefaultButtonShadowThickness(current)))
	{
	  pbnew->highlight_thickness += Xm3D_ENHANCE_PIXEL;
	  adjustment += Xm3D_ENHANCE_PIXEL;
	}
      else if (pbnew->highlight_thickness != pbcurrent->highlight_thickness)
	{
	  pbnew->highlight_thickness += Xm3D_ENHANCE_PIXEL;
	  adjustment += Xm3D_ENHANCE_PIXEL;
	}
    }
  else
    {
      if (PBG_DefaultButtonShadowThickness(current))
	{
	  /* The default_button_shadow_thickness was > 0 and is now
	   * being set to 0, so take away the adjustment for enhancement.
	   */
	  if (pbnew->highlight_thickness == pbcurrent->highlight_thickness)
	    {
	      pbnew->highlight_thickness -= Xm3D_ENHANCE_PIXEL;
	      adjustment -= Xm3D_ENHANCE_PIXEL;
	    }
	}
      /*
       * This will have a bug if in a XtSetValues the application
       * removes the default_button_shadow_thickness and also
       * sets the high-light-thickness to a value of
       * (old-high-light-thickness (from previous XtSetValue) +
       *  Xm3D_ENHANCE_PIXEL).
       * This will be documented.
       */
    }
  return (adjustment);
}

/*ARGSUSED*/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
  XmPushButtonGadget pb = (XmPushButtonGadget) wid;

  if (XtIsRealized((Widget)pb))
    {
      if (LabG_IsMenupane(pb))
	{
	  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	  Boolean etched_in = dpy->display.enable_etched_in_menu;
	  ShellWidget mshell = (ShellWidget)XtParent(XtParent(pb));

	  if (!mshell->shell.popped_up)
	    return;

	  DrawPushButtonLabelGadget (pb, event, region);

	  /* Refresh border highlight too. */
	  if (PBG_Armed(pb))
	      DrawPushButtonGadgetShadows (pb);
	}
      else
	{
	  DrawPushButtonLabelGadget (pb, event, region);
	  DrawPushButtonGadgetShadows (pb);

	  if (pb->gadget.highlighted)
	    DrawBorderHighlight((Widget) pb);
	}
    }
}

/*
 * DrawPushButtonLabelGadget()
 */

static void
DrawPushButtonLabelGadget(
        XmPushButtonGadget pb,
        XEvent *event,
        Region region )
{
  GC	     tmp_gc = NULL;
  GC	     fill_tmp_gc = NULL;
  Boolean    replace_for_fill_tmpGC = False;
  Boolean    replaceGC = False;
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
  Boolean etched_in = dpy->display.enable_etched_in_menu;



  if (PBG_Armed(pb) &&
      ((! LabG_IsMenupane(pb) && PBG_FillOnArm(pb)) ||
       (LabG_IsMenupane(pb) && etched_in)))
    {
      if ((LabG_LabelType(pb) == XmSTRING ||
          LabG_LabelType(pb) == XmPIXMAP_AND_STRING) &&
	  (PBG_ArmColor(pb) == LabG_Foreground(pb)))
	{
	  tmp_gc = LabG_NormalGC(pb);
	  LabG_NormalGC(pb) = LabG_BackgroundGC(pb);
	  replaceGC = True;
	}

      fill_tmp_gc = LabG_BackgroundGC(pb);
      LabG_BackgroundGC(pb) = PBG_FillGc(pb);
      replace_for_fill_tmpGC = True;
    }

  DrawLabelGadget(pb, event, region);

  if (replaceGC)
    LabG_NormalGC(pb) = tmp_gc;

  if (replace_for_fill_tmpGC)
    LabG_BackgroundGC(pb) =  fill_tmp_gc;
}


/*
 * DrawLabelGadget()
 */

static void
DrawLabelGadget(
        XmPushButtonGadget pb,
        XEvent *event,
        Region region )
{
  Boolean    deadjusted = False;
  LRectangle background_box;

  if (LabG_LabelType(pb) == XmPIXMAP ||
     LabG_LabelType(pb) == XmPIXMAP_AND_STRING)
    {
      if (PBG_Armed(pb))
	{
	  if (PBG_ArmPixmap(pb) != XmUNSPECIFIED_PIXMAP)
	    LabG_Pixmap(pb) = PBG_ArmPixmap(pb);
	  else
	    LabG_Pixmap(pb) = PBG_UnarmPixmap(pb);
	}
      else   /* pushbutton is unarmed */
	LabG_Pixmap(pb) = PBG_UnarmPixmap(pb);
    }

  /*
   * This doesn't need to be done here every time.  But it does do
   * the right thing.
   */
  ComputePBLabelArea(pb, &background_box);

  /*
   * Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment")
   * from the margin values, so we don't confuse LabelG.  The
   * original code did the same thing, but in a round-about way.
   */
  _XmProcessLock();

  if (PBG_DefaultButtonShadowThickness(pb) > 0)
    {
      deadjusted = True;
      LabG_MarginLeft(pb) -= Xm3D_ENHANCE_PIXEL;
      LabG_MarginRight(pb) -= Xm3D_ENHANCE_PIXEL;
      LabG_MarginTop(pb) -= Xm3D_ENHANCE_PIXEL;
      LabG_MarginBottom(pb) -= Xm3D_ENHANCE_PIXEL;
    }

  _XmRedisplayLabG((Widget) pb, event, region, &background_box);

  if (deadjusted)
    {
      LabG_MarginLeft(pb) += Xm3D_ENHANCE_PIXEL;
      LabG_MarginRight(pb) += Xm3D_ENHANCE_PIXEL;
      LabG_MarginTop(pb) += Xm3D_ENHANCE_PIXEL;
      LabG_MarginBottom(pb) += Xm3D_ENHANCE_PIXEL;
    }

  _XmProcessUnlock();

}



/*
 * DrawPushButtonGadgetShadows()
 *
 *  Note: PushButton has two types of shadows: primitive-shadow and
 *	default-button-shadow.
 *  If pushbutton is in a menu only primitive shadows are drawn.
 */

static void
DrawPushButtonGadgetShadows(
        XmPushButtonGadget pb )
{
  if (PBG_DefaultButtonShadowThickness(pb)
#ifdef DEFAULT_GLYPH_PIXMAP
      || (_XmGetDefaultGlyphPixmap (XtScreen((Widget)(pb)), NULL, NULL) !=
	  XmUNSPECIFIED_PIXMAP)
#endif
      )
    {
      EraseDefaultButtonShadows (pb);
      if (PBG_ShowAsDefault(pb))
	DrawDefaultButtonShadows (pb);
    }

  if (pb->gadget.shadow_thickness > 0)
    DrawPBGadgetShadows(pb);
}

/*
 * DrawPBGadgetShadows (pb)
 *   - Should be called only if PrimitiveShadowThickness > 0
 */

static void
DrawPBGadgetShadows(
        XmPushButtonGadget pb )
{
  GC top_gc, bottom_gc;
  int dx, adjust, shadow_thickness;

  if (PBG_Armed(pb))
    {
      bottom_gc = LabG_TopShadowGC(pb);
      top_gc = LabG_BottomShadowGC(pb);
    }
  else
    {
      bottom_gc = LabG_BottomShadowGC(pb);
      top_gc = LabG_TopShadowGC(pb);
    }

  shadow_thickness = pb->gadget.shadow_thickness;

  if ((shadow_thickness > 0) && (top_gc) && (bottom_gc))
    {
      if (PBG_Compatible(pb))
	adjust = PBG_ShowAsDefault(pb);
      else
	adjust = PBG_DefaultButtonShadowThickness(pb);

      if (adjust > 0)
	{
	  adjust = (adjust << 1);
	  dx = pb->gadget.highlight_thickness + adjust +
	    pb->gadget.shadow_thickness;
	}
      else
	dx = pb->gadget.highlight_thickness;

      if ((pb->rectangle.width > 2 * dx) &&
	  (pb->rectangle.height > 2 * dx))
	{
	  XmeDrawShadows (XtDisplay (pb), XtWindow (pb), top_gc, bottom_gc,
			  dx + pb->rectangle.x, dx + pb->rectangle.y,
			  pb->rectangle.width - 2 * dx,
			  pb->rectangle.height - 2 * dx,
			  shadow_thickness, XmSHADOW_OUT);
	}
    }
}

static void
EraseDefaultButtonShadows(
        XmPushButtonGadget pb )
{
  int x, y, width, height, delta;
  int default_button_shadow;
  XtEnum default_button_emphasis;

  if (PBG_Compatible(pb))
    default_button_shadow = (int) (PBG_ShowAsDefault(pb));
  else
    default_button_shadow = (int) (PBG_DefaultButtonShadowThickness(pb));

  if (default_button_shadow > 0)
    {
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));

      default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmINTERNAL_HIGHLIGHT:
	  delta = Xm3D_ENHANCE_PIXEL;
	  break;

	case XmEXTERNAL_HIGHLIGHT:
	  delta = pb->gadget.highlight_thickness;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      x = pb->rectangle.x + delta;
      y = pb->rectangle.y + delta;
      width = pb->rectangle.width - 2 * delta;
      height = pb->rectangle.height - 2 * delta;

      if ((width > 0) && (height > 0))
	XmeClearBorder(XtDisplay (pb), XtWindow (XtParent(pb)),
		       x, y, width, height, default_button_shadow);
    }

#ifdef DEFAULT_GLYPH_PIXMAP
  else if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
	   XmUNSPECIFIED_PIXMAP)
    EraseDefaultGlyphPixmap(pb);
#endif
}

/*
 * DrawDefaultButtonShadows()
 *  - get the topShadowColor and bottomShadowColor from the parent;
 *    use those colors to construct top and bottom gc; use these
 *    GCs to draw the shadows of the button.
 *  - Should not be called if pushbutton is in a row column or in a menu.
 *  - Should be called only if a defaultbuttonshadow is to be drawn.
 */

static void
DrawDefaultButtonShadows(
        XmPushButtonGadget pb )
{
  GC top_gc, bottom_gc;
  int default_button_shadow_thickness;

#ifdef DEFAULT_GLYPH_PIXMAP
  if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL)
      != XmUNSPECIFIED_PIXMAP)
    {
      DrawDefaultGlyphPixmap(pb);
      return;
    }
#endif

  top_gc = XmParentBottomShadowGC(pb);
  bottom_gc = XmParentTopShadowGC(pb);

  if ((bottom_gc == None) || (top_gc == None))
    return;

  if (PBG_Compatible(pb))
    default_button_shadow_thickness = PBG_ShowAsDefault(pb);
  else
    default_button_shadow_thickness = PBG_DefaultButtonShadowThickness(pb);

  /*
   * Compute location of bounding box to contain the defaultButtonShadow.
   */
  if ((default_button_shadow_thickness > 0) &&
      (pb->rectangle.width > 2 * pb->gadget.highlight_thickness) &&
      (pb->rectangle.height > 2 * pb->gadget.highlight_thickness))
    {
      int x, y, width, height, delta;
      XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(pb));
      XtEnum default_button_emphasis = xm_dpy->display.default_button_emphasis;

      switch (default_button_emphasis)
	{
	case XmEXTERNAL_HIGHLIGHT:
	  delta = pb->gadget.highlight_thickness;
	  break;

	case XmINTERNAL_HIGHLIGHT:
	  delta = Xm3D_ENHANCE_PIXEL;
	  break;

	default:
	  assert(FALSE);
	  return;
	}

      x = pb->rectangle.x + delta;
      y = pb->rectangle.y + delta;
      width = pb->rectangle.width - 2 * delta;
      height = pb->rectangle.height - 2 * delta;

      XmeDrawShadows(XtDisplay(pb), XtWindow(pb),
		     top_gc, bottom_gc, x, y, width, height,
		     default_button_shadow_thickness, XmSHADOW_OUT);
    }
}

static XmImportOperator
ShowAsDef_ToHorizPix(
        Widget widget,
        int offset,
        XtArgVal *value )
{
  XtArgVal         oldValue;
  XmImportOperator returnVal;

  oldValue = *value;
  returnVal = XmeToHorizontalPixels(widget, offset, value);

  if (oldValue  &&  !*value)
    *value = (XtArgVal) 1;

  return(returnVal);
}

static Boolean
ComputePBLabelArea(
        XmPushButtonGadget pb,
        LRectangle *box )
{
  Boolean result = True;
  int dx, adjust;
  short fill = 0;

  if ((PBG_ArmColor(pb) == LabG_TopShadowColor(pb)) ||
      (PBG_ArmColor(pb) == LabG_BottomShadowColor(pb)))
    fill = 1;

  if (pb == NULL)
    result = False;
  else
    {
      if (PBG_DefaultButtonShadowThickness(pb) > 0)
	{
	  adjust = PBG_DefaultButtonShadowThickness(pb);
	  if (! LabG_IsMenupane(pb))
	    adjust += pb->gadget.shadow_thickness;
	  adjust = (adjust << 1);
	  dx = pb->gadget.highlight_thickness + adjust + fill;
	}
      else
	{
	  dx = pb->gadget.highlight_thickness;
	  if (! LabG_IsMenupane(pb))
	    dx += pb->gadget.shadow_thickness + fill;
	}

      box->x = dx + pb->rectangle.x;
      box->y = dx + pb->rectangle.y;
      adjust = (dx << 1);
      box->width = pb->rectangle.width - adjust;
      box->height= pb->rectangle.height - adjust;
    }

  return (result);
}

static void
ExportHighlightThickness(
        Widget widget,
        int offset,
        XtArgVal *value )
{
  if (PBG_DefaultButtonShadowThickness(widget) ||
      PBG_ShowAsDefault(widget))
    {
      if ((int)*value >= Xm3D_ENHANCE_PIXEL)
	*value -= Xm3D_ENHANCE_PIXEL;
    }

  XmeFromHorizontalPixels (widget, offset, value);
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
     XmPushButtonGadget newpb)
{
  unsigned int onW = 0, onH = 0, onW2 = 0, onH2 = 0;

  if (PBG_ArmPixmap(newpb) != XmUNSPECIFIED_PIXMAP)
    {
      XmeGetPixmapData(XtScreen(newpb), LabG_Pixmap(newpb),
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW, &onH);
      XmeGetPixmapData(XtScreen(newpb), PBG_ArmPixmap(newpb),
		     NULL, NULL, NULL, NULL, NULL, NULL,
		     &onW2, &onH2);
      newpb->label.PixmapRect.width = MAX(onW2, onW);
      newpb->label.PixmapRect.height = MAX(onH2, onH);
      _XmLabelGCalcTextRect((Widget)newpb);
    }

  /* Let LabelG do the rest. */
  {
      XtWidgetProc resize;
      _XmProcessLock();
      resize = xmLabelGadgetClassRec.rect_class.resize;
      _XmProcessUnlock();
      (* resize) ((Widget) newpb);
  }
}

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
  XmPushButtonGadget pb = (XmPushButtonGadget) w;
  Dimension dbShadowTh;

  switch (state)
    {
    case XmDEFAULT_READY:
      {
	/* We have pixels, but the button unit type might not be
	 * pixel, so save it and restore it after the setvalues.
	 */
	unsigned char saved_unit_type = ((XmGadget)w)->gadget.unit_type;
#ifdef DEFAULT_GLYPH_PIXMAP
	unsigned int def_pixmap_width;

	if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)),
				     &def_pixmap_width, NULL) !=
	    XmUNSPECIFIED_PIXMAP)
	  {
	    /* we will use the margin right area, so increase it */
	    PBG_Compatible(pb) = False;
	    ((XmGadget)w)->gadget.unit_type = XmPIXELS;
	    XtVaSetValues(w, XmNmarginRight, def_pixmap_width, NULL);
	    ((XmGadget)w)->gadget.unit_type = saved_unit_type;
	  }
	else
#endif
	  if (!PBG_DefaultButtonShadowThickness(pb))
	    {
	      if (pb->gadget.shadow_thickness > 1)
		dbShadowTh = pb->gadget.shadow_thickness >> 1;
	      else
		dbShadowTh = pb->gadget.shadow_thickness;

	      /* CR 7474: Disable pushbutton compatibility mode. */
	      PBG_Compatible(pb) = False;
	      ((XmGadget)w)->gadget.unit_type = XmPIXELS;
	      XtVaSetValues(w, XmNdefaultButtonShadowThickness,
			    dbShadowTh, NULL);
	      ((XmGadget)w)->gadget.unit_type = saved_unit_type;
	    }
      }
      break;

    case XmDEFAULT_ON :
      /* CR 7474: Disable pushbutton compatibility mode. */
      PBG_Compatible(pb) = False;
      XtVaSetValues(w, XmNshowAsDefault, True, NULL);
      break;

    case XmDEFAULT_OFF :
      XtVaSetValues(w, XmNshowAsDefault, False, NULL);
      break;

    case XmDEFAULT_FORGET :
    default:
#ifdef DEFAULT_GLYPH_PIXMAP
      if (_XmGetDefaultGlyphPixmap(XtScreen((Widget)(pb)), NULL, NULL) !=
	  XmUNSPECIFIED_PIXMAP)
	XtVaSetValues(w, XmNmarginRight, 0, NULL);
      else
#endif
	if (!PBG_DefaultButtonShadowThickness(pb))
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
XmCreatePushButtonGadget(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
  return XtCreateWidget (name, xmPushButtonGadgetClass,
			 parent, arglist, argcount);
}

Widget
XmVaCreatePushButtonGadget(Widget parent, char *name, ...)
{
Widget w = NULL;
va_list var;
int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmPushButtonGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedPushButtonGadget(Widget parent, char *name, ...)
{
Widget w = NULL;
va_list var;
int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmPushButtonGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
