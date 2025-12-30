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
static char rcsid[] = "$TOG: TextSel.c /main/24 1998/12/07 11:12:08 mgreess $"
#endif
#endif

#include <X11/Xatom.h>
#include <Xm/DragC.h>
#include <Xm/DropTrans.h>
#include <Xm/TraitP.h>		/* for XmeTraitSet() */
#include <Xm/TransferT.h>
#include <Xm/XmosP.h>
#include "TextI.h"
#include "TextInI.h"
#include "TextSelI.h"
#include "TextStrSoI.h"
#include "TransferI.h"		/* for _XmConvertComplete() */
#include "TraversalI.h"
#include "XmI.h"


/********    Static Function Declarations    ********/

static void InsertSelection(
                        Widget w,
                        XtPointer closure,
                        Atom *seltype,
                        Atom *type,
                        XtPointer value,
                        unsigned long *length,
                        int *format,
			XtPointer tid) ;
static void HandleInsertTargets(
                        Widget w,
                        XtPointer closure,
                        Atom *seltype,
                        Atom *type,
                        XtPointer value,
                        unsigned long *length,
                        int *format,
			XtPointer tid) ;

static void HandleDrop(Widget w,
		       XmDropProcCallbackStruct *cb,
		       XmDestinationCallbackStruct *ds);

static void HandleTargets(Widget w,
			  XtPointer ignore,
			  XmSelectionCallbackStruct *ds);

static void DoStuff(Widget w,
		    XtPointer closure,
		    XmSelectionCallbackStruct *ds);

static void DropDestroyCB(Widget w,
			  XtPointer clientData,
			  XtPointer callData);

static void DropTransferProc(Widget w, XtPointer ignore,
			     XmSelectionCallbackStruct *ds);

static void SetDropContext(Widget w);

static void DeleteDropContext(Widget w);
static void TextSecondaryWrapper(Widget, XtPointer,
				 XmSelectionCallbackStruct *);
static void TextConvertCallback(Widget, XtPointer,
				XmConvertCallbackStruct*);
static void TextDestinationCallback(Widget, XtPointer,
				    XmDestinationCallbackStruct*);

/********    End Static Function Declarations    ********/

/* Transfer Trait record for Text */

static XmConst XmTransferTraitRec TextTransfer = {
  0,  				/* version */
  (XmConvertCallbackProc) 	TextConvertCallback,
  (XmDestinationCallbackProc)	TextDestinationCallback,
  (XmDestinationCallbackProc)	NULL,
};

static XContext _XmTextDNDContext = 0;
static _XmTextPrimSelect *prim_select;
static _XmInsertSelect insert_select;

static void SetPrimarySelection(Widget w, XtEnum op, XmTransferDoneCallbackStruct *ts)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition cursorPos = tw->text.cursor_position;

  (void)op;
  (void)ts;
  _XmProcessLock();
  if (!prim_select) {
    _XmProcessUnlock();
    return;
  }

  if (prim_select->num_chars > 0) {
    data->anchor = prim_select->position;
    cursorPos = prim_select->position + prim_select->num_chars;
    _XmTextSetCursorPosition(w, cursorPos);
    _XmTextSetDestinationSelection(w, tw->text.cursor_position,
				   False, prim_select->time);
    (*tw->text.source->SetSelection)(tw->text.source, data->anchor,
				     tw->text.cursor_position,
				     prim_select->time);
  }
  if (--prim_select->ref_count == 0) {
    XtFree((XtPointer)prim_select);
    prim_select = NULL;
  }
  _XmProcessUnlock();
}


static void CleanPrimarySelection(Widget w, XtEnum op, XmTransferDoneCallbackStruct *ts)
{
  (void)op;
  (void)ts;
  _XmProcessLock();
  if (!prim_select) {
    _XmProcessUnlock();
    return;
  }

  if (--prim_select->ref_count == 0) {
    XtFree((XtPointer)prim_select);
    prim_select = NULL;
  }
  _XmProcessUnlock();
}


static void
TextSecondaryWrapper(Widget w, XtPointer closure,
			XmSelectionCallbackStruct *ds)
{
  Atom XA_TARGETS = XInternAtom(XtDisplay(w), XmSTARGETS, False);

  if (ds -> target == XA_TARGETS)
    HandleInsertTargets(w, closure, &(ds -> selection), &(ds -> type),
			ds -> value, &(ds -> length), &(ds -> format),
			ds -> transfer_id);
  else
    InsertSelection(w, closure, &(ds -> selection), &(ds -> type),
		    ds -> value, &(ds -> length), &(ds -> format),
		    ds -> transfer_id);
}

static void
InsertSelection(
        Widget w,
        XtPointer closure,
        Atom *seltype,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format,
	XtPointer tid)
{
  _XmInsertSelect *_insert_select = (_XmInsertSelect *)closure;
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition left = 0;
  XmTextPosition right = 0;
  Boolean dest_disjoint = False;
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False);
#if XM_UTF8
  Atom UTF8_STRING = XInternAtom(XtDisplay(w), XmSUTF8_STRING, False);
#endif
  char * total_value = NULL;
  XmTextBlockRec block, newblock;
  XmTextPosition cursorPos;
  Boolean freeBlock;

  if (!value) {
    _insert_select->done_status = True;
    return;
  }

  /* Don't do replace if there is no text to add */
  if (*(char *)value == '\0' || *length == 0){
    XtFree((char*)value);
    value = NULL;
    _insert_select->done_status = True;
    return;
  }

  if (_insert_select->select_type == XmPRIM_SELECT) {
    if (!(*tw->text.source->GetSelection)(tw->text.source, &left, &right) ||
	left == right) {
      XBell(XtDisplay(w), 0);
      XtFree((char*)value);
      value = NULL;
      _insert_select->done_status = True;
      _insert_select->success_status = False;
      return;
    }
  } else if (_insert_select->select_type == XmDEST_SELECT) {
    if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
	left != right) {
      if (tw->text.cursor_position < left ||
	  tw->text.cursor_position > right ||
	  !tw->text.input->data->pendingdelete) {
	left = right = tw->text.cursor_position;
	dest_disjoint = True;
      }
    } else
      left = right = tw->text.cursor_position;
  }

  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);

  block.format = XmFMT_8_BIT;

  if (*type == COMPOUND_TEXT || *type == XA_STRING
#if XM_UTF8
      || *type == UTF8_STRING
#endif
  ) {
    if ((total_value =
	 _XmTextToLocaleText(w, value, *type, *format,
			     *length, NULL)) != NULL) {
      block.ptr = total_value;
      block.length = strlen(block.ptr);
    } else {
      _insert_select->done_status = True;
      _insert_select->success_status = False;
      (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
      return;
    }
  } else {  /* it must be either CS_OF_ENCODING or TEXT */
    block.ptr = (char*)value;
    /* NOTE: casting *length could result in a truncated long. */
    block.length = (unsigned) *length;
    block.format = XmFMT_8_BIT;
  }

  if (_XmTextModifyVerify(tw, (XEvent *)_insert_select->event, &left, &right,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    if ((*tw->text.source->Replace)(tw, (XEvent *)_insert_select->event,
				    &left, &right,
				    &newblock, False) != EditDone) {
      if (tw->text.verify_bell) XBell(XtDisplay(w), 0);
      _insert_select->success_status = False;
    } else {
      _insert_select->success_status = True;

      if (!tw->text.add_mode) tw->text.input->data->anchor = left;

      if (tw->text.add_mode && cursorPos >= left && cursorPos <= right)
	tw->text.pendingoff = FALSE;
      else
	tw->text.pendingoff = TRUE;

      _XmTextSetCursorPosition(w, cursorPos);
      _XmTextSetDestinationSelection(w, tw->text.cursor_position, False,
				     _insert_select->event->time);

      if (_insert_select->select_type == XmDEST_SELECT) {
	if (left != right) {
	  if (!dest_disjoint || !tw->text.add_mode) {
	    (*tw->text.source->SetSelection)(tw->text.source,
					     tw->text.cursor_position,
					     tw->text.cursor_position,
					     _insert_select->event->time);
	  }
	}
      }
      _XmTextValueChanged(tw, (XEvent *)_insert_select->event);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  }

  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
  if (total_value) XtFree(total_value);
  XtFree((char*)value);
  value = NULL;
  _insert_select->done_status = True;
}

static void
HandleInsertTargets(
        Widget w,
        XtPointer closure,
        Atom *seltype,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format,
	XtPointer tid )
{
  enum { XmATEXT, XmACOMPOUND_TEXT,
#if XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] = { XmSTEXT, XmSCOMPOUND_TEXT,
#if XM_UTF8
      XmSUTF8_STRING
#endif
      };

  _XmInsertSelect *_insert_select = (_XmInsertSelect *) closure;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
  Atom target;
  Atom *atom_ptr;
  Boolean supports_encoding_data = False;
  Boolean supports_CT = False;
  Boolean supports_text = False;
  Boolean supports_utf8_string = False;
  int i;

  if (0 == *length) {
    XtFree((char *)value);
    _insert_select->done_status = True;
    return; /* Supports no targets, so don't bother sending anything */
  }

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  atom_ptr = (Atom *)value;

  for (i = 0; i < *length; i++, atom_ptr++) {
    if (*atom_ptr == atoms[XmATEXT])
      supports_text = True;

    if (*atom_ptr == CS_OF_ENCODING)
      supports_encoding_data = True;

    if (*atom_ptr == atoms[XmACOMPOUND_TEXT])
      supports_CT = True;

#if XM_UTF8
    if (*atom_ptr == atoms[XmAUTF8_STRING])
      supports_utf8_string = True;
#endif
  }

  if (supports_text && supports_encoding_data)
    target = atoms[XmATEXT];
#if XM_UTF8
  else if (supports_utf8_string)
    target = atoms[XmAUTF8_STRING];
#endif
  else if (supports_CT)
    target = atoms[XmACOMPOUND_TEXT];
  else if (supports_encoding_data)
    target = CS_OF_ENCODING;
  else
    target = XA_STRING;

  XmTransferValue(tid, target,
		  (XtCallbackProc) TextSecondaryWrapper,
		  closure, _insert_select -> event -> time);
}

Boolean
_XmTextConvert(
        Widget w,
        Atom *selection,
        Atom *target,
        Atom *type,
        XtPointer *value,
        unsigned long *length,
        int *format,
	Widget drag_context,
        XEvent *event )
{
  enum { XmA_MOTIF_DESTINATION, XmAINSERT_SELECTION, XmADELETE,
	 XmATARGETS, XmATEXT, XmACOMPOUND_TEXT, XmATIMESTAMP,
	 XmA_MOTIF_DROP, XmACLIPBOARD, XmANULL,
#if XM_UTF8
	 XmAUTF8_STRING,
#endif
	 NUM_ATOMS };
  static char *atom_names[] = {
    XmS_MOTIF_DESTINATION, XmSINSERT_SELECTION, XmSDELETE,
    XmSTARGETS, XmSTEXT, XmSCOMPOUND_TEXT, XmSTIMESTAMP,
    XmS_MOTIF_DROP, XmSCLIPBOARD, XmSNULL,
#if XM_UTF8
    XmSUTF8_STRING
#endif
    };

  XmTextWidget tw = (XmTextWidget) w;
  XmTextSource source;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING;
  XSelectionRequestEvent * req_event = (XSelectionRequestEvent *) event;
  Boolean has_selection = False;
  XmTextPosition left = 0;
  XmTextPosition right = 0;
  Boolean is_primary;
  Boolean is_secondary;
  Boolean is_destination;
  Boolean is_drop;
  int target_count = 0;
  XTextProperty tmp_prop;
  int status = 0;
  char * tmp_value;
  Time _time;

  if (w == NULL) return False;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);
  CS_OF_ENCODING = XmeGetEncodingAtom(w);

  if (req_event == NULL)
    _time = XtLastTimestampProcessed(XtDisplay(w));
  else
    _time = req_event -> time;

  source = tw->text.source;

  if (*selection == XA_PRIMARY || *selection == atoms[XmACLIPBOARD]) {
    has_selection = (*tw->text.source->GetSelection)(source, &left, &right);
    is_primary = True;
    is_secondary = is_destination = is_drop = False;
  } else if (*selection == atoms[XmA_MOTIF_DESTINATION]) {
    has_selection = tw->text.input->data->has_destination;
    is_destination = True;
    is_secondary = is_primary = is_drop = False;
  } else if (*selection == XA_SECONDARY) {
    has_selection = _XmTextGetSel2(tw, &left, &right);
    is_secondary = True;
    is_destination = is_primary = is_drop = False;
  } else if (*selection == atoms[XmA_MOTIF_DROP]) {
    has_selection = (*tw->text.source->GetSelection)(source, &left, &right);
    is_drop = True;
    is_destination = is_primary = is_secondary = False;
  } else
    return False;


  /*
   * TARGETS identifies what targets the text widget can
   * provide data for.
   */
  if (*target == atoms[XmATARGETS]) {
    Atom *targs = XmeStandardTargets(w, 10, &target_count);

    *value = (XtPointer) targs;
    if (XA_STRING != CS_OF_ENCODING) {
      targs[target_count] = CS_OF_ENCODING;  target_count++;
    }
    if (is_primary || is_destination) {
      targs[target_count] = atoms[XmAINSERT_SELECTION];  target_count++;
    }
    if (is_primary || is_secondary || is_drop) {
      targs[target_count] = atoms[XmACOMPOUND_TEXT];  target_count++;
      targs[target_count] = atoms[XmATEXT];  target_count++;
      targs[target_count] = XA_STRING;  target_count++;
#if XM_UTF8
      targs[target_count] = atoms[XmAUTF8_STRING];  target_count++;
#endif
    }
    if (is_primary || is_drop) {
      targs[target_count] = atoms[XmADELETE]; target_count++;
    }
    *type = XA_ATOM;
    *length = target_count;
    *format = 32;
  } else if (*target == atoms[XmATIMESTAMP]) {
    Time *timestamp;
    timestamp = (Time *) XtMalloc(sizeof(Time));
    if (is_primary)
      *timestamp = source->data->prim_time;
    else if (is_destination)
      *timestamp = tw->text.input->data->dest_time;
    else if (is_secondary)
      *timestamp = tw->text.input->data->sec_time;
    else if (is_drop)
      *timestamp = tw->text.input->data->sec_time;
    *value = (XtPointer) timestamp;
    *type = XA_INTEGER;
    *length = sizeof(Time) / 4;
    *format = 32;
  } else if (*target == XA_STRING) {
    /* Provide data for XA_STRING requests */
    *type = (Atom) XA_STRING;
    *format = 8;
    if (is_destination || !has_selection) return False;
    tmp_prop.value = NULL;
    tmp_value = _XmStringSourceGetString(tw, left, right, False);
    status = XmbTextListToTextProperty(XtDisplay(tw), &tmp_value, 1,
				       (XICCEncodingStyle)XStringStyle,
				       &tmp_prop);
    XtFree(tmp_value);
    if (status == Success || status > 0){
      /* NOTE: casting tmp_prop.nitems could result in a truncated long. */
      if (0 >= tmp_prop.nitems)
        *value = (XtPointer) XtMalloc(1);
      else
        *value = (XtPointer) XtMalloc((unsigned)tmp_prop.nitems);
      memcpy((void*)*value, (void*)tmp_prop.value,(size_t)tmp_prop.nitems);
      if (tmp_prop.value != NULL) XFree((char*)tmp_prop.value);
      *length = tmp_prop.nitems;
    } else {
      *value = NULL;
      *length = 0;
      return False;
    }

  } else if (*target == atoms[XmATEXT] || *target == CS_OF_ENCODING) {
    *type = CS_OF_ENCODING;
    *format = 8;
    if (is_destination || !has_selection) return False;
    *value = (XtPointer)_XmStringSourceGetString(tw, left, right, False);
    *length = strlen((char*) *value);
  } else if (*target == atoms[XmACOMPOUND_TEXT]) {
    *type = atoms[XmACOMPOUND_TEXT];
    *format = 8;
    if (is_destination || !has_selection) return False;
    tmp_prop.value = NULL;
    tmp_value =_XmStringSourceGetString(tw, left, right, False);
    status = XmbTextListToTextProperty(XtDisplay(tw), &tmp_value, 1,
				       (XICCEncodingStyle)XCompoundTextStyle,
				       &tmp_prop);
    XtFree(tmp_value);
    if (status == Success || status > 0) {
      /* NOTE: casting tmp_prop.nitems could result in a truncated long. */
      *value = (XtPointer) XtMalloc((unsigned) tmp_prop.nitems);
      memcpy((void*)*value, (void*)tmp_prop.value,(size_t)tmp_prop.nitems);
      if (tmp_prop.value != NULL) XFree((char*)tmp_prop.value);
      *length = tmp_prop.nitems;
    } else {
      *value =  NULL;
      *length = 0;
      return False;
    }
#if XM_UTF8
  } else if (*target == atoms[XmAUTF8_STRING]) {
    *type = atoms[XmAUTF8_STRING];
    *format = 8;
    if (is_destination || !has_selection) return False;
    tmp_prop.value = NULL;
    tmp_value =_XmStringSourceGetString(tw, left, right, False);
    status = XmbTextListToTextProperty(XtDisplay(tw), &tmp_value, 1,
				       (XICCEncodingStyle)XUTF8StringStyle,
				       &tmp_prop);
    XtFree(tmp_value);
    if (status == Success || status > 0) {
      /* NOTE: casting tmp_prop.nitems could result in a truncated long. */
      *value = (XtPointer) XtMalloc((unsigned) tmp_prop.nitems);
      memcpy((void*)*value, (void*)tmp_prop.value,(size_t)tmp_prop.nitems);
      if (tmp_prop.value != NULL) XFree((char*)tmp_prop.value);
      *length = tmp_prop.nitems;
    } else {
      *value =  NULL;
      *length = 0;
      return False;
    }
#endif
  } else if (*target == atoms[XmAINSERT_SELECTION]) {
    if (is_secondary)
      return False;
    else
      return True;
    /* Delete the selection */
  } else if (*target == atoms[XmADELETE]) {
    XmTextBlockRec block, newblock;
    XmTextPosition cursorPos;
    Boolean freeBlock;

    if (!(is_primary || is_drop)) return False;

    /* The on_or_off flag is set to prevent unecessary
       cursor shifting during the Replace operation */
    tw->text.on_or_off = off;

    block.ptr = "";
    block.length = 0;
    block.format = XmFMT_8_BIT;

    if (_XmTextModifyVerify(tw, event, &left, &right,
			    &cursorPos, &block, &newblock, &freeBlock)) {
      if ((*tw->text.source->Replace)(tw, event, &left, &right,
				      &newblock, False) != EditDone) {
	if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
	return False;
      } else {
	if (is_drop) {
	  if (_XmTextGetDropReciever((Widget)tw) != (Widget) tw)
	    _XmTextSetCursorPosition((Widget)tw, cursorPos);
	} else {
	  if ((*selection == atoms[XmACLIPBOARD]) ||
	      (req_event != NULL &&
	      req_event->requestor != XtWindow((Widget) tw)))
	    _XmTextSetCursorPosition(w, cursorPos);
	}
	_XmTextValueChanged(tw, (XEvent *) req_event);
      }
      if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
    }
    if (!tw->text.input->data->has_destination)
      tw->text.input->data->anchor = tw->text.cursor_position;

    (*tw->text.source->SetSelection)(tw->text.source,
				     tw->text.cursor_position,
				     tw->text.cursor_position,
				     _time);

    *type = atoms[XmANULL];
    *value = NULL;
    *length = 0;
    *format = 8;

    tw->text.on_or_off = on;

    /* unknown selection type */
  } else
    return FALSE;
  return TRUE;
}

void
_XmTextLoseSelection(
        Widget w,
        Atom *selection )
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextSource source = tw->text.source;
  Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(w),
				       XmS_MOTIF_DESTINATION, False);
  /* Losing Primary Selection */
  if (*selection == XA_PRIMARY && _XmStringSourceHasSelection(source)) {
    XmAnyCallbackStruct cb;
    (*source->SetSelection)(source, 1, -999,
			    XtLastTimestampProcessed(XtDisplay(w)));
    cb.reason = XmCR_LOSE_PRIMARY;
    cb.event = NULL;
    XtCallCallbackList(w, tw->text.lose_primary_callback, (XtPointer) &cb);
    /* Losing Destination Selection */
  } else if (*selection == MOTIF_DESTINATION) {
    tw->text.input->data->has_destination = False;
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);
    tw->text.output->data->blinkstate = on;
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
    /* Losing Secondary Selection */
  } else if (*selection == XA_SECONDARY && tw->text.input->data->hasSel2){
    _XmTextSetSel2(tw, 1, -999, XtLastTimestampProcessed(XtDisplay(w)));
  }
}


static void
HandleDrop(Widget w,
	   XmDropProcCallbackStruct *cb,
	   XmDestinationCallbackStruct *ds)
{
  Widget drag_cont, initiator;
  XmTextWidget tw = (XmTextWidget) w;
  Cardinal numExportTargets, n;
  Atom *exportTargets;
  Atom desiredTarget = None;
  Arg args[10];
  XmTextPosition insert_pos, left, right;
  Boolean doTransfer = False;
  XtPointer tid = ds->transfer_id;
  _XmTextDropTransferRec *transfer_rec = NULL;

  drag_cont = cb->dragContext;

  n = 0;
  XtSetArg(args[n], XmNsourceWidget, &initiator); n++;
  XtSetArg(args[n], XmNexportTargets, &exportTargets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &numExportTargets); n++;
  XtGetValues((Widget) drag_cont, args, n);

  insert_pos = (*tw->text.output->XYToPos)(tw, cb->x, cb->y);

  if (cb->operation & XmDROP_MOVE && w == initiator &&
      ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
       left != right && insert_pos >= left && insert_pos <= right)) {
    XmTransferDone(tid, XmTRANSFER_DONE_FAIL);
  } else {
    enum { XmATEXT, XmACOMPOUND_TEXT,
#if XM_UTF8
        XmAUTF8_STRING,
#endif
	NUM_ATOMS };
    static char *atom_names[] = { XmSTEXT, XmSCOMPOUND_TEXT,
#if XM_UTF8
        XmSUTF8_STRING
#endif
	};
    Atom atoms[XtNumber(atom_names)];
    Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
    Boolean encoding_found = False;
    Boolean c_text_found = False;
    Boolean string_found = False;
    Boolean text_found = False;
    Boolean utf8_string_found = False;

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

    /* intialize data to send to drop transfer callback */
    transfer_rec = (_XmTextDropTransferRec *)
      XtMalloc(sizeof(_XmTextDropTransferRec));
    transfer_rec->widget = w;
    transfer_rec->insert_pos = insert_pos;
    transfer_rec->num_chars = 0;
    transfer_rec->timestamp = cb->timeStamp;

    if (cb->operation & XmDROP_MOVE) {
      transfer_rec->move = True;
    } else {
      transfer_rec->move = False;
    }

    for (n = 0; n < numExportTargets; n++) {
      if (exportTargets[n] == CS_OF_ENCODING) {
	desiredTarget = CS_OF_ENCODING;
	encoding_found = True;
	break;
      }
#if XM_UTF8
      if (exportTargets[n] == atoms[XmAUTF8_STRING]) utf8_string_found = True;
#endif
      if (exportTargets[n] == atoms[XmACOMPOUND_TEXT]) c_text_found = True;
      if (exportTargets[n] == XA_STRING) string_found = True;
      if (exportTargets[n] == atoms[XmATEXT]) text_found = True;
    }

    n = 0;
    if (encoding_found || c_text_found || string_found || text_found
        || utf8_string_found) {
      if (!encoding_found) {
	if (c_text_found)
	  desiredTarget = atoms[XmACOMPOUND_TEXT];
#if XM_UTF8
	else if (utf8_string_found)
	  desiredTarget = atoms[XmAUTF8_STRING];
#endif
	else if (string_found)
	  desiredTarget = XA_STRING;
	else
	  desiredTarget = atoms[XmATEXT];
      }

      if (cb->operation & XmDROP_MOVE || cb->operation & XmDROP_COPY) {
	doTransfer = True;
      } else {
	XmTransferDone(tid, XmTRANSFER_DONE_FAIL);
      }
    } else {
      XmTransferDone(tid, XmTRANSFER_DONE_FAIL);
    }
  }
  SetDropContext(w);

  if (doTransfer) {
    XmeTransferAddDoneProc(tid, (XmSelectionFinishedProc) DropDestroyCB);
    XmTransferValue(tid, desiredTarget,
		    (XtCallbackProc) DropTransferProc,
		    (XtPointer) transfer_rec, 0);
  }
}

/* Request targets from selection receiver; move the rest of this
 * to a new routine (the name of which is passed during the request
 * for targets).  The new routine will look at the target list and
 * determine what target to place in the pair.  It will then do
 * any necessary conversions before "thrusting" the selection value
 * onto the receiver.  This will guarantee the best chance at a
 * successful exchange.
 */

static void
HandleTargets(Widget w,
	      XtPointer closure,
	      XmSelectionCallbackStruct *ds)
{
  enum { XmACOMPOUND_TEXT, XmACLIPBOARD, XmATEXT,
#if XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] =  { XmSCOMPOUND_TEXT, XmSCLIPBOARD, XmSTEXT,
#if XM_UTF8
      XmSUTF8_STRING
#endif
      };

  XmTextWidget tw = (XmTextWidget) w;
  Atom CS_OF_ENCODING;
  Atom atoms[XtNumber(atom_names)];
  Boolean supports_encoding_data = False;
  Boolean supports_CT = False;
  Boolean supports_text = False;
  Boolean supports_utf8_string = False;
  Atom *atom_ptr;
  XPoint *point = (XPoint *)closure;
  Atom targets[2];
  XmTextPosition select_pos;
  XmTextPosition left, right;
  int i;

  if (!ds->length) {
    XtFree((char *)ds->value);
    ds->value = NULL;
    return;
  }

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);
  CS_OF_ENCODING = XmeGetEncodingAtom(w);

  atom_ptr = (Atom *)ds->value;

  for (i = 0; i < ds->length; i++, atom_ptr++) {
    if (*atom_ptr == atoms[XmATEXT])
      supports_text = True;

    if (*atom_ptr == CS_OF_ENCODING)
      supports_encoding_data = True;

    if (*atom_ptr == atoms[XmACOMPOUND_TEXT])
      supports_CT = True;

#if XM_UTF8
    if (*atom_ptr == atoms[XmAUTF8_STRING])
      supports_utf8_string = True;
#endif
  }


  /*
   * Set stuff position to the x and y position of
   * the button pressed event for primary pastes.
   */
  if (ds->selection != atoms[XmACLIPBOARD] && point) {
    select_pos =
      (*tw->text.output->XYToPos)(tw, (Position)point->x, (Position)point->y);
  } else {
    select_pos = tw->text.cursor_position;
  }

  if (ds->selection != atoms[XmACLIPBOARD]) {
    if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
	left != right && select_pos > left &&
	select_pos < right) {
      XtFree((char *)ds->value);
      ds->value = NULL;
      return;
    }
  }

  _XmProcessLock();
  if (prim_select) {
    prim_select->ref_count++;
  } else {
    prim_select = (_XmTextPrimSelect *)
      XtMalloc((unsigned) sizeof(_XmTextPrimSelect));
  }
  prim_select->position = select_pos;
  prim_select->time = XtLastTimestampProcessed(XtDisplay(w));
  prim_select->num_chars = 0;

  /* If owner supports TEXT and the current locale, ask for TEXT.
   * If not, and if the owner supports compound text, ask for
   * compound text. If not, and owner and I have the same encoding,
   * ask for that encoding. If not, fall back position is to ask for
   * STRING and try to convert it locally.
   */

  if (supports_text && supports_encoding_data)
    prim_select->target = targets[0] = atoms[XmATEXT];
#if XM_UTF8
  else if (supports_utf8_string)
    prim_select->target = targets[0] = atoms[XmAUTF8_STRING];
#endif
  else if (supports_CT)
    prim_select->target = targets[0] = atoms[XmACOMPOUND_TEXT];
  else if (supports_encoding_data)
    prim_select->target = targets[0] = CS_OF_ENCODING;
  else
    prim_select->target = targets[0] = XA_STRING;

  prim_select->ref_count = 1;
  /* Make request to call DoStuff() with the primary selection. */
  XmTransferValue(ds->transfer_id, targets[0], (XtCallbackProc) DoStuff,
		  (XtPointer) prim_select, prim_select->time);

  _XmProcessUnlock();
  XtFree((char *)ds->value);
  ds->value = NULL;
}

/* Pastes the primary selection to the stuff position. */
static void
DoStuff(Widget w,
	XtPointer closure,
	XmSelectionCallbackStruct *ds)
{
  enum { XmANULL, XmACLIPBOARD, XmATEXT, XmACOMPOUND_TEXT,
#if XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] =  {
    XmSNULL, XmSCLIPBOARD, XmSTEXT, XmSCOMPOUND_TEXT,
#if XM_UTF8
    XmSUTF8_STRING
#endif
    };

  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;
  Atom atoms[XtNumber(atom_names)];
  XmTextBlockRec block, newblock;
  XmTextPosition cursorPos = tw->text.cursor_position;
  XmTextPosition right, left, replace_from, replace_to;
  _XmTextPrimSelect *_prim_select = (_XmTextPrimSelect *) closure;
  char * total_value = NULL;
  Boolean freeBlock;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (!o_data->hasfocus && _XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  if (ds->selection != atoms[XmACLIPBOARD] &&
      !(ds->length) && ds->type != atoms[XmANULL]) {
    /* Backwards compatibility for 1.0 Selections */
    _XmProcessLock();
    if (_prim_select->target == atoms[XmATEXT]) {
      _prim_select->target = XA_STRING;
      XmTransferValue(ds->transfer_id, XA_STRING, (XtCallbackProc) DoStuff,
		      (XtPointer)_prim_select, _prim_select->time);
    }
    _XmProcessUnlock();
    XtFree((char *)ds->value);
    ds->value = NULL;
    return;
  }

  /* if length == 0 and ds->type is the NULL atom we are assuming
   * that a DELETE target is requested.
   */
  if (ds->type == atoms[XmANULL]) {
    _XmProcessLock();
    if (_prim_select->num_chars > 0 && data->selectionMove) {
      data->anchor = _prim_select->position;
      cursorPos = _prim_select->position + _prim_select->num_chars;
      _XmTextSetCursorPosition(w, cursorPos);
      _XmTextSetDestinationSelection(w, tw->text.cursor_position,
				     False, _prim_select->time);
      (*tw->text.source->SetSelection)(tw->text.source, data->anchor,
				       tw->text.cursor_position,
				       _prim_select->time);
    }
    _XmProcessUnlock();
  } else {
    XmTextSource source = GetSrc(w);
    int max_length = 0;
    Boolean local = _XmStringSourceHasSelection(source);
    Boolean *pendingoff = NULL;
    Boolean dest_disjoint = True;

    block.format = XmFMT_8_BIT;

    if (ds->type == atoms[XmACOMPOUND_TEXT] ||
#if XM_UTF8
        ds->type == atoms[XmAUTF8_STRING] ||
#endif
	ds->type == XA_STRING) {
      if ((total_value =
	   _XmTextToLocaleText(w, ds->value, ds->type, ds->format,
			       ds->length, NULL)) != NULL) {
	block.ptr = total_value;
	block.length = strlen(block.ptr);
      } else {
	block.ptr = total_value = XtMalloc((unsigned)1);
	*(block.ptr) = '\0';
	block.length = 0;
      }
    } else {
      block.ptr = (char*)ds->value;
      block.length = (int) ds->length; /* NOTE: this causes a truncation on
					  some architectures */
    }

    if (data->selectionMove && local) {
      max_length = _XmStringSourceGetMaxLength(source);
      _XmStringSourceSetMaxLength(source, INT_MAX);
    }

    replace_from = replace_to = _prim_select->position;
    pendingoff = _XmStringSourceGetPending(tw);

    if (ds->selection == atoms[XmACLIPBOARD]) {
      if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right)) {
	if (tw->text.input->data->pendingdelete &&
	    replace_from >= left && replace_to <= right){
	  replace_from = left;
	  replace_to = right;
	  dest_disjoint = False;
	}
      }
   } else {
      /* The on_or_off flag is set to prevent unnecessary
	 cursor shifting during the Replace operation */
      tw->text.on_or_off = off;

      _XmStringSourceSetPending(tw, (Boolean *)FALSE);
    }

    if (_XmTextModifyVerify(tw, ds->event, &replace_from, &replace_to,
			    &cursorPos, &block, &newblock, &freeBlock)) {
      _XmProcessLock();
      _prim_select->num_chars = _XmTextCountCharacters(newblock.ptr,
						      newblock.length);
      _XmProcessUnlock();
      if ((*tw->text.source->Replace)(tw, ds->event,
				      &replace_from, &replace_to,
				      &newblock, False) != EditDone) {
	XtCallActionProc(w, "beep", NULL, (String *) NULL, (Cardinal) 0);
	_XmProcessLock();
	_prim_select->num_chars = 0; /* Stop SetPrimarySelection from doing
				       anything */
	_XmProcessUnlock();
	_XmStringSourceSetPending(tw, pendingoff);
      } else {
	if ((newblock.length > 0 && !data->selectionMove) ||
	    ds->selection == atoms[XmACLIPBOARD]) {
	  _XmTextSetCursorPosition(w, cursorPos);
	  _XmTextSetDestinationSelection(w, tw->text.cursor_position,
					 False, _prim_select->time);
	}
	if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right)) {
	  if (ds->selection == atoms[XmACLIPBOARD]) {
	    data->anchor = replace_from;
	    if (left != right && (!dest_disjoint || !tw->text.add_mode))
	      (*source->SetSelection)(source, tw->text.cursor_position,
				      tw->text.cursor_position,
				      _prim_select->time);
	  } else {
	    if (data->selectionMove) {
	      _XmProcessLock();
	      if (left < replace_from) {
		_prim_select->position = replace_from -
		  _prim_select->num_chars;
	      } else {
		_prim_select->position = replace_from;
	      }
	      _XmProcessUnlock();
	    }
	    if (cursorPos < left || cursorPos > right)
	      _XmStringSourceSetPending(tw, (Boolean *)TRUE);
	    else
	      _XmStringSourceSetPending(tw, pendingoff);
	  }
	} else {
	  _XmProcessLock();
	  if (ds->selection == atoms[XmACLIPBOARD])
	    data->anchor = replace_from;
	  else if (!data->selectionMove && !tw->text.add_mode &&
	      _prim_select->num_chars != 0)
	    data->anchor = _prim_select->position;
	  _XmProcessUnlock();
	}
	_XmTextValueChanged(tw, ds->event);
      }
      if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
    } else {
      XtCallActionProc(w, "beep", NULL, (String *) NULL, (Cardinal) 0);
      _XmProcessLock();
      _prim_select->num_chars = 0; /* Stop SetPrimarySelection from doing
				     anything */
      _XmProcessUnlock();
      _XmStringSourceSetPending(tw, pendingoff);
    }

    if (data->selectionMove && local) {
      _XmStringSourceSetMaxLength(source, max_length);
    }

    if (ds->selection != atoms[XmACLIPBOARD])
      tw->text.on_or_off = on;

    if (pendingoff) XtFree((char *)pendingoff);
  }

  if (total_value) XtFree(total_value);
  XtFree((char *)ds->value);
  ds->value = NULL;
}

static void
DropDestroyCB(Widget      w,
	      XtPointer   clientData,
	      XtPointer   callData)
{
  XmTransferDoneCallbackStruct *ts = (XmTransferDoneCallbackStruct *)callData;

  DeleteDropContext(w);
  if (ts->client_data != NULL) XtFree((char*) ts->client_data);
}

static void
DropTransferProc(Widget w,
		 XtPointer closure,
		 XmSelectionCallbackStruct *ds)
{
  enum { XmACOMPOUND_TEXT, XmANULL, XmADELETE,
#if XM_UTF8
  XmAUTF8_STRING,
#endif
  NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSNULL, XmSDELETE,
#if XM_UTF8
      XmSUTF8_STRING
#endif
      };

  _XmTextDropTransferRec *transfer_rec = (_XmTextDropTransferRec *) closure;
  XmTextWidget tw = (XmTextWidget) transfer_rec->widget;
  InputData data = tw->text.input->data;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
  XmTextPosition insertPosLeft, insertPosRight, left, right, cursorPos;
  XmTextBlockRec block, newblock;
  XmTextSource source = GetSrc((Widget)tw);
  int max_length = 0;
  Boolean local = _XmStringSourceHasSelection(source);
  char * total_value = NULL;
  Boolean pendingoff;
  Boolean freeBlock;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  /* When type = NULL, we are assuming a DELETE request has been requested */
  if (ds->type == atoms[XmANULL]) {
    if (transfer_rec->num_chars > 0 && transfer_rec->move) {
      data->anchor = transfer_rec->insert_pos;
      cursorPos = transfer_rec->insert_pos + transfer_rec->num_chars;
      _XmTextSetCursorPosition((Widget)tw, cursorPos);
      _XmTextSetDestinationSelection((Widget)tw, tw->text.cursor_position,
				     False,
				     XtLastTimestampProcessed(XtDisplay(w)));
      (*tw->text.source->SetSelection)(tw->text.source, data->anchor,
				       tw->text.cursor_position,
				       XtLastTimestampProcessed(XtDisplay(w)));
      if (ds->value) {
	XtFree((char *) ds->value);
	ds->value = NULL;
      }
      return;
    }
  }

  if (!ds->value ||
      (ds->type != atoms[XmACOMPOUND_TEXT] &&
#if XM_UTF8
       ds->type != atoms[XmAUTF8_STRING] &&
#endif
       ds->type != CS_OF_ENCODING &&
       ds->type != XA_STRING)) {
    XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);
    if (ds->value) {
      XtFree((char*) ds->value);
      ds->value = NULL;
    }
    return;
  }

  insertPosLeft = insertPosRight = transfer_rec->insert_pos;

  if (ds->type == XA_STRING || ds->type == atoms[XmACOMPOUND_TEXT]
#if XM_UTF8
      || ds->type == atoms[XmAUTF8_STRING]
#endif
  ) {
    if ((total_value = _XmTextToLocaleText(w, ds->value, ds->type,
					   8, ds->length, NULL)) != NULL) {
      block.ptr = total_value;
      block.length = strlen(block.ptr);
    } else {
      if (ds->value) {
	XtFree((char*) ds->value);
	ds->value = NULL;
      }
      return;
    }
  } else {
    block.ptr = (char *) ds->value;
    block.length = (int) ds->length; /* NOTE: this causes a truncation on
					  some architectures */
  }

  block.format = XmFMT_8_BIT;

  if (data->pendingdelete &&
      (*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
      (left != right)){
    if(insertPosLeft > left && insertPosLeft < right) insertPosLeft = left;
    if(insertPosRight < right && insertPosRight > left) insertPosRight = right;
  }

  if (transfer_rec->move && local) {
    max_length = _XmStringSourceGetMaxLength(source);
    _XmStringSourceSetMaxLength(source, INT_MAX);
  }

  /* The on_or_off flag is set to prevent unecessary
     cursor shifting during the Replace operation */
  tw->text.on_or_off = off;

  pendingoff = tw->text.pendingoff;
  tw->text.pendingoff = FALSE;

  if (_XmTextModifyVerify(tw, ds->event, &insertPosLeft, &insertPosRight,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    if ((*tw->text.source->Replace)(tw, ds->event,
				    &insertPosLeft, &insertPosRight,
				    &newblock, False) != EditDone) {
      if (tw->text.verify_bell) XBell(XtDisplay(tw), 0);
      tw->text.pendingoff = pendingoff;
    } else {
      transfer_rec->num_chars = _XmTextCountCharacters(newblock.ptr,
						       newblock.length);
      if (transfer_rec->num_chars > 0 && !transfer_rec->move) {
	_XmTextSetCursorPosition((Widget)tw, cursorPos);
	_XmTextSetDestinationSelection((Widget)tw,
				       tw->text.cursor_position,False,
				       transfer_rec->timestamp);
      }
      if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right)) {
	if (transfer_rec->move && left < insertPosLeft)
	  transfer_rec->insert_pos = insertPosLeft -
	    transfer_rec->num_chars;
	if (cursorPos < left || cursorPos > right)
	  tw->text.pendingoff = TRUE;
      } else {
	if (!transfer_rec->move && !tw->text.add_mode &&
	    transfer_rec->num_chars != 0)
	  data->anchor = insertPosLeft;
      }
      if (transfer_rec->move) {
	XmTransferValue(ds->transfer_id,
			atoms[XmADELETE],
			(XtCallbackProc) DropTransferProc,
			(XtPointer) transfer_rec, 0);
      }

      if (transfer_rec->move && local) {
	_XmStringSourceSetMaxLength(source, max_length);
      }

      _XmTextValueChanged(tw, (XEvent *) ds->event);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  } else {
    if (tw->text.verify_bell) XBell(XtDisplay(tw), 0);
    tw->text.pendingoff = pendingoff;
  }
  tw->text.on_or_off = on;

  if (total_value) XtFree(total_value);
  if (ds->value != NULL) XtFree((char*) ds->value);
  ds->value = NULL;
}

static void
SetDropContext(Widget w)
{
  Display *display = XtDisplay(w);
  Screen  *screen = XtScreen(w);
  XContext loc_context;

  _XmProcessLock();
  if (_XmTextDNDContext == 0)
	_XmTextDNDContext = XUniqueContext();
  loc_context = _XmTextDNDContext;
  _XmProcessUnlock();

  XSaveContext(display, (Window)screen,
	       loc_context, (XPointer)w);
}


static void
DeleteDropContext(Widget w)
{
  Display *display = XtDisplay(w);
  Screen  *screen = XtScreen(w);

  _XmProcessLock();
  XDeleteContext(display, (Window)screen, _XmTextDNDContext);
  _XmProcessUnlock();
}


Widget
_XmTextGetDropReciever(Widget w)
{
  Display *display = XtDisplay(w);
  Screen  *screen = XtScreen(w);
  Widget widget;
  XContext loc_context;

  _XmProcessLock();
  loc_context = _XmTextDNDContext;
  _XmProcessUnlock();
  if (loc_context == 0) return NULL;

  if (!XFindContext(display, (Window)screen,
		    loc_context, (char **) &widget)) {
    return widget;
  }

  return NULL;
}



/********************************************
 * Transfer trait method implementation
 ********************************************/

static void TextConvertCallback(Widget w, XtPointer ignore, XmConvertCallbackStruct *cs)
{
  enum { XmADELETE, XmA_MOTIF_LOSE_SELECTION,
	 XmA_MOTIF_EXPORT_TARGETS, XmATEXT, XmACOMPOUND_TEXT,
	 XmATARGETS, XmA_MOTIF_CLIPBOARD_TARGETS, XmACLIPBOARD,
#if XM_UTF8
	 XmAUTF8_STRING,
#endif
	 NUM_ATOMS };
  static char *atom_names[] = { XmSDELETE, XmS_MOTIF_LOSE_SELECTION,
	 XmS_MOTIF_EXPORT_TARGETS, XmSTEXT, XmSCOMPOUND_TEXT,
	 XmSTARGETS, XmS_MOTIF_CLIPBOARD_TARGETS, XmSCLIPBOARD,
#if XM_UTF8
	 XmSUTF8_STRING
#endif
	 };

  Atom XA_CS_OF_ENCODING = XmeGetEncodingAtom(w);
  XtPointer value;
  Atom type;
  unsigned long size;
  int format;
  Atom atoms[XtNumber(atom_names)];

  (void)ignore;
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  value = NULL;

  if (cs->target == atoms[XmA_MOTIF_LOSE_SELECTION]) {
    _XmTextLoseSelection(w, &(cs->selection));
    cs->status = XmCONVERT_DONE;
    return;
  }

  if (cs->target == atoms[XmADELETE] &&
      cs->selection == XA_SECONDARY) {
    _XmTextHandleSecondaryFinished(w, cs->event);
    cs->status = XmCONVERT_DONE;
    return;
  }

  /* When this is called as a result of a clipboard copy link,  we
     don't have any available targets.  Make sure to return immediately
     without modification */
  if (cs->selection == atoms[XmACLIPBOARD] &&
      cs->parm == (XtPointer) XmLINK &&
      (cs->target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS] ||
       cs->target == atoms[XmATARGETS])) return;

  if (!_XmTextConvert(w, &cs->selection, &cs->target,
		      &type, &value, &size, &format,
		      (Widget) cs->source_data, cs->event)) {
    value = NULL;
    type = XA_INTEGER;
    size = 0;
    format = 8;
  }

  if (cs->target == atoms[XmADELETE]) {
    cs->status = XmCONVERT_DONE;
    cs->type = type;
    cs->value = value;
    cs->length = size;
    cs->format = format;
    return;
  }

  if (cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS] ||
      cs->target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS]) {
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 5);
    int n = 0;

    value = (XtPointer) targs;
#if XM_UTF8
    targs[n] = atoms[XmAUTF8_STRING]; n++;
#endif
    targs[n] = atoms[XmACOMPOUND_TEXT]; n++;
    targs[n] = atoms[XmATEXT]; n++;
    targs[n] = XA_STRING; n++;
    if (XA_CS_OF_ENCODING != XA_STRING) {
      targs[n] = XA_CS_OF_ENCODING; n++;
    }
    format = 32;
    size = n;
    type = XA_ATOM;
  }

  _XmConvertComplete(w, value, size, format, type, cs);
}

/************************************************
 * Free data allocated for destination callback
 ************************************************/
static void FreeLocationData(Widget w, XtEnum op, XmTransferDoneCallbackStruct *ts)
{
	XmDestinationCallbackStruct *ds;

	(void)w;
	(void)op;
	ds = _XmTransferGetDestinationCBStruct(ts->transfer_id);
	XtFree((XtPointer)ds->location_data);
	ds->location_data = NULL;
}

static void TextDestinationCallback(Widget w, XtPointer closure,
                                    XmDestinationCallbackStruct *ds)
{
  enum { XmATARGETS, XmA_MOTIF_DROP, NUM_ATOMS };
  static char *atom_names[] = { XmSTARGETS, XmS_MOTIF_DROP };

  Atom atoms[XtNumber(atom_names)];
  XPoint DropPoint;

  (void)closure;
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  /*
   ** In case of a primary transfer operation where a location_data
   ** has been allocated, register a done proc to be called when
   ** the data transfer is complete to free the location_data
   */
  if (ds->selection == XA_PRIMARY && ds->location_data)
      XmeTransferAddDoneProc(ds->transfer_id, FreeLocationData);

  /* If we aren't sensitive,  don't allow transfer */
  if (! w -> core.sensitive ||
      ! w -> core.ancestor_sensitive)
    XmTransferDone(ds -> transfer_id, XmTRANSFER_DONE_FAIL);

  /* We don't handle LINKs internally */
  if (ds->operation == XmLINK) return;

  if (ds->selection == XA_PRIMARY && ds->operation == XmMOVE)
    XmeTransferAddDoneProc(ds->transfer_id, SetPrimarySelection);
  else
    XmeTransferAddDoneProc(ds->transfer_id, CleanPrimarySelection);

  if (ds->selection == atoms[XmA_MOTIF_DROP]) {
    XmDropProcCallbackStruct *cb =
      (XmDropProcCallbackStruct *) ds->destination_data;

    DropPoint.x = cb->x;
    DropPoint.y = cb->y;

    ds->location_data = (XtPointer) &DropPoint;

    if (cb->dropAction != XmDROP_HELP)
      HandleDrop(w, cb, ds);
    ds->location_data = NULL;
  }
  else if (ds->selection == XA_SECONDARY) {
    Atom CS_OF_ENCODING;

    CS_OF_ENCODING = XmeGetEncodingAtom(w);

    _XmProcessLock();
    insert_select.done_status = False;
    insert_select.success_status = False;
    insert_select.event = (XSelectionRequestEvent *) ds->event;
    insert_select.select_type = XmDEST_SELECT;

    if (((Atom) ds->location_data) != CS_OF_ENCODING) {
      /*
       * Make selection request to find out which targets
       * the selection can provide.
       */
      XmTransferValue(ds->transfer_id, atoms[XmATARGETS],
		      (XtCallbackProc) TextSecondaryWrapper,
		      (XtPointer) &insert_select, ds->time);
    } else {
      /*
       * Make selection request to replace the selection
       * with the insert selection.
       */
      XmTransferValue(ds->transfer_id, ((Atom) ds->location_data),
		      (XtCallbackProc) TextSecondaryWrapper,
		      (XtPointer) &insert_select, ds->time);
    }
    _XmProcessUnlock();
  } else
    /* CLIPBOARD or PRIMARY */
    XmTransferValue(ds->transfer_id, atoms[XmATARGETS],
		    (XtCallbackProc) HandleTargets,
		    ds->location_data, ds->time);
}

void _XmTextInstallTransferTrait(void)
{
	XmeTraitSet(
		(XtPointer)xmTextWidgetClass,
		XmQTtransfer,
		(XtPointer)&TextTransfer
	);
}

