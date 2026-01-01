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
 */

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ScrolledW.c /main/16 1997/07/25 16:49:57 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>                               /* for abs, float operation... */
#include <Xm/ClipWindowP.h>
#include <Xm/DragDrop.h>
#include <Xm/DrawP.h>
#include <Xm/NavigatorT.h>
#include <Xm/ScrollBarP.h>                        /* might be worth getting rid of this one */
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "GeoUtilsI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScrolledWI.h"
#include "ScrollFramTI.h"
#include "TraversalI.h"
#include "XmI.h"

#define MAXPOS ((1 << 15)-1)

#define SB_BORDER_WIDTH 0
#define DEFAULT_SPACING 4
#define DEFAULT_SIZE 100
#define ExistManaged( wid)      (wid && XtIsManaged( wid))
#define defaultTranslations _XmScrolledW_ScrolledWindowXlations

/* Auto drag private ********/

#define FORWARD 0
#define BACKWARD 1

typedef struct _AutoDragClosure
{
    Widget widget;
    unsigned char direction ;
} AutoDragClosure;

typedef struct _AutoDragRects
{
    XRectangle up;
    XRectangle left;
} AutoDragRectsRec, *AutoDragRects;

#define POINT_IN_RECT(X, Y, rect) \
    (((X) >= rect.x) && ((Y) >= rect.y) && \
    ((X) <= rect.x + rect.width) && ((Y) <= rect.y + rect.height))

/*********/

#define SWMessage6  _XmMMsgScrolledW_0004
#define SWMessage7  _XmMMsgScrolledW_0005
#define SWMessage8  _XmMMsgScrolledW_0006
#define SWMessage9  _XmMMsgScrolledW_0007
#define SWMessage10 _XmMMsgScrolledW_0008
#define SWMessage11 _XmMMsgScrolledW_0009

/********    Static Function Declarations    ********/

static void ScrollBarPlacementDefault (
    Widget w,
    int offset,
    XrmValue *value);

static void VisualPolicyDefault(
    Widget widget,
    int offset,
    XrmValue *value) ;

static void SliderMove(
    Widget w,
    XtPointer closure,
    XtPointer cd) ;

static void MoveWindow(
    XmScrolledWindowWidget sw,
    int value,
    unsigned char direction) ;

static void LeftEdge(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void RightEdge(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void TopEdge(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void BottomEdge(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void PageLeft(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void PageRight(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void PageUp(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void PageDown(
    Widget wid,
    XEvent *event,
    String *params,
    Cardinal *num_params) ;

static void ClassPartInitialize(
    WidgetClass wc) ;

static void Initialize(
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num_args) ;

static void Redisplay(
    Widget wid,
    XEvent *event,
    Region region) ;

static void ComputeLocations(
    XmScrolledWindowWidget sw,
    Dimension HSBht, Dimension VSBht,
    Boolean HasHSB, Boolean HasVSB,
    Position * newx, Position * newy,
    Position * hsbX, Position * hsbY,
    Position * vsbX, Position * vsbY);

static void VariableLayout(
    XmScrolledWindowWidget sw) ;

static void ConstantLayout(
    XmScrolledWindowWidget sw) ;

static void Resize(
    Widget wid) ;

static void Destroy(
    Widget wid) ;

static void GetVariableSize(
    XmScrolledWindowWidget sw,
    Dimension *pwidth,
    Dimension *pheight) ;

static XtGeometryResult GeometryManager(
    Widget w,
    XtWidgetGeometry *request,
    XtWidgetGeometry *reply) ;

static void ChangeManaged(
    Widget wid) ;

static void InsertChild(
    Widget w) ;

static void DeleteChild(
    Widget w) ;

static XtGeometryResult QueryGeometry(
    Widget wid,
    XtWidgetGeometry *request,
    XtWidgetGeometry *ret) ;

static Boolean SetValues(
    Widget cw,
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num_args) ;

static void ConstraintInitialize(
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num_args) ;

static void GetHorRects(
    Widget sw,
    XRectangle ** hrect,
    Cardinal * num_hrect);

static void GetVertRects(
    Widget sw,
    XRectangle ** vrect,
    Cardinal * num_vrect);

static void HandleDrop(
    Widget       w,
    XtPointer    client_data,
    XtPointer    call_data);

static void HandleDrag(
    Widget       w,
    XtPointer    client_data,
    XtPointer    call_data);

static void ScrollFrameInit (
    Widget sf,
    XtCallbackProc moveCB,
    Widget scrollable);

static Boolean  GetInfo(
    Widget sf,
    Cardinal * dimension,
    Widget ** nav_list,
    Cardinal * num_nav_list) ;

static void AddNavigator(
    Widget sf,
    Widget nav,
    Mask dimMask);

static void RemoveNavigator(
    Widget sf,
    Widget nav);

static void UpdateOrigGeom(Widget sf,
    Widget child,
    XtWidgetGeometry *geom);

static void CheckKids(
    XmScrolledWindowWidget sw);

/********    End Static Function Declarations    ********/

/************************************************************************
 *									*
 * Scrolled Window Resources						*
 *									*
 ************************************************************************/

static XtResource resources[] =
{

    {
        XmNhorizontalScrollBar, XmCHorizontalScrollBar,
        XmRWidget, sizeof(Widget),
        XtOffsetOf(XmScrolledWindowRec, swindow.hScrollBar),
        XmRImmediate, NULL
    },
    {
        XmNverticalScrollBar, XmCVerticalScrollBar, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmScrolledWindowRec, swindow.vScrollBar),
        XmRImmediate, NULL
    },
    {
        XmNworkWindow, XmCWorkWindow, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmScrolledWindowRec, swindow.WorkWindow),
        XmRImmediate, NULL
    },
    {
        XmNclipWindow, XmCClipWindow, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmScrolledWindowRec, swindow.ClipWindow),
        XmRImmediate, NULL
    },
    {
        XmNscrollingPolicy, XmCScrollingPolicy, XmRScrollingPolicy,
        sizeof(unsigned char),
        XtOffsetOf(XmScrolledWindowRec, swindow.ScrollPolicy),
        XmRImmediate, (XtPointer)  XmAPPLICATION_DEFINED
    },
    {
        /* Obsolete resource */
        XmNvisualPolicy, XmCVisualPolicy, XmRVisualPolicy,
        sizeof (unsigned char),
        XtOffsetOf(XmScrolledWindowRec, swindow.VisualPolicy),
        XmRCallProc,  (XtPointer) VisualPolicyDefault
    },
    {
        XmNscrollBarDisplayPolicy, XmCScrollBarDisplayPolicy,
        XmRScrollBarDisplayPolicy, sizeof (char),
        XtOffsetOf(XmScrolledWindowRec, swindow.ScrollBarPolicy),
        XmRImmediate,  (XtPointer) RESOURCE_DEFAULT
    },
    {
        XmNscrollBarPlacement, XmCScrollBarPlacement, XmRScrollBarPlacement,
        sizeof (unsigned char),
        XtOffsetOf(XmScrolledWindowRec, swindow.Placement),
        XmRCallProc, (XtPointer) ScrollBarPlacementDefault
    },

    {
        XmNscrolledWindowMarginWidth, XmCScrolledWindowMarginWidth,
        XmRHorizontalDimension, sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, swindow.WidthPad),
        XmRImmediate, (XtPointer) 0
    },
    {
        XmNscrolledWindowMarginHeight, XmCScrolledWindowMarginHeight,
        XmRVerticalDimension, sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, swindow.HeightPad),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, swindow.pad),
        XmRImmediate,  (XtPointer) RESOURCE_DEFAULT
    },

    {
        XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, manager.shadow_thickness),
        XmRImmediate,  (XtPointer) RESOURCE_DEFAULT
    },
    {
        XmNtraverseObscuredCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList),
        XtOffsetOf(XmScrolledWindowRec, swindow.traverseObscuredCallback),
        XmRImmediate, NULL
    },
    {
        XmNautoDragModel, XmCAutoDragModel, XmRAutoDragModel,
        sizeof(XtEnum),
        XtOffsetOf(XmScrolledWindowRec, swindow.auto_drag_model),
        XmRImmediate, (XtPointer) XmAUTO_DRAG_ENABLED
    }
};

static XtResource constraints[] =
{
    {
        XmNscrolledWindowChildType, XmCScrolledWindowChildType,
        XmRScrolledWindowChildType,
        sizeof (unsigned char),
        XtOffsetOf(XmScrolledWindowConstraintRec, swindow.child_type),
        XmRImmediate, (XtPointer) RESOURCE_DEFAULT
    },
};

/****************
 *
 * Resolution independent resources
 *
 ****************/

static XmSyntheticResource syn_resources[] =
{
    {
        XmNscrolledWindowMarginWidth,
        sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, swindow.WidthPad),
        XmeFromHorizontalPixels,
        XmeToHorizontalPixels
    },

    {
        XmNscrolledWindowMarginHeight,
        sizeof (short),
        XtOffsetOf(XmScrolledWindowRec, swindow.HeightPad),
        XmeFromVerticalPixels,
        XmeToVerticalPixels
    },

    {
        XmNspacing,
        sizeof (Dimension),
        XtOffsetOf(XmScrolledWindowRec, swindow.pad),
        XmeFromHorizontalPixels ,
        XmeToHorizontalPixels
    },

};

/****************
 * Actions for the ScrolledWindow.
 * Those actions are called by the ClipWindow when it receives
 * the grabbed events. They might also be called in the context of the
 * SW directly (in the margins).
 ****************/

static XtActionsRec actions[] =
{
    {"SWBeginLine",        LeftEdge},
    {"SWEndLine",          RightEdge},
    {"SWTopLine",          TopEdge},
    {"SWBottomLine",       BottomEdge},
    {"SWLeftPage",         PageLeft},
    {"SWRightPage",        PageRight},
    {"SWUpPage",           PageUp},
    {"SWDownPage",         PageDown},
};

/*******************************************/
/*  Declaration of class extension records */
/*******************************************/

static XmScrolledWindowClassExtRec scrolled_windowClassExtRec =
{
    NULL,
    NULLQUARK,
    XmScrolledWindowClassExtVersion,
    sizeof(XmScrolledWindowClassExtRec),
    GetHorRects,                                  /* auto drag get_hor_rects */
    GetVertRects,                                 /* auto drag get_vert_rects */
};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

externaldef(xmscrolledwindowclassrec) XmScrolledWindowClassRec
xmScrolledWindowClassRec =
{
    {
        /* core_class fields      */
        (WidgetClass) &xmManagerClassRec,         /* superclass         */
        "XmScrolledWindow",                       /* class_name         */
        sizeof(XmScrolledWindowRec),              /* widget_size        */
        NULL,                                     /* class_initialize   */
        ClassPartInitialize,                      /* class_partinit     */
        FALSE,                                    /* class_inited       */
        Initialize,                               /* initialize         */
        NULL,                                     /* Init hook	  */
        XtInheritRealize,                         /* realize            */
        actions,                                  /* actions		  */
        XtNumber(actions),                        /* num_actions	  */
        resources,                                /* resources          */
        XtNumber(resources),                      /* num_resources      */
        NULLQUARK,                                /* xrm_class          */
        TRUE,                                     /* compress_motion	  */
        XtExposeCompressMaximal,                  /* compress_exposure  */
        TRUE,                                     /* compress_enterleave*/
        FALSE,                                    /* visible_interest   */
        Destroy,                                  /* destroy            */
        Resize,                                   /* resize             */
        Redisplay,                                /* expose             */
        SetValues,                                /* set_values         */
        (XtArgsFunc)NULL,                         /* set values hook    */
        XtInheritSetValuesAlmost,                 /* set values almost  */
        NULL,                                     /* get values hook    */
        NULL,                                     /* accept_focus       */
        XtVersion,                                /* Version            */
        NULL,                                     /* PRIVATE cb list    */
        XtInheritTranslations,                    /* tm_table		  */
        QueryGeometry,                            /* query_geometry     */
        NULL,                                     /* display_accelerator*/
        NULL,                                     /* extension          */
    },
    {
        /* composite_class fields */
        GeometryManager,                          /* geometry_manager   */
        ChangeManaged,                            /* change_managed     */
        InsertChild,                              /* insert_child	  */
        DeleteChild,                              /* delete_child	  */
        NULL,                                     /* Extension          */
    }
    ,
    {
        /* Constraint class Init */
        constraints,                              /* resource list        */
        XtNumber(constraints),                    /* num resources        */
        sizeof (XmScrolledWindowConstraintRec),   /* constraint size      */
        ConstraintInitialize,                     /* init proc            */
        NULL,                                     /* destroy proc         */
        NULL,                                     /* set values proc      */
        NULL,
    },
    /* Manager Class */
    {
        defaultTranslations,                      /* translations        */
        syn_resources,                            /* get resources      	  */
        XtNumber(syn_resources),                  /* num syn_resources 	  */
        NULL,                                     /* get_cont_resources     */
        0,                                        /* num_get_cont_resources */
        XmInheritParentProcess,                   /* parent_process         */
        NULL,                                     /* extension           */
    },

    {
        /* Scrolled Window class */
        (XtPointer) &scrolled_windowClassExtRec,  /* auto drag extension */
    }
};

externaldef(xmscrolledwindowwidgetclass) WidgetClass
xmScrolledWindowWidgetClass = (WidgetClass)&xmScrolledWindowClassRec;

/* Trait record for ScrolledWindow */

static const XmScrollFrameTraitRec scrolledWindowSFT =
{
    1,                                            /* version	   */
    ScrollFrameInit,                              /* init		   */
    GetInfo,                                      /* getInfo	   */
    AddNavigator,                                 /* addNavigation   */
    RemoveNavigator,                              /* removeNavigator */
    UpdateOrigGeom,                               /* updateOrigGeom */
};

/*********************************************************************
 *
 * ScrollBarPlacementDefault
 *    This procedure provides the dynamic default behavior for
 *    the scrollbar placement.
 *
 *********************************************************************/

static void
ScrollBarPlacementDefault (Widget widget,
int offset,                                       /* unused */
XrmValue *value)
{
    static unsigned char placement;

    value->addr = (char*) &placement;

    if (LayoutIsRtoLM(((XmScrolledWindowWidget)widget)))
        placement = XmBOTTOM_LEFT;
    else
        placement = XmBOTTOM_RIGHT;

}


/*********************************************************************
 *
 * VisualPolicyDefault
 *    This procedure provides the dynamic default behavior for
 *    the visualPolicy. VisualPolicy is obsolete, the default set it
 *    to whatever is correct (depending on scrollpolicy) and Initialize
 *    checks that this default hasn't changed.
 *
 *********************************************************************/
static void
VisualPolicyDefault(
Widget widget,
int offset,                                       /* unused */
XrmValue *value )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) widget;
    static unsigned char visual_policy ;

    value->addr = (XPointer) &visual_policy;

    if (sw->swindow.ScrollPolicy == XmAUTOMATIC)
        visual_policy = XmCONSTANT ;
    else
        visual_policy = XmVARIABLE ;
}


/************************************************************************
 *									*
 *  SliderMove							        *
 *  Callback for the value changes of navigators.	                *
 *  That's the only place where AUTO scrolledWindow directly moves      *
 *   the clipped kids. Everywhere it uses the navigator API to move it.
 *  This function loops the clip window child list and applies a
 *   move depending on the child_type resource.
 *
 ************************************************************************/
static void
SliderMove(
Widget w,
XtPointer closure,
XtPointer cd )
{
    /* w is a navigator widget */

    XmClipWindowWidget clip = (XmClipWindowWidget) closure;
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) clip->core.parent;
    Cardinal i ;
    XmScrolledWindowConstraint swc;
    Widget child ;
    XmNavigatorDataRec nav_data ;

    /* get the navigator information using the trait getValue since I
       cannot use a callback struct */

    nav_data.valueMask = NavValue ;
    ((XmNavigatorTrait)XmeTraitGet((XtPointer) XtClass(w), XmQTnavigator))
        ->getValue(w, &nav_data) ;

    /* look at the kind of navigator and make the appropriate move */

    if (nav_data.dimMask & NavigDimensionX)
    {
        sw->swindow.hOrigin = nav_data.value.x;

        for (i = 0; i < clip->composite.num_children; i++)
        {
            child = clip->composite.children[i];
            if (XtIsManaged(child) && !child->core.being_destroyed)
            {
                swc = GetSWConstraint(child);

                /* only regular work_area and scroll_hor kids
                   can move in this direction */
                if ((swc->child_type == XmWORK_AREA) ||
                    (swc->child_type == XmSCROLL_HOR))
                {
                    Position real_orig_x ;

                    if (LayoutIsRtoL((Widget)sw))
                        real_orig_x = clip->core.width - swc->orig_x
                            - child->core.width;
                    else
                        real_orig_x = swc->orig_x ;

                    if (LayoutIsRtoLM(sw))
                        XmeConfigureObject(child,
                            real_orig_x + nav_data.value.x,
                            child->core.y,
                            child->core.width,
                            child->core.height,
                            child->core.border_width);
                    else
                        XmeConfigureObject(child,
                            swc->orig_x - nav_data.value.x,
                            child->core.y,
                            child->core.width,
                            child->core.height,
                            child->core.border_width);
                }
            }
        }
    }

    if (nav_data.dimMask & NavigDimensionY)
    {
        sw->swindow.vOrigin = nav_data.value.y;

        for (i = 0; i < clip->composite.num_children; i++)
        {
            child = clip->composite.children[i];
            if (XtIsManaged(child) && !child->core.being_destroyed)
            {
                swc = GetSWConstraint(child);

                /* only regular work_area and scroll_vert kids
                   can move in this direction */
                if ((swc->child_type == XmWORK_AREA) ||
                    (swc->child_type == XmSCROLL_VERT))
                {
                    XmeConfigureObject(child, child->core.x,
                        swc->orig_y - nav_data.value.y,
                        child->core.width,
                        child->core.height,
                        child->core.border_width);
                }
            }
        }
    }

    /* now update the other navigator value */
    _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, False);

}


/************************************************************************
 *									*
 *  MoveWindow	: called for paging only		                *
 *       Set the scrollbar (h or v) value and move the workwindow       *
 *		using the sb callback and deal with traversal           *
 *									*
 ************************************************************************/
static void
MoveWindow(
XmScrolledWindowWidget sw,
int value,
unsigned char orientation)
{
    Widget focus ;
    XtCallbackList tmp ;
    XmNavigatorDataRec nav_data ;

    if (sw->swindow.ScrollPolicy == XmAUTOMATIC)
    {

        /* Save the focus before pagin.bg */
        focus = XmGetFocusWidget((Widget) sw);

        /* possible value for orientation: horizontal or vertical */
        if (orientation == XmHORIZONTAL)
        {
            nav_data.value.x = value ;
            nav_data.dimMask = NavigDimensionX ;
        }
        else
        {
            nav_data.value.y = value ;
            nav_data.dimMask = NavigDimensionY ;
        }

        nav_data.valueMask = NavValue ;
        _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, True);

        /* CR 7095: Don't move focus unless we must. */

        /* Call XmProcessTraversal after having removed the
         * traverseObscuredCallback, so that it isn't called and
         * result in the paging operation going back to origin.
         */
        tmp = sw->swindow.traverseObscuredCallback ;
        sw->swindow.traverseObscuredCallback = NULL ;
        if (focus && XmIsTraversable(focus))
            XmProcessTraversal(focus, XmTRAVERSE_CURRENT);
        else
            XmProcessTraversal(sw->swindow.WorkWindow, XmTRAVERSE_CURRENT);
        sw->swindow.traverseObscuredCallback = tmp ;
    }
}


/************************************************************************
 *                                                                      *
 * LeftEdge - move the view to the left edge                            *
 *                                                                      *
 ************************************************************************/
static void
LeftEdge(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (!sw->swindow.hScrollBar) return ;

    MoveWindow (sw, sw->swindow.hmin,
        sw->swindow.hScrollBar->scrollBar.orientation);
}


/************************************************************************
 *                                                                      *
 * RightEdge - move the view to the Right edge                          *
 *                                                                      *
 ************************************************************************/
static void
RightEdge(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (!sw->swindow.hScrollBar) return ;

    MoveWindow (sw, sw->swindow.hmax - sw->swindow.hExtent,
        sw->swindow.hScrollBar->scrollBar.orientation);
}


/************************************************************************
 *                                                                      *
 * TopEdge - move the view to the Top edge                              *
 *                                                                      *
 ************************************************************************/
static void
TopEdge(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (!sw->swindow.vScrollBar) return ;

    MoveWindow (sw, sw->swindow.vmin,
        sw->swindow.vScrollBar->scrollBar.orientation);
}


/************************************************************************
 *                                                                      *
 * BottomEdge - move the view to the Bottom edge                        *
 *                                                                      *
 ************************************************************************/
static void
BottomEdge(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (!sw->swindow.vScrollBar) return ;

    MoveWindow (sw, sw->swindow.vmax - sw->swindow.vExtent,
        sw->swindow.vScrollBar->scrollBar.orientation);
}


/************************************************************************
 *                                                                      *
 * PageLeft - Scroll left a page                                        *
 *                                                                      *
 ************************************************************************/
static void
PageLeft(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    int value ;

    if (sw->swindow.hScrollBar)
        value = sw->swindow.hOrigin -
            (sw->swindow.hScrollBar)->scrollBar.page_increment;
    else {
        if (sw->swindow.WorkWindow)
            value = sw->swindow.hOrigin -
            sw->swindow.WorkWindow->core.width;
        else
            return;
    }

    MoveWindow (sw, MAX(value, sw->swindow.hmin), XmHORIZONTAL);
}


/************************************************************************
 *                                                                      *
 * PageRight - Scroll Right a page                                      *
 *                                                                      *
 ************************************************************************/
static void
PageRight(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    int value ;

    if (sw->swindow.hScrollBar)
        value = sw->swindow.hOrigin +
            (sw->swindow.hScrollBar)->scrollBar.page_increment;
    else {
        if (sw->swindow.WorkWindow)
            value = sw->swindow.hOrigin + sw->swindow.WorkWindow->core.width;
        else
            return;
    }

    MoveWindow (sw, MIN(value, (sw->swindow.hmax - sw->swindow.hExtent)),
        XmHORIZONTAL);
}


/************************************************************************
 *                                                                      *
 * PageUp - Scroll up a page                                            *
 *                                                                      *
 ************************************************************************/
static void
PageUp(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    int value ;

    if (sw->swindow.vScrollBar)
        value = sw->swindow.vOrigin -
            (sw->swindow.vScrollBar)->scrollBar.page_increment;
    else {
        if (sw->swindow.WorkWindow)
            value = sw->swindow.vOrigin - sw->swindow.WorkWindow->core.height;
        else
            return;
    }

    MoveWindow (sw, MAX(value, sw->swindow.vmin), XmVERTICAL);
}


/************************************************************************
 *                                                                      *
 * PageDown - Scroll Down a page                                        *
 *                                                                      *
 ************************************************************************/
static void
PageDown(
Widget wid,
XEvent *event,
String *params,
Cardinal *num_params )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    int value ;

    if (sw->swindow.vScrollBar)
        value = sw->swindow.vOrigin +
            (sw->swindow.vScrollBar)->scrollBar.page_increment;
    else {
        if (sw->swindow.WorkWindow)
            value = sw->swindow.vOrigin + sw->swindow.WorkWindow->core.height;
        else
            return;
    }

    MoveWindow (sw, MIN(value, (sw->swindow.vmax - sw->swindow.vExtent)),
        XmVERTICAL);

}


/************************************************************************
 *									*
 *  ClassPartInitialize - Set up the fast subclassing.			*
 *									*
 ************************************************************************/
static void
ClassPartInitialize(
WidgetClass wc )
{
    XmScrolledWindowWidgetClass swc = (XmScrolledWindowWidgetClass) wc;
    XmScrolledWindowWidgetClass super =
        (XmScrolledWindowWidgetClass) wc->core_class.superclass;
    XmScrolledWindowClassExt         *wcePtr, *scePtr;

    /* Inheritance of the auto drag drop site rectangles */

    wcePtr = _XmGetScrolledWindowClassExtPtr(swc, NULLQUARK);

    if (((WidgetClass)swc != xmScrolledWindowWidgetClass) && (*wcePtr))
    {

        scePtr = _XmGetScrolledWindowClassExtPtr(super, NULLQUARK);

        if ((*wcePtr)->get_hor_rects == XmInheritGetAutoDragRectsProc)
            (*wcePtr)->get_hor_rects = (*scePtr)->get_hor_rects;

        if ((*wcePtr)->get_vert_rects == XmInheritGetAutoDragRectsProc)
            (*wcePtr)->get_vert_rects = (*scePtr)->get_vert_rects;

    }

    _XmFastSubclassInit (wc, XmSCROLLED_WINDOW_BIT);

    /* Install the scrollFrame trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTscrollFrame, (XtPointer)&scrolledWindowSFT);
}


/************************************************************************
 *									*
 *  Initialize								*
 *									*
 ************************************************************************/
static void
Initialize(
Widget rw,
Widget nw,
ArgList args,
Cardinal *num_args )
{
    XmScrolledWindowWidget request = (XmScrolledWindowWidget) rw ;
    XmScrolledWindowWidget new_w = (XmScrolledWindowWidget) nw ;
    Cardinal n;
    Arg loc_args[5] ;

    /****************
     *
     *  Check validity
     *
     ****************/

    if(!XmRepTypeValidValue( XmRID_SCROLLING_POLICY,
        new_w->swindow.ScrollPolicy, (Widget) new_w)    )
    {
        new_w->swindow.ScrollPolicy = XmAPPLICATION_DEFINED;
    }

    /* VisualPolicy has already been set using ScrollPolicy in the
       default proc, check that no one changed it */
    if(!XmRepTypeValidValue( XmRID_VISUAL_POLICY,
        new_w->swindow.VisualPolicy, (Widget) new_w)    )
    {
        if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
            new_w->swindow.VisualPolicy = XmCONSTANT;
        else
            new_w->swindow.VisualPolicy = XmVARIABLE;
    }

    if ((new_w->swindow.ScrollPolicy == XmAPPLICATION_DEFINED) &&
        (new_w->swindow.VisualPolicy != XmVARIABLE))
    {
        XmeWarning( (Widget) new_w, SWMessage11);
        new_w->swindow.VisualPolicy = XmVARIABLE;
    }

    /* force constant & automatic */
    if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
        new_w->swindow.VisualPolicy = XmCONSTANT;

    if (new_w->swindow.ScrollBarPolicy == (unsigned char)RESOURCE_DEFAULT)
    {
        if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
            new_w->swindow.ScrollBarPolicy = XmAS_NEEDED;
        else
            new_w->swindow.ScrollBarPolicy = XmSTATIC;
    }

    if(!XmRepTypeValidValue(XmRID_SCROLL_BAR_DISPLAY_POLICY,
        new_w->swindow.ScrollBarPolicy, (Widget) new_w) )
    {
        if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
            new_w->swindow.ScrollBarPolicy = XmAS_NEEDED;
        else
            new_w->swindow.ScrollBarPolicy = XmSTATIC;
    }

    if ((new_w->swindow.VisualPolicy == XmVARIABLE) &&
        (request->swindow.ScrollBarPolicy == XmAS_NEEDED))
    {
        XmeWarning( (Widget) new_w, SWMessage8);
        new_w->swindow.ScrollBarPolicy = XmSTATIC;
    }

    if(!XmRepTypeValidValue( XmRID_SCROLL_BAR_PLACEMENT,
        new_w->swindow.Placement, (Widget) new_w)    )
    {
        new_w->swindow.Placement = XmBOTTOM_RIGHT;
    }

    if (new_w->swindow.pad == (Dimension)RESOURCE_DEFAULT)
        new_w->swindow.pad = DEFAULT_SPACING;

    if (request->manager.shadow_thickness == (Dimension)RESOURCE_DEFAULT)
    {
        if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
        {
            new_w->manager.shadow_thickness = 2;
        }
        else
        {
            new_w->manager.shadow_thickness = 0;
        }
    }

    /****************
     *
     * Init internal fields now
     *
     ****************/

    new_w->swindow.FromResize = FALSE;

    /* the mins actually never change, the others, max, extent, etc,
       get update the first time the SW is resized */
    new_w->swindow.hmin = 0 ;
    new_w->swindow.vmin = 0 ;

    /****************
     *
     * We will set the X and Y offsets to the pad values. That lets us use
     * the four variables as a "margin" to the user, but MainWindow can still
     * dink around with them if it wants.
     *
     ****************/
    new_w->swindow.XOffset = new_w->swindow.WidthPad;
    new_w->swindow.YOffset = new_w->swindow.HeightPad;

    /* these fields are used to find out if the object need
       to be reposition according to the scrollbars minima. */
    /* that happens the first time and
       when the scrollbars disappears too */
    new_w->swindow.sw_prev_x = MAXPOS;
    new_w->swindow.sw_prev_y = MAXPOS;

    /****************
     *
     * Add our class translation table
     *
     ****************/
    XtAugmentTranslations((Widget) new_w, (XtTranslations)
        ((XmManagerClassRec *)XtClass(new_w))->
        manager_class.translations);

    /***********************
      Initialize the auto_drag timer */

    new_w->swindow.auto_drag_timer = 0 ;

    /***********************
      Initialize the auto_drag rectangles */

    if (new_w->swindow.auto_drag_model == XmAUTO_DRAG_ENABLED)
        new_w->swindow.auto_drag_rects = XtCalloc(1, sizeof(AutoDragRectsRec));
    else
        new_w->swindow.auto_drag_rects = NULL;

    /***********************
      Initialize the scroll_frame flag */

    new_w->swindow.scroll_frame_inited = False;

    /****************
     *
     * APPLICATION_DEFINED : we're almost done.
     *
     ****************/
    if (new_w->swindow.ScrollPolicy == XmAPPLICATION_DEFINED)
    {
        Dimension future_width, future_height ;   /* without kids */

        new_w->swindow.InInit = False;

        /* Need to default AreaWidth&Height to something that makes
           sense, since they might be used in Redisplay before layout
           if no kid is present in the SW.
           They will be changed if the SW gets a child.
           If the SW doesn't get a child, its size will be DEFAULT_SIZE if
           it hasn't one already (if it is null). */

        if (new_w->core.width == 0) future_width = DEFAULT_SIZE ;
        else future_width = new_w->core.width ;
        if (new_w->core.height == 0) future_height = DEFAULT_SIZE ;
        else future_height = new_w->core.height ;

        if (future_width > new_w->manager.shadow_thickness*2)
            new_w->swindow.AreaWidth = future_width
                - (new_w->manager.shadow_thickness * 2);
        else
            new_w->swindow.AreaWidth = 2 ;

        if (future_height > new_w->manager.shadow_thickness*2)
            new_w->swindow.AreaHeight = future_height
                - (new_w->manager.shadow_thickness * 2);
        else
            new_w->swindow.AreaHeight = 2 ;

        /* In app defined, we do not initialize the scrollframe,
           so that List and Text could check that it
           hasn't been done yet and do it */

        return;
    }

    /***************--------------------------***********************
     *
     * Else we're in AUTOMATIC, and we are about to create private
     *  children, so set up a flag for use in InsertChild
     *
     *****************/

    new_w->swindow.InInit = True;

    /* Default dimension here, not in ChangeManaged because
       auto mode isn't based on children size */
    if (new_w->core.width == 0) new_w->core.width = DEFAULT_SIZE ;
    if (new_w->core.height == 0) new_w->core.height = DEFAULT_SIZE ;

    /* default Area fields too, see above comments */
    if (new_w->core.width > new_w->manager.shadow_thickness*2)
        new_w->swindow.AreaWidth = new_w->core.width
            - (new_w->manager.shadow_thickness * 2);
    else
        new_w->swindow.AreaWidth = 2 ;

    if (new_w->core.height > new_w->manager.shadow_thickness*2)
        new_w->swindow.AreaHeight = new_w->core.height
            - (new_w->manager.shadow_thickness * 2);
    else
        new_w->swindow.AreaHeight = 2 ;

    /****************
     *
     * create a clip window.
     *
     ****************/

    n = 0 ;
    XtSetArg (loc_args[n], XmNscrolledWindowChildType, XmCLIP_WINDOW); n++ ;
    /* give an initial size to the clipwindow, since when layoutDirection
       is reversed, the clipwindow size is used from ConstraintInitialize
       (before the first layout, which resizes the clipwindow) */
    XtSetArg (loc_args[n], XmNwidth, new_w->swindow.AreaWidth); n++ ;
    XtSetArg (loc_args[n], XmNheight, new_w->swindow.AreaHeight); n++ ;
    new_w->swindow.ClipWindow = (XmDrawingAreaWidget)
        XtCreateManagedWidget("ClipWindow", xmClipWindowWidgetClass,
        (Widget) new_w, loc_args, n);

    /***********************
      Initialize the scrollFrame with move callback for Automatic */

    ((XmScrollFrameTrait)
        XmeTraitGet((XtPointer) XtClass(nw), XmQTscrollFrame))->
        init(nw, SliderMove, (Widget) new_w->swindow.ClipWindow);

    /****************
     *
     *  create the scroll bars.
     *  location and size don't matter right now -
     *  we'll figure them out for real at changemanaged time.
     *
     *  The trait AddNavigator method will be set up in
     *  InsertChild so that APP_DEFINED (ScrolledList, Text, etc)
     *  get it for their scrollbars as well.
     *
     ****************/

    /**********************
     * vertical scrollbar */

    n = 0 ;
    XtSetArg (loc_args[n], XmNorientation, XmVERTICAL); n++;
    new_w->swindow.vScrollBar = (XmScrollBarWidget)
        XtCreateManagedWidget("VertScrollBar", xmScrollBarWidgetClass,
        (Widget) new_w, loc_args, n);

    /************************
     * horizontal scrollbar */

    n = 0 ;
    XtSetArg (loc_args[n], XmNorientation, XmHORIZONTAL); n++;
    new_w->swindow.hScrollBar = (XmScrollBarWidget)
        XtCreateManagedWidget("HorScrollBar", xmScrollBarWidgetClass,
        (Widget) new_w, loc_args, n);

    /* could set childType for the scrollbar here too, but it's more
       convenient to do that in InsertChild, since app defined scrollbar
       are only available down there */

    /* turn off the InsertChild flag */
    new_w->swindow.InInit = FALSE;
}


/************************************************************************
 *									*
 *  Redisplay - General redisplay function called on exposure events.	*
 *									*
 ************************************************************************/
static void
Redisplay(
Widget wid,
XEvent *event,
Region region )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    Dimension st, bw;

    if (!XtIsRealized (wid)) return ;             /* get rid of that when GM redone*/

    st = sw->manager.shadow_thickness;
    if (sw->swindow.ScrollPolicy == XmAUTOMATIC)
    {
        XmeDrawShadows (XtDisplay (sw), XtWindow (sw),
            sw -> manager.bottom_shadow_GC,
            sw -> manager.top_shadow_GC,
            sw->swindow.ClipWindow->core.x - st,
            sw->swindow.ClipWindow->core.y - st,
            sw -> swindow.AreaWidth + (st * 2),
            sw -> swindow.AreaHeight + (st * 2),
            sw->manager.shadow_thickness,
            XmSHADOW_OUT);
    }
    else
    {
        if (sw->swindow.WorkWindow)
        {
            bw = sw->swindow.WorkWindow->core.border_width;
            XmeDrawShadows (XtDisplay (sw), XtWindow (sw),
                sw -> manager.bottom_shadow_GC,
                sw -> manager.top_shadow_GC,
                sw->swindow.WorkWindow->core.x - st,
                sw->swindow.WorkWindow->core.y - st,
                sw -> swindow.AreaWidth + ((bw + st) * 2),
                sw -> swindow.AreaHeight + ((bw + st) * 2),
                sw->manager.shadow_thickness,
                XmSHADOW_OUT);
        }
        else
            XmeDrawShadows (XtDisplay (sw), XtWindow (sw),
                sw -> manager.bottom_shadow_GC,
                sw -> manager.top_shadow_GC,
                0, 0,
                sw -> swindow.AreaWidth + (st * 2),
                sw -> swindow.AreaHeight + (st * 2),
                sw->manager.shadow_thickness,
                XmSHADOW_OUT);
    }

    XmeRedisplayGadgets(wid, event, region);

}


/************************************************************************
 *									*
 *  InsertChild: set internal pointers					*
 *									*
 ************************************************************************/
static void
InsertChild(
Widget w )
{

    XmManagerWidgetClass superclass =
        (XmManagerWidgetClass) xmManagerWidgetClass;
    XmScrolledWindowWidget   sw = (XmScrolledWindowWidget)w->core.parent ;
    XmScrolledWindowConstraint swc = GetSWConstraint(w);
    Arg args[5] ;
    Cardinal n ;
    XtWidgetProc insert_child;

    if (!XtIsRectObj(w)) return;

    _XmProcessLock();
    insert_child = superclass->composite_class.insert_child;
    _XmProcessUnlock();

    /****************
     * Manage the child type defaulting first -
     * for auto created children (clip/scrollbars/sep) as well
     *
     ****************/
    if (swc->child_type == (unsigned char) RESOURCE_DEFAULT)
    {

        if (XmIsScrollBar(w))
        {
            if (((XmScrollBarWidget) w)->scrollBar.orientation
                == XmHORIZONTAL)
            {
                swc->child_type = XmHOR_SCROLLBAR ;
            }
            else
            {
                swc->child_type = XmVERT_SCROLLBAR ;
            }
        }
        else
        {
            swc->child_type = XmWORK_AREA ;
        }
    }

    if ((swc->child_type == XmHOR_SCROLLBAR) ||
        (swc->child_type == XmVERT_SCROLLBAR))
    {
        /************************/
        /* Deal with the Scrollbar auto drag drop site register here, */
        /* create the dropsite only if resource says so */
        /* The model is: the scrollbars are used as drop site,
           with 2 rectangles around the workarea, defined
           upon resize using XmDropSiteUpdate in UpdateAutoDrag */

        /* this is done for both AUTO and APP_DEFINED scrollbars */

        if (sw->swindow.auto_drag_model == XmAUTO_DRAG_ENABLED)
        {
            n = 0 ;
            XtSetArg(args[n], XmNdropProc, HandleDrop); n++ ;
            XtSetArg(args[n], XmNdragProc, HandleDrag); n++ ;
            XtSetArg(args[n], XmNnumImportTargets, 1); n++ ;
            XtSetArg(args[n], XmNimportTargets, w); n++ ;
            XmDropSiteRegister(w, args, n);
        }
    }

    /************************
      Do the navigator association now.
      ************************/
    if (swc->child_type == XmHOR_SCROLLBAR)
        ((XmScrollFrameTrait)
        XmeTraitGet((XtPointer) XtClass((Widget)(sw)), XmQTscrollFrame))->
        addNavigator((Widget)sw, w, NavigDimensionX);
    else
    if (swc->child_type == XmVERT_SCROLLBAR)
        ((XmScrollFrameTrait)
        XmeTraitGet((XtPointer) XtClass((Widget)(sw)), XmQTscrollFrame))->
        addNavigator((Widget)sw, w, NavigDimensionY);

    /************************
      Check if auto created child (Clipwindow, Scrollbars)
       if yes, insert using superclass method */
    if (sw->swindow.InInit)
    {
        (*insert_child)(w);
    }
    else
    {

        /****************
         *
         * APP_DEFINED, handle scrollbars case, set the workwindow if not
         *         already set.
         *
         ****************/

        if (sw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED)
        {

            if (swc->child_type == XmHOR_SCROLLBAR)
                sw->swindow.hScrollBar = (XmScrollBarWidget) w;
            else
            if (swc->child_type == XmVERT_SCROLLBAR)
                sw->swindow.vScrollBar = (XmScrollBarWidget) w;
            else
            if (swc->child_type == XmWORK_AREA)
            {
                if (!sw->swindow.WorkWindow) sw->swindow.WorkWindow = w;
            }
        }

        /****************
         *
         *  AUTOMATIC: set the workwindow and reparent it to the clipwindow .
         *
         ****************/
        else
        {
            /* maintain the old workwindow semantics */
            if (swc->child_type == XmWORK_AREA) sw->swindow.WorkWindow = w;

            /* reparent all the clipped type children */
            if ((swc->child_type == XmWORK_AREA) ||
                (swc->child_type == XmSCROLL_HOR) ||
                (swc->child_type == XmSCROLL_VERT) ||
                (swc->child_type == XmNO_SCROLL))
            {
                w->core.parent = (Widget )sw->swindow.ClipWindow;
            }
        }

        /* call insert child now. For the reparented workarea, that will
           insert it in the clipwindow child list */

        (*insert_child)(w);

        /* Widgets already there might still be hanging around,
           it is the responsability of the application to destroy
           or unmanage or unmap them */
    }

}


/************************************************************************
 *									*
 *  DeleteChild: reset internal pointers to NULL			*
 *									*
 ************************************************************************/
static void
DeleteChild(
Widget child )
{
    Widget swWid = XtParent(child);
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) swWid;
    XtWidgetProc      delete_child;

    if (child == sw->swindow.WorkWindow)
        sw->swindow.WorkWindow = NULL;
    /* auto drag will stop by itself when destroying these */
    if (child == (Widget) sw->swindow.hScrollBar)
        sw->swindow.hScrollBar = NULL;
    if (child == (Widget) sw->swindow.vScrollBar)
        sw->swindow.vScrollBar = NULL;

    /* Clean up the Navigator.  Metro Link fix, suggested by
     * Kevin B. Hendricks of Java-Linux project. */
    if (XmeTraitGet((XtPointer) XtClass(child), XmQTnavigator) != NULL)
      RemoveNavigator(swWid, child);

    _XmProcessLock();
    delete_child =
        ((CompositeWidgetClass)xmScrolledWindowClassRec.core_class.superclass)
        ->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)(child);
}


/************************************************************************
 *									*
 * ComputeLocations - position the scrollbars and the workarea		*
 *       using the scrollbar placement resource				*
 *	Used in both Variable and Constant layout			*
 *									*
 ************************************************************************/
static void
ComputeLocations(
XmScrolledWindowWidget sw,
Dimension HSBht, Dimension VSBht,
Boolean HasHSB, Boolean HasVSB,
Position * newx, Position * newy,
Position * hsbX, Position * hsbY,
Position * vsbX, Position * vsbY)
{
    Dimension shad = sw->manager.shadow_thickness,
        pad = sw->swindow.pad,
        vsb_width, hsb_height;

    vsb_width = (HasVSB) ? (sw->swindow.vScrollBar->core.width) : 0;

    hsb_height = (HasHSB) ? (sw->swindow.hScrollBar->core.height) : 0;

    /****************
     *
     * Initialize the placement variables - these are correct for
     * bottom-right placement of the scrollbars.
     *
     ****************/
    *newx = sw->swindow.XOffset + shad + HSBht;
    *newy = sw->swindow.YOffset + shad + VSBht;
    *hsbX = sw->swindow.XOffset;
    *vsbY = sw->swindow.YOffset;
    *vsbX = (HasVSB) ? (sw->core.width -  sw->swindow.WidthPad -
        vsb_width) : sw->core.width;
    *hsbY = (HasHSB) ? (sw->core.height - sw->swindow.HeightPad -
        hsb_height): sw->core.height;
    /****************
     *
     * Check out the scrollbar placement policy and hack the locations
     * accordingly.
     *
     ****************/
    switch (sw->swindow.Placement)
    {
        case XmTOP_LEFT:
            *newx = (HasVSB) ? (vsb_width +
                sw->swindow.XOffset + pad + shad +
                HSBht)
                : (sw->swindow.XOffset + shad + HSBht);

            *newy = (HasHSB) ? (hsb_height +
                sw->swindow.YOffset + pad + shad +
                VSBht)
                : (sw->swindow.YOffset + shad + VSBht);

            *hsbX = *newx - HSBht - shad;
            *hsbY = sw->swindow.YOffset;
            *vsbX = sw->swindow.XOffset;
            *vsbY = *newy - VSBht - shad;
            break;
        case XmTOP_RIGHT:
            *newy = (HasHSB) ? (hsb_height +
                sw->swindow.YOffset + pad + shad +
                VSBht)
                : (sw->swindow.YOffset + shad + VSBht);

            *vsbY = *newy - shad - VSBht;
            *hsbY = sw->swindow.YOffset;
            break;
        case XmBOTTOM_LEFT:
            *newx = (HasVSB) ? (vsb_width +
                sw->swindow.XOffset + pad + shad +
                HSBht)
                : (sw->swindow.XOffset + shad + HSBht);
            *hsbX = *newx - HSBht - shad;
            *vsbX = sw->swindow.XOffset;
            break;
        default:
            break;
    }
}


static void
CheckKids(
XmScrolledWindowWidget sw )
{

    /* do a sanity check */
    if( sw->swindow.WorkWindow != NULL &&
        sw->swindow.WorkWindow->core.being_destroyed )
    {
        sw->swindow.WorkWindow = NULL;
    }
    if( sw->swindow.hScrollBar != NULL &&
        sw->swindow.hScrollBar->core.being_destroyed )
    {
        sw->swindow.hScrollBar = NULL;
    }
    if( sw->swindow.vScrollBar != NULL &&
        sw->swindow.vScrollBar->core.being_destroyed )
    {
        sw->swindow.vScrollBar = NULL;
    }
}


/************************************************************************
 *									*
 * VariableLayout - Layout the workarea and the scrollbars for    	*
 *      APPLICATION_DEFINED. ScrolledWindow has a size already.    	*
 *   APP_DEFINED SW still use sw->swindow.WorkWindow			*
 ************************************************************************/
static void
VariableLayout(
XmScrolledWindowWidget sw )
{
    Position     newx, newy, vsbX, vsbY, hsbX, hsbY;
    Dimension HSBht = 0, VSBht = 0, VSBwidth = 0, HSBheight = 0;
    Dimension MyWidth, MyHeight, childWidth, childHeight;
    Dimension shad = sw->manager.shadow_thickness,
        pad = sw->swindow.pad, bw;
    int tmp;
    Boolean HasHSB, HasVSB, HSBTrav = True, VSBTrav = True;
    XtWidgetGeometry  desired, preferred;

    CheckKids(sw);

    /* no Workwindow, make scrollbars invisible --
       Doesn't seem right to do that given app_defined policy... */
    if (!ExistManaged(sw->swindow.WorkWindow))
    {
        if (sw->swindow.vScrollBar)
            XmeConfigureObject((Widget) sw->swindow.vScrollBar,
                sw->core.width, 0,
                (sw->swindow.vScrollBar)->core.width,
                (sw->swindow.vScrollBar)->core.height,
                (sw->swindow.vScrollBar)->core.border_width);

        if (sw->swindow.hScrollBar)
            XmeConfigureObject((Widget) sw->swindow.hScrollBar,
                0, sw->core.height,
                (sw->swindow.hScrollBar)->core.width,
                (sw->swindow.hScrollBar)->core.height,
                (sw->swindow.hScrollBar)->core.border_width);
        return;
    }

    bw = sw->swindow.WorkWindow->core.border_width ;

    /* Compute the available size after a subclass, like mainwindow,
       has taken its share of layout. */

    /* worry about going negative */
    tmp = (int) sw->core.width - sw->swindow.XOffset - sw->swindow.WidthPad;
    if (tmp <= 0)
        MyWidth = 10;
    else
        MyWidth = (Dimension )tmp;
    tmp = (int) sw->core.height - sw->swindow.YOffset - sw->swindow.HeightPad;
    if (tmp <= 0)
        MyHeight = 10;
    else
        MyHeight = (Dimension )tmp;

    /* CR 5319 begin */
    if (sw->core.width > sw->swindow.WidthPad + shad*2)
        sw->swindow.AreaWidth = sw->core.width - sw->swindow.WidthPad
            - ((shad * 2) );
    else
        sw->swindow.AreaWidth = 0;
    if (sw->core.height > sw->swindow.HeightPad + shad*2)
        sw->swindow.AreaHeight = sw->core.height - sw->swindow.HeightPad
            - ((shad * 2) );
    else
        sw->swindow.AreaHeight = 0;
    /* CR 5319 end */

    tmp = (int) MyWidth - (((int )shad+(int )bw) * 2);
    if (tmp <= 0)
        childWidth = 2;
    else
        childWidth = (Dimension )tmp;

    tmp = (int )MyHeight - (((int )shad+(int )bw) * 2);
    if (tmp <= 0)
        childHeight = 2;
    else
        childHeight = (Dimension )tmp;

    /****************
     *
     * OK, we just figured out the maximum size for the child (with no
     * scrollbars (it's in childWidth, childHeight). Now query the
     * kid geometry - tell him that we are going to resize him to childWidth,
     * childHeight and give him a chance to muck with the scrollbars or whatever.
     * Then, we re-layout according to the current scrollbar config.
     *
     ****************/

    desired.x = shad + sw->swindow.XOffset;
    desired.y = shad + sw->swindow.YOffset;
    desired.border_width = bw;
    desired.height = childHeight;
    desired.width = childWidth;
    desired.request_mode = (CWWidth | CWHeight);

    /* child may manage or unmanage the scrollbars, set a flag up
       and check for it in ChangeManaged */
    sw->swindow.FromResize = TRUE;

    XtQueryGeometry(sw->swindow.WorkWindow, &desired, &preferred);

    sw->swindow.FromResize = FALSE;

    bw = preferred.border_width;

    HasHSB = ExistManaged((Widget) sw->swindow.hScrollBar);
    HasVSB = ExistManaged((Widget) sw->swindow.vScrollBar);

    if (HasHSB)
    {
        HSBht = sw->swindow.hScrollBar->primitive.highlight_thickness ;
        HSBTrav = sw->swindow.hScrollBar->primitive.traversal_on ;
        HSBheight = sw->swindow.hScrollBar->core.height ;
    }

    if (HasVSB)
    {
        VSBht = sw->swindow.vScrollBar->primitive.highlight_thickness ;
        VSBTrav = sw->swindow.vScrollBar->primitive.traversal_on ;
        VSBwidth = sw->swindow.vScrollBar->core.width ;
    }

    /****************
     *
     * Here's a cool undocumented feature. If the scrollbar is not
     * traversable,but has a highlight thickness, we assume it's something
     * that wants to draw the highlight around the workwindow, and wants to
     * have the scrollbars line up properly. Something like Scrolled Text,
     * for instance :-)
     *
     ****************/
    if (ExistManaged(sw->swindow.WorkWindow) &&
        XmIsPrimitive(sw->swindow.WorkWindow))
    {
        if (HSBht && !HSBTrav) HSBht = 0;
        if (VSBht && !VSBTrav) VSBht = 0;
    }

    /****************
     *
     * Get the position for scrollbars and workarea
     *
     ****************/
    ComputeLocations(sw,
        HSBht, VSBht, HasHSB, HasVSB,
        &newx, &newy, &hsbX, &hsbY, &vsbX, &vsbY);

    /* reset the child sizes */

    childWidth = (HasVSB) ? MyWidth - (VSBwidth +
        (2 * VSBht) + SB_BORDER_WIDTH + ((HSBht + bw + shad)*2) + pad )
        : MyWidth - ((HSBht + shad+bw) * 2);

    childHeight = (HasHSB) ? MyHeight - (HSBheight +
        (2 * HSBht) + SB_BORDER_WIDTH + ((VSBht + shad + bw)*2) + pad )
        : MyHeight - ((VSBht + shad+bw) * 2);

    if (childWidth > MyWidth) childWidth = 2;
    if (childHeight > MyHeight) childHeight = 2;

    XmeConfigureObject(sw->swindow.WorkWindow, newx, newy,
        childWidth, childHeight, bw);

    /* not needed here, because never auto, but we leave it for now */
    if (sw->swindow.ClipWindow)
        XmeConfigureObject((Widget) sw->swindow.ClipWindow, newx,
            newy, childWidth, childHeight, 0);

    sw->swindow.AreaWidth = childWidth;
    sw->swindow.AreaHeight = childHeight;

    /* always forces one scrollbar dimension */
    sw->swindow.hsbWidth = childWidth  + ((HSBht + shad + bw) * 2);
    sw->swindow.vsbHeight = childHeight  + ((VSBht + shad + bw) * 2);

    if (HasVSB)
        XmeConfigureObject((Widget) sw->swindow.vScrollBar, vsbX, vsbY,
            VSBwidth, sw->swindow.vsbHeight,
            SB_BORDER_WIDTH);

    if (HasHSB)
        XmeConfigureObject((Widget) sw->swindow.hScrollBar, hsbX, hsbY,
            sw->swindow.hsbWidth, HSBheight,
            SB_BORDER_WIDTH);

}


/************************************************************************
 *									*
 * ConstantLayout - Layout the scrolled window for AUTOMATIC		*
 *									*
 ************************************************************************/
static void
ConstantLayout(
XmScrolledWindowWidget sw )
{
    XmDrawingAreaWidget clip = (XmDrawingAreaWidget) sw->swindow.ClipWindow ;
    int i, hinc, hpage_inc, vinc, vpage_inc;
    Position newx, newy;
    Position hsbX, hsbY, vsbX, vsbY, clipX, clipY;
    int clipWidth, clipHeight;
    int MyWidth, MyHeight;
    Dimension  HSBht, VSBht,
        shad = sw->manager.shadow_thickness,
        pad = sw->swindow.pad;
    Boolean HasHSB, HasVSB, HSBExists, VSBExists, no_scroll_present = False;
    XmScrolledWindowConstraint swc;
    Widget child ;
    XmNavigatorDataRec nav_data ;

    /* Compute the available size after a subclass, like mainwindow,
       has taken its share of layout. */

    MyWidth = sw->core.width - sw->swindow.XOffset  - sw->swindow.WidthPad;
    MyHeight = sw->core.height - sw->swindow.YOffset  - sw->swindow.HeightPad;

    HSBExists = ExistManaged((Widget) sw->swindow.hScrollBar);
    VSBExists = ExistManaged((Widget) sw->swindow.vScrollBar);

    /* If there's no kid to clip, keep the scrollbars invisible
       by moving them out of the frame */

    for (i = 0; i < clip->composite.num_children; i++)
    {
        if (XtIsManaged(clip->composite.children[i])) break ;
    }

    /* ran thru the entire list: no managed child in the clipwindow */
    if (i == clip->composite.num_children)
    {
        if (VSBExists)
        {
            XmeConfigureObject((Widget) sw->swindow.vScrollBar,
                sw->core.width,
                sw->swindow.vScrollBar->core.y,
                (sw->swindow.vScrollBar)->core.width,
                (sw->swindow.vScrollBar)->core.height,
                (sw->swindow.vScrollBar)->core.border_width);
            sw->swindow.vsbX = sw->swindow.vScrollBar->core.x;
        }
        if (HSBExists)
        {
            XmeConfigureObject((Widget) sw->swindow.hScrollBar,
                sw->swindow.hScrollBar->core.x,
                sw->core.height,
                (sw->swindow.hScrollBar)->core.width,
                (sw->swindow.hScrollBar)->core.height,
                (sw->swindow.hScrollBar)->core.border_width);
            sw->swindow.hsbY = sw->swindow.hScrollBar->core.y;
        }

        /* looks bogus, but who care of this extreme case */
        sw->swindow.AreaWidth = sw->core.width  -
            sw->swindow.WidthPad - ((shad * 2) );
        sw->swindow.AreaHeight = sw->core.height -
            sw->swindow.HeightPad - ((shad * 2) );

        XmeConfigureObject((Widget) clip,
            shad + sw->swindow.XOffset,
            shad + sw->swindow.YOffset,
            sw->swindow.AreaWidth, sw->swindow.AreaHeight, 0);
        return;
    }

    HSBht = (HSBExists) ?
        sw->swindow.hScrollBar->primitive.highlight_thickness : 0;
    VSBht = (VSBExists) ?
        sw->swindow.vScrollBar->primitive.highlight_thickness : 0;

    /* find the new min x and y children orig position and the
       new max width and height (position+dimension) stuff as well */

    newx = MAXPOS;
    newy = MAXPOS;
    sw->swindow.hmax = 0;
    sw->swindow.vmax = 0;

    for (i = 0; i < clip->composite.num_children; i++)
    {
        child = clip->composite.children[i];
        if (ExistManaged(child))
        {
            swc = GetSWConstraint(child);

            /* only regular work_area and scroll_vert kids
               count for the vmax calculation */
            if ((swc->child_type == XmWORK_AREA) ||
                (swc->child_type == XmSCROLL_VERT))
            {
                newy = MIN(newy, swc->orig_y);
                sw->swindow.vmax = MAX(sw->swindow.vmax,
                    swc->orig_y + child->core.height
                    + 2*child->core.border_width);
            }

            /* only regular work_area and scroll_hor kids
               count for the hmax calculation */
            if ((swc->child_type == XmWORK_AREA) ||
                (swc->child_type == XmSCROLL_HOR))
            {
                newx = MIN(newx, swc->orig_x);
                sw->swindow.hmax = MAX(sw->swindow.hmax,
                    swc->orig_x + child->core.width
                    + 2*child->core.border_width);
            }

            if (swc->child_type == XmNO_SCROLL)
                no_scroll_present = True ;
        }
    }

    /* here we have to have an additional check for the case
       there is no WORKAREA but only HOR or VERT SCROLL.
       We want to get a h/vmax not null in this case. */

    if (sw->swindow.hmax && !sw->swindow.vmax)
    {
        sw->swindow.vmax = MyHeight - (shad*2) ;
        newy = 0 ;
    }

    if (!sw->swindow.hmax && sw->swindow.vmax)
    {
        sw->swindow.hmax = MyWidth - (shad*2) ;
        newx = 0 ;
    }

    if (!sw->swindow.hmax && !sw->swindow.vmax)
    {
        /* check if there is only a NO_SCROLL... */
        if (no_scroll_present)
        {
            sw->swindow.vmax = MyHeight - (shad*2) ;
            sw->swindow.hmax = MyWidth - (shad*2) ;
            newx = 0 ;
            newy = 0 ;
        }
        else
        {
            return ;
        }
    }

    /****************
     *
     * Look at my size and set the clip dimensions accordingly. If the kid
     * fits easily into the space, and we can ditch the scrollbars, set
     * the clip dimension to the size of the window and flag the scrollbars
     * as false. If the kid won't fit in either direction, or if the
     * scrollbars are constantly in the way, set the clip dimensions to the
     * minimum area and flag both scrollbars as true. Otherwise, look at
     * the dimensions, and see if either one needs a scrollbar.
     *
     ****************/

    if (((MyHeight - (shad*2)) >= sw->swindow.vmax) &&
        ((MyWidth - (shad*2)) >= sw->swindow.hmax) &&
        (sw->swindow.ScrollBarPolicy == XmAS_NEEDED))
    {
        clipWidth = MyWidth - (shad * 2);
        clipHeight = MyHeight - (shad * 2);
        HasVSB = HasHSB = FALSE;
        /* no visible scrollbar anymore, move the objects to their
           origin */
        sw->swindow.sw_prev_x = MAXPOS ;
        sw->swindow.sw_prev_y = MAXPOS ;
    }
    else
    {
        clipHeight = (HSBExists) ?
            MyHeight - (sw->swindow.hScrollBar->core.height +
            SB_BORDER_WIDTH + ((HSBht + shad) * 2 + pad))
            : MyHeight - ((VSBht + shad) * 2 );
        clipWidth = (VSBExists) ?
            MyWidth - (sw->swindow.vScrollBar->core.width +
            SB_BORDER_WIDTH + ((VSBht + shad) * 2 + pad))
            : MyWidth - ((HSBht + shad) * 2 );

        if ((((MyHeight - (shad*2)) < sw->swindow.vmax) &&
            ((MyWidth - (shad*2)) < sw->swindow.hmax))  ||
            (sw->swindow.ScrollBarPolicy == XmSTATIC))
        {
            HasVSB = HasHSB = TRUE;
        }
        else
        {
            HasVSB = HasHSB = TRUE;
            if ((sw->swindow.vmax <= clipHeight) ||
                (!VSBExists))
            {
                clipWidth = MyWidth - ((HSBht + shad) * 2);
                clipHeight += VSBht;
                sw->swindow.sw_prev_y = MAXPOS ;
                HasVSB = FALSE;
            }
            if ((sw->swindow.hmax <= clipWidth) ||
                (!HSBExists))
            {
                clipHeight = MyHeight - ((VSBht + shad) * 2);
                clipWidth += HSBht;
                sw->swindow.sw_prev_x = MAXPOS ;
                HasHSB = FALSE;
            }
        }
    }

    HasVSB = (HasVSB && VSBExists);
    HasHSB = (HasHSB && HSBExists);

    /* safe check */
    if (clipHeight < 2) clipHeight = 2 ;
    if (clipWidth < 2) clipWidth = 2 ;

    /****************
     *
     * Now set characteristics for every navigators.
     *  we update the navigators even unmanaged, so that an application can
     *  unamanaged them and still use them as "movers".
     * If the new minimum position has changed (in either x or y direction)
     * we set the value to 0. otherwise, it stays what it was.
     *
     ****************/
    nav_data.valueMask = NavValue;

    /* slider size = extent */
    if ((sw->swindow.vmax - sw->swindow.vmin) < clipHeight)
        sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vmin;
    else
        sw->swindow.vExtent = clipHeight;

    /* always true the first time */
    if (newy != sw->swindow.sw_prev_y)
    {
        /* move all the navigators to 0,
           which doesn't mean move the scrolled object to 0,
           since they can have offset now */
        sw->swindow.vOrigin = 0;
        nav_data.value.y = sw->swindow.vOrigin ;
        nav_data.dimMask = NavigDimensionY ;
        _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, True);
        sw->swindow.sw_prev_y = newy ;
    }

    if (sw->swindow.vOrigin > (sw->swindow.vmax - sw->swindow.vExtent))
    {
        sw->swindow.vOrigin = sw->swindow.vmax - sw->swindow.vExtent;
        nav_data.value.y = sw->swindow.vOrigin ;
        nav_data.dimMask = NavigDimensionY;
        _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, True);
    }

    if ((vinc = (sw->swindow.vmax / 10)) < 1) vinc = 1;
    if ((vpage_inc = (clipHeight - (clipHeight / 10))) < 1)
        vpage_inc = clipHeight;

    if ((sw->swindow.hmax - sw->swindow.hmin) < clipWidth)
        sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hmin;
    else
        sw->swindow.hExtent = clipWidth;

    if (newx != sw->swindow.sw_prev_x)
    {
        sw->swindow.hOrigin = 0;
        nav_data.value.x = sw->swindow.hOrigin ;
        nav_data.dimMask = NavigDimensionX ;
        _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, True);
        /* when reversed, the moving proc has to be called manually,
           for the reverse configure to happen. I might not get called
           by the navigator API when the value already matches */
        if (LayoutIsRtoLM((Widget)sw))
            SliderMove((Widget)sw->swindow.hScrollBar, clip, NULL);
        sw->swindow.sw_prev_x = newx ;
    }

    if (sw->swindow.hOrigin > (sw->swindow.hmax - sw->swindow.hExtent))
    {
        sw->swindow.hOrigin = sw->swindow.hmax - sw->swindow.hExtent;
        nav_data.value.x = sw->swindow.hOrigin ;
        nav_data.dimMask = NavigDimensionX ;
        _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, True);
    }

    if ((hinc = (sw->swindow.hmax / 10)) < 1) hinc = 1;
    if ((hpage_inc = (clipWidth - (clipWidth / 10))) < 1)
        hpage_inc = clipWidth;

    nav_data.value.x = sw->swindow.hOrigin ;
    nav_data.value.y = sw->swindow.vOrigin ;
    nav_data.minimum.x = sw->swindow.hmin ;
    nav_data.minimum.y = sw->swindow.vmin ;
    nav_data.maximum.x = sw->swindow.hmax ;
    nav_data.maximum.y = sw->swindow.vmax ;
    nav_data.slider_size.x = sw->swindow.hExtent ;
    nav_data.slider_size.y = sw->swindow.vExtent ;
    nav_data.increment.x = hinc ;
    nav_data.increment.y = vinc ;
    nav_data.page_increment.x = hpage_inc ;
    nav_data.page_increment.y = vpage_inc ;

    nav_data.dimMask = NavigDimensionX|NavigDimensionY ;
    nav_data.valueMask = NavValue|NavMinimum|NavMaximum|
        NavSliderSize|NavIncrement|NavPageIncrement;

    _XmSFUpdateNavigatorsValue((Widget)sw, &nav_data, False);

    /****************
     *
     * Now the layout.
     *
     ****************/

    if (HasHSB)
        if (!HasVSB)
            sw->swindow.hsbWidth = MyWidth;
    else
        sw->swindow.hsbWidth = clipWidth  + ((HSBht + shad) * 2);
    else
        sw->swindow.hsbWidth = MyWidth;

    if (HasVSB)
        if (!HasHSB)
            sw->swindow.vsbHeight = MyHeight;
    else
        sw->swindow.vsbHeight = clipHeight  + ((VSBht + shad) * 2);
    else
        sw->swindow.vsbHeight = MyHeight;

    HSBht = (HasHSB)
        ? sw->swindow.hScrollBar->primitive.highlight_thickness : 0;

    VSBht = (HasVSB)
        ? sw->swindow.vScrollBar->primitive.highlight_thickness : 0;

    /****************
     *
     * Get the position for scrollbars and workarea
     *
     ****************/
    ComputeLocations(sw,
        HSBht, VSBht, HasHSB, HasVSB,
        &clipX, &clipY, &hsbX, &hsbY, &vsbX, &vsbY);

    /****************
     *
     * Finally - move the widgets.
     *
     ****************/
    XmeConfigureObject((Widget) sw->swindow.ClipWindow,
        clipX, clipY, clipWidth, clipHeight, 0);
    sw->swindow.AreaWidth = clipWidth;
    sw->swindow.AreaHeight = clipHeight;

    if (HasVSB)
        XmeConfigureObject( (Widget) sw->swindow.vScrollBar, vsbX, vsbY,
            sw->swindow.vScrollBar->core.width, sw->swindow.vsbHeight,
            SB_BORDER_WIDTH);
    else
    if (VSBExists)
    {
        XmeConfigureObject((Widget) sw->swindow.vScrollBar, sw->core.width,
            sw->swindow.YOffset, sw->swindow.vScrollBar->core.width,
            sw->swindow.vsbHeight, SB_BORDER_WIDTH);
        if (_XmFocusIsHere( (Widget) sw->swindow.vScrollBar))
            XmProcessTraversal( (Widget) sw, XmTRAVERSE_NEXT_TAB_GROUP);
    }

    if (HasHSB)
        XmeConfigureObject( (Widget) sw->swindow.hScrollBar, hsbX, hsbY,
            sw->swindow.hsbWidth,
            sw->swindow.hScrollBar->core.height, SB_BORDER_WIDTH);
    else
    if (HSBExists)
    {
        XmeConfigureObject( (Widget) sw->swindow.hScrollBar,
            sw->swindow.XOffset, sw->core.height, sw->swindow.hsbWidth,
            sw->swindow.hScrollBar->core.height, SB_BORDER_WIDTH);
        if (_XmFocusIsHere( (Widget) sw->swindow.hScrollBar))
            XmProcessTraversal( (Widget) sw, XmTRAVERSE_NEXT_TAB_GROUP);
    }

    if (VSBExists) sw->swindow.vsbX = sw->swindow.vScrollBar->core.x;
    if (HSBExists) sw->swindow.hsbY = sw->swindow.hScrollBar->core.y;
}


/************************************************************************
 *									*
 * HandleDrop:	does nothing special                    	        *
 *									*
 ************************************************************************/
static void
HandleDrop(
Widget    w,                                      /* unused */
XtPointer client_data,                            /* unused */
XtPointer call_data)
{
    XmDropProcCallback      DropProc;
    Widget          dc;
    Arg args[1] ;

    DropProc = (XmDropProcCallback)call_data;
    dc = DropProc->dragContext;
    XtSetArg(args[0], XmNtransferStatus, XmTRANSFER_FAILURE);
    XmDropTransferStart(dc, args, 1);
}


static void
TimerEvent(
XtPointer closure,
XtIntervalId *id)                                 /* unused */
{
    AutoDragClosure * auto_clos = (AutoDragClosure *) closure ;
    XmScrollBarWidget scrb = (XmScrollBarWidget) auto_clos->widget ;
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) XtParent(scrb) ;
    unsigned char direction = auto_clos->direction;
    Boolean hit = False ;
    int value ;
    XmNavigatorDataRec nav_data ;
    int repeat_delay = 100 ;

    /* for now, I don't have the get_range for all navigators,
       so special case the scrollbar which I know */
    if (!XmIsScrollBar(auto_clos->widget)) return ;

    /* use XmScrollBar: value, increment, maximum, slider_size, minimum */

    if (direction == FORWARD)
    {
        if ((scrb->scrollBar.value + scrb->scrollBar.increment) <=
            (scrb->scrollBar.maximum - scrb->scrollBar.slider_size))
            value = scrb->scrollBar.value + scrb->scrollBar.increment ;
        else
        {
            value = scrb->scrollBar.maximum - scrb->scrollBar.slider_size ;
            hit = True ;
        }
    }
    else
    {
        if ((scrb->scrollBar.value - scrb->scrollBar.increment) >=
            scrb->scrollBar.minimum)
            value = scrb->scrollBar.value - scrb->scrollBar.increment ;
        else
        {
            value = scrb->scrollBar.minimum ;
            hit = True ;
        }
    }

    nav_data.valueMask = NavValue ;

    /* possible value for direction: horizontal or vertical */
    if (scrb->scrollBar.orientation == XmHORIZONTAL)
    {
        nav_data.value.x = value ;
        nav_data.dimMask = NavigDimensionX ;
    }
    else
    {
        nav_data.value.y = value ;
        nav_data.dimMask = NavigDimensionY ;
    }

    _XmSFUpdateNavigatorsValue ((Widget)sw, &nav_data, True);

    XSync (XtDisplay (scrb), False);              /* needed ? */

    /* reactivate the timer with the same closure if not hit */
    if (!hit)
    {
        XtVaGetValues(auto_clos->widget, XmNrepeatDelay, &repeat_delay, NULL);
        sw->swindow.auto_drag_timer = XtAppAddTimeOut (
            XtWidgetToApplicationContext((Widget) scrb),
            repeat_delay,
            TimerEvent, closure);
    }
    else
    {
        /* free the closure and mark the timer as non existent */
        XtFree((char*) closure);
        sw->swindow.auto_drag_timer = 0 ;
    }
}


static void
HandleDrag(
Widget   w,
XtPointer    client_data,                         /* unused */
XtPointer    call_data)
{
    XmDragProcCallback  dragProc = (XmDragProcCallback)call_data;
    int initial_delay = 250;
    AutoDragClosure * auto_clos;
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) XtParent(w) ;
    AutoDragRects rects = (AutoDragRects) sw -> swindow.auto_drag_rects;

    /* w is one of the scrollbar */

    if (dragProc->reason ==  XmCR_DROP_SITE_ENTER_MESSAGE)
    {
        /* add a start update so that scrollbar moves generating widget
           configurations be bracketed for dropsite update. */

        XmDropSiteStartUpdate(w);

        /* if that fails, initial_delay will stay 250 */
        XtVaGetValues(w, XmNinitialDelay, &initial_delay, NULL);

        if (sw->swindow.auto_drag_timer == 0)
        {
            sw->swindow.auto_drag_closure = auto_clos =
                (AutoDragClosure *) XtMalloc(sizeof(AutoDragClosure)) ;
            auto_clos->widget = w ;

            /* Here we need to find in which direction to move.
               This is done using a private protocol between the
               GetAutoDragRects procs and this function: each scrollbar is
               a drop site with potentially multiple rectangles,
               the first half of them define the backward scroll and the
               second half the forward scroll.
               For now we only handle 2 rectangles, one top or left and
               one bottom or right of the corresp. scrollbar */

            /* see if the current location is in the first rectangle*/
            /* don't bother checking num_rect */
            if (POINT_IN_RECT(dragProc->x, dragProc->y, rects -> up) ||
                POINT_IN_RECT(dragProc->x, dragProc->y, rects -> left))
                auto_clos->direction = BACKWARD ;
            else
                auto_clos->direction = FORWARD ;

            sw->swindow.auto_drag_timer =
                XtAppAddTimeOut (XtWidgetToApplicationContext(w),
                initial_delay,
                TimerEvent, (XtPointer) auto_clos);
        }
    } else
    if (dragProc->reason ==  XmCR_DROP_SITE_LEAVE_MESSAGE)
    {
        /* end the update when leaving the dropsite, which means
           when all move have been completed */

        XmDropSiteEndUpdate(w);

        if (sw->swindow.auto_drag_timer)
        {
            XtRemoveTimeOut (sw->swindow.auto_drag_timer);
            XtFree(sw->swindow.auto_drag_closure);
            sw->swindow.auto_drag_timer = 0 ;
        }
    }
}


/************************************************************************
 *									*
 *  GetAutoDragRects class methods                                      *
 *									*
 ************************************************************************/

static void
GetHorRects(
Widget sw,
XRectangle ** hrect,
Cardinal * num_hrect)
{
    Widget w ;
    XmScrolledWindowWidget scw = (XmScrolledWindowWidget) sw ;

    *num_hrect = 2 ;
    *hrect = (XRectangle *) XtMalloc(sizeof(XRectangle) * (*num_hrect)) ;

    /* The hor rectangles are the ones that horizontally auto scroll,
       they are defined by areas on the left and right of the
       workarea, e.g. the margins, the spacing, the scrollbars
       and the shadows */

    /* Both rects are computed using only the relative work_area or
       clipwindow (in AUTO) location within the scrolled window:
       this is the area between the widget and its parent frame.
       Then they need to be translated into the scrollbar coord system. */

    /* what id to use for the sw child frame */
    if (scw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED)
    {
        w = scw->swindow.WorkWindow;
        if (!w) w = sw ;                          /* fallback */
    } else
    w = (Widget) scw->swindow.ClipWindow;

    /* The first rectangle is the one that makes the scrollbar goes left */
    (*hrect)[0].x = 0 - scw->swindow.hScrollBar->core.x ;
    (*hrect)[0].y = w->core.y - scw->swindow.hScrollBar->core.y ;
    (*hrect)[0].width = MAX(2, w->core.x) ;
    (*hrect)[0].height = w->core.height ;

    /* The second rectangle is the one that makes the scrollbar goes right */
    (*hrect)[1].x = w->core.x + w->core.width
        - scw->swindow.hScrollBar->core.x ;
    (*hrect)[1].y = (*hrect)[0].y ;
    (*hrect)[1].width = sw->core.width - (*hrect)[1].x ;
    /* if width too small, set it to 2 *inside* the SW frame */
    if ((*hrect)[1].width <= 2)
    {
        (*hrect)[1].width = 2 ;
        (*hrect)[1].x -= 2 ;
    }
    (*hrect)[1].height = (*hrect)[0].height ;

}


static void
GetVertRects(
Widget sw,
XRectangle ** vrect,
Cardinal * num_vrect)
{
    Widget w ;
    XmScrolledWindowWidget scw = (XmScrolledWindowWidget) sw ;

    *num_vrect = 2 ;
    *vrect = (XRectangle *) XtMalloc(sizeof(XRectangle) * (*num_vrect)) ;

    /* The vertical rectangles are the ones that vertically auto scroll,
       they are defined by areas on the top and bottom of the
       workarea, e.g. the margins, the spacing, the scrollbars
       and the shadows */

    /* Both rects are computed using only the relative work_area or
       clipwindow (in AUTO) location within the scrolled window:
       this is the area between the widget and its parent frame.
       Then they need to be translated into the scrollbar coord system. */

    /* what id to use for the sw child frame */
    if (scw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED)
    {
        w = scw->swindow.WorkWindow;
        if (!w) w = sw ;                          /* fallback */
    } else
    w = (Widget) scw->swindow.ClipWindow;

    /* The first rectangle is the one that makes the scrollbar goes up */
    (*vrect)[0].x = w->core.x - scw->swindow.vScrollBar->core.x ;
    (*vrect)[0].y = 0 - scw->swindow.vScrollBar->core.y ;
    (*vrect)[0].width = w->core.width ;
    (*vrect)[0].height = MAX(2, w->core.y) ;

    /* The second rectangle is the one that makes the scrollbar goes down */
    (*vrect)[1].x = (*vrect)[0].x ;
    (*vrect)[1].y = w->core.y + w->core.height
        - scw->swindow.vScrollBar->core.y ;
    (*vrect)[1].width = (*vrect)[0].width ;
    (*vrect)[1].height = sw->core.height - (*vrect)[1].y ;

    /* if height too small, set it to 2 *inside* the SW frame */
    if ((*vrect)[1].height <= 2)
    {
        (*vrect)[1].height = 2 ;
        (*vrect)[1].y -= 2 ;
    }
}


/************************************************************************
 *									*
 *  UpdateAutoDrag						        *
 *    update the dropsite size or create them the first time	        *
 *									*
 ************************************************************************/
static void
UpdateAutoDrag(
XmScrolledWindowWidget sw)

{
    Widget hsb = (Widget) sw->swindow.hScrollBar,
        vsb = (Widget) sw->swindow.vScrollBar;
    Arg args[10] ;
    Cardinal n ;
    XRectangle *hrect, *vrect;
    Cardinal num_hrect, num_vrect ;
    AutoDragRects rects = (AutoDragRects) sw -> swindow.auto_drag_rects;

    /* Entering here we know we have valid dropsites,
       we now update their size */

    /* Each drop site is associated with a scrollbar, it has
       2 rectangles corresponding to the areas (on both side
       of the workarea) that autoscrolls this scrollbar */

    if (hsb)
    {

        /* Get the 2 horizontal rectangles defining the drop site
           for horizontal autoscroll using a class method, so that
           subclasses can override them */

        XmScrolledWindowClassExt              *wcePtr;
        WidgetClass  wc = XtClass((Widget) sw);

        wcePtr = _XmGetScrolledWindowClassExtPtr(wc, NULLQUARK);

        if (*wcePtr && (*wcePtr)->get_hor_rects)
        {
            (*((*wcePtr)->get_hor_rects)) ((Widget) sw, &hrect, &num_hrect) ;
        }
        else
        {
            /* no extension found or an extension with NULL proc,
               so use the standard one */
            GetHorRects ((Widget) sw, &hrect, &num_hrect) ;
        }

        n = 0 ;
        XtSetArg(args[n], XmNdropRectangles, hrect); n++ ;
        XtSetArg(args[n], XmNnumDropRectangles, num_hrect); n++ ;
        XmDropSiteUpdate(hsb, args, n);

        /* Assign drop data in auto_drag_rects */
        rects -> left = hrect[0];

        #ifdef AUTODRAG_DEBUG
        printf("hrect %d %d %d %d / %d %d %d %d \n",
            hrect[0].x, hrect[0].y, hrect[0].width, hrect[0].height,
            hrect[1].x, hrect[1].y, hrect[1].width, hrect[1].height);
        #endif

        XtFree((char *)hrect);
    }

    if (vsb)
    {
        /* Get the 2 vertical rectangles defining the drop site
           for vertical autoscroll using a class method, so that
           subclasses can override them */

        XmScrolledWindowClassExt              *wcePtr;
        WidgetClass   wc = XtClass((Widget) sw);

        wcePtr = _XmGetScrolledWindowClassExtPtr(wc, NULLQUARK);

        if (*wcePtr && (*wcePtr)->get_vert_rects)
        {
            (*((*wcePtr)->get_vert_rects)) ((Widget) sw, &vrect, &num_vrect) ;
        }
        else
        {
            /* no extension found or an extension with NULL proc,
               so use the standard one */
            GetVertRects ((Widget) sw, &vrect, &num_vrect) ;
        }

        n = 0 ;
        XtSetArg(args[n], XmNdropRectangles, vrect); n++ ;
        XtSetArg(args[n], XmNnumDropRectangles, num_vrect); n++ ;
        XmDropSiteUpdate(vsb, args, n);

        /* Assign drop data in auto_drag_rects */
        rects -> up = vrect[0];

        #ifdef AUTODRAG_DEBUG
        printf("vrect %d %d %d %d / %d %d %d %d \n",
            vrect[0].x, vrect[0].y, vrect[0].width, vrect[0].height,
            vrect[1].x, vrect[1].y, vrect[1].width, vrect[1].height);
        #endif

        XtFree((char *)vrect);
    }
}


/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/
static void
Destroy(
Widget wid )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (sw->swindow.scroll_frame_inited)
    {
        if (sw->swindow.scroll_frame_data->nav_list)
            XtFree((char*)sw->swindow.scroll_frame_data->nav_list);
        XtFree((char*)sw->swindow.scroll_frame_data);
    }

    if (sw->swindow.auto_drag_rects)
        XtFree((char*) sw->swindow.auto_drag_rects);
}


/************************************************************************
 *									*
 *  Resize								*
 *  Recompute the size of the scrolled window.				*
 *  Also update the auto Drag drop site rectangles                      *
 *  This function also clears the window and reexpose, so SetValues     *
 *  doesn't have to return True if it is going to resize.               *
 *									*
 ************************************************************************/
static void
Resize(
Widget wid )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;

    if (sw->swindow.VisualPolicy == XmVARIABLE)
        VariableLayout(sw);
    else
        ConstantLayout(sw);

    /* very expensive, redo that asap */
    if (XtIsRealized (wid))
    {
        XClearArea(XtDisplay(wid), XtWindow(wid), 0, 0,
            XtWidth(wid),XtHeight(wid), True);

    }

    if (sw->swindow.auto_drag_model == XmAUTO_DRAG_ENABLED)
    {
        UpdateAutoDrag(sw);
    }
}


/************************************************************************
 *									*
 * GetVariableSize - Figure out the size of the scrolledwindow based 	*
 *		on the current scrollbar + workwindow sizes.    	*
 *		non zero value for passed argument mean don't touch     *
 *									*
 ************************************************************************/
static void
GetVariableSize(
XmScrolledWindowWidget sw,
Dimension *pwidth,
Dimension *pheight)
{
    XmScrollBarWidget   hsb = sw->swindow.hScrollBar,
        vsb = sw->swindow.vScrollBar;
    Widget      w = sw->swindow.WorkWindow;
    Dimension       hsheight,vswidth;
    Dimension       hsbht = 0, vsbht = 0,
        shads = sw->manager.shadow_thickness * 2;

    if (ExistManaged(w))
    {
        if (ExistManaged((Widget) vsb))
        {
            vsbht = 2 * vsb->primitive.highlight_thickness;
            vswidth = vsb->core.width + sw->swindow.pad + vsbht;
        } else vswidth = 0;

        if (ExistManaged((Widget) hsb))
        {
            hsbht = 2 * hsb->primitive.highlight_thickness;
            hsheight = hsb->core.height + sw->swindow.pad + hsbht;
        } else hsheight = 0;

        if (!*pwidth)
            *pwidth = w->core.width + (2 * w->core.border_width) + vswidth +
                shads + hsbht + sw->swindow.XOffset + sw->swindow.WidthPad;
        if (!*pheight)
            *pheight = w->core.height + (2*w->core.border_width) + hsheight +
                shads + vsbht + sw->swindow.YOffset + sw->swindow.HeightPad;
    }
    else
    {
        if (!*pwidth) *pwidth = sw->core.width;
        if (!*pheight) *pheight = sw->core.height;
    }

    /* might still be null */
    if (!*pwidth) *pwidth = DEFAULT_SIZE ;
    if (!*pheight) *pheight = DEFAULT_SIZE ;

}


/************************************************************************
 *									*
 *  GeometryManager							*
 *									*
 ************************************************************************/
static XtGeometryResult
GeometryManager(
Widget w,
XtWidgetGeometry *request,
XtWidgetGeometry *reply )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget ) w->core.parent;
    XmScrollBarWidget hsb, vsb;
    XtGeometryResult  retval = XtGeometryNo;
    Dimension       newWidth,newHeight;
    Dimension       hsheight,vswidth;
    Dimension       hsbht, vsbht, bw, ht;
    XtWidgetGeometry  parent_request ;
    XmScrolledWindowConstraint swc = GetSWConstraint(w);

    CheckKids(sw);

    hsbht = vsbht = 0;
    reply->request_mode = 0;
    ht = sw->manager.shadow_thickness * 2;
    hsb = sw->swindow.hScrollBar;
    vsb = sw->swindow.vScrollBar;

    /****************
     *
     * Check the generic child: allow any position and size.
     * Application is responsible for proper geometry related to
     * scrollbars, clipwindow, etc. Application knows the layout
     * rules with margins, spacing, etc.
     * App also need to track the resize because generic children are
     * just ignored byt he SW.
     * Scrollbar requests that move or resize the sb in their allocated
     * area are treated as always yes, it's the appli responsabily
     * to not screw up the layout.
     * this doesn't handle multiple requests: both width and height for instance.
     ****************/
    if ((swc->child_type == XmGENERIC_CHILD) ||
        ((w  == (Widget) vsb) && (request->request_mode & CWY)) ||
        ((w  == (Widget) vsb) && (request->request_mode & CWHeight)) ||
        ((w  == (Widget) hsb) && (request->request_mode & CWX)) ||
        ((w  == (Widget) hsb) && (request->request_mode & CWWidth)))
    {
        if (!IsQueryOnly(request))
        {

            if (IsX(request)) w->core.x = request->x;
            if (IsY(request)) w->core.y = request->y;
            if (IsWidth(request)) w->core.width = request->width;
            if (IsHeight(request)) w->core.height = request->height;
            if (IsBorder(request)) w->core.border_width =
                    request->border_width;
        }
        return XtGeometryYes;
    }

    /****************
     *
     * Carry forward that ugly wart for scrolled primitives...
     *
     ****************/

    if (ExistManaged((Widget) vsb))
    {
        vsbht = 2 * vsb->primitive.highlight_thickness;
        if (ExistManaged(sw->swindow.WorkWindow)        &&
            XmIsPrimitive(sw->swindow.WorkWindow)           &&
            !(XmIsTraversable((Widget)sw->swindow.vScrollBar)))
            vsbht = 0;
        vswidth = vsb->core.width + sw->swindow.pad + vsbht;
    }
    else
        vswidth = 0;

    if (ExistManaged((Widget) hsb))
    {
        hsbht = 2 * hsb->primitive.highlight_thickness;
        if (ExistManaged(sw->swindow.WorkWindow)        &&
            XmIsPrimitive(sw->swindow.WorkWindow)           &&
            !(XmIsTraversable((Widget)sw->swindow.hScrollBar)))
            hsbht = 0;
        hsheight = hsb->core.height + sw->swindow.pad + hsbht;
    }
    else
        hsheight = 0;

    /****************
     *
     * Scrollbars
     ****************/

    /* we've allowed the other case before, deny this one */
    if ((w  == (Widget) vsb) && (request->request_mode & CWX))
        return( XtGeometryNo);
    if ((w  == (Widget) hsb) && (request->request_mode & CWY))
        return(XtGeometryNo);

    /* always deny borderwidth changes, too much work for nothing */
    if (((w  == (Widget) hsb) || (w  == (Widget) vsb)) &&
        (request->request_mode & CWBorderWidth))
        return(XtGeometryNo);                     /* border apply to both dimensions */

    if (((w == (Widget) hsb) && ((Dimension)w->core.y < sw->core.height)) ||
        ((w == (Widget) vsb) && ((Dimension)w->core.x < sw->core.width)))
    {
        /* Visible scrollbar request */

        /* we already eliminated the bad case */
        if(request->request_mode & CWWidth)
            newWidth = sw->core.width - w->core.width + request->width;
        else
            newWidth = sw->core.width ;

        if(request->request_mode & CWHeight)
            newHeight = sw->core.height - w->core.height + request->height;
        else
            newHeight = sw->core.height ;

        parent_request.request_mode = CWWidth | CWHeight;
        parent_request.width = newWidth ;
        parent_request.height = newHeight ;

        /* Stop here in the QueryOnly case */
        if (request->request_mode & XtCWQueryOnly)
        {
            parent_request.request_mode |= XtCWQueryOnly;
            return XtMakeGeometryRequest((Widget) sw, &parent_request, NULL);
        }

        /* else it's for real */
        retval = _XmMakeGeometryRequest((Widget) sw, &parent_request);

        if (retval == XtGeometryYes)
        {
            XtWidgetProc resize;

            /* update the child geometry fields */
            if (request->request_mode & CWBorderWidth)
                w->core.border_width = request->border_width;
            if (request->request_mode & CWWidth)
                w->core.width = request->width;
            if (request->request_mode & CWHeight)
                w->core.height = request->height;

            /* relayout moi - maybe need an instigator param here? */
            _XmProcessLock();
            resize = XtCoreProc(sw, resize);
            _XmProcessUnlock();
            (*resize) ((Widget) sw) ;
        }
        return(retval);

    }
    else if ((w == (Widget) hsb) || (w == (Widget) vsb))
    {
        /* Invisible scrollbar ok request */
        if (!(request->request_mode & XtCWQueryOnly))
        {
            if (request->request_mode & CWBorderWidth)
                w->core.border_width = request->border_width;
            if (request->request_mode & CWWidth)
                w->core.width = request->width;
            if (request->request_mode & CWHeight)
                w->core.height = request->height;
            if (request->request_mode & CWX)
                w->core.x = request->x;
            if (request->request_mode & CWY)
                w->core.y = request->y;
        }
        return XtGeometryYes;
    }

    /**********************
     *  ELSE IT COMES FROM THE WORKWINDOW
     **********************/

    /* APPLICATION_DEFINED: try to grow the SW */
    if (sw->swindow.VisualPolicy == XmVARIABLE)
    {

        if (request->request_mode & CWBorderWidth)
            bw = (request->border_width * 2);
        else
            bw = (w->core.border_width * 2);

        if(request->request_mode & CWWidth)
            newWidth = request->width + vswidth + ht + bw + hsbht
                + sw->swindow.XOffset + sw->swindow.WidthPad ;
        else
            newWidth = w->core.width + vswidth + ht + bw + hsbht
                + sw->swindow.XOffset + sw->swindow.WidthPad ;

        if(request->request_mode & CWHeight)
            newHeight = request->height + hsheight + ht + bw + vsbht
                + sw->swindow.YOffset + sw->swindow.HeightPad ;
        else
            newHeight = w->core.height + hsheight + ht + bw + vsbht
                + sw->swindow.YOffset + sw->swindow.HeightPad;

        parent_request.request_mode = CWWidth | CWHeight;
        parent_request.width = newWidth ;
        parent_request.height = newHeight ;

        /* Stop here in the QueryOnly case */
        if (request->request_mode & XtCWQueryOnly)
        {
            parent_request.request_mode |= XtCWQueryOnly;
            return XtMakeGeometryRequest((Widget) sw, &parent_request, NULL);
        }

        /* else it's for real */
        retval = _XmMakeGeometryRequest((Widget) sw, &parent_request);

        if (retval == XtGeometryNo && !(request->request_mode & CWWidth)) {
            parent_request.request_mode = CWHeight;
            retval = _XmMakeGeometryRequest((Widget) sw, &parent_request);
        }

        if (retval == XtGeometryYes)
        {
            XtWidgetProc resize;

            /* update the child geometry fields */
            if (request->request_mode & CWBorderWidth)
                w->core.border_width = request->border_width;
            if (request->request_mode & CWWidth)
                w->core.width = request->width;
            if (request->request_mode & CWHeight)
                w->core.height = request->height;

            /* relayout moi - maybe need an instigator param here ? */
            if (!XmIsMainWindow(sw))
            {
                _XmProcessLock();
                resize = XtCoreProc(sw, resize);
                _XmProcessUnlock();
                (*resize) ((Widget) sw);
            }
        }
        return(retval);
    }
    /* AUTOMATIC doesn't go thru here anymore, but use a trait */

    return XtGeometryNo;
}


/************************************************************************
 *
 *  UpdateOrigGeom trait method
 *
 *************************************<->***********************************/
static void
UpdateOrigGeom(Widget sf, Widget child, XtWidgetGeometry *geom)
{
    _XmSWNotifyGeoChange(sf, child, geom);
}


/************************************************************************
 *									*
 *  	_XmSWNotifyGeoChange	                                        *
 *        Used by the ClipWindow geometry manager.                      *
 *        could spare a trait method here, but that would add to the    *
 *        scrollFrame set, which has enough.                             *
 *									*
 ************************************************************************/
void
_XmSWNotifyGeoChange(
Widget sw,
Widget child,
XtWidgetGeometry *request)
{
    /* called from ClipW GM and ChangeManage */
    XtWidgetProc resize;

    if (child)
    {
        XmScrolledWindowConstraint swc = GetSWConstraint(child);
        /* child is the ClipWindow child */

        /* reset original/set child position */
        if (request->request_mode & CWX)
        {
            swc->orig_x = request->x ;
        }

        if (request->request_mode & CWY)
        {
            swc->orig_y = request->y ;
        }
    }

    /* Just call Layout, which does all the scrollbars management */

    if (XtIsRealized(sw))
    {
        _XmProcessLock();
        resize = XtCoreProc(sw, resize);
        _XmProcessUnlock();
        (*resize)(sw) ;
    }
}


/************************************************************************
 *									*
 *  ChangeManaged - called whenever there is a change in the managed	*
 *		    set.						*
 *									*
 ************************************************************************/
static void
ChangeManaged(
Widget wid )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    XtWidgetGeometry desired ;
    XtWidgetProc resize;

    if (sw->swindow.FromResize) return;

    CheckKids(sw);

    if (sw->swindow.VisualPolicy == XmVARIABLE)
    {
        /* the first time, only attemps to change non specified sizes */
        if (!XtIsRealized(wid))
        {
            desired.width = XtWidth(wid) ;        /* might be 0 */
            desired.height = XtHeight(wid) ;      /* might be 0 */
        }
        else
        {
            desired.width = 0 ;
            desired.height = 0 ;
        }

        GetVariableSize(sw, &desired.width, &desired.height);
        desired.request_mode = CWWidth;
        (void) _XmMakeGeometryRequest(wid, &desired);

        desired.request_mode = CWHeight;
        (void) _XmMakeGeometryRequest(wid, &desired);
    }

    _XmProcessLock();
    resize = XtCoreProc(sw, resize);
    _XmProcessUnlock();
    (*resize) (wid) ;

    XmeNavigChangeManaged(wid);
}


/************************************************************************
 *									*
 *  QueryGeometry - Query proc for the scrolled window.			*
 *									*
 *  This routine will examine the geometry passed in an recalculate our	*
 *  width/height as follows:  if either the width or height is set, we	*
 *  take that as our new size, and figure out the other dimension to be	*
 *  the minimum size we need to be to display the entire child.  Note	*
 *  that this will only happen in auto mode, with as_needed display 	*
 *  policy.								*
 *									*
 ************************************************************************/
static XtGeometryResult
QueryGeometry(
Widget wid,
XtWidgetGeometry *request,
XtWidgetGeometry *ret )
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) wid ;
    Dimension          MyWidth, MyHeight, KidHeight, KidWidth;
    Widget         w;
    XmScrollBarWidget  hsb, vsb;
    Dimension          hsheight,vswidth;
    Dimension          hsbht, vsbht, ht;
    XtWidgetGeometry   desired, preferred;
    XtGeometryResult retval = XtGeometryYes;

    ret -> request_mode = 0;
    w = sw->swindow.WorkWindow;
    hsb = sw->swindow.hScrollBar;
    vsb = sw->swindow.vScrollBar;

    /****************
     *
     * If the request mode is zero, fill out out default height & width.
     *
     ****************/
    if (request->request_mode == 0)
    {
        if ((sw->swindow.VisualPolicy == XmCONSTANT) ||
            (!sw->swindow.WorkWindow))
        {
            ret->width = sw->core.width;
            ret->height = sw->core.height;
            ret->request_mode = (CWWidth | CWHeight);
            return (XtGeometryAlmost);
        }
        hsbht = vsbht = 0;
        ht = sw->manager.shadow_thickness * 2;

        desired.request_mode = 0;
        XtQueryGeometry(sw->swindow.WorkWindow, &desired, &preferred);

        KidWidth = preferred.width;
        KidHeight = preferred.height;
        if (ExistManaged((Widget) vsb))
        {
            vsbht = 2 * vsb->primitive.highlight_thickness;
            vswidth = vsb->core.width + sw->swindow.pad + vsbht;
        }
        else
            vswidth = 0;

        if (ExistManaged((Widget) hsb))
        {
            hsbht = 2 * hsb->primitive.highlight_thickness;
            hsheight = hsb->core.height + sw->swindow.pad + hsbht;
        }
        else
            hsheight = 0;

        if (ExistManaged(w))
        {
            ret->width = KidWidth + (2 * w->core.border_width) + vswidth +
                ht + hsbht + sw->swindow.XOffset + sw->swindow.WidthPad;
            ret->height = KidHeight  + (2 * w->core.border_width) + hsheight +
                ht + vsbht + sw->swindow.YOffset + sw->swindow.HeightPad;
        }
        else
        {
            ret->width = sw->core.width;
            ret->height = sw->core.height;
        }
        ret->request_mode = (CWWidth | CWHeight);
        return (XtGeometryAlmost);
    }

    /****************
     *
     * If app mode, or static scrollbars, or no visible kid,
     * accept the new size, and return our current size for any
     * missing dimension.
     *
     ****************/
    if ((sw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED) ||
        (!ExistManaged(w)))
    {
        if (!(request -> request_mode & CWWidth))
        {
            ret->request_mode |= CWWidth;
            ret->width = sw->core.width;
            retval = XtGeometryAlmost;
        }
        if (!(request -> request_mode & CWHeight))
        {
            ret->request_mode |= CWHeight;
            ret->height = sw->core.height;
            retval = XtGeometryAlmost;
        }
        return(retval);
    }

    /****************
     *
     * Else look for the specified dimension, and set the other size so that we
     * just enclose the child.
     * If the new size would cause us to lose the scrollbar, figure
     * out the other dimension as well and return that, too.
     *
     ****************/

    hsbht = vsbht = 0;
    ht = sw->manager.shadow_thickness * 2;
    hsb = sw->swindow.hScrollBar;
    vsb = sw->swindow.vScrollBar;

    if ((request -> request_mode & CWWidth) &&
        (request -> request_mode & CWHeight)&&
        (sw->swindow.ScrollBarPolicy == XmAS_NEEDED))
    {
        ret->height = w->core.height + (2 * w->core.border_width) +
            ht + sw->swindow.YOffset + sw->swindow.HeightPad;
        ret->width = w->core.width + (2 * w->core.border_width) +
            ht + sw->swindow.XOffset + sw->swindow.WidthPad;
        ret->request_mode |= (CWWidth | CWHeight);
        return(XtGeometryAlmost);
    }

    if (request -> request_mode & CWHeight)
    {

        MyHeight = request->height - sw->swindow.YOffset -
            sw->swindow.HeightPad - ht;

        if (((w->core.height + (2 * w->core.border_width)) > MyHeight) ||
            (sw->swindow.ScrollBarPolicy == XmSTATIC))
        {
            vsbht = 2 * vsb->primitive.highlight_thickness;
            vswidth = vsb->core.width + sw->swindow.pad;
        }
        else
        {
            vswidth = 0;
            ret->request_mode |= CWHeight;
            ret->height = w->core.height + (2 * w->core.border_width) +
                ht + sw->swindow.YOffset + sw->swindow.HeightPad;
        }

        ret->request_mode |= CWWidth;
        ret->width = w->core.width + (2 * w->core.border_width) + vswidth +
            ht + vsbht + sw->swindow.XOffset + sw->swindow.WidthPad;
        retval = XtGeometryAlmost;
    }

    if (request -> request_mode & CWWidth)
    {
        MyWidth = request->width - sw->swindow.XOffset -
            sw->swindow.WidthPad - ht;

        if (((w->core.width + (2 * w->core.border_width)) > MyWidth) ||
            (sw->swindow.ScrollBarPolicy == XmSTATIC))
        {
            hsbht = 2 * hsb->primitive.highlight_thickness;
            hsheight = hsb->core.height + sw->swindow.pad;
        }
        else
        {
            hsheight = 0;
            ret->request_mode |= CWWidth;
            ret->width = w->core.width + (2 * w->core.border_width) +
                ht + sw->swindow.XOffset + sw->swindow.WidthPad;
        }

        ret->request_mode |= CWHeight;
        ret->height = w->core.height + (2 * w->core.border_width) + hsheight +
            ht + hsbht + sw->swindow.YOffset + sw->swindow.HeightPad;
        retval = XtGeometryAlmost;
    }

    return(retval);
}


/************************************************************************
 *									*
 *  SetValues								*
 *									*
 ************************************************************************/
static Boolean
SetValues(
Widget cw,
Widget rw,
Widget nw,
ArgList args,                                     /* unused */
Cardinal *num_args )                              /* unused */
{
    XmScrolledWindowWidget current = (XmScrolledWindowWidget) cw ;
    XmScrolledWindowWidget request = (XmScrolledWindowWidget) rw ;
    XmScrolledWindowWidget new_w = (XmScrolledWindowWidget) nw ;
    Boolean Flag = FALSE;

    /*******************
     * check validity first
     *******************/

    CheckKids(new_w);

    if(!XmRepTypeValidValue( XmRID_SCROLL_BAR_DISPLAY_POLICY,
        new_w->swindow.ScrollBarPolicy, (Widget) new_w))
    {
        new_w->swindow.ScrollBarPolicy = current->swindow.ScrollBarPolicy;
    }

    if (request->swindow.ScrollPolicy != current->swindow.ScrollPolicy)
    {
        XmeWarning( (Widget) new_w, SWMessage6);
        new_w->swindow.ScrollPolicy = current->swindow.ScrollPolicy;
    }

    if (request->swindow.VisualPolicy != current->swindow.VisualPolicy)
    {
        XmeWarning( (Widget) new_w, SWMessage7);
        new_w->swindow.VisualPolicy = current->swindow.VisualPolicy;
    }

    if ((new_w->swindow.VisualPolicy == XmVARIABLE) &&
        (request->swindow.ScrollBarPolicy == XmAS_NEEDED))
    {
        XmeWarning( (Widget) new_w, SWMessage8);
        new_w->swindow.ScrollBarPolicy = XmSTATIC;
    }

    if (new_w->swindow.ScrollPolicy == XmAUTOMATIC)
    {
        if (new_w->swindow.hScrollBar != current->swindow.hScrollBar)
        {
            XmeWarning( (Widget) new_w, SWMessage9);
            new_w->swindow.hScrollBar = current->swindow.hScrollBar;
        }
        if (new_w->swindow.vScrollBar != current->swindow.vScrollBar)
        {
            XmeWarning( (Widget) new_w, SWMessage9);
            new_w->swindow.vScrollBar = current->swindow.vScrollBar;
        }
    }

    if (new_w->swindow.ClipWindow != current->swindow.ClipWindow)
    {
        XmeWarning( (Widget) new_w, SWMessage10);
        new_w->swindow.ClipWindow = current->swindow.ClipWindow;
    }

    if(!XmRepTypeValidValue( XmRID_SCROLL_BAR_PLACEMENT,
        new_w->swindow.Placement, (Widget) new_w))
    {
        new_w->swindow.Placement = current->swindow.Placement;
    }

    /* In AUTOMATIC, we explicitly forbid changing the workArea,
       because the clipWindow reparenting can't be undone or done at this
       point.
       The only way to swicth between workArea is to destroy the current one,
       which would set it to NULL internally, and recreate it so
       that InsertChild takes it

    if (new_w->swindow.ScrollPolicy == XmAUTOMATIC) {
        if (new_w->swindow.WorkWindow != current->swindow.WorkWindow) {
        XmeWarning((Widget) new_w, SWMessage2);
        new_w->swindow.WorkWindow = current->swindow.WorkWindow;
    }
    }*/

    /*******************
     * then check if there needs to be a relayout of the internals
     * Here we choose not to grow or shrink the SW if one of those
     * change, we just relayout everything in the current boundaries.
     ********************/
    if ((new_w->swindow.WidthPad != current->swindow.WidthPad) ||
        (new_w->swindow.HeightPad != current->swindow.HeightPad) ||
        (new_w->manager.shadow_thickness!=current->manager.shadow_thickness)||
        (new_w->swindow.pad != current->swindow.pad) ||
        (new_w->swindow.Placement != current->swindow.Placement)  ||
        (new_w->swindow.ScrollBarPolicy != current->swindow.ScrollBarPolicy))
    {
        XtWidgetProc resize;

        new_w->swindow.XOffset = new_w->swindow.WidthPad;
        new_w->swindow.YOffset = new_w->swindow.HeightPad;
        /* relayout moi, resize will rexpose as well */
        _XmProcessLock();
        resize = XtCoreProc(new_w, resize);
        _XmProcessUnlock();
        (*resize)(nw) ;
    }

    /* the following can only happen in APP_DEFINED mode */
    if ((new_w->swindow.hScrollBar != current->swindow.hScrollBar)||
        (new_w->swindow.vScrollBar != current->swindow.vScrollBar)||
        (new_w->swindow.WorkWindow != current->swindow.WorkWindow))
    {

        /* set our core geometry to the needed size -
           no resizePolicy here...
           See comment in MainW.c for this realized check */
        if (XtIsRealized((Widget)new_w))
        {
            new_w->core.width = 0 ;
            new_w->core.height = 0 ;
            GetVariableSize(new_w, &new_w->core.width, &new_w->core.height);
        }
    }

    if (new_w->swindow.auto_drag_model != current->swindow.auto_drag_model)
    {

        if (new_w->swindow.auto_drag_model == XmAUTO_DRAG_ENABLED)
        {
            Arg s_args[5];
            Cardinal n ;

            /* moving from disable to enable, allocate the rects the
               first time thru */
            if (new_w->swindow.auto_drag_rects == NULL)
                new_w->swindow.auto_drag_rects =
                    XtCalloc(1, sizeof(AutoDragRectsRec));

            /* reinstall the D&D callback on the potential scrollbars */
            n = 0 ;
            XtSetArg(s_args[n], XmNdropProc, HandleDrop), n++ ;
            XtSetArg(s_args[n], XmNdragProc, HandleDrag), n++ ;
            XtSetArg(s_args[n], XmNnumImportTargets, 1), n++ ;
            XtSetArg(s_args[n], XmNimportTargets, new_w), n++ ;
            assert(n <= XtNumber(s_args));
            if (new_w->swindow.hScrollBar)
                XmDropSiteRegister((Widget)new_w->swindow.hScrollBar,
                    s_args, n);
            if (new_w->swindow.vScrollBar)
                XmDropSiteRegister((Widget)new_w->swindow.vScrollBar,
                    s_args, n);
        }
        else
        {
            /* moving from enable to disable, unregister the d&d site
               but leave the drag_rect around */
            if (new_w->swindow.hScrollBar)
                XmDropSiteUnregister((Widget)new_w->swindow.hScrollBar);
            if (new_w->swindow.vScrollBar)
                XmDropSiteUnregister((Widget)new_w->swindow.vScrollBar);
        }
    }

    return (Flag);
}


/************************************************************************
 *
 *  Constraint Initialize
 *
 *************************************<->***********************************/
static void
ConstraintInitialize(
Widget rw,                                        /* unused */
Widget nw,
ArgList args,                                     /* unused */
Cardinal *num_args )                              /* unused */
{
    XmScrolledWindowConstraint swc;

    if (!XtIsRectObj (nw)) return;

    swc = GetSWConstraint(nw);

    if ((swc->child_type != (unsigned char) RESOURCE_DEFAULT) &&
        (!XmRepTypeValidValue(XmRID_SCROLLED_WINDOW_CHILD_TYPE,
        swc->child_type, nw)))
    {
        swc->child_type = (unsigned char) RESOURCE_DEFAULT;
    }

    /* for all children, get their original position.
       It actually only matter for clipped children, but quicker this way */
    swc->orig_x = nw->core.x ;
    swc->orig_y = nw->core.y ;
}


/***************************************
   No ConstraintSetValues.  Because of the reparenting to the clipwindow,
   it won't be called in all cases anyway. ClipWindow could get one,
   and here one as well, with the only benefit to reject wrong
   childType setting, not worth it IHMO. The tracking od the set position
   is done in the ClipWindow geometry manager for the clipped kid, which
   calls the notifygeochange API
 ***************************************/

/************************************************************************
 *
 *  ScrollFrame Init trait method
 *
 *************************************<->***********************************/
static void
ScrollFrameInit (
Widget sf,
XtCallbackProc moveCB,
Widget scrollable)
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) sf ;

    if (sw->swindow.scroll_frame_inited) return ;

    sw->swindow.scroll_frame_inited = True ;

    sw->swindow.scroll_frame_data = (XmScrollFrameData)
        XtMalloc(sizeof(XmScrollFrameDataRec));

    sw->swindow.scroll_frame_data->num_nav_list = 0 ;
    sw->swindow.scroll_frame_data->nav_list = NULL ;
    sw->swindow.scroll_frame_data->num_nav_slots = 0 ;

    sw->swindow.scroll_frame_data->move_cb = moveCB ;
    sw->swindow.scroll_frame_data->scrollable = scrollable ;
}


/************************************************************************
 *
 *  GetInfo
 *	scrolledwindow is a 2 dimensional frame.
 *
 ************************************************************************/
static Boolean
GetInfo(
Widget sf,
Cardinal * dimension,
Widget ** nav_list,
Cardinal * num_nav_list)
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) sf ;

    if (dimension) *dimension = 2 ;               /* two dimensional frame */

    if (sw->swindow.scroll_frame_inited)
    {

        if (nav_list) *nav_list = sw->swindow.scroll_frame_data->nav_list ;
        if (num_nav_list)
            *num_nav_list = sw->swindow.scroll_frame_data->num_nav_list ;
    }

    return sw->swindow.scroll_frame_inited ;
}


/************************************************************************
 *
 *  AddNavigator trait method
 *
 *************************************<->***********************************/
static void
AddNavigator(
Widget sf,
Widget nav,
Mask dimMask)
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) sf ;

    /* check if already initialized scrollFrame */
    if (!sw->swindow.scroll_frame_inited)
    {
        XmScrollFrameTrait sf_trait = (XmScrollFrameTrait)
            XmeTraitGet((XtPointer) XtClass(sf), XmQTscrollFrame);

        /* not inited yet: it's a non ScrolledList/Text APP_DEFINED
           or AUTOMATIC scrolled window.
           Initialize now, when the first navigator is added.
           We cannot give any good move callback to install on the other
           navigators, so use NULL. In this case, we rely on the navigators
           to have their own callback prior to be added to the navigator list */
        sf_trait->init(sf, (XtCallbackProc) NULL, (Widget) NULL);
    }

    /* now scroll_frame_data is always valid */
    _XmSFAddNavigator(sf, nav, dimMask,  sw->swindow.scroll_frame_data);
}


/************************************************************************
 *
 *  RemoveNavigator trait method
 *
 *************************************<->***********************************/
static void
RemoveNavigator(
Widget sf,
Widget nav)
{
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget) sf ;

    if (sw->swindow.scroll_frame_inited)
        _XmSFRemoveNavigator(sf, nav, sw->swindow.scroll_frame_data);
}


/************************************************************************
 *									*
 *  	_XmSWGetClipArea
 *        Used by the Traversal code.
 *         - widget specifies a child of the clipwindow.
 *	   - rect returns the area of the clip window (in root window
 *	     coordinates) where that child can appear.
 *	   - returns True is successful, otherwise False.
 *    (For a clip window with only one work window this will
 *     degenerate to the root bounds of the clip window itself)
 *
 ************************************************************************/

Boolean
_XmSWGetClipArea(Widget widget,
XRectangle *rect)
{
    XmClipWindowWidget clip = (XmClipWindowWidget) XtParent(widget);
    XmScrolledWindowWidget sw;
    Cardinal i ;
    Widget child ;
    XmScrolledWindowConstraint swc;
    Position root_x, root_y ;

    /* reality check */
    if (!clip || !XmIsClipWindow(clip)) return False ;
    sw = (XmScrolledWindowWidget) XtParent(clip);
    if (!sw) return False ;

    /* init with the clipwindow frame */
    rect->x = 0 ;
    rect->y = 0 ;
    rect->width = clip->core.width ;
    rect->height = clip->core.height ;

    /* loop thru the clip children, and for each partially scrollable
       babie, modify the returned rect. This assumes a WORK_AREA (all
       scrollable) kid is the target, some SCROLL_HOR or VERT sibling
       are obscuring it, and since we only return ONE rectangle, we
       special case the header (y=0) or the first column (x=0) child */
    for (i = 0; i < clip->composite.num_children; i++)
    {
        child = clip->composite.children[i];
        if (ExistManaged(child))
        {
            swc = GetSWConstraint(child);
            if (swc->child_type == XmSCROLL_HOR)
            {
                /* this one doesn't move vertically. If its y is 0
                   takes its height as the new y of our rectangle, if
                   its y is not 0, it becomes our rect height (everything
                   below its y is considered unaccessible) */
                if (child->core.y == 0)
                {
                    rect->y = child->core.height ;
                    rect->height -= child->core.height ;
                } else
                rect->height = child->core.y ;
            } else
            if (swc->child_type == XmSCROLL_HOR)
            {
                /* same thing for x */
                if (child->core.x == 0)
                {
                    rect->x = child->core.width ;
                    rect->width -= child->core.width ;
                } else
                rect->width = child->core.x ;
            }
        }
    }

    /* root coordinates is wanted. (Xt doesn't seem to allow for
       rect to be passed in and out at the same time) */
    XtTranslateCoords((Widget)clip, rect->x, rect->y,
        &root_x, &root_y);
    rect->x = root_x ;
    rect->y = root_y ;

    return True;
}


/************************************************************************
 *									*
 * Public API Functions							*
 *									*
 ************************************************************************/

/************************************************************************
 *									*
 * XmScrolledWindowSetAreas - set a new widget set.			*
 *		a NULL means: don't change the parameter        	*
 *        - deprecated in favor of SetValues, which it uses             *
 ************************************************************************/
void
XmScrolledWindowSetAreas(
Widget w,
Widget hscroll,
Widget vscroll,
Widget wregion )
{
    Arg args[5] ;
    Cardinal n;

    n = 0;
    if (hscroll)
    {
        XtSetArg (args[n], XmNhorizontalScrollBar, hscroll); n++;
    }
    if (vscroll)
    {
        XtSetArg (args[n], XmNverticalScrollBar, vscroll); n++;
    }
    if (wregion)
    {
        XtSetArg (args[n], XmNworkWindow, wregion); n++;
    }
    XtSetValues(w, args, n);
}


/************************************************************************
 *									*
 * XmCreateScrolledWindow - hokey interface to XtCreateWidget.		*
 *									*
 ************************************************************************/
Widget
XmCreateScrolledWindow(
Widget parent,
char *name,
ArgList args,
Cardinal argCount )
{

    return (XtCreateWidget(name, xmScrolledWindowWidgetClass,
        parent, args, argCount ));
}

Widget
XmVaCreateScrolledWindow(
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
                         xmScrolledWindowWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedScrolledWindow(
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
                         xmScrolledWindowWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/***************************************************************************
  DESIGN NOTES:

  InsertChild takes care of setting the internal scrollbar or workarea
  fields to the last inserted child.

  The previous kid might still be hanging around in the managed child list
  if the application hasn't done something about it.

  In the regular case, one creates a SW, add kids and everything is fine.
  (and SetAreas or SetValues is a NoOp).
If the appli creates another kid as the workarea for instance
(same problem for another scrollbar in APP_DEFINED), it becomes the
current workarea and it is on top by default - no problem.
Now if the application switches the 2 workareas and reset the first one
as the current workarea using XtSetValues, who is responsible for
raising it on top (or lowering, or unmapping - the other) ?
I'd say SetValues deals with that situation using RaiseWidget.

When a child is deleted, if it is a current resource, the internal
field is NULLed, otherwise, it is just deleted from the child list.
(ChangeManaged is called for the relayout).

***************************************************************************/

