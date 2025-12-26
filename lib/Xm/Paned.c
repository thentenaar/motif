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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <X11/cursorfont.h>

#include "XmI.h"
#include <Xm/PanedP.h>
#include <Xm/SashP.h>
#include <Xm/Separator.h>
#include <Xm/Screen.h>

#include <Xm/ExtP.h>

typedef enum {UpLeftPane = 'U',	/* Adjust panes above or to the left.*/
	      LowRightPane = 'L',/* Adjust panes below or the the right. */
	      AnyPane = 'A' } Direction;

#define NO_DELTA -99
#define NO_INDEX -100
#define BLOCK 10
#define LARGE_INC ("LargeIncr")

#define XmNisAPane "isAPane"

/************************************************************
 *       GLOBAL DECLARATIONS
 ************************************************************/

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

/*
 * These characters all get compiled into each application as/is, so
 * the default translations table is not very readable, but is nice and small.
 */

static char defSashTranslations[] =
    "<Key>osfHelp: Help()\n\
     !c<Key>osfUp: SashAction(K,10,Up)\n\
     None<Key>osfUp: SashAction(K,1,Up)\n\
     !c<Key>osfDown: SashAction(K,10,Down)\n\
     None<Key>osfDown: SashAction(K,1,Down)\n\
     !c<Key>osfLeft: SashAction(K,10,Left)\n\
     None<Key>osfLeft: SashAction(K,1,Left)\n\
     !c<Key>osfRight: SashAction(K,10,Right)\n\
     None<Key>osfRight: SashAction(K,1,Right)\n\
     s~m~a<Key>Tab: PrevTabGroup()\n\
     ~m~a<Key>Tab: NextTabGroup()\n\
     ~c ~s ~m ~a <Btn1Down>: SashAction(S)\n\
     ~c ~s ~m ~a <Btn1Motion>: SashAction(M)\n\
     ~c ~s ~m ~a <BtnUp>: SashAction(C)\n\
     <FocusIn>: SashFocusIn()\n\
     <FocusOut>: SashFocusOut()\n\
     <Unmap>: PrimitiveUnmap()\n\
     <EnterWindow>: enter()\n\
     <LeaveWindow>: leave()";

/* Internal (yet useful) Motif routine. */
extern void _XmBackgroundColorDefault();

/************************************************************
 *     Private functions.
 ************************************************************/

static void ClassInitialize(void);

static XmImportOperator ToPanedOppositePixels(Widget, int, XtArgVal *);
static XmImportOperator ToPanedChildPixels(Widget, int, XtArgVal *);

static Dimension PaneSize(Widget, Boolean);
static Dimension GetRequestInfo(XtWidgetGeometry *, Boolean);

static Pane ChoosePaneToResize(XmPanedWidget, int, Direction, Boolean);

static int GetEventLocation(XmPanedWidget, XEvent *);

static Boolean PopPaneStack(XmPanedWidget), IsPane(Widget);
static Boolean SatisfiesRule3(Pane, Boolean);
static Boolean SatisfiesRule2(Pane), SatisfiesRule1(Pane, Boolean, Direction);
static Boolean RefigureLocations(XmPanedWidget, int, Direction);

static XtGeometryResult AdjustPanedSize(XmPanedWidget, Dimension, Boolean,
					Boolean, Dimension *, Dimension *);

static void ResetSize(XmPanedWidget, Boolean);
static void GetPrefSizes(XmPanedWidget, Dimension *, Dimension *);
static void FromPanedChildPixels(Widget, int, XtArgVal *);
static void FromPanedOppositePixels(Widget, int, XtArgVal *);
static void LoopAndRefigureChildren(XmPanedWidget, int, int, Dimension *);
static void CommitNewLocations(XmPanedWidget, Widget);
static void RefigureLocationsAndCommit(Widget), CreateSeparator(Widget);
static void _DrawRect(XmPanedWidget, GC, int, int, unsigned int, unsigned int);
static void _DrawTrackLines(XmPanedWidget, Boolean), ResetSash(Widget);
static void StartSashAdjustment(XmPanedWidget, Widget);
static void MoveSashAdjustment(XmPanedWidget, Widget, int);
static void CommitSashAdjustment(XmPanedWidget, Widget), ReleaseGCs(Widget);
static void ProcessKeyEvent(XtPointer, XtIntervalId *);
static void HandleSash(Widget, XtPointer, XtPointer);
static void ManageAndUnmanageSashs(XmPanedWidget), CreateSash(Widget);
static void SetChildrenPrefSizes(XmPanedWidget, Dimension, Boolean, Boolean);
static void PushPaneStack(XmPanedWidget, Pane), ClearPaneStack(XmPanedWidget);
static void GetPaneStack(XmPanedWidget, Boolean, Pane *, int *);

/************************************************************
 *  Semi-Public functions
 ************************************************************/

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static Boolean PaneSetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static XtGeometryResult GeometryManager(Widget,	XtWidgetGeometry *,
					XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
				      XtWidgetGeometry *);

static void Destroy(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal*);
static void InsertChild(Widget);
static void Realize(Widget, Mask *, XSetWindowAttributes *);
static void ConstraintDestroy(Widget), ReManageChildren(XmPanedWidget);
static void ChangeManaged(Widget), Resize(Widget);

/*  Resource definitions for Subclasses of Primitive */

static Position def_pos = -10;	/* negative numbers cannot be set with immed.*/

static XtResource resources[] =
{
  {
    XmNspacing, XmCSpacing, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.internal_bw),
    XmRImmediate, (XtPointer) 10
  },

  {
    XmNmarginWidth, XmCMargin, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.margin_width),
    XmRImmediate, (XtPointer) 3
  },

  {
    XmNmarginHeight, XmCMargin, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.margin_height),
    XmRImmediate, (XtPointer) 3
  },

  {
    XmNrefigureMode, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedRec, paned.refiguremode),
    XmRImmediate, (XtPointer) TRUE
  },

  {
    XmNorientation, XmCOrientation, XmROrientation,
    sizeof(unsigned char), XtOffsetOf(XmPanedRec, paned.orientation),
    XmRImmediate, (XtPointer) XmVERTICAL
  },

  {
    XmNseparatorOn, XmCSeparatorOn, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedRec, paned.separator_on),
    XmRImmediate, (XtPointer) TRUE
  },

    /* Cursors - these are used in both horiz and vertical mode. */

  {
    XmNcursor, XmCCursor, XmRCursor,
    sizeof(Cursor), XtOffsetOf(XmPanedRec, paned.cursor),
    XmRImmediate, None
  },

    /* Sash control resources */

  {
    XmNsashIndent, XmCSashIndent, XmRHorizontalPosition,
    sizeof(Position), XtOffsetOf(XmPanedRec, paned.sash_indent),
    XmRHorizontalPosition, (XtPointer) &def_pos
  },

  {
    XmNsashTranslations, XmCTranslations, XmRTranslationTable,
    sizeof(XtTranslations), XtOffsetOf(XmPanedRec, paned.sash_translations),
    XmRString, (XtPointer)defSashTranslations
  },

  {
    XmNsashWidth, XmCSashWidth, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.sash_width),
    XmRImmediate, (XtPointer)0
  },

  {
    XmNsashHeight, XmCSashHeight, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.sash_height),
    XmRImmediate, (XtPointer)0
  },

  {
    XmNsashShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.sash_shadow_thickness),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNallowUnusedSpace, XmCAllowUnusedSpace, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedRec, paned.allow_unused_space),
    XmRImmediate, (XtPointer) TRUE
  }
};

/* Definition for resources that need special processing in get values */

static XmSyntheticResource get_resources[] =
{
  {
    XmNmarginWidth, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.margin_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNmarginHeight, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.margin_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNspacing, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.internal_bw),
    _XmFromPanedPixels, (XmImportProc) _XmToPanedPixels
  },

  {
    XmNsashIndent, sizeof(Position),
    XtOffsetOf(XmPanedRec, paned.sash_indent),
    FromPanedOppositePixels, (XmImportProc) ToPanedOppositePixels
  },

  {
    XmNsashWidth, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.sash_width),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  },

  {
    XmNsashHeight, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.sash_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  },

  {
    XmNsashShadowThickness, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.sash_shadow_thickness),
    XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
  }
};

static XtResource subresources[] =
{
  {
    XmNallowResize, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.allow_resize),
    XmRImmediate, (XtPointer) FALSE
  },

  {
    XmNpaneMinimum, XmCPaneMinimum, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedConstraintsRec, paned.min),
    XmRImmediate, (XtPointer) 1
  },

  {
    XmNpaneMaximum, XmCPaneMaximum, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedConstraintsRec, paned.max),
    XmRImmediate, (XtPointer) 1000
  },
#ifdef POSITION_IMPLEMENTED
  {
    XmNposition, XmCPosition, XmRInt,
    sizeof(int), XtOffsetOf(XmPanedConstraintsRec, paned.position),
    XmRImmediate, (XtPointer) 0
  },
#endif
  {
    XmNpreferredPaneSize, XmCPreferredPaneSize, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedConstraintsRec, paned.preferred_size),
    XmRImmediate, (XtPointer) XmPanedAskChild
  },

  /*
   * Overrides user's grip placement and go back to preferred size when
   * paned window is resized or a management change happens.
   */

  {
    XmNresizeToPreferred, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.resize_to_pref),
    XmRImmediate, (XtPointer) FALSE
  },

  {
    XmNskipAdjust, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.skip_adjust),
    XmRImmediate, (XtPointer) FALSE
  },

  {
    XmNshowSash, XmCShowSash, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.show_sash),
    XmRImmediate, (XtPointer) TRUE
  },

    /*
     * This is an internal constraint resources set on sashes and separators
     * at start up to False, it should be True for all real panes.
     */

  {
    XmNisAPane, XmNisAPane, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.is_a_pane),
    XmRImmediate, (XtPointer) TRUE
  }
};

/* Definition for constraint resources that need special */
/* processing in get values                              */

static XmSyntheticResource get_constraint_resources[] =
{
  {
    XmNpaneMinimum, sizeof(Dimension),
    XtOffsetOf(XmPanedConstraintsRec, paned.min),
    FromPanedChildPixels, (XmImportProc) ToPanedChildPixels
  },

  {
    XmNpaneMaximum, sizeof(Dimension),
    XtOffsetOf(XmPanedConstraintsRec, paned.max),
    FromPanedChildPixels, (XmImportProc) ToPanedChildPixels
  },

  {
    XmNpreferredPaneSize, sizeof(Dimension),
    XtOffsetOf(XmPanedConstraintsRec, paned.preferred_size),
    FromPanedChildPixels, (XmImportProc) ToPanedChildPixels
  }
};

#define SuperClass ((ConstraintWidgetClass)&xmManagerClassRec)

XmPanedClassRec xmPanedClassRec = {
   {
/* core class fields */
    /* superclass         */   (WidgetClass) SuperClass,
    /* class name         */   "XmPaned",
    /* size               */   sizeof(XmPanedRec),
    /* class_initialize   */   ClassInitialize,
    /* class_part init    */   NULL,
    /* class_inited       */   FALSE,
    /* initialize         */   Initialize,
    /* initialize_hook    */   NULL,
    /* realize            */   Realize,
    /* actions            */   NULL,
    /* num_actions        */   0,
    /* resources          */   (XtResource*)resources,
    /* resource_count     */   XtNumber(resources),
    /* xrm_class          */   NULLQUARK,
    /* compress_motion    */   TRUE,
    /* compress_exposure  */   TRUE,
    /* compress_enterleave*/   TRUE,
    /* visible_interest   */   FALSE,
    /* destroy            */   Destroy,
    /* resize             */   Resize,
    /* expose             */   NULL,
    /* set_values         */   SetValues,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   XtInheritSetValuesAlmost,
    /* get_values_hook    */   NULL,
    /* accept_focus       */   NULL,
    /* version            */   XtVersion,
    /* callback_private   */   NULL,
    /* tm_table           */   XtInheritTranslations,
    /* query_geometry	  */   (XtGeometryHandler) QueryGeometry,
    /* display_accelerator*/   XtInheritDisplayAccelerator,
    /* extension          */   NULL
   }, {
/* composite class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   InsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
   }, {
/* constraint class fields */
    /* subresources       */   (XtResource*)subresources,
    /* subresource_count  */   XtNumber(subresources),
    /* constraint_size    */   sizeof(XmPanedConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   ConstraintDestroy,
    /* set_values         */   PaneSetValues,
    /* extension          */   NULL
   }, {
/* manager_class fields   */
      XtInheritTranslations,                    /* translations           */
      get_resources,                            /* get resources          */
      XtNumber(get_resources),                  /* num get_resources      */
      get_constraint_resources,                 /* get_cont_resources     */
      XtNumber(get_constraint_resources),       /* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,                                     /* extension              */
  }, {
/* paned_window_class fields */
      NULL,                                     /* extension                 */
  }
};

WidgetClass xmPanedWidgetClass = (WidgetClass) &xmPanedClassRec;

/***********************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*
 *     Function Name: ClassInitialize
 *     Description: Initialized XmPaned widget class
 *     Arguments: none
 *     Returns: nothing
 */
static void
ClassInitialize(void)
{
  /* do nothing */
}

/*	Function Name: AdjustPanedSize
 *	Description: Adjusts the size of the pane.
 *	Arguments: pw - the paned widget to adjust.
 *                 off_size - the new off_size to use.
 *                 always_compute_on_size - a self explanatory Boolean.
 *                 query_only - perform a query_only?
 *                 result_ret - result of query ** RETURNED **
 *                 on_size_ret - the new on_size ** RETURNED **
 *                 off_size_ret - the new off_size ** RETURNED **
 *	Returns: the amount of change in size.
 */

static XtGeometryResult
AdjustPanedSize(XmPanedWidget pw, Dimension off_size,
		Boolean always_compute_on_size, Boolean query_only,
		Dimension *on_size_ret, Dimension *off_size_ret)
{
    Dimension old_size = PaneSize( (Widget) pw, IsVert(pw));
    Dimension newsize = 0;
    XtWidgetGeometry request, reply;
    XtGeometryResult result;

    if ((PaneSize((Widget) pw, IsVert(pw)) <= 1) || always_compute_on_size) {
	GetPrefSizes(pw, &newsize, NULL);
	if (newsize < 1)
	    newsize = 1;
    }
    else {
	if (IsVert(pw))
	    newsize = pw->core.height;
	else
	    newsize = pw->core.width;
    }

    if ( IsVert(pw) ) {
        request.width = off_size + 2 * XmPaned_margin_width(pw);
	request.height = newsize;
    }
    else {
        request.width = newsize;
	request.height = off_size + 2 * XmPaned_margin_height(pw);
    }

    request.request_mode = CWWidth | CWHeight;
    if (query_only)
	request.request_mode |= XtCWQueryOnly;

    result = XtMakeGeometryRequest( (Widget) pw, &request, &reply );
    if (result == XtGeometryAlmost)
    {
	if (query_only)
	    request = reply;
	else
	    result = XtMakeGeometryRequest((Widget) pw, &reply, &request);
    }

    reply = request;	/* simplifies things later... */

    if ( result == XtGeometryNo ) {
	if (on_size_ret != NULL)
	    *on_size_ret = old_size;
	if (off_size_ret != NULL)
	    *off_size_ret = PaneSize((Widget) pw, !IsVert(pw));
    }
    else {	/* result == XtGeometryYes */
	if (on_size_ret != NULL)
	    *on_size_ret = GetRequestInfo( &reply, IsVert(pw) );
	if (off_size_ret != NULL)
	    *off_size_ret = GetRequestInfo( &reply, !IsVert(pw) );
    }

    if ( IsVert(pw) ) {
	if (on_size_ret != NULL)
	    *on_size_ret -= 2 * XmPaned_margin_height(pw);
	if (off_size_ret != NULL)
	    *off_size_ret -= 2 * XmPaned_margin_width(pw);
    }
    else {
	if (on_size_ret != NULL)
	    *on_size_ret -= 2 * XmPaned_margin_width(pw);
	if (off_size_ret != NULL)
	    *off_size_ret -= 2 * XmPaned_margin_height(pw);
    }

    return(result);
}

/************************************************************
 *
 *  Resolution Independance stuff for dealing in the face
 *  of orientation changes.
 *
 ************************************************************/

/*	Function Name: FromPanedPixels
 *	Description: Converts from pixels to current unit type
 *                   does either horiz or vert depending on orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

void
_XmFromPanedPixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) widget;

    if (IsVert(pw))
	XmeFromVerticalPixels(widget, offset, value);
    else
	XmeFromHorizontalPixels(widget, offset, value);
}

/*	Function Name: FromPanedChildPixels
 *	Description: Converts from pixels to current unit type
 *                   does either horiz or vert depending on
 *                   of the parent of this widget orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

static void
FromPanedChildPixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) XtParent(widget);

    if (IsVert(pw))
	XmeFromVerticalPixels(widget, offset, value);
    else
	XmeFromHorizontalPixels(widget, offset, value);
}

/*	Function Name: ToPanedPixels
 *	Description: Converts to pixels from current unit type
 *                   does either horiz or vert depending on orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

XmImportOperator
_XmToPanedPixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) widget;

    if (IsVert(pw))
	return(XmeToVerticalPixels(widget, offset, value));
    else
	return(XmeToHorizontalPixels(widget, offset, value));
}

/*	Function Name: ToPanedChildPixels
 *	Description: Converts to pixels from current unit type
 *                   does either horiz or vert depending on parent's
 *                   orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

static XmImportOperator
ToPanedChildPixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) XtParent(widget);

    if (IsVert(pw))
	return(XmeToVerticalPixels(widget, offset, value));
    else
	return(XmeToHorizontalPixels(widget, offset, value));
}

/*	Function Name: FromPanedOppositePixels
 *	Description: Converts from pixels to current unit type
 *                   does either horiz or vert depending on orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

static void
FromPanedOppositePixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) widget;

    if (IsVert(pw))
	XmeFromHorizontalPixels(widget, offset, value);
    else
	XmeFromVerticalPixels(widget, offset, value);
}

/*	Function Name: ToPanedOppositePixels
 *	Description: Converts to pixels from current unit type
 *                   does either horiz or vert depending on orientation.
 *	Arguments: widget - the paned widget.
 *                 offset, value - passed to correct function based
 *                                 on orientation.
 *	Returns: none
 */

static XmImportOperator
ToPanedOppositePixels(Widget widget, int offset, XtArgVal *value)
{
    XmPanedWidget pw = (XmPanedWidget) widget;

    if (IsVert(pw))
	return(XmeToHorizontalPixels(widget, offset, value));
    else
	return(XmeToVerticalPixels(widget, offset, value));
}

/*	Function Name: PaneSize
 *	Description: returns the width or height of the pane depending
 *                   upon the orientation we are using.
 *	Arguments: w - and widget.
 *                 vertical - TRUE if this is vertically oriented pane.
 *	Returns: the size requested
 *
 *      vertical  - return height
 *      !vertical - return width
 */

static Dimension
PaneSize(Widget w, Boolean vertical)
{
    if (vertical)
	return (w->core.height);
    return (w->core.width);
}

/*	Function Name: GetRequestInfo
 *	Description: returns request information.
 *	Arguments:  geo_struct - a geometry struct to get information out of.
 *                  vert - TRUE if this is a vertical paned widget.
 *	Returns: the request information.
 */

static Dimension
GetRequestInfo(XtWidgetGeometry *geo_struct, Boolean vert)
{
    if (vert)
	return ((Dimension) geo_struct->height);
    return ((Dimension) geo_struct->width);
}

/*	Function Name: ChoosePaneToResize.
 *	Description: This function chooses a pane to resize.
 *                   They are chosen using the following rules:
 *
 *                   1) size < max && size > min
 *                      if pane is growing or has a sash.
 *                      (keeps non sash panes from shrinking).
 *                   3) skip adjust == FALSE
 *                   4) widget not its prefered height &&
 *                      this change will bring it closer &&
 *                      The user has not resized this pane.
 *
 *                   If no widgets are found that fits all the rules then
 *                      rule #3 is broken.
 *                   If there are still no widgets found than
 *                      rule #2 is broken.
 *                   Rule #1 is never broken.
 *                   If no widgets are found then NULL is returned.
 *
 *	Arguments: pw - the paned widget.
 *                 paneindex - the index of the current pane.
 *                 dir - direction to search first.
 *                 shrink - TRUE if we need to shrink a pane, FALSE otherwise.
 *	Returns: pane to resize or NULL.
 */

static Pane
ChoosePaneToResize(XmPanedWidget pw,
		   int paneindex, Direction dir, Boolean shrink)
{
    Widget *childP;
    Pane pane;
    int rules = 3;
    Direction _dir = dir;
    int _index = paneindex;
    int curIndex;

    if ( (paneindex == NO_INDEX) || (dir == AnyPane) ) {  /* Use defaults. */
	_dir = LowRightPane;		/* Go up. - really. */
	_index = XmPaned_num_panes(pw) - 1;   /* Start the last pane, and work
					       backwards. */
    }

    childP = NthPane(pw, _index);
    while(TRUE) {
        pane = PaneInfo(*childP);

        if ((rules < 3 || SatisfiesRule3(pane, shrink)) &&
	    (rules < 2 || SatisfiesRule2(pane))         &&
	    (SatisfiesRule1(pane, shrink, dir))         &&
	    ((paneindex != PaneIndex(*childP)) || (dir == AnyPane)) )
	{
	    return(pane);
	}

	/*
	 * This is counter-intuitive, but if we are resizing the pane
	 * above the sash we want to choose a pane below the sash to lose,
	 * and visa-versa.
	 */

	if (shrink || (dir == AnyPane)) {
	    if (_dir == LowRightPane)
		--childP;
	    else
		++childP;
	}
	else {
	    if (_dir == LowRightPane)
		++childP;
	    else
		--childP;
	}


	/*
	 * If we have come to and edge then reduce the rule set, and try again.
	 * If we are reduced the rules to none, then return NULL.
	 */

	curIndex = childP - NthPane(pw, 0);
	if ((curIndex < 0) || (curIndex >= XmPaned_num_panes(pw))) {
	    if (--rules < 1)  /* less strict rules. */
		return(NULL);
	    childP = NthPane(pw, _index);
	}
    }
}

/*	Function Name: IsPane
 *	Description: Returns true if this is a pane.
 *	Arguments: w - a child of the paned widget.
 *	Returns: none.
 */

static Boolean
IsPane(Widget w)
{
    Pane pane = PaneInfo(w);

    return ((pane != NULL) && pane->is_a_pane);
}

/*	Function Name: StatisfiesRule1
 *	Description: check for to see if this pane satisfies rule 1.
 *	Arguments: pane - the pane to check.
 *                 shrink -TRUE if we want to shrink this pane, FALSE otherwise
 *	rEturns: TRUE if the rule is satisfied.
 */

static Boolean
SatisfiesRule1(Pane pane, Boolean shrink, Direction dir)
{
    /*
     * If this pane is at its preferred size and is asked to shrink
     * while having no sash then do not allow it.
     *
     * NOTE: If Dir == AnyPane then this is not due to a grip resize
     *       and this rule does not take effect.
     */

    if ((dir != AnyPane) &&
	shrink && (pane->sash == NULL) && (pane->size == pane->wp_size))
    {
	return(False);
    }

    return((shrink && (pane->size != pane->min)) ||
	   (!shrink && (pane->size != pane->max)) );
}

/*	Function Name: StatisfiesRule2
 *	Description: check for to see if this pane satisfies rule 2.
 *	Arguments: pane - the pane to check.
 *	Returns: TRUE if the rule is satisfied.
 */

static Boolean
SatisfiesRule2(Pane pane)
{
    return(!pane->skip_adjust);
}

/*	Function Name: StatisfiesRule3
 *	Description: check for to see if this pane satisfies rule 3.
 *	Arguments: pane - the pane to check.
 *                 shrink -TRUE if we want to shrink this pane, FALSE otherwise
 *	Returns: TRUE if the rule is satisfied.
 */

static Boolean
SatisfiesRule3(Pane pane, Boolean shrink)
{
    return ((pane->size != pane->wp_size) &&
	    ((shrink && ((int)pane->wp_size <= pane->size)) ||
	     (!shrink && ((int)pane->wp_size >= pane->size))) );
}

/*	Function Name: LoopAndRefigureChildren.
 *	Description: if we are resizing either the UpleftPane or LowRight Pane
 *                   loop through all the children to see if any will allow us
 *                   to resize them.
 *	Arguments: pw - the paned widget.
 *                 paneindex - the number of the pane border we are moving.
 *                 dir - the pane to move (either UpLeftPane or LowRightPane).
 *                 sizeused - current amount of space used.
 *                            THIS VALUE IS USED AND RETURNED.
 *	Returns: none.
 */

static void
LoopAndRefigureChildren(XmPanedWidget pw,
			int paneindex, int dir, Dimension *sizeused)
{
    int pane_size = (int) PaneSize( (Widget) pw, IsVert(pw));
    Boolean shrink = ((int)(*sizeused) > (int)pane_size);

    while (*sizeused != pane_size) { /* While all panes do not fit properly. */
	/*
	 * Choose a pane to resize.
	 * First look on the Pane Stack, and then go hunting for another one.
	 * If we fail to find a pane to resize then give up.
	 */
        Pane pane;
	int start_size;
	Dimension old;
	Boolean rule3_ok = FALSE, from_stack = TRUE;

	GetPaneStack(pw, shrink, &pane, &start_size);
	if (pane == NULL) {
	    pane = ChoosePaneToResize(pw, paneindex, (Direction)dir, shrink);
	    if (pane == NULL)
	        return; /* no one to resize, give up. */

	    rule3_ok = SatisfiesRule3(pane, shrink);
	    from_stack = FALSE;
	    PushPaneStack(pw, pane);
	}

	/*
	 * Try to resize this pane so that all panes will fit, take min and max
	 * into account.
	 */

	old = pane->size;
	pane->size += pane_size - *sizeused;

	if (from_stack) {
	    if (shrink) {
	        ASSIGN_MAX(pane->size, start_size);
	    }			/* don't remove these braces. */
	    else
	        ASSIGN_MIN(pane->size, start_size);

	    if (pane->size == start_size) (void) PopPaneStack(pw);
	}
	else if (rule3_ok) {
	    if (shrink) {
	        ASSIGN_MAX(pane->size, (int) pane->wp_size);
	    }			/* don't remove these braces. */
	    else
	        ASSIGN_MIN(pane->size, (int) pane->wp_size);
	}

	ASSIGN_MAX(pane->size, (int) pane->min);
	ASSIGN_MIN(pane->size, (int) pane->max);
	*sizeused += (pane->size - old);
    }
}

/*	Function Name: GetPrefSizes
 *	Description: Gets the preferred On Size.
 *	Arguments: w - the paned widget.
 * RETURNED        on_size, off_size;
 *	Returns: none.
 */

static void
GetPrefSizes(XmPanedWidget pw, Dimension *on_size, Dimension *off_size)
{
    Widget *childP;
    Pane pane;
    Dimension sash_size, sizeused;
    Boolean vert = IsVert(pw);

    if (on_size != NULL) {
	sizeused = 0;

	/*
	 * Get an initial estimate of the size we will use.
	 */

	if (vert)
	    sash_size = XmPaned_sash_height(pw);
	else
	    sash_size = XmPaned_sash_width(pw);

	for ( childP = XmPaned_managed_children(pw);
	      childP < (XmPaned_managed_children(pw)
			   + XmPaned_num_panes(pw));
	      childP++) {
	    pane = PaneInfo(*childP);
	    ASSIGN_MAX(pane->size, (int) pane->min);
	    ASSIGN_MIN(pane->size, (int) pane->max);
	    sizeused += (int) pane->size + 2 * (*childP)->core.border_width;

	    if (!IsLastPane(pw, childP)) {
		if (HasSash(*childP))
		    sizeused += MAX(XmPaned_internal_bw(pw), sash_size);
		else
		    sizeused += XmPaned_internal_bw(pw);
	    }
	}

	/*
	 * Get rid of extra spacing from previous 'for' loop and add in
	 * margin height at the top and bottom of the paned window
	 */

	if( vert)
	    sizeused += 2*XmPaned_margin_height(pw);
	else
	    sizeused += 2*XmPaned_margin_width(pw);

	*on_size = sizeused;
    }

    if (off_size != NULL) {
	sizeused = 1;

	for ( childP = XmPaned_managed_children(pw) ;
	      childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	      childP++ ) {
	    pane = PaneInfo(*childP);

	    if (XtIsManaged(*childP) && (pane->wp_off_size > sizeused))
		sizeused = pane->wp_off_size;
	}

	*off_size = sizeused;
    }
}

/*	Function Name: RefigureLocations
 *	Description: refigures all locations of children.
 *	Arguments: pw - the paned widget.
 *                 paneindex - child to start refiguring at.
 *                 dir - direction to move from child.
 *	Returns: True if anything actually moved.
 *
 *      There are special arguments to paneindex and dir, they are:
 *      paneindex - NO_INDEX.
 *      dir   - AnyPane.
 *
 *      If either of these is true then all panes may be resized and
 *      the choosing of panes procedes in reverse order starting with the
 *      last child.
 */

static Boolean
RefigureLocations(XmPanedWidget pw, int paneindex, Direction dir)
{
    Widget *childP;
    Pane pane;
    int pane_size = (int) PaneSize( (Widget) pw, IsVert(pw) );
    Dimension old, sizeused;
    Position loc = 0;
    int sash_size, moved = True;

    if (XmPaned_num_panes(pw) == 0)
	return False;

    GetPrefSizes(pw, &sizeused, NULL);

    if (sizeused == pane_size)
	moved = False;
    else
	LoopAndRefigureChildren(pw, paneindex, dir, &sizeused);

    /*
     * If we still are not the right size, then tell the pane that
     * wanted to resize that it can't.
     */

    if ( (paneindex != NO_INDEX) && (dir != AnyPane) ) {
	pane = PaneInfo(*NthPane(pw, paneindex));
	old = pane->size;

	if(pane_size != sizeused) {
	    moved = False;
	}
	pane->size += pane_size - sizeused;
	ASSIGN_MAX(pane->size, (int) pane->min);
	ASSIGN_MIN(pane->size, (int) pane->max);
	sizeused += pane->size - old;
    }

    if (!XmPaned_allow_unused_space(pw))
    {
      if (pane_size > sizeused)
      {
        PaneInfo(*NthPane(pw, paneindex-1))->size += (pane_size - sizeused);
      }
    }

    /*
     * It is possible that the panes will not fit inside the vpaned widget, but
     * we have tried out best.
     *
     * Assign each pane a location.
     */

    if(IsVert(pw)) {
	loc = XmPaned_margin_height(pw);
    } else {
	loc = XmPaned_margin_width(pw);
    }

    /*
     * This is where we actually tell the widget where to draw the track
     * lines.
     */

    if (IsVert(pw))
	sash_size = XmPaned_sash_height(pw);
    else
	sash_size = XmPaned_sash_width(pw);

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
	if (LayoutIsRtoLM(pw) && !(IsVert(pw))) {
	    loc += PaneInfo(*childP)->size + 2 * (*childP)->core.border_width;
	    PaneInfo(*childP)->delta = pw->core.width - loc;
	} else {
	    PaneInfo(*childP)->delta = loc;
	    loc += PaneInfo(*childP)->size + 2 * (*childP)->core.border_width;
	}

	if (HasSash(*childP))
	    loc += MAX(XmPaned_internal_bw(pw), sash_size);
	else
	    loc += XmPaned_internal_bw(pw);
    }

    return moved;
}

/*	Function Name: CommitNewLocations
 *	Description: Commits all of the previously figured locations.
 *	Arguments: pw - the paned widget.
 *                 no_resize_child - child not to resize
 *	Returns: none.
 *
 * no_resize_child is only used by the geometry manager to implement an
 *                 XtGeometryYes policy as Motif1.2 dictates.
 */

static void
CommitNewLocations(XmPanedWidget pw, Widget no_resize_child)
{
    Dimension width, height;
    Widget *childP, sash, separator;
    Pane pane;
    XWindowChanges changes;
    XWindowChanges sep;
    int	offset, sash_size, internal_space;

    if (!XmPaned_refiguremode(pw))
	return;

    if (IsVert(pw)) {
	offset = XmPaned_margin_width(pw);
	sash_size = XmPaned_sash_height(pw);
    }
    else {
	offset = XmPaned_margin_height(pw);
	sash_size = XmPaned_sash_width(pw);
    }

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
	pane = PaneInfo(*childP);
	sash = pane->sash; /* may be NULL. */
	separator = pane->separator; /* may be NULL. */

	if (HasSash(*childP))
	    internal_space = MAX(XmPaned_internal_bw(pw), sash_size);
	else
	    internal_space = XmPaned_internal_bw(pw);

	/* Ensure we don't underflow the child dimensions */
	width = pw->core.width - 2 * (offset + (*childP)->core.border_width);
	if (width > pw->core.width)
		width = 1;

	height = pw->core.height - 2 * (offset + (*childP)->core.border_width);
	if (height > pw->core.height)
		height = 1;

	if (IsVert(pw)) {
	    if (*childP == no_resize_child) {
		(*childP)->core.x = offset;
		(*childP)->core.y = pane->delta;
		(*childP)->core.width = width;
		(*childP)->core.height = pane->size;
	    }
	    else {
		_XmConfigureWidget(*childP, (Position) offset, pane->delta, width,
				   (Dimension)pane->size,
				   (Dimension)(*childP)->core.border_width);
	    }

	    if (sash != NULL) {	    /* Move and Display the Sash */

		/*
		 * Motif style does - indent from right edge, pos from left
		 */

	        if(XmPaned_sash_indent(pw) < 0) {
	            changes.x = (pw->core.width + XmPaned_sash_indent(pw) -
				 sash->core.width -
				 sash->core.border_width * 2);
		} else {
	            changes.x = XmPaned_sash_indent(pw);
		}
		changes.y = ((*childP)->core.y + (*childP)->core.height +
			     2 * (*childP)->core.border_width +
			     internal_space/2 - sash->core.height/2 -
			     sash->core.border_width);
	    }

	    if (separator != NULL) {
		sep.width = pw->core.width;
		sep.height = XmPaned_sash_shadow_thickness(pw);
		sep.x = 0;
		sep.y = ((*childP)->core.y + (*childP)->core.height -
			 sep.height / 2 - separator->core.border_width +
			 internal_space / 2 +
			 2 * (*childP)->core.border_width);
	    }

	} else {
	    if (*childP == no_resize_child) {
		(*childP)->core.x = pane->delta;
		(*childP)->core.y = offset;
		(*childP)->core.width = pane->size;
		(*childP)->core.height = height;
	    }
	    else {
		_XmConfigureWidget(*childP, pane->delta, (Position) offset,
				   (Dimension) pane->size, height,
				   (Dimension) (*childP)->core.border_width);
	    }

	    if (sash != NULL) {	    /* Move and Display the Sash */
		if (LayoutIsRtoLM(pw))
		    changes.x = ((*childP)->core.x -
			     internal_space / 2 - sash->core.width/2 -
			     sash->core.border_width);
		else
		    changes.x = ((*childP)->core.x + (*childP)->core.width +
			     2 * (*childP)->core.border_width +
			     internal_space / 2 - sash->core.width/2 -
			     sash->core.border_width);
		/* Motif style does - indent from right edge, pos from left */
	        if(XmPaned_sash_indent(pw) < 0) {
		    changes.y = pw->core.height + XmPaned_sash_indent(pw) -
			sash->core.height - sash->core.border_width*2;
		}
		else {
		    changes.y = XmPaned_sash_indent(pw);
		}
	    }

	    if (separator != NULL) {
		sep.width = XmPaned_sash_shadow_thickness(pw);
		sep.height = pw->core.height;
		sep.y = 0;
		if (LayoutIsRtoLM(pw))
		    sep.x = ((*childP)->core.x -
			 sep.width / 2 - separator->core.border_width -
			 internal_space / 2);
		else
		    sep.x = ((*childP)->core.x + (*childP)->core.width -
			 sep.width / 2 - separator->core.border_width +
			 internal_space / 2 +
			 2 * (*childP)->core.border_width);
	    }
	}

	/*
	 * This should match XtMoveWidget, except that we're also insuring the
	 * sash is Raised in the same request.
	 */

	if(separator != NULL) {
	    _XmConfigureWidget(separator, sep.x, sep.y,
			       sep.width, sep.height,
			       separator->core.border_width);
	}

	if (sash != NULL) {
	    sash->core.x = changes.x;
	    sash->core.y = changes.y;
	    changes.stack_mode = Above;	/* assures sash is always above sep. */

	    if (XtIsRealized(sash)) {
		XmDropSiteStartUpdate((Widget) pw);

	        XConfigureWindow(XtDisplay(sash), XtWindow(sash),
				 CWX | CWY | CWStackMode, &changes );

		XmDropSiteEndUpdate((Widget) pw);
	    }
	}
    }

    ClearPaneStack(pw);
}

/*	Function Name: RefigureLocationsAndCommit
 *	Description: Refigures all locations in a paned widget and
 *                   commits them immediately.
 *	Arguments: pw - the paned widget.
 *	Returns: none
 *
 *      This function does nothing if any of the following are true.
 *      o refiguremode is false.
 *      o The widget is unrealized.
 *      o There are no panes is the paned widget.
 *
 *      NOTE: This is the resize Procedure for the Paned widget.
 */

static void
RefigureLocationsAndCommit(Widget w)
{
    XmPanedWidget pw = (XmPanedWidget) w;

    if (XmPaned_refiguremode(pw) && XtIsRealized( (Widget) pw) &&
	XmPaned_num_panes(pw) > 0 )
    {
	RefigureLocations(pw, NO_INDEX, AnyPane);
	CommitNewLocations(pw, NULL);
    }
}

/*	Function Name: _DrawRect
 *	Description: Draws a rectangle in the proper orientation.
 *	Arguments: pw - the paned widget.
 *                 gc - gc to used for the draw.
 *                 on_olc, off_loc - location of upper left corner of rect.
 *                 on_size, off_size - size of rectangle.
 *	Returns: none
 */

static void
_DrawRect(XmPanedWidget pw, GC gc, int on_loc, int off_loc,
	  unsigned int on_size, unsigned int off_size)
{
    if (IsVert(pw))
	XFillRectangle(XtDisplay(pw), XtWindow(pw), gc,
		       off_loc, on_loc, off_size, on_size);
    else
	XFillRectangle(XtDisplay(pw), XtWindow(pw), gc,
		       on_loc, off_loc, on_size, off_size);
}

/*	Function Name: _DrawTrackLines
 *	Description: Draws the lines that animate the pane borders when the
 *                   sashs are moved.
 *	Arguments: pw - the Paned widget.
 *                 erase - if True then just erase track lines, else
 *                         draw them in.
 *	Returns: none.
 */

static void
_DrawTrackLines(XmPanedWidget pw, Boolean erase)
{
    Widget *childP;
    Pane pane;
    int on_loc, off_loc;
    unsigned int on_size, off_size;
    int sash_size, internal_space;

    /* Always full width of the widget accross the other dimension */
    off_loc = 0;
    off_size = PaneSize((Widget) pw, !IsVert(pw));

    if(IsVert(pw))
	sash_size = XmPaned_sash_height(pw);
    else
	sash_size = XmPaned_sash_width(pw);

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
        pane = PaneInfo(*childP);

	/*
	 * First child never has a track line.
	 */

        if ((((IsVert(pw) || !(LayoutIsRtoLM(pw))) &&
	   (XmPaned_managed_children(pw) != childP)) ||
	   (!(IsVert(pw)) && LayoutIsRtoLM(pw) &&
	   !(IsLastPane(pw, childP)))) &&
	   (erase || (pane->olddelta != pane->delta)))
	{
	    if(IsVert(pw)) {
		on_size = pane->separator ?
			pane->separator->core.height : (Dimension) 2;
	    } else {
		on_size = pane->separator ?
			pane->separator->core.width: (Dimension) 2;
	    }

	    if (HasSash(*(childP - 1)))
		internal_space = MAX(XmPaned_internal_bw(pw), sash_size);
	    else
		internal_space = XmPaned_internal_bw(pw);

	    if (!erase && (pane->olddelta != NO_DELTA)) {
		/* This formula might have to correct for border width of
		 * the sep in the future, but the Paned widget creates them
		 * with a zero width border just to make sure, so it is never
		 * an issue.  I did not feel the need to allow users to
		 * specify borders around the separators because it simply
		 * would look stupid. -- aja
		 */
		on_loc = (pane->olddelta -
			  ((int) on_size + internal_space) / 2);

		_DrawRect(pw, XmPaned_flipgc(pw),
			  on_loc, off_loc, on_size, off_size);
	    }

	    on_loc = (pane->delta -
		      ((int) on_size + internal_space) / 2);

	    _DrawRect(pw, XmPaned_flipgc(pw),
		      on_loc, off_loc, on_size, off_size);
	}

	pane->olddelta = pane->delta;
    }
}

/*
 * This allows good reuse of code, as well as descriptive function names.
 */

#define DrawTrackLines(pw) _DrawTrackLines((pw), FALSE);
#define EraseTrackLines(pw) _DrawTrackLines((pw), TRUE);

/*	Function Name: GetEventLocation
 *	Description: Converts and event to an x and y location.
 *	Arguments: pw - the paned widget.
 *                 event - a pointer to an event.
 *	Returns: if this is a vertical pane then (y) else (x).
 */

static int
GetEventLocation(XmPanedWidget pw, XEvent *event)
{
    switch (event->xany.type) {
    case ButtonPress:
    case ButtonRelease:
	return((IsVert(pw)) ? event->xbutton.y_root : event->xbutton.x_root);
    case KeyPress:
    case KeyRelease:
	return((IsVert(pw)) ? event->xkey.y_root : event->xkey.x_root);
    case MotionNotify:
	return((IsVert(pw)) ? event->xmotion.y_root : event->xmotion.x_root);
    default:
	return XmPaned_start_loc(pw);
    }
}

/*	Function Name: StartSashAdjustment
 *	Description: Starts the sash adjustment procedure.
 *	Arguments: pw - the paned widget.
 *                 sash - the sash widget selected.
 *	Returns: none.
 */

static void
StartSashAdjustment(XmPanedWidget pw, Widget sash)
{
    Widget *childP;

    XmPaned_repane_status(pw) = BEGAN_ADJUST;

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
        PaneInfo(*childP)->olddelta = NO_DELTA;
    }
}

/*	Function Name: MoveSashAdjustment
 *	Description: This routine moves all panes around when a sash is moved.
 *	Arguments: pw - the paned widget.
 *                 sash - the sash that we are moving.
 *                 loc - location of pointer in proper direction.
 *	Returns: none.
 */

static void
MoveSashAdjustment(XmPanedWidget pw, Widget sash, int loc)
{
    int index, diff = loc - XmPaned_start_loc(pw);
    Boolean vert = IsVert(pw);
    Widget w;

    w = *NthPane(pw, PaneIndex(sash));
    /* CR03589 CR03163 should check the sash loc is over the maximum */
    if ((PaneSize(w, vert) + diff) > PaneInfo(w)->max)
	diff = PaneInfo(w)->max - PaneSize(w, vert);
    if (LayoutIsRtoLM(pw) && !(IsVert(pw)))
	PaneInfo(w)->size = PaneSize(w, vert) - diff;
    else
	PaneInfo(w)->size = PaneSize(w, vert) + diff;

    w = *NthPane(pw, PaneIndex(sash) + 1);
    if (LayoutIsRtoLM(pw) && !(IsVert(pw)))
	PaneInfo(w)->size = PaneSize(w, vert) + diff;
    else
	PaneInfo(w)->size = PaneSize(w, vert) - diff;

    index = PaneIndex(sash);
    if (diff < 0)
	index += 1;

    (void) RefigureLocations(pw, index,
			     (diff < 0) ? LowRightPane : UpLeftPane);
}

/*	Function Name: CommitSashAdjustment
 *	Description: Commits the sash adjustment.
 *	Arguments: pw - the paned widget.
 *	Returns: none
 */

static void
CommitSashAdjustment(XmPanedWidget pw, Widget sash)
{
    int i;

    /*
     * We only do it if we were in a sash adjustment, this prevents odd
     * effects if the user binds a button up to commit, with no
     * corresponding start on button down.
     */

    if(XmPaned_repane_status(pw) != NO_ADJUST) {
    	EraseTrackLines(pw);
    	CommitNewLocations(pw, NULL);
	XmPaned_repane_status(pw) = NO_ADJUST;
    }

    /*
     * Set the new user preferred size of the pane above and below the sash.
     */

    for (i = 0; i < 2; i++) {
	Pane pane = PaneInfo(*NthPane(pw, PaneIndex(sash) + i));
	pane->wp_size = pane->size;
    }
}

/*************************************<->*************************************
 *
 *  ProcessKeyEvent
 *
 *   Description:
 *   -----------
 *    This function processes a batch of key pressed events
 *    so that a sash movement action via the keyboard doesn't
 *    get too far behind the key event actions.
 *
 *************************************<->***********************************/

static void
ProcessKeyEvent(XtPointer client_data, XtIntervalId *id)
{
    Widget sash = (Widget)client_data;
    XmPanedWidget pw = (XmPanedWidget)XtParent(sash);
    Widget *childP;
    int i;

    /*
     * NOTE:  w is a sash, to get position we have
     * to get index of pane associated with this sash.
     */

    childP = XmPaned_managed_children(pw) + PaneIndex(sash);

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
	PaneInfo(*childP)->olddelta = NO_DELTA;
    }

    XmPaned_start_loc(pw) = 0;
    MoveSashAdjustment(pw, sash, XmPaned_increment_count(pw));
    CommitNewLocations(pw, NULL);

    /*
     * Set the new user preferred size of the pane above and below the sash.
     */

    for (i = 0; i < 2; i++) {
	Pane pane = PaneInfo(*NthPane(pw, PaneIndex(sash) + i));
	pane->wp_size = pane->size;
    }

    XmPaned_increment_count(pw) = 0;
}

/*	Function Name: HandleSash
 *	Description: Handles the sash manipulations.
 *	Arguments: sash - the sash widget that has been moved.
 *                 junk - ** NOT USED **
 *                 call_data - data passed to us from the sash widget.
 *	Returns: none.
 */

static void
HandleSash(Widget sash, XtPointer junk, XtPointer callData)
{
    struct XmSashCallbackData *call_data = (struct XmSashCallbackData *)callData;
    XmPanedWidget pw = (XmPanedWidget) XtParent(sash);
    int loc;
    char action_type;
    short increment;
    static String params[] = { "HandleSash" };
    Cardinal num = 1;
    XEvent *event = (XEvent *) call_data->event;

    action_type = *call_data->params[0];

    if (call_data->num_params == 0                             ||
	(action_type == 'C' && call_data->num_params != 1)     ||
	(action_type == 'K' && call_data->num_params != 3)     ||
	(action_type == 'M' && call_data->num_params != 1)     ||
	(action_type == 'S' && call_data->num_params != 1))
    {
	_XmWarningMsg((Widget) pw,
		XmNbadActionParameters, XmNbadActionParametersMsg,
		params, num);

	return;
    }

    if (isascii(action_type) && islower(action_type))
	action_type = toupper(action_type);

    loc = GetEventLocation(pw, event);

    if (event->xany.type == KeyPress) {
	char restptr[BUFSIZ];	/* points to the rest of the string. */

	/* Get movement size */

	if ((increment = atoi(call_data->params[1])) == 0) {
	    if (streq(call_data->params[1], LARGE_INC))
		increment = 10;
	    else
		increment = 1;
	}

	switch(call_data->params[2][0]) {
	default:
	    return;
	case  'U':		/* Up */
	    XmCopyISOLatin1Lowered(restptr, call_data->params[2] + 2);

	    if (!IsVert(pw))
		increment = 0;
	    else
		increment = -increment;
	    break;
	case 'L':		/* Left */
	    XmCopyISOLatin1Lowered(restptr, call_data->params[2] + 4);
	    if (IsVert(pw))
		increment = 0;
	    else
		increment = -increment;
	    break;
	case  'D':		/* Down */
	    XmCopyISOLatin1Lowered(restptr, call_data->params[2] + 4);
	    if (!IsVert(pw))
		increment = 0;
	    break;
	case  'R':		/* Right */
	    XmCopyISOLatin1Lowered(restptr, call_data->params[2] + 5);
	    if (IsVert(pw))
		increment = 0;
	    break;
	}

	/*
	 * Check to see if there is a string after the up/left/down/right
	 * and if there is then change the keyboard traversal mode.
	 */

	if (XmPaned_increment_count(pw) == 0) {
	    XmPaned_increment_count(pw) = increment;
	    XtAppAddTimeOut(XtWidgetToApplicationContext((Widget) pw),
			    XtGetMultiClickTime(XtDisplay((Widget) pw)),
			    ProcessKeyEvent, (XtPointer) sash);
	} else
	    XmPaned_increment_count(pw) += increment;

	return;
    }

    if ((event->xany.type == ButtonPress) ||
	(event->xany.type == ButtonRelease))
    {
	if (event->xbutton.button != 1)
	    return;
    }

    switch (action_type) {
    case 'S':		/* Start adjustment */
	XmPaned_resize_children_to_pref(pw) = FALSE;
	StartSashAdjustment(pw, sash);
	DrawTrackLines(pw);
	XmPaned_start_loc(pw) = loc;
	break;

    case 'M':
	MoveSashAdjustment(pw, sash, loc);
	DrawTrackLines(pw);
	break;

    case 'C':
	CommitSashAdjustment(pw, sash);
	break;

    default:
        {
	    _XmWarningMsg((Widget) pw, XmNbadActionParameters,
		    XmNbadActionParametersMsg, params, num);

	    return;
	}
    }
}

/*	Function Name: ManageAndUnmanageSashs
 *	Description: This function manages and unmanages the sashs so that
 *                   the managed state of each sash matches that of its pane.
 *	Arguments: pw - the paned widget.
 *	Returns: none.
 */

static void
ManageAndUnmanageSashs(XmPanedWidget pw)
{
   WidgetList managed_sashs, unmanaged_sashs;
   Widget *managedP, *unmanagedP;
   WidgetList managed_seps, unmanaged_seps;
   Widget *managedS, *unmanagedS;
   Widget *childP;
   Cardinal alloc_size;
   Boolean last_child_has_sash = False;

   alloc_size = (Cardinal) sizeof(Widget) * pw->composite.num_children / 2;
   managedP = managed_sashs = (WidgetList) XtMalloc(alloc_size);
   unmanagedP = unmanaged_sashs = (WidgetList) XtMalloc(alloc_size);
   managedS = managed_seps = (WidgetList) XtMalloc(alloc_size);
   unmanagedS = unmanaged_seps = (WidgetList) XtMalloc(alloc_size);

   ForAllChildren(pw, childP) {
       if (!IsPane(*childP))
	   continue;

       if (HasSash(*childP)) {
	   last_child_has_sash = True;
	   if (XtIsManaged(*childP))
	       *managedP++ = PaneInfo(*childP)->sash;
	   else
	       *unmanagedP++ = PaneInfo(*childP)->sash;
       }
       else
	   last_child_has_sash = False;

       if (HasSep(*childP)) {
	   if (XtIsManaged(*childP))
	       *managedS++ = PaneInfo(*childP)->separator;
	   else
	       *unmanagedS++ = PaneInfo(*childP)->separator;
       }
   }

   if (managedP != managed_sashs) {
       if (last_child_has_sash) {
	   *unmanagedP = *--managedP;   /* Last sash is never managed */
	   unmanagedP++;
       }

       XtManageChildren( managed_sashs, (Cardinal)(managedP - managed_sashs) );
   }

   if (unmanagedP != unmanaged_sashs)
       XtUnmanageChildren(unmanaged_sashs,
			   (Cardinal)(unmanagedP - unmanaged_sashs) );

   XtFree((char *)managed_sashs);
   XtFree((char *)unmanaged_sashs);

   if (managedS != managed_seps) {
       if (last_child_has_sash) {
	   *unmanagedS = *--managedS;   /* Last sash is never managed */
	   unmanagedS++;
       }

       XtManageChildren( managed_seps, (Cardinal)(managedS - managed_seps) );
   }

   if (unmanagedS != unmanaged_seps)
       XtUnmanageChildren( unmanaged_seps,
			   (Cardinal)(unmanagedS - unmanaged_seps) );

   XtFree((char *)managed_seps);
   XtFree((char *)unmanaged_seps);
}

/*	Function Name: CreateSeparator
 *	Description: Creates a separator widget.
 *	Arguments: child - the child that wants a sep. to be created for it.
 *	Returns: none.
 */

static void
CreateSeparator(Widget child)
{
    XmPanedWidget pw = (XmPanedWidget) XtParent(child);
    Arg args[10];
    Cardinal num_args = 0;

    if (PaneInfo(child)->separator != NULL)
	return;

    XtSetArg(args[num_args], XmNborderWidth, 0); num_args++;
    XtSetArg(args[num_args], XmNhighlightThickness, 0); num_args++;
    XtSetArg(args[num_args], XmNseparatorType, XmSHADOW_ETCHED_IN); num_args++;
    XtSetArg(args[num_args], XmNmargin, (int)(4 * (XmScreenDpi(XmScreenOfObject(pw)) / 96.))); num_args++;
    XtSetArg(args[num_args], XmNnavigationType, XmNONE); num_args++;
    XtSetArg(args[num_args], XmNisAPane, False); num_args++;

    if (IsVert(pw)) {
        XtSetArg(args[num_args], XmNorientation, XmHORIZONTAL); num_args++;
        XtSetArg(args[num_args], XmNwidth, pw->core.width); num_args++;
    } else {
        XtSetArg(args[num_args], XmNorientation, XmVERTICAL); num_args++;
        XtSetArg(args[num_args], XmNheight, pw->core.height); num_args++;
    }

    PaneInfo(child)->separator = XtCreateWidget("separator",
						xmSeparatorWidgetClass,
						(Widget)pw, args, num_args);
}

/*	Function Name: CreateSash
 *	Description: Creates a sash widget.
 *	Arguments: child - the child that wants a sash to be created for it.
 *	Returns: none.
 */
static void CreateSash(Widget child)
{
    XmPanedWidget pw = (XmPanedWidget) XtParent(child);
    Arg arglist[20];
    Cardinal num_args = 0;

    XtSetArg(arglist[num_args], XmNcursor, XmPaned_sash_cursor(pw)); num_args++;
    XtSetArg(arglist[num_args], XmNtranslations, XmPaned_sash_translations(pw));
    num_args++;
    XtSetArg(arglist[num_args], XmNwidth, XmPaned_sash_width(pw)); num_args++;
    XtSetArg(arglist[num_args], XmNheight, XmPaned_sash_height(pw)); num_args++;
    XtSetArg(arglist[num_args], XmNshadowThickness,
	     XmPaned_sash_shadow_thickness(pw)); num_args++;
    XtSetArg(arglist[num_args], XmNtraversalOn, True); num_args++;
    XtSetArg(arglist[num_args], XmNnavigationType, XmTAB_GROUP);
    num_args++;
    XtSetArg(arglist[num_args], XmNunitType, XmPIXELS); num_args++;
    XtSetArg(arglist[num_args], XmNisAPane, False); num_args++;
    PaneInfo(child)->sash = XtCreateWidget("sash", xmSashWidgetClass,
					   (Widget)pw, arglist, num_args);

    XtAddCallback(PaneInfo(child)->sash, XmNcallback,
		  HandleSash, (XtPointer) child);
}

/*	Function Name: GetGCs
 *	Description: Gets new GC's.
 *	Arguments: w - the paned widget.
 *	Returns: none.
 */

static void
GetGCs(Widget w)
{
    XmPanedWidget pw = (XmPanedWidget) w;
    XtGCMask valuemask;
    XGCValues values;

    /*
     * Draw Track lines (animate pane borders) in foreground color ^ bg color.
     */

    values.function = GXinvert;
    values.plane_mask = pw->manager.foreground ^ pw->core.background_pixel;
    values.subwindow_mode = IncludeInferiors;
    valuemask = GCPlaneMask | GCFunction | GCSubwindowMode;
    XmPaned_flipgc(pw) = XtGetGC(w, valuemask, &values);
}

/*	Function Name: ReleaseGCs
 *	Description: Releases all GC's associated with this widget.
 *	Arguments: w - the widget.
 *	Returns: none.
 */

static void
ReleaseGCs(Widget w)
{
    XmPanedWidget pw = (XmPanedWidget)w;

    XtReleaseGC( w, XmPaned_flipgc(pw) );
}

/*	Function Name: SetChildrenPrefSizes.
 *	Description: Sets the preferred sizes of the children.
 *	Arguments: pw - the paned widget.
 *                 off_size - query children based on this off size.
 *                 use_off_size - If False then ignore the off size.
 *                 only set the pref size if this is a new child.
 *	Returns: none.
 */

static void
SetChildrenPrefSizes(XmPanedWidget pw, Dimension off_size,
		     Boolean use_off_size, Boolean only_if_new)
{
    Widget * childP;
    Boolean vert = IsVert(pw);
    XtWidgetGeometry request, reply;

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
	Pane pane = PaneInfo(*childP);

	if (only_if_new && pane->prefs_inited)
	    break;

	pane->prefs_inited = True;

        if (XmPaned_resize_children_to_pref(pw) || (pane->size == 0) ||
	    (pane->resize_to_pref))
	{
	    if (pane->preferred_size != XmPanedAskChild) {
	        pane->wp_size = pane->preferred_size;
		if (!use_off_size) {
		    (void) XtQueryGeometry(*childP, NULL, &reply);

		    pane->wp_off_size = GetRequestInfo(&reply, !vert);
		}
	    }
	    else {
		request.request_mode = 0;
		if (use_off_size) {
		    if( vert ) {
			request.request_mode |= CWWidth;
			request.width = off_size;
		    }
		    else {
			request.request_mode |= CWHeight;
			request.height = off_size;
		    }
		}

		(void) XtQueryGeometry(*childP, &request, &reply);

		pane->wp_size = GetRequestInfo(&reply, vert);
		if (!use_off_size)
		    pane->wp_off_size = GetRequestInfo(&reply, !vert);
	    }

	    pane->size = pane->wp_size;
	}
	else if (!use_off_size) {
	    (void) XtQueryGeometry(*childP, NULL, &reply);

	    pane->wp_off_size = GetRequestInfo(&reply, !vert);
	}
    }
}

/*	Function Name: ResetSash
 *	Description: Turn on or off the sash depending on the state of
 *                   the paned widget.
 *	Arguments: set - the widget this sash is associated with.
 *	Returns: none.
 */

static void
ResetSash(Widget set)
{
    Pane pane = PaneInfo(set);
    XmPanedWidget pw = (XmPanedWidget) XtParent(set);

    if (HasSash(set) && (!pane->show_sash || ForceSashOff(pane))) {
	XtDestroyWidget(pane->sash);
	pane->sash = NULL;
    }
    else if (!HasSash(set) && pane->show_sash && !ForceSashOff(pane)) {
	CreateSash(set);
	if (XtIsRealized((Widget) pw)) {
	    /*
	     * If paned is unrealized this will happen automatically
	     * at realize time.
	     */

	    if (XtIsManaged(set)) {
		/*
		 * Since the paned is realized this will
		 * manage and realize the sash.
		 */
		XtManageChild(PaneInfo(set)->sash);
	    }
	}
    }

    RefigureLocationsAndCommit((Widget) pw);
}

/************************************************************
 *
 * Stack Manipulation routines (Private).
 *
 ************************************************************/

/*	Function Name: PushPaneStack
 *	Description: Pushes a value onto the pane stack.
 *	Arguments: pw - the paned widget.
 *                 pane - the pane that we are pushing.
 *	Returns: none.
 */

static void
PushPaneStack(XmPanedWidget pw, Pane pane)
{
    PaneStack * stack = (PaneStack *) XtMalloc(sizeof(PaneStack));

    stack->next = XmPaned_stack(pw);
    stack->pane = pane;
    stack->start_size = pane->size;

    XmPaned_stack(pw) = stack;
}

/*	Function Name: GetPaneStack
 *	Description: Gets the top value from the pane stack.
 *	Arguments: pw - the paned widget.
 *                 shrink - TRUE if we want to shrink this pane,
 *                          FALSE otherwise.
 * ** RETURNED **  pane - the pane that we are popping.
 * ** RETURNED **  start_size - the size that this pane started at.
 *	Returns: none.
 */

static void
GetPaneStack(XmPanedWidget pw, Boolean shrink, Pane *pane, int *start_size)
{
    if (XmPaned_stack(pw) == NULL)
	*pane = NULL;
    else {
	*pane = XmPaned_stack(pw)->pane;
	*start_size = XmPaned_stack(pw)->start_size;

	if (shrink != ((*pane)->size > *start_size))
	    *pane = NULL;
    }
}

/*	Function Name: PopPaneStack
 *	Description: Pops the top item off the pane stack.
 *	Arguments: pw - the paned widget.
 *	Returns: TRUE if this is not the last element on the stack.
 */

static Boolean
PopPaneStack(XmPanedWidget pw)
{
    PaneStack * stack = XmPaned_stack(pw);

    if (stack == NULL)
	return(FALSE);

    XmPaned_stack(pw) = stack->next;
    XtFree((char*)stack);

    return(XmPaned_stack(pw) != NULL);
}

/*	Function Name: ClearPaneStack
 *	Description: removes all entries from the pane stack.
 *	Arguments: pw - the paned widget.
 *	Returns: none
 */

static void
ClearPaneStack(XmPanedWidget pw)
{
    while(PopPaneStack(pw)) {}	/* SUPPRESS 530 */
}

/************************************************************
 *
 * Semi-public routines.
 *
 ************************************************************/

/* The Geometry Manager only allows changes after Realize if
 * allow_resize is True in the constraints record.
 *
 * For vertically paned widgets:
 *
 * It only allows height changes, but offers the requested height
 * as a compromise if both width and height changes were requested.
 *
 * For horizontal widgets the converse is true.
 * As all good Geometry Managers should, we will return No if the
 * request will have no effect; i.e. when the requestor is already
 * of the desired geometry.
 */

static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
    XmPanedWidget pw = (XmPanedWidget) XtParent(w);
    XtGeometryMask mask = request->request_mode;
    Dimension old_size, old_wpsize, old_wp_off_size, old_paned_size;
    Pane pane = PaneInfo(w);
    Boolean vert = IsVert(pw);
    Dimension on_size, off_size;
    XtGeometryResult result;
    Boolean almost = FALSE;

/*
 * If any of the following is true, disallow the geometry change.
 *
 * o The paned widget is realized and allow_resize is false for the pane.
 * o The requested size is the same as the current size.
 */

    if (!pane->is_a_pane) {
	if (request->request_mode & CWWidth)
	    w->core.width = request->width;
	if (request->request_mode & CWHeight)
	    w->core.height = request->height;
	if (request->request_mode & CWX)
	    w->core.x = request->x;
	if (request->request_mode & CWY)
	    w->core.y = request->y;

	return(XtGeometryYes);	/* allow any change to sashes or seps. */
    }

    if (!(request->request_mode & CWWidth))
	request->width = w->core.width;

    if (!(request->request_mode & CWHeight))
	request->height = w->core.height;

    if ((XtIsRealized((Widget)pw) && !pane->allow_resize)        ||
        ((GetRequestInfo(request, vert) == PaneSize(w, vert)) &&
	 (GetRequestInfo(request, !vert) == PaneSize(w, !vert))))
    {
        return XtGeometryNo;
    }

    old_paned_size = PaneSize((Widget) pw, vert);
    old_wpsize = pane->wp_size;
    old_size = pane->size;
    old_wp_off_size = pane->wp_off_size;

    if (vert) {
	if (mask & CWHeight)
	    pane->wp_size = pane->size = GetRequestInfo(request, vert);
	if (mask & CWWidth)
	    pane->wp_off_size = GetRequestInfo(request, !vert);
    }
    else {
	if (mask & CWWidth)
	    pane->wp_size = pane->size = GetRequestInfo(request, vert);
	if (mask & CWHeight)
	    pane->wp_off_size = GetRequestInfo(request, !vert);
    }


    GetPrefSizes(pw, NULL, &off_size);

    /*
     * Ask our parent what would happen if we wanted to asked to be
     * the a new size that will satisify the request of our child?
     */

    result = AdjustPanedSize(pw, off_size, True, True, &on_size, &off_size);

    /*
     * Fool the Refigure Locations proc to thinking that we are
     * a different on_size;
     */

    if (result != XtGeometryNo) {
	if (vert) {
	    pw->core.height = on_size + 2 * XmPaned_margin_height(pw);
	}
	else {
	    pw->core.width = on_size + 2 * XmPaned_margin_width(pw);
	}
    }

    RefigureLocations(pw, PaneIndex(w), AnyPane);

/*
 * Set up reply struct and reset core on_size.
 */

    if (vert) {
        pw->core.height = old_paned_size;
	reply->width = off_size;
        reply->height = pane->size;
    }
    else {
        pw->core.width = old_paned_size;
	reply->width = pane->size;
        reply->height = off_size;
    }

/*
 * IF either of the following is true.
 *
 * o There was a "off_size" request and the new "off_size" is different
 *   from that requested.
 * o There was no "off_size" request and the new "off_size" is different
 *
 * o The "on_size" we will allow is different from that requested.
 *
 * THEN: set almost
 */

    if ( !((vert ? CWWidth : CWHeight) & mask))
    {
        if (vert)
        {
	    request->width = w->core.width;
        }
        else
        {
	    request->height = w->core.height;
        }
    }
    almost = (mask & ~(CWWidth | CWHeight | XtCWQueryOnly));
    almost |= GetRequestInfo(request, !vert) != GetRequestInfo(reply, !vert);
    almost |= GetRequestInfo(request, vert) != GetRequestInfo(reply, vert);

    if ( (mask & XtCWQueryOnly) || almost ) {
	pane->wp_size = old_wpsize;
	pane->wp_off_size = old_wp_off_size;
	pane->size = old_size;
	RefigureLocations(pw, PaneIndex(w), AnyPane);
	reply->request_mode = CWWidth | CWHeight;
	if (almost)
	    return XtGeometryAlmost;
    }
    else {
        (void) AdjustPanedSize(pw, GetRequestInfo(reply, !vert),
			       True, False, NULL, NULL);
	CommitNewLocations(pw, w);	/* layout already refigured. */
    }
    return XtGeometryYes;
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

static void
Initialize(Widget request, Widget set, ArgList args, Cardinal * num_args)
{
    XmPanedWidget pw = (XmPanedWidget)set;

    GetGCs( (Widget) pw);

    XmPaned_recursively_called(pw) = False;
    XmPaned_stack(pw) = NULL;
    XmPaned_resize_children_to_pref(pw) = TRUE;
    XmPaned_num_panes(pw) = 0;
    XmPaned_increment_count(pw) = 0;
    XmPaned_repane_status(pw) = NO_ADJUST;
    XmPaned_num_slots(pw) = 0;
    XmPaned_managed_children(pw) = NULL;

    /*
     * Undo the size change that XmManager did to us!!!
     */

    pw->core.width = request->core.width;
    pw->core.height = request->core.height;

    if (!XmPaned_sash_width(pw))
        XmPaned_sash_width(pw) = (int)(4 + 6 * XmScreenDpi(XmScreenOfObject(pw)) / 96.);

    if (!XmPaned_sash_height(pw))
        XmPaned_sash_height(pw) = (int)(4 + 6 * XmScreenDpi(XmScreenOfObject(pw)) / 96.);

    XmPaned_sash_cursor(pw) = XCreateFontCursor(
        XtDisplay(pw),
        IsVert(pw) ? XC_sb_v_double_arrow : XC_sb_h_double_arrow
    );
}

/*	Function Name: Realize
 *	Description:   Called to realize this widget.
 *	Arguments:     w - Widget to realize.
 *                     valueMask, attributes - attributes to use when creating
 *                     this widget's window.
 *	Returns:       none.
 *
 * This overrides the Manager's frobbing with various values.
 */

static void
Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
    XmPanedWidget pw = (XmPanedWidget) w;
    Widget * childP;

    if (w->core.height == 0)
	w->core.height = 1;

    if (w->core.width == 0)
	w->core.width = 1;

    /*
     * Set the cursor on the paned window.
     */

    if ((attributes->cursor = XmPaned_cursor(pw)) != None)
	*valueMask |= CWCursor;

    XtCreateWindow (w, InputOutput, CopyFromParent, *valueMask, attributes);

    /*
     * Before we commit the new locations we need to realize all the panes and
     * their sashs.
     */

    for ( childP = XmPaned_managed_children(pw) ;
	  childP < XmPaned_managed_children(pw) + XmPaned_num_panes(pw);
	  childP++ ) {
        XtRealizeWidget( *childP );
	if (HasSash (*childP)) {
	    XtRealizeWidget( PaneInfo(*childP)->sash );
	}
	if (HasSep(*childP)) {
	    XtRealizeWidget( PaneInfo(*childP)->separator );
	}
    }

    RefigureLocationsAndCommit(w);
    XmPaned_resize_children_to_pref(pw) = FALSE;
} /* Realize */

/*	Function Name: Destroy
 *	Description: Called when the widget is destroyed, cleans up.
 *	Arguments: w - the widget.
 *	Returns: none.
 */

static void Destroy(Widget w)
{
    XmPanedWidget pw = (XmPanedWidget)w;

    ReleaseGCs(w);
    ClearPaneStack(pw);
    XtFree((XtPointer)XmPaned_managed_children(pw));
    XFreeCursor(XtDisplay(w), XmPaned_sash_cursor(pw));
}

/*	Function Name: InsertChild
 *	Description: called when a new child has been added to the paned window
 *	Arguments: w - the new child.
 *	Returns: none.
 */

static void
InsertChild(Widget w)
{
   XmPanedWidget pw = (XmPanedWidget) XtParent(w);
   Pane pane = PaneInfo(w);
   Arg	arglist[10];
   int	num_args;

   if (_XmGadgetWarning(w))
       return;

   /*
    * Insert the child widget in the composite children list with the
    * superclass insert_child routine.
    */

    {
        XtWidgetProc insert_child;

        _XmProcessLock();
        insert_child = SuperClass->composite_class.insert_child;
        _XmProcessUnlock();

        (*insert_child)(w);
    }

   /* These should be in Constraint init... */

   pane->sash = NULL;
   pane->separator = NULL;
   pane->prefs_inited = False;

   if (!IsPane(w))
       return;

   if(pane->min == pane->max)   /* Motif behavior */
       pane->show_sash = False;

   /* A basic Sanity Check */
   if (pane->min > pane->max)
   {

        /*
         * This error will cause a crash later on
         * lets make this error know to the programmer
         * right off the bat, this will (should) never
         * be "discovered" later after the thing has been used several
         * times.
         */
        fprintf(stderr, "XiError: XmPaned Widget resource conflict\n");
        fprintf(stderr, "XmNpaneMax is less than XmNpaneMin.\n");
        fprintf(stderr, "XmNpaneMax = %d XmNpaneMin = %d\n\n", pane->max, pane->min);
        exit(EXIT_FAILURE);
   }

   /* Panes will be added in the order they are created, for now... */

   if (pane->show_sash)
       CreateSash(w);

   if (XmPaned_separator_on(pw))
       CreateSeparator(w);

   pane->size = 0;

   /* Motif navigation stuff */
   num_args = 0;
   XtSetArg(arglist[num_args], XmNnavigationType, XmTAB_GROUP);
   num_args++;
   XtSetValues(w, arglist, num_args);

} /* InsertChild */

/*	Function Name: ConstraintDestroy
 *	Description: Called to clean up after child with constraints is
 *                   destroyed.
 *	Arguments: w - the child that has been destroyed.
 *	Returns: none.
 */

static void
ConstraintDestroy(Widget w)
{
    /* Remove the subwidget info and destroy the sash */

    if (IsPane(w)) {
	if(HasSash(w))
	    XtDestroyWidget(PaneInfo(w)->sash);
	if(HasSep(w))
	    XtDestroyWidget(PaneInfo(w)->separator);
    }
} /* DeleteChild */

/**********************************************************************
 *
 *  ReManageChildren
 *	This procedure will be called by the ChangeManged procedure
 *	It will reassemble the currently managed children into the
 *      "paned_window.managed_children" list.
 *
 ********************************************************************/

static void
ReManageChildren(XmPanedWidget pw)
{
    int i;

    XmPaned_num_panes(pw) = 0;
    for (i = 0; i < pw->composite.num_children; i++) {
	if(XtIsManaged(pw->composite.children[i]) &&
           IsPane(pw->composite.children[i]) )
	{
	    Pane pane = PaneInfo(pw->composite.children[i]);

	    /* expand our storage area if needed */
	    if ((XmPaned_num_panes(pw) + 1) > XmPaned_num_slots(pw)) {
		XmPaned_num_slots(pw) += BLOCK;
		XmPaned_managed_children(pw) = (WidgetList)
			XtRealloc ((XtPointer) XmPaned_managed_children(pw),
				(XmPaned_num_slots(pw) * sizeof(Widget)));
	    }

#ifndef POSITION_IMPLEMENTED
	    /*
	     * This is a hack to get around position change not
	     * being implemented yet.
	     */

	    if (HasSash(pw->composite.children[i]))
		PaneInfo(pane->sash)->position = XmPaned_num_panes(pw);

	    pane->position = XmPaned_num_panes(pw);
#endif

	    XmPaned_managed_children(pw)[XmPaned_num_panes(pw)++] =
		pw->composite.children[i];
	}
    }
}

/*	Function Name: ChangeManaged
 *	Description: Called when one of the children changes management state.
 *	Arguments: w - the paned widget.
 *	Returns: none.
 */

static void
ChangeManaged(Widget w)
{
   XmPanedWidget pw = (XmPanedWidget)w;

   if (XmPaned_recursively_called(pw)++) return;

   ManageAndUnmanageSashs(pw);
   XmPaned_recursively_called(pw) = False;
   ReManageChildren(pw);

   /*
    * ForAllPanes can now be used.
    */

   ResetSize(pw, PaneSize(w, IsVert(pw)) <= 1);

   /* for Motif navigation */
   XmeNavigChangeManaged((Widget)pw);
} /* ChangeManaged */

/*	Function Name: ResetSize
 *	Description: Resets the size of the paned widget to its preferred.
 *                   Checks are made on all children to attempt to size
 *                   then to the preferred.
 *	Arguments: pw - the paned widget.
 *                 recalc_off_size - If this is true the do not
 *                                   use the current off size, recalc for
 *                                   a new value.
 *	Returns: none.
 */

static void
ResetSize(XmPanedWidget pw, Boolean recalc_off_size)
{
   Boolean test, vert = IsVert(pw);
   Dimension off_size;

   /*
    *  Must be called once before GetPrefSizes.
    */

   test = !XtIsRealized((Widget) pw) || recalc_off_size;
   SetChildrenPrefSizes(pw, 0, False, test);

   if (recalc_off_size || XtIsRealized((Widget) pw)) {
       GetPrefSizes(pw, NULL, &off_size);
   }
   else {
       off_size = PaneSize( (Widget) pw, !vert );

       if (vert)
	   off_size -= 2 * XmPaned_margin_width(pw);
       else
	   off_size -= 2 * XmPaned_margin_height(pw);
   }

   /*
    * This gets the real off size our parent will allow, may be
    * quite different than what we want.
    */

   if (XtIsRealized((Widget) pw))
   {
       (void) AdjustPanedSize(pw, off_size, True, True, NULL, &off_size);
   }

   /*
    * Given the new off size, ask our children what size they would like
    * to be, and reconfigure ourself to the new size.
    */

   SetChildrenPrefSizes(pw, off_size, True, False);
   (void) AdjustPanedSize(pw, off_size,
			  XtIsRealized((Widget) pw), False, NULL, NULL);
   RefigureLocationsAndCommit((Widget) pw);
}

/*	Function Name: Resize
 *	Description: The paned widget's resize proc.
 *	Arguments: w - the paned widget.
 *	Returns: none.
 */

static void

Resize(Widget w)
{
    XmPanedWidget pw = (XmPanedWidget)w;
    int size;

    size = PaneSize(w, !IsVert(pw));

    if (IsVert(pw))
	size -= 2 * XmPaned_margin_width(pw);
    else
	size -= 2 * XmPaned_margin_height(pw);

    SetChildrenPrefSizes((XmPanedWidget) w, size, True, False);

    if (!XtIsRealized(w))
	return;

    RefigureLocationsAndCommit(w);
}

/*	Function Name: SetValues
 *	Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *	Arguments:     old - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - The arguments passed to the set
 *                                      values call.
 *	Returns:       none
 */

static Boolean
SetValues(Widget old, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    XmPanedWidget old_pw = (XmPanedWidget) old;
    XmPanedWidget set_pw = (XmPanedWidget) set;
    int num_panes = XmPaned_num_panes(set_pw);
    Widget *childP;
    Boolean refigure = False, commit = False;
    Arg sargs[3];
    int num_sargs = 0;

    /*
     * Manager is getting in the way, fix it.
     */

    if (request->core.height == 0)
	set->core.height = request->core.height;

    if (request->core.width == 0)
	set->core.width = request->core.width;

    if (request->core.border_width == 0)
	set->core.border_width = request->core.border_width;

    if ((XmPaned_cursor(old_pw) != XmPaned_cursor(set_pw)) && XtIsRealized(set))
        XDefineCursor(XtDisplay(set), XtWindow(set), XmPaned_cursor(set_pw));

    if ((old_pw->manager.foreground != set_pw->manager.foreground) ||
	(old_pw->core.background_pixel != set_pw->core.background_pixel))
    {
        ReleaseGCs(old);
	GetGCs(set);
    }

    /* If we change sep status we must do for all seps */

    if (XmPaned_separator_on(old_pw) != XmPaned_separator_on(set_pw))
    {
	if (XmPaned_separator_on(set_pw)) {
	    WidgetList sep_children;
	    Cardinal num_separators = 0;

	    /* This should be more than enough space */
	    sep_children = (WidgetList) XtMalloc(num_panes * sizeof(Widget));

	    for ( childP = XmPaned_managed_children(set_pw) ;
		  childP < XmPaned_managed_children(set_pw)
		      + XmPaned_num_panes(set_pw);
		  childP++ ) {
		CreateSeparator(*childP);
		sep_children[num_separators] = PaneInfo(*childP)->separator;
		num_separators++;
	    }

	    XtManageChildren((WidgetList) sep_children, num_separators);
	    XtFree((char *)sep_children);
	    commit = True;
	}
	else {
	    for ( childP = XmPaned_managed_children(set_pw) ;
		  childP < XmPaned_managed_children(set_pw)
		      + XmPaned_num_panes(set_pw);
		  childP++ ) {
		Pane pane = PaneInfo(*childP);
		if (pane->separator != NULL) {
		    XtDestroyWidget(pane->separator);
		    pane->separator = NULL;
		}
	    }
	}
    }

    if (IsVert(old_pw) != IsVert(set_pw)) {
	Cardinal num_sep_args;
	Arg sep_args[10];

	num_sep_args = 0;
	if (IsVert(set_pw))
	    XtSetArg(sep_args[num_sep_args], XmNorientation, XmHORIZONTAL);
	else
	    XtSetArg(sep_args[num_sep_args], XmNorientation, XmVERTICAL);

	num_sep_args++;

	ForAllChildren(set_pw, childP) {
	    Pane pane = PaneInfo(*childP);
	    if (pane->separator != NULL) {
		XtSetValues(pane->separator, sep_args, num_sep_args);
	    }
	}

	XmPaned_resize_children_to_pref(set_pw) = TRUE;
	ResetSize(set_pw, TRUE);
	XmPaned_resize_children_to_pref(set_pw) = FALSE;

	return(TRUE);
    }

    /* Build an arg list for global changes to the sashs */
    if (XmPaned_sash_cursor(old_pw) != XmPaned_sash_cursor(set_pw)) {
	XtSetArg(sargs[num_sargs], XmNcursor, XmPaned_sash_cursor(set_pw));
	num_sargs++;
	refigure = True;
    }

    if (XmPaned_sash_width(old_pw) != XmPaned_sash_width(set_pw)) {
	XtSetArg(sargs[num_sargs], XmNwidth, XmPaned_sash_width(set_pw));
	num_sargs++;
	refigure = True;
    }

    if (XmPaned_sash_height(old_pw) != XmPaned_sash_height(set_pw)) {
	XtSetArg(sargs[num_sargs], XmNheight, XmPaned_sash_height(set_pw));
	num_sargs++;
	refigure = True;
    }

    if (XmPaned_sash_shadow_thickness(old_pw) !=
	XmPaned_sash_shadow_thickness(set_pw))
    {
	XtSetArg(sargs[num_sargs], XmNshadowThickness,
		 XmPaned_sash_shadow_thickness(set_pw)); num_sargs++;
    }

    if (XmPaned_sash_translations(old_pw) != XmPaned_sash_translations(set_pw))
    {
	XmeWarning(set, XmNstaticTranslationsMsg);

	XmPaned_sash_translations(set_pw) = XmPaned_sash_translations(old_pw);
    }

    if (num_sargs != 0) {
	for ( childP = XmPaned_managed_children(set_pw) ;
	      childP < XmPaned_managed_children(set_pw)
		  + XmPaned_num_panes(set_pw);
	      childP++ )
	    if (HasSash(*childP))
		XtSetValues(PaneInfo(*childP)->sash, sargs, num_sargs);

	refigure = True;
    }

    if ((XmPaned_internal_bw(old_pw) != XmPaned_internal_bw(set_pw)) ||
	(XmPaned_margin_width(old_pw) != XmPaned_margin_width(set_pw)) ||
	(XmPaned_margin_height(old_pw)!= XmPaned_margin_height(set_pw)))
    {
	refigure = True;
    }

    if ( (XmPaned_sash_indent(old_pw) != XmPaned_sash_indent(set_pw)) &&
	(XtIsRealized(set)) )
    {
	commit = True;
    }

    if (refigure) {
	Dimension off_size = PaneSize((Widget) old_pw, !IsVert(old_pw));

	if (IsVert(old_pw))
	    off_size -=	2 * XmPaned_margin_width(old_pw);
	else
	    off_size -= 2 * XmPaned_margin_height(old_pw);

	(void) AdjustPanedSize(set_pw, off_size, True, False, NULL, NULL);

	RefigureLocations(set_pw, NO_INDEX, AnyPane);
	commit = True;
    }

    if (commit)
	CommitNewLocations(set_pw, NULL);


    return (False);
} /* SetValues */

/*      Function Name: QueryGeometry
 *      Description:   Called when my parent wants to know my preferred size.
 *      Arguments:     w - the widget to check.
 *                     intended - parent imposed geometry.
 *                     preferred - what I would like.
 *      Returns:       status.
 */

static XtGeometryResult
QueryGeometry(Widget w, XtWidgetGeometry *request,
              XtWidgetGeometry *preferred)
{
    XmPanedWidget pw = (XmPanedWidget) w;
    Dimension on_size, off_size;

    GetPrefSizes(pw, &on_size, &off_size);

    if (IsVert(pw)) {
	preferred->width = off_size + 2 * XmPaned_margin_width(pw);
	preferred->height = on_size + 2 * XmPaned_margin_height(pw);
    }
    else {
	preferred->width = on_size   + 2 * XmPaned_margin_width(pw);
	preferred->height = off_size + 2 * XmPaned_margin_height(pw);
    }

    return(_XmHWQuery(w, request, preferred));
}

/*	Function Name: PaneSetValues
 *	Description:   Called when some constraint data needs to be modified
 *                     on-the-fly.
 *	Arguments:     old - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     new - what will acutally be the new values.
 *                     args, num_args - The arguments passed to
 *                                      the child's setvalues call.
 *	Returns:       none
 */

static Boolean
PaneSetValues(Widget old, Widget request, Widget new,
	      ArgList args, Cardinal * num_args)
{
    Pane old_pane = PaneInfo(old);
    Pane new_pane = PaneInfo(new);
    Boolean sash_reset = False;

    /* Check for new min and max. */

    if ((old_pane->min != new_pane->min) || (old_pane->max != new_pane->max)) {
	if (ForceSashOff(old_pane) != ForceSashOff(new_pane))
	    sash_reset = True;
    }

    if (old_pane->show_sash != new_pane->show_sash)
	sash_reset = True;

    if (sash_reset)
	ResetSash(new);

    return(False);
}

/************************************************************
 *
 * Public routines.
 *
 ************************************************************/

/*	Function Name: XmPanedGetPanes
 *	Description: Returns the number of panes in the paned widget.
 *	Arguments: w - the paned widget.
 *                 panes - the list of all panes contained in this widget.
 *                 num - the number of panes.
 *	Returns: the number of panes in the paned widget.
 */

int
XmPanedGetPanes(Widget w, WidgetList *panes, int *num)
{
    XmPanedWidget pw = (XmPanedWidget) w;

    _XmWidgetToAppContext(w);
    _XmAppLock(app);

    *num = XmPaned_num_panes(pw);
    *panes = XmPaned_managed_children(pw);

    _XmAppUnlock(app);

    return(*num);
}

/*	Function Name: XmCreatePaned
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreatePaned(Widget parent, String name, ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmPanedWidgetClass, parent, args, num_args));
}

