/* $TOG: String.c /main/10 1997/04/16 10:00:15 dbl $ */
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
/*
 * HISTORY
 */
/************************************ WARNING **********************************
 *
 * ExmString is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way
 ************************************ WARNING *********************************/


/*******************************************************************************
 *
 * String.c - ExmString widget.  This widget renders a compound string.
 *            The ExmString widget demonstrates how to work with strings
 *            in Motif widgets.  Functionally, the widget is similar to
 *            XmLabel, except that XmLabel can render both pixmaps and
 *            compound strings.  The ExmString widget manipulates the
 *            resources and traits used by other Motif string-oriented
 *            widgets.  See the "OSF/Motif Widget Writer's Guide" for details.
 *
 ******************************************************************************/

/* Include appropriate header files. */
#include <Xm/XmP.h>   /* private header file for the XmPrimitive widget */
#include <Xm/XmosP.h>  /* header file for MB_CUR_MAX */
#include <Exm/ExmStringP.h>  /* private header file for the ExmString widget */
#include <Xm/RepType.h> /* header file for representation type facility */
#include <Xm/TraitP.h>  /* header file for installing traits */
#include <Xm/AccTextT.h> /* header file for AccessTextual trait */
#include <Xm/SpecRenderT.h> /* header file for XmLABEL_RENDER_TABLE */
#include <Xm/Screen.h>   /* header file for screen information */
#include <Xm/DrawP.h>   /* header file for Draw routines */


/* Declare all static functions. */
static void DefaultFont (
                        Widget w,
                        int offset,
                        XrmValue *value
                        );
static void GetValuesCompoundString(
                        Widget w,
                        int resource,
                        XtArgVal *value);
static void ClassInitialize(void);
static void ClassPartInitialize(
                        WidgetClass widgetclass);
static void Initialize(
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void Destroy (
                        Widget w );
static void Resize (
                        Widget w );
static void AlignmentDirection(
                        Widget w );
static Boolean SetValues (
                        Widget old_w,
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static XtGeometryResult QueryGeometry (
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply);
static void DrawVisual (
                        Widget w );
static void CalcVisualSize (
                        Widget w );
static void CreateGC (
                        Widget w );
static Boolean WidgetBaselines (
                        Widget w,
                        Dimension **baselines,
                        int *line_count);
static Boolean WidgetDisplayRect (
                        Widget       w,
                        XRectangle  *displayrect);
static void StringSetValue(
                        Widget w,
                        XtPointer s,
                        int format);
static XtPointer StringGetValue(
                        Widget w,
                        int format);
static int StringPreferredFormat(
                        Widget w);


/* Define constants here. */
static XmConst int  FIND_NATURAL_SIZE = 0;
#define UNSUPPORTED_FORMAT "Someone is trying to get or set a value for \n\
ExmNcompoundString; however, the specified format is undefined.\n"



/* Define the translations string for the ExmString widget.
   All six actions will be handled by the XmPrimitive widget. */
static char defaultTranslations[] =
"<EnterWindow>:		PrimitiveEnter()\n\
<LeaveWindow>:		PrimitiveLeave()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfHelp:		PrimitiveHelp()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()";

/* No actions array needed. */


/* Define the resources for the ExmString widget.  */
static XtResource resources[] =
{
   {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof (Boolean),
	XtOffsetOf( XmPrimitiveRec, primitive.traversal_on),
	XmRImmediate,
	(XtPointer) False /* override the default. */
    },
    {
	ExmNcompoundString,
	ExmCCompoundString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf( ExmStringRec,string.compound_string),
	XmRImmediate,
	(XtPointer) NULL
    },
    {
	XmNrenderTable,
	XmCRenderTable,
	XmRRenderTable,
	sizeof(XmRenderTable),
	XtOffsetOf( ExmStringRec,string.render_table),
	XtRCallProc,
	(XtPointer) DefaultFont
    },
    {
	XmNalignment,
	XmCAlignment,
	XmRAlignment,
	sizeof(unsigned char),
	XtOffsetOf( ExmStringRec,string.alignment),
	XmRImmediate,
	(XtPointer) XmALIGNMENT_CENTER
    },
   {
        XmNrecomputeSize,
        XmCRecomputeSize,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( ExmStringRec,string.recompute_size),
        XmRImmediate,
        (XtPointer) True
   },
};


/* Provide a synthetic resource for ExmNcompoundString. */
static XmSyntheticResource syn_resources[] =
{
   {
        ExmNcompoundString,
        sizeof(XmString),
        XtOffsetOf(ExmStringRec, string.compound_string),
        GetValuesCompoundString,
        NULL
   }
};


/* Here is the primitive class extension record. */
static XmPrimitiveClassExtRec primClassExtRec = {
    /* next_extension */             NULL,
    /* record_type */                NULLQUARK,
    /* version */                    XmPrimitiveClassExtVersion,
    /* record_size */                sizeof(XmPrimitiveClassExtRec),
    /* widget_baseline */            WidgetBaselines,
    /* widget_display_rect */        WidgetDisplayRect,
    /* widget_margins */             NULL,
};

externaldef (exmstringclassrec) ExmStringClassRec exmStringClassRec = {
  {
    /* superclass */                 (WidgetClass)&exmSimpleClassRec,
    /* class_name */                 "ExmString",
    /* widget_size */                sizeof(ExmStringRec),
    /* class_initialize */           ClassInitialize,
    /* class_part_initialize */      ClassPartInitialize,
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    NULL,
    /* num_actions */                0,
    /* resources */                  resources,
    /* num_resources */              XtNumber(resources),
    /* xrm_class */                  NULLQUARK,
    /* compress_motion */            TRUE,
    /* compress_exposure */          XtExposeCompressMaximal,
    /* compress_enterleave */        TRUE,
    /* visible_interest */           FALSE,
    /* destroy */                    Destroy,
    /* resize */                     Resize,
    /* expose */                     XtInheritExpose,
    /* set_values */                 SetValues,
    /* set_values_hook */            NULL,
    /* set_values_almost */          XtInheritSetValuesAlmost,
    /* get_values_hook */            NULL,
    /* accept_focus */               NULL,
    /* version */                    XtVersion,
    /* callback_private */           NULL,
    /* tm_table */                   defaultTranslations,
    /* query_geometry */             QueryGeometry,
    /* display_accelerator */        NULL,
    /* extension */                  NULL,
  },
  { /* XmPrimitive */
    /* border_highlight */           XmInheritBorderHighlight,
    /* border_unhighlight */         XmInheritBorderUnhighlight,
    /* translations */               XtInheritTranslations,
    /* arm_and_activate */           NULL,
    /* syn_resources */              syn_resources,
    /* num_syn_resources */          XtNumber(syn_resources),
    /* extension */                  (XtPointer)&primClassExtRec,
  },
  { /* ExmSimple */
    /* draw_visual */                DrawVisual,
    /* draw_shadow */                ExmInheritDrawShadow,
    /* create_text_gc */             CreateGC,
    /* destroy_text_gc */            ExmInheritDestroyGC,
    /* select_text_gc */             ExmInheritSelectGC,
    /* calc_visual_size */           CalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */                ExmInheritReconfigure,
    /* extension */                  NULL,
  },
  { /* ExmString */
    /* default_render_table_type */  XmLABEL_RENDER_TABLE,
    /* extension */                  NULL,
  }
};

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro for OS-independent global definitions. */
externaldef( exmstringwidgetclass) WidgetClass exmStringWidgetClass =
                                  (WidgetClass) &exmStringClassRec;


/* Define static representation type variables here. */
static XmRepTypeId alignmentId;


/* Define trait structure variables here. */
/* Since ExmString displays a primary parcel of text, the ExmString
   widget must install the XmQTaccessTextual trait.  The following
   declaration is of a trait structure variable named StringATT. */
static XmConst XmAccessTextualTraitRec StringATT = {
  0,  				/* version */
  StringGetValue,
  StringSetValue,
  StringPreferredFormat,
};

/*******************************************************************************
 *
 *  DefaultFont:
 *      Called by the Intrinsics to establish a default value for the
 *      XmNrenderTable resource.
 *
 ******************************************************************************/
static void
DefaultFont (
        Widget w,
        int offset,
        XrmValue *value
            )
{
 ExmStringWidgetClass wc = (ExmStringWidgetClass)XtClass(w);
 static XmRenderTable  f1;

 /* Find the default render table associated with the default
    render table type. */
   f1 = XmeGetDefaultRenderTable (w,
                                  wc->string_class.default_render_table_type);

   value->addr = (XtPointer)&f1;
   value->size = sizeof(f1);
}


/************************************************************************
 *
 * GetValuesCompoundString
 *	This is a synthetic resource function called by Motif when
 *      an application calls XtGetValues to access the value of
 *      ExmNcompoundString.
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
GetValuesCompoundString(
        Widget w,
        int resource,	/* unused */
        XtArgVal *value)
{
 ExmStringWidget sw = (ExmStringWidget) w;
 XmString  string;

 /* All Motif widgets are responsible for making a copy of an XmString
    resource whenever an application accesses the resource through a call
    to XtGetValues. */
   string = XmStringCopy(sw->string.compound_string);

   *value = (XtArgVal) string;
}


/*******************************************************************************
 *
 *  ClassInitialize
 *      Called by the Intrinsics the first time a widget of this class is
 *      instantiated.
 *
 ******************************************************************************/
static void
ClassInitialize(void)
{
 /* Use existing Motif representation types for XmNalignment. */
   alignmentId = XmRepTypeGetId (XmRAlignment);
   if (alignmentId == XmREP_TYPE_INVALID)
     XmeWarning(NULL, "The value of XmNalignment is illegal.");
}

/************************************************************************
 *
 *  ClassPartInitialize
 *      Called by the Intrinsics when this widget or a subclass of this
 *      widget is instantiated.
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass widgetclass )
{
 /* Install the XmQTaccessTextual trait on the ExmString class and all
    its subclasses. */
   XmeTraitSet((XtPointer) widgetclass, XmQTaccessTextual,
               (XtPointer) &StringATT);
}


/*******************************************************************************
 *
 *  Initialize
 *      Called when this widget is instantiated.
 *
 ******************************************************************************/
static void
Initialize (
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
           )
{
  ExmStringWidgetClass wc = (ExmStringWidgetClass)XtClass(new_w);
  ExmStringWidget nw = (ExmStringWidget)new_w;
  unsigned char stringDirection;
  Arg dirArgs[1];

  /* Copy the compound string and render table. */
  nw->string.compound_string = XmStringCopy(nw->string.compound_string);
  nw->string.render_table = XmRenderTableCopy(nw->string.render_table,
					      NULL, 0);

  /* Validate  XmNalignment */
  if (!XmRepTypeValidValue (alignmentId, nw->string.alignment, (Widget)nw))
    nw->string.alignment = XmALIGNMENT_CENTER;

  /* If the XmNstringDirection resource is set to XmSTRING_DIRECTION_DEFAULT,
     then we need to figure out what the default string direction is.
     If the parent is a manager, then we'll set the string direction of
     ExmString to the string direction of the manager.  */

  AlignmentDirection((Widget)nw);

  if (wc->simple_class.calc_visual_size)
    (*(wc->simple_class.calc_visual_size))(new_w);

  if (wc->simple_class.reconfigure)
    (*(wc->simple_class.reconfigure))(exmStringWidgetClass, new_w, NULL);
}



/*******************************************************************************
 *
 *  Destroy
 *      Called by the Intrinsics whenever this widget is deallocated.
 *
 ******************************************************************************/
static void
Destroy (
       Widget w
        )
{
 ExmStringWidget sw = (ExmStringWidget)w;

 /* Free the memory used to hold the ExmNcompound_string and
    XmNrenderTable resource values. */
   if (sw->string.compound_string != NULL)
     XmStringFree (sw->string.compound_string);

   if (sw->string.render_table != NULL)
     XmRenderTableFree (sw->string.render_table);
}



/*******************************************************************************
 *
 * Resize
 *
 ******************************************************************************/
static void
Resize (
        Widget w
       )
{
 ExmStringWidget sw = (ExmStringWidget)w;
 Dimension  mw, mh;
 Dimension  window_decoration_thickness;

 /* Configure internal geometry using current size */
   if ((sw->simple.visual.width == FIND_NATURAL_SIZE) ||
       (sw->simple.visual.height == FIND_NATURAL_SIZE))
     return;

   window_decoration_thickness = sw->primitive.highlight_thickness  +
                                 sw->primitive.shadow_thickness;

 /* Determine where the string's bounding box should start. */

 /* First determine the X coordinate of the bounding box. */
  mw = window_decoration_thickness + sw->simple.margin_width;

  switch (sw->string.text_starts_here)
    {
        case ExmSTART_STRING_LEFT_SIDE :
          sw->simple.visual.x = mw;
          break;
        case ExmSTART_STRING_RIGHT_SIDE :
          sw->simple.visual.x = sw->core.width - (mw + sw->simple.visual.width +
                                                  window_decoration_thickness);
          break;
        case ExmCENTER_STRING :
          sw->simple.visual.x = ((int)(sw->core.width - sw->simple.visual.width))/2;
          break;
    };


 /* Now do the same for the vertical dimension. */
   mh = window_decoration_thickness + sw->simple.margin_height;

   /* If the widget has enough vertical space to display all the lines in
      the string, then center the string. */
   if ((int)sw->core.height >
           (int)((2 * window_decoration_thickness) + sw->simple.visual.height)) {
     sw->simple.visual.y = ((int)(sw->core.height - sw->simple.visual.height))/2;
   }
   else if ((int)sw->core.height > ((int)(2 * window_decoration_thickness))) {
    /* Space is very tight.  We will eliminate the top margin altogether and
       start the first line of the compound string snug against the bottom of
       the top edge of the window decorations. */
     sw->simple.visual.y = window_decoration_thickness;
   }
  else
    /* Space is so tight that we do not have enough space to display even
       one pixel of the visual. */
     sw->simple.visual.y = window_decoration_thickness;

}

/*******************************************************************************
 *
 * AlignmentDirection:
 *     Called by Initialize and by SetValues.
 *
 ******************************************************************************/
static void
AlignmentDirection(
        Widget w
                     )
{
 ExmStringWidget sw = (ExmStringWidget)w;

 /* This method determines where the text starts.  The text could
      * be centered.
      * start at the left side of the widget.
      * start at the right side of the widget.
 */
   if (sw->string.alignment == XmALIGNMENT_CENTER)
   /* The string will be centered. */
     sw->string.text_starts_here = ExmCENTER_STRING;

   else if (
            (XmDirectionMatch(sw->primitive.layout_direction, XmLEFT_TO_RIGHT) &&
	     sw->string.alignment == XmALIGNMENT_BEGINNING)
                                ||
            (XmDirectionMatch(sw->primitive.layout_direction, XmRIGHT_TO_LEFT) &&
             sw->string.alignment == XmALIGNMENT_END)
           )
   /* The string will start at the left side of the widget. */
     sw->string.text_starts_here = ExmSTART_STRING_LEFT_SIDE;

   else if (
            (XmDirectionMatch(sw->primitive.layout_direction, XmLEFT_TO_RIGHT) &&
             sw->string.alignment == XmALIGNMENT_END)
                                ||
            (XmDirectionMatch(sw->primitive.layout_direction, XmRIGHT_TO_LEFT) &&
             sw->string.alignment == XmALIGNMENT_BEGINNING)
           )
   /* The string will start at the right side of the widget. */
     sw->string.text_starts_here = ExmSTART_STRING_RIGHT_SIDE;
}



/*******************************************************************************
 *
 *  SetValues
 *
 ******************************************************************************/
static Boolean
SetValues (
        Widget old_w,
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
          )
{
  ExmStringWidgetClass wc = (ExmStringWidgetClass)XtClass(new_w);
  ExmStringWidget cw = (ExmStringWidget)old_w;
  ExmStringWidget nw = (ExmStringWidget)new_w;
  Boolean redisplayFlag = False;

  /* Copy ExmNcompoundString if its value has changed */
  if (nw->string.compound_string != cw->string.compound_string) {
    nw->string.compound_string = XmStringCopy(nw->string.compound_string);
    XmStringFree(cw->string.compound_string);
    cw->string.compound_string = (XmString) NULL;
    nw->simple.need_to_reconfigure = True;
  }

  /* Update XmNcompound_string whenever XmNrenderTable changes */
  if (nw->string.render_table != cw->string.render_table) {
    nw->string.render_table = XmRenderTableCopy(nw->string.render_table,
						NULL, 0);
    XmRenderTableFree (cw->string.render_table);
    cw->string.render_table = (XmRenderTable) NULL;
    nw->simple.need_to_reconfigure = True;
  }

  /* Validate any changes to the value of the XmNalignment resource.
     If the requested new value is not valid, then reset the value of the
     XmNalignment resource to the old value. */
  if (nw->string.alignment != cw->string.alignment) {
    if (!XmRepTypeValidValue(alignmentId, nw->string.alignment, (Widget)nw))
      nw->string.alignment = cw->string.alignment;
    else nw->simple.need_to_reconfigure = True;
  }

  /* If the XmNrecomputeSize resource used to be False but is now True, then
     we will have to set in motion the series of calls that will lead to
     an appropriate resize of the widget. */
  if ((nw->string.recompute_size == True) &&
      (cw->string.recompute_size == False)) {
    nw->simple.need_to_reconfigure = True;

    if (nw->core.width == cw->core.width)
      nw->core.width = FIND_NATURAL_SIZE;
    if (nw->core.height == cw->core.height)
      nw->core.height = FIND_NATURAL_SIZE;
  }

  /* If the Alignment has changed or the layout direction has changed,
     then the widget needs to determine a new starting position for
     the bounding text box. */
  if ( nw->string.alignment != cw->string.alignment ||
      nw->primitive.layout_direction != cw->primitive.layout_direction)
    AlignmentDirection((Widget)nw);

  /* Determine whether or not the widget needs to be reconfigured.  Just
     about any change to a resource will necessitate a reconfiguration.
     If the widget does need to be reconfigured, call Reconfigure. */
  if (nw->simple.need_to_reconfigure == True) {
    if (wc->simple_class.reconfigure)
      (*(wc->simple_class.reconfigure))(exmStringWidgetClass, new_w, old_w);

    redisplayFlag = True;
  }

  return (redisplayFlag);
}


/*******************************************************************************
 *
 *  QueryGeometry
 *     Called by the Intrinsics in response to a proposed changed in geometry.
 *
 ******************************************************************************/
static XtGeometryResult
QueryGeometry(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
  ExmStringWidgetClass swc = (ExmStringWidgetClass)XtClass(w);
  ExmStringWidget sw = (ExmStringWidget) w ;

  if (!XtIsRealized(w)) {
    /* Widget has not been realized yet. */
    reply->width  = XtWidth(w);   /* might be 0 */
    reply->height = XtHeight(w);  /* might be 0 */
  } else {
    if (sw->string.recompute_size) {
      /* The user will allow the ExmString widget to change size, so
	 let's call CalcWidgetSize to calculate the widget's
	 preferred size. */
      int save_w, save_h;

      save_w = XtWidth(w);
      save_h = XtHeight(w);
      if (swc->simple_class.calc_widget_size)
	(*(swc->simple_class.calc_widget_size))((Widget)sw);
      sw -> simple.pref_width = XtWidth(w);
      sw -> simple.pref_height = XtHeight(w);
      XtWidth(w) = save_w;
      XtHeight(w) = save_h;
    }
    reply->width  = sw -> simple.pref_width;
    reply->height = sw -> simple.pref_height;
  }

  /* Return our preferred size */
  return XmeReplyToQueryGeometry(w, request, reply);
}

/*******************************************************************************
 *
 *  DrawVisual
 *
 ******************************************************************************/
static void
DrawVisual (
        Widget w
           )
{
 ExmStringWidgetClass wc = (ExmStringWidgetClass)XtClass(w);
 ExmStringWidget sw = (ExmStringWidget)w;

 /* If the compound string is not NULL and if there is enough space in the
    widget to draw at least a little portion of the compound string, then
    render the string with XmStringDraw. */
   if (sw->string.compound_string &&
      (sw->simple.visual.width != 0) &&
      (sw->simple.visual.height != 0)) {
     XmStringDraw (XtDisplay(sw), XtWindow(sw),
                   sw->string.render_table,
                   sw->string.compound_string,
		   wc->simple_class.select_gc(w),
		   sw->simple.visual.x, sw->simple.visual.y,
		   sw->simple.visual.width, sw->string.alignment,
                   sw->primitive.layout_direction, NULL);
     XmeClearBorder(XtDisplay(sw), XtWindow(sw),
                    (int)0, (int)0,
                    (Dimension)sw->core.width, (Dimension)sw->core.height,
                    (Dimension)(sw->primitive.highlight_thickness +
                                sw->primitive.shadow_thickness)
                   );
   }
}



/*******************************************************************************
 *
 *  CreateGC:
 *      Called by the Initialize method of the base class (ExmSimple).
 *
 ******************************************************************************/
static void
CreateGC (
        Widget w
         )
{
 ExmStringWidget sw = (ExmStringWidget)w;
 XGCValues    values;
 XtGCMask     valueMask;
 XFontStruct *fs = (XFontStruct *) NULL;
 Arg          args[2];
 Pixmap       insensitiveStippleBitmap;

 /* This function creates two GC's: one to render a sensitive widget
    and the other to render an insensitive widget. */

 /* First, create the sensitive GC (named normal_gc). */
   valueMask = GCForeground | GCBackground | GCGraphicsExposures;
   values.foreground = sw->primitive.foreground;
   values.background = sw->core.background_pixel;
   values.graphics_exposures = False;

 /* In order to set the GCFont field of the GC, we must gather XFontStruct
    information out of the render table. This is only to get a reasonable
    initial value. XmStringDraw will pick the necessary fonts from the
    render table, so we will not need to update the GC when the render
    table changes. */
   if (XmeRenderTableGetDefaultFont(sw->string.render_table, &fs)) {
     values.font = fs->fid;
     valueMask |= GCFont;
   }

   sw->simple.normal_gc = XtGetGC ((Widget)sw, valueMask, &values);

 /* Next, create the insensitive GC.  This GC will share the same
    foreground, background, font, and graphics exposures as the sensitive
    GC, but will hold a different fill style and stipple pattern. */
   valueMask |= GCFillStyle | GCStipple;
   values.fill_style = FillStippled;

/* Gather the Motif-appropriate insensitive stipple bitmap. */
   XtSetArg(args[0], XmNinsensitiveStippleBitmap, &insensitiveStippleBitmap);
   XtGetValues(XmGetXmScreen(XtScreen(w)), args, 1);
   values.stipple = insensitiveStippleBitmap;

   sw->simple.insensitive_gc = XtGetGC((Widget) sw, valueMask, &values);
}



/*******************************************************************************
 *
 * CalcVisualSize
 *
 ******************************************************************************/
static void
CalcVisualSize (
        Widget w
               )
{
 ExmStringWidget sw = (ExmStringWidget)w;

 /* Ideally, how much space would the compound string require? */
   if (sw->string.compound_string && !XmStringEmpty(sw->string.compound_string))
     XmStringExtent (sw->string.render_table, sw->string.compound_string,
                     &(sw->simple.visual.width), &(sw->simple.visual.height));
   else {
 /* The string is empty, so it requires no space. */
     sw->simple.visual.width = 0;
     sw->simple.visual.height = 0;
   }
}


/*******************************************************************************
 *
 * WidgetBaselines:
 *      Called by manager widgets needing to align text-based visuals.
 *      This method can also be called directly by an application through
 *      the XmWidgetGetBaselines function.
 *
 ******************************************************************************/
static Boolean
WidgetBaselines(
        Widget w,
        Dimension **baselines,
        int *line_count)
{
 XmStringContext context = NULL;
 XmString        current_string;

 XmString string1 = NULL, string2;
 char *text1, *text2;
 unsigned int len;
 XmStringComponentType type;
 XmRenderTable RenderTable;
 Dimension *base_array;
 Dimension Offset;
 int index;
 ExmStringWidget sw = (ExmStringWidget)w;

 /* This function returns True to indicate that the widget is displaying
    text, or False to indicate that the widget is not displaying text.
    If the widget is displaying text, this function calculates the baseline
    of each displayed line of text. */

   index = 0;
   RenderTable = sw->string.render_table;

 /* Initialize the string context.  If the XmStringInitContext call returns
    False, then no string context could be found which suggests that no
    text is being displayed in the widget. */
   current_string = sw->string.compound_string;
   if (!XmStringInitContext (&context, current_string))
     return (False);

 /* Determine how many lines are in the displayed string. */
   *line_count = XmStringLineCount(current_string);

    base_array = (Dimension *)XtMalloc((sizeof(Dimension) * (*line_count)));
/*    Offset = ((ExmStringWidget) w)->string.visual.y;  */
    Offset = sw->simple.visual.y;

 /* Go through the compound string, segment by segment. */
   while ((type = XmStringGetNextTriple(context, &len, (XtPointer *)&text1)) != XmSTRING_COMPONENT_END) {
      if (string1)
          XmStringFree(string1);
      string1 = XmStringCreateLocalized(text1);
      XtFree(text1);

      if (type == XmSTRING_COMPONENT_SEPARATOR) {
        while (XmStringPeekNextTriple(context) == XmSTRING_COMPONENT_SEPARATOR) {
           XmStringGetNextTriple(context, &len, (XtPointer *)&text1);
           base_array[index++] = Offset + XmStringBaseline(RenderTable, string1);
           Offset += XmStringHeight(RenderTable, string1);
        }
      } else if ((type = XmStringGetNextTriple(context, &len, (XtPointer *)&text2))) {
        if (type == XmSTRING_COMPONENT_SEPARATOR) {
            string2 = XmStringCreateLocalized(text2);
            string1 = XmStringConcat(string1, string2);
            base_array[index++] = Offset + XmStringBaseline (RenderTable, string1);
            Offset += XmStringHeight (RenderTable, string1);
        } else {
          string2 = XmStringCreateLocalized(text2);
          string1 = XmStringConcat(string1, string2);
        }

        XtFree(text2);
        XmStringFree(string2);
      } else break;
    } /* end of outer while loop */

    base_array[index++] = Offset + XmStringBaseline (RenderTable, string1);

    XmStringFree(string1);

    *baselines = base_array;

    XmStringFreeContext(context);

    return (True);
}


/*******************************************************************************
 *
 *  WidgetDisplayRect:
 *      Called by several Motif managers to determine how to align the visuals
 *      drawn by primitives.  In addition, an application can access this
 *      method by calling XmWidgetGetDisplayRect.
 *
 ******************************************************************************/
static Boolean
WidgetDisplayRect(
        Widget       w,
        XRectangle  *displayrect
                 )
{
 ExmStringWidget  my_widget = (ExmStringWidget) w;

   if ((my_widget->simple.visual.width > 0 &&
        my_widget->simple.visual.height > 0)) {
     displayrect->x =       my_widget->simple.visual.x;
     displayrect->y =       my_widget->simple.visual.y;
     displayrect->width =   my_widget->simple.visual.width;
     displayrect->height =  my_widget->simple.visual.height;
     return True;  /* Yes, this widget contains something displayable. */
   }
   else  {
     return False; /* this widget is not currently displaying a visual. */
   }
}

/*******************************************************************************
 *
 *  Trait Methods:
 *      We now provide the code for the three methods defined by the
 *      XmQTaccessTextual trait.
 *
 ******************************************************************************/


/*******************************************************************************
 *
 *  StringGetValue: Called by another widget (generally a parent of
 *     an ExmString).  This trait method must return the currently
 *     displayed text string in the format that the caller requests.
 *
 ******************************************************************************/
static XtPointer
StringGetValue(
     Widget w,
     int format)
{
 ExmStringWidget  string_w = (ExmStringWidget) w;
 XmString     value;
 int          n;
 Arg          args[4];
 XmParseTable map[2];
 XmString     separator;
 XtPointer    result;
 XmTextType   output_type;

 /* Get the string that the ExmString widget is currently displaying. */
 value = XmStringCopy(string_w -> string.compound_string);

 /* The "value" variable know holds the string in XmString format.  */

   switch (format)   {
      case XmFORMAT_XmSTRING:
         /* If the caller wants "value" returned as a
            compound string, no conversion is necessary. */
           return (XtPointer) value;

      case XmFORMAT_MBYTE:
      case XmFORMAT_WCS:
         /* If the caller wants "value" returned as a multibyte string
            or as a wide character string, then we have to convert it. */
           if (format == XmFORMAT_MBYTE)
             output_type = XmMULTIBYTE_TEXT;
           else
             output_type = XmWIDECHAR_TEXT;

         /* Create a very simple parse table consisting of two
            parse mappings. */
           separator = XmStringSeparatorCreate();

           n = 0;
           XtSetArg( args[n], XmNpattern, "\n"); n++;
           XtSetArg( args[n], XmNpatternType, output_type ); n++;
           XtSetArg( args[n], XmNsubstitute, separator ); n++;
           XtSetArg( args[n], XmNincludeStatus, XmINSERT ); n++;
           map[0] = (XmParseMapping  *) XmParseMappingCreate( args, n );
           XmStringFree(separator);

           separator = XmStringComponentCreate(
                                 XmSTRING_COMPONENT_TAB, 0, NULL);
           n = 0;
           XtSetArg( args[n], XmNpattern, "\t"); n++;
           XtSetArg( args[n], XmNpatternType, output_type ); n++;
           XtSetArg( args[n], XmNsubstitute, separator ); n++;
           XtSetArg( args[n], XmNincludeStatus, XmINSERT ); n++;
           map[1] = (XmParseMapping  *) XmParseMappingCreate( args, n );
           XmStringFree(separator);

         /* Unparse "value" into either MULTIBYTE or WCS format. */
           result = XmStringUnparse (value,
                                     (XmStringTag) NULL,
                                     (XmTextType)NULL,
                                     (XmTextType)output_type,
                                     (XmParseTable)map,
                                     XtNumber(map),
                                     XmOUTPUT_ALL);

         /* Variable "result" now holds the text of the string in either
            MULTIBYTE or WCS format. */
           return (XtPointer)result;

      default:
           XmeWarning((Widget)w, UNSUPPORTED_FORMAT);
           return NULL;
   }


}

/*******************************************************************************
 *
 *  StringSetValue:
 *       Called by another widget to set the value of the ExmNcompoundString
 *       resource.
 *
 ******************************************************************************/
static void
StringSetValue(
     Widget w,
     XtPointer string,
     int format)
{
 Arg       args[1];
 XmString  temp;
 Boolean   freetemp;
 int       length;
 char     *str;
 wchar_t  *str2;

  /* The caller will pass a new value for ExmNcompoundString.  This new
     value will be passed in the "string" argument.  However, there is
     no guarantee that the input "string" will be passed in XmString format.
     If the input "string" is passed in WCS or MULTIBYTE format, then we
     must convert the "string" into XmString format.  Once the "string"
     is in XmString format, we can use it as the new value of
     ExmNcompoundString. */
    switch (format)    {
        case XmFORMAT_XmSTRING: temp = (XmString) string;
                                freetemp = False;
                                break;

        case XmFORMAT_WCS:      str2 = (wchar_t *) string;
	                      /* How long is str2? */
                                length = 0;
	                        while (str2[length] != 0)
	                           length++;
                              /* malloc enough space to hold str */
                                str = (char*) XtMalloc(MB_CUR_MAX * (length+1));
                                wcstombs(str, str2, MB_CUR_MAX * (length+1));
				XtFree((char *) string);
                                string = str;

        case XmFORMAT_MBYTE:    temp = XmStringCreateLocalized(string);
                                freetemp = True;
                                break;

        default:                XmeWarning((Widget)w, UNSUPPORTED_FORMAT);
                                return;
    }

    XtSetArg(args[0], ExmNcompoundString, temp);
    XtSetValues(w, args, 1);

    if (freetemp)
      XmStringFree(temp);
}

/*******************************************************************************
 *
 *  StringPreferredFormat
 *      Called by another widget to determine the preferred string format of
 *      this widget.  The possible returned formats are:
 *           * XmFORMAT_XmSTRING  (Motif compound string format)
 *           * XmFORMAT_MBYTE     (Multibyte format)
 *           * XmFORMAT_WCS       (Wide character string format)
 *
 ******************************************************************************/
static int
StringPreferredFormat(
     Widget w)
{
 /* Choose XmFORMAT_XmSTRING because the ExmString widget holds its displayed
    text in XmString format (as opposed to Multibyte or WCS format). */
   return(XmFORMAT_XmSTRING);
}


/*******************************************************************************
 *
 *  ExmCreateString
 *      Externally accessable function for creating a String widget
 *
 ******************************************************************************/
Widget
ExmCreateString (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                )
{
   return (XtCreateWidget(name,exmStringWidgetClass,parent,arglist,argCount));
}

