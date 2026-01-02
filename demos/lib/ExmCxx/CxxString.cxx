/* $XConsortium: CxxString.cxx /main/4 1995/07/17 11:45:39 drk $ */
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
/*
 * HISTORY
 */

/* WARNING: This widget is for educational purposes only.  Real
            applications should NOT instantiate this widget.
            OSF makes no guarantees on the stability of this widget.
*/

/*******************************************************************************
 *
 * String.c - ExmString widget.  This widget renders a compound string.
 *            The ExmString widget demonstrates how to
 *               * use strings in a widget.
 *               * use render tables in a widget.
 *               * create the XmQTaccessTextual trait.
 *            See the "OSF/Motif Widget Writer's Guide" for details.
 *
 ******************************************************************************/

/* Include appropriate header files. */
#include <Xm/XmP.h>   /* private header file for the XmPrimitive widget */
#include <Xm/XmosP.h> /* header file for multibyte functions */
#include "CxxString.h"  /* header file for the ExmString widget */
#include <Xm/DrawP.h>   /* header file for XmeClearBorder */
#include <Xm/RepType.h> /* header file for representation type facility */
#include <Xm/TraitP.h>  /* header file for installing traits */
#include <Xm/AccTextT.h> /* header file for AccessTextual trait */
#include <Xm/SpecRenderT.h> /* header file for XmLABEL_RENDER_TABLE */
#include <Xm/Screen.h> /* header file for screen information */

/* Define constants here. */
static const int  FIND_NATURAL_SIZE = 0;
#define UNSUPPORTED_FORMAT "Someone is trying to get or set a value for \n\
ExmNcompoundString; however, the specified format is undefined.\n"

// Define the translations string for the ExmString widget.
// All six actions will be handled by the XmPrimitive widget.
static char defaultTranslations[] =
"<EnterWindow>:		PrimitiveEnter()\n\
<LeaveWindow>:		PrimitiveLeave()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfHelp:		PrimitiveHelp()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()";

// Define the resources.
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
	XtOffsetOf( ExmCxxString, string.compound_string),
	XmRImmediate,
	(XtPointer) NULL
    },
    {
	XmNrenderTable, /* change to XmNrenderTable? */
	XmCRenderTable,
	XmRRenderTable,
	sizeof(XmRenderTable),
	XtOffsetOf( ExmCxxString,string.render_table),
	XtRCallProc,
	(XtPointer) &ExmCxxString::DefaultFont
    },
    {
	XmNalignment,
	XmCAlignment,
	XmRAlignment,
	sizeof(unsigned char),
	XtOffsetOf( ExmCxxString,string.alignment),
	XmRImmediate,
	(XtPointer) XmALIGNMENT_CENTER
    },
    {
        XmNrecomputeSize,
        XmCRecomputeSize,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( ExmCxxString,string.recompute_size),
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
        XtOffsetOf(ExmCxxString, string.compound_string),
        ExmCxxString::GetValuesCompoundString,
        NULL
   }
};

// No actions array needed.
// static XtActionsRec ActionsList[] = {};

/************************************
 * Class declaration for CxxString *
 ************************************/
ExmCxxStringClass::ExmCxxStringClass(char*		name,
				     WidgetClass	parentClass,
				     Cardinal		widgetSize,
				     XtProc		class_init,
				     XtActionList	actions,
				     Cardinal		num_actions,
				     XtResourceList	res,
				     Cardinal		num_res,
				     XtInitProc		cxx_cast)
   : ExmCxxSimpleClass(name, parentClass, widgetSize, class_init,
                       actions, num_actions, res, num_res, cxx_cast)
{
 // Override all core class record fields that need to be overrided.
   core_class.tm_table = defaultTranslations;
   core_class.compress_exposure = XtExposeCompressMaximal;

 // Override all primitive class record fields that need to be overrided.

 // Override all simple class record fields that need to be overrided.

 // Define string class record fields.
   default_render_table_type = XmLABEL_RENDER_TABLE;
   cxx_simple_extension      = NULL;
};


// Set up the class record such that whenever it is instantiated,
// the widget metaclass constructor is called.
externaldef(exmCxxStringclassrec)
ExmCxxStringClass exmCxxStringClassRec("ExmCxxString",
				     (WidgetClass) &exmCxxSimpleClassRec,
				     sizeof(ExmCxxString),
				     ExmCxxStringClass::ClassInitialize,
				     NULL, // ActionsList,
				     0, // XtNumber(ActionsList),
				     resources,
				     XtNumber(resources),
				     ExmCxxString::_MakeCxxWidget);


// Establish the widget class name as an externally accessible symbol.
// Use the "externaldef" macro rather than the "extern" keyword.
externaldef(exmCxxStringwidgetclass) WidgetClass exmCxxStringWidgetClass =
                                      (WidgetClass) &exmCxxStringClassRec;

// Define trait structure variables.
// Since ExmString works with XmString data, the ExmString widget must hold
// the XmQTaccessTextual trait.  Here, we need to declare a trait structure
// variable named StringATT.
static XmAccessTextualTraitRec StringATT = {
  0,  				/* version */
  ExmCxxString::StringGetValue,
  ExmCxxString::StringSetValue,
  ExmCxxString::StringPreferredFormat,
};

// Define some global representation type variables.
static XmRepTypeId alignmentId;
long dummy; /* memory leaks */

/******************************************************************************
 *
 *  DefaultFont:
 *      Called by the Intrinsics to establish a default value for the
 *      XmNrenderTable resource.
 *
 *****************************************************************************/
void ExmCxxString::DefaultFont (
        Widget w,
        int,
        XrmValue *value
            )
{
  static XmRenderTable  f1;

  // Find the default render table associated with the default
  // render table type.
  //  f1 = XmeGetDefaultRenderTable(w, string_class.default_render_table_type);
  f1 = XmeGetDefaultRenderTable (w,  ((ExmCxxStringWidgetClass) XtClass(w))->
				 default_render_table_type);

  value->addr = (char *)&f1;
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

void ExmCxxString::GetValuesCompoundString(Widget w, int, XtArgVal *value)
{
  ExmCxxString *sw = (ExmCxxString *) w;
  XmString  string;

  /* All Motif widgets are responsible for making a copy of an XmString
     resource whenever an application accesses the resource through a call
     to XtGetValues. */
  string = XmStringCopy(sw->string.compound_string);

  *value = (XtArgVal) string;
}

/************************************************************************
 *
 *  _MakeCxxWidget
 *	Call `new' to make each widget a C++ object
 *
 ************************************************************************/
void ExmCxxString::_MakeCxxWidget(Widget,
                                  Widget new_w,
			          ArgList, Cardinal*)
{
    ExmCxxStringWidget dummy = IDLE_NEW(new_w) ExmCxxString;
}

/******************************************************************************
 *
 *  ClassInitialize
 *     Called the first time a widget of this class is instantiated.
 *
 *****************************************************************************/
void ExmCxxStringClass::ClassInitialize(void)
{
  // The following statement is not required in the C version of this widget;
  // however, you must provide this statement in the C++ version.
  exmCxxStringClassRec.SetBaseClassExtensionQuark();

  // Use existing Motif representation type for XmNalignment
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
void ExmCxxStringClass::class_part_initialize()
{
 // Install the XmQTaccessTextual trait on the ExmString class and all
 // its subclasses.
   XmeTraitSet((XtPointer)(WidgetClass)this , XmQTaccessTextual,
               (XtPointer) &StringATT);
}


/******************************************************************************
 *
 *  Initialize
 *      Called when this widget is instantiated.
 *
 *****************************************************************************/
void ExmCxxString::initialize (
        Widget request_w,
        ArgList args,
        Cardinal *num_args
           )
{
  Arg dirArgs[1];

  // Simulate chaining by calling the initialize method of ExmCxxString's
  // superclass (ExmCxxSimple).
  ExmCxxSimple::initialize(request_w, args, num_args);

  // Copy compound_string
  string.compound_string = XmStringCopy(string.compound_string);
  string.render_table = XmRenderTableCopy(string.render_table, NULL, 0);

  /* Validate  XmNalignment */
  if (!XmRepTypeValidValue (alignmentId, string.alignment, (Widget)this))
    string.alignment = XmALIGNMENT_CENTER;

  AlignmentDirection();
  calc_visual_size();
  reconfigure(exmCxxStringWidgetClass, NULL);
}


/******************************************************************************
 *
 *  Destructor
 *      Called by the Intrinsics whenever this widget is deallocated.
 *
 *****************************************************************************/
ExmCxxString::~ExmCxxString()
{
 // Free the memory used to hold the ExmNcompound_string and
 // XmNrenderTable resource values.
   if (string.compound_string != NULL)
     XmStringFree(string.compound_string);

   if (string.render_table != NULL)
     XmRenderTableFree(string.render_table);
}



/******************************************************************************
 *
 * Resize
 *
 *****************************************************************************/
void ExmCxxString::resize(void)
{
  Dimension  mw, mh;
  Dimension  window_decoration_thickness;

  // Configure internal geometry using current size.
  if ((simple.visual.width  == FIND_NATURAL_SIZE) ||
      (simple.visual.height == FIND_NATURAL_SIZE))
   return;

  window_decoration_thickness = primitive.highlight_thickness  +
    			        primitive.shadow_thickness;

  /* Determine where the string's bounding box should start. */
  /* First determine the X coordinate of the bounding box. */
  mw = window_decoration_thickness + simple.margin_width;

  switch (string.text_starts_here)
    {
    case ExmSTART_STRING_LEFT_SIDE :
      simple.visual.x = mw;
      break;
    case ExmSTART_STRING_RIGHT_SIDE :
      simple.visual.x = core.width - (mw + simple.visual.width +
					      window_decoration_thickness);
      break;
    case ExmCENTER_STRING :
      simple.visual.x = (core.width - simple.visual.width)/2;
      break;
    };


  /* Now do the same for the vertical dimension. */
  mh = window_decoration_thickness + simple.margin_height;

  /* If the widget has enough vertical space to display all the lines in
     the string, then center the string. */
  if (core.height >
      ((2 * window_decoration_thickness) + simple.visual.height)) {
    simple.visual.y = (core.height - simple.visual.height)/2;
  }
  else if (core.height > 2 * window_decoration_thickness) {
    /* Space is very tight.  We will eliminate the top margin altogether and
       start the first line of the compound string snug against the bottom of
       the top edge of the window decorations. */
    simple.visual.y = window_decoration_thickness;
  }
  else
    /* Space is so tight that we do not have enough space to display even
       one pixel of the visual. */
    simple.visual.y = window_decoration_thickness;
}

/****************************************************************************
 *
 * AlignmentDirection:
 *     Called by Initialize and by SetValues.
 *
 ****************************************************************************/
void ExmCxxString::AlignmentDirection(void)
{
  /*
   * This method determines where the text starts.  The text could
   * be centered.
   * start at the left side of the widget.
   * start at the right side of the widget.
   */

  if (string.alignment == XmALIGNMENT_CENTER)
    /* The string will be centered. */
    string.text_starts_here = ExmCENTER_STRING;

  else if (
	   (XmDirectionMatch(primitive.layout_direction, XmLEFT_TO_RIGHT) &&
	    string.alignment == XmALIGNMENT_BEGINNING)
	   ||
	   (XmDirectionMatch(primitive.layout_direction, XmRIGHT_TO_LEFT) &&
	    string.alignment == XmALIGNMENT_END)
           )
    /* The string will start at the left side of the widget. */
    string.text_starts_here = ExmSTART_STRING_LEFT_SIDE;

  else if (
	   (XmDirectionMatch(primitive.layout_direction, XmLEFT_TO_RIGHT) &&
	    string.alignment == XmALIGNMENT_END)
	   ||
	   (XmDirectionMatch(primitive.layout_direction, XmRIGHT_TO_LEFT) &&
	    string.alignment == XmALIGNMENT_BEGINNING)
           )
    /* The string will start at the right side of the widget. */
    string.text_starts_here = ExmSTART_STRING_RIGHT_SIDE;
}


/******************************************************************************
 *
 *  SetValues
 *
 *****************************************************************************/
Boolean ExmCxxString::set_values (
        Widget old_w,
        Widget request_w,
        ArgList,
        Cardinal*)
{
 ExmCxxString *cw = (ExmCxxString *)old_w;
 ExmCxxString *rw = (ExmCxxString *)request_w;
 Boolean redisplayFlag = False;

 /* Free old and Copy new ExmNcompoundString if its value has changed */
 if (!XmStringCompare(string.compound_string, cw->string.compound_string)) {
   XmStringFree(cw->string.compound_string);
   // Copy the new string
   string.compound_string = XmStringCopy(string.compound_string);
 }

 // Update XmNcompound_string whenever XmNrenderTable changes
 if (string.render_table != cw->string.render_table) {
   string.render_table = XmRenderTableCopy(string.render_table,
					   NULL, 0);
   XmRenderTableFree (cw->string.render_table);
   cw->string.render_table = (XmRenderTable) NULL;
   simple.need_to_reconfigure = True;
 }

 // Validate any changes to the value of the XmNalignment resource.
 // If the requested new value is not valid, then reset the value of the
 // XmNalignment resource to the old value.
 if (string.alignment != cw->string.alignment) {
   if (!XmRepTypeValidValue(alignmentId, string.alignment, (Widget)this))
     string.alignment = cw->string.alignment;
   else simple.need_to_reconfigure = True;
 }

 // If the XmNrecomputeSize resource used to be False but is now True, then
 // we will have to set in motion the series of calls that will lead to
 // an appropriate size for the widget.
 if ((string.recompute_size == True) &&
     (cw->string.recompute_size == False)) {
   simple.need_to_reconfigure = True;

   if (core.width == cw->core.width)
     core.width = FIND_NATURAL_SIZE;
   if (core.height == cw->core.height)
     core.height = FIND_NATURAL_SIZE;
 }

 // If the Alignment has changed or the string direction has changed,
 // then the widget needs to determine a new starting position for
 // the bounding text box.
 if ( string.alignment != cw->string.alignment ||
      primitive.layout_direction != cw->primitive.layout_direction)
   AlignmentDirection();

 // Determine whether or not the widget needs to be reconfigured.  Just
 // about any change to a resource will necessitate a reconfiguration.
   // If the widget does need to be reconfigured, call Reconfigure.
   if (simple.need_to_reconfigure == True) {
     reconfigure(exmCxxStringWidgetClass, old_w);
     redisplayFlag = True;
   }

 return (redisplayFlag);
}

/******************************************************************************
 *
 *  QueryGeometry
 *     Called by the Intrinsics in response to a proposed changed in geometry.
 *
 *****************************************************************************/
XtGeometryResult  ExmCxxString::query_geometry(
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
  Widget w = (Widget) this;

  if (!XtIsRealized(w)) {
    /* Widget has not been realized yet. */
    reply->width  = XtWidth(w);   /* might be 0 */
    reply->height = XtHeight(w);  /* might be 0 */
  } else {
    if (string.recompute_size) {
      /* The user will allow the ExmCxxString widget to change size, so
	 let's call CalcWidgetSize to calculate the widget's
	 preferred size. */
      int save_w, save_h;

      save_w = XtWidth(w);
      save_h = XtHeight(w);
      calc_widget_size();
      simple.pref_width = XtWidth(w);
      simple.pref_height = XtHeight(w);
      XtWidth(w) = save_w;
      XtHeight(w) = save_h;
    }
    reply->width  = simple.pref_width;
    reply->height = simple.pref_height;
  }

  /* Return our preferred size */
  return XmeReplyToQueryGeometry(w, request, reply);
}

/******************************************************************************
 *
 *  DrawVisual
 *
 *****************************************************************************/
void ExmCxxString::draw_visual (void)
{
 // Assuming that there is at least a minimal amount of space inside
 // the widget, and assuming that the compound string is not NULL,
 // then render the widget with XmStringDraw.
   if (string.compound_string &&
      (simple.visual.width != 0) &&
      (simple.visual.height != 0)) {
     XmStringDraw (XtDisplay(this), XtWindow(this),
                   string.render_table,
                   string.compound_string,
		   select_gc(),
		   simple.visual.x, simple.visual.y,
		   simple.visual.width, string.alignment,
		   primitive.layout_direction, NULL);
     XmeClearBorder(XtDisplay(this), XtWindow(this),
                    (int)0, (int)0,
                    (Dimension)core.width, (Dimension)core.height,
                    (Dimension)(primitive.highlight_thickness +
                                primitive.shadow_thickness +
                                simple.margin_width));
   }
}

/******************************************************************************
 *
 *  CreateGC:
 *      Called by the Initialize method of the base class (ExmSimple).
 *
 *****************************************************************************/
void ExmCxxString::create_gc(void)
{
  XGCValues values;
  XtGCMask valueMask;
  XFontStruct *fs = (XFontStruct *) NULL;
  Arg          args[2];
  Pixmap       insensitiveStippleBitmap;

  /* This function creates two GC's: one to render a sensitive widget
     and the other to render an insensitive widget. */

  /* First, create the sensitive GC (named normal_GC). */
  valueMask = GCForeground | GCBackground | GCGraphicsExposures;
  values.foreground = primitive.foreground;
  values.background = core.background_pixel;
  values.graphics_exposures = False;

  /* In order to set the GCFont field of the GC, we must gather XFontStruct
     information out of the render table. */
  XmeRenderTableGetDefaultFont(string.render_table, &fs);
  if (fs != NULL) {
    values.font = fs->fid;
    valueMask |= GCFont;
  }

  simple.normal_gc = XtGetGC ((Widget)this, valueMask, &values);

  /* Next, create the insensitive GC.  This GC will share the same
     foreground, background, font, and graphics exposures as the sensitive
     GC, but will hold a different fill style and stipple pattern. */
  valueMask |= GCFillStyle | GCStipple;
  values.fill_style = FillStippled;

  /* Gather the Motif-appropriate insensitive stipple bitmap. */
  XtSetArg(args[0], XmNinsensitiveStippleBitmap, &insensitiveStippleBitmap);
  XtGetValues(XmGetXmScreen(XtScreen((Widget) this)), args, 1);
  values.stipple = insensitiveStippleBitmap;

  simple.insensitive_gc = XtGetGC((Widget) this, valueMask, &values);
}

/******************************************************************************
 *
 * CalcVisualSize
 *
 *****************************************************************************/
void ExmCxxString::calc_visual_size(void)
{
  // Ideally, how much space would the compound string require?
 if (string.compound_string && !XmStringEmpty(string.compound_string)) {
   XmStringExtent (string.render_table, string.compound_string,
		   &(simple.visual.width), &(simple.visual.height));
 } else {
   // The string is empty, so it requires no space.
   simple.visual.width = 0;
   simple.visual.height = 0;
 }
}

/******************************************************************************
 *
 * WidgetBaselines:
 *      Called by manager widgets needing to align text-based visuals.
 *      This method can also be called directly by an application through
 *      the XmWidgetGetBaselines function.
 *
 *****************************************************************************/
Boolean ExmCxxString::widget_baseline(
        Dimension **baselines,
        int *line_count)
{
  XmStringContext context = NULL;
  XmString        current_string = NULL;

  XmString str = NULL, str2;
  XmString str1 = NULL;
  char* text1;
  char* text2;
  XmStringTag char_set1, char_set2;
  XmStringDirection direction1, direction2;
  XmRenderTable RenderTable=NULL;
  Boolean separator1, separator2;
  Dimension *base_array;
  Dimension Offset;
  int index;

  /* This function returns True to indicate that the widget is displaying
     text, or False to indicate that the widget is not displaying text.
     If the widget is displaying text, this function calculates the baseline
     of each displayed line of text. */

  index = 0;
  RenderTable = string.render_table;

  /* Initialize the string context.  If the XmStringInitContext call returns
     False, then no string context could be found which suggests that no
     text is being displayed in the widget. */
  current_string = string.compound_string;
  if (!XmStringInitContext (&context, current_string))
    return (False);

  /* Determine how many lines are in the displayed string. */
  *line_count = XmStringLineCount(current_string);

  base_array = (Dimension *)XtMalloc((sizeof(Dimension) * (*line_count)));
  /*    Offset = ((ExmStringWidget) w)->string.visual.y;  */
  Offset = simple.visual.y;

  /* Go through the compound string, segment by segment. */
  while (XmStringGetNextSegment (context, &text1, &char_set1, &direction1,
				 &separator1)) {
    if (str1)
      XmStringFree(str1);

    str1 = XmStringCreate(text1, char_set1);
    XtFree(text1);

    if (separator1)
      {
        while (XmStringPeekNextComponent(context) ==
	       XmSTRING_COMPONENT_SEPARATOR) {
	  XmStringGetNextComponent (context, &text1, &char_set1, &direction1,
				    NULL, NULL, NULL);
	  base_array[index++] = Offset +
	    XmStringBaseline (RenderTable, str1);
	  Offset += XmStringHeight (RenderTable, str1);
	}
      } else if (XmStringGetNextSegment(context, &text2, &char_set2,
					&direction2, &separator2)) {
        if (separator2)
	  {
	    str2 = XmStringCreate(text2, char_set2);
	    str1 = XmStringConcat(str1, str2);
	    base_array[index++] = Offset +
	      XmStringBaseline (RenderTable, str1);
	    Offset += XmStringHeight (RenderTable, str1);
	  } else {
	    str2 = XmStringCreate(text2, char_set2);
	    str1 = XmStringConcat(str1, str2);
	  }

	XtFree(text2);
	XmStringFree(str2);
	XtFree(char_set2);
      } else {
	XtFree(char_set1);
	break;
      }

    XtFree(char_set1);
  } /* end of outer while loop */

  base_array[index++] = Offset + XmStringBaseline (RenderTable, str1);

  XmStringFree(str1);

  *baselines = base_array;

  XmStringFreeContext(context);
  XmStringFree(str);

  return (True);
}


/******************************************************************************
 *
 *  WidgetDisplayRect:
 *      Called by several Motif managers to determine how to align the visuals
 *      drawn by primitives.  In addition, an application can access this
 *      method by calling XmWidgetGetDisplayRect.
 *
 *****************************************************************************/
Boolean ExmCxxString::widget_display_rect(
        XRectangle  *displayrect
                 )
{
 // Determine the bounding box of the string.
   if ((simple.visual.width > 0 &&
        simple.visual.height > 0)) {
     displayrect->x =       simple.visual.x;
     displayrect->y =       simple.visual.y;
     displayrect->width =   simple.visual.width;
     displayrect->height =  simple.visual.height;
     return True;  // Yes, this widget contains something displayable.
   }
   else  {
     return False; // this widget is not currently displaying a visual.
   }
}

/******************************************************************************
 *
 *  Trait Methods:
 *      We now provide the code for the three methods defined by the
 *      AccessTextual trait.
 *
 *****************************************************************************/


/******************************************************************************
 *
 *  StringGetValue
 *
 *****************************************************************************/
XtPointer ExmCxxString::StringGetValue(
     Widget w,
     int format)
{
  ExmCxxString *string_w = (ExmCxxString *) w;
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
 *  StringSetValue
 *
 ******************************************************************************/
void ExmCxxString::StringSetValue(
     Widget w,
     XtPointer s,
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
  case XmFORMAT_XmSTRING:
    temp = (XmString) s;
    freetemp = False;
    break;

  case XmFORMAT_WCS:
    str2 = (wchar_t *) s;
    /* How long is str2? */
    length = 0;
    while (str2[length] != 0)
      length++;
    /* malloc enough space to hold str */
    str = (char*) XtMalloc(MB_CUR_MAX * (length+1));
    wcstombs(str, str2, MB_CUR_MAX * (length+1));
    s = str;

  case XmFORMAT_MBYTE:
    temp = XmStringCreateLocalized((char*) s);
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

/******************************************************************************
 *
 *  StringPreferredFormat
 *      Called by a parent widget to determine the preferred format of
 *      this widget.  The possible returned formats are:
 *            * XmFORMAT_XmSTRING
 *            * XmFORMAT_MBYTE
 *            * XmFORMAT_WCS
 *
 *****************************************************************************/
int ExmCxxString::StringPreferredFormat(Widget)
{
  /* Choose XmFORMAT_XmSTRING because the ExmString widget holds its displayed
     text in XmString format (as opposed to Multibyte or WCS format). */
  return(XmFORMAT_XmSTRING);
}

/******************************************************************************
 *
 *  ExmCreateString
 *      Externally accessable function for creating a String widget
 *
 *****************************************************************************/
Widget ExmCreateString (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                )
{
   return (XtCreateWidget(name,exmCxxStringWidgetClass,parent,
			  arglist,argCount));
}

