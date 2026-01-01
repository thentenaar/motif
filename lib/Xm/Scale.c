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
static char rcsid[] = "$TOG: Scale.c /main/31 1999/10/13 16:18:07 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <X11/Xos.h>

#ifndef CSRG_BASED
/*
 * Modification by Integrated Computer Solutions, Inc.  May 2000
 *
 * Original:
 *
 * # ifdef linux
 * #  define RADIXCHAR MON_DECIMAL_POINT
 * # endif
 * # include <langinfo.h>
 *
 * glibc >= 2.0 defines RADIXCHAR in langinfo.h, simply make sure
 * that it is not redefined here.
 */
# include <langinfo.h>
# if defined(linux) && !defined(RADIXCHAR)
#  define RADIXCHAR DECIMAL_POINT
# endif
# ifdef X_LOCALE
#  ifdef linux
   /* avoid conflicting with <X11/Xlocale.h> defines */
#   undef LC_ALL
#   undef LC_COLLATE
#   undef LC_CTYPE
#   undef LC_MONETARY
#   undef LC_NUMERIC
#   undef LC_TIME
#  endif
# endif
#else
# define nl_langinfo(radixchar)	"."
#endif /* !CSRG_BASED */

#ifdef __cplusplus
extern "C" { /* some 'locale.h' do not have prototypes (sun) */
#endif
#include <X11/Xlocale.h>
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif /* __cplusplus */

#include <Xm/DisplayP.h>
#include <Xm/DragC.h>
#include <Xm/DragIconP.h>
#include <Xm/LabelG.h>
#include <Xm/ScaleP.h>
#include <Xm/ScrollBarP.h>	/* for slider geometrical data */
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/XmosP.h>
#include <Xm/VaSimpleP.h>
#include "GeoUtilsI.h"
#include "GMUtilsI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "TransferI.h"
#include "TraversalI.h"
#include "XmI.h"

#define state_flags last_value

#define MESSAGE1	_XmMMsgScale_0000
#define MESSAGE2	_XmMMsgScale_0001
#define MESSAGE3	_XmMMsgScale_0002
#define MESSAGE5	_XmMMsgScaleScrBar_0004
#define MESSAGE7	_XmMMsgScale_0006
#define MESSAGE8	_XmMMsgScale_0007
#define MESSAGE9	_XmMMsgScale_0008


static Region null_region = NULL;


/* Convenience macros and definitions */

#define TotalWidth(w)   (w->core.width + (w->core.border_width * 2))
#define TotalHeight(w)  (w->core.height + (w->core.border_width * 2))

#define SCROLLBAR_MAX	1000000000
#define SCALE_VALUE_MARGIN 3
#define SCALE_DEFAULT_MAJOR_SIZE \
	(100 + (2 * sw->scale.highlight_thickness))
#define SCALE_DEFAULT_MINOR_SIZE \
	(15 + (2 * sw->scale.highlight_thickness))

#define SLIDER_SIZE(sca)	((sca->scale.sliding_mode == XmTHERMOMETER)?\
				 0:sca->scale.slider_size)


/* this one is context dependent, args and n are used */
#define SET(name, val) {XtSetArg (args[n], (name), (val)); n++;}


#define LeadXTic(sb, sca) (sb->scrollBar.slider_area_x \
		     + (Dimension) (((float) SLIDER_SIZE(sca) / 2.0) + 0.5))
#define LeadYTic(sb, sca) (sb->scrollBar.slider_area_y \
		     + (Dimension) (((float) SLIDER_SIZE(sca) / 2.0) + 0.5))
#define TrailXTic(sb, sca) (sb->core.width - (sb->scrollBar.slider_area_x \
		     + sb->scrollBar.slider_area_width\
		      - (Dimension) (((float) SLIDER_SIZE(sca) / 2.0) + 0.5)))
#define TrailYTic(sb, sca) (sb->core.height - (sb->scrollBar.slider_area_y \
		     + sb->scrollBar.slider_area_height\
		      - (Dimension) (((float) SLIDER_SIZE(sca) / 2.0) + 0.5)))


/********    Static Function Declarations    ********/

static void ScaleGetTitleString(
                        Widget wid,
                        int resource,
                        XtArgVal *value) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void ProcessingDirectionDefault(
                        XmScaleWidget widget,
                        int offset,
                        XrmValue *value) ;
static void SliderVisualDefault(
                        XmScaleWidget widget,
                        int offset,
                        XrmValue *value) ;
static void SliderMarkDefault(
                        XmScaleWidget widget,
                        int offset,
                        XrmValue *value) ;
static void EditableDefault(
                        XmScaleWidget widget,
                        int offset,
                        XrmValue *value) ;
static void ValidateInitialState(
                        XmScaleWidget req,
                        XmScaleWidget new_w) ;
static Widget CreateScaleTitle(
                        XmScaleWidget new_w) ;
static Widget CreateScaleScrollBar(
                        XmScaleWidget new_w) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetForegroundGC(
                        XmScaleWidget sw) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void CalcScrollBarData(
                        XmScaleWidget sw,
                        int *value,
                        int *slider_size,
                        int *increment,
                        int *page) ;
static void Resize(
                        Widget wid) ;
static void ValidateInputs(
                        XmScaleWidget cur,
                        XmScaleWidget new_w) ;
static void HandleTitle(
                        XmScaleWidget cur,
                        XmScaleWidget req,
                        XmScaleWidget new_w) ;
static void HandleScrollBar(
                        XmScaleWidget cur,
                        XmScaleWidget req,
                        XmScaleWidget new_w) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args_in,
                        Cardinal *num_args_in) ;
static void Realize(
                        Widget w,
                        XtValueMask *p_valueMask,
                        XSetWindowAttributes *attributes) ;
static void Destroy(
                        Widget wid) ;
static XtGeometryResult GeometryManager(
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static Dimension MaxLabelWidth(
                        XmScaleWidget sw) ;
static Dimension MaxLabelHeight(
                        XmScaleWidget sw) ;
static Dimension ValueTroughWidth(
                        XmScaleWidget sw) ;
static Dimension ValueTroughHeight(
                        XmScaleWidget sw) ;
static Dimension ValueTroughAscent(
                        XmScaleWidget sw) ;
static Dimension ValueTroughDescent(
                        XmScaleWidget sw) ;
static Dimension TitleWidth(
                        XmScaleWidget sw) ;
static Dimension TitleHeight(
                        XmScaleWidget sw) ;
static Cardinal NumManaged(
			   XmScaleWidget sw,
			   Widget * first_man,
			   Widget * last_man);
static Dimension MajorLeadPad(
                        XmScaleWidget sw) ;
static Dimension MajorTrailPad(
                        XmScaleWidget sw) ;
static Dimension ScrollWidth(
                        XmScaleWidget sw) ;
static Dimension ScrollHeight(
                        XmScaleWidget sw) ;
static void GetScaleSize(
                        XmScaleWidget sw,
                        Dimension *w,
                        Dimension *h) ;
static void LayoutHorizontalLabels(
                        XmScaleWidget sw,
                        XRectangle *scrollBox,
                        XRectangle *labelBox,
                        Widget instigator) ;
static void LayoutHorizontalScale(
                        XmScaleWidget sw,
                        XtWidgetGeometry * desired,
			Widget instigator) ;
static void LayoutVerticalLabels(
                        XmScaleWidget sw,
                        XRectangle *scrollBox,
                        XRectangle *labelBox,
                        Widget instigator) ;
static void LayoutVerticalScale(
                        XmScaleWidget sw,
                        XtWidgetGeometry * desired,
			Widget instigator) ;
static void ChangeManaged(
                        Widget wid) ;
static void GetValueString(
                        XmScaleWidget sw,
                        int value,
                        String buffer);
static void ShowValue(
		        XmScaleWidget sw) ;
static void SetScrollBarData(
                        XmScaleWidget sw) ;
static void ValueChanged(
                        Widget wid,
                        XtPointer closure,
                        XtPointer call_data) ;
static XtGeometryResult QueryGeometry(
                        Widget wid,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *desired) ;
static XmNavigability WidgetNavigable(
                        Widget wid) ;
static void StartDrag (Widget  w,
                        XtPointer data,
		        XEvent  *event,
		        Boolean *cont) ;
static void DragConvertCallback (Widget w,
				 XtPointer client_data,
				 XmConvertCallbackStruct *cs);
static void CheckSetRenderTable(Widget wid,
				int offset,
				XrmValue *value);

/********    End Static Function Declarations    ********/



/*  Resource definitions for Scale class */

static XtResource resources[] =
{
   {
       XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
       sizeof (Dimension), XtOffsetOf(XmManagerRec, manager.shadow_thickness),
       XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
       XmNvalue, XmCValue, XmRInt,
       sizeof(int), XtOffsetOf(XmScaleRec,scale.value),
       XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },

   {
       XmNmaximum, XmCMaximum, XmRInt,
       sizeof(int), XtOffsetOf(XmScaleRec,scale.maximum),
       XmRImmediate, (XtPointer)100
   },

   {
       XmNminimum, XmCMinimum, XmRInt,
       sizeof(int), XtOffsetOf(XmScaleRec,scale.minimum),
       XmRImmediate, (XtPointer)0
   },

   {
       XmNorientation, XmCOrientation, XmROrientation,
       sizeof(unsigned char), XtOffsetOf(XmScaleRec,scale.orientation),
       XmRImmediate, (XtPointer) XmVERTICAL
   },

   {
       XmNprocessingDirection, XmCProcessingDirection, XmRProcessingDirection,
       sizeof(unsigned char),
       XtOffsetOf(XmScaleRec,scale.processing_direction),
       XmRCallProc, (XtPointer) ProcessingDirectionDefault
   },

   {
       XmNtitleString, XmCTitleString, XmRXmString,
       sizeof(XmString), XtOffsetOf(XmScaleRec,scale.title),
       XmRImmediate, (XtPointer) NULL
   },

   {
	"pri.vate","Pri.vate",XmRInt,
	sizeof(int), XtOffsetOf(XmScaleRec,scale.last_value),
	XmRImmediate, (XtPointer) False
   },

   {
       XmNfontList, XmCFontList, XmRFontList,
       sizeof(XmFontList), XtOffsetOf(XmScaleRec, scale.font_list),
       XmRCallProc, (XtPointer)CheckSetRenderTable
   },

   {
       XmNrenderTable, XmCRenderTable, XmRRenderTable,
       sizeof(XmRenderTable), XtOffsetOf(XmScaleRec, scale.font_list),
       XmRCallProc, (XtPointer)CheckSetRenderTable
   },

   {
       XmNshowValue, XmCShowValue, XmRShowValue,
       sizeof(XtEnum), XtOffsetOf(XmScaleRec,scale.show_value),
       XmRImmediate, (XtPointer) XmNONE
   },

   {
       XmNdecimalPoints, XmCDecimalPoints, XmRShort,
       sizeof(short), XtOffsetOf(XmScaleRec,scale.decimal_points),
       XmRImmediate, (XtPointer) 0
   },

   {
       XmNscaleWidth, XmCScaleWidth, XmRHorizontalDimension,
       sizeof (Dimension), XtOffsetOf(XmScaleRec, scale.scale_width),
       XmRImmediate, (XtPointer) 0
   },

   {
       XmNscaleHeight, XmCScaleHeight, XmRVerticalDimension,
       sizeof (Dimension), XtOffsetOf(XmScaleRec, scale.scale_height),
       XmRImmediate, (XtPointer) 0
   },

   {
       XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
       sizeof (Dimension), XtOffsetOf(XmScaleRec, scale.highlight_thickness),
       XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
       XmNhighlightOnEnter, XmCHighlightOnEnter, XmRBoolean,
       sizeof (Boolean), XtOffsetOf(XmScaleRec, scale.highlight_on_enter),
       XmRImmediate, (XtPointer) False
   },
   {
       XmNvalueChangedCallback, XmCCallback, XmRCallback,
       sizeof(XtCallbackList),
       XtOffsetOf(XmScaleRec,scale.value_changed_callback),
       XmRCallback, (XtPointer) NULL
   },

   {
      XmNconvertCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
      XtOffsetOf(XmScaleRec, scale.convert_callback),
      XmRCallback, (XtPointer) NULL
   },

   {
       XmNdragCallback, XmCCallback, XmRCallback,
       sizeof(XtCallbackList),
       XtOffsetOf(XmScaleRec,scale.drag_callback),
       XmRCallback, (XtPointer) NULL
   },

   {
       XmNscaleMultiple, XmCScaleMultiple, XmRInt,
       sizeof(int), XtOffsetOf(XmScaleRec,scale.scale_multiple),
       XmRImmediate, (XtPointer) 0
   },
   {
       XmNslidingMode, XmCSlidingMode, XmRSlidingMode,
       sizeof(XtEnum), XtOffsetOf(XmScaleRec,scale.sliding_mode),
       XmRImmediate, (XtPointer) XmSLIDER
   },
   {
       XmNeditable, XmCEditable, XmRBoolean,
       sizeof(XtEnum), XtOffsetOf(XmScaleRec,scale.editable),
       XmRCallProc, (XtPointer) EditableDefault
   },
   {
       XmNsliderVisual, XmCSliderVisual, XmRSliderVisual,
       sizeof (XtEnum),
       XtOffsetOf(XmScaleRec, scale.slider_visual),
       XmRCallProc, (XtPointer) SliderVisualDefault
   },
   {
       XmNsliderMark, XmCSliderMark, XmRSliderMark,
       sizeof (XtEnum),
       XtOffsetOf(XmScaleRec, scale.slider_mark),
       XmRCallProc, (XtPointer) SliderMarkDefault
   },
   { /* undocumented - need synthetic hook to be complete */
       XmNsliderSize, XmCSliderSize, XmRHorizontalInt, sizeof (int),
       XtOffsetOf(XmScaleRec, scale.slider_size),
       XmRImmediate, (XtPointer) 30
   },
   {
       XmNshowArrows, XmCShowArrows, XmRShowArrows, sizeof (XtEnum),
       XtOffsetOf(XmScaleRec, scale.show_arrows),
       XmRImmediate, (XtPointer) XmNONE
   },
};


/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
    {
	XmNtitleString,
	sizeof (XmString), XtOffsetOf(XmScaleRec, scale.title),
	ScaleGetTitleString, (XmImportProc)NULL
    },
    {
	XmNscaleWidth,
	sizeof (Dimension), XtOffsetOf(XmScaleRec, scale.scale_width),
	XmeFromHorizontalPixels, XmeToHorizontalPixels
    },
    {
	XmNscaleHeight,
	sizeof (Dimension), XtOffsetOf(XmScaleRec, scale.scale_height),
	XmeFromVerticalPixels, XmeToVerticalPixels
    }
};


/*  Scale class record definition  */

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    (XtInitProc)NULL,			/* InitializePrehook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPrehook	*/
    (XtInitProc)NULL,			/* InitializePosthook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    (XtInitProc)NULL,			/* secondaryCreate	*/
    (XmGetSecResDataFunc)NULL, 		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    (XtArgsProc)NULL,			/* getValuesPrehook	*/
    (XtArgsProc)NULL,			/* getValuesPosthook	*/
    (XtWidgetClassProc)NULL,            /* classPartInitPrehook */
    (XtWidgetClassProc)NULL,            /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    (XmFocusChangeProc)NULL,            /* focusChange          */
    (XmWrapperData)NULL			/* wrapperData 		*/
};

externaldef(xmscaleclassrec) XmScaleClassRec xmScaleClassRec =
{
   {                                            /* core_class fields    */
      (WidgetClass) &xmManagerClassRec,         /* superclass         */
      "XmScale",                                /* class_name         */
      sizeof(XmScaleRec),                       /* widget_size        */
      ClassInitialize,                          /* class_initialize   */
      ClassPartInitialize,                      /* class_part_init    */
      FALSE,                                    /* class_inited       */
      Initialize,                               /* initialize         */
      (XtArgsProc)NULL,                         /* initialize_hook    */
      Realize,                                  /* realize            */
      NULL,                                     /* actions            */
      0,                                        /* num_actions        */
      resources,                                /* resources          */
      XtNumber(resources),                      /* num_resources      */
      NULLQUARK,                                /* xrm_class          */
      TRUE,                                     /* compress_motion    */
      XtExposeCompressMaximal,                  /* compress_exposure  */
      TRUE,                                     /* compress_enterlv   */
      FALSE,                                    /* visible_interest   */
      Destroy,                                  /* destroy            */
      Resize,                                   /* resize             */
      Redisplay,                                /* expose             */
      SetValues,                                /* set_values         */
      (XtArgsFunc)NULL,                         /* set_values_hook    */
      XtInheritSetValuesAlmost,                 /* set_values_almost  */
      (XtArgsProc)NULL,                         /* get_values_hook    */
      (XtAcceptFocusProc)NULL,                  /* accept_focus       */
      XtVersion,                                /* version            */
      NULL,                                     /* callback_private   */
      XtInheritTranslations,                    /* tm_table           */
      (XtGeometryHandler) QueryGeometry,        /* query_geometry     */
      (XtStringProc)NULL,                       /* display_accelerator*/
      (XtPointer)&baseClassExtRec,              /* extension          */
   },

   {                                            /* composite_class fields */
      GeometryManager,                          /* geometry_manager   */
      ChangeManaged,                            /* change_managed     */
      XtInheritInsertChild,                     /* insert_child       */
      XtInheritDeleteChild,                     /* delete_child       */
      NULL,                                     /* extension          */
   },

   {                                            /* constraint_class fields */
      NULL,                                     /* resource list        */
      0,                                        /* num resources        */
      sizeof(XmManagerConstraintRec),           /* constraint size      */
      (XtInitProc)NULL,                         /* init proc            */
      (XtWidgetProc)NULL,                       /* destroy proc         */
      (XtSetValuesFunc)NULL,                    /* set values proc      */
      NULL,                                     /* extension            */
   },


   {		/* manager_class fields */
      XtInheritTranslations,			/* translations           */
      syn_resources,				/* syn_resources      	  */
      XtNumber(syn_resources),			/* num_syn_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_syn_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           	  */
   },

   {                                            /* scale class - none */
      (XtPointer) NULL                          /* extension */
   }
};

externaldef(xmscalewidgetclass) WidgetClass
	xmScaleWidgetClass = (WidgetClass)&xmScaleClassRec;


/* Transfer trait record */

static const XmTransferTraitRec ScaleTransfer = {
  0, 						/* version */
  (XmConvertCallbackProc) DragConvertCallback,	/* convertProc */
  NULL,						/* destinationProc */
  NULL,						/* destinationPreHookProc */
};

/****************************************************************/
/************** Synthetic hook & default routines ***************/
/****************************************************************/

static void
ScaleGetTitleString(
        Widget wid,
        int resource,		/* unused */
        XtArgVal *value)
{
	XmScaleWidget scale = (XmScaleWidget) wid ;
	Arg           al[1] ;

	if (scale->scale.title == NULL) {
	    /* mean that the title has never been set, so
	       we should return NULL, not the label value which
	       is the label name, not NULL,  in this case */
	    *value = (XtArgVal) NULL ;
	} else {
	    /* title = -1, our magic value used to tell: look in
	       the label child. */
	    XtSetArg (al[0], XmNlabelString, value);	/* make a copy */
	    XtGetValues (scale->composite.children[0], al, 1);
	}
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
        XmScaleWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
	static unsigned char direction;

	value->addr = (XPointer) &direction;

	if (widget->scale.orientation == XmHORIZONTAL)
        {
           if (LayoutIsRtoLM(widget))
                direction = XmMAX_ON_LEFT;
           else
		direction = XmMAX_ON_RIGHT;
        }
	else /* XmVERTICAL  -- range checking done during widget
		                   initialization */
		direction = XmMAX_ON_TOP;
}



/*********************************************************************
 *
 * SliderVisualDefault
 *
 *
 *********************************************************************/
static void
SliderVisualDefault(
        XmScaleWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum slider_visual ;

      value->addr = (XPointer) &slider_visual;

      if (widget->scale.sliding_mode == XmTHERMOMETER) {
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
        XmScaleWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum slider_mark ;

      value->addr = (XPointer) &slider_mark;

      if (!widget->scale.editable) slider_mark = XmNONE ;
      else {
	  if (widget->scale.sliding_mode == XmTHERMOMETER)
	      slider_mark = XmROUND_MARK ;
	  else
	      slider_mark = XmETCHED_LINE ;
      }
}

/*********************************************************************
 *
 * EditableDefault
 *
 *
 *********************************************************************/
static void
EditableDefault(
        XmScaleWidget widget,
        int offset,		/* unused */
        XrmValue *value )
{
      static XtEnum editable ;

      value->addr = (XPointer) &editable;

      if (widget->scale.sliding_mode == XmTHERMOMETER) {
          editable = False ;
      } else {
	  editable = True ;
      }

}


/*
 * XmRCallProc routine for checking list.font before setting it to NULL
 * if no value is specified for both XmNrenderTable and XmNfontList.
 * If "last_value" is True, then function has been called twice on same
 * widget, thus resource needs to be set NULL, otherwise leave it alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value )
{
  XmScaleWidget sw = (XmScaleWidget)wid;

  /* Check if been here before */
  if (sw->scale.last_value)
      value->addr = NULL;
  else {
      sw->scale.last_value = True;
      value->addr = (char*)&(sw->scale.font_list);
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
  baseClassExtRec.record_type = XmQmotif ;

  if (null_region == NULL)
    null_region = XCreateRegion();
}



/************************************************************************
 *
 *  ClassPartInitialize
 *     Initialize the fast subclassing.
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
   _XmFastSubclassInit (wc, XmSCALE_BIT);

    /* Install transfer trait */
    XmeTraitSet((XtPointer)wc, XmQTtransfer, (XtPointer) &ScaleTransfer);
}



/*********************************************************************
 *  Initialize
 *      Validate all of the argument data for the widget, create the
 *	title label and scrollbar.
 *
 *********************************************************************/
static void
ValidateInitialState(
        XmScaleWidget req,	/* unused */
        XmScaleWidget new_w )
{
	Boolean default_value = FALSE;
        float value_range;


	if (new_w->scale.minimum >= new_w->scale.maximum)
	{
		new_w->scale.minimum = 0;
		new_w->scale.maximum = 100;
		XmeWarning( (Widget) new_w, MESSAGE1);
	}

        value_range = (float)((float)new_w->scale.maximum -
			      (float)new_w->scale.minimum);

        if (value_range > (float)((float)INT_MAX / (float) 2.0))
        {
             new_w->scale.minimum = 0;
	     if (new_w->scale.maximum > (INT_MAX / 2))
	         new_w->scale.maximum = INT_MAX / 2;
            XmeWarning( (Widget) new_w, MESSAGE9);
        }

	if (new_w->scale.value == XmINVALID_DIMENSION)
	{
		new_w->scale.value = 0;
		default_value = True;
	}

	if (new_w->scale.value < new_w->scale.minimum)
	{
		new_w->scale.value = new_w->scale.minimum;
		if (!default_value) XmeWarning( (Widget) new_w, MESSAGE2);
	}

	if (new_w->scale.value > new_w->scale.maximum)
	{
		new_w->scale.value = new_w->scale.minimum;
		if (!default_value) XmeWarning( (Widget) new_w, MESSAGE3);
	}

	if(!XmRepTypeValidValue( XmRID_ORIENTATION,
				new_w->scale.orientation, (Widget) new_w) )
	{
		new_w->scale.orientation = XmVERTICAL;
	}

	if (!XmRepTypeValidValue( XmRID_SHOW_VALUE,
				new_w->scale.show_value, (Widget) new_w) )
	    {
		new_w->scale.show_value = XmNONE;
	    }

	if (new_w->scale.orientation == XmHORIZONTAL)
	{
		if ((new_w->scale.processing_direction != XmMAX_ON_RIGHT) &&
			(new_w->scale.processing_direction != XmMAX_ON_LEFT))

		{
			new_w->scale.processing_direction = XmMAX_ON_RIGHT;
			XmeWarning( (Widget) new_w, MESSAGE5);
		}
	}
	else
	{
		if ((new_w->scale.processing_direction != XmMAX_ON_TOP) &&
			(new_w->scale.processing_direction != XmMAX_ON_BOTTOM))
		{
			new_w->scale.processing_direction = XmMAX_ON_TOP;
			XmeWarning( (Widget) new_w, MESSAGE5);
		}
	}

	if (new_w->scale.scale_multiple > (new_w->scale.maximum
		- new_w->scale.minimum))
	{
		XmeWarning( (Widget) new_w, MESSAGE7);
		new_w->scale.scale_multiple = (new_w->scale.maximum
			- new_w->scale.minimum) / 10;
	}
	else if (new_w->scale.scale_multiple < 0)
	{
		XmeWarning( (Widget) new_w, MESSAGE8);
		new_w->scale.scale_multiple = (new_w->scale.maximum
			- new_w->scale.minimum) / 10;
	}
	else if (new_w->scale.scale_multiple == 0)
		new_w->scale.scale_multiple = (new_w->scale.maximum
			- new_w->scale.minimum) / 10;
	/* Assure a minimum value of 1 */
        if (new_w->scale.scale_multiple < 1)
                new_w->scale.scale_multiple = 1;
}

static Widget
CreateScaleTitle(
        XmScaleWidget new_w )
{
	XmLabelGadget title;
	Arg args[5];
	int n;

	/*  Create the title label gadget  */

	/* title can be NULL or a valid XmString, if null,
	   the label will use its own name as XmString */
	n = 0;
	XtSetArg (args[n], XmNlabelString, new_w->scale.title);	n++;
	XtSetArg (args[n], XmNfontList, new_w->scale.font_list);	n++;

	title = (XmLabelGadget) XmCreateLabelGadget( (Widget) new_w,
						    "Title",
		args, n);

	if (new_w->scale.title) {
	    XtManageChild ((Widget) title);
	    new_w->scale.title = (XmString) -1 ;
	} /* scale.title need to be set to some special not NULL value
	     in order to see any change at SetValues time and also to
	     return NULL at Getvalue time in the hook. This is pirs 3197:
	     when you setvalues a new xmstring as title, the value of the
	     title field, a pointer, might be the same. */

	return((Widget) title);
}

static Widget
CreateScaleScrollBar(
        XmScaleWidget new_w )
{
    Widget scrollbar;
    Arg args[25];
    int n = 0;

    /*  Build up an arg list for and create the scrollbar  */

    n = 0;
    SET(XmNmaximum, SCROLLBAR_MAX);
    SET(XmNminimum, 0);
    SET(XmNshowArrows, new_w->scale.show_arrows);
    SET(XmNunitType, XmPIXELS);
    SET(XmNorientation, new_w->scale.orientation);
    SET(XmNprocessingDirection, new_w->scale.processing_direction);
    SET(XmNslidingMode, new_w->scale.sliding_mode);
    SET(XmNsliderVisual, new_w->scale.slider_visual);
    SET(XmNsliderMark, new_w->scale.slider_mark);
    SET(XmNeditable, new_w->scale.editable);
    if (new_w->scale.scale_width != 0)
	SET(XmNwidth, new_w->scale.scale_width);
    if (new_w->scale.scale_height != 0)
	SET(XmNheight, new_w->scale.scale_height);


    /* then get everything else from the scale parent */
    /* another more incestuous but also more powerful - because it
       allows customization - way of doing that would be to provide
       resource default proc in ScrollBar that look for a Scale parent
       and inherit those visual resources */
    SET(XmNhighlightColor, new_w->manager.highlight_color);
    SET(XmNhighlightPixmap, new_w->manager.highlight_pixmap);
    SET(XmNhighlightThickness, new_w->scale.highlight_thickness);
    SET(XmNhighlightOnEnter, new_w->scale.highlight_on_enter);
    SET(XmNtraversalOn, new_w->manager.traversal_on);
    SET(XmNshadowThickness, new_w->manager.shadow_thickness);
    SET(XmNbackground, new_w->core.background_pixel);
    SET(XmNtopShadowColor, new_w->manager.top_shadow_color);
    SET(XmNbottomShadowColor, new_w->manager.bottom_shadow_color);
    SET(XmNtopShadowPixmap, new_w->manager.top_shadow_pixmap);
    SET(XmNbottomShadowPixmap, new_w->manager.bottom_shadow_pixmap);
    scrollbar = XmCreateScrollBar( (Widget) new_w, "Scrollbar", args, n);

    XtManageChild(scrollbar);

    XtAddCallback(scrollbar, XmNvalueChangedCallback, ValueChanged, NULL);
    XtAddCallback(scrollbar, XmNdragCallback, ValueChanged, NULL);

    return(scrollbar);
}


/************************************************************************
 *
 *  GetForegroundGC
 *     Get the graphics context used for drawing the slider value.
 *
 ************************************************************************/
static void
GetForegroundGC(
        XmScaleWidget sw )
{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground | GCGraphicsExposures;
   values.foreground = sw->manager.foreground;
   values.background = sw->core.background_pixel;
   values.graphics_exposures = False;
   if (sw->scale.font_struct)
     values.font = sw->scale.font_struct->fid, valueMask |= GCFont;

/*   if ((sw->core.background_pixmap != None) &&
       (sw->core.background_pixmap != XmUNSPECIFIED_PIXMAP)) {
       valueMask |= GCFillStyle | GCTile ;
       values.fill_style = FillTiled;
       values.tile = sw->core.background_pixmap;
   }*/

   /* Added dynamic clip mask & don't care about origion to merge with
      Label[Gadget] and List GC:s */
   sw->scale.foreground_GC = XtAllocateGC ((Widget) sw, 0, valueMask, &values,
					   GCClipMask,
					   GCClipXOrigin | GCClipYOrigin);
}

static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmScaleWidget req = (XmScaleWidget) rw ;
    XmScaleWidget new_w = (XmScaleWidget) nw ;

    new_w->scale.value_region = XCreateRegion();

    /* Validate the incoming data  */
    ValidateInitialState(req, new_w);

    if (new_w->scale.font_list == NULL)
	new_w->scale.font_list =
	    XmeGetDefaultRenderTable( (Widget) new_w, XmLABEL_FONTLIST);

    /*  Set the scale font struct used for interactive value display  */
    /*  to the 0th font in the title font list.  If not font list is  */
    /*  provides, open up fixed and use that.                         */

    new_w->scale.font_list = XmFontListCopy(new_w->scale.font_list);

    if (new_w->scale.font_list) {
        if (!XmeRenderTableGetDefaultFont(new_w->scale.font_list,
					  &new_w->scale.font_struct))
	    new_w->scale.font_struct = NULL;
#if !USE_XFT
    } else {
	new_w->scale.font_struct =
	  XLoadQueryFont (XtDisplay (new_w), XmDEFAULT_FONT);
	if (new_w->scale.font_struct == NULL)
	    new_w->scale.font_struct = XLoadQueryFont (XtDisplay (new_w), "*");
#endif
    }

    (void) CreateScaleTitle(new_w);
    (void) CreateScaleScrollBar(new_w);

    /*  Get the foreground GC and initialize internal variables  */

    GetForegroundGC (new_w);

    new_w->scale.show_value_x = 0;
    new_w->scale.show_value_y = 0;
    new_w->scale.show_value_width = 0;
    new_w->scale.show_value_height = 0;
    new_w->scale.state_flags = 0 ;

    /* add the handler that drags the value shown in the scale window */
    XtAddEventHandler(nw, ButtonPressMask, False, StartDrag, NULL);
}


/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *     Only redisplays the gadgets (title included) and the value,
 *       the scrollbar will take care of itself.
 *
 ************************************************************************/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    XmScaleWidget sw = (XmScaleWidget) wid ;

    XmeRedisplayGadgets( (Widget) sw, event, region);

    ShowValue (sw);
}




/************************************************************************
 *
 *  Resize
 *     Re-layout children.
 *
 ************************************************************************/
static void
Resize(
        Widget wid )
{
    XmScaleWidget sw = (XmScaleWidget) wid ;
    XtWidgetGeometry desired ;

    /* Find out what the best possible answer would be, the layout
       routines use this optimum for placing the children */
    desired.width =  0;
    desired.height =  0;
    GetScaleSize(sw, &desired.width, &desired.height);

    if (sw->scale.orientation == XmHORIZONTAL)
	LayoutHorizontalScale(sw, &desired, NULL);
    else
	LayoutVerticalScale(sw, &desired, NULL);

    /* Scale has a gravity None, so resize will always generate redisplay */
}




/************************************************************************
 *
 *  SetValues stuff
 *
 ************************************************************************/

static void
ValidateInputs(
        XmScaleWidget cur,
        XmScaleWidget new_w )
{
   float value_range;
   /* Validate the incoming data  */

   if (new_w->scale.minimum >= new_w->scale.maximum)
   {
      new_w->scale.minimum = cur->scale.minimum;
      new_w->scale.maximum = cur->scale.maximum;
      XmeWarning( (Widget) new_w, MESSAGE1);
   }

   value_range = (float)((float)new_w->scale.maximum -
			 (float)new_w->scale.minimum);
   if (value_range > (float)((float)INT_MAX / (float) 2.0))
   {
       new_w->scale.minimum = 0;
         if (new_w->scale.maximum > (INT_MAX / 2))
             new_w->scale.maximum = INT_MAX / 2;
        XmeWarning( (Widget) new_w, MESSAGE9);
   }

   if (new_w->scale.value < new_w->scale.minimum)
   {
      new_w->scale.value = new_w->scale.minimum;
      XmeWarning( (Widget) new_w, MESSAGE2);
   }

   if (new_w->scale.value > new_w->scale.maximum)
   {
      new_w->scale.value = new_w->scale.maximum;
      XmeWarning( (Widget) new_w, MESSAGE3);
   }

   if(!XmRepTypeValidValue( XmRID_SLIDING_MODE,
			   new_w->scale.sliding_mode, (Widget) new_w) )
   {
      new_w->scale.sliding_mode = cur->scale.sliding_mode;
   }


   if(!XmRepTypeValidValue( XmRID_ORIENTATION,
			   new_w->scale.orientation, (Widget) new_w)    )
       {
       new_w->scale.orientation = cur->scale.orientation;
   }


   if(!XmRepTypeValidValue( XmRID_SHOW_VALUE,
			   new_w->scale.show_value, (Widget) new_w)    )
       {
       new_w->scale.show_value = cur->scale.show_value;
   }


   if (new_w->scale.orientation == XmHORIZONTAL)
   {
      if (new_w->scale.processing_direction != XmMAX_ON_LEFT &&
          new_w->scale.processing_direction != XmMAX_ON_RIGHT)
      {
         new_w->scale.processing_direction = cur->scale.processing_direction;
         XmeWarning( (Widget) new_w, MESSAGE5);
      }
   }
   else
   {
      if (new_w->scale.processing_direction != XmMAX_ON_TOP &&
          new_w->scale.processing_direction != XmMAX_ON_BOTTOM)
      {
         new_w->scale.processing_direction = cur->scale.processing_direction;
         XmeWarning( (Widget) new_w, MESSAGE5);
      }
   }

   if (new_w->scale.scale_multiple != cur->scale.scale_multiple)
       {
	   if (new_w->scale.scale_multiple > (new_w->scale.maximum
					      - new_w->scale.minimum))
	       {
		   XmeWarning( (Widget) new_w, MESSAGE7);
		   new_w->scale.scale_multiple = (new_w->scale.maximum
						  - new_w->scale.minimum) / 10;
	       }
	   else if (new_w->scale.scale_multiple < 0)
	       {
		   XmeWarning( (Widget) new_w, MESSAGE8);
		   new_w->scale.scale_multiple = (new_w->scale.maximum
						  - new_w->scale.minimum) / 10;
	       }
	   else if (new_w->scale.scale_multiple == 0)
	       new_w->scale.scale_multiple = (new_w->scale.maximum
					      - new_w->scale.minimum) / 10;
	   /* Assure a minimum value of 1 */
	   if (new_w->scale.scale_multiple < 1)
	       new_w->scale.scale_multiple = 1;
       }
}

static void
HandleTitle(
        XmScaleWidget cur,
        XmScaleWidget req,	/* unused */
        XmScaleWidget new_w )
{
	Arg args[5];
	int n = 0;

	/* cur title is either NULL or (-1), as set in CreateScaleTitle,
	   so diff are always pertinent */
	/* new title can be NULL or a valid xmstring */
	if (new_w->scale.title != cur->scale.title) {
	    XtSetArg (args[n], XmNlabelString, new_w->scale.title);	n++;
	}

	if (new_w->scale.font_list != cur->scale.font_list) {
	    XtSetArg (args[n], XmNfontList, new_w->scale.font_list);	n++;
	}

	if (n) XtSetValues (new_w->composite.children[0], args, n);

	if (new_w->scale.title != cur->scale.title) {
	    if (new_w->scale.title != NULL) {
		/* new title differs from old one and is no null, so
		   it's a valid xmstring that we change to -1 */
		XtManageChild(new_w->composite.children[0]);
		new_w->scale.title = (XmString) -1 ;
	    }
	    else  /* new title differs from old one and is null,
		   so we let it be null, so that get scale title returns
		   null instead of the label string */
		XtUnmanageChild (new_w->composite.children[0]);
	}
}

static void
HandleScrollBar(
        XmScaleWidget cur,
        XmScaleWidget req,	/* unused */
        XmScaleWidget new_w )
{
	Arg args[30];
	int n = 0;
	Widget scrollbar = new_w->composite.children[1];
	int slider_size, increment, page, value ;

	/* reset any attributes of the scrollbar */
	SET(XmNshowArrows, new_w->scale.show_arrows);
	SET(XmNorientation, new_w->scale.orientation);
	SET(XmNprocessingDirection, new_w->scale.processing_direction);
	if (new_w->scale.scale_width != cur->scale.scale_width)
	    SET(XmNwidth, new_w->scale.scale_width);
	if (new_w->scale.scale_height != cur->scale.scale_height)
	    SET(XmNheight, new_w->scale.scale_height);
	SET(XmNslidingMode, new_w->scale.sliding_mode);
	SET(XmNsliderMark, new_w->scale.slider_mark);
	SET(XmNsliderVisual, new_w->scale.slider_visual);
	SET(XmNeditable, new_w->scale.editable);

	/* there is an issue of propagation here, whether or not
	   we want to force it. There is a behavior compatibility
	   issue if we decide to change the current situation */
	SET(XmNsensitive, new_w->core.sensitive);

	SET(XmNhighlightColor, new_w->manager.highlight_color);
        SET(XmNhighlightPixmap, new_w->manager.highlight_pixmap);
	SET(XmNhighlightThickness, new_w->scale.highlight_thickness);
	SET(XmNshadowThickness, new_w->manager.shadow_thickness);
	SET(XmNhighlightOnEnter, new_w->scale.highlight_on_enter);
	SET(XmNtraversalOn, new_w->manager.traversal_on);
	SET(XmNbackground, new_w->core.background_pixel);
	SET(XmNtopShadowColor, new_w->manager.top_shadow_color);
	SET(XmNtopShadowPixmap, new_w->manager.top_shadow_pixmap);
	SET(XmNbottomShadowColor, new_w->manager.bottom_shadow_color);
	SET(XmNbottomShadowPixmap, new_w->manager.bottom_shadow_pixmap);

	CalcScrollBarData(new_w, &value, &slider_size, &increment, &page);
	SET(XmNvalue, value);
	SET(XmNsliderSize, slider_size);
	SET(XmNincrement, increment);
	SET(XmNpageIncrement, page);

	XtSetValues (scrollbar, args, n);

	SetScrollBarData(new_w);
}


/************************************************************************
 *
 *  SetValues class method
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args_in,	/* unused */
        Cardinal *num_args_in )	/* unused */
{
    XmScaleWidget cur = (XmScaleWidget) cw ;
    XmScaleWidget req = (XmScaleWidget) rw ;
    XmScaleWidget new_w = (XmScaleWidget) nw ;
    Boolean redisplay = False ;

#define DIFF(x) ((new_w->x) != (cur->x))

    /* this flag is checked in the GM */
    new_w->scale.state_flags |= FROM_SET_VALUE ;

    if (DIFF(scale.orientation)) {

	/* Make sure that processing direction tracks orientation */
	if (!DIFF(scale.processing_direction)) {
	    if ((new_w->scale.orientation == XmHORIZONTAL) &&
		(cur->scale.processing_direction == XmMAX_ON_TOP))
		new_w->scale.processing_direction = XmMAX_ON_RIGHT;
	    else if ((new_w->scale.orientation == XmHORIZONTAL) &&
		     (cur->scale.processing_direction == XmMAX_ON_BOTTOM))
		new_w->scale.processing_direction = XmMAX_ON_LEFT;
	    else if ((new_w->scale.orientation == XmVERTICAL) &&
		     (cur->scale.processing_direction == XmMAX_ON_LEFT))
		new_w->scale.processing_direction = XmMAX_ON_BOTTOM;
	    else if ((new_w->scale.orientation == XmVERTICAL) &&
		     (cur->scale.processing_direction == XmMAX_ON_RIGHT))
		new_w->scale.processing_direction = XmMAX_ON_TOP;
	}

	/* Make scale_width and scale_height track orientation too */
	if ((new_w->scale.scale_width == cur->scale.scale_width) &&
	    (new_w->scale.scale_height == cur->scale.scale_height)) {
	    new_w->scale.scale_width = cur->scale.scale_height;
	    new_w->scale.scale_height = cur->scale.scale_width;
	}

    }

    ValidateInputs(cur, new_w);

    HandleTitle(cur, req, new_w);
    HandleScrollBar(cur, req, new_w);

	/*  Set the font struct for the value displayed  */

    if (DIFF(scale.font_list)) {

#if !USE_XFT
	if ((cur->scale.font_list == NULL) &&
	    (cur->scale.font_struct != NULL))
	    XFreeFont(XtDisplay (cur), cur->scale.font_struct);
#endif

        if (cur->scale.font_list) XmFontListFree(cur->scale.font_list);

	if (new_w->scale.font_list == NULL)
	    new_w->scale.font_list =
		XmeGetDefaultRenderTable( (Widget) new_w, XmLABEL_FONTLIST);

	new_w->scale.font_list = XmFontListCopy(new_w->scale.font_list);

	if (new_w->scale.font_list != NULL) {
	    if (!XmeRenderTableGetDefaultFont(new_w->scale.font_list,
					      &new_w->scale.font_struct))
	        new_w->scale.font_struct = NULL;
#if USE_XFT
        /* TODO: should it be ifndef? */
	} else {
	    new_w->scale.font_struct =
		XLoadQueryFont(XtDisplay(new_w), XmDEFAULT_FONT);
	    if (new_w->scale.font_struct == NULL)
		new_w->scale.font_struct =
		    XLoadQueryFont(XtDisplay(new_w), "*");
#endif
	}

	XtReleaseGC ((Widget) new_w, new_w->scale.foreground_GC);
	GetForegroundGC (new_w);
	redisplay = True;
    }


    if (XtIsRealized((Widget)new_w) &&
	( DIFF(scale.font_list) ||
	DIFF(scale.highlight_thickness) ||
	DIFF(scale.scale_height) ||
	DIFF(scale.scale_width) ||
	DIFF(scale.orientation) ||
	DIFF(manager.unit_type) ||
	DIFF(manager.shadow_thickness) ||
	/* need to check on req for title since HandleTitle made
	   the new_w field equal to cur */
	(req->scale.title != cur->scale.title) ||
	/* major show value change only */
	(DIFF(scale.show_value) &&
	 ((new_w->scale.show_value == XmNONE) ||
	  (cur->scale.show_value == XmNONE))))) {

	Dimension width=0, height=0 ;
	    /*
	     * Re-calculate the size of the Scale if a new size was not
	     * specified, and only if realized.
             */

	GetScaleSize (new_w, &width, &height);

	if (new_w->core.width == cur->core.width)
	    new_w->core.width = width ;

	if (new_w->core.height == cur->core.height)
	    new_w->core.height = height ;
    }

    if (XtIsRealized((Widget)new_w) &&
	(DIFF(scale.sliding_mode) ||
	/* minor show value change only */
	(DIFF(scale.show_value) &&
	 (new_w->scale.show_value != XmNONE) &&
	 (cur->scale.show_value != XmNONE)) ||
	DIFF(scale.show_arrows))) {
	XtWidgetProc resize;

	/* generate a relayout and ask for redisplay, only if realized */
	_XmProcessLock();
	resize = xmScaleClassRec.core_class.resize;
	_XmProcessUnlock();

	(* resize) (nw);
	redisplay = True;
    }


    if (XtIsRealized((Widget)new_w) &&
	(DIFF(scale.decimal_points) ||
	DIFF(scale.value) ||
	DIFF(scale.minimum) ||
 	DIFF(scale.maximum) ||
	DIFF(scale.processing_direction) ||
	DIFF(scale.show_value))) {
	ShowValue(new_w);
    }


    /*  See if the GC needs to be regenerated  */

    if (DIFF(manager.foreground) ||
	DIFF(core.background_pixel)||
	DIFF(core.background_pixmap)) {
	XtReleaseGC ((Widget) new_w, new_w->scale.foreground_GC);
	GetForegroundGC (new_w);
	redisplay = True;
    }

    /* unset the GM flag */
    new_w->scale.state_flags &= ~FROM_SET_VALUE ;

    return (redisplay);
#undef DIFF
}



/************************************************************************
 *
 *  Realize
 *	Can't use the standard Manager class realize procedure,
 *      because it creates a window with NW gravity, and the
 *      scale wants a gravity of None.
 *
 ************************************************************************/
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
   attributes->bit_gravity = ForgetGravity;
   attributes->do_not_propagate_mask =
      ButtonPressMask | ButtonReleaseMask |
      KeyPressMask | KeyReleaseMask | PointerMotionMask;

   XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);
}



/************************************************************************
 *
 *  Destroy
 *	Free the callback lists attached to the scale.
 *
 ************************************************************************/
static void
Destroy(
        Widget wid )
{
    XmScaleWidget sw = (XmScaleWidget) wid ;

    XtReleaseGC ((Widget) sw, sw->scale.foreground_GC);

#if USE_XFT
    if (sw->scale.font_list == NULL && sw->scale.font_struct != NULL)
	XFreeFont (XtDisplay (sw), sw->scale.font_struct);
#endif

    if (sw->scale.font_list) XmFontListFree(sw->scale.font_list);

    if (sw->scale.value_region)
      XDestroyRegion(sw->scale.value_region);
}




/************************************************************************
 *
 *  QueryGeometry
 *
 ************************************************************************/
static XtGeometryResult
QueryGeometry(
        Widget widget,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    /* deal with user initial size setting */
    if (!XtIsRealized(widget))  {
	desired->width = XtWidth(widget) ;    /* might be 0 */
	desired->height = XtHeight(widget) ;  /* might be 0 */
    } else {
	/* always computes natural size afterwards */
	desired->width = 0 ;
	desired->height = 0 ;
    }

    GetScaleSize ((XmScaleWidget) widget, &desired->width, &desired->height);

    /* this function will set CWidth and CHeight */
    return XmeReplyToQueryGeometry(widget, intended, desired) ;
}


/************************************************************************
 *
 *  GeometryManager
 *	Accept everything except change in position.
 *
 ************************************************************************/
static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply ) /* unused */
{
    XtWidgetGeometry desired ;
    XmScaleWidget sw = (XmScaleWidget) XtParent(w) ;

    if (IsQueryOnly(request)) return XtGeometryYes;


    if (IsWidth(request)) w->core.width = request->width;
    if (IsHeight(request)) w->core.height = request->height;
    if (IsBorder(request)) w->core.border_width = request->border_width;

    /* no need to do any layout if it is our change, Xt will
     generate one at the end */
    if (sw->scale.state_flags & FROM_SET_VALUE) return XtGeometryYes;


    /* Find out what the best possible answer would be */
    desired.width =  0;
    desired.height =  0;
    GetScaleSize(sw, &desired.width, &desired.height);

    /* ask that to our parent */
    desired.request_mode = (CWWidth | CWHeight);
    _XmMakeGeometryRequest((Widget) sw, &desired);

    /* layout using the new size (accepted or not) */
    if (sw->scale.orientation == XmHORIZONTAL)
	LayoutHorizontalScale(sw, &desired, w);
    else /* sw->scale.orientation == XmVERTICAL */
	LayoutVerticalScale(sw, &desired, w);

    return XtGeometryYes;
}




/*********************************************************************
 *  ChangeManaged
 *     Layout children.
 *
 *********************************************************************/
static void
ChangeManaged(
        Widget wid )
{
    XmScaleWidget sw = (XmScaleWidget) wid ;
    XtWidgetGeometry desired ;
    Dimension tmp_width = 0, tmp_height = 0 ;

    GetScaleSize(sw, &tmp_width, &tmp_height);
    desired.width = tmp_width ;
    desired.height = tmp_height ;

    if (!XtIsRealized((Widget)sw))  {
	/* the first time, only attemps to change non specified sizes */
	if (XtWidth(sw)) desired.width = XtWidth(sw) ;
	if (XtHeight(sw)) desired.height = XtHeight(sw) ;
    }

    desired.request_mode = (CWWidth | CWHeight);
    _XmMakeGeometryRequest((Widget) sw, &desired);

    /* layout with no instigator, but with our real preferred size */
    desired.width = tmp_width ;
    desired.height = tmp_height ;
    if (sw->scale.orientation == XmHORIZONTAL)
	LayoutHorizontalScale(sw, &desired, NULL);
    else
	LayoutVerticalScale(sw, &desired, NULL);

    XmeNavigChangeManaged( (Widget) sw);
}





static void
GetScaleSize(
        XmScaleWidget sw,
        Dimension *w,
        Dimension *h )
{
    Dimension sav_w, sav_h;

    sav_w = XtWidth(sw);
    sav_h = XtHeight(sw);

    /* Mark the scale as anything goes */
    XtWidth(sw) = *w;
    XtHeight(sw) = *h;

    /* only override the pointed dimensions if they are null */

    if (sw->scale.orientation == XmHORIZONTAL)  {
	if (!*w) {
	    *w = MAX(TitleWidth(sw),
		     MajorLeadPad(sw) + ScrollWidth(sw) + MajorTrailPad(sw));
	}

	if (!*h) {
	    *h = MaxLabelHeight(sw) + ValueTroughHeight(sw)
		+ ScrollHeight(sw) + TitleHeight(sw);
	    if (sw->scale.show_value) *h += SCALE_VALUE_MARGIN;
	}
    } else /* sw->scale.orientation == XmVERTICAL */  {
	if (!*w) {
	    *w = MaxLabelWidth(sw) + ValueTroughWidth(sw)
		+ ScrollWidth(sw) + TitleWidth(sw);
	    if (sw->scale.show_value) *w += SCALE_VALUE_MARGIN;
	}

	if (!*h) {
	    *h = MAX(TitleHeight(sw),
		     MajorLeadPad(sw) + ScrollHeight(sw) + MajorTrailPad(sw));
	}
    }

    /* Don't ever desire 0 dimensions */
    if (!*w) *w = 1;
    if (!*h) *h = 1;

    /* Restore the current values */
    XtWidth(sw) = sav_w;
    XtHeight(sw) = sav_h;

}


static Dimension
MaxLabelWidth(
        XmScaleWidget sw )
{
    int i;
    Widget c;
    Dimension max = 0;

    /* start at 2 to skip the title and the scrollbar */
    for ( i = 2; i < sw->composite.num_children; i++)
	{
	    c = sw->composite.children[i];
	    if (XtIsManaged(c) &&
		!((Object)c)->object.being_destroyed)
		ASSIGN_MAX(max, TotalWidth(c));
	}

    return (max);
}

static Dimension
MaxLabelHeight(
        XmScaleWidget sw )
{
    int i;
    Widget c;
    Dimension max = 0;

    /* start at 2 to skip the title and the scrollbar */
    for ( i = 2; i < sw->composite.num_children; i++)
	{
	    c = sw->composite.children[i];
	    if (XtIsManaged(c) &&
		!((Object)c)->object.being_destroyed)
		ASSIGN_MAX(max, TotalHeight(c));
	}

    return (max);
}

static Dimension
ValueTroughHeight(
        XmScaleWidget sw)
{
#if USE_XFT
    int ret_val = 0;
    if (sw->scale.show_value) {
        XmRenderTableGetDefaultFontExtents(sw->scale.font_list,
                                           &ret_val, NULL, NULL);
    }
    return (Dimension)ret_val;
#else
    char buff[15];
    Dimension tmp_max, tmp_min, result;
    int direction, ascent, descent;
    XCharStruct overall_return;

#define GET_MAX(tmp, max_or_min_value) {\
    if (sw->scale.decimal_points)\
	    sprintf(buff, "%d%c", max_or_min_value,\
		    nl_langinfo(RADIXCHAR)[0]);\
	else\
	    sprintf(buff, "%d", max_or_min_value);\
	    \
	XTextExtents(sw->scale.font_struct, buff, strlen(buff),\
		     &direction, &ascent, &descent, &overall_return);\
	    \
	    tmp = ascent + descent;\
	    }

    if (sw->scale.show_value) {
	GET_MAX(tmp_max, sw->scale.maximum) ;
	GET_MAX(tmp_min, sw->scale.minimum) ;
	result = MAX(tmp_min, tmp_max);
	return (result);
	}
    else
	return (0);
#undef GET_MAX
#endif
}

static Dimension
ValueTroughAscent(
        XmScaleWidget sw)
{
#if USE_XFT
    int ret_val = 0;
    if (sw->scale.show_value) {
        XmRenderTableGetDefaultFontExtents(sw->scale.font_list,
                                           NULL, &ret_val, NULL);
    }
    return (Dimension)ret_val;
#else
    char buff[15];
    Dimension tmp_max, tmp_min, result;
    int direction, ascent, descent;
    XCharStruct overall_return;

#define GET_MAX(tmp, max_or_min_value) {\
    if (sw->scale.decimal_points)\
	    sprintf(buff, "%d%c", max_or_min_value,\
		    nl_langinfo(RADIXCHAR)[0]);\
	else\
	    sprintf(buff, "%d", max_or_min_value);\
	    \
	XTextExtents(sw->scale.font_struct, buff, strlen(buff),\
		     &direction, &ascent, &descent, &overall_return);\
	    \
	    tmp = ascent;\
	    }

    if (sw->scale.show_value) {
	GET_MAX(tmp_max, sw->scale.maximum) ;
	GET_MAX(tmp_min, sw->scale.minimum) ;
	result = MAX(tmp_min, tmp_max);
	return (result);
	}
    else
	return (0);
#undef GET_MAX
#endif
}

static Dimension
ValueTroughDescent(
        XmScaleWidget sw)
{
#if USE_XFT
    int ret_val = 0;
    if (sw->scale.show_value) {
        XmRenderTableGetDefaultFontExtents(sw->scale.font_list,
                                           NULL, NULL, &ret_val);
    }
    return (Dimension)ret_val;
#else
    char buff[15];
    Dimension tmp_max, tmp_min, result;
    int direction, ascent, descent;
    XCharStruct overall_return;

#define GET_MAX(tmp, max_or_min_value) {\
    if (sw->scale.decimal_points)\
	    sprintf(buff, "%d%c", max_or_min_value,\
		    nl_langinfo(RADIXCHAR)[0]);\
	else\
	    sprintf(buff, "%d", max_or_min_value);\
	    \
	XTextExtents(sw->scale.font_struct, buff, strlen(buff),\
		     &direction, &ascent, &descent, &overall_return);\
	    \
	    tmp = descent;\
	    }

    if (sw->scale.show_value) {
	GET_MAX(tmp_max, sw->scale.maximum) ;
	GET_MAX(tmp_min, sw->scale.minimum) ;
	result = MAX(tmp_min, tmp_max);
	return (result);
	}
    else
	return (0);
#undef GET_MAX
#endif
}

static Dimension
ValueTroughWidth(
        XmScaleWidget sw)
{
    char buff[15];
    Dimension tmp_max, tmp_min, result;
    int direction, ascent, descent;
    XCharStruct overall_return;

#if USE_XFT
#define GET_MAX(tmp, max_or_min_value) {\
    XmString tmp_str;\
    if (sw->scale.decimal_points)\
	    sprintf(buff, "%d%c", max_or_min_value,\
		    nl_langinfo(RADIXCHAR)[0]);\
	else\
	    sprintf(buff, "%d", max_or_min_value);\
	    \
    tmp = XmStringWidth(sw->scale.font_list, tmp_str = XmStringCreateLocalized(buff));\
    XmStringFree(tmp_str);\
}
#else
#define GET_MAX(tmp, max_or_min_value) {\
    if (sw->scale.decimal_points)\
	    sprintf(buff, "%d%c", max_or_min_value,\
		    nl_langinfo(RADIXCHAR)[0]);\
	else\
	    sprintf(buff, "%d", max_or_min_value);\
	    \
	XTextExtents(sw->scale.font_struct, buff, strlen(buff),\
		     &direction, &ascent, &descent, &overall_return);\
	    \
	    tmp = overall_return.rbearing - overall_return.lbearing;\
	    }
#endif

    if (sw->scale.show_value) {
	GET_MAX(tmp_max, sw->scale.maximum) ;
	GET_MAX(tmp_min, sw->scale.minimum) ;
	result = MAX(tmp_min, tmp_max);
	return (result);
	}
    else
	return (0);
#undef GET_MAX
}


static Dimension
TitleWidth(
        XmScaleWidget sw )
{
    Dimension tmp = 0;
    Widget title_widget = sw->composite.children[0];

    if (XtIsManaged(title_widget)) {
	tmp = TotalWidth(title_widget) ;

	if (sw->scale.orientation == XmVERTICAL)
	    tmp += (TotalHeight(title_widget)) >> 2;
    }

    return(tmp);
}



static Dimension
TitleHeight(
        XmScaleWidget sw )
{
    Dimension tmp = 0;
    Widget title_widget = sw->composite.children[0];

    if (XtIsManaged(title_widget)) {
	tmp = TotalHeight(title_widget);

	if (sw->scale.orientation == XmHORIZONTAL)
	    tmp += (TotalHeight(title_widget)) >> 2;
    }

    return(tmp);
}


static Cardinal
NumManaged(
        XmScaleWidget sw,
        Widget * first_man,
        Widget * last_man)
{
    Cardinal i, num_managed = 0 ;
    Widget first_tic = NULL, last_tic = NULL, c  ;

    for (i = 2; i < sw->composite.num_children; i++) {
	c = sw->composite.children[i];
	if (XtIsManaged(c) &&
	    !((Object)c)->object.being_destroyed) {
	    num_managed ++ ;
	    if (!first_tic) first_tic = c ;
	    last_tic = c ;
	}
    }

    if (first_man) *first_man = first_tic ;
    if (last_man) *last_man = last_tic ;

    return num_managed + 2 ;
}


static Dimension
MajorLeadPad(
        XmScaleWidget sw )
{
    XmScrollBarWidget sb = (XmScrollBarWidget)(sw->composite.children[1]);
    int tmp1 = 0, tmp2;
    Cardinal num_managed ;
    Widget first_tic  ;

    num_managed = NumManaged(sw, &first_tic, NULL);

    if (num_managed > 3) {
	if (sw->scale.orientation == XmHORIZONTAL)
	    tmp1 = (TotalWidth(first_tic) / 2)
		- LeadXTic(sb, sw);
	else
	    tmp1 = (TotalHeight(first_tic) / 2)
		- LeadYTic(sb, sw);

    } else if (num_managed == 3) {
	/*
	 * This is a potential non-terminal recursion.
	 *
	 * Currently MajorScrollSize has knowledge of this potential
	 * problem and has guards around the call to this procedure.
	 * Modify with care.
	 */

	if (sw->scale.orientation == XmHORIZONTAL)
	    tmp1 = ((int)TotalWidth(first_tic) -
		    (int)ScrollWidth(sw))/2;
	else
	    tmp1 = ((int)TotalHeight(first_tic) -
		    (int)ScrollHeight(sw))/2;
    }

    tmp1 -= (sb->primitive.highlight_thickness + sb->primitive.shadow_thickness);

    if (sw->scale.orientation == XmHORIZONTAL)
	tmp2 = ((int)ValueTroughWidth(sw) / 2) - (int)LeadXTic(sb, sw);
    else {
	if (sw->scale.sliding_mode == XmTHERMOMETER)
	    tmp2 = ((int)ValueTroughAscent(sw)) - (int)LeadYTic(sb, sw);
	else
	    tmp2 = ((int)ValueTroughHeight(sw) / 2) - (int)LeadYTic(sb, sw);
    }

    tmp2 -= (sb->primitive.highlight_thickness
	     + sb->primitive.shadow_thickness);

    ASSIGN_MAX(tmp1, 0);
    ASSIGN_MAX(tmp2, 0);

    return(MAX(tmp1, tmp2));
}



static Dimension
MajorTrailPad(
        XmScaleWidget sw )
{
    XmScrollBarWidget sb = (XmScrollBarWidget) (sw->composite.children[1]);
    int tmp1 = 0, tmp2;
    Cardinal num_managed ;
    Widget first_tic, last_tic ;

    num_managed = NumManaged(sw, &first_tic, &last_tic);

    if (num_managed > 3) {
	if (sw->scale.orientation == XmHORIZONTAL)
	    tmp1 = ((int)TotalWidth(last_tic) / 2)
				   - (int)TrailXTic(sb, sw);
	else
	    tmp1 = ((int)TotalHeight(last_tic) / 2)
				   - (int)TrailYTic(sb, sw);
    } else if (num_managed == 3) {
	/*
	 * This is a potential non-terminal recursion.
	 *
	 * Currently MajorScrollSize has knowledge of this potential
	 * problem and has guards around the call to this procedure.
	 * Modify with care.
	 */

	if (sw->scale.orientation == XmHORIZONTAL)
	    tmp1 = ((int)TotalWidth(first_tic) -
		    (int)ScrollWidth(sw))/2;
	else
	    tmp1 = ((int)TotalHeight(first_tic) -
		    (int)ScrollHeight(sw))/2;
    }

    tmp1 -= (sb->primitive.highlight_thickness
		+ sb->primitive.shadow_thickness);

    if (sw->scale.orientation == XmHORIZONTAL)
	tmp2 = ((int)ValueTroughWidth(sw) / 2) - (int)TrailXTic(sb, sw);
    else {
	if (sw->scale.sliding_mode == XmTHERMOMETER)
	    tmp2 = ((int)ValueTroughDescent(sw)) - (int)TrailYTic(sb, sw);
	else
	    tmp2 = ((int)ValueTroughHeight(sw) / 2) - (int)TrailYTic(sb, sw);
    }

    tmp2 += (sb->primitive.highlight_thickness
	     + sb->primitive.shadow_thickness);

    ASSIGN_MAX(tmp1, 0);
    ASSIGN_MAX(tmp2, 0);

    return(MAX(tmp1, tmp2));
}



static Dimension
ScrollWidth(
        XmScaleWidget sw )
{
    int tmp = 0;

    if (sw->scale.orientation == XmVERTICAL) {
	if (!(tmp = sw->scale.scale_width))
	    tmp = SCALE_DEFAULT_MINOR_SIZE;
	else
	    tmp = sw->scale.scale_width;
    } else {
	if (!(tmp = sw->scale.scale_width)) {
	    if (sw->core.width != 0) {
		Cardinal num_managed ;

		num_managed = NumManaged(sw, NULL, NULL);
		/* Have to catch an indirect recursion here */
		if (num_managed > 3)
		    tmp = (int)sw->core.width
			- (MajorLeadPad(sw) + MajorTrailPad(sw));
		else {
		    /* Magic to handle excessively wide values */
		    int tmp1, tmp2;
		    XmScrollBarWidget sb = (XmScrollBarWidget)
			sw->composite.children[1];

		    tmp1 = ((int)ValueTroughWidth(sw) / 2) -
			(int)LeadXTic(sb, sw);
		    tmp2 = ((int)ValueTroughWidth(sw) / 2) -
			(int)TrailXTic(sb, sw);
		    ASSIGN_MAX(tmp1, 0);
		    ASSIGN_MAX(tmp2, 0);
		    tmp = (int)sw->core.width - tmp1 - tmp2;
		}
	    }
	}

	if (tmp <= 0) {
	    Cardinal num_managed ;

	    num_managed = NumManaged(sw, NULL, NULL);

	    if (num_managed > 2) {
		/* Have to catch an indirect recursion here */
		if (num_managed > 3) {
		    Dimension tic, diff;
		    XmScrollBarWidget sb = (XmScrollBarWidget)
			sw->composite.children[1];

		    tmp = MAX((num_managed - 2)* MaxLabelWidth(sw), SCALE_DEFAULT_MAJOR_SIZE);
		    tic = sb->primitive.highlight_thickness
			+ sb->primitive.shadow_thickness
			    + (Dimension) (((float) SLIDER_SIZE( sw) / 2.0)
					   + 0.5);

		    diff = tic - ((int)MaxLabelWidth(sw) / 2);

		    if (diff > 0) tmp+= (2 * diff);
		}
		else
			tmp = MAX(MaxLabelWidth(sw), SCALE_DEFAULT_MAJOR_SIZE);
	    }
	}

	if (tmp <= 0) tmp = SCALE_DEFAULT_MAJOR_SIZE;
    }

    return((Dimension) tmp);
}

static Dimension
ScrollHeight(
        XmScaleWidget sw )
{
    int tmp;

    if (sw->scale.orientation == XmHORIZONTAL) {
	if (!(tmp = sw->scale.scale_height))
	    tmp = SCALE_DEFAULT_MINOR_SIZE;
	else
	    tmp = sw->scale.scale_height;
    }	else {
	if (!(tmp = sw->scale.scale_height)){
	    if (sw->core.height != 0)
		{
		    Cardinal num_managed ;

		    num_managed = NumManaged(sw, NULL, NULL);
		    /* Have to catch an indirect recursion here */
		    if (num_managed > 3)
			tmp = (int)sw->core.height
			    - (MajorLeadPad(sw) + MajorTrailPad(sw));
		    else
		    {
			    int tmp1, tmp2;
			    XmScrollBarWidget sb = (XmScrollBarWidget)
				sw->composite.children[1];

			    tmp1 = ((int)ValueTroughHeight(sw) / 2) -
			    			(int)LeadYTic(sb, sw);
			    tmp2 = ((int)ValueTroughHeight(sw) / 2) -
			    			(int)TrailYTic(sb, sw);

			    ASSIGN_MAX(tmp1, 0);
			    ASSIGN_MAX(tmp2, 0);
			    tmp = (int)sw->core.height - tmp1 - tmp2;
		    }
		}
	}

	if (tmp <= 0){
	    Cardinal num_managed ;

	    num_managed = NumManaged(sw, NULL, NULL);

	    if (num_managed > 2){
		/* Have to catch an indirect recursion here */
		    if (num_managed > 3) {
			Dimension tic, diff;
			XmScrollBarWidget sb = (XmScrollBarWidget)
			    sw->composite.children[1];

			tmp = MAX((num_managed - 2)* MaxLabelHeight(sw), SCALE_DEFAULT_MAJOR_SIZE);
			tic = sb->primitive.highlight_thickness
			    + sb->primitive.shadow_thickness
				+ (Dimension) (((float) SLIDER_SIZE(sw) / 2.0)
					       + 0.5);

			diff = tic - (MaxLabelHeight(sw) / 2);

			if (diff > 0) tmp+= (2 * diff);
		    }
		else
		    tmp = MAX(MaxLabelHeight(sw), SCALE_DEFAULT_MAJOR_SIZE);
	    }
	}

	if (tmp <= 0) tmp = SCALE_DEFAULT_MAJOR_SIZE;
    }

    return((Dimension)tmp);
}



static void
LayoutHorizontalLabels(
		       XmScaleWidget sw,
		       XRectangle *scrollBox,
		       XRectangle *labelBox,
		       Widget instigator )
{
	Dimension first_tic_dim, last_tic_dim;
	float tic_interval, tmp ;
	XmScrollBarWidget sb = (XmScrollBarWidget)
		(sw->composite.children[1]);
	Widget w, first_tic;
	int i;
	Position x, y, y1;
	Cardinal num_managed ;

	y1 = labelBox->y + labelBox->height;

	num_managed = NumManaged(sw, &first_tic, NULL);

	if (num_managed > 3)
	{
	  first_tic_dim = scrollBox->x + LeadXTic(sb, sw);
	  last_tic_dim = (scrollBox->x + sb->core.width) - TrailXTic(sb, sw);
	  tic_interval = (float)(last_tic_dim - first_tic_dim)
	    / (num_managed - 3);

	  for (i = 2, tmp = first_tic_dim;
	       i < sw->composite.num_children;
	       i++)
	    {
	      if (LayoutIsRtoLM(sw) &&
		  sw->scale.processing_direction == XmMAX_ON_LEFT)
		w = sw->composite.children[sw->composite.num_children - i + 1];
	      else
		w = sw->composite.children[i];

	      if (!XtIsManaged(w) ||
		  ((Object)w)->object.being_destroyed) continue ;

	      x = (int) tmp - (TotalWidth(w) / 2);
	      y = y1 - TotalHeight(w);
	      if (instigator != w)
		XmeConfigureObject(w, x, y,
				   w->core.width, w->core.height,
				   w->core.border_width);
	      else {
		w->core.x = x ;
		w->core.y = y ;
	      }

	      tmp += tic_interval ;
	    }
	}
	else if (num_managed == 3)
	{
		w = first_tic;
		y = y1 - TotalHeight(w);
		if (XtIsManaged(w) &&
		    !((Object)w)->object.being_destroyed) {

		    tmp = (sb->scrollBar.slider_area_width -
			   TotalWidth(w)) / 2;
		    x = scrollBox->x + sb->scrollBar.slider_area_x
			+ (int) tmp ;
		    if (instigator != w)
			XmeConfigureObject(w, x, y, w->core.width,
					   w->core.height,
					   w->core.border_width);
		    else {
			w->core.x = x ;
			w->core.y = y ;
		    }
		}
	}
}

static void
LayoutHorizontalScale(
        XmScaleWidget sw,
	XtWidgetGeometry * desired,
	Widget instigator)
{
	int diff_w, diff_h, tdiff;
	XRectangle labelBox, valueBox, scrollBox, titleBox;

	diff_w = XtWidth(sw) - desired->width;
	diff_h = XtHeight(sw) - desired->height;


	titleBox.height = TitleHeight(sw);
	scrollBox.height = ScrollHeight(sw);
	valueBox.height = ValueTroughHeight(sw);
	labelBox.height = MaxLabelHeight(sw);

	/* Figure out all of the y locations */
	if (diff_h >= 0)
	{
		/*
		 * We place the title, scrollbar, and value from the right
		 */
		titleBox.y = XtHeight(sw) - titleBox.height;
		scrollBox.y = titleBox.y - scrollBox.height;

		if (sw->scale.show_value == XmNEAR_BORDER) {
		    valueBox.y = 0;
		    labelBox.y = scrollBox.y - labelBox.height;
		} else { /* NEAR_SLIDER or NONE */
		    labelBox.y = 0;
		    valueBox.y = scrollBox.y - valueBox.height;
		}
	}
	else if ((tdiff = diff_h + TitleHeight(sw)) >= 0)
	{
		/* Place from the left and let the title get clipped */

		if (sw->scale.show_value == XmNEAR_BORDER) {
		    valueBox.y = 0;
		    labelBox.y = valueBox.y + valueBox.height;
		} else { /* NEAR_SLIDER or NONE */
		    labelBox.y = 0;
		    valueBox.y = labelBox.y + labelBox.height;
		}

		scrollBox.y = valueBox.y + valueBox.height;

		titleBox.y = scrollBox.y + scrollBox.height;
	}
	else if ((tdiff += ValueTroughHeight(sw)) >= 0)
	{
		/*
		 * The title is outside the window, and the labels are
		 * allowed overwrite (occlude) the value display region
		 */
		titleBox.y = XtHeight(sw);
		scrollBox.y = titleBox.y - scrollBox.height;

		if (sw->scale.show_value == XmNEAR_BORDER) {
		    valueBox.y = 0;
		    labelBox.y = scrollBox.y - labelBox.height;
		} else { /* NEAR_SLIDER or NONE */
		    valueBox.y = scrollBox.y - valueBox.height;
		    labelBox.y = 0;
		}
	}
	else if ((tdiff += MaxLabelHeight(sw)) >= 0)
	{
		/*
		 * The title is outside the window, the value trough is
		 * completely coincident with the label region, and the
		 * labels are clipped from the left
		 */
		titleBox.y = XtHeight(sw);
		scrollBox.y = titleBox.y - scrollBox.height;
		labelBox.y = scrollBox.y - labelBox.height;
		valueBox.y = scrollBox.y - valueBox.height;
	}
	else
	{
		/*
		 * Just center the scrollbar in the available space.
		 */
		titleBox.y = XtHeight(sw);
		valueBox.y = titleBox.y;
		labelBox.y = valueBox.y;
		scrollBox.y = (XtHeight(sw) - ScrollHeight(sw)) / 2;
	}

	if (diff_w >= 0)
	{
		scrollBox.x = MajorLeadPad(sw);
		scrollBox.width = ScrollWidth(sw);
	}
	else
	{
		Dimension sb_min, avail, lp, tp;
		XmScrollBarWidget sb = (XmScrollBarWidget)
			(sw->composite.children[1]);

		sb_min = (2 * sb->primitive.highlight_thickness)
			+ (4 * sb->primitive.shadow_thickness)
			+ SLIDER_SIZE( sw);

		lp = MajorLeadPad(sw);
		tp = MajorTrailPad(sw);
		avail = XtWidth(sw) - lp - tp;

		if (avail < sb_min)
		{
			scrollBox.width = sb_min;
			scrollBox.x = (XtWidth(sw) - sb_min) / 2;
		}
		else
		{
			scrollBox.width = avail;
			scrollBox.x = lp;
		}
	}

        if (LayoutIsRtoLM(sw))
        {
           titleBox.x = ScrollWidth(sw) - TitleWidth(sw);
           XmeConfigureObject(sw->composite.children[0],
			      titleBox.x, titleBox.y,
			      (sw->composite.children[0])->core.width,
			      (sw->composite.children[0])->core.height,
			      (sw->composite.children[0])->core.border_width);
        }
        else
	  if (instigator != sw->composite.children[0])
	    XmeConfigureObject(sw->composite.children[0], 0, titleBox.y,
			       (sw->composite.children[0])->core.width,
			       (sw->composite.children[0])->core.height,
			       (sw->composite.children[0])->core.border_width);
	  else {
	    sw->composite.children[0]->core.x = 0 ;
	    sw->composite.children[0]->core.y = titleBox.y ;
	  }


	if (instigator != sw->composite.children[1])
	    XmeConfigureObject(sw->composite.children[1],
			       scrollBox.x, scrollBox.y,
			       scrollBox.width, scrollBox.height, 0);
	else {
	    sw->composite.children[1]->core.x = scrollBox.x ;
	    sw->composite.children[1]->core.y = scrollBox.y ;
	    sw->composite.children[1]->core.width = scrollBox.width ;
	    sw->composite.children[1]->core.height = scrollBox.height ;
	    sw->composite.children[1]->core.border_width = 0 ;
	}

	SetScrollBarData(sw);

	LayoutHorizontalLabels(sw, &scrollBox, &labelBox, instigator);
}

static void
LayoutVerticalLabels(
		     XmScaleWidget sw,
		     XRectangle *scrollBox,
		     XRectangle *labelBox,
		     Widget instigator )
{
	Dimension first_tic_dim, last_tic_dim;
	XmScrollBarWidget sb = (XmScrollBarWidget)
		(sw->composite.children[1]);
	Widget w, first_tic;
	int i;
	float tmp, tic_interval;
	Position x, x1, y;
	Cardinal num_managed ;

	num_managed = NumManaged(sw, &first_tic, NULL);

	x1 = labelBox->x + labelBox->width;

	if (num_managed > 3)
	{
		first_tic_dim = scrollBox->y + LeadYTic(sb, sw);
		last_tic_dim = (scrollBox->y + sb->core.height) -
		    TrailYTic(sb, sw);
		tic_interval = (float)(last_tic_dim - first_tic_dim)
			/ (num_managed - 3);

		for (i = 2, tmp = first_tic_dim;
			i < sw->composite.num_children;
			i++)
		{
			w = sw->composite.children[i];
			if (!XtIsManaged(w) ||
			    ((Object)w)->object.being_destroyed) continue ;

			y = (int) tmp - (TotalHeight(w) / 2);
			if (LayoutIsRtoLM(sw))
			  x = labelBox->x;
			else
			  x = x1 - TotalWidth(w);
			if (instigator != w)
			    XmeConfigureObject(w, x, y,
					       w->core.width, w->core.height,
					       w->core.border_width);
			else {
			    w->core.x = x ;
			    w->core.y = y ;
			}

			tmp += tic_interval ;
		}
	}
	else if (num_managed == 3)
	{
		w = first_tic;
		if (XtIsManaged(w) &&
		    !((Object)w)->object.being_destroyed) {

		    x = x1 - TotalWidth(w);
		    tmp = (sb->scrollBar.slider_area_height -
			   TotalHeight(w)) / 2;
		    y = scrollBox->y + sb->scrollBar.slider_area_y
			+ (int) tmp;
		    if (instigator != w)
			XmeConfigureObject(w, x, y, w->core.width,
					   w->core.height,
					   w->core.border_width);
		    else {
			w->core.x = x ;
			w->core.y = y ;
		    }
		}
	}
}

static void
LayoutVerticalScale(
        XmScaleWidget sw,
	XtWidgetGeometry * desired,
	Widget instigator)
{
	int diff_w, diff_h, tdiff;
	XRectangle labelBox, valueBox, scrollBox, titleBox;

	diff_w = XtWidth(sw) - desired->width;
	diff_h = XtHeight(sw) - desired->height;

	titleBox.width = TitleWidth(sw);
	scrollBox.width = ScrollWidth(sw);
	valueBox.width = ValueTroughWidth(sw);
	labelBox.width = MaxLabelWidth(sw);

	/* Figure out all of the x locations */
	if (diff_w >= 0)
	{
	  if (LayoutIsRtoLM(sw)) {
	    /*
	     * Place the title, scrollbar, and value from the left
	     */
	    titleBox.x = 0;
	    scrollBox.x = titleBox.x + titleBox.width;

	    if (sw->scale.show_value == XmNEAR_BORDER) {
	      valueBox.x = XtWidth(sw) - valueBox.width;
	      labelBox.x = scrollBox.x + scrollBox.width;
	    } else { /* NEAR_SLIDER or NONE */
	      valueBox.x = scrollBox.x + scrollBox.width;
	      labelBox.x = XtWidth(sw) - labelBox.width;
	    }
	  } else {
	    /*
	     * We place the title, scrollbar, and value from the right
	     */
	    titleBox.x = XtWidth(sw) - titleBox.width;
	    scrollBox.x = titleBox.x - scrollBox.width;

	    if (sw->scale.show_value == XmNEAR_BORDER) {
	      valueBox.x = 0;
	      labelBox.x = scrollBox.x - labelBox.width;
	    } else { /* NEAR_SLIDER or NONE */
	      valueBox.x = scrollBox.x - valueBox.width;
	      labelBox.x = 0;
	    }
	  }
	}
	else if ((tdiff = diff_w + TitleWidth(sw)) >= 0)
	{
	  if (LayoutIsRtoLM(sw)) {
	    /* Place from the right and let the title get clipped */
	    if (sw->scale.show_value == XmNEAR_BORDER) {
	      valueBox.x = XtWidth(sw) - labelBox.width;
	      labelBox.x = valueBox.x - valueBox.width;
	      scrollBox.x = labelBox.x - scrollBox.width;
	    } else { /* NEAR_SLIDER or NONE */
	      labelBox.x = XtWidth(sw) - labelBox.width;
	      valueBox.x = labelBox.x - valueBox.width;
	      scrollBox.x = valueBox.x - scrollBox.width;
	    }

	    titleBox.x = scrollBox.x - titleBox.width;
	  } else {
	    /* Place from the left and let the title get clipped */
	    if (sw->scale.show_value == XmNEAR_BORDER) {
		valueBox.x = 0;
		labelBox.x = valueBox.x + valueBox.width;
		scrollBox.x = labelBox.x + labelBox.width;
	    } else { /* NEAR_SLIDER or NONE */
		labelBox.x = 0;
		valueBox.x = labelBox.x + labelBox.width;
		scrollBox.x = valueBox.x + valueBox.width;
	    }

	    titleBox.x = scrollBox.x + scrollBox.width;
	  }
	}
	else if ((tdiff += ValueTroughWidth(sw)) >= 0)
	{
		/*
		 * The title is outside the window, and the labels are
		 * allowed overwrite (occlude) the value display region
		 */
                if (LayoutIsRtoLM(sw))
                {
                   titleBox.x = -titleBox.width;
                   scrollBox.x = 0;
		   if (sw->scale.show_value == XmNEAR_BORDER) {
		     labelBox.x = scrollBox.x + scrollBox.width;
		     valueBox.x = XtWidth(sw) - valueBox.width;
		   } else { /* NEAR_SLIDER or NONE */
		     valueBox.x = scrollBox.x + scrollBox.width;
		     labelBox.x = XtWidth(sw) - labelBox.width;
		   }
                 } else {
		   titleBox.x = XtWidth(sw);
		   scrollBox.x = titleBox.x - scrollBox.width;

		   if (sw->scale.show_value == XmNEAR_BORDER) {
		     labelBox.x = scrollBox.x - labelBox.width;
		     valueBox.x = 0;
		   } else { /* NEAR_SLIDER or NONE */
		     valueBox.x = scrollBox.x - valueBox.width;
		     labelBox.x = 0;
		   }
		 }
	}
	else if ((tdiff += MaxLabelWidth(sw)) >= 0)
	{
		/*
		 * The title is outside the window, the value trough is
		 * completely coincident with the label region, and the
		 * labels are clipped from the left
		 */
		titleBox.x = XtWidth(sw);
		scrollBox.x = titleBox.x - scrollBox.width;
		valueBox.x = scrollBox.x - valueBox.width;
		labelBox.x = scrollBox.x - labelBox.width;
                if (LayoutIsRtoLM(sw))
                {
                   titleBox.x = -titleBox.width;
                   scrollBox.x = 0;
                   valueBox.x = scrollBox.x + scrollBox.width;
                   labelBox.x = scrollBox.x + scrollBox.width;
                }
	}
	else
	    {
		/*
		 * Just center the scrollbar in the available space.
		 */
		titleBox.x = XtWidth(sw);
		valueBox.x = titleBox.x;
		labelBox.x = valueBox.x;
		scrollBox.x = (XtWidth(sw) - ScrollWidth(sw)) / 2;
	    }

	if (diff_h >= 0)
	{
		scrollBox.y = MajorLeadPad(sw);
		scrollBox.height = ScrollHeight(sw);
	}
	else
	{
		Dimension sb_min, avail, lp, tp;
		XmScrollBarWidget sb = (XmScrollBarWidget)
			(sw->composite.children[1]);

		sb_min = (2 * sb->primitive.highlight_thickness)
			+ (4 * sb->primitive.shadow_thickness)
			+ SLIDER_SIZE( sw);

		lp = MajorLeadPad(sw);
		tp = MajorTrailPad(sw);
		avail = XtHeight(sw) - lp - tp;

		if (avail < sb_min)
		{
			scrollBox.height = sb_min;
			scrollBox.y = (XtHeight(sw) - sb_min) / 2;
		}
		else
		{
			scrollBox.height = avail;
			scrollBox.y = lp;
		}
	}

	if (instigator != sw->composite.children[0])
	    XmeConfigureObject(sw->composite.children[0],
			       titleBox.x, 0,
			       (sw->composite.children[0])->core.width,
			       (sw->composite.children[0])->core.height,
			       (sw->composite.children[0])->core.border_width);
	else {
	    sw->composite.children[0]->core.x = titleBox.x ;
	    sw->composite.children[0]->core.y = 0 ;
	}

	if (instigator != sw->composite.children[1])
	    XmeConfigureObject(sw->composite.children[1],
			       scrollBox.x, scrollBox.y,
			       scrollBox.width, scrollBox.height, 0);
	else {
	    sw->composite.children[1]->core.x = scrollBox.x ;
	    sw->composite.children[1]->core.y = scrollBox.y ;
	    sw->composite.children[1]->core.width = scrollBox.width ;
	    sw->composite.children[1]->core.height = scrollBox.height ;
	    sw->composite.children[1]->core.border_width = 0 ;
	}

	SetScrollBarData(sw);

	LayoutVerticalLabels(sw, &scrollBox, &labelBox, instigator);
}




/************************************************************************/
static void
GetValueString(
        XmScaleWidget sw,
        int value,
        String buffer)
{
    int i;
    int  diff, dec_point_size;
    struct lconv *loc_values;

    if (sw->scale.decimal_points > 0) {
      /* Add one to decimal points to get leading zero, since
	 only US sometimes skips this zero, not other countries */
      sprintf (buffer,"%.*d", sw->scale.decimal_points+1, value);

      diff = strlen(buffer) - sw->scale.decimal_points;
      loc_values = localeconv();
      dec_point_size = strlen(loc_values->decimal_point);

      for (i = strlen(buffer); i >= diff; i--)
	buffer[i+dec_point_size] = buffer[i];

      for (i=0; i<dec_point_size; i++)
	buffer[diff+i] = loc_values->decimal_point[i];

    } else
      sprintf (buffer,"%d", value);
}



/************************************************************************
 *
 *  ShowValue
 *     Display or erase the slider value.
 *
 ************************************************************************/
static void
ShowValue(
        XmScaleWidget sw)
{
#if USE_XFT
    Dimension x, y, width, height;
#else
    int x, y, width, height;
#endif
    XCharStruct width_return;
    char buffer[256];
    int direction, descent;
    XmScrollBarWidget scrollbar;
    Region value_region = sw->scale.value_region;
    XRectangle value_rect;
    XmString tmp_str;

    if (!XtIsRealized((Widget)sw)) return;

    x = sw->scale.show_value_x;
    y = sw->scale.show_value_y;
    width = sw->scale.show_value_width;
    height = sw->scale.show_value_height;

    if (!sw->scale.show_value) { /* turn off the value display */

	if (width) { /* We were displaying, so we must clear it */

	    XClearArea (XtDisplay (sw), XtWindow (sw), x, y, width,
			height, FALSE);
	    value_rect.x = x;
	    value_rect.y = y;
	    value_rect.width = width;
	    value_rect.height = height;
	    XIntersectRegion(null_region, value_region, value_region);
	    XUnionRectWithRegion(&value_rect, value_region, value_region);
	    XmeRedisplayGadgets( (Widget) sw, NULL, value_region);
	}
	sw->scale.show_value_width = 0;
	return;
    }

    /*
     * Time for the real work.
     */

    if (width)	{
	/* Clear the old one */
	value_rect.x = x;
	value_rect.y = y;
	value_rect.width = width;
	value_rect.height = height;
	XIntersectRegion(null_region, value_region, value_region);
	XClearArea(XtDisplay(sw), XtWindow(sw), x, y, width, height, FALSE);
	XUnionRectWithRegion(&value_rect, value_region, value_region);
	XmeRedisplayGadgets( (Widget) sw, NULL, value_region);
    }

    /*  Get a string representation of the new value  */

    GetValueString(sw, sw->scale.value, buffer);

    /*  Calculate the x, y, width, and height of the string to display  */

#if USE_XFT
    XmStringExtent(sw->scale.font_list, tmp_str = XmStringCreateLocalized(buffer),
		  &width, &height);
    XmStringFree(tmp_str);
    sw->scale.show_value_width = width;
    sw->scale.show_value_height = height;
#else
    XTextExtents (sw->scale.font_struct, buffer, strlen(buffer),
		  &direction, &height, &descent, &width_return);
    width = width_return.rbearing - width_return.lbearing;
    sw->scale.show_value_width = width;
    sw->scale.show_value_height = height + descent;
#endif

    scrollbar = (XmScrollBarWidget) sw->composite.children[1];

    if (sw->scale.orientation == XmHORIZONTAL) {
#if USE_XFT
	x = scrollbar->core.x
	    + scrollbar->scrollBar.slider_x
	    + ((sw->scale.sliding_mode)?
	    scrollbar->scrollBar.slider_width: 0)
	    - (width - SLIDER_SIZE( sw)) / 2;
	if (sw->scale.show_value == XmNEAR_BORDER)
		/*tmp: should store the max */
		y = scrollbar->core.y - MaxLabelHeight(sw) - height - 3;
	else /* NEAR_SLIDER or NONE */
	    y = scrollbar->core.y - height - 3;
#else
	x = scrollbar->core.x
	    + scrollbar->scrollBar.slider_x
	    + ((sw->scale.sliding_mode)?
		scrollbar->scrollBar.slider_width: 0)
		- (width_return.rbearing - SLIDER_SIZE( sw)) / 2;
	if (sw->scale.show_value == XmNEAR_BORDER)
	    /*tmp: should store the max */
	    y = scrollbar->core.y - MaxLabelHeight(sw) - 3;
	else /* NEAR_SLIDER or NONE */
	    y = scrollbar->core.y - 3;
#endif
    } else {
	if (sw->scale.show_value == XmNEAR_BORDER) {
	    if (LayoutIsRtoLM(sw))
		x = scrollbar->core.x + scrollbar->core.width +
		    MaxLabelWidth(sw);
	    else
#if USE_XFT
		x = scrollbar->core.x - MaxLabelWidth(sw) -
		    sw->scale.show_value_width - SCALE_VALUE_MARGIN;
#else
		x = scrollbar->core.x - MaxLabelWidth(sw) -
		    width_return.rbearing - SCALE_VALUE_MARGIN;
#endif
	} else { /* NEAR_SLIDER or NONE */
	    if (LayoutIsRtoLM(sw))
		x = scrollbar->core.x + scrollbar->core.width;
	    else
#if USE_XFT
		x = scrollbar->core.x - sw->scale.show_value_width - SCALE_VALUE_MARGIN;
#else
		x = scrollbar->core.x - width_return.rbearing - SCALE_VALUE_MARGIN;
#endif
	}
#if USE_XFT
	y = scrollbar->core.y + scrollbar->scrollBar.slider_y
	    + SLIDER_SIZE(sw)/2 - height/2;
#else
	y = scrollbar->core.y + scrollbar->scrollBar.slider_y
	    + SLIDER_SIZE(sw) + ((height - SLIDER_SIZE( sw)) / 2) - 3;
#endif
    }

#if USE_XFT
    sw->scale.show_value_x = x;
    sw->scale.show_value_y = y + 1;
#else
    sw->scale.show_value_x = x + width_return.lbearing;
    sw->scale.show_value_y = y - height + 1;
#endif

    /*  Display the string  */
    XSetClipMask(XtDisplay(sw), sw->scale.foreground_GC, None);
#if USE_XFT
    XmStringDraw(XtDisplay(sw), XtWindow(sw), sw->scale.font_list,
                    tmp_str = XmStringCreateLocalized(buffer),
		    sw->scale.foreground_GC,
		    x, y, width, XmALIGNMENT_CENTER,
		    sw->manager.string_direction,
		    NULL);
    XmStringFree(tmp_str);
#else
    XDrawImageString (XtDisplay(sw), XtWindow(sw),
		 sw->scale.foreground_GC, x, y, buffer, strlen(buffer));
#endif
}


/*********************************************************************
 *
 * CalcScrollBarData
 * Figure out the scale derived attributes of the scrollbar child.
 *
 *********************************************************************/
static void
CalcScrollBarData(
        XmScaleWidget sw,
        int *pvalue,
        int *pslider_size,
        int *pincrement,
        int *ppage )
{
    int slider_size, increment, page, value ;
    Dimension scrollbar_size;
    float sb_value, tmp;
    XmScrollBarWidget scrollbar =
	    (XmScrollBarWidget) sw->composite.children[1];
    int ht = scrollbar->primitive.highlight_thickness;
    int st = scrollbar->primitive.shadow_thickness;
    int size;

	/*  Adjust the slider size to take SLIDER_SIZE area.    */
	/*  Adjust value to be in the bounds of the scrollbar.  */

    if (scrollbar->scrollBar.orientation == XmHORIZONTAL)
	scrollbar_size = scrollbar->scrollBar.slider_area_width + 2 *(ht +st);
    else
	scrollbar_size = scrollbar->scrollBar.slider_area_height + 2 *(ht +st);

    size = scrollbar_size - 2 * (sw->scale.highlight_thickness
		 + sw->manager.shadow_thickness) ;

    /* prevent divide by zero error and integer rollover */
    if (size <= 0)
		scrollbar_size = 1;
	else
	    /* this looks suspicious to me, but it is bc to let it in */
		scrollbar_size -= 2 * (sw->scale.highlight_thickness
			+ sw->manager.shadow_thickness);

    slider_size = (SCROLLBAR_MAX / scrollbar_size) * SLIDER_SIZE( sw);

    /*
     * Now error check our arithmetic
     */
    if (slider_size < 0)
	slider_size = SCROLLBAR_MAX;
    else if (slider_size < 1)
	slider_size = 1;
    else if (slider_size > SCROLLBAR_MAX)
	slider_size = SCROLLBAR_MAX;

    sb_value = (float) (sw->scale.value - sw->scale.minimum) /
		(float) (sw->scale.maximum - sw->scale.minimum);
    sb_value = sb_value * (float) (SCROLLBAR_MAX - slider_size);

    value = (int) sb_value;

    ASSIGN_MIN(value, SCROLLBAR_MAX - slider_size);
    ASSIGN_MAX(value, 0);

    /* Set up the increment processing correctly */

    tmp = (float) SCROLLBAR_MAX - (float) slider_size;

    increment = (int) ((tmp /
		(float) (sw->scale.maximum - sw->scale.minimum)) + 0.5);
    ASSIGN_MAX(increment, 1);

    page = sw->scale.scale_multiple * (increment);
    ASSIGN_MAX(page, 1);

    *pvalue = value ;
    *pslider_size = slider_size ;
    *pincrement = increment ;
    *ppage = page ;
}


/*********************************************************************
 *
 * SetScrollBarData
 *   Call CalcScrollBarData and do a setvalues on the SB.
 *
 *********************************************************************/
static void
SetScrollBarData(
        XmScaleWidget sw)
{
    int slider_size, increment, page, value ;
    Arg args[5] ;
    Cardinal n = 0 ;
    XmScrollBarWidget scrollbar =
	    (XmScrollBarWidget) sw->composite.children[1];

    CalcScrollBarData(sw, &value, &slider_size, &increment, &page);

    SET(XmNvalue, value);
    SET(XmNsliderSize, slider_size);
    SET(XmNincrement, increment);
    SET(XmNpageIncrement, page);

    XtSetValues ((Widget)scrollbar, args, n);
}




/*********************************************************************
 *
 *  ValueChanged
 *	Callback procedure invoked from the scrollbars value being changed.
 *
 *********************************************************************/
static void
ValueChanged(
        Widget wid,
        XtPointer closure,	/* unused */
        XtPointer call_data )
{
    XmScaleWidget sw = (XmScaleWidget) XtParent (wid);
    XmScrollBarCallbackStruct * scroll_callback =
	(XmScrollBarCallbackStruct *) call_data;
    XmScaleCallbackStruct scale_callback;
    float sb_value;
    XmScrollBarWidget sb = (XmScrollBarWidget)(sw->composite.children[1]);

    sb_value = (float) scroll_callback->value
	     / (float) (SCROLLBAR_MAX - sb->scrollBar.slider_size);
    sb_value = (sb_value * (float) (sw->scale.maximum - sw->scale.minimum))
	       + (float) sw->scale.minimum;

    /* Set up the round off correctly */
    if (sb_value < 0.0) sb_value -= 0.5;
    else if (sb_value > 0.0) sb_value += 0.5;

    sw->scale.value = (int) sb_value;

    ShowValue (sw);

    scale_callback.event = scroll_callback->event;
    scale_callback.reason = scroll_callback->reason;
    scale_callback.value = sw->scale.value;

    if (scale_callback.reason == XmCR_DRAG)
	XtCallCallbackList((Widget) sw, sw->scale.drag_callback,
			   &scale_callback);
    else /* value changed and to_top and to_bottom */
	{
	    scale_callback.reason = XmCR_VALUE_CHANGED;
	    XtCallCallbackList((Widget) sw,
			       sw->scale.value_changed_callback,
			       &scale_callback);
	}
}



/************************************************************************
 *
 * StartDrag:
 * This routine is performed by the initiator when a drag starts
 * (in this case, when mouse button 2 was pressed).  It starts
 * the drag processing, and establishes a drag context
 *
 ************************************************************************/
static void
StartDrag (Widget  w,
	   XtPointer data,	/* unused */
	   XEvent  *event,
	   Boolean *cont)	/* unused */
{
   Widget drag_icon;
   Arg             args[10];
   Cardinal        n;
   XmScaleWidget sw = (XmScaleWidget) w ;
   XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

   /* CDE - allow user to not drag labels and label subclasses
      also,  disable drag if enable_btn1_transfer is set to
      BUTTON2_ADJUST and the trigger was button2 */
   if (! dpy -> display.enable_unselectable_drag ||
       (dpy -> display.enable_btn1_transfer == XmBUTTON2_ADJUST &&
	event && event -> xany.type == ButtonPress &&
	event -> xbutton.button == 2)) return;

   /* first check that the click is OK: button 2 and in the value label */
   if ((!sw->scale.show_value) ||
       (event->xbutton.button != Button2) ||
       ((event->xbutton.x < sw->scale.show_value_x) ||
	(event->xbutton.y < sw->scale.show_value_y) ||
	(event->xbutton.x > sw->scale.show_value_x +
	 sw->scale.show_value_width) ||
	(event->xbutton.y > sw->scale.show_value_y +
	 sw->scale.show_value_height))) return ;

   drag_icon = XmeGetTextualDragIcon(w);

   n = 0;
   XtSetArg(args[n], XmNcursorBackground, sw->core.background_pixel);  n++;
   XtSetArg(args[n], XmNcursorForeground, sw->manager.foreground);  n++;
   XtSetArg(args[n], XmNsourceCursorIcon, drag_icon);  n++;
   XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++ ;
   (void) XmeDragSource(w, NULL, event, args, n);
}


/************************************************************************
 *
 * DragConvertProc:
 * This routine returns the value of the scale,
 * converted into compound text.
 *
 ************************************************************************/
static void
DragConvertCallback (Widget w,
		     XtPointer client_data, /* unused */
		     XmConvertCallbackStruct *cs)
{
   enum { XmACOMPOUND_TEXT, XmATARGETS, XmA_MOTIF_EXPORT_TARGETS,
	  XmA_MOTIF_CLIPBOARD_TARGETS, XmAUTF8_STRING, NUM_ATOMS };
   static char *atom_names[] = {
     XmSCOMPOUND_TEXT, XmSTARGETS, XmS_MOTIF_EXPORT_TARGETS,
     XmS_MOTIF_CLIPBOARD_TARGETS, XmSUTF8_STRING };

   char	         tmpstring[100];
   char         *strlist;
   char         *passtext;
   XmScaleWidget sw = (XmScaleWidget) w;
   Atom		 atoms[XtNumber(atom_names)];
   XTextProperty tp;
   Atom		 type = None ;
   XtPointer	 value = NULL ;
   unsigned long size = 0 ;
   int		 format = 8 ;

   assert(XtNumber(atom_names) == NUM_ATOMS);
   XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

   /* Begin fixing the bug OSF 4846 */
   /* get the value of the scale and convert it to compound text */
   GetValueString(sw, sw->scale.value, tmpstring);

   if (cs -> target == atoms[XmATARGETS] ||
       cs -> target == atoms[XmA_MOTIF_EXPORT_TARGETS] ||
       cs -> target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS]) {
     int count = 0;
     Atom *targs;

     if (cs -> target == atoms[XmATARGETS])
       targs = XmeStandardTargets(w, 3, &count);
     else
       targs = (Atom *) XtMalloc(sizeof(Atom) * 3);

     value = (XtPointer) targs;
     targs[count] = XA_STRING; count++;
     targs[count] = atoms[XmACOMPOUND_TEXT]; count++;
     targs[count] = atoms[XmAUTF8_STRING]; count++;
     size = count;
     type = XA_ATOM;
     format = 32;
   }

   if (cs -> target == XA_STRING ||
       cs -> target == atoms[XmAUTF8_STRING]) {
     /* handle plain STRING and UTF8_STRING first */
     type = cs -> target;
     value = (XtPointer) XtNewString(tmpstring);
     size = strlen((char*) value);
     format = 8;
   }

   /* this routine processes only compound text now */
   if (cs -> target == atoms[XmACOMPOUND_TEXT]) {
     strlist = tmpstring;
     tp.value = NULL;
     XmbTextListToTextProperty(XtDisplay(w), &strlist, 1,
			       XCompoundTextStyle, &tp);
     passtext = XtNewString((char*)tp.value);
     XtFree((char*)tp.value);
     /* End fixing the bug OSF 4846 */

     /* format the value for transfer.  convert the value from
      * compound string to compound text for the transfer */
     type = atoms[XmACOMPOUND_TEXT];
     value = (XtPointer) passtext;
     size = strlen(passtext);
     format = 8;
   }

   _XmConvertComplete(w, value, size, format, type, cs);
}



/************************************************************************
 *
 *  WidgetNavigable class method
 *
 ************************************************************************/
static XmNavigability
WidgetNavigable(
        Widget wid)
{
  if(    XtIsSensitive(wid)
     &&  ((XmManagerWidget) wid)->manager.traversal_on    )
    {
      XmNavigationType nav_type
	                   = ((XmManagerWidget) wid)->manager.navigation_type ;

      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
         ||  (    (nav_type == XmTAB_GROUP)
	      &&  !_XmShellIsExclusive( wid))    )
	{
	  return XmDESCENDANTS_TAB_NAVIGABLE ;
	}
    }
  return XmNOT_NAVIGABLE ;
}


/************************************************************************
 *
 *	External API functions.
 *
 ************************************************************************/


/************************************************************************
 *
 *  XmScaleSetValue
 *
 ************************************************************************/
void
XmScaleSetValue(
        Widget w,
        int value )
{
    XmScaleWidget sw = (XmScaleWidget) w;
    XtAppContext app = XtWidgetToApplicationContext(w);

    _XmAppLock(app);

    if (value < sw->scale.minimum) {
	XmeWarning( (Widget) sw, MESSAGE2);
	_XmAppUnlock(app);
	return ;
    }

    if (value > sw->scale.maximum) {
	XmeWarning( (Widget) sw, MESSAGE3);
	_XmAppUnlock(app);
	return ;
    }

    sw->scale.value = value ;
    SetScrollBarData(sw);
    ShowValue(sw);

    _XmAppUnlock(app);
}




/************************************************************************
 *
 *  XmScaleGetValue
 *
 ************************************************************************/
void
XmScaleGetValue(
        Widget w,
        int *value )
{
   XmScaleWidget sw = (XmScaleWidget) w;
   XtAppContext app = XtWidgetToApplicationContext(w);

   _XmAppLock(app);
   *value = sw->scale.value;
   _XmAppUnlock(app);
}

/************************************************************************
 *
 *  XmCreateScale
 *
 ************************************************************************/
Widget
XmCreateScale(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget(name, xmScaleWidgetClass,
			  parent, arglist, argcount));
}

Widget
XmVaCreateScale(
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
                         xmScaleWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedScale(
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
                         xmScaleWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

