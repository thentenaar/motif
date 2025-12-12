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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Traversal.c /main/20 1999/08/17 11:52:26 vipin $"
#endif
#endif

#include <Xm/GadgetP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuShellP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScrolledWP.h>
#include <Xm/TraitP.h>
#include <Xm/TravConT.h>
#include <Xm/VendorSEP.h>
#include <Xm/VirtKeysP.h>
#include <Xm/DisplayP.h>
#include "BaseClassI.h"
#include "CallbackI.h"
#include "RepTypeI.h"
#include "TravActI.h"
#include "TraversalI.h"


/********    Static Function Declarations    ********/

static Widget FindFirstManaged(
                        Widget wid) ;
static Boolean CallTraverseObsured(
                        Widget new_focus,
                        XmTraversalDirection dir) ;
static Boolean IsTraversable(
                        Widget wid,
                        Boolean require_in_view) ;
static Widget FindFirstFocus(
                        Widget wid) ;
static Boolean CallFocusMoved(Widget		   old,
			      Widget		   new_wid,
			      XEvent		   *event,
			      XmTraversalDirection direction);
static Widget RedirectTraversal(Widget		     old_focus,
				Widget		     new_focus,
				unsigned int	     focus_policy,
				XmTraversalDirection direction,
				unsigned int	     pass);

/********    End Static Function Declarations    ********/


XmFocusData
_XmCreateFocusData( void )
{
  return (XmFocusData) XtCalloc(1, sizeof( XmFocusDataRec)) ;
}

void
_XmDestroyFocusData(
        XmFocusData focusData )
{
  _XmFreeTravGraph( &(focusData->trav_graph)) ;
  XtFree((char *) focusData->trav_graph.excl_tab_list) ;
  XtFree((char *) focusData) ;
}

void
_XmSetActiveTabGroup(
        XmFocusData focusData,
        Widget tabGroup )
{
    focusData->active_tab_group = tabGroup;
}

Widget
_XmGetActiveItem(
        Widget w )
{
  return XmGetFocusWidget( w) ;
}

/*ARGSUSED*/
void
_XmNavigInitialize(
        Widget request,		/* unused */
        Widget new_wid,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmFocusData focusData ;

  if(    (focusData = _XmGetFocusData( new_wid)) != NULL    )
    {
      XmNavigationType navType = _XmGetNavigationType( new_wid) ;

      if(    navType == XmEXCLUSIVE_TAB_GROUP    )
	{
	  ++(focusData->trav_graph.exclusive) ;
	  _XmTabListAdd( &(focusData->trav_graph), new_wid) ;
	}
      else
	{
	  if(    navType == XmSTICKY_TAB_GROUP    )
	    {
	      _XmTabListAdd( &(focusData->trav_graph), new_wid) ;
	    }
	}
      if(    focusData->trav_graph.num_entries
	 &&  _XmGetNavigability( new_wid)    )
	{
	  /* If the graph exists, add the new navigable widget.
	   */
	  _XmTravGraphAdd( &(focusData->trav_graph), new_wid) ;
	}
    }
  /* If the traversal graph doesn't exist, do nothing, since the
   * new widget will be picked-up when the graph is needed and created.
   */
  return ;
}

/*ARGSUSED*/
Boolean
_XmNavigSetValues(
        Widget current,
        Widget request,		/* unused */
        Widget new_wid,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  /* This routine is called from the SetValues method of Manager,
   * Primitive, and Gadget to keep the traversal data structures
   * up-to-date in regards to changes in the traversability of widgets.
   *
   * There are three purposes for this routine:
   *
   *   1:  Update the traversal graph in response to changes in
   *       a widget's resources such that the widget is newly
   *       eligible to receive the traversal focus.
   *
   *   2:  Update the focus data according to changes in
   *       the Motif 1.0 "exclusive tab group" behavior.
   *
   *   3:  If the new widget of the SetValues call is the focus
   *       widget and it becomes ineligible to have the focus,
   *       then find an alternative to receive the focus (or
   *       reset the focus for the hierarchy to the bootstrap
   *       condition).
   */

  XmFocusData focusData ;

  if(    (focusData = _XmGetFocusData( new_wid)) != NULL    )
    {
      XmTravGraph graph = &(focusData->trav_graph) ;
      XmNavigationType newNavType = _XmGetNavigationType( new_wid) ;
      XmNavigationType curNavType = _XmGetNavigationType( current) ;
      Boolean ChangeInExclusive = FALSE ;

      if(    curNavType != newNavType    )
	{
	  if(    (curNavType == XmEXCLUSIVE_TAB_GROUP)
	     ||  (newNavType == XmEXCLUSIVE_TAB_GROUP)    )
	    {
	      /* This widget was "exclusive", now it is not (or vice-versa).
	       * Update the value of the focus data "exclusive" field.
	       */
	      ChangeInExclusive = TRUE ;

	      if(    newNavType == XmEXCLUSIVE_TAB_GROUP    )
		{
		  ++(graph->exclusive) ;
		}
	      else
		{
		  --(graph->exclusive) ;
		}
	    }
	  if(    (newNavType == XmEXCLUSIVE_TAB_GROUP)
	     ||  (newNavType == XmSTICKY_TAB_GROUP)    )
	    {
	      if(    (curNavType != XmEXCLUSIVE_TAB_GROUP)
		 &&  (curNavType != XmSTICKY_TAB_GROUP)    )
		{
		  _XmTabListAdd( graph, new_wid) ;
		}
	    }
	  else
	    {
	      if(    (curNavType == XmEXCLUSIVE_TAB_GROUP)
		 ||  (curNavType == XmSTICKY_TAB_GROUP)    )
		{
		  _XmTabListDelete( graph, new_wid) ;
		}
	    }
	}
      if(    XtIsRealized( new_wid)
	 &&  (focusData->focus_policy == XmEXPLICIT)    )
	{
	  if(    graph->num_entries    )
	    {
	      if(    ChangeInExclusive    )
		{
		  /* Since widget has changed to/from exlusive tab group
		   * behavior, need to re-make the traversal graph (as needed).
		   */
		  _XmFreeTravGraph( graph) ;
		}
	      else
		{
		  XmNavigability cur_nav = _XmGetNavigability( current) ;
		  XmNavigability new_nav = _XmGetNavigability( new_wid) ;

		  if(    !cur_nav  &&  new_nav    )
		    {
		      /* Newly navigable widget; add it to the
		       * traversal graph.
		       */
		      _XmTravGraphAdd( graph, new_wid) ;
		    }
		  else
		    {
		      if(    cur_nav != new_nav    )
			{
			  /* Navigability changed; need to re-create the
			   * graph the next time it is needed.
			   */
			  _XmFreeTravGraph( graph) ;
			}
		    }
		}
	    }
	  if(    !(focusData->focus_item)    )
	    {
	      Widget shell ;

	      /* CR 5417: Remove (focusData->focalPoint != XmMySelf) test. */
	      if(    XmIsTraversable( new_wid)
		 &&  (shell = _XmFindTopMostShell( new_wid))
		 &&  XmeFocusIsInShell( shell))
		{
		  /* Hierarchy currently has no focus, and this widget is
		   * now traversable, so bootstrap the focus for the hierarchy.
		   */
		  _XmMgrTraversal( shell, XmTRAVERSE_CURRENT) ;
		}
	    }
	  else
	    {
	      if(    (focusData->focus_item == new_wid)
		 &&  !IsTraversable( new_wid, TRUE)    )
		{
		  /* The new_wid now has the focus and is no longer
		   * traversable, so traverse away from it to the
		   * next traversable item.
		   */
		  Widget new_focus = _XmTraverseAway( graph, new_wid,
                                    (focusData->active_tab_group != new_wid)) ;
		  if(    !new_focus    )
		    {
		      /* Could not find another widget eligible to take
		       * the focus, so use any widget to re-initialize/clear
		       * the focus in the widget hierarchy.
		       */
		      new_focus = new_wid ;
		    }
		  _XmMgrTraversal( new_focus, XmTRAVERSE_CURRENT) ;

		  if(    !XtIsSensitive( new_wid)    )
		    {
		      /* Since widget has become insensitive, it did not
		       * receive the focus-out event.  Call the focus
		       * change method directly.
		       */
		      _XmWidgetFocusChange( new_wid, XmFOCUS_OUT) ;
		    }
		  return TRUE ;
		}
	    }
	}
    }
  return FALSE ;
}

void
XmeNavigChangeManaged(
        Widget wid )
{
  /* This routine must be called from the ChangeManaged method of
   * all composite widgets that may have traversable children.
   * This routine checks to see if the focus widget is traversable;
   * if it is not, then an alternative traversable widget is found
   * or the focus for the hierarchy is reset to the bootstrap condition.
   *
   * This routine also detects the condition for which there is no
   * focus widget in the hierarchy and a newly managed widget is
   * now eligible to have the focus; the focus is then initialized.
   */
  XmFocusData focus_data ;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  if(    XtIsRealized( wid)
     &&  (focus_data = _XmGetFocusData( wid))
     &&  (focus_data->focus_policy == XmEXPLICIT)    )
    {
      if(    focus_data->focus_item == NULL    )
	{
	  Widget firstManaged ;

	  if(    XtIsShell( wid)    )
	    {
	      if(    focus_data->first_focus == NULL    )
		{
		  focus_data->first_focus = FindFirstFocus( wid) ;
		}
	      if(    (firstManaged = FindFirstManaged( wid)) != NULL    )
		{
		  /* Set bootstrap trigger for hierarchy that
		   * has no current focus.
		   */
		  XtSetKeyboardFocus( wid, firstManaged) ;
		}
	    }
	}
      else
	{
	  /* If the focus widget is being destroyed, do nothing for now.
	   * We need to wait until _XmNavigDestroy is called to initiate
	   * the focus change; if we don't defer selection of the focus
	   * widget, the Intrinsics-generated focus-out event for the
	   * focus widget will go to the newly-selected focus widget
	   * (instead of the widget being destroyed, as intended).
	   */
	  if(    !(focus_data->focus_item->core.being_destroyed)
	     &&  !IsTraversable( focus_data->focus_item, TRUE)    )
	    {
	      Widget new_focus = _XmTraverseAway( &(focus_data->trav_graph),
                                     focus_data->focus_item,
		                          (focus_data->active_tab_group
		                                  != focus_data->focus_item)) ;
	      if(    !new_focus    )
		{
		  new_focus = focus_data->focus_item ;
		}
        if(new_focus)
	      _XmMgrTraversal( new_focus, XmTRAVERSE_CURRENT) ;
	    }
	}
    }
  _XmAppUnlock(app);
  return ;
}

static Widget
FindFirstManaged(
	Widget wid)
{
  if(    XtIsShell( wid)    )
    {
      unsigned i = 0 ;

      while(    i < ((CompositeWidget) wid)->composite.num_children    )
	{
	  if(    XtIsManaged( ((CompositeWidget) wid)
				                  ->composite.children[i])    )
	    {
	      return ((CompositeWidget) wid)->composite.children[i] ;
	    }
	  ++i ;
	}
    }
  return NULL ;
}

void
_XmNavigResize(
	Widget wid)
{
  /* This routine must be called by all composites with (potentially)
   * traversable children.  This is generally handled for all managers
   * in the resize wrapper routines.
   *
   * This routine makes sure that the focus widget is always in view,
   * either by invoking the XmNtraverseObscurredCallback mechansism
   * of Scrolled Window or by finding an alternative focus widget.
   */
  XmFocusData focus_data ;

  if(    XtIsRealized( wid)  &&  !XtIsShell( wid)
     &&  (focus_data = _XmGetFocusData( wid))    )
    {
      /* If the focus item is being destroyed, do nothing, since this
       * will be handled more appropriately by _XmNavigDestroy().
       */
      if(    (focus_data->focus_policy == XmEXPLICIT)
	 &&  (    !(focus_data->focus_item)
	      ||  !((focus_data->focus_item)->core.being_destroyed))    )
	{
	  if(    !(focus_data->focus_item)    )
	    {
	      /* Hierarchy has no focus widget; re-initialize/clear the
	       * focus, but only if the parent is a managed shell (to
	       * avoid premature initialization during XtRealizeWidget).
	       */
	      Widget parent = XtParent( wid) ;
	      Widget firstManaged ;

	      if(    parent && XtIsShell( parent)
		 &&  (firstManaged = FindFirstManaged( parent))    )
		{
		  /* Set bootstrap trigger for hierarchy that
		   * has no current focus.
		   */
		  XtSetKeyboardFocus( wid, firstManaged) ;
		}
	    }
	  else
	    {
	      if(    !IsTraversable( focus_data->focus_item, TRUE)    )
		{
		  /* Widget is not traversable, either because it is not
		   * viewable or some other reason.  Test again, this
		   * time allowing for obscured traversal.
		   *
		   * If it is not traversable regardless of the
		   * XmNtraverseObscuredCallback, or traversal to the
		   * obscured widget fails for some other reason, traverse
		   * away from the non-traversable widget.
		   */
		  if(    !IsTraversable( focus_data->focus_item, FALSE)
		     ||  !_XmMgrTraversal( focus_data->focus_item,
			                          XmTRAVERSE_CURRENT)    )
		    {
		      Widget new_focus = _XmTraverseAway(
			     &(focus_data->trav_graph), focus_data->focus_item,
                                (focus_data->active_tab_group
				                  != focus_data->focus_item)) ;
		      if(    !new_focus    )
			{
			  new_focus = focus_data->focus_item ;
			}
            if(new_focus)
		      _XmMgrTraversal( new_focus, XmTRAVERSE_CURRENT) ;
		    }
		}
	    }
	}
    }
}

void
_XmValidateFocus(
        Widget wid )
{
  XmFocusData focus_data = _XmGetFocusData( wid) ;

  if(    focus_data
     &&  (focus_data->focus_policy == XmEXPLICIT)
     &&  (focus_data->focus_item != NULL)
     &&  !IsTraversable( focus_data->focus_item, TRUE)    )
    {
      Widget new_focus = _XmTraverseAway( &(focus_data->trav_graph),
		                 focus_data->focus_item,
                    (focus_data->active_tab_group != focus_data->focus_item)) ;
      if(    !new_focus    )
	{
	  new_focus = wid ;
	}
      _XmMgrTraversal( new_focus, XmTRAVERSE_CURRENT) ;
    }
}

void
_XmNavigDestroy(
        Widget wid )
{
  /* This routine is used to keep the traversal data up-to-date with
   * regards to widgets which are being destroyed.  It must be called
   * by all composites that might have traversable children.  The
   * DeleteChild method for Manager calls this routine, so its
   * subclasses can explicitly chain to its superclasses DeleteChild
   * method or call this routine directly.
   *
   * In addition to finding a new focus widget if it is being
   * destroyed, this routine must make sure that there are no
   * stale pointers to the widget being destroyed in any of its
   * data structures.
   */
  XmFocusData focusData = _XmGetFocusData( wid) ;

  if(    focusData    )
    {
      XmTravGraph trav_list = &(focusData->trav_graph) ;
      XmNavigationType navType = _XmGetNavigationType( wid) ;

      if(    wid == focusData->first_focus    )
	{
	  focusData->first_focus = NULL ;
	}
      if(    navType == XmEXCLUSIVE_TAB_GROUP    )
	{
	  --(trav_list->exclusive) ;
	  _XmTabListDelete( trav_list, wid) ;
	}
      else
	{
	  if(    navType == XmSTICKY_TAB_GROUP    )
	    {
	      _XmTabListDelete( trav_list, wid) ;
	    }
	}
      if(    focusData->focus_item == wid    )
	{
	  /* The focus widget for this hierarhcy is being destroyed.
	   * Traverse away if in explicit mode, or just clear the
	   * focus item field.
	   */
	  Widget new_focus ;

	  if(    (focusData->focus_policy != XmEXPLICIT)
	     ||  (    !(new_focus = _XmTraverseAway( trav_list,
				       focusData->focus_item,
                                         (focusData->active_tab_group != wid)))
		  &&  !(new_focus = _XmFindTopMostShell( wid)))
	     ||  !_XmMgrTraversal( new_focus, XmTRAVERSE_CURRENT)    )
	    {
	      focusData->focus_item = NULL ;
	    }
	}
      if(    focusData->trav_graph.num_entries    )
	{
	  _XmTravGraphRemove( trav_list, wid) ;
	}
      if(    focusData->active_tab_group == wid    )
	{
	  focusData->active_tab_group = NULL ;
	}
      if(    focusData->old_focus_item == wid    )
	{
	  focusData->old_focus_item = NULL ;
	}
      if(    focusData->pointer_item == wid    )
	{
	  focusData->pointer_item = NULL ;
	}
    }
  return ;
}

static Boolean
CallFocusMoved(Widget		    old,
	       Widget		    new_wid,
	       XEvent		    *event,
	       XmTraversalDirection direction)
{
  Widget w ;
  Widget topShell ;
  XtCallbackList callbacks ;
  Boolean contin = TRUE ;

  if (old)
    w = old;
  else /* if (new_wid) -- if there's no w assignment we're in big trouble! */
    w = new_wid;

  topShell 	= (Widget) _XmFindTopMostShell(w);

  /*
   * make sure it's a shell that has a vendorExt object
   */
  if (XmIsVendorShell(topShell))
    {
      XmWidgetExtData		extData;
      XmVendorShellExtObject	vendorExt;

      extData	= _XmGetWidgetExtData(topShell, XmSHELL_EXTENSION);
      if(extData==NULL) return (contin);

      if ((vendorExt = (XmVendorShellExtObject) extData->widget) != NULL)
	{
	  if ((callbacks = vendorExt->vendor.focus_moved_callback) != NULL)
	    {
	      XmFocusMovedCallbackStruct	callData;

	      callData.reason		= XmCR_FOCUS_MOVED;
	      callData.event		= event;
	      callData.cont		= True;
	      callData.old_focus	= old;
	      callData.new_focus	= new_wid;
	      callData.focus_policy	= vendorExt->vendor.focus_policy;
	      callData.direction	= direction;

	      _XmCallCallbackList((Widget) vendorExt, callbacks,
				  (XtPointer) &callData);
	      contin = callData.cont ;
	    }
	}
    }
  return( contin) ;
}

Boolean
_XmCallFocusMoved(
        Widget old,
        Widget new_wid,
        XEvent *event )
{
  return CallFocusMoved(old, new_wid, event, XmTRAVERSE_CURRENT);
}

Boolean
_XmMgrTraversal(
        Widget wid,
        XmTraversalDirection direction)
{
  /* This routine is the workhorse for all traversal activities. */
  Widget top_shell ;
  Widget old_focus ;
  Widget new_focus ;
  Widget new_active_tab ;
  XmFocusData focus_data;
  XmTravGraph trav_list ;
  Boolean rtnVal = FALSE ;
  XmTraversalDirection local_dir;
  XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(wid));

#define traversal_in_progress \
   ((XmDisplayInfo *)(dd->display.displayInfo))->traversal_in_progress

  if(    traversal_in_progress
     ||  !(top_shell = _XmFindTopMostShell( wid))
     ||  top_shell->core.being_destroyed
     ||  !(focus_data = _XmGetFocusData( wid))
     ||  (focus_data->focus_policy != XmEXPLICIT)    )
    {
      return FALSE ;
    }
  traversal_in_progress = TRUE ;

  /* Recursive traversal calls can sometimes be generated during
   * the handling of focus events and associated callbacks.
   * In this version of Motif, recursive calls always fail.
   *
   * Future enhancements could include the addition of a queue
   * for recursive calls; these calls would then be serviced on
   * a FIFO basis following the completion of the initial traversal
   * processing.  Sequential FIFO processing is essential for
   * providing a consistent and predicable environment for
   * focus change callbacks and event processing.
   */
  trav_list = &(focus_data->trav_graph) ;
  old_focus = focus_data->focus_item ;

  if(    (old_focus == NULL)
     &&  (wid == top_shell)
     &&  focus_data->first_focus
     &&  IsTraversable( focus_data->first_focus, TRUE)    )
    {
      new_focus = focus_data->first_focus ;

      if (direction == XmTRAVERSE_GLOBALLY_FORWARD)
	local_dir = XmTRAVERSE_NEXT_TAB_GROUP;
      else if (direction == XmTRAVERSE_GLOBALLY_BACKWARD)
	local_dir = XmTRAVERSE_PREV_TAB_GROUP;
      else
	local_dir = direction;
    }
  else
    {
      new_focus = _XmTraverse(trav_list, direction, &local_dir, wid) ;
    }

  new_focus = RedirectTraversal(old_focus, new_focus,
				focus_data->focus_policy, local_dir, 0);

  if(    new_focus
     &&  (new_focus == old_focus)
     &&  focus_data->old_focus_item    )
    {
      /* When traversal does not cause the focus to change
       * to a different widget, focus-change events should
       * not be generated.  The old_focus_item will be NULL
       * when the focus is moving into this shell hierarchy
       * from a different shell; in this case, focus-in
       * events should be generated below.
       */
      rtnVal = TRUE ;
    }
  else if(    new_focus
	  &&  (new_active_tab = XmGetTabGroup( new_focus))
	  &&  CallFocusMoved(old_focus, new_focus, NULL, local_dir)
	  &&  CallTraverseObsured(new_focus, local_dir))
    {
      /* Set the keyboard focus in two steps; first to None, then
       * to the new focus widget.  This will cause appropriate
       * focus-in and focus-out events to be generated, even if
       * the focus change is between two gadgets.
       *
       * Note that XtSetKeyboardFocus() generates focus change
       * events "in-line", so focus data and manager active_child
       * fields are not updated until after the focus-out events have
       * been generated and dispatched to the current focus item.
       *
       * The FocusResetFlag is used to tell event actions procs to
       * ignore any focus-in event that might be generated by the
       * window manager (which won't like the fact that there the
       * focus is now going to point to nobody).
       */
      _XmSetFocusFlag( top_shell, XmFOCUS_RESET, TRUE) ;
      XtSetKeyboardFocus( top_shell, None) ;
      _XmSetFocusFlag( top_shell, XmFOCUS_RESET, FALSE) ;

      _XmClearFocusPath( old_focus) ;

      focus_data->active_tab_group = new_active_tab ;

      if(    (new_active_tab != new_focus)
	 &&  XmIsManager( new_active_tab)    )
	{
	  XmManagerWidget manager = (XmManagerWidget) new_active_tab;
	  manager->manager.active_child = new_focus;
	}
      if(    (new_active_tab != XtParent( new_focus))  /* Set above. */
	 &&  XmIsManager( XtParent( new_focus))    )
	{
	  XmManagerWidget manager = (XmManagerWidget) XtParent(new_focus);
	  manager->manager.active_child = new_focus ;
	}
      focus_data->focus_item = new_focus ;
      focus_data->old_focus_item = old_focus ? old_focus : new_focus ;

      /* Setting the focus data and manager active_child fields enables
       * focus-in events to be propagated to the new focus widget.
       */
      XtSetKeyboardFocus( top_shell, new_focus) ;

      rtnVal = TRUE ;
    }
  else
    {
      /* Have failed to traverse to a new widget focus widget.
       * If the current focus widget is no longer traversable,
       * then reset focus data to its bootstrap state.
       */
      if(    !old_focus
	 ||  !IsTraversable( old_focus, TRUE)    )
	{
	  Widget firstManaged = FindFirstManaged( top_shell) ;

	  _XmSetFocusFlag( top_shell, XmFOCUS_RESET, TRUE) ;
	  XtSetKeyboardFocus( top_shell, firstManaged) ;
	  _XmSetFocusFlag( top_shell, XmFOCUS_RESET, FALSE) ;

	  _XmClearFocusPath( old_focus) ;
	  _XmFreeTravGraph( trav_list) ;
	}
    }

  if(    trav_list->num_entries
     &&  (focus_data->focalPoint == XmUnrelated)
     &&  (    XmIsVendorShell( top_shell)
	 ||  !XmeFocusIsInShell( top_shell))    )
    {
      /* Free the graversal graph whenever the focus is out of this
       * shell hierarchy, so memory use is limited to one traversal
       * graph per display.  Since VendorShell has a handler which
       * tracks the input focus, all we need to do is look at the
       * focusData field.  For MenuShell and others, we need to go
       * through the X server to find out where the focus is.
       *
       * Note the logic of the above conditional; VendorShell is the
       * only shell class that maintains the focalPoint field of the
       * focus data.  So, if its a VendorShell and focalPoint says
       * "unrelated", we have the answer; any other shell and we need
       * to call the generic focus test routine.
       */
      _XmFreeTravGraph( trav_list) ;
    }
  traversal_in_progress = FALSE ;
  return rtnVal ;
}

static Boolean
CallTraverseObsured(
        Widget new_focus,
        XmTraversalDirection dir)
{
  Widget prev;
  Widget ancestor = new_focus;
  XRectangle focus_rect;	/* Area we're trying to make visible. */
  XRectangle clip_rect;		/* Area a given ancestor obscures. */
  XRectangle view_rect;		/* Temporary intersection of the two. */
  XmTraverseObscuredCallbackStruct call_data;

  call_data.reason = XmCR_OBSCURED_TRAVERSAL;
  call_data.event = NULL;
  call_data.traversal_destination = new_focus;
  call_data.direction = dir;

  _XmSetRect(&focus_rect, new_focus);

  /* Look for ancestors that clip this window. */
  for (prev = ancestor;
       ((ancestor = XtParent(ancestor)) != NULL) && !XtIsShell(ancestor);
       prev = ancestor)
    {
      /* CR 9705: Special case overlapping work windows. */
      if (!_XmIsScrollableClipWidget(ancestor, False, &clip_rect))
	_XmSetRect(&clip_rect, ancestor);

      if (!_XmIntersectionOf(&focus_rect, &clip_rect, &view_rect) ||
	  (view_rect.width != focus_rect.width) ||
	  (view_rect.height != focus_rect.height))
	{
	  /* This ancestor clips somebody. */
	  Widget sw = _XmIsScrollableClipWidget(prev, True, &focus_rect);
	  if (sw != NULL)
	    {
	      XtCallbackList callbacks = ((XmScrolledWindowWidget) sw)
		->swindow.traverseObscuredCallback;
	      XtCallCallbackList(sw, callbacks, (XtPointer) &call_data);

	      ancestor = sw;
	    }
	  else
	    {
	      _XmIntersectRect(&focus_rect, ancestor, &focus_rect);
	    }
	}
    }

  return IsTraversable( new_focus, TRUE);
}

void
_XmClearFocusPath(
        Widget wid )
{
  /* This routine should be called whenever the focus of a shell
   * hierarchy needs to be reset to the bootstrap condition.
   *
   * This routine clears the active_child field of all manager
   * widget ancestors of the widget argument, and clears other
   * focus widget fields of the focus data record.  The clearing
   * of the old_focus_item field indicates to the traversal code
   * that the focus is not in this shell hierarchy.
   */
  XmFocusData focus_data ;

  while(    wid  &&  !XtIsShell( wid)    )
    {
      if(    XmIsManager( wid)    )
	{
	  ((XmManagerWidget) wid)->manager.active_child = NULL ;
	}
      wid = XtParent( wid) ;
    }
  if(    (focus_data = _XmGetFocusData( wid)) != NULL    )
    {
      focus_data->focus_item = NULL ;
      focus_data->old_focus_item = NULL ;
      focus_data->active_tab_group = NULL ;
    }
}

Boolean
_XmFocusIsHere(
        Widget w )
{
    XmFocusData focus_data;
    Widget	item;

    if ((focus_data = _XmGetFocusData( w)) &&
	(item = focus_data->focus_item))
      {
	  for (; !XtIsShell(item); item = XtParent(item))
	    if (item == w)
	      return True;
      }
    return(False);
}

unsigned char
_XmGetFocusPolicy(
        Widget w )
{
  Widget topmost_shell ;

  /* Find the topmost shell widget
   */
  topmost_shell = _XmFindTopMostShell( w) ;

  if(    XtIsVendorShell( topmost_shell)    )
    {
      XmWidgetExtData xwed = _XmGetWidgetExtData(topmost_shell, XmSHELL_EXTENSION);

      if(xwed == NULL) return(XmPOINTER);

      return (((XmVendorShellExtObject)
	       (xwed)->widget)
	      ->vendor.focus_policy) ;
    }
  else
    {
      if(    XmIsMenuShell( topmost_shell)    )
	{
	  return( ((XmMenuShellWidget) topmost_shell)->menu_shell.focus_policy);
	}
    }
  return( XmPOINTER) ;
}

Widget
_XmFindTopMostShell(
        Widget w )
{
  while(    w && !XtIsShell( w)    )
    {
      w = XtParent( w) ;
    }
  return( w) ;
}

/*ARGSUSED*/
void
_XmFocusModelChanged(
        Widget wid,
        XtPointer client_data,	/* unused */
        XtPointer call_data )
{
  /* Invoked by the VendorShell widget, when the focus_policy changes.
   * Registered as a callback by both the Manager and Primitive classes,
   * when the parent is a VendorShell widget.
   */
  unsigned char new_focus_policy = (unsigned char)(unsigned long) call_data ;
  Widget shell = _XmFindTopMostShell( wid) ;
  XmFocusData focus_data = _XmGetFocusData( shell) ;

  if(    focus_data    )
    {
      if(    new_focus_policy == XmEXPLICIT    )
	{
	  Widget new_item = focus_data->pointer_item ;

	  if(    new_item != NULL    )
	    {
	      if(    XmIsManager( new_item)
		 &&  (((XmManagerWidget) new_item)
		                     ->manager.highlighted_widget != NULL)    )
		{
		  new_item = ((XmManagerWidget) new_item)
		                                 ->manager.highlighted_widget ;
		}
	      _XmWidgetFocusChange( new_item, XmLEAVE) ;
	    }
	  if(    (new_item == NULL)
	     ||  !_XmMgrTraversal( new_item, XmTRAVERSE_CURRENT)    )
	    {
	      _XmMgrTraversal( shell, XmTRAVERSE_CURRENT) ;
	    }
	}
      else /* new_focus_policy == XmPOINTER */
	{
	  if(    focus_data->focus_item    )
	    {
	      Widget firstManaged = FindFirstManaged( shell) ;

	      _XmWidgetFocusChange( focus_data->focus_item, XmFOCUS_OUT) ;

	      _XmClearFocusPath( focus_data->focus_item) ;
	      _XmSetFocusFlag( shell, XmFOCUS_RESET, TRUE) ;
	      XtSetKeyboardFocus( shell, firstManaged) ;
	      _XmSetFocusFlag( shell, XmFOCUS_RESET, FALSE) ;
	    }
	  _XmFreeTravGraph( &(focus_data->trav_graph)) ;
	}
    }
}


XmFocusData
_XmGetFocusData (Widget wid)
{
   /* This function returns a pointer to the focus data associated with the
    * topmost shell.  This allows us to treat the location opaquely.
    */
   while (wid && !XtIsShell (wid))
   {
      wid = XtParent (wid);
   }
   if (wid && !(wid->core.being_destroyed))
   {
      if (XmIsVendorShell (wid))
      {
         XmWidgetExtData xwed = _XmGetWidgetExtData (wid, XmSHELL_EXTENSION);
         XmVendorShellExtObject vse;

         if (xwed == NULL)
            return NULL;
         vse = (XmVendorShellExtObject) xwed->widget;
         if (vse && vse->vendor.focus_data)
         {
            vse->vendor.focus_data->focus_policy = vse->vendor.focus_policy;
            return vse->vendor.focus_data;
         }
      }
      else
      {
         if (XmIsMenuShell (wid)
             && ((XmMenuShellWidget) wid)->menu_shell.focus_data)
         {
            ((XmMenuShellWidget) wid)->menu_shell.focus_data->
               focus_policy =
               ((XmMenuShellWidget) wid)->menu_shell.focus_policy;
            return ((XmMenuShellWidget) wid)->menu_shell.focus_data;
         }
      }
   }
   return NULL;
}


Boolean
_XmComputeVisibilityRect(Widget      w,
			 XRectangle *rectPtr,
			 Boolean     include_initial_border,
			 Boolean     allow_scrolling)
{
  /* This function will generate a rectangle describing the portion of
   * the specified widget which is not clipped by any of its ancestors.
   * It also verifies that the ancestors are both managed and
   * mapped_when_managed.
   *
   * It will return TRUE if the rectangle returned in rectPtr has a
   * non-zero area; it will return FALSE if the widget is not visible.
   *
   * If allow_scrolling is set and w is the work area child of an
   * automatic scrolled window with a non-null XmNtraverseObscuredCallback,
   * then the clip window is used as the initial rectangle for w.
   */
  Widget sw;

  if (!_XmIsViewable(w))
    {
      _XmClearRect(rectPtr);
      return False;
    }

  if (allow_scrolling && w && XtParent(w) &&
      ((sw = _XmIsScrollableClipWidget(w, True, rectPtr)) != NULL))
    {
      w = sw;

      if (!_XmIsViewable(w))
        {
	  _XmClearRect(rectPtr);
	  return False;
	}
    }
  else
    {
      _XmSetRect(rectPtr, w);
    }

  if (include_initial_border)
    {
      int border = XtBorderWidth(w);

      rectPtr->x -= border;
      rectPtr->y -= border;
      rectPtr->width += 2 * border;
      rectPtr->height += 2 * border;
    }

  /* Process all widgets, excluding the shell widget. */
  while (((w = XtParent(w)) != NULL) && !XtIsShell(w))
    {
      if (!_XmIsViewable(w) || !_XmIntersectRect(rectPtr, w, rectPtr))
        {
	  _XmClearRect(rectPtr);
	  return False;
	}
    }

  return True;
}

Boolean
_XmGetPointVisibility(Widget w,
		      int    root_x,
		      int    root_y)
{
  /* Compute whether a point is really visible inside a widget. */
  XRectangle rect;

  if (_XmComputeVisibilityRect(w, &rect, TRUE, FALSE))
    {
      return ((root_x >= rect.x) &&
	      (root_x <  rect.x + (int)rect.width) &&
	      (root_y >= rect.y) &&
	      (root_y <  rect.y + (int)rect.height));
    }

  return False;
}

void
_XmSetRect(
        register XRectangle *rect,
        Widget w )
{
  /* Initialize the rectangle structure to the specified values.
   * The widget must be realized.
   */
   Widget p;
   Position x, y;

   p = XtParent(w);
   XtTranslateCoords(p ? p : w, w->core.x, w->core.y, &x, &y);
   rect->x = x + w->core.border_width;
   rect->y = y + w->core.border_width;
   rect->width = w->core.width;
   rect->height = w->core.height;
}

int
_XmIntersectRect(
        register XRectangle *srcRectA,
        register Widget widget,
        register XRectangle *dstRect )
{
  /* Intersects the specified rectangle with the rectangle describing the
   * passed-in widget.  Returns True if they intersect, or False if they
   * do not.
   */
  XRectangle srcRectB;

  _XmSetRect(&srcRectB, widget);

  return( (int) _XmIntersectionOf( srcRectA, &srcRectB, dstRect)) ;
}

int
_XmEmptyRect(
        register XRectangle *r )
{
   if (r->width <= 0 || r->height <= 0)
      return (TRUE);

   return (FALSE);
}

void
_XmClearRect(
        register XRectangle *r )
{
   r->x = 0;
   r->y = 0;
   r->width = 0;
   r->height = 0;
}

Boolean
_XmIsNavigable(
	Widget wid)
{
  XmNavigability nav = _XmGetNavigability( wid) ;
  if(    (nav != XmTAB_NAVIGABLE)
     &&  (nav != XmCONTROL_NAVIGABLE)    )
    {
      return FALSE ;
    }
  while(    (wid = XtParent( wid)) && !XtIsShell( wid)    )
    {
      if(    !_XmGetNavigability( wid)    )
	{
	  return FALSE ;
	}
    }
  return TRUE ;
}

void
_XmWidgetFocusChange(
        Widget wid,
        XmFocusChange change)
{
  XmBaseClassExt *er ;

  if(    XtIsRectObj( wid)
     && !wid->core.being_destroyed    )
    {
      if(    (er = _XmGetBaseClassExtPtr( XtClass( wid), XmQmotif))
	 && (*er)
	 && ((*er)->version >= XmBaseClassExtVersion)
	 && (*er)->focusChange    )
        {
	  (*((*er)->focusChange))( wid, change) ;
	}
      else
        {   /* From here on is compatibility code.
	     */
	  WidgetClass wc ;

	  if(    XmIsPrimitive( wid)    )
            {
	      wc = (WidgetClass) &xmPrimitiveClassRec ;
	    }
	  else if(    XmIsGadget( wid)     )
	    {
	      wc = (WidgetClass) &xmGadgetClassRec ;
	    }
	  else if(    XmIsManager( wid)    )
	    {
	      wc = (WidgetClass) &xmManagerClassRec ;
	    }
	  else
	    {
	      wc = NULL ;
	    }

	  if(    wc
	     && (er = _XmGetBaseClassExtPtr( wc, XmQmotif))
	     && (*er)
	     && ((*er)->version >= XmBaseClassExtVersion)
	     && (*er)->focusChange    )
            {
	      (*((*er)->focusChange))( wid, change) ;
	    }
	}
    }
  return ;
}

Widget
_XmNavigate(
        Widget wid,
        XmTraversalDirection direction )
{
  XmTraversalDirection local_dir;
  XmFocusData focus_data;
  Widget nav_wid = NULL ;
  Widget shell = _XmFindTopMostShell( wid) ;

  if(    (focus_data = _XmGetFocusData( shell))
     &&  (focus_data->focus_policy == XmEXPLICIT)    )
    {
      XmTravGraph trav_list = &(focus_data->trav_graph) ;

      nav_wid = _XmTraverse( trav_list, direction, &local_dir, wid) ;

      nav_wid = RedirectTraversal(focus_data->focus_item, nav_wid,
				  focus_data->focus_policy, local_dir, 0);

      if(    trav_list->num_entries
	 &&  (focus_data->focalPoint == XmUnrelated)
	 &&  (    XmIsVendorShell( shell)
	      ||  !XmeFocusIsInShell( shell))    )
	{
	  _XmFreeTravGraph( trav_list) ;
	}
    }
  return nav_wid ;
}

void
_XmSetInitialOfTabGroup(
	Widget tab_group,
	Widget init_focus)
{
  XmFocusData focus_data ;

  if(    XmIsManager( tab_group)    )
    {
      ((XmManagerWidget) tab_group)->manager.initial_focus = init_focus ;
    }
  if(    (focus_data = _XmGetFocusData( tab_group))
     &&  focus_data->trav_graph.num_entries    )
    {
      _XmSetInitialOfTabGraph( &(focus_data->trav_graph),
			      tab_group, init_focus) ;
    }
}

static Boolean
IsTraversable(
        Widget wid,
	Boolean require_in_view)
{
  if(    wid
     &&  _XmIsNavigable( wid)    )
    {
      if(    require_in_view    )
	{
	  return (XmGetVisibility( wid) != XmVISIBILITY_FULLY_OBSCURED) ;
	}
      else
	{
	  /* _XmGetEffectiveView() returns the view port in
	   * which the widget could be viewed through the use
	   * of the XmNtraverseObscuredCallback of ScrolledWindow.
	   */
	  XRectangle visRect ;

	  return _XmGetEffectiveView( wid, &visRect) ;
	}
    }
  return FALSE ;
}

void
_XmResetTravGraph(
	Widget wid)
{
  XmFocusData focus_data = _XmGetFocusData( wid) ;

  if(    focus_data  &&  focus_data->trav_graph.num_entries    )
    {
      _XmFreeTravGraph( &(focus_data->trav_graph)) ;
    }
}

Boolean
XmeFocusIsInShell(
        Widget wid)
{
  Window focus ;
  Widget focus_wid ;
  Widget shell_of_wid = _XmFindTopMostShell( wid) ;
  XmFocusData focus_data ;
  int revert ;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  if(    XmIsVendorShell( shell_of_wid)
     &&  (focus_data = _XmGetFocusData( shell_of_wid))    )
    {
      if(    focus_data->focalPoint != XmUnrelated    )
	{
	  _XmAppUnlock(app);
	  return TRUE ;
	}
    }
  else
    {
      XGetInputFocus( XtDisplay( shell_of_wid), &focus, &revert) ;

      if(    (focus != PointerRoot)
	 &&  (focus != None)
	 &&  (focus_wid = XtWindowToWidget( XtDisplay( shell_of_wid), focus))
	 &&  (shell_of_wid == _XmFindTopMostShell( focus_wid))    )
	{
	  _XmAppUnlock(app);
	  return TRUE ;
	}
    }
  _XmAppUnlock(app);
  return FALSE ;
}

Boolean
_XmShellIsExclusive(
	Widget wid)
{
  XmFocusData focusData = _XmGetFocusData( wid) ;

  if(    focusData
     &&  focusData->trav_graph.exclusive    )
    {
      return TRUE ;
    }
  return FALSE ;
}

static Widget
FindFirstFocus(
	Widget wid)
{
  Widget shell = _XmFindTopMostShell( wid) ;

  return _XmNavigate( shell, XmTRAVERSE_CURRENT) ;
}

Widget
_XmGetFirstFocus(
	Widget wid)
{
  XmFocusData focus_data = _XmGetFocusData( wid) ;

  if(    focus_data    )
    {
      if(    focus_data->focus_item    )
	{
	  return focus_data->focus_item ;
	}
      else
	{
	  if(    focus_data->first_focus == NULL    )
	    {
	      focus_data->first_focus = FindFirstFocus( wid) ;
	    }
          return focus_data->first_focus ;
	}
    }
  return NULL ;
}


/*******************
 * Public procedures
 *******************/

Boolean
XmIsTraversable(
        Widget wid)
{
  Boolean traversable;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  traversable = IsTraversable( wid, FALSE) ;
  _XmAppUnlock(app);
  return traversable;
}

XmVisibility
XmGetVisibility(
        Widget wid)
{
  XRectangle rect ;
  Window rootwindow, parent_window, p_window, *children;
  unsigned int numchildren;
  int i;
  Window *windowptr;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  if(    !wid
     || !_XmComputeVisibilityRect(wid, &rect, FALSE, TRUE)    )
    {
      _XmAppUnlock(app);
      return( XmVISIBILITY_FULLY_OBSCURED) ;
    }
  if(    (rect.width != XtWidth( wid))
     || (rect.height != XtHeight( wid))    )
    {
      _XmAppUnlock(app);
      return( XmVISIBILITY_PARTIALLY_OBSCURED) ;
    }

  /* Obscurity by siblings */

  children = NULL;
  if (!(parent_window = XtWindow(XtParent(wid))) ||
	XQueryTree( XtDisplay(wid), parent_window, &rootwindow,
		&p_window, &children, &numchildren ) == 0)
  {
        if (children) XFree(children);
        _XmAppUnlock(app);
        return( XmVISIBILITY_UNOBSCURED) ;
  }

  windowptr = children;

  /* walk through those which are under the window of interest */
  for ( i = 0; i < numchildren; i++ )
  {
        if ( *windowptr == XtWindow(wid))
        {
	  break;
        }
        windowptr++;
  }
  i++;
  windowptr++;

  /* process windows above the window of interest */
  if(i < numchildren) {
      XRectangle parent_rect, srcRectB, intersect_rect;
      XWindowAttributes window_attributes_return;
      Region region = XCreateRegion();
      Region tmp_region = XCreateRegion();
      Region left_region = XCreateRegion();
      XmVisibility value;

      XUnionRectWithRegion(&rect, region, region);
      while(i < numchildren) {
        XGetWindowAttributes(XtDisplay(wid), *windowptr,
				&window_attributes_return);

        if(window_attributes_return.map_state == IsViewable) {
          _XmSetRect(&parent_rect, XtParent(wid));
          srcRectB.x = parent_rect.x + window_attributes_return.x
					+ window_attributes_return.border_width;
          srcRectB.y = parent_rect.y + window_attributes_return.y
					+ window_attributes_return.border_width;
          srcRectB.width = window_attributes_return.width;
          srcRectB.height = window_attributes_return.height;

          /* accumulate all the region covered by siblings */
          if(_XmIntersectionOf( &rect, &srcRectB, &intersect_rect)) {
            XUnionRectWithRegion(&intersect_rect, tmp_region, tmp_region);
          }
        }
        i++;
        windowptr++;
     }

     XSubtractRegion(region, tmp_region, left_region);
     if(XEqualRegion(region, left_region)) {
	value = XmVISIBILITY_UNOBSCURED;
     } else if(XEmptyRegion(left_region)) {
	value = XmVISIBILITY_FULLY_OBSCURED;
     } else {
	value = XmVISIBILITY_PARTIALLY_OBSCURED;
     }
     XDestroyRegion(region);
     XDestroyRegion(tmp_region);
     XDestroyRegion(left_region);
    if (children) XFree(children);
     _XmAppUnlock(app);
     return (value);
  }
  XFree((Window*)children);

  _XmAppUnlock(app);
  return( XmVISIBILITY_UNOBSCURED) ;
}

Widget
XmGetTabGroup(
        Widget wid)
{
  XmFocusData focus_data ;
  Boolean exclusive ;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  if(    !wid
     || (_XmGetFocusPolicy( wid) != XmEXPLICIT)
     || !(focus_data = _XmGetFocusData( wid))    )
    {
      _XmAppUnlock(app);
      return( NULL) ;
    }
  exclusive = !!(focus_data->trav_graph.exclusive) ;

  do
    {
      XmNavigationType nav_type = _XmGetNavigationType( wid) ;

      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	 ||  (    (nav_type == XmTAB_GROUP)
	      &&  !exclusive)    )
	{
	  _XmAppUnlock(app);
	  return( wid) ;
	}
    } while(    (wid = XtParent( wid)) && !XtIsShell( wid)    ) ;

  _XmAppUnlock(app);
  return wid ;
}

Widget
XmGetFocusWidget(
        Widget wid)
{
  Widget focus_wid = NULL ;
  XmFocusData focus_data = _XmGetFocusData( wid) ;
  _XmWidgetToAppContext(wid);

  _XmAppLock(app);
  if(    focus_data != NULL    )
    {
      if(    focus_data->focus_policy == XmEXPLICIT    )
        {
	  focus_wid = focus_data->focus_item ;
	}
      else
        {
	  focus_wid = focus_data->pointer_item ;

	  if(    (focus_wid != NULL)
	     &&  XmIsManager( focus_wid)
	     &&  (((XmManagerWidget) focus_wid)
		                     ->manager.highlighted_widget != NULL)    )
	    {
	      focus_wid = ((XmManagerWidget) focus_wid)
		                                 ->manager.highlighted_widget ;
	    }
	}
    }
  _XmAppUnlock(app);
  return focus_wid ;
}

Boolean
XmProcessTraversal(
        Widget w,
        XmTraversalDirection dir)
{
  XmFocusData focus_data ;
  Boolean ret_val = FALSE;

  if ( w )
  {
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    if(    (focus_data = _XmGetFocusData( w))
       &&  (focus_data->focus_policy == XmEXPLICIT)    )
      {
        if(    dir != XmTRAVERSE_CURRENT    )
          {
	    if(    focus_data->focus_item    )
	      {
	        w = focus_data->focus_item ;
              }
	    else
	      {
	        w = _XmFindTopMostShell( w) ;
	      }
          }
        ret_val = _XmMgrTraversal( w, dir) ;
      }
    _XmAppUnlock(app);
  }
  return ret_val;
}

void
XmAddTabGroup(
        Widget tabGroup )
{
    Arg		arg;

    XtSetArg(arg, XmNnavigationType, XmEXCLUSIVE_TAB_GROUP);
    XtSetValues(tabGroup, &arg, 1);
}

void
XmRemoveTabGroup(
        Widget w )
{
  Arg		arg;

  XtSetArg(arg, XmNnavigationType, XmNONE);
  XtSetValues(w, &arg, 1);
}

/*
 * Invoke the traversal redirection trait for all ancestors of both
 * old_focus and new_focus.  Repeat until concensus is achieved.
 */
static Widget
RedirectTraversal(Widget	       old_focus,
		  Widget	       new_focus,
		  unsigned int	       focus_policy,
		  XmTraversalDirection direction,
		  unsigned int	       pass)
{
  XmTraversalControlTrait trav_trait;
  Widget proposal = new_focus;
  Widget parent;

  /* Try not to get into an infinite loop. */
  if (pass >= 255)
    {
      assert(FALSE);
      return NULL;
    }

  /* Check ancestors of the old focus. */
  for (parent = old_focus; parent != NULL; parent = XtParent(parent))
    {
      trav_trait = (XmTraversalControlTrait)
	XmeTraitGet((XtPointer) XtClass(parent), XmQTtraversalControl);

      if (trav_trait && trav_trait->redirect)
	{
	  proposal = trav_trait->redirect(old_focus, new_focus,
					  focus_policy, direction, pass);
	  if (proposal != new_focus)
	    return RedirectTraversal(old_focus, proposal,
				     focus_policy, direction, pass + 1);
	}
    }

  /* Check ancestors of the new focus. */
  for (parent = new_focus; parent != NULL; parent = XtParent(parent))
    {
      trav_trait = (XmTraversalControlTrait)
	XmeTraitGet((XtPointer) XtClass(parent), XmQTtraversalControl);

      if (trav_trait && trav_trait->redirect)
	{
	  proposal = trav_trait->redirect(old_focus, new_focus,
					  focus_policy, direction, pass);
	  if (proposal != new_focus)
	    return RedirectTraversal(old_focus, proposal,
				     focus_policy, direction, pass + 1);
	}
    }

  /* Nobody changed our mind. */
  return new_focus;
}
