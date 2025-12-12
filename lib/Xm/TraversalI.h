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
#ifndef _XmTraversalI_h
#define _XmTraversalI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif


#define NavigTypeIsTabGroup(navigation_type) \
  ((navigation_type == XmTAB_GROUP) || \
   (navigation_type == XmSTICKY_TAB_GROUP) || \
   (navigation_type == XmEXCLUSIVE_TAB_GROUP))


typedef enum {
    XmUnrelated,
    XmMyAncestor,
    XmMyDescendant,
    XmMyCousin,
    XmMySelf
} XmGeneology;

typedef struct _XmTravGraphRec
{
    union _XmTraversalNodeRec *head ;
    Widget top ;
    union _XmTraversalNodeRec *current ;
    unsigned short num_entries ;
    unsigned short num_alloc ;
    unsigned short next_alloc ;
    unsigned short exclusive ;
    unsigned short tab_list_alloc ;
    unsigned short num_tab_list ;
    Widget *excl_tab_list ;
} XmTravGraphRec, * XmTravGraph ;


typedef struct _XmFocusDataRec {
    Widget	active_tab_group;
    Widget	focus_item;
    Widget	old_focus_item;
    Widget	pointer_item;
    Widget	old_pointer_item;
    Boolean	needToFlush;
    XCrossingEvent lastCrossingEvent;
    XmGeneology focalPoint;
    unsigned char focus_policy ; /* Mirrors focus_policy resource when focus */
    XmTravGraphRec trav_graph ;  /*   data retrieved using _XmGetFocusData().*/
    Widget      first_focus ;
} XmFocusDataRec ;

typedef enum
{
  XmTAB_GRAPH_NODE, XmTAB_NODE, XmCONTROL_GRAPH_NODE, XmCONTROL_NODE
} XmTravGraphNodeType ;

typedef union _XmDeferredGraphLink
{
  int offset ;
  struct _XmGraphNodeRec *link ;
} XmDeferredGraphLink ;

typedef struct _XmAnyNodeRec               /* Common */
{
  unsigned char type ;
  XmNavigationType nav_type ;
  XmDeferredGraphLink tab_parent ;
  Widget widget ;
  XRectangle rect ;
  union _XmTraversalNodeRec *next ;
  union _XmTraversalNodeRec *prev ;
} XmAnyNodeRec, *XmAnyNode ;

typedef struct _XmControlNodeRec
{
  XmAnyNodeRec any ;
  union _XmTraversalNodeRec *up ;
  union _XmTraversalNodeRec *down ;
} XmControlNodeRec, *XmControlNode ;

typedef struct _XmTabNodeRec
{
  XmAnyNodeRec any ;
} XmTabNodeRec, *XmTabNode ;

typedef struct _XmGraphNodeRec
{
  XmAnyNodeRec any ;
  union _XmTraversalNodeRec *sub_head ;
  union _XmTraversalNodeRec *sub_tail ;
} XmGraphNodeRec, *XmGraphNode ;

typedef union _XmTraversalNodeRec
{
  XmAnyNodeRec any ;
  XmControlNodeRec control ;
  XmTabNodeRec tab ;
  XmGraphNodeRec graph ;
} XmTraversalNodeRec, *XmTraversalNode ;

typedef struct
{
  XmTraversalNode *items;
  XmTraversalNode lead_item;
  Cardinal num_items;
  Cardinal max_items;
  Position min_hint;
  Position max_hint;
} XmTraversalRow;


/********    Private Function Declarations for Traversal.c    ********/

extern XmFocusData _XmCreateFocusData( void ) ;
extern void _XmDestroyFocusData(
                        XmFocusData focusData) ;
extern void _XmSetActiveTabGroup(
                        XmFocusData focusData,
                        Widget tabGroup) ;
extern Widget _XmGetActiveItem(
                        Widget w) ;
extern void _XmNavigInitialize(
                        Widget request,
                        Widget new_wid,
                        ArgList args,
                        Cardinal *num_args) ;
extern Boolean _XmNavigSetValues(
                        Widget current,
                        Widget request,
                        Widget new_wid,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmNavigResize(
                        Widget wid) ;
extern void _XmValidateFocus(
                        Widget wid) ;
extern void _XmNavigDestroy(
                        Widget wid) ;
extern Boolean _XmCallFocusMoved(
                        Widget old,
                        Widget new_wid,
                        XEvent *event) ;
extern Boolean _XmMgrTraversal(
                        Widget wid,
                        XmTraversalDirection direction) ;
extern void _XmClearFocusPath(
                        Widget wid) ;
extern Boolean _XmFocusIsHere(
                        Widget w) ;
extern unsigned char _XmGetFocusPolicy(
                        Widget w) ;
extern Widget _XmFindTopMostShell(
                        Widget w) ;
extern void _XmFocusModelChanged(
                        Widget wid,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern XmFocusData _XmGetFocusData(
                        Widget wid) ;
extern Boolean _XmComputeVisibilityRect(
                        Widget w,
                        XRectangle *rectPtr,
			Boolean include_initial_border,
			Boolean allow_scrolling) ;
extern Boolean _XmGetPointVisibility(Widget w,
				     int root_x,
				     int root_y);
extern void _XmSetRect(
                        register XRectangle *rect,
                        Widget w) ;
extern int _XmIntersectRect(
                        register XRectangle *srcRectA,
                        register Widget widget,
                        register XRectangle *dstRect) ;
extern int _XmEmptyRect(
                        register XRectangle *r) ;
extern void _XmClearRect(
                        register XRectangle *r) ;
extern Boolean _XmIsNavigable(
                        Widget wid) ;
extern void _XmWidgetFocusChange(
                        Widget wid,
                        XmFocusChange change) ;
extern Widget _XmNavigate(
                        Widget wid,
                        XmTraversalDirection direction) ;
extern void _XmSetInitialOfTabGroup(
                        Widget tab_group,
                        Widget init_focus) ;
extern void _XmResetTravGraph(
                        Widget wid) ;
extern Boolean _XmShellIsExclusive(
                        Widget wid) ;
extern Widget _XmGetFirstFocus(
                        Widget wid) ;

/********    End Private Function Declarations    ********/

/********    Private Function Declarations for TraversalI.c    ********/

extern XmNavigability _XmGetNavigability(
                        Widget wid) ;
extern Boolean _XmIsViewable(
                        Widget wid) ;
extern Widget _XmIsScrollableClipWidget(
                        Widget work_window,
			Boolean scrollable,
                        XRectangle *visRect) ;
extern Boolean _XmGetEffectiveView(
                        Widget wid,
                        XRectangle *visRect) ;
extern Boolean _XmIntersectionOf(
                        register XRectangle *srcRectA,
                        register XRectangle *srcRectB,
                        register XRectangle *destRect) ;
extern XmNavigationType _XmGetNavigationType(
                        Widget widget) ;
extern Widget _XmGetActiveTabGroup(
                        Widget wid) ;
extern Widget _XmTraverseAway(
                        XmTravGraph list,
                        Widget wid,
                        Boolean wid_is_control) ;
extern Widget _XmTraverse(
                        XmTravGraph list,
                        XmTraversalDirection action,
                        XmTraversalDirection *local_dir,
                        Widget reference_wid) ;
extern void _XmFreeTravGraph(
                        XmTravGraph trav_list) ;
extern void _XmTravGraphRemove(
                        XmTravGraph tgraph,
                        Widget wid) ;
extern void _XmTravGraphAdd(
                        XmTravGraph tgraph,
                        Widget wid) ;
extern void _XmTravGraphUpdate(
                        XmTravGraph tgraph,
                        Widget wid) ;
extern Boolean _XmNewTravGraph(
                        XmTravGraph trav_list,
                        Widget top_wid,
                        Widget init_current) ;
extern Boolean _XmSetInitialOfTabGraph(
                        XmTravGraph trav_graph,
                        Widget tab_group,
                        Widget init_focus) ;
extern void _XmTabListAdd(
                        XmTravGraph graph,
                        Widget wid) ;
extern void _XmTabListDelete(
                        XmTravGraph graph,
                        Widget wid) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTraversalI_h */
