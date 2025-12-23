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
/*
 * HISTORY
 */

/* Old comments from RowColumn.c changes carried forward:
 * Revision 1.10.8.2  92/09/18  19:22:07  drand
 * 	CR #5050: Changed the focus code so that an internal routine
 * 	SetInputFocus is called in place of XSetInputFocus.  This allows us to
 * 	setup our own error handler and ignore the errors if the window has
 * 	been unmapped.  XSetInputFocus does the correct thing in this case and
 * 	ignoring the error appears to be the correct course of action.
 * 	[1992/09/18  19:19:50  drand]
 *
 *
 * Revision 1.10.4.13  1992/08/10  13:12:32  deblois
 * 	Added fix to handle focus problem CRs 4896 and 4912.
 * 	[1992/08/10  13:11:14  deblois]
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char *rcsid = "$TOG: RCMenu.c /main/25 1999/05/24 18:06:57 samborn $";
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include "XmI.h"
#include <X11/keysym.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/DisplayP.h>
#include <Xm/GadgetP.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuT.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScreenP.h>
#include <Xm/SeparatoGP.h>
#include <Xm/SeparatorP.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/TraitP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/VendorSP.h>
#include "GadgetUtiI.h"
#include "ManagerI.h"
#include "MapEventsI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "MessagesI.h"
#include "RCMenuI.h"
#include "RowColumnI.h"
#include "ScreenI.h"
#include "TearOffI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "VendorSI.h"

static void SwallowEventHandler(
				   Widget widget,
				   XtPointer client_data,
				   XEvent *event,
				   Boolean *continue_to_dispatch);
static void LocatePulldown(
                        XmRowColumnWidget root,
                        XmCascadeButtonWidget p,
                        XmRowColumnWidget m,
                        XEvent *event) ;
static int OnPostFromList(
                        XmRowColumnWidget menu,
                        Widget widget) ;
static XmRowColumnWidget FindMenu(
                        Widget w) ;
static void LotaMagic(
                        XmRowColumnWidget m,
                        RectObj child,
                        XmRowColumnWidget *parent_m,
                        Widget *w) ;
static int InMenu(
                        XmRowColumnWidget search_m,
                        XmRowColumnWidget *parent_m,
                        RectObj child,
                        Widget *w) ;
static Boolean SearchMenu(
                        XmRowColumnWidget search_m,
                        XmRowColumnWidget *parent_m,
                        RectObj child,
                        Widget *w,
                        Boolean setHistory) ;
static void PrepareToCascade(
                        XmRowColumnWidget submenu,
                        Widget cb,
                        XEvent *event) ;
static void GetLastSelectToplevel(
                        XmRowColumnWidget submenu) ;
static Boolean ShouldDispatchFocusOut(
                        Widget widget) ;
static int MenuStatus(
                        Widget wid );
static int  MenuType( Widget ) ;
static void PositionMenu(
                        register XmRowColumnWidget m,
                        XButtonPressedEvent *event) ;
static void ButtonMenuPopDown(
                        Widget w,
                        XEvent *event,
                        Boolean *popped_up) ;
static void MenuArm(
                        Widget w) ;
static void MenuDisarm(
                        Widget w) ;
static void TearOffArm(
                        Widget w) ;
static void MenuBarCleanup(
                        XmRowColumnWidget rc) ;
static void InvalidateOldFocus(
                        Widget oldWidget,
                        Window **poldFocus,
                        XEvent *event) ;
static void ProcessMenuTree(
                        XmRowColumnWidget w,
                        int mode) ;
static void AddToKeyboardList(
                        Widget w,
                        char *kbdEventStr,
                        Boolean needGrab,
                        Boolean isMnemonic) ;
static void _AddToKeyboardList(
                        Widget w,
                        unsigned int eventType,
                        KeySym keysym,
                        unsigned int modifiers,
                        Boolean needGrab,
                        Boolean isMnemonic) ;
static void AddKeycodeToKeyboardList(
                        Widget w,
                        unsigned int eventType,
                        KeyCode keycode,
                        KeySym keysym,
                        unsigned int modifiers,
                        Boolean needGrab,
                        Boolean isMnemonic) ;
static void RemoveFromKeyboardList(
                        Widget w) ;
static void GrabKeyOnAssocWidgets(
                        XmRowColumnWidget rowcol,
                        KeyCode detail,
                        unsigned int modifiers) ;
static void UngrabKeyOnAssocWidgets(
                        XmRowColumnWidget rowcol,
                        KeyCode detail,
                        unsigned int modifiers) ;
static Boolean InSharedMenupaneHierarchy(
                        XmRowColumnWidget m) ;
static void SetCascadeField(
                        XmRowColumnWidget m,
                        Widget cascadeBtn,
                        Boolean attach) ;
static void BtnDownInRowColumn(
                        Widget rc,
                        XEvent *event,
                        Position x_root,
                        Position y_root) ;
static void CheckUnpostAndReplay(
                        Widget rc,
                        XEvent *event) ;
static Boolean VerifyMenuButton(
                        Widget w,
                        XEvent *event );
static Boolean UpdateMenuHistory(
                        XmRowColumnWidget menu,
                        Widget child,
                        Boolean updateOnMemWidgetMatch) ;
static void RadioBehaviorAndMenuHistory(
                        XmRowColumnWidget m,
                        Widget w) ;
static void ChildsActivateCallback(
                        XmRowColumnWidget rowcol,
                        Widget child,
                        XtPointer call_value) ;
static void EntryFired(
                        Widget w,
                        XtPointer client_data,
                        XmAnyCallbackStruct *callback) ;
static int NoTogglesOn(
                        XmRowColumnWidget m) ;
static int IsInWidgetList(
                        register XmRowColumnWidget m,
                        RectObj w) ;
static void AllOffExcept(
                        XmRowColumnWidget m,
                        Widget w) ;
static void MenuShellPopdown( Widget w, XEvent *e) ;
static Boolean MenuSystemPopdown( Widget w, XEvent *e ) ;
static Boolean MenuSystemButtonPopdown( Widget w, XEvent *e ) ;
static void MenuChildFocus(Widget w) ;
static Widget MenuPopupPosted(Widget w) ;
static void DismissTearOffSubMenu(XmRowColumnWidget menu);

/*
 * Define trait records
 */

XmMenuSystemTraitRec _XmRC_menuSystemRecord = {
  0,			/* version */
  MenuType,
  MenuStatus,
  (XmMenuSystemCascadeProc)		PrepareToCascade,
  (XmMenuSystemVerifyProc)		VerifyMenuButton,
  (XmMenuSystemControlTraversalProc)	_XmSetMenuTraversal,
  (XmMenuSystemMenuBarCleanupProc)	MenuBarCleanup,
  (XmMenuSystemPopdownProc)		MenuSystemPopdown,
  (XmMenuSystemPopdownProc)		MenuSystemButtonPopdown,
  _XmRestoreExcludedTearOffToToplevelShell,
  _XmRestoreTearOffToMenuShell,
  MenuArm,
  MenuDisarm,
  TearOffArm,
  (XmMenuSystemEntryCallbackProc)	ChildsActivateCallback,
  (XmMenuSystemUpdateHistoryProc)	UpdateMenuHistory,
  (XmMenuSystemGetPostedFromWidgetProc)	GetLastSelectToplevel,
  (XmMenuSystemPositionProc)		PositionMenu,
  (XmMenuSystemUpdateBindingsProc)	_XmRC_DoProcessMenuTree,
  (XmMenuSystemRecordPostFromWidgetProc)	SetCascadeField,
  MenuShellPopdown,
  MenuChildFocus,
  MenuPopupPosted
};

/*
 * this entry is set in label and label gadget's class field so that
 * all communication from the buttons to the RowColumn are done through
 * this entry and then revectored to the appropriate routine.
 */
void
_XmRCMenuProcedureEntry(
        int proc,
        Widget widget,
        ... )
{
        int flag ;
        XtPointer data ;
        XtPointer data2 ;
        va_list ap ;
   va_start( ap, widget) ;

   /* note! in general, the "flag" argument is rarely used (but is needed for
   ** backward compatibility)
   */
   switch (proc)
   {
     case XmMENU_CASCADING:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       PrepareToCascade((XmRowColumnWidget) data, widget, (XEvent *) data2);
       break;

     case XmMENU_POPDOWN:
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       _XmMenuPopDown (widget, (XEvent *) data, (Boolean *) data2);
       break;

     case XmMENU_SHELL_POPDOWN:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;

       MenuShellPopdown(widget, (XEvent *) data);
       break;

     case XmMENU_BUTTON:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       * ((Boolean *) data2) = VerifyMenuButton (widget, (XEvent *) data);
       break;

     case XmMENU_CALLBACK:
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       /* data points to the widget which was activated */
       ChildsActivateCallback ((XmRowColumnWidget) widget,
	  (Widget) data, (XtPointer) data2);
       break;

     case XmMENU_TRAVERSAL:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       /* data points to a boolean */
       _XmSetMenuTraversal(widget, (Boolean) flag);
       break;

     case XmMENU_SUBMENU:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       SetCascadeField ((XmRowColumnWidget) widget, (Widget) data,
	  (Boolean) flag);
       break;

     case XmMENU_PROCESS_TREE:		/* appears to be obsolete within Xm */
       _XmRC_DoProcessMenuTree (widget, XmREPLACE);
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       break;

     case XmMENU_ARM:			/* appears to be obsolete within Xm */
       MenuArm (widget);
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       break;

     case XmMENU_DISARM:

       MenuDisarm (widget);
       break;

     case XmMENU_BAR_CLEANUP:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;

       MenuBarCleanup ((XmRowColumnWidget) widget);
       break;

     case XmMENU_STATUS:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       * ((int *) data) = MenuStatus(widget);
       break;

     case XmMENU_MEMWIDGET_UPDATE:	/* appears to be obsolete within Xm */
       if (UpdateMenuHistory((XmRowColumnWidget)XtParent(widget), widget, True))
	 RC_MemWidget(XtParent(widget)) = widget;
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       break;

     case XmMENU_BUTTON_POPDOWN:	/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;
       ButtonMenuPopDown (widget, (XEvent *) data, (Boolean *) data2);
       break;

     case XmMENU_RESTORE_EXCLUDED_TEAROFF_TO_TOPLEVEL_SHELL: /* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;

       _XmRestoreExcludedTearOffToToplevelShell(widget, (XEvent *) data);
       break;

     case XmMENU_RESTORE_TEAROFF_TO_TOPLEVEL_SHELL: /* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       _XmRestoreTearOffToToplevelShell(widget, (XEvent *) data);
       break;

     case XmMENU_RESTORE_TEAROFF_TO_MENUSHELL:	/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;

       _XmRestoreTearOffToMenuShell(widget, (XEvent *) data);
       break;

     case XmMENU_GET_LAST_SELECT_TOPLEVEL: /* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;
       data2 = va_arg( ap, XtPointer) ;

       GetLastSelectToplevel((XmRowColumnWidget) widget);
       break;

     case XmMENU_TEAR_OFF_ARM:		/* appears to be obsolete within Xm */
       flag = va_arg( ap, int) ;
       data = va_arg( ap, XtPointer) ;

       TearOffArm(widget);
       break;

     default:
       break;
  }
va_end( ap) ;
}

static void
MenuShellPopdown( Widget w, XEvent *e)
{
  (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
     menu_shell_class.popdownEveryone))(w, e, NULL, NULL);
}

/*
 * Called by PrepareToCascade() and ChildsActivateCallback().
 * This will allow XmGetPostedFromWidget() to be called from map and activate
 * or entrycallback callbacks.
 */
static void
GetLastSelectToplevel(
        XmRowColumnWidget submenu )
{
   XmRowColumnWidget topLevel;
   XmMenuState mst = _XmGetMenuState((Widget)submenu);

   /* "_XmLastSelectToplevel" only set for accelerators via
    * KeyboardInputHandler().
    */
   if (IsPopup(submenu))
   {
      /* A popup's lastSelectToplevel is always itself!  The cascadeBtn
       * indicates the postedFromWidget.
       */
      if (mst->RC_LastSelectToplevel)
	 RC_CascadeBtn(submenu) = mst->RC_LastSelectToplevel;
   }
   else
   {
      if (mst->RC_LastSelectToplevel)
	 topLevel = (XmRowColumnWidget)mst->RC_LastSelectToplevel;
      else
      {
	 _XmGetActiveTopLevelMenu ((Widget) submenu, (Widget *) &topLevel);
	 /* If the active topLevel menu is torn, then use it's saved away
	  * lastSelectTopLevel from tear-time.
	  */
	 if (RC_TearOffActive(topLevel))
	    topLevel = (XmRowColumnWidget)
	       topLevel->row_column.tear_off_lastSelectToplevel;
      }

      submenu->row_column.lastSelectToplevel = (Widget) topLevel;
   }
}

/*
 * called by cascadebutton (or CBG) before cascading a menu.  The interface
 * is through the menuProcs handle.
 */
static void
PrepareToCascade(
        XmRowColumnWidget submenu,
        Widget cb,
        XEvent *event )
{
   RC_CascadeBtn(submenu) = cb;
   RC_PostButton(submenu) = RC_PostButton(XtParent(cb));
   RC_PostModifiers(submenu) = RC_PostModifiers(XtParent(cb));
   RC_PostEventType(submenu) = RC_PostEventType(XtParent(cb));
   RC_PopupPosted(XtParent(cb)) = XtParent(submenu);

   if (IsOption(XtParent(cb)))
       RC_MemWidget(submenu) = RC_MemWidget(XtParent(cb));

   PositionMenu (submenu, (XButtonPressedEvent *) event);

   /*
    * Set submenu's lastselectToplevel in case map callback needs info
    */
   GetLastSelectToplevel(submenu);
}

static void
LocatePulldown(
        XmRowColumnWidget root,
        XmCascadeButtonWidget p,
        XmRowColumnWidget m,
        XEvent *event )
{
    XmScreen s;
    XmMonitorInfo *monitor;
    Position x, y, x1, y1, x2, y2;

    if (!root)
      return;

    x = y = 0;                  /* punt, don't know */

    if (IsOption (root))           /* near hot spot */
    {                              /* of option button (p) */
        if (! XtIsRealized( (Widget) m))
           XtRealizeWidget( (Widget) m);

	if (LayoutIsRtoLG(p))
	  x = RC_MemWidget(m) ?
	      (XtWidth(p) -
	       (XtWidth(RC_MemWidget(m)) + G_HighlightThickness(p) +
		2*MGR_ShadowThickness(m))) :
	      XtWidth(p) - G_HighlightThickness(p);
        else
	  x = RC_MemWidget(m) ?
	      (G_HighlightThickness(p) + MGR_ShadowThickness(m) -
	      XtX(RC_MemWidget(m))) : G_HighlightThickness(p);

        y = RC_MemWidget(m) ? (Half (XtHeight (p)) - (XtY(RC_MemWidget(m)) +
            Half(XtHeight(RC_MemWidget(m))))) : XtY(p);
    }

    else if (IsBar (root))
      {
	/* aligned with left edge of  cascade button, below cascade button.
	   If it is a vertical menu bar,  in which case position
	   off the left or right edge (depending on RtoL) */
	if (LayoutIsRtoLM(m))
	  {
	    /* aligned with right edge of cascade button */
	    if (! XtIsRealized ((Widget)m))  XtRealizeWidget ((Widget) m);
	    x = XtWidth(p) - XtWidth(m);
	    /* Vertical bar,  move it back */
	    if (! IsHorizontal(root)) x -= XtWidth(p);
	  } else {
	    if (IsHorizontal(root))
	      x = 0;
	    else
	      x = XtWidth(p);
	  }

	if (IsHorizontal(root))
	  y = XtHeight (p);
	else
	  y = 0;
      }

    else if (XmIsCascadeButtonGadget(p) && CBG_HasCascade(p))
    {
	/* gadget; have to use parent as base for coordinates */
        if (LayoutIsRtoLM(m))
           x = XtX(p) + CBG_Cascade_x(p) - XtWidth(m);
        else
	  x = XtX(p) + CBG_Cascade_x(p) + CBG_Cascade_width(p);
        y = XtY(p) + CBG_Cascade_y(p);

	/* cast only to eliminate warning */
	p = (XmCascadeButtonWidget)XtParent(p);

    }

    else if (XmIsCascadeButton(p) && CB_HasCascade(p))
    {
        if (LayoutIsRtoLM(m))
           x = CB_Cascade_x(p) - XtWidth(m);
        else
	  x = CB_Cascade_x(p) + CB_Cascade_width(p);
        y = CB_Cascade_y(p);
    }

    /*
     * Take these co-ords which are in the cascade button
     * widget's co-ord system and convert them to the root
     * window co-ords.
     */
    XtTranslateCoords((Widget)p, x, y, &x1, &y1);

    /**
     * Constrain ourselves to the physical screen, either where we want
     * to be (x1,y1) or where the button event happend; and absolute worst,
     * at (0,0).
     */
    s = XmScreenOfObject((Widget)p);
    if (!(monitor = XmGetMonitorInfoAt(s, x1, y1))) {
        if (!event || !(monitor = XmGetMonitorInfoAt(s, event->xbutton.x_root, event->xbutton.y_root))) {
            /* Well, it might be completely offscreen... */
            XtTranslateCoords((Widget)p, 0, 0, &x2, &y2);
            if (!(monitor = XmGetMonitorInfoAt(s, x2, y2)))
                monitor = XmGetMonitorInfoAt(s, 0, 0);
        }
    }

    if (x1 < 0) x1 = monitor->x + 4;
    if (x1 + XtWidth(m) >= monitor->width)
        x1 = monitor->x + monitor->width - XtWidth(m) - 4;

    if (y1 < 0) y1 = monitor->y + 4;
    if (y1 + XtHeight(m) >= monitor->height)
        y1 = monitor->y + monitor->height - XtHeight(m) - 4;

    XtX(m) = x1;
    XtY(m) = y1;
    FreeXmMonitorInfo(monitor);
}

/* These next two routines are provided to change and get the way popup and
 * option menus react to a posting ButtonClick.  The default (True) is for
 * the menupane to remain posted when a button release is received within
 * XtMultiClickTime of the posting button press.  Setting popupMenuClick to
 * False will cause these types of menus to always unpost on Button Release.
 * Note that these are _Xm functions for 1.2 only and will be replaced with
 * resource access in Motif 1.3!  Be forewarned!
 */
void
_XmSetPopupMenuClick(
        Widget wid,
	Boolean popupMenuClick)
{
   if (wid && XmIsRowColumn(wid))
      RC_popupMenuClick(wid) = popupMenuClick;
}

Boolean
_XmGetPopupMenuClick(
        Widget wid )
{
   if (wid && XmIsRowColumn(wid))
      return((Boolean)RC_popupMenuClick(wid));
   return(True);		/* If undefined, always return CUA */
}

/***************************************************************************
 *
 *
 * next section is action routines, these are called by the row column event
 * handler in response to events.  The handler parses its way through
 * the actions table calling these routines as directed.  It is these
 * routines which will typically decide the application should be
 * called via a callback.  This is where callback reasons are produced.
 *
 */
static XmRowColumnWidget
FindMenu(
        Widget w )
{
    if (XmIsRowColumn(w))
        return ((XmRowColumnWidget) w);        /* row column itself */
    else
        return ((XmRowColumnWidget) XtParent (w)); /* subwidget */
}



/*
 * popdown anything that should go away
 */
void
_XmMenuPopDown(
        Widget w,
        XEvent *event,
	Boolean *popped_up )
{
   XmRowColumnWidget rc = FindMenu(w);
   XmRowColumnWidget toplevel_menu;
   XmMenuShellWidget ms;
   Boolean posted;
   Time _time;

   _time = _XmGetDefaultTime(w, event);

   _XmGetActiveTopLevelMenu ((Widget) rc, (Widget *) &toplevel_menu);

   /* MenuShell's PopdownDone expects the leaf node submenu so that it can
    * determine if BSelect might have occured in one of its gadgets.  It
    * will popdown everything from the active top down.  Use RC_PopupPosted
    * for menubar to PopdownDone can get to a menushell.
    */
   if (IsBar(rc))
   {
      if (RC_PopupPosted(rc))
      {
	 (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	    menu_shell_class.popdownDone))((Widget) RC_PopupPosted(rc), event,
	    NULL, NULL);
      }
      else /* in PM mode - requires special attention */
	 {
            /* No submenus posted; must clean up ourselves */
            _XmMenuFocus( (Widget) rc, XmMENU_END, _time);
            XtUngrabPointer( (Widget) rc, _time);

            MenuBarCleanup(rc);
            _XmSetInDragMode((Widget) rc, False);
 	    MenuDisarm((Widget)rc);
	 }
   }
   else
   if (!XmIsMenuShell(XtParent(rc)))	/* torn! */
   {
      /* if popupPosted pop it down.
       * else restore state to inactive as if the menu was active... in case
       * the menu was really not active, _XmMenuFocus(), MenuDisarm(), and
       * XtUngrabPointer() hopefully return w/o doing anything.
       */
      if (RC_PopupPosted(rc))	/* if no popupPosted, nothing to do! */
      {
	 (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	    menu_shell_class.popdownDone))((Widget) RC_PopupPosted(rc), event,
	    NULL, NULL);
      }
      else
      {
         _XmMenuFocus(XtParent(rc), XmMENU_END, _time);
	 MenuDisarm ((Widget) toplevel_menu);
	 XtUngrabPointer(XtParent(rc), _time);
      }
   }
   else if (IsOption(toplevel_menu) &&
	    !XmIsRowColumn(w)
	    && (w != RC_MemWidget(rc)))
   {
      _XmMenuFocus(XtParent(rc), XmMENU_END, _time);
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	 menu_shell_class.popdownEveryone))((Widget) XtParent(rc), event,
	 NULL, NULL);
      MenuDisarm ((Widget) toplevel_menu);
      XtUngrabPointer(XtParent(rc), _time);
   }
   else
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	 menu_shell_class.popdownDone))((Widget) rc, event, NULL, NULL);

   if (IsPulldown(rc))
      ms = (XmMenuShellWidget)XtParent(rc);
   else if (IsPulldown(toplevel_menu) || IsPopup(toplevel_menu))
      ms = (XmMenuShellWidget)XtParent(toplevel_menu);
   else if (IsOption(toplevel_menu))
      ms = (XmMenuShellWidget)XtParent(RC_OptionSubMenu(toplevel_menu));
   else
      ms = NULL;

   if (ms && XmIsMenuShell(ms))	/* && !torn */
   {
      if ((posted = ms->shell.popped_up) != False)
         MenuDisarm((Widget) rc);
   }
   else
      posted = False;

   if (popped_up)
     *popped_up = posted;
}

/*
 * Swallow focus and crossing events while the menubar is active.  This
 * allows traversal to work in menubar while its submenus have the "real"
 * focus.
 */
static void
SwallowEventHandler(
	Widget widget,		/* unused */
	XtPointer client_data,	/* unused */
	XEvent *event,
	Boolean *continue_to_dispatch)
{
  /* while the Menubar is active don't allow any focus type events to the
   * shell
   */
  switch (event->type)
  {
     case EnterNotify:
     case LeaveNotify:
     case FocusOut:
     /* Allow FocusIn so when we XmProcessTraversal(menupane), the first item
      * gets the focus and highlight.
     case FocusIn:
      */
        *continue_to_dispatch = False;
  }
}

void
_XmSetSwallowEventHandler(
	Widget widget,
	Boolean add_handler )
{
   EventMask           eventMask;

   eventMask = EnterWindowMask | LeaveWindowMask | FocusChangeMask;
   if (add_handler)
      XtInsertEventHandler( _XmFindTopMostShell(widget), eventMask, False,
         SwallowEventHandler, NULL, XtListHead);
   else
      XtRemoveEventHandler(_XmFindTopMostShell(widget), eventMask, False,
         SwallowEventHandler, NULL);
}

/*
 * Action routines specific to traversal.
 */
void
_XmMenuUnmap(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   /*
    * Typically, when a cascade button is in a transient menupane, we just
    * want to ignore unmap requests.  However, when it is in a menubar, we
    * want it handled normally.
    */
   if (RC_Type(XtParent(cb)) == XmMENU_BAR)
      _XmPrimitiveUnmap( (Widget) cb, event, NULL, NULL);
}

void
_XmMenuFocusOut(
        Widget cb,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
    /*
     * This ugly piece of code has once again reared its head.  This is to
     * take care of a focus out on the cascade of a tear off menupane.
     */
    if (!ShouldDispatchFocusOut(cb))
       return;

    /* Forward the event for normal processing */
    _XmPrimitiveFocusOut( cb, event, NULL, NULL);
}



/*
 * The normal primitive FocusIn procedure does nothing if a transient
 * operation is happening.  Since we are part of the transient operation,
 * and we still want focus to work, we need to use the internal function.
 */
void
_XmMenuFocusIn(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   /* Forward the event for normal processing */
   _XmPrimitiveFocusInInternal( (Widget) cb, event, NULL, NULL);
}

/*
 * Class function which is used to clean up the menubar when it is leaving
 * the mode where the user has pressed F10 to start traversal in the
 * menubar.
 */
static void
MenuBarCleanup(
        XmRowColumnWidget rc )
{
    _XmMenuSetInPMMode ((Widget)rc, False);
}

static int
SIF_ErrorHandler(Display *display, /* unused */
		 XErrorEvent* event)
{
  return 0;
}

static void
SetInputFocus(Display *display, Window focus, int revert_to, Time time)
{
  XErrorHandler old_Handler;

  /* Setup error proc and reset error flag */
  old_Handler = XSetErrorHandler((XErrorHandler) SIF_ErrorHandler);

  /* Set the input focus */
  XSetInputFocus(display, focus, revert_to, time);
  XSync(display, False);

  XSetErrorHandler(old_Handler);
}

void
_XmMenuFocus(
        Widget w,
        int operation,
        Time _time )
{
   XmMenuState mst = _XmGetMenuState((Widget)w);
   Window tmpWindow;
   int tmpRevert;

   if (_time == CurrentTime)
     _time = XtLastTimestampProcessed(XtDisplay(w));

   switch (operation)
      {
	case XmMENU_END:
	  if (mst->RC_menuFocus.oldFocus != (Window) NULL)
	  {
	     /* Reset the focus when there's a valid window:
	      *  - the old focus window/widget is a shell and it's popped up
	      *  - the old focus window/widget is viewable (mapped)
	      * And the destroy callback hasn't invalidated oldFocus
	      */
	     if (mst->RC_menuFocus.oldWidget != 0)
	     {
		XtRemoveCallback(mst->RC_menuFocus.oldWidget,
				 XtNdestroyCallback,
				 (XtCallbackProc)InvalidateOldFocus,
				 (XtPointer) &mst->RC_menuFocus.oldFocus);

		/* oldWidget is not destroyed so the window is valid. */
		if (XtIsRealized(mst->RC_menuFocus.oldWidget))
		{
		    XWindowAttributes xwa ;

		   /* 99.9% of the time, oldWidget is a shell.  So we'll
		    * funnel everything through XGetWindowAttributes.  For
		    * non-shells we could just call _XmIsViewable().
		    */
		   XGetWindowAttributes( XtDisplay(mst->RC_menuFocus.
		      oldWidget), mst->RC_menuFocus.oldFocus, &xwa);
		   if (xwa.map_state == IsViewable)
/** old code with fix for 5715
		   if (!XtIsShell(mst->RC_menuFocus.oldWidget) ||
		       XtIsApplicationShell(mst->RC_menuFocus.oldWidget) ||
		       (((ShellWidget)mst->RC_menuFocus.oldWidget)->
			  shell.popped_up))
**/
		     {
		       SetInputFocus(XtDisplay(w), mst->RC_menuFocus.oldFocus,
				     mst->RC_menuFocus.oldRevert,
				     mst->RC_menuFocus.oldTime);
		     }
		 }
	      }
 	     /*
 	      * Since the focus may have been taken from a window and not a widget,
 	      * set the focus back to that window if there is not associated widget.
 	      * This was added to correct focus problems in CR 4896 and 4912.
 	      */
 	     else
 	       {
 		  SetInputFocus(XtDisplay(w), mst->RC_menuFocus.oldFocus,
				mst->RC_menuFocus.oldRevert,
				mst->RC_menuFocus.oldTime);
 	       }
	     mst->RC_menuFocus.oldFocus = 0;
	     mst->RC_menuFocus.oldRevert = 0;
	     mst->RC_menuFocus.oldWidget = (Widget) NULL;
	  }
	  XtUngrabKeyboard(w, _time);
	  break;
	case XmMENU_BEGIN:

	  XGetInputFocus(XtDisplay(w), &mst->RC_menuFocus.oldFocus, &mst->
	     RC_menuFocus.oldRevert);
	  mst->RC_menuFocus.oldWidget = XtWindowToWidget(XtDisplay(w), mst->
	     RC_menuFocus.oldFocus);
	  mst->RC_menuFocus.oldTime = _time - 1;

	  SetInputFocus(XtDisplay(w), XtWindow(w), mst->RC_menuFocus.oldRevert,
			mst->RC_menuFocus.oldTime);

	  /*
	   * If unable to set focus, it means that some other application
	   * (hopefully the WM) has set the focus more recently; try to
	   * set focus with _time instead of _time - 1.  Otherwise,
	   * it's possible some other application (hopefully the WM)
	   * will set the focus soon - see XmMENU_MIDDLE comments.
	   */
	  XGetInputFocus(XtDisplay(w), &tmpWindow, &tmpRevert);
	  if (tmpWindow != XtWindow(w))
	  {
	    SetInputFocus(XtDisplay(w), XtWindow(w), tmpRevert, _time);

	    mst->RC_menuFocus.oldRevert = tmpRevert;
	    mst->RC_menuFocus.oldTime = _time;
	    if (tmpWindow != mst->RC_menuFocus.oldFocus)
	    {
	      mst->RC_menuFocus.oldFocus = tmpWindow;
	      mst->RC_menuFocus.oldWidget =
		XtWindowToWidget(XtDisplay(w), tmpWindow);
	    }
	  }

	  if (mst->RC_menuFocus.oldWidget != NULL)
	    XtAddCallback(mst->RC_menuFocus.oldWidget, XtNdestroyCallback,
	      (XtCallbackProc) InvalidateOldFocus,
	      (XtPointer) &mst-> RC_menuFocus.oldFocus);

	  XFlush(XtDisplay(w));

	  break;
	case XmMENU_MIDDLE:
	  SetInputFocus(XtDisplay(w), XtWindow(w),
			mst->RC_menuFocus.oldRevert,
			mst->RC_menuFocus.oldTime);

	  /*
	   * If unable to set focus, some other application has set
	   * focus more recently than time saved by our last success.
	   * Try _time (if later than oldTime), and update menuFocus
	   * structure appropriately.
	   */
	  XGetInputFocus(XtDisplay(w), &tmpWindow, &tmpRevert);
	  if ((tmpWindow != XtWindow(w)) &&
	      (_time > mst->RC_menuFocus.oldTime))
	  {
	    SetInputFocus(XtDisplay(w), XtWindow(w), tmpRevert, _time);

	    mst->RC_menuFocus.oldRevert = tmpRevert;
	    mst->RC_menuFocus.oldTime = _time;
	    if (tmpWindow != mst->RC_menuFocus.oldFocus)
	    {
	      if (mst->RC_menuFocus.oldFocus != (Window) NULL &&
		  mst->RC_menuFocus.oldWidget != NULL)
		XtRemoveCallback(mst->RC_menuFocus.oldWidget,
				 XtNdestroyCallback,
				 (XtCallbackProc)InvalidateOldFocus,
				 (XtPointer) &mst->RC_menuFocus.oldFocus);

	      mst->RC_menuFocus.oldFocus = tmpWindow;
	      mst->RC_menuFocus.oldWidget =
		XtWindowToWidget(XtDisplay(w), tmpWindow);

	      if (mst->RC_menuFocus.oldWidget != NULL)
		XtAddCallback(mst->RC_menuFocus.oldWidget, XtNdestroyCallback,
			      (XtCallbackProc) InvalidateOldFocus,
			      (XtPointer) &mst->RC_menuFocus.oldFocus);
	    }
	  }
	  break;
      }
}


/*
 * Class function which is invoked when the post accelerator is received
 * for a popup menu or the menubar, or the post mnemonic is received for
 * an option menu.
 */
void
_XmRCArmAndActivate(
        Widget w,
        XEvent *event,
        String *parms,
        Cardinal *num_parms )
{
   XmRowColumnWidget m = (XmRowColumnWidget) w ;
   int i;
   XmCascadeButtonWidget child = NULL;
   XmMenuState mst = _XmGetMenuState((Widget)w);
   Time _time;

   _time = _XmGetDefaultTime(w, event);

   if (IsPopup(m))
   {
      if (RC_TornOff(m) && !XmIsMenuShell(XtParent(m)))
	 _XmRestoreTearOffToMenuShell((Widget)m, event);

      if (!XtIsManaged((Widget)m))
      {
	 Position x, y;

	 /* the posted from widget is saved in RC_CascadeBtn */
	 if (mst->RC_LastSelectToplevel)
	    RC_CascadeBtn(m) = mst->RC_LastSelectToplevel;
	 else
	    RC_CascadeBtn(m) = XtParent(XtParent(m));	/* help! help! */

	 /* Position & post menupane; then enable traversal */
	 RC_SetWidgetMoved(m, True);

/*_XmLastSelectToplevel */

	 /* Position the pane off of the last selected toplevel, or if there
	  * isn't one, off of its parent (help!).
	  * Place it in the upper left corner.
	  */

	 if (mst->RC_LastSelectToplevel)
	     XtTranslateCoords(mst->RC_LastSelectToplevel, 0, 0, &x, &y);
	 else
	     XtTranslateCoords(XtParent(XtParent(m)), 0, 0, &x, &y);

	 XtX(m) = x;
	 XtY(m) = y;

	 /* Verify popup for MenuShell's manage_set_changed() */
	 mst->RC_ButtonEventStatus.time = event->xkey.time;
	 mst->RC_ButtonEventStatus.verified = True;
	 mst->RC_ButtonEventStatus.event = *((XButtonEvent *)event);

	 XtManageChild( (Widget) m);
	 _XmSetInDragMode((Widget) m, False);
         XmProcessTraversal((Widget) m, XmTRAVERSE_CURRENT);
      }
      else
      {
         /* Let the menushell widget clean things up */
         (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
            menu_shell_class.popdownDone))(XtParent(m), event, NULL, NULL);
      }
   }
   else if (IsOption(m))
   {
      XmGadget g = (XmGadget) XmOptionButtonGadget( (Widget) m);

      /* Let the cascade button gadget do the work */
      (*(((XmGadgetClassRec *)XtClass(g))->gadget_class.
          arm_and_activate)) ((Widget) g, event, parms, num_parms);
   }
   else if (IsBar(m))
   {
      if (RC_IsArmed(m))
      {
	 _XmMenuPopDown((Widget) m, event, NULL);
      }
      else
      {
         _XmMenuSetInPMMode ((Widget)m, True);

	 /* traversal must be on for children to be traversable */
         m->manager.traversal_on = True;

	 /* First look for a non-Help menu child.  If this fails
	    we'll use the help menu */
         for (i = 0; i < m->composite.num_children; i++)
         {
	    child = (XmCascadeButtonWidget)m->composite.children[i];

            if (!IsHelp(m, (Widget)child) &&
	        XmIsTraversable( (Widget) child))
               break;
         }

	 /* See if we found one */
	 if (i >= m->composite.num_children)
	   {
	     /* If we haven't,  and there's no help menu,  then fail */
	     if (!(RC_HelpPb (m) &&
		   XmIsTraversable( (Widget) RC_HelpPb (m)))) {
	       m->manager.traversal_on = False;
	       return;
	     } else
	       /* But use the help menu if it exists */
	       child = (XmCascadeButtonWidget) RC_HelpPb(m);
	   }

         /* Don't post the menu if the menu cannot control grabs! */
          if (_XmMenuGrabKeyboardAndPointer((Widget)m, _time) != GrabSuccess)
          {
            return;
          }

	 _XmMenuFocus( (Widget) m, XmMENU_BEGIN, _time);
	 MenuArm((Widget) child);

	 RC_SetBeingArmed(m, False);

         /* To support menu replay, keep the pointer in sync mode.
	  * The grab freezes the pointer queue, unfreeze for this
	  * instance where a keyevent has caused the grab.
	  */
	 XAllowEvents(XtDisplay(m), SyncPointer, CurrentTime);

         _XmSetInDragMode((Widget) m, False);
      }
   }
   else if (IsPulldown(m))	/* Catch the Escape in a cascading menu! */
   {
      /* Let the menushell widget clean things up */
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
         menu_shell_class.popdownOne))(XtParent(m), event, NULL, NULL);
   }
}


/*
 * The following functions are used to manipulate lists of keyboard events
 * which are of interest to the menu system; i.e. accelerators and mnemonics.
 * The top level component within a menu system (the menubar, or the popup
 * menupane, or the option menu, or the work area menu) keeps a list of
 * the events it cares about.  Items are only added to the list when the
 * associated menu component has a complete path to the top level component.
 * Items are removed when the complete path is broken.
 *
 * The times at which a complete path may be formed is 1) When a button
 * in a menupane becomes managed, when a pulldown menupane is attached
 * to a cascading button, when the application sets the popupEnabled
 * resource to True, or when an option menu is created.  A complete path
 * may be broken when 1) a button in a menupane is unmanaged, a pulldown
 * menupane is detached from a cascading button, when the application clears
 * the popupEnabled resource or an option menu is destroyed.
 *
 * The event handler for catching keyboard events is added by the row column
 * widget during its initialize process, and removed when the row column
 * widget is destroyed.  Keyboard grabs, which are needed to make accelerators
 * work, are added either when the accelerator is registered, or when the
 * associated widget is realized; grabs cannot be added to a widget which
 * does not have a window!
 */


/*
 * This function is used both by the row column widget, and components which
 * are contained within a menu (toggle, pushbutton and cascadebutton).  The
 * row column widget uses it to process a tree of menu components; when a
 * menupane is linked to a cascade button, the new menupane, along with any
 * submenus cascading from it, will be processed.  The row column widget
 * will use the XmADD or XmDELETE mode to to this.  When a menu component
 * needs to change its accelerator or mnemonics, it will use the XmREPLACE
 * mode.
 *
 * When this function is used to delete a tree of keyboard actions, the
 * link between the menupane at the root (parameter 'w') and the cascade
 * button it is attached to must not yet have been broken.  This allows
 * the function to trace up the hierarchy and find the top level widget.
 */
void
_XmRC_DoProcessMenuTree(
        Widget w,
        int mode )          /* Add, Replace or Delete */
{
   /*
    * Depending upon the type of widget 'w' is, we may end up adding/deleting
    * keyboard actions for no widgets, the specified widget only, or the
    * specified widget and all others which cascade from it.
    */
   if (XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w))
   {
      /* If our parent is not a row column, then abort */
      if (XmIsRowColumn(XtParent(w)))
      {
         if (IsOption(XtParent(w)))
         {
            if (mode == XmREPLACE)
               return;

            /*
             * A cascade button in an option menu does not have an
             * accelerator or a mnemonic associated with it.  However,
             * its submenu may, so we need to process it.
             */
            if (XmIsCascadeButtonGadget(w))
                w = (Widget)CBG_Submenu(w);
         }
         else if (IsWorkArea(XtParent(w)))
         {
            /*
             * Since work area menus do not support cascade buttons
             * with submenus, we will treat this like a pushbutton.
             */
            if (mode == XmREPLACE)
            {
               /* Remove old one, if it exists */
               _XmRC_ProcessSingleWidget(w, XmDELETE);
               mode = XmADD;
            }

            _XmRC_ProcessSingleWidget(w, mode);
            return;
         }
         else if (IsBar(XtParent(w)) ||
                  IsPopup(XtParent(w)) ||
                  IsPulldown(XtParent(w)))
         {
            if (mode == XmREPLACE)
            {
               /* When replacing, don't worry about submenus */
               _XmRC_ProcessSingleWidget(w, XmDELETE);
               _XmRC_ProcessSingleWidget(w, XmADD);
               return;
            }

            /*
             * If we are in a menubar, a popup menupane or a pulldown
             * menupane, then we need to not only modify our keyboard
             * event, but also any which are defined in our submenu.
             */
            _XmRC_ProcessSingleWidget(w, mode);

            if (XmIsCascadeButtonGadget(w))
                w = (Widget)CBG_Submenu(w);
            else
                w = (Widget)CB_Submenu(w);
         }
      }
   }
   else if (XmeTraitGet((XtPointer) XtClass(w), XmQTmenuSavvy) != NULL &&
	    XtClass(w) != xmLabelWidgetClass &&
	    XtClass(w) != xmLabelGadgetClass)
   {
      if (mode == XmREPLACE)
      {
         /* Remove old one */
         _XmRC_ProcessSingleWidget(w, XmDELETE);
         mode = XmADD;
      }

      /*
       * In both of these cases, we need only modify the keyboard
       * event associated with this widget.
       */
      _XmRC_ProcessSingleWidget(w, mode);
      return;
   }
   else if (XmIsRowColumn(w))
   {
      /*
       * When the popupEnabled resource is enabled for a popup menupane,
       * we need to add the accelerator associated with the menu, followed
       * by all keyboard events associated with submenus.  The reverse
       * happens when the resource is disabled.
       *
       * When an option menu is created, we will add its posting mnemonic;
       * its submenu is taken care of when it is attached to the cascading
       * button.
       */
      if (IsPopup(w))
      {
         if (mode == XmREPLACE)
         {
            /* Don't worry about submenus during a replace operation */
            _XmRC_ProcessSingleWidget(w, XmDELETE);
            _XmRC_ProcessSingleWidget(w, XmADD);
            return;
         }

         _XmRC_ProcessSingleWidget(w, mode);
      }
      else if (IsOption(w) || IsBar(w))
      {
         if (mode == XmREPLACE)
         {
            _XmRC_ProcessSingleWidget(w, XmDELETE);
            mode = XmADD;
         }

         _XmRC_ProcessSingleWidget(w, mode);
         return;
      }
   }
   else
   {
      /* Unknown widget type; do nothing */
      return;
   }

   /* Process any submenus.
    * Don't call this if the widget is in the process of being destroyed
    *   since its children are already gone.
    * Don't process if mode is DELETE and it's in a sharedmenupanehierarchy
    *   to preverve accelerators and mnemonics.  If we allow check to slip
    *   through to RemoveFromKeyboardList, the mnemonics disappear.
    */
   if (w && !(w->core.being_destroyed) &&
       !((mode == XmDELETE) &&
	 InSharedMenupaneHierarchy((XmRowColumnWidget)w)))
   {
       ProcessMenuTree((XmRowColumnWidget)w, mode);
   }
}


/*
 * Given a row column widget, all keyboard events are processed
 * for the items within the row column widget, and then recursively
 * for any submenus cascading from this row column widget.
 */
static void
ProcessMenuTree(
        XmRowColumnWidget w,
        int mode )
{
   int i;
   Widget child;

   if (w == NULL)
      return;

   for (i = 0; i < w->composite.num_children; i++)
   {
      if (XtIsManaged((child = w->composite.children[i])))
      {
         _XmRC_ProcessSingleWidget(child, mode);

         if (XmIsCascadeButtonGadget(child))
         {
            ProcessMenuTree((XmRowColumnWidget) CBG_Submenu(child), mode);
         }
         else if (XmIsCascadeButton(child))
         {
            ProcessMenuTree((XmRowColumnWidget)
                             CB_Submenu( (XmCascadeButtonWidget) child), mode);
         }
      }
   }
}


/*
 * This function adds/deletes the mnemonic and/or accelerator associated
 * with the specified widget.  The work that is done is dependent both
 * on the widget in question, and sometimes the parent of the widget.
 *
 * When adding a keyboard event, we first check the component to see if
 * it has any keyboard events defined; if not, then nothing is done.  However,
 * when removing a keyboard event, we simply attempt to remove the entry for
 * the specified widget; we can't check to see if the widget had one defined,
 * because the instance structure may no longer contain the information.
 */
void
_XmRC_ProcessSingleWidget(
        Widget w,
        int mode )
{
   Arg args[2];
   Widget child;
   XmMenuSavvyTrait menuSavvy;

   menuSavvy = (XmMenuSavvyTrait)
     XmeTraitGet((XtPointer) XtClass(w), XmQTmenuSavvy);

   if (menuSavvy != NULL) {
     if (mode == XmADD)
       {
	 char *accelerator = NULL;
	 KeySym mnemonic = XK_VoidSymbol;

	 if (menuSavvy -> getAccelerator != NULL)
	   accelerator = menuSavvy -> getAccelerator(w);
	 if (menuSavvy -> getMnemonic != NULL)
	   mnemonic = menuSavvy -> getMnemonic(w);

	 /* These can have both an accelerator and a mnemonic */
	 if (mnemonic != XK_VoidSymbol &&
	     mnemonic != (KeySym) NULL) /* CR 5255 */
	   {
	     if ((XmIsCascadeButton(w) ||
		  XmIsCascadeButtonGadget(w)) &&
		 XmIsRowColumn(XtParent(w)) &&
		 IsBar(XtParent(w))) {
	       /* Add Alt-mnemonic for cascade buttons in
		  menu bars only */
	       _AddToKeyboardList (w, KeyRelease, mnemonic, Mod1Mask,
				   True, False);
	     }
	     _AddToKeyboardList (w, KeyRelease, mnemonic, 0,
				 False, True);
	   }

	 if (accelerator && (strlen(accelerator) > 0))
	   {
	     AddToKeyboardList(w, accelerator, True, False);
	   }
       }
     else
       RemoveFromKeyboardList(w);
   }
   else if (XmIsRowColumn(w)) {
     XmRowColumnWidget m = (XmRowColumnWidget) w;

      if (IsPopup(m) || IsBar(m))
	{
	  /*
	   * Popup Menus and the menubar may have an accelerator associated
	   * with them
	   */
	  if (mode == XmADD)
	    {
	      if (RC_MenuAccelerator(m) && (strlen(RC_MenuAccelerator(m)) > 0))
		{
		  AddToKeyboardList(w, RC_MenuAccelerator(m), True, False);
		}
	    }
	  else
            RemoveFromKeyboardList(w);
	}
      else if (IsOption(m))
	{
	 child = XmOptionLabelGadget( (Widget) m);
         /* Option menus may have a mnemonics associated with them */
         if (mode == XmADD)
         {
            if (RC_Mnemonic(m) != XK_VoidSymbol)
	    {
	       _AddToKeyboardList (w, KeyRelease, RC_Mnemonic(m), Mod1Mask,
				   True, True);
	       if (child)
	       {
		  XtSetArg(args[0], XmNmnemonic, RC_Mnemonic(m));
		  XtSetValues(child, args, 1);
	       }
	     }
         }
         else
         {
            RemoveFromKeyboardList(w);

            /* Tell the label gadget */
            if (child &&
		!child->core.being_destroyed)
            {
               XtSetArg(args[0], XmNmnemonic, '\0');
	       XtSetValues(child, args, 1);
            }
         }
      }
   }
}


/*
 * This function actually does the work of converting the accelerator
 * or mnemonic string into a workable format, and registering the keyboard
 * grab, if possible.
 */
static void
AddToKeyboardList(
        Widget w,
        char *kbdEventStr,
        Boolean needGrab,
        Boolean isMnemonic )
{
  int          *eventTypes;
  KeySym       *keysyms;
  unsigned int *modifiers;
  int num_keys = 0;
  int count, i;
  XmKeyBinding keys;

  if (kbdEventStr == NULL)
    return;

  count = _XmMapKeyEvents(kbdEventStr, &eventTypes, &keysyms, &modifiers);
  for (i = 0; i < count; i++)
    {
      /* If it is a virtual key, get the physical keys and modifiers */
      num_keys = XmeVirtualToActualKeysyms(XtDisplay(w), keysyms[i], &keys);
      while (--num_keys >= 0)
	if (keys[num_keys].keysym != NoSymbol)
	  {
	    /* Grab any modifiers in the virtual key binding too. */
	    _AddToKeyboardList(w, eventTypes[i], keys[num_keys].keysym,
			       modifiers[i] | keys[num_keys].modifiers,
			       needGrab, isMnemonic);
	    keysyms[i] = NoSymbol;
	  }
      XtFree((char*) keys);

      /* If not a virtual key process normally. */
      if (keysyms[i] != NoSymbol)
	_AddToKeyboardList (w, eventTypes[i], keysyms[i], modifiers[i],
			    needGrab, isMnemonic);
    }

  XtFree((char *) eventTypes);
  XtFree((char *) keysyms);
  XtFree((char *) modifiers);
}

/*
 * This is a lower level interface to AddToKeyboardList which avoids the
 * overhead of passing in a string.
 */
static void
_AddToKeyboardList(
        Widget w,
        unsigned int eventType,
        KeySym keysym,
        unsigned int modifiers,
        Boolean needGrab,
        Boolean isMnemonic )
{
   KeyCode keycode = 1;  /* Keycodes lie in the range 8 - 255 */
   int i;

   /*
    * if needGrab is FALSE, delay the call to XKeysymToKeycode until the
    * first time the value of the keycode is needed.
    */
   if (needGrab)
   {
      /* Convert keysym to keycode; needed by X grab call */
      if ((keycode = XKeysymToKeycode(XtDisplay(w), keysym)) == 0)
      {
	  XmeWarning( (Widget)w, BadMnemonicCharMsg);
	  return;
      }
   }

  if( !isMnemonic )
  {
    AddKeycodeToKeyboardList(w, eventType, keycode, keysym,
                             modifiers, needGrab, isMnemonic);
  }
  else
  {
    Display *display = XtDisplay(w);
    KeySym *ks_tbl;
    KeyCode min_kc_rtn;
    int syms_per;
    int min_kc, max_kc, num_kc, num_ks;
    KeySym uc, lc;

    XDisplayKeycodes(display, &min_kc, &max_kc);
    num_kc = (max_kc - min_kc) + 1;

    ks_tbl = XtGetKeysymTable(display, &min_kc_rtn, &syms_per);

    num_ks = num_kc * syms_per;

    for (i = 0; i < num_ks; i+=syms_per)
    {
       XtConvertCase( display, ks_tbl[i], &lc, &uc) ;

       if ((ks_tbl[i+1] == NoSymbol) || (ks_tbl[i+1] == uc))
       {
          if (keysym == lc || keysym == uc)
             AddKeycodeToKeyboardList(w, eventType,
                                      (KeyCode)(min_kc_rtn+(i/syms_per)),
                                      keysym, modifiers, needGrab, isMnemonic);
       }
       else
       {
          if (keysym == ks_tbl[i])
             AddKeycodeToKeyboardList(w, eventType,
                                      (KeyCode)(min_kc_rtn+(i/syms_per)),
                                      keysym, modifiers, needGrab, isMnemonic);
       }
    }
  }
}


/*
 * This is a lower level interface to AddToKeyboardList which avoids the
 * overhead of passing in a string.
 */
static void
AddKeycodeToKeyboardList(
        Widget w,
        unsigned int eventType,
        KeyCode keycode,
        KeySym keysym,
        unsigned int modifiers,
        Boolean needGrab,
        Boolean isMnemonic )
{
   Widget rowcol;
   XmKeyboardData * list;
   int i;

   if (XmIsRowColumn(w))
       rowcol = w;
   else
       rowcol = XtParent(w);

   /* Add to the list of keyboard entries */
   if (MGR_NumKeyboardEntries(rowcol) >= MGR_SizeKeyboardList(rowcol))
   {
      /* Grow list */
      MGR_SizeKeyboardList(rowcol) += 10;
      MGR_KeyboardList(rowcol) =
	     (XmKeyboardData *)XtRealloc( (char *) MGR_KeyboardList(rowcol),
		  (MGR_SizeKeyboardList(rowcol) * sizeof(XmKeyboardData)));
   }

   list = MGR_KeyboardList(rowcol);
   i = MGR_NumKeyboardEntries(rowcol);
   list[i].eventType = eventType;
   list[i].keysym = keysym;
   list[i].key = keycode;
   list[i].modifiers = isMnemonic ?
      (modifiers &  ~(ShiftMask | LockMask)) : modifiers;
   list[i].component = w;
   list[i].needGrab = needGrab;
   list[i].isMnemonic = isMnemonic;
   MGR_NumKeyboardEntries(rowcol)++;

   if (needGrab)
   {
      GrabKeyOnAssocWidgets ((XmRowColumnWidget) rowcol, keycode, modifiers);
   }
}

/*
 * This function removes all keyboard entries associated with a particular
 * component within a row column widget.
 * If the component is a push or toggle button, the keyboard_list entry
 * must be removed so that CheckKey() doesn't catch the old key events
 * if this is a replacement!  If the menu-item has a grab installed,
 * then we can only ungrab if the parent pane is not shared.  The same
 * applies for (shareable) Popups.  Menubar and Option accelerators aren't
 * shared and can always be ungrabbed.
 */
static void
RemoveFromKeyboardList(
        Widget w )
{
   XmRowColumnWidget rowcol;
   XmKeyboardData * klist;
   int count;
   int i, j;
   Boolean notInSharedMenupaneHierarchy;

   if (XmIsRowColumn(w))
       rowcol = (XmRowColumnWidget)w;
   else
       rowcol = (XmRowColumnWidget)XtParent(w);

   if (IsWorkArea(rowcol))
      return;

   notInSharedMenupaneHierarchy = !InSharedMenupaneHierarchy(rowcol);

   klist = MGR_KeyboardList(rowcol);
   count = MGR_NumKeyboardEntries(rowcol);

   for (i = 0; i < count; )
   {
      if (klist[i].component == w)
      {
	 /* NOTE that the ungrabs on shared menupane associate widgets are not
          * done and thus causes extra event deliveries and possible memory
          * leaks.  The problem is that it is difficult to tell whether an
          * item should really be ungrabbed since the sharing of menupanes
          * could mean that this item exists somewhere else on this hierarchy.
	  */
         if (klist[i].needGrab &&
	     (w->core.being_destroyed || notInSharedMenupaneHierarchy)) {
	   Boolean another = False;
	   int k;

	   /* CR 7259,  don't ungrab if another entry requires the
	      grab on the same key/modifier */
	   for(k = 0; k < count; k++) {
	     if (k != i &&
		 klist[k].needGrab &&
		 klist[k].key == klist[i].key &&
		 klist[k].modifiers == klist[i].modifiers) {
	       another = True;
	       break;
	     }
	   }

	   if (! another)
	     UngrabKeyOnAssocWidgets(rowcol, klist[i].key, klist[i].modifiers);
	 }

         /* Move the rest of the entries up 1 slot */
         for (j = i; j < count -1; j++)
	     klist[j] = klist[j+1];

         MGR_NumKeyboardEntries(rowcol) = MGR_NumKeyboardEntries(rowcol)-1;
         count--;
      }
      else
         i++;
   }
}

/*
 * search the postFromList and return the index of the found widget.  If it
 * is not found, return -1
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

/*
 * Useful for MenuBars and Option Menus to determine where to set up the
 * event handlers and grabs.
 */
void
_XmRCGetTopManager(
        Widget w,
        Widget *topManager )
{
   while (XmIsManager(XtParent(w)))
       w = XtParent(w);

   * topManager = w;
}

/*
 * Returns the toplevel menu widget in an acive menu hierarchy.
 *
 * This function is only useful when the menu system is active.  That is
 * the only time that the CascadeBtn field in the RowColumn in guaranteed
 * to be valid.
 */
void
_XmGetActiveTopLevelMenu(
        Widget wid,
        Widget *rwid )
{
   XmRowColumnWidget w = (XmRowColumnWidget) wid ;
   XmRowColumnWidget *topLevel = (XmRowColumnWidget *) rwid ;

   /*
    * find toplevel by following up the chain. Popups use CascadeBtn to
    * keep the active widget in the postFromList.  Stop at tear off!
    */
   while (RC_CascadeBtn(w) && (!IsPopup(w)) && XmIsMenuShell(XtParent(w)))
       w = (XmRowColumnWidget) XtParent(RC_CascadeBtn(w));

   * topLevel = w;
}

static void
GrabKeyWithLockMask (
        Widget widget,
        KeyCode keycode,
        Modifiers modifiers,
        Boolean owner_events,
        int pointer_mode,
        int keyboard_mode )
{
    /* Make sure the modifiers mask is known */
    _XmCheckInitModifiers();

    /* And grab all the key with all combinations of modifiers */
    XtGrabKey(widget, keycode, modifiers,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|LockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|ScrollLockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|NumLockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|LockMask|ScrollLockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|LockMask|NumLockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|ScrollLockMask|NumLockMask,
            owner_events, pointer_mode, keyboard_mode );
    XtGrabKey(widget, keycode, modifiers|LockMask|ScrollLockMask|NumLockMask,
            owner_events, pointer_mode, keyboard_mode );
}

static void
UngrabKeyWithLockMask (
        Widget widget,
        KeyCode keycode,
        Modifiers modifiers)
{
    /* Make sure the modifiers are known */
    _XmCheckInitModifiers();

    /* And ungrab all combinations of known modifiers */
    XtUngrabKey(widget, keycode, modifiers);
    XtUngrabKey(widget, keycode, modifiers|LockMask);
    XtUngrabKey(widget, keycode, modifiers|ScrollLockMask);
    XtUngrabKey(widget, keycode, modifiers|NumLockMask);
    XtUngrabKey(widget, keycode, modifiers|LockMask|ScrollLockMask);
    XtUngrabKey(widget, keycode, modifiers|LockMask|NumLockMask);
    XtUngrabKey(widget, keycode, modifiers|ScrollLockMask|NumLockMask);
    XtUngrabKey(widget, keycode, modifiers|LockMask|ScrollLockMask|NumLockMask);
}

/*
 * set up the grabs on the appropriate assoc widgets.  For a popup, this
 * is all of the widgets on the postFromList.  For a menubar and option
 * menu, this is the top manager widget in their hierarchy.  For a
 * pulldown, the assoc widgets can only be determined by following the
 * chain up the postFromList.
 */
static void
GrabKeyOnAssocWidgets(
        XmRowColumnWidget rowcol,
        KeyCode detail,
        unsigned int modifiers )
{
   Widget topManager;
   int i;

   if (IsPopup(rowcol))
   {
      for (i=0; i < rowcol->row_column.postFromCount; i++)
         GrabKeyWithLockMask (rowcol->row_column.postFromList[i], detail, modifiers,
            False, GrabModeAsync, GrabModeAsync);
   }
   else if (IsBar(rowcol) || IsOption(rowcol))
   {
      _XmRCGetTopManager ((Widget) rowcol, &topManager);
      GrabKeyWithLockMask (topManager, detail, modifiers, False,
	 GrabModeAsync, GrabModeAsync);
   }
   else if (IsPulldown(rowcol))
   {
      for (i=0; i<rowcol->row_column.postFromCount; i++)
         GrabKeyOnAssocWidgets ((XmRowColumnWidget)
            XtParent(rowcol->row_column.postFromList[i]), detail, modifiers);
   }
}

/*
 */
static void
UngrabKeyOnAssocWidgets(
        XmRowColumnWidget rowcol,
        KeyCode detail,
        unsigned int modifiers )
{
   Widget assocWidget;
   int i;

   if (IsPopup(rowcol))
   {
      for (i=0; i < rowcol->row_column.postFromCount; i++)
      {
	 assocWidget = rowcol->row_column.postFromList[i];
	 if (!assocWidget->core.being_destroyed)
	    UngrabKeyWithLockMask (assocWidget, detail, modifiers);
      }
   }
   else if (IsBar(rowcol) || IsOption(rowcol))
   {
      _XmRCGetTopManager ((Widget) rowcol, &assocWidget);
      if (!assocWidget->core.being_destroyed)
         UngrabKeyWithLockMask (assocWidget, detail, modifiers);
   }
   else if (IsPulldown(rowcol))
   {
      for (i=0; i<rowcol->row_column.postFromCount; i++)
         UngrabKeyOnAssocWidgets ((XmRowColumnWidget)
            XtParent(rowcol->row_column.postFromList[i]), detail, modifiers);
   }
}


void
_XmRC_AddToPostFromList(
        XmRowColumnWidget m,
        Widget widget )
{

   if (m->row_column.postFromListSize == m->row_column.postFromCount)
   {
      /* increase the size to fit the new one and one more */
      m->row_column.postFromListSize += 2;
      m->row_column.postFromList = (Widget *)
	  XtRealloc ( (char *) m->row_column.postFromList,
		     m->row_column.postFromListSize * sizeof(Widget));
   }

   m->row_column.postFromList[m->row_column.postFromCount++] = widget;

   /* If the popup's attach widget mysteriously is destroyed, remove the
    * attach widget from the popup's postFromList.  (Note the cascade
    * button already does this from its Destroy method).
    */
   if (IsPopup(m))
      XtAddCallback(widget, XtNdestroyCallback,
		    (XtCallbackProc)_XmRC_RemoveFromPostFromListOnDestroyCB,
		    (XtPointer)m);
}

void _XmRC_RemoveFromPostFromListOnDestroyCB(Widget w, XtPointer clientData, XtPointer callData)
{
   /* The attach_widget has been destroyed.  Remove it from the Popup's
    * postFromList.
    */
  _XmRC_RemoveFromPostFromList((XmRowColumnWidget)clientData, w);
}

void
_XmRC_RemoveFromPostFromList(
        XmRowColumnWidget m,
        Widget widget )
{
   int i;
   Boolean found = False;

   for (i=0; i < m->row_column.postFromCount; i++)
   {
      if (!found)
      {
	 if (widget == m->row_column.postFromList[i])
	 {
	    /* remove this entry */
	    found = True;
	 }
      }
      else
	  m->row_column.postFromList[i-1] = m->row_column.postFromList[i];
   }
   if (found)
     {
       m->row_column.postFromCount--;
       if (IsPopup(m))
 	 XtRemoveCallback(widget, XtNdestroyCallback,
			  (XtCallbackProc)_XmRC_RemoveFromPostFromListOnDestroyCB, (XtPointer)m);
     }
}


static Boolean
InSharedMenupaneHierarchy(
        XmRowColumnWidget m)
{
   while (m && XmIsRowColumn(m) && (IsPulldown(m) || IsPopup(m)))
   {
      if (m->row_column.postFromCount == 1)
	 m = (XmRowColumnWidget)XtParent(m->row_column.postFromList[0]);
      else
	 return(True);
   }

   return(False);
}

/* helper function for SetCascadeField; take down the tear-off menu [possibly
** shared], on the grounds that the application wishes the menu to be removed,
** which we deduce from the disconnection made
*/
static void DismissTearOffSubMenu (XmRowColumnWidget menu)
{
   int i;

   if ( (menu == NULL) ||
        !XmIsRowColumn(menu) ||
        !IsPulldown(menu) ||
        (menu->core.being_destroyed) )
       return;

   for (i = 0; i < menu->composite.num_children; i++)
   {
      Widget child = menu->composite.children[i];
      if (XmIsCascadeButtonGadget(child))
      {
         if ( CBG_Submenu(child) )
            DismissTearOffSubMenu((XmRowColumnWidget) CBG_Submenu(child));
      }
      else if (XmIsCascadeButton(child))
      {
         if ( CB_Submenu(child) )
            DismissTearOffSubMenu((XmRowColumnWidget) CB_Submenu(child));
      }
   }

   if (RC_TornOff(menu) && RC_TearOffActive(menu))
         _XmDismissTearOff(XtParent(menu), NULL, NULL);
}

/*
 * This is a class function exported by the RowColumn widget.  It is used
 * by the CascadeButton widget to signal that a menupane has either been
 * attached to a cascade button widget, or detached from a cascade button
 * widget.
 */
static void
SetCascadeField(
        XmRowColumnWidget m,
        Widget cascadeBtn,
        Boolean attach )
{
   int mode;

   if (attach)
   {
      mode = XmADD;

      /* if being attached to an option menu, set the option menus submenu */
      if (RC_Type(XtParent(cascadeBtn)) == XmMENU_OPTION)
	  RC_OptionSubMenu(XtParent(cascadeBtn)) = (Widget) m;

      if (XmIsMenuShell(XtParent(m)))
      {
	 XtX(XtParent(m)) = XtY(XtParent(m)) = 0;
      }

      if (m->row_column.postFromCount &&
	  (RC_TearOffModel(m) == XmTEAR_OFF_ENABLED))
         XmeWarning( (Widget)m, TearOffSharedMenupaneMsg);

      if (OnPostFromList (m, cascadeBtn) != -1)
	  /* already in the list, this means no work to do */
	  return;

      _XmRC_AddToPostFromList (m, cascadeBtn);
   }

   else
   {
      Boolean wasShared = InSharedMenupaneHierarchy(m);
      mode = XmDELETE;

      DismissTearOffSubMenu(m);
      _XmRC_RemoveFromPostFromList (m, cascadeBtn);

      /* if being removed from an option menu, set the option menus submenu */
      if (RC_Type(XtParent(cascadeBtn)) == XmMENU_OPTION)
	  RC_OptionSubMenu(XtParent(cascadeBtn)) = (Widget) NULL;

      /* CR 5550 fix begin */
      if (    ( m != NULL )
	  && ( RC_CascadeBtn(m) == (Widget)cascadeBtn )
	  )
	{
	  RC_CascadeBtn(m) = (Widget) NULL;
	}
      /* CR 5550 fix end */

      /* if we're in a shared menupane hierarchy, don't process (delete) the
       * accelerators and mnemonics!
       */
      if (wasShared)
	 return;
   }

   /* process the accelerators and mnemonics */
   _XmRC_DoProcessMenuTree( (Widget) m, mode);
}


/*
 * _XmMatchBSelectEvent() is intended to be used to check if the event is
 * a valid BSelect.  It will only validate the event if the menu hierarchy
 * is posted (so BSelect doesn't override whichButton to post a menu).
 */
Boolean
_XmMatchBSelectEvent(
	Widget wid,
	XEvent *event )
{
   XmRowColumnWidget rc;

   /* First make sure the menu hierarchy is posted */
   /* But if it's torn off then don't worry about posted */
   if (XmIsMenuShell(XtParent(wid)))
   {
      _XmGetActiveTopLevelMenu(wid, (Widget *)&rc);
      if ( (IsPopup(rc) &&
            !((XmMenuShellWidget)XtParent(rc))->shell.popped_up) ||
	   (!IsPopup(rc) && !RC_PopupPosted(rc)) )
	 return(False);
   }

   rc = (XmRowColumnWidget)wid;

   if ( !event )
      return (False);

   if ( !_XmMatchBtnEvent( event,  XmIGNORE_EVENTTYPE,
	  /*BSelect.button, BSelect.modifiers*/ Button1, AnyModifier ))
      return(False);

   return(True);
}

Boolean
_XmMatchBDragEvent(
	Widget wid,
	XEvent *event )
{
   XmRowColumnWidget rc;

   /* First make sure the menu hierarchy is posted */
   /* But if it's torn off then don't worry about posted */
   if (XmIsMenuShell(XtParent(wid)))
   {
      _XmGetActiveTopLevelMenu(wid, (Widget *)&rc);
      if ( (IsPopup(rc) &&
            !((XmMenuShellWidget)XtParent(rc))->shell.popped_up) ||
	   (!IsPopup(rc) && !RC_PopupPosted(rc)) )
	 return(False);
   }

   rc = (XmRowColumnWidget)wid;

   if ( !event )
      return (False);

   if ( !_XmMatchBtnEvent( event,  XmIGNORE_EVENTTYPE,
	  /*BDrag.button, BDrag.modifiers*/ Button2, 0 ))
      return(False);

   return(True);
}

static void
BtnDownInRowColumn(
        Widget rc,
        XEvent *event,
	Position x_root,
	Position y_root	   )
{
   XmGadget gadget;

   Position relativeX = event->xbutton.x_root - x_root;
   Position relativeY = event->xbutton.y_root - y_root;

   _XmSetMenuTraversal (rc, False);

   if ((gadget = (XmGadget) XmObjectAtPoint( rc, relativeX, relativeY)) != NULL)
   {
       /* event occured in a gadget w/i the rowcol */
       _XmDispatchGadgetInput( (Widget) gadget, event,
			      XmARM_EVENT);
   } else
      if (!XmIsMenuShell(XtParent(rc)))
      {
	 TearOffArm((Widget)rc);
      }

   /* For style guide conformance and consistency with Popup's and pulldown-
    * buttons, popdown cascading submenus below this point on button press
    * where individual buttons wouldn't be taking care of this.
    */
   if ((!gadget ||
	!XtIsSensitive((Widget)gadget) ||
	!(XmIsCascadeButtonGadget(gadget))) &&		/* !cbg kludge */
       (RC_PopupPosted(rc)))
   {
       (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	  menu_shell_class.popdownEveryone))(RC_PopupPosted(rc),NULL,
					     NULL, NULL);
   }

   /* BtnDown in the menu(bar) but outside of any cascade button or cascade
    * gadget must install grabs so events continue to be processed by the
    * menubar and are not erroneously passed to other widgets.
    */
   if (IsBar(rc) && !RC_IsArmed(rc) && !gadget)
   {
     Time _time = _XmGetDefaultTime(rc, event);
     Widget top_shell;

     /* Don't post the menu if the menu cannot control grabs! */
     if (_XmMenuGrabKeyboardAndPointer(rc, _time) != GrabSuccess)
     {
       _XmRecordEvent (event);
       return;
     }

     _XmMenuFocus((Widget) rc, XmMENU_BEGIN, _time);

     /* This nasty section of code is mostly to handle focus and highlighting.
      * The call to MenuArm() calls XmProcessTraversal() to move the focus to
      * the menubar.  We don't want the first cascade button to highlight.  So,
      * the menubar manage flag is turned off to circumvent the call to
      * XmProcessTraversal.  (This might be better served by sending in a flag
      * to MenuArm() that avoids the call completely - hint, hint).  Then we
      * do some focus hocus pocus to turn off focus so that the current focus
      * item becomes unhighlighted and NOT move the focus/highlight to the
      * menubar yet (See Traversal.c _XmMgrTraversal() for precedence).  The
      * next cascade button enter event will take care of that.
      */
     rc->core.managed = False;
     MenuArm((Widget) rc);
     rc->core.managed = True;
     top_shell = _XmFindTopMostShell(rc);
     _XmSetFocusFlag( top_shell, XmFOCUS_RESET, TRUE) ;
     XtSetKeyboardFocus( top_shell, None) ;
     _XmSetFocusFlag( top_shell, XmFOCUS_RESET, FALSE) ;

     _XmSetInDragMode((Widget) rc, True);
     RC_SetBeingArmed(rc, False);
   }

   _XmRecordEvent(event);
   XAllowEvents(XtDisplay(rc), SyncPointer, CurrentTime);
}

static void
CheckUnpostAndReplay(
        Widget rc,
        XEvent *event )
{
   XmMenuState mst = _XmGetMenuState((Widget)rc);

    if (_XmGetUnpostBehavior(rc) == XmUNPOST_AND_REPLAY)
    {
	_XmGetActiveTopLevelMenu(rc, &mst->RC_ReplayInfo.toplevel_menu);
	mst->RC_ReplayInfo.time = event->xbutton.time;

	/* do this before popdown since ptr is ungrabed there */
	XAllowEvents(XtDisplay(rc), ReplayPointer, CurrentTime);
	_XmMenuPopDown(rc, event, NULL);
    }
    else
    {
	_XmSetMenuTraversal (rc, False);
	_XmRecordEvent(event);
	XAllowEvents(XtDisplay(rc), SyncPointer, CurrentTime);
    }
}

void
_XmHandleMenuButtonPress(
        Widget wid,
        XEvent *event )
{
    Position x_root, y_root;

    XtTranslateCoords (wid, 0, 0, &x_root, &y_root);

    if ((event->xbutton.x_root >= x_root) &&
	(event->xbutton.x_root < x_root + wid->core.width) &&
	(event->xbutton.y_root >= y_root) &&
	(event->xbutton.y_root < y_root + wid->core.height))
    {
	/* happened in this rowcolumn */
	BtnDownInRowColumn (wid, event, x_root, y_root);
    }
    else if (RC_PopupPosted(wid))
    {
	/* follow down menu heierarchy */
	_XmHandleMenuButtonPress (((CompositeWidget) RC_PopupPosted(wid))->
				  composite.children[0], event);

    }
    else
    {
	/* nothing else posted */
	CheckUnpostAndReplay (wid, event);
	return;
    }
}


/*
 * Button Action Procs
 */
void
_XmMenuBtnDown(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   Position x_root, y_root;
   Widget topLevel;
   XmMenuState mst = _XmGetMenuState((Widget)wid);
   Time _time = _XmGetDefaultTime((Widget) wid, event);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(wid), XmQTmenuSystem);

    if (!_XmIsEventUnique(event))
	return;

    /* This code no longer checks the button for a match.  Any button
       outside the RowColumn will take down the menu */

   /* Overload _XmButtonEventStatus's time for MenuShell's
    * managed_set_changed routine to determine if an option menu is
    * trying to post using BSelect Click.  _XmButtonEventStatus's
    * verified is irrelevant.
    */
   if (IsOption(rc))
     {
       mst->RC_ButtonEventStatus.time = event->xbutton.time;
     }

   XtTranslateCoords ((Widget) rc, 0, 0, &x_root, &y_root);

   if (menuSTrait -> verifyButton((Widget) rc, event) &&
       (event->xbutton.x_root >= x_root) &&
       (event->xbutton.x_root < x_root + rc->core.width) &&
       (event->xbutton.y_root >= y_root) &&
       (event->xbutton.y_root < y_root + rc->core.height))
     {
       if (!XmIsMenuShell(XtParent(rc)) &&
	   RC_Type(rc) != XmMENU_BAR &&
	   RC_Type(rc) != XmMENU_OPTION)
	 XChangeActivePointerGrab(XtDisplay(rc),
				  ((unsigned int) (ButtonReleaseMask |
						   PointerMotionMask |
						   PointerMotionHintMask |
						   EnterWindowMask |
						   LeaveWindowMask)),
				  _XmGetMenuCursorByScreen(XtScreen(rc)),
				  _time);

       /* happened in this rowcolumn */
       BtnDownInRowColumn ((Widget) rc, event, x_root, y_root);
     }
   else
     {
       _XmGetActiveTopLevelMenu( (Widget) rc, &topLevel );
       if ((Widget) rc == topLevel)
	 {
	   /* already looked in the toplevel, move to a pulldown */
	   if (RC_PopupPosted(rc))
	     {
	       topLevel = ((CompositeWidget) RC_PopupPosted(rc))->
		 composite.children[0];
	     }

	   else
	     {
	       /* nothing else posted */
	       CheckUnpostAndReplay ((Widget) rc, event);
	       return;
	     }
	 }
       /* travel down menu to see if in another menu widget */
       _XmHandleMenuButtonPress (topLevel, event);
     }
}


void
_XmMenuBtnUp(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
   XmRowColumnWidget w = (XmRowColumnWidget) wid ;
   XmGadget gadget;
   Time _time = _XmGetDefaultTime(wid, event);
   XmMenuSystemTrait menuSTrait;
   Boolean nonButton;	/* that is, make a good guess about whether the widget
			** or gadget under the mouse-button release will do
			** anything itself to pop the menu down and to restore
			** the cursor to its correct shape -- buttons do, other
			** widgets we don't know about, but assume that labels
			** and separators are the most likely candidates
			*/

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(wid), XmQTmenuSystem);

   /* Support menu replay, free server input queue until next button event */
   XAllowEvents(XtDisplay(w), SyncPointer, CurrentTime);

   if (! _XmIsEventUnique(event) ||
       ! menuSTrait -> verifyButton(wid, event) ||
       (IsBar(w) && ! RC_IsArmed(w)))
      return;

   if (w->core.window == event->xbutton.window)
      gadget = (XmGadget) XmObjectAtPoint( (Widget) w, event->xbutton.x,
			       event->xbutton.y);
   else
      gadget = NULL;

   nonButton = ((IsPulldown(w) || IsPopup(w)) && !XmIsMenuShell(XtParent(w)) &&
       (!gadget || (XtClass(gadget) == xmLabelGadgetClass) ||
		   (XtClass(gadget) == xmSeparatorGadgetClass)));
   if ((gadget != NULL) && XtIsSensitive((Widget)gadget))
   {
      _XmDispatchGadgetInput( (Widget) gadget, event, XmACTIVATE_EVENT);
      if (nonButton)
	_XmMenuPopDown( (Widget) w, event, NULL );
   }
   else if (IsBar(w) || _XmIsTearOffShellDescendant((Widget)w))
   {
      /* Only drop in here when no other widget took the event */
      _XmMenuPopDown((Widget) w, event, NULL);
      if (IsBar(w))		/* Only necessary for truth and beauty */
         MenuBarCleanup(w);
      MenuDisarm((Widget) w);
      _XmMenuFocus( (Widget) w, XmMENU_END, _time);
      XtUngrabPointer( (Widget) w, _time);
   }
   _XmSetInDragMode((Widget) w, False);

   /* In a tear off, if the button up occured over a label or separator or
    * not in a button child, unhighlight it and reset the focus to the first
    * qualified item.
    */
   if (nonButton)
   {
        if (w->manager.active_child && XmIsGadget(w->manager.active_child))
            {
                if (  ((XmGadgetClass) XtClass(w->manager.active_child))->gadget_class.border_unhighlight)
                   (*(((XmGadgetClass) XtClass(w->manager.active_child))->gadget_class.border_unhighlight)) (w->manager.active_child);
            }
      _XmClearFocusPath((Widget)w);
      XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);
   }
}

/******************************************************************
 * _XmMenuGadgetDrag encapsulates ManagerGadgetDrag and
 * makes sure that the queue is unblocked and the menu is unposted
 * on btn2 events when the menu system is active.  If the menu
 * system is active, then it unposts the menu before continuing.
 ******************************************************************/
void
_XmMenuGadgetDrag(Widget wid, XEvent *event,
		  String *params, Cardinal *num_params)
{
   /* We need to make sure and ignore the replay if we
      are unposting the menu.  This can have bad timing
      effects in the drag & drop code */
   if (! _XmIsEventUnique(event)) return;

   /* If the menu is up then we take it down,  otherwise we
      start a drag */
   if (RC_IsArmed(wid)) {
     _XmMenuBtnDown( wid, event, params, num_params );
   } else {
     _XmRecordEvent(event);
     /* Call Manager action routine */
     _XmGadgetDrag( wid, event, params, num_params );
   }
}


void
_XmRC_UpdateOptionMenuCBG(
        Widget cbg,
        Widget memWidget )
{
   XmString xmstr = NULL;
   Pixmap pix, ipix;
   Arg al[5];
   int ac = 0;

   /* If one of these is NULL then something is wrong. */
   if (cbg == (Widget) NULL ||
       memWidget == (Widget) NULL) return;

   if (XmIsLabelGadget(memWidget))
   {
      XmLabelGadget lg = (XmLabelGadget) memWidget;

      if (LabG_IsText (lg))
      {
         XtSetArg (al[ac], XmNlabelType, XmSTRING);    ac++;
	 xmstr = XmStringCopy(LabG__label(lg));
         XtSetArg (al[ac], XmNlabelString, xmstr);      ac++;

	 if (LabG_Font(lg) != LabG_Font(cbg))
	 {
            XtSetArg (al[ac], XmNfontList, LabG_Font(lg)); ac++;
	 }
      }
      else if (LabG_IsPixmap (lg))
      {
         XtSetArg (al[ac], XmNlabelType, XmPIXMAP);    ac++;
	 pix = LabG_Pixmap(lg);
         XtSetArg (al[ac], XmNlabelPixmap, pix);      ac++;
	 ipix = LabG_PixmapInsensitive(lg);
         XtSetArg (al[ac], XmNlabelInsensitivePixmap, ipix);      ac++;
      }
      else
      {
         XtSetArg (al[ac], XmNlabelType, XmPIXMAP_AND_STRING);    ac++;
	 xmstr = XmStringCopy(LabG__label(lg));
         XtSetArg (al[ac], XmNlabelString, xmstr);      ac++;

	 if (LabG_Font(lg) != LabG_Font(cbg))
	 {
            XtSetArg (al[ac], XmNfontList, LabG_Font(lg)); ac++;
	 }
	 pix = LabG_Pixmap(lg);
         XtSetArg (al[ac], XmNlabelPixmap, pix);      ac++;
	 ipix = LabG_PixmapInsensitive(lg);
         XtSetArg (al[ac], XmNlabelInsensitivePixmap, ipix);      ac++;
      }
      XtSetValues (cbg, al, ac);
   }
   else if (XmIsLabel(memWidget))
   {
      XmLabelWidget lw = (XmLabelWidget) memWidget;

      if (Lab_IsText (lw))
      {
         XtSetArg (al[ac], XmNlabelType, XmSTRING);    ac++;
	 xmstr = XmStringCopy(lw->label._label);
         XtSetArg (al[ac], XmNlabelString, xmstr);      ac++;

	 if (lw->label.font != LabG_Font(cbg))
	 {
            XtSetArg (al[ac], XmNfontList, lw->label.font); ac++;
	 }
      }
      else if (Lab_IsPixmap (lw))
      {
         XtSetArg (al[ac], XmNlabelType, XmPIXMAP);    ac++;
	 pix = lw->label.pixmap;
         XtSetArg (al[ac], XmNlabelPixmap, pix);      ac++;
	 ipix = lw->label.pixmap_insen;
         XtSetArg (al[ac], XmNlabelInsensitivePixmap, ipix);      ac++;
      }
      else
      {
         XtSetArg (al[ac], XmNlabelType, XmPIXMAP_AND_STRING);    ac++;
	 xmstr = XmStringCopy(lw->label._label);
         XtSetArg (al[ac], XmNlabelString, xmstr);      ac++;

	 if (lw->label.font != LabG_Font(cbg))
	 {
            XtSetArg (al[ac], XmNfontList, lw->label.font); ac++;
	 }
	 pix = lw->label.pixmap;
         XtSetArg (al[ac], XmNlabelPixmap, pix);      ac++;
	 ipix = lw->label.pixmap_insen;
         XtSetArg (al[ac], XmNlabelInsensitivePixmap, ipix);      ac++;
      }
      XtSetValues (cbg, al, ac);
   }

   if (xmstr)
      XmStringFree(xmstr);
}

/*
 * At one time this code was tossed out, but it's is once again needed to
 * disregard a focus out event to a cascade in a torn menupane when focus is
 * moved to its newly posted submenu.
 */
static Boolean
ShouldDispatchFocusOut(
        Widget widget )
{
   /* skip focus out for CBs in Torn off panes with cascading submenus
    * unless the cascaded submenu is another torn off
    */
   if (XmIsCascadeButton(widget) &&
       RC_Type(XtParent(widget)) != XmMENU_BAR &&
       !XmIsMenuShell(XtParent(XtParent(widget))) &&
       CB_Submenu(widget) &&
       ((XmMenuShellWidget) XtParent(CB_Submenu(widget)))->shell.popped_up &&
       XmIsMenuShell(XtParent(CB_Submenu(widget))))
   {
       return (False);
   }
   return (True);
}

void
_XmMenuBarGadgetSelect(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   Widget child;

   /* Only dispatch the event to the cascade gadget when the menubar is
    * armed (which infers in explicit mode for menubars - see MenuArm())
    */
   if (RC_IsArmed(rc)/* && _XmGetFocusPolicy( (Widget) mw) == XmEXPLICIT*/)
   {
      child = rc->manager.active_child ;
      if(child && XmIsGadget(child) && XtIsSensitive(child))
	 _XmDispatchGadgetInput(child, event, XmACTIVATE_EVENT);
   }
}

void
_XmMenuGadgetTraverseCurrent(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    Widget child ;

    if (!_XmIsEventUnique(event))
	return;

    child = (Widget) _XmInputForGadget(wid, event->xbutton.x,
				       event->xbutton.y);

    if (child == NULL)
	/*
	 * Button press happened outside of any gadgets.  Call
	 * the normal BtnDown action so menu panes can be popped
	 * down if necessary.
	 */
	XtCallActionProc(wid, "MenuBtnDown", event, params, *num_params);
    else  {
	XmProcessTraversal(child, XmTRAVERSE_CURRENT) ;
	XAllowEvents(XtDisplay(wid), SyncPointer, CurrentTime);
	_XmRecordEvent(event);
    }
}

void
_XmMenuGadgetTraverseCurrentUp(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    Widget child ;

    if (!_XmIsEventUnique(event))
	return;

    child = (Widget) _XmInputForGadget(wid, event->xbutton.x,
				       event->xbutton.y);

    if (child == NULL)
	/*
	 * Button release happened outside of any gadgets.  Call
	 * the normal BtnUp action so menu panes can be popped
	 * down if necessary.
	 */
	XtCallActionProc(wid, "MenuBtnUp", event, params, *num_params);
    else  {
	XAllowEvents(XtDisplay(wid), SyncPointer, CurrentTime);
	_XmRecordEvent(event);
    }
}

static int
MenuStatus( Widget wid )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   int menu_status = 0;

   /* We'll steal the RC_SetBit macro - it should still work for ints as well
    * as bytes.
    */
   RC_SetBit(menu_status, XmMENU_TORN_BIT, RC_TornOff(rc));
   RC_SetBit(menu_status, XmMENU_TEAR_OFF_SHELL_DESCENDANT_BIT,
      _XmIsTearOffShellDescendant((Widget)rc));
   RC_SetBit(menu_status, XmMENU_POPUP_POSTED_BIT, RC_PopupPosted(rc));
   RC_SetBit(menu_status, XmMENU_IN_DRAG_MODE_BIT, _XmGetInDragMode(wid));

   return(menu_status);
}

/* Return menu type from type field */
static int
MenuType(
	Widget wid )
{
  return(RC_Type(wid));
}

/*
 * position the row column widget where it wants to be; normally, this
 * is used only for popup or pulldown menupanes.
 */
static void
PositionMenu(
        register XmRowColumnWidget m,
        XButtonPressedEvent *event )
{
    XmRowColumnWidget root;
    XmCascadeButtonWidget p;

    if (m == NULL)
       return;

    switch (m->row_column.type)
    {
        case XmMENU_OPTION:
        case XmMENU_BAR:
        case XmWORK_AREA:
            break;

        /*
         * remaining cases take advantage of the fact that positioning of
         * a popup shells' only child is not normal.  Just change the widget
         * itself, when popped up the shell will put itself at this location
         * and move the child to 0,0.  Saves a bunch of thrashing about.
         */

        case XmMENU_POPUP:          /* position near mouse */
            if (LayoutIsRtoLM(m))
                XtX (m) = event->x_root -  XtWidth (m);
            else
	      XtX (m) = event->x_root;
            XtY (m) = event->y_root;
            RC_SetWidgetMoved (m, TRUE);
            break;

        case XmMENU_PULLDOWN:
            p = (XmCascadeButtonWidget) m->row_column.cascadeBtn;

            if (p != NULL)
               root = (XmRowColumnWidget) XtParent (p);
            else
               return;

            if (! XmIsRowColumn(root))
               root = NULL;

            LocatePulldown(
                root,           /* menu above */
                p,              /* pulldown linking the two */
                m,              /* menu to position */
                (XEvent *) event);         /* event causing pulldown */

	    RC_SetWidgetMoved (m, TRUE);

            break;
    }
}

/*
 *  - popdown anything that should go away (Calls _XmMenuPopDown)
 *
 * Called whenever a button is activated so that potential tear off restores
 * on the parent menupane don't occur.
 */
static void
ButtonMenuPopDown(
        Widget w,
        XEvent *event,
	Boolean *popped_up )
{
   XmRowColumnWidget rc = FindMenu(w);
   XmRowColumnWidget pane = (XmRowColumnWidget) NULL;
   short depth;
   Boolean posted;
   XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(w));
   XmExcludedParentPaneRec *excPP =
		&(((XmDisplayInfo *)(dd->display.displayInfo))->excParentPane);

   /* - Don't restore any torn panes in the active menu hierarchy.
    *   "lastSelectToplevel" must be preserved for possible callback usage.
    */

   for (pane=rc, depth=0;
 	XmIsRowColumn(pane) &&
	((IsPulldown(pane) || IsPopup(pane)) &&
	 XmIsMenuShell(XtParent(pane)));
	depth++)
   {
       if ((depth+1) > excPP->pane_list_size)
       {
	  excPP->pane_list_size += 4;
	  excPP->pane = (Widget *)
			XtRealloc((char *)excPP->pane,
				  sizeof(Widget) * excPP->pane_list_size);
       }
       (excPP->pane)[depth] = (Widget)pane;

       /* Someone (mwm?) has posted a popup in an unorthodox manner.  Either
 	* the posting event was not verified or the postFromWidget is not
 	* a Motif widget.  Try to save the situation.
 	*/
       if (RC_CascadeBtn(pane) && ! XtIsShell(pane))
	 pane = (XmRowColumnWidget) XtParent(RC_CascadeBtn(pane));
       else
	 break;
       /* If the parent isn't a rowcolumn widget then stop walking
	  up the parents */
       if (pane == (XmRowColumnWidget) NULL ||
	   ! XmIsRowColumn(pane)) break;
   }

   excPP->num_panes = depth;

   _XmMenuPopDown((Widget)rc, event, &posted);

   /* _XmExcludedParentPane structure is used to delay the restoration to
    * preserve menu state until after PushB and ToggleB have a chance to
    * call their arm, activate, and disarm callbacks.  If the menu (e.g.
    * option menu) was posted as a result of click (remain posted), the
    * button up may be caught by the menu-item's activate callback but
    * dropped on the floor.  In this case, _XmExcludedParentPane should
    * be reset.
    */
   if (posted)
     excPP->num_panes = 0;

   if (popped_up)
     *popped_up = posted;
}

static int
InMenu(
        XmRowColumnWidget search_m,
        XmRowColumnWidget *parent_m,
        RectObj child,
        Widget *w )
{
    if (IsInWidgetList (search_m, child))
    {
        *parent_m = search_m;
        *w = (Widget) child;
        return (TRUE);
    }

    return (FALSE);
}

static Boolean
SearchMenu(
        XmRowColumnWidget search_m,
        XmRowColumnWidget *parent_m,
        RectObj child,
        Widget *w,
	Boolean setHistory )
{
    register Widget *q;
    register int i;

    if ( ! InMenu (search_m, parent_m, child, w))
    {
        for (i = 0, q = search_m->composite.children;
             i < search_m->composite.num_children;
             i++, q++)
        {
            if (XtIsManaged(*q))
            {
                if (XmIsCascadeButtonGadget(*q))
                {
                    XmCascadeButtonGadget p =
                        (XmCascadeButtonGadget) *q;

                    if (CBG_Submenu(p) &&
                        SearchMenu ((XmRowColumnWidget) CBG_Submenu(p),
                           (XmRowColumnWidget *) parent_m, (RectObj) child, w,
				     setHistory))
                    {
			if (setHistory)
			    RC_MemWidget(search_m) = (Widget) child;
                        return (TRUE);
                    }
                }
                else if (XmIsCascadeButton(*q))
                {
                    XmCascadeButtonWidget p =
                        (XmCascadeButtonWidget) *q;

                    if (CB_Submenu(p) &&
                        SearchMenu ((XmRowColumnWidget) CB_Submenu(p),
                           (XmRowColumnWidget *) parent_m, (RectObj) child, w,
				     setHistory))
                    {
			if (setHistory)
			    RC_MemWidget(search_m) = (Widget) child;
                        return (TRUE);
                    }
                }
            }
        }
        return (FALSE);
    }
    if (setHistory)
	RC_MemWidget(search_m) = (Widget) child;

    return (TRUE);
}

static void
LotaMagic(
        XmRowColumnWidget m,
        RectObj child,
        XmRowColumnWidget *parent_m,
        Widget *w )
{
    *parent_m = NULL;
    *w = NULL;

    SearchMenu (m, parent_m, child, w, False);
}

static int
NoTogglesOn(
        XmRowColumnWidget m )
{
    register Widget *q;
    register int i;

    ForManagedChildren (m, i, q)
    {
        if (XmIsToggleButtonGadget(*q))
        {
            if (XmToggleButtonGadgetGetState (*q))
               return (FALSE);
        }
        else if (XmIsToggleButton(*q))
        {
            if (XmToggleButtonGetState (*q))
               return (FALSE);
        }
    }

    return (TRUE);
}

static int
IsInWidgetList(
        register XmRowColumnWidget m,
        RectObj w )
{
    register Widget *q;
    register int i;

    if ((m == NULL) || (w == NULL)) return (FALSE);

    for (i = 0, q = m->composite.children;
         i < m->composite.num_children;
         i++, q++)

        if ((*q == (Widget) w) && IsManaged (*q)) return (TRUE);

    return (FALSE);
}

static void
AllOffExcept(
        XmRowColumnWidget m,
        Widget w )
{
    register Widget *q;
    register int i;

    if (w)  /* then all widgets except this one go off */
    {
        ForManagedChildren (m, i, q)
        {
            if (*q != w)
            {
                if (XmIsToggleButtonGadget(*q))
                {
                   if (XmToggleButtonGadgetGetState (*q))
                        XmToggleButtonGadgetSetState (*q, FALSE, TRUE);
                }
                else if (XmIsToggleButton(*q))
                {
                   if (XmToggleButtonGetState (*q))
                        XmToggleButtonSetState (*q, FALSE, TRUE);
                }
            }
        }
    }
}

/*
 * called by the buttons to verify that the passed in event is one that
 * should be acted upon.  This is called through the menuProcs handle
 */
static Boolean
VerifyMenuButton(
        Widget w,
        XEvent *event)
{
  Boolean valid;

  if (IsPopup(w)) {
    valid = event && (_XmMatchBtnEvent( event, XmIGNORE_EVENTTYPE,
				       RC_PostButton(w),
				       RC_PostModifiers(w)) ||
		      _XmMatchBSelectEvent( w, event)) ;
  } else {
    /* CDE modification allows any button to activate a cascade
       button to show a menu */
    valid = event &&
      (event -> type == ButtonPress || event -> type == ButtonRelease);
  }

  return(valid);
}

/*
 * This routine is called at Initialize or SetValues time.  It updates
 * the memory widget in the current rowcolumn and up to the top level(s)
 * menus.  If there is a postFromList on the pulldown, it goes up each
 * branch.  If an option menu is found at the top, then its cascadebutton
 * is updated with the latest stuff.
 */
static Boolean
UpdateMenuHistory(
        XmRowColumnWidget menu,
        Widget child,
	Boolean updateOnMemWidgetMatch)
{
   int i;
   Widget cb;
   Boolean retval = False;

   if (IsOption(menu))
   {
      if (updateOnMemWidgetMatch && (RC_MemWidget(menu) != child))
         return(False);

      if ((cb = XmOptionButtonGadget( (Widget) menu)) != NULL)
      {
	 _XmRC_UpdateOptionMenuCBG (cb, child);
	 retval = True;
      }
   }
   else if (IsPulldown(menu))
   {
      for (i=0; i < menu->row_column.postFromCount; i++)
      {
	 Widget parent_menu = XtParent(menu->row_column.postFromList[i]);

	 if (UpdateMenuHistory ((XmRowColumnWidget) parent_menu, child,
			    updateOnMemWidgetMatch))
	 {
	    RC_MemWidget(parent_menu) = child;
	    /* Don't return immediately - allow next postFromWidget to
	     * update menuHistory as well.
	     */
	    retval = True;
	 }
      }
   }
   return(retval);
}

/*
 * this is a mess... the menu spec'd is the menu to set the history for;
 * the child spec'd is the child who we are pretending fired-off.   The
 * problem is the child may be in any sub-menu of this cascade.  This is
 * called by Initialize or SetValues.
 */
void
_XmRC_SetMenuHistory(
        XmRowColumnWidget m,
        RectObj child )
{
   XmRowColumnWidget parent_m;
   Widget w;

   if (IsNull (child))
       return;

   /* make sure that the child is in the menu hierarchy */
   LotaMagic (m, child, &parent_m, &w);

   if (w)
       if (UpdateMenuHistory (parent_m, w, False))
	  RC_MemWidget(parent_m) = w;
}

/*
 * This routine is similar to the one above, except it only sets the
 * memory widget for the submenus down the cascade.  This is called during
 * option menus setvalues and initialize routines.
 */
void
_XmRC_SetOptionMenuHistory(
        XmRowColumnWidget m,
        RectObj child )
{
   XmRowColumnWidget parent_m = NULL;
   Widget w = NULL;

   if (IsNull (child))
	return;

    SearchMenu (m, &parent_m, child, &w, True);


}

/*
 * note that this is potentially recursive, setting the state of a
 * toggle in this row column widget may re-enter this routine...
 */
static void
RadioBehaviorAndMenuHistory(
        XmRowColumnWidget m,
        Widget w )
{
   XmRowColumnWidget menu;
   Widget cb;
   Boolean done = FALSE;

   if (! IsManaged(w))
       return;

   if (RC_RadioBehavior(m))
   {
      if (XmIsToggleButtonGadget(w))
      {
	 /* turned on */
	 if (XmToggleButtonGadgetGetState (w))
	     AllOffExcept (m, w);

	 /* he turned off */
	 else
	 {
            if (RC_RadioAlwaysOne(m))
                if (NoTogglesOn (m))
		    /* can't allow that */
                    XmToggleButtonGadgetSetState (w, TRUE, TRUE);
	 }
      }
      else if (XmIsToggleButton (w))
      {
	 /* turned on */
	 if (XmToggleButtonGetState (w))
	     AllOffExcept (m, w);

	 /* turned off */
	 else
	 {
            if (RC_RadioAlwaysOne(m))
                if (NoTogglesOn (m))
		    /* can't allow that */
                    XmToggleButtonSetState (w, TRUE, TRUE);
	 }
      }

      /* record for posterity */
      RC_MemWidget (m) = w;
   }

   /* record the mouse memory and history widget all the way up the cascade */
   menu = m;
   cb = 0;
   while ( ! done)
   {
      RC_MemWidget (menu) = w;

      if (! IsPopup(menu) && RC_CascadeBtn(menu))
      {
	cb = RC_CascadeBtn(menu);
	menu = (XmRowColumnWidget) XtParent (cb);
      }

      else
	  done = TRUE;
   }

   /* option menu cascade button gadget must be updated */
   if (IsOption(menu))
       _XmRC_UpdateOptionMenuCBG (cb, w);
}

/*
 * This routine is used to emulate the override callback functionality that
 * was available in the R3 library used by Xm and to do the radio behavior
 * and menu history functionality for RowColumns.  The buttons call this
 * function through the MenuProcs interface.
 */
static void
ChildsActivateCallback(
        XmRowColumnWidget rowcol,
        Widget child,
        XtPointer call_value )
{
   Arg arg[1];
   int i;
   XtCallbackList callbacks;
   char *c = NULL;
   XmMenuSavvyTrait menuSavvyRec;

   menuSavvyRec = (XmMenuSavvyTrait)
     XmeTraitGet((XtPointer) XtClass(child), XmQTmenuSavvy);

   if (menuSavvyRec != NULL &&
       menuSavvyRec -> getActivateCBName != NULL)
     c = menuSavvyRec -> getActivateCBName();

   /*
    * Set up info before the entry callback is called
    */
   GetLastSelectToplevel(rowcol);

   if (rowcol->row_column.entry_callback != NULL)
   {
      XtSetArg (arg[0], c, &callbacks);
      XtGetValues (child, arg, 1);

      /* make sure the all of the drawing has been done before the callback */
      XFlush (XtDisplay (rowcol));

      /*
       * cycle through the list and call the entry fired routine for each
       * entry in this callback list, sending in the data for each entry.
       * If the list is NULL, or empty, call the entry fired function once.
       */
      if ((callbacks == NULL) ||
	  (callbacks[0].callback == (XtCallbackProc) NULL)) /* CR 5256 */
	  EntryFired (child, NULL, (XmAnyCallbackStruct *) call_value);

      else
      {
	  int count;
	  XtPointer * callbackClosure;
	  /*
	   * Note:  We must make a copy of the callback data returned since
	   * the information may be lost on the next Xt call.
	   * We are only interested in the closure data for the entry callback.
	   */

	  count = 0;
	  while (callbacks[count].callback != NULL)
	    count++;

	  callbackClosure =
	      (XtPointer *) XtMalloc(sizeof(XtPointer) * count);

	  for (i=0; i < count; i++)
	      callbackClosure[i] = callbacks[i].closure;

	  for (i=0; i < count; i++)
	    EntryFired (child, callbackClosure[i], (XmAnyCallbackStruct *) call_value);

	  XtFree ((char *)callbackClosure);
      }
   }
   else
       /* no entry callback, but must do radio behavior & menu history */
       EntryFired (child, NULL, (XmAnyCallbackStruct *) call_value);
}

/*
 * This is the callback for widgets which are composited into row column
 * widgets.  It notifies the menu that some individual widget fired off;
 * this allows * the row column widget to tell the application if it wants
 * to know.  also to do various other automagical things
 */
static void
EntryFired(
        Widget w,
        XtPointer client_data,
        XmAnyCallbackStruct *callback )
{
    XmRowColumnWidget m = (XmRowColumnWidget) XtParent (w);
    XmRowColumnCallbackStruct mr;

    mr.reason       = XmCR_ACTIVATE;    /* menu activated */
    mr.widget       = w;
    mr.data         = (char *) client_data;
    mr.callbackstruct   = (char *) callback;  /* subwidget structure */
    mr.event            = callback->event;

    RadioBehaviorAndMenuHistory (m, w);

    /* fire callback when done, so that it has correct (current) information */
    XtCallCallbackList ((Widget) m, m->row_column.entry_callback, &mr);
}

static void
MenuArm(
        Widget w )
{
   XmRowColumnWidget m = FindMenu(w);
   XmMenuState mst = _XmGetMenuState((Widget)w);

   if (!RC_IsArmed(m))
   {
      /*
       * indicate that the display is grabbed so that drag & drop will not
       * interfere with the menu interaction.
       */
      XmDisplay disp = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
      disp->display.userGrabbed = True;

      if (IsBar(m))
      {
         Widget topmostShell = _XmFindTopMostShell( (Widget) m);
         Arg args[1];

         /*
          * save the current highlighted item so that it can be restored after
          * the menubar traversal mode is finished.  This only makes sense
          * if the menubar itself is not the current tabgroup.
          */
         mst->RC_activeItem = _XmGetActiveItem( (Widget) m);
         if (mst->RC_activeItem && XtParent(mst->RC_activeItem) == (Widget)m)
            mst->RC_activeItem = NULL;

         /* Make sure focus policy is explicit during active menubar */
         if ((RC_OldFocusPolicy(m) = _XmGetFocusPolicy( (Widget) m)) !=
              XmEXPLICIT)
         {
           /* Generate a synthetic crossing event from current focus widget
            * to menubar so the current focus widget can unhighlight itself.
            * This is necessary in case the leave routine checks for XmPOINTER
            * focus policy (as in _XmLeaveGadget).
            */
            XCrossingEvent xcrossing;

            /* If activeItem is NULL, then there's no possible widget/gadget to
            * unhighlight.
             */
            if (mst->RC_activeItem)
            {
               xcrossing.type = LeaveNotify;
               xcrossing.serial =
                  LastKnownRequestProcessed(XtDisplay(mst->RC_activeItem));
               xcrossing.send_event = False;
               xcrossing.display = XtDisplay(mst->RC_activeItem);
               xcrossing.window = XtWindow(mst->RC_activeItem);
               xcrossing.subwindow = 0; /* CR 5257 */
               xcrossing.time =
		  XtLastTimestampProcessed(XtDisplay(mst->RC_activeItem));
               xcrossing.mode = 1;
               xcrossing.detail = NotifyNonlinear;
               xcrossing.same_screen = True;
               xcrossing.focus = True;
               xcrossing.state = 0;

               XtDispatchEvent((XEvent *) &xcrossing);
            }

            XtSetArg (args[0], XmNkeyboardFocusPolicy, XmEXPLICIT);
            XtSetValues (topmostShell, args, 1);
         }

         /*
          * _XmMenuFocus(XmMENU_BEGIN) already called; switch tab group.
          * 'widget' is cascade button for menubar case.
          */
         m->manager.traversal_on = True;
         XmProcessTraversal(w, XmTRAVERSE_CURRENT);

         /*
          * Menubars need their own exclusive/SL grab, so that they will
          * still get input even when a cascade button without a submenu
          * has the focus.
          */
         _XmAddGrab( (Widget) m, XtGrabNonexclusive, True);
         RC_SetBeingArmed(m, True);

         /* Swallow focus & crossing events to menubar (only) */
         _XmSetSwallowEventHandler((Widget) m, True);
      }

      RC_SetArmed (m, True);
   }
}

static void
MenuDisarm(
        Widget w )
{
   XmRowColumnWidget m = FindMenu(w);
   XmMenuState mst = _XmGetMenuState((Widget)w);

   if (RC_IsArmed(m))
   {
      /*
       * indicate that the display is ungrabbed so that drag & drop
       * is enabled if this is the toplevel menu
       */
      if (IsBar(m) || IsPopup(m) || IsOption(m) ||
	  (IsPulldown(m) && !(XmIsMenuShell(XtParent(m)))))
      {
	  XmDisplay disp = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
	  disp->display.userGrabbed = False;
      }

      if (IsBar(m))
      {
         Widget topmostShell = _XmFindTopMostShell( (Widget) m);
         Arg args[1];

         _XmRemoveGrab( (Widget) m);
         RC_SetBeingArmed(m, False);

         m->manager.traversal_on = False;

         /* restore focus policy */
         if (RC_OldFocusPolicy(m) == XmEXPLICIT)
         {
            /*
             * restore the activeItem that had the focus BEFORE the
             * menubar mode was entered.
             */
            if (mst->RC_activeItem
		&& !mst->RC_activeItem->core.being_destroyed)
	    {
               XmProcessTraversal (mst->RC_activeItem, XmTRAVERSE_CURRENT);
               mst->RC_activeItem = NULL;
            } else
            {
               XmProcessTraversal (topmostShell, XmTRAVERSE_NEXT_TAB_GROUP);
            }
         }
         else   /* XmPOINTER */
         {
            if (m->manager.active_child)
            {
               XmCascadeButtonHighlight(m->manager.active_child, False);
               _XmClearFocusPath((Widget) m);
            }

	    /* Clear internal focus structures so key events will be
	     * dispatched correctly.
	     */
	    XtSetKeyboardFocus(topmostShell, None);

            XtSetArg (args[0], XmNkeyboardFocusPolicy, XmPOINTER);
            XtSetValues (topmostShell, args, 1);
         }
         _XmSetSwallowEventHandler((Widget) m, False);
     }

      /* tear off shell */
     else if ((IsPulldown(m) || IsPopup(m)) && !XmIsMenuShell(XtParent(m)))
     {
       /* this needs to be done for XmPOINTER focus, because there is
	  no event for the focus out translation.  There doesn't seem
          to be any way of detecting that we are in pointer, though. */
       if (m->manager.active_child)
       {
	 XmCascadeButtonHighlight(m->manager.active_child, False);
       }

       _XmRemoveGrab((Widget) m);

       RC_SetBeingArmed(m, False);
     }
     RC_SetArmed (m, FALSE);
   }
}

static void
TearOffArm(
        Widget w )
{
   XmRowColumnWidget m = FindMenu(w);
   Display *dpy = XtDisplay(w);
   XmMenuSystemTrait menuSTrait;
   Time _time = XtLastTimestampProcessed(XtDisplay(w));

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass((Widget) m), XmQTmenuSystem);

   /* If the parent is torn and active, AND this is an initial selection
    * (e.g. ButtonPress), place the menu system into an active state by
    * arming and setting up grabs.
    */
   if ((IsPulldown(m) || IsPopup(m)) &&
        !XmIsMenuShell(XtParent(m)) && !RC_IsArmed(m))
   {
       /* Don't post the menu if the menu cannot control grabs! */
       if (_XmMenuGrabKeyboardAndPointer((Widget)m, _time) != GrabSuccess)
       {
       return;
       }

      _XmMenuFocus((Widget)m, XmMENU_BEGIN, _time);

      /* To support menu replay, keep the pointer in sync mode */
      XAllowEvents(dpy, SyncPointer, CurrentTime);

      menuSTrait -> arm((Widget) m);

      /* A submenu is posted!  Set up modal grab */
      /* Make it act like a pseudo popup and give it spring loaded
       * characteristics.
       */
      _XmAddGrab( (Widget) m, XtGrabNonexclusive, True);

      XFlush(dpy);
   }
}

static void
InvalidateOldFocus(
	Widget oldWidget,	/* unused */
	Window **poldFocus,
	XEvent *event)		/* unused */
{
  *poldFocus = NULL;
}

/*
 * Return the widget which the menu was posted from.
 *   - If this is in a popup, it is the widget which initiated the post
 *     (via positioning & managing or via armAndActivate).
 *   - If it is in a pulldown from a menubar or option menu, then the
 *     returned widget is the menubar or option menu.
 *   - If it is a tear off popup or pulldown, the postedFromWidget was
 *     determined at tear time and stored away.
 */
Widget
XmGetPostedFromWidget(
        Widget menu )
{
   XmRowColumnWidget toplevel;
   Widget wid = NULL;
   _XmWidgetToAppContext(menu);

   _XmAppLock(app);
   if (menu && XmIsRowColumn(menu))
   {
      toplevel = (XmRowColumnWidget)
	 ((XmRowColumnWidget)menu)->row_column.lastSelectToplevel;

      if (toplevel && IsPopup(toplevel))
      {
	 /* active widget is kept in cascadeBtn field for popups */
	 wid =  RC_CascadeBtn(toplevel);
      }
      else
	 wid = (Widget)toplevel;
   }
   _XmAppUnlock(app);
   return wid;
}

Widget
XmGetTearOffControl(
        Widget menu )
{
   Widget wid = NULL;
   _XmWidgetToAppContext(menu);

   _XmAppLock(app);
   if (menu && XmIsRowColumn(menu))
      wid = RC_TearOffControl(menu);
   _XmAppUnlock(app);
   return wid;
}


void
XmMenuPosition(
        Widget p,
        XButtonPressedEvent *event )
{
   _XmWidgetToAppContext(p);
   _XmAppLock(app);
   PositionMenu ((XmRowColumnWidget) p, event);
   _XmAppUnlock(app);
}



static Boolean
MenuSystemPopdown( Widget w, XEvent *e )
{
  Boolean popped_up;

  _XmMenuPopDown(w, e, &popped_up);

  return(popped_up);
}

static Boolean
MenuSystemButtonPopdown( Widget w, XEvent *e )
{
  Boolean popped_up;

  ButtonMenuPopDown(w, e, &popped_up);

  return(popped_up);
}

static void
MenuChildFocus(Widget w)
{
  /* So KHelp event is delivered correctly */
  _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, TRUE);
  XtSetKeyboardFocus(XtParent(XtParent(w)), (Widget) w);
  _XmSetFocusFlag( XtParent(XtParent(w)), XmFOCUS_IGNORE, FALSE);
}

static Widget
MenuPopupPosted(Widget w)
{
  return(RC_PopupPosted(w));
}
