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
static char rcsid[] = "$TOG: Display.c /main/23 1997/06/18 17:36:59 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>

#include <X11/Xatom.h>
#include <Xm/AtomMgr.h>
#include <Xm/DisplayP.h>
#include <Xm/DropTransP.h>
#include <Xm/ScreenP.h>
#include <Xm/TransltnsP.h>
#include "XmI.h"
#include "DisplayI.h"
#include "DragBSI.h"
#include "DragCI.h"
#include "DragICCI.h"
#include "MessagesI.h"
#include "VirtKeysI.h"
#include "ColorObjI.h"

#define MESSAGE1 _XmMMsgDisplay_0001
#define MESSAGE2 _XmMMsgDisplay_0002
#define MESSAGE3 _XmMMsgDisplay_0003

#define TheDisplay(dd) (XtDisplayOfObject((Widget)dd))
#define TheScreen(dd) (XtScreen((Widget)dd))

#define Offset(x) (XtOffsetOf( struct _XmDisplayRec, x))

#define CHECK_TIME(dc, time) \
  ((dc->drag.dragStartTime <= time) && \
   ((dc->drag.dragFinishTime == 0) || (time <= dc->drag.dragFinishTime)))

#define INVALID_PROTOCOL_VALUE 255


/********    Static Function Declarations    ********/

static void DisplayClassPartInitialize(
                        WidgetClass wc) ;
static void DisplayClassInitialize( void ) ;
static void SetDragReceiverInfo(
                        Widget w,
                        XtPointer client_data,
                        XEvent *event,
                        Boolean *dontSwallow) ;
static void TreeUpdateHandler(
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void DisplayInitialize(
                        Widget requested_widget,
                        Widget new_widget,
                        ArgList args,
                        Cardinal *num_args) ;
static void DisplayInsertChild(
                        Widget w) ;
static void DisplayDeleteChild(
                        Widget w) ;
static void DisplayDestroy(
                        Widget w) ;
static XmDragContext FindDC(
                        XmDisplay xmDisplay,
                        Time time,
                        Boolean sourceIsExternal) ;
static int isMine(
                        Display *dpy,
                        register XEvent *event,
                        char *arg) ;
static void ReceiverShellExternalSourceHandler(
                        Widget w,
                        XtPointer client_data,
                        XEvent *event,
                        Boolean *dontSwallow) ;
static Widget GetDisplay(
                        Display *display) ;

/********    End Static Function Declarations    ********/

externaldef(displaymsg) String	_Xm_MOTIF_DRAG_AND_DROP_MESSAGE = NULL;

static XContext	displayContext = 0;
static WidgetClass curDisplayClass = NULL;

static XtResource resources[] = {
    {
	XmNdropSiteManagerClass, XmCDropSiteManagerClass, XmRWidgetClass,
	sizeof(WidgetClass), Offset(display.dropSiteManagerClass),
	XmRImmediate, (XtPointer)&xmDropSiteManagerClassRec,
    },
    {
	XmNdropTransferClass, XmCDropTransferClass, XmRWidgetClass,
	sizeof(WidgetClass), Offset(display.dropTransferClass),
	XmRImmediate, (XtPointer)&xmDropTransferClassRec,
    },
    {
	XmNdragContextClass, XmCDragContextClass, XmRWidgetClass,
	sizeof(WidgetClass), Offset(display.dragContextClass),
	XmRImmediate, (XtPointer)&xmDragContextClassRec,
    },
    {
	XmNdragInitiatorProtocolStyle, XmCDragInitiatorProtocolStyle,
	XmRDragInitiatorProtocolStyle, sizeof(unsigned char),
	Offset(display.dragInitiatorProtocolStyle),
	XmRImmediate, (XtPointer)XmDRAG_PREFER_RECEIVER,
    },
    {
	XmNdragReceiverProtocolStyle, XmCDragReceiverProtocolStyle,
	XmRDragReceiverProtocolStyle, sizeof(unsigned char),
	Offset(display.dragReceiverProtocolStyle),
	XmRImmediate, (XtPointer) XmDRAG_PREFER_PREREGISTER,
    },
    {
	XmNdefaultVirtualBindings, XmCDefaultVirtualBindings,
	XmRString, sizeof(String),
	Offset(display.bindingsString),
	XmRImmediate, (XtPointer)NULL,
    },
    {
        XmNuserData, XmCUserData, XmRPointer,
        sizeof(XtPointer), Offset(display.user_data),
        XmRImmediate, (XtPointer) NULL
    },
    {
        XmNmotifVersion, XmCMotifVersion, XmRInt,
        sizeof(int), Offset(display.motif_version),
        XmRImmediate, (XtPointer) XmVersion
    },
    {
        XmNenableWarp, XmCEnableWarp, XmREnableWarp,
        sizeof(XtEnum), Offset(display.enable_warp),
        XmRImmediate, (XtPointer) True
    },

    {
        XmNdragStartCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(display.dragStartCallback),
        XmRImmediate, (XtPointer)NULL,
    },

    {
        XmNnoFontCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(display.noFontCallback),
        XmRImmediate, (XtPointer)NULL,
    },

    {
        XmNnoRenditionCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(display.noRenditionCallback),
        XmRImmediate, (XtPointer)NULL,
    },

    {
        XmNenableBtn1Transfer, XmCEnableBtn1Transfer,
        XmREnableBtn1Transfer, sizeof(XtEnum),
	Offset(display.enable_btn1_transfer),
	XmRImmediate, (XtPointer) XmOFF
    },

    {
        XmNenableButtonTab, XmCEnableButtonTab,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_button_tab),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNenableEtchedInMenu, XmCEnableEtchedInMenu,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_etched_in_menu),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNdefaultButtonEmphasis, XmCDefaultButtonEmphasis,
	XmRDefaultButtonEmphasis, sizeof(XtEnum),
	Offset(display.default_button_emphasis),
	XmRImmediate, (XtPointer) XmEXTERNAL_HIGHLIGHT
    },

    {
        XmNenableToggleColor, XmCEnableToggleColor,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_toggle_color),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNenableToggleVisual, XmCEnableToggleVisual,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_toggle_visual),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNenableDragIcon, XmCEnableDragIcon,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_drag_icon),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNenableUnselectableDrag, XmCEnableUnselectableDrag,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_unselectable_drag),
	XmRImmediate, (XtPointer) True
    },

    {
        XmNenableThinThickness, XmCEnableThinThickness,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_thin_thickness),
	XmRImmediate, (XtPointer) False
    },

    {
        XmNenableMultiKeyBindings, XmCEnableMultiKeyBindings,
	XmRBoolean, sizeof(Boolean),
	Offset(display.enable_multi_key_bindings),
        XmRImmediate, (XtPointer) False
    },

    /* Force this resource to avoid loop in the pixmap conversion.
       XmDisplay is a VendorShell, so when iconPixmap is set in
       a resource file, the VendorShell converter is called, but
       since it needs an XmDisplay (via an XmScreen resource), it
       goes and create the XmDisplay, for which the iconPixmap is
       also converted: loop. By forcing it to Bitmap, the screen
       bitmapConversionModel is not needed and it works. This is a
       small limitation in XmDisplay */
    {   XmNiconPixmap, XmCIconPixmap, XmRBitmap,
	sizeof(Pixmap),
	XtOffsetOf(WMShellRec, wm.wm_hints.icon_pixmap),
	XmRImmediate, NULL
    },
};

#undef Offset

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    (XtInitProc)NULL,			/* InitializePrehook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPrehook	*/
    (XtInitProc)NULL,			/* InitializePosthook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    (XtInitProc)NULL,			/* secondaryCreate	*/
    (XmGetSecResDataFunc)NULL,        	/* getSecRes data	*/
    { 0 },     				/* fastSubclass flags	*/
    (XtArgsProc)NULL,			/* getValuesPrehook	*/
    (XtArgsProc)NULL,			/* getValuesPosthook	*/
    (XtWidgetClassProc)NULL,               /* classPartInitPrehook */
    (XtWidgetClassProc)NULL,               /* classPartInitPosthook*/
    NULL,               /* ext_resources        */
    NULL,               /* compiled_ext_resources*/
    0,                  /* num_ext_resources    */
    FALSE,              /* use_sub_resources    */
    (XmWidgetNavigableProc)NULL,               /* widgetNavigable      */
    (XmFocusChangeProc)NULL,               /* focusChange          */
    (XmWrapperData)NULL	/* wrapperData		*/
};


externaldef(xmdisplayclassrec)
XmDisplayClassRec xmDisplayClassRec = {
    {
	(WidgetClass) &applicationShellClassRec,	/* superclass		*/
	"XmDisplay",			/* class_name 		*/
	sizeof(XmDisplayRec),	 	/* size 		*/
	DisplayClassInitialize,		/* Class Initializer 	*/
	DisplayClassPartInitialize,	/* class_part_init 	*/
	FALSE, 				/* Class init'ed ? 	*/
	DisplayInitialize,		/* initialize         	*/
 	_XmColorObjCreate,		/* initialize_notify    */
	XtInheritRealize,		/* realize            	*/
	NULL,	 			/* actions            	*/
	0,				/* num_actions        	*/
	resources,			/* resources          	*/
	XtNumber(resources),		/* resource_count     	*/
	NULLQUARK, 			/* xrm_class          	*/
	FALSE, 				/* compress_motion    	*/
	FALSE, 				/* compress_exposure  	*/
	FALSE, 				/* compress_enterleave	*/
	FALSE, 				/* visible_interest   	*/
	DisplayDestroy,			/* destroy            	*/
	(XtWidgetProc)NULL, 		/* resize             	*/
	(XtExposeProc)NULL, 		/* expose             	*/
	(XtSetValuesFunc)NULL, 		/* set_values         	*/
	(XtArgsFunc)NULL, 		/* set_values_hook      */
	(XtAlmostProc)NULL,	 	/* set_values_almost    */
	(XtArgsProc)NULL,		/* get_values_hook      */
	(XtAcceptFocusProc)NULL, 	/* accept_focus       	*/
	XtVersion, 			/* intrinsics version 	*/
	NULL, 				/* callback offsets   	*/
	NULL,				/* tm_table           	*/
	(XtGeometryHandler)NULL, 	/* query_geometry       */
	(XtStringProc)NULL, 		/* display_accelerator  */
	(XtPointer)&baseClassExtRec, 	/* extension            */
    },
    { 					/* composite class record */
	(XtGeometryHandler)NULL,	/* geometry_manager 	*/
	(XtWidgetProc)NULL,		/* change_managed	*/
	DisplayInsertChild,		/* insert_child		*/
	DisplayDeleteChild, 		/* from the shell 	*/
	NULL, 				/* extension record     */
    },
    { 					/* shell class record 	*/
	NULL, 				/* extension record     */
    },
    { 					/* wm shell class record */
	NULL, 				/* extension record     */
    },
    { 					/* vendor shell class record */
	NULL,				/* extension record     */
    },
    { 					/* toplevelclass record */
	NULL, 				/* extension record     */
    },
    { 					/* appShell record 	*/
	NULL, 				/* extension record     */
    },
    {					/* Display class	*/
	GetDisplay,			/* GetDisplay		*/
	NULL,				/* extension		*/
    },
};

externaldef(xmdisplayclass) WidgetClass
      xmDisplayClass = (WidgetClass) (&xmDisplayClassRec);



static void
DisplayClassPartInitialize(
	WidgetClass wc )
{
	_XmFastSubclassInit(wc, XmDISPLAY_BIT);
}

static void
DisplayClassInitialize( void )
{
	baseClassExtRec.record_type = XmQmotif;
    _Xm_MOTIF_DRAG_AND_DROP_MESSAGE =
		XmMakeCanonicalString("_MOTIF_DRAG_AND_DROP_MESSAGE");
}

static void
SetDragReceiverInfo(
        Widget w,
        XtPointer client_data,
        XEvent *event,
        Boolean *dontSwallow )
{
    XmDisplay	dd = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

    if (XtIsRealized(w)) {
	_XmSetDragReceiverInfo(dd, (Widget)client_data);
	XtRemoveEventHandler(w, StructureNotifyMask, False,
			     SetDragReceiverInfo,
			     client_data);
    }
}

/*
 * this routine is registered on the XmNtreeUpdateProc resource of the
 * dropSiteManager.  It is called whenever the tree is changed.
 */
static void
TreeUpdateHandler(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmAnyCallbackStruct	    	*anyCB = (XmAnyCallbackStruct *)call;
    XmDisplay	  	dd = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

    if (dd->display.dragReceiverProtocolStyle == XmDRAG_NONE)
		return;

    switch(anyCB->reason) {
      case XmCR_DROP_SITE_TREE_ADD:
	{
	    XmDropSiteTreeAddCallback cb =
	      (XmDropSiteTreeAddCallback)anyCB;

	    if (XtIsRealized(cb->rootShell)) {
		_XmSetDragReceiverInfo(dd, cb->rootShell);
	    }
	    else {
		XtAddEventHandler(cb->rootShell,
				  StructureNotifyMask, False,
				  SetDragReceiverInfo,
				  (XtPointer)cb->rootShell);
	    }
	    /*
	     * ClientMessages are not maskable so all we have to
	     * do is indicate interest in non-maskable events.
	     */
	    XtAddEventHandler(cb->rootShell, NoEventMask, True,
			      ReceiverShellExternalSourceHandler,
			      (XtPointer)dd);
	}
	break;
      case XmCR_DROP_SITE_TREE_REMOVE:
	{
	    XmDropSiteTreeRemoveCallback cb =
	      (XmDropSiteTreeRemoveCallback)anyCB;
	    XtRemoveEventHandler(cb->rootShell, NoEventMask, True,
				 ReceiverShellExternalSourceHandler,
				 (XtPointer)dd);
	    if (XtIsRealized(cb->rootShell))
	      _XmClearDragReceiverInfo(cb->rootShell);
	}
	break;
      default:
	break;
    }
}

/************************************************************************
 *
 *  DisplayInitialize
 *
 ************************************************************************/
static void
DisplayInitialize(
        Widget requested_widget,
        Widget new_widget,
        ArgList args,
        Cardinal *num_args )
{
    static char *atom_names[] = {
      "Customize Data:0",	/* XmSCUSTOMIZE_DATA + "0" */
      XmIATOM_PAIR, XmIAVERAGE_WIDTH, XmIPIXEL_SIZE, XmIRESOLUTION_Y,
      XmIWM_DELETE_WINDOW, XmI_MOTIF_DRAG_ATOMS,
      XmI_MOTIF_DRAG_INITIATOR_INFO, XmI_MOTIF_DRAG_PROXY_WINDOW,
      XmI_MOTIF_DRAG_RECEIVER_INFO, XmI_MOTIF_DRAG_TARGETS,
      XmI_MOTIF_DRAG_WINDOW,
      XmSCLIPBOARD, XmSCLIP_TEMPORARY, XmSCOMPOUND_TEXT, XmSDELETE,
      XmSINCR, XmSINSERT_SELECTION, XmSLENGTH, XmSLINK_SELECTION,
      XmSMULTIPLE, XmSNULL, XmSTARGETS, XmSTEXT, XmSTIMESTAMP,
      XmSTRANSFER_FAILURE, XmSTRANSFER_SUCCESS, XmSWM_STATE,
      XmS_MOTIF_ATOM_0, XmS_MOTIF_CLIP_DATA_DELETE,
      XmS_MOTIF_CLIP_DATA_REQUEST, XmS_MOTIF_CLIP_HEADER,
      XmS_MOTIF_CLIP_LOCK_ACCESS_VALID, XmS_MOTIF_CLIP_MESSAGE,
      XmS_MOTIF_CLIP_NEXT_ID, XmS_MOTIF_CLIP_TIME,
      XmS_MOTIF_CLIPBOARD_TARGETS, XmS_MOTIF_DESTINATION,
      XmS_MOTIF_DRAG_OFFSET, XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS,
      XmS_MOTIF_DROP, XmS_MOTIF_EXPORT_TARGETS, XmS_MOTIF_LOSE_SELECTION,
      _XA_MOTIF_BINDINGS, _XA_MOTIF_WM_HINTS, _XA_MOTIF_WM_INFO,
      _XA_MOTIF_WM_MENU, _XA_MOTIF_WM_MESSAGES, _XA_MOTIF_WM_OFFSET,
      "WM_PROTOCOLS"
    };

    Atom	atoms[XtNumber(atom_names)];
    XmDisplay	xmDisplay = (XmDisplay)new_widget;
    int		dummy1, dummy2;
    XContext 	context;

    /*
     * Pre-load the Xlib atom cache with atoms we are likely to use,
     * similar to the way _XmInitAtomPairs() did in  Motif 1.2.x.
     */
    XInternAtoms(XtDisplay(xmDisplay), atom_names, XtNumber(atom_names),
		 False, atoms);

    xmDisplay->display.shellCount = 0;

    xmDisplay->display.numModals = 0;
    xmDisplay->display.modals = NULL;
    xmDisplay->display.maxModals = 0;
    xmDisplay->display.userGrabbed = False;
    xmDisplay->display.activeDC = NULL;
    xmDisplay->display.dsm = (XmDropSiteManagerObject) NULL;

    xmDisplay->display.proxyWindow =
      _XmGetDragProxyWindow(XtDisplay(xmDisplay));

    _XmInitByteOrderChar();
    xmDisplay->display.xmim_info = NULL;

    xmDisplay->display.displayInfo = (XtPointer) XtNew(XmDisplayInfo);
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->SashCursor = 0L;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->TearOffCursor = 0L;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->UniqueStamp = 0L;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->destinationWidget=
	(Widget)NULL;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->excParentPane.pane =
	(Widget *)NULL;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->excParentPane.pane_list_size = 0;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->excParentPane.num_panes= 0;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->resetFocusFlag = 0;
    ((XmDisplayInfo *)(xmDisplay->display.displayInfo))->traversal_in_progress=
	FALSE;

    xmDisplay->display.displayHasShapeExtension =
      XShapeQueryExtension(XtDisplay(xmDisplay), &dummy1, &dummy2);

    /* Handle dynamic default of receiver protocol style */
    if (xmDisplay->display.dragReceiverProtocolStyle ==
	INVALID_PROTOCOL_VALUE) {
      if (xmDisplay->display.displayHasShapeExtension)
	xmDisplay->display.dragReceiverProtocolStyle = XmDRAG_PREFER_DYNAMIC;
      else
	xmDisplay->display.dragReceiverProtocolStyle =
	  XmDRAG_PREFER_PREREGISTER;
    }

    _XmVirtKeysInitialize (new_widget);

    _XmProcessLock();
    if (displayContext == 0)
      displayContext = XUniqueContext();
    context = displayContext;
    _XmProcessUnlock();

	if (! XFindContext(XtDisplay(xmDisplay), None, context,
		(char **) &xmDisplay))
	{
		/*
		 * There's one already created for this display.
		 * What should we do?  If we destroy the previous one, we may
		 * wreak havoc with shell modality and screen objects.  BUT,
		 * Xt doesn't really give us a way to abort a create.  We'll
		 * just let the new one dangle.
		 */

		XmeWarning((Widget) xmDisplay, MESSAGE1);
	}
	else
	{
		XSaveContext(XtDisplayOfObject((Widget)xmDisplay),
			 None,
			 context,
			 (char *)xmDisplay);
	}

    if (xmDisplay->display.enable_multi_key_bindings) {
	Display * display = XtDisplay(new_widget);
	int i, num_screens = ScreenCount(display);
	XrmDatabase new_db;

	for (i = 0; i < num_screens; i++)  {
	    Screen * screen = ScreenOfDisplay(display, i);
	    XrmDatabase db = XtScreenDatabase(screen);
	    new_db = XrmGetStringDatabase(_XmDisplay_baseTranslations);
	    XrmCombineDatabase(new_db, &db, False);
	}
    }
}


/************************************************************************
 *
 *  DisplayInsertChild
 *
 ************************************************************************/
static void
DisplayInsertChild(
        Widget w )
{
	if (XtIsRectObj(w)) {
		XtWidgetProc insert_child;

		_XmProcessLock();
		insert_child = ((CompositeWidgetClass)compositeWidgetClass)
				->composite_class.insert_child;
		_XmProcessUnlock();
		(*insert_child)(w);
	}
}


/************************************************************************
 *
 *  DisplayDeleteChild
 *
 ************************************************************************/
static void
DisplayDeleteChild(
        Widget w )
{
	if (XtIsRectObj(w)) {
		XtWidgetProc delete_child;

		_XmProcessLock();
		delete_child = ((CompositeWidgetClass)compositeWidgetClass)
				->composite_class.delete_child;
		_XmProcessUnlock();
		(*delete_child)(w);
	}
}

/************************************************************************
 *
 *  DisplayDestroy
 *
 ************************************************************************/
static void
DisplayDestroy(
        Widget w )
{
    XmDisplay dd = (XmDisplay) w ;
    XContext context;

    _XmProcessLock();
    context = displayContext;
    _XmProcessUnlock();


    XtFree((char *) dd->display.modals);

    if (((XmDisplayInfo *)(dd->display.displayInfo))
		->excParentPane.pane != (Widget *)NULL)
	XtFree((char *) ((XmDisplayInfo *)(dd->display.displayInfo))
		->excParentPane.pane);
    XtFree((char *) dd->display.displayInfo);

    /* Destroy the DropSiteManager object. */
    if (dd->display.dsm != NULL)
	XtDestroyWidget((Widget) dd->display.dsm);

    _XmClearDisplayTables(XtDisplay(w));

    _XmVirtKeysDestroy (w);

    XDeleteContext( XtDisplay( w), None, context) ;
}

XmDropSiteManagerObject
_XmGetDropSiteManagerObject(
        XmDisplay xmDisplay )
{
  /* Defer the creation of the XmDisplayObject's DropSiteManager until
   *   it is referenced, since the converters of some DSM resources
   *   (animationPixmap, for instance) depend on the presence of the
   *   Display/Screen objects, and a circular recursive creation loop
   *   results if the DSM is created during DisplayInitialize.
   */
  if(    xmDisplay->display.dsm == NULL    )
    {
      Arg lclArgs[1] ;

      XtSetArg( lclArgs[0], XmNtreeUpdateProc, TreeUpdateHandler) ;
      xmDisplay->display.dsm = (XmDropSiteManagerObject) XtCreateWidget( "dsm",
                                       xmDisplay->display.dropSiteManagerClass,
                                              (Widget) xmDisplay, lclArgs, 1) ;
    }
  return( xmDisplay->display.dsm);
}


unsigned char
_XmGetDragProtocolStyle(
        Widget w )
{
    XmDisplay		xmDisplay;
    unsigned char	style;

    xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

    switch(xmDisplay->display.dragReceiverProtocolStyle) {
	  case XmDRAG_NONE:
	  case XmDRAG_DROP_ONLY:
	    style = XmDRAG_NONE;
	    break;
	  case XmDRAG_DYNAMIC:
	    style = XmDRAG_DYNAMIC;
	    break;
	  case XmDRAG_PREFER_DYNAMIC:
	  case XmDRAG_PREFER_PREREGISTER:
	  case XmDRAG_PREREGISTER:
	    style = XmDRAG_PREREGISTER;
	    break;
	  case XmDRAG_XDND:
	    style = XmDRAG_XDND;
	    break;
	  default:
	    style = XmDRAG_NONE;
	    break;
	}
    return style;
}

Widget
XmGetDragContext(
        Widget w,
        Time time )
{
	XmDisplay		xmDisplay;
	XmDragContext	matchedDC = NULL, dc = NULL;
	Cardinal		i;

	_XmWidgetToAppContext(w);
	_XmAppLock(app);

	xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(w));
	for(i = 0; i < xmDisplay->composite.num_children; i++)
	{
		dc = (XmDragContext)(xmDisplay->composite.children[i]);
		if ((XmIsDragContext((Widget) dc)) && (CHECK_TIME(dc, time)) &&
			((!matchedDC) ||
				(matchedDC->drag.dragStartTime
					< dc->drag.dragStartTime)) &&
			!dc->core.being_destroyed)
			matchedDC = dc;
	}
	_XmAppUnlock(app);
	return((Widget)matchedDC);
}

Widget
_XmGetDragContextFromHandle(
        Widget w,
        Atom iccHandle )
{
	XmDisplay		xmDisplay;
	XmDragContext	dc;
	Cardinal		i;

	xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(w));

	for(i = 0; i < xmDisplay->composite.num_children; i++)
	{
		dc = (XmDragContext)(xmDisplay->composite.children[i]);
		if ((XmIsDragContext((Widget) dc)) &&
			(dc->drag.iccHandle == iccHandle) &&
			!dc->core.being_destroyed)
			return((Widget)dc);
	}
	return(NULL);
}




static XmDragContext
FindDC(
        XmDisplay xmDisplay,
        Time time,
        Boolean sourceIsExternal )
{
	XmDragContext	dc = NULL;
	Cardinal			i;

	for(i = 0; i < xmDisplay->composite.num_children; i++)
	{
		dc = (XmDragContext)(xmDisplay->composite.children[i]);
		if ((XmIsDragContext((Widget) dc)) &&
			(CHECK_TIME(dc, time)) &&
			(dc->drag.sourceIsExternal == sourceIsExternal) &&
			!dc->core.being_destroyed)
			return(dc);
	}
	return(NULL);
}

static int
isMine(
        Display *dpy,
        register XEvent *event,
        char *arg )
{
	XmDisplayEventQueryStruct 	*q = (XmDisplayEventQueryStruct *) arg;
	XmICCCallbackStruct		callback, *cb = &callback;

	/* Once we have a drop start we must stop looking at the queue */
	if (q->hasDropStart)
		return(False);

	if (!_XmICCEventToICCCallback((XClientMessageEvent *)event,
			cb, XmICC_INITIATOR_EVENT))
		return(False);

	if ((cb->any.reason == XmCR_DROP_SITE_ENTER) ||
		(cb->any.reason == XmCR_DROP_SITE_LEAVE))
	{
		/*
		 * We must assume this to be a dangling set of messages, so
		 * we will quietly consume it in the interest of hygene.
		 */
		return(True);
	}


	if (!q->dc)
		q->dc = FindDC(q->dd, cb->any.timeStamp, True);
	/*
	 * if we can find a dc then we have already processed
	 * an enter for this shell.
	 */

	switch(cb->any.reason)
	{
		case XmCR_TOP_LEVEL_ENTER:
			q->hasLeave = False;
			if (q->dc == NULL)
			{
				*(q->enterCB) = *(XmTopLevelEnterCallbackStruct *)cb;
				q->hasEnter = True;
			}
		break;
		case XmCR_TOP_LEVEL_LEAVE:
			if (q->dc != NULL)
			{
				*(q->leaveCB) = *(XmTopLevelLeaveCallbackStruct *)cb;
				q->hasLeave = True;
				q->hasMotion = False;
			}
			else
			{
			  /*
			   * hasEnter is true if we are missing a
			   * drag context because an enter got compressed.
			   * In that case, all is OK, and no warning is
			   * needed. ??? Is a warning ever needed ???
			   */
			  if (!q->hasEnter)
			    XmeWarning((Widget) q->dd, MESSAGE2);
			}
			q->hasEnter = False;
		break;
		case XmCR_DRAG_MOTION:
			*(q->motionCB) = *(XmDragMotionCallbackStruct *)cb;
			q->hasMotion = True;
		break;
		case XmCR_DROP_START:
			*(q->dropStartCB) = *(XmDropStartCallbackStruct *)cb;
			q->hasDropStart = True;
		break;
		default:
		break;
	}
	return True;
}

/*
 * this handler is used to catch messages from external drag
 * contexts that want to map motion or drop events
 */
static void
ReceiverShellExternalSourceHandler(
        Widget w,
        XtPointer client_data,
        XEvent *event,
        Boolean *dontSwallow )
{
    XmDragTopLevelClientDataStruct 	topClientData;
    XmTopLevelEnterCallbackStruct	enterCB;
    XmTopLevelLeaveCallbackStruct	leaveCB;
    XmDropStartCallbackStruct		dropStartCB;
    XmDragMotionCallbackStruct		motionCB;
    XmDisplayEventQueryStruct		 		q;
    Widget	  			shell = w;
    XmDisplay			dd = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    XmDragContext			dc;
    XmDropSiteManagerObject		dsm = _XmGetDropSiteManagerObject( dd);
    XmDropStartCallback     dsCallback = &dropStartCB;
    XmTopLevelEnterCallback teCallback = &enterCB;

    /*
     * If dd has an active dc then we are the initiator.  We shouldn't
     * do receiver processing as the initiator, so bail out.
     */
    if (dd->display.activeDC != NULL)
	return;

    q.dd = dd;
    q.dc = (XmDragContext)NULL;
    q.enterCB = &enterCB;
    q.leaveCB = &leaveCB;
    q.motionCB = &motionCB;
    q.dropStartCB = &dropStartCB;
    q.hasEnter =
      q.hasDropStart =
	q.hasLeave =
	  q.hasMotion = False;
    /*
     * Since this event handler gets called for all non-maskable events,
     * we want to bail out now with don't swallow if we don't want this
     * event.  Otherwise we'll swallow it.
     */

     /*
	  * process the event that fired this event handler.
	  * If it's not a Receiver DND event, bail out.
	  */
    if (!isMine(XtDisplayOfObject(w), event, (char*)&q))
		return;

    /*
     * swallow all the pending messages inline except the last motion
     */
    while (XCheckIfEvent( XtDisplayOfObject(w), event, isMine, (char*)&q))
      /*EMPTY*/;

    dc = q.dc;

    if (q.hasEnter || q.hasMotion || q.hasDropStart || q.hasLeave) {
	/*
	 * handle a dangling leave first
	 */
	if (q.hasLeave)
	  {
	      XmTopLevelLeaveCallback	callback = &leaveCB;

	      topClientData.destShell = shell;
	      topClientData.xOrigin = shell->core.x;
	      topClientData.yOrigin = shell->core.y;
	      topClientData.sourceIsExternal = True;
	      topClientData.iccInfo = NULL;
	      topClientData.window = XtWindow(w);
	      topClientData.dragOver = NULL;

	      _XmDSMUpdate(dsm,
			   (XtPointer)&topClientData,
			   (XtPointer)callback);
	      /* destroy it if no drop. otherwise done in dropTransfer */
	      if (!q.hasDropStart)
	      {
		XtDestroyWidget((Widget)dc);
		q.dc = dc = NULL;
	      }
	  }
	/*
	 * check for a dropStart from a preregister client or an enter
	 * either of which require a dc to be alloced
	 */
	if (q.hasEnter || q.hasDropStart) {
	    if (!q.dc) {
		Arg		args[6];
		Cardinal	i = 0;
		Time		timeStamp;
		Window		window;
		Atom		iccHandle;

		if (q.hasDropStart) {
		    timeStamp = dsCallback->timeStamp;
		    window = dsCallback->window;
		    iccHandle = dsCallback->iccHandle;
		} else {
		    timeStamp = teCallback->timeStamp;
		    window = teCallback->window;
		    iccHandle = teCallback->iccHandle;
		    XtSetArg(args[i], XmNexportTargets, teCallback->targets); i++;
		    XtSetArg(args[i], XmNnumExportTargets, teCallback->n_targets); i++;
		}
		XtSetArg(args[i], XmNsourceWindow, window);i++;
		XtSetArg(args[i], XmNsourceIsExternal,True);i++;
		XtSetArg(args[i], XmNstartTime, timeStamp);i++;
		XtSetArg(args[i], XmNiccHandle, iccHandle);i++;
		dc = (XmDragContext) XtCreateWidget("dragContext",
			dd->display.dragContextClass, (Widget)dd, args, i);

		/*
		 * force in value for dropTransfer to use in selection
		 * calls.
		 */
		dc->drag.currReceiverInfo =
		  _XmAllocReceiverInfo(dc);
		dc->drag.currReceiverInfo->shell = shell;
                dc->drag.currReceiverInfo->dragProtocolStyle =
			                  dd->display.dragReceiverProtocolStyle;
		if (!q.hasDropStart && teCallback->dragProtocolStyle == XmDRAG_XDND)
			dc->drag.currReceiverInfo->dragProtocolStyle = XmDRAG_XDND;
		_XmReadInitiatorInfo((Widget)dc);
	    }

	    topClientData.destShell = shell;
	    topClientData.xOrigin = XtX(shell);
	    topClientData.yOrigin = XtY(shell);
	    topClientData.width = XtWidth(shell);
	    topClientData.height = XtHeight(shell);
	    topClientData.sourceIsExternal = True;
	    topClientData.iccInfo = NULL;
	}

	if (!dc) return;

	if (q.hasDropStart) {
	    dc->drag.dragFinishTime = dropStartCB.timeStamp;
	    if (dropStartCB.x == -1 || dropStartCB.y == -1) {
	        dropStartCB.x = dc->drag.currReceiverInfo->xLast;
	        dropStartCB.y = dc->drag.currReceiverInfo->yLast;
	    }

	    _XmDSMUpdate(dsm,
			 (XtPointer)&topClientData,
			 (XtPointer)&dropStartCB);
	}
	/*
	 * we only see enters if they're not matched with a leave
	 */
	if (q.hasEnter) {
	    _XmDSMUpdate(dsm,
			 (XtPointer)&topClientData,
			 (XtPointer)&enterCB);
	}
	if (q.hasMotion) {
	    XmDragMotionCallback	callback = &motionCB;
	    XmDragMotionClientDataStruct	motionData;

	    /**
	     * Xdnd doesn't give us position info for certain events, so we'll track
	     * the last position we got in the drag context for later.
	     */
	    dc->drag.currReceiverInfo->xLast = callback->x;
	    dc->drag.currReceiverInfo->yLast = callback->y;
	    motionData.window = XtWindow(w);
	    motionData.dragOver = NULL;
	    _XmDSMUpdate(dsm, (XtPointer)&motionData, (XtPointer)callback);
	}
    }
}

static Widget
GetDisplay(
        Display *display )
{
	XmDisplay	xmDisplay = NULL;
	XContext        context;
	Arg args[3];
	int n;

	_XmProcessLock();
	context = displayContext;
	_XmProcessUnlock();

	if ((context == 0) ||
		(XFindContext(display, None, context,
			(char **) &xmDisplay)))
	{
		String	name, w_class;

		XtGetApplicationNameAndClass(display, &name, &w_class);

		n = 0;
		XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
		XtSetArg(args[n], XmNwidth, 1); n++;
		XtSetArg(args[n], XmNheight, 1); n++;
		xmDisplay = (XmDisplay) XtAppCreateShell(name, w_class,
			xmDisplayClass, display, args, n);
	}

	/* We need a window to be useful */
	if (!XtIsRealized((Widget)xmDisplay))
	  {
	    XtRealizeWidget((Widget)xmDisplay);

	    /*
	     * Got the resources - get rid of name-type properties.
	     * Otherwise, clients (ie: xwd) may get the wrong window
	     * using the -name option.
	     */
	    XDeleteProperty(display, XtWindow(xmDisplay), XA_WM_NAME);
	    XDeleteProperty(display, XtWindow(xmDisplay), XA_WM_ICON_NAME);
	    XDeleteProperty(display, XtWindow(xmDisplay), XA_WM_CLASS);
	  }

	return ((Widget)xmDisplay);
}

Widget
XmGetXmDisplay(
        Display *display )
{
	XmDisplayClass dC;
	Widget w;
	_XmDisplayToAppContext(display);

	/*
	 * We have a chicken and egg problem here; we'd like to get
	 * the display via a class function, but we don't know which
	 * class to use.  Hence the magic functions _XmGetXmDisplayClass
	 * and _XmSetXmDisplayClass.
	 */

	_XmAppLock(app);
	_XmProcessLock();
	dC = (XmDisplayClass) _XmGetXmDisplayClass();
	w = (*(dC->display_class.GetDisplay))(display);
	_XmProcessUnlock();
	_XmAppUnlock(app);

	return w;
}

/*
 * It would be nice if the next two functions were methods, but
 * for obvious reasons they're not.
 */

WidgetClass
_XmGetXmDisplayClass( void )
{
	WidgetClass class;

	_XmProcessLock();
	if (curDisplayClass == NULL)
		curDisplayClass = xmDisplayClass;
	class = curDisplayClass;
	_XmProcessUnlock();
	return class;
}

WidgetClass
_XmSetXmDisplayClass(
	WidgetClass wc )
{
	WidgetClass oldDisplayClass;
	WidgetClass sc = wc;

	_XmProcessLock();
	oldDisplayClass = curDisplayClass;
	/*
	 * We aren't going to let folks just set any old class in as the
	 * display class.  They will have to use subclasses of xmDisplay.
	 */
	while ((sc != NULL) && (sc != xmDisplayClass))
		sc = sc->core_class.superclass;

	if (sc != NULL)
		curDisplayClass = wc;
	else
		XmeWarning(NULL, MESSAGE3);

	_XmProcessUnlock();
	return(oldDisplayClass);
}

/**********************************************************************
 *
 * _XmSetThickness
 * This procedure is called as the resource default XtRCallProc
 * to default the shadow and highlight thickness resources.  If
 * enablethinThickness is true (CDE apps), the value is "1 pixel",
 * else defaults to "2".
 **********************************************************************/
void
_XmSetThickness(
        Widget widget,
        int offset,
        XrmValue *value )
{
	XmDisplay	 xmDisplay;
	static Dimension thickness;

	xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(widget));

	if (xmDisplay->display.enable_thin_thickness) {
		thickness = 1;
	}
	else {
		thickness = 2;
	}

	value->addr = (XPointer)&thickness;

}

/**********************************************************************
 *
 * _XmSetThicknessDefault0
 * This procedure is called as the resource default XtRCallProc
 * to default the shadow and highlight thickness resources.  If
 * enablethinThickness is true (CDE apps), the value is "1 pixel",
 * else defaults to "0".
 **********************************************************************/
void
_XmSetThicknessDefault0(
        Widget widget,
        int offset,
        XrmValue *value )
{
	XmDisplay	 xmDisplay;
	static Dimension thickness;

	xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(widget));

	if (xmDisplay->display.enable_thin_thickness) {
		thickness = 1;
	}
	else {
		thickness = 0;
	}

	value->addr = (XPointer)&thickness;

}
