/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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
static char rcsid[] = "$TOG: TextF.c /main/65 1999/09/01 17:28:48 mgreess $"
#endif
#endif

#include <stdio.h>
#include <limits.h>		/* required for MB_LEN_MAX definition */
#include <string.h>

#include "XmI.h"
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/AccColorT.h>
#include <Xm/AccTextT.h>
#include <Xm/CutPaste.h>
#include <Xm/Display.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/DragIconP.h>
#include <Xm/DrawP.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/ManagerP.h>
#include <Xm/TraitP.h>
#include <Xm/TransferP.h>
#include <Xm/TransltnsP.h>
#include <Xm/XmosP.h>
#include <Xm/VaSimpleP.h>
#include "DestI.h"
#include "DisplayI.h"
#include "ImageCachI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "TextFI.h"
#include "TextFSelI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "VendorSEI.h"

#include <Xm/XmP.h>

#define MSG1		_XmMMsgTextF_0000
#define MSG2		_XmMMsgTextF_0001
#define MSG3		_XmMMsgTextF_0002 /* unused */
#define MSG4		_XmMMsgTextF_0003 /* unused */
#define MSG5		_XmMMsgTextF_0004 /* unused */
#define MSG7		_XmMMsgTextF_0006
#define WC_MSG1		_XmMMsgTextFWcs_0000 /* unused */
#define GRABKBDERROR	_XmMMsgRowColText_0024

#define TEXT_INCREMENT		 32
#define PRIM_SCROLL_INTERVAL	100
#define SEC_SCROLL_INTERVAL	200
#define XmDYNAMIC_BOOL		255

/* For the action parameters that are processed as reptypes */
#define _RIGHT 0
#define _LEFT  1

#define EventBindings1	_XmTextF_EventBindings1
#define EventBindings2	_XmTextF_EventBindings2
#define EventBindings3	_XmTextF_EventBindings3

#define TEXT_MAX_INSERT_SIZE 64    /* Size of buffer for XLookupString. */

typedef struct {
  Boolean has_destination;
  XmTextPosition position;
  int replace_length;
  Boolean quick_key;
} TextFDestDataRec, *TextFDestData;

/********    Static Function Declarations    ********/

static XmImportOperator StringToXmString(Widget w, int n, XtArgVal *value);
static XmImportOperator WideToXmString(Widget w, int n, XtArgVal *value);
static void XmStringToString(Widget w, int n, XtArgVal *value);
static void XmStringToWide(Widget w, int n, XtArgVal *value);

static void FreeContextData(Widget w,
			    XtPointer clientData,
			    XtPointer callData);

static TextFDestData GetTextFDestData(Widget w);

static _XmHighlightRec * FindHighlight(XmTextFieldWidget w,
				       XmTextPosition position);

static void InsertHighlight(XmTextFieldWidget w,
			    XmTextPosition position,
			    XmHighlightMode mode);

static void TextFieldSetHighlight(XmTextFieldWidget tf,
				  XmTextPosition left,
				  XmTextPosition right,
				  XmHighlightMode mode);

static Boolean GetXYFromPos(XmTextFieldWidget tf,
			    XmTextPosition position,
			    Position *x,
			    Position *y);

static Boolean CurrentCursorState(XmTextFieldWidget tf);

static void PaintCursor(XmTextFieldWidget tf);

static void BlinkInsertionPoint(XmTextFieldWidget tf);

static void HandleTimer(XtPointer closure,
                        XtIntervalId *id);

static void ChangeBlinkBehavior(XmTextFieldWidget tf, Boolean turn_on);

static void GetRect(XmTextFieldWidget tf,
                    XRectangle *rect);

static void SetFullGC(XmTextFieldWidget tf,
                        GC gc);

static void SetMarginGC(XmTextFieldWidget tf,
                          GC gc);

static void SetNormGC(XmTextFieldWidget tf,
                        GC gc,
                        Boolean change_stipple,
                        Boolean stipple);

static void SetShadowGC(XmTextFieldWidget tf,
                        GC gc);

static void SetInvGC(XmTextFieldWidget tf,
		       GC gc);

static void DrawText(XmTextFieldWidget tf, GC gc, int x, int y, XmString string);
static int FindPixelLength(XmTextFieldWidget tf, XmString s);

static void DrawTextSegment(XmTextFieldWidget tf,
			    XmHighlightMode mode,
			    XmTextPosition prev_seg_start,
			    XmTextPosition seg_start,
			    XmTextPosition seg_end,
			    XmTextPosition next_seg,
			    Boolean stipple,
			    int y,
			    int *x);

static void RedisplayText(XmTextFieldWidget tf,
			  XmTextPosition start,
			  XmTextPosition end);

static void ComputeSize(XmTextFieldWidget tf,
                        Dimension *width,
                        Dimension *height);

static XtGeometryResult TryResize(XmTextFieldWidget tf,
                                  Dimension width,
                                  Dimension height);

static Boolean AdjustText(XmTextFieldWidget tf,
			  XmTextPosition position,
                          Boolean flag);

static void AdjustSize(XmTextFieldWidget tf);

static Boolean ModifyVerify(XmTextFieldWidget tf, XEvent *event,
                            XmTextPosition *replace_prev,
                            XmTextPosition *replace_next,
                            XmString *insert,
                            size_t *insert_length,
                            XmTextPosition *newInsert);

static void ResetClipOrigin(XmTextFieldWidget tf);

static void InvertImageGC(XmTextFieldWidget tf);

static void ResetImageGC(XmTextFieldWidget tf);

typedef enum { ForceTrue, DontCare } PassDisown;
static void SetCursorPosition(XmTextFieldWidget tf,
			      XEvent *event,
			      XmTextPosition position,
                              Boolean adjust_flag,
                              Boolean call_cb,
                              Boolean set_dest,
			      PassDisown passDisown);

static void VerifyBounds(XmTextFieldWidget tf,
			 XmTextPosition *from,
			 XmTextPosition *to);

static XmTextPosition GetPosFromX(XmTextFieldWidget tf,
                                  Position x);

static Boolean SetDestination(Widget w,
			      XmTextPosition position,
			      Boolean disown,
			      Time set_time);

static Boolean VerifyLeave(XmTextFieldWidget tf,
			   XEvent *event);

static Boolean _XmTextFieldIsWordBoundary(XmTextFieldWidget tf,
					  XmTextPosition pos1,
					  XmTextPosition pos2);

static void FindWord(XmTextFieldWidget tf, XmTextPosition begin,
                     XmTextPosition *left, XmTextPosition *right);
static void FindPrevWord(XmTextFieldWidget tf, XmTextPosition *left, XmTextPosition *right);
static void FindNextWord(XmTextFieldWidget tf, XmTextPosition *left, XmTextPosition *right);

static void CheckDisjointSelection(Widget w,
				   XmTextPosition position,
				   Time sel_time);

static Boolean NeedsPendingDelete(XmTextFieldWidget tf);

static Boolean NeedsPendingDeleteDisjoint(XmTextFieldWidget tf);

static void InsertChar(Widget w,
		       XEvent *event,
		       char **params,
		       Cardinal *num_params);

static void DeletePrevChar(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void DeleteNextChar(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void DeletePrevWord(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void DeleteNextWord(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void DeleteToEndOfLine(Widget w,
			      XEvent *event,
			      char **params,
			      Cardinal *num_params);

static void DeleteToStartOfLine(Widget w,
				XEvent *event,
				char **params,
				Cardinal *num_params);

static void ProcessCancel(Widget w,
			  XEvent *event,
			  char **params,
			  Cardinal *num_params);

static void Activate(Widget w,
		     XEvent *event,
		     char **params,
		     Cardinal *num_params);

static void SetAnchorBalancing(XmTextFieldWidget tf,
			       XmTextPosition position);

static void SetNavigationAnchor(XmTextFieldWidget tf,
				XmTextPosition old_position,
				XmTextPosition new_position,
                                Boolean extend);

static void CompleteNavigation(XmTextFieldWidget tf,
			       XEvent *event,
			       XmTextPosition position,
			       Time time,
                               Boolean extend);

static void SimpleMovement(Widget w,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params,
			   XmTextPosition cursorPos,
			   XmTextPosition position);

static void BackwardChar(Widget w, XEvent *event, char **params, Cardinal *num_params);
static void ForwardChar(Widget w, XEvent *event, char **params, Cardinal *num_params);
static void BackwardWord(Widget w, XEvent *event, char **params, Cardinal *num_params);
static void ForwardWord(Widget w, XEvent *event, char **params, Cardinal *num_params);
static void EndOfLine(Widget w, XEvent *event, char **params, Cardinal *num_params);
static void BeginningOfLine(Widget w, XEvent *event, char **params, Cardinal *num_params);

static void SetSelection(XmTextFieldWidget tf,
			 XmTextPosition left,
			 XmTextPosition right,
                         Boolean redisplay);

static void ProcessHorizontalParams(Widget w,
				    XEvent *event,
				    char **params,
				    Cardinal *num_params,
				    XmTextPosition *left,
				    XmTextPosition *right,
				    XmTextPosition *position);

static void ProcessSelectParams(Widget w,
				XEvent *event,
				XmTextPosition *left,
				XmTextPosition *right,
				XmTextPosition *position);

static void KeySelection(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void TextFocusIn(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void TextFocusOut(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void SetScanIndex(XmTextFieldWidget tf,
			 XEvent *event);

static void ExtendScanSelection(XmTextFieldWidget tf,
				XEvent *event);

static void SetScanSelection(XmTextFieldWidget tf,
			     XEvent *event);

static void StartPrimary(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void MoveDestination(Widget w,
			    XEvent *event,
			    char **params,
			    Cardinal *num_params);

static void ExtendPrimary(Widget w,
			  XEvent *event,
			  char **params,
			  Cardinal *num_params);

static void ExtendEnd(Widget w,
		      XEvent *event,
		      char **params,
		      Cardinal *num_params);

static void DoExtendedSelection(Widget w,
				Time time);

static void DoSecondaryExtend(Widget w,
			      Time ev_time);

static void BrowseScroll(XtPointer closure,
			 XtIntervalId *id);

static Boolean CheckTimerScrolling(Widget w,
				   XEvent *event);

static void RestorePrimaryHighlight(XmTextFieldWidget tf,
				    XmTextPosition prim_left,
				    XmTextPosition prim_right);

static void StartDrag(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void DragStart(XtPointer data,
		      XtIntervalId *id);

static void StartSecondary(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void ProcessBDrag(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void ProcessBDragEvent(Widget w,
			       XEvent *event,
			       char **params,
			       Cardinal *num_params);

static void ProcessBDragRelease(Widget w,
				XEvent *event,
				String *params,
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

static void ExtendSecondary(Widget w,
			    XEvent *event,
			    char **params,
			    Cardinal *num_params);

static void Stuff(Widget w,
		  XEvent *event,
		  char **params,
		  Cardinal *num_params);

static void SecondaryNotify(Widget w,
			    XEvent *event,
			    char **params,
			    Cardinal *num_params);

static void ProcessCopy(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void ProcessLink(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void ProcessMove(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void DeleteSelection(Widget w,
			    XEvent *event,
			    char **params,
			    Cardinal *num_params);

static void ClearSelection(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void PageRight(Widget w,
		      XEvent *event,
		      char **params,
		      Cardinal *num_params);

static void PageLeft(Widget w,
		     XEvent *event,
		     char **params,
		     Cardinal *num_params);

static void CopyPrimary(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void CutPrimary(Widget w,
		       XEvent *event,
		       char **params,
		       Cardinal *num_params);

static void LinkPrimary(Widget w,
		       XEvent *event,
		       char **params,
		       Cardinal *num_params);

static void SetAnchor(Widget w,
		      XEvent *event,
		      char **params,
		      Cardinal *num_params);

static void ToggleOverstrike(Widget w,
			     XEvent *event,
			     char **params,
			     Cardinal *num_params);

static void ToggleAddMode(Widget w,
			  XEvent *event,
			  char **params,
			  Cardinal *num_params);

static void SelectAll(Widget w,
		      XEvent *event,
		      char **params,
		      Cardinal *num_params);

static void DeselectAll(Widget w,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params);

static void VoidAction(Widget w,
		       XEvent *event,
		       char **params,
		       Cardinal *num_params);

static void CutClipboard(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void CopyClipboard(Widget w,
			  XEvent *event,
			  char **params,
			  Cardinal *num_params);

static void PasteClipboard(Widget w,
			   XEvent *event,
			   char **params,
			   Cardinal *num_params);

static void TraverseDown(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void TraverseUp(Widget w,
		       XEvent *event,
		       char **params,
		       Cardinal *num_params);

static void TraverseHome(Widget w,
			 XEvent *event,
			 char **params,
			 Cardinal *num_params);

static void TraverseNextTabGroup(Widget w,
				 XEvent *event,
				 char **params,
				 Cardinal *num_params);

static void TraversePrevTabGroup(Widget w,
				 XEvent *event,
				 char **params,
				 Cardinal *num_params);

static void TextEnter(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void TextLeave(Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);

static void ClassInitialize(void);

static void ClassPartInitialize(WidgetClass w_class);

static void Validates(XmTextFieldWidget tf);

static void  LoadFontMetrics(XmTextFieldWidget tf);

static void InitializeTextStruct(XmTextFieldWidget tf);

static void LoadGCs(XmTextFieldWidget tf,
		    Pixel background,
		    Pixel foreground);

static void MakeIBeamOffArea(XmTextFieldWidget tf,
                             Dimension width,
                             Dimension height);

static Pixmap FindPixmap(Screen *screen,
			 char *image_name,
			 Pixel foreground,
			 Pixel background,
			 int depth);

static void MakeIBeamStencil(XmTextFieldWidget tf,
			     int line_width);

static void MakeAddModeCursor(XmTextFieldWidget tf,
			      int line_width);

static void MakeCursors(XmTextFieldWidget tf);

static void DragProcCallback(Widget w,
			     XtPointer client,
			     XtPointer call);

static void RegisterDropSite(Widget w);

static void Initialize(Widget request,
		       Widget new_w,
		       ArgList args,
		       Cardinal *num_args);

static void Realize(Widget w,
		    XtValueMask *valueMask,
		    XSetWindowAttributes *attributes);

static void Destroy(Widget wid);

static void Resize(Widget w);

static XtGeometryResult QueryGeometry(Widget w,
				      XtWidgetGeometry *intended,
				      XtWidgetGeometry *reply);

static void TextFieldExpose(Widget w,
			    XEvent *event,
			    Region region);

static Boolean SetValues(Widget old,
			 Widget request,
			 Widget new_w,
			 ArgList args,
			 Cardinal *num_args);

static Boolean TextFieldGetBaselines(Widget w,
				     Dimension **baselines,
				     int *line_count);

static Boolean TextFieldGetDisplayRect(Widget w,
				       XRectangle *display_rect);

static void TextFieldMarginsProc(Widget w,
				 XmBaselineMargins *margins_rec);

static XtPointer TextFieldGetValue(Widget w,
				   int format);

static void TextFieldSetValue(Widget w,
			      XtPointer s,
			      int format);

static int TextFieldPreferredValue(Widget w);

static void CheckSetRenderTable(Widget wid,
				int offset,
				XrmValue *value);

static Boolean TextFieldRemove(Widget w,
			       XEvent *event);
static void TextFieldReplace(Widget w,
			     XmTextPosition from_pos,
			     XmTextPosition to_pos,
			     char *value,
			     int is_wc);
static int PreeditStart(XIC xic,
                        XPointer client_data,
                        XPointer call_data);

static void PreeditDone(XIC xic,
                        XPointer client_data,
                        XPointer call_data);

static void PreeditDraw(XIC xic,
                        XPointer client_data,
                        XIMPreeditDrawCallbackStruct *call_data);

static void PreeditCaret(XIC xic,
                         XPointer client_data,
                         XIMPreeditCaretCallbackStruct *call_data);

static void PreeditSetCursorPosition(XmTextFieldWidget tf,
                                     XmTextPosition position);

static void TextFieldResetIC(Widget w);
static void doSetHighlight(Widget w, XmTextPosition left, XmTextPosition right,
                         XmHighlightMode mode) ;
static Boolean TrimHighlights(XmTextFieldWidget tf, int *low, int *high);

static void ResetUnder(XmTextFieldWidget tf);

/********    End Static Function Declarations    ********/

static const XmTextScanType sarray[] = {
  XmSELECT_POSITION, XmSELECT_WORD, XmSELECT_LINE
};

static XContext _XmTextFDestContext = 0;

/* default translations and action recs */
static XtActionsRec text_actions[] = {
/* Text Replacing Bindings */
  {"self-insert",		InsertChar},
  {"delete-previous-character",	DeletePrevChar},
  {"delete-next-character",	DeleteNextChar},
  {"delete-previous-word",	DeletePrevWord},
  {"delete-next-word",		DeleteNextWord},
  {"delete-to-end-of-line",	DeleteToEndOfLine},
  {"delete-to-start-of-line",	DeleteToStartOfLine},
/* Miscellaneous Bindings */
  {"activate",			Activate},
  {"process-cancel",		ProcessCancel},
  {"process-bdrag",		ProcessBDrag},
  {"process-bdrag-event",	ProcessBDragEvent},
  {"process-bselect",		ProcessBSelect},
  {"process-bselect-event",	ProcessBSelectEvent},
/* Motion Bindings */
  {"backward-character",	BackwardChar},
  {"forward-character",		ForwardChar},
  {"backward-word",		BackwardWord},
  {"forward-word",		ForwardWord},
  {"end-of-line",		EndOfLine},
  {"beginning-of-line",		BeginningOfLine},
  {"page-left",			PageLeft},
  {"page-right",		PageRight},
/* Selection Bindings */
  {"key-select",		KeySelection},
  {"grab-focus",		StartPrimary},
  {"move-destination",		MoveDestination},
  {"extend-start",		ExtendPrimary},
  {"extend-adjust",		ExtendPrimary},
  {"extend-end",		ExtendEnd},
  {"delete-selection",		DeleteSelection},
  {"clear-selection",		ClearSelection},
  {"cut-primary",		CutPrimary},
  {"link-primary",		LinkPrimary},
  {"copy-primary",		CopyPrimary},
  {"set-anchor",		SetAnchor},
  {"toggle-overstrike",		ToggleOverstrike},
  {"toggle-add-mode",		ToggleAddMode},
  {"select-all",		SelectAll},
  {"deselect-all",		DeselectAll},
/* Quick Cut and Paste Bindings */
  {"secondary-start",		StartSecondary},
  {"secondary-adjust",		ExtendSecondary},
  {"copy-to",			ProcessCopy},
  {"link-to",			ProcessLink},
  {"move-to",			ProcessMove},
  {"quick-cut-set",		VoidAction},
  {"quick-copy-set",		VoidAction},
  {"do-quick-action",		VoidAction},
/* Clipboard Bindings */
  {"cut-clipboard",		CutClipboard},
  {"copy-clipboard",		CopyClipboard},
  {"paste-clipboard",		PasteClipboard},
/* Traversal */
  {"traverse-next",		TraverseDown},
  {"traverse-prev",		TraverseUp},
  {"traverse-home",		TraverseHome},
  {"next-tab-group",		TraverseNextTabGroup},
  {"prev-tab-group",		TraversePrevTabGroup},
/* Focus */
  {"focusIn",			TextFocusIn},
  {"focusOut",			TextFocusOut},
  {"enter",			TextEnter},
  {"leave",			TextLeave},
};

static XtResource resources[] =
{
  {
    XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.activate_callback),
    XmRCallback, NULL
  },

  {
    XmNlosingFocusCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.losing_focus_callback),
    XmRCallback, NULL
  },

  {
    XmNfocusCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.focus_callback),
    XmRCallback, NULL
  },

  {
	XmNmodifyVerifyCallbackStr, XmCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(struct _XmTextFieldRec, text.str_modify_verify_callback),
	XmRCallback, NULL
  },

  { /* Deprecated */
    XmNmodifyVerifyCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.modify_verify_callback),
    XmRCallback, NULL
  },

  { /* Deprecated */
    XmNmodifyVerifyCallbackWcs, XmCCallback, XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.wcs_modify_verify_callback),
    XmRCallback, NULL
  },

  {
    XmNmotionVerifyCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.motion_verify_callback),
    XmRCallback, NULL
  },

  {
    XmNgainPrimaryCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.gain_primary_callback),
    XmRCallback, NULL
  },

  {
    XmNlosePrimaryCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.lose_primary_callback),
    XmRCallback, NULL
  },

  {
    XmNvalueChangedCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.value_changed_callback),
    XmRCallback, NULL
  },

  {
    XmNdestinationCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextFieldRec, text.destination_callback),
    XmRCallback, NULL
  },

  {
	XmNvalueString, XmCXmString, XmRXmString, sizeof(XmString),
	XtOffsetOf(struct _XmTextFieldRec, text.xms_value),
	XtRImmediate, NULL
  },

  {
    XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
    sizeof(Dimension),
    XtOffsetOf(struct _XmTextFieldRec, text.margin_height),
    XmRImmediate, (XtPointer) 5
  },

  {
    XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension),
    XtOffsetOf(struct _XmTextFieldRec, text.margin_width),
    XmRImmediate, (XtPointer) 5
  },

  {
    XmNcursorPosition, XmCCursorPosition, XmRTextPosition,
    sizeof (XmTextPosition),
    XtOffsetOf(struct _XmTextFieldRec, text.cursor_position),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNcolumns, XmCColumns, XmRShort, sizeof(short),
    XtOffsetOf(struct _XmTextFieldRec, text.columns),
    XmRImmediate, (XtPointer) 20
  },

  {
    XmNmaxLength, XmCMaxLength, XmRInt, sizeof(int),
    XtOffsetOf(struct _XmTextFieldRec, text.max_length),
    XmRImmediate, (XtPointer) INT_MAX
  },

  {
    XmNblinkRate, XmCBlinkRate, XmRInt, sizeof(int),
    XtOffsetOf(struct _XmTextFieldRec, text.blink_rate),
    XmRImmediate, (XtPointer) 500
  },

  {
      "pri.vate","Pri.vate",XmRBoolean,
      sizeof(Boolean), XtOffsetOf(struct _XmTextFieldRec,
	  text.check_set_render_table),
	  XmRImmediate, (XtPointer) False
  },

  {
    XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
    XtOffsetOf(struct _XmTextFieldRec, text.font_list),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
    XtOffsetOf(struct _XmTextFieldRec, text.font_list),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNselectionArray, XmCSelectionArray, XmRPointer,
    sizeof(XtPointer),
    XtOffsetOf(struct _XmTextFieldRec, text.selection_array),
    XmRImmediate, (XtPointer) sarray
  },

  {
    XmNselectionArrayCount, XmCSelectionArrayCount, XmRInt, sizeof(int),
    XtOffsetOf(struct _XmTextFieldRec, text.selection_array_count),
    XmRImmediate, (XtPointer) XtNumber(sarray)
  },

  {
    XmNresizeWidth, XmCResizeWidth, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmTextFieldRec, text.resize_width),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNpendingDelete, XmCPendingDelete, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmTextFieldRec, text.pending_delete),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmTextFieldRec, text.editable),
    XmRImmediate, (XtPointer) True
  },

  { /* XXX: This was false if the widget had a print shell ancestor */
    XmNcursorPositionVisible, XmCCursorPositionVisible, XmRBoolean,
    sizeof(Boolean),
    XtOffsetOf(struct _XmTextFieldRec, text.cursor_position_visible),
    XmRImmediate, (XtPointer)True
  },

 {
   XmNverifyBell, XmCVerifyBell, XmRBoolean, sizeof(Boolean),
   XtOffsetOf(struct _XmTextFieldRec, text.verify_bell),
   XmRImmediate, (XtPointer) XmDYNAMIC_BOOL
 },

 {
   XmNselectThreshold, XmCSelectThreshold, XmRInt, sizeof(int),
   XtOffsetOf(struct _XmTextFieldRec, text.threshold),
   XmRImmediate, (XtPointer) 5
 },

 {
   XmNnavigationType, XmCNavigationType, XmRNavigationType,
   sizeof (unsigned char),
   XtOffsetOf(struct _XmPrimitiveRec, primitive.navigation_type),
   XmRImmediate, (XtPointer) XmTAB_GROUP
 }
};

static XmSyntheticResource syn_resources[] =
{
 {
   XmNmarginWidth,
   sizeof(Dimension),
   XtOffsetOf(struct _XmTextFieldRec, text.margin_width),
   XmeFromHorizontalPixels,
   XmeToHorizontalPixels
 },

 {
   XmNmarginHeight,
   sizeof(Dimension),
   XtOffsetOf(struct _XmTextFieldRec, text.margin_height),
   XmeFromVerticalPixels,
   XmeToVerticalPixels
 },

 {
   XmNvalue,
   sizeof(String),
   XtOffsetOf(struct _XmTextFieldRec, text.value),
   XmStringToString, /* export */
   StringToXmString  /* import */
 },

 {
   XmNvalueWcs,
   sizeof(wchar_t *),
   XtOffsetOf(struct _XmTextFieldRec, text.wc_value),
   XmStringToWide, /* export */
   WideToXmString  /* import */
 }
};

static XmPrimitiveClassExtRec _XmTextFPrimClassExtRec = {
  NULL,
  NULLQUARK,
  XmPrimitiveClassExtVersion,
  sizeof(XmPrimitiveClassExtRec),
  TextFieldGetBaselines,                  /* widget_baseline */
  TextFieldGetDisplayRect,               /* widget_display_rect */
  TextFieldMarginsProc,                  /* get/set widget margins */
};

externaldef(xmtextfieldclassrec) XmTextFieldClassRec xmTextFieldClassRec =
{
  {
    (WidgetClass) &xmPrimitiveClassRec,		/* superclass         */
    "XmTextField",				/* class_name         */
    sizeof(XmTextFieldRec),		        /* widget_size        */
    ClassInitialize,				/* class_initialize   */
    ClassPartInitialize,			/* class_part_initiali*/
    FALSE,					/* class_inited       */
    Initialize,					/* initialize         */
    (XtArgsProc)NULL,				/* initialize_hook    */
    Realize,					/* realize            */
    text_actions,				/* actions            */
    XtNumber(text_actions),			/* num_actions        */
    resources,					/* resources          */
    XtNumber(resources),			/* num_resources      */
    NULLQUARK,					/* xrm_class          */
    TRUE,					/* compress_motion    */
    XtExposeCompressMaximal |			/* compress_exposure  */
      XtExposeNoRegion,
    TRUE,					/* compress_enterleave*/
    FALSE,					/* visible_interest   */
    Destroy,					/* destroy            */
    Resize,					/* resize             */
    TextFieldExpose,				/* expose             */
    SetValues,					/* set_values         */
    (XtArgsFunc)NULL,				/* set_values_hook    */
    XtInheritSetValuesAlmost,			/* set_values_almost  */
    (XtArgsProc)NULL,				/* get_values_hook    */
    (XtAcceptFocusProc)NULL,			/* accept_focus       */
    XtVersion,					/* version            */
    NULL,					/* callback_private   */
    NULL,					/* tm_table           */
    QueryGeometry,				/* query_geometry     */
    (XtStringProc)NULL,				/* display accel      */
    NULL,					/* extension          */
  },

  {	                          		/* Xmprimitive        */
    XmInheritBorderHighlight,        		/* border_highlight   */
    XmInheritBorderUnhighlight,              	/* border_unhighlight */
    NULL,					/* translations	      */
    (XtActionProc)NULL,             		/* arm_and_activate   */
    syn_resources,            			/* syn resources      */
    XtNumber(syn_resources),  			/* num syn_resources  */
    (XtPointer) &_XmTextFPrimClassExtRec,	/* extension          */
  },

  {                         			/* text class         */
    NULL,                     			/* extension          */
  }
};

externaldef(xmtextfieldwidgetclass) WidgetClass xmTextFieldWidgetClass =
					 (WidgetClass) &xmTextFieldClassRec;

/* AccessXmString Trait record for TextField */
static const XmAccessTextualTraitRec textFieldCS = {
  0,  				/* version */
  TextFieldGetValue,
  TextFieldSetValue,
  TextFieldPreferredValue,
};

static void ClassInitialize(void)
{
  _XmTextFieldInstallTransferTrait();
  XmeTraitSet((XtPointer)xmTextFieldWidgetClass, XmQTaccessTextual,
	      (XtPointer) &textFieldCS);
}

static XmImportOperator StringToXmString(Widget w, int n, XtArgVal *value) {
	(void)w;
	(void)n;

	if (!*value)
		return XmSYNTHETIC_NONE;

	*value = (XtArgVal)XmStringCreateMultibyte((String)*value, NULL);
	return XmSYNTHETIC_NONE;
}

static XmImportOperator WideToXmString(Widget w, int n, XtArgVal *value) {
	(void)w;
	(void)n;

	if (!*value)
		return XmSYNTHETIC_NONE;

	*value = (XtArgVal)XmStringCreateWide((wchar_t *)*value, NULL);
	return XmSYNTHETIC_NONE;
}

static void XmStringToString(Widget w, int n, XtArgVal *value) {
	XmString s = XmTextFieldGetXmString(w);
	*value = (XtArgVal)XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmMULTIBYTE_TEXT);
	XmStringFree(s);
}

static void XmStringToWide(Widget w, int n, XtArgVal *value) {
	XmString s = XmTextFieldGetXmString(w);
	*value = (XtArgVal)XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT);
	XmStringFree(s);
}

static void
FreeContextData(Widget w,		/* unused */
		XtPointer clientData,
		XtPointer callData)	/* unused */
{
  XmTextContextData ctx_data = (XmTextContextData) clientData;
  Display *display = DisplayOfScreen(ctx_data->screen);
  XtPointer data_ptr;

  if (XFindContext(display, (Window) ctx_data->screen,
		   ctx_data->context, (char **) &data_ptr)) {

    if (ctx_data->type != '\0') {
      if (data_ptr)
	XtFree((char *) data_ptr);
    }

    XDeleteContext (display, (Window) ctx_data->screen, ctx_data->context);
  }

  XtFree ((char *) ctx_data);
}

static TextFDestData
GetTextFDestData(Widget w)
{
  TextFDestData dest_data;
  Display *display = XtDisplay(w);
  Screen *screen = XtScreen(w);
  XContext loc_context;

  _XmProcessLock();
  if (_XmTextFDestContext == 0)
    _XmTextFDestContext = XUniqueContext();
  loc_context = _XmTextFDestContext;
  _XmProcessUnlock();

  if (XFindContext(display, (Window) screen,
		   loc_context, (char **) &dest_data)) {
    XmTextContextData ctx_data;
    Widget xm_display = (Widget) XmGetXmDisplay(display);

    ctx_data = (XmTextContextData) XtMalloc(sizeof(XmTextContextDataRec));

    ctx_data->screen = screen;
    ctx_data->context = loc_context;
    ctx_data->type = _XM_IS_DEST_CTX;

    dest_data = (TextFDestData) XtCalloc((unsigned)sizeof(TextFDestDataRec),
					 (unsigned) 1);

    XtAddCallback(xm_display, XmNdestroyCallback,
		  (XtCallbackProc) FreeContextData, (XtPointer) ctx_data);

    XSaveContext(XtDisplay(w), (Window) screen,
		 loc_context, (XPointer)dest_data);
  }

  return dest_data;
}

void
_XmTextFToggleCursorGC(Widget widget)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) widget;
  XGCValues values;
  unsigned long valueMask;
  Pixmap stipple = XmUNSPECIFIED_PIXMAP;

  SetFullGC(tf, tf->text.image_gc);
  ResetClipOrigin(tf);

  if (!XtIsRealized(widget)) return;

  if (!XtIsSensitive((Widget)tf)) {
    valueMask = GCForeground|GCBackground|GCFillStyle|GCStipple|GCFunction;
    values.foreground = _XmAssignInsensitiveColor((Widget)tf);
    values.background = tf->core.background_pixel;
    values.fill_style = FillStippled;

    if (tf->text.overstrike) {
      if (tf->text.stipple_tile == XmUNSPECIFIED_PIXMAP) return;
      values.stipple = tf->text.stipple_tile;
      values.function = GXxor;
    } else {
      if (tf->text.cursor == XmUNSPECIFIED_PIXMAP) return;
      values.stipple = tf->text.cursor;
      values.function = GXcopy;
    }
  } else {
  if (tf->text.overstrike) {
    valueMask = GCFillStyle|GCFunction|GCForeground|GCBackground;
    if (!tf->text.add_mode && XtIsSensitive(widget) &&
	(tf->text.has_focus || tf->text.has_destination)) {
      values.fill_style = FillSolid;
    } else {
      valueMask |= GCStipple;
      values.fill_style = FillStippled;
      values.stipple = tf->text.stipple_tile;
    }
    values.foreground = values.background =
      tf->primitive.foreground ^ tf->core.background_pixel;
    values.function = GXxor;
  } else {
    valueMask = GCStipple;
    if (XGetGCValues(XtDisplay(widget), tf->text.image_gc,
		     valueMask, &values))
      stipple = values.stipple;
    valueMask = GCFillStyle|GCFunction|GCForeground|GCBackground;
    if (XtIsSensitive(widget) && !tf->text.add_mode &&
	(tf->text.has_focus || tf->text.has_destination)) {
      if (tf->text.cursor == XmUNSPECIFIED_PIXMAP) return;
      if (stipple != tf->text.cursor) {
	values.stipple = tf->text.cursor;
	valueMask |= GCStipple;
      }
    } else {
      if (tf->text.add_mode_cursor == XmUNSPECIFIED_PIXMAP) return;
      if (stipple != tf->text.add_mode_cursor) {
	values.stipple = tf->text.add_mode_cursor;
	valueMask |= GCStipple;
      }
    }
    values.fill_style = FillStippled;
    values.function = GXcopy;
    if (tf->text.have_inverted_image_gc) {
      values.background = tf->primitive.foreground;
      values.foreground = tf->core.background_pixel;
    } else {
      values.foreground = tf->primitive.foreground;
      values.background = tf->core.background_pixel;
    }
  }
  }

  XSetClipMask(XtDisplay(widget), tf->text.save_gc, None);
  XChangeGC(XtDisplay(widget), tf->text.image_gc, valueMask, &values);
}

/*
 * Find the highlight record corresponding to the given position.  Returns a
 * pointer to the record.  The third argument indicates whether we are probing
 * the left or right edge of a highlighting range.
 */
static _XmHighlightRec *
FindHighlight(XmTextFieldWidget w,
	      XmTextPosition position)
{
  _XmHighlightRec *l = w->text.highlight.list;
  int i;

  for (i=w->text.highlight.number - 1; i>=0; i--)
    if (position >= l[i].position) {
      l = l + i;
      break;
    }

  return(l);
}

static void
InsertHighlight(XmTextFieldWidget w,
		XmTextPosition position,
		XmHighlightMode mode)
{
  _XmHighlightRec *l1;
  _XmHighlightRec *l = w->text.highlight.list;
  int i, j;

  l1 = FindHighlight(w, position);
  if (l1->position == position)
    l1->mode = mode;
  else {
    i = (l1 - l) + 1;
    w->text.highlight.number++;
    if (w->text.highlight.number > w->text.highlight.maximum) {
      w->text.highlight.maximum = w->text.highlight.number;
      l = w->text.highlight.list = (_XmHighlightRec *)
	XtRealloc((char *) l, (unsigned)(w->text.highlight.maximum *
					 sizeof(_XmHighlightRec)));
    }
    for (j=w->text.highlight.number-1; j>i; j--)
      l[j] = l[j-1];
    l[i].position = position;
    l[i].mode = mode;
  }
}

static void
TextFieldSetHighlight(XmTextFieldWidget tf,
		      XmTextPosition left,
		      XmTextPosition right,
		      XmHighlightMode mode)
{
  _XmHighlightRec *l;
  XmHighlightMode endmode;
  int i, j;

  if (left >= right || right <= 0) return;

  _XmTextFieldDrawInsertionPoint(tf, False);
  endmode = FindHighlight(tf, right)->mode;
  InsertHighlight(tf, left, mode);
  InsertHighlight(tf, right, endmode);
  l = tf->text.highlight.list;
  i = 1;
  while (i < tf->text.highlight.number) {
    if (l[i].position >= left && l[i].position < right)
      l[i].mode = mode;
    if (l[i].mode == l[i-1].mode) {
      tf->text.highlight.number--;
      for (j=i; j<tf->text.highlight.number; j++)
	l[j] = l[j+1];
    } else i++;
  }
  if (TextF_CursorPosition(tf) > left && TextF_CursorPosition(tf) < right) {
    if (mode == XmHIGHLIGHT_SELECTED) {
      InvertImageGC(tf);
    } else if (mode != XmHIGHLIGHT_SELECTED) {
      ResetImageGC(tf);
    }
  }
  tf->text.refresh_ibeam_off = True;
  _XmTextFieldDrawInsertionPoint(tf, True);
}

/*
 * Get x and y based on position.
 */
static Boolean
GetXYFromPos(XmTextFieldWidget tf,
	     XmTextPosition position,
	     Position *x,
	     Position *y)
{
  XmString s;

  /* initialize the x and y positions to zero */
  *x = 0;
  *y = 0;

  if (position > (XmTextPosition)tf->text.string_length)
    return False;

  s = XmStringSubstr(tf->text.xms_value, 0, (size_t)position);
  *x += FindPixelLength(tf, s);
  XmStringFree(s);

  *y += tf->primitive.highlight_thickness + tf->primitive.shadow_thickness
    + tf->text.margin_top + TextF_FontAscent(tf);
  *x += (Position) tf->text.h_offset;

  return True;
}

static Boolean
CurrentCursorState(XmTextFieldWidget tf)
{
  if (tf->text.cursor_on < 0) return False;
  if (tf->text.blink_on || !XtIsSensitive((Widget)tf))
    return True;
  return False;
}

/*
 * Paint insert cursor
 */
static void
PaintCursor(XmTextFieldWidget tf)
{
  int pxlen;
  Position x, y;
  XmTextPosition position;
  XmString s;

  if (!TextF_CursorPositionVisible(tf)) return;

  _XmTextFToggleCursorGC((Widget)tf);

  position = TextF_CursorPosition(tf);
  (void) GetXYFromPos(tf, position, &x, &y);

  if (!tf->text.overstrike)
    x -=(tf->text.cursor_width >> 1) + 1; /* "+1" for 1 pixel left of char */
  else {
    s = XmStringSubstr(tf->text.xms_value, position, 1);
    pxlen = FindPixelLength(tf, s);
    XmStringFree(s);
    if (pxlen > tf->text.cursor_width)
      x += (pxlen - tf->text.cursor_width) >> 1;
  }
  y = (y + (Position) TextF_FontDescent(tf)) -
    (Position) tf->text.cursor_height;

  /* If time to paint the I Beam... first capture the IBeamOffArea, then draw
   * the IBeam */

  if (tf->text.refresh_ibeam_off == True) { /* get area under IBeam first */
    /* Fill is needed to realign clip rectangle with gc */
    XFillRectangle(XtDisplay((Widget)tf), XtWindow((Widget)tf),
		   tf->text.save_gc, 0, 0, 0, 0);
    XCopyArea(XtDisplay(tf), XtWindow(tf), tf->text.ibeam_off,
	      tf->text.save_gc, x, y, tf->text.cursor_width,
	      tf->text.cursor_height, 0, 0);
    tf->text.refresh_ibeam_off = False;
  }

    /* redraw cursor, being very sure to keep it within the bounds of the
    ** text area, not spilling into the highlight area
    */
  {
  int cursor_width = tf->text.cursor_width;
  int cursor_height = tf->text.cursor_height;
  if ((tf->text.cursor_on >= 0) && tf->text.blink_on) {
        if ((int)(x + tf->text.cursor_width) > (int)(tf->core.width -
		 tf->primitive.shadow_thickness -
		 tf->primitive.highlight_thickness))
          cursor_width = (tf->core.width -
                                 (tf->primitive.shadow_thickness +
                                 tf->primitive.highlight_thickness)) - x;
        if (cursor_width > 0 && cursor_height > 0) {
          if (!XtIsSensitive((Widget) tf)) {
            SetShadowGC(tf, tf->text.image_gc);
            XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.image_gc, x + 1, y + 1,
                           (unsigned int) cursor_width, (unsigned int) cursor_height);
          }
          _XmTextFToggleCursorGC((Widget) tf);
    		XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.image_gc, x, y,
		   cursor_width, cursor_height);
	    }
  } else {
        Position src_x = 0;
        if ((int)(x + tf->text.cursor_width) > (int)(tf->core.width -
                            tf->primitive.shadow_thickness -
                            tf->primitive.highlight_thickness)) {
          cursor_width = (tf->core.width -
                                 (tf->primitive.shadow_thickness +
                                 tf->primitive.highlight_thickness)) - x;
        }
         else if (x < (Position)(tf->primitive.highlight_thickness +
                                 tf->primitive.shadow_thickness)) {
             cursor_width = tf->text.cursor_width -
                                 (tf->primitive.highlight_thickness +
                                 tf->primitive.shadow_thickness - x);
             src_x = tf->text.cursor_width - cursor_width;
             x = tf->primitive.highlight_thickness +
                                 tf->primitive.shadow_thickness;
         }
        if ((int)(y + tf->text.cursor_height) > (int)(tf->core.height -
                                 (tf->primitive.highlight_thickness +
                                 tf->primitive.shadow_thickness))) {
          cursor_height = tf->text.cursor_height -
                                 ((y + tf->text.cursor_height) -
                                 (tf->core.height -
                                 (tf->primitive.highlight_thickness +
                                 tf->primitive.shadow_thickness)));
        }
           if (cursor_width > 0 && cursor_height > 0)
  	  	XCopyArea(XtDisplay(tf), tf->text.ibeam_off, XtWindow(tf),
          tf->text.save_gc, src_x, 0, cursor_width,
          cursor_height, x, y);
  	}
  }
}

void
_XmTextFieldDrawInsertionPoint(XmTextFieldWidget tf,
                               Boolean turn_on)
{

  if (turn_on == True) {
    tf->text.cursor_on += 1;
    if (TextF_BlinkRate(tf) == 0 || !tf->text.has_focus)
      tf->text.blink_on = True;
  } else {
    if (tf->text.blink_on && (tf->text.cursor_on == 0))
      if (tf->text.blink_on == CurrentCursorState(tf) &&
	  XtIsRealized((Widget)tf)) {
	tf->text.blink_on = !tf->text.blink_on;
	PaintCursor(tf);
      }
    tf->text.cursor_on -= 1;
  }

  if (tf->text.cursor_on < 0 || !XtIsRealized((Widget) tf))
    return;

  PaintCursor(tf);
}

static void
BlinkInsertionPoint(XmTextFieldWidget tf)
{
  if ((tf->text.cursor_on >= 0) &&
      (tf->text.blink_on == CurrentCursorState(tf)) &&
      XtIsRealized((Widget)tf)) {
    tf->text.blink_on = !tf->text.blink_on;
    PaintCursor(tf);
  }
}



/*
 * Handle blink on and off
 */
static void
HandleTimer(XtPointer closure,
	    XtIntervalId *id)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) closure;

  if (TextF_BlinkRate(tf) != 0)
    tf->text.timer_id =
      XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tf),
		      (unsigned long)TextF_BlinkRate(tf),
		      HandleTimer,
		      (XtPointer) closure);
  if (tf->text.has_focus && XtIsSensitive((Widget)tf)) BlinkInsertionPoint(tf);
}


/*
 * Change state of blinking insert cursor on and off
 */
static void
ChangeBlinkBehavior(XmTextFieldWidget tf,
                    Boolean turn_on)
{

  if (turn_on) {
    if (TextF_BlinkRate(tf) != 0 && tf->text.timer_id == (XtIntervalId)0)
      tf->text.timer_id =
	XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)tf),
			(unsigned long)TextF_BlinkRate(tf),
			HandleTimer,
			(XtPointer) tf);
    tf->text.blink_on = True;
  } else {
    if (tf->text.timer_id)
      XtRemoveTimeOut(tf->text.timer_id);
    tf->text.timer_id = (XtIntervalId)0;
  }
}

static void
GetRect(XmTextFieldWidget tf,
        XRectangle *rect)
{
  Dimension margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Dimension margin_top = tf->text.margin_top + tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
  Dimension margin_bottom = tf->text.margin_bottom +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  if (margin_width < tf->core.width)
    rect->x = margin_width;
  else
    rect->x = tf->core.width;

  if (margin_top  < tf->core.height)
    rect->y = margin_top;
  else
    rect->y = tf->core.height;

  if ((Dimension)(2 * margin_width) < tf->core.width)
    rect->width = (int) tf->core.width - (2 * margin_width);
  else
    rect->width = 0;

  if ((Dimension)(margin_top + margin_bottom) < tf->core.height)
    rect->height = (int) tf->core.height - (margin_top + margin_bottom);
  else
    rect->height = 0;
}

static void
SetFullGC(XmTextFieldWidget tf,
	    GC gc)
{
  XRectangle ClipRect;

  /* adjust clip rectangle to allow the cursor to paint into the margins */
  ClipRect.x = tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
  ClipRect.y = tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
  ClipRect.width = tf->core.width - (2 * (tf->primitive.shadow_thickness +
                                          tf->primitive.highlight_thickness));
  ClipRect.height = tf->core.height - (2 *(tf->primitive.shadow_thickness +
					   tf->primitive.highlight_thickness));

  XSetClipRectangles(XtDisplay(tf), gc, 0, 0, &ClipRect, 1,
                     Unsorted);
}

static void
SetMarginGC(XmTextFieldWidget tf,
	      GC gc)
{
  XRectangle ClipRect;

  GetRect(tf, &ClipRect);
  XSetClipRectangles(XtDisplay(tf), gc, 0, 0, &ClipRect, 1,
                     Unsorted);
}

/*
 * Set new clipping rectangle for text field.  This is
 * done on each focus in event since the text field widgets
 * share the same GC.
 */
void _XmTextFieldSetClipRect(XmTextFieldWidget tf)
{
	SetMarginGC(tf, tf->text.gc);
}

static void
SetNormGC(XmTextFieldWidget tf,
	    GC gc,
            Boolean change_stipple,
            Boolean stipple)
{
  unsigned long valueMask = (GCForeground | GCBackground);
  XGCValues values;

  _XmTextFieldSetClipRect(tf);
  values.foreground = tf->primitive.foreground;
  values.background = tf->core.background_pixel;

  if (change_stipple) {
    valueMask |= GCFillStyle;
    if (stipple) {
      /*generally gray insensitive foreground (instead stipple)*/
		  values.foreground = _XmAssignInsensitiveColor((Widget)tf);
    	  values.fill_style = FillSolid;
    } else
      values.fill_style = FillSolid;
  }

  XChangeGC(XtDisplay(tf), gc, valueMask, &values);
}

static void
SetShadowGC(XmTextFieldWidget tf, GC gc)
{
  unsigned long valueMask = (GCForeground | GCBackground);
  XGCValues values;

  values.foreground = tf->primitive.top_shadow_color;
  values.background = tf->core.background_pixel;

  XChangeGC(XtDisplay(tf), gc, valueMask, &values);
}

static void
SetInvGC(XmTextFieldWidget tf,
	   GC gc)
{
  unsigned long valueMask = (GCForeground | GCBackground);
  XGCValues values;

  _XmTextFieldSetClipRect(tf);
  values.foreground = tf->core.background_pixel;
  values.background = tf->primitive.foreground;

  XChangeGC(XtDisplay(tf), gc, valueMask, &values);
}

static void DrawText(XmTextFieldWidget tf, GC gc, int x, int y, XmString string)
{
	XmStringDraw(XtDisplay(tf), XtWindow(tf), TextF_FontList(tf),
	             string, gc, x, y - TextF_FontAscent(tf), tf->core.width,
	             tf->text.alignment, XmLEFT_TO_RIGHT, NULL);
}

static int FindPixelLength(XmTextFieldWidget tf, XmString s)
{
	return (int)XmStringWidth(TextF_FontList(tf), s);
}

static void
DrawTextSegment(XmTextFieldWidget tf,
		XmHighlightMode mode,
		XmTextPosition prev_seg_start,
		XmTextPosition seg_start,
		XmTextPosition seg_end,
		XmTextPosition next_seg,
		Boolean stipple,
		int y,
		int *x)
{
  XmString s, prev;
  int x_seg_len;

  /* update x position up to start position */
  prev = XmStringSubstr(tf->text.xms_value, prev_seg_start, (size_t)seg_start - (size_t)prev_seg_start);
  s    = XmStringSubstr(tf->text.xms_value, seg_start, (size_t)seg_end - (size_t)seg_start);
  *x  += FindPixelLength(tf, prev);
  x_seg_len = FindPixelLength(tf, s);

  if (mode == XmHIGHLIGHT_SELECTED) {
    /* Draw the selected text using an inverse gc */
    SetNormGC(tf, tf->text.gc, False, False);
    XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.gc, *x,
		   y - TextF_FontAscent(tf), x_seg_len,
		   TextF_FontAscent(tf) + TextF_FontDescent(tf));
    SetInvGC(tf, tf->text.gc);
  } else {
    SetInvGC(tf, tf->text.gc);
    XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.gc, *x,
		   y - TextF_FontAscent(tf), x_seg_len,
		   TextF_FontAscent(tf) + TextF_FontDescent(tf));
    SetNormGC(tf, tf->text.gc, True, stipple);
  }

  if (stipple) {
    /*Draw shadow for insensitive text*/
    SetShadowGC(tf, tf->text.gc);
    DrawText(tf, tf->text.gc, *x+1, y+1, s);
    SetNormGC(tf, tf->text.gc, True, stipple);
  }

  DrawText(tf, tf->text.gc, *x, y, s);
  if (stipple) SetNormGC(tf, tf->text.gc, True, !stipple);

  if (mode == XmHIGHLIGHT_SECONDARY_SELECTED)
    XDrawLine(XtDisplay(tf), XtWindow(tf), tf->text.gc, *x, y,
	      *x + x_seg_len - 1, y);

  /* update x position up to the next highlight position */
  XmStringFree(s);
  XmStringFree(prev);
  s = XmStringSubstr(tf->text.xms_value, seg_start, (size_t)next_seg - (size_t)seg_start);
  *x += FindPixelLength(tf, s);
  XmStringFree(s);
}

/*
 * Redisplay the new adjustments that have been made the the text
 * field widget.
 */
static void
RedisplayText(XmTextFieldWidget tf,
	      XmTextPosition start,
	      XmTextPosition end)
{
  _XmHighlightRec *l = tf->text.highlight.list;
  XRectangle rect;
  int x, y, i;
  XmString s;
  Dimension margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Dimension margin_top = tf->text.margin_top + tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
  Dimension margin_bottom = tf->text.margin_bottom +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Boolean stipple = False;

  if (!XtIsRealized((Widget)tf)) return;

  if (tf->text.in_setvalues) {
    tf->text.redisplay = True;
    return;
  }

  if ((int)tf->core.width - (int)(2 * margin_width) <= 0)
    return;
  if ((int)tf->core.height - (int)(margin_top + margin_bottom) <= 0)
    return;

  _XmTextFieldDrawInsertionPoint(tf, False);

  /* Get the current rectangle.
   */
  GetRect(tf, &rect);

  x = (int) tf->text.h_offset;
  y = margin_top + TextF_FontAscent(tf);

  if (!XtIsSensitive((Widget)tf)) stipple = True;

  /* search through the highlight array and draw the text */
  for (i = 0; i + 1 < tf->text.highlight.number; i++) {

    /* make sure start is within current highlight */
    if (l[i].position <= start && start < l[i+1].position &&
	l[i].position < end) {

      if (end > l[i+1].position) {

	DrawTextSegment(tf, l[i].mode, l[i].position, start,
			l[i+1].position, l[i+1].position, stipple, y, &x);

	/* update start position to the next highlight position */
	start = l[i+1].position;

      } else {

 	if (start > end) {
	    XmTextPosition tmp = start;
	    start = end;
	    end = tmp;
 	}

	DrawTextSegment(tf, l[i].mode, l[i].position, start,
			end, l[i+1].position, stipple, y, &x);
	start = end;
      }
    } else { /* start not within current record */
      s = XmStringSubstr(tf->text.xms_value, l[i].position, (size_t)l[i+1].position - (size_t)l[i].position);
      x += FindPixelLength(tf, s);
      XmStringFree(s);
    }
  }  /* end for loop */

  /* complete the drawing of the text to the end of the line */
  if (l[i].position < end) {
    DrawTextSegment(tf, l[i].mode, l[i].position, start,
		    end, tf->text.string_length, stipple, y, &x);
  } else {
    s = XmStringSubstr(tf->text.xms_value, l[i].position, tf->text.string_length - (size_t)l[i].position);
    x += FindPixelLength(tf, s);
    XmStringFree(s);
  }

  if (x < (Position)(rect.x + rect.width)) {
    SetInvGC(tf, tf->text.gc);
    XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.gc, x, rect.y,
		   rect.x + rect.width - x, rect.height);
  }
  tf->text.refresh_ibeam_off = True;
  _XmTextFieldDrawInsertionPoint(tf, True);
}


/*
 * Use the font along with the resources that have been set
 * to determine the height and width of the text field widget.
 */
static void
ComputeSize(XmTextFieldWidget tf,
	    Dimension *width,
	    Dimension *height)
{
  XmString s;
  Dimension tmp = 0;

  if (TextF_ResizeWidth(tf) &&
      TextF_Columns(tf) < tf->text.string_length) {
    tmp = FindPixelLength(tf, tf->text.xms_value);

    *width = tmp + (2 * (TextF_MarginWidth(tf) +
			 tf->primitive.shadow_thickness +
			 tf->primitive.highlight_thickness));
  } else {
    *width = TextF_Columns(tf) * tf->text.average_char_width +
      2 * (TextF_MarginWidth(tf) + tf->primitive.shadow_thickness +
	   tf->primitive.highlight_thickness);
  }

  if (height != NULL)
    *height = TextF_FontDescent(tf) + TextF_FontAscent(tf) +
      2 * (TextF_MarginHeight(tf) + tf->primitive.shadow_thickness +
	   tf->primitive.highlight_thickness);
}

/*
 * TryResize - Attempts to resize the width of the text field widget.
 * If the attempt fails or is ineffective, return GeometryNo.
 */
static XtGeometryResult
TryResize(XmTextFieldWidget tf,
          Dimension width,
          Dimension height)
{
  Dimension reswidth, resheight;
  Dimension origwidth = tf->core.width;
  XtGeometryResult result;

  result = XtMakeResizeRequest((Widget)tf, width, height,
			       &reswidth, &resheight);

  if (result == XtGeometryAlmost) {
    result = XtMakeResizeRequest((Widget)tf, reswidth, resheight,
				 &reswidth, &resheight);

    if (reswidth == origwidth)
      result = XtGeometryNo;
    return result;
  }

  /*
   * Caution: Some geometry managers return XtGeometryYes
   *	        and don't change the widget's size.
   */
  if (tf->core.width != width || tf->core.height != height)
    result = XtGeometryNo;

  return result;
}


/*
 * Function AdjustText
 *
 * AdjustText ensures that the character at the given position is entirely
 * visible in the Text Field widget.  If the character is not already entirely
 * visible, AdjustText changes the Widget's h_offset appropriately.  If
 * the text must be redrawn, AdjustText calls RedisplayText.
 *
 */
static Boolean
AdjustText(XmTextFieldWidget tf,
	   XmTextPosition position,
           Boolean flag)
{
  int left_edge = 0;
  int diff;
  XmString s;
  Dimension margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Dimension thickness    = 2 * (tf->primitive.shadow_thickness +
				tf->primitive.highlight_thickness);
  Dimension temp;


  s = XmStringSubstr(tf->text.xms_value, 0, (size_t)position);
  left_edge = FindPixelLength(tf, s) + tf->text.h_offset;
  XmStringFree(s);

  if (left_edge <= (int)margin_width && position == tf->text.string_length) {
    position = MAX(position - TextF_Columns(tf)/2, 0);
    s = XmStringSubstr(tf->text.xms_value, 0, (size_t)position);
    left_edge = FindPixelLength(tf, s) + tf->text.h_offset;
    XmStringFree(s);
  }

  if ((diff = left_edge - margin_width) < 0) {
    /* We need to scroll the string to the right. */
    if (!XtIsRealized((Widget)tf)) {
      tf->text.h_offset -= diff;
      return True;
    }
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.h_offset -= diff;
    SetInvGC(tf, tf->text.gc);
    SetFullGC(tf, tf->text.gc);
    if (tf->core.height <= thickness)
      temp = 0;
    else
      temp = tf->core.height - thickness;
    XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.gc,
		   tf->primitive.shadow_thickness +
		   tf->primitive.highlight_thickness,
		   tf->primitive.shadow_thickness +
		   tf->primitive.highlight_thickness,
		   TextF_MarginWidth(tf),
		   temp);
    SetMarginGC(tf, tf->text.gc);
    RedisplayText(tf, 0, tf->text.string_length);
    _XmTextFieldDrawInsertionPoint(tf, True);
    return True;
  } else if ((diff = (left_edge -
		      (int)(tf->core.width - margin_width))) > 0) {
    /* We need to scroll the string to the left. */
    if (!XtIsRealized((Widget)tf)) {
      tf->text.h_offset -= diff;
      return True;
    }
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.h_offset -= diff;
    SetInvGC(tf, tf->text.gc);
    SetFullGC(tf, tf->text.gc);
    if (tf->core.width <= thickness)
      temp = 0;
    else
      temp = tf->core.width - thickness;
    XFillRectangle(XtDisplay(tf), XtWindow(tf), tf->text.gc,
		   tf->core.width - margin_width,
		   tf->primitive.shadow_thickness +
		   tf->primitive.highlight_thickness,
		   TextF_MarginWidth(tf),
		   temp);
    SetMarginGC(tf, tf->text.gc);
    RedisplayText(tf, 0, tf->text.string_length);
    _XmTextFieldDrawInsertionPoint(tf, True);
    return True;
  }

  if (flag) RedisplayText(tf, position, tf->text.string_length);

  return False;
}

/*
 * AdjustSize
 *
 * Adjust size will resize the text to ensure that all the text is visible.
 * It will also adjust text that is shrunk.  Shrinkage is limited to the
 * size determined by the XmNcolumns resource.
 */
static void
AdjustSize(XmTextFieldWidget tf)
{
  XtGeometryResult result = XtGeometryYes;
  int left_edge = 0;
  int diff;
  Boolean redisplay = False;
  Dimension margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  left_edge = FindPixelLength(tf, tf->text.xms_value) + margin_width;
  if ((diff = (left_edge - (tf->core.width - (margin_width)))) > 0) {
    if (tf->text.in_setvalues) {
      tf->core.width += diff;
      return;
    }
    /* Attempt to resize.  If it doesn't succeed, do scrolling.  */
    result = TryResize(tf, tf->core.width + diff, tf->core.height);
    if (result == XtGeometryYes) {
      XtWidgetProc resize;

      _XmProcessLock();
      resize = tf->core.widget_class->core_class.resize;
      _XmProcessUnlock();
      (*resize)((Widget)tf);
      return;
    } else {
      /* We need to scroll the string to the left. */
      tf->text.h_offset = tf->text.alignment == XmALIGNMENT_END ? diff : margin_width - diff;
    }
  } else {
    Dimension width;

    /* If the new size is smaller than core size, we need
     * to shrink.  Note: new size will never be less than the
     * width determined by the columns resource.
     */
    ComputeSize(tf, &width, NULL);
    if (width < tf->core.width) {
      if (tf->text.in_setvalues) {
	tf->core.width = width;
	return;
      }
      result = TryResize(tf, width, tf->core.height);
      if (result == XtGeometryYes) {
	XtWidgetProc resize;

	_XmProcessLock();
	resize = tf->core.widget_class->core_class.resize;
	_XmProcessUnlock();
	(*resize)((Widget)tf);

	return;
      }
    }
  }

  redisplay = AdjustText(tf, TextF_CursorPosition(tf), False);

  if (!redisplay)
    RedisplayText(tf, 0, tf->text.string_length);
}

static Boolean ModifyVerify(XmTextFieldWidget tf, XEvent *event,
                            XmTextPosition *replace_prev,
                            XmTextPosition *replace_next,
                            XmString *insert,
                            size_t *insert_length,
                            XmTextPosition *newInsert)
{
	XmString str;
	XmTextPosition curr_insert;
	XmTextVerifyCallbackStruct vcb;
	XmTextVerifyCallbackStructWcs wcs_vcb;
	XmTextVerifyCallbackStructStr vcb_str;
	XmTextBlockRec newblock;
	XmTextBlockRecWcs wcs_newblock;
	char *cptr;
	wchar_t *wptr;

	/* if there are no callbacks, don't waste any time... just return True */
	if (!TextF_ModifyVerifyCallbackStr(tf) &&
	    !TextF_ModifyVerifyCallback(tf)    &&
	    !TextF_ModifyVerifyCallbackWcs(tf))
		return True;

	vcb_str.doit = True;
	curr_insert = *newInsert = TextF_CursorPosition(tf);
	if (TextF_ModifyVerifyCallbackStr(tf)) {
		str                = XmStringCopy(*insert);
		vcb_str.reason     = XmCR_MODIFYING_TEXT_VALUE;
		vcb_str.event      = event;
		vcb_str.currInsert = curr_insert;
		vcb_str.newInsert  = *newInsert;
		vcb_str.startPos   = *replace_prev;
		vcb_str.endPos     = *replace_next;
		vcb_str.str        = str;
		XtCallCallbackList((Widget)tf, TextF_ModifyVerifyCallbackStr(tf), (XtPointer)&vcb_str);
		XmStringFree(str);

		if (vcb_str.doit) {
			if (vcb_str.str != *insert) {
				*insert        = vcb_str.str;
				*insert_length = XmStringLen(vcb_str.str);
			}

			*replace_prev = vcb_str.startPos;
			*replace_next = vcb_str.endPos;
			*newInsert    = vcb_str.newInsert;
			curr_insert   = vcb_str.currInsert;
		}
	}

	vcb.doit = True;
	if (TextF_ModifyVerifyCallback(tf) && vcb_str.doit) {
		newblock.format = XmFMT_8_BIT;
		newblock.length = *insert_length;
		cptr = newblock.ptr = XtCalloc(*insert_length + 1, 1);

		if (newblock.length) {
			cptr = XmStringUngenerate(*insert, NULL, XmUTF8_TEXT, XmCHARSET_TEXT);
			memcpy(newblock.ptr, cptr, newblock.length);
			XtFree(cptr);
			cptr = newblock.ptr;
		}

		vcb.reason     = XmCR_MODIFYING_TEXT_VALUE;
		vcb.event      = event;
		vcb.currInsert = curr_insert;
		vcb.newInsert  = *newInsert;
		vcb.text       = &newblock;
		vcb.startPos   = *replace_prev;
		vcb.endPos     = *replace_next;
		XtCallCallbackList((Widget) tf, TextF_ModifyVerifyCallback(tf), (XtPointer)&vcb);
		XtFree(cptr);

		if (vcb.doit) {
			if (newblock.ptr != cptr ||
			    newblock.length != *insert_length) {
				XmStringFree(*insert);
				newblock.ptr[newblock.length] = '\0';
				*insert        = XmStringGenerate(newblock.ptr, NULL, XmCHARSET_TEXT, NULL);
				*insert_length = XmStringLen(*insert);
			}

			*replace_prev = vcb.startPos;
			*replace_next = vcb.endPos;
			*newInsert    = vcb.newInsert;
			curr_insert   = vcb.currInsert;
		}
	}

	wcs_vcb.doit = True;
	if (TextF_ModifyVerifyCallbackWcs(tf) && vcb_str.doit && vcb.doit) {
		wcs_newblock.length        = (int)*insert_length;
		wptr = wcs_newblock.wcsptr = (wchar_t *)XtCalloc(wcs_newblock.length + 1, sizeof(wchar_t));

		if (wcs_newblock.length) {
			wptr = XmStringUngenerate(*insert, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT);
			memcpy(wcs_newblock.wcsptr, wptr, wcs_newblock.length * sizeof(wchar_t));
			XtFree((XtPointer)wptr);
			wptr = wcs_newblock.wcsptr;
		}

		wcs_vcb.reason     = XmCR_MODIFYING_TEXT_VALUE;
		wcs_vcb.event      = event;
		wcs_vcb.currInsert = curr_insert;
		wcs_vcb.newInsert  = *newInsert;
		wcs_vcb.text       = &wcs_newblock;
		wcs_vcb.startPos   = *replace_prev;
		wcs_vcb.endPos     = *replace_next;
		XtCallCallbackList((Widget) tf, TextF_ModifyVerifyCallbackWcs(tf), (XtPointer)&wcs_vcb);
		XtFree((XtPointer)wptr);

		if (wcs_vcb.doit) {
			if (wcs_newblock.wcsptr != wptr || *insert_length != (size_t)wcs_newblock.length) {
				XmStringFree(*insert);
				wcs_newblock.wcsptr[wcs_newblock.length] = 0;
				*insert        = XmStringCreateWide(wcs_newblock.wcsptr, NULL);
				*insert_length = XmStringLen(*insert);
			}

			*replace_prev = wcs_vcb.startPos;
			*replace_next = wcs_vcb.endPos;
			*newInsert    = wcs_vcb.newInsert;
		}
	}

	/* Disallow the modification if any callback indicated so */
	return vcb_str.doit && wcs_vcb.doit && vcb.doit;
}

static void
ResetClipOrigin(XmTextFieldWidget tf)
{
  int x, y;
  Position x_pos, y_pos;

  (void) GetXYFromPos(tf, TextF_CursorPosition(tf), &x_pos, &y_pos);

  if (!XtIsRealized((Widget)tf)) return;

  x = (int) x_pos; y = (int) y_pos;

  x -=(tf->text.cursor_width >> 1) + 1;

  y = (y + TextF_FontDescent(tf)) - tf->text.cursor_height;

  XSetTSOrigin(XtDisplay(tf), tf->text.image_gc, x, y);
}

static void
InvertImageGC (XmTextFieldWidget tf)
{
  if (tf->text.have_inverted_image_gc) return;

  tf->text.have_inverted_image_gc = True;
}

static void
ResetImageGC (XmTextFieldWidget tf)
{
  if (!tf->text.have_inverted_image_gc) return;

  tf->text.have_inverted_image_gc = False;
}



/*
 * Calls the motion verify callback.  If the doit flag is true,
 * then reset the cursor_position and call AdjustText() to
 * move the text if need be.
 */

void
_XmTextFieldSetCursorPosition(XmTextFieldWidget tf,
			      XEvent *event,
			      XmTextPosition position,
                              Boolean adjust_flag,
                              Boolean call_cb)
{
  SetCursorPosition(tf, event, position, adjust_flag, call_cb, True,DontCare);
}

static void
SetCursorPosition(XmTextFieldWidget tf,
		  XEvent *event,
		  XmTextPosition position,
                  Boolean adjust_flag,
                  Boolean call_cb,
                  Boolean set_dest,
		  PassDisown passDisown)
{
  XmTextVerifyCallbackStruct cb;
  Boolean flag = False;
  XPoint xmim_point;
  XRectangle xmim_area;
  _XmHighlightRec *hl_list = tf->text.highlight.list;
  int i;

  if (position < 0) position = 0;

  if (position > (XmTextPosition)tf->text.string_length)
    position = (XmTextPosition)tf->text.string_length;

  if (TextF_CursorPosition(tf) != position && call_cb) {
    /* Call Motion Verify Callback before Cursor Changes Positon */
    cb.reason = XmCR_MOVING_INSERT_CURSOR;
    cb.event  = event;
    cb.currInsert = TextF_CursorPosition(tf);
    cb.newInsert = position;
    cb.doit = True;
    XtCallCallbackList((Widget) tf, TextF_MotionVerifyCallback(tf),
		       (XtPointer) &cb);

    if (!cb.doit) {
      if (tf->text.verify_bell) XBell(XtDisplay((Widget)tf), 0);
      return;
    }
  }
  _XmTextFieldDrawInsertionPoint(tf, False);

  TextF_CursorPosition(tf) = position;

  if (!tf->text.add_mode && tf->text.pending_off && tf->text.has_primary) {
    SetSelection(tf, position, position, True);
    flag = True;
  }

  /* Deterimine if we need an inverted image GC or not.  Get the highlight
   * record for the cursor position.  If position is on a boundary of
   * a highlight, then we always display cursor in normal mode (i.e. set
   * normal image GC).  If position is within a selected highlight rec,
   * then make sure the image GC is inverted.  If we've moved out of a
   * selected highlight region, restore the normal image GC. */

  for (i = tf->text.highlight.number - 1; i >= 0; i--) {
    if (position >= hl_list[i].position || i == 0)
      break;
  }

  if (position == hl_list[i].position)
    ResetImageGC(tf);
  else if (hl_list[i].mode != XmHIGHLIGHT_SELECTED)
    ResetImageGC(tf);
  else
    InvertImageGC(tf);

  if (adjust_flag) (void) AdjustText(tf, position, flag);

  tf->text.refresh_ibeam_off = True;
  _XmTextFieldDrawInsertionPoint(tf, True);

  (void) GetXYFromPos(tf, TextF_CursorPosition(tf),
		      &xmim_point.x, &xmim_point.y);
  (void)TextFieldGetDisplayRect((Widget)tf, &xmim_area);
  XmImVaSetValues((Widget)tf, XmNspotLocation, &xmim_point,
		  XmNarea, &xmim_area, NULL);

  if (set_dest)
    (void) SetDestination((Widget) tf, TextF_CursorPosition(tf),
		(DontCare == passDisown) ? False : True,
			  XtLastTimestampProcessed(XtDisplay((Widget)tf)));
}


/*
 * This routine is used to verify that the positions are within the bounds
 * of the current TextField widgets value.  Also, it ensures that left is
 * less than right.
 */
static void
VerifyBounds(XmTextFieldWidget tf,
	     XmTextPosition *from,
	     XmTextPosition *to)
{
  XmTextPosition tmp;

  if (*from < 0)
    *from = 0;
  else if (*from > tf->text.string_length) {
    *from = tf->text.string_length;
  }
  if (*to < 0)
    *to = 0;
  else if (*to > tf->text.string_length) {
    *to = tf->text.string_length;
  }
  if (*from > *to) {
    tmp = *to;
    *to = *from;
    *from = tmp;
  }
}

/*
 * Function _XmTextFieldReplaceText
 *
 * _XmTextFieldReplaceText is a utility function for the text-modifying
 * action procedures below (InsertChar, DeletePrevChar, and so on).
 * _XmTextFieldReplaceText does the real work of editing the string,
 * including:
 *
 *   (1) invoking the modify verify callbacks,
 *   (2) allocating more memory for the string if necessary,
 *   (3) doing the string manipulation,
 *   (4) moving the selection (the insertion point), and
 *   (5) redrawing the text.
 *
 * Though the procedure claims to take a char* argument, MB_CUR_MAX determines
 * what the different routines will actually pass to it.  If MB_CUR_MAX is
 * greater than 1, then "insert" points to wchar_t data and we must set up
 * the appropriate cast.  In all cases, insert_length is the number of
 * characters (not bytes) to be inserted.
 */
Boolean _XmTextFieldReplaceText(XmTextFieldWidget tf, XEvent *event,
                                XmTextPosition replace_prev,
                                XmTextPosition replace_next,
                                XmString insert,
                                size_t insert_length,
                                Boolean move_cursor,
                                Boolean modify_verify)
{
  int replace_length, low = 0, high = 0;
  int delta = 0;
  XmTextPosition cursorPos, newInsert;
  XmTextPosition old_pos = replace_prev;
  XmString repl, insert_orig = insert;
  size_t insert_length_orig = XmStringLen(insert);

  VerifyBounds(tf, &replace_prev, &replace_next);
  if (!TextF_Editable(tf)) {
    if (tf->text.verify_bell)
      XBell(XtDisplayOfObject((Widget)tf), 0);
    XmStringFree(insert);
    return False;
  }

  if (tf->text.programmatic_highlights)
  {
  	/*
	** XmTextFieldSetHighlight called since last interaction here
	** that resulted in clearing program-set highlights
	*/
	if (TrimHighlights(tf, &low, &high))
		{
	    	RedisplayText(tf, low, high);
		tf->text.programmatic_highlights = False;
		}
  }

  replace_length = (int) (replace_next - replace_prev);
  delta = insert_length - replace_length;

  /* Disallow insertions that go beyond max length boundries. */
  if ((delta >= 0) && !FUnderVerifyPreedit(tf) &&
      ((tf->text.string_length + delta) > (size_t)TextF_MaxLength(tf))) {
    if (tf->text.verify_bell)
      XBell(XtDisplayOfObject((Widget)tf), 0);
    XmStringFree(insert);
    return False;
  }

  /* If there are modify verify callbacks, verify that we want to continue
   * the action.
   */
  newInsert = TextF_CursorPosition(tf);
  if (modify_verify &&
      !ModifyVerify(tf, event, &replace_prev, &replace_next, &insert,
                    &insert_length, &newInsert)) {
    if (tf->text.verify_bell)
      XBell(XtDisplayOfObject((Widget)tf), 0);
    if (FUnderVerifyPreedit(tf)) {
      FVerifyCommitNeeded(tf) = True;
      PreEnd(tf) -= (int)insert_length_orig;
    }

    XmStringFree(insert);
    return False;
  }

  if (FUnderVerifyPreedit(tf)) {
    if (insert_length != insert_length_orig || insert != insert_orig) {
      FVerifyCommitNeeded(tf) = True;
      PreEnd(tf) += insert_length - insert_length_orig;
    }
  }

  if (insert != insert_orig)
    XmStringFree(insert_orig);
  VerifyBounds(tf, &replace_prev, &replace_next);
  replace_length = (int)(replace_next - replace_prev);
  delta = insert_length - replace_length;

  /* Disallow insertions that go beyond max length boundries. */
  if (delta >= 0 && !FUnderVerifyPreedit(tf) &&
      ((tf->text.string_length + delta) > (size_t)TextF_MaxLength(tf))) {
    if (tf->text.verify_bell)
      XBell(XtDisplayOfObject((Widget)tf), 0);
    XmStringFree(insert);
    return False;
  }

  /* make sure selections are turned off prior to changing text */
  if (tf->text.has_primary &&
      tf->text.prim_pos_left != tf->text.prim_pos_right)
    doSetHighlight((Widget)tf, tf->text.prim_pos_left,
			    tf->text.prim_pos_right, XmHIGHLIGHT_NORMAL);

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (tf->text.has_primary && replace_prev < tf->text.prim_pos_right &&
      replace_next > tf->text.prim_pos_left) {
    if (replace_prev <= tf->text.prim_pos_left) {
      if (replace_next < tf->text.prim_pos_right) {
	/* delete encompasses left half of the selection
	 * so move left endpoint
	 */
	tf->text.prim_pos_left = replace_next;
      } else {
	/* delete encompasses the selection so set selection to NULL */
           tf->text.prim_pos_left = tf->text.prim_pos_right;
        }
       } else {
        /*
         * the 2 cases below were incorrect, and have now
         * been synchronized with the corresponding block of code in
         * TextStrSo.c
         */
        if (replace_next > tf->text.prim_pos_right) {
          /* delete encompasses the right half of the selection
           * so move right endpoint
           */
           tf->text.prim_pos_right = replace_prev;
        } else {
          /* delete is completely within the selection
           * so decrease size of selection.
           */
           tf->text.prim_pos_right -= (replace_next - replace_prev);
        }

       }
    }

  /* Do the actual string replacement */
  if (replace_next == replace_prev)
    repl = XmStringInsert(tf->text.xms_value, replace_next, insert);
  else repl = XmStringReplace(tf->text.xms_value, replace_prev, replace_next - replace_prev, insert);

  XmStringFree(tf->text.xms_value);
  XmStringFree(insert);
  tf->text.xms_value     = repl;
  tf->text.string_length += delta;

  if (tf->text.has_primary &&
      tf->text.prim_pos_left != tf->text.prim_pos_right) {
    if (replace_prev <= tf->text.prim_pos_left) {
      tf->text.prim_pos_left += delta;
      tf->text.prim_pos_right += delta;
    }
    if (tf->text.prim_pos_left > tf->text.prim_pos_right)
      tf->text.prim_pos_right = tf->text.prim_pos_left;
  }

  /* make sure the selection are redisplay,
     since they were turned off earlier */
  if (tf->text.has_primary &&
      tf->text.prim_pos_left != tf->text.prim_pos_right)
    doSetHighlight((Widget)tf, tf->text.prim_pos_left,
			    tf->text.prim_pos_right, XmHIGHLIGHT_SELECTED);

  if (move_cursor) {
    if (TextF_CursorPosition(tf) != newInsert) {
      if (newInsert > (XmTextPosition)tf->text.string_length) {
	cursorPos = (XmTextPosition)tf->text.string_length;
      } else if (newInsert < 0) {
	cursorPos = 0;
      } else {
	cursorPos = newInsert;
      }
    } else
      cursorPos = replace_next + (insert_length - replace_length);
    if (event != NULL) {
      SetDestination((Widget)tf, cursorPos, False, event->xkey.time);
    } else {
      SetDestination((Widget)tf, cursorPos, False,
                     XtLastTimestampProcessed(XtDisplayOfObject((Widget)tf)));
    }
    _XmTextFieldSetCursorPosition(tf, event, cursorPos, False, True);
  }

  if (TextF_ResizeWidth(tf) && tf->text.do_resize) {
    AdjustSize(tf);
  } else {
    AdjustText(tf, TextF_CursorPosition(tf), False);
    RedisplayText(tf, old_pos, tf->text.string_length);
  }

  _XmTextFieldDrawInsertionPoint(tf, True);
  return True;
}

/*
 * Reset selection flag and selection positions and then display
 * the new settings.
 */
void
_XmTextFieldDeselectSelection(Widget w,
			      Boolean disown,
			      Time sel_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (w != NULL && disown) {
    if (!sel_time) sel_time = _XmValidTimestamp(w);
    /*
     * Disown the primary selection (This function is a no-op if
     * this widget doesn't own the primary selection)
     */
    XtDisownSelection(w, XA_PRIMARY, sel_time);
  }
  if (tf != NULL) {
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.has_primary = False;
    tf->text.take_primary = True;
    TextFieldSetHighlight(tf, tf->text.prim_pos_left,
			  tf->text.prim_pos_right, XmHIGHLIGHT_NORMAL);
    tf->text.prim_pos_left = tf->text.prim_pos_right =
      tf->text.prim_anchor = TextF_CursorPosition(tf);

    if (!tf->text.has_focus && tf->text.add_mode)
      tf->text.add_mode = False;

    RedisplayText(tf, 0, tf->text.string_length);

    _XmTextFieldDrawInsertionPoint(tf, True);
  }
}

/*
 * Finds the cursor position from the given X value.
 */
static XmTextPosition
GetPosFromX(XmTextFieldWidget tf,
            Position x)
{
  XmString s;
  XmTextPosition position;
  int temp_x = 0;
  int next_char_width = 0;

  /* Decompose the x to equal the length of the text string */
  temp_x += (int) tf->text.h_offset;

  /* Next width is an offset allowing button presses on the left side
   * of a character to select that character, while button presses
   * on the rigth side of the character select the  NEXT character.
   */

  if (tf->text.string_length > 0) {
    s = XmStringSubstr(tf->text.xms_value, 0, 1);
    next_char_width = FindPixelLength(tf, s);
    XmStringFree(s);
  }

  for (position = 0; temp_x + next_char_width/2 < (int) x &&
       position < tf->text.string_length; position++) {

    temp_x+=next_char_width;    /*
				 * We still haven't reached the x pos.
				 * Add the width and find the next chars
				 * width.
				 */

    /*
     * If there is a next position, find its width.  Otherwise, use the
     * current "next" width.
     */

    if (tf->text.string_length > position + 1) {
      s = XmStringSubstr(tf->text.xms_value, position + 1, 1);
      next_char_width = FindPixelLength(tf, s);
      XmStringFree(s);
    }
  } /* for */

  return position;
}

static Boolean
SetDestination(Widget w,
	       XmTextPosition position,
	       Boolean disown,
	       Time set_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean result = TRUE;
  Atom MOTIF_DESTINATION = XInternAtom(XtDisplay(w),
				       XmS_MOTIF_DESTINATION, False);

  if (!XtIsRealized(w)) return False;

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (!disown) {
    if (!tf->text.has_destination) {
      if (!set_time) set_time = _XmValidTimestamp(w);
      result = XmeSecondarySink(w, set_time);
      tf->text.dest_time = set_time;
      tf->text.has_destination = result;

      if (result) _XmSetDestination(XtDisplay(w), w);
    }
  } else {
    if (tf->text.has_destination) {
      if (!set_time) set_time = _XmValidTimestamp(w);
      XtDisownSelection(w, MOTIF_DESTINATION, set_time);

      /* Call XmGetDestination(dpy) to get widget that last had
	 destination cursor. */
      if (w == XmGetDestination(XtDisplay(w)))
	_XmSetDestination(XtDisplay(w), (Widget)NULL);

      tf->text.has_destination = False;
    }
  }

  _XmTextFieldDrawInsertionPoint(tf, True);

  return result;
}

Boolean
_XmTextFieldSetDestination(Widget w,
			   XmTextPosition position,
			   Time set_time)
{
  Boolean result;

  result = SetDestination(w, position, False, set_time);

  return result;
}


/*
 * Calls the losing focus verify callback to verify that the application
 * want to traverse out of the text field widget.  Returns the result.
 */
static Boolean
VerifyLeave(XmTextFieldWidget tf,
	    XEvent *event)
{
  XmTextVerifyCallbackStruct  cbdata;

  cbdata.reason = XmCR_LOSING_FOCUS;
  cbdata.event = event;
  cbdata.doit = True;
  cbdata.currInsert = TextF_CursorPosition(tf);
  cbdata.newInsert = TextF_CursorPosition(tf);
  cbdata.startPos = TextF_CursorPosition(tf);
  cbdata.endPos = TextF_CursorPosition(tf);
  cbdata.text = NULL;
  XtCallCallbackList((Widget) tf, TextF_LosingFocusCallback(tf),
		     (XtPointer) &cbdata);
  tf->text.take_primary = True;
  return(cbdata.doit);
}

/**
 * This routine is used to determine if two adjacent characters
 * constitute a word boundary
 */
static Boolean _XmTextFieldIsWordBoundary(XmTextFieldWidget tf,
                                          XmTextPosition pos1,
                                          XmTextPosition pos2)
{
	XmCodepoint c1, c2;

	/* if positions aren't adjacent, return False */
	if (pos1 < pos2 && ((pos2 - pos1) != 1))      return False;
	else if (pos2 < pos1 && ((pos1 - pos2) != 1)) return False;

	/* Use space as the sole word boundary for now */
	if (XmStringCodepointAt(tf->text.xms_value, pos1) == ' ' ||
	    XmStringCodepointAt(tf->text.xms_value, pos2) == ' ')
		return True;

	return False;
}

static void FindWord(XmTextFieldWidget tf, XmTextPosition begin,
                     XmTextPosition *left, XmTextPosition *right)
{
	XmTextPosition start, end;

	for (start = begin; start > 0; start--) {
		if (_XmTextFieldIsWordBoundary(tf, start - 1, start))
			break;
	}

	*left = start;
	for (end = begin; end <= (XmTextPosition)tf->text.string_length; end++) {
		if (end < (XmTextPosition)tf->text.string_length) {
			if (_XmTextFieldIsWordBoundary(tf, end, end + 1)) {
				end += 2; /* want to return position of next word; end + 1 */
				break;    /* is that position && *right = end - 1... */
			}
		}
	}

	*right = end - 1;
}

static void FindPrevWord(XmTextFieldWidget tf, XmTextPosition *left, XmTextPosition *right)
{
	XmTextPosition start = TextF_CursorPosition(tf);

	if (start > 0 && _XmTextFieldIsWordBoundary(tf, start - 1, start)) {
		for (; start > 0; start--) {
			if (!_XmTextFieldIsWordBoundary(tf, start - 1, start)) {
				start--;
				break;
			}
		}
	}

	FindWord(tf, start, left, right);
}

static void FindNextWord(XmTextFieldWidget tf, XmTextPosition *left, XmTextPosition *right)
{
	XmTextPosition end = TextF_CursorPosition(tf);

	if (_XmTextFieldIsWordBoundary(tf, end - 1, end)) {
		for (end = TextF_CursorPosition(tf); end < (XmTextPosition)tf->text.string_length; end++) {
			if (!_XmTextFieldIsWordBoundary(tf, end - 1, end))
				break;
		}
	}

	FindWord(tf, end, left, right);

	/* Set right to the last word boundary following the current word */
	while (*right < (XmTextPosition)tf->text.string_length && _XmTextFieldIsWordBoundary(tf, *right - 1, *right))
		*right += 1;

	if (*right < (XmTextPosition)tf->text.string_length)
		*right -= 1;
}

static void
CheckDisjointSelection(Widget w,
		       XmTextPosition position,
		       Time sel_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;

  if (tf->text.add_mode ||
      (tf->text.has_primary && left != right &&
       position >= left && position <= right))
    tf->text.pending_off = FALSE;
  else
    tf->text.pending_off = TRUE;

  if (left == right) {
    (void) SetDestination(w, position, False, sel_time);
    tf->text.prim_anchor = position;
  } else {
    (void) SetDestination(w, position, False, sel_time);
    if (!tf->text.add_mode) tf->text.prim_anchor = position;
  }
}

static Boolean
NeedsPendingDelete(XmTextFieldWidget tf)
{
  return (tf->text.add_mode ?
	  (TextF_PendingDelete(tf) &&
	   tf->text.has_primary &&
	   tf->text.prim_pos_left != tf->text.prim_pos_right &&
	   tf->text.prim_pos_left <= TextF_CursorPosition(tf) &&
	   tf->text.prim_pos_right >= TextF_CursorPosition(tf)) :
	  (tf->text.has_primary &&
	   tf->text.prim_pos_left != tf->text.prim_pos_right));
}

static Boolean
NeedsPendingDeleteDisjoint(XmTextFieldWidget tf)
{
  return (TextF_PendingDelete(tf) &&
	  tf->text.has_primary &&
	  tf->text.prim_pos_left != tf->text.prim_pos_right &&
	  tf->text.prim_pos_left <= TextF_CursorPosition(tf) &&
	  tf->text.prim_pos_right >= TextF_CursorPosition(tf));
}


/****************************************************************
 *
 * Input functions defined in the action table.
 *
 ****************************************************************/

static void
InsertChar(Widget w,
	   XEvent *event,
	   char **params,
	   Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmString insert;
  char insert_string[TEXT_MAX_INSERT_SIZE + 1]; /* NULL-terminated below */
  XmTextPosition cursorPos =0 , nextPos = 0;
  int insert_length, i;
  int num_chars;
  Boolean replace_res;
  Boolean pending_delete = False;
  Status status_return;
  XmAnyCallbackStruct cb;

  /* Determine what was pressed. */
  insert_length = XmImUtf8LookupString(w, (XKeyEvent *)event, insert_string,
                                       TEXT_MAX_INSERT_SIZE, NULL, &status_return);

  if (insert_length > 0 && !TextF_Editable(tf)) {
    if (tf->text.verify_bell) XBell(XtDisplay((Widget)tf), 0);
    return;
  }

  /* If there is more data than we can handle, bail out */
  if (status_return == XBufferOverflow || insert_length > TEXT_MAX_INSERT_SIZE)
    return;

  /* *LookupString in some cases can return the NULL as a character, such
   * as when the user types <Ctrl><back_quote> or <Ctrl><@>.  Text widget
   * can't handle the NULL as a character, so we dump it here.
   */

  for (i=0; i < insert_length; i++)
    if (insert_string[i] == 0) insert_length = 0; /* toss out input string */

  if (insert_length > 0) {
    _XmTextFieldDrawInsertionPoint(tf, False);
    if (NeedsPendingDeleteDisjoint(tf)) {
      if (!tf->text.has_primary ||
	  (cursorPos = tf->text.prim_pos_left) ==
	  (nextPos = tf->text.prim_pos_right)) {
	tf->text.prim_anchor = TextF_CursorPosition(tf);
      }
      pending_delete = True;

      tf->text.prim_anchor = TextF_CursorPosition(tf);

    } else {
      cursorPos = nextPos = TextF_CursorPosition(tf);
    }

    insert = XmStringCreate(insert_string, "UTF-8");
    if (tf->text.overstrike) nextPos += insert_length;
    if (nextPos > tf->text.string_length) nextPos = tf->text.string_length;
    replace_res = _XmTextFieldReplaceText(tf, (XEvent *) event, cursorPos,
                                          nextPos, insert,
                                          insert_length, True, True);

    if (replace_res) {
      if (pending_delete) {
	_XmTextFieldStartSelection(tf, TextF_CursorPosition(tf),
				   TextF_CursorPosition(tf), event->xkey.time);
	tf->text.pending_off = False;
      }
      CheckDisjointSelection(w, TextF_CursorPosition(tf),
			     event->xkey.time);
      _XmTextFieldSetCursorPosition(tf, event, TextF_CursorPosition(tf),
				    False, True);
      cb.reason = XmCR_VALUE_CHANGED;
      cb.event = event;
      XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			 (XtPointer) &cb);
    }
    _XmTextFieldDrawInsertionPoint(tf, True);
  }
}

static void
DeletePrevChar(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else {
    if (tf->text.has_primary &&
	tf->text.prim_pos_left != tf->text.prim_pos_right) {
      if (TextF_CursorPosition(tf) - 1 >= 0)
	if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf) - 1,
				    TextF_CursorPosition(tf), NULL, 0, True, True)) {
	  CheckDisjointSelection(w, TextF_CursorPosition(tf),
				 event->xkey.time);
	  _XmTextFieldSetCursorPosition(tf, event,
					TextF_CursorPosition(tf),
					False, True);
	  cb.reason = XmCR_VALUE_CHANGED;
	  cb.event = event;
	  XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			     (XtPointer) &cb);
	}
    } else if (TextF_CursorPosition(tf) - 1 >= 0) {
      if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf) - 1,
				  TextF_CursorPosition(tf), NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event, TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
    }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DeleteNextChar(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else {
    if (tf->text.has_primary &&
	tf->text.prim_pos_left != tf->text.prim_pos_right) {
      if (TextF_CursorPosition(tf) < tf->text.string_length)
	if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf),
				    TextF_CursorPosition(tf) + 1, NULL, 0, True, True)) {
	  CheckDisjointSelection(w, TextF_CursorPosition(tf),
				 event->xkey.time);
	  _XmTextFieldSetCursorPosition(tf, event,
					TextF_CursorPosition(tf),
					False, True);
	  cb.reason = XmCR_VALUE_CHANGED;
	  cb.event = event;
	  XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			     (XtPointer) &cb);
	}
    } else if (TextF_CursorPosition(tf) < tf->text.string_length)
      if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf),
				  TextF_CursorPosition(tf) + 1, NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event,
				      TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DeletePrevWord(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left, right;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else {
    FindPrevWord(tf, &left, &right);
    if (tf->text.has_primary &&
	tf->text.prim_pos_left != tf->text.prim_pos_right) {
      if (_XmTextFieldReplaceText(tf, event, left, TextF_CursorPosition(tf),
				  NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event,
				      TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
    } else if (TextF_CursorPosition(tf) - 1 >= 0)
      if (_XmTextFieldReplaceText(tf, event, left, TextF_CursorPosition(tf),
				  NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event,
				      TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DeleteNextWord(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left, right;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else {
    FindNextWord(tf, &left, &right);
    if (tf->text.has_primary &&
	tf->text.prim_pos_left != tf->text.prim_pos_right) {
      if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf),
				  right, NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event,
				      TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
    } else if (TextF_CursorPosition(tf) < tf->text.string_length)
      if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf),
				  right, NULL, 0, True, True)) {
	CheckDisjointSelection(w, TextF_CursorPosition(tf),
			       event->xkey.time);
	_XmTextFieldSetCursorPosition(tf, event,
				      TextF_CursorPosition(tf),
				      False, True);
	cb.reason = XmCR_VALUE_CHANGED;
	cb.event = event;
	XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			   (XtPointer) &cb);
      }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}


static void
DeleteToEndOfLine(Widget w,
		  XEvent *event,
		  char **params,
		  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else if (TextF_CursorPosition(tf) < tf->text.string_length) {
    if (_XmTextFieldReplaceText(tf, event, TextF_CursorPosition(tf),
				tf->text.string_length, NULL, 0, True, True)) {
      CheckDisjointSelection(w, TextF_CursorPosition(tf),
			     event->xkey.time);
      _XmTextFieldSetCursorPosition(tf, event, TextF_CursorPosition(tf),
				    False, True);
      cb.reason = XmCR_VALUE_CHANGED;
      cb.event = event;
      XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			 (XtPointer) &cb);
    }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DeleteToStartOfLine(Widget w,
		    XEvent *event,
		    char **params,
		    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;

  /* if pending delete is on and there is a selection */
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (NeedsPendingDelete(tf)) (void) TextFieldRemove(w, event);
  else if (TextF_CursorPosition(tf) - 1 >= 0) {
    if (_XmTextFieldReplaceText(tf, event, 0,
			        TextF_CursorPosition(tf), NULL, 0, True, True)) {
      CheckDisjointSelection(w, TextF_CursorPosition(tf),
			     event->xkey.time);
      _XmTextFieldSetCursorPosition(tf, event, TextF_CursorPosition(tf),
				    False, True);
      cb.reason = XmCR_VALUE_CHANGED;
      cb.event = event;
      XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
			 (XtPointer) &cb);
    }
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ProcessCancel(Widget w,
	      XEvent *event,
	      char **params,
	      Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  XmParentInputActionRec  p_event;

  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_CANCEL;
  p_event.event = event;/* Pointer to XEvent. */
  p_event.params = params; /* Or use what you have if   */
  p_event.num_params = num_params;/* input is from translation.*/

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (tf->text.has_secondary) {
    tf->text.cancel = True;
    /* This will mark the has_secondary field to False. */
    _XmTextFieldSetSel2(w, 1, 0, False, event->xkey.time);
    XtUngrabKeyboard(w, CurrentTime);
  }

  if (tf->text.has_primary && tf->text.extending) {
    tf->text.cancel = True;
    /* reset orig_left and orig_right */
    _XmTextFieldStartSelection(tf, tf->text.orig_left,
			       tf->text.orig_right, event->xkey.time);
    tf->text.pending_off = False;
    _XmTextFieldSetCursorPosition(tf, NULL, tf->text.stuff_pos, True, True);
  }

  if (!tf->text.cancel)
    (void) _XmParentProcess(XtParent(tf), (XmParentProcessData) &p_event);

  if (tf->text.select_id) {
    XtRemoveTimeOut(tf->text.select_id);
    tf->text.select_id = 0;
  }
  _XmTextFieldDrawInsertionPoint(tf, True);

}

static void
Activate(Widget w,
	 XEvent *event,
	 char **params,
	 Cardinal *num_params)
{
  XmAnyCallbackStruct cb;
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmParentInputActionRec  p_event;

  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_ACTIVATE;
  p_event.event = event;/* Pointer to XEvent. */
  p_event.params = params; /* Or use what you have if   */
  p_event.num_params = num_params;/* input is from translation.*/

  cb.reason = XmCR_ACTIVATE;
  cb.event  = event;
  XtCallCallbackList(w, TextF_ActivateCallback(tf), (XtPointer) &cb);

  (void) _XmParentProcess(XtParent(w), (XmParentProcessData) &p_event);
}

static void
SetAnchorBalancing(XmTextFieldWidget tf,
		   XmTextPosition position)
{
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;
  float bal_point;

  if (!tf->text.has_primary ||
      left == right) {
    tf->text.prim_anchor = position;
  } else {
    bal_point = (float)(((float)(right - left) / 2.0) + (float)left);

    /* shift anchor and direction to opposite end of the selection */
    if ((float)position < bal_point) {
      tf->text.prim_anchor = tf->text.orig_right;
    } else if ((float)position > bal_point) {
      tf->text.prim_anchor = tf->text.orig_left;
    }
  }
}

static void
SetNavigationAnchor(XmTextFieldWidget tf,
		    XmTextPosition old_position,
		    XmTextPosition new_position,
                    Boolean extend)
{
  XmTextPosition left = tf->text.prim_pos_left,
                 right = tf->text.prim_pos_right;
  Boolean has_selection = tf->text.has_primary && left != right;

  if (!tf->text.add_mode) {
    if (extend) {
      if (old_position < left || old_position > right)
      {
	/* start outside selection - anchor at start position */
	tf->text.prim_anchor = old_position;
    }
      else if (!has_selection ||
          ((left <= new_position) && (new_position <= right)))
      {
	/* no selection, or moving into selection */
	SetAnchorBalancing(tf, old_position);
    }
      else
      {
	/* moving to outside selection */
	SetAnchorBalancing(tf, new_position);
    }
    } else {
      if (has_selection) {
	SetSelection(tf, old_position, old_position, True);
	tf->text.prim_anchor = old_position;
      }
    }
  } else if (extend) {
    if (old_position < left || old_position > right)
    {
      /* start outside selection - anchor at start position */
      tf->text.prim_anchor = old_position;
  }
    else if (!has_selection ||
        ((left <= new_position )&& (new_position <= right)))
    {
      /* no selection, or moving into selection */
      SetAnchorBalancing(tf, old_position);
  }
    else
    {
      /* moving to outside selection */
      SetAnchorBalancing(tf, new_position);
  }
  }
}

static void
CompleteNavigation(XmTextFieldWidget tf,
		   XEvent *event,
		   XmTextPosition position,
		   Time time,
                   Boolean extend)
{
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;

  if ((tf->text.add_mode &&
       tf->text.has_primary &&
       position >= left && position <= right) || extend)
    tf->text.pending_off = FALSE;
  else
    tf->text.pending_off = TRUE;

  _XmTextFieldSetCursorPosition(tf, event, position, True, True);

  if (extend) {
    if (tf->text.prim_anchor > position) {
      left = position;
      right = tf->text.prim_anchor;
    } else {
      left = tf->text.prim_anchor;
      right = position;
    }
    _XmTextFieldStartSelection(tf, left, right, time);
    tf->text.pending_off = False;

    tf->text.orig_left = left;
    tf->text.orig_right = right;
  }
}

static void
SimpleMovement(Widget w,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params,
	       XmTextPosition cursorPos,
	       XmTextPosition position)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean extend = False;
  int value;

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". If we found a match then set the Boolean to true. */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  extend = True;
      }
  }

  _XmTextFieldDrawInsertionPoint(tf, False);
  SetNavigationAnchor(tf, cursorPos, position, extend);
  CompleteNavigation(tf, event, position, event->xkey.time, extend);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void BackwardChar(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf  = (XmTextFieldWidget)w;
	XmTextPosition cursor = TextF_CursorPosition(tf);

	if (cursor <= 0)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	SimpleMovement(w, event, params, num_params, cursor, cursor - 1);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void ForwardChar(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf  = (XmTextFieldWidget)w;
	XmTextPosition cursor = TextF_CursorPosition(tf);

	if (cursor >= (XmTextPosition)tf->text.string_length)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	SimpleMovement(w, event, params, num_params, cursor, cursor + 1);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void BackwardWord(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf = (XmTextFieldWidget)w;
	XmTextPosition cursor, pos, dummy;

	if ((cursor = TextF_CursorPosition(tf)) <= 0)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	FindPrevWord(tf, &pos, &dummy);
	SimpleMovement(w, event, params, num_params, cursor, pos);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void ForwardWord(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf = (XmTextFieldWidget)w;
	XmTextPosition cursor, dummy, pos;

	if ((cursor = TextF_CursorPosition(tf)) >= (XmTextPosition)tf->text.string_length)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	FindNextWord(tf, &dummy, &pos);
	SimpleMovement(w, event, params, num_params, cursor, pos);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void EndOfLine(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf = (XmTextFieldWidget)w;
	XmTextPosition cursor, pos;

	pos = (XmTextPosition)tf->text.string_length;
	if ((cursor = TextF_CursorPosition(tf)) >= pos)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	SimpleMovement(w, event, params, num_params, cursor, pos);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void BeginningOfLine(Widget w, XEvent *event, char **params, Cardinal *num_params)
{
	XmTextFieldWidget tf = (XmTextFieldWidget) w;
	XmTextPosition cursor = TextF_CursorPosition(tf);

	if (cursor <= 0)
		return;

	_XmTextFieldDrawInsertionPoint(tf, False);
	SimpleMovement(w, event, params, num_params, cursor, 0);
	_XmTextFieldDrawInsertionPoint(tf, True);
}

static void
SetSelection(XmTextFieldWidget tf,
	     XmTextPosition left,
	     XmTextPosition right,
             Boolean redisplay)
{
  XmTextPosition display_left, display_right;
  XmTextPosition old_prim_left, old_prim_right;

  if (left < 0) left = 0;
  if (right < 0) right = 0;

  if (left > (XmTextPosition)tf->text.string_length)
    left = tf->text.string_length;
  if (right > (XmTextPosition)tf->text.string_length)
    right = tf->text.string_length;

  if (left == right && tf->text.prim_pos_left != tf->text.prim_pos_right &&
      tf->text.add_mode) {
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.add_mode = False;
    _XmTextFieldDrawInsertionPoint(tf, True);
  }
  if (left == tf->text.prim_pos_left && right == tf->text.prim_pos_right)
    return;

  TextFieldSetHighlight(tf, tf->text.prim_pos_left,
			tf->text.prim_pos_right, XmHIGHLIGHT_NORMAL);

  old_prim_left = tf->text.prim_pos_left;
  old_prim_right = tf->text.prim_pos_right;

  if (left > right) {
    tf->text.prim_pos_left = right;
    tf->text.prim_pos_right = left;
  } else {
    tf->text.prim_pos_left = left;
    tf->text.prim_pos_right = right;
  }

  TextFieldSetHighlight(tf, tf->text.prim_pos_left,
			tf->text.prim_pos_right, XmHIGHLIGHT_SELECTED);

  if (redisplay) {
    if (old_prim_left > tf->text.prim_pos_left) {
      display_left = tf->text.prim_pos_left;
    } else if (old_prim_left < tf->text.prim_pos_left) {
      display_left = old_prim_left;
    } else
      display_left = (old_prim_right > tf->text.prim_pos_right) ?
	tf->text.prim_pos_right : old_prim_right;

    if (old_prim_right < tf->text.prim_pos_right) {
      display_right = tf->text.prim_pos_right;
    } else if (old_prim_right > tf->text.prim_pos_right) {
      display_right = old_prim_right;
    } else
      display_right = (old_prim_left < tf->text.prim_pos_left) ?
	tf->text.prim_pos_left : old_prim_left;

    if (display_left > (XmTextPosition)tf->text.string_length)
      display_left = tf->text.string_length;

    if (display_right > (XmTextPosition)tf->text.string_length)
      display_right = tf->text.string_length;

    RedisplayText(tf, display_left, display_right);
  }
  tf->text.refresh_ibeam_off = True;
}

/*
 * Begin the selection by gaining ownership of the selection
 * and setting the selection parameters.
 */
void
_XmTextFieldStartSelection(XmTextFieldWidget tf,
			   XmTextPosition left,
			   XmTextPosition right,
			   Time sel_time)
{
  if (!XtIsRealized((Widget)tf)) return;

  /* if we don't already own the selection */
  if (tf->text.take_primary ||
      (tf->text.prim_pos_left == tf->text.prim_pos_right && left != right)) {
    if (!sel_time) sel_time = _XmValidTimestamp((Widget)tf);
    /* Try to gain ownership. */
    if (XmePrimarySource((Widget) tf, sel_time)) {
      XmAnyCallbackStruct cb;

      tf->text.prim_time = sel_time;
      _XmTextFieldDrawInsertionPoint(tf, False);
      if (tf->text.prim_pos_left != tf->text.prim_pos_right)
	doSetHighlight((Widget)tf, tf->text.prim_pos_left,
				tf->text.prim_pos_right, XmHIGHLIGHT_NORMAL);
      tf->text.has_primary = True;
      tf->text.take_primary = False;
      tf->text.prim_pos_left = tf->text.prim_pos_right =
	tf->text.prim_anchor = TextF_CursorPosition(tf);
      /*
       * Set the selection boundries for highlighting the text,
       * and marking the selection.
       */
      SetSelection(tf, left, right, True);

      _XmTextFieldDrawInsertionPoint(tf, True);

      /* Call the gain selection callback */
      cb.reason = XmCR_GAIN_PRIMARY;
      cb.event = NULL;
      XtCallCallbackList((Widget) tf, tf->text.gain_primary_callback,
			 (XtPointer) &cb);

    } else
      /*
       * Failed to gain ownership of the selection so make sure
       * the text does not think it owns the selection.
       * (this might be overkill)
       */
      _XmTextFieldDeselectSelection((Widget)tf, True, sel_time);
  } else {
    _XmTextFieldDrawInsertionPoint(tf, False);
    doSetHighlight((Widget)tf, tf->text.prim_pos_left,
			    tf->text.prim_pos_right, XmHIGHLIGHT_NORMAL);
    tf->text.prim_pos_left = tf->text.prim_pos_right =
      tf->text.prim_anchor = TextF_CursorPosition(tf);
    /*
     * Set the new selection boundries for highlighting the text,
     * and marking the selection.
     */
    SetSelection(tf, left, right, True);

    _XmTextFieldDrawInsertionPoint(tf, True);
  }
}

static void
ProcessHorizontalParams(Widget w,
			XEvent *event,
			char **params,
			Cardinal *num_params,
			XmTextPosition *left,
			XmTextPosition *right,
			XmTextPosition *position)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition old_cursorPos = TextF_CursorPosition(tf);
  int direction;

  *position = TextF_CursorPosition(tf);

  if (!tf->text.has_primary ||
      (*left = tf->text.prim_pos_left) == (*right = tf->text.prim_pos_right)) {
    tf->text.orig_left = tf->text.orig_right = tf->text.prim_anchor;
    *left = *right = old_cursorPos;
  }

  if (*num_params > 0)
  {
      /* Process the parameters. We can only have a single parameter which
	 will be either "right" or "left". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_DIRECTION_ACTION_PARAMS,
			   params[0], False, &direction) == True)
      {
	  if (direction == _RIGHT) {
	      if (*position >= tf->text.string_length) return;
	      (*position)++;
	  }
	  else if (direction == _LEFT) {
	      if (*position <= 0) return;
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
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  *position = TextF_CursorPosition(tf);

  if (!tf->text.has_primary ||
      tf->text.prim_pos_left == tf->text.prim_pos_right) {
    if (*position > tf->text.prim_anchor) {
      *left = tf->text.prim_anchor;
      *right = *position;
    } else {
      *left = *position;
      *right = tf->text.prim_anchor;
    }
  }
}


static void
KeySelection(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextPosition position = 0, left, right;
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition cursorPos;
  int direction;

  TextFieldResetIC(w);
  _XmTextFieldDrawInsertionPoint(tf,False); /* Turn off I beam blink
					       during selection */

  tf->text.orig_left = tf->text.prim_pos_left;
  tf->text.orig_right = tf->text.prim_pos_right;

  cursorPos = TextF_CursorPosition(tf);
  if (*num_params > 0)
  {
      /* Process the parameters. The only legal values are "right" and
	 "left". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_DIRECTION_ACTION_PARAMS,
			   params[0], False, &direction) == True)
      {
	  SetAnchorBalancing(tf, cursorPos);
      }
  }

  tf->text.extending = True;

  if (*num_params == 0)
  {
    position = cursorPos;
    ProcessSelectParams(w, event, &left, &right, &position);
  }
  else if (*num_params > 0)
  {
      /* Process the parameters. The only legal values are "right" and
	 "left". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_DIRECTION_ACTION_PARAMS,
			   params[0], False, &direction) == True)
      {
	  ProcessHorizontalParams(w, event, params, num_params, &left,
				  &right, &position);
      }
  }

  cursorPos = position;

  if (position < 0 || position > (XmTextPosition)tf->text.string_length) {
    _XmTextFieldDrawInsertionPoint(tf,True); /* Turn on I beam now
						that we are done */
    tf->text.extending = False;
    return;
  }

  /* shift anchor and direction to opposite end of the selection */
  if (position > tf->text.prim_anchor) {
    right = cursorPos = position;
    left = tf->text.prim_anchor;
  } else {
    left = cursorPos = position;
    right = tf->text.prim_anchor;
  }

  if (left > right) {
    XmTextPosition tempIndex = left;
    left = right;
    right = tempIndex;
  }

  if (tf->text.take_primary)
    _XmTextFieldStartSelection(tf, left, right, event->xbutton.time);
  else
    SetSelection(tf, left, right, True);

  tf->text.pending_off = False;

  _XmTextFieldSetCursorPosition(tf, event, cursorPos, True, True);
  (void) SetDestination(w, cursorPos, False, event->xkey.time);

  tf->text.orig_left = tf->text.prim_pos_left;
  tf->text.orig_right = tf->text.prim_pos_right;

  tf->text.extending = False;
  _XmTextFieldDrawInsertionPoint(tf,True); /* Turn on I beam now
					      that we are done */

}

static void
TextFocusIn(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;
  XRectangle xmim_area;
  XPoint xmim_point;

  if (event->xfocus.send_event && !(tf->text.has_focus)) {
    tf->text.has_focus = True;
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.blink_on = False;

    tf->text.refresh_ibeam_off = True;
    if (_XmGetFocusPolicy(w) == XmEXPLICIT) {
      if (((XmTextFieldWidgetClass)
	   XtClass(w))->primitive_class.border_highlight) {
	(*((XmTextFieldWidgetClass)
	   XtClass(w))->primitive_class.border_highlight)(w);
      }
      if (!tf->text.has_destination && !tf->text.sel_start)
	(void) SetDestination(w, TextF_CursorPosition(tf), False,
			      XtLastTimestampProcessed(XtDisplay(w)));
    }
    if (XtIsSensitive(w)) ChangeBlinkBehavior(tf, True);
    _XmTextFieldDrawInsertionPoint(tf, True);
    (void) GetXYFromPos(tf, TextF_CursorPosition(tf),
			&xmim_point.x, &xmim_point.y);
    (void)TextFieldGetDisplayRect((Widget)tf, &xmim_area);
    XmImVaSetFocusValues(w, XmNspotLocation, &xmim_point,
			 XmNarea, &xmim_area, NULL);

    cb.reason = XmCR_FOCUS;
    cb.event = event;
    XtCallCallbackList (w, tf->text.focus_callback, (XtPointer) &cb);
  }

  _XmPrimitiveFocusIn(w, event, params, num_params);
}


static void
TextFocusOut(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (event->xfocus.send_event && tf->text.has_focus) {
    ChangeBlinkBehavior(tf, False);
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.has_focus = False;
    tf->text.blink_on = True;
    _XmTextFieldDrawInsertionPoint(tf, True);
    if (((XmTextFieldWidgetClass) XtClass(tf))
	->primitive_class.border_unhighlight) {
      (*((XmTextFieldWidgetClass) XtClass(tf))
       ->primitive_class.border_unhighlight)((Widget) tf);
    }
    XmImUnsetFocus(w);
  }

  /* If traversal is on, then the leave verification callback is called in
     the traversal event handler */
  if (event->xfocus.send_event && !tf->text.traversed &&
      _XmGetFocusPolicy(w) == XmEXPLICIT) {
    if (!VerifyLeave(tf, event)) {
      if (tf->text.verify_bell) XBell(XtDisplay(w), 0);
      return;
    }
  } else
    if (tf->text.traversed) {
      tf->text.traversed = False;
    }
}

static void
SetScanIndex(XmTextFieldWidget tf,
	     XEvent *event)
{
  Time sel_time;

  if (event->type == ButtonPress) sel_time = event->xbutton.time;
  else sel_time = event->xkey.time;


  if (sel_time > tf->text.last_time &&
      (int)(sel_time - tf->text.last_time) < XtGetMultiClickTime(XtDisplay(tf))) {
    /*
     * Fix for HaL DTS 9841 - Increment the sarray_index first, then check to
     *			  see if it is greater that the count.  Otherwise,
     *			  an error will occur.
     */
    tf->text.sarray_index++;
    if (tf->text.sarray_index >= TextF_SelectionArrayCount(tf)) {
      tf->text.sarray_index = 0;
    }
    /*
     * End fix for HaL DTS 9841
     */
  } else
    tf->text.sarray_index = 0;

  tf->text.last_time = sel_time;
}

static void
ExtendScanSelection(XmTextFieldWidget tf,
		    XEvent *event)
{
  XmTextPosition pivot_left, pivot_right;
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;
  XmTextPosition new_position = GetPosFromX(tf, (Position) event->xbutton.x);
  XmTextPosition cursorPos = TextF_CursorPosition(tf);
  Boolean pivot_modify = False;
  float bal_point;

  if (!tf->text.has_primary ||
      left == right) {
    tf->text.orig_left = tf->text.orig_right =
      tf->text.prim_anchor = TextF_CursorPosition(tf);
    bal_point = (XmTextPosition) tf->text.prim_anchor ;
  } else
    bal_point = (float)(((float)(right - left) / 2.0) + (float)left);

  if (!tf->text.extending)
  {
    if ((float)new_position < bal_point)
    {
      tf->text.prim_anchor = tf->text.orig_right;
    }
    else if ((float)new_position > bal_point)
    {
      tf->text.prim_anchor = tf->text.orig_left;
    }
}
  tf->text.extending = True;

  switch (TextF_SelectionArray(tf)[tf->text.sarray_index]) {
  case XmSELECT_POSITION:
    if (tf->text.take_primary && new_position != tf->text.prim_anchor)
      _XmTextFieldStartSelection(tf, tf->text.prim_anchor,
				 new_position, event->xbutton.time);
    else if (tf->text.has_primary)
      SetSelection(tf, tf->text.prim_anchor, new_position, True);
    tf->text.pending_off = False;
    cursorPos = new_position;
    break;
  case XmSELECT_WHITESPACE:
  case XmSELECT_WORD:
    FindWord(tf, new_position, &left, &right);
    FindWord(tf, tf->text.prim_anchor,
	     &pivot_left, &pivot_right);
    tf->text.pending_off = False;
    if (left != pivot_left || right != pivot_right) {
      if (left > pivot_left)
	left = pivot_left;
      if (right < pivot_right)
	right = pivot_right;
      pivot_modify = True;
    }
    if (tf->text.take_primary)
      _XmTextFieldStartSelection(tf, left, right, event->xbutton.time);
    else
      SetSelection(tf, left, right, True);

    if (pivot_modify) {
      if ((((right - left) / 2) + left) <= new_position) {
	cursorPos = right;
      } else
	cursorPos = left;
    } else {
      if (left >= TextF_CursorPosition(tf))
	cursorPos = left;
      else
	cursorPos = right;
    }
    break;
  default:
    break;
  }
  if (cursorPos != TextF_CursorPosition(tf)) {
    (void) SetDestination((Widget)tf, cursorPos, False, event->xkey.time);
    _XmTextFieldSetCursorPosition(tf, event, cursorPos, True, True);
  }
}

static void
SetScanSelection(XmTextFieldWidget tf,
		 XEvent *event)
{
  XmTextPosition left, right;
  XmTextPosition new_position = 0;
  XmTextPosition cursorPos = TextF_CursorPosition(tf);
  Position dummy = 0;
  Boolean update_position = False;

  SetScanIndex(tf, event);

  if (event->type == ButtonPress)
    new_position = GetPosFromX(tf, (Position) event->xbutton.x);
  else
    new_position = TextF_CursorPosition(tf);

  _XmTextFieldDrawInsertionPoint(tf,False); /* Turn off I beam
					       blink during selection */

  switch (TextF_SelectionArray(tf)[tf->text.sarray_index]) {
  case XmSELECT_POSITION:
    tf->text.prim_anchor = new_position;
    if (tf->text.has_primary) {
      SetSelection(tf, new_position, new_position, True);
      tf->text.pending_off = False;
    }
    cursorPos = new_position;
    update_position = True;
    break;
  case XmSELECT_WHITESPACE:
  case XmSELECT_WORD:
    FindWord(tf, TextF_CursorPosition(tf), &left, &right);
    if (tf->text.take_primary)
      _XmTextFieldStartSelection(tf, left, right, event->xbutton.time);
    else
      SetSelection(tf, left, right, True);
    tf->text.pending_off = False;
    if ((((right - left) / 2) + left) <= new_position)
      cursorPos = right;
    else
      cursorPos = left;
    break;
  case XmSELECT_LINE:
  case XmSELECT_OUT_LINE:
  case XmSELECT_PARAGRAPH:
  case XmSELECT_ALL:
    if (tf->text.take_primary)
      _XmTextFieldStartSelection(tf, 0, tf->text.string_length,
				 event->xbutton.time);
    else
      SetSelection(tf, 0, tf->text.string_length, True);
    tf->text.pending_off = False;
    if (event->type == ButtonPress)
    {
      if ((XmTextPosition)(tf->text.string_length) / 2 <= new_position)
      {
	cursorPos = (XmTextPosition)tf->text.string_length;
      }
      else
      {
	cursorPos = 0;
      }
    }
    break;
  }

  (void) SetDestination((Widget)tf, cursorPos, False, event->xkey.time);
  if (cursorPos != TextF_CursorPosition(tf) || update_position) {
    _XmTextFieldSetCursorPosition(tf, event, cursorPos, True, True);
  }
  GetXYFromPos(tf, cursorPos, &(tf->text.select_pos_x),
	       &dummy);
  _XmTextFieldDrawInsertionPoint(tf,True);
}


static void
StartPrimary(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  if (!tf->text.has_focus && _XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  _XmTextFieldDrawInsertionPoint(tf,False);
  SetScanSelection(tf, event); /* use scan type to set the selection */
  tf->text.stuff_pos = TextF_CursorPosition(tf);
  _XmTextFieldDrawInsertionPoint(tf,True);
}


static void
MoveDestination(Widget w,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left = tf->text.prim_pos_left,
                 right = tf->text.prim_pos_right;
  XmTextPosition new_position;
  Boolean old_has_focus = tf->text.has_focus;
  Boolean reset_cursor = False;

  TextFieldResetIC(w);
  new_position = GetPosFromX(tf, (Position) event->xbutton.x);

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (tf->text.has_primary && (right != left))
    (void) SetDestination(w, new_position, False, event->xbutton.time);

  tf->text.pending_off = False;

  if (!tf->text.has_focus && _XmGetFocusPolicy(w) == XmEXPLICIT)
    (void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);

  /* Doing the the MoveDestination caused a traversal into my, causing
   * me to gain focus... Cursor is now on when it shouldn't be. */
  if ((reset_cursor = !old_has_focus && tf->text.has_focus) != False)
    _XmTextFieldDrawInsertionPoint(tf, False);

  _XmTextFieldSetCursorPosition(tf, event, new_position,
				True, True);
  if (new_position < left && new_position > right)
    tf->text.pending_off = True;

  /*
   * if cursor was turned off as a result of the focus state changing
   * then we need to undo the decrement to the cursor_on variable
   * by redrawing the insertion point.
   */
  if (reset_cursor)
    _XmTextFieldDrawInsertionPoint(tf, True);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ExtendPrimary(Widget w,
	      XEvent *event,
	      char **params,
	      Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);

  if (tf->text.cancel) return;

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.do_drop = False;

  if (event->type == ButtonPress)
    tf->text.stuff_pos = TextF_CursorPosition(tf);

  if (!CheckTimerScrolling(w, event)) {
    if (event->type == ButtonPress)
      DoExtendedSelection(w, event->xbutton.time);
    else
      DoExtendedSelection(w, event->xkey.time);
  } else
    ExtendScanSelection(tf, event); /* use scan type to set the selection */

  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ExtendEnd(Widget w,
	  XEvent *event,
	  char **params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;


  if (tf->text.prim_pos_left == 0 && tf->text.prim_pos_right == 0)
    tf->text.orig_left = tf->text.orig_right = TextF_CursorPosition(tf);
  else {
    tf->text.orig_left = tf->text.prim_pos_left;
    tf->text.orig_right = tf->text.prim_pos_right;
    tf->text.cancel = False;
  }

  if (tf->text.select_id) {
    XtRemoveTimeOut(tf->text.select_id);
    tf->text.select_id = 0;
  }
  tf->text.select_pos_x = 0;
  tf->text.extending = False;
}

static void
DoExtendedSelection(Widget w,
		    Time time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition position, cursorPos;
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;
  XmTextPosition pivot_left, pivot_right;
  Boolean pivot_modify = False;
  float bal_point;

  if (tf->text.cancel) {
    if (tf->text.select_id)
	{
        XtRemoveTimeOut(tf->text.select_id);
        tf->text.select_id = 0;
	}
    return;
  }

  cursorPos = TextF_CursorPosition(tf);
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (!tf->text.has_primary || left == right) {
    tf->text.prim_anchor = tf->text.cursor_position;
    left = right = TextF_CursorPosition(tf);
    tf->text.orig_left = tf->text.orig_right = tf->text.prim_anchor;
    bal_point = tf->text.prim_anchor;
  } else
    bal_point = (float)(((float)(tf->text.orig_right - tf->text.orig_left)
			 / 2.0) + (float)tf->text.orig_left);

  position = GetPosFromX(tf, tf->text.select_pos_x);

  if (!tf->text.extending)
  {
    if ((float)position < bal_point) {
      tf->text.prim_anchor = tf->text.orig_right;
    } else if ((float)position > bal_point) {
      tf->text.prim_anchor = tf->text.orig_left;
    }
  }

  tf->text.extending = True;

  /* Extend selection in same way as ExtendScan would do */

  switch (TextF_SelectionArray(tf)[tf->text.sarray_index]) {
  case XmSELECT_POSITION:
    if (tf->text.take_primary && position != tf->text.prim_anchor)
      _XmTextFieldStartSelection(tf, tf->text.prim_anchor,
				 position, time);
    else if (tf->text.has_primary)
      SetSelection(tf, tf->text.prim_anchor, position, True);
    tf->text.pending_off = False;
    cursorPos = position;
    break;
  case XmSELECT_WHITESPACE:
  case XmSELECT_WORD:
    FindWord(tf, position, &left, &right);
    FindWord(tf, tf->text.prim_anchor,
	     &pivot_left, &pivot_right);
    tf->text.pending_off = False;
    if (left != pivot_left || right != pivot_right) {
      if (left > pivot_left)
	left = pivot_left;
      if (right < pivot_right)
	right = pivot_right;
      pivot_modify = True;
    }
    if (tf->text.take_primary)
      _XmTextFieldStartSelection(tf, left, right, time);
    else
      SetSelection(tf, left, right, True);

    if (pivot_modify) {
      if ((((right - left) / 2) + left) <= position) {
	cursorPos = right;
      } else
	cursorPos = left;
    } else {
      if (left >= TextF_CursorPosition(tf))
	cursorPos = left;
      else
	cursorPos = right;
    }
    break;
  default:
    break;
  }
  if (cursorPos != TextF_CursorPosition(tf)) {
    (void) SetDestination((Widget)tf, cursorPos, False, time);
    _XmTextFieldSetCursorPosition(tf, NULL, cursorPos, True, True);
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DoSecondaryExtend(Widget w,
		  Time ev_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  XmTextPosition position = GetPosFromX(tf, tf->text.select_pos_x);

  if (tf->text.cancel) return;

  if (position < tf->text.sec_anchor) {
    if (tf->text.sec_pos_left > 0)
      _XmTextFieldSetSel2(w, position, tf->text.sec_anchor, False, ev_time);
    if (tf->text.sec_pos_left >= 0) AdjustText(tf, tf->text.sec_pos_left, True);
  } else if (position > tf->text.sec_anchor) {
    if (tf->text.sec_pos_right < tf->text.string_length)
      _XmTextFieldSetSel2(w, tf->text.sec_anchor, position, False, ev_time);
    if (tf->text.sec_pos_right >= 0)
      AdjustText(tf, tf->text.sec_pos_right, True);
  } else {
    _XmTextFieldSetSel2(w, position, position, False, ev_time);
    if (position >= 0) AdjustText(tf, position, True);
  }

  tf->text.sec_extending = True;
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
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (tf->text.cancel) {
    tf->text.select_id = 0;
    return;
  }

  if (!tf->text.select_id) return;

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (tf->text.sec_extending)
    DoSecondaryExtend(w, XtLastTimestampProcessed(XtDisplay(w)));
  else if (tf->text.extending)
    DoExtendedSelection(w, XtLastTimestampProcessed(XtDisplay(w)));

  XSync (XtDisplay(w), False);

  _XmTextFieldDrawInsertionPoint(tf, True);

  tf->text.select_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
				       (unsigned long) PRIM_SCROLL_INTERVAL,
				       BrowseScroll, (XtPointer) w);
}


static Boolean
CheckTimerScrolling(Widget w,
		    XEvent *event)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Dimension margin_size = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Dimension top_margin = TextF_MarginHeight(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  tf->text.select_pos_x = event->xmotion.x;

  if ((event->xmotion.x > (int) margin_size) &&
      (event->xmotion.x < (int) (tf->core.width - margin_size))  &&
      (event->xmotion.y > (int) top_margin) &&
      (event->xmotion.y < (int) (top_margin + TextF_FontAscent(tf) +
                                 TextF_FontDescent(tf)))) {

    if (tf->text.select_id) {
      XtRemoveTimeOut(tf->text.select_id);
      tf->text.select_id = 0;
    }
  } else {
    /* to the left of the text */
    if (event->xmotion.x <= (int) margin_size)
      tf->text.select_pos_x = (Position) (margin_size -
                                          (tf->text.average_char_width + 1));
    /* to the right of the text */
    else if (event->xmotion.x >= (int) (tf->core.width - margin_size))
      tf->text.select_pos_x = (Position) ((tf->core.width - margin_size) +
					  tf->text.average_char_width + 1);
    if (!tf->text.select_id)
      tf->text.select_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					   (unsigned long) SEC_SCROLL_INTERVAL,
					   BrowseScroll, (XtPointer) w);
    return True;
  }
  return False;
}

static void
RestorePrimaryHighlight(XmTextFieldWidget tf,
			XmTextPosition prim_left,
			XmTextPosition prim_right)
{
  if (tf->text.sec_pos_right >= prim_left &&
      tf->text.sec_pos_right <= prim_right) {
    /* secondary selection is totally inside primary selection */
    if (tf->text.sec_pos_left >= prim_left) {
      TextFieldSetHighlight(tf, prim_left, tf->text.sec_pos_left,
                            XmHIGHLIGHT_SELECTED);
      TextFieldSetHighlight(tf, tf->text.sec_pos_left,
			    tf->text.sec_pos_right,
			    XmHIGHLIGHT_NORMAL);
      TextFieldSetHighlight(tf, tf->text.sec_pos_right, prim_right,
			    XmHIGHLIGHT_SELECTED);
      /* right side of secondary selection is inside primary selection */
    } else {
      TextFieldSetHighlight(tf, tf->text.sec_pos_left, prim_left,
			    XmHIGHLIGHT_NORMAL);
      TextFieldSetHighlight(tf, prim_left, tf->text.sec_pos_right,
			    XmHIGHLIGHT_SELECTED);
    }
  } else {
    /* left side of secondary selection is inside primary selection */
    if (tf->text.sec_pos_left <= prim_right &&
	tf->text.sec_pos_left >= prim_left) {
      TextFieldSetHighlight(tf, tf->text.sec_pos_left, prim_right,
			    XmHIGHLIGHT_SELECTED);
      TextFieldSetHighlight(tf, prim_right, tf->text.sec_pos_right,
			    XmHIGHLIGHT_NORMAL);
    } else  {
      /* secondary selection encompasses the primary selection */
      if (tf->text.sec_pos_left <= prim_left &&
	  tf->text.sec_pos_right >= prim_right) {
	TextFieldSetHighlight(tf, tf->text.sec_pos_left, prim_left,
			      XmHIGHLIGHT_NORMAL);
	TextFieldSetHighlight(tf, prim_left, prim_right,
			      XmHIGHLIGHT_SELECTED);
	TextFieldSetHighlight(tf, prim_right, tf->text.sec_pos_right,
			      XmHIGHLIGHT_NORMAL);
	/* secondary selection is outside primary selection */
      } else {
	TextFieldSetHighlight(tf, prim_left, prim_right,
			      XmHIGHLIGHT_SELECTED);
	TextFieldSetHighlight(tf, tf->text.sec_pos_left,
			      tf->text.sec_pos_right,
			      XmHIGHLIGHT_NORMAL);
      }
    }
  }
}

void
_XmTextFieldSetSel2(Widget w,
		    XmTextPosition left,
		    XmTextPosition right,
		    Boolean disown,
		    Time sel_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean result;

  if (tf->text.has_secondary) {
    if (left == tf->text.sec_pos_left && right == tf->text.sec_pos_right)
      return;

    /* If the widget has the primary selection, make sure the selection
     * highlight is restored appropriately.
     */
    if (tf->text.has_primary)
      RestorePrimaryHighlight(tf, tf->text.prim_pos_left,
			      tf->text.prim_pos_right);
    else
      TextFieldSetHighlight(tf, tf->text.sec_pos_left,
			    tf->text.sec_pos_right, XmHIGHLIGHT_NORMAL);
  }

  if (left < right) {
    if (!tf->text.has_secondary) {
      if (!sel_time) sel_time = _XmValidTimestamp(w);
      result = XmeSecondarySource(w, sel_time);
      tf->text.sec_time = sel_time;
      tf->text.has_secondary = result;
      if (result) {
	tf->text.sec_pos_left = left;
	tf->text.sec_pos_right = right;
      }
    } else {
      tf->text.sec_pos_left = left;
      tf->text.sec_pos_right = right;
    }
    tf->text.sec_drag = True;
  } else {
    if (left > right)
      tf->text.has_secondary = False;
    tf->text.sec_pos_left = tf->text.sec_pos_right = left;
    if (disown) {
      if (!sel_time) sel_time = _XmValidTimestamp(w);
      XtDisownSelection(w, XA_SECONDARY, sel_time);
      tf->text.has_secondary = False;
    }
  }

  TextFieldSetHighlight((XmTextFieldWidget) w, tf->text.sec_pos_left,
			tf->text.sec_pos_right,
			XmHIGHLIGHT_SECONDARY_SELECTED);

  /* This can be optimized for performance enhancement */

  RedisplayText(tf, 0, tf->text.string_length);
}

static void
StartDrag(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Widget drag_icon;
  Arg args[6];
  int n;

  drag_icon = XmeGetTextualDragIcon(w);

  n = 0;
  XtSetArg(args[n], XmNcursorBackground, tf->core.background_pixel);  n++;
  XtSetArg(args[n], XmNcursorForeground, tf->primitive.foreground);  n++;
  XtSetArg(args[n], XmNsourceCursorIcon, drag_icon);  n++;
  if (TextF_Editable(tf)) {
    XtSetArg(args[n], XmNdragOperations, (XmDROP_MOVE | XmDROP_COPY)); n++;
  } else {
    XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++;
  }
  (void) XmeDragSource(w, (XtPointer) w, event, args, n);
}

static	void
DragStart(XtPointer data,
	  XtIntervalId *id)	/* unused */
{
  XmTextFieldWidget tf = (XmTextFieldWidget)data;

  tf->text.drag_id = 0;
  StartDrag((Widget)tf, tf->text.transfer_action->event,
	    tf->text.transfer_action->params,
	    tf->text.transfer_action->num_params);
}

static void
StartSecondary(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition position = GetPosFromX(tf, (Position) event->xbutton.x);
  int status;

  tf->text.sel_start = True;
  XAllowEvents(XtDisplay(w), AsyncBoth, event->xbutton.time);
  tf->text.sec_anchor = position;
  tf->text.selection_move = FALSE;
  tf->text.selection_link = FALSE;

  status = XtGrabKeyboard(w, False, GrabModeAsync, GrabModeAsync,
			  event->xbutton.time);

  if (status != GrabSuccess)
    XmeWarning(w, GRABKBDERROR);
}


static void
ProcessBDrag(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  if (tf->text.extending)
    return;

   /* if the user has clicked twice very quickly, don't lose the original left
   ** position
   */
   if (!tf->text.has_secondary || (tf->text.sec_pos_left == tf->text.sec_pos_right))
    tf->text.sec_pos_left = GetPosFromX(tf, (Position) event->xbutton.x);

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (InSelection(w, event)) {
    tf->text.sel_start = False;
    StartDrag(w, event, params, num_params);
  } else {
    StartSecondary(w, event, params, num_params);
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
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
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition position;
  XmTextPosition left = tf->text.prim_pos_left,
                 right = tf->text.prim_pos_right;
  Position left_x, right_x, dummy;

  position = GetPosFromX(tf, (Position) event->xbutton.x);

  return (tf->text.has_primary &&
	  left != right &&
	  ( (position > left && position < right) ||
	    ( position == left &&
	      GetXYFromPos(tf, left, &left_x, &dummy) &&
	      event->xbutton.x > left_x) ||
	    ( position == right &&
	      GetXYFromPos(tf, right, &right_x, &dummy) &&
	      event->xbutton.x < right_x)));
}

static void
ProcessBSelect(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
#define ABS_DELTA(x1, x2) (x1 < x2 ? x2 - x1 : x1 - x2)

  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XtEnum drag_on_btn1 = XmOFF;
  Time event_time = event->xbutton.time;
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
	  (event_time > tf->text.last_time &&
	   (int)(event_time - tf->text.last_time) <
	   XtGetMultiClickTime(XtDisplay(w)))) {
	if (*num_params > 0)
	  XtCallActionProc(w, params[0], event, NULL, 0);
      } else {
	if (tf->text.drag_id)
	  XtRemoveTimeOut(tf->text.drag_id);
	if (tf->text.transfer_action == NULL) {
	  tf->text.transfer_action =
	    (_XmTextActionRec *) XtMalloc(sizeof(_XmTextActionRec));
	  tf->text.transfer_action->event = (XEvent *)XtMalloc(sizeof(XEvent));
	}
	memcpy((void *)tf->text.transfer_action->event, (void *)event,
	       sizeof(XEvent));
	tf->text.transfer_action->params = params;
	tf->text.transfer_action->num_params = num_params;
	tf->text.drag_id = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
					   XtGetMultiClickTime(XtDisplay(w)),
					   DragStart, (XtPointer)w);
      }
      break;
    case ButtonRelease:
      if (tf->text.drag_id) {
	XtRemoveTimeOut(tf->text.drag_id);
	tf->text.drag_id = 0;
	if (*tf->text.transfer_action->num_params) {
	  XtCallActionProc(w, tf->text.transfer_action->params[0],
			   tf->text.transfer_action->event, NULL, 0);
	}
      }
      XtCallActionProc(w, params[0], event, NULL, 0);
      break;
    case MotionNotify:
      if (tf->text.drag_id) {
	XEvent *press = tf->text.transfer_action->event;
	if (ABS_DELTA(press->xbutton.x_root, event->xmotion.x_root) >
	    tf->text.threshold ||
	    ABS_DELTA(press->xbutton.y_root, event->xmotion.y_root) >
	    tf->text.threshold) {
	  XtRemoveTimeOut(tf->text.drag_id);
	  tf->text.drag_id = 0;
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

static void
ExtendSecondary(Widget w,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition position = GetPosFromX(tf, (Position) event->xbutton.x);

  TextFieldResetIC(w);

  if (tf->text.cancel) return;

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (position < tf->text.sec_anchor) {
    _XmTextFieldSetSel2(w, position, tf->text.sec_anchor,
			False, event->xbutton.time);
  } else if (position > tf->text.sec_anchor) {
    _XmTextFieldSetSel2(w, tf->text.sec_anchor, position,
			False, event->xbutton.time);
  } else {
    _XmTextFieldSetSel2(w, position, position, False, event->xbutton.time);
  }

  tf->text.sec_extending = True;

  if (!CheckTimerScrolling(w, event))
    DoSecondaryExtend(w, event->xmotion.time);

  _XmTextFieldDrawInsertionPoint(tf, True);
}



static void
Stuff(Widget w,
      XEvent *event,
      char **params,
      Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XPoint *point = NULL;

  /* Request targets from the selection owner so you can decide what to
   * request.  The decision process and request for the selection is
   * taken care of in HandleTargets().
   */
  if (event && event->type == ButtonRelease) {
      /* WARNING: do not free the following memory in this module.  It
       * will be freed in FreeLocationData, triggered at the end of
       * the data transfer operation.
       */
      point = (XPoint *) XtMalloc(sizeof(XPoint));
      point->x = event->xbutton.x;
      point->y = event->xbutton.y;
  }

  if (tf->text.selection_link)
    XmePrimarySink(w, XmLINK, (XtPointer) point,
		   event->xbutton.time);
  else if (tf->text.selection_move)
    XmePrimarySink(w, XmMOVE, (XtPointer) point,
		   event->xbutton.time);
  else
    XmePrimarySink(w, XmCOPY, (XtPointer) point,
		   event->xbutton.time);
}

void
_XmTextFieldHandleSecondaryFinished(Widget w,
			 XEvent *event)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  TextFDestData dest_data;
  XmTextPosition left, right;
  int adjustment = 0;
  Time time = XtLastTimestampProcessed(XtDisplay(w));
  XmAnyCallbackStruct cb;

  dest_data = GetTextFDestData(w);

  if (dest_data->has_destination) {
    adjustment = (int) (tf->text.sec_pos_right - tf->text.sec_pos_left);

    doSetHighlight(w, tf->text.sec_pos_left,
			    tf->text.sec_pos_right, XmHIGHLIGHT_NORMAL);
    if (dest_data->position <= tf->text.sec_pos_left) {
      tf->text.sec_pos_left += adjustment - dest_data->replace_length;
      tf->text.sec_pos_right += adjustment - dest_data->replace_length;
    } else if (dest_data->position > tf->text.sec_pos_left &&
	       dest_data->position < tf->text.sec_pos_right) {
      tf->text.sec_pos_left -= dest_data->replace_length;
      tf->text.sec_pos_right += adjustment - dest_data->replace_length;
    }
  }

  left = tf->text.sec_pos_left;
  right = tf->text.sec_pos_right;

  /* This will mark the has_secondary field to False. */
  (void) _XmTextFieldSetSel2(w, 1, 0, False, time);

  if (_XmTextFieldReplaceText(tf, event, left, right, NULL, 0, False, True)) {
    XmTextPosition cursorPos;
    if (dest_data->has_destination && TextF_CursorPosition(tf) > right) {
      cursorPos = TextF_CursorPosition(tf) - (right - left);
      if (!dest_data->quick_key)
	_XmTextFieldSetCursorPosition(tf, NULL, cursorPos, True, True);
      (void) SetDestination((Widget) tf, cursorPos, False, time);
    }
    if (!dest_data->has_destination) {
      /* make some adjustments necessary -- cursor position may refer to
      ** text which is gone
      */
      cursorPos = TextF_CursorPosition(tf);
      if (left < cursorPos)
		cursorPos -= (right - left);
      tf->text.prim_anchor = cursorPos;
      if (tf->text.add_mode) {
	_XmTextFieldDrawInsertionPoint(tf, False);
	tf->text.add_mode = False;
        TextF_CursorPosition(tf) = cursorPos;
	_XmTextFieldDrawInsertionPoint(tf, True);
      }
      else if (cursorPos != TextF_CursorPosition(tf))
	{
		/* if it changed, redraw, carefully using internal routines
		** to avoid calling _XmSetDestination
		*/
		_XmTextFieldDrawInsertionPoint(tf, False);
        	TextF_CursorPosition(tf) = cursorPos;
		SetCursorPosition(tf, NULL, cursorPos, False, False, True, ForceTrue);
		_XmTextFieldDrawInsertionPoint(tf, True);
	}
    }

    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
		       (XtPointer) &cb);
  }
}


/*
 * Notify the primary selection that the secondary selection
 * wants to insert it's selection data into the primary selection.
 */
   /* REQUEST TARGETS FROM SELECTION RECEIVER; THEN CALL HANDLETARGETS
    * WHICH LOOKS AT THE TARGET LIST AND DETERMINE WHAT TARGET TO PLACE
    * IN THE PAIR.  IT WILL THEN DO ANY NECESSARY CONVERSIONS BEFORE
    * TELLING THE RECEIVER WHAT TO REQUEST AS THE SELECTION VALUE.
    * THIS WILL GUARANTEE THE BEST CHANCE AT A SUCCESSFUL EXCHANGE.
    */
static void
SecondaryNotify(Widget w,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Atom CS_OF_ENCODING = XmeGetEncodingAtom(w);
  TextFDestData dest_data;
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;

  if (tf->text.selection_move == TRUE && tf->text.has_destination &&
      TextF_CursorPosition(tf) >= tf->text.sec_pos_left &&
      TextF_CursorPosition(tf) <= tf->text.sec_pos_right) {
      /* This will mark the has_secondary field to False. */
      (void) _XmTextFieldSetSel2(w, 1, 0, False, event->xbutton.time);
      return;
  }

  /*
   * Determine what the reciever supports so you can tell 'em what to
   * request.
   */

  dest_data = GetTextFDestData(w);

  dest_data->has_destination = tf->text.has_destination;
  dest_data->position = TextF_CursorPosition(tf);
  dest_data->replace_length = 0;

  if (*(num_params) == 1) dest_data->quick_key = True;
  else dest_data->quick_key = False;

  if (tf->text.has_primary && left != right) {
    if (dest_data->position >= left && dest_data->position <= right)
      dest_data->replace_length = (int) (right - left);
  }

  /*
   * Make a request for the primary selection to convert to
   * type INSERT_SELECTION as per ICCCM.
   */

  if (tf->text.selection_link)
    XmeSecondaryTransfer(w, CS_OF_ENCODING, XmLINK, event->xbutton.time);
  else if (tf->text.selection_move)
    XmeSecondaryTransfer(w, CS_OF_ENCODING, XmMOVE, event->xbutton.time);
  else
    XmeSecondaryTransfer(w, CS_OF_ENCODING, XmCOPY, event->xbutton.time);
}

   /*
    * LOOKS AT THE TARGET LIST AND DETERMINE WHAT TARGET TO PLACE
    * IN THE PAIR.  IT WILL THEN DO ANY NECESSARY CONVERSIONS BEFORE
    * TELLING THE RECEIVER WHAT TO REQUEST AS THE SELECTION VALUE.
    * THIS WILL GUARANTEE THE BEST CHANCE AT A SUCCESSFUL EXCHANGE.
    */

static void
ProcessBDragRelease(Widget w,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XButtonEvent      *ev = (XButtonEvent *) event;
  XmTextPosition position;

  if (tf->text.extending)
    return;

  /* Work around for intrinsic bug.  Remove once bug is fixed. */
  XtUngrabPointer(w, ev->time);

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (!tf->text.cancel) XtUngrabKeyboard(w, CurrentTime);

  position = GetPosFromX(tf, (Position) event->xbutton.x);

  if (tf->text.sel_start) {
    if (tf->text.has_secondary &&
	tf->text.sec_pos_left != tf->text.sec_pos_right) {
      if ((Dimension)ev->x > tf->core.width || ev->x < 0 ||
	  (Dimension)ev->y > tf->core.height || ev->y < 0) {
	  /* This will mark the has_secondary field to False. */
	  _XmTextFieldSetSel2(w, 1, 0, False, event->xkey.time);
      } else {
	  SecondaryNotify(w, event, params, num_params);
      }
    } else if (!tf->text.sec_drag && !tf->text.cancel &&
	       tf->text.sec_pos_left == position) {
      /*
       * Copy contents of primary selection to the stuff position found above.
       */
      Stuff(w, event, params, num_params);
    }
  }

  if (tf->text.select_id) {
    XtRemoveTimeOut(tf->text.select_id);
    tf->text.select_id = 0;
  }

  tf->text.sec_extending = False;

  tf->text.sec_drag = False;
  tf->text.sel_start = False;
  tf->text.cancel = False;
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ProcessCopy(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = FALSE;
  tf->text.selection_link = FALSE;
  ProcessBDragRelease(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ProcessLink(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = FALSE;
  tf->text.selection_link = TRUE;
  ProcessBDragRelease(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ProcessMove(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = TRUE;
  tf->text.selection_link = FALSE;
  ProcessBDragRelease(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}


static void
DeleteSelection(Widget w,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
  (void) TextFieldRemove(w, event);
}

static void
ClearSelection(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left = tf->text.prim_pos_left;
  XmTextPosition right = tf->text.prim_pos_right;
  int num_spaces = 0;
  XmAnyCallbackStruct cb;
  Boolean rep_result = False;
  char *spaces = NULL;
  XmString sp;

  if (left < right)
    num_spaces = (int)(right - left);
  else
    num_spaces = (int)(left - right);

  if (num_spaces <= 0)
    return;

  _XmTextFieldDrawInsertionPoint(tf, False);
  spaces = XtMalloc(num_spaces + 1);
  memset(spaces, ' ', num_spaces);
  spaces[num_spaces] = '\0';
  sp = XmStringCreateMultibyte(spaces, NULL);
  XtFree(spaces);

  rep_result = _XmTextFieldReplaceText(tf, (XEvent *)event, left, right,
                                       sp, num_spaces, False, True);

  if (rep_result) {
    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf), (XtPointer)&cb);
  }

  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
PageRight(Widget w,
	  XEvent *event,
	  char **params,
	  Cardinal *num_params)
{
  Position x, y;
  int length = 0, value;
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Dimension margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  TextFieldResetIC(w);
  length = FindPixelLength(tf, tf->text.xms_value);

  /* if widget is wider than text, ignore page-right action*/
  if (length <= (int)(tf->core.width - (2 * margin_width)))
    return;

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  SetAnchorBalancing(tf, TextF_CursorPosition(tf));
      }
  }

  GetXYFromPos(tf, TextF_CursorPosition(tf), &x, &y);

  if ((int)(length - ((int)(tf->core.width - (2 * margin_width)) -
		tf->text.h_offset) ) > (int)(tf->core.width - (2 * margin_width)))
    tf->text.h_offset -= tf->core.width - (2 * margin_width);
  else
    tf->text.h_offset = -(length - (tf->core.width - (2 * margin_width)));

  RedisplayText(tf, 0, tf->text.string_length);
  _XmTextFieldSetCursorPosition(tf, event, GetPosFromX(tf, x),
				True, True);

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  KeySelection(w, event, params, num_params);
      }
  }

  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
PageLeft(Widget w,
	 XEvent *event,
	 char **params,
	 Cardinal *num_params)
{
  Position x, y;
  int value;
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  int margin_width = (int)TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  TextFieldResetIC(w);

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  SetAnchorBalancing(tf, TextF_CursorPosition(tf));
      }
  }

  GetXYFromPos(tf, TextF_CursorPosition(tf), &x, &y);
  if (margin_width  <= tf->text.h_offset +
      ((int)tf->core.width - (2 * margin_width)))
    tf->text.h_offset = margin_width;
  else
    tf->text.h_offset += tf->core.width - (2 * margin_width);

  RedisplayText(tf, 0, tf->text.string_length);
  _XmTextFieldSetCursorPosition(tf, event, GetPosFromX(tf, x),
				True, True);

  if (*num_params > 0)
  {
      /* There is only one valid reptype value for this reptype, i.e.
	 "extend". A True return value means that parameter was "extend". */
      if (_XmConvertActionParamToRepTypeId((Widget) w,
			   XmRID_TEXTFIELD_EXTEND_MOVEMENT_ACTION_PARAMS,
			   params[0], False, &value) == True)
      {
	  KeySelection(w, event, params, num_params);
      }
  }

  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
CopyPrimary(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = False;
  tf->text.selection_link = False;

  /* perform the primary paste action */
  Stuff(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
CutPrimary(Widget w,
	   XEvent *event,
	   char **params,
	   Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = True;
  tf->text.selection_link = False;

  Stuff(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
LinkPrimary(Widget w,
	   XEvent *event,
	   char **params,
	   Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.selection_move = False;
  tf->text.selection_link = True;
  Stuff(w, event, params, num_params);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
SetAnchor(Widget w,
	  XEvent *event,
	  char **params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  tf->text.prim_anchor = TextF_CursorPosition(tf);
  (void) SetDestination(w, tf->text.prim_anchor, False, event->xkey.time);
  if (tf->text.has_primary) {
    _XmTextFieldStartSelection(tf, tf->text.prim_anchor,
			       tf->text.prim_anchor, event->xkey.time);
    if (tf->text.add_mode) {
      _XmTextFieldDrawInsertionPoint(tf, False);
      tf->text.add_mode = False;
      _XmTextFieldDrawInsertionPoint(tf, True);
    }
  }
}

static void
ToggleOverstrike(Widget w,
		 XEvent *event,
		 char **params,
		 Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.overstrike = !tf->text.overstrike;
  tf->text.refresh_ibeam_off = True;
  if (tf->text.overstrike)
    tf->text.cursor_width = tf->text.cursor_height >> 1;
  else {
    tf->text.cursor_width = 5;
    if (tf->text.cursor_height > 19)
      tf->text.cursor_width++;
  }
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
ToggleAddMode(Widget w,
	      XEvent *event,
	      char **params,
	      Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);

  tf->text.add_mode = !tf->text.add_mode;
  if (tf->text.add_mode &&
      (!tf->text.has_primary ||
       tf->text.prim_pos_left == tf->text.prim_pos_right))
    tf->text.prim_anchor = TextF_CursorPosition(tf);

  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
SelectAll(Widget w,
	  XEvent *event,
	  char **params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  TextFieldResetIC(w);
  _XmTextFieldDrawInsertionPoint(tf, False);
  if (tf->text.take_primary)
    _XmTextFieldStartSelection(tf, 0, tf->text.string_length,
			       event->xbutton.time);
  else
    SetSelection(tf, 0, tf->text.string_length, True);

  /* Call _XmTextFieldSetCursorPosition to force image gc to be updated
   * in case the i-beam is contained within the selection */

  tf->text.pending_off = False;

  _XmTextFieldSetCursorPosition(tf, NULL, TextF_CursorPosition(tf),
				False, False);
  tf->text.prim_anchor = 0;

  (void) SetDestination(w, TextF_CursorPosition(tf),
			False, event->xkey.time);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
DeselectAll(Widget w,
	    XEvent *event,
	    char **params,
	    Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  SetSelection(tf, TextF_CursorPosition(tf), TextF_CursorPosition(tf), True);
  tf->text.pending_off = True;
  _XmTextFieldSetCursorPosition(tf, event, TextF_CursorPosition(tf),
				True, True);
  tf->text.prim_anchor = TextF_CursorPosition(tf);
  (void) SetDestination(w, TextF_CursorPosition(tf),
			False, event->xkey.time);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
VoidAction(Widget w,
	   XEvent *event,
	   char **params,
	   Cardinal *num_params)
{
  /* Do Nothing */
}

static void
CutClipboard(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget)w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (TextF_Editable(tf) && tf->text.prim_pos_left != tf->text.prim_pos_right)
    (void) XmeClipboardSource(w, XmMOVE, event->xkey.time);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
CopyClipboard(Widget w,
	      XEvent *event,
	      char **params,
	      Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  _XmTextFieldDrawInsertionPoint(tf, False);
  if (tf->text.prim_pos_left != tf->text.prim_pos_right)
    (void) XmeClipboardSource(w, XmCOPY, event->xkey.time);
  (void) SetDestination(w, TextF_CursorPosition(tf), False, event->xkey.time);
  _XmTextFieldDrawInsertionPoint(tf, True);
}

static void
PasteClipboard(Widget w,
	       XEvent *event,
	       char **params,
	       Cardinal *num_params)
{
  _XmTextFieldDrawInsertionPoint((XmTextFieldWidget)w, False);
  ((XmTextFieldWidget)w)->text.selection_move = FALSE;
  ((XmTextFieldWidget)w)->text.selection_link = FALSE;
  XmeClipboardSink(w, XmCOPY, NULL);
  _XmTextFieldDrawInsertionPoint((XmTextFieldWidget)w, True);
}

Boolean
XmTextFieldPaste(Widget w)
{
  Boolean status;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  TextFieldResetIC(w);
  ((XmTextFieldWidget)w)->text.selection_move = FALSE;
  ((XmTextFieldWidget)w)->text.selection_link = FALSE;
  status = XmeClipboardSink(w, XmCOPY, NULL);

  _XmAppUnlock(app);
  return(status);
}

Boolean
XmTextFieldPasteLink(Widget w)
{
  Boolean status;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ((XmTextFieldWidget)w)->text.selection_move = FALSE;
  ((XmTextFieldWidget)w)->text.selection_link = TRUE;
  status = XmeClipboardSink(w, XmLINK, NULL);

  _XmAppUnlock(app);
  return(status);
}

static void
TraverseDown(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (tf->primitive.navigation_type == XmNONE && VerifyLeave(tf, event)) {
    tf->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_DOWN))
      tf->text.traversed = False;
  }
}


static void
TraverseUp(Widget w,
	   XEvent *event,
	   char **params,
	   Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (tf->primitive.navigation_type == XmNONE && VerifyLeave(tf, event)) {
    tf->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_UP))
      tf->text.traversed = False;
  }
}

static void
TraverseHome(Widget w,
	     XEvent *event,
	     char **params,
	     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  /* Allow the verification routine to control the traversal */
  if (tf->primitive.navigation_type == XmNONE && VerifyLeave(tf, event)) {
    tf->text.traversed = True;
    if (!_XmMgrTraversal(w, XmTRAVERSE_HOME))
      tf->text.traversed = False;
  }
}


static void
TraverseNextTabGroup(Widget w,
		     XEvent *event,
		     char **params,
		     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  /* Allow the verification routine to control the traversal */
  if (VerifyLeave(tf, event)) {
    XmTraversalDirection dir;
    XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    Boolean enable_button_tab = xm_dpy->display.enable_button_tab;

    dir = (enable_button_tab ?
	   XmTRAVERSE_GLOBALLY_FORWARD : XmTRAVERSE_NEXT_TAB_GROUP);

    tf->text.traversed = True;
    if (!_XmMgrTraversal(w, dir))
      tf->text.traversed = False;
  }
}


static void
TraversePrevTabGroup(Widget w,
		     XEvent *event,
		     char **params,
		     Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  /* Allow the verification routine to control the traversal */
  if (VerifyLeave(tf, event)) {
    XmTraversalDirection dir;
    XmDisplay xm_dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
    Boolean enable_button_tab = xm_dpy->display.enable_button_tab;

    dir = (enable_button_tab ?
	   XmTRAVERSE_GLOBALLY_BACKWARD : XmTRAVERSE_PREV_TAB_GROUP);

    tf->text.traversed = True;
    if (!_XmMgrTraversal(w, dir))
      tf->text.traversed = False;
  }
}


static void
TextEnter(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmAnyCallbackStruct cb;
  XRectangle xmim_area;
  XPoint xmim_point;

  /* Use != NotifyInferior along with event->xcrossing.focus to avoid
   * sending input method info if reason for the event is pointer moving
   * from TextF widget to over-the-spot window (case when over-the-spot
   * is child of TextF widget). */
  if (_XmGetFocusPolicy(w) != XmEXPLICIT && !(tf->text.has_focus) &&
      event->xcrossing.focus &&
      (event->xcrossing.detail != NotifyInferior)) {
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.blink_on = False;
    tf->text.has_focus = True;
    if (XtIsSensitive(w)) ChangeBlinkBehavior(tf, True);
    _XmTextFieldDrawInsertionPoint(tf, True);
    GetXYFromPos(tf, TextF_CursorPosition(tf), &xmim_point.x,
		 &xmim_point.y);
    (void)TextFieldGetDisplayRect((Widget)tf, &xmim_area);
    XmImVaSetFocusValues(w, XmNspotLocation, &xmim_point,
			 XmNarea, &xmim_area, NULL);
    cb.reason = XmCR_FOCUS;
    cb.event = event;
    XtCallCallbackList (w, tf->text.focus_callback, (XtPointer) &cb);
  }

  _XmPrimitiveEnter(w, event, params, num_params);
}


static void
TextLeave(Widget w,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  /* use detail!= NotifyInferior to handle focus change due to pointer
   * wandering into over-the-spot input window - we don't want to change
   * IM's focus state in this case. */
  if (_XmGetFocusPolicy(w) != XmEXPLICIT && tf->text.has_focus &&
      event->xcrossing.focus &&
      (event->xcrossing.detail != NotifyInferior)) {
    if (XtIsSensitive(w)) ChangeBlinkBehavior(tf, False);
    _XmTextFieldDrawInsertionPoint(tf, False);
    tf->text.has_focus = False;
    tf->text.blink_on = True;
    _XmTextFieldDrawInsertionPoint(tf, True);
    (void) VerifyLeave(tf, event);
    XmImUnsetFocus(w);
  }

  _XmPrimitiveLeave(w, event, params, num_params);
}

/****************************************************************
 *
 * Private definitions.
 *
 ****************************************************************/

/*
 * ClassPartInitialize sets up the fast subclassing for the widget.i
 * It also merges translation tables.
 */
static void
ClassPartInitialize(WidgetClass w_class)
{
  char *event_bindings;

  _XmFastSubclassInit (w_class, XmTEXT_FIELD_BIT);
  event_bindings = (char *)XtMalloc((unsigned) (strlen(EventBindings1) +
						strlen(EventBindings2) +
						strlen(EventBindings3) + 1));

  strcpy(event_bindings, EventBindings1);
  strcat(event_bindings, EventBindings2);
  strcat(event_bindings, EventBindings3);
  w_class->core_class.tm_table =
    (String) XtParseTranslationTable(event_bindings);
  XtFree(event_bindings);
}

/****************************************************************
 *
 * Private functions used in Initialize.
 *
 ****************************************************************/

/*
 * Verify that the resource settings are valid.  Print a warning
 * message and reset the s if the are invalid.
 */
static void
Validates(XmTextFieldWidget tf)
{
  XtPointer temp_ptr;

  if (TextF_CursorPosition(tf) < 0) {
    XmeWarning ((Widget)tf, MSG1);
    TextF_CursorPosition(tf) = 0;
  }

  if (TextF_Columns(tf) <= 0) {
    XmeWarning ((Widget)tf, MSG2);
    TextF_Columns(tf) = 20;
  }

  if (TextF_SelectionArray(tf) == NULL)
    TextF_SelectionArray(tf) = (XmTextScanType *) sarray;

  if (TextF_SelectionArrayCount(tf) <= 0)
    TextF_SelectionArrayCount(tf) = XtNumber(sarray);

  /*
   * Fix for HaL DTS 9841 - copy the selectionArray into dedicated memory.
   */
  temp_ptr = (XtPointer)TextF_SelectionArray(tf);
  TextF_SelectionArray(tf) =
    (XmTextScanType *)XtMalloc(TextF_SelectionArrayCount(tf) *
			       sizeof(XmTextScanType));
  memcpy((void *)TextF_SelectionArray(tf), (void *)temp_ptr,
	 (TextF_SelectionArrayCount(tf) * sizeof(XmTextScanType)));
  /*
   * End fix for HaL DTS 9841
   */
}

static void LoadFontMetrics(XmTextFieldWidget tf)
{
	XmString s;
	XmRenderTable rt = TextF_FontList(tf);

	if (!XmStringIsValid(tf->text.xms_value) || XmStringEmpty(tf->text.xms_value)) {
		s = XmStringCreate("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
		                   XmFONTLIST_DEFAULT_TAG);
	} else s = XmStringCopy(tf->text.xms_value);

	TextF_FontAscent(tf)  = XmStringBaseline(rt, s);
	TextF_FontDescent(tf) = XmStringHeight(rt, s) - XmStringBaseline(rt, s);
	tf->text.average_char_width = XmStringWidth(rt, s) / 62;
	XmStringFree(s);
}

/*
 * Initialize the s in the text fields instance record.
 */
static void
InitializeTextStruct(XmTextFieldWidget tf)
{
  /* Flag used in losing focus verification to indicate that a traversal
   * key was pressed.  Must be initialized to False.
   */
  XIMCallback xim_cb[5];        /* on the spot im callbacks */
  Arg args[11];                 /* To set initial values to input method */
  Cardinal n = 0;
  XPoint xmim_point;
  XRectangle xmim_area;
  tf->text.traversed = False;

  tf->text.add_mode = False;
  tf->text.has_focus = False;
  tf->text.blink_on = True;
  tf->text.cursor_on = 0;
  tf->text.has_rect = False;
  tf->text.has_primary = False;
  tf->text.take_primary = True;
  tf->text.has_secondary = False;
  tf->text.has_destination = False;
  tf->text.overstrike = False;
  tf->text.selection_move = False;
  tf->text.sel_start = False;
  tf->text.pending_off = True;
  tf->text.fontlist_created = False;
  tf->text.cancel = False;
  tf->text.extending = False;
  tf->text.prim_time = 0;
  tf->text.dest_time = 0;
  tf->text.select_id = 0;
  tf->text.select_pos_x = 0;
  tf->text.sec_extending = False;
  tf->text.sec_drag = False;
  tf->text.changed_visible = False;
  tf->text.refresh_ibeam_off = True;
  tf->text.in_setvalues = False;
  tf->text.do_resize = True;
  tf->text.have_inverted_image_gc = False;
  tf->text.margin_top = TextF_MarginHeight(tf);
  tf->text.margin_bottom = TextF_MarginHeight(tf);
  tf->text.programmatic_highlights = False;
  tf->text.alignment = XmALIGNMENT_BEGINNING;

  if (!TextF_FontList(tf))
    TextF_FontList(tf) = XmeGetDefaultRenderTable((Widget)tf, XmTEXT_FONTLIST);
  TextF_FontList(tf) = XmRenderTableCopy(TextF_FontList(tf), NULL, 0);
  LoadFontMetrics(tf);

  tf->text.gc = NULL;
  tf->text.image_gc = NULL;
  tf->text.save_gc = NULL;
  tf->text.cursor_gc = NULL;

  tf->text.h_offset = (TextF_MarginWidth(tf) +
		       tf->primitive.shadow_thickness +
		       tf->primitive.highlight_thickness);

  if (TextF_CursorPosition(tf) > (XmTextPosition)tf->text.string_length)
    TextF_CursorPosition(tf) = (XmTextPosition)tf->text.string_length;

  tf->text.orig_left = tf->text.orig_right = tf->text.prim_pos_left =
    tf->text.prim_pos_right = tf->text.prim_anchor = TextF_CursorPosition(tf);

  tf->text.sec_pos_left = tf->text.sec_pos_right =
    tf->text.sec_anchor = TextF_CursorPosition(tf);

  tf->text.cursor_height = tf->text.cursor_width = 0;
  tf->text.stipple_tile = _XmGetInsensitiveStippleBitmap((Widget) tf);

  tf->text.add_mode_cursor = XmUNSPECIFIED_PIXMAP;
  tf->text.cursor = XmUNSPECIFIED_PIXMAP;
  tf->text.ibeam_off = XmUNSPECIFIED_PIXMAP;
  tf->text.image_clip = XmUNSPECIFIED_PIXMAP;

  tf->text.last_time = 0;

  tf->text.sarray_index = 0;

  /* Initialize highlight elements */
  tf->text.highlight.number = tf->text.highlight.maximum = 1;
  tf->text.highlight.list =
    (_XmHighlightRec *)XtMalloc((unsigned)sizeof(_XmHighlightRec));
  tf->text.highlight.list[0].position = 0;
  tf->text.highlight.list[0].mode = XmHIGHLIGHT_NORMAL;

  tf->text.timer_id = (XtIntervalId)0;
  tf->text.drag_id = (XtIntervalId)0;
  tf->text.transfer_action = NULL;

  XmTextFieldSetEditable((Widget)tf, TextF_Editable(tf));

  if (TextF_Editable(tf)) {
    XmImRegister((Widget)tf, 0);
    GetXYFromPos(tf, TextF_CursorPosition(tf), &xmim_point.x, &xmim_point.y);
    (void)TextFieldGetDisplayRect((Widget)tf, &xmim_area);
    n = 0;
    XtSetArg(args[n], XmNfontList, TextF_FontList(tf)); n++;
    XtSetArg(args[n], XmNbackground, tf->core.background_pixel); n++;
    XtSetArg(args[n], XmNforeground, tf->primitive.foreground); n++;
    XtSetArg(args[n], XmNbackgroundPixmap,tf->core.background_pixmap);n++;
    XtSetArg(args[n], XmNspotLocation, &xmim_point); n++;
    XtSetArg(args[n], XmNarea, &xmim_area); n++;
    XtSetArg(args[n], XmNlineSpace,
	     TextF_FontAscent(tf) + TextF_FontDescent(tf)); n++;

    /*
     * On the spot support. Register preedit callbacks during initialize.
     */
    xim_cb[0].client_data = (XPointer)tf;
    xim_cb[0].callback = (XIMProc)PreeditStart;
    xim_cb[1].client_data = (XPointer)tf;
    xim_cb[1].callback = (XIMProc)PreeditDone;
    xim_cb[2].client_data = (XPointer)tf;
    xim_cb[2].callback = (XIMProc)PreeditDraw;
    xim_cb[3].client_data = (XPointer)tf;
    xim_cb[3].callback = (XIMProc)PreeditCaret;
    XtSetArg(args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
    XtSetArg(args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
    XtSetArg(args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
    XtSetArg(args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;

    XmImSetValues((Widget)tf, args, n);
  }

  /*
   * Initialize on the spot data in tf structure
   */
  tf->text.onthespot = (OnTheSpotData)XtMalloc(sizeof(OnTheSpotDataRec));
  tf->text.onthespot->start = tf->text.onthespot->end =
    tf->text.onthespot->cursor = 0;
  tf->text.onthespot->under_preedit = False;
  tf->text.onthespot->under_verify_preedit = False;
  tf->text.onthespot->verify_commit = False;
}

/*
 * Get the graphics context for filling the background, and for drawing
 * and inverting text.  Used a unique pixmap so all text field widgets
 * share common GCs.
 */
static void
LoadGCs(XmTextFieldWidget tf,
        Pixel background,
        Pixel foreground)
{
  XGCValues values;
  unsigned long valueMask = (GCFunction | GCForeground | GCBackground |
			     GCGraphicsExposures);
  unsigned long dynamicMask = GCClipMask;
  unsigned long unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;

  /*
   * Get GC for saving area under the cursor.
   */
  values.function = GXcopy;
  values.foreground = tf->primitive.foreground;
  values.background = tf->core.background_pixel;
  values.graphics_exposures = (Bool) False;
  if (tf->text.save_gc != NULL)
    XtReleaseGC((Widget)tf, tf->text.save_gc);
  tf->text.save_gc = XtAllocateGC((Widget) tf, tf->core.depth, valueMask,
				  &values, dynamicMask, unusedMask);

  /*
   * Get GC for drawing text.
   */
  values.foreground = foreground ^ background;
  values.background = 0;
  values.graphics_exposures = (Bool) True;
  if (tf->text.gc != NULL)
    XtReleaseGC((Widget)tf, tf->text.gc);
  dynamicMask |= GCForeground | GCBackground | GCFillStyle | GCStipple;
  tf->text.gc = XtAllocateGC((Widget) tf, tf->core.depth, valueMask,
			     &values, dynamicMask, 0);

  /* Create a temporary GC - change it later in make IBEAM */
  valueMask |= GCStipple | GCFillStyle;
  values.stipple = tf->text.stipple_tile;
  values.fill_style = FillStippled;
  values.graphics_exposures = (Bool) False;
  if (tf->text.image_gc != NULL)
    XtReleaseGC((Widget)tf, tf->text.image_gc);
  dynamicMask |= (GCTileStipXOrigin | GCTileStipYOrigin | GCFunction);
  tf->text.image_gc = XtAllocateGC((Widget) tf, tf->core.depth, valueMask,
				   &values, dynamicMask, 0);
}

static void
MakeIBeamOffArea(XmTextFieldWidget tf,
                 Dimension width,
                 Dimension height)
{
  Display *dpy = XtDisplay(tf);
  Screen  *screen = XtScreen(tf);

  /* Create a pixmap for storing the screen data where the I-Beam will
   * be painted */

  tf->text.ibeam_off = XCreatePixmap(dpy, RootWindowOfScreen(screen), width,
				     height, tf->core.depth);
  tf->text.refresh_ibeam_off = True;
}

static Pixmap
FindPixmap(
    Screen *screen,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth )
{
    XmAccessColorDataRec acc_color_rec;

    acc_color_rec.foreground = foreground;
    acc_color_rec.background = background;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL;
    return  _XmGetColoredPixmap(screen, image_name,
				&acc_color_rec, depth, True);
}

static void
MakeIBeamStencil(XmTextFieldWidget tf,
		 int line_width)
{
  Screen *screen = XtScreen(tf);
  char pixmap_name[64];
  XGCValues values;
  unsigned long valueMask;

  sprintf(pixmap_name, "_XmText_%d_%d", tf->text.cursor_height, line_width);
  tf->text.cursor = FindPixmap(screen, pixmap_name, 1, 0, 1);

  if (tf->text.cursor == XmUNSPECIFIED_PIXMAP) {
    Display *dpy = XtDisplay(tf);
    XSegment segments[3];

    /* Create a pixmap for the I-Beam stencil */
    tf->text.cursor = XCreatePixmap(dpy, XtWindow(tf), tf->text.cursor_width,
				    tf->text.cursor_height, 1);


    /* Fill in the stencil with a solid in preparation
     * to "cut out" the I-Beam
     */
    values.foreground = 0;
    values.line_width = 0;
    values.fill_style = FillSolid;
    values.function = GXcopy;
    valueMask = GCForeground | GCLineWidth | GCFillStyle | GCFunction;
    XChangeGC(dpy, tf->text.cursor_gc, valueMask, &values);

    XFillRectangle(dpy, tf->text.cursor, tf->text.cursor_gc, 0, 0,
		   tf->text.cursor_width, tf->text.cursor_height);

    /* Change the GC for use in "cutting out" the I-Beam shape */
    values.foreground = 1;
    values.line_width = line_width;
    XChangeGC(dpy, tf->text.cursor_gc, GCForeground | GCLineWidth, &values);

    /* Draw the segments of the I-Beam */
    /* 1st segment is the top horizontal line of the 'I' */
    segments[0].x1 = 0;
    segments[0].y1 = line_width - 1;
    segments[0].x2 = tf->text.cursor_width;
    segments[0].y2 = line_width - 1;

    /* 2nd segment is the bottom horizontal line of the 'I' */
    segments[1].x1 = 0;
    segments[1].y1 = tf->text.cursor_height - 1;
    segments[1].x2 = tf->text.cursor_width;
    segments[1].y2 = tf->text.cursor_height - 1;

    /* 3rd segment is the vertical line of the 'I' */
    segments[2].x1 = tf->text.cursor_width >> 1;
    segments[2].y1 = line_width;
    segments[2].x2 = tf->text.cursor_width >> 1;
    segments[2].y2 = tf->text.cursor_height - 1;

    /* Draw the segments onto the cursor */
    XDrawSegments(dpy, tf->text.cursor, tf->text.cursor_gc, segments, 3);

    /* Install the cursor for pixmap caching */
    (void) _XmCachePixmap(tf->text.cursor, XtScreen(tf), pixmap_name, 1, 0,
			    1, tf->text.cursor_width, tf->text.cursor_height);
  }

  /* Get/create the image_gc used to paint the I-Beam */

  valueMask = (GCStipple | GCForeground | GCBackground | GCFillStyle);
  if (!tf->text.overstrike) {
    values.foreground = tf->primitive.foreground;
    values.background = tf->core.background_pixel;
  } else
    values.background = values.foreground =
      tf->core.background_pixel ^ tf->primitive.foreground;
  values.stipple = tf->text.cursor;
  values.fill_style = FillStippled;
  XChangeGC(XtDisplay(tf), tf->text.image_gc, valueMask, &values);

}


/* The IBeam Stencil must have already been created before this routine
 * is called.
 */

static void
MakeAddModeCursor(XmTextFieldWidget tf,
		  int line_width)
{
  Screen *screen = XtScreen(tf);
  char pixmap_name[64];

  sprintf(pixmap_name, "_XmText_AddMode_%d_%d",
	  tf->text.cursor_height, line_width);

  tf->text.add_mode_cursor = FindPixmap(screen, pixmap_name, 1, 0, 1);

  if (tf->text.add_mode_cursor == XmUNSPECIFIED_PIXMAP) {
    XtGCMask  valueMask;
    XGCValues values;
    Display *dpy = XtDisplay(tf);

    tf->text.add_mode_cursor = XCreatePixmap(dpy, XtWindow(tf),
					     tf->text.cursor_width,
					     tf->text.cursor_height, 1);

    values.function = GXcopy;
    valueMask = GCFunction;
    XChangeGC(dpy, tf->text.cursor_gc, valueMask, &values);

    XCopyArea(dpy, tf->text.cursor, tf->text.add_mode_cursor,
	      tf->text.cursor_gc, 0, 0,
	      tf->text.cursor_width, tf->text.cursor_height, 0, 0);

    valueMask = (GCForeground | GCBackground | GCTile | GCFillStyle |
		 GCFunction | GCTileStipXOrigin);

    values.function = GXand;
    values.tile = tf->text.stipple_tile;
    values.fill_style = FillTiled;
    values.ts_x_origin = -1;
    values.foreground = tf->primitive.foreground;
    values.background = tf->core.background_pixel;

    XChangeGC(dpy, tf->text.cursor_gc, valueMask, &values);

    XFillRectangle(dpy, tf->text.add_mode_cursor, tf->text.cursor_gc,
		   0, 0, tf->text.cursor_width, tf->text.cursor_height);

    /* Install the pixmap for pixmap caching */
    _XmCachePixmap(tf->text.add_mode_cursor,
		     XtScreen(tf), pixmap_name, 1, 0,
		     1, tf->text.cursor_width, tf->text.cursor_height);
  }
}


static void
MakeCursors(XmTextFieldWidget tf)
{
  Screen *screen = XtScreen(tf);
  int line_width = 1;
  int oldwidth = tf->text.cursor_width;
  int oldheight = tf->text.cursor_height;

  if (!XtIsRealized((Widget) tf)) return;

  tf->text.cursor_width = 5;
  tf->text.cursor_height = TextF_FontAscent(tf) + TextF_FontDescent(tf);

  /* setup parameters to make a thicker I-Beam */
  if (tf->text.cursor_height > 19) {
    tf->text.cursor_width++;
    line_width = 2;
  }

  if (tf->text.cursor == XmUNSPECIFIED_PIXMAP ||
      tf->text.add_mode_cursor == XmUNSPECIFIED_PIXMAP ||
      tf->text.ibeam_off == XmUNSPECIFIED_PIXMAP ||
      oldheight != tf->text.cursor_height ||
      oldwidth != tf->text.cursor_width) {

    if (tf->text.cursor_gc == NULL) {
      unsigned long valueMask = 0;
      XGCValues values;
      unsigned long dynamicMask =
	GCForeground | GCLineWidth | GCTile | GCFillStyle |
	GCBackground | GCFunction | GCTileStipXOrigin;

      tf->text.cursor_gc = XtAllocateGC((Widget)tf, 1, valueMask, &values,
					dynamicMask, 0);
    }

    /* Remove old ibeam off area */
    if (tf->text.ibeam_off != XmUNSPECIFIED_PIXMAP)
      XFreePixmap(XtDisplay((Widget)tf), tf->text.ibeam_off);

    /* Remove old insert cursor */
    if (tf->text.cursor != XmUNSPECIFIED_PIXMAP) {
      (void) XmDestroyPixmap(screen, tf->text.cursor);
      tf->text.cursor = XmUNSPECIFIED_PIXMAP;
    }

    /* Remove old add mode cursor */
    if (tf->text.add_mode_cursor != XmUNSPECIFIED_PIXMAP) {
      (void) XmDestroyPixmap(screen, tf->text.add_mode_cursor);
      tf->text.add_mode_cursor = XmUNSPECIFIED_PIXMAP;
    }

    /* Create area in which to save text located underneath I beam */
    MakeIBeamOffArea(tf, MAX(tf->text.cursor_height>>1,tf->text.cursor_height),
		     tf->text.cursor_height);

    /* Create a new i-beam cursor */
    MakeIBeamStencil(tf, line_width);

    /* Create a new add_mode cursor */
    MakeAddModeCursor(tf, line_width);
  }

  if (tf->text.overstrike)
    tf->text.cursor_width = tf->text.cursor_height >> 1;
}

static void
DragProcCallback(Widget w,
		 XtPointer client,
		 XtPointer call)
{
  enum { XmACOMPOUND_TEXT, XmATEXT, XmAUTF8_STRING, NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSTEXT, XmSUTF8_STRING };

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
  targets[4] = atoms[XmAUTF8_STRING];

  drag_cont = cb->dragContext;

  n = 0;
  XtSetArg(args[n], XmNexportTargets, &exp_targets); n++;
  XtSetArg(args[n], XmNnumExportTargets, &num_exp_targets); n++;
  XtGetValues(drag_cont, args, n);

  switch(cb->reason) {
  case XmCR_DROP_SITE_ENTER_MESSAGE:
    if (XmTargetsAreCompatible(XtDisplay(drag_cont), exp_targets,
			       num_exp_targets, targets, 4))
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

  if (cb -> dropSiteStatus == XmVALID_DROP_SITE) {
    if (cb -> operation != XmDROP_COPY &&
	cb -> operation != XmDROP_MOVE)
      cb -> dropSiteStatus = XmINVALID_DROP_SITE;
  }
}

static void
RegisterDropSite(Widget w)
{
  enum { XmACOMPOUND_TEXT, XmATEXT, XmAUTF8_STRING, NUM_ATOMS };
  static char *atom_names[] = { XmSCOMPOUND_TEXT, XmSTEXT, XmSUTF8_STRING };

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
  targets[4] = atoms[XmAUTF8_STRING];

  n = 0;
  XtSetArg(args[n], XmNimportTargets, targets); n++;
  XtSetArg(args[n], XmNnumImportTargets, 5); n++;
  XtSetArg(args[n], XmNdragProc, DragProcCallback); n++;
  XmeDropSink(w, args, n);
}

/*
 * Initialize
 *    Intializes the text data and ensures that the data in new
 * is valid.
 */

static void
Initialize(Widget request,
	   Widget new_w,
	   ArgList args,
	   Cardinal *num_args)
{
  Cardinal i;
  XmTextFieldWidget req_tf = (XmTextFieldWidget) request;
  XmTextFieldWidget new_tf = (XmTextFieldWidget) new_w;
  Dimension width, height;
  XmString val = NULL, wcs = NULL;

  Validates(new_tf);
  InitializeTextStruct(new_tf);

  LoadGCs(new_tf, new_tf->core.background_pixel,
	  new_tf->primitive.foreground);

  ComputeSize(new_tf, &width, &height);

  if (req_tf->core.width == 0)
    new_tf->core.width = width;
  if (req_tf->core.height == 0)
    new_tf->core.height = height;

  RegisterDropSite(new_w);

  if (new_tf->text.verify_bell == (Boolean) XmDYNAMIC_BOOL) {
    if (_XmGetAudibleWarning(new_w) == XmBELL)
      new_tf->text.verify_bell = True;
    else
      new_tf->text.verify_bell = False;
  }

  /* If we have XmString, use it */
  if (req_tf->text.xms_value) {
    new_tf->text.xms_value     = req_tf->text.xms_value;
    new_tf->text.string_length = XmStringLen(new_tf->text.xms_value);
    return;
  } else if (new_tf->text.xms_value) return;

  /**
   * Probe for the values given by our synthetics, giving priority to
   * the XmNvalueWcs
   */
  for (i = 0; i < *num_args; i++) {
    if (args[i].name == XmNvalue || !strcmp(args[i].name, XmNvalue)) {
      if (val) {
        XmStringFree(val);
        val = NULL;
      }

      if (XmStringIsValid((XmString)args[i].value))
        val = (XmString)args[i].value;
    }

    if (args[i].name == XmNvalueWcs || !strcmp(args[i].name, XmNvalueWcs)) {
      if (wcs) {
        XmStringFree(wcs);
        wcs = NULL;
      }

      if (XmStringIsValid((XmString)args[i].value))
        wcs = (XmString)args[i].value;
    }
  }

  if (wcs) {
    new_tf->text.xms_value     = wcs;
    new_tf->text.string_length = XmStringLen(wcs);
    if (val) XmStringFree(val);
  } else if (val) {
    new_tf->text.xms_value     = val;
    new_tf->text.string_length = XmStringLen(val);
  }
}

static void
Realize(Widget w,
        XtValueMask *valueMask,
        XSetWindowAttributes *attributes)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Arg args[11];                 /* To set initial values to input method */
  XIMCallback xim_cb[5];        /* on the spot im callback data */
  Cardinal n = 0;

  XtCreateWindow(w, (unsigned int) InputOutput,
		 (Visual *) CopyFromParent, *valueMask, attributes);
  MakeCursors(tf);

  if (TextF_Editable(tf)){
  /*
   * On the spot support. Register preedit callbacks.
   */
    xim_cb[0].client_data = (XPointer)tf;
    xim_cb[0].callback = (XIMProc)PreeditStart;
    xim_cb[1].client_data = (XPointer)tf;
    xim_cb[1].callback = (XIMProc)PreeditDone;
    xim_cb[2].client_data = (XPointer)tf;
    xim_cb[2].callback = (XIMProc)PreeditDraw;
    xim_cb[3].client_data = (XPointer)tf;
    xim_cb[3].callback = (XIMProc)PreeditCaret;
    XtSetArg(args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
    XtSetArg(args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
    XtSetArg(args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
    XtSetArg(args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;
    XmImSetValues((Widget)tf, args, n);
  }
}

static void
Destroy(Widget wid)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) wid;
  Widget dest = XmGetDestination(XtDisplay(wid));

  if (dest == wid)
    _XmSetDestination(XtDisplay(wid), NULL);

  if (tf->text.timer_id)
    XtRemoveTimeOut(tf->text.timer_id);

  if (tf->text.drag_id)
    XtRemoveTimeOut(tf->text.drag_id);

  if (tf->text.select_id)
    {
    XtRemoveTimeOut(tf->text.select_id);
    tf->text.select_id = 0;
    }

  if (tf->text.transfer_action) {
    XtFree((char *)tf->text.transfer_action->event);
    XtFree((char *)tf->text.transfer_action);
  }

  XtReleaseGC(wid, tf->text.gc);
  XtReleaseGC(wid, tf->text.image_gc);
  XtReleaseGC(wid, tf->text.save_gc);
  XtReleaseGC(wid, tf->text.cursor_gc);

  XtFree((char *)tf->text.highlight.list);

  XmFontListFree((XmFontList)TextF_FontList(tf));

  if (tf->text.add_mode_cursor != XmUNSPECIFIED_PIXMAP)
    (void) XmDestroyPixmap(XtScreen(tf), tf->text.add_mode_cursor);

  if (tf->text.cursor != XmUNSPECIFIED_PIXMAP)
    (void) XmDestroyPixmap(XtScreen(tf), tf->text.cursor);

  if (tf->text.ibeam_off != XmUNSPECIFIED_PIXMAP)
    XFreePixmap(XtDisplay((Widget)tf), tf->text.ibeam_off);

  if (tf->text.onthespot != NULL)
    XtFree((char *)tf->text.onthespot);

  /*
   * Fix for HaL DTS 9841 - release the data for the selectionArray.
   */
  XtFree((char *)TextF_SelectionArray(tf));

  XmImUnregister(wid);
  XmStringFree(tf->text.xms_value);
}

static void
Resize(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  int text_width, new_width, offset;

  tf->text.do_resize = False;

#ifdef AS_TEXTFIELD
  tf->text.h_offset = TextF_MarginWidth(tf) + tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
#else
  new_width = tf->core.width - (2 * (TextF_MarginWidth(tf) +
				     tf->primitive.shadow_thickness +
				     tf->primitive.highlight_thickness));
  offset = tf->text.h_offset - (TextF_MarginWidth(tf) +
				tf->primitive.shadow_thickness +
				tf->primitive.highlight_thickness);

  text_width = FindPixelLength(tf, tf->text.xms_value);
  if (text_width - new_width < -offset)
  {
    if (text_width - new_width >= 0)
    {
      tf->text.h_offset = (new_width - text_width) + TextF_MarginWidth(tf) +
	tf->primitive.shadow_thickness +
	tf->primitive.highlight_thickness;
  }
    else
    {
      tf->text.h_offset = TextF_MarginWidth(tf) +
	tf->primitive.shadow_thickness +
	tf->primitive.highlight_thickness;
  }
}

#endif

  tf->text.refresh_ibeam_off = True;

  (void) AdjustText(tf, TextF_CursorPosition(tf), True);

  tf->text.do_resize = True;
}


/************************************************************************
 *
 *  QueryGeometry
 *
 ************************************************************************/
static XtGeometryResult
QueryGeometry(Widget widget,
	      XtWidgetGeometry *intended,
	      XtWidgetGeometry *desired)
{
  /* this function deals with resizeWidth False */
  ComputeSize((XmTextFieldWidget) widget,
	      &desired->width, &desired->height);

  return XmeReplyToQueryGeometry(widget, intended, desired);
}


/*
 * Redisplay will redraw shadows, borders, and text.
 */
static void
TextFieldExpose(Widget w,
		XEvent *event,
		Region region)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XGCValues values;


  if (event->xany.type != Expose) return;

  tf->text.do_resize = False;

  /* I can get here even though the widget isn't visible (i.e. my parent is
   * sized so that I have nothing visible.  In this case, capturing the putback
   * area yields garbage...  And if this area is not in an area where text
   * will be drawn (i.e. forcing something new/valid to be there next time I
   * go to capture it) the garbage persists.  To prevent this, initialize the
   * putback area and then update it to a solid background color.
   */

  tf->text.refresh_ibeam_off = False;
  values.clip_mask = None;
  values.foreground = tf->core.background_pixel;
  XChangeGC(XtDisplay(w), tf->text.save_gc, GCForeground|GCClipMask, &values);
  XFillRectangle(XtDisplay(w), tf->text.ibeam_off, tf->text.save_gc, 0, 0,
		 tf->text.cursor_width, tf->text.cursor_height);
  values.foreground = tf->primitive.foreground;
  XChangeGC(XtDisplay(w), tf->text.save_gc, GCForeground, &values);

  _XmTextFieldDrawInsertionPoint(tf, False);

  if (XtIsRealized((Widget)tf)) {
    if (tf->primitive.shadow_thickness > 0)
      XmeDrawShadows(XtDisplay(tf), XtWindow(tf),
		     tf->primitive.bottom_shadow_GC,
		     tf->primitive.top_shadow_GC,
		     (int) tf->primitive.highlight_thickness,
		     (int) tf->primitive.highlight_thickness,
		     (int) (tf->core.width -
			    (2 * tf->primitive.highlight_thickness)),
		     (int) (tf->core.height -
			    (2 * tf->primitive.highlight_thickness)),
		     (int) tf->primitive.shadow_thickness,
		     XmSHADOW_OUT);


    if (tf->primitive.highlighted) {
      if (((XmTextFieldWidgetClass)XtClass(tf))
	  ->primitive_class.border_highlight) {
	(*((XmTextFieldWidgetClass) XtClass(tf))
	 ->primitive_class.border_highlight)((Widget) tf);
      }
    } else {
      if (((XmTextFieldWidgetClass) XtClass(tf))
	  ->primitive_class.border_unhighlight) {
	(*((XmTextFieldWidgetClass) XtClass(tf))
	 ->primitive_class.border_unhighlight)((Widget) tf);
      }
    }

    RedisplayText(tf, 0, tf->text.string_length);
  }

  tf->text.refresh_ibeam_off = True;

  _XmTextFieldDrawInsertionPoint(tf, True);

  tf->text.do_resize = True;
}

/*
 *
 * SetValues
 *    Checks the new text data and ensures that the data is valid.
 * Invalid values will be rejected and changed back to the old
 * values.
 *
 */
static Boolean
SetValues(Widget old,
	  Widget request,
	  Widget new_w,
	  ArgList args,
	  Cardinal *num_args)
{
  Cardinal i;
  XmTextFieldWidget new_tf = (XmTextFieldWidget) new_w;
  XmTextFieldWidget old_tf = (XmTextFieldWidget) old;
  Boolean cursor_pos_set = False;
  Boolean new_size = False;
  Boolean redisplay = False;
  Boolean redisplay_text = False;
  Boolean new_font = False;
  Boolean mod_ver_ret = False;
  Dimension new_width = new_tf->core.width;
  Dimension new_height = new_tf->core.height;
  Arg im_args[10];
  XPoint xmim_point;
  XRectangle xmim_area;
  XmTextPosition from_position = 0, new_position = 0, newInsert;
  XmString val = NULL, wcs = NULL;
  int n = 0;
  XmAnyCallbackStruct cb;

  if (new_w->core.being_destroyed) return False;
  TextFieldResetIC(old);

  new_tf->text.in_setvalues = True;
  new_tf->text.redisplay = False;

  /* If new cursor position, copy the old cursor pos to the new widget
   * so that when we turn off the i-beam, the current location (old
   * widget) is used, but the new i-beam parameters (on/off, state, ...)
   * are utilized.  Then move the cursor.  Otherwise, just turn off
   * the i-beam. */

  if (TextF_CursorPosition(new_tf) != TextF_CursorPosition(old_tf)) {
    new_position = TextF_CursorPosition(new_tf);
    TextF_CursorPosition(new_tf) = TextF_CursorPosition(old_tf);
    _XmTextFieldDrawInsertionPoint(old_tf, False);
    new_tf->text.blink_on = old_tf->text.blink_on;
    new_tf->text.cursor_on = old_tf->text.cursor_on;
    _XmTextFieldSetCursorPosition(new_tf, NULL, new_position,
				  True, True);
    (void) SetDestination(new_w, TextF_CursorPosition(new_tf), False,
			  XtLastTimestampProcessed(XtDisplay(new_w)));
    cursor_pos_set = True;
  } else {
    for (i = 0; i < *num_args; i++)
      if (strcmp(args[i].name, XmNcursorPosition) == 0) {
	cursor_pos_set = True;
	new_position = TextF_CursorPosition(new_tf);
	break;
      }

    _XmTextFieldDrawInsertionPoint(old_tf, False);
    new_tf->text.blink_on = old_tf->text.blink_on;
    new_tf->text.cursor_on = old_tf->text.cursor_on;
  }

  if (!XtIsSensitive(new_w) &&
      new_tf->text.has_destination) {
    (void) SetDestination(new_w, TextF_CursorPosition(new_tf),
			  True, XtLastTimestampProcessed(XtDisplay(new_w)));
  }

  if (TextF_SelectionArray(new_tf) == NULL)
    TextF_SelectionArray(new_tf) = TextF_SelectionArray(old_tf);

  if (TextF_SelectionArrayCount(new_tf) <= 0)
    TextF_SelectionArrayCount(new_tf) = TextF_SelectionArrayCount(old_tf);

  /*
   * Fix for HaL DTS 9841 - If the new and old selectionArrays do not match,
   *			  free the old array and then copy the new array.
   */
  if (TextF_SelectionArray(new_tf) != TextF_SelectionArray(old_tf)) {
    XtPointer temp_ptr;

    XtFree((char *)TextF_SelectionArray(old_tf));
    temp_ptr = (XtPointer)TextF_SelectionArray(new_tf);
    TextF_SelectionArray(new_tf) =
      (XmTextScanType *)XtMalloc(TextF_SelectionArrayCount(new_tf) *
				 sizeof(XmTextScanType));
    memcpy((void *)TextF_SelectionArray(new_tf), (void *)temp_ptr,
	   (TextF_SelectionArrayCount(new_tf) * sizeof(XmTextScanType)));
  }
  /*
   * End fix for HaL DTS 9841
   */


  /* Make sure the new_tf cursor position is a valid value.
   */
  if (TextF_CursorPosition(new_tf) < 0) {
    XmeWarning (new_w, MSG1);
    TextF_CursorPosition(new_tf) = TextF_CursorPosition(old_tf);
    cursor_pos_set = False;
  }

  if (TextF_FontList(new_tf) != TextF_FontList(old_tf)) {
    new_font = True;
    if (!TextF_FontList(new_tf))
      TextF_FontList(new_tf) = XmeGetDefaultRenderTable(new_w, XmTEXT_FONTLIST);
    TextF_FontList(new_tf) = XmRenderTableCopy(TextF_FontList(new_tf), NULL, 0);
    LoadFontMetrics(new_tf);
    XtSetArg(im_args[n], XmNfontList, TextF_FontList(new_tf)); n++;
    redisplay = True;
  }

  /* OSF says:  if XmNvalueWcs set, it overrides multibyte */
  /* Probe for our synthetic values */
  for (i = 0; i < *num_args; i++) {
    if (args[i].name == XmNvalue || !strcmp(args[i].name, XmNvalue)) {
      if (val) {
        XmStringFree(val);
        val = NULL;
      }

      if (XmStringIsValid((XmString)args[i].value))
        val = (XmString)args[i].value;
    }

    if (args[i].name == XmNvalueWcs || !strcmp(args[i].name, XmNvalueWcs)) {
      if (wcs) {
        XmStringFree(wcs);
        wcs = NULL;
      }

      if (XmStringIsValid((XmString)args[i].value))
        wcs = (XmString)args[i].value;
    }
  }

  /* XmString didn't change, but these might have */
  if (new_tf->text.xms_value == old_tf->text.xms_value) {
    if (wcs) {
      new_tf->text.xms_value = wcs;
      if (val) XmStringFree(val);
    } else if (val) {
      new_tf->text.xms_value = val;
    }
  } else {
    XmStringFree(wcs);
    XmStringFree(val);
  }

  if (new_tf->text.xms_value != old_tf->text.xms_value) {
    from_position = 0;
    new_position  = old_tf->text.string_length;
    new_tf->text.string_length = XmStringLen(new_tf->text.xms_value);

    if (!ModifyVerify(new_tf, NULL, &from_position, &new_position,
                      &new_tf->text.xms_value, &new_tf->text.string_length,
                      &newInsert)) {
      if (new_tf->text.verify_bell)
        XBell(XtDisplayOfObject((Widget)new_tf), 0);
      XmStringFree(new_tf->text.xms_value);
      new_tf->text.xms_value     = old_tf->text.xms_value;
      new_tf->text.string_length = old_tf->text.string_length;
    } else {
      XmStringFree(old_tf->text.xms_value);

      doSetHighlight(new_w, new_tf->text.prim_pos_left,
                     new_tf->text.prim_pos_right,
                     XmHIGHLIGHT_NORMAL);
      new_tf->text.pending_off = True;

      /**
       * if new_position was > old_tf->text.string_length, last time
       * the SetCursorPosition didn't take.
       */
      if (!cursor_pos_set || new_position > (XmTextPosition)old_tf->text.string_length) {
        _XmTextFieldSetCursorPosition(new_tf, NULL, new_position,
                                      True, False);
        if (new_tf->text.has_destination)
          SetDestination(new_w, TextF_CursorPosition(new_tf), False,
                         XtLastTimestampProcessed(XtDisplay(new_w)));
      }

      if (!new_tf->text.do_resize || !TextF_ResizeWidth(new_tf)) {
        new_tf->text.h_offset = TextF_MarginWidth(new_tf)          +
                                new_tf->primitive.shadow_thickness +
                                new_tf->primitive.highlight_thickness;
        if (!AdjustText(new_tf, TextF_CursorPosition(new_tf), False))
          redisplay_text = True;
      } else AdjustSize(new_tf);

      memset(&cb, 0, sizeof cb);
      cb.reason = XmCR_VALUE_CHANGED;
      XtCallCallbackList(new_w, TextF_ValueChangedCallback(new_tf), (XtPointer)&cb);
    }
  }

  if (new_tf->primitive.foreground != old_tf->primitive.foreground ||
      TextF_FontList(new_tf)!= TextF_FontList(old_tf) ||
      new_tf->core.background_pixel != old_tf->core.background_pixel) {
    LoadGCs(new_tf, new_tf->primitive.foreground,
	    new_tf->core.background_pixel);
    MakeCursors(new_tf);
    redisplay = True;
    XtSetArg(im_args[n], XmNbackground, new_tf->core.background_pixel); n++;
    XtSetArg(im_args[n], XmNforeground, new_tf->primitive.foreground); n++;
  }

  if (new_tf->text.has_focus && XtIsSensitive((Widget)new_tf) &&
      TextF_BlinkRate(new_tf) != TextF_BlinkRate(old_tf)) {

    if (TextF_BlinkRate(new_tf) == 0) {
      new_tf->text.blink_on = True;
      if (new_tf->text.timer_id) {
	XtRemoveTimeOut(new_tf->text.timer_id);
	new_tf->text.timer_id = (XtIntervalId)0;
      }
    } else if (new_tf->text.timer_id == (XtIntervalId)0) {
      new_tf->text.timer_id =
	XtAppAddTimeOut(XtWidgetToApplicationContext(new_w),
			(unsigned long)TextF_BlinkRate(new_tf),
			HandleTimer,
			(XtPointer) new_tf);
    }
    BlinkInsertionPoint(new_tf);
  }

  if (TextF_MarginHeight(new_tf) != TextF_MarginHeight(old_tf)) {
    new_tf->text.margin_top = TextF_MarginHeight(new_tf);
    new_tf->text.margin_bottom = TextF_MarginHeight(new_tf);
  }

  new_size = TextF_MarginWidth(new_tf) != TextF_MarginWidth(old_tf) ||
             TextF_MarginHeight(new_tf) != TextF_MarginHeight(old_tf) ||
	     TextF_FontList(new_tf) != TextF_FontList(old_tf) ||
	     new_tf->primitive.highlight_thickness !=
	       old_tf->primitive.highlight_thickness ||
	     new_tf->primitive.shadow_thickness !=
	       old_tf->primitive.shadow_thickness;


  if (TextF_Columns(new_tf) < 0) {
    XmeWarning (new_w, MSG7);
    TextF_Columns(new_tf) = TextF_Columns(old_tf);
  }

  if (!(new_width != old_tf->core.width &&
	new_height != old_tf->core.height)) {
    if (TextF_Columns(new_tf) != TextF_Columns(old_tf) || new_size) {
      Dimension width, height;

      ComputeSize(new_tf, &width, &height);
      AdjustText(new_tf, 0, False);

      if (new_width == old_tf->core.width)
	new_w->core.width = width;
      if (new_height == old_tf->core.height)
	new_w->core.height = height;
      new_tf->text.h_offset = TextF_MarginWidth(new_tf) +
	new_tf->primitive.shadow_thickness +
	  new_tf->primitive.highlight_thickness;
      redisplay = True;
    }
  } else {
    if (new_width != new_tf->core.width)
      new_tf->core.width = new_width;
    if (new_height != new_tf->core.height)
      new_tf->core.height = new_height;
  }

  new_tf->text.refresh_ibeam_off = True; /* force update of putback area */

  _XmTextFieldDrawInsertionPoint(new_tf, True);

  if (XtIsSensitive((Widget)new_tf) != XtIsSensitive((Widget)old_tf)) {
    if (XtIsSensitive(new_w)) {
      _XmTextFieldDrawInsertionPoint(new_tf, False);
      new_tf->text.blink_on = False;
      _XmTextFieldDrawInsertionPoint(new_tf, True);
    } else {
      if (new_tf->text.has_focus) {
	ChangeBlinkBehavior(new_tf, False);
	_XmTextFieldDrawInsertionPoint(new_tf, False);
	new_tf->text.has_focus = False;
	new_tf->text.blink_on = True;
	_XmTextFieldDrawInsertionPoint(new_tf, True);
	(void) VerifyLeave(new_tf, NULL);
      }
    }
    if (new_tf->text.string_length > 0) redisplay = True;
  }

  (void)TextFieldGetDisplayRect((Widget)new_tf, &xmim_area);
  GetXYFromPos(new_tf, TextF_CursorPosition(new_tf), &xmim_point.x,
	       &xmim_point.y);

  if (TextF_Editable(old_tf) != TextF_Editable(new_tf)) {
    Boolean editable = TextF_Editable(new_tf);
    TextF_Editable(new_tf) = TextF_Editable(old_tf);
    XmTextFieldSetEditable(new_w, editable);
  } else if (new_font && TextF_Editable(new_tf)) {
    /* We want to be able to connect to an IM if XmNfontList has changed. */
    TextF_Editable(new_tf) = False;
    XmTextFieldSetEditable(new_w, True);
  }

  XtSetArg(im_args[n], XmNbackgroundPixmap,
	   new_tf->core.background_pixmap); n++;
  XtSetArg(im_args[n], XmNspotLocation, &xmim_point); n++;
  XtSetArg(im_args[n], XmNarea, &xmim_area); n++;
  XtSetArg(im_args[n], XmNlineSpace,
	   TextF_FontAscent(new_tf) + TextF_FontDescent(new_tf)); n++;
  XmImSetValues((Widget)new_tf, im_args, n);

  if (new_font) XmFontListFree((XmFontList)TextF_FontList(old_tf));

  if (!redisplay) redisplay = new_tf->text.redisplay;

  /* If I'm forced to redisplay, then actual widget won't be updated
   * until the expose proc.  Force the ibeam putback to be refreshed
   * at expose time so that it reflects true visual state of the
   * widget.  */

  if (redisplay) new_tf->text.refresh_ibeam_off = True;

  new_tf->text.in_setvalues = False;

  if ((!TextF_Editable(new_tf) || !XtIsSensitive(new_w)) &&
      new_tf->text.has_destination)
    (void) SetDestination(new_w, 0, False, (Time)0);

  /* don't shrink to nothing */
  if (new_tf->core.width == 0) new_tf->core.width = old_tf->core.width;
  if (new_tf->core.height == 0) new_tf->core.height = old_tf->core.height;

  if (!redisplay && redisplay_text)
    RedisplayText(new_tf, 0, new_tf->text.string_length);

  return redisplay;
}

/********************************************
 * AccessTextual trait method implementation
 ********************************************/

static XtPointer TextFieldGetValue(Widget w, int format)
{
	XmString s;
	XtPointer str;

	s = XmTextFieldGetXmString(w);
	switch (format) {
	case XmFORMAT_XmSTRING:
		return s;
	case XmFORMAT_MBYTE:
		str = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmMULTIBYTE_TEXT);
		XmStringFree(s);
		return str;
	case XmFORMAT_WCS:
		str = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT);
		XmStringFree(s);
		return str;
	}

	XmStringFree(s);
	return NULL;
}

static void TextFieldSetValue(Widget w, XtPointer s, int format)
{
	char *str;
	XmString xs;

	switch (format) {
	case XmFORMAT_XmSTRING:
		XmTextFieldSetXmString(w, s);
		return;
	case XmFORMAT_MBYTE:
		xs = XmStringCreateMultibyte(s, NULL);
		XmTextFieldSetXmString(w, xs);
		XmStringFree(xs);
		return;
	case XmFORMAT_WCS:
		xs = XmStringCreateWide((wchar_t *)s, NULL);
		XmTextFieldSetXmString(w, xs);
		XmStringFree(xs);
		return;
	}
}

static int TextFieldPreferredValue(Widget w) /* unused */
{
	return XmFORMAT_XmSTRING;
}

/*
 * XmRCallProc routine for checking text.font_list before setting it to NULL
 * if no value is specified for both XmNrenderTable and XmNfontList.
 * If "check_set_render_table" == True, then function has been called twice
 * on same widget, thus resource needs to be set NULL, otherwise leave it
 * alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmTextFieldWidget tf = (XmTextFieldWidget)wid;

  if (tf->text.check_set_render_table)
	value->addr = NULL;
  else {
	tf->text.check_set_render_table = True;
	value->addr = (char*)&(tf->text.font_list);
  }
}

static Boolean
TextFieldRemove(Widget w,
		XEvent *event)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition left = tf->text.prim_pos_left, right = tf->text.prim_pos_right;
  XmAnyCallbackStruct cb;

  if (TextF_Editable(tf) == False)
    return False;

  TextFieldResetIC(w);
  if (!tf->text.has_primary || left == right) {
    tf->text.prim_anchor = TextF_CursorPosition(tf);
    return False;
  }

  if (_XmTextFieldReplaceText(tf, event, left, right, NULL, 0, True, True)) {
    _XmTextFieldStartSelection(tf, TextF_CursorPosition(tf),
			       TextF_CursorPosition(tf),
			       XtLastTimestampProcessed(XtDisplay(w)));
    tf->text.pending_off = False;
    cb.reason = XmCR_VALUE_CHANGED;
    cb.event = event;
    XtCallCallbackList((Widget) tf, TextF_ValueChangedCallback(tf),
		       (XtPointer) &cb);
  }

  tf->text.prim_anchor = TextF_CursorPosition(tf);

  return True;
}

static Boolean
TextFieldGetBaselines(Widget w,
		      Dimension ** baselines,
		      int *line_count)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Dimension *base_array;
  _XmWidgetToAppContext(w);
  _XmAppLock(app);

  *line_count = 1;
  base_array = (Dimension *) XtMalloc(sizeof(Dimension));

  base_array[0] = tf->text.margin_top + tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness + TextF_FontAscent(tf);

  *baselines = base_array;
  _XmAppUnlock(app);
  return True;
}

static Boolean
TextFieldGetDisplayRect(Widget w,
			XRectangle * display_rect)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Position margin_width = TextF_MarginWidth(tf) +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  Position margin_top = tf->text.margin_top + tf->primitive.shadow_thickness +
    tf->primitive.highlight_thickness;
  Position margin_bottom = tf->text.margin_bottom +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;
  (*display_rect).x = margin_width;
  (*display_rect).y = margin_top;
  (*display_rect).width = tf->core.width - (2 * margin_width);
  (*display_rect).height = tf->core.height - (margin_top + margin_bottom);

  return True;
}

static void
TextFieldMarginsProc(Widget w,
		     XmBaselineMargins *margins_rec)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  if (margins_rec->get_or_set == XmBASELINE_SET) {
    tf->text.margin_top = margins_rec->margin_top;
  } else {
    margins_rec->margin_top = tf->text.margin_top;
    margins_rec->margin_bottom = tf->text.margin_bottom;
    margins_rec->text_height = TextF_FontAscent(tf) + TextF_FontDescent(tf);
    margins_rec->shadow = tf->primitive.shadow_thickness;
    margins_rec->highlight = tf->primitive.highlight_thickness;
    margins_rec->margin_height = 0;
  }
}

/*
 * This function shows the correspondence of rendition data between the input
 * server and XmTextField
 */
static XmHighlightMode
_XimFeedbackToXmHighlightMode(XIMFeedback fb)
{
    switch (fb) {
      case XIMReverse:
        return(XmHIGHLIGHT_SELECTED);
      case XIMUnderline:
        return(XmHIGHLIGHT_SECONDARY_SELECTED);
      case XIMHighlight:
        return(XmHIGHLIGHT_NORMAL);
      case XIMPrimary:
	return(XmHIGHLIGHT_SELECTED);
      case XIMSecondary:
	return(XmHIGHLIGHT_SECONDARY_SELECTED);
      case XIMTertiary:
        return(XmHIGHLIGHT_SELECTED);
      default:
        return(XmHIGHLIGHT_NORMAL);
    }
}
/*
 * This function treats the rendition data.
 */
static void
PreeditSetRendition(Widget w,
                    XIMPreeditDrawCallbackStruct* data)
{
    XIMText *text = data->text;
    int cnt;
    XIMFeedback fb;
    XmTextPosition prestart = PreStart((XmTextFieldWidget)w)+data->chg_first, left, right;
    XmHighlightMode mode;
    XmTextFieldWidget tf = (XmTextFieldWidget)w;

    if (!text->length) {
        return;
    }

    if (!text->feedback)
        return;

    fb = text->feedback[0];     /* initial feedback */
    left = right = prestart;    /* mode start/end position */
    mode = _XimFeedbackToXmHighlightMode(fb); /* mode */
    cnt = 1;                    /* counter initialize */

    while (cnt < (int)text->length) {
        if (fb != text->feedback[cnt]) {
            right = prestart + cnt;
            doSetHighlight(w, left, right, mode);

            left = right;       /* start position update */
            fb = text->feedback[cnt]; /* feedback update */
            mode = _XimFeedbackToXmHighlightMode(fb);
        }
        cnt++;                  /* counter increment */
    }
    doSetHighlight(w, left, (prestart + cnt), mode);
                                /* for the last segment */
}

/*
 * This function and _XmTextFieldSetCursorPosition are almost same. The
 * difference is that this function don't call user's callbacks link
 * XmNmotionVerifyCallback.
 */
static void
PreeditSetCursorPosition(XmTextFieldWidget tf,
                         XmTextPosition position)
{
    int i;
    _XmHighlightRec *hl_list = tf->text.highlight.list;

    if (position < 0) position = 0;

    if (position > tf->text.string_length)
       position = tf->text.string_length;

    _XmTextFieldDrawInsertionPoint(tf, False);

    TextF_CursorPosition(tf) = position;
    for (i = tf->text.highlight.number - 1; i >= 0; i--){
       if (position >= hl_list[i].position || i == 0)
          break;
    }

    if (position == hl_list[i].position)
       ResetImageGC(tf);
    else if (hl_list[i].mode != XmHIGHLIGHT_SELECTED)
       ResetImageGC(tf);
    else
       InvertImageGC(tf);

    ResetClipOrigin(tf);

    tf->text.refresh_ibeam_off = True;
    _XmTextFieldDrawInsertionPoint(tf, True);
}

static void PreeditVerifyReplace(XmTextFieldWidget tf,
				 XmTextPosition start,
				 XmTextPosition end,
				 XmString insert,
				 size_t insert_length,
				 XmTextPosition cursor,
				 Boolean *end_preedit)
{
  FUnderVerifyPreedit(tf) = True;
  _XmTextFieldReplaceText(tf, NULL, start, end, insert, insert_length, True, False);
  FUnderVerifyPreedit(tf) = False;
  if (FVerifyCommitNeeded(tf)) {
    TextFieldResetIC((Widget) tf);
    *end_preedit = True;
  }
  _XmTextFieldSetCursorPosition(tf, NULL, cursor, False, True);
}

/*
 * This is the function set to XNPreeditStartCallback resource.
 * This function is called when the preedit process starts.
 * Initialize the preedit data and also treat pending delete.
 */
static int
PreeditStart(XIC xic,
             XPointer client_data,
             XPointer call_data)
{
    XmTextPosition cursorPos, nextPos, lastPos;
    Boolean replace_res, pending_delete = False;
    wchar_t *wc;
    char *mb;
    Widget w = (Widget) client_data;
    XmTextFieldWidget tf = (XmTextFieldWidget) client_data;

    tf->text.onthespot->over_len = 0;
    tf->text.onthespot->over_str = NULL;
    tf->text.onthespot->over_maxlen = 0;

    /* If TextField is not editable, returns 0. So input server never */
    /* call Preedit Draw callback */
    if (!TextF_Editable(tf)) {
        if (tf->text.verify_bell) XBell(XtDisplay((Widget)tf), 0);
        tf->text.onthespot->under_preedit = False;
        return 0;
    }

    if (NeedsPendingDeleteDisjoint(tf)){
        _XmTextFieldDrawInsertionPoint(tf, False);
        if (!XmTextFieldGetSelectionPosition(w, &cursorPos, &nextPos) ||
                                                cursorPos == nextPos) {
          tf->text.prim_anchor = TextF_CursorPosition(tf);
        }
        pending_delete = True;

        tf->text.prim_anchor = TextF_CursorPosition(tf);
        replace_res = _XmTextFieldReplaceText(tf, NULL, cursorPos,
                                              nextPos, NULL, 0, True, False);

        if (replace_res){
            if (pending_delete)
                XmTextFieldSetSelection(w, TextF_CursorPosition(tf),
                        TextF_CursorPosition(tf),
                        XtLastTimestampProcessed(XtDisplay((Widget)tf)));

            CheckDisjointSelection(w, TextF_CursorPosition(tf),
                        XtLastTimestampProcessed(XtDisplay((Widget)tf)));

            _XmTextFieldSetCursorPosition(tf,
                        NULL,
                        TextF_CursorPosition(tf), False, True);
        }
        _XmTextFieldDrawInsertionPoint(tf, True);
    }

    PreStart(tf) = PreEnd(tf) = PreCursor(tf) = TextF_CursorPosition(tf);
    tf->text.onthespot->under_preedit = True;

    if (tf->text.overstrike) {
       lastPos = tf->text.string_length;
       tf->text.onthespot->over_len = lastPos - PreCursor(tf);
       tf->text.onthespot->over_str = XmStringSubstr(tf->text.xms_value, PreStart(tf), tf->text.onthespot->over_len);
    }

    return -1;
}

/*
 * This is the function set to XNPreeditDoneCallback resource.
 * This function is called when the preedit process is finished.
 */
static void
PreeditDone(XIC xic,
            XPointer client_data,
            XPointer call_data)
{
    XmString s, over;
    Boolean replace_res;
    XmTextFieldWidget tf = (XmTextFieldWidget)client_data;
    Widget p = (Widget) tf;
    Boolean need_verify, end_preedit = False;

    if (!TextF_Editable(tf))
        return;

    while (!XtIsShell(p))
	p = XtParent(p);
    XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

    if (PreEnd(tf) > PreStart(tf)) {
      if (need_verify) {
        PreeditVerifyReplace(tf, PreStart(tf), PreEnd(tf), NULL, 0,
                                PreStart(tf), &end_preedit);
        if (end_preedit) return;
      }
      else
        _XmTextFieldReplaceText(tf, NULL, PreStart(tf), PreEnd(tf), NULL, 0, True, False);
    }

    if (tf->text.overstrike){
      over = XmStringCopy(tf->text.onthespot->over_str);

      if (need_verify) {
        PreeditVerifyReplace(tf, PreStart(tf), PreStart(tf), over,
                             XmStringLen(over), PreStart(tf), &end_preedit);
	    if (end_preedit) return;
      } else {
        _XmTextFieldDrawInsertionPoint(tf, False);
        replace_res = _XmTextFieldReplaceText(tf, NULL, PreStart(tf),
                                              PreStart(tf), over,
                                              XmStringLen(over), True, False);
        TextF_CursorPosition(tf) = PreStart(tf);
        PreeditSetCursorPosition(tf, TextF_CursorPosition(tf));
        _XmTextFieldDrawInsertionPoint(tf, True);
      }

      XmStringFree(tf->text.onthespot->over_str);
      tf->text.onthespot->over_str = NULL;
      tf->text.onthespot->over_len = tf->text.onthespot->over_maxlen = 0;
    }

    PreStart(tf) = PreEnd(tf) = PreCursor(tf) = 0;
    tf->text.onthespot->under_preedit = False;
}

/*
 * This is the function set to XNPreeditDrawCallback resource.
 * This function is called when the input server requests XmTextField
 * to draw a preedit string.
 */
static void
PreeditDraw(XIC xic,
            XPointer client_data,
            XIMPreeditDrawCallbackStruct *call_data)
{
    Widget w = (Widget) client_data;
    XmTextFieldWidget tf = (XmTextFieldWidget) client_data;
    int insert_length = 0;
    XmTextPosition startPos, endPos, rest_len =0 , tmp_end;
    Boolean replace_res;
    int i;
    int recover_len=0;
    char *ptr=NULL;
    Widget p =w;
    Boolean need_verify, end_preedit = False;
    XmString s, rest = NULL, recover = NULL;

    if (!TextF_Editable(tf))
        return;

    if (call_data->text &&
        (insert_length = call_data->text->length) > TEXT_MAX_INSERT_SIZE)
        return;

    if (call_data->chg_length>PreEnd(tf)-PreStart(tf))
        call_data->chg_length = PreEnd(tf)-PreStart(tf);

    while (!XtIsShell(p)) p = XtParent(p);
    XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

    _XmTextFieldDrawInsertionPoint(tf, False);
    doSetHighlight(w, PreStart(tf)+call_data->chg_first,
        PreStart(tf)+call_data->chg_first + call_data->chg_length,
        XmHIGHLIGHT_NORMAL);


    if (!tf->text.overstrike && (!call_data->text || !insert_length)) {
        startPos = PreStart(tf) + call_data->chg_first;
        endPos = startPos + call_data->chg_length;
	PreEnd(tf) -= endPos - startPos;
	if (need_verify) {
	  PreeditVerifyReplace(tf, startPos, endPos, NULL, 0,
					startPos, &end_preedit);
	  if (end_preedit) {
	    _XmTextFieldDrawInsertionPoint(tf, True);
	    return;
	  }
	}
	else {
      replace_res = _XmTextFieldReplaceText(tf, NULL, startPos,
                                            endPos, NULL, 0, True, False);
	}
	_XmTextFieldDrawInsertionPoint(tf, True);
        return;
    }

    if (call_data->text) {
    if ((call_data->text->encoding_is_wchar &&
         !call_data->text->string.wide_char) ||
        (!call_data->text->encoding_is_wchar &&
         !call_data->text->string.multi_byte)){
        PreeditSetRendition(w, call_data);
        PreeditSetCursorPosition(tf, TextF_CursorPosition(tf));
        _XmTextFieldDrawInsertionPoint(tf, True);
        return;
    }
    }

    startPos = PreStart(tf) + call_data->chg_first;
    endPos = startPos + call_data->chg_length;

   if (tf->text.overstrike){
        startPos = PreStart(tf) + call_data->chg_first;
        tmp_end = (XmTextPosition)(PreEnd(tf) + insert_length -
                                                call_data->chg_length);
        if (tf->text.onthespot->over_maxlen < tmp_end - PreStart(tf)){
            if (tmp_end - PreStart(tf) > tf->text.onthespot->over_len){
                endPos = startPos + call_data->chg_length;
                tf->text.onthespot->over_maxlen = tf->text.onthespot->over_len;
            } else {
                endPos = PreEnd(tf) + tmp_end - PreStart(tf) -
                                tf->text.onthespot->over_maxlen;
                tf->text.onthespot->over_maxlen = tmp_end - PreStart(tf);
            }
        } else
        if (tf->text.onthespot->over_maxlen > tmp_end - PreStart(tf)) {
            endPos = PreEnd(tf);
            recover_len = tf->text.onthespot->over_maxlen - tmp_end +
                                PreStart(tf);
            tf->text.onthespot->over_maxlen = tmp_end - PreStart(tf);
            if (recover_len > 0) {
              recover = XmStringSubstr(tf->text.onthespot->over_str, tf->text.onthespot->over_maxlen, (size_t)recover_len);
            }
        } else
            endPos = startPos + call_data->chg_length;

        rest_len = (XmTextPosition)(PreEnd(tf) - PreStart(tf) -
                        call_data->chg_first - call_data->chg_length);
        if (rest_len > 0) {
            rest = XmStringSubstr(tf->text.xms_value,
                                  PreStart(tf) + call_data->chg_first +
                                  call_data->chg_length, (size_t)rest_len);
        }
    }

    if (tf->text.overstrike)
        PreEnd(tf) = startPos + insert_length;
    else
        PreEnd(tf) += insert_length - endPos + startPos;

    if (PreEnd(tf) < PreStart(tf))
        PreEnd(tf) = PreStart(tf);

    PreCursor(tf) = PreStart(tf) + call_data->caret;
    if (call_data->text->encoding_is_wchar)
      s = XmStringCreateWide(call_data->text->string.wide_char, NULL);
    else s = XmStringCreateMultibyte(call_data->text->string.multi_byte, NULL);

    if (tf->text.overstrike && rest)
      s = XmStringConcatAndFree(s, rest);

    if (tf->text.overstrike && recover)
      s = XmStringConcatAndFree(s, recover);

    if (need_verify) {
      PreeditVerifyReplace(tf, startPos, endPos, s, XmStringLen(s),
                           PreCursor(tf), &end_preedit);
      if (end_preedit) {
        _XmTextFieldDrawInsertionPoint(tf, True);
        return;
      }
    } else {
      replace_res = _XmTextFieldReplaceText(tf, NULL, startPos, endPos, s,
                                            XmStringLen(s), True, False);
      PreeditSetCursorPosition(tf, PreCursor(tf));
    }

    if (insert_length > 0)
       PreeditSetRendition(w, call_data);
    _XmTextFieldDrawInsertionPoint(tf, True);
}

/*
 * This is the function set to XNPreeditCaretCallback resource.
 * This function is called when the input server requests XmTextField to move
 * the caret.
 */
static void
PreeditCaret(XIC xic,
             XPointer client_data,
             XIMPreeditCaretCallbackStruct *call_data)
{
    XmTextPosition new_position;
    XmTextFieldWidget tf = (XmTextFieldWidget)client_data;
    Widget p = (Widget) tf;
    Boolean need_verify;

    if (!TextF_Editable(tf))
        return;

    while (!XtIsShell(p))
	p = XtParent(p);
    XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

    _XmTextFieldDrawInsertionPoint(tf, False);
    switch (call_data->direction) {
        case XIMForwardChar:
          new_position = PreCursor(tf) + 1 - PreStart(tf);
          break;
        case XIMBackwardChar:
          new_position = PreCursor(tf) - 1 - PreStart(tf);
          break;
        case XIMAbsolutePosition:
          new_position = (XmTextPosition) call_data->position;
          break;
        default:
          new_position = PreCursor(tf) - PreStart(tf);
    }

    TextF_CursorPosition(tf) = PreCursor(tf) = PreStart(tf) + new_position;
    if (need_verify) {
	FUnderVerifyPreedit(tf) = True;
	_XmTextFieldSetCursorPosition(tf, NULL, PreCursor(tf), False, True);
	FUnderVerifyPreedit(tf) = False;
    } else
    	PreeditSetCursorPosition(tf, TextF_CursorPosition(tf));
    _XmTextFieldDrawInsertionPoint(tf, True);
}

/*
 * 1. Call XmImMbResetIC to reset the input method and get the current
 *    preedit string.
 * 2. Set the string to XmTextField
 */
static void
TextFieldResetIC(Widget w)
{
    size_t insert_length;
    Boolean replace_res;
    XmTextPosition cursorPos, nextPos;
    XmTextFieldWidget tf = (XmTextFieldWidget)w;
    XmString s = NULL;
    char *mb = NULL;

    if (!(tf->text.onthespot->under_preedit))
        return;

    if (FVerifyCommitNeeded(tf)) {
      FVerifyCommitNeeded(tf) = False;
      s = XmStringSubstr(tf->text.xms_value, PreStart(tf), PreEnd(tf) - PreStart(tf));
      XmImMbResetIC(w, &mb);
      XFree(mb);
    } else {
      XmImMbResetIC(w, &mb);
      s = XmStringCreateMultibyte(mb, NULL);
      XFree(mb);
    }

    if (!s) {
        ResetUnder(tf);
        return;
    }

    if (!TextF_Editable(tf)) {
      if (tf->text.verify_bell)
        XBell(XtDisplay((Widget)tf), 0);
    }

    if ((insert_length = XmStringLen(s)) > TEXT_MAX_INSERT_SIZE) {
        ResetUnder(tf);
        XmStringFree(s);
        return;
    }

    cursorPos = nextPos = TextF_CursorPosition(tf);
    if (tf->text.overstrike && nextPos != (XmTextPosition)tf->text.string_length)
      nextPos++;

    replace_res = _XmTextFieldReplaceText(tf, NULL, cursorPos,
                                          nextPos, s, insert_length, True, True);

    if (replace_res)
        _XmTextFieldSetCursorPosition(tf, NULL,
                                      TextF_CursorPosition(tf), False, True);
    _XmTextFieldDrawInsertionPoint(tf, True);
    XmStringFree(s);
    ResetUnder(tf);
}


static void
ResetUnder(XmTextFieldWidget tf)
{
    if (XmImGetXICResetState((Widget)tf) != XIMPreserveState)
      tf->text.onthespot->under_preedit = False;
}

/***********************************<->***************************************

 *                              Public Functions                             *
 ***********************************<->***************************************/

XmString XmTextFieldGetXmString(Widget w)
{
	XmString s;
	_XmWidgetToAppContext(w);

	_XmAppLock(app);
	s = XmStringCopy(((XmTextFieldWidget)w)->text.xms_value);
	_XmAppUnlock(app);
	return s;
}

char *XmTextFieldGetString(Widget w)
{
	char *ret;
	XmTextFieldWidget tf = (XmTextFieldWidget) w;
	_XmWidgetToAppContext(w);

	_XmAppLock(app);
	ret = XmStringUngenerate(tf->text.xms_value, NULL, XmUTF8_TEXT, XmMULTIBYTE_TEXT);
	_XmAppUnlock(app);
	return ret;
}

wchar_t *XmTextFieldGetStringWcs(Widget w)
{
	char *ret;
	XmTextFieldWidget tf = (XmTextFieldWidget) w;
	_XmWidgetToAppContext(w);

	_XmAppLock(app);
	ret = XmStringUngenerate(tf->text.xms_value, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT);
	_XmAppUnlock(app);
	return (wchar_t *)ret;
}

XmString XmTextFieldSubstring(Widget w, XmTextPosition start, size_t length)
{
	XmString s;
	_XmWidgetToAppContext(w);

	_XmAppLock(app);
	s = XmStringSubstr(((XmTextFieldWidget)w)->text.xms_value, start, length);
	_XmAppUnlock(app);

	return s;
}

int XmTextFieldGetSubstring(Widget widget, XmTextPosition start,
                            int num_chars, int buf_size, char *buffer)
{
	int ret = XmCOPY_SUCCEEDED;
	char *bytes = NULL;
	XmString s;
	_XmWidgetToAppContext(widget);

	if (num_chars < 0 || !buffer)
		return XmCOPY_FAILED;

	_XmAppLock(app);
	if (!(s     = XmTextFieldSubstring(widget, start, (size_t)num_chars))   ||
	    !(bytes = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmMULTIBYTE_TEXT)) ||
	    (size_t)buf_size < strlen(bytes) + 1)
		goto copy_failed;

	if ((size_t)num_chars > XmStringLen(s))
		ret = XmCOPY_TRUNCATED;
	XmStringFree(s);

	memcpy(buffer, bytes, strlen(bytes) + 1);
	XtFree(bytes);
	_XmAppUnlock(app);
	return ret;

copy_failed:
	_XmAppUnlock(app);
	XtFree(bytes);
	XmStringFree(s);
	return XmCOPY_FAILED;
}

int XmTextFieldGetSubstringWcs(Widget widget, XmTextPosition start,
                               int num_chars, int buf_size, wchar_t *buffer)
{
	int ret = XmCOPY_SUCCEEDED;
	char *bytes = NULL;
	XmString s;
	_XmWidgetToAppContext(widget);

	if (num_chars < 0 || !buffer)
		return XmCOPY_FAILED;

	_XmAppLock(app);
	if (!(s     = XmTextFieldSubstring(widget, start, (size_t)num_chars))    ||
	    !(bytes = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT)) ||
	    (size_t)buf_size < wcslen((wchar_t *)bytes))
		goto copy_failed;

	if ((size_t)num_chars > XmStringLen(s))
		ret = XmCOPY_TRUNCATED;
	XmStringFree(s);

	memcpy(buffer, bytes, (wcslen((wchar_t *)bytes) + 1) * sizeof(wchar_t));
	XtFree(bytes);
	_XmAppUnlock(app);
	return ret;

copy_failed:
	_XmAppUnlock(app);
	XtFree(bytes);
	XmStringFree(s);
	return XmCOPY_FAILED;
}

XmTextPosition XmTextFieldGetLastPosition(Widget w)
{
	XmTextFieldWidget tf = (XmTextFieldWidget) w;
	XmTextPosition ret;
	_XmWidgetToAppContext(w);

	_XmAppLock(app);
	ret = (XmTextPosition)tf->text.string_length;
	_XmAppUnlock(app);
	return ret;
}

void XmTextFieldSetXmString(Widget w, XmString value)
{
	size_t length;
	XmAnyCallbackStruct cb;
	XmTextPosition from = 0, to, new_insert = 0;
	XmTextFieldWidget tf = (XmTextFieldWidget)w;

	_XmWidgetToAppContext(w);
	_XmAppLock(app);
	TextFieldResetIC(w);

	if (!value)
		value = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);

	memset(&cb, 0, sizeof cb);
	if (XtIsSensitive(w) && tf->text.has_focus)
		ChangeBlinkBehavior(tf, False);
	_XmTextFieldDrawInsertionPoint(tf, False);

	to     = tf->text.string_length;
	length = XmStringLen(value);
	if (!ModifyVerify(tf, NULL, &from, &to, &value, &length, &new_insert)) {
		if (tf->text.verify_bell)
			XBell(XtDisplayOfObject(w), 0);
		_XmAppUnlock(app);
		return;
	}

	TextFieldSetHighlight(tf, 0, tf->text.string_length, XmHIGHLIGHT_NORMAL);
	XmStringFree(tf->text.xms_value);
	tf->text.xms_value     = XmStringCopy(value);
	tf->text.string_length = length;
	tf->text.pending_off   = True;
	SetCursorPosition(tf, NULL, 0, True, True, False, DontCare);

	if (!tf->text.do_resize || !TextF_ResizeWidth(tf)) {
		tf->text.h_offset = TextF_MarginWidth(tf) +
		                    tf->primitive.shadow_thickness +
		                    tf->primitive.highlight_thickness;
		if (!AdjustText(tf, TextF_CursorPosition(tf), False))
			RedisplayText(tf, 0, tf->text.string_length);
	} else AdjustSize(tf);

	cb.reason = XmCR_VALUE_CHANGED;
	XtCallCallbackList(w, TextF_ValueChangedCallback(tf), (XtPointer)&cb);
	tf->text.refresh_ibeam_off = True;

	if (XtIsSensitive(w) && tf->text.has_focus)
		ChangeBlinkBehavior(tf, True);
	_XmTextFieldDrawInsertionPoint(tf, True);
	_XmAppUnlock(app);
}

void XmTextFieldSetString(Widget w, char *value)
{
	XmString s;

	s = XmStringCreateMultibyte(value, NULL);
	XmTextFieldSetXmString(w, s);
	XmStringFree(s);
}

void XmTextFieldSetStringWcs(Widget w, wchar_t *wc_value)
{
	XmString s;

	s = XmStringCreateWide(wc_value, NULL);
	XmTextFieldSetXmString(w, s);
	XmStringFree(s);
}

void XmTextFieldReplaceString(Widget w, XmTextPosition from,
                              XmTextPosition to, XmString value)
{
	size_t length;
	Boolean rep           = False;
	Boolean deselected    = False;
	XmTextFieldWidget tf  = (XmTextFieldWidget)w;
	Boolean save_editable;
	int save_maxlength;
	XmTextPosition cursor;
	XmAnyCallbackStruct cb;

	_XmWidgetToAppContext(w);
	_XmAppLock(app);

	/* TODO: Emit a warning here? */
	if (to < from)
		return;

	memset(&cb, 0, sizeof cb);
	save_editable  = TextF_Editable(tf);
	save_maxlength = TextF_MaxLength(tf);

	VerifyBounds(tf, &from, &to);
	TextF_Editable(tf)  = True;
	TextF_MaxLength(tf) = INT_MAX;

	length = XmStringLen(value);
	rep    = _XmTextFieldReplaceText(tf, NULL, from, to, XmStringCopy(value),
	                                 length, False, True);
	TextF_Editable(tf)  = save_editable;
	TextF_MaxLength(tf) = save_maxlength;

	if (!rep) {
		_XmAppUnlock(app);
		return;
	}

	if (tf->text.has_primary) {
		if ((tf->text.prim_pos_left  > from &&
		     tf->text.prim_pos_left  < to) ||
		    (tf->text.prim_pos_right > from &&
		     tf->text.prim_pos_right < to) ||
		    (tf->text.prim_pos_left  <= from &&
		     tf->text.prim_pos_right >= to)) {
			_XmTextFieldDeselectSelection(w, False, XtLastTimestampProcessed(XtDisplayOfObject(w)));
			deselected = True;
		}
	}

	cursor = TextF_CursorPosition(tf);
	if (from <= cursor) {
		/* Replace will not move us, we still want this to happen */
		if (cursor < to)
			cursor = (cursor - from) <= (XmTextPosition)length ?
			         cursor : from + (XmTextPosition)length;
		else cursor -= (to - from) + (XmTextPosition)length;
		SetCursorPosition(tf, NULL, cursor, True, True, False, DontCare);
	}

	/*
	 * Replace Text utilizes an optimization in deciding which text to redraw;
	 * in the case that the selection has been changed (as above), this can
	 * cause part/all of the replaced text to NOT be redrawn.  The following
	 * AdjustText call ensures that it IS drawn in this case.
	 */
	if (deselected)
		AdjustText(tf, from, True);
	SetDestination(w, TextF_CursorPosition(tf), False, XtLastTimestampProcessed(XtDisplay(w)));

	cb.reason = XmCR_VALUE_CHANGED;
	XtCallCallbackList(w, TextF_ValueChangedCallback(tf), (XtPointer)&cb);
	_XmAppUnlock(app);
}

void XmTextFieldReplace(Widget w, XmTextPosition from_pos,
                        XmTextPosition to_pos, char *value)
{
	XmString s;

	s = XmStringCreateMultibyte(value, NULL);
	XmTextFieldReplaceString(w, from_pos, to_pos, s);
	XmStringFree(s);
}

void XmTextFieldReplaceWcs(Widget w, XmTextPosition from_pos,
                           XmTextPosition to_pos, wchar_t *wc_value)
{
	XmString s;

	s = XmStringCreateWide(wc_value, NULL);
	XmTextFieldReplaceString(w, from_pos, to_pos, s);
	XmStringFree(s);
}

void XmTextFieldInsertString(Widget w, XmTextPosition pos, XmString value)
{
	XmTextFieldReplaceString(w, pos, pos, value);
}

void XmTextFieldInsert(Widget w, XmTextPosition position, char *value)
{
	XmString s;

	s = XmStringCreateMultibyte(value, NULL);
	XmTextFieldInsertString(w, position, s);
	XmStringFree(s);
}

void XmTextFieldInsertWcs(Widget w, XmTextPosition position, wchar_t *wcstring)
{
	XmString s;

	s = XmStringCreateWide(wcstring, NULL);
	XmTextFieldInsertString(w, position, s);
	XmStringFree(s);
}

void
XmTextFieldSetAddMode(Widget w,
                      Boolean state)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if (tf->text.add_mode == state) {
    _XmAppUnlock(app);
    return;
  }

  _XmTextFieldDrawInsertionPoint(tf, False);
  tf->text.add_mode = state;
  _XmTextFieldDrawInsertionPoint(tf, True);
  _XmAppUnlock(app);
}

Boolean
XmTextFieldGetAddMode(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = tf->text.add_mode;
  _XmAppUnlock(app);
  return ret_val;
}

Boolean
XmTextFieldGetEditable(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = TextF_Editable(tf);
  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextFieldSetEditable(Widget w,
                       Boolean editable)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XPoint xmim_point;
  XRectangle xmim_area;
  Arg args[11];                 /* To set initial values to input method */
  XIMCallback xim_cb[5];        /* on the spot im callbacks */
  Cardinal n = 0;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* if widget previously wasn't editable, no input method has yet been
   * registered.  So, if we're making it editable now, register the IM and
   * give the IM the relevent values. */

  if (!TextF_Editable(tf) && editable) {
    XmImRegister((Widget)tf, 0);

    GetXYFromPos(tf, TextF_CursorPosition(tf), &xmim_point.x,
		 &xmim_point.y);
    (void)TextFieldGetDisplayRect((Widget)tf, &xmim_area);
    n = 0;
    XtSetArg(args[n], XmNfontList, TextF_FontList(tf)); n++;
    XtSetArg(args[n], XmNbackground, tf->core.background_pixel); n++;
    XtSetArg(args[n], XmNforeground, tf->primitive.foreground); n++;
    XtSetArg(args[n], XmNbackgroundPixmap,tf->core.background_pixmap);n++;
    XtSetArg(args[n], XmNspotLocation, &xmim_point); n++;
    XtSetArg(args[n], XmNarea, &xmim_area); n++;
    XtSetArg(args[n], XmNlineSpace,
	     TextF_FontAscent(tf)+ TextF_FontDescent(tf)); n++;

    /*
     * On the spot support. Register preedit callbacks.
     */
    xim_cb[0].client_data = (XPointer)tf;
    xim_cb[0].callback = (XIMProc)PreeditStart;
    xim_cb[1].client_data = (XPointer)tf;
    xim_cb[1].callback = (XIMProc)PreeditDone;
    xim_cb[2].client_data = (XPointer)tf;
    xim_cb[2].callback = (XIMProc)PreeditDraw;
    xim_cb[3].client_data = (XPointer)tf;
    xim_cb[3].callback = (XIMProc)PreeditCaret;
    XtSetArg(args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
    XtSetArg(args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
    XtSetArg(args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
    XtSetArg(args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;

    if (tf->text.has_focus)
       XmImSetFocusValues((Widget)tf, args, n);
    else
       XmImSetValues((Widget)tf, args, n);

  } else if (TextF_Editable(tf) && !editable) {
    XmImUnregister(w);
  }

  TextF_Editable(tf) = editable;

  n = 0;
  if (editable) {
    XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_ACTIVE); n++;
  } else {
    XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_INACTIVE); n++;
  }

  XmDropSiteUpdate((Widget)tf, args, n);
  _XmAppUnlock(app);
}

int
XmTextFieldGetMaxLength(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  int ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = TextF_MaxLength(tf);
  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextFieldSetMaxLength(Widget w,
			int max_length)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  TextF_MaxLength(tf) = max_length;
  _XmAppUnlock(app);
}

XmTextPosition
XmTextFieldGetInsertionPosition(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = TextF_CursorPosition(tf);
  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextFieldSetInsertionPosition(Widget w,
				XmTextPosition position)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  TextFieldResetIC(w);
  SetCursorPosition(tf, NULL, position, True, True, False, DontCare);
  _XmAppUnlock(app);
}

Boolean
XmTextFieldGetSelectionPosition(Widget w,
				XmTextPosition *left,
				XmTextPosition *right)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (tf->text.has_primary) {
    *left = tf->text.prim_pos_left;
    *right = tf->text.prim_pos_right;
  }
  _XmAppUnlock(app);
  return tf->text.has_primary;
}

XmString XmTextFieldGetSelectionString(Widget w)
{
	XmString s;
	XmTextFieldWidget tf = (XmTextFieldWidget)w;

	_XmWidgetToAppContext(w);
	_XmAppLock(app);
	s = XmStringSubstr(tf->text.xms_value, tf->text.prim_pos_left,
	                   (size_t)tf->text.prim_pos_right - (size_t)tf->text.prim_pos_left);
	_XmAppUnlock(app);
	return s;
}

char *XmTextFieldGetSelection(Widget w)
{
	XmString s;
	char *out;

	s   = XmTextFieldGetSelectionString(w);
	out = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmMULTIBYTE_TEXT);
	XmStringFree(s);
	return out;
}

wchar_t *XmTextFieldGetSelectionWcs(Widget w)
{
	XmString s;
	char *out;

	s   = XmTextFieldGetSelectionString(w);
	out = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmWIDECHAR_TEXT);
	XmStringFree(s);
	return (wchar_t *)out;
}

Boolean
XmTextFieldRemove(Widget w)
{
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = TextFieldRemove(w, NULL);
  _XmAppUnlock(app);

  return ret_val;
}

Boolean
XmTextFieldCopy(Widget w,
		Time clip_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* using the clipboard facilities, copy the selected
     text to the clipboard */
  if (tf->text.prim_pos_left != tf->text.prim_pos_right)
  {
    _XmAppUnlock(app);
    return XmeClipboardSource(w, XmCOPY, clip_time);
  }

  _XmAppUnlock(app);
  return False;
}

Boolean
XmTextFieldCopyLink(Widget w,
		    Time clip_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if (tf->text.prim_pos_left != tf->text.prim_pos_right)
  {
    ret_val = XmeClipboardSource(w, XmLINK, clip_time);
    _XmAppUnlock(app);
    return ret_val;
  }

  _XmAppUnlock(app);
  return False;
}

Boolean
XmTextFieldCut(Widget w,
	       Time clip_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (TextF_Editable(tf) == False)
  {
    _XmAppUnlock(app);
    return False;
  }

  if (tf->text.prim_pos_left != tf->text.prim_pos_right)
  {
    ret_val = XmeClipboardSource(w, XmMOVE, clip_time);
    _XmAppUnlock(app);
    return ret_val;
  }

  _XmAppUnlock(app);
  return False;

}

void
XmTextFieldClearSelection(Widget w,
			  Time sel_time)
{
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  _XmTextFieldDeselectSelection(w, False, sel_time);
  _XmAppUnlock(app);
}

void
XmTextFieldSetSelection(Widget w,
			XmTextPosition first,
			XmTextPosition last,
			Time sel_time)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  TextFieldResetIC(w);
  tf->text.take_primary = True;
  _XmTextFieldStartSelection(tf, first, last, sel_time);
  tf->text.pending_off = False;
  SetCursorPosition(tf, NULL, last, True, True, False, DontCare);
  _XmAppUnlock(app);
}

XmTextPosition
XmTextFieldXYToPos(Widget w,
                   Position x,
                   Position y)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  XmTextPosition ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = GetPosFromX(tf, x);
  _XmAppUnlock(app);
  return (ret_val);
}

Boolean
XmTextFieldPosToXY(Widget w,
		   XmTextPosition position,
		   Position *x,
		   Position *y)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  ret_val = GetXYFromPos(tf, position, x, y);
  _XmAppUnlock(app);
  return (ret_val);
}


/*
 * Force the given position to be displayed.  If position is out of bounds,
 * then don't force any position to be displayed.
 */
void
XmTextFieldShowPosition(Widget w,
			XmTextPosition position)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if ( (position < 0) || (position > tf->text.string_length) ) {
	_XmAppUnlock(app);
	return;
  }

  AdjustText(tf, position, True);
  _XmAppUnlock(app);
}

void
XmTextFieldSetHighlight(Widget w,
			XmTextPosition left,
			XmTextPosition right,
			XmHighlightMode mode)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  doSetHighlight(w, left, right, mode);
  tf->text.programmatic_highlights = True;
  _XmAppUnlock(app);
}

static Boolean
TrimHighlights(XmTextFieldWidget tf, int *low, int *high)
{
 	/*
 	** We have a situation in which the programmer has called
 	** XmTextFieldSetHighlight and the user is now interacting with the
 	** text, which has the possible effect of mis-inserting and doing all
 	** sorts of nasty stuff, mostly because this widget assumes that such
 	** settings are ephemeral and last only as long as user interaction.
 	** As programmer-defined highlights are assumed to be reasonable only
 	** for e.g. non-editable text areas, reset them.
 	*/

	Boolean changed = False;
 	Boolean justChanged = False;
 	_XmHighlightRec *l = tf->text.highlight.list;
 	int i;

 	for (i=0; i < tf->text.highlight.number; i++)
 	{
 		/* iterate through list, resetting spurious back to normal;
 		** unfortunately, we can have has_primary even when there is
 		** no primary selection anymore, so check pending-deleteness
 		*/
 		if (justChanged)
 			*high = l[i].position;
 		if (((XmHIGHLIGHT_SECONDARY_SELECTED == l[i].mode) && !tf->text.has_secondary)
 		  ||((XmHIGHLIGHT_SELECTED == l[i].mode) && !NeedsPendingDelete(tf)))
 			{
 			l[i].mode = XmHIGHLIGHT_NORMAL;
 			if (!changed)
 				*low = l[i].position;
 			changed = True;
 			justChanged = True;
 			}
 		else
 			justChanged = False;
 	}
 	if (justChanged)
 		*high = tf->text.string_length;

 	if (changed)
 	{
  	  int j;
 	  /* coalescing blocks; reduce number only */
 	  i = 1;
 	  while (i < tf->text.highlight.number) {
 	    if (l[i].mode == l[i-1].mode) {
 	      tf->text.highlight.number--;
 	      for (j=i; j<tf->text.highlight.number; j++)
 		l[j] = l[j+1];
 	    } else i++;
 	  }
 	}

 	return changed;
}

static void
doSetHighlight(Widget w, XmTextPosition left, XmTextPosition right,
			XmHighlightMode mode)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;

  /* If right position is out-bound, change it to the last position. */
  if (right > tf->text.string_length)
    right = tf->text.string_length;

  /* If left is out-bound, don't do anything. */
  if (left >= right || right <= 0) {
     return;
  }

  if (left < 0) left = 0;

  TextFieldSetHighlight(tf, left, right, mode);

  RedisplayText(tf, left, right);
}

int
XmTextFieldGetBaseline(Widget w)
{
  XmTextFieldWidget tf = (XmTextFieldWidget) w;
  Dimension margin_top;
  int ret_val;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  margin_top = tf->text.margin_top +
    tf->primitive.shadow_thickness +
      tf->primitive.highlight_thickness;

  ret_val = (int) margin_top + (int) TextF_FontAscent(tf);
  _XmAppUnlock(app);

  return(ret_val);
}
/*
 * Function:
 *    XmCreateTextField()
 *    XmVaCreateTextField()
 *    XmVaCreateManagedTextField()
 *
 *  Description:
 *      Basic creation routines for the motif TextField Widget Class.
 */

Widget
XmCreateTextField(Widget parent,
		  char *name,
		  ArgList arglist,
		  Cardinal argcount)
{
  return (XtCreateWidget(name, xmTextFieldWidgetClass,
			 parent, arglist, argcount));
}

Widget
XmVaCreateTextField(
        Widget parent,
        char *name,
        ...)
{
    Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmTextFieldWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}

Widget
XmVaCreateManagedTextField(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmTextFieldWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

