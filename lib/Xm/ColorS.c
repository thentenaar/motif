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
 *
 */

/************************************************************
 *       INCLUDE FILES
 ************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "ColorSP.h"

#include <Xm/Xm.h>
#include <Xm/VaSimpleP.h>

#include <Xm/ButtonBox.h>
#include <Xm/Scale.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>

#include <Xm/ExtP.h>
#include "XmI.h"

/************************************************************
 *       TYPEDEFS AND DEFINES
 ************************************************************/

#define SUPERCLASS ((WidgetClass) &xmManagerClassRec)

/************************************************************
 *       MACROS
 ************************************************************/

/************************************************************
 *       GLOBAL DECLARATIONS
 ************************************************************/

extern void 		XmeNavigChangeManaged(Widget);

/************************************************************
 *       STATIC FUNCTION DECLARATIONS
 ************************************************************/

static void ChangeManaged(Widget w);
static void ClassInitialize(void), Destroy(Widget), Resize(Widget);
static void ClassPartInitialize(WidgetClass w_class);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryHandler(Widget, XtWidgetGeometry *,
					XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
				      XtWidgetGeometry *);

static Boolean UpdateColorWindow(XmColorSelectorWidget, Boolean);
static Boolean color_name_changed(XmColorSelectorWidget, char *);
static Boolean FindColor(XmColorSelectorWidget, int *);
static Boolean CvtStringToColorMode(Display *, XrmValuePtr, Cardinal,
				    XrmValuePtr, XrmValuePtr, XtPointer *);
static Boolean DefaultVisualDisplay(XmColorSelectorWidget, Pixel, XColor, char *);

static void CalcPreferredSize(XmColorSelectorWidget, Dimension *, Dimension *);
static void SelectColor(XmColorSelectorWidget);
static void slider_changed(Widget, XtPointer, XtPointer);
static void list_selected(Widget, XtPointer, XtPointer);
static void change_mode(Widget, XtPointer, XtPointer);
static void new_mode(XmColorSelectorWidget, XmColorMode);
static void compute_size(XmColorSelectorWidget);
static void init_color_list(XmColorSelectorWidget, ArgList, Cardinal, Boolean);
static void SetSliders(XmColorSelectorWidget);

static void CreateColorSliders(XmColorSelectorWidget, ArgList, Cardinal);
static void CreateSelectorRadio(XmColorSelectorWidget, ArgList, Cardinal);
static void CreateColorWindow(XmColorSelectorWidget, ArgList, Cardinal);
static void NoPrivateColormaps(XmColorSelectorWidget, Pixel, XColor, char *);
static void PrivateColormaps(XmColorSelectorWidget, Pixel, XColor, char *);

#ifdef notdef
static void CreateTypes(XmColorSelectorWidget, Widget, ArgList, Cardinal);
#endif

static int CmpColors(const void *, const void *);
static char *find_name(char *);
static int GetVisual(XmColorSelectorWidget);

static void GetValues_XmNredSliderLabel ( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNgreenSliderLabel( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNblueSliderLabel( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNcolorListTogLabel( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNsliderTogLabel( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNnoCellError( Widget w, int n, XtArgVal *value) ;
static void GetValues_XmNfileReadError( Widget w, int n, XtArgVal *value) ;

/************************************************************
 *       STATIC DECLARATIONS
 ************************************************************/

static XtResource resources[] =
{
  {
    XmNcolorMode, XmCColorMode, XmRXmColorMode,
    sizeof(XmColorMode), XtOffsetOf(XmColorSelectorRec, cs.color_mode),
    XmRImmediate, (XtPointer) XmScaleMode
  },

  {
    XmNcolorName, XmCString, XmRString,
    sizeof(String), XtOffsetOf(XmColorSelectorRec, cs.color_name),
    XmRString, "White"
  },
  {
    XmNmarginWidth, XmCMargin, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmColorSelectorRec, cs.margin_width),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNmarginHeight, XmCMargin, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmColorSelectorRec, cs.margin_height),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNredSliderLabel, XmCSliderLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[0]),
    XmRString, (XtPointer) "Red"
  },

  {
    XmNgreenSliderLabel, XmCSliderLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[1]),
    XmRString, (XtPointer) "Green"
  },

  {
    XmNblueSliderLabel, XmCSliderLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[2]),
    XmRString, (XtPointer) "Blue"
  },

  {
    XmNcolorListTogLabel, XmCTogLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.tog_labels[0]),
    XmRString, (XtPointer) "Color List"
  },

  {
    XmNsliderTogLabel, XmCTogLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.tog_labels[1]),
    XmRString,(XtPointer)"Color Sliders"
  },

  {
    XmNnoCellError, XmCNoCellError, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.no_cell_error),
    XmRString, (XtPointer)"\n\nNo Color Cell Available!"
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNmarginWidth, sizeof(Dimension),
    XtOffsetOf(XmColorSelectorRec, cs.margin_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNmarginHeight, sizeof(Dimension),
    XtOffsetOf(XmColorSelectorRec, cs.margin_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNredSliderLabel, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[0]),
    GetValues_XmNredSliderLabel, NULL
  },

  {
    XmNgreenSliderLabel, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[1]),
    GetValues_XmNgreenSliderLabel, NULL
  },

  {
    XmNblueSliderLabel, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.slider_labels[2]),
    GetValues_XmNblueSliderLabel, NULL
  },

  {
    XmNcolorListTogLabel, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.tog_labels[0]),
    GetValues_XmNcolorListTogLabel, NULL
  },

  {
    XmNsliderTogLabel, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.tog_labels[1]),
    GetValues_XmNsliderTogLabel, NULL
  },

  {
    XmNnoCellError, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.no_cell_error),
    GetValues_XmNnoCellError, NULL
  },

  {
    XmNfileReadError, sizeof(XmString),
    XtOffsetOf(XmColorSelectorRec, cs.strings.file_read_error),
    GetValues_XmNfileReadError, NULL
  }
};

XmColorSelectorClassRec xmColorSelectorClassRec = {
  { /* core fields */
    /* superclass               */      SUPERCLASS,
    /* class_name               */      "XmColorSelector",
    /* widget_size              */      sizeof(XmColorSelectorRec),
    /* class_initialize         */      ClassInitialize,
    /* class_part_initialize    */      ClassPartInitialize,
    /* class_inited             */      False,
    /* initialize               */      Initialize,
    /* initialize_hook          */      NULL,
    /* realize                  */      XtInheritRealize,
    /* actions                  */      NULL,
    /* num_actions              */      (Cardinal)0,
    /* resources                */      (XtResource*)resources,
    /* num_resources            */      XtNumber(resources),
    /* xrm_class                */      NULLQUARK,
    /* compress_motion          */      True,
    /* compress_exposure        */      True,
    /* compress_enterleave      */      True,
    /* visible_interest         */      False,
   /* destroy                  */      Destroy,
    /* resize                   */      Resize,
    /* expose                   */      NULL,
    /* set_values               */      SetValues,
    /* set_values_hook          */      NULL,
    /* set_values_almost        */      XtInheritSetValuesAlmost,
    /* get_values_hook          */      NULL,
    /* accept_focus             */      NULL,
    /* version                  */      XtVersion,
    /* callback_private         */      NULL,
    /* tm_table                 */      XtInheritTranslations,
    /* query_geometry           */      (XtGeometryHandler) QueryGeometry,
    /* display_accelerator      */      XtInheritDisplayAccelerator,
    /* extension                */      NULL,
  },
  {            /* composite_class fields */
    /* geometry_manager   	*/      GeometryHandler,
    /* change_managed     	*/      ChangeManaged,
    /* insert_child       	*/      XtInheritInsertChild,
    /* delete_child       	*/      XtInheritDeleteChild,
    /* extension          	*/      NULL,
  },
  {            /* constraint_class fields */
   /* resource list        	*/ 	NULL,
   /* num resources        	*/ 	0,
   /* constraint size      	*/ 	sizeof(XmColorSelectorConstraintRec),
   /* destroy proc         	*/ 	NULL,
   /* init proc            	*/ 	NULL,
   /* set values proc      	*/ 	NULL,
   /* extension            	*/ 	NULL,
  },
  {            /* manager_class fields */
   /* default translations   	*/      XtInheritTranslations,
   /* syn_resources          	*/      get_resources,
   /* num_syn_resources      	*/      XtNumber(get_resources),
   /* syn_cont_resources     	*/      NULL,
   /* num_syn_cont_resources 	*/      0,
   /* parent_process         	*/      NULL,
   /* extension              	*/      NULL,
  },
  {	      /* color_selector_class fields */
    /* mumble 		  	*/	NULL,
  }
};

WidgetClass xmColorSelectorWidgetClass = (WidgetClass)&xmColorSelectorClassRec;


#define N_COLORS 782
static const struct ColorInfo color_info[N_COLORS];

/************************************************************
 *       STATIC CODE
 ************************************************************/

/*      Function Name: ClassInitialize
 *      Description:   Called to initialize class specific information.
 *      Arguments:     widget_class - the widget class.
 *      Returns:       none.
 */

static void
ClassInitialize(void)
{
    XmColorSelectorClassRec *wc = &xmColorSelectorClassRec;

    XtSetTypeConverter(XmRString, XmRXmColorMode,
		       (XtTypeConverter) CvtStringToColorMode,
		       NULL, (Cardinal) 0, XtCacheAll, NULL);
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmCOLORSELECTOR_BIT);
}

/*      Function Name: Initialize
 *      Description:   Called to initialize information specific
 *                     to this widget.
 *      Arguments:     request - what was originally requested.
 *                     set - what will be created (our superclasses have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *      Returns:       none.
 */

static void
Initialize(Widget request, Widget set, ArgList args, Cardinal *num_args)
{
    XmColorSelectorWidget	csw = (XmColorSelectorWidget)set;
    Dimension			width, height;
    String 			temp;
    char			message_buffer[BUFSIZ];
    ArgList 			f_args;
    Cardinal 			f_num_args;
    Widget			button;

    _XmFilterArgs(args, *num_args, xm_std_filter, &f_args, &f_num_args);

    /*
     * Initialize important values.
     */

    XmColorS_good_cell(csw) = False;

    temp = XmColorS_color_name(csw);
    XmColorS_color_name(csw) = NULL;
    XmColorS_list(csw) = NULL;

    CreateColorSliders(csw, f_args, f_num_args);
    CreateSelectorRadio(csw, f_args, f_num_args);
    CreateColorWindow(csw, f_args, f_num_args);
    XmColorS_colors(csw) = NULL;
    init_color_list(csw, f_args, f_num_args, True);

    if (!color_name_changed(csw, temp)) {
        snprintf(message_buffer, BUFSIZ, XmNunparsableColorMsg, temp);
	XmeWarning((Widget)set, message_buffer);
	color_name_changed(csw, "White");
    }

    slider_changed(NULL, (XtPointer) csw, NULL);

    CalcPreferredSize(csw, &width, &height);

    if ( csw->core.width < 1 )
	csw->core.width = width;

    if ( csw->core.height < 1 )
	csw->core.height = height;

    new_mode(csw, XmColorS_color_mode(csw));
    button = XmColorS_chose_mode(csw)[XmColorS_color_mode(csw)];
    XmToggleButtonSetState(button, True, False);

    XtFree((XtPointer) f_args);

    {
    int i;
    for( i = 0; i < 3; i++ )
    	XmColorS_strings(csw).slider_labels[i] = XmStringCopy(XmColorS_strings(csw).slider_labels[i]);
    for (i = 0; i< XmColorSelector_NUM_TOGGLES; i++)
	XmColorS_strings(csw).tog_labels[i] = XmStringCopy(XmColorS_strings(csw).tog_labels[i]);
    XmColorS_strings(csw).file_read_error = XmStringCopy(XmColorS_strings(csw).file_read_error);
    XmColorS_strings(csw).no_cell_error = XmStringCopy(XmColorS_strings(csw).no_cell_error);
    }

}

/*      Function Name: Destroy
 *      Description:   Called to destroy this widget.
 *      Arguments:     w - Color Selector Widget to destroy.
 *      Returns:       none.
 */

static void
Destroy(Widget w)
{
    XmColorSelectorWidget	csw = (XmColorSelectorWidget)w;

    if (XmColorS_good_cell(csw)) {
    	XFreeColors(XtDisplay(csw), csw->core.colormap,
		    &XmColorS_color_pixel(csw), 1, 0);
    }

    XtFree((XtPointer)XmColorS_colors(csw));
    XtFree((XtPointer)XmColorS_color_name(csw));

    {
    int i;
    for( i = 0; i < 3; i++ )
    	XmStringFree(XmColorS_strings(csw).slider_labels[i]);
    for (i = 0; i< XmColorSelector_NUM_TOGGLES; i++)
	XmStringFree(XmColorS_strings(csw).tog_labels[i]);
    XmStringFree(XmColorS_strings(csw).file_read_error);
    XmStringFree(XmColorS_strings(csw).no_cell_error);
    }
}

/*      Function Name: Resize
 *      Description:   Called when this widget has been resized.
 *      Arguments:     w - Color Selector Widget to realize.
 *      Returns:       none.
 */

static void
Resize(Widget w)
{
    compute_size((XmColorSelectorWidget)w);
}

/*      Function Name: SetValues
 *      Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *      Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *      Returns:       none
 */

static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal *num_args)
{
    XmColorSelectorWidget	csw = (XmColorSelectorWidget)set;
    XmColorSelectorWidget	curr = (XmColorSelectorWidget)current;

    /*
     * Pass argument list through to all children.
     */

    {
	ArgList f_args;
	Cardinal f_num_args;

	_XmFilterArgs(args, *num_args, xm_std_filter, &f_args, &f_num_args);
	_XmSetValuesOnChildren(set, f_args, f_num_args);
	XtFree((XtPointer) f_args);
    }

    if (XmColorS_color_mode(curr) != XmColorS_color_mode(csw))
    {
	new_mode(csw, XmColorS_color_mode(csw));
	XmToggleButtonSetState(XmColorS_chose_mode(csw)[XmColorS_color_mode(csw)],
			       True, True);
    }

    if ((XmColorS_margin_height(curr) != XmColorS_margin_height(csw)) ||
	(XmColorS_margin_width(curr) != XmColorS_margin_width(csw)))
    {
	compute_size(csw);
    }

    if (XmColorS_color_name(curr) != XmColorS_color_name(csw))
    {
	String		oldValue;	/* old color name, will free. */
	String		newValue;	/* new color name, allocate   */
	char            string_buffer[BUFSIZ];

	oldValue = XmColorS_color_name(curr);
	newValue = XmColorS_color_name(csw);

	if (!streq(newValue, oldValue))
	{
	    /*
	     * Color name changed will automatically free the old
	     * value on success...
	     */

	    XmColorS_color_name(csw) = oldValue; /* so it free's the right thing. */
	    if (!color_name_changed(csw, newValue)) {
		snprintf(string_buffer, BUFSIZ, XmNunparsableColorMsg, newValue);
		XmeWarning(set, string_buffer);

		XmColorS_color_name(csw) = oldValue;
	    }
	}
	else {
	    XtFree(oldValue);
	    XmColorS_color_name(csw) = XtNewString(newValue);
	}
    }

    {
    int i;
    for( i = 0; i < 3; i++ )
	{
    	if (XmColorS_strings(curr).slider_labels[i] != XmColorS_strings(csw).slider_labels[i])
		{
		XmStringFree(XmColorS_strings(curr).slider_labels[i]);
    		XmColorS_strings(csw).slider_labels[i] = XmStringCopy(XmColorS_strings(csw).slider_labels[i]);
		XtVaSetValues(XmColorS_sliders(csw)[i], XmNtitleString, XmColorS_strings(csw).slider_labels[i], NULL);
		}
	}
    for (i = 0; i< XmColorSelector_NUM_TOGGLES; i++)
	{
	if (XmColorS_strings(curr).tog_labels[i] != XmColorS_strings(csw).tog_labels[i])
		{
		XmStringFree(XmColorS_strings(curr).tog_labels[i]);
		XmColorS_strings(csw).tog_labels[i] = XmStringCopy(XmColorS_strings(csw).tog_labels[i]);
		XtVaSetValues(XmColorS_chose_mode(csw)[i], XmNlabelString, XmColorS_strings(csw).tog_labels[i], NULL);
		}
	}

	if (XmColorS_strings(curr).file_read_error != XmColorS_strings(csw).file_read_error)
	{
		XmStringFree(XmColorS_strings(curr).file_read_error);
		XmColorS_strings(csw).file_read_error = XmStringCopy(XmColorS_strings(csw).file_read_error);
	}
	if (XmColorS_strings(curr).no_cell_error != XmColorS_strings(csw).no_cell_error)
	{
		XmStringFree(XmColorS_strings(curr).no_cell_error);
		XmColorS_strings(csw).no_cell_error = XmStringCopy(XmColorS_strings(csw).no_cell_error);
	}
    }

    return FALSE;
}

/*      Function Name: GeometryHandler
 *      Description:   handles request from children for size changes.
 *      Arguments:     child - the child to change.
 *                     request - desired geometry of child.
 *                     result - what will be allowed if almost.
 *      Returns:       status.
 */

static XtGeometryResult
GeometryHandler(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *result)
{
    return(XtGeometryNo);
}

/*	Function Name: QueryGeometry
 *	Description:   Called when my parent wants to know what size
 *                     I would like to be.
 *	Arguments:     w - the widget to check.
 *                     indended - constriants imposed by the parent.
 *                     preferred - what I would like.
 *	Returns:       See Xt Manual.
 */

static XtGeometryResult
QueryGeometry(Widget w,XtWidgetGeometry *intended, XtWidgetGeometry *preferred)
{
    CalcPreferredSize((XmColorSelectorWidget) w,
		      &(preferred->width), &(preferred->height));

    return(_XmHWQuery(w, intended, preferred));
}

/*	Function Name: ChangeManaged
 *	Description: Called when a management change happens.
 *	Arguments: w - the csw widget.
 *	Returns: none
 */

static void
ChangeManaged(Widget w)
{
    compute_size((XmColorSelectorWidget) w);

    XmeNavigChangeManaged(w);
}

/************************************************************
 * Type Converters.
 ************************************************************/

/*      Function Name: 	CvtStringToColorMode
 *      Description:   	Converts a string to a ColorMode
 *      Arguments:     	dpy - the X Display.
 *                     	args, num_args - *** NOT USED ***
 *                     	from - contains the string to convert.
 *                     	to - contains the converted node state.
 *                      junk - *** NOT USED *** .
 *      Returns:
 */

static Boolean
CvtStringToColorMode(Display *dpy, XrmValuePtr args, Cardinal num_args,
		     XrmValuePtr from, XrmValuePtr to, XtPointer * junk)
{
    static XmColorMode	mode;
    char		lowerName[BUFSIZ];

    XmCopyISOLatin1Lowered(lowerName, (char *)from->addr);

    if (streq(lowerName, "listmode"))
	mode = XmListMode;
    else if (streq(lowerName, "scalemode"))
	mode = XmScaleMode;
    else {
        XtDisplayStringConversionWarning(dpy, from->addr, XmRXmColorMode);
        return(False);          /* Conversion failed. */
    }

    to->size = sizeof(XmColorMode);
    if ( to->addr == NULL ) {
        to->addr = (XtPointer)&mode;
        return(True);
    }
    else if ( to->size >= sizeof(XmColorMode) ) {
	XmColorMode *state = (XmColorMode *) to->addr;

	*state = mode;
	return(True);
    }

    return(False);
}

/************************************************************
 *      LOCAL CODE
 ************************************************************/

/*	Function Name: CalcPreferredSize
 *	Description: Calculates the size this widget would prefer to be.
 *	Arguments: csw - the color selector widget.
 *  RETURNED       width, height - preferred size of the color selector.
 *	Returns: none.
 */

static void
CalcPreferredSize(XmColorSelectorWidget csw,
		  Dimension *width, Dimension *height)
{
    XtWidgetGeometry geo;
    Widget *childP;

    *height = *width = 0;
    ForAllChildren(csw, childP) {
	if (*childP == XmColorS_bb(csw))
	    continue;

	(void)XtQueryGeometry(*childP, NULL, &geo);

	ASSIGN_MAX(*width, (geo.width + (2 * geo.border_width)));

	geo.height += 2 * geo.border_width;
	if ( *childP == XtParent(XmColorS_color_window(csw)) )
	    continue;
	else if ( *childP == XmColorS_scrolled_list(csw) )
	    *height += (int)(4 * geo.height)/3;
	else
	    *height += geo.height;

	*height += XmColorS_margin_height(csw);
    }

    *width += 2 * XmColorS_margin_width(csw);
    *height += 2 * XmColorS_margin_height(csw);
}

/*      Function Name: 	color_name_changed
 *      Description:   	Change in the color name string.
 *      Arguments:     	csw - the color selector widget.
 *                     	name - the color name.
 *      Returns:       	True if successful
 */

static Boolean
color_name_changed(XmColorSelectorWidget csw, char *name)
{
    String old_val = XmColorS_color_name(csw);

    if ( name == NULL ) {
	XmColorS_color_name(csw) = NULL;
	XtFree((XtPointer) old_val);
	return(True);
    }

    XmColorS_color_name(csw) = XtNewString(name);

    if (!UpdateColorWindow(csw, True)) {
	XtFree((XtPointer) XmColorS_color_name(csw));
	XmColorS_color_name(csw) = old_val;
	return(False);
    }

    SetSliders(csw);
    SelectColor(csw);
    XtFree((XtPointer) old_val);
    return(True);
}

/*	Function Name: SetSliders
 *	Description: Sets the values in the color sliders.
 *	Arguments: csw - the color selector widget.
 *	Returns: none
 */

static void
SetSliders(XmColorSelectorWidget csw)
{
    static Arg args[] = {
	{ XmNvalue, (XtArgVal) NULL },
    };

    args[0].value = (XtArgVal) XmColorS_slider_red(csw);
    XtSetValues(XmColorS_sliders(csw)[0], args, XtNumber(args));

    args[0].value = (XtArgVal) XmColorS_slider_green(csw);
    XtSetValues(XmColorS_sliders(csw)[1], args, XtNumber(args));

    args[0].value = (XtArgVal) XmColorS_slider_blue(csw);
    XtSetValues(XmColorS_sliders(csw)[2], args, XtNumber(args));
}

/*	Function Name: SelectColor
 *	Description: Selects the color in the list that corrosponds
 *                   to the current values of the RGB sliders.
 *	Arguments: csw - the color selector widget.
 *	Returns: none.
 */

static void
SelectColor(XmColorSelectorWidget csw)
{
    int color_num;

    if (FindColor(csw, &color_num)) {
	XmListSelectPos(XmColorS_list(csw), color_num + 1, False);
	XmListSetBottomPos(XmColorS_list(csw), color_num + 1);
    }
    else
	XmListDeselectAllItems(XmColorS_list(csw));
}


/*	Function Name:	EndsInDigits
 *	Description:	Determines if a string ends in a digit
 *	Returns:	True if it does
 */

static int
EndsInDigits(char *str)
{
    char *c = str;
    while(*c != '\0') c++;	/* advance to end of string marker */
    c--;			/* back to the last character */
    if(c >= str && isascii(*c) && isdigit(*c))
	return True;

    return False;
}

/*	Function Name: FindColor
 *	Description: Finds the index into the colors array associated with
 *                   the current slider values.  Attempts to find the slot
 *		     with a the best matching name.
 *	Arguments: csw - The color selector widget.
 * RETURNED        color_num - The color index that was found.
 *	Returns: True if color was found, false otherwise.
 *
 * NOTE: if False is returned then color_num has an undefined value.
 */

static Boolean
FindColor(XmColorSelectorWidget csw, int *color_num)
{
    ColorInfo *ptr;
    int i, red, green, blue;

    /*
     * Obtain the color settings from the ColorSelector
     * data structure
     */
    red = XmColorS_slider_red(csw);
    green = XmColorS_slider_green(csw);
    blue = XmColorS_slider_blue(csw);
    ptr = XmColorS_colors(csw);

    /*
     * Flag for finding color value
     */
    *color_num = -1;

    /*
     * Find color within the exisiting colormap assigned to
     * ColorSelector
     */
    for (i = 0; i < XmColorS_num_colors(csw); i++, ptr++)
    {
	if ((ptr->red == red) && (ptr->green == green) && (ptr->blue == blue))
	{
	    if( *color_num < 0 )
		*color_num = i;

	    /* Only change the selected color if it is better in some way */
	    if(XmColorS_color_name(csw)) {
		if(XmColorS_color_name(csw)[0] == '#')
		    *color_num = i;

		if(streq(XmColorS_color_name(csw), ptr->name))
		{
		    *color_num = i;
		    return(True);
		}
	    }
	    if(! EndsInDigits(ptr->name)) {
	        *color_num = i;
		return(True);
	    }
	}
    }

    return(*color_num >= 0);
}

/*      Function Name: 	slider_changed
 *      Description:   	One of the sliders was pressed
 *      Arguments:     	w - the slider widget.
 *                     	csw - the color selector.
 *                     	scale - the scale widget callback struct.
 *      Returns:       	none.
 */

static void
slider_changed(Widget w, XtPointer csw_ptr, XtPointer scale_ptr)
{
    XmColorSelectorWidget csw = (XmColorSelectorWidget) csw_ptr;
    XmScaleCallbackStruct *scale = (XmScaleCallbackStruct *) scale_ptr;

    if (scale_ptr != NULL) {	/* Set a new value. */
	if (w == XmColorS_sliders(csw)[0])
	    XmColorS_slider_red(csw) = scale->value;
	else if(w == XmColorS_sliders(csw)[1])
	    XmColorS_slider_green(csw) = scale->value;
	else if(w == XmColorS_sliders(csw)[2])
	    XmColorS_slider_blue(csw) = scale->value;
    }

    UpdateColorWindow(csw, False);
}

/*	Function Name: UpdateColorWindow
 *	Description: Updates the color window display.
 *	Arguments: csw - the color selector widget.
 *                 use_name - if TRUE then use the color name to update.
 *                            if FALSE then use the color sliders to update.
 *	Returns: True if successful
 */

static Boolean
UpdateColorWindow(XmColorSelectorWidget csw, Boolean use_name)
{
    int index;
    XColor color;
    Pixel foreground;
    char buf[10], new_label[BUFSIZ];

    if (!use_name) /* Update color names */
    {
	char	*freeMe;

	freeMe = XmColorS_color_name(csw);
	sprintf(buf, "#%02x%02x%02x", XmColorS_slider_red(csw),
		XmColorS_slider_green(csw), XmColorS_slider_blue(csw));

	if (FindColor(csw, &index))
	{
	    XmColorS_color_name(csw) = XtNewString(XmColorS_colors(csw)[index].name);
	    sprintf(new_label, "%s (%s)", XmColorS_color_name(csw), buf);
	}
	else
	{
	    XmColorS_color_name(csw) = XtNewString(buf);
	    sprintf(new_label, "%s", buf);
	}

	XtFree((XtPointer)freeMe );
	color.red = XmColorS_slider_red(csw) * 256;
	color.green = XmColorS_slider_green(csw) * 256;
	color.blue = XmColorS_slider_blue(csw) * 256;
    }
    else /* Update color slider */
    {
	if(XParseColor(XtDisplay(csw), csw->core.colormap,
		       XmColorS_color_name(csw), &color) == 0)
	{
	    return(False);
	}

	XmColorS_slider_red(csw) = color.red / 256;
	XmColorS_slider_green(csw) = color.green / 256;
	XmColorS_slider_blue(csw) = color.blue / 256;

	/*
	 * Attempt to replace a name that begins with a # with a real color
	 * name.
	 */

	if ((XmColorS_color_name(csw)[0] == '#') && FindColor(csw, &index))
	{
	    XtFree(XmColorS_color_name(csw));
	    XmColorS_color_name(csw) = XtNewString(XmColorS_colors(csw)[index].name);
	}
	sprintf(buf, "#%02x%02x%02x", color.red/256, color.green/256, color.blue/256);
	sprintf(new_label, "%s (%s)", XmColorS_color_name(csw), buf);
    }

    {
	long test = (long) color.red;
	test += (long) color.green;
	test += (long) color.blue;

	if (test/3 > 0x7000)
	{
	    foreground = BlackPixelOfScreen(XtScreen((Widget) csw));
	}
	else
	{
	    foreground = WhitePixelOfScreen(XtScreen((Widget) csw));
	}
    }

    /*
     * Check on the default visual
     */
    if (DefaultVisualDisplay(csw, foreground, color, (char *)new_label))
    {
	return True;
    } else
    {
	return False;
    }
}


/*      Function Name: 	list_selected
 *      Description:   	One of the list widgets was selected.
 *      Arguments:     	w - the slider widget.
 *                     	csw - the color selector.
 *                     	list - the list widget callback struct.
 *      Returns:       	none.
 */

static void
list_selected(Widget w, XtPointer csw_ptr, XtPointer list_ptr)
{
  XmColorSelectorWidget csw = (XmColorSelectorWidget) csw_ptr;
  XmListCallbackStruct *list = (XmListCallbackStruct *) list_ptr;

  XtFree(XmColorS_color_name(csw));

  XmColorS_color_name(csw) =
    XmStringUnparse(list->item,
        NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

/* deprecated
  XmStringGetLtoR(list->item, XmFONTLIST_DEFAULT_TAG,
		  &(XmColorS_color_name(csw)));
*/

  UpdateColorWindow(csw, True);
}

/*      Function Name: 	change_mode
 *      Description:   	One of the change mode buttons was pressed.
 *      Arguments:     	w - the slider widget.
 *                     	csw_ptr - the color selector.
 *                     	tp - the toggle widget callback struct.
 *      Returns:       	none.
 */

static void
change_mode(Widget w, XtPointer csw_ptr, XtPointer tp)
{
  XmColorSelectorWidget csw = (XmColorSelectorWidget) csw_ptr;
  XmToggleButtonCallbackStruct *toggle = (XmToggleButtonCallbackStruct *) tp;

  /*
   * Ignore unsets.
   */

  if (toggle->reason == XmCR_VALUE_CHANGED && toggle->set) {
    /*
     * Change the mode if it is different.
     */

    if ((w == XmColorS_chose_mode(csw)[XmListMode]) &&
	(XmColorS_color_mode(csw) != XmListMode))
      {
	new_mode(csw, XmListMode);
      }
    else if ((w == XmColorS_chose_mode(csw)[XmScaleMode]) &&
	     (XmColorS_color_mode(csw) != XmScaleMode))
      {
	new_mode(csw, XmScaleMode);
      }
  }
}

/*      Function Name: 	new_mode
 *      Description:   	mode has changed
 *      Arguments: 	csw - the color selector.
 *                      mode - the new mode.
 *      Returns:       	none.
 */

static void
new_mode(XmColorSelectorWidget csw, XmColorMode mode)
{
  XmColorS_color_mode(csw) = mode;

  if (mode == XmScaleMode) {
    SetSliders(csw);

    XtUnmanageChild(XmColorS_scrolled_list(csw));
    XtManageChild(XmColorS_bb(csw));
  }
  else {
      SelectColor(csw);	/* Select the current color in the list. */

      XtUnmanageChild(XmColorS_bb(csw));
      XtManageChild(XmColorS_scrolled_list(csw));
  }
}

/*      Function Name: 	compute_size
 *      Description:   	Do all the size and position computing.
 *      Arguments: 	csw - the color selector.
 *      Returns:       	none.
 */

static void
compute_size(XmColorSelectorWidget csw)
{
    XtWidgetGeometry	input, radio_geom, color_geom;
    Dimension		width, height;
    Position		x,y;			/* positions		*/

    /*
     * First size and place the button box and scrolled list.
     */

    y = XmColorS_margin_height(csw);
    x = XmColorS_margin_width(csw);
    width = csw->core.width - (2 * XmColorS_margin_width(csw));

    input.width = width;
    input.request_mode = CWWidth;

    (void) XtQueryGeometry(XmColorS_chose_radio(csw), NULL, &radio_geom);
    (void) XtQueryGeometry(XmColorS_color_window(csw), &input, &color_geom);

    height = (csw->core.height - 4 * XmColorS_margin_height(csw) -
	      (radio_geom.height + 2 * radio_geom.border_width));

    /*
     * Leave space for the margins and make the color area 1/3 the height
     * of the scrolled list and button box.
     */

    color_geom.height = height / 4;
    height -= color_geom.height;
    color_geom.height -= 2 * color_geom.border_width;

    _XmConfigureWidget(XmColorS_bb(csw), x, y, width, height, 0);
    _XmConfigureWidget(XmColorS_scrolled_list(csw), x, y, width, height, 0);

    y += height + XmColorS_margin_height(csw);

    /*
     * Place the radio box.
     */

    if ( radio_geom.width < csw->core.width )
	x = (int)(csw->core.width - radio_geom.width) / 2;
    else
	x = XmColorS_margin_width(csw);

    _XmConfigureWidget(XmColorS_chose_radio(csw), x, y, radio_geom.width,
		       radio_geom.height, radio_geom.border_width);

    y += radio_geom.height + XmColorS_margin_height(csw);

    /*
     * Lastly, place the color window
     */

    _XmConfigureWidget(XtParent(XmColorS_color_window(csw)), XmColorS_margin_width(csw), y,
		       width, color_geom.height, color_geom.border_width);
}

/*      Function Name: 	init_color_list
 *      Description:   	Read all the color names and add them to the list.
 *      Arguments: 	csw - the color selector.
 *                      cargs, cnum_args - a filtered arg list that was
 *                                       passed to create the color selector.
 *      Returns:       	none.
 */
static void init_color_list(XmColorSelectorWidget csw, ArgList cargs, Cardinal cnum_args, Boolean initial)
{
    FILE *file;
    char buf[BUFSIZ];
    char string_buffer[BUFSIZ + (BUFSIZ >> 1)];
    char *color_name;
    int i;
    Arg	*margs, args[20];
	XmString *strs;

    /*
     * Create new list if needed, or delete any old list items.
     */
    if (XmColorS_list(csw) == NULL)
    {
	i = 0;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(args[i], XmNvisibleItemCount, 15); i++;
	margs = XtMergeArgLists(args, i, cargs, cnum_args);
	XmColorS_list(csw) = XmCreateScrolledList((Widget) csw, "list",
					    margs, i + cnum_args);

	XtManageChild(XmColorS_list(csw));
	XmColorS_scrolled_list(csw) = XtParent(XmColorS_list(csw));

	if (XmColorS_color_mode(csw) != XmListMode)
	{
	    /* Hide the scrolled list until it need be visible... */
	    XtUnmanageChild(XmColorS_scrolled_list(csw));
	}

	XtFree((XtPointer) margs);
    }
    else
    {
	XmListDeleteAllItems(XmColorS_list(csw));
    }

    /*
    ** Because of the internal functioning of the XmList, it is better to
    ** zero out the selected item list rather than to let the item currently
    ** selected be re-selected by the XmList when the new list of colors is
    ** assigned. As is, the XmList iteratively searches through the list of
    ** selected items for each item added. Resetting the selectedItem list to
    ** NULL/0 ensures that we don't have O(n*m) XmStringCompare operations
    ** done when setting the new list below.
    ** Also, resetting the list saves us in the case in which the rgb_file
    ** is invalid or doesn't contain this selected string.
    */
    XtVaSetValues(XmColorS_list(csw), XmNselectedItems, NULL, XmNselectedItemCount, 0, NULL);
	strs = (XmString *)XtCalloc(N_COLORS, sizeof(XmString));
	for (i = 0; i < N_COLORS; i++)
		strs[i] = XmStringCreateLocalized(color_info[i].name);
	XtVaSetValues(XmColorS_list(csw), XmNitems, strs, XmNitemCount, N_COLORS, NULL);
	for (i = 0; i < N_COLORS; i++)
		XmStringFree(strs[i]);
	XtFree((XtPointer)strs);

	XtFree((char*)XmColorS_colors(csw));
	XmColorS_colors(csw)     = color_info;
	XmColorS_num_colors(csw) = N_COLORS;

	/* It would be better if we had cached the current index number so
	** we could just reset the list to the string corresponding to that
	** value, but instead wind up going through FindColor to reestablish
	** the selected string
	*/
	if (!initial)
		SelectColor(csw);

    XtAddCallback(XmColorS_list(csw), XmNsingleSelectionCallback,  list_selected, csw);
    XtAddCallback(XmColorS_list(csw), XmNbrowseSelectionCallback,  list_selected, csw);
}

/*	Function Name: CmpColors
 *	Description: Compares two colors.
 *	Arguments: ptr_1, ptr_2 - two colors too compare.
 *	Returns: none
 */

static int
CmpColors(const void * ptr_1, const void * ptr_2)
{
    ColorInfo *color1, *color2;

    color1 = (ColorInfo *) ptr_1;
    color2 = (ColorInfo *) ptr_2;

    return(strcmp(color1->name, color2->name));
}

/*      Function Name: 	find_name
 *      Description:   	Go through the buffer for looking for the name
 *			of a color string.
 *      Arguments: 	buffer - list of color names.
 *      Returns:       	pointer in the buffer where the string begins.
 */

static char*
find_name(char *buffer)
{
    char *curr, *temp;	/* current pointer */

    for (curr = buffer; curr != NULL && *curr != '\0'; curr++) {
	/*
	 * Look for first non number, non space or tab.
	 */

	if (isascii(*curr) && (isdigit(*curr) || isspace(*curr)))
	    continue;

	temp = (char *) strchr(curr, '\n');
	*temp = '\0';

	return(curr);
    }
    return(NULL);
}

/*      Function Name: 	CreateColorSliders
 *      Description:   	creates a button with three sliders (Red, Green, Blue).
 *      Arguments: 	csw - the color selector widget.
 *                      cargs, cnum_args - a filtered arg list that was
 *                                       passed to create the color selector.
 *      Returns:       	none.
 */

static void
CreateColorSliders(XmColorSelectorWidget csw,
		   ArgList cargs, Cardinal cnum_args)
{
    int i;
    Cardinal	num_args, title;
    Arg		*margs, args[10];

    num_args = 0;
    XtSetArg(args[num_args], XmNborderWidth, 0); num_args++;
    XtSetArg(args[num_args], XmNorientation, XmVERTICAL); num_args++;
    XtSetArg(args[num_args], XmNfillOption, XmFillMinor); num_args++;
    margs = XtMergeArgLists(args, num_args, cargs, cnum_args);
    XmColorS_bb(csw) = XtCreateManagedWidget("buttonBox", xmButtonBoxWidgetClass,
				       (Widget) csw,
				       margs, cnum_args + num_args);
    XtFree((XtPointer) margs);

    num_args = 0;
    XtSetArg(args[num_args], XmNmaximum, 255); num_args++;
    XtSetArg(args[num_args], XmNorientation, XmHORIZONTAL); num_args++;
    XtSetArg(args[num_args], XmNshowValue, True); num_args++;
    XtSetArg(args[num_args], XmNprocessingDirection, XmMAX_ON_RIGHT);
    num_args++;
    XtSetArg(args[num_args], XmNtitleString, NULL); title = num_args++;

    margs = XtMergeArgLists(args, num_args, cargs, cnum_args);

    for( i = 0; i < 3; i++ ) {
	margs[title].value = (XtArgVal) XmColorS_strings(csw).slider_labels[i];
	XmColorS_sliders(csw)[i] = XtCreateManagedWidget("scale",
						   xmScaleWidgetClass,
						   XmColorS_bb(csw), margs,
						   num_args + cnum_args);

	XtAddCallback(XmColorS_sliders(csw)[i], XmNdragCallback,
		      slider_changed, csw);
	XtAddCallback(XmColorS_sliders(csw)[i], XmNvalueChangedCallback,
		      slider_changed, csw);
    }
    XtFree((XtPointer) margs);
}

/*      Function Name: 	CreateSelectorRadio
 *      Description:   	creates a radio box with two toggles for selector
 *			type.
 *      Arguments: 	csw - the color selector widget.
 *                      cargs, cnum_args - a filtered arg list that was
 *                                       passed to create the color selector.
 *      Returns:       	none.
 */

static void
CreateSelectorRadio(XmColorSelectorWidget csw,
		    ArgList cargs, Cardinal cnum_args)
{
    Widget w;
    Cardinal	i, label;
    Arg		*margs, args[5];
    int count;
    static String names[] = { "colorListToggle", "colorSlidersToggle" };

    i = 0;
    XtSetArg(args[i], XmNradioBehavior, True); i++;
    XtSetArg(args[i], XmNpacking, XmPACK_COLUMN); i++;
    XtSetArg(args[i], XmNnumColumns, 2); i++;
    margs = XtMergeArgLists(args, i, cargs, cnum_args);
    w = XtCreateManagedWidget("radioBox", xmRowColumnWidgetClass,
			      (Widget) csw, margs, i + cnum_args);
    XmColorS_chose_radio(csw) = w;
    XtFree((XtPointer) margs);

    i = 0;
    XtSetArg(args[i], XmNlabelString, NULL); label = i++;
    margs = XtMergeArgLists(args, i, cargs, cnum_args);

    for (count = 0; count < XmColorSelector_NUM_TOGGLES; count++) {
	margs[label].value = (XtArgVal) XmColorS_strings(csw).tog_labels[count];

	w = XtCreateManagedWidget(names[count], xmToggleButtonWidgetClass,
				  XmColorS_chose_radio(csw), margs, i + cnum_args);
	XmColorS_chose_mode(csw)[count] = w;

	XtAddCallback(w, XmNvalueChangedCallback, change_mode, csw);
    }

    XtFree((XtPointer) margs);
}

/*      Function Name: 	CreateColorWindow
 *      Description:   	creates a label in a frame to display the
 *			currently selected color.
 *      Arguments: 	csw - the color selector widget.
 *                      cargs, cnum_args - a filtered arg list that was
 *                                       passed to create the color selector.
 *      Returns:       	none.
 */

static void
CreateColorWindow(XmColorSelectorWidget csw,ArgList cargs, Cardinal cnum_args)
{
    Widget fr;
    Arg *margs, args[10];
    Cardinal n;

    fr = XtCreateManagedWidget("colorFrame", xmFrameWidgetClass,
			       (Widget) csw, cargs, cnum_args);

    n = 0;
    XtSetArg(args[n], XmNrecomputeSize, False); n++;
    margs = XtMergeArgLists(args, n, cargs, cnum_args);
    XmColorS_color_window(csw) = XtCreateManagedWidget("colorWindow",
						       xmLabelWidgetClass,
						       fr, margs, n + cnum_args);
    XtFree((XtPointer) margs);
}

static void GetValues_XmNredSliderLabel ( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[0]);
}

static void GetValues_XmNgreenSliderLabel( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[1]);
}

static void GetValues_XmNblueSliderLabel( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[2]);
}

static void GetValues_XmNcolorListTogLabel( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).tog_labels[0]);
}

static void GetValues_XmNsliderTogLabel( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).tog_labels[1]);
}

static void GetValues_XmNnoCellError( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).no_cell_error);
}

static void GetValues_XmNfileReadError( Widget w, int n, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).file_read_error);
}

/*      Function Name: 	GetVisual
 *      Description:   	Gets the defaults visual of the screen
 *      Arguments: 	csw - the color selector widget.
 *      Returns:       	Visual id.
 */

static int
GetVisual(XmColorSelectorWidget csw)
{
    Visual * vis;
    int visual;

    vis = DefaultVisual(XtDisplay(csw), XDefaultScreen(XtDisplay(csw)));
    visual = vis->class;

    return visual;
}

/*      Function Name: 	NoPrivateColormaps
 *      Description:   	Determines the color to be used.
 *      Arguments: 	csw - the color selector widget.
 *			foreground - default color for the ColorSelector.
 *			color - Current color attributes.
 *			str - label for the ColorSelector.
 *      Returns:       	None.
 */

static void
NoPrivateColormaps(XmColorSelectorWidget csw, Pixel foreground,
		     XColor color, char *str)
{
    Arg args[5];
    XmString xm_str;
    Cardinal num_args;

    xm_str = XmStringCreateLocalized(str);
    num_args = 0;

    if (!XmColorS_good_cell(csw))
    {
	if(XAllocColor(XtDisplay(csw), csw->core.colormap, &color) )
	{
	    XmColorS_color_pixel(csw) = color.pixel;
	    XmColorS_good_cell(csw) = True;
	}
    } else {
	if (XAllocColor(XtDisplay(csw), csw->core.colormap, &color) )
	{
	    XmColorS_color_pixel(csw) = color.pixel;
	    XmColorS_good_cell(csw) = True;
	}
	else
	{
	    XmString out;
	    out = XmStringConcatAndFree(xm_str, XmColorS_strings(csw).no_cell_error);
	    xm_str = out;
	}
    }

    if (XmColorS_good_cell(csw))
    {
	color.flags = DoRed | DoGreen | DoBlue;
	color.pixel = XmColorS_color_pixel(csw);
	XtSetArg(args[num_args], XmNforeground, foreground); num_args++;
	XtSetArg(args[num_args], XmNbackground, XmColorS_color_pixel(csw));
	num_args++;
	XtSetValues(XmColorS_color_window(csw), args, num_args);
    }

    XtSetArg(args[num_args], XmNlabelString, xm_str); num_args++;
    XtSetValues(XmColorS_color_window(csw), args, num_args);
    XmStringFree(xm_str);
}

/*      Function Name: 	DoPrivateColormaps
 *      Description:   	Determines the color to be used.
 *      Arguments: 	csw - the color selector widget.
 *			foreground - default color for the ColorSelector.
 *			color - Current color attributes.
 *			str - label for the ColorSelector.
 *      Returns:       	None.
 */

static void
PrivateColormaps(XmColorSelectorWidget csw, Pixel foreground, XColor color, char *str)
{
    Arg args[5];
    XmString xm_str;
    Cardinal num_args;

    xm_str = XmStringCreateLocalized(str);
    num_args = 0;

    if (!XmColorS_good_cell(csw)) {
        if(XAllocColorCells(XtDisplay(csw), csw->core.colormap,
                            0, 0, 0, &(XmColorS_color_pixel(csw)), 1))
        {
            XmColorS_good_cell(csw) = True;
        }
        else {
            XmString out;

	    out = XmStringConcatAndFree(xm_str, XmColorS_strings(csw).no_cell_error);
            xm_str = out;
        }
    }

    if (XmColorS_good_cell(csw)) {
        color.flags = DoRed | DoGreen | DoBlue;
        color.pixel = XmColorS_color_pixel(csw);
        XStoreColor(XtDisplay((Widget) csw), csw->core.colormap, &color);
        XtSetArg(args[num_args], XmNforeground, foreground); num_args++;
        XtSetArg(args[num_args], XmNbackground, XmColorS_color_pixel(csw));
        num_args++;
    }

    XtSetArg(args[num_args], XmNlabelString, xm_str); num_args++;
    XtSetValues(XmColorS_color_window(csw), args, num_args);
    XmStringFree(xm_str);
}

/*
 *      Function Name: 	DefaultVisualDisplay
 *      Description:   	Determines the default visual and allocates
 *			the color depending upon the visual classes
 *      Arguments: 	csw - the color selector widget.
 *			foreground - default color for the ColorSelector.
 *			color - Current color attributes.
 *			str - label for the ColorSelector.
 *      Returns:       	Returns true on a valid visual class.
 *			False otherwise.
 */

static Boolean
DefaultVisualDisplay(XmColorSelectorWidget csw, Pixel foreground, XColor color, char *str)
{
    int visual = 0;
    visual = GetVisual(csw);

    /*
     * Obtain a valid color cell. In case, if one not available
     */
    if ( visual == StaticColor || visual == TrueColor || \
	 visual == StaticGray )
    {
	NoPrivateColormaps(csw, foreground, color, str);
	return True;
    } else if ( visual == PseudoColor || visual == DirectColor || \
		visual == GrayScale )
    {
	PrivateColormaps(csw, foreground, color, str);
	return True;
    } else
    {
	return False;
    }
}

/************************************************************
 *
 *  Public functions.
 *
 ************************************************************/

/*	Function Name: XmCreateColorSelector
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateColorSelector(Widget parent, String name,
		      ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmColorSelectorWidgetClass,
			  parent, args, num_args));
}

Widget
XmVaCreateColorSelector(
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
                         xmColorSelectorWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedColorSelector(
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
                         xmColorSelectorWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

static const struct ColorInfo color_info[N_COLORS] = {
/*    R    G    B     name                  */
    { 240, 248, 255, "alice blue"             },
    { 240, 248, 255, "Alice Blue"             },
    { 250, 235, 215, "antique white"          },
    { 250, 235, 215, "AntiqueWhite"           },
    { 255, 239, 219, "AntiqueWhite1"          },
    { 238, 223, 204, "AntiqueWhite2"          },
    { 205, 192, 176, "AntiqueWhite3"          },
    { 139, 131, 120, "AntiqueWhite4"          },
    {   0, 255, 255, "aqua"                   },
    { 127, 255, 212, "aquamarine"             },
    { 127, 255, 212, "aquamarine1"            },
    { 118, 238, 198, "aquamarine2"            },
    { 102, 205, 170, "aquamarine3"            },
    {  69, 139, 116, "aquamarine4"            },
    { 240, 255, 255, "azure"                  },
    { 240, 255, 255, "azure1"                 },
    { 224, 238, 238, "azure2"                 },
    { 193, 205, 205, "azure3"                 },
    { 131, 139, 139, "azure4"                 },
    { 245, 245, 220, "beige"                  },
    { 255, 228, 196, "bisque"                 },
    { 255, 228, 196, "bisque1"                },
    { 238, 213, 183, "bisque2"                },
    { 205, 183, 158, "bisque3"                },
    { 139, 125, 107, "bisque4"                },
    {   0,   0,   0, "black"                  },
    { 255, 235, 205, "blanched almond"        },
    { 255, 235, 205, "BlanchedAlmond"         },
    {   0,   0, 255, "blue"                   },
    { 138,  43, 226, "blue violet"            },
    {   0,   0, 255, "blue1"                  },
    {   0,   0, 238, "blue2"                  },
    {   0,   0, 205, "blue3"                  },
    {   0,   0, 139, "blue4"                  },
    { 138,  43, 226, "BlueViolet"             },
    { 165,  42,  42, "brown"                  },
    { 255,  64,  64, "brown1"                 },
    { 238,  59,  59, "brown2"                 },
    { 205,  51,  51, "brown3"                 },
    { 139,  35,  35, "brown4"                 },
    { 222, 184, 135, "burlywood"              },
    { 255, 211, 155, "burlywood1"             },
    { 238, 197, 145, "burlywood2"             },
    { 205, 170, 125, "burlywood3"             },
    { 139, 115,  85, "burlywood4"             },
    {  95, 158, 160, "cadet blue"             },
    {  95, 158, 160, "CadetBlue"              },
    { 152, 245, 255, "CadetBlue1"             },
    { 142, 229, 238, "CadetBlue2"             },
    { 122, 197, 205, "CadetBlue3"             },
    {  83, 134, 139, "CadetBlue4"             },
    { 127, 255,   0, "chartreuse"             },
    { 127, 255,   0, "chartreuse1"            },
    { 118, 238,   0, "chartreuse2"            },
    { 102, 205,   0, "chartreuse3"            },
    {  69, 139,   0, "chartreuse4"            },
    { 210, 105,  30, "chocolate"              },
    { 255, 127,  36, "chocolate1"             },
    { 238, 118,  33, "chocolate2"             },
    { 205, 102,  29, "chocolate3"             },
    { 139,  69,  19, "chocolate4"             },
    { 255, 127,  80, "coral"                  },
    { 255, 114,  86, "coral1"                 },
    { 238, 106,  80, "coral2"                 },
    { 205,  91,  69, "coral3"                 },
    { 139,  62,  47, "coral4"                 },
    { 100, 149, 237, "cornflower blue"        },
    { 100, 149, 237, "CornflowerBlue"         },
    { 255, 248, 220, "cornsilk"               },
    { 255, 248, 220, "cornsilk1"              },
    { 238, 232, 205, "cornsilk2"              },
    { 205, 200, 177, "cornsilk3"              },
    { 139, 136, 120, "cornsilk4"              },
    { 220,  20,  60, "crimson"                },
    {   0, 255, 255, "cyan"                   },
    {   0, 255, 255, "cyan1"                  },
    {   0, 238, 238, "cyan2"                  },
    {   0, 205, 205, "cyan3"                  },
    {   0, 139, 139, "cyan4"                  },
    {   0,   0, 139, "dark blue"              },
    {   0, 139, 139, "dark cyan"              },
    { 184, 134,  11, "dark goldenrod"         },
    { 169, 169, 169, "dark gray"              },
    {   0, 100,   0, "dark green"             },
    { 169, 169, 169, "dark grey"              },
    { 189, 183, 107, "dark khaki"             },
    { 139,   0, 139, "dark magenta"           },
    {  85, 107,  47, "dark olive green"       },
    { 255, 140,   0, "dark orange"            },
    { 153,  50, 204, "dark orchid"            },
    { 139,   0,   0, "dark red"               },
    { 233, 150, 122, "dark salmon"            },
    { 143, 188, 143, "dark sea green"         },
    {  72,  61, 139, "dark slate blue"        },
    {  47,  79,  79, "dark slate gray"        },
    {  47,  79,  79, "dark slate grey"        },
    {   0, 206, 209, "dark turquoise"         },
    { 148,   0, 211, "dark violet"            },
    {   0,   0, 139, "DarkBlue"               },
    {   0, 139, 139, "DarkCyan"               },
    { 184, 134,  11, "DarkGoldenrod"          },
    { 255, 185,  15, "DarkGoldenrod1"         },
    { 238, 173,  14, "DarkGoldenrod2"         },
    { 205, 149,  12, "DarkGoldenrod3"         },
    { 139, 101,   8, "DarkGoldenrod4"         },
    { 169, 169, 169, "DarkGray"               },
    {   0, 100,   0, "DarkGreen"              },
    { 169, 169, 169, "DarkGrey"               },
    { 189, 183, 107, "DarkKhaki"              },
    { 139,   0, 139, "DarkMagenta"            },
    {  85, 107,  47, "DarkOliveGreen"         },
    { 202, 255, 112, "DarkOliveGreen1"        },
    { 188, 238, 104, "DarkOliveGreen2"        },
    { 162, 205,  90, "DarkOliveGreen3"        },
    { 110, 139,  61, "DarkOliveGreen4"        },
    { 255, 140,   0, "DarkOrange"             },
    { 255, 127,   0, "DarkOrange1"            },
    { 238, 118,   0, "DarkOrange2"            },
    { 205, 102,   0, "DarkOrange3"            },
    { 139,  69,   0, "DarkOrange4"            },
    { 153,  50, 204, "DarkOrchid"             },
    { 191,  62, 255, "DarkOrchid1"            },
    { 178,  58, 238, "DarkOrchid2"            },
    { 154,  50, 205, "DarkOrchid3"            },
    { 104,  34, 139, "DarkOrchid4"            },
    { 139,   0,   0, "DarkRed"                },
    { 233, 150, 122, "DarkSalmon"             },
    { 143, 188, 143, "DarkSeaGreen"           },
    { 193, 255, 193, "DarkSeaGreen1"          },
    { 180, 238, 180, "DarkSeaGreen2"          },
    { 155, 205, 155, "DarkSeaGreen3"          },
    { 105, 139, 105, "DarkSeaGreen4"          },
    {  72,  61, 139, "DarkSlateBlue"          },
    {  47,  79,  79, "DarkSlateGray"          },
    { 151, 255, 255, "DarkSlateGray1"         },
    { 141, 238, 238, "DarkSlateGray2"         },
    { 121, 205, 205, "DarkSlateGray3"         },
    {  82, 139, 139, "DarkSlateGray4"         },
    {  47,  79,  79, "DarkSlateGrey"          },
    {   0, 206, 209, "DarkTurquoise"          },
    { 148,   0, 211, "DarkViolet"             },
    { 255,  20, 147, "deep pink"              },
    {   0, 191, 255, "deep sky blue"          },
    { 255,  20, 147, "DeepPink"               },
    { 255,  20, 147, "DeepPink1"              },
    { 238,  18, 137, "DeepPink2"              },
    { 205,  16, 118, "DeepPink3"              },
    { 139,  10,  80, "DeepPink4"              },
    {   0, 191, 255, "DeepSkyBlue"            },
    {   0, 191, 255, "DeepSkyBlue1"           },
    {   0, 178, 238, "DeepSkyBlue2"           },
    {   0, 154, 205, "DeepSkyBlue3"           },
    {   0, 104, 139, "DeepSkyBlue4"           },
    { 105, 105, 105, "dim gray"               },
    { 105, 105, 105, "dim grey"               },
    { 105, 105, 105, "DimGray"                },
    { 105, 105, 105, "DimGrey"                },
    {  30, 144, 255, "dodger blue"            },
    {  30, 144, 255, "DodgerBlue"             },
    {  30, 144, 255, "DodgerBlue1"            },
    {  28, 134, 238, "DodgerBlue2"            },
    {  24, 116, 205, "DodgerBlue3"            },
    {  16,  78, 139, "DodgerBlue4"            },
    { 178,  34,  34, "firebrick"              },
    { 255,  48,  48, "firebrick1"             },
    { 238,  44,  44, "firebrick2"             },
    { 205,  38,  38, "firebrick3"             },
    { 139,  26,  26, "firebrick4"             },
    { 255, 250, 240, "floral white"           },
    { 255, 250, 240, "FloralWhite"            },
    {  34, 139,  34, "forest green"           },
    {  34, 139,  34, "ForestGreen"            },
    { 255,   0, 255, "fuchsia"                },
    { 220, 220, 220, "gainsboro"              },
    { 248, 248, 255, "ghost white"            },
    { 248, 248, 255, "GhostWhite"             },
    { 255, 215,   0, "gold"                   },
    { 255, 215,   0, "gold1"                  },
    { 238, 201,   0, "gold2"                  },
    { 205, 173,   0, "gold3"                  },
    { 139, 117,   0, "gold4"                  },
    { 218, 165,  32, "goldenrod"              },
    { 255, 193,  37, "goldenrod1"             },
    { 238, 180,  34, "goldenrod2"             },
    { 205, 155,  29, "goldenrod3"             },
    { 139, 105,  20, "goldenrod4"             },
    { 190, 190, 190, "gray"                   },
    {   0,   0,   0, "gray0"                  },
    {   3,   3,   3, "gray1"                  },
    {  26,   26, 26, "gray10"                 },
    { 255, 255, 255, "gray100"                },
    {  28,  28,  28, "gray11"                 },
    {  31,  31,  31, "gray12"                 },
    {  33,  33,  33, "gray13"                 },
    {  36,  36,  36, "gray14"                 },
    {  38,  38,  38, "gray15"                 },
    {  41,  41,  41, "gray16"                 },
    {  43,  43,  43, "gray17"                 },
    {  46,  46,  46, "gray18"                 },
    {  48,  48,  48, "gray19"                 },
    {   5,   5,   5, "gray2"                  },
    {  51,  51,  51, "gray20"                 },
    {  54,  54,  54, "gray21"                 },
    {  56,  56,  56, "gray22"                 },
    {  59,  59,  59, "gray23"                 },
    {  61,  61,  61, "gray24"                 },
    {  64,  64,  64, "gray25"                 },
    {  66,  66,  66, "gray26"                 },
    {  69,  69,  69, "gray27"                 },
    {  71,  71,  71, "gray28"                 },
    {  74,  74,  74, "gray29"                 },
    {   8,   8,   8, "gray3"                  },
    {  77,  77,  77, "gray30"                 },
    {  79,  79,  79, "gray31"                 },
    {  82,  82,  82, "gray32"                 },
    {  84,  84,  84, "gray33"                 },
    {  87,  87,  87, "gray34"                 },
    {  89,  89,  89, "gray35"                 },
    {  92,  92,  92, "gray36"                 },
    {  94,  94,  94, "gray37"                 },
    {  97,  97,  97, "gray38"                 },
    {  99,  99,  99, "gray39"                 },
    {  10,  10,  10, "gray4"                  },
    { 102, 102, 102, "gray40"                 },
    { 105, 105, 105, "gray41"                 },
    { 107, 107, 107, "gray42"                 },
    { 110, 110, 110, "gray43"                 },
    { 112, 112, 112, "gray44"                 },
    { 115, 115, 115, "gray45"                 },
    { 117, 117, 117, "gray46"                 },
    { 120, 120, 120, "gray47"                 },
    { 122, 122, 122, "gray48"                 },
    { 125, 125, 125, "gray49"                 },
    {  13,  13,  13, "gray5"                  },
    { 127, 127, 127, "gray50"                 },
    { 130, 130, 130, "gray51"                 },
    { 133, 133, 133, "gray52"                 },
    { 135, 135, 135, "gray53"                 },
    { 138, 138, 138, "gray54"                 },
    { 140, 140, 140, "gray55"                 },
    { 143, 143, 143, "gray56"                 },
    { 145, 145, 145, "gray57"                 },
    { 148, 148, 148, "gray58"                 },
    { 150, 150, 150, "gray59"                 },
    {  15,  15,  15, "gray6"                  },
    { 153, 153, 153, "gray60"                 },
    { 156, 156, 156, "gray61"                 },
    { 158, 158, 158, "gray62"                 },
    { 161, 161, 161, "gray63"                 },
    { 163, 163, 163, "gray64"                 },
    { 166, 166, 166, "gray65"                 },
    { 168, 168, 168, "gray66"                 },
    { 171, 171, 171, "gray67"                 },
    { 173, 173, 173, "gray68"                 },
    { 176, 176, 176, "gray69"                 },
    {  18,  18,  18, "gray7"                  },
    { 179, 179, 179, "gray70"                 },
    { 181, 181, 181, "gray71"                 },
    { 184, 184, 184, "gray72"                 },
    { 186, 186, 186, "gray73"                 },
    { 189, 189, 189, "gray74"                 },
    { 191, 191, 191, "gray75"                 },
    { 194, 194, 194, "gray76"                 },
    { 196, 196, 196, "gray77"                 },
    { 199, 199, 199, "gray78"                 },
    { 201, 201, 201, "gray79"                 },
    {  20,  20,  20, "gray8"                  },
    { 204, 204, 204, "gray80"                 },
    { 207, 207, 207, "gray81"                 },
    { 209, 209, 209, "gray82"                 },
    { 212, 212, 212, "gray83"                 },
    { 214, 214, 214, "gray84"                 },
    { 217, 217, 217, "gray85"                 },
    { 219, 219, 219, "gray86"                 },
    { 222, 222, 222, "gray87"                 },
    { 224, 224, 224, "gray88"                 },
    { 227, 227, 227, "gray89"                 },
    {  23,  23,  23, "gray9"                  },
    { 229, 229, 229, "gray90"                 },
    { 232, 232, 232, "gray91"                 },
    { 235, 235, 235, "gray92"                 },
    { 237, 237, 237, "gray93"                 },
    { 240, 240, 240, "gray94"                 },
    { 242, 242, 242, "gray95"                 },
    { 245, 245, 245, "gray96"                 },
    { 247, 247, 247, "gray97"                 },
    { 250, 250, 250, "gray98"                 },
    { 252, 252, 252, "gray99"                 },
    {   0, 255,   0, "green"                  },
    { 173, 255,  47, "green yellow"           },
    {   0, 255,   0, "green1"                 },
    {   0, 238,   0, "green2"                 },
    {   0, 205,   0, "green3"                 },
    {   0, 139,   0, "green4"                 },
    { 173, 255,  47, "GreenYellow"            },
    { 190, 190, 190, "grey"                   },
    {   0,   0,   0, "grey0"                  },
    {   3,   3,   3, "grey1"                  },
    {  26,  26,  26, "grey10"                 },
    { 255, 255, 255, "grey100"                },
    {  28,  28,  28, "grey11"                 },
    {  31,  31,  31, "grey12"                 },
    {  33,  33,  33, "grey13"                 },
    {  36,  36,  36, "grey14"                 },
    {  38,  38,  38, "grey15"                 },
    {  41,  41,  41, "grey16"                 },
    {  43,  43,  43, "grey17"                 },
    {  46,  46,  46, "grey18"                 },
    {  48,  48,  48, "grey19"                 },
    {   5,   5,   5, "grey2"                  },
    {  51,  51,  51, "grey20"                 },
    {  54,  54,  54, "grey21"                 },
    {  56,  56,  56, "grey22"                 },
    {  59,  59,  59, "grey23"                 },
    {  61,  61,  61, "grey24"                 },
    {  64,  64,  64, "grey25"                 },
    {  66,  66,  66, "grey26"                 },
    {  69,  69,  69, "grey27"                 },
    {  71,  71,  71, "grey28"                 },
    {  74,  74,  74, "grey29"                 },
    {   8,   8,   8, "grey3"                  },
    {  77,  77,  77, "grey30"                 },
    {  79,  79,  79, "grey31"                 },
    {  82,  82,  82, "grey32"                 },
    {  84,  84,  84, "grey33"                 },
    {  87,  87,  87, "grey34"                 },
    {  89,  89,  89, "grey35"                 },
    {  92,  92,  92, "grey36"                 },
    {  94,  94,  94, "grey37"                 },
    {  97,  97,  97, "grey38"                 },
    {  99,  99,  99, "grey39"                 },
    {  10,  10,  10, "grey4"                  },
    { 102, 102, 102, "grey40"                 },
    { 105, 105, 105, "grey41"                 },
    { 107, 107, 107, "grey42"                 },
    { 110, 110, 110, "grey43"                 },
    { 112, 112, 112, "grey44"                 },
    { 115, 115, 115, "grey45"                 },
    { 117, 117, 117, "grey46"                 },
    { 120, 120, 120, "grey47"                 },
    { 122, 122, 122, "grey48"                 },
    { 125, 125, 125, "grey49"                 },
    {  13,  13,  13, "grey5"                  },
    { 127, 127, 127, "grey50"                 },
    { 130, 130, 130, "grey51"                 },
    { 133, 133, 133, "grey52"                 },
    { 135, 135, 135, "grey53"                 },
    { 138, 138, 138, "grey54"                 },
    { 140, 140, 140, "grey55"                 },
    { 143, 143, 143, "grey56"                 },
    { 145, 145, 145, "grey57"                 },
    { 148, 148, 148, "grey58"                 },
    { 150, 150, 150, "grey59"                 },
    {  15,  15,  15, "grey6"                  },
    { 153, 153, 153, "grey60"                 },
    { 156, 156, 156, "grey61"                 },
    { 158, 158, 158, "grey62"                 },
    { 161, 161, 161, "grey63"                 },
    { 163, 163, 163, "grey64"                 },
    { 166, 166, 166, "grey65"                 },
    { 168, 168, 168, "grey66"                 },
    { 171, 171, 171, "grey67"                 },
    { 173, 173, 173, "grey68"                 },
    { 176, 176, 176, "grey69"                 },
    {  18,  18,  18, "grey7"                  },
    { 179, 179, 179, "grey70"                 },
    { 181, 181, 181, "grey71"                 },
    { 184, 184, 184, "grey72"                 },
    { 186, 186, 186, "grey73"                 },
    { 189, 189, 189, "grey74"                 },
    { 191, 191, 191, "grey75"                 },
    { 194, 194, 194, "grey76"                 },
    { 196, 196, 196, "grey77"                 },
    { 199, 199, 199, "grey78"                 },
    { 201, 201, 201, "grey79"                 },
    {  20,  20,  20, "grey8"                  },
    { 204, 204, 204, "grey80"                 },
    { 207, 207, 207, "grey81"                 },
    { 209, 209, 209, "grey82"                 },
    { 212, 212, 212, "grey83"                 },
    { 214, 214, 214, "grey84"                 },
    { 217, 217, 217, "grey85"                 },
    { 219, 219, 219, "grey86"                 },
    { 222, 222, 222, "grey87"                 },
    { 224, 224, 224, "grey88"                 },
    { 227, 227, 227, "grey89"                 },
    {  23,  23,  23, "grey9"                  },
    { 229, 229, 229, "grey90"                 },
    { 232, 232, 232, "grey91"                 },
    { 235, 235, 235, "grey92"                 },
    { 237, 237, 237, "grey93"                 },
    { 240, 240, 240, "grey94"                 },
    { 242, 242, 242, "grey95"                 },
    { 245, 245, 245, "grey96"                 },
    { 247, 247, 247, "grey97"                 },
    { 250, 250, 250, "grey98"                 },
    { 252, 252, 252, "grey99"                 },
    { 240, 255, 240, "honeydew"               },
    { 240, 255, 240, "honeydew1"              },
    { 224, 238, 224, "honeydew2"              },
    { 193, 205, 193, "honeydew3"              },
    { 131, 139, 131, "honeydew4"              },
    { 255, 105, 180, "hot pink"               },
    { 255, 105, 180, "HotPink"                },
    { 255, 110, 180, "HotPink1"               },
    { 238, 106, 167, "HotPink2"               },
    { 205,  96, 144, "HotPink3"               },
    { 139,  58,  98, "HotPink4"               },
    { 205,  92,  92, "indian red"             },
    { 205,  92,  92, "IndianRed"              },
    { 255, 106, 106, "IndianRed1"             },
    { 238,  99,  99, "IndianRed2"             },
    { 205,  85,  85, "IndianRed3"             },
    { 139,  58,  58, "IndianRed4"             },
    {  75,   0, 130, "indigo"                 },
    { 255, 255, 240, "ivory"                  },
    { 255, 255, 240, "ivory1"                 },
    { 238, 238, 224, "ivory2"                 },
    { 205, 205, 193, "ivory3"                 },
    { 139, 139, 131, "ivory4"                 },
    { 240, 230, 140, "khaki"                  },
    { 255, 246, 143, "khaki1"                 },
    { 238, 230, 133, "khaki2"                 },
    { 205, 198, 115, "khaki3"                 },
    { 139, 134,  78, "khaki4"                 },
    { 230, 230, 250, "lavender"               },
    { 255, 240, 245, "lavender blush"         },
    { 255, 240, 245, "LavenderBlush"          },
    { 255, 240, 245, "LavenderBlush1"         },
    { 238, 224, 229, "LavenderBlush2"         },
    { 205, 193, 197, "LavenderBlush3"         },
    { 139, 131, 134, "LavenderBlush4"         },
    { 124, 252,   0, "lawn green"             },
    { 124, 252,   0, "LawnGreen"              },
    { 255, 250, 205, "lemon chiffon"          },
    { 255, 250, 205, "LemonChiffon"           },
    { 255, 250, 205, "LemonChiffon1"          },
    { 238, 233, 191, "LemonChiffon2"          },
    { 205, 201, 165, "LemonChiffon3"          },
    { 139, 137, 112, "LemonChiffon4"          },
    { 173, 216, 230, "light blue"             },
    { 240, 128, 128, "light coral"            },
    { 224, 255, 255, "light cyan"             },
    { 238, 221, 130, "light goldenrod"        },
    { 250, 250, 210, "light goldenrod yellow" },
    { 211, 211, 211, "light gray"             },
    { 144, 238, 144, "light green"            },
    { 211, 211, 211, "light grey"             },
    { 255, 182, 193, "light pink"             },
    { 255, 160, 122, "light salmon"           },
    {  32, 178, 170, "light sea green"        },
    { 135, 206, 250, "light sky blue"         },
    { 132, 112, 255, "light slate blue"       },
    { 119, 136, 153, "light slate gray"       },
    { 119, 136, 153, "light slate grey"       },
    { 176, 196, 222, "light steel blue"       },
    { 255, 255, 224, "light yellow"           },
    { 173, 216, 230, "LightBlue"              },
    { 191, 239, 255, "LightBlue1"             },
    { 178, 223, 238, "LightBlue2"             },
    { 154, 192, 205, "LightBlue3"             },
    { 104, 131, 139, "LightBlue4"             },
    { 240, 128, 128, "LightCoral"             },
    { 224, 255, 255, "LightCyan"              },
    { 224, 255, 255, "LightCyan1"             },
    { 209, 238, 238, "LightCyan2"             },
    { 180, 205, 205, "LightCyan3"             },
    { 122, 139, 139, "LightCyan4"             },
    { 238, 221, 130, "LightGoldenrod"         },
    { 255, 236, 139, "LightGoldenrod1"        },
    { 238, 220, 130, "LightGoldenrod2"        },
    { 205, 190, 112, "LightGoldenrod3"        },
    { 139, 129,  76, "LightGoldenrod4"        },
    { 250, 250, 210, "LightGoldenrodYellow"   },
    { 211, 211, 211, "LightGray"              },
    { 144, 238, 144, "LightGreen"             },
    { 211, 211, 211, "LightGrey"              },
    { 255, 182, 193, "LightPink"              },
    { 255, 174, 185, "LightPink1"             },
    { 238, 162, 173, "LightPink2"             },
    { 205, 140, 149, "LightPink3"             },
    { 139,  95, 101, "LightPink4"             },
    { 255, 160, 122, "LightSalmon"            },
    { 255, 160, 122, "LightSalmon1"           },
    { 238, 149, 114, "LightSalmon2"           },
    { 205, 129,  98, "LightSalmon3"           },
    { 139,  87,  66, "LightSalmon4"           },
    {  32, 178, 170, "LightSeaGreen"          },
    { 135, 206, 250, "LightSkyBlue"           },
    { 176, 226, 255, "LightSkyBlue1"          },
    { 164, 211, 238, "LightSkyBlue2"          },
    { 141, 182, 205, "LightSkyBlue3"          },
    {  96, 123, 139, "LightSkyBlue4"          },
    { 132, 112, 255, "LightSlateBlue"         },
    { 119, 136, 153, "LightSlateGray"         },
    { 119, 136, 153, "LightSlateGrey"         },
    { 176, 196, 222, "LightSteelBlue"         },
    { 202, 225, 255, "LightSteelBlue1"        },
    { 188, 210, 238, "LightSteelBlue2"        },
    { 162, 181, 205, "LightSteelBlue3"        },
    { 110, 123, 139, "LightSteelBlue4"        },
    { 255, 255, 224, "LightYellow"            },
    { 255, 255, 224, "LightYellow1"           },
    { 238, 238, 209, "LightYellow2"           },
    { 205, 205, 180, "LightYellow3"           },
    { 139, 139, 122, "LightYellow4"           },
    {   0, 255,   0, "lime"                   },
    {  50, 205,  50, "lime green"             },
    {  50, 205,  50, "LimeGreen"              },
    { 250, 240, 230, "linen"                  },
    { 255,  0,  255, "magenta"                },
    { 255,  0,  255, "magenta1"               },
    { 238,  0,  238, "magenta2"               },
    { 205,  0,  205, "magenta3"               },
    { 139,  0,  139, "magenta4"               },
    { 176,  48,  96, "maroon"                 },
    { 255,  52, 179, "maroon1"                },
    { 238,  48, 167, "maroon2"                },
    { 205,  41, 144, "maroon3"                },
    { 139,  28,  98, "maroon4"                },
    { 102, 205, 170, "medium aquamarine"      },
    {   0,   0, 205, "medium blue"            },
    { 186,  85, 211, "medium orchid"          },
    { 147, 112, 219, "medium purple"          },
    {  60, 179, 113, "medium sea green"       },
    { 123, 104, 238, "medium slate blue"      },
    {   0, 250, 154, "medium spring green"    },
    {  72, 209, 204, "medium turquoise"       },
    { 199,  21, 133, "medium violet red"      },
    { 102, 205, 170, "MediumAquamarine"       },
    {   0,   0, 205, "MediumBlue"             },
    { 186,  85, 211, "MediumOrchid"           },
    { 224, 102, 255, "MediumOrchid1"          },
    { 209,  95, 238, "MediumOrchid2"          },
    { 180,  82, 205, "MediumOrchid3"          },
    { 122,  55, 139, "MediumOrchid4"          },
    { 147, 112, 219, "MediumPurple"           },
    { 171, 130, 255, "MediumPurple1"          },
    { 159, 121, 238, "MediumPurple2"          },
    { 137, 104, 205, "MediumPurple3"          },
    {  93,  71, 139, "MediumPurple4"          },
    {  60, 179, 113, "MediumSeaGreen"         },
    { 123, 104, 238, "MediumSlateBlue"        },
    {   0, 250, 154, "MediumSpringGreen"      },
    {  72, 209, 204, "MediumTurquoise"        },
    { 199,  21, 133, "MediumVioletRed"        },
    {  25,  25, 112, "midnight blue"          },
    {  25,  25, 112, "MidnightBlue"           },
    { 245, 255, 250, "mint cream"             },
    { 245, 255, 250, "MintCream"              },
    { 255, 228, 225, "misty rose"             },
    { 255, 228, 225, "MistyRose"              },
    { 255, 228, 225, "MistyRose1"             },
    { 238, 213, 210, "MistyRose2"             },
    { 205, 183, 181, "MistyRose3"             },
    { 139, 125, 123, "MistyRose4"             },
    { 255, 228, 181, "moccasin"               },
    { 255, 222, 173, "navajo white"           },
    { 255, 222, 173, "NavajoWhite"            },
    { 255, 222, 173, "NavajoWhite1"           },
    { 238, 207, 161, "NavajoWhite2"           },
    { 205, 179, 139, "NavajoWhite3"           },
    { 139, 121,  94, "NavajoWhite4"           },
    {   0,   0, 128, "navy"                   },
    {   0,   0, 128, "navy blue"              },
    {   0,   0, 128, "NavyBlue"               },
    { 253, 245, 230, "old lace"               },
    { 253, 245, 230, "OldLace"                },
    { 128, 128,   0, "olive"                  },
    { 107, 142,  35, "olive drab"             },
    { 107, 142,  35, "OliveDrab"              },
    { 192, 255,  62, "OliveDrab1"             },
    { 179, 238,  58, "OliveDrab2"             },
    { 154, 205,  50, "OliveDrab3"             },
    { 105, 139,  34, "OliveDrab4"             },
    { 255, 165,   0, "orange"                 },
    { 255,  69,   0, "orange red"             },
    { 255, 165,   0, "orange1"                },
    { 238, 154,   0, "orange2"                },
    { 205, 133,   0, "orange3"                },
    { 139,  90,   0, "orange4"                },
    { 255,  69,   0, "OrangeRed"              },
    { 255,  69,   0, "OrangeRed1"             },
    { 238,  64,   0, "OrangeRed2"             },
    { 205,  55,   0, "OrangeRed3"             },
    { 139,  37,   0, "OrangeRed4"             },
    { 218, 112, 214, "orchid"                 },
    { 255, 131, 250, "orchid1"                },
    { 238, 122, 233, "orchid2"                },
    { 205, 105, 201, "orchid3"                },
    { 139,  71, 137, "orchid4"                },
    { 238, 232, 170, "pale goldenrod"         },
    { 152, 251, 152, "pale green"             },
    { 175, 238, 238, "pale turquoise"         },
    { 219, 112, 147, "pale violet red"        },
    { 238, 232, 170, "PaleGoldenrod"          },
    { 152, 251, 152, "PaleGreen"              },
    { 154, 255, 154, "PaleGreen1"             },
    { 144, 238, 144, "PaleGreen2"             },
    { 124, 205, 124, "PaleGreen3"             },
    {  84, 139,  84, "PaleGreen4"             },
    { 175, 238, 238, "PaleTurquoise"          },
    { 187, 255, 255, "PaleTurquoise1"         },
    { 174, 238, 238, "PaleTurquoise2"         },
    { 150, 205, 205, "PaleTurquoise3"         },
    { 102, 139, 139, "PaleTurquoise4"         },
    { 219, 112, 147, "PaleVioletRed"          },
    { 255, 130, 171, "PaleVioletRed1"         },
    { 238, 121, 159, "PaleVioletRed2"         },
    { 205, 104, 137, "PaleVioletRed3"         },
    { 139,  71,  93, "PaleVioletRed4"         },
    { 255, 239, 213, "papaya whip"            },
    { 255, 239, 213, "PapayaWhip"             },
    { 255, 218, 185, "peach puff"             },
    { 255, 218, 185, "PeachPuff"              },
    { 255, 218, 185, "PeachPuff1"             },
    { 238, 203, 173, "PeachPuff2"             },
    { 205, 175, 149, "PeachPuff3"             },
    { 139, 119, 101, "PeachPuff4"             },
    { 205, 133,  63, "peru"                   },
    { 255, 192, 203, "pink"                   },
    { 255, 181, 197, "pink1"                  },
    { 238, 169, 184, "pink2"                  },
    { 205, 145, 158, "pink3"                  },
    { 139,  99, 108, "pink4"                  },
    { 221, 160, 221, "plum"                   },
    { 255, 187, 255, "plum1"                  },
    { 238, 174, 238, "plum2"                  },
    { 205, 150, 205, "plum3"                  },
    { 139, 102, 139, "plum4"                  },
    { 176, 224, 230, "powder blue"            },
    { 176, 224, 230, "PowderBlue"             },
    { 160,  32, 240, "purple"                 },
    { 155,  48, 255, "purple1"                },
    { 145,  44, 238, "purple2"                },
    { 125,  38, 205, "purple3"                },
    {  85,  26, 139, "purple4"                },
    { 102,  51, 153, "rebecca purple"         },
    { 102,  51, 153, "RebeccaPurple"          },
    { 255,   0,   0, "red"                    },
    { 255,   0,   0, "red1"                   },
    { 238,   0,   0, "red2"                   },
    { 205,   0,   0, "red3"                   },
    { 139,   0,   0, "red4"                   },
    { 188, 143, 143, "rosy brown"             },
    { 188, 143, 143, "RosyBrown"              },
    { 255, 193, 193, "RosyBrown1"             },
    { 238, 180, 180, "RosyBrown2"             },
    { 205, 155, 155, "RosyBrown3"             },
    { 139, 105, 105, "RosyBrown4"             },
    {  65, 105, 225, "royal blue"             },
    {  65, 105, 225, "RoyalBlue"              },
    {  72, 118, 255, "RoyalBlue1"             },
    {  67, 110, 238, "RoyalBlue2"             },
    {  58,  95, 205, "RoyalBlue3"             },
    {  39,  64, 139, "RoyalBlue4"             },
    { 139,  69,  19, "saddle brown"           },
    { 139,  69,  19, "SaddleBrown"            },
    { 250, 128, 114, "salmon"                 },
    { 255, 140, 105, "salmon1"                },
    { 238, 130,  98, "salmon2"                },
    { 205, 112,  84, "salmon3"                },
    { 139,  76,  57, "salmon4"                },
    { 244, 164,  96, "sandy brown"            },
    { 244, 164,  96, "SandyBrown"             },
    {  46, 139,  87, "sea green"              },
    {  46, 139,  87, "SeaGreen"               },
    {  84, 255, 159, "SeaGreen1"              },
    {  78, 238, 148, "SeaGreen2"              },
    {  67, 205, 128, "SeaGreen3"              },
    {  46, 139,  87, "SeaGreen4"              },
    { 255, 245, 238, "seashell"               },
    { 255, 245, 238, "seashell1"              },
    { 238, 229, 222, "seashell2"              },
    { 205, 197, 191, "seashell3"              },
    { 139, 134, 130, "seashell4"              },
    { 160,  82,  45, "sienna"                 },
    { 255, 130,  71, "sienna1"                },
    { 238, 121,  66, "sienna2"                },
    { 205, 104,  57, "sienna3"                },
    { 139,  71,  38, "sienna4"                },
    { 192, 192, 192, "silver"                 },
    { 135, 206, 235, "sky blue"               },
    { 135, 206, 235, "SkyBlue"                },
    { 135, 206, 255, "SkyBlue1"               },
    { 126, 192, 238, "SkyBlue2"               },
    { 108, 166, 205, "SkyBlue3"               },
    {  74, 112, 139, "SkyBlue4"               },
    { 106,  90, 205, "slate blue"             },
    { 112, 128, 144, "slate gray"             },
    { 112, 128, 144, "slate grey"             },
    { 106,  90, 205, "SlateBlue"              },
    { 131, 111, 255, "SlateBlue1"             },
    { 122, 103, 238, "SlateBlue2"             },
    { 105,  89, 205, "SlateBlue3"             },
    {  71,  60, 139, "SlateBlue4"             },
    { 112, 128, 144, "SlateGray"              },
    { 198, 226, 255, "SlateGray1"             },
    { 185, 211, 238, "SlateGray2"             },
    { 159, 182, 205, "SlateGray3"             },
    { 108, 123, 139, "SlateGray4"             },
    { 112, 128, 144, "SlateGrey"              },
    { 255, 250, 250, "snow"                   },
    { 255, 250, 250, "snow1"                  },
    { 238, 233, 233, "snow2"                  },
    { 205, 201, 201, "snow3"                  },
    { 139, 137, 137, "snow4"                  },
    {   0, 255, 127, "spring green"           },
    {   0, 255, 127, "SpringGreen"            },
    {   0, 255, 127, "SpringGreen1"           },
    {   0, 238, 118, "SpringGreen2"           },
    {   0, 205, 102, "SpringGreen3"           },
    {   0, 139,  69, "SpringGreen4"           },
    {  70, 130, 180, "steel blue"             },
    {  70, 130, 180, "SteelBlue"              },
    {  99, 184, 255, "SteelBlue1"             },
    {  92, 172, 238, "SteelBlue2"             },
    {  79, 148, 205, "SteelBlue3"             },
    {  54, 100, 139, "SteelBlue4"             },
    { 210, 180, 140, "tan"                    },
    { 255, 165,  79, "tan1"                   },
    { 238, 154,  73, "tan2"                   },
    { 205, 133,  63, "tan3"                   },
    { 139,  90,  43, "tan4"                   },
    {   0, 128, 128, "teal"                   },
    { 216, 191, 216, "thistle"                },
    { 255, 225, 255, "thistle1"               },
    { 238, 210, 238, "thistle2"               },
    { 205, 181, 205, "thistle3"               },
    { 139, 123, 139, "thistle4"               },
    { 255,  99,  71, "tomato"                 },
    { 255,  99,  71, "tomato1"                },
    { 238,  92,  66, "tomato2"                },
    { 205,  79,  57, "tomato3"                },
    { 139,  54,  38, "tomato4"                },
    {  64, 224, 208, "turquoise"              },
    {   0, 245, 255, "turquoise1"             },
    {   0, 229, 238, "turquoise2"             },
    {   0, 197, 205, "turquoise3"             },
    {   0, 134, 139, "turquoise4"             },
    { 238, 130, 238, "violet"                 },
    { 208,  32, 144, "violet red"             },
    { 208,  32, 144, "VioletRed"              },
    { 255,  62, 150, "VioletRed1"             },
    { 238,  58, 140, "VioletRed2"             },
    { 205,  50, 120, "VioletRed3"             },
    { 139,  34,  82, "VioletRed4"             },
    { 128, 128, 128, "web gray"               },
    {   0, 128,   0, "web green"              },
    { 128, 128, 128, "web grey"               },
    { 128,   0,   0, "web maroon"             },
    { 128,   0, 128, "web purple"             },
    { 128, 128, 128, "WebGray"                },
    {  0,  128,   0, "WebGreen"               },
    { 128, 128, 128, "WebGrey"                },
    { 128,   0,   0, "WebMaroon"              },
    { 128,   0, 128, "WebPurple"              },
    { 245, 222, 179, "wheat"                  },
    { 255, 231, 186, "wheat1"                 },
    { 238, 216, 174, "wheat2"                 },
    { 205, 186, 150, "wheat3"                 },
    { 139, 126, 102, "wheat4"                 },
    { 255, 255, 255, "white"                  },
    { 245, 245, 245, "white smoke"            },
    { 245, 245, 245, "WhiteSmoke"             },
    { 190, 190, 190, "x11 gray"               },
    {   0, 255,   0, "x11 green"              },
    { 190, 190, 190, "x11 grey"               },
    { 176,  48,  96, "x11 maroon"             },
    { 160,  32, 240, "x11 purple"             },
    { 190, 190, 190, "X11Gray"                },
    {   0, 255,   0, "X11Green"               },
    { 190, 190, 190, "X11Grey"                },
    { 176,  48,  96, "X11Maroon"              },
    { 160,  32, 240, "X11Purple"              },
    { 255, 255,   0, "yellow"                 },
    { 154, 205,  50, "yellow green"           },
    { 255, 255,   0, "yellow1"                },
    { 238, 238,   0, "yellow2"                },
    { 205, 205,   0, "yellow3"                },
    { 139, 139,   0, "yellow4"                },
    { 154, 205,  50, "YellowGreen"            },
};

