/* $XConsortium: UTMactions.c /main/5 1995/07/15 21:12:10 drk $ */
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

#include <Xm/XmP.h>
#include <Xm/TransferP.h>
#include "UTMtransfer.h"

/* We assume no more than 100 targets will ever be available.  MAXT
   is this upper limit */

#define MAXT 100
#define BYTELENGTH( length, format ) \
  ((format == 8) ? length : \
   ((format == 16) ? length * sizeof(short) : \
    (length * sizeof(long))))

TransferDataRec datums[MAXT]; /* Should never be larger than this */
unsigned int num_datums = 0;

/*******************************************************************
 * UpdateList
 * 
 * This takes the transfered items and updates the displayed list
 * with the transferred target names.  Note that we call a version of
 * XGetAtomName with an installed error handler.  This prevents an
 * app. exit if the Atom id is illegal.  
 *******************************************************************/

static void 
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
      }
      else {
        temp = XmStringCreateLocalized("Illegal atom");
      }
    } 
    else {
      temp = XmStringCreateLocalized("Bad target");
      if (name != NULL) XFree(name);
    }

    XmListAddItemUnselected(list, temp, 0);
    XmStringFree(temp);
  }
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

static void 
ReceiveData(Widget w, XtPointer ignore, XmSelectionCallbackStruct *data)
{
  if (data -> target != XInternAtom(XtDisplay(w), XmSTARGETS, False)) {
    if (num_datums < MAXT) {
      datums[num_datums].target = data -> target;
      datums[num_datums].type = data -> type;
      datums[num_datums].length = data -> length;
      datums[num_datums].format = data -> format;
      datums[num_datums].value = data -> value;
      num_datums++;
    }
  } else {
    Atom *dlist = (Atom *) data -> value;
    Atom DELETE, TARGETS, INSERT_SELECTION, LINK_SELECTION;
    int i, count = 0;

    if (data -> length == 0) {
      XmTransferDone(data -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }

    /* This doneProc is performed after all transfers have completed. */
    XmeTransferAddDoneProc(data -> transfer_id, 
                           (XmSelectionFinishedProc) UpdateList);

    DELETE = XInternAtom(XtDisplay(w), XmSDELETE, False);
    TARGETS = XInternAtom(XtDisplay(w), XmSTARGETS, False);
    INSERT_SELECTION = XInternAtom(XtDisplay(w), "INSERT_SELECTION", False);
    LINK_SELECTION = XInternAtom(XtDisplay(w), "LINK_SELECTION", False);

    /* first free current datums */
    for(i = 0; i < num_datums; i++) {
      XtFree(datums[num_datums].value);
      datums[num_datums].value = NULL;
    }

    num_datums = 0;

    XmTransferStartRequest(data -> transfer_id);

    for(i = 0; i < data -> length; i++) {
      if (dlist[i] != DELETE &&
	  dlist[i] != INSERT_SELECTION &&
	  dlist[i] != LINK_SELECTION &&
          dlist[i] != TARGETS &&
	  dlist[i] != None) {
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
TargetDestinationCB(Widget w, XtPointer ignore, XtPointer call_data)
{
  XmDestinationCallbackStruct *cs;
  char *selection_atom_name, *atom_name;

  cs = (XmDestinationCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  printf("Destination Callback of drawing area called with: \n");
  printf("        selection = %s\n", selection_atom_name);
  printf("        operation = %s\n\n", GetStringFrom(cs->operation));

  XmTransferValue(cs -> transfer_id, 
                  XInternAtom(XtDisplay(w), XmSTARGETS, False),
                  (XtCallbackProc) ReceiveData, NULL, 0);
}

void 
TargetConvertCB(Widget w, XtPointer ignore, XtPointer call_data)
{
  XmConvertCallbackStruct *cs;
  char *selection_atom_name, *target_atom_name;

 
  Atom TARGETS = XInternAtom(XtDisplay(w), XmSTARGETS, False);
  Atom ME_TARGETS = XInternAtom(XtDisplay(w), XmS_MOTIF_EXPORT_TARGETS, False);
  Atom MC_TARGETS = XInternAtom(XtDisplay(w), 
                                XmS_MOTIF_CLIPBOARD_TARGETS, False);
  Atom DELETE = XInternAtom(XtDisplay(w), XmSDELETE, False);
  Atom LS = XInternAtom(XtDisplay(w), XmS_MOTIF_LOSE_SELECTION, False);
  Atom MDEST = XInternAtom(XtDisplay(w), XmS_MOTIF_DESTINATION, False);

  cs = (XmConvertCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  target_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (target_atom_name == NULL)
       target_atom_name = "Illegal atom";

  printf("Convert Callback of drawing area called with: \n");
  printf("        selection         = %s\n", selection_atom_name);
  printf("        conversion target = %s\n\n", target_atom_name);
  

  if (cs -> target == ME_TARGETS ||
      cs -> target == MC_TARGETS) {
    /* Create an array large enough to hold all targets */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * num_datums);
    int i;
    int count = 0;

    for(i = 0; i < num_datums; i++) {
      if (datums[i].target != DELETE &&
	  datums[i].target != MC_TARGETS &&
	  datums[i].target != ME_TARGETS &&
          datums[i].target != TARGETS) {
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
  }
 else if (cs -> target == LS) {
    /* Decide which selection was lost,  remove that label */
    if (cs -> selection == XA_PRIMARY)
      XtUnmanageChild(OwnPrimLG);
    else if (cs -> selection == MDEST)
      XtUnmanageChild(OwnSecLG);
  } 

 else {
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

char * 
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

char *
GetStringFrom(XtEnum operation)
{
  char *returnvalue;

  switch(operation) {
        case XmMOVE:
           returnvalue = "XmMOVE";
           break;
        case XmCOPY: 
           returnvalue = "XmCOPY";
           break;
        case XmLINK:
           returnvalue = "XmLINK";
           break;
        case XmOTHER:
           returnvalue = "XmOTHER";
           break;
        default:
           returnvalue = "Bad operation";
        }

  return(returnvalue);
}
