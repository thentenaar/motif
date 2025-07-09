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
static void read_rgb_file(XmColorSelectorWidget, ArgList, Cardinal, Boolean);
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
#ifdef VMS    
  {
    XmNrgbFile, XmCString, XmRString,
    sizeof(String), XtOffsetOf(XmColorSelectorRec, cs.rgb_file),
    XmRString, (XtPointer) "sys$manager:decw$rgb.dat"
  },
#else
  {
    XmNrgbFile, XmCString, XmRString,
    sizeof(String), XtOffsetOf(XmColorSelectorRec, cs.rgb_file),
    XmRString, (XtPointer) "/usr/lib/X11/rgb.txt"
  },
#endif
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
  },

  {
    XmNfileReadError, XmCFileReadError, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmColorSelectorRec, cs.strings.file_read_error),
    XmRString, (XtPointer)"Could not read rgb.txt file:"
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
static void 
#ifdef _NO_PROTO
ClassPartInitialize(w_class)
        WidgetClass w_class ;
#else
ClassPartInitialize(WidgetClass w_class)
#endif /* _NO_PROTO */
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

/* ARGSUSED */
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

    XmColorS_rgb_file(csw) = XtNewString(XmColorS_rgb_file(csw));
    XmColorS_colors(csw) = NULL;
    read_rgb_file(csw, f_args, f_num_args, True);

    if (!color_name_changed(csw, temp)) {
        snprintf(message_buffer, BUFSIZ, XmNunparsableColorMsg, temp);
	XmeWarning((Widget)set, message_buffer);

	(void) color_name_changed(csw, "White");
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

/* ARGSUSED */
static void	
Destroy(Widget w)
{
    XmColorSelectorWidget	csw = (XmColorSelectorWidget)w;

    if (XmColorS_good_cell(csw)) {
    	XFreeColors(XtDisplay(csw), csw->core.colormap,
		    &XmColorS_color_pixel(csw), 1, 0);
    }

    XtFree((char*) XmColorS_colors(csw));
    XtFree((char*) XmColorS_color_name(csw));
    XtFree((char*) XmColorS_rgb_file(csw));

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

/* ARGSUSED */
static void	
Resize(Widget w)
{
    compute_size((XmColorSelectorWidget)w);
}

static Boolean AreDiff(char *s1, char *s2)
{
	if (s1 && !s2)	return True;
	if (s2 && !s1)	return True;
	if (!s1 && !s2)	return False;
	/* they exist; now safe to do strcmp */
	return strcmp(s1, s2);
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

/* ARGSUSED */
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

    /* 
    ** Don't compare pointers; they are allocated, so passing the same file
    ** in twice will trip this expensive function unless we compare the 
    ** values of the strings (when they exist)
    */
    if (AreDiff(XmColorS_rgb_file(curr), XmColorS_rgb_file(csw)))
    {
	read_rgb_file(csw, NULL, 0, False);
    }
    if (XmColorS_rgb_file(curr) != XmColorS_rgb_file(csw))
    {
    	XtFree((char*) XmColorS_rgb_file(curr));
        XmColorS_rgb_file(csw) = XtNewString(XmColorS_rgb_file(csw));
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

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
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
    register char *c = str;
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
    register ColorInfo *ptr;
    register int i, red, green, blue;
    
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

		if(streq(XmColorS_color_name(csw), ptr->name) ||
		   streq(XmColorS_color_name(csw), ptr->no_space_lower_name)) 
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

/* ARGSUSED */
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
    char buf[XmColorSelector_COLOR_NAME_SIZE], new_label[BUFSIZ];

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

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
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

/*      Function Name: 	read_rgb_file
 *      Description:   	Read in all the color names and add them to the list.
 *      Arguments: 	csw - the color selector.
 *                      cargs, cnum_args - a filtered arg list that was
 *                                       passed to create the color selector.
 *      Returns:       	none.
 */

/* ARGSUSED */
static void
read_rgb_file(XmColorSelectorWidget csw, ArgList cargs, Cardinal cnum_args, Boolean initial)
{
    FILE *file;
    char buf[BUFSIZ];
    char string_buffer[BUFSIZ + (BUFSIZ >> 1)];
    char *color_name;
    ColorInfo * color_info = NULL;
    register int i;
    Arg	*margs, args[20];
 
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
    XtVaSetValues(XmColorS_list(csw),
	XmNselectedItems, NULL, XmNselectedItemCount, 0, NULL);

    /*
     * Read in all the colornames.
     */
    if ((file = fopen(XmColorS_rgb_file(csw), "r")) != NULL) {
	register int alloc, count, len;
	register char *name;

	alloc = count = 0;

    	while(fgets(buf, BUFSIZ, file)) {
	    /*
	     * Skip any comment lines in the file
	     */
	    if ( buf[0] == '!' ) continue;
	    
	    if (count >= alloc) {
		if (0 == alloc)
			alloc = 755;	/* rather than stat the file and determine a good value to use, just use enough for X11R5, X11R6, and OpenWindows3 */
		else
			{
			alloc += 20;
			}
		color_info = (ColorInfo *)XtRealloc((XtPointer) color_info,
						    sizeof(ColorInfo) * alloc);
	    }

	    sscanf(buf, "%hu %hu %hu", &(color_info[count].red),
		   &(color_info[count].green), &(color_info[count].blue));

	    if ((color_name = find_name(buf)) == NULL)
		continue;

	    len = strlen(color_name);
	    if (len > XmColorSelector_COLOR_NAME_SIZE) {
		color_name[XmColorSelector_COLOR_NAME_SIZE - 1] = '\0';
		snprintf(string_buffer, sizeof string_buffer,
			 XmNcolorNameTooLongMsg, buf, color_name);
		XmeWarning((Widget)csw, string_buffer);
	    }

	    name = color_info[count].no_space_lower_name;
	    for (i = 0; i < len; i++) {
		register char c = color_name[i];

		/*
		 * Copy in all characters that are ascii and non-spaces.
		 */
		if (!isascii(c))
		    continue;
		if (!isspace(c))
		    *name++ = tolower(c);
	    }
	    *name = '\0';

	    name = color_info[count].name;
	    color_name[0] = toupper(color_name[0]);
	    for (i = 0; i < len; i++) {
		register char c = color_name[i];

		/*
		 * Capitalize all characters after a space.
		 */

		if (!isascii(c))
		    continue;
		if (isspace(c) && ((i + 1) < len)) {
		    color_name[i + 1] = toupper(color_name[i + 1]);
		}

		*name++ = c;
	    }
	    *name = '\0';

	    count++;
	}
	fclose(file);
	    
	qsort(color_info, count, sizeof(ColorInfo), CmpColors);

	/*
	 * Remove duplicates.
	 */
	i = 0;
	while (i < (count - 1)) {
	    if (streq(color_info[i].no_space_lower_name,
		      color_info[i + 1].no_space_lower_name))
	    {
		register int j;
		register ColorInfo *ptr;

		ptr = color_info + i;
		j = i;

		/*
		 * This makes sure that the one with the space is
		 * left in place in favor of the one without.
		 */
		if (strchr(ptr->name, ' ') != NULL) {
		    j++;
		    ptr++;
		}

		while ( ++j < count) {
		    ptr[0] = ptr[1];
		    ptr++;
		}

		count--;	/*Something has been removed, decrement count*/
	    }
	    else
		i++;
	}

	{
		XmString *strs = (XmString*)XtMalloc(sizeof(XmString)*count);
		for (i = 0; i < count; i++) 
			strs[i] = XmStringCreateLocalized(color_info[i].name);
		XtVaSetValues(XmColorS_list(csw),
			XmNitems, strs,
			XmNitemCount, count, 
			NULL);
		for (i = 0; i < count; i++) 
			XmStringFree(strs[i]);
	        XtFree((char*)strs);
	}

	XtFree((char*)XmColorS_colors(csw));
	XmColorS_colors(csw) = color_info;
	XmColorS_num_colors(csw) = count;

	/* It would be better if we had cached the current index number so
	** we could just reset the list to the string corresponding to that 
	** value, but instead wind up going through FindColor to reestablish
	** the selected string
	*/
	if (!initial)
		SelectColor(csw);
    }
    else {
	XmString 	str;

	XmListAddItem(XmColorS_list(csw), XmColorS_strings(csw).file_read_error, 0);

	str = XmStringCreateLocalized(XmColorS_rgb_file(csw));
	XmListAddItem(XmColorS_list(csw), str, 0);
	XmStringFree(str);

	XtFree((char*)XmColorS_colors(csw));
	XmColorS_colors(csw) = NULL;
	XmColorS_num_colors(csw) = 0;
    }

    XtAddCallback(XmColorS_list(csw), XmNsingleSelectionCallback,
		  list_selected, csw);	
    XtAddCallback(XmColorS_list(csw), XmNbrowseSelectionCallback,
		  list_selected, csw);	
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

    return(strcmp(color1->no_space_lower_name, color2->no_space_lower_name));
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
    register char *curr, *temp;	/* current pointer */

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

/* ARGSUSED */
static void
CreateColorSliders(XmColorSelectorWidget csw, 
		   ArgList cargs, Cardinal cnum_args)
{
    register int i;
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

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
static void GetValues_XmNredSliderLabel ( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[0]);
}
/* ARGSUSED */
static void GetValues_XmNgreenSliderLabel( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[1]);
}
/* ARGSUSED */
static void GetValues_XmNblueSliderLabel( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).slider_labels[2]);
}
/* ARGSUSED */
static void GetValues_XmNcolorListTogLabel( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).tog_labels[0]);
}
/* ARGSUSED */
static void GetValues_XmNsliderTogLabel( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).tog_labels[1]);
}
/* ARGSUSED */
static void GetValues_XmNnoCellError( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).no_cell_error);
}
/* ARGSUSED */
static void GetValues_XmNfileReadError( Widget w, int n, XtArgVal *value) 
{
    (*value) = (XtArgVal) XmStringCopy(XmColorS_strings(w).file_read_error);
}

/*      Function Name: 	GetVisual
 *      Description:   	Gets the defaults visual of the screen
 *      Arguments: 	csw - the color selector widget.
 *      Returns:       	Visual id.
 */

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
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

/* ARGSUSED */
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
    register Widget w;
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
