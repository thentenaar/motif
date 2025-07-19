/* $XConsortium: TextFunc.c /main/16 1996/11/19 12:37:29 drk $ */
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/XmosP.h>
#include <Xm/TextStrSoP.h>
#include "XmI.h"
#include "TextFI.h"
#include "TextI.h"
#include "TextInI.h"
#include "TextStrSoI.h"

/****************************************************************
 *
 * Public definitions with TextField calls.
 *
 ****************************************************************/

XmTextPosition
XmTextGetLastPosition(Widget widget)
{
  XmTextPosition ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)){
    XmTextFieldWidget tf = (XmTextFieldWidget) widget;

    ret_val = tf->text.string_length;
  } else {
    XmTextSource source;

    source = GetSrc(widget);
    ret_val = (*source->Scan)(source, 0, XmSELECT_ALL, XmsdRight, 1, TRUE);
  }
  _XmAppUnlock(app);
  return ret_val;
}

void
_XmTextReplace(Widget widget,
	       XmTextPosition frompos,
	       XmTextPosition topos,
	       char *value,
               Boolean is_wchar)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextSource source;
  XmTextBlockRec block, newblock;
  Boolean editable, freeBlock;
  Boolean need_free = False;
  int max_length;
  int num_chars;
  wchar_t * tmp_wc;
  XmTextPosition selleft, selright, cursorPos;
  char * tmp_block = NULL;

  source = GetSrc(tw);

  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);

  if ((*source->GetSelection)(tw->text.source, &selleft, &selright)) {
    if ((selleft > frompos && selleft < topos)  ||
	(selright >frompos && selright < topos) ||
	(selleft <= frompos && selright >= topos)) {
      (*source->SetSelection)(tw->text.source, tw->text.cursor_position,
			      tw->text.cursor_position,
			      XtLastTimestampProcessed(XtDisplay(widget)));
      if (tw->text.input->data->pendingdelete)
	tw->text.pendingoff = FALSE;
    }
  }

  block.format = XmFMT_8_BIT;
  if (!is_wchar) {
    if (value == NULL)
      block.length = 0;
    else
      block.length = strlen(value);
    block.ptr = value;
  } else { /* value is really a wchar_t ptr cast to char* */
    if (value == NULL) {
      block.length = 0;
    } else {
      for (tmp_wc = (wchar_t*)value, num_chars = 0;
	   *tmp_wc != (wchar_t)0L;
	   num_chars++) tmp_wc++;
      tmp_block = XtMalloc((unsigned)
			   (num_chars + 1) * (int)tw->text.char_size);
      block.ptr = tmp_block;
      need_free = True;
      tmp_wc = (wchar_t *) value;
      /* if successful, wcstombs returns number of bytes, else -1 */
      block.length = wcstombs(block.ptr, tmp_wc,
			      (num_chars + 1) * (int)tw->text.char_size);
      if (block.length == -1) {
	block.length = 0; /* if error, don't insert anything */
	block.ptr[0] = '\0'; /* use the empty string */
      }
    }
  }
  editable = _XmStringSourceGetEditable(source);
  max_length = _XmStringSourceGetMaxLength(source);

  _XmStringSourceSetEditable(source, TRUE);
  _XmStringSourceSetMaxLength(source, INT_MAX);
  if (_XmTextModifyVerify(tw, NULL, &frompos, &topos,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    (*source->Replace)(tw, NULL, &frompos, &topos, &newblock, False);
    if (frompos == tw->text.cursor_position && frompos == topos) {
      _XmTextSetCursorPosition((Widget)tw, cursorPos);
    }
    _XmTextValueChanged(tw, NULL);
    if (UnderVerifyPreedit(tw))
      if (newblock.length != block.length ||
	strncmp(newblock.ptr, block.ptr, block.length) != 0) {
	VerifyCommitNeeded(tw) = True;
	PreEndTW(tw) += _XmTextCountCharacters(newblock.ptr, newblock.length)
			- _XmTextCountCharacters(block.ptr, block.length);
      }

    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  }
  else
    if (UnderVerifyPreedit(tw)) {
      VerifyCommitNeeded(tw) = True;
      PreEndTW(tw) -= _XmTextCountCharacters(block.ptr, block.length);
    }

  if (need_free)
    XtFree(tmp_block);
  _XmStringSourceSetEditable(source, editable);
  _XmStringSourceSetMaxLength(source, max_length);

  if (tw->text.input->data->has_destination)
    _XmTextSetDestinationSelection(widget, tw->text.cursor_position,
			   False, XtLastTimestampProcessed(XtDisplay(widget)));
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);

}

void
XmTextReplace(Widget widget,
	      XmTextPosition frompos,
	      XmTextPosition topos,
	      char *value)
{
  if (XmIsTextField(widget))
    XmTextFieldReplace(widget, frompos, topos, value);
  else {
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextReplace(widget, frompos, topos, value, False);
    _XmAppUnlock(app);
  }
}

void
XmTextReplaceWcs(Widget widget,
		 XmTextPosition frompos,
		 XmTextPosition topos,
		 wchar_t *value)
{
  if (XmIsTextField(widget))
    XmTextFieldReplaceWcs(widget, frompos, topos, (wchar_t*) value);
  else {
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextReplace(widget, frompos, topos, (char*) value, True);
    _XmAppUnlock(app);
  }
}

void
XmTextInsert(Widget widget,
	     XmTextPosition position,
	     char *value)
{
  XmTextReplace(widget, position, position, value);
}


void
XmTextInsertWcs(Widget widget,
		XmTextPosition position,
		wchar_t *wc_value)
{
  XmTextReplaceWcs(widget, position, position, wc_value);
}


void
XmTextSetAddMode(Widget widget,
                 Boolean state)
{
  if (XmIsTextField(widget))
    XmTextFieldSetAddMode(widget, state);
  else {
    XmTextWidget tw = (XmTextWidget) widget;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    if (tw->text.add_mode == state) {
	_XmAppUnlock(app);
	return;
    }

    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);
    tw->text.add_mode = state;
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
    _XmAppUnlock(app);
  }
}

Boolean
XmTextGetAddMode(Widget widget)
{
  Boolean ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)){
    XmTextFieldWidget tf = (XmTextFieldWidget) widget;
    ret_val = tf->text.add_mode;
  } else {
    XmTextWidget tw = (XmTextWidget) widget;
    ret_val = tw->text.add_mode;
  }
  _XmAppUnlock(app);
  return ret_val;
}

Boolean
XmTextGetEditable(Widget widget)
{
  Boolean ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget))
    ret_val = TextF_Editable(widget);
  else
    ret_val = _XmStringSourceGetEditable(GetSrc(widget));

  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextSetEditable(Widget widget,
                  Boolean editable)
{
  if (XmIsTextField(widget))
    XmTextFieldSetEditable(widget, editable);
  else {
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextSetEditable(widget, editable);
    _XmAppUnlock(app);
  }
}

int
XmTextGetMaxLength(Widget widget)
{
  int ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget))
    ret_val = TextF_MaxLength(widget);
  else
    ret_val = _XmStringSourceGetMaxLength(GetSrc(widget));

  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextSetMaxLength(Widget widget,
		   int max_length)
{
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget))
    TextF_MaxLength(widget) = max_length;
  else {
    XmTextWidget tw = (XmTextWidget) widget;

    tw->text.max_length = max_length;
    _XmStringSourceSetMaxLength(GetSrc(tw), max_length);
  }
  _XmAppUnlock(app);
}


XmTextPosition
XmTextGetInsertionPosition(Widget widget)
{
  XmTextPosition ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget))
    ret_val = TextF_CursorPosition(widget);
  else {
    XmTextWidget tw = (XmTextWidget) widget;

    ret_val = tw->text.cursor_position;
  }
  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextSetInsertionPosition(Widget widget,
			   XmTextPosition position)
{
  XmTextWidget tw = (XmTextWidget) widget;

  if (XmIsTextField(widget))
    XmTextFieldSetInsertionPosition(widget, position);
  else {
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextResetIC(widget);
    _XmTextSetCursorPosition(widget, position);

    _XmTextSetDestinationSelection(widget, tw->text.cursor_position,
		False, XtLastTimestampProcessed(XtDisplay(widget)));
    _XmAppUnlock(app);
  }
}

Boolean
XmTextRemove(Widget widget)
{
  if (XmIsTextField(widget))
    return(XmTextFieldRemove(widget));
  else {
    XmTextWidget tw = (XmTextWidget) widget;
    XmTextSource source;
    XmTextPosition left, right;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    if (tw->text.editable == False) {
      _XmAppUnlock(app);
      return False;
    }

    source = tw->text.source;
    _XmTextResetIC(widget);
    if (!(*source->GetSelection)(source, &left, &right) ||
	left == right) {
      tw->text.input->data->anchor = tw->text.cursor_position;
      _XmAppUnlock(app);
      return False;
    }

    XmTextReplace(widget, left, right, NULL);

    if (tw->text.cursor_position > left)
      _XmTextSetCursorPosition(widget, left);

    tw->text.input->data->anchor = tw->text.cursor_position;

    _XmAppUnlock(app);
    return True;
  }
}

Boolean
XmTextCopy(Widget widget,
	   Time copy_time)
{
  Boolean result = False;
  XmTextPosition left, right;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmTextGetSelectionPosition(widget, &left, &right) && right != left)
    /* start copy to clipboard */
    result = XmeClipboardSource(widget, XmCOPY, copy_time);
  _XmAppUnlock(app);

  return result;
}

Boolean
XmTextCopyLink(Widget widget,
	       Time copy_time)
{
  Boolean result = False;
  XmTextPosition left, right;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmTextGetSelectionPosition(widget, &left, &right) && right != left)
    /* start copy to clipboard */
    result = XmeClipboardSource(widget, XmLINK, copy_time);
  _XmAppUnlock(app);

  return result;
}

Boolean
XmTextCut(Widget widget,
	  Time cut_time)
{
  Boolean result = False;
  XmTextPosition left, right;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  /* can't cut if you can't edit */
  if (XmTextGetEditable(widget) &&
      XmTextGetSelectionPosition(widget, &left, &right) &&
      (right != left))
    /* start copy to clipboard */
    result = XmeClipboardSource(widget, XmMOVE, cut_time);
  _XmAppUnlock(app);

  return result;
}


/*
 * Retrieves the current data from the clipboard
 * and paste it at the current cursor position
 */
Boolean
XmTextPaste(Widget widget)
{
  Boolean status;
  InputData data;

  if (XmIsTextField(widget))
    return XmTextFieldPaste(widget);

  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  _XmTextResetIC(widget);
  data = ((XmTextWidget) widget)->text.input->data;

  data->selectionMove = FALSE;
  data->selectionLink = FALSE;
  status = XmeClipboardSink(widget, XmCOPY, NULL);
  _XmAppUnlock(app);
  return(status);
}

/*
 * Retrieves the current data from the clipboard
 * and paste it at the current cursor position
 */
Boolean
XmTextPasteLink(Widget widget)
{
  Boolean status;

  InputData data;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  data = ((XmTextWidget) widget)->text.input->data;

  data->selectionMove = FALSE;
  data->selectionLink = True;
  status = XmeClipboardSink(widget, XmLINK, NULL);
  _XmAppUnlock(app);
  return(status);
}

char *
XmTextGetSelection(Widget widget)
{
  if (XmIsTextField(widget))
    return(XmTextFieldGetSelection(widget));
  else {
    XmTextSource source;
    XmTextPosition left, right;
    char *ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    source = GetSrc(widget);
    if ((!(*source->GetSelection)(source, &left, &right)) || right == left)
    {
	_XmAppUnlock(app);
	return NULL;
    }

    ret_val = _XmStringSourceGetString((XmTextWidget)widget, left,
				    right, False);
    _XmAppUnlock(app);
    return ret_val;
  }
}

wchar_t *
XmTextGetSelectionWcs(Widget widget)
{
  if (XmIsTextField(widget))
    return(XmTextFieldGetSelectionWcs(widget));
  else {
    XmTextSource source;
    XmTextPosition left, right;
    wchar_t *ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    source = GetSrc(widget);
    if (!(*source->GetSelection)(source, &left, &right)) {
      _XmAppUnlock(app);
      return NULL;
    }

    ret_val = (wchar_t *)_XmStringSourceGetString((XmTextWidget)widget,
		left, right, True);
    _XmAppUnlock(app);
    return ret_val;
  }
}



void
XmTextSetSelection(Widget widget,
		   XmTextPosition first,
		   XmTextPosition last,
		   Time set_time)
{
  if (XmIsTextField(widget))
    XmTextFieldSetSelection(widget, first, last, set_time);
  else {
    XmTextSource source;
    XmTextWidget tw = (XmTextWidget) widget;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextResetIC(widget);
    if (first < 0 || last > tw->text.last_position) {
	_XmAppUnlock(app);
	return;
    }

    source = GetSrc(widget);
    source->data->take_selection = True;
    (*source->SetSelection)(source, first, last, set_time);
    tw->text.pendingoff = FALSE;
    _XmTextSetCursorPosition(widget, last);
    _XmTextSetDestinationSelection(widget, tw->text.cursor_position, False,
				   set_time);
    _XmAppUnlock(app);
  }
}

void
XmTextClearSelection(Widget widget,
		     Time clear_time)
{
  if (XmIsTextField(widget))
    XmTextFieldClearSelection(widget, clear_time);
  else {
    XmTextWidget tw = (XmTextWidget) widget;
    XmTextSource source = GetSrc(widget);
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    (*source->SetSelection)(source, 1, -999, source->data->prim_time);
    if (tw->text.input->data->pendingdelete) {
      tw->text.pendingoff = FALSE;
    }
    _XmAppUnlock(app);
  }
}

Boolean
XmTextGetSelectionPosition(Widget widget,
			   XmTextPosition *left,
			   XmTextPosition *right)
{
  Boolean ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)) {
    XmTextFieldWidget tf = (XmTextFieldWidget) widget;

    if (!tf->text.has_primary) {
      ret_val = False;
    } else {
      *left = tf->text.prim_pos_left;
      *right = tf->text.prim_pos_right;
      ret_val = True;
    }
  } else {
    XmTextWidget tw = (XmTextWidget) widget;
    ret_val = (*tw->text.source->GetSelection)(tw->text.source, left, right);
  }
  _XmAppUnlock(app);

  return ret_val;
}

XmTextPosition
XmTextXYToPos(Widget widget,
              Position x,
              Position y)
{
  if (XmIsTextField(widget))
    return(XmTextFieldXYToPos(widget, x, y));
  else {
    XmTextWidget tw = (XmTextWidget) widget;
    XmTextPosition ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    ret_val = (*tw->text.output->XYToPos)(tw, x, y);
    _XmAppUnlock(app);
    return ret_val;
  }
}

Boolean
XmTextPosToXY(Widget widget,
	      XmTextPosition position,
	      Position *x,
	      Position *y)
{
  if (XmIsTextField(widget))
    return(XmTextFieldPosToXY(widget, position, x, y));
  else {
    XmTextWidget tw = (XmTextWidget) widget;
    Boolean ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    ret_val = (*tw->text.output->PosToXY)(tw, position, x, y);
    _XmAppUnlock(app);
    return ret_val;
  }
}

/*
 * Force the given position to be displayed.  If position < 0, then don't force
 * any position to be displayed.
 */
void
XmTextShowPosition(Widget widget,
		   XmTextPosition position)
{
  if (XmIsTextField(widget))
    XmTextFieldShowPosition(widget, position);
  else {
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    _XmTextShowPosition(widget, position);
    _XmAppUnlock(app);
  }
}

int
XmTextGetBaseline(Widget widget)
{
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)) {
    XmTextFieldWidget tf = (XmTextFieldWidget) widget;
    Dimension margin_top;
    int ret_val;

    if(XmDirectionMatch(XmPrim_layout_direction(tf),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
      _XmAppUnlock(app);
      return(0);
    }

    margin_top = tf->text.margin_top +
        tf->primitive.shadow_thickness +
	tf->primitive.highlight_thickness;

    ret_val = (int) margin_top + (int) TextF_FontAscent(tf);
    _XmAppUnlock(app);
    return ret_val;
  } else {
    Dimension *baselines = NULL;
    int temp_bl = 0;
    int line_count = 0;
    XmPrimitiveClassExt           *wcePtr;
    XmTextWidget tw = (XmTextWidget) widget;

    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
      _XmAppUnlock(app);
      return(0);
    }

    wcePtr = _XmGetPrimitiveClassExtPtr(XtClass(widget), NULLQUARK);
    if (wcePtr && *wcePtr && (*wcePtr)->widget_baseline)
      (void)(*(*wcePtr)->widget_baseline)(widget, &baselines, &line_count);

    if (line_count)
      temp_bl = baselines[0];

    if (baselines)
      XtFree((XtPointer)baselines);
    _XmAppUnlock(app);
    return temp_bl;
  }
}

int
XmTextGetCenterline(Widget widget)
{
  Dimension *baselines = NULL;
  int temp_bl = 0;
  int line_count = 0;
  XmPrimitiveClassExt           *wcePtr;
  XmTextWidget tw = (XmTextWidget) widget;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
    _XmAppUnlock(app);
    return(0);
  }

  wcePtr = _XmGetPrimitiveClassExtPtr(XtClass(widget), NULLQUARK);
  if (wcePtr && *wcePtr && (*wcePtr)->widget_baseline)
    (void)(*(*wcePtr)->widget_baseline)(widget, &baselines, &line_count);

  if (line_count)
    temp_bl = baselines[0];

  if (baselines)
    XtFree((XtPointer)baselines);
  _XmAppUnlock(app);
  return temp_bl;
}

void
XmTextSetHighlight(Widget w,
		   XmTextPosition left,
		   XmTextPosition right,
		   XmHighlightMode mode)
{
  if (XmIsTextField(w)) {
    XmTextFieldSetHighlight(w, left, right, mode);
  } else {
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    _XmTextSetHighlight(w, left, right, mode);
    _XmAppUnlock(app);
  }
}

static int
_XmTextGetSubstring(Widget widget,
		    XmTextPosition start,
		    int num_chars,
		    int buf_size,
		    char *buffer,
                    Boolean want_wchar)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextBlockRec block;
  XmTextPosition pos, end;
  wchar_t * wc_buffer = (wchar_t*)buffer;
  int destpos = 0;

  end = start + num_chars;

  num_chars = 0; /* We're done with the value passed in, so let's
		  * re-use it when needed for the wchar functionality
		  * instead of creating a local automatic variable.
		  */

  for (pos = start; pos < end;) {
    pos = (*tw->text.source->ReadSource)(tw->text.source, pos, end,
					 &block);
    if (block.length == 0) {
      if (!want_wchar)
	buffer[destpos] = '\0';
      else
	wc_buffer[destpos] = (wchar_t)0L;
      return XmCOPY_TRUNCATED;
    }

    if (!want_wchar) {
      if (((destpos + block.length) * sizeof(char)) >= buf_size)
	return XmCOPY_FAILED;
    } else { /* Need number of characters for buffer comparison */
      num_chars = _XmTextCountCharacters(block.ptr, block.length);
      if (((destpos + num_chars) * sizeof(char)) >= buf_size)
	return XmCOPY_FAILED;
    }

    if (!want_wchar) {
      (void)memcpy((void*)&buffer[destpos], (void*)block.ptr,
		   block.length);
      destpos += block.length;
    } else { /* want wchar_t* data */
      num_chars = mbstowcs(&wc_buffer[destpos], block.ptr, num_chars);
      if (num_chars < 0) num_chars = 0;
      destpos += num_chars;
    }
  }

  if (!want_wchar)
    buffer[destpos] = '\0';
  else
    wc_buffer[destpos] = (wchar_t)0L;

  return XmCOPY_SUCCEEDED;
}

int
XmTextGetSubstring(Widget widget,
		   XmTextPosition start,
		   int num_chars,
		   int buf_size,
		   char *buffer)
{
  if (XmIsTextField(widget))
    return (XmTextFieldGetSubstring(widget, start, num_chars,
				    buf_size, buffer));
  else {
    int ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    ret_val =_XmTextGetSubstring(widget, start, num_chars, buf_size,
			       buffer, False);
    _XmAppUnlock(app);
    return ret_val;
  }
}

int
XmTextGetSubstringWcs(Widget widget,
		      XmTextPosition start,
		      int num_chars,
		      int buf_size,
		      wchar_t *buffer)
{
  if (XmIsTextField(widget))
    return (XmTextFieldGetSubstringWcs(widget, start, num_chars,
				       buf_size, buffer));
  else {
    int ret_val;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    ret_val =_XmTextGetSubstring(widget, start, num_chars, buf_size,
			       (char*) buffer, True);
    _XmAppUnlock(app);
    return ret_val;
  }
}
