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
static char rcsid[] = "$TOG: TearOff.c /main/15 1997/08/21 14:19:26 csn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/cursorfont.h>
#include <Xm/BaseClassP.h>
#include <Xm/Cursor.h>
#include <Xm/DisplayP.h>
#include <Xm/GadgetP.h>
#include <Xm/LabelP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuT.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/SeparatorP.h>
#include <Xm/TraitP.h>
#include <Xm/VirtKeysP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "RCMenuI.h"
#include "RowColumnI.h"
#include "ScreenI.h"
#include "TearOffI.h"
#include "TraversalI.h"
#include "XmI.h"

#define IsPopup(m)     \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_POPUP)
#define IsPulldown(m)  \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_PULLDOWN)
#define IsOption(m)    \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_OPTION)
#define IsBar(m)       \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_BAR)

/* Bury these here for now - not spec'd for 1.2, maybe for 1.3? */
#define CREATE_TEAR_OFF 0
#define RESTORE_TEAR_OFF_TO_TOPLEVEL_SHELL 1
#define RESTORE_TEAR_OFF_TO_MENUSHELL 2
#define DESTROY_TEAR_OFF 3

#define OUTLINE_WIDTH	2
#define SEGS_PER_DRAW	(4*OUTLINE_WIDTH)


/********    Static Function Declarations    ********/

static GC InitXmTearOffXorGC(
                        Widget wid) ;
static void SetupOutline(
                        Widget wid,
			GC gc,
                        XSegment pOutline[],
                        XEvent *event,
			Dimension delta_x,
			Dimension delta_y ) ;
static void EraseOutline(
                        Widget wid,
			GC gc,
                        XSegment *pOutline) ;
static void MoveOutline(
                        Widget wid,
			GC gc,
                        XSegment *pOutline,
                        XEvent *event,
			Dimension delta_x,
			Dimension delta_y ) ;
static void PullExposureEvents(
                        Widget wid ) ;
static void MoveOpaque(
                        Widget wid,
                        XEvent *event,
			Dimension delta_x,
			Dimension delta_y ) ;
static void GetConfigEvent(
                        Display *display,
                        Window window,
                        unsigned long mask,
                        XEvent *event) ;
static Cursor GetTearOffCursor(Widget wid);
static Boolean DoPlacement(
                        Widget wid,
                        XEvent *event) ;
static void CallTearOffMenuActivateCallback(
                        Widget wid,
                        XEvent *event,
			unsigned short origin) ;
static void CallTearOffMenuDeactivateCallback(
                        Widget wid,
                        XEvent *event,
			unsigned short origin) ;
static void RemoveTearOffEventHandlers(
			Widget wid ) ;
static void DismissOnPostedFromDestroy(
			Widget w,
			XtPointer clientData,
			XtPointer callData ) ;
static void DisplayDestroyCallback (
			Widget w,
			XtPointer client_data,
			XtPointer call_data );
/********    End Static Function Declarations    ********/


static GC
InitXmTearOffXorGC(
	Widget wid )
{
    XGCValues gcv;
    XtGCMask  mask;

    mask = GCFunction | GCLineWidth | GCSubwindowMode | GCCapStyle;
    gcv.function = GXinvert;
    gcv.line_width = 0;
    gcv.cap_style = CapNotLast;
    gcv.subwindow_mode = IncludeInferiors;

    return (XCreateGC (XtDisplay(wid), wid->core.screen->root,
		       mask, &gcv));
}

static void
SetupOutline(
	Widget wid,
	GC gc,
	XSegment pOutline[],
	XEvent *event,
	Dimension delta_x,
	Dimension delta_y)
{
   Position x, y;
   Dimension w, h;
   int n = 0;
   int i;

   x = event->xbutton.x_root - delta_x;
   y = event->xbutton.y_root - delta_y;
   w = wid->core.width;
   h = wid->core.height;

   for(i=0; i<OUTLINE_WIDTH; i++)
   {
      pOutline[n].x1 = x;
      pOutline[n].y1 = y;
      pOutline[n].x2 = x + w - 1;
      pOutline[n++].y2 = y;

      pOutline[n].x1 = x + w - 1;
      pOutline[n].y1 = y;
      pOutline[n].x2 = x + w - 1;
      pOutline[n++].y2 = y + h - 1;

      pOutline[n].x1 = x + w - 1;
      pOutline[n].y1 = y + h  - 1;
      pOutline[n].x2 = x;
      pOutline[n++].y2 = y + h - 1;

      pOutline[n].x1 = x;
      pOutline[n].y1 = y + h  - 1;
      pOutline[n].x2 = x;
      pOutline[n++].y2 = y;

      x += 1;
      y += 1;
      w -= 2;
      h -= 2;
   }

   XDrawSegments(XtDisplay(wid), wid->core.screen->root, gc,
		 pOutline, SEGS_PER_DRAW);
}

static void
EraseOutline(
	Widget wid,
	GC gc,
	XSegment *pOutline )
{
   XDrawSegments(XtDisplay(wid), wid->core.screen->root, gc,
		 pOutline, SEGS_PER_DRAW);
}

static void
MoveOutline(
	Widget wid,
	GC gc,
	XSegment *pOutline,
	XEvent *event,
	Dimension delta_x,
	Dimension delta_y )
{
   EraseOutline(wid, gc, pOutline);
   SetupOutline(wid, gc, pOutline, event, delta_x, delta_y);
}

static void
PullExposureEvents(
	Widget wid )
{
   XEvent      event;
   /*
    * Force the exposure events into the queue
    */
   XSync (XtDisplay(wid), False);

   /*
    * Selectively extract the exposure events
    */
   while (XCheckMaskEvent (XtDisplay(wid), ExposureMask, &event))
   {
      /*
       * Dispatch widget related event:
       */
      XtDispatchEvent (&event);
   }
}

static void
MoveOpaque(
	Widget wid,
	XEvent *event,
	Dimension delta_x,
	Dimension delta_y )
{
   /* Move the MenuShell */
   XMoveWindow(XtDisplay(wid), XtWindow(XtParent(wid)),
      event->xbutton.x_root - delta_x, event->xbutton.y_root - delta_y);

   /* cleanup exposed frame parts */
   PullExposureEvents (wid);
}


#define CONFIG_GRAB_MASK (ButtonPressMask|ButtonReleaseMask|\
		     PointerMotionMask|PointerMotionHintMask|\
		     KeyPress|KeyRelease)

#define POINTER_GRAB_MASK (ButtonPressMask|ButtonReleaseMask|\
                   PointerMotionMask|PointerMotionHintMask)


static void
GetConfigEvent(
	Display *display,
	Window window,
	unsigned long mask,
	XEvent *event )
{
   Window root_ret, child_ret;
   int root_x, root_y, win_x, win_y;
   unsigned int mask_ret;

   /* Block until a motion, button, or key event comes in */
   XWindowEvent(display, window, mask, event);

   if (event->type == MotionNotify &&
        event->xmotion.is_hint == NotifyHint)
   {
       /*
        * "Ack" the motion notify hint
        */
       if ((XQueryPointer (display, window, &root_ret,
               &child_ret, &root_x, &root_y, &win_x,
               &win_y, &mask_ret)))
       {
           /*
            * The query pointer values say that the pointer
            * moved to a new location.
            */
           event->xmotion.window = root_ret;
           event->xmotion.subwindow = child_ret;
           event->xmotion.x = root_x;
           event->xmotion.y = root_y;
           event->xmotion.x_root = root_x;
           event->xmotion.y_root = root_y;
       }
   }
}

static void DisplayDestroyCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
	(void)call_data;
	XFreeCursor(XtDisplay(w), (Cursor)client_data);
}

static Cursor GetTearOffCursor(Widget wid)
{
	XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(wid));
	Cursor TearOffCursor =
		((XmDisplayInfo *)(dd->display.displayInfo))->TearOffCursor;

	if (TearOffCursor == None)
		{
		/* create some data shared among all instances on this
		** display; the first one along can create it, and
		** any one can remove it; note no reference count
		*/
        	TearOffCursor = XmeLoadCursor(XtDisplay(wid), XtScreen(wid), "fleur");
		if (TearOffCursor == None)
			TearOffCursor = _XmGetMenuCursorByScreen(XtScreen(wid));
		else
			XtAddCallback((Widget)dd, XtNdestroyCallback,
			  DisplayDestroyCallback,(XtPointer)TearOffCursor);
		((XmDisplayInfo *)(dd->display.displayInfo))->TearOffCursor =
			TearOffCursor;
		}

   return TearOffCursor;
}

static Boolean
DoPlacement(
       Widget wid,
       XEvent *event )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XSegment outline[SEGS_PER_DRAW];
   Boolean placementDone;
   KeyCode *KCancel;
   KeySym keysym = osfXK_Cancel;
   int num_keys, index;
   XmKeyBinding keys;
   Boolean moveOpaque = False;
   Dimension delta_x, delta_y;
   Dimension old_x_root = 0;
   Dimension old_y_root = 0;
   GC tearoffGC;

   /* Determine which keycodes are bound to osfXK_Cancel. */
   num_keys = XmeVirtualToActualKeysyms(XtDisplay(rc), keysym, &keys);
   KCancel = (KeyCode*) XtMalloc(num_keys * sizeof(KeyCode));
   for (index = 0; index < num_keys; index++)
     KCancel[index] = XKeysymToKeycode(XtDisplay(rc), keys[index].keysym);
   XtFree((char*) keys);

   /* Regrab the pointer and keyboard to the root window.  Grab in Async
    * mode to free up the input queues.
    */
   XGrabPointer(XtDisplay(rc), rc->core.screen->root, FALSE,
        (unsigned int)POINTER_GRAB_MASK,
        GrabModeAsync, GrabModeAsync, rc->core.screen->root,
        GetTearOffCursor(wid), CurrentTime);

   XGrabKeyboard(XtDisplay(rc), rc->core.screen->root, FALSE,
      GrabModeAsync, GrabModeAsync, CurrentTime);

   tearoffGC = InitXmTearOffXorGC((Widget)rc);

   delta_x = event->xbutton.x_root - XtX(XtParent(rc));
   delta_y = event->xbutton.y_root - XtY(XtParent(rc));

   moveOpaque = _XmGetMoveOpaqueByScreen(XtScreen(rc));

   /* Set up a dummy event of the menu's current position in case the
    * move-opaque is cancelled.
    */
   if (moveOpaque)
   {
     old_x_root = XtX(XtParent(rc));
     old_y_root = XtY(XtParent(rc));
     MoveOpaque((Widget)rc, event, delta_x, delta_y);
   }
   else
      SetupOutline((Widget)rc, tearoffGC, outline, event, delta_x, delta_y);

   placementDone = FALSE;

   while (!placementDone)
   {
      GetConfigEvent (XtDisplay(rc), rc->core.screen->root, CONFIG_GRAB_MASK,
         event);        /* ok to overwrite event? */

      switch (event->type)
      {
         case ButtonRelease:
            if (event->xbutton.button == /*BDrag */ 2)
            {
	       if (!moveOpaque)
                  EraseOutline((Widget)rc, tearoffGC, outline);
	       else
		  /* Signal next menushell post to reposition */
		  XtX(XtParent(rc)) = XtY(XtParent(rc)) = 0;

               placementDone = TRUE;
	       event->xbutton.x_root -= delta_x;
	       event->xbutton.y_root -= delta_y;
            }
            break;

         case MotionNotify:
	    if (moveOpaque)
	       MoveOpaque((Widget)rc, event, delta_x, delta_y);
	    else
               MoveOutline((Widget)rc, tearoffGC, outline, event,
			   delta_x, delta_y);
            break;

         case KeyPress:
	    /* Shouldn't we be checking modifiers too??? */
	    for (index = 0; index < num_keys; index++)
	      if (event->xkey.keycode == KCancel[index])
		{
		  if (!moveOpaque)
		    EraseOutline((Widget)rc, tearoffGC, outline);
		  else
		    {
		      event->xbutton.x_root = old_x_root;
		      event->xbutton.y_root = old_y_root;
		      MoveOpaque((Widget)rc, event, 0, 0);
		    }

		  XtFree((char*) KCancel);
		  return(FALSE);
		}
            break;
      }
   }
   XFreeGC(XtDisplay(rc), tearoffGC);

   XUngrabKeyboard(XtDisplay(rc), CurrentTime);
   XUngrabPointer(XtDisplay(rc), CurrentTime);

   XtFree((char*) KCancel);

   return (TRUE);
}

static void
CallTearOffMenuActivateCallback(
	Widget wid,
	XEvent *event,
	unsigned short origin )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XmRowColumnCallbackStruct callback;

   if (!rc->row_column.tear_off_activated_callback)
      return;

   callback.reason = XmCR_TEAR_OFF_ACTIVATE;
   callback.event  = event;
   callback.widget = NULL;	/* these next two fields are spec'd NULL */
   callback.data   = (char *)(unsigned long) origin;
   callback.callbackstruct = NULL;
   XtCallCallbackList ((Widget)rc, rc->row_column.tear_off_activated_callback,
      &callback);
}

static void
CallTearOffMenuDeactivateCallback(
	Widget wid,
	XEvent *event,
	unsigned short origin )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XmRowColumnCallbackStruct callback;

   if (!rc->row_column.tear_off_deactivated_callback)
      return;

   callback.reason = XmCR_TEAR_OFF_DEACTIVATE;
   callback.event  = event;
   callback.widget = NULL;	/* these next two fields are spec'd NULL */
   callback.data   = (char *)(unsigned long) origin;
   callback.callbackstruct = NULL;
   XtCallCallbackList ((Widget) rc,
      rc->row_column.tear_off_deactivated_callback, &callback);
}

/*
 * This event handler is added to label widgets and separator widgets inside
 * a tearoff menu pane.  This enables the RowColumn to watch for the button
 * presses inside these widgets and to 'do the right thing'.
 */
void
_XmTearOffBtnDownEventHandler(
        Widget reportingWidget,
        XtPointer data,		/* unused */
        XEvent *event,
        Boolean *cont )
{
    Widget parent;

    if (reportingWidget)
    {
       /* make sure only called for widgets inside a menu rowcolumn */
	  if ((XmIsRowColumn(parent = XtParent(reportingWidget))) &&
	      (RC_Type(parent) != XmWORK_AREA))
	  {
	      _XmMenuBtnDown (parent, event, NULL, 0);
	  }
    }
    *cont = True;
}

void
_XmTearOffBtnUpEventHandler(
        Widget reportingWidget,
        XtPointer data,		/* unused */
        XEvent *event,
        Boolean *cont )
{
    Widget parent;

    if (reportingWidget)
    {
       /* make sure only called for widgets inside a menu rowcolumn */
	  if ((XmIsRowColumn(parent = XtParent(reportingWidget))) &&
	      (RC_Type(parent) != XmWORK_AREA))
	  {
	      _XmMenuBtnUp (parent, event, NULL, 0);
	  }
    }
    *cont = True;
}

void
_XmAddTearOffEventHandlers(
	Widget wid )
{
    XmRowColumnWidget rc = (XmRowColumnWidget) wid;
    Widget child;
    int i;
    Cursor cursor = _XmGetMenuCursorByScreen(XtScreen(wid));
    XmMenuSavvyTrait	mtrait;

    for (i=0; i < rc->composite.num_children; i++)
    {
	child = rc->composite.children[i];

	mtrait = (XmMenuSavvyTrait) XmeTraitGet(XtClass(child), XmQTmenuSavvy);

	/*
	 * The label and separator widgets do not care about
	 * button presses.  Add an event handler for these widgets
	 * so that the tearoff RowColumn is alerted about presses in
	 * these widgets.
	 *
	 * This now determines the right widgets to install the
	 * handlers on by using the MenuSavvy trait.  If the
	 * widget can't supply an activateCB name,  then it is
	 * assumed that the widget isn't activatable.
	 *
	 */
	if (! XmIsGadget(child) &&
	    (mtrait == (XmMenuSavvyTrait) NULL ||
	     mtrait -> getActivateCBName ==
	     (XmMenuSavvyGetActivateCBNameProc) NULL))
	  {
    	    XtAddEventHandler(child, ButtonPressMask, False,
			      _XmTearOffBtnDownEventHandler,  NULL);
	    XtAddEventHandler(child, ButtonReleaseMask, False,
			      _XmTearOffBtnUpEventHandler,  NULL);
	  }

	if (XtIsWidget(child))
	   XtGrabButton (child, (int)AnyButton, AnyModifier, TRUE,
	      (unsigned int)ButtonPressMask, GrabModeAsync, GrabModeAsync,
	      None, cursor);
    }
}


static void
RemoveTearOffEventHandlers(
	Widget wid )
{
    XmRowColumnWidget rc = (XmRowColumnWidget) wid;
    Widget child;
    int i;

    for (i=0; i < rc->composite.num_children; i++)
    {
	child = rc->composite.children[i];

	/*
	 * Remove the event handlers on the label and separator widgets.
	 */
	if ((XtClass(child) == xmLabelWidgetClass) ||
	    _XmIsFastSubclass(XtClass(child), XmSEPARATOR_BIT))
	{

	    XtRemoveEventHandler(child, ButtonPressMask, False,
				 _XmTearOffBtnDownEventHandler,  NULL);
	    XtRemoveEventHandler(child, ButtonReleaseMask, False,
				 _XmTearOffBtnUpEventHandler,  NULL);
	}

	if (XtIsWidget(child) && !child->core.being_destroyed)
	   XtUngrabButton(child, (unsigned int)AnyButton, AnyModifier);
    }
}

void
_XmDestroyTearOffShell(
	Widget wid )
{
   TopLevelShellWidget to_shell = (TopLevelShellWidget)wid;

   to_shell->composite.num_children = 0;

   if (to_shell->core.being_destroyed)
     return;

   XtPopdown((Widget)to_shell);

   if (to_shell->core.background_pixmap != XtUnspecifiedPixmap)
   {
     XFreePixmap(XtDisplay(to_shell), to_shell->core.background_pixmap);
     to_shell->core.background_pixmap = XtUnspecifiedPixmap;
   }

   /* Before destroying the shell, force XtSetKeyboardFocus to remove any
   ** XmNdestroyCallbacks on other widgets which name this shell as client_data
   */
   XtSetKeyboardFocus((Widget)to_shell, NULL);

   XtDestroyWidget((Widget)to_shell);
}

void
_XmDismissTearOff(
	Widget shell,
        XtPointer closure,
        XtPointer call_data)	/* unused */
{
   XmRowColumnWidget submenu = NULL;

   /* The first time a pane is torn, there's no tear off shell to destroy.
    */
   if (!shell ||
       !(((ShellWidget)shell)->composite.num_children) ||
       !(submenu =
	 (XmRowColumnWidget)(((ShellWidget)shell)->composite.children[0])) ||
       !RC_TornOff(submenu))
      return;

   RC_SetTornOff(submenu, FALSE);
   RC_SetTearOffActive(submenu, FALSE);

   /* Unhighlight the active child and clear the focus for the next post */
   if (submenu->manager.active_child)
   {
      /* update visible focus/highlighting */
      if (XmIsPrimitive(submenu->manager.active_child))
      {
	  (*(((XmPrimitiveClassRec *)XtClass(submenu->manager.
	  active_child))->primitive_class.border_unhighlight))
	  (submenu->manager.active_child);
      }
      else if (XmIsGadget(submenu->manager.active_child))
      {
	  (*(((XmGadgetClassRec *)XtClass(submenu->manager.
	  active_child))->gadget_class.border_unhighlight))
	  (submenu->manager.active_child);
      }
      /* update internal focus state */
      _XmClearFocusPath((Widget) submenu);

      /* Clear the Intrinsic focus from the tear off widget hierarchy.
       * Necessary to remove the FocusChangeCallback from the active item.
       */
      XtSetKeyboardFocus(shell, NULL);
   }

   if (XmIsMenuShell(shell))
   {
      /* Shared menupanes require extra manipulation.  We gotta be able
       * to optimize this when there's more time.
       */
      if ((((ShellWidget)shell)->composite.num_children) > 1)
         XUnmapWindow(XtDisplay(submenu), XtWindow(submenu));

      _XmDestroyTearOffShell(RC_ParentShell(submenu));

      /* remove orphan destroy callback from postFromWidget */
      XtRemoveCallback(submenu->row_column.tear_off_lastSelectToplevel,
	 XtNdestroyCallback, (XtCallbackProc)DismissOnPostedFromDestroy,
	 (XtPointer) RC_ParentShell(submenu));
   }
   else	/* toplevel shell! */
   {
      /* Shared menupanes require extra manipulation.  We gotta be able
       * to optimize this when there's more time.
       */
      if ((((ShellWidget)RC_ParentShell(submenu))->composite.num_children) > 1)
         XUnmapWindow(XtDisplay(submenu), XtWindow(submenu));


      _XmDestroyTearOffShell(shell);
      if (submenu)
      {
	 XtParent(submenu) = RC_ParentShell(submenu);
         XReparentWindow(XtDisplay(submenu), XtWindow(submenu),
	    XtWindow(XtParent(submenu)), XtX(submenu), XtY(submenu));
         submenu->core.mapped_when_managed = False;
         submenu->core.managed = False;
         if (RC_TearOffControl(submenu))
          XtManageChild(RC_TearOffControl(submenu));
      }

      /* Only Call the callbacks if we're not popped up (parent is not a
       * menushell).  Popping up the shell caused the unmap & deactivate
       * callbacks to be called already.
       */
      _XmCallRowColumnUnmapCallback((Widget)submenu, NULL);
      CallTearOffMenuDeactivateCallback((Widget)submenu, (XEvent *)closure,
	 DESTROY_TEAR_OFF);
      RemoveTearOffEventHandlers ((Widget) submenu);

      /* remove orphan destroy callback from postFromWidget */
      XtRemoveCallback(submenu->row_column.tear_off_lastSelectToplevel,
	 XtNdestroyCallback, (XtCallbackProc)DismissOnPostedFromDestroy,
	 (XtPointer) shell);
   }
}

static void
DismissOnPostedFromDestroy(
	Widget w,		/* unused */
	XtPointer clientData,
	XtPointer callData )	/* unused */
{
   _XmDismissTearOff((Widget)clientData, NULL, NULL);
}

#define DEFAULT_TEAR_OFF_TITLE ""
#define TEAR_OFF_TITLE_SUFFIX " Tear-off"
#define TEAR_OFF_CHARSET "ISO8859-1"

void _XmTearOffInitiate(Widget wid, XEvent *event)
{
   enum { XmAWM_DELETE_WINDOW, XmA_MOTIF_WM_HINTS, NUM_ATOMS };
   static char *atom_names[] = { XmIWM_DELETE_WINDOW, _XA_MOTIF_WM_HINTS };

   XmRowColumnWidget submenu = (XmRowColumnWidget)wid;
   Widget cb;
   XmRowColumnWidget rc;
   XmString label_xms;
   unsigned char label_type;
   Arg args[20];
   ShellWidget to_shell;
   Widget toplevel;
   PropMwmHints *rprop = NULL;     /* receive pointer */
   PropMwmHints sprop;          /* send structure */
   Atom atoms[XtNumber(atom_names)];
   Atom actual_type;
   int actual_format;
   unsigned long num_items, bytes_after;
   XEvent newEvent;
   XmMenuState mst = _XmGetMenuState((Widget)wid);
   XtWidgetProc proc;
   Cursor cursor = None;

   if (IsPulldown(submenu))
      cb = RC_CascadeBtn(submenu);
   else
      cb = NULL;

   if (RC_TearOffModel(submenu) == XmTEAR_OFF_DISABLED)
      return;

   /* The submenu must be posted before we allow the tear off action */
   if (!(XmIsMenuShell(XtParent(submenu)) &&
         ((XmMenuShellWidget)XtParent(submenu))->shell.popped_up))
      return;

   if (XmIsRowColumn(wid))
      rc = (XmRowColumnWidget)wid;
   else
      rc = (XmRowColumnWidget) XtParent (wid);
   _XmGetActiveTopLevelMenu((Widget)rc, (Widget *)&rc);

   /* Set up the important event fields for the new position */
   memcpy(&newEvent, event, sizeof(XButtonEvent));

   /* if it's from a BDrag, find the eventual destination */
   if ((event->xbutton.type == ButtonPress) &&
       (event->xbutton.button == 2/*BDrag*/))
   {
      if (!DoPlacement((Widget) submenu, &newEvent))     /* Cancelled! */
      {
         /* restore grabs back to menu and reset menu cursor? */

         /* If the toplevel menu is an option menu, the grabs are attached to
          * the top submenu else leave it as the menubar or popup.
          */
         if (IsOption(rc))
            rc = (XmRowColumnWidget)RC_OptionSubMenu(rc);

         _XmGrabPointer((Widget) rc, True,
            (unsigned int)(ButtonPressMask | ButtonReleaseMask |
            EnterWindowMask | LeaveWindowMask),
            GrabModeSync, GrabModeAsync, None,
            _XmGetMenuCursorByScreen(XtScreen(rc)), CurrentTime);

         _XmGrabKeyboard((Widget)rc, True, GrabModeSync, GrabModeSync,
            CurrentTime);
         XAllowEvents(XtDisplay(rc), AsyncKeyboard, CurrentTime);

         XAllowEvents(XtDisplay(rc), SyncPointer, CurrentTime);

         /* and reset the input focus to the leaf submenu's menushell */
         _XmMenuFocus(XtParent(submenu), XmMENU_MIDDLE, CurrentTime);
         return;
      }
   }
   else
   {
      newEvent.xbutton.x_root = XtX(XtParent(submenu));
      newEvent.xbutton.y_root = XtY(XtParent(submenu));
   }


   /* If the submenu already exists, take it down for a retear */
   _XmDismissTearOff(XtParent(submenu), (XtPointer)event, NULL);

   /* Shared menupanes require extra manipulation.  We gotta be able
    * to optimize this when there's more time.
    */
   if ((((ShellWidget)XtParent(submenu))->composite.num_children) > 1)
      XMapWindow(XtDisplay(submenu), XtWindow(submenu));

   /*
    * Popdown the menu hierarchy!
    */
   /* First save the GetPostedFromWidget */
   if (mst->RC_LastSelectToplevel)
   {
      submenu->row_column.tear_off_lastSelectToplevel =
	  mst->RC_LastSelectToplevel;
   }
   else
      if (RC_TornOff(rc) && RC_TearOffActive(rc))
         submenu->row_column.tear_off_lastSelectToplevel =
            rc->row_column.tear_off_lastSelectToplevel;
      else
      {
	/* Fix CR 7983 - Assigning NULL RC_CascadeBtn causes core dump */
	 if (IsPopup(submenu) && RC_CascadeBtn(submenu))
	    submenu->row_column.tear_off_lastSelectToplevel =
	       RC_CascadeBtn(submenu);
	 else
	    submenu->row_column.tear_off_lastSelectToplevel = (Widget)rc;
      }

   if (!XmIsMenuShell(XtParent(rc)))    /* MenuBar or TearOff */
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
         menu_shell_class.popdownEveryone))(RC_PopupPosted(rc), event, NULL,
            NULL);
   else
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
         menu_shell_class.popdownEveryone))(XtParent(rc), event, NULL, NULL);

   _XmSetInDragMode((Widget) rc, False);

   /* popdownEveryone() calls popdownDone() which will call MenuDisarm() for
    * each pane.  We need to take care of non-popup toplevel menus (bar/option).
    */
   (*(((XmRowColumnClassRec *)XtClass(rc))->row_column_class.
      menuProcedures)) (XmMENU_DISARM, (Widget) rc);

   _XmMenuFocus( (Widget) rc, XmMENU_END, CurrentTime);
   XtUngrabPointer( (Widget) rc, CurrentTime);

   XtUnmanageChild(RC_TearOffControl(submenu));

   /* Use the toplevel application shell for the parent of the new transient
    * shell.  This way, the submenu won't inadvertently be destroyed on
    * associated-widget's shell destruction.  We'll make the connection to
    * associate-widget with XmNtransientFor.
    */
   for (toplevel = wid; XtParent(toplevel); )
      toplevel = XtParent(toplevel);

   XtSetArg(args[0], XmNdeleteResponse, XmDO_NOTHING);
   /* system & title */
   XtSetArg(args[1], XmNmwmDecorations,
      MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU);
   XtSetArg(args[2], XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE);
   /* need shell resize for pulldown to resize correctly when reparenting
    * back without tear off control managed.
    */
   XtSetArg(args[3], XmNallowShellResize, True);
   if (XmIsRowColumn(submenu->row_column.tear_off_lastSelectToplevel) &&
       IsPopup(submenu->row_column.tear_off_lastSelectToplevel))
   {
      XtSetArg(args[4], XmNtransientFor,
	 _XmFindTopMostShell(RC_CascadeBtn(
	    submenu->row_column.tear_off_lastSelectToplevel)));
   }
   else
      XtSetArg(args[4], XmNtransientFor,
	 _XmFindTopMostShell(submenu->row_column.tear_off_lastSelectToplevel));
   /* Sorry, still a menu - explicit mode only so focus doesn't screw up */
   XtSetArg(args[5], XmNkeyboardFocusPolicy, XmEXPLICIT);
   /* Start Fix CR 5459
    * It is important to set the shell's visual information (visual, colormap,
    * depth) to match the menu whose parent it is becoming.  If you fail to do
    * so, then when the user brings up a second copy of a menu that is already
    * posted, a BADMATCH error occurs.
    */
   XtSetArg(args[6], XmNvisual,
	    ((XmMenuShellWidget)(XtParent(wid)))->shell.visual);
   XtSetArg(args[7], XmNcolormap,
	    ((XmMenuShellWidget)(XtParent(wid)))->core.colormap);
   XtSetArg(args[8], XmNdepth,
	    ((XmMenuShellWidget)(XtParent(wid)))->core.depth);
   /* End Fix CR 5459 */
   to_shell = (ShellWidget)XtCreatePopupShell(DEFAULT_TEAR_OFF_TITLE,
      transientShellWidgetClass,
      toplevel, args, 9);

   if (RC_TearOffTitle(submenu) != NULL)
     XmeSetWMShellTitle(RC_TearOffTitle(submenu), (Widget) to_shell);
   else if (cb) {
     Widget lwid, mwid;

     /* If the top menu of the active menu hierarchy is an option menu,
      * use the option menu's label for the name of the shell.
      */
     mwid = XmGetPostedFromWidget(XtParent(cb));
     if (mwid && IsOption(mwid))
       lwid = XmOptionLabelGadget(mwid);
     else
       lwid = cb;

     XtSetArg(args[0], XmNlabelType, &label_type);
     XtGetValues((Widget)lwid, args, 1);

     /* better be a compound string! */
     if (label_type == XmSTRING || label_type == XmPIXMAP_AND_STRING)
       {
	 XmString title_xms, suffix_xms;

	 XtSetArg(args[0], XmNlabelString, &label_xms);
	 XtGetValues((Widget)lwid, args, 1);

 	 suffix_xms = XmStringCreate(TEAR_OFF_TITLE_SUFFIX, TEAR_OFF_CHARSET);
 	 title_xms = XmStringConcatAndFree(label_xms, suffix_xms);

 	 XmeSetWMShellTitle(title_xms, (Widget)to_shell);

 	 XmStringFree(title_xms);
       }
   }

   assert(XtNumber(atom_names) == NUM_ATOMS);
   XInternAtoms(XtDisplay(to_shell), atom_names, XtNumber(atom_names), FALSE,
		atoms);

   XmAddWMProtocolCallback((Widget)to_shell, atoms[XmAWM_DELETE_WINDOW],
      _XmDismissTearOff, NULL);

   /* Add internal destroy callback to postFromWidget to eliminate orphan tear
    * off menus.
    */
   XtAddCallback(submenu->row_column.tear_off_lastSelectToplevel,
      XtNdestroyCallback, (XtCallbackProc)DismissOnPostedFromDestroy,
      (XtPointer) to_shell);

   RC_ParentShell(submenu) = XtParent(submenu);
   XtParent(submenu) = (Widget)to_shell;

   /* Needs to be set before the user gets a callback */
   RC_SetTornOff(submenu, TRUE);
   RC_SetTearOffActive(submenu, TRUE);

   _XmAddTearOffEventHandlers ((Widget) submenu);
   CallTearOffMenuActivateCallback((Widget)submenu, event,
      CREATE_TEAR_OFF);
   _XmCallRowColumnMapCallback((Widget)submenu, event);

   /* To get Traversal: _XmGetManagedInfo() to work correctly */
   submenu->core.mapped_when_managed = True;
   XtManageChild((Widget)submenu);

   /* Insert submenu into the new toplevel shell */
   _XmProcessLock();
   proc = ((TransientShellWidgetClass)transientShellWidgetClass)->
		composite_class.insert_child;
   _XmProcessUnlock();
   (*proc)((Widget)submenu);

   /* Quick!  Configure the size (and location) of the shell before managing
    * so submenu doesn't get resized.
    */
   XmeConfigureObject((Widget) to_shell,
      newEvent.xbutton.x_root, newEvent.xbutton.y_root,
      XtWidth(submenu), XtHeight(submenu), XtBorderWidth(to_shell));

   /* Call change_managed routine to set up focus info */
   _XmProcessLock();
   proc = ((TransientShellWidgetClass)transientShellWidgetClass)->
		composite_class.change_managed;
   _XmProcessUnlock();
   (*proc)((Widget)to_shell);

   XtRealizeWidget((Widget)to_shell);

   /* Wait until after to_shell realize to set the focus */
   XmProcessTraversal((Widget)submenu, XmTRAVERSE_CURRENT);

   XGetWindowProperty(XtDisplay(to_shell), XtWindow(to_shell),
		      atoms[XmA_MOTIF_WM_HINTS], 0,
		      PROP_MWM_HINTS_ELEMENTS, False,
		      atoms[XmA_MOTIF_WM_HINTS],
		      &actual_type, &actual_format, &num_items, &bytes_after,
		      (unsigned char **)&rprop);

   if ((actual_type != atoms[XmA_MOTIF_WM_HINTS]) ||
       (actual_format != 32) ||
       (num_items < PROP_MOTIF_WM_INFO_ELEMENTS))
   {
       if (rprop != NULL) XFree((char *)rprop);
   }
   else
   {
      memset(&sprop, 0, sizeof sprop);
      /* Fix for 9346,  use sizeof(long) to calculate total
	 size of block from get property */
      memcpy(&sprop, rprop, (size_t)sizeof(long) * num_items);
      if (rprop != NULL) XFree((char *)rprop);

      sprop.flags |= MWM_HINTS_STATUS;
      sprop.status |= MWM_TEAROFF_WINDOW;
      XChangeProperty(XtDisplay(to_shell), XtWindow(to_shell),
		      atoms[XmA_MOTIF_WM_HINTS], atoms[XmA_MOTIF_WM_HINTS], 32,
		      PropModeReplace,
		      (unsigned char *) &sprop, PROP_MWM_HINTS_ELEMENTS);
   }

   /* Set our menu cursor */
   if ((cursor = _XmGetMenuCursorByScreen(XtScreen(to_shell))) != None)
       XDefineCursor(XtDisplay(to_shell), XtWindow(to_shell), cursor);

   /* Notify the server of the change */
   XReparentWindow(XtDisplay(to_shell), XtWindow(submenu), XtWindow(to_shell), 0, 0);
   XtPopup((Widget)to_shell, XtGrabNone);
   RC_SetArmed (submenu, False);
   RC_SetTearOffDirty(submenu, False);
}

Boolean
_XmIsTearOffShellDescendant(
	Widget wid )
{
   XmRowColumnWidget rc = (XmRowColumnWidget)wid;
   Widget cb;

   while (rc && (IsPulldown(rc) || IsPopup(rc)) && XtIsShell(XtParent(rc)))
   {
      if (RC_TearOffActive(rc))
         return(True);

      /* Popup is the top! "cascadeBtn" is postFromWidget! */
      if (IsPopup(rc))
	 break;

      if (!(cb = RC_CascadeBtn(rc)))
         break;
      rc = (XmRowColumnWidget)XtParent(cb);
   }

   return (False);
}

void
_XmLowerTearOffObscuringPoppingDownPanes(
        Widget ancestor,
	Widget tearOff )
{
   XRectangle tearOff_rect, intersect_rect;
   ShellWidget shell;

   _XmSetRect(&tearOff_rect, tearOff);
   if (IsBar(ancestor) || IsOption(ancestor))
   {
      if ((shell = (ShellWidget)RC_PopupPosted(ancestor)) != NULL)
	 ancestor = shell->composite.children[0];
   }

   while (ancestor && (IsPulldown(ancestor) || IsPopup(ancestor)))
   {
      if (_XmIntersectRect( &tearOff_rect, ancestor, &intersect_rect ))
      {
 	 XtUnmapWidget(XtParent(ancestor));
	 RC_SetTearOffDirty(tearOff, True);
      }
      if ((shell = (ShellWidget)RC_PopupPosted(ancestor)) != NULL)
	 ancestor = shell->composite.children[0];
      else
	 break;
   }
   if (RC_TearOffDirty(tearOff))
      XFlush(XtDisplay(ancestor));
}

void
_XmRestoreExcludedTearOffToToplevelShell(
        Widget wid,
	XEvent *event )
{
   int i;
   Widget pane;
   XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(wid));
   XmExcludedParentPaneRec *excPP =
	&(((XmDisplayInfo *)(dd->display.displayInfo))->excParentPane);

   for(i=0; i < excPP->num_panes; i++)
   {
      if ((pane = (excPP->pane)[i]) != NULL)
      {
	 /* Reset to NULL first so that _XmRestoreTearOffToToplevelShell()
	  * doesn't prematurely abort.
	  */
	 (excPP->pane)[i] = NULL;
	 _XmRestoreTearOffToToplevelShell(pane, event);
      }
      else
	 break;
   }
   excPP->num_panes = 0;
}

/*
 * The menupane was just posted, it's current parent is the original menushell.
 * Reparent the pane back to the tear off toplevel shell.
 */
void
_XmRestoreTearOffToToplevelShell(
        Widget wid,
	XEvent *event )
{
   XmRowColumnWidget rowcol = (XmRowColumnWidget)wid;
   XtGeometryResult answer;
   Dimension almostWidth, almostHeight;
   int i;
   XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(wid));
   XmExcludedParentPaneRec *excPP =
	&(((XmDisplayInfo *)(dd->display.displayInfo))->excParentPane);

   for(i=0; i < excPP->num_panes; i++)
      if ((Widget)rowcol == (excPP->pane)[i])
	 return;

   if (RC_TornOff(rowcol) && !RC_TearOffActive(rowcol))
   {
      ShellWidget shell;

      /* Unmanage the toc before reparenting to preserve the focus item. */
      XtUnmanageChild(RC_TearOffControl(rowcol));

      /* In case we're dealing with a shared menushell, the rowcol is kept
       * managed.  We need to reforce the pane to be managed so that the
       * pane's geometry is recalculated.  This allows the tear off to be
       * forced larger by mwm so that the title is not clipped.  It's
       * done here to minimize the lag time/flashing.
       */
      XtUnmanageChild((Widget)rowcol);

      /* swap parents to the toplevel shell */
      shell = (ShellWidget)XtParent(rowcol);
      XtParent(rowcol) = RC_ParentShell(rowcol);
      RC_ParentShell(rowcol) = (Widget)shell;
      RC_SetTearOffActive(rowcol, TRUE);

      /* Sync up the server */
      XReparentWindow(XtDisplay(shell), XtWindow(rowcol),
         XtWindow(XtParent(rowcol)), 0, 0);

      XFlush(XtDisplay(shell));

      if (XtParent(rowcol)->core.background_pixmap != XtUnspecifiedPixmap)
      {
         XFreePixmap(XtDisplay(XtParent(rowcol)),
            XtParent(rowcol)->core.background_pixmap);
         XtParent(rowcol)->core.background_pixmap = XtUnspecifiedPixmap;
      }

      /* The menupost that reparented the pane back to the menushell has
       * wiped out the active_child.  We need to restore it.
       * Check this out if FocusPolicy == XmPOINTER!
       */
      rowcol->manager.active_child = _XmGetActiveItem((Widget)rowcol);

      _XmAddTearOffEventHandlers ((Widget) rowcol);

      /* Restore lastSelectToplevel as if the (torn) menu is posted */
      if (IsPulldown(rowcol))
	 rowcol->row_column.lastSelectToplevel =
	    rowcol->row_column.tear_off_lastSelectToplevel;
      else /* IsPopup */
	 RC_CascadeBtn(rowcol) =
	    rowcol->row_column.tear_off_lastSelectToplevel;

      CallTearOffMenuActivateCallback((Widget)rowcol, event,
	 RESTORE_TEAR_OFF_TO_TOPLEVEL_SHELL);
      _XmCallRowColumnMapCallback((Widget)rowcol, event);

      /*
       * In case the rowcolumn's geometry has changed, make a resize
       * request to the top level shell so that it can changed.  All
       * geometry requests were handled through the menushell and the
       * top level shell was left unchanged.
       */
      answer = XtMakeResizeRequest (XtParent(rowcol), XtWidth(rowcol),
				    XtHeight(rowcol), &almostWidth,
				    &almostHeight);

      if (answer == XtGeometryAlmost)
	  answer = XtMakeResizeRequest (XtParent(rowcol), almostWidth,
					almostHeight, NULL, NULL);



      /* As in _XmTearOffInitiate(), To get Traversal: _XmGetManagedInfo()
       * to work correctly.
       */
      rowcol->core.mapped_when_managed = True;
      XtManageChild((Widget)rowcol);

      /* rehighlight the previous focus item */
      XmProcessTraversal(rowcol->row_column.tear_off_focus_item,
	 XmTRAVERSE_CURRENT);
   }
}

void
_XmRestoreTearOffToMenuShell(
        Widget wid,
	XEvent *event )
{
   XmRowColumnWidget submenu = (XmRowColumnWidget) wid;
   XmMenuState mst = _XmGetMenuState((Widget)wid);
   XtExposeProc expose;
   Boolean wasDirty = False;

   if (RC_TornOff(submenu) && RC_TearOffActive(submenu))
   {
      ShellWidget shell;
      GC gc;
      XGCValues values;
      int i;
      Widget child;

      /* If the pane was previously obscured, it may require redrawing
       * before taking a pixmap snapshot.
       * Note: event could be NULL on right arrow browse through menubar back
       *   to this submenu.
       */
      if (RC_TearOffDirty(submenu) ||
	  (event && (event->type == ButtonPress) &&
	   (event->xbutton.time == mst->RC_ReplayInfo.time) &&
	   (mst->RC_ReplayInfo.toplevel_menu == (Widget)submenu)) ||
	  XmeFocusIsInShell((Widget)submenu))
      {
	 RC_SetTearOffDirty(submenu, False);
         wasDirty = True;

	 /* First make sure that the previous active child is unhighlighted.
	  * In the tear off's inactive state, no children should be
	  * highlighted.
	  */
	 if ((child = submenu->manager.active_child) != NULL)
	 {
	    if (XtIsWidget(child))
	       (*(((XmPrimitiveClassRec *)XtClass(child))->
		  primitive_class.border_unhighlight))(child);
	    else
	       (*(((XmGadgetClassRec *)XtClass(child))->
		  gadget_class.border_unhighlight))(child);
	 }

	 /* Redraw the submenu and its gadgets */
	 _XmProcessLock();
	 expose = XtClass(submenu)->core_class.expose;
	 _XmProcessUnlock();
	 if (expose)
	    (*expose)((Widget)submenu, NULL, NULL);

	 /* Redraw the submenu's widgets */
	 for (i=0; i<submenu->composite.num_children; i++)
	 {
	    child = submenu->composite.children[i];
	    if (XtIsWidget(child))
	    {
	       _XmProcessLock();
	       expose = XtClass(child)->core_class.expose;
	       _XmProcessUnlock();
	       if (expose)
		  (*expose)(child, event, NULL);
	    }
	 }
	 XFlush(XtDisplay(submenu));
      }

      shell =  (ShellWidget)XtParent(submenu);      /* this is a toplevel */

      /* Save away current focus item.  Then clear the focus path so that
       * the XmProcessTraversal() in _XmRestoreTOToTopLevelShell() re-
       * highlights the focus_item.
       */
      submenu->row_column.tear_off_focus_item =
	 XmGetFocusWidget((Widget)submenu);
      _XmClearFocusPath((Widget) submenu);

      /* Get a pixmap holder first! */

      values.graphics_exposures = False;
      values.subwindow_mode = IncludeInferiors;
      gc = XtGetGC((Widget) shell, GCGraphicsExposures | GCSubwindowMode,
		   &values);

      /* Fix for CR #4855, use of default depth, DRand 6/4/92 */
      shell->core.background_pixmap = XCreatePixmap(XtDisplay(shell),
	 RootWindowOfScreen(XtScreen(shell)),
	 shell->core.width, shell->core.height,
         shell->core.depth);
      /* End of Fix #4855 */

      XCopyArea(XtDisplay(shell), XtWindow(submenu),
	 shell->core.background_pixmap, gc, 0, 0,
	 shell->core.width, shell->core.height,
	 0, 0);

      XtReleaseGC((Widget) shell, gc);

      XtParent(submenu) = RC_ParentShell(submenu);
      RC_ParentShell(submenu) = (Widget)shell;
      RC_SetTearOffActive(submenu, False);
      if (wasDirty)
         XtMapWidget(XtParent(submenu));

      submenu->core.mapped_when_managed = False;
      submenu->core.managed = False;

      /* Sync up the server */
      XSetWindowBackgroundPixmap(XtDisplay(shell), XtWindow(shell),
	 shell->core.background_pixmap);

      XReparentWindow(XtDisplay(shell), XtWindow(submenu),
	 XtWindow(XtParent(submenu)), XtX(submenu), XtY(submenu));

      XtManageChild(RC_TearOffControl(submenu));

      /* The traversal graph needs to be zeroed/freed when reparenting back
       * to a MenuShell.  This handles the case of shared/torn panes where
       * the pane moves from shell(context1) -> torn-shell -> shell(context2).
       * Shell(context2) receives the TravGraph from shell(context1).
       */
       /* even if only one pane, sensitivity of menu items may have changed, so
       ** wind up forcing a reevaluation of the traversing graph
       */
       if (submenu->row_column.postFromCount >= 1)
         _XmResetTravGraph(submenu->core.parent);

      _XmCallRowColumnUnmapCallback((Widget)submenu, event);
      CallTearOffMenuDeactivateCallback((Widget)submenu, event,
	 RESTORE_TEAR_OFF_TO_MENUSHELL);
      RemoveTearOffEventHandlers ((Widget) submenu);
   }
}
