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
static char rcsid[] = "$TOG: ScrollBar.c /main/20 1997/03/10 14:52:28 dbl $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <Xm/DisplayP.h>        /* for enableThinThickness */
#include <Xm/DrawP.h>
#include <Xm/DropSMgr.h>	/* for XmDropSiteStartUpdate/EndUPdate */
#include <Xm/ManagerP.h>
#include <Xm/NavigatorT.h>
#include <Xm/ScrollBarP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "XmI.h"

/* see comments in ScrollBarP.h */
#define slider_visual	etched_slider
#define flat_slider_GC	unhighlight_GC

#define MESSAGE1	_XmMMsgScrollBar_0000
#define MESSAGE2	_XmMMsgScrollBar_0001
#define MESSAGE3	_XmMMsgScrollBar_0002
#define MESSAGE4	_XmMMsgScrollBar_0003
#define MESSAGE6	_XmMMsgScaleScrBar_0004
#define MESSAGE7	_XmMMsgScrollBar_0004
#define MESSAGE8	_XmMMsgScrollBar_0005
#define MESSAGE9	_XmMMsgScrollBar_0006
#define MESSAGE10	_XmMMsgScrollBar_0007
#define MESSAGE13	_XmMMsgScrollBar_0008
#define MESSAGE14	_XmMMsgMotif_0001

#define MAXDIMENSION	65535

#define DRAWARROW(sbw, t_gc, b_gc, x, y, dir)\
    XmeDrawArrow(XtDisplay ((Widget) sbw),\
		XtWindow ((Widget) sbw),\
		t_gc, b_gc,\
		sbw->scrollBar.foreground_GC,\
		x-1, y-1,\
		sbw->scrollBar.arrow_width+2,\
		sbw->scrollBar.arrow_height+2,\
		sbw->primitive.shadow_thickness,\
		dir);

#define PROCESS_DIR_INVERSED(sbw) \
    ((sbw->scrollBar.processing_direction == XmMAX_ON_LEFT) || \
     (sbw->scrollBar.processing_direction == XmMAX_ON_TOP))

#define INVERSED_VALUE(sbw) \
    (sbw->scrollBar.maximum + sbw->scrollBar.minimum - \
	sbw->scrollBar.value - sbw->scrollBar.slider_size)

/********    Static Function Declarations    ********/

static void ClassPartInitialize(
                        WidgetClass wc) ;
static void ProcessingDirectionDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void BackgroundPixelDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void TraversalDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void HighlightDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void SliderVisualDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void SliderMarkDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void EditableDefault(
                        XmScrollBarWidget widget,
                        int offset,
                        XrmValue *value) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetForegroundGC(
                        XmScrollBarWidget sbw) ;
static void GetUnavailableGC(
                        XmScrollBarWidget sbw) ;
static void GetFlatSliderGC(
                        XmScrollBarWidget sbw) ;
static void GetSliderPixmap(
                        XmScrollBarWidget sbw) ;
static void CalcSliderRect(
                        XmScrollBarWidget sbw,
                        short *slider_x,
                        short *slider_y,
                        short *slider_width,
                        short *slider_height) ;
static void DrawSliderPixmap(
                        XmScrollBarWidget sbw) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Resize(
                        Widget wid) ;
static void Realize(
                        Widget sbw,
                        XtValueMask *window_mask,
                        XSetWindowAttributes *window_attributes) ;
static void Destroy(
                        Widget wid) ;
static Boolean ValidateInputs(
                        XmScrollBarWidget current,
                        XmScrollBarWidget request,
                        XmScrollBarWidget new_w) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static int CalcSliderVal(
                        XmScrollBarWidget sbw,
                        int x,
                        int y) ;
static void Select(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Release(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Moved(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void TopOrBottom(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void IncrementUpOrLeft(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void IncrementDownOrRight(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void PageUpOrLeft(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void PageDownOrRight(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void CancelDrag(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void MoveSlider(
                        XmScrollBarWidget sbw,
                        int currentX,
                        int currentY) ;
static void RedrawSliderWindow(
                        XmScrollBarWidget sbw) ;
static Boolean ChangeScrollBarValue(
                        XmScrollBarWidget sbw) ;
static void TimerEvent(
                        XtPointer closure,
                        XtIntervalId *id) ;
static void ScrollCallback(
                        XmScrollBarWidget sbw,
                        int reason,
                        int value,
                        int xpixel,
                        int ypixel,
                        XEvent *event) ;
static void ExportScrollBarValue(
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static XmImportOperator ImportScrollBarValue(
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;


static void NavigChangeMoveCB(Widget nav,
			      XtCallbackProc moveProc,
			      XtPointer closure,
			      Boolean setunset) ;
static void NavigSetValue(Widget nav,
			  XmNavigatorData nav_data,
			  Boolean notify) ;
static void NavigGetValue(Widget nav,
			  XmNavigatorData nav_data) ;

/********    End Static Function Declarations    ********/

/*  Default translation table and action list  */

#define defaultTranslations	_XmScrollBar_defaultTranslations

static XtActionsRec actions[] =
{
	{ "Select",                 Select },
	{ "Release",                Release },
	{ "Moved",                  Moved },
	{ "TopOrBottom",            TopOrBottom },
	{ "IncrementUpOrLeft",      IncrementUpOrLeft },
	{ "IncrementDownOrRight",   IncrementDownOrRight },
	{ "PageUpOrLeft",           PageUpOrLeft },
	{ "PageDownOrRight",        PageDownOrRight },
	{ "CancelDrag",             CancelDrag },
};


/*  Resource list for ScrollBar  */

static XtResource resources[] =
{
	{ XmNnavigationType, XmCNavigationType, XmRNavigationType,
	  sizeof(unsigned char),
	  XtOffsetOf(XmScrollBarRec, primitive.navigation_type),
	  XmRImmediate, (XtPointer) XmSTICKY_TAB_GROUP
	},
	{ XmNbackground, XmCBackground, XmRPixel, sizeof(Pixel),
	  XtOffsetOf(XmScrollBarRec, core.background_pixel),
	  XmRCallProc, (XtPointer) BackgroundPixelDefault
	},
	{ XmNtroughColor, XmCTroughColor, XmRPixel, sizeof(Pixel),
	  XtOffsetOf(XmScrollBarRec, scrollBar.trough_color),
	  XmRCallProc, (XtPointer) _XmSelectColorDefault
	},
	{ XmNvalue, XmCValue, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.value),
	  XmRImmediate, (XtPointer) XmINVALID_DIMENSION
	},
	{ XmNminimum, XmCMinimum, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.minimum),
	  XmRImmediate, (XtPointer) 0
	},
	{ XmNmaximum, XmCMaximum, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.maximum),
	  XmRImmediate, (XtPointer) 100
	},
	{ XmNsliderSize, XmCSliderSize, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.slider_size),
	  XmRImmediate, (XtPointer) XmINVALID_DIMENSION
	},
	{ XmNshowArrows, XmCShowArrows, XmRShowArrows, sizeof (XtEnum),
	  XtOffsetOf(XmScrollBarRec, scrollBar.show_arrows),
	  XmRImmediate, (XtPointer) XmEACH_SIDE
	},
	{ XmNorientation, XmCOrientation,
	  XmROrientation, sizeof (unsigned char),
	  XtOffsetOf(XmScrollBarRec, scrollBar.orientation),
	  XmRImmediate, (XtPointer) XmVERTICAL
	},
	{ XmNprocessingDirection, XmCProcessingDirection,
	  XmRProcessingDirection, sizeof (unsigned char),
	  XtOffsetOf(XmScrollBarRec, scrollBar.processing_direction),
	  XmRCallProc, (XtPointer) ProcessingDirectionDefault
	},
	{ XmNincrement, XmCIncrement, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.increment),
	  XmRImmediate, (XtPointer) 1
	},
	{ XmNpageIncrement, XmCPageIncrement, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.page_increment),
	  XmRImmediate, (XtPointer) 10
	},
	{ XmNinitialDelay, XmCInitialDelay, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.initial_delay),
	  XmRImmediate, (XtPointer) 250
	},
	{ XmNrepeatDelay, XmCRepeatDelay, XmRInt, sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.repeat_delay),
	  XmRImmediate, (XtPointer) 50
	},
	{ XmNvalueChangedCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.value_changed_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNincrementCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.increment_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNdecrementCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.decrement_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNpageIncrementCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.page_increment_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNpageDecrementCallback, XmCCallback,
	  XmRCallback, sizeof (XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.page_decrement_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNtoTopCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.to_top_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNtoBottomCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.to_bottom_callback),
	  XmRPointer, (XtPointer) NULL
	},
	{ XmNdragCallback, XmCCallback,
	  XmRCallback, sizeof(XtCallbackList),
	  XtOffsetOf(XmScrollBarRec, scrollBar.drag_callback),
	  XmRPointer, (XtPointer) NULL
	},
        {
          XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof (Boolean),
          XtOffsetOf(XmPrimitiveRec, primitive.traversal_on),
          XmRCallProc, (XtPointer) TraversalDefault
        },
        {
          XmNhighlightThickness, XmCHighlightThickness,
	  XmRHorizontalDimension, sizeof (Dimension),
          XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
          XmRCallProc, (XtPointer) HighlightDefault
        },
        {
	  XmNsnapBackMultiple, XmCSnapBackMultiple, XmRShort,
          sizeof (unsigned short),
          XtOffsetOf(XmScrollBarRec, scrollBar.snap_back_multiple),
          XmRImmediate, (XtPointer) MAXDIMENSION
        },
        {
          XmNslidingMode, XmCSlidingMode, XmRSlidingMode,
	  sizeof(XtEnum), XtOffsetOf(XmScrollBarRec, scrollBar.sliding_mode),
	  XmRImmediate, (XtPointer) XmSLIDER
        },
	{
          XmNeditable, XmCEditable, XmRBoolean,
          sizeof(XtEnum), XtOffsetOf(XmScrollBarRec,scrollBar.editable),
          XmRCallProc, (XtPointer) EditableDefault
        },
        {
	  XmNsliderVisual, XmCSliderVisual, XmRSliderVisual,
          sizeof (XtEnum),
          XtOffsetOf(XmScrollBarRec, scrollBar.slider_visual),
          XmRCallProc, (XtPointer) SliderVisualDefault
        },
        {
	  XmNsliderMark, XmCSliderMark, XmRSliderMark,
          sizeof (XtEnum),
          XtOffsetOf(XmScrollBarRec, scrollBar.slider_mark),
          XmRCallProc, (XtPointer) SliderMarkDefault
        },
   };


/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
	{ XmNvalue,
	  sizeof (int),
	  XtOffsetOf(XmScrollBarRec, scrollBar.value),
	  ExportScrollBarValue,
	  ImportScrollBarValue
	},
};


externaldef(xmscrollbarclassrec) XmScrollBarClassRec xmScrollBarClassRec =
{
   {
      (WidgetClass) &xmPrimitiveClassRec, /* superclass	         */
      "XmScrollBar",                    /* class_name	         */
      sizeof(XmScrollBarRec),           /* widget_size	         */
      NULL,                             /* class_initialize      */
      ClassPartInitialize,              /* class_part_initialize */
      FALSE,                            /* class_inited          */
      Initialize,                       /* initialize	         */
      (XtArgsProc)NULL,                 /* initialize_hook       */
      Realize,                          /* realize	         */
      actions,                          /* actions               */
      XtNumber(actions),                /* num_actions	         */
      resources,                        /* resources	         */
      XtNumber(resources),              /* num_resources         */
      NULLQUARK,                        /* xrm_class	         */
      TRUE,                             /* compress_motion       */
      XtExposeCompressMaximal,          /* compress_exposure     */
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */
      Destroy,                          /* destroy               */
      Resize,                           /* resize                */
      Redisplay,                        /* expose                */
      SetValues,                        /* set_values    	 */
      (XtArgsFunc)NULL,                 /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      (XtArgsProc)NULL,			/* get_values_hook       */
      (XtAcceptFocusProc)NULL,          /* accept_focus	         */
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      defaultTranslations,              /* tm_table              */
      (XtGeometryHandler)NULL,          /* query_geometry        */
      (XtStringProc)NULL,               /* display_accelerator   */
      (XtPointer) NULL,                 /* extension             */
   },

   {
      XmInheritWidgetProc,		/* border_highlight   */
      XmInheritWidgetProc,		/* border_unhighlight */
      NULL,				/* translations       */
      (XtActionProc)NULL,		/* arm_and_activate   */
      syn_resources,   			/* syn_resources      */
      XtNumber(syn_resources),		/* num syn_resources  */
      NULL,				/* extension          */
   },

   {
      (XtPointer) NULL,			/* extension          */
   },
};

externaldef(xmscrollbarwidgetclass) WidgetClass xmScrollBarWidgetClass =
     (WidgetClass) &xmScrollBarClassRec;


/* Trait record for ScrollBar */

static XmConst XmNavigatorTraitRec scrollBarNT = {
  0,		/* version */
  NavigChangeMoveCB,
  NavigSetValue,
  NavigGetValue,
};

/*********************************************************************
 *
 *  ExportScrollBarValue
 *	Convert the scrollbar value from the normal processing direction
 *	to reverse processing if needed.
 *
 *********************************************************************/
static void
ExportScrollBarValue(
        Widget wid,
        int offset,		/* unused */
        XtArgVal *value )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
	if (PROCESS_DIR_INVERSED(sbw))
		*value = (XtArgVal) INVERSED_VALUE(sbw);
	else
		*value = (XtArgVal) sbw->scrollBar.value;
}

/*********************************************************************
 *
 *  ImportScrollBarValue
 *  Indicate that the value did indeed change.
 *
 *********************************************************************/
static XmImportOperator
ImportScrollBarValue(
        Widget wid,
        int offset,		/* unused */
        XtArgVal *value )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;

	sbw->scrollBar.flags |= VALUE_SET_FLAG;
	*value = (XtArgVal)sbw->scrollBar.value;
	return(XmSYNTHETIC_LOAD);
}


/*********************************************************************
 *
 * ProcessingDirectionDefault
 *    This procedure provides the dynamic default behavior for
 *    the processing direction resource dependent on the orientation.
 *
 *********************************************************************/
static void
ProcessingDirectionDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
	static unsigned char direction;

	value->addr = (XPointer) &direction;

	if (widget->scrollBar.orientation == XmHORIZONTAL)
        {
           if (LayoutIsRtoLP(widget))
	     direction = XmMAX_ON_LEFT;
           else
	     direction = XmMAX_ON_RIGHT;
        }
	else /* XmVERTICAL  -- range checking done during widget
		initialization */
	  direction = XmMAX_ON_BOTTOM;
}


/*********************************************************************
 *
 * BackgroundPixelDefault
 *    This procedure provides the dynamic default behavior for
 *    the background color. It looks to see if the parent is a
 *    ScrolledWindow, and if so, it uses the parent background.
 *    This is mostly for compatibility with 1.1 where the scrolledwindow
 *    was forcing its scrollbar color to its own background.
 *    Note that it works for both automatic and non automatic SW,
 *    which is a new feature for non automatic.
 *
 *********************************************************************/
static void
BackgroundPixelDefault(
        XmScrollBarWidget widget,
        int offset,
        XrmValue *value )
{
	static Pixel background;
	Widget parent = XtParent(widget) ;

	if (XmIsScrolledWindow(parent)) {
	    value->addr = (XPointer) &background;
	    background = parent->core.background_pixel;
	    return ;
	}

	/* else use the primitive defaulting mechanism */

	_XmBackgroundColorDefault((Widget )widget, offset, value);
}

/*********************************************************************
 *
 * TraversalDefault
 *    This procedure provides the dynamic default behavior for
 *    the traversal. It looks to see if the parent is a
 *    ScrolledWindow, and if so, it sets it to On.
 *    This is mostly for compatibility with 1.1 where the scrolledwindow
 *    was forcing its scrollbar traversal to On
 *    Note that it works only for automatic.
 *
 *********************************************************************/
static void
TraversalDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static Boolean traversal ;
      Widget parent = XtParent(widget) ;
      Arg al[1] ;
      unsigned char sp ;

      traversal = False ;
      value->addr = (XPointer) &traversal;

      if (XmIsScrolledWindow(parent)) {
          XtSetArg(al[0], XmNscrollingPolicy, &sp);
          XtGetValues(parent, al, 1);
          if (sp == XmAUTOMATIC) {
              traversal = True ;
              return ;
          }
      }
}



/*********************************************************************
 *
 * SliderVisualDefault
 *
 *
 *********************************************************************/
static void
SliderVisualDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum slider_visual ;

      value->addr = (XPointer) &slider_visual;

      if (widget->scrollBar.sliding_mode == XmTHERMOMETER) {
          slider_visual = XmTROUGH_COLOR ;
      } else {
	  slider_visual = XmSHADOWED_BACKGROUND ;
      }

}



/*********************************************************************
 *
 * SliderMarkDefault
 *
 *
 *********************************************************************/
static void
SliderMarkDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum slider_mark ;

      value->addr = (XPointer) &slider_mark;

      if ((widget->scrollBar.sliding_mode == XmTHERMOMETER) &&
	  (widget->scrollBar.editable))
	  slider_mark = XmROUND_MARK ;
      else
	  slider_mark = XmNONE ;
}



/*********************************************************************
 *
 * EditableDefault
 *
 *
 *********************************************************************/
static void
EditableDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum editable ;

      value->addr = (XPointer) &editable;

      if (widget->scrollBar.sliding_mode == XmTHERMOMETER) {
          editable = False ;
      } else {
	  editable = True ;
      }

}

/*********************************************************************
 *
 * HighlightDefault
 *    This procedure provides the dynamic default behavior for
 *    the highlight. It looks to see if the parent is a
 *    ScrolledWindow, and if so, it sets it to 2 or 1 depending on
 *    the enableThinThickness resource, otherwise, 0.
 *    Note that it works only for automatic.
 *
 *********************************************************************/
static void
HighlightDefault(
        XmScrollBarWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
    static Dimension highlight ;
    Widget parent = XtParent(widget) ;
    Arg al[1] ;
    unsigned char sp ;
    Boolean thinthickness = False;

    highlight = 0 ;
    value->addr = (XPointer) &highlight;

    if (XmIsScrolledWindow(parent)) {
	XtSetArg(al[0], XmNscrollingPolicy, &sp);
	XtGetValues(parent, al, 1);
	if (sp == XmAUTOMATIC) {
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(widget));
	    thinthickness = dpy->display.enable_thin_thickness;

	    if (thinthickness) {
		highlight = 1;
	    }
	    else {
		highlight = 2 ;
	    }
	    return ;
	}
    }
}




/*********************************************************************
 *
 *  ClassPartInitialize
 *     Initialize the fast subclassing.
 *
 *********************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
    _XmFastSubclassInit (wc, XmSCROLL_BAR_BIT);

    /* Install the navigator trait for all subclasses */
    XmeTraitSet((XtPointer)wc, XmQTnavigator, (XtPointer) &scrollBarNT);
}






/*********************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 *********************************************************************/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmScrollBarWidget request = (XmScrollBarWidget) rw ;
    XmScrollBarWidget new_w = (XmScrollBarWidget) nw ;

    Boolean default_value = FALSE;

    if(!XmRepTypeValidValue( XmRID_SHOW_ARROWS,
			    new_w->scrollBar.show_arrows, (Widget) new_w) )
	{
	    new_w->scrollBar.show_arrows = XmEACH_SIDE;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDER_VISUAL,
			    new_w->scrollBar.slider_visual, (Widget) new_w) )
	{
	    new_w->scrollBar.slider_visual = XmSHADOWED_BACKGROUND;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDER_MARK,
			    new_w->scrollBar.slider_mark, (Widget) new_w) )
	{
	    new_w->scrollBar.slider_mark = XmNONE;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDING_MODE,
			    new_w->scrollBar.sliding_mode, (Widget) new_w) )
	{
	    new_w->scrollBar.sliding_mode = XmSLIDER;
	}

     if (new_w->scrollBar.value == XmINVALID_DIMENSION)
	{
	    new_w->scrollBar.value = 0;
	    default_value = True;
	}

    /* Validate the incoming data  */

    if (new_w->scrollBar.minimum >= new_w->scrollBar.maximum)
	{
	    new_w->scrollBar.minimum = 0;
	    new_w->scrollBar.maximum = 100;
	    XmeWarning( (Widget) new_w, MESSAGE1);
	}

    if (new_w->scrollBar.slider_size == XmINVALID_DIMENSION)
	{
	    new_w->scrollBar.slider_size = (new_w->scrollBar.maximum
					    - new_w->scrollBar.minimum) / 10;
	    if (new_w->scrollBar.slider_size < 1)
		new_w->scrollBar.slider_size = 1;
	}

    if (new_w->scrollBar.slider_size < 1)
	{
	    new_w->scrollBar.slider_size = 1;
	    XmeWarning( (Widget) new_w, MESSAGE2);
	}

    if (new_w->scrollBar.slider_size >
	(new_w->scrollBar.maximum - new_w->scrollBar.minimum))
	{
	    new_w->scrollBar.slider_size = new_w->scrollBar.maximum
		- new_w->scrollBar.minimum;
	    XmeWarning( (Widget) new_w, MESSAGE13);
	}

    /* in thermo, slider_size is forced to be 0 */
    if (new_w->scrollBar.sliding_mode == XmTHERMOMETER)
	new_w->scrollBar.slider_size = 0 ;

    if (new_w->scrollBar.value < new_w->scrollBar.minimum)
	{
	    new_w->scrollBar.value = new_w->scrollBar.minimum;
	    if (!default_value) XmeWarning( (Widget) new_w, MESSAGE3);
	}

    if (new_w->scrollBar.value >
	new_w->scrollBar.maximum - new_w->scrollBar.slider_size)
	{
	    new_w->scrollBar.value = new_w->scrollBar.minimum;
	    if (!default_value) XmeWarning( (Widget) new_w, MESSAGE4);
	}

    if(    !XmRepTypeValidValue(XmRID_ORIENTATION,
				new_w->scrollBar.orientation, (Widget) new_w))
	{
	    new_w->scrollBar.orientation = XmVERTICAL;
	}

    if (new_w->scrollBar.orientation == XmHORIZONTAL)
	{
	    if ((new_w->scrollBar.processing_direction != XmMAX_ON_RIGHT) &&
		(new_w->scrollBar.processing_direction != XmMAX_ON_LEFT))

		{
		    new_w->scrollBar.processing_direction = XmMAX_ON_RIGHT;
		    XmeWarning( (Widget) new_w, MESSAGE6);
		}
	}
    else
	{
	    if ((new_w->scrollBar.processing_direction != XmMAX_ON_TOP) &&
		(new_w->scrollBar.processing_direction != XmMAX_ON_BOTTOM))
		{
		    new_w->scrollBar.processing_direction = XmMAX_ON_BOTTOM;
		    XmeWarning( (Widget) new_w, MESSAGE6);
		}
	}

    if (new_w->scrollBar.increment <= 0)
	{
	    new_w->scrollBar.increment = 1;
	    XmeWarning( (Widget) new_w, MESSAGE7);
	}

    if (new_w->scrollBar.page_increment <= 0)
	{
	    new_w->scrollBar.page_increment = 10;
	    XmeWarning( (Widget) new_w, MESSAGE8);
	}

    if (new_w->scrollBar.initial_delay <= 0)
	{
	    new_w->scrollBar.initial_delay = 250;
	    XmeWarning( (Widget) new_w, MESSAGE9);
	}

    if (new_w->scrollBar.repeat_delay <= 0)
	{
	    new_w->scrollBar.repeat_delay = 75;
	    XmeWarning( (Widget) new_w, MESSAGE10);
	}

    /*  Set up a geometry for the widget if it is currently 0.  */

    if (request->core.width == 0)
	{
	    if (new_w->scrollBar.orientation == XmHORIZONTAL)
		new_w->core.width += 100;
	    else
		new_w->core.width += 11;
	}
    if (request->core.height == 0)
	{
	    if (new_w->scrollBar.orientation == XmHORIZONTAL)
		new_w->core.height += 11;
	    else
		new_w->core.height += 100;
	}

    /*  Reverse the value for reverse processing.  */

    if (PROCESS_DIR_INVERSED(new_w))
	new_w->scrollBar.value = INVERSED_VALUE(new_w);

    /*  Set the internally used variables.  */

    new_w->scrollBar.flags = 0;
    if (new_w->scrollBar.slider_size < (new_w->scrollBar.maximum
					- new_w->scrollBar.minimum))
	{
	    new_w->scrollBar.flags |= SLIDER_AVAILABLE;

	    if (new_w->scrollBar.value > new_w->scrollBar.minimum)
		new_w->scrollBar.flags |= ARROW1_AVAILABLE;
	    if (new_w->scrollBar.value < (new_w->scrollBar.maximum
					  - new_w->scrollBar.slider_size))
		new_w->scrollBar.flags |= ARROW2_AVAILABLE;
	}
    else
	{
	    /*
	     * For correct setvalues processing, when the slider is
	     * unavailable, the arrows should be available.
	     */
	    new_w->scrollBar.flags |= ARROW1_AVAILABLE;
	    new_w->scrollBar.flags |= ARROW2_AVAILABLE;
	}

    new_w->scrollBar.pixmap = 0;
    new_w->scrollBar.sliding_on = FALSE;
    new_w->scrollBar.timer = 0;
    new_w->scrollBar.add_flags = 0 ;

    new_w->scrollBar.arrow_width = 0;
    new_w->scrollBar.arrow_height = 0;

    new_w->scrollBar.arrow1_x = 0;
    new_w->scrollBar.arrow1_y = 0;
    new_w->scrollBar.arrow1_selected = FALSE;

    new_w->scrollBar.arrow2_x = 0;
    new_w->scrollBar.arrow2_y = 0;
    new_w->scrollBar.arrow2_selected = FALSE;

    new_w->scrollBar.saved_value = new_w->scrollBar.value;

    if (LayoutIsRtoLP(new_w))
	new_w->scrollBar.flags &= ~VALUE_SET_FLAG;

    /*  Get the drawing graphics contexts.  */

    GetForegroundGC(new_w);
    GetUnavailableGC(new_w);
    GetFlatSliderGC(new_w);

    /* call the resize method to get an initial size */

    {
	XtWidgetProc resize;
	_XmProcessLock();
	resize = new_w->core.widget_class->core_class.resize;
	_XmProcessUnlock();

	(* (resize)) ((Widget) new_w);
    }

}




/************************************************************************
 *
 *  GetForegroundGC
 *     Get the graphics context used for drawing the slider and arrows.
 *
 ************************************************************************/
static void
GetForegroundGC(
        XmScrollBarWidget sbw )
{
    XGCValues values;
    XtGCMask  valueMask;

    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.foreground = sbw->core.background_pixel;
    values.background = sbw->primitive.foreground;
    values.graphics_exposures = False;

    sbw->scrollBar.foreground_GC = XtAllocateGC ((Widget) sbw, 0, valueMask,
						 &values, 0, GCFont);
}

/************************************************************************
 *
 *  GetFlatSliderGC
 *     Get the graphics context used for drawing the flat slider
 *
 ************************************************************************/
static void
GetFlatSliderGC(
        XmScrollBarWidget sbw )
{
    XGCValues values;
    XtGCMask  valueMask, unusedMask;

    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    unusedMask = GCFont | GCClipXOrigin | GCClipYOrigin;
    if (sbw->scrollBar.slider_visual == XmTROUGH_COLOR)
	values.foreground = sbw->scrollBar.trough_color;
    else
	values.foreground = sbw->primitive.foreground;
    values.background = sbw->core.background_pixel;
    values.graphics_exposures = False;

    sbw->scrollBar.flat_slider_GC = XtAllocateGC ((Widget) sbw, 0, valueMask,
						  &values, GCClipMask,
						  unusedMask);
}



/************************************************************************
 *
 *  GetUnavailableGC
 *     Get the graphics context used for drawing the slider and arrows
 *     as being unavailable.
 *
 ************************************************************************/
static void
GetUnavailableGC(
        XmScrollBarWidget sbw )
{
    XGCValues values;
    XtGCMask  valueMask, unusedMask;

    valueMask = GCForeground | GCBackground | GCGraphicsExposures |
	        GCFillStyle | GCStipple;
    unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;
    values.graphics_exposures = False;
    values.fill_style = FillStippled;
    values.background = sbw->core.background_pixel;
    values.foreground = sbw->primitive.foreground;

    values.stipple = _XmGetInsensitiveStippleBitmap((Widget) sbw);

    sbw->scrollBar.unavailable_GC = XtAllocateGC((Widget) sbw, 0, valueMask,
						 &values, GCClipMask,
						 unusedMask);
}




/************************************************************************
 *
 *  Logic of the scrollbar pixmap management:
 *  ----------------------------------------
 *     A pixmap the size of the trough area is created each time the
 *     scrollbar changes size.
 *     This pixmap receives the drawing of the slider which is then
 *     copied on the scrollbar window whenever exposure is needed.
 *     GetSliderPixmap:
 *         creates the pixmap and possibly free the current one if present.
 *         the pixmap is free upon destruction of the widget.
 *         the field pixmap == 0 means there is no pixmap to freed.
 *         Is called from Resize method.
 *     DrawSliderPixmap:
 *         draws the slider graphics (sized shadowed rectangle) in the pixmap.
 *         the fields slider_width and height must have been calculated.
 *         Is called from Resize, after the pixmap has been created,
 *           and from SetValues, if something has changed in the visual
 *           of the slider.
 *     RedrawSliderWindow:
 *         clears the current scrollbar slider area, computes the
 *         new position and call CopySliderInWindow.
 *         Is called from SetValues method, from increment actions, and
 *         from ChangeScrollBarValue (from Select, Timer).
 *    CopySliderInWindow:
 *         color slider case and then dump the slider pixmap using
 *         XCopyArea. Called from Redisplay and from Move, where
 *         the more expensive RedrawSliderWindow is not needed.
 *
 ************************************************************************/


/************************************************************************
 *
 *  GetSliderPixmap
 *     Create the new pixmap for the slider.
 *     This pixmap is the size of the widget minus the arrows.
 *
 ************************************************************************/
static void
GetSliderPixmap(
        XmScrollBarWidget sbw )
{

   if (sbw->scrollBar.pixmap)
      XFreePixmap (XtDisplay (sbw), sbw->scrollBar.pixmap);

   sbw->scrollBar.pixmap =
      XCreatePixmap (XtDisplay(sbw), RootWindowOfScreen(XtScreen(sbw)),
                     sbw->scrollBar.slider_area_width,
		     sbw->scrollBar.slider_area_height,
		     sbw->core.depth);
}





/************************************************************************
 *
 *  DrawSliderPixmap
 *     Draw the slider graphic into the pixmap.
 *     Draw the rectangle with a shadow or not and a mark in
 *     the middle or the side.
 *
 ************************************************************************/
static void
DrawSliderPixmap(
        XmScrollBarWidget sbw )
{
   register int slider_width = sbw->scrollBar.slider_width;
   register int slider_height = sbw->scrollBar.slider_height;
   register Drawable slider = sbw->scrollBar.pixmap;

   if ((sbw->scrollBar.slider_visual ==  XmFOREGROUND_COLOR) ||
       (sbw->scrollBar.slider_visual ==  XmTROUGH_COLOR)) {
	   /* we use the same GC, previously filled with either the
	      foreground or the trough_color pixel */
	   /* The trough area itself has been set, as the window background,
	      to either the trough color (not in that case) or the background
	      pixel */
	   XSetClipMask(XtDisplay((Widget) sbw),
			sbw->scrollBar.flat_slider_GC,
			None);
	   XFillRectangle (XtDisplay ((Widget) sbw), slider,
			   sbw->scrollBar.flat_slider_GC,
			   0, 0, slider_width, slider_height);
   } else
   if ((sbw->scrollBar.slider_visual == XmBACKGROUND_COLOR) ||
       (sbw->scrollBar.slider_visual == XmSHADOWED_BACKGROUND)) {

       /* in all other case, draw the shadow */
       XFillRectangle (XtDisplay ((Widget) sbw), slider,
		       sbw->scrollBar.foreground_GC,
		       0, 0, slider_width, slider_height);

       if (sbw->scrollBar.slider_visual == XmSHADOWED_BACKGROUND)
	   XmeDrawShadows (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC,
			   0, 0, slider_width, slider_height,
			   sbw->primitive.shadow_thickness,
			   XmSHADOW_OUT);
   }


   if (sbw->scrollBar.sliding_mode == XmTHERMOMETER) {
       /* in thermo mode, the mark must go on the side
	  of the slider, not in the middle.
	  We do that by modifying slider_width or _height
	  up front and share the same code thereafter */
       if (sbw->scrollBar.orientation == XmHORIZONTAL) {
	   if (PROCESS_DIR_INVERSED(sbw)) {
	       slider_width = THERMO_MARK_OFFSET ;
	   } else {
	       slider_width = 2 * slider_width - THERMO_MARK_OFFSET ;
	   }
       } else {
	   if (PROCESS_DIR_INVERSED(sbw)) {
	       slider_height = THERMO_MARK_OFFSET;
	   } else {
	       slider_height = 2 * slider_height - THERMO_MARK_OFFSET;
	   }
       }
   }

   if (sbw->scrollBar.slider_mark == XmETCHED_LINE) {

      if (sbw->scrollBar.orientation == XmHORIZONTAL) {
         XDrawLine (XtDisplay (sbw), slider,
                    sbw->primitive.bottom_shadow_GC,
                    slider_width / 2 - 1, 1,
                    slider_width / 2 - 1, slider_height - 2);
         XDrawLine (XtDisplay (sbw), slider,
                    sbw->primitive.top_shadow_GC,
                    slider_width / 2, 1,
                    slider_width / 2, slider_height - 2);
      } else {
         XDrawLine (XtDisplay (sbw), slider,
                    sbw->primitive.bottom_shadow_GC,
                    1, slider_height / 2 - 1,
                    slider_width - 2, slider_height / 2 - 1);
         XDrawLine (XtDisplay (sbw), slider,
                    sbw->primitive.top_shadow_GC,
                    1, slider_height / 2,
                    slider_width - 2, slider_height / 2);
      }
   } else

   if (sbw->scrollBar.slider_mark == XmTHUMB_MARK) {
       Dimension thumb_spacing = 4, margin = 2 ;

       if (sbw->scrollBar.orientation == XmHORIZONTAL) {
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   slider_width / 2, 0,
			   2, slider_height, 2, margin,
			   XmVERTICAL, XmSHADOW_ETCHED_OUT);
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   slider_width / 2 - thumb_spacing, 0,
			   2, slider_height, 2, margin,
			   XmVERTICAL, XmSHADOW_ETCHED_OUT);
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   slider_width / 2 + thumb_spacing, 0,
			   2, slider_height, 2, margin,
			   XmVERTICAL, XmSHADOW_ETCHED_OUT);
      }
      else
      {
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   0, slider_height / 2,
			   slider_width, 2, 2, margin,
			   XmHORIZONTAL, XmSHADOW_ETCHED_OUT);
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   0, slider_height / 2 - thumb_spacing,
			   slider_width, 2, 2, margin,
			   XmHORIZONTAL, XmSHADOW_ETCHED_OUT);
         XmeDrawSeparator (XtDisplay (sbw), slider,
			   sbw->primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC, NULL,
			   0, slider_height / 2 + thumb_spacing,
			   slider_width, 2, 2, margin,
			   XmHORIZONTAL, XmSHADOW_ETCHED_OUT);

      }
   }

   if (sbw->scrollBar.slider_mark == XmROUND_MARK) {
       Dimension radius = DEFAULT_ROUND_MARK_RADIUS ;

       XmeDrawCircle(XtDisplay (sbw), slider,
		     sbw->primitive.top_shadow_GC,
		     sbw->primitive.bottom_shadow_GC,
		     NULL,
		     slider_width / 2 - radius,
		     slider_height / 2 - radius,
		     2*radius, 2*radius,
		     sbw->primitive.shadow_thickness, 0);
   }

}

/************************************************************************
 *
 *  CopySliderInWindow
 *	Dump the slider pixmap into the window using CopyArea.
 *
 ************************************************************************/
static void
CopySliderInWindow(
        XmScrollBarWidget sbw )
{
    /* use the pixmap that contains the slider graphics */
    if (XtIsRealized((Widget)sbw) && sbw->scrollBar.pixmap) {
	XCopyArea (XtDisplay ((Widget) sbw),
		   sbw->scrollBar.pixmap, XtWindow ((Widget) sbw),
		   sbw->scrollBar.foreground_GC,
		   0, 0,
		   sbw->scrollBar.slider_width, sbw->scrollBar.slider_height,
		   sbw->scrollBar.slider_x, sbw->scrollBar.slider_y);
    }
}

/************************************************************************
 *
 *  RedrawSliderWindow
 *	Clear the trough area at the current slider position,
 *      recompute the slider coordinates and redraw the slider the window by
 *      copying from the pixmap graphics.
 *
 ************************************************************************/
static void
RedrawSliderWindow(
        XmScrollBarWidget sbw )
{
    short old_slider_width = sbw->scrollBar.slider_width ;
    short old_slider_height = sbw->scrollBar.slider_height ;

    if (XtIsRealized((Widget)sbw))
	XClearArea(XtDisplay ((Widget) sbw), XtWindow ((Widget) sbw),
		   (int) sbw->scrollBar.slider_area_x,
		   (int) sbw->scrollBar.slider_area_y,
		   (unsigned int) sbw->scrollBar.slider_area_width,
		   (unsigned int) sbw->scrollBar.slider_area_height,
		   (Bool) FALSE);

    CalcSliderRect(sbw,
		   &(sbw->scrollBar.slider_x),
		   &(sbw->scrollBar.slider_y),
		   &(sbw->scrollBar.slider_width),
		   &(sbw->scrollBar.slider_height));

    if ((old_slider_width != sbw->scrollBar.slider_width) ||
	(old_slider_height != sbw->scrollBar.slider_height))
	DrawSliderPixmap(sbw);

    CopySliderInWindow(sbw);
}




/************************************************************************
 *
 *  CalcSliderRect
 *     Calculate the slider location and size in pixels so that
 *     it can be drawn.  Note that number and location of pixels
 *     is always positive, so no special case rounding is needed.
 *     DD: better be a CalcSliderPosition and CalcSliderSize, since
 *         this routine is often use for one _or_ the other case.
 *
 ************************************************************************/
static void
CalcSliderRect(
        XmScrollBarWidget sbw,
        short *slider_x,
        short *slider_y,
        short *slider_width,
        short *slider_height )
{
	float range;
	float trueSize;
	float factor;
	float slideSize;
	int minSliderWidth;
	int minSliderHeight;
	int hitTheWall = 0;
	int value ;

	/* Set up */
	if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
		trueSize =  sbw->scrollBar.slider_area_width;
		minSliderWidth = MIN_SLIDER_LENGTH;
		if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
		    minSliderWidth = 1;
		minSliderHeight = MIN_SLIDER_THICKNESS;

	}
	else /* orientation == XmVERTICAL */
	{
		trueSize = sbw->scrollBar.slider_area_height;
		minSliderWidth = MIN_SLIDER_THICKNESS;
		minSliderHeight = MIN_SLIDER_LENGTH;
		if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
		    minSliderHeight = 1;
	}

	/* Total number of user units displayed */
	range = sbw->scrollBar.maximum - sbw->scrollBar.minimum;

	/* A naive notion of pixels per user unit */
	factor = trueSize / range;

	if (PROCESS_DIR_INVERSED(sbw))
	    value = INVERSED_VALUE(sbw);
	else
	    value = sbw->scrollBar.value ;

	/* A naive notion of the size of the slider in pixels */
	/* in thermo, slider_size is 0 ans is ignored */
	if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
	    slideSize = (float) value * factor;
	else
	    slideSize = (float) (sbw->scrollBar.slider_size) * factor;



	/* NOTE SIDE EFFECT */
#define MAX_SCROLLBAR_DIMENSION(val, min)\
	((val) > (min)) ? (val) : (hitTheWall = min)


	/* Don't let the slider get too small */
	if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
		*slider_width = MAX_SCROLLBAR_DIMENSION(
			(int) (slideSize + 0.5), minSliderWidth);
		*slider_height = MAX(sbw->scrollBar.slider_area_height,
			minSliderHeight);
	}
	else /* orientation == XmVERTICAL */
	{
		*slider_width = MAX(sbw->scrollBar.slider_area_width,
			minSliderWidth);
		*slider_height = MAX_SCROLLBAR_DIMENSION((int)
			(slideSize + 0.5), minSliderHeight);
	}

	if (hitTheWall)
	{
		/*
		 * The slider has not been allowed to take on its true
		 * proportionate size (it would have been too small).  This
		 * breaks proportionality of the slider and the conversion
		 * between pixels and user units.
		 *
		 * The factor needs to be tweaked in this case.
		 */

		trueSize -= hitTheWall; /* actual pixels available */
		range -= sbw->scrollBar.slider_size; /* actual range */
	        if (range == 0) range = 1;
		factor = trueSize / range;

	}

	if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
		/* Many parentheses to explicitly control type conversion. */
		if (sbw->scrollBar.sliding_mode == XmTHERMOMETER) {
		    if (PROCESS_DIR_INVERSED(sbw)) {
			*slider_x = sbw->scrollBar.slider_area_x +
			    sbw->scrollBar.slider_area_width - *slider_width;
		    } else {
			*slider_x = sbw->scrollBar.slider_area_x;
		    }
		} else
		    *slider_x = ((int) (((((float) sbw->scrollBar.value)
			- ((float) sbw->scrollBar.minimum)) * factor) + 0.5))
			+ sbw->scrollBar.slider_area_x;
		*slider_y = sbw->scrollBar.slider_area_y ;
	}
	else
	{
		*slider_x = sbw->scrollBar.slider_area_x;
		if (sbw->scrollBar.sliding_mode == XmTHERMOMETER) {
		    if (PROCESS_DIR_INVERSED(sbw)) {
			*slider_y = sbw->scrollBar.slider_area_y +
			    sbw->scrollBar.slider_area_height - *slider_height;
		    } else {
			*slider_y = sbw->scrollBar.slider_area_y ;
		    }
		} else
		    *slider_y = ((int) (((((float) sbw->scrollBar.value)
			- ((float) sbw->scrollBar.minimum)) * factor) + 0.5))
			+ sbw->scrollBar.slider_area_y;
	}

	/* One final adjustment (of questionable value--preserved
	   for visual backward compatibility) */

	if ((sbw->scrollBar.orientation == XmHORIZONTAL)
		&&
		((*slider_x + *slider_width) > (sbw->scrollBar.slider_area_x
			+ sbw->scrollBar.slider_area_width)))
	{
		*slider_x = sbw->scrollBar.slider_area_x
			+ sbw->scrollBar.slider_area_width - *slider_width;
	}

	if ((sbw->scrollBar.orientation == XmVERTICAL)
		&&
		((*slider_y + *slider_height) > (sbw->scrollBar.slider_area_y
			+ sbw->scrollBar.slider_area_height)))
	{
		*slider_y = sbw->scrollBar.slider_area_y
			+ sbw->scrollBar.slider_area_height - *slider_height;
	}
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
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;


    if (sbw->primitive.shadow_thickness > 0)
	XmeDrawShadows (XtDisplay (sbw), XtWindow (sbw),
		      sbw->primitive.bottom_shadow_GC,
		      sbw->primitive.top_shadow_GC,
		      sbw->primitive.highlight_thickness,
		      sbw->primitive.highlight_thickness,
		      sbw->core.width-2 *
		        sbw->primitive.highlight_thickness,
		      sbw->core.height-2 *
		        sbw->primitive.highlight_thickness,
		      sbw->primitive.shadow_thickness,
		      XmSHADOW_OUT);

    /* dump the pixmap that contains the slider graphics */
    CopySliderInWindow(sbw);

    if (sbw -> scrollBar.show_arrows) {

	DRAWARROW(sbw, ((sbw->scrollBar.arrow1_selected)?
		 sbw -> primitive.bottom_shadow_GC:
		 sbw -> primitive.top_shadow_GC),
		((sbw->scrollBar.arrow1_selected)?
		 sbw -> primitive.top_shadow_GC :
		 sbw -> primitive.bottom_shadow_GC),
		sbw->scrollBar.arrow1_x,
		sbw->scrollBar.arrow1_y,
		sbw->scrollBar.arrow1_orientation);
	DRAWARROW(sbw, ((sbw->scrollBar.arrow2_selected)?
		 sbw -> primitive.bottom_shadow_GC:
		 sbw -> primitive.top_shadow_GC),
		((sbw->scrollBar.arrow2_selected)?
		 sbw -> primitive.top_shadow_GC :
		 sbw -> primitive.bottom_shadow_GC),
		sbw->scrollBar.arrow2_x,
		sbw->scrollBar.arrow2_y,
		sbw->scrollBar.arrow2_orientation);
  }

    if (!(XtIsSensitive(wid))) {
        XSetClipMask(XtDisplay(sbw), sbw->scrollBar.unavailable_GC, None);
	XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
		       sbw->scrollBar.unavailable_GC,
		       sbw->primitive.highlight_thickness
		       + sbw->primitive.shadow_thickness,
		       sbw->primitive.highlight_thickness
		       + sbw->primitive.shadow_thickness,
		       XtWidth(sbw) - (2 * (sbw->primitive.highlight_thickness
				+ sbw->primitive.shadow_thickness)),
		       XtHeight(sbw) - (2 * (sbw->primitive.highlight_thickness
				+ sbw->primitive.shadow_thickness)));
    }
#ifdef FUNKY_INSENSITIVE_VISUAL
    else if (sbw->scrollBar.show_arrows)
    {
        XSetClipMask(XtDisplay(sbw), sbw->scrollBar.unavailable_GC, None);
        if (!(sbw->scrollBar.flags & ARROW1_AVAILABLE))
        {
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);
        }
        if (!(sbw->scrollBar.flags & ARROW2_AVAILABLE))
        {
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);
        }
    }
#endif


    /* envelop primitive expose method for highlight */
    {
	XtExposeProc expose;

	_XmProcessLock();
	expose = xmPrimitiveClassRec.core_class.expose;
	_XmProcessUnlock();

	(*(expose))(wid, event, region) ;
    }

}





/************************************************************************
 *
 *  Resize
 *     Process resizes on the widget by destroying and recreating the
 *     slider pixmap.
 *     Also draw the correct sized slider onto this pixmap.
 *
 ************************************************************************/
static void
Resize(
        Widget wid )
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
    register int ht = sbw->primitive.highlight_thickness;
    register int st = sbw->primitive.shadow_thickness;

#define CHECK(x) if (x <= 0) x = 1

#define BOTH_ARROWS_NEAR_SIDE(sbw) \
	    (((sbw->scrollBar.show_arrows == XmMIN_SIDE) &&\
	      !PROCESS_DIR_INVERSED(sbw)) ||\
	     ((sbw->scrollBar.show_arrows == XmMAX_SIDE) &&\
	      PROCESS_DIR_INVERSED(sbw)))

#define BOTH_ARROWS_FAR_SIDE(sbw) \
	  (((sbw->scrollBar.show_arrows == XmMIN_SIDE) &&\
	    PROCESS_DIR_INVERSED(sbw)) ||\
	   ((sbw->scrollBar.show_arrows == XmMAX_SIDE) &&\
	    !PROCESS_DIR_INVERSED(sbw)))

#define ARROW1_NEAR_SIDE(sbw) \
	  ( (sbw->scrollBar.show_arrows == XmEACH_SIDE) ||\
	   BOTH_ARROWS_NEAR_SIDE (sbw) )

#define ARROW2_FAR_SIDE(sbw) \
	 ((sbw->scrollBar.show_arrows == XmEACH_SIDE) ||\
	  BOTH_ARROWS_FAR_SIDE (sbw) )

    /*  Calculate all of the internal data for slider */

    if (sbw->scrollBar.show_arrows) {

	if (sbw->scrollBar.orientation == XmHORIZONTAL) {

	    sbw->scrollBar.arrow1_orientation = XmARROW_LEFT;
	    sbw->scrollBar.arrow2_orientation = XmARROW_RIGHT;

	    /*  left arrow position and size  */

	    sbw->scrollBar.arrow1_y = ht + st;

	    sbw->scrollBar.arrow_width =
		sbw->scrollBar.arrow_height = sbw->core.height
		    - 2 * (ht + st);

	    if (ARROW1_NEAR_SIDE (sbw)) {
		 sbw->scrollBar.arrow1_x = ht + st;
	     } else {
		 sbw->scrollBar.arrow1_x = sbw->core.width -ht -st
		     - 2 * sbw->scrollBar.arrow_width;
	     }

	    if (sbw->core.width <
		2 * (sbw->scrollBar.arrow_width + ht + st)
		+ MIN_SLIDER_LENGTH + 2)
		sbw->scrollBar.arrow_width = (sbw->core.width
			 - (MIN_SLIDER_LENGTH + 2 + 2 * (ht + st))) / 2;

	    /*  slide area position and size  */

	    if (sbw->scrollBar.show_arrows == XmEACH_SIDE) {
		sbw->scrollBar.slider_area_x =
		    ht + st + sbw->scrollBar.arrow_width + 1;
	    } else
	    if (BOTH_ARROWS_NEAR_SIDE (sbw)) {
		sbw->scrollBar.slider_area_x =
		    ht + st + 2 * sbw->scrollBar.arrow_width + 2;
	    } else {
		sbw->scrollBar.slider_area_x = ht + st ;
	    }

	    sbw->scrollBar.slider_area_width =
		sbw->core.width
		    - 2 * (ht + st + sbw->scrollBar.arrow_width + 1);

	    if ((2*(ht+st)) > XtHeight(sbw))
		sbw->scrollBar.slider_area_y = XtHeight(sbw) / 2;
	    else
		sbw->scrollBar.slider_area_y = ht + st;

	    sbw->scrollBar.slider_area_height =
		sbw->core.height - 2 * (ht + st);


	    /*  right arrow position  */

	    if (ARROW2_FAR_SIDE(sbw)) {
		sbw->scrollBar.arrow2_x = ht + st
		    + sbw->scrollBar.arrow_width + 1 +
			sbw->scrollBar.slider_area_width + 1;
	    } else {
		sbw->scrollBar.arrow2_x = ht + st
		    + sbw->scrollBar.arrow_width ;
	    }

	    sbw->scrollBar.arrow2_y = ht + st;

	} else { /* VERTICAL */

	    sbw->scrollBar.arrow1_orientation = XmARROW_UP;
	    sbw->scrollBar.arrow2_orientation = XmARROW_DOWN;

	    /*  top arrow position and size  */

	    sbw->scrollBar.arrow1_x = ht + st;

	    sbw->scrollBar.arrow_width = sbw->scrollBar.arrow_height =
		sbw->core.width - 2 * (ht + st);

	    if (ARROW1_NEAR_SIDE(sbw)) {
		sbw->scrollBar.arrow1_y = ht + st;
	    } else {
		sbw->scrollBar.arrow1_y = sbw->core.height -ht -st
		    - 2 * sbw->scrollBar.arrow_height;
	    }

	    if (sbw->core.height <
		2 * (sbw->scrollBar.arrow_height + ht + st)
		+ MIN_SLIDER_LENGTH +2)
		sbw->scrollBar.arrow_height = (sbw->core.height
			- (MIN_SLIDER_LENGTH + 2 + 2 * (ht + st))) / 2;

	    /*  slide area position and size  */

	    if (sbw->scrollBar.show_arrows == XmEACH_SIDE) {
		sbw->scrollBar.slider_area_y =
		    ht + st + sbw->scrollBar.arrow_height + 1;
	    } else
	    if (BOTH_ARROWS_NEAR_SIDE (sbw)) {
		sbw->scrollBar.slider_area_y =
		    ht + st + 2 * sbw->scrollBar.arrow_height + 2 ;
	    } else {
		sbw->scrollBar.slider_area_y = ht + st ;
	    }

	    sbw->scrollBar.slider_area_height = sbw->core.height
		- 2 * (ht + st + sbw->scrollBar.arrow_height +1);

	    if ((2*(st+ht)) > XtWidth(sbw))
		sbw->scrollBar.slider_area_x = XtWidth(sbw) / 2;
	    else
		sbw->scrollBar.slider_area_x = ht + st;

	    sbw->scrollBar.slider_area_width = sbw->core.width
		- 2 * (ht + st);


	    /*  down arrow position  */
	    if (ARROW2_FAR_SIDE(sbw)) {
		sbw->scrollBar.arrow2_y = ht + st
		    + sbw->scrollBar.arrow_height + 1 +
			sbw->scrollBar.slider_area_height + 1;
	    } else {
		sbw->scrollBar.arrow2_y = ht + st
		    + sbw->scrollBar.arrow_height ;
	    }

	    sbw->scrollBar.arrow2_x = ht + st;
	}

	CHECK(sbw->scrollBar.arrow_height);
	CHECK(sbw->scrollBar.arrow_width);
    } else {
	sbw->scrollBar.arrow_width = 0;
	sbw->scrollBar.arrow_height = 0;

	if (sbw->scrollBar.orientation == XmHORIZONTAL) {
	    /*  slide area position and size  */

	    sbw->scrollBar.slider_area_x = ht + st;
	    sbw->scrollBar.slider_area_width = sbw->core.width
		- 2 * (ht + st);

	    if ((2*(ht+st)) > XtHeight(sbw))
		sbw->scrollBar.slider_area_y = XtHeight(sbw) / 2;
	    else
		sbw->scrollBar.slider_area_y = ht + st;
	    sbw->scrollBar.slider_area_height = sbw->core.height
		- 2 * (ht + st);
	} else {
	    /*  slide area position and size  */

	    sbw->scrollBar.slider_area_y = ht + st;
	    sbw->scrollBar.slider_area_height = sbw->core.height
		- 2 * (ht + st);

	    if ((2*(st+ht)) > XtWidth(sbw))
		sbw->scrollBar.slider_area_x = XtWidth(sbw) / 2;
	    else
		sbw->scrollBar.slider_area_x = ht + st;
	    sbw->scrollBar.slider_area_width = sbw->core.width
		- 2 * (ht + st);
	}
    }

    CHECK(sbw->scrollBar.slider_area_height);
    CHECK(sbw->scrollBar.slider_area_width);

    GetSliderPixmap (sbw); /* the size of the scrollbar window - arrows */

    CalcSliderRect(sbw,
		   &(sbw->scrollBar.slider_x),
		   &(sbw->scrollBar.slider_y),
		   &(sbw->scrollBar.slider_width),
		   &(sbw->scrollBar.slider_height));

    DrawSliderPixmap (sbw);
}




/*********************************************************************
 *
 * Realize
 *
 ********************************************************************/
static void
Realize(
        Widget wid,
        XtValueMask *window_mask,
        XSetWindowAttributes *window_attributes )
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;

    *window_mask |= CWBitGravity;
    window_attributes->bit_gravity = ForgetGravity;

    /* if we are in the slider color = trough color case, we need to
       get the regular background as the trough (= window) color,
       otherwise, we need to get the trough color */
    if (sbw->scrollBar.slider_visual != XmTROUGH_COLOR) {
	*window_mask |= CWBackPixel ;
	window_attributes->background_pixel = sbw->scrollBar.trough_color;
    }

    XtCreateWindow (wid, InputOutput, CopyFromParent, *window_mask,
		    window_attributes);
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
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;

    XtReleaseGC ((Widget) sbw, sbw->scrollBar.foreground_GC);
    XtReleaseGC ((Widget) sbw, sbw->scrollBar.unavailable_GC);
    XtReleaseGC ((Widget) sbw, sbw->scrollBar.flat_slider_GC);

    if (sbw->scrollBar.pixmap != 0)
	XFreePixmap (XtDisplay (sbw), sbw->scrollBar.pixmap);

    if (sbw->scrollBar.timer != 0)
     {
       XtRemoveTimeOut (sbw->scrollBar.timer);
       sbw->scrollBar.timer = 0;
     }
}




/************************************************************************
 *
 *  ValidateInputs
 *
 ************************************************************************/
static Boolean
ValidateInputs(
	       XmScrollBarWidget current,
	       XmScrollBarWidget request, /* unused */
	       XmScrollBarWidget new_w )
{
    Boolean returnFlag = TRUE;
    int value ;

    /* Validate the incoming data  */

    if (new_w->scrollBar.minimum >= new_w->scrollBar.maximum)
	{
	    new_w->scrollBar.minimum = current->scrollBar.minimum;
	    new_w->scrollBar.maximum = current->scrollBar.maximum;
	    XmeWarning( (Widget) new_w, MESSAGE1);
	    returnFlag = FALSE;
	}

    if (new_w->scrollBar.sliding_mode != current->scrollBar.sliding_mode) {
	if (new_w->scrollBar.sliding_mode != XmTHERMOMETER) {
	    new_w->scrollBar.slider_size = (new_w->scrollBar.maximum
					    - new_w->scrollBar.minimum) / 10;
	    if (new_w->scrollBar.slider_size < 1)
		new_w->scrollBar.slider_size = 1;
	} else
	    new_w->scrollBar.slider_size = 0 ;
    }

    if (new_w->scrollBar.sliding_mode != XmTHERMOMETER) {

	if (new_w->scrollBar.slider_size < 1) {
	    if ((new_w->scrollBar.maximum - new_w->scrollBar.minimum) <
		current->scrollBar.slider_size)
		new_w->scrollBar.slider_size = new_w->scrollBar.maximum
		    - new_w->scrollBar.minimum;
	    else
		new_w->scrollBar.slider_size = current->scrollBar.slider_size;
	    XmeWarning( (Widget) new_w, MESSAGE2);
	    returnFlag = FALSE;
	}

	if ((new_w->scrollBar.slider_size >
	     new_w->scrollBar.maximum - new_w->scrollBar.minimum)) {
	    if ((new_w->scrollBar.maximum - new_w->scrollBar.minimum) <
		current->scrollBar.slider_size)
		new_w->scrollBar.slider_size = new_w->scrollBar.maximum
		    - new_w->scrollBar.minimum;
	    else
		new_w->scrollBar.slider_size = current->scrollBar.slider_size;
	    XmeWarning( (Widget) new_w, MESSAGE13);
	    returnFlag = FALSE;
	}
    } else
	new_w->scrollBar.slider_size = 0 ;

    if (new_w->scrollBar.value < new_w->scrollBar.minimum)
	{
	    new_w->scrollBar.value = new_w->scrollBar.minimum;
	    XmeWarning( (Widget) new_w, MESSAGE3);
	    returnFlag = FALSE;
	}

    /* do the checking on the real user value */
    if (new_w->scrollBar.value == current->scrollBar.value) {
	if (PROCESS_DIR_INVERSED(new_w))
	    /* use new for value since that's the one getting changed below */
	    value = INVERSED_VALUE(new_w);
	else
	    value = new_w->scrollBar.value ;
    } else
	value = new_w->scrollBar.value ;

    if (value > new_w->scrollBar.maximum - new_w->scrollBar.slider_size)
	{
	    new_w->scrollBar.value =
		new_w->scrollBar.maximum - new_w->scrollBar.slider_size;
	    new_w->scrollBar.flags |= VALUE_SET_FLAG;
	    XmeWarning( (Widget) new_w, MESSAGE4);
	}

    if(  !XmRepTypeValidValue( XmRID_ORIENTATION,
			      new_w->scrollBar.orientation, (Widget) new_w))
	{
	    new_w->scrollBar.orientation = current->scrollBar.orientation;
	    returnFlag = FALSE;
	}

    if (new_w->scrollBar.orientation == XmHORIZONTAL)
	{
	    if ((new_w->scrollBar.processing_direction != XmMAX_ON_LEFT) &&
		(new_w->scrollBar.processing_direction != XmMAX_ON_RIGHT))
		{
		    new_w->scrollBar.processing_direction =
			current->scrollBar.processing_direction;
		    XmeWarning( (Widget) new_w, MESSAGE6);
		    returnFlag = FALSE;
		}
	}
    else /* new_w->scrollBar.orientation == XmVERTICAL */
	{
	    if ((new_w->scrollBar.processing_direction != XmMAX_ON_TOP) &&
		(new_w->scrollBar.processing_direction != XmMAX_ON_BOTTOM))
		{
		    new_w->scrollBar.processing_direction =
			current->scrollBar.processing_direction;
		    XmeWarning( (Widget) new_w, MESSAGE6);
		    returnFlag = FALSE;
		}
	}

    if (new_w->scrollBar.increment <= 0)
	{
	    new_w->scrollBar.increment = current->scrollBar.increment;
	    XmeWarning( (Widget) new_w, MESSAGE7);
	    returnFlag = FALSE;
	}

    if (new_w->scrollBar.page_increment <= 0)
	{
	    new_w->scrollBar.page_increment =
		current->scrollBar.page_increment;
	    XmeWarning( (Widget) new_w,  MESSAGE8);
	    returnFlag = FALSE;
	}

    if (new_w->scrollBar.initial_delay <= 0)
	{
	    new_w->scrollBar.initial_delay = current->scrollBar.initial_delay;
	    XmeWarning( (Widget) new_w, MESSAGE9);
	    returnFlag = FALSE;
	}

    if (new_w->scrollBar.repeat_delay <= 0)
	{
	    new_w->scrollBar.repeat_delay = current->scrollBar.repeat_delay;
	    XmeWarning( (Widget) new_w, MESSAGE10);
	    returnFlag = FALSE;
	}

    if (new_w->core.width == 0)
	{
	    if (new_w->scrollBar.orientation == XmHORIZONTAL)
		new_w->core.width += 100;
	    else
		new_w->core.width += 11;
	}

    if (new_w->core.height == 0)
	{
	    if (new_w->scrollBar.orientation == XmHORIZONTAL)
		new_w->core.height += 11;
	    else
		new_w->core.height += 100;
	}

    return(returnFlag);
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
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmScrollBarWidget current = (XmScrollBarWidget) cw ;
    XmScrollBarWidget request = (XmScrollBarWidget) rw ;
    XmScrollBarWidget new_w = (XmScrollBarWidget) nw ;
    Boolean returnFlag = FALSE;
    Boolean current_backwards = PROCESS_DIR_INVERSED(current);
    Boolean new_backwards = PROCESS_DIR_INVERSED(new_w);



    if(!XmRepTypeValidValue( XmRID_SHOW_ARROWS,
			    new_w->scrollBar.show_arrows, (Widget) new_w) )
	{
	    new_w->scrollBar.show_arrows = current->scrollBar.sliding_mode;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDING_MODE,
			    new_w->scrollBar.sliding_mode, (Widget) new_w) )
	{
	    new_w->scrollBar.sliding_mode = current->scrollBar.sliding_mode;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDER_VISUAL,
			    new_w->scrollBar.slider_visual, (Widget) new_w) )
	{
	    new_w->scrollBar.slider_visual = current->scrollBar.slider_visual;
	}

    if(!XmRepTypeValidValue( XmRID_SLIDER_MARK,
			    new_w->scrollBar.slider_mark, (Widget) new_w) )
	{
	    new_w->scrollBar.slider_mark = current->scrollBar.slider_mark;
	}

    if (new_w->scrollBar.orientation == XmHORIZONTAL)
      {
	if (new_w->scrollBar.processing_direction == XmMAX_ON_LEFT &&
	    !(new_w->scrollBar.flags & VALUE_SET_FLAG) &&
	    ((new_w->scrollBar.slider_size != current->scrollBar.slider_size) ||
	     (new_w->scrollBar.maximum != current->scrollBar.maximum) ||
	     (new_w->scrollBar.minimum != current->scrollBar.minimum) ))

	  {
	    new_w->scrollBar.value = (new_w->scrollBar.maximum
				      + new_w->scrollBar.minimum
				      - new_w->scrollBar.slider_size) -
					  INVERSED_VALUE(current);
	    new_backwards = FALSE;
	    current_backwards = FALSE;
	  }
      }


    /* Make sure that processing direction tracks orientation */

    if ((new_w->scrollBar.orientation != current->scrollBar.orientation)
	&&
	(new_w->scrollBar.processing_direction ==
	 current->scrollBar.processing_direction))
	{
	    if ((new_w->scrollBar.orientation == XmHORIZONTAL) &&
		(current->scrollBar.processing_direction == XmMAX_ON_TOP))
		new_w->scrollBar.processing_direction = XmMAX_ON_LEFT;
	    else if ((new_w->scrollBar.orientation == XmHORIZONTAL) &&
		     (current->scrollBar.processing_direction ==
		      XmMAX_ON_BOTTOM))
		new_w->scrollBar.processing_direction = XmMAX_ON_RIGHT;
	    else if ((new_w->scrollBar.orientation == XmVERTICAL) &&
		     (current->scrollBar.processing_direction == XmMAX_ON_LEFT))
		new_w->scrollBar.processing_direction = XmMAX_ON_TOP;
	    else if ((new_w->scrollBar.orientation == XmVERTICAL) &&
		     (current->scrollBar.processing_direction == XmMAX_ON_RIGHT))
		new_w->scrollBar.processing_direction = XmMAX_ON_BOTTOM;
	}

    while (!ValidateInputs(current, request, new_w)) /*EMPTY*/;

    /*
     * Because someone somewhere originally thought that it was clever
     * for the scrollbar widget to do all of its internal processing in
     * just one direction, all of the interface procedures have to go
     * through extreme gymnastics to support reversal.
     */
    if ((new_backwards && !current_backwards) ||
	(!new_backwards && current_backwards))
	{
	    if (new_w->scrollBar.flags & VALUE_SET_FLAG)
		{
		    if (new_backwards)
			new_w->scrollBar.value = INVERSED_VALUE(new_w);
		}
	    else
		{
		    new_w->scrollBar.value = INVERSED_VALUE(new_w);
		}
	}
    else
	{
	    if ((new_w->scrollBar.flags & VALUE_SET_FLAG) &&
		(new_backwards))
		new_w->scrollBar.value = INVERSED_VALUE(new_w);
	}

    if (new_w->scrollBar.flags & VALUE_SET_FLAG)
	new_w->scrollBar.flags &= ~VALUE_SET_FLAG;

    /*  See if the GC needs to be regenerated  */

    if (new_w->core.background_pixel != current->core.background_pixel)
	{
	    XtReleaseGC((Widget) new_w, new_w->scrollBar.foreground_GC);
	    GetForegroundGC(new_w);
	}

    if (((new_w->scrollBar.slider_visual == XmTROUGH_COLOR) &&
	(new_w->scrollBar.trough_color != current->scrollBar.trough_color)) ||
	((new_w->scrollBar.slider_visual == XmFOREGROUND_COLOR) &&
	 (new_w->primitive.foreground != current->primitive.foreground)))
	{
	    XtReleaseGC((Widget) new_w, new_w->scrollBar.flat_slider_GC);
	    GetFlatSliderGC(new_w);
	}

    /*
     * See if the trough (a.k.a the window background) needs to be
     * changed to use a different pixel.
     */
    if (XtIsRealized(nw)) {
	Pixel change_to = XmUNSPECIFIED_PIXEL ;
	/* slider_visual == XmTROUGH_COLOR is the case where the
	   window background is the real core.background_pixel, all the
	   other use the trough_color as the window background */

	if ((new_w->scrollBar.slider_visual == XmTROUGH_COLOR) &&
	    (current->scrollBar.slider_visual != XmTROUGH_COLOR)) {
	    /* no need to care for background change since Core did it */
	    change_to = new_w->core.background_pixel;
	}
	if ((new_w->scrollBar.slider_visual != XmTROUGH_COLOR) &&
	    ((current->scrollBar.slider_visual == XmTROUGH_COLOR) ||
	     (new_w->scrollBar.trough_color != current->scrollBar.trough_color) ||
	     /* if the background had changed, Core has certainly reset the
		window background already, so we need to undo that */
	     (new_w->core.background_pixel != current->core.background_pixel))) {
	    change_to = new_w->scrollBar.trough_color ;
	}
	if (change_to != XmUNSPECIFIED_PIXEL) {
	    returnFlag = TRUE;
	    XtReleaseGC((Widget) new_w, new_w->scrollBar.flat_slider_GC);
	    GetFlatSliderGC(new_w);
	    XSetWindowBackground(XtDisplay((Widget)new_w),
				 XtWindow((Widget)new_w), change_to);
	}
    }

    /*
     * See if the widget needs to be redrawn.  Minimize the amount
     * of redraw by having specific checks.
     */

    if ((new_w->scrollBar.orientation !=
	 current->scrollBar.orientation)         ||
	(new_w->primitive.shadow_thickness !=
	 current->primitive.shadow_thickness)    ||
	(new_w->primitive.highlight_thickness !=
	 current->primitive.highlight_thickness) ||
	(new_w->scrollBar.show_arrows !=
	 current->scrollBar.show_arrows))
	{
	    /* call Resize method, that will have the effect of
	       recomputing all the internal variables (arrow size,
	       trough are) and recreating the slider pixmap. */
	    XtWidgetProc resize;
	    _XmProcessLock();
	    resize = new_w->core.widget_class->core_class.resize;
	    _XmProcessUnlock();

	    (* (resize)) ((Widget) new_w);
	    returnFlag = TRUE;
	}

    if ((new_w->primitive.foreground !=
	 current->primitive.foreground)
	||
	(new_w->core.background_pixel != current->core.background_pixel)
	||
	(new_w->primitive.top_shadow_color !=
	 current->primitive.top_shadow_color)
	||
	(new_w->scrollBar.slider_visual !=
	 current->scrollBar.slider_visual)
	||
	(new_w->scrollBar.slider_mark !=
	 current->scrollBar.slider_mark)
	||
	(new_w->scrollBar.trough_color !=
	 current->scrollBar.trough_color)
	||
	(new_w->primitive.bottom_shadow_color !=
	 current->primitive.bottom_shadow_color))
	{
	    returnFlag = TRUE;
	    /* only draw the slider graphics, no need to change the
	       pixmap (call to GetSliderPixmap) nor the slider size
	       (call to CalcSliderRect). */
	    DrawSliderPixmap(new_w);

	}

    if ((new_w->scrollBar.slider_size !=
	 current->scrollBar.slider_size)                    ||
	(new_w->scrollBar.minimum != current->scrollBar.minimum) ||
	(new_w->scrollBar.maximum != current->scrollBar.maximum) ||
	(new_w->scrollBar.processing_direction !=
	 current->scrollBar.processing_direction)) {

	/* have to clear the current slider before setting the
	   new slider position and size */
	if (XtIsRealized(nw))
	    XClearArea(XtDisplay((Widget)new_w),
		       XtWindow((Widget)new_w),
		       new_w->scrollBar.slider_x,
		       new_w->scrollBar.slider_y,
		       new_w->scrollBar.slider_width,
		       new_w->scrollBar.slider_height, False);

	/* recompute the slider size and draw in the pixmap */
	CalcSliderRect(new_w,
		       &(new_w->scrollBar.slider_x),
		       &(new_w->scrollBar.slider_y),
		       &(new_w->scrollBar.slider_width),
		       &(new_w->scrollBar.slider_height));

	/* redraw the slider in the pixmap */
	DrawSliderPixmap (new_w);

	if (new_w->scrollBar.slider_size >= (new_w->scrollBar.maximum
					     - new_w->scrollBar.minimum))
	    {
		new_w->scrollBar.flags &= ~SLIDER_AVAILABLE;
		/*
		 * Disabling the slider enables the arrows.  This
		 * leaves the scrollbar in a state amenable to reenabling
		 * the slider.
		 */
		new_w->scrollBar.flags |= ARROW1_AVAILABLE;
		new_w->scrollBar.flags |= ARROW2_AVAILABLE;
		returnFlag = TRUE;
	    }
	else
	    {
		if (! (new_w->scrollBar.flags & SLIDER_AVAILABLE)) {
		    returnFlag = TRUE;
		    new_w->scrollBar.flags |= SLIDER_AVAILABLE;
		} else {
		    /* directly use the pixmap that contains the slider
		       graphics, no need to call RedrawSliderWindow since the
		       cleararea and the calcrect have already been made */
		    CopySliderInWindow(new_w);
		}
	    }
    }


    if (new_w->scrollBar.value != current->scrollBar.value) {
	/* the value has changed, the slider needs to move. */
	RedrawSliderWindow (new_w);

	if (XtIsRealized(nw))
	{
	/* Following lines taken from Redisplay code; the XmNvalue can change
        ** even when the widget is insensitive. Other paths through the code
	** involve user interaction and so sensitivity doesn't need to be
	** considered.
	** NOTE! doesn't deal with FUNKY_INSENSITIVE_VISUAL
	*/
	    if (!(XtIsSensitive((Widget)new_w))) {
		XmScrollBarWidget sbw = (XmScrollBarWidget) new_w;
		XSetClipMask(XtDisplay(sbw), sbw->scrollBar.unavailable_GC, None);
		XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
			       sbw->scrollBar.unavailable_GC,
			       sbw->primitive.highlight_thickness
			       + sbw->primitive.shadow_thickness,
			       sbw->primitive.highlight_thickness
			       + sbw->primitive.shadow_thickness,
			       XtWidth(sbw) - (2 * (sbw->primitive.highlight_thickness
					+ sbw->primitive.shadow_thickness)),
			       XtHeight(sbw) - (2 * (sbw->primitive.highlight_thickness
					+ sbw->primitive.shadow_thickness)));
	    }
	}
    }

    if (XtIsSensitive(nw) != XtIsSensitive(cw))
	returnFlag = TRUE;

    return(returnFlag);
}




/************************************************************************
 *
 *  CalcSliderVal
 *     Calculate the slider val in application coordinates given
 *     the input x and y.
 *
 ************************************************************************/
static int
CalcSliderVal(
        XmScrollBarWidget sbw,
        int x,
        int y )
{
	float range;
	float trueSize;       /* size of slider area in pixels */
	float referencePoint; /* origin of slider */
	float proportion;
	int int_proportion;
	int slider_area_origin;


	if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
	    referencePoint = (float) x - sbw->scrollBar.separation_x;
	    trueSize = sbw->scrollBar.slider_area_width;
	    if (sbw->scrollBar.sliding_mode != XmTHERMOMETER)
		trueSize -= sbw->scrollBar.slider_width;
	    slider_area_origin = sbw->scrollBar.slider_area_x;
	}
	else
	{
	    referencePoint = (float) y - sbw->scrollBar.separation_y;
	    trueSize = sbw->scrollBar.slider_area_height;
	    if (sbw->scrollBar.sliding_mode != XmTHERMOMETER)
		trueSize -= sbw->scrollBar.slider_height;
	    slider_area_origin = sbw->scrollBar.slider_area_y;
	}

	if (trueSize > 0)

	    /* figure the proportion of slider area between the origin
	       of the slider area and the origin of the slider. */
	    proportion = (referencePoint - slider_area_origin
		 + (((sbw->scrollBar.show_arrows == XmEACH_SIDE) &&
		    (sbw->scrollBar.sliding_mode != XmTHERMOMETER))?1:0)) /
			      trueSize;
	else
		/*
		 * We've got an interesting problem here.  There isn't any
		 * slider area available to slide in.  What should the value
		 * of the scrollbar be when the user tries to drag the slider?
		 *
		 * Setting proportion to 1 snaps to maximum.  Setting
		 * proportion to the reciprocal of "range" will cause the
		 * slider to snap to the minimum.
		 *
		 */
		proportion = 1;

	/* Actual range displayed */
	range = sbw->scrollBar.maximum - sbw->scrollBar.minimum
		- sbw->scrollBar.slider_size;

	/* Now scale the proportion in pixels to user units */
	proportion = (proportion * range)
		+ ((float) sbw->scrollBar.minimum);

	/* Round off appropriately */
	if (proportion > 0)
		proportion += 0.5;
	else if (proportion < 0)
		proportion -= 0.5;

	int_proportion = (int) proportion;

	if (int_proportion < sbw->scrollBar.minimum)
		int_proportion = sbw->scrollBar.minimum;
	else if (int_proportion > (sbw->scrollBar.maximum
			- sbw->scrollBar.slider_size))
		int_proportion = sbw->scrollBar.maximum
			- sbw->scrollBar.slider_size;

	return (int_proportion);
}




/************************************************************************
 *
 *  Select
 *     This function processes selections occuring on the scrollBar.
 *
 ************************************************************************/
static void
Select(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
    XButtonPressedEvent *buttonEvent = (XButtonPressedEvent *) event ;
    int slider_x = sbw->scrollBar.slider_x;
    int slider_y = sbw->scrollBar.slider_y;
    int slider_width = sbw->scrollBar.slider_width;
    int slider_height = sbw->scrollBar.slider_height;
    Boolean slider_moved;

    if (!sbw->scrollBar.editable) return ;


    /* add a start update when the button is pressed
       so that scrollbar moves generating widget
       configurations be bracketed for dropsite update.
       The endupdate is done in Release */

    XmDropSiteStartUpdate(wid);

    sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

#ifndef DEBUG_NO_SB_GRAB
    if (XtGrabKeyboard(wid, False, GrabModeAsync,
		       GrabModeAsync, buttonEvent->time) == GrabSuccess)
	sbw->scrollBar.flags |= KEYBOARD_GRABBED;
#endif

    XAllowEvents(XtDisplay(wid), AsyncPointer, CurrentTime);
    XAllowEvents(XtDisplay(wid), AsyncKeyboard, CurrentTime);

    if (!(sbw->scrollBar.flags & SLIDER_AVAILABLE))
	return;
    if ((buttonEvent->button == Button1) &&
	(!XmIsScrolledWindow(XtParent(wid))))
	(void) XmProcessTraversal( (Widget) sbw, XmTRAVERSE_CURRENT);

    sbw->scrollBar.separation_x = 0;
    sbw->scrollBar.separation_y = 0;


    if ((sbw->scrollBar.orientation == XmHORIZONTAL) &&
	(buttonEvent->y >= slider_y)                      &&
	(buttonEvent->y <= slider_y + slider_height)  &&
	(buttonEvent->button == Button1) &&
	(sbw->scrollBar.sliding_mode == XmTHERMOMETER) &&
	(((PROCESS_DIR_INVERSED(sbw)) &&
	  (buttonEvent->x >= slider_x) &&
	  (buttonEvent->x <= slider_x + THERMO_MARK_OFFSET)) ||
	 (!PROCESS_DIR_INVERSED(sbw) &&
	  (buttonEvent->x <= slider_x + slider_width)   &&
	  (buttonEvent->x >= slider_x + slider_width - THERMO_MARK_OFFSET))))
	/* hack */
	buttonEvent->button = Button2 ;

    if ((sbw->scrollBar.orientation == XmVERTICAL) &&
	(buttonEvent->x >= slider_x) &&
	(buttonEvent->x <= slider_x + slider_width)   &&
	(buttonEvent->button == Button1) &&
	(sbw->scrollBar.sliding_mode == XmTHERMOMETER) &&
	(((PROCESS_DIR_INVERSED(sbw)) &&
	  (buttonEvent->y >= slider_y) &&
	  (buttonEvent->y <= slider_y + THERMO_MARK_OFFSET)) ||
	 (!PROCESS_DIR_INVERSED(sbw) &&
	  (buttonEvent->y >= slider_y + slider_height - THERMO_MARK_OFFSET) &&
	  (buttonEvent->y <= slider_y + slider_height))))
	/* hack */
	buttonEvent->button = Button2 ;

    /*  Calculate whether the selection point is in the slider  */
    if ((buttonEvent->x >= slider_x)                 &&
	(buttonEvent->x <= slider_x + slider_width)   &&
	(buttonEvent->y >= slider_y)                      &&
	(buttonEvent->y <= slider_y + slider_height) &&
	((buttonEvent->button != Button1) ||
	 (sbw->scrollBar.sliding_mode != XmTHERMOMETER)))
	{
	    sbw->scrollBar.initial_x = slider_x;
	    sbw->scrollBar.initial_y = slider_y;
	    sbw->scrollBar.sliding_on = True;
	    sbw->scrollBar.saved_value = sbw->scrollBar.value;
	    sbw->scrollBar.arrow1_selected = FALSE;
	    sbw->scrollBar.arrow2_selected = FALSE;

	    if ((buttonEvent->button == Button1) &&
		(sbw->scrollBar.sliding_mode != XmTHERMOMETER))
		{
		    sbw->scrollBar.separation_x = buttonEvent->x - slider_x;
		    sbw->scrollBar.separation_y = buttonEvent->y - slider_y;
		}
	    else  if (buttonEvent->button == Button2)
		{
		    /* Warp the slider to the cursor, and then drag */
		    if  (sbw->scrollBar.sliding_mode != XmTHERMOMETER) {
			if (sbw->scrollBar.orientation == XmHORIZONTAL)
			    sbw->scrollBar.separation_x =
				sbw->scrollBar.slider_width / 2;
			else
			    sbw->scrollBar.separation_y =
				sbw->scrollBar.slider_height / 2;
		    } else {
			sbw->scrollBar.separation_x = 0 ;
			sbw->scrollBar.separation_y = 0 ;
		    }
		    Moved ((Widget) sbw, (XEvent *) buttonEvent,
			   params, num_params);
		}

	    return;
	}

    /* ... in the trough (i.e. slider area)... */
    else if ((buttonEvent->x >= sbw->scrollBar.slider_area_x)   &&
	     (buttonEvent->y >= sbw->scrollBar.slider_area_y)        &&
	     (buttonEvent->x <= sbw->scrollBar.slider_area_x
	      + sbw->scrollBar.slider_area_width)                 &&
	     (buttonEvent->y <= sbw->scrollBar.slider_area_y
	      + sbw->scrollBar.slider_area_height)) {

	sbw->scrollBar.arrow1_selected = FALSE;
	sbw->scrollBar.arrow2_selected = FALSE;
	sbw->scrollBar.saved_value = sbw->scrollBar.value;

	if (buttonEvent->button == Button1) {
	    Position limit_x, limit_y ;

	    /* Page the slider up or down */
	    /* what is up or down depends on the processing direction... */

	    limit_x = sbw->scrollBar.slider_x ;
	    limit_y = sbw->scrollBar.slider_y ;
	    if  (sbw->scrollBar.sliding_mode == XmTHERMOMETER) {
		if (PROCESS_DIR_INVERSED(sbw)) {
		    limit_x = sbw->scrollBar.slider_area_width -
			sbw->scrollBar.slider_width ;
		    limit_y = sbw->scrollBar.slider_area_height -
			sbw->scrollBar.slider_height ;
		} else {
		    limit_x = sbw->scrollBar.slider_width ;
		    limit_y = sbw->scrollBar.slider_height ;
		}
	    }

	    if (sbw->scrollBar.orientation == XmHORIZONTAL) {
		if (buttonEvent->x < limit_x)
		    sbw->scrollBar.change_type = XmCR_PAGE_DECREMENT;
		else
		    sbw->scrollBar.change_type = XmCR_PAGE_INCREMENT;
	    }
	    else
		{
		    if (buttonEvent->y < limit_y)
			sbw->scrollBar.change_type = XmCR_PAGE_DECREMENT;
		    else
			sbw->scrollBar.change_type = XmCR_PAGE_INCREMENT;
		}
	    slider_moved = ChangeScrollBarValue(sbw);
	}
	else  /* Button2 */ {
		/* Warp the slider to the cursor, and then drag */

		 if  (sbw->scrollBar.sliding_mode != XmTHERMOMETER) {
			if (sbw->scrollBar.orientation == XmHORIZONTAL)
			    sbw->scrollBar.separation_x =
				sbw->scrollBar.slider_width / 2;
			else
			    sbw->scrollBar.separation_y =
				sbw->scrollBar.slider_height / 2;
		    } else {
			sbw->scrollBar.separation_x = 0 ;
			sbw->scrollBar.separation_y = 0 ;
		    }

		sbw->scrollBar.initial_x = slider_x;
		sbw->scrollBar.initial_y = slider_y;
		sbw->scrollBar.sliding_on = True;

		Moved ((Widget) sbw, (XEvent *) buttonEvent,
		       params, num_params);
		return;
	    }
    }

    /* ... in arrow 1 */
    else if ((buttonEvent->x >= sbw->scrollBar.arrow1_x)  &&
	     (buttonEvent->y >= sbw->scrollBar.arrow1_y)       &&
	     (buttonEvent->x <= sbw->scrollBar.arrow1_x
	      + sbw->scrollBar.arrow_width)                 &&
	     (buttonEvent->y <= sbw->scrollBar.arrow1_y
	      + sbw->scrollBar.arrow_height))
	{
	    sbw->scrollBar.change_type = XmCR_DECREMENT;
	    sbw->scrollBar.saved_value = sbw->scrollBar.value;
	    sbw->scrollBar.arrow1_selected = True;

	    slider_moved = ChangeScrollBarValue(sbw) ;
	    DRAWARROW(sbw, sbw->primitive.bottom_shadow_GC,
		      sbw -> primitive.top_shadow_GC,
		      sbw->scrollBar.arrow1_x,
		      sbw->scrollBar.arrow1_y,
		      sbw->scrollBar.arrow1_orientation);
	}

    /* ... in arrow 2 */
    else if ((buttonEvent->x >= sbw->scrollBar.arrow2_x)      &&
	     (buttonEvent->y >= sbw->scrollBar.arrow2_y)           &&
	     (buttonEvent->x <= sbw->scrollBar.arrow2_x
	      + sbw->scrollBar.arrow_width)                     &&
	     (buttonEvent->y <= sbw->scrollBar.arrow2_y
	      + sbw->scrollBar.arrow_height))
	{
	    sbw->scrollBar.change_type = XmCR_INCREMENT;
	    sbw->scrollBar.saved_value = sbw->scrollBar.value;
	    sbw->scrollBar.arrow2_selected = True;

	    slider_moved = ChangeScrollBarValue(sbw) ;
	    DRAWARROW(sbw, sbw->primitive.bottom_shadow_GC,
		      sbw -> primitive.top_shadow_GC,
		      sbw->scrollBar.arrow2_x,
		      sbw->scrollBar.arrow2_y,
		      sbw->scrollBar.arrow2_orientation);
	}
    else
	/* ... in the highlight area.  */
	return;

    if (slider_moved) {

	ScrollCallback (sbw, sbw->scrollBar.change_type,
			sbw->scrollBar.value, 0, 0, (XEvent *) buttonEvent);

	XSync (XtDisplay((Widget)sbw), False);

	sbw->scrollBar.flags |= FIRST_SCROLL_FLAG ;
	sbw->scrollBar.flags &= ~END_TIMER;


	if (!sbw->scrollBar.timer)
	    sbw->scrollBar.timer = XtAppAddTimeOut
		(XtWidgetToApplicationContext((Widget) sbw),
		 (unsigned long) sbw->scrollBar.initial_delay,
		 TimerEvent, (XtPointer) sbw);
    }
}




/************************************************************************
 *
 *  Release
 *     This function processes releases occuring on the scrollBar.
 *
 ************************************************************************/
static void
Release(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;

    if (!sbw->scrollBar.editable) return ;


    /* add an end update when the button is released.
       see comment in Select for the start update */

    XmDropSiteEndUpdate(wid);

    sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

    if (sbw->scrollBar.flags & KEYBOARD_GRABBED)
	{
	    XtUngrabKeyboard(wid, ((XButtonPressedEvent *)event)->time);
	    sbw->scrollBar.flags &= ~KEYBOARD_GRABBED;
	}

#ifdef FUNKY_INSENSITIVE_VISUAL
    if ( (!(sbw->scrollBar.flags & ARROW1_AVAILABLE)) &&
	(sbw->scrollBar.value > sbw->scrollBar.minimum))
	{
	    XClearArea(XtDisplay(sbw), XtWindow(sbw),
		       sbw->scrollBar.arrow1_x,
		       sbw->scrollBar.arrow1_y,
		       sbw->scrollBar.arrow_width,
		       sbw->scrollBar.arrow_height,
		       FALSE);

	    DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
		       sbw->primitive.bottom_shadow_GC,
		       sbw->scrollBar.arrow1_x,
		       sbw->scrollBar.arrow1_y,
		       sbw->scrollBar.arrow1_orientation);

	    sbw->scrollBar.flags |= ARROW1_AVAILABLE;
	}
    else if (sbw->scrollBar.value == sbw->scrollBar.minimum)
	sbw->scrollBar.flags &= ~ARROW1_AVAILABLE;

    if ( (!(sbw->scrollBar.flags & ARROW2_AVAILABLE)) &&
	(sbw->scrollBar.value < (sbw->scrollBar.maximum
				 - sbw->scrollBar.slider_size)))
	{
	    XClearArea(XtDisplay(sbw), XtWindow(sbw),
		       sbw->scrollBar.arrow2_x,
		       sbw->scrollBar.arrow2_y,
		       sbw->scrollBar.arrow_width,
		       sbw->scrollBar.arrow_height,
		       FALSE);

	    DRAWARROW (sbw, sbw->primitive.top_shadow_GC,
		       sbw -> primitive.bottom_shadow_GC,
		       sbw->scrollBar.arrow2_x,
		       sbw->scrollBar.arrow2_y,
		       sbw->scrollBar.arrow2_orientation);

	    sbw->scrollBar.flags |= ARROW2_AVAILABLE;
	}
    else if (sbw->scrollBar.value == (sbw->scrollBar.maximum
				      - sbw->scrollBar.slider_size))
	sbw->scrollBar.flags &= ~ARROW2_AVAILABLE;
#endif
    if (sbw->scrollBar.arrow1_selected)
	{
	    sbw->scrollBar.arrow1_selected = False;

	    DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
		       sbw->primitive.bottom_shadow_GC,
		       sbw->scrollBar.arrow1_x,
		       sbw->scrollBar.arrow1_y,
		       sbw->scrollBar.arrow1_orientation);
	}

    if (sbw->scrollBar.arrow2_selected)
	{
	    sbw->scrollBar.arrow2_selected = False;

	    DRAWARROW (sbw, sbw->primitive.top_shadow_GC,
		       sbw -> primitive.bottom_shadow_GC,
		       sbw->scrollBar.arrow2_x,
		       sbw->scrollBar.arrow2_y,
		       sbw->scrollBar.arrow2_orientation);
	}

    if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
        return;

    if (sbw->scrollBar.timer != 0)
	{
	    sbw->scrollBar.flags |= END_TIMER;
	}

    if (sbw->scrollBar.sliding_on == True)
	{
	    sbw->scrollBar.sliding_on = False;
	    ScrollCallback (sbw, XmCR_VALUE_CHANGED, sbw->scrollBar.value,
			    event->xbutton.x, event->xbutton.y, event);
	}

#ifdef FUNKY_INSENSITIVE_VISUAL
    XSetClipMask(XtDisplay(sbw), sbw->scrollBar.unavailable_GC, None);
    if (! (sbw->scrollBar.flags & ARROW1_AVAILABLE))
	{
	    XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
			   sbw->scrollBar.unavailable_GC,
			   sbw->scrollBar.arrow1_x,
			   sbw->scrollBar.arrow1_y,
			   sbw->scrollBar.arrow_width,
			   sbw->scrollBar.arrow_height);
	}
    else if (! (sbw->scrollBar.flags & ARROW2_AVAILABLE))
	{
	    XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
			   sbw->scrollBar.unavailable_GC,
			   sbw->scrollBar.arrow2_x,
			   sbw->scrollBar.arrow2_y,
			   sbw->scrollBar.arrow_width,
			   sbw->scrollBar.arrow_height);
	}
#endif
}




/************************************************************************
 *
 *  Moved
 *     This function processes mouse moved events during interactive
 *     slider moves.
 *
 ************************************************************************/
static void
Moved(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
    XButtonPressedEvent * buttonEvent = (XButtonPressedEvent *) event;
    int newX, newY;
    int realX, realY;
    int slideVal;
    int button_x;
    int button_y;
    int real_width_limit =
	(sbw->scrollBar.snap_back_multiple +
	 (buttonEvent->x > 0)) * XtWidth(wid);
    int real_height_limit =
	(sbw->scrollBar.snap_back_multiple +
	 (buttonEvent->y > 0)) * XtHeight(wid) ;

    if (!sbw->scrollBar.editable) return ;

    if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE)) return;

    /* operation was cancelled, so don't restart the move
       as it could happen with the snapBack stuff */
    if (sbw->scrollBar.flags & OPERATION_CANCELLED) return;

    if (!sbw->scrollBar.sliding_on) return;

    /* Only deal with snap_back if operation was started by a
       click in the slider. */
    if (((sbw->scrollBar.orientation == XmVERTICAL) &&
	 ((buttonEvent->x > real_width_limit) ||
	  (-buttonEvent->x > real_width_limit))) ||
	((sbw->scrollBar.orientation == XmHORIZONTAL) &&
	 ((buttonEvent->y > real_height_limit) ||
	  (-buttonEvent->y > real_height_limit)))) {
	/* going out of the snap back area */

	if (!(sbw->scrollBar.add_flags & SNAPPED_OUT)) {
	    short savedX, savedY, j1, j2;

	    /* get the saved value, also used by the Cancel action */
	    sbw->scrollBar.value = sbw->scrollBar.saved_value;
	    CalcSliderRect(sbw, &savedX, &savedY, &j1, &j2);
	    MoveSlider(sbw, savedX, savedY);
	    if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
	    ScrollCallback (sbw, XmCR_VALUE_CHANGED,
			    sbw->scrollBar.value, savedX, savedY,
			    (XEvent *) buttonEvent);

	    sbw->scrollBar.add_flags |= SNAPPED_OUT ;
	}
	return ;
    } else {
	/* moving in the snap back area */
	sbw->scrollBar.add_flags &= ~SNAPPED_OUT ; ;
    }

    button_x = buttonEvent->x;
    button_y = buttonEvent->y;

    /*
     * Force button_x and button_y to be within the slider_area.
     */
    if (button_x < sbw->scrollBar.slider_area_x)
	button_x = sbw->scrollBar.slider_area_x;

    if (button_x >
	sbw->scrollBar.slider_area_x + sbw->scrollBar.slider_area_width)
	button_x = sbw->scrollBar.slider_area_x
	    + sbw->scrollBar.slider_area_width;

    if (button_y < sbw->scrollBar.slider_area_y)
	button_y = sbw->scrollBar.slider_area_y;

    if (button_y >
	sbw->scrollBar.slider_area_y
	+ sbw->scrollBar.slider_area_height)
	button_y = sbw->scrollBar.slider_area_y
	    + sbw->scrollBar.slider_area_height;


    /*
     * Calculate the new origin of the slider.
     * Bound the values with the slider area.
     */
    if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
	    newX = realX = button_x - sbw->scrollBar.separation_x;
	    newY = realY = sbw->scrollBar.slider_y;

	    if (newX < sbw->scrollBar.slider_area_x)
		newX = sbw->scrollBar.slider_area_x;

	    if ((newX + sbw->scrollBar.slider_width >
		sbw->scrollBar.slider_area_x
		+ sbw->scrollBar.slider_area_width) &&
		    (sbw->scrollBar.sliding_mode != XmTHERMOMETER))
		newX = sbw->scrollBar.slider_area_x
		    + sbw->scrollBar.slider_area_width
			- sbw->scrollBar.slider_width;
	}
    else
	{
	    newX = realX = sbw->scrollBar.slider_x;
	    newY = realY = button_y - sbw->scrollBar.separation_y;


	    if (newY < sbw->scrollBar.slider_area_y)
		newY = sbw->scrollBar.slider_area_y;

	    if ((newY + sbw->scrollBar.slider_height >
		sbw->scrollBar.slider_area_y
		+ sbw->scrollBar.slider_area_height) &&
		    (sbw->scrollBar.sliding_mode != XmTHERMOMETER))
		newY = sbw->scrollBar.slider_area_y
		    + sbw->scrollBar.slider_area_height
			- sbw->scrollBar.slider_height;
	}


    if (((sbw->scrollBar.orientation == XmHORIZONTAL) &&
	 (realX != sbw->scrollBar.initial_x))
	||
	((sbw->scrollBar.orientation == XmVERTICAL)   &&
	 (realY != sbw->scrollBar.initial_y)))
	{
	    slideVal = CalcSliderVal (sbw, button_x, button_y);

	    if ((newX != sbw->scrollBar.initial_x) ||
		(newY != sbw->scrollBar.initial_y))
		{
		    MoveSlider (sbw, newX, newY);
		    sbw->scrollBar.initial_x = newX;
		    sbw->scrollBar.initial_y = newY;
		}

	    if (slideVal != sbw->scrollBar.value)
		{
		    sbw->scrollBar.value = slideVal;

		    if (slideVal >= (sbw->scrollBar.maximum
				     - sbw->scrollBar.slider_size))
			slideVal = sbw->scrollBar.maximum
			    - sbw->scrollBar.slider_size;

		    if (slideVal <= sbw->scrollBar.minimum)
			slideVal = sbw->scrollBar.minimum;
		    if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
			RedrawSliderWindow (sbw);
		    ScrollCallback(sbw, XmCR_DRAG,
				   sbw->scrollBar.value = slideVal,
				   buttonEvent->x, buttonEvent->y,
				   (XEvent *) buttonEvent);
		}
	}
}




/*********************************************************************
 *
 *  TopOrBottom
 *	Issue the to top or bottom callbacks.
 *
 *********************************************************************/
static void
TopOrBottom(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
    XmScrollBarPart *sbp = (XmScrollBarPart *) &(sbw->scrollBar);

    if (!sbw->scrollBar.editable) return ;

    sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

    if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
	return;

    if (event->type == KeyPress) {
	Modifiers junk;
	KeySym key_sym;
	XKeyPressedEvent *keyEvent = (XKeyPressedEvent *) event;

	key_sym = XtGetActionKeysym(event, &junk);

	if (key_sym == osfXK_BeginLine) {
	    if (sbp->orientation == XmVERTICAL) {
		if (sbp->processing_direction == XmMAX_ON_BOTTOM)
		    MoveSlider(sbw, sbp->slider_x, sbp->slider_area_y);
		else
		    MoveSlider(sbw,
			       sbp->slider_x,
			       sbp->slider_area_y + sbp->slider_area_height
			       - sbp->slider_height);
	    } else {
		if (sbp->processing_direction == XmMAX_ON_RIGHT)
		    MoveSlider(sbw, sbp->slider_area_x, sbp->slider_y);
		else
		    MoveSlider(sbw,
			       sbp->slider_area_x + sbp->slider_area_width
			       - sbp->slider_width,
			       sbp->slider_y);
	    }
	    /*
	     * The following grevious bogosity is due to the fact
	     * that the key behavior was implemented long after
	     * the rest of this code, and so we have to work around
	     * currently operating nonsense.
	     *
	     * Specifically, since the dawn of time, ScrollBar
	     * processes in just one direction, and does any necessary
	     * reversal just before calling the callback.
	     *
	     * We now proceed to trick that code into doing the right
	     * thing anyway
	     */
	    if (!PROCESS_DIR_INVERSED(sbw)) {
		sbp->value = sbp->minimum;
		if (sbp->sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback(sbw, XmCR_TO_TOP, sbp->value,
			       keyEvent->x, keyEvent->y,
			       (XEvent *) keyEvent);
	    } else {
		sbp->value = sbp->maximum - sbp->slider_size;
		if (sbp->sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback(sbw, XmCR_TO_BOTTOM, sbp->value,
			       keyEvent->x, keyEvent->y,
			       (XEvent *) keyEvent);
	    }
	}
	else /* key_sym == osfXK_EndLine */ {
	    if (sbp->orientation == XmVERTICAL) {
		if (sbp->processing_direction == XmMAX_ON_BOTTOM)
		    MoveSlider(sbw,
			       sbp->slider_x,
			       sbp->slider_area_y + sbp->slider_area_height
			       - sbp->slider_height);
		else
		    MoveSlider(sbw, sbp->slider_x, sbp->slider_area_y);
	    } else {
		if (sbp->processing_direction == XmMAX_ON_RIGHT)
		    MoveSlider(sbw,
			       sbp->slider_area_x + sbp->slider_area_width
			       - sbp->slider_width,
			       sbp->slider_y);
		else
		    MoveSlider(sbw, sbp->slider_area_x, sbp->slider_y);
	    }

	    /* See above for explanation of this nonsense */
	    if (!PROCESS_DIR_INVERSED(sbw)) {
		sbp->value = sbp->maximum - sbp->slider_size;
		if (sbp->sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback(sbw, XmCR_TO_BOTTOM, sbp->value,
			       keyEvent->x, keyEvent->y,
			       (XEvent *) keyEvent);
	    } else {
		sbp->value = sbp->minimum;
		if (sbp->sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback (sbw, XmCR_TO_TOP, sbp->value,
				keyEvent->x, keyEvent->y,
				(XEvent *) keyEvent);
	    }
	}
    } else  /* event->type == ButtonPress */ {
	XButtonPressedEvent *buttonEvent =
	    (XButtonPressedEvent *) event;

	XmDropSiteStartUpdate(wid);

	if /* In arrow1... */
	    ((buttonEvent->x >= sbp->arrow1_x)                   &&
	     (buttonEvent->y >= sbp->arrow1_y)                    &&
	     (buttonEvent->x <= sbp->arrow1_x + sbp->arrow_width) &&
	     (buttonEvent->y <= sbp->arrow1_y + sbp->arrow_height)) {
		sbp->change_type = XmCR_DECREMENT;
		sbp->arrow1_selected = True;

		DRAWARROW(sbw, sbw->primitive.bottom_shadow_GC,
			  sbw -> primitive.top_shadow_GC,
			  sbw->scrollBar.arrow1_x,
			  sbw->scrollBar.arrow1_y,
			  sbw->scrollBar.arrow1_orientation);

		if (sbp->orientation == XmVERTICAL)
		    MoveSlider(sbw, sbp->slider_x, sbp->slider_area_y);
		else
		    MoveSlider(sbw, sbp->slider_area_x, sbp->slider_y);

		sbp->value = sbp->minimum;
		if (sbp->sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback (sbw, XmCR_TO_TOP, sbp->value,
				buttonEvent->x, buttonEvent->y,
				(XEvent *) buttonEvent);
	    }

	else if /* In arrow2... */
	    ((buttonEvent->x >= sbp->arrow2_x)  &&
	     (buttonEvent->y >= sbp->arrow2_y)   &&
	     (buttonEvent->x <= sbp->arrow2_x
	      + sbp->arrow_width)             &&
	     (buttonEvent->y <= sbp->arrow2_y
	      + sbp->arrow_height))
		{
		    sbp->change_type = XmCR_INCREMENT;
		    sbp->arrow2_selected = True;

		    DRAWARROW (sbw, sbw->primitive.bottom_shadow_GC,
			       sbw -> primitive.top_shadow_GC,
			       sbw->scrollBar.arrow2_x,
			       sbw->scrollBar.arrow2_y,
			       sbw->scrollBar.arrow2_orientation);
		    if (sbp->orientation == XmVERTICAL)
			MoveSlider(sbw,
				   sbp->slider_x,
				   sbp->slider_area_y + sbp->slider_area_height
				   - sbp->slider_height);
		    else
			MoveSlider(sbw,
				   sbp->slider_area_x + sbp->slider_area_width
				   - sbp->slider_width,
				   sbp->slider_y);
		    sbp->value = sbp->maximum - sbp->slider_size;
		    if (sbp->sliding_mode == XmTHERMOMETER)
			RedrawSliderWindow (sbw);
		    ScrollCallback (sbw, XmCR_TO_BOTTOM,
				    sbp->value, buttonEvent->x, buttonEvent->y,
				    (XEvent *) buttonEvent);
		}
	else if /* in the trough between arrow2 and the slider... */
	    (((sbp->sliding_mode != XmTHERMOMETER) &&
	      (((sbp->orientation == XmHORIZONTAL)       &&
	      (buttonEvent->x >= sbp->slider_area_x) &&
	      (buttonEvent->x < sbp->slider_x)       &&
	      (buttonEvent->y >= sbp->slider_area_y) &&
	      (buttonEvent->y <= sbp->slider_area_y
	       + sbp->slider_area_height))
	     ||
	     ((sbp->orientation == XmVERTICAL) &&
	      (buttonEvent->y >= sbp->slider_area_y)  &&
	      (buttonEvent->y < sbp->slider_y)        &&
	      (buttonEvent->x >= sbp->slider_area_x)  &&
	      (buttonEvent->x < sbp->slider_area_x
	       + sbp->slider_area_width)))) ||
	     /* only partial treatment, processing direction
		has to be handled here */
	     ((sbp->sliding_mode == XmTHERMOMETER) &&
	      (((sbp->orientation == XmHORIZONTAL)       &&
	      (buttonEvent->x >= sbp->slider_area_x) &&
	      (buttonEvent->x < sbp->slider_width)       &&
	      (buttonEvent->y >= sbp->slider_area_y) &&
	      (buttonEvent->y <= sbp->slider_area_y
	       + sbp->slider_area_height))
	     ||
	     ((sbp->orientation == XmVERTICAL) &&
	      (buttonEvent->y < sbp->slider_area_height
	       - sbp->slider_height) &&
	      (buttonEvent->x >= sbp->slider_area_x)  &&
	      (buttonEvent->x < sbp->slider_area_x
	       + sbp->slider_area_width)))))
		{
		    if (sbp->orientation == XmVERTICAL)
			MoveSlider(sbw, sbp->slider_x, sbp->slider_area_y);
		    else
			MoveSlider(sbw, sbp->slider_area_x, sbp->slider_y);
		    sbp->value = sbp->minimum;
		    if (sbp->sliding_mode == XmTHERMOMETER)
			RedrawSliderWindow (sbw);
		    ScrollCallback (sbw, XmCR_TO_TOP, sbp->value,
				    buttonEvent->x, buttonEvent->y,
				    (XEvent *) buttonEvent);
		}
	else if
		/* in the trough between arrow1 and the slider... */
	    (((sbp->orientation == XmHORIZONTAL)                     &&
	      (buttonEvent->x > sbp->slider_x + sbp->slider_width) &&
	      (buttonEvent->x <= sbp->slider_area_x
	       + sbp->slider_area_width)                        &&
	      (buttonEvent->y >= sbp->slider_area_y)               &&
	      (buttonEvent->y <= sbp->slider_area_y
	       + sbp->slider_area_height))
	     ||
	     ((sbp->orientation == XmVERTICAL)           &&
	      (buttonEvent->y > sbp->slider_y
	       + sbp->slider_height)               &&
	      (buttonEvent->y <= sbp->slider_area_y
	       + sbp->slider_area_height)          &&
	      (buttonEvent->x >= sbp->slider_area_x)  &&
	      (buttonEvent->x <= sbp->slider_area_x
	       + sbp->slider_area_width))
	     || (sbp->sliding_mode == XmTHERMOMETER)) {
		if (sbp->orientation == XmVERTICAL)
		    MoveSlider(sbw,
			       sbp->slider_x,
			       sbp->slider_area_y + sbp->slider_area_height
			       - sbp->slider_height);
		else
		    MoveSlider(sbw,
			       sbp->slider_area_x + sbp->slider_area_width
			       - sbp->slider_width,
			       sbp->slider_y);
		sbp->value = sbp->maximum - sbp->slider_size;
		if (sbp->sliding_mode == XmTHERMOMETER)
			RedrawSliderWindow (sbw);
		ScrollCallback (sbw, XmCR_TO_BOTTOM,
				sbp->value, buttonEvent->x, buttonEvent->y,
				(XEvent *) buttonEvent);
	    }

    }
#ifdef FUNKY_INSENSITIVE_VISUAL
    XSetClipMask(XtDisplay(sbw), sbw->scrollBar.unavailable_GC, None);
    if (sbp->value == sbp->minimum) {
	XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
		       sbw->scrollBar.unavailable_GC,
		       sbw->scrollBar.arrow1_x,
		       sbw->scrollBar.arrow1_y,
		       sbw->scrollBar.arrow_width,
		       sbw->scrollBar.arrow_height);

	sbw->scrollBar.flags &= ~ARROW1_AVAILABLE;

	if (! (sbw->scrollBar.flags & ARROW2_AVAILABLE)) {
	    XClearArea(XtDisplay(sbw), XtWindow(sbw),
		       sbw->scrollBar.arrow2_x,
		       sbw->scrollBar.arrow2_y,
		       sbw->scrollBar.arrow_width,
		       sbw->scrollBar.arrow_height,
		       FALSE);

	    DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
		       sbw->primitive.bottom_shadow_GC,
		       sbw->scrollBar.arrow2_x,
		       sbw->scrollBar.arrow2_y,
		       sbw->scrollBar.arrow2_orientation);

	    sbw->scrollBar.flags |= ARROW2_AVAILABLE;
	}
    }
    else /* sbp->value == (sbp->maximum - sbp->slider_size) */
	{
	    /*		XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
			sbw->scrollBar.unavailable_GC,
			sbw->scrollBar.arrow2_x,
			sbw->scrollBar.arrow2_y,
			sbw->scrollBar.arrow_width,
			sbw->scrollBar.arrow_height);
			*/
	    sbw->scrollBar.flags &= ~ARROW2_AVAILABLE;

	    if (! (sbw->scrollBar.flags & ARROW1_AVAILABLE)) {
		XClearArea(XtDisplay(sbw), XtWindow(sbw),
			   sbw->scrollBar.arrow1_x,
			   sbw->scrollBar.arrow1_y,
			   sbw->scrollBar.arrow_width,
			   sbw->scrollBar.arrow_height,
			   FALSE);

		DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
			   sbw->primitive.bottom_shadow_GC,
			   sbw->scrollBar.arrow1_x,
			   sbw->scrollBar.arrow1_y,
			   sbw->scrollBar.arrow1_orientation);

		sbw->scrollBar.flags |= ARROW1_AVAILABLE;
	    }
	}
#endif
}




/*********************************************************************
 *
 *  IncrementUpOrLeft
 *	The up or left key was pressed, decrease the value by
 *	one increment.
 *
 *********************************************************************/
static void
IncrementUpOrLeft(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;

	int new_value;
	int key_pressed;

	if (!num_params || (*num_params != 1) || !params)
	{
	    XmeWarning(wid, MESSAGE14);
	    return;
	}

	if (!sbw->scrollBar.editable) return ;


	sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

	if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
		return;

	/*
 	 * arg value passed in will either be "up" for the up key or
	 * "left" for the left arrow key (or for compatibility 0 -> up
	 * key or 1 -> left key. The key needs to be compared with the
	 * scrollbar orientation to ensure only the proper directional key
	 * presses work.
	 */

	if (_XmConvertActionParamToRepTypeId((Widget) sbw,
		     XmRID_SCROLL_BAR_INCREMENT_UP_OR_LEFT_ACTION_PARAMS,
		     params[0], True, &key_pressed) == False)
	{
	    /* We couldn't convert the value. Just assume a value of 0. */
	    key_pressed = 0;
	}

	if (((key_pressed == 0) &&
		(sbw->scrollBar.orientation == XmHORIZONTAL))
		||
		((key_pressed == 1) &&
		(sbw->scrollBar.orientation == XmVERTICAL)))
		return;

	new_value = sbw->scrollBar.value - sbw->scrollBar.increment;

	if (new_value < sbw->scrollBar.minimum)
		new_value = sbw->scrollBar.minimum;

	if (new_value != sbw->scrollBar.value)
	{
		sbw->scrollBar.value = new_value;
#ifdef FUNKY_INSENSITIVE_VISUAL
		if ((sbw->scrollBar.value = new_value)
			== sbw->scrollBar.minimum)
		{
		        XSetClipMask(XtDisplay(sbw),
				     sbw->scrollBar.unavailable_GC, None);
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);

            sbw->scrollBar.flags &= ~ARROW1_AVAILABLE;
		}
#endif
		if ((sbw->scrollBar.show_arrows) &&
		    (! (sbw->scrollBar.flags & ARROW2_AVAILABLE)))
		{
			XClearArea(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height,
				FALSE);

			DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
				sbw->primitive.bottom_shadow_GC,
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow2_orientation);

			sbw->scrollBar.flags |= ARROW2_AVAILABLE;
		}

		RedrawSliderWindow (sbw);

		ScrollCallback (sbw, XmCR_DECREMENT, sbw->scrollBar.value,
                        event->xbutton.x, event->xbutton.y, event);
	}
}




/*********************************************************************
 *
 *  IncrementDownOrRight
 *	The down or right key was pressed, increase the value by
 *	one increment.
 *
 *********************************************************************/
static void
IncrementDownOrRight(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
	int new_value;
	int key_pressed;

	if (!num_params || (*num_params != 1) || !params)
	{
	    XmeWarning(wid, MESSAGE14);
	    return;
	}

	if (!sbw->scrollBar.editable) return ;

	sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

	if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
		return;

	/*
 	 * arg value passed in will either be "down" for the down key or
	 * "right" for the right arrow key (or for compatibility 0 -> down
	 * key or 1 -> right key. The key needs to be compared with the
	 * scrollbar orientation to ensure only the proper directional key
	 * presses work.
	 */

	if (_XmConvertActionParamToRepTypeId((Widget) sbw,
		     XmRID_SCROLL_BAR_INCREMENT_DOWN_OR_RIGHT_ACTION_PARAMS,
		     params[0], True, &key_pressed) == False)
	{
	    /* We couldn't convert the value. Just assume a value of 0. */
	    key_pressed = 0;
	}

	if (((key_pressed == 0) &&
		(sbw->scrollBar.orientation == XmHORIZONTAL))
		||
		((key_pressed == 1) &&
		(sbw->scrollBar.orientation == XmVERTICAL)))
		return;

	new_value = sbw->scrollBar.value + sbw->scrollBar.increment;

	if (new_value > sbw->scrollBar.maximum - sbw->scrollBar.slider_size)
		new_value = sbw->scrollBar.maximum - sbw->scrollBar.slider_size;

	if (new_value != sbw->scrollBar.value)
	{
		sbw->scrollBar.value = new_value;
#ifdef FUNKY_INSENSITIVE_VISUAL
		if ((sbw->scrollBar.value = new_value)
			== (sbw->scrollBar.maximum - sbw->scrollBar.slider_size))
		{
		        XSetClipMask(XtDisplay(sbw),
				     sbw->scrollBar.unavailable_GC, None);
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);

            sbw->scrollBar.flags &= ~ARROW2_AVAILABLE;
		}
#endif
		if ((sbw->scrollBar.show_arrows) &&
		    (! (sbw->scrollBar.flags & ARROW1_AVAILABLE)))

		{
			XClearArea(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height,
				FALSE);

			DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
				sbw->primitive.bottom_shadow_GC,
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow1_orientation);

			sbw->scrollBar.flags |= ARROW1_AVAILABLE;
		}

		RedrawSliderWindow (sbw);

		ScrollCallback (sbw, XmCR_INCREMENT, sbw->scrollBar.value,
                                    event->xbutton.x, event->xbutton.y, event);
	}
}




/*********************************************************************
 *
 *  PageUpOrLeft
 *	The up or left key was pressed, decrease the value by
 *	one increment.
 *
 *********************************************************************/
static void
PageUpOrLeft(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
	int new_value;
	int key_pressed;

	if (!num_params || (*num_params != 1) || !params)
	{
	    XmeWarning(wid, MESSAGE14);
	    return;
	}

	if (!sbw->scrollBar.editable) return ;

	sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

	if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
		return;
	/*
 	 * arg value passed in will either be "up" for the up key or
	 * "left" for the left arrow key (or for compatibility 0 -> up
	 * key or 1 -> left key. The key needs to be compared with the
	 * scrollbar orientation to ensure only the proper directional key
	 * presses work.
	 */

	if (_XmConvertActionParamToRepTypeId((Widget) sbw,
		     XmRID_SCROLL_BAR_PAGE_UP_OR_LEFT_ACTION_PARAMS,
		     params[0], True, &key_pressed) == False)
	{
	    /* We couldn't convert the value. Just assume a value of 0. */
	    key_pressed = 0;
	}

	if (((key_pressed == 0) &&
		(sbw->scrollBar.orientation == XmHORIZONTAL))
		||
		((key_pressed == 1) &&
		(sbw->scrollBar.orientation == XmVERTICAL)))
		return;

	new_value = sbw->scrollBar.value - sbw->scrollBar.page_increment;

	if (new_value < sbw->scrollBar.minimum)
		new_value = sbw->scrollBar.minimum;

	if (new_value != sbw->scrollBar.value)
	{
		sbw->scrollBar.value = new_value;
#ifdef FUNKY_INSENSITIVE_VISUAL
		if ((sbw->scrollBar.value = new_value)
			== sbw->scrollBar.minimum)
		{
		        XSetClipMask(XtDisplay(sbw),
				     sbw->scrollBar.unavailable_GC, None);
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);

            sbw->scrollBar.flags &= ~ARROW1_AVAILABLE;
		}
#endif
               if ((sbw->scrollBar.show_arrows) &&
                   (! (sbw->scrollBar.flags & ARROW2_AVAILABLE)))
		{
			XClearArea(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height,
				FALSE);

			DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
				sbw->primitive.bottom_shadow_GC,
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow2_orientation);

			sbw->scrollBar.flags |= ARROW2_AVAILABLE;
		}

		RedrawSliderWindow (sbw);

		ScrollCallback (sbw, XmCR_PAGE_DECREMENT,
				sbw->scrollBar.value,
				event->xbutton.x, event->xbutton.y, event);
	}
}




/*********************************************************************
 *
 *  PageDownOrRight
 *	The down or right key was pressed, increase the value by
 *	one increment.
 *
 *********************************************************************/
static void
PageDownOrRight(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) wid ;
	int new_value;
	int key_pressed;

	if (!num_params || (*num_params != 1) || !params)
	{
	    XmeWarning(wid, MESSAGE14);
	    return;
	}

	if (!sbw->scrollBar.editable) return ;

	sbw->scrollBar.flags &=  ~OPERATION_CANCELLED ;

	if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
		return;

	/*
 	 * arg value passed in will either be "down" for the down key or
	 * "right" for the right arrow key (or for compatibility 0 -> down
	 * key or 1 -> right key. The key needs to be compared with the
	 * scrollbar orientation to ensure only the proper directional key
	 * presses work.
	 */

	if (_XmConvertActionParamToRepTypeId((Widget) sbw,
		     XmRID_SCROLL_BAR_PAGE_DOWN_OR_RIGHT_ACTION_PARAMS,
		     params[0], True, &key_pressed) == False)
	{
	    /* We couldn't convert the value. Just assume a value of 0. */
	    key_pressed = 0;
	}

	if (((key_pressed == 0) &&
		(sbw->scrollBar.orientation == XmHORIZONTAL))
		||
		((key_pressed == 1) &&
		(sbw->scrollBar.orientation == XmVERTICAL)))
		return;

	new_value = sbw->scrollBar.value + sbw->scrollBar.page_increment;

	if (new_value > sbw->scrollBar.maximum - sbw->scrollBar.slider_size)
		new_value = sbw->scrollBar.maximum - sbw->scrollBar.slider_size;

	if (new_value != sbw->scrollBar.value)
	{
		sbw->scrollBar.value = new_value;
#ifdef FUNKY_INSENSITIVE_VISUAL
		if ((sbw->scrollBar.value = new_value)
			== (sbw->scrollBar.maximum - sbw->scrollBar.slider_size))
		{
		        XSetClipMask(XtDisplay(sbw),
				     sbw->scrollBar.unavailable_GC, None);
			XFillRectangle(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.unavailable_GC,
				sbw->scrollBar.arrow2_x,
				sbw->scrollBar.arrow2_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height);

            sbw->scrollBar.flags &= ~ARROW2_AVAILABLE;
		}
#endif
               if ((sbw->scrollBar.show_arrows) &&
                   (! (sbw->scrollBar.flags & ARROW1_AVAILABLE)))
		{
			XClearArea(XtDisplay(sbw), XtWindow(sbw),
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow_width,
				sbw->scrollBar.arrow_height,
				FALSE);

			DRAWARROW (sbw, sbw -> primitive.top_shadow_GC,
				sbw->primitive.bottom_shadow_GC,
				sbw->scrollBar.arrow1_x,
				sbw->scrollBar.arrow1_y,
				sbw->scrollBar.arrow1_orientation);

			sbw->scrollBar.flags |= ARROW1_AVAILABLE;
		}

		RedrawSliderWindow (sbw);

		ScrollCallback (sbw, XmCR_PAGE_INCREMENT, sbw->scrollBar.value,
                                    event->xbutton.x, event->xbutton.y, event);
	}
}

static void
CancelDrag(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
	XmScrollBarWidget sbw = (XmScrollBarWidget) wid;

	if (!sbw->scrollBar.editable) return ;

	if (sbw->scrollBar.flags & KEYBOARD_GRABBED)
	{
		short savedX, savedY, j1, j2;

		XtUngrabKeyboard(wid, ((XButtonPressedEvent *)event)->time);
		sbw->scrollBar.flags &= ~KEYBOARD_GRABBED;

		sbw->scrollBar.flags |= OPERATION_CANCELLED;

		sbw->scrollBar.sliding_on = False;
		sbw->scrollBar.value = sbw->scrollBar.saved_value;
		CalcSliderRect(sbw, &savedX, &savedY, &j1, &j2);
		MoveSlider(sbw, savedX, savedY);
		if (sbw->scrollBar.sliding_mode == XmTHERMOMETER)
		    RedrawSliderWindow (sbw);
		ScrollCallback (sbw, XmCR_VALUE_CHANGED,
		                sbw->scrollBar.value, savedX, savedY,
				(XEvent *) event);

		if (sbw->scrollBar.timer != 0)
		{
			sbw->scrollBar.flags |= END_TIMER;
		}

	}
	else
	{
		XmParentInputActionRec pp_data ;

		pp_data.process_type = XmINPUT_ACTION ;
		pp_data.action = XmPARENT_CANCEL ;
		pp_data.event = event ;
		pp_data.params = params ;
		pp_data.num_params = num_params ;

		_XmParentProcess( XtParent( wid), (XmParentProcessData) &pp_data) ;
	}
}


/*********************************************************************
 *
 *  MoveSlider
 *	Given x and y positions, move the slider and clear the area
 *	moved out of.
 *
 *********************************************************************/
static void
MoveSlider(
        XmScrollBarWidget sbw,
        int currentX,
        int currentY )
{
    int oldX = sbw->scrollBar.slider_x;
    int oldY = sbw->scrollBar.slider_y;
    int width = sbw->scrollBar.slider_width;
    int height = sbw->scrollBar.slider_height;

    XSegment seg[2];

    if ((currentX == oldX) && (currentY == oldY))
	return;

    if (sbw->scrollBar.sliding_mode == XmTHERMOMETER) {
	if (sbw->scrollBar.orientation == XmHORIZONTAL)
	    sbw->scrollBar.slider_x = currentX;
	else
	    sbw->scrollBar.slider_y = currentY;
	return ;
    }

    if (sbw->scrollBar.orientation == XmHORIZONTAL)
	{
	    sbw->scrollBar.slider_x = currentX;

	    seg[0].y1 = seg[0].y2 = oldY + 2;
	    seg[1].y1 = seg[1].y2 = oldY + height - 3;

	    if (oldX < currentX)
		{
		    seg[0].x1 = seg[1].x1 = oldX;
		    seg[0].x2 = seg[1].x2 = oldX + currentX - oldX - 1;
		}
	    else
		{
		    seg[0].x1 = seg[1].x1 = currentX + width;
		    seg[0].x2 = seg[1].x2 = seg[0].x1 + oldX - currentX - 1;
		}


	    if (sbw->scrollBar.pixmap != 0)
		{
		    CopySliderInWindow(sbw);
		    XClearArea (XtDisplay((Widget)sbw),
				XtWindow((Widget)sbw),
				seg[0].x1, oldY,
				seg[0].x2 - seg[0].x1 + 1,
				height, False);
		}
	}
    else /* sbw->scrollBar.orientation == XmVERTICAL */
	{
	    sbw->scrollBar.slider_y = currentY;

	    seg[0].x1 = seg[0].x2 = oldX + 2;
	    seg[1].x1 = seg[1].x2 = oldX + width - 3;

	    if (oldY < currentY)
		{
		    seg[0].y1 = seg[1].y1 = oldY;
		    seg[0].y2 = seg[1].y2 = oldY + currentY - oldY - 1;
		}
	    else
		{
		    seg[0].y1 = seg[1].y1 = currentY + height;
		    seg[0].y2 = seg[1].y2 = seg[0].y1 + oldY - currentY - 1;
		}

	    if (sbw->scrollBar.pixmap != 0)
		{
		    CopySliderInWindow(sbw);
		    XClearArea (XtDisplay((Widget)sbw),
				XtWindow((Widget)sbw),
				oldX, seg[0].y1, width,
				seg[0].y2 - seg[0].y1 + 1, False);
		}
	}
}




/************************************************************************
 *
 *  ChangeScrollBarValue
 *	Change the scrollbar value by the indicated change type.  Return
 *	True if the value changes, False otherwise.
 *
 ************************************************************************/
static Boolean
ChangeScrollBarValue(
        XmScrollBarWidget sbw )
{
    register unsigned char change_type = sbw->scrollBar.change_type;
    register int change_amount = 0;
    register Boolean returnFlag = TRUE;
    register int old_value = sbw->scrollBar.value;

    if (! (sbw->scrollBar.flags & SLIDER_AVAILABLE))
	return(FALSE);
    /*  Get the amount to change the scroll bar value based on  */
    /*  the type of change occuring.                            */

    if (change_type == XmCR_INCREMENT)
	change_amount = sbw->scrollBar.increment;
    else if (change_type == XmCR_PAGE_INCREMENT)
	change_amount = sbw->scrollBar.page_increment;
    else if (change_type == XmCR_DECREMENT)
	change_amount = -sbw->scrollBar.increment;
    else if (change_type == XmCR_PAGE_DECREMENT)
	change_amount = -sbw->scrollBar.page_increment;

    /* Change the value */
    sbw->scrollBar.value += change_amount;

    /* Truncate and set flags as appropriate */
    if (sbw->scrollBar.value >= (sbw->scrollBar.maximum
				 - sbw->scrollBar.slider_size))
	sbw->scrollBar.value = sbw->scrollBar.maximum
	    - sbw->scrollBar.slider_size;

    if (sbw->scrollBar.value <= sbw->scrollBar.minimum)
	sbw->scrollBar.value = sbw->scrollBar.minimum;

    if ((returnFlag = (sbw->scrollBar.value != old_value)) != False) {
	RedrawSliderWindow (sbw);
    }


    return (returnFlag);
}




/*********************************************************************
 *
 *  TimerEvent
 *	This is an event processing function which handles timer
 *	event evoked because of arrow selection.
 *
 *********************************************************************/
static void
TimerEvent(
        XtPointer closure,
        XtIntervalId *id )	/* unused */
{
        XmScrollBarWidget sbw = (XmScrollBarWidget) closure ;
	Boolean flag;

	sbw->scrollBar.timer = 0;

	if (sbw->scrollBar.flags & END_TIMER)
	{
		sbw->scrollBar.flags &= ~END_TIMER;
		return;
	}

	if (sbw->scrollBar.flags & FIRST_SCROLL_FLAG)
	{
		XSync (XtDisplay (sbw), False);

		sbw->scrollBar.flags &= ~FIRST_SCROLL_FLAG;

		sbw->scrollBar.timer =
		XtAppAddTimeOut (XtWidgetToApplicationContext((Widget) sbw),
                                   (unsigned long) sbw->scrollBar.repeat_delay,
                                                  TimerEvent, (XtPointer) sbw);
		return;
	}


	/*  Change the scrollbar slider value  */

	flag = ChangeScrollBarValue (sbw);

	/*  If the orgin was changed invoke the application supplied  */
	/*  slider moved callbacks                                    */

	if (flag)
		ScrollCallback (sbw, sbw->scrollBar.change_type,
			sbw->scrollBar.value, 0, 0, NULL);

	/*
	 * If the callback does alot of processing, and XSync is needed
	 * to flush the output and input buffers.  If this is not done,
	 * the entry back to MainLoop will cause the flush.  The server
	 * will then perform it work which may take longer than the timer
	 * interval which will cause the scrollbar to be stuck in a loop.
	 */

	XSync (XtDisplay (sbw), False);

	/*  Add the repeat timer and check that the scrollbar hasn't been set
	    insensitive by some callbacks */

	if (flag)
	{
		sbw->scrollBar.timer =
		    XtAppAddTimeOut (XtWidgetToApplicationContext((Widget) sbw),
				     (unsigned long) sbw->scrollBar.repeat_delay,
				     TimerEvent, (XtPointer) sbw);
	}
}




/************************************************************************
 *
 *  ScrollCallback
 *	This routine services the widget's callbacks.  It calls the
 *	specific callback if it is not empty.  If it is empty then the
 *	main callback is called.
 *
 ************************************************************************/
static void
ScrollCallback(
        XmScrollBarWidget sbw,
        int reason,
        int value,
        int xpixel,
        int ypixel,
        XEvent *event )
{
   XmScrollBarCallbackStruct call_value;

   call_value.reason = reason;
   call_value.event  = event;

   if (PROCESS_DIR_INVERSED(sbw)) {
       switch (reason) {
       case XmCR_INCREMENT:
	   call_value.reason = reason = XmCR_DECREMENT;
	   break;
       case XmCR_DECREMENT:
	   call_value.reason = reason = XmCR_INCREMENT;
	   break;
       case XmCR_PAGE_INCREMENT:
	   call_value.reason = reason = XmCR_PAGE_DECREMENT;
	   break;
       case XmCR_PAGE_DECREMENT:
	   call_value.reason = reason = XmCR_PAGE_INCREMENT;
	   break;
       case XmCR_TO_TOP:
	   call_value.reason = reason = XmCR_TO_BOTTOM;
	   break;
       case XmCR_TO_BOTTOM:
	   call_value.reason = reason = XmCR_TO_TOP;
	   break;
       }
       call_value.value = sbw->scrollBar.maximum
	   + sbw->scrollBar.minimum - value - sbw->scrollBar.slider_size;
   }
   else
       call_value.value = value;


   if (sbw->scrollBar.orientation == XmHORIZONTAL)
      call_value.pixel = xpixel;
   else
      call_value.pixel = ypixel;

   switch (reason) {

   case XmCR_VALUE_CHANGED:
       XtCallCallbackList ((Widget) sbw, sbw->scrollBar.value_changed_callback,
			   &call_value);
       break;

   case XmCR_INCREMENT:
       if  (sbw->scrollBar.increment_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.increment_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				 sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_DECREMENT:
       if  (sbw->scrollBar.decrement_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.decrement_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_PAGE_INCREMENT:
       if  (sbw->scrollBar.page_increment_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.page_increment_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_PAGE_DECREMENT:
       if  (sbw->scrollBar.page_decrement_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.page_decrement_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				 sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_TO_TOP:
       if (sbw->scrollBar.to_top_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.to_top_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_TO_BOTTOM:
       if (sbw->scrollBar.to_bottom_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.to_bottom_callback, &call_value);
       else
	   {
	       call_value.reason = XmCR_VALUE_CHANGED;
	       XtCallCallbackList ((Widget) sbw,
				 sbw->scrollBar.value_changed_callback, &call_value);
	   }
       break;

   case XmCR_DRAG:
       if (sbw->scrollBar.drag_callback)
	   XtCallCallbackList ((Widget) sbw,
			       sbw->scrollBar.drag_callback, &call_value);
       break;
   }
}




/************************************************************************
 *
 *  NavigChangeMoveCB
 *	add or remove the callback list to be called on any move.
 *      Since valueChangedCallback is not called on increment (& co)
 *      if an increment callback is present, we have to set them all.
 *
 ************************************************************************/
static void
NavigChangeMoveCB(
           Widget nav,
	   XtCallbackProc moveCB,
           XtPointer closure,
           Boolean setunset)
{
    if (setunset) {
	XtAddCallback (nav, XmNvalueChangedCallback, moveCB, closure);
	XtAddCallback (nav, XmNincrementCallback, moveCB, closure);
	XtAddCallback (nav, XmNdecrementCallback, moveCB, closure);
	XtAddCallback (nav, XmNpageIncrementCallback, moveCB, closure);
	XtAddCallback (nav, XmNpageDecrementCallback, moveCB, closure);
	XtAddCallback (nav, XmNtoTopCallback, moveCB, closure);
	XtAddCallback (nav, XmNtoBottomCallback, moveCB, closure);
	XtAddCallback (nav, XmNdragCallback, moveCB, closure);
    } else {
	XtRemoveCallback (nav, XmNvalueChangedCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNincrementCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNdecrementCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNpageIncrementCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNpageDecrementCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNtoTopCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNtoBottomCallback, moveCB, closure);
	XtRemoveCallback (nav, XmNdragCallback, moveCB, closure);
    }
}





/************************************************************************
 *
 *  NavigSetValue
 *	change the value and possibly call the callbacks
 *
 ************************************************************************/
static void
NavigSetValue(
           Widget nav,
	   XmNavigatorData nav_data,
           Boolean notify)
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) nav;
    int save_value;
    Arg arglist[6];
    Cardinal n;

    /* register which dimension this scrollbar is to operate */
    if (nav_data->valueMask & NavDimMask) {
	sbw->scrollBar.dimMask = nav_data->dimMask ;
    }

    /* scrollbar is a one dimensional navigator, it expects
       only one dimension to be set, the one that has been registered
       for using the NavDimMask (as treated above) */

    if (!(sbw->scrollBar.dimMask & nav_data->dimMask)) return ;

    /* we need to fetch either the x or y values out of the
       passed nav_data record, depending on the dimmask.
       scrollbar is only interested in one value */


    save_value = sbw->scrollBar.value;

    n = 0;
    if (nav_data->valueMask & NavValue) {
      if ((PROCESS_DIR_INVERSED(sbw) ?
	   INVERSED_VALUE(sbw) : sbw->scrollBar.value) !=
	  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->value)) {
	XtSetArg (arglist[n], XmNvalue,
		  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->value));n++;
      }
    }


    if ((nav_data->valueMask & NavMinimum) &&
	(sbw->scrollBar.minimum !=
	 ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->minimum))) {
	XtSetArg (arglist[n], XmNminimum,
		  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->minimum));n++;
    }

    if ((nav_data->valueMask & NavMaximum) &&
	(sbw->scrollBar.maximum !=
	 ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->maximum))) {
	XtSetArg (arglist[n], XmNmaximum,
		  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->maximum));n++;
    }

    if (sbw->scrollBar.sliding_mode != XmTHERMOMETER) {
	if ((nav_data->valueMask & NavSliderSize) &&
	    (sbw->scrollBar.slider_size !=
	     ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->slider_size)) &&
	    (ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->slider_size) != 0)) {
	    XtSetArg (arglist[n], XmNsliderSize,
		      ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->slider_size));
	    n++;
	}
    }

    if ((nav_data->valueMask & NavIncrement) &&
	(sbw->scrollBar.increment !=
	 ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->increment)) &&
	(ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->increment) != 0)) {
	XtSetArg (arglist[n], XmNincrement,
		  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->increment)); n++;
    }

    if ((nav_data->valueMask & NavPageIncrement) &&
	(sbw->scrollBar.page_increment !=
	 ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->page_increment)) &&
	(ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->page_increment) != 0)) {
	XtSetArg (arglist[n], XmNpageIncrement,
		  ACCESS_DIM(sbw->scrollBar.dimMask, nav_data->page_increment));
	n++;
    }

    if (n) XtSetValues (nav, arglist, n);

    if (notify && sbw->scrollBar.value != save_value)
	ScrollCallback (sbw, XmCR_VALUE_CHANGED,
			sbw->scrollBar.value, 0, 0, NULL);

}


/************************************************************************
 *
 *  NavigGetValue
 *	reports the all the data for this navigator scrollbar
 *      nav_data allocated by the caller
 ************************************************************************/
static void
NavigGetValue(
           Widget nav,
           XmNavigatorData nav_data)
{
    XmScrollBarWidget sbw = (XmScrollBarWidget) nav;

    nav_data->dimMask =  sbw->scrollBar.dimMask;

    if (nav_data->valueMask & NavValue) {
	int value ;

	if (PROCESS_DIR_INVERSED(sbw)) {
	    value = INVERSED_VALUE(sbw);
	} else
	    value = sbw->scrollBar.value;

	ASSIGN_DIM(nav_data->dimMask, nav_data->value, value) ;
    }

    if (nav_data->valueMask & NavMinimum)
	ASSIGN_DIM(nav_data->dimMask, nav_data->minimum,
		   sbw->scrollBar.minimum) ;


    if (nav_data->valueMask & NavMaximum)
	ASSIGN_DIM(nav_data->dimMask, nav_data->maximum,
		   sbw->scrollBar.maximum) ;


    if (nav_data->valueMask & NavSliderSize)
	ASSIGN_DIM(nav_data->dimMask, nav_data->slider_size,
		   sbw->scrollBar.slider_size) ;

    if (nav_data->valueMask & NavIncrement)
	ASSIGN_DIM(nav_data->dimMask, nav_data->increment,
		   sbw->scrollBar.increment) ;

    if (nav_data->valueMask & NavPageIncrement)
	ASSIGN_DIM(nav_data->dimMask, nav_data->page_increment,
		   sbw->scrollBar.page_increment) ;

}




/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/


/************************************************************************
 *
 *  XmCreateScrollBar
 *	Create an instance of a scrollbar and return the widget id.
 *
 ************************************************************************/
Widget
XmCreateScrollBar(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, xmScrollBarWidgetClass,
                           parent, arglist, argcount));
}

Widget
XmVaCreateScrollBar(
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
                         xmScrollBarWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedScrollBar(
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
                         xmScrollBarWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/************************************************************************
 *
 *  XmScrollBarGetValues
 *	Return some scrollbar values.
 *
 ************************************************************************/
void
XmScrollBarGetValues(
        Widget w,
        int *value,
        int *slider_size,
        int *increment,
        int *page_increment )
{
   XmScrollBarWidget sbw = (XmScrollBarWidget) w;
   XtAppContext app = XtWidgetToApplicationContext(w);

   _XmAppLock(app);

   if (PROCESS_DIR_INVERSED(sbw)) {
      if (value) *value = INVERSED_VALUE(sbw);
   } else
      if (value) *value = sbw->scrollBar.value;

   if (slider_size) *slider_size = sbw->scrollBar.slider_size;
   if (increment) *increment = sbw->scrollBar.increment;
   if (page_increment) *page_increment = sbw->scrollBar.page_increment;
   _XmAppUnlock(app);
}




/************************************************************************
 *
 *  XmScrollBarSetValues
 *	Set some scrollbar values.
 *
 ************************************************************************/
void
XmScrollBarSetValues(
        Widget w,
        int value,
        int slider_size,
        int increment,
        int page_increment,
        Boolean notify )
{
   XmScrollBarWidget sbw = (XmScrollBarWidget) w;
   int save_value;
   Arg arglist[4];
   Cardinal n;
   XtAppContext app = XtWidgetToApplicationContext(w);

   _XmAppLock(app);


   save_value = sbw->scrollBar.value;

   n = 0;
   XtSetArg (arglist[n], XmNvalue, value);			n++;

   if (sbw->scrollBar.sliding_mode != XmTHERMOMETER) {
       if (slider_size != 0) {
	   XtSetArg (arglist[n], XmNsliderSize, slider_size);	n++;
       }
   }

   if (increment != 0) {
      XtSetArg (arglist[n], XmNincrement, increment);		n++;
   }

   if (page_increment != 0) {
      XtSetArg (arglist[n], XmNpageIncrement, page_increment);	n++;
   }

   XtSetValues ((Widget) sbw, arglist, n);

   if (notify && sbw->scrollBar.value != save_value)
      ScrollCallback (sbw, XmCR_VALUE_CHANGED,
                      sbw->scrollBar.value, 0, 0, NULL);
   _XmAppUnlock(app);
}

