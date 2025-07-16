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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: BaseClass.c /main/20 1997/03/31 13:14:31 dbl $"
#endif
#endif
#define HAVE_EXTENSIONS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/XmP.h>
#include <X11/ShellP.h>
#include <Xm/ExtObjectP.h>
#include <Xm/Screen.h>
#include <Xm/VendorSEP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "BaseClassI.h"
#include "CallbackI.h"
#include "DropSMgrI.h"
#include "MessagesI.h"
#include "TraversalI.h"
#include "XmI.h"

#define MSG1	_XmMMsgBaseClass_0000
#define MSG2	_XmMMsgBaseClass_0001

#define IsBaseClass(wc) \
  ((wc == xmGadgetClass) 		||\
   (wc == xmManagerWidgetClass)		||\
   (wc == xmPrimitiveWidgetClass)	||\
   (wc == vendorShellWidgetClass) 	||\
   (wc == xmDisplayClass)		||\
   (wc == xmScreenClass)		||\
   (wc == xmExtObjectClass)		||\
   (_XmIsFastSubclass(wc, XmMENU_SHELL_BIT)))

#define isWrappedXtClass(wc) \
   ((wc == rectObjClass)	||\
    (wc == compositeWidgetClass))


/*
 * These must be initialized; otherwise they are "secondary symbols"
 * and are not actually present in the library under HP/UX 10.0.  That
 * caused exportlistgen to hide them entirely, causing links to fail.
 */
externaldef(baseclass) XrmQuark XmQmotif = NULLQUARK;
externaldef(baseclass) XmBaseClassExt *_Xm_fastPtr = NULL;

typedef struct _XmObjectClassWrapper {
    XtInitProc		initialize;
    XtSetValuesFunc	setValues;
    XtArgsProc		getValues;
    XtWidgetClassProc	classPartInit;
} XmObjectClassWrapper;

static XmObjectClassWrapper objectClassWrapper;
static XContext resizeRefWContext = 0;
static XContext geoRefWContext = 0;

#define GetRefW(dpy, context, w) \
	if (XFindContext(dpy, None, context, (XPointer *) &w))  w = NULL
#define SetRefW(dpy, context, w) \
	XSaveContext(dpy, None, context, (char *)w)

externaldef(xminheritclass) int _XmInheritClass = 0;


/********    Static Function Declarations    ********/

static XmWrapperData GetWrapperData(WidgetClass w_class);
static XContext ExtTypeToContext(unsigned char extType);
static void RealizeWrapper0(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper1(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper2(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper3(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper4(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper5(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper6(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper7(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper8(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper9(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static void RealizeWrapper10(Widget w, Mask *vmask, XSetWindowAttributes *attr);
static Cardinal GetRealizeDepth(WidgetClass wc);
static void RealizeWrapper(Widget w,
			   Mask *vmask,
			   XSetWindowAttributes *attr,
			   Cardinal depth);
static void ResizeWrapper0(Widget w);
static void ResizeWrapper1(Widget w);
static void ResizeWrapper2(Widget w);
static void ResizeWrapper3(Widget w);
static void ResizeWrapper4(Widget w);
static void ResizeWrapper5(Widget w);
static void ResizeWrapper6(Widget w);
static void ResizeWrapper7(Widget w);
static void ResizeWrapper8(Widget w);
static void ResizeWrapper9(Widget w);
static void ResizeWrapper10(Widget w);
static void ResizeWrapper11(Widget w);
static void ResizeWrapper12(Widget w);
static void ResizeWrapper13(Widget w);
static Cardinal GetResizeDepth(WidgetClass wc);
static void ResizeWrapper(Widget w, int depth);
static XtGeometryResult GeometryHandlerWrapper0(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper1(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper2(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper3(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper4(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper5(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper6(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper7(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper8(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper9(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper10(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper11(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static XtGeometryResult GeometryHandlerWrapper12(Widget w,
						XtWidgetGeometry *desired,
						XtWidgetGeometry *allowed);
static Cardinal GetGeometryHandlerDepth(WidgetClass wc);
static XtGeometryResult GeometryHandlerWrapper(Widget w,
					       XtWidgetGeometry *desired,
					       XtWidgetGeometry *allowed,
					       int depth);
static XmBaseClassExt * BaseClassPartInitialize(WidgetClass wc);
static void ClassPartInitRootWrapper(WidgetClass wc);
static void ClassPartInitLeafWrapper(WidgetClass wc);
static XtResourceList * CreateIndirectionTable(XtResourceList resources,
						 Cardinal num_resources);
static void InitializeRootWrapper(
			Widget req,
		   	Widget new_w,
			ArgList args,
			Cardinal *num_args) ;
static void InitializeLeafWrapper(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args,
			int depth);
static void CInitializeLeafWrapper(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args,
			int depth);
static Boolean SetValuesRootWrapper(
			Widget current,
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static Boolean SetValuesLeafWrapper(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth);
static Boolean CSetValuesLeafWrapper(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth);
static void GetValuesRootWrapper(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth);
static int GetDepth(WidgetClass wc);

static void InitializeLeafWrapper0(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper1(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper2(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper3(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper4(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper5(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper6(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper7(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper8(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void InitializeLeafWrapper9(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);

static void CInitializeLeafWrapper0(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper1(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper2(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper3(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper4(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper5(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper6(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper7(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper8(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);
static void CInitializeLeafWrapper9(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args);


static Boolean SetValuesLeafWrapper0(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper1(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper2(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper3(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper4(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper5(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper6(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper7(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper8(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean SetValuesLeafWrapper9(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);

static Boolean CSetValuesLeafWrapper0(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper1(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
			Cardinal *num_args);
static Boolean CSetValuesLeafWrapper2(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper3(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper4(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper5(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper6(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper7(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper8(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static Boolean CSetValuesLeafWrapper9(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);


static void GetValuesLeafWrapper0(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper1(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper2(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper3(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper4(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper5(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper6(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper7(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper8(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void GetValuesLeafWrapper9(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);

/********    End Static Function Declarations    ********/


XmGenericClassExt *
_XmGetClassExtensionPtr(
        XmGenericClassExt *listHeadPtr,
        XrmQuark owner )
{
  XmGenericClassExt *lclPtr = listHeadPtr;

#ifdef DEBUG
  if (!lclPtr)
    {
      XmeWarning(NULL, "_XmGetClassExtensionPtr: invalid class ext pointer");
      return NULL;
    }
#endif /* DEBUG */

  while (lclPtr && *lclPtr && ((*lclPtr)->record_type != owner))
    lclPtr = (XmGenericClassExt *) &((*lclPtr)->next_extension);

  return lclPtr;
}

static XmWrapperData
GetWrapperData(
        WidgetClass w_class )
{
  XmBaseClassExt *wcePtr;

  wcePtr = _XmGetBaseClassExtPtr( w_class, XmQmotif);

  if (!*wcePtr)
    {
      *wcePtr = (XmBaseClassExt) XtCalloc(1, sizeof(XmBaseClassExtRec));
      (*wcePtr)->next_extension = NULL;
      (*wcePtr)->record_type 	= XmQmotif;
      (*wcePtr)->version	= XmBaseClassExtVersion;
      (*wcePtr)->record_size	= sizeof( XmBaseClassExtRec);
    }

  if ((*wcePtr)->version < XmBaseClassExtVersion)
    return NULL;

  if (!((*wcePtr)->wrapperData))
    (*wcePtr)->wrapperData =
      (XmWrapperData) XtCalloc(1, sizeof(XmWrapperDataRec));

  return (*wcePtr)->wrapperData;
}


typedef struct _ExtToContextRec {
    unsigned char	extType;
    XContext		context;
} ExtToContextRec, *ExtToContext;

static XContext
ExtTypeToContext(
        unsigned char extType )
{
  static ExtToContextRec extToContextMap[16];
  Cardinal		 i;
  ExtToContext		 curr;
  XContext		 context = 0;

  _XmProcessLock();
  for (i = 0, curr = &extToContextMap[0];
       i < XtNumber(extToContextMap) && !context;
       i++, curr++)
    {
      if (curr->extType == extType)
	context = curr->context;
      else if (!curr->extType)
	{
	  curr->extType = extType;
	  context = curr->context = XUniqueContext();
	}
    }
  _XmProcessUnlock();

  if (!context)
    XmeWarning(NULL, MSG1);

  return context;
}

typedef struct _XmAssocDataRec {
    XtPointer			data;
    struct _XmAssocDataRec	*next;
} XmAssocDataRec, *XmAssocData;

void
_XmPushWidgetExtData(
        Widget widget,
        XmWidgetExtData data,
        unsigned char extType )
{
  XmAssocData  newData;
  XmAssocData  assocData = NULL;
  XmAssocData *assocDataPtr;
  Boolean      empty;
  XContext     widgetExtContext = ExtTypeToContext(extType);

  newData = (XmAssocData) XtCalloc(1, sizeof(XmAssocDataRec));

  newData->data = (XtPointer)data;

  empty = XFindContext(XtDisplay(widget), (Window) widget,
		       widgetExtContext, (char **) &assocData);

  assocDataPtr = &assocData;
  while (*assocDataPtr)
    assocDataPtr = &((*assocDataPtr)->next);

  *assocDataPtr = newData;

  if (empty)
    XSaveContext(XtDisplay(widget), (Window) widget,
		 widgetExtContext, (XPointer) assocData);
}

void
_XmPopWidgetExtData(
        Widget widget,
        XmWidgetExtData *dataRtn,
        unsigned char extType )
{
  XmAssocData  assocData = NULL;
  XmAssocData *assocDataPtr;
  XContext     widgetExtContext = ExtTypeToContext(extType);

  /* Initialize the return parameter. */
  *dataRtn = NULL;

  if (XFindContext(XtDisplay(widget),
		   (Window) widget,
		   widgetExtContext,
		   (char **) &assocData))
    {
#ifdef DEBUG
      XmeWarning(NULL, MSG2);
#endif
      return;
    }

  assocDataPtr = &assocData;
  while ((*assocDataPtr) && (*assocDataPtr)->next)
    assocDataPtr = &((*assocDataPtr)->next);

  if (*assocDataPtr == assocData)
    XDeleteContext(XtDisplay(widget), (Window)widget, widgetExtContext);

  if (*assocDataPtr)
    {
      *dataRtn = (XmWidgetExtData) (*assocDataPtr)->data;
      XtFree((char *) *assocDataPtr);
      *assocDataPtr = NULL;
    }
}

XmWidgetExtData
_XmGetWidgetExtData(
        Widget widget,
        unsigned char extType )
{
  XmAssocData  assocData = NULL;
  XmAssocData *assocDataPtr;
  XContext     widgetExtContext = ExtTypeToContext(extType);


  if ((XFindContext(XtDisplay(widget),
		    (Window) widget,
		    widgetExtContext,
		    (char **) &assocData)))
    {
#ifdef DEBUG
      XmeWarning(NULL, "no extension data on stack");
#endif /* DEBUG */
      return NULL;
    }
  else
    {
      assocDataPtr = &assocData;
      while ((*assocDataPtr)->next)
	assocDataPtr = &((*assocDataPtr)->next);

      return (XmWidgetExtData) (*assocDataPtr)->data;
    }
}


Boolean
_XmIsSubclassOf(
	WidgetClass wc,
	WidgetClass sc)
{
  WidgetClass p = wc;

  while ((p) && (p != sc))
    p = p->core_class.superclass;

  return (p == sc);
}

/*********************************************************************
 *
 *  RealizeWrappers for vendorShell
 *
 *********************************************************************/

static void
RealizeWrapper0(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 0);
}

static void
RealizeWrapper1(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 1);
}

static void
RealizeWrapper2(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 2);
}

static void
RealizeWrapper3(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 3);
}

static void
RealizeWrapper4(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 4);
}

static void
RealizeWrapper5(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 5);
}

static void
RealizeWrapper6(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 6);
}

static void
RealizeWrapper7(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 7);
}

static void
RealizeWrapper8(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 8);
}

static void
RealizeWrapper9(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 9);
}

static void
RealizeWrapper10(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr )
{
  RealizeWrapper(w, vmask, attr, 10);
}

/* Used to be XmConst, but this was not linking on Solaris */
static XtRealizeProc realizeWrappers[] = {
    RealizeWrapper0,
    RealizeWrapper1,
    RealizeWrapper2,
    RealizeWrapper3,
    RealizeWrapper4,
    RealizeWrapper5,
    RealizeWrapper6,
    RealizeWrapper7,
    RealizeWrapper8,
    RealizeWrapper9,
    RealizeWrapper10
};

static Cardinal
GetRealizeDepth(
        WidgetClass wc )
{
  Cardinal i;

  i = 0;
  while (wc && wc != vendorShellWidgetClass)
    {
      i++;
      wc = wc->core_class.superclass;
    }

  if (wc)
    return i;
#ifdef DEBUG
  else
    XtError("bad class for shell realize");
#endif /* DEBUG */

  return 0;
}

/************************************************************************
 *
 *  RealizeWrapper
 *
 ************************************************************************/

static void
RealizeWrapper(
        Widget w,
        Mask *vmask,
        XSetWindowAttributes *attr,
        Cardinal depth )
{
  if (XmIsVendorShell(w))
    {
      XmWidgetExtData	extData;
      WidgetClass	wc = XtClass(w);
      XmWrapperData	wrapperData;
      XtRealizeProc 	realize;
      Cardinal		leafDepth = GetRealizeDepth(wc);
      Cardinal		depthDiff = leafDepth - depth;

      while (depthDiff)
	{
	  depthDiff--;
	  wc = wc->core_class.superclass;
	}

      _XmProcessLock();
      wrapperData = GetWrapperData(wc);
      realize = wrapperData ? wrapperData->realize : NULL;
      _XmProcessUnlock();
      if (realize)
	(*realize)(w, vmask, attr);

#if 0
      /* DRK 6/20/94 -- This change breaks our test environment; when
       *	present the normal shell isn't added to the grab list
       *	because VendorExtRealize is never called.  It used to
       *	be called for the ApplicationShell class.
       */

      /*
       * CR 9266: Only call the RealizeCallback if we're doing the
       * VendorShell class level. Do not call multiple times for
       * VendorShell subclasses.
       */
      if ((extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION)) &&
	  (extData->widget) && wc == vendorShellWidgetClass)
#else
      /*
       * CR 3353 - Avoid calling the RealizeCallback twice for DialogShells
       *	by checking the WidgetClass name.  If it is XmDialogShell,
       *	do not call the callback (it will be called prior when
       *	the WidgetClass is VendorShell).
       */
      if ((extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION)) &&
	  (extData->widget) &&
	  strcmp(wc->core_class.class_name, "XmDialogShell"))
#endif
	{
	  _XmCallCallbackList(extData->widget,
			      ((XmVendorShellExtObject)
			       (extData->widget))->vendor.realize_callback,
			      NULL);
	}
#ifdef DEBUG
      else
	XmeWarning(NULL, "we only support realize callbacks on shells");
#endif /* DEBUG */
    }
}

/*********************************************************************
 *
 *  ResizeWrappers for rectObj
 *
 *********************************************************************/

static void
ResizeWrapper0(
        Widget w )
{
  ResizeWrapper(w, 0);
}

static void
ResizeWrapper1(
        Widget w )
{
  ResizeWrapper(w, 1);
}

static void
ResizeWrapper2(
        Widget w )
{
  ResizeWrapper(w, 2);
}

static void
ResizeWrapper3(
        Widget w )
{
  ResizeWrapper(w, 3);
}

static void
ResizeWrapper4(
        Widget w )
{
  ResizeWrapper(w, 4);
}

static void
ResizeWrapper5(
        Widget w )
{
  ResizeWrapper(w, 5);
}

static void
ResizeWrapper6(
        Widget w )
{
  ResizeWrapper(w, 6);
}

static void
ResizeWrapper7(
        Widget w )
{
  ResizeWrapper(w, 7);
}

static void
ResizeWrapper8(
        Widget w )
{
  ResizeWrapper(w, 8);
}

static void
ResizeWrapper9(
        Widget w )
{
  ResizeWrapper(w, 9);
}

static void
ResizeWrapper10(
        Widget w )
{
  ResizeWrapper(w, 10);
}

static void
ResizeWrapper11(
        Widget w )
{
  ResizeWrapper(w, 11);
}

static void
ResizeWrapper12(
        Widget w )
{
  ResizeWrapper(w, 12);
}

static void
ResizeWrapper13(
        Widget w )
{
  ResizeWrapper(w, 13);
}

/* Used to be XmConst, but this was not linking on Solaris */
static XtWidgetProc resizeWrappers[] = {
    ResizeWrapper0,
    ResizeWrapper1,
    ResizeWrapper2,
    ResizeWrapper3,
    ResizeWrapper4,
    ResizeWrapper5,
    ResizeWrapper6,
    ResizeWrapper7,
    ResizeWrapper8,
    ResizeWrapper9,
    ResizeWrapper10,
    ResizeWrapper11,
    ResizeWrapper12,
    ResizeWrapper13
};

static Cardinal
GetResizeDepth(
        WidgetClass wc )
{
  Cardinal i;

  i = 0;
  while (wc && wc != rectObjClass)
    {
      i++;
      wc = wc->core_class.superclass;
    }

  if (wc)
    return i;

  return 0;
}

/************************************************************************
 *
 *  ResizeWrapper
 *
 ************************************************************************/

static void
ResizeWrapper(
        Widget w,
	int depth )
{
  Widget refW = NULL;
  WidgetClass	wc = XtClass(w);
  Display *dpy = XtDisplay(w);
  XmWrapperData	wrapperData;
  XtWidgetProc  resize;
  Cardinal	leafDepth = GetResizeDepth(wc);
  Cardinal	depthDiff = leafDepth - depth;
  Boolean	call_navig_resize = FALSE;

  /* Call _XmNavigResize() only once per resize event, so nested
   *	resize calls are completed before evaluating the status of
   *	the focus widget.  Only check for lost focus in
   *	response to resize events from a Shell; otherwise
   *	_XmNavigResize may (prematurely) determine that the
   *	focus widget is no longer traversable before the
   *	new layout is complete.
   */
  if (XtParent(w) && XtIsShell(XtParent(w)))
    call_navig_resize = TRUE;

  while (depthDiff)
    {
      depthDiff--;
      wc = wc->core_class.superclass;
    }

  GetRefW(dpy, resizeRefWContext, refW);
  _XmProcessLock();
  wrapperData = GetWrapperData(wc);
  resize = wrapperData ? wrapperData->resize: NULL;
  _XmProcessUnlock();

  if (resize)
    {
      if ((refW == NULL) && (_XmDropSiteWrapperCandidate(w)))
	{
	  refW = w;
	  SetRefW(dpy, resizeRefWContext, refW);
	  XmDropSiteStartUpdate(refW);
	  (*resize)(w);
	  XmDropSiteEndUpdate(refW);
	  refW = NULL;
	  SetRefW(dpy, resizeRefWContext, refW);
	}
      else
	(*resize)(w);
    }

  if (call_navig_resize)
    _XmNavigResize( w);
}

/*********************************************************************
 *
 *  GeometryHandlerWrappers for composite
 *
 *********************************************************************/

static XtGeometryResult
GeometryHandlerWrapper0(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 0);
}

static XtGeometryResult
GeometryHandlerWrapper1(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 1);
}

static XtGeometryResult
GeometryHandlerWrapper2(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 2);
}

static XtGeometryResult
GeometryHandlerWrapper3(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 3);
}

static XtGeometryResult
GeometryHandlerWrapper4(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 4);
}

static XtGeometryResult
GeometryHandlerWrapper5(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 5);
}

static XtGeometryResult
GeometryHandlerWrapper6(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 6);
}

static XtGeometryResult
GeometryHandlerWrapper7(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 7);
}

static XtGeometryResult
GeometryHandlerWrapper8(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 8);
}

static XtGeometryResult
GeometryHandlerWrapper9(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 9);
}

static XtGeometryResult
GeometryHandlerWrapper10(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 10);
}

static XtGeometryResult
GeometryHandlerWrapper11(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 11);
}

static XtGeometryResult
GeometryHandlerWrapper12(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed )
{
  return GeometryHandlerWrapper(w, desired, allowed, 12);
}

/* Used to be XmConst, but this was not linkking on Solaris */
static XtGeometryHandler geometryHandlerWrappers[] = {
    GeometryHandlerWrapper0,
    GeometryHandlerWrapper1,
    GeometryHandlerWrapper2,
    GeometryHandlerWrapper3,
    GeometryHandlerWrapper4,
    GeometryHandlerWrapper5,
    GeometryHandlerWrapper6,
    GeometryHandlerWrapper7,
    GeometryHandlerWrapper8,
    GeometryHandlerWrapper9,
    GeometryHandlerWrapper10,
    GeometryHandlerWrapper11,
    GeometryHandlerWrapper12
};

static Cardinal
GetGeometryHandlerDepth(
        WidgetClass wc )
{
  Cardinal i;

  i = 0;
  while (wc && wc != rectObjClass)
    {
      i++;
      wc = wc->core_class.superclass;
    }

  if (wc)
    return i;

  return 0;
}

/************************************************************************
 *
 *  GeometryHandlerWrapper
 *
 ************************************************************************/
static XtGeometryResult
GeometryHandlerWrapper(
        Widget w,
	XtWidgetGeometry *desired,
	XtWidgetGeometry *allowed,
	int depth)
{
  Widget 	    refW = NULL;
  XtGeometryResult  result = XtGeometryNo;
  Widget	    parent = XtParent(w);
  WidgetClass	    wc = XtClass(parent);
  Display 	    *dpy = XtDisplay(w);
  XmWrapperData	    wrapperData;
  XtGeometryHandler geometry_manager;
  Cardinal	    leafDepth = GetGeometryHandlerDepth(wc);
  Cardinal	    depthDiff = leafDepth - depth;

  while (depthDiff)
    {
      depthDiff--;
      wc = wc->core_class.superclass;
    }

  GetRefW(dpy, geoRefWContext, refW);
  _XmProcessLock();
  wrapperData = GetWrapperData(wc);
  geometry_manager = wrapperData ? wrapperData->geometry_manager: NULL;
  _XmProcessUnlock();

  if (geometry_manager)
    {
      if ((refW == NULL) && (_XmDropSiteWrapperCandidate(w)))
	{
	  refW = w;
	  SetRefW(dpy, geoRefWContext, refW);
	  XmDropSiteStartUpdate(refW);
	  result = (*geometry_manager) (w, desired, allowed);
	  XmDropSiteEndUpdate(refW);
	  refW = NULL;
	  SetRefW(dpy, geoRefWContext, refW);
	}
      else
	result = (*geometry_manager) (w, desired, allowed);
    }

  return result;
}

/************************************************************************
 *
 *  BaseClassPartInitialize
 *
 ************************************************************************/
static XmBaseClassExt *
BaseClassPartInitialize(
        WidgetClass wc )
{
  XmBaseClassExt	*wcePtr, *scePtr;
  Cardinal		i;
  Boolean		inited;
  XmWrapperData		wcData, scData;
  Boolean		isBaseClass = IsBaseClass(wc);

  /*
   * This routine is called out of the ClassPartInitRootWrapper. It
   * needs to make sure that this is a Motif class and if it is,
   * then to initialize it.  We assume that the base classes always
   * have a static initializer !!!
   */

  wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
  scePtr = _XmGetBaseClassExtPtr(wc->core_class.superclass, XmQmotif);

  if (!isBaseClass &&
      !isWrappedXtClass(wc) &&
      (!scePtr || !(*scePtr)))
    return NULL;

  if ((isBaseClass) || (scePtr && (*scePtr)))
    {
      if (!(*wcePtr))
	{
	  inited = False;
	  *wcePtr = (XmBaseClassExt) XtCalloc(1, sizeof(XmBaseClassExtRec));
	  (*wcePtr)->classPartInitPrehook  = XmInheritClassPartInitPrehook;
	  (*wcePtr)->classPartInitPosthook = XmInheritClassPartInitPosthook;
	  (*wcePtr)->initializePrehook     = XmInheritInitializePrehook;
	  (*wcePtr)->setValuesPrehook      = XmInheritSetValuesPrehook;
	  (*wcePtr)->getValuesPrehook      = XmInheritGetValuesPrehook;
	  (*wcePtr)->initializePosthook    = XmInheritInitializePosthook;
	  (*wcePtr)->setValuesPosthook     = XmInheritSetValuesPosthook;
	  (*wcePtr)->getValuesPosthook     = XmInheritGetValuesPosthook;
	  (*wcePtr)->secondaryObjectClass  = XmInheritClass;
	  (*wcePtr)->secondaryObjectCreate = XmInheritSecObjectCreate;
	  (*wcePtr)->getSecResData         = XmInheritGetSecResData;
	  (*wcePtr)->widgetNavigable       = XmInheritWidgetNavigable;
	  (*wcePtr)->focusChange           = XmInheritFocusChange;
	}
      else
	inited = True;

      /* this should get done by the static initializers */
      for (i = 0; i < 32; i++)
	(*wcePtr)->flags[i] = 0;

      if (scePtr && *scePtr)
	{
	  if (!inited)
	    {
	      (*wcePtr)->next_extension = NULL;
	      (*wcePtr)->record_type 	= (*scePtr)->record_type;
	      (*wcePtr)->version	= (*scePtr)->version;
	      (*wcePtr)->record_size	= (*scePtr)->record_size;
	    }
	  if ((*wcePtr)->classPartInitPrehook == XmInheritClassPartInitPrehook)
	    (*wcePtr)->classPartInitPrehook = (*scePtr)->classPartInitPrehook;
	  if ((*wcePtr)->classPartInitPosthook == XmInheritClassPartInitPosthook)
	    (*wcePtr)->classPartInitPosthook = (*scePtr)->classPartInitPosthook;
	  if ((*wcePtr)->initializePrehook == XmInheritInitializePrehook)
	    (*wcePtr)->initializePrehook = (*scePtr)->initializePrehook;
	  if ((*wcePtr)->setValuesPrehook == XmInheritSetValuesPrehook)
	    (*wcePtr)->setValuesPrehook = (*scePtr)->setValuesPrehook;
	  if ((*wcePtr)->getValuesPrehook == XmInheritGetValuesPrehook)
	    (*wcePtr)->getValuesPrehook = (*scePtr)->getValuesPrehook;
	  if ((*wcePtr)->initializePosthook == XmInheritInitializePosthook)
	    (*wcePtr)->initializePosthook = (*scePtr)->initializePosthook;
	  if ((*wcePtr)->setValuesPosthook == XmInheritSetValuesPosthook)
	    (*wcePtr)->setValuesPosthook = (*scePtr)->setValuesPosthook;
	  if ((*wcePtr)->getValuesPosthook == XmInheritGetValuesPosthook)
	    (*wcePtr)->getValuesPosthook = (*scePtr)->getValuesPosthook;
	  if ((*wcePtr)->secondaryObjectClass == XmInheritClass)
	    (*wcePtr)->secondaryObjectClass = (*scePtr)->secondaryObjectClass;
	  if ((*wcePtr)->secondaryObjectCreate == XmInheritSecObjectCreate)
	    (*wcePtr)->secondaryObjectCreate = (*scePtr)->secondaryObjectCreate;
	  if ((*wcePtr)->getSecResData == XmInheritGetSecResData)
	    (*wcePtr)->getSecResData = (*scePtr)->getSecResData;
	  if ((*wcePtr)->widgetNavigable == XmInheritWidgetNavigable)
	    (*wcePtr)->widgetNavigable = (*scePtr)->widgetNavigable;
	  if ((*wcePtr)->focusChange == XmInheritFocusChange)
	    (*wcePtr)->focusChange = (*scePtr)->focusChange;
	}
#ifdef DEBUG
      else if (!IsBaseClass(wc))
	XtError("class must have non-null superclass extension");
#endif /* DEBUG */

      /*
       * If this class has a secondary object class and that
       * class does not have it's own extension (or has not
       * been class inited because its a pseudo class) then
       * we will give a dummy pointer so that fast subclass
       * checking will not fail for the meta classes
       * (gadget, manager, etc...)
       */
      {
	WidgetClass	sec = (*wcePtr)->secondaryObjectClass;
	static XmBaseClassExtRec       xmExtExtensionRec = {
	  NULL,                                     /* Next extension       */
	  NULLQUARK,                                /* record type XmQmotif */
	  XmBaseClassExtVersion,                    /* version              */
	  sizeof(XmBaseClassExtRec),                /* size                 */
	};

	if (xmExtExtensionRec.record_type == NULLQUARK)
	  xmExtExtensionRec.record_type = XmQmotif;

	if (sec && !sec->core_class.extension)
	  sec->core_class.extension = (XtPointer)&xmExtExtensionRec;
      }
    }

  wcData = GetWrapperData(wc);
  scData = GetWrapperData(wc->core_class.superclass);

  if ((wc == vendorShellWidgetClass) ||
      _XmIsSubclassOf(wc, vendorShellWidgetClass))
    {
      /* Wrap Realize */
      /*
       * check if this widget was using XtInherit and got the wrapper
       * from the superclass
       */
      if (wc->core_class.realize == XtInheritRealize)
	{
	  wcData->realize = scData->realize;
	}
      /*
       * It has declared it's own realize routine so save it
       */
      else
	{
	  wcData->realize = wc->core_class.realize;
	}
      wc->core_class.realize = realizeWrappers[GetRealizeDepth(wc)];
    }

  if ((wc == rectObjClass) ||
      _XmIsSubclassOf(wc, rectObjClass))
    {
      /* Wrap resize */
      /*
       * check if this widget was using XtInherit and got the wrapper
       * from the superclass
       */
      if (wc->core_class.resize == XtInheritResize)
	{
	  wcData->resize = scData->resize;
	}
      /*
       * It has declared it's own resize routine so save it
       */
      else
	{
	  wcData->resize = wc->core_class.resize;
	}
      wc->core_class.resize = resizeWrappers[GetResizeDepth(wc)];
    }

  if ((wc == compositeWidgetClass) ||
      _XmIsSubclassOf(wc, compositeWidgetClass))
    {
      /* Wrap GeometryManager */
      /*
       * check if this widget was using XtInherit and got the wrapper
       * from the superclass
       */
      if (((CompositeWidgetClass) wc)->composite_class.geometry_manager
	  == XtInheritGeometryManager)
	{
	  wcData->geometry_manager = scData->geometry_manager;
	}
      /*
       * It has declared it's own resize routine so save it
       */
      else
	{
	  wcData->geometry_manager = ((CompositeWidgetClass) wc)
	    ->composite_class.geometry_manager;
	}
      ((CompositeWidgetClass) wc)->composite_class.geometry_manager =
	(XtGeometryHandler)
	  geometryHandlerWrappers[GetGeometryHandlerDepth(wc)];
    }

  return wcePtr;
}

/*
 * This function replaces the objectClass classPartInit slot and is
 * called at the start of the first XtCreate invocation.
 */
static void
ClassPartInitRootWrapper(
        WidgetClass wc )
{
  XtWidgetClassProc *leafFuncPtr;
  XmBaseClassExt    *wcePtr;


  wcePtr = BaseClassPartInitialize(wc);
  /*
   * check that it's a class that we know about
   */
  if (wcePtr && *wcePtr)
    {
      if ((*wcePtr)->classPartInitPrehook)
	(*((*wcePtr)->classPartInitPrehook)) (wc);

      /*
       * If we have a prehook, then envelop the leaf class function
       * that whould be called last.
       */
      if ((*wcePtr)->classPartInitPosthook)
	{
	  XmWrapperData wrapperData;

	  wrapperData = GetWrapperData(wc);
	  leafFuncPtr = (XtWidgetClassProc *)
	    &(wc->core_class.class_part_initialize);
	  wrapperData->classPartInitLeaf = *leafFuncPtr;
	  *leafFuncPtr = ClassPartInitLeafWrapper;
	}
    }

  if (objectClassWrapper.classPartInit)
    (* objectClassWrapper.classPartInit) (wc);
}

static void
ClassPartInitLeafWrapper(
        WidgetClass wc )
{
  XtWidgetClassProc *leafFuncPtr;
  XmBaseClassExt    *wcePtr;

  wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

  if (*wcePtr && (*wcePtr)->classPartInitPosthook)
    {
      XmWrapperData wrapperData;

      wrapperData = GetWrapperData(wc);
      leafFuncPtr = (XtWidgetClassProc *)
	&(wc->core_class.class_part_initialize);

      if (wrapperData->classPartInitLeaf)
	(* wrapperData->classPartInitLeaf) (wc);
      if ((*wcePtr)->classPartInitPosthook)
	(*((*wcePtr)->classPartInitPosthook)) (wc);
#ifdef DEBUG
      else
	XmeWarning(NULL, "there should be a non-null hook for a leaf wrapper");
#endif /* DEBUG */
      *leafFuncPtr = wrapperData->classPartInitLeaf;
      wrapperData->classPartInitLeaf = NULL;
    }
}

static Boolean
is_constraint_subclass(WidgetClass cls)
{
  WidgetClass sc;

  for (sc = cls; sc != NULL; sc = sc->core_class.superclass)
    if (sc == (WidgetClass) &constraintClassRec)
      return True;

  return False;
}


void
_XmInitializeExtensions( void )
{
  static Boolean firstTime = True;

  if (firstTime)
    {
      XmQmotif = XrmPermStringToQuark("OSF_MOTIF");

      objectClassWrapper.initialize =
	objectClass->core_class.initialize;
      objectClassWrapper.setValues =
	objectClass->core_class.set_values;
      objectClassWrapper.getValues =
	objectClass->core_class.get_values_hook;
      objectClassWrapper.classPartInit =
	objectClass->core_class.class_part_initialize;
      objectClass->core_class.class_part_initialize =
	ClassPartInitRootWrapper;
      objectClass->core_class.initialize =
	InitializeRootWrapper;
      objectClass->core_class.set_values =
	SetValuesRootWrapper;
      objectClass->core_class.get_values_hook =
	GetValuesRootWrapper;
      firstTime = False;
    }
    resizeRefWContext = XUniqueContext();
    geoRefWContext = XUniqueContext();
}


Cardinal
_XmSecondaryResourceData(
        XmBaseClassExt bcePtr,
        XmSecondaryResourceData **secResDataRtn,
        XtPointer client_data,
        String name,
        String class_name,
        XmResourceBaseProc basefunctionpointer )
{
  WidgetClass		  secObjClass;
  XmSecondaryResourceData secResData, *sd;
  Cardinal 		  count = 0;

  if (bcePtr)
    {
      secObjClass = ( (bcePtr)->secondaryObjectClass);
      if (secObjClass)
	{
	  secResData = XtNew(XmSecondaryResourceDataRec);

	  _XmTransformSubResources(secObjClass->core_class.resources,
				   secObjClass->core_class.num_resources,
				   &(secResData->resources),
				   &(secResData->num_resources));

	  secResData->name = name;
	  secResData->res_class = class_name;
	  secResData->client_data = client_data;
	  secResData->base_proc = basefunctionpointer;
	  sd = XtNew(XmSecondaryResourceData);
	  *sd = secResData;
	  *secResDataRtn = sd;
	  count++;
	}
    }

  return count;
}

/*
 * This function makes assumptions about what the Intrinsics is
 * doing with the resource lists.  It is based on the X11R5 version
 * of the Intrinsics.  It is used as a work around for a bug in
 * the Intrinsics that deals with recompiling already compiled resource
 * lists.  When that bug is fixed, this function should be removed.
 */
static XtResourceList*
CreateIndirectionTable(XtResourceList resources,
			 Cardinal num_resources)
{
  register int i;
  XtResourceList* table;

  table = (XtResourceList*)XtMalloc(num_resources * sizeof(XtResourceList));
  for (i = 0; i < num_resources; i++)
    table[i] = (XtResourceList)(&(resources[i]));

  return table;
}

/*
 * The statement in this function calls CreateIndirectionTable() which
 * is scheduled for removal (see comment in function above).
 * It is used as a work around for an X11R5 Intrinsics bug.  When the
 * bug is fixed, change to the assignement statement so the
 * constraint_class.reources is assigned comp_resources.  Next,
 * change the class_inited field of the core_class record to False.
 * Then remove the check for class_inited, in the conditional statement
 * which calls XtInitializeWidgetClass() and move the call to
 * XtInitializeWidgetClass() to just above the call to
 * XtGetConstraintResourceList().  Then remove the call to
 * XtFree() and the last two assignment statements.
 */
void
_XmTransformSubResources(
	XtResourceList comp_resources,
	Cardinal num_comp_resources,
	XtResourceList *resources,
	Cardinal *num_resources)
{
   static ConstraintClassRec shadowObjectClassRec = {
      {
       /* superclass           */  (WidgetClass) &constraintClassRec,
       /* class_name           */  "Shadow",
       /* widget_size          */  sizeof(ConstraintRec),
       /* class_initialize     */  NULL,
       /* class_part_initialize*/  NULL,
       /* class_inited         */  FALSE,
       /* initialize           */  NULL,
       /* initialize_hook      */  NULL,
       /* realize              */  XtInheritRealize,
       /* actions              */  NULL,
       /* num_actions          */  0,
       /* resources            */  NULL,
       /* num_resources        */  0,
       /* xrm_class            */  NULLQUARK,
       /* compress_motion      */  FALSE,
       /* compress_exposure    */  TRUE,
       /* compress_enterleave  */  FALSE,
       /* visible_interest     */  FALSE,
       /* destroy              */  NULL,
       /* resize               */  NULL,
       /* expose               */  NULL,
       /* set_values           */  NULL,
       /* set_values_hook      */  NULL,
       /* set_values_almost    */  XtInheritSetValuesAlmost,
       /* get_values_hook      */  NULL,
       /* accept_focus         */  NULL,
       /* version              */  XtVersion,
       /* callback_offsets     */  NULL,
       /* tm_table             */  NULL,
       /* query_geometry       */  NULL,
       /* display_accelerator  */  NULL,
       /* extension            */  NULL
     },

     { /**** CompositePart *****/
       /* geometry_handler     */  NULL,
       /* change_managed       */  NULL,
       /* insert_child         */  XtInheritInsertChild,
       /* delete_child         */  XtInheritDeleteChild,
       /* extension            */  NULL
     },

     { /**** ConstraintPart ****/
       /* resources            */  NULL,
       /* num_resources        */  0,
       /* constraint_size      */  0,
       /* initialize           */  NULL,
       /* destroy              */  NULL,
       /* set_values           */  NULL,
       /* extension            */  NULL
     }
   };

   if (((int)comp_resources[0].resource_offset) >= 0)
     {
       XtResourceList tmp_resources;

       tmp_resources = (XtResourceList)
	 XtMalloc(sizeof(XtResource) * num_comp_resources);

       memcpy(tmp_resources, comp_resources,
	      sizeof(XtResource) * num_comp_resources);

       *resources = tmp_resources;
       *num_resources = num_comp_resources;
     }
   else
     {
       if (!shadowObjectClassRec.core_class.class_inited)
	 XtInitializeWidgetClass((WidgetClass) &shadowObjectClassRec);

       /* This next statement is marked for change */
       shadowObjectClassRec.constraint_class.resources = (XtResourceList)
	 CreateIndirectionTable(comp_resources, num_comp_resources);

       shadowObjectClassRec.constraint_class.num_resources = num_comp_resources;

       XtGetConstraintResourceList((WidgetClass) &shadowObjectClassRec,
				   resources, num_resources);

       if (shadowObjectClassRec.constraint_class.resources)
	 XtFree((char *) shadowObjectClassRec.constraint_class.resources);

       shadowObjectClassRec.constraint_class.resources = NULL;
       shadowObjectClassRec.constraint_class.num_resources = 0;
     }
}

static XtInitProc InitializeLeafWrappers[] = {
	InitializeLeafWrapper0,
	InitializeLeafWrapper1,
	InitializeLeafWrapper2,
	InitializeLeafWrapper3,
	InitializeLeafWrapper4,
	InitializeLeafWrapper5,
	InitializeLeafWrapper6,
	InitializeLeafWrapper7,
	InitializeLeafWrapper8,
	InitializeLeafWrapper9
};
static XtInitProc CInitializeLeafWrappers[] = {
	CInitializeLeafWrapper0,
	CInitializeLeafWrapper1,
	CInitializeLeafWrapper2,
	CInitializeLeafWrapper3,
	CInitializeLeafWrapper4,
	CInitializeLeafWrapper5,
	CInitializeLeafWrapper6,
	CInitializeLeafWrapper7,
	CInitializeLeafWrapper8,
	CInitializeLeafWrapper9
};

/*
 * This function replaces the objectClass initialize slot and is
 * called at the start of every XtCreate invocation.
 */
static void
InitializeRootWrapper(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args)
{
	WidgetClass wc = XtClass(new_w);
	XmBaseClassExt *wcePtr;

	wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	if (wcePtr && *wcePtr) {

	   if ((*wcePtr)->initializePrehook)
		(*((*wcePtr)->initializePrehook))(req, new_w, args, num_args);


	   if ((*wcePtr)->initializePosthook) {
		XmWrapperData wrapperData;

	   	_XmProcessLock();

		if (!XtIsShell(new_w) && XtParent(new_w)
			&& XtIsConstraint(XtParent(new_w))) {
			ConstraintWidgetClass cwc;

			cwc = (ConstraintWidgetClass) XtClass(XtParent(new_w));
			wrapperData = GetWrapperData((WidgetClass) cwc);
			if (wrapperData->constraintInitializeLeafCount ==0)
			{
				wrapperData->constraintInitializeLeaf =
					cwc->constraint_class.initialize;
				cwc->constraint_class.initialize =
					CInitializeLeafWrappers[
						GetDepth((WidgetClass) cwc)];
			}
			(wrapperData->constraintInitializeLeafCount)++;
		}
		else {
			wrapperData = GetWrapperData(wc);
			if (wrapperData->initializeLeafCount ==0) {
				wrapperData->initializeLeaf =
					wc->core_class.initialize;
				wc->core_class.initialize =
					InitializeLeafWrappers[
						GetDepth(wc)];
			}
			(wrapperData->initializeLeafCount)++;
		}

		_XmProcessUnlock();
	   }

	   if (objectClassWrapper.initialize)
		(*objectClassWrapper.initialize)(req, new_w,args, num_args);
	}
}

static void
InitializeLeafWrapper(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args,
			int depth)
{
	WidgetClass wc = XtClass(new_w);
	XtInitProc init_proc = NULL;
	XtInitProc post_proc = NULL;
	int leafDepth = GetDepth(wc);
	XmWrapperData wrapperData;

	_XmProcessLock();

	if (leafDepth == depth) { /* Correct depth */
	   wrapperData = GetWrapperData(wc);

	   if (!XtIsShell(new_w) && XtParent(new_w) &&
			XtIsConstraint(XtParent(new_w))) {
		init_proc = wrapperData->initializeLeaf;
	   }
	   else {
		/* We're home ! */
	   	XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
		init_proc = wrapperData->initializeLeaf;
		if ((post_proc = (*wcePtr)->initializePosthook)) {
		    if ((--(wrapperData->initializeLeafCount)) == 0)
			wc->core_class.initialize = wrapperData->initializeLeaf;
		}
	   }
	}
	else {
		int depthDiff = leafDepth - depth;

		for ( ; depthDiff;
		        depthDiff--, wc = wc->core_class.superclass)
			{};

		wrapperData = GetWrapperData(wc);
		init_proc = wrapperData->initializeLeaf;
	}

	_XmProcessUnlock();

	if (init_proc)
		(*init_proc)(req, new_w, args, num_args);
	if (post_proc)
		(*post_proc)(req, new_w, args, num_args);
}

static void
CInitializeLeafWrapper(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args,
			int depth)
{
	WidgetClass wc = XtClass(new_w);
	ConstraintWidgetClass cwc = (ConstraintWidgetClass)
					XtClass(XtParent(new_w));
	XtInitProc init_proc = NULL;
	XtInitProc post_proc = NULL;
	int leafDepth = GetDepth((WidgetClass) cwc);
	XmWrapperData wrapperData;

	_XmProcessLock();

	if (leafDepth == depth) {
		XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
		wrapperData = GetWrapperData((WidgetClass) cwc);

		init_proc = wrapperData->constraintInitializeLeaf;
		if ((post_proc = (*wcePtr)->initializePosthook)) {
		    if ((--(wrapperData->constraintInitializeLeafCount)) ==0)
			cwc->constraint_class.initialize =
				wrapperData->constraintInitializeLeaf;
		}
	}
	else {
		int depthDiff = leafDepth - depth;

		for ( ; depthDiff;
		        depthDiff--, cwc = (ConstraintWidgetClass)
		                           cwc->core_class.superclass)
			{};

		wrapperData = GetWrapperData((WidgetClass) cwc);
		init_proc = wrapperData->constraintInitializeLeaf;
	}

	_XmProcessUnlock();

	if (init_proc)
		(*init_proc)(req, new_w, args, num_args);
	if (post_proc)
		(*post_proc)(req, new_w, args, num_args);
}

static void
InitializeLeafWrapper0(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 0);
}
static void
InitializeLeafWrapper1(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 1);
}
static void
InitializeLeafWrapper2(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 2);
}
static void
InitializeLeafWrapper3(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 3);
}
static void
InitializeLeafWrapper4(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 4);
}
static void
InitializeLeafWrapper5(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 5);
}
static void
InitializeLeafWrapper6(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 6);
}
static void
InitializeLeafWrapper7(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 7);
}
static void
InitializeLeafWrapper8(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 8);
}
static void
InitializeLeafWrapper9(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	InitializeLeafWrapper(req, new_w, args, num_args, 9);
}

static void
CInitializeLeafWrapper0(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,0);
}
static void
CInitializeLeafWrapper1(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,1);
}
static void
CInitializeLeafWrapper2(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,2);
}
static void
CInitializeLeafWrapper3(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,3);
}
static void
CInitializeLeafWrapper4(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,4);
}
static void
CInitializeLeafWrapper5(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,5);
}
static void
CInitializeLeafWrapper6(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,6);
}
static void
CInitializeLeafWrapper7(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,7);
}
static void
CInitializeLeafWrapper8(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,8);
}
static void
CInitializeLeafWrapper9(
	Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
	CInitializeLeafWrapper(req, new_w, args, num_args,9);
}

static XtSetValuesFunc SetValuesLeafWrappers[] = {
	SetValuesLeafWrapper0,
	SetValuesLeafWrapper1,
	SetValuesLeafWrapper2,
	SetValuesLeafWrapper3,
	SetValuesLeafWrapper4,
	SetValuesLeafWrapper5,
	SetValuesLeafWrapper6,
	SetValuesLeafWrapper7,
	SetValuesLeafWrapper8,
	SetValuesLeafWrapper9
};
static XtSetValuesFunc CSetValuesLeafWrappers[] = {
	CSetValuesLeafWrapper0,
	CSetValuesLeafWrapper1,
	CSetValuesLeafWrapper2,
	CSetValuesLeafWrapper3,
	CSetValuesLeafWrapper4,
	CSetValuesLeafWrapper5,
	CSetValuesLeafWrapper6,
	CSetValuesLeafWrapper7,
	CSetValuesLeafWrapper8,
	CSetValuesLeafWrapper9
};

/*
 * This function replaces the objectClass set_values slot and is
 * called at the start of every XtSetValues invocation.
 */
static Boolean
SetValuesRootWrapper(
			Widget current,
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num_args)
{
	WidgetClass wc = XtClass(new_w);
	XmBaseClassExt *wcePtr;
	Boolean returnVal = False;

	wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	if (wcePtr && *wcePtr) {

	   if ((*wcePtr)->setValuesPrehook)
		returnVal |= (*((*wcePtr)->setValuesPrehook))
				(current, req, new_w, args,  num_args);

	   if ((*wcePtr)->setValuesPosthook) {
	    	XmWrapperData wrapperData;

	    	_XmProcessLock();

	    	if (!XtIsShell(new_w) && XtParent(new_w)
		    	&& XtIsConstraint(XtParent(new_w))) {
	    		ConstraintWidgetClass cwc;

			cwc = (ConstraintWidgetClass) XtClass(XtParent(new_w));
			wrapperData = GetWrapperData((WidgetClass) cwc);
			if (wrapperData->constraintSetValuesLeafCount ==0)
			{
				wrapperData->constraintSetValuesLeaf =
					cwc->constraint_class.set_values;
				cwc->constraint_class.set_values =
					CSetValuesLeafWrappers[
						GetDepth((WidgetClass) cwc)];
			}
			(wrapperData->constraintSetValuesLeafCount)++;
		}
		else {
			wrapperData = GetWrapperData(wc);
			if (wrapperData->setValuesLeafCount ==0) {
				wrapperData->setValuesLeaf =
					wc->core_class.set_values;
				wc->core_class.set_values =
					SetValuesLeafWrappers[
						GetDepth(wc)];
			}
			(wrapperData->setValuesLeafCount)++;
		}

	   	_XmProcessUnlock();
	   }
	}

	if (objectClassWrapper.setValues)
	    returnVal |= (*objectClassWrapper.setValues)
				(current, req, new_w, args, num_args);

	return returnVal;
}

static Boolean
SetValuesLeafWrapper(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth)
{
	WidgetClass wc = XtClass(new_w);
	XtSetValuesFunc setvalues_proc = NULL;
	XtSetValuesFunc post_proc = NULL;
	Boolean returnVal = False;
	int leafDepth = GetDepth(wc);
	XmWrapperData wrapperData;

	_XmProcessLock();

	if (leafDepth == depth) { /* Correct depth */
	   wrapperData = GetWrapperData(wc);

	   if (!XtIsShell(new_w) && XtParent(new_w) &&
			   XtIsConstraint(XtParent(new_w))) {
		setvalues_proc = wrapperData->setValuesLeaf;
	   }
	   else {
		/* We're home ! */
	   	XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
		setvalues_proc = wrapperData->setValuesLeaf;
		if (!(post_proc = (*wcePtr)->setValuesPosthook)) {
		    if ((--(wrapperData->setValuesLeafCount)) ==0)
			wc->core_class.set_values =
				wrapperData->setValuesLeaf;
		}
	   }
	}
	else {
		int depthDiff = leafDepth - depth;

		for ( ; depthDiff;
			depthDiff--, wc = wc->core_class.superclass)
			{};

		wrapperData = GetWrapperData(wc);
		setvalues_proc = wrapperData->setValuesLeaf;
	}

	_XmProcessUnlock();

	if (setvalues_proc)
		returnVal |= (*setvalues_proc)
				(current, req, new_w, args, num_args);
	if (post_proc)
		returnVal |= (*post_proc)
				(current, req, new_w, args, num_args);

	return returnVal;
}

static Boolean
CSetValuesLeafWrapper(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth)
{
	WidgetClass wc = XtClass(new_w);
	ConstraintWidgetClass cwc = (ConstraintWidgetClass)
					XtClass(XtParent(new_w));
	XtSetValuesFunc setvalues_proc = NULL;
	XtSetValuesFunc post_proc = NULL;
	Boolean returnVal = False;
	int leafDepth = GetDepth((WidgetClass) cwc);
	XmWrapperData wrapperData;

	_XmProcessLock();

	if (leafDepth == depth) {
		XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
		wrapperData = GetWrapperData((WidgetClass) cwc);
		setvalues_proc = wrapperData->constraintSetValuesLeaf;
		if ((post_proc = (*wcePtr)->setValuesPosthook)) {
		    if ((--(wrapperData->constraintSetValuesLeafCount)) ==0)
			cwc->constraint_class.set_values =
				wrapperData->constraintSetValuesLeaf;
		}
	}
	else {
		int depthDiff = leafDepth - depth;

	 	for ( ; depthDiff;
		 	depthDiff--, cwc = (ConstraintWidgetClass)
		                           cwc->core_class.superclass)
			{};

		wrapperData = GetWrapperData((WidgetClass) cwc);
		setvalues_proc = wrapperData->constraintSetValuesLeaf;
	}

	_XmProcessUnlock();

	if (setvalues_proc)
		returnVal |= (*setvalues_proc)
				(current, req, new_w, args, num_args);
	if (post_proc)
		returnVal |= (*post_proc)
				(current, req, new_w, args, num_args);

	return returnVal;
}

static Boolean
SetValuesLeafWrapper0(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args )
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 0);
}
static Boolean
SetValuesLeafWrapper1(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 1);
}
static Boolean
SetValuesLeafWrapper2(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 2);
}
static Boolean
SetValuesLeafWrapper3(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 3);
}
static Boolean
SetValuesLeafWrapper4(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 4);
}
static Boolean
SetValuesLeafWrapper5(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 5);
}
static Boolean
SetValuesLeafWrapper6(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 6);
}
static Boolean
SetValuesLeafWrapper7(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 7);
}
static Boolean
SetValuesLeafWrapper8(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 8);
}
static Boolean
SetValuesLeafWrapper9(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return SetValuesLeafWrapper(current, req, new_w, args, num_args, 9);
}

static Boolean
CSetValuesLeafWrapper0(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 0);
}
static Boolean
CSetValuesLeafWrapper1(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 1);
}
static Boolean
CSetValuesLeafWrapper2(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 2);
}
static Boolean
CSetValuesLeafWrapper3(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 3);
}
static Boolean
CSetValuesLeafWrapper4(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 4);
}
static Boolean
CSetValuesLeafWrapper5(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 5);
}
static Boolean
CSetValuesLeafWrapper6(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 6);
}
static Boolean
CSetValuesLeafWrapper7(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 7);
}
static Boolean
CSetValuesLeafWrapper8(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 8);
}
static Boolean
CSetValuesLeafWrapper9(
	Widget current, Widget req, Widget new_w, ArgList args,
		Cardinal *num_args)
{
	return CSetValuesLeafWrapper(current, req, new_w, args, num_args, 9);
}

static XtArgsProc GetValuesLeafWrappers[] = {
	GetValuesLeafWrapper0,
	GetValuesLeafWrapper1,
	GetValuesLeafWrapper2,
	GetValuesLeafWrapper3,
	GetValuesLeafWrapper4,
	GetValuesLeafWrapper5,
	GetValuesLeafWrapper6,
	GetValuesLeafWrapper7,
	GetValuesLeafWrapper8,
	GetValuesLeafWrapper9
};

/*
 * This function replaces the objectClass get_values slot and is
 * called at the start of every XtGetValues invocation.
 */
static void
GetValuesRootWrapper(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args)
{
	WidgetClass wc = XtClass(new_w);
	XmBaseClassExt *wcePtr;

	wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	if (wcePtr && *wcePtr) {

	   if ((*wcePtr)->getValuesPrehook)
		(*((*wcePtr)->getValuesPrehook))(new_w, args, num_args);

	   if ((*wcePtr)->getValuesPosthook) {
		XmWrapperData wrapperData;

		_XmProcessLock();

		wrapperData = GetWrapperData(wc);
		if (wrapperData->getValuesLeafCount == 0) {
			wrapperData->getValuesLeaf =
				wc->core_class.get_values_hook;
			wc->core_class.get_values_hook =
				GetValuesLeafWrappers[GetDepth(wc)];
		}
		(wrapperData->getValuesLeafCount)++;

		_XmProcessUnlock();
	   }
	}
	if (objectClassWrapper.getValues)
		(*objectClassWrapper.getValues) (new_w, args, num_args);
}

static void
GetValuesLeafWrapper(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args,
			int depth)
{
	WidgetClass wc = XtClass(new_w);
	XtArgsProc getvalues_proc = NULL;
	XtArgsProc post_proc = NULL;
	int leafDepth = GetDepth(wc);
	XmWrapperData wrapperData;

	_XmProcessLock();

	if (leafDepth == depth) { /* Correct depth */
	   XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	   wrapperData = GetWrapperData(wc);
	   getvalues_proc = wrapperData->getValuesLeaf;
	   if ((post_proc = ((*wcePtr)->getValuesPosthook))) {
	    if ((--(wrapperData->getValuesLeafCount)) == 0)
		wc->core_class.get_values_hook =
			wrapperData->getValuesLeaf;
	   }
	}
	else {
		int depthDiff = leafDepth - depth;

		for ( ; depthDiff;
			depthDiff--, wc = wc->core_class.superclass)
			{};

		wrapperData = GetWrapperData(wc);
		getvalues_proc = wrapperData->getValuesLeaf;
	}

	_XmProcessUnlock();

	if (getvalues_proc)
		(*getvalues_proc)(new_w, args, num_args);
	if (post_proc)
		(*post_proc)(new_w, args,  num_args);
}

static void
GetValuesLeafWrapper0( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 0);
}
static void
GetValuesLeafWrapper1( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 1);
}
static void
GetValuesLeafWrapper2( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 2);
}
static void
GetValuesLeafWrapper3( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 3);
}
static void
GetValuesLeafWrapper4( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 4);
}
static void
GetValuesLeafWrapper5( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 5);
}
static void
GetValuesLeafWrapper6( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 6);
}
static void
GetValuesLeafWrapper7( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 7);
}
static void
GetValuesLeafWrapper8( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 8);
}
static void
GetValuesLeafWrapper9( Widget new_w, ArgList args,Cardinal *num_args)
{
	GetValuesLeafWrapper(new_w, args, num_args, 9);
}

static int
GetDepth(WidgetClass wc)
{
	int i;

	for (i = 0;
	     wc && wc != rectObjClass;
	     i++, wc = wc->core_class.superclass) {};

	if (wc)
		return i;
	else
		return 0;
}

/*
 * These symbols must always be present so applications compiling with
 * -DXTHREADS can still link against libraries built without it.  How
 * those applications recognize non MT-safe libraries is a different
 * issue.
 */
inline void _XmFastSubclassInit(WidgetClass wc, unsigned int bit)
{
	XmBaseClassExt *basePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	if (basePtr && (*basePtr))
		_XmSetFlagsBit(((*basePtr)->flags), bit);
}

Boolean inline
_XmIsFastSubclass(WidgetClass wc, unsigned int bit)
{
	XmBaseClassExt *basePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

	if (!basePtr || !(*basePtr))
		return False;

	if (_XmGetFlagsBit(((*basePtr)->flags), bit))
		return True;
	else
		return False;
}

