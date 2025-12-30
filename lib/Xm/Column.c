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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>
#include <Xm/ColumnP.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/VaSimpleP.h>
#include "XmI.h"

#define XK_LATIN1
#include <X11/keysymdef.h>

static void ClassInitialize(void);

static void Initialize(Widget, Widget, ArgList, Cardinal*);
static void ClassPartInitialize(WidgetClass w_class);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal*);
static void Destroy(Widget);
static void Resize(Widget);
static XtGeometryResult QueryGeometry(Widget,
        XtWidgetGeometry*, XtWidgetGeometry*);
static XtGeometryResult GeometryManager(Widget,
        XtWidgetGeometry*, XtWidgetGeometry*);
static void ChangeManaged(Widget);
static void ConstraintInitialize(Widget, Widget, ArgList, Cardinal*);
static Boolean ConstraintSetValues(Widget, Widget, Widget, ArgList, Cardinal*);
static void ConstraintDestroy(Widget);
static void ConstraintGetValues(Widget, ArgList, Cardinal*);


static Boolean CvtStringToXiAlignment(
	Display*, XrmValue*, Cardinal*, XrmValue*, XrmValue*, XtPointer
);
static Boolean CvtStringToFillStyle(
	Display*, XrmValue*, Cardinal*, XrmValue*, XrmValue*, XtPointer
);
static Boolean CvtStringToDistribution(
	Display*, XrmValue*, Cardinal*, XrmValue*, XrmValue*, XtPointer
);

static void Layout(
	XmColumnWidget, Widget, XtWidgetGeometry*, int, int
);
static void HorizontalLayout(
	XmColumnWidget, Widget, XtWidgetGeometry*, int, int
);
static void VerticalLayout(
	XmColumnWidget, Widget, XtWidgetGeometry*, int, int
);
static void VerifyResources(
	XmColumnWidget, XmColumnWidget, XmColumnWidget
);
static void VerifyConstraints(
	Widget, Widget, Widget
);
static void CalcSize(
	XmColumnWidget, Widget, XtWidgetGeometry*, Boolean, Dimension*, Dimension*
);
static Boolean CompareGeometry(

	XtWidgetGeometry*, XtWidgetGeometry*

);
static Boolean CompareGeometryToWidget(
	XtWidgetGeometry*, Widget

);
static void CheckSetEntryLabelRenderTable(Widget wid, int offs, XrmValue *value);
static void CheckSetDefaultEntryLabelRenderTable(Widget wid, int offs, XrmValue *value);
static void XmColumnLabelDestroyedCallback(
	Widget, XtPointer, XtPointer
);

#if 0	/* POSITION HANDLING */
	Note: this code was never finished and has been pulled out. The
	public and semi-public traces have been pulled out of the header files.
	Everything is marked with the #if used above.
#endif

#define BBPart(w) ((XmBulletinBoardPart*)(&(((XmBulletinBoardWidget)(w))->bulletin_board)))
#define XiC(w) ((XmColumnConstraintPart*)(&((XmColumnConstraintPtr)((w)->core.constraints))->column))
#define XiValidChild(c) (((c)) != NULL && XtIsManaged((c)) && \
			 !(c)->core.being_destroyed && \
			 XiC(c)->label_widget != NULL)
#define XmColumn(c) ((XmColumnWidget) XtParent((c)))
#define XiAlignment(c) ((XiC((c))->label_alignment == XmALIGNMENT_UNSPECIFIED) \
			? XmColumn_default_label_alignment(XmColumn((c))) \
			: XiC(c)->label_alignment)
#if 0	/* POSITION HANDLING */
#define XiPosition(c) ((XiC(c)->label_position == XiLABEL_POSITION_UNSPECIFIED)\
		       ? XmColumn_default_label_position(XmColumn(c)) \
		       : XiC(c)->label_position)
#endif
#define XiFill(c) ((XiC(c)->fill_style == XmFILL_UNSPECIFIED)\
		   ? XmColumn_default_fill_style(XmColumn(c)) \
		   : XiC(c)->fill_style)

#define XiWidth(c) (XtWidth(c) + 2 * XtBorderWidth(c))
#define XiHeight(c) (XtHeight(c) + 2 * XtBorderWidth(c))


#if 0	/* POSITION HANDLING */

/* from public .h file */
#define XiLABEL_POSITION_UNSPECIFIED	0
#define XiLABEL_POSITION_CENTER 	(1L<<0)
#define XiLABEL_POSITION_LEFT		(1L<<1)
#define XiLABEL_POSITION_RIGHT		(1L<<2)
#define XiLABEL_POSITION_TOP		(1L<<3)
#define XiLABEL_POSITION_BOTTOM		(1L<<4)

/* structure member elements from private P.h file */
    unsigned char default_label_position;
    unsigned char       label_position;
#define XmColumnC_label_position(w) XmColCField(w, label_position, unsigned char)
#define XmColumn_default_label_position(w) XmColField(w, default_label_position, unsigned char)

#endif

#define DEFAULT_ALIGNMENT XmALIGNMENT_BEGINNING
#if 0	/* POSITION HANDLING */
#define DEFAULT_POSITION XiLABEL_POSITION_LEFT
#endif
#define DEFAULT_ORIENTATION XmVERTICAL
#define DEFAULT_FILL_STYLE XmFILL_RAGGED

static XtResource resources[] =
{
  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmColumnRec, column.check_set_render_table),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNdefaultEntryLabelFontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmBulletinBoardRec, bulletin_board.label_font_list),
    XmRCallProc, (XtPointer) CheckSetDefaultEntryLabelRenderTable
  },

  {
    XmNdefaultEntryLabelRenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmBulletinBoardRec, bulletin_board.label_font_list),
    XmRCallProc, (XtPointer) CheckSetDefaultEntryLabelRenderTable
  },

  {
    XmNdefaultEntryLabelAlignment, XmCAlignment, XmRXmAlignment,
    sizeof(unsigned char), XtOffsetOf(XmColumnRec, column.default_label_alignment),
    XmRImmediate, (XtPointer) DEFAULT_ALIGNMENT
  },
#if 0	/* POSITION HANDLING */
  {
    XmNdefaultEntryLabelPosition, XmCEntryLabelPosition, XmRLabelPosition,
    sizeof(unsigned char), XtOffsetOf(XmColumnRec, column.default_label_position),
    XmRImmediate, (XtPointer) DEFAULT_POSITION
  },
#endif
  {
    XmNdefaultFillStyle, XmCFillStyle, XmRFillStyle,
    sizeof(unsigned char), XtOffsetOf(XmColumnRec, column.default_fill_style),
    XmRImmediate, (XtPointer) DEFAULT_FILL_STYLE
  },

  {
    XmNitemSpacing, XmCItemSpacing, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmColumnRec, column.item_spacing),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNlabelSpacing, XmCLabelSpacing, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmColumnRec, column.label_spacing),
    XmRImmediate, (XtPointer) 10
  },

  {
    XmNorientation, XmCOrientation, XmROrientation,
    sizeof(unsigned char), XtOffsetOf(XmColumnRec, column.orientation),
    XmRImmediate, (XtPointer) DEFAULT_ORIENTATION
  },

  {
    XmNdistribution, XmCDistribution, XmRDistribution,
    sizeof(unsigned char), XtOffsetOf(XmColumnRec, column.distribution),
    XmRImmediate, (XtPointer) XmDISTRIBUTE_TIGHT
  }
};

static XmSyntheticResource get_resources[] =
{
  {
    XmNlabelSpacing, sizeof(Dimension),
    XtOffsetOf(XmColumnRec, column.label_spacing),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNitemSpacing, sizeof(Dimension),
    XtOffsetOf(XmColumnRec, column.item_spacing),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  }
};

static XtResource constraint_resources[] =
{
  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmColumnConstraintRec, column.check_set_render_table),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNentryLabelFontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmColumnConstraintRec, column.label_font_list),
    XmRCallProc, (XtPointer) CheckSetEntryLabelRenderTable
  },

  {
    XmNentryLabelRenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmColumnConstraintRec, column.label_font_list),
    XmRCallProc, (XtPointer) CheckSetEntryLabelRenderTable
  },

  {
    XmNentryLabelAlignment, XmCAlignment, XmRXmAlignment,
    sizeof(unsigned char), XtOffsetOf(XmColumnConstraintRec, column.label_alignment),
    XmRImmediate, (XtPointer) XmALIGNMENT_UNSPECIFIED
  },
#if 0	/* POSITION HANDLING */
  {
    XmNentryLabelPosition, XmCEntryLabelPosition, XmRLabelPosition,
    sizeof(unsigned char), XtOffsetOf(XmColumnConstraintRec, column.label_position),
    XmRImmediate, (XtPointer) XiLABEL_POSITION_UNSPECIFIED
  },
#endif
  {
    XmNfillStyle, XmCFillStyle, XmRFillStyle,
    sizeof(unsigned char), XtOffsetOf(XmColumnConstraintRec, column.fill_style),
    XmRImmediate, (XtPointer) XmFILL_UNSPECIFIED
  },

  {
    XmNentryLabelType, XmCLabelType, XmRLabelType,
    sizeof(unsigned char), XtOffsetOf(XmColumnConstraintRec, column.label_type),
    XmRImmediate, (XtPointer) XmSTRING
  },

  {
      XmNentryLabelString, XmCLabelString, XmRXmString,
      sizeof(XmString), XtOffsetOf(XmColumnConstraintRec, column.label_string),
      XmRImmediate, (XtPointer) NULL
  },

  {
      XmNentryLabelPixmap, XmCLabelPixmap, XmRPrimForegroundPixmap,
      sizeof(Pixmap), XtOffsetOf(XmColumnConstraintRec, column.label_pixmap),
      XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
      XmNshowEntryLabel, XmCShowLabel, XmRBoolean,
      sizeof(Boolean), XtOffsetOf(XmColumnConstraintRec, column.show_label),
      XmRImmediate, (XtPointer) True
  },

  {
      XmNstretchable, XmCStretchable, XmRBoolean,
      sizeof(Boolean), XtOffsetOf(XmColumnConstraintRec, column.stretchable),
      XmRImmediate, (XtPointer) False
  }
};

/*
 * Synthetic constraints.  Note that these are NOT currently used,
 * as the constraint get_values_hook method below seems to work where
 * synthetic resources don't.  Dunno.
 */
static void Get_entryLabelString(Widget, int, XtArgVal *);
static XmSyntheticResource cont_get_resources[] =
{
  {
    XmNentryLabelString, sizeof(XmString),
    XtOffsetOf(XmColumnConstraintRec, column.label_string),
    Get_entryLabelString, (XmImportProc) NULL
  }
};

ConstraintClassExtensionRec xiColumnConstraintExtension = {
    NULL,			         /* next_extension  */
    NULLQUARK,			         /* record_type     */
    XtConstraintExtensionVersion,        /* version         */
    sizeof(ConstraintClassExtensionRec), /* record_size     */
    ConstraintGetValues		         /* get_values_hook */
};

XmColumnClassRec xmColumnClassRec = {
  {
    /* core_class members      */
    /* superclass         */	(WidgetClass) &xmBulletinBoardClassRec,
    /* class_name         */	"XmColumn",
    /* widget_size        */	sizeof(XmColumnRec),
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
    /* destroy            */	Destroy,
    /* resize             */	Resize,
    /* expose             */	XtInheritExpose,
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
    /* insert_child       */	XtInheritInsertChild,
    /* delete_child       */	XtInheritDeleteChild,
    /* extension          */	NULL,
  },
  { /* constraint_class fields */
    /* resource list      */    (XtResource*)constraint_resources,
    /* num resources      */ 	XtNumber(constraint_resources),
    /* constraint size    */ 	sizeof(XmColumnConstraintRec),
    /* init proc          */ 	ConstraintInitialize,
    /* destroy proc       */ 	ConstraintDestroy,
    /* set values proc    */ 	ConstraintSetValues,
    /* extension          */ 	&xiColumnConstraintExtension,
  },
  { /* manager_class fields */
    /* default translations */ 	XtInheritTranslations,
    /* syn_resources        */ 	get_resources,
    /* num_syn_resources    */ 	XtNumber(get_resources),
    /* syn_cont_resources   */ 	NULL, /* cont_get_resources, */
    /* num_syn_cont_resources */ 0, /* XtNumber(cont_get_resources),*/
    /* parent_process       */ 	XmInheritParentProcess,
    /* extension            */ 	NULL,
  },
  { /* bulletin board members */
    /* always_install_accel */  False,
    /* geo_matrix_create    */  NULL,
    /* focus_moved_proc     */  XmInheritFocusMovedProc,
    /* extension            */  NULL,
  },
  {
    /* column class members */
    /* extension            */	NULL,
  }
};

WidgetClass xmColumnWidgetClass = (WidgetClass) &xmColumnClassRec;

/*
 * Function:
 *	ClassInitialize(void)
 * Description:
 *	This function is called the first time XmColumn or subclass is
 *	created.  This function is used to install all need type
 *	converters for this widget class.
 * Input:
 *	None.
 * Output:
 *	None.
 */
static void
ClassInitialize(void)
{
    XmColumnClassRec* wc = &xmColumnClassRec;

#if 0	/* POSITION HANDLING */
    XtSetTypeConverter(XmRString, XmRLabelPosition,
		       (XtTypeConverter) CvtStringToLabelPosition,
		       NULL, 0, XtCacheAll, NULL);
#endif
    XtSetTypeConverter(XmRString, XmRXmAlignment,
		       (XtTypeConverter) CvtStringToXiAlignment,
		       NULL, 0, XtCacheAll, NULL);
    XtSetTypeConverter(XmRString, XmRFillStyle,
		       (XtTypeConverter) CvtStringToFillStyle,
		       NULL, 0, XtCacheAll, NULL);
    XtSetTypeConverter(XmRString, XmRDistribution,
		       (XtTypeConverter) CvtStringToDistribution,
		       NULL, 0, XtCacheAll, NULL);
}

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmCOLUMN_BIT);
}

/*
 * Function:
 *	Initialize(request, set, arg_list, arg_cnt)
 * Description:
 *	This function is called to initialize the resource values for each
 *	new widget instance.  This function verifies the resource values
 *	and takes the needed actions to initialize the new instance.
 * Input:
 *	request  : Widget   - user resource requests
 *	set      : Widget   - the resource values for the new widget
 *	arg_list : ArgList  - the argument list used to set the resource
 *	ang_cnt  : Cardinal - the number of arguments in the list
 * Output:
 *	None.
 */
static void
Initialize(Widget request, Widget set, ArgList arg_list, Cardinal *arg_cnt)
{
    XmColumnWidget rc = (XmColumnWidget) request,
                   sc = (XmColumnWidget) set;

    VerifyResources(rc, (XmColumnWidget) NULL, sc);

    if( rc->core.width == 0 )
    {
	sc->core.width = 2 * (rc->manager.shadow_thickness +
			      BBPart(rc)->margin_width);
    }
    if( rc->core.height == 0 )
    {
	sc->core.height = 2 * (rc->manager.shadow_thickness +
			       BBPart(rc)->margin_height);
    }
}

/*
 * Function:
 *	Destroy(widget)
 * Description:
 *	This function is called when an instance of the Column is being
 *	destroyed.  This function deallocates any memory allocated by the
 *	instance.
 * Input:
 *	widget : Widget - the widget being destroyed
 * Output:
 *	None.
 */
static void
Destroy(Widget widget)
{
    /* This space intentionally left blank */
}

/*
 * Function:
 *	Resize(widget)
 * Description:
 *	This function is called when an instance changes size.  This
 *	function needs to adjust the childrens sizes and positions
 *	appropriately for the new size.
 * Input:
 *	widget : Widget - the widget that changed size.
 * Output:
 *	None.
 */
static void
Resize(Widget widget)
{
    WidgetClass    sc = XtSuperclass(widget);
    XmColumnWidget cw = (XmColumnWidget) widget;

    XtWidgetProc resize;

    _XmProcessLock();
    resize = *sc->core_class.resize;
    _XmProcessUnlock();
    (* resize) (widget);

    Layout(cw, NULL, NULL, -1, -1);

    XmColumn_resize_done(cw) = True;
}

/*
 * Function:
 *	SetValues(current, request, set, arg_list, arg_cnt)
 * Description:
 *	This function is called when the user changes the resource
 *	values for the column.  This function adjusts the columns
 *	appearance and behavior based on the new resource settings.
 * Input:
 *	current  : Widget  - the current resource values for the widget
 *	request  : Widget  - the requested resource values for the widget
 *	set      : Widget  - the new resource values for the widget
 *	arg_list : ArgList - the argument list used to change the resource
 *			     values
 *	arg_cnt  :Cardinal - the number of arguments
 * Output:
 *	Boolean - True if the Column needs redisplayed, else False.
 */
static Boolean
SetValues(Widget current, Widget request, Widget set, ArgList arg_list,
	  Cardinal *arg_cnt)
{
    XmColumnWidget cc = (XmColumnWidget) current,
                   cs = (XmColumnWidget) set;
    Boolean        request_size = False,
                   relayout = False;
    WidgetList     kid, kids = cs->composite.children;
    Cardinal       n, i, kidCnt = cs->composite.num_children;
    Arg            args[10];

    VerifyResources((XmColumnWidget) request, cc, cs);

    if( XmColumn_item_spacing(cc) != XmColumn_item_spacing(cs) ||
        XmColumn_label_spacing(cc) != XmColumn_label_spacing(cs) ||
        XmColumn_distribution(cc) != XmColumn_distribution(cs) ||
        XmColumn_orientation(cc) != XmColumn_orientation(cs) ||
        cc->manager.shadow_thickness != cs->manager.shadow_thickness ||
        BBPart(cc)->margin_width != BBPart(cs)->margin_width ||
        BBPart(cc)->margin_height != BBPart(cs)->margin_height)
    {
	request_size = True;
    }

    if( XmColumn_default_fill_style(cc) != XmColumn_default_fill_style(cs)
#if 0	/* POSITION HANDLING */
	|| XmColumn_default_label_position(cc) != XmColumn_default_label_position(cs)
#endif
	)
    {
	relayout = True;
    }

    n = 0;
    if( cc->core.background_pixel != cs->core.background_pixel )
    {
	XtSetArg(args[n], XmNbackground, cs->core.background_pixel); n++;
    }
    if( cc->manager.foreground != cs->manager.foreground )
    {
	XtSetArg(args[n], XmNforeground, cs->manager.foreground); n++;
    }

    for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
    {
	if( (*kid) == NULL || (*kid)->core.being_destroyed ||
	    XiC(*kid)->label_widget == NULL ) continue;

	XtSetValues(XiC(*kid)->label_widget, args, n);
    }

    if( BBPart(cc)->label_font_list !=
            BBPart(cs)->label_font_list )
    {
	for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
	{
	    if( (*kid) == NULL || (*kid)->core.being_destroyed ||
	       XiC(*kid)->label_widget == NULL ) continue;

	    if( XiC(*kid)->label_font_list == NULL )
	    {
		XtVaSetValues(XiC(*kid)->label_widget,
			      XmNrenderTable,
			      BBPart(cs)->label_font_list,
			      NULL);
	    }
	}
    }

    if( XmColumn_default_label_alignment(cc) !=
            XmColumn_default_label_alignment(cs) )
    {
	for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
	{
	    if( (*kid) == NULL || (*kid)->core.being_destroyed ||
	       XiC(*kid)->label_widget == NULL ) continue;

	    if( XiC(*kid)->label_alignment == XmALIGNMENT_UNSPECIFIED )
	    {
		XtVaSetValues(XiC(*kid)->label_widget,
			      XmNalignment,
			      XmColumn_default_label_alignment(cs),
			      NULL);
	    }
	}
    }

    if( request_size )
    {
	Dimension width, height;

	XmColumn_resize_done(cs) = False;
	CalcSize(cs, NULL, NULL, False, &width, &height);
	if( XtMakeResizeRequest((Widget) cs, width, height, &width,
			&height) == XtGeometryAlmost )
	{
	    XmColumn_resize_done(cs) = False;
	    XtMakeResizeRequest((Widget) cs, width, height, NULL, NULL);
	}

	relayout = !XmColumn_resize_done(cs);
    }

    if( relayout )
    {
	Resize((Widget) cs);
    }

    return( False );
}

/*
 * Function:
 *	QueryGeometry(widget, request, allowed)
 * Description:
 *	This function is called when someone wants to know how the Column
 *	would react to a specific geometry or when someone wants to know
 *	what size the Column wants to be.
 * Input:
 *	widget  : Widget            - the column widget
 *	request : XtWidgetGeometry* - the requested widget geometry
 *	allowed : XtWidgetGeometry* - the geometry column is will to accept
 * Output:
 *	XtWidgetResult - the columns response to the geometry request
 */
static XtGeometryResult
QueryGeometry(Widget widget, XtWidgetGeometry *request,
	      XtWidgetGeometry *wanted)
{
    XmColumnWidget   cw = (XmColumnWidget) widget;
    XtGeometryResult result;
    Dimension        width, height;

    /*
     * Lets start by calling this handly dandy function to calculate
     * the geometry we want to be.
     */
    CalcSize(cw, NULL, NULL, False, &width, &height);

    /*
     * Now lets see if the caller requested anything and if not
     * lets give him our prefered geometry.
     */
    if( request->request_mode == 0 )
    {
	wanted->request_mode = CWWidth | CWHeight;
	wanted->width = width;
	wanted->height = height;

	if( width == XtWidth(cw) && height == XtHeight(cw) )
	{
	    return( XtGeometryNo );
	}
	return( XtGeometryAlmost );
    }

    *wanted = *request;
    if( request->request_mode & CWWidth )
    {
	if( request->width < width )
	{
	    wanted->width = width;
	}
    }

    if( request->request_mode & CWHeight )
    {
	if( request->height < height )
	{
	    wanted->height = height;
	}
    }

    /*
     * Now we have set everything in our return structure to what we
     * want it to be.  Now all that is left is to come up with the
     * correct return result.  The return result is something like.
     *
     * o request == wanted and wanted != real	-> XtGeometryYes
     * o request == wanted and wanted == real	-> XtGeometryNo
     * o request != wanted and wanted != real	-> XtGeometryAlmost
     * o request != wanted and wanted == real   -> XtGeometryNo
     */

    /*
     * Lets first see if request == wanted
     */
    if( CompareGeometryToWidget(wanted, (Widget) cw) )
    {
	result = XtGeometryNo;
    }
    else
    {
	if( CompareGeometry(request, wanted) )
	{
	    result = XtGeometryYes;
	}
	else
	{
	    result = XtGeometryNo;
	}
    }

    return( result );
}

/*
 * Function:
 *	GeometryManager(widget, request, allowed)
 * Description:
 *	This function is called when a child of the Column would like
 *	to change size.  This function reacts to the child's request.
 * Input:
 *	widget  : Widget            - the child requesting the change
 *	request : XtWidgetGeometry* - the requested widget geometry
 *	allowed : XtWidgetGeometry* - the geometry column is will to accept
 * Output:
 *	XtWidgetResult - the columns response to the geometry request
 */
static XtGeometryResult
GeometryManager(Widget widget, XtWidgetGeometry *request,
		XtWidgetGeometry *allowed)
{
    XmColumnWidget   cw = (XmColumnWidget) XtParent(widget);
    Dimension        width, height, width_return, height_return;
    XtGeometryResult result;
    Boolean          equal;

    /*
     * Now being the mean manager that we are, we are only going to
     * allow our children to change geometry parts the affect their
     * size.  i.e. we will not even discuss with them their position,
     * this is mainly because we do not want to deal with it.
     */

    *allowed = *request;
    allowed->request_mode = request->request_mode =
	(request->request_mode & ~(CWX | CWY));

    /*
     * Now lets see what this child wants to change.
     */
    if( request->request_mode & CWWidth )
    {
	allowed->width = request->width;
    }
    else
    {
	allowed->width = XiC(widget)->request_width;
    }

    if( request->request_mode & CWHeight )
    {
	allowed->height = request->height;
    }
    else
    {
	allowed->height = XiC(widget)->request_height;
    }

    if( request->request_mode & CWBorderWidth )
    {
	allowed->border_width = request->border_width;
    }
    else
    {
	allowed->border_width = XtBorderWidth(widget);
    }

    /*
     * Now that we know what size our child want to be lets see
     * how that effects the geometry we want to be.
     */
    CalcSize(cw, widget, allowed, False, &width, &height);

    /*
     * Now that we know that size that we want to be, lets ask our
     * parent and see what they have to say.
     */
    if( request->request_mode & XtCWQueryOnly )
    {
	XtWidgetGeometry req, alw;

	req.request_mode = CWWidth | CWHeight | XtCWQueryOnly;
	req.width = width;
	req.height = height;

	width = XtWidth(cw);
	height = XtHeight(cw);
	switch( XtMakeGeometryRequest((Widget) cw, &req, &alw) )
	{
	case XtGeometryYes:
	    return( XtGeometryYes );
	    break;
	case XtGeometryAlmost:
	    if( alw.request_mode & CWWidth ) width = alw.width;
	    if( alw.request_mode & CWHeight ) height = alw.height;
	case XtGeometryNo:
	default:
	    Layout(cw, widget, allowed, width, height);

	    allowed->width = XiC(widget)->position.width;
	    allowed->height = XiC(widget)->position.height;
	    break;
	}
    }
    else
    {
	Dimension cur_width, cur_height;

	XiC(widget)->request_width = allowed->width;
	XiC(widget)->request_height = allowed->height;
	XmColumn_resize_done(cw) = False;

	cur_width = XtWidth(cw);
	cur_height = XtHeight(cw);
	switch( XtMakeResizeRequest((Widget) cw, width, height,
				    &width_return, &height_return) )
	{
	case XtGeometryYes:
	    /*
	     * It appears that our parent will has let us change to
	     * the size that we want to be, so lets see if we need to
	     * call our resize procedure.
	     */
	    if( !XmColumn_resize_done(cw) ) Resize((Widget) cw);

	    Layout(cw, widget, allowed, width, height);
	    allowed->width = XiC(widget)->position.width;
	    allowed->height = XiC(widget)->position.height;
	    break;
	case XtGeometryAlmost:
	    cur_width = width_return;
	    cur_height = height_return;
	case XtGeometryNo:
	default:
	    Layout(cw, widget, allowed, cur_width, cur_height);

	    allowed->width = XiC(widget)->position.width;
	    allowed->height = XiC(widget)->position.height;
	    break;
	}
    }

    width = XtWidth(widget);
    height = XtHeight(widget);
    equal = CompareGeometryToWidget(allowed, widget);

    if( equal )
    {
	result = XtGeometryNo;
    }
    else
    {
	if( CompareGeometry(request, allowed) )
	{
	    result = XtGeometryYes;
	}
	else
	{
	    result = XtGeometryAlmost;
	}
    }

    if( result == XtGeometryYes )
    {
	Layout(cw, NULL, NULL, -1, -1);
    }

    return( result );
}

/*
 * Function:
 *	ChangeManaged(widget)
 * Description:
 *	This is called when one of the children of the column changes
 *	managed state.  This routine just takes care of this situation.
 * Input:
 *	widget : Widget - the column widget
 * Output:
 *	None.
 */
static void
ChangeManaged(Widget widget)
{
    static Boolean in = False;
    XmColumnWidget cw = (XmColumnWidget) widget;
    WidgetList     kid = cw->composite.children;
    Widget         label;
    Cardinal       i;
    Dimension      width, height;

    if( in ) return;

    in = True;

    for( i = 0; i < cw->composite.num_children; ++i, ++kid )
    {
	if( !XiValidChild(*kid) )
	{
	  if( (*kid) != NULL && !XtIsManaged(*kid) )
	    {	/* CR03731 */
	      if (XiC(*kid)->label_widget) {
		XtUnmanageChild(XiC(*kid)->label_widget);
	      }
	      XiC(*kid)->request_width = 0;
	      XiC(*kid)->request_height = 0;
	    }
	    continue;
	}

	label = XiC(*kid)->label_widget;
	if( !XiC(*kid)->show_label )
	{
	    if( XtIsManaged(*kid) ) XtUnmanageChild(label);
	}
	else if( XtIsManaged(*kid) != XtIsManaged(label) )
	{
	    if( XtIsManaged(*kid) )
	    {
		XtManageChild(label);
	    }
	    else
	    {
		XtUnmanageChild(label);
	    }
	}

	if( XiC(*kid)->request_width == 0 && XtIsManaged(*kid) )
	{
	    XiC(*kid)->request_width = XtWidth(*kid);
	    XiC(*kid)->request_height = XtHeight(*kid);
	}
	if( XtIsManaged(label) )
	{
	    if( XiC(label)->request_width == 0 )
	    {
#if 0
		XiC(label)->request_width = XtWidth(label);
		XiC(label)->request_height = XtHeight(label);
#else
		{
		/* Unfortunately, XtWidth() and XtHeight() may not be valid in
		** this case. The request_width and request_height values are
		** used both to indicate real requested size and also, when 0,
		** to indicate a label with particular geometry needs.
		** However, if the code goes through ConstraintSetValues (in
		** code generated by the Xcessory tools, constraint resources
		** are set via set-values after widget creation) first, the
		** sizes are 0 and are then set to 1 in VerticalLayout; then
		** the code comes through here and updates the request values
		** from the real widths -- 1 -- which are then set back on the
		** widget as the geometry. The net result is that the labels
		** appear with width 1 (and correct height).
		**
		** Ideally we would cache the real initial requested value and
		** update from that value.
		**
		** For now, rather than storing those values, query the
		** label locally for its preferences and use those; the result
		** should be that the size is set correctly (going through
		** CalcSize and then VerticalLayout).
		*/

    		    XtWidgetGeometry wants;
		    XtQueryGeometry(label, NULL, &wants);

		    if( wants.request_mode & CWWidth )
		    {
			XiC(label)->request_width = wants.width;
		    }
		    else
		    {
			XiC(label)->request_width = XtWidth(label);
		    }

		    if( wants.request_mode & CWHeight )
		    {
			XiC(label)->request_height = wants.height;
		    }
		    else
		    {
			XiC(label)->request_height = XtHeight(label);
		    }
		}
#endif
	    }
	}
	else
	{
	    XiC(label)->request_width = 0;
	    XiC(label)->request_height = 0;
	}
    }

    CalcSize(cw, NULL, NULL, False, &width, &height);

    if( XtMakeResizeRequest(widget, width, height, &width, &height) ==
        XtGeometryAlmost )
    {
	XtMakeResizeRequest(widget, width, height, NULL, NULL);
    }

    Layout(cw, NULL, NULL, -1, -1);

    in = False;
}

/*
 * Function:
 *	ConstraintInitialize(request, new_w, arg_list, arg_cnt)
 * Description:
 *	This function initializes the constraint record for a new child,
 *	this includes adding a label for each child.
 * Input:
 *	request  : Widget   - the requested resource values
 *	new_w      : Widget   - the new_w resource values (the new widget)
 *	arg_list : ArgList  - the argument used to create the widget
 *	arg_cnt  : Cardinal - the number of arguments
 * Output:
 *	None.
 */
static void
ConstraintInitialize(Widget request, Widget new_w, ArgList arg_list,
		     Cardinal *arg_cnt)
{
    static         Boolean label_widget = False;	/* STATIC DATA */
    XmColumnWidget cw = (XmColumnWidget) XtParent(new_w);
    XmBulletinBoardPart *bbpart;

	/* CR03562 CR02961 When ChangeManaged is bypassed, request width and height
	   are not set. The 2 line will prevent this assumption which sometimes
	   will case the widget size to have zero width/height */
#if 1
	/* Note! below fix problematic w.r.t. ChangeManaged code, and needs
	** revisiting; back out temporarily
	*/
	/*
	** It is possible that the widget will have a constraint resource set
	** on it before the XmColumn itself is realized (ChangeManaged is
	** bypassed in that case), so that we enter the Layout code without
	** having the values request_width and request_height set from the
	** code in ChangeManaged. Rather than use 0 here, which gives poor
	** results when those values are used in XtConfigureWidget to set
	** the size, initialize them to reasonable default values.
	*/
	XiC(new_w)->request_width = XtWidth(new_w);
	XiC(new_w)->request_height = XtHeight(new_w);
#else
	XiC(new_w)->request_width = 0;
	XiC(new_w)->request_height = 0;
#endif

    XiC(new_w)->label_string = XmStringCopy(XiC(new_w)->label_string);
    if( label_widget )
    {
	XiC(new_w)->label_alignment = XmALIGNMENT_UNSPECIFIED;
#if 0	/* POSITION HANDLING */
	XiC(new_w)->label_position = XiLABEL_POSITION_UNSPECIFIED;
#endif
	XiC(new_w)->label_type = XmSTRING;
	XiC(new_w)->label_pixmap = XmUNSPECIFIED_PIXMAP;
	XiC(new_w)->label_string = (XmString) NULL;
	XiC(new_w)->label_widget = (Widget) NULL;
	XiC(new_w)->show_label = False;
    }
    else
    {
	Arg    args[64];
	int    nargs;
	char   buf[256];
	Widget label;
	XmFontList lfont;

	VerifyConstraints(request, NULL, new_w);

	if( strlen(XtName(new_w)) > 240 )
	{
	    strncpy(buf, XtName(new_w), 240);
	    buf[240] = '\0';
	    strcat(buf, "_label");
	}
	else
	{
	    strcpy(buf, XtName(new_w));
	    strcat(buf, "_label");
	}

	label_widget = True;

	lfont = XmColumnC_label_font_list(new_w);
	bbpart = BBPart(XtParent(new_w));
	if(lfont == NULL)
	    lfont = bbpart->label_font_list;

	nargs = 0;
	XtSetArg(args[nargs], XmNmarginWidth,        0); nargs++;
	XtSetArg(args[nargs], XmNmarginHeight,       0); nargs++;
	XtSetArg(args[nargs], XmNmarginTop,          0); nargs++;
	XtSetArg(args[nargs], XmNmarginBottom,       0); nargs++;
	XtSetArg(args[nargs], XmNmarginLeft,         0); nargs++;
	XtSetArg(args[nargs], XmNmarginRight,        0); nargs++;
	XtSetArg(args[nargs], XmNshadowThickness,    0); nargs++;
	XtSetArg(args[nargs], XmNhighlightThickness, 0); nargs++;
	XtSetArg(args[nargs], XmNtraversalOn,      False); nargs++;
	XtSetArg(args[nargs], XmNlabelType,   XiC(new_w)->label_type); nargs++;
	XtSetArg(args[nargs],
		 XmNlabelString, XiC(new_w)->label_string); nargs++;
	XtSetArg(args[nargs],
		 XmNlabelPixmap, XiC(new_w)->label_pixmap); nargs++;
	XtSetArg(args[nargs], XmNalignment,   XiAlignment(new_w));     nargs++;
	XtSetArg(args[nargs], XmNrenderTable,    lfont);          nargs++;
	XtSetArg(args[nargs], XmNrecomputeSize, True);                 nargs++;
	XtSetArg(args[nargs], XmNforeground,  cw->manager.foreground); nargs++;
	XtSetArg(args[nargs],
		 XmNbackground,  cw->core.background_pixel); nargs++;
	label = XtCreateWidget(buf, xmLabelWidgetClass, (Widget) cw,
		       args, nargs);
	XiC(new_w)->label_widget = label;

	XtAddCallback(label, XmNdestroyCallback,
		      XmColumnLabelDestroyedCallback, (XtPointer) new_w);
	XiC(label)->label_alignment = XmALIGNMENT_UNSPECIFIED;
#if 0	/* POSITION HANDLING */
	XiC(label)->label_position = XiLABEL_POSITION_UNSPECIFIED;
#endif
	XiC(label)->label_type = XmSTRING;
	XiC(label)->label_pixmap = XmUNSPECIFIED_PIXMAP;
	XiC(label)->label_string = (XmString) NULL;
	XiC(label)->label_widget = (Widget) NULL;
	XiC(label)->show_label = False;
	label_widget = False;
    }
}

/*
 * Function:
 *	ConstraintSetValues(current, request, new_w, arg_list, arg_cnt)
 * Description:
 *	This function is called when the user changes an attribute of
 * 	one of the columns children.  This routine reacts to the change.
 * Input:
 *	current  : Widget   - the current state of the child
 *	request  : Widget   - the request state of the child
 *	new_w      : Widget   - the child with the requested changes
 *	arg_list : ArgList  - the argument that modified the child
 *	arg_cnt  : Cardinal - the number of argument
 * Output:
 *	Boolean - True if the child needs to be redisplayed, else False.
 */
static Boolean
ConstraintSetValues(Widget current, Widget request, Widget new_w,
		    ArgList arg_list, Cardinal *arg_cnt)
{
    XmColumnWidget         cw = (XmColumnWidget) XtParent(new_w);
    XmColumnConstraintPart *cc = XiC(current),
                           *sc = XiC(new_w);
    Boolean                relayout = False;
    Arg                    args[10];
    Cardinal               i = 0;

    if( XiC(new_w)->label_widget == NULL ) return( False );

    VerifyConstraints(request, current, new_w);

    if(
#if 0	/* POSITION HANDLING */
	cc->label_position != sc->label_position ||
#endif
        cc->fill_style != sc->fill_style ||
        cc->show_label != sc->show_label )
    {
	relayout = True;
    }

    if( cc->label_font_list != sc->label_font_list )
    {
	XmFontList lfont = XmColumnC_label_font_list(new_w);
	if(lfont == NULL)
	    lfont = BBPart(XtParent(new_w))->label_font_list;
	XtSetArg(args[i], XmNrenderTable, lfont); ++i;
    }

    if( cc->label_alignment != sc->label_alignment )
    {
	XtSetArg(args[i], XmNalignment, XiAlignment(new_w)); ++i;
    }

    if( cc->label_string != sc->label_string )
    {
	XmStringFree(cc->label_string);
	sc->label_string = XmStringCopy(sc->label_string);
	XtSetArg(args[i], XmNlabelString, sc->label_string); ++i;
    }

    if( cc->label_pixmap != sc->label_pixmap )
    {
	XtSetArg(args[i], XmNlabelPixmap, sc->label_pixmap); ++i;
    }

    if( cc->label_type != sc->label_type )
    {
	XtSetArg(args[i], XmNlabelType, sc->label_type); ++i;
    }

    if( i >  0 ) XtSetValues(sc->label_widget, args, i);

    XmColumn_resize_done(cw) = False;
    if( cc->show_label != sc->show_label )
    {
	if( sc->show_label )
	{
	    XtManageChild(sc->label_widget);
	}
	else
	{
	    XtUnmanageChild(sc->label_widget);
	}
    }

    if( relayout && !XmColumn_resize_done(cw) )
    {
	Layout(cw, NULL, NULL, -1, -1);
    }

    return( False );
}

/*
 * Function:
 *	ConstraintDestroy(widget)
 * Description:
 *	This function is called to deallocate any resources allocated by
 *	the constraint record.
 * Input:
 *	widget : Widget - the widget being destroyed.
 * Output:
 *	None.
 */
static void
ConstraintDestroy(Widget widget)
{
    XmStringFree(XiC(widget)->label_string);
    if( XiC(widget)->label_widget != NULL )
    {
	XtRemoveCallback(XiC(widget)->label_widget, XmNdestroyCallback,
			 XmColumnLabelDestroyedCallback, (XtPointer) widget);
	XtDestroyWidget(XiC(widget)->label_widget);
	XiC(widget)->label_widget = NULL;
    }
}

/*
 * Function:
 *      ConstraintGetValues
 * Description:
 *      The constraint get_values_hook method.  It makes copies of the
 *      XmNentryLabelString values to prevent returning internal
 *      XmString data.
 * Input:
 *      w - The child widget
 *      args - Array of Arg structures
 *      num_args - Number of entries in args
 * Output:
 *      none (other than the silently copies XmString value)
 */
static void
ConstraintGetValues(Widget w, ArgList args, Cardinal *num_args)
{
    XrmQuark                    quark;
    int                         i;

    quark = XrmStringToQuark(XmNentryLabelString);
    for (i = 0; i < ((int) *num_args); i++)
    {
        if (quark == XrmStringToQuark(args[i].name))
        {
            args[i].value =
		(XtArgVal) XmStringCopy(XmColumnC_label_string(w));
            break;
        }
    }
}

static int
CompareISOLatin1 (char *first, char *second)
{
    unsigned char *ap, *bp;

    for (ap = (unsigned char *) first, bp = (unsigned char *) second;
         *ap && *bp; ap++, bp++) {
        unsigned char a, b;

        if ((a = *ap) != (b = *bp)) {
            /* try lowercasing and try again */

            if ((a >= XK_A) && (a <= XK_Z))
		a += (XK_a - XK_A);
            else if ((a >= XK_Agrave) && (a <= XK_Odiaeresis))
              a += (XK_agrave - XK_Agrave);
            else if ((a >= XK_Ooblique) && (a <= XK_Thorn))
              a += (XK_oslash - XK_Ooblique);

            if ((b >= XK_A) && (b <= XK_Z))
              b += (XK_a - XK_A);
            else if ((b >= XK_Agrave) && (b <= XK_Odiaeresis))
              b += (XK_agrave - XK_Agrave);
            else if ((b >= XK_Ooblique) && (b <= XK_Thorn))
              b += (XK_oslash - XK_Ooblique);

            if (a != b) break;
        }
    }
    return (((int) *bp) - ((int) *ap));
}

/*
 * Function:
 *	done
 * Description:
 *	This macro is used by the resource conversion routines and
 *	simply assigns the value and returns the correct result.
 */
#define done(type, value) 				\
    {							\
	if( to->addr != NULL )				\
	{						\
	    if( to->size < sizeof(type) )		\
	    {						\
		to->size = sizeof(type);		\
		return( False );			\
	    }						\
	    *(type*)(to->addr) = (value);		\
	}						\
	else						\
	{						\
	    static type static_val;			\
	    static_val = (value);			\
	    to->addr = (XtPointer) &static_val;		\
	}						\
	to->size = sizeof(type);			\
	return( True );					\
    }

#if 0	/* POSITION HANDLING */
/*
 * Function:
 *	CvtStringToLabelPosition(dpy, args, arg_cnt, from, to, data)
 * Description:
 *	This function converts a string representation of the representation
 *	type XmRLabelPosition to an actual value.
 * Input:
 *	dpy     : Display   - unused
 *	args    : XrmValue* - unused
 *	arg_cnt : Cardinal  - unused
 *	from    : XrmValue* - contains the string representation of the value
 *	to      : XrmValue* - returns the actual value
 *	data    : XtPointer - unused
 * Output:
 *	Boolean - True if the conversion was successful else False.
 */
static Boolean
CvtStringToLabelPosition(Display *dpy, XrmValue *args, Cardinal *arg_cnt,
			 XrmValue *from, XrmValue *to, XtPointer data)
{
    unsigned char result = XiLABEL_POSITION_LEFT;
    String        str = (String) (from->addr);

    if( CompareISOLatin1(str, "label_position_unspecified") == 0 ||
        CompareISOLatin1(str, "unspecified") == 0 )
    {
	result = XiLABEL_POSITION_UNSPECIFIED;
    }
    else if( CompareISOLatin1(str, "label_position_left") == 0 ||
	     CompareISOLatin1(str, "left") == 0 )
    {
	result = XiLABEL_POSITION_LEFT;
    }
    else if( CompareISOLatin1(str, "label_position_right") == 0 ||
	     CompareISOLatin1(str, "right") == 0 )
    {
	result = XiLABEL_POSITION_RIGHT;
    }
    else if( CompareISOLatin1(str, "label_position_top") == 0 ||
	     CompareISOLatin1(str, "top") == 0 )
    {
	result = XiLABEL_POSITION_TOP;
    }
    else if( CompareISOLatin1(str, "label_position_bottom") == 0 ||
	     CompareISOLatin1(str, "bottom") == 0 )
    {
	result = XiLABEL_POSITION_BOTTOM;
    }
    else if( CompareISOLatin1(str, "label_position_center") == 0 ||
	     CompareISOLatin1(str, "CENTER") == 0 )
    {
	result = XiLABEL_POSITION_CENTER;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, from->addr, XmRLabelPosition);
	return( False );
    }

    done(unsigned char, result);
}
#endif

/*
 * Function:
 *	CvtStringToXiAlignment(dpy, args, arg_cnt, from, to, data)
 * Description:
 *	This function converts a string representation of the representation
 *	type XmRXnAlignment to an actual value.
 * Input:
 *	dpy     : Display   - unused
 *	args    : XrmValue* - unused
 *	arg_cnt : Cardinal  - unused
 *	from    : XrmValue* - contains the string representation of the value
 *	to      : XrmValue* - returns the actual value
 *	data    : XtPointer - unused
 * Output:
 *	Boolean - True if the conversion was successful else False.
 */
static Boolean
CvtStringToXiAlignment(Display *dpy, XrmValue *args, Cardinal *arg_cnt,
		       XrmValue *from, XrmValue *to, XtPointer data)

{
    unsigned char result = XmALIGNMENT_CENTER;
    String        str = (String) (from->addr);

    if( CompareISOLatin1(str, "alignment_unspecified") == 0 ||
        CompareISOLatin1(str, "unspecified") == 0 )
    {
	result = XmALIGNMENT_UNSPECIFIED;
    }
    else if( CompareISOLatin1(str, "alignment_beginning") == 0 ||
	     CompareISOLatin1(str, "beginning") == 0 )
    {
	result = XmALIGNMENT_BEGINNING;
    }
    else if( CompareISOLatin1(str, "alignment_center") == 0 ||
	     CompareISOLatin1(str, "center") == 0 )
    {
	result = XmALIGNMENT_CENTER;
    }
    else if( CompareISOLatin1(str, "alignment_end") == 0 ||
	     CompareISOLatin1(str, "end") == 0 )
    {
	result = XmALIGNMENT_END;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, from->addr, XmRXmAlignment);
	return( False );
    }

    done(unsigned char, result);
}

/*
 * Function:
 *	CvtStringToFillStyle(dpy, args, arg_cnt, from, to, data)
 * Description:
 *	This function converts a string representation of the representation
 *	type XmRFillStyle to an actual value.
 * Input:
 *	dpy     : Display   - unused
 *	args    : XrmValue* - unused
 *	arg_cnt : Cardinal  - unused
 *	from    : XrmValue* - contains the string representation of the value
 *	to      : XrmValue* - returns the actual value
 *	data    : XtPointer - unused
 * Output:
 *	Boolean - True if the conversion was successful else False.
 */
static Boolean
CvtStringToFillStyle(Display *dpy, XrmValue *args, Cardinal *arg_cnt,
		     XrmValue *from, XrmValue *to, XtPointer data)
{
    unsigned char result = XmFILL_UNSPECIFIED;
    String        str = (String) (from->addr);

    if( CompareISOLatin1(str, "fill_unspecified") == 0 ||
        CompareISOLatin1(str, "unspecified") == 0 )
    {
	result = XmFILL_UNSPECIFIED;
    }
    else if( CompareISOLatin1(str, "fill_flush") == 0 ||
	     CompareISOLatin1(str, "flush") == 0 )
    {
	result = XmFILL_FLUSH;
    }
    else if( CompareISOLatin1(str, "fill_ragged") == 0 ||
	     CompareISOLatin1(str, "ragged") == 0 )
    {
	result = XmFILL_RAGGED;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, from->addr, XmRFillStyle);
	return( False );
    }

    done(unsigned char, result);
}

/*
 * Function:
 *	CvtStringToDistribution(dpy, args, arg_cnt, from, to, data)
 * Description:
 *	This function converts a string representation of the representation
 *	type XmRDistribution to an actual value.
 * Input:
 *	dpy     : Display   - unused
 *	args    : XrmValue* - unused
 *	arg_cnt : Cardinal  - unused
 *	from    : XrmValue* - contains the string representation of the value
 *	to      : XrmValue* - returns the actual value
 *	data    : XtPointer - unused
 * Output:
 *	Boolean - True if the conversion was successful else False.
 */
static Boolean
CvtStringToDistribution(Display *dpy, XrmValue *args, Cardinal *arg_cnt,
			XrmValue *from, XrmValue *to, XtPointer data)
{
    unsigned char result = XmDISTRIBUTE_TIGHT;
    String        str = (String) (from->addr);

    if( CompareISOLatin1(str, "distribute_tight") == 0 ||
        CompareISOLatin1(str, "tight") == 0 )
    {
	result = XmDISTRIBUTE_TIGHT;
    }
    else if( CompareISOLatin1(str, "distribute_spread") == 0 ||
	     CompareISOLatin1(str, "spread") == 0 )
    {
	result = XmDISTRIBUTE_SPREAD;
    }
    else
    {
	XtDisplayStringConversionWarning(dpy, from->addr, XmRDistribution);
	return( False );
    }

    done(unsigned char, result);
}

/*
 * Function:
 *	VerifyResources(request, current, new_w)
 * Description:
 *	This function verifies the values for the Column's resource in
 *	"new_w" resetting them to the previous or default values if an
 *	invalid resource value was set.
 * Input:
 *	request	: XmColumnWidget - the requested setting for the widget
 *	current : XmColumnWidget - the current setting for the widget
 *	new_w     : XmColumnWidget - the setting for the widget
 * Output:
 *	None.
 */
static void
VerifyResources(XmColumnWidget request, XmColumnWidget current,
		XmColumnWidget new_w)
{
    Boolean  reset;

    if( BBPart(new_w)->label_font_list == NULL )
    {
	BBPart(new_w)->label_font_list =
	    XmeGetDefaultRenderTable((Widget) new_w, XmLABEL_FONTLIST);
    }

    reset = False;
#if 0	/* POSITION HANDLING */
    switch( XmColumn_default_label_position(new_w) )
    {
    case XiLABEL_POSITION_CENTER:
    case XiLABEL_POSITION_LEFT:
    case XiLABEL_POSITION_RIGHT:
    case XiLABEL_POSITION_TOP:
    case XiLABEL_POSITION_BOTTOM:
	break;
    case XiLABEL_POSITION_UNSPECIFIED:
	XmeWarning((Widget) new_w),
			"The illegal resource value \"XiLABEL_POSITION_UNSPECIFIED\" was assigned to the resource XmNDefaultLabelPosition");
	reset = True;
	break;
    default:
	XmeWarning((Widget) new_w,
			"An illegal resource value was assigned to the resource XmNDefaultLabelPosition");
	reset = True;
	break;
    }

    if( reset )
    {
	XmColumn_default_label_position(new_w) =
	    (current != NULL
	     ? XmColumn_default_label_position(current)
	     : DEFAULT_POSITION);
    }
#endif

    reset = False;
    switch( XmColumn_default_label_alignment(new_w) )
    {
    case XmALIGNMENT_BEGINNING:
    case XmALIGNMENT_CENTER:
    case XmALIGNMENT_END:
	break;
    case XmALIGNMENT_UNSPECIFIED:
	XmeWarning((Widget) new_w,
			"The illegal resource value \"XmALIGNMENT_UNSPECIFIED\" was assigned to the resource XmNdefaultEntryLabelAlignment");
	reset = True;
	break;
    default:
	XmeWarning((Widget) new_w,
			"An illegal resource value was assigned to the resource XmNdefaultEntryLabelAlignment");
	reset = True;
	break;
    }

    if( reset )
    {
	XmColumn_default_label_alignment(new_w) =
	    (current != NULL
	     ? XmColumn_default_label_alignment(current)
	     : DEFAULT_ALIGNMENT);
    }

    reset = False;
    switch( XmColumn_orientation(new_w) )
    {
    case XmHORIZONTAL:
    case XmVERTICAL:
	break;
    default:
	XmeWarning((Widget) new_w,
			"An illegal resource value was assigned to the resource XmNorientation");
	reset = True;
	break;
    }

    if( reset )
    {
	XmColumn_orientation(new_w) =
	    (current != NULL
	     ? XmColumn_orientation(current)
	     : DEFAULT_ORIENTATION);
    }
}

/*
 * Function:
 *	Layout(cw, child, child_size, col_width, col_height)
 * Description:
 *	This is simply a launcher routine.  This routine passes control
 *	to the appropriate layout routine depending on the orientation
 *	of the column.
 * Input:
 *	cw         : XmColumnWidget -   the column to layout
 *	child      : Widget            - A childs whose geometry is specified
 *	child_size : XtWidgetGeometry* - the specified child geometry
 *	col_width  : int               - a specified width for the column
 *	col_height : int               - a specified height for the column
 * Output:
 *	None.
 */
static void
Layout(XmColumnWidget cw, Widget child, XtWidgetGeometry *child_size,
       int col_width, int col_height)
{
    if (XmColumn_orientation(cw) == XmHORIZONTAL)
    {
	HorizontalLayout(cw, child, child_size, col_width, col_height);
    }
    else
    {
	VerticalLayout(cw, child, child_size, col_width, col_height);
    }
}

/*
 * Function:
 *	HorizontalLayout(cw, child, child_size, col_witdh, col_height)
 * Description:
 *	This routine handles the horizontal layout for the column widget
 * Input:
 *	cw         : XmColumnWidget -   the column to layout
 *	child      : Widget            - A childs whose geometry is specified
 *	child_size : XtWidgetGeometry* - the specified child geometry
 *	col_width  : int               - a specified width for the column
 *	col_height : int               - a specified height for the column
 * Output:
 *	None.
 */
static void
HorizontalLayout(XmColumnWidget cw, Widget child,
		 XtWidgetGeometry *child_size, int col_width, int col_height)
{
    /* Do the Horizontal Layout, Baby! */
    WidgetList	kids = cw->composite.children, kid;
    Widget	label;
    Cardinal	i, kidCnt = cw->composite.num_children;
    Position	x, y;
    int		cWidth, cBorder, cbWidth, lWidth, valid, space;
    Dimension	fillHeight;
    int		ispace = (int)XmColumn_item_spacing(cw);
    int 	kidSpace;

    if( col_width < 0 ) col_width = XtWidth(cw);
    if( col_height < 0 ) col_height = XtHeight(cw);

    fillHeight = col_height - 2 * (cw->manager.shadow_thickness +
				   BBPart(cw)->margin_height);


    for( i = 0, kid = kids, lWidth = 0, cbWidth = 0, valid = 0,
	 space = 0, kidSpace = 0; i < kidCnt; ++i, ++kid )
    {
	XiC(*kid)->position.x = XiC(*kid)->position.y = 0;
	if( *kid == child )
	{
	    XiC(*kid)->position.width = child_size->width;
	    XiC(*kid)->position.height = child_size->height;
	}
	else
	{
	    XiC(*kid)->position.width = XiC(*kid)->request_width;
	    XiC(*kid)->position.height = XiC(*kid)->request_height;
	}

	if( !XiValidChild(*kid) ) continue;
	valid++;

	if( *kid == child )
	    cbWidth += child_size->width + 2*child_size->border_width;
	else
	    cbWidth += XiC(*kid)->request_width + 2*XtBorderWidth(*kid);

	if( !XiC(*kid)->show_label ) continue;

	space += XmColumn_label_spacing(cw);
	kidSpace++;

	label = XiC(*kid)->label_widget;
	if( label == child )
		lWidth += child_size->width;
	else
		lWidth += XiC(label)->request_width;
    }

    x = cw->manager.shadow_thickness + BBPart(cw)->margin_width;
    if( valid > 0 && (x + lWidth + space + cbWidth + ispace*(valid-1) + x  > col_width ) )
    {
	/*
	 * First try to shrink labelSpacing, then itemSpacing, otherwise clip.
	 */
	int have = (col_width - 2 * x),
	    want = lWidth + space + ispace*(valid-1) + cbWidth,
	    diff = want - have;

	if ( space - diff > 1)
		{
		space -= diff;
		}
	else
	{
		if (0 != space)
			space = kidSpace;
	    	want = lWidth + space + ispace*(valid-1) + cbWidth;
	    	diff = want - have;

		if (ispace*(valid-1) - diff > 1)
			ispace = (ispace*(valid-1)-diff)/(valid-1);
		else
		{
			if (0 != ispace)
				ispace = 1;
		}
	}
    }
    if (0==kidSpace) kidSpace = 1;
    space /= kidSpace;

    /*
     * Calculating the widths for the various kids is a one shot, so lets
     * first walk through the kids and calculate all the width.
     */
    for( kid = kids, i = 0; i < kidCnt; ++i, ++kid )
    {
	int mySpace;
	if( !XiValidChild(*kid) ) continue;

	label = XiC(*kid)->label_widget;
	XiC(label)->position.width = XtWidth(label);
        if( XiC(*kid)->show_label )
		{
		lWidth = XtWidth(label);
		mySpace = space;
		}
	else
		{
		lWidth = 0;
		mySpace = 0;
		}
	if (LayoutIsRtoLM(cw))
	    XiC(label)->position.x = col_width - x - lWidth;
	else
	    XiC(label)->position.x = x;

	/* First, let's calculate the kid's X-position */
	if( *kid == child )
		cWidth = XiC(*kid)->position.width = child_size->width;
	else
		cWidth = XiC(*kid)->position.width = XiC(*kid)->request_width;
	if (LayoutIsRtoLM(cw))
	    XiC(*kid)->position.x = col_width - (x + lWidth + space) - cWidth;
	else
	    XiC(*kid)->position.x = x + lWidth + space;
	if( child == *kid )
	    cBorder = child_size->border_width;
	else
	    cBorder = XtBorderWidth(*kid);
	x += lWidth + mySpace + cWidth + ispace + 2*cBorder;

	/* Now, let's calculate the kid's Y-position */
	y = cw->manager.shadow_thickness + BBPart(cw)->margin_height;

	XiC(label)->position.y += y;
	XiC(*kid)->position.y += y;

	/* If XmNfillStyle == XmFILL_FLUSH, adjust height to same as column */
	if (XiFill(*kid) == XmFILL_FLUSH)
	{
	    XiC(label)->position.height = fillHeight;
	    XiC(*kid)->position.height = fillHeight;
	}
    }

    if( child == NULL )
    {
	for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
	{
	    if( !XiValidChild(*kid) ) continue;

	    label = XiC(*kid)->label_widget;

	    if( XiC(*kid)->show_label ) {
	      XtConfigureWidget(label, XiC(label)->position.x,
				XiC(label)->position.y,
				XiC(label)->position.width,
				XiC(*kid)->position.height, 0);
	    }

	    XtConfigureWidget(*kid, XiC(*kid)->position.x,
			      XiC(*kid)->position.y,
			      XiC(*kid)->position.width,
			      XiC(*kid)->position.height,
			      XtBorderWidth(*kid));
	}
    }
}

/*
 * Function:
 *	VerticalLayout(cw, child, child_size, col_witdh, col_height)
 * Description:
 *	This routine handles the vertical layout for the column widget
 * Input:
 *	cw         : XmColumnWidget -   the column to layout
 *	child      : Widget            - A childs whose geometry is specified
 *	child_size : XtWidgetGeometry* - the specified child geometry
 *	col_width  : int               - a specified width for the column
 *	col_height : int               - a specified height for the column
 * Output:
 *	None.
 */
static void
VerticalLayout(XmColumnWidget cw, Widget child, XtWidgetGeometry *child_size,
	       int col_width, int col_height)
{
    WidgetList kids = cw->composite.children, kid;
    Widget     label;
    Cardinal   i, kidCnt = cw->composite.num_children, j;
    Position   x, y;
    int        cWidth, cMaxWidth, cMinWidth, cHeight, cBorder, cbWidth, cbHeight,
               lWidth, lHeight, cnt, hExtra, hEach =0 , hLeft = 0, valid, space;
    Dimension  width, height;
    Boolean    change, stretch;

    if( col_width < 0 ) col_width = XtWidth(cw);
    if( col_height < 0 ) col_height = XtHeight(cw);

    CalcSize(cw, NULL, NULL, False, &width, &height);
    for( i = 0, kid = kids, lWidth = 0, cMinWidth = 0, cMaxWidth = 0, cnt = 0, valid = 0,
	 space = 0; i < kidCnt; ++i, ++kid )
    {
	XiC(*kid)->position.x = XiC(*kid)->position.y = 0;
	if( *kid == child )
	{
	    XiC(*kid)->position.width = child_size->width;
	    XiC(*kid)->position.height = child_size->height;
	}
	else
	{
	    XiC(*kid)->position.width = XiC(*kid)->request_width;
	    XiC(*kid)->position.height = XiC(*kid)->request_height;
	}

	if( !XiValidChild(*kid) ) continue;
	valid++;

	if( *kid == child )
	{
	    cbWidth = child_size->width + 2*child_size->border_width;
	}
	else
	{
	    cbWidth = XiC(*kid)->request_width + 2*XtBorderWidth(*kid);
	}
	if( cMinWidth == 0 )
	{
	    cMinWidth = cbWidth;
	}
	else if( cbWidth < cMinWidth )
	{
	    cMinWidth = cbWidth;
	}
	if( cMaxWidth == 0 )
	{
	    cMaxWidth = cbWidth;
	}
	else if( cbWidth > cMaxWidth )
	{
	    cMaxWidth = cbWidth;
	}

	if( XiC(*kid)->stretchable ) ++cnt;

	if( !XiC(*kid)->show_label ) continue;

	space = XmColumn_label_spacing(cw);
	label = XiC(*kid)->label_widget;

	if( child == label )
	{
	    if( (int)child_size->width > lWidth )
	    {
		lWidth = child_size->width;
	    }
	}
	else
	{
	    if( (int)XiC(label)->request_width > lWidth )
	    {
		lWidth = XiC(label)->request_width;
	    }
	}
    }

    x = cw->manager.shadow_thickness + BBPart(cw)->margin_width;
    if( valid > 0 && (x + lWidth + space + cMaxWidth + x > col_width ) )
    {
	int have = (col_width - 2 * x),
	    want = lWidth + space + cMaxWidth,
	    diff = want - have;

	/* Try to subtract from the label space first; if we can't do it (it
	** may be 0 right now),
	** then the data fields will shrink until they are the size of the
	** smallest field, and after that the labels will shrink.
	** The shadow and margin aren't affected.
	*/
	if ( space - diff > 1)
		space -= diff;
	else
	{
		if (0 != space)
			space = 1;
	    	want = lWidth + space + cMinWidth;
	    	diff = want - have;

		if (diff > 0)
		{
			if( lWidth - diff > 1 )
			    lWidth -= diff;
			else
			    lWidth = 1;
		}
	}
    }

    /*
     * Calculating the widths for the various kids is a one shot, so lets
     * first walk through the kids and calculate all the width.
     */
    for( kid = kids, i = 0; i < kidCnt; ++i, ++kid )
    {
	if( !XiValidChild(*kid) ) continue;

	label = XiC(*kid)->label_widget;
	cBorder = XtBorderWidth(*kid);
	if (LayoutIsRtoLM(cw))
	    XiC(label)->position.x = col_width - x - lWidth;
	else
	    XiC(label)->position.x = x;
	XiC(label)->position.width = lWidth;

	/* cbWidth is whatever is left over */
	cbWidth = col_width -
		((int)cw->manager.shadow_thickness +
		    (int)BBPart(cw)->margin_width +
			x + lWidth + space);
	if( cbWidth < 1 ) cbWidth = 1;
	cWidth = cbWidth - 2*cBorder;
	if( cWidth < 1 ) cWidth = 1;
	/* cWidth is now the value to use for XmFILL_FLUSH */
	if( XiFill(*kid) == XmFILL_RAGGED )
	{
	    if( child == *kid )
	    {
		if( (int) child_size->width < cWidth )
		{
		    cWidth = child_size->width;
		}
	    }
	    else
	    {
		if( (int) XiC(*kid)->request_width < cWidth )
		{
		    cWidth = XiC(*kid)->request_width;
		}
	    }
	}

	if (LayoutIsRtoLM(cw))
	    XiC(*kid)->position.x = col_width - (x + lWidth + space + cWidth);
	else
	    XiC(*kid)->position.x = x + lWidth + space;
	XiC(*kid)->position.width = cWidth;
    }

    /*
     * Now that we have calculated the x position and the width of
     * each of the children lets now try to calculate the y position
     * and the height.
     */

    space = XmColumn_item_spacing(cw);
    hExtra = col_height - height;
    /* but first make a quick check on reducing the itemSpacing */
    if (hExtra < 0)
	{
	if (valid)
		{
		int totalItemSpacing = (valid - 1) * space;
		if (totalItemSpacing + hExtra > 0)
			space = (totalItemSpacing + hExtra)/(valid-1);
		else
			space = 1;
		}
	hExtra = 0;	/* or very close to it */
	}
    for( j = 0; j < 2; ++j )
    {
	do {
	    y = cw->manager.shadow_thickness +
		BBPart(cw)->margin_height;

	    if( j != 0 )
	    {
		cnt = valid;
	    }

	    if( cnt > 0 )
	    {
		hEach = hExtra / cnt;
		hLeft = hExtra % cnt;
	    }
	    else if( cnt == 0 )
	    {
		hEach = 0;
		hLeft = hExtra;
	    }
	    change = False;

	    for( kid = kids, i = 0; i < kidCnt; ++i, ++kid )
	    {
		if( !XiValidChild(*kid) ) continue;

		label = XiC(*kid)->label_widget;
		cHeight = XiC(*kid)->position.height;

		if( j == 0 || hExtra > 0 )
		{
		    stretch = XiC(*kid)->stretchable;
		}
		else
		{
		    stretch = True;
		}

		if( stretch )
		{
		    if( hExtra < 0 && cHeight > 1 )
		    {
			if( hEach != 0 )
			{
			    int tmp = -hEach;

			    change = True;
			    if( tmp < cHeight )
			    {
				hExtra += tmp;
				cHeight -= tmp;
			    }
			    else
			    {
				tmp = cHeight - 1;
				hExtra += tmp;
				cHeight = 1;
			    }
			}

			if( hLeft != 0 && cHeight > 1 )
			{
			    change = True;
			    hLeft++;
			    hExtra++;
			    cHeight--;
			}
		    }
		    else if( hExtra > 0 )
		    {
			change = True;
			cHeight += hEach;
			hExtra -= hEach;

			if( hLeft > 0 )
			{
			    cHeight++;
			    hLeft--;
			    hExtra--;
			}
		    }
		}

		if( cHeight < 1 ) cHeight = 1;
		if( child == *kid )
		{
		    cBorder = child_size->border_width;
		}
		else
		{
		    cBorder = XtBorderWidth(*kid);
		}

		if( XiC(*kid)->show_label )
		{
		    if( child == label )
		    {
			lHeight = child_size->height;
		    }
		    else
		    {
			lHeight = XiC(label)->request_height;
		    }
		}
		else
		{
		    lHeight = 1;
		}
		cbHeight = cHeight + 2 * cBorder;

		if( cbHeight > lHeight || stretch )
		{
		    lHeight = cbHeight;
		}
		else
		{
		    cHeight = lHeight - 2*cBorder;
		    if( cHeight < 1 ) cHeight = 1;
		    cbHeight = cHeight + 2*cBorder;
		}

		XiC(label)->position.y = y;
		XiC(label)->position.height = lHeight;

		XiC(*kid)->position.y = y;
		XiC(*kid)->position.height = cHeight;

		if( child == *kid )
		{
		    cBorder = child_size->border_width;
		}
		else
		{
		    cBorder = XtBorderWidth(*kid);
		}
		y += XiC(*kid)->position.height + (2 * cBorder) + space;
	    }
	} while( hExtra != 0 && change );
    }

    /*
     * If we get here and we still have some extra space we want to see
     * if the user wants to distribute the space between children or
     * not.
     */
    if( XmColumn_distribution(cw) == XmDISTRIBUTE_SPREAD &&
        hExtra > 0 )
    {
	if( valid == 1 )
	{
	    hEach = hExtra / 2;
	    XiC(*kids)->position.y += hEach;
	}
	else
	{
	    if( valid > 1 )
	    {
		valid--;
		hEach = hExtra / valid;
		hLeft = hExtra % valid;
	    }
	    else
	    {
		hEach = 0;
		hLeft = hExtra;
	    }

	    y = cw->manager.shadow_thickness +
		BBPart(cw)->margin_height;
	    for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
	    {
		if( !XiValidChild(*kid) ) continue;

		if( i > 0 )
		{
		    XiC(*kid)->position.y = y;
		    XiC(XiC(*kid)->label_widget)->position.y = y;
		}
		space = XmColumn_item_spacing(cw) + hEach;
		if( hLeft > 0 ) {
		    space++;
		    hLeft--;
		}
		if( child == *kid )
		{
		    cBorder = child_size->border_width;
		}
		else
		{
		    cBorder = XtBorderWidth(*kid);
		}
		y += XiC(*kid)->position.height + (2 * cBorder) +
		    space;
	    }
	}
    }

    if( child == NULL )
    {
	for( i = 0, kid = kids; i < kidCnt; ++i, ++kid )
	{
	    if( !XiValidChild(*kid) ) continue;

	    label = XiC(*kid)->label_widget;

	    if( XiC(*kid)->show_label )
	    {
		XtConfigureWidget(label, XiC(label)->position.x,
				  XiC(label)->position.y,
				  XiC(label)->position.width,
				  XiC(*kid)->position.height, 0);
	    }

	    XtConfigureWidget(*kid,  XiC(*kid)->position.x,
			      XiC(*kid)->position.y,
			      XiC(*kid)->position.width,
			      XiC(*kid)->position.height,
			      XtBorderWidth(*kid));
	}
    }
}

/*
 * Function:
 *	VerifyConstraint(request, current, set)
 * Description:
 *	This function verifies the values for the Column child's constraint
 *	resources in "set" resetting them to the previous or default
 *	values if an invalid resource value was set.
 * Input:
 *	request	: Widget - the requested setting for the widget
 *	current : Widget - the current setting for the widget
 *	set     : Widget - the setting for the widget
 * Output:
 *	None.
 */
static void
VerifyConstraints(Widget request, Widget current, Widget set)
{
    Boolean  reset;

#if 0	/* POSITION HANDLING */
    reset = False;
    switch( XiC(set)->label_position )
    {
    case XiLABEL_POSITION_CENTER:
    case XiLABEL_POSITION_LEFT:
    case XiLABEL_POSITION_RIGHT:
    case XiLABEL_POSITION_TOP:
    case XiLABEL_POSITION_BOTTOM:
    case XiLABEL_POSITION_UNSPECIFIED:
	break;
    default:
	XmeWarning(set,
			"An illegal resource value was assigned to the resource XmNentryLabelPosition");
	reset = True;
	break;
    }
    if( reset )
    {
	XiC(set)->label_position = (current != NULL
				   ? XiC(current)->label_position
				   : XiLABEL_POSITION_UNSPECIFIED);
    }
#endif

    reset = False;
    switch( XiC(set)->label_alignment )
    {
    case XmALIGNMENT_BEGINNING:
    case XmALIGNMENT_CENTER:
    case XmALIGNMENT_END:
    case XmALIGNMENT_UNSPECIFIED:
	break;
    default:
	XmeWarning(set,
		"An illegal resource value was assigned to the resource XmNentryLabelAlignment");
	reset = True;
	break;
    }
    if( reset )
    {
	XiC(set)->label_alignment = (current != NULL
				    ? XiC(current)->label_alignment
				    : XmALIGNMENT_UNSPECIFIED);
    }

    reset = False;
    switch( XiC(set)->fill_style )
    {
    case XmFILL_UNSPECIFIED:
    case XmFILL_FLUSH:
    case XmFILL_RAGGED:
	break;
    default:
	XmeWarning(set,
		"An illegal resource value was assigned to the resource XmNfillStyle");
	reset = True;
	break;
    }
    if( reset )
    {
	XiC(set)->fill_style = (current != NULL
			       ? XiC(current)->fill_style
			       : XmFILL_UNSPECIFIED);
    }
}

/*
 * Function:
 *	CalcSize(cw, child, child_size, query, width, height)
 * Description:
 *	This function calculates and returns the prefered size for the
 *	column.
 * Input:
 *	cw         : XmColumnWidget    - the column widget
 *	child      : Widget            - a widget whose geometry is specified
 *	child_size : XtWidgetGeometry* - the specified child's geometry
 *	query      : Boolean           - query the kids?
 *	width      : Dimension*        - return the desired width
 *	height	   : Dimension*	       - return the desired height
 * Output:
 *	None.
 */
static void
CalcSize(XmColumnWidget cw, Widget child, XtWidgetGeometry *child_size,
	 Boolean query, Dimension *width, Dimension *height)
{
  int              _width = 0, _height = 0, cnt = 0;
  Cardinal         i, kidCnt = cw->composite.num_children;
  WidgetList       kid, kids = cw->composite.children;
  Widget           label;
  Dimension        cWidth, cHeight, cBorder, cSum = 0, lWidth, lHeight;
  Dimension        lSum = 0, space = 0, hSumSpace = 0;
  XtWidgetGeometry wants;

  for (i = 0, kid = kids; i < kidCnt; ++i, ++kid)
    {
      if (!XiValidChild(*kid)) continue;

      if (XiC(*kid)->show_label)
	{
	  space = XmColumn_label_spacing(cw);
	  hSumSpace += XmColumn_label_spacing(cw);
	}

      /*
       * Check for the child widgets preferred geometry.
       * if the prefered geometry is greater than the requested
       * geometry, then set the query geometry to "True".
       * Doing this, column widget will take care of improper
       * size settings on the compound children widgets (CR03821)
       */
      query = False;
      XtQueryGeometry(*kid, NULL, &wants);
      if (wants.width > XiC(*kid)->request_width || \
	  wants.height >  XiC(*kid)->request_height)
	{
	  query = True;
	}

      if (*kid == child && child_size != NULL)
	{
	  cWidth = child_size->width;
	  cHeight = child_size->height;
	  cBorder = child_size->border_width;
	} else if (query)
	  {
	    XtQueryGeometry(*kid, NULL, &wants);

	    if (wants.request_mode & CWWidth)
	      {
		cWidth = wants.width;
		XiC(*kid)->request_width = wants.width;
	      }
	    else
	      {
		cWidth = XiC(*kid)->request_width;
	      }

	    if (wants.request_mode & CWHeight)
	      {
		cHeight = wants.height;
		XiC(*kid)->request_height = wants.height;
	      }
	    else
	      {
		cHeight = XiC(*kid)->request_height;
	      }

	    if (wants.request_mode & CWBorderWidth)
	      {
		cBorder = wants.border_width;
	      }
	    else
	      {
		cBorder = XtBorderWidth(*kid);
	      }
	  }
      else
	{
	  cWidth = XiC(*kid)->request_width;
	  cHeight = XiC(*kid)->request_height;
	  cBorder = XtBorderWidth(*kid);
	}

      cWidth += (2 * cBorder);
      cHeight += (2 * cBorder);
      if (XtIsManaged((label = XiC(*kid)->label_widget)))
	{
	  if (label == child && child_size != NULL)
	    {
	      lWidth = child_size->width;
	      lHeight = child_size->height;
	    }
	  else if (query)
	    {
	      XtQueryGeometry(label, NULL, &wants);

	      if (wants.request_mode & CWWidth)
		{
		  lWidth = wants.width;
		}
	      else
		{
		  lWidth = XiC(label)->request_width;
		}

	      if (wants.request_mode & CWHeight)
		{
		  lHeight = wants.height;
		}
	      else
		{
		  lHeight = XiC(label)->request_height;
		}
	    }
	  else
	    {
	      lWidth = XiC(label)->request_width;
	      lHeight = XiC(label)->request_height;
	    }
	}
      else
	{
	  lWidth = lHeight = 0;
	}

      if (XmColumn_orientation(cw) == XmVERTICAL)
	{
	  if (lWidth > lSum) lSum = lWidth;
	  if (cWidth > cSum) cSum = cWidth;

	  _height += (lHeight > cHeight ? lHeight : cHeight);
	}
      else	/* XmHORIZONTAL Layout */
	{
	  /* Choose the maximum height */
	  if (_height < (int)cHeight) _height = cHeight;
	  if (_height < (int)lHeight) _height = lHeight;

	  _width += lWidth + cWidth;
	}

      cnt++;
    }

  if (cnt > 1) --cnt;

  if (XmColumn_orientation(cw) == XmVERTICAL)
    {
      _width = lSum + cSum + space +
	2 * (cw->manager.shadow_thickness +
	     BBPart(cw)->margin_width);

      _height += (cnt * XmColumn_item_spacing(cw) +
		  2 * (cw->manager.shadow_thickness +
		       BBPart(cw)->margin_height));
    }
  else	/* XmHORIZONTAL Layout */
    {
      _width += (hSumSpace +
		 2 * (cw->manager.shadow_thickness +
		      BBPart(cw)->margin_width)) + cnt*XmColumn_item_spacing(cw);

      _height += 2 * (cw->manager.shadow_thickness +
		      BBPart(cw)->margin_height);
    }

  if (_width < 1) _width = 1;
  if (_height < 1) _height = 1;

  if (width != NULL) *width = _width;
  if (height != NULL) *height = _height;
}

/*
 * Function:
 *	CompareGeometry(geom1, geom2)
 * Description:
 *	This function compares to XtWidgetGeometry structure to see if
 *	the are equal.
 * Input:
 *	geom1 : XtWidgetGeometry* - a geometry spec
 *	geom2 : XtWidgetGeometry* - another geometry spec
 * Output:
 *	Boolean - True if the geometry structures are equal, else False
 */
static Boolean
CompareGeometry(XtWidgetGeometry *geom1, XtWidgetGeometry *geom2)
{
    Boolean result;

    result = (geom1 == NULL || geom2 == NULL) ||
	(geom1->request_mode != geom2->request_mode) ||
	(geom1->request_mode & CWX && geom1->x != geom2->x) ||
	(geom1->request_mode & CWY && geom1->y != geom2->y)  ||
	(geom1->request_mode & CWWidth && geom1->width != geom2->width) ||
	(geom1->request_mode & CWHeight && geom1->height != geom2->height) ||
	(geom1->request_mode & CWBorderWidth &&
	 geom1->border_width != geom2->border_width);

    return( !result );
}

/*
 * Function:
 *	CompareGeometryToWidget(geom, widget)
 * Description:
 *	Compares a geometry spec to a widget's actual geometry.
 * Input:
 *	geom   : XtWidgetGeometry* - the geometry
 *	widget : Widget            - the widget
 * Output:
 *	Boolean - True if the widget equals the geom spec, else False.
 */
static Boolean
CompareGeometryToWidget(XtWidgetGeometry *geom, Widget widget)
{
    Boolean result;

    result = (geom == NULL || widget == NULL) ||
	(geom->request_mode == 0) ||
	(geom->request_mode & CWX && geom->x != XtX(widget)) ||
        (geom->request_mode & CWY && geom->y != XtY(widget)) ||
        (geom->request_mode & CWWidth && geom->width != XtWidth(widget)) ||
        (geom->request_mode & CWHeight &&
	 geom->height != XtHeight(widget)) ||
        (geom->request_mode & CWBorderWidth &&
	 geom->border_width != XtBorderWidth(widget));

    return( !result );
}

/*
 * XmRCallProc routine for checking label_font_list before setting it to NULL
 * If constrainit's "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */

static void
CheckSetEntryLabelRenderTable(Widget wid, int offs, XrmValue *value)
{
  XmColumnConstraintPart* cc = XiC(wid);

  /* Check if been here before */
  if (cc->check_set_render_table)
      value->addr = NULL;
  else {
      cc->check_set_render_table = True;
      value->addr = (char*)&(cc->label_font_list);
  }

}

/*
 * XmRCallProc routine for checking label_font_list before setting it to NULL
 * If column's "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */

static void
CheckSetDefaultEntryLabelRenderTable(Widget wid, int offs, XrmValue *value)
{
  XmBulletinBoardPart* bb = BBPart(wid);
  XmColumnWidget c = (XmColumnWidget)wid;

  /* Check if been here before */
  if (c->column.check_set_render_table)
      value->addr = NULL;
  else {
      c->column.check_set_render_table = True;
      value->addr = (char*)&(bb->label_font_list);
  }

}
/*
 * Function:
 *	XmColumnLabelDestroyedCallback(widget, client, cbdata)
 * Description:
 *	This callback is called when a label that is associated with a field
 *	is destoyed.  The purpose of this callback is to inform the column
 *	when this happens to it does not try to do bad things.
 * Input:
 *	widget : Widget    - the widget being destroyed
 *	client : XtPointer - the widget that this label is associated with
 *	cbdata : XtPointer - unused.
 * Output:
 *	None.
 */
static void
XmColumnLabelDestroyedCallback(Widget widget, XtPointer client,
			       XtPointer cbdata)
{
    Widget field = (Widget) client;

    XiC(field)->label_widget = NULL;
}

static void Get_entryLabelString (Widget widget, int offset, XtArgVal *value)
{
    (*value) = (XtArgVal) XmStringCopy(XiC(widget)->label_string);
}

/*
 * Function:
 *	XmCreateColumn(parent, name, arg_list, arg_cnt);
 * Description:
 *	Creates an unmanaged instance of an XmColumn and returns its
 *	widget id.
 * Input:
 *	parent   : Widget   - the parent of the new instance.
 *	name     : String   - the name of the new instance.
 *	arg_list : ArgList  - the arguments to create the instance with.
 *	arg_cnt  : Cardinal - the number of arguments in the list
 *
 * Output:
 *	Widget - the widget id of the new instance.
 */
Widget
XmCreateColumn(Widget parent, String name, ArgList arg_list, Cardinal arg_cnt)
{
    return( XtCreateWidget(name, xmColumnWidgetClass, parent, arg_list,
			   arg_cnt) );
}

Widget
XmVaCreateColumn(
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
                         xmColumnWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedColumn(
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
                         xmColumnWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
