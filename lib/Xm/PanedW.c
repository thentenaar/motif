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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: PanedW.c /main/24 1999/07/13 07:46:01 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <ctype.h>
#include <X11/cursorfont.h>
#include "XmI.h"
#include <Xm/PanedWP.h>
#include <Xm/SeparatoG.h>
#include <Xm/SashP.h>
#include <Xm/VaSimpleP.h>
#include "MessagesI.h"
#include "RepTypeI.h"

typedef enum {FirstPane='U', LastPane='L'} Direction;

#define MESSAGE4	_XmMMsgPanedW_0000
#define MESSAGE5	_XmMMsgPanedW_0001
#define MESSAGE6	_XmMMsgPanedW_0002
#define MESSAGE8	_XmMMsgPanedW_0004
#define MESSAGE9	_XmMMsgPanedW_0005

#define PaneInfo(w)	((XmPanedWindowConstraintPtr)(w)->core.constraints)
#define IsPane(w)	(PaneInfo(w)->panedw.isPane)
#define PaneIndex(w)	(PaneInfo(w)->panedw.position)
#define PanePosIndex(w)	(PaneInfo(w)->panedw.position_index)

/*****************************************************************/
/* Code used for carrying the orientation code is a clean way.
   Do not manipulate width, height, or x and y anymore, but
   a major and minor dimension and an independant position */

#define Horizontal(pw) ((pw)->paned_window.orientation == XmHORIZONTAL)


/* since we cannot change the instance record field name, for binary
   compatibility,  we have to macro those as well */
#define PaneDMajor(w)	(PaneInfo(w)->panedw.dheight)
#define PaneDPos(w)	(PaneInfo(w)->panedw.dy)
#define PaneOldDPos(w)	(PaneInfo(w)->panedw.olddy)

#define PaneStartPos(w)	(w->paned_window.starty)
#define PaneFirst(w)	(w->paned_window.top_pane)
#define PaneLast(w)	(w->paned_window.bottom_pane)
#define PaneFirstDMajor(w) (w->paned_window.top_pane->panedw.dheight)
#define PaneLastDMajor(w)  (w->paned_window.bottom_pane->panedw.dheight)


/* then come all the new paradigm of major/minor dimension */

#define Major(pw, w, h) ((Horizontal(pw))?w:h)
#define Minor(pw, w, h) (Major(pw,h,w))

#define MajorMargin(pw) (Major(pw, (pw)->paned_window.margin_width,\
			    (pw)->paned_window.margin_height))
#define MinorMargin(pw) (Major(pw, (pw)->paned_window.margin_height,\
			    (pw)->paned_window.margin_width))

#define MajorSize(pw) (Major(pw, (pw)->core.width, (pw)->core.height))
#define MinorSize(pw) (Major(pw, (pw)->core.height, (pw)->core.width))

#define MajorChildSize(pw, child) (Major(pw, \
                       (child)->core.width, (child)->core.height))
#define MinorChildSize(pw, child) (Major(pw, \
                       (child)->core.height, (child)->core.width))

#define MajorChildPos(pw, child) (Major(pw, (child)->core.x, (child)->core.y))

#define MajorReq(pw, request) (Major(pw, (request)->width, (request)->height))
#define MinorReq(pw, request) (Major(pw, (request)->height, (request)->width))

#define MajorAssign(pw,x,v) if (Horizontal(pw)) (x).width = v; else (x).height = v
#define MinorAssign(pw,x,v) if (Horizontal(pw)) (x).height = v; else (x).width = v


/*****************************************************************/



#define XmBLOCK	10  /* used for the list of managed children */

#define DEFAULT_SASH_INDENT_LTOR       -10
#define DEFAULT_SASH_INDENT_RTOL        10

/* Enumerated values used for HandleSash action, whose parameters
   are processing using reptype. */
enum { _START, _MOVE, _COMMIT, _KEY };
enum { _DEFAULT_INCR, _LARGE_INCR };
enum { _UP, _DOWN, _RIGHT, _LEFT, _FIRST, _LAST };

/********    Static Function Declarations    ********/

static void ReManageChildren(
                        XmPanedWindowWidget pw) ;
static int NeedsAdjusting(
                        register XmPanedWindowWidget pw) ;
static XtGeometryResult AdjustPanedWindowMajor(
                        XmPanedWindowWidget pw,
                        Dimension newdim,
                        Dimension *reply_dim) ;
static void ResetDMajors(
                        XmPanedWindowWidget pw) ;
static void RefigureLocations(
                        register XmPanedWindowWidget pw,
                        int c_index,
                        Direction dir,
                        Boolean rflag,
                        Boolean sflag) ;
static void CommitNewLocations(
                        XmPanedWindowWidget pw,
                        Widget instigator) ;
static void RefigureLocationsAndCommit(
                        XmPanedWindowWidget pw,
                        int c_index,
                        Direction dir,
                        Boolean rflag) ;
static void DrawTrackLines(
                        XmPanedWindowWidget pw) ;
static void EraseTrackLines(
                        XmPanedWindowWidget pw) ;
static void ProcessKeyEvent(
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void HandleSash(
                        Widget w,
                        XtPointer closure,
                        XtPointer callData) ;
static XtGeometryResult GeometryManager(
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void SashIndentDefault(Widget widget,
			      int offset,
			      XrmValue *value );
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void Initialize(
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void ConstraintInit(
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Realize(
                        Widget w,
                        XtValueMask *p_valueMask,
                        XSetWindowAttributes *attributes) ;
static void Destroy(
                        Widget w) ;
static Cardinal InsertOrder(
                        Widget w) ;
static void InsertChild(
                        register Widget w) ;
static void ChangeManaged(
                        Widget w) ;
static void Resize(
                        Widget wid) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean PaneSetValues(
                        Widget old,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void ConstraintDestroy(
                        Widget w) ;
static void AdjustGC(
                        XmPanedWindowWidget pw) ;
static void GetFlipGC(
                        XmPanedWindowWidget pw) ;

/********    End Static Function Declarations    ********/


/****************************************************************
 *
 * Paned Window Resources
 *
 ****************************************************************/
#define Offset(field) XtOffsetOf( struct _XmPanedWindowRec, paned_window.field)

static XtResource resources[] = {
    {XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
       Offset(margin_width), XmRImmediate, (XtPointer) 3},

    {XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, sizeof(Dimension),
       Offset(margin_height), XmRImmediate, (XtPointer) 3},

/* need to change the XmR to something dynamic: XmRPanedWMajorDimension,
   that checks eh orientation first */
    {XmNspacing, XmCSpacing, XmRVerticalDimension, sizeof(Dimension),
       Offset(spacing), XmRImmediate, (XtPointer) 8},

    {XmNrefigureMode, XmCBoolean, XmRBoolean, sizeof(Boolean),
       Offset(refiguremode), XmRImmediate, (XtPointer) TRUE},

    {XmNseparatorOn, XmCSeparatorOn, XmRBoolean, sizeof(Boolean),
       Offset(separator_on), XmRImmediate, (XtPointer) TRUE},

/* need to change the XmR to something dynamic XmRPanedWMinorDimension */
    {XmNsashIndent, XmCSashIndent, XmRHorizontalPosition, sizeof(Position),
       Offset(sash_indent), XmRCallProc, (XtPointer) SashIndentDefault},

    {XmNsashWidth, XmCSashWidth, XmRHorizontalDimension, sizeof(Dimension),
       Offset(sash_width), XmRImmediate, (XtPointer) 10},

    {XmNsashHeight, XmCSashHeight, XmRVerticalDimension, sizeof(Dimension),
       Offset(sash_height), XmRImmediate, (XtPointer) 10},

    {XmNsashShadowThickness, XmCShadowThickness, XmRHorizontalDimension,
       sizeof(Dimension), Offset(sash_shadow_thickness), XmRCallProc,
       (XtPointer) _XmSetThickness},

    {XtNinsertPosition, XtCInsertPosition, XtRFunction, sizeof(XtOrderProc),
       XtOffsetOf(XmPanedWindowRec, composite.insert_position),
       XtRImmediate, (XtPointer) InsertOrder},

    {XmNorientation, XmCOrientation, XmROrientation,
       sizeof(unsigned char),  Offset(orientation),
       XmRImmediate, (XtPointer) XmVERTICAL
    },
};

/* Definition for resources that need special processing in get values */

static XmSyntheticResource get_resources[] =
{
   { XmNmarginWidth,
     sizeof(Dimension),
     Offset(margin_width),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels
   },

   { XmNmarginHeight,
     sizeof(Dimension),
     Offset(margin_height),
     XmeFromVerticalPixels,
     XmeToVerticalPixels
   },

   { XmNspacing,
     sizeof(Dimension),
     Offset(spacing),
/* need to change to something dynamic FromPanedWMajorPixels, and
   ToPanedWMajorPixels, that check orientation first  */
     XmeFromVerticalPixels,
     XmeToVerticalPixels
   },

   { XmNsashIndent,
     sizeof(Position),
     Offset(sash_indent),
/* need to change to FromPanedWMinorPixels, and  ToPanedWMinorPixels */
     XmeFromVerticalPixels,
     XmeToVerticalPixels
   },

   { XmNsashWidth,
     sizeof(Dimension),
     Offset(sash_width),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels
   },

   { XmNsashHeight,
     sizeof(Dimension),
     Offset(sash_height),
     XmeFromVerticalPixels,
     XmeToVerticalPixels
   },

   { XmNsashShadowThickness,
     sizeof(Dimension),
     Offset(sash_shadow_thickness),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels
   },
};

#undef Offset



/****************************************************************
 *
 * Paned Window Constraint Resources For Its Constraint Record
 *
 ****************************************************************/

#define Offset(field) XtOffsetOf( struct _XmPanedWindowConstraintRec, panedw.field)

static XtResource constraint_resources[] = {
    {XmNallowResize, XmCBoolean, XmRBoolean, sizeof(Boolean),
	 Offset(allow_resize), XmRImmediate, (XtPointer) FALSE},
/* need to change the XmR to XmRPanedWMajorDimension */
    {XmNpaneMinimum, XmCPaneMinimum, XmRVerticalDimension, sizeof(Dimension),
         Offset(min), XmRImmediate, (XtPointer) 1},
    {XmNpaneMaximum, XmCPaneMaximum, XmRVerticalDimension, sizeof(Dimension),
         Offset(max), XmRImmediate, (XtPointer) 1000},

    {XmNskipAdjust, XmCBoolean, XmRBoolean, sizeof(Boolean),
         Offset(skip_adjust), XmRImmediate, (XtPointer) FALSE},
    {XmNpositionIndex, XmCPositionIndex, XmRShort, sizeof(short),
         Offset(position_index), XmRImmediate, (XtPointer) XmLAST_POSITION},
};

/* Definition for constraint resources that need special */
/* processing in get values                              */

static XmSyntheticResource get_constraint_resources[] =
{
    {  XmNpaneMinimum,
       sizeof(Dimension),
       Offset(min),
/* need to change to FromPanedWMajorPixels and ToPanedWMajorPixels */
       XmeFromVerticalPixels,
       XmeToVerticalPixels
    },

    {  XmNpaneMaximum,
       sizeof(Dimension),
       Offset(max),
/* need to change to FromPanedWMajorPixels and ToPanedWMajorPixels */
       XmeFromVerticalPixels,
       XmeToVerticalPixels
    },

};

#undef Offset


/*************************************<->*************************************
 *
 *
 *   Description:  PanedWindow full class record
 *   -----------
 *************************************<->***********************************/

externaldef(xmpanedwindowclassrec) XmPanedWindowClassRec xmPanedWindowClassRec =
{
   {	 					/* core class fields   */
      (WidgetClass) &xmManagerClassRec,		/* superclass          */
      "XmPanedWindow",				/* class name          */
      sizeof(XmPanedWindowRec),			/* size                */
      NULL,					/* class initialize    */
      ClassPartInitialize,			/* class_part_inite    */
      FALSE,					/* class_inited        */
      Initialize,				/* initialize          */
      NULL,					/* initialize_hook     */
      Realize,					/* realize             */
      NULL,          				/* actions             */
      0,                			/* num_actions         */
      resources,				/* resourses           */
      XtNumber(resources),			/* resource_count      */
      NULLQUARK,				/* xrm_class           */
      TRUE,					/* compress_motion     */
      XtExposeCompressMaximal,			/* compress_exposure   */
      TRUE,					/* compress_enter/lv   */
      FALSE,					/* visible_interest    */
      Destroy,					/* destroy             */
      Resize,					/* resize              */
      XmeRedisplayGadgets,			/* expose              */
      SetValues,				/* set_values          */
      NULL,					/* set_values_hook     */
      XtInheritSetValuesAlmost,			/* set_values_almost   */
      NULL,					/* get_values_hook     */
      NULL,					/* accept_focus        */
      XtVersion,				/* version             */
      NULL, 					/* callback_private    */
      XtInheritTranslations,			/* tm_table            */
      NULL,					/* Query Geometry proc */
      NULL,					/* display accelerator */
      NULL,					/* extension           */
   },

   {						/* composite class fields */
      GeometryManager,				/* geometry_manager       */
      ChangeManaged,				/* change_managed         */
      InsertChild,				/* insert_child           */
      XtInheritDeleteChild,			/* delete_child           */
      NULL,					/* extension              */
   },

   {						/* constraint class fields */
      constraint_resources,			/* subresourses            */
      XtNumber(constraint_resources),		/* subresource_count       */
      sizeof(XmPanedWindowConstraintRec),	/* constraint_size         */
      ConstraintInit,				/* initialize              */
      ConstraintDestroy,			/* destroy                 */
      PaneSetValues,				/* set_values              */
      NULL,					/* extension               */
  },

  {						/* manager_class fields   */
      XtInheritTranslations,	 		/* translations      	  */
      get_resources,				/* get resources      	  */
      XtNumber(get_resources),			/* num get_resources 	  */
      get_constraint_resources,			/* get_cont_resources     */
      XtNumber(get_constraint_resources),	/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL, 					/* extension              */
  },

  {						/* paned_window_class fields */
      NULL, 					/* extension                 */
  }
};

externaldef(xmpanedwindowwidgetclass) WidgetClass xmPanedWindowWidgetClass =
					   (WidgetClass) &xmPanedWindowClassRec;

/************************************************************************
 *
 *  SashIndentDefault
 *     Set up the default sash indentation
 *
 ************************************************************************/
static void
SashIndentDefault(Widget widget,
		  int offset,	/* unused */
		  XrmValue *value )
{
  static Position indent;

  value->addr = (XPointer) &indent;

  if (LayoutIsRtoLM(widget))
    indent = (Position) DEFAULT_SASH_INDENT_RTOL;
  else
    indent = (Position) DEFAULT_SASH_INDENT_LTOR;
}


/************************************************************************
 *
 *  ClassPartInitialize
 *     Set up the fast subclassing for the widget.
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmPANED_WINDOW_BIT);
}



/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
static void
Initialize(
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmPanedWindowWidget pw = (XmPanedWindowWidget) new_w;

 /* Protect against empty widgets */
  pw->paned_window.pane_count = 0;
  pw->paned_window.managed_children =
                          (WidgetList) XtMalloc(XmBLOCK*sizeof(Widget));
  pw->paned_window.num_slots = XmBLOCK;
  pw->paned_window.num_managed_children = 0;
  PaneStartPos(pw) = 0;
  PaneFirst(pw) = NULL;
  PaneLast(pw) = NULL;
  pw->paned_window.flipgc = NULL;
  pw->paned_window.increment_count = 0;
  pw->paned_window.resize_at_realize = True;
  pw->paned_window.timer = 0 ;

/*************************************************************
 * NOTE: that sash_indent is made to conform to a correct size
 * during changed managed time/layout time.  Since the size of
 * the window may change we won't require that
 * abs(sash_indent) <= width of a pane.
 ************************************************************/

  pw -> paned_window.recursively_called = FALSE;

  /* DON'T ALLOW HEIGHT/WIDTH TO BE 0, OR X DOESN'T HANDLE THIS WELL */

  if (pw->core.width == 0) pw->core.width = 10;
  if (pw->core.height == 0) pw->core.height = 10;

  /* Check orientation value */
  if(!XmRepTypeValidValue(XmRID_ORIENTATION,
			  pw->paned_window.orientation, (Widget) pw)) {
      pw->paned_window.orientation = XmVERTICAL;
  }

}


/*************************************<->*************************************
 *
 *  Realize
 *
 *   Description:
 *   -----------
 *    Create our window, set NW gravity (Realize), set up
 *
 *
 *************************************<->***********************************/
static void
Realize(
        Widget w,
        XtValueMask *p_valueMask,
        XSetWindowAttributes *attributes )
{
    register XmPanedWindowWidget pw = (XmPanedWindowWidget)w;
    WidgetList children = pw->paned_window.managed_children;
    int num_children = pw->paned_window.num_managed_children;
    Widget *childP;
    Mask valueMask = *p_valueMask;

    valueMask |= CWBitGravity | CWDontPropagate;
    attributes->bit_gravity = NorthWestGravity;
    attributes->do_not_propagate_mask = ButtonPressMask|
	ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask;

    XtCreateWindow (w, InputOutput, CopyFromParent, valueMask, attributes);

    GetFlipGC(pw);

    /* one last time, in case we grew to try to return an
     * XtGeometryAlmost for a child, but the child decided not to grow
     * or in case some child grew itself and we didn't hear about it.
     */
    if (pw->paned_window.resize_at_realize) {
	XtWidgetProc resize;

	_XmProcessLock();
	resize = pw->core.widget_class->core_class.resize;
	_XmProcessUnlock();

	(*resize)( (Widget) pw );
    }

    ReManageChildren(pw);

    children = pw->paned_window.managed_children;
    num_children = pw->paned_window.num_managed_children;

    /* now we have to make sure all the sashs are on above their
     * panes, which means that we have to realize all our children
     * here and now.  If we realize from the beginning of the list,
     * then the sashs (which are at the end) will be Above by default. */

    for (childP = children; childP - children < num_children; childP++)
	    XtRealizeWidget( *childP );

} /* Realize */


/*************************************<->*************************************
 *
 *  Destroy
 *
 *************************************<->***********************************/
static void
Destroy(
        Widget w )
{
    XmPanedWindowWidget pw = (XmPanedWindowWidget)w;

    if (pw->paned_window.timer)
	XtRemoveTimeOut(pw->paned_window.timer);

    if (pw->paned_window.flipgc != NULL) {
       XtReleaseGC(w, pw->paned_window.flipgc);
       pw->paned_window.flipgc = NULL;
    }

    XtFree( (char *) pw->paned_window.managed_children);

} /* Destroy */


/*************************************<->*************************************
 *
 *  Resize
 *
 *************************************<->***********************************/
static void
Resize(
        Widget wid )
{
    XmPanedWindowWidget pw = (XmPanedWindowWidget) wid ;
    RefigureLocationsAndCommit( pw, pw->paned_window.pane_count - 1,
			       LastPane, True);
} /* Resize */



/*************************************<->*************************************
 *
 *  ConstraintDestroy
 *
 *   Description:
 *   -----------
 *    Destroy the sash of any pane which is being destroyed.
 *
 *
 *************************************<->***********************************/
static void
ConstraintDestroy(
        Widget w )
{
   if (!XtIsRectObj(w)) return;

   /*
    * If this is an ordinary pane,  delete its sash (if it has one)
    * and separator (if it has one) and then invoke the standard
    * inherited delete child routines.
    */
   if (XtIsRectObj(w) && IsPane(w))
   {
        if (PaneInfo(w)->panedw.sash != NULL)
          XtDestroyWidget(PaneInfo(w)->panedw.sash);

        if (PaneInfo(w)->panedw.separator != NULL)
          XtDestroyWidget(PaneInfo(w)->panedw.separator);

 	/* the destroyed child has already been removed from the list of
 	** children, so update the other panes so that their positions match
 	** their positionIndex values (no need to be clever and update only
 	** those that change)
 	*/
 	{
 	XmPanedWindowWidget pw = (XmPanedWindowWidget)w->core.parent;
	if (!(pw->core.being_destroyed))
 	  {
	      int i;
	      for (i = 0;
		   (i < pw->composite.num_children)
		       && IsPane(pw->composite.children[i]);
		   i++)
		  PanePosIndex(pw->composite.children[i]) = i;
	  }
 	}

   }
}


/*************************************<->*************************************
 *
 *  void AdjustGC(pw)
 *          XmPanedWindowWidget  pw;
 *
 *   Description:
 *   -----------
 *   Set up the clip regions so the track lines do not draw on
 *   top of the sashes.
 *
 *************************************<->***********************************/
static void
AdjustGC(
        XmPanedWindowWidget pw )
{
    XRectangle clip_rect;
    register int i;
    Region sash_region, clip_region;
    if (pw->composite.num_children > 0) {
    	sash_region = XCreateRegion();
    	clip_region = XCreateRegion();

    	/* find all the managed sashes and add their area to the sash region */
    	for (i = 0; i < pw->composite.num_children; i++)
    	{
        	if (XmIsSash(pw->composite.children[i]) &&
            		XtIsManaged(pw->composite.children[i])) {
    	   	clip_rect.width = pw->composite.children[i]->core.width;
    	   	clip_rect.height = pw->composite.children[i]->core.height;
    	   	clip_rect.x = pw->composite.children[i]->core.x;
    	   	clip_rect.y = pw->composite.children[i]->core.y;
	   	XUnionRectWithRegion(&clip_rect, sash_region, sash_region);
        	}
    	}

    	/* set up the initial clip region */
    	clip_rect.width = pw->core.width;
    	clip_rect.height = pw->core.height;
    	clip_rect.x = 0;
    	clip_rect.y = 0;
    	XUnionRectWithRegion(&clip_rect, clip_region, clip_region);

    	/* remove the sash regions from the clip region */
    	XSubtractRegion(clip_region, sash_region, clip_region);

    	/* set the clip region, so the track
	   lines won't be draw on the sashes */
    	XSetRegion(XtDisplay(pw), pw->paned_window.flipgc, clip_region);

    	/* remove the clip regions */
    	XDestroyRegion(sash_region);
    	XDestroyRegion(clip_region);
   }
}


/*************************************<->*************************************
 *
 *  void GetFlipGC(pw)
 *          XmPanedWindowWidget  pw;
 *
 *   Description:
 *   -----------
 *   Create a GC which can be used to draw/erase track lines when the
 *   the size of the panes is being changed.
 *
 *************************************<->***********************************/
static void
GetFlipGC(
        XmPanedWindowWidget pw )
{
    unsigned long valuemask = GCForeground | GCSubwindowMode | GCFunction;
    unsigned long dynamicMask = GCClipMask;
    XGCValues	values;

    values.foreground = pw->core.background_pixel ^ pw->manager.foreground;
    values.subwindow_mode = IncludeInferiors;
    values.function = GXxor;
    pw->paned_window.flipgc = XtAllocateGC((Widget)pw, 0, valuemask, &values,
					   dynamicMask, 0);
}


/**********************************************************************
 *
 *  ReManageChildren
 *	This procedure will be called by the ChangeManged procedure
 *	It will reassemble the currently managed children into the
 *      "paned_window.managed_children" list.
 *  One day I think I'm gonna remove this stuff... dd
 ********************************************************************/
static void
ReManageChildren(
        XmPanedWindowWidget pw )
{
   int i;

   pw->paned_window.num_managed_children = 0;

   for (i = 0; i < pw->composite.num_children; i++)
   {
       if (XtIsManaged(pw->composite.children[i]))
       {
	    if ((pw->paned_window.num_managed_children+1) >
				 pw->paned_window.num_slots)
            {
	       pw->paned_window.num_slots += XmBLOCK;
	       pw->paned_window.managed_children = (WidgetList)
		  XtRealloc ((char *) pw->paned_window.managed_children,
			     (pw->paned_window.num_slots * sizeof(Widget)));
            }
	    pw->paned_window.managed_children
	            [pw->paned_window.num_managed_children++] =
		       pw->composite.children[i];
       }
   }
}

/*************************************<->*************************************
 *
 *  NeedsAdjusting (pw)
 *
 *   Description:
 *   -----------
 *     Calculate the major size needed to fully display this paned window.
 *
 *************************************<->***********************************/
static int
NeedsAdjusting(
        register XmPanedWindowWidget pw )
{
   int needed, i;

   needed = 0;
   for (i = 0; i < pw->paned_window.pane_count; i++) {
     needed += PaneDMajor(pw->paned_window.managed_children[i]) +
                 2 * pw->paned_window.managed_children[i]->core.border_width +
                 pw->paned_window.spacing;
   }

  /*
   * Get rid of extra spacing from previous 'for' loop and add in
   * major margin at the top and bottom of the panedw window
   */
   if (pw->paned_window.pane_count > 0)
       needed += 2 * MajorMargin(pw) - pw->paned_window.spacing;

   return (needed != MajorSize(pw)) ? needed : 0 ;
}


/*************************************<->*************************************
 *
 *  AdjustPanedWindowMajor
 *
 *   Description:
 *   -----------
 *     Request a new size for the paned window from its parent.
 *     If the requested new dim is less than 1, then ask for size of 1.
 *
 *************************************<->***********************************/
static XtGeometryResult
AdjustPanedWindowMajor(
        XmPanedWindowWidget pw,
        Dimension newdim,
        Dimension *reply_dim )
{
    Dimension replyWidth, replyHeight;
    XtGeometryResult result = XtGeometryNo;

    if (newdim < 1) newdim = 1;
    switch (XtMakeResizeRequest((Widget)pw,
				Major(pw, newdim, pw->core.width),
				Major(pw, pw->core.height, newdim),
 			        &replyWidth, &replyHeight))
    {
      case XtGeometryYes:
          *reply_dim = newdim;
          result = XtGeometryYes;
          break;

      case XtGeometryAlmost:
          XtMakeResizeRequest((Widget)pw, replyWidth, replyHeight, NULL, NULL);
          *reply_dim = Major(pw, replyWidth, replyHeight);
          result = XtGeometryAlmost;
          break;

      case XtGeometryNo:
      default:
          *reply_dim = MajorSize(pw);
          result = XtGeometryNo;
          break;
    }
    return(result);
}


/*************************************<->*************************************
 *
 *  ResetDMajors
 *
 *   Description:
 *   -----------
 *     Set the desired size field in the constraint record for each
 *     pane.
 *
 *************************************<->***********************************/
static void
ResetDMajors(
        XmPanedWindowWidget pw )
{
    Widget *childP;
    int i;

    for (i=0, childP = pw->paned_window.managed_children;
          i < pw->paned_window.pane_count;  childP++, i++)
             PaneDMajor(*childP) = MajorChildSize(pw, *childP);
}


/*************************************<->*************************************
 *
 *  RefigureLocations
 *
 *   Description:
 *   -----------
 *   This is the workhorse routine which actually computes where the children
 *   are going to be placed.  It honors any Min/Max constraints placed on a
 *   pane as well as honoring the direction to do the refiguring.
 *
 *************************************<->***********************************/
static void
RefigureLocations(
        register XmPanedWindowWidget pw,
        int c_index,
        Direction dir,
        Boolean rflag,
        Boolean sflag )
{
    WidgetList children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    int _dir = (dir == FirstPane) ? 1 : -1;
    int spacing;
    XmPanedWindowConstraintPart * pane;
    register Widget *childP;
    Position pos;
    int sizeused;
    int cdir, i;
    int pass;

    if (num_panes == 0 || !pw->paned_window.refiguremode)
	return;

    spacing = pw->paned_window.spacing;

   /*
    * ENFORCE THE MIN/MAX CONSTRAINTS; ALSO KEEP TRACK OF THE
    * TOTAL SIZE NEEDED TO DISPLAY VPANED WINDOW BASED ON
    * DESIRED SIZES OF PANES.
    */
   sizeused = 0;
   for (childP = children, i = 0; i < num_panes; childP++, i++)
     {
       pane = &(PaneInfo(*childP)->panedw);

       if (PaneDMajor(*childP) < pane->min) PaneDMajor(*childP) = pane->min;
       else if (PaneDMajor(*childP) > pane->max) PaneDMajor(*childP) = pane->max;

       sizeused += PaneDMajor(*childP) + spacing + 2 * (*childP)->core.border_width;

     }
   /*
    * Get rid of extra spacing from previous 'for' loop and add in
    * margin height at the top and bottom of the paned window
    */
    sizeused += 2*MajorMargin(pw) - spacing;

    childP = children + c_index;
    if (dir == FirstPane && c_index != num_panes - 1) childP++;
    cdir = _dir;
   /* allow at most 3 passes through the panes to adjust the heights */
    for (pass = 0; sizeused != MajorSize(pw) &&
		             pass < (9 * num_panes); pass++) {
	pane = &(PaneInfo(*childP)->panedw);
	if ((!pane->skip_adjust || sflag) || cdir != _dir) {
	    int old = PaneDMajor(*childP);
	    if (sizeused < MajorSize(pw))
	        PaneDMajor(*childP)+= MajorSize(pw) - sizeused;
	    else
	        if (sizeused - MajorSize(pw) < PaneDMajor(*childP) &&
                    PaneDMajor(*childP) - (sizeused - MajorSize(pw)) > 1)
		    PaneDMajor(*childP) -= sizeused - MajorSize(pw);
	        else
		    PaneDMajor(*childP) = 1;
	    if (PaneDMajor(*childP) < pane->min) PaneDMajor(*childP) = pane->min;
	    if (PaneDMajor(*childP) > pane->max) PaneDMajor(*childP) = pane->max;
	    sizeused += (PaneDMajor(*childP) - old);
	}
	childP+= cdir;
 /*
  * WE GET INTO THE NEXT WHILE LOOP WHEN WE HAVE EXHAUSTED OUR
  * LIST OF CHILDREN AND WE STILL NEED TO REDISTRIBUTE A CHANGE IN
  * SIZE, NOW WE WILL TRY TO CHANGE DIRECTION AND SEE IF THERE
  * IS A PANE BACK WHERE WE STARTED FROM THAT CAN ABSORB THE
  * SHORTAGE/OVERAGE
  */
	while ((childP < children) || ((childP - children) >= num_panes)) {
	    cdir = -cdir;
	    if (cdir == _dir) {
    		pos = MajorMargin(pw);
    		for (childP = children, i = 0; i < num_panes; childP++, i++) {
       			PaneDPos(*childP) = pos;
       			pos += PaneDMajor(*childP) + spacing +
                             2 * (*childP)->core.border_width;
    		}
    		pos += MajorMargin(pw) - spacing;
                /* if not resizing, make sure the sum of the pane
                   heights are not greater than the vpane height */
                if (!rflag){
                   if (pos > MajorSize(pw)) {
                       childP = children + c_index;
                       pane = &(PaneInfo(*childP)->panedw);
                       if (PaneDMajor(*childP) > (pos - MajorSize(pw)))
                         PaneDMajor(*childP) = (PaneDMajor(*childP) -
                                               (pos - MajorSize(pw)));
                       else
                         PaneDMajor(*childP) = 1;

                   } else {
                       return;
                   }
                } else
                   return;
             }
	     childP = children + c_index + cdir;

       /* HANDLE SPECIAL CASE */
           if ((c_index == 0) && (cdir < 0)) childP++;
	}
    }
    pos = MajorMargin(pw);
    for (childP = children, i = 0; i < num_panes; childP++, i++) {
       PaneDPos(*childP) = pos;
       pos += PaneDMajor(*childP) + spacing +
            2 * (*childP)->core.border_width;
    }

}


/*************************************<->*************************************
 *
 *  CommitNewLocations
 *
 *   Description:
 *   -----------
 *    Use the core width/height,  It also raises the sash for the
 *    pane, and prevents the sash from disappearing from the screen.
 *
 *************************************<->***********************************/
static void
CommitNewLocations(
        XmPanedWindowWidget pw,
	Widget instigator)
{
    WidgetList children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    register Widget *childP;
    XWindowChanges changes;
    int i, offset, sepPos;
    int minor_dim, major_dim ;

    changes.stack_mode = Above;

    offset = MinorMargin(pw);

    for (childP = children, i = 0; i < num_panes; childP++, i++) {
	register XmPanedWindowConstraintPart * pane =
                                 &(PaneInfo(*childP)->panedw);
	register Widget sash = pane->sash;
	register Widget separator = pane->separator;

        if (sash)  /* IF THIS IS NOT NULL */
	{
	     int tmp = MinorSize(pw) - 2 * ((*childP)->core.border_width +
					  MinorMargin(pw)) ;
	     if (tmp <= 0) tmp = 1;

	     if (*childP != instigator) {
		 XmeConfigureObject(*childP,
				    Major(pw, PaneDPos(*childP), offset),
				    Major(pw, offset, PaneDPos(*childP)),
				    Major(pw, PaneDMajor(*childP), tmp),
				    Major(pw, tmp, PaneDMajor(*childP)),
				    (*childP)->core.border_width);
	     }
	     if (separator)
	     {
		 sepPos = MajorChildPos(pw, *childP) + MajorChildSize(pw, *childP) +
		   2 * (*childP)->core.border_width +
		     pw->paned_window.spacing/2 - MajorChildSize(pw, separator)/2 -
		       separator->core.border_width;

		 XmeConfigureObject(separator,
				    Major(pw, sepPos, separator->core.x),
				    Major(pw, separator->core.y,  sepPos),
				    Major(pw, separator->core.width, pw->core.width),
				    Major(pw, pw->core.height, separator->core.height),
				    separator->core.border_width);
           }

	   /* Move and Display the Sash */
           if (pw->paned_window.sash_indent < 0)
  	        minor_dim = MinorSize(pw) + pw->paned_window.sash_indent
		           - MinorChildSize(pw, sash) - sash->core.border_width*2;
           else
                minor_dim = pw->paned_window.sash_indent;


            /* PREVENT SASH FROM DISAPPEARING FROM SCREEN */
           if ((minor_dim > (MinorSize(pw) - MinorChildSize(pw, sash))) ||
              (minor_dim < 0))
                minor_dim = 0;

           major_dim = MajorChildPos(pw, *childP) + MajorChildSize(pw, *childP) +
                       2 * (*childP)->core.border_width +
                       pw->paned_window.spacing/2 - MajorChildSize(pw, sash)/2 -
		       sash->core.border_width;

	   /* This should match XmeConfigureObject, except that we're
	    * also insuring the sash is Raised in the same request */

	   sash->core.x = changes.x = Major(pw, major_dim, minor_dim);
	   sash->core.y = changes.y = Major(pw, minor_dim, major_dim);

	   if (XtIsRealized(pane->sash))
	       XConfigureWindow( XtDisplay(pane->sash), XtWindow(pane->sash),
			         CWX | CWY | CWStackMode, &changes );
         } else {
	    if (*childP != instigator) {
		int tmp = MinorSize(pw) - 2*(pw->core.border_width +
					   MinorMargin(pw)) ;
		if (tmp <= 0) tmp = 1;
		XmeConfigureObject( *childP,
				   Major(pw, PaneDPos(*childP), offset),
				   Major(pw, offset, PaneDPos(*childP)),
				   Major(pw, PaneDMajor(*childP), tmp),
				   Major(pw, tmp, PaneDMajor(*childP)),
				   (*childP)->core.border_width);
	   }
         }
    }
}


/*************************************<->*************************************
 *
 *  RefigureLocationsAndCommit
 *
 *   Description:
 *   -----------
 *    A utility call that does the call to calculate the pane layout
 *    and then move the panes to their new locations.
 *
 *************************************<->***********************************/
static void
RefigureLocationsAndCommit(
        XmPanedWindowWidget pw,
        int c_index,
        Direction dir,
        Boolean rflag )
{
    if (pw->paned_window.refiguremode) {
	RefigureLocations(pw, c_index, dir, rflag, False);
	CommitNewLocations(pw, NULL);
    }
}


/*************************************<->*************************************
 *
 *  DrawTrackLines
 *
 *   Description:
 *   -----------
 *     Erase any old track lines (point are kept in the pane's constraint
 *     record--olddPos field) and then draw new track lines across the top
 *     of all panes (except the first).  These lines will be interactively
 *     moved (by other routines) to respond to the user's request to resize
 *     panes within the VPane Manager.
 *
 *************************************<->***********************************/
static void
DrawTrackLines(
        XmPanedWindowWidget pw )
{
    Widget *childP;
    XmPanedWindowConstraintPart * pane;
    Widget *children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    Dimension sep_size;
    int offset;

    for (childP = children + 1; childP - children < num_panes; childP++) {
        pane = &(PaneInfo(*childP)->panedw);
        sep_size =
		 pane->separator ? MajorChildSize(pw, pane->separator): 2;

	if (PaneOldDPos(*childP) != PaneDPos(*childP)) {
	    offset =  PaneOldDPos(*childP) - (pw->paned_window.spacing +
                      sep_size) / 2;
            XDrawLine(XtDisplay(pw), XtWindow(pw), pw->paned_window.flipgc,
		      Major(pw, offset, 0),
		      Major(pw, 0, offset),
		      Major(pw, offset, pw->core.width),
		      Major(pw, pw->core.height, offset));
	    offset = PaneDPos(*childP) - (pw->paned_window.spacing +
                      sep_size) / 2;
            XDrawLine(XtDisplay(pw), XtWindow(pw), pw->paned_window.flipgc,
		      Major(pw, offset, 0),
		      Major(pw, 0, offset),
		      Major(pw, offset, pw->core.width),
		      Major(pw, pw->core.height, offset));
	    PaneOldDPos(*childP) = PaneDPos(*childP);
	}
    }
}

/*************************************<->*************************************
 *
 *  EraseTrackLines
 *
 *   Description:
 *   -----------
 *   After the user has stopped adjusting the pane sizes, erase the last
 *   set of track lines (remember that DrawTrackLines erases old track
 *   lines before drawing new ones).
 *
 *************************************<->***********************************/
static void
EraseTrackLines(
        XmPanedWindowWidget pw )
{
    Widget *childP;
    XmPanedWindowConstraintPart * pane;
    Widget *children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    Dimension sep_size;
    int offset;

    for (childP = children + 1; childP - children < num_panes; childP++) {
        pane = &(PaneInfo(*childP)->panedw);
        sep_size = pane->separator ? MajorChildSize(pw, pane->separator): 2;
	offset = PaneOldDPos(*childP) - (pw->paned_window.spacing +
                  sep_size) / 2;
	XDrawLine(XtDisplay(pw), XtWindow(pw), pw->paned_window.flipgc,
		  Major(pw, offset, 0), Major(pw, 0, offset),
		  Major(pw, offset, pw->core.width),
		  Major(pw, pw->core.height, offset));
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
ProcessKeyEvent(
        XtPointer client_data,
        XtIntervalId *id )
{
    Widget w = (Widget) client_data;
    register XmPanedWindowWidget pw = (XmPanedWindowWidget)w->core.parent;
    register WidgetList children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    Widget *childP;
    XmPanedWindowConstraintPart * pane;
    short c_index;
    int diff;

    /* first mark the timeout as removed */
    pw->paned_window.timer = 0 ;

    PaneFirst(pw) = PaneLast(pw) = NULL;

    if (pw->paned_window.increment_count < 0) {
        /* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
           TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
	c_index = PaneIndex(PaneInfo(w)->panedw.sash);
        if (c_index < (num_panes-1)) c_index++;

	pane = &(PaneInfo(children[c_index])->panedw);
	while (pane->max == pane->min && c_index < num_panes - 1)
	     pane = &(PaneInfo(children[++c_index])->panedw);
 	PaneLast(pw) = PaneInfo(children[c_index]);
    } else {
        /* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
           TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
 	c_index = PaneIndex(PaneInfo(w)->panedw.sash);

	pane = &(PaneInfo(children[c_index])->panedw);
	while (pane->max == pane->min && c_index > 0)
             pane = &(PaneInfo(children[--c_index])->panedw);
	PaneFirst(pw) = PaneInfo(children[c_index]);
    }

    for (childP = children; childP - children < num_panes; childP++)
       PaneOldDPos(*childP) = -99;

    ResetDMajors( pw );
    diff = pw->paned_window.increment_count;
    if (PaneFirst(pw)) {
       /* make sure size don't go negative */
        if ((-diff) >= (int)PaneFirstDMajor(pw)) {
           /* can't add as much to other pane */
/*******************************************************************
            diff = -PaneFirstDMajor(pw) + 1;
*******************************************************************/
            PaneFirstDMajor(pw) = 1;
        }
        else
            PaneFirstDMajor(pw) += diff;
        RefigureLocationsAndCommit(pw, PaneIndex(PaneInfo(w)->panedw.sash),
                                   FirstPane, False);
    } else if (PaneLast(pw)) {
        if (diff >= (int)PaneLastDMajor(pw)) {
            PaneLastDMajor(pw) = 1;
        } else
            PaneLastDMajor(pw) -= diff;
        RefigureLocationsAndCommit(pw, PaneIndex(PaneInfo(w)->panedw.sash),
                                   LastPane, False);
    }
    pw->paned_window.increment_count = 0;
}


/*************************************<->*************************************
 *
 *  HandleSash
 *
 *   Description:
 *   -----------
 *    Selection Events on the sashes invoke this routine through
 *    callbacks.  An adjustment of the size of the panes always involves
 *    2 panes (one to shrink and one to grow) if the type of sash grab is
 *    "This Window Only" then both a top and bottom pane are selected (these
 *    are the panes which will be adjusted); if the grab type is "Upper Pane"
 *    then only a "top" pane is chosen and the correct "bottom"
 *    pane will be determined by the "RefigureLocations" routine.  If
 *    the grab type is "Lower" then only a "bottom" pane is chosen and the
 *    the correct "top" pane is found by the "RefigureLocations" routine.
 *
 *************************************<->***********************************/
static void
HandleSash(
        Widget w,
        XtPointer closure,
        XtPointer callData )
{
    SashCallData call_data = (SashCallData)callData;
    register XmPanedWindowWidget pw = (XmPanedWindowWidget)w->core.parent;
    register WidgetList children = pw->paned_window.managed_children;
    int num_panes = pw->paned_window.pane_count;
    short increment = 1;
    short c_index;
    int diff, pos, action_param, increment_param, direction_param;
    Widget *childP;
    XmPanedWindowConstraintPart * pane;

    if (call_data->num_params == 0)
    {
        XmeWarning( (Widget) pw, MESSAGE8);
        return;
    }

    _XmConvertActionParamToRepTypeId((Widget) pw,
				     XmRID_PANED_WINDOW_SASH_ACTION_PARAMS,
				     call_data->params[0],
				     False, &action_param);

    switch (call_data->event->xany.type)
    {
      case ButtonPress:
      case ButtonRelease:
	pos = Major(pw, call_data->event->xbutton.x_root,
		    call_data->event->xbutton.y_root);
	break;

      case KeyRelease:
	return;

      case KeyPress:
	if (call_data->num_params < 3)
	{
	    XmeWarning( (Widget) pw, MESSAGE8);
	    return;
	}

	/* Verify that we have a KEY action */
	if (action_param == _KEY)
	{
	    /* Validate the other two parameters */
	    if ((_XmConvertActionParamToRepTypeId((Widget) pw,
			     XmRID_PANED_WINDOW_SASH_INCREMENT_ACTION_PARAMS,
			     call_data->params[1],
			     False, &increment_param) == False) ||
		(_XmConvertActionParamToRepTypeId((Widget) pw,
			     XmRID_PANED_WINDOW_SASH_DIRECTION_ACTION_PARAMS,
			     call_data->params[2],
			     False, &direction_param) == False))
	      return;

	    /* Have to track Up or Left */
	    if (direction_param == _UP || direction_param == _LEFT)
	    {
		if (increment_param == _LARGE_INCR)
		  increment = -10;
		else
		  increment = -1;
	    }
	    /* Have to track Down or Right */
	    else if (direction_param == _DOWN || direction_param == _RIGHT)
	    {
		if (increment_param == _LARGE_INCR)
		  increment = 10;
		else
		  increment = 1;
	    }
	    if (!pw->paned_window.increment_count) {
		pw->paned_window.increment_count = increment;
		pw->paned_window.timer =
		    XtAppAddTimeOut(XtWidgetToApplicationContext( (Widget) pw),
				XtGetMultiClickTime(XtDisplay(pw)),
				ProcessKeyEvent, (XtPointer) w);
	    }
	    else pw->paned_window.increment_count += increment;
	}
	return;

      case MotionNotify:
	pos = Major(pw, call_data->event->xmotion.x_root,
		    call_data->event->xmotion.y_root);
	break;

      default:
	pos = PaneStartPos(pw);
    }

    switch (action_param)
    {
      case _START:	/* Start adjustment */
	PaneFirst(pw) = PaneLast(pw) = NULL;

	/* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
	   TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
	c_index = PaneIndex(PaneInfo(w)->panedw.sash);
	if (c_index < (num_panes-1)) c_index++;

	pane = &(PaneInfo(children[c_index])->panedw);
	while (pane->max == pane->min && c_index < num_panes - 1)
	  pane = &(PaneInfo(children[++c_index])->panedw);
	PaneLast(pw) = PaneInfo(children[c_index]);

	/* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
	   TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
	c_index = PaneIndex(PaneInfo(w)->panedw.sash);

	pane = &(PaneInfo(children[c_index])->panedw);
	while (pane->max == pane->min && c_index > 0)
	  pane = &(PaneInfo(children[--c_index])->panedw);
	PaneFirst(pw) = PaneInfo(children[c_index]);

	PaneStartPos(pw) = pos;

	for (childP = children; childP - children < num_panes; childP++)
	  PaneOldDPos(*childP) = -99;

	if (pw->paned_window.flipgc)
	  AdjustGC(pw);
	break;

      case _MOVE:
	ResetDMajors( pw );
	diff = pos - PaneStartPos(pw);
	if (diff > 0 && PaneFirst(pw)) {
	    /* make sure size don't go negative */
	    if ((-diff) >= (int)PaneFirstDMajor(pw)) {
		/* can't add as much to other pane */
		/******************************************************
		  diff = -PaneFirstDMajor(pw) + 1;
		  *****************************************************/
		PaneFirstDMajor(pw) = 1;
	    }
	    else {
		PaneFirstDMajor(pw) += diff;
	    }
	    RefigureLocations(pw, PaneIndex(PaneInfo(w)->panedw.sash),
			      FirstPane, False, True);
	} else if (PaneLast(pw)) {
	    if (diff >= (int)PaneLastDMajor(pw)) {
		PaneLastDMajor(pw) = 1;
	    } else {
		PaneLastDMajor(pw) -= diff;
	    }
	    RefigureLocations(pw, PaneIndex(PaneInfo(w)->panedw.sash),
			      LastPane, False, True);
	}

	DrawTrackLines(pw);
	break;


      case _COMMIT:
	EraseTrackLines(pw);
	CommitNewLocations(pw, NULL);
	break;

      default:
	XmeWarning( (Widget) pw, MESSAGE9);
    }
}



/*************************************<->*************************************
 *
 * GeometryManager
 *
 *   Description:
 *   -----------
 *  The Geometry Manager only allows changes after Realize if
 *  allow_resize is True in the constraints record.  It
 *  only allows height changes, but offers the requested height
 *  as a compromise if both width and height changes were requested.
 *  As all good Geometry Managers should, we will return No if the
 *  request will have no effect; i.e. when the requestor is already
 *  of the desired geometry.
 *
 *************************************<->***********************************/
static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
    XmPanedWindowWidget pw = (XmPanedWindowWidget) w->core.parent;
    XtGeometryMask mask = request->request_mode;
    XtWidgetGeometry allowed, geo_desired, geo_reply;
    XmPanedWindowConstraintPart * pane = &(PaneInfo(w)->panedw);
    Boolean is_almost = FALSE ;
    register Widget *children;
    int i;
    Dimension childMinor, childBorderWidth, new_major, old_dmajor, tmp;
    int num_panes =0;
    register Widget *childP;

    /* First treat the special case resulting from a change in positionIndex */
    if (PanePosIndex(w) == XmLAST_POSITION) {
                    /* as set in ConstraintSetValues */
       /* first reset the value of positionIndex to its real value */
       for (i = 0 ; i < pw->composite.num_children; i++)
         if (pw->composite.children[i] == w) {
             PanePosIndex(w) = i ;
             break ;
         }

       /* then accept the desired change */
       if (IsX(request) && request->x >= 0) w->core.x = request->x;
       if (IsY(request) && request->y >= 0) w->core.y = request->y;
       if (IsHeight(request) && request->height > 0)
         w->core.height = request->height;
       if (IsWidth(request) && request->width > 0)
         w->core.width = request->width;
      return XtGeometryYes;
    }

       /* yes for an internal kid */
    if (XmIsSash(w)) {
	if ((mask & CWX) && request->x >= 0) w->core.x = request->x;
        if ((mask & CWY) && request->y >= 0) w->core.y = request->y;
        if ((mask & CWHeight) && request->height > 0)
	   w->core.height = request->height;
        if ((mask & CWWidth) && request->width > 0)
	   w->core.width = request->width;
	return XtGeometryYes;
    }

    old_dmajor = PaneDMajor(w);

    /* disallow resizes if flag for this pane says no */
    /* why this realized test ? */
    if (XtIsRealized((Widget)pw) && !pane->allow_resize)
      return XtGeometryNo;

    /* reject attempts that do not want to adjust height or width */
    /* issue : and borderwidth? */
    if (!(mask & (CWWidth | CWHeight)))
	return XtGeometryNo;

	/* mark with almost request that attemps to change position *and*
       size (since we've already pass the previous test) */
    if (mask & (CWX | CWY)) is_almost = TRUE ;

    /* check for queryonly */
    if ((mask & XtCWQueryOnly) || is_almost)
	geo_desired.request_mode = XtCWQueryOnly;
    else
	geo_desired.request_mode = 0 ;


    if ((mask & Minor(pw, CWWidth, CWHeight))) {
      /*
       * NOW RECOMPUTE THE LIST OF MANAGED CHILDREN.
       */
	ReManageChildren(pw);
	children = pw->paned_window.managed_children;

      /*
       * COUNT THE NUMBER OF PANES THAT ARE MANAGED.
       */
	childP = children;
	while ( (num_panes < pw->paned_window.num_managed_children) &&
                XtIsRectObj(*childP) &&
		IsPane(*childP))
		{
		     childP++;
                     num_panes++;
		}

	pw->paned_window.pane_count = num_panes;

      /*
       * SET WIDTH OF PANED WINDOW EQUAL TO THAT OF WIDEST CHILD
       * seems useless...
       */
	childMinor = 0;
	childBorderWidth = 0;
	for (childP = children, i = 0; i < num_panes; childP++, i++)
	    {
		if ((MinorChildSize(pw, *childP) + (*childP)->core.border_width) >
		    childMinor + childBorderWidth) {
		    childMinor = MinorChildSize(pw, *childP) ;
		    childBorderWidth = (*childP)->core.border_width;
		}
	    }
	if (childMinor < 1) childMinor = 1;

	/* only allowed to grow */
	if (MinorReq(pw, request) >= childMinor) {

	    MinorAssign(pw, geo_desired, MinorReq(pw, request) +
			2 * (childBorderWidth + MinorMargin(pw))) ;
	    geo_desired.request_mode |= Minor(pw, CWWidth, CWHeight);

	    switch (XtMakeGeometryRequest((Widget) pw, &geo_desired,
					 &geo_reply))
	       {
	       case XtGeometryYes:
		   MinorAssign(pw, allowed, MinorReq(pw, request)) ;
		   break;

	       case XtGeometryAlmost:
		   if (MinorReq(pw, &geo_reply) >
		       2*(childBorderWidth + MinorMargin(pw)))
		       tmp = MinorReq(pw, &geo_reply) -
			   2*(childBorderWidth + MinorMargin(pw));
		   else tmp = 1;
		   MinorAssign(pw, allowed, tmp);
		   if (MinorReq(pw, &allowed) < childMinor) {
		       MinorAssign(pw, allowed, childMinor) ;
		   }
		   is_almost = TRUE ;
		   break;

	       case XtGeometryNo:
               default:
		   MinorAssign(pw, allowed, MinorSize(pw) -
                                  2*(childBorderWidth + MinorMargin(pw)));
		   break;
	       }
	} else {
	    /* give the max possible and almost */
	    MinorAssign(pw, allowed, childMinor) ;
	    is_almost = TRUE ;
	}

    } else {
	/* kid not interested by Width, take its current size */
	MinorAssign(pw, allowed, MinorChildSize(pw, w));
    }

    /* the whole stuff of individual requests, first Width and then Height
       doesn't look correct in my opinion (what if parent has a
       constrainted ratio?), but I think we can live with it for now */

    /* just forget about the CWWidth setting and go for Height */
    if ((geo_desired.request_mode & XtCWQueryOnly) || is_almost)
	geo_desired.request_mode = XtCWQueryOnly;

    if (mask & Major(pw, CWWidth, CWHeight)) {
	/* try to almost the sucker */
	if (MajorReq(pw, request) < pane->min) {
	    MajorAssign(pw, allowed, pane->min);
	    MajorAssign(pw, *request, pane->min);
	    is_almost = TRUE ;
	    geo_desired.request_mode = XtCWQueryOnly;
	}

	if (MajorReq(pw, request) > pane->max)  {
	    MajorAssign(pw, allowed, pane->max);
	    MajorAssign(pw, *request, pane->max);
	    is_almost = TRUE ;
	    geo_desired.request_mode = XtCWQueryOnly;
	}

	/* try out the new size */
	ResetDMajors( pw );
	old_dmajor = PaneDMajor(w);
	PaneDMajor(w) = MajorReq(pw, request);

	if ((new_major = NeedsAdjusting(pw)) != 0) {

	    MajorAssign(pw, geo_desired, new_major);
	    geo_desired.request_mode |= Major(pw, CWWidth, CWHeight);

	    switch(XtMakeGeometryRequest((Widget) pw, &geo_desired,
					 &geo_reply)) {
		case XtGeometryYes:
		    MajorAssign(pw, allowed, MajorReq(pw, request));
		    break;

		case XtGeometryAlmost:
		    if ((new_major > MajorReq(pw, &geo_reply)) &&
			MajorReq(pw, request) > (new_major - MajorReq(pw, &geo_reply)))
			tmp = MajorReq(pw, request) -
			    (new_major - MajorReq(pw, &geo_reply));
		    else
			tmp = 1 ;
		    MajorAssign(pw, allowed, tmp);
		    if (MajorReq(pw, &allowed) < pane->min) {
			MajorAssign(pw, allowed, pane->min);
		    }
		    if (MajorReq(pw, &allowed) > pane->max) {
			MajorAssign(pw, allowed, pane->max);
		    }
		    is_almost = TRUE ;
		    break;

		case XtGeometryNo:
                default:
		    MajorAssign(pw, allowed, MajorChildSize(pw, w));
		    break;
		}
	}
	else /* ok */{
	    MajorAssign(pw, allowed, MajorReq(pw, request));
	}
    } else {
	/* kid not interested by Height, take its current size */
	MajorAssign(pw, allowed, MajorChildSize(pw, w));
    }

    /* now the conclusion:
       If it's not almosted yet (due to a position request) and if I got
       what I've asked for or if I didn't ask anything in this dimension,
       then it's a Yes.
       If queryonly was specified, don't forget to just undo the change made.
    */
    if ((!is_almost) &&
	(((mask & CWWidth) && (allowed.width == request->width)) ||
	 (!(mask & CWWidth))) &&
	(((mask & CWHeight) && (allowed.height == request->height)) ||
	 (!(mask & CWHeight)))) {

	if (!(mask & XtCWQueryOnly)) {
	    w->core.width = allowed.width;
	    w->core.height = allowed.height;

	    RefigureLocations(pw, PaneIndex(w), FirstPane, False, False);
	    if (Horizontal(pw)) {
		w->core.x = PaneDPos(w) ;
		w->core.y = pw->paned_window.margin_height ;
	    } else {
		w->core.y = PaneDPos(w) ;
		w->core.x = pw->paned_window.margin_width ;
	    }

	    /* relayout all kids except instigator */
	    CommitNewLocations(pw, w );
	} else {
	    PaneDMajor(w) = old_dmajor;
	}

	return XtGeometryYes ;
    }

    /* else we decide to reply Almost, so undo the change made
       and reply allowed */

    PaneDMajor(w) = old_dmajor;
    allowed.request_mode = CWWidth|CWHeight ;
    *reply = allowed;
    return XtGeometryAlmost;
}


/************************************************************************
 *
 *  Constraint Initialize
 *
 *  This routine is called to initialize the constraint record for
 *  every child inserted into the paned_window window.
 *
 ************************************************************************/
static void
ConstraintInit(
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmPanedWindowWidget pw = (XmPanedWindowWidget) XtParent(new_w);
  XmPanedWindowConstraintPart * pane = &(PaneInfo(new_w)->panedw);
  int size;

  if (!XtIsRectObj(new_w)) return;

  /* don't let sashes or separators get a non default position - which could
     easily happen with resource like  XmPanedWindow*kid*positionIndex: 0 */
  if (pw->paned_window.recursively_called)
      pane->position_index = XmLAST_POSITION ;

  size = MajorChildSize(pw, new_w);

  if (pane->min == 0)
    {
        XmeWarning( (Widget) pw, MESSAGE4);
     	pane->min = 1;
    }

  if (pane->max == 0)
    {
        XmeWarning( (Widget) pw, MESSAGE5);
     	pane->max = pane->min + 1;
    }

  if (pane->min > pane->max)
    {
        XmeWarning( (Widget) pw, MESSAGE6);
     	pane->max = pane->min + 1;
    }

  /* ENFORCE MIN/MAX if child already managed */
  if (size < pane->min) size = pane->min;
  if (size > pane->max) size = pane->max;

  if (XtIsManaged(new_w))
    XmeConfigureObject(new_w,
		       new_w->core.x, new_w->core.y,
		       Major(pw, size, new_w->core.width),
		       Major(pw, new_w->core.height, size),
		       new_w->core.border_width);
}



/***************************************************************************
 *
 * static Cardinal
 * InsertOrder (cw, args, p_num_args)
 *
 *   This function searches through the composite widget's list of children
 *   to find the correct insertion position for the new child.  If the
 *   new child is an ordinary child (not a subclass of XmSashWidget)
 *   the position returned will cause it to be inserted after the other
 *   ordinary children but before any Sashs; if the new child is a
 *   sash the position returned will cause it to be inserted after
 *   the regular panes.
 *
 *   This procedure also handles the positionIndex constraint resource.
 *
 ************************************************************************/
static Cardinal
InsertOrder(
        Widget w )
{
   CompositeWidget pw = (CompositeWidget) XtParent(w);
   Cardinal i=0 ;

   /* find the number of regular children */
   while ((i < pw->composite.num_children) &&
		IsPane(pw->composite.children[i]))
                i++;

   /* i is the maximum positionIndex allowed, 0 is the min */
   /* if a paned position has been specified - should not happen
      for sash and separator, since we tracked that in constraint init -
      and if it's a correct value, use it */
   if (PanePosIndex(w) != XmLAST_POSITION) {
       if ((PanePosIndex(w) >= 0) && (PanePosIndex(w) < i)) {
	   return PanePosIndex(w) ;
       }
   } /* all PanePosIndex will be res-et in InsertChild proc */

   return (i);
}



/*************************************<->*************************************
 *
 *  InsertChild()
 *
 *************************************<->***********************************/
static void
InsertChild(
        register Widget w )
{
   XmPanedWindowWidget pw = (XmPanedWindowWidget)w->core.parent;
   XmPanedWindowConstraintPart * pane = &(PaneInfo(w)->panedw);
   Arg args[10];
   int n,i;
   Widget *p ;
   XmNavigationType navType;
   XtWidgetProc insert_child;

   if (!XtIsRectObj(w)) return;

   /*
    * Insert the child widget in the composite children list with an
    * insertion procedure exported from Manager.c, it, in turn, will
    * make use of the special insert procedure, InsertOrder, defined
    * above.  Essentially, ordinary panes are grouped together at the
    * beginning of the list of composite children, sashs are always
    * put at the end of the list.
    */

   _XmProcessLock();
   insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)->
			composite_class.insert_child;
   _XmProcessUnlock();
   (*insert_child) (w);

  /*
   * If we are creating a sash for an ordinary pane, then just
   * return here. However, before we do, set its "isPane" flag
   * to false, meaning that this is NOT a pane; if it is a
   * pane then set its "isPane" flag to TRUE.
   */
   if (pw->paned_window.recursively_called)
    {
      pane->isPane = FALSE;
      pane->separator = NULL;
      pane->sash = NULL;
      return;
    }
    pane->isPane = TRUE;

    n = 0;
    XtSetArg(args[n], Minor(pw, XmNwidth, XmNheight), MinorSize(pw)); n++;
    XtSetArg(args[n], XmNborderWidth, 0); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
    XtSetArg(args[n], XmNmargin, 0); n++;
    XtSetArg(args[n], XmNorientation,
                      Minor(pw, XmHORIZONTAL, XmVERTICAL)); n++;
    XtSetArg(args[n], XmNnavigationType, (XtArgVal) XmNONE); n++;
    pw->paned_window.recursively_called = True;
    pane->separator = XtCreateWidget("Separator", xmSeparatorGadgetClass,
                                     (Widget)pw, args, n);
    pw->paned_window.recursively_called = False;
    PaneInfo(pane->separator)->panedw.separator = w;

   /* If we create a sash then have the pane's constraint rec point
    * to the sash, ignore the constraint rec of the sash (Yes, it
    * gets one).  If we don't create a sash for the pane, just set
    * that field to NULL.
    */
    n = 0;
    XtSetArg(args[n], XmNwidth, pw->paned_window.sash_width); n++;
    XtSetArg(args[n], XmNheight, pw->paned_window.sash_height); n++;
    XtSetArg(args[n], XmNshadowThickness,
                pw->paned_window.sash_shadow_thickness); n++;
    XtSetArg(args[n], XmNunitType, (XtArgVal) XmPIXELS); n++;
    pw->paned_window.recursively_called = True;
    pane->sash = XtCreateWidget("Sash", xmSashWidgetClass, (Widget)pw, args, n);
    XtAddCallback(pane->sash, XmNcallback, HandleSash, (XtPointer)w);
    pw->paned_window.recursively_called = False;

    XtSetArg(args[0], XmNnavigationType, &navType);
    XtGetValues(w, args, 1);
    if(navType == XmNONE){
    	XtSetArg(args[0], XmNnavigationType, (XtArgVal) XmTAB_GROUP);
    	XtSetValues(w, args, 1);
    };

    PaneInfo(pane->sash)->panedw.sash = w;

    /* re-set the correct positionIndex values for everybody if
     * the new kid has been inserted in the list instead of put at the end
     */
    if (PanePosIndex(w) != pw->composite.num_children)
	for (i = 0, p = pw->composite.children;
	     i < pw->composite.num_children; i++, p++) {
            PanePosIndex(*p) = i ;
        }

} /* InsertChild */




/*************************************<->*************************************
 *
 * ChangeManaged
 *
 *************************************<->***********************************/
static void
ChangeManaged(
        Widget w )
{
   register XmPanedWindowWidget pw = (XmPanedWindowWidget)w;
   register Widget *childP;
   register int i;
   Widget *children;
   int num_children = pw->composite.num_children;
   Widget sash, separator;
   Dimension minor_dim = 0, major_dim = 0;
   Dimension childMinor, childBorderWidth, newMinor;
   Dimension needed;
   int num_panes = 0;
   XmPanedWindowConstraintPart * pane;
   XtGeometryResult result;

  /*
   * THIS PREVENTS US FROM RE-ENTERING THIS CODE AS WE MANAGE/UNMANAGE
   * THE SASHES
   */
   if (pw->paned_window.recursively_called++)  return;

  /*
   * NOW RECOMPUTE THE LIST OF MANAGED CHILDREN.
   */
   ReManageChildren(pw);
   children = pw->paned_window.managed_children;

  /*
   * COUNT THE NUMBER OF PANES THAT ARE MANAGED.
   */
   childP = children;
   while ((num_panes < pw->paned_window.num_managed_children) &&
             XtIsRectObj(*childP) &&
	     IsPane(*childP))
		{
		  childP++;
                  num_panes++;
		}

   pw->paned_window.pane_count = num_panes;


  /*
   * SET WIDTH OF PANED WINDOW EQUAL TO THAT OF WIDEST CHILD
   */

   childMinor = 0;
   childBorderWidth = 0;
   major_dim = 0;
   for (childP = children, i = 0; i < num_panes; childP++, i++)
   {
       pane = &(PaneInfo(*childP)->panedw);
       if (MinorChildSize(pw, *childP) + (*childP)->core.border_width >
            childMinor + childBorderWidth) {
	   childMinor = MinorChildSize(pw, *childP);
	   childBorderWidth = (*childP)->core.border_width;
       }

       if (MajorChildSize(pw, *childP) < pane->min)
	 XmeConfigureObject(*childP,
			    (*childP)->core.x, (*childP)->core.y,
			    Major(pw, pane->min, (*childP)->core.width),
			    Major(pw, (*childP)->core.height, pane->min),
			    (*childP)->core.border_width);
       if (MajorChildSize(pw, *childP) > pane->max)
	 XmeConfigureObject(*childP,
			    (*childP)->core.x, (*childP)->core.y,
			    Major(pw, pane->max, (*childP)->core.width),
			    Major(pw, (*childP)->core.height, pane->max),
			    (*childP)->core.border_width);
       major_dim += MajorChildSize(pw, *childP) + 2*(*childP)->core.border_width;
   }

   if (childMinor < 1) childMinor = 1;

  /*
   * NOW SCAN THE COMPOSITE LIST OF CHILDREN, AND MAKE SURE
   * THAT THEIR MANAGEMENT SETTING REFLECTS THAT OF THEIR PANE.
   */

   for (childP = pw->composite.children, i = 0;
                     i < num_children; childP++, i++)
    {
      if (! IsPane(*childP)) break;  /* jump out of loop */
      sash = PaneInfo(*childP)->panedw.sash;
      separator = PaneInfo(*childP)->panedw.separator;

      /* Realize child now so it won't get realized and put on the
         top of the stack, above the sash, when it is realized later */
      if (XtIsRealized((Widget)pw) && XtIsManaged(*childP))
          XtRealizeWidget(*childP);

      /* KEEP SOME RECORD OF DESIRED HEIGHT */
      PaneDMajor(*childP) = MajorChildSize(pw, *childP);

      newMinor = childMinor + 2*(childBorderWidth -
                                 (*childP)->core.border_width);

      if (XtIsManaged(*childP))
	XmeConfigureObject( *childP,
			   (*childP)->core.x, (*childP)->core.y,
			   Major(pw, (*childP)->core.width, newMinor),
			   Major(pw, newMinor, (*childP)->core.height),
			   (*childP)->core.border_width);
       if ((XtIsManaged(*childP)) && (*childP != children[num_panes-1])) {
              if (separator && pw->paned_window.separator_on) {
                  if (!XtIsManaged(separator)) XtManageChild(separator);
  	          if (XtIsRealized(separator))
		    XRaiseWindow(XtDisplay(separator), XtWindow(separator));
	      }

              if (sash) {
                 if (PaneInfo(*childP)->panedw.min !=
	 	     PaneInfo(*childP)->panedw.max) {
                    if (!XtIsManaged(sash)) XtManageChild(sash);
  	            if (XtIsRealized(sash))
		       XRaiseWindow( XtDisplay(sash), XtWindow(sash) );
	         } else
                    if (XtIsManaged(sash)) XtUnmanageChild(sash);
              }
	} else {
            if (sash)
               if (XtIsManaged(sash)) XtUnmanageChild(sash);
            if (separator && pw->paned_window.separator_on)
               if (XtIsManaged(separator)) XtUnmanageChild(separator);
        }
     }

  /* NOW CHANGEMANAGED CAN BE ENTERED NORMALLY */
   pw->paned_window.recursively_called = False;

   /*
    * TRAVERSE MANAGED PANES AND SET THE POSITION FIELD IN THE CONSTRAINT
    * RECORD TO 0, 1, 2, 3, 4 ETC.
    */

   childP = pw->paned_window.managed_children;
   for (i = 0; i < pw->paned_window.pane_count; childP++)
      (PaneInfo(*childP))->panedw.position = i++;

   minor_dim = childMinor;

   if (major_dim < 1) major_dim = 1;

   minor_dim += 2*(MinorMargin(pw) + childBorderWidth);
   major_dim += pw->paned_window.spacing*((num_panes)?num_panes-1:0) +
             2*MajorMargin(pw);

   while ((result = XtMakeResizeRequest(w,
					Major(pw, major_dim, minor_dim),
					Major(pw, minor_dim, major_dim),
					Major(pw, &major_dim, &minor_dim),
					Major(pw, &minor_dim, &major_dim)))
	  == XtGeometryAlmost) /*EMPTY*/;

   if (result == XtGeometryYes || result == XtGeometryAlmost ||
       MajorChildSize(pw, w) == major_dim) {
      /* see if the major size of the Paned Window
         needs to be adjusted to fit all the panes */
      if ((needed = NeedsAdjusting(pw)) != 0)
         AdjustPanedWindowMajor(pw, needed, &major_dim);
   } else {
       pw->paned_window.resize_at_realize = False;
   }

   ResetDMajors( pw );

   if (XtIsRealized((Widget)pw))
      RefigureLocationsAndCommit(pw, 0, FirstPane, False);

   XmeNavigChangeManaged((Widget)pw);

} /* ChangeManaged */



/*************************************<->*************************************
 *
 *  SetValues
 *   -----------------
 *
 *************************************<->***********************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmPanedWindowWidget oldpw = (XmPanedWindowWidget) cw ;
    XmPanedWindowWidget requestpw = (XmPanedWindowWidget) rw ;
    XmPanedWindowWidget newpw = (XmPanedWindowWidget) nw ;
    Boolean returnFlag = False;
    WidgetList children = newpw->composite.children;
    register Widget *childP;
    int num_children = newpw->composite.num_children;
    Arg sashargs[3];
    int i, minor_dim, major_dim;
    int n = 0;

   if (oldpw->core.background_pixel != newpw->core.background_pixel)
     {
       if (newpw->paned_window.flipgc != NULL) {
          XtReleaseGC(nw, newpw->paned_window.flipgc);
       }
       GetFlipGC(newpw);
       returnFlag = True;
     }

   if (newpw->paned_window.sash_width == 0)
      newpw->paned_window.sash_width = oldpw->paned_window.sash_width;

   if (oldpw->paned_window.sash_width != newpw->paned_window.sash_width)
    {
       XtSetArg(sashargs[n], XmNwidth, newpw->paned_window.sash_width); n++;
    }

   if (newpw->paned_window.sash_height == 0)
      newpw->paned_window.sash_height = oldpw->paned_window.sash_height;

   if (oldpw->paned_window.sash_height != newpw->paned_window.sash_height)
    {
       XtSetArg(sashargs[n], XmNheight, newpw->paned_window.sash_height); n++;
    }

   if (oldpw->paned_window.sash_shadow_thickness !=
           newpw->paned_window.sash_shadow_thickness)
    {
       XtSetArg(sashargs[n], XmNshadowThickness,
                newpw->paned_window.sash_shadow_thickness); n++;
    }

   if (oldpw->paned_window.separator_on != newpw->paned_window.separator_on &&
       num_children > 2) {
      WidgetList sep_children;
      Cardinal num_separators = 0;

     /* This should be more than enough space */
      sep_children = (WidgetList) XtMalloc((num_children/3) * sizeof(Widget));

      for (childP = children, i = 0; i < num_children; childP++, i++) {
          if (IsPane(*childP)) {
             Widget separator = PaneInfo(*childP)->panedw.separator;
             if (separator) {
                sep_children[num_separators] = separator;
                num_separators++;
             }
          }
       }
      if (num_separators != 0) {
         if (newpw->paned_window.separator_on)
            XtManageChildren((WidgetList) sep_children, num_separators);
         else
            XtUnmanageChildren((WidgetList) sep_children, num_separators);
      }

      XtFree((char *)sep_children);
   }

   if(!XmRepTypeValidValue( XmRID_ORIENTATION,
			   newpw->paned_window.orientation, (Widget) newpw)){
       newpw->paned_window.orientation = oldpw->paned_window.orientation;
   }
   if (newpw->paned_window.orientation != oldpw->paned_window.orientation)
   {
      ChangeManaged( (Widget) newpw);
      returnFlag = True;
   }


   if (oldpw->paned_window.sash_indent != newpw->paned_window.sash_indent ||
       oldpw->paned_window.margin_width != newpw->paned_window.margin_width ||
       oldpw->paned_window.margin_height != newpw->paned_window.margin_height ||
       oldpw->paned_window.sash_width != newpw->paned_window.sash_width ||
       oldpw->paned_window.sash_height != newpw->paned_window.sash_height ||
       oldpw->paned_window.sash_shadow_thickness !=
				 newpw->paned_window.sash_shadow_thickness ||
       oldpw->paned_window.spacing != newpw->paned_window.spacing) {

      for (childP = children, i = 0; i < num_children; childP++, i++) {
          if (IsPane(*childP)) {
             register XmPanedWindowConstraintPart * pane =
                                    &(PaneInfo(*childP)->panedw);
             register Widget sash = pane->sash;

             if (sash)  /* IF THIS IS NOT NULL */
             {
                /* Send Down Changes to Sash */
	        assert(n <= XtNumber(sashargs));
                if (n != 0)  /* something is in the arglist */
                   XtSetValues((Widget)sash, sashargs, n);

	        /* Move and Display the Sash */
                if (newpw->paned_window.sash_indent < 0)
  	           minor_dim = MinorSize(newpw) +
		       newpw->paned_window.sash_indent -
			   MinorChildSize(newpw, sash) - sash->core.border_width*2;
                 else
                   minor_dim = newpw->paned_window.sash_indent;


                 /* PREVENT SASH FROM DISAPPEARING FROM SCREEN */
                 if ((minor_dim > (MinorSize(newpw) -
			       MinorChildSize(newpw, sash))) || (minor_dim < 0))
                    minor_dim= 0;

	         major_dim = MajorChildPos(newpw, *childP) + MajorChildSize(newpw, *childP) +
	             2 * (*childP)->core.border_width +
		     newpw->paned_window.spacing/2 - MajorChildSize(newpw, sash)/2 -
		     sash->core.border_width;

		 XmeConfigureObject(sash,
				    Minor(newpw, minor_dim,major_dim),
				    Major(newpw, minor_dim,major_dim),
				    sash->core.width,
				    sash->core.height,
				    sash->core.border_width);
              }
          }
     }
	/* make the windows match the new locations */
	CommitNewLocations(newpw,NULL);
   }
   if (oldpw->paned_window.margin_width != newpw->paned_window.margin_width) {
      newpw->core.width = newpw->core.width +
                          ((2 * newpw->paned_window.margin_width) -
                           (2 * oldpw->paned_window.margin_width));
      returnFlag = True;
   }

   if ((oldpw->paned_window.spacing != newpw->paned_window.spacing ||
	oldpw->paned_window.margin_height != newpw->paned_window.margin_height ||
       (requestpw->paned_window.refiguremode &&
				 !(oldpw->paned_window.refiguremode))) &&
       XtIsRealized((Widget)newpw)) {
       Dimension needed;

      if ((needed = NeedsAdjusting(newpw)) != 0) {
         newpw->core.height = needed;
         returnFlag = True;
      }
   }

   return(returnFlag);
} /* SetValues */



/*************************************<->*************************************
 *
 *  PaneSetValues
 *
 *
 *************************************<->***********************************/
static Boolean
PaneSetValues(
        Widget old,
        Widget request,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
   XmPanedWindowWidget pw = (XmPanedWindowWidget)new_w->core.parent;
   Arg sashargs[4];
   int i, count ;
   XmPanedWindowConstraintPart * old_pane = &(PaneInfo(old)->panedw);
   XmPanedWindowConstraintPart * new_pane = &(PaneInfo(new_w)->panedw);
   register Widget tmp;
   XtWidgetGeometry current ;

   if (!XtIsRectObj(new_w)) return(FALSE);

    if (PanePosIndex(old) != PanePosIndex(new_w)) {

      /* first check for a valid value */
       /* count the number of pane children : not sash and separator */
       i = 0 ;
       while ((i < pw->composite.num_children) &&
		IsPane(pw->composite.children[i]))
		i++;

       /* special public value */
      if (PanePosIndex(new_w) == XmLAST_POSITION)
          PanePosIndex(new_w) = i - 1 ;

      if ((PanePosIndex(new_w) < 0) || (PanePosIndex(new_w) >= i)) {
          PanePosIndex(new_w) = PanePosIndex(old) ; /* warning ? */
      } else {
          int inc ;       /* change the configuration of the children list:
             put the requesting child at the new position and
             shift the others as needed (2 cases here) */
          tmp = pw->composite.children[PanePosIndex(old)] ;
          if (PanePosIndex(new_w) < PanePosIndex(old)) inc = -1 ; else inc = 1 ;

          for (i = PanePosIndex(old)  ; i != PanePosIndex(new_w) ; i+=inc) {
              pw->composite.children[i] = pw->composite.children[i+inc];
              PanePosIndex(pw->composite.children[i]) = i ;
          }
          pw->composite.children[PanePosIndex(new_w)] = tmp ;

	  /* save the current geometry of the child */
	  current.x = XtX(new_w) ;
	  current.y = XtY(new_w) ;
	  current.width = XtWidth(new_w) ;
	  current.height = XtHeight(new_w) ;
	  current.border_width = XtBorderWidth(new_w) ;

          /* re-layout, move the child and possibly change the pw size */
          if (XtIsRealized((Widget)pw)) ChangeManaged((Widget) pw) ;

          /* if we have changed the position/size of this child, next
             step after this setvalues chain is the geometry manager
             request. We need to tell the geometry manager that
             this request is to be always honored. As the positionIndex field
             itself is self-computable, we can use it to track this
             case. We set it to a magic value here, and in the geometry
             manager, we'll have to reset it to its correct value by
             re-computing it - adding a field in the instance is another way
             for doing that, clever but more expensive */
          if ((current.x != XtX(new_w)) ||
	      (current.y != XtY(new_w)) ||
	      (current.width != XtWidth(new_w)) ||
	      (current.height != XtHeight(new_w)) ||
	      (current.border_width != XtBorderWidth(new_w)))
	      PanePosIndex(new_w) = XmLAST_POSITION ;
      }
   }

   if (old_pane->min != new_pane->min || old_pane->max != new_pane->max)
    {

       if (new_pane->min < 1)
	{
           XmeWarning( (Widget) pw, MESSAGE4);
	   new_pane->min = old_pane->min;
	}

       if (new_pane->max < 1)
	{
           XmeWarning( (Widget) pw, MESSAGE5);
	   new_pane->max = old_pane->max;
	}

       if (new_pane->min > new_pane->max)
	{
           XmeWarning( (Widget) pw, MESSAGE6);
	   new_pane->min = old_pane->min;
	   new_pane->max = old_pane->max;
	}

       if ((new_pane->min == new_pane->max) &&
            (new_pane->sash != NULL))
      	{
           XtUnmanageChild(new_pane->sash);
      	}
       else
      	{
           count =pw->paned_window.pane_count -1;
           if (new_pane->position != count) {
              if (new_pane->separator == NULL) {
   		 pw->paned_window.recursively_called = True;
	         new_pane->separator = XtCreateWidget("separator",
                                                     xmSeparatorGadgetClass,
                                                     (Widget)pw, NULL, 0);
   		 pw->paned_window.recursively_called = False;

                 if (XtIsRealized(new_w)) XtRealizeWidget(new_pane->separator);

	      }
              if (pw->paned_window.separator_on)
                       XtManageChild(new_pane->separator);
              if (new_pane->min != new_pane->max) {
                 if (new_pane->sash == NULL) {
		    Cardinal nargs = 0;

                    XtSetArg(sashargs[nargs], XmNwidth,
			     pw->paned_window.sash_width), nargs++;
                    XtSetArg(sashargs[nargs], XmNheight,
			     pw->paned_window.sash_height), nargs++;
                    XtSetArg(sashargs[nargs], XmNshadowThickness,
                             pw->paned_window.sash_shadow_thickness), nargs++;
                    XtSetArg(sashargs[nargs], XmNunitType,
			     (XtArgVal) XmPIXELS), nargs++;
		    assert(nargs <= XtNumber(sashargs));
   		    pw->paned_window.recursively_called = True;
	            new_pane->sash =
		      XtCreateManagedWidget("sash",
					    xmSashWidgetClass, (Widget)pw,
					    sashargs, nargs);
		    XtAddCallback(new_pane->sash, XmNcallback,
				  HandleSash, (XtPointer)new_w);
   		    pw->paned_window.recursively_called = False;

                    if (XtIsRealized(new_w))  XtRealizeWidget(new_pane->sash);

	          }
               }
	    }
	}

       if (XtIsManaged(new_w) && XtIsRealized((Widget)pw))
       {
	  if (new_pane->min > MajorChildSize(pw, new_w))
          {
	      MajorAssign(pw, new_w->core, new_pane->min);
          }
	  if (new_pane->max < MajorChildSize(pw, new_w))
          {
	      MajorAssign(pw, new_w->core, new_pane->max);
          }
	  old_pane->min = new_pane->min;
	  old_pane->max = new_pane->max;
       }

       /* save the current geometry of the child  (CR 7671) */
       current.x = XtX(new_w) ;
       current.y = XtY(new_w) ;
       current.width = XtWidth(new_w) ;
       current.height = XtHeight(new_w) ;
       current.border_width = XtBorderWidth(new_w) ;

       /* re-layout, move the child and possibly change the pw size */
       if (XtIsRealized((Widget)pw)) ChangeManaged((Widget) pw) ;

       if ((current.x != XtX(new_w)) ||
	   (current.y != XtY(new_w)) ||
	   (current.width != XtWidth(new_w)) ||
	   (current.height != XtHeight(new_w)) ||
	   (current.border_width != XtBorderWidth(new_w)))
	   /* see above comment on LAST_POSITION */
	   PanePosIndex(new_w) = XmLAST_POSITION ;

       return(True);
   }

   return False;
}


/*************************************<->*************************************
 *
 *  Widget XmCreatePanedWindow(parent, name, args, argCount)
 *      Widget 	parent;
 *      char 	*name;
 *      ArgList	args;
 *      int	argCount;
 *
 *   Description:
 *   -----------
 *   A convenience routine to be used in creating a new PanedWindow
 *   manager widget.
 *
 *************************************<->***********************************/
Widget
XmCreatePanedWindow(
        Widget parent,
        char *name,
        ArgList args,
        Cardinal argCount )
{
    return (XtCreateWidget(name, xmPanedWindowWidgetClass,
                                   parent, args, argCount));
}

Widget
XmVaCreatePanedWindow(
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
                         xmPanedWindowWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedPanedWindow(
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
                         xmPanedWindowWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
