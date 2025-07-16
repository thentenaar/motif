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
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char *rcsid = "$XConsortium: RCPopup.c /main/7 1996/03/28 15:15:24 daniel $";
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include "XmI.h"
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ShellP.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/GadgetP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuT.h>
#include <Xm/MenuUtilP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/RowColumnP.h>
#include <Xm/TraitP.h>
#include "GadgetUtiI.h"
#include "HashI.h"
#include "MapEventsI.h"
#include "MenuStateI.h"
#include "MessagesI.h"
#include "RCMenuI.h"
#include "RowColumnI.h"
#include "ScreenI.h"
#include "TearOffI.h"
#include "UniqueEvnI.h"


static void ButtonEventHandler(
                        Widget w,
                        XtPointer data,
                        XEvent *event,
                        Boolean *cont) ;
static void PopupMenuEventHandler(Widget wid, XtPointer ignored,
				  XEvent *e, Boolean *cont) ;
static void AddHandlersToPostFromWidget(
                        Widget popup,
                        Widget widget) ;
static Widget FindBestMatchWidget(Widget, XEvent*);
static Widget FindPopupMenu(Widget toplevel, Widget target,
			    XEvent* event, int level);
static void EventNoop(
                        Widget reportingWidget,
                        XtPointer data,
                        XEvent *event,
                        Boolean *cont) ;
static Boolean ProcessKey(
                        XmRowColumnWidget rowcol,
                        XEvent *event) ;
static Boolean CheckKey(
                        XmRowColumnWidget rowcol,
                        XEvent *event) ;
static int MatchInKeyboardList(
                        XmRowColumnWidget rowcol,
                        XKeyEvent *event,
                        int startIndex) ;
static Boolean AllWidgetsAccessible(Widget w) ;
static int OnPostFromList(XmRowColumnWidget menu,
			  Widget widget ) ;
static void RemoveTable(Widget, XtPointer, XtPointer) ;
static Widget MenuMatches(Widget possible, int level, XEvent*);

/* Used to store popup information in hash table for non-popup-list
   menus added by XmAddToPopupFromList */

typedef struct _PopupListRec {
  WidgetList	popups;
  Cardinal	num_popups;
} PopupListRec, *PopupList;

static XmHashTable popup_table = (XmHashTable) NULL;

/**************************************************************************/
/* From the Specification:
 *
 *   The API view changes with the addition of new values for existing
 *   resources and the addition of a specialized callback for the classes
 *   Primitive and Manager. The combination of these provides the
 *   necessary means to inform Motif of automatic popup menu behavior and
 *   allows user to customize of the automatic behavior. The current delay
 *   for unhandled menu events (a three second hang) will be removed.
 *
 *   The behavior of the popup menu system will only change with
 *   automatic menus. When the user provides a triggering button event,
 *   the menu system will choose the correct menu to present.
 **************************************************************************/
/*
 * The design:
 *
 * When a user creates a new popup menu then we will install a particular
 * event handler on the menu's widget parent. Along with this we install
 * a grab on the button specified in XmNmenuPost or XmNwhichButton. The
 * nature of grabs is such that all events will go to the topmost grab
 * window, therefore the event handler must do all the child list
 * processing itself (versus the Intrinsics performing the search).
 *
 * For keyboard events,  the menu system has done half the job in the
 * past.  Here we use the new event handler for just the posting
 * of the popup menu.  We continue to use the old handler for keyboard
 * input interest for accelerators and mnemonics.
 *
 * The posting algorithm is as follows:
 *
 * 1. On receipt of a posting event, the handler will search the child
 * list for a candidate widget or gadget, and track the most specific
 * popup menu available (these can be found in the popup list). The
 * criteria for a match includes matching the XmNmenuPost information.
 *
 * 2. Matching criteria include:
 *
 *    * The menu must have XmNpopupEnabled set to either
 *      XmPOPUP_AUTOMATIC or XmPOPUP_AUTOMATIC_RECURSIVE.
 *
 *    * The popup menu is chosen according to creation order. If there is
 *      more than one, the first correct match is chosen.
 *
 *    * If the popup menu is found in a parent of the target widget, and
 *      the popup menu must also have XmNpopupEnabled set to
 *      XmPOPUP_AUTOMATIC_RECURSIVE to match.
 *
 * 3. Once a selection is made, if the menu's parent widget has a
 * popupHandlerCallback, it is invoked. The callback allows the user to
 * determine if a more specific menu is necessary, such as would be the
 * case in a graphical manipulation environment, and includes all the
 * necessary information.
 *
 * If the user has posted the menu (by clicking the appropriate button)
 * rather then just a button down, select, button up, then we need to
 * deal with the unpost and replay semantics of the menu.
 *
 * For keyboard events we use the current focus widget instead of the
 * pointer position and we always post the menu.
 **************************************************************************/

/*****************************************************************/
/*								 */
/*  New Button Press handler for the Popup menu system and the   */
/*  associated routines.   This code is installed as the event   */
/*  handler when the type of the Popup menu is 			 */
/*  XmPOPUP_AUTOMATIC or XmPOPUP_AUTOMATIC_RECURSIVE.	         */
/*								 */
/*****************************************************************/

static Widget lasttarget = (Widget) 0;

static void
PopupMenuEventHandler(Widget wid, XtPointer poster,
		      XEvent *e, Boolean *cont)
{
  Boolean seen;
  Widget postWidget;
  Widget popup;
  XmPopupHandlerCallbackStruct cb;
  XmMenuState mst = _XmGetMenuState((Widget) wid);
  XmRowColumnWidget popupW = (XmRowColumnWidget) poster ;
  Time _time = _XmGetDefaultTime(wid, e);

  /* Now look at the event type.  It better be a button or
     key event.  Otherwise abort */

  if (e -> type != ButtonPress && e -> type != ButtonRelease &&
      e -> type != KeyPress && e -> type != KeyRelease)
    return;

  /* First determine if this might have been a replay.  This variable
     gets passed to the callback as the repost field */

  seen = (mst->RC_ReplayInfo.time == _time);

  /* Decide whether this event is a posting event.  If it is
     then we turn cont off to prevent multiple
     PopupMenuEventHandlers from being invoked on a single
     event.  */

  mst->RC_ButtonEventStatus.time = _time;

  if (e -> type == KeyPress || e -> type == KeyRelease) {
    mst->RC_ButtonEventStatus.verified = True;
  } else {
    mst->RC_ButtonEventStatus.verified =
      _XmMatchBtnEvent( e, RC_PostEventType(popupW),
		       RC_PostButton(popupW),
		       RC_PostModifiers(popupW));
  }

  /* Add the handler to remove the grab if an error occurs */
  if (mst -> RC_ButtonEventStatus.verified) {
    if (!popupW->core.being_destroyed &&
	 !popupW->row_column.popup_workproc)
       popupW->row_column.popup_workproc =
	 XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) popupW),
			  _XmRC_PostTimeOut, (XtPointer) popupW);
    mst->RC_ButtonEventStatus.waiting_to_be_managed = TRUE;
    mst->RC_ButtonEventStatus.event = e -> xbutton;

    *cont = False;
  }
  else /* If this is not a verified event then return */
    return;

  if (e -> type == ButtonPress || e -> type == ButtonRelease) {
    /* OK.  We have a button event on some widget in our hierarchy.
       Go find the most specific widget */

    postWidget = FindBestMatchWidget(wid, e);
  } else {
    /* If this is a key event then use the focus data for the most
       specific widget */

    postWidget = XmGetFocusWidget(wid);

    if (postWidget == (Widget) NULL) postWidget = wid;
  }

  /* Now find the right popup */

  popup = FindPopupMenu(wid, postWidget, e, 0);

  /* Construct the callback structure */
  _XmProcessLock();
  if (seen && (postWidget == lasttarget))
    {
      cb.reason = XmCR_REPOST;
      cb.postIt = False;
    }
  else
    {
      cb.reason = XmCR_POST;
      cb.postIt = True;
    }

  lasttarget = postWidget;
  _XmProcessUnlock();


  cb.event = e;
  cb.menuToPost = popup;
  cb.target = postWidget;

  /* Call the callback(s)
   *
   * Gadgets have no callback list, hence the callbacks of the manager
   * widget are called.
   */
  switch (XtHasCallbacks(postWidget, XmNpopupHandlerCallback))
    {
    case XtCallbackNoList:
      if (XtHasCallbacks(wid, XmNpopupHandlerCallback))
        XtCallCallbacks(wid, XmNpopupHandlerCallback, (XtPointer) &cb);
      break;
    case XtCallbackHasNone:
      break;
    case XtCallbackHasSome:
      XtCallCallbacks(postWidget, XmNpopupHandlerCallback, (XtPointer) &cb);
      break;
    }


  popup = cb.menuToPost;

  if (popup != 0 && cb.postIt) {
    if (RC_TornOff(popup) && !XmIsMenuShell(XtParent(popup)))
      _XmRestoreTearOffToMenuShell((Widget)popup, e);

    /*
     * popups keep active widget in postFromList in cascadeBtn field -
     * moved here from PositionMenu so XmGetPostedFromWidget set up
     * BEFORE application's event handler.
     */
    RC_CascadeBtn(popup) = XtParent(XtParent(popup));

    if (e -> type == KeyPress || e -> type == KeyRelease) {
      XmRowColumnClassRec * rc;

      rc = (XmRowColumnClassRec *)XtClass(popup);
      (*(rc->row_column_class.armAndActivate)) (popup, e, NULL, NULL);
    } else {
      XmMenuPosition(popup, (XButtonPressedEvent *) e);
      XtManageChild(popup);
    }
  } else {
    /* If we've gotten to here,  we decided not to post the
       menu,  so continue to process the event */
    *cont = True;
  }

}

/***************************************************************
 * FindBestMatchWidget walks down to the most specific widget
 * that matches the event.  For each apparent range match,  we
 * check if this matching widget is a composite.  Each composite
 * then pushes a level and determines if a subwidget of the
 * composite might match the event
 ***************************************************************/

static Widget
FindBestMatchWidget(Widget wid, XEvent* event)
{
  Widget target;
  Widget possible;
  CompositeRec *ctarget;
  int i;
  int appx, appy;
  int delx, dely;
  int done;
  int found;

  target = wid;
  done = 0;

  /* appx,appy always contain the relative event (x,y) in
     the current widget being examined */
  appx = event -> xbutton.x;
  appy = event -> xbutton.y;

  while(done == 0) {
    if (XtIsComposite(target))
      {
	ctarget = (CompositeRec *) target;
	found = 0;
	for(i = 0; i < ctarget -> composite.num_children; i++) {
	  possible = ctarget -> composite.children[i];
	  if (XtIsManaged(possible)) {
	    delx = appx - XtX(possible);
	    dely = appy - XtY(possible);
	    if (delx >= 0 && delx <= XtWidth(possible) &&
		dely >= 0 && dely <= XtHeight(possible)) {
	      target = possible;
	      appx = delx;
	      appy = dely;
	      found = 1;
	      break;
	    }
	  }
	}
	/* Composite with no children or event outside children */
	if (found == 0) done = 1;
      }
    else
      done = 1;
  }

  return(target);
}

/***********************************************************
 * FindPopupMenu starts at the target widget (at level == 0)
 * and starts to walk up the widget hierarchy up to the
 * toplevel widget.
 *
 * At each level,  the popup list is searched for a popup
 * menu with the correct menuPost flags and the correct
 * value of popupEnabled.
 **********************************************************/

static Widget
FindPopupMenu(Widget toplevel, Widget target, XEvent *event, int level)
{
  int i;
  WidgetRec *thiswid = (WidgetRec *) target;
  Widget possible = NULL;

  if (target == NULL) return(NULL);

  if (! XmIsGadget(target)) {
    for(i = 0; i < thiswid -> core.num_popups; i++) {
      possible = thiswid -> core.popup_list[i];
      if ((possible = MenuMatches(possible, level, event)) != NULL) break;
    }
    if (! possible) {
      PopupList list = NULL;
      _XmProcessLock();
      if (popup_table) {
      /* How we could get here without the table being inited
	 is beyond me.  But just in case,  avoid crashing */
      	list = (PopupList) _XmGetHashEntry(popup_table, (XmHashKey) target);
      }
      _XmProcessUnlock();
      if (list) {
	for(i = 0; i < list -> num_popups; i++) {
	  possible = list -> popups[i];
	  if ((possible = MenuMatches(possible, level, event)) != NULL) break;
	}
      }
    }
  }

  if (possible)
    return(possible);
  else if (toplevel != target)
    return(FindPopupMenu(toplevel, XtParent(target), event, level + 1));
  else
    return((Widget) NULL);
}

static Widget
MenuMatches(Widget menu, int level, XEvent *event)
{
  Boolean found = False;

  if (XtIsShell(menu) &&
      ((CompositeRec *) menu) -> composite.num_children == 1) {
    menu = ((CompositeRec *) menu) -> composite.children[0];
    if (XmIsRowColumn(menu) &&
	IsPopup(menu) &&
	(((level == 0) &&
	  (RC_PopupEnabled(menu) == XmPOPUP_AUTOMATIC)) ||
	 (RC_PopupEnabled(menu) == XmPOPUP_AUTOMATIC_RECURSIVE)))
      /* We have a candidate.  It is a popup menu with
	 an appropriate popup enabled value */
      {
	if (event -> type == KeyPress || event -> type == KeyRelease) {
	  found = MatchInKeyboardList((XmRowColumnWidget) menu,
				      (XKeyEvent *) event, 0) != -1;
	} else {
	  found = _XmMatchBtnEvent(event, RC_PostEventType(menu),
				   RC_PostButton(menu),
				   RC_PostModifiers(menu));
	}
      }
  }

  if (found)
    return(menu);
  else
    return((Widget) NULL);
}


/*Argsused*/
Boolean
_XmRC_PostTimeOut( XtPointer wid )
{
  XmRowColumnWidget popup = (XmRowColumnWidget) wid;
  XmMenuState mst = _XmGetMenuState((Widget) wid);
  Time _time = XtLastTimestampProcessed(XtDisplay((Widget) wid));

  popup->row_column.popup_workproc = 0;
  if (mst->RC_ButtonEventStatus.waiting_to_be_managed)
  {
     XtUngrabPointer((Widget) popup, _time);
     mst->RC_ButtonEventStatus.waiting_to_be_managed = FALSE;
     mst->RC_ButtonEventStatus.verified = FALSE;
  }

  return TRUE;
}

/*
 * ButtonEventHandler is inserted at the head of the event handlers.  We must
 * pre-verify the events that popup a menupane.  When the application manages
 * the popup menupane, MenuShell's managed_set_changed(), checks the
 * verification.
 */
static void
ButtonEventHandler(
        Widget w,
        XtPointer data,
        XEvent *event,
        Boolean *cont )
{
   XmRowColumnWidget popup = (XmRowColumnWidget) data ;
   XButtonEvent *xbutton_event = (XButtonEvent *)event;
   XmMenuState mst = _XmGetMenuState((Widget) w);

   /*
    * if this event  has been seen before, and some other popup has
    * already marked it as verified, then we don't need to bother with it.
    */
   if ((mst->RC_ButtonEventStatus.time == xbutton_event->time) &&
       (mst->RC_ButtonEventStatus.verified == True))
     /*
      * If this is a torn-off menu, then we should really make sure
      * that THIS menu, not the tear-off was verified.  Otherwise,
      * there is no way for the menu's client (ie. mwm) to map the
      * window since it may be currently reparented.
      */
     if ( !RC_TornOff(popup) ||
	  !_XmMatchBtnEvent(event, RC_PostEventType(popup), RC_PostButton(popup),
			    RC_PostModifiers(popup)) )
       return;

   mst->RC_ButtonEventStatus.time = xbutton_event->time;
   mst->RC_ButtonEventStatus.verified = _XmMatchBtnEvent( event,
      RC_PostEventType(popup), RC_PostButton(popup), RC_PostModifiers(popup));

   if (mst->RC_ButtonEventStatus.verified)
   {
     XtUngrabPointer((Widget) popup,CurrentTime);
     mst->RC_ButtonEventStatus.waiting_to_be_managed = TRUE;
     if (!popup->core.being_destroyed &&
	 !popup->row_column.popup_workproc)
       popup->row_column.popup_workproc =
	 XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) popup),
			  _XmRC_PostTimeOut, (XtPointer) popup);
     mst->RC_ButtonEventStatus.event = *xbutton_event;

     if (RC_TornOff(popup) && !XmIsMenuShell(XtParent(popup)))
       _XmRestoreTearOffToMenuShell((Widget)popup, event);

     /*
      * popups keep active widget in postFromList in cascadeBtn field -
      * moved here from PositionMenu so XmGetPostedFromWidget set up
      * BEFORE application's event handler.
      */
     RC_CascadeBtn(popup) = XtWindowToWidget(XtDisplay(popup),
					     xbutton_event->window);
   }
}

static void
AddHandlersToPostFromWidget(
        Widget popup,
        Widget widget )
{
   Cursor cursor;

   cursor = _XmGetMenuCursorByScreen(XtScreen(popup));

   if (RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC ||
       RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC_RECURSIVE)
     XtInsertEventHandler(widget, ButtonPressMask|ButtonReleaseMask,
			  False, PopupMenuEventHandler,
			  (XtPointer) popup, XtListHead);
   else
     XtInsertEventHandler(widget, ButtonPressMask|ButtonReleaseMask,
			  False, ButtonEventHandler, (XtPointer) popup, XtListHead);

   if ((RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC ||
	RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC_RECURSIVE))
     XtAddEventHandler(widget, KeyPressMask|KeyReleaseMask,
		       False, PopupMenuEventHandler, (XtPointer) popup);
   else
     XtAddEventHandler(widget, KeyPressMask|KeyReleaseMask,
		       False, _XmRC_KeyboardInputHandler, (XtPointer) popup);

   /*
    * Add an event handler on the associated widget for ButtonRelease
    * events.  This is so that a quick press/release pair does not get
    * lost if the release is processed before our pointer grab is made.
    * This will guarantee that the associated widget gets the button
    * release event; it would be discarded if the widget was not selecting
    * for button release events.
    */
   XtAddEventHandler(widget, ButtonReleaseMask,
		     False, EventNoop, NULL);

   /*
    * Must add a passive grab, so that owner_events is set to True
    * when the button grab is activated; this is so that enter/leave
    * events get dispatched by the server to the client.
    */
   XtGrabButton (widget, RC_PostButton(popup), RC_PostModifiers(popup),
		 TRUE, (unsigned int)ButtonReleaseMask, GrabModeAsync,
		 GrabModeSync, None, cursor);
}

void
_XmRC_RemoveHandlersFromPostFromWidget(
        Widget popup,
        Widget widget )
{

   if (RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC ||
       RC_PopupEnabled(popup) == XmPOPUP_AUTOMATIC_RECURSIVE)
     XtRemoveEventHandler(widget, ButtonPressMask|ButtonReleaseMask,
			  False, PopupMenuEventHandler, (XtPointer) popup);
   else
     XtRemoveEventHandler(widget,	ButtonPressMask|ButtonReleaseMask,
			  False, ButtonEventHandler, (XtPointer) popup);

   XtRemoveEventHandler(widget,	KeyPressMask|KeyReleaseMask,
			False, _XmRC_KeyboardInputHandler, (XtPointer) popup);

   XtRemoveEventHandler(widget, ButtonReleaseMask,
			False, EventNoop, NULL);

   /* Remove our passive grab */
   if (!widget->core.being_destroyed)
      XtUngrabButton (widget, RC_PostButton(popup), AnyModifier);
}


/*
 * Add the Popup Menu Event Handlers needed for posting and accelerators
 */
void
_XmRC_AddPopupEventHandlers(
        XmRowColumnWidget pane )
{
   int i;

   /* to myself for gadgets */
   XtAddEventHandler( (Widget) pane, KeyPressMask|KeyReleaseMask,
		     False, _XmRC_KeyboardInputHandler, (XtPointer) pane);

   /* Add to Our shell parent */
   XtAddEventHandler(XtParent(pane), KeyPressMask|KeyReleaseMask,
		     False, _XmRC_KeyboardInputHandler, pane);

   /* add to all of the widgets in the postFromList*/
   for (i=0; i < pane->row_column.postFromCount; i++)
   {
      AddHandlersToPostFromWidget ((Widget) pane, pane->row_column.postFromList[i]);
   }
}

/*
 * Remove the Popup Menu Event Handlers needed for posting and accelerators
 */
void
_XmRC_RemovePopupEventHandlers(
        XmRowColumnWidget pane )
{
   int i;

   /* Remove it from us */
   XtRemoveEventHandler((Widget) pane, KeyPressMask|KeyReleaseMask,
			False, _XmRC_KeyboardInputHandler, (XtPointer) pane);

   /* Remove it from our shell parent */
   XtRemoveEventHandler(XtParent(pane), KeyPressMask|KeyReleaseMask,
			False, _XmRC_KeyboardInputHandler, (XtPointer) pane);

   /* Remove it from the postFrom widgets */
   for (i=0; i < pane->row_column.postFromCount; i++)
   {
      _XmRC_RemoveHandlersFromPostFromWidget((Widget) pane,
					     pane->row_column.postFromList[i]);
   }
}



/*
 *************************************************************************
 *
 * Semi-public Routines
 *
 *************************************************************************
 */
/*
 * _XmPostPopupMenu is intended for use by applications that do their own
 * event processing/dispatching.  This convenience routine makes sure that
 * the menu system has a crack at verifying the event before MenuShell's
 * managed_set_changed() routine tries to post the popup.
 */
void
_XmPostPopupMenu(
        Widget wid,
        XEvent *event )
{
   Window saveWindow;
   XmMenuState mst = _XmGetMenuState((Widget)wid);

   if (!(wid && XmIsRowColumn(wid) && IsPopup(wid)))
      return;

   /* We'll still verify the incoming button event.  But for all other cases
    * we'll just take for granted that the application knows what it's doing
    * and force the menu to post.
    */
   if (event->type == ButtonPress || event->type == ButtonRelease)
   {
      ButtonEventHandler( wid, (XtPointer) wid, event, NULL); /* drand #4973 */
   }
   else
      {
	 mst->RC_ButtonEventStatus.verified = True;
	 /* This could be trouble if the event type passed in does not have
	  * a time stamp!
	  */
	 mst->RC_ButtonEventStatus.time = event->xkey.time;
	 mst->RC_ButtonEventStatus.waiting_to_be_managed = True;
	 mst->RC_ButtonEventStatus.event = *((XButtonEvent *)event);
      }
   if (mst->RC_ButtonEventStatus.verified) /* sync up Xt timestamp */
   {
      saveWindow = event->xany.window;
      event->xany.window = 0;
      XtDispatchEvent(event);
      event->xany.window = saveWindow;
   }
   XtManageChild(wid);
}


/* When an application uses shared menupanes, enabling accelerators are a
 * tricky situation.  Before now, the application was required to set all
 * menu-items sensitive when the menu hierarchy unposted.  Then when the
 * activate event arrived, if the event was via an accelerator, internal
 * verification was done.  NOW, with tear offs, this may not be possible.
 * The application may need to leave menu-items insensitive so that the
 * tear off acts and looks as desired.  In this case, the application
 * calls this function to pass the events to accelerated insensitive menu
 * items for its own internal validation.
 */
void
_XmAllowAcceleratedInsensitiveUnmanagedMenuItems(
	Widget wid,
        Boolean allowed )
{
   _XmGetMenuState((Widget)wid)->
      RC_allowAcceleratedInsensitiveUnmanagedMenuItems = (Boolean)allowed;
}

static void
EventNoop(
        Widget reportingWidget,	/* unused */
        XtPointer data,		/* unused */
        XEvent *event,		/* unused */
        Boolean *cont )		/* unused */
{
   /*
    * Do nothing; the purpose is to override the actions taken by the
    * Primitive translations.
    */
}

/*
 * This is the event handler which catches, verifies and dispatches all
 * accelerators and mnemonics defined for a given menu hierarchy.  It
 * is attached to the menu's associated widget, along with an assortment
 * of other widgets.
 */
void
_XmRC_KeyboardInputHandler(
        Widget reportingWidget,
        XtPointer data,
        XEvent *event,
        Boolean *cont )		/* unused */
{
   XmRowColumnWidget topLevel = (XmRowColumnWidget) data;
   ShellWidget topLevelShell = (ShellWidget)XtParent(topLevel);
   XmMenuState mst = _XmGetMenuState((Widget)topLevel);

   /* Process the event only if not already processed */
   if (!_XmIsEventUnique(event))
      return;

   if (IsBar(topLevel) || IsOption(topLevel))
       if (! AllWidgetsAccessible((Widget) topLevel))
	   return;

   /*
    * XmGetPostFromWidget() requires help to identify the topLevel widget
    * when a menupane is posted via accelerators.
    */
   if (IsBar(topLevel) || IsOption(topLevel))
      mst->RC_LastSelectToplevel = (Widget) topLevel;
   else if ((IsPopup(topLevel) || IsPulldown(topLevel)) &&
	    !XmIsMenuShell(topLevelShell) &&
	    XmeFocusIsInShell((Widget)topLevel))
      mst->RC_LastSelectToplevel = topLevel->row_column.tear_off_lastSelectToplevel;
   else if (IsPopup(topLevel))
   {
      /* If the popup is already posted, lastSelectToplevel already set! */
      if (!(XmIsMenuShell(topLevelShell) && topLevelShell->shell.popped_up))
         mst->RC_LastSelectToplevel = reportingWidget;	/* popup */
   }
   else
      mst->RC_LastSelectToplevel = NULL;

   ProcessKey (topLevel, event);

   /* reset toplevel "accelerator" state to NULL */
   mst->RC_LastSelectToplevel = NULL;
}

/*
 * try to find a match in the menu for the key event.   Cascade down the
 * submenus if necessary
 */
static Boolean
ProcessKey(
        XmRowColumnWidget rowcol,
        XEvent *event )
{
   Boolean found = FALSE;
   int i;
   Widget child;
   Widget SaveCascadeButton;

   /* Try to use it on the current rowcol */
   if (! CheckKey (rowcol, event))
   {
      /* not used, try moving down the cascade */
      for (i=0; (i < rowcol->composite.num_children) && (! found); i++)
      {
	 child = rowcol->composite.children[i];

	 /* only check sensitive and managed cascade buttons */
	 if (XtIsSensitive(child) && XtIsManaged(child))
	 {
	    if (XmIsCascadeButtonGadget(child))
	    {
	       if (CBG_Submenu(child))
	       {
		   SaveCascadeButton = RC_CascadeBtn(CBG_Submenu(child));
		   /* Build the menu cascade for menuHistory */
		   RC_CascadeBtn(CBG_Submenu(child)) = child;
		   found = ProcessKey((XmRowColumnWidget)
		        ((XmCascadeButtonGadget)child)->cascade_button.submenu,
			event);
		   /* Restore the cascade button / submenu link in case of
		    * shared menupanes.
		    */
		   if (!found)
		       RC_CascadeBtn(CBG_Submenu(child)) = SaveCascadeButton;

	       }
	    }
	    else if (XmIsCascadeButton(child))
	    {
	       if (CB_Submenu(child))
	       {
		   SaveCascadeButton = RC_CascadeBtn(CB_Submenu(child));
		   RC_CascadeBtn(CB_Submenu(child)) = child;
		   found = ProcessKey((XmRowColumnWidget)
		        ((XmCascadeButtonWidget)child)->cascade_button.submenu,
			event);
		   if (!found)
		       RC_CascadeBtn(CB_Submenu(child)) = SaveCascadeButton;
	       }
	    }
	 }
      }
      return (found);
   }
   else
       return (True);
}

/*
 * Check if the key event is used in the rowcol
 */
static Boolean
CheckKey(
        XmRowColumnWidget rowcol,
        XEvent *event )
{
   int menu_index = 0;
   XmKeyboardData * entry;
   ShellWidget shell;

   /* Process all matching key events */
   while ((menu_index = MatchInKeyboardList(rowcol, (XKeyEvent *) event,
                                                            menu_index)) != -1)
   {
      entry = MGR_KeyboardList(rowcol) + menu_index;

      /* Ignore this entry if it is not accessible to the user */
      if (XmIsRowColumn(entry->component))
      {
	 /*
	  * Rowcols are not accessible if they are insensitive or
	  * if menubars or optionmenus are unmanaged.
	  */
	 if (! XtIsSensitive(entry->component) ||
	     ((RC_Type(entry->component) != XmMENU_POPUP) &&
	      (RC_Type(entry->component) != XmMENU_PULLDOWN) &&
	      (! XtIsManaged(entry->component))))
	 {
	    menu_index++;
	    continue;
	 }
      }
      else if (((XmIsMenuShell(XtParent(rowcol)) &&
		 ((XmMenuShellWidget)XtParent(rowcol))->shell.popped_up) ||
		!_XmGetMenuState((Widget)rowcol)->
		   RC_allowAcceleratedInsensitiveUnmanagedMenuItems) &&
               (!XtIsSensitive(entry->component) ||
		!XtIsManaged(entry->component)))
      {
      /* In general, insensitive or unmanaged buttons are not accessible.
       * However, to support shared menupanes, we will allow applications to
       *   pass all accelerated items through, regardless of senstivity/managed.
       *   EXCEPT when the pane is posted, and then sensitivity is presumed
       *   valid.
       * (The individual menu items also checks if the tear off has the focus.)
       */
	 menu_index++;
	 continue;
      }

      /*
       * For a mnemonic, the associated component must be visible, and
       * it must be in the last menupane posted.
       * This only needs to be checked for a popup or pulldown menu pane.
       */
      if (entry->isMnemonic)
      {
         if (XmeTraitGet((XtPointer) XtClass(entry->component),
			 XmQTmenuSavvy) != NULL)
	 {
	   if (IsBar(XtParent(entry->component)) &&
	       ! RC_PopupPosted(XtParent(entry->component)) &&
	       ((XmManagerWidget) XtParent(entry->component))->
	       manager.active_child == NULL)
	     {
	       menu_index++;
	       continue;
	     }
	   else if (IsPopup(XtParent(entry->component)) ||
		    IsPulldown(XtParent(entry->component)))
	     {
	       /* See if the associated shell is visible */
	       shell = (ShellWidget)XtParent(XtParent(entry->component));

	       /*
		* Verify the pane is popped up, and the active pane is our
		* parent (this is necessary because of shared menupanes.
		*/
	       if ((!shell->shell.popped_up) ||
		   (shell->composite.children[0] !=
		    XtParent(entry->component)))
		 {
		   menu_index++;
		   continue;
		 }

	       /* Verify we are the last pane */
	       if (RC_PopupPosted(shell->composite.children[0]))
		 {
		   menu_index++;
		   continue;
		 }
	     }
	 }
         else if (XmIsRowColumn(entry->component))
         {
	    /*
	     * Ignore the posting mnemonic for an option menu, if its
	     * menupane is already posted.
	     */
	    if (RC_PopupPosted(entry->component))
	    {
	       menu_index++;
	       continue;
	    }
	 }
      }

      /* Key event - make sure we're not in drag mode */
      _XmSetInDragMode(entry->component, False);

      /* Fix 7766, Ungrab the keyboard.  This seems to be necessary to
	 avoid focus problems in some cases where the activate callback
	 does "fancy" things like tracking locate.  Strictly speaking
	 the ungrab shouldn't be necessary */
      if (XmIsGadget(entry->component))
	XtUngrabKeyboard(XtParent(entry->component), CurrentTime);
      else
	XtUngrabKeyboard(entry->component, CurrentTime);

      /* Perform the action associated with the keyboard event */
      if (XmIsPrimitive(entry->component))
      {
         XmPrimitiveClassRec * prim;

         prim = (XmPrimitiveClassRec *)XtClass(entry->component);

         (*(prim->primitive_class.arm_and_activate))
                                         (entry->component, event, NULL, NULL);
      }
      else if (XmIsGadget(entry->component))
      {
         XmGadgetClassRec * gadget;

         gadget = (XmGadgetClassRec *)XtClass(entry->component);

         (*(gadget->gadget_class.arm_and_activate))
                                         (entry->component, event, NULL, NULL);
      }
      else if (XmIsRowColumn(entry->component))
      {
         XmRowColumnClassRec * rc;

         rc = (XmRowColumnClassRec *)XtClass(entry->component);
         (*(rc->row_column_class.armAndActivate)) (entry->component, event,
                                                                   NULL, NULL);
      }

      /* used the key */
      _XmRecordEvent(event);
      return (True);
   }

   /* did not use the key */
   return (False);
}

/*
 * This function searches the list of keyboard events associated with the
 * specified  row column widget to see if any of them match the
 * passed in X event.  This function can be called multiple times, to get
 * all entries which match.
 */
static int
MatchInKeyboardList(
        XmRowColumnWidget rowcol,
        XKeyEvent *event,
        int startIndex )
{
   XmKeyboardData * klist = MGR_KeyboardList(rowcol);
   int count = MGR_NumKeyboardEntries(rowcol);
   int i;

   if (klist == NULL)
      return(-1);

   for (i = startIndex; i < count; i++)
   {
      /*
       * We want to ignore shift and shift-lock for mnemonics.  So, OR the
       * event's two bits with the (previously two bits initialized to zero)
       * klist.modifier
       *
       * If the .key field is 1, then we have delayed calling XKeysymToKeycode
       * until now.
       */
       if (klist[i].key == 1)
	   klist[i].key = XKeysymToKeycode(XtDisplay(rowcol), klist[i].keysym);

       if (klist[i].key != NoSymbol)
       {
	   if (_XmMatchKeyEvent((XEvent *) event, klist[i].eventType,
				klist[i].key, klist[i].isMnemonic ?
				klist[i].modifiers | (event->state &
						     (ShiftMask | LockMask)) :
				klist[i].modifiers))
	   {
	       return(i);
	   }
       }
   }

   /* No match */
   return (-1);
}

/*
 * This function determines if the widget to which a menu is
 * attached is accessible to the user.  The widget is considered
 * accessible if it, and its ancestors, are both sensitive and
 * managed.  This is useful for MenuBars and Option Menus only.
 */
static Boolean
AllWidgetsAccessible(
        Widget w )
{
   while (w && XtParent(w) && !XtIsShell(w))
   {
      if (!XtIsSensitive(w) || !XtIsManaged(w) || !w->core.mapped_when_managed)
         return (False);

      w = XtParent(w);
   }

   return (True);
}

/*
 * search the postFromList and return the index of the found widget.  If it
 * is not found, return -1
 *
 * This function is duplicated from RCMenu.c.
 */
static int
OnPostFromList(
        XmRowColumnWidget menu,
        Widget widget )
{
   int i;

   for (i = 0; i < menu->row_column.postFromCount; i++)
   {
      if (menu->row_column.postFromList[i] == widget)
	  return (i);
   }

   return (-1);
}

/************************************************************************
 *
 * XmAddToPostFromList and XmRemoveFromPostFromList
 *
 ************************************************************************/

void
XmAddToPostFromList(
        Widget m,
        Widget widget )
{
  XmRowColumnWidget menu = (XmRowColumnWidget) m ;
  Arg args[1];
  _XmWidgetToAppContext(m);

  _XmAppLock(app);
  /* only continue if its a valid widget and a popup or pulldown menu */
  if (! XmIsRowColumn(menu) ||
      ! (IsPopup(menu) || IsPulldown(menu)) ||
      ! widget) {
    _XmAppUnlock(app);
    return;
  }

  if (OnPostFromList(menu, widget) == -1)
    {
      /* The first section of this code,  new in 2.0,  places
	 the popup information in a secondary list so we can
	 find it when doing automatic popup behavior */
      PopupList list;

      /* Add to associated popup data for widget */
      _XmProcessLock();
      if (! popup_table) {
	popup_table = _XmAllocHashTable(100, NULL, NULL);
      }

      /* See if we have an associated table.  If not,  create
	 one. */
      if ((list = (PopupList) _XmGetHashEntry(popup_table, (XmHashKey) widget))
	  == NULL) {
	/* Resize if the table has many more entries than slots */
	if (_XmHashTableCount(popup_table) >
	    (2 * _XmHashTableSize(popup_table)))
	  _XmResizeHashTable(popup_table, 2 * _XmHashTableSize(popup_table));

	list = (PopupList) XtMalloc(sizeof(PopupListRec));
	list -> popups = (WidgetList) NULL;
	list -> num_popups = 0;
	_XmAddHashEntry(popup_table, (XmHashKey) widget, (XtPointer) list);
	XtAddCallback(widget, XtNdestroyCallback, RemoveTable, NULL);
      }
      _XmProcessUnlock();

      /* Add to list,  we add the menu's shell parent */
      list -> popups = (WidgetList) XtRealloc((char*) list -> popups,
					      sizeof(Widget) *
					      (list -> num_popups + 1));
      list -> popups[list -> num_popups] = XtParent(m);
      list -> num_popups++;

      if (IsPulldown(menu))
	{
	  XtSetArg (args[0], XmNsubMenuId, menu);
	  XtSetValues (widget, args, 1);
	}
      else
	{
	  _XmRC_AddToPostFromList (menu, widget);
	  AddHandlersToPostFromWidget ((Widget) menu, widget);
	  _XmRC_DoProcessMenuTree ((Widget)menu, XmADD);
	}
    }
    _XmAppUnlock(app);
}

void
XmRemoveFromPostFromList(
        Widget m,
        Widget widget )
{
  XmRowColumnWidget menu = (XmRowColumnWidget) m ;
  Arg args[1];
  _XmWidgetToAppContext(m);

  _XmAppLock(app);
  /* only continue if its a valid widget and a popup or pulldown menu */
  if (! XmIsRowColumn(menu) ||
      ! (IsPopup(menu) || IsPulldown(menu)) ||
      ! widget) {
    _XmAppUnlock(app);
    return;
  }

  if ((OnPostFromList(menu, widget)) != -1)
    {
      PopupList list;

      _XmProcessLock();
      if (popup_table) {
	int i, j;

	/* How we could get here without the table being inited
	   is beyond me.  But just in case,  avoid crashing */
	list = (PopupList) _XmGetHashEntry(popup_table, (XmHashKey) widget);

	/* Remove from associated list */
	for(i = 0; i < list -> num_popups;) {
	  if (list -> popups[i] == XtParent(m)) {
	    /* First shift all the remaining elements */
	    for(j = i; j < list -> num_popups - 1; j++) {
	      list -> popups[j] = list -> popups[j + 1];
	    }
	    list -> num_popups--;
	  } else {
	    i++;
	  }
	}
      }
      _XmProcessUnlock();

      if (IsPulldown(menu))
	{
	  XtSetArg (args[0], XmNsubMenuId, NULL);
	  XtSetValues (widget, args, 1);
	}
      else
	{
	 _XmRC_RemoveFromPostFromList (menu, widget);
	 _XmRC_RemoveHandlersFromPostFromWidget ((Widget) menu, widget);
	 _XmRC_DoProcessMenuTree ((Widget)menu, XmDELETE);
      }
   }
   _XmAppUnlock(app);
}

static void
RemoveTable(Widget w,
	    XtPointer ig1,	/* unused */
	    XtPointer ig2)	/* unused */
{
  PopupList list;

  _XmProcessLock();
  list = (PopupList) _XmGetHashEntry(popup_table, (XmHashKey) w);
  _XmRemoveHashEntry(popup_table, (XmHashKey) w);
  XtFree((char*) list -> popups);
  XtFree((char*) list);
  _XmProcessUnlock();
}
