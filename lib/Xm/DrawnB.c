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
static char rcsid[] = "$TOG: DrawnB.c /main/20 1999/04/29 13:05:14 samborn $"
#endif
#endif
/*
 * Include files & Static Routine Definitions
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <X11/X.h>
#include <Xm/ActivatableT.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/DrawnBP.h>
#include <Xm/LabelP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include "XmI.h"
#include "RepTypeI.h"
#include "LabelI.h"
#include "MenuUtilI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "PrimitiveI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"


#define DELAY_DEFAULT 100

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
static void ArmTimeout (
        		XtPointer closure,
        		XtIntervalId *id ) ;
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
static void BorderUnhighlight(
                        Widget wid) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void InitializePrehook(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Resize(
                        Widget wid) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void DrawPushButton(
                        XmDrawnButtonWidget db,
                        Boolean armed) ;
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
static void Realize(
                        Widget w,
                        XtValueMask *p_valueMask,
                        XSetWindowAttributes *attributes) ;
static void Destroy(
                        Widget wid) ;

static void ChangeCB(Widget w,
		     XtCallbackProc activCB,
		     XtPointer closure,
		     Boolean setunset) ;

static void DB_FixTearoff(XmDrawnButtonWidget db);

/********    End Static Function Declarations    ********/

/*************************************<->*************************************
 *
 *
 *   Description:   translation tables for class: DrawnButton
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/

static XtTranslations default_parsed;

#define defaultTranslations	_XmDrawnB_defaultTranslations

static XtTranslations menu_parsed;

#define menuTranslations	_XmDrawnB_menuTranslations


/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: DrawnButton
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *   Note that Primitive will register additional event handlers
 *   for traversal.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  {"Arm", 	Arm		 },
  {"Activate", 	Activate		 },
  {"MultiActivate", MultiActivate		 },
  {"MultiArm",	MultiArm },
  {"ArmAndActivate", ArmAndActivate },
  {"Disarm", 	Disarm		 },
  {"BtnDown", 	BtnDown		 },
  {"BtnUp", 	BtnUp		 },
  {"Enter", 	Enter		 },
  {"Leave",	Leave		 },
  {"ButtonTakeFocus", _XmButtonTakeFocus },
  {"MenuButtonTakeFocus", _XmMenuButtonTakeFocus	 },
  {"MenuButtonTakeFocusUp", _XmMenuButtonTakeFocusUp }
};


/*  The resource list for Drawn Button  */

static XtResource resources[] =
{
   {
     XmNmultiClick, XmCMultiClick, XmRMultiClick, sizeof (unsigned char),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.multiClick),
     XmRImmediate, (XtPointer) XmMULTICLICK_KEEP
   },

   {
     XmNpushButtonEnabled, XmCPushButtonEnabled, XmRBoolean, sizeof (Boolean),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.pushbutton_enabled),
     XmRImmediate, (XtPointer) False
   },

   {
     XmNshadowType, XmCShadowType, XmRShadowType, sizeof(unsigned char),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.shadow_type),
     XmRImmediate, (XtPointer) XmSHADOW_ETCHED_IN
   },

   {
     XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.activate_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.arm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNdisarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.disarm_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNexposeCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.expose_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNresizeCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmDrawnButtonRec, drawnbutton.resize_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof(Dimension),
     XtOffsetOf( struct _XmDrawnButtonRec, primitive.shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNlabelString, XmCXmString, XmRXmString, sizeof(XmString),
     XtOffsetOf( struct _XmDrawnButtonRec, label._label),
     XmRImmediate, (XtPointer) XmUNSPECIFIED
   },
   {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf( struct _XmPrimitiveRec, primitive.traversal_on),
	XmRImmediate,
	(XtPointer) True
   },

   {
	XmNhighlightThickness,
	XmCHighlightThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _XmPrimitiveRec, primitive.highlight_thickness),
	XmRCallProc,
	(XtPointer) _XmSetThickness
   }
};

static XmBaseClassExtRec       drawnBBaseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    InitializePrehook,                        /* initialize prehook   */
    SetValuesPrehook,			      /* set_values prehook   */
    InitializePosthook,                       /* initialize posthook  */
    XmInheritSetValuesPosthook,               /* set_values posthook  */
    XmInheritClass,                           /* secondary class      */
    XmInheritSecObjectCreate,                 /* creation proc        */
    XmInheritGetSecResData,                   /* getSecResData        */
    {0},                                      /* fast subclass        */
    XmInheritGetValuesPrehook,                /* get_values prehook   */
    XmInheritGetValuesPosthook,               /* get_values posthook  */
    XmInheritClassPartInitPrehook,            /* classPartInitPrehook */
    XmInheritClassPartInitPosthook,           /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};



/*************************************<->*************************************
 *
 *
 *   Description:  global class record for instances of class: DrawnButton
 *   -----------
 *
 *   Defines default field settings for this class record.
 *
 *************************************<->***********************************/

externaldef(xmdrawnbuttonclassrec) XmDrawnButtonClassRec xmDrawnButtonClassRec ={
  {
/* core_class record */
    /* superclass	  */	(WidgetClass) &xmLabelClassRec,
    /* class_name	  */	"XmDrawnButton",
    /* widget_size	  */	sizeof(XmDrawnButtonRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeNoCompress,
    /* compress_enterlv   */    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	NULL,
    /* display_accelerator */   NULL,
    /* extension          */    (XtPointer) &drawnBBaseClassExtRec,
  },

  { /* primitive_class record       */

   /* Primitive border_highlight   */	BorderHighlight,
    /* Primitive border_unhighlight */	BorderUnhighlight,
    /* translations		    */  XtInheritTranslations,
    /* arm_and_activate		    */  ArmAndActivate,
    /* get resources		    */  NULL,
    /* num get_resources	    */  0,
    /* extension		    */  NULL,
  },

  { /* label_class record */

    /* setOverrideCallback*/    XmInheritWidgetProc,
    /* Menu procedures    */    XmInheritMenuProc,
    /* menu trav xlations */	XtInheritTranslations,
    /* extension	  */	NULL,
  },

  { /* drawnbutton_class record */

    /* extension	  */    NULL,
  }

};
externaldef(xmdrawnbuttonwidgetclass) WidgetClass xmDrawnButtonWidgetClass =
			     (WidgetClass)&xmDrawnButtonClassRec;

/* Trait record for drawnButton */

static XmConst XmActivatableTraitRec drawnButtonAT = {
  0,		/* version */
  ChangeCB,
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


/************************************************************************
 *
 *     Arm
 *
 *     This function processes button 1 down occuring on the drawnbutton.
 *     Mark the drawnbutton as armed if XmNpushButtonEnabled is TRUE.
 *     The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/
static void
Arm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
    XButtonEvent *buttonEvent = (XButtonEvent *) event;
    XmDrawnButtonCallbackStruct call_value;

    (void) XmProcessTraversal((Widget) db, XmTRAVERSE_CURRENT);

    db -> drawnbutton.armed = TRUE;
    if (event && (event->type == ButtonPress))
	db -> drawnbutton.armTimeStamp = buttonEvent->time;

    if (db->drawnbutton.pushbutton_enabled)
	DrawPushButton(db, db->drawnbutton.armed);

    if (db->drawnbutton.arm_callback) {
	XFlush(XtDisplay (db));

	call_value.reason = XmCR_ARM;
	call_value.event = event;
	call_value.window = XtWindow (db);
	XtCallCallbackList ((Widget) db, db->drawnbutton.arm_callback,
			    &call_value);
    }
}


static void
MultiArm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    if (((XmDrawnButtonWidget) wid)->drawnbutton.multiClick == XmMULTICLICK_KEEP)
			Arm (wid, event, NULL, NULL);
}

/************************************************************************
 *
 *     Activate
 *
 *     Mark the drawnbutton as unarmed (i.e. inactive).
 *     The foreground and background colors will revert to the
 *     unarmed state if XmNinvertOnArm is set to TRUE.
 *     If the button release occurs inside of the DrawnButton, the
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
        XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   if (db -> drawnbutton.armed == FALSE)
      return;

   db->drawnbutton.click_count = 1;
   ActivateCommon ((Widget) db, buttonEvent, params, num_params);

}

static void
MultiActivate(
        Widget wid,
        XEvent *buttonEvent,
        String *params,
        Cardinal *num_params )
{
        XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   /* When a multi click sequence occurs and the user Button Presses and
    * holds for a length of time, the final release should look like a
    * new/separate activate.
    */
  if (db->drawnbutton.multiClick == XmMULTICLICK_KEEP)
  { if ((buttonEvent->xbutton.time - db->drawnbutton.armTimeStamp) >
	   XtGetMultiClickTime(XtDisplay(db)))
     db->drawnbutton.click_count = 1;
   else
     db->drawnbutton.click_count++;
   ActivateCommon ((Widget) db, buttonEvent, params, num_params) ;
   Disarm ((Widget) db, buttonEvent, params, num_params) ;
 }
}

static void
ActivateCommon(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

   if (event && (event->xbutton.type != ButtonRelease))
       return;

   db -> drawnbutton.armed = FALSE;
   if (db->drawnbutton.pushbutton_enabled)
	DrawPushButton(db, db->drawnbutton.armed);


  /* CR 9181: Consider clipping when testing visibility. */
  if ((db->drawnbutton.activate_callback) &&
      ((event->xany.type == ButtonPress) ||
       (event->xany.type == ButtonRelease)) &&
      _XmGetPointVisibility(wid, event->xbutton.x_root, event->xbutton.y_root))
   {
      XFlush(XtDisplay (db));

      call_value.reason = XmCR_ACTIVATE;
      call_value.event = event;
      call_value.window = XtWindow (db);
      call_value.click_count = db->drawnbutton.click_count;

      if ((db->drawnbutton.multiClick == XmMULTICLICK_DISCARD) &&
	  (call_value.click_count > 1))
      {
	  return;
      }

      if (menuSTrait != NULL)
      {
	menuSTrait->entryCallback(XtParent(db), (Widget) db,
					   &call_value);
      }

      if ((! db->label.skipCallback) &&
	  (db->drawnbutton.activate_callback))
      {
	 XtCallCallbackList ((Widget) db, db->drawnbutton.activate_callback,
				&call_value);
      }
   }
}



static void
DB_FixTearoff( XmDrawnButtonWidget db)
{
	 if  (XmMENU_PULLDOWN == db->label.menu_type)
	 {
		Widget mwid = XmGetPostedFromWidget(XtParent(db));
		if (mwid && XmIsRowColumn(mwid)
			&& (XmMENU_OPTION == RC_Type(mwid))
			&& _XmIsActiveTearOff(XtParent(db)))
			XmProcessTraversal((Widget) db, XmTRAVERSE_CURRENT);
	 }
}
/************************************************************************
 *
 *     ArmAndActivate
 *
 ************************************************************************/
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

   db -> drawnbutton.armed = TRUE;
   if (db->drawnbutton.pushbutton_enabled)
	DrawPushButton(db, db->drawnbutton.armed);

   XFlush(XtDisplay (db));

   if (db->drawnbutton.arm_callback)
   {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      call_value.window = XtWindow (db);
      XtCallCallbackList ((Widget) db, db->drawnbutton.arm_callback, &call_value);
   }

   call_value.reason = XmCR_ACTIVATE;
   call_value.event = event;
   call_value.window = XtWindow (db);
   call_value.click_count = 1;		/* always 1 in kselect */

   if (menuSTrait != NULL)
   {
     menuSTrait->entryCallback(XtParent(db), (Widget) db,
					&call_value);
   }

   if ((! db->label.skipCallback) &&
       (db->drawnbutton.activate_callback))
   {
      XtCallCallbackList ((Widget) db, db->drawnbutton.activate_callback,
			  &call_value);
   }

   db->drawnbutton.armed = FALSE;

   if (db->drawnbutton.disarm_callback)
   {
      call_value.reason = XmCR_DISARM;
      XtCallCallbackList ((Widget) db, db->drawnbutton.disarm_callback,
                             &call_value);
   }

   /* If the button is still around, show it released, after a short delay */
   if (!db->core.being_destroyed && db->drawnbutton.pushbutton_enabled)
   {
       db->drawnbutton.timer = XtAppAddTimeOut(
				       XtWidgetToApplicationContext((Widget)db),
                                       (unsigned long) DELAY_DEFAULT,
                                       ArmTimeout,
                                       (XtPointer)db);
   }
}

static void
ArmTimeout (
	XtPointer closure,
	XtIntervalId *id )
{
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) closure ;

  db -> drawnbutton.timer = 0;

  if (db->drawnbutton.pushbutton_enabled &&
      XtIsRealized((Widget)db) && XtIsManaged((Widget)db))
   {
     DrawPushButton(db, db->drawnbutton.armed);
     XFlush (XtDisplay (db));
   }
}



/************************************************************************
 *
 *    Disarm
 *
 *     Mark the drawnbutton as unarmed (i.e. active).
 *     The foreground and background colors will revert to the
 *     unarmed state if XmNinvertOnSelect is set to TRUE and the
 *     drawnbutton is not in a menu.
 *     The callbacks for XmNdisarmCallback are called..
 *
 ************************************************************************/
static void
Disarm(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;

   db -> drawnbutton.armed = FALSE;

   if (db->drawnbutton.disarm_callback)
   {
      XFlush(XtDisplay (db));

      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      call_value.window = XtWindow (db);
      XtCallCallbackList ((Widget) db, db->drawnbutton.disarm_callback, &call_value);
   }
}



/************************************************************************
 *
 *     BtnDown
 *
 *     This function processes a button down occuring on the drawnbutton
 *     when it is in a popup, pulldown, or option menu.
 *     Popdown the posted menu.
 *     Turn parent's traversal off.
 *     Mark the drawnbutton as armed (i.e. active).
 *     The callbacks for XmNarmCallback are called.
 *
 ************************************************************************/

static void
BtnDown(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
  XmDrawnButtonCallbackStruct call_value;
  Boolean validButton = False;
  Boolean already_armed;
  ShellWidget popup;
  XmMenuSystemTrait menuSTrait;

  /* Support menu replay, free server input queue until next button event */
  XAllowEvents(XtDisplay(db), SyncPointer, CurrentTime);

  /* If no menu system trait then parent isn't a menu as it should be. */
  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(db)), XmQTmenuSystem);
  if (menuSTrait == NULL)
    return;

  if (event && (event->type == ButtonPress))
    validButton = menuSTrait->verifyButton(XtParent(db), event);

  if (!validButton)
    return;

  _XmSetInDragMode((Widget)db, True);

  /* Popdown other popus that may be up */
  if (!(popup = (ShellWidget)_XmGetRC_PopupPosted(XtParent(db))))
    {
      if (!XmIsMenuShell(XtParent(XtParent(db))))
	{
	  /* In case tear off not armed and no grabs in place, do it now.
	   * Ok if already armed and grabbed - nothing done.
	   */
	  menuSTrait->tearOffArm(XtParent(db));
	}
    }

  if (popup)
    {
      if (popup->shell.popped_up)
	menuSTrait->popdownEveryone((Widget) popup, event);
    }

  /* Set focus to this drawnbutton.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal ((Widget) db, XmTRAVERSE_CURRENT);
  /* get the location cursor - get consistent with Gadgets */

  already_armed = db->drawnbutton.armed;
  db->drawnbutton.armed = TRUE;

  if (db->drawnbutton.arm_callback && !already_armed)
    {
      XFlush (XtDisplay (db));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget) db, db->drawnbutton.arm_callback, &call_value);
    }
  _XmRecordEvent (event);
}

/************************************************************************
 *
 *     BtnUp
 *
 *     This function processes a button up occuring on the drawnbutton
 *     when it is in a popup, pulldown, or option menu.
 *     Mark the drawnbutton as unarmed (i.e. inactive).
 *     The callbacks for XmNactivateCallback are called.
 *     The callbacks for XmNdisarmCallback are called.
 *
 ************************************************************************/

static void
BtnUp(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
  Widget parent =  XtParent(db);
  XmDrawnButtonCallbackStruct call_value;
  Boolean flushDone = False;
  Boolean validButton = False;
  Boolean popped_up;
  Boolean is_menupane = Lab_IsMenupane(db);
  Widget shell = XtParent(XtParent(db));
  XmMenuSystemTrait menuSTrait;

  /* If no menu system trait then parent isn't a menu as it should be. */
  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(db)), XmQTmenuSystem);
  if (menuSTrait == NULL)
    return;

  if (event && (event->type == ButtonRelease))
    validButton = menuSTrait->verifyButton(parent, event);

  if (!validButton || (db->drawnbutton.armed == FALSE))
    return;

  db->drawnbutton.armed = FALSE;

  if (is_menupane && !XmIsMenuShell(shell))
    popped_up = menuSTrait->popdown((Widget) db, event);
  else
    popped_up = menuSTrait->buttonPopdown((Widget) db, event);

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
      menuSTrait->entryCallback(parent, (Widget) db, &call_value);
      flushDone = True;
    }

  if ((! db->label.skipCallback) &&
      (db->drawnbutton.activate_callback))
    {
      XFlush (XtDisplay (db));
      flushDone = True;
      XtCallCallbackList ((Widget) db, db->drawnbutton.activate_callback,
			  &call_value);
    }
  if (db->drawnbutton.disarm_callback)
    {
      if (!flushDone)
	XFlush (XtDisplay (db));
      call_value.reason = XmCR_DISARM;
      call_value.event = event;
      XtCallCallbackList ((Widget) db, db->drawnbutton.disarm_callback,
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
	  if (XtIsSensitive((Widget)db))
	    {
	      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(db));
	      Boolean etched_in = dpy->display.enable_etched_in_menu;

	      if ((db->core.width > 2 * db->primitive.highlight_thickness) &&
		  (db->core.height > 2 * db->primitive.highlight_thickness))
		XmeDrawShadows
		  (XtDisplay (db), XtWindow (db),
		   db->primitive.bottom_shadow_GC,
		   db->primitive.top_shadow_GC,
		   db->primitive.highlight_thickness,
		   db->primitive.highlight_thickness,
		   db->core.width - 2 * db->primitive.highlight_thickness,
		   db->core.height - 2 * db->primitive.highlight_thickness,
		   db->primitive.shadow_thickness,
		   etched_in ? XmSHADOW_IN : XmSHADOW_OUT);

	      XFlush (XtDisplay (db));

	      if (db->core.being_destroyed == False)
		{
		  if (!db->drawnbutton.timer)
		    db->drawnbutton.timer =
		      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)db),
				      (unsigned long) DELAY_DEFAULT,
				      ArmTimeout,
				      (XtPointer)(db));
		}

	      db->drawnbutton.armed = TRUE;
	      if (db->drawnbutton.arm_callback)
		{
		  call_value.reason = XmCR_ARM;
		  call_value.event = event;
		  XtCallCallbackList ((Widget) db, db->drawnbutton.arm_callback,
				      &call_value);
		}
	    }
	}
      else
	menuSTrait->reparentToTearOffShell(XtParent(db), event);
    }

  _XmSetInDragMode((Widget)db, False);

  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  if (!XmIsMenuShell(shell))
    XmProcessTraversal((Widget) db, XmTRAVERSE_CURRENT);
  DB_FixTearoff(db);
}

/************************************************************************
 *
 *  Enter
 *
 ************************************************************************/
static void
Enter(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;

   if (Lab_IsMenupane(db)) {
     if ((((ShellWidget) XtParent(XtParent(db)))->shell.popped_up) &&
	 _XmGetInDragMode((Widget)db))
       {
	 XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

	 if (db->drawnbutton.armed)
	   return;

	 /* So KHelp event is delivered correctly */
	 _XmSetFocusFlag (XtParent(XtParent(db)), XmFOCUS_IGNORE, TRUE);
	 XtSetKeyboardFocus(XtParent(XtParent(db)), (Widget)db);
	 _XmSetFocusFlag (XtParent(XtParent(db)), XmFOCUS_IGNORE, FALSE);

	 db -> drawnbutton.armed = TRUE;

	 ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	 if (db->drawnbutton.pushbutton_enabled)
	   DrawPushButton(db, db->drawnbutton.armed);

	 if (db->drawnbutton.arm_callback)
	   {
	     XFlush (XtDisplay (db));

	     call_value.reason = XmCR_ARM;
	     call_value.event = event;
	     XtCallCallbackList ((Widget) db,
				 db->drawnbutton.arm_callback, &call_value);
	   }
       }
   }
   else {
     _XmPrimitiveEnter (wid, event, params, num_params);

     if (db -> drawnbutton.pushbutton_enabled &&
	 db -> drawnbutton.armed == TRUE)
       DrawPushButton(db, TRUE);
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
      XEvent *event,
      String *params,
      Cardinal *num_params )
{
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
  XmDrawnButtonCallbackStruct call_value;

  if (Lab_IsMenupane(db)) {
    if (_XmGetInDragMode((Widget)db) && db->drawnbutton.armed &&
	(/* !ActiveTearOff || */ event->xcrossing.mode == NotifyNormal))
      {
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

	db->drawnbutton.armed = FALSE;

	((XmManagerWidget) XtParent(wid))->manager.active_child = NULL;

	if (db->drawnbutton.pushbutton_enabled) {
	  XmeClearBorder
	    (XtDisplay (db), XtWindow (db),
	     db->primitive.highlight_thickness,
	     db->primitive.highlight_thickness,
	     db->core.width - 2 * db->primitive.highlight_thickness,
	     db->core.height - 2 * db->primitive.highlight_thickness,
	     db->primitive.shadow_thickness);
	}

	if (db->drawnbutton.disarm_callback)
	  {
	    XFlush (XtDisplay (db));

	    call_value.reason = XmCR_DISARM;
	    call_value.event = event;
	    XtCallCallbackList ((Widget) db,
				db->drawnbutton.disarm_callback, &call_value);
	  }
      }
  }
  else {
    _XmPrimitiveLeave (wid, event, params, num_params);

    if (db -> drawnbutton.pushbutton_enabled &&
	db -> drawnbutton.armed == TRUE)
      DrawPushButton(db, FALSE);
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
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
  XmDrawnButtonCallbackStruct call_value;
  XEvent * event = NULL;

  if (Lab_IsMenupane(db)) {
    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
    Boolean already_armed = db->drawnbutton.armed;

    db->drawnbutton.armed = TRUE;

    if (db->drawnbutton.pushbutton_enabled)
      DrawPushButton(db, db->drawnbutton.armed);

    if (!already_armed && db->drawnbutton.arm_callback)
      {
	XFlush (XtDisplay (db));

	call_value.reason = XmCR_ARM;
	call_value.event = event;
	XtCallCallbackList ((Widget) db, db->drawnbutton.arm_callback,
			    &call_value);
      }
  }
  else {
    XtWidgetProc border_highlight;

    _XmProcessLock();
    border_highlight = xmLabelClassRec.primitive_class.border_highlight;
    _XmProcessUnlock();
    (*border_highlight)(wid);
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
  XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
  XmDrawnButtonCallbackStruct call_value;
  XEvent * event = NULL;

  if (Lab_IsMenupane(db))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
      Boolean already_armed = db->drawnbutton.armed;

      db->drawnbutton.armed = FALSE;

      if (db->drawnbutton.pushbutton_enabled) {
	XmeClearBorder
	  (XtDisplay (db), XtWindow (db),
	   db->primitive.highlight_thickness,
	   db->primitive.highlight_thickness,
	   db->core.width - 2 * db->primitive.highlight_thickness,
	   db->core.height - 2 * db->primitive.highlight_thickness,
	   db->primitive.shadow_thickness);
      }

      if (already_armed && db->drawnbutton.disarm_callback)
	{
	  XFlush (XtDisplay (db));

	  call_value.reason = XmCR_DISARM;
	  call_value.event = event;
	  XtCallCallbackList ((Widget) db, db->drawnbutton.disarm_callback,
			      &call_value);
	}
    }
  else {
    XtWidgetProc border_unhighlight;

    _XmProcessLock();
    border_unhighlight = xmLabelClassRec.primitive_class.border_unhighlight;
    _XmProcessUnlock();
    (*border_unhighlight)(wid);
  }
}



/************************************************************************
 *
 *  ClassInitialize
 *     Set up the base class extension record.
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
  /* parse the various translation tables */
  menu_parsed    = XtParseTranslationTable(menuTranslations);
  default_parsed = XtParseTranslationTable(defaultTranslations);

   /* set up base class extension quark */
   drawnBBaseClassExtRec.record_type = XmQmotif;
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
  _XmFastSubclassInit (wc, XmDRAWN_BUTTON_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabel */
  _XmLabelCloneMenuSavvy (wc, &MenuSavvyRecord);

  /* Install the activatable trait for all subclasses */
  XmeTraitSet((XtPointer)wc, XmQTactivatable, (XtPointer) &drawnButtonAT);
}

/************************************************************
 *
 * InitializePrehook
 *
 * Put the proper translations in core_class tm_table so that
 * the data is massaged correctly
 *
 ************************************************************/

static void
InitializePrehook(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmDrawnButtonWidget bw = (XmDrawnButtonWidget) nw ;
  unsigned char type;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(nw)), XmQTmenuSystem);

  _XmSaveCoreClassTranslations (nw);

  if (menuSTrait != NULL)
    type = menuSTrait->type(XtParent(nw));
  else
    type = XmWORK_AREA;

  _XmProcessLock();
  if (type == XmMENU_PULLDOWN ||
      type == XmMENU_POPUP)
    nw->core.widget_class->core_class.tm_table = (String) menu_parsed;
  else
    nw->core.widget_class->core_class.tm_table = (String) default_parsed;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (bw->label.font == NULL)
    bw->label.font = XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST);
  _XmProcessUnlock();
}

/************************************************************
 *
 * InitializePosthook
 *
 * restore core class translations
 *
 ************************************************************/

static void
InitializePosthook(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  _XmRestoreCoreClassTranslations (nw);
}

/*************************************<->*************************************
 *
 *  Initialize
 *
 *************************************<->***********************************/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
   XmDrawnButtonWidget new_w = (XmDrawnButtonWidget) nw ;
   XmDrawnButtonWidget req_w = (XmDrawnButtonWidget) rw ;

   /* CR 2990:  Use XmNbuttonFontList as the default font. */
   if (req_w->label.font == NULL)
     {
       XmFontListFree (new_w->label.font);
       new_w->label.font =
	 XmFontListCopy (XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST));
     }

   new_w->drawnbutton.armed = FALSE;
   new_w->drawnbutton.timer = 0;

   /* if menuProcs is not set up yet, try again */
   if (xmLabelClassRec.label_class.menuProcs == (XmMenuProc)NULL)
      xmLabelClassRec.label_class.menuProcs =
	 (XmMenuProc) _XmGetMenuProcContext();

   if(    !XmRepTypeValidValue( XmRID_SHADOW_TYPE,
                               new_w->drawnbutton.shadow_type, (Widget) new_w)    )
   {
      new_w -> drawnbutton.shadow_type = XmSHADOW_ETCHED_IN;
   }

}

/*************************************<->*************************************
 *
 *  Resize (db)
 *
 *************************************<->***********************************/
static void
Resize(
        Widget wid )
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;
   XtWidgetProc resize;

   _XmProcessLock();
   resize = xmLabelClassRec.core_class.resize;
   _XmProcessUnlock();

  (* resize) ((Widget) db);

   /* CR 5419: Suppress redundant calls to the resize callbacks. */
   if (db->drawnbutton.resize_callback &&
       !Lab_ComputingSize(db))
   {
      XFlush(XtDisplay (db));
      call_value.reason = XmCR_RESIZE;
      call_value.event = NULL;
      call_value.window = XtWindow (db);
      XtCallCallbackList ((Widget) db, db->drawnbutton.resize_callback, &call_value);
   }
}


/*************************************<->*************************************
 *
 *  Redisplay (db, event, region)
 *
 *************************************<->***********************************/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
   XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   XmDrawnButtonCallbackStruct call_value;

   if (XtIsRealized((Widget)db))
   {
        if (event) {
	 XtExposeProc expose;

	 _XmProcessLock();
	 expose = xmLabelClassRec.core_class.expose;
	 _XmProcessUnlock();

         (* expose) ((Widget) db, event, region);
        }

 	if (db->drawnbutton.pushbutton_enabled)
 	    DrawPushButton(db, db->drawnbutton.armed);

 	else
 	    XmeDrawShadows(XtDisplay((Widget) db),
 			    XtWindow((Widget) db),
 			    db -> primitive.top_shadow_GC,
 			    db -> primitive.bottom_shadow_GC,
 			    db -> primitive.highlight_thickness,
 			    db -> primitive.highlight_thickness,
 			    db -> core.width - 2 *
 			       db -> primitive.highlight_thickness,
 			    db -> core.height - 2 *
 			       db -> primitive.highlight_thickness,
			    db -> primitive.shadow_thickness,
 			    db->drawnbutton.shadow_type);

      if (db->drawnbutton.expose_callback)
      {
         XFlush(XtDisplay (db));

	 call_value.reason = XmCR_EXPOSE;
	 call_value.event = event;
	 call_value.window = XtWindow (db);
	 XtCallCallbackList ((Widget) db, db->drawnbutton.expose_callback, &call_value);
      }

   }
}


static void
DrawPushButton(
        XmDrawnButtonWidget db,
        Boolean armed )
{
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) db));
  Boolean etched_in = dpy -> display.enable_etched_in_menu;
  Boolean in_menu = Lab_IsMenupane((Widget) db);
  Boolean do_draw;
  unsigned int type;

  do_draw = (! in_menu) || (in_menu && armed);

  if (in_menu)
    type = etched_in ? XmSHADOW_IN : XmSHADOW_OUT;
  else
    type = armed ? XmSHADOW_IN : XmSHADOW_OUT;

  if (do_draw)
    XmeDrawShadows (XtDisplay (db), XtWindow (db),
		    db -> primitive.top_shadow_GC,
		    db -> primitive.bottom_shadow_GC,
		    db -> primitive.highlight_thickness,
		    db -> primitive.highlight_thickness,
		    db -> core.width - 2 *
		    db->primitive.highlight_thickness,
		    db -> core.height - 2 *
		    db->primitive.highlight_thickness,
		    db -> primitive.shadow_thickness,
		    type);
}


/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/

static Boolean
SetValuesPrehook(
        Widget cw,		/* unused */
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmDrawnButtonWidget bw = (XmDrawnButtonWidget) nw ;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (bw->label.font == NULL)
    bw->label.font = XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST);

  return False;
}

/*************************************<->*************************************
 *
 *  SetValues(current, request, new_w)
 *
 *   Description:
 *   -----------
 *     This is the set values procedure for the drawnbutton class.  It is
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
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
   XmDrawnButtonWidget current = (XmDrawnButtonWidget) cw ;
   XmDrawnButtonWidget new_w = (XmDrawnButtonWidget) nw ;
   Boolean  flag = FALSE;    /* our return value */

    /*  Check the data put into the new widget.  */

   if(    !XmRepTypeValidValue( XmRID_SHADOW_TYPE,
                               new_w->drawnbutton.shadow_type, (Widget) new_w)    )
   {
      new_w->drawnbutton.shadow_type = current->drawnbutton.shadow_type ;
   }

   if (new_w -> drawnbutton.shadow_type != current-> drawnbutton.shadow_type ||
       new_w -> primitive.foreground != current -> primitive.foreground    ||
       new_w -> core.background_pixel != current -> core.background_pixel  ||
       new_w -> primitive.highlight_thickness !=
       current -> primitive.highlight_thickness                          ||
       new_w -> primitive.shadow_thickness !=
       current -> primitive.shadow_thickness)
   {
      flag = TRUE;
   }

   return(flag);
}




/*************************************************************************
 *
 *  Realize
 *	This function sets the bit gravity to forget.
 *
 *************************************************************************/
static void
Realize(
        Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
   Mask valueMask = *p_valueMask;

   valueMask |= CWBitGravity | CWDontPropagate;
   attributes->bit_gravity = ForgetGravity;
   attributes->do_not_propagate_mask =
      ButtonPressMask | ButtonReleaseMask |
      KeyPressMask | KeyReleaseMask | PointerMotionMask;

   XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);
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
        XmDrawnButtonWidget db = (XmDrawnButtonWidget) wid ;
   if (db->drawnbutton.timer)
       XtRemoveTimeOut (db->drawnbutton.timer);
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
    if (setunset) {
	XtAddCallback (w, XmNactivateCallback, activCB, closure);
    } else {
	XtRemoveCallback (w, XmNactivateCallback, activCB, closure);
    }
}


/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/


/************************************************************************
 *
 *  XmCreateDrawnButton
 *	Create an instance of a drawnbutton and return the widget id.
 *
 ************************************************************************/
Widget
XmCreateDrawnButton(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, xmDrawnButtonWidgetClass,
                           parent, arglist, argcount));
}

Widget
XmVaCreateDrawnButton(
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
                         xmDrawnButtonWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedDrawnButton(
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
                         xmDrawnButtonWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
