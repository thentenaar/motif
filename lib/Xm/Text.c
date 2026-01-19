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
static char rcsid[] = "$TOG: Text.c /main/47 1999/01/26 15:18:26 mgreess $"
#endif
#endif
#define TEXT

#include <string.h>
#include <X11/Xos.h>
#include <X11/keysymdef.h>
#include <Xm/AccTextT.h>
#include <Xm/BaseClassP.h>
#include <Xm/CutPaste.h>
#include <Xm/Display.h>
#include <Xm/DropSMgr.h>
#include <Xm/ManagerP.h>
#include <Xm/ScrolledW.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/XmosP.h>
#include <Xm/VaSimpleP.h>
#include "MessagesI.h"
#include "RepTypeI.h"
#include "TextI.h"
#include "TextInI.h"
#include "TextOutI.h"
#include "TextSelI.h"
#include "TextStrSoI.h"
#include "VendorSEI.h"
#include "XmI.h"
#include "XmStringI.h"

/* Resolution independence conversion functions */

#define MESSAGE2	_XmMMsgText_0000

/* Memory Management for global line table */
#define INIT_TABLE_SIZE		  64
#define TABLE_INCREMENT		1024
#define XmDYNAMIC_BOOL		 255

/* Change ChangeVSB() and RedisplayHBar from TextOut.c to non-static functions;
 * they are needed for updating the scroll bars after re-enable redisplay.
 * DisableRedisplay prohibits the visuals of the widget from being updated
 * as the widget's contents are changed.  If the widget is a scrolled widget,
 * this change prohibits the scroll bars from being updated until redisplay
 * is re-enabled.
 */
extern void _XmChangeVSB(XmTextWidget widget);
extern void _XmRedisplayHBar(XmTextWidget widget);
extern void _XmChangeHSB(XmTextWidget widget);
extern void _XmRedisplayVBar(XmTextWidget widget);

/********    Static Function Declarations    ********/

static void NullAddWidget(XmTextSource source,
			  XmTextWidget tw);

static void NullRemoveWidget(XmTextSource source,
			     XmTextWidget tw);

static XmTextPosition NullRead(XmTextSource source,
			       XmTextPosition position,
			       XmTextPosition last_position,
			       XmTextBlock block);

static XmTextStatus NullReplace(XmTextWidget tw,
				XEvent *event,
				XmTextPosition *start,
				XmTextPosition *end,
				XmTextBlock block,
                                Boolean call_callbacks);

static XmTextPosition NullScan(XmTextSource source,
                               XmTextPosition position,
                               XmTextScanType sType,
                               XmTextScanDirection dir,
			       int n,
                               Boolean include);

static Boolean NullGetSelection(XmTextSource source,
                                XmTextPosition *start,
                                XmTextPosition *end);

static void NullSetSelection(XmTextSource source,
			     XmTextPosition start,
			     XmTextPosition end,
			     Time time);

static void _XmCreateCutBuffers(Widget w);

static Cardinal GetSecResData(WidgetClass w_class,
			      XmSecondaryResourceData **secResDataRtn);

static void ClassPartInitialize(WidgetClass wc);

static void ClassInitialize(void);

static void AddRedraw(XmTextWidget tw,
		      XmTextPosition left,
		      XmTextPosition right);

static _XmHighlightRec * FindHighlight(XmTextWidget tw,
				       XmTextPosition position,
				       XmTextScanDirection dir);

static void DisplayText(XmTextWidget tw,
                        XmTextPosition updateFrom,
                        XmTextPosition updateTo);

static void RedrawChanges(XmTextWidget tw);

static void DoMove(XmTextWidget tw,
		   int startcopy,
		   int endcopy,
		   int destcopy);

static void RefigureLines(XmTextWidget tw);

static void RemoveLines(XmTextWidget tw,
                        int num_lines,
                        unsigned int cur_index);

static void AddLines(XmTextWidget tw,
		     XmTextLineTable temp_table,
		     unsigned int tmp_index,
		     unsigned int current_index);

static void InitializeLineTable(XmTextWidget tw,
				int size);

static void FindHighlightingChanges(XmTextWidget tw);

static void Redisplay(XmTextWidget tw);

static void InsertHighlight(XmTextWidget tw,
			    XmTextPosition position,
			    XmHighlightMode mode);

static void Initialize(Widget rw,
		       Widget nw,
		       ArgList args,
		       Cardinal *num_args);

static void InitializeHook(Widget wid,
			   ArgList args,
			   Cardinal *num_args_ptr);

static void Realize(Widget w,
		    XtValueMask *valueMask,
		    XSetWindowAttributes *attributes);

static void Destroy(Widget w);

static void Resize(Widget w);

static void DoExpose(Widget w,
		     XEvent *event,
		     Region region);

static void GetValuesHook(Widget w,
			  ArgList args,
			  Cardinal *num_args_ptr);

static Boolean SetValues(Widget oldw,
			 Widget reqw,
			 Widget new_w,
			 ArgList args,
			 Cardinal *num_args);

static XtGeometryResult QueryGeometry(Widget w,
				      XtWidgetGeometry *intended,
				      XtWidgetGeometry *reply);

static void _XmTextSetString(Widget widget,
			     char *value);

static XtPointer TextGetValue(Widget w,
			      int format);

static void TextSetValue(Widget w,
			 XtPointer s,
			 int format);

static int TextPreferredValue(Widget w);

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

static void ResetUnder(XmTextWidget tw);

/********    End Static Function Declarations    ********/

/*
 * For resource list management.
 */

static XmTextSourceRec nullsource;
static XmTextSource nullsourceptr = &nullsource;

#define _XmTextEventBindings1	_XmTextIn_XmTextEventBindings1
#define _XmTextEventBindings2	_XmTextIn_XmTextEventBindings2
#define _XmTextEventBindings3	_XmTextIn_XmTextEventBindings3
#define _XmTextVEventBindings	_XmTextIn_XmTextVEventBindings

#define EraseInsertionPoint(tw)\
{\
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);\
}

#define TextDrawInsertionPoint(tw)\
{\
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);\
}

static XtResource resources[] =
{
  {
    XmNsource, XmCSource, XmRPointer, sizeof(XtPointer),
    XtOffsetOf(struct _XmTextRec, text.source),
    XmRPointer, (XtPointer) &nullsourceptr
  },

  {
    XmNactivateCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.activate_callback),
    XmRCallback, NULL
  },

  {
    XmNfocusCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.focus_callback),
    XmRCallback, NULL
  },

  {
    XmNlosingFocusCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.losing_focus_callback),
    XmRCallback, NULL
  },

  {
    XmNvalueChangedCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.value_changed_callback),
    XmRCallback, NULL
  },

  {
    XmNdestinationCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.destination_callback),
    XmRCallback, NULL
  },

  {
    XmNmodifyVerifyCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.modify_verify_callback),
    XmRCallback, NULL
  },

  {
    XmNmodifyVerifyCallbackWcs, XmCCallback, XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.wcs_modify_verify_callback),
    XmRCallback, NULL
  },

  {
    XmNmotionVerifyCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.motion_verify_callback),
    XmRCallback, NULL
  },

  {
    XmNgainPrimaryCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.gain_primary_callback),
    XmRCallback, NULL
  },

  {
    XmNlosePrimaryCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
    XtOffsetOf(struct _XmTextRec, text.lose_primary_callback),
    XmRCallback, NULL
  },

  {
    XmNvalue, XmCValue, XmRString, sizeof(String),
    XtOffsetOf(struct _XmTextRec, text.value),
    XmRString, ""
  },

  {
    XmNvalueWcs, XmCValueWcs, XmRValueWcs, sizeof(wchar_t*),
    XtOffsetOf(struct _XmTextRec, text.wc_value),
    XmRString, NULL
  },

  {
    XmNmaxLength, XmCMaxLength, XmRInt, sizeof(int),
    XtOffsetOf(struct _XmTextRec, text.max_length),
    XmRImmediate, (XtPointer) INT_MAX
  },

  {
    XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension, sizeof(Dimension),
    XtOffsetOf(struct _XmTextRec, text.margin_height),
    XmRImmediate, (XtPointer) 5
  },

  {
    XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
    XtOffsetOf(struct _XmTextRec, text.margin_width),
    XmRImmediate, (XtPointer) 5
  },

  {
    XmNoutputCreate, XmCOutputCreate,  XmRFunction, sizeof(OutputCreateProc),
    XtOffsetOf(struct _XmTextRec, text.output_create),
    XmRFunction, (XtPointer) NULL
  },

  {
    XmNinputCreate, XmCInputCreate, XmRFunction, sizeof(InputCreateProc),
    XtOffsetOf(struct _XmTextRec, text.input_create),
    XmRFunction, (XtPointer) NULL
  },

  {
    XmNtopCharacter, XmCTopCharacter, XmRTextPosition, sizeof(XmTextPosition),
    XtOffsetOf(struct _XmTextRec, text.top_character),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNcursorPosition, XmCCursorPosition, XmRTextPosition,
    sizeof (XmTextPosition),
    XtOffsetOf(struct _XmTextRec, text.cursor_position),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNeditMode, XmCEditMode, XmREditMode, sizeof(int),
    XtOffsetOf(struct _XmTextRec, text.edit_mode),
    XmRImmediate, (XtPointer) XmSINGLE_LINE_EDIT
  },

  {
    XmNautoShowCursorPosition, XmCAutoShowCursorPosition, XmRBoolean,
    sizeof(Boolean),
    XtOffsetOf(struct _XmTextRec, text.auto_show_cursor_position),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNeditable, XmCEditable, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmTextRec, text.editable),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNverifyBell, XmCVerifyBell, XmRBoolean, sizeof(Boolean),
    XtOffsetOf(struct _XmTextRec, text.verify_bell),
    XmRImmediate, (XtPointer) XmDYNAMIC_BOOL
  },

  {
    XmNnavigationType, XmCNavigationType, XmRNavigationType,
    sizeof (unsigned char),
    XtOffsetOf(struct _XmPrimitiveRec, primitive.navigation_type),
    XmRImmediate, (XtPointer) XmTAB_GROUP
  },

  {
    XmNtotalLines, XmCTotalLines, XmRInt,
    sizeof (int),
    XtOffsetOf(struct _XmTextRec, text.total_lines),
    XmRImmediate, (XtPointer) 1
  },


};

/* Definition for resources that need special processing in get values */

static XmSyntheticResource get_resources[] =
{
  {
    XmNmarginWidth,
    sizeof(Dimension),
    XtOffsetOf(struct _XmTextRec, text.margin_width),
    XmeFromHorizontalPixels,
    XmeToHorizontalPixels
  },

  {
    XmNmarginHeight,
    sizeof(Dimension),
    XtOffsetOf(struct _XmTextRec, text.margin_height),
    XmeFromVerticalPixels,
    XmeToVerticalPixels
  },
};

static XmBaseClassExtRec       textBaseClassExtRec = {
  NULL,                                     /* Next extension       */
  NULLQUARK,                                /* record type XmQmotif */
  XmBaseClassExtVersion,                    /* version              */
  sizeof(XmBaseClassExtRec),                /* size                 */
  XmInheritInitializePrehook,               /* initialize prehook   */
  XmInheritSetValuesPrehook,                /* set_values prehook   */
  XmInheritInitializePosthook,              /* initialize posthook  */
  XmInheritSetValuesPosthook,               /* set_values posthook  */
  XmInheritClass,   		      	    /* secondary class      */
  XmInheritSecObjectCreate,                 /* creation proc        */
  GetSecResData,                	    /* getSecResData 	      */
  {0},                                      /* fast subclass        */
  XmInheritGetValuesPrehook,                /* get_values prehook   */
  XmInheritGetValuesPosthook,               /* get_values posthook  */
  XmInheritClassPartInitPrehook,            /* classPartInitPrehook */
  XmInheritClassPartInitPosthook,           /* classPartInitPosthook*/
  NULL,                                     /* ext_resources        */
  NULL,                                     /* compiled_ext_resources*/
  0,                                        /* num_ext_resources    */
  FALSE,                                    /* use_sub_resources    */
  XmInheritWidgetNavigable,                 /* widgetNavigable      */
  XmInheritFocusChange,                     /* focusChange          */
  NULL,		      		            /* wrapperData 	      */
};

static XmPrimitiveClassExtRec _XmTextPrimClassExtRec = {
  NULL,
  NULLQUARK,
  XmPrimitiveClassExtVersion,
  sizeof(XmPrimitiveClassExtRec),
  _XmTextGetBaselines,                  /* widget_baseline */
  _XmTextGetDisplayRect,                /* widget_display_rect */
  _XmTextMarginsProc,			/* get/set widget margins */
};

externaldef(xmtextclassrec) XmTextClassRec xmTextClassRec = {
  {
/* core_class fields */
    /* superclass	  */	(WidgetClass) &xmPrimitiveClassRec,
    /* class_name	  */	"XmText",
    /* widget_size	  */	sizeof(XmTextRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initiali*/	ClassPartInitialize,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */	InitializeHook,
    /* realize		  */	Realize,
    /* actions		  */    NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressMaximal,
    /* compress_enterleave*/	TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	Destroy,
    /* resize		  */	Resize,
    /* expose		  */	DoExpose,
    /* set_values	  */	SetValues,
    /* set_values_hook	  */	NULL,
    /* set_values_almost  */	XtInheritSetValuesAlmost,
    /* get_values_hook    */	GetValuesHook,
    /* accept_focus	  */	NULL,
    /* version		  */	XtVersion,
    /* callback_private   */	NULL,
    /* tm_table		  */	NULL,
    /* query_geometry     */    QueryGeometry,
    /* display accel	  */	NULL,
    /* extension	  */	(XtPointer)&textBaseClassExtRec,
  },

/* primitive_class fields  */
  {
    XmInheritBorderHighlight,             /* Primitive border_highlight   */
    XmInheritBorderUnhighlight,           /* Primitive border_unhighlight */
    NULL,         		          /* translations                 */
    NULL,         		          /* arm_and_activate             */
    get_resources,	    	          /* get resources 	          */
    XtNumber(get_resources),	          /* num get_resources            */
    (XtPointer) &_XmTextPrimClassExtRec,  /* extension                    */
  },

  {				/* text class fields */
    NULL,             	        /* extension         */
  }
};

externaldef(xmtextwidgetclass) WidgetClass xmTextWidgetClass =
					 (WidgetClass) &xmTextClassRec;

/****************************************************************
 *
 * Definitions for the null source.
 *
 ****************************************************************/

static void
NullAddWidget(XmTextSource source,
	      XmTextWidget tw)
{
}

static void
NullRemoveWidget(XmTextSource source,
		 XmTextWidget tw)
{
}

static XmTextPosition
NullRead(XmTextSource source,
	 XmTextPosition position,
	 XmTextPosition last_position,
	 XmTextBlock block)
{
  block->ptr = NULL;
  block->length = 0;
  block->format = XmFMT_8_BIT;

  return 0;
}

static XmTextStatus
NullReplace(XmTextWidget tw,
	    XEvent * event,
	    XmTextPosition *start,
	    XmTextPosition *end,
	    XmTextBlock block,
            Boolean call_callbacks)
{
  return EditError;
}

static XmTextPosition
NullScan(XmTextSource source,
	 XmTextPosition position,
	 XmTextScanType sType,
	 XmTextScanDirection dir,
	 int n,
         Boolean include)
{
  return 0;
}

static Boolean
NullGetSelection(XmTextSource source,
		 XmTextPosition *start,
		 XmTextPosition *end)
{
  return FALSE;
}

static void
NullSetSelection(XmTextSource source,
		 XmTextPosition start,
		 XmTextPosition end,
		 Time time)
{
}

static void
_XmCreateCutBuffers(Widget w)
{
  static XContext context = 0;
  char * tmp = NULL;
  Display *dpy = XtDisplay(w);
  Screen *screen = XtScreen(w);
  XContext local_context;

  _XmProcessLock();
  if (!context)
    context = XUniqueContext();

  local_context = context;
  _XmProcessUnlock();

  if (XFindContext(dpy, (Window)screen, local_context, &tmp)) {
    XmTextContextData ctx_data;
    Widget xm_display = (Widget) XmGetXmDisplay(dpy);

    ctx_data = (XmTextContextData) XtMalloc(sizeof(XmTextContextDataRec));

    ctx_data->screen = screen;
    ctx_data->context = local_context;
    ctx_data->type = '\0';

    XtAddCallback(xm_display, XmNdestroyCallback,
		  (XtCallbackProc) _XmTextFreeContextData,
		  (XtPointer) ctx_data);

    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER0,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER1,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER2,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER3,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER4,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER5,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER6,
		    XA_STRING, 8, PropModeAppend, NULL, 0);
    XChangeProperty(dpy, RootWindowOfScreen(screen), XA_CUT_BUFFER7,
		    XA_STRING, 8, PropModeAppend, NULL, 0);

    XSaveContext(dpy, (Window)screen, local_context, tmp);
  }
}

/****************************************************************
 *
 * Private definitions.
 *
 ****************************************************************/
/************************************************************************
 *
 *  GetSecResData
 *
 ************************************************************************/
static Cardinal
GetSecResData(WidgetClass w_class,
	      XmSecondaryResourceData **secResDataRtn)
{
  XmSecondaryResourceData               *secResDataPtr;

  secResDataPtr =
    (XmSecondaryResourceData *) XtMalloc(sizeof(XmSecondaryResourceData) * 2);

  _XmTextInputGetSecResData(&secResDataPtr[0]);
  _XmTextOutputGetSecResData(&secResDataPtr[1]);
  *secResDataRtn = secResDataPtr;

  return 2;
}

/*********************************************************************/
/* Trait definitions                                                 */
/*********************************************************************/

/* AccessXmString Trait record for Text */
static const XmAccessTextualTraitRec TextCS = {
  0,  				/* version */
  TextGetValue,			/* getValue */
  TextSetValue,			/* setValue */
  TextPreferredValue,		/* preferredFormat */
};

/****************************************************************
 *
 * ClassPartInitialize
 *     Set up the fast subclassing for the widget. Set up merged
 *     Translation table.
 *
 ****************************************************************/
static void
ClassPartInitialize(WidgetClass wc)
{
  XmTextWidgetClass twc = (XmTextWidgetClass) wc;
  WidgetClass super;
  XmPrimitiveClassExt *wcePtr, *scePtr;
  char * event_bindings;

  _XmProcessLock();
  super = twc->core_class.superclass;
  wcePtr = _XmGetPrimitiveClassExtPtr(wc, NULLQUARK);
  scePtr = _XmGetPrimitiveClassExtPtr(super, NULLQUARK);

  if ((*wcePtr)->widget_baseline == XmInheritBaselineProc)
    (*wcePtr)->widget_baseline = (*scePtr)->widget_baseline;

  if ((*wcePtr)->widget_display_rect == XmInheritDisplayRectProc)
    (*wcePtr)->widget_display_rect  = (*scePtr)->widget_display_rect;

  event_bindings = (char *)XtMalloc(strlen(_XmTextEventBindings1) +
				    strlen(_XmTextEventBindings2) +
				    strlen(_XmTextEventBindings3) + 1);
  strcpy(event_bindings, _XmTextEventBindings1);
  strcat(event_bindings, _XmTextEventBindings2);
  strcat(event_bindings, _XmTextEventBindings3);
  xmTextClassRec.core_class.tm_table =
    (String) XtParseTranslationTable(event_bindings);

  XtFree(event_bindings);

  _XmFastSubclassInit (wc, XmTEXT_BIT);
  _XmProcessUnlock();
}

/****************************************************************
 *
 * ClassInitialize
 *
 *
 ****************************************************************/
static void
ClassInitialize(void)
{
  xmTextClassRec.core_class.actions =
    (XtActionList)_XmdefaultTextActionsTable;
  xmTextClassRec.core_class.num_actions = _XmdefaultTextActionsTableSize;

  nullsource.AddWidget = NullAddWidget;
  nullsource.RemoveWidget = NullRemoveWidget;
  nullsource.ReadSource = NullRead;
  nullsource.Replace = NullReplace;
  nullsource.Scan = NullScan;
  nullsource.GetSelection = NullGetSelection;
  nullsource.SetSelection = NullSetSelection;

  textBaseClassExtRec.record_type = XmQmotif;
  /* Install traits */
  _XmTextInstallTransferTrait();
  XmeTraitSet((XtPointer)xmTextWidgetClass, XmQTaccessTextual,
	      (XtPointer) &TextCS);
}


/*
 * Mark the given range of text to be redrawn.
 */
static void
AddRedraw(XmTextWidget tw,
	  XmTextPosition left,
	  XmTextPosition right)
{
  RangeRec *r = tw->text.repaint.range;
  int i;

  if (left == tw->text.last_position &&
      tw->text.output->data->number_lines >= 1)
    left = (*tw->text.source->Scan)(tw->text.source, left,
					XmSELECT_POSITION, XmsdLeft, 1, TRUE);

  if (left < right) {
    for (i = 0; i < tw->text.repaint.number; i++) {
      if (left <= r[i].to && right >= r[i].from) {
	r[i].from = MIN(left, r[i].from);
	r[i].to = MAX(right, r[i].to);
	return;
      }
    }
    if (tw->text.repaint.number >= tw->text.repaint.maximum) {
      tw->text.repaint.maximum = tw->text.repaint.number + 1;
      tw->text.repaint.range = r = (RangeRec *)
	XtRealloc((char *)r, tw->text.repaint.maximum * sizeof(RangeRec));
    }
    r[tw->text.repaint.number].from = left;
    r[tw->text.repaint.number].to = right;
    tw->text.repaint.number++;
  }
}

/*
 * Find the highlight record corresponding to the given position.  Returns a
 * pointer to the record.  The third argument indicates whether we are probing
 * the left or right edge of a highlighting range.
 */
static _XmHighlightRec *
FindHighlight(XmTextWidget tw,
	      XmTextPosition position,
	      XmTextScanDirection dir)
{
  _XmHighlightRec *l = tw->text.highlight.list;
  int i;
  if (dir == XmsdLeft) {
    for (i=tw->text.highlight.number - 1; i>=0;  i--)
      if (position >= l[i].position) {
	l = l + i;
	break;
      }
  } else {
    for (i=tw->text.highlight.number - 1; i>=0; i--)
      if (position > l[i].position) {
	l = l + i;
	break;
      }
  }
  return(l);
}

/*
 * Redraw the specified range of text.  Should only be called by
 * RedrawChanges(), below (as well as calling itself recursively).
 */
static void
DisplayText(XmTextWidget tw,
	    XmTextPosition updateFrom,
	    XmTextPosition updateTo)
{
  LineNum i;
  XmTextPosition nextstart;
  _XmHighlightRec *l1, *l2;

  if (updateFrom < tw->text.top_character)
    updateFrom = tw->text.top_character;
  if (updateTo > tw->text.bottom_position)
    updateTo = tw->text.bottom_position;
  if (updateFrom > updateTo) return;

  l1 = FindHighlight(tw, updateFrom, XmsdLeft);
  l2 = FindHighlight(tw, updateTo, XmsdRight);
  if ( (l1 != l2) && (l1->position != l2->position) ) {
    DisplayText(tw, updateFrom, l2->position);
    updateFrom = l2->position;
  }

  /*
   * Once we get here, we need to paint all of the text from updateFrom to
   * updateTo with current highlightmode.  We have to break this into
   * separate lines, and then call the output routine for each line.
   */

  for (i = _XmTextPosToLine(tw, updateFrom);
       updateFrom <= updateTo && i < tw->text.number_lines;
       i++) {
    nextstart = tw->text.line[i+1].start;
    (*tw->text.output->Draw)(tw, i, updateFrom,
				 MIN(updateTo, nextstart), l2->mode);
    updateFrom = nextstart;
  }
}

/*
 * Redraw the changed areas of the text.  This should only be called by
 * Redisplay(), below.
 */
static void
RedrawChanges(XmTextWidget tw)
{
  RangeRec *r = tw->text.repaint.range;
  XmTextPosition updateFrom, updateTo;
  int w, i;

  EraseInsertionPoint(tw);

  while (tw->text.repaint.number != 0) {
    updateFrom = r[0].from;
    w = 0;
    for (i=1; i<tw->text.repaint.number; i++) {
      if (r[i].from < updateFrom) {
	updateFrom = r[i].from;
	w = i;
      }
    }
    updateTo = r[w].to;
    tw->text.repaint.number--;
    r[w].from = r[tw->text.repaint.number].from;
    r[w].to = r[tw->text.repaint.number].to;
    for (i=tw->text.repaint.number-1; i>=0; i--) {
      while (i < tw->text.repaint.number) {
	updateTo = MAX(r[i].to, updateTo);
	tw->text.repaint.number--;
	r[i].from = r[tw->text.repaint.number].from;
	r[i].to = r[tw->text.repaint.number].to;
      }
    }
    DisplayText(tw, updateFrom, updateTo);
  }
  if (tw->text.first_position == tw->text.last_position) {
    (*tw->text.output->Draw)(tw, (LineNum) 0,
				 tw->text.first_position,
				 tw->text.last_position,
				 XmHIGHLIGHT_NORMAL);
  }
  TextDrawInsertionPoint(tw);
}

static void
DoMove(XmTextWidget tw,
       int startcopy,
       int endcopy,
       int destcopy)
{
  Line line = tw->text.line;
  LineNum i;

  EraseInsertionPoint(tw);
  if (tw->text.disable_depth == 0 &&
      (*tw->text.output->MoveLines)(tw, (LineNum) startcopy,
					(LineNum) endcopy, (LineNum) destcopy))
    {
      TextDrawInsertionPoint(tw);
      return;
    }
  for (i=destcopy; i <= destcopy + endcopy - startcopy; i++)
    AddRedraw(tw, line[i].start, line[i+1].start);
  TextDrawInsertionPoint(tw);
}


/*
 * Find the starting position of the line that is delta lines away from the
 * line starting with position start.
 */
XmTextPosition
_XmTextFindScroll(XmTextWidget tw,
		  XmTextPosition start,
		  int delta)
{
  XmTextLineTable line_table;
  unsigned int t_index;
  unsigned int max_index = 0;

  if (tw->text.total_lines <= tw->text.table_index)
    tw->text.table_index = tw->text.total_lines - 1;


  line_table = tw->text.line_table;
  t_index = tw->text.table_index;

  max_index = tw->text.total_lines - 1;



  /* look forward to find the current record */
  if (line_table[t_index].start_pos < (unsigned int) start) {
    while (t_index <= max_index &&
	   line_table[t_index].start_pos < (unsigned int) start) t_index++;
	   /* special handling if last lines of text are blank */
           if (t_index <= max_index &&
                (line_table[t_index].start_pos == tw->text.last_position) &&
	  	(tw->text.number_lines == -delta) && t_index == max_index)
	  	t_index++;
  } else
    /* look backward to find the current record */
    while (t_index &&
	   line_table[t_index].start_pos > (unsigned int) start) t_index--;

  if (delta > 0) {
    t_index += delta;
    if (t_index > tw->text.total_lines - 1)
      t_index = tw->text.total_lines - 1;
  } else {
    if (t_index > -delta)
      t_index += delta;
    else
      t_index = 0;
  }

  start = line_table[t_index].start_pos;

  tw->text.table_index = t_index;

  return start;
}

/*
 * Refigure the line breaks in this widget.
 */
static void
RefigureLines(XmTextWidget tw)
{
  Line line = tw->text.line;
  LineNum i, j;
  Line oldline = NULL;
  static XmTextPosition tell_output_force_display = -1;
  int oldNumLines = tw->text.number_lines;
  int startcopy, endcopy, destcopy, lastcopy; /* %%% Document! */

  if (tw->text.in_refigure_lines || !tw->text.needs_refigure_lines)
    return;
  tw->text.in_refigure_lines = TRUE;
  tw->text.needs_refigure_lines = FALSE;
  if (XtIsRealized((Widget)tw)) EraseInsertionPoint(tw);
  oldline = (Line) XtMalloc((oldNumLines + 2) * sizeof(LineRec));

  memcpy((void *) oldline, (void *) line,
	 (size_t) (oldNumLines + 1) * sizeof(LineRec));


  if (tw->text.pending_scroll != 0) {
    tw->text.new_top = _XmTextFindScroll(tw, tw->text.new_top,
					     tw->text.pending_scroll);
    tw->text.pending_scroll = 0;
  }
  if (tw->text.new_top < tw->text.first_position)
    tw->text.new_top = tw->text.first_position;
  line[0].start = tw->text.top_character = tw->text.new_top;
  line[0].past_end = FALSE;
  line[0].extra = NULL;

  tw->text.number_lines = 0;
  j = 0;
  startcopy = endcopy = lastcopy = destcopy = -99;
  for (i = 0; i == 0 || !line[i-1].past_end; i++) {
    if (i+2 > tw->text.maximum_lines) {
      tw->text.maximum_lines = i+2;
      line = tw->text.line = (Line)
	XtRealloc((char *)line,
		  tw->text.maximum_lines * sizeof(LineRec));
    }
    while (j < oldNumLines && oldline[j].start < line[i].start)
      j++;
    if (j < oldNumLines && oldline[j].start >= oldline[j+1].start)
      j = oldNumLines;
    if (j >= oldNumLines)
      oldline[j].start = -1; /* Make comparisons fail. */
    if (line[i].start >= tw->text.forget_past ||
	line[i].start != oldline[j].start ||
	oldline[j].changed ||
	oldline[j+1].changed) {
      line[i].past_end =
	!(*tw->text.output->MeasureLine)(tw, i, line[i].start,
					     &line[i+1].start, &line[i].extra);
      line[i+1].extra = NULL;
      if (!line[i].past_end &&
	  (line[i+1].start == PASTENDPOS) &&
	  (line[i].start != PASTENDPOS))
	AddRedraw(tw, line[i].start, tw->text.last_position);
    } else {
      line[i] = oldline[j];
      oldline[j].extra = NULL;
      line[i].past_end =
	!(*tw->text.output->MeasureLine)(tw, i, line[i].start,
					     NULL, NULL);

      line[i+1].start = oldline[j+1].start;
      line[i+1].extra = oldline[j+1].extra;
    }
    if (!line[i].past_end) {
      if (line[i].start != oldline[j].start ||
	  line[i+1].start != oldline[j+1].start ||
	  line[i].start >= tw->text.forget_past) {
	AddRedraw(tw, line[i].start, line[i+1].start);
      } else {
	if (i != j && line[i+1].start >= tw->text.last_position)
	  AddRedraw(tw, tw->text.last_position,
		    tw->text.last_position);
	if (oldline[j].changed)
	  AddRedraw(tw, oldline[j].changed_position,
		    line[i+1].start);
	if (i != j && line[i].start != PASTENDPOS) {
	  if (endcopy == j-1) {
	    endcopy = j;
	    lastcopy++;
	  } else if (lastcopy >= 0 && j <= lastcopy) {
	    /* This line was stomped by a previous move. */
	    AddRedraw(tw, line[i].start, line[i+1].start);
	  } else {
	    if (startcopy >= 0)
	      DoMove(tw, startcopy, endcopy, destcopy);
	    startcopy = endcopy = j;
	    destcopy = lastcopy = i;
	  }
	}
      }
    }
    line[i].changed = FALSE;
    if (!line[i].past_end) tw->text.number_lines++;
    else tw->text.bottom_position =
      MIN(line[i].start, tw->text.last_position);
  }
  if (startcopy >= 0) {
    DoMove(tw, startcopy, endcopy, destcopy);
  }
  for (j=0; j<=oldNumLines; j++)
    if (oldline[j].extra) {
      XtFree((char *) oldline[j].extra);
      oldline[j].extra = NULL;
    }

  XtFree((char *)oldline); /* XTHREADS */
  tw->text.in_refigure_lines = FALSE;
  if (tw->text.top_character >= tw->text.last_position &&
      tw->text.last_position > tw->text.first_position &&
      tw->text.output->data->number_lines > 1) {
    tw->text.pending_scroll = -1; /* Try to not ever display nothing. */
    tw->text.needs_refigure_lines = TRUE;
  }
  if (tw->text.force_display >= 0) {
    if (tw->text.force_display < tw->text.top_character) {
      if (tw->text.force_display > tw->text.first_position)
        tw->text.new_top = tw->text.force_display + 1 ;
      else
        tw->text.new_top = tw->text.first_position;
      tw->text.pending_scroll--;
    } else if (tw->text.force_display > tw->text.bottom_position) {
      /* need to add one to account for border condition,
       * i.e. cursor at begginning of line
       */
      if (tw->text.force_display < tw->text.last_position)
	tw->text.new_top = tw->text.force_display + 1;
      else
	tw->text.new_top = tw->text.last_position;
      tw->text.pending_scroll -= tw->text.number_lines;
    } else if (tw->text.force_display ==
	       line[tw->text.number_lines].start) {
      tw->text.new_top = tw->text.force_display;
      tw->text.pending_scroll -= (tw->text.number_lines - 1);
    }
    tw->text.needs_refigure_lines = TRUE;
    _XmProcessLock();
    tell_output_force_display = tw->text.force_display;
    _XmProcessUnlock();
    tw->text.force_display = -1;
  }
  if (tw->text.needs_refigure_lines) {
    RefigureLines(tw);
    if (XtIsRealized((Widget)tw)) TextDrawInsertionPoint(tw);
    return;
  }
  AddRedraw(tw, tw->text.forget_past, tw->text.bottom_position);
  tw->text.forget_past = LONG_MAX;
  _XmProcessLock();
  if (tell_output_force_display >= 0) {
    (*tw->text.output->MakePositionVisible)(tw,
					tell_output_force_display);
    tell_output_force_display = -1;
  }
  _XmProcessUnlock();
  if (XtIsRealized((Widget)tw)) TextDrawInsertionPoint(tw);
}

/************************************************************************
 *
 * RemoveLines() - removes the lines from the global line table.
 *      widget - the widget that contains the global table.
 *      num_lines - number of lines to be removed.
 *      cur_line - pointer to the start of the lines to be removed.
 *
 ************************************************************************/
static void
RemoveLines(XmTextWidget tw,
	    int num_lines,
	    unsigned int cur_index)
{
  if (!num_lines) return;

  /* move the existing lines at the end of the buffer */
  if (tw->text.total_lines > cur_index)
    memmove((void *) &tw->text.line_table[cur_index - num_lines],
	    (void *) &tw->text.line_table[cur_index],
	    (size_t) ((tw->text.total_lines - (cur_index)) *
		      sizeof (XmTextLineTableRec)));

  /* reduce total line count */
  tw->text.total_lines -= num_lines;

  /* fix for bug 5166 */
  if (tw->text.total_lines <= tw->text.table_index)
    tw->text.table_index = tw->text.total_lines - 1;


  /* Shrink Table if Necessary */
  if ((tw->text.table_size > TABLE_INCREMENT &&
       tw->text.total_lines <= tw->text.table_size-TABLE_INCREMENT) ||
      tw->text.total_lines <= tw->text.table_size >> 1) {

    tw->text.table_size = INIT_TABLE_SIZE;

    while (tw->text.total_lines >= tw->text.table_size) {
      if (tw->text.table_size < TABLE_INCREMENT)
	tw->text.table_size *= 2;
      else
	tw->text.table_size += TABLE_INCREMENT;
    }

    tw->text.line_table = (XmTextLineTable)
      XtRealloc((char *) tw->text.line_table,
		tw->text.table_size * sizeof(XmTextLineTableRec));
  }
}

static void
AddLines(XmTextWidget tw,
	 XmTextLineTable temp_table,
	 unsigned int tmp_index,
	 unsigned int current_index)
{
  unsigned int i;
  unsigned int size_needed;
  unsigned int cur_index;
  unsigned int temp_index;

  cur_index = current_index;
  temp_index = tmp_index;
  size_needed = tw->text.total_lines + temp_index;

  /* make sure table is big enough to handle the additional lines */
  if (tw->text.table_size < size_needed) {
    while (tw->text.table_size < size_needed)
      if (tw->text.table_size < TABLE_INCREMENT)
	tw->text.table_size *= 2;
      else
	tw->text.table_size += TABLE_INCREMENT;
    tw->text.line_table = (XmTextLineTable)
      XtRealloc((char *) tw->text.line_table,
		tw->text.table_size * sizeof(XmTextLineTableRec));
  }

  /* move the existing lines at the end of the buffer */
  if (tw->text.total_lines > cur_index)
    memmove((void *) &tw->text.line_table[cur_index + temp_index],
	    (void *) &tw->text.line_table[cur_index],
	    (size_t) ((tw->text.total_lines - cur_index) *
		      sizeof (XmTextLineTableRec)));

  tw->text.total_lines += temp_index;

  /* Add the lines from the temp table */
  if (temp_table)
    for (i = 0; i < temp_index; i++, cur_index++)
      tw->text.line_table[cur_index] = temp_table[i];
}

void
_XmTextRealignLineTable(XmTextWidget tw,
			XmTextLineTable *temp_table,
			int *temp_table_size,
			unsigned int cur_index,
			XmTextPosition cur_start,
			XmTextPosition cur_end)

{
  int table_size;
  XmTextPosition line_end;
  XmTextPosition next_start;
  XmTextLineTable line_table;

  if (temp_table) {
    line_table = *temp_table;
    table_size = *temp_table_size;
  } else {
    line_table = tw->text.line_table;
    table_size = tw->text.table_size;
  }

  line_table[cur_index].start_pos = next_start = cur_start;
  cur_index++;

  line_end = (*tw->text.source->Scan)(tw->text.source, cur_start,
					  XmSELECT_LINE, XmsdRight, 1, TRUE);
  while (next_start < cur_end) {
    if (_XmTextShouldWordWrap(tw))
      next_start = _XmTextFindLineEnd(tw, cur_start, NULL);
    else {
      if (cur_start != line_end)
	next_start = line_end;
      else
	next_start = PASTENDPOS;
    }
    if (next_start == PASTENDPOS || next_start == cur_end) break;
    if (next_start == cur_start)
      next_start = (*tw->text.source->Scan) (tw->text.source,
						 cur_start, XmSELECT_POSITION,
						 XmsdRight, 1, TRUE);
    if (cur_index >= table_size) {
      if (table_size < TABLE_INCREMENT)
	table_size *= 2;
      else
	table_size += TABLE_INCREMENT;

      line_table = (XmTextLineTable) XtRealloc((char *)line_table,
					       table_size *
					       sizeof(XmTextLineTableRec));
    }
    line_table[cur_index].start_pos = (unsigned int) next_start;
    if (line_end == next_start) {
      line_table[cur_index].virt_line = 0;
      line_end = (*tw->text.source->Scan)(tw->text.source,
					      next_start, XmSELECT_LINE,
					      XmsdRight, 1, TRUE);
    } else
      line_table[cur_index].virt_line = 1;
    cur_index++;
    cur_start = next_start;
  }

  if (temp_table) {
    *temp_table = line_table;
    *temp_table_size = cur_index;
  } else {
    tw->text.total_lines = cur_index;
    tw->text.line_table = line_table;
    tw->text.table_size = table_size;
  }
}

static void
InitializeLineTable(XmTextWidget tw,
		    int size)
{
  unsigned int t_index;
  XmTextLineTable line_table;

  line_table = (XmTextLineTable) XtMalloc(size * sizeof(XmTextLineTableRec));

  for (t_index = 0; t_index < size; t_index++) {
    line_table[t_index].start_pos = 0;
    line_table[t_index].virt_line = 0;
  }

  tw->text.line_table = line_table;
  tw->text.table_index = 0;
  tw->text.table_size = size;
}

unsigned int
_XmTextGetTableIndex(XmTextWidget tw,
		     XmTextPosition pos)
{
  XmTextLineTable line_table;
  unsigned int cur_index;
  unsigned int max_index;
  XmTextPosition position;

  position = pos;
  max_index = tw->text.total_lines - 1;
  line_table = tw->text.line_table;
  cur_index = tw->text.table_index;

  /* look forward to find the current record */
  if (line_table[cur_index].start_pos < (unsigned int) position) {
    while (cur_index < max_index &&
	   line_table[cur_index].start_pos < (unsigned int) position)
      cur_index++;
    /* if over shot it by one */
    if ((unsigned int) position < line_table[cur_index].start_pos) cur_index--;
  } else
    /* look backward to find the current record */
    while (cur_index &&
	   line_table[cur_index].start_pos > (unsigned int) position)
      cur_index--;

  return (cur_index);
}



void
_XmTextUpdateLineTable(Widget widget,
		       XmTextPosition start,
		       XmTextPosition end,
		       XmTextBlock block,
                       Boolean update)
{
  unsigned int cur_index;
  unsigned int begin_index;
  unsigned int end_index;
  XmTextLineTable line_table;
  unsigned int max_index;
  int lines_avail;
  int length;
  long delta;
  unsigned int start_index;
  unsigned int top_index;
  XmTextWidget tw = (XmTextWidget) widget;
  Boolean word_wrap = _XmTextShouldWordWrap(tw);
  XmTextPosition cur_start, cur_end;
  int diff = 0;
  int block_num_chars = 0;
  int char_size = 0;

  lines_avail = 0;
  max_index = tw->text.total_lines - 1;
  if (tw->text.char_size != 1)
    block_num_chars = _XmTextCountCharacters(block->ptr, block->length);
  else
    block_num_chars = block->length;
  delta = block_num_chars - (end - start);
  length = block_num_chars;

  if (tw->text.line_table == NULL)
  {
    if (tw->text.edit_mode == XmSINGLE_LINE_EDIT)
    {
      InitializeLineTable(tw, 1);
    }
    else
    {
      InitializeLineTable(tw, INIT_TABLE_SIZE);
    }
  }
  /* if there is no change or we expect RelignLineTable()
     to be called before the line table is necessary */
  if ((start == end && length == 0) ||
      (word_wrap && !XtIsRealized(widget)
       && XmIsScrolledWindow(XtParent(widget))
       && XtIsShell(XtParent(XtParent(widget))))) {
    return;
  }

  line_table = tw->text.line_table;

  cur_index = _XmTextGetTableIndex(tw, start);
  top_index = _XmTextGetTableIndex(tw, tw->text.top_character);

  begin_index = start_index = end_index = cur_index;

  if (word_wrap && delta > 0)
    cur_end = end + delta;
  else
    cur_end = end;

  /* Find the cur_end position.
     Count the number of lines that were deleted. */
  if (end > start) {
    if (end_index < tw->text.total_lines) {
      while (end_index < max_index &&
	     line_table[end_index + 1].start_pos <= (unsigned int) cur_end) {
	end_index++;
	lines_avail++;
      }
    } else if (line_table[end_index].start_pos > (unsigned int) start &&
	       line_table[end_index].start_pos <= (unsigned int) cur_end) {
      lines_avail++;
    }
  }

  cur_index = end_index;

  if (word_wrap) {
    int i;
    XmTextLineTable temp_table = NULL;
    int temp_table_size = 0;

    if (line_table[start_index].virt_line) start_index--;

    begin_index = start_index;

    /* get the start position of the line at the start index. */
    cur_start = line_table[begin_index].start_pos;

    /* If we are not at the end of the table, */
    if (cur_index < max_index) {

      /* find the next non-wordwrapped line. */
      while (cur_index < max_index) {
	cur_index++;
	if (!line_table[cur_index].virt_line) break;
      }

      /* Continue only if we have found a non-wordwrapped line. */
      if (!line_table[cur_index].virt_line) {

	  /* Set the cur_end position to the position of
	     the next non-wordwrapped line. */
	  cur_end = line_table[cur_index].start_pos;
	  /* estimate the temp table size - in number of lines */
	  temp_table_size = cur_index - begin_index;
	  /* make sure the size is not zero */
	  if (!temp_table_size) temp_table_size++;
	  /* do initial allocation of the temp_table */
	  temp_table = (XmTextLineTable) XtMalloc(temp_table_size *
						  sizeof(XmTextLineTableRec));
	  /* Determine the lines that have changed. */
	  _XmTextRealignLineTable(tw, &temp_table, &temp_table_size,
				  0, cur_start, cur_end + delta);

	  /* Compute the difference in the number of lines that have changed */
	  diff = temp_table_size - (cur_index - begin_index);

	  /* if new/wrapped lines were added, push line down*/
	  if (diff > 0)
	      AddLines(tw, NULL, diff, cur_index);
	  /* if new/wrapped lines were deleted, move line up */
	  else
	      RemoveLines(tw, -diff, cur_index);

	  /*
	   * The line table may have been realloc'd in any of the three
	   * previous function calls, so it must be reassigned to prevent
	   * a stale pointer.
	   */
	  line_table = tw->text.line_table;

	  /* Bypass the first entry in the temp_table */
	  begin_index++;

	  /* Add the lines from the temp table */
	  for (i = 1; i < temp_table_size; i++, begin_index++)
	      line_table[begin_index] = temp_table[i];

	  /* Free temp table */
	  XtFree((char *)temp_table);

	  /* Adjust the cur_index by the number of lines that changed. */
	  cur_index += diff;
	  max_index += diff;

	  /* Adjust start values in table by the amount of change */
	  while (cur_index <= max_index) {
	      line_table[cur_index].start_pos += delta;
	      cur_index++;
	  }
      } else
	  /* we are at the end of the table */
	  _XmTextRealignLineTable(tw, NULL, 0, begin_index,
				  cur_start, PASTENDPOS);

    } else
      /* add lines to the end */
      _XmTextRealignLineTable(tw, NULL, 0, begin_index,
			      cur_start, PASTENDPOS);
  } else {
    char *ptr;
    XmTextLineTable temp_table;
    int temp_table_size;
    int temp_index;

    temp_table = NULL;
    temp_table_size = 0;
    temp_index = 0;
    ptr = block->ptr;
    cur_start = start;

    while (cur_index < max_index) {
      cur_index++;
      line_table[cur_index].start_pos += delta;
    }

    if (tw->text.char_size == 1) {
      char *nl;
      while (length > 0 && (nl = (char *)memchr(ptr, '\012', length)) != NULL) {
	nl++;
	cur_start += (nl - ptr);
	length -= (nl - ptr);
	ptr = nl;
	if (lines_avail && begin_index < tw->text.total_lines) {
	  begin_index++;
	  lines_avail--;
	  line_table[begin_index].start_pos = (unsigned int)cur_start;
	} else {
	  if (temp_index >= temp_table_size) {
	    if (!temp_table_size) {
	      if (tw->text.output->data->columns > 1) {
		temp_table_size = length /
		  (tw->text.output->data->columns / 2);
		if (!temp_table_size) temp_table_size = 1;
	      } else {
		if (length)
		  temp_table_size = length;
		else
		  temp_table_size = 1;
	      }
	    } else
	      temp_table_size *= 2;
	    temp_table = (XmTextLineTable)XtRealloc((char*)temp_table,
				  temp_table_size * sizeof(XmTextLineTableRec));
	  }
	  temp_table[temp_index].start_pos = (unsigned int) cur_start;
	  temp_table[temp_index].virt_line = (unsigned int) 0;
	  temp_index++;
	}
      }
    } else {
      while (length--) {
	char_size = mblen(ptr, tw->text.char_size);
	if (char_size < 0) break; /* error */
	cur_start++;
	if (char_size == 1 && *ptr == '\012') {
	  ptr++;
	  if (lines_avail && begin_index < tw->text.total_lines) {
	    begin_index++;
	    lines_avail--;
	    line_table[begin_index].start_pos = (unsigned int)cur_start;
	  } else {
	    if (temp_index >= temp_table_size) {
	      if (!temp_table_size) {
		if (tw->text.output->data->columns > 1) {
		  temp_table_size = length /
		    (tw->text.output->data->columns / 2);
		  if (!temp_table_size) temp_table_size = 1;
		} else {
		  if (length)
		    temp_table_size = length;
		  else
		    temp_table_size = 1;
		}
	      } else
		temp_table_size *= 2;
	      temp_table =(XmTextLineTable)XtRealloc((char*)temp_table,
                               temp_table_size * sizeof(XmTextLineTableRec));
	    }
	    temp_table[temp_index].start_pos = (unsigned int) cur_start;
	    temp_table[temp_index].virt_line = (unsigned int) 0;
	    temp_index++;
	  }
	} else {
	  ptr += char_size;
	}
      }
    }

    /* add a block of lines to the line table */
    if (temp_index) {
      AddLines(tw, temp_table, temp_index, begin_index + 1);
    }

    /* remove lines that are no longer necessary */
    if (lines_avail) {
      RemoveLines(tw, lines_avail, end_index + 1);
    }

    /*
     * The line table may have been realloc'd in any of the three
     * previous function calls, so it must be reassigned to prevent
     * a stale pointer.
     */
    line_table = tw->text.line_table;

    diff = temp_index - lines_avail;

    if (temp_table) XtFree((char *)temp_table);
  }

  if (update) {
    if (start < tw->text.top_character) {
      if (end < tw->text.top_character) {
	tw->text.top_line += diff;
	tw->text.new_top = tw->text.top_character + delta;
      } else {
	int adjusted;
	if (diff < 0)
	  adjusted = diff + (top_index - start_index);
	else
	  adjusted = diff - (top_index - start_index);
	tw->text.top_line += adjusted;
	if (adjusted + (int) start_index <= 0) {
	  tw->text.new_top = 0;
	} else if (adjusted + start_index > max_index) {
	  tw->text.new_top = line_table[max_index].start_pos;
	} else {
	  tw->text.new_top = line_table[start_index + adjusted].start_pos;
	}
      }
      tw->text.top_character = tw->text.new_top;
      tw->text.forget_past = MIN(tw->text.forget_past, tw->text.new_top);

      tw->text.top_line = _XmTextGetTableIndex(tw, tw->text.new_top);

      if (tw->text.top_line < 0)
	tw->text.top_line = 0;

      if (tw->text.top_line > tw->text.total_lines)
	tw->text.top_line = tw->text.total_lines - 1;
    }

    if (tw->text.table_index > tw->text.total_lines)
      tw->text.table_index = tw->text.total_lines;

    if (tw->text.on_or_off == on) {
      XmTextPosition cursorPos = tw->text.cursor_position;
      if (start < tw->text.cursor_position) {
        if (tw->text.cursor_position < end) {
	  if (tw->text.cursor_position - start <= block_num_chars)
	    cursorPos = tw->text.cursor_position;
	  else
	    cursorPos = start + block_num_chars;
        } else {
	  cursorPos = tw->text.cursor_position - (end - start) + block_num_chars;
        }
        _XmTextSetCursorPosition(widget, cursorPos);
      } else if (start == tw->text.cursor_position && tw->text.auto_show_cursor_position) {
        _XmTextShowPosition((Widget)tw, cursorPos);
      }
    }
  }
}


/*
 * Compare the old_highlight list and the highlight list, determine what
 * changed, and call AddRedraw with the changed areas.
 */
static void
FindHighlightingChanges(XmTextWidget tw)
{
  int n1 = tw->text.old_highlight.number;
  int n2 = tw->text.highlight.number;
  _XmHighlightRec *l1 = tw->text.old_highlight.list;
  _XmHighlightRec *l2 = tw->text.highlight.list;
  int i1, i2;
  XmTextPosition next1, next2, last_position;

  i1 = i2 = 0;
  last_position = 0;
  while (i1 < n1 && i2 < n2) {
    if (i1 < n1-1) next1 = l1[i1+1].position;
    else next1 = tw->text.last_position;
    if (i2 < n2-1) next2 = l2[i2+1].position;
    else next2 = tw->text.last_position;
    if (l1[i1].mode != l2[i2].mode) {
      AddRedraw(tw, last_position, MIN(next1, next2));
    }
    last_position = MIN(next1, next2);
    if (next1 <= next2) i1++;
    if (next1 >= next2) i2++;
  }
}

/*
 * Actually do some work.  This routine gets called to actually paint all the
 * stuff that has been pending. Prevent recursive calls and text redisplays
 * during destroys
 */
static void
Redisplay(XmTextWidget tw)
{
   /* Prevent recursive calls or text redisplay during detroys. */
  if (tw->text.in_redisplay || tw->core.being_destroyed ||
      tw->text.disable_depth != 0 || !XtIsRealized((Widget)tw)) return;

  EraseInsertionPoint(tw);

  tw->text.in_redisplay = TRUE;

  if (tw->text.needs_refigure_lines) RefigureLines(tw);
  tw->text.needs_redisplay = FALSE;

  if (tw->text.highlight_changed) {
    FindHighlightingChanges(tw);
    tw->text.highlight_changed = FALSE;
  }

  RedrawChanges(tw);

  /* Can be caused by auto-horiz scrolling... */
  if (tw->text.needs_redisplay) {
    RedrawChanges(tw);
    tw->text.needs_redisplay = FALSE;
  }
  tw->text.in_redisplay = FALSE;

  TextDrawInsertionPoint(tw);
}



/****************************************************************
 *
 * Definitions exported to output.
 *
 ****************************************************************/

/*
 * Mark the given range of text to be redrawn.
 */

void
_XmTextMarkRedraw(XmTextWidget tw,
		  XmTextPosition left,
		  XmTextPosition right)
{
  if (left < right) {
    AddRedraw(tw, left, right);
    tw->text.needs_redisplay = TRUE;
    if (tw->text.disable_depth == 0) Redisplay(tw);
  }
}


/*
 * Return the number of lines in the linetable.
 */
LineNum
_XmTextNumLines(XmTextWidget tw)
{
  if (tw->text.needs_refigure_lines) RefigureLines(tw);
  return tw->text.number_lines;
}

void
_XmTextLineInfo(XmTextWidget tw,
		LineNum line,
		XmTextPosition *startpos,
		LineTableExtra *extra)
{
  if (tw->text.needs_refigure_lines) RefigureLines(tw);
  if (tw->text.number_lines >= line) {
    if (startpos) *startpos = tw->text.line[line].start;
    if (extra) *extra = tw->text.line[line].extra;
  } else {
    if (startpos) {
      unsigned int cur_index =
	_XmTextGetTableIndex(tw, tw->text.line[line - 1].start);
      if (cur_index < tw->text.total_lines - 1)
	*startpos = tw->text.line_table[cur_index + 1].start_pos;
      else
	*startpos = tw->text.last_position;
    }
    if (extra) *extra = NULL;
  }
}

/*
 * Return the line number containing the given position.  If text currently
 * knows of no line containing that position, returns NOLINE.
 */
LineNum
_XmTextPosToLine(XmTextWidget tw,
		 XmTextPosition position)
{
  int i;
  if (tw->text.needs_refigure_lines) RefigureLines(tw);
  if (position < tw->text.top_character ||
      position  > tw->text.bottom_position)
    return NOLINE;
  for (i=0; i<tw->text.number_lines; i++)
    if (tw->text.line[i+1].start > position) return i;
  if (position == tw->text.line[tw->text.number_lines].start)
    return tw->text.number_lines;
  return NOLINE;  /* Couldn't find line with given position */
}



/****************************************************************
 *
 * Definitions exported to sources.
 *
 ****************************************************************/
void
_XmTextInvalidate(XmTextWidget tw,
		  XmTextPosition position,
		  XmTextPosition topos,
		  long delta)
{
  LineNum l;
  int i;
  XmTextPosition p, endpos;
  int shift = 0;
  int shift_start = 0;

#define ladjust(p) \
  if ((p > position && p != PASTENDPOS) ||	                            \
      (p == position && delta < 0)) {		                            \
    p += delta;					                            \
    if (p < tw->text.first_position) p = tw->text.first_position;   \
    if (p > tw->text.last_position) p = tw->text.last_position;     \
  }

#define radjust(p) \
  if ((p > position && p != PASTENDPOS) ||		                    \
      (p == position && delta > 0)) {			                    \
    p += delta;					       		            \
    if (p < tw->text.first_position) p = tw->text.first_position;   \
    if (p > tw->text.last_position) p = tw->text.last_position;     \
  }

  tw->text.first_position =
    (*tw->text.source->Scan)(tw->text.source, 0,
				 XmSELECT_ALL, XmsdLeft, 1, FALSE);
    tw->text.last_position =
      (*tw->text.source->Scan)(tw->text.source,  0,
				   XmSELECT_ALL, XmsdRight, 1, FALSE);
  if (delta == NODELTA) {
    if (tw->text.top_character == topos && position != topos) {
      tw->text.pending_scroll = -1;
      tw->text.forget_past = MIN(tw->text.forget_past, position);
    }
    if (tw->text.top_character > position &&
	tw->text.bottom_position < topos) {
      tw->text.new_top = position;
      tw->text.pending_scroll = -1;
      tw->text.forget_past = MIN(tw->text.forget_past, position);
    }

    if (tw->text.in_resize && tw->text.line_table != NULL) {
      unsigned int top_index, last_index, next_index;
      int index_offset, lines_used;

      top_index = tw->text.top_line;
      last_index = _XmTextGetTableIndex(tw, tw->text.last_position);

      lines_used = (last_index - top_index) + 1;

      if (top_index != 0 &&
	  tw->text.output->data->number_lines > lines_used) {
	index_offset = tw->text.output->data->number_lines-lines_used;
	if (index_offset < tw->text.total_lines - lines_used)
	  next_index = top_index - index_offset;
	else
	  next_index = 0;
	tw->text.new_top = tw->text.top_character =
	  tw->text.line_table[next_index].start_pos;
      }
    }

    tw->text.forget_past = MIN(tw->text.forget_past, position);
  } else {
    for (i=0; i<tw->text.repaint.number; i++) {
      radjust(tw->text.repaint.range[i].from);
      ladjust(tw->text.repaint.range[i].to);
    }
    for (i=1; i < tw->text.highlight.number; i++) {
      if (delta < 0 &&
	  tw->text.highlight.list[i].position >= position - delta)
	ladjust(tw->text.highlight.list[i].position);
      if (delta > 0 &&
	  ((tw->text.highlight.list[i].position > position) ||
	   ((tw->text.highlight.list[i].position == position) &&
	    (tw->text.highlight.list[i].mode != XmHIGHLIGHT_NORMAL))))
	radjust(tw->text.highlight.list[i].position);
    }
    for (i=1; i<tw->text.old_highlight.number; i++) {
      if (delta < 0 &&
	  tw->text.old_highlight.list[i].position >= position - delta)
	ladjust(tw->text.old_highlight.list[i].position);
      if (delta > 0 &&
	  ((tw->text.old_highlight.list[i].position > position) ||
	   ((tw->text.old_highlight.list[i].position == position) &&
	    (tw->text.old_highlight.list[i].mode != XmHIGHLIGHT_NORMAL))))
	radjust(tw->text.old_highlight.list[i].position);
    }
    for (i=0; i <= tw->text.number_lines &&
	 tw->text.line[i].start != PASTENDPOS; i++) {
      if (delta > 0) {
	radjust(tw->text.line[i].start);
      } else {
	if (tw->text.line[i].start > position &&
	    tw->text.line[i].start <= topos) {
	  if (i != 0 && shift_start == 0)
	    shift_start = i;
	  shift++;
	} else {
	  radjust(tw->text.line[i].start);
	}
      }
      if (tw->text.line[i].changed) {
	radjust(tw->text.line[i].changed_position);
      }
    }
    if (shift) {
      for (i=shift_start; i < tw->text.number_lines; i++) {
	if ((i < (shift_start + shift)) && tw->text.line[i].extra)
	  XtFree((char *)tw->text.line[i].extra);
	if (i + shift < tw->text.number_lines) {
	  tw->text.line[i].start = tw->text.line[i+shift].start;
	  tw->text.line[i].extra = tw->text.line[i+shift].extra;
	} else {
	  tw->text.line[i].start = PASTENDPOS;
	  tw->text.line[i].extra = NULL;
	}
	tw->text.line[i].changed = TRUE;
	if (tw->text.line[i].start != PASTENDPOS)
	  tw->text.line[i].changed_position =
	    tw->text.line[i + 1].start - 1;
	else
	  tw->text.line[i].changed_position = PASTENDPOS;
      }
    }
    ladjust(tw->text.bottom_position);
    tw->text.output->data->refresh_ibeam_off = True;
    endpos = topos;
    radjust(endpos);

    /* Force _XmTextPosToLine to not bother trying to recalculate. */
    tw->text.needs_refigure_lines = FALSE;
    for (l = _XmTextPosToLine(tw, position), p = position;
	 l < tw->text.number_lines &&
	 tw->text.line[l].start <= endpos;
	 l++, p = tw->text.line[l].start) {
      if (l != NOLINE) {
	if (tw->text.line[l].changed) {
	  tw->text.line[l].changed_position =
	    MIN(p, tw->text.line[l].changed_position);
	} else {
	  tw->text.line[l].changed_position = p;
	  tw->text.line[l].changed = TRUE;
	}
      }
    }
  }
  (*tw->text.output->Invalidate)(tw, position, topos, delta);
  (*tw->text.input->Invalidate)(tw, position, topos, delta);
  tw->text.needs_refigure_lines = tw->text.needs_redisplay = TRUE;
  if (tw->text.disable_depth == 0) Redisplay(tw);
}

static void
InsertHighlight(XmTextWidget tw,
		XmTextPosition position,
		XmHighlightMode mode)
{
  _XmHighlightRec *l1;
  _XmHighlightRec *l = tw->text.highlight.list;
  int i, j;

  l1 = FindHighlight(tw, position, XmsdLeft);
  if (l1->position == position && position != 0)
    l1->mode = mode;
  else {
    i = (l1 - l) + 1;
    tw->text.highlight.number++;
    if (tw->text.highlight.number > tw->text.highlight.maximum) {
      tw->text.highlight.maximum = tw->text.highlight.number;
      l = tw->text.highlight.list = (_XmHighlightRec *)
	XtRealloc((char *) l, tw->text.highlight.maximum *
		  sizeof(_XmHighlightRec));
    }
    for (j=tw->text.highlight.number-1; j>i; j--)
      l[j] = l[j-1];
    l[i].position = position;
    l[i].mode = mode;
  }
}

/****************************************************************
 *
 * Creation definitions.
 *
 ****************************************************************/
/*
 * Create the text widget.  To handle default condition of the core
 * height and width after primitive has already reset it's height and
 * width, use request values and reset height and width to original
 * height and width state.
 */
static void
Initialize(Widget rw,
	   Widget nw,
	   ArgList args,
	   Cardinal *num_args)
{
  XmTextWidget req = (XmTextWidget) rw;
  XmTextWidget newtw = (XmTextWidget) nw;

  if (MB_CUR_MAX > 0)
    newtw->text.char_size = (char)MB_CUR_MAX;
  else
    newtw->text.char_size = 1;

  if (req->core.width == 0) newtw->core.width = req->core.width;
  if (req->core.height == 0) newtw->core.height = req->core.height;

  /* Flag used in losing focus verification to indicate that a traversal
     key was pressed.  Must be initialized to False */
  newtw->text.traversed = False;

  newtw->text.total_lines = 1;
  newtw->text.top_line = 0;
  newtw->text.vsbar_scrolling = False;
  newtw->text.hsbar_scrolling = False;
  newtw->text.in_setvalues = False;

  if (newtw->text.output_create == NULL)
    newtw->text.output_create = _XmTextOutputCreate;
  if (newtw->text.input_create == NULL)
    newtw->text.input_create = _XmTextInputCreate;

  /*  The following resources are defaulted to invalid values to indicate    */
  /*  that it was not set by the application.  If it gets to this point      */
  /*  and they are still invalid then set them to their appropriate default. */

  if (!XmRepTypeValidValue(XmRID_EDIT_MODE,
			   newtw->text.edit_mode, nw)) {
    newtw->text.edit_mode = XmSINGLE_LINE_EDIT;
  }

   /* All 8 buffers must be created to be able to rotate the cut buffers */
   _XmCreateCutBuffers(nw);

   if (newtw->text.verify_bell == (Boolean) XmDYNAMIC_BOOL) {
     if (_XmGetAudibleWarning(nw) == XmBELL)
       newtw->text.verify_bell = True;
     else
       newtw->text.verify_bell = False;
   }

   /*
    * Initialize on-the-spot data
    */
   newtw->text.onthespot = (OnTheSpotDataTW) XtMalloc(sizeof(OnTheSpotDataRecTW)
);
   newtw->text.onthespot->start = newtw->text.onthespot->end =
   newtw->text.onthespot->cursor = newtw->text.onthespot->over_len =
   newtw->text.onthespot->over_maxlen = 0;
   newtw->text.onthespot->over_str = NULL;
   newtw->text.onthespot->under_preedit = False;
   newtw->text.onthespot->under_verify_preedit = False;
   newtw->text.onthespot->verify_commit = False;
   newtw->text.tm_table = (XtTranslations)NULL;
}

/*
 * Create a text widget.  Note that most of the standard stuff is actually
 * to be done by the output create routine called here, since output is in
 * charge of window handling.
 */
static void
InitializeHook(Widget wid,
	       ArgList args,
	       Cardinal *num_args_ptr)
{
  XmTextWidget tw;
  Cardinal num_args = *num_args_ptr;
  XmTextSource source;
  XmTextPosition top_character;
  XmTextBlockRec block;
  Position dummy;
  Boolean used_source = False;

  tw = (XmTextWidget) wid;

  /* If text.wc_value is set, it overrides. Call _Xm..Create with it. */
  if (tw->text.source == nullsourceptr) {
    if (tw->text.wc_value != NULL) {
      source = _XmStringSourceCreate((char*)tw->text.wc_value, True);
      tw->text.value = NULL;
      tw->text.wc_value = NULL;
    } else {
      source = _XmStringSourceCreate(tw->text.value, False);
      tw->text.value = NULL;
    }
  } else {
    source = tw->text.source;
    if (tw->text.wc_value != NULL) {
      char * tmp_value;
      int num_chars, n_bytes;

      for (num_chars=0; tw->text.wc_value[num_chars]!=0L; num_chars++)
	/*EMPTY*/;

      tmp_value = XtMalloc((unsigned)
			   (num_chars + 1) * (int)tw->text.char_size);
      n_bytes = wcstombs(tmp_value, tw->text.wc_value,
			 (num_chars + 1) * (int)tw->text.char_size);
      if (n_bytes == -1) n_bytes = 0;
      tmp_value[n_bytes] = 0;  /* NULL terminate the string */
      _XmStringSourceSetValue(tw, tmp_value);
      XtFree(tmp_value);
      tw->text.wc_value = NULL;
    } else if (tw->text.value != NULL) {
      /* Default value or argument ? */
      int i;
      for (i = 0; i < num_args; i++)
	if (tw->text.value == (char *)args[i].value &&
	    (args[i].name == XmNvalue ||
	     strcmp(args[i].name, XmNvalue) == 0)) {
	  _XmStringSourceSetValue(tw, tw->text.value);
	  break;
	}
    }
    tw->text.value = NULL;
    used_source = True;
  }

  tw->text.disable_depth = 1;
  tw->text.first_position = 0;
  tw->text.last_position = 0;
  tw->text.dest_position = 0;

  tw->text.needs_refigure_lines = tw->text.needs_redisplay = TRUE;
  tw->text.number_lines = 0;
  tw->text.maximum_lines = 1;
  tw->text.line = (Line) XtMalloc(sizeof(LineRec));
  tw->text.line->start = PASTENDPOS;
  tw->text.line->changed = False;
  tw->text.line->changed_position = PASTENDPOS;
  tw->text.line->past_end = False;
  tw->text.line->extra = NULL;
  tw->text.repaint.number = tw->text.repaint.maximum = 0;
  tw->text.repaint.range = (RangeRec *) XtMalloc(sizeof(RangeRec));
  tw->text.highlight.number = tw->text.highlight.maximum = 1;
  tw->text.highlight.list = (_XmHighlightRec *)
    XtMalloc(sizeof(_XmHighlightRec));
  tw->text.highlight.list[0].position = 0;
  tw->text.highlight.list[0].mode = XmHIGHLIGHT_NORMAL;
  tw->text.old_highlight.number = 0;
  tw->text.old_highlight.maximum = 1;
  tw->text.old_highlight.list = (_XmHighlightRec *)
    XtMalloc(sizeof(_XmHighlightRec));
  tw->text.highlight_changed = FALSE;
  tw->text.on_or_off = on;
  tw->text.force_display = -1;
  tw->text.in_redisplay = tw->text.in_refigure_lines = FALSE;
  tw->text.in_resize = FALSE;
  tw->text.in_expose = FALSE;
  tw->text.pending_scroll = 0;
  tw->text.new_top = tw->text.top_character;
  tw->text.bottom_position = 0;
  tw->text.add_mode = False;
  tw->text.pendingoff = True;
  tw->text.forget_past = 0;

  /* Translation table overwrite */
  if (XmDirectionMatch(XmPrim_layout_direction(tw),
		       XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
    char *vevent_bindings;

    vevent_bindings =
		(String)XtMalloc(strlen(_XmTextIn_XmTextVEventBindings) + 1);
    strcpy(vevent_bindings, _XmTextIn_XmTextVEventBindings);
    tw->text.tm_table = (XtTranslations)XtParseTranslationTable(vevent_bindings);
    XtFree(vevent_bindings);
    XtOverrideTranslations(wid, tw->text.tm_table);
  }

  /* Initialize table */
  if (tw->text.edit_mode == XmSINGLE_LINE_EDIT)
    InitializeLineTable(tw, 1);
  else
    InitializeLineTable(tw, INIT_TABLE_SIZE);

  (*tw->text.source->RemoveWidget)(tw->text.source, tw);
  tw->text.source = source;
  (*tw->text.source->AddWidget)(tw->text.source, tw);
  (*tw->text.output_create)(wid, args, num_args);

  _XmTextSetEditable(wid, tw->text.editable);
  _XmStringSourceSetMaxLength(GetSrc(tw), tw->text.max_length);

  (*tw->text.input_create)(wid, args, num_args);

  tw->text.first_position =
    (*tw->text.source->Scan)(tw->text.source, 0,
				 XmSELECT_ALL, XmsdLeft, 1, FALSE);
  tw->text.last_position =
    (*tw->text.source->Scan)(tw->text.source, 0,
				 XmSELECT_ALL, XmsdRight, 1, FALSE);

  if (tw->text.cursor_position < 0)
    tw->text.cursor_position = 0;

  if (tw->text.cursor_position > tw->text.last_position)
    tw->text.cursor_position = tw->text.last_position;

  tw->text.dest_position = tw->text.cursor_position;

  if (!tw->text.editable || !XtIsSensitive(wid))
    _XmTextSetDestinationSelection(wid, 0, False, (Time)NULL);

  if (tw->text.edit_mode == XmMULTI_LINE_EDIT)
    top_character = (*tw->text.source->Scan)(tw->text.source,
                                                 tw->text.top_character,
						 XmSELECT_LINE, XmsdLeft, 1,
						 FALSE);
  else
    top_character = tw->text.top_character;

  tw->text.new_top = top_character;
  tw->text.top_character = 0;
  _XmTextInvalidate(tw, top_character, top_character, NODELTA);
  if (tw->text.disable_depth == 0)
    Redisplay(tw);

  /*
   * Fix for CR 5704 - If the source has already been created, do not use
   *                   the original code - it has already been processed and
   *                   the gaps are not where they were the first time
   *                   through for this source.  Instead, use
   *                   code similar to that used in XmTextSetSource().
   */
  if (!used_source) {
    tw->text.source->data->gap_start[0] = '\0'; /*Hack to utilize initial
						  value when setting line
						  table - saves a malloc
						  and free. */
    if (tw->text.char_size == 1) {
      block.ptr = tw->text.source->data->ptr;
      if (block.ptr == NULL) block.length = 0;
      else block.length = strlen(block.ptr);
    } else
      (void)(*tw->text.source->ReadSource)(source, 0, source->data->length,
					       &block);
  } else
    (void)(*tw->text.source->ReadSource)(source, 0, source->data->length,
					     &block);

  _XmTextUpdateLineTable(wid, 0, 0, &block, False);

  _XmStringSourceSetGappedBuffer(source->data, tw->text.cursor_position);

  tw->text.forget_past = tw->text.first_position;

  tw->text.disable_depth = 0;
  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position,
				  &(tw->text.cursor_position_x), &dummy);
}

static void
Realize(Widget w,
        XtValueMask *valueMask,
        XSetWindowAttributes *attributes)
{
  XmTextWidget tw = (XmTextWidget) w;
  Position dummy;
  Arg im_args[20];
  XIMCallback xim_cb[4];
  Cardinal n = 0;

  (*tw->text.output->realize)(w, valueMask, attributes);
  (*tw->text.output->PosToXY)(tw, tw->text.cursor_position,
			      &(tw->text.cursor_position_x), &dummy);

  if (tw->text.editable) {
  /*
   * Register on the spot callbacks.
   */
    xim_cb[0].client_data = (XPointer)tw;
    xim_cb[0].callback = (XIMProc)PreeditStart;
    xim_cb[1].client_data = (XPointer)tw;
    xim_cb[1].callback = (XIMProc)PreeditDone;
    xim_cb[2].client_data = (XPointer)tw;
    xim_cb[2].callback = (XIMProc)PreeditDraw;
    xim_cb[3].client_data = (XPointer)tw;
    xim_cb[3].callback = (XIMProc)PreeditCaret;
    XtSetArg(im_args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
    XtSetArg(im_args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
    XtSetArg(im_args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
    XtSetArg(im_args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;
    XmImSetValues(w, im_args, n);
  }
}


/****************************************************************
 *
 * Semi-public definitions.
 *
 ****************************************************************/
static void
Destroy(Widget w)
{
  XmTextWidget tw = (XmTextWidget) w;
  int j;

  (*tw->text.source->RemoveWidget)(tw->text.source, tw);
  if (tw->text.input->destroy) (*tw->text.input->destroy)(w);
  if (tw->text.output->destroy) (*tw->text.output->destroy)(w);

  for (j = 0; j < tw->text.number_lines; j++) {
    if (tw->text.line[j].extra)
      XtFree((char *)tw->text.line[j].extra);
  }

  XtFree((char *)tw->text.line);

  XtFree((char *)tw->text.repaint.range);
  XtFree((char *)tw->text.highlight.list);
  XtFree((char *)tw->text.old_highlight.list);

  if (tw->text.line_table != NULL)
    XtFree((char *)tw->text.line_table);

  if (tw->text.onthespot != NULL)
    XtFree((char *)tw->text.onthespot);

  if (tw->text.tm_table != NULL)
    XtFree((char *)tw->text.tm_table);
}

static void
Resize(Widget w)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* this flag prevents resize requests */
  tw->text.in_resize = True;

  if (_XmTextShouldWordWrap(tw))
    _XmTextRealignLineTable(tw, NULL, 0, 0, 0, PASTENDPOS);

  (*(tw->text.output->resize))(w, FALSE);

  tw->text.in_resize = False;
}

static void
DoExpose(Widget w,
	 XEvent *event,
	 Region region)
{
  XmTextWidget tw = (XmTextWidget) w;

  /* this flag prevents resize requests */
  tw->text.in_expose = True;

  (*(tw->text.output->expose))(w, event, region);

  tw->text.in_expose = False;
}

static void
GetValuesHook(Widget w,
	      ArgList args,
	      Cardinal *num_args_ptr)
{
  XmTextWidget tw = (XmTextWidget) w;
  Cardinal num_args = *num_args_ptr;
  int i;

  XtGetSubvalues((XtPointer) tw,
		 resources, XtNumber(resources), args, num_args);

  for (i = 0; i < num_args; i++) {
    if (!strcmp(args[i].name, XmNvalue)) {
      *((XtPointer *)args[i].value) =
	(XtPointer)_XmStringSourceGetValue(GetSrc(tw), False);
    }
  }

  for (i = 0; i < num_args; i++) {
    if (!strcmp(args[i].name, XmNvalueWcs)) {
      *((XtPointer *)args[i].value) =
	(XtPointer)_XmStringSourceGetValue(GetSrc(tw), True);
    }
  }

  (*tw->text.output->GetValues)(w, args, num_args);
  (*tw->text.input->GetValues)(w, args, num_args);
}

void
_XmTextSetTopCharacter(Widget widget,
		       XmTextPosition top_character)
{
  XmTextWidget tw = (XmTextWidget) widget;
  LineNum line_num;

  if (tw->text.edit_mode != XmSINGLE_LINE_EDIT) {
    line_num = _XmTextGetTableIndex(tw, top_character);
    top_character = tw->text.line_table[line_num].start_pos;
  }

  if (top_character != tw->text.new_top) {
    EraseInsertionPoint(tw);
    tw->text.new_top = top_character;
    tw->text.pending_scroll = 0;
    tw->text.needs_refigure_lines = tw->text.needs_redisplay = TRUE;
    if (XmSINGLE_LINE_EDIT == tw->text.edit_mode)
	tw->text.output->data->hoffset = 0;
    if (tw->text.disable_depth == 0)
      Redisplay(tw);
    TextDrawInsertionPoint(tw);
  }
  /*
   * Fixes CDExc23645 and CDExc23898
   */
  else if (tw->text.auto_show_cursor_position &&
	   tw->text.edit_mode == XmSINGLE_LINE_EDIT)
    _XmTextShowPosition(widget, top_character);
}

static void
LosingFocus(XmTextWidget tw)
{
  XmTextVerifyCallbackStruct  cbdata;

  cbdata.reason = XmCR_LOSING_FOCUS;
  cbdata.event = NULL;
  cbdata.doit = True;
  cbdata.currInsert = tw->text.cursor_position;
  cbdata.newInsert = tw->text.cursor_position;
  cbdata.startPos = tw->text.cursor_position;
  cbdata.endPos = tw->text.cursor_position;
  cbdata.text = NULL;
  XtCallCallbackList((Widget)tw, tw->text.losing_focus_callback,
		     (XtPointer) &cbdata);
  tw->text.source->data->take_selection = True;
}

static Boolean
SetValues(Widget oldw,
	  Widget reqw,
	  Widget new_w,
	  ArgList args,
	  Cardinal *num_args)
{
  XmTextWidget old = (XmTextWidget) oldw;
  XmTextWidget newtw = (XmTextWidget) new_w;
  XmTextPosition new_cursor_pos;
  Boolean o_redisplay;
  Position dummy;
  Boolean need_new_cursorPos = False;
  Boolean need_text_redisplay = False;
  Boolean new_source = (newtw->text.source != old->text.source);
  XmTextSource cache_source = NULL;

  if (newtw->core.being_destroyed)
  {
      return False;
  }

  _XmTextResetIC(oldw);

  newtw->text.in_setvalues = True;

  if (newtw->text.cursor_position<0)
  {
        newtw->text.cursor_position=0;
    }

  /* It is unfortunate that the rest of the Text widget code, particularly the
  ** redisplay code, assumes that the current source is valid; in fact, it may
  ** have been changed by a set-values call. Ideally, we would be able to
  ** handle before anything else a change in the XmNsource resource of the
  ** widget; in practice, the changes would be extensive.
  ** Compromise by temporarily restoring the old value for those pieces of
  ** code which affect the display of the old source; then restore the new
  ** value for the display of the current source. That is, deal with the old
  ** value for just this one line of code.
  */
  if (new_source) {
      cache_source = newtw->text.source;
      newtw->text.source = old->text.source;
  }

  EraseInsertionPoint(newtw); /* assumes newtw->text.source matches values */

  if (new_source)
      newtw->text.source = cache_source;

  _XmTextDisableRedisplay(newtw, TRUE);

  /* set cursor_position to a known acceptable value (0 is always acceptable)
   */
  new_cursor_pos = newtw->text.cursor_position;
  newtw->text.cursor_position = 0;

  if (! XtIsSensitive(new_w) &&
      newtw->text.input->data->has_destination) {
    _XmTextSetDestinationSelection(new_w, 0, True,
				   XtLastTimestampProcessed(XtDisplay(new_w)));
  }

  if (!XmRepTypeValidValue(XmRID_EDIT_MODE,
			     newtw->text.edit_mode, new_w)) {
    newtw->text.edit_mode = old->text.edit_mode;
  }

  if ((old->text.top_character != newtw->text.top_character) &&
      (newtw->text.top_character != newtw->text.new_top)) {
    XmTextPosition new_top;
    if (newtw->text.output->data->resizeheight &&
     !(newtw->text.output->data->scrollvertical &&
     XmIsScrolledWindow(XtParent((Widget)newtw))) )
      new_top = 0;
    else
      new_top = newtw->text.top_character;

    newtw->text.top_character = old->text.top_character;
    _XmTextSetTopCharacter(new_w, new_top);
    if (newtw->text.needs_refigure_lines)
      newtw->text.top_character = new_top;
  }

  if (old->text.source != newtw->text.source) {
    XmTextSource source = newtw->text.source;
    newtw->text.source = old->text.source;
    o_redisplay = newtw->text.needs_redisplay;
    XmTextSetSource(new_w, source, old->text.top_character, 0);
    need_text_redisplay = newtw->text.needs_redisplay;
    newtw->text.needs_redisplay = o_redisplay;
  }

  if (old->text.editable != newtw->text.editable) {
    Boolean editable = newtw->text.editable;
    newtw->text.editable = old->text.editable;
    _XmTextSetEditable(new_w, editable);
  }

  _XmStringSourceSetMaxLength(GetSrc(newtw), newtw->text.max_length);

  /* Four cases to handle for value:
   *   1. user set both XmNvalue and XmNwcValue.
   *   2. user set the opposite resource (i.e. value is a char*
   *      and user set XmNwcValue, or vice versa).
   *   3. user set the corresponding resource (i.e. value is a char*
   *      and user set XmNValue, or vice versa).
   *   4. user set neither XmNValue nor XmNwcValue
   */

  /* OSF says:  if XmNvalueWcs set, it overrides all else */

  if (newtw->text.wc_value != NULL) {
    /* user set XmNvalueWcs resource - it rules ! */
    wchar_t * wc_value;
    char * tmp_value;
    int num_chars, n_bytes;

    num_chars = n_bytes = 0;

    for (num_chars = 0, wc_value = newtw->text.wc_value;
	 wc_value[num_chars] != 0L;) num_chars++;

    tmp_value = XtMalloc((unsigned)
			 (num_chars + 1) * (int)newtw->text.char_size);
    n_bytes = wcstombs(tmp_value, newtw->text.wc_value,
		       (num_chars + 1) * (int)newtw->text.char_size);
    if (n_bytes == -1) n_bytes = 0;
    tmp_value[n_bytes] = 0;  /* NULL terminate the string */
    o_redisplay = newtw->text.needs_redisplay;
    newtw->text.wc_value = NULL;
    newtw->text.value = NULL;
    _XmStringSourceSetValue(newtw, tmp_value);
    need_text_redisplay = newtw->text.needs_redisplay;
    newtw->text.needs_redisplay = o_redisplay;
    XtFree(tmp_value);
    need_new_cursorPos = True;
  } else if (newtw->text.value != NULL) {
    char * tmp_value;

    newtw->text.pendingoff = TRUE;
    o_redisplay = newtw->text.needs_redisplay;
    tmp_value = newtw->text.value;
    newtw->text.value = NULL;
    _XmStringSourceSetValue(newtw, tmp_value);
    need_text_redisplay = newtw->text.needs_redisplay;
    newtw->text.needs_redisplay = o_redisplay;
    need_new_cursorPos = True;
  }

  /* return cursor_position to it's original changed value */
  newtw->text.cursor_position = new_cursor_pos;

  if (old->text.cursor_position != newtw->text.cursor_position) {
    XmTextPosition new_position = newtw->text.cursor_position;
    newtw->text.cursor_position = old->text.cursor_position;

    if (new_position > newtw->text.source->data->length)
      _XmTextSetCursorPosition(new_w, newtw->text.source->data->length);
    else
      _XmTextSetCursorPosition(new_w, new_position);
  } else if (need_new_cursorPos) {
    XmTextPosition cursorPos = -1;
    int ix;

    for (ix = 0; ix < *num_args; ix++)
      if (strcmp(args[ix].name, XmNcursorPosition) == 0) {
	cursorPos = (XmTextPosition)args[ix].value;
	break;
      }
    if (cursorPos == -1)
      cursorPos = (*newtw->text.source->Scan)(newtw->text.source,
					      newtw->text.cursor_position,
					      XmSELECT_ALL, XmsdLeft, 1, TRUE);
    _XmTextSetCursorPosition(new_w, cursorPos);
  } else
    if (newtw->text.cursor_position > newtw->text.source->data->length) {
      _XmTextSetCursorPosition(new_w, newtw->text.source->data->length);
    }


  o_redisplay = (*newtw->text.output->SetValues)
    (oldw, reqw, new_w, args, num_args);
  (*newtw->text.input->SetValues)(oldw, reqw, new_w, args, num_args);
  newtw->text.forget_past = 0;
  newtw->text.disable_depth--;   /* _XmTextEnableRedisplay() is not called
				    because we don't want a repaint yet */
  TextDrawInsertionPoint(newtw); /* increment cursor_on stack in lieu of
				    _XmTextEnableRedisplay() call. */
  (*newtw->text.output->PosToXY)(newtw, newtw->text.cursor_position,
				 &(newtw->text.cursor_position_x), &dummy);

  if (o_redisplay) newtw->text.needs_redisplay = True;

  TextDrawInsertionPoint(newtw);

  if (XtIsSensitive(new_w) != XtIsSensitive(oldw)) {
    if (XtIsSensitive(new_w)) {
      EraseInsertionPoint(newtw);
      newtw->text.output->data->blinkstate = off;
      TextDrawInsertionPoint(newtw);
    } else {
      if (newtw->text.output->data->hasfocus) {
	newtw->text.output->data->hasfocus = False;
	_XmTextChangeBlinkBehavior(newtw, False);
	EraseInsertionPoint(newtw);
	newtw->text.output->data->blinkstate = on;
	TextDrawInsertionPoint(newtw);
        XmImUnsetFocus(new_w);
      }
    }
    if (newtw->text.source->data->length > 0)
      newtw->text.needs_redisplay = True;
  }

  if ((!newtw->text.editable || !XtIsSensitive(new_w)) &&
      _XmTextHasDestination(new_w))
    _XmTextSetDestinationSelection(new_w, 0, False, (Time)NULL);

  /* don't shrink to nothing */
  if (newtw->core.width == 0) newtw->core.width = old->core.width;
  if (newtw->core.height == 0) newtw->core.height = old->core.height;

  /* Optimization for the case when only XmNvalue changes.
     This considerably reduces flashing due to unneeded redraws */
  if (need_text_redisplay &&
      !newtw->text.needs_redisplay &&
      newtw->text.disable_depth == 0) {
    EraseInsertionPoint(newtw);
    newtw->text.disable_depth++;
    newtw->text.needs_redisplay = True;
    _XmTextEnableRedisplay(newtw);
    newtw->text.needs_redisplay = False;
  }

  newtw->text.in_setvalues = newtw->text.needs_redisplay;

  return newtw->text.needs_redisplay;
}

static XtGeometryResult
QueryGeometry(Widget w,
	      XtWidgetGeometry *intended,
	      XtWidgetGeometry *reply)
{
  XmTextWidget tw = (XmTextWidget) w;

  if (GMode (intended) & (~(CWWidth | CWHeight)))
    return(XtGeometryNo);

  reply->request_mode = (CWWidth | CWHeight);

  (*tw->text.output->GetPreferredSize)(w, &reply->width, &reply->height);
  if ((GMode(intended) != GMode(reply)) ||
      (reply->width != intended->width) ||
      (reply->height != intended->height))
    return (XtGeometryAlmost);
  else {
    reply->request_mode = 0;
    return (XtGeometryYes);
  }
}

static void
_XmTextSetString(Widget widget,
		 char *value)
{
  XmTextWidget tw = (XmTextWidget) widget;

  _XmTextResetIC(widget);

  tw->text.pendingoff = TRUE;
  if (value == NULL) value = "";
  _XmStringSourceSetValue(tw, value);

  /* after set, move insertion cursor to beginning of string. */
  _XmTextSetCursorPosition(widget, 0);
}

void
_XmTextSetCursorPosition(Widget widget,
			 XmTextPosition position)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextSource source;
  XmTextVerifyCallbackStruct cb;
  Position dummy;
  int n = 0;
  XPoint xmim_point;
  XRectangle xmim_area;
  Arg args[10];

  if (position < 0) {
    position = 0;
  }

  if (position > tw->text.last_position) {
    position = tw->text.last_position;
  }

  source = GetSrc(tw);

  /* if position hasn't changed, don't call the modify verify callback */
  if (position != tw->text.cursor_position) {
    /* Call Motion Verify Callback before Cursor Changes Positon */
    cb.reason = XmCR_MOVING_INSERT_CURSOR;
    cb.event  = NULL;
    cb.currInsert = tw->text.cursor_position;
    cb.newInsert = position;
    cb.doit = True;
    XtCallCallbackList (widget, tw->text.motion_verify_callback,
			(XtPointer) &cb);

    /* Cancel action upon application request */
    if (!cb.doit) {
      if (tw->text.verify_bell) XBell(XtDisplay(widget), 0);
      return;
    }
  }

  /* Erase insert cursor prior to move */
  EraseInsertionPoint(tw);
  tw->text.cursor_position = position;

  /*
   * If not in add_mode and pending delete state is on reset
   * the selection.
   */
  if (!tw->text.add_mode && tw->text.pendingoff &&
      _XmStringSourceHasSelection(source))
    (*source->SetSelection)(source, position, position,
			    XtLastTimestampProcessed(XtDisplay(widget)));

  /* ensure that IBeam at new location will be displayed correctly */
  _XmTextMovingCursorPosition(tw, position); /*correct GC for new location */

  if (tw->text.auto_show_cursor_position)
    _XmTextShowPosition(widget, position);
  if (tw->text.needs_redisplay && tw->text.disable_depth == 0)
    Redisplay(tw);

  (*tw->text.output->PosToXY) (tw, position, &(tw->text.cursor_position_x),
			       &dummy);

  tw->text.output->data->refresh_ibeam_off = True; /* update IBeam off area
						    * before drawing IBeam */

  (*tw->text.output->PosToXY)(tw, position, &xmim_point.x, &xmim_point.y);
  (void)_XmTextGetDisplayRect((Widget)tw, &xmim_area);
  n = 0;
  XtSetArg(args[n], XmNspotLocation, &xmim_point); n++;
  XtSetArg(args[n], XmNarea, &xmim_area); n++;
  XmImSetValues((Widget)tw, args, n);
  TextDrawInsertionPoint(tw);
}

void
_XmTextDisableRedisplay(XmTextWidget widget,
                        Boolean losesbackingstore)
{
  widget->text.disable_depth++;
  EraseInsertionPoint(widget);
}

void
_XmTextEnableRedisplay(XmTextWidget widget)
{
  if (widget->text.disable_depth) widget->text.disable_depth--;
  if (widget->text.disable_depth == 0 && widget->text.needs_redisplay)
    Redisplay(widget);

  /* If this is a scrolled widget, better update the scroll bars to reflect
   * any changes that have occured while redisplay has been disabled.  */

  if (widget->text.disable_depth == 0) {
    if (XmDirectionMatch(XmPrim_layout_direction(widget),
			 XmTOP_TO_BOTTOM_RIGHT_TO_LEFT)) {
      if (widget->text.output->data->scrollvertical &&
	  XmIsScrolledWindow(XtParent(widget)))
	_XmRedisplayVBar(widget);
      if (widget->text.output->data->scrollhorizontal &&
	  XmIsScrolledWindow(XtParent(widget)) &&
	  !widget->text.hsbar_scrolling)
	_XmChangeHSB(widget);
    } else {
      if (widget->text.output->data->scrollvertical &&
	  XmIsScrolledWindow(XtParent(widget)) &&
	  !widget->text.vsbar_scrolling)
	_XmChangeVSB(widget);
      if (widget->text.output->data->scrollhorizontal &&
	  XmIsScrolledWindow(XtParent(widget)))
	_XmRedisplayHBar(widget);
    }
  }

  TextDrawInsertionPoint(widget);
}

/* Count the number of characters represented in the char* str.  By
 * definition, if MB_CUR_MAX == 1 then num_count_bytes == number of characters.
 * Otherwise, use mblen to calculate. */

int
_XmTextCountCharacters(char *str,
		       int num_count_bytes)
{
  char * bptr;
  int count = 0;
  int char_size = 0;

  if (num_count_bytes <= 0)
    return 0;

  if (MB_CUR_MAX == 1 || MB_CUR_MAX == 0) /* Sun sets MB_CUR_MAX to 0, Argg!!*/
    return num_count_bytes;

  for (bptr = str; num_count_bytes > 0; count++, bptr+= char_size) {
    char_size = mblen(bptr, MB_CUR_MAX);
    if (char_size <= 0) break; /* error */
    num_count_bytes -= char_size;
  }
  return count;
}

void
_XmTextSetEditable(Widget widget,
                  Boolean editable)
{
  Arg args[20];
  XIMCallback xim_cb[4];
  Cardinal n = 0;
  XPoint xmim_point;
  XRectangle xmim_area;
  XmTextWidget tw = (XmTextWidget) widget;

  if (!tw->text.editable && editable) {
    OutputData o_data = tw->text.output->data;

    XmImRegister(widget, 0);

    (*tw->text.output->PosToXY)(tw, tw->text.cursor_position,
				&xmim_point.x, &xmim_point.y);
    (void)_XmTextGetDisplayRect((Widget)tw, &xmim_area);
    n = 0;
    XtSetArg(args[n], XmNfontList, o_data->fontlist); n++;
    XtSetArg(args[n], XmNbackground, widget->core.background_pixel); n++;
    XtSetArg(args[n], XmNforeground, tw->primitive.foreground); n++;
    XtSetArg(args[n], XmNbackgroundPixmap,
	     widget->core.background_pixmap); n++;
    XtSetArg(args[n], XmNspotLocation, &xmim_point); n++;
    XtSetArg(args[n], XmNarea, &xmim_area); n++;
    XtSetArg(args[n], XmNlineSpace, o_data->lineheight); n++;

   /*
    * 	Register on the spot callbacks.
    */

   xim_cb[0].client_data = (XPointer)tw;
   xim_cb[0].callback = (XIMProc)PreeditStart;
   xim_cb[1].client_data = (XPointer)tw;
   xim_cb[1].callback = (XIMProc)PreeditDone;
   xim_cb[2].client_data = (XPointer)tw;
   xim_cb[2].callback = (XIMProc)PreeditDraw;
   xim_cb[3].client_data = (XPointer)tw;
   xim_cb[3].callback = (XIMProc)PreeditCaret;
   XtSetArg(args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
   XtSetArg(args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
   XtSetArg(args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
   XtSetArg(args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;

   if (o_data->hasfocus)
      XmImSetFocusValues(widget, args, n);
   else
      XmImSetValues(widget, args, n);
  } else if (tw->text.editable && !editable) {
    XmImUnregister(widget);
  }

  tw->text.editable = editable;

  n = 0;

  if (editable) {
    XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_ACTIVE); n++;
  } else {
    XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_INACTIVE); n++;
  }

  XmDropSiteUpdate(widget, args, n);

  _XmStringSourceSetEditable(GetSrc(tw), editable);
}

void
_XmTextSetHighlight(Widget w,
		   XmTextPosition left,
		   XmTextPosition right,
		   XmHighlightMode mode)
{
  XmTextWidget tw = (XmTextWidget)w;
  _XmHighlightRec *l;
  XmHighlightMode endmode;
  int i, j;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  /* If right position is out-bound, change it to the last position. */
  if (right > tw->text.last_position)
    right = tw->text.last_position;

  /* If left is out-bound, don't do anything. */
  if (left >= right || right <= 0) {
    _XmAppUnlock(app);
    return;
  }

  if (left < 0)
    left = 0;

  EraseInsertionPoint(tw);
  if (!tw->text.highlight_changed) {
    tw->text.highlight_changed = TRUE;
    if (tw->text.old_highlight.maximum < tw->text.highlight.number) {
      tw->text.old_highlight.maximum = tw->text.highlight.number;
      tw->text.old_highlight.list = (_XmHighlightRec *)
	XtRealloc((char *)tw->text.old_highlight.list,
		  tw->text.old_highlight.maximum *
		  sizeof(_XmHighlightRec));
    }
    tw->text.old_highlight.number = tw->text.highlight.number;
    memcpy((void *) tw->text.old_highlight.list,
	   (void *) tw->text.highlight.list,
	   (size_t) tw->text.old_highlight.number *
	   sizeof(_XmHighlightRec));
  }
  endmode = FindHighlight(tw, right, XmsdLeft)->mode;
  InsertHighlight(tw, left, mode);
  InsertHighlight(tw, right, endmode);
  l = tw->text.highlight.list;
  i = 1;
  while (i < tw->text.highlight.number) {
    if (l[i].position >= left && l[i].position < right)
      l[i].mode = mode;
    if (l[i].mode == l[i-1].mode) {
      tw->text.highlight.number--;
      for (j=i; j<tw->text.highlight.number; j++)
	l[j] = l[j+1];
    } else i++;
  }
  /* Force the image GC to be updated based on the new highlight record */
  _XmTextMovingCursorPosition(tw, tw->text.cursor_position);
  tw->text.needs_redisplay = TRUE;
  if (tw->text.disable_depth == 0)
    Redisplay(tw);

  tw->text.output->data->refresh_ibeam_off = True;
  TextDrawInsertionPoint(tw);

  _XmAppUnlock(app);
}

void
_XmTextShowPosition(Widget widget,
		    XmTextPosition position)
{
  XmTextWidget tw = (XmTextWidget) widget;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);

  if (!tw->text.needs_refigure_lines &&
      (position < 0 || (position >= tw->text.top_character &&
			position < tw->text.bottom_position))) {
    (*tw->text.output->MakePositionVisible)(tw, position);
    _XmAppUnlock(app);
    return;
  }
  tw->text.force_display = position;
  tw->text.needs_refigure_lines = tw->text.needs_redisplay = TRUE;

  if (tw->text.disable_depth == 0) Redisplay(tw);

  _XmAppUnlock(app);
}

/* Used by CDE's lib/DtWidget/Editor.c  */
XmTextLineTable
_XmTextGetLineTable(Widget widget,
		    int *total_lines)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextLineTable line_table;

  *total_lines = tw->text.total_lines;
  line_table = (XmTextLineTable) XtMalloc((unsigned) *total_lines *
					  sizeof(XmTextLineTableRec));

  memcpy((void *) line_table, (void *) tw->text.line_table,
	 *total_lines * sizeof(XmTextLineTableRec));

  return line_table;
}

/********************************************
 * AccessTextual trait method implementation
 ********************************************/

static XtPointer
TextGetValue(Widget w,
	     int format)
{
  char *str;
  XmString tmp;

  switch(format) {
  case XmFORMAT_XmSTRING:
    str = XmTextGetString(w);
    tmp = XmStringCreateLocalized(str);
    if (str != NULL) XtFree(str);
    return((XtPointer) tmp);
  case XmFORMAT_MBYTE:
    return((XtPointer) XmTextGetString(w));
  case XmFORMAT_WCS:
    return((XtPointer) XmTextGetStringWcs(w));
  }

  return(NULL);
}

static void
TextSetValue(Widget w,
	     XtPointer s,
	     int format)
{
  char *str;

  switch(format) {
  case XmFORMAT_XmSTRING:
    str = (char *)XmStringUngenerate((XmString)s, NULL,
                                     XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    XmTextSetString(w, str);
    if (str != NULL) XtFree(str);
    break;
  case XmFORMAT_MBYTE:
    XmTextSetString(w, (char*) s);
    break;
  case XmFORMAT_WCS:
    XmTextSetStringWcs(w, (wchar_t *) s);
  }
}

static int
TextPreferredValue(Widget w)	/* unused */
{
  return(XmFORMAT_MBYTE);
}

/**
 ** New functions for the on the spot support.
 **/

/*
 * This function and _XmTextSetCursorPosition are almost the same.
 * The difference is that this function doesn't call any user's
 * callbacks like XmNmotionVerifyCallback
 */
static void
_XmTextPreeditSetCursorPosition(Widget widget,
                                XmTextPosition position)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextSource source;
  Position dummy;

  if (position < 0) {
    position = 0;
  }

  if (position > tw->text.last_position) {
    position = tw->text.last_position;
  }

  source = GetSrc(tw);

  /* Erase insert cursor prior to move */
  EraseInsertionPoint(tw);
  tw->text.cursor_position = position;

  /* ensure that IBeam at new location will be displayed correctly */
  _XmTextMovingCursorPosition(tw, position); /*correct GC for new location */
  (*tw->text.output->PosToXY) (tw, position, &(tw->text.cursor_position_x),
                               &dummy);
  if (tw->text.auto_show_cursor_position)
    XmTextShowPosition(widget, position);
  if (tw->text.needs_redisplay && tw->text.disable_depth == 0)
    Redisplay(tw);

  _XmTextResetClipOrigin(tw, position, False); /* move clip origin */
  tw->text.output->data->refresh_ibeam_off = True; /* update IBeam off area
                                                    * before drawing IBeam */

  TextDrawInsertionPoint(tw);
}

static void PreeditVerifyReplace(Widget w,
			 XmTextPosition frompos,
			 XmTextPosition topos,
			 char *mb,
			 XmTextPosition cursor,
			 Boolean *end_preedit)
{
  XmTextWidget tw = (XmTextWidget)w;

  UnderVerifyPreedit(tw) = True;
  _XmTextReplace(w, frompos, topos, mb, False);
  UnderVerifyPreedit(tw) = False;
  if (VerifyCommitNeeded(tw)) {
    _XmTextResetIC(w);
    *end_preedit = True;
  }
  _XmTextSetCursorPosition(w, cursor);
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
  XmTextPosition left, right, lastPos;
  Widget w = (Widget) client_data;
  XmTextWidget tw = (XmTextWidget) client_data;

  if (PreUnder(tw))
    return 0;

  /* check editable */
  if (!tw->text.source->data->editable){
    PreUnder(tw) = False;
    return 0;
  }

  PreOverLen(tw) = PreOverMaxLen(tw) = 0L;
  PreOverStr(tw) = NULL;

  /* Treat Pending delete */
  if (_XmTextNeedsPendingDeleteDis(tw, &left, &right, False))
    _XmTextReplace(w, left, right, NULL, False);

  PreStartTW(tw) = PreEndTW(tw) = PreCursorTW(tw) = XmTextGetCursorPosition(w);
  PreUnder(tw) = True;

  /* when overstrike mode, stock text buffer */
  if (tw->text.input->data->overstrike){
    lastPos = (*(tw->text.source->Scan))(tw->text.source,
                                         PreCursorTW(tw),
                                         XmSELECT_LINE, XmsdRight, 1, TRUE);
    PreOverLen(tw) = lastPos - PreCursorTW(tw);
    PreOverStr(tw) = _XmStringSourceGetString(tw, PreCursorTW(tw),
                                              lastPos, False);
  }

  return (-1);
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
  char *mb;
  XmTextBlockRec block;
  XmTextWidget tw = (XmTextWidget)client_data;
  Widget w = (Widget)client_data;
  int size, num_bytes = 0;
  Widget p = w;
  Boolean need_verify, end_preedit = False;

  if (!PreUnder(tw))
    return;

  while (!XtIsShell(p))
    p = XtParent(p);
  XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

  /*
   * Delete preedit string
   */
  if (PreEndTW(tw) > PreStartTW(tw)) {
    if (need_verify) {
	PreeditVerifyReplace(w, PreStartTW(tw), PreEndTW(tw), NULL,
				PreStartTW(tw), &end_preedit);
	if (end_preedit) return;
    }
    else {
    	block.ptr = NULL;
    	block.length = 0;
    	block.format = XmFMT_8_BIT;
    	(*tw->text.source->Replace)(tw, NULL, &PreStartTW(tw), &PreEndTW(tw),
				&block, False);
    }
  }

  if (tw->text.input->data->overstrike && PreOverMaxLen(tw) > 0){
    if (PreOverMaxLen(tw) == PreOverLen(tw))
      mb = PreOverStr(tw);
    else {
        mb = XtMalloc((PreOverMaxLen(tw)+1)*tw->text.char_size);
        for (size = PreOverMaxLen(tw); size >0; size--)
           num_bytes +=mblen(PreOverStr(tw)+num_bytes, tw->text.char_size);
        memmove (mb, PreOverStr(tw), num_bytes);
        mb[num_bytes] = 0;
    }

    if (need_verify) {
	PreeditVerifyReplace(w, PreStartTW(tw), PreStartTW(tw), mb,
                                PreStartTW(tw), &end_preedit);
        if (end_preedit) return;
    }
    else {
	(*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
                                                                off);
    	block.ptr = mb;
    	block.length = strlen(block.ptr);
    	block.format = XmFMT_8_BIT;

    	(*tw->text.source->Replace)(tw, NULL, &PreStartTW(tw), &PreStartTW(tw),
				&block, False);
    	_XmTextPreeditSetCursorPosition(w, PreStartTW(tw));
	(*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
                                                                on);
    }

    if (PreOverMaxLen(tw) != PreOverLen(tw))
      XtFree(mb);
    PreOverMaxLen(tw) = PreOverLen(tw) = 0;
    XtFree((char *)PreOverStr(tw));
  }

  PreStartTW(tw) = PreEndTW(tw) = PreCursorTW(tw) = 0;
  PreUnder(tw) = False;
}

/*
 * This function shows the correspondence of rendition data between
 * the input server and XmTextField.
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
  unsigned short cnt;
  XIMFeedback fb;
  XmTextPosition prestart = PreStartTW((XmTextWidget)w)+data->chg_first, left, right;
  XmHighlightMode mode;

  if (!text->length)
    return;

  if (!text->feedback)
    return;

  fb = text->feedback[0];                       /* initial feedback */
  left = right = prestart;                      /* mode start/end position */
  mode = _XimFeedbackToXmHighlightMode(fb);     /* mode */
  cnt = 1;                                      /* counter initialize */

  while (cnt < text->length) {
    if (fb != text->feedback[cnt]) {
      right = prestart + cnt;
      XmTextSetHighlight(w, left, right, mode);

      left = right;                             /* start position update */
      fb = text->feedback[cnt];                 /* feedback update */
      mode = _XimFeedbackToXmHighlightMode(fb);
    }
    cnt++;                                      /* counter increment */
  }
  XmTextSetHighlight(w, left, (prestart + cnt), mode);
                                                /* for the last segment */
}

#define TEXT_MAX_INSERT_SIZE 512

/*
 * This is the function set to XNPreeditDrawCallback resource.
 * This function is called when the input server requests XmText
 * to draw a preedit string.
 */
static void
PreeditDraw(XIC xic,
            XPointer client_data,
            XIMPreeditDrawCallbackStruct *call_data)
{
  Widget w = (Widget) client_data;
  XmTextWidget tw = (XmTextWidget) client_data;
  InputData data = tw->text.input->data;
  XmTextPosition startPos, endPos, rest_len, tmp_end;
  char *mb, *over_mb;
  XmTextBlockRec block;
  unsigned short insert_length=0;
  int i;
  int total_mb_len;
  int recover_len = 0;
  char *ptr;
  OutputData o_data = tw->text.output->data;
  XFontStruct *font = o_data->font;
  XRectangle overall_ink;
  int escapement;
  size_t mb_siz;
  Widget p =w;
  Boolean need_verify, end_preedit = False;

  if (!PreUnder(tw))
    return;

  /* if no data in callback structs simply return - nothing to do */
  if (!call_data->caret && !call_data->chg_first && !call_data->chg_length
      && !call_data->text)
    return;

  /* have we exceeded max size of preedit buffer? - then punt */
  if (call_data->text &&
      ((insert_length = call_data->text->length) > TEXT_MAX_INSERT_SIZE))
    return;

  if (call_data->chg_length>PreEndTW(tw)-PreStartTW(tw))
    call_data->chg_length = PreEndTW(tw)-PreStartTW(tw);

  /* loop to determine parent shell widget id */
  while (!XtIsShell(p))
    p = XtParent(p);

  /* determine whether verify preedit is set in shell widget */
  XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

  /* turn cursor off */
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);
  XmTextSetHighlight(w, PreStartTW(tw)+call_data->chg_first,
                PreStartTW(tw) + call_data->chg_first + call_data->chg_length,
                XmHIGHLIGHT_NORMAL);

  /* preedit deletion */
  if (!data->overstrike && (!call_data->text || !insert_length)){
    startPos = PreStartTW(tw) + call_data->chg_first;
    endPos = startPos + call_data->chg_length;
    PreCursorTW(tw) = startPos;
    PreEndTW(tw) -= endPos - startPos;

    if (need_verify) {
      PreeditVerifyReplace(w, startPos, endPos, NULL, startPos, &end_preedit);
      if (end_preedit) {
	(*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
					       on);
	return;
      }
    }
    else {
      block.ptr = NULL;
      block.length = 0;
      block.format = XmFMT_8_BIT;

      if ((*tw->text.source->Replace)(tw, NULL, &startPos, &endPos,
				      &block, False) != EditDone) {
      	  XBell(XtDisplay(tw), 0);
	  return;
      }
      else
      	_XmTextPreeditSetCursorPosition(w, PreCursorTW(tw));
    }
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
    return;
  }

  /* sanity check data to make sure its *really* there */
  if (call_data->text)
    if ((call_data->text->encoding_is_wchar &&
	 !call_data->text->string.wide_char) ||
	(!call_data->text->encoding_is_wchar &&
	 !call_data->text->string.multi_byte)){

      PreeditSetRendition(w, call_data);
      (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
      return;
    }

  /* convert text data to char - it may be wchar or char */
  if (insert_length > 0){
    if (o_data->use_fontset) {
      if (call_data->text->encoding_is_wchar){
        mb = XtMalloc((insert_length+1)*tw->text.char_size);
        mb_siz = wcstombs(mb, call_data->text->string.wide_char,
                  insert_length);
      }
      else {
	mb = XtMalloc((insert_length+1)*tw->text.char_size);
        strcpy(mb,call_data->text->string.multi_byte);
      }

      /* set TextExtents for preedit data, if unable, punt */
      escapement = XmbTextExtents((XFontSet)font, mb, strlen(mb),
                                  &overall_ink, NULL );
      if (escapement == 0 && overall_ink.width == 0 &&
          strchr(mb, '\t') == 0 ) {

        XtFree(mb);
	(*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
					       on);
        return;
      }
    }
    else {
      (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
      return;
    }
  }
  else {
    mb = XtMalloc(4);
    mb[0] = '\0';
  }

  /* setup overstrike buffers and data */
  if (data->overstrike) {
    startPos = PreStartTW(tw) + call_data->chg_first;
    tmp_end = (XmTextPosition)(PreEndTW(tw) + insert_length -
                               call_data->chg_length);
    if (PreOverMaxLen(tw) < tmp_end - PreStartTW(tw)){
      if (tmp_end - PreStartTW(tw) > PreOverLen(tw)){
        endPos = startPos + call_data->chg_length;
        PreOverMaxLen(tw) = PreOverLen(tw);
      }
      else {
        endPos = PreEndTW(tw) + tmp_end - PreStartTW(tw) - PreOverMaxLen(tw);
        PreOverMaxLen(tw) = tmp_end - PreStartTW(tw);
      }
    }
    else {
      if (PreOverMaxLen(tw) > tmp_end - PreStartTW(tw)){
	endPos = PreEndTW(tw);
	recover_len = PreOverMaxLen(tw) - tmp_end + PreStartTW(tw);
	PreOverMaxLen(tw) = tmp_end - PreStartTW(tw);
      }
      else
	endPos = startPos + call_data->chg_length;
    }

    rest_len = PreEndTW(tw) - PreStartTW(tw) - call_data->chg_first -
      call_data->chg_length;
    if (rest_len) {
      over_mb = _XmStringSourceGetString(tw, (XmTextPosition)
		  (PreStartTW(tw)+call_data->chg_first+call_data->chg_length),
		  PreEndTW(tw), False);
      mb = XtRealloc(mb, strlen(mb)+strlen(over_mb)+1);
      strcat(mb, over_mb);
      XtFree(over_mb);
    }

    if (recover_len > 0) {
      mb = XtRealloc(mb, strlen(mb) + (recover_len + 1 ) * tw->text.char_size);
      ptr = PreOverStr(tw);
      for (i=0; i<PreOverMaxLen(tw); i++)
	ptr += mblen(ptr, 4);
      total_mb_len = 0;
      for (i=0; i<recover_len; i++)
	total_mb_len += mblen(ptr + total_mb_len, 4);
      i = strlen(mb);
      strncat(mb, ptr, total_mb_len);
      mb[i+total_mb_len] = '\0';
    }
  }
  else {
    startPos = PreStartTW(tw) + call_data->chg_first;
    endPos = startPos + call_data->chg_length;
  }

  if (data->overstrike)
      PreEndTW(tw) = startPos + insert_length;
  else
      PreEndTW(tw) += insert_length - endPos + startPos;
  PreCursorTW(tw) = PreStartTW(tw) + call_data->caret;

  /* verify preedit set, so call PreeditVerifyReplace */
  if (need_verify) {
    PreeditVerifyReplace(w, startPos, endPos, mb,
			 PreCursorTW(tw), &end_preedit);
    if (end_preedit) {
      (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
      return;
    }
  }
  /* no need to verify, just insert text into buffer */
  else {
    block.ptr = mb;
    block.length = strlen(mb);
    block.format = XmFMT_8_BIT;
    if ((*tw->text.source->Replace)(tw, NULL, &startPos, &endPos,
				    &block, False) != EditDone) {
      XBell(XtDisplay(tw), 0);
      return;
    }
    else
      _XmTextPreeditSetCursorPosition(w, PreCursorTW(tw));
  }

  /* set feedback */
  if (insert_length>0)
    PreeditSetRendition(w, call_data);

  /* turn cursor back on */
  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
  if (mb)
    XtFree(mb);
}

/*
 * This is the function set to XNPreeditCaretCallback resource.
 * This function is called when the input server requests XmText to move
 * the caret.
 */
static void
PreeditCaret(XIC xic,
             XPointer client_data,
             XIMPreeditCaretCallbackStruct *call_data)
{
  XmTextWidget tw = (XmTextWidget) client_data;
  XmSourceData data = tw->text.source->data;
  Widget w = (Widget) client_data;
  XmTextPosition new_position, start = 0;
  Widget p = (Widget) tw;
  Boolean need_verify;

  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);
  while (!XtIsShell(p))
    p = XtParent(p);
  XtVaGetValues(p, XmNverifyPreedit, &need_verify, NULL);

  switch (call_data->direction) {
  case XIMForwardChar:
    new_position = PreCursorTW(tw) + 1 - PreStartTW(tw);
    break;
  case XIMBackwardChar:
    new_position = PreCursorTW(tw) - 1 - PreStartTW(tw);
    break;
  case XIMAbsolutePosition:
    new_position = (XmTextPosition) call_data->position;
    break;
  default:
    new_position = PreCursorTW(tw) - PreStartTW(tw);
  }

  _XmTextValidate(&start, &new_position, data->length);
  PreCursorTW(tw) = PreStartTW(tw) + new_position;

  if (need_verify) {
    UnderVerifyPreedit(tw) = True;
    _XmTextSetCursorPosition (w, PreCursorTW(tw));
    UnderVerifyPreedit(tw) = False;
  }
  else
    _XmTextPreeditSetCursorPosition(w, PreCursorTW(tw));

  (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, on);
}

static void
ResetUnder(XmTextWidget tw)
{
    if (XmImGetXICResetState((Widget)tw) != XIMPreserveState)
        PreUnder(tw) = False;
}

/*
 * Resets input method context.
 *
 * 1. Call XmImMbResetIC to reset the input method and get the current
 *    preedit string.
 * 2. Set the string to XmText
 */
void
_XmTextResetIC(Widget widget)
{
  int escapement, n, size;
  char *mb, *tmp_mb;
  XRectangle overall_ink;
  XmTextPosition beginPos, nextPos, lastPos;
  XmTextWidget tw = (XmTextWidget) widget;
  InputData data = tw->text.input->data;
  OutputData o_data = tw->text.output->data;
  XFontStruct *font = o_data->font;

  if (!PreUnder((XmTextWidget) widget))
    return;

  if (VerifyCommitNeeded(tw)) {
    VerifyCommitNeeded(tw) = False;
    mb = _XmStringSourceGetString(tw, PreStartTW(tw), PreEndTW(tw), False);
    XmImMbResetIC(widget, &tmp_mb);
    if (tmp_mb) XtFree(tmp_mb);
  }
  else
    XmImMbResetIC(widget, &mb);

  if (!mb) {
    ResetUnder(tw);
    return;
  }
  n = strlen(mb);

  if (n > TEXT_MAX_INSERT_SIZE) {
    ResetUnder(tw);
    return;
  }

  if (n > 0) {
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position, off);
    mb[n]='\0';
    if (o_data->use_fontset) {
      escapement = XmbTextExtents((XFontSet)font, mb, n, &overall_ink, NULL );
      if (escapement == 0 && overall_ink.width == 0 &&
          strchr(mb, '\t') == 0 ) {
        (*tw->text.output->DrawInsertionPoint)(tw,
                                               tw->text.cursor_position, on);
        ResetUnder(tw);
        return;
      }
    } else {
      (*tw->text.output->DrawInsertionPoint)(tw,
                                             tw->text.cursor_position, on);
      ResetUnder(tw);
      return;
    }
    beginPos = nextPos = XmTextGetCursorPosition(widget);

    if (data->overstrike) {
      tmp_mb = XtMalloc((n+1)*tw->text.char_size);
      size = _XmTextBytesToCharacters(tmp_mb, mb, n, False,
                                      tw->text.char_size);
      nextPos += size;
      XtFree(tmp_mb);
      lastPos = (*(tw->text.source->Scan))(tw->text.source,
                                           beginPos, XmSELECT_LINE,
                                           XmsdRight, 1, TRUE);
      if (nextPos > lastPos) nextPos = lastPos;
    }
    else if (PreUnder(tw) && PreStartTW(tw) < PreEndTW(tw)) {
      beginPos = PreStartTW(tw);
      nextPos = PreEndTW(tw);
      XmTextSetHighlight((Widget)tw, beginPos, nextPos, XmHIGHLIGHT_NORMAL);
    }

    _XmTextReplace(widget, beginPos, nextPos, mb, False);
    (*tw->text.output->DrawInsertionPoint)(tw,
                                             tw->text.cursor_position, on);

    XtFree(mb);
  }
  ResetUnder(tw);
}

XmTextPosition
_XmTextSetPreeditPosition(Widget w,
                          XmTextPosition position)
{
  XmTextWidget tw = (XmTextWidget)w;
  XmTextPosition cursorPos = position;
  if (tw != NULL && tw->text.onthespot) {
    if (PreUnder(tw) && PreStartTW(tw) < PreEndTW(tw)) {
      int diff = PreEndTW(tw) - PreStartTW(tw);
      PreStartTW(tw) = cursorPos;
      cursorPos += diff;
      PreEndTW(tw) = PreCursorTW(tw) = cursorPos;
    }
    else {
      PreStartTW(tw) = PreEndTW(tw) = PreCursorTW(tw) =
        cursorPos;
    }
  }
  return cursorPos;
}

/****************************************************************
 *
 * Public definitions.
 *
 ****************************************************************/

char *
XmTextGetString(Widget widget)
{
  char *text_copy = NULL;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)) {
    XmAccessTextualTrait textT;
    textT = (XmAccessTextualTrait)
      XmeTraitGet((XtPointer) XtClass(widget), XmQTaccessTextual);
    if (textT)
      text_copy = (char*) textT->getValue(widget, XmFORMAT_MBYTE);
  } else
    text_copy = _XmStringSourceGetValue(GetSrc(widget), False);

  _XmAppUnlock(app);
  return (text_copy);
}


wchar_t *
XmTextGetStringWcs(Widget widget)
{
  wchar_t *text_copy = NULL;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)){
    XmAccessTextualTrait textT;
    textT = (XmAccessTextualTrait)
      XmeTraitGet((XtPointer) XtClass(widget), XmQTaccessTextual);
    if (textT)
      text_copy = (wchar_t *) textT->getValue(widget, XmFORMAT_WCS);
  }
  else
    text_copy = (wchar_t *) _XmStringSourceGetValue(GetSrc(widget), True);
  _XmAppUnlock(app);
  return (text_copy);
}

void
XmTextSetString(Widget widget,
		char *value)
{
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (XmIsTextField(widget)) {
    XmAccessTextualTrait textT;
    textT = (XmAccessTextualTrait)
      XmeTraitGet((XtPointer) XtClass(widget), XmQTaccessTextual);
    if (textT)
      textT->setValue(widget, (XtPointer)value, XmFORMAT_MBYTE);
  } else
    _XmTextSetString(widget, value);

  _XmAppUnlock(app);
}

void
XmTextSetStringWcs(Widget widget,
		   wchar_t *wc_value)
{
  char * tmp;
  int num_chars = 0;
  int result;
  XmTextWidget tw = (XmTextWidget) widget;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if(XmIsTextField(widget)) {
    XmAccessTextualTrait textT;
    textT = (XmAccessTextualTrait)
      XmeTraitGet((XtPointer) XtClass(widget), XmQTaccessTextual);
    if (textT)
      textT->setValue(widget, (XtPointer)wc_value, XmFORMAT_WCS);
  } else {

    for (num_chars = 0; wc_value[num_chars] != (wchar_t)0L; num_chars++)
      /*EMPTY*/;

    tmp = XtMalloc((unsigned) (num_chars + 1) * (int)tw->text.char_size);
    result = wcstombs(tmp, wc_value,
		      (num_chars + 1) * (int)tw->text.char_size);

    if (result == (size_t) -1) {/* if wcstombs fails, it returns (size_t) -1 */
      XtFree(tmp);              /* if invalid data, pass in the empty string */
      _XmTextSetString(widget, "");
    } else {
      _XmTextSetString(widget, tmp);
      XtFree(tmp);
    }
  }
  _XmAppUnlock(app);
}

XmTextPosition
XmTextGetTopCharacter(Widget widget)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextPosition ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  if (tw->text.needs_refigure_lines)
    RefigureLines(tw);
  ret_val = tw->text.top_character;
  _XmAppUnlock(app);
  return ret_val;
}


void
XmTextSetTopCharacter(Widget widget,
		      XmTextPosition top_character)
{
  XmTextWidget tw = (XmTextWidget) widget;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
   if (tw->text.output->data->resizeheight &&
       !(tw->text.output->data->scrollvertical &&
       XmIsScrolledWindow(XtParent(widget))) ) {
    if (tw->text.top_character == 0) {
      _XmAppUnlock(app);
      return;
    }
    else
      top_character = 0;
  }

  _XmTextSetTopCharacter(widget, top_character);
  _XmAppUnlock(app);
}


XmTextSource
XmTextGetSource(Widget widget)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextSource ret_val;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  ret_val = tw->text.source;
  _XmAppUnlock(app);
  return ret_val;
}

void
XmTextSetSource(Widget widget,
		XmTextSource source,
		XmTextPosition top_character,
		XmTextPosition cursor_position)
{
  XmTextWidget tw = (XmTextWidget) widget;
  XmTextPosition pos = 0;
  XmTextPosition last_pos = 0;
  XmTextPosition old_pos = 0;
  XmTextBlockRec block;
  int n = 0;
  XPoint xmim_point;
  XRectangle xmim_area;
  Arg args[10];
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);

  _XmTextResetIC(widget);
  EraseInsertionPoint(tw);
  if (source == NULL) {
    XmeWarning(widget, MESSAGE2);
    _XmAppUnlock(app);
    return;
  }

  /* zero out old line table */
  block.ptr = NULL;
  block.length = 0;
  _XmTextUpdateLineTable(widget, 0, 0, &block, False);
  tw->text.total_lines = 1;

  (*tw->text.source->RemoveWidget)(tw->text.source, tw);
  tw->text.source = source;

  if (cursor_position > source->data->length)
    cursor_position = source->data->length;
  else if (cursor_position < 0)
    cursor_position = 0;

  tw->text.cursor_position = cursor_position;
  _XmTextMovingCursorPosition(tw, cursor_position); /*correct GC for
						     * new location */
  tw->text.output->data->refresh_ibeam_off = True;
  (*tw->text.source->AddWidget)(tw->text.source, tw);
  _XmStringSourceSetGappedBuffer(source->data, cursor_position);
  if (tw->text.edit_mode == XmMULTI_LINE_EDIT)
    top_character = (*tw->text.source->Scan)(tw->text.source, top_character,
					     XmSELECT_LINE, XmsdLeft, 1,
					     FALSE);

  tw->text.new_top = top_character;
  tw->text.top_character = 0;

  /* reset line table with new source */
  last_pos = (XmTextPosition) source->data->length;
  while (pos < last_pos) {
    pos = (*tw->text.source->ReadSource)(source, pos, last_pos, &block);
    if (block.length == 0)
      break;
    _XmTextUpdateLineTable(widget, old_pos, old_pos, &block, False);
    old_pos = pos;
  }

  _XmTextInvalidate(tw, top_character, top_character, NODELTA);
  if (tw->text.disable_depth == 0)
    Redisplay(tw);

  /* Tell the input method the new x,y location of the cursor */
  (*tw->text.output->PosToXY)(tw, cursor_position, &xmim_point.x,
			      &xmim_point.y);
  (void)_XmTextGetDisplayRect((Widget)tw, &xmim_area);
  n = 0;
  XtSetArg(args[n], XmNspotLocation, &xmim_point); n++;
  XtSetArg(args[n], XmNarea, &xmim_area); n++;
  XmImSetValues((Widget)tw, args, n);

  TextDrawInsertionPoint(tw);

  _XmAppUnlock(app);
}


void
XmTextScroll(Widget widget,
	     int n)
{
  XmTextWidget tw = (XmTextWidget) widget;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);

  tw->text.pending_scroll += n;
  tw->text.needs_refigure_lines = tw->text.needs_redisplay = TRUE;

  if (tw->text.disable_depth == 0) Redisplay(tw);

  _XmAppUnlock(app);
}

void
XmTextDisableRedisplay(Widget widget)
{
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  _XmTextDisableRedisplay((XmTextWidget)widget, False);
  _XmAppUnlock(app);
}

void
XmTextEnableRedisplay(Widget widget)
{
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  _XmTextEnableRedisplay((XmTextWidget)widget);
  _XmAppUnlock(app);
}

Widget
XmCreateScrolledText(Widget parent,
		     char *name,
		     ArgList arglist,
		     Cardinal argcount)
{
  Widget swindow;
  Widget stext;
  Arg args_cache[30];
  ArgList merged_args;
  int n;
  char s_cache[30];
  char *s;
  Cardinal s_size;
  Cardinal arg_size = argcount + 5;
  _XmWidgetToAppContext(parent);

  _XmAppLock(app);
  s_size = ((name) ? strlen(name) : 0) + 3;

  s = (char *) XmStackAlloc(s_size, s_cache);  /* Name + NULL + "SW" */
  if (name) {
    strcpy(s, name);
    strcat(s, "SW");
  } else {
    strcpy(s, "SW");
  }

  /*
   * merge the application arglist with the required preset arglist, for
   * creating the scrolled window portion of the scroll text.
   */
  merged_args = (ArgList)XmStackAlloc(arg_size*sizeof(Arg), args_cache);
  for (n=0; n < argcount; n++) {
    merged_args[n].name = arglist[n].name;
    merged_args[n].value = arglist[n].value;
  }
  XtSetArg(merged_args[n], XmNscrollingPolicy,
	   (XtArgVal) XmAPPLICATION_DEFINED); n++;
  XtSetArg(merged_args[n], XmNvisualPolicy, (XtArgVal)XmVARIABLE); n++;
  XtSetArg(merged_args[n], XmNscrollBarDisplayPolicy, (XtArgVal)XmSTATIC); n++;
  XtSetArg(merged_args[n], XmNshadowThickness, (XtArgVal) 0); n++;

  swindow = XtCreateManagedWidget(s, xmScrolledWindowWidgetClass, parent,
				  merged_args, n);
  XmStackFree(s, s_cache);
  XmStackFree((char *)merged_args, args_cache);

  /* Create Text widget.  */
  stext = XtCreateWidget(name, xmTextWidgetClass, swindow, arglist, argcount);

  /* Add callback to destroy ScrolledWindow parent. */
  XtAddCallback (stext, XmNdestroyCallback, _XmDestroyParentCallback, NULL);

  _XmAppUnlock(app);
  /* Return Text.*/
  return (stext);
}

Widget
XmCreateText(Widget parent,
	     char *name,
	     ArgList arglist,
	     Cardinal argcount)
{
  return XtCreateWidget(name, xmTextWidgetClass, parent, arglist, argcount);
}

Widget
XmVaCreateText(
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
                         xmTextWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget
XmVaCreateManagedText(
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
                         xmTextWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

