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
#ifndef _XmDragCI_h
#define _XmDragCI_h

#include <Xm/XmP.h>
#include <Xm/DragCP.h>
#include <Xm/DragIconP.h>
#include <Xm/DropSMgrP.h>
#include <Xm/DisplayP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _XmDragStart(dc, srcW, event) \
  ((*((XmDragContextClass)XtClass(dc))->drag_class.start) (dc, srcW, event))

#define _XmDragCancel(dc) \
  ((*((XmDragContextClass)XtClass(dc))->drag_class.cancel) (dc))

#define _XmDCtoDD(dc)	((XmDisplay)XtParent(dc))

#define _XmDRAG_MASK_BASE \
	(ButtonPressMask | ButtonReleaseMask | ButtonMotionMask)
#ifdef DRAG_USE_MOTION_HINTS
#define _XmDRAG_GRAB_MASK \
	(_XmDRAG_MASK_BASE PointerMotionHintMask)
#else
#define _XmDRAG_GRAB_MASK _XmDRAG_MASK_BASE
#endif /* _XmDRAG_USE_MOTION_HINTS */

#define _XmDRAG_EVENT_MASK(dc) \
  ((((XmDragContext)dc)->drag.trackingMode == XmDRAG_TRACK_WM_QUERY) \
   ? (_XmDRAG_GRAB_MASK | EnterWindowMask | LeaveWindowMask) \
   : (_XmDRAG_GRAB_MASK))

enum{	XmCR_DROP_SITE_TREE_ADD = _XmNUMBER_DND_CB_REASONS,
	XmCR_DROP_SITE_TREE_REMOVE
	} ;
/*
 *  values for dragTrackingMode 
 */
enum { 
  XmDRAG_TRACK_WM_QUERY, XmDRAG_TRACK_MOTION, XmDRAG_TRACK_WM_QUERY_PENDING
};


/* Strings to use for the intern atoms */
typedef String	XmCanonicalString;

#define XmMakeCanonicalString( s) \
	(XmCanonicalString) XrmQuarkToString(XrmStringToQuark(s))

#define _XmAllocAndCopy( data, len) \
	memcpy((XtPointer) XtMalloc(len), (XtPointer)(data), (len))


typedef struct _XmDragTopLevelClientDataStruct{
    Widget	destShell;
    Position	xOrigin, yOrigin;
	Dimension	width, height;
    XtPointer	iccInfo;
    Boolean	sourceIsExternal;
	Window	window;
	Widget	dragOver;
} XmDragTopLevelClientDataStruct, *XmDragTopLevelClientData;

typedef struct _XmDragMotionClientDataStruct{
    Window	window;
    Widget	dragOver;
} XmDragMotionClientDataStruct, *XmDragMotionClientData;


/*
 * dsm to dragDisplay comm
 */
/* Move to DropSMgrI.h */
typedef struct _XmDropSiteTreeAddCallbackStruct{
    int		    	reason;
    XEvent          	*event;
    Widget		rootShell;
    Cardinal		numDropSites;
    Cardinal		numArgsPerDSHint;
} XmDropSiteTreeAddCallbackStruct, *XmDropSiteTreeAddCallback;

/* Move to DropSMgrI.h */
typedef struct _XmDropSiteTreeRemoveCallbackStruct{
    int			reason;
    XEvent          	*event;
    Widget		rootShell;
} XmDropSiteTreeRemoveCallbackStruct, *XmDropSiteTreeRemoveCallback;

/* Move to DropSMgrI.h */
typedef struct _XmDropSiteTreeUpdateCallbackStruct{
    int			reason;
    XEvent          	*event;
    Widget		rootShell;
    Cardinal		numDropSites;
    Cardinal		numArgsPerDSHint;
} XmDropSiteTreeUpdateCallbackStruct, *XmDropSiteTreeUpdateCallback;

typedef struct _XmDropSiteEnterPendingCallbackStruct{
    int                 reason;
    XEvent              *event;
    Time                timeStamp;
    Boolean		enter_pending;
} XmDropSiteEnterPendingCallbackStruct, *XmDropSiteEnterPendingCallback;

/* Move to DropSMgrI.h */
typedef struct _XmAnimationData {
    Widget		dragOver;
    Window		window;
    Position		windowX, windowY;
    Screen		*screen;
    XmRegion		clipRegion;
    XmRegion		dropSiteRegion;
    XtPointer		saveAddr;
} XmAnimationDataRec, *XmAnimationData;


/********    Private Function Declarations    ********/

extern XmDragReceiverInfo _XmAllocReceiverInfo( 
                        XmDragContext dc) ;
extern unsigned char _XmGetActiveProtocolStyle( 
                        Widget w) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDragCI_h */
