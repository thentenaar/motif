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
static char rcsid[] = "$XConsortium: ShellE.c /main/10 1995/10/25 20:19:54 cde-sun $"
#endif
#endif

#include <Xm/ShellEP.h>
#include <X11/ShellP.h>
#include <Xm/VendorSEP.h>
#include <Xm/ScreenP.h>
#include "XmI.h"

/********    Static Function Declarations    ********/

static void ShellClassPartInitialize(
                        WidgetClass w) ;
static void StructureNotifyHandler(
                        Widget wid,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *continue_to_dispatch) ;

/********    End Static Function Declarations    ********/


/***************************************************************************
 *
 * Class Record
 *
 ***************************************************************************/

#define Offset(field) XtOffsetOf( struct _XmShellExtRec, shell.field)

static XtResource shellResources[] =
{
    {
	XmNuseAsyncGeometry, XmCUseAsyncGeometry, XmRBoolean,
	sizeof(Boolean), Offset(useAsyncGeometry),
	XmRImmediate, FALSE,
    },
};
#undef Offset

externaldef(xmshellextclassrec)
XmShellExtClassRec xmShellExtClassRec = {
    {
	(WidgetClass) &xmDesktopClassRec,/* superclass		*/
	"Shell",			/* class_name 		*/
	sizeof(XmShellExtRec),	 	/* size 		*/
	NULL,		 		/* Class Initializer 	*/
	ShellClassPartInitialize, 	/* class_part_init 	*/
	FALSE, 				/* Class init'ed ? 	*/
	NULL,				/* initialize         	*/
	NULL, 				/* initialize_notify    */
	NULL,	 			/* realize            	*/
	NULL,	 			/* actions            	*/
	0,				/* num_actions        	*/
	shellResources,			/* resources          	*/
	XtNumber(shellResources),	/* resource_count     	*/
	NULLQUARK, 			/* xrm_class          	*/
	FALSE, 				/* compress_motion    	*/
	FALSE, 				/* compress_exposure  	*/
	FALSE, 				/* compress_enterleave	*/
	FALSE, 				/* visible_interest   	*/
	NULL,				/* destroy            	*/
	NULL,		 		/* resize             	*/
	NULL, 				/* expose             	*/
	NULL,		 		/* set_values         	*/
	NULL, 				/* set_values_hook      */
	NULL,			 	/* set_values_almost    */
	NULL,				/* get_values_hook      */
	NULL, 				/* accept_focus       	*/
	XtVersion, 			/* intrinsics version 	*/
	NULL, 				/* callback offsets   	*/
	NULL,				/* tm_table           	*/
	NULL, 				/* query_geometry       */
	NULL, 				/* display_accelerator  */
	NULL, 				/* extension            */
    },
    {					/* ext */
	NULL,				/* synthetic resources	*/
	0,				/* num syn resources	*/
	NULL,				/* extension		*/
    },
    {					/* desktop		*/
	NULL,				/* child_class		*/
	XtInheritInsertChild,		/* insert_child		*/
	XtInheritDeleteChild,		/* delete_child		*/
	NULL,				/* extension		*/
    },
    {					/* shell ext		*/
	StructureNotifyHandler,	        /* structureNotify*/
	NULL,				/* extension		*/
    },
};

externaldef(xmShellExtobjectclass) WidgetClass
  xmShellExtObjectClass = (WidgetClass) (&xmShellExtClassRec);


/************************************************************************
 *
 *  ClassPartInitialize
 *    Set up the inheritance mechanism for the routines exported by
 *    vendorShells class part.
 *
 ************************************************************************/
static void
ShellClassPartInitialize(
        WidgetClass w )
{
    XmShellExtObjectClass wc = (XmShellExtObjectClass) w;
    XmShellExtObjectClass sc =
      (XmShellExtObjectClass) wc->object_class.superclass;

    if (wc == (XmShellExtObjectClass)xmShellExtObjectClass)
      return;

    if (wc->shell_class.structureNotifyHandler == XmInheritEventHandler)
      wc->shell_class.structureNotifyHandler =
	sc->shell_class.structureNotifyHandler;
}

/************************************************************************
 *
 *  StructureNotifyHandler
 *
 ************************************************************************/
static void
StructureNotifyHandler(
        Widget wid,
        XtPointer closure,
        XEvent *event,
        Boolean *continue_to_dispatch )
{
    ShellWidget 	w = (ShellWidget) wid;
    WMShellWidget 		wmshell = (WMShellWidget) w;
    Boolean  			sizechanged = False, mwm = False;
    Position 			tmpx, tmpy;
    XmShellExtObject		shellExt = (XmShellExtObject) closure;
    XmVendorShellExtObject	vendorExt = (XmVendorShellExtObject)closure;
    XmVendorShellExtPart	*vePPtr;
    XmScreen			xmScreen;

    /*
     *  for right now if this is being used by overrides bug out
     */
    if (!XmIsVendorShell(wid))
      return;
    else
      vePPtr = (XmVendorShellExtPart *) &(vendorExt->vendor);

    _XmDisplayToAppContext(XtDisplayOfObject(wid));
    if (XmIsScreen(vendorExt->desktop.parent))
      xmScreen = (XmScreen) (vendorExt->desktop.parent);
    else
      xmScreen = XmScreenOfObject(wid);

    switch(event->type) {
      case MapNotify:
	break;
      case UnmapNotify:
	/*
	 * try to keep the pop up field synced up so it won't disallow
	 * a new pop up request.
	 */
	/*
	 * make sure we have good coords
	 */
	XtTranslateCoords((Widget) w, 0, 0, &tmpx, &tmpy);
	/*
	 * if the offsets match up, then offset our values so we'll go in
	 * the same place the next time.
	 */
	if ((vePPtr->xAtMap != w->core.x) ||
	    (vePPtr->yAtMap != w->core.y))
	  {
	      _XmAppLock(app);
	      mwm = xmScreen->screen.mwmPresent;
	      _XmAppUnlock(app);
	      if (mwm)
		{
		    if (vePPtr->lastOffsetSerial &&
			(vePPtr->lastOffsetSerial >=
			 vendorExt->shell.lastConfigureRequest) &&
			((vePPtr->xOffset + vePPtr->xAtMap) == w->core.x) &&
			((vePPtr->yOffset + vePPtr->yAtMap) == w->core.y))
		      {
			  w->core.x -= vePPtr->xOffset;
			  w->core.y -= vePPtr->yOffset;
			  w->shell.client_specified &= ~_XtShellPositionValid;

			  vePPtr->externalReposition = False;
		      }
		    else
		      {
			  vePPtr->externalReposition = True;
		      }
		}
	      else
		vePPtr->externalReposition = True;
	  }
	break;

      case ConfigureNotify:
	/*
	 * only process configureNotifies that aren't stale
	 */
	if (event->xany.serial <
	    shellExt->shell.lastConfigureRequest)
	  {
	      /*
	       *  make sure the hard wired event handler in shell is not called
	       */
	      if (shellExt->shell.useAsyncGeometry)
		*continue_to_dispatch = False;
	  }
	else
	  {
#define NEQ(x)	( w->core.x != event->xconfigure.x )
	      if( NEQ(width) || NEQ(height) || NEQ(border_width) ) {
		  sizechanged = TRUE;
	      }
#undef NEQ
	      w->core.width = event->xconfigure.width;
	      w->core.height = event->xconfigure.height;
	      w->core.border_width = event->xconfigure.border_width;
	      if (event->xany.send_event /* ICCCM compliant synthetic ev */
		  /* || w->shell.override_redirect */
		  || w->shell.client_specified & _XtShellNotReparented)
		{
		    w->core.x = event->xconfigure.x;
		    w->core.y = event->xconfigure.y;
		    w->shell.client_specified |= _XtShellPositionValid;
		}
	      else w->shell.client_specified &= ~_XtShellPositionValid;
	      if (XtIsWMShell(wid) && !wmshell->wm.wait_for_wm) {
		  /* Consider trusting the wm again */
		  WMShellPart *wmp = &(wmshell->wm);
#define EQ(x) (wmp->size_hints.x == w->core.x)
		  if (EQ(x) && EQ(y) && EQ(width) && EQ(height)) {
		      wmshell->wm.wait_for_wm = TRUE;
		  }
#undef EQ
	      }
	  }
	break;
      case ReparentNotify:
	if (event->xreparent.window == XtWindow(w)) {
	    if (event->xreparent.parent != RootWindowOfScreen(XtScreen(w)))
	      {
		  w->shell.client_specified &= ~_XtShellNotReparented;
		  /*
		   * check to see if it's mwm
		   */
		  _XmAppLock(app);
		  if (!(xmScreen->screen.numReparented++))
		    xmScreen->screen.mwmPresent = XmIsMotifWMRunning((Widget)w);
		  _XmAppUnlock(app);
	      }
	    else
	      {
		  w->core.x = event->xreparent.x;
		  w->core.y = event->xreparent.y;
		  w->shell.client_specified |= _XtShellNotReparented;
		  _XmAppLock(app);
		  xmScreen->screen.numReparented--;
		  _XmAppUnlock(app);
	      }
	    w->shell.client_specified &= ~_XtShellPositionValid;
	}
	return;

      default:
	return;
    }

    if (sizechanged) {
      XtWidgetProc resize;

      _XmProcessLock();
      resize = XtClass(w)->core_class.resize;
      _XmProcessUnlock();

      if (resize != (XtWidgetProc) NULL)
	(*resize)((Widget) w);
    }
}

