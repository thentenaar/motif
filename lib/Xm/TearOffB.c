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
static char rcsid[] = "$XConsortium: TearOffB.c /main/12 1996/03/06 17:09:22 pascale $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/DrawP.h>
#include <Xm/RowColumnP.h>
#include <Xm/TearOffBP.h>
#include <Xm/TraitP.h>
#include <Xm/MenuT.h>
#include <Xm/TransltnsP.h>
#include "MapEventsI.h"
#include "MessagesI.h"
#include "RCMenuI.h"
#include "RepTypeI.h"
#include "TearOffI.h"
#include "XmI.h"

/********    Static Function Declarations    ********/

static void HeightDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
static void GetSeparatorGC(
                        XmTearOffButtonWidget tob) ;

static void Initialize(
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
static void Destroy(Widget w);
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void BDrag(Widget wid, XEvent *event, String *param, Cardinal *num_param);
static void BActivate(Widget wid, XEvent *event, String *param, Cardinal *num_param);
static void KActivate(Widget wid, XEvent *event, String *param, Cardinal *num_param);
static void ClassInitialize( void );
static void ClassPartInitialize(
                        WidgetClass wc) ;

/********    End Static Function Declarations    ********/

#define DEFAULT_HEIGHT 10


/* Definition for resources that need special processing in get values */

static XmSyntheticResource syn_resources[] =
{
   {
      XmNmargin,
      sizeof (Dimension),
      XtOffsetOf( struct _XmTearOffButtonRec, tear_off_button.margin),
      XmeFromHorizontalPixels,
      XmeToHorizontalPixels
   },
};

/*  Resource list for Separator  */

static XtResource resources[] =
{
   {
      XmNseparatorType, XmCSeparatorType, XmRSeparatorType, sizeof (unsigned char),
      XtOffsetOf( struct _XmTearOffButtonRec, tear_off_button.separator_type),
      XmRImmediate, (XtPointer) XmSHADOW_ETCHED_OUT_DASH
   },

   {
      XmNmargin,
      XmCMargin,
      XmRHorizontalDimension,
      sizeof (Dimension),
      XtOffsetOf( struct _XmTearOffButtonRec, tear_off_button.margin),
      XmRImmediate, (XtPointer)  0
   },

	/* The magic value will signal recomputeSize setting */

   {
     "pri.vate", "Pri.vate", XmRBoolean, sizeof(Boolean),
     XtOffsetOf(XmTearOffButtonRec, tear_off_button.set_recompute_size),
     XmRImmediate, (XtPointer) False
   },
   {
     XmNheight, XmCDimension, XmRVerticalDimension, sizeof(Dimension),
     XtOffsetOf( struct _WidgetRec, core.height), XmRCallProc,
     (XtPointer) HeightDefault
   },

};


/*************************************<->*************************************
 *
 *
 *   Description:   translation tables for class: PushButton
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/

#define overrideTranslations	_XmTearOffB_overrideTranslations


/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: TearOffButton
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *   Note that Primitive will register additional event handlers
 *   for traversal.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  {"BDrag", 		BDrag		 	},
  {"BActivate", 	BActivate		},
  {"KActivate", 	KActivate		},
};


externaldef(xmtearoffbuttonclassrec)
	XmTearOffButtonClassRec xmTearOffButtonClassRec = {
  {
/* core_class record */
    /* superclass	  */	(WidgetClass) &xmPushButtonClassRec,
    /* class_name	  */	"XmTearOffButton",
    /* widget_size	  */	sizeof(XmTearOffButtonRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
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
    /* resize		  */	XtInheritResize,
    /* expose		  */	Redisplay,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator */   NULL,
    /* extension          */    NULL,
  },

  { /* primitive_class record       */

    /* Primitive border_highlight   */	XmInheritBorderHighlight,
    /* Primitive border_unhighlight */	XmInheritBorderUnhighlight,
    /* translations		    */  XtInheritTranslations,
    /* arm_and_activate		    */  XmInheritArmAndActivate,
    /* get resources		    */  syn_resources,
    /* num get_resources	    */  XtNumber(syn_resources),
    /* extension		    */  NULL,
  },

  { /* label_class record */

    /* setOverrideCallback	*/	XmInheritWidgetProc,
    /* menu procedures		*/	XmInheritMenuProc,
    /* menu traversal xlation	*/ 	XtInheritTranslations,
    /* extension		*/	(XtPointer) NULL,
  },

  { /* pushbutton_class record */
    /* extension		*/	(XtPointer) NULL,
  },

  { /* tearoffbutton_class record */
    /* Button override xlation	*/	XtInheritTranslations,
  }

};
externaldef(xmtearoffbuttonwidgetclass)
   WidgetClass xmTearOffButtonWidgetClass = (WidgetClass)&xmTearOffButtonClassRec;

/*********************************************************************
 *
 * HeightDefault
 *    This procedure provides the dynamic default behavior for
 *    the height. It the height is not explicitly set by the user
 *    then, a default is given and recomputeSize will be set True in
 *    Initialize, which will have  RC probably change it to suit it needs.
 *    If this is not called, recomputeSize will be False
 *
 *********************************************************************/
static void HeightDefault(Widget widget, int offset, XrmValue *value)
{
	static Dimension default_height = DEFAULT_HEIGHT ;

	(void)offset;
	value->addr = (XPointer)&default_height;
	((XmTearOffButtonWidget)widget)->tear_off_button.set_recompute_size = True;
}

/************************************************************************
 *
 *  GetSeparatorGC
 *     Get the graphics context used for drawing the separator.
 *
 ************************************************************************/
static void
GetSeparatorGC(
        XmTearOffButtonWidget tob )
{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground;

   values.foreground = tob->primitive.foreground;
   values.background = tob->core.background_pixel;

   if (tob -> tear_off_button.separator_type == XmSINGLE_DASHED_LINE ||
       tob -> tear_off_button.separator_type == XmDOUBLE_DASHED_LINE)
   {
      valueMask = valueMask | GCLineStyle;
      values.line_style = LineDoubleDash;
   }

   tob->tear_off_button.separator_GC =
      XtGetGC ((Widget) tob, valueMask, &values);
}

/************************************************************************
 *
 *  ClassInitialize
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
   xmTearOffButtonClassRec.tearoffbutton_class.translations =
      (String)XtParseTranslationTable(overrideTranslations);
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
  _XmFastSubclassInit (wc, XmTEAROFF_BUTTON_BIT);
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
   XmTearOffButtonWidget new_w = (XmTearOffButtonWidget) nw ;
   XtTranslations trans;

   (void)rw;
   (void)args;
   (void)num_args;
   GetSeparatorGC((XmTearOffButtonWidget)nw);

   _XmProcessLock();
   trans = (XtTranslations) ((XmTearOffButtonClassRec *)
		XtClass(nw))->tearoffbutton_class.translations;
   _XmProcessUnlock();
   XtOverrideTranslations(nw, trans);

   if(!XmRepTypeValidValue(XmRID_SEPARATOR_TYPE,
			 new_w->tear_off_button.separator_type,
			 (Widget) new_w)) {
     new_w -> tear_off_button.separator_type = XmSHADOW_ETCHED_OUT_DASH;
   }

   /* force the orientation */
   new_w->tear_off_button.orientation = XmHORIZONTAL ;

   /* if set_recompute_size is True, this widget didn't have a
      specific height, so set its recompute_size to True, so that
      RCLayout can override it. if set_recompute_size is False, a
      specific height was given, so force recompute_size to False */
   if (new_w->tear_off_button.set_recompute_size) {
       new_w->label.recompute_size = True;
       new_w->tear_off_button.set_recompute_size = False ;
   } else
       new_w->label.recompute_size = False;
}

/************************************************************************
 *
 *  Redisplay (tob, event, region)
 *   Description:
 *   -----------
 *     Cause the widget, identified by tob, to be redisplayed.
 *
 ************************************************************************/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    XmTearOffButtonWidget tob = (XmTearOffButtonWidget) wid ;

    /*
     * Where do we check for dependency on MenyType ??
     */
    if (XtIsRealized((Widget)tob)) {
	XtExposeProc expose;

	XmeDrawSeparator(XtDisplay(tob), XtWindow(tob),
			 tob->primitive.top_shadow_GC,
			 tob->primitive.bottom_shadow_GC,
			 tob->tear_off_button.separator_GC,
			 tob->primitive.highlight_thickness,
			 tob->primitive.highlight_thickness,
			 tob->core.width -
			 2*tob->primitive.highlight_thickness,
			 tob->core.height -
			 2*tob->primitive.highlight_thickness,
			 tob->primitive.shadow_thickness,
			 tob->tear_off_button.margin,
			 tob->tear_off_button.orientation,
			 tob->tear_off_button.separator_type);

	/* envelop primitive expose method for highlight */

	_XmProcessLock();
	expose = xmPrimitiveClassRec.core_class.expose;
	_XmProcessUnlock();
	(*expose)(wid,event, region) ;
    }
}

/************************************************************************
 *
 *  Destroy
 *      Remove the callback lists.
 *
 ************************************************************************/
static void
Destroy(
        Widget wid )
{
   XtReleaseGC (wid,
      ((XmTearOffButtonWidget) wid)->tear_off_button.separator_GC);
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
   XmTearOffButtonWidget current = (XmTearOffButtonWidget) cw ;
   XmTearOffButtonWidget new_w = (XmTearOffButtonWidget) nw ;
   Boolean flag = FALSE;

   (void)rw;
   (void)args;
   (void)num_args;
   if(!XmRepTypeValidValue(XmRID_SEPARATOR_TYPE,
                           new_w->tear_off_button.separator_type, (Widget) new_w))
   {
      new_w -> tear_off_button.separator_type = XmSHADOW_ETCHED_OUT_DASH;
   }

   /* force the orientation */
   new_w -> tear_off_button.orientation = XmHORIZONTAL;

   if ((new_w->core.background_pixel != current->core.background_pixel) ||
       (new_w->tear_off_button.separator_type !=
         current->tear_off_button.separator_type) ||
       (new_w->primitive.foreground != current->primitive.foreground))
   {
      XtReleaseGC ((Widget) new_w, new_w->tear_off_button.separator_GC);
      GetSeparatorGC (new_w);
      flag = TRUE;
   }

   if ((new_w->tear_off_button.margin != current->tear_off_button.margin) ||
       (new_w->primitive.shadow_thickness !=
         current->primitive.shadow_thickness))
   {
      flag = TRUE;
   }

   return (flag);
}

/************************************************************************
 *
 *  BDrag
 *    On button 2 down, tear off the menu
 *
 ************************************************************************/
static void BDrag(Widget wid, XEvent *event, String *param, Cardinal *num_param)
{
	(void)param;
	(void)num_param;
	_XmTearOffInitiate(XtParent(wid), event);
}

/************************************************************************
 *
 *  BActivate
 *    On either a button down or a button up, tear off the menu
 *
 ************************************************************************/
static void BActivate(Widget wid, XEvent *event, String *param, Cardinal *num_param)
{
   XmMenuSystemTrait menu_t;
   Widget parent = XtParent(wid);

	(void)param;
	(void)num_param;
	menu_t = XmeTraitGet(XtClass(parent), XmQTmenuSystem);
	if (menu_t && menu_t->verifyButton(parent, event))
		_XmTearOffInitiate(parent, event);
}

/************************************************************************
 *
 *  KActivate
 *    Initiate Tear Off on a keypress
 ************************************************************************/
static void KActivate(Widget wid, XEvent *event, String *param, Cardinal *num_param)
{
   XButtonEvent xb_ev ;
   Widget parent = XtParent(wid);
   Position x, y;

   /* stick the tear off at the same location as the submenu */
   (void)param;
   (void)num_param;
   memcpy(&xb_ev, &event->xbutton, sizeof xb_ev);
   XtTranslateCoords(parent, XtX(parent), XtY(parent), &x, &y);

   xb_ev.x_root = x;
   xb_ev.y_root = y;
   _XmTearOffInitiate(parent, (XEvent *)&xb_ev);
}

