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
static char rcsid[] = "$XConsortium: ArrowB.c /main/16 1995/10/25 19:50:57 cde-sun $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <Xm/ArrowBP.h>
#include <Xm/TransltnsP.h>
#include <Xm/DrawP.h>
#include <Xm/ActivatableT.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmI.h"

#define DELAY_DEFAULT	100

/********    Static Function Declarations    ********/

static void ClassPartInitialize(
                        WidgetClass wc);
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void GetArrowGC(
                        XmArrowButtonWidget aw);
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region);
static void Destroy(
                        Widget w);
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void Arm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void MultiArm(
                        Widget aw,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void Activate(
                        Widget wid,
                        XEvent *buttonEvent,
                        String *params,
                        Cardinal *num_params);
static void MultiActivate(
                        Widget wid,
                        XEvent *buttonEvent,
                        String *params,
                        Cardinal *num_params);
static void ActivateCommon(
                        Widget wid,
                        XEvent *buttonEvent);
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ArmTimeout(
                        XtPointer closure,
                        XtIntervalId *id);
static void Disarm(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void Enter(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void Leave(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ChangeCB(Widget w,
		     XtCallbackProc activCB,
		     XtPointer closure,
		     Boolean setunset);
static void DrawArrow(XmArrowButtonWidget aw,
		      GC top_gc,
		      GC bottom_gc,
		      GC center_gc);

/********    End Static Function Declarations    ********/

/*  Default translation table and action list  */

#define defaultTranslations	_XmArrowB_defaultTranslations

static XtActionsRec actionsList[] =
{
  { "Activate",		Activate	    },
  { "MultiActivate",	MultiActivate	    },
  { "Arm",		Arm		    },
  { "MultiArm",		MultiArm	    },
  { "Disarm",		Disarm		    },
  { "ArmAndActivate",	ArmAndActivate	    },
  { "Enter",		Enter               },
  { "Leave",		Leave               },
  { "ButtonTakeFocus",	_XmButtonTakeFocus  }
};


/*  Resource list for ArrowButton  */

static XtResource resources[] =
{
  {
    XmNmultiClick, XmCMultiClick, XmRMultiClick,
    sizeof(unsigned char),
    XtOffsetOf( struct _XmArrowButtonRec, arrowbutton.multiClick),
    XmRImmediate, (XtPointer) XmMULTICLICK_KEEP
  },

  {
    XmNarrowDirection, XmCArrowDirection, XmRArrowDirection,
    sizeof(unsigned char),
    XtOffsetOf( struct _XmArrowButtonRec, arrowbutton.direction),
    XmRImmediate, (XtPointer) XmARROW_UP
  },

  {
    XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf( struct _XmArrowButtonRec, arrowbutton.activate_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf( struct _XmArrowButtonRec, arrowbutton.arm_callback),
    XmRPointer, (XtPointer) NULL
  },

  {
    XmNdisarmCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf( struct _XmArrowButtonRec, arrowbutton.disarm_callback),
    XmRPointer, (XtPointer) NULL
  },
   {
     XmNdetailShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf(XmArrowButtonRec, arrowbutton.detail_shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   }
};

static XmSyntheticResource syn_resources[] =
{
    {
        XmNdetailShadowThickness,
        sizeof(Dimension),
        XtOffsetOf(XmArrowButtonRec, arrowbutton.detail_shadow_thickness),
        XmeFromHorizontalPixels,XmeToHorizontalPixels
    }
};


/*  The ArrowButton class record definition  */

externaldef (xmarrowbuttonclassrec) XmArrowButtonClassRec xmArrowButtonClassRec=
{
  { /* Core fields */
    (WidgetClass) &xmPrimitiveClassRec, /* superclass            */
    "XmArrowButton",			/* class_name	         */
    sizeof(XmArrowButtonRec),		/* widget_size	         */
    (XtProc)NULL,			/* class_initialize      */
    ClassPartInitialize,		/* class_part_initialize */
    FALSE,				/* class_inited          */
    Initialize,				/* initialize	         */
    (XtArgsProc)NULL,			/* initialize_hook       */
    XtInheritRealize,			/* realize	         */
    actionsList,			/* actions               */
    XtNumber(actionsList),		/* num_actions    	 */
    resources,				/* resources	         */
    XtNumber(resources),		/* num_resources         */
    NULLQUARK,				/* xrm_class	         */
    TRUE,				/* compress_motion       */
    XtExposeCompressMaximal,		/* compress_exposure     */
    TRUE,				/* compress_enterleave   */
    FALSE,				/* visible_interest      */
    Destroy,				/* destroy               */
    (XtWidgetProc)NULL,			/* resize                */
    Redisplay,				/* expose                */
    SetValues,				/* set_values	         */
    (XtArgsFunc)NULL,			/* set_values_hook       */
    XtInheritSetValuesAlmost,		/* set_values_almost     */
    (XtArgsProc)NULL,			/* get_values_hook       */
    (XtAcceptFocusProc)NULL,		/* accept_focus	         */
    XtVersion,				/* version               */
    (XtPointer)NULL,			/* callback private      */
    defaultTranslations,		/* tm_table              */
    (XtGeometryHandler)NULL,		/* query_geometry        */
    (XtStringProc)NULL,			/* display_accelerator   */
    (XtPointer)NULL			/* extension             */
  },

  { /* XmPrimitive fields */
    XmInheritBorderHighlight,		/* Primitive border_highlight   */
    XmInheritBorderUnhighlight,		/* Primitive border_unhighlight */
    XtInheritTranslations,		/* translations                 */
    ArmAndActivate,			/* arm_and_activate             */
    syn_resources,         		/* syn_resources */
    XtNumber(syn_resources),	        /* num_syn_resources */
    (XtPointer) NULL         		/* extension                    */
  },

  { /* XmArrowButtonWidget fields */
    (XtPointer) NULL			/* extension			*/
  }
};

externaldef(xmarrowbuttonwidgetclass) WidgetClass xmArrowButtonWidgetClass =
			  (WidgetClass) &xmArrowButtonClassRec;


/* Trait record for arrowButton */
static const XmActivatableTraitRec arrowButtonAT =
{
  0,				/* version	*/
  ChangeCB			/* changeCB	*/
};

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
  _XmFastSubclassInit (wc, XmARROW_BUTTON_BIT);

  /* Install the activatable trait for all subclasses */
  XmeTraitSet((XtPointer)wc, XmQTactivatable, (XtPointer) &arrowButtonAT);
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
        Cardinal *num_args)
{
  XmArrowButtonWidget request = (XmArrowButtonWidget) rw;
  XmArrowButtonWidget new_w = (XmArrowButtonWidget) nw;

  /*
   *  Check the data put into the new widget from .Xdefaults
   *  or through the arg list.
   */
  (void)args;
  (void)num_args;
  if (!XmRepTypeValidValue(XmRID_ARROW_DIRECTION,
			   new_w->arrowbutton.direction, (Widget) new_w))
    {
      new_w->arrowbutton.direction = XmARROW_UP;
    }


  /*  Set up a geometry for the widget if it is currently 0.  */
  if (request->core.width == 0)
    new_w->core.width += 15;
  if (request->core.height == 0)
    new_w->core.height += 15;


  /*  Set the internal arrow variables  */
  new_w->arrowbutton.timer = 0;
  new_w->arrowbutton.selected = False;

  /*  Get the drawing graphics contexts.  */
  GetArrowGC (new_w);
}

/************************************************************************
 *
 *  GetArrowGC
 *     Get the graphics context used for drawing the arrowbutton.
 *
 ************************************************************************/

static void
GetArrowGC(
        XmArrowButtonWidget aw )
{
  XGCValues values;
  XtGCMask  valueMask, unusedMask;

  valueMask = GCForeground | GCBackground | GCGraphicsExposures;
  unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;

  values.foreground = aw->primitive.foreground;
  values.background = aw->core.background_pixel;
  values.graphics_exposures = False;

  aw->arrowbutton.arrow_GC = XtAllocateGC((Widget) aw, 0, valueMask, &values,
					  GCClipMask, unusedMask);

  valueMask |= GCFillStyle | GCStipple;
  values.fill_style = FillOpaqueStippled;
  values.stipple = _XmGetInsensitiveStippleBitmap((Widget) aw);

  aw->arrowbutton.insensitive_GC = XtAllocateGC((Widget) aw, 0, valueMask,
						&values, GCClipMask,
						unusedMask);
}

/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/

static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;
  int iwidth, iheight;
  XtExposeProc expose;

  iwidth = (int) aw->core.width - 2 * aw->primitive.highlight_thickness;
  iheight = (int) aw->core.height - 2 * aw->primitive.highlight_thickness;

  /*  Draw the arrow  */
  if ((iwidth > 0) && (iheight > 0))
    {
      if (aw->primitive.shadow_thickness > 0)
	XmeDrawShadows(XtDisplay (aw), XtWindow (aw),
		       aw->primitive.top_shadow_GC,
		       aw->primitive.bottom_shadow_GC,
		       aw->primitive.highlight_thickness,
		       aw->primitive.highlight_thickness,
		       aw->core.width - 2 * aw->primitive.highlight_thickness,
		       aw->core.height - 2 * aw->primitive.highlight_thickness,
		       aw->primitive.shadow_thickness,
		       XmSHADOW_OUT);

      if (aw->arrowbutton.selected && XtIsSensitive(wid))
	DrawArrow(aw, aw->primitive.bottom_shadow_GC,
		  aw->primitive.top_shadow_GC, aw->arrowbutton.arrow_GC);
      else
	DrawArrow(aw, aw->primitive.top_shadow_GC,
		  aw->primitive.bottom_shadow_GC,
		  (XtIsSensitive(wid) ?
		   aw->arrowbutton.arrow_GC : aw->arrowbutton.insensitive_GC));
    }

  /* Envelop our superclass expose method */
  _XmProcessLock();
  expose = xmPrimitiveClassRec.core_class.expose;
  _XmProcessUnlock();
  (*(expose)) ((Widget) aw, event, region);
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
  XmArrowButtonWidget aw = (XmArrowButtonWidget) w;

  if (aw->arrowbutton.timer)
    XtRemoveTimeOut(aw->arrowbutton.timer);

  XtReleaseGC(w, aw->arrowbutton.arrow_GC);
  XtReleaseGC(w, aw->arrowbutton.insensitive_GC);
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args)
{
  XmArrowButtonWidget current = (XmArrowButtonWidget) cw;
  XmArrowButtonWidget new_w = (XmArrowButtonWidget) nw;

  Boolean returnFlag = FALSE;

  /*  Check the data put into the new widget.  */
  (void)args;
  (void)num_args;
  if (!XmRepTypeValidValue(XmRID_ARROW_DIRECTION,
			   new_w->arrowbutton.direction, (Widget) new_w))
    {
      new_w->arrowbutton.direction = current->arrowbutton.direction;
    }


  /*  See if the GC's need to be regenerated and widget redrawn.  */
  if (new_w->core.background_pixel != current->core.background_pixel ||
      new_w->primitive.foreground != current->primitive.foreground)
    {
      returnFlag = TRUE;
      XtReleaseGC ((Widget) new_w, new_w->arrowbutton.arrow_GC);
      XtReleaseGC ((Widget) new_w, new_w->arrowbutton.insensitive_GC);
      GetArrowGC (new_w);
    }

  if (new_w->arrowbutton.direction != current->arrowbutton.direction ||
      XtIsSensitive(nw) != XtIsSensitive(cw) ||
      new_w->primitive.highlight_thickness !=
      current->primitive.highlight_thickness ||
      new_w->primitive.shadow_thickness != current->primitive.shadow_thickness)
    {
      returnFlag = TRUE;
    }

  return (returnFlag);
}

/************************************************************************
 *
 *  Arm
 *     This function processes button 1 down occuring on the arrowButton.
 *
 ************************************************************************/
static void
Arm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;
  XmArrowButtonCallbackStruct call_value;

  (void)params;
  (void)num_params;
  XmProcessTraversal((Widget) aw, XmTRAVERSE_CURRENT);

  aw->arrowbutton.selected = True;
  aw->arrowbutton.armTimeStamp = event->xbutton.time; /* see MultiActivate */

  DrawArrow(aw, aw->primitive.bottom_shadow_GC,
	    aw->primitive.top_shadow_GC, NULL);

  if (aw->arrowbutton.arm_callback)
    {
      XFlush(XtDisplay(aw));

      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget) aw,
			 aw->arrowbutton.arm_callback,
			 &call_value);
    }
}

static void
MultiArm(
        Widget aw,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  if (((XmArrowButtonWidget) aw)->arrowbutton.multiClick == XmMULTICLICK_KEEP)
    Arm(aw, event, params, num_params);
}

/************************************************************************
 *
 *  Activate
 *     This function processes button 1 up occuring on the arrowButton.
 *     If the button 1 up occurred inside the button the activate
 *     callbacks are called.
 *
 ************************************************************************/
static void
Activate(
        Widget wid,
        XEvent *buttonEvent,
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;

  (void)params;
  (void)num_params;
  if (aw->arrowbutton.selected == False)
    return;

  aw->arrowbutton.click_count = 1;
  ActivateCommon((Widget) aw, buttonEvent);
}

static void
MultiActivate(
        Widget wid,
        XEvent *buttonEvent,
        String *params,
        Cardinal *num_params)
{
  /* When a multi click sequence occurs and the user Button Presses and
   * holds for a length of time, the final release should look like a
   * new/separate activate.
   */
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;

  if (aw->arrowbutton.multiClick == XmMULTICLICK_KEEP)
    {
      if ((buttonEvent->xbutton.time - aw->arrowbutton.armTimeStamp) >
	  XtGetMultiClickTime(XtDisplay(aw)))
	aw->arrowbutton.click_count = 1;
      else
	aw->arrowbutton.click_count++;

      ActivateCommon((Widget) aw, buttonEvent);
      Disarm ((Widget) aw, buttonEvent, params, num_params);
    }
}

static void
ActivateCommon(
        Widget wid,
        XEvent *buttonEvent)
{
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;
  XmArrowButtonCallbackStruct call_value;

  aw->arrowbutton.selected = False;

  DrawArrow(aw, aw->primitive.top_shadow_GC,
	    aw->primitive.bottom_shadow_GC, NULL);

  /* CR 9181: Consider clipping when testing visibility. */
  if (((buttonEvent->xany.type == ButtonPress) ||
       (buttonEvent->xany.type == ButtonRelease)) &&
      _XmGetPointVisibility(wid,
			    buttonEvent->xbutton.x_root,
			    buttonEvent->xbutton.y_root) &&
      (aw->arrowbutton.activate_callback))
    {
      XFlush(XtDisplay(aw));

      call_value.reason = XmCR_ACTIVATE;
      call_value.event = buttonEvent;
      call_value.click_count = aw->arrowbutton.click_count;

      if ((aw->arrowbutton.multiClick == XmMULTICLICK_DISCARD) &&
	  (call_value.click_count > 1))
	return;

      XtCallCallbackList((Widget) aw,
			 aw->arrowbutton.activate_callback,
			 &call_value);
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
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonWidget ab = (XmArrowButtonWidget) wid;
  XmArrowButtonCallbackStruct call_value;
  XtExposeProc expose;

  (void)params;
  (void)num_params;
  ab->arrowbutton.selected = TRUE;
  _XmProcessLock();
  expose = ab->core.widget_class->core_class.expose;
  _XmProcessUnlock();
  (*(expose)) ((Widget) ab, event, FALSE);

  XFlush (XtDisplay (ab));

  if (ab->arrowbutton.arm_callback)
    {
      call_value.reason = XmCR_ARM;
      call_value.event = event;
      XtCallCallbackList((Widget)ab, ab->arrowbutton.arm_callback, &call_value);
    }

  call_value.reason = XmCR_ACTIVATE;
  call_value.event = event;
  call_value.click_count = 1;	/* always 1 in kselect */

  if (ab->arrowbutton.activate_callback)
    {
      XFlush (XtDisplay (ab));
      XtCallCallbackList((Widget)ab,ab->arrowbutton.activate_callback,
			 &call_value);
    }

  ab->arrowbutton.selected = FALSE;

  if (ab->arrowbutton.disarm_callback)
    {
      XFlush (XtDisplay (ab));
      call_value.reason = XmCR_DISARM;
      XtCallCallbackList((Widget) ab, ab->arrowbutton.disarm_callback,
			 &call_value);
    }

  /* If the button is still around, show it released after a short delay */
  if (ab->core.being_destroyed == False)
    {
      ab->arrowbutton.timer =
	XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)ab),
			(unsigned long) DELAY_DEFAULT,
			ArmTimeout, (XtPointer)ab);
    }
}

static void
ArmTimeout(
        XtPointer closure,
        XtIntervalId *id )
{
  XmArrowButtonWidget ab = (XmArrowButtonWidget) closure;

  ab->arrowbutton.timer = 0;

  if (XtIsRealized((Widget)ab) && XtIsManaged((Widget)ab))
    {
      XtExposeProc expose;

      _XmProcessLock();
      expose = ab->core.widget_class->core_class.expose;
      _XmProcessUnlock();
      (*(expose)) ((Widget) ab, NULL, FALSE);
      XFlush (XtDisplay (ab));
    }
}

/************************************************************************
 *
 *  Disarm
 *     This function processes button 1 up occuring on the arrowButton.
 *
 ************************************************************************/
static void
Disarm(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;
  XmArrowButtonCallbackStruct call_value;

  (void)params;
  (void)num_params;
  aw->arrowbutton.selected = False;

  DrawArrow(aw, aw->primitive.top_shadow_GC,
	    aw->primitive.bottom_shadow_GC, NULL);

  call_value.reason = XmCR_DISARM;
  call_value.event = event;
  XtCallCallbackList((Widget) aw, aw->arrowbutton.disarm_callback, &call_value);
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
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;

  _XmPrimitiveEnter (wid, event, params, num_params);

  if (aw->arrowbutton.selected && XtIsSensitive(wid))
    DrawArrow(aw, aw->primitive.bottom_shadow_GC,
	      aw->primitive.top_shadow_GC, NULL);
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
  XmArrowButtonWidget aw = (XmArrowButtonWidget) wid;

  _XmPrimitiveLeave (wid, event, params, num_params);

  if (aw->arrowbutton.selected && XtIsSensitive(wid))
    DrawArrow(aw, aw->primitive.top_shadow_GC,
	      aw->primitive.bottom_shadow_GC, NULL);
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
 *  XmCreateArrowButton
 *	Create an instance of an arrowbutton and return the widget id.
 *
 ************************************************************************/

Widget
XmCreateArrowButton(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
  return XtCreateWidget(name, xmArrowButtonWidgetClass, parent,
			arglist, argcount);
}

Widget
XmVaCreateArrowButton(
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
                         xmArrowButtonWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedArrowButton(
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
                         xmArrowButtonWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}

/* Wrapper around XmeDrawArrow to calculate sizes. */
static void
DrawArrow(XmArrowButtonWidget aw,
	  GC		      top_gc,
	  GC		      bottom_gc,
	  GC		      center_gc)
{
  Position x, y;
  Dimension width, height;
  Dimension margin = (aw->primitive.highlight_thickness +
			  aw->primitive.shadow_thickness);

  /* Don't let large margins cause confusion. */
  if (margin <= (aw->core.width / 2))
    {
      x = margin;
      width = aw->core.width - (margin * 2);
    }
  else
    {
      x = aw->core.width / 2;
      width = 0;
    }

  if (margin <= (aw->core.height / 2))
    {
      y = margin;
      height = aw->core.height - (margin * 2);
    }
  else
    {
      y = aw->core.height / 2;
      height = 0;
    }

  /* The way we currently handle 1 shadowThickness in XmeDrawArrow
     is by drawing the center a little bit bigger, so the center_gc has
     to be present. Kinda hacky... */
  if (!center_gc &&
      aw->arrowbutton.detail_shadow_thickness == 1)
      center_gc = aw->arrowbutton.arrow_GC ;

  if (center_gc)
    XSetClipMask(XtDisplay((Widget)aw), center_gc, None);

  XmeDrawArrow (XtDisplay ((Widget) aw), XtWindow ((Widget) aw),
		top_gc, bottom_gc, center_gc,
		x, y, width, height,
		aw->arrowbutton.detail_shadow_thickness,
		aw->arrowbutton.direction);
}

