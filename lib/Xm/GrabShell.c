/* $XConsortium: GrabShell.c /main/9 1996/08/15 17:12:04 pascale $ */
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


#include "XmI.h"
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/cursorfont.h>
#include <Xm/DrawP.h>
#include <Xm/GrabShellP.h>
#include <Xm/MenuUtilP.h>
#include <Xm/ScreenP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VendorSEP.h>
#include <Xm/VendorSP.h>
#include "ColorI.h"
#include "MenuShellI.h"
#include "PixConvI.h"
#include "UniqueEvnI.h"

/* Warning messages */

#define default_translations	_XmGrabShell_translations

#define Events	(EnterWindowMask | LeaveWindowMask | \
		 ButtonPressMask | ButtonReleaseMask)

/********    Static Function Declarations    ********/
static void MouseWheel (Widget grabshell,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);
static void BtnUp (Widget grabshell,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);
static void BtnDown (Widget grabshell,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);
static void Popdown (Widget grabshell,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);
static void ClassPartInitialize (WidgetClass wc);
static void Initialize (Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static Boolean SetValues (Widget cw,
			  Widget rw,
			  Widget nw,
			  ArgList args,
			  Cardinal *num_args);
static void Resize (Widget wid);
static void ChangeManaged (Widget w);
static XtGeometryResult GeometryManager(
                        Widget wid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void Destroy (Widget wid);
static void MapNotifyHandler(Widget shell, XtPointer client_data,
			     XEvent *, Boolean *);
static void _XmFastExpose (Widget widget);
static void DrawBorder (Widget widget);
static void DoLayout (Widget gs);
static void GSAllowEvents(Widget gs, int, Time);

static int IgnoreXErrors(Display *, XErrorEvent *);


/********    End Static Function Declarations    ********/

static XtActionsRec actionsList[] =
{
  { "GrabShellBtnDown", BtnDown },
  { "GrabShellBtnUp",   BtnUp },
  { "GrabShellPopdown", Popdown },
  { "GrabShellMouseWheel", MouseWheel }
};

#define Offset(field) (XtOffsetOf(XmGrabShellRec, field))

static XtResource resources[] =
{
  {
    XmNallowShellResize, XmCAllowShellResize, XmRBoolean,
    sizeof(Boolean), Offset(shell.allow_shell_resize),
    XtRImmediate, (XtPointer)TRUE
  },
  {
    XmNbackground, XmCBackground, XmRPixel,
    sizeof (Pixel), Offset(core.background_pixel),
    XmRCallProc, (XtPointer) _XmBackgroundColorDefault
  },
  {
    XmNoverrideRedirect, XmCOverrideRedirect, XmRBoolean,
    sizeof(Boolean), Offset(shell.override_redirect),
    XtRImmediate, (XtPointer)TRUE
  },
  {
    XmNsaveUnder, XmCSaveUnder, XmRBoolean,
    sizeof(Boolean), Offset(shell.save_under),
    XtRImmediate, (XtPointer)FALSE
  },
  {
    XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(grab_shell.shadow_thickness),
    XmRImmediate, (XtPointer)2
  },
  {
    XmNtransient, XmCTransient, XmRBoolean,
    sizeof(Boolean), Offset(wm_shell.transient),
    XtRImmediate, (XtPointer)TRUE
  },
  {
    XmNwaitForWm, XmCWaitForWm, XmRBoolean,
    sizeof(Boolean), Offset(wm_shell.wait_for_wm),
    XtRImmediate, (XtPointer)FALSE
  },
  {
    XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
    sizeof(Pixel), Offset(grab_shell.top_shadow_color),
    XmRCallProc, (XtPointer) _XmTopShadowColorDefault
  },
  {
    XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
    sizeof(Pixmap), Offset(grab_shell.top_shadow_pixmap),
    XmRCallProc, (XtPointer) _XmTopShadowPixmapDefault
  },
  {
    XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
    sizeof(Pixel), Offset(grab_shell.bottom_shadow_color),
    XmRCallProc, (XtPointer) _XmBottomShadowColorDefault
  },
  {
    XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
    sizeof(Pixmap), Offset(grab_shell.bottom_shadow_pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },
  {
    XmNgrabStyle, XmCGrabStyle, XmRInt,
    sizeof(int), Offset(grab_shell.grab_style),
    XmRImmediate, (XtPointer) GrabModeAsync
  },
  {
    XmNownerEvents, XmCOwnerEvents, XmRBoolean,
    sizeof(Boolean), Offset(grab_shell.owner_events),
    XmRImmediate, (XtPointer) FALSE
  }
};
#undef Offset

externaldef(xmgrabshellclassrec) XmGrabShellClassRec xmGrabShellClassRec =
{
  { /* core class fields */
    (WidgetClass) &vendorShellClassRec,	/* superclass		 */
    "XmGrabShell",			/* class_name		 */
    sizeof (XmGrabShellWidgetRec),	/* widget_size		 */
    NULL,				/* class_initialize	 */
    ClassPartInitialize,		/* class_part_initialize */
    FALSE,				/* class_inited		 */
    Initialize,				/* initialize		 */
    (XtArgsProc)NULL,			/* initialize_hook	 */
    XtInheritRealize,			/* realize		 */
    actionsList,			/* actions		 */
    XtNumber(actionsList),		/* num_actions		 */
    resources,				/* resource list	 */
    XtNumber(resources),		/* resource_count	 */
    NULLQUARK,				/* xrm_class		 */
    True,				/* compress_motion	 */
    XtExposeCompressMaximal,		/* compress_exposure	 */
    TRUE,				/* compress_enterleave	 */
    FALSE,				/* visible_interest	 */
    Destroy,				/* destroy		 */
    Resize,				/* resize		 */
    NULL,				/* expose		 */
    SetValues,				/* set_values		 */
    (XtArgsFunc)NULL,			/* set_values_hook	 */
    XtInheritSetValuesAlmost,		/* set_values_almost	 */
    (XtArgsProc)NULL,			/* get_values_hook	 */
    (XtAcceptFocusProc)NULL,		/* accept_focus		 */
    XtVersion,				/* version		 */
    NULL,				/* callback_private	 */
    default_translations,		/* tm_table		 */
    (XtGeometryHandler)NULL,		/* query_geometry	 */
    (XtStringProc)NULL,			/* display_accelerator	 */
    NULL,				/* extension		 */
  },
  { /* composite class fields */
    GeometryManager, 		     	/* geometry_manager	 */
    ChangeManaged,			/* change_managed	 */
    XtInheritInsertChild,		/* insert_child		 */
    XtInheritDeleteChild,		/* delete_child		 */
    NULL,				/* extension		 */
  },
  { /* shell class fields */
    NULL,				/* extension		 */
  },
  { /* wmshell class fields */
    NULL,				/* extension		 */
  },
  { /* vendor shell class fields */
    NULL,				/* extension		 */
  },
  { /* grabshell class fields */
    NULL,				/* extension		 */
  },
};


externaldef(xmgrabshellwidgetclass) WidgetClass xmGrabShellWidgetClass =
   (WidgetClass) &xmGrabShellClassRec;

/* ------------- WIDGET CLASS METHODS ---------- */

/*
 * Initialize()
 */

/*ARGSUSED*/
static void
Initialize(Widget req,		/* unused */
	   Widget new_w,
	   ArgList args,	/* unused */
	   Cardinal *num_args)	/* unused */
{
  XmGrabShellWidget grabsh = (XmGrabShellWidget)new_w;

  XtAddEventHandler(new_w, StructureNotifyMask, False, MapNotifyHandler, NULL);

  grabsh->grab_shell.unpost_time = (Time) -1;
  grabsh->grab_shell.cursor = None;

  grabsh->grab_shell.top_shadow_GC =
    _XmGetPixmapBasedGC (new_w,
			 grabsh->grab_shell.top_shadow_color,
			 grabsh->core.background_pixel,
			 grabsh->grab_shell.top_shadow_pixmap);

  grabsh->grab_shell.bottom_shadow_GC =
    _XmGetPixmapBasedGC (new_w,
			 grabsh->grab_shell.bottom_shadow_color,
			 grabsh->core.background_pixel,
			 grabsh->grab_shell.bottom_shadow_pixmap);

  /* CR 6723:  The BtnUp event may arrive before MapNotify. */
  grabsh->grab_shell.post_time = XtLastTimestampProcessed(XtDisplay(new_w));

  /* CR 9920:  Popdown may be requested before MapNotify. */
  grabsh->grab_shell.mapped = False;

}

/*
 * ClassPartInitialize()
 *	Set up the fast subclassing.
 */

static void
ClassPartInitialize(WidgetClass wc)
{
  _XmFastSubclassInit (wc, XmGRAB_SHELL_BIT);
}

/*
 * SetValues()
 */

/*ARGSUSED*/
static Boolean
SetValues(Widget cw,
	  Widget rw,		/* unused */
	  Widget nw,
	  ArgList args,		/* unused */
	  Cardinal *num_args)	/* unused */
{
  XmGrabShellWidget new_w = (XmGrabShellWidget) nw;
  XmGrabShellWidget old_w = (XmGrabShellWidget) cw;
  Boolean redisplay = FALSE;

  if (old_w->grab_shell.shadow_thickness != new_w->grab_shell.shadow_thickness)
    {
      if (XtIsRealized(nw)) {
	DoLayout(nw);
	redisplay = TRUE;
      }
    }

  if ((old_w->grab_shell.top_shadow_color !=
       new_w->grab_shell.top_shadow_color) ||
      (old_w->grab_shell.top_shadow_pixmap !=
       new_w->grab_shell.top_shadow_pixmap))
    {
      XtReleaseGC (nw, new_w->grab_shell.top_shadow_GC);
      new_w->grab_shell.top_shadow_GC =
	_XmGetPixmapBasedGC (nw,
			     new_w->grab_shell.top_shadow_color,
			     new_w->core.background_pixel,
			     new_w->grab_shell.top_shadow_pixmap);
      redisplay = TRUE;
    }

  if ((old_w->grab_shell.bottom_shadow_color !=
       new_w->grab_shell.bottom_shadow_color) ||
      (old_w->grab_shell.bottom_shadow_pixmap !=
       new_w->grab_shell.bottom_shadow_pixmap))
    {
      XtReleaseGC (nw, new_w->grab_shell.bottom_shadow_GC);
      new_w->grab_shell.bottom_shadow_GC =
	_XmGetPixmapBasedGC (nw,
			     new_w->grab_shell.bottom_shadow_color,
			     new_w->core.background_pixel,
			     new_w->grab_shell.bottom_shadow_pixmap);
      redisplay = TRUE;
    }

  return redisplay;
}

/*
 * PopupCB()
 *	Grabs.
 */

/*ARGSUSED*/
static void
MapNotifyHandler(Widget shell, XtPointer client_data,
		 XEvent *event, Boolean *cont)
{
  XmGrabShellWidget grabshell = (XmGrabShellWidget)shell;
  Time time;
  XErrorHandler old_handler;

  /* Only handles map events */
  if (event -> type != MapNotify) return;

  /* CR 9920:  Popdown may be called before MapNotify. */
  grabshell->grab_shell.mapped = True;

  if (!(time = XtLastTimestampProcessed(XtDisplay(shell))))
    time = CurrentTime;
  if (grabshell->grab_shell.cursor == None)
    grabshell->grab_shell.cursor =
      XCreateFontCursor (XtDisplay(grabshell), XC_arrow);

  _XmFastExpose(shell);

  (void) XtGrabKeyboard(shell, grabshell -> grab_shell.owner_events,
			grabshell -> grab_shell.grab_style,
			GrabModeAsync, time);

  (void) XtGrabPointer(shell, grabshell -> grab_shell.owner_events,
		       Events,
		       grabshell -> grab_shell.grab_style,
		       GrabModeAsync, None,
		       grabshell->grab_shell.cursor, time);

  GSAllowEvents(shell, SyncPointer, time);

  /* Fix focus to shell */
  XGetInputFocus(XtDisplay(shell), &grabshell->grab_shell.old_focus,
		 &grabshell->grab_shell.old_revert_to);
  old_handler = XSetErrorHandler(IgnoreXErrors);
  XSetInputFocus(XtDisplay(shell), XtWindow(shell), RevertToParent, time);
  XSync(XtDisplay(shell), False);
  XSetErrorHandler(old_handler);
}

static void MouseWheel (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
	XmGrabShellWidget grabshell = (XmGrabShellWidget) w;
	GSAllowEvents(w, SyncPointer, event -> xbutton.time);
}

/*
 * For BtnUp and BtnDown events we need to decide whether to
 * popdown the grabshell.  We "see" these if the user presses
 * outside the shell.
 *
 * To decide,  we call the XmNhasInterestCB to see if our poster
 * wants to handle the event.  If our poster does,  we call
 * XAllowEvents with REPLAY to get the event to the poster,  otherwise
 * we Popdown()
 *
 */

static void
BtnUp (Widget w,
       XEvent *event,
       String *params,
       Cardinal *num_params)
{
  XmGrabShellWidget grabshell = (XmGrabShellWidget) w;
  int delta;

  /* Handle click to post
     we then ignore the event if it occured within the
     click to post time */
  delta = event -> xbutton.time - grabshell -> grab_shell.post_time;
  if (delta <= XtGetMultiClickTime(XtDisplay(w))) {
    GSAllowEvents(w, SyncPointer, event -> xbutton.time);
    return;
  }

  Popdown(w, event, params, num_params);
}

static void
BtnDown (Widget grabshell,
	 XEvent *event,
	 String *params,
	 Cardinal *num_params)
{
  int x, y;
  Window win;

  /* Ignore modal cascade replay of event */
  if (! _XmIsEventUnique(event)) return;

  /* Move to grabshell's coordinate system */
  XTranslateCoordinates(XtDisplay(grabshell), event -> xbutton.window,
			XtWindow(grabshell),
			event -> xbutton.x, event -> xbutton.y,
			&x, &y, &win);

  /* Popdown if outside the shell */
  if (x >= 0 && y >= 0 &&
      x <= XtWidth(grabshell) && y <= XtHeight(grabshell)) {
    GSAllowEvents(grabshell, SyncPointer, event -> xbutton.time);
  } else {
    Popdown(grabshell, event, params, num_params);
  }
}

/*
 * Popdown()
 *	Popdown a GrabShell widget, also flag it's child as unmanaged.
 */

/*ARGSUSED*/
static void
Popdown(Widget shell,
        XEvent *event,		/* unused */
	String *params,
	Cardinal *num_params)
{
  XmScreen screen = (XmScreen) XmGetXmScreen(XtScreen(shell));
  XmGrabShellWidget grabshell = (XmGrabShellWidget)shell;
  Time time;

  /* Record for replay detection */
  if (event && (event->type == ButtonPress || event->type == ButtonRelease)) {
    grabshell->grab_shell.unpost_time = event->xbutton.time;
  }

  if (!(time = XtLastTimestampProcessed(XtDisplay(shell))))
    time = CurrentTime;

  /* CR 9920:  Popdown may be called before MapNotify. */
  if (grabshell->shell.popped_up && grabshell->grab_shell.mapped)
    {
      XErrorHandler old_handler;

      if (screen -> screen.unpostBehavior == XmUNPOST_AND_REPLAY)
	GSAllowEvents(shell, ReplayPointer, event ? event->xbutton.time : time);
      XtUngrabPointer(shell, time);
      XtUngrabKeyboard(shell, time);
      _XmPopdown(shell);

      /* Reset focus to old holder */
      old_handler = XSetErrorHandler(IgnoreXErrors);
      if (time != CurrentTime) time = time - 1; /* Avoid race in wm */
      XSetInputFocus(XtDisplay(shell), grabshell->grab_shell.old_focus,
		     grabshell->grab_shell.old_revert_to, time);
      XSync(XtDisplay(shell), False);
      XSetErrorHandler(old_handler);
    }

  grabshell->grab_shell.mapped = False;
}

/*
 * This only calls allow events if we have a sync grab.
 */
static void
GSAllowEvents(Widget gs, int mode, Time time)
{
  XmGrabShellWidget grabshell = (XmGrabShellWidget) gs;

  if (grabshell -> grab_shell.grab_style == GrabModeSync) {
    XAllowEvents(XtDisplay(gs), mode, time);
  }
}


/*
 * Destroy()
 */

static void
Destroy(Widget widg)
{
  XmGrabShellWidget grabshell = (XmGrabShellWidget) widg;

  if (grabshell->grab_shell.cursor != None)
    XFreeCursor(XtDisplay(widg), grabshell->grab_shell.cursor);
}

/*
 * DoLayout()
 */

static void
DoLayout(Widget wid)
{
  XmGrabShellWidget gs = (XmGrabShellWidget)wid;

  if (XtIsManaged(gs->composite.children[0]))
    {
      Widget childwid = gs->composite.children[0];
      Position offset = (gs->grab_shell.shadow_thickness +
			 childwid->core.border_width);
      int cw = ((int) gs->core.width) - 2 * offset;
      int ch = ((int) gs->core.height) - 2 * offset;
      Dimension childW = MAX(1, cw);
      Dimension childH = MAX(1, ch);

      XmeConfigureObject (childwid, offset, offset,
			  childW, childH, childwid->core.border_width);
    }
}

/************************************************************************
 *
 *  GeometryManager
 *
 ************************************************************************/
/*ARGSUSED*/
static XtGeometryResult
GeometryManager(
	 Widget wid,
	 XtWidgetGeometry *request,
	 XtWidgetGeometry *reply ) /* unused */
{
  XmGrabShellWidget gs = (XmGrabShellWidget) XtParent(wid);
  XtWidgetGeometry modified;
  int bw;
  XtGeometryResult ret_val;

  /* Copy the existing request */
  modified = *request;

  bw = XtBorderWidth(wid);

  /* Add shell's shadow thickness and child's borderwidth */
  modified.width += 2*bw + 2*gs->grab_shell.shadow_thickness;
  modified.height += 2*bw + 2*gs->grab_shell.shadow_thickness;

  _XmProcessLock();
  /* Send to vendor shell for final */
  ret_val = ((VendorShellClassRec *) vendorShellWidgetClass) ->
	 composite_class.geometry_manager(wid,&modified,reply);
  _XmProcessUnlock();
  return ret_val;
}


/*
 * ChangeManaged()
 */

static void
ChangeManaged(Widget wid)
{
  XmGrabShellWidget gs = (XmGrabShellWidget)wid;
  ShellWidget       shell = (ShellWidget)wid;
  Dimension         bw = 0;
  XtWidgetGeometry  pref, mygeom, replygeom;
  XtGeometryResult  result;
  Widget	    child;

  mygeom.request_mode = 0;
  mygeom.width = 0;
  mygeom.height = 0;
  if (gs->composite.num_children)
    {
      child = gs->composite.children[0];
      if (XtIsManaged(child))
	{
	  /* Get child's preferred size */
	  result = XtQueryGeometry(child, NULL, &pref);

	  /* Take whatever they want */
	  if (pref.request_mode & CWWidth)
	    {
	      mygeom.width = pref.width;
	      mygeom.request_mode |=  CWWidth;
	    }

	  if (pref.request_mode & CWHeight)
	    {
	      mygeom.height = pref.height;
	      mygeom.request_mode |=  CWHeight;
	    }

	  if (pref.request_mode & CWBorderWidth)
	    bw = pref.border_width;
	  else
	    bw = child->core.border_width;
	}
    }

  mygeom.width += 2*bw + 2*gs->grab_shell.shadow_thickness;
  mygeom.height += 2*bw + 2*gs->grab_shell.shadow_thickness;

  result = XtMakeGeometryRequest((Widget)shell, &mygeom, &replygeom);
  switch (result)
    {
    case XtGeometryAlmost:
      XtMakeGeometryRequest((Widget)shell, &replygeom, NULL);
      /* fall through. */
    case XtGeometryYes:
      DoLayout(wid);
      break;
    case XtGeometryNo:
    case XtGeometryDone:
      break;
    }
}

/*
 * Resize()
 */

static void
Resize(Widget w)
{
  DoLayout(w);
}

/*
 * When using an override redirect window, it is safe to draw to the
 * window as soon as you have mapped it; you need not wait for exposure
 * events to arrive.  So ... to force shells to post quickly, we will
 * redraw all of the items now, and ignore the exposure events we receive
 * later.
 */

static void
_XmFastExpose(Widget widg)
{
  register int i;
  register Widget child;
  XmGrabShellWidget gs = (XmGrabShellWidget)widg;

  _XmProcessLock();
  (*(XtClass(widg)->core_class.expose)) (widg, NULL, NULL);
  _XmProcessUnlock();

  /* Process each windowed child */
  for (i = 0; i < gs->composite.num_children; i++)
    {
      child = gs->composite.children[i];

      if (XtIsWidget(child) && XtIsManaged(child)) {
        _XmProcessLock();
	(*(XtClass(child)->core_class.expose)) (child, NULL, NULL);
	_XmProcessUnlock();
      }
    }

  XFlush(XtDisplay(widg));
  DrawBorder(widg);
}

/*
 * DrawBorder()
 */

static void
DrawBorder(Widget widg)
{
  XmGrabShellWidget gs = (XmGrabShellWidget)widg;
  int offset = 0;

  XmeDrawShadows(XtDisplay(widg), XtWindow(widg),
		 gs->grab_shell.top_shadow_GC,
		 gs->grab_shell.bottom_shadow_GC,
		 offset, offset,
		 XtWidth(widg) - 2 * offset,
		 XtHeight(widg) - 2 * offset,
		 gs->grab_shell.shadow_thickness,
		 XmSHADOW_OUT);
}

/*
 * IgnoreXErrors()
 *	An XErrorHandler that smothers errors.
 */

/*ARGSUSED*/
static int
IgnoreXErrors(Display *dpy,	/* unused */
	      XErrorEvent *event) /* unused */
{
  return 0;
}

/*******************
 * Public Routines *
 *******************/

Widget
XmCreateGrabShell(Widget parent,
		  char *name,
		  ArgList al,
		  Cardinal ac)
{
  return XtCreatePopupShell(name, xmGrabShellWidgetClass, parent, al, ac);
}

