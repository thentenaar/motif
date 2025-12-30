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

#include <stdio.h>

#include "XmI.h"
#include "MessagesI.h"
#include <Xm/MultiListP.h>
#include <Xm/Ext18List.h>
#include <Xm/Ext18ListP.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>

#include <Xm/VaSimpleP.h>
#include <Xm/ExtP.h>

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/
#define BUG_FIX_1385 1

#define H_MARGIN 5
#define V_MARGIN 5

#define FIND_STRING   _XmMMsgResource_0013

extern XmMultiListRowInfo \
           *XmI18ListFindRow(Widget, String, int *, Boolean, Boolean);
extern int *XmI18ListGetSelectedRowArray(XmI18ListWidget, int *);
extern void XmI18ListMakeRowVisible(XmI18ListWidget, int);
extern void XmI18ListDeselectRow(XmI18ListWidget, int);
extern void XmI18ListSelectRow(XmI18ListWidget, int, Boolean);
extern void XmI18ListSelectAllItems(XmI18ListWidget, Boolean);
extern void XmI18ListDeselectItems(XmI18ListWidget, XmString, int);
extern void XmI18ListSelectItems(XmI18ListWidget, XmString, int, Boolean);

static Widget CreateTitle(Widget, XmString, ArgList, Cardinal);
static Widget CreateScrollbar(Widget, Boolean, ArgList, Cardinal);
static Widget CreateFindButton(Widget, ArgList, Cardinal);
static Widget CreateFindText(Widget, ArgList, Cardinal);
static Widget CreateFrame(Widget, ArgList, Cardinal);

static void   ActivateTextSearch(Widget, XtPointer, XtPointer);
static void   LayoutChildren(Widget), ChangeManaged(Widget);
static void   PositionFindAndFindText(Widget);

static void ClassInitialize();
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ClassPartInitialize(WidgetClass w_class);

static void Resize(Widget), Destroy(Widget w);

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void GetValuesHook(Widget, ArgList, Cardinal *);

static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
					XtWidgetGeometry *);

static XtResource resources[] =
{
  {
    XmNwidth, XmCWidth, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(CoreRec, core.width),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNheight, XmCHeight, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(CoreRec, core.height),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNtitle, XmCTitle, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmMultiListRec, ext_list.title),
    XmRImmediate, (XtPointer) NULL
  },
  {
    XmNtitleString, XmCTitleString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmMultiListRec, ext_list.title_string),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNdoubleClickCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmMultiListRec, ext_list.double_click),
    XmRImmediate, (XtPointer) NULL
  },
  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmMultiListRec, ext_list.check_set_select_callback),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNselectCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmMultiListRec, ext_list.single_select),
    XmRImmediate, (XtPointer) NULL
  },

  /* this is the old name for the above */
  {
    XmNsingleSelectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmMultiListRec, ext_list.single_select),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNshowFind, XmCShowFind, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmMultiListRec, ext_list.show_find),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNfindLabel, XmCFindLabel, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmMultiListRec, ext_list.find_label),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNitemNotFoundCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmMultiListRec, ext_list.not_found),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNitemFoundCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmMultiListRec, ext_list.item_found),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNvisibleItemCount, XmCVisibleItemCount, XmRInt,
    sizeof(int), XtOffsetOf(XmMultiListRec, ext_list.visible_rows),
    XmRImmediate, (XtPointer) XmMultiList_DEFAULT_VISIBLE_COUNT
  },
#ifndef BUG_FIX_1385
  {
    XmNnumColumns, XmCNumColumns, XmRShort,
    sizeof(short), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNnumRows, XmCNumRows, XmRShort,
    sizeof(short), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNselectedColumn, XmCSelectedColumn, XmRShort,
    sizeof(short), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNcolumnTitles, XmCColumnTitles, XmRXmStringTable,
    sizeof(XmString *), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNentryData, XmCEntryData, XmRPointer,
    sizeof(XtPointer), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNentryBackground, XmCBackground, XtRPixel,
    sizeof(Pixel), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNfirstColumnPixmaps, XmCFirstColumnPixmaps, XmRBoolean,
    sizeof(Boolean), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNverticalScrollBar, XmCScrollBar, XmRWidget,
    sizeof(Widget), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNhorizontalScrollBar, XmCScrollBar, XmRWidget,
    sizeof(Widget), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNfirstRow, XmCFirstLocation, XmRShort,
    sizeof(short), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNfirstColumn, XmCFirstLocation, XmRShort,
    sizeof(short), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNsortFunctions, XmCFunction, XmRFunction,
    sizeof(Xm18SortFunction **), 0,
    NULL, (XtPointer) NULL
  },

  {
    XmNselectionPolicy, XmCSelectionPolicy, XmRSelectionPolicy,
    sizeof(unsigned char), 0,
    NULL, (XtPointer) NULL
  }
#endif
};

static XmSyntheticResource syn_resources[] =
{
  {
    XmNwidth,
    sizeof (Dimension), XtOffsetOf(CoreRec, core.width),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },

  {
    XmNheight,
    sizeof (Dimension), XtOffsetOf(CoreRec, core.height),
    XmeFromVerticalPixels, XmeToVerticalPixels
  }
};

XmMultiListClassRec xmMultiListClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass)&xmManagerClassRec,
    /* class_name		*/	"XmMultiList",
    /* widget_size		*/	sizeof(XmMultiListRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize */         ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/      NULL,
    /* num_actions		*/      (Cardinal)0,
    /* resources		*/	(XtResource*)resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	GetValuesHook,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	XtInheritTranslations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
   {		/* composite_class fields */
    /* geometry_manager   */      GeometryManager,
    /* change_managed     */      ChangeManaged,
    /* insert_child       */      XtInheritInsertChild,
    /* delete_child       */      XtInheritDeleteChild,
    /* extension          */      NULL,
   },
   {		/* constraint_class fields */
    /* resource list        */         NULL,
    /* num resources        */         0,
    /* constraint size      */         0,
    /* destroy proc         */         NULL,
    /* init proc            */         NULL,
    /* set values proc      */         NULL,
    /* extension            */         NULL,
   },
   {		/* manager_class fields */
    /* default translations   */      XtInheritTranslations,
    /* syn_resources          */      syn_resources,
    /* num_syn_resources      */      XtNumber(syn_resources),
    /* syn_cont_resources     */      NULL,
    /* num_syn_cont_resources */      0,
    /* parent_process         */      NULL,
    /* extension	      */      NULL,
   },
  { /* Extended List fields */
      NULL                      /* extension          */
  }
};

WidgetClass xmMultiListWidgetClass = (WidgetClass)&xmMultiListClassRec;

/*      Function Name: ClassInitialize
 *      Description:   Initializes class-specific data (offsets)
 *      Arguments:     none
 *      Returns:       nothing
 */
static void
ClassInitialize()
{
  /* do nothing */
}

/*	Function Name: Initialize
 *	Description:   Called to initialize information specific
 *                     to this widget.
 *	Arguments:     req - what was originally requested.
 *                     set - what will be created (our superclassed have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */
static void Initialize(Widget req, Widget set,
		       ArgList args, Cardinal * num_args)
{
    ArgList f_args;
    Cardinal f_num_args;
    XmMultiListWidget elist = (XmMultiListWidget) set;

    /*
     * Pass filtered setvalues requests down to all children.
     */

    _XmFilterArgs(args, *num_args, xm_std_filter, &f_args, &f_num_args);

	XmMultiList_last_search(elist) = NULL;
    XmMultiList_ilist(elist) = NULL;

    if (XmMultiList_title(elist) != NULL)
	XmMultiList_title(elist) = XmStringCopy(XmMultiList_title(elist));

    if (XmMultiList_title_string(elist) != NULL)
	XmMultiList_title_string(elist) =
	    XmStringCopy(XmMultiList_title_string(elist));

    if (XmMultiList_find_label(elist) != NULL)
	XmMultiList_find_label(elist) = XmStringCopy(XmMultiList_find_label(elist));
	else
	XmMultiList_find_label(elist) =
		XmStringGenerate(FIND_STRING, NULL, XmCHARSET_TEXT, NULL);

    XmMultiList_title_wid(elist) = CreateTitle(set,
					    (XmMultiList_title_string(elist) ?
					     XmMultiList_title_string(elist) :
					     XmMultiList_title(elist)),
					    f_args, f_num_args);

    XmMultiList_v_bar(elist) = CreateScrollbar(set, True, f_args, f_num_args);
    XmMultiList_h_bar(elist) = CreateScrollbar(set, False, f_args, f_num_args);

    /*
     * Scrollbars must be created before the frame.
     */

    XmMultiList_frame(elist) = CreateFrame(set, f_args, f_num_args);

    XmMultiList_find(elist) = CreateFindButton(set, f_args, f_num_args);
    XmMultiList_find_text(elist) = CreateFindText(set, f_args, f_num_args);

    /* Compute our initial height & width if unset... */
    if (XtHeight(req) == 0)
    {
	int	title_height = 0;

	if (XmMultiList_title(elist) || XmMultiList_title_string(elist))
	    title_height = XmMultiList_title_wid(elist)->core.height + 2 *
		(XmMultiList_title_wid(elist)->core.border_width + V_MARGIN);

	if (XmMultiList_show_find(elist))
	    XtHeight(set) = XmMultiList_h_bar(elist)->core.height +
		XmMultiList_frame(elist)->core.height + title_height +
		XmMultiList_find(elist)->core.height + 2 *
		(V_MARGIN + elist->core.border_width +
		 XmMultiList_h_bar(elist)->core.border_width +
		 XmMultiList_frame(elist)->core.border_width +
		 XmMultiList_find(elist)->core.border_width);
	else
	    XtHeight(set) = XmMultiList_title_wid(elist)->core.height +
		XmMultiList_h_bar(elist)->core.height + title_height +
		XmMultiList_frame(elist)->core.height + 2 *
		(V_MARGIN + elist->core.border_width +
		 XmMultiList_h_bar(elist)->core.border_width +
		 XmMultiList_frame(elist)->core.border_width);
    }

    if (XtWidth(req) == 0)
    {
	XtWidth(set) = XmMultiList_ilist(elist)->core.width +
	    XmMultiList_v_bar(elist)->core.width + (5 * H_MARGIN) +
	    (2 * elist->core.border_width);
    }

    XtFree((XtPointer) f_args);
}


/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void
ClassPartInitialize(WidgetClass w_class)
{
    _XmFastSubclassInit (w_class, XmEXT18LIST_BIT);
}





/*	Function Name: Destroy
 *	Description:   Cleans up after the widget.
 *	Arguments:     w - the widget.
 *	Returns:       none.
 */

static void
Destroy(Widget w)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    XmStringFree(XmMultiList_title(elist));
    XmStringFree(XmMultiList_title_string(elist));
    XmStringFree(XmMultiList_find_label(elist));

    XtRemoveAllCallbacks(w, XmNsingleSelectionCallback);
}

/*	Function Name: Resize
 *	Description:   Called when this widget has been resized.
 *	Arguments:     w - Extended List Widget to realize.
 *	Returns:       none.
 */

static void
Resize(Widget w)
{
    LayoutChildren(w);
}

/*      Function Name: ChangeManaged
 *      Description:   when a management change has occurred.
 *      Arguments:     w - the Ext List widget.
 *      Returns:       none.
 */
static void
ChangeManaged(Widget w)
{
    LayoutChildren(w);
   XmeNavigChangeManaged((Widget)w);   /* for Motif navigation */
}

/*	Function Name: GetValuesHook
 *	Description:   Called to retrieve resource values from either
 *                     the extended list or the interal list
 *      Arguments:     w - the extended list widget.
 *                     args, num_args - the argument list.
 *	Returns:       none
 *
 *      Added:         04/09/92 beth
 */
static void
GetValuesHook(Widget w, ArgList args, Cardinal *num_args)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;
    Cardinal i, num=0, j = *num_args;
    Arg i_args[15];

    /*
     * Get the correct args to pass to the I18List widget
     */
    for (i=0; i < j; i++) {
        if (streq(args[i].name, XmNnumColumns)           ||
            streq(args[i].name, XmNnumRows)              ||
            streq(args[i].name, XmNselectedColumn)       ||
            streq(args[i].name, XmNcolumnTitles)         ||
            streq(args[i].name, XmNentryData)            ||
            streq(args[i].name, XmNentryBackground)      ||
            streq(args[i].name, XmNfirstColumnPixmaps)   ||
            streq(args[i].name, XmNfontList)             ||
            streq(args[i].name, XmNrenderTable)          ||
            streq(args[i].name, XmNverticalScrollBar)    ||
            streq(args[i].name, XmNhorizontalScrollBar)  ||
            streq(args[i].name, XmNfirstRow)             ||
            streq(args[i].name, XmNfirstColumn)          ||
            streq(args[i].name, XmNsortFunctions)        ||
            streq(args[i].name, XmNselectionPolicy))  {

     	    XtSetArg(i_args[num], args[i].name, args[i].value);
            num++;
        }
	else if (streq(args[i].name, XmNtitle))
			*(XmString*)args[i].value = XmStringCopy(XmMultiList_title(elist));
	else if (streq(args[i].name, XmNtitleString))
			*(XmString*)args[i].value = XmStringCopy(XmMultiList_title_string(elist));
	else if (streq(args[i].name, XmNfindLabel))
			*(XmString*)args[i].value = XmStringCopy(XmMultiList_find_label(elist));
    }

    /*
     * Pass argument list through to the IList widget.
     */
    if (num > 0)
        XtGetValues(XmMultiList_ilist(elist), i_args, num);
}

/*	Function Name: SetValues
 *	Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

#define NUM_FIND_WIDGETS 2

static Boolean
SetValues(Widget current, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    XmMultiListWidget e_old = (XmMultiListWidget) current;
    XmMultiListWidget e_set = (XmMultiListWidget) set;
    ArgList f_args;
    Cardinal f_num_args;
    Boolean redisplay = False;

    /*
     * Pass the SetValues request through to all children.
     */

    _XmFilterArgs(args, *num_args, xm_std_filter, &f_args, &f_num_args);
    _XmSetValuesOnChildren((Widget) e_set, f_args, f_num_args);
    XtFree((XtPointer) f_args);

    if (XmMultiList_visible_rows(e_old) != XmMultiList_visible_rows(e_set))
    {
	if (XmMultiList_visible_rows(e_set) < 1)
	    XmMultiList_visible_rows(e_set) = 1;	/* reset values less than 1 */
    }

    if (XmMultiList_show_find(e_old) != XmMultiList_show_find(e_set))
    {
		Widget find_widgets[NUM_FIND_WIDGETS];
		find_widgets[0] = XmMultiList_find(e_set);
		find_widgets[1] = XmMultiList_find_text(e_set);

        if (XmMultiList_show_find(e_set))
            XtManageChildren(find_widgets, NUM_FIND_WIDGETS);
        else
            XtUnmanageChildren(find_widgets, NUM_FIND_WIDGETS);
    }

    /*
     * This check may return, so it must be last.
     */

    if (XmMultiList_find_label(e_old) != XmMultiList_find_label(e_set))
    {
	Arg _args[1];

	redisplay = True;
	XmStringFree(XmMultiList_find_label(e_old));

	if (XmMultiList_find_label(e_set) != NULL)
	    XmMultiList_find_label(e_set) =
		XmStringCopy(XmMultiList_find_label(e_set));
	else
	    XmMultiList_find_label(e_set) =
		XmStringCreateLocalized(FIND_STRING);

	XtSetArg(_args[0], XmNlabelString, XmMultiList_find_label(e_set));
	XtSetValues(XmMultiList_find(e_set), _args, 1);
    }

    /*
     * MultiList supports both XmNtitle (for backwards compatibility) and
     * XmNtitleString resources, so we must check for changes to both here
     */
    if ((XmMultiList_title(e_old) != XmMultiList_title(e_set)) ||
	(XmMultiList_title_string(e_old) != XmMultiList_title_string(e_set)))
    {
	XmString	*title, old_title;

	/* note: fails if both are set at the same time */

	if (XmMultiList_title_string(e_old) != XmMultiList_title_string(e_set))
	{
	    title = &XmMultiList_title_string(e_set);
	    old_title = XmMultiList_title_string(e_old);
	}
	else
	{
	    title = &XmMultiList_title(e_set);
	    old_title = XmMultiList_title(e_old);
	}

	if ((*title == NULL) || (old_title == NULL))
	{
	    LayoutChildren(set);
	}

	if (*title != NULL)
	{
	    Arg		zargs[2];
	    Cardinal	znum_args = 0;

	    *title = XmStringCopy(*title);

	    XtSetArg(zargs[znum_args], XmNlabelString, *title); znum_args++;
	    XtSetValues(XmMultiList_title_wid(e_set), zargs, znum_args);
	}

	XmStringFree(old_title);
    }

    return(redisplay);
}

/*	Function Name: GeometryManager
 *	Description:   handles requests from children for a size change.
 *	Arguments:     child - the child to change.
 *                     request - the geometry that the child wants.
 *                     return - what we will allow if this is an almost.
 *	Returns:       status.
 *
 * This function will allow any width or height request on the label,
 * find button, or frame.  Any other widget will have its request
 * disallowed.
 */

static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry * request,
		XtWidgetGeometry * result)
{
    XmMultiListWidget	elist = (XmMultiListWidget) XtParent(w);
    XtGeometryResult	answer = XtGeometryYes;

    if (!(request->request_mode & (CWWidth | CWHeight | CWBorderWidth)))
	return(XtGeometryNo);

    /*
     * Only these three widgets are ever allowed to initiate a resize
     * request:  Frame (parent of I18List), Title Widget, Find Button
     *
     * The scrollbars should be allowed to be thicker, and the text area
     * should be able to ask to resize the window wider if necessary...
     */
    if ((w != XmMultiList_title_wid(elist)) && (w != XmMultiList_find(elist)) &&
	(w != XmMultiList_frame(elist)))
	return(XtGeometryNo);

    /*
     * Handle the request from the I18List (frame & ilist) child to resize...
     */
    if ((w == XmMultiList_frame(elist)) &&
	((request->request_mode & CWWidth) ||
	 (request->request_mode & CWHeight)))
    {
	Widget			parent = XtParent(XtParent(w));
	XtWidgetGeometry	ext_request, ext_reply;

	ext_request.request_mode = 0;

	if (request->request_mode & CWWidth)
	{
	    ext_request.width = elist->core.width -
		(w->core.width - request->width);
	    ext_request.request_mode |= CWWidth;
	}

	if (request->request_mode & CWHeight)
	{
	    ext_request.height = elist->core.height -
		(w->core.height - request->height);
	    ext_request.request_mode |= CWHeight;
	}

	answer = XtMakeGeometryRequest(parent, &ext_request, &ext_reply);

	if (answer == XtGeometryYes)
	{
	    if (request->request_mode & CWHeight)
		elist->core.height = ext_request.height;
	    if (request->request_mode & CWWidth)
		elist->core.width = ext_request.width;
	}
	else if (answer == XtGeometryAlmost)
	{
	    if (request->request_mode & CWHeight)
		elist->core.height = ext_reply.height;
	    if (request->request_mode & CWWidth)
		elist->core.width = ext_reply.width;
	}
    }

    if (!(request->request_mode & CWWidth))
	request->width = w->core.width;

    if (!(request->request_mode & CWHeight))
	request->height = w->core.height;

    if (!(request->request_mode & CWBorderWidth))
	request->border_width = w->core.border_width;

    *result = *request;

    if (request->request_mode & (CWX | CWY | CWStackMode | CWSibling)) {
	return(XtGeometryAlmost);
    }

    if (request->request_mode & XtCWQueryOnly)
	return(XtGeometryYes);

    /*
     * A real allowed request, set the new dimensions
     */
    w->core.width = result->width;
    w->core.height = result->height;

    LayoutChildren((Widget) elist);

    return(answer);
}

/************************************************************
 *
 * Actions and Callbacks (More entry points into the widget code).
 *
 ************************************************************/

static void
ActivateTextSearch(Widget w, XtPointer elist_ptr, XtPointer client)
{
    XmMultiListWidget elist = (XmMultiListWidget) elist_ptr;
    XmMultiListCallbackStruct find_info;
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *) client;
    Boolean reset = True;
    XmMultiListRowInfo *match;

    char * ptr;
    wchar_t *wc_string;

    if (!XmMultiList_show_find(elist))
        return;

    ptr = XmTextFieldGetString(XmMultiList_find_text(elist));
    wc_string = XmTextFieldGetStringWcs(XmMultiList_find_text(elist));

    if (XmMultiList_last_search(elist))
      reset = !streq(ptr, XmMultiList_last_search(elist));

    /* Last parameter tells search proc to highlight a matching row... */
    match = XmI18ListFindRow(XmMultiList_ilist(elist), ptr,
			     &find_info.column, reset, True);

    find_info.event = cbs->event;
    find_info.string = ptr;
    find_info.wc_string = wc_string;
    find_info.row = match;

    if (!match)
    {
	XBell(XtDisplay(w), 0);

	find_info.reason = XmMULTILIST_NOT_FOUND;

	XtCallCallbackList((Widget) elist, XmMultiList_not_found(elist),
			   (XtPointer) &find_info);
    }
    else
    {
	find_info.reason = XmMULTILIST_FOUND;

	XtCallCallbackList((Widget) elist, XmMultiList_item_found(elist),
			   (XtPointer) &find_info);
    }

    XtFree((XtPointer)wc_string);
    XtFree(XmMultiList_last_search(elist));
    XmMultiList_last_search(elist) = ptr;
}

/************************************************************
*	LOCAL CODE
*************************************************************/

/*	Function Name: LayoutChildren
 *	Description:   Lays out the "child" widgets in the Ext List widget.
 *	Arguments:     w - the ext list widget.
 *	Returns:       none.
 */

static void LayoutChildren(Widget w)
{
    Arg args[1];
    XmMultiListWidget elist = (XmMultiListWidget) w;

    Widget t_wid = XmMultiList_title_wid(elist);
    Widget frame = XmMultiList_frame(elist);
    Widget find_text = XmMultiList_find_text(elist);
    Widget vbar = XmMultiList_v_bar(elist);
    Widget hbar = XmMultiList_h_bar(elist);

    Dimension title_height, h_thick, v_thick, temp_val;
    Position scroll_top, scroll_bottom, scroll_left, scroll_right;

    Dimension height = elist->core.height;
    Dimension width = elist->core.width;

    /*
     * Put Find and Find Text along the bottom
     */
    PositionFindAndFindText(w);

    if ((XmMultiList_title(elist) == NULL) &&
	(XmMultiList_title_string(elist) == NULL))
    {
	scroll_top = V_MARGIN;

	XtSetArg(args[0], XmNmappedWhenManaged, False);
	XtSetValues(t_wid, args, (Cardinal) 1);
    }
    else
    {
	/*
	 * Put the title widget in the top left corner
	 */
	if (LayoutIsRtoLM(w))
	    _XmMoveWidget(t_wid, (Position)(width - H_MARGIN - XtWidth(t_wid)),
	                  (Position) V_MARGIN);
	else
	    _XmMoveWidget(t_wid, (Position) H_MARGIN, (Position) V_MARGIN);

	XtSetArg(args[0], XmNmappedWhenManaged, True);
	XtSetValues(t_wid, args, (Cardinal) 1);

	title_height = t_wid->core.height + 2 * t_wid->core.border_width;

	scroll_top = title_height + 2 * V_MARGIN;
    }

    v_thick = vbar->core.width + 2 * vbar->core.border_width;
    h_thick = hbar->core.height + 2 * vbar->core.border_width;

    if (XmMultiList_show_find(elist))
        scroll_bottom = find_text->core.y - find_text->core.border_width - V_MARGIN;
    else
    	scroll_bottom = height - V_MARGIN;

    scroll_left = H_MARGIN;
    scroll_right = width - H_MARGIN;

    /*
     * Place the frame.
     *
     * NOTE: I am reusing "width" and "height" for the size of the widget
     *       that I am about to configure.
     */

    temp_val = v_thick + scroll_left + H_MARGIN;
    if ((int)temp_val >= (int)scroll_right)
	width = 1;
    else
	width = scroll_right - temp_val;

    temp_val = h_thick + scroll_top + V_MARGIN;
    if ( (Position) temp_val >= scroll_bottom )
	height = 1;
    else
	height = scroll_bottom - temp_val;

    _XmConfigureWidget(frame, scroll_left, scroll_top, width, height,
		       frame->core.border_width);

    /*
     * Place the scrollbars.
     */

    _XmConfigureWidget(hbar, scroll_left, scroll_bottom - (Position) h_thick,
		       width, hbar->core.height, hbar->core.border_width);

    temp_val = h_thick + scroll_top + V_MARGIN;
    if ( (Position) temp_val >= scroll_bottom)
	height = 1;
    else
	height = scroll_bottom - temp_val;

    _XmConfigureWidget(vbar, scroll_right - v_thick, scroll_top,
		       vbar->core.width, height, vbar->core.border_width);
}

/*  Function Name: PositionFindAndFindText
 *  Description:   Positions the find and find text widgets.
 *  Arguments:     parent - the parent of the find and find text- the EList
 *  Returns:       none.
 */

static void PositionFindAndFindText(Widget w)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    Widget find = XmMultiList_find(elist);
    Widget find_text = XmMultiList_find_text(elist);

    Dimension find_height, max_child_height, find_width;
    Dimension height = elist->core.height;
    Dimension width = elist->core.width;
    Position find_x, find_y;

    if (!XmMultiList_show_find(elist))
        return;

    find_height = find->core.height + 2 * find->core.border_width;
    find_width = find->core.width + 2 * find->core.border_width;
    max_child_height = find_text->core.height + 2*find_text->core.border_width;

    if (find_height > max_child_height)
    max_child_height = find_height;

    /*
     * Put the Find widget in the lower left corner.
     */
    if (LayoutIsRtoLM(w))
        find_x = width - H_MARGIN - find_width;
    else
        find_x = H_MARGIN;

    find_y = (Position) height -
         (Position) ((int)(max_child_height + find_height)/2 + V_MARGIN);

    _XmMoveWidget(find, find_x, find_y);

    /*
     * Put the Find Text widget next to the find widget,
     * and resize it to fill the entire bottom width.
     */

    {
    Dimension text_width, temp_val;
    Position x, y;


    if (LayoutIsRtoLM(w))
	x = H_MARGIN;
    else
	x = (Dimension) find_x + find_width + H_MARGIN;

    y = find_y - (int)(max_child_height - find_height)/2;

    temp_val = x + H_MARGIN;
    if (temp_val >= width)
        text_width = 1;
    else
	if (LayoutIsRtoLM(w))
            text_width = width - find_width - temp_val;
	else
            text_width = width - temp_val;

    _XmConfigureWidget(find_text, x, y, text_width, find_text->core.height,
               find_text->core.border_width);
    }
}
/************************************************************
 *
 * Functions that Create Widgets.
 *
 ************************************************************/

/*	Function Name: CreateTitle
 *	Description:   Creates the Title widget.
 *	Arguments:     parent - parent of the title widget.
 *                     label - The label string.
 *                     args, num_args - arguments used to create ext list.
 *	Returns:       the title widget.
 */

static Widget
CreateTitle(Widget parent, XmString label, ArgList args, Cardinal num_args)
{
    Widget w;
    Arg *new_args, largs[5];
    Cardinal num_largs;

    num_largs = 0;
    if (label != NULL) {
	XtSetArg(largs[num_largs], XmNlabelString, label); num_largs++;
    }
    new_args = XtMergeArgLists(args, num_args, largs, num_largs);

    w = XtCreateManagedWidget("title", xmLabelWidgetClass, parent,
			      new_args, num_largs + num_args);

    XtFree((XtPointer) new_args);
    return(w);
}

/*	Function Name: CreateScrollbar
 *	Description:   Creates a vert or horizontal scrollbar.
 *	Arguments:     parent - parent of the scrollbar widget.
 *                     is_vert - if true then create a vertical scrollbar,
 *                               else, create a horiz one.
 *                     args, num_args - arguments used to create ext list.
 *	Returns:       none
 */

static Widget
CreateScrollbar(Widget parent, Boolean is_vert,ArgList args, Cardinal num_args)
{
    Widget w;
    Arg *new_args, largs[5];
    Cardinal num_largs = 0;
    String name;

    if (is_vert) {
	XtSetArg(largs[num_largs], XmNorientation, XmVERTICAL);
	name = "vertBar";
    }
    else {
	XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL);
	name = "horizBar";
    }
    num_largs++;
    new_args = XtMergeArgLists(args, num_args, largs, num_largs);

    w = XtCreateManagedWidget(name, xmScrollBarWidgetClass, parent,
			      new_args, num_args + num_largs);

    XtFree((XtPointer) new_args);

    return(w);
}

static Widget
CreateFindButton(Widget parent, ArgList args, Cardinal num_args)
{
    Widget w;
    Arg *new_args, largs[5];
    Cardinal num_largs = 0;
    XmMultiListWidget elist = (XmMultiListWidget) parent;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, XmMultiList_find_label(elist));
                  num_largs++;
    new_args = XtMergeArgLists(args, num_args, largs, num_largs);

    w = XtCreateWidget("find", xmPushButtonWidgetClass, parent,
                  new_args, num_largs + num_args);
    XtAddCallback(w, XmNactivateCallback, ActivateTextSearch, parent);

    if (XmMultiList_show_find(elist))
        XtManageChild(w);

    XtFree((XtPointer) new_args);

    return(w);
}

/*  Function Name: CreateFindText
 *  Description:   Creates the find text.
 *  Arguments:     parent - parent of the find text widget.
 *                     args, num_args - arguments used to create ext list.
 *  Returns:       the find text widget.
 */

static Widget
CreateFindText(Widget parent, ArgList args, Cardinal num_args)
{
    Widget w;
    XmMultiListWidget elist = (XmMultiListWidget) parent;

    w = XtCreateWidget("findText", xmTextFieldWidgetClass,
                  parent, args, num_args);
    XtAddCallback(w, XmNactivateCallback, ActivateTextSearch, parent);

    if (XmMultiList_show_find(elist))
        XtManageChild(w);

    return(w);
}


/*	Function Name: CreateFrame
 *	Description:   Creates the frame to contain the list region
 *	Arguments:     parent - parent of the frame.
 *                     args, num_args - arguments used to create ext list.
 *	Returns:       none
 */

static Widget
CreateFrame(Widget parent, ArgList args, Cardinal num_args)
{
    XmMultiListWidget elist = (XmMultiListWidget) parent;
    Widget w;
    Arg largs[2], * new_args;
    Cardinal num_largs;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNshadowType, XmSHADOW_IN); num_largs++;
    new_args = XtMergeArgLists(args, num_args, largs, num_largs);

    w = XtCreateWidget("frame", xmFrameWidgetClass, parent,
		       new_args, num_args + num_largs);

    XtFree((XtPointer) new_args);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNverticalScrollBar, XmMultiList_v_bar(elist));
    num_largs++;
    XtSetArg(largs[num_largs], XmNhorizontalScrollBar, XmMultiList_h_bar(elist));
    num_largs++;

    new_args = XtMergeArgLists(args, num_args, largs, num_largs);

    XmMultiList_ilist(elist) = XtCreateManagedWidget("list", xmI18ListWidgetClass,
						  w, new_args,
						  num_args + num_largs);

    XtFree((XtPointer) new_args);

    /* PWC - Note...
     *
     * Required to force geometry of frame here (it won't inherit child's size)
     */
    w->core.width = XmMultiList_ilist(elist)->core.width +
	2 * w->core.border_width;
    w->core.height = XmMultiList_ilist(elist)->core.height +
	2 * w->core.border_width;

    XtManageChild(w);

    return(w);
}

/************************************************************
 *
 * Public Functions.
 *
 ************************************************************/

/* -kat 1/18/91
 * Function Name: XmMultiListGetSelectedRows
 * Description:   Takes an Extended List and returns a NULL terminated array
 *                of pointers to selected rows from the internal list
 * Arguments:     w - the extended list widget
 * Returns:       XmMultiListRowInfo **
 */

XmMultiListRowInfo **
XmMultiListGetSelectedRows(Widget w)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;
    XmMultiListRowInfo **ret_rows = NULL;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    ret_rows = XmI18ListGetSelectedRows(XmMultiList_ilist(elist));

    _XmAppUnlock(app);
    return ret_rows;
}

/*	Function Name: XmCreateMultiList
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateMultiList(Widget parent, String name,
		     ArgList args, Cardinal num_args)
{
    return (XtCreateWidget(name, xmMultiListWidgetClass,
			   parent, args, num_args));
}

Widget
XmVaCreateMultiList(
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
                         xmMultiListWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedMultiList(
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
                         xmMultiListWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/*  Function Name: XmMultiListUnselectAllItems
 *  Description:   Unselects all rows
 *  Arguments:     w - the ilist widget.
 *  Returns:       none
 */
void
XmMultiListUnselectAllItems(Widget w)
{
  XmMultiListWidget elist = (XmMultiListWidget)w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

  Xm18IListUnselectAllItems( XmMultiList_ilist(elist) );

  _XmAppUnlock(app);
}


/*  Function Name: XmMultiListUnselectItem
 *  Description:   Unselects the row passed in
 *  Arguments:     w - the ilist widget.
 *                 row_info - ptr to the row passed in
 *  Returns:       none
 */
void
XmMultiListUnselectItem(Widget w, XmMultiListRowInfo *row_info)
{
  XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

  Xm18IListUnselectItem( XmMultiList_ilist(elist), row_info );

    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListToggleRow
 *  Description:   Toggles the selection state of a specified row
 *  Arguments:     w - the extended list widget
 *  Returns:       none
 */
void
XmMultiListToggleRow(Widget w, short row)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListToggleRow(XmMultiList_ilist(elist), row);

    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListSelectItems
 *  Description:   Set selection state by matching column entries to XmString
 *  Arguments:     w - the extended list widget
 *		   item - XmString to use as selection key
 *		   notify - if True, call XmNsingleSelectionCallback
 *		   column - column number (0 - N) to match (or XmANY_COLUMN)
 *  Returns:       none
 */
void XmMultiListSelectItems(Widget w, XmString item,
			    int column, Boolean notify)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListSelectItems((XmI18ListWidget)XmMultiList_ilist(elist),
			 item, column, notify);

    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListDeselectItems
 *  Description:   Set selection state by matching column entries to XmString
 *  Arguments:     w - the extended list widget
 *		   item - XmString to use as selection key
 *		   column - column number (0 - N) to match (or XmANY_COLUMN)
 *  Returns:       none
 */
void
XmMultiListDeselectItems(Widget w, XmString item, int column)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListDeselectItems((XmI18ListWidget)XmMultiList_ilist(elist),
			   item, column);

    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListSelectAllItems
 *  Description:   Set selection state of all rows
 *  Arguments:     w - the extended list widget
 *		   notify - if True, call XmNsingleSelectionCallback
 *  Returns:       none
 */
void XmMultiListSelectAllItems(Widget w, Boolean notify)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListSelectAllItems((XmI18ListWidget)XmMultiList_ilist(elist), notify);
    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListSelectRow
 *  Description:   Set selection state on all rows
 *  Arguments:     w - the extended list widget
 *                 row - the row to select
 *		   notify - if True, call XmNsingleSelectionCallback
 *  Returns:       none
 */
void
XmMultiListSelectRow(Widget w, int row, Boolean notify)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListSelectRow((XmI18ListWidget)XmMultiList_ilist(elist), row, notify);
    _XmAppUnlock(app);
}

/*  Function Name: XmMultiListDeselectRow
 *  Description:   Set selection state on all rows
 *  Arguments:     w - the extended list widget
 *                 row - the row to select
 *  Returns:       none
 */
void
XmMultiListDeselectRow(Widget w, int row)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListDeselectRow((XmI18ListWidget)XmMultiList_ilist(elist), row);
    _XmAppUnlock(app);
}


/*  Function Name: XmMultiGetSelectedRowArray
 *  Description:   Creates an array of integers which are selected row #'s
 *  Arguments:     w - the extended list widget
 *		   rows - pointer to the array of selected row #'s
 *		   num_rows - pointer to the number of selected rows
 *  Returns:       selected rows array
 */
int *
XmMultiListGetSelectedRowArray(Widget w, int *num_rows)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;
    int *ret_val;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    ret_val = XmI18ListGetSelectedRowArray(
            (XmI18ListWidget)XmMultiList_ilist(elist),
					num_rows);
    _XmAppUnlock(app);
    return ret_val;
}

/*  Function Name: XmMultiListMakeRowVisible
 *  Description:   Shifts the visible extended list rows as desired
 *  Arguments:     w - the extended list widget
 *		   row - the row number wished to be made visible
 *  Returns:       none
 */
void
XmMultiListMakeRowVisible(Widget w, int row)
{
    XmMultiListWidget elist = (XmMultiListWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    XmI18ListMakeRowVisible((XmI18ListWidget)XmMultiList_ilist(elist), row);

    _XmAppUnlock(app);
}

/* aliases of deprecated functiosn of XmExt18List */
extern WidgetClass xmExt18ListWidgetClass XM_ALIAS(xmMultiListWidgetClass);
XmMultiListRowInfo ** XmExt18ListGetSelectedRows(Widget) XM_ALIAS(XmMultiListGetSelectedRows);
Widget XmCreateExtended18List(Widget, String, ArgList, Cardinal) XM_ALIAS(XmCreateMultiList);
Widget XmCreateExt18List(Widget, String, ArgList, Cardinal) XM_ALIAS(XmCreateMultiList);
Widget XmVaCreateExt18List(Widget, char *, ...) XM_ALIAS(XmVaCreateMultiList);
Widget XmVaCreateManagedExt18List(Widget, char *, ...) XM_ALIAS(XmVaCreateManagedMultiList);
void XmExt18ListUnselectAllItems(Widget) XM_ALIAS(XmMultiListUnselectAllItems);
void XmExt18ListUnselectItem(Widget, XmMultiListRowInfo*) XM_ALIAS(XmMultiListUnselectItem);
void XmExt18ListToggleRow(Widget, short) XM_ALIAS(XmMultiListToggleRow);
void XmExt18ListSelectItems(Widget, XmString, int, Boolean) XM_ALIAS(XmMultiListSelectItems);
void XmExt18ListDeselectItems(Widget, XmString, int) XM_ALIAS(XmMultiListDeselectItems);
void XmExt18ListSelectAllItems(Widget, Boolean) XM_ALIAS(XmMultiListSelectAllItems);
void XmExt18ListSelectRow(Widget, int, Boolean) XM_ALIAS(XmMultiListSelectRow);
void XmExt18ListDeselectRow(Widget, int) XM_ALIAS(XmMultiListDeselectRow);
int *XmExt18ListGetSelectedRowArray(Widget, int *) XM_ALIAS(XmMultiListGetSelectedRowArray);
void XmExt18ListMakeRowVisible(Widget, int) XM_ALIAS(XmMultiListMakeRowVisible);
extern XmMultiListClassRec xmExt18ListClassRec XM_ALIAS(xmMultiListClassRec);

