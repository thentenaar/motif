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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: MenuUtil.c /main/16 1999/05/13 15:57:21 mgreess $"
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/MenuShellP.h>
#include <Xm/RowColumnP.h>
#include <Xm/ScreenP.h>
#include <Xm/XmosP.h>
#include "GadgetUtiI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "MessagesI.h"
#include "RCMenuI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"

#define GRABPTRERROR    _XmMMsgCascadeB_0003
#define GRABKBDERROR    _XmMMsgRowColText_0024

#define EVENTS              ((unsigned int) (ButtonPressMask | \
                              ButtonReleaseMask | EnterWindowMask | \
                              LeaveWindowMask))

/********    Static Function Declarations    ********/

static void MenuTraverse(
                        Widget w,
                        XEvent *event,
                        XmTraversalDirection direction) ;
static void GadgetCleanup(
                        XmRowColumnWidget rc,
                        XmGadget oldActiveChild) ;
static Boolean WrapRight(
                        XmRowColumnWidget rc) ;
static Boolean WrapLeft(
                        XmRowColumnWidget rc) ;
static void LocateChild(
                        XmRowColumnWidget rc,
                        Widget wid,
                        XmTraversalDirection direction) ;
static void MoveDownInMenuBar(
                        XmRowColumnWidget rc,
                        Widget pw) ;
static void MoveLeftInMenuBar(
                        XmRowColumnWidget rc,
                        Widget pw) ;
static void MoveRightInMenuBar(
                        XmRowColumnWidget rc,
                        Widget pw) ;
static void FindNextMenuBarItem(
                        XmRowColumnWidget menubar) ;
static void FindPrevMenuBarItem(
                        XmRowColumnWidget menubar) ;
static Boolean ValidateMenuBarItem(
                        Widget oldActiveChild,
                        Widget newActiveChild) ;
static Boolean FindNextMenuBarCascade(
                        XmRowColumnWidget menubar) ;
static Boolean FindPrevMenuBarCascade(
                        XmRowColumnWidget menubar) ;
static Boolean ValidateMenuBarCascade(
                        Widget oldActiveChild,
                        Widget newMenuChild) ;

/********    End Static Function Declarations    ********/


Boolean
_XmIsActiveTearOff (
       Widget widget)
{
    XmRowColumnWidget menu = (XmRowColumnWidget) widget;

    if (RC_TearOffActive(menu))
        return (True);
    else
        return (False);
}


/*
 * Call XtGrabPointer with retry
 */
int
_XmGrabPointer(
	Widget widget,
	Bool owner_events,
	unsigned int event_mask,
	int pointer_mode,
	int keyboard_mode,
	Window confine_to,
	Cursor cursor,
	Time time )
{
   register int status = 0, retry;

   for (retry=0; retry < 5; retry++)
   {
      if ((status = XtGrabPointer(widget, owner_events, event_mask,
         			  pointer_mode, keyboard_mode, confine_to,
				  cursor, time)) == GrabSuccess)
	 break;

      XmeMicroSleep(1000);
   }
   if (status != GrabSuccess)
      XmeWarning((Widget) widget, GRABPTRERROR);

   return(status);
}

/*
 * Call XtGrabKeyboard with retry
 */
int
_XmGrabKeyboard(
	Widget widget,
	Bool owner_events,
	int pointer_mode,
	int keyboard_mode,
	Time time )
{
   register int status = 0, retry;

   for (retry=0; retry < 5; retry++)
   {
      if ((status = XtGrabKeyboard(widget, owner_events,
         pointer_mode, keyboard_mode, time)) == GrabSuccess)
	 break;
      XmeMicroSleep(1000);
   }
   if (status != GrabSuccess)
      XmeWarning(widget, GRABKBDERROR);

   return(status);
}


void
_XmMenuSetInPMMode (
	Widget wid,
	Boolean flag )
{
   _XmGetMenuState((Widget)wid)->MU_InPMMode = flag;
}

/*
 * This menuprocs procedure allows an external object to turn on and off menu
 * traversal.
 */
void
_XmSetMenuTraversal(
        Widget wid,
        Boolean traversalOn )
{
   if (traversalOn)
   {
      _XmSetInDragMode(wid, False);
      if (!XmProcessTraversal(wid , XmTRAVERSE_CURRENT))
         XtSetKeyboardFocus(XtParent(wid), wid);
   }
   else
   {
     _XmSetInDragMode(wid, True);
     if(    XmIsMenuShell( XtParent( wid))    )
       {
	 /* Must be careful not to trash the traversal environment
	  * for RowColumns which are not using menu-specific traversal.
	  */
	 _XmLeafPaneFocusOut(wid);
       }
   }
}


void
_XmLeafPaneFocusOut(
	Widget wid )
{
   XEvent fo_event;
   Widget widget;
   XmRowColumnWidget rc = (XmRowColumnWidget)wid;

   /* find the leaf pane */
   while (RC_PopupPosted(rc))
     rc = (XmRowColumnWidget)
       ((XmMenuShellWidget)RC_PopupPosted(rc))->composite.children[0];

   fo_event.type = FocusOut;
   fo_event.xfocus.send_event = True;
   if ((widget = rc->manager.active_child) && XmIsCascadeButtonGadget(widget))
   {
      /* clear the internal focus path; also active_child = NULL which happens
       * to make cascadebutton focus out work correctly.
       */
      _XmClearFocusPath((Widget)rc);
      _XmDispatchGadgetInput(widget, NULL, XmFOCUS_OUT_EVENT);
      ((XmGadget)widget)->gadget.have_traversal = False;
   }
   else
   {
      if (widget && XmIsPrimitive(widget) &&
          (((XmPrimitiveWidgetClass)(widget->core.widget_class))->
            primitive_class.border_unhighlight != (XtWidgetProc)NULL))
         (*(((XmPrimitiveWidgetClass)(widget->core.widget_class))->
            primitive_class.border_unhighlight))((Widget) widget);
      else
	 _XmManagerFocusOut( (Widget) rc, &fo_event, NULL, NULL);

      /* clears the focus_item so that next TraverseToChild() will work */
      _XmClearFocusPath((Widget)rc);
   }
}

void
_XmMenuHelp(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XmGadget gadget;

   if (!_XmIsEventUnique(event) ||
       (!RC_IsArmed(rc) && !((RC_Type(rc) == XmMENU_OPTION) ||
			     (RC_Type(rc) == XmMENU_PULLDOWN))))
     return;

   if (!_XmGetInDragMode ((Widget)rc))
   {
     if ((gadget = (XmGadget) rc->manager.active_child) != NULL)
	_XmDispatchGadgetInput( (Widget) gadget, event, XmHELP_EVENT);
     else
     {
	_XmSocorro( (Widget) rc, event, NULL, NULL);
	_XmMenuPopDown((Widget)rc, event, NULL);
     }
   }
   else
   {
     if ((gadget = (XmGadget)
	  XmObjectAtPoint((Widget) rc, event->xkey.x, event->xkey.y)) != NULL)
        _XmDispatchGadgetInput( (Widget) gadget, event, XmHELP_EVENT);
     else
     {
	_XmSocorro( (Widget) rc, event, NULL, NULL);
	_XmMenuPopDown((Widget)rc, event, NULL);
     }
   }
   _XmRecordEvent(event);
}

static void
MenuTraverse(
        Widget w,
        XEvent *event,
        XmTraversalDirection direction )
{
   Widget parent;

   /*
    * The case may occur where the reporting widget is in fact the
    * RowColumn widget, and not a child.  This will occur if the
    * RowColumn has not traversable children.
    */
   if (XmIsRowColumn(w))
      parent = w;
   else if (XmIsRowColumn(XtParent(w)))
      parent = XtParent(w);
   else
      return;

   if ((RC_Type(parent) == XmMENU_POPUP) ||
       (RC_Type(parent) == XmMENU_PULLDOWN) ||
       (RC_Type(parent) == XmMENU_BAR))
   {
      _XmRecordEvent(event);
      (*(((XmRowColumnWidgetClass)XtClass(parent))->row_column_class.
          traversalHandler))( (Widget) parent, (Widget) w, direction);
   }
}

void
_XmMenuTraverseLeft(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
    if (_XmIsEventUnique(event))
   {
	 MenuTraverse(wid, event, XmTRAVERSE_LEFT);
   }
}

void
_XmMenuTraverseRight(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   if (_XmIsEventUnique(event))
   {
	 MenuTraverse(wid, event, XmTRAVERSE_RIGHT);
   }
}

void
_XmMenuTraverseUp(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   if (_XmIsEventUnique(event))
   {
	 MenuTraverse(wid, event, XmTRAVERSE_UP);
   }
}

void
_XmMenuTraverseDown(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   if (_XmIsEventUnique(event))
   {
	 MenuTraverse(wid, event, XmTRAVERSE_DOWN);
   }
}

void
_XmMenuEscape(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   Widget parent = XtParent(w);

   /* Process the event only if not already processed */
   if (!_XmIsEventUnique(event))
      return;

   /*
    * Catch case where its a menubar w/ no submenus up - can't call
    *   menushell's popdown, call rowcolumn's instead.
    */
   if ((XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w)) &&
	XmIsRowColumn(parent) && (RC_Type(parent) == XmMENU_BAR) &&
	!RC_PopupPosted(parent))
   {
      (*(((XmRowColumnClassRec *)XtClass(parent))->row_column_class.
	 menuProcedures)) (XmMENU_POPDOWN, parent, NULL, event, NULL);
   }
   else
       /* Let the menushell widget clean things up */
       (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	  menu_shell_class.popdownOne))(w, event, NULL, NULL);
}

void
_XmRC_GadgetTraverseDown(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseDown((Widget) gadget, event, param, num_param);
}

void
_XmRC_GadgetTraverseUp(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseUp((Widget) gadget, event, param, num_param);
}

void
_XmRC_GadgetTraverseLeft(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   /*
    * If there is not active child, then this RowColumn has
    * no traversable children, so it's fielding traversal
    * requests itself.
    */
   if (gadget)
      _XmMenuTraverseLeft((Widget) gadget, event, param, num_param);
   else
      _XmMenuTraverseLeft((Widget) rc, event, param, num_param);
}

void
_XmRC_GadgetTraverseRight(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   /*
    * If there is not active child, then this RowColumn has
    * no traversable children, so it's fielding traversal
    * requests itself.
    */
   if (gadget)
      _XmMenuTraverseRight((Widget) gadget, event, param, num_param);
   else
      _XmMenuTraverseRight((Widget) rc, event, param, num_param);
}


/*
 * In case we've moved into our out of a gadget, we need to take care
 * of the highlighting ourselves, since the gadget will not get a focus
 * event.
 */
static void
GadgetCleanup(
        XmRowColumnWidget rc,
        XmGadget oldActiveChild )
{
    XmGadget newActiveChild = (XmGadget)rc->manager.active_child;

    if (oldActiveChild != newActiveChild)
    {
        if (oldActiveChild && XmIsGadget(oldActiveChild))
        {
            _XmDispatchGadgetInput( (Widget) oldActiveChild, NULL,
                                                            XmFOCUS_OUT_EVENT);
            oldActiveChild->gadget.have_traversal = False;
        }
    }
}


/*
 * At the edge of the menu, decide what to do in this case
 */
static Boolean
WrapRight (
        XmRowColumnWidget rc )
{
   Widget topLevel;
   Widget oldActiveChild = rc->manager.active_child;
   Boolean done = False;

   _XmGetActiveTopLevelMenu ((Widget) rc, (Widget *) &topLevel);

   /* if in a menubar system, try to move to next menubar item cascade */
   if (XmIsMenuShell(XtParent(rc)) && (RC_Type(topLevel) == XmMENU_BAR) &&
       (FindNextMenuBarCascade((XmRowColumnWidget) topLevel)))
   {
      GadgetCleanup(rc, (XmGadget) oldActiveChild);
      done = True;
   }

   return (done);
}

/*
 * At the edge of the menu, decide what to do in this case
 */
static Boolean
WrapLeft (
        XmRowColumnWidget rc )
{
   Widget oldActiveChild = rc->manager.active_child;
   Boolean done = False;

   /*
    * If we're the topmost pulldown menupane from a menubar, then unpost
    * and move to the next available item in the menubar, and post its
    * submenu.
    */
   if (XmIsMenuShell(XtParent(rc)) &&
       (RC_Type (rc) != XmMENU_POPUP) && RC_CascadeBtn(rc) &&
       (RC_Type (XtParent(RC_CascadeBtn(rc))) == XmMENU_BAR) &&
       (FindPrevMenuBarCascade((XmRowColumnWidget)
                                      XtParent(RC_CascadeBtn(rc)))))
   {
      GadgetCleanup(rc, (XmGadget) oldActiveChild);
      done = True;
   }

   /*
    * if we are in a pulldown from another posted menupane, unpost this one
    */
   else if ((RC_Type(rc) == XmMENU_PULLDOWN) && RC_CascadeBtn(rc) &&
            (RC_Type(XtParent(RC_CascadeBtn(rc))) != XmMENU_OPTION) &&
            XmIsMenuShell(XtParent(rc)))
   {
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
                  menu_shell_class.popdownOne)) (XtParent(rc), NULL, NULL,
                                                 NULL);
      done = True;
   }

   return (done);
}

/*
 * Search for the next menu item according to the direction
 */
static void
LocateChild (
        XmRowColumnWidget rc,
        Widget wid,
        XmTraversalDirection direction )
{
   Boolean done = False;
   Widget nextWidget;

   /* special case a popped up submenu with no traversable items */
   if (XmIsRowColumn(wid) &&
       ((XmManagerWidget) wid)->manager.active_child == 0)
   {
     if (direction == XmTRAVERSE_LEFT)
       WrapLeft (rc);
     else if (direction == XmTRAVERSE_RIGHT)
       WrapRight (rc);
   }
   else
   {
     nextWidget = _XmNavigate(wid, direction);

     if (direction == XmTRAVERSE_LEFT)
     {
       /* watch for left wrap */
       if ((wid->core.x <= nextWidget->core.x) ||
	   (nextWidget->core.y + nextWidget->core.height <= wid->core.y) ||
	   (nextWidget->core.y >= wid->core.y + wid->core.height))
	 done = WrapLeft(rc);
     }
     else if (direction == XmTRAVERSE_RIGHT)
     {
       /* watch for right wrap */
       if ((wid->core.x >= nextWidget->core.x) ||
	   (wid->core.y + wid->core.height <= nextWidget->core.y) ||
	   (wid->core.y >= nextWidget->core.y + nextWidget->core.height))
	 done = WrapRight(rc);
     }

     if (!done)
       _XmMgrTraversal (nextWidget, XmTRAVERSE_CURRENT);
   }

}

 void
_XmMenuTraversalHandler(
        Widget w,
        Widget pw,
        XmTraversalDirection direction )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) w;

   if (_XmGetInDragMode((Widget) rc))
      return;

   if ( LayoutIsRtoLM(rc) ) {
     if (direction == XmTRAVERSE_RIGHT)
       direction = XmTRAVERSE_LEFT;
     else if (direction == XmTRAVERSE_LEFT)
       direction = XmTRAVERSE_RIGHT;
   }
   if (RC_Type(rc) != XmMENU_BAR)
   {
      /* check for cascading into a submenu */
      if (direction == XmTRAVERSE_RIGHT &&
          XmIsCascadeButtonGadget(pw) && CBG_Submenu(pw))
      {
         (*(((XmGadgetClassRec *)XtClass(pw))->gadget_class.
            arm_and_activate))( (Widget) pw, NULL, NULL, NULL);
      }
      else if (direction == XmTRAVERSE_RIGHT &&
               XmIsCascadeButton(pw) && CB_Submenu(pw))
      {
         (*(((XmPrimitiveClassRec *)XtClass(pw))->primitive_class.
             arm_and_activate)) ((Widget) pw, NULL, NULL, NULL);
      }

      else
         LocateChild (rc, pw, direction);
   }

   else
   {
       switch (direction)
       {
          case XmTRAVERSE_DOWN:
          {
             MoveDownInMenuBar (rc, pw);
             break;
          }

          case XmTRAVERSE_LEFT:
          {
	    MoveLeftInMenuBar(rc, pw);
	    break;
          }

          case XmTRAVERSE_RIGHT:
          {
	    MoveRightInMenuBar(rc, pw);
	    break;
          }

          case XmTRAVERSE_UP:
	  default:
	     break;
       }
    }
}


/*
 * When the PM menubar mode is active, down arrow will
 * cause us to post the menupane associated with the active cascade button
 * in the menubar.
 */
static void
MoveDownInMenuBar(
        XmRowColumnWidget rc,
        Widget pw )
{
    if (rc->manager.active_child == NULL)
        return;

    if (XmIsPrimitive(pw))
    {
        XmPrimitiveClassRec * prim;

	CB_SetTraverse (pw, TRUE);
        prim = (XmPrimitiveClassRec *)XtClass(pw);
        (*(prim->primitive_class.arm_and_activate)) ((Widget) pw, NULL,
						     NULL, NULL);
	CB_SetTraverse (pw, FALSE);
    }

    else if (XmIsGadget(pw))
    {
        XmGadgetClassRec * gad;

	CBG_SetTraverse (pw, TRUE);
        gad = (XmGadgetClassRec *)XtClass(pw);
        (*(gad->gadget_class.arm_and_activate)) ((Widget) pw, NULL,
						 NULL, NULL);
	CBG_SetTraverse (pw, FALSE);
    }
}

static void
MoveLeftInMenuBar(
        XmRowColumnWidget rc,
        Widget pw )
{
   XmMenuState mst = _XmGetMenuState((Widget)rc);

   if ((mst->MU_CurrentMenuChild != NULL) &&
       (RC_PopupPosted(rc) != NULL) &&
       ((XmIsCascadeButtonGadget(pw) && !CBG_Submenu(pw)) ||
       (XmIsCascadeButton(pw) && !CB_Submenu(pw))))
   {
      /* Move to the previous item in the menubar */
      FindPrevMenuBarItem(rc);
   }
   else
   {
      /* Move to the previous item in the menubar */
      mst->MU_CurrentMenuChild = NULL;
      FindPrevMenuBarItem(rc);
   }
}

static void
MoveRightInMenuBar(
        XmRowColumnWidget rc,
        Widget pw )
{
   XmMenuState mst = _XmGetMenuState((Widget)rc);

   if ((rc->manager.active_child == NULL) &&
        ((XmIsCascadeButtonGadget(pw) && !CBG_Submenu(pw)) ||
        (XmIsCascadeButton(pw) && !CB_Submenu(pw))))
   {
      FindNextMenuBarCascade(rc);
   }
   else
   {
      /* Move to the next item in the menubar */
      mst->MU_CurrentMenuChild = NULL;
      FindNextMenuBarItem(rc);
   }
}


/*
 * Find the next cascade button in the menubar which can be traversed to.
 */
static void
FindNextMenuBarItem(
        XmRowColumnWidget menubar )
{
   register int i, j;
   int upper_limit;
   Widget active_child;

   /*
    * We're not in the PM menubar mode if we don't have an active child.
    */
   if (menubar->manager.active_child == NULL)
       return;

   upper_limit = menubar->composite.num_children;
   active_child = menubar->manager.active_child;

   /* Find the index of the currently active item */
   for (i = 0; i < upper_limit; i++)
   {
      if (menubar->composite.children[i] == active_child)
	 break;
   }

   /* Start looking at the next child */
   for (j = 0, i++; j < upper_limit - 1; j++, i++)
   {
       /* Wrap, if necessary */
       if (i >= upper_limit)
	  i = 0;

	if (ValidateMenuBarItem(active_child, menubar->composite.children[i]))
	  return;
   }
}


/*
 * Find the previous cascade button in the menubar which can be traversed to.
 */
static void
FindPrevMenuBarItem(
        XmRowColumnWidget menubar )
{
   register int i, j;
   int upper_limit;
   Widget active_child;

   /* We're not in the PM menubar mode if we don't have an active child */
   if (menubar->manager.active_child == NULL)
       return;

   upper_limit = menubar->composite.num_children;
   active_child = menubar->manager.active_child;

   /* Find the index of the currently active item */
   for (i = 0; i < upper_limit; i++)
   {
       if (menubar->composite.children[i] == active_child)
	   break;
   }

   /* Start looking at the previous child */
   for (j = 0, --i; j < upper_limit - 1; j++, --i)
   {
       /* Wrap, if necessary */
       if (i < 0)
	  i = upper_limit - 1;

       if (ValidateMenuBarItem(active_child, menubar->composite.children[i]))
	  return;
   }
}

static Boolean
ValidateMenuBarItem (
	Widget oldActiveChild,
        Widget newActiveChild)
{
   XmMenuState mst = _XmGetMenuState((Widget)oldActiveChild);

   if (XmIsTraversable(newActiveChild))
   {
      (void) XmProcessTraversal (newActiveChild, XmTRAVERSE_CURRENT);

      if (XmIsPrimitive(newActiveChild))
      {
         XmPrimitiveClassRec * prim;

         prim = (XmPrimitiveClassRec *)XtClass(newActiveChild);

         if (!mst->MU_InPMMode && CB_Submenu(newActiveChild))
            (*(prim->primitive_class.arm_and_activate)) (newActiveChild, NULL,
                                                                   NULL, NULL);
     }
      else if (XmIsGadget(newActiveChild))
      {
         XmGadgetClassRec * gadget;

         gadget = (XmGadgetClassRec *)XtClass(newActiveChild);

         if (!mst->MU_InPMMode && CBG_Submenu(newActiveChild))
            (*(gadget->gadget_class.arm_and_activate)) (newActiveChild, NULL,
                                                                   NULL, NULL);
      }
      return True;
   }
   else
      return False;
}

/*
 * Find the next hierarchy in the menubar which can be traversed to.
 */
static Boolean
FindNextMenuBarCascade(
        XmRowColumnWidget menubar )
{
   Widget active_child = NULL;
   register int i, j;
   int upper_limit;
   ShellWidget shell;
   XmMenuState mst = _XmGetMenuState((Widget)menubar);

   upper_limit = menubar->composite.num_children;

   /*
    * Determine which child is popped up.
    */
   shell = (ShellWidget) RC_PopupPosted(menubar);
   if (shell != NULL)
      active_child = mst->MU_CurrentMenuChild =
         RC_CascadeBtn(shell->composite.children[0]);

   /* Find the index of the currently active item */
   for (i = 0; i < upper_limit; i++)
   {
      if (menubar->composite.children[i] == mst->MU_CurrentMenuChild)
          break;
   }

   /* Start looking at the next child */
   for (j = 0, i++; j < upper_limit - 1; j++, i++)
   {
      /* Wrap, if necessary */
      if (i >= upper_limit)
          i = 0;

      mst->MU_CurrentMenuChild = menubar->composite.children[i];
      if (ValidateMenuBarCascade(active_child, mst->MU_CurrentMenuChild))
         return True;
   }
   return False;
}


/*
 * Find the previous hierarchy in the menubar which can be traversed to.
 */
static Boolean
FindPrevMenuBarCascade(
        XmRowColumnWidget menubar )
{
    Widget active_child = NULL;
    register int i, j;
    int upper_limit;
    ShellWidget shell;
    XmMenuState mst = _XmGetMenuState((Widget)menubar);

    upper_limit = menubar->composite.num_children;

    /* Determine which child is popped up */
    shell = (ShellWidget) RC_PopupPosted(menubar);
    if (shell != NULL)
       active_child = mst->MU_CurrentMenuChild =
          RC_CascadeBtn(shell->composite.children[0]);

    /* Find the index of the currently active item */
    for (i = 0; i < upper_limit; i++)
    {
        if (menubar->composite.children[i] == mst->MU_CurrentMenuChild)
           break;
    }

    /* Start looking at the previous child */
    for (j = 0, --i; j < upper_limit - 1; j++, --i)
    {
        /* Wrap, if necessary */
        if (i < 0)
           i = upper_limit - 1;

        mst->MU_CurrentMenuChild = menubar->composite.children[i];
        if (ValidateMenuBarCascade(active_child, mst->MU_CurrentMenuChild))
           return True;
    }
    return False;
}

static Boolean
ValidateMenuBarCascade (Widget oldActiveChild, /* unused */
			Widget newMenuChild)
{
   XmRowColumnWidget menubar = (XmRowColumnWidget)XtParent(newMenuChild);
   Time _time = XtLastTimestampProcessed(XtDisplay(menubar));

   if (XmIsTraversable(newMenuChild))
   {
      if (XmIsCascadeButtonGadget(newMenuChild))
      {
         XmGadgetClassRec * gadget;

         gadget = (XmGadgetClassRec *)XtClass(newMenuChild);

         if (RC_PopupPosted(menubar) && !CBG_Submenu(newMenuChild))
         {
	     (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
		menu_shell_class.popdownEveryone))
		 (RC_PopupPosted(menubar),NULL, NULL, NULL);

            /* Return the X focus to the Menubar hierarchy from the menushell.
             * Set the Xt focus to the cascade
             */
            _XmMenuFocus((Widget) menubar, XmMENU_MIDDLE, _time);
            (void)XmProcessTraversal(newMenuChild, XmTRAVERSE_CURRENT);
         }
         else
         {
            (*(gadget->gadget_class.arm_and_activate)) (newMenuChild, NULL,
                                                                   NULL, NULL);
         }
         return True;
      }
      else if (XmIsCascadeButton (newMenuChild))
      {
         XmPrimitiveClassRec * prim;

         prim = (XmPrimitiveClassRec *)XtClass(newMenuChild);

         /* No submenu means PM mode */
         if (RC_PopupPosted(menubar) && !CB_Submenu(newMenuChild))
         {
	     (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
		menu_shell_class.popdownEveryone))
		 (RC_PopupPosted(menubar),NULL, NULL, NULL);

            /* Update X and Xt focus */
            _XmMenuFocus((Widget) menubar, XmMENU_MIDDLE, _time);
            (void)XmProcessTraversal(newMenuChild, XmTRAVERSE_CURRENT);
         }
         else
         {
            (*(prim->primitive_class.arm_and_activate)) (newMenuChild, NULL,
                                                                   NULL, NULL);
         }
         return True;
      }
   }
   return False;
}


/* (New) Grab strategy for menus requires grab before posting. If not possible
  * don't post the menu! This will ensure grabs active during a posted menu.
  * This will help consistency by preventing simultaneously posted menus.
  */
int
_XmMenuGrabKeyboardAndPointer(
      Widget widget,
      Time time )
{

   register int status =
           (_XmGrabKeyboard(widget,
                            True,
                            GrabModeSync,
                            GrabModeAsync,
                            time) != GrabSuccess);
   if (status)
      return(status);

   status = _XmGrabPointer(widget, True, EVENTS, GrabModeSync,
       GrabModeAsync, None, XmGetMenuCursor(XtDisplay(widget)), time) !=
         GrabSuccess;

   if (status)
      XtUngrabKeyboard(widget, CurrentTime);

   return(status);
}
