/* $TOG: SpinB.c /main/27 1999/04/16 08:48:58 mgreess $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/cursorfont.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <Xm/AccTextT.h>
#include <Xm/DrawP.h>
#include <Xm/NavigatorT.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "GeoUtilsI.h"
#include "GMUtilsI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmI.h"

#include <Xm/SpinBP.h>

static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass classPart);
static void Initialize(Widget req, Widget w,
		       ArgList args, Cardinal *num_args);
static void Destroy(Widget w);
static void Resize(Widget w);
static void Redisplay(Widget w, XEvent *event, Region region);
static Boolean SetValues(Widget old, Widget req, Widget new_w,
			 ArgList args, Cardinal *num_args);
static XtGeometryResult QueryGeometry(Widget w,
				      XtWidgetGeometry*req,
				      XtWidgetGeometry*rep );
static XtGeometryResult GeometryManager(Widget w,
					XtWidgetGeometry *req,
					XtWidgetGeometry *rep);
static void ChangeManaged(Widget w);
static void InsertChild(Widget newChild);
static void ConstraintInitialize(Widget req, Widget new_w,
				 ArgList args, Cardinal *num_args);
static void ConstraintDestroy(Widget w);
static Boolean ConstraintSetValues(Widget old, Widget req, Widget new_w,
				   ArgList args, Cardinal *num_args);
static void SpinChildFocusChange(Widget    focusWidget, XtPointer focusClient,
				 XEvent    *focusEvent,
				 Boolean   *focusContinue);
static void SpinBArm(Widget   armWidget, XEvent   *armEvent,
		     String   *armParams, Cardinal *armCount );
static void SpinBDisarm(Widget   disarmWidget, XEvent   *disarmEvent,
			String   *disarmParams, Cardinal *disarmCount);
static void SpinBFirst(Widget   firstWidget, XEvent   *firstEvent,
		       String   *firstParams, Cardinal *firstCount );
static void SpinBLast(Widget   lastWidget, XEvent   *lastEvent,
		      String   *lastParams, Cardinal *lastCount );
static void SpinBLeft(Widget   leftWidget, XEvent   *leftEvent,
		      String   *leftParams, Cardinal *leftCount );
static void SpinBNext(Widget   nextWidget, XEvent   *nextEvent,
		      String   *nextParams, Cardinal *nextCount );
static void SpinBPrior(Widget   priorWidget, XEvent   *priorEvent,
		       String   *priorParams, Cardinal *priorCount );
static void SpinBRight(Widget   rightWidget, XEvent   *rightEvent,
		       String   *rightParams, Cardinal *rightCount );
static void SpinBEnter(Widget, XEvent*, String*, Cardinal*);
static void SpinBLeave(Widget, XEvent*, String*, Cardinal*);
static void ClearArrows(Widget clearW);
static Boolean UpArrowSensitive(XmSpinBoxWidget spinW);
static Boolean DownArrowSensitive(XmSpinBoxWidget spinW);
static int NumericChildCount(XmSpinBoxWidget spinW);
static Boolean WidgetIsChild(XmSpinBoxWidget spinW, Widget child);
static Boolean ChildIsTraversable(Widget w);
static int GetArrowDirection(Widget w, int spinDir);
static void LayoutSpinBox(Widget w, XtWidgetGeometry *spinG, Widget child);
static void NumToString(char **buffer, int min, int max,
			int decimal, int value );
static void DrawSpinArrow(Widget arrowWidget, int arrowFlag);
static void SpinTimeOut(Widget w, int spinDelay);
static void UpdateChildText(Widget textW);
static Boolean ArrowWasHit(Widget arrowW, int arrowType, XEvent *arrowEvent);
static void SpinBArrow(XtPointer spinData, XtIntervalId *spinInterval);
static void SpinBAction(Widget   actionWidget, short    arrowHit );
static void FireCallbacks(XmSpinBoxCallbackStruct   *spinBoxCallData,
			  XtCallbackList callbackList,
			  Widget arrowWidget,
			  XEvent *arrowEvent,
			  int    arrowReason);
static void ArrowCallback(Widget arrowWidget,
			  XEvent *arrowEvent,
			  int    arrowReason);
static Boolean ArrowVerify(Widget arrowWidget,
			   XEvent *arrowEvent,
			   int    arrowReason );
static void ArrowSpinUp(Widget w, XEvent *callEvent);
static void ArrowSpinDown(Widget w, XEvent *callEvent);
static void GetSpinSize(Widget w, Dimension *wide, Dimension *high);
static void SpinNChangeMoveCB(Widget nav, XtCallbackProc moveCB,
			      XtPointer closure, Boolean setunset );
static void SpinNSetValue(Widget nav, XmNavigatorData nav_data,
			  Boolean notify );
static void SpinNGetValue(Widget nav, XmNavigatorData nav_data );
static void GetPositionValue(Widget w, int offset, XtArgVal *value);
static XmImportOperator SetPositionValue(Widget w, int offset, XtArgVal *value);
static int GetMaximumPositionValue(XmSpinBoxConstraint sc);
static int GetMinimumPositionValue(XmSpinBoxConstraint sc);
static char * ValidatePositionValue(XmSpinBoxConstraint sc, int *position);
static Boolean CvtStringToPositionValue(Display *dpy,
                                        XrmValue *args,
                                        Cardinal *num_args,
                                        XrmValue *from,
                                        XrmValue *to,
                                        XtPointer *converter_data);


/* Macros */

#define SB_ArrowLayout(w)	(((XmSpinBoxWidget) (w))->spinBox.arrow_layout)
#define SB_ArrowsAreStacked(w) \
	((SB_ArrowLayout(w) == XmARROWS_END) || \
	 (SB_ArrowLayout(w) == XmARROWS_BEGINNING))
#define SB_NumArrowsWide(w)	((SB_ArrowsAreStacked(w)) ? 1 : 2)
#define SB_NumArrowsHigh(w)	((SB_ArrowsAreStacked(w)) ? 2 : 1)
#define SB_ShadowMargin		2
#define SB_ShadowPixels(w) \
    ( (((XmSpinBoxWidget) (w))->manager.shadow_thickness) ? \
       (((XmSpinBoxWidget) (w))->manager.shadow_thickness + SB_ShadowMargin) : \
       0 )

/* Actions table */

static XtActionsRec actionsTable [] =
{
  {"SpinBArm",	           SpinBArm },
  {"SpinBDisarm",	   SpinBDisarm },
  {"SpinBPrior",	   SpinBPrior },
  {"SpinBNext",		   SpinBNext },
  {"SpinBLeft",		   SpinBLeft },
  {"SpinBRight",	   SpinBRight },
  {"SpinBFirst",	   SpinBFirst },
  {"SpinBLast",		   SpinBLast },
  {"SpinBEnter",	   SpinBEnter },
  {"SpinBLeave",	   SpinBLeave },
};

#define BAD_SPIN_VALUES			_XmMMsgSpinB_0003
#define BAD_SPIN_INCREMENT		_XmMMsgSpinB_0004
#define BAD_SPIN_DIRECTION		_XmMMsgSpinB_0005
#define BAD_SPIN_POSITION_MIN		_XmMMsgSpinB_0006
#define BAD_SPIN_POSITION_MAX		_XmMMsgSpinB_0007
#define BAD_SPIN_POSITION_TYPE		_XmMMsgSpinB_0008

#define DEFAULT_ARROW_SIZE 16

#define defaultTranslations _XmSpinB_defaultTranslations

static XtAccelerators    spinAccel;

/* Resources */
#define Offset(field) XtOffsetOf(XmSpinBoxRec,spinBox.field)
static XtResource resources[] = {
  { XmNarrowLayout, XmCArrowLayout, XmRArrowLayout,
    sizeof(unsigned char), Offset(arrow_layout),
    XmRImmediate, (XtPointer) XmARROWS_END
  },
  { XmNarrowOrientation, XmCArrowOrientation, XmRArrowOrientation,
    sizeof(unsigned char), Offset(arrow_orientation),
    XmRImmediate, (XtPointer) XmARROWS_VERTICAL
  },
  { XmNarrowSize, XmCArrowSize, XmRHorizontalDimension,
    sizeof(Dimension), Offset(arrow_size),
    XmRImmediate, (XtPointer) DEFAULT_ARROW_SIZE
  },
  { XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension), Offset(margin_width),
    XmRImmediate, (XtPointer) 2
  },
  { XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
    sizeof(Dimension), Offset(margin_height),
    XmRImmediate, (XtPointer) 2
  },
  { XmNspacing, XmCSpacing, XmRHorizontalDimension,
    sizeof(Dimension), Offset(spacing),
    XmRImmediate, (XtPointer) 0
  },
  { XmNinitialDelay, XmCInitialDelay, XmRInt,
    sizeof(unsigned int), Offset(initial_delay),
    XmRImmediate, (XtPointer) 250
  },
  { XmNrepeatDelay, XmCRepeatDelay, XmRInt,
    sizeof(unsigned int), Offset(repeat_delay),
    XmRImmediate, (XtPointer) 200
  },
  { XmNdefaultArrowSensitivity, XmCDefaultArrowSensitivity,
    XmRArrowSensitivity,
    sizeof(unsigned char), Offset(default_arrow_sensitivity),
    XmRImmediate, (XtPointer) XmARROWS_SENSITIVE
  },
  { XmNmodifyVerifyCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), Offset(modify_verify_cb),
    XmRPointer, NULL
  },
  { XmNvalueChangedCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), Offset(value_changed_cb),
    XmRPointer, NULL,
  },
  { XmNdetailShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(detail_shadow_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  }
};

/* Resources */

#define ConstraintOffset(field)\
  XtOffsetOf(XmSpinBoxConstraintRec,spinBox.field)

static XtResource constraints[] = {
  { XmNspinBoxChildType, XmCSpinBoxChildType, XmRSpinBoxChildType,
    sizeof(unsigned char), ConstraintOffset(sb_child_type),
    XmRImmediate, (XtPointer) XmSTRING
  },
  { XmNpositionType, XmCPositionType, XmRPositionType,
    sizeof(unsigned char), ConstraintOffset(position_type),
    XmRImmediate, (XtPointer) XmPOSITION_VALUE
  },
  { XmNnumValues, XmCNumValues, XmRInt,
    sizeof(int), ConstraintOffset(num_values),
    XmRImmediate, (XtPointer) 0
  },
  { XmNvalues, XmCValues, XmRXmStringTable,
    sizeof(XmStringTable), ConstraintOffset(values),
    XmRStringTable, NULL
  },
  { XmNminimumValue, XmCMinimumValue, XmRInt,
    sizeof(int), ConstraintOffset(minimum_value),
    XmRImmediate, (XtPointer) 0
  },
  { XmNmaximumValue, XmCMaximumValue, XmRInt,
    sizeof(int), ConstraintOffset(maximum_value),
    XmRImmediate, (XtPointer) 10
  },
  { XmNincrementValue, XmCIncrementValue, XmRInt,
    sizeof(int), ConstraintOffset(increment_value),
    XmRImmediate, (XtPointer) 1
  },
  { XmNdecimalPoints, XmCDecimalPoints, XmRShort,
    sizeof(short), ConstraintOffset(decimal_points),
    XmRImmediate, (XtPointer) 0
  },
  { XmNarrowSensitivity, XmCArrowSensitivity, XmRArrowSensitivity,
    sizeof(unsigned char), ConstraintOffset(arrow_sensitivity),
    XmRImmediate, (XtPointer) XmARROWS_DEFAULT_SENSITIVITY
  },
  { XmNwrap, XmCWrap, XmRBoolean,
    sizeof(Boolean), ConstraintOffset(wrap),
    XmRImmediate, (XtPointer) True
  },
  { XmNposition, XmCPosition, XmRPositionValue,
    sizeof(int), ConstraintOffset(position),
    XmRImmediate, (XtPointer) 0
  }
};

static XmSyntheticResource syn_resources[] =
{
  { XmNspacing, sizeof(Dimension),
    Offset(spacing),
    XmeFromHorizontalPixels,
    XmeToHorizontalPixels
  },
  { XmNmarginHeight, sizeof(Dimension),
    Offset(margin_height),
    XmeFromVerticalPixels,
    XmeToVerticalPixels
  },
  { XmNmarginWidth, sizeof(Dimension),
    Offset(margin_width),
    XmeFromHorizontalPixels,
    XmeToHorizontalPixels
  },
  { XmNdetailShadowThickness, sizeof(Dimension),
    Offset(detail_shadow_thickness),
    XmeFromHorizontalPixels,
    XmeToHorizontalPixels
  }
};

static XmSyntheticResource syn_constraints[] =
{
  { XmNposition, sizeof(int),
    ConstraintOffset(position),
    GetPositionValue,
    SetPositionValue
  }
};



static XmBaseClassExtRec spinBoxBaseClassExtRec = {
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
  NULL,                         /* ClassPartInitPrehook */
  NULL,                         /* classPartInitPosthook*/
  NULL,                         /* ext_resources        */
  NULL,                         /* compiled_ext_resources*/
  0,                            /* num_ext_resources    */
  FALSE,                        /* use_sub_resources    */
  XmInheritWidgetNavigable,     /* widgetNavigable      */
  NULL                          /* focusChange          */
  };

/*  The Spin class record definition  */

externaldef (xmspinboxclassrec) XmSpinBoxClassRec xmSpinBoxClassRec= {
  {
    (WidgetClass)&xmManagerClassRec,    /* superclass */
    "XmSpinBox",                        /* class_name */
    sizeof(XmSpinBoxRec),               /* widget_size */
    ClassInitialize,    		/* class_initialize */
    ClassPartInitialize,                /* class_part_initialize */
    FALSE,    		                /* class_inited */
    Initialize,    	                /* initialize */
    NULL,    		                /* initialize_hook */
    XtInheritRealize,		        /* realize */
    actionsTable,      	                /* actions */
    XtNumber(actionsTable),             /* num_actions */
    resources,    	                /* resources */
    XtNumber(resources),                /* num_resources */
    NULLQUARK,    	                /* xrm_class */
    TRUE,    		                /* compress_motion */
    XtExposeCompressMaximal |          	/* compress_exposure */
	XtExposeNoRegion,
    TRUE,    		              	/* compress_enterleave */
    FALSE,    		              	/* visible_interest */
    Destroy,    		      	/* destroy */
    Resize,			      	/* resize */
    Redisplay,    	              	/* expose */
    SetValues,    	              	/* set_values */
    NULL,    		              	/* set_values_hook */
    XtInheritSetValuesAlmost,          	/* set_values_almost */
    NULL,    		              	/* get_values_hook */
    XtInheritAcceptFocus,	      	/* accept_focus */
    XtVersion,    	              	/* version */
    NULL,    		              	/* callback private */
    defaultTranslations,	        /* tm_table */
    QueryGeometry,    		      	/* query_geometry */
    NULL,    		              	/* display_accelerator */
    (XtPointer)&spinBoxBaseClassExtRec,	/* extension */
  },

  {    /* composite_class fields */
    GeometryManager,                	/* geometry_manager */
    ChangeManaged,    	              	/* change_managed */
    InsertChild,		        /* insert_child */
    XtInheritDeleteChild,	        /* delete_child */
    NULL,    		                /* extension */
  },

  {    /* constraint_class fields */
    constraints,    		      	/* resource list */
    XtNumber(constraints),	      	/* num resources */
    sizeof(XmSpinBoxConstraintRec),    	/* constraint size */
    ConstraintInitialize,              	/* init proc */
    ConstraintDestroy,			/* destroy proc */
    ConstraintSetValues,		/* set values proc */
    NULL,    		             	/* extension */
  },
      /* manager_class fields */
  {
    NULL,    		              	/* translations */
    syn_resources,              	/* syn_resources */
    XtNumber(syn_resources),           	/* num_syn_resources */
    syn_constraints,	              	/* syn_cont_resources */
    XtNumber(syn_constraints),         	/* num_syn_cont_resources */
    NULL,    		              	/* parent_process */
    NULL,    		              	/* extension */
  },
      /* spinbox_class fields */
  {
    NULL,    		              	/* get_callback_widget */
    NULL    		              	/* extension */
  }

};

externaldef(xmspinboxwidgetclass) WidgetClass xmSpinBoxWidgetClass =
       (WidgetClass)&xmSpinBoxClassRec;

static XmConst XmNavigatorTraitRec spinBoxNT =
{
  0,
  SpinNChangeMoveCB,
  SpinNSetValue,
  SpinNGetValue,
};

static XtConvertArgRec selfConvertArgs[] = {
    { XtBaseOffset, (XtPointer) 0, sizeof(int) }
};

/******************************************************************************
 **
 ***			METHODS
 **
 *****************************************************************************/

static void
ClassInitialize(void)
{
  spinAccel = XtParseAcceleratorTable(_XmSpinB_defaultAccelerators);

  /* set up base class extension quark */
  spinBoxBaseClassExtRec.record_type = XmQmotif;
}

static void
ClassPartInitialize(WidgetClass classPart)
{
  XmSpinBoxWidgetClass spinC;

  spinC = (XmSpinBoxWidgetClass) classPart;

  _XmFastSubclassInit(classPart, XmSPINBOX_BIT);

  /* Install the navigator trait for all subclasses */
  XmeTraitSet((XtPointer)spinC, XmQTnavigator, (XtPointer) &spinBoxNT);

  XtSetTypeConverter( XmRString, XmRPositionValue, CvtStringToPositionValue,
                      selfConvertArgs, XtNumber(selfConvertArgs),
                      XtCacheNone, (XtDestructor) NULL) ;

}

static void
Initialize(Widget req,		/* unused */
	   Widget new_w,
	   ArgList args,	/* unused */
	   Cardinal *num_args)	/* unused */
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)new_w;
  XGCValues	  GCvalues;
  XtGCMask	  GCmask, unusedMask;

  spinW->spinBox.textw = 0;
  spinW->spinBox.dim_mask = 0;
  spinW->spinBox.last_hit = 0;
  spinW->spinBox.spin_timer = 0;
  spinW->spinBox.make_change = 0;

  spinW->spinBox.boundary = 0;

  spinW->spinBox.ideal_height = 0;
  spinW->spinBox.ideal_width = 0;

  spinW->spinBox.up_arrow_pressed = False;
  spinW->spinBox.down_arrow_pressed = False;

  spinW->spinBox.up_arrow_rect.x = 0;
  spinW->spinBox.up_arrow_rect.y = 0;
  spinW->spinBox.up_arrow_rect.width = 0;
  spinW->spinBox.up_arrow_rect.height = 0;

  spinW->spinBox.down_arrow_rect.x = 0;
  spinW->spinBox.down_arrow_rect.y = 0;
  spinW->spinBox.down_arrow_rect.width = 0;
  spinW->spinBox.down_arrow_rect.height = 0;

  if (!spinW->core.accelerators)
    spinW->core.accelerators = spinAccel;

  if (spinW->spinBox.initial_delay < 1)
    spinW->spinBox.initial_delay = spinW->spinBox.repeat_delay;

  /* Get arrow GC */
  GCmask = GCForeground | GCBackground | GCGraphicsExposures;
  GCvalues.foreground = spinW->core.background_pixel;
  GCvalues.background = spinW->manager.foreground;
  GCvalues.graphics_exposures = False;

  /* Share gc with scrollbar */
  spinW->spinBox.arrow_gc = XtAllocateGC(new_w, 0, GCmask, &GCvalues,
					 0, GCFont);

  GCmask |= GCFillStyle | GCStipple;
  unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;

  GCvalues.background = spinW->core.background_pixel;
  GCvalues.foreground = spinW->manager.foreground;
  GCvalues.fill_style = FillOpaqueStippled;
  GCvalues.stipple = _XmGetInsensitiveStippleBitmap(new_w);

  /* share GC with ArrowButton */
  spinW->spinBox.insensitive_gc = XtAllocateGC(new_w, 0, GCmask, &GCvalues,
					       GCClipMask, unusedMask);
}

static void
Destroy(Widget w)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)w;

  if (spinW->spinBox.arrow_gc != NULL)
    {
      XtReleaseGC(w, spinW->spinBox.arrow_gc);
      spinW->spinBox.arrow_gc = NULL;
    }

  if (spinW->spinBox.insensitive_gc != NULL)
    {
      XtReleaseGC(w, spinW->spinBox.insensitive_gc);
      spinW->spinBox.insensitive_gc = NULL;
    }
}

static void
Resize(Widget w)
{
  XtWidgetGeometry spinG;

  spinG.width = XtWidth(w);
  spinG.height = XtHeight(w);

  LayoutSpinBox(w, &spinG, NULL);

  if (XtIsRealized(w))
     XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);			/*  Force Redisplay */
}

static void
Redisplay(Widget w,
	  XEvent *event,	/* unused */
	  Region region)	/* unused */
{

  XmSpinBoxWidget	spinW = (XmSpinBoxWidget) w;

  if (XtIsRealized(w))
    {
      ClearArrows(w);

      if (spinW->manager.shadow_thickness > 0)
	{
	  int	width, height;

	  width = (spinW->spinBox.ideal_width < XtWidth(spinW)) ?
		  spinW->spinBox.ideal_width :
		  XtWidth(spinW);

	  height = (spinW->spinBox.ideal_height < XtHeight(spinW)) ?
		  spinW->spinBox.ideal_height :
		  XtHeight(spinW);

	  XmeDrawShadows(
			 XtDisplay(w), XtWindow(w),
		         spinW->manager.top_shadow_GC,
		         spinW->manager.bottom_shadow_GC,
		         0, 0, width, height,
		         spinW->manager.shadow_thickness,
			 XmSHADOW_OUT
		        );
	}

      _XmSetFocusFlag(w,XmFOCUS_IGNORE, False);

      DrawSpinArrow(w, XmARROW_UP);
      DrawSpinArrow(w, XmARROW_DOWN);
    }

}

static Boolean
SetValues(Widget old,
	  Widget req,
	  Widget new_w,
	  ArgList args,		/* unused */
	  Cardinal *num_args )	/* unused */
{
  XtWidgetGeometry spinG;
  XmSpinBoxWidget	 oldW = (XmSpinBoxWidget)old;
  XmSpinBoxWidget	 reqW = (XmSpinBoxWidget)req;
  XmSpinBoxWidget	 newW = (XmSpinBoxWidget)new_w;
  Boolean		 displayFlag;

  displayFlag = False;

  /*  Initial delay must be positive	*/
  if (newW->spinBox.initial_delay < 1)
    newW->spinBox.initial_delay = newW->spinBox.repeat_delay;

  if ((newW->core.sensitive != oldW->core.sensitive) ||
      (newW->core.ancestor_sensitive != oldW->core.ancestor_sensitive))
    displayFlag = True;

  /*  Check for geo changes, if realized	*/
  if (XtIsRealized(new_w) &&
      (reqW->spinBox.arrow_layout  != oldW->spinBox.arrow_layout ||
      reqW->spinBox.margin_width  != oldW->spinBox.margin_width ||
      reqW->spinBox.margin_height != oldW->spinBox.margin_height ||
      reqW->spinBox.spacing       != oldW->spinBox.spacing ||
      reqW->spinBox.arrow_size    != oldW->spinBox.arrow_size))
    {
      spinG.width = 0;
      spinG.height = 0;

      GetSpinSize(new_w, &spinG.width, &spinG.height);
      XtWidth(new_w) = spinG.width;
      XtHeight(new_w) = spinG.height;

      if (XtIsRealized(old))
        ClearArrows(old);

      LayoutSpinBox(new_w, &spinG, NULL);
      displayFlag = True;
    }

  if (reqW->spinBox.default_arrow_sensitivity
      !=  oldW->spinBox.default_arrow_sensitivity ||
      reqW->spinBox.detail_shadow_thickness
      != oldW->spinBox.detail_shadow_thickness)
    displayFlag = True;

  return(displayFlag);
}

static XtGeometryResult
QueryGeometry(Widget w, XtWidgetGeometry *req, XtWidgetGeometry *rep)
{
  XmSpinBoxWidget	 spinW = (XmSpinBoxWidget)w;

  if (!XtIsRealized((Widget)spinW))
    {
      rep->width = XtWidth(w);
      rep->height = XtHeight(w);
    }
  else
    {
      rep->width = 0;
      rep->height = 0;
    }

  GetSpinSize(w, &rep->width, &rep->height);

  return(XmeReplyToQueryGeometry(w, req, rep));
}

static XtGeometryResult
GeometryManager(Widget w,
		XtWidgetGeometry *req,
		XtWidgetGeometry *rep) /* unused */
{
  XtGeometryResult spinResult;
  XtWidgetGeometry spinG;
  XtWidgetGeometry origG;

  if (IsX(req))
    if (w->core.x != req->x)
      return(XtGeometryNo);

  if (IsY(req))
    if (w->core.y != req->y)
      return(XtGeometryNo);

  origG.width = w->core.width;
  origG.height = w->core.height;

  if (IsWidth(req))
    w->core.width = req->width;

  if (IsHeight(req))
    w->core.height = req->height;

  spinG.width = 0;
  spinG.height = 0;

  GetSpinSize(XtParent(w), &spinG.width, &spinG.height);

  spinG.request_mode = (CWWidth | CWHeight);

  spinResult = _XmMakeGeometryRequest(XtParent(w), &spinG);

  if (spinResult == XtGeometryYes)
   {
     LayoutSpinBox(XtParent(w), &spinG, w);

     /*  Force Redisplay */
     if (XtIsRealized(w))
       XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
   }
  else
    {
      w->core.width = origG.width;
      w->core.height = origG.height;

      spinResult = XtGeometryNo;
    }

  return(spinResult);
}

static void
ChangeManaged(Widget w)
{
  XtWidgetGeometry spinG;
  XmSpinBoxWidget	 spinW = (XmSpinBoxWidget) w;
  int		 i;

  if (XtIsRealized(w))
    {
      spinG.width = 0;
      spinG.height =  0;
    }
  else
    {
      spinG.width = XtWidth(w);
      spinG.height = XtHeight(w);
    }

  GetSpinSize(w, &spinG.width, &spinG.height);

  spinG.request_mode = CWWidth | CWHeight;

  _XmMakeGeometryRequest(w, &spinG);

  LayoutSpinBox(w, &spinG, NULL);

  /* Update managed children */
  /* Also make sure that focus is on the last created and managed
   * text/text_field child. If the last text/text_field child is not managed,
   * focus should be on the one previous to that. XmQTaccessTextual trait can
   * not be used to confirm the child to be text or text_field as this trait
   * is held by other widgets such as label as well.
   */
  for (i = 0; i < SB_ChildCount(spinW); i++) {
    if (XtIsManaged(spinW->composite.children[i])) {
      if (XmIsTextField(spinW->composite.children[i]) ||
	  XmIsText(spinW->composite.children[i])) {
	 spinW->spinBox.textw  = spinW->composite.children[i];
     }
      UpdateChildText(spinW->composite.children[i]);
    }
  }
}

static void
InsertChild(Widget newChild)
{
  XmSpinBoxWidget      spinW = (XmSpinBoxWidget)XtParent(newChild);
  XtWidgetProc insert_child;

  /* call manager's InsertChild method */
  _XmProcessLock();
  insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)
     			->composite_class.insert_child;
  _XmProcessUnlock();
  (*insert_child)(newChild);

  if (XmeTraitGet((XtPointer)XtClass(newChild), XmQTaccessTextual) != NULL)
    {
      spinW->spinBox.textw  = newChild;

      XtInsertEventHandler(
			   newChild,
			   FocusChangeMask,
			   False,
			   SpinChildFocusChange,
			   (XtPointer) spinW,
			   XtListHead
			   );

      /* To handle implicit mode,  we also call the focus change
	 if a button is clicked.  The work is finished in the event
	 handler */
      XtInsertEventHandler(
			   newChild,
			   ButtonPressMask,
			   False,
			   SpinChildFocusChange,
			   (XtPointer) spinW,
			   XtListHead
			   );
    }

  XtInstallAccelerators(newChild, (Widget)spinW);
}


static void
ConstraintInitialize(Widget req,
		     Widget new_w,
		     ArgList args, /* unused */
		     Cardinal *num_args) /* unused */
{
  XmSpinBoxConstraint	newC = SB_GetConstraintRec(new_w);
  XmSpinBoxConstraint	reqC = SB_GetConstraintRec(req);
  XmSpinBoxWidget	spinW;
  int			valLoop;
  char			*error = (char *) NULL;

  spinW = (XmSpinBoxWidget)XtParent(new_w);

  /* Numeric Child*/
  if (SB_ChildIsNumeric(newC))
    {
      /* enforce reasonable parameters */
      if (newC->increment_value == 0)
	{
	  XmeWarning(new_w, BAD_SPIN_INCREMENT);
	  newC->increment_value = 1;
	}

      if ((newC->minimum_value < newC->maximum_value
	  && newC->increment_value < 0)
      ||  (newC->minimum_value > newC->maximum_value
	  && newC->increment_value > 0))
	{
	  XmeWarning(new_w, BAD_SPIN_DIRECTION);
	  newC->increment_value *= -1;
	}
    }
  /* String Child*/
  else
    {
    if (reqC->values != NULL)
      {
  /* buffer the values XmStringTable */
      newC->values = (XmString *)XtMalloc(reqC->num_values * sizeof(XmString));

      if (newC->values != NULL)
	for (valLoop = 0; valLoop < reqC->num_values; valLoop++)
	  newC->values[valLoop] = XmStringCopy(reqC->values[valLoop]);
      }

#if 0
    /*
     * This is ifdef'ed out to be BC with DtSpinBox warning messages.
     */
    if (newC->values == NULL || newC->num_values == 0)
      if (ChildIsTraversable(new_w))
	if (XmeTraitGet((XtPointer)XtClass(new_w), XmQTaccessTextual) != NULL)
          XmeWarning(new_w, BAD_SPIN_VALUES);
#endif
    }

  if (newC->position_type != XmPOSITION_VALUE &&
      newC->position_type != XmPOSITION_INDEX)
    {
      newC->position_type = XmPOSITION_VALUE;
      XmeWarning(new_w, BAD_SPIN_POSITION_TYPE);
    }

  error = ValidatePositionValue(newC, &newC->position);
  if (error)
    XmeWarning(new_w, error);

  spinW->spinBox.up_arrow_pressed = False;
  spinW->spinBox.down_arrow_pressed = False;

  /* No reason to do the work until the child is managed */
  if (XtIsManaged(new_w))
    UpdateChildText(new_w);
}

static void
ConstraintDestroy(
        Widget w )
{
  XmSpinBoxConstraint	spinC = SB_GetConstraintRec(w);
  int			itemLoop;

      /* give back the old values XmStringTable */
  if (spinC->values != NULL)
    {
      for (itemLoop = 0; itemLoop < spinC->num_values; itemLoop++)
	XmStringFree(spinC->values[itemLoop]);

      XtFree((char*)spinC->values);

      spinC->values = NULL;
      spinC->num_values = 0;
    }
}

static Boolean
ConstraintSetValues(Widget   old,
		    Widget   req,
		    Widget   new_w,
		    ArgList  args, /* unused */
		    Cardinal *num_args ) /* unused */
{
  XmSpinBoxConstraint  oldC = SB_GetConstraintRec(old);
  XmSpinBoxConstraint  reqC = SB_GetConstraintRec(req);
  XmSpinBoxConstraint  newC = SB_GetConstraintRec(new_w);
  XmSpinBoxWidget      spinW = (XmSpinBoxWidget)XtParent(new_w);
  Boolean	       redisplayText = False;
  int		       nvi;
  int		       valLoop;
  char		       *error = (char *) NULL;


  /*
   * These resources have CG permissions only:
   *	XmNspinBoxChildType XmNpositionType
   */
  if (newC->position_type != oldC->position_type)
    {
      newC->position_type = oldC->position_type;
      XmeWarning(new_w, BAD_SPIN_POSITION_TYPE);
    }
  /*
   * BINARY COMPATIBILITY with DTSPINBOX
   *
   * However, DtSpinBox does not prevent setting XmNspinBoxChildType
   * so we have to allow it.
   * newC->sb_child_type = oldC->sb_child_type;
   */
  if (newC->sb_child_type != oldC->sb_child_type)
    redisplayText = True;

  /**** Numeric Child ****/
  if (SB_ChildIsNumeric(newC))
    {
      /* enforce reasonable parameters */
      if (newC->increment_value == 0)
	{
          XmeWarning(new_w, BAD_SPIN_INCREMENT);
	  newC->increment_value = 1;
	}

      /* if something has changed ... */
      if (newC->minimum_value  != oldC->minimum_value
	  ||  newC->maximum_value  != oldC->maximum_value
	  ||  newC->increment_value != oldC->increment_value
	  ||  newC->decimal_points != oldC->decimal_points
	  ||  newC->position != oldC->position)
	{
	  redisplayText = True;

	  /* force the step to go the right way */
	  if ((newC->minimum_value < newC->maximum_value
	      && newC->increment_value < 0)
	  ||  (newC->minimum_value > newC->maximum_value
	      && newC->increment_value > 0))
	      {
	        XmeWarning(new_w, BAD_SPIN_DIRECTION);
	        newC->increment_value *= -1;
	      }

	  error = ValidatePositionValue(newC, &newC->position);
	  if (error)
 	    XmeWarning(new_w, error);
	}
    }

  /****  String Child  ****/
  else  if (ChildIsTraversable(new_w)) {
  /* buffer the new values XmStringTable */
      if (reqC->values == NULL)
	reqC->values = oldC->values;
      else if (reqC->values != oldC->values)
	{
	  newC->values =
	   (XmString *)XtMalloc(reqC->num_values * sizeof(XmString));

	  if (newC->values != NULL)
	    for (valLoop = 0; valLoop < reqC->num_values; valLoop++)
	      newC->values[valLoop] = XmStringCopy(reqC->values[valLoop]);
	}

      error = ValidatePositionValue(newC, &newC->position);
      if (error)
 	XmeWarning(new_w, error);

#if 0
      if (newC->values == NULL || newC->num_values == 0)
	if (XmeTraitGet((XtPointer)XtClass(new_w), XmQTaccessTextual) != NULL)
	  XmeWarning(new_w, BAD_SPIN_VALUES);
#endif

      if ((newC->position != oldC->position)
      ||  (newC->values != oldC->values)
      ||  (newC->num_values < oldC->num_values
	   && newC->position > newC->num_values))
	redisplayText = True;

    /* give back the old values XmStringTable */
    if (reqC->values != oldC->values) {
        if (oldC->values != NULL)
	  for (nvi = 0; nvi < oldC->num_values; nvi++)
	    XmStringFree(oldC->values[nvi]);

        XtFree((char*)oldC->values);

        oldC->values = NULL;
    }
   }

  /* If the current focus child is the one requesting the change */
  if (XtIsRealized((Widget)spinW) && spinW->spinBox.textw == new_w)
    {
    if (newC->arrow_sensitivity != oldC->arrow_sensitivity)
      {
      DrawSpinArrow((Widget) spinW, XmARROW_UP);
      DrawSpinArrow((Widget) spinW, XmARROW_DOWN);
      }
    }

  if (redisplayText)
    UpdateChildText(new_w);

  return(False);
}

/******************************************************************************
 **
 ***			EVENT HANDLERS
 **
 *****************************************************************************/

/******************************************************************************
 * SpinChildFocusChange
 *   Event Handler for Focus Change.
 *****************************************************************************/

static void
SpinChildFocusChange(Widget focusWidget,
		     XtPointer focusClient,
		     XEvent *focusEvent,
		     Boolean *focusContinue) /* unused */
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)focusClient;

  if (_XmGetFocusPolicy((Widget) focusClient) == XmEXPLICIT) {
    if (focusEvent->type == FocusIn) {
      if (spinW->spinBox.textw != focusWidget)
	{
	  spinW->spinBox.textw = focusWidget;
	}
    }
  } else {
    /* Only care if this is BSelect */
    if (focusEvent->type == ButtonPress &&
	focusEvent->xbutton.button == Button1) {
      if (spinW->spinBox.textw != (Widget) NULL) {
	Widget child = spinW->spinBox.textw;
	WidgetClass wc = XtClass(child);

	if (XmIsPrimitive(child)) {
	  (*(((XmPrimitiveWidgetClass) wc)
	     ->primitive_class.border_unhighlight))(child) ;
	}
      }
      spinW->spinBox.textw = focusWidget;
      if (spinW->spinBox.textw != (Widget) NULL) {
	Widget child = spinW->spinBox.textw;
      	WidgetClass wc = XtClass(child);

	if (XmIsPrimitive(child)) {
	  (*(((XmPrimitiveWidgetClass) wc)
	     ->primitive_class.border_highlight))(child) ;
	}
      }
    }
  }


  if (focusWidget != (Widget) NULL) {
    DrawSpinArrow((Widget)focusClient, XmARROW_UP);
    DrawSpinArrow((Widget)focusClient, XmARROW_DOWN);
  }
}

/******************************************************************************
 **
 ***			ACTIONS
 **
 *****************************************************************************/

/*****************************************************************************
 * The Enter and Leave actions deal with implicit mode
 * (keyboardFocusPolicy == POINTER),  where we need to indicate to the user
 * which child widget is to be updated on a mouse action.
 *****************************************************************************/
static void
SpinBEnter(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)widget;
  Widget child = (Widget) spinW -> spinBox.textw;

  /* We only perform this action for POINTER mode */
  if (_XmGetFocusPolicy(widget) != XmPOINTER) return;

  if (child != (Widget) NULL) {
    WidgetClass wc = XtClass(child);

    if (XmIsPrimitive(child)) {
      (*(((XmPrimitiveWidgetClass) wc)
	 ->primitive_class.border_highlight))(child) ;
    }
  }
}

static void
SpinBLeave(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)widget;
  Widget child = (Widget) spinW -> spinBox.textw;

  /* We only perform this action for POINTER mode */
  if (_XmGetFocusPolicy(widget) != XmPOINTER) return;

  if (child != (Widget) NULL) {
    WidgetClass wc = XtClass(child);

    if (XmIsPrimitive(child)) {
      (*(((XmPrimitiveWidgetClass) wc)
	 ->primitive_class.border_unhighlight))(child) ;
    }
  }
}

/******************************************************************************
 * SpinBArm
 *	Action for BSelect Down.
 *****************************************************************************/

static void
SpinBArm(Widget armWidget,
	 XEvent *armEvent,
	 String *armParams,	/* unused */
	 Cardinal *armCount)	/* unused */
{
  if (armEvent->type == ButtonPress)
  {
    if (ArrowWasHit(armWidget, XmARROW_UP, armEvent))
      SpinBAction(armWidget, XmARROW_UP);
    else if (ArrowWasHit(armWidget, XmARROW_DOWN, armEvent))
      SpinBAction(armWidget, XmARROW_DOWN);
  }
}

/******************************************************************************
 * SpinBDisarm
 *	Action for BSelect Up.
 *****************************************************************************/

static void
SpinBDisarm(Widget   disarmWidget,
	    XEvent   *disarmEvent,
	    String   *disarmParams, /* unused */
	    Cardinal *disarmCount) /* unused */
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)disarmWidget;

  if (spinW->spinBox.up_arrow_pressed || spinW->spinBox.down_arrow_pressed)
    {
      if (spinW->spinBox.initial_delay > 0 && spinW->spinBox.repeat_delay > 0)
	if (spinW->spinBox.spin_timer)
	  XtRemoveTimeOut(spinW->spinBox.spin_timer);

      spinW->spinBox.up_arrow_pressed = False;
      spinW->spinBox.down_arrow_pressed = False;

      DrawSpinArrow(disarmWidget, XmARROW_UP);
      DrawSpinArrow(disarmWidget, XmARROW_DOWN);

      if (spinW->spinBox.make_change)
	{
	  if (spinW->spinBox.last_hit == XmARROW_UP)
	    ArrowSpinUp(disarmWidget, disarmEvent);
	  else if (spinW->spinBox.last_hit == XmARROW_DOWN)
	    ArrowSpinDown(disarmWidget, disarmEvent);
	}

      ArrowCallback(disarmWidget, disarmEvent, XmCR_OK);
    }

  spinW->spinBox.make_change = False;
}

/******************************************************************************
 * SpinBFirst
 *	Action for BeginData Key.
 *****************************************************************************/

static void
SpinBFirst(Widget   firstWidget,
	   XEvent   *firstEvent,
	   String   *firstParams, /* unused */
	   Cardinal *firstCount) /* unused */
{
  XmSpinBoxConstraint spinC;
  XmSpinBoxWidget	  spinW = (XmSpinBoxWidget)firstWidget;
  Widget		  child;
  int		  savePosition;

  child = XtWindowToWidget(XtDisplay(firstWidget), firstEvent->xany.window);

  child = spinW->spinBox.textw;

  if (WidgetIsChild(spinW, child) && DownArrowSensitive(spinW))
    {
      spinW->spinBox.textw = child;

      spinC = SB_GetConstraintRec(child);

      savePosition = spinC->position;
      spinC->position = SB_ChildMinimumPositionValue(spinC);

      if (ArrowVerify((Widget)spinW, firstEvent, XmCR_SPIN_FIRST))
	{
	  UpdateChildText(spinW->spinBox.textw);
	  ArrowCallback((Widget)spinW, firstEvent, XmCR_SPIN_FIRST);
	  ArrowCallback((Widget)spinW, firstEvent, XmCR_OK);
	}
      else
	spinC->position = savePosition;
    }
}

/******************************************************************************
 * SpinBLast
 *	Action for EndOfData Key.
 *****************************************************************************/

static void
SpinBLast(Widget lastWidget,
	  XEvent *lastEvent,
	  String *lastParams,	/* unused */
	  Cardinal *lastCount)	/* unused */
{
  XmSpinBoxConstraint spinC;
  XmSpinBoxWidget 	  spinW = (XmSpinBoxWidget)lastWidget;
  Widget		  child;
  int 		  savePosition;

  child = XtWindowToWidget(XtDisplay(lastWidget), lastEvent->xany.window);
  child = spinW->spinBox.textw;

  if (WidgetIsChild(spinW, child) && UpArrowSensitive(spinW))
    {
      spinW->spinBox.textw = child;
      spinC = SB_GetConstraintRec(child);

      savePosition = spinC->position;
      spinC->position = SB_ChildMaximumPositionValue(spinC);

      if (ArrowVerify((Widget)spinW, lastEvent, XmCR_SPIN_LAST))
	{
	  UpdateChildText(spinW->spinBox.textw);
	  ArrowCallback((Widget)spinW, lastEvent, XmCR_SPIN_LAST);
	  ArrowCallback((Widget)spinW, lastEvent, XmCR_OK);
	}
      else
	spinC->position = savePosition;
    }
}

/******************************************************************************
 * SpinBLeft
 *	Action for Left Arrow.
 *****************************************************************************/

static void
SpinBLeft(Widget   leftWidget, XEvent   *leftEvent,
	  String   *leftParams, Cardinal *leftCount)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)leftWidget;

  if (LayoutIsRtoLM(spinW))
    SpinBNext(leftWidget, leftEvent, leftParams, leftCount);
  else
    SpinBPrior(leftWidget, leftEvent, leftParams, leftCount);
}

/******************************************************************************
 * SpinBNext
 *	Action for UpArrow Key.
 *****************************************************************************/

static void
SpinBNext(Widget nextWidget,
	  XEvent *nextEvent,	/* unused */
	  String *nextParams,	/* unused */
	  Cardinal *nextCount)	/* unused */
{
  SpinBAction(nextWidget, XmARROW_UP);
}

/******************************************************************************
 * SpinBPrior
 *	Action for DownArrow Key.
 *****************************************************************************/

static void
SpinBPrior(Widget   priorWidget,
	   XEvent   *priorEvent, /* unused */
	   String   *priorParams, /* unused */
	   Cardinal *priorCount ) /* unused */
{
  SpinBAction(priorWidget, XmARROW_DOWN);
}

/******************************************************************************
 * SpinBRight
 *	Action for RightArrow Key.
 *****************************************************************************/
static void
SpinBRight(Widget   rightWidget, XEvent   *rightEvent,
	   String   *rightParams, Cardinal *rightCount)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)rightWidget;

  if (LayoutIsRtoLM(spinW))
    SpinBPrior(rightWidget, rightEvent, rightParams, rightCount);
  else
    SpinBNext(rightWidget, rightEvent, rightParams, rightCount);
}


/******************************************************************************
 **
 ***			OTHER FUNCTIONS
 **
 *****************************************************************************/

/******************************************************************************
 *  ClearArrows
 *	Erase Exisiting Arrows
 *****************************************************************************/
static void
ClearArrows(Widget clearW)
{
  XClearArea(XtDisplay(clearW), XtWindow(clearW), 0, 0, 0, 0, False);
}

/******************************************************************************
 * UpArrowSensitive
 *   Returns sensitive/insensitive for arrow.
 *****************************************************************************/
static Boolean
UpArrowSensitive(XmSpinBoxWidget spinW)
{
  XmSpinBoxConstraint spinC;
  unsigned char upState;

  if (XtIsSensitive((Widget) spinW) != True)
    upState = (unsigned char)XmARROWS_INSENSITIVE;
  else if (SB_ChildCount(spinW) && SB_WithChild(spinW))
    {
      spinC = SB_GetConstraintRec(spinW->spinBox.textw);

      upState = spinC->arrow_sensitivity;
    }
  else
    upState = (unsigned char)XmARROWS_DEFAULT_SENSITIVITY;

  if (upState == (unsigned char)XmARROWS_DEFAULT_SENSITIVITY)
    upState = spinW->spinBox.default_arrow_sensitivity;

  return(upState & (unsigned char)XmARROWS_INCREMENT_SENSITIVE);
}

/******************************************************************************
 * DownArrowSensitive
 *   Returns sensitive/insensitive for arrow.
 *****************************************************************************/
static Boolean
DownArrowSensitive(XmSpinBoxWidget spinW)
{
  XmSpinBoxConstraint spinC;
  unsigned char	  downState;

  if (XtIsSensitive((Widget) spinW) != True)
    downState = (unsigned char)XmARROWS_INSENSITIVE;
  else if (SB_ChildCount(spinW) && SB_WithChild(spinW))
    {
      spinC = SB_GetConstraintRec(spinW->spinBox.textw);

      downState = spinC->arrow_sensitivity;
    }
  else
    downState = (unsigned char)XmARROWS_DEFAULT_SENSITIVITY;

  if (downState == (unsigned char)XmARROWS_DEFAULT_SENSITIVITY)
    downState = spinW->spinBox.default_arrow_sensitivity;

  return(downState & (unsigned char)XmARROWS_DECREMENT_SENSITIVE);
}


/******************************************************************************
 * NumericChildCount
 *	Return Number of Numeric Children.
 *****************************************************************************/
static int
NumericChildCount(XmSpinBoxWidget spinW)
{
  XmSpinBoxConstraint  spinC;
  int		   i;
  int		   childCount;

  childCount = 0;

  if (SB_WithChild(spinW))
    for (i = 0; i < SB_ChildCount(spinW); i++)
      {
	spinC = SB_GetConstraintRec(spinW->composite.children[i]);

	if (SB_ChildIsNumeric(spinC))
	  childCount++;
      }

  return(childCount);
}

/******************************************************************************
 * WidgetIsChild
 *	Return True if Widget is SpinBox Child
 *****************************************************************************/
static Boolean
WidgetIsChild(XmSpinBoxWidget spinW, Widget child)
{
  Boolean childFlag;
  int     i;

  childFlag = False;

  if (SB_WithChild(spinW))
    for (i = 0; i < SB_ChildCount(spinW); i++)
      if (spinW->composite.children[i] == child)
	{
	  childFlag = True;
	  break;
	}

  return(childFlag);
}

/******************************************************************************
 *  ChildIsTraversable
 *	Return True if XmNtraversalOn is set for child.
 *****************************************************************************/
static Boolean
ChildIsTraversable(Widget w)
{
  Boolean traverseFlag;
  Arg	  argList[2];
  int	  n = 0;

  XtSetArg(argList[n], XmNtraversalOn, &traverseFlag); n++;
  XtGetValues(w, argList, n);

  return(traverseFlag);
}

/******************************************************************************
 *  GetArrowDirection
 *	Returns the direction in which the arrow should be drawn
 *
 *	widget	- the spin box widget whose arrows are being drawn.
 *	spinDir - the direction the arrow should cause the spinbox to spin.
 *****************************************************************************/
static int
GetArrowDirection(Widget w, int spinDir)
{
  int arrowDirection;
  int downDirection;
  int upDirection;
  int isRtoL;

  isRtoL = (int) LayoutIsRtoLM(w);

  if (SB_GetArrowOrientation(w) == (unsigned char) XmARROWS_VERTICAL)
    {
      upDirection = XmARROW_UP;
      downDirection = XmARROW_DOWN;
    }
  else
    {
      if (isRtoL)
	{
	  upDirection   = XmARROW_LEFT;
	  downDirection = XmARROW_RIGHT;
        }
      else
	{
	  upDirection   = XmARROW_RIGHT;
	  downDirection = XmARROW_LEFT;
	}
    }

  if (spinDir == XmARROW_UP)
	arrowDirection = upDirection;
  else
	arrowDirection = downDirection;

  return(arrowDirection);
}

/******************************************************************************
 * LayoutSpinBox
 *	Position Children and Arrows.
 *
 * The ideal layout of the children and arrows have the centerline of
 * the arrow layout running through the center of the median child
 * position.  The median child position is roughly at the center of the
 * widget,  modified by the baseline positions.
 *
 * Degradation behavior will first sacrifice the margins if the widget
 * isn't given enough room.  Following this will be the spacing between
 * widgets,  then finally the children will be shrunk (although the arrows
 * will not be).
 *
 * At beta,  this code will use the approximation of the centerline
 * of the widget for the center of the child position and will not
 * perform the widget shrinking part of the graceful degradation of
 * layout.
 *
 * At final,  this should be updated.
 *****************************************************************************/

typedef XmSpinBoxRec MySpinBoxRec;

static void
LayoutSpinBox(Widget w,
	      XtWidgetGeometry *spinG,
	      Widget child)	/* unused */
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget) w;
  MySpinBoxRec  *myW = (MySpinBoxRec *) w;
  int		arrowLayout;
  int		arrowSize;
  int		i;

  int		marginX, marginY;
  int		numArrowsX, numArrowsY;
  int		spacingX, spacingY;
  int		posX, posY;
  int		Xposition, Xmargin;

  /* May need to update this if stacked arrows are put back */
  Position	upX =0 ;
  Position	downX  = 0;

  arrowLayout = (int) spinW->spinBox.arrow_layout;
  arrowSize = spinW->spinBox.arrow_size;

  /*
   * Figure the starting position of the arrows and children
   * in the X direction:
   *
   *	1.  If there is enough room for our ideal width, then use the
   *	    .margin_width and .spacing resources to arrange the SpinBox.
   *    2.  If not enough room, give up the .margin_width spacing.
   *	3.  If still not enough room, shrink the spacing between
   *	    subwidgets.
   */
  spacingX = spinW->spinBox.spacing;
  marginX = spinW->spinBox.margin_width + SB_ShadowPixels(spinW);
  numArrowsX = SB_NumArrowsWide(spinW);
  if (spinW->spinBox.ideal_width > spinG->width)
    {
      int requiredWidth = spinW->spinBox.ideal_width -
		          (2 * spinW->spinBox.margin_width);

      marginX = 0;
      if (requiredWidth > spinG->width)
	{
          int spacesX = SB_ChildCount(spinW) + numArrowsX;
	  int deltaX = requiredWidth - spinG->width;

	  spacingX = ((spacesX * spinW->spinBox.spacing) - deltaX) / spacesX;
	  if (spacingX < 0)
	    spacingX = 0;
	}
    }

  /*
   * Figure the starting position of the arrows and children
   * in the Y direction.  Use the same algorithm as for the
   * X direction with the following addition:
   *
   *	1a. If there is more room than required for our ideal width,
   *        expand the margins 'til the arrows are centered vertically.
   */
  spacingY = spinW->spinBox.spacing;
  numArrowsY = SB_NumArrowsHigh(spinW);
  if (spinW->spinBox.ideal_height > spinG->height)
    {
      int requiredHeight = spinW->spinBox.ideal_height -
		          (2 * spinW->spinBox.margin_height);

      marginY = 0;
      if (requiredHeight > spinG->height)
	{
	  int deltaY = requiredHeight - spinG->width;

	  spacingY =
	    ((numArrowsY * spinW->spinBox.spacing) - deltaY) / numArrowsY;
	  if (spacingY < 0)
	    spacingY = 0;
	}
    }
  else
    {
      int arrowsSpace = ((numArrowsY * arrowSize) +
			 ((numArrowsY - 1) * spinW->spinBox.spacing));
      marginY = (spinG->height - arrowsSpace) / 2;
    }

  /*
   * Get the starting position of the first SpinBox child in the X direction.
   */
  posX = marginX;
  switch(arrowLayout)
    {
      case XmARROWS_BEGINNING:
      case XmARROWS_FLAT_BEGINNING:
	if (!LayoutIsRtoLM(w))
          posX += (numArrowsX * (arrowSize + spacingX));
        break;
      case XmARROWS_SPLIT:
        posX += ((numArrowsX / 2) * (arrowSize + spacingX));
        break;
      case XmARROWS_END:
      case XmARROWS_FLAT_END:
	if (LayoutIsRtoLM(w))
          posX += (numArrowsX * (arrowSize + spacingX));
        break;
      default:
        break;
    }

  /*
   * Now position the managed children of the SpinBox.
   */
  for (i = 0; i < SB_ChildCount(spinW); i++)
    {
      Widget	childW = spinW->composite.children[i];

      if (w != childW && XtIsManaged(childW))
	{
	  posY = (spinG->height - XtHeight(childW)) / 2;
	  XmeConfigureObject(childW, posX, posY,
			     ((Widget)childW)->core.width,
			     ((Widget)childW)->core.height,
			     ((Widget)childW)->core.border_width
			     );

	  posX += XtWidth(childW) + spacingX;
	}
    }

  /*
   * Save the dimensions of the up and down arrows
   * for use by the arrow drawing procedure.
   */
  spinW->spinBox.up_arrow_rect.width =
    spinW->spinBox.up_arrow_rect.height =
      spinW->spinBox.down_arrow_rect.width =
	spinW->spinBox.down_arrow_rect.height = arrowSize;

  /*
   * Save the X and Y positions of the up and down arrows
   * for use by the arrow drawing procedure.
   * NOTE:  The window origin for X windows is the upper left-hand
   *        corner.  Therefore, the .up_arrow_rect.y gets the smaller
   *	    Y component.
   */
  spinW->spinBox.up_arrow_rect.y = marginY;
  spinW->spinBox.down_arrow_rect.y =
    marginY + ((numArrowsY - 1) * (spacingY + arrowSize));

  if (LayoutIsRtoLM(w))
    {
      Xposition = marginX;
      Xmargin   = posX;
    }
  else
    {
      Xposition = posX;
      Xmargin   = marginX;
    }
  switch(arrowLayout)
    {
      case XmARROWS_BEGINNING:
	downX = upX = Xmargin;
	break;
      case XmARROWS_FLAT_BEGINNING:
	if(LayoutIsRtoLM(w))
	  {
	    upX   = Xmargin;
	    downX = Xmargin + spacingX + arrowSize;
	  }
	else
	  {
	    upX   = Xmargin + spacingX + arrowSize;
	    downX = Xmargin;
	  }
	break;
      case XmARROWS_SPLIT:
	upX   = Xposition;
	downX = Xmargin;
	break;
      case XmARROWS_END:
	downX = upX = Xposition;
	break;
      case XmARROWS_FLAT_END:
	if(LayoutIsRtoLM(w))
	  {
	    upX   = Xposition;
	    downX = Xposition + spacingX + arrowSize;
	  }
	else
	  {
	    upX   = Xposition + spacingX + arrowSize;
	    downX = Xposition;
	  }
	break;
      default:
	break;
    }
    spinW->spinBox.up_arrow_rect.x = upX;
    spinW->spinBox.down_arrow_rect.x = downX;
}

/******************************************************************************
 * NumToString
 *   Convert Number to String to Be Displayed in Child
 *****************************************************************************/

static void
NumToString(char **buffer, int min, int max, int decimal, int value)
{
  float result;
  int   digits;
  int   test;

  digits = 0;

  if (decimal < 1)
    decimal = 0;

  /*
   * BINARY COMPATIBILITY with DTSPINBOX
   *
   * This causes spaces to be insterted in the string passed back in
   * buffer.  DtSpinBox does not do this so we have to alter the
   * behavior.
   *
   * test = MAX((int)abs(min), (int)abs(max));
   */
  if (value == 0)
  {
    digits = 1;
    if (decimal > 0)
      digits += decimal + 1;
  }
  else
  {
    test = abs(value);

    while (test > 0)
    {
      test = test / 10;
      digits++;
    }

    if (decimal > 0)
      digits = (digits <= decimal) ? decimal + 2 : digits + 1;

    if (value < 0)
      digits++;
  }

  test = decimal;
  result = (float)value;
  while (test > 0)
  {
    test--;
    result/=10.0;
  }

  *buffer = (char *)XtMalloc((digits + 1) * sizeof(char));
  if (*buffer)
  {
#ifdef __osf__
    if (decimal == 0)
      sprintf(*buffer, "%*.0f", digits, result);
    else
#endif
      sprintf(*buffer, "%*.*f", digits, decimal, result);
  }
}

/******************************************************************************
 * DrawSpinArrow
 *	Draw a Left or Right Arrow.
 *****************************************************************************/
static void
DrawSpinArrow(Widget arrowWidget, int arrowFlag)
{
  XmSpinBoxWidget	spinW = (XmSpinBoxWidget)arrowWidget;
  Dimension	arrowHeight;
  Dimension	arrowWidth;
  Position	arrowX;
  Position	arrowY;
  Boolean	arrowPressed;
  int		arrowDirection;
  GC		arrowGC;

  if (XtIsRealized((Widget)spinW))
    {
      arrowPressed = False;

      if (arrowFlag == XmARROW_UP)
	{
	  arrowX = spinW->spinBox.up_arrow_rect.x;
	  arrowY = spinW->spinBox.up_arrow_rect.y;
	  arrowWidth = spinW->spinBox.up_arrow_rect.width;
	  arrowHeight = spinW->spinBox.up_arrow_rect.height;

	  if (UpArrowSensitive(spinW))
	    {
	      arrowGC = spinW->spinBox.arrow_gc;
	      arrowPressed = spinW->spinBox.up_arrow_pressed;
	    }
	  else {
	    arrowGC = spinW->spinBox.insensitive_gc;
	    XSetClipMask(XtDisplay(arrowWidget), arrowGC, None);
	  }
	}
      else
	{
	  arrowX = spinW->spinBox.down_arrow_rect.x;
	  arrowY = spinW->spinBox.down_arrow_rect.y;
	  arrowWidth = spinW->spinBox.down_arrow_rect.width;
	  arrowHeight = spinW->spinBox.down_arrow_rect.height;

	  if (DownArrowSensitive(spinW))
	    {
	      arrowGC = spinW->spinBox.arrow_gc;
	      arrowPressed = spinW->spinBox.down_arrow_pressed;
	    }
	  else {
	    arrowGC = spinW->spinBox.insensitive_gc;
	    XSetClipMask(XtDisplay(arrowWidget), arrowGC, None);
	  }
	}

      arrowWidth  = (arrowWidth  > 1) ? arrowWidth  - 1 : 0;
      arrowHeight = (arrowHeight > 1) ? arrowHeight - 1 : 0;
      arrowDirection = GetArrowDirection(arrowWidget, arrowFlag);

      XmeDrawArrow(
		   XtDisplay(arrowWidget),
		   XtWindow(arrowWidget),
		   arrowPressed ? spinW->manager.bottom_shadow_GC :
		   spinW->manager.top_shadow_GC,
		   arrowPressed ? spinW->manager.top_shadow_GC :
		   spinW->manager.bottom_shadow_GC,
		   arrowGC,
		   arrowX,
		   arrowY,
		   arrowWidth,
		   arrowHeight,
		   spinW->spinBox.detail_shadow_thickness,
		   arrowDirection
		   );
    }
}

/******************************************************************************
 * SpinTimeOut
 *	Add TimeOut for Spinning.
 *****************************************************************************/
static void
SpinTimeOut(Widget w, int spinDelay)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)w;

  if (spinW->spinBox.initial_delay > 0 && spinW->spinBox.repeat_delay > 0)
    spinW->spinBox.spin_timer = XtAppAddTimeOut(
					       XtWidgetToApplicationContext(w),
					       spinDelay,
					       SpinBArrow,
					       (XtPointer)w
					       );
}

/******************************************************************************
 * UpdateChildText()
 *   Updates the text widget with the current selection, by position.
 *****************************************************************************/
static void
UpdateChildText(Widget textW)
{
  XmAccessTextualTrait  textT;
  XmSpinBoxConstraint   textC;
  char                 *buffer = NULL;

  textT = (XmAccessTextualTrait)
    XmeTraitGet((XtPointer)XtClass(textW), XmQTaccessTextual);

  if (textT == NULL)
    return;

  if (textW)
    {
      textC = SB_GetConstraintRec(textW);

      if (SB_ChildIsNumeric(textC))
	{
	  NumToString(&buffer,
		      textC->minimum_value,
		      textC->maximum_value,
		      textC->decimal_points,
		      textC->position );

	  textT->setValue(textW, (XtPointer) buffer, XmFORMAT_MBYTE);

	  if (buffer)
	    XtFree(buffer);
	}
      else
	if (textC->values != NULL && textC->num_values)
	  {
	    textT->setValue(textW,
			    (XtPointer)
			    textC->values[textC->position],
			    XmFORMAT_XmSTRING);
	  }
    }
}

/******************************************************************************
 * ArrowWasHit
 *  Returns True if Pointer was Over Arrow When Bselect was Issued.
 *****************************************************************************/
static Boolean
ArrowWasHit(Widget arrowW, int arrowType, XEvent *arrowEvent)
{
  XmSpinBoxWidget  spinW;
  XButtonEvent *hitEvent;
  XRectangle   arrowArea;
  int          arrowHit;
  int	     hitX;
  int	     hitY;

  arrowHit = False;

  if (arrowEvent->type == ButtonPress)
    {
      spinW = (XmSpinBoxWidget)arrowW;

      hitEvent = (XButtonEvent *)arrowEvent;

      if (arrowType == XmARROW_UP)
	arrowArea = spinW->spinBox.up_arrow_rect;
      else
	arrowArea = spinW->spinBox.down_arrow_rect;

      hitX = hitEvent->x - arrowArea.x;		/* Normalize Event Position */
      hitY = hitEvent->y - arrowArea.y;

      if (hitX < 0 || hitX > arrowArea.width
	  ||  hitY < 0 || hitY > arrowArea.height)
	arrowHit = False;
      else
	arrowHit = True;
    }

  return(arrowHit);
}

/******************************************************************************
 * SpinBArrow
 *	Function Called by TimeOut.
 *****************************************************************************/

static void
SpinBArrow(XtPointer spinData,
	   XtIntervalId *spinInterval) /* unused */
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)spinData;

  spinW->spinBox.make_change = False;

  if (spinW->spinBox.up_arrow_pressed)
    {
      if (UpArrowSensitive(spinW))
	{
	  SpinTimeOut((Widget) spinData, spinW->spinBox.repeat_delay);
	  DrawSpinArrow((Widget) spinData, XmARROW_UP);
	  ArrowSpinUp((Widget) spinData, (XEvent *) NULL);
	}
      else
	{
	  spinW->spinBox.up_arrow_pressed = False;

	  DrawSpinArrow((Widget) spinData, XmARROW_UP);
	}
    }
  else if (spinW->spinBox.down_arrow_pressed)
    {
      if (DownArrowSensitive(spinW))
	{
	  SpinTimeOut((Widget) spinData, spinW->spinBox.repeat_delay);
	  DrawSpinArrow((Widget) spinData, XmARROW_DOWN);
	  ArrowSpinDown((Widget) spinData, (XEvent *) NULL);
	}
      else
	{
	  spinW->spinBox.down_arrow_pressed = False;

	  DrawSpinArrow((Widget)spinData, XmARROW_DOWN);
	}
    }
}

/******************************************************************************
 * SpinBAction
 *	This Function Does the Work.
 *****************************************************************************/
static void
SpinBAction(Widget   actionWidget, short    arrowHit)
{
  XmSpinBoxWidget	spinW = (XmSpinBoxWidget)actionWidget;
  Boolean		upHit;
  Boolean		downHit;

  upHit = (arrowHit == XmARROW_UP);
  downHit = (arrowHit == XmARROW_DOWN);

  if ((upHit && UpArrowSensitive(spinW))
      || (downHit && DownArrowSensitive(spinW)))
    {
      spinW->spinBox.make_change = True;
      spinW->spinBox.last_hit = arrowHit;

      if (SB_ChildCount(spinW) && SB_WithChild(spinW))
	XmProcessTraversal(spinW->spinBox.textw, XmTRAVERSE_CURRENT);

      if (upHit)
	{
	  spinW->spinBox.up_arrow_pressed = True;
	  DrawSpinArrow(actionWidget, XmARROW_UP);
	}
      else if (downHit)
	{
	  spinW->spinBox.down_arrow_pressed = True;
	  DrawSpinArrow(actionWidget, XmARROW_DOWN);
	}

      if (spinW->spinBox.initial_delay)
	SpinTimeOut(actionWidget, spinW->spinBox.initial_delay);
    }
  else
    spinW->spinBox.make_change = False;
}

/******************************************************************************
 * FireCallbacks()
 *	Setup Callback(s) for SpinBox.
 *****************************************************************************/
static void
FireCallbacks(XmSpinBoxCallbackStruct	*spinBoxCallData,
	      XtCallbackList	callbackList,
     	      Widget		arrowWidget,
     	      XEvent		*arrowEvent,
     	      int		arrowReason)
{
  XmSpinBoxConstraint		spinC;
  XmSpinBoxWidget               spinW;
  XmSpinBoxWidgetClass          spinWC;
  XmString			xmString = (XmString) NULL;

  spinW = (XmSpinBoxWidget) arrowWidget;
  spinWC = (XmSpinBoxWidgetClass) XtClass(arrowWidget);

  spinBoxCallData->reason = arrowReason;
  spinBoxCallData->event  = arrowEvent;
  spinBoxCallData->widget =
	(spinWC->spinBox_class.get_callback_widget) ?
	(Widget) (*(spinWC->spinBox_class.get_callback_widget))((Widget)spinW) :
	(Widget) spinW->spinBox.textw;

  if (SB_ChildCount(spinW) && SB_WithChild(spinW))
    {
      XtArgVal position;
      spinC = SB_GetConstraintRec(spinW->spinBox.textw);

      spinBoxCallData->doit = True;
      position = spinC->position;
      GetPositionValue( (Widget) spinW->spinBox.textw,
			XtOffset(XmSpinBoxConstraint, position),
			&position);
      spinBoxCallData->position = position;
      if (spinC->sb_child_type == XmSTRING)
	{
	  if ((spinC->num_values > 0) && (spinC->position < spinC->num_values))
	    spinBoxCallData->value  = spinC->values[spinC->position];
          else
	    spinBoxCallData->value  = NULL;
	}
      else
        {
          char	*buffer = (char *) NULL;

          NumToString(&buffer,
		      spinC->minimum_value,
		      spinC->maximum_value,
		      spinC->decimal_points,
		      spinC->position );
          if (buffer)
            xmString = XmStringCreateLocalized(buffer);

	  spinBoxCallData->value = xmString;

          if (buffer)
	    XtFree(buffer);
        }

      if (arrowReason == XmCR_SPIN_NEXT
	  ||  arrowReason == XmCR_SPIN_PRIOR)
	spinBoxCallData->crossed_boundary = spinW->spinBox.boundary;
      else
	spinBoxCallData->crossed_boundary = False;
    }
  else
    {
      spinBoxCallData->doit = False;
      spinBoxCallData->position = 0;
      spinBoxCallData->value  = NULL;
      spinBoxCallData->crossed_boundary = False;
    }

  /* inform the application of the change */
  XtCallCallbackList((Widget) spinW, callbackList, (XtPointer) spinBoxCallData);

  /* Clean up the temporary XmString created to hold the XmNUMERIC value. */
  if (xmString != (XmString) NULL)
    XmStringFree(xmString);
}


/******************************************************************************
 * ArrowCallback()
 *	Setup and Call ValueChanged Callback(s) for SpinBox.
 *****************************************************************************/
static void
ArrowCallback(Widget arrowWidget, XEvent *arrowEvent, int arrowReason)
{
  XmSpinBoxWidget               spinW = (XmSpinBoxWidget) arrowWidget;
  XmSpinBoxCallbackStruct	spinBoxCallData;

  FireCallbacks(&spinBoxCallData,
	        spinW->spinBox.value_changed_cb,
	        arrowWidget,
	        arrowEvent,
	        arrowReason);
}


/******************************************************************************
 * ArrowVerify()
 *	Setup and Call ModifyVerify Callback(s) for SpinBox.
 *****************************************************************************/
static Boolean
ArrowVerify(Widget arrowWidget, XEvent *arrowEvent, int arrowReason)
{
  XmSpinBoxWidget		spinW = (XmSpinBoxWidget) arrowWidget;
  XmSpinBoxCallbackStruct	spinBoxCallData;

  FireCallbacks(&spinBoxCallData,
	        spinW->spinBox.modify_verify_cb,
	        arrowWidget,
	        arrowEvent,
	        arrowReason);

  if (SB_ChildCount(spinW) && SB_WithChild(spinW) && spinBoxCallData.doit)
  {
    char		*error = (char *) NULL;
    XtArgVal		position = spinBoxCallData.position;
    int			int_pos;
    XmSpinBoxConstraint	spinC = SB_GetConstraintRec(spinW->spinBox.textw);

    (void) SetPositionValue((Widget) spinW->spinBox.textw,
			    XtOffset(XmSpinBoxConstraint, position),
			    &position);
    int_pos = position;

    error = ValidatePositionValue(spinC, &int_pos);
    if (error)
      XmeWarning((Widget) spinW, error);

    spinC->position = int_pos;
  }

  return(spinBoxCallData.doit);
}

/******************************************************************************
 * ArrowSpinUp()
 *	Spin Increment Arrow.
 *****************************************************************************/
static void
ArrowSpinUp(Widget w, XEvent *callEvent)
{
  XmSpinBoxConstraint  spinC;
  XmSpinBoxWidget	 spinW;
  int              inPosition;

  spinW = (XmSpinBoxWidget)w;

  if (SB_ChildCount(spinW) && SB_WithChild(spinW))
    {
      spinC = SB_GetConstraintRec(spinW->spinBox.textw);

      inPosition = spinC->position;
      spinW->spinBox.boundary = False;
      spinC->position += (SB_ChildIsNumeric(spinC) ?
			  spinC->increment_value : 1);

      if (spinC->position > SB_ChildMaximumPositionValue(spinC))
	{
	  if (spinC->wrap)
            {
              spinW->spinBox.boundary = True;
              spinC->position = SB_ChildMinimumPositionValue(spinC);
            }
          else
	    {
	      spinC->position = inPosition;
	      XBell(XtDisplay(spinW), 0);
            }
        }


      /* Update the Text Widget */
      if (inPosition != spinC->position)
      {
	if (ArrowVerify((Widget)spinW, callEvent, XmCR_SPIN_NEXT))
	  {
	    UpdateChildText(spinW->spinBox.textw);
	    ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_NEXT);
	  }
	else
        {
	  spinC->position = inPosition;
        }
      }
    }
  else
    ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_NEXT);
}

/******************************************************************************
 * ArrowSpinDown
 *	Spin Decrement Arrow.
 *****************************************************************************/
static void
ArrowSpinDown(Widget w, XEvent *callEvent)
{
  XmSpinBoxConstraint  spinC;
  XmSpinBoxWidget	 spinW;
  int              inPosition;

  spinW = (XmSpinBoxWidget)w;

  if (SB_ChildCount(spinW) && SB_WithChild(spinW))
    {
      spinC = SB_GetConstraintRec(spinW->spinBox.textw);

      inPosition = spinC->position;
      spinW->spinBox.boundary = False;
      spinC->position -= (SB_ChildIsNumeric(spinC) ?
			  spinC->increment_value : 1);

      if (spinC->position < SB_ChildMinimumPositionValue(spinC))
	{
	  if (spinC->wrap)
            {
              spinW->spinBox.boundary = True;
              spinC->position = SB_ChildMaximumPositionValue(spinC);
            }
          else
	    {
	      spinC->position = inPosition;
	      XBell(XtDisplay(spinW), 0);
            }
        }

      /* Update the Text Widget */
      if (inPosition != spinC->position)
      {
	if (ArrowVerify((Widget)spinW, callEvent, XmCR_SPIN_PRIOR))
	  {
	    UpdateChildText(spinW->spinBox.textw);
	    ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_PRIOR);
	  }
	else
        {
	  spinC->position = inPosition;
        }
      }
    }
  else
    ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_PRIOR);
}

/*****************************************************************************
 * GetSpinSize
 *****************************************************************************/
static void
GetSpinSize(Widget w, Dimension *wide, Dimension *high)
{
  XmSpinBoxWidget spinW;
  Dimension	 childHeight;
  Dimension	 saveWide;
  Dimension	 saveHigh;
  Widget	 childW;
  int		 i;
  int            arrowSize;
  int            arrowsWide;
  int            arrowsHigh;
  int            spacing;

  spinW = (XmSpinBoxWidget)w;

  saveWide = XtWidth(spinW);
  saveHigh = XtHeight(spinW);

  XtWidth(spinW) = *wide;
  XtHeight(spinW) = *high;


  arrowSize = spinW->spinBox.arrow_size;
  arrowsWide = SB_NumArrowsWide(spinW);
  arrowsHigh = SB_NumArrowsHigh(spinW);
  spacing = spinW->spinBox.spacing;

  if (*wide == 0)
    {
      *wide = arrowsWide * arrowSize;
      *wide += (arrowsWide - 1) * spacing;
      *wide += 2 * spinW->spinBox.margin_width;
      *wide += 2 * SB_ShadowPixels(spinW);

      if (SB_WithChild(spinW))
	for (i = 0; i < SB_ChildCount(spinW); i++)
	  {
	    childW = spinW->composite.children[i];

	    if (XtIsManaged(childW))
	      *wide += XtWidth(childW) + spinW->spinBox.spacing;
	  }

      /* Remember our best width */
      spinW->spinBox.ideal_width = *wide;
    }

  if (!*high)
    {
      *high = arrowsHigh * arrowSize;
      *high += (arrowsHigh - 1) * spacing;
      *high += 2 * spinW->spinBox.margin_height;

      if (SB_WithChild(spinW))
	for (i = 0; i < SB_ChildCount(spinW); i++)
	  {
	    childW = spinW->composite.children[i];

	    if (XtIsManaged(childW))
	      {
		childHeight = XtHeight(childW);
		*high = MAX(*high, childHeight);
	      }
	  }

      /* Factor in the shadow thickness and remember our best size */
      *high += 2 * SB_ShadowPixels(spinW);
      spinW->spinBox.ideal_height = *high;
    }

  if (*wide == 0)
    *wide = 1;

  if (*high == 0)
    *high = 1;

  XtWidth(spinW) = saveWide;
  XtHeight(spinW) = saveHigh;
}

/******************************************************************************
 * SpinNChangeMoveCB
 *	Navigator Trait Change/Move Callback.
 *****************************************************************************/
static void
SpinNChangeMoveCB(Widget nav, XtCallbackProc moveCB,
		  XtPointer closure, Boolean setunset)
{
  if (setunset)
    XtAddCallback (nav, XmNvalueChangedCallback, moveCB, closure);
  else
    XtRemoveCallback (nav, XmNvalueChangedCallback, moveCB, closure);
}

/******************************************************************************
 * SpinNSetValue
 *	Navigator SetValue Function.
 *****************************************************************************/
static void
SpinNSetValue(Widget nav, XmNavigatorData nav_data, Boolean notify)
{
  XmSpinBoxConstraint spinC;
  XmSpinBoxWidget	spinW = (XmSpinBoxWidget)nav;
  Arg		arglist[6];
  Cardinal	argCount;
  int		lastValue;
  int		numericCount;
  int		i;
  int		minimum;
  int		increment;
  Mask mask ;

  if (nav_data->valueMask & NavDimMask)
      spinW->spinBox.dim_mask = nav_data->dimMask ;

  if (!(numericCount = NumericChildCount(spinW))) return;

  if (!(spinW->spinBox.dim_mask & nav_data->dimMask))
    return ;

  /* Spin box can be a 2d dimensional navigator at most.
     If there is only one dimension set, the following loop is only
     rnu once since the mask is update at the end of it */

  mask = spinW->spinBox.dim_mask ;

  for (numericCount = 0, i = 0;
       i < SB_ChildCount(spinW) && numericCount < 2 && mask;
       i++)
    {
      spinC = SB_GetConstraintRec(spinW->composite.children[i]);

      if (SB_ChildIsNumeric(spinC))
	{
	  argCount = 0;
	  numericCount++;

	  lastValue = spinC->position;
	  minimum=spinC->minimum_value;
	  increment=spinC->increment_value;

	  if ((nav_data->valueMask & NavMinimum)
	      && (spinC->minimum_value !=
		  ACCESS_DIM(mask, nav_data->minimum)))
	    {
	      XtSetArg (arglist[argCount], XmNminimumValue,
			ACCESS_DIM(mask, nav_data->minimum));
	      minimum = ACCESS_DIM(mask, nav_data->minimum);
	      argCount++;
	    }

	  if ((nav_data->valueMask & NavIncrement)
	      && (spinC->increment_value !=
		  ACCESS_DIM(mask, nav_data->increment)))
	    {
	      XtSetArg (arglist[argCount], XmNincrementValue,
			ACCESS_DIM(mask, nav_data->increment));
	      increment = ACCESS_DIM(mask, nav_data->increment);
	      argCount++;
	    }

	  /* Process value if different from current value or either
	     increment or minimumValue changed (which will change the
	     calculation */

	  if ((nav_data->valueMask & NavValue)
	      &&  ((argCount != 0) ||
		   (lastValue != ACCESS_DIM(mask, nav_data->value))))
	    {
	      XtArgVal position = ACCESS_DIM(mask, nav_data->value);

	      GetPositionValue(
			(Widget) spinW->composite.children[i],
			XtOffset(XmSpinBoxConstraint, position),
			&position);

	      XtSetArg (arglist[argCount], XmNposition, ((int)position));
	      argCount++;
	    }

	  if ((nav_data->valueMask & NavMaximum)
	      && (spinC->maximum_value !=
		  ACCESS_DIM(mask, nav_data->maximum)))
	    {
	      XtSetArg (arglist[argCount], XmNmaximumValue,
			ACCESS_DIM(mask, nav_data->maximum) - 1);
	      argCount++;
	    }

	  if (argCount)
	    XtSetValues (spinW->composite.children[i],
			 arglist, argCount);

	  if (notify &&
	      ACCESS_DIM(mask, nav_data->value) != lastValue)
	    ArrowCallback((Widget)spinW, NULL, XmCR_OK);


	  /* mark the dimMask as x goes. So that if there is a second
	     numeric child, it only gets y setting. If there was no
	     X in the current mask, just stop here */
	  if (mask & NavigDimensionX) mask &= ~NavigDimensionX ;
	  else mask = 0 ;
	}
    }
}

/******************************************************************************
 * SpinNGetValue
 *	Navigator GetValue Function.
 *****************************************************************************/
static void
SpinNGetValue(Widget nav, XmNavigatorData nav_data)
{
  XmSpinBoxConstraint   spinC;
  XmSpinBoxWidget	spinW;
  int		i;
  int		numericCount;
  Mask mask ;

  spinW = (XmSpinBoxWidget) nav;

  if (!(numericCount = NumericChildCount(spinW))) return;

  mask = nav_data->dimMask =  spinW->spinBox.dim_mask;

  if (nav_data->valueMask & (NavValue|NavMinimum|NavMaximum|NavIncrement)) {
      /* get the value out of the numeric children, in order  */
      for (numericCount = 0, i = 0;
	   i < SB_ChildCount(spinW) && numericCount < 2 && mask;
	   i++)
	{
	  spinC = SB_GetConstraintRec(spinW->composite.children[i]);
	  if (SB_ChildIsNumeric(spinC)) {
	      numericCount++;

	      ASSIGN_DIM(mask, nav_data->value, spinC->position);
	      ASSIGN_DIM(mask, nav_data->minimum, spinC->minimum_value);
	      ASSIGN_DIM(mask, nav_data->maximum, spinC->maximum_value + 1);
	      ASSIGN_DIM(mask, nav_data->increment, spinC->increment_value);

	      mask &= ~NavigDimensionX ;
	    }
	}

    }
}


/******************************************************************************
 * GetPositionValue
 *	XmExportProc conversion routine for converting between internal
 *	and external representation for XmNposition resource.  Internal
 *	representation is always POSITION_VALUE.  External representation
 *	can be either POSITION_VALUE or POSITION_INDEX as determined
 *	by XmNpositionType.
 *****************************************************************************/
static void
GetPositionValue(Widget w, int offset, XtArgVal *value)
{
  XmSpinBoxConstraint	wc = SB_GetConstraintRec(w);

  if (SB_ChildIsNumeric(wc) && (!SB_ChildPositionTypeIsValue(wc)))
      *value = (*value - wc->minimum_value) / wc->increment_value;
}

/******************************************************************************
 * SetPositionValue
 *	XmImportProc conversion routine for converting between external
 *	and internal representation for XmNposition resource.  Internal
 *	representation is always POSITION_VALUE.  External representation
 *	can be either POSITION_VALUE or POSITION_INDEX as determined
 *	by XmNpositionType.
 *****************************************************************************/
static XmImportOperator
SetPositionValue(Widget w, int offset, XtArgVal *value)
{
  XmSpinBoxConstraint	wc = SB_GetConstraintRec(w);

  if (SB_ChildIsNumeric(wc) && (!SB_ChildPositionTypeIsValue(wc)))
      *value = wc->minimum_value + (*value * wc->increment_value);

  return(XmSYNTHETIC_LOAD);
}

/******************************************************************************
 * GetMaximumPositionValue
 *	Returns the maximum allowable position for this widget.
 *****************************************************************************/
static int
GetMaximumPositionValue(XmSpinBoxConstraint sc)
{
  int	max;

  if (sc == (XmSpinBoxConstraint) NULL)
    max = 0;
  else if (SB_ChildIsNumeric(sc))
    max = sc->maximum_value;
  else
    max = (sc->num_values  > 0) ? (sc->num_values - 1) : 0;

  return(max);
}

/******************************************************************************
 * GetMinimumPositionValue
 *	Returns the minimum allowable position for this widget.
 *****************************************************************************/
static int
GetMinimumPositionValue(XmSpinBoxConstraint sc)
{
  int	min;

  if (sc == (XmSpinBoxConstraint) NULL)
    min = 0;
  else if (SB_ChildIsNumeric(sc))
    min = sc->minimum_value;
  else
    min = 0;

  return(min);
}

/******************************************************************************
 * ValidatePositionValue
 *	Returns the minimum allowable position for this widget.
 *****************************************************************************/
static char *
ValidatePositionValue(XmSpinBoxConstraint sc, int *position)
{
  int	val;
  char	*err = (char *) NULL;

  val = SB_ChildMaximumPositionValue(sc);
  if (*position > val) {
    *position = val;
    err = BAD_SPIN_POSITION_MAX;
  }

  val = SB_ChildMinimumPositionValue(sc);
  if (*position < val) {
    *position = val;
    err = BAD_SPIN_POSITION_MIN;
  }

  return(err);
}

static Boolean
CvtStringToPositionValue(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  XtArgVal		value;
  int			offset = XtOffset(XmSpinBoxConstraint, position);
  Widget		w = *((Widget *) args[0].addr);

  if (sscanf(from->addr, "%ld", (long*)&value) == 0)
    {
      XtDisplayStringConversionWarning(display,
				       (char *)from->addr,
				       XmRPositionValue);
      return False;
    }

  (void) SetPositionValue(w, offset, &value);
  _XM_CONVERTER_DONE( to, int, (int)value, ; )
}


/*
 *
 * Public API
 *
 */

/************************************************************************
 *  XmCreateSpinBox()
 *  XmVaCreateSpinBox()
 *  XmVaCreateManagedSpinBox()
 *	Create an instance of a Spin widget and return the widget id.
 ************************************************************************/
Widget
XmCreateSpinBox(Widget parent, String name,
		ArgList arglist, Cardinal argcount)
{
  return(XtCreateWidget(name, xmSpinBoxWidgetClass, parent,
			arglist, argcount));
}

Widget XmVaCreateSpinBox(Widget parent, char *name, ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmSpinBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}

Widget XmVaCreateManagedSpinBox(Widget parent, char *name, ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmSpinBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/************************************************************************
 *  XmSpinBoxValidatePosition
 *	Validate the position value specified in string.
 ************************************************************************/
int
XmSpinBoxValidatePosition(Widget text_field, int *position)
{
  int			i;
  float			fPosition;
  int			iPosition;
  int			positionOffset =
				XtOffset(XmSpinBoxConstraint, position);
  int           iOffset;
  String		string;
  XmAccessTextualTrait  textT;
  XmSpinBoxConstraint	wc;
  XtAppContext 		app;

  if (text_field == (Widget) NULL)
    return(XmCURRENT_VALUE);

  app = XtWidgetToApplicationContext(text_field);
  _XmAppLock(app);

  textT = (XmAccessTextualTrait)
    XmeTraitGet((XtPointer) XtClass(text_field), XmQTaccessTextual);
  if (textT == NULL) {
    _XmAppUnlock(app);
    return(XmCURRENT_VALUE);
  }

  wc = SB_GetConstraintRec(text_field);
  if ((wc == (XmSpinBoxConstraint) NULL) || (! SB_ChildIsNumeric(wc)))
    {
      if ((wc) && (position))
	*position = wc->position;
      _XmAppUnlock(app);
      return(XmCURRENT_VALUE);
    }

  string = textT->getValue(text_field, XmFORMAT_MBYTE);
  if (sscanf(string, "%f", &fPosition) == 0)
    {
      if (position)
	{
	  XtArgVal external_position = wc->position;
          GetPositionValue(text_field, positionOffset, &external_position);
	  *position = (int)external_position;
	}

      _XmAppUnlock(app);
      return(XmCURRENT_VALUE);
    }
  XtFree(string);

  for (i=0; i<wc->decimal_points; i++)
    fPosition *= 10.0;

  iPosition = (int) fPosition;

  if (iPosition < SB_ChildMinimumPositionValue(wc))
    {
      if (position)
	{
	  XtArgVal external_position = SB_ChildMinimumPositionValue(wc);
          GetPositionValue(text_field, positionOffset, &external_position);
	  *position = (int)external_position;
	}

      _XmAppUnlock(app);
      return(XmMINIMUM_VALUE);
    }

  if (iPosition > SB_ChildMaximumPositionValue(wc))
    {
      if (position)
	{
	  XtArgVal external_position = SB_ChildMaximumPositionValue(wc);
          GetPositionValue(text_field, positionOffset, &external_position);
	  *position = (int)external_position;
	}

      _XmAppUnlock(app);
      return(XmMAXIMUM_VALUE);
    }

  iOffset = iPosition - SB_ChildMinimumPositionValue(wc);
  if ((iOffset % wc->increment_value) != 0)
    {
      int	iValue = wc->increment_value;

      if (position)
	{
	  XtArgVal external_position = SB_ChildMinimumPositionValue(wc) + (iOffset / iValue) * iValue;
          GetPositionValue(text_field, positionOffset, &external_position);
          *position = (int)external_position;
	}

      _XmAppUnlock(app);
      return(XmINCREMENT_VALUE);
    }

  if (position)
    {
      XtArgVal external_position = iPosition;
      GetPositionValue(text_field, positionOffset, &external_position);
      *position = (int)external_position;
    }

  _XmAppUnlock(app);
  return(XmVALID_VALUE);
}
