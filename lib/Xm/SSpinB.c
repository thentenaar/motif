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

/******************************************************************************
 *
 *	File:	SSpinB.c
 *	Date:	June 1, 1995
 *	Author:	Mitchell Greess
 *
 *	Contents:
 *		Implements the XmSimpleSpinBox widget.
 *
 ******************************************************************************/


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

#include <Xm/TextF.h>
#include <Xm/VaSimpleP.h>
#include "MessagesI.h"
#include "XmI.h"

#include <Xm/SSpinBP.h>

static void	Initialize(
			Widget		req,
			Widget		w,
			ArgList		args,
			Cardinal	*num_args);
static Boolean	SetValues(
			Widget		old,
			Widget		req,
			Widget		new_w,
			ArgList		args,
			Cardinal	*num_args);
static void	InsertChild(
			Widget		newChild);
static Widget	GetCallbackWidget(
			Widget		widget);
static void	SyntheticGetValue(
			Widget widget,
			int offset,
			XtArgVal *value);

/* Resources */
#define Offset(field) XtOffsetOf(XmSimpleSpinBoxRec,simpleSpinBox.field)
#define ManagerOffset(field) XtOffsetOf(XmSimpleSpinBoxRec,manager.field)
static XtResource resources[] = {
  /*
   * Inherited resources.
   */
  {
    XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), ManagerOffset(shadow_thickness),
    XmRImmediate, (XtPointer) 1
  },

  /*
   * SimpleSpinBox resources.
   */

  {
    XmNarrowSensitivity, XmCArrowSensitivity, XmRArrowSensitivity,
    sizeof(unsigned char), Offset(arrow_sensitivity),
    XmRImmediate, (XtPointer) XmARROWS_DEFAULT_SENSITIVITY
  },

  {
    XmNcolumns, XmCColumns, XmRShort,
    sizeof(short), Offset(columns),
    XmRImmediate, (XtPointer) 20
  },

  {
    XmNdecimalPoints, XmCDecimalPoints, XmRShort,
    sizeof(short), Offset(decimal_points),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNeditable, XmCEditable, XmRBoolean,
    sizeof(Boolean), Offset(editable),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNincrementValue, XmCIncrementValue, XmRInt,
    sizeof(int), Offset(increment_value),
    XmRImmediate, (XtPointer) 1
  },

  {
    XmNmaximumValue, XmCMaximumValue, XmRInt,
    sizeof(int), Offset(maximum_value),
    XmRImmediate, (XtPointer) 10
  },

  {
    XmNminimumValue, XmCMinimumValue, XmRInt,
    sizeof(int), Offset(minimum_value),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNnumValues, XmCNumValues, XmRInt,
    sizeof(int), Offset(num_values),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNposition, XmCPosition, XmRInt,
    sizeof(int), Offset(position),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNpositionType, XmCPositionType, XmRPositionType,
    sizeof(unsigned char), Offset(position_type),
    XmRImmediate, (XtPointer) XmPOSITION_VALUE
  },

  {
    XmNspinBoxChildType, XmCSpinBoxChildType, XmRSpinBoxChildType,
    sizeof(unsigned char), Offset(sb_child_type),
    XmRImmediate, (XtPointer) XmSTRING
  },

  {
    XmNtextField, XmCTextField, XmRWidget,
    sizeof(Widget), Offset(text_field),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNvalues, XmCValues, XmRXmStringTable,
    sizeof(XmStringTable), Offset(values),
    XmRStringTable, NULL
  },

  {
    XmNwrap, XmCWrap, XmRBoolean,
    sizeof(Boolean), Offset(wrap),
    XmRImmediate, (XtPointer) True
  }

};

static XmSyntheticResource syn_resources[] =
{
  {
    XmNarrowSensitivity, sizeof(unsigned char), Offset(arrow_sensitivity),
    SyntheticGetValue, NULL
  },

  {
    XmNcolumns, sizeof(short), Offset(columns),
    SyntheticGetValue, NULL
  },

  {
    XmNdecimalPoints, sizeof(short), Offset(decimal_points),
    SyntheticGetValue, NULL
  },

  {
    XmNeditable, sizeof(Boolean), Offset(editable),
    SyntheticGetValue, NULL
  },

  {
    XmNincrementValue, sizeof(int), Offset(increment_value),
    SyntheticGetValue, NULL
  },

  {
    XmNmaximumValue, sizeof(int), Offset(maximum_value),
    SyntheticGetValue, NULL
  },

  {
    XmNminimumValue, sizeof(int), Offset(minimum_value),
    SyntheticGetValue, NULL
  },

  {
    XmNnumValues, sizeof(int), Offset(num_values),
    SyntheticGetValue, NULL
  },

  {
    XmNposition, sizeof(int), Offset(position),
    SyntheticGetValue, NULL
  },

  {
    XmNpositionType, sizeof(unsigned char), Offset(position_type),
    SyntheticGetValue, NULL
  },

  {
    XmNspinBoxChildType, sizeof(unsigned char), Offset(sb_child_type),
    SyntheticGetValue, NULL
  },

  {
    XmNvalues, sizeof(XmStringTable), Offset(values),
    SyntheticGetValue, NULL
  },

  {
    XmNwrap, sizeof(Boolean), Offset(wrap),
    SyntheticGetValue, NULL
  }
};

/*  The Spin class record definition  */
externaldef (xmspinboxclassrec) XmSimpleSpinBoxClassRec xmSimpleSpinBoxClassRec= {
  {
    (WidgetClass)&xmSpinBoxClassRec,    /* superclass */
    "XmSimpleSpinBox",                  /* class_name */
    sizeof(XmSimpleSpinBoxRec),         /* widget_size */
    (XtProc) NULL,		    	/* class_initialize */
    (XtWidgetClassProc) NULL,		/* class_part_initialize */
    FALSE,    		                /* class_inited */
    Initialize,    	                /* initialize */
    NULL,    		                /* initialize_hook */
    XtInheritRealize,		        /* realize */
    NULL, 	     	                /* actions */
    0,					/* num_actions */
    resources,    	                /* resources */
    XtNumber(resources),                /* num_resources */
    NULLQUARK,    	                /* xrm_class */
    TRUE,    		                /* compress_motion */
    XtExposeCompressMaximal,           	/* compress_exposure */
    TRUE,    		              	/* compress_enterleave */
    FALSE,    		              	/* visible_interest */
    (XtWidgetProc) NULL,		/* destroy */
    XtInheritResize,		      	/* resize */
    XtInheritExpose,   	              	/* expose */
    SetValues,    	              	/* set_values */
    (XtArgsFunc) NULL,			/* set_values_hook */
    XtInheritSetValuesAlmost,          	/* set_values_almost */
    (XtArgsProc) NULL,			/* get_values_hook */
    XtInheritAcceptFocus,	      	/* accept_focus */
    XtVersion,    	              	/* version */
    (XtPointer) NULL,			/* callback proc list */
    XtInheritTranslations,	        /* tm_table */
    XtInheritQueryGeometry, 	      	/* query_geometry */
    (XtStringProc) NULL,		/* display_accelerator */
    (XtPointer) NULL,			/* extension */
  },

  {    /* composite_class fields */
    XtInheritGeometryManager,          	/* geometry_manager */
    XtInheritChangeManaged,            	/* change_managed */
    InsertChild,		        /* insert_child */
    XtInheritDeleteChild,	        /* delete_child */
    (XtPointer) NULL,			/* extension */
  },

  {    /* constraint_class fields */
    (XtResourceList) NULL,     		/* resource_list */
    (Cardinal) 0,		      	/* num_resources */
    (Cardinal) sizeof(
        XmSimpleSpinBoxConstraintRec),	/* constraint_size */
    (XtInitProc) NULL,			/* init_proc */
    (XtWidgetProc) NULL,		/* destroy_proc */
    (XtSetValuesFunc) NULL,		/* set_values_proc */
    (XtPointer) NULL,    		/* extension */
  },
      /* manager_class fields */
  {
    XtInheritTranslations,		/* translations */
    (XmSyntheticResource *) syn_resources, /* syn_resources */
    XtNumber(syn_resources),           	/* num_syn_resources */
    (XmSyntheticResource *) NULL,	/* syn_cont_resources */
    0,    		              	/* num_syn_cont_resources */
    (XmParentProcessProc) NULL,		/* parent_process */
    (XtPointer) NULL,			/* extension */
  },
      /* spinbox_class fields */
  {
    (XmGetCallbackWidgetProc)
        GetCallbackWidget,		/* get_callback_widget */
    (XtPointer) NULL,			/* extension */
  },
      /* simple_spinbox_class fields */
  {
    (XtPointer) NULL,			/* extension */
  }

};

externaldef(xmsimplespinboxwidgetclass) WidgetClass xmSimpleSpinBoxWidgetClass =
       (WidgetClass) &xmSimpleSpinBoxClassRec;


#define BAD_SSPIN_SET_TEXT_FIELD	_XmMMsgSSpinB_0001
#define BAD_SSPIN_SET_POSITION_TYPE	_XmMMsgSSpinB_0002
#define BAD_SSPIN_SET_ITEM		_XmMMsgSSpinB_0003

/******************************************************************************
 **
 ***			MACROS
 **
 *****************************************************************************/

#define SSB_TEXTFIELD_NAME_FORMAT	"%s_TF"
#define SSB_SIMPLE_SPIN_BOX_PART(w)	\
	(& ((XmSimpleSpinBoxWidget)(w))->simpleSpinBox)

/******************************************************************************
 **
 ***			METHODS
 **
 *****************************************************************************/

static void
Initialize(Widget	request,	/* unused */
	   Widget	new,
	   ArgList	user_args,	/* unused */
	   Cardinal	*n_user_args)	/* unused */
{
    char			*widget_name;
    XmSimpleSpinBoxWidget	ssb_w;
    XmSimpleSpinBoxPart		*ssb_p;
    XmSpinBoxConstraint		textf_c;
    Arg				args[XtNumber(resources)];
    int				nargs = 0;

    ssb_w = (XmSimpleSpinBoxWidget) new;
    ssb_p = &ssb_w->simpleSpinBox;

    ssb_w->simpleSpinBox.text_field = (Widget) NULL;

    /*
     * Create and insert the text field child widget;
     */
    widget_name = XtMalloc(strlen(XtName(new)) + 10);
    sprintf(widget_name, SSB_TEXTFIELD_NAME_FORMAT, XtName(new));

    /*
     * Collect all the resources that apply to the TextField child,
     * and push include them in the creation call.
     * Allow the SpinBox widget worry about ensuring values are valid
     * and about saving values.
     * Once the child has been created, copy the relevant values out of
     * the constraint record for the TextField into our instance record.
     */
    XtSetArg(args[nargs], XmNarrowSensitivity,ssb_p->arrow_sensitivity);nargs++;
    XtSetArg(args[nargs], XmNdecimalPoints, ssb_p->decimal_points); nargs++;
    XtSetArg(args[nargs], XmNincrementValue, ssb_p->increment_value); nargs++;
    XtSetArg(args[nargs], XmNmaximumValue, ssb_p->maximum_value); nargs++;
    XtSetArg(args[nargs], XmNminimumValue, ssb_p->minimum_value); nargs++;
    XtSetArg(args[nargs], XmNnumValues, ssb_p->num_values); nargs++;
    XtSetArg(args[nargs], XmNposition, ssb_p->position); nargs++;
    XtSetArg(args[nargs], XmNpositionType, ssb_p->position_type); nargs++;
    XtSetArg(args[nargs], XmNspinBoxChildType, ssb_p->sb_child_type); nargs++;
    XtSetArg(args[nargs], XmNvalues, ssb_p->values); nargs++;
    XtSetArg(args[nargs], XmNwrap,ssb_p->wrap);nargs++;

    XtSetArg(args[nargs], XmNeditable, ssb_p->editable); nargs++;
    XtSetArg(args[nargs], XmNcolumns, ssb_p->columns); nargs++;

    ssb_p->text_field = XtCreateManagedWidget(	widget_name,
						xmTextFieldWidgetClass,
						(Widget) new,
						args, nargs);
    XtSetValues(ssb_p->text_field, args, nargs);

    textf_c = SB_GetConstraintRec(ssb_p->text_field);
    ssb_p->arrow_sensitivity = textf_c->arrow_sensitivity;
    ssb_p->decimal_points = textf_c->decimal_points;
    ssb_p->increment_value = textf_c->increment_value;
    ssb_p->maximum_value = textf_c->maximum_value;
    ssb_p->minimum_value = textf_c->minimum_value;
    ssb_p->num_values = textf_c->num_values;
    ssb_p->position = textf_c->position;
    ssb_p->position_type = textf_c->position_type;
    ssb_p->sb_child_type = textf_c->sb_child_type;
    ssb_p->values = textf_c->values;
    ssb_p->wrap = textf_c->wrap;

    XtVaGetValues(ssb_p->text_field,
		  XmNeditable, &ssb_p->editable,
		  XmNcolumns, &ssb_p->columns,
		  NULL);

    XtFree(widget_name);
}

static Boolean
SetValues(Widget	current,
	  Widget	request, 	/* unused */
	  Widget	new,
	  ArgList	args,		/* unused */
	  Cardinal	*num_args)	/* unused */
{
    XmSimpleSpinBoxPart	*cur_ssbp = SSB_SIMPLE_SPIN_BOX_PART(current);
    XmSimpleSpinBoxPart	*new_ssbp = SSB_SIMPLE_SPIN_BOX_PART(new);
    XmSpinBoxConstraint	textf_c;

    /*
     * These resources have CG permissions only:
     *	XmNpositionType, XmNspinBoxChildType, XmNtextField
     *
     * BINARY COMPATIBILITY with DTSPINBOX
     * However, DtSpinBox does not prevent setting XmNspinBoxChildType
     * so we have to allow it.
     *
     * new_ssbp->sb_child_type = cur_ssbp->sb_child_type;
     */
    if (new_ssbp->position_type != cur_ssbp->position_type) {
        new_ssbp->position_type = cur_ssbp->position_type;
	XmeWarning(new, BAD_SSPIN_SET_POSITION_TYPE);
    }
    if (new_ssbp->text_field != cur_ssbp->text_field) {
        new_ssbp->text_field = cur_ssbp->text_field;
	XmeWarning(new, BAD_SSPIN_SET_TEXT_FIELD);
    }

    /*
     * Collect changed args and push them onto the TextField child.
     * Let the SpinBox widget worry about ensuring values are valid
     * and about saving values.
     *
     * Later, we'll copy the relevant values back into the SimpleSpinBox
     * record to ensure consistancy.
     */
    XtVaSetValues(new_ssbp->text_field,
		  XmNarrowSensitivity, new_ssbp->arrow_sensitivity,
		  XmNdecimalPoints, new_ssbp->decimal_points,
		  XmNincrementValue, new_ssbp->increment_value,
		  XmNmaximumValue, new_ssbp->maximum_value,
		  XmNminimumValue, new_ssbp->minimum_value,
		  XmNnumValues, new_ssbp->num_values,
		  XmNposition, new_ssbp->position,
		  XmNspinBoxChildType, new_ssbp->sb_child_type,
		  XmNvalues, new_ssbp->values,
		  XmNwrap, new_ssbp->wrap,

		  XmNeditable, new_ssbp->editable,
		  XmNcolumns, new_ssbp->columns,
		  NULL);

    textf_c = SB_GetConstraintRec(new_ssbp->text_field);
    new_ssbp->arrow_sensitivity = textf_c->arrow_sensitivity;
    new_ssbp->decimal_points = textf_c->decimal_points;
    new_ssbp->increment_value = textf_c->increment_value;
    new_ssbp->maximum_value = textf_c->maximum_value;
    new_ssbp->minimum_value = textf_c->minimum_value;
    new_ssbp->num_values = textf_c->num_values;
    new_ssbp->position = textf_c->position;
    new_ssbp->sb_child_type = textf_c->sb_child_type;
    new_ssbp->values = textf_c->values;
    new_ssbp->wrap = textf_c->wrap;

    XtVaGetValues(new_ssbp->text_field,
		  XmNeditable, &new_ssbp->editable,
		  XmNcolumns, &new_ssbp->columns,
		  NULL);

    return FALSE;
}


static void
InsertChild(Widget newChild)
{
    XmSimpleSpinBoxWidget	ssb_w;
    XtWidgetProc 		insert_child;

    ssb_w = (XmSimpleSpinBoxWidget) XtParent(newChild);
    if (ssb_w->composite.num_children != 0) {
	XmeWarning((Widget) ssb_w, BAD_SSPIN_SET_TEXT_FIELD);
	return;
    }

    /* Call SpinBox's InsertChild method */
    _XmProcessLock();
    insert_child = ((XmSpinBoxWidgetClass)xmSpinBoxWidgetClass)->
	 			composite_class.insert_child;
    _XmProcessUnlock();
    (*insert_child)(newChild);
}

static Widget
GetCallbackWidget(Widget widget)
{
    XmSimpleSpinBoxWidget	ssb_w;

    ssb_w = (XmSimpleSpinBoxWidget) widget;
    return((Widget) ssb_w->simpleSpinBox.text_field);
}

/******************************************************************************
 * SyntheticGetValue
 *	XmExportProc conversion routine.
 *	Used to retrieve constraint resources from the spinbox for the
 *	textfield child.
 *****************************************************************************/
static void SyntheticGetValue(Widget widget, int offset, XtArgVal *value)
{
	XmSimpleSpinBoxWidget ssb_w = (XmSimpleSpinBoxWidget)widget;

	switch (offset) {
	case Offset(arrow_sensitivity):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNarrowSensitivity, value, NULL);
		break;
	case Offset(columns):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNcolumns, value, NULL);
		break;
	case Offset(decimal_points):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNdecimalPoints, value, NULL);
		break;
	case Offset(editable):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNeditable, value, NULL);
		break;
	case Offset(increment_value):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNincrementValue, value, NULL);
		break;
	case Offset(minimum_value):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNminimumValue, value, NULL);
		break;
	case Offset(maximum_value):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNmaximumValue, value, NULL);
		break;
	case Offset(num_values):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNnumValues, value, NULL);
		break;
	case Offset(position):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNposition, value, NULL);
		break;
	case Offset(position_type):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNpositionType, value, NULL);
		break;
	case Offset(sb_child_type):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNspinBoxChildType, value, NULL);
		break;
	case Offset(values):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNvalues, value, NULL);
		break;
	case Offset(wrap):
		XtVaGetValues(ssb_w->simpleSpinBox.text_field, XmNwrap, value, NULL);
		break;
	default:
		fprintf(stderr,
		        "SimpleSpinBox ERROR:  Invalid synthetic resource offset  %d\n",
		        offset);
   }
}

/******************************************************************************
 **
 ***			EVENT HANDLERS
 **
 *****************************************************************************/


/******************************************************************************
 **
 ***			ACTIONS
 **
 *****************************************************************************/


/******************************************************************************
 **
 ***			OTHER FUNCTIONS
 **
 *****************************************************************************/


/*
 * Public API
 */

/************************************************************************
 *  XmCreateSimpleSpinBox
 *	Create an instance of a Spin widget and return the widget id.
 ************************************************************************/
Widget
XmCreateSimpleSpinBox(	Widget		parent,
			String		name,
			ArgList		arglist,
			Cardinal	argcount)
{
    return(XtCreateWidget(name, xmSimpleSpinBoxWidgetClass, parent,
			  arglist, argcount));
}

Widget
XmVaCreateSimpleSpinBox(
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
                         xmSimpleSpinBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedSimpleSpinBox(
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
                         xmSimpleSpinBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/************************************************************************
 *  XmSimpleSpinBoxAddItem
 *	Add an item to the list of strings at the specified position.
 ************************************************************************/
void
XmSimpleSpinBoxAddItem(	Widget		ssb_w,
			XmString	item,
			int	 	pos)
{
    XmSimpleSpinBoxPart	*ssb_p = SSB_SIMPLE_SPIN_BOX_PART(ssb_w);
    XmSpinBoxConstraint	textf_c;
    XmStringTable	values;
    int			new_nvalues;
    int			i;
    _XmWidgetToAppContext(ssb_w);

    _XmAppLock(app);

    /*
     * Get the latest resource values for the SimpleSpinBox child.
     * These may have been changed either by internal operation
     * of the SpinBox or by the application doing a SetValues on
     * the SimpleSpinBox child.
     */
    XtVaGetValues(ssb_p->text_field,
		  XmNarrowSensitivity, &ssb_p->arrow_sensitivity,
		  XmNdecimalPoints, &ssb_p->decimal_points,
		  XmNincrementValue, &ssb_p->increment_value,
		  XmNmaximumValue, &ssb_p->maximum_value,
		  XmNminimumValue, &ssb_p->minimum_value,
		  XmNnumValues, &ssb_p->num_values,
		  XmNposition, &ssb_p->position,
		  XmNspinBoxChildType, &ssb_p->sb_child_type,
		  XmNvalues, &ssb_p->values,
		  XmNwrap, &ssb_p->wrap,

		  XmNeditable, &ssb_p->editable,
		  XmNcolumns, &ssb_p->columns,
		  NULL);

    /*
     * Error checking.
     */
    if (ssb_p->sb_child_type != XmSTRING) {
      _XmAppUnlock(app);
      return;
    }

    if (item == (XmString) NULL) {
      _XmAppUnlock(app);
      return;
    }

    /*
     * BINARY COMPATIBILITY with DTSPINBOX
     *
     * User gives pos starting at 1 (0 means end of list)
     */
    pos--;
    if ((pos < 0) || (pos > ssb_p->num_values))
      pos = ssb_p->num_values;

    new_nvalues = ssb_p->num_values + 1;

    /*
     * Keep the position up to date.
     */
    if (ssb_p->position > pos)
      ssb_p->position++;

    /*
     * Copy the current array of values adding in the new item.
     */
    values = (XmStringTable)
	     XtRealloc((char *) NULL, sizeof(XmString) * new_nvalues);
    if (values == (XmStringTable) NULL) {
      _XmAppUnlock(app);
      return;
    }

    for (i=0; i<pos; i++)
      values[i] = XmStringCopy(ssb_p->values[i]);
    values[pos] =  XmStringCopy(item);
    for (i=pos+1; i<new_nvalues; i++)
      values[i] = XmStringCopy(ssb_p->values[i-1]);

    /*
     * Set the values array in the parent and save the XmStringTable.
     */
    XtVaSetValues(ssb_p->text_field,
    		  XmNvalues, values,
    		  XmNnumValues, new_nvalues,
    		  XmNposition, ssb_p->position,
		  NULL);
    textf_c = SB_GetConstraintRec(ssb_p->text_field);
    ssb_p->values = textf_c->values;
    ssb_p->num_values = textf_c->num_values;
    ssb_p->position = textf_c->position;

   /*
    * Free up the memory in the values array.
    */
    for (i=0; i<new_nvalues; i++)
      if (values[i] != (XmString) NULL)
        XmStringFree(values[i]);

    XtFree((char *) values);
    _XmAppUnlock(app);
}

/************************************************************************
 *  XmSimpleSpinBoxDeletePos
 *	Delete the item at the specified position from the list of strings.
 ************************************************************************/
void
XmSimpleSpinBoxDeletePos(
			Widget		ssb_w,
			int	 	pos)
{
    XmSimpleSpinBoxPart	*ssb_p = SSB_SIMPLE_SPIN_BOX_PART(ssb_w);
    XmSpinBoxConstraint	textf_c;
    int			i, skipped;
    int			new_nvalues;
    XmStringTable	values;
    _XmWidgetToAppContext(ssb_w);

    _XmAppLock(app);

    /*
     * Get the latest resource values for the SimpleSpinBox child.
     * These may have been changed either by internal operation
     * of the SpinBox or by the application doing a SetValues on
     * the SimpleSpinBox child.
     */
    XtVaGetValues(ssb_p->text_field,
		  XmNarrowSensitivity, &ssb_p->arrow_sensitivity,
		  XmNdecimalPoints, &ssb_p->decimal_points,
		  XmNincrementValue, &ssb_p->increment_value,
		  XmNmaximumValue, &ssb_p->maximum_value,
		  XmNminimumValue, &ssb_p->minimum_value,
		  XmNnumValues, &ssb_p->num_values,
		  XmNposition, &ssb_p->position,
		  XmNspinBoxChildType, &ssb_p->sb_child_type,
		  XmNvalues, &ssb_p->values,
		  XmNwrap, &ssb_p->wrap,

		  XmNeditable, &ssb_p->editable,
		  XmNcolumns, &ssb_p->columns,
		  NULL);

    /*
     * Error checking.
     */
    if ((ssb_p->sb_child_type != XmSTRING) || (ssb_p->num_values < 1)) {
      _XmAppUnlock(app);
      return;
    }

    /*
     * BINARY COMPATIBILITY with DTSPINBOX
     *
     * User gives pos starting at 1 (0 means end of list)
     */
    pos--;
    if ((pos < 0) || (pos > ssb_p->num_values))
      pos = ssb_p->num_values - 1;

    new_nvalues = ssb_p->num_values - 1;

    /*
     * Keep the position up to date.
     */
    if (ssb_p->position > pos)
      ssb_p->position--;

    /*
     * Copy the current array of values skipping the item in position 'pos'.
     */
    values = (XmStringTable)
	     XtRealloc((char *) NULL, sizeof(XmString) * new_nvalues);
    if (values == (XmStringTable) NULL) {
      _XmAppUnlock(app);
      return;
    }

    for (i=0, skipped=0; i<ssb_p->num_values; i++)
      if (i != pos)
        values[i-skipped] = XmStringCopy(ssb_p->values[i]);
      else
	skipped++;

    /*
     * Set the values array in the parent and save the XmStringTable.
     */
    XtVaSetValues(ssb_p->text_field,
    		  XmNvalues, values,
    		  XmNnumValues, new_nvalues,
    		  XmNposition, ssb_p->position,
		  NULL);
    textf_c = SB_GetConstraintRec(ssb_p->text_field);
    ssb_p->values = textf_c->values;
    ssb_p->num_values = textf_c->num_values;
    ssb_p->position = textf_c->position;

   /*
    * Free up the memory in the values array.
    */
    for (i=0; i<new_nvalues; i++)
      if (values[i] != (XmString) NULL)
        XmStringFree(values[i]);

    XtFree((char *) values);
    _XmAppUnlock(app);
}

/************************************************************************
 *  XmSimpleSpinBoxDeletePos
 *	Make the given item the currently visible item in the text-field
 *	or label.
 ************************************************************************/
void
XmSimpleSpinBoxSetItem(	Widget		ssb_w,
			XmString 	item)
{
    XmSimpleSpinBoxPart	*ssb_p = SSB_SIMPLE_SPIN_BOX_PART(ssb_w);
    XmSpinBoxConstraint	textf_c;
    int			pos;
    Arg			args[1];
    _XmWidgetToAppContext(ssb_w);

    _XmAppLock(app);

    /*
     * Get the latest resource values for the SimpleSpinBox child.
     * These may have been changed either by internal operation
     * of the SpinBox or by the application doing a SetValues on
     * the SimpleSpinBox child.
     */
    XtVaGetValues(ssb_p->text_field,
		  XmNarrowSensitivity, &ssb_p->arrow_sensitivity,
		  XmNdecimalPoints, &ssb_p->decimal_points,
		  XmNincrementValue, &ssb_p->increment_value,
		  XmNmaximumValue, &ssb_p->maximum_value,
		  XmNminimumValue, &ssb_p->minimum_value,
		  XmNnumValues, &ssb_p->num_values,
		  XmNposition, &ssb_p->position,
		  XmNspinBoxChildType, &ssb_p->sb_child_type,
		  XmNvalues, &ssb_p->values,
		  XmNwrap, &ssb_p->wrap,

		  XmNeditable, &ssb_p->editable,
		  XmNcolumns, &ssb_p->columns,
		  NULL);

    if (item && ssb_p->num_values > 0) {
	for (pos=0; pos<ssb_p->num_values; pos++)
	  if (XmStringCompare(item, ssb_p->values[pos]))
	    break;

	if (pos < ssb_p->num_values) {
	    XtSetArg(args[0], XmNposition, pos);
            XtSetValues(ssb_p->text_field, args, 1);
            textf_c = SB_GetConstraintRec(ssb_p->text_field);
            ssb_p->position = textf_c->position;
        }
	else
	  XmeWarning((Widget) ssb_w, BAD_SSPIN_SET_ITEM);
    }
    _XmAppUnlock(app);
}
