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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: TraversalI.c /main/13 1997/10/13 11:28:33 cshi $"
#endif
#endif

#include <stdlib.h>
#include <Xm/BaseClassP.h>
#include <Xm/ClipWindowP.h>
#include <Xm/GadgetP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/SashP.h>
#include <Xm/ScrolledWP.h>
#include <Xm/XmosP.h>		/* for memmove() */
#include "ScrolledWI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmI.h"

#define XmTRAV_LIST_ALLOC_INCREMENT	 16
#define XmTAB_LIST_ALLOC_INCREMENT	  8
#define STACK_SORT_LIMIT		128

/* Type of sort comparison function. */
typedef int (*Comparator)(const void *, const void *);

/********    Static Function Declarations    ********/

static Boolean NodeIsTraversable(XmTraversalNode node);
static XmTraversalNode TraverseControl(XmTraversalNode cur_node,
				       XmTraversalDirection action);
static XmTraversalNode NextControl(XmTraversalNode ctl_node);
static XmTraversalNode PrevControl(XmTraversalNode ctl_node);
static Boolean InitializeCurrent(XmTravGraph list,
				 Widget wid,
				 Boolean renew_list_if_needed);
static XmTraversalNode GetNextNearestNode(XmGraphNode graph,
					  XRectangle *rect,
					  XmDirection layout);
static XmTraversalNode TraverseTab(XmTraversalNode cur_node,
				   XmTraversalDirection action);
static XmTraversalNode AllocListEntry(XmTravGraph list);
static Boolean GetChildList(Widget composite,
			    Widget **widget_list,
			    Cardinal *num_in_list);
static void GetNodeList(Widget wid,
                        XRectangle *parent_rect,
                        XmTravGraph trav_list,
                        int tab_parent,
                        int control_parent);
static XmTraversalNode GetNodeFromGraph(XmGraphNode graph,
					Widget wid);
static XmTraversalNode GetNodeOfWidget(XmTravGraph trav_list,
				       Widget wid);
static void LinkNodeList(XmTravGraph list);
static Boolean NodesOverlap(XmTraversalNode nodeA,
			    XmTraversalNode nodeB,
			    Boolean         horizontal);
static void TruncateRow(XmTraversalRow  *row,
			Cardinal         length,
			XmTraversalNode *free_list,
			Cardinal        *first_free,
			Cardinal	 max_free,
			Boolean		 horizontal,
			XmDirection	 layout);
static void AppendToRow(XmTraversalNode  item,
			XmTraversalRow  *row,
			Boolean		 horizontal,
			XmDirection	 layout);
static Boolean NodeDominates(XmTraversalNode node_1,
			     XmTraversalNode node_2,
			     Boolean	     horizontal,
			     XmDirection     layout);
static void Sort(XmTraversalNode *nodes,
		 size_t           n_mem,
		 Boolean          horizontal,
		 XmDirection      layout);
static int CompareExclusive(const void *A, const void *B);
static Comparator HorizNodeComparator(XmDirection layout);
static int CompareNodesHorizLT(const void *A, const void *B);
static int CompareNodesHorizRT(const void *A, const void *B);
static int CompareNodesHorizLB(const void *A, const void *B);
static int CompareNodesHorizRB(const void *A, const void *B);
static Comparator VertNodeComparator(XmDirection layout);
static int CompareNodesVertLT(const void *A, const void *B);
static int CompareNodesVertRT(const void *A, const void *B);
static int CompareNodesVertLB(const void *A, const void *B);
static int CompareNodesVertRB(const void *A, const void *B);
static void SortTabGraph(XmGraphNode graph,
			 Boolean exclusive,
			 XmDirection layout);
static void SortControlGraph(XmGraphNode graph,
			     Boolean exclusive,
			     XmDirection layout);
static void SortNodeList(XmTravGraph trav_list);
static Boolean SetInitialNode(XmGraphNode graph,
			      XmTraversalNode init_node);
static void SetInitialWidgets(XmTravGraph trav_list);
static void GetRectRelativeToShell(Widget wid,
				   XRectangle *rect);
static int SearchTabList(XmTravGraph graph,
			 Widget wid);
static void DeleteFromTabList(XmTravGraph graph,
			      int indx);

static Boolean LastControl(Widget w,
			   XmTraversalDirection dir,
			   XmTravGraph graph);
static XmTraversalDirection LocalDirection(Widget w,
					   XmTraversalDirection direction);

/********    End Static Function Declarations    ********/


#ifdef DEBUG_TRAVERSAL
static void PrintControl(XmTraversalNode node);
static void PrintTab(XmTraversalNode node);
static void PrintGraph(XmTraversalNode node);
static void PrintNodeList(XmTravGraph list);
#endif /* DEBUG_TRAVERSAL */


static XmTravGraph SortReferenceGraph;

XmNavigability
_XmGetNavigability(Widget wid)
{
  if (XtIsRectObj(wid) &&
      !wid->core.being_destroyed)
    {
      XmBaseClassExt *er;

      if ((er = _XmGetBaseClassExtPtr(XtClass(wid), XmQmotif)) &&
	  (*er) &&
	  ((*er)->version >= XmBaseClassExtVersion) &&
	  (*er)->widgetNavigable)
        {
	  return (*((*er)->widgetNavigable))(wid);
	}
      else
        {
	  /* From here on is compatibility code. */
	  WidgetClass wc;

	  if (XmIsPrimitive(wid))
	    wc = (WidgetClass) &xmPrimitiveClassRec;
	  else if (XmIsGadget(wid))
	    wc = (WidgetClass) &xmGadgetClassRec;
	  else if (XmIsManager(wid))
	    wc = (WidgetClass) &xmManagerClassRec;
	  else
	    wc = NULL;

	  if (wc &&
	      (er = _XmGetBaseClassExtPtr(wc, XmQmotif)) &&
	      (*er) &&
	      ((*er)->version >= XmBaseClassExtVersion) &&
	      (*er)->widgetNavigable)
            {
	      return (*((*er)->widgetNavigable))(wid);
	    }
	}
    }

  return XmNOT_NAVIGABLE;
}

Boolean
_XmIsViewable(Widget wid)
{
  /* This routine returns TRUE is the widget is realized, managed
   * and, for window objects, mapped; returns FALSE otherwise.
   * A realized RowColumn with a MenuShell parent is always considered
   * viewable.
   */
  XWindowAttributes xwa;

  if (!wid->core.being_destroyed && XtIsRealized(wid))
    {
      /* Treat menupanes specially. */
      if (XmIsRowColumn(wid) && XmIsMenuShell(XtParent(wid)))
	return TRUE;

      if (XtIsManaged(wid))
        {
	  /* CR 5593:  Trust mapped_when_managed even though the spec */
	  /*	hasn't yet been updated to allow it. */
	  if (XmIsGadget(wid) || wid->core.mapped_when_managed)
	    return TRUE;

	  /* Managed, but not necessarily mapped. */
	  XGetWindowAttributes(XtDisplay(wid), XtWindow(wid), &xwa);

	  if (xwa.map_state == IsViewable)
	    return TRUE;
	}
    }

  return FALSE;
}

Widget
_XmIsScrollableClipWidget(Widget child,
			  Boolean scrollable,
			  XRectangle *visRect)
{
  Widget wid = XtParent(child);
  XmScrolledWindowWidget sw;

  if (wid &&
      XmIsClipWindow(wid) &&
      ((sw = (XmScrolledWindowWidget) XtParent(wid)) != NULL) &&
      XmIsScrolledWindow(sw) &&
      ((Widget) sw->swindow.ClipWindow == wid) &&
      (!scrollable || sw->swindow.traverseObscuredCallback))
    {
      if (visRect)
	{
	  if (!child || !_XmSWGetClipArea(child, visRect))
	    _XmSetRect(visRect, wid);
	}

      return (Widget)sw;
    }

  return NULL;
}

Boolean
_XmGetEffectiveView(Widget wid,
		    XRectangle *visRect)
{
  /* This function will generate a rectangle describing the portion of the
   * specified widget or its scrolling area (for non-null
   * XmNtraverseObscuredCallback) which is not clipped by any of its
   * ancestors.  It also verifies that the ancestors are viewable.
   *
   * It will return TRUE if the visibility rectangle returned in visRect
   * has a non-zero area, FALSE if the visibility area is zero or one
   * of its ancestors is not viewable.
   */
  Widget prev;
  XRectangle tmpRect, widRect;
  Boolean acceptClipping = TRUE;

  if (!_XmIsViewable(wid))
    {
      _XmClearRect(visRect);
      return FALSE;
    }
  _XmSetRect(visRect, wid);

  /* Process all widgets, excluding the shell widget */
  for (prev = wid;
       ((wid = XtParent(wid)) != NULL) && !XtIsShell(wid);
       prev = wid)
    {
      if (!_XmIsViewable(wid))
        {
	  _XmClearRect(visRect);
	  return FALSE;
	}

      if (_XmIsScrollableClipWidget(prev, True, visRect))
        {
	  /* This wid is the clip window for a Scrolled Window. */
	  acceptClipping = FALSE;
	}
      else
	{
	  /* CR 9705: Special case overlapping work windows. */
	  if (!_XmIsScrollableClipWidget(prev, False, &widRect))
	    _XmSetRect(&widRect, wid);

	  if (acceptClipping)
	    {
	      if (!_XmIntersectionOf(visRect, &widRect, visRect))
		return FALSE;
	    }
	  else
	    {
	      if (!_XmIntersectionOf(visRect, &widRect, &tmpRect) ||
		  (visRect->width != tmpRect.width) ||
		  (visRect->height != tmpRect.height))
		{
		  _XmClearRect(visRect);
		  return FALSE;
		}
	    }
	}
    }

  return TRUE;
}

Boolean
_XmIntersectionOf(XRectangle *srcRectA,
		  XRectangle *srcRectB,
		  XRectangle *destRect)
{
  /* Returns TRUE if there is a non-zero area at the intersection of the
   *   two source rectangles, FALSE otherwise.  The destRect receives
   *   the rectangle of intersection (is cleared if no intersection).
   */
  int srcABot, srcBBot;
  int srcARight, srcBRight;
  int newHeight, newWidth;

  srcABot = srcRectA->y + srcRectA->height - 1;
  srcBBot = srcRectB->y + srcRectB->height - 1;
  srcARight = srcRectA->x + srcRectA->width - 1;
  srcBRight = srcRectB->x + srcRectB->width - 1;

  if (srcRectA->x >= srcRectB->x)
    destRect->x = srcRectA->x;
  else
    destRect->x = srcRectB->x;

  if (srcRectA->y > srcRectB->y)
    destRect->y = srcRectA->y;
  else
    destRect->y = srcRectB->y;

  if (srcARight >= srcBRight)
    {
      newWidth = srcBRight - destRect->x + 1;
      destRect->width = (newWidth > 0) ? newWidth : 0;
    }
  else
    {
      newWidth = srcARight - destRect->x + 1;
      destRect->width = (newWidth > 0) ? newWidth : 0;
    }

  if (srcABot > srcBBot)
    {
      newHeight = srcBBot - destRect->y + 1;
      destRect->height = (newHeight > 0) ? newHeight : 0;
    }
  else
    {
      newHeight = srcABot - destRect->y + 1;
      destRect->height = (newHeight > 0) ? newHeight : 0;
    }

  return (destRect->width && destRect->height);
}

XmNavigationType
_XmGetNavigationType(Widget widget)
{
  if (XmIsPrimitive(widget))
    return ((XmPrimitiveWidget) widget)->primitive.navigation_type;
  else if (XmIsGadget(widget))
    return ((XmGadget) widget)->gadget.navigation_type;
  else if (XmIsManager(widget))
    return ((XmManagerWidget) widget)->manager.navigation_type;
  else
    return XmNONE;
}

Widget
_XmGetActiveTabGroup(Widget wid)
{
  /* Return the active tab group for the specified widget hierarchy */
  XmFocusData focus_data = _XmGetFocusData(wid);

  if (focus_data)
    return focus_data->active_tab_group;
  else
    return NULL;
}

static Boolean
NodeIsTraversable(XmTraversalNode node)
{
  return (node &&
	  node->any.widget &&
	  (node->any.type != XmTAB_GRAPH_NODE) &&
	  (node->any.type != XmCONTROL_GRAPH_NODE) &&
	  XmIsTraversable(node->any.widget));
}

static XmTraversalNode
TraverseControl(XmTraversalNode cur_node,
		XmTraversalDirection action)
{
  XmTraversalNode new_ctl;
  XmTraversalDirection local_dir = LocalDirection(cur_node->any.widget,action);
  XmTraversalDirection dir = local_dir;

  if (!cur_node)
    return NULL;

  if (cur_node->any.type == XmCONTROL_GRAPH_NODE)
    {
      cur_node = cur_node->graph.sub_head;

      if (!cur_node)
	return NULL;

      dir = XmTRAVERSE_HOME;
    }
  else
    {
      if (cur_node->any.type != XmCONTROL_NODE)
	return NULL;
    }

  new_ctl = cur_node;
  do {
    switch(   dir   )
      {
      case XmTRAVERSE_NEXT:
	new_ctl = NextControl(new_ctl);
	break;

      case XmTRAVERSE_RIGHT:
	new_ctl = new_ctl->any.next;
	break;

      case XmTRAVERSE_UP:
	new_ctl = new_ctl->control.up;
	break;

      case XmTRAVERSE_PREV:
	new_ctl = PrevControl(new_ctl);
	break;

      case XmTRAVERSE_LEFT:
	new_ctl = new_ctl->any.prev;
	break;

      case XmTRAVERSE_DOWN:
	new_ctl = new_ctl->control.down;
	break;

      case XmTRAVERSE_HOME:
	new_ctl = new_ctl->any.tab_parent.link->sub_head;
	cur_node = new_ctl->any.tab_parent.link->sub_tail;

	/* Reset dir in case new_ctl is not traversable.*/
	if (action == XmTRAVERSE_GLOBALLY_BACKWARD)
	  {
	    new_ctl = new_ctl->any.prev;
	    cur_node = cur_node->any.prev;
	    dir = local_dir;
	  }
	else if (action == XmTRAVERSE_GLOBALLY_FORWARD)
	  dir = local_dir;
	else
	  dir = XmTRAVERSE_RIGHT;
	break;

      case XmTRAVERSE_CURRENT:
	break;

      default:
	new_ctl = NULL;
	break;
      }

    if ((new_ctl == NULL) || NodeIsTraversable(new_ctl))
      return new_ctl;
  } while (new_ctl != cur_node);

  return NULL;
}

static XmTraversalNode
NextControl(XmTraversalNode ctl_node)
{
  XmTraversalNode ptr = ctl_node;
  XmTraversalNode next = NULL;
  XmTraversalNode min = ctl_node;

  do {
    if ((ptr > ctl_node) &&
	((ptr < next) || (next == NULL)))
      next = ptr;

    if (ptr < min)
      min = ptr;

    ptr = ptr->any.next;
  } while (ptr != ctl_node);

  if (next == NULL)
    next = min;

  return next;
}

static XmTraversalNode
PrevControl(XmTraversalNode ctl_node)
{
  XmTraversalNode ptr = ctl_node;
  XmTraversalNode prev = NULL;
  XmTraversalNode max = ctl_node;

  do {
    if ((ptr < ctl_node) &&
	((ptr > prev) || (prev == NULL)))
      prev = ptr;

    if (ptr > max)
      max = ptr;

    ptr = ptr->any.prev;
  } while (ptr != ctl_node);

  if (prev == NULL)
    prev = max;

  return prev;
}

static Boolean
InitializeCurrent(XmTravGraph list,
		  Widget wid,
		  Boolean renew_list_if_needed)
{
  /* Returns TRUE if the current node has been initialized
   * to a non-NULL value.
   *
   * This routine first tries a linear search for the first
   * node in the node list.  If it fails to get a match, it
   * will do a linear search successively on each ancestor of
   * the widget, up to the shell.  If a match continues to
   * be elusive, this routine sets the current node to be
   * head of the list, which is always a tab-graph node.
   *
   * In general, this routine will either set list->current
   * to the node of the widget argument, or to a tab-graph
   * node.  The only time when the returned "current" does
   * not match the argument AND it is not a tab-graph node is
   * when an ancestor of the argument widget has navigability
   * of either XmCONTROL_NAVIGABLE or XmTAB_NAVIGABLE (such
   * as Drawing Area).
   */
  XmTraversalNode cur_node = list->current;

  if (cur_node)
    {
      if (!wid || (wid == cur_node->any.widget))
	return TRUE;

      cur_node = NULL;
    }

  if (!(cur_node = GetNodeOfWidget(list, wid)))
    {
      if (renew_list_if_needed && _XmGetNavigability(wid))
	return _XmNewTravGraph(list, list->top, wid);

      while ((wid = XtParent(wid)) &&
	     !XtIsShell(wid) &&
	     !(cur_node = GetNodeOfWidget(list, wid)))
	/*EMPTY*/;
    }

  if (cur_node)
    list->current = cur_node;
  else
    {
      if (!(list->current))
	list->current = list->head;
    }

  return TRUE;
}

Widget
_XmTraverseAway(XmTravGraph list,
		Widget wid,
		Boolean wid_is_control)
{
  /* This routine traverses away from the reference widget.  The routine
   * tries to return the widget that would be the next one traversed due
   * to a XmTRAVERSE_RIGHT or XmTRAVERSE_NEXT_TAB_GROUP.
   * When the wid_is_control argument is TRUE, this routine tries to
   * return the next traversable control following the reference widget.
   */
  if (!(list->num_entries))
    {
      if (!_XmNewTravGraph(list, list->top, wid))
	return NULL;
    }
  else
    {
      if (!InitializeCurrent(list, wid, TRUE))
	return NULL;
    }

  if ((list->current->any.widget != wid) &&
      (list->current->any.type == XmTAB_GRAPH_NODE))
    {
      XmTraversalNode nearest_node;
      XRectangle wid_rect;

      if (wid_is_control)
	{
	  /* A tab-graph node is always immediately followed by its
	   * corresponding control-graph node.
	   */
	  list->current = list->current + 1;
	}
      GetRectRelativeToShell(wid, &wid_rect);

      if ((nearest_node =
	   GetNextNearestNode((XmGraphNode) list->current,
			      &wid_rect,
			      GetLayout(list->current->any.widget))) != NULL)
	{
	  list->current = nearest_node;
	}
    }

  if ((list->current->any.widget == wid) ||
      !NodeIsTraversable(list->current))
    {
      XmTraversalNode rtnNode;

      /* If current is a control node or control graph node, try to
       * traverse to a control.  If current is a tab node, or if the
       * attempted traversal to a control node fails, then traverse
       * to the next tab group.
       */
      if (((list->current->any.type != XmCONTROL_NODE) &&
	   (list->current->any.type != XmCONTROL_GRAPH_NODE)) ||
	  !(rtnNode = TraverseControl(list->current, XmTRAVERSE_RIGHT)))
	{
	  rtnNode = TraverseTab(list->current, XmTRAVERSE_NEXT_TAB_GROUP);
	}
      list->current = rtnNode;
    }

  if (list->current && (list->current->any.widget != wid))
    return list->current->any.widget;
  else
    return NULL;
}

static XmTraversalNode
GetNextNearestNode(XmGraphNode graph,
		   XRectangle *rect,
		   XmDirection layout)
{
  XmTraversalNode node;

  if ((node = graph->sub_head) != NULL)
    {
      XmTraversalNode *list_ptr;
      unsigned idx;
      XmTraversalNode *node_list;
      XmTraversalNode storage[STACK_SORT_LIMIT];
      XmTraversalNodeRec reference;
      unsigned num_nodes = 1;    /* One node for the reference rectangle. */

      /* Count the nodes in the graph. */
      do {
	++num_nodes;
      } while ((node != graph->sub_tail) && (node = node->any.next));

      node_list = (XmTraversalNode*)
	XmStackAlloc(num_nodes * sizeof(XmTraversalNode), storage);

      /* Now copy nodes onto node list for sorting. */
      list_ptr = node_list;
      reference.any.rect = *rect; /* Only the rectangle is used in sorting. */
      reference.any.widget = NULL;
      *list_ptr++ = &reference;
      node = graph->sub_head;
      idx = 1;                    /* Leave one for reference node. */
      do {
	*list_ptr++ = node;
	node = node->any.next;
      } while (++idx < num_nodes);

      Sort(node_list, num_nodes, True, layout);

      /* Now find the reference node in the sorted list. */
      idx = 0;
      node = NULL;
      do {
	if (node_list[idx] == &reference)
	  {
	    /* Return node which follows the reference node in the
	     * sorted list.
	     */
	    ++idx;
	    if (idx == num_nodes)
	      {
		/* If reference node was the last in the list, then
		 * wrap to the beginning of the list.
		 */
		idx = 0;
	      }
	    node = node_list[idx];
	    break;
	  }
      } while (idx++ < num_nodes);

      XmStackFree((char*) node_list, storage);
    }

  return node;
}

Widget
_XmTraverse(XmTravGraph list,
	    XmTraversalDirection action,
	    XmTraversalDirection *local_dir,
	    Widget reference_wid)
{
  XmTraversalNode rtnNode;
  Boolean traverse_control;

  /* Initialize the out parameters. */
  *local_dir = action;

  if ((action == XmTRAVERSE_CURRENT) && reference_wid)
    {
      /* Try short-circuit evaluation for XmTRAVERSE_CURRENT (for
       * improved performance only; would be handled correctly below).
       */
      XmNavigability wid_nav = _XmGetNavigability(reference_wid);

      if ((wid_nav == XmTAB_NAVIGABLE) ||
	  (wid_nav == XmCONTROL_NAVIGABLE))
	{
	  if (XmIsTraversable(reference_wid))
	    return reference_wid;
	  else
	    return NULL;
	}
    }

  if (!list->num_entries)
    {
      if (!_XmNewTravGraph(list, list->top, reference_wid))
	return NULL;
    }
  else
    {
      if (!InitializeCurrent(list, reference_wid, TRUE))
	return NULL;
    }

  /* After the preceding initialization, list->current either points
   * to the node of the reference widget (or its nearest ancestor
   * which is in the traversal graph) or to the beginning of the list
   * (if the reference widget is NULL).
   *
   * If the reference widget is a composite, then there will be two
   * nodes associated with it; one containing control nodes and the
   * other containing tab nodes (including the node containing
   * is associated control nodes).  For a composite reference
   * widget, list->current will be the tab node, since it is
   * guaranteed to appear first in the list of nodes and will
   * match first in the search for the reference widget.
   */
  if (action == XmTRAVERSE_CURRENT)
    {
      if (list->current->any.widget != reference_wid)
	{
	  /* The reference widget was not found in the graph.
	   * Finding ancestors is not good enough for XmTRAVERSE_CURRENT.
	   */
	  return NULL;
	}

      if ((list->current->any.type == XmTAB_NODE) ||
	  (list->current->any.type == XmCONTROL_NODE))
	{
	  if (NodeIsTraversable(list->current))
	    return reference_wid;

	  /* Since this node has no subgraph (no traversable descendents)
	   * and is not traversable, XmTRAVERSE_CURRENT fails.
	   */
	  return NULL;
	}
    }

  /* Are we going to traverse to a control or a tab group? */
  switch (action)
    {
    case XmTRAVERSE_NEXT_TAB_GROUP:
    case XmTRAVERSE_PREV_TAB_GROUP:
      traverse_control = FALSE;
      break;

    case XmTRAVERSE_CURRENT:
      traverse_control = (list->current->any.type == XmCONTROL_GRAPH_NODE);
      break;

    case XmTRAVERSE_GLOBALLY_FORWARD:
    case XmTRAVERSE_GLOBALLY_BACKWARD:
      /* This test is slightly wrong: it should be something like */
      /* traverse_control = !LastControl() || OnlyOneTabGroup(); */
      traverse_control = !LastControl(reference_wid, action, list);
      if (traverse_control)
	*local_dir = LocalDirection(reference_wid, action);
      else
	*local_dir = (action == XmTRAVERSE_GLOBALLY_FORWARD ?
		      XmTRAVERSE_NEXT_TAB_GROUP : XmTRAVERSE_PREV_TAB_GROUP);
      break;

    default:
      traverse_control = TRUE;
      break;
    }

  if (traverse_control)
    rtnNode = TraverseControl(list->current, *local_dir);
  else
    {
      rtnNode = TraverseTab(list->current, action);
      if (!rtnNode &&
	  ((action == XmTRAVERSE_GLOBALLY_FORWARD) ||
	   (action == XmTRAVERSE_GLOBALLY_BACKWARD)))
	{
	  /* Tab traversal will fail if there is only one tab group, */
	  /* but we want to make sure global traversal succeeds. */
	  rtnNode = TraverseControl(list->current, action);
	}
    }

  if (rtnNode)
    {
      list->current = rtnNode;
      return rtnNode->any.widget;
    }
  else
    return NULL;
}

static XmTraversalNode
TraverseTab(XmTraversalNode cur_node,
	    XmTraversalDirection action)
{
  /* This routine handles tab group traversal.  It is assumed
   * that the "action" argument is not associated with traversal
   * among controls.
   *
   * Returns the leaf traversal node containing the widget which
   * would next receive the focus.
   */
  XmTraversalNode new_tab;

  if (!cur_node)
    return NULL;

  if (cur_node->any.type == XmCONTROL_NODE)
    {
      /* This routine is for tab group traversal, so if current node
       * is a control node, reset cur_node to be its tab-group control
       * graph.
       */
      cur_node = (XmTraversalNode) cur_node->any.tab_parent.link;

      /* A tab-graph node is the only node that might have a NULL
       * tab_parent, so cur_node is guaranteed to be non-NULL.
       */
    }

  new_tab = cur_node;
  do {
    switch (action)
      {
      case XmTRAVERSE_GLOBALLY_FORWARD:
      case XmTRAVERSE_NEXT_TAB_GROUP:
      case XmTRAVERSE_CURRENT:
      default:
	if ((new_tab->any.type == XmTAB_GRAPH_NODE) &&
	    (new_tab->graph.sub_head))
	  {
	    /* new_tab is a tab graph with a non-null subgraph;
	     * go down into the graph to continue the search
	     * for the next traversable node.
	     */
	    new_tab = new_tab->graph.sub_head;
	  }
	/* new_tab is not a tab graph with a non-null subgraph. */
	else if (new_tab->any.next)
	  {
	    /* Try the next node at this level. */
	    new_tab = new_tab->any.next;
	  }
	else
	  {
	    /* The next node is null, so move up out of this
	     * subgraph.  Keep going up until a non-null "next"
	     * is found, but stop and fail if going up above
	     * the cur_node if action is XmTRAVERSE_CURRENT.
	     */
	    XmTraversalNode top_node = new_tab;

	    while (((new_tab = (XmTraversalNode)
		     new_tab->any.tab_parent.link) != NULL) &&
		   ((action != XmTRAVERSE_CURRENT) || (new_tab != cur_node)) &&
		   !(new_tab->any.next))
	      {
		top_node = new_tab;
	      }

	    if ((action == XmTRAVERSE_CURRENT) && (new_tab == cur_node))
	      return NULL;

	    if (!new_tab)
	      {
		/* Got to the top level of the traversal graph
		 * without finding a non-null "next", so start
		 * the cycle over; "take it from the top".
		 */
		new_tab = top_node;
	      }
	    else
	      {
		/* We now know new_tab->any.next is non-null. */
		new_tab = new_tab->any.next;
	      }
	  }
	break;

      case XmTRAVERSE_GLOBALLY_BACKWARD:
      case XmTRAVERSE_PREV_TAB_GROUP:
	/* Same structure as for XmTRAVERSE_NEXT_TAB_GROUP,
	 * except the reverse direction.  Also, don't have
	 * to worry about XmTRAVERSE_CURRENT, which behaves
	 * like XmTRAVERSE_NEXT_TAB_GROUP for a specific subgraph.
	 */
	if ((new_tab->any.type == XmTAB_GRAPH_NODE) &&
	    (new_tab->graph.sub_tail))
	  {
	    new_tab = new_tab->graph.sub_tail;
	  }
	else if (new_tab->any.prev)
	  {
	    new_tab = new_tab->any.prev;
	  }
	else
	  {
	    XmTraversalNode top_node = new_tab;

	    while ((new_tab = (XmTraversalNode)new_tab->any.tab_parent.link) &&
		   !(new_tab->any.prev))
	      {
		top_node = new_tab;
	      }

	    if (!new_tab)
	      new_tab = top_node;
	    else
	      new_tab = new_tab->any.prev;
	  }
	break;
      }

    if (new_tab->any.type == XmCONTROL_GRAPH_NODE)
      {
	/* While tabbing, the only way to know whether a control graph
	 * is traversable is to attempt to traverse to it; can't depend
	 * on the ensuing NodeIsTraversable().
	 */
	XmTraversalNode rtnNode = TraverseControl(new_tab, action);
	if (rtnNode)
	  return rtnNode;
      }

    if (new_tab == cur_node)
      {
	/* This is how we get out when there are no traversable
	 * nodes in this traversal graph.  We have traversed
	 * the entire graph and have come back to the starting
	 * point, so fail.
	 */
	return NULL;
      }
  } while (!NodeIsTraversable(new_tab));

  return new_tab;
}

void
_XmFreeTravGraph(XmTravGraph trav_list)
{
  if (trav_list->num_alloc)
    {
      XtFree((char *) trav_list->head);
      trav_list->head = NULL;
      trav_list->next_alloc = trav_list->num_alloc;
      trav_list->num_alloc = 0;
      trav_list->num_entries = 0;
      trav_list->current = NULL;
      trav_list->top = NULL;
    }
}

void
_XmTravGraphRemove(XmTravGraph tgraph,
		   Widget wid)
{
  XmTraversalNode node;

  if (tgraph->num_entries)
    {
      while ((node = GetNodeOfWidget(tgraph, wid)) != NULL)
	node->any.widget = NULL;
    }
}

void
_XmTravGraphAdd(XmTravGraph tgraph,
		Widget wid)
{
  if (tgraph->num_entries &&
      !GetNodeOfWidget(tgraph, wid))
    _XmFreeTravGraph(tgraph);
}

void
_XmTravGraphUpdate(XmTravGraph tgraph,
		   Widget wid)	/* unused */
{
  _XmFreeTravGraph(tgraph);
}

Boolean
_XmNewTravGraph(XmTravGraph trav_list,
		Widget top_wid,
		Widget init_current)
{
  /* This procedure assumes that trav_list has been zeroed.
   * before the initial call to _XmNewTravGraph.  Subsequent
   * calls to _XmFreeTravGraph and _XmNewTravGraph do not
   * require re-initialization.
   */
  XRectangle w_rect;

  if (top_wid)
    {
      trav_list->top = top_wid;
    }
  else
    {
      if (!(trav_list->top))
	{
	  top_wid = init_current;
	  while (top_wid  &&  !XtIsShell(top_wid))
	    top_wid = XtParent(top_wid);
	  trav_list->top = top_wid;
	}
    }
  if (!trav_list->top || trav_list->top->core.being_destroyed)
    {
      _XmFreeTravGraph(trav_list);
      return FALSE;
    }
  trav_list->num_entries = 0;
  trav_list->current = NULL;

  /* Traversal topography uses a coordinate system which is relative
   * to the shell window (avoids toolkit/window manager involvement).
   * Initialize x and y to cancel to zero when shell coordinates are
   * added in.
   */
  w_rect.x = - (Position) (XtX(top_wid) + XtBorderWidth(top_wid));
  w_rect.y = - (Position) (XtY(top_wid) + XtBorderWidth(top_wid));
  w_rect.width = XtWidth(top_wid);
  w_rect.height = XtHeight(top_wid);

  GetNodeList(top_wid, &w_rect, trav_list, -1, -1);

  if ((trav_list->num_entries + XmTRAV_LIST_ALLOC_INCREMENT) <
      trav_list->num_alloc)
    {
      /* Except for the very first allocation of the traversal graph for
       * this shell hierarchy, the size of the initial allocation is based
       * on the number of nodes allocated for the preceding version of the
       * graph (as per the next_alloc field of the XmTravGraph).  To prevent
       * a "grow-only" behavior in the size of the allocation, we must
       * routinely attempt to prune excessive memory allocation.
       */
      trav_list->num_alloc -= XmTRAV_LIST_ALLOC_INCREMENT;
      trav_list->head = (XmTraversalNode) XtRealloc((char *) trav_list->head,
		          trav_list->num_alloc * sizeof(XmTraversalNodeRec));
    }

  LinkNodeList(trav_list);

  SortNodeList(trav_list);

  SetInitialWidgets(trav_list);

  InitializeCurrent(trav_list, init_current, FALSE);

#ifdef DEBUG_TRAVERSAL
  PrintNodeList(trav_list);
#endif

  return TRUE;
}

#define UnallocLastListEntry(list) (--(list->num_entries))

static XmTraversalNode
AllocListEntry(
        XmTravGraph list)
{
  if (!(list->num_alloc))
    {
      /* Use next_alloc (from previous allocation of list)
       * as starting point for size of array.
       */
      if (list->next_alloc)
	list->num_alloc = list->next_alloc;
      else
	list->num_alloc = XmTRAV_LIST_ALLOC_INCREMENT;

      list->head = (XmTraversalNode)
	XtMalloc(list->num_alloc * sizeof(XmTraversalNodeRec));
    }
  else
    {
      if (list->num_entries == list->num_alloc)
	{
	  list->num_alloc += XmTRAV_LIST_ALLOC_INCREMENT;

	  list->head = (XmTraversalNode)
	    XtRealloc((char *) list->head,
		      list->num_alloc * sizeof(XmTraversalNodeRec));
	}
    }

  return &(list->head[list->num_entries++]);
}

static Boolean
GetChildList(Widget composite,
	     Widget **widget_list,
	     Cardinal *num_in_list)
{
  XmManagerClassExt *mext;

  if (XmIsManager(composite) &&
      (mext = (XmManagerClassExt *)
       _XmGetClassExtensionPtr((XmGenericClassExt *)
			       &(((XmManagerWidgetClass) XtClass(composite))
				 ->manager_class.extension), NULLQUARK)) &&
      *mext &&
      (*mext)->traversal_children)
    {
      return (*((*mext)->traversal_children))(composite,
					      widget_list,
					      num_in_list);
    }

  return FALSE;
}

static void
GetNodeList(Widget wid,
	    XRectangle *parent_rect,
	    XmTravGraph trav_list,
	    int tab_parent,
	    int control_parent)
{
  /* This routine returns a node list of all navigable widgets in
   * a sub-hierarchy.  The order of the list is such that the node
   * of widget's parent is guaranteed to precede the child widget's
   * node.
   */
  /* This routine fills in the following fields of the
   * traversal nodes by this routine:
   *   any.type
   *   any.widget
   *   any.nav_type
   *   any.rect
   *   any.tab_parent (initialized by offset)
   *   graph.sub_head (set to NULL)
   *   graph.sub_tail (set to NULL)
   */
  XmTraversalNode list_entry;
  int list_entry_offset;
  XmNavigability node_type;

  if (wid->core.being_destroyed ||
      (!(node_type = _XmGetNavigability(wid)) && !XtIsShell(wid)))
    return;

  list_entry_offset = (int) trav_list->num_entries;
  list_entry = AllocListEntry(trav_list);

  list_entry->any.widget = wid;
  list_entry->any.rect.x = parent_rect->x + XtX(wid) + XtBorderWidth(wid);
  list_entry->any.rect.y = parent_rect->y + XtY(wid) + XtBorderWidth(wid);
  list_entry->any.rect.width = XtWidth(wid);
  list_entry->any.rect.height = XtHeight(wid);
  /* Bootstrap: first entry must behave like a tab group. */
  list_entry->any.nav_type = (list_entry_offset ?
			      _XmGetNavigationType(wid) : XmSTICKY_TAB_GROUP);

  if (node_type == XmCONTROL_NAVIGABLE)
    {
      list_entry->any.type = XmCONTROL_NODE;
      list_entry->any.tab_parent.offset = control_parent;
    }
  else if (node_type == XmTAB_NAVIGABLE)
    {
      list_entry->any.type = XmTAB_NODE;
      list_entry->any.tab_parent.offset = tab_parent;
    }
  else if (((node_type == XmNOT_NAVIGABLE) && list_entry_offset) ||
	   !XtIsComposite(wid))
    {
      UnallocLastListEntry(trav_list);
    }
  else /* XmDESCENDANTS_NAVIGABLE or XmDESCENDANTS_TAB_NAVIGABLE. */
    {
      int controls_graph_offset;
      XmTraversalNode controls_graph;
      int i;
      Widget *trav_children;
      Cardinal num_trav_children;
      Boolean free_child_list;
      XRectangle tmp_rect;
      XRectangle *list_entry_rect = &tmp_rect;

      tmp_rect  = list_entry->any.rect;

      if (node_type == XmDESCENDANTS_NAVIGABLE)
	{
	  /* This Composite has no purpose in the traversal
	   * graph, so use the passed-in parents and a
	   * temporary copy of this widget's rectangle to
	   * avoid the need for the memory of this list entry.
	   */
	  controls_graph_offset = control_parent;
	  list_entry_offset = tab_parent;
	  tmp_rect = *list_entry_rect;
	  list_entry_rect = &tmp_rect;
	  UnallocLastListEntry(trav_list);
	}
      else /* (node_type == XmDESCENDANTS_TAB_NAVIGABLE) */
	{
	  list_entry->any.type = XmTAB_GRAPH_NODE;
	  list_entry->graph.sub_head = NULL;
	  list_entry->graph.sub_tail = NULL;
	  list_entry->any.tab_parent.offset = tab_parent;

	  controls_graph_offset = list_entry_offset + 1;
	  controls_graph = AllocListEntry(trav_list);
	  *controls_graph = trav_list->head[list_entry_offset];
	  controls_graph->any.tab_parent.offset = list_entry_offset;
	  controls_graph->any.type = XmCONTROL_GRAPH_NODE;
	}

      if (!(free_child_list = GetChildList(wid, &trav_children,
					   &num_trav_children)))
	{
	  trav_children = ((CompositeWidget) wid)->composite.children;
	  num_trav_children = ((CompositeWidget) wid)->composite.num_children;
	}

      for (i = 0; i < num_trav_children; i++)
	GetNodeList(trav_children[i], list_entry_rect, trav_list,
		    list_entry_offset, controls_graph_offset);

      if (free_child_list)
	XtFree((char *) trav_children);
    }
}

static XmTraversalNode
GetNodeFromGraph(XmGraphNode graph,
		 Widget wid)
{
  XmTraversalNode node;

  if (wid && (node = graph->sub_head))
    {
      do {
	if (node->any.widget == wid)
	  return node;
      } while ((node != graph->sub_tail) && (node = node->any.next));
    }

  return NULL;
}

static XmTraversalNode
GetNodeOfWidget(XmTravGraph trav_list,
		Widget wid)
{
  /* This function returns the first node in the list to match the
   * widget argument.
   *
   * Note that tab-group composites will have two nodes in the
   * node list; a tab-graph node and a control-graph node.
   * The first match will always be the tab-graph node and
   * the corresponding control-graph node will always be the
   * next node in the list; many callers of this routine depend
   * on this behavior.
   *
   * Also note that the graph list is maintained such that
   * "obsolete" nodes have the widget id field set to NULL;
   * this ensures that this routine will never return a node
   * which is not part of the current linked traversal graph.
   */

  if (wid)
    {
      unsigned cnt = 0;
      XmTraversalNode list_ptr = trav_list->head;

      while (cnt++ < trav_list->num_entries)
	{
	  if (list_ptr->any.widget == wid)
	    return list_ptr;

	  ++list_ptr;
	}
    }

  return NULL;
}

static void
LinkNodeList(XmTravGraph list)
{
  XmTraversalNode head = list->head;
  XmTraversalNode entry = head;
  unsigned cnt = 0;

  while (cnt++ < list->num_entries)
    {
      XmGraphNode parent;

      if (entry->any.tab_parent.offset < 0)
	parent = NULL;
      else
	parent = (XmGraphNode) &(head[entry->any.tab_parent.offset]);

      entry->any.tab_parent.link = parent;

      if (parent)
	{
	  if (!parent->sub_tail)
	    parent->sub_head = entry;
	  else
	    parent->sub_tail->any.next = entry;

	  entry->any.prev = parent->sub_tail;
	  entry->any.next = NULL;
	  parent->sub_tail = entry;
	}
      else
	{
	  entry->any.prev = NULL;
	  entry->any.next = NULL;
	}

      ++entry;
    }
}

/*
 * A helper for Sort().  Determine whether two nodes overlap enough to
 * be considered part of the same row.
 */
static Boolean
NodesOverlap(XmTraversalNode nodeA,
	     XmTraversalNode nodeB,
	     Boolean 	     horizontal)
{
  Dimension Acent, Bcent;

  if (horizontal)
    {
      Acent = nodeA->any.rect.y + ((nodeA->any.rect.height) >> 1);
      Bcent = nodeB->any.rect.y + ((nodeB->any.rect.height) >> 1);

      if (((nodeA->any.rect.y + nodeA->any.rect.height) < Bcent) &&
	  (Acent < (Dimension) nodeB->any.rect.y))
	return False;
      else if (((nodeB->any.rect.y + nodeB->any.rect.height) < Acent) &&
	       (Bcent < (Dimension) nodeA->any.rect.y))
	return False;
    }
  else
    {
      Acent = nodeA->any.rect.x + ((nodeA->any.rect.width) >> 1);
      Bcent = nodeB->any.rect.x + ((nodeB->any.rect.width) >> 1);

      if (((nodeA->any.rect.x + nodeA->any.rect.width) < Bcent) &&
	  (Acent < (Dimension) nodeB->any.rect.x))
	return False;
      else if (((nodeB->any.rect.x + nodeB->any.rect.width) < Acent) &&
	       (Bcent < (Dimension) nodeA->any.rect.x))
	return False;
    }

  return True;
}

/*
 * A helper for Sort().  Truncate a row to a given length, putting
 * orphaned items back into the free list in order.
 */
static void
TruncateRow(XmTraversalRow  *row,
	    Cardinal         length,
	    XmTraversalNode *free_list,
	    Cardinal        *first_free,
	    Cardinal	     max_free,
	    Boolean	     horizontal,
	    XmDirection      layout)
{
  Cardinal tmp;
  XmTraversalNode item;
  Comparator compare =
    (horizontal ? HorizNodeComparator(layout) : VertNodeComparator(layout));

  assert(length <= row->num_items);
  while (row->num_items > length)
    {
      /* Select an item to free. */
      item = row->items[--(row->num_items)];
      free_list[(*first_free)--] = item;
      assert(((int)*first_free) >= 0);

      /* This can't be the determining item for sorting rows. */
      if (item == row->lead_item)
	row->lead_item = NULL;

      /* Push it back onto the free list in sorted order. */
      for (tmp = *first_free + 2; tmp < max_free; tmp++)
	{
	  if (compare (free_list + tmp, &item) < 0)
	    {
	      free_list[tmp - 1] = free_list[tmp];
	      free_list[tmp] = item;
	    }
	  else
	    {
	      /* The free list was sorted initially, so we're done. */
	      break;
	    }
	}
    }
}

/* A helper for Sort().  Append a node to a row. */
static void
AppendToRow(XmTraversalNode item,
	    XmTraversalRow *row,
	    Boolean	    horizontal,
	    XmDirection     layout)
{
  Cardinal tmp;

  /* Make sure there is enough room in the array. */
  assert (row->num_items <= row->max_items);
  if (row->num_items == row->max_items)
    {
      row->max_items += 10;
      row->items = (XmTraversalNode *)
	XtRealloc((char*)row->items, row->max_items * sizeof(XmTraversalNode));
    }

  /* Append this item.*/
  row->items[row->num_items++] = item;

  /* Update cached row information. */
  tmp = row->num_items - 1;
  if (row->lead_item == NULL)
    {
      row->lead_item = row->items[0];
      tmp = 1;
      row->min_hint = 32767;	/* Should be MAX_POSITION. */
      row->max_hint = -32768;	/* Should be MIN_POSITION. */
    }
  for (; tmp < row->num_items; tmp++)
    {
      XmTraversalNode node = row->items[tmp];

      /* Look for a new dominant item. */
      if (NodeDominates(node, row->lead_item, horizontal, layout))
	row->lead_item = node;

      /* Look for new bounds hints. */
      if (horizontal)
	{
	  ASSIGN_MIN(row->min_hint, node->any.rect.y);
	  ASSIGN_MAX(row->max_hint, node->any.rect.y + node->any.rect.height);
	}
      else
	{
	  ASSIGN_MIN(row->min_hint, node->any.rect.x);
	  ASSIGN_MAX(row->max_hint, node->any.rect.x + node->any.rect.width);
	}
    }
}

/*
 * A helper for Sort().  Test whether a node dominates another node
 * in the same row.  A dominant node may force itself into an existing
 * row even if it has to displace nodes previously assigned to that row.
 */
static Boolean
NodeDominates(XmTraversalNode node_1,
	      XmTraversalNode node_2,
	      Boolean	      horizontal,
	      XmDirection     layout)
{
  if (horizontal)
    {
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return (node_1->any.rect.y < node_2->any.rect.y);
      else
	return ((node_1->any.rect.y + node_1->any.rect.height) >
		(node_2->any.rect.y + node_2->any.rect.height));
    }
  else
    {
      if (XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT, XmHORIZONTAL_MASK))
	return (node_1->any.rect.x < node_2->any.rect.x);
      else
	return ((node_1->any.rect.x + node_1->any.rect.width) >
		(node_2->any.rect.x + node_2->any.rect.width));
    }
}

static void
Sort(XmTraversalNode *list,
     size_t           n_mem,
     Boolean	      horizontal,
     XmDirection      layout)
{
  /* This rather involved algorithm is intended to produce a grid-like */
  /* traversal order (like the keys on a telephone), but at the same */
  /* time use an "intuitive" definition of which nodes form a row. */
  XmTraversalRow *rows = NULL;
  int row;
  int num_rows = 0;
  Cardinal first_free = (Cardinal) -1;

  /* Take the easy out in degenerate cases. */
  if (n_mem <= 1)
    return;

  /* First sort everything based simply on the absolute coordinates. */
  if (horizontal)
    qsort(list, n_mem, sizeof(XmTraversalNode), HorizNodeComparator(layout));
  else
    qsort(list, n_mem, sizeof(XmTraversalNode), VertNodeComparator(layout));

  /* Assign each free node to a row. */
  while (++first_free < n_mem)
    {
      XmTraversalNode node = list[first_free];
      Boolean new_row = True;
      Boolean done = False;
      Boolean might_overlap = False ;
      Cardinal row_len = 0;

      row = 0;
      while (!done && (row < num_rows))
	{
	  /* Extract some row-specific information. */
	  if (new_row)
	    {
	      new_row = False;
	      row_len = rows[row].num_items;

	      if (horizontal)
              {
		might_overlap = (rows[row].max_hint > node->any.rect.y) &&
		  (rows[row].min_hint < (node->any.rect.y +
					 node->any.rect.height));
              }
              else
              {
		might_overlap = (rows[row].max_hint > node->any.rect.x) &&
		  (rows[row].min_hint < (node->any.rect.x +
					 node->any.rect.width));
              }
	    }

	  /* Append this node if it overlaps the end of the row. */
	  if (NodesOverlap(node, rows[row].items[row_len - 1], horizontal))
	    {
	      TruncateRow(rows + row, row_len, list, &first_free, n_mem,
			  horizontal, layout);
	      AppendToRow(node, rows + row, horizontal, layout);
	      done = True;
	    }

	  /* Else if this node is dominant backtrack and try again. */
	  else if ((row_len > 1) && might_overlap &&
		   NodeDominates(node, rows[row].items[row_len-1], horizontal, layout))
	    row_len--;

	  /* Try the next row. */
	  else
	    {
	      row++;
	      new_row = True;
	    }
	}

      /* Create a new row if necessary. */
      if (!done)
	{
	  XmTraversalRow new_data;
	  Cardinal new_index = num_rows;

	  new_data.items = XtNew(XmTraversalNode);
	  new_data.items[0] = node;
	  new_data.lead_item = node;
	  new_data.num_items = 1;
	  new_data.max_items = 1;
	  if (horizontal)
	    {
	      new_data.min_hint = node->any.rect.y;
	      new_data.max_hint = node->any.rect.y + node->any.rect.height;
	    }
	  else
	    {
	      new_data.min_hint = node->any.rect.x;
	      new_data.max_hint = node->any.rect.x + node->any.rect.width;
	    }

	  num_rows++;
	  rows = (XmTraversalRow *)
	    XtRealloc((char*) rows, num_rows * sizeof(XmTraversalRow));

	  /* Keep rows sorted by initial element. */
	  for (row = new_index; row > 0; row--)
	    {
	      if (NodeDominates(node, rows[row-1].lead_item, horizontal,layout))
		{
		  memcpy(rows + row, rows + row - 1, sizeof(XmTraversalRow));
		  new_index--;
		}
	      else
		break;
	    }

	  memcpy(rows + new_index, &new_data, sizeof(XmTraversalRow));
	}
    }
  assert(first_free == n_mem);

  /* Copy nodes out of rows and into the final order. */
  first_free = 0;
  if ((horizontal &&
       XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT, XmHORIZONTAL_MASK)) ||
      (!horizontal &&
       XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK)))
    {
      /* Nodes appear in the desired order. */
      for (row = 0; row < num_rows; row++)
	{
	  assert(rows[row].num_items && rows[row].items);
	  memcpy(list + first_free, rows[row].items,
		 sizeof(XmTraversalNode) * rows[row].num_items);
	  first_free += rows[row].num_items;
	  XtFree((char*) rows[row].items);
	}
    }
  else
    {
      /* Nodes are in exactly the wrong order, but the head is right. */
      int col;
      for (row = num_rows - 1; row >= 0; row--)
	{
	  assert(rows[row].num_items && rows[row].items);
	  for (col = rows[row].num_items - 1; col >= 0; col--)
	    list[++first_free % n_mem] = rows[row].items[col];
	  XtFree((char*) rows[row].items);
	}
    }
  assert(first_free == n_mem);
  XtFree((char*) rows);
}

static int CompareExclusive(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;
  int PositionA = SearchTabList(SortReferenceGraph, nodeA->any.widget);
  int PositionB = SearchTabList(SortReferenceGraph, nodeB->any.widget);

  if (PositionA < PositionB)
    return -1;
  else if (PositionA > PositionB)
    return 1;
  else
    return 0;
}

/* Select a horizontal comparator for this layout direction. */
static Comparator HorizNodeComparator(XmDirection layout)
{
  if (XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT, XmHORIZONTAL_MASK))
    {
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return CompareNodesHorizLT;
      else
	return CompareNodesHorizLB;
    }
  else
    {
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return CompareNodesHorizRT;
      else
	return CompareNodesHorizRB;
    }
}

/* Compare nodes horizontally in a Left-to-Right, Top-to-Bottom layout. */
static int CompareNodesHorizLT(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if (nodeA->any.rect.x != nodeB->any.rect.x)
    return (nodeA->any.rect.x < nodeB->any.rect.x) ? -1 : 1;

  if (nodeA->any.rect.y != nodeB->any.rect.y)
    return (nodeA->any.rect.y < nodeB->any.rect.y) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  return 0;
}

/* Compare nodes horizontally in a Right-to-Left, Top-to-Bottom layout. */
static int CompareNodesHorizRT(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if ((nodeA->any.rect.x + nodeA->any.rect.width) !=
      (nodeB->any.rect.x + nodeB->any.rect.width))
    return ((nodeA->any.rect.x + nodeA->any.rect.width) >
	    (nodeB->any.rect.x + nodeB->any.rect.width)) ? -1 : 1;

  if (nodeA->any.rect.y != nodeB->any.rect.y)
    return (nodeA->any.rect.y < nodeB->any.rect.y) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  return 0;
}

/* Compare nodes horizontally in a Left-to-Right, Bottom-to-Top layout. */
static int CompareNodesHorizLB(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if (nodeA->any.rect.x != nodeB->any.rect.x)
    return (nodeA->any.rect.x < nodeB->any.rect.x) ? -1 : 1;

  if ((nodeA->any.rect.y + nodeA->any.rect.height) !=
      (nodeB->any.rect.y + nodeB->any.rect.height))
    return ((nodeA->any.rect.y + nodeA->any.rect.height) >
	    (nodeB->any.rect.y + nodeB->any.rect.height)) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  return 0;
}

/* Compare nodes horizontally in a Right-to-Left, Bottom-to-Top layout. */
static int CompareNodesHorizRB(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if ((nodeA->any.rect.x + nodeA->any.rect.width) !=
      (nodeB->any.rect.x + nodeB->any.rect.width))
    return ((nodeA->any.rect.x + nodeA->any.rect.width) >
	    (nodeB->any.rect.x + nodeB->any.rect.width)) ? -1 : 1;

  if ((nodeA->any.rect.y + nodeA->any.rect.height) !=
      (nodeB->any.rect.y + nodeB->any.rect.height))
    return ((nodeA->any.rect.y + nodeA->any.rect.height) >
	    (nodeB->any.rect.y + nodeB->any.rect.height)) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  return 0;
}

/* Select a vertical comparator for this layout direction. */
static Comparator VertNodeComparator(XmDirection layout)
{
  if (XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT, XmHORIZONTAL_MASK))
    {
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return CompareNodesVertLT;
      else
	return CompareNodesVertLB;
    }
  else
    {
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return CompareNodesVertRT;
      else
	return CompareNodesVertRB;
    }
}

/* Compare nodes vertically in a Left-to-Right, Top-to-Bottom layout. */
static int CompareNodesVertLT(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if (nodeA->any.rect.y != nodeB->any.rect.y)
    return (nodeA->any.rect.y < nodeB->any.rect.y) ? -1 : 1;

  if (nodeA->any.rect.x != nodeB->any.rect.x)
    return (nodeA->any.rect.x < nodeB->any.rect.x) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  return 0;
}

/* Compare nodes vertically in a Right-to-Left, Top-to-Bottom layout. */
static int CompareNodesVertRT(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(XmTraversalNode *)B;

  if (nodeA->any.rect.y != nodeB->any.rect.y)
    return (nodeA->any.rect.y < nodeB->any.rect.y) ? -1 : 1;

  if ((nodeA->any.rect.x + nodeA->any.rect.width) !=
      (nodeB->any.rect.x + nodeB->any.rect.width))
    return ((nodeA->any.rect.x + nodeA->any.rect.width) >
	    (nodeB->any.rect.x + nodeB->any.rect.width)) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  return 0;
}

/* Compare nodes vertically in a Left-to-Right, Bottom-to-Top layout. */
static int CompareNodesVertLB(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(XmTraversalNode *)B;

  if ((nodeA->any.rect.y + nodeA->any.rect.height) !=
      (nodeB->any.rect.y + nodeB->any.rect.height))
    return ((nodeA->any.rect.y + nodeA->any.rect.height) >
	    (nodeB->any.rect.y + nodeB->any.rect.height)) ? -1 : 1;

  if (nodeA->any.rect.x != nodeB->any.rect.x)
    return (nodeA->any.rect.x < nodeB->any.rect.x) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  return 0;
}

/* Compare nodes vertically in a Right-to-Left, Bottom-to-Top layout. */
static int CompareNodesVertRB(const void *A, const void *B)
{
  const XmTraversalNode nodeA = *(const XmTraversalNode *)A;
  const XmTraversalNode nodeB = *(const XmTraversalNode *)B;

  if ((nodeA->any.rect.y + nodeA->any.rect.height) !=
      (nodeB->any.rect.y + nodeB->any.rect.height))
    return ((nodeA->any.rect.y + nodeA->any.rect.height) >
	    (nodeB->any.rect.y + nodeB->any.rect.height)) ? -1 : 1;

  if ((nodeA->any.rect.x + nodeA->any.rect.width) !=
      (nodeB->any.rect.x + nodeB->any.rect.width))
    return ((nodeA->any.rect.x + nodeA->any.rect.width) >
	    (nodeB->any.rect.x + nodeB->any.rect.width)) ? -1 : 1;

  if (nodeA->any.rect.width != nodeB->any.rect.width)
    return (nodeA->any.rect.width < nodeB->any.rect.width) ? -1 : 1;

  if (nodeA->any.rect.height != nodeB->any.rect.height)
    return (nodeA->any.rect.height < nodeB->any.rect.height) ? -1 : 1;

  return 0;
}

/* Tab groups are sorted by "forward" direction. */

static void
SortTabGraph(XmGraphNode graph,
	     Boolean exclusive,
	     XmDirection layout)
{
  XmTraversalNode head = graph->sub_head;

  if (head)
    {
      XmTraversalNode node = head;
      XmTraversalNode *list_ptr;
      unsigned num_nodes = 1;
      unsigned idx;
      XmTraversalNode *node_list;
      XmTraversalNode storage[STACK_SORT_LIMIT];

      while ((node = node->any.next) != NULL)
	++num_nodes;

      node_list = (XmTraversalNode*)
	XmStackAlloc(num_nodes * sizeof(XmTraversalNode), storage);

      /* Now copy nodes onto node list for sorting. */
      node = head;
      list_ptr = node_list;
      do {
	*list_ptr++ = node;
      } while ((node = node->any.next) != NULL);

      assert(graph->any.type == XmTAB_GRAPH_NODE);
      if (num_nodes > 1)
	{
	  if (exclusive)
	    qsort(node_list, num_nodes, sizeof(XmTraversalNode),
		  CompareExclusive);
	  else
	    {
	      /* For non-exclusive tab traversal, leave the control
	       * subgraph at the beginning of the list, since it
	       * always gets first traversal.
	       */
	      Boolean horizontal;
	      Boolean reverse;

	      /* Sort axis depends on layout precedence. */
	      horizontal = XmDirectionMatchPartial(layout,
						   XmPRECEDENCE_HORIZ_MASK,
						   XmPRECEDENCE_MASK);
	      reverse = (horizontal ?
			 !XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT,
						  XmHORIZONTAL_MASK) :
			 !XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM,
						  XmVERTICAL_MASK));

	      Sort((node_list + 1), (num_nodes - 1), horizontal, layout);

	      if (reverse)
		{
		  XmTraversalNode tmp;

		  /* If right/down is not forward reverse the list. */
		  for (idx = 0; idx < ((num_nodes - 1) / 2); idx++)
		    {
		      memcpy((char*) &tmp,
			     (char*) (node_list + idx + 1),
			     sizeof(XmTraversalNode));
		      memcpy((char*) (node_list + idx + 1),
			     (char*) (node_list + num_nodes - idx - 1),
			     sizeof(XmTraversalNode));
		      memcpy((char*) (node_list + num_nodes - idx - 1),
			     (char*) &tmp,
			     sizeof(XmTraversalNode));
		    }

		  /* Rotate the initial element into the proper position. */
		  if (num_nodes > 2)
		    {
		      memcpy((char*) &tmp,
			     (char*) (node_list + num_nodes - 1),
			     sizeof(XmTraversalNode));
		      memmove((char*) (node_list + 2),
			      (char*) (node_list + 1),
			      (num_nodes - 2) * sizeof(XmTraversalNode));
		      memcpy((char*) (node_list + 1),
			     (char*) &tmp,
			     sizeof(XmTraversalNode));
		    }
		}
	    }
	}

      list_ptr = node_list;
      graph->sub_head = *list_ptr;
      (*list_ptr)->any.prev = NULL;

      idx = 0;
      while (++idx < num_nodes)
	{
	  /* This loop does one less than the number of nodes. */
	  (*list_ptr)->any.next = *(list_ptr + 1);
	  ++list_ptr;
	  (*list_ptr)->any.prev = *(list_ptr - 1);
	}
      (*list_ptr)->any.next = NULL;
      graph->sub_tail = *list_ptr;

      XmStackFree((char*) node_list, storage);
    }
}

/* Controls are sorted by absolute direction. */

static void
SortControlGraph(XmGraphNode graph,
		 Boolean exclusive,
		 XmDirection layout)
{
  XmTraversalNode head = graph->sub_head;

  if (head)
    {
      XmTraversalNode node = head;
      XmTraversalNode *list_ptr;
      unsigned num_nodes = 1;
      unsigned idx;
      XmTraversalNode *node_list;
      XmTraversalNode storage[STACK_SORT_LIMIT];

      while ((node = node->any.next) != NULL)
	++num_nodes;

      node_list = (XmTraversalNode*)
	XmStackAlloc(num_nodes * sizeof(XmTraversalNode), storage);

      /* Now copy nodes onto node list for sorting. */
      node = head;
      list_ptr = node_list;
      do {
	*list_ptr++ = node;
      } while ((node = node->any.next) != NULL);

      assert(graph->any.type == XmCONTROL_GRAPH_NODE);
      if (!exclusive || (graph->any.nav_type == XmSTICKY_TAB_GROUP))
	Sort(node_list, num_nodes, True, layout);

      list_ptr = node_list;
      graph->sub_head = *list_ptr;
      (*list_ptr)->any.prev = NULL;

      idx = 0;
      while (++idx < num_nodes)
	{
	  /* This loop does one less than the number of nodes. */
	  (*list_ptr)->any.next = *(list_ptr + 1);
	  ++list_ptr;
	  (*list_ptr)->any.prev = *(list_ptr - 1);
	}
      (*list_ptr)->any.next = NULL;
      graph->sub_tail = *list_ptr;

      /* Add links for wrapping behavior of control nodes. */
      graph->sub_head->any.prev = graph->sub_tail;
      graph->sub_tail->any.next = graph->sub_head;

      /* Now use current list to sort again, this time for up and down. */
      if (!exclusive || (graph->any.nav_type == XmSTICKY_TAB_GROUP))
	Sort(node_list, num_nodes, False, layout);

      list_ptr = node_list;
      (*list_ptr)->control.up = list_ptr[num_nodes - 1];

      idx = 0;
      while (++idx < num_nodes)
	{
	  /* This loop does one less than the number of nodes. */
	  (*list_ptr)->control.down = *(list_ptr + 1);
	  ++list_ptr;
	  (*list_ptr)->control.up = *(list_ptr - 1);
	}
      (*list_ptr)->control.down = *node_list;

      /* If vertical precedence is required set initial focus on */
      /* the first column instead of the first row. */
      if (! XmDirectionMatchPartial(layout, XmPRECEDENCE_HORIZ_MASK,
				    XmPRECEDENCE_MASK))
	{
	  graph->sub_head = *node_list;
	  graph->sub_tail = *list_ptr;
	}

      XmStackFree((char*) node_list, storage);
    }
}

static void
SortNodeList(XmTravGraph trav_list)
{
  XmTraversalNode ptr = trav_list->head;
  unsigned cnt = 0;

  _XmProcessLock();
  SortReferenceGraph = trav_list; /* Slime for CompareExclusive(). */

  while (cnt++ < trav_list->num_entries)
    {
      if (ptr->any.type == XmTAB_GRAPH_NODE)
	{
	  SortTabGraph((XmGraphNode) ptr, !!(trav_list->exclusive),
		       GetLayout(ptr->any.widget));
	}
      else if (ptr->any.type == XmCONTROL_GRAPH_NODE)
	{
	  SortControlGraph((XmGraphNode) ptr, !!(trav_list->exclusive),
			   GetLayout(ptr->any.widget));
	}

      ++ptr;
    }
    _XmProcessUnlock();
}

Boolean
_XmSetInitialOfTabGraph(XmTravGraph trav_graph,
			Widget tab_group,
			Widget init_focus)
{
  XmTraversalNode tab_node = GetNodeOfWidget(trav_graph, tab_group);
  XmGraphNode control_graph_node;

  if (tab_node &&
      ((tab_node->any.type == XmTAB_GRAPH_NODE) ||
       (tab_node->any.type == XmCONTROL_GRAPH_NODE)))
    {
      if (SetInitialNode((XmGraphNode) tab_node,
		         GetNodeFromGraph((XmGraphNode) tab_node,
					  init_focus)) ||
	  ((control_graph_node = (XmGraphNode)
	    GetNodeFromGraph((XmGraphNode) tab_node, tab_group)) &&
	   SetInitialNode(control_graph_node,
			  GetNodeFromGraph(control_graph_node, init_focus)) &&
	   SetInitialNode((XmGraphNode) tab_node,
			  (XmTraversalNode) control_graph_node)))
	{
	  return TRUE;
	}
    }

  return FALSE;
}

static Boolean
SetInitialNode(XmGraphNode graph,
	       XmTraversalNode init_node)
{
  /* It is presumed that init_node was derived from a call
   * to GetNodeFromGraph with the same "graph" argument.
   * It is a bug if the parent of init_node is not graph.
   */
  if (init_node)
    {
      if (init_node != graph->sub_head)
	{
	  if (graph->any.type == XmTAB_GRAPH_NODE)
	    {
	      graph->sub_tail->any.next = graph->sub_head;
	      graph->sub_head->any.prev = graph->sub_tail;
	      graph->sub_head = init_node;
	      graph->sub_tail = init_node->any.prev;
	      graph->sub_tail->any.next = NULL;
	      init_node->any.prev = NULL;
	    }
	  else
	    {
	      graph->sub_head = init_node;
	      graph->sub_tail = init_node->any.prev;
	    }
	}

      return TRUE;
    }

  return FALSE;
}

static void
SetInitialWidgets(XmTravGraph trav_list)
{
  Widget init_focus;
  XmTraversalNode ptr = trav_list->head;
  XmTraversalNode init_node;
  unsigned cnt = 0;

  while (cnt < trav_list->num_entries)
    {
      if (((ptr->any.type == XmTAB_GRAPH_NODE) ||
	   (ptr->any.type == XmCONTROL_GRAPH_NODE)) &&
	  ptr->graph.sub_head)
	{
	  if (ptr->any.widget &&
	      XmIsManager(ptr->any.widget) &&
	      (init_focus =
	       ((XmManagerWidget) ptr->any.widget)->manager.initial_focus) &&
	      (init_node = GetNodeFromGraph((XmGraphNode) ptr, init_focus)))
	    {
	      SetInitialNode((XmGraphNode) ptr, init_node);
	    }
	  else
	    {
	      if (ptr->any.type == XmTAB_GRAPH_NODE)
		{
		  /* A tab graph node is always followed immediately
		   * by its corresponding control graph.
		   *
		   * Make sure that the control graph is the first
		   * tab node of every tab graph (unless specified
		   * otherwise).
		   */
		  SetInitialNode((XmGraphNode) ptr, (ptr + 1));
		}
	    }
	}

      ++ptr;
      ++cnt;
    }
}

static void
GetRectRelativeToShell(Widget wid,
		       XRectangle *rect)
{
  /* This routine returns the rectangle of the widget relative to its
   * nearest shell ancestor.
   */
  Position x = 0;
  Position y = 0;

  rect->width = XtWidth(wid);
  rect->height = XtHeight(wid);

  do {
    x += XtX(wid) + XtBorderWidth(wid);
    y += XtY(wid) + XtBorderWidth(wid);
  } while ((wid = XtParent(wid)) && !XtIsShell(wid));

  rect->x = x;
  rect->y = y;
}

void
_XmTabListAdd(XmTravGraph graph,
	      Widget wid)
{
  if (SearchTabList(graph, wid) < 0)
    {
      if (!(graph->tab_list_alloc))
	{
	  Widget shell = _XmFindTopMostShell(wid);

	  graph->tab_list_alloc = XmTAB_LIST_ALLOC_INCREMENT;
	  graph->excl_tab_list = (Widget *)
	    XtMalloc(graph->tab_list_alloc * sizeof(Widget));
	  graph->excl_tab_list[graph->num_tab_list++] = shell;
	}

      if (graph->num_tab_list >= graph->tab_list_alloc)
	{
	  graph->tab_list_alloc += XmTAB_LIST_ALLOC_INCREMENT;
	  graph->excl_tab_list = (Widget *)
	    XtRealloc((char *) graph->excl_tab_list,
		      graph->tab_list_alloc * sizeof(Widget));
	}

      graph->excl_tab_list[graph->num_tab_list++] = wid;
    }
}

void
_XmTabListDelete(XmTravGraph graph,
		 Widget wid)
{
  DeleteFromTabList(graph, SearchTabList(graph, wid));

  if ((graph->num_tab_list + XmTAB_LIST_ALLOC_INCREMENT) <
      graph->tab_list_alloc)
    {
      graph->tab_list_alloc -= XmTAB_LIST_ALLOC_INCREMENT;
      graph->excl_tab_list = (Widget *)
	XtRealloc((char *) graph->excl_tab_list,
		  graph->tab_list_alloc * sizeof(Widget));
    }
}

static int
SearchTabList(XmTravGraph graph,
	      Widget wid)
{
  int i;

  for (i = 0; i < graph->num_tab_list; i++)
    {
      if (graph->excl_tab_list[i] == wid)
	return i;
    }

  return -1;
}

static void
DeleteFromTabList(XmTravGraph graph,
		  int indx)
{
  if (indx >= 0)
    {
      /* CR 7455: Don't index off the end of the array. */
      while ((indx + 1) < graph->num_tab_list)
	{
	  graph->excl_tab_list[indx] = graph->excl_tab_list[indx+1];
	  ++indx;
	}
      --(graph->num_tab_list);
    }
}

static Boolean
LastControl(Widget w,
	    XmTraversalDirection dir,
	    XmTravGraph graph)
{
  XmTraversalNode cur_node, new_ctl;

  cur_node = GetNodeOfWidget(graph, w);
  if (!cur_node)
    return False;

  if (cur_node->any.type == XmCONTROL_GRAPH_NODE)
    {
      /* This is only true on composites. */
      cur_node = cur_node->graph.sub_head;
      if (!cur_node)
	return True;
    }
  else
    {
      if (cur_node->any.type != XmCONTROL_NODE)
	return True;
    }

  new_ctl = cur_node;
  do {
    switch(dir)
      {
      case XmTRAVERSE_GLOBALLY_FORWARD:
	/* The action will cause wrapping when the new_ctrl matches the
         * last item on the tab_parent's list.
	 */
	if (new_ctl == new_ctl->any.tab_parent.link->sub_head->any.prev)
	  return True;
	else
	  new_ctl = new_ctl->any.next;
	break;

      case XmTRAVERSE_GLOBALLY_BACKWARD:
	/* Moving backward will cause wrapping when the new control is the
	 * first item in the tab group.
	 */
	if (new_ctl == new_ctl->any.tab_parent.link->sub_head)
	  return True;
	else
	  new_ctl = new_ctl->any.prev;
	break;

      default:
	return False;
      }
  } while (new_ctl &&
	   !NodeIsTraversable(new_ctl) &&
	   (new_ctl != cur_node));

  return False;
}

static XmTraversalDirection
LocalDirection(Widget w,
	       XmTraversalDirection direction)
{
  XmDirection layout;
  Boolean forward;

  /* Local direction is only interesting for global traversal. */
  switch (direction)
    {
    case XmTRAVERSE_GLOBALLY_FORWARD:
      forward = TRUE;
      break;

    case XmTRAVERSE_GLOBALLY_BACKWARD:
      forward = FALSE;
      break;

    default:
      return direction;
    }

  /* Get the current layout direction. */
  if (XmIsManager(w))
    layout = LayoutM(w);
  else if (XmIsPrimitive(w))
    layout = LayoutP(w);
  else if (XmIsGadget(w))
    layout = LayoutG(w);
  else
    layout = _XmGetLayoutDirection(w);

  if (XmDirectionMatchPartial(layout,
			      XmPRECEDENCE_HORIZ_MASK,
			      XmPRECEDENCE_MASK))
    {
      /* Horizontal precendence. */
      if (XmDirectionMatchPartial(layout, XmLEFT_TO_RIGHT, XmHORIZONTAL_MASK))
	return (forward ? XmTRAVERSE_RIGHT : XmTRAVERSE_LEFT);
      else
	return (forward ? XmTRAVERSE_LEFT : XmTRAVERSE_RIGHT);
    }
  else
    {
      /* Vertical precedence. */
      if (XmDirectionMatchPartial(layout, XmTOP_TO_BOTTOM, XmVERTICAL_MASK))
	return (forward ? XmTRAVERSE_DOWN : XmTRAVERSE_UP);
      else
	return (forward ? XmTRAVERSE_UP : XmTRAVERSE_DOWN);
    }
}

#ifdef DEBUG_TRAVERSAL

#define WNAME(node) (XtName(node->any.widget))
#define CNAME(node) (node->any.widget->core.widget_class->core_class.class_name)

static void
PrintControl(XmTraversalNode node)
{
  printf("%X: control %s \"%s\", (%d,%d), parent: %X, next: %X, prev: %X, up: %X, down: %X\n",
	 node, CNAME(node), WNAME(node), node->any.rect.x, node->any.rect.y,
	 node->any.tab_parent.link, node->any.next, node->any.prev,
	 node->control.up, node->control.down);
}

static void
PrintTab(XmTraversalNode node)
{
  printf("%X: tab %s \"%s\", (%d,%d), parent: %X, next: %X, prev: %X\n",
	 node, CNAME(node), WNAME(node), node->any.rect.x, node->any.rect.y,
	 node->any.tab_parent.link, node->any.next, node->any.prev);
}

static void
PrintGraph(XmTraversalNode node)
{
  char *gr_str;

  if (node->any.type == XmCONTROL_GRAPH_NODE)
    gr_str = "ctl_graph";
  else
    gr_str = "tab_graph";

  printf("%X: %s %s \"%s\", (%d,%d), parent: %X, sub_head: %X, sub_tail: %X, next: %X, prev: %X\n",
	 node,
	 gr_str,
	 CNAME(node), WNAME(node), node->any.rect.x, node->any.rect.y,
	 node->any.tab_parent.link, node->graph.sub_head, node->graph.sub_tail,
	 node->any.next, node->any.prev);
}

static void
PrintNodeList(XmTravGraph list)
{
  XmTraversalNode ptr = list->head;
  unsigned idx;

  for (idx = 0; idx < list->num_entries; idx++)
    {
      switch (ptr->any.type)
	{
	case XmTAB_GRAPH_NODE:
	case XmCONTROL_GRAPH_NODE:
	  PrintGraph(ptr);
	  break;
	case XmTAB_NODE:
	  PrintTab(ptr);
	  break;
	case XmCONTROL_NODE:
	  PrintControl(ptr);
	  break;
	}
      ++ptr;
    }
}
#endif /* DEBUG_TRAVERSAL */

