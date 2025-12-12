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
/*
 * HISTORY
 */
/* $XConsortium: TransferI.h /main/6 1996/10/16 16:57:37 drk $ */
#ifndef _XmTransferI_H
#define _XmTransferI_H

#include <Xm/TransferT.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************/
/* Structure which is stored for each transfer request.  This   */
/* is where XmTransferValue will keep the data for the internal */
/* wrapper. 							*/
/****************************************************************/

#define TB_NONE		    0
#define TB_IGNORE	    1
#define TB_INTERNAL   	    2

typedef struct {
  XtPointer		next;
  XtPointer		client_data;
  XtPointer		location_data;
  int			flags;
  Atom			target;
  XtCallbackProc	selection_proc;
} TransferBlockRec, *TransferBlock;

/****************************************************************/
/* This structure forms the context block for each transfer_id. */
/* These structures are chained to allow for multiple           */
/* concurrent outstanding data transfers.                       */
/****************************************************************/

typedef struct {
  XtPointer	next;
  XtPointer	prev;
  Widget	widget;
  Atom		selection;
  Atom		real_selection;
  XtEnum	op;
  int		count;
  int		outstanding;
  int		flags;
  int		status;
  Widget	drag_context;
  Widget	drop_context;
  XmSelectionFinishedProc	*doneProcs;
  int		numDoneProcs;
  XtCallbackProc		auto_proc;
  XtPointer	client_data;
  XmDestinationCallbackStruct	*callback_struct;
  TransferBlock last;
  TransferBlock requests;
} TransferContextRec, *TransferContext;

enum{ TC_NONE = 0, TC_FLUSHED = 1, TC_CALLED_WIDGET = 2,
      TC_CALLED_CALLBACKS = 4, TC_EXITED_DH = 8, 
      TC_DID_DELETE = 16, TC_IN_MULTIPLE = 32 };

/****************************************************************/
/* The convertProc has a small context block which is           */
/* setup by the source calls and deleted in the loseProc        */
/****************************************************************/

typedef struct {
  long		op;	/* Make it big so it can hold a variety of data */
  int		flags;
  long		itemid;
  XtPointer	location_data;
  XtPointer	client_data;
  Widget	drag_context;
} ConvertContextRec, *ConvertContext;

enum{ CC_NONE = 0};

/****************************************************************/
/* Need to keep a count of outstanding SNAPSHOT requests.       */
/****************************************************************/

typedef struct {
  long		outstanding;
  Atom		distinguisher;
} SnapshotRequestRec, *SnapshotRequest;

/* Internal functions */

extern char * _XmTextToLocaleText(Widget, XtPointer, Atom, int,
				  unsigned long, Boolean *);

extern void _XmConvertHandlerSetLocal(void);

extern Boolean _XmConvertHandler(Widget wid, Atom *selection, Atom *target, 
			  Atom *type, XtPointer *value, 
			  unsigned long *size, int *fmt);

extern Boolean _XmDestinationHandler(Widget wid, Atom selection, XtEnum op,
			      XmSelectionFinishedProc done_proc,
			      XtPointer location_data, Time time,
			      XSelectionRequestEvent *event);

extern void _XmConvertComplete(Widget wid, XtPointer value, 
			unsigned long size, int format, Atom type,
			XmConvertCallbackStruct *cs);

extern XmDestinationCallbackStruct
       *_XmTransferGetDestinationCBStruct(XtPointer);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTransferI_H */

