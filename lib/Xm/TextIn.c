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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: TextIn.c /main/36 1999/01/27 16:10:29 mgreess $"
#endif
#endif

#include <stdio.h>
#include <string.h>
#include <Xm/XmosP.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/VendorP.h>
#include <Xm/AtomMgr.h>
#include <Xm/DisplayP.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/DragIconP.h>
#include <Xm/DropSMgr.h>
#include <Xm/ManagerP.h>
#include <Xm/ScreenP.h>
#include <Xm/TextSelP.h>
#include <Xm/TransltnsP.h>
#include "XmI.h"
#include "BaseClassI.h"
#include "DestI.h"
#include "DisplayI.h"
#include "MessagesI.h"
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "TextI.h"
#include "TextInI.h"
#include "TextOutI.h"
#include "TextStrSoI.h"
#include "TravActI.h"
#include "TraversalI.h"

#if USE_XFT
#include <X11/Xft/Xft.h>
#endif

#define MSG1	        _XmMMsgTextIn_0000
#define GRABKBDERROR	_XmMMsgRowColText_0024

/* For actions whose parameters are processed by reptype */
#define _RIGHT   0
#define _LEFT    1
#define _UP      0
#define _DOWN    1

#define TEXT_MAX_INSERT_SIZE 512

#define EraseInsertionPoint(tw)\
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off)

#define DisplayInsertionPoint(tw)\
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on)

typedef struct {
  Boolean has_destination;
  XmTextPosition position;
  long replace_length;
  Boolean quick_key;
  XmTextWidget widget;
} TextDestDataRec, *TextDestData;


/********    Static Function Declarations    ********/

static TextDestData GetTextDestData(Widget tw);


/* static void CheckSync(Widget, XtPointer, XEvent *, Boolean *); */

static void RingBell(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);

static Boolean DeleteOrKill(XmTextWidget tw,
			    XEvent *event,
			    XmTextPosition from,
			    XmTextPosition to,
                            Boolean kill,
			    XmTextPosition *cursorPos);

static void StuffFromBuffer(XmTextWidget tw,
                            XEvent *event,
                            int buffer);

static void UnKill(Widget w,
                   XEvent *event,
                   String *params,
                   Cardinal *num_params);

static void RemoveCurrentSelection(Widget w,
                                   XEvent *event,
                                   String *params,
                                   Cardinal *num_params,
                                   Boolean kill);

static void DeleteCurrentSelection(Widget w,
				   XEvent *event,
				   String *params,
				   Cardinal *num_params);

static void KillCurrentSelection(Widget w,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);

static void CheckDisjointSelection(Widget w,
                                   XmTextPosition position,
                                   Time sel_time);

static void SelfInsert(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void InsertString(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void ProcessVerticalParams(Widget w,
				  XEvent *event,
				  String *params,
				  Cardinal *num_params);

static void ProcessHorizontalParams(Widget w,
				    XEvent *event,
				    String *params,
				    Cardinal *num_params,
				    XmTextPosition *left,
				    XmTextPosition *right,
				    XmTextPosition *position);

static void ProcessSelectParams(Widget w,
				XEvent *event,
				XmTextPosition *left,
				XmTextPosition *right,
				XmTextPosition *position);

static XmTextPosition SelectOutLine(XmTextWidget tw,
                                    XmTextPosition position,
                                    XmTextScanDirection dir,
                                    int count);

static void KeySelection(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void SetAnchorBalancing(XmTextWidget tw,
			       XmTextPosition position);

static void SetNavigationAnchor(XmTextWidget tw,
				XmTextPosition old_position,
				XmTextPosition new_position,
				Time time,
                                Boolean extend);

static void CompleteNavigation(XmTextWidget tw,
			       XmTextPosition position,
			       Time time,
                               Boolean extend);

static void SimpleMovement(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params,
			   XmTextScanDirection dir,
			   XmTextScanType type,
                           Boolean include);

static void MoveForwardChar(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void MoveBackwardChar(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void MoveForwardWord(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void MoveBackwardWord(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void MoveForwardParagraph(Widget w,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);

static void MoveBackwardParagraph(Widget w,
				  XEvent *event,
				  String *params,
				  Cardinal *num_params);

static void MoveToLineStart(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void MoveToLineEnd(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void _MoveNextLine(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params,
                          Boolean pendingoff);

static void MoveNextLine(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void _MovePreviousLine(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params,
                              Boolean pendingoff);

static void MovePreviousLine(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void MoveNextPage(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void MovePreviousPage(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void MovePageLeft(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void MovePageRight(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void MovePageUp(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void MovePageDown(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void MoveBeginningOfFile(Widget w,
				XEvent *event,
				String *params,
				Cardinal *num_params);

static void MoveEndOfFile(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void ScrollOneLineUp(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void ScrollOneLineDown(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void ScrollCursorVertically(Widget w,
				   XEvent *event,
				   String *params,
				   Cardinal *num_params);

static void AddNewLine(Widget w,
		       XEvent *event,
                       Boolean move_cursor);

static void InsertNewLine(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void InsertNewLineAndBackup(Widget w,
				   XEvent *event,
				   String *params,
				   Cardinal *num_params);

static void InsertNewLineAndIndent(Widget w,
				   XEvent *event,
				   String *params,
				   Cardinal *num_params);

static void RedrawDisplay(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void Activate(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);

static void ToggleOverstrike(Widget w,
			     XEvent *event,
			     char **params,
			     Cardinal *num_params);

static void ToggleAddMode(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void SetCursorPosition(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void RemoveBackwardChar(Widget w,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params,
                               Boolean kill);

static void DeleteBackwardChar(Widget w,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params);

static void KillBackwardChar(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void RemoveForwardWord(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params,
                              Boolean kill);

static void DeleteForwardWord(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void KillForwardWord(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void RemoveBackwardWord(Widget w,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params,
                               Boolean kill);

static void DeleteBackwardWord(Widget w,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params);

static void KillBackwardWord(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void RemoveForwardChar(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params,
                              Boolean kill);

static void KillForwardChar(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void DeleteForwardChar(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void RemoveToEndOfLine(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params,
                              Boolean kill);

static void RemoveToStartOfLine(Widget w,
				XEvent *event,
				String *params,
				Cardinal *num_params,
                                Boolean kill);

static void DeleteToStartOfLine(Widget w,
				XEvent *event,
				String *params,
				Cardinal *num_params);

static void KillToStartOfLine(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void DeleteToEndOfLine(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void KillToEndOfLine(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void RestorePrimaryHighlight(InputData data,
				    XmTextPosition prim_left,
				    XmTextPosition prim_right);

static void SetSelectionHint(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void a_Selection(XmTextWidget tw,
                        Position x,
                        Position y,
                        Time sel_time,
			int set_empty_selection);

static void SetAnchor(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void DoSelection(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void SetScanType(Widget w,
                        InputData data,
                        XEvent *event);

static void StartPrimary(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void StartSecondary(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);

static void StartDrag(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void DragStart(XtPointer data,
		      XtIntervalId *id);

static void ProcessBDrag(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void ProcessBDragEvent(Widget w,
			      XEvent *event,
			      char **params,
			      Cardinal *num_params);

static Boolean InSelection(Widget w,
			   XEvent *event);

static void ProcessBSelect(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void ProcessBSelectEvent(Widget w,
				XEvent *event,
				char **params,
				Cardinal *num_params);

static Boolean dragged(SelectionHint selectionHint,
		       XEvent *event,
		       int threshold);

static void DoExtendedSelection(Widget w,
				Time ev_time);

static void DoSecondaryExtend(Widget w,
			      Time ev_time);

static void BrowseScroll(XtPointer closure,
			 XtIntervalId *id);

static Boolean CheckTimerScrolling(Widget w,
				   XEvent *event);

static void StartExtendSelection(Widget w,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);

static void ExtendSelection(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void ExtendSecondary(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void ExtendEnd(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void DoGrabFocus(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void MoveDestination(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void Stuff(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params);

static void SecondaryNotify(Widget w,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);

static void VoidAction(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void ExtendSecondaryEnd(Widget w,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params);

static void SelectAll(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void DeselectAll(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void ClearSelection(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);

static void ProcessBDragRelease(Widget w,
				XEvent *event,
				String *params,
				Cardinal *num_params);

static void ProcessCopy(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void ProcessLink(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void ProcessMove(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void CopyPrimary(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void CutPrimary(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void LinkPrimary(Widget w,
			XEvent *event,
			String *params,
			Cardinal *num_params);

static void CutClipboard(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void CopyClipboard(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void PasteClipboard(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);

static Boolean VerifyLeave(Widget w,
			   XEvent *event);

static void TextLeave(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void TextFocusIn(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void TextFocusOut(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void TraverseDown(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void TraverseUp(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void TraverseHome(Widget w,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);

static void TraverseNextTabGroup(Widget w,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);

static void TraversePrevTabGroup(Widget w,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);

static void ProcessCancel(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void ProcessReturn(Widget w,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);

static void ProcessTab(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);

static void ProcessUp(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void ProcessDown(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void ProcessShiftUp(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);

static void ProcessShiftDown(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void ProcessLeft(Widget w,
		        XEvent *event,
		        String *params,
		        Cardinal *num_params);

static void ProcessRight(Widget w,
                         XEvent *event,
                         String *params,
                         Cardinal *num_params);

static void ProcessShiftLeft(Widget w,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);

static void ProcessShiftRight(Widget w,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);

static void ProcessHome(Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);

static void Invalidate(XmTextWidget tw,
		       XmTextPosition position,
		       XmTextPosition topos,
		       long delta);

static void InputGetValues(Widget wid,
			   ArgList args,
			   Cardinal num_args);

static void InputSetValues(Widget oldw,
			   Widget reqw,
			   Widget new_w,
			   ArgList args,
			   Cardinal *num_args);

static void InputDestroy(Widget w);

static XtPointer InputBaseProc(Widget widget,
			       XtPointer client_data);

static void DragProcCallback(Widget w,
			     XtPointer client,
			     XtPointer call);

static void RegisterDropSite(Widget w);

static XmTextPosition XtoPosInLine(XmTextWidget tw,
				   Position x,
				   LineNum line);

static XmTextPosition YtoPosInLine(XmTextWidget tw,
				   Position y,
				   LineNum line);

/********    End Static Function Declarations    ********/



static XContext _XmTextDestContext = 0;

static XmConst XmTextScanType sarray[] = {
  XmSELECT_POSITION, XmSELECT_WORD, XmSELECT_LINE, XmSELECT_ALL
};


static XtResource input_resources[] = {
  {
    XmNselectionArray, XmCSelectionArray, XmRPointer, sizeof(XtPointer),
    XtOffsetOf(struct _InputDataRec, sarray),
    XmRImmediate, (XtPointer) sarray
  },

  {
    XmNselectionArrayCount, XmCSelectionArrayCount, XmRInt, sizeof(int),
    XtOffsetOf(struct _InputDataRec, sarraycount),
    XmRImmediate, (XtPointer) XtNumber(sarray)
  },

  {
    XmNpendingDelete, XmCPendingDelete, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _InputDataRec, pendingdelete),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNselectThreshold, XmCSelectThreshold, XmRInt, sizeof(int),
    XtOffsetOf(struct _InputDataRec, threshold),
    XmRImmediate, (XtPointer) 5
  },

};

static TextDestData
GetTextDestData(Widget tw)
{
  TextDestData dest_data;
  Display *display = XtDisplay(tw);
  Screen *screen = XtScreen(tw);
  XContext loc_context;

  _XmProcessLock();
  if (_XmTextDestContext == 0)
    _XmTextDestContext = XUniqueContext();
  loc_context = _XmTextDestContext;
  _XmProcessUnlock();

  if (XFindContext(display, (Window)screen,
		   loc_context, (char **) &dest_data)) {
    XmTextContextData ctx_data;
    Widget xm_display = (Widget) XmGetXmDisplay(display);

    ctx_data = (XmTextContextData) XtMalloc(sizeof(XmTextContextDataRec));

    ctx_data->screen = screen;
    ctx_data->context = loc_context;
    ctx_data->type = _XM_IS_DEST_CTX;

    dest_data = (TextDestData) XtCalloc(1, sizeof(TextDestDataRec));

    XtAddCallback(xm_display, XmNdestroyCallback,
		  (XtCallbackProc) _XmTextFreeContextData,
		  (XtPointer) ctx_data);

    XSaveContext(display, (Window)screen,
		 loc_context, (char *)dest_data);
  }

  return dest_data;
}

Boolean
_XmTextNeedsPendingDeleteDis(XmTextWidget tw,
			     XmTextPosition *left,
			     XmTextPosition *right,
			     int check_add_mode)
{
  InputData data = tw->text.input->data;

  if (!(*tw->text.source->GetSelection)(tw->text.source, left, right)) {
    *left = *right = tw->text.cursor_position;
    return False;
  } else
    if (check_add_mode && !tw->text.add_mode)
      return (*left != *right);
    else
      return (data->pendingdelete &&
              *left != *right && *left <= tw->text.cursor_position &&
              *right >= tw->text.cursor_position);
}


/*
 * static void
 * CheckSync(Widget w,
 * 	  XtPointer tmp,
 * 	  XEvent *event,
 * 	  Boolean *cont)
 * {
 *   XmTextWidget tw = (XmTextWidget) w;
 *   InputData data = tw->text.input->data;
 *   XEvent ev2;
 *   Boolean onewaiting;
 *
 *   if (XPending(XtDisplay(tw))) {
 *     XPeekEvent(XtDisplay(tw), &ev2);
 *     onewaiting = (ev2.xany.type == KeyPress &&
 * 		  ev2.xany.window == XtWindow(tw));
 *   } else onewaiting = FALSE;
 *   if (data->syncing) {
 *     if (!onewaiting) {
 *       data->syncing = FALSE;
 *       _XmTextEnableRedisplay(tw);
 *     }
 *   } else {
 *     if (onewaiting) {
 *       data->syncing = TRUE;
 *       _XmTextDisableRedisplay(tw, FALSE);
 *     }
 *   }
 * }
 */

static void
RingBell(Widget w,
	 XEvent *event,
	 String *params,
	 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.verify_bell)
    XBell(XtDisplay(tw), 0);
}


Boolean
_XmTextHasDestination(Widget w)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  return (data->has_destination);
}


Boolean
_XmTextSetDestinationSelection(Widget w,
			       XmTextPosition position,
			       Boolean disown,
			       Time set_time)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  Boolean result = TRUE;
  Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(w),
				       XmS_MOTIF_DESTINATION, False);

  if (!XtIsRealized(w)) return False;

  EraseInsertionPoint(tw);

  if (!disown) {
    if (!data->has_destination) {
      if (!set_time) set_time = _XmValidTimestamp(w);
      XmeSecondarySink(w, set_time);
      data->dest_time = set_time;
      data->has_destination = result;
      if (result) _XmSetDestination(XtDisplay(w), w);
    }
    tw->text.dest_position = position;
  } else {
    if (data->has_destination) {
      if (!set_time) set_time = _XmValidTimestamp(w);
      XtDisownSelection(w, MOTIF_DESTINATION, set_time);

      /* Call XmGetDestination(dpy) to get widget that last had
	 destination cursor. */
      if (w == XmGetDestination(XtDisplay(w)))
	_XmSetDestination(XtDisplay(w), (Widget)NULL);

      data->has_destination = False;
    }
  }

  DisplayInsertionPoint(tw);

  return result;
}

static Boolean
DeleteOrKill(XmTextWidget tw,
	     XEvent *event,
	     XmTextPosition from,
	     XmTextPosition to,
             Boolean kill,
             XmTextPosition *cursorPos)
{
  XmTextBlockRec block, newblock;
  Boolean freeBlock;
  char *ptr;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));


  _XmTextDisableRedisplay(tw,False);
  if (kill && from < to) {
    ptr = _XmStringSourceGetString(tw, from, to, False);
    XRotateBuffers(XtDisplay(tw), 1);
    XStoreBuffer(XtDisplay(tw), ptr, strlen(ptr), 0);
    XtFree(ptr);
  }
  block.ptr = "";
  block.length = 0;
  block.format = XmFMT_8_BIT;

  if (_XmTextModifyVerify(tw, event, &from, &to,
			  cursorPos, &block, &newblock, &freeBlock)) {
    if ((*tw->text.source->Replace)(tw, NULL, &from,
				    &to, &newblock, False) != EditDone) {
      _XmTextEnableRedisplay(tw);
      RingBell((Widget)tw, (XEvent *) NULL, (String *) NULL, (Cardinal) 0);
      if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
      return FALSE;
    } else {
      tw->text.needs_redisplay = tw->text.needs_refigure_lines = True;
      _XmTextEnableRedisplay(tw);
      _XmTextSetDestinationSelection((Widget)tw, tw->text.cursor_position,
				     False, event_time);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  } else {
    _XmTextEnableRedisplay(tw);
    RingBell((Widget)tw, (XEvent *) NULL, (String *) NULL, (Cardinal) 0);
    return FALSE;
  }
  return TRUE;
}

static void
StuffFromBuffer(XmTextWidget tw,
		XEvent *event,
		int buffer)
{
  XmTextPosition cursorPos;
  XmTextPosition from_pos, to_pos;
  XmTextBlockRec block, newblock;
  Boolean freeBlock;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  from_pos = to_pos = tw->text.cursor_position;
  block.ptr = XFetchBuffer(XtDisplay(tw), &(block.length), buffer);
  block.format = XmFMT_8_BIT;
  if (_XmTextModifyVerify(tw, event, &from_pos, &to_pos,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    if ((*tw->text.source->Replace)(tw, NULL, &from_pos, &to_pos,
				    &newblock, False) != EditDone) {
      RingBell((Widget)tw, (XEvent *) NULL, (String *) NULL, (Cardinal) 0);
    } else {
      _XmTextSetCursorPosition((Widget)tw, cursorPos);
      _XmTextSetDestinationSelection((Widget)tw, tw->text.cursor_position,
				     False, event_time);
      _XmTextValueChanged(tw, event);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  } else {
    RingBell((Widget)tw, (XEvent *) NULL, (String *) NULL, (Cardinal) 0);
  }
  if (block.ptr) XtFree(block.ptr);
}

static void
UnKill(Widget w,
       XEvent *event,
       String *params,
       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  StuffFromBuffer(tw, event, 0);
}

static void
RemoveCurrentSelection(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params,
                       Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, cursorPos, left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (!(*tw->text.source->GetSelection)(tw->text.source, &left, &right)) {
    XBell(XtDisplay(tw), 0);
  } else {
    if (!_XmStringSourceGetEditable(GetSrc(w))) {
      /* Why bother with any of the following? Nothing will happen! */
      return;
    }
    if (left < right) {
      cursorPos = tw->text.cursor_position;
      (*tw->text.source->SetSelection)(tw->text.source, cursorPos,
				       cursorPos, event_time);
      if (DeleteOrKill(tw, event, left, right, kill, &newCursorPos)) {
	if (cursorPos > left && cursorPos <= right) {
	  _XmTextSetCursorPosition(w, newCursorPos);
	  _XmTextSetDestinationSelection(w, tw->text.cursor_position,
					 False, event_time);
	}
	_XmTextValueChanged(tw, event);
      } else
	(*tw->text.source->SetSelection)(tw->text.source, left,
					 right, event_time);
    }
  }
}

static void
DeleteCurrentSelection(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params)
{
  _XmTextResetIC(w);
  RemoveCurrentSelection(w, event, params, num_params, FALSE);
}

static void
KillCurrentSelection(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  _XmTextResetIC(w);
  RemoveCurrentSelection(w, event, params, num_params, TRUE);
}

static void
CheckDisjointSelection(Widget w,
		       XmTextPosition position,
		       Time sel_time)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition left, right;
  InputData data = tw->text.input->data;

  left = right = position;

  if (tw->text.add_mode ||
      ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
       left != right && position >= left && position <= right))
    tw->text.pendingoff = FALSE;
  else
    tw->text.pendingoff = TRUE;

  if (left == right) {
    _XmTextSetDestinationSelection(w, position, False, sel_time);
    data->anchor = position;
  } else {
    _XmTextSetDestinationSelection(w, position, False, sel_time);
    if (!tw->text.add_mode) data->anchor = position;
  }
}

static Boolean
PrintableString(XmTextWidget tw,
		char* str,
		int n)
{
#ifdef SUPPORT_ZERO_WIDTH
  /* some locales (such as Thai) have characters that are
   * printable but non-spacing. These should be inserted,
   * even if they have zero width.
   */
  if (tw->text.char_size == 1) {
    int i;
    for (i = 0; i < n; i++) {
      if (!isprint((unsigned char)str[i])) {
	return False;
      }
    }
    return True;
  } else {
    /* tw->text.char_size > 1 */
    int i, csize;
    wchar_t wc;
    for (i = 0, csize = mblen(str, tw->text.char_size);
	 i < n;
	 i += csize, csize=mblen(&(str[i]), tw->text.char_size))
      {
	if (csize < 0)
	  return False;
	if (mbtowc(&wc, &(str[i]), tw->text.char_size) <= 0)
	  return False;
	if (!iswprint(wc)) {
	  return False;
	}
      }
    return True;
  }
#else /* SUPPORT_ZERO_WIDTH */
  OutputData o_data = tw->text.output->data;
  if (o_data->use_fontset) {
    return (XmbTextEscapement((XFontSet)o_data->font, str, n) != 0);
#if USE_XFT
  } else if (o_data->use_xft) {
    XGlyphInfo ext;
    XftTextExtentsUtf8(XtDisplay(tw), (XftFont*)o_data->font, (_Xconst FcChar8 *)str, n, &ext);
    return ext.xOff != 0;
#endif
  } else {
    return (XTextWidth(o_data->font, str, n) != 0);
  }
#endif /* SUPPORT_ZERO_WIDTH */
}

static void
SelfInsert(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  char str[TEXT_MAX_INSERT_SIZE+ 1];	/* NULL-terminated below */
  int n, i;
  XmTextPosition cursorPos, beginPos, nextPos, lastPos;
  XmTextPosition left, right;
  XmTextBlockRec block, newblock;
  Status status_return;
  Boolean pending_delete = False;
  Boolean freeBlock;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  /* Determine what was pressed.
   */
  n = XmImMbLookupString(w, (XKeyEvent *) event, str, TEXT_MAX_INSERT_SIZE,
			 (KeySym *) NULL, &status_return);

  /* If the user has input more data than we can handle, bail out */
  if (status_return == XBufferOverflow || n > TEXT_MAX_INSERT_SIZE)
    return;

  /* *LookupString in some cases can return the NULL as a character, such
   * as when the user types <Ctrl><back_quote> or <Ctrl><@>.  Text widget
   * can't handle the NULL as a character, so we dump it here.
   */

  for (i=0; i < n; i++)
    if (str[i] == 0) n = 0; /* just toss out the entire input string */

  if (n > 0) {
    EraseInsertionPoint(tw);
    str[n]='\0';

    if (!PrintableString(tw, str, n) && strchr(str, '\t') == NULL) {
      DisplayInsertionPoint(tw);
      return;
    }

    beginPos = nextPos = tw->text.cursor_position;
    if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, FALSE)) {
      beginPos = left;
      nextPos = right;
      pending_delete = True;
    } else if (data->overstrike) {
      nextPos += _XmTextCountCharacters(str, n);
      lastPos = (*(tw->text.source->Scan))(tw->text.source,
					   beginPos, XmSELECT_LINE,
					   XmsdRight, 1, TRUE);
      if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) {
	if (nextPos > lastPos) nextPos = lastPos;
      } else
	if (nextPos >= lastPos)
        {
	  if (lastPos < tw->text.source->data->length)
          {
	    nextPos = lastPos-1;
          }
	  else
          {
	    nextPos = lastPos;
          }
        }
    }
    block.ptr = str;
    block.length = n;
    block.format = XmFMT_8_BIT;
    if (_XmTextModifyVerify(tw, event, &beginPos, &nextPos,
			    &cursorPos, &block, &newblock, &freeBlock)) {
      if (pending_delete)
	(*tw->text.source->SetSelection)(tw->text.source, cursorPos,
					 cursorPos, event_time);
      if ((*tw->text.source->Replace)(tw, NULL, &beginPos, &nextPos,
				      &newblock, False) != EditDone) {
	RingBell(w, event, params, num_params);
      } else {
	cursorPos = _XmTextSetPreeditPosition(w, cursorPos);
	_XmTextSetCursorPosition(w, cursorPos);
	CheckDisjointSelection(w, tw->text.cursor_position,
			       event_time);
	_XmTextValueChanged(tw, event);
      }
      if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
    } else {
      RingBell(w, event, params, num_params);
    }
    DisplayInsertionPoint(tw);
  }
}

static void
InsertString(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newCursorPos, beginPos, nextPos, left, right;
  char *str;
  int i;
  XmTextBlockRec block, newblock;
  Boolean value_changed = False;
  Boolean pending_delete = False;
  Boolean freeBlock;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextDisableRedisplay(tw, TRUE);
  cursorPos = beginPos = nextPos = tw->text.cursor_position;

  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, FALSE)) {
    beginPos = left;
    nextPos = right;
    pending_delete = True;
  }


  for (i=0; i<*num_params; i++) {
    str = params[i];
    block.ptr = str;
    block.length = strlen(str);
    block.format = XmFMT_8_BIT;
    if (_XmTextModifyVerify(tw, event, &beginPos, &nextPos,
			    &newCursorPos, &block, &newblock, &freeBlock)) {
      if (pending_delete) {
	(*tw->text.source->SetSelection)(tw->text.source, cursorPos,
					 cursorPos, event_time);
	pending_delete = False;
      }
      if ((*tw->text.source->Replace)(tw, NULL, &beginPos, &nextPos,
				      &newblock, False) != EditDone) {
	RingBell(w, event, params, num_params);
	if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
	break;
      } else {
	if (freeBlock && newblock.ptr) {
	  XtFree(newblock.ptr);
	  newblock.ptr = NULL;
	}
	cursorPos = newCursorPos;
	value_changed = True;
      }
    } else {
      RingBell(w, event, params, num_params);
      break;
    }
  }

  if (value_changed) {
    _XmTextSetCursorPosition(w, cursorPos);
    CheckDisjointSelection(w, tw->text.cursor_position, event_time);
    _XmTextValueChanged(tw, event);
  }

  _XmTextEnableRedisplay(tw);

}

static void
ProcessVerticalParams(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params)
{
  char *dir;
  Cardinal num;
  int direction;
  XmTextWidget tw = (XmTextWidget) w;

  if (*num_params > 0)
  {
    if (XmDirectionMatch(XmPrim_layout_direction(tw),
				   XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_HORIZONTAL_DIRECTION_ACTION_PARAMS,
			   params[0], False, &direction) == True)
      {
	  if (direction == _RIGHT)
	  {
	      dir = "extend";
	      num = 1;
	      _MovePreviousLine(w, event, &dir, &num, False);
	  }
	  else if (direction == _LEFT)
	  {
	      dir = "extend";
	      num = 1;
	      _MoveNextLine(w, event, &dir, &num, False);
	  }
      }
    } else {
      if (_XmConvertActionParamToRepTypeId((Widget) w,
				   XmRID_TEXT_VERTICAL_DIRECTION_ACTION_PARAMS,
				   params[0], False, &direction) == True)
      {
	  if (direction == _UP)
	  {
	      dir = "extend";
	      num = 1;
	      _MovePreviousLine(w, event, &dir, &num, False);
	  }
	  else if (direction == _DOWN)
	  {
	      dir = "extend";
	      num = 1;
	      _MoveNextLine(w, event, &dir, &num, False);
	  }
      }
    }
  }
}

static void
ProcessHorizontalParams(Widget w,
			XEvent *event,
			String *params,
			Cardinal *num_params,
			XmTextPosition *left,
			XmTextPosition *right,
			XmTextPosition *position)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition old_cursorPos = tw->text.cursor_position;
  int direction;

  *position = (*tw->text.source->Scan)(tw->text.source,
				       tw->text.cursor_position,
				       XmSELECT_POSITION, XmsdRight, 1, False);

  if (!(*tw->text.source->GetSelection)
      (tw->text.source, left, right) || *left == *right) {
    data->origLeft = data->origRight = data->anchor;
    *left = *right = old_cursorPos;
  }

  /* move text cursor in direction of cursor key */
  if (*num_params > 0)
  {
    if (XmDirectionMatch(XmPrim_layout_direction(tw),
				   XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
	if (_XmConvertActionParamToRepTypeId((Widget) w,
				   XmRID_TEXT_VERTICAL_DIRECTION_ACTION_PARAMS,
				   params[0], False, &direction) == True)
	{
	    if (direction == _UP)
	      (*position)--;
	    else if(direction == _DOWN)
	      (*position)++;
	}
    } else {
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_HORIZONTAL_DIRECTION_ACTION_PARAMS,
			   params[0], False, &direction) == True)
      {
	  if (direction == _RIGHT)
	    (*position)++;
	  else if (direction == _LEFT)
	    (*position)--;
      }
    }
  }
}


static void
ProcessSelectParams(Widget w,
		    XEvent *event,
		    XmTextPosition *left,
		    XmTextPosition *right,
		    XmTextPosition *position)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  if (!((*tw->text.source->GetSelection)(tw->text.source, left, right))
      || *left == *right) {
    if (*position > data->anchor) {
      *left = data->anchor;
      *right = *position;
    } else {
      *left = *position;
      *right = data->anchor;
    }
  }
}

static XmTextPosition
SelectOutLine(XmTextWidget tw,
	      XmTextPosition position,
	      XmTextScanDirection dir,
	      int count)
{
  unsigned int line;

  line = _XmTextGetTableIndex(tw, position);
  if (dir == XmsdLeft) {
    count--;
    line -= count;
  } else {
    line += count;
  }
  if ((int)line < 0) line = 0;
  if (line < tw->text.total_lines)
    return tw->text.line_table[line].start_pos;
  else {
    XmTextSource source = GetSrc(tw);

    line = tw->text.total_lines - 1;
    return (*source->Scan)(source, tw->text.line_table[line].start_pos,
			   XmSELECT_ALL, XmsdRight, 1, TRUE);
  }
}


static void
KeySelection(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextPosition position, left, right, cursorPos;
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextScanDirection  cursorDir;                              /* PIR1858 */
  XmTextPosition       tempIndex;                              /* PIR1858 */
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));


  _XmTextResetIC(w);
  /* reset origLeft and origRight */
  (*tw->text.source->GetSelection)(tw->text.source,
				   &(data->origLeft), &(data->origRight));

  position = cursorPos = tw->text.cursor_position;

  data->selectionHint.x = data->selectionHint.y = 0;
  data->extending = TRUE;

  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  if (*num_params == 0) {
    position = cursorPos;
    ProcessSelectParams(w, event, &left, &right, &position);
  } else if (*num_params > 0)
  {

    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
    {
      if (_XmConvertActionParamToRepTypeId((Widget) w,
				 XmRID_TEXT_VERTICAL_DIRECTION_ACTION_PARAMS,
				 params[0], False, &value) == True)
      {
	SetAnchorBalancing(tw, cursorPos);
	ProcessHorizontalParams(w, event, params, num_params, &left,
				&right, &position);
      } else if (_XmConvertActionParamToRepTypeId((Widget) w,
			       XmRID_TEXT_HORIZONTAL_DIRECTION_ACTION_PARAMS,
			       params[0], False, &value) == True)
      {
        /* The only legal values for this reptype are "right" and "left". If
	   the parameter is a valid reptype, then it must be one of those
	   values. */
	ProcessVerticalParams(w, event, params, num_params);
	_XmTextEnableRedisplay(tw);
	data->extending = FALSE;
	DisplayInsertionPoint(tw);
	return;
      }
   }
    else
    {

      if (_XmConvertActionParamToRepTypeId((Widget) w,
			         XmRID_TEXT_HORIZONTAL_DIRECTION_ACTION_PARAMS,
			         params[0], False, &value) == True) {
        /* The only legal values for this reptype are "right" and "left". If
	   the parameter is a valid reptype, then it must be one of those
	   values. */
	SetAnchorBalancing(tw, cursorPos);
	ProcessHorizontalParams(w, event, params, num_params, &left,
			        &right, &position);
      } else if (_XmConvertActionParamToRepTypeId((Widget) w,
				   XmRID_TEXT_VERTICAL_DIRECTION_ACTION_PARAMS,
				   params[0], False, &value) == True) {
        /* The only legal values for this reptype are "up" and "down". If
	   the parameter is a valid reptype, then it must be one of those
	   values. */
	ProcessVerticalParams(w, event, params, num_params);
	_XmTextEnableRedisplay(tw);
	data->extending = FALSE;
	DisplayInsertionPoint(tw);
	return;
      }
    }
  }
  cursorPos = position;

  if (position < 0 || position > tw->text.last_position) {
    _XmTextEnableRedisplay(tw);
    DisplayInsertionPoint(tw);
    return;
  }

  if (cursorPos < data->anchor)
    data->extendDir = XmsdLeft;
  else if (cursorPos > data->anchor)
    data->extendDir = XmsdRight;

  cursorDir = data->extendDir;   /* PIR1858 */
  if (data->extendDir == XmsdRight) {
    if (cursorPos < right)  /* PIR1858:  We are backtracking  */
      cursorDir = XmsdLeft;  /* PIR1858 */
    if (data->stype == XmSELECT_OUT_LINE) {
      right = cursorPos = SelectOutLine(tw, position, cursorDir, 1);
    } else {
      right = cursorPos = (*tw->text.source->Scan)(tw->text.source,
						   position, data->stype,
						   cursorDir, 1, FALSE);
    }
    left = data->anchor;
  } else {
    if (cursorPos > left)  /* PIR1858: We are backtracking */
      cursorDir = XmsdRight;/* PIR1858 */
    if (data->stype == XmSELECT_OUT_LINE) {
      left = cursorPos = SelectOutLine(tw, position, cursorDir, 1);
    } else {
      left = cursorPos = (*tw->text.source->Scan)(tw->text.source,
						  position, data->stype,
						  cursorDir, 1, FALSE);
    }
    right = data->anchor;
  }

  if (left > right) {  /* PIR1858: We are on other side of anchor */
    tempIndex = left;
    left = right;
    right = tempIndex;
  }  /* end PIR1858 */

  (*tw->text.source->SetSelection)(tw->text.source, left, right,
				   event_time);
  tw->text.pendingoff = FALSE;
  _XmTextSetCursorPosition(w, cursorPos);
  _XmTextSetDestinationSelection(w, tw->text.cursor_position,
				 False, event_time);

  if (tw->text.auto_show_cursor_position &&
      cursorPos == tw->text.bottom_position)
    (*tw->text.output->MakePositionVisible)(tw, cursorPos);

  _XmTextEnableRedisplay(tw);

  /* reset origLeft and origRight */
  (*tw->text.source->GetSelection)(tw->text.source,
				   &(data->origLeft), &(data->origRight));
  data->extending = FALSE;
  DisplayInsertionPoint(tw);
}


static void
SetAnchorBalancing(XmTextWidget tw,
		   XmTextPosition position)
{
  InputData data = tw->text.input->data;
  XmTextPosition left, right;
  float bal_point;

  if (!((*tw->text.source->GetSelection)(tw->text.source, &left, &right))
      || left == right) {
    data->anchor = position;
  } else {
    bal_point = (float)(((float)(right - left) / 2.0) + (float)left);

    /* shift anchor and direction to opposite end of the selection */
    if ((float)position < bal_point) {
      data->extendDir = XmsdLeft;
      data->anchor = data->origRight;
    } else if ((float)position > bal_point) {
      data->extendDir = XmsdRight;
      data->anchor = data->origLeft;
    }
  }
}

static void
SetNavigationAnchor(XmTextWidget tw,
		    XmTextPosition old_position,
		    XmTextPosition new_position,
		    Time time,
                    Boolean extend)
{
  XmTextPosition left = old_position, right = old_position;
  InputData data = tw->text.input->data;
  Boolean has_selection;

  has_selection = ((*tw->text.source->GetSelection)(tw->text.source,
						    &left, &right)
		   && left != right);
  if (!tw->text.add_mode) {
    if (extend) {
      if (old_position < left || old_position > right)
	/* start outside selection - anchor at start position */
	data->anchor = old_position;
      else if (!has_selection ||
          ((left <= new_position) &&
          (new_position <= right)))
	/* no selection, or moving into selection */
	SetAnchorBalancing(tw, old_position);
      else
	/* moving to outside selection */
	SetAnchorBalancing(tw, new_position);
    } else {
      if (has_selection) {
	(*tw->text.source->SetSelection)(tw->text.source, old_position,
					 old_position, time);
	data->anchor = old_position;
      }
    }
  } else if (extend) {
    if (old_position < left || old_position > right)
      /* start outside selection - anchor at start position */
      data->anchor = old_position;
    else if (!has_selection ||
        ((left <= new_position) && (new_position <= right)))
      /* no selection, or moving into selection */
      SetAnchorBalancing(tw, old_position);
    else
      /* moving to outside selection */
      SetAnchorBalancing(tw, new_position);
  }
}

static void
CompleteNavigation(XmTextWidget tw,
		   XmTextPosition position,
		   Time time,
                   Boolean extend)
{
  XmTextPosition left, right;
  InputData data = tw->text.input->data;

  if ((tw->text.add_mode && (*tw->text.source->GetSelection)
       (tw->text.source, &left, &right) &&
       position >= left && position <= right) || extend)
    tw->text.pendingoff = FALSE;
  else
    tw->text.pendingoff = TRUE;

  if (extend) {
    if (data->anchor > position) {
      left = position;
      right = data->anchor;
    } else {
      left = data->anchor;
      right = position;
    }
    (*tw->text.source->SetSelection)(tw->text.source, left, right, time);

    data->origLeft = left;
    data->origRight = right;
  }
  _XmTextSetCursorPosition((Widget)tw, position);
}

static void
SimpleMovement(Widget w,
	       XEvent *event,
	       String *params ,
	       Cardinal *num_params ,
	       XmTextScanDirection dir,
	       XmTextScanType type,
               Boolean include)
{
  XmTextPosition cursorPos, newPos;
  XmTextWidget tw = (XmTextWidget) w;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  newPos = (*tw->text.source->Scan)(tw->text.source, cursorPos,
				    type, dir, 1, include);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);
  DisplayInsertionPoint(tw);
}


static void
MoveForwardChar(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params,
		 XmsdRight, XmSELECT_POSITION, TRUE);
  DisplayInsertionPoint(tw);
}

static void
MoveBackwardChar(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params,
		 XmsdLeft, XmSELECT_POSITION, TRUE);
  DisplayInsertionPoint(tw);
}

static void
MoveForwardWord(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition position, cursorPos;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  cursorPos = tw->text.cursor_position;

  EraseInsertionPoint(tw);
  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  /* Add if We only want to select to the end of the word w/o spaces
     if (*num_params > 0)
     {
        There is only one valid reptype value for this reptype, i.e.
	"extend". A True return value means that parameter was "extend".
     if (_XmConvertActionParamToRepTypeId((Widget) w,
		       XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
		       params[0], False, &value) == True)
     {
     position = (*tw->text.source->Scan)(tw->text.source, cursorPos,
     XmSELECT_WORD, XmsdRight, 1, FALSE);
     if(position == cursorPos) {
     position = (*tw->text.source->Scan)(tw->text.source, position,
     XmSELECT_WORD, XmsdRight, 1, TRUE);
     position = (*tw->text.source->Scan)(tw->text.source, position,
     XmSELECT_WORD, XmsdRight, 1, FALSE);
     }
     } else
     */
  position = (*tw->text.source->Scan)(tw->text.source, cursorPos,
				      XmSELECT_WORD, XmsdRight, 1, TRUE);

  SetNavigationAnchor(tw, cursorPos, position, event_time, extend);

  CompleteNavigation(tw, position, event_time, extend);
  DisplayInsertionPoint(tw);
}

static void
MoveBackwardWord(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition position, cursorPos;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  cursorPos = tw->text.cursor_position;

  EraseInsertionPoint(tw);
  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  position = (*tw->text.source->Scan)(tw->text.source, cursorPos,
				      XmSELECT_WORD, XmsdLeft, 1, FALSE);
  if(position == cursorPos) {
    position = (*tw->text.source->Scan)(tw->text.source, position,
                                        XmSELECT_WORD, XmsdLeft, 1, TRUE);
    position = (*tw->text.source->Scan)(tw->text.source, position,
                                        XmSELECT_WORD, XmsdLeft, 1, FALSE);
  }

  SetNavigationAnchor(tw, cursorPos, position, event_time, extend);

  CompleteNavigation(tw, position, event_time, extend);
  DisplayInsertionPoint(tw);
}

static void
MoveForwardParagraph(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params,
		 XmsdRight, XmSELECT_PARAGRAPH, FALSE);
  DisplayInsertionPoint(tw);
}

static void
MoveBackwardParagraph(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params,
		 XmsdLeft, XmSELECT_PARAGRAPH, FALSE);
  DisplayInsertionPoint(tw);
}

static void
MoveToLineStart(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  LineNum line;
  XmTextPosition position, cursorPos;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  EraseInsertionPoint(tw);

  _XmTextShowPosition(w, cursorPos);
  line = _XmTextPosToLine(tw, cursorPos);
  if (line == NOLINE) {
    XBell(XtDisplay(tw), 0);
  } else {
    _XmTextLineInfo(tw, line, &position, (LineTableExtra *) NULL);
    SetNavigationAnchor(tw, cursorPos, position, event_time, extend);
    CompleteNavigation(tw, position, event_time, extend);
  }
  DisplayInsertionPoint(tw);
}

static void
MoveToLineEnd(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  LineNum line;
  XmTextPosition position, cursorPos;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  cursorPos = tw->text.cursor_position;

  EraseInsertionPoint(tw);
  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  _XmTextShowPosition(w, cursorPos);
  line = _XmTextPosToLine(tw, cursorPos);
  if (line == NOLINE) {
    XBell(XtDisplay(tw), 0);
  } else {
    _XmTextLineInfo(tw, line+1, &position, (LineTableExtra *) NULL);
    if (position == PASTENDPOS)
      position = (*tw->text.source->Scan)(tw->text.source, position,
					  XmSELECT_ALL, XmsdRight, 1, TRUE);
    else
      position = (*tw->text.source->Scan)(tw->text.source, position,
					  XmSELECT_POSITION, XmsdLeft,
					  1, TRUE);
    SetNavigationAnchor(tw, cursorPos, position, event_time, extend);

    CompleteNavigation(tw, position, event_time, extend);
  }
  DisplayInsertionPoint(tw);
}

static void
_MoveNextLine(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params,
              Boolean pendingoff)
{
  XmTextWidget tw = (XmTextWidget) w;
  LineNum line;
  XmTextPosition currentPos, newPos, start, start2;
  XmTextPosition next;
  Position savePosX = tw->text.cursor_position_x;
  Position cur_x = 0, cur_y = 0;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;

  currentPos = tw->text.cursor_position;

  (*tw->text.output->PosToXY)(tw, currentPos, &cur_x, &cur_y);

  if (*num_params > 0) {
    /* There is only one valid reptype value for this reptype, i.e.
       "extend". A True return value means that parameter was "extend". */
    if (_XmConvertActionParamToRepTypeId((Widget) w,
				     XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
				     params[0], False, &value) == True) {
      extend = True;
    }
  }

  _XmTextShowPosition(w, currentPos);
  line = _XmTextPosToLine(tw, currentPos);
  if (line == NOLINE) {
    XBell(XtDisplay(tw), 0);
    return;
  }

  _XmTextLineInfo(tw, line+1, &start, (LineTableExtra *) NULL);
  if (start == PASTENDPOS) {
    newPos = (*tw->text.source->Scan)(tw->text.source, currentPos,
				      XmSELECT_ALL, XmsdRight, 1, TRUE);

    SetNavigationAnchor(tw, currentPos, newPos, event_time, extend);

    CompleteNavigation(tw, newPos, event_time, extend);
    tw->text.pendingoff = pendingoff;
  } else {
    /* If 16-bit characters are offset by half-width, need to handle it:
     * AABBCC...   Now move cursor down from beginning of CC
     * cDDEEFF...  Should be at beginning of either EE or FF, down again
     * GGHHII...   Should now be at beginning of II */
    /* Force Scroll to next line so that XtoPos..  won't fail */
    if (line == tw->text.number_lines - 1) {
      _XmTextShowPosition(w, start);
      /* This may cause a multi-line scroll.  We better reset line */
      line = _XmTextPosToLine(tw, start);
      if(XmDirectionMatch(XmPrim_layout_direction(tw),
			  XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
	newPos = YtoPosInLine(tw, cur_y, line);
      else
	newPos = XtoPosInLine(tw, savePosX, line);
    } else {
      if(XmDirectionMatch(XmPrim_layout_direction(tw),
			  XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
	newPos = YtoPosInLine(tw, cur_y, line+1);
      else
	newPos = XtoPosInLine(tw, savePosX, line+1);
    }

    next = (*tw->text.source->Scan)(tw->text.source, newPos,
				    XmSELECT_LINE, XmsdRight, 1, FALSE);

    SetNavigationAnchor(tw, currentPos, newPos, event_time, extend);

    CompleteNavigation(tw, newPos, event_time, extend);

    if (tw->text.cursor_position != next)
      tw->text.cursor_position_x = savePosX;
  }

  _XmTextShowPosition(w, tw->text.cursor_position);
  line = _XmTextPosToLine(tw, tw->text.cursor_position);
  if (line != NOLINE) {
    _XmTextLineInfo(tw, line, &start2, (LineTableExtra *) NULL);
    if (start2 != start && start != PASTENDPOS) {
      newPos = (*tw->text.source->Scan)(tw->text.source, start,
					XmSELECT_LINE, XmsdRight,
					1, FALSE);
      SetNavigationAnchor(tw, currentPos, newPos, event_time, extend);

      CompleteNavigation(tw, newPos, event_time, extend);
    }
  }
}

static void
MoveNextLine(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  _XmTextResetIC(w);
  _MoveNextLine(w, event, params, num_params, True);
}

static void
_MovePreviousLine(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params,
                  Boolean pendingoff)
{
  XmTextWidget tw = (XmTextWidget) w;
  LineNum line;
  XmTextPosition currentPos, newPos, origstart, start, start2;
  XmTextPosition	next;
  Boolean changed = False;
  Position savePosX = tw->text.cursor_position_x;
  Position cur_x = 0, cur_y = 0;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;

  /* Recompute lines if necessary */
  (void) XmTextGetTopCharacter(w);

  currentPos = tw->text.cursor_position;

  (*tw->text.output->PosToXY)(tw, currentPos, &cur_x, &cur_y);

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  _XmTextShowPosition(w, currentPos);
  line = _XmTextPosToLine(tw, currentPos);
  if (line == NOLINE) {
    XmeWarning(w, MSG1);
    newPos = currentPos;
  } else {
    _XmTextLineInfo(tw, line, &origstart, (LineTableExtra *) NULL);
    if (line == 0) {
      XmTextScroll(w, -1);
      line = _XmTextPosToLine(tw, currentPos);
      if (line == 0) {
	newPos = (*tw->text.source->Scan)(tw->text.source, currentPos,
					  XmSELECT_ALL, XmsdLeft, 1, TRUE);
	changed = True;
	goto done;
      }
      if (line == NOLINE) line = 1;
    }
    _XmTextLineInfo(tw, line-1, &start, (LineTableExtra *) NULL);
    /* If 16-bit characters are offset by half-width, need to handle it:
     * AABBCC...   Now move cursor up from beginning of II
     * cDDEEFF...  Should be at beginning of either EE or FF; up again
     * GGHHII...   Should now be at beginning of CC */
    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			  XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
      newPos = YtoPosInLine(tw, cur_y, line-1);
    else
      newPos = XtoPosInLine(tw, tw->text.cursor_position_x, line-1);
    next = (*tw->text.source->Scan)(tw->text.source, newPos, XmSELECT_LINE,
				    XmsdRight, 1, FALSE);
    if (newPos == next) changed = True;
    _XmTextShowPosition(w, newPos);
    line = _XmTextPosToLine(tw, newPos);
    if (line != NOLINE) {
      _XmTextLineInfo(tw, line, &start2, (LineTableExtra *) NULL);
      if (start2 != start) {
	newPos = (*tw->text.source->Scan)(tw->text.source, origstart,
					  XmSELECT_POSITION, XmsdLeft, 1,TRUE);
      }
    }
  }
 done:
  SetNavigationAnchor(tw, currentPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);
  if (!changed)
    tw->text.cursor_position_x = savePosX;
}

static void
MovePreviousLine(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  _MovePreviousLine(w, event, params, num_params, True);
}


static void
MoveNextPage(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x, y;
  int n, value;
  Boolean extend = False;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;

  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, cursorPos, &x, &y);

  n = _XmTextNumLines(tw);
  if (n > 1) n--;

  XmTextScroll(w, n);

  /* When y = 0, improper scrolling results.  This makes
   * sure no extra scroll results.
   */
  if (y <= 0) {
    OutputData o_data = tw->text.output->data;
    y += o_data->topmargin;
  }
  else {
    y -= tw->text.output->data->font_ascent;
  }

  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}



static void
MovePreviousPage(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x, y;
  int n, value;
  Boolean extend = False;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) return;

  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position, &x, &y);
  y -= tw->text.output->data->font_ascent;

  n = _XmTextNumLines(tw);
  if (n > 1) n--;

  XmTextScroll(w, -n);

  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}


static void
MovePageLeft(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x, y;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position, &x, &y);
  y -= tw->text.output->data->font_ascent;
  _XmTextChangeHOffset(tw, -tw->text.inner_widget->core.width);
  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}


static void
MovePageRight(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x, y;
  Boolean extend = False;
  int value;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position, &x, &y);
  y -= tw->text.output->data->font_ascent;
  _XmTextChangeHOffset(tw, tw->text.inner_widget->core.width);
  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}


static void
MovePageUp(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x = 0, y = 0;
  Boolean extend = False;
  int value = 0;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position, &x, &y);
  _XmTextChangeVOffset(tw, -tw->text.inner_widget->core.height);
  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}


static void
MovePageDown(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, newPos;
  Position x = 0, y = 0;
  Boolean extend = False;
  int value = 0;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, FALSE);

  cursorPos = tw->text.cursor_position;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXT_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position, &x, &y);
  _XmTextChangeVOffset(tw, tw->text.inner_widget->core.height);
  newPos = (*tw->text.output->XYToPos)(tw, x, y);

  SetNavigationAnchor(tw, cursorPos, newPos, event_time, extend);

  CompleteNavigation(tw, newPos, event_time, extend);

  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}


static void
MoveBeginningOfFile(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params, XmsdLeft, XmSELECT_ALL, TRUE);
  DisplayInsertionPoint(tw);
}



static void
MoveEndOfFile(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  SimpleMovement(w, event, params, num_params, XmsdRight, XmSELECT_ALL, TRUE);
  DisplayInsertionPoint(tw);
}

static void
ScrollOneLineUp(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  EraseInsertionPoint(tw);
  XmTextScroll(w, 1);
  DisplayInsertionPoint(tw);
}

static void
ScrollOneLineDown(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  EraseInsertionPoint(tw);
  XmTextScroll(w, -1);
  DisplayInsertionPoint(tw);
}

static void
ScrollCursorVertically(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition pos;
  LineNum desired_line, cur_line;
  int percentage;
  OutputData data = tw->text.output->data;

  if (*num_params == 0) {
    if (event)
      pos = (*tw->text.output->XYToPos)(tw, event->xbutton.x,
					event->xbutton.y);
    else
      pos = tw->text.cursor_position;
    if (pos == tw->text.line[tw->text.number_lines].start)
      desired_line = tw->text.number_lines-1;
    else
      for(desired_line=0; desired_line<tw->text.number_lines-1; desired_line++)
	if (tw->text.line[desired_line+1].start > pos) break;
  } else {
    tw->text.top_character = 0;
    tw->text.bottom_position = tw->text.last_position;
    sscanf(*params, "%d", &percentage);
    desired_line = ((data->number_lines - 1) * percentage) /100;
  }
  if (tw->text.cursor_position == tw->text.line[tw->text.number_lines].start)
    cur_line = tw->text.number_lines;
  else
    for (cur_line=0; cur_line<tw->text.number_lines; cur_line++)
      if (tw->text.line[cur_line+1].start > tw->text.cursor_position) break;

  XmTextScroll(w, (int)(cur_line - desired_line));
}

static void
AddNewLine(Widget w,
	   XEvent *event,
           Boolean move_cursor)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition cursorPos, beginPos, nextPos, left, right;
  XmTextBlockRec block, newblock;
  Boolean pending_delete = False;
  Boolean freeBlock;
  char str[32];
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  str[0] = '\n';
  str[1] = 0;
  block.length = 1;
  block.ptr = str;
  block.format = XmFMT_8_BIT;

  EraseInsertionPoint(tw);
  beginPos = nextPos = tw->text.cursor_position;
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, FALSE)) {
    beginPos = left;
    nextPos = right;
    pending_delete = True;
  }
  if (_XmTextModifyVerify(tw, event, &beginPos, &nextPos,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    if (pending_delete) {
      (*tw->text.source->SetSelection)(tw->text.source, cursorPos,
				       cursorPos, event_time);
    }
    if ((*tw->text.source->Replace)(tw, NULL, &beginPos, &nextPos,
				    &newblock, False) != EditDone) {
      if (tw->text.verify_bell) XBell(XtDisplay(tw), 0);
    } else {
      if (move_cursor) {
	_XmTextSetCursorPosition(w, cursorPos);
      } else {
	_XmTextSetCursorPosition(w, beginPos);
      }
      CheckDisjointSelection(w, tw->text.cursor_position,
			     event_time);
      _XmTextValueChanged(tw, event);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  } else {
    if (tw->text.verify_bell) XBell(XtDisplay(tw), 0);
  }
  DisplayInsertionPoint(tw);
}

static void
InsertNewLine(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  AddNewLine(w, event, True);
  DisplayInsertionPoint(tw);
}

static void
InsertNewLineAndBackup(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  AddNewLine(w, event, False);
  DisplayInsertionPoint(tw);
}

static void
InsertNewLineAndIndent(Widget w,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextBlockRec block, newblock;
  XmTextPosition  pos, from_pos, to_pos, left, right, cursorPos, newCursorPos;
  Boolean freeBlock, value_changed = False;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  _XmTextDisableRedisplay(tw, TRUE);
  pos = tw->text.cursor_position;
  left = (*tw->text.source->Scan)(tw->text.source, pos,
				  XmSELECT_LINE, XmsdLeft,  1, FALSE);
  if(left != (*tw->text.source->Scan)(tw->text.source, left,
				      XmSELECT_WHITESPACE, XmsdRight, 1,
				      FALSE)) {
    AddNewLine(w, event, True);
  } else {
    right = (*tw->text.source->Scan)(tw->text.source, left,
				     XmSELECT_WHITESPACE, XmsdRight, 1, TRUE);
    if(right > pos)
      right = pos;
    AddNewLine(w, event, True);
    cursorPos = from_pos = to_pos = tw->text.cursor_position;
    while(left < right) {
      left=(*tw->text.source->ReadSource)(tw->text.source,
					  left, right, &block);
      if (_XmTextModifyVerify(tw, event, &from_pos, &to_pos,
			      &newCursorPos, &block, &newblock, &freeBlock)) {
	if ((*tw->text.source->Replace)(tw, NULL, &from_pos, &to_pos,
					&newblock, False) != EditDone) {
	  RingBell(w, event, params, num_params);
	  if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
	  break;
	} else {
	  cursorPos = newCursorPos;
	  if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
	  value_changed = True;
	}
      } else {
	RingBell(w, event, params, num_params);
	break;
      }
    }
    _XmTextSetCursorPosition(w, cursorPos);
    CheckDisjointSelection(w, tw->text.cursor_position, event_time);
    if (value_changed)
      _XmTextValueChanged(tw, event);
  }
  _XmTextEnableRedisplay(tw);
  DisplayInsertionPoint(tw);
}

static void
RedrawDisplay(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition top = tw->text.top_character;

  _XmTextInvalidate(tw, top, top, NODELTA);
}

static void
Activate(Widget w,
	 XEvent *event,
	 String *params,
	 Cardinal *num_params)
{
  XmAnyCallbackStruct cb;
  XmTextWidget tw = (XmTextWidget) w;
  XmParentInputActionRec  p_event;

  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_ACTIVATE;
  p_event.event = event;            /* Pointer to XEvent. */
  p_event.params = params;          /* Or use what you have if   */
  p_event.num_params = num_params;  /* input is from translation.*/

  cb.reason = XmCR_ACTIVATE;
  cb.event  = event;
  XtCallCallbackList(w, tw->text.activate_callback, (XtPointer) &cb);

  (void) _XmParentProcess(XtParent(tw), (XmParentProcessData) &p_event);
}

static void
ToggleOverstrike(Widget w,
		 XEvent *event,
		 char **params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  data->overstrike = !data->overstrike;
  o_data->refresh_ibeam_off = True;
  if (data->overstrike)
    o_data->cursorwidth = o_data->cursorheight >> 1;
  else {
    o_data->cursorwidth = 5;
    if (o_data->cursorheight > 19)
      o_data->cursorwidth++;
  }
  DisplayInsertionPoint(tw);
}


static void
ToggleAddMode(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition left, right;

  EraseInsertionPoint(tw);

  tw->text.add_mode = !tw->text.add_mode;

  DisplayInsertionPoint(tw);

  if (tw->text.add_mode &&
      (!(*tw->text.source->GetSelection)(data->widget->text.source,
					 &left, &right) || left == right)) {
    data->anchor = tw->text.dest_position;
  }
}


static void
SetCursorPosition(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  _XmTextResetIC(w);
  if (event)
    _XmTextSetCursorPosition(w, (*tw->text.output->XYToPos)(tw,
							    event->xbutton.x,
							    event->xbutton.y));
}

static void
RemoveBackwardChar(Widget w,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params,
     Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, cursorPos, nextPos, left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  cursorPos = nextPos = tw->text.cursor_position;

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    nextPos = tw->text.cursor_position;
    cursorPos = (*tw->text.source->Scan)(tw->text.source, nextPos,
					 XmSELECT_POSITION, XmsdLeft, 1, TRUE);
    if (DeleteOrKill(tw, event, cursorPos, nextPos, kill, &newCursorPos)) {
      _XmTextSetCursorPosition(w, newCursorPos);
      CheckDisjointSelection(w, tw->text.cursor_position,
			     event_time);
      _XmTextValueChanged(tw, event);
    }
  }
  DisplayInsertionPoint(tw);
}

static void
DeleteBackwardChar(Widget w,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveBackwardChar(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
KillBackwardChar(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveBackwardChar(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}


static void
RemoveForwardWord(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params,
                  Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    _XmTextDisableRedisplay(tw, TRUE);
    left = tw->text.cursor_position;
    right = (*tw->text.source->Scan)(tw->text.source, left,
				     XmSELECT_WORD, XmsdRight, 1, TRUE);

    if (left < right) {
      if (DeleteOrKill(tw, event, left, right, kill, &newCursorPos)) {
	_XmTextSetCursorPosition(w, newCursorPos);
	CheckDisjointSelection(w, tw->text.cursor_position,
			       event_time);
	_XmTextValueChanged(tw, event);
      }
    }
    _XmTextEnableRedisplay(tw);
  }
  DisplayInsertionPoint(tw);
}

static void
DeleteForwardWord(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveForwardWord(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
KillForwardWord(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveForwardWord(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}

static void
RemoveBackwardWord(Widget w,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params,
                   Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    _XmTextDisableRedisplay(tw, TRUE);
    right = tw->text.cursor_position;
    left = (*tw->text.source->Scan)(tw->text.source, right,
				    XmSELECT_WORD, XmsdLeft, 1, FALSE);
    if (left == right) {
      left = (*tw->text.source->Scan)(tw->text.source, left,
				      XmSELECT_WORD, XmsdLeft, 1, TRUE);
      left = (*tw->text.source->Scan)(tw->text.source, left,
				      XmSELECT_WORD, XmsdLeft, 1, FALSE);
    }
    if (left < right) {
      if (DeleteOrKill(tw, event, left, right, kill, &newCursorPos)) {
	_XmTextSetCursorPosition(w, newCursorPos);
	CheckDisjointSelection(w, tw->text.cursor_position,
			       event_time);
	_XmTextValueChanged(tw, event);
      }
    }
    _XmTextEnableRedisplay(tw);
  }
  DisplayInsertionPoint(tw);
}

static void
DeleteBackwardWord(Widget w,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveBackwardWord(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
KillBackwardWord(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveBackwardWord(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}

static void
RemoveForwardChar(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params,
                  Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, cursorPos, nextPos, left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    cursorPos = tw->text.cursor_position;
    nextPos = (*tw->text.source->Scan)(tw->text.source, cursorPos,
				       XmSELECT_POSITION, XmsdRight, 1, TRUE);
    if (DeleteOrKill(tw, event, cursorPos, nextPos, kill, &newCursorPos)) {
      _XmTextSetCursorPosition(w, newCursorPos);
      CheckDisjointSelection(w, tw->text.cursor_position,event_time);
      _XmTextValueChanged(tw, event);
    }
  }
  DisplayInsertionPoint(tw);
}

static void
KillForwardChar(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveForwardChar(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}

static void
DeleteForwardChar(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveForwardChar(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
RemoveToEndOfLine(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params,
                  Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, left, right;
  LineNum line;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    _XmTextDisableRedisplay(tw, TRUE);
    left = tw->text.cursor_position;
    line = _XmTextPosToLine(tw, left);
    if (line == NOLINE)
      right = left;
    else {
      _XmTextLineInfo(tw, line+1, &right, (LineTableExtra *) NULL);
      if (right == PASTENDPOS)
	right = (*tw->text.source->Scan)(tw->text.source, right,
					 XmSELECT_ALL, XmsdRight, 1, TRUE);
      else
	right = (*tw->text.source->Scan)(tw->text.source, right,
					 XmSELECT_POSITION, XmsdLeft, 1, TRUE);
    }
    if (left < right) {
      if (DeleteOrKill(tw, event, left, right, kill, &newCursorPos)) {
	_XmTextSetCursorPosition(w, newCursorPos);
	CheckDisjointSelection(w, tw->text.cursor_position,event_time);
	_XmTextValueChanged(tw, event);
      }
    } else if (left == right)
      DeleteForwardChar(w, event, params, num_params);

    _XmTextEnableRedisplay(tw);
  }
  DisplayInsertionPoint(tw);
}

static void
RemoveToStartOfLine(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params,
                    Boolean kill)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition newCursorPos, left, cursorPos, right;
  LineNum line;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, TRUE)) {
    RemoveCurrentSelection(w, event, params, num_params, kill);
  } else {
    _XmTextDisableRedisplay(tw, TRUE);
    cursorPos = tw->text.cursor_position;
    _XmTextShowPosition(w, cursorPos);
    line = _XmTextPosToLine(tw, cursorPos);
    if (line == NOLINE) {
      XBell(XtDisplay(tw), 0);
    } else {
      _XmTextLineInfo(tw, line, &left, (LineTableExtra *) NULL);
      if (left < cursorPos) {
	if (DeleteOrKill(tw, event, left, cursorPos, kill, &newCursorPos)) {
	  _XmTextSetCursorPosition(w, newCursorPos);
	  CheckDisjointSelection(w, tw->text.cursor_position,
				 event_time);
	  _XmTextValueChanged(tw, event);
	}
      } else if (left == cursorPos)
	DeleteBackwardChar(w, event, params, num_params);

    }
    _XmTextEnableRedisplay(tw);
  }
  DisplayInsertionPoint(tw);
}

static void
DeleteToStartOfLine(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveToStartOfLine(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
KillToStartOfLine(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveToStartOfLine(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}

static void
DeleteToEndOfLine(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveToEndOfLine(w, event, params, num_params, FALSE);
  DisplayInsertionPoint(tw);
}

static void
KillToEndOfLine(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  RemoveToEndOfLine(w, event, params, num_params, TRUE);
  DisplayInsertionPoint(tw);
}

static void
RestorePrimaryHighlight(InputData data,
			XmTextPosition prim_left,
			XmTextPosition prim_right)
{
  if (data->sel2Right >= prim_left && data->sel2Right <= prim_right) {
    /* secondary selection is totally inside primary selection */
    if (data->sel2Left >= prim_left) {
      _XmTextSetHighlight((Widget)data->widget, prim_left, data->sel2Left,
			 XmHIGHLIGHT_SELECTED);
      _XmTextSetHighlight((Widget)data->widget, data->sel2Left, data->sel2Right,
			 XmHIGHLIGHT_NORMAL);
      _XmTextSetHighlight((Widget)data->widget, data->sel2Right, prim_right,
			 XmHIGHLIGHT_SELECTED);
      /* right side of secondary selection is inside primary selection */
    } else {
      _XmTextSetHighlight((Widget)data->widget, prim_left, data->sel2Right,
			 XmHIGHLIGHT_SELECTED);
      _XmTextSetHighlight((Widget)data->widget, data->sel2Left, prim_left,
			 XmHIGHLIGHT_NORMAL);
    }
  } else {
    /* left side of secondary selection is inside primary selection */
    if (data->sel2Left <= prim_right && data->sel2Left >= prim_left) {
      _XmTextSetHighlight((Widget)data->widget, data->sel2Left, prim_right,
			 XmHIGHLIGHT_SELECTED);
      _XmTextSetHighlight((Widget)data->widget, prim_right, data->sel2Right,
			 XmHIGHLIGHT_NORMAL);
    } else  {
      /* secondary selection encompasses the primary selection */
      if (data->sel2Left <= prim_left && data->sel2Right >= prim_right) {
	_XmTextSetHighlight((Widget)data->widget, data->sel2Left, prim_left,
			   XmHIGHLIGHT_NORMAL);
	_XmTextSetHighlight((Widget)data->widget, prim_left, prim_right,
			   XmHIGHLIGHT_SELECTED);
	_XmTextSetHighlight((Widget)data->widget, prim_right, data->sel2Right,
			   XmHIGHLIGHT_NORMAL);
	/* secondary selection is outside primary selection */
      } else {
	_XmTextSetHighlight((Widget)data->widget, prim_left, prim_right,
			   XmHIGHLIGHT_SELECTED);
	_XmTextSetHighlight((Widget)data->widget, data->sel2Left,
			   data->sel2Right, XmHIGHLIGHT_NORMAL);
      }
    }
  }
}

Boolean
_XmTextSetSel2(XmTextWidget tw,
	       XmTextPosition left,
	       XmTextPosition right, /* if right == -999, then we're in */
	       Time set_time)       /*   LoseSelection, so don't call  */
{
  InputData data = tw->text.input->data;
  Boolean result = TRUE;

  _XmTextDisableRedisplay(data->widget, FALSE);
  if (data->hasSel2) {
    XmTextPosition prim_left, prim_right;

    /* If the tw has the primary selection, make sure the selection
     * highlight is restored appropriately.
     */
    if ((*data->widget->text.source->GetSelection)(data->widget->text.source,
						   &prim_left, &prim_right))
      RestorePrimaryHighlight(data, prim_left, prim_right);
    else
      _XmTextSetHighlight((Widget) data->widget, data->sel2Left,
			 data->sel2Right, XmHIGHLIGHT_NORMAL);
  }

  if (!set_time) set_time = _XmValidTimestamp((Widget)tw);
  if (left <= right) {
    if (!data->hasSel2) {
      result = XmeSecondarySource((Widget) data->widget, set_time);
      data->sec_time = set_time;
      data->hasSel2 = result;
    } else
      result = TRUE;
    if (result) {
      _XmTextSetHighlight((Widget) data->widget, left, right,
			 XmHIGHLIGHT_SECONDARY_SELECTED);
      data->sel2Left = left;
      data->sel2Right = right;
    }
  } else {
    data->hasSel2 = FALSE;
    if (right != -999)
      XtDisownSelection((Widget) data->widget, XA_SECONDARY, set_time);
  }
  _XmTextEnableRedisplay(data->widget);
  return result;
}

Boolean
_XmTextGetSel2(XmTextWidget tw,
	       XmTextPosition *left,
	       XmTextPosition *right)
{
  InputData data = tw->text.input->data;

  if (data->hasSel2 && data->sel2Left <= data->sel2Right) {
    *left = data->sel2Left;
    *right = data->sel2Right;
    return TRUE;
  } else {
    data->hasSel2 = FALSE;
    return FALSE;
  }
}

static void
SetSelectionHint(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  if (event) {
    data->selectionHint.x = event->xbutton.x;
    data->selectionHint.y = event->xbutton.y;
  }
}

/*
 * This routine implements multi-click selection in a hardwired manner.
 * It supports multi-click entity cycling (char, word, line, file) and mouse
 * motion adjustment of the selected entities (i.e. select a word then, with
 * button still down, adjust which word you really meant by moving the mouse).
 * [Note: This routine is to be replaced by a set of procedures that
 * will allows clients to implements a wide class of draw through and
 * multi-click selection user interfaces.]
 */
static void
a_Selection(XmTextWidget tw,
	    Position x,
	    Position y,
	    Time sel_time,
	    int set_empty_selection)
{
  InputData data = tw->text.input->data;
  XmTextPosition position, newLeft, newRight;

  _XmTextDisableRedisplay(tw, FALSE);
  position = (*tw->text.output->XYToPos)(tw, x, y);

  if (data->stype == XmSELECT_OUT_LINE) {
    newLeft = SelectOutLine(tw, position, XmsdLeft, 1);
    newRight = SelectOutLine(tw, position, XmsdRight, 1);
  } else {
    newLeft = (*tw->text.source->Scan)(tw->text.source, position,
				       data->stype, XmsdLeft, 1, FALSE);
    newRight = (*tw->text.source->Scan)(tw->text.source, position,
					data->stype, XmsdRight, 1,
					data->stype == XmSELECT_LINE);
  }
  if (data->stype == XmSELECT_WORD && (int)tw->text.char_size > 1) {
    if (position == (*tw->text.source->Scan)
	(tw->text.source, newLeft, data->stype, XmsdRight, 1, FALSE))
      newLeft = position;
  }
  if (set_empty_selection || newLeft != newRight)
    (*tw->text.source->SetSelection)(tw->text.source, newLeft,
				     newRight, sel_time);
  tw->text.pendingoff = FALSE;
  if (position - newLeft < newRight - position) {
    _XmTextSetCursorPosition((Widget) tw, newLeft);
    data->extendDir = XmsdLeft;
  } else {
    _XmTextSetCursorPosition((Widget) tw, newRight);
    data->extendDir = XmsdRight;
  }
  _XmTextSetDestinationSelection((Widget)tw, tw->text.cursor_position,
				 False, sel_time);
  _XmTextShowPosition((Widget) tw, (XmTextPosition) -1);
  _XmTextEnableRedisplay(tw);
  data->origLeft = newLeft;
  data->origRight = newRight;
}

static void
SetAnchor(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  data->anchor = tw->text.cursor_position;
  _XmTextSetDestinationSelection(w, data->anchor, False, event_time);
  if ((*tw->text.source->GetSelection)
      (tw->text.source, &left, &right)) {
    (*tw->text.source->SetSelection)(tw->text.source, data->anchor,
				     data->anchor, event_time);
  }
}

static void
DoSelection(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  if (event)
    a_Selection(tw, event->xbutton.x, event->xbutton.y,
		event->xbutton.time, True);
}

static void
SetScanType(Widget w,
	    InputData data,
	    XEvent *event)
{
  int i;
  int multi_click_time;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  multi_click_time = XtGetMultiClickTime(XtDisplay(w));

  if (event_time > data->lasttime &&
      event_time - data->lasttime < multi_click_time) {

    i = 0;
    while (i < data->sarraycount && data->sarray[i] != data->stype) i++;

    if (++i >= data->sarraycount) i = 0;
    data->stype = data->sarray[i];
  } else {			/* single-click event */
    data->stype = data->sarray[0];
  }
  data->lasttime = event_time;
}

static void
StartPrimary(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition left, right;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  if (event)
    data->anchor = (*tw->text.output->XYToPos)(tw, event->xbutton.x,
					       event->xbutton.y);
  else
    data->anchor = tw->text.cursor_position;
  SetSelectionHint(w, event, params, num_params);
  SetScanType(w, data, event);
  if (data->stype != XmSELECT_POSITION ||
      ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
       left != right))
    DoSelection(w, event, params, num_params);
  else
    _XmTextSetDestinationSelection(w, data->anchor,
				   False, event_time);
  DisplayInsertionPoint(tw);

}


static void
StartSecondary(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  int status;
  Position x, y;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));
  if (!event_time) event_time = _XmValidTimestamp(w);

  data->sel_start = True;
  XAllowEvents(XtDisplay(w), AsyncBoth, event_time);

  if (event) {
    data->Sel2Hint.x = event->xbutton.x;
    data->Sel2Hint.y = event->xbutton.y;
  } else {
    (*tw->text.output->PosToXY)(tw, tw->text.cursor_position,
				&x, &y);
    data->Sel2Hint.x = x;
    data->Sel2Hint.y = y;
  }
  data->selectionMove = FALSE;
  data->selectionLink = FALSE;
  data->cancel = False;

  status = XtGrabKeyboard(w, False, GrabModeAsync,
			  GrabModeAsync, CurrentTime);

  if (status != GrabSuccess) XmeWarning(w, GRABKBDERROR);
}

static void
StartDrag(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  Widget drag_icon;
  Arg args[10];
  int n = 0;

  drag_icon = XmeGetTextualDragIcon(w);

  n = 0;
  XtSetArg(args[n], XmNcursorBackground, tw->core.background_pixel);  n++;
  XtSetArg(args[n], XmNcursorForeground, tw->primitive.foreground);  n++;
  XtSetArg(args[n], XmNsourceCursorIcon, drag_icon);  n++;
  if (_XmStringSourceGetEditable(GetSrc(w))) {
    XtSetArg(args[n], XmNdragOperations, (XmDROP_MOVE | XmDROP_COPY)); n++;
  } else {
    XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++;
  }
  (void) XmeDragSource(w, NULL, event, args, n);
}


static	void
DragStart(XtPointer data,
	  XtIntervalId *id)	/* unused */
{
  XmTextWidget tw = (XmTextWidget)data;
  InputData indata = tw->text.input->data;

  indata->drag_id = 0;
  StartDrag((Widget)tw, indata->transfer_action->event,
	    indata->transfer_action->params,
	    indata->transfer_action->num_params);
}


static void
ProcessBDrag(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  if (data->extending)
    return;

  EraseInsertionPoint(tw);
  if (InSelection(w, event)) {
    data->sel_start = False;
    StartDrag(w, event, params, num_params);
  } else {
    StartSecondary(w, event, params, num_params);
  }
  DisplayInsertionPoint(tw);
}

static void
ProcessBDragEvent(Widget w,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XtEnum drag_on_btn1 = XmOFF;
  XmDisplay dpy;

  dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  drag_on_btn1 = dpy->display.enable_btn1_transfer;

  if (drag_on_btn1 == XmBUTTON2_ADJUST && *num_params > 0)
    XtCallActionProc(w, params[0], event, NULL, 0);
  else if (*num_params > 1)
    XtCallActionProc(w, params[1], event, NULL, 0);
}

static Boolean
InSelection(Widget w,
	    XEvent *event)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition position, left, right;
  Position left_x, left_y, right_x, right_y;
  Position x, y;

  if (event) {
    position = (*tw->text.output->XYToPos)(tw, event->xbutton.x,
					   event->xbutton.y);
    x = event->xbutton.x;
  } else {
    position = tw->text.cursor_position;
    (*tw->text.output->PosToXY)(tw, position, &x, &y);
  }

  return ((*tw->text.source->GetSelection)(tw->text.source, &left, &right) &&
	  (right != left) &&
	  ( (position > left && position < right) ||
	    ( position == left &&
	      (*tw->text.output->PosToXY)(tw, left, &left_x, &left_y) &&
	      x > left_x) ||
	    ( position == right &&
	      (*tw->text.output->PosToXY)(tw, right, &right_x, &right_y) &&
	      x < right_x)));
}

static void
ProcessBSelect(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  Time event_time = event->xbutton.time;
  XtEnum drag_on_btn1 = XmOFF;
  XmDisplay dpy;

  dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  drag_on_btn1 = dpy->display.enable_btn1_transfer;

  if (!drag_on_btn1) {
    if (*num_params > 0)
      XtCallActionProc(w, params[0], event, NULL, 0);
    return;
  }

  if (*num_params == 0) {
    if (event->type == ButtonPress &&
	InSelection(w, event))
      StartDrag(w, event, params, num_params);
  } else {
    switch (event->type) {
    case ButtonPress:
      if (!InSelection(w, event) ||
	  (event_time > data->lasttime &&
	   event_time - data->lasttime < XtGetMultiClickTime(XtDisplay(w)))) {
	if (*num_params > 0)
	  XtCallActionProc(w, params[0], event, NULL, 0);
      } else {
	if (data->drag_id)
	  XtRemoveTimeOut(data->drag_id);
	if (data->transfer_action == NULL) {
	  data->transfer_action =
	    (_XmTextActionRec *) XtMalloc(sizeof(_XmTextActionRec));
	  data->transfer_action->event = (XEvent *)XtMalloc(sizeof(XEvent));
	}
	memcpy((void *)data->transfer_action->event, (void *)event,
	       sizeof(XEvent));
	data->transfer_action->params = params;
	data->transfer_action->num_params = num_params;
	data->drag_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					XtGetMultiClickTime(XtDisplay(w)),
					DragStart, (XtPointer)w);
	SetSelectionHint(w, event, params, num_params);
      }
      break;
    case ButtonRelease:
      if (data->drag_id) {
	XtRemoveTimeOut(data->drag_id);
	data->drag_id = 0;
	data->selectionHint.x = data->selectionHint.y = 0;
	if (*data->transfer_action->num_params) {
	  XtCallActionProc(w, data->transfer_action->params[0],
			   data->transfer_action->event, NULL, 0);
	}
      }
      XtCallActionProc(w, params[0], event, NULL, 0);
      break;
    case MotionNotify:
      if (data->drag_id) {
	if (dragged(data->selectionHint, event, data->threshold)) {
	  data->selectionHint.x = data->selectionHint.y = 0;
	  XtRemoveTimeOut(data->drag_id);
	  data->drag_id = 0;
	  StartDrag(w, event, params, num_params);
	}
      } else if (*num_params > 0)
	XtCallActionProc(w, params[0], event, NULL, 0);
      break;
    }
  }
}


static void
ProcessBSelectEvent(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XtEnum drag_on_btn1 = XmOFF;
  XmDisplay dpy;

  dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  drag_on_btn1 = dpy->display.enable_btn1_transfer;

  if (drag_on_btn1 == XmBUTTON2_TRANSFER && *num_params > 0)
    XtCallActionProc(w, params[0], event, NULL, 0);
  else if (*num_params > 1)
    XtCallActionProc(w, params[1], event, NULL, 0);
}
/*
 * This routine implements extension of the currently selected text in
 * the "current" mode (i.e. char word, line, etc.). It worries about
 * extending from either end of the selection and handles the case when you
 * cross through the "center" of the current selection (e.g. switch which
 * end you are extending!).
 * [NOTE: This routine will be replaced by a set of procedures that
 * will allows clients to implements a wide class of draw through and
 * multi-click selection user interfaces.]
*/
static Boolean
dragged(SelectionHint selectionHint,
        XEvent *event,
        int threshold)
{
  int xdiff, ydiff;
  xdiff = event ? abs(selectionHint.x - event->xbutton.x) : 0;
  ydiff = event ? abs(selectionHint.y - event->xbutton.y) : 0;
  if((xdiff > threshold) || (ydiff > threshold))
    return TRUE;
  else
    return FALSE;
}

static void
DoExtendedSelection(Widget w,
		    Time ev_time)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition position, left, right, cursorPos;
  float bal_point;

  if (data->cancel) {
    if (data->select_id) XtRemoveTimeOut(data->select_id);
    data->select_id = 0;
    return;
  }

  _XmTextDisableRedisplay(tw, FALSE);
  if (!((*tw->text.source->GetSelection)
	(tw->text.source, &left, &right)) || left == right) {
    data->anchor = tw->text.dest_position;
    left = right = tw->text.cursor_position;
    data->origLeft = data->origRight = data->anchor;
    bal_point = data->anchor;
  } else
    bal_point = (float)(((float)(data->origRight - data->origLeft) / 2.0) +
			(float)data->origLeft);

  position = (*tw->text.output->XYToPos)(tw, data->select_pos_x,
					 data->select_pos_y);

  /* shift anchor and direction to opposite end of the selection */

  if ((float)position <= bal_point) {
    data->anchor = data->origRight;
    if (!data->extending)
      data->extendDir = XmsdLeft;
  } else if ((float)position > bal_point) {
    data->anchor = data->origLeft;
    if (!data->extending)
      data->extendDir = XmsdRight;
  }

  data->extending = TRUE;

  /* check for change in extend direction */
  if ((data->extendDir == XmsdRight && position < data->anchor) ||
      (data->extendDir == XmsdLeft && position > data->anchor)) {
    data->extendDir =
      (data->extendDir == XmsdRight) ? XmsdLeft : XmsdRight;

    left = data->origLeft;
    right = data->origRight;
  }


  if (data->extendDir == XmsdRight) {
    if (data->stype == XmSELECT_OUT_LINE) {
      right = cursorPos = SelectOutLine(tw, position, XmsdRight, 1);
    } else {
      right = cursorPos =
	(*tw->text.source->Scan)(tw->text.source, position,
				 data->stype, XmsdRight, 1,
				 data->stype == XmSELECT_LINE);
    }
    left = data->anchor;
  } else {
    if (data->stype == XmSELECT_OUT_LINE) {
      left = cursorPos = SelectOutLine(tw, position, XmsdLeft, 1);
    } else {
      left = cursorPos = (*tw->text.source->Scan)(tw->text.source,
						  position, data->stype,
						  XmsdLeft, 1, FALSE);
    }
    if (data->stype == XmSELECT_WORD &&
	(int)tw->text.char_size > 1) {
      if (position == (*tw->text.source->Scan) (tw->text.source,
						left, data->stype,
						XmsdRight, 1, FALSE))
	left = cursorPos = position;
    }
    right = data->anchor;
  }

  (*tw->text.source->SetSelection)(tw->text.source, left, right, ev_time);
  tw->text.pendingoff = FALSE;
  _XmTextSetCursorPosition(w, cursorPos);
  _XmTextSetDestinationSelection(w, tw->text.cursor_position, False, ev_time);
  _XmTextEnableRedisplay(tw);
}

static void
DoSecondaryExtend(Widget w,
		  Time ev_time)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition position, left, right;

  position = (*tw->text.output->XYToPos)(tw, data->select_pos_x,
					 data->select_pos_y);

  _XmTextDisableRedisplay(tw, FALSE);
  _XmTextGetSel2(tw, &left, &right);
  /* check for change in extend direction */
  if ((data->Sel2ExtendDir == XmsdRight && position < data->Sel2OrigLeft) ||
      (data->Sel2ExtendDir == XmsdLeft &&
       position > data->Sel2OrigRight)) {
    data->Sel2ExtendDir =
      (data->Sel2ExtendDir == XmsdRight) ? XmsdLeft : XmsdRight;
    left = data->Sel2OrigLeft;
    right = data->Sel2OrigRight;
  }

  if (data->Sel2ExtendDir == XmsdRight)
    right = (*tw->text.source->Scan)(tw->text.source, position,
				     XmSELECT_POSITION, XmsdRight,1, FALSE);
  else
    left = (*tw->text.source->Scan)(tw->text.source, position,
				    XmSELECT_POSITION, XmsdLeft,  1, FALSE);
  (void) _XmTextSetSel2(tw, left, right, ev_time);
  _XmTextShowPosition(w, position);
  _XmTextEnableRedisplay(tw);
}

/************************************************************************
 *                                                                      *
 * BrowseScroll - timer proc that scrolls the list if the user has left *
 *              the window with the button down. If the button has been *
 *              released, call the standard click stuff.                *
 *                                                                      *
 ************************************************************************/
static void
BrowseScroll(XtPointer closure,
	     XtIntervalId *id)
{
  Widget w = (Widget) closure;
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  int interval = 100;

  if (data->cancel) {
    data->select_id = 0;
    return;
  }

  if (!data->select_id) return;

  if (data->Sel2Extending)
    DoSecondaryExtend(w, XtLastTimestampProcessed(XtDisplay(w)));
  else if (data->extending)
    DoExtendedSelection(w, XtLastTimestampProcessed(XtDisplay(w)));

  /* ask the vertical scroller its delay */
  if (tw->text.output->data->vbar)
    XtVaGetValues((Widget)tw->text.output->data->vbar,
		  XmNrepeatDelay, &interval, NULL);

  XSync (XtDisplay(w), False);

  data->select_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
				    (unsigned long)interval,
				    BrowseScroll, (XtPointer) w);
}


static Boolean
CheckTimerScrolling(Widget w,
		    XEvent *event)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;
  int interval = 200;

  if (!event)
    return False;

  data->select_pos_x = event->xmotion.x;
  data->select_pos_y = event->xmotion.y;

  if ((event->xmotion.x > (int)o_data->leftmargin) &&
      (event->xmotion.x < (int)(tw->core.width - o_data->rightmargin))  &&
      (event->xmotion.y > (int)o_data->topmargin) &&
      (event->xmotion.y < (int)(o_data->topmargin + (o_data->lineheight *
						     o_data->number_lines)))) {

    if (data->select_id) {
      XtRemoveTimeOut(data->select_id);
      data->select_id = 0;
    }
  } else {
    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
      /* to the above of the text */
      if (event->xmotion.y <= (int) o_data->topmargin)
	data->select_pos_y = (Position) (o_data->topmargin -
					 (o_data->font_ascent +
					  o_data->font_descent + 1));
      /* to the below of the text */
      else if (event->xmotion.y >= (int) (tw->core.height -
					  o_data->bottommargin))
	data->select_pos_y = (Position) ((tw->core.height -
					  o_data->bottommargin) +
					 (o_data->font_ascent +
					  o_data->font_descent + 1));
      /* right the text */
      if (event->xmotion.x >= (int) (tw->core.width - o_data->rightmargin)) {
	data->select_pos_x = (int) ((tw->core.width - o_data->rightmargin) +
				     o_data->linewidth);
	if (tw->text.top_line == 0)
	  data->select_pos_x = tw->core.width;

      /* left the text */
      } else if (event->xmotion.x <=
		 (int) ((tw->core.width - o_data->rightmargin) -
			(o_data->linewidth * (o_data->number_lines + 1))))
	data->select_pos_y = (tw->core.width - o_data->rightmargin) -
			      (o_data->linewidth * (o_data->number_lines + 1));

      /* ask the vertical scroller its delay */
      if (o_data->hbar)
	XtVaGetValues(o_data->hbar,
		      XmNinitialDelay, &interval, NULL);
    } else {
      /* to the left of the text */
      if (event->xmotion.x <= (int) o_data->leftmargin)
	data->select_pos_x = (Position) (o_data->leftmargin -
					 (o_data->averagecharwidth + 1));
      /* to the right of the text */
      else if (event->xmotion.x >= (int) (tw->core.width -
					  o_data->rightmargin))
	data->select_pos_x = (Position) ((tw->core.width -
					  o_data->rightmargin) +
					 o_data->averagecharwidth + 1);
      /* above the text */
      if (event->xmotion.y <= (int) o_data->topmargin) {
	data->select_pos_y = (int) (o_data->topmargin - o_data->lineheight);
	if (tw->text.top_line == 0)
	  data->select_pos_x = 0;

      /* below the text */
      } else if (event->xmotion.y >=
		 (int) (o_data->topmargin +
			(o_data->lineheight * o_data->number_lines)))
	data->select_pos_y = o_data->topmargin + (o_data->lineheight *
						  (o_data->number_lines + 1));

      /* ask the vertical scroller its delay */
      if (o_data->vbar)
	XtVaGetValues(o_data->vbar,
		      XmNinitialDelay, &interval, NULL);
    }

    if (!data->select_id)
      data->select_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					interval, BrowseScroll, (XtPointer) w);
    return True;
  }
  return False;
}

static void
StartExtendSelection(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  data->cancel = False;
  data->stuffpos = tw->text.cursor_position;
  ExtendSelection(w, event, params, num_params);
}

static void
ExtendSelection(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  if (data->cancel) return;
  _XmTextResetIC(w);

  EraseInsertionPoint(tw);
  if (!o_data->hasfocus && _XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  if (data->selectionHint.x || data->selectionHint.y) {
    if(!dragged(data->selectionHint, event, data->threshold)) {
      DisplayInsertionPoint(tw);
      return;
    }
    a_Selection(tw, data->selectionHint.x, data->selectionHint.y,
		event_time, False);
    data->selectionHint.x = data->selectionHint.y = 0;
    data->extending = True;
  }

  if (!CheckTimerScrolling(w, event))
    DoExtendedSelection(w, event_time);

  DisplayInsertionPoint(tw);
}

static void
ExtendSecondary(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition position, hintposition;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  if (data->cancel) return;
  _XmTextResetIC(w);

  EraseInsertionPoint(tw);
  if (event)
    position = (*tw->text.output->XYToPos)(tw, event->xbutton.x,
					   event->xbutton.y);
  else
    position = tw->text.cursor_position;

  if(data->Sel2Hint.x || data->Sel2Hint.y) {
    if(!dragged(data->Sel2Hint, event, data->threshold)) {
      DisplayInsertionPoint(tw);
      return;
    }
    hintposition = (*tw->text.output->XYToPos)(tw, data->Sel2Hint.x,
					       data->Sel2Hint.y);
    if(position < hintposition) {
      data->Sel2Extending = _XmTextSetSel2(tw, position, hintposition,
					   event_time);
      data->Sel2OrigLeft = hintposition; /**/
      data->Sel2OrigRight = hintposition;
      data->Sel2ExtendDir = XmsdLeft;
    } else {
      data->Sel2Extending = _XmTextSetSel2(tw, hintposition, position,
					   event_time);
      data->Sel2OrigLeft = hintposition;
      data->Sel2OrigRight = hintposition; /**/
      data->Sel2ExtendDir = XmsdRight;
    }
    data->Sel2Hint.x = data->Sel2Hint.y = 0;
  }

  if(!data->Sel2Extending) {
    DisplayInsertionPoint(tw);
    return;
  }

  if (!CheckTimerScrolling(w, event))
    DoSecondaryExtend(w, event_time);

  DisplayInsertionPoint(tw);
}

static void
ExtendEnd(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  if(data->extending || dragged(data->selectionHint, event, data->threshold)) {
    ExtendSelection(w, event, params, num_params);
    (*tw->text.source->GetSelection)(tw->text.source,
				     &(data->origLeft), &(data->origRight));
  }

  if (data->select_id) {
    XtRemoveTimeOut(data->select_id);
    data->select_id = 0;
  }

  data->select_pos_x = 0;
  data->select_pos_y = 0;
  data->extending = FALSE;
  data->selectionHint.x = data->selectionHint.y = 0;

  if (!data->sel_start) data->cancel = True;
}

static void
DoGrabFocus(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;

  _XmTextResetIC(w);
  data->cancel = False;
  if (event) {
    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
    /* to the top of the text */
      if (event->xbutton.y <= (int) o_data->topmargin)
        event->xbutton.y = (Position) (o_data->topmargin + 1);

      /* to the bottom of the text */
      else if (event->xbutton.y >=
	       (int) (tw->core.height - o_data->bottommargin))
        event->xbutton.y =
	    (Position)((tw->core.height - o_data->bottommargin)- 1);
      /* above the text */
      if (event->xbutton.x >= (int) (tw->core.width - o_data->rightmargin))
        event->xbutton.x = (int) (tw->core.width - o_data->rightmargin) - 1;

      /* below the text */
      else if (event->xbutton.x <=
	       (int)(tw->core.width - o_data->rightmargin -
	       (o_data->linewidth * o_data->number_lines)))
        event->xbutton.x = ((int)(tw->core.width - o_data->rightmargin) -
			    (o_data->linewidth * o_data->number_lines)) + 1;
    } else {
      /* to the left of the text */
      if (event->xbutton.x <= (int) o_data->leftmargin)
        event->xbutton.x = (Position) (o_data->leftmargin + 1);

      /* to the right of the text */
      else if (event->xbutton.x >= (int) (tw->core.width - o_data->rightmargin))
        event->xbutton.x = (Position)((tw->core.width - o_data->rightmargin)- 1);
      /* above the text */
      if (event->xbutton.y <= (int) o_data->topmargin)
        event->xbutton.y = (int) (o_data->topmargin + 1);

      /* below the text */
      else if (event->xbutton.y >=
	       (int)(o_data->topmargin + (o_data->lineheight *
					  o_data->number_lines)))
        event->xbutton.y = (o_data->topmargin +
			    (o_data->lineheight * o_data->number_lines)) - 1;
    }
  }

  /* CR 9058: "No-op" calls to traversal may generate focus events. */
  if ((_XmGetFocusPolicy(w) == XmEXPLICIT) &&
      (XmGetFocusWidget(w) != w))
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  StartPrimary(w, event, params, num_params);
  if (data->stype == XmSELECT_POSITION)
    SetCursorPosition(w, event, params, num_params);
  else if (tw->text.add_mode) {
    EraseInsertionPoint(tw);
    tw->text.add_mode = False;
    DisplayInsertionPoint(tw);
  }
  data->stuffpos = tw->text.cursor_position;
}

static void
MoveDestination(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition new_pos, left, right;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  if (event)
    new_pos = (*tw->text.output->XYToPos)(tw, event->xbutton.x,
					  event->xbutton.y);
  else
    new_pos = tw->text.cursor_position;

  if ((*tw->text.source->GetSelection)(tw->text.source, &left, &right)
      && (right != left))
    _XmTextSetDestinationSelection(w, new_pos, False, event_time);

  tw->text.pendingoff = False;
  if (_XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  _XmTextSetCursorPosition(w, new_pos);
  if (tw->text.cursor_position < left || tw->text.cursor_position > right)
    tw->text.pendingoff = TRUE;
  DisplayInsertionPoint(tw);
}

/* This function make the request to do a primary paste */

static void
Stuff(Widget w,
      XEvent *event,
      String *params,
      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XPoint *point = NULL;
  InputData data = tw->text.input->data;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  _XmTextResetIC(w);
  /* Request targets from the selection owner so you can decide what to
   * request.  The decision process and request for the selection is
   * taken care of in HandleTargets().
   */

  if (event && event->type == ButtonRelease) {
      /* WARNING: do not free the following memory in this module.
       * It will be freed in FreeLocationData, triggered at the end of
       * the data transfer operation.
       */
      point = (XPoint *) XtMalloc(sizeof(XPoint));
      point->x = event->xbutton.x;
      point->y = event->xbutton.y;
  }

  if (!event_time) event_time = _XmValidTimestamp(w);

  if (data->selectionLink)
    XmePrimarySink(w, XmLINK, (XtPointer) point,
		   event_time);
  else if (data->selectionMove)
    XmePrimarySink(w, XmMOVE, (XtPointer) point,
		   event_time);
  else
    XmePrimarySink(w, XmCOPY, (XtPointer) point,
		   event_time);
}

void
_XmTextHandleSecondaryFinished(Widget w,
			       XEvent *event)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextWidget dest_tw;
  InputData data = tw->text.input->data;
  TextDestData dest_data;
  long adjustment = 0;
  XmTextBlockRec block, newblock;
  XmTextPosition left, right;
  XmTextPosition cursorPos;
  Boolean freeBlock;
  Time time = XtLastTimestampProcessed(XtDisplay(w));

  dest_data = GetTextDestData(w);
  dest_tw = dest_data->widget;

  if (dest_data->has_destination) {
    adjustment = data->sel2Right - data->sel2Left;

    if (dest_data->position <= data->sel2Left) {
      data->sel2Left -= dest_data->replace_length;
      data->sel2Right += adjustment - dest_data->replace_length;
    } else if (dest_data->position > data->sel2Left &&
	       dest_data->position < data->sel2Right) {
      data->sel2Left -= dest_data->replace_length;
      data->sel2Right += adjustment - dest_data->replace_length;
    }
  }

  left = data->sel2Left;
  right = data->sel2Right;

  (void) _XmTextSetSel2(tw, 1, 0, time);

  block.ptr = "";
  block.length = 0;
  block.format = XmFMT_8_BIT;
  if (dest_data->position <= data->sel2Left) left += adjustment;
  if (_XmTextModifyVerify(tw, NULL, &left, &right,
			  &cursorPos, &block, &newblock, &freeBlock)) {
    if ((*tw->text.source->Replace)(tw, NULL, &left, &right,
				    &newblock, False) != EditDone) {
      RingBell(w, NULL, (String *) NULL, (Cardinal) 0);
    } else {
      int count;
      count = _XmTextCountCharacters(newblock.ptr, newblock.length);

      if (dest_data->has_destination && dest_data->position > right) {
	if (cursorPos == left + count)
	  cursorPos = dest_data->position + count;
	if (!dest_data->quick_key)
	  _XmTextSetCursorPosition((Widget)dest_tw, cursorPos);
	_XmTextSetDestinationSelection((Widget)dest_tw,
				       dest_tw->text.cursor_position,
				       False, time);
      } else if (count > 0 && dest_data->has_destination) {
	if (!dest_data->quick_key)
	  _XmTextSetCursorPosition((Widget)dest_tw, cursorPos);
	_XmTextSetDestinationSelection((Widget)dest_tw,
				       dest_tw->text.cursor_position,
				       False, time);
      }
      if (!_XmStringSourceHasSelection(dest_tw->text.source)) {
	dest_tw->text.input->data->anchor = dest_data->position;
      }
      if (!dest_data->has_destination && tw->text.add_mode) {
	EraseInsertionPoint(tw);
	tw->text.add_mode = False;
	DisplayInsertionPoint(tw);
      }
      _XmTextValueChanged(tw, event);
    }
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
  } else {
    RingBell(w, NULL, (String *) NULL, (Cardinal) 0);
  }
}

/* Send a client message to perform the quick cut/copy and paste */
static void
SecondaryNotify(Widget w,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextWidget dest_tw;
  InputData data = tw->text.input->data;
  TextDestData dest_data;
  XmTextPosition left, right;
  Time event_time = event ? event->xbutton.time :
                            XtLastTimestampProcessed(XtDisplay(w));

  _XmTextResetIC(w);
  if (data->selectionMove == TRUE && data->has_destination &&
      tw->text.dest_position >= data->sel2Left &&
      tw->text.dest_position <= data->sel2Right) {
    (void)_XmTextSetSel2(tw, 1, 0, event_time);
    return;
  }

  dest_data = GetTextDestData(w);

  dest_data->replace_length = 0;

  dest_tw = tw;

  if (!dest_tw->text.input->data->has_destination &&
      dest_tw->text.source->data->numwidgets > 1) {
    int i;

    for (i=0; i<tw->text.source->data->numwidgets; i++) {
      dest_tw = (XmTextWidget) tw->text.source->data->widgets[i];
      if (dest_tw->text.input->data->has_destination) break;
    }
    if (i == tw->text.source->data->numwidgets) dest_tw = tw;
  }

  dest_data->has_destination = dest_tw->text.input->data->has_destination;
  dest_data->position = dest_tw->text.dest_position;
  dest_data->widget = dest_tw;

  if (*(num_params) == 1) dest_data->quick_key = True;
  else dest_data->quick_key = False;

  if ((*dest_tw->text.source->GetSelection)
      (dest_tw->text.source, &left, &right) && left != right) {
    if (dest_data->position >= left && dest_data->position <= right)
      dest_data->replace_length = right - left;
  }

  /* special fix for handling case of shared source with secondary select */
  _XmTextSetHighlight((Widget) data->widget, data->sel2Left,
		     data->sel2Right, XmHIGHLIGHT_NORMAL);

  /*
   * Make a request for the primary selection to convert to
   * type INSERT_SELECTION as per ICCCM.
   */
  if (!event_time) event_time = _XmValidTimestamp(w);

  if (data->selectionLink)
    XmeSecondaryTransfer(w, XmeGetEncodingAtom(w),
			 XmLINK, event_time);
  else if (data->selectionMove)
    XmeSecondaryTransfer(w, XmeGetEncodingAtom(w),
			 XmMOVE, event_time);
  else
    XmeSecondaryTransfer(w, XmeGetEncodingAtom(w),
			 XmCOPY, event_time);
}

static void
VoidAction(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  /* Do Nothing */
}

/*
 * This function set the final position of the secondary selection and
 * calls SecondaryNotify().
 */
static void
ExtendSecondaryEnd(Widget w,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (!data->cancel) XtUngrabKeyboard(w, CurrentTime);

  /* if the pointer is inside the text area, do the secondary transfer */
  if (event)
    if (event->xbutton.x > (int) tw->core.width || event->xbutton.x < 0 ||
	event->xbutton.y > (int) tw->core.height || event->xbutton.y < 0) {
      if (data->hasSel2 && data->Sel2Extending) {
	data->cancel = True;
	_XmTextSetSel2(tw, 1, 0, event_time);
      }
    }

  if ((data->Sel2Extending || dragged(data->Sel2Hint, event, data->threshold))
      && !data->cancel) {
    _XmTextGetSel2(tw, &(data->Sel2OrigLeft), &(data->Sel2OrigRight));
    SecondaryNotify(w, event, params, num_params);
  }

  /* Re-initialize the secondary selection data */
  data->select_pos_x = 0;
  data->select_pos_y = 0;
  data->Sel2Extending = FALSE;
  data->Sel2Hint.x = data->Sel2Hint.y = 0;
  data->sel_start = False;

  if (data->select_id) {
    XtRemoveTimeOut(data->select_id);
    data->select_id = 0;
  }

  data->cancel = True;
}


/*
 * This Action Proc selects all of the text.
 */
static void
SelectAll(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition last_position =
    (*tw->text.source->Scan)(tw->text.source, 0, XmSELECT_ALL,
			     XmsdRight, 1, TRUE);
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  data->stype = XmSELECT_ALL;
  (*tw->text.source->SetSelection)(tw->text.source, 0,
				   last_position, event_time);
  _XmTextMovingCursorPosition(tw, tw->text.cursor_position);
  data->anchor = 0;
  DisplayInsertionPoint(tw);
}


/*
 * This Action Proc deselects all of the text.
 */
static void
DeselectAll(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XmTextPosition cursorPos = tw->text.cursor_position;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  _XmTextSetDestinationSelection(w, cursorPos, False, event_time);
  data->stype = XmSELECT_POSITION;
  (*tw->text.source->SetSelection)(tw->text.source, cursorPos,
				   cursorPos, event_time);
  _XmTextMovingCursorPosition(tw, tw->text.cursor_position);
  data->anchor = cursorPos;
  DisplayInsertionPoint(tw);
}


/*
 * This Action Proc replaces the primary selection with spaces
 */
static void
ClearSelection(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextPosition left, right;
  Boolean freeBlock;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  EraseInsertionPoint(tw);
  if (!(*tw->text.source->GetSelection)
      (tw->text.source, &left, &right)) {
    XBell(XtDisplay(tw), 0);
  } else if (left != right) {
    char *select_string = _XmStringSourceGetString(tw, left, right,
						   False);
    XmTextBlockRec block, newblock;
    int num_spaces = right - left;
    int i;

    for(i = 0; i < num_spaces; i++) {
      if (select_string[i] != '\012') select_string[i] = ' ';
    }

    block.ptr = select_string;
    block.length = num_spaces;
    block.format = XmFMT_8_BIT;
    if (_XmTextModifyVerify(tw, event, &left, &right,
			    NULL, &block, &newblock, &freeBlock)) {
      if ((*tw->text.source->Replace)(tw, NULL, &left, &right,
				      &newblock, False) != EditDone) {
	RingBell(w, event, params, num_params);
      } else {
	/* should we set cursor position? */
	_XmTextSetDestinationSelection(w, tw->text.cursor_position,
				       False, event_time);
	_XmTextValueChanged(tw, event);
      }
      if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
    } else {
      RingBell(w, event, params, num_params);
    }
    XtFree(select_string);
  }
  DisplayInsertionPoint(tw);
}

static void
ProcessBDragRelease(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;
  XButtonEvent	*ev = (XButtonEvent *) event;

  if (data->extending)
    return; /* For Btn2up when Btn1 is down */

  /* Work around for intrinsic bug.  Remove once bug is fixed. */
  XtUngrabPointer(w, ev->time);

  EraseInsertionPoint(tw);
  if (data->sel_start) {
    if (dragged(data->Sel2Hint, event, data->threshold)) {
      if (data->Sel2Extending) {
	/*
	 * Secondary selection:
	 * Causes the converter to perform a delete action of the
	 * secondary selection when the Convert routine is called.
	 */
	ExtendSecondaryEnd(w, event, params, num_params);
      } else {
	/* Not a drag action, not secondary selection, not Quick transfer.
	 * At least we have to ungrab the keyboard...
	 */
	if (!data->cancel) XtUngrabKeyboard(w, CurrentTime);
      }
    } else {
      /*
       * Quick transfer: Copy contents of primary selection to the
       * stuff position found above.
       */
      Stuff(w, event, params, num_params);
      if (!data->cancel) XtUngrabKeyboard(w, CurrentTime);
    }
    data->sel_start = False;
  }
  DisplayInsertionPoint(tw);
}

/*
 * This function does a primary copy and paste on mouse button actions.
 * It copies the contents of the primary selection to the x and y
 * position of the button pressed event.
 */
static void
ProcessCopy(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  data->selectionMove = False;
  data->selectionLink = False;
  ProcessBDragRelease(w, event, params, num_params);
  DisplayInsertionPoint(tw);

  data->cancel = True;
}

/*
 * This function does a primary link on mouse button actions.
 * It links the contents of the primary selection to the x and y
 * position of the button pressed event.
 */
static void
ProcessLink(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  EraseInsertionPoint(tw);
  data->selectionMove = False;
  data->selectionLink = True;
  ProcessBDragRelease(w, event, params, num_params);
  DisplayInsertionPoint(tw);

  data->cancel = True;
}

/* This function does a primary cut and paste on mouse button actions. */
static void
ProcessMove(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  data->selectionMove = True;
  data->selectionLink = False;
  ProcessBDragRelease(w, event, params, num_params);
  DisplayInsertionPoint(tw);

  data->cancel = True;
}


/* This function does a primary copy and paste on keyboard actions. */
static void
CopyPrimary(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  data->selectionMove = False;
  data->selectionLink = FALSE;

  /* perform the primary paste action */
  Stuff(w, event, params, num_params);
  DisplayInsertionPoint(tw);
}

/* This function does a primary cut and paste on keyboard actions. */
static void
CutPrimary(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  _XmTextResetIC(w);
  EraseInsertionPoint(tw);
  data->selectionMove = True;
  data->selectionLink = FALSE;

  /* perform the primary paste action */
  Stuff(w, event, params, num_params);
  DisplayInsertionPoint(tw);
}

/* This function does a primary link on keyboard actions. */
static void
LinkPrimary(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  EraseInsertionPoint(tw);
  data->selectionMove = False;
  data->selectionLink = True;

  /* perform the primary paste action */
  Stuff(w, event, params, num_params);
  DisplayInsertionPoint(tw);
}

static void
CutClipboard(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextSource source = GetSrc(tw);
  XmTextPosition left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (!event_time) event_time = _XmValidTimestamp(w);

  EraseInsertionPoint(tw);
  if (_XmStringSourceGetEditable(GetSrc(w)) &&
      (*source->GetSelection)(source, &left, &right) && right != left)
    XmeClipboardSource(w, XmMOVE, event_time);
  DisplayInsertionPoint(tw);
}


static void
CopyClipboard(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  XmTextSource source = GetSrc(w);
  XmTextPosition left, right;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  if (!event_time) event_time = _XmValidTimestamp(w);

  EraseInsertionPoint(tw);
  if ((*source->GetSelection)(source, &left, &right) && right != left)
    XmeClipboardSource(w, XmCOPY, event_time);
  _XmTextSetDestinationSelection(w, tw->text.cursor_position,
				 False, event_time);
  DisplayInsertionPoint(tw);
}


static void
PasteClipboard(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  EraseInsertionPoint(tw);
  data->selectionMove = FALSE;
  data->selectionLink = FALSE;
  XmeClipboardSink(w, XmCOPY, NULL);
  DisplayInsertionPoint(tw);
}

static Boolean
VerifyLeave(Widget w,
	    XEvent *event)
{
  XmTextWidget tw = (XmTextWidget) w;

  XmTextVerifyCallbackStruct  cbdata;

  cbdata.reason = XmCR_LOSING_FOCUS;
  cbdata.event = event;
  cbdata.doit = True;
  cbdata.currInsert = tw->text.cursor_position;
  cbdata.newInsert = tw->text.cursor_position;
  cbdata.startPos = tw->text.cursor_position;
  cbdata.endPos = tw->text.cursor_position;
  cbdata.text = NULL;
  XtCallCallbackList(w, tw->text.losing_focus_callback, (XtPointer) &cbdata);
  tw->text.source->data->take_selection = True;
  return(cbdata.doit);
}

static void
TextLeave(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  if (_XmGetFocusPolicy(w) == XmPOINTER)
    VerifyLeave(w, event);

  _XmPrimitiveLeave(w, event, params, num_params);
}

static void
TextFocusIn(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (!event || !event->xfocus.send_event) return;

  if (_XmGetFocusPolicy(w) == XmEXPLICIT && !_XmTextHasDestination(w) &&
      !tw->text.input->data->sel_start)
    _XmTextSetDestinationSelection(w, tw->text.cursor_position, False,
				   XtLastTimestampProcessed(XtDisplay(w)));

  _XmPrimitiveFocusIn(w, event, params, num_params);
}

static void
TextFocusOut(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* If traversal is on, then the leave verification callback is called in
     the traversal event handler */
  if (event && event->xfocus.send_event &&
      _XmGetFocusPolicy(w) == XmEXPLICIT && !tw->text.traversed) {
    (void) VerifyLeave(w, event);
  } else
    if (tw->text.traversed) tw->text.traversed = False;

  _XmPrimitiveFocusOut(w, event, params, num_params);
}

static void
TraverseDown(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* Find out if there is anything else to traverse to */
  /* Allow the verification routine to control the traversal */
  if (tw->primitive.navigation_type == XmNONE && VerifyLeave(w, event)) {
    tw->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_DOWN))
      tw->text.traversed = False;
  }
}

static void
TraverseUp(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* Allow the verification routine to control the traversal */
  if (tw->primitive.navigation_type == XmNONE && VerifyLeave(w, event)) {
    tw->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_UP))
      tw->text.traversed = False;
  }
}

static void
TraverseHome(Widget w,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* Allow the verification routine to control the traversal */
  if (tw->primitive.navigation_type == XmNONE && VerifyLeave(w, event)) {
    tw->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_HOME))
      tw->text.traversed = False;
  }
}

static void
TraverseNextTabGroup(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* Allow the verification routine to control the traversal */
  if (VerifyLeave(w, event)) {
    XmTraversalDirection dir;
    XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    Boolean enable_button_tab = xm_dpy->display.enable_button_tab;

    dir = (enable_button_tab ?
	   XmTRAVERSE_GLOBALLY_FORWARD : XmTRAVERSE_NEXT_TAB_GROUP);

    tw->text.traversed = True;
    if (!_XmMgrTraversal(w, dir))
      tw->text.traversed = False;
  }
}

static void
TraversePrevTabGroup(Widget w,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* Allow the verification routine to control the traversal */
  if (VerifyLeave(w, event)) {
    XmTraversalDirection dir;
    XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    Boolean enable_button_tab = xm_dpy->display.enable_button_tab;

    dir = (enable_button_tab ?
	   XmTRAVERSE_GLOBALLY_BACKWARD : XmTRAVERSE_PREV_TAB_GROUP);

    tw->text.traversed = True;
    if (!_XmMgrTraversal(w, dir))
      tw->text.traversed = False;
  }
}

/***************************************************************************
 * Functions to process text tw in multi-line edit mode versus single      *
 * line edit mode.                                                         *
 ***************************************************************************/

static void
ProcessCancel(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;
  InputData data = tw->text.input->data;

  XmParentInputActionRec  p_event;
  Time event_time = event ? event->xkey.time :
                            XtLastTimestampProcessed(XtDisplay((Widget)tw));

  data->cancel = False;

  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_CANCEL;
  p_event.event = event;           /* Pointer to XEvent. */
  p_event.params = params;         /* Or use what you have if   */
  p_event.num_params = num_params; /* input is from translation.*/

  EraseInsertionPoint(tw);
  if (data->sel_start) {
    data->cancel = True;
    if (data->hasSel2 && data->Sel2Extending)
      _XmTextSetSel2(tw, 1, 0, event_time);
    XtUngrabKeyboard(w, CurrentTime);
  }

  if (_XmStringSourceHasSelection(tw->text.source) && data->extending) {
    data->cancel = True;
    /* restore cursor position */
    _XmTextSetCursorPosition((Widget)tw, data->stuffpos);
    _XmTextSetDestinationSelection((Widget)tw, tw->text.cursor_position,
				   False, event_time);
    /* reset origLeft and origRight */
    (*tw->text.source->SetSelection)(tw->text.source, data->origLeft,
				     data->origRight, event_time);
  }

  if (!data->cancel)
    (void) _XmParentProcess(XtParent(tw), (XmParentProcessData) &p_event);

  if (data->select_id) {
    XtRemoveTimeOut(data->select_id);
    data->select_id = 0;
  }
  DisplayInsertionPoint(tw);

  data->cancel = True;
}

static void
ProcessReturn(Widget w,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT)
    Activate(w, event, params, num_params);
  else {
    EraseInsertionPoint(tw);
    InsertNewLine(w, event, params, num_params);
    DisplayInsertionPoint(tw);
  }
}

static void
ProcessTab(Widget w,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT ||
      !_XmStringSourceGetEditable(GetSrc(w)))
    if (*num_params == 0 || *(params[0]) == 'N')
      TraverseNextTabGroup(w, event, params, num_params);
    else
      TraversePrevTabGroup(w, event, params, num_params);
  else
    if (*num_params == 0 || *(params[0]) == 'N')
      SelfInsert(w, event, params, num_params);
    /* do nothing for shift+Tab in multiline mode */
}

static void
ProcessUp(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmMULTI_LINE_EDIT) {
    EraseInsertionPoint(tw);
    MovePreviousLine(w, event, params, num_params);
    DisplayInsertionPoint(tw);
  } else if (w != XmGetTabGroup(w))
    TraverseUp(w, event, params, num_params);
}

static void
ProcessDown(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmMULTI_LINE_EDIT) {
    EraseInsertionPoint(tw);
    MoveNextLine(w, event, params, num_params);
    DisplayInsertionPoint(tw);
  } else if (w != XmGetTabGroup(w))
    TraverseDown(w, event, params, num_params);
}


static void
ProcessLeft(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.edit_mode == XmMULTI_LINE_EDIT) {
    EraseInsertionPoint(tw);
    MoveNextLine(w, event, params, num_params);
    DisplayInsertionPoint(tw);
  } else if (w != XmGetTabGroup(w))
    TraverseDown(w, event, params, num_params);
}

static void
ProcessRight(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.edit_mode == XmMULTI_LINE_EDIT) {
    EraseInsertionPoint(tw);
    MovePreviousLine(w, event, params, num_params);
    DisplayInsertionPoint(tw);
  } else if (w != XmGetTabGroup(w))
    TraverseUp(w, event, params, num_params);
}


static void
ProcessShiftUp(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) {
    TraverseUp(w, event, params, num_params);
  } else {
    char *dir = "extend";
    Cardinal num = 1;
    EraseInsertionPoint(tw);
    _MovePreviousLine(w, event, &dir, &num, False);
    DisplayInsertionPoint(tw);
  }
}

static void
ProcessShiftDown(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  _XmTextResetIC(w);
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) {
    TraverseDown(w, event, params, num_params);
  } else {
    char *dir = "extend";
    Cardinal num = 1;
    EraseInsertionPoint(tw);
    _MoveNextLine(w, event, &dir, &num, False);
    DisplayInsertionPoint(tw);
  }
}


static void
ProcessShiftLeft(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) {
    TraverseUp(w, event, params, num_params);
  } else {
    char *dir = "extend";
    Cardinal num = 1;
    EraseInsertionPoint(tw);
    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
      _MoveNextLine(w, event, &dir, &num, False);
    else
      _MovePreviousLine(w, event, &dir, &num, False);
    DisplayInsertionPoint(tw);
  }
}

static void
ProcessShiftRight(Widget w,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT) {
    TraverseDown(w, event, params, num_params);
  } else {
    char *dir = "extend";
    Cardinal num = 1;
    EraseInsertionPoint(tw);
    if(XmDirectionMatch(XmPrim_layout_direction(tw),
			XmTOP_TO_BOTTOM_RIGHT_TO_LEFT))
      _MovePreviousLine(w, event, &dir, &num, False);
    else
      _MoveNextLine(w, event, &dir, &num, False);
    DisplayInsertionPoint(tw);
  }
}


static void
ProcessHome(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmTextWidget tw = (XmTextWidget) w;

  EraseInsertionPoint(tw);
  MoveToLineStart(w, event, params, num_params);
  DisplayInsertionPoint(tw);
}


static XtActionsRec ZdefaultTextActionsTable[] = {
/* Insert bindings */
  {"self-insert",		SelfInsert},
  {"insert-string",		InsertString},
/* Motion bindings */
  {"grab-focus",		DoGrabFocus},
  {"set-insertion-point",	SetCursorPosition},
  {"forward-character", 	MoveForwardChar},
  {"backward-character", 	MoveBackwardChar},
  {"forward-word", 		MoveForwardWord},
  {"backward-word", 		MoveBackwardWord},
  {"forward-paragraph", 	MoveForwardParagraph},
  {"backward-paragraph", 	MoveBackwardParagraph},
  {"beginning-of-line", 	MoveToLineStart},
  {"end-of-line", 		MoveToLineEnd},
  {"next-line", 		MoveNextLine},
  {"previous-line", 		MovePreviousLine},
  {"next-page", 		MoveNextPage},
  {"previous-page", 		MovePreviousPage},
  {"page-left", 		MovePageLeft},
  {"page-right", 		MovePageRight},
  {"beginning-of-file", 	MoveBeginningOfFile},
  {"end-of-file", 		MoveEndOfFile},
  {"move-destination",		MoveDestination},
  {"scroll-one-line-up", 	ScrollOneLineUp},
  {"scroll-one-line-down", 	ScrollOneLineDown},
  {"scroll-cursor-vertically", 	ScrollCursorVertically},
/* Delete bindings */
  {"delete-selection", 		DeleteCurrentSelection},
  {"delete-next-character", 	DeleteForwardChar},
  {"delete-previous-character",	DeleteBackwardChar},
  {"delete-next-word", 		DeleteForwardWord},
  {"delete-previous-word", 	DeleteBackwardWord},
  {"delete-to-end-of-line", 	DeleteToEndOfLine},
  {"delete-to-start-of-line",	DeleteToStartOfLine},
/* Kill bindings */
  {"kill-selection", 		KillCurrentSelection},
  {"kill-next-character", 	KillForwardChar},
  {"kill-previous-character",	KillBackwardChar},
  {"kill-next-word", 		KillForwardWord},
  {"kill-previous-word", 	KillBackwardWord},
  {"kill-to-end-of-line", 	KillToEndOfLine},
  {"kill-to-start-of-line",	KillToStartOfLine},
/* Unkill bindings */
  {"unkill", 			UnKill},
  {"stuff", 			Stuff},
/* New line bindings */
  {"newline-and-indent", 	InsertNewLineAndIndent},
  {"newline-and-backup", 	InsertNewLineAndBackup},
  {"newline",			InsertNewLine},
/* Selection bindings */
  {"select-all", 		SelectAll},
  {"deselect-all", 		DeselectAll},
  {"select-start", 		StartPrimary},
  {"quick-cut-set", 		VoidAction},
  {"quick-copy-set", 		VoidAction},
  {"do-quick-action", 		VoidAction},
  {"key-select", 		KeySelection},
  {"set-anchor", 		SetAnchor},
  {"select-adjust", 		DoSelection},
  {"select-end", 		DoSelection},
  {"extend-start", 		StartExtendSelection},
  {"extend-adjust", 		ExtendSelection},
  {"extend-end", 		ExtendEnd},
  {"set-selection-hint",	SetSelectionHint},
  {"process-bdrag",		ProcessBDrag},
  {"process-bdrag-event",	ProcessBDragEvent},
  {"process-bselect",		ProcessBSelect},
  {"process-bselect-event",	ProcessBSelectEvent},
  {"secondary-start",		StartSecondary},
  {"secondary-drag",		StartDrag},
  {"secondary-adjust",		ExtendSecondary},
  {"secondary-notify",		ExtendSecondaryEnd},
  {"clear-selection",		ClearSelection},
  {"copy-to",			ProcessCopy},
  {"link-to",			ProcessLink},
  {"move-to",			ProcessMove},
  {"copy-primary",		CopyPrimary},
  {"cut-primary",		CutPrimary},
  {"link-primary",              LinkPrimary},
/* Clipboard bindings */
  {"copy-clipboard",		CopyClipboard},
  {"cut-clipboard",		CutClipboard},
  {"paste-clipboard",		PasteClipboard},
/* Miscellaneous bindings */
  {"beep", 			RingBell},
  {"redraw-display", 		RedrawDisplay},
  {"activate",			Activate},
  {"toggle-overstrike",		ToggleOverstrike},
  {"toggle-add-mode",		ToggleAddMode},
  {"Help",			_XmPrimitiveHelp},
  {"enter",                     _XmPrimitiveEnter},
  {"leave",			TextLeave},
  {"focusIn",			TextFocusIn},
  {"focusOut",			TextFocusOut},
  {"unmap",			_XmPrimitiveUnmap},
/* Process multi-line and single line bindings */
  {"process-cancel",		ProcessCancel},
  {"process-return",		ProcessReturn},
  {"process-tab",		ProcessTab},
  {"process-up",		ProcessUp},
  {"process-down",		ProcessDown},
  {"process-shift-up",		ProcessShiftUp},
  {"process-shift-down",	ProcessShiftDown},
  {"process-home",		ProcessHome},
/* Traversal bindings*/
  {"traverse-next",		TraverseDown},
  {"traverse-prev",		TraverseUp},
  {"traverse-home",		TraverseHome},
  {"next-tab-group",		TraverseNextTabGroup},
  {"prev-tab-group",		TraversePrevTabGroup},
  {"page-up",			MovePageUp},
  {"page-down",			MovePageDown},
  {"process-left",		ProcessLeft},
  {"process-right",		ProcessRight},
  {"process-shift-left",	ProcessShiftLeft},
  {"process-shift-right",	ProcessShiftRight},
};

externaldef(nonvisible) XtPointer _XmdefaultTextActionsTable =
					 (XtPointer) ZdefaultTextActionsTable;

externaldef(nonvisible) Cardinal _XmdefaultTextActionsTableSize =
                                XtNumber(ZdefaultTextActionsTable);

/* added <Key> event */
#define _XmTextEventBindings1	_XmTextIn_XmTextEventBindings1
#define _XmTextEventBindings2	_XmTextIn_XmTextEventBindings2
#define _XmTextEventBindings3	_XmTextIn_XmTextEventBindings3
#define _XmTextVEventBindings	_XmTextIn_XmTextVEventBindings

static void
Invalidate(XmTextWidget tw,
	   XmTextPosition position,
	   XmTextPosition topos,
	   long delta)
{
  InputData data = tw->text.input->data;
  if (delta == NODELTA) return; /* Just use what we have as best guess. */
  if (data->origLeft > position) data->origLeft += delta;
  if (data->origRight >= position) data->origRight += delta;
}

static void
InputGetValues(Widget wid,
	       ArgList args,
	       Cardinal num_args)
{
  XmTextWidget tw = (XmTextWidget) wid;
  XtGetSubvalues((XtPointer) tw->text.input->data,
		 input_resources, XtNumber(input_resources), args, num_args);
}

static void
InputSetValues(Widget oldw,
	       Widget reqw,
	       Widget new_w,
	       ArgList args,
	       Cardinal *num_args)
{
  XmTextWidget tw = (XmTextWidget) new_w;
  InputData data = tw->text.input->data;
  XtPointer temp_ptr;

  temp_ptr = (XtPointer)data->sarray;

  XtSetSubvalues((XtPointer) data,
		 input_resources, XtNumber(input_resources), args, *num_args);
  /*
   * Fix for HaL DTS 9841 - If the new selectionArray is different than the old
   *                        selectionArray, delete the old selectionArray and
   * 			  then copy the new selectionArray.
   */
  if ((XtPointer)data->sarray != temp_ptr)
    {
      XtFree((char *)temp_ptr);
      temp_ptr = (XtPointer)data->sarray;
      data->sarray = (XmTextScanType *)XtMalloc(data->sarraycount *
						sizeof(XmTextScanType));
      memcpy((void *)data->sarray, (void *)temp_ptr, (data->sarraycount *
						      sizeof(XmTextScanType)));
    }
  /*
   * End Fix for HaL DTS 9841
   */
}

static void
InputDestroy(Widget w)
{
  XmTextWidget tw = (XmTextWidget) w;
  Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(tw),
				       XmS_MOTIF_DESTINATION, False);
  Widget dest = XmGetDestination(XtDisplay(w));

  if (dest == w)
    _XmSetDestination(XtDisplay(w), NULL);

  if (tw->core.window == XGetSelectionOwner(XtDisplay(tw),
					    MOTIF_DESTINATION))
    XtDisownSelection(w, MOTIF_DESTINATION,
		      XtLastTimestampProcessed(XtDisplay(w)));

  if (tw->core.window == XGetSelectionOwner(XtDisplay(tw),
					    XA_PRIMARY))
    XtDisownSelection(w, XA_PRIMARY,
		      XtLastTimestampProcessed(XtDisplay(w)));

  if (tw->core.window == XGetSelectionOwner(XtDisplay(tw),
					    XA_SECONDARY))
    XtDisownSelection(w, XA_SECONDARY,
		      XtLastTimestampProcessed(XtDisplay(w)));

  if (tw->text.input->data->drag_id)
    XtRemoveTimeOut(tw->text.input->data->drag_id);

  if (tw->text.input->data->select_id)
    XtRemoveTimeOut(tw->text.input->data->select_id);

  if (tw->text.input->data->transfer_action) {
    XtFree((char *)tw->text.input->data->transfer_action->event);
    XtFree((char *)tw->text.input->data->transfer_action);
  }

  /*
   * Fix for HaL DTS 9841 - release the data for the selectionArray.
   */
  XtFree((char *)tw->text.input->data->sarray);
  XtFree((char *)tw->text.input->data);
  XtFree((char *)tw->text.input);
  /*
    XmDropSiteUnregister(w);
    */
  XmImUnregister(w);
}

static XtPointer
InputBaseProc(Widget widget,
	      XtPointer client_data)
{
  XtPointer ret_val;
  XmTextWidget tw = (XmTextWidget) widget;

  _XmProcessLock();
  ret_val = (XtPointer) tw->text.input;
  _XmProcessUnlock();
  return ret_val;
}


void
_XmTextInputGetSecResData(XmSecondaryResourceData *secResDataRtn)
{
  XmSecondaryResourceData               secResData;

  secResData = XtNew(XmSecondaryResourceDataRec);

  _XmTransformSubResources(input_resources, XtNumber(input_resources),
			   &(secResData->resources),
			   &(secResData->num_resources));

  secResData->name = NULL;
  secResData->res_class = NULL;
  secResData->client_data = NULL;
  secResData->base_proc = InputBaseProc;
  *secResDataRtn = secResData;
}

static void
DragProcCallback(Widget w,
		 XtPointer client,
		 XtPointer call)
{
  enum { XmACOMPOUND_TEXT, XmATEXT,
#ifdef XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSTEXT,
#ifdef XM_UTF8
      XmSUTF8_STRING
#endif
      };

  XmDragProcCallbackStruct *cb = (XmDragProcCallbackStruct *)call;
  Widget drag_cont;
  Atom targets[5];
  Arg args[10];
  Atom *exp_targets;
  Cardinal num_exp_targets, n;
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  targets[0] = XmeGetEncodingAtom(w);
  targets[1] = atoms[XmACOMPOUND_TEXT];
  targets[2] = XA_STRING;
  targets[3] = atoms[XmATEXT];
#ifdef XM_UTF8
  targets[4] = atoms[XmAUTF8_STRING];
#endif

  drag_cont = cb->dragContext;

  n = 0;
  XtSetArg(args[n], XmNexportTargets, &exp_targets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &num_exp_targets); n++;
  XtGetValues(drag_cont, args, n);

  switch(cb->reason) {
  case XmCR_DROP_SITE_ENTER_MESSAGE:
#ifdef XM_UTF8
    if (XmTargetsAreCompatible(XtDisplay(drag_cont), exp_targets,
			       num_exp_targets, targets, 5))
#else
    if (XmTargetsAreCompatible(XtDisplay(drag_cont), exp_targets,
			       num_exp_targets, targets, 4))
#endif
      cb->dropSiteStatus = XmVALID_DROP_SITE;
    else
      cb->dropSiteStatus = XmINVALID_DROP_SITE;
    break;
  case XmCR_DROP_SITE_LEAVE_MESSAGE:
  case XmCR_DROP_SITE_MOTION_MESSAGE:
  case XmCR_OPERATION_CHANGED:
    break;
  default:
    /* other messages we consider invalid */
    cb->dropSiteStatus = XmINVALID_DROP_SITE;
    break;
  }

  if (cb->dropSiteStatus == XmVALID_DROP_SITE) {
    if (cb->operation != XmDROP_COPY &&
	cb->operation != XmDROP_MOVE)
      cb->dropSiteStatus = XmINVALID_DROP_SITE;
  }
}

static void
RegisterDropSite(Widget w)
{
  enum { XmACOMPOUND_TEXT, XmATEXT,
#ifdef XM_UTF8
      XmAUTF8_STRING,
#endif
      NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSTEXT,
#ifdef XM_UTF8
      XmSUTF8_STRING
#endif
      };

  Atom targets[5];
  Arg args[10];
  int n;
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  targets[0] = XmeGetEncodingAtom(w);
  targets[1] = atoms[XmACOMPOUND_TEXT];
  targets[2] = XA_STRING;
  targets[3] = atoms[XmATEXT];
#ifdef XM_UTF8
  targets[4] = atoms[XmAUTF8_STRING];
#endif

  n = 0;
  XtSetArg(args[n], XmNimportTargets, targets); n++;
#ifdef XM_UTF8
  XtSetArg(args[n], XmNnumImportTargets, 5); n++;
#else
  XtSetArg(args[n], XmNnumImportTargets, 4); n++;
#endif
  XtSetArg(args[n], XmNdragProc, DragProcCallback); n++;
  XmeDropSink(w, args, n);
}


void
_XmTextInputCreate(Widget wid,
		   ArgList args,
		   Cardinal num_args)
{
  XmTextWidget tw = (XmTextWidget) wid;
  Input input;
  InputData data;
  XtPointer temp_ptr;

  tw->text.input = input = (Input) XtMalloc((unsigned) sizeof(InputRec));
  input->data = data = (InputData) XtMalloc((unsigned) sizeof(InputDataRec));
  XtGetSubresources(wid, (XtPointer)data, NULL, NULL, input_resources,
		    XtNumber(input_resources), args, num_args);
  data->widget = tw;

  if (data->sarray == NULL) data->sarray = (XmTextScanType *) sarray;

  if (data->sarraycount <= 0) data->sarraycount = XtNumber(sarray);

  /*
   * Fix for HaL DTS 9841 - copy the selectionArray into dedicated memory.
   */
  temp_ptr = (XtPointer)data->sarray;
  data->sarray = (XmTextScanType *)XtMalloc(data->sarraycount *
					    sizeof(XmTextScanType));
  memcpy((void *)data->sarray, (void *)temp_ptr, (data->sarraycount *
						  sizeof(XmTextScanType)));
  /*
   * End fix for HaL DTS 9841
   */

  data->lasttime = 0;
  data->cancel = True;
  data->stype = data->sarray[0];
  data->extendDir = XmsdRight;
  data->extending = FALSE;
  data->sel_start = FALSE;
  data->origLeft = 0;
  data->origRight = 0;
  data->selectionHint.x = data->selectionHint.y = 0;
  data->anchor = 0;

  data->hasSel2 = FALSE;
  data->sel2Left = 0;
  data->sel2Right = 0;
  data->Sel2OrigLeft =  0;
  data->Sel2OrigRight =  0;
  data->Sel2ExtendDir = XmsdRight;
  data->Sel2Extending = FALSE;
  data->Sel2Hint.x = data->Sel2Hint.y = 0;
  data->select_pos_x = data->select_pos_y = 0;

  data->select_id = 0;
  data->sec_time = 0;
  data->dest_time = 0;
  data->syncing = FALSE;
  data->has_destination = FALSE;
  data->overstrike = FALSE;
  data->selectionMove = FALSE;
  data->selectionLink = FALSE;
  data->drag_id = 0;
  data->transfer_action = NULL;

  /* This EventHandler scans the event queue to see if there are more
     key events pending for this widget. If there are, it will disable
     redisplay until there are no more key event on the queue. This can
     have severe effects when using certain input methods: The event
     that was in the queue may not get passed to the eventhandler - it
     could be filtered out by the input method. If this was the last key
     event, the text may never be redrawn again. Put it in a comment for
     now. Also, this seems to get used only in rare cases.
     XtAddEventHandler((Widget) tw, KeyPressMask, FALSE, CheckSync, NULL);
  */

  input->Invalidate = Invalidate;
  input->GetValues = InputGetValues;
  input->SetValues = InputSetValues;
  input->destroy = InputDestroy;


  if (tw->text.editable) {
    XmTextSetEditable((Widget)tw, False);
    XmTextSetEditable((Widget)tw, True);
  }

  RegisterDropSite(wid);
}

static XmTextPosition
XtoPosInLine(XmTextWidget tw,
	     Position x,
	     LineNum line)
{
  OutputData data = tw->text.output->data;
  Position        x1 = 0, y1 = 0;
  XmTextPosition  pos;

  pos = (*tw->text.output->XYToPos)(tw, x,
				    line * data->lineheight + data->topmargin);

  (*tw->text.output->PosToXY)(tw, pos, &x1, &y1);
  if (pos > 0 && x1 > x)
    return pos-1;
  else
    return pos;
}


static XmTextPosition
YtoPosInLine(XmTextWidget tw,
	     Position y,
	     LineNum line)
{
  OutputData data = tw->text.output->data;
  Position        x1 = 0, y1 = 0;
  XmTextPosition  pos;

  pos = (*tw->text.output->XYToPos)(tw, tw->text.inner_widget->core.width -
					(line * data->linewidth) -
					data->rightmargin, y);

  (*tw->text.output->PosToXY)(tw, pos, &x1, &y1);
  if (pos > 0 && y1 > y)
    return pos-1;
  else
    return pos;
}

