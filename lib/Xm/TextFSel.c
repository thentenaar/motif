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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: TextFSel.c /main/22 1997/10/14 15:38:30 cshi $"
#endif
#endif

#include <X11/Xatom.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragC.h>
#include <Xm/TraitP.h>		/* for XmeTraitSet() */
#include <Xm/TransferP.h>
#include <Xm/TransferT.h>
#include <Xm/XmosP.h>
#include "TextFI.h"
#include "TextFSelI.h"
#include "TransferI.h"		/* for _XmConvertComplete() */
#include "TraversalI.h"		/* for _XmGetFocusPolicy() */
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
			XtPointer tid);
static void HandleDrop(Widget w,
		       XmDropProcCallbackStruct *cb,
		       XmDestinationCallbackStruct *ds);
static void TextFieldConvertCallback(Widget,
				     XtPointer,
				     XmConvertCallbackStruct*);

static void TextFieldDestinationCallback(Widget,
					 XtPointer,
					 XmDestinationCallbackStruct*);
static void SetDropContext(Widget w);

static void DeleteDropContext(Widget w);

static void HandleTargets(Widget w,
			  XtPointer ignore,
			  XmSelectionCallbackStruct *ds);

static void HandleDrop(Widget w,
		       XmDropProcCallbackStruct *cb,
		       XmDestinationCallbackStruct *ds);

static void DropDestroyCB(Widget w,
			  XtPointer clientData,
			  XtPointer callData);

static void DropTransferProc(Widget w,
			     XtPointer ignore,
			     XmSelectionCallbackStruct *ds);
static void DoStuff(Widget w,
		    XtPointer ignore,
		    XmSelectionCallbackStruct *ds);

/********    End Static Function Declarations    ********/


/* Transfer Trait record for TextField */

static XmConst XmTransferTraitRec textFieldTT = {
  0,  				/* version */
  (XmConvertCallbackProc) 	TextFieldConvertCallback,
  (XmDestinationCallbackProc)	TextFieldDestinationCallback,
  (XmDestinationCallbackProc)	NULL,
};

static XContext _XmTextFDNDContext = 0;
static _XmInsertSelect insert_select;
static _XmTextPrimSelect *prim_select;

/*ARGSUSED*/
static void
SetPrimarySelection(Widget w,
		    XtEnum op,	/* unused */
		    XmTransferDoneCallbackStruct *ts) /* unused */
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition cursorPos = tf->text.cursor_position;

  _XmProcessLock();
  if (!prim_select) {
    _XmProcessUnlock();
    return;
  }

  if (prim_select->num_chars > 0) {
    tf->text.prim_anchor = prim_select->position;
    cursorPos = prim_select->position + prim_select->num_chars;
    _XmTextFieldStartSelection(tf, tf->text.prim_anchor, cursorPos,
			       prim_select->time);
    tf->text.pending_off = False;
    _XmTextFieldSetCursorPosition(tf, NULL, cursorPos, True, True);
  }
  if (--prim_select->ref_count == 0) {
    XtFree((char *)prim_select);
    prim_select = NULL;
  }
  _XmProcessUnlock();
}


/*ARGSUSED*/
static void
CleanPrimarySelection(Widget w,
		    XtEnum op,	/* unused */
		    XmTransferDoneCallbackStruct *ts) /* unused */
{
  _XmProcessLock();
  if (!prim_select)
  {
    _XmProcessUnlock();
    return;
  }

  if (--prim_select->ref_count == 0) {
    XtFree((char *)prim_select);
    prim_select = NULL;
  }
  _XmProcessUnlock();
}


static void
TextFieldSecondaryWrapper(Widget w, XtPointer closure,
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

/* ARGSUSED */
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
  _XmInsertSelect *_insert_select = (_XmInsertSelect *) closure;
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left = 0;
  XmTextPosition right = 0;
  Boolean replace_res = False;
  Boolean dest_disjoint = False;
  wchar_t * wc_value;
  char * temp;
  int num_chars = 0;
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False);
  Atom UTF8_STRING = XInternAtom(XtDisplay(w), XmSUTF8_STRING, False);
  char * total_value = NULL;
  XmAnyCallbackStruct cb;

  if (!value) {
    _insert_select->done_status = True;
    return;
  }

  /* Don't do replace if there is not text to add */
  if (*(char *) value == (char)'\0' || *length == 0){
    XtFree((char *)value);
    _insert_select->done_status = True;
    return;
  }

  if (_insert_select->select_type == XmPRIM_SELECT) {
    if (!tf->text.has_primary ||
	tf->text.prim_pos_left == tf->text.prim_pos_right) {
      XBell(XtDisplay(w), 0);
      XtFree((char *)value);
      _insert_select->done_status = True;
      _insert_select->success_status = False;
      return;
    }
  } else if (_insert_select->select_type == XmDEST_SELECT) {
    if (tf->text.has_primary &&
	(left = tf->text.prim_pos_left) != (right = tf->text.prim_pos_right)) {
      if ( TextF_CursorPosition(tf) < left ||
	  TextF_CursorPosition(tf) > right ||
	  !tf->text.pending_delete) {
	left = right = TextF_CursorPosition(tf);
	dest_disjoint = True;
      }
    } else
      left = right = TextF_CursorPosition(tf);
  }


  if (*type == COMPOUND_TEXT || *type == XA_STRING || *type == UTF8_STRING) {
    total_value =  _XmTextToLocaleText(w, value, *type, *format,
				       *length, NULL);
    if (total_value) {
      if (tf->text.max_char_size == 1) {
	num_chars = strlen(total_value);
	replace_res = _XmTextFieldReplaceText(tf,
					      (XEvent *)_insert_select->event,
					      left, right, total_value,
					      num_chars, True);
      } else { /* must convert to wchar_t before passing to Replace */
	int len = strlen(total_value) + 1;
	wc_value = (wchar_t *)XtMalloc((unsigned) len * sizeof(wchar_t));
	num_chars = mbstowcs(wc_value, total_value, len);
	if (num_chars < 0)
	  num_chars = 0;
	else
	  replace_res = _XmTextFieldReplaceText(tf,
                                                (XEvent *)_insert_select->event,
                                                left, right, (char*) wc_value,
                                                num_chars, True);
	XtFree((char *)wc_value);
      }
      XtFree(total_value);
    }
  } else { /* it must be either TEXT or codeset of the locale */
    if (tf->text.max_char_size == 1) {
      /* NOTE: casting *length could result in a truncated long. */
      num_chars = *length;
      replace_res = _XmTextFieldReplaceText(tf,
					    (XEvent *)_insert_select->event,
					    left, right, (char *)value,
					    (unsigned)*length, True);
    } else {
      temp = XtMalloc((unsigned) *length + 1);
      /* NOTE: casting *length could result in a truncated long. */
      (void)memcpy((void*)temp, (void*)value, (size_t)*length);
      temp[*length] = '\0';
      wc_value = (wchar_t*)XtMalloc((unsigned)
				    (*length + 1) * sizeof(wchar_t));

      /* NOTE: casting *length could result in a truncated long. */
      num_chars = mbstowcs(wc_value, temp, (unsigned)*length + 1);
      if (num_chars < 0)
	num_chars = 0;
      else
	replace_res = _XmTextFieldReplaceText(tf,
					      (XEvent *)_insert_select->event,
					      left, right, (char*) wc_value,
					      num_chars, True);
      XtFree(temp);
      XtFree((char *)wc_value);
    }
  }

  if (!replace_res) {
    _insert_select->success_status = False;
  } else {
    _insert_select->success_status = True;

    if (!tf->text.add_mode) tf->text.prim_anchor = left;

    tf->text.pending_off = True;
    _XmTextFieldSetCursorPosition(tf, NULL, left + num_chars, False, True);
    (void) _XmTextFieldSetDestination(w, TextF_CursorPosition(tf),
				      _insert_select->event->time);
    if (_insert_select->select_type == XmDEST_SELECT) {
      if (left != right) {
	if (!dest_disjoint || !tf->text.add_mode) {
	  _XmTextFieldStartSelection(tf, TextF_CursorPosition(tf),
				     TextF_CursorPosition(tf),
				     _insert_select->event->time);
	}
      }
    }
    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = (XEvent *)_insert_select->event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
		       (XtPointer) &cb);
  }

  XtFree((char *)value);
  value = NULL;
  _insert_select->done_status = True;
}

/* ARGSUSED */
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
  enum { XmATEXT, XmACOMPOUND_TEXT, XmAUTF8_STRING, NUM_ATOMS };
  static char *atom_names[] = { XmSTEXT, XmSCOMPOUND_TEXT, XmSUTF8_STRING };

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
  else if (supports_CT)
    target = atoms[XmACOMPOUND_TEXT];
#if XM_UTF8
  else if (supports_utf8_string)
    target = atoms[XmAUTF8_STRING];
#endif
  else if (supports_encoding_data)
    target = CS_OF_ENCODING;
  else
    target = XA_STRING;

  XmTransferValue(tid, target,
		  (XtCallbackProc) TextFieldSecondaryWrapper,
		  closure, _insert_select -> event -> time);
}


/* ARGSUSED */
Boolean
_XmTextFieldConvert(
        Widget w,
        Atom *selection,
        Atom *target,
        Atom *type,
        XtPointer *value,
        unsigned long *length,
        int *format,
	Widget drag_context,
        XEvent *event)
{
  enum { XmA_MOTIF_DESTINATION, XmAINSERT_SELECTION, XmADELETE,
	 XmATARGETS, XmATEXT, XmACOMPOUND_TEXT, XmATIMESTAMP,
	 XmA_MOTIF_DROP, XmACLIPBOARD, XmANULL, XmAUTF8_STRING,
	 NUM_ATOMS };
  static char *atom_names[] = {
    XmS_MOTIF_DESTINATION, XmSINSERT_SELECTION, XmSDELETE,
    XmSTARGETS, XmSTEXT, XmSCOMPOUND_TEXT, XmSTIMESTAMP,
    XmS_MOTIF_DROP, XmSCLIPBOARD, XmSNULL, XmSUTF8_STRING };

  XmTextFieldWidget tf;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
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
  char *tmp_value;
  int ret_status = 0;
  Time _time;
  XmAnyCallbackStruct cb;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (req_event == NULL)
    _time = XtLastTimestampProcessed(XtDisplay(w));
  else
    _time = req_event -> time;

  tf = (XmTextFieldWidget) w;

  if (tf == NULL) return False;

  if (*selection == XA_PRIMARY || *selection == atoms[XmACLIPBOARD]) {
    has_selection = tf->text.has_primary;
    left = tf->text.prim_pos_left;
    right = tf->text.prim_pos_right;
    is_primary = True;
    is_secondary = is_destination = is_drop = False;
  } else if (*selection == atoms[XmA_MOTIF_DESTINATION]) {
    has_selection = tf->text.has_destination;
    is_destination = True;
    is_secondary = is_primary = is_drop = False;
  } else if (*selection == XA_SECONDARY) {
    has_selection = tf->text.has_secondary;
    left = tf->text.sec_pos_left;
    right = tf->text.sec_pos_right;
    is_secondary = True;
    is_destination = is_primary = is_drop = False;
  } else if (*selection == atoms[XmA_MOTIF_DROP]) {
    has_selection = tf->text.has_primary;
    left = tf->text.prim_pos_left;
    right = tf->text.prim_pos_right;
    is_drop = True;
    is_destination = is_primary = is_secondary = False;
  } else {
    return False;
  }

  /*
   * TARGETS identifies what targets the textfield widget can
   * provide data for.
   */
  if (*target == atoms[XmATARGETS]) {
    Atom *targs = XmeStandardTargets(w, 10, &target_count);

    *value = (XtPointer) targs;
    if (XA_STRING != CS_OF_ENCODING) {
      targs[target_count] = CS_OF_ENCODING;  target_count++;
    }
    if (is_primary || is_destination) {
      targs[target_count] = atoms[XmAINSERT_SELECTION]; target_count++;
    }
    if (is_primary || is_secondary || is_drop) {
      targs[target_count] = atoms[XmACOMPOUND_TEXT]; target_count++;
      targs[target_count] = atoms[XmATEXT]; target_count++;
      targs[target_count] = XA_STRING; target_count++;
      targs[target_count] = atoms[XmAUTF8_STRING]; target_count++;
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
      *timestamp = tf->text.prim_time;
    else if (is_destination)
      *timestamp = tf->text.dest_time;
    else if (is_secondary)
      *timestamp = tf->text.sec_time;
    else if (is_drop)
      *timestamp = tf->text.prim_time;
    *value = (XtPointer) timestamp;
    *type = XA_INTEGER;
    *length = sizeof(Time) / 4;
    *format = 32;
  } else if (*target == XA_STRING) {
    *type = (Atom) XA_STRING;
    *format = 8;
    if (is_destination || !has_selection) return False;

    /* put a char* value into tmp_value, then convert to 8859.1 */
    if (tf->text.max_char_size != 1) {
      int stat ;

      /* NOTE: casting (right - left) could result in a truncated long. */
      *length = _XmTextFieldCountBytes(tf, TextF_WcValue(tf) + left,
				       (int)(right - left));
      tmp_value = XtMalloc((unsigned) *length + 1);
      stat = wcstombs(tmp_value, TextF_WcValue(tf) + left,
		      (unsigned)*length); /* NOTE: casting *length could
					     result in a truncated long. */
      if (stat < 0) /* wcstombs will return neg value on conv failure */
	*length = 0;
      else *length = (unsigned long) stat ;
    } else {
      *length = right - left;
      tmp_value = XtMalloc((unsigned) *length + 1);
      /* get the selection value */
      (void)memcpy((void*)tmp_value, (void*)(TextF_Value(tf) + left),
		   (size_t)*length); /* NOTE: casting *length could result
					  in a truncated long. */
    }
    tmp_value[*length] = '\0';
    tmp_prop.value = NULL;
    /* convert tmp_value to 8859.1 */
    ret_status = XmbTextListToTextProperty(XtDisplay(w), &tmp_value, 1,
					   (XICCEncodingStyle)XStringStyle,
					   &tmp_prop);
    XtFree(tmp_value);
    if (ret_status == Success || ret_status > 0){
      *value = (XtPointer) tmp_prop.value;
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
    if (tf->text.max_char_size != 1) {
      int stat ;

      /* NOTE: casting (right - left) could result in a truncated long. */
      *length = _XmTextFieldCountBytes(tf, TextF_WcValue(tf) + left,
				       (int)(right - left));
      *value = XtMalloc((unsigned) *length + 1);
      stat = wcstombs((char *)*value, TextF_WcValue(tf) + left,
		      (unsigned)*length); /* NOTE: casting *length could
					     result in a truncated long */
      if (stat < 0) /* wcstombs return neg value on conv failure */
	*length = 0;
      else *length = (unsigned long) stat ;
    } else {
      *length = right - left;
      *value = XtMalloc((unsigned) *length + 1);
      /* get the selection value */
      (void)memcpy((void*)*value, (void*)(TextF_Value(tf) + left),
		   (size_t)*length); /* NOTE: casting *length could result
					  in a truncated long. */
    }
    (*(char **)value)[*length]='\0';
  } else if (*target == atoms[XmACOMPOUND_TEXT]) {
    *type = atoms[XmACOMPOUND_TEXT];
    *format = 8;
    if (is_destination || !has_selection) return False;
    if (tf->text.max_char_size != 1) {
      int stat ;

      /* convert to char* before converting to CT.  NOTE: casting
       * (right - left) could result in a truncated long.
       */
      *length = _XmTextFieldCountBytes(tf, TextF_WcValue(tf) + left,
				       (int)(right - left));
      tmp_value = XtMalloc((unsigned) *length + 1);
      stat = wcstombs(tmp_value, TextF_WcValue(tf) + left,
		      (unsigned)*length); /* NOTE: casting *length could
					     result in a truncated long. */
      if (stat < 0) /* wcstombs will return neg value on conv failure */
	*length = 0;
      else *length = (unsigned long) stat ;
    } else { /* malloc the space and copy the data to be converted */
      *length = right - left;
      tmp_value = XtMalloc((unsigned) *length + 1);
      /* get the selection value */
      (void)memcpy((void*)tmp_value, (void*)(TextF_Value(tf) + left),
		   (size_t)*length); /* NOTE: casting *length could result
					  in a truncated long. */
    }
    tmp_value[*length] = '\0';
    tmp_prop.value = NULL;
    /* Convert to compound text */
    ret_status =
      XmbTextListToTextProperty(XtDisplay(w), &tmp_value, 1,
				(XICCEncodingStyle)XCompoundTextStyle,
				&tmp_prop);
    XtFree(tmp_value);
    if (ret_status == Success || ret_status > 0){
      *length = tmp_prop.nitems;
      *value = (XtPointer)tmp_prop.value;
    } else {
      *value = NULL;
      *length = 0;
      return False;
    }
#if XM_UTF8
  } else if (*target == atoms[XmAUTF8_STRING]) {
    *type = atoms[XmAUTF8_STRING];
    *format = 8;
    if (is_destination || !has_selection) return False;
    if (tf->text.max_char_size != 1) {
      int stat ;

      /* convert to char* before converting to CT.  NOTE: casting
       * (right - left) could result in a truncated long.
       */
      *length = _XmTextFieldCountBytes(tf, TextF_WcValue(tf) + left,
				       (int)(right - left));
      tmp_value = XtMalloc((unsigned) *length + 1);
      stat = wcstombs(tmp_value, TextF_WcValue(tf) + left,
		      (unsigned)*length); /* NOTE: casting *length could
					     result in a truncated long. */
      if (stat < 0) /* wcstombs will return neg value on conv failure */
	*length = 0;
      else *length = (unsigned long) stat ;
    } else { /* malloc the space and copy the data to be converted */
      *length = right - left;
      tmp_value = XtMalloc((unsigned) *length + 1);
      /* get the selection value */
      (void)memcpy((void*)tmp_value, (void*)(TextF_Value(tf) + left),
		   (size_t)*length); /* NOTE: casting *length could result
					  in a truncated long. */
    }
    tmp_value[*length] = '\0';
    tmp_prop.value = NULL;
    /* Convert to compound text */
    ret_status =
      XmbTextListToTextProperty(XtDisplay(w), &tmp_value, 1,
				(XICCEncodingStyle)XUTF8StringStyle,
				&tmp_prop);
    XtFree(tmp_value);
    if (ret_status == Success || ret_status > 0){
      *length = tmp_prop.nitems;
      *value = (XtPointer)tmp_prop.value;
    } else {
      *value = NULL;
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
    Boolean move_cursor = True;

    if (!(is_primary || is_drop)) return False;

    left = tf->text.prim_pos_left;
    right = tf->text.prim_pos_right;

      if (is_drop) {
	if (_XmTextFieldGetDropReciever((Widget)tf) == (Widget) tf)
	  move_cursor = False;
      } else {
	if (req_event != NULL &&
	    req_event->requestor == XtWindow((Widget)tf))
	  move_cursor = False;
      }

    if (!_XmTextFieldReplaceText(tf, (XEvent *) req_event,
				 left, right, NULL, 0, move_cursor)) {
      tf->text.has_primary = True;
      return False;
    }

    _XmTextFieldStartSelection(tf, tf->text.prim_anchor,
			       tf->text.prim_anchor, _time);

    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = (XEvent *) req_event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
		       (XtPointer) &cb);

    tf->text.has_primary = True;

    if (tf->text.has_destination)
      tf->text.prim_anchor = TextF_CursorPosition(tf);

    *type = atoms[XmANULL];
    *value = NULL;
    *length = 0;
    *format = 8;
  } else
    /* unknown selection type */
    return FALSE;
  return TRUE;
}

/* ARGSUSED */
void
_XmTextFieldLoseSelection(
        Widget w,
        Atom *selection )
{
    XmTextFieldWidget tf = (XmTextFieldWidget) w;
    Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(w),
                                        XmS_MOTIF_DESTINATION, False);
/* Losing Primary Selection */
    if (*selection == XA_PRIMARY && tf->text.has_primary) {
        XmAnyCallbackStruct cb;
        _XmTextFieldDeselectSelection(w, False, 0);

        cb.reason = XmCR_LOSE_PRIMARY;
        cb.event = NULL;
        XtCallCallbackList(w, tf->text.lose_primary_callback, (XtPointer) &cb);
/* Losing Destination Selection */
    } else if (*selection == MOTIF_DESTINATION) {
        Boolean orig_ibeam_off = tf->text.refresh_ibeam_off;
        tf->text.has_destination = False;
       /* if we have focus, we have a valid putback area.  If we don't have
	* focus, don't want to update the putback with the destination cursor
	* image.
	*/
	tf->text.refresh_ibeam_off = False;
	_XmTextFieldDrawInsertionPoint(tf, False);
	tf->text.blink_on = True;
	_XmTextFieldDrawInsertionPoint(tf, True);
	/* Restore the state of the refresh_ibeam_off flag. */
        tf->text.refresh_ibeam_off = orig_ibeam_off;
/* Losing Secondary Selection */
    } else if (*selection == XA_SECONDARY && tf->text.has_secondary){
        _XmTextFieldSetSel2(w, 0, 0, True,
			    XtLastTimestampProcessed(XtDisplay(w)));
    }
}


static void
SetDropContext(Widget w)
{
  Display *display = XtDisplay(w);
  Screen *screen = XtScreen(w);
  XContext loc_context;


  _XmProcessLock();
  if (_XmTextFDNDContext == 0)
    _XmTextFDNDContext = XUniqueContext();
  loc_context = _XmTextFDNDContext;
  _XmProcessUnlock();

  XSaveContext(display, (Window)screen,
	       loc_context, (XPointer)w);
}

static void
DeleteDropContext(Widget w)
{
  Display *display = XtDisplay(w);
  Screen *screen = XtScreen(w);
  XContext loc_context;

  _XmProcessLock();
  loc_context = _XmTextFDNDContext;
  _XmProcessUnlock();

  XDeleteContext(display, (Window)screen, loc_context);
}

Widget
_XmTextFieldGetDropReciever(Widget w)
{
  Widget widget;
  XContext loc_context;

  _XmProcessLock();
  loc_context = _XmTextFDNDContext;
  _XmProcessUnlock();

  if (loc_context == 0) return NULL;

  if (!XFindContext(XtDisplay(w), (Window) XtScreen(w),
		    loc_context, (char **) &widget)) {
    return widget;
  }

  return NULL;
}


/* ARGSUSED */
static void
DropDestroyCB(Widget      w,
	      XtPointer   clientData,
	      XtPointer   callData)
{
  XmTransferDoneCallbackStruct *ts =
    (XmTransferDoneCallbackStruct *) callData;

  DeleteDropContext(w);
  if (ts->client_data != NULL) XtFree((char*) ts->client_data);
}

static void
DropTransferProc(Widget w, XtPointer closure,
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
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
  XmTextPosition insertPosLeft, insertPosRight, left, right, cursorPos;
  int max_length = 0;
  Boolean local = tf->text.has_primary;
  char * total_value = NULL;
  wchar_t * wc_total_value;
  unsigned long total_length = 0;
  int wc_total_length;
  Boolean replace = False;
  XmAnyCallbackStruct cb;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  /* When type = NULL, we are assuming a DELETE request has been requested */
  if (ds->type == atoms[XmANULL]) {
    if (transfer_rec->num_chars > 0 && transfer_rec->move) {
      tf->text.prim_anchor = transfer_rec->insert_pos;
      cursorPos = transfer_rec->insert_pos + transfer_rec->num_chars;
      _XmTextFieldSetCursorPosition(tf, NULL, cursorPos,
				    False, True);
      _XmTextFieldStartSelection(tf, tf->text.prim_anchor,
				 TextF_CursorPosition(tf),
				 XtLastTimestampProcessed(XtDisplay(w)));
      tf->text.pending_off = False;
      _XmTextFieldSetCursorPosition(tf, NULL, TextF_CursorPosition(tf),
				    True, True);
    }
    if (ds->value) {
      XtFree((char*) ds->value);
      ds->value = NULL;
    }
    return;
  }

  if (!(ds->value) ||
      (ds->type != CS_OF_ENCODING &&
       ds->type != atoms[XmACOMPOUND_TEXT] &&
#if XM_UTF8
       ds->type != atoms[XmAUTF8_STRING] &&
#endif
       ds->type != XA_STRING)) {
    XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);
    if (ds->value) {
      XtFree((char*) ds->value);
      ds->value = NULL;
    }
    return;
  }

  insertPosLeft = insertPosRight = transfer_rec->insert_pos;

  if (ds->type == XA_STRING
#if XM_UTF8
      || ds->type == atoms[XmAUTF8_STRING]
#endif
      || ds->type == atoms[XmACOMPOUND_TEXT]) {
    if ((total_value = _XmTextToLocaleText(w, ds->value, ds->type,
					   8, ds->length, NULL)) != NULL)
      total_length = strlen(total_value);
    else
      if (ds->value) {
	XtFree((char*) ds->value);
	ds->value = NULL;
      }
  } else {
    total_value = (char*) ds->value;
    total_length = ds->length;
  }

  if (total_value == NULL) return;

  if (TextF_PendingDelete(tf) && tf->text.has_primary &&
      tf->text.prim_pos_left != tf->text.prim_pos_right) {
      if(insertPosLeft > tf->text.prim_pos_left
		&& insertPosLeft <  tf->text.prim_pos_right)
        insertPosLeft = tf->text.prim_pos_left;
      if(insertPosRight < tf->text.prim_pos_right
		&& insertPosRight >  tf->text.prim_pos_left)
        insertPosRight = tf->text.prim_pos_right;
  }

  transfer_rec->num_chars = _XmTextFieldCountCharacters(tf, total_value,
							total_length);

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (transfer_rec->move && local) {
    max_length = TextF_MaxLength(tf);
    TextF_MaxLength(tf) = INT_MAX;
  }

  if (tf->text.max_char_size == 1) {
    replace = _XmTextFieldReplaceText(tf, ds->event, insertPosLeft,
				      insertPosRight, (char *) total_value,
				      (int)total_length, False);
  } else {
    wc_total_length = _XmTextFieldCountCharacters(tf, total_value,
                                                  total_length);
    wc_total_value = (wchar_t*)XtMalloc((unsigned)
					(wc_total_length+1) * sizeof(wchar_t));
    wc_total_length = mbstowcs(wc_total_value, total_value, wc_total_length+1);
    if (wc_total_length > 0)
      replace = _XmTextFieldReplaceText(tf, ds->event, insertPosLeft,
					insertPosRight, (char *)wc_total_value,
					wc_total_length, False);
    XtFree((char*)wc_total_value);
  }

  if (replace) {
    tf->text.pending_off = FALSE;
    if (transfer_rec->num_chars > 0 && !transfer_rec->move) {
      cursorPos = transfer_rec->insert_pos + transfer_rec->num_chars;
      _XmTextFieldSetCursorPosition(tf, NULL, cursorPos,
				    True, True);
      _XmTextFieldSetDestination((Widget)tf, TextF_CursorPosition(tf),
				 transfer_rec->timestamp);
    }
    left = tf->text.prim_pos_left;
    right = tf->text.prim_pos_right;
    if (tf->text.has_primary) {
      if (transfer_rec->move && left < transfer_rec->insert_pos)
	transfer_rec->insert_pos -= transfer_rec->num_chars;
      if (TextF_CursorPosition(tf) < left ||
	  TextF_CursorPosition(tf) > right)
	tf->text.pending_off = TRUE;
    } else {
      if (!transfer_rec->move && !tf->text.add_mode &&
	  transfer_rec->num_chars != 0)
	tf->text.prim_anchor = insertPosLeft;
    }
    if (transfer_rec->move) {
      XmTransferValue(ds->transfer_id,
		      atoms[XmADELETE],
		      (XtCallbackProc) DropTransferProc,
		      (XtPointer) transfer_rec, 0);
    }
    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = ds->event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
		       (XtPointer) &cb);
  }

  if (transfer_rec->move && local) {
    TextF_MaxLength(tf) = max_length;
  }

  if (total_value && (total_value != (char*)ds->value))
    XtFree(total_value);
  if (ds->value) {
    XtFree((char*) ds->value);
    ds->value = NULL;
  }

  _XmTextFieldDrawInsertionPoint(tf, True);
}

/*ARGSUSED*/
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
  static char *atom_names[] = {
    XmSNULL, XmSCLIPBOARD, XmSTEXT, XmSCOMPOUND_TEXT,
#if XM_UTF8
    XmSUTF8_STRING
#endif
    };

  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition right=0, left=0, replace_from, replace_to;
  int prim_char_length = 0;
  Boolean replace_res = False;
  XmAnyCallbackStruct cb;
  Atom atoms[XtNumber(atom_names)];
  _XmTextPrimSelect *_prim_select = (_XmTextPrimSelect *) closure;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (!tf->text.has_focus && _XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  if (ds->selection != atoms[XmACLIPBOARD] &&
      ds->length == 0 &&
      ds->type != atoms[XmANULL]) {
    /* Backwards compatibility for 1.0 Selections */
    _XmProcessLock();
    if (_prim_select->target == atoms[XmATEXT]) {
      _prim_select->target = XA_STRING;
      XmTransferValue(ds->transfer_id, XA_STRING, (XtCallbackProc) DoStuff,
		      (XtPointer) _prim_select, _prim_select->time);
    }
    _XmProcessUnlock();
    XtFree((char *)ds->value);
    ds->value = NULL;
    return;
  }

  /* if ds->length == 0 and ds->type is the NULL atom we are assuming
   * that a DELETE target is requested.
   */
  if (ds->type == atoms[XmANULL]) {
    _XmProcessLock();
    if (_prim_select->num_chars > 0 && tf->text.selection_move) {
      prim_char_length = _prim_select->num_chars;
      _XmTextFieldStartSelection(tf, _prim_select->position,
				 _prim_select->position + prim_char_length,
				 _prim_select->time);
      tf->text.pending_off = False;
      _XmTextFieldSetCursorPosition(tf, NULL,
				    _prim_select->position + prim_char_length,
				    True, True);
      tf->text.prim_anchor = _prim_select->position;
    }
    _XmProcessUnlock(); /* for _prim_select */
  } else {
    int max_length = 0;
    Boolean local = tf->text.has_primary;
    Boolean dest_disjoint = True;

    if (tf->text.selection_move && local) {
      max_length = TextF_MaxLength(tf);
      TextF_MaxLength(tf) = INT_MAX;
    }
    _XmProcessLock();
    replace_from = replace_to = _prim_select->position;
    _XmProcessUnlock();
    if (ds->selection == atoms[XmACLIPBOARD]) {
      if (tf->text.has_primary) {
	left = tf->text.prim_pos_left;
	right = tf->text.prim_pos_right;
	if (tf->text.pending_delete &&
	    replace_from >= left && replace_to <= right) {
          replace_from = left;
          replace_to = right;
          dest_disjoint = False;
	}
      }
    }

    if (ds->type == atoms[XmACOMPOUND_TEXT] ||
#if XM_UTF8
	ds->type == atoms[XmAUTF8_STRING] ||
#endif
	ds->type == XA_STRING) {
      char *total_value;

      if ((total_value =
	   _XmTextToLocaleText(w, ds->value, ds->type, ds->format, ds->length,
			       NULL))
	  != NULL) {
	if (tf->text.max_char_size == 1) {
	  _XmProcessLock();
	  _prim_select->num_chars = strlen(total_value);
	  replace_res =
	    _XmTextFieldReplaceText (tf, ds->event,
				     replace_from,
				     replace_to,
				     total_value,
				     _prim_select->num_chars,
				     ds->selection == atoms[XmACLIPBOARD]);
	  _XmProcessUnlock();
	  XtFree(total_value);
	} else {
	  wchar_t * wc_value;
	  int tmp_len = strlen(total_value) + 1;

	  _XmProcessLock();
	  _prim_select->num_chars = 0;
	  wc_value = (wchar_t*)XtMalloc ((unsigned) tmp_len * sizeof(wchar_t));
	  _prim_select->num_chars = mbstowcs(wc_value, total_value, tmp_len);
	  if (_prim_select->num_chars < 0)
	    _prim_select->num_chars = 0;
	  else
	    replace_res =
	      _XmTextFieldReplaceText(tf, ds->event,
				      replace_from,
				      replace_to,
				      (char*)wc_value,
				      _prim_select->num_chars,
				      ds->selection == atoms[XmACLIPBOARD]);
	  _XmProcessUnlock();
	  XtFree((char*)wc_value);
	  XtFree(total_value);
	}
      } else { /* initialize _prim_select values for possible delete oper */
	_XmProcessLock();
	_prim_select->num_chars = 0;
	_XmProcessUnlock();
      }
    } else {
      if (tf->text.max_char_size == 1) {
	/* Note: length may be truncated during cast to int */
	_XmProcessLock();
	_prim_select->num_chars = (int) ds->length;
	replace_res =
	  _XmTextFieldReplaceText(tf, ds->event,
				  replace_from,
				  replace_to,
				  (char *) ds->value,
				  _prim_select->num_chars,
				  ds->selection == atoms[XmACLIPBOARD]);
	_XmProcessUnlock();
      } else {
	wchar_t * wc_value;
	char *temp;

	temp = XtMalloc((unsigned) ds->length + 1);
	(void)memcpy((void*)temp, (void*)ds->value, (size_t)ds->length);
	temp[(size_t)ds->length] = '\0';

	wc_value = (wchar_t*)XtMalloc ((unsigned)
				       ((ds->length + 1) * sizeof(wchar_t)));
	_XmProcessLock();

	_prim_select->num_chars = mbstowcs(wc_value, (char *) temp,
					  (size_t) ds->length+1);

	if (_prim_select->num_chars < 0)
	  _prim_select->num_chars = 0;
	else {
	  wc_value[_prim_select->num_chars] = 0;
	  replace_res =
	    _XmTextFieldReplaceText(tf, ds->event,
				    replace_from,
				    replace_to,
				    (char*)wc_value,
				    _prim_select->num_chars,
				    ds->selection == atoms[XmACLIPBOARD]);
	}
	_XmProcessUnlock();
	XtFree(temp);
	XtFree((char*)wc_value);
      }
    }

    if (replace_res) {
      XmTextPosition cursorPos = 0;

      if (ds->selection != atoms[XmACLIPBOARD]) {
	tf->text.pending_off = FALSE;
	_XmProcessLock();
	cursorPos = replace_from + _prim_select->num_chars;
	if (_prim_select->num_chars > 0 && !tf->text.selection_move) {
	  _XmTextFieldSetCursorPosition(tf, NULL, cursorPos,
					True, True);
	  (void) _XmTextFieldSetDestination(w, cursorPos, _prim_select->time);
	_XmProcessUnlock();
	}
      } else {
	_XmProcessLock();
	(void) _XmTextFieldSetDestination(w, TextF_CursorPosition(tf),
					  _prim_select->time);
	_XmProcessUnlock();
      }
      left = tf->text.prim_pos_left;
      right = tf->text.prim_pos_right;
      if (tf->text.has_primary) {
	if (ds->selection == atoms[XmACLIPBOARD]) {
	  if (left != right && (!dest_disjoint || !tf->text.add_mode))
	    _XmProcessLock();
	    _XmTextFieldStartSelection(tf, TextF_CursorPosition(tf),
				       TextF_CursorPosition(tf),
				       _prim_select->time);
	    _XmProcessUnlock();
	} else {
	  _XmProcessLock();
	  if (tf->text.selection_move && left < _prim_select->position)
	    _prim_select->position -= _prim_select->num_chars;
	  if (left <= cursorPos && right >= cursorPos)
	    tf->text.pending_off = TRUE;
	  _XmProcessUnlock();
	}
      } else {
	_XmProcessLock();
	if (ds->selection == atoms[XmACLIPBOARD])
	  tf->text.prim_anchor = replace_from;
	else if (!tf->text.selection_move && !tf->text.add_mode &&
		 _prim_select->num_chars != 0)
	  tf->text.prim_anchor = _prim_select->position;
	_XmProcessUnlock();
      }
      cb.reason = XmCR_VALUE_CHANGED;
      cb.event = ds->event;
      XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			 (XtPointer) &cb);
    } else {
      _XmProcessLock();
      _prim_select->num_chars = 0; /* Stop SetPrimarySelection from doing
				     anything */
      _XmProcessUnlock();
    }

    if (tf->text.selection_move && local) {
      TextF_MaxLength(tf) = max_length;
    }
  }

  XtFree((char *)ds->value);
  ds->value = NULL;
}

static void
HandleTargets(Widget w, XtPointer closure,
	      XmSelectionCallbackStruct *ds)
{
  enum { XmACOMPOUND_TEXT, XmACLIPBOARD, XmATEXT,
#if XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSCLIPBOARD, XmSTEXT,
#if XM_UTF8
      XmSUTF8_STRING
#endif
      };

  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Atom atoms[XtNumber(atom_names)];
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
  XmTextPosition left, right;
  Boolean supports_encoding_data = False;
  Boolean supports_CT = False;
  Boolean supports_utf8_string = False;
  Boolean supports_text = False;
  XPoint *point = (XPoint *)closure;
  Atom *atom_ptr;
  Atom targets[2];
  XmTextPosition select_pos;
  int i;

  if (!ds->length) {
    XtFree((char *)ds->value);
    ds->value = NULL;
    return; /* Supports no targets, so don't bother sending anything */
  }

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

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
    select_pos = XmTextFieldXYToPos((Widget)tf, (Position)point->x, 0);
  } else {
    select_pos = TextF_CursorPosition(tf);
  }

  if (ds->selection != atoms[XmACLIPBOARD]) {
    left = tf->text.prim_pos_left;
    right = tf->text.prim_pos_right;
    if (tf->text.has_primary &&
	left != right && select_pos > left && select_pos < right) {
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

static void
HandleDrop(Widget w,
	   XmDropProcCallbackStruct *cb,
	   XmDestinationCallbackStruct *ds)
{
  Widget drag_cont, initiator;
  Cardinal numExportTargets, n;
  Atom *exportTargets;
  Atom desiredTarget = None;
  Arg args[10];
  XmTextPosition insert_pos, left, right;
  Display *display = XtDisplay(w);
  Boolean doTransfer = False;
  _XmTextDropTransferRec *transfer_rec;
  XtPointer tid = ds->transfer_id;

  drag_cont = cb->dragContext;
  transfer_rec = (_XmTextDropTransferRec *) NULL;

  n = 0;
  XtSetArg(args[n], XmNsourceWidget, &initiator); n++;
  XtSetArg(args[n], XmNexportTargets, &exportTargets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &numExportTargets); n++;
  XtGetValues((Widget) drag_cont, args, n);

  insert_pos = XmTextFieldXYToPos(w, cb->x, 0);

  left = ((XmTextFieldWidget)w)->text.prim_pos_left;
  right = ((XmTextFieldWidget)w)->text.prim_pos_right;
  if (cb->operation & XmDROP_MOVE && w == initiator &&
      ((XmTextFieldWidget)w)->text.has_primary &&
      left != right && insert_pos >= left && insert_pos <= right) {
    /*EMPTY*/
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
    Boolean utf8_string_found = False;
    Boolean c_text_found = False;
    Boolean string_found = False;
    Boolean text_found = False;

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

    /* intialize data to send to drop transfer callback */
    transfer_rec = (_XmTextDropTransferRec *)
      XtMalloc(sizeof(_XmTextDropTransferRec));
    transfer_rec->widget = w;
    transfer_rec->insert_pos = insert_pos;
    transfer_rec->num_chars = 0;
    transfer_rec->timestamp = cb->timeStamp;
    transfer_rec->move = False;

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
    if (encoding_found || c_text_found || string_found || text_found) {
      if (!encoding_found) {
#if XM_UTF8
	if (utf8_string_found)
	  desiredTarget = atoms[XmAUTF8_STRING];
	else
#endif
	if (c_text_found)
	  desiredTarget = atoms[XmACOMPOUND_TEXT];
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


/*ARGSUSED*/
static void
TextFieldConvertCallback(Widget w,
			 XtPointer ignore, /* unused */
			 XmConvertCallbackStruct *cs)
{
  enum { XmADELETE, XmA_MOTIF_LOSE_SELECTION,
	 XmA_MOTIF_EXPORT_TARGETS, XmA_MOTIF_CLIPBOARD_TARGETS,
	 XmACOMPOUND_TEXT, XmATEXT, XmATARGETS, XmACLIPBOARD,
#if XM_UTF8
         XmAUTF8_STRING,
#endif
	 NUM_ATOMS };
  static char *atom_names[] = { XmSDELETE, XmS_MOTIF_LOSE_SELECTION,
	 XmS_MOTIF_EXPORT_TARGETS, XmS_MOTIF_CLIPBOARD_TARGETS,
	 XmSCOMPOUND_TEXT, XmSTEXT, XmSTARGETS, XmSCLIPBOARD,
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

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  value = NULL;

  if (cs->target == atoms[XmA_MOTIF_LOSE_SELECTION]) {
    _XmTextFieldLoseSelection(w, &(cs->selection));
    cs->status = XmCONVERT_DONE;
    return;
  }

  if (cs->target == atoms[XmADELETE] &&
      cs->selection == XA_SECONDARY) {
    _XmTextFieldHandleSecondaryFinished(w, cs->event);
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

  if (!_XmTextFieldConvert(w, &cs->selection, &cs->target,
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

/*ARGSUSED*/
static void
FreeLocationData(Widget w,     /* unused */
		 XtEnum op,    /* unused */
		 XmTransferDoneCallbackStruct *ts)
{
  XmDestinationCallbackStruct *ds;

  ds = _XmTransferGetDestinationCBStruct(ts->transfer_id);

  XtFree((char*) ds->location_data);

  ds->location_data = NULL;
}

/*ARGSUSED*/
static void
TextFieldDestinationCallback(Widget w,
			     XtPointer closure, /* unused */
			     XmDestinationCallbackStruct *ds)
{
  enum { XmATARGETS, XmA_MOTIF_DROP, NUM_ATOMS };
  static char *atom_names[] = { XmSTARGETS, XmS_MOTIF_DROP };
  Atom atoms[XtNumber(atom_names)];
  XPoint DropPoint;

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
  } else if (ds->selection == XA_SECONDARY) {
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
		      (XtCallbackProc) TextFieldSecondaryWrapper,
		      (XtPointer) &insert_select, ds->time);
    } else {
      /*
       * Make selection request to replace the selection
       * with the insert selection.
       */
      XmTransferValue(ds->transfer_id, ((Atom) ds->location_data),
		      (XtCallbackProc) TextFieldSecondaryWrapper,
		      (XtPointer) &insert_select, ds->time);
    }
    _XmProcessUnlock();
  } else
    /* CLIPBOARD or PRIMARY */
    XmTransferValue(ds->transfer_id, atoms[XmATARGETS],
		    (XtCallbackProc) HandleTargets,
		    ds->location_data, ds->time);
}

void
_XmTextFieldInstallTransferTrait(void)
{
  XmeTraitSet((XtPointer)xmTextFieldWidgetClass, XmQTtransfer,
	      (XtPointer) &textFieldTT);
}
