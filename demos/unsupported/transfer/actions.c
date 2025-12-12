/* $XConsortium: actions.c /main/5 1995/07/15 20:46:10 drk $ */
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

#include <Xm/XmP.h>
#include <Xm/TransferP.h>
#include "transfer.h"

/* We assume no more than 100 targets will ever be available.  MAXT
   is this upper limit */

#define MAXT 100
#define BYTELENGTH( length, format ) \
  ((format == 8) ? length : \
   ((format == 16) ? length * sizeof(short) : \
    (length * sizeof(long))))


TransferDataRec datums[MAXT]; /* Should never be larger than this */
unsigned int num_datums = 0;

TransferDataRec snapshot[MAXT];
unsigned int num_snapshot = 0;

/*******************************************************************
 * UpdateList
 * 
 * This takes the transfered items and updates the displayed list
 * with the transferred target names.  Note that we call a version of
 * XGetAtomName with an installed error handler.  This prevents an
 * app. exit if the Atom id is illegal.  
 *******************************************************************/

void 
UpdateList(Widget w, XtEnum ignore, XmTransferDoneCallbackStruct *data)
{
  int i;

  XmListDeleteAllItems(list);

  for(i = 0; i < num_datums; i++) {
    char *name;
    XmString temp;

    if (datums[i].target != 0) {
      name = GetSafeAtom(XtDisplay(w), datums[i].target);
      if (name != NULL) {
	temp = XmStringCreateLocalized(name);
	XFree(name);
      } else {
	temp = XmStringCreateLocalized("Illegal atom");
      }
    } else {
      temp = XmStringCreateLocalized("Bad target");
      if (name != NULL) XFree(name);
    }

    XmListAddItemUnselected(list, temp, 0);
    XmStringFree(temp);
  }

  XmListSelectPos(list, 1, True);
}

/********************************************************************
 * ReceiveData
 *
 * This requests the targets passed from the request for TARGETS that
 * happed in the destination callback.  Note that you must not start
 * an infinite loop of requesting the target TARGETS.  It will normally
 * be in the list of targets received.  Also,  requesting DELETE has
 * both a side effect and no value associated.  So we do not request
 * these targets.
 ********************************************************************/

void 
ReceiveData(Widget w, XtPointer ignore, XmSelectionCallbackStruct *data)
{
  Atom MRT = XInternAtom(XtDisplay(w), XmS_MOTIF_RENDER_TABLE, False);

  if (data -> target != XInternAtom(XtDisplay(w), "TARGETS", False)
      && num_datums < MAXT) {
    datums[num_datums].target = data -> target;
    datums[num_datums].type = data -> type;
    datums[num_datums].length = data -> length;
    datums[num_datums].format = data -> format;
    datums[num_datums].value = data -> value;
    datums[num_datums].deferred = False;
    num_datums++;

    /* If this is a render table,  use it on the display text 
       widget. */
    if (data -> target == MRT &&
	data -> type == XA_STRING) {
      XmRenderTable tab;
      tab = XmRenderTableCvtFromProp(w, (char*) data -> value, 
				     (unsigned int) data -> length);
      XtVaSetValues(text_display, XmNrenderTable, tab, NULL, NULL);
    }

  } else {
    Atom *dlist = (Atom *) data -> value;
    Atom DELETE, TARGETS, INSERT_SELECTION, LINK_SELECTION;
    int i, count = 0;

    if (data -> length == 0) {
      XmTransferDone(data -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }

    num_datums = 0;

    /* This doneProc is performed after all transfers have completed. */
    XmeTransferAddDoneProc(data -> transfer_id, 
			   (XmSelectionFinishedProc) UpdateList);

    DELETE = XInternAtom(XtDisplay(w), "DELETE", False);
    TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
    INSERT_SELECTION = XInternAtom(XtDisplay(w), "INSERT_SELECTION", False);
    LINK_SELECTION = XInternAtom(XtDisplay(w), "LINK_SELECTION", False);

    /* first free current datums */
    for(i = 0; i < num_datums; i++)
      XtFree(datums[num_datums].value);

    XmTransferStartRequest(data -> transfer_id);

    for(i = 0; i < data -> length; i++) {
      if (dlist[i] != DELETE &&
	  dlist[i] != INSERT_SELECTION &&
	  dlist[i] != LINK_SELECTION &&
	  dlist[i] != None &&
	  dlist[i] != TARGETS) {
	XmTransferValue(data -> transfer_id, dlist[i], 
			(XtCallbackProc) ReceiveData, NULL, 0);
      }
    }
    
    XmTransferSendRequest(data -> transfer_id, 0);
  }
}

/*****************************************************************
 * DestinationCallback
 *
 * This just requests the list of targets.  ReceiveData does the
 * rest.
 *****************************************************************/

void 
targetDestinationCallback(Widget w, XtPointer ignore, 
			  XmDestinationCallbackStruct *cs)
{
  XmTransferValue(cs -> transfer_id,
		  XInternAtom(XtDisplay(w), "TARGETS", False),
		  (XtCallbackProc) ReceiveData, NULL, 0);
}

void 
targetConvertCallback(Widget w, XtPointer ignore, XmConvertCallbackStruct *cs)
{
  Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
  Atom ME_TARGETS = XInternAtom(XtDisplay(w), "_MOTIF_EXPORT_TARGETS", False);
  Atom MC_TARGETS = XInternAtom(XtDisplay(w), 
				"_MOTIF_CLIPBOARD_TARGETS", False);
  Atom MD_TARGETS = XInternAtom(XtDisplay(w), 
				"_MOTIF_DEFERRED_CLIPBOARD_TARGETS", False);
  Atom SNAPSHOT = XInternAtom(XtDisplay(w),
			      "_MOTIF_SNAPSHOT", False);
  Atom DONE = XInternAtom(XtDisplay(w), "DONE", False);
  Atom NAMED = XInternAtom(XtDisplay(w), _NAMED, False);
  Atom DELETE = XInternAtom(XtDisplay(w), "DELETE", False);
  Atom LS = XInternAtom(XtDisplay(w), "_MOTIF_LOSE_SELECTION", False);
  Atom MDEST = XInternAtom(XtDisplay(w), "_MOTIF_DESTINATION", False);
  Atom A_SNAPSHOT = XInternAtom(XtDisplay(w),
				"_TRANSFER_SNAPSHOT", False);

  if (cs -> selection == A_SNAPSHOT) {
    cs -> status = XmCONVERT_REFUSE;

    if (cs -> target == DONE) {
      int i;

      /* Disown the particular snapshot */
      XtDisownSelection(w, cs -> selection, 
			XtLastTimestampProcessed(XtDisplay(w)));

      /* Free the data */
      for(i = 0; i < num_snapshot; i++)
	XtFree(snapshot[i].value);
      num_snapshot = 0;

      cs -> status = XmCONVERT_DONE;
    } else if (cs -> target != LS) {
      /* Ignore LOSE_SELECTION,  this was handled above for the DONE
	 target */
      if (num_snapshot > 0) {
	int i;
	
	for(i = 0; 
	    i < num_snapshot && snapshot[i].target != cs -> target; 
	    i++);

	if (i < num_snapshot) {
	  unsigned long size = BYTELENGTH(snapshot[i].length, 
					  snapshot[i].format);
	  XtPointer rval = XtMalloc(size);
	  
	  memcpy(rval, snapshot[i].value, size);
	  cs -> value = rval;
	  cs -> type = snapshot[i].type;
	  cs -> length = snapshot[i].length;
	  cs -> format = snapshot[i].format;
	  cs -> status = XmCONVERT_DONE;
	}
      } 
    }
    return;
  }
    
  if (cs -> target == ME_TARGETS ||
      cs -> target == MC_TARGETS ||
      cs -> target == MD_TARGETS) {
    /* Create an array large enough to hold all targets */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * num_datums);
    int i;
    int count = 0;
    Boolean deferred = (cs -> target == MD_TARGETS);
    Boolean clipboard = (cs -> target == MC_TARGETS);

    /* Never grab side effect targets,  and only grab the right
       set for the clipboard target requests */
    for(i = 0; i < num_datums; i++) {
      if (datums[i].target != DELETE &&
	  datums[i].target != TARGETS &&
	  datums[i].target != MC_TARGETS &&
	  datums[i].target != MD_TARGETS &&
	  datums[i].target != ME_TARGETS &&
	  ((cs -> target == ME_TARGETS) ||
	   (clipboard && datums[i].deferred == False) ||
	   (deferred && datums[i].deferred == True))
	  )
	{
	  targs[count++] = datums[i].target;
	}
    }
    
    if (count > 0) {
      cs -> value = (XtPointer) targs;
      cs -> length = count;
      cs -> format = 32;
      cs -> type = XA_ATOM;
      cs -> status = XmCONVERT_DONE;
    } else {
      XtFree((char*) targs);
      cs -> status = XmCONVERT_REFUSE;
    }
  } else if (cs -> target == TARGETS) {
    /* Create an array large enough to hold all targets */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * num_datums);
    int i;

    for(i = 0; i < num_datums; i++) {
      targs[i] = datums[i].target;
    }

    cs -> value = (XtPointer) targs;
    cs -> length = num_datums;
    cs -> format = 32;
    cs -> type = XA_ATOM;
    cs -> status = XmCONVERT_DONE;
  } else if (cs -> target == LS) {
    /* Decide which selection was lost,  turn off that toggle */
    if (cs -> selection == XA_PRIMARY)
      XmToggleButtonSetState(op, False, False);
    else if (cs -> selection == MDEST)
      XmToggleButtonSetState(omd, False, False);
    else if (cs -> selection == NAMED)
      XmToggleButtonSetState(onamed, False, False);
  } else if (cs -> target == SNAPSHOT) {
    Atom snapshot_atom = (Atom) cs -> parm;
    int i, count, size;

    /* Note that it would not generally be acceptable to 
       have only a single distinguisher atom available for 
       SNAPSHOT requests,  but this demonstration and testing
       program doesn't hold multiple snapshots */
    if (snapshot_atom == (Atom) NULL) {
      snapshot_atom = A_SNAPSHOT;
    }

    /* Setup the snapshot data */
    for(i = 0, count = 0; i < num_datums; i++) {
      if (datums[i].deferred) {
	/* Copy most of the data */
	snapshot[count] = datums[i];
	/* Now realloc the value */
	size = BYTELENGTH(datums[i].length, datums[i].format);
	snapshot[count].value = XtMalloc(size);
	memcpy(snapshot[count].value, datums[i].value, size);
	count++;
      }
    }

    num_snapshot = count;

    /* Setup the return value (the snapshot atom) */
    cs -> value = (XtPointer) XtMalloc(sizeof(Atom));
    cs -> length = 1;
    cs -> type = XA_ATOM;
    cs -> format = 32;
    ((Atom *) cs -> value)[0] = snapshot_atom;

    /* Own the particular snapshot.  We won't bother keeping
       state,  as this is for testing,  not for real persistence.
       Note that using XtLastTimestampProcessed here is not
       ideal,  but the SNAPSHOT distinguisher selection atom 
       is a display unique unowned atom,  so it isn't crucial */
    XmeNamedSource(w, snapshot_atom,
		   XtLastTimestampProcessed(XtDisplay(w)));
    cs -> status = XmCONVERT_DONE;
  } else {
    /* Try and find the right target,  otherwise fail */
    int i = 0;
    
    while(i < num_datums &&
	  datums[i].target != cs -> target) 
      i++;

    if (i >= num_datums /* Not found */ ||
	datums[i].type == (Atom) 0 /* Illegal */ )
      {
	cs -> value = NULL;
	cs -> length = 0;
	cs -> type = None;
	cs -> format = 8;
	cs -> status = XmCONVERT_REFUSE;
      }
    else
      {
	char *temp;
	int size_in_bytes;

	size_in_bytes = datums[i].length * datums[i].format / 8;
	temp = XtMalloc(size_in_bytes);
	memcpy(temp, datums[i].value, size_in_bytes);

	cs -> status = XmCONVERT_DONE;
	cs -> length = datums[i].length;
	cs -> format = datums[i].format;
	cs -> type = datums[i].type;
	cs -> value = temp;
      }
  }
}

/* Error handler for XGetAtomName */

static int SIF_ErrorFlag;
 
static int 
SIF_ErrorHandler(Display *display, XErrorEvent *event)
{
  SIF_ErrorFlag = event -> type;

  return 0;
}

char* 
GetSafeAtom(Display *display, Atom a)
{
  XErrorHandler old_Handler;
  char *returnvalue;

  /* Setup error proc and reset error flag */
  old_Handler = XSetErrorHandler((XErrorHandler) SIF_ErrorHandler);
  SIF_ErrorFlag = 0;

  returnvalue = XGetAtomName(display, a);

  XSetErrorHandler(old_Handler);

  if (SIF_ErrorFlag == 0)
    return(returnvalue);
  else
    return(NULL);
}
