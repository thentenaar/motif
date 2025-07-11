/* $TOG: Transfer.c /main/17 1997/07/16 16:31:41 csn $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <X11/Xatom.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragDrop.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TraitP.h>
#include <Xm/VendorSP.h>
#include <Xm/XmosP.h>
#include "CutPasteI.h"
#include "DragBSI.h"
#include "HashI.h"
#include "MessagesI.h"
#include "ResEncodI.h"
#include "TransferI.h"
#include "XmI.h"


typedef enum { DoXFree, DoFree } FreeType;
#define FreeSafeAtomName(val,how) if (1) { if (DoXFree==how) XFree(val); else free(val); }

static ConvertContext LookupContextBlock(Display*, Atom);
static void ClearContextBlock(Display*, Atom);
static void ClipboardCallback(Widget, long*, long*, int*);
static void DisownCallback(Widget, XtPointer, XtPointer);
static void FreeTransferID(XtPointer);
static void CallDoneProcs(Widget, XtPointer, XmTransferDoneCallbackStruct*);
static XtPointer GetTransferID(void);
static void SelectionCallbackWrapper(Widget, XtPointer, Atom*, Atom*,
				     XtPointer, unsigned long*, int*);
static Boolean DragConvertHandler(Widget, Atom *, Atom *, Atom *,
				  XtPointer *, unsigned long *, int *);
static void SecondaryConvertHandler(Widget, XtPointer,
				    XmConvertCallbackStruct *);
static void ReleaseSecondaryLock(Widget, XtEnum, XmTransferDoneCallbackStruct*);
static void DropDestinationHandler(Widget, XtPointer,
				   XmDropProcCallbackStruct *);
static TransferBlock AddTransferBlock(TransferContext tc);
static void SecondaryDone(Widget, XtPointer, Atom*, Atom*,
			  XtPointer, unsigned long*, int*);
static void TransferWarning(Widget w, char* name, char* type, char* message);
static void DeleteDropCBStruct(Widget w, XtEnum ignored_status,
			       XmTransferDoneCallbackStruct *cs);
static void FinishTransfer(Widget wid, TransferContext tc);
static char* GetSafeAtomName(Display *display, Atom a, FreeType *howFree);
static void LoseProc(Widget w, Atom *selection);
static void ClipboardLoseProc(Widget w, Atom *selection);

#define BAD_ATOM_MESSAGE    _XmMMsgTransfer_0005
#define BAD_STATUS_MESSAGE  _XmMMsgTransfer_0004
#define BAD_SCB_MESSAGE     _XmMMsgTransfer_0000
#define BAD_MATCHED_CONVERT _XmMMsgTransfer_0002
#define BAD_STATUS          _XmMMsgTransfer_0003
#define ERROR_MULTIPLE_IN_PROGRESS _XmMMsgTransfer_0006
#define ERROR_MULTIPLE_NOT_IN_PROGRESS _XmMMsgTransfer_0007
#define MERGE "XmeConvertMerge"
#define ATOM  "XGetAtomName"
#define MATCH "Format or type mismatch"
#define ARG "Argument"
#define START_MULTIPLE "XmTransferStartRequest"
#define END_MULTIPLE "XmTransferSendRequest"

#define XmS_MOTIF_SNAPSHOT "_MOTIF_SNAPSHOT"
#define XmSCLIPBOARD_MANAGER "CLIPBOARD_MANAGER"

#define BYTELENGTH( length, format ) \
  ((format == 8) ? length : \
   ((format == 16) ? length * sizeof(short) : \
    (length * sizeof(long))))

static int local_convert_flag = 0;

static XmHashTable DataIdDictionary = NULL;

void
_XmConvertHandlerSetLocal(void)
{
  _XmProcessLock();
  local_convert_flag = 1;
  _XmProcessUnlock();
}

/************************************************************************/
/*									*/
/* Convert section.  These functions provide an API to setting up 	*/
/* selections,  working with the clipboard and starting Drag and Drop.  */
/* 									*/
/************************************************************************/

/****************************************************************/
/* ConvertHandler is a generalized version of the convert proc  */
/* in Xt.  It calls the convertCallbacks and the transferTrait  */
/* on the particular widgetclass.				*/
/****************************************************************/

Boolean
_XmConvertHandler(Widget wid, Atom *selection, Atom *target,
		  Atom *type, XtPointer *value,
		  unsigned long *size, int *fmt)
{
  enum { XmA_MOTIF_DESTINATION, XmAINSERT_SELECTION,
	 XmALINK_SELECTION, XmA_MOTIF_LOSE_SELECTION, XmA_MOTIF_DROP,
	 XmACLIPBOARD, XmA_MOTIF_CLIPBOARD_TARGETS,
	 XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS, NUM_ATOMS };
  static char *atom_names[] = { XmS_MOTIF_DESTINATION, XmSINSERT_SELECTION,
	 XmSLINK_SELECTION, XmS_MOTIF_LOSE_SELECTION, XmS_MOTIF_DROP,
	 XmSCLIPBOARD, XmS_MOTIF_CLIPBOARD_TARGETS,
	 XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS };

  XmTransferTrait ttrait;
  XmConvertCallbackStruct cbstruct;
  ConvertContext cc;
  Atom atoms[XtNumber(atom_names)];
  Atom real_selection_atom = None; /* DND hides the selection atom from us */
  int my_local_convert_flag;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(wid), atom_names, XtNumber(atom_names), False, atoms);

  _XmProcessLock();
  my_local_convert_flag = local_convert_flag;
  _XmProcessUnlock();

  /* Find the context block */

  cc = LookupContextBlock(XtDisplay(wid), *selection);

  /* Setup the callback structure */

  cbstruct.reason = XmCR_OK;
  cbstruct.event = NULL;
  cbstruct.selection = *selection;
  cbstruct.target = *target;
  cbstruct.source_data = (XtPointer) cc -> drag_context;
  cbstruct.flags = XmCONVERTING_NONE;
  cbstruct.location_data = cc -> location_data;
  cbstruct.status = XmCONVERT_DEFAULT;
  cbstruct.value = NULL;
  cbstruct.type = XA_INTEGER;
  cbstruct.format = 8;
  cbstruct.length = 0;


  _XmProcessLock();
  /* Get the request event if we can */
  if (my_local_convert_flag == 0) {
    Arg args[1];
    Widget req_widget;

    if (*selection == atoms[XmA_MOTIF_DROP]) {
      XtSetArg(args[0], XmNiccHandle, &real_selection_atom);
      XtGetValues(cc -> drag_context, args, 1);
      cbstruct.event = (XEvent *) XtGetSelectionRequest(cc -> drag_context,
							real_selection_atom,
							NULL);
      req_widget = cc -> drag_context;
    } else {
      cbstruct.event = (XEvent *) XtGetSelectionRequest(wid, *selection,
							NULL);
      req_widget = wid;
    }

    /* Get the parameters from this request.  Use the correct
       selection atom here as well */
    {
      Atom sel_atom = (real_selection_atom != None)
	? real_selection_atom : *selection;
      XtGetSelectionParameters(req_widget, sel_atom, NULL,
			       &cbstruct.parm_type, &cbstruct.parm,
			       &cbstruct.parm_length, &cbstruct.parm_format);
    }
  } else {
    /* We're called locally when XmeClipboardSource is invoked and there
       is no CLIPBOARD_MANAGER.  In this case we must pickup the operation
       from the context block and put it in the parameter */
    if (*selection == atoms[XmACLIPBOARD]) {
      if (*target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS] ||
	  *target == atoms[XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS]) {
	cbstruct.parm = (XtPointer) cc -> op;
	cbstruct.parm_length = 1;
	cbstruct.parm_format = 32;
	cbstruct.parm_type = XA_INTEGER;
      }	else {
	cbstruct.parm = NULL;
	cbstruct.parm_length = 0;
	cbstruct.parm_format = 8;
	cbstruct.parm_type = None;
      }
    }
  }
  _XmProcessUnlock();

  if (cbstruct.event != NULL &&
      ((XSelectionRequestEvent *) cbstruct.event) -> requestor ==
      ((XSelectionRequestEvent *) cbstruct.event) -> owner) {
    cbstruct.flags |= XmCONVERTING_SAME;
  }

  _XmProcessLock();
  /* Reset bypass flag */
  local_convert_flag = 0;
  _XmProcessUnlock();

  if (*selection != atoms[XmA_MOTIF_DESTINATION] ||
      *target == atoms[XmA_MOTIF_LOSE_SELECTION]) {
    /* First we call any convert callbacks */

    if (XtHasCallbacks(wid, XmNconvertCallback) == XtCallbackHasSome)
      XtCallCallbacks(wid, XmNconvertCallback, &cbstruct);

    if (cbstruct.status == XmCONVERT_MORE)
      {
	/* Print error message,  and revert this flag to
	   XmCONVERT_DEFAULT */
	XmeWarning(wid, BAD_STATUS_MESSAGE);
	cbstruct.status = XmCONVERT_DEFAULT;
      }

    /* Now lookup the trait on this widget and call the
       internal routine */

    if (cbstruct.status == XmCONVERT_DEFAULT ||
	cbstruct.status == XmCONVERT_MERGE) {
      ttrait = (XmTransferTrait)
	XmeTraitGet((XtPointer) XtClass(wid), XmQTtransfer);
      if (ttrait != NULL)
	ttrait -> convertProc(wid, NULL, &cbstruct);
    }
  }

  /* If this is an INSERT_SELECTION or LINK_SELECTION then
     we call SecondaryConvertHandler to finish the work */
  if (cbstruct.status == XmCONVERT_DEFAULT &&
      (*target == atoms[XmAINSERT_SELECTION] ||
       *target == atoms[XmALINK_SELECTION]))
    SecondaryConvertHandler(wid, NULL, &cbstruct);

  /* Copy out the flags value for CLIPBOARD to use */
  cc -> flags = cbstruct.flags;

  if (cbstruct.status == XmCONVERT_DONE ||
      cbstruct.status == XmCONVERT_DEFAULT)
    {
      /* Copy out the data */
      *value = cbstruct.value;
      *size = cbstruct.length;
      *fmt = cbstruct.format;
      *type = cbstruct.type;
      return True;
    }
  else
    {
      *value = NULL;
      *size = 0;
      *fmt = 8;
      *type = None;
      return False;
    }
}

/****************************************************************/
/* DragConvertHandler acts as a wrapper to convert handler for	*/
/* drag and drop.  This is because drag and drop passes the     */
/* DragContext as the widget to the convert proc in the drag    */
/****************************************************************/

static Boolean
DragConvertHandler(Widget drag_context, Atom *selection, Atom *target,
		      Atom *type, XtPointer *value,
		      unsigned long *size, int *fmt)
{
  ConvertContext cc;
  Atom MOTIF_DROP = XInternAtom(XtDisplay(drag_context), XmS_MOTIF_DROP, False);

  /* Find the context block */

  cc = LookupContextBlock(XtDisplay(drag_context), MOTIF_DROP);

  /* Originating widget was stored in client_data */

  return(_XmConvertHandler((Widget) cc -> client_data,
			   selection, target,
			   type, value, size, fmt));

}

static int secondary_lock = 0;

/********************************************************************/
/* SecondaryConvertHandler handles the detail of                    */
/* Secondary selection.  This is because secondary is a synchronous */
/* mechanism which requires a spinlock.				    */
/********************************************************************/

/*ARGSUSED*/
static void
SecondaryConvertHandler(Widget w,
			XtPointer ignored, /* unused */
			XmConvertCallbackStruct *cs)
{
  enum { XmANULL, XmAINSERT_SELECTION, XmALINK_SELECTION, NUM_ATOMS };
  static char *atom_names[] = {XmSNULL, XmSINSERT_SELECTION, XmSLINK_SELECTION};

  XtAppContext app = XtWidgetToApplicationContext(w);
  _XmTextInsertPair *pair;
  XSelectionRequestEvent *req_event;
  static unsigned long old_serial = 0;
  Atom atoms[XtNumber(atom_names)];
  XtEnum operation;

  _XmProcessLock();
  if (secondary_lock != 0) {
    cs -> status = XmCONVERT_REFUSE;
    _XmProcessUnlock();
    return;
  }
  _XmProcessUnlock();

  req_event = XtGetSelectionRequest(w, cs -> selection, NULL);

  cs -> event = (XEvent *) req_event;

  _XmProcessLock();
  /* Work around for intrinsics selection bug */
  if (req_event != NULL && old_serial != req_event->serial)
    old_serial = req_event->serial;
  else {
    cs -> status = XmCONVERT_REFUSE;
    _XmProcessUnlock();
    return;
  }
  _XmProcessUnlock();

  if (cs -> parm_length == 0)
    {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }

  pair = (_XmTextInsertPair *) cs -> parm;

  _XmProcessLock();
  /* Lock */
  secondary_lock = 1;
  _XmProcessUnlock();

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (cs -> target == atoms[XmAINSERT_SELECTION])
    operation = XmCOPY;
  else if (cs -> target == atoms[XmALINK_SELECTION])
    operation = XmLINK;
  else
    operation = XmOTHER;

  if (_XmDestinationHandler(w, pair -> selection,
			    operation, ReleaseSecondaryLock,
			    (XtPointer) pair -> target,
			    req_event -> time, req_event) != True) {
    cs -> status = XmCONVERT_REFUSE;
    return;
  }

  /*
   * Make sure the above selection request is completed
   * before returning from the convert proc.
   */

#ifdef XTHREADS
  while (XtAppGetExitFlag(app) == False) {
#else
  for (;;) {
#endif
    XEvent event;
    XtInputMask mask;

    if (secondary_lock == 0) break;
#ifndef XTHREADS
    XtAppNextEvent(app, &event);
    XtDispatchEvent(&event);
#else
    while (!(mask = XtAppPending(app)))
	;  /* Busy waiting - so that we don't lose our lock */
    if (mask & XtIMXEvent) { /* We have an XEvent */
	/* Get the event since we know its there.
	 * Note that XtAppNextEvent would also process
	 * timers/alternate inputs.
	 */
	XtAppNextEvent(app, &event); /* no blocking */
	XtDispatchEvent(&event); /* Process it */
    }
    else /* not an XEvent, process it */
	XtAppProcessEvent(app, mask); /* non blocking */
#endif
  }

  cs -> value = NULL;
  cs -> type = atoms[XmANULL];
  cs -> format = 8;
  cs -> length = 0;
  cs -> status = XmCONVERT_DONE;
}

/*ARGSUSED*/
static void
ReleaseSecondaryLock(Widget w,	/* unused */
		     XtEnum a,	/* unused */
		     XmTransferDoneCallbackStruct *ts) /* unused */
{
  _XmProcessLock();
  secondary_lock = 0;
  _XmProcessUnlock();
}


/****************************************************************/
/* ClipboardLoseProc allows us to perform the delete operation  */
/* for the case where the user is performing a CUT to the       */
/* CLIPBOARD,  and the CLIPBOARD is owned by a			*/
/* CLIPBOARD_MANAGER.						*/
/****************************************************************/
static void
ClipboardLoseProc(Widget w, Atom *selection)
{
  Atom DELETE = XInternAtom(XtDisplay(w), XmSDELETE, False);
  XtPointer value;
  Atom type;
  unsigned long size;
  int fmt;

  _XmConvertHandlerSetLocal();
  _XmConvertHandler(w, selection, &DELETE,
		    &type, &value, &size, &fmt);

  LoseProc(w, selection);
}

/****************************************************************/
/* LoseProc is just a thin wrapper routine so			*/
/* we have a place to do bookkeeping.				*/
/****************************************************************/

static void
LoseProc(Widget w, Atom *selection)
{
  XtPointer value;
  Atom type;
  unsigned long size;
  int fmt;
  Atom MOTIF_LOSE = XInternAtom(XtDisplay(w), XmS_MOTIF_LOSE_SELECTION, False);

  _XmConvertHandlerSetLocal();
  _XmConvertHandler(w, selection, &MOTIF_LOSE,
		    &type, &value, &size, &fmt);
  XtFree((char*) value);
  XtRemoveCallback(w, XmNdestroyCallback, DisownCallback,
		   (XtPointer) *selection);
}

/********************************************************************/
/* This gets run to disown the selection if the widget is destroyed */
/********************************************************************/

/*ARGSUSED*/
static void
DisownCallback(Widget w,
	       XtPointer ignore, /* unused */
	       XtPointer client_data)
{
  Time time = XtLastTimestampProcessed(XtDisplay(w));

  XtDisownSelection(w, (Atom) client_data, time);
}


/****************************************************************/
/* XmeConvertMerge  merges the new data into the old return     */
/* value in the callback structure.				*/
/****************************************************************/
void
XmeConvertMerge(XtPointer data, Atom type, int format,
		unsigned long size, XmConvertCallbackStruct *cs)
{
  _XmProcessLock();
  if (cs -> status != XmCONVERT_MERGE) {
    TransferWarning(NULL, MERGE, ARG, BAD_STATUS);
    _XmProcessUnlock();
    return;
  }

  /* Merge the results.  Format and type better agree */
  if (format == cs -> format && type == cs -> type)
    {
      int total_size, offset, user_bytes;

      /* Calculate all sizes in bytes */
      offset = BYTELENGTH(cs -> length, cs -> format);
      user_bytes = BYTELENGTH(size, format);
      total_size = offset + user_bytes;
      /* Reallocate user data */
      cs-> value = (XtPointer) XtRealloc((char*) cs -> value, total_size);
      if (cs -> value == NULL) {
    	_XmProcessUnlock();
	return;
      }
      /* Copy widget data to cs.value from data */
      memcpy(&((char*) cs -> value)[offset], (char*) data, user_bytes);
      /* Add in the new size */
      cs -> length += size;
    }
  else /* If not,  print an error message */
    TransferWarning(NULL, MERGE, MATCH, BAD_MATCHED_CONVERT);
  _XmProcessUnlock();
}


/****************************************************************/
/* XmeTransferAddDoneProc(tid, done_proc)			*/
/* Adds a new done proc to the end of the current list of done  */
/* procs.  							*/
/****************************************************************/

void
XmeTransferAddDoneProc(XtPointer id,
		       XmSelectionFinishedProc done_proc)
{
  TransferContext tid = (TransferContext) id;

  _XmProcessLock();
  tid -> numDoneProcs++;

  if (tid -> numDoneProcs == 1)
    tid -> doneProcs = (XmSelectionFinishedProc *)
      XtMalloc(sizeof(XmSelectionFinishedProc*));
  else
    tid -> doneProcs = (XmSelectionFinishedProc *)
      XtRealloc((char*) tid -> doneProcs,
		sizeof(XmSelectionFinishedProc*) * tid -> numDoneProcs);

  tid -> doneProcs[tid -> numDoneProcs - 1] = done_proc;
  _XmProcessUnlock();
}


/****************************************************************/
/* XmePrimarySource(Widget, Time) 				*/
/* Owns the primary selection and sets up the appropriate       */
/* conversion handling						*/
/****************************************************************/

Boolean
XmePrimarySource(Widget w, Time time)
{
  return(XmeNamedSource(w, XA_PRIMARY, time));
}

/****************************************************************/
/* XmeNamedSource(Widget, Atom, Time) 				*/
/* Owns the named selection and sets up the appropriate         */
/* conversion handling						*/
/****************************************************************/

Boolean
XmeNamedSource(Widget w, Atom sel, Time time)
{
  Boolean status;

  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ClearContextBlock(XtDisplay(w), sel);

  if (time == 0) time = XtLastTimestampProcessed(XtDisplay(w));

  status = XtOwnSelection(w, sel, time, _XmConvertHandler,
			  LoseProc, NULL);

  if (status) XtAddCallback(w, XmNdestroyCallback, DisownCallback,
			    (XtPointer) sel);

  _XmAppUnlock(app);
  return(status);
}

/****************************************************************/
/* XmeSecondarySource(Widget, Op, Time) 			*/
/* Owns the secondary selection and sets up the appropriate     */
/* conversion handling.  This is the function to call when 	*/
/* starting the secondary selection.  				*/
/****************************************************************/

Boolean
XmeSecondarySource(Widget w, Time time)
{
  return(XmeNamedSource(w, XA_SECONDARY, time));
}

/****************************************************************/
/* XmeSecondaryTransfer(Widget, target, op, Time) 		*/
/* Triggers the actual secondary transfer by requesting the     */
/* target passed into XmeSecondarySource of the selection 	*/
/* _MOTIF_DESTINATION						*/
/****************************************************************/

void
XmeSecondaryTransfer(Widget w, Atom target, XtEnum op, Time time)
{
  enum { XmA_MOTIF_DESTINATION, XmAINSERT_SELECTION,
	 XmALINK_SELECTION, XmAATOM_PAIR, NUM_ATOMS };
  static char *atom_names[] = { XmS_MOTIF_DESTINATION, XmSINSERT_SELECTION,
	 XmSLINK_SELECTION, XmIATOM_PAIR };

  ConvertContext cc;
  _XmTextInsertPair pair;
  Atom transfer_target;
  Atom atoms[XtNumber(atom_names)];
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  cc = LookupContextBlock(XtDisplay(w), XA_SECONDARY);
  cc -> op = op;

  if (op == XmLINK)
    transfer_target = atoms[XmALINK_SELECTION];
  else
    transfer_target = atoms[XmAINSERT_SELECTION];

  pair.selection = XA_SECONDARY;
  pair.target = target;

  XtSetSelectionParameters(w, atoms[XmA_MOTIF_DESTINATION], atoms[XmAATOM_PAIR],
			   (XtPointer) &pair, 2, 32);
  XtGetSelectionValue(w, atoms[XmA_MOTIF_DESTINATION], transfer_target,
		      SecondaryDone, NULL, time);
  _XmAppUnlock(app);
}

/*****************************************************************/
/* SecondaryDone deals with the completion of the                */
/* convert selection request started in XmeSecondaryTransfer     */
/* This also callback to a widget supplied routine to deal with  */
/* issues surrounding the success or failure of the transfer     */
/*****************************************************************/
/*ARGSUSED*/
static void
SecondaryDone(Widget wid,
	      XtPointer client_data, /* unused */
	      Atom *selection,	/* unused */
	      Atom *type, XtPointer value,
	      unsigned long *length, int *format)
{
  ConvertContext cc;
  Boolean success;
  Atom convert_selection;
  Atom DELETE = XInternAtom(XtDisplay(wid), XmSDELETE, False);

  cc = LookupContextBlock(XtDisplay(wid), XA_SECONDARY);

  if (*type == None && *length == 0 && value == NULL)
    success = False;
  else
    success = True;

  convert_selection = XA_SECONDARY;
  /* Call the convertCallback with target DELETE if successful */
  if (success && cc -> op == XmMOVE) {
    _XmConvertHandlerSetLocal();
    _XmConvertHandler(wid, &convert_selection,
		      &DELETE,
		      type, (XtPointer*) &value, length, format);
    XtFree((char*) value);
  }

  XtDisownSelection(wid, convert_selection,
		    XtLastTimestampProcessed(XtDisplay(wid)));
}



/****************************************************************/
/* XmeClipboardSource(Widget, Op, Time)				*/
/* Puts data onto the clipboard.  				*/
/****************************************************************/

Boolean
XmeClipboardSource(Widget w, XtEnum op, Time time)
{
  enum { XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS,
	 XmA_MOTIF_CLIPBOARD_TARGETS, XmACLIPBOARD,
	 XmACLIPBOARD_MANAGER, XmA_MOTIF_SNAPSHOT, XmADELETE, NUM_ATOMS };
  static char *atom_names[] = { XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS,
	 XmS_MOTIF_CLIPBOARD_TARGETS, XmSCLIPBOARD,
	 XmSCLIPBOARD_MANAGER, XmS_MOTIF_SNAPSHOT, XmSDELETE };

  ConvertContext cc;
  Atom type, type2, *targets;
  XtPointer value;
  unsigned long size, size2;
  int format, format2;
  int i, count, status, transferred = 0;
  Display *display;
  long itemid;
  char *name;
  FreeType howFree;
  Atom atoms[XtNumber(atom_names)];
  Window clipboard_owner;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  display = XtDisplay(w);

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

  if (time == 0) time = XtLastTimestampProcessed(display);

  ClearContextBlock(display, atoms[XmACLIPBOARD]);

  cc = LookupContextBlock(display, atoms[XmACLIPBOARD]);
  cc -> op = op;

  /* If there is a clipboard manager,  then just take
     ownership of the clipboard selection and the manager
     will do the rest.  Otherwise we use the motif clipboard
     code. */
  clipboard_owner = XGetSelectionOwner(display, atoms[XmACLIPBOARD_MANAGER]);
  if (clipboard_owner != None) {
    if (op == XmMOVE) {
      /* We call a special lose proc for move which will call
	 _XmConvertHandler to delete the selection */
      status = XtOwnSelection(w, atoms[XmACLIPBOARD], time,
			      _XmConvertHandler, ClipboardLoseProc, NULL);
    } else {
      status = XtOwnSelection(w, atoms[XmACLIPBOARD], time,
			      _XmConvertHandler, LoseProc, NULL);
    }

    if (status)
    {
      XtAddCallback(w, XmNdestroyCallback, DisownCallback,
			      (XtPointer) atoms[XmACLIPBOARD]);
    } else {
      _XmAppUnlock(app);
      return True;
    }
  }

  /* Use Motif clipboard */
  status = XmClipboardStartCopy(display, XtWindow(w), NULL,
				time, w, ClipboardCallback, &itemid);

  if (status == XmClipboardLocked) {
	_XmAppUnlock(app);
	return(False);
  }

  /* OK.  We've got the clipboard, now setup the targets items */
  cc -> itemid = itemid;

  /* Call the converter to get the targets for the clipboard
     if _MOTIF_CLIPBOARD_TARGETS doesn't work then try
     TARGETS target */
  _XmConvertHandlerSetLocal();
  if (_XmConvertHandler(w, &atoms[XmACLIPBOARD],
			&atoms[XmA_MOTIF_CLIPBOARD_TARGETS],
			&type, &value, &size, &format) == True &&
      size != 0 &&
      type == XA_ATOM) {
    targets = (Atom *) value;
    count = size;
    /* For each item we register the format, ask to convert it
       and if it is converted we put it on the clipboard */
    for(i = 0; i < count; i++) {
      name = GetSafeAtomName(display, targets[i], &howFree);
      _XmConvertHandlerSetLocal();
      if (_XmConvertHandler(w, &atoms[XmACLIPBOARD], &targets[i],
			    &type2, &value, &size2, &format2) == True &&
	  ! (cc -> flags & XmCONVERTING_PARTIAL)) {
	XmClipboardRegisterFormat(display, name, format2);
	/* format must be 8, 16 or 32. */
	size2 = BYTELENGTH(size2, format2);
	transferred++;
	/* Critical section for MT */
	_XmProcessLock();
	_XmClipboardPassType(type2);
	XmClipboardCopy(display, XtWindow(w), itemid, name, value, size2, 0, 0);
	_XmProcessUnlock();
	/* End Critical section for MT */
      }
      XtFree((char*) value);
      FreeSafeAtomName(name,howFree);
    }
    XtFree((char*) targets);
  }

  /* Call the converter to get the deferred targets for the
     clipboard */
  _XmConvertHandlerSetLocal();
  if( _XmConvertHandler(w, &atoms[XmACLIPBOARD],
			&atoms[XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS],
			&type, &value, &size, &format) == True &&
      size != 0 &&
      type == XA_ATOM) {
    _XmProcessLock();
    if (DataIdDictionary == NULL) {
      /* Create dictionary which stores data about particular
	 snapshots and particular dataids.  Since it
	 takes an integer as a key,  don't need match or hash
	 functions */
      DataIdDictionary = _XmAllocHashTable(10, NULL, NULL);
    }
    _XmProcessUnlock();

    targets = (Atom *) value;
    count = size;
    /* If there are deferred targets then the snapshot target
       must be converted successfully.  The value returned
       by snapshot will be used as a unique id in the
       clipboard callback to identify this deferred data
       item */
    _XmConvertHandlerSetLocal();
    if (_XmConvertHandler(w, &atoms[XmACLIPBOARD], &atoms[XmA_MOTIF_SNAPSHOT],
			  &type2, &value, &size2, &format2) == True) {
      long data_id;
      SnapshotRequest req;

      if (count != 0) {
	req = (SnapshotRequest) XtMalloc(sizeof(SnapshotRequestRec));
	req -> outstanding = 0;
	req -> distinguisher = * (Atom *) value;
      } else {
	req = NULL;
      }

      XtFree((char*) value);

      for(i = 0; i < count; i++) {
	name = GetSafeAtomName(display, targets[i], &howFree);
	transferred++;
	/* Critical section for MT */
	_XmProcessLock();
	_XmClipboardPassType(type2);
	XmClipboardCopy(display, XtWindow(w), itemid, name,
			NULL, 0, targets[i], &data_id);
	_XmProcessUnlock();
	/* End Critical section for MT */
	/* Associate the data_id with this snapshot and increment
	   the number of requests outstanding */
	_XmProcessLock();
	_XmAddHashEntry(DataIdDictionary, (XmHashKey)data_id, (XtPointer)req);
	_XmProcessUnlock();
	req -> outstanding++;
        FreeSafeAtomName(name,howFree);
      }
    }
    XtFree((char*) targets);
  }
  XmClipboardEndCopy(display, XtWindow(w), itemid);

  if (op == XmMOVE && transferred != 0) {
    _XmConvertHandlerSetLocal();
    _XmConvertHandler(w, &atoms[XmACLIPBOARD], &atoms[XmADELETE],
		      &type, &value, &size, &format);
    XtFree((char*) value);
  }

  if (transferred != 0) {
    _XmAppUnlock(app);
    return(True);
  }
  else {
    _XmAppUnlock(app);
    return(False);
  }
}

static void
ClipboardCallback(Widget w, long *data_id, long *target, int *reason)
{
  XtPointer value;
  Atom type;
  unsigned long size;
  int format;
  Display *display;
  Atom CLIPBOARD = XInternAtom(XtDisplay(w), XmSCLIPBOARD, False);
  SnapshotRequest req;
  ConvertContext cc;

  cc = LookupContextBlock(XtDisplay(w), CLIPBOARD);

  _XmProcessLock();
  req = (SnapshotRequest) _XmGetHashEntry(DataIdDictionary,
					  (XmHashKey) *data_id);
  /* Decrement count and remove this association */
  req -> outstanding--;
  _XmRemoveHashEntry(DataIdDictionary, (XtPointer)data_id);
  _XmProcessUnlock();

  display = XtDisplay(w);

  if (*reason != XmCR_CLIPBOARD_DATA_DELETE) {
    _XmConvertHandlerSetLocal();
    if (_XmConvertHandler(w, &req -> distinguisher, (Atom *) target,
			  &type, &value, &size, &format) == True &&
	! (cc -> flags & XmCONVERTING_PARTIAL)) {
      char *name;
      FreeType howFree;

      size = BYTELENGTH( size, format );
      if (format % 8 != 0) size++;

      name = GetSafeAtomName(display, * (Atom *) target, &howFree);
      XmClipboardRegisterFormat(display, name, format);
      FreeSafeAtomName(name,howFree);
      /* Critical section for MT */
      _XmProcessLock();
      _XmClipboardPassType(type);
      XmClipboardCopyByName(display, XtWindow(w), *data_id,
			    value, size, 0L);
      _XmProcessUnlock();
      XtFree((char*) value);
    }
    else
      XmClipboardCopyByName(display, XtWindow(w), *data_id,
			    NULL, 0, 0L);
  }

  if (req -> outstanding == 0) {
    Atom done = XInternAtom(display, XmIDONE, False);

    /* If this was the last item,  call _XmConvertHandler with
       DELETE on the distinguisher and then free the req */
    _XmConvertHandlerSetLocal();
    _XmConvertHandler(w, &req -> distinguisher,
		      (Atom *) &done, &type, &value, &size, &format);
    XtFree((char*) value);
    XtFree((char*) req);
  }
}

/****************************************************************/
/* XmeDragSource						*/
/* Sets up for drag and drop and calls XmDragStart		*/
/****************************************************************/

Widget
XmeDragSource(Widget w, XtPointer location_data, XEvent *event,
	      ArgList in_args, Cardinal in_arg_count)
{
  enum { XmA_MOTIF_DROP, XmA_MOTIF_EXPORT_TARGETS, NUM_ATOMS };
  static char *atom_names[] = { XmS_MOTIF_DROP, XmS_MOTIF_EXPORT_TARGETS };

  Arg *args;
  int arg_count;
  XtPointer targets;
  unsigned long size;
  Atom type;
  int format;
  Widget dragContext;
  ConvertContext cc;
  Atom atoms[XtNumber(atom_names)];
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  /* merge and copy arg list */
  arg_count = in_arg_count + 10;
  args = (Arg *) XtMalloc(sizeof(Arg) * arg_count);
  for(arg_count = 0; arg_count < in_arg_count; arg_count++)
    args[arg_count] = in_args[arg_count];

  arg_count = in_arg_count;

  ClearContextBlock(XtDisplay(w), atoms[XmA_MOTIF_DROP]);

  cc = LookupContextBlock(XtDisplay(w), atoms[XmA_MOTIF_DROP]);

  cc -> location_data = location_data;
  cc -> client_data = (XtPointer) w;

  XtSetArg(args[arg_count], XmNconvertProc, DragConvertHandler);
  arg_count++;

  _XmConvertHandlerSetLocal();
  if (_XmConvertHandler(w, &atoms[XmA_MOTIF_DROP],
			&atoms[XmA_MOTIF_EXPORT_TARGETS],
			&type, &targets, &size, &format) == True) {
    XtSetArg(args[arg_count], XmNexportTargets, targets); arg_count++;
    XtSetArg(args[arg_count], XmNnumExportTargets, size); arg_count++;
  } else {
    /* Free copied arguments */
    XtFree((char*) args);
    XtFree((char*) targets);
    _XmAppUnlock(app);
    return(NULL);
  }

  XtSetArg(args[arg_count], XmNclientData, location_data); arg_count++;

  dragContext = XmDragStart(w, event, args, arg_count);
  cc -> drag_context = dragContext;

  /* Free copied arguments */
  XtFree((char*) args);
  XtFree((char*) targets);

  _XmAppUnlock(app);
  return(dragContext);
}


/****************************************************************/
/* Destination section						*/
/* 								*/
/****************************************************************/

/* internal flag for transfer block setup */
static int TB_internal = 0;

Boolean
_XmDestinationHandler(Widget wid, Atom selection, XtEnum op,
		      XmSelectionFinishedProc done_proc,
		      XtPointer location_data, Time time,
		      XSelectionRequestEvent *event)
{
  Window selection_owner;
  TransferContext tc;
  XmDestinationCallbackStruct *cbstruct;
  XmTransferTrait ttrait;
  Atom MOTIF_DROP = XInternAtom(XtDisplay(wid), XmS_MOTIF_DROP, False);

  cbstruct = (XmDestinationCallbackStruct *)
    XtMalloc(sizeof(XmDestinationCallbackStruct));

  cbstruct -> reason = XmCR_OK;
  cbstruct -> event = (XEvent *) event;
  cbstruct -> selection = selection;
  cbstruct -> flags = 0;
  cbstruct -> operation = op;
  cbstruct -> location_data = location_data;
  cbstruct -> destination_data = NULL;
  cbstruct -> time = time;

  /* Setup transfer */
  cbstruct -> transfer_id = (XtPointer) GetTransferID();
  tc = (TransferContext) cbstruct -> transfer_id;
  tc -> widget = wid;
  tc -> numDoneProcs = 0;
  tc -> doneProcs = NULL;
  tc -> auto_proc = (XtCallbackProc) NULL;
  tc -> status = XmTRANSFER_DONE_DEFAULT;
  tc -> flags = TC_NONE;
  tc -> selection = selection;
  tc -> real_selection = selection;
  tc -> op = op;
  tc -> client_data = NULL;
  tc -> drop_context = (Widget) NULL;
  tc -> drag_context = (Widget) NULL;
  tc -> callback_struct = cbstruct;

  if (done_proc != NULL)
    XmeTransferAddDoneProc((XtPointer) tc, done_proc);

  ttrait = (XmTransferTrait)
    XmeTraitGet((XtPointer) XtClass(wid), XmQTtransfer);

  if (tc -> selection == MOTIF_DROP) {
    /* We pass the drop callback struct in through location_data */
    XmDropProcCallbackStruct *ds = (XmDropProcCallbackStruct *) location_data;
    XtPointer malloc_ds;
    int i;
    Arg args[1];

    malloc_ds = XtMalloc(sizeof(XmDropProcCallbackStruct));
    memcpy(malloc_ds, ds, sizeof(XmDropProcCallbackStruct));
    location_data = malloc_ds;
    XmeTransferAddDoneProc((XtPointer) tc, DeleteDropCBStruct);
    tc -> drag_context = ds -> dragContext;

    i = 0;
    XtSetArg(args[i], XmNiccHandle, &tc -> real_selection); i++;
    XtGetValues(ds->dragContext, args, i);

    /* If this is a drop,  we need to do more to figure out who
       really owns the selection */
    selection_owner = XGetSelectionOwner(XtDisplay(wid), tc -> real_selection);
    if (XtWindowToWidget(XtDisplay(wid), selection_owner) != (Widget) NULL) {
      ConvertContext cc;
      cc = LookupContextBlock(XtDisplay(wid), MOTIF_DROP);

      /* We go get the origination information if this is in the same
	 client as the originator.  We know its the same client if
	 XtWindowToWidget is successful */
      if (cc -> client_data == (XtPointer) wid)
	cbstruct -> flags |= XmCONVERTING_SAME;
    }

    /* We pass this callback struct on through destination_data
       and get new data from the widget trait for location_data */
    cbstruct -> destination_data = location_data;
    cbstruct -> location_data = NULL;
  } else { /* Not D&D */
    /* For regular selections,  we can just use this info,  otherwise
       we need to get the real selection atom for D&D */
    selection_owner = XGetSelectionOwner(XtDisplay(wid), selection);
    if (selection_owner == XtWindow(wid))
      cbstruct -> flags |= XmCONVERTING_SAME;
  }

  /* Call the prehook to allow the widget to setup information.  This
     is currently only envisioned to be useful in D&D */
  if (ttrait != NULL && ttrait -> destinationPreHookProc != NULL)
    ttrait -> destinationPreHookProc(wid, NULL, cbstruct);

    /* First we call any destination callbacks */
  if (XtHasCallbacks(wid, XmNdestinationCallback) == XtCallbackHasSome)
    XtCallCallbacks(wid, XmNdestinationCallback, cbstruct);

  tc -> flags |= TC_CALLED_CALLBACKS;

  /* Now lookup the trait on this widget and call the
     internal routine if there were no transfers via the
     destination callbacks (or no destination callbacks) or
     there were transfers and the user set the status to
     DEFAULT and has finished (if it hasn't finished we'll
     handle this in SelectionCallbackWrapper) */
  if (ttrait != NULL &&
      tc -> status == XmTRANSFER_DONE_DEFAULT &&
      ((tc -> count == 0) ||
       (tc -> outstanding == 0 &&
	! (TC_CALLED_WIDGET)))) {
    _XmProcessLock();
    TB_internal = 1;
    _XmProcessUnlock();
    tc -> flags |= TC_CALLED_WIDGET;
    if (ttrait -> destinationProc != 0)
      ttrait -> destinationProc(wid, NULL, cbstruct);
    _XmProcessLock();
    TB_internal = 0;
    _XmProcessUnlock();
  }

  if (tc -> count == 0 &&
      tc -> selection == MOTIF_DROP) {
    XmDropProcCallbackStruct *ds = (XmDropProcCallbackStruct *) location_data;

    if (ds -> dropAction == XmDROP_HELP) {
      /* If a drop help occured, then we do not want to cleanup yet,
	 despite there being no transfers.  Right at this moment,
	 the user is deciding whether to accept or cancel the drop
	 based on the presented dialog. */
      tc -> flags |= TC_EXITED_DH; /* Indicate safe to free record */
      /* Exit immediately to avoid freeing the transfer block below */
      return(True);
    } else {
      /* If no transfers occurred,  and this is a drop,
	 then it failed */
      Arg args[2];
      XtSetArg(args[0], XmNtransferStatus, XmTRANSFER_FAILURE);
      XtSetArg(args[1], XmNnumDropTransfers, 0);
      XmDropTransferStart(tc -> drag_context, args, 2);
    }
  }

  /* If we either have performed no transfers or we are finished
     with the transfers,  go finish the work */
  if (tc -> count == 0 || tc -> outstanding == 0)
    {
      FinishTransfer(wid, tc);
      return(True);
    }
  else {
    /* Otherwise set the flag so SelectionCallbackWrapper can
       finish the work */
    tc -> flags |= TC_EXITED_DH; /* Indicate safe to free record */
    return(True);
  }
}

static void
FinishTransfer(Widget wid, TransferContext tc)
{
  XmTransferDoneCallbackStruct ts;

  tc -> flags |= TC_FLUSHED;  /* Ignore any future requests */
  ts.reason = XmCR_OK;
  ts.event = (XEvent *) NULL;
  ts.selection = tc -> selection;
  ts.transfer_id = (XtPointer) tc;

  if (tc -> status == XmTRANSFER_DONE_FAIL)
    ts.status = XmTRANSFER_DONE_FAIL;
  else
    ts.status = XmTRANSFER_DONE_SUCCEED;

  /* Override if no transfers have occurred */
  if (tc -> count == 0) ts.status = XmTRANSFER_DONE_FAIL;

  ts.client_data = tc -> client_data;

  CallDoneProcs(wid, tc, &ts);
  XtFree((char*) tc -> callback_struct);
  FreeTransferID(tc);
}

/****************************************************************/
/* DropDestinationHandler acts as a wrapper to the destination  */
/* handler for drag and drop.  This is because drag and drop    */
/* passes the DragContext as the widget to the destination proc */
/* in the drop							*/
/****************************************************************/

/*ARGSUSED*/
static void
DeleteDropCBStruct(Widget w,	/* unused */
		   XtEnum ignored_status, /* unused */
		   XmTransferDoneCallbackStruct *cs)
{
  TransferContext tc = (TransferContext) cs -> transfer_id;

  /* The malloc'd structure is in the destination_data member */
  XtFree((char*) tc -> callback_struct -> destination_data);
}

/*ARGSUSED*/
static void
DropDestinationHandler(Widget w,
		       XtPointer client_data, /* unused */
		       XmDropProcCallbackStruct *ds)
{
  Atom MOTIF_DROP = XInternAtom(XtDisplay(w), XmS_MOTIF_DROP, False);
  XtEnum op;

  if (ds -> dropAction == XmDROP_HELP ||
      ds -> operation == XmDROP_NOOP)
    op = XmOTHER;
  else
    op = ds -> operation;

  (void) _XmDestinationHandler(w, MOTIF_DROP, op, NULL,
			       (XtPointer) ds, ds -> timeStamp, NULL);
}


/*************************************************************/
/* XmePrimarySink begins a transfer for the contents of the  */
/* XA_PRIMARY selection.                                     */
/*************************************************************/
Boolean
XmePrimarySink(Widget w, XtEnum op, XtPointer location_data, Time time)
{
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = _XmDestinationHandler(w, XA_PRIMARY, op, NULL,
			       location_data, time, NULL);
  _XmAppUnlock(app);
  return ret_val;
}

/*************************************************************/
/* XmeNamedSink begins a transfer for the contents of the    */
/* named selection.                                          */
/*************************************************************/
Boolean
XmeNamedSink(Widget w, Atom sel, XtEnum op, XtPointer location_data, Time time)
{
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = _XmDestinationHandler(w, sel, op, NULL,
			       location_data, time, NULL);
  _XmAppUnlock(app);
  return ret_val;
}

/*************************************************************/
/* XmeSecondarySink takes ownership of the MOTIF_DESTINATION */
/* selection.                                                */
/*************************************************************/
Boolean
XmeSecondarySink(Widget w, Time time)
{
  Boolean status;
  Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(w), XmS_MOTIF_DESTINATION, False);
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ClearContextBlock(XtDisplay(w), MOTIF_DESTINATION);

  if (time == 0) time = XtLastTimestampProcessed(XtDisplay(w));

  /* Setup our end of the secondary selection */

  status = XtOwnSelection(w, MOTIF_DESTINATION, time,
			  _XmConvertHandler, LoseProc, NULL);

  if (status) XtAddCallback(w, XmNdestroyCallback, DisownCallback,
			    (XtPointer) MOTIF_DESTINATION);

  _XmAppUnlock(app);
  return(status);
}

/**************************************************************/
/* XmeClipboardSink begins a transfer for the contents of the */
/* CLIPBOARD selection.                                       */
/**************************************************************/
Boolean
XmeClipboardSink(Widget w, XtEnum op, XtPointer location_data)
{
  Atom CLIPBOARD = XInternAtom(XtDisplay(w), XmSCLIPBOARD, False);
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = _XmDestinationHandler(w, CLIPBOARD, op,
			       NULL, location_data, 0, NULL);
  _XmAppUnlock(app);
  return ret_val;
}

/*************************************************************/
/* XmeDropSink creates a drop site that will use the         */
/* destinationCallbacks to handle drops.                     */
/*************************************************************/
void
XmeDropSink(Widget w, ArgList in_args, Cardinal in_arg_count)
{
  Arg *args;
  int arg_count;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* merge and copy arg list */
  arg_count = in_arg_count + 2;
  args = (Arg *) XtMalloc(sizeof(Arg) * arg_count);
  for(arg_count = 0; arg_count < in_arg_count; arg_count++)
    args[arg_count] = in_args[arg_count];

  arg_count = in_arg_count;

  XtSetArg(args[arg_count], XmNdropProc, DropDestinationHandler);
  arg_count++;

  XmDropSiteRegister(w, args, arg_count);

  XtFree((char*) args);
  _XmAppUnlock(app);
}


/*************************************************************/
/* Transfer routine section                                  */
/*************************************************************/

/************************************************************************/
/* XmTransferDone allows the user to control the transfer queue		*/
/* If the user calls XmTransferDone with a status of DEFAULT then	*/
/* all remaining non-internal transfers are ignored and the widget's    */
/* internal transfers are done.   SUCCEED, FAIL or CONTINUE cause the   */
/* entire queue to be flushed,  and the appropriate status to be set.   */
/************************************************************************/
void
XmTransferDone(XtPointer transfer_id, XmTransferStatus status)
{
  TransferContext tc = (TransferContext) transfer_id;
  Atom MOTIF_DROP = XInternAtom(XtDisplay(tc -> widget), XmS_MOTIF_DROP, False);
  _XmWidgetToAppContext(tc->widget);

  _XmAppLock(app);
  tc -> status = status;

  /* Make sure MULTIPLE request is unblocked */
  if (tc -> flags & TC_IN_MULTIPLE) {
    tc -> flags &= ~ TC_IN_MULTIPLE;
    XtSendSelectionRequest(tc -> widget, tc -> selection,
			   XtLastTimestampProcessed(XtDisplay(tc -> widget)));
  }

  if (status == XmTRANSFER_DONE_SUCCEED ||
      status == XmTRANSFER_DONE_FAIL ||
      status == XmTRANSFER_DONE_CONTINUE)
    {
      tc -> flags |= TC_FLUSHED;

      if (status == XmTRANSFER_DONE_FAIL &&
	  tc -> selection == MOTIF_DROP) {
	Arg args[2];
	XtSetArg(args[0], XmNtransferStatus, XmTRANSFER_FAILURE);
	XtSetArg(args[1], XmNnumDropTransfers, 0);
	if (tc -> drop_context != (Widget) NULL)
	  XtSetValues(tc -> drop_context, args, 2);
	else
	  XmDropTransferStart(tc -> drag_context, args, 2);

	/* Also,  if there are no transfers,  and we have exited
	   _XmDestinationHandler,  we must cleanup the transfer
	   infomation here as SelectionCallbackWrapper,  where the
	   data is normally freed, won't be called */
	if (tc -> count == 0 &&
	    tc -> flags & TC_EXITED_DH ) {
	  FinishTransfer(tc -> widget, tc);
	}
      }
    }
  else if (status == XmTRANSFER_DONE_DEFAULT)
    {
      TransferBlock tb;

      /* If we are going to default then we'll skip
	 all requests placed by callbacks */
      for(tb = tc -> requests;
	  tb != NULL;
	  tb = (TransferBlock) tb -> next) {
	if (!(tb -> flags & TB_INTERNAL))
	  tb -> flags = tb -> flags | TB_IGNORE;
      }
    }
  _XmAppUnlock(app);
}

/************************************************************************/
/* XmTransferSetParameters defines a set of parameters to be passed     */
/* with the next call to XmTransferValue.				*/
/************************************************************************/

void
XmTransferSetParameters(XtPointer transfer_id,
			XtPointer parm,
			int parm_fmt,
			unsigned long parm_length,
			Atom parm_type)
{
  TransferContext tc = (TransferContext) transfer_id;
  _XmWidgetToAppContext(tc->widget);

  _XmAppLock(app);
  /******************************************************/
  /* Return if we already finished this transfer set    */
  /* The problem is that if the flags are set then we   */
  /* are about to delete the transferContext in         */
  /* SelectionCallbackWrapper()                         */
  /******************************************************/
  if (tc -> flags & TC_FLUSHED) {
	_XmAppUnlock(app);
	return;
  }

  if (parm_fmt == 0) parm_fmt = 8;

  if (parm != NULL)
    XtSetSelectionParameters(tc -> widget, tc -> real_selection,
			     parm_type, parm, parm_length, parm_fmt);
  _XmAppUnlock(app);
}

/************************************************************************/
/* XmTransferValue allows the user to get data from the owner of the    */
/* selection for which we started the destination callback.		*/
/* This takes care of the small details for getting the data from 	*/
/* either the selection mechanism (user, PRIMARY, SECONDARY, or		*/
/* CLIPBOARD) or from another mechanism (Drag and Drop).		*/
/************************************************************************/

void
XmTransferValue(XtPointer transfer_id,
		Atom target,
		XtCallbackProc proc,
		XtPointer client_data,
		Time time)
{
  enum { XmACLIPBOARD, XmA_MOTIF_DROP, NUM_ATOMS };
  static char *atom_names[] = { XmSCLIPBOARD, XmS_MOTIF_DROP };

  TransferContext tc = (TransferContext) transfer_id;
  TransferBlock tb;
  unsigned long length;
  Atom atoms[XtNumber(atom_names)];
  _XmWidgetToAppContext(tc->widget);

  _XmAppLock(app);
  /******************************************************/
  /* Return if we already finished this transfer set    */
  /* The problem is that if the flags are set then we   */
  /* are about to delete the transferContext in         */
  /* SelectionCallbackWrapper()                         */
  /******************************************************/
  if (tc -> flags & TC_FLUSHED) {
	_XmAppUnlock(app);
	return;
  }

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(tc -> widget), atom_names, XtNumber(atom_names),
	       False, atoms);

  if (time == 0)
    time = XtLastTimestampProcessed(XtDisplay(tc -> widget));

  tb = AddTransferBlock(tc);

  tb -> client_data = client_data;
  tb -> selection_proc = proc;
  tb -> target = target;
  tb -> location_data = NULL;

  tc -> outstanding++;
  tc -> count++;

  if (tc -> selection == atoms[XmACLIPBOARD]) {
    /* Assure the clipboard is owned to prevent orphan data
       problems in the data transfer */
    XmClipboardInquireLength(XtDisplay(tc -> widget),
			     XtWindow(tc -> widget),
			     XmSTARGETS,
			     &length);
  }

  if (tc -> selection != atoms[XmA_MOTIF_DROP])
    {
      XtGetSelectionValue(tc -> widget, tc -> real_selection, target,
			  SelectionCallbackWrapper, (XtPointer)tc, time);
    }
  else
    {
      XmDropTransferEntryRec transfers[1];

      transfers[0].client_data = (XtPointer) tc;
      transfers[0].target = tb -> target;
      if (tc -> drop_context == NULL)
	{
	  Arg args[5];

	  XtSetArg(args[0], XmNdropTransfers, transfers);
	  XtSetArg(args[1], XmNnumDropTransfers, 1);
	  XtSetArg(args[2], XmNtransferProc, SelectionCallbackWrapper);
	  tc -> drop_context =
	    (Widget) XmDropTransferStart(tc -> drag_context, args, 3);
	}
      else
	XmDropTransferAdd(tc -> drop_context, transfers, 1);
    }
  _XmAppUnlock(app);
}


/************************************************************************/
/* XmTransferStartRequest and XmTransferSendRequest bracket a MULTIPLE	*/
/* request.  In between the user calls XmTransferSetParameters and 	*/
/* XmTransferValue to arrange requests.					*/
/************************************************************************/
void
XmTransferStartRequest(XtPointer transfer_id)
{
  TransferContext tc = (TransferContext) transfer_id;
  _XmWidgetToAppContext(tc->widget);

  _XmAppLock(app);
  /******************************************************/
  /* Return if we already finished this transfer set    */
  /* The problem is that if the flags are set then we   */
  /* are about to delete the transferContext in         */
  /* SelectionCallbackWrapper()                         */
  /******************************************************/
  if (tc -> flags & TC_FLUSHED) {
	_XmAppUnlock(app);
	return;
  }

  if (tc -> flags & TC_IN_MULTIPLE) {
    char *sel;
    FreeType howFree;

    sel = GetSafeAtomName(XtDisplay(tc -> widget), tc -> selection, &howFree);
    /* Already doing a multiple */
    TransferWarning(tc->widget, START_MULTIPLE,
		    sel,
		    ERROR_MULTIPLE_IN_PROGRESS);
    FreeSafeAtomName(sel,howFree);
    _XmAppUnlock(app);
    return;
  }

  tc -> flags |= TC_IN_MULTIPLE;

  XtCreateSelectionRequest(tc -> widget, tc -> real_selection);
  _XmAppUnlock(app);
}

void
XmTransferSendRequest(XtPointer transfer_id, Time time)
{
  TransferContext tc = (TransferContext) transfer_id;
  _XmWidgetToAppContext(tc->widget);

  _XmAppLock(app);
  /******************************************************/
  /* Return if we already finished this transfer set    */
  /* The problem is that if the flags are set then we   */
  /* are about to delete the transferContext in         */
  /* SelectionCallbackWrapper()                         */
  /******************************************************/
  if (tc -> flags & TC_FLUSHED) {
    /* Assume that cleanup would be appropriate here */
    XtCancelSelectionRequest(tc -> widget, tc -> real_selection);
    _XmAppUnlock(app);
    return;
  }

  if (! (tc -> flags & TC_IN_MULTIPLE)) {
    char     *sel;
    FreeType howFree;

    sel = GetSafeAtomName(XtDisplay(tc -> widget), tc -> selection, &howFree);
    /* Not doing a multiple */
    TransferWarning(tc->widget, END_MULTIPLE,
		    sel,
		    ERROR_MULTIPLE_NOT_IN_PROGRESS);
    FreeSafeAtomName(sel, howFree);
    _XmAppUnlock(app);
    return;
  }

  tc -> flags &= ~ TC_IN_MULTIPLE;

  if (time == 0) time = XtLastTimestampProcessed(XtDisplay(tc -> widget));

  XtSendSelectionRequest(tc -> widget, tc -> real_selection, time);
  _XmAppUnlock(app);
}

/************************************************************************/
/* SelectionCallbackWrapper does the bookeeping for the transfer 	*/
/* routines and makes sure that the TransferContext is deleted when 	*/
/* there are no more outstanding transfers.				*/
/************************************************************************/
static void
SelectionCallbackWrapper(Widget wid, XtPointer client_data,
			 Atom *selection, Atom *type,
			 XtPointer value, unsigned long *length,
			 int *format)
{
  enum { XmA_MOTIF_DROP, XmADELETE, NUM_ATOMS };
  static char *atom_names[] = { XmS_MOTIF_DROP, XmSDELETE };

  XmSelectionCallbackStruct cbstruct;
  TransferContext tc = (TransferContext) client_data;
  TransferBlock tb = tc -> requests;
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(wid), atom_names, XtNumber(atom_names), False, atoms);

  /* Get the real widget if this is a drop transfer */
  if (tc -> selection == atoms[XmA_MOTIF_DROP])
    wid = tc -> widget;

  if (tc -> outstanding == 0) {
    XmeWarning(wid, BAD_SCB_MESSAGE);
    return;
  }

  if (tb != NULL) {
    /* Unchain this transfer block */
    tc -> requests = (TransferBlock) tb -> next;
    /* If this is the last block then reset last */
    if (tc -> last == tb) tc -> last = NULL;
  }

  if (! (tc -> flags & TC_FLUSHED)) {
    if (tb != NULL &&
	! (tb -> flags & TB_IGNORE)) {
      cbstruct.reason = XmCR_OK;
      cbstruct.event = (XEvent *) NULL;
      cbstruct.selection = *selection;
      cbstruct.target = tb -> target;
      cbstruct.transfer_id = (XtPointer) tc;
      cbstruct.flags = XmSELECTION_DEFAULT;
      cbstruct.remaining = tc -> outstanding;
      cbstruct.type = *type;
      cbstruct.value = value;
      cbstruct.length = *length;
      cbstruct.format = *format;

      if (tb -> selection_proc != NULL)
	tb -> selection_proc(wid, tb -> client_data, &cbstruct);
    }
  }


  /* Free this transfer block */
  if (tb != NULL) {
    XtFree((char*) tb);
  }

  /* Ignore callbacks after we're done */
  tc -> outstanding--;

  /* When outstanding is 0,  check to see if the status is
     XmTRANSFER_DONE_DEFAULT.  This indicates that we
     should attempt calling the widget's destination
     proc.  We'll set a flag in tc to indicate that
     we've done this,  so we don't repeat the action */
  if (tc -> outstanding == 0 &&
      tc -> status == XmTRANSFER_DONE_DEFAULT &&
      tc -> flags & TC_CALLED_CALLBACKS &&
      !(tc -> flags & TC_CALLED_WIDGET)) {
    XmTransferTrait ttrait;

    tc -> flags |= TC_CALLED_WIDGET;
    ttrait = (XmTransferTrait)
      XmeTraitGet((XtPointer) XtClass(wid), XmQTtransfer);

    /* Now lookup the trait on this widget and call the
       internal routine. */
    if (ttrait != NULL) {
    _XmProcessLock();
      TB_internal = 1;
    _XmProcessUnlock();
      if (ttrait -> destinationProc != 0)
	ttrait -> destinationProc(wid, NULL, tc -> callback_struct);
    _XmProcessLock();
      TB_internal = 0;
    _XmProcessUnlock();
    }
  }

  /* Send a delete if this is a move operation and we've complete
     successfully for PRIMARY transfer */
  if (tc -> selection == XA_PRIMARY &&
      tc -> outstanding == 0 &&
      tc -> count != 0 &&
      (tc -> status == XmTRANSFER_DONE_SUCCEED ||
       tc -> status == XmTRANSFER_DONE_DEFAULT) &&
      tc -> op == XmMOVE &&
      ! (tc -> flags & TC_DID_DELETE)) {
    tc -> flags |= TC_DID_DELETE;
    XmTransferValue((XtPointer) tc, atoms[XmADELETE], NULL, NULL,
		    XtLastTimestampProcessed(XtDisplay(wid)));
  }

  /* When outstanding reaches 0,  free context block.  But don't
     do this in the local case.  There we can free in the caller,
     so check the TC_EXITED_DH flag to see if we've exited
     _XmDestinationHandler yet. */
  if (tc -> outstanding == 0 &&
      tc -> flags & TC_EXITED_DH ) {
    FinishTransfer(wid, tc);
  }
}

/**********************************************************/
/* Context block handlers for convert and transfer blocks */
/**********************************************************/

typedef struct __XmCCKey {
  Display *display;
  Atom	  selection;
} _XmCCKeyRec, *_XmCCKey;

static Boolean
CCMatch(XtPointer x, XtPointer y)
{
  _XmCCKey a, b;

  a = (_XmCCKey) x;
  b = (_XmCCKey) y;

  return(a -> display   ==  b -> display &&
	 a -> selection ==  b -> selection);
}

static XmHashValue
CCHash(XtPointer x)
{
  _XmCCKey a;

  a = (_XmCCKey) x;

  return((XmHashValue) ((long) a -> display + (long) a -> selection));
}

static XmHashTable ConvertHashTable = (XmHashTable) NULL;

static ConvertContext
LookupContextBlock(Display *d, Atom a)
{
  ConvertContext cc;
  _XmCCKeyRec x;

  x.display = d;
  x.selection = a;

  _XmProcessLock();
  if (ConvertHashTable == (XmHashTable) NULL)
    ConvertHashTable = _XmAllocHashTable(10, CCMatch, CCHash);

  cc = (ConvertContext) _XmGetHashEntry(ConvertHashTable, (XmHashKey) &x);
  _XmProcessUnlock();

  if (cc == NULL) {
    _XmCCKey new_k;

    new_k = (_XmCCKey) XtMalloc(sizeof(_XmCCKeyRec));
    new_k -> display = d;
    new_k -> selection = a;

    /* Allocate a context block for this selection */
    cc = (ConvertContext) XtMalloc(sizeof(ConvertContextRec));
    _XmProcessLock();
    _XmAddHashEntry(ConvertHashTable, (XmHashKey)new_k, (XtPointer)cc);
    _XmProcessUnlock();
  }

  return(cc);
}

static void
ClearContextBlock(Display *d, Atom a)
{
  ConvertContext cc;

  cc = LookupContextBlock(d, a);

  cc -> flags = 0;
  cc -> op = 0;
  cc -> itemid = 0;
  cc -> location_data = NULL;
  cc -> client_data = NULL;
  cc -> drag_context = (Widget) NULL;
}

/* Functions to get and free transfer ids */

static TransferContext global_tc = NULL;
static TransferContext free_tc = NULL;

static XtPointer
GetTransferID(void)
{
  TransferContext rval;

  /* If there is one on the free list,  unchain it
     and return it */

  _XmProcessLock();
  if (free_tc != NULL)
    {
      rval = free_tc;
      free_tc = (TransferContext) rval -> next;
    }
  else
    rval = (TransferContext) XtMalloc(sizeof(TransferContextRec));

  /* Put it on the chain */

  rval -> next = (XtPointer) global_tc;
  rval -> prev = NULL;

  if (global_tc != NULL) global_tc -> prev = (XtPointer) rval;
  global_tc = rval;
  _XmProcessUnlock();

  /* Initialize */
  rval -> outstanding = 0;
  rval -> count = 0;
  rval -> flags = TC_NONE;
  rval -> requests = NULL;
  rval -> last = NULL;

  return((XtPointer) rval);
}

static void
FreeTransferID(XtPointer id)
{
  TransferContext tid = (TransferContext) id;
  TransferContext pid, nid;

  /* Free done_proc list */
  if (tid -> doneProcs != NULL) XtFree((char*) tid -> doneProcs);

  /* first unchain from global_tc */

  if (global_tc == tid)
    {
      _XmProcessLock();
      global_tc = (TransferContext) tid -> next;
      if (global_tc != NULL)
	global_tc -> prev = NULL;
      _XmProcessUnlock();
    }
  else
    {
      /* Get previous and next */
      pid = (TransferContext) tid -> prev;
      nid = (TransferContext) tid -> next;
      /* Connect prev and next */
      if (pid != NULL) pid -> next = (XtPointer) nid;
      if (nid != NULL) nid -> prev = (XtPointer) pid;
    }

  _XmProcessLock();
  /* Put on free list */
  tid -> next = (XtPointer) free_tc;
  free_tc = tid;
  _XmProcessUnlock();
}

static void
CallDoneProcs(Widget wid, XtPointer id, XmTransferDoneCallbackStruct *ts)
{
  int i;
  TransferContext tid = (TransferContext) id;

  for(i = 0; i < tid -> numDoneProcs; i++) {
    (tid -> doneProcs[i])(wid, tid -> op, ts);
  }
}

static TransferBlock
AddTransferBlock(TransferContext tc)
{
  TransferBlock tb;

  tb = (TransferBlock) XtMalloc(sizeof(TransferBlockRec));
  tb -> next = NULL;
  /* we append blocks to the end of the list */
  if (tc -> requests == NULL)
    {
      tc -> requests = tb;
      tc -> last = tb;
    }
  else
    {
      (tc -> last) -> next = (XtPointer) tb;
      tc -> last = tb;
    }

  _XmProcessLock();
  if (TB_internal)
    tb -> flags = TB_INTERNAL;
  else
    tb -> flags = TB_NONE;
  _XmProcessUnlock();

  return(tb);
}

/* Warning routine */
static void
TransferWarning(Widget w, char* name, char* type, char* message)
{
  XmeWarning(w, message);
}

/****************************************************************/
/* Standard target support					*/
/*								*/
/* This support makes it easy for all widgets to support a set	*/
/* of targets which can be automatically converted to.		*/
/*								*/
/****************************************************************/

/*
 * XmeStandardTargets takes a widget, and a count of the widget's
 * private target list, and returns a list of standard targets.
 * The count of standard targets is returned in the passed in
 * integer
 */

#define MAXBUILTIN 12

Atom*
XmeStandardTargets(Widget w, int count, int *tcount)
{
  enum { XmATARGETS, XmATIMESTAMP, XmAFOREGROUND, XmABACKGROUND,
	 XmACLASS, XmANAME, XmACLIENT_WINDOW, XmA_MOTIF_RENDER_TABLE,
	 XmA_MOTIF_ENCODING_REGISTRY, NUM_ATOMS };
  static char *atom_names[] = {
    XmSTARGETS, XmSTIMESTAMP, XmIFOREGROUND, XmIBACKGROUND,
    XmICLASS, XmINAME, XmSCLIENT_WINDOW, XmS_MOTIF_RENDER_TABLE,
    XmS_MOTIF_ENCODING_REGISTRY };

  int i = 0;
  Atom atoms[XtNumber(atom_names)];
  Atom *targets;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  targets = (Atom *) XtMalloc(sizeof(Atom) * MAXBUILTIN);

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  targets[i] = atoms[XmATARGETS]; i++;
  targets[i] = atoms[XmATIMESTAMP]; i++;
  targets[i] = atoms[XmAFOREGROUND]; i++;
  targets[i] = atoms[XmABACKGROUND]; i++;
  targets[i] = XA_COLORMAP; i++;
  targets[i] = atoms[XmACLASS]; i++;
  targets[i] = atoms[XmANAME]; i++;
  targets[i] = atoms[XmACLIENT_WINDOW]; i++;
  targets[i] = atoms[XmA_MOTIF_RENDER_TABLE]; i++;
  targets[i] = atoms[XmA_MOTIF_ENCODING_REGISTRY]; i++;

  /* Realloc the full size now */
  targets = (Atom *) XtRealloc((char*) targets, sizeof(Atom) * (count + i));

  *tcount = i; /* Return the builtin target count */
  _XmAppUnlock(app);
  return(targets);
}

/*
 * XmeStandardConvert is called when receiving an unknown
 * target.  It should be called last in most convert procs.
 */

/*ARGSUSED*/
void
XmeStandardConvert(Widget w,
		   XtPointer ignore, /* unused */
		   XmConvertCallbackStruct *cs)
{
  enum { XmATARGETS, XmAFOREGROUND, XmAPIXEL, XmABACKGROUND,
	 XmACLASS, XmANAME, XmACLIENT_WINDOW, XmA_MOTIF_RENDER_TABLE,
	 XmA_MOTIF_ENCODING_REGISTRY, NUM_ATOMS };
  static char *atom_names[] = {
    XmSTARGETS, XmIFOREGROUND, XmIPIXEL, XmIBACKGROUND,
    XmICLASS, XmINAME, XmSCLIENT_WINDOW, XmS_MOTIF_RENDER_TABLE,
    XmS_MOTIF_ENCODING_REGISTRY };

  Arg arg[1];
  Atom atoms[XtNumber(atom_names)];
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (atoms[XmATARGETS] == cs -> target) {
    int tcount;
    cs -> value = (XtPointer) XmeStandardTargets(w, 0, &tcount);
    cs -> format = 32;
    cs -> length = tcount;
    cs -> type = XA_ATOM;
  } else if (atoms[XmAFOREGROUND] == cs -> target) {
    Pixel *fg;

    if (XmIsGadget(w)) w = XtParent(w);

    fg = (Pixel *) XtMalloc(sizeof(Pixel));
    XtSetArg(arg[0], XtNforeground, fg);
    XtGetValues(w, arg, 1);

    cs -> value = (XtPointer) fg;
    cs -> format = 32;
    cs -> length = 1;
    cs -> type = atoms[XmAPIXEL];
  } else if (atoms[XmABACKGROUND] == cs -> target) {
    Pixel *bg;

    if (XmIsGadget(w)) w = XtParent(w);

    bg = (Pixel *) XtMalloc(sizeof(Pixel));
    XtSetArg(arg[0], XtNbackground, bg);
    XtGetValues(w, arg, 1);

    cs -> value = (XtPointer) bg;
    cs -> format = 32;
    cs -> length = 1;
    cs -> type = atoms[XmAPIXEL];
  } else if (XA_COLORMAP == cs -> target) {
    Colormap *cmap;

    if (XmIsGadget(w)) w = XtParent(w);

    cmap = (Colormap *) XtMalloc(sizeof(Colormap));
    XtSetArg(arg[0], XtNcolormap, cmap);
    XtGetValues(w, arg, 1);

    cs -> value = (XtPointer) cmap;
    cs -> format = 32;
    cs -> length = 1;
    cs -> type = XA_COLORMAP;
  } else if (atoms[XmACLASS] == cs -> target) {
    Widget current;
    unsigned long bytesAfter;

    cs -> value = NULL;
    cs -> format = 32;
    cs -> length = 0;
    cs -> type = XA_INTEGER;

    for(current = w;
	current != (Widget) NULL;
	current = XtParent(current)) {
      if (XtIsShell(current)) {
	XGetWindowProperty(XtDisplay(current), XtWindow(current),
			   XA_WM_CLASS, 0L, 100000L, False,
			   (Atom) AnyPropertyType,
			   &cs -> type,
			   &cs -> format,
			   &cs -> length,
			   &bytesAfter,
			   (unsigned char**) &cs -> value);
	if (cs -> value != NULL) break;
      }
    }
  } else if (atoms[XmANAME] == cs -> target) {
    Widget current;
    unsigned long bytesAfter;
    Atom type;
    int format;
    unsigned char *value = NULL;
    char *total_value;
    unsigned long length;

    for(current = w;
	current != (Widget) NULL;
	current = XtParent(current)) {
      if (XtIsShell(current)) {
	XGetWindowProperty(XtDisplay(current), XtWindow(current),
			   XA_WM_NAME, 0L, 100000L, False,
			   (Atom) AnyPropertyType,
			   &type,
			   &format,
			   &length,
			   &bytesAfter,
			   &value);
	if (value != NULL) break;
      }
    }

    total_value = _XmTextToLocaleText(w, (XtPointer)value, type,
				      format, length, NULL);

    cs -> value = (XtPointer) total_value;
    cs -> format = 8;
    cs -> length = total_value != NULL ? strlen(total_value) : 0;
    cs -> type = XmeGetEncodingAtom(w);
  } else if (atoms[XmACLIENT_WINDOW] == cs -> target) {
    Widget current;
    Window *cw;

    cw = (Window *) XtMalloc(sizeof(Window));
    for(current = w; current != (Widget) NULL; current = XtParent(current))
      if (XtIsShell(current)) break;

    *cw = XtWindow(current);
    cs -> value = (XtPointer) cw;
    cs -> format = 32;
    cs -> length = 1;
    cs -> type = XA_WINDOW;
  } else if (atoms[XmA_MOTIF_RENDER_TABLE] == cs -> target) {
    XmRenderTable table;
    Arg args[1];
    char *value;
    int size;

    table = (XmRenderTable) NULL;
    XtSetArg(args[0], XmNrenderTable, &table);
    XtGetValues(w, args, 1);

    if (table == NULL) {
      /* If we didn't find a render table on this widget, then
	 go ahead and look up the chain for something which
	 does have one */
      table = XmeGetDefaultRenderTable(w, XmTEXT_RENDER_TABLE);
    }

    if (table != NULL) {
      size = XmRenderTableCvtToProp(w, table, &value);
      cs -> value = (XtPointer) value;
      cs -> format = 8;
      cs -> length = size;
      cs -> type = XA_STRING;
    }
  } else if (atoms[XmA_MOTIF_ENCODING_REGISTRY] == cs -> target) {
    int len;

    cs -> format = 8;
    cs -> type = XA_STRING;
    cs -> value = _XmGetEncodingRegistryTarget(&len);
    cs -> length = len;
  }
  _XmAppUnlock(app);
}

Atom
XmeGetEncodingAtom(Widget w)
{
  int ret_status = 0;
  XTextProperty tmp_prop;
  char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
  Atom encoding;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  tmp_prop.value = NULL; /* just in case X doesn't do it */
  ret_status = XmbTextListToTextProperty(XtDisplay(w), &tmp_string, 1,
					 (XICCEncodingStyle)XTextStyle,
					 &tmp_prop);
  if (ret_status == Success)
    encoding = tmp_prop.encoding;
  else
    encoding = None;        /* XmbTextList... should always be able
			   * to convert XPCS characters; but in
			   * case its broken, this prevents a core
			   * dump.
			   */
  if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);
  _XmAppUnlock(app);
  return(encoding);
}


char *
_XmTextToLocaleText(Widget w,
		    XtPointer value,
		    Atom type,
		    int format,
		    unsigned long length,
		    Boolean *success)
{
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False);
#if XM_UTF8
  Atom UTF8_STRING = XInternAtom(XtDisplay(w), XmSUTF8_STRING, False);
#endif
  XTextProperty text_prop;
  int status;
  char ** values;
  int num_values = 0;
  char *total_value = NULL;
  int malloc_size = 0;
  int i;

  if (type == XA_STRING || type == COMPOUND_TEXT
#if XM_UTF8
      || type == UTF8_STRING
#endif
     ) {
    text_prop.value = (unsigned char *) value;
    text_prop.encoding = type;
    text_prop.format = format;
    text_prop.nitems = length;

    status = XmbTextPropertyToTextList(XtDisplay(w), &text_prop, &values,
				       &num_values);

    if (success != NULL) {
      if (status == Success || status > 0)
	*success = True;
      else
	*success = False;
    }

    if (num_values) {
      for (i = 0; i < num_values ; i++)
	malloc_size += strlen(values[i]);

      total_value = XtMalloc ((unsigned) malloc_size + 1);
      total_value[0] = '\0';
      for (i = 0; i < num_values ; i++)
	strcat(total_value, values[i]);
      XFreeStringList(values);
    }
  }
  return total_value;
}

void
_XmConvertComplete(Widget wid, XtPointer value,
		   unsigned long size, int format, Atom type,
		   XmConvertCallbackStruct *cs)
{

  if (value == NULL && cs -> value == NULL) {
    XmeStandardConvert(wid, NULL, cs);
  } else {
    if (cs -> status == XmCONVERT_MERGE) {
      XmeConvertMerge(value, type, format,  size, cs);
      XtFree((char*) value);
    } else {
      /* Not merging */
      if (cs -> value != NULL) XtFree((char*) cs -> value);
      cs -> type = type;
      cs -> value = value;
      cs -> length = size;
      cs -> format = format;
    }
  }

  if (cs -> value != NULL)
    cs -> status = XmCONVERT_DONE;
  else
    cs -> status = XmCONVERT_REFUSE;
}

XmDestinationCallbackStruct*
_XmTransferGetDestinationCBStruct(XtPointer tid)
{
  TransferContext tc = (TransferContext) tid;

  return(tc -> callback_struct);
}

/* Error handler for XGetAtomName */

static int SIF_ErrorFlag;

/*ARGSUSED*/
static int
SIF_ErrorHandler(
     Display *display,		/* unused */
     XErrorEvent *event)

{
  _XmProcessLock();
  SIF_ErrorFlag = event -> type;
  _XmProcessUnlock();

  return 0;
}

/* NOTE! XGetAtomName return value MUST be freed with XFree; however, there
 ** isn't a good way to allocate data which can be freed with XFree. We could
 ** cache a static character pointer to NULL and check it to decide whether or
 ** not to free; for now, just pass information back on what to do with the
 ** returned value.
 */
static char*
GetSafeAtomName(Display *display, Atom a, FreeType *howFree)
{
  XErrorHandler old_Handler;
  char *returnvalue;

  /* Setup error proc and reset error flag */
  old_Handler = XSetErrorHandler((XErrorHandler) SIF_ErrorHandler);
  _XmProcessLock();
  SIF_ErrorFlag = 0;
  _XmProcessUnlock();

  returnvalue = XGetAtomName(display, a);
  *howFree = DoXFree;

  XSetErrorHandler(old_Handler);

  _XmProcessLock();
  if (SIF_ErrorFlag != 0) {
    returnvalue = (char*) malloc(1);
    returnvalue[0] = 0; /* Create empty string to return */
    *howFree = DoFree;
    TransferWarning(NULL, ATOM, ARG, BAD_ATOM_MESSAGE);
  }
  _XmProcessUnlock();
  return(returnvalue);
}
