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
#include <Xm/Xm.h>
#include <stdio.h>
#include <Xm/ButtonBoxP.h>
#include <Xm/ExtP.h>
#include <Xm/TraitP.h>
#include <Xm/TakesDefT.h>
#include <Xm/VaSimpleP.h>
#include "XmI.h"

/************************************************************
 *       TYPEDEFS AND DEFINES
 ************************************************************/

#define SUPERCLASS ((ConstraintWidgetClass) &xmManagerClassRec)

/************************************************************
 *       MACROS
 ************************************************************/

#define IsHorizontal(x) (XmButtonBox_orientation((x)) == XmHORIZONTAL)
#define MarginH(x)	(XmButtonBox_margin_height((x)))
#define MarginW(x)	(XmButtonBox_margin_width((x)))
#define FillOption(x)	(XmButtonBox_fill_option((x)))
#define EqualSize(x)	(XmButtonBox_equal_size((x)))

/************************************************************
 *       EXTERNAL DECLARATIONS
 ************************************************************/

/************************************************************
 *       STATIC FUNCTION DECLARATIONS
 ************************************************************/

static void 	Initialize(Widget, Widget, ArgList, Cardinal *);
static void	    ClassInitialize(void);

static void     ClassPartInitialize(WidgetClass w_class) ;

static void	    InsertChild(Widget);
static void     Resize(Widget), ChangeManaged(Widget);
static void 	ConstraintInitialize(Widget, Widget, ArgList, Cardinal *);

static Boolean  SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static Boolean  ConstraintSetValues(Widget, Widget,
				    Widget, ArgList, Cardinal *);
static Boolean CvtStringToFillOption(Display *, XrmValuePtr, Cardinal *,
				     XrmValuePtr, XrmValuePtr);

static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
					XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
				      XtWidgetGeometry *);

static XtGeometryResult TryNewLayout(Widget, Mask *, Boolean);
static Cardinal	       	CalcChildrenPrefSizes(XmButtonBoxWidget, Dimension *,
					      Dimension *, Dimension *);
static void		LayoutChildren(Widget, Widget);
static void	 	CalcChildSize(XmButtonBoxWidget, Widget, Dimension,
				      Dimension, Dimension, Cardinal,
				      Dimension *, Dimension *);

static void ButtonBoxSetDefaultShadow(Widget button) ;


/************************************************************
 *       STATIC DECLARATIONS
 ************************************************************/

static XtResource resources[] =
{
  {
    XmNequalSize, XmCEqualSize, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmButtonBoxRec, button_box.equal_size),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNfillOption, XmCFillOption, XmRXmFillOption,
    sizeof(XmFillOption), XtOffsetOf(XmButtonBoxRec, button_box.fill_option),
    XmRImmediate, (XtPointer) XmFillNone
  },

  {
    XmNmarginHeight, XmCMargin, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmButtonBoxRec, button_box.margin_height),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNmarginWidth, XmCMargin, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmButtonBoxRec, button_box.margin_width),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNorientation, XmCOrientation, XmROrientation,
    sizeof(unsigned char), XtOffsetOf(XmButtonBoxRec, button_box.orientation),
    XmRImmediate, (XtPointer)XmHORIZONTAL
  },

  {
    XmNdefaultButton, XmCWidget, XmRWidget,
    sizeof(Widget), XtOffsetOf(XmButtonBoxRec, button_box.default_button),
    XmRImmediate, (XtPointer) NULL
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNmarginHeight, sizeof(Dimension),
    XtOffsetOf(XmButtonBoxRec, button_box.margin_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNmarginWidth, sizeof(Dimension),
    XtOffsetOf(XmButtonBoxRec, button_box.margin_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

static XtResource constraints[] =
{
   {
      "pri.vate1", "Pri.vate1", XmRDimension, sizeof(Dimension),
      XtOffsetOf(XmBBoxConstraintsRec, bbox.pref_width),
      XmRImmediate, (XtPointer) 0
   },
   {
      "pri.vate2", "Pri.vate2", XmRAttachment, sizeof(Dimension),
      XtOffsetOf(XmBBoxConstraintsRec, bbox.pref_height),
      XmRImmediate, (XtPointer) 0
   }
};

XmButtonBoxClassRec xmButtonBoxClassRec = {
  {
    /* core_class members      */
    /* superclass         */	(WidgetClass)SUPERCLASS,
    /* class_name         */	"XmButtonBox",
    /* widget_size        */	sizeof(XmButtonBoxRec),
    /* class_initialize   */	ClassInitialize,
    /* class_part_init    */	ClassPartInitialize,
    /* class_inited       */	False,
    /* initialize         */	Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */	XtInheritRealize,
    /* actions            */	NULL,


    /* num_actions        */	0,
    /* resources          */	(XtResource*)resources,
    /* num_resources      */	XtNumber(resources),
    /* xrm_class          */	NULLQUARK,
    /* compress_motion    */	True,
    /* compress_exposure  */	True,
    /* compress_enterleave*/	True,
    /* visible_interest   */	False,
    /* destroy            */	NULL,
    /* resize             */	Resize,
    /* expose             */	NULL,
    /* set_values         */	SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */	XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */	NULL,
    /* version            */	XtVersion,
    /* callback_private   */	NULL,
    /* tm_table           */	XtInheritTranslations,
    /* query_geometry     */	(XtGeometryHandler) QueryGeometry,
    /* display_accelerator*/	NULL,
    /* extension          */	NULL,
  },
  {
    /* composite_class members */
    /* geometry_manager   */	GeometryManager,
    /* change_managed     */	ChangeManaged,
    /* insert_child       */	InsertChild,
    /* delete_child       */	XtInheritDeleteChild,
    /* extension          */	NULL,
  },
  { /* constraint_class fields */
    /* resource list      */    constraints,
    /* num resources      */    XtNumber(constraints),
    /* constraint size    */ 	sizeof(XmBBoxConstraintsRec),
    /* init proc          */ 	ConstraintInitialize,
    /* destroy proc       */ 	NULL,
    /* set values proc    */ 	ConstraintSetValues,
    /* extension          */ 	NULL,
  },
  { /* manager_class fields */
    /* default translations */ 	XtInheritTranslations,
    /* syn_resources        */ 	get_resources,
    /* num_syn_resources    */ 	XtNumber(get_resources),
    /* syn_cont_resources   */ 	NULL,
    /* num_syn_cont_resources */ 0,
    /* parent_process       */ 	XmInheritParentProcess,
    /* extension            */ 	NULL,
  },
  {
    /* Row class members */
    /* extension            */	NULL,
  }
};

WidgetClass xmButtonBoxWidgetClass = (WidgetClass)&xmButtonBoxClassRec;

/************************************************************
 *       STATIC CODE
 ************************************************************/

/*      Function Name: ClassInitialize
 *      Description:   Called to initialize information specific
 *                     to this widget class.
 *      Arguments:     none.
 *      Returns:       none.
 */
static void ClassInitialize(void)
{
    XtSetTypeConverter(XmRString, XmRXmFillOption,
		       (XtTypeConverter) CvtStringToFillOption,
                       NULL, (Cardinal)0, XtCacheAll, (XtDestructor)NULL);
}

/****************************************************************/
static void
ClassPartInitialize(WidgetClass w_class )
{
    XmButtonBoxWidgetClass bbClass
        = (XmButtonBoxWidgetClass) w_class ;

/* this seems to do nothing.... why is it here? -rh
    XmButtonBoxWidgetClass bbSuper
	    = (XmButtonBoxWidgetClass) w_class->core_class.superclass ;
*/
    _XmFastSubclassInit (w_class, XmBUTTONBOX_BIT);


    /* Install the specifyrendertable trait for all subclasses */
    /*
    XmeTraitSet((XtPointer)w_class, XmQTspecifyRenderTable,
		(XtPointer) &bulletinbSRT);
    */
    return ;
}


/****************************************************************/


/*	Function Name: Initialize
 *	Description:   Called to initialize information specific
 *                     to this widget.
 *	Arguments:     req - what was originally requested.
 *                     new - what will be created (our superclasses have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */
static void Initialize(Widget req, Widget set,
		       ArgList args, Cardinal * num_args)
{
    /*
     * This is here just in case no children are added before the
     * widget is realized.
     */

    if (req->core.width == 0)
	set->core.width = 1;

    if (req->core.height == 0)
	set->core.height = 1;
}

/*	Function Name: ConstraintInitialize
 *	Description:   Called to initialize information specific
 *                     to this child widget.
 *	Arguments:     req - what was originally requested.
 *                     set - what will be created (our superclassed have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */
static void
ConstraintInitialize(Widget request, Widget set, ArgList args, Cardinal *num_args)
{
    XmButtonBoxC_pref_width(set) = XmButtonBoxC_pref_height(set) = 0;
}

/*      Function Name: ConstraintSetValues
 *      Description:   Called when child widget data needs to be modified on-
 *                     the-fly.
 *      Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *      Returns:       none
 */
static Boolean
ConstraintSetValues(Widget current, Widget request, Widget set,
		    ArgList args, Cardinal * num_args)
{
    if ((XmButtonBoxC_pref_width(set) != set->core.width) ||
	(XmButtonBoxC_pref_height(set) != set->core.height))
    {
	/*
	 * The desired height or width changed, this will
	 * tell the icon box to perform a new XtQueryGeom.
	 */

	XmButtonBoxC_pref_width(set) = XmButtonBoxC_pref_height(set) = 0;
    }

    return(False);
}

/*
 *	Function Name: Resize
 *      Description:   Called when this widget has been resized.
 *      Arguments:     w - Button Box Widget to resize.
 *      Returns:       none.
 */
static void Resize(Widget w)
{
	LayoutChildren(w, NULL);
}

/*      Function Name: QueryGeometry
 *      Description:   Called when my parent wants to know my
 *                     preferred size.
 *      Arguments:     w - the widget to check.
 *                     intended - parent imposed geometry.
 *                     preferred - what I would like.
 *      Returns:       status.
 */

static XtGeometryResult
QueryGeometry(Widget w, XtWidgetGeometry *request,
              XtWidgetGeometry *preferred)
{
    XmButtonBoxWidget 	bbox = (XmButtonBoxWidget)w;
    Dimension		max_major, max_minor, child_major_total;

    /*
     * Calculate our desired size.
     */

    (void) CalcChildrenPrefSizes(bbox, &max_major,
				 &max_minor, &child_major_total);

    if (IsHorizontal(bbox)) {
	preferred->width = child_major_total + (2 * MarginW(bbox));
	preferred->height = max_minor + (2 * MarginH(bbox));
    }
    else
    {
	preferred->width = max_minor + (2 * MarginW(bbox));
	preferred->height = child_major_total + (2 * MarginH(bbox));
    }

    return(_XmHWQuery(w, request, preferred));
}

/*      Function Name: GeometryManager
 *      Description:   handles request from children for size changes.
 *      Arguments:     child - the child to change.
 *                     request - desired geometry of child.
 *                     result - what will be allowed if almost.
 *      Returns:       status.
 */
static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry *request,
                XtWidgetGeometry *reply)
{
    XmButtonBoxWidget	bbox = (XmButtonBoxWidget)w->core.parent;
    Mask             	mask = 0;
    XtGeometryResult 	result;

    /*
     * Say no to any change in position.
     */

    if ( (request->request_mode & (CWX | CWY | CWBorderWidth)) &&
	 !(request->request_mode & (CWWidth | CWHeight)) )
    {
	return (XtGeometryNo);
    }

    if ( request->request_mode & (CWWidth | CWHeight) )
    {
	Dimension		max_major, max_minor;
	Dimension		child_major, child_minor;
	Dimension	        child_width, child_height;
	Dimension		child_major_total;

	Cardinal   		num_managed;

	/*
	 * Save the old size, and set the corresponding
	 * widget fields to the requested sizes.
	 */

	Dimension old_width       = XmButtonBoxC_pref_width(w);
	Dimension old_height      = XmButtonBoxC_pref_height(w);

	if ( request->request_mode & CWWidth )
	    XmButtonBoxC_pref_width(w)  = request->width;

	if ( request->request_mode & CWHeight )
	    XmButtonBoxC_pref_height(w) = request->height;

	/*
	 * See if everything fits with the new size.
	 */

	(void) TryNewLayout((Widget)bbox, &mask,
			    (request->request_mode & XtCWQueryOnly));

	num_managed = CalcChildrenPrefSizes(bbox, &max_major,
					    &max_minor, &child_major_total);

	CalcChildSize(bbox, w, max_major, max_minor,
		      child_major_total, num_managed, &child_major,
		      &child_minor);

	if (IsHorizontal(bbox)) {
	    child_height = child_minor;
	    child_width = child_major;
	}
	else {
	    child_height = child_major;
	    child_width = child_minor;
	}

	if ( (int)child_width >= (int)(2 * w->core.border_width) )
	    child_width -= (2 * w->core.border_width);

	if ( (int)child_height >= (int)(2 * w->core.border_width) )
	    child_height -= (2 * w->core.border_width);

	if (((child_width == XmButtonBoxC_pref_width(w)) ||
	     !(request->request_mode & CWWidth)) &&
	    ((child_height == XmButtonBoxC_pref_height(w)) ||
	     !(request->request_mode & CWHeight)))
	{
	    result = XtGeometryYes;
	}
	else if ((child_width != w->core.width) ||
		 (child_height != w->core.height))
	{
	    reply->request_mode = CWHeight | CWWidth;
	    reply->height = child_height;
	    reply->width = child_width;
	    result = XtGeometryAlmost;
	}
	else {
	    result = XtGeometryNo;
	}

	if ((request->request_mode & XtCWQueryOnly) ||
	    (result != XtGeometryYes))
	{
	    XmButtonBoxC_pref_width(w) = old_width;
	    XmButtonBoxC_pref_height(w) = old_height;
	}
	else
	    LayoutChildren((Widget)bbox, w);

	/*
	 * We didn't get what we wanted, our preferred
	 * size should be recomputed the next time we have
	 * a chance to ask it, given that we're rejecting
	 * its current geometry and that it may not agree
	 * with our Almost reply.
	 */

	if (result != XtGeometryYes) {
	    if ( request->request_mode & CWWidth )
		XmButtonBoxC_pref_width(w) = 0;

	    if ( request->request_mode & CWHeight )
		XmButtonBoxC_pref_height(w) = 0;
	}

	return(result);
    }

    /*
     * Stacking request only.
     */

    return (XtGeometryYes);
}


/*      Function Name: ChangeManaged
 *      Description:   when a management change has occurred.
 *      Arguments:     w - the ButtonBox widget.
 *      Returns:       none.
 */
static void
ChangeManaged(Widget w)
{
    Mask		mask = 0;
    /* CR03420 mask should be set accordingly */
    if (w->core.width == 1) mask |= CWWidth;
    if (w->core.height == 1) mask |= CWHeight;
    if (mask) (void)TryNewLayout(w, &mask, False);
    LayoutChildren(w, NULL);

   /*
    * for Motif navigation
    */

   XmeNavigChangeManaged(w);
}


/*      Function Name: InsertChild
 *      Description:   when a child widget is added.
 *      Arguments:     w - the child widget
 *      Returns:       none.
 */
static void
InsertChild(Widget child)
{
Boolean         is_button = False ;
XmButtonBoxWidget bb = (XmButtonBoxWidget) XtParent( child) ;
    /*
     * Check for non-widgets - ButtonBox doesn't support gadgets, for now ...
     */

    if (_XmGadgetWarning(child))
    {
    	return;
    }

    /*
     * Insert the child widget in the composite children list with the
     * superclass insert_child routine.
     */

    /* (*SUPERCLASS->composite_class.insert_child)(w); */

    /* Another process may be updating the superclass's data */
    /* (This was missed for some reason... - scorch)
     * use composite class insert proc to do all the dirty work
     */
    {
       XtWidgetProc insert_child;
       _XmProcessLock();
       insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)->
				composite_class.insert_child;
       _XmProcessUnlock();
       (*insert_child)(child);
    }

    if(XmeTraitGet((XtPointer)XtClass(child), XmQTtakesDefault))
    {
        is_button = TRUE ;

        if( XmButtonBox_default_button( bb) )
        {
            /* Notify child that it has to be ready for default visual */
            ButtonBoxSetDefaultShadow( child) ;
        }
    }

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
          ArgList args, Cardinal * num_args)
{
    XmButtonBoxWidget b_old = (XmButtonBoxWidget) current;
    XmButtonBoxWidget b_set = (XmButtonBoxWidget) set;
    Boolean	      newLayout = False;
    Mask mask = 0; /* CR03420 */
    if ( XmButtonBox_equal_size(b_set) != XmButtonBox_equal_size(b_old) ||
	 XmButtonBox_fill_option(b_set) != XmButtonBox_fill_option(b_old) ||
	 XmButtonBox_margin_height(b_set) != XmButtonBox_margin_height(b_old) ||
	 XmButtonBox_margin_width(b_set) != XmButtonBox_margin_width(b_old) ||
	 XmButtonBox_orientation(b_set) != XmButtonBox_orientation(b_old) )
    {
	newLayout = True;
    }

    if (set->core.width == 0)
    { /* CR03420 */
      set->core.width = 1; /* setting to 1 to prevent zero width */
      mask |= CWWidth;
    }
    if (set->core.height == 0)
    { /* CR03420 */
      set->core.height = 1; /* setting to 1 to prevent zero height */
      mask |= CWHeight;
    }
    if (mask) TryNewLayout((Widget)b_set, &mask, False);
    if ( newLayout == True ) LayoutChildren((Widget)b_set, NULL);

    return(False);
}

/************************************************************
 * Type Converters.
 ************************************************************/

/*      Function Name: CvtStringToFillOption
 *      Description:   Converts a string to a FillOption
 *      Arguments:     dpy - the X Display.
 *                     args, num_args - *** NOT USED ***
 *                     fromVal - contains the string to convert.
 *                     toVal - contains the converted node state.
 *      Returns:
 */
static Boolean
CvtStringToFillOption(Display * dpy, XrmValuePtr args, Cardinal *num_args,
		      XrmValuePtr fromVal, XrmValuePtr toVal)
{
    static XmFillOption 	option;
    char 			lowerName[BUFSIZ];

    XmCopyISOLatin1Lowered(lowerName, (char *)fromVal->addr);

    if ( streq(lowerName, "none") || streq(lowerName,"fillnone") )
        option = XmFillNone;
    else if ( streq(lowerName, "major") || streq(lowerName, "fillmajor") )
	option = XmFillMajor;
    else if ( streq(lowerName, "minor") || streq(lowerName, "fillminor") )
	option = XmFillMinor;
    else if ( streq(lowerName, "all") || streq(lowerName, "fillall") )
	option = XmFillAll;
    else
    {
	XtDisplayStringConversionWarning(dpy, fromVal->addr, XmRXmFillOption);
        return(False);          /* Conversion failed. */
    }

    if ( toVal->addr == NULL )
    {
        toVal->size = sizeof(XmFillOption);
        toVal->addr = (XtPointer)&option;
        return(True);
    }
    else
    {
	toVal->size = sizeof(XmFillOption);
	if ( toVal->size >= sizeof(XmFillOption) )
	{
	    XmFillOption *state = (XmFillOption *)toVal->addr;

	    *state = option;
	    return(True);
	}
	else
	{
	    return(False);
	}
    }
}

/************************************************************
 *      LOCAL CODE
 ************************************************************/

/*      Function Name: TryNewLayout
 *      Description:   tries to do a layout within the current width
 *                     and height.  If that fails, it tries to resize and
 *                     do it within the geometry given in preferred_size.
 *      Arguments:     parent - the ButtonBox
 *		       mask - request mask
 *		       queryOnly - should this only query
 *      Returns:       status.
 */
static XtGeometryResult
TryNewLayout(Widget parent, Mask *mask,
	     Boolean queryOnly)
{
    XmButtonBoxWidget 	bbox = (XmButtonBoxWidget)parent;
    Dimension		new_width, new_height;
    Dimension		max_major, max_minor, child_major_total;

    /*
     * Get the bounding width and height of all children.
     */

    (void) CalcChildrenPrefSizes(bbox, &max_major,
				 &max_minor, &child_major_total);

    if (IsHorizontal(bbox))
    {
	new_width = child_major_total + 2 * MarginW(bbox);
	new_height = max_minor + 2 * MarginH(bbox);
    }
    else
    {
	new_width = max_minor + 2 * MarginW(bbox);
	new_height = child_major_total + 2 * MarginH(bbox);
    }

    /*
     * Be sure that we are always at least one pixel high.
     */

    if (new_width == 0)
	new_width = 1;
    if (new_height == 0)
	new_height = 1;

    if ( new_width != parent->core.width || new_height != parent->core.height )
    {
	XtWidgetGeometry	request, reply;
	XtGeometryResult	result;

	request.width = new_width;
	request.height = new_height;

	reply.width = 0;
	reply.height = 0;

	request.request_mode = *mask; /* CR03420 */
	if ( queryOnly )
	{
	    request.request_mode |= XtCWQueryOnly;
	}
	result = XtMakeGeometryRequest(parent, &request, &reply);

	if ( request.width == reply.width )
	{
	    *mask |= CWWidth;
	}
	if ( request.height == reply.height )
	{
	    *mask |= CWHeight;
	}

	if ( result == XtGeometryAlmost && !queryOnly )
	{
	    result = XtMakeGeometryRequest(parent, &reply, &reply);
	}

	return(result);
    }

    *mask = CWWidth | CWHeight;
    return(XtGeometryYes);
}


/*      Function Name: LayoutChildren
 *      Description:   Lays out the "child" widgets in the ButtonBox widget.
 *      Arguments:     w - the ButtonBox widget.
 *                     special_child - Treat this child special, actually
 *                                     set the new width and height instead
 *                                     of calling the Xt routine.  This helps
 *                                     the geom mgr implement XtGeometryYes
 *                                     policy.
 *      Returns:       none.
 */
static void LayoutChildren(Widget w, Widget special_child)
{
    XmButtonBoxWidget 	bbox = (XmButtonBoxWidget)w;
    Widget             *childp;

    Dimension		margin_major, margin_minor;
    Dimension		max_major, max_minor;
    Dimension		box_major, box_minor;
    Dimension		child_major, child_minor;
    Dimension	        child_width, child_height;
    Dimension		child_major_total;

    Position		pos_minor;
    Position   	        pos_x, pos_y;

    float  		pad = 0, pos_major;
    Cardinal   		num_managed;

    child_major = 0;
    child_minor = 0;

    if ( IsHorizontal(bbox) )
    {
	margin_major = MarginW(bbox);
	margin_minor = MarginH(bbox);
	box_major = bbox->core.width;
	box_minor = bbox->core.height;
    }
    else
    {
	margin_major = MarginH(bbox);
	margin_minor = MarginW(bbox);
	box_major = bbox->core.height;
	box_minor = bbox->core.width;
    }
    if ((int)box_major > (int)(2 * margin_major))
	box_major -= 2 * margin_major;
    else
	box_major = 1;

    if ((int)box_minor > (int)(2 * margin_minor))
	box_minor -= 2 * margin_minor;
    else
	box_minor = 1;

    num_managed = CalcChildrenPrefSizes(bbox, &max_major,
					&max_minor, &child_major_total);

    pad = 0.0;
    pos_major = (float) margin_major;
    if ((FillOption(bbox) == XmFillNone) || (FillOption(bbox) == XmFillMinor))
    {
	if ( box_major >= child_major_total ) {
	    pad = ((float) box_major -
		   (float) child_major_total) / ((float) num_managed + 1.0);
	    pos_major += pad;
	}
    }

    ForAllChildren(bbox, childp) {
	if ( !XtIsManaged(*childp) )
	    continue;

	CalcChildSize(bbox, *childp, max_major, max_minor,
		      child_major_total, num_managed, &child_major,
		      &child_minor);

	if ((FillOption(bbox) == XmFillNone) ||
	    (FillOption(bbox) == XmFillMajor))
	{
	    pos_minor = (((int)(box_minor - child_minor)) /2) + margin_minor;
	}
	else
	    pos_minor = margin_minor;

	if ( IsHorizontal(bbox) ) {
	    child_width = child_major;
	    child_height = child_minor;
	    if (LayoutIsRtoLM(bbox))
		pos_x = box_major - (int) pos_major - child_width;
	    else
		pos_x = (int) pos_major;
	    pos_y = pos_minor;
	}
	else {
	    pos_x = pos_minor;
	    pos_y = (int) pos_major;
	    child_width = child_minor;
	    child_height = child_major;
	}

	if ( (int)child_width >= (int)(2 * (*childp)->core.border_width) )
	    child_width -= (2 * (*childp)->core.border_width);

	if ( (int)child_height >= (int)(2 * (*childp)->core.border_width) )
	    child_height -= (2 * (*childp)->core.border_width);

	child_width = (child_width == 0) ? 1 : child_width;
	child_height = (child_height == 0) ? 1 : child_height;

	if (*childp == special_child) {
	    special_child->core.x = pos_x;
	    special_child->core.y = pos_y;
	    special_child->core.width = child_width;
	    special_child->core.height = child_height;
	}
	else {
	    _XmConfigureWidget(*childp, pos_x, pos_y, child_width,
			       child_height, (*childp)->core.border_width);
	}

	pos_major += (float) child_major + pad;

	/* Encourage the child to redraw */
	if (XtIsRealized(*childp))
		XClearArea(XtDisplay(*childp), XtWindow(*childp), 0, 0, 0, 0, True);
	}
}

/*      Function Name: CalcChildrenPrefSizes
 *      Description:   Determines the total size in the major direction
 *		       of all children of the ButtonBox, as well as the
 *                     maximum value in both major and minor directions.
 *      Arguments:     bbox - the ButtonBox parent.
 *		       max_major - the maximum size of all the children
 *				   in the major dimension.
 *		       total - return location for the total size in the
 *			       major direction.
 *      Returns:       number of managed children.
 */
static Cardinal
CalcChildrenPrefSizes(XmButtonBoxWidget bbox, Dimension *max_major,
		      Dimension *max_minor, Dimension *total)
{
    Cardinal		num_managed = 0;
    Widget	       *childp;
    XtWidgetGeometry	geo;
    Dimension	       *maj_dim, *minor_dim;
    Boolean		is_equal = EqualSize(bbox);

    if ( IsHorizontal(bbox) )
    {
	maj_dim = &geo.width;
	minor_dim = &geo.height;
    }
    else
    {
	maj_dim = &geo.height;
	minor_dim = &geo.width;
    }

    *total = *max_major = *max_minor = 1;

    ForAllChildren(bbox, childp) {

	if ( !XtIsManaged(*childp) )
	    continue;

	num_managed++;

	if ((XmButtonBoxC_pref_width(*childp) == 0) ||
	    (XmButtonBoxC_pref_height(*childp) == 0))
	{
	    XtQueryGeometry(*childp, NULL, &geo);
	}
	else
	    geo.border_width = (*childp)->core.border_width;

	if (XmButtonBoxC_pref_width(*childp) == 0)
	    XmButtonBoxC_pref_width(*childp) = geo.width;
	else
	    geo.width = XmButtonBoxC_pref_width(*childp);

	if (XmButtonBoxC_pref_height(*childp) == 0)
	    XmButtonBoxC_pref_height(*childp) = geo.height;
	else
	    geo.height = XmButtonBoxC_pref_height(*childp);

	ASSIGN_MAX(*max_major, *maj_dim + (2 * geo.border_width));
	ASSIGN_MAX(*max_minor, *minor_dim + (2 * geo.border_width));

	if (!is_equal)
	    *total += (*maj_dim + (2 * geo.border_width));
    }

    if ( is_equal )
        *total = num_managed * (*max_major);

    return(num_managed);
}

/*      Function Name: CalcChildSize
 *      Description:   Determines the size of a child
 *      Arguments:     bbox - the ButtonBox parent.
 *		       w - the child of interest.
 *		       max_major - the largest size allowed in the
 *				   major direction.
 *		       max_minor - the largest size allowed in the
 *				   minor direction.
 *		       child_major_total - total size of all children in the
 *					   major direction.
 *		       num_managed - number of managed children.
 *		       child_major - return location for child's base
 *				     major size.
 *		       child_minor - return location for child's base
 *				     minor size.
 *      Returns:       none.
 */
static void
CalcChildSize(XmButtonBoxWidget bbox, Widget w, Dimension max_major,
	      Dimension max_minor, Dimension child_major_total,
	      Cardinal num_managed, Dimension *child_major,
	      Dimension *child_minor)
{
    XtWidgetGeometry	geo;
    Dimension		box_minor, box_major;
    Dimension		margin_major, margin_minor;

    if ((XmButtonBoxC_pref_width(w) == 0)||(XmButtonBoxC_pref_height(w) == 0))
	XtQueryGeometry(w, NULL, &geo);
    else
	geo.border_width = w->core.border_width;

    if (XmButtonBoxC_pref_width(w) != 0)
	geo.width = XmButtonBoxC_pref_width(w);

    if (XmButtonBoxC_pref_height(w) != 0)
	geo.height = XmButtonBoxC_pref_height(w);

    if ( IsHorizontal(bbox) )
    {
	margin_major = MarginW(bbox);
	margin_minor = MarginH(bbox);
	box_major = bbox->core.width;
        box_minor = bbox->core.height;
        *child_major = geo.width;
        *child_minor = geo.height;
    }
    else
    {
	margin_major = MarginH(bbox);
	margin_minor = MarginW(bbox);
	box_major = bbox->core.height;
        box_minor = bbox->core.width;
        *child_major = geo.height;
        *child_minor = geo.width;
    }
    *child_major += (2 * geo.border_width);
    *child_minor += (2 * geo.border_width);

    if ((int)box_major > (int)(2 * margin_major))
	box_major -= (2 * margin_major);
    else
	box_major = 1;

    if ((int)box_minor > (int)(2 * margin_minor))
	box_minor -= (2 * margin_minor);
    else
	box_minor = 1;

    if ( EqualSize(bbox) )
    {
        *child_major = max_major;
        *child_minor = max_minor;
    }
    if ( FillOption(bbox) == XmFillMajor || FillOption(bbox) == XmFillAll ||
	 child_major_total > box_major )
    {
	*child_major = (Dimension)
	    (((float)*child_major/(float)child_major_total) * box_major);
    }
    if ( FillOption(bbox) == XmFillMinor || FillOption(bbox) == XmFillAll ||
	 *child_minor > box_minor )
    {
	*child_minor = box_minor;
    }
}

/************************************************************
 *
 *  Public functions
 *
 ************************************************************/

/*	Function Name: XmCreateButtonBox
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateButtonBox(Widget parent, String name, ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name,
			  xmButtonBoxWidgetClass, parent, args, num_args));
}

Widget
XmVaCreateButtonBox(
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
                         xmButtonBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedButtonBox(
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
                         xmButtonBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/****************************************************************/
static void
ButtonBoxSetDefaultShadow(Widget button)
{
    XmTakesDefaultTrait trait_default ;

    trait_default = (XmTakesDefaultTrait) XmeTraitGet((XtPointer)
						      XtClass(button),
						      XmQTtakesDefault) ;
    if (trait_default)
	trait_default->showAsDefault  (button, XmDEFAULT_READY);
}

