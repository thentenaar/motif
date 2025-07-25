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
static char rcsid[] = "$TOG: XmIm.c /main/28 1997/10/13 14:57:31 cshi $"
#endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/VendorSEP.h>
#include <Xm/VendorSP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "BaseClassI.h"
#include "MessagesI.h"
#include "XmI.h"
#include "XmImI.h"
#include <X11/Xlib.h>

#if !HAVE_XICPROC
typedef Bool (*XICProc)( XIC, XPointer, XPointer);
#endif

/* Data structures:
 *	While multiple XIMs are not currently supported, some thought
 * was given to how they might be implemented.  Currently both
 * XmImDisplayInfo and XmImShellInfo contain per-XIM fields.  Also the
 * locale and XmNinputMethod are implicit in the XmImDisplayInfo.
 * The back-pointer for the original source of shared XICs is perhaps
 * overly general, but will ease XmPER_MANAGER sharing if implemented.
 *
 * If an XIC is shared among several widgets, all will reference a
 * single XmImXICInfo.
 */

typedef struct _XmImRefRec {
  Cardinal	num_refs;	/* Number of referencing widgets. */
  Cardinal	max_refs;	/* Maximum length of refs array. */
  Widget*	refs;		/* Array of referencing widgets. */
  XtPointer     **callbacks;
} XmImRefRec, *XmImRefInfo;

typedef struct _PreeditBufferRec {
  unsigned short length;
  wchar_t 	 *text;
  XIMFeedback  	 *feedback;
  int 		 caret;
  XIMCaretStyle	 style;
} PreeditBufferRec, *PreeditBuffer;

typedef struct _XmImXICRec {
  struct _XmImXICRec *next;	/* Links all have the same XIM. */
  XIC		xic;		/* The XIC. */
  Window	focus_window;	/* Cached information about the XIC. */
  XIMStyle	input_style;	/* ...ditto... */
  int		status_width;	/* ...ditto... */
  int		preedit_width;	/* ...ditto... */
  int		sp_height;	/* ...ditto... */
  Boolean	has_focus;	/* Does this XIC have keyboard focus. */
  Boolean	anonymous;	/* Do we have exclusive rights to this XIC. */
  XmImRefRec	widget_refs;	/* Widgets referencing this XIC. */
  struct _XmImXICRec **source; /* Original source of shared XICs. */
  PreeditBuffer preedit_buffer;
} XmImXICRec, *XmImXICInfo;

typedef struct _XmImShellRec {
  /* per-Shell fields. */
  Widget 	im_widget;	/* Dummy widget to make intrinsics behave. */
  Widget	current_widget;	/* Widget whose visual we're matching. */

  /* per <Shell,XIM> fields. */
  XmImXICInfo	shell_xic;	/* For PER_SHELL sharing policy. */
  XmImXICInfo	iclist;		/* All known XICs for this <XIM,Shell>. */
} XmImShellRec, *XmImShellInfo;

typedef struct {
  /* per-Display fields. */
  XContext	current_xics;	/* Map widget -> current XmImXICInfo. */

  /* per-XIM fields. */
  XIM		xim;		/* The XIM. */
  XIMStyles	*styles;	/* XNQueryInputStyle result. */
  XmImRefRec	shell_refs;	/* Shells referencing this XIM. */
} XmImDisplayRec, *XmImDisplayInfo;


/*
 * Although the current implementation of XVaNestedList is similar
 * to an Xt ArgList, this is not guaranteed by the spec.  The only
 * approved interface for creating XVaNestedLists takes pairs of
 * (char*, XPointer) paramters.
 */
typedef struct {
  char    *name;
  XPointer value;
} VaArg;

typedef struct {
  Cardinal count;
  Cardinal max;
  VaArg   *args;
} VaArgListRec, *VaArgList;


/********    Static Function Declarations    ********/

static int add_sp(String name,
		  XPointer value,
		  VaArgList slp,
		  VaArgList plp,
		  VaArgList vlp);
static int add_p(String name,
		 XPointer value,
		 VaArgList slp,
		 VaArgList plp,
		 VaArgList vlp);
static int add_fs(String name,
		  XPointer value,
		  VaArgList slp,
		  VaArgList plp,
		  VaArgList vlp);
static int add_bgpxmp(String name,
		      XPointer value,
		      VaArgList slp,
		      VaArgList plp,
		      VaArgList vlp);

static XIMStyle check_style(XIMStyles *styles,
			    XIMStyle preedit_style,
			    XIMStyle status_style);
static int ImGetGeo(Widget vw,
		    XmImXICInfo this_icp );
static void ImSetGeo(Widget vw,
		     XmImXICInfo this_icp );
static void ImGeoReq(Widget vw);
static XFontSet extract_fontset(XmFontList fl);
static XmImDisplayInfo get_xim_info(Widget w);
static XtPointer* get_im_info_ptr(Widget w,
				  Boolean create);
static XmImShellInfo get_im_info(Widget w,
				 Boolean create);
static void draw_separator(Widget vw);
static void null_proc(Widget w,
		      XtPointer ptr,
		      XEvent *ev,
		      Boolean *b);
static void ImCountVaList(va_list var,
			  int *total_count);
static ArgList ImCreateArgList(va_list var,
			       int total_count);

static XmImXICInfo create_xic_info(Widget	   shell,
				   XmImDisplayInfo xim_info,
				   XmImShellInfo   im_info,
                                   XmInputPolicy   input_policy);
static XmImXICInfo recreate_xic_info(XIC		xic,
                                     Widget		shell,
				     XmImDisplayInfo xim_info,
				     XmImShellInfo   im_info);
static void set_values(Widget w,
		       ArgList args,
		       Cardinal num_args,
                       XmInputPolicy policy);

static XmImXICInfo get_current_xic(XmImDisplayInfo xim_info,
				   Widget 	   widget);
static void set_current_xic(XmImXICInfo     xic_info,
			    XmImDisplayInfo xim_info,
			    Widget 	    widget);
static void unset_current_xic(XmImXICInfo     xic_info,
			      XmImShellInfo   im_info,
			      XmImDisplayInfo xim_info,
			      Widget 	      widget);

static Cardinal add_ref(XmImRefInfo refs,
			Widget      widget);
static Cardinal remove_ref(XmImRefInfo refs,
			   Widget      widget);

static XVaNestedList VaCopy(VaArgList   list);
static void VaSetArg(VaArgList   list,
		     char       *name,
		     XPointer    value);

static int ImPreeditStartCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditDoneCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditDrawCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditCaretCallback(XIC xic,
				   XPointer client_data,
				   XPointer call_data);
static void ImFreePreeditBuffer(PreeditBuffer pb);
static void set_callback_values(Widget w,
                    String name,
                    XIMCallback *value,
                    VaArgList vlp,
                    XmInputPolicy input_policy);
static void regist_real_callback(Widget w,
                     XIMProc call,
                     int swc);
static XICProc get_real_callback(Widget w,
                  int swc,
		  Widget *real_widget);
static void move_preedit_string(XmImXICInfo icp,
			  Widget wfrom,
			  Widget wto);

     /********    End Static Function Declarations    ********/


typedef int (*XmImResLProc)(String, XPointer,
			   VaArgList, VaArgList, VaArgList);


typedef struct {
  String xmstring;
  String xstring;
  XrmName xrmname;
  XmImResLProc proc;
} XmImResListRec;

static XmImResListRec XmImResList[] = {
  {XmNbackground,       XNBackground,       NULLQUARK, add_sp},
  {XmNforeground,       XNForeground,       NULLQUARK, add_sp},
  {XmNbackgroundPixmap, XNBackgroundPixmap, NULLQUARK, add_bgpxmp},
  {XmNspotLocation,     XNSpotLocation,     NULLQUARK, add_p},
  {XmNfontList,         XNFontSet,          NULLQUARK, add_fs},
  {XmNrenderTable,      XNFontSet,          NULLQUARK, add_fs},
  {XmNlineSpace,        XNLineSpace,        NULLQUARK, add_sp},
  {XmNarea,             XNArea,             NULLQUARK, add_p},
  {XmNpreeditStartCallback, XNPreeditStartCallback, NULLQUARK, NULL},
  {XmNpreeditDoneCallback,  XNPreeditDoneCallback,  NULLQUARK, NULL},
  {XmNpreeditDrawCallback,  XNPreeditDrawCallback,  NULLQUARK, NULL},
  {XmNpreeditCaretCallback, XNPreeditCaretCallback, NULLQUARK, NULL},
};

#define OVERTHESPOT	"overthespot"
#define OFFTHESPOT	"offthespot"
#define ROOT		"root"
#define ONTHESPOT	"onthespot"

#define PREEDIT_START   0
#define PREEDIT_DONE    1
#define PREEDIT_DRAW    2
#define PREEDIT_CARET   3

#define SEPARATOR_HEIGHT 2

#define GEO_CHG 0x1
#define BG_CHG  0x2

#define MSG1	_XmMMsgXmIm_0000

void
XmImRegister(Widget w,
	     unsigned int reserved) /* unused */
{
  Widget p;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;
  XmInputPolicy input_policy = XmINHERIT_POLICY;

  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* Find the enclosing shell. */
  p = XtParent(w);
  while (!XtIsShell(p))
    p = XtParent(p);

  /* Lookup or create per-shell IM info and an XIM. */
  if (((xim_info = get_xim_info(p)) == NULL) ||
      (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(p, True)) == NULL) {
    _XmAppUnlock(app);
    return;
  }

  /* Check that this widget doesn't already have a current XIC. */
  if (get_current_xic(xim_info, w) != NULL) {
    _XmAppUnlock(app);
    return;
  }

  /* See if this widget will be sharing an existing XIC. */
  XtVaGetValues(p, XmNinputPolicy, &input_policy, NULL);
  switch (input_policy)
    {
    case XmPER_SHELL:
      if (im_info->shell_xic == NULL)
	(void) create_xic_info(p, xim_info, im_info, input_policy);
      set_current_xic(im_info->shell_xic, xim_info, w);
      break;

    case XmPER_WIDGET:
      set_current_xic(create_xic_info(p, xim_info, im_info, input_policy),
		      xim_info, w);
      break;

    case XmINHERIT_POLICY:
      break;

    default:
      assert(False);
    }
  _XmAppUnlock(app);
}

void
XmImUnregister(Widget w)
{
  register XmImDisplayInfo xim_info;
  register XmImShellInfo im_info;
  register XmImXICInfo xic_info;
  XtAppContext app;

  /* Punt if insufficient information was provided. */
  if (w == NULL)
    return;

  app = XtWidgetToApplicationContext(w);
  _XmAppLock(app);
  /* Locate this record. */
  xim_info = get_xim_info(w);
  if ((xic_info = get_current_xic(xim_info, w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(w, False)) == NULL) {
    _XmAppUnlock(app);
    return;
  }

  /* Unregister this record. */
  unset_current_xic(xic_info, im_info, xim_info, w);

  if (im_info->iclist == NULL) {
    Widget vw = XtParent(w);
    while (!XtIsShell(vw)) vw = XtParent(vw);
    ImGeoReq(vw);
  }
  _XmAppUnlock(app);
}

void
XmImSetFocusValues(Widget w,
		   ArgList args,
		   Cardinal num_args)
{
  register XmImXICInfo xic_info;
  Widget p;
  Pixel fg, bg;
  XmFontList fl=NULL;
  XFontSet fs=NULL;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XVaNestedList list;
  Window wind;
  XmInputPolicy input_policy;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);

  if ((xic_info = get_current_xic(get_xim_info(p), w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }

  wind = xic_info->focus_window;
  xic_info->focus_window = XtWindow(w);

  set_values(w, args, num_args, XmINHERIT_POLICY);

  if (wind != XtWindow(w)) {
    /* Safe, since we have a window - so it's no gadget */
    XtVaGetValues(w, XmNbackground, &bg, NULL);
    XtVaGetValues(w, XmNforeground, &fg, NULL);
    XtVaGetValues(w, XmNfontList, &fl, NULL);
    if (fl) fs = extract_fontset(fl);
    if (fs)
      list = XVaCreateNestedList(0,
				 XNBackground, bg,
				 XNForeground, fg,
				 XNFontSet,    fs, NULL);
    else
      list = XVaCreateNestedList(0,
				 XNBackground, bg,
				 XNForeground, fg, NULL);
    XSetICValues(xic_info->xic,
		 XNFocusWindow, XtWindow(w),
		 XNStatusAttributes,  list,
		 XNPreeditAttributes, list,
		 NULL);
    XFree(list);

    if (xic_info->input_style & XIMPreeditCallbacks) {
      XtVaGetValues(p, XmNinputPolicy, &input_policy, NULL);
      if (input_policy == XmPER_SHELL && wind)
	move_preedit_string(xic_info,
			XtWindowToWidget(XtDisplay(w), wind), w);
    }
  }
  XSetICFocus(xic_info->xic);
  xic_info->has_focus = True;

  extData = _XmGetWidgetExtData((Widget)p, XmSHELL_EXTENSION);
  if (extData)
    {
      ve = (XmVendorShellExtObject) extData->widget;

      if (ve->vendor.im_height)
        {
          im_info = (XmImShellInfo)ve->vendor.im_info;
          im_info->current_widget = w;
          XtVaGetValues(w, XmNbackground, &bg, NULL);
          XtVaSetValues(p, XmNbackground, bg, NULL);
          ImGeoReq(p);
          draw_separator(p);
        }
    }
  _XmAppUnlock(app);
}

void
XmImSetValues(Widget w,
	      ArgList args,
	      Cardinal num_args )
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  set_values(w, args, num_args, XmINHERIT_POLICY);
  _XmAppUnlock(app);
}

void
XmImUnsetFocus(Widget w)
{
  register XmImXICInfo xic_info;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if ((xic_info = get_current_xic(get_xim_info(w), w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }

  if (xic_info->xic)
    XUnsetICFocus(xic_info->xic);
  xic_info->has_focus = False;
  _XmAppUnlock(app);
}

XIM
XmImGetXIM(Widget w)
{
  XmImDisplayInfo xim_info;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  xim_info = get_xim_info(w);
  if (xim_info != NULL) {
    _XmAppUnlock(app);
    return xim_info->xim;
  }
  else {
    _XmAppUnlock(app);
    return NULL;
  }
}

void
XmImCloseXIM(Widget w)
{
  XmDisplay xmDisplay;
  XmImDisplayInfo xim_info;
  Widget shell;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  int height, base_height;
  Arg args[1];
  XtWidgetGeometry my_request;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* Allow (xim_info->xim == NULL) so we can reset the "failed" flag. */
  if ((xim_info = get_xim_info(w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }

  /* Remove all references to all XICs */
  while (xim_info->shell_refs.refs != NULL)
    {
      shell = xim_info->shell_refs.refs[0];
      _XmImFreeShellData(shell, get_im_info_ptr(shell, False));
      assert((xim_info->shell_refs.refs == NULL) ||
	     (xim_info->shell_refs.refs[0] != shell));
    }

  shell = w;
  while (!XtIsShell(shell))
    shell = XtParent(shell);

  extData = _XmGetWidgetExtData((Widget)shell, XmSHELL_EXTENSION);
  if (extData) {
    ve = (XmVendorShellExtObject) extData->widget;
    height = ve->vendor.im_height;

    if (height != 0){
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(shell, args, 1);
      if (base_height > 0){
	base_height -= height;
	XtSetArg(args[0], XtNbaseHeight, base_height);
	XtSetValues(shell, args, 1);
      }
      if(!(XtIsRealized(shell)))
	shell->core.height -= height;
      else {
	my_request.height = shell->core.height - height;
	my_request.request_mode = CWHeight;
	XtMakeGeometryRequest(shell, &my_request, NULL);
      }
      ve->vendor.im_height = 0;
    }
  }

  /* Close the XIM. */
  if (xim_info->xim != NULL)
    {
      XCloseIM(xim_info->xim);
      xim_info->xim = NULL;
    }

  XFree(xim_info->styles);
  xim_info->styles = NULL;

  xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  xmDisplay->display.xmim_info = NULL;
  XtFree((char *) xim_info);
  _XmAppUnlock(app);
}

int
XmImMbLookupString(Widget w,
		   XKeyPressedEvent *event,
		   char *buf,
		   int nbytes,
		   KeySym *keysym,
		   int *status )
{
  register XmImXICInfo icp;
  int ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if ((icp = get_current_xic(get_xim_info(w), w)) == NULL ||
      icp->xic == NULL)
    {
      if (status)
	*status = XLookupBoth;
      ret_val = XLookupString(event, buf, nbytes, keysym, 0);
      _XmAppUnlock(app);
      return ret_val;
    }

  ret_val = XmbLookupString( icp->xic, event, buf, nbytes,
				keysym, status );
  _XmAppUnlock(app);
  return ret_val;
}

XIC
XmImGetXIC(Widget 	 w,
	   XmInputPolicy input_policy,
	   ArgList 	 args,
	   Cardinal 	 num_args)
{
  XmImDisplayInfo xim_info;
  XmImShellInfo im_info;
  XmImXICInfo xic_info;
  Widget shell;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  xim_info = get_xim_info(w);
  im_info = get_im_info(w, True);
  xic_info = get_current_xic(xim_info, w);

  if ((xim_info == NULL) || (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return NULL;
  }

  /* Find the enclosing shell. */
  shell = w;
  while (!XtIsShell(shell))
    shell = XtParent(shell);

  /* Resolve the true input policy. */
  if (input_policy == XmINHERIT_POLICY)
    XtVaGetValues(shell, XmNinputPolicy, &input_policy, NULL);

  /* If there is already a current XIC, we may want to unregister it. */
  switch (input_policy)
    {
    case XmPER_SHELL:
      if ((xic_info != NULL) && (im_info->shell_xic != xic_info))
	{
	  unset_current_xic(xic_info, im_info, xim_info, w);
	  xic_info = NULL;
	}
      break;

    case XmPER_WIDGET:
      if (xic_info != NULL)
	{
	  unset_current_xic(xic_info, im_info, xim_info, w);
	  xic_info = NULL;
	}
      break;

    default:
      assert(False);
    }

  /* Register an XIC with the desired input policy. */
  if (xic_info == NULL)
    {
      xic_info = create_xic_info(shell, xim_info, im_info, input_policy);
      set_current_xic(xic_info, xim_info, w);
    }

  /* Set the values, which creates an XIC. */
  set_values(w, args, num_args, input_policy);

  /* Return the current XIC. */
  if (xic_info != NULL) {
    _XmAppUnlock(app);
    return xic_info->xic;
  }
  _XmAppUnlock(app);
  return NULL;
}

XIC
XmImSetXIC(Widget widget,
	   XIC    xic)
{
  XmImDisplayInfo xim_info;
  XmImShellInfo im_info;
  XmImXICInfo xic_info;
  Widget shell;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  xim_info = get_xim_info(widget);
  im_info = get_im_info(widget, True);
  xic_info = get_current_xic(xim_info, widget);
  if ((xim_info == NULL) || (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return NULL;
  }

  /* This may be a simple query. */
  if (xic == NULL)
    {
      /* No XIC is registered for this widget. */
      if (xic_info == NULL) {
	_XmAppUnlock(app);
	return NULL;
      }

      /* Force creation of the XIC. */
      if (xic_info->xic == NULL)
	set_values(widget, NULL, 0, XmINHERIT_POLICY);

      _XmAppUnlock(app);
      return xic_info->xic;
    }

  /* We don't support multiple IMs. */
  if (XIMOfIC(xic) != xim_info->xim) {
    _XmAppUnlock(app);
    return NULL;
  }

  /* Unregister the current XIC. */
  if (xic_info != NULL)
    {
      /* Setting the current XIC to itself is a no-op. */
      if (xic_info->xic == xic) {
	_XmAppUnlock(app);
	return xic;
      }

      unset_current_xic(xic_info, im_info, xim_info, widget);
      xic_info = NULL;
    }

  /* Find the enclosing shell. */
  shell = widget;
  while (!XtIsShell(shell))
    shell = XtParent(shell);

  /* Get or create xic_info for this xic. */
  xic_info = recreate_xic_info(xic, shell, xim_info, im_info);

  /* Make this the current XIC for this widget. */
  set_current_xic(xic_info, xim_info, widget);

  _XmAppUnlock(app);
  return xic;
}

void
XmImFreeXIC(Widget w,
	    XIC    context)
{
  register int index;
  register XmImDisplayInfo xim_info;
  register XmImShellInfo im_info;
  register XmImXICInfo xic_info;
  XtAppContext app;

  /* Punt if insufficient information was provided. */
  if (w == NULL)
    return;

  app = XtWidgetToApplicationContext(w);
  _XmAppLock(app);
  /* Locate this record. */
  xim_info = get_xim_info(w);
  if ((xic_info = get_current_xic(xim_info, w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(w, False)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((context != NULL) && (xic_info->xic != context)) {
    _XmAppUnlock(app);
    return;
  }

  /* Remove all references. */
  index = xic_info->widget_refs.num_refs;
  while (--index >= 0)
    unset_current_xic(xic_info, im_info, xim_info,
		      xic_info->widget_refs.refs[index]);
  _XmAppUnlock(app);
}

/*********************
 * Private Functions *
 *********************/

/* Free a VendorShellExt's im_info field. */
void
_XmImFreeShellData(Widget     widget,
		   XtPointer* data)
{
  XmImShellInfo   im_info;
  XmImDisplayInfo xim_info;
  XmImXICInfo	  xic_info;
  Widget	  reference;

  if ((data == NULL) ||
      (im_info = (XmImShellInfo) *data) == NULL)
    return;

  /* Ignore (xim_info->xim == NULL), since it is immaterial here. */
  xim_info = get_xim_info(widget);
  if (xim_info == NULL)
    return;

  /* Remove any dangling references. */
  while (im_info->iclist != NULL)
    {
      xic_info = im_info->iclist;
      reference = xic_info->widget_refs.refs[0];
      unset_current_xic(xic_info, im_info, xim_info, reference);
      assert((xic_info != im_info->iclist) ||
	     (reference != xic_info->widget_refs.refs[0]));
    }
  assert(im_info->shell_xic == NULL);

  /* Delete the dummy widget. */
  if (im_info->im_widget != NULL &&
      !widget->core.being_destroyed)
    {
      XtDestroyWidget(im_info->im_widget);
      im_info->im_widget = NULL;
    }

  /* Remove this shell as a reference to the XIM. */
  (void) remove_ref(&xim_info->shell_refs, widget);

  /* Delete the data. */
  XtFree((char *) im_info);
  *data = NULL;
}

void
_XmImChangeManaged(
		   Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  register int height, old_height;

  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData) {
    ve = (XmVendorShellExtObject) extData->widget;

    old_height = ve->vendor.im_height;

    height = ImGetGeo(vw, NULL);
    if (!ve->vendor.im_vs_height_set) {
      Arg args[1];
      int base_height;
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(vw, args, 1);
      if (base_height > 0) {
        base_height += (height - old_height);
        XtSetArg(args[0], XtNbaseHeight, base_height);
        XtSetValues(vw, args, 1);
      }
      vw->core.height += (height - old_height);
    }
  }
}

void
_XmImRealize(
	     Widget vw )
{
  XmImXICInfo icp;
  Pixel bg;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;

  xim_info = get_xim_info(vw);
  im_info = get_im_info(vw, False);

  if ((xim_info == NULL) ||
      (im_info == NULL) ||
      (im_info->iclist == NULL))
    return;

  /* We need to synchronize here to make sure the server has created
   * the client window before the input server attempts to reparent
   * any windows to it
   */
  XSync(XtDisplay(vw), False);

  for (icp = im_info->iclist; icp != NULL; icp = icp->next)
    {
      if (!icp->xic) continue;
      XSetICValues(icp->xic, XNClientWindow, XtWindow(vw), NULL);
    }

  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData)
    ve = (XmVendorShellExtObject) extData->widget;
  else
    ve = NULL;
  if (ve && ve->vendor.im_height == 0) {
    ShellWidget shell = (ShellWidget)(vw);
    Boolean resize = shell->shell.allow_shell_resize;

    if (!resize) shell->shell.allow_shell_resize = True;
    ImGeoReq(vw);
    if (!resize) shell->shell.allow_shell_resize = False;
  } else
    ImSetGeo(vw, NULL);

  /* For some reason we need to wait till now before we set the
   * initial background pixmap.
   */
  if (ve && ve->vendor.im_height && im_info->current_widget)
    {
      XtVaGetValues(im_info->current_widget, XmNbackground, &bg, NULL);
      XtVaSetValues(vw, XmNbackground, bg, NULL);
    }
}

void
_XmImResize(
	    Widget vw )
{
  ImGetGeo(vw, NULL);
  ImSetGeo(vw, NULL);
}

void
_XmImRedisplay(
	       Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;

  if ((extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION)) == NULL)
    return;

  ve = (XmVendorShellExtObject) extData->widget;

  if (ve->vendor.im_height == 0)
    return;

  draw_separator(vw);
}

/********************
 * Static functions *
 ********************/

/* Locate an XmImXICInfo struct for an existing XIC. */
static XmImXICInfo
recreate_xic_info(XIC		  xic,
		  Widget	  shell,
		  XmImDisplayInfo xim_info,
		  XmImShellInfo   im_info)
{
  Cardinal index;
  XmImXICInfo xic_info;
  assert(xic != NULL);

  /* Search for an existing record in this shell's im_info. */
  for (xic_info = im_info->iclist;
       xic_info != NULL;
       xic_info = xic_info->next)
    {
      if (xic_info->xic == xic)
	return xic_info;
    }

  /* Search for an existing record in another shell's im_info? */
  for (index = 0; index < xim_info->shell_refs.num_refs; index++)
    if (shell != xim_info->shell_refs.refs[index])
      {
	XmImShellInfo tmp_info =
	  get_im_info(xim_info->shell_refs.refs[index], False);
	assert(tmp_info != NULL);

	for (xic_info = tmp_info->iclist;
	     xic_info != NULL;
	     xic_info = xic_info->next)
	  {
	    if (xic_info->xic == xic)
	      return xic_info;
	  }
      }

  /* This XIC must have been created by the application directly. */
  xic_info = XtNew(XmImXICRec);
  bzero((char*) xic_info, sizeof(XmImXICRec));
  (void) XGetICValues(xic, XNInputStyle, &xic_info->input_style, NULL);
  xic_info->next = im_info->iclist;
  im_info->iclist = xic_info;

  if (XtIsRealized (shell))
    {
      /* If client_window hasn't been set already, set it now. */
      (void) XSetICValues(xic, XNClientWindow, XtWindow(shell), NULL);

      /* Update cached geometry fields */
      ImGetGeo(shell, xic_info);
      ImSetGeo(shell, xic_info);
    }

  return xic_info;
}

/* Attempt to create an XmImXICInfo struct.  Return it or NULL. */
static XmImXICInfo
create_xic_info(Widget		shell,
		XmImDisplayInfo xim_info,
		XmImShellInfo   im_info,
                XmInputPolicy	input_policy)
{
  XIMStyle style = 0;
  char tmp[BUFSIZ];
  char *cp = NULL;
  char *tp = NULL;
  char *cpend = NULL;
  register XIMStyles *styles;
  XmImXICInfo xic_info;

  /* Determine the input style to be used for this XIC. */
  styles = xim_info->styles;
  XtVaGetValues(shell, XmNpreeditType, &cp, NULL);

  if (cp != NULL)
    {
      /* Parse for the successive commas */
      cp = strcpy(tmp,cp);
      cpend = &tmp[strlen(tmp)];
      assert(strlen(tmp) < BUFSIZ);
      while((style == 0) && (cp < cpend))
        {
          tp = strchr(cp,',');
          if (tp)
            *tp = 0;
          else
            tp = cpend;

          /* Look for an acceptable supported style. */
          if (XmeNamesAreEqual(cp, OVERTHESPOT))
            style = check_style(styles, XIMPreeditPosition,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, OFFTHESPOT))
            style = check_style(styles, XIMPreeditArea,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, ROOT))
            style = check_style(styles, XIMPreeditNothing,
                                XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, ONTHESPOT))
            style = check_style(styles, XIMPreeditCallbacks,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);

          cp = tp+1;
        }
    }
  if (style == 0)
    {
      /* Try for a fallback style, or give up and use XLookupString. */
      if ((style = check_style(styles, XIMPreeditNone, XIMStatusNone)) == 0)
        return NULL;
    }

  /* We need to create this widget whenever there is a non-simple
   * input method in order to stop the intrinsics from calling
   * XMapSubwindows, thereby improperly mapping input method
   * windows which have been made children of the client or
   * focus windows.
   */
  if ((im_info->im_widget == NULL) &&
      (style & (XIMStatusArea | XIMPreeditArea | XIMPreeditPosition)))
    im_info->im_widget =
      XtVaCreateWidget("xmim_wrapper", coreWidgetClass,
		       shell, XmNwidth, 10, XmNheight, 10, NULL);

  /* Create the XIC info record. */
  xic_info = XtNew(XmImXICRec);
  bzero((char*) xic_info, sizeof(XmImXICRec));
  xic_info->input_style = style;
  xic_info->anonymous = True;
  xic_info->preedit_buffer = XtNew(PreeditBufferRec);
  bzero((char *) xic_info->preedit_buffer, sizeof(PreeditBufferRec));

  xic_info->next = im_info->iclist;
  im_info->iclist = xic_info;

  /* Setup sharing for this XIC. */
  switch (input_policy)
    {
    case XmPER_SHELL:
      assert (im_info->shell_xic == NULL);
      im_info->shell_xic = xic_info;
      im_info->shell_xic->source = &im_info->shell_xic;
      break;

    case XmPER_WIDGET:
    case XmINHERIT_POLICY:
      break;

    default:
      assert(False);
    }

  return xic_info;
}


#define IsCallback(name) \
  if (name == XrmStringToName(XmNpreeditStartCallback) || \
      name == XrmStringToName(XmNpreeditDoneCallback) || \
      name == XrmStringToName(XmNpreeditDrawCallback) || \
      name == XrmStringToName(XmNpreeditCaretCallback))


static void
set_values(Widget w,
	   ArgList args,
	   Cardinal num_args,
           XmInputPolicy input_policy )
{
  register XmImXICInfo icp;
  XmImDisplayInfo xim_info;
  XmImResListRec *rlp;
  register int i, j;
  register ArgList argp = args;
  VaArgListRec status_vlist, preedit_vlist, xic_vlist;
  XVaNestedList va_slist, va_plist, va_vlist;
  XrmName name, area_name = XrmStringToName(XmNarea);
  Widget p;
  XmImShellInfo im_info;
  int flags = 0;
  Pixel bg;
  char *ret;
  unsigned long mask = 0;
  Boolean unrecognized = False;

  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);

  xim_info = get_xim_info(p);
  if ((icp = get_current_xic(xim_info, w)) == NULL)
    return;

  im_info = get_im_info(p, False);
  assert(im_info != NULL);

  if (!XtIsRealized(p)) {
    /* If vendor widget not realized, then the current info
     * is that for the last widget to set values.
     */
    im_info->current_widget = w;
  }

  if (icp->xic &&
      icp->focus_window && icp->focus_window != XtWindow(w))
    return;

  bzero((char*) &status_vlist, sizeof(VaArgListRec));
  bzero((char*) &preedit_vlist, sizeof(VaArgListRec));
  bzero((char*) &xic_vlist, sizeof(VaArgListRec));
  for (i = num_args; i > 0; i--, argp++) {
    name = XrmStringToName(argp->name);
    if (name == area_name && !(icp->input_style & XIMPreeditPosition))
      continue;

    IsCallback(name){
      if (icp->input_style & XIMPreeditCallbacks){
        set_callback_values(w, argp->name, (XIMCallback *)(argp->value),
                                                &preedit_vlist, input_policy);
        continue;
      } else
        continue;
    }

    _XmProcessLock();
    for (rlp = XmImResList, j = XtNumber(XmImResList); j != 0; j--, rlp++) {
      if (rlp->xrmname == name)	{
	flags |= (*rlp->proc)(rlp->xstring, (XPointer) argp->value,
			 &status_vlist, &preedit_vlist, &xic_vlist);
	break;
      }
    }
    _XmProcessUnlock();
    if (j == 0) {
      /* Simply pass unrecognized values along */
      VaSetArg(&xic_vlist, argp->name, (XPointer) argp->value);
      unrecognized = True;
    }
  }

  /* We do not create the IC until the initial data is ready to be passed */
  assert(xim_info != NULL);
  if (icp->xic == NULL) {
    if (XtIsRealized(p)) {
      XSync(XtDisplay(p), False);
      VaSetArg(&xic_vlist, XNClientWindow, (XPointer) XtWindow(p));
    }
    if (icp->focus_window) {
      VaSetArg(&xic_vlist, XNFocusWindow, (XPointer) icp->focus_window);
    }
    VaSetArg(&xic_vlist, XNInputStyle, (XPointer) icp->input_style);

    va_plist = VaCopy(&preedit_vlist);
    if (va_plist)
      VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
    va_slist = VaCopy(&status_vlist);
    if (va_slist)
      VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
    va_vlist = VaCopy(&xic_vlist);

    if (va_vlist)
    {
      icp->xic = XCreateIC(xim_info->xim, XNVaNestedList, va_vlist, NULL);
      if (icp->xic == NULL)
      {
        icp->input_style = XIMPreeditNothing | XIMStatusNothing;
        icp->xic = XCreateIC(xim_info->xim, XNInputStyle, icp->input_style,
            XNClientWindow, XtWindow(p), XNFocusWindow, XtWindow(p), NULL);
      }
    }
    else
      icp->xic = XCreateIC(xim_info->xim, NULL);

    if (va_vlist) XFree(va_vlist);
    if (va_plist) XFree(va_plist);
    if (va_slist) XFree(va_slist);
    if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
    if (status_vlist.args) XtFree((char *)status_vlist.args);
    if (xic_vlist.args) XtFree((char *)xic_vlist.args);

    if (icp->xic == NULL) {
      unset_current_xic(icp, im_info, xim_info, w);
      return;
    }
    XGetICValues(icp->xic, XNFilterEvents, &mask, NULL);
    if (mask) {
      XtAddEventHandler(p, (EventMask)mask, False, null_proc, NULL);
    }
    if (XtIsRealized(p)) {
      im_info->current_widget = w;
      if (XmIsDialogShell(p)) {
	for (i = 0;
	     i < ((CompositeWidget)p)->composite.num_children;
	     i++)
	  if (XtIsManaged(((CompositeWidget)p)->composite.children[i])) {
	    ImGeoReq(p);
	    break;
	  }
      } else
	ImGeoReq(p);
    }
    /* Is this new XIC supposed to be shared? */
    switch (input_policy)
      {
      case XmPER_SHELL:
	assert(im_info->shell_xic == NULL);
	im_info->shell_xic = icp;
	break;

      case XmPER_WIDGET:
      case XmINHERIT_POLICY:
	break;
      default:
	assert(False);
      }
  } else {
    /* Try to modify the existing XIC. */
    va_plist = VaCopy(&preedit_vlist);
    if (va_plist)
      VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
    va_slist = VaCopy(&status_vlist);
    if (va_slist)
      VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
    va_vlist = VaCopy(&xic_vlist);

    if (va_vlist)
      ret = XSetICValues(icp->xic, XNVaNestedList, va_vlist, NULL);
    else
      ret = NULL;

    if (va_vlist) XFree(va_vlist);
    if (va_plist) XFree(va_plist);
    if (va_slist) XFree(va_slist);
    if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
    if (status_vlist.args) XtFree((char *)status_vlist.args);
    if (xic_vlist.args) XtFree((char *)xic_vlist.args);

    /* ??? Both a write-once and an unrecognized arg might be present. */
    if ((ret != NULL) && unrecognized)	{
      unsigned long status_bg, status_fg;
      unsigned long preedit_bg, preedit_fg;

      /* ??? This code assumes that the XIM hasn't changed. */
      assert(XIMOfIC(icp->xic) == xim_info->xim);

      /* We do this in case an input method does not support
       * change of some value, but does allow it to be set on
       * create.  If however the value is not one of the
       * standard values, this im may not support it so we
       * should ignore it.
       */

      va_slist = XVaCreateNestedList(0,
				     XNBackground, &status_bg,
				     XNForeground, &status_fg,
				     NULL);
      va_plist = XVaCreateNestedList(0,
				     XNBackground, &preedit_bg,
				     XNForeground, &preedit_fg,
				     NULL);
      XGetICValues(icp->xic,
		   XNStatusAttributes,  va_slist,
		   XNPreeditAttributes, va_plist,
		   NULL);
      XFree(va_slist);
      XFree(va_plist);

      if (icp->anonymous)
	XDestroyIC(icp->xic);
      icp->anonymous = TRUE;
      icp->xic = NULL;

      VaSetArg(&status_vlist, XNBackground, (XPointer) status_bg);
      VaSetArg(&status_vlist, XNForeground, (XPointer) status_fg);

      VaSetArg(&preedit_vlist, XNBackground, (XPointer) preedit_bg);
      VaSetArg(&preedit_vlist, XNForeground, (XPointer) preedit_fg);

      if (XtIsRealized(p)) {
	XSync(XtDisplay(p), False);
	VaSetArg(&xic_vlist, XNClientWindow, (XPointer) XtWindow(p));
      }
      if (icp->focus_window) {
	VaSetArg(&xic_vlist, XNFocusWindow, (XPointer)icp->focus_window);
      }
      VaSetArg(&xic_vlist, XNInputStyle, (XPointer) icp->input_style);

      va_plist = VaCopy(&preedit_vlist);
      if (va_plist)
	VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
      va_slist = VaCopy(&status_vlist);
      if (va_slist)
	VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
      va_vlist = VaCopy(&xic_vlist);

      if (va_vlist)
      {
	icp->xic = XCreateIC(xim_info->xim, XNVaNestedList, va_vlist, NULL);
        if (icp->xic == NULL)
        {
          icp->input_style = XIMPreeditNothing | XIMStatusNothing;
          icp->xic = XCreateIC(xim_info->xim, XNInputStyle, icp->input_style,
              XNClientWindow, XtWindow(p), XNFocusWindow, XtWindow(p), NULL);
        }
      }
      else
	icp->xic = XCreateIC(xim_info->xim, NULL);

      if (va_vlist) XFree(va_vlist);
      if (va_plist) XFree(va_plist);
      if (va_slist) XFree(va_slist);
      if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
      if (status_vlist.args) XtFree((char *)status_vlist.args);
      if (xic_vlist.args) XtFree((char *)xic_vlist.args);

      if (icp->xic == NULL) {
	unset_current_xic(icp, im_info, xim_info, w);
	return;
      }
      ImGeoReq(p);
      if (icp->has_focus)
	XSetICFocus(icp->xic);
      return;
    }
    if (flags & GEO_CHG) {
      ImGeoReq(p);
      if (icp->has_focus)
	XSetICFocus(icp->xic);
    }
  }

  /* Since we do not know whether a set values may have been done
   * on top shadow or bottom shadow (used for the separator), we
   * will redraw the separator in order to keep the visuals in sync
   * with the current text widget. Also repaint background if needed.
   */
  if ((im_info->current_widget == w) && (flags & BG_CHG)) {
    XtVaGetValues(w, XmNbackground, &bg, NULL);
    XtVaSetValues(p, XmNbackground, bg, NULL);
  }
}


static void
ImFreePreeditBuffer(PreeditBuffer pb)
{
  if (pb->text) XtFree((char *)pb->text);
  if (pb->feedback) XtFree((char *)pb->feedback);
  XtFree((char *)pb);
}


static int
ImPreeditStartCallback(XIC xic,
		       XPointer client_data,
		       XPointer call_data)
{
  XICProc proc;
  Widget real = NULL;

  assert(client_data);
  proc = get_real_callback((Widget)client_data, PREEDIT_START, &real);
  if (proc)
    (*proc)(xic, (XPointer)real, call_data);

  return (-1);
}

static void
ImPreeditDoneCallback(XIC xic,
		      XPointer client_data,
		      XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data;
  XmImShellInfo im_info;
  XmImXICInfo icp;
  Widget real = NULL;

  assert(client_data);
  if ((im_info = get_im_info(w, False)) == NULL)
    return;
  if ((icp = im_info->shell_xic) == NULL)
    return;

  proc = get_real_callback((Widget)client_data, PREEDIT_DONE, &real);
  if (proc)
    (*proc)(xic, (XPointer)real, call_data);

  if (icp->preedit_buffer->text)
    XtFree((char *)icp->preedit_buffer->text);
  if (icp->preedit_buffer->feedback)
    XtFree((char *)icp->preedit_buffer->feedback);
  bzero((char *)icp->preedit_buffer, sizeof(PreeditBufferRec));
}

static void
ImPreeditDrawCallback(XIC xic,
		      XPointer client_data,
		      XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data;
  XmImShellInfo im_info;
  XmImXICInfo icp;
  PreeditBuffer pb;
  XIMText *text;
  XIMPreeditDrawCallbackStruct *data =
		(XIMPreeditDrawCallbackStruct *) call_data;
  int from=0, to=0, ml=0;
  wchar_t *wchar;
  Widget real = NULL;

  assert(client_data);

  /* update the preedit buffer */
  if ((im_info = get_im_info(w, False)) == NULL)
    return;
  if ((icp = im_info->shell_xic) == NULL)
    return;

  pb = icp->preedit_buffer;
  pb->caret = data->caret;
  text = data->text;

  if ((int)data->chg_length > (int)pb->length)
    data->chg_length = pb->length;

  if (data->text) { /* text field is non-NULL */

    if (data->chg_length > 0) { /* replace */
	if ((int)text->length > (int)data->chg_length) {
	  pb->text = (wchar_t *)
	    XtRealloc((char *)pb->text, (pb->length - data->chg_length +
			text->length + 1) * sizeof(wchar_t));
	  pb->feedback = (XIMFeedback *)
	    XtRealloc((char *)pb->feedback, (pb->length - data->chg_length +
			 text->length + 1) *sizeof(XIMFeedback));
	}
	from = data->chg_first + data->chg_length;
	to = data->chg_first + text->length;
 	ml = pb->length - from;
    }
    else if (data->chg_length == 0) { /* insert */
      /* do we really need to change anything? */
      if (data->text->length) {
        pb->text = (wchar_t *)
	  XtRealloc((char *)pb->text, (pb->length + text->length +1)
		    * sizeof(wchar_t));
	pb->feedback = (XIMFeedback *)
	  XtRealloc((char *)pb->feedback, (pb->length + text->length +1)
		    * sizeof(XIMFeedback));
	from = data->chg_first;
	to = data->chg_first + text->length;
	ml = pb->length - from;
      }
    }

    /*
    ** if preedit buffer changed, then we munge it,
    ** otherwise we just leave it alone
    */
    if (from || to || ml) {

      /* convert multibyte to wide char */
      wchar = (wchar_t *)XtMalloc ((text->length +1) * sizeof(wchar_t));
      if (text->encoding_is_wchar)
        memcpy(wchar, text->string.wide_char, text->length * sizeof(wchar_t));
      else
        mbstowcs(wchar, text->string.multi_byte, text->length + 1);

      /* make change */
      memmove((char *)pb->text + to * sizeof(wchar_t),
	      (char *)pb->text + from * sizeof(wchar_t),
	      ml * sizeof(wchar_t));
      memmove((char *)pb->feedback + to * sizeof(XIMFeedback),
	      (char *)pb->feedback + from * sizeof(XIMFeedback),
	      ml * sizeof(XIMFeedback));

      memmove((char *)pb->text + data->chg_first * sizeof(wchar_t),
	      (char *)wchar,
	      text->length * sizeof(wchar_t));

      /* feedback may be NULL, check for it */
      if (text->feedback)
	memmove((char *)pb->feedback + data->chg_first * sizeof(XIMFeedback),
		(char *)text->feedback,
		text->length * sizeof(XIMFeedback));

      pb->length = pb->length + text->length - data->chg_length;
      bzero((char *)pb->text + pb->length * sizeof(wchar_t),
	    sizeof(wchar_t));
      bzero((char *)pb->feedback + pb->length * sizeof(XIMFeedback),
	    sizeof(XIMFeedback));

      XtFree((char *) wchar);
    }
  }
  else { /* text field is NULL, delete */
    from = data->chg_first + data->chg_length;
    to = data->chg_first;
    ml = pb->length - from;
    memmove((char *)pb->text + to * sizeof(wchar_t),
	    (char *)pb->text + from * sizeof(wchar_t),
	    ml * sizeof(wchar_t));
    memmove((char *)pb->feedback + to * sizeof(XIMFeedback),
	    (char *)pb->feedback + from * sizeof(XIMFeedback),
	    ml * sizeof(XIMFeedback));

    pb->length = pb->length - data->chg_length;
    bzero((char *)pb->text + pb->length * sizeof(wchar_t),
	  data->chg_length * sizeof(wchar_t));
    bzero((char *)pb->feedback + pb->length * sizeof(XIMFeedback),
	  data->chg_length * sizeof(XIMFeedback));
  }

  proc = get_real_callback((Widget)client_data, PREEDIT_DRAW, &real);
  if (proc)
    (*proc)(xic, (XPointer)real, call_data);
}

static void
ImPreeditCaretCallback(XIC xic,
		       XPointer client_data,
		       XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data;
  XmImShellInfo im_info;
  XmImXICInfo icp;
  PreeditBuffer pb;
  XIMPreeditCaretCallbackStruct *data =
		(XIMPreeditCaretCallbackStruct *) call_data;
  Widget real = NULL;

  assert(client_data);

/* update the preedit buffer */
  if ((im_info = get_im_info(w, False)) == NULL)
        return;
  if ((icp = im_info->shell_xic) == NULL)
        return;

  pb = icp->preedit_buffer;
  pb->style = data->style;

  switch (data->direction) {
    case XIMForwardChar:
	pb->caret = pb->caret + 1;
	break;
    case XIMBackwardChar:
	pb->caret = pb->caret - 1;
	break;
    case XIMAbsolutePosition:
	pb->caret = data->position;
	break;
    default:
	break;
  }

  proc = get_real_callback((Widget)client_data, PREEDIT_CARET, &real);
  if (proc)
    (*proc)(xic, (XPointer)real, call_data);
}

static XICProc
get_real_callback(Widget w,
                  int swc,
		  Widget *real_widget)
{
  XmImShellInfo im_info;
  XmImXICInfo icp;
  int i, target;
  XmImRefRec refs;

  if ((im_info = get_im_info(w, False)) == NULL)
        return (XICProc)NULL;
  if ((icp = im_info->shell_xic) == NULL)
        return (XICProc)NULL;

  if (*real_widget == NULL)
    *real_widget = XtWindowToWidget(XtDisplay(w), icp->focus_window);

  refs = icp->widget_refs;
  target = refs.num_refs;
  for (i = 0; i < refs.num_refs; i++){
    if (refs.refs[i] == *real_widget){
      target = i;
      break;
    }
  }

  assert(target < refs.num_refs);
  if (refs.callbacks[target])
    return (XICProc)refs.callbacks[target][swc];
  else
    return (XICProc)NULL;
}

static void
regist_real_callback(Widget w,
                     XIMProc call,
                     int swc)
{
  Widget p;
  register XmImXICInfo icp;
  XmImDisplayInfo xim_info;
  XmImRefRec refs;
  int i, target = 0;

  p = w;
  while (!XtIsShell(p))
  {
    p = XtParent(p);
  }

  xim_info = get_xim_info(p);
  if ((icp = get_current_xic(xim_info, w)) == NULL)
  {
    return;
  }

  refs = icp->widget_refs;

  for (i = 0; i < refs.num_refs; i++)
  {
    if (refs.refs[i] == w)
    {
      target = i;
      break;
    }
  }

  if (!refs.callbacks[target])
    refs.callbacks[target] = (XtPointer *)XtMalloc(4 * sizeof(XtPointer));

  refs.callbacks[target][swc] = (XtPointer)call;
}

static int
NameToSwitch(String name)
{

  if (!strcmp(name, XmNpreeditStartCallback))
    return PREEDIT_START;
  if (!strcmp(name, XmNpreeditDoneCallback))
    return PREEDIT_DONE;
  if (!strcmp(name, XmNpreeditDrawCallback))
    return PREEDIT_DRAW;
  if (!strcmp(name, XmNpreeditCaretCallback))
    return PREEDIT_CARET;

  return 100;
}

static void
set_callback_values(Widget w,
                    String name,
                    XIMCallback *value,
                    VaArgList vlp,
                    XmInputPolicy input_policy)
{
  XIMProc call = value->callback;
  int s = NameToSwitch(name);
  XmInputPolicy ip = input_policy;
  Widget p =NULL;

  if (input_policy == XmINHERIT_POLICY){
    p = w;
    while (!XtIsShell(p))
      p = XtParent(p);
    XtVaGetValues(p, XmNinputPolicy, &ip, NULL);
  }

  switch (s) {
    case PREEDIT_START :
      if (ip == XmPER_SHELL){
        call = value->callback;
        regist_real_callback(w, call, s);
	value->client_data = (XPointer)p;
        value->callback = (XIMProc) ImPreeditStartCallback;
        VaSetArg(vlp, XNPreeditStartCallback, (XPointer)value);
      } else
        VaSetArg(vlp, XNPreeditStartCallback, (XPointer)value);
      break;

    case PREEDIT_DONE :
      if (ip == XmPER_SHELL){
        call = value->callback;
        regist_real_callback(w, call, s);
	value->client_data = (XPointer)p;
        value->callback = (XIMProc) ImPreeditDoneCallback;
        VaSetArg(vlp, XNPreeditDoneCallback, (XPointer)value);
      } else
        VaSetArg(vlp, XNPreeditDoneCallback, (XPointer)value);
      break;

    case PREEDIT_DRAW :
      if (ip == XmPER_SHELL){
        call = value->callback;
        regist_real_callback(w, call, s);
	value->client_data = (XPointer)p;
        value->callback = (XIMProc) ImPreeditDrawCallback;
        VaSetArg(vlp, XNPreeditDrawCallback, (XPointer)value);
      } else
        VaSetArg(vlp, XNPreeditDrawCallback, (XPointer)value);
      break;

    case PREEDIT_CARET :
      if (ip == XmPER_SHELL){
        call = value->callback;
        regist_real_callback(w, call, s);
	value->client_data = (XPointer)p;
        value->callback = (XIMProc) ImPreeditCaretCallback;
        VaSetArg(vlp, XNPreeditCaretCallback, (XPointer)value);
      } else
        VaSetArg(vlp, XNPreeditCaretCallback, (XPointer)value);
      break;

    default :
      assert(False);
  }
}


static void
move_preedit_string(XmImXICInfo icp,
		    Widget wfrom,
		    Widget wto)
{
  PreeditBuffer pb = icp->preedit_buffer;
  XIMPreeditDrawCallbackStruct draw_data;
  XIMText text;
  XICProc proc;

  proc = get_real_callback(wfrom, PREEDIT_DONE, &wfrom);
  if (proc)
    (*proc)(icp->xic, (XPointer)wfrom, NULL);

  proc = get_real_callback(wto, PREEDIT_START, &wto);
  if (proc)
    (*proc)(icp->xic, (XPointer)wto, NULL);

  if (pb->length == 0)
    return;

  draw_data.caret = pb->caret;
  draw_data.chg_first = 0;
  draw_data.chg_length = 0;
  text.length = pb->length;
  text.feedback = pb->feedback;
  text.encoding_is_wchar = True;
  text.string.wide_char = pb->text;
  draw_data.text = &text;
  proc = get_real_callback(wto, PREEDIT_DRAW, &wto);
  if (proc)
    (*proc)(icp->xic, (XPointer)wto, (XPointer)&draw_data);
}


static int
add_sp(String name,
       XPointer value,
       VaArgList slp,
       VaArgList plp,
       VaArgList vlp )		/* unused */
{
  VaSetArg(slp, name, value);
  VaSetArg(plp, name, value);

  return BG_CHG;
}

static int
add_p(String name,
      XPointer value,
      VaArgList slp,		/* unused */
      VaArgList plp,
      VaArgList vlp )		/* unused */
{
  VaSetArg(plp, name, value);

  return 0;
}


static int
add_fs(String name,
       XPointer value,
       VaArgList slp,
       VaArgList plp,
       VaArgList vlp )		/* unused */
{
  XFontSet fs;

  if ( (fs = extract_fontset((XmFontList)value)) == NULL)
    return 0;

  VaSetArg(slp, name, (XPointer) fs);
  VaSetArg(plp, name, (XPointer) fs);

  return GEO_CHG;
}

static int
add_bgpxmp(String name,
	   XPointer value,
	   VaArgList slp,
	   VaArgList plp,
	   VaArgList vlp )
{
  if ( (Pixmap)value == XtUnspecifiedPixmap )
    return 0;

  return add_sp( name, value, slp, plp, vlp );
}

static XIMStyle
check_style(XIMStyles *styles,
	    XIMStyle preedit_style,
	    XIMStyle status_style )
{
  register int i;

  /* Is this preedit & status style combination supported? */
  for (i=0; i < (int) styles->count_styles; i++)
    {
      if ((styles->supported_styles[i] & preedit_style) &&
	  (styles->supported_styles[i] & status_style))
	return styles->supported_styles[i];
    }
  return 0;
}


/* if this_icp is non-null, operations will only be performed on the
   corresponding IC. (Basically disables looping) */

static int
ImGetGeo(Widget  vw,
	 XmImXICInfo this_icp )
{
  XmImXICInfo icp;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  int height = 0;
  XRectangle rect;
  XRectangle *rp;
  XmImShellInfo im_info;
  XVaNestedList set_list, get_list;

  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData)
    ve = (XmVendorShellExtObject) extData->widget;
  else
    ve = NULL;

  im_info = get_im_info(vw, False);

  if (ve && ((im_info == NULL) || (im_info->iclist == NULL))) {
    ve->vendor.im_height = 0;
    return 0;
  }

  set_list = XVaCreateNestedList(0, XNAreaNeeded, (XPointer) &rect, NULL);
  get_list = XVaCreateNestedList(0, XNAreaNeeded, (XPointer) &rp, NULL);

  for (icp = this_icp ? this_icp : im_info->iclist;
       icp != NULL;
       icp = icp->next) {
    if (icp->xic) {
      if (icp->input_style & XIMStatusArea) {
	rect.width = vw->core.width;
	rect.height = 0;

	XSetICValues(icp->xic, XNStatusAttributes, set_list, NULL);
	XGetICValues(icp->xic, XNStatusAttributes, get_list, NULL);

	if ((int) rp->height > height)
	  height = rp->height;

	icp->status_width = MIN(rp->width, vw->core.width);
	icp->sp_height = rp->height;
	  XFree(rp);
      }
      if (icp->input_style & XIMPreeditArea) {
	rect.width = vw->core.width;
	rect.height = 0;

	XSetICValues(icp->xic, XNPreeditAttributes, set_list, NULL);
	XGetICValues(icp->xic, XNPreeditAttributes, get_list, NULL);

	if ((int) rp->height > height)
	  height = rp->height;

	icp->preedit_width = MIN((int) rp->width,
				 (int) (vw->core.width - icp->status_width));
	if (icp->sp_height < (int) rp->height)
	  icp->sp_height = rp->height;
	XFree(rp);
      }
    }

    if (this_icp)
      break;
  }

  XFree(set_list);
  XFree(get_list);

  if (height)
    height += SEPARATOR_HEIGHT;

  if (ve)
    ve->vendor.im_height = height;
  return height;
}


/* if this_icp is non-null, operations will only be performed on the
   corresponding IC. (Basically disables looping) */

static void
ImSetGeo(Widget  vw,
	 XmImXICInfo this_icp )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  register XmImXICInfo icp;
  XRectangle rect_status;
  XRectangle rect_preedit;
  XmImShellInfo im_info;
  XVaNestedList va_slist, va_plist;
  unsigned long use_slist, use_plist;

  im_info = get_im_info(vw, False);
  if ((im_info == NULL) || (im_info->iclist == NULL))
    return;

  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;

  if (ve->vendor.im_height == 0)
    return;

  va_slist = XVaCreateNestedList(0, XNArea, &rect_status, NULL);
  va_plist = XVaCreateNestedList(0, XNArea, &rect_preedit, NULL);

  for (icp = this_icp ? this_icp : im_info->iclist;
       icp != NULL;
       icp = icp->next)
    {
      if ((use_slist = (icp->input_style & XIMStatusArea)) != 0)
	{
	  rect_status.x = 0;
	  rect_status.y = vw->core.height - icp->sp_height;
	  rect_status.width = icp->status_width;
	  rect_status.height = icp->sp_height;
	}

      if ((use_plist = (icp->input_style & XIMPreeditArea)) != 0)
	{
	  rect_preedit.x = icp->status_width;
	  rect_preedit.y = vw->core.height - icp->sp_height;
	  rect_preedit.width = icp->preedit_width;
	  rect_preedit.height = icp->sp_height;
        } else if ((use_plist = (icp->input_style & XIMPreeditPosition)) != 0)
        {
          unsigned int  margin;
        /*
         * im_info->current_widget can contains NULL,
         * for example, when widget having XIC focus is disposed.
         * Thus, we should check this and avoid dereferencing NULL pointer.
         */
          if (im_info->current_widget == NULL)
            break;
          margin = ((XmPrimitiveWidget)im_info->current_widget)
                  ->primitive.shadow_thickness
              + ((XmPrimitiveWidget)im_info->current_widget)
                  ->primitive.highlight_thickness;

            rect_preedit.width = MIN(icp->preedit_width,
                  XtWidth(im_info->current_widget) - 2*margin);
            rect_preedit.height = MIN(icp->sp_height,
                  XtHeight(im_info->current_widget) - 2*margin);
	}

      if (use_slist && use_plist)
	XSetICValues(icp->xic,
		     XNStatusAttributes,  va_slist,
		     XNPreeditAttributes, va_plist,
		     NULL);
      else if (use_slist)
	XSetICValues(icp->xic,
		     XNStatusAttributes,  va_slist,
		     NULL);
      else if (use_plist)
	XSetICValues(icp->xic,
		     XNPreeditAttributes, va_plist,
		     NULL);

      if (this_icp)
	break;
    }

  XFree(va_slist);
  XFree(va_plist);
}

static void
ImGeoReq(Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XtWidgetGeometry my_request;
  int old_height;
  int delta_height;
  ShellWidget shell = (ShellWidget)(vw);

  if (!(shell->shell.allow_shell_resize) && XtIsRealized(vw))
    return;

  extData = _XmGetWidgetExtData(vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;

  old_height = ve->vendor.im_height;
  ImGetGeo(vw, NULL);
  if ((delta_height = ve->vendor.im_height - old_height) != 0)
    {
      int base_height;
      Arg args[1];
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(vw, args, 1);
      if (base_height > 0) {
	base_height += delta_height;
	XtSetArg(args[0], XtNbaseHeight, base_height);
	XtSetValues(vw, args, 1);
      }
      my_request.height = vw->core.height + delta_height;
      my_request.request_mode = CWHeight;
      XtMakeGeometryRequest(vw, &my_request, NULL);
    }
  ImSetGeo(vw, NULL);
}

static XFontSet
extract_fontset(
		XmFontList fl )
{
  XmFontContext context;
  XmFontListEntry next_entry;
  XmFontType type_return;
  XtPointer tmp_font;
  XFontSet first_fs = NULL;
  char *font_tag = NULL;

  if (!XmFontListInitFontContext(&context, fl))
    return NULL;

  do {
    next_entry = XmFontListNextEntry(context);
    if (next_entry)
      {
	tmp_font = XmFontListEntryGetFont(next_entry, &type_return);
	if (type_return == XmFONT_IS_FONTSET)
	  {
	    font_tag = XmFontListEntryGetTag(next_entry);
	    if (!strcmp(font_tag, XmFONTLIST_DEFAULT_TAG))
	      {
		XmFontListFreeFontContext(context);
		if (font_tag) XtFree(font_tag);
		return (XFontSet)tmp_font;
	      }
	    if (font_tag) XtFree(font_tag);
	    if (first_fs == NULL)
	      first_fs = (XFontSet)tmp_font;
	  }
      }
  } while (next_entry);

  XmFontListFreeFontContext(context);
  return first_fs;
}

/* Fetch (creating if necessary) the Display's xmim_info. */
static XmImDisplayInfo
get_xim_info(Widget  widget)
{
  XmDisplay xmDisplay;
  char tmp[BUFSIZ];
  char *cp = NULL;
  XmImDisplayInfo xim_info;
  String name, w_class;
  Display *dpy;
  Widget shell;

  if (widget == NULL)
    return NULL;

  /* Find the parent shell. */
  shell = widget;
  while (!XtIsShell(shell))
    shell = XtParent(shell);

  dpy = XtDisplay(shell);
  xmDisplay = (XmDisplay) XmGetXmDisplay(dpy);
  xim_info = (XmImDisplayInfo)xmDisplay->display.xmim_info;

  /* If this is a simple lookup we're done. */
  if (xim_info != NULL)
    return xim_info;

  /* Create a record so that we only try XOpenIM() once. */
  xim_info = XtNew(XmImDisplayRec);
  bzero((char*) xim_info, sizeof(XmImDisplayRec));
  xmDisplay->display.xmim_info = (XtPointer)xim_info;

  /* Setup any specified locale modifiers. */
  XtVaGetValues(shell, XmNinputMethod, &cp, NULL);
  if (cp != NULL)
    {
      strcpy(tmp,"@im=");
      strcat(tmp,cp);
      assert(strlen(tmp) < BUFSIZ);
      XSetLocaleModifiers(tmp);
    }

  XtGetApplicationNameAndClass(dpy, &name, &w_class);

  /* Try to open the input method. */
  xim_info->xim = XOpenIM(dpy, XtDatabase(dpy), name, w_class);
  if (xim_info->xim == NULL)
    {
#ifdef XOPENIM_WARNING
      /* Generate a warning if XOpenIM was supposed to work. */
      /* Use the WMShell's XmNtitleEncoding as a shibboleth. */
      Atom encoding = (Atom) 0;
      XtVaGetValues(shell, XmNtitleEncoding, &encoding, NULL);
      if (encoding != XA_STRING)
	XmeWarning ((Widget)widget, MSG1);
#endif

      /* Leave the null xim_info attached to the display so we only */
      /* print the warning message once. */
      return xim_info;
    }

  /* Lookup the styles this input method supports. */
  if (XGetIMValues(xim_info->xim,
		   XNQueryInputStyle, &xim_info->styles, NULL) != NULL)
    {
      XCloseIM(xim_info->xim);
      xim_info->xim = NULL;
      XmeWarning ((Widget)widget, MSG1);
      return xim_info;
    }

  /* Initialize the list of xrm names */
  {
    XmImResListRec *rlp;
    register int i;

    _XmProcessLock();
    for (rlp = XmImResList, i = XtNumber(XmImResList);
	 i != 0;
	 i--, rlp++)
      rlp->xrmname = XrmStringToName(rlp->xmstring);
    _XmProcessUnlock();
  }

  return xim_info;
}

static XtPointer*
get_im_info_ptr(Widget  w,
		Boolean create)
{
  Widget p;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;

  if (w == NULL)
    return NULL;

  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);

  /* Check extension data since app could be attempting to create
   * a text widget as child of menu shell.  This is illegal, and will
   * be detected later, but check here so we don't core dump.
   */
  if ((extData = _XmGetWidgetExtData((Widget)p, XmSHELL_EXTENSION)) == NULL)
    return NULL;

  ve = (XmVendorShellExtObject) extData->widget;

  if ((ve->vendor.im_info == NULL) && create)
    {
      im_info = XtNew(XmImShellRec);
      bzero((char*) im_info, sizeof(XmImShellRec));
      ve->vendor.im_info = (XtPointer)im_info;

      xim_info = get_xim_info(p);
      (void) add_ref(&xim_info->shell_refs, p);
    }

  return &ve->vendor.im_info;
}

static XmImShellInfo
get_im_info(Widget w,
	    Boolean create)
{
  XmImShellInfo* ptr = (XmImShellInfo *) get_im_info_ptr(w, create);
  if (ptr != NULL)
    return *ptr;
  else
    return NULL;
}

static void
draw_separator(Widget vw )
{
  XmPrimitiveWidget pw;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;

  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;
  if ((im_info = (XmImShellInfo)ve->vendor.im_info) == NULL)
    return;
  pw = (XmPrimitiveWidget)im_info->current_widget;
  if (!pw || !XmIsPrimitive(pw))
    return;

  XmeDrawSeparator(XtDisplay(vw), XtWindow(vw),
		   pw->primitive.top_shadow_GC,
		   pw->primitive.bottom_shadow_GC,
		   0,
		   0,
		   vw->core.height - ve->vendor.im_height,
		   vw->core.width,
		   SEPARATOR_HEIGHT,
		   SEPARATOR_HEIGHT,
		   0, 			/* separator.margin */
		   XmHORIZONTAL,		/* separator.orientation */
		   XmSHADOW_ETCHED_IN); /* separator.separator_type */
}

static void
null_proc(Widget w,		/* unused */
	  XtPointer ptr,		/* unused */
	  XEvent *ev,		/* unused */
	  Boolean *b )		/* unused */
{
  /* This function does nothing.  It is only there to allow the
   * event mask required by the input method to be added to
   * the client window.
   */
}

/* The following section contains the varargs functions */


/*VARARGS*/
void
XmImVaSetFocusValues(Widget w,
		     ... )
{
  va_list	var;
  int	    	total_count;
  ArgList     args;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  va_start(var,w);
  ImCountVaList(var, &total_count);
  va_end(var);

  va_start(var,w);
  args  = ImCreateArgList(var, total_count);
  va_end(var);

  XmImSetFocusValues(w, args, total_count);
  XtFree((char *)args);
  _XmAppUnlock(app);
}

/*VARARGS*/
void
  XmImVaSetValues(Widget w,
		  ... )
{
  va_list	var;
  int	    	total_count;
  ArgList     args;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  va_start(var,w);
  ImCountVaList(var, &total_count);
  va_end(var);

  va_start(var,w);
  args  = ImCreateArgList(var, total_count);
  va_end(var);

  XmImSetValues(w, args, total_count);
  XtFree((char *)args);
  _XmAppUnlock(app);
}


static void
ImCountVaList(va_list var,
	      int *total_count )
{
  String          attr;

  *total_count = 0;

  for(attr = va_arg(var, String); attr != NULL; attr = va_arg(var, String))
    {
      (void) va_arg(var, XPointer);
      ++(*total_count);
    }
}

static ArgList
ImCreateArgList(va_list var,
		int total_count )
{
  ArgList args = (ArgList)XtCalloc(total_count, sizeof(Arg));
  register int i;

  assert(args || (total_count == 0));
  for (i = 0; i < total_count; i++)
    {
      args[i].name = va_arg(var,String);
      args[i].value = (XtArgVal)va_arg(var,XPointer);
    }

  return args;
}

/* Return the current xic info for a widget, or NULL. */
static XmImXICInfo
get_current_xic(XmImDisplayInfo xim_info,
		Widget          widget)
{
  XmImXICInfo xic_info;

  if ((xim_info == NULL) ||
      (xim_info->current_xics == (XContext) 0))
    return NULL;

  if (XFindContext(XtDisplay(widget), (XID) widget,
		   xim_info->current_xics, (XPointer*) &xic_info) != 0)
    return NULL;
  else
    return xic_info;
}

/* Set the current XIC for an unregistered widget. */
static void
set_current_xic(XmImXICInfo 	xic_info,
		XmImDisplayInfo	xim_info,
		Widget		widget)
{
  if (xic_info == NULL)
    return;

  /* Record this widget as a reference to this XIC. */
  (void) add_ref(&xic_info->widget_refs, widget);

  /* Set the current XIC for this widget. */
  if (!xim_info->current_xics)
    xim_info->current_xics = XUniqueContext();
  (void) XSaveContext(XtDisplay(widget), (XID) widget,
		      xim_info->current_xics, (XPointer) xic_info);
}

/* Unset the current XIC for a widget, freeing data as necesary. */
static void
unset_current_xic(XmImXICInfo	  xic_info,
		  XmImShellInfo   im_info,
		  XmImDisplayInfo xim_info,
		  Widget 	  widget)
{
  /* Remove the current xic for this widget. */
  assert(xim_info->current_xics != (XContext) 0);
  (void) XDeleteContext(XtDisplay(widget), (XID) widget,
			xim_info->current_xics);
  if (im_info->current_widget == widget)
    im_info->current_widget = NULL;

  /* Remove this widget as a reference to this XIC. */
  if (remove_ref(&xic_info->widget_refs, widget) == 0)
    {
      /* Remove this xic_info from the master list. */
      XmImXICInfo *ptr;
      for (ptr = &(im_info->iclist); *ptr != NULL; ptr = &((*ptr)->next))
	if (*ptr == xic_info)
	  {
	    *ptr = xic_info->next;
	    break;
	  }

      /* Don't let anyone share this XIC. */
      if (xic_info->source != NULL)
	*(xic_info->source) = NULL;

      /* Destroy the XIC */
      if ((xic_info->anonymous) && (xic_info->xic != NULL))
	XDestroyIC(xic_info->xic);

      ImFreePreeditBuffer (xic_info->preedit_buffer);
      XtFree((char *) xic_info);
    }
}

/* Add a widget to a list of references. */
static Cardinal
add_ref(XmImRefInfo refs,
	Widget	    widget)
{
#ifdef DEBUG
  /* Verify that we don't already have a reference. */
  register Cardinal index;
  for (index = 0; index < refs->num_refs; index++)
    assert(refs->refs[index] != widget);
#endif

  /* Make room in the array. */
  if (refs->num_refs == refs->max_refs)
    {
      if (refs->max_refs == 0)
	refs->max_refs = 10;
      else
	refs->max_refs += (refs->max_refs / 2);

      refs->refs = (Widget*) XtRealloc((char *) refs->refs,
				       refs->max_refs * sizeof(Widget));
      refs->callbacks = (XtPointer **) XtRealloc((char *) refs->callbacks,
                                       refs->max_refs * sizeof(XtPointer *));
    }
  assert(refs->num_refs < refs->max_refs);

  refs->callbacks[refs->num_refs] = NULL;

  /* Insert this reference. */
  refs->refs[refs->num_refs++] = widget;

  return refs->num_refs;
}

/* Remove a widget from a list of references. */
static Cardinal
remove_ref(XmImRefInfo refs,
	   Widget      widget)
{
  /* Is this the last reference? */
  refs->num_refs--;
  if (refs->num_refs > 0)
    {
      /* Just remove this reference. */
      int index = 0;
      while (index <= refs->num_refs) {
	if (refs->refs[index] == widget)
	  {
	    refs->refs[index] = refs->refs[refs->num_refs];
	    refs->refs[refs->num_refs] = NULL;
            XtFree((char *)refs->callbacks[index]);
            refs->callbacks[index] = refs->callbacks[refs->num_refs];
            refs->callbacks[refs->num_refs] = NULL;

	    break;
	  }
	index++;
      }

      /* Free some storage from the array? */
      if ((refs->num_refs * 3 < refs->max_refs) &&
	  (refs->max_refs >= 20))
	{
	  refs->max_refs /= 2;
	  refs->refs = (Widget*) XtRealloc((char *) refs->refs,
					   refs->max_refs * sizeof(Widget));
          refs->callbacks = (XtPointer **) XtRealloc((char *) refs->callbacks,
                                   refs->max_refs * sizeof(XtPointer *));
	}
    }
  else
    {
      /* Free the references array. */
      XtFree((char *) refs->refs);
      refs->refs = NULL;
      XtFree((char *) refs->callbacks[0]);
      XtFree((char *) refs->callbacks);
      refs->callbacks = NULL;
      refs->max_refs = 0;
    }

  return refs->num_refs;
}

/* Convert a VaArgList into a true XVaNestedList. */
static XVaNestedList
VaCopy(VaArgList list)
{
  /* This is ugly, but it's a legal way to construct a nested */
  /* list whose length is unknown at compile time.  If MAXARGS is */
  /* increased more parameter pairs should be added below.   A */
  /* recursive approach would leak memory. */
  register Cardinal count = list->count;
  register VaArg *args = list->args;

#define VA_NAME(index)	(index < count ? args[index].name : NULL)
#define VA_VALUE(index)	(index < count ? args[index].value : NULL)

  assert(count <= 10);
  return XVaCreateNestedList(0,
			     VA_NAME(0), VA_VALUE(0),
			     VA_NAME(1), VA_VALUE(1),
			     VA_NAME(2), VA_VALUE(2),
			     VA_NAME(3), VA_VALUE(3),
			     VA_NAME(4), VA_VALUE(4),
			     VA_NAME(5), VA_VALUE(5),
			     VA_NAME(6), VA_VALUE(6),
			     VA_NAME(7), VA_VALUE(7),
			     VA_NAME(8), VA_VALUE(8),
			     VA_NAME(9), VA_VALUE(9),
			     NULL);

#undef VA_NAME
#undef VA_VALUE
}

static void
VaSetArg(VaArgList list,
	 char     *name,
	 XPointer  value)
{
  if (list->max <= list->count)
    {
      list->max += 10;
      list->args = (VaArg*) XtRealloc((char*) list->args,
				      list->max * sizeof(VaArg));
    }

  list->args[list->count].name = name;
  list->args[list->count].value = value;
  list->count++;
}


void
XmImMbResetIC(
    Widget w,
    char **mb)
{
    register XmImXICInfo icp;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);

    *mb = NULL;

    if ((icp = get_current_xic(get_xim_info(w), w)) == NULL ||
	icp->xic == NULL) {
	_XmAppUnlock(app);
        return;
    }

    if (!(icp->input_style & XIMPreeditCallbacks)) {
	_XmAppUnlock(app);
	return;
    }

    *mb = XmbResetIC(icp->xic);
    _XmAppUnlock(app);
}

XIMResetState
XmImGetXICResetState(Widget w)
{
    XmImXICInfo icp;
    XIMResetState state = XIMInitialState;
    icp = get_current_xic(get_xim_info(w), w);
    if (icp != NULL && icp->xic != NULL)
        XGetICValues(icp->xic, XNResetState, &state, NULL);
    return state;
}
