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
static char rcsid[] = "$TOG: DragC.c /main/29 1997/10/07 12:19:52 cshi $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef DEFAULT_WM_TIMEOUT
#define DEFAULT_WM_TIMEOUT 5000
#endif

#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <Xm/AtomMgr.h>
#include <Xm/DisplayP.h>
#include <Xm/DragOverSP.h>
#include <Xm/GadgetP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VendorSP.h>
#include "XmI.h"
#include "DisplayI.h"
#include "DragBSI.h"
#include "DragCI.h"
#include "DragICCI.h"
#include "DragOverSI.h"
#include "MessagesI.h"
#include "TraversalI.h"
#include "VendorSI.h"

#include <stdio.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif 

/* force the multi screen support on */
#define MULTI_SCREEN_DONE

#ifdef DEBUG
#define Warning(str)	XmeWarning(NULL, str)
#else
#define Warning(str)	/*EMPTY*/
#endif

#define BIGSIZE ((Dimension)32767)

#define MESSAGE1 _XmMMsgDragC_0001
#define MESSAGE2 _XmMMsgDragC_0002
#define MESSAGE3 _XmMMsgDragC_0003
#define MESSAGE4 _XmMMsgDragC_0004
#define MESSAGE5 _XmMMsgDragC_0005
#define MESSAGE6 _XmMMsgDragC_0006

typedef struct _MotionEntryRec{
    int		type;
    Time	time;
    Window	window;
    Window	subwindow;
    Position	x, y;
    unsigned int state;
} MotionEntryRec, *MotionEntry;

#define STACKMOTIONBUFFERSIZE 	120

typedef struct _MotionBufferRec{
    XmDragReceiverInfo currReceiverInfo;
    Cardinal		count;
    MotionEntryRec      entries[STACKMOTIONBUFFERSIZE];
} MotionBufferRec, *MotionBuffer;

#define MOTIONFILTER		16

/********    Static Function Declarations    ********/

static void GetRefForeground( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
static void CopyRefForeground( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
static void GetRefBackground( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
static void DragContextInitialize( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *numArgs) ;
static Boolean DragContextSetValues( 
                        Widget old,
                        Widget ref,
                        Widget new_w,
                        Arg *args,
                        Cardinal *numArgs) ;
static void DragContextDestroy( 
                        Widget w) ;
static void DragContextClassInitialize( void ) ;
static void DragContextClassPartInitialize( WidgetClass ) ;
static Window GetClientWindow( 
                        Display *dpy,
                        Window win,
                        Atom atom) ;
static void ValidateDragOver( 
                        XmDragContext dc,
                        unsigned char oldStyle,
                        unsigned char newStyle) ;
static XmDragReceiverInfo FindReceiverInfo( 
                        XmDragContext dc,
                        Window win) ;
static void GetDestinationInfo( 
                        XmDragContext dc,
                        Window root,
                        Window win) ;
static void GetScreenInfo( 
                        XmDragContext dc) ;
static void SendDragMessage( 
                        XmDragContext dc,
                        Window destination,
                        unsigned char messageType) ;
static void GenerateClientCallback( 
                        XmDragContext dc,
                        unsigned char reason) ;
static void DropLoseIncrSelection( 
                        Widget w,
                        Atom *selection,
                        XtPointer clientData) ;
static void DropLoseSelection( 
                        Widget w,
                        Atom *selection) ;
static void DragDropFinish( 
                        XmDragContext dc) ;
static Boolean DropConvertIncrCallback( 
                        Widget w,
                        Atom *selection,
                        Atom *target,
                        Atom *typeRtn,
                        XtPointer *valueRtn,
                        unsigned long *lengthRtn,
                        int *formatRtn,
                        unsigned long *maxLengthRtn,
                        XtPointer clientData,
                        XtRequestId *requestID) ;
static Boolean DropConvertCallback( 
                        Widget w,
                        Atom *selection,
                        Atom *target,
                        Atom *typeRtn,
                        XtPointer *valueRtn,
                        unsigned long *lengthRtn,
                        int *formatRtn) ;
static void DragStartProto( 
                        XmDragContext dc) ;
static void NewScreen( 
                        XmDragContext dc,
                        Window newRoot) ;
static void LocalNotifyHandler( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void ExternalNotifyHandler( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void InitiatorMsgHandler( 
                        Widget w,
                        XtPointer clientData,
                        XEvent *event,
                        Boolean *dontSwallow) ;
static void SiteEnteredWithLocalSource( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void SiteLeftWithLocalSource( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void OperationChanged( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void SiteMotionWithLocalSource( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void DropStartConfirmed( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static Widget GetShell( 
                        Widget w) ;
static void InitDropSiteManager( 
                        XmDragContext dc) ;
static void TopWindowsReceived( 
                        Widget w,
                        XtPointer client_data,
                        Atom *selection,
                        Atom *type,
                        XtPointer value,
                        unsigned long *length,
                        int *format) ;
static void DragStart( 
                        XmDragContext dc,
                        Widget src,
                        XEvent *event) ;
static void DragStartWithTracking( 
                        XmDragContext dc) ;
static void UpdateMotionBuffer( 
			XmDragContext dc,
                        MotionBuffer mb,
                        XEvent *event) ;
static void DragMotionProto( 
                        XmDragContext dc,
                        Window root,
                        Window subWindow) ;
static void ProcessMotionBuffer( 
                        XmDragContext dc,
                        MotionBuffer mb) ;
static void DragMotion( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void DragKey( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void DropStartTimeout( 
                        XtPointer clientData,
                        XtIntervalId *id) ;
static void DropFinishTimeout( 
                        XtPointer clientData,
                        XtIntervalId *id) ;
static void FinishAction( 
                        XmDragContext dc,
                        XEvent *ev) ;
static void CheckModifiers( 
                        XmDragContext dc,
                        unsigned int state) ;
static void IgnoreButtons( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void CancelDrag( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void HelpDrag( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void FinishDrag( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
static void InitiatorMainLoop( 
                        XtPointer clientData,
                        XtIntervalId *id) ;
static void DragCancel( 
                        XmDragContext dc) ;
static void CalculateDragOperation(
			XmDragContext dc) ;
static void cancelDrag( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;
static void noMoreShell ( 
                        Widget w,
                        XtPointer client,
                        XtPointer call) ;

/********    End Static Function Declarations    ********/


static XtActionsRec	dragContextActions[] = 
{
    {"FinishDrag"	, FinishDrag	},
    {"CancelDrag"	, CancelDrag	},
    {"HelpDrag"		, HelpDrag	},
    {"DragMotion"	, DragMotion	},
    {"DragKey"		, DragKey	},
    {"IgnoreButtons"	, IgnoreButtons	},
};

static XmConst unsigned char	protocolMatrix[7][6] = {

    /*
     *
     * Rows are initiator styles, Columns are receiver styles.
     *
     * Receiver     NO   DO   PP   P    PD   D
     * Initiator  -------------------------------
     *      NO    | NO | NO | NO | NO | NO | NO |
     *      DO    | NO | DO | DO | DO | DO | DO |
     *      PP    | NO | DO | P  | P  | P  | D  |
     *      P     | NO | DO | P  | P  | P  | DO |
     *      PD    | NO | DO | D  | P  | D  | D  |
     *      D     | NO | DO | D  | DO | D  | D  |
     *      PR    | NO | DO | P  | P  | D  | D  |
     */


    { /* Initiator == XmDRAG_NONE ==   0 */
	XmDRAG_NONE,        XmDRAG_NONE,        XmDRAG_NONE,
	XmDRAG_NONE,        XmDRAG_NONE,        XmDRAG_NONE,
    },
    { /* Initiator == DROP_ONLY ==      1 */
	XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_DROP_ONLY,
	XmDRAG_DROP_ONLY,   XmDRAG_DROP_ONLY,   XmDRAG_DROP_ONLY,
    },
    { /* Initiator == PREFER_PREREG ==  2 */
	XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_PREREGISTER,
	XmDRAG_PREREGISTER, XmDRAG_PREREGISTER, XmDRAG_DYNAMIC,
    },
    { /* Initiator == PREREG ==         3 */
	 XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_PREREGISTER,
	 XmDRAG_PREREGISTER, XmDRAG_PREREGISTER, XmDRAG_DROP_ONLY,
    },
    { /* Initiator == PREFER_DYNAMIC == 4 */
	 XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_DYNAMIC,
	 XmDRAG_PREREGISTER, XmDRAG_DYNAMIC,     XmDRAG_DYNAMIC,
    },
    { /* Initiator == XmDRAG_DYNAMIC == 5 */
	 XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_DYNAMIC,
	 XmDRAG_DROP_ONLY,   XmDRAG_DYNAMIC,     XmDRAG_DYNAMIC,
    },
    { /* Initiator == DRAG_RECEIVER ==  6 */
	 XmDRAG_NONE,        XmDRAG_DROP_ONLY,   XmDRAG_PREREGISTER,
	 XmDRAG_PREREGISTER, XmDRAG_DYNAMIC,     XmDRAG_DYNAMIC,
    },
};

/***************************************************************************
 *
 * Default values for resource lists
 *
 ***************************************************************************/

#define Offset(x)	(XtOffsetOf(XmDragContextRec, x))

static XtResource dragContextResources[] = 
{
    {
	XmNsourceWidget, XmCSourceWidget, XmRWidget,
	sizeof(Widget), Offset(drag.sourceWidget),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNexportTargets, XmCExportTargets, XmRAtomList,
	sizeof(Atom *), Offset(drag.exportTargets),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNnumExportTargets, XmCNumExportTargets, XmRInt,
	sizeof(Cardinal), Offset(drag.numExportTargets),
	XmRImmediate, (XtPointer)0,
    },
    {
	XmNconvertProc, XmCConvertProc, XmRFunction,
	sizeof(XmConvertSelectionRec), Offset(drag.convertProc),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNclientData, XmCClientData, XmRPointer,
	sizeof(XtPointer), Offset(drag.clientData),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNincremental, XmCIncremental, XmRBoolean,
	sizeof(Boolean), Offset(drag.incremental),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdragOperations, XmCDragOperations, XmRUnsignedChar,
	sizeof(unsigned char), Offset(drag.dragOperations),
	XmRImmediate, (XtPointer)(XmDROP_COPY | XmDROP_MOVE),
    },
    {
	XmNsourceCursorIcon, XmCSourceCursorIcon,
	XmRWidget, sizeof(Widget),
	Offset(drag.sourceCursorIcon), XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNsourcePixmapIcon, XmCSourcePixmapIcon,
	XmRWidget, sizeof(Widget),
	Offset(drag.sourcePixmapIcon), XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNstateCursorIcon, XmCStateCursorIcon,
	XmRWidget, sizeof(Widget),
	Offset(drag.stateCursorIcon), XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNoperationCursorIcon, XmCOperationCursorIcon,
	XmRWidget, sizeof(Widget),
	Offset(drag.operationCursorIcon), XmRImmediate, (XtPointer)NULL,
    },
    {	
	XmNcursorBackground, XmCCursorBackground, XmRPixel, 
	sizeof(Pixel), Offset(drag.cursorBackground), XmRCallProc,
	(XtPointer)GetRefBackground,
    },
    {	
	XmNcursorForeground, XmCCursorForeground, XmRPixel,
	sizeof(Pixel), Offset(drag.cursorForeground), XmRCallProc, (XtPointer)GetRefForeground,	
    },
    {
	XmNvalidCursorForeground, XmCValidCursorForeground, XmRPixel,
	sizeof(Pixel), Offset(drag.validCursorForeground), XmRCallProc, (XtPointer)CopyRefForeground,	
    },
    {	
	XmNinvalidCursorForeground, XmCInvalidCursorForeground,
	XmRPixel, sizeof(Pixel), Offset(drag.invalidCursorForeground),
	XmRCallProc, (XtPointer)CopyRefForeground,	
    },
    {	
	XmNnoneCursorForeground, XmCNoneCursorForeground, XmRPixel,
	sizeof(Pixel), Offset(drag.noneCursorForeground), 
	XmRCallProc, (XtPointer)CopyRefForeground,	
    },
    {
	XmNdropSiteEnterCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.siteEnterCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdropSiteLeaveCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.siteLeaveCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNtopLevelEnterCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.topLevelEnterCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdragMotionCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.dragMotionCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNtopLevelLeaveCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.topLevelLeaveCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdropStartCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.dropStartCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdragDropFinishCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.dragDropFinishCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdropFinishCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.dropFinishCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNoperationChangedCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), Offset(drag.operationChangedCallback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNblendModel, XmCBlendModel,
	XmRBlendModel,
        sizeof(unsigned char), Offset(drag.blendModel),
        XmRImmediate, (XtPointer)XmBLEND_ALL,
    },
    {
	XmNsourceIsExternal, XmCSourceIsExternal, XmRBoolean,
        sizeof(Boolean), Offset(drag.sourceIsExternal),
        XmRImmediate, (XtPointer)False,
    },
    {
	XmNsourceWindow, XmCSourceWindow, XmRWindow,
        sizeof(Window), Offset(drag.srcWindow),
        XmRImmediate, (XtPointer)None,
    },
    {
	XmNstartTime, XmCStartTime, XmRInt,
        sizeof(Time), Offset(drag.dragStartTime),
        XmRImmediate, (XtPointer)0,
    },
    {
	XmNiccHandle, XmCICCHandle, XmRAtom,
        sizeof(Atom), Offset(drag.iccHandle),
        XmRImmediate, (XtPointer)None,
    },
};



externaldef(xmdragcontextclassrec)
XmDragContextClassRec xmDragContextClassRec = {
    {	
	(WidgetClass) &coreClassRec,	/* superclass	*/   
	"XmDragContext",		/* class_name 		*/   
	sizeof(XmDragContextRec),	/* size 		*/   
	DragContextClassInitialize,	/* Class Initializer 	*/   
	DragContextClassPartInitialize,	/* class_part_init 	*/ 
	FALSE, 				/* Class init'ed ? 	*/   
	DragContextInitialize,		/* initialize         	*/   
	NULL, 				/* initialize_notify    */ 
	XtInheritRealize,		/* realize            	*/   
	dragContextActions,		/* actions		*/
	XtNumber(dragContextActions),	/* num_actions        	*/   
	dragContextResources,		/* resources          	*/   
	XtNumber(dragContextResources),/* resource_count    	*/   
	NULLQUARK, 			/* xrm_class          	*/   
	FALSE, 				/* compress_motion    	*/   
	XtExposeNoCompress, 		/* compress_exposure  	*/   
	FALSE, 				/* compress_enterleave	*/   
	FALSE, 				/* visible_interest   	*/   
	DragContextDestroy,		/* destroy            	*/   
	NULL,		 		/* resize             	*/   
	NULL, 				/* expose             	*/   
	DragContextSetValues,		/* set_values         	*/   
	NULL, 				/* set_values_hook      */ 
	NULL,			 	/* set_values_almost    */ 
	NULL,				/* get_values_hook      */ 
	NULL, 				/* accept_focus       	*/   
	XtVersion, 			/* intrinsics version 	*/   
	NULL, 				/* callback offsets   	*/   
	_XmDragC_defaultTranslations,	/* tm_table           	*/   
	NULL, 				/* query_geometry       */ 
	NULL, 				/* display_accelerator  */ 
	NULL, 				/* extension            */ 
    },	
    {					/* dragContext	*/
	DragStart,			/* start		*/
	DragCancel,			/* cancel		*/
	NULL,				/* extension record	*/
    },
};

externaldef(dragContextclass) WidgetClass 
      xmDragContextClass = (WidgetClass) &xmDragContextClassRec;

/*ARGSUSED*/
static void 
GetRefForeground(
        Widget widget,
        int offset,
        XrmValue *value )
{
    static Pixel	pixel;

    XmDragContext	dc = (XmDragContext)widget;
    Widget		sw = dc->drag.sourceWidget;

    pixel = BlackPixelOfScreen(XtScreen(widget));

    value->addr = (XPointer)(&pixel);
    value->size = sizeof(Pixel);

    if (sw) {
	if (XmIsGadget(sw))
	  pixel = ((XmManagerWidget)(XtParent(sw)))->manager.foreground;
	else if (XmIsPrimitive(sw))
	  pixel = ((XmPrimitiveWidget)sw)->primitive.foreground;
	else if (XmIsManager(sw))
	  pixel = ((XmManagerWidget)sw)->manager.foreground;
    }
} /* GetRefForeground */


/*ARGSUSED*/
static void 
CopyRefForeground(
        Widget widget,
        int offset,
        XrmValue *value )
{
    XmDragContext	dc = (XmDragContext)widget;

    value->addr = (XPointer)(&dc->drag.cursorForeground);
    value->size = sizeof(Pixel);
} /* CopyRefForeground */


/*ARGSUSED*/
static void 
GetRefBackground(
        Widget widget,
        int offset,
        XrmValue *value )
{
    static Pixel	pixel;

    XmDragContext	dc = (XmDragContext)widget;
    Widget		sw = dc->drag.sourceWidget;

    pixel = WhitePixelOfScreen(XtScreen(dc));

    value->addr = (XPointer)(&pixel);
    value->size = sizeof(Pixel);

    if (sw) {
	if (XmIsGadget(sw))
	  pixel = ((XmManagerWidget)(XtParent(sw)))->core.background_pixel;
	else 
	  pixel = sw->core.background_pixel;
    }
} /* GetRefBackground */


/*ARGSUSED*/
static void 
DragContextInitialize(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *numArgs )
{
    XmDragContext	dc = (XmDragContext)new_w;

    dc->drag.roundOffTime = 50;

    dc->drag.dragFinishTime =
      dc->drag.dropFinishTime = 0;

    dc->drag.inDropSite = False;
    dc->drag.dragTimerId = (XtIntervalId) NULL;
    dc->drag.activeBlendModel = dc->drag.blendModel;
    dc->drag.trackingMode = XmDRAG_TRACK_MOTION ;
    dc->drag.curDragOver = dc->drag.origDragOver = NULL;
    dc->drag.startX = dc->drag.startY = 0;

    dc->drag.SaveEventMask = 0L;

    InitDropSiteManager(dc);

    if (dc->drag.exportTargets) {
	unsigned int 	size;
	size = sizeof(Atom) * dc->drag.numExportTargets;
	dc->drag.exportTargets = (Atom *)
	  _XmAllocAndCopy(dc->drag.exportTargets, size);
    }
    dc->core.x =
      dc->core.y = 0;
    dc->core.width =
      dc->core.height = 16;

    /* pixel values in drag context refer to the source widget's colormap */
    if (dc->drag.sourceWidget)  {
	Widget sw = dc->drag.sourceWidget;
	dc->core.colormap = XmIsGadget(sw) ?
	    XtParent(sw)->core.colormap : sw->core.colormap;
    }
  
   XtRealizeWidget((Widget)dc);

    dc->drag.currReceiverInfo = 
      dc->drag.receiverInfos = NULL;
    dc->drag.numReceiverInfos =
      dc->drag.maxReceiverInfos = 0;

    dc->drag.dragDropCancelEffect = False;
}

/*ARGSUSED*/
static Boolean 
DragContextSetValues(
        Widget old,
        Widget ref,
        Widget new_w,
        Arg *args,
        Cardinal *numArgs )
{
    XmDragContext	oldDC = (XmDragContext)old;
    XmDragContext	newDC = (XmDragContext)new_w;
    XmDragOverShellWidget dos = newDC->drag.curDragOver;

    if (oldDC->drag.exportTargets != newDC->drag.exportTargets) {
	if (oldDC->drag.exportTargets) /* should have been freed */
	  XtFree( (char *)oldDC->drag.exportTargets);
	if (newDC->drag.exportTargets) {
	    unsigned int 	size;
	    size = sizeof(Atom) * newDC->drag.numExportTargets;
	    newDC->drag.exportTargets = (Atom *)
	      _XmAllocAndCopy(newDC->drag.exportTargets, size);
	}
    }
    if ( oldDC->drag.operationCursorIcon != newDC->drag.operationCursorIcon ||
      oldDC->drag.sourceCursorIcon != newDC->drag.sourceCursorIcon ||
      oldDC->drag.sourcePixmapIcon != newDC->drag.sourcePixmapIcon ||
      oldDC->drag.stateCursorIcon != newDC->drag.stateCursorIcon ||
      oldDC->drag.cursorBackground != newDC->drag.cursorBackground ||
      oldDC->drag.cursorForeground != newDC->drag.cursorForeground ||
      oldDC->drag.noneCursorForeground != newDC->drag.noneCursorForeground ||
      oldDC->drag.invalidCursorForeground !=
                               newDC->drag.invalidCursorForeground ||
      oldDC->drag.validCursorForeground !=
                               newDC->drag.validCursorForeground) {
      _XmDragOverChange((Widget)dos, dos->drag.cursorState);
    }
    return False;
}

static void 
DragContextDestroy(
        Widget w )
{
  XmDragContext	dc = (XmDragContext)w;
  Cardinal		i;

  /* Fix CR 5556:  Restore root event mask saved at DragStart time */
   if (0 != dc->drag.SaveEventMask) 
      XSelectInput(XtDisplay(dc), dc->drag.currWmRoot, dc->drag.SaveEventMask);

  if (dc->drag.exportTargets)
    XtFree((char *)dc->drag.exportTargets);

  dc->drag.exportTargets = NULL;

  if (dc->drag.dragTimerId)
    {
      XtRemoveTimeOut(dc->drag.dragTimerId);
      dc->drag.dragTimerId = (XtIntervalId) NULL;
    }

  if (dc->drag.receiverInfos)
    {
#ifdef MULTI_SCREEN_DONE
      if (dc->drag.trackingMode != XmDRAG_TRACK_MOTION)
	{
	  EventMask mask;
	  XmDragReceiverInfo info;

	  for (i = 1; i < dc->drag.numReceiverInfos; i++)
	    {
	      info = &(dc->drag.receiverInfos[i]);

	      if (info->shell)
		mask = XtBuildEventMask(info->shell);
	      else
		mask = 0;

	      XSelectInput(XtDisplay(w), info->window, mask);
	    }
	}
#endif /* MULTI_SCREEN_DONE */
      XtFree((char *)dc->drag.receiverInfos);
    }
}

static void 
DragContextClassInitialize( void )
{
  /* The applications should call XtRegisterGrabAction. */
}   

static void 
DragContextClassPartInitialize( WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmDRAG_CONTEXT_BIT);
}

static Window 
GetClientWindow(
        Display *dpy,
        Window win,
        Atom atom )
{
    Window 		root, parent;
    Window 		*children;
    unsigned int 	nchildren;
    int		 	i;
    Atom 		type = None;
    int 		format;
    unsigned long 	nitems, after;
    unsigned char 	*data = NULL;
    Window 		inf = 0;

    XGetWindowProperty(dpy, win, atom, 0, 0, False, AnyPropertyType,
		       &type, &format, &nitems, &after, &data);
    if (data)
	XFree(data);

    if (type)
      return win;
    else {
	if (!XQueryTree(dpy, win, &root, &parent, &children, &nchildren) ||
	    (nchildren == 0))
	  return 0;
	for (i = nchildren - 1; i >= 0; i--) {
	    if ((inf = GetClientWindow(dpy, children[i], atom)) != 0) {
	      XFree(children);
	      return inf;
	    }
	}
	XFree(children);
    }
    return 0;
}

static void 
ValidateDragOver(
        XmDragContext dc,
        unsigned char oldStyle,
        unsigned char newStyle )
{
  Arg		args[1];
  XmDisplay	xmDisplay = (XmDisplay)XtParent(dc);
  unsigned char initiator = XmDRAG_NONE;
  
  initiator = xmDisplay->display.dragInitiatorProtocolStyle;
  
  if (newStyle != oldStyle) 
    {
      /*
       * If we are in pixmap mode, release the grab.
       */
      if (dc->drag.serverGrabbed)
	{
	  XUngrabServer(XtDisplay(dc));
	  dc->drag.serverGrabbed = False;
	  if (xmDisplay -> display.displayHasShapeExtension)
	    XtSetArg(args[0], XmNdragOverMode, XmDRAG_WINDOW);
	  else
	    XtSetArg(args[0], XmNdragOverMode, XmCURSOR);
	  XtSetValues( (Widget)dc->drag.curDragOver, args, 1);
	}
    }
}


static XmDragReceiverInfo
FindReceiverInfo(
        XmDragContext dc,
        Window win )
{
    Cardinal	i;
    XmDragReceiverInfo info = NULL;

    for (i = 0; i < dc->drag.numReceiverInfos; i++) {
	info = &dc->drag.receiverInfos[i];
	if ((info->frame == win) || (info->window == win))
	  break;
    }
    if (i < dc->drag.numReceiverInfos)
      return info;
    else 
      return NULL;
}

XmDragReceiverInfo
_XmAllocReceiverInfo(
        XmDragContext dc )
{
    Cardinal	offset = 0;

    if (dc->drag.currReceiverInfo) {
	offset = (Cardinal) (dc->drag.currReceiverInfo -
			     dc->drag.receiverInfos);
    }
    if (dc->drag.numReceiverInfos == dc->drag.maxReceiverInfos) {
	dc->drag.maxReceiverInfos = dc->drag.maxReceiverInfos*2 + 2;
	dc->drag.receiverInfos = (XmDragReceiverInfoStruct *)
	  XtRealloc((char*)dc->drag.receiverInfos,
		    dc->drag.maxReceiverInfos *
		    sizeof(XmDragReceiverInfoStruct));
    }
    if (offset)
      dc->drag.currReceiverInfo = &(dc->drag.receiverInfos[offset]);
    dc->drag.rootReceiverInfo = dc->drag.receiverInfos;
    return &(dc->drag.receiverInfos[dc->drag.numReceiverInfos++]);
}

/* Find a window with WM_STATE, else return win itself, as per ICCCM */
/*ARGSUSED*/
static void 
GetDestinationInfo(
        XmDragContext dc,
	Window root,
        Window win )
{
    Window	 	clientWin = win;
    Display		*dpy = XtDisplayOfObject((Widget) dc);
    Atom 		WM_STATE =  XInternAtom(dpy, XmSWM_STATE, True);
    unsigned char 	oldStyle = dc->drag.activeProtocolStyle;
    XmDragReceiverInfo 	currReceiverInfo;

    dc->drag.crossingTime = dc->drag.lastChangeTime;

    currReceiverInfo = 
      dc->drag.currReceiverInfo = FindReceiverInfo(dc, win);
        
    /* 
     * check for bootstrap case 
     */
    if ((dc->drag.trackingMode == XmDRAG_TRACK_MOTION) &&
	(XtWindow(dc->drag.srcShell) == win) &&
	(!currReceiverInfo || 
	 (currReceiverInfo->frame == currReceiverInfo->window))) {
	Window		currRoot = dc->drag.currWmRoot;
	int 		root_x, root_y;
        Position	rel_x, rel_y;
	/* 
	 * set frame (win) to something reasonable
	 */
        rel_x = dc->drag.startX - dc->drag.srcShell->core.x;
        rel_y = dc->drag.startY - dc->drag.srcShell->core.y;

        if (rel_x < 0) rel_x = 0;
	if (rel_y < 0) rel_y = 0;

	(void) XTranslateCoordinates(XtDisplayOfObject((Widget) dc), 
				     win, currRoot,
				     rel_x, rel_y,
				     &root_x, &root_y,
				     &win);
	if (currReceiverInfo)
	  currReceiverInfo->frame = win;
    }

    if (currReceiverInfo == NULL) {
	if (clientWin == win) {
	    if ((clientWin = GetClientWindow(dpy, win, WM_STATE)) == 0)
	      clientWin = win;
	}
	currReceiverInfo = 
	  dc->drag.currReceiverInfo = _XmAllocReceiverInfo(dc);
	currReceiverInfo->frame = win;
	currReceiverInfo->window = clientWin;
	currReceiverInfo->shell = XtWindowToWidget(dpy, clientWin);
    }

    /* 
     * we fetch the root info in NewScreen
     */
    if (currReceiverInfo != dc->drag.rootReceiverInfo /* is it the root ? */) {
      if (!currReceiverInfo->shell) {
	if (_XmGetDragReceiverInfo(dpy,
				   currReceiverInfo->window,
				   currReceiverInfo))
	  {
	    switch (currReceiverInfo->dragProtocolStyle) {
	    case XmDRAG_PREREGISTER:
	    case XmDRAG_PREFER_PREREGISTER:
	    case XmDRAG_PREFER_DYNAMIC:
	      break;
	    case XmDRAG_DYNAMIC:
	    case XmDRAG_DROP_ONLY:
	    case XmDRAG_NONE:
	      /* free the data returned by the icc layer */
	      _XmFreeDragReceiverInfo(currReceiverInfo->iccInfo);
	      break;
	    }
	  }
      } else {
	XmDisplay	xmDisplay = (XmDisplay)XtParent(dc);

	/*
	 * We only have a protocol style if we have drop sites.
	 */
	if (_XmDropSiteShell(dc->drag.currReceiverInfo->shell))
	  currReceiverInfo->dragProtocolStyle =
	    xmDisplay->display.dragReceiverProtocolStyle;
	else
	  currReceiverInfo->dragProtocolStyle = XmDRAG_NONE;

	currReceiverInfo->xOrigin = dc->drag.currReceiverInfo->shell->core.x;
	currReceiverInfo->yOrigin = dc->drag.currReceiverInfo->shell->core.y;
	currReceiverInfo->width = dc->drag.currReceiverInfo->shell->core.width;
	currReceiverInfo->height = dc->drag.currReceiverInfo->shell->core.height;
	currReceiverInfo->depth = dc->drag.currReceiverInfo->shell->core.depth;
	currReceiverInfo->iccInfo = NULL;
      }
    }

    /*
     * If we're still waiting on the window manager, then don't mess
     * with the active protocol style.
     */
    if (dc->drag.trackingMode != XmDRAG_TRACK_WM_QUERY_PENDING)
      {
	dc->drag.activeProtocolStyle =
	  _XmGetActiveProtocolStyle((Widget)dc);

	ValidateDragOver(dc, oldStyle, dc->drag.activeProtocolStyle);
      }
}


static void 
GetScreenInfo(
        XmDragContext dc )
{
    Display	*dpy = XtDisplay(dc);
    Window	root = RootWindowOfScreen(XtScreen(dc->drag.curDragOver));
    XmDragReceiverInfo rootInfo;

    /* 
     * the rootInfo is the first entry in the receiverInfo
     * array
     */
    
    if (dc->drag.numReceiverInfos == 0) {
	dc->drag.rootReceiverInfo =
	  rootInfo = _XmAllocReceiverInfo(dc);
    } else {
	dc->drag.rootReceiverInfo =
	  rootInfo = dc->drag.receiverInfos;
    }

    rootInfo->frame = None;
    rootInfo->window = root;
    rootInfo->shell = XtWindowToWidget(dpy, root);
    rootInfo->xOrigin = rootInfo->yOrigin = 0;
    rootInfo->width = XWidthOfScreen(dc->drag.currScreen);
    rootInfo->height = XHeightOfScreen(dc->drag.currScreen);
    rootInfo->depth = DefaultDepthOfScreen(dc->drag.currScreen);
    rootInfo->iccInfo = NULL;

    if (_XmGetDragReceiverInfo(dpy, root, rootInfo))
      {
	  switch (rootInfo->dragProtocolStyle) {
	    case XmDRAG_PREREGISTER:
	    case XmDRAG_PREFER_PREREGISTER:
	    case XmDRAG_PREFER_DYNAMIC:
	      break;
	    case XmDRAG_DYNAMIC:
	    case XmDRAG_DROP_ONLY:
	    case XmDRAG_NONE:
	      /* free the data returned by the icc layer */
	      _XmFreeDragReceiverInfo(rootInfo->iccInfo);
	      break;
	  }
      }
} 


static void 
SendDragMessage(
        XmDragContext dc,
        Window destination,
        unsigned char messageType )
{
    XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay)(XtParent(dc)));
    XmICCCallbackStruct	callbackRec;
    int			reason = _XmMessageTypeToReason(messageType);
    
    callbackRec.any.event = NULL;

    if ((dc->drag.activeProtocolStyle == XmDRAG_NONE) ||
	((dc->drag.activeProtocolStyle == XmDRAG_DROP_ONLY) &&
	 (reason != XmCR_DROP_START)))
      return;

    switch(callbackRec.any.reason = reason) {
      case XmCR_TOP_LEVEL_ENTER:
	{
	    XmTopLevelEnterCallback	callback =
	      (XmTopLevelEnterCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->window = dc->drag.srcWindow;
	    callback->dragProtocolStyle = dc->drag.activeProtocolStyle;
	    callback->screen = dc->drag.currScreen;
	    callback->x = dc->drag.currReceiverInfo->xOrigin;
	    callback->y = dc->drag.currReceiverInfo->yOrigin;
	    callback->iccHandle = dc->drag.iccHandle;
	}
	break;
      case XmCR_TOP_LEVEL_LEAVE:
	{
	    XmTopLevelLeaveCallback	callback =
	      (XmTopLevelLeaveCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->window = dc->drag.srcWindow;
	}
	break;
      case XmCR_DRAG_MOTION:
	{
	    XmDragMotionCallback	callback =
	      (XmDragMotionCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->x = dc->core.x;
	    callback->y = dc->core.y;
	    callback->operation = dc->drag.operation;
	    callback->operations = dc->drag.operations;

	    /* Outgoing motion; be conservative */
	    callback->dropSiteStatus = XmNO_DROP_SITE;
	}
	break;
      case XmCR_OPERATION_CHANGED:
	{
	    XmOperationChangedCallback	callback =
	      (XmOperationChangedCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->operation = dc->drag.operation;
	    callback->operations = dc->drag.operations;
	}
	break;
      case XmCR_DROP_START:
	{
	    XmDropStartCallback 	callback =
	      (XmDropStartCallback)&callbackRec;

	    callback->timeStamp = dc->drag.dragFinishTime;
	    callback->operation = dc->drag.operation;
	    callback->operations = dc->drag.operations;
	    callback->dropAction = dc->drag.dragCompletionStatus;
	    callback->dropSiteStatus = dsm->dropManager.curDropSiteStatus;
	    callback->x = dc->core.x;
	    callback->y = dc->core.y;
	    callback->iccHandle = dc->drag.iccHandle;
	    callback->window = XtWindow(dc->drag.srcShell);
	}
	break;
      default:
	break;
    }

    /* 	
     * if we're the initiator and the destination isn't us and either
     * its the drop message or the dynamic protocol send it to the wire
     */
    if ((!dc->drag.currReceiverInfo->shell) && 
	(!dc->drag.sourceIsExternal /* sanity check */) &&
	((dc->drag.activeProtocolStyle == XmDRAG_DYNAMIC) ||
	 (reason == XmCR_DROP_START)))
      {
	  _XmSendICCCallback(XtDisplayOfObject((Widget) dc), destination,
			     &callbackRec, XmICC_INITIATOR_EVENT);
      }
    else {
	XtPointer			data;
	XmDragTopLevelClientDataStruct	topLevelData;
	XmDragMotionClientDataStruct	motionData;

	if ((reason == XmCR_TOP_LEVEL_ENTER)   ||
	    (reason == XmCR_TOP_LEVEL_LEAVE)   || 
	    (reason == XmCR_DROP_START)){
	    
	    topLevelData.destShell = dc->drag.currReceiverInfo->shell;
	    topLevelData.sourceIsExternal = dc->drag.sourceIsExternal;
	    topLevelData.iccInfo = dc->drag.currReceiverInfo->iccInfo;
	    topLevelData.xOrigin =
			(Position)(dc->drag.currReceiverInfo->xOrigin);
	    topLevelData.yOrigin = (Position)
			(dc->drag.currReceiverInfo->yOrigin);
	    topLevelData.width = (Dimension)
			(dc->drag.currReceiverInfo->width);
	    topLevelData.height = (Dimension)
			(dc->drag.currReceiverInfo->height);
	    topLevelData.window = dc->drag.currReceiverInfo->window;
	    topLevelData.dragOver = (Widget)dc->drag.curDragOver;
	    data = (XtPointer)&topLevelData;
	}
	else if ((reason == XmCR_DRAG_MOTION) ||
	    (reason == XmCR_OPERATION_CHANGED)) {
	    motionData.window = dc->drag.currReceiverInfo->window;
	    motionData.dragOver = (Widget)dc->drag.curDragOver;
	    data = (XtPointer)&motionData;
	}
	else {
		data = NULL;
	}

	_XmDSMUpdate(dsm, 
		     (XtPointer)data,
		     (XtPointer)&callbackRec);
    }
}

static void 
GenerateClientCallback(
        XmDragContext dc,
        unsigned char reason )
{
    XmICCCallbackStruct	callbackRec;
    XtCallbackList	callbackList = NULL;

    callbackRec.any.event = NULL;

    switch(callbackRec.any.reason = reason) {
      case XmCR_TOP_LEVEL_ENTER:
	if ((callbackList = dc->drag.topLevelEnterCallback) != NULL) {
	    XmTopLevelEnterCallback	callback =
	      (XmTopLevelEnterCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->window = dc->drag.currReceiverInfo->window;
	    callback->dragProtocolStyle =
	      dc->drag.activeProtocolStyle;
	    callback->screen = dc->drag.currScreen;
	    callback->iccHandle = dc->drag.iccHandle;
	    callback->x = dc->drag.currReceiverInfo->xOrigin;
	    callback->y = dc->drag.currReceiverInfo->yOrigin;
	}
	break;
      case XmCR_TOP_LEVEL_LEAVE:
	if ((callbackList = dc->drag.topLevelLeaveCallback) != NULL) {
	    XmTopLevelLeaveCallback	callback =
	      (XmTopLevelLeaveCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->screen = dc->drag.currScreen;
	    callback->window = dc->drag.currReceiverInfo->window;
	}
	break;
      case XmCR_DRAG_MOTION:
	if ((callbackList = dc->drag.dragMotionCallback) != NULL) {
	    XmDragMotionCallback	callback =
	      (XmDragMotionCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->x = dc->core.x;
	    callback->y = dc->core.y;
	    callback->operation = dc->drag.operation;
	    callback->operations = dc->drag.operations;

	    /*
	     * If we're over DropOnly client, be optimistic.
	     */
	    if (dc->drag.activeProtocolStyle == XmDRAG_DROP_ONLY)
	      {
		callback->dropSiteStatus = XmVALID_DROP_SITE;
	      }
	    else
	      {
		/*
		 * Otherwise, we're over the root (see
		 * DragMotionProto), and there's no drop site
		 * under us.
		 */
		callback->dropSiteStatus = XmNO_DROP_SITE;
	      }
	}
	break;
      case XmCR_OPERATION_CHANGED:
	if ((callbackList = dc->drag.operationChangedCallback) != NULL) {
	    XmOperationChangedCallback	callback =
	      (XmOperationChangedCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	    callback->operation = dc->drag.operation;
	    callback->operations = dc->drag.operations;

            /*
	     * If we're over DropOnly client, be optimistic.
	     */
	    if (dc->drag.activeProtocolStyle == XmDRAG_DROP_ONLY)
              {
		callback->dropSiteStatus = XmVALID_DROP_SITE;
              }
	    else
              {
		/*
		 * Otherwise, we're over the root (see
		 * DragMotionProto), and there's no drop site
		 * under us.
		 */
		callback->dropSiteStatus = XmNO_DROP_SITE;
              }
	}
	break;
      case XmCR_DROP_SITE_ENTER:
	{
	    XmeWarning((Widget)dc, MESSAGE1);
	}
	break;
      case XmCR_DROP_SITE_LEAVE:
	{
	    XmDropSiteLeaveCallback	callback =
	      (XmDropSiteLeaveCallback)&callbackRec;
	    
	    callback->timeStamp = dc->drag.lastChangeTime;
	}
	break;
      default:
	break;
    }
    if (callbackList) 
    {
      XtCallCallbackList( (Widget)dc, callbackList, &callbackRec);

      if (callbackList == dc->drag.dragMotionCallback)
      {
         XmDragOverShellWidget dos = dc->drag.curDragOver;
         XmDragMotionCallback  callback =
                       (XmDragMotionCallback)&callbackRec;

         dc->drag.operation = callback->operation;
         dc->drag.operations = callback->operations;
         if (dos->drag.cursorState != callback->dropSiteStatus)
         {
            dos->drag.cursorState = callback->dropSiteStatus;
            _XmDragOverChange((Widget)dos, dos->drag.cursorState);
         }
      }
    }
}

/*ARGSUSED*/
static void 
DropLoseIncrSelection(
        Widget w,
        Atom *selection,
        XtPointer clientData )
{
    DropLoseSelection( w, selection) ;
}


static void 
DropLoseSelection(
        Widget w,
        Atom *selection)
{   
  XmDragContext	dc ;

  if (!(dc = (XmDragContext) _XmGetDragContextFromHandle( w, *selection)))
    {
      XmeWarning(w, MESSAGE2) ;
    }

  if (dc && dc->drag.dropFinishTime == 0) 
    {   
      XmeWarning(w, MESSAGE3) ;
    }
} 

static void 
DragDropFinish(
        XmDragContext dc )
{
    Widget w = NULL;

    XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay)(XtParent(dc)));

    /* Handle completion */
    if (dc->drag.dropFinishCallback) {
	XmDropFinishCallbackStruct cb;
	
	cb.reason = XmCR_DROP_FINISH;
	cb.event = NULL;
	cb.timeStamp = dc->drag.dropFinishTime;
	cb.operation = dc->drag.operation;
	cb.operations = dc->drag.operations;
	cb.dropSiteStatus = dsm->dropManager.curDropSiteStatus;
	cb.dropAction = dc->drag.dragCompletionStatus;
	cb.completionStatus = dc->drag.dragDropCompletionStatus;
	XtCallCallbackList((Widget) dc, dc->drag.dropFinishCallback, &cb);
	dc->drag.dragDropCompletionStatus = cb.completionStatus;
    }

    if (dc->drag.blendModel != XmBLEND_NONE &&
	dc->drag.dragDropCancelEffect == False) {
      _XmDragOverFinish((Widget)dc->drag.curDragOver, 
			dc->drag.dragDropCompletionStatus);
    }

    if (dc->drag.dragDropFinishCallback) {
	XmDragDropFinishCallbackStruct cb;
	
	cb.reason = XmCR_DRAG_DROP_FINISH;
	cb.event = NULL;
	cb.timeStamp = dc->drag.dropFinishTime;
	XtCallCallbackList((Widget) dc, dc->drag.dragDropFinishCallback, &cb);
    }
    /*
     * we send this now so that the non-local receiver can clean up
     * its dc after everything is done
     */

    XtDisownSelection(dc->drag.srcShell, 
		      dc->drag.iccHandle,
		      dc->drag.dragFinishTime);

    _XmFreeMotifAtom((Widget)dc, dc->drag.iccHandle);

    XtRemoveEventHandler(dc->drag.srcShell, FocusChangeMask, True,
			 InitiatorMsgHandler, 
			 (XtPointer)dc);

    XtVaGetValues((Widget)dc, XmNsourceWidget, &w, NULL);
    if (w)
	XtRemoveCallback(w, XmNdestroyCallback, cancelDrag, (XtPointer)dc);

    XtDestroyWidget((Widget) dc);
}



/*
 * This callback is called when a drag operation needs to be terminated.
 * This can occur when the widget whose id is contained in the drag data 
 * structure is destroyed.
 */
static void 
cancelDrag(
        Widget w,
 	XtPointer client,
 	XtPointer call)
{
    Widget dragContext = (Widget)client;
    XmDragCancel(dragContext);
}

/*
 * This routine is passed as the frontend to the convertProc.
 */
/*VARARGS*/
static Boolean 
DropConvertIncrCallback(
        Widget w,
        Atom *selection,
        Atom *target,
        Atom *typeRtn,
        XtPointer *valueRtn,
        unsigned long *lengthRtn,
        int *formatRtn,
        unsigned long *maxLengthRtn,
        XtPointer clientData,
        XtRequestId *requestID )
{
    enum { 
      XmATRANSFER_SUCCESS, XmATRANSFER_FAILURE,
      XmA_MOTIF_CANCEL_DROP_EFFECT, XmA_MOTIF_DROP, XmATARGETS, NUM_ATOMS };
    static char *atom_names[] = {
      XmSTRANSFER_SUCCESS, XmSTRANSFER_FAILURE,
      XmS_MOTIF_CANCEL_DROP_EFFECT, XmS_MOTIF_DROP, XmSTARGETS };

    XmDragContext	dc;
    Time		dropTime;
    Boolean 		returnVal = True;
    Boolean 		success;
    Atom		atoms[XtNumber(atom_names)];

    dropTime = XtGetSelectionRequest(w, *selection, requestID)->time;

    if (!(dc = (XmDragContext)
	  _XmGetDragContextFromHandle(w, *selection))) {
	XmeWarning(w, MESSAGE2);
	return False;
    }
    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplayOfObject((Widget) dc), atom_names,
		 XtNumber(atom_names), False, atoms);
    if ((success = (*target == atoms[XmATRANSFER_SUCCESS])) ||
	(*target == atoms[XmATRANSFER_FAILURE]))
	{
	  if (success)
	    dc->drag.dragDropCompletionStatus = XmDROP_SUCCESS;
	  else
	    dc->drag.dragDropCompletionStatus = XmDROP_FAILURE;
	  *typeRtn = *target;
	  *lengthRtn = 0;
	  *formatRtn = 32;
	  *valueRtn = NULL;
	  *maxLengthRtn = 0;
	  /*
	   * the time is really of the start of the transfer but ...
	   */
	  dc->drag.dropFinishTime = dropTime;
	  DragDropFinish(dc);
      }
    else if (*target == atoms[XmA_MOTIF_CANCEL_DROP_EFFECT]) 
      {
	dc->drag.dragDropCancelEffect = True;
      }
    else /* normal transfer */
      {
	  Atom		motifDrop = atoms[XmA_MOTIF_DROP];
	  returnVal =  (Boolean)((*(dc->drag.convertProc.sel_incr))
				 ((Widget)dc, 
				  &motifDrop,
				  target, 
				  typeRtn, 
				  valueRtn, 
				  lengthRtn, 
				  formatRtn,
				  maxLengthRtn,
				  clientData,
				  requestID));
      }
    
    /* add code to handle TARGET target automatically if not there yet */
    if ((! returnVal) && (*target == atoms[XmATARGETS])) {
	int len = sizeof(Atom) * dc->drag.numExportTargets;
 
	if (dc->drag.incremental & 0x2) {
	    dc->drag.incremental = 1;
	    *valueRtn = NULL;
	    *lengthRtn = 0;
	} else {
	    *valueRtn = XtMalloc(len);
	    memmove(*valueRtn, dc->drag.exportTargets, len);
	    *lengthRtn = dc->drag.numExportTargets;
	    dc->drag.incremental = 3;
	}
	*formatRtn = 32;
	*typeRtn = XA_ATOM;
	returnVal = True;
    }

    return returnVal;
}

/*
 * This routine is passed as the frontend to the convertProc.
 */
/*VARARGS*/
static Boolean 
DropConvertCallback(
        Widget w,
        Atom *selection,
        Atom *target,
        Atom *typeRtn,
        XtPointer *valueRtn,
        unsigned long *lengthRtn,
        int *formatRtn )
{
    enum { 
      XmATRANSFER_SUCCESS, XmATRANSFER_FAILURE,
      XmA_MOTIF_CANCEL_DROP_EFFECT, XmA_MOTIF_DROP, XmATARGETS, NUM_ATOMS };
    static char *atom_names[] = {
      XmSTRANSFER_SUCCESS, XmSTRANSFER_FAILURE,
      XmS_MOTIF_CANCEL_DROP_EFFECT, XmS_MOTIF_DROP, XmSTARGETS };

    XmDragContext	dc;
    Time		dropTime;
    Boolean 		returnVal = True;
    Boolean		success;
    Atom		atoms[XtNumber(atom_names)];

    dropTime = XtGetSelectionRequest(w, *selection, NULL)->time;

    if (!(dc = (XmDragContext)
	  _XmGetDragContextFromHandle(w, *selection))) {
	XmeWarning(w, MESSAGE2);
	return False;
    }
    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplayOfObject((Widget) dc), atom_names,
		 XtNumber(atom_names), False, atoms);
    if ((success = (*target == atoms[XmATRANSFER_SUCCESS])) ||
	(*target == atoms[XmATRANSFER_FAILURE]))
      {
	  if (success)
	    dc->drag.dragDropCompletionStatus = XmDROP_SUCCESS;
	  else
	    dc->drag.dragDropCompletionStatus = XmDROP_FAILURE;
	  *typeRtn = *target;
	  *lengthRtn = 0;
	  *formatRtn = 32;
	  *valueRtn = NULL;
	  /*
	   * the time is really of the start of the transfer but ...
	   */
	  dc->drag.dropFinishTime = dropTime;
	  DragDropFinish(dc);
      }
    else if (*target == atoms[XmA_MOTIF_CANCEL_DROP_EFFECT])
      {
	dc->drag.dragDropCancelEffect = True;
      }
    else /* normal transfer */
      {
	  Atom		motifDrop = atoms[XmA_MOTIF_DROP];
	  returnVal =  (Boolean)
		       ((*(dc->drag.convertProc.sel))
				 ((Widget)dc, 
				  &motifDrop,
				  target, 
				  typeRtn, 
				  valueRtn, 
				  lengthRtn, 
				  formatRtn));
      }

    /* add code to handle TARGET target automatically if not there yet */
    if ((! returnVal) && (*target == atoms[XmATARGETS])) {
	int len = sizeof(Atom) * dc->drag.numExportTargets;
	*valueRtn = XtMalloc(len);
	memmove(*valueRtn, dc->drag.exportTargets, len);
	*lengthRtn = dc->drag.numExportTargets;
	*formatRtn = 32;
	*typeRtn = XA_ATOM;
	returnVal = True;
    }
 
    return returnVal;
}

/*ARGSUSED*/
static void 
DragStartProto(
        XmDragContext dc)
{
    /*
     * bootstrap the top_level_window code
     */
    _XmWriteInitiatorInfo((Widget)dc);
    GetDestinationInfo(dc, 
		       RootWindowOfScreen(XtScreen(dc)),
		       XtWindow(dc->drag.srcShell));
    GenerateClientCallback(dc, XmCR_TOP_LEVEL_ENTER);
    SendDragMessage(dc, dc->drag.currReceiverInfo->window, XmTOP_LEVEL_ENTER);
    SendDragMessage(dc, dc->drag.currReceiverInfo->window, XmDRAG_MOTION);
}


static void 
NewScreen(
	  XmDragContext	dc,
	  Window	newRoot)
{
  XmDisplay	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(dc));
  Cardinal	i;
  Arg		args[8];
  Widget  old = (Widget) (dc->drag.curDragOver);
  
  /* Find the new screen number */
  for (i = 0; i < XScreenCount(XtDisplayOfObject((Widget) dc)); i++)
    if (RootWindow(XtDisplayOfObject((Widget) dc), i) == newRoot)
      break;
  
  dc->drag.currScreen =
    ScreenOfDisplay(XtDisplayOfObject((Widget) dc), i);
  dc->drag.currWmRoot = RootWindowOfScreen(dc->drag.currScreen);
  
  
  /* Build a new one */
  i = 0;
  /*
   * Always come up in window mode (or cursor if SHAPE is not available).
   */
  if (dpy -> display.displayHasShapeExtension)
    XtSetArg(args[i], XmNdragOverMode, XmDRAG_WINDOW);
  else
    XtSetArg(args[i], XmNdragOverMode, XmCURSOR);
  i++;
  
  XtSetArg(args[i], XmNhotX, dc->core.x); i++;
  XtSetArg(args[i], XmNhotY, dc->core.y); i++;
  XtSetArg(args[i], XmNbackgroundPixmap, None); i++;
  XtSetArg(args[i], XmNscreen, dc->drag.currScreen);i++;
  XtSetArg(args[i], XmNdepth, DefaultDepthOfScreen(dc->drag.currScreen));i++;
  XtSetArg(args[i], XmNcolormap, 
	   DefaultColormapOfScreen(dc->drag.currScreen));i++;
  XtSetArg(args[i], XmNvisual, DefaultVisualOfScreen(dc->drag.currScreen));i++;
  
  /*
   * As popup child(ren) of the dc, the drag over(s) will 
   * automatically destroyed by Xt when the dc is destroyed.
   * Isn't that handy?
   */
  dc->drag.curDragOver = (XmDragOverShellWidget)
    XtCreatePopupShell("dragOver", xmDragOverShellWidgetClass,
		       (Widget) dc, args, i);
  
  if (dc->drag.currScreen == XtScreen(dc->drag.srcShell))
    _XmDragOverSetInitialPosition((Widget)dc->drag.curDragOver,
				  dc->drag.startX, dc->drag.startY);
  
  if (old != NULL) {
    if (old != (Widget) (dc->drag.origDragOver))
      XtDestroyWidget(old);
    else
      _XmDragOverHide((Widget)dc->drag.origDragOver, 0, 0, NULL);
  }
  
  GetScreenInfo(dc);
  
  if (dc->drag.origDragOver == NULL)
    {
      dc->drag.origDragOver = dc->drag.curDragOver;
    }
  
  if (dc->drag.trackingMode == XmDRAG_TRACK_MOTION)
    {
      EventMask	mask = _XmDRAG_EVENT_MASK(dc);
      
      if (XGrabPointer(XtDisplayOfObject((Widget) dc->drag.curDragOver),
		       RootWindowOfScreen(XtScreen(dc->drag.curDragOver)),
		       False,
		       mask,
		       GrabModeSync,
		       GrabModeAsync,
		       None,
		       _XmDragOverGetActiveCursor((Widget)dc->drag.curDragOver),
		       dc->drag.lastChangeTime) != GrabSuccess)
	Warning(MESSAGE4);
      
      XAllowEvents(XtDisplayOfObject((Widget) dc->drag.srcShell),
		   SyncPointer,
		   dc->drag.lastChangeTime);
    }
}


/*ARGSUSED*/
static void 
LocalNotifyHandler(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDropSiteManagerObject 	dsm = (XmDropSiteManagerObject)w;
    XmDragContext	dc = (XmDragContext)client;

    switch(((XmAnyICCCallback)call)->reason) {
      case XmCR_DROP_SITE_ENTER:
	SiteEnteredWithLocalSource((Widget)dsm, (XtPointer)dc, (XtPointer)call);
	break;

      case XmCR_DROP_SITE_LEAVE:
	/* SiteLeftWithLocalSource expect to find an
         * XmDropSiteEnterPendingCallbackStruct, so upgrade the
	 * XmDropSiteLeaveCallbackStruct. 
	 */
        {
	XmDropSiteLeaveCallbackStruct *cb = 
	    (XmDropSiteLeaveCallbackStruct *) call;
        XmDropSiteEnterPendingCallbackStruct new_call;
	new_call.reason = cb->reason;
	new_call.event = cb->event;
	new_call.timeStamp = cb->timeStamp;
	new_call.enter_pending = False;
	SiteLeftWithLocalSource((Widget) dsm, (XtPointer)dc, 
				(XtPointer) &new_call);
	break;
	}

      case XmCR_DRAG_MOTION:
	SiteMotionWithLocalSource((Widget)dsm, (XtPointer)dc, (XtPointer)call);
	break;

      case XmCR_OPERATION_CHANGED:
	OperationChanged((Widget)dsm, (XtPointer)dc, (XtPointer)call);
	break;

      case XmCR_DROP_START:
	DropStartConfirmed((Widget)dsm, (XtPointer)dc, (XtPointer)call);

      default:
	break;
    }
}

/*
 * sends replies to drag messages 
 */
/*ARGSUSED*/
static void 
ExternalNotifyHandler(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmAnyICCCallback	cb = (XmAnyICCCallback)call;

    switch(cb->reason) {
      case XmCR_DROP_SITE_ENTER:
      case XmCR_DROP_SITE_LEAVE:
      case XmCR_DRAG_MOTION:
      case XmCR_OPERATION_CHANGED:
      case XmCR_DROP_START:
	/*
	 * send a message to the external source 
	 */
	_XmSendICCCallback(XtDisplayOfObject((Widget) dc), 
			   dc->drag.srcWindow, 
			   (XmICCCallback)cb,
			   XmICC_RECEIVER_EVENT);
	break;

      default:
	XmeWarning((Widget)dc, MESSAGE5);
	break;
    }
}


/*
 * catches replies on drag messages 
 */
/*ARGSUSED*/
static void 
InitiatorMsgHandler(
        Widget w,
        XtPointer clientData,
        XEvent *event,
        Boolean *dontSwallow )
{
  XmDragContext	dc =(XmDragContext)clientData;
  XmICCCallbackStruct		callbackRec;

  if ((dc && (event->type != ClientMessage)) ||
      (!_XmICCEventToICCCallback((XClientMessageEvent *)event,
				 &callbackRec, XmICC_RECEIVER_EVENT)) ||
      (dc->drag.dragStartTime > callbackRec.any.timeStamp) ||
      (dc->drag.crossingTime > callbackRec.any.timeStamp))
    return;

  LocalNotifyHandler(w, (XtPointer)dc, (XtPointer)&callbackRec);
}

/*ARGSUSED*/
static void 
SiteEnteredWithLocalSource(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmDropSiteEnterCallbackStruct  *cb = (XmDropSiteEnterCallbackStruct *)call;
    
    CalculateDragOperation(dc);

    /* check against the current location of the pointer */

    if (dc->drag.siteEnterCallback) {
	XtCallCallbackList((Widget) dc, dc->drag.siteEnterCallback, cb);
    }
    dc->drag.operation = cb->operation;
    dc->drag.operations = cb->operations;
    dc->drag.inDropSite = True;

    _XmDragOverChange((Widget)dc->drag.curDragOver, cb->dropSiteStatus);
}

/*ARGSUSED*/
static void 
SiteLeftWithLocalSource(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmDropSiteEnterPendingCallbackStruct  *cb =
			 (XmDropSiteEnterPendingCallbackStruct *)call;
    
    dc->drag.inDropSite = False;

    if (dc->drag.siteLeaveCallback) {
	XtCallCallbackList((Widget) dc, dc->drag.siteLeaveCallback,
			   (XmDropSiteLeaveCallbackStruct *)cb);
    }

    CalculateDragOperation(dc);

    /*
     * Don't forward laggard echo leaves to dragUnder
     */
    if (dc->drag.dragFinishTime == 0 && !cb->enter_pending)
      _XmDragOverChange((Widget)dc->drag.curDragOver, XmNO_DROP_SITE);
}

/*ARGSUSED*/
static void 
OperationChanged(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmOperationChangedCallbackStruct  *cb = (XmOperationChangedCallbackStruct *)call;
    
    if (dc->drag.operationChangedCallback) {
	XtCallCallbackList((Widget) dc, dc->drag.operationChangedCallback, cb);
    }
    dc->drag.operation = cb->operation;
    dc->drag.operations = cb->operations;
    _XmDragOverChange((Widget)dc->drag.curDragOver, cb->dropSiteStatus);
}

/*ARGSUSED*/
static void 
SiteMotionWithLocalSource(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmDragMotionCallbackStruct  *cb = (XmDragMotionCallbackStruct *)call;

    if (dc->drag.dragMotionCallback) {
	XtCallCallbackList((Widget) dc, dc->drag.dragMotionCallback, cb);
    }

    /* Application should not be able to change callback data so
     * don't call _XmDragOverChange(), this will result in better
     * drag performance.
     */
}


/*ARGSUSED*/
static void 
DropStartConfirmed(
        Widget w,
        XtPointer client,
        XtPointer call )
{
    XmDragContext	dc = (XmDragContext)client;
    XmDropStartCallbackStruct  *cb = (XmDropStartCallbackStruct *)call;
    XtAppContext	appContext = XtWidgetToApplicationContext((Widget)dc);

    if (dc->drag.dragTimerId) {
	XtRemoveTimeOut(dc->drag.dragTimerId);
	dc->drag.dragTimerId = (XtIntervalId) NULL;
    }

    /* Add a long timeout in case the drop site dies. */
    dc->drag.dragTimerId = 
      XtAppAddTimeOut(appContext,
		      XtAppGetSelectionTimeout(appContext) * 10,
		      DropFinishTimeout,
		      (XtPointer)dc);

    if (dc->drag.dropStartCallback) {
	XtCallCallbackList( (Widget)dc, dc->drag.dropStartCallback, cb);
    }
    dc->drag.dragCompletionStatus = cb->dropAction;

    switch(dc->drag.dragCompletionStatus) {
      case XmDROP:
      case XmDROP_INTERRUPT:
      case XmDROP_HELP:
	break;
      case XmDROP_CANCEL: 
	break;
    }
}


static Widget 
GetShell(
        Widget w )
{
    while (w && !XtIsShell(w))
      w = XtParent(w);
    return w;
}

static void 
InitDropSiteManager(
        XmDragContext dc )
{
    XmDropSiteManagerObject	dsm;
    Arg				args[4];
    Cardinal			i = 0;

    dsm = _XmGetDropSiteManagerObject((XmDisplay)(XtParent(dc)));
    
    XtSetArg(args[i], XmNclientData, dc); i++;
    if (dc->drag.sourceIsExternal) {
	XtSetArg(args[i], XmNnotifyProc, ExternalNotifyHandler); i++;
    }
    else {
	XtSetArg(args[i], XmNnotifyProc, LocalNotifyHandler); i++;
    }
    XtSetValues((Widget)dsm, args, i);
}

/*ARGSUSED*/
static void 
TopWindowsReceived(
        Widget w,
        XtPointer client_data,
        Atom *selection,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format )
{
    XmDragContext dc = (XmDragContext)client_data;
	XmDisplay dd = (XmDisplay) w;
    Cardinal	i;
    XmDragReceiverInfo	currInfo, startInfo;
    Window	*clientWindows;
    unsigned char	oldStyle;

    if (dd->display.activeDC != dc) {
      /* Too late... */
      return;
    }

    /* CR 6337.  Must call DragOverChange if the blendModel
       changes.  This doesn't happen often,  only in certain
       cases involving PREREGISTER */

    if (dc -> drag.blendModel != dc -> drag.activeBlendModel) {
      dc->drag.blendModel = dc->drag.activeBlendModel;
      _XmDragOverChange((Widget)dc->drag.curDragOver, XmNO_DROP_SITE);
    }


#ifdef MULTI_SCREEN_DONE
    if ((*length != 0) && (*format == 32) && (*type == XA_WINDOW)) { 
	/* 
	 * we make a receiverInfo array one larger than the number of
	 * client windows since we keep the root info in array[0].
	 */
	if (dc->drag.numReceiverInfos >= 1)
	  startInfo = dc->drag.receiverInfos;
	else
	  startInfo = NULL;

	dc->drag.numReceiverInfos = 
	  dc->drag.maxReceiverInfos = *length + 1;
	dc->drag.receiverInfos = (XmDragReceiverInfo)
	  XtCalloc(dc->drag.maxReceiverInfos, sizeof(XmDragReceiverInfoStruct));
	clientWindows = (Window *)value;

	if (startInfo) {
	    memcpy((char *)dc->drag.receiverInfos, 
		   (char *)startInfo,
		   sizeof(XmDragReceiverInfoStruct));
	    dc->drag.rootReceiverInfo = dc->drag.receiverInfos;
	    XtFree((char *)startInfo);
	}
#ifdef DEBUG
	else
	  Warning("we don't have startInfo when we should\n");
#endif /* DEBUG */	
	for (i = 1; i < dc->drag.numReceiverInfos; i++) {
	    currInfo = &dc->drag.receiverInfos[i];
	    currInfo->window = clientWindows[i-1];
	    currInfo->shell = XtWindowToWidget(XtDisplay(dc), 
					       currInfo->window);
	    if (currInfo->shell == NULL) {
		XSelectInput(XtDisplay(dc),
			     currInfo->window,
			     EnterWindowMask | LeaveWindowMask);
	    }
			     
	}
	/*
	 * set the currReceiver to the srcShell since that's where
	 * we're confined to 
	 */
	dc->drag.currReceiverInfo = 
	  FindReceiverInfo(dc, XtWindow(dc->drag.srcShell));
	dc->drag.trackingMode = XmDRAG_TRACK_WM_QUERY;

	oldStyle = dc->drag.activeProtocolStyle;
	dc->drag.activeProtocolStyle =
		_XmGetActiveProtocolStyle((Widget)dc);
	ValidateDragOver(dc, oldStyle, dc->drag.activeProtocolStyle);
    }
    else
#endif /* MULTI_SCREEN_DONE */
    {
	EventMask	mask;
	Window		confineWindow;
	Cursor		cursor;

	dc->drag.trackingMode = XmDRAG_TRACK_MOTION;
	GetDestinationInfo(dc, 
			   dc->drag.currWmRoot,
			   dc->drag.currReceiverInfo->window);
#ifndef MULTI_SCREEN_DONE
	confineWindow = RootWindowOfScreen(XtScreen(dc));
#else
	confineWindow = None;
#endif /* MULTI_SCREEN_DONE */

	/*
	 * we need to regrab so that the confine window can be changed
	 * from the source window. If there was another value for
	 * trackingMode like XmDRAG_TRACK_WM_QUERY_FAILED we could do
	 * this in DragStartWithTracking
	 */

	mask = _XmDRAG_EVENT_MASK(dc);
	cursor = _XmDragOverGetActiveCursor((Widget)dc->drag.curDragOver);

	if (XGrabPointer(XtDisplayOfObject((Widget) dc),
			 RootWindowOfScreen(XtScreen(dc)),
			 False,
			 mask,
			 GrabModeSync,
			 GrabModeAsync,
			 confineWindow,
			 cursor,
			 dc->drag.lastChangeTime) != GrabSuccess)
	  Warning(MESSAGE4);
    }
#ifdef MULTI_SCREEN_DONE
    if (value)
      XtFree((char *)value);
#endif /* MULTI_SCREEN_DONE */

    DragStartWithTracking(dc);
}


/* ARGSUSED */
static void 
DragStart(
        XmDragContext dc,
        Widget src,
        XEvent *event )
{
    XmDisplay		dd;
    unsigned char 	activeProtocolStyle;
    unsigned int	state = event->xbutton.state;
    EventMask		mask;
    Window		saveWindow;
    Window		confineWindow;
    Cursor		cursor = None;

    dd = (XmDisplay)XtParent(dc);
    dd->display.activeDC = dc;
    dd->display.userGrabbed = True;

    dc->drag.crossingTime = 
      dc->drag.dragStartTime = 
        dc->drag.lastChangeTime = event->xbutton.time;

    dc->drag.startX = dc->core.x = event->xbutton.x_root;
    dc->drag.startY = dc->core.y = event->xbutton.y_root;
    dc->drag.curDragOver = NULL;
    dc->drag.origDragOver = NULL;
    dc->drag.srcShell = GetShell(src);
    dc->drag.srcWindow = XtWindow(dc->drag.srcShell);
    dc->drag.iccHandle = _XmAllocMotifAtom((Widget)dc, dc->drag.dragStartTime);
    
    if (dc->drag.incremental) 
      XtOwnSelectionIncremental(dc->drag.srcShell, 
				dc->drag.iccHandle,
				dc->drag.dragStartTime,
				DropConvertIncrCallback,
				DropLoseIncrSelection,
				NULL, NULL, dc->drag.clientData);
    else
      XtOwnSelection(dc->drag.srcShell, 
		     dc->drag.iccHandle,
		     dc->drag.dragStartTime,
		     DropConvertCallback,
		     DropLoseSelection,
		     NULL);
    

    dc->drag.serverGrabbed = False;
    dc->drag.sourceIsExternal = False;

    dc->drag.activeProtocolStyle = activeProtocolStyle =
      _XmGetActiveProtocolStyle((Widget)dc);

    switch (dc->drag.activeProtocolStyle)
      {
      case XmDRAG_PREREGISTER:
	dc->drag.activeProtocolStyle = XmDRAG_DYNAMIC;
      case XmDRAG_DYNAMIC:
	break;
      case XmDRAG_DROP_ONLY:
	dc->drag.activeProtocolStyle = XmDRAG_NONE;
      case XmDRAG_NONE:
	break;
      }

    /* must be either DYNAMIC or NONE at this point */

    /*
     * start out with the default operations in effective operations
     */

    dc->drag.lastEventState = state;
    CalculateDragOperation(dc);
    dc->drag.sourceIsExternal = False;

    /*
     * if we're in query_pending mode then initialize the
     * currReceiverInfo to us
     */
    if (dc->drag.trackingMode == XmDRAG_TRACK_MOTION) {
	dc->drag.activeProtocolStyle = activeProtocolStyle;
#ifndef MULTI_SCREEN_DONE
      confineWindow = RootWindowOfScreen(XtScreen(dc));
#else
      confineWindow = None;
    } else { /* XmDRAG_TRACK_WM_QUERY */
	dc->drag.trackingMode = XmDRAG_TRACK_WM_QUERY_PENDING;
	confineWindow = XtWindow(dc->drag.srcShell);
    }
#endif /*  MULTI_SCREEN_DONE */

    if (dc->drag.trackingMode == XmDRAG_TRACK_WM_QUERY_PENDING &&
        activeProtocolStyle == XmDRAG_PREREGISTER) {
       dc->drag.blendModel = XmBLEND_NONE;
    }

    NewScreen(dc, RootWindowOfScreen(XtScreen(dc)));

    XtInsertEventHandler(dc->drag.srcShell, FocusChangeMask, True,
			 InitiatorMsgHandler, 
			 (XtPointer)dc,
			 XtListHead);

    mask = _XmDRAG_EVENT_MASK(dc);
    
    /*
     * we grab on the Xt pointer so that Xt doesn't interfere with us.
     * Also once we have the WM_QUERY capability this will work.
     * Otherwise we need to grab on the root so we can track the
     * changes in sub_window and infer the top_level crossings
     */
    /*
     * some more sleaze to get around the ungrab.  Since we can't rely
     * on the caller to have done an owner_events true (for Xt), we need to
     * guarantee that the release event goes to us. The grab window
     * must be viewable so we can't use the dc window
     */
	
    saveWindow = dc->core.window;

    cursor = _XmDragOverGetActiveCursor((Widget)dc->drag.curDragOver);

    dc->core.window = RootWindowOfScreen(XtScreen(dc));
    if ((XtGrabPointer((Widget)dc,
		       False,
		       (unsigned int) mask, 
		       GrabModeSync,
		       GrabModeAsync,
		       confineWindow,
		       cursor,
		       dc->drag.dragStartTime) != GrabSuccess) ||
	(XGrabPointer(XtDisplayOfObject((Widget) dc),
		      RootWindowOfScreen(XtScreen(dc)),
		      False,
		      mask, 
		      GrabModeSync,
		      GrabModeAsync,
		      confineWindow,
		      cursor,
		      dc->drag.dragStartTime) != GrabSuccess) ||
	(XGrabKeyboard(XtDisplayOfObject((Widget) dc), 
		       RootWindowOfScreen(XtScreen(dc)),
		       False,
		       GrabModeSync,
		       GrabModeAsync,
		       dc->drag.dragStartTime) != GrabSuccess)) 
      Warning(MESSAGE4);
    _XmAddGrab((Widget)dc, True, False);
    dc->core.window = saveWindow;
 
    /* Begin fixing OSF CR 5556 */ 
    /*
     * Add ButtonMotionMask to the already set-up event mask for root window.
     * This gets reinstalled in DragContextDestroy()
     */
    {
      XWindowAttributes       xwa;
      
      XGetWindowAttributes(XtDisplay(dc), dc->drag.currWmRoot, &xwa);
      dc->drag.SaveEventMask = xwa.your_event_mask;
      XSelectInput(XtDisplay(dc),
		   dc->drag.currWmRoot,
		   xwa.your_event_mask | ButtonMotionMask);
    }
    /* End fixing OSF CR 5556 */ 

    if (dc->drag.trackingMode == XmDRAG_TRACK_WM_QUERY_PENDING) {
        enum { XmA_MOTIF_WM_QUERY, XmA_MOTIF_WM_ALL_CLIENTS, NUM_ATOMS };
	static char *atom_names[] = { 
	  XmS_MOTIF_WM_QUERY, XmS_MOTIF_WM_ALL_CLIENTS };
	Atom atoms[XtNumber(atom_names)];

	assert(XtNumber(atom_names) == NUM_ATOMS);
	XInternAtoms(XtDisplay(dc), atom_names, XtNumber(atom_names), 
		     False, atoms);

	XtGetSelectionValue((Widget)dd,
			    atoms[XmA_MOTIF_WM_QUERY],
			    atoms[XmA_MOTIF_WM_ALL_CLIENTS],
			    TopWindowsReceived, 
			    (XtPointer)dc,
			    dc->drag.dragStartTime);

	XAllowEvents(XtDisplayOfObject((Widget) dc->drag.srcShell),
		     SyncPointer,
		     dc->drag.dragStartTime);
    }
    else 
      DragStartWithTracking(dc);

    XSync(XtDisplay(dc), False);
 
    XtAppAddTimeOut( XtWidgetToApplicationContext( (Widget)dc),
		    0, InitiatorMainLoop,
		    (XtPointer)(&dd->display.activeDC));
}


/* ARGSUSED */
static void 
DragStartWithTracking(
        XmDragContext dc)
{
    if (dc->drag.dragFinishTime)
      return;

    if (dc->drag.trackingMode == XmDRAG_TRACK_WM_QUERY) {
	EventMask	mask = _XmDRAG_EVENT_MASK(dc);
	Window		confineWindow;
	Cursor		cursor;

#ifndef MULTI_SCREEN_DONE
	confineWindow = RootWindowOfScreen(XtScreen(dc));
#else
	confineWindow = None;
#endif
	cursor = _XmDragOverGetActiveCursor((Widget)dc->drag.curDragOver);

	/*
	 * set owner events to true so that the crossings and motion
	 * are reported relative to the destination windows. Don't
	 * tell Xt about it so we can continue to get everything
	 * reported to the dragContext via the spring-loaded/XtGrab
	 * interaction. Blegh
	 */
	if (XGrabPointer(XtDisplayOfObject((Widget) dc),
			 RootWindowOfScreen(XtScreen(dc)),
			 True,
			 mask,
			 GrabModeSync,
			 GrabModeAsync,
			 confineWindow,
			 cursor,
			 dc->drag.lastChangeTime) != GrabSuccess)
	  Warning(MESSAGE4);
    }

    XAllowEvents(XtDisplayOfObject((Widget) dc->drag.srcShell),
		 SyncPointer,
		 dc->drag.lastChangeTime);
}

static void 
UpdateMotionBuffer(
	XmDragContext dc,
        MotionBuffer mb,
	XEvent	     *event)
{
    Time time ;
    unsigned int state;
    Window window, subwindow;
    Position x ;
    Position y ;

    if (dc->drag.currReceiverInfo == NULL)
      return;

    dc->drag.lastChangeTime = event->xmotion.time;

    /*
     * we munged the window and subwindow fields before calling
     * XtDispatchEvent so we could get the event thru to the
     * DragContext.  The subwindow field will hold the interesting
     * info. The window field is always set (by us) to the DC window.
     */
    switch(event->type) {
      case MotionNotify:
	if (mb->count && ((mb->count % (STACKMOTIONBUFFERSIZE)) == 0)) {
	    if (mb->count == (STACKMOTIONBUFFERSIZE)){
		MotionBuffer	oldMb = mb;
		Cardinal size;
		
		size = sizeof(MotionBufferRec) +
		  (STACKMOTIONBUFFERSIZE * sizeof(MotionEntryRec));
		mb = (MotionBuffer) XtMalloc(size);
		memcpy((char *)mb, (char *)oldMb, sizeof(MotionBufferRec));
	    }
	    else  {
		mb = (MotionBuffer)
		  XtRealloc((char *)mb, 
			    (sizeof(MotionBufferRec) +
			     (mb->count + STACKMOTIONBUFFERSIZE) *sizeof(MotionEntryRec)));
	    }
	}
	/*	
	 * for right now use the root although this wont work for
	 * pseudo-roots
	 */
	time = event->xmotion.time;
	state = event->xmotion.state;
	x = event->xmotion.x_root;
	y = event->xmotion.y_root;
	window = event->xmotion.root;
	if (dc->drag.trackingMode != XmDRAG_TRACK_MOTION) {
	    subwindow = mb->currReceiverInfo->window;
	}
	else {
	    subwindow = event->xmotion.subwindow;
	}
	mb->entries[mb->count].time = time;
	mb->entries[mb->count].window = window;
	mb->entries[mb->count].subwindow = subwindow;
	mb->entries[mb->count].state = state;
	mb->entries[mb->count].x = x;
	mb->entries[mb->count++].y = y;
	break;

      case EnterNotify:
	if ((event->xcrossing.mode == NotifyNormal) &&
	    (dc->drag.trackingMode != XmDRAG_TRACK_MOTION)) {
	    XmDragReceiverInfo	rInfo;
	    if ((rInfo = FindReceiverInfo(dc, event->xcrossing.subwindow))
		!= NULL)
	      mb->currReceiverInfo = rInfo;
	}
	break;

      case LeaveNotify:
	if ((event->xcrossing.mode == NotifyNormal) &&
	    (dc->drag.trackingMode != XmDRAG_TRACK_MOTION)) {
	    XmDragReceiverInfo	rInfo;
	    if ((rInfo = FindReceiverInfo(dc, event->xcrossing.subwindow))
		!= NULL) {
		if (rInfo == mb->currReceiverInfo)
		  mb->currReceiverInfo = dc->drag.rootReceiverInfo;
	    }
	}
	break;
      default:
	break;
    }
}
   

static void 
DragMotionProto(
        XmDragContext dc,
        Window root,
        Window subWindow )
{
  Boolean incrementTimestamp = False;

  /*
   * We've selected for motion on the root window. This allows us to
   * use the subwindow field to know whenever we have left or
   * entered a potential top-level window.
   */
  if ((root != dc->drag.currWmRoot) ||
      ((((dc->drag.trackingMode == XmDRAG_TRACK_MOTION) &&
	 (dc->drag.currReceiverInfo->frame != subWindow))) ||
       ((dc->drag.trackingMode == XmDRAG_TRACK_WM_QUERY) &&
	(dc->drag.currReceiverInfo->window != subWindow))))
    {
      if (dc->drag.currReceiverInfo->window)
	{
	  if ((dc->drag.activeProtocolStyle != XmDRAG_NONE) &&
	      (dc->drag.activeProtocolStyle != XmDRAG_DROP_ONLY))
	    {
	      /*
	       * Assumes the root window doesn't contain drop-sites !!!
	       * Send motion to old window.
	       *
	       * ** Old stuff for bootstrap
	       * ** if (dc->drag.currReceiverInfo->frame) 
	       * ** only send to non-initial windows 
	       */
	      /* 
	       * if the receiver is dynamic and we've been
	       * informed that we're in a drop-site then
	       * generate a dropsite leave to the initiator. If
	       * we haven't yet been informed of a drop-site
	       * enter (due to latency) but one is in the pipes,
	       * it will be ignored once it gets in based on the
	       * timestamp being stale.
	       *
	       * We'll make sure it's stale by incrementing the
	       * timestamp by one millisecond.  This is a no-no but
	       * it makes it easy to identify the echo events from
	       * the receiver.  Its also relatively safe until we
	       * get micro-second response times :-)
	       */
	      if ((dc->drag.activeProtocolStyle == XmDRAG_DYNAMIC) &&
		  (!dc->drag.currReceiverInfo->shell) &&
		  (dc->drag.inDropSite))
		{
		  GenerateClientCallback(dc, XmCR_DROP_SITE_LEAVE);
		  dc->drag.inDropSite = False;
		  incrementTimestamp = True;
		}
	      SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			      XmDRAG_MOTION);
	      SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			      XmTOP_LEVEL_LEAVE);
	    }
	  GenerateClientCallback(dc, XmCR_TOP_LEVEL_LEAVE);
	}
      
      if (root != dc->drag.currWmRoot)
	NewScreen(dc, root);
      
      GetDestinationInfo(dc, root, subWindow);
      
      /* we should special-case the root window */
      if (dc->drag.currReceiverInfo->window)
	{
	  if ((dc->drag.activeProtocolStyle != XmDRAG_NONE) &&
	      (dc->drag.activeProtocolStyle != XmDRAG_DROP_ONLY))
	    {
	      SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			      XmTOP_LEVEL_ENTER);
	    }
	  /* clear iccInfo for dsm's sanity */
	  dc->drag.currReceiverInfo->iccInfo = NULL;
	  GenerateClientCallback(dc, XmCR_TOP_LEVEL_ENTER);
	}
    }
  if (dc->drag.currReceiverInfo->window)
    {
      if ((dc->drag.activeProtocolStyle != XmDRAG_NONE) &&
	  (dc->drag.activeProtocolStyle != XmDRAG_DROP_ONLY))
	SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			XmDRAG_MOTION);
      else
	GenerateClientCallback(dc, XmCR_DRAG_MOTION);
    }
  else
    GenerateClientCallback(dc, XmCR_DRAG_MOTION);
  if (incrementTimestamp)
    dc->drag.lastChangeTime++;
}

static void 
ProcessMotionBuffer(
        XmDragContext dc,
        MotionBuffer mb )
{
    Cardinal	incr, i, j, max;
    Window	protoWindow = None;

    incr = (mb->count / MOTIONFILTER);
    if (incr == 0) incr = 1;
    max = mb->count / incr;
    j = (mb->count + incr - 1) % incr;
    for (i = 0; i < max; i++, j += incr) {
	dc->core.x = mb->entries[j].x;
	dc->core.y = mb->entries[j].y;

	if (mb->entries[j].state != dc->drag.lastEventState)
	  CheckModifiers(dc, mb->entries[j].state);
	if (dc->drag.currWmRoot != mb->entries[j].window) {
	    /*
	     * cause the callbacks to get called so the client can
	     * change the dragOver visuals
	     */
	    DragMotionProto(dc, mb->entries[j].window, None);
	    protoWindow = None;
	}
	else 
	  protoWindow = mb->entries[j].subwindow;

    }
    _XmDragOverMove((Widget)dc->drag.curDragOver, dc->core.x, dc->core.y);

    /* if the protoWindow gotten above is the drag window,
     * we can use XTranslateCoordinates to look through the
     * one pixel hole (which has just been moved to be under where
     * the pointer was), and determine what real window we are over
     */
    if (protoWindow == XtWindow(dc->drag.curDragOver))
      {
	Window currRoot = dc->drag.currWmRoot;
	int dummyx, dummyy;
	    
	(void) XTranslateCoordinates(XtDisplay(dc), currRoot, currRoot,
				     dc->core.x, dc->core.y,
				     &dummyx, &dummyy,
				     &protoWindow);
      }

    /*
     * actually inform the receiver/initiator that movement has
     * occurred
     */
    DragMotionProto(dc, dc->drag.currWmRoot, protoWindow);

    if (mb->count > STACKMOTIONBUFFERSIZE)
      XtFree( (char *)mb);
}

#define IsGrabEvent(ev) \
  ((ev->type == ButtonPress) ||\
   (ev->type == ButtonRelease) ||\
   (ev->type == KeyPress) ||\
   (ev->type == KeyRelease))

/* ARGSUSED */
static void 
DragMotion(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    XmDragContext	dc = (XmDragContext)w;
    MotionBufferRec	stackBuffer;
    MotionBuffer	motionBuffer = &stackBuffer;
    Boolean		grabEvent = False;

    stackBuffer.count = 0;
    stackBuffer.currReceiverInfo = dc->drag.currReceiverInfo;
    UpdateMotionBuffer(dc, motionBuffer,event);

    /* 
     * We need to process all outstanding events of interest
     * inline and flush out any stale motion events.  Need to
     * check for state change events like modifier or button press.
     */
    
    /*
     * get any that came in after
     */
    while (!grabEvent &&
	   XCheckMaskEvent(XtDisplayOfObject((Widget) w),
			   _XmDRAG_EVENT_MASK(dc),
			   event)) {
	grabEvent = IsGrabEvent(event);
	if (!grabEvent) {
	    if (dc->drag.trackingMode != XmDRAG_TRACK_MOTION)
	      event->xmotion.subwindow = event->xmotion.window;
	    UpdateMotionBuffer(dc, motionBuffer, event);
	}
	else
	  XPutBackEvent(XtDisplay(dc), event);
    }
    ProcessMotionBuffer(dc, motionBuffer);
    XFlush(XtDisplayOfObject((Widget) dc));
}

/* ARGSUSED */
static void 
DragKey(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
  char 		*direction;
  int 		dx, dy;
  XKeyEvent 	*keyevent = (XKeyEvent *) event;
  XEvent	motionEvent;
  XmDisplay     dd;
  unsigned int	state = 0;

  dd = (XmDisplay) XtParent(w);

  direction = params[0];

  if (event == NULL) return;

  if (strcmp(direction, "Up") == 0) 
    { dx = 0; dy = -1; }
  else if (strcmp(direction, "Down") == 0)
    { dx = 0; dy = 1; }
  else if (strcmp(direction, "Left") == 0)
    { dx = -1; dy = 0; }
  else if (strcmp(direction, "Right") == 0)
    { dx = 1; dy = 0; }
  else { /* Update on modifier key changes */
    dx = 0; dy = 0; 
    if (event -> type == KeyPress)
      state = keyevent -> state; 
  } 

  if (keyevent -> state & ControlMask) 
    { dx *= 16; dy *= 16; }

  if (dd -> display.enable_warp == False) {
    dx = 0; dy = 0; 
  } else {
    XWarpPointer(XtDisplay(w), None, None, 0, 0, 0, 0, dx, dy);
  }

  /* Send a button2 motion event so that the drag
     feedback will happen within DragMotion */
  motionEvent.xmotion.type = MotionNotify;
  motionEvent.xmotion.window = keyevent -> window;
  motionEvent.xmotion.subwindow = keyevent -> subwindow;
  motionEvent.xmotion.time = keyevent -> time;
  motionEvent.xmotion.root = keyevent -> root;
  motionEvent.xmotion.x = XtX(w) + dx;
  motionEvent.xmotion.y = XtY(w) + dy;
  motionEvent.xmotion.x_root = keyevent -> x_root;
  motionEvent.xmotion.y_root = keyevent -> y_root;
  motionEvent.xmotion.same_screen = keyevent -> same_screen;

  /* Or in the modifier bits if this is an update */
  motionEvent.xmotion.state = Button2Mask | state;
  motionEvent.xmotion.is_hint = 0;

  DragMotion(w, (XEvent *) &motionEvent, NULL, 0);
}

/*ARGSUSED*/
static void 
DropStartTimeout(
        XtPointer clientData,
        XtIntervalId *id )
{
    XmDragContext	dc = (XmDragContext)clientData;
    XmDropStartCallbackStruct	callbackRec, *callback = &callbackRec;
    XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay)(XtParent(dc)));

    if (dc->drag.dropStartCallback) {
	callback->reason = XmCR_DROP_START;
	callback->event = NULL;
	callback->timeStamp = dc->drag.dragFinishTime;
	callback->operation = dc->drag.operation;
	callback->operations = dc->drag.operations;
	callback->dropAction = XmDROP_CANCEL;
	callback->dropSiteStatus = dsm->dropManager.curDropSiteStatus;
	callback->x = dc->core.x;
	callback->y = dc->core.y;
	callback->iccHandle = dc->drag.iccHandle;
	callback->window = XtWindow(dc->drag.srcShell);
	XtCallCallbackList((Widget)dc, dc->drag.dropStartCallback, callback);
	dc->drag.dragCompletionStatus = callback->dropAction;
        dsm->dropManager.curDropSiteStatus = callback->dropSiteStatus;
    }
    dc->drag.dragDropCompletionStatus = XmDROP_FAILURE;
    dc->drag.dropFinishTime = dc->drag.dragFinishTime;

    DragDropFinish(dc);
}

/*ARGSUSED*/
static void
DropFinishTimeout(
        XtPointer clientData,
        XtIntervalId *id )
{
    XmDragContext	dc = (XmDragContext)clientData;

    dc->drag.dragDropCompletionStatus = XmDROP_FAILURE;
    dc->drag.dropFinishTime = dc->drag.dragFinishTime;

    DragDropFinish(dc);
}
 
/*ARGSUSED*/
static void 
FinishAction(
        XmDragContext dc,
        XEvent *ev )
{
    unsigned int	state = 0;
    Arg			args[4];
    Cardinal		i = 0;
    XmDisplay		dd = (XmDisplay) XmGetXmDisplay(XtDisplay(dc));

    dd->display.activeDC = NULL;
    dd->display.userGrabbed = False;

    if (ev) {
	switch(ev->type) {
	  case ButtonRelease:
	    state = ev->xbutton.state;
	    dc->drag.lastChangeTime = ev->xbutton.time;
	    dc->core.x = ev->xbutton.x_root;
	    dc->core.y = ev->xbutton.y_root;
	    break;
	  case KeyPress:
	    state = ev->xkey.state;
	    dc->drag.lastChangeTime = ev->xkey.time;
	    dc->core.x = ev->xkey.x_root;
	    dc->core.y = ev->xkey.y_root;
	    break;
	}
	
	/*
	 * start out with the default operations in effective operations
	 */
  	dc->drag.lastEventState = state;
 	CalculateDragOperation(dc);
    }
      
    /*
     * change the dragOver to a window in so it can persist after the
     * ungrab 
     */
    if (dc->drag.curDragOver) {
       unsigned char currentMode;
       unsigned char currentActiveMode;

       XtSetArg(args[0], XmNdragOverMode, &currentMode);
       XtSetArg(args[1], XmNdragOverActiveMode, &currentActiveMode);
       XtGetValues((Widget) dc->drag.curDragOver, args, 2);

       i = 0;
       XtSetArg(args[i], XmNhotX, dc->core.x); i++;
       XtSetArg(args[i], XmNhotY, dc->core.y); i++;
       if (currentActiveMode == XmCURSOR ||
	   (currentMode != XmDRAG_WINDOW && currentMode != XmWINDOW) ) {
	 XtSetArg(args[i], XmNdragOverMode, XmWINDOW); i++;
       }
       XtSetValues((Widget) dc->drag.curDragOver, args, i);

       XUngrabPointer(XtDisplayOfObject((Widget) dc), dc->drag.lastChangeTime);
       XtUngrabPointer((Widget) dc, dc->drag.dragFinishTime);
       XUngrabKeyboard(XtDisplayOfObject((Widget) dc), dc->drag.lastChangeTime);

	_XmRemoveGrab((Widget)dc);
    }

    if (dc->drag.serverGrabbed)
      XUngrabServer(XtDisplay((Widget) dc));
    /*
     * if we're in pre-register with a non-local raeceiver then we need
     * to flush a top-level leave to the local dsm. 
     */
    dc->drag.dragFinishTime = dc->drag.lastChangeTime;
	    
    if (dc->drag.inDropSite) {
	GenerateClientCallback(dc, XmCR_DROP_SITE_LEAVE);
	dc->drag.inDropSite = False;
    }

    if (dc->drag.currReceiverInfo) {
       if (dc->drag.currReceiverInfo->window) {
	  SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			  XmTOP_LEVEL_LEAVE);
	  GenerateClientCallback(dc, XmCR_TOP_LEVEL_LEAVE);

	  if ((dc->drag.activeProtocolStyle != XmDRAG_NONE) &&
	      ((dc->drag.dragCompletionStatus == XmDROP) ||
	       (dc->drag.dragCompletionStatus == XmDROP_HELP))) {
	    
	      XtAppContext appContext= XtWidgetToApplicationContext((Widget)dc);
	      /*
	       * we send the leave message in the dragDropFinish so
	       * that a non-local receiver can cleanup its dc
	       */
	      dc->drag.dragTimerId = 
	        XtAppAddTimeOut(appContext,
			        XtAppGetSelectionTimeout(appContext),
			        DropStartTimeout,
			        (XtPointer)dc);
	      SendDragMessage(dc, dc->drag.currReceiverInfo->window,
			      XmDROP_START);
	  }
	  else {
	      dc->drag.dragDropCompletionStatus = XmDROP_FAILURE;
	      dc->drag.dropFinishTime = dc->drag.dragFinishTime;
	      DropStartTimeout((XtPointer)dc, NULL);
	  }
	}
       dc->drag.currReceiverInfo->frame = 0;
     } else {
       /* Cleanup anyway */
       DropStartTimeout((XtPointer)dc, NULL);
     }
}



/* ARGSUSED */
static void 
CheckModifiers(
	       XmDragContext	dc,
	       unsigned int	state)
{
    unsigned char	oldOperation = dc->drag.operation;
    unsigned char	oldOperations = dc->drag.operations;

    dc->drag.lastEventState = state;
    CalculateDragOperation(dc);

    if ((oldOperations != dc->drag.operations) ||
	(oldOperation != dc->drag.operation)) {
	if ((dc->drag.currReceiverInfo->window) &&
	    (dc->drag.activeProtocolStyle != XmDRAG_NONE) &&
	    (dc->drag.activeProtocolStyle != XmDRAG_DROP_ONLY)) {
	    SendDragMessage(dc, 
			    dc->drag.currReceiverInfo->window,
			    XmOPERATION_CHANGED);
	}
	else {
	    GenerateClientCallback(dc, XmCR_OPERATION_CHANGED);
	}
    }
}

/* ARGSUSED */
static void 
IgnoreButtons(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    XmDragContext	dc = (XmDragContext)w;

    /*
     * the user has pressed some other buttons and caused the server
     * to synch up. Swallow and continue
     */

    XAllowEvents(XtDisplayOfObject((Widget) dc->drag.srcShell),
		 SyncPointer,
		 dc->drag.lastChangeTime);
}

/* ARGSUSED */
static void 
CancelDrag(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    XmDragContext	dc = (XmDragContext)w;

    /* 
     * only cancel if drag has not yet completed
     */
    if (dc->drag.dragFinishTime == 0) {
	dc->drag.dragCompletionStatus = XmDROP_CANCEL;
	FinishAction(dc, event);
    }
}

/* ARGSUSED */
static void 
HelpDrag(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    XmDragContext	dc = (XmDragContext)w;

    dc->drag.dragCompletionStatus = XmDROP_HELP;
    FinishAction(dc, event);
}


/* ARGSUSED */
static void 
FinishDrag(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *numParams )
{
    XmDragContext	dc = (XmDragContext)w;

    dc->drag.dragCompletionStatus = XmDROP;
    FinishAction(dc, event);
}

static void 
noMoreShell(
        Widget w,
 	XtPointer client,
 	XtPointer call)
{
    Boolean *contAction = (Boolean *)client;
    *contAction = False;
}


/*ARGSUSED*/
static void 
InitiatorMainLoop(
        XtPointer clientData,
        XtIntervalId *id )
{
    XmDragContext 	*activeDC = (XmDragContext *)clientData;
    XtAppContext	appContext;
    XEvent		event;
    Widget		focusWidget, shell;
    Boolean		contAction = True;

    if (*activeDC) {
	appContext = XtWidgetToApplicationContext((Widget) *activeDC);
        shell = (*activeDC)->drag.srcShell;
        focusWidget = XmGetFocusWidget((Widget)((*activeDC)->drag.srcShell));
        if (_XmGetFocusPolicy(shell) == XmEXPLICIT) {
	   XtSetKeyboardFocus(shell, None);
        } else {
           XmFocusData         focusData = _XmGetFocusData(shell);
           if(focusData)
             focusData->needToFlush = False;
	   /* CR 6384,  check for null focusWidget */
	   if (focusWidget != (Widget) NULL) {
	     if (XmIsPrimitive(focusWidget)) {
	       if (((XmPrimitiveWidgetClass) XtClass(focusWidget))
		   ->primitive_class.border_unhighlight)
		 (*(((XmPrimitiveWidgetClass) XtClass(focusWidget))
		    ->primitive_class.border_unhighlight))(focusWidget);
	     } else if (XmIsGadget(focusWidget)) {
	       if (((XmGadgetClass) XtClass(focusWidget))
		   ->gadget_class.border_unhighlight)
                 (*(((XmGadgetClass) XtClass(focusWidget))
		    ->gadget_class.border_unhighlight))(focusWidget);
	     }
	   }
        }
	DragStartProto(*activeDC);
    }
    else return;

    XtAddCallback (shell, XmNdestroyCallback, noMoreShell, 
		   (XtPointer)&contAction);
      
    while ( (*activeDC) && (XtAppGetExitFlag(appContext) == False) ) {
	XmDragContext dc = *activeDC;
#ifdef XTHREADS
	XtInputMask mask;

      while(!(mask = XtAppPending(appContext)))
	; /* busy wait */
      if (mask & XtIMXEvent)
      {
#endif
	XtAppNextEvent(appContext, &event);
	/*
	 * make sure evil Focus outs don't confuse Xt and cause the
	 * unhighlighting of the source hierarchy.
	 */
	switch(event.type) {
	  case FocusIn:
	  case FocusOut:
	    break;
	  case KeyPress:
	  case KeyRelease:
	  case ButtonPress:
	  case ButtonRelease:
	  case EnterNotify:
	  case LeaveNotify:
	  case MotionNotify:
	    {
	      /* dispatch it onto the dc */
		switch(dc->drag.trackingMode) {
		  case XmDRAG_TRACK_MOTION:
		    break;
#ifdef MULTI_SCREEN_DONE
		  case XmDRAG_TRACK_WM_QUERY:
		    event.xmotion.subwindow = event.xmotion.window;
		    break;
#endif /*  MULTI_SCREEN_DONE */
		  case XmDRAG_TRACK_WM_QUERY_PENDING:
		    event.xmotion.subwindow = event.xmotion.window;
		    break;
		} 
	    }
	    event.xmotion.window = XtWindow(dc);
	    break;
	}

        if ((event.type == MotionNotify ||
             event.type == LeaveNotify ||
             event.type == EnterNotify) &&
	     event.xmotion.state == dc->drag.lastEventState)
           DragMotion((Widget)dc, &event, NULL, 0);
        else
	   XtDispatchEvent(&event);
#ifdef XTHREADS
      }
      else
	XtAppProcessEvent(appContext, mask);
#endif
    }
    /* guard against the possibility that shell was destroyed in the last event
     * loop while the drag operation was going on (e.g. by a timer)
     */
    if (contAction) {
	XtRemoveCallback (shell, XmNdestroyCallback, noMoreShell,
			  (XtPointer)&contAction);
	if (_XmGetFocusPolicy(shell) == XmEXPLICIT) {
	    XtSetKeyboardFocus(shell, focusWidget);
	}
    }
}



Widget
XmDragStart(
        Widget w,
        XEvent *event,
        ArgList args,
        Cardinal numArgs )
{
    XmDragContext	dc;
    XmDisplay		dd = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    Arg			lclArgs[1];
    Arg			*mergedArgs;
    XmDragStartCallbackStruct	cb;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    if (dd->display.dragInitiatorProtocolStyle == XmDRAG_NONE) {
      _XmAppUnlock(app);
      return(NULL);
    }

    if (event->type != ButtonPress &&
	event->type != ButtonRelease &&
	event->type != KeyRelease &&
	event->type != KeyPress &&
	event->type != MotionNotify ) {
	XmeWarning(w, MESSAGE6);
	_XmAppUnlock(app);
	return NULL;
    }

    cb.reason = XmCR_DRAG_START;
    cb.event = event;
    cb.widget = w;
    cb.doit = True;
    XtCallCallbackList((Widget)dd, dd->display.dragStartCallback,
		       (XtPointer) &cb);

    if (!cb.doit) {
	_XmAppUnlock(app);
	return NULL;
    }

    /*
     * check if the menu system is already active
     */
    if (dd->display.userGrabbed) {
#ifdef DEBUG
	Warning("can't drag; menu system active\n");
#endif	
	_XmAppUnlock(app);
	return NULL;
    }

    XtSetArg(lclArgs[0], XmNsourceWidget, w);

    if (numArgs) {
	mergedArgs = XtMergeArgLists(args, numArgs, lclArgs, 1);
    }
    else {
	mergedArgs = lclArgs;
    }
    dc = (XmDragContext)
      XtCreateWidget("dragContext", xmDragContextClass,
		     (Widget) dd, mergedArgs, numArgs + 1);
    XtAddCallback(w, XmNdestroyCallback, cancelDrag, (XtPointer)dc);
    _XmDragStart(dc, w, event);

    if (numArgs)
      XtFree( (char *)mergedArgs);
    _XmAppUnlock(app);
    return (Widget)dc;
}


static void 
DragCancel(
        XmDragContext dc )
{
    dc->drag.dragCompletionStatus = XmDROP_CANCEL;
    FinishAction(dc, NULL);
}

void 
XmDragCancel(
        Widget dragContext )
{
    _XmWidgetToAppContext(dragContext);
    _XmAppLock(app);
    DragCancel((XmDragContext)dragContext);
    _XmAppUnlock(app);
}


/*ARGSUSED*/
Boolean 
XmTargetsAreCompatible(
        Display *dpy,
        Atom *exportTargets,
        Cardinal numExportTargets,
        Atom *importTargets,
        Cardinal numImportTargets )
{
    Cardinal		j, k;
    _XmDisplayToAppContext(dpy);

    _XmAppLock(app);
    for (j = 0; j < numExportTargets; j++)
      for (k = 0; k < numImportTargets; k++)
	if (exportTargets[j] == importTargets[k]) {
	  _XmAppUnlock(app);
	  return True;
	}
    _XmAppUnlock(app);
    return False;
}


unsigned char 
_XmGetActiveProtocolStyle(
        Widget w )
{
  unsigned char initiator = XmDRAG_NONE,
    receiver = XmDRAG_NONE,
    active = XmDRAG_NONE;
  XmDragContext	dc = (XmDragContext)w;
  XmDisplay		xmDisplay = (XmDisplay)XtParent(dc);

  initiator = xmDisplay->display.dragInitiatorProtocolStyle;
  receiver = xmDisplay->display.dragReceiverProtocolStyle;

  if (!dc->drag.sourceIsExternal)
    {
      /* We are the initiator.  Find the receiver. */
      if (dc->drag.currReceiverInfo)
	{
	  receiver = dc->drag.currReceiverInfo->dragProtocolStyle;
	}
      else
	{
	  /*
	   * This function is sometimes called before we have
	   * set up the receiver info struct on the dc.  In these
	   * cases, we are still in the initiating client, and so
	   * we will use the initiating client's receiver protocol
	   * style.  In order to do this we have to emulate the 
	   * protocol style decision made in GetDestinationInfo.
	   * But we can't, since we don't really know the shell
	   * widget--the particular shell widget may not have a dnd
	   * property attached to it.
	   *
	   * There are a number of different guesses that we could
	   * make.  None of them is substantially better than
	   * not guessing at all.  Not guessing only messes up if the
	   * shell really doesn't have any drop sites--we should be
	   * returning NONE, but we might return something else.
	   */
	  /*EMPTY*/
	}

      active = protocolMatrix[initiator][receiver];
    }
  else
    {
      /* We are the receiver, so we can't be preregister.  (Since
       * the receiver doesn't hear about the drag during the drag,
       * and so get ACTIVE protocol makes no sense.)
       */
      switch(receiver)
	{
	case XmDRAG_NONE:
	  active = XmDRAG_NONE;
	  break;
	case XmDRAG_DROP_ONLY:
	  /* this must be post drop emulation of drag */
	case XmDRAG_PREREGISTER:
	case XmDRAG_DYNAMIC:
	case XmDRAG_PREFER_DYNAMIC:
	case XmDRAG_PREFER_PREREGISTER:
	case XmDRAG_PREFER_RECEIVER:
	  active = XmDRAG_DYNAMIC;
	  break;
	}
    }
  return active;
}

static void
CalculateDragOperation(XmDragContext dc)
{
  /*
   * Re-set to the initial settings of operation and operations
   */
  dc->drag.operations = dc->drag.dragOperations;
  if ((dc->drag.lastEventState & ShiftMask) &&
      (dc->drag.lastEventState & ControlMask)) {
    dc->drag.operations = 
      dc->drag.operation =  (unsigned char)
	(XmDROP_LINK & dc->drag.dragOperations);
  }
  else if (dc->drag.lastEventState & ShiftMask) {
    dc->drag.operations =
      dc->drag.operation =  (unsigned char)
	(XmDROP_MOVE & dc->drag.dragOperations);
  }
  else if (dc->drag.lastEventState & ControlMask) {
    dc->drag.operations = 
      dc->drag.operation = (unsigned char)
	(XmDROP_COPY & dc->drag.dragOperations);
  }
  else if (XmDROP_MOVE &dc->drag.dragOperations)
    dc->drag.operation = (unsigned char)XmDROP_MOVE;
  else if (XmDROP_COPY &dc->drag.dragOperations)
    dc->drag.operation = (unsigned char)XmDROP_COPY;
  else if (XmDROP_LINK &dc->drag.dragOperations)
    dc->drag.operation = (unsigned char)XmDROP_LINK;
  else {
    dc->drag.operations = dc->drag.operation = 0;
  }
}
