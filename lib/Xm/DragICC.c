/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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
static char rcsid[] = "$TOG: DragICC.c /main/14 1997/06/18 17:38:07 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <X11/Xatom.h>
#include <Xm/DisplayP.h>
#include <Xm/DragC.h>
#include "DisplayI.h"
#include "DragBSI.h"
#include "DragICCI.h"
#include "MessagesI.h"
#include "RegionI.h"
#include "XmI.h"

#ifdef DEBUG
#include <stdio.h>		/* for printf() */
#endif /* DEBUG */

#define MESSAGE1	_XmMMsgDragICC_0000
#define MESSAGE2	_XmMMsgDragICC_0001

#define BUFFER_DATA	0
#define BUFFER_HEAP	1

#define NUM_HEADER_ARGS 0
#define MAXSTACK	1000

/*
 * We assume that there are only 64 possible messageTypes for each
 * clientMessage type that we use.  This allows us to only eat up a
 * single byte in the data area of the event.  The following tables
 * and routines are used to map back and forth between the client
 * message and the callback reasons
 */
typedef struct _ReasonTable {
    int			reason;
    unsigned char	messageType;
} ReasonTable;

/***************************************************************

For improving response time in a Drag operation, OSF/Motif allows clients
to cache frequently needed data on window properties to reduce roundtrip
X server requests.

The data is stored on window properties of the motifDragWindow, a
persistent, override-redirect, InputOnly child window of the display's
default root window.  A client looks for the motifDragWindow id on the
"_MOTIF_DRAG_WINDOW" property of the display's default root window.
If the window id is NULL or invalid, a client may create the motifDragWindow.
The motifDragWindow is mapped but is not visible on the screen.


Three sets of data are stored on motifDragWindow properties:

	1. a list of atom names/value pairs,
	2. an atom table, and
	3. a targets list table.

The "_MOTIF_DRAG_ATOM_PAIRS" property contains a list of atoms
name/value pairs from the "_MOTIF_DRAG_ATOM_PAIRS" property on
motifDragWindow. If the property does not exist on motifDragWindow, a
client may create the list of atom name/value pairs and store it on the
property. The list created by the OSF/Motif toolkit includes most of the
atoms used by Xm.

If a client finds the "_MOTIF_DRAG_ATOM_PAIRS" property, it may read
that property and cache the atoms, avoiding multiple roundtrip
server requests to intern the atoms itself.

The "_MOTIF_DRAG_ATOMS" property on motifDragWindow contains an atoms
table, consisting of pairs of atoms and timestamps.  The atoms have been
interned once and are available for clients to use without repeated
roundtrip server requests to intern them.  A timestamp of 0 indicates
that the atom is available.  A nonzero timestamp indicates when the
atom was last allocated to a client.  The OSF/Motif toolkit initializes
this property to contains only atom "_MOTIF_ATOM_0" with timestamp 0.

A client requests an atom by trying to find an available atom in the
table.  If it succeeds it sets the atom's timestamp to the value
specified and returns the atom.  If no atom is available, a client
may add an atom to the table with the specified timestamp, updates the
"_MOTIF_DRAG_ATOMS" property on motifDragWindow, and use the new
atom.  These new atoms are named "_MOTIF_ATOM_n" where n is 1, 2, 3, etc.

The "_MOTIF_DRAG_TARGETS" property on motifDragWindow contains a
targets table, consisting of a sequence of target lists to be shared
among clients.  In other words, this property is a list of lists.
These target lists are sorted into ascending order to avoid
permutations.  By sharing the targets table, clients may pass target
lists between themselves by exchanging only the corresponding target
list index in the protocol messages.

The targets table on the "_MOTIF_DRAG_TARGETS" property initially
contain only two lists:

 		{ 0,		}, and
 		{ XA_STRING,	}

To add a target list to the table, a client must first sorts the
target list into ascending order, then search the targets table for a
match. If it finds a match, it may use the index of the matching
targets table entry in the drag protocol.  Otherwise, it adds the
sorted target list to the table, updates the "_MOTIF_DRAG_TARGETS"
property on motifDragWindow, and uses the index of the new targets
table entry.


******************************************************************/

/********    Static Function Declarations    ********/

static XmICCEventType GetMessageData(
                        Display *display,
                        xmICCMessageStruct *xmessage,
                        XmICCCallbackStruct *callback) ;
static void SwapMessageData(
                        xmICCMessageStruct *xmessage) ;
#ifdef DEBUG
static void PrintMessage(char, xmICCMessageStruct *);
#endif /* DEBUG */

/********    End Static Function Declarations    ********/

externaldef(dragicc)
unsigned char _XmByteOrderChar = 0;

static XmConst ReasonTable	reasonTable[] = {
    {	XmCR_TOP_LEVEL_ENTER,		XmTOP_LEVEL_ENTER	},
    {	XmCR_TOP_LEVEL_LEAVE,		XmTOP_LEVEL_LEAVE	},
    {	XmCR_DRAG_MOTION,		XmDRAG_MOTION		},
    {	XmCR_DROP_SITE_ENTER,		XmDROP_SITE_ENTER	},
    {	XmCR_DROP_SITE_LEAVE,		XmDROP_SITE_LEAVE	},
    {	XmCR_DROP_START,		XmDROP_START		},
    {	XmCR_DROP_FINISH,		XmDROP_FINISH		},
    {	XmCR_DRAG_DROP_FINISH,		XmDRAG_DROP_FINISH	},
    {	XmCR_OPERATION_CHANGED,		XmOPERATION_CHANGED	},
};

static XmConst int	messageTable[] = {
    XmCR_TOP_LEVEL_ENTER,	/* XmTOP_LEVEL_ENTER 	0 */
    XmCR_TOP_LEVEL_LEAVE,	/* XmTOP_LEVEL_LEAVE 	1 */
    XmCR_DRAG_MOTION,		/* XmDRAG_MOTION	2 */
    XmCR_DROP_SITE_ENTER,	/* XmDROP_SITE_ENTER	3 */
    XmCR_DROP_SITE_LEAVE,	/* XmDROP_SITE_LEAVE	4 */
    XmCR_DROP_START,		/* XmDROP_START		5 */
    XmCR_DROP_FINISH,		/* XmDROP_FINISH	6 */
    XmCR_DRAG_DROP_FINISH,	/* XmDRAG_DROP_FINISH	7 */
    XmCR_OPERATION_CHANGED,	/* XmOPERATION_CHANGED	8 */
};

static unsigned char xdnd_version     = 5; /**< Xdnd protocol version */
static unsigned char xdnd_version_min = 3; /**< Xdnd minimum version  */

/************************************************************************
 *
 *  _XmReasonToMessageType()
 *
 ***********************************************************************/

unsigned char
_XmReasonToMessageType(
        int reason )
{
    int	i;

    for (i = 0; i < XtNumber(reasonTable); i++)
      if (reasonTable[i].reason == reason)
	return((unsigned char)i);
    return 0xFF;
}

/************************************************************************
 *
 *  _XmMessageTypeToReason()
 *
 ***********************************************************************/

unsigned int
_XmMessageTypeToReason(
        unsigned char messageType )
{
    return(messageTable[messageType]);
}

/************************************************************************
 *
 *  _XmICCCallbackToICCEvent()
 *
 *  Reformat a motif callback into a client message
 ***********************************************************************/
void
_XmICCCallbackToICCEvent(
        Display *display,
        Window window,
        XmICCCallback callback,
        XClientMessageEvent *cmev,
	XmICCEventType type )
{
    xmICCMessageStruct	*xmessage = (xmICCMessage)&cmev->data.b[0];

    cmev->display = display;
    cmev->type = ClientMessage;
    cmev->serial = LastKnownRequestProcessed(display);
    cmev->send_event = True;
    cmev->window = window;
    cmev->format = 8;
    cmev->message_type = XInternAtom(display,
		_Xm_MOTIF_DRAG_AND_DROP_MESSAGE, False);


    xmessage->any.byte_order = (BYTE) _XmByteOrderChar;
    xmessage->any.message_type = (BYTE)
      _XmReasonToMessageType(callback->any.reason);

    switch (callback->any.reason) {
      case XmCR_TOP_LEVEL_ENTER:
	/*
	 * this message goes to receiver
	 */
	{
	    XmTopLevelEnterCallback cb =
	      (XmTopLevelEnterCallback)callback;

	    xmessage->topLevelEnter.flags = 0;
	    xmessage->topLevelEnter.time  = cb->timeStamp;
	    xmessage->topLevelEnter.src_window = cb->window;
	    xmessage->topLevelEnter.icc_handle = cb->iccHandle;
	    /* load has_drop_sites */
	}
	break;
      case XmCR_TOP_LEVEL_LEAVE:
	/*
	 * this message goes to receiver
	 */
	{
	    XmTopLevelLeaveCallback cb =
	      (XmTopLevelLeaveCallback)callback;

	    xmessage->topLevelLeave.flags = 0;
	    xmessage->topLevelLeave.time  = cb->timeStamp;
	    xmessage->topLevelLeave.src_window = cb->window;
	}
	break;
      case XmCR_DRAG_MOTION:
	/*
	 * this message goes both ways
	 */
	{
	    XmDragMotionCallback	cb =
	      (XmDragMotionCallback)callback;

	    xmessage->dragMotion.flags = 0;
	    xmessage->dragMotion.flags |= PUT_SITE_STATUS(cb->dropSiteStatus);
	    xmessage->dragMotion.flags |= PUT_OPERATION(cb->operation);
	    xmessage->dragMotion.flags |= PUT_MULTIOPS(cb->operations);
	    xmessage->dragMotion.time   = cb->timeStamp;
	    xmessage->dragMotion.x	= cb->x;
	    xmessage->dragMotion.y	= cb->y;
	}
	break;
      case XmCR_OPERATION_CHANGED:
	/*
	 * this message goes both ways
	 */
	{
	    XmOperationChangedCallback	cb =
	      (XmOperationChangedCallback)callback;

	    xmessage->operationChanged.flags = 0;
	    xmessage->operationChanged.flags |=
			PUT_OPERATION(cb->operation);
	    xmessage->operationChanged.flags |=
			PUT_SITE_STATUS(cb->dropSiteStatus);
	    xmessage->operationChanged.flags |=
			PUT_MULTIOPS(cb->operations);
	    xmessage->operationChanged.time   = cb->timeStamp;
	}
	break;
      case XmCR_DROP_SITE_ENTER:
	/*
	 * this message goes to initiator
	 */
	{
	    XmDropSiteEnterCallback	cb =
	      (XmDropSiteEnterCallback)callback;

	    /* invalid flags stuff ||| */
	    xmessage->dropSiteEnter.flags = 0;
	    xmessage->dropSiteEnter.flags |= PUT_OPERATION(cb->operation);
	    xmessage->dropSiteEnter.flags |= PUT_MULTIOPS(cb->operations);
	    xmessage->dropSiteEnter.flags |=
		PUT_SITE_STATUS(cb->dropSiteStatus);
	    xmessage->dropSiteEnter.time  = cb->timeStamp;
	    xmessage->dropSiteEnter.x = cb->x;
	    xmessage->dropSiteEnter.y = cb->y;
	}
	break;
      case XmCR_DROP_SITE_LEAVE:
	/*
	 * this message goes to initiator
	 */
	{
	    XmDropSiteLeaveCallback	cb =
	      (XmDropSiteLeaveCallback)callback;

	    /* invalid flags stuff ||| */
	    xmessage->dropSiteLeave.flags = 0;
	    xmessage->dropSiteLeave.time  = cb->timeStamp;
	}
	break;
      case XmCR_DROP_START:
	/*
	 * this message goes to receiver
	 */
	{
	    XmDropStartCallback	cb =
	      (XmDropStartCallback)callback;

	    xmessage->drop.flags = 0;
	    xmessage->drop.flags |= PUT_SITE_STATUS(cb->dropSiteStatus);
	    xmessage->drop.flags |= PUT_COMPLETION(cb->dropAction);
	    xmessage->drop.flags |= PUT_OPERATION(cb->operation);
	    xmessage->drop.flags |= PUT_MULTIOPS(cb->operations);
	    xmessage->drop.time  = cb->timeStamp;
	    xmessage->drop.x     = cb->x;
	    xmessage->drop.y     = cb->y;
	    xmessage->drop.icc_handle = cb->iccHandle;
	    xmessage->drop.src_window = cb->window;
	}
	break;
      default:
	break;
    }

#ifdef DEBUG
    /* Print message */
    PrintMessage('S', xmessage);
#endif /* DEBUG */

    xmessage->any.message_type |= PUT_ICC_EVENT_TYPE(type);
}

static void _XmICCCallbackToXdndEvent(Display *display, Window window,
                                      Window src_window,
                                      XmICCCallback callback,
                                      XClientMessageEvent *ev,
                                      XmICCEventType type)
{
	Atom xdndEnter, xdndPosition, xdndStatus, xdndLeave, xdndDrop;
	Atom xdndFinished, xdndMove, xdndCopy, xdndLink;
	XmDragMotionCallback motion = (XmDragMotionCallback)callback;
	XmDropStartCallback  drop   = (XmDropStartCallback)callback;
	XmDropFinishCallback finish = (XmDropFinishCallback)callback;

	xdndEnter    = XInternAtom(display, "XdndEnter",      False);
	xdndLeave    = XInternAtom(display, "XdndLeave",      False);
	xdndPosition = XInternAtom(display, "XdndPosition",   False);
	xdndStatus   = XInternAtom(display, "XdndStatus",     False);
	xdndDrop     = XInternAtom(display, "XdndDrop",       False);
	xdndFinished = XInternAtom(display, "XdndFinished",   False);
	xdndMove     = XInternAtom(display, "XdndActionMove", False);
	xdndCopy     = XInternAtom(display, "XdndActionCopy", False);
	xdndLink     = XInternAtom(display, "XdndActionLink", False);

	ev->format     = 32;
	ev->display    = display;
	ev->type       = ClientMessage;
	ev->serial     = LastKnownRequestProcessed(display);
	ev->send_event = True;
	ev->window     = window;

	memset(&ev->data.b, 0, sizeof ev->data.b);
	if (callback->any.reason != XmCR_TOP_LEVEL_ENTER) {
		switch (motion->operation) {
		case XmDROP_MOVE: ev->data.l[4] = xdndMove; break;
		case XmDROP_COPY: ev->data.l[4] = xdndCopy; break;
		case XmDROP_LINK: ev->data.l[4] = xdndLink; break;
		}
	}

	/* The receiver receives XdndPosition and sends XdndStatus */
	if (type == XmICC_RECEIVER_EVENT) {
		if (callback->any.reason == XmCR_DRAG_MOTION)
			callback->any.reason = XmCR_OPERATION_CHANGED;
		if (callback->any.reason == XmCR_DROP_START)
			return;
	}

	switch (callback->any.reason) {
	case XmCR_TOP_LEVEL_ENTER:
		/**
		 * We set bit 0 in l[1] to signify that the other end
		 * should query the XdndTypesList and not rely on the
		 * 'quick' three Atoms method of conveyance.
		 */
#ifdef DEBUG_XDND
		printf("-> xdndEnter from:0x%lx to:0x%lx\n", src_window, window);
#endif
		ev->message_type = xdndEnter;
		ev->data.l[0]    = src_window;
		ev->data.l[1]    = (xdnd_version_min << 24) | 1;
		ev->data.l[2]    = None;
		ev->data.l[3]    = None;
		ev->data.l[4]    = None;
		break;
	case XmCR_TOP_LEVEL_LEAVE:
#ifdef DEBUG_XDND
		printf("-> xdndLeave from:0x%lx to:0x%lx\n",  src_window, window);
#endif
		ev->message_type = xdndLeave;
		ev->data.l[0]    = src_window;
		break;
	case XmCR_DRAG_MOTION:
#ifdef DEBUG_XDND
		printf("-> xdndPosition 0x%lx\n", src_window);
#endif
		ev->message_type = xdndPosition;
		ev->data.l[0]    = src_window;
		ev->data.l[2]    = ((motion->x & 0xffff) << 16) | (motion->y & 0xffff);
		ev->data.l[3]    = motion->timeStamp;
		break;
	case XmCR_DROP_SITE_ENTER:
	case XmCR_OPERATION_CHANGED:
#ifdef DEBUG_XDND
		printf("-> xdndStatus (ds: %d)\n", motion->dropSiteStatus == XmVALID_DROP_SITE);
#endif
		ev->message_type  = xdndStatus;
		ev->data.l[0]     = src_window;
		ev->data.l[1]     = 2 | (motion->dropSiteStatus == XmVALID_DROP_SITE);
		break;
	case XmCR_DROP_SITE_LEAVE:
#ifdef DEBUG_XDND
		puts("-> xdndStatus (leave)");
#endif
		ev->message_type  = xdndStatus;
		ev->data.l[0]     = src_window;
		ev->data.l[1]     = 2;
		break;
	case XmCR_DROP_START:
#ifdef DEBUG_XDND
		puts("-> xdndDrop");
#endif
		ev->message_type = xdndDrop;
		ev->data.l[0]    = src_window;
		ev->data.l[2]    = drop->timeStamp;
		break;
	case XmCR_DROP_FINISH:
	case XmCR_DRAG_DROP_FINISH:
#ifdef DEBUG_XDND
		printf("-> xdndFinished (%d)\n", finish->dropAction != XmDROP_FAILURE);
#endif
		ev->message_type = xdndFinished;
		ev->data.l[0]    = src_window;
		ev->data.l[1]    = finish->dropAction != XmDROP_FAILURE;
		ev->data.l[2]    = ev->data.l[4];
		break;
	}
}

/************************************************************************
 *
 *  _XmSendICCCallback()
 *
 ***********************************************************************/
void _XmSendICCCallback(Display *display, Window window, Window src_window,
                        int protocol_style, XmICCCallback callback,
                        XmICCEventType type)
{
	XClientMessageEvent msgEvent;
	XmDisplay dd = (XmDisplay)XmGetXmDisplay(display);
	Window receiverWindow = dd->display.proxyWindow;

	if (protocol_style == XmDRAG_XDND)
		_XmICCCallbackToXdndEvent(display, window, src_window, callback, &msgEvent, type);
	else if (callback->any.reason == XmCR_DROP_FINISH)
		return;
	else _XmICCCallbackToICCEvent(display, window, callback, &msgEvent, type);

	/* Fix for 8746 */
	if (receiverWindow == None || (protocol_style != XmDRAG_XDND && type == XmICC_RECEIVER_EVENT)) {
		/* always ACK to the src win */
		receiverWindow = window;
	}

	XSendEvent(display, receiverWindow, False, 0, (XEvent *)&msgEvent);
}

/**
 * Translate inbound Xdnd events to our ICC callbacks
 *
 * The Ask and Private actions are not supported.
 */
static Boolean _XdndToMotifEvent(XClientMessageEvent *msgEv,
                                 XmICCCallbackStruct *callback)
{
	XmTopLevelEnterCallback te = (XmTopLevelEnterCallback)callback;
	XmTopLevelLeaveCallback tl = (XmTopLevelLeaveCallback)callback;
	XmDragMotionCallback    dm = (XmDragMotionCallback)callback;
	XmDropSiteEnterCallback de = (XmDropSiteEnterCallback)callback;
	XmDropStartCallback     ds = (XmDropStartCallback)callback;
	XmDropFinishCallback    df = (XmDropFinishCallback)callback;

	unsigned long op = XmDROP_NOOP;
	Atom xdndEnter, xdndPosition, xdndStatus, xdndLeave, xdndDrop;
	Atom xdndFinished, xdndSelection, xdndMove, xdndCopy, xdndLink;
	unsigned long n_targets = 0;

	if (msgEv->format != 32)
		return False;

	xdndEnter     = XInternAtom(msgEv->display, "XdndEnter",      False);
	xdndLeave     = XInternAtom(msgEv->display, "XdndLeave",      False);
	xdndPosition  = XInternAtom(msgEv->display, "XdndPosition",   False);
	xdndStatus    = XInternAtom(msgEv->display, "XdndStatus",     False);
	xdndDrop      = XInternAtom(msgEv->display, "XdndDrop",       False);
	xdndFinished  = XInternAtom(msgEv->display, "XdndFinished",   False);
	xdndSelection = XInternAtom(msgEv->display, "XdndSelection",  False);
	xdndMove      = XInternAtom(msgEv->display, "XdndActionMove", False);
	xdndCopy      = XInternAtom(msgEv->display, "XdndActionCopy", False);
	xdndLink      = XInternAtom(msgEv->display, "XdndActionLink", False);

	if ((Atom)msgEv->data.l[4] == xdndMove) op = XmDROP_MOVE;
	if ((Atom)msgEv->data.l[4] == xdndCopy) op = XmDROP_COPY;
	if ((Atom)msgEv->data.l[4] == xdndLink) op = XmDROP_LINK;

	/**
	 * NB: We can't rely on the timestamp in the message data
	 * as it may get sign extended somewhere before Xt calls
	 * ReceiverShellExternalSourceHandler(), even if it's
	 * correct when the other side puts it on the wire.
	 */
	callback->any.event     = NULL;
	callback->any.timeStamp = XtLastTimestampProcessed(msgEv->display);

	if (msgEv->message_type == xdndEnter) {
#ifdef DEBUG_XDND
		puts("<- xdndEnter");
#endif
		te->reason            = XmCR_TOP_LEVEL_ENTER;
		te->window            = msgEv->data.l[0];
		te->iccHandle         = xdndSelection;
		te->dragProtocolStyle = XmDRAG_XDND;

		if (!(msgEv->data.l[1] & 1)) {
			te->targets[0] = msgEv->data.l[2];
			te->targets[1] = msgEv->data.l[3];
			te->targets[2] = msgEv->data.l[4];
			te->n_targets = (te->targets[0] != None) +
			                (te->targets[1] != None) +
			                (te->targets[2] != None);
		}
	} else if (msgEv->message_type == xdndLeave) {
#ifdef DEBUG_XDND
		puts("<- xdndLeave");
#endif
		tl->reason = XmCR_TOP_LEVEL_LEAVE;
		tl->window = msgEv->data.l[0];
	} else if (msgEv->message_type == xdndPosition) {
#ifdef DEBUG_XDND
		puts("<- xdndPosition");
#endif
		dm->reason         = XmCR_DRAG_MOTION;
		dm->x              = (Position)(msgEv->data.l[2] >> 16);
		dm->y              = (Position)(msgEv->data.l[2] & 0xffff);
		dm->operation      = op;
		dm->operations     = XmDROP_COPY | XmDROP_MOVE | XmDROP_LINK;
		dm->dropSiteStatus = op == XmDROP_NOOP ? XmINVALID_DROP_SITE : XmVALID_DROP_SITE;
	} else if (msgEv->message_type == xdndStatus) {
#ifdef DEBUG_XDND
		printf("<- xdndStatus (ds? %ld op=%lu)\n", msgEv->data.l[1] & 1, op);
#endif
		de->reason         = XmCR_DROP_SITE_ENTER;
		de->x              = -1;
		de->y              = -1;
		de->operation      = op;
		de->operations     = XmDROP_COPY | XmDROP_MOVE | XmDROP_LINK;
		de->dropSiteStatus = (op != XmDROP_NOOP && (msgEv->data.l[1] & 1))
		                     ? XmVALID_DROP_SITE : XmINVALID_DROP_SITE;
	} else if (msgEv->message_type == xdndDrop) {
#ifdef DEBUG_XDND
		puts("<- xdndDrop");
#endif
		ds->reason            = XmCR_DROP_START;
		ds->window            = msgEv->data.l[0];
		ds->iccHandle         = xdndSelection;
		ds->operation         = op;
		ds->operations        = XmDROP_COPY | XmDROP_MOVE | XmDROP_LINK;
		ds->dropSiteStatus    = XmVALID_DROP_SITE;
		ds->x                 = -1;
		ds->y                 = -1;
	} else if (msgEv->message_type == xdndFinished) {
#ifdef DEBUG_XDND
		puts("<- xdndFinished");
#endif
		df->reason           = XmCR_DROP_FINISH;
		df->completionStatus = XmDROP_SUCCESS;
	} else return False;

	return True;
}

/************************************************************************
 *
 *  GetMessageData()
 *
 *  Message data has already been byte-swapped, if necessary.
 ***********************************************************************/
static XmICCEventType
GetMessageData(
        Display *display,
        xmICCMessageStruct *xmessage,
        XmICCCallbackStruct *callback )
{
    XmICCEventType type;
	unsigned char message_type;

	message_type = xmessage->any.message_type;
    type = GET_ICC_EVENT_TYPE(message_type);
	message_type &= CLEAR_ICC_EVENT_TYPE;

    /* Load up the callback */
    callback->any.reason = (int)
      _XmMessageTypeToReason(message_type);
    callback->any.event = NULL;
    callback->any.timeStamp = (Time) xmessage->any.time;

    switch(message_type) {
      case XmTOP_LEVEL_ENTER:
	{
	    XmTopLevelEnterCallback	cb =
	      (XmTopLevelEnterCallback)callback;

	    cb->window = (Window) xmessage->topLevelEnter.src_window;
	    cb->iccHandle = (Atom) xmessage->topLevelEnter.icc_handle;
	}
	break;

      case XmTOP_LEVEL_LEAVE:
	{
	    XmTopLevelLeaveCallback	cb =
	      (XmTopLevelLeaveCallback)callback;

	    cb->window = (Window) xmessage->topLevelLeave.src_window;
	}
	break;

      case XmDRAG_MOTION:
	{
	    XmDragMotionCallback	cb =
	      (XmDragMotionCallback)callback;

	    cb->x = (Position) cvtINT16toShort(xmessage->dragMotion.x);
	    cb->y = (Position) cvtINT16toShort(xmessage->dragMotion.y);
	    cb->operation = (unsigned char)
		GET_OPERATION(xmessage->dragMotion.flags);
	    cb->operations = (unsigned char)
		GET_MULTIOPS(xmessage->dragMotion.flags);
	    cb->dropSiteStatus = (unsigned char)
		GET_SITE_STATUS(xmessage->dragMotion.flags);
	}
	break;
      case XmOPERATION_CHANGED:
	{
	    XmOperationChangedCallback	cb =
	      (XmOperationChangedCallback)callback;

	    cb->operation = (unsigned char)
		GET_OPERATION(xmessage->dragMotion.flags);
	    cb->operations = (unsigned char)
		GET_MULTIOPS(xmessage->dragMotion.flags);
	    cb->dropSiteStatus = (unsigned char)
	        GET_SITE_STATUS(xmessage->dragMotion.flags);
	}
	break;

      case XmCR_DROP_SITE_ENTER:
	/*
	 * this message goes to initiator
	 */
	{
	    XmDropSiteEnterCallback	cb =
	      (XmDropSiteEnterCallback)callback;

	    cb->x = (Position) cvtINT16toShort(xmessage->dropSiteEnter.x);
	    cb->y = (Position) cvtINT16toShort(xmessage->dropSiteEnter.y);

	    cb->operation = (unsigned char)
		GET_OPERATION(xmessage->dropSiteEnter.flags);
	    cb->operations = (unsigned char)
		GET_MULTIOPS(xmessage->dropSiteEnter.flags);
	    cb->dropSiteStatus = (unsigned char)
		GET_SITE_STATUS(xmessage->dropSiteEnter.flags);
	}
	break;

      case XmCR_DROP_SITE_LEAVE:
	/*
	 * this message goes to initiator
	 */
	break;

      case XmCR_DROP_START:
	/*
	 * this message goes to receiver
	 */
	{
	    XmDropStartCallback	cb =
	      (XmDropStartCallback)callback;

	    cb->operation = (unsigned char)
		GET_OPERATION(xmessage->drop.flags);
	    cb->operations = (unsigned char)
		GET_MULTIOPS(xmessage->drop.flags);
	    cb->dropAction = (unsigned char)
		GET_COMPLETION(xmessage->drop.flags);
	    cb->dropSiteStatus = (unsigned char)
	        GET_SITE_STATUS(xmessage->drop.flags);
	    cb->x = (Position) cvtINT16toShort(xmessage->drop.x);
	    cb->y = (Position) cvtINT16toShort(xmessage->drop.y);
	    cb->iccHandle = (Atom) xmessage->drop.icc_handle;
	    cb->window = (Window) xmessage->drop.src_window;
	}
	break;
      case XmCR_DROP_FINISH:
      case XmCR_DRAG_DROP_FINISH:
	/*
	 * this message goes to initiator
	 */
	break;

      default:
	XmeWarning ((Widget) XmGetXmDisplay (display), MESSAGE1);
	break;
    }

    return(type);
}

/************************************************************************
 *
 *  SwapMessageData()
 *
 ***********************************************************************/

static void
SwapMessageData(
        xmICCMessageStruct *xmessage )
{
    swap2bytes(xmessage->any.flags);
    swap4bytes(xmessage->any.time);

    switch(xmessage->any.message_type) {
      case XmTOP_LEVEL_ENTER:
	{
	    swap4bytes(xmessage->topLevelEnter.src_window);
	    swap4bytes(xmessage->topLevelEnter.icc_handle);
	}
	break;

      case XmTOP_LEVEL_LEAVE:
	{
	    swap4bytes(xmessage->topLevelLeave.src_window);
	}
	break;

      case XmDRAG_MOTION:
	{
	    swap2bytes(xmessage->dragMotion.x);
	    swap2bytes(xmessage->dragMotion.y);
	}
	break;

      case XmDROP_SITE_ENTER:
	{
	    swap2bytes(xmessage->dropSiteEnter.x);
	    swap2bytes(xmessage->dropSiteEnter.y);
	}
	break;

      case XmDROP_START:
	{
	    swap2bytes(xmessage->drop.x);
	    swap2bytes(xmessage->drop.y);
	    swap4bytes(xmessage->drop.icc_handle);
	    swap4bytes(xmessage->drop.src_window);
	}
	break;

      case XmDROP_SITE_LEAVE:
      default:
	break;
    }
}

/************************************************************************
 *
 *  _XmICCEventToICCCallback()
 *
 ***********************************************************************/
Boolean _XmICCEventToICCCallback(XClientMessageEvent *msgEv,
                                 XmICCCallback callback,
                                 XmICCEventType type)
{
	Atom motif_dnd_message_atom;
	xmICCMessage xmessage;

	if (msgEv->type != ClientMessage)
		return False;

	xmessage = (xmICCMessage)msgEv->data.b;
	if (msgEv->format == 8) {
		motif_dnd_message_atom = XInternAtom(msgEv->display,
		                                     _Xm_MOTIF_DRAG_AND_DROP_MESSAGE, False);
		if (msgEv->message_type != motif_dnd_message_atom)
			return False;

		if (xmessage->any.byte_order != _XmByteOrderChar) {
			/*
			 * swap it inplace and update the byte_order field so no one
			 * else will try to reswap it.  This could happen since we're
			 * probably being called out of an event Handler on a list
			 */
			SwapMessageData(xmessage);
			xmessage->any.byte_order = _XmByteOrderChar;
		}
#ifdef DEBUG
		/* Print data */
		PrintMessage('R', xmessage);
#endif /* DEBUG */

		return (type == GetMessageData(msgEv->display, xmessage, callback))
		       ? True : False;
	}

	return _XdndToMotifEvent(msgEv, callback);
}

/************************************************************************
 *
 *  _XmReadDragBuffer()
 *
 ***********************************************************************/

CARD16
_XmReadDragBuffer(
        xmPropertyBuffer propBuf,
        BYTE which,
        BYTE *ptr,
        CARD32 size )
{
    xmByteBufRec  *buf;
    CARD32	numCurr;

    if (which == BUFFER_DATA) /* data */
      buf = &propBuf->data;
    else
      buf = &propBuf->heap;

    numCurr = buf->curr - buf->bytes;
    if (numCurr + size > buf->size) {
	size = buf->size - numCurr;
    }
    memcpy(ptr, buf->curr, (size_t)size);
    buf->curr += size;
    return size;
}

/************************************************************************
 *
 *  _XmWriteDragBuffer()
 *
 ***********************************************************************/

CARD16
_XmWriteDragBuffer(
        xmPropertyBuffer propBuf,
        BYTE which,
        BYTE *ptr,
        CARD32 size )
{
    CARD16	returnVal;
    xmByteBufRec  *buf;

    if (which == BUFFER_DATA) /* data */
      buf = &propBuf->data;
    else
      buf = &propBuf->heap;

    if (buf->size + size > buf->max) {
       buf->max += 1000;
       if (buf->bytes == buf->stack) {
	  buf->bytes = (BYTE*)XtMalloc(buf->max);
	  memcpy(buf->bytes, buf->stack, buf->size);
       }
       else {
	  buf->bytes = (BYTE*)XtRealloc((char *)buf->bytes, (Cardinal)buf->max);
       }
    }
    memcpy(buf->bytes + buf->size, ptr, (size_t)size);
    returnVal = buf->size;
    buf->size += (size_t) size;
    return returnVal;
}

/************************************************************************
 *
 *  _XmWriteInitiatorInfo()
 *
 ***********************************************************************/
void _XmWriteInitiatorInfo(Widget dc)
{
    xmDragInitiatorInfoStruct	infoRec;
    Atom		initiatorAtom, iccHandle, xdndTypeList;
    Window		srcWindow;
    Arg			args[4];
    XmDisplay		xmDisplay = (XmDisplay)XtParent(dc);
    Atom		*exportTargets;
    Cardinal		numExportTargets;

    XtSetArg(args[0], XmNexportTargets, &exportTargets);
    XtSetArg(args[1], XmNnumExportTargets, &numExportTargets);
    XtSetArg(args[2], XmNsourceWindow, &srcWindow);
    XtSetArg(args[3], XmNiccHandle, &iccHandle);
    XtGetValues(dc, args, 4);

    infoRec.byte_order = _XmByteOrderChar;
    infoRec.protocol_version = _MOTIF_DRAG_PROTOCOL_VERSION;
    infoRec.targets_index =
      _XmTargetsToIndex((Widget)xmDisplay, exportTargets, numExportTargets);
    infoRec.icc_handle = iccHandle;

    initiatorAtom = XInternAtom(XtDisplayOfObject(dc),
				 XmI_MOTIF_DRAG_INITIATOR_INFO,
				 False);

    /* write the buffer to the property */
    XChangeProperty (XtDisplayOfObject(dc),
		     srcWindow,
		     iccHandle, initiatorAtom,
		     8, PropModeReplace,
		     (unsigned char *)&infoRec,
		     sizeof(xmDragInitiatorInfoStruct));

    xdndTypeList = XInternAtom(XtDisplayOfObject(dc), "XdndTypeList", False);
    XChangeProperty(XtDisplayOfObject(dc), srcWindow, xdndTypeList, XA_ATOM,
                    32, PropModeReplace, (unsigned char *)exportTargets,
                    numExportTargets);
}

/************************************************************************
 *
 *  _XmReadInitiatorInfo()
 *
 *  We assume that the dc has been initialized enough to have it's
 *  source window field set.
 ***********************************************************************/
void _XmReadInitiatorInfo(Widget dc)
{
    xmDragInitiatorInfoStruct *info = NULL;
    int format, set_exports = 0;
    unsigned long bytesafter, lengthRtn;
    long length;
    Arg args[3];
    Window srcWindow;
    Cardinal numExportTargets = 0;
    Atom initiatorAtom, type, iccHandle, xdndTypeList, *exportTargets = NULL;
    unsigned char *data = NULL;

    XtSetArg(args[0], XmNsourceWindow, &srcWindow);
    XtSetArg(args[1], XmNiccHandle, &iccHandle);
    XtGetValues(dc, args, 2);

    initiatorAtom = XInternAtom(XtDisplayOfObject(dc),
				 XmI_MOTIF_DRAG_INITIATOR_INFO,
				 FALSE);
    length = 100000L;
    if (XGetWindowProperty (XtDisplayOfObject(dc),
			     srcWindow,
			     iccHandle,
			     0L,
			     length,
			     False,
			     initiatorAtom,
			     &type,
			     &format,
			     &lengthRtn,
			     &bytesafter,
			     (unsigned char **) &info) == Success)
      {
	if(lengthRtn >= sizeof(xmDragInitiatorInfoStruct))
	  {
	    if (info->byte_order != _XmByteOrderChar) {
	      swap2bytes(info->targets_index);
	      swap4bytes(info->icc_handle);
	    }
	    ++set_exports;
	    numExportTargets =
	      _XmIndexToTargets(dc, info->targets_index, &exportTargets);
	  }
      } else {
	/**
	 * Get the Xdnd type list, if the receiver has published it
	 */
	xdndTypeList = XInternAtom(XtDisplayOfObject(dc), "XdndTypeList", False);
	if (XGetWindowProperty(XtDisplayOfObject(dc), srcWindow, xdndTypeList,
	                       0, 65536, False, XA_ATOM, &type, &format,
	                       &lengthRtn, &bytesafter, &data) == Success) {
	    if (type == XA_ATOM && format == 32) {
		++set_exports;
		exportTargets    = (Atom *)data;
		numExportTargets = lengthRtn;
	    }
	}
      }

    /* exportTargets gets duplicated in DragContextSetValues() */
    if (set_exports) {
	    XtSetArg(args[0], XmNexportTargets, exportTargets);
	    XtSetArg(args[1], XmNnumExportTargets, numExportTargets);
	    XtSetArg(args[2], XmNiccHandle, iccHandle);
	    XtSetValues(dc, args, 3);
    }

    /* free the memory that Xlib passed us */
    if (info) XFree(info);
    if (data) XFree(data);
}

/************************************************************************
 *
 *  _XmGetDragReceiverInfo()
 *
 *  The caller is responsible for freeing (using XFree) the dataRtn
 *  pointer that passes thru the memory allocated by XGetWindowProperty.
 ***********************************************************************/
Boolean _XmGetDragReceiverInfo(Display *display, Window window,
                               XmDragReceiverInfoStruct *receiverInfoRtn)
{
    xmDragReceiverInfoStruct *iccInfo = NULL;
    int format;
    unsigned long bytesafter, lengthRtn, length;
    XmReceiverDSTreeStruct *dsmInfo;
    Window root;
    unsigned int bw;
    unsigned char *data;
    Atom drag_hints_atom, type = None;
    Atom xdndAware, xdndProxy;
    XmDisplay dd = (XmDisplay) XmGetXmDisplay(display);

    /* get their geometry */
    assert(receiverInfoRtn);
    XGetGeometry(display, window, &root,
                 &(receiverInfoRtn->xOrigin),
                 &(receiverInfoRtn->yOrigin),
                 &(receiverInfoRtn->width),
                 &(receiverInfoRtn->height), &bw, &(receiverInfoRtn->depth));
    XTranslateCoordinates(display, window, root, -(int)bw, -(int)bw,
                          &(receiverInfoRtn->xOrigin),
                          &(receiverInfoRtn->yOrigin), &root);

    drag_hints_atom = XInternAtom(display,
				   XmI_MOTIF_DRAG_RECEIVER_INFO,
				   FALSE);
    length = 100000L;
    if (XGetWindowProperty(display,
			    window,
			    drag_hints_atom,
			    0L,
			    length,
			    False,
			    drag_hints_atom,
			    &type,
			    &format,
			    &lengthRtn,
			    &bytesafter,
			    (unsigned char **) &iccInfo) == Success)
      {
	if (lengthRtn >= sizeof(xmDragReceiverInfoStruct)) {
	  if (iccInfo->protocol_version != _MOTIF_DRAG_PROTOCOL_VERSION)
	    {
	      XmeWarning((Widget)dd, MESSAGE2);
	    }
	  if (iccInfo->byte_order != _XmByteOrderChar) {
	    swap2bytes(iccInfo->num_drop_sites);
	    swap4bytes(iccInfo->proxy_window);
	    swap4bytes(iccInfo->heap_offset);
	  }

	  dd->display.proxyWindow = iccInfo->proxy_window;
	  receiverInfoRtn->dragProtocolStyle = iccInfo->drag_protocol_style;

	  dsmInfo = XtNew(XmReceiverDSTreeStruct);
	  dsmInfo->byteOrder = iccInfo->byte_order;
	  dsmInfo->numDropSites = iccInfo->num_drop_sites;
	  dsmInfo->currDropSite = 0;
	  dsmInfo->propBufRec.data.bytes = (BYTE*)iccInfo;
	  dsmInfo->propBufRec.data.size = (size_t) iccInfo->heap_offset;
	  dsmInfo->propBufRec.heap.bytes =
	    (BYTE*)iccInfo + iccInfo->heap_offset;
	  dsmInfo->propBufRec.heap.size = (size_t)
	    (lengthRtn - iccInfo->heap_offset);
	  /*
	   * skip over the info that we've already got
	   */
	  dsmInfo->propBufRec.data.curr =
	    (BYTE*)iccInfo + sizeof(xmDragReceiverInfoStruct);
	  (receiverInfoRtn)->iccInfo = (XtPointer) dsmInfo;
	  return True;
	}
      }

    /**
     * Target doesn't support Motif DND, try Xdnd
     */
    if (iccInfo)
	XFree(iccInfo);
    receiverInfoRtn->iccInfo = NULL;
    receiverInfoRtn->dragProtocolStyle = XmDRAG_NONE;

    xdndAware = XInternAtom(display, "XdndAware", False);
    xdndProxy = XInternAtom(display, "XdndProxy", False);
    dd->display.proxyWindow = window;
    if (XGetWindowProperty(display, window, xdndProxy, 0, 1, False,
                           XA_WINDOW, &type, &format, &lengthRtn,
                           &bytesafter, &data) == Success) {
	if (type != None && format == 32 && length == 1)
	    dd->display.proxyWindow = *(Window *)data;
	XFree(data);
    }

    /* Check for XdndAware and protocol version */
    if (XGetWindowProperty(display, dd->display.proxyWindow, xdndAware,
                           0, 1, False, AnyPropertyType, &type, &format,
                           &length, &bytesafter, &data) == Success) {
	if (type != None && format == 32 && length == 1) {
	    if (*(Atom *)data >= (Atom)xdnd_version_min) {
		receiverInfoRtn->dragProtocolStyle = XmDRAG_XDND;
		XFree(data);
		return True;
	    }
	}

	/* Xdnd versions < 3 are not supported */
	XFree(data);
    }

    return False;
}

/************************************************************************
 *
 *  _XmReadDSFromStream()
 *
 ***********************************************************************/
Boolean
_XmReadDSFromStream(
        XmDropSiteManagerObject dsm,
        XtPointer iccInfo,
        XmICCDropSiteInfo dropSiteInfoRtn )
{
    xmDSHeaderStruct	dsHeader;
    XmReceiverDSTree	dsmInfo = (XmReceiverDSTree)iccInfo;
    xmPropertyBufferRec	*propBuf = &dsmInfo->propBufRec;
    int			i;
    xmICCRegBoxRec	box;
    XmRegion		region;

    _XmReadDragBuffer (propBuf, BUFFER_DATA,
		       (BYTE*)&dsHeader, sizeof(xmDSHeaderStruct));

    if (dsmInfo->byteOrder != _XmByteOrderChar) {
	swap2bytes(dsHeader.flags);
	swap2bytes(dsHeader.import_targets_id);
	swap4bytes(dsHeader.dsRegionNumBoxes);
    }
    dropSiteInfoRtn->header.traversalType =
	(unsigned char) GET_TRAVERSAL_TYPE(dsHeader.flags);
    dropSiteInfoRtn->header.dropActivity =
	(unsigned char) GET_DS_ACTIVITY(dsHeader.flags);
    dropSiteInfoRtn->header.dropType =
	(unsigned char) GET_DS_TYPE(dsHeader.flags);
    dropSiteInfoRtn->header.operations =
	(unsigned char) GET_MULTIOPS(dsHeader.flags);
    dropSiteInfoRtn->header.animationStyle =
	(unsigned char) GET_ANIMATION_STYLE(dsHeader.flags);
    dropSiteInfoRtn->header.importTargetsID =
	(unsigned short) dsHeader.import_targets_id;

    switch (dropSiteInfoRtn->header.animationStyle) {

	case XmDRAG_UNDER_HIGHLIGHT:
	    {
		XmICCDropSiteHighlight	info =
		    (XmICCDropSiteHighlight) dropSiteInfoRtn;
		xmDSHighlightDataStruct	dsHighlight;

		_XmReadDragBuffer (propBuf, BUFFER_DATA,
		                   (BYTE*)&dsHighlight,
				   sizeof(xmDSHighlightDataStruct));
		if (dsmInfo->byteOrder != _XmByteOrderChar) {
		    swap2bytes(dsHighlight.borderWidth);
		    swap2bytes(dsHighlight.highlightThickness);
		    swap4bytes(dsHighlight.background);
		    swap4bytes(dsHighlight.highlightColor);
		    swap4bytes(dsHighlight.highlightPixmap);
		}

                info->animation_data.borderWidth =
		    (Dimension) dsHighlight.borderWidth;
		info->animation_data.highlightThickness =
		    (Dimension) dsHighlight.highlightThickness;

                info->animation_data.background =
		    (Pixel) dsHighlight.background;
		info->animation_data.highlightColor =
		    (Pixel) dsHighlight.highlightColor;
		info->animation_data.highlightPixmap =
		    (Pixmap) dsHighlight.highlightPixmap;
	    }
	    break;

	case XmDRAG_UNDER_SHADOW_IN:
	case XmDRAG_UNDER_SHADOW_OUT:
	    {
		XmICCDropSiteShadow	info =
		    (XmICCDropSiteShadow) dropSiteInfoRtn;
		xmDSShadowDataStruct	dsShadow;

		_XmReadDragBuffer (propBuf, BUFFER_DATA,
		                   (BYTE*)&dsShadow,
				   sizeof(xmDSShadowDataStruct));
		if (dsmInfo->byteOrder != _XmByteOrderChar) {
		    swap2bytes(dsShadow.borderWidth);
		    swap2bytes(dsShadow.highlightThickness);
		    swap2bytes(dsShadow.shadowThickness);
		    swap4bytes(dsShadow.foreground);
		    swap4bytes(dsShadow.topShadowColor);
		    swap4bytes(dsShadow.bottomShadowColor);
		    swap4bytes(dsShadow.topShadowPixmap);
		    swap4bytes(dsShadow.bottomShadowPixmap);
		}

                info->animation_data.borderWidth =
		    (Dimension) dsShadow.borderWidth;
		info->animation_data.highlightThickness =
		    (Dimension) dsShadow.highlightThickness;
		info->animation_data.shadowThickness =
		    (Dimension) dsShadow.shadowThickness;

                info->animation_data.foreground =
		    (Pixel) dsShadow.foreground;
		info->animation_data.topShadowColor =
		    (Pixel) dsShadow.topShadowColor;
		info->animation_data.bottomShadowColor =
		    (Pixel) dsShadow.bottomShadowColor;
		info->animation_data.topShadowPixmap =
		    (Pixmap) dsShadow.topShadowPixmap;
		info->animation_data.bottomShadowPixmap =
		    (Pixmap) dsShadow.bottomShadowPixmap;
	    }
	    break;

	case XmDRAG_UNDER_PIXMAP:
	    {
		XmICCDropSitePixmap	info =
		    (XmICCDropSitePixmap) dropSiteInfoRtn;
		xmDSPixmapDataStruct	dsPixmap;

		_XmReadDragBuffer (propBuf, BUFFER_DATA,
		                   (BYTE*)&dsPixmap,
				   sizeof(xmDSPixmapDataStruct));
		if (dsmInfo->byteOrder != _XmByteOrderChar) {
		    swap2bytes(dsPixmap.borderWidth);
		    swap2bytes(dsPixmap.highlightThickness);
		    swap2bytes(dsPixmap.shadowThickness);
		    swap2bytes(dsPixmap.animationPixmapDepth);
		    swap4bytes(dsPixmap.foreground);
		    swap4bytes(dsPixmap.background);
		    swap4bytes(dsPixmap.animationPixmap);
		    swap4bytes(dsPixmap.animationMask);
		}

                info->animation_data.borderWidth =
		    (Dimension) dsPixmap.borderWidth;
		info->animation_data.highlightThickness =
		    (Dimension) dsPixmap.highlightThickness;
		info->animation_data.shadowThickness =
		    (Dimension) dsPixmap.shadowThickness;
		info->animation_data.animationPixmapDepth =
		    (Cardinal) dsPixmap.animationPixmapDepth;

                info->animation_data.foreground =
		    (Pixel) dsPixmap.foreground;
                info->animation_data.background =
		    (Pixel) dsPixmap.background;
		info->animation_data.animationPixmap =
		    (Pixmap) dsPixmap.animationPixmap;
		info->animation_data.animationMask =
		    (Pixmap) dsPixmap.animationMask;
	    }
	    break;

	case XmDRAG_UNDER_NONE:
	    {
		XmICCDropSiteNone	info =
		    (XmICCDropSiteNone) dropSiteInfoRtn;
		xmDSNoneDataStruct	dsNone;

		_XmReadDragBuffer (propBuf, BUFFER_DATA,
		                   (BYTE*)&dsNone,
				   sizeof(xmDSNoneDataStruct));
		if (dsmInfo->byteOrder != _XmByteOrderChar) {
		    swap2bytes(dsNone.borderWidth);
		}

                info->animation_data.borderWidth =
		    (Dimension) dsNone.borderWidth;
	    }
	    break;
	default:
	    break;
    }

    /*
     *  Read the region, byte swapping if necessary.
     */

    region = dropSiteInfoRtn->header.region =
	_XmRegionCreateSize ((long) dsHeader.dsRegionNumBoxes);

    for (i = 0; i < (long) dsHeader.dsRegionNumBoxes; i++) {
	_XmReadDragBuffer(propBuf, BUFFER_DATA, (BYTE*) &box,
		              sizeof(xmICCRegBoxRec));
	if (dsmInfo->byteOrder != _XmByteOrderChar) {
	    swap2bytes(box.x1);
	    swap2bytes(box.x2);
	    swap2bytes(box.y1);
	    swap2bytes(box.y2);
	}
        region->rects[i].x1 = (short) cvtINT16toShort(box.x1);
        region->rects[i].x2 = (short) cvtINT16toShort(box.x2);
        region->rects[i].y1 = (short) cvtINT16toShort(box.y1);
        region->rects[i].y2 = (short) cvtINT16toShort(box.y2);
    }
    region->numRects = (long) dsHeader.dsRegionNumBoxes;
    _XmRegionComputeExtents (region);

    if (++dsmInfo->currDropSite == dsmInfo->numDropSites) {
	/* free all the wire data */
	XtFree((char *)dsmInfo->propBufRec.data.bytes);
	XtFree((char *)dsmInfo);
#ifdef DEBUG
	printf("freed the dsmInfo, all done\n");
#endif
    }
    return True;
}

/************************************************************************
 *
 *  _XmWriteDSToStream()
 *
 ***********************************************************************/
void
_XmWriteDSToStream(
        XmDropSiteManagerObject dsm,
        XtPointer stream,
        XmICCDropSiteInfo dropSiteInfo )
{
    xmDSHeaderStruct	dsHeader;
    XmReceiverDSTree	dsmInfo = (XmReceiverDSTree)stream;
    xmPropertyBufferRec	*propBuf = &dsmInfo->propBufRec;
    int			i;
    xmICCRegBoxRec	box;
    XmRegion		region = dropSiteInfo->header.region;

    dsHeader.flags = 0;
    dsHeader.flags |= PUT_TRAVERSAL_TYPE(dropSiteInfo->header.traversalType);
    dsHeader.flags |= PUT_DS_ACTIVITY(dropSiteInfo->header.dropActivity);
    dsHeader.flags |= PUT_DS_TYPE(dropSiteInfo->header.dropType);
    dsHeader.flags |= PUT_MULTIOPS(dropSiteInfo->header.operations);
    dsHeader.flags |= PUT_ANIMATION_STYLE(dropSiteInfo->header.animationStyle);
    dsHeader.import_targets_id = dropSiteInfo->header.importTargetsID;
    dsHeader.dsRegionNumBoxes =  region->numRects;

    _XmWriteDragBuffer(propBuf, BUFFER_DATA, (BYTE*)&dsHeader,
		       sizeof(xmDSHeaderStruct));

    switch (dropSiteInfo->header.animationStyle) {

	case XmDRAG_UNDER_HIGHLIGHT:
	    {
		XmICCDropSiteHighlight	info =
		    (XmICCDropSiteHighlight) dropSiteInfo;
		xmDSHighlightDataStruct	dsHighlight;

                dsHighlight.borderWidth =
		     info->animation_data.borderWidth;
                dsHighlight.highlightThickness =
		     info->animation_data.highlightThickness;

                dsHighlight.background =
		     info->animation_data.background;
                dsHighlight.highlightColor =
		     info->animation_data.highlightColor;
                dsHighlight.highlightPixmap =
		     info->animation_data.highlightPixmap;

		_XmWriteDragBuffer (propBuf, BUFFER_DATA,
				    (BYTE*)&dsHighlight,
		                    sizeof(xmDSHighlightDataStruct));
	    }
	    break;

	case XmDRAG_UNDER_SHADOW_IN:
	case XmDRAG_UNDER_SHADOW_OUT:
	    {
		XmICCDropSiteShadow	info =
		    (XmICCDropSiteShadow) dropSiteInfo;
		xmDSShadowDataStruct	dsShadow;

                dsShadow.borderWidth =
		     info->animation_data.borderWidth;
                dsShadow.highlightThickness =
		     info->animation_data.highlightThickness;
                dsShadow.shadowThickness =
		     info->animation_data.shadowThickness;

                dsShadow.foreground =
		     info->animation_data.foreground;
                dsShadow.topShadowColor =
		     info->animation_data.topShadowColor;
                dsShadow.bottomShadowColor =
		     info->animation_data.bottomShadowColor;
                dsShadow.topShadowPixmap =
		     info->animation_data.topShadowPixmap;
                dsShadow.bottomShadowPixmap =
		     info->animation_data.bottomShadowPixmap;

		_XmWriteDragBuffer (propBuf, BUFFER_DATA,
				    (BYTE*)&dsShadow,
		                    sizeof(xmDSShadowDataStruct));
	    }
	    break;

	case XmDRAG_UNDER_PIXMAP:
	    {
		XmICCDropSitePixmap	info =
		    (XmICCDropSitePixmap) dropSiteInfo;
		xmDSPixmapDataStruct	dsPixmap;

                dsPixmap.borderWidth =
		     info->animation_data.borderWidth;
                dsPixmap.highlightThickness =
		     info->animation_data.highlightThickness;
                dsPixmap.shadowThickness =
		     info->animation_data.shadowThickness;
                dsPixmap.animationPixmapDepth =
		     info->animation_data.animationPixmapDepth;

                dsPixmap.foreground =
		     info->animation_data.foreground;
                dsPixmap.background =
		     info->animation_data.background;
                dsPixmap.animationPixmap =
		     info->animation_data.animationPixmap;
                dsPixmap.animationMask =
		     info->animation_data.animationMask;

		_XmWriteDragBuffer (propBuf, BUFFER_DATA,
				    (BYTE*)&dsPixmap,
		                    sizeof(xmDSPixmapDataStruct));
	    }
	    break;

	case XmDRAG_UNDER_NONE:
	    {
		XmICCDropSiteNone	info =
		    (XmICCDropSiteNone) dropSiteInfo;
		xmDSNoneDataStruct	dsNone;

                dsNone.borderWidth =
		     info->animation_data.borderWidth;

		_XmWriteDragBuffer (propBuf, BUFFER_DATA,
				    (BYTE*)&dsNone,
		                    sizeof(xmDSNoneDataStruct));
	    }
	    break;
	default:
	    break;
    }

    /* write each rectangle box */

    for (i = 0; i < region->numRects; i++) {
        box.x1 = (INT16) region->rects[i].x1;
        box.x2 = (INT16) region->rects[i].x2;
        box.y1 = (INT16) region->rects[i].y1;
        box.y2 = (INT16) region->rects[i].y2;
        _XmWriteDragBuffer(propBuf, BUFFER_DATA, (BYTE*) &box,
		           sizeof(xmICCRegBoxRec));
    }
}

/************************************************************************
 *
 *  _XmFreeDragReceiverInfo()
 *
 ***********************************************************************/

void
_XmFreeDragReceiverInfo(
        XtPointer info )
{
    XmReceiverDSTreeStruct	*dsmInfo = (XmReceiverDSTreeStruct *)info;

    if (dsmInfo) {
	XtFree((char *)dsmInfo->propBufRec.data.bytes);
	XtFree((char *)dsmInfo);
    }
}

/************************************************************************
 *
 *  _XmClearDragReceiverInfo()
 *
 *  We can pass in the shell since we're pushing a property at one of
 *  our windows.
 ***********************************************************************/
void _XmClearDragReceiverInfo(Widget shell)
{
	Atom receiverAtom, xdndAware;

	receiverAtom = XInternAtom(XtDisplayOfObject(shell), XmI_MOTIF_DRAG_RECEIVER_INFO, False);
	xdndAware    = XInternAtom(XtDisplayOfObject(shell), "XdndAware", False);
	XDeleteProperty(XtDisplayOfObject(shell), XtWindow(shell), receiverAtom);
	XDeleteProperty(XtDisplayOfObject(shell), XtWindow(shell), xdndAware);
}

/************************************************************************
 *
 *  _XmSetDragReceiverInfo()
 *
 *  We can pass in the shell since we're pushing a property at one of
 *  our windows.  This procedure should not be called for receivers
 *  with a protocol style of NONE, because such receivers should not
 *  have any visible receiver info.
 ***********************************************************************/
void _XmSetDragReceiverInfo(XmDisplay dd, Widget shell)
{
    Cardinal			numDropSites = 0;
    BYTE			stackData[MAXSTACK];
    BYTE			stackHeap[MAXSTACK];
    xmPropertyBufferRec		*propBuf;
    xmDragReceiverInfoStruct	infoRec, *infoRecPtr;
    XmReceiverDSTreeStruct	dsmInfoRec;
    XmDropSiteManagerObject 	dsm;
    Atom receiverAtom, xdndAware;

    dsm          = _XmGetDropSiteManagerObject(dd);
    receiverAtom = XInternAtom(XtDisplayOfObject(shell), XmI_MOTIF_DRAG_RECEIVER_INFO, False);

    dsmInfoRec.numDropSites = 0;
    dsmInfoRec.currDropSite = 0;

    propBuf = &dsmInfoRec.propBufRec;
    propBuf->data.stack =
      propBuf->data.bytes = stackData;
    propBuf->data.size = 0;
    propBuf->data.max = MAXSTACK;
    propBuf->heap.stack =
      propBuf->heap.bytes = stackHeap;
    propBuf->heap.size = 0;
    propBuf->heap.max = MAXSTACK;

    infoRec.byte_order = _XmByteOrderChar;
    infoRec.protocol_version = _MOTIF_DRAG_PROTOCOL_VERSION;

    infoRec.drag_protocol_style = dd->display.dragReceiverProtocolStyle;
	infoRec.proxy_window = None;

    _XmWriteDragBuffer (propBuf, BUFFER_DATA, (BYTE*)&infoRec,
		        sizeof(xmDragReceiverInfoStruct));

	/*
	 * Only attach a drop site tree to the property if the receiver
	 * protocol style is not dynamic and not drop only (this procedure
	 * shouldn't get called if style is NONE).
	 */
    if ((dd->display.dragReceiverProtocolStyle != XmDRAG_DYNAMIC) &&
        (dd->display.dragReceiverProtocolStyle != XmDRAG_DROP_ONLY))
      {
	  numDropSites = _XmDSMGetTreeFromDSM(dsm, shell, (XtPointer)&dsmInfoRec);
      }
    else
      {
	/*
	 * However, this procedure is called on the realize of the
	 * shell, and the drop site manager is counting on having a
	 * chance to synchronize the drop site db with the actual
	 * widget information.
	 */

	 _XmSyncDropSiteTree(shell);
      }

    infoRecPtr = (xmDragReceiverInfoStruct *)propBuf->data.bytes;

    infoRecPtr->num_drop_sites = numDropSites;

    infoRecPtr->heap_offset = propBuf->data.size;

    /* write the buffer to the property */
    XChangeProperty (XtDisplayOfObject(shell),
		     XtWindow(shell),
		     receiverAtom, receiverAtom,
		     8,
		     PropModeReplace,
		     (unsigned char *)propBuf->data.bytes,
		     propBuf->data.size);
    if (propBuf->data.bytes != propBuf->data.stack)
      XtFree((char *)propBuf->data.bytes);

    if (propBuf->heap.size) {
	XChangeProperty (XtDisplayOfObject(shell),
			 XtWindow(shell),
			 receiverAtom, receiverAtom,
			 8, PropModeAppend,
			 (unsigned char *)propBuf->heap.bytes,
			 propBuf->heap.size);
	if (propBuf->heap.bytes != propBuf->heap.stack)
	  XtFree((char *)propBuf->heap.bytes);

    }

    xdndAware = XInternAtom(XtDisplayOfObject(shell), "XdndAware", False);
    XChangeProperty(XtDisplayOfObject(shell), XtWindow(shell),
                    xdndAware, XA_ATOM, 32, PropModeReplace,
                    &xdnd_version, 1);
}

/************************************************************************
 *
 *  _XmInitByteOrderChar()
 *
 ***********************************************************************/

void
_XmInitByteOrderChar( void )
{
    _XmProcessLock();
    /* bootstrap the byteorder if needed */
    if (!_XmByteOrderChar) {
	unsigned int		endian = 1;

	/* get rid of irritating saber warning */
	/*SUPPRESS 112*/
	if (*((char *)&endian))
	  _XmByteOrderChar = 'l';
	else
	  _XmByteOrderChar = 'B';
	}
    _XmProcessUnlock();
}

#ifdef DEBUG
static void
PrintMessage(char c, xmICCMessageStruct *xmessage)
{
  int message_type = xmessage->any.message_type;

  message_type &= CLEAR_ICC_EVENT_TYPE;

  switch (_XmMessageTypeToReason(message_type)) {
  case XmCR_TOP_LEVEL_ENTER:
    printf("%c ", c);
    printf("TL Enter %x %x %x %x\n",
	   xmessage->topLevelEnter.flags,
	   xmessage->topLevelEnter.time,
	   xmessage->topLevelEnter.src_window,
	   xmessage->topLevelEnter.icc_handle);
    break;
  case XmCR_TOP_LEVEL_LEAVE:
    printf("%c ", c);
    printf("TL Leave %x %x %x\n",
	   xmessage->topLevelLeave.flags,
	   xmessage->topLevelLeave.time,
	   xmessage->topLevelLeave.src_window);
    break;
  case XmCR_DRAG_MOTION:
#ifdef DEBUG_DRAG_MOTION
    printf("%c ", c);
    printf("Motion %x %x %x %x\n",
	   xmessage->dragMotion.flags,
	   xmessage->dragMotion.time,
	   xmessage->dragMotion.x,
	   xmessage->dragMotion.y);
#endif /* DEBUG_DRAG_MOTION */
    break;
  case XmCR_OPERATION_CHANGED:
    printf("%c ", c);
    printf("Op changed %x %x\n",
	   xmessage->operationChanged.flags,
	   xmessage->operationChanged.time);
    break;
  case XmCR_DROP_SITE_ENTER:
    printf("%c ", c);
    printf("Enter %x %x %x %x\n",
	   xmessage->dropSiteEnter.flags,
	   xmessage->dropSiteEnter.time,
	   xmessage->dropSiteEnter.x,
	   xmessage->dropSiteEnter.y);
    break;
  case XmCR_DROP_SITE_LEAVE:
    printf("%c ", c);
    printf("Leave %x %x\n",
	   xmessage->dropSiteLeave.flags,
	   xmessage->dropSiteLeave.time);
    break;
  case XmCR_DROP_START:
    printf("%c ", c);
    printf("Start %x %x %x %x %x %x\n",
	   xmessage->drop.flags,
	   xmessage->drop.time,
	   xmessage->drop.x,
	   xmessage->drop.y,
	   xmessage->drop.icc_handle,
	   xmessage->drop.src_window);
    break;
  case XmCR_DROP_FINISH:
  case XmCR_DRAG_DROP_FINISH:
    printf("%c ", c);
    puts("Finish");
    break;
  default:
    break;
  }
}
#endif /* DEBUG */

