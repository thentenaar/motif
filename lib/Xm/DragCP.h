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
#ifndef _XmDragCP_h
#define _XmDragCP_h

#include <Xm/XmP.h>
#include <Xm/DragC.h>

#include <X11/Shell.h>
#include <X11/ShellP.h>

#include <Xm/DragIcon.h>
#include <Xm/DragOverS.h>
#include <Xm/DropSMgrP.h>

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
 *
 * DragContext (RootWrapper) Widget Private Data
 *
 ***********************************************************************/

typedef void	(*XmDragStartProc)( XmDragContext, Widget, XEvent *);
typedef void 	(*XmDragCancelProc)( XmDragContext) ;


typedef struct {
    XmDragStartProc		start;
    XmDragCancelProc		cancel;
    XtPointer       		extension;
} XmDragContextClassPart;

typedef struct _XmDragContextClassRec {
    CoreClassPart	      	core_class;
    XmDragContextClassPart	drag_class;
} XmDragContextClassRec;

externalref XmDragContextClassRec xmDragContextClassRec;

#define XtDragByPoll 	0
#define XtDragByEvent	1

typedef struct {
    Window		frame;
    Window		window;
    Widget		shell;
    unsigned char	flags;
    unsigned char	dragProtocolStyle;
    int			xOrigin, yOrigin;
    Position	xLast, yLast;
    unsigned int	width, height;
    unsigned int	depth;
    XtPointer		iccInfo;
} XmDragReceiverInfoStruct, *XmDragReceiverInfo;


typedef union _XmConvertSelectionRec
  {
    XtConvertSelectionIncrProc sel_incr ;
    XtConvertSelectionProc     sel ;
  } XmConvertSelectionRec ;


typedef struct _XmDragContextPart{
    /****  resources ****/

    Atom			*exportTargets;
    Cardinal			numExportTargets;
    XmConvertSelectionRec	convertProc;
    XtPointer			clientData;
    XmDragIconObject		sourceCursorIcon;
    XmDragIconObject		stateCursorIcon;
    XmDragIconObject		operationCursorIcon;
    XmDragIconObject		sourcePixmapIcon;
    Pixel			cursorBackground;
    Pixel			cursorForeground;
    Pixel			validCursorForeground;
    Pixel			invalidCursorForeground;
    Pixel			noneCursorForeground;
    XtCallbackList		dragMotionCallback;
    XtCallbackList		operationChangedCallback;
    XtCallbackList		siteEnterCallback;
    XtCallbackList		siteLeaveCallback;
    XtCallbackList		topLevelEnterCallback;
    XtCallbackList		topLevelLeaveCallback;
    XtCallbackList		dropStartCallback;
    XtCallbackList		dropFinishCallback;
    XtCallbackList		dragDropFinishCallback;
    unsigned char		dragOperations;
    Boolean			incremental;
    unsigned char		blendModel;

    /* private resources */
    Window			srcWindow;
    Time			dragStartTime;
    Atom			iccHandle;
    Widget			sourceWidget;
    Boolean			sourceIsExternal;

    /**** instance data ****/
    Boolean			topWindowsFetched;
    unsigned char 		commType;
    unsigned char		animationType;

    unsigned char		operation;
    unsigned char		operations;
    unsigned int		lastEventState;
    unsigned char		dragCompletionStatus;
    unsigned char		dragDropCompletionStatus;
    Boolean			forceIPC;
    Boolean			serverGrabbed;
    Boolean			useLocal;
    Boolean			inDropSite;
    XtIntervalId 		dragTimerId;

    Time			roundOffTime;
    Time			lastChangeTime;
    Time			crossingTime;

    Time			dragFinishTime;
    Time			dropFinishTime;

    Atom			dropSelection;
    Widget			srcShell;
	Position		startX, startY;

    XmID			siteID;

    Screen			*currScreen;
    Window			currWmRoot;
    XmDragOverShellWidget	curDragOver;
    XmDragOverShellWidget	origDragOver;

    XmDragReceiverInfoStruct	*currReceiverInfo;
    XmDragReceiverInfoStruct	*rootReceiverInfo;
    XmDragReceiverInfoStruct	*receiverInfos;
    Cardinal			numReceiverInfos;
    Cardinal			maxReceiverInfos;

    unsigned char		trackingMode;
    unsigned char		activeProtocolStyle;
    unsigned char               activeBlendModel;
    Boolean			dragDropCancelEffect;
    long 			SaveEventMask; 		/* Save the current root eventMask so that D&D works for MWM */
} XmDragContextPart;


typedef  struct _XmDragContextRec{
    CorePart	 		core;
    XmDragContextPart		drag;
} XmDragContextRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDragCP_h */
