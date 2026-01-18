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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: List.c /main/47 1999/10/12 16:58:17 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <X11/Xatom.h>
#include <Xm/XmosP.h>
#include "XmI.h"
#include <Xm/CutPaste.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/DragIconP.h>
#include <Xm/DrawP.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/ListP.h>
#include <Xm/ManagerP.h>
#include <Xm/NavigatorT.h>
#include <Xm/ScreenP.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrollBarP.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/ScrolledWP.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "DisplayI.h"
#include "DrawI.h"
#include "RepTypeI.h"
#include "MessagesI.h"
#include "ScreenI.h"
#include "ScrollFramTI.h"
#include "TransferI.h"		/* for _XmConvertComplete() */
#include "TravActI.h"
#include "TraversalI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"
#include "ToolTipI.h"
#include <Xm/XmP.h>

#define	BUTTONDOWN	1
#define	SHIFTDOWN	2
#define	CTRLDOWN	4
#define	ALTDOWN		8
#define	TOPLEAVE	1
#define	BOTTOMLEAVE	2
#define	LEFTLEAVE	4
#define	RIGHTLEAVE	8

#define MOTION_THRESHOLD	3

#define LIST_MAX_INPUT_SIZE	64

#define	UNKNOWN_LENGTH		-1

#define LINEHEIGHTS(lw,lines)	\
	((lines) * ((lw)->list.MaxItemHeight + (lw)->list.spacing))

#define RECOUNT_SELECTION	-1

/****************
 *
 * What do I set the inc to when I have no idea ofthe fonts??
 *
 ****************/
#define	CHAR_WIDTH_GUESS	10


/****************
 *
 * List Error Messages
 *
 ****************/

#define ListMessage0	_XmMMsgList_0000
#define ListMessage5	_XmMMsgList_0005
#define ListMessage6	_XmMMsgList_0006
#define ListMessage8	_XmMMsgList_0007
#define ListMessage11	_XmMMsgList_0008
#define ListMessage12	_XmMMsgList_0009
#define ListMessage13	_XmMMsgList_0010
#define ListMessage14	_XmMMsgList_0011
#define ListMessage15	_XmMMsgList_0012
#define ListMessage16	_XmMMsgList_0013
#define ListMessage17	_XmMMsgList_0014
#define ListMessage18	_XmMMsgList_0015

/********    Static Function Declarations    ********/

static void SliderMove(Widget w, XtPointer closure, XtPointer call_data);
static void NullRoutine(Widget wid);
static void ClassPartInitialize(WidgetClass wc);
static void Initialize(Widget request, Widget w,
		       ArgList args, Cardinal *num_args);
static void Redisplay(Widget wid, XEvent *event, Region region);
static void Resize(Widget wid);
static Dimension CalcVizWidth(XmListWidget lw);
static int ComputeVizCount(XmListWidget lw);
static Boolean SetValues(Widget old, Widget request, Widget new_w,
			 ArgList args, Cardinal *num_args);
static void Destroy(Widget wid);
static XtGeometryResult QueryProc(Widget wid,
				  XtWidgetGeometry *request,
				  XtWidgetGeometry *ret);
static void CheckSetRenderTable(Widget wid, int offset, XrmValue *value);
static void CvtToExternalPos(Widget wid, int offset, XtArgVal *value);
static XmImportOperator CvtToInternalPos(Widget wid,
					 int offset,
					 XtArgVal *value);
static void DrawListShadow(XmListWidget w);
static void DrawList(XmListWidget w, XEvent *event, Boolean all);
static void DrawItem(Widget w, int position);
static void DrawItems(XmListWidget lw, int top, int bot, Boolean all);
static void DrawHighlight(XmListWidget lw, int position, Boolean on);
static void SetClipRect(XmListWidget widget);
static void SetDefaultSize(XmListWidget lw,
			   Dimension *width,
			   Dimension *height,
			   Boolean reset_max_width,
			   Boolean reset_max_height);
static void MakeGC(XmListWidget lw);
static void MakeHighlightGC(XmListWidget lw, Boolean AddMode);
static void ChangeHighlightGC(XmListWidget lw, Boolean AddMode);
static Boolean SetVerticalScrollbar(XmListWidget lw);
static Boolean SetHorizontalScrollbar(XmListWidget lw);
static void SetNewSize(XmListWidget lw,
		       Boolean reset_max_width,
		       Boolean reset_max_height,
		       Dimension old_max_height);
static void ResetExtents(XmListWidget lw,
			 Boolean recache_extents);
static void FixStartEnd(XmListWidget lw,
			int pos, int count, int *start, int *end);
static int AddInternalElements(XmListWidget lw,
			       XmString *items,
			       int nitems,
			       int position,
			       Boolean selectable);
static int DeleteInternalElements(XmListWidget lw,
				  XmString string,
				  int position,
				  int count);
static int DeleteInternalElementPositions(XmListWidget lw,
					  int *position_list,
					  int position_count,
					  int oldItemCount);
static int ReplaceInternalElement(XmListWidget lw,
				  int position,
				  Boolean selectable);
static void AddItems(XmListWidget lw, XmString *items, int nitems, int pos);
static void DeleteItems(XmListWidget lw, int nitems, int pos);
static void DeleteItemPositions(XmListWidget lw,
				int *position_list,
				int position_count,
				Boolean track_kbd);
static void ReplaceItem(XmListWidget lw, XmString item, int pos);
static int ItemNumber(XmListWidget lw, XmString item);
static int ItemExists(XmListWidget lw, XmString item);
static Boolean OnSelectedList(XmListWidget lw, XmString item, int pos);
static void CopyItems(XmListWidget lw);
static void CopySelectedItems(XmListWidget lw);
static void CopySelectedPositions(XmListWidget lw);
static void ClearItemList(XmListWidget lw);
static void ClearSelectedList(XmListWidget lw);
static void ClearSelectedPositions(XmListWidget lw);
static void BuildSelectedList(XmListWidget lw, Boolean commit);
static void BuildSelectedPositions(XmListWidget lw, int count);
static void UpdateSelectedList(XmListWidget lw, Boolean rebuild);
static void UpdateSelectedPositions(XmListWidget lw, int count);
static int WhichItem(XmListWidget w, Position EventY);
static void SelectRange(XmListWidget lw, int first, int last, Boolean select);
static void RestoreRange(XmListWidget lw, int first, int last, Boolean dostart);
static void ArrangeRange(XmListWidget lw, int item);
static void HandleNewItem(XmListWidget lw, int item, int olditem);
static void HandleExtendedItem(XmListWidget lw, int item);
static void VerifyMotion(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void SelectElement(Widget wid,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);
static void KbdSelectElement(Widget wid,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);
static void UnSelectElement(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void KbdUnSelectElement(Widget wid,
			       XEvent *event,
			       String *params,
			       Cardinal *num_params);
static void ExSelect(Widget wid,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params);
static void ExUnSelect(Widget wid,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);
static void CtrlBtnSelect(Widget wid,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);
static void CtrlSelect(Widget wid,
		       XEvent *event,
		       String *params,
		       Cardinal *num_params);
static void CtrlUnSelect(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void KbdShiftSelect(Widget wid,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);
static void KbdShiftUnSelect(Widget wid,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);
static void KbdCtrlSelect(Widget wid,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);
static void KbdCtrlUnSelect(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void KbdActivate(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void KbdCancel(Widget wid,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);
static void KbdToggleAddMode(Widget wid,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);
static void KbdSelectAll(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void KbdDeSelectAll(Widget wid,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);
static void DefaultAction(XmListWidget lw, XEvent *event);
static void ClickElement(XmListWidget lw,
			 XEvent *event,
			 Boolean default_action);
static void ListFocusIn(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ListFocusOut(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void BrowseScroll(XtPointer closure, XtIntervalId *id);
static void ListLeave(Widget wid,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);
static void ListEnter(Widget wid,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);
static void MakeItemVisible(XmListWidget lw, int item);
static void PrevElement(XmListWidget lw,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void NextElement(XmListWidget lw,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void NormalNextElement(Widget wid,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);
static void ShiftNextElement(Widget wid,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);
static void CtrlNextElement(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void ExtendAddNextElement(Widget wid,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);
static void NormalPrevElement(Widget wid,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);
static void ShiftPrevElement(Widget wid,
			     XEvent *event,
			     String *params,
			     Cardinal *num_params);
static void CtrlPrevElement(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void ExtendAddPrevElement(Widget wid,
				 XEvent *event,
				 String *params,
				 Cardinal *num_params);
static void KbdPrevPage(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void KbdNextPage(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void KbdLeftChar(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void KbdLeftPage(Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void BeginLine(Widget wid,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params);
static void KbdRightChar(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void KbdRightPage(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void EndLine(Widget wid,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params);
static void TopItem(Widget wid,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params);
static void EndItem(Widget wid,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params);
static void ExtendTopItem(Widget wid,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);
static void ExtendEndItem(Widget wid,
			  XEvent *event,
			  String *params,
			  Cardinal *num_params);
static void ListItemVisible(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void ListCopyToClipboard(Widget wid,
				XEvent *event,
				String *params,
				Cardinal *num_params);
static void DragDropFinished(Widget w, XtPointer closure, XtPointer call_data);
static void ListProcessDrag(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void DragStart(XtPointer closure, XtIntervalId* id);
static void ListProcessBtn1(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void ListProcessBtn2(Widget wid,
			    XEvent *event,
			    String *params,
			    Cardinal *num_params);
static void ListQuickNavigate(Widget wid,
			      XEvent *event,
			      String *params,
			      Cardinal *num_params);
static wchar_t FirstChar(XmString string);
static Boolean CompareCharAndItem(XmListWidget lw,
				  wchar_t input_char,
				  int pos);
static void ListConvert(Widget, XtPointer, XmConvertCallbackStruct*);
static void ListPreDestProc(Widget, XtPointer, XmDestinationCallbackStruct*);
static void APIAddItems(XmListWidget lw,
                        XmString *items,
                        int item_count,
                        int pos,
                        Boolean select);
static void CleanUpList(XmListWidget lw, Boolean always);
static void APIReplaceItems(Widget w,
			    XmString *old_items,
			    int item_count,
			    XmString *new_items,
			    Boolean select);
static void APIReplaceItemsPos(Widget w,
			       XmString *new_items,
			       int item_count,
			       int position,
			       Boolean select);
static void APISelect(XmListWidget lw, int item_pos, Boolean notify);
static void APIDeletePositions(XmListWidget lw,
			       int *positions,
			       int count,
			       Boolean track_kbd);
static void SetSelectionParams(XmListWidget lw);
static void GetPreeditPosition(XmListWidget lw, XPoint *xmim_point);
static void ScrollBarDisplayPolicyDefault(Widget widget,
					  int offset,
					  XrmValue *value);
static void ListScrollUp(Widget wid,
			 XEvent *event,
			 String *params,
			 Cardinal *num_params);
static void ListScrollDown(Widget wid,
			   XEvent *event,
			   String *params,
			   Cardinal *num_params);

/********    End Static Function Declarations    ********/

/**************
 *
 *  Translation tables for List. These are used to drive the selections
 *
 **************/

#define ListXlations1	_XmList_ListXlations1
#define ListXlations2	_XmList_ListXlations2

/****************
 *
 *  Actions Lists
 *
 ****************/

static XtActionsRec ListActions[] = {
  { "ListButtonMotion",		  VerifyMotion		},
  { "ListShiftSelect",		  ExSelect		},
  { "ListShiftUnSelect",	  ExUnSelect		},
  { "ListBeginExtend",  	  ExSelect		},
  { "ListEndExtend",		  ExUnSelect		},
  { "ListCtrlSelect",  		  CtrlSelect		},
  { "ListCtrlUnSelect",		  CtrlUnSelect		},
  { "ListBeginToggle",  	  CtrlBtnSelect		},
  { "ListEndToggle",		  CtrlUnSelect		},
  { "ListShiftCtrlSelect",	  ExSelect		},
  { "ListShiftCtrlUnSelect",	  ExUnSelect		},
  { "ListExtendAddSelect",	  ExSelect		},
  { "ListExtendAddUnSelect",	  ExUnSelect		},
  { "ListItemSelect",		  SelectElement		},
  { "ListItemUnSelect",		  UnSelectElement	},
  { "ListBeginSelect",		  SelectElement		},
  { "ListEndSelect",		  UnSelectElement	},
  { "ListKbdBeginSelect",	  KbdSelectElement	},
  { "ListKbdEndSelect",		  KbdUnSelectElement	},
  { "ListKbdShiftSelect",	  KbdShiftSelect	},
  { "ListKbdShiftUnSelect",	  KbdShiftUnSelect	},
  { "ListKbdCtrlSelect",	  KbdCtrlSelect		},
  { "ListKbdCtrlUnSelect",  	  KbdCtrlUnSelect	},
  { "ListKbdBeginExtend",      	  KbdShiftSelect	},
  { "ListKbdEndExtend",  	  KbdShiftUnSelect	},
  { "ListKbdBeginToggle",      	  KbdCtrlSelect		},
  { "ListKbdEndToggle",  	  KbdCtrlUnSelect	},
  { "ListKbdSelectAll",    	  KbdSelectAll		},
  { "ListKbdDeSelectAll",	  KbdDeSelectAll	},
  { "ListKbdActivate",      	  KbdActivate		},
  { "ListKbdCancel",		  KbdCancel		},
  { "ListAddMode",		  KbdToggleAddMode	},
  { "ListPrevItem",      	  NormalPrevElement	},
  { "ListNextItem",      	  NormalNextElement	},
  { "ListPrevPage",    		  KbdPrevPage		},
  { "ListNextPage",    		  KbdNextPage		},
  { "ListLeftChar",    		  KbdLeftChar		},
  { "ListLeftPage",    		  KbdLeftPage		},
  { "ListRightChar",  		  KbdRightChar		},
  { "ListRightPage",  		  KbdRightPage		},
  { "ListCtrlPrevItem",  	  CtrlPrevElement	},
  { "ListCtrlNextItem",  	  CtrlNextElement	},
  { "ListShiftPrevItem",  	  ShiftPrevElement	},
  { "ListShiftNextItem",  	  ShiftNextElement	},
  { "List_ShiftCtrlPrevItem",	  ExtendAddPrevElement	},
  { "List_ShiftCtrlNextItem",	  ExtendAddNextElement	},
  { "ListAddPrevItem", 		  CtrlPrevElement	},
  { "ListAddNextItem", 		  CtrlNextElement	},
  { "ListExtendPrevItem", 	  ShiftPrevElement	},
  { "ListExtendNextItem", 	  ShiftNextElement	},
  { "ListExtendAddPrevItem",  	  ExtendAddPrevElement	},
  { "ListExtendAddNextItem",  	  ExtendAddNextElement	},
  { "ListBeginLine",  		  BeginLine		},
  { "ListEndLine",		  EndLine		},
  { "ListBeginData",		  TopItem		},
  { "ListEndData",		  EndItem		},
  { "ListBeginDataExtend",	  ExtendTopItem		},
  { "ListEndDataExtend",	  ExtendEndItem		},
  { "ListFocusIn",		  ListFocusIn		},
  { "ListFocusOut",		  ListFocusOut		},
  { "ListEnter",		  ListEnter		},
  { "ListLeave",		  ListLeave		},
  { "ListScrollCursorVertically", ListItemVisible	},
  { "ListScrollCursorVisible",	  ListItemVisible	},
  { "ListScrollUp",		  ListScrollUp		},
  { "ListScrollDown",		  ListScrollDown	},
  /* name above is correct; maintain this one for 1.2.0 compatibility */
  { "ListCopyToClipboard",	  ListCopyToClipboard	},
  { "ListProcessDrag",		  ListProcessDrag	},
  { "ListQuickNavigate",	  ListQuickNavigate	},
  { "ListProcessBtn1",		  ListProcessBtn1	},
  { "ListProcessBtn2",		  ListProcessBtn2	},
};

/************************************************************************
 *									*
 * XmList Resources.							*
 * 									*
 ************************************************************************/

static XtResource resources[] = {
  {
    XmNlistSpacing, XmCListSpacing, XmRVerticalDimension, sizeof(Dimension),
    XtOffsetOf(XmListRec, list.ItemSpacing),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNlistMarginWidth, XmCListMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmListRec, list.margin_width),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNlistMarginHeight, XmCListMarginHeight, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmListRec, list.margin_height),
    XmRImmediate, (XtPointer) 0
  },
  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmListRec, list.MouseMoved),
    XmRImmediate, (XtPointer)False
  },
  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmListRec, list.font),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },
  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmListRec, list.font),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },
  {
    XmNstringDirection, XmCStringDirection, XmRStringDirection,
    sizeof(XmStringDirection), XtOffsetOf(XmListRec, list.StrDir),
    XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
  },
#ifndef XM_PART_BC
  {
    XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
    sizeof(XmDirection), XtOffsetOf(XmPrimitiveRec, primitive.layout_direction),
    XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
  },
#endif
  {
    XmNitems, XmCItems, XmRXmStringTable,
    sizeof(XmStringTable), XtOffsetOf(XmListRec, list.items),
    XmRStringTable, NULL
  },
  {
    XmNitemCount, XmCItemCount, XmRInt,
    sizeof(int), XtOffsetOf(XmListRec, list.itemCount),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNselectedItems, XmCSelectedItems, XmRXmStringTable,
    sizeof(XmStringTable), XtOffsetOf(XmListRec, list.selectedItems),
    XmRStringTable, NULL
  },
  {
    XmNselectedItemCount, XmCSelectedItemCount, XmRInt,
    sizeof(int), XtOffsetOf(XmListRec, list.selectedItemCount),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNselectedPositions, XmCSelectedPositions, XmRPointer,
    sizeof(XtPointer), XtOffsetOf(XmListRec, list.selectedPositions),
    XmRImmediate, (XtPointer) NULL
  },
  {
    XmNselectedPositionCount, XmCSelectedPositionCount, XmRInt,
    sizeof(int), XtOffsetOf(XmListRec, list.selectedPositionCount),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNvisibleItemCount, XmCVisibleItemCount, XmRInt,
    sizeof(int), XtOffsetOf(XmListRec, list.visibleItemCount),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNtopItemPosition, XmCTopItemPosition, XmRTopItemPosition,
    sizeof(int), XtOffsetOf(XmListRec, list.top_position),
    XmRImmediate, (XtPointer) 0
  },
  {
    XmNselectionPolicy, XmCSelectionPolicy, XmRSelectionPolicy,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.SelectionPolicy),
    XmRImmediate, (XtPointer) XmBROWSE_SELECT
  },
  {
    XmNselectionMode, XmCSelectionMode, XmRSelectionMode,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.SelectionMode),
    XmRImmediate, (XtPointer) XmNORMAL_MODE
  },
  {
    XmNlistSizePolicy, XmCListSizePolicy, XmRListSizePolicy,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.SizePolicy),
    XmRImmediate, (XtPointer) XmVARIABLE
  },
  {
    XmNscrollBarDisplayPolicy, XmCScrollBarDisplayPolicy,
    XmRScrollBarDisplayPolicy,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.ScrollBarDisplayPolicy),
    XmRCallProc, (XtPointer) ScrollBarDisplayPolicyDefault
  },
  {
    XmNautomaticSelection, XmCAutomaticSelection, XmRAutomaticSelection,
    sizeof(XtEnum), XtOffsetOf(XmListRec, list.AutoSelect),
    XmRImmediate, (XtPointer) XmNO_AUTO_SELECT
  },
  {
    XmNdoubleClickInterval, XmCDoubleClickInterval, XmRInt,
    sizeof(int), XtOffsetOf(XmListRec, list.ClickInterval),
    XmRImmediate, (XtPointer) (-1)
  },
  {
    XmNsingleSelectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.SingleCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNmultipleSelectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.MultipleCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNextendedSelectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.ExtendCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNbrowseSelectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.BrowseCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNdefaultActionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.DefaultCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNdestinationCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmListRec, list.DestinationCallback),
    XmRCallback, (XtPointer)NULL
  },
  {
    XmNhorizontalScrollBar, XmCHorizontalScrollBar, XmRWidget,
    sizeof(Widget), XtOffsetOf(XmListRec, list.hScrollBar),
    XmRImmediate, NULL
  },
  {
    XmNverticalScrollBar, XmCVerticalScrollBar, XmRWidget,
    sizeof(Widget), XtOffsetOf(XmListRec, list.vScrollBar),
    XmRImmediate, NULL
  },
  {
    XmNnavigationType, XmCNavigationType, XmRNavigationType,
    sizeof(unsigned char), XtOffsetOf(XmListRec, primitive.navigation_type),
    XmRImmediate, (XtPointer) XmTAB_GROUP
  },
  {
    XmNprimaryOwnership, XmCPrimaryOwnership, XmRPrimaryOwnership,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.PrimaryOwnership),
    XmRImmediate, (XtPointer) XmOWN_NEVER
  },
  {
    XmNmatchBehavior, XmCMatchBehavior, XmRMatchBehavior,
    sizeof(unsigned char), XtOffsetOf(XmListRec, list.matchBehavior),
    XmRImmediate, (XtPointer) XmQUICK_NAVIGATE
  },
  {
    XmNselectColor, XmCSelectColor, XmRSelectColor,
    sizeof(Pixel), XtOffsetOf(XmListRec, list.selectColor),
    XmRImmediate, (XtPointer)XmREVERSED_GROUND_COLORS
  }
};

/****************
 *
 * Synthetic resources
 *
 ****************/

static XmSyntheticResource get_resources[] = {
  {
    XmNlistSpacing,
    sizeof(Dimension), XtOffsetOf(XmListRec, list.ItemSpacing),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },

  {
    XmNlistMarginWidth,
    sizeof(Dimension), XtOffsetOf(XmListRec, list.margin_width),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },

  {
    XmNlistMarginHeight,
    sizeof(Dimension), XtOffsetOf(XmListRec, list.margin_height),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },

  {
    XmNtopItemPosition,
    sizeof(int), XtOffsetOf(XmListRec, list.top_position),
    CvtToExternalPos, CvtToInternalPos
  },
};

/************************************************************************
 *									*
 * 	              Class record for XmList class			*
 *									*
 ************************************************************************/

static XmBaseClassExtRec BaseClassExtRec = {
  NULL,				/* next_extension	 */
  NULLQUARK,			/* record_type		 */
  XmBaseClassExtVersion,	/* version		 */
  sizeof(XmBaseClassExtRec),	/* record_size		 */
  NULL,				/* InitializePrehook	 */
  NULL,				/* SetValuesPrehook	 */
  NULL,				/* InitializePosthook	 */
  NULL,				/* SetValuesPosthook	 */
  NULL,				/* secondaryObjectClass	 */
  NULL,				/* secondaryCreate	 */
  NULL,				/* getSecRes data	 */
  { 0 },			/* fastSubclass flags	 */
  NULL,				/* get_values_prehook	 */
  NULL,				/* get_values_posthook	 */
  NULL,				/* classPartInitPrehook  */
  NULL,				/* classPartInitPosthook */
  NULL,				/* ext_resources         */
  NULL,				/* compiled_ext_resources*/
  0,				/* num_ext_resources     */
  FALSE,			/* use_sub_resources     */
  XmInheritWidgetNavigable,	/* widgetNavigable       */
  XmInheritFocusChange,		/* focusChange           */
};

externaldef(xmlistclassrec) XmListClassRec xmListClassRec = {
  {
    (WidgetClass) &xmPrimitiveClassRec,	/* superclass	      */
    "XmList",				/* class_name	      */
    sizeof(XmListRec),			/* widget_size	      */
    NULL,				/* class_initialize   */
    ClassPartInitialize,		/* class part init    */
    FALSE,				/* class_inited       */
    Initialize,				/* initialize	      */
    NULL,				/* widget init hook   */
    XtInheritRealize,			/* realize	      */
    ListActions,			/* actions	      */
    XtNumber(ListActions),		/* num_actions	      */
    resources,				/* resources	      */
    XtNumber(resources),		/* num_resources      */
    NULLQUARK,				/* xrm_class	      */
    True,				/* compress_motion    */
    XtExposeCompressMaximal |		/* compress_exposure  */
	XtExposeNoRegion,
    TRUE,				/* compress enter/exit*/
    FALSE,				/* visible_interest   */
    Destroy,				/* destroy	      */
    Resize,				/* resize	      */
    Redisplay,				/* expose	      */
    SetValues,				/* set values 	      */
    NULL,				/* set values hook    */
    XtInheritSetValuesAlmost,		/* set values almost  */
    NULL,				/* get_values hook    */
    NULL,				/* accept_focus	      */
    XtVersion,				/* version	      */
    NULL,				/* callback offset    */
    NULL,				/* default trans      */
    QueryProc,				/* query geo proc     */
    NULL,				/* disp accelerator   */
    (XtPointer) &BaseClassExtRec, 	/* extension          */
  },
  {
    NullRoutine,			/* border_highlight   */
    NullRoutine,			/* border_unhighlight */
    NULL,				/* translations       */
    NULL,				/* arm_and_activate   */
    get_resources,			/* get resources      */
    XtNumber(get_resources),		/* num get_resources  */
    NULL,				/* extension          */
  },
  {
    (XtPointer) NULL,			/* extension	      */
  }
};

externaldef(xmlistwidgetclass) WidgetClass xmListWidgetClass =
       (WidgetClass)&xmListClassRec;


/* Transfer trait record */

static const XmTransferTraitRec ListTransfer = {
  0, 						/* version */
  (XmConvertCallbackProc) ListConvert,		/* convertProc */
  NULL,						/* destinationProc */
  (XmDestinationCallbackProc) ListPreDestProc,	/* destinationPreHookProc */
};

static void
NullRoutine(Widget wid)		/* unused */
{
  /*EMPTY*/
}

/************************************************************************
 *									*
 *  SliderMove							        *
 *  Callback for the value changes of navigators.	                *
 *									*
 ************************************************************************/
static void
SliderMove(Widget w,
	   XtPointer closure,
	   XtPointer cd)
{
  /* w is a navigator widget */

  XmListWidget lw = (XmListWidget) closure;
  XmNavigatorDataRec nav_data;

  /* get the navigator information using the trait getValue since I
     cannot use a callback struct */

  nav_data.valueMask = NavValue;
  ((XmNavigatorTrait)XmeTraitGet((XtPointer) XtClass(w), XmQTnavigator))
    ->getValue(w, &nav_data);


  if (lw->list.Traversing)
    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);

  /* look at the kind of navigator and make the appropriate update */

  if (nav_data.dimMask & NavigDimensionX) {
    lw->list.XOrigin = nav_data.value.x;
    lw->list.hOrigin = nav_data.value.x;
  }

  if (nav_data.dimMask & NavigDimensionY) {
    lw->list.top_position = (int) nav_data.value.y;
  }

  DrawList(lw, NULL, TRUE);

  /* now update the other navigator value */
  _XmSFUpdateNavigatorsValue(XtParent((Widget)lw), &nav_data, False);

}

/************************************************************************
 *									*
 *  ClassPartInitialize - Set up the fast subclassing.			*
 *									*
 ************************************************************************/

static void
ClassPartInitialize(WidgetClass wc)
{
  char *xlats;

  _XmFastSubclassInit (wc, XmLIST_BIT);

  xlats = (char *)
    ALLOCATE_LOCAL(strlen(ListXlations1) + strlen(ListXlations2) + 1);
  strcpy(xlats, ListXlations1);
  strcat(xlats, ListXlations2);
  wc->core_class.tm_table =(String) XtParseTranslationTable(xlats);
  DEALLOCATE_LOCAL((char *)xlats);

  /* Install transfer trait */
  XmeTraitSet((XtPointer)wc, XmQTtransfer, (XtPointer) &ListTransfer);
}

/************************************************************************
 *									*
 * Initialize - initialize the instance.				*
 *									*
 ************************************************************************/
static void
Initialize(Widget request,
	   Widget w,
	   ArgList args,	/* unused */
	   Cardinal *num_args)	/* unused */
{
  XmListWidget lw = (XmListWidget) w;
  Dimension width, height;
  int i, j;
  XmScrollFrameTrait scrollFrameTrait;
  XrmValue val;

  lw->list.LastItem = 0;
  lw->list.Event = 0;
  lw->list.LastHLItem = 0;
  lw->list.StartItem = 0;
  lw->list.EndItem = 0;
  lw->list.OldStartItem = 0;
  lw->list.OldEndItem = 0;
  lw->list.DownCount = 0;
  lw->list.DownTime = 0;
  lw->list.NormalGC = NULL;
  lw->list.InverseGC = NULL;
  lw->list.HighlightGC = NULL;
  lw->list.InsensitiveGC = NULL;
  lw->list.XOrigin = 0;
  lw->list.Traversing = FALSE;
  lw->list.KbdSelection = FALSE;
  lw->list.CurrentKbdItem = 0;
  lw->list.AppendInProgress = FALSE;
  lw->list.FromSetSB = FALSE;
  lw->list.FromSetNewSize = FALSE;
  lw->list.DragID = 0;
  lw->list.MaxItemHeight = 0;
  lw->list.LeaveDir = 0;
  lw->list.hOrigin = 0;
  lw->list.hExtent = lw->list.hmax = 0;
  lw->list.AutoSelectionType = XmAUTO_UNSET;
  lw->list.LastSetVizCount = 0;		/* CR 6014 */
  lw->list.scratchRend = NULL;
  lw->list.drag_start_timer = 0;
  lw->list.drag_abort_action = 0;
  lw->list.MaxWidth = 0;


  /* For Quick Navigate */
  XmImRegister(w, 0);

  if (lw->list.ItemSpacing < 0)
    {
      lw->list.ItemSpacing = 0;
      XmeWarning((Widget) lw, ListMessage11);
    }

  if (lw->list.top_position < -1)
    {
      lw->list.top_position = 0;
      XmeWarning((Widget) lw, ListMessage15);
    }
  lw->list.previous_top_position = 0;

  if (lw->list.ClickInterval < 0)
    lw->list.ClickInterval = XtGetMultiClickTime(XtDisplay(lw));

  if (lw->primitive.highlight_thickness)
    lw->list.HighlightThickness = lw->primitive.highlight_thickness + 1;
  else
    lw->list.HighlightThickness = 0;

  lw->list.BaseX = ((Position)lw->list.margin_width +
		    lw->list.HighlightThickness +
		    lw->primitive.shadow_thickness);

  lw->list.BaseY = ((Position)lw->list.margin_height +
		    lw->list.HighlightThickness +
		    lw->primitive.shadow_thickness);

  lw->list.InternalList = NULL;

  if (!XmRepTypeValidValue(XmRID_SELECTION_POLICY,
			   lw->list.SelectionPolicy, (Widget) lw))
    lw->list.SelectionPolicy = XmBROWSE_SELECT;

  if (!XmRepTypeValidValue(XmRID_LIST_SIZE_POLICY,
			   lw->list.SizePolicy, (Widget) lw))
    lw->list.SizePolicy = XmVARIABLE;

  if (!XmRepTypeValidValue(XmRID_SCROLL_BAR_DISPLAY_POLICY,
			   lw->list.ScrollBarDisplayPolicy, (Widget) lw))
    lw->list.ScrollBarDisplayPolicy = XmAS_NEEDED;

  if (!XmRepTypeValidValue(XmRID_PRIMARY_OWNERSHIP,
			   lw->list.PrimaryOwnership, (Widget) lw))
    lw->list.PrimaryOwnership = XmOWN_NEVER;

  if (!XmRepTypeValidValue(XmRID_MATCH_BEHAVIOR,
			   lw->list.matchBehavior, (Widget) lw))
    lw->list.matchBehavior = XmQUICK_NAVIGATE;

#ifndef NO_XM_1_2_BC
  /*
   * Some pre-Motif 2.0 XmManager subclasses may be bypassing the
   * synthetic resouce GetValues hook and passing us the manager's raw
   * string_direction field (which is now a layout_direction).  Fixup
   * the common/simple cases.
   */
  switch (lw->list.StrDir)
    {
    case XmLEFT_TO_RIGHT:
    case XmRIGHT_TO_LEFT:
      /* These string directions are erroneous uses of layout directions. */
      lw->list.StrDir = XmDirectionToStringDirection(lw->list.StrDir);
      break;
    default:
      /* Other unknown layout directions will still get a warning. */
      break;
    }
#endif

  /* If layout_direction is set, it overrides string_direction.
   * If string_direction is set, but not layout_direction, use
   * string_direction value.  If neither is set, get from parent.
   */
  if (XmPrim_layout_direction(lw) != XmDEFAULT_DIRECTION)
    {
      if (lw->list.StrDir == XmDEFAULT_DIRECTION)
	lw->list.StrDir =
	  XmDirectionToStringDirection(XmPrim_layout_direction(lw));
    }
  else if (lw->list.StrDir != XmDEFAULT_DIRECTION)
    {
      XmPrim_layout_direction(lw) =
	XmStringDirectionToDirection(lw->list.StrDir);
    }
  else
    {
      XmPrim_layout_direction(lw) = _XmGetLayoutDirection(XtParent(w));
      lw->list.StrDir =
	XmDirectionToStringDirection(XmPrim_layout_direction(lw));
    }

  if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION,
			   lw->list.StrDir, (Widget) lw))
    lw->list.StrDir = XmSTRING_DIRECTION_L_TO_R;

  if (lw->list.font == NULL)
    lw->list.font = XmeGetDefaultRenderTable ((Widget) lw,XmTEXT_FONTLIST);
  lw->list.font = XmFontListCopy(lw->list.font);


  /*
   * Force selection mode to appropriate value, except for Extended
   * Select, where both modes are allowed (default is Normal, set in
   * the XtResource record)
   */

  if ((lw->list.SelectionPolicy == XmMULTIPLE_SELECT) ||
      (lw->list.SelectionPolicy == XmSINGLE_SELECT))
    lw->list.SelectionMode = XmADD_MODE;
  else if (lw->list.SelectionPolicy == XmBROWSE_SELECT)
    lw->list.SelectionMode = XmNORMAL_MODE;
  else if (!XmRepTypeValidValue(XmRID_SELECTION_MODE,
				lw->list.SelectionMode, (Widget) lw))
    lw->list.SelectionMode = XmADD_MODE;

  /* Deal with selectColor */
  lw->list.scratchRend = XmRenditionCreate(NULL, XmS, NULL, 0);
  if (lw->list.selectColor == XmDEFAULT_SELECT_COLOR)
    {
      _XmSelectColorDefault((Widget)lw,
			    XtOffsetOf(XmListRec, list.selectColor),
			    &val);
      lw->list.selectColor = *((Pixel*) val.addr);
    }
  else if (lw->list.selectColor == XmHIGHLIGHT_COLOR)
    {
      lw->list.selectColor = lw->primitive.highlight_color;
    }

  MakeGC(lw);
  MakeHighlightGC(lw, (lw->list.SelectionMode == XmADD_MODE));

  lw->list.spacing = lw->list.ItemSpacing + lw->list.HighlightThickness;

  /* Copy the font, item and selected item lists into our space.
   * THE USER IS RESPONSIBLE FOR FREEING THE ORIGINAL LISTS!
   */
  ASSIGN_MAX(lw->list.itemCount, 0);
  ASSIGN_MAX(lw->list.selectedItemCount, 0);
  ASSIGN_MAX(lw->list.selectedPositionCount, 0);

  if ((lw->list.itemCount && !lw->list.items) ||
      (!lw->list.itemCount && lw->list.items))
    {
      XmeWarning((Widget) lw, ListMessage16);
    }
  if (lw->list.top_position == -1)
    lw->list.top_position = lw->list.itemCount ? lw->list.itemCount - 1 : 0;

  CopyItems(lw);
  CopySelectedItems(lw);

  /* If we have items, add them to the internal list and calculate our
   * default size.
   */

  if (lw->list.items && (lw->list.itemCount > 0))
    {
      lw->list.InternalList = NULL;
      (void) AddInternalElements(lw, lw->list.items, lw->list.itemCount,
				 0, TRUE);

      /* CR 9375: Preserve initial selectedItems a little longer. */
      if (!lw->list.selectedItemCount)
	{
	  /* CR 5560: Clear selected list to avoid memory leak. */
	  ClearSelectedList(lw);
	  BuildSelectedList(lw, TRUE);
	  BuildSelectedPositions(lw, lw->list.selectedItemCount);
	}
      else
	{
	  BuildSelectedPositions(lw, RECOUNT_SELECTION);
	}
    }
  else
    {
      /* The selectedPositionList has not been copied yet, so we can't */
      /* use ClearSelectedPositions() because it would free it. */
      lw->list.selectedPositionCount = 0;
      lw->list.selectedPositions = NULL;
    }

  /* BEGIN OSF Fix pir 2730 */
  /* Must have at least one visible. */
  /* Save from AddInternalElement */
  lw->list.visibleItemCount = ((XmListWidget)request)->list.visibleItemCount;
  if (lw->list.visibleItemCount < 0)
    {
      lw->list.visibleItemCount = 1;
      XmeWarning((Widget)lw, ListMessage0);
    }
  else if (lw->list.visibleItemCount == 0)
    {
      /* Assume that the user didn't set it.*/
      lw->list.visibleItemCount = ComputeVizCount(lw);
    }
  else
    {
      /* Otherwise leave it to whatever it was set. */
      lw->list.LastSetVizCount = lw->list.visibleItemCount;
    }
  /* END OSF Fix pir 2730 */

  SetDefaultSize(lw, &width, &height, False, False);
  SetSelectionParams(lw);

  if (!request->core.width)
    lw->core.width = width;
  if (!request->core.height)
    lw->core.height = height;
  else
    {
      /* We got a height - make sure viz tracks */
      lw->list.visibleItemCount = ComputeVizCount(lw);
    }

  /* CR 8298: The spot location may depend on lw->list.spacing. */
  /* CR 8436: The spot location may depend on lw->list.top_position. */
  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget) lw,
		      XmNspotLocation, &xmim_point,
		      XmNfontList, lw->list.font, NULL);
    }

  /*
   * OK, we're all set for the list stuff. Now look at our parent and see
   * if it's a non inited ScrollFrame. If it is, create the navigators
   * and set up all the scrolling stuff using the trait.
   */

  scrollFrameTrait = (XmScrollFrameTrait)
    XmeTraitGet((XtPointer) XtClass(lw->core.parent), XmQTscrollFrame);

  if (scrollFrameTrait == NULL ||
      scrollFrameTrait->getInfo (lw->core.parent, NULL, NULL, NULL))
    {
      lw->list.Mom = NULL;
      return;
    }


  /*
   * Set up the default move callback so that our navigator gets
   * associated nicely by the scrollFrame.
   */
  scrollFrameTrait->init (lw->core.parent, SliderMove, (Widget)lw);

  lw->list.Mom = (XmScrolledWindowWidget) lw->core.parent;
  /* The cast above is necessary because of the type of the Mom field,
   * but ScrolledWindow is no longer required, ScrollFrame is. */

  {
    Arg vSBArgs[11];

    i = 0;
    XtSetArg (vSBArgs[i], XmNorientation, XmVERTICAL), i++;
    XtSetArg (vSBArgs[i], XmNunitType, XmPIXELS), i++;
    XtSetArg (vSBArgs[i], XmNshadowThickness,
	      lw->primitive.shadow_thickness), i++;
    XtSetArg (vSBArgs[i], XmNhighlightThickness, 0), i++;
    XtSetArg(vSBArgs[i], XmNtraversalOn, FALSE), i++;
    assert(i <= XtNumber(vSBArgs));

    lw->list.vScrollBar = (XmScrollBarWidget)
      XmCreateScrollBar((Widget) lw->list.Mom, "VertScrollBar", vSBArgs, i);

    SetVerticalScrollbar(lw);
  }


  /* Only create the horizontal sb if in static size mode. */
  if (lw->list.SizePolicy != XmVARIABLE)
    {
      /* This code probably not needed, since SetHorizontalScrollbar
       * does the same kind of thing. */
      lw->list.hmin = 0;
      lw->list.hmax = lw->list.MaxWidth + (lw->list.BaseX * 2);
      lw->list.hExtent = lw->core.width;
      lw->list.hOrigin = lw->list.XOrigin;
      if ((lw->list.hExtent + lw->list.hOrigin) > lw->list.hmax)
	lw->list.hExtent = lw->list.hmax - lw->list.hOrigin;

      {
	Arg hSBArgs[11];

	j = 0;

	XtSetArg (hSBArgs[j], XmNorientation, XmHORIZONTAL), j++;
	XtSetArg (hSBArgs[j], XmNunitType, XmPIXELS), j++;
	XtSetArg (hSBArgs[j], XmNshadowThickness,
		  lw->primitive.shadow_thickness), j++;
	XtSetArg (hSBArgs[j], XmNhighlightThickness, 0), j++;
	XtSetArg(hSBArgs[j], XmNtraversalOn, FALSE), j++;
	assert(j <= XtNumber(hSBArgs));

	lw->list.hScrollBar = (XmScrollBarWidget)
	  XmCreateScrollBar((Widget)lw->list.Mom, "HorScrollBar", hSBArgs,j);

	SetHorizontalScrollbar(lw);
      }
    }
}

/************************************************************************
 *									*
 * ReDisplay - draw the visible list items.				*
 *									*
 ************************************************************************/
static void
Redisplay(Widget wid,
	  XEvent *event,
	  Region region)
{
  XmListWidget lw = (XmListWidget) wid;

  DrawListShadow(lw);
  DrawList(lw, event, TRUE);

  /* CR 6529: Redraw the highlight too. */
  if (lw->list.Traversing)
    DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
}

/************************************************************************
 *									*
 * Resize - redraw the list in response to orders from above.		*
 *									*
 ************************************************************************/

static void
Resize(Widget wid)
{
  XmListWidget lw = (XmListWidget) wid;
  int listwidth, top;
  int viz;

  /* Don't allow underflow! */
  int borders = 2 * (lw->list.margin_width +
		     lw->list.HighlightThickness +
		     lw->primitive.shadow_thickness);
  if ((int)lw->core.width <= borders)
    listwidth = 1;
  else
    listwidth = lw->core.width - borders;

  /****************
   *
   * The current strategy here is to assume that the user initiated the
   * resize request on me, or on my parent. As such, we will calculate a
   * new visible item count, even though it may confuse the thing that
   * set the visible count in the first place.
   * Oh, well.
   *
   ****************/

  top = lw->list.top_position;
  viz = ComputeVizCount(lw);

  /* CR 2782: Only force top_position if we've lost vScrollBar. */
  if (!lw->list.vScrollBar || !XtIsManaged((Widget)lw->list.vScrollBar))
    if ((lw->list.itemCount - top) < viz)
      {
	top = lw->list.itemCount -  viz;
	ASSIGN_MAX(top, 0);
	lw->list.top_position = top;
      }

  lw->list.LastSetVizCount = lw->list.visibleItemCount = viz;
  SetVerticalScrollbar(lw);

  if (lw->list.SizePolicy != XmVARIABLE)
    {
      if ((int)(lw->list.MaxWidth - lw->list.XOrigin) < listwidth)
	lw->list.XOrigin = lw->list.MaxWidth - listwidth;

      ASSIGN_MAX(lw->list.XOrigin, 0);
      SetHorizontalScrollbar(lw);
    }

  if (XtIsRealized((Widget)lw))
    SetClipRect(lw);
}

/************************************************************************
 *									*
 * ComputeVizCount - return the number of items that would fit in the	*
 * current height.  If there are no items, guess.			*
 *									*
 ************************************************************************/

static int
ComputeVizCount(XmListWidget lw)
{
  int viz, lineheight;
  int listheight;
  /*  BEGIN OSF Fix pir 2730 */
  XFontStruct *font_struct = (XFontStruct *) NULL;
  /* END OSF Fix pir 2730 */

  /*  Don't let listheight underflow to a large positive number! */
  int borders = 2 * (lw->primitive.shadow_thickness +
		     lw->list.HighlightThickness +
		     lw->list.margin_height);
  if ((int)lw->core.height <= borders)
    listheight = 1;
  else
    listheight = lw->core.height - borders;

  /* CR 2730 */
  if (lw->list.InternalList && lw->list.itemCount)
    {
      /* Just use the calculated heights of the items. */
      lineheight = lw->list.MaxItemHeight;
    }
  else /* Have to guess by getting height of default font. */
    {
#if USE_XFT
      XmRenderTableGetDefaultFontExtents(lw->list.font,
                                         &lineheight, NULL, NULL);
      if (lineheight == 0)
        lineheight = 1;
#else
      if (XmeRenderTableGetDefaultFont(lw->list.font, &font_struct))
	lineheight = font_struct->ascent + font_struct->descent;
      else
	lineheight = 1;
#endif
    }

  if (lineheight + lw->list.spacing != 0)
    viz = ((int)(listheight + lw->list.spacing) /
	   (int)(lineheight + lw->list.spacing));
  else
    viz = (int)(listheight + lw->list.spacing);

  if (!viz)
    viz++;			/* Always have at least one item visible */

  /* END OSF Fix pir 2730 */

  return viz;
}

/************************************************************************
 *									*
 * SetValues - change the instance data					*
 *									*
 ************************************************************************/
static Boolean
SetValues(Widget old,
	  Widget request,
	  Widget new_w,
	  ArgList args,
	  Cardinal *num_args)
{
  XmListWidget oldlw = (XmListWidget) old;
  XmListWidget newlw = (XmListWidget) new_w;
  Boolean new_size = FALSE;
  Boolean new_size_viz = FALSE;
  Boolean reset_max = FALSE;
  Boolean redraw = FALSE;
  Boolean reset_select = FALSE;
  Boolean build_selected_list = FALSE;
  Dimension width = 0;
  Dimension height = 0;
  int i, j;
  XrmValue val;

  if (!XmRepTypeValidValue(XmRID_SELECTION_POLICY,
			   newlw->list.SelectionPolicy, (Widget) newlw))
    newlw->list.SelectionPolicy = oldlw->list.SelectionPolicy;

  if (!XmRepTypeValidValue(XmRID_SELECTION_MODE,
			   newlw->list.SelectionMode, (Widget) newlw))
    newlw->list.SelectionMode = oldlw->list.SelectionMode;

  if (!XmRepTypeValidValue(XmRID_PRIMARY_OWNERSHIP,
			   newlw->list.PrimaryOwnership, (Widget) newlw))
    newlw->list.PrimaryOwnership = oldlw->list.PrimaryOwnership;

  if (!XmRepTypeValidValue(XmRID_MATCH_BEHAVIOR,
			   newlw->list.matchBehavior, (Widget) newlw))
    newlw->list.matchBehavior = oldlw->list.matchBehavior;

  /*
   * Selection Policy changes take precedence over Selection Mode changes.
   * If an attempt is made to change both resources and they do not
   * complement one another, the Selection Policy value will be used.
   * XmEXTENDED_SELECT policy can be in ADD or NORMAL mode.
   */
  if ((newlw->list.SelectionPolicy != oldlw->list.SelectionPolicy) ||
      (newlw->list.SelectionMode != oldlw->list.SelectionMode))
    {
      if ((newlw->list.SelectionPolicy == XmMULTIPLE_SELECT) ||
	  (newlw->list.SelectionPolicy == XmSINGLE_SELECT))
	newlw->list.SelectionMode = XmADD_MODE;
      else if (newlw->list.SelectionPolicy == XmBROWSE_SELECT)
	newlw->list.SelectionMode = XmNORMAL_MODE;

      if (newlw->list.SelectionMode != oldlw->list.SelectionMode)
	{
	  DrawHighlight(newlw, newlw->list.CurrentKbdItem, FALSE);
	  ChangeHighlightGC(newlw,
			    (newlw->list.SelectionMode == XmADD_MODE));
	  DrawHighlight(newlw, newlw->list.CurrentKbdItem, TRUE);
	}
    }

  if (!XmRepTypeValidValue(XmRID_LIST_SIZE_POLICY,
			   newlw->list.SizePolicy, (Widget) newlw))
    newlw->list.SizePolicy = oldlw->list.SizePolicy;

  if (!XmRepTypeValidValue(XmRID_SCROLL_BAR_DISPLAY_POLICY,
			   newlw->list.ScrollBarDisplayPolicy, (Widget) newlw))
    newlw->list.ScrollBarDisplayPolicy = oldlw->list.ScrollBarDisplayPolicy;

  if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION, newlw->list.StrDir,
			   (Widget) newlw))
    newlw->list.StrDir = oldlw->list.StrDir;


  if (newlw->list.SizePolicy != oldlw->list.SizePolicy)
    {
      XmeWarning((Widget) newlw, ListMessage5);
      newlw->list.SizePolicy = oldlw->list.SizePolicy;
    }

  /****************
   *
   * Visual attributes
   *
   ****************/

  if (newlw->list.StrDir != oldlw->list.StrDir) redraw = TRUE;

  /* CR 6907: Redraw if layout direction changes. */
  if (LayoutIsRtoLP(newlw) != LayoutIsRtoLP(oldlw))
    redraw = TRUE;

  if ((newlw->list.margin_width != oldlw->list.margin_width) ||
      (newlw->list.margin_height != oldlw->list.margin_height))
    new_size = TRUE;

  if (newlw->list.ItemSpacing != oldlw->list.ItemSpacing)
    {
      if (newlw->list.ItemSpacing >= 0)
	{
	  reset_max = new_size = TRUE;
	}
      else
	{
	  newlw->list.ItemSpacing = oldlw->list.ItemSpacing;
	  XmeWarning((Widget) newlw, ListMessage11);
	}
    }

  if ((newlw->list.ItemSpacing != oldlw->list.ItemSpacing) ||
      (newlw->primitive.highlight_thickness !=
       oldlw->primitive.highlight_thickness))
    {
      reset_max = new_size = TRUE;

      if (newlw->primitive.highlight_thickness)
	newlw->list.HighlightThickness =
	  newlw->primitive.highlight_thickness + 1;
      else
	newlw->list.HighlightThickness = 0;

      newlw->list.spacing = (newlw->list.HighlightThickness +
			     newlw->list.ItemSpacing);
      if (newlw->primitive.highlight_thickness !=
	  oldlw->primitive.highlight_thickness)
	ChangeHighlightGC(newlw, (newlw->list.SelectionMode == XmADD_MODE));

      ResetExtents(newlw, False);
      reset_max = FALSE;
    }

  /* CR 5583: Attempt to resize if shadow thickness changes. */
  if (newlw->primitive.shadow_thickness != oldlw->primitive.shadow_thickness)
    new_size = TRUE;

  if (newlw->list.visibleItemCount != oldlw->list.visibleItemCount)
    {
      if (newlw->list.visibleItemCount < 0)
    	{
	  newlw->list.visibleItemCount = oldlw->list.visibleItemCount;
	  XmeWarning((Widget) newlw, ListMessage0);
    	}
      else if (newlw->list.visibleItemCount == 0)
	{
	  /* CR 6014, 8655, 9604: Stop forcing list size. */
	  new_size_viz = TRUE;
	  newlw->list.LastSetVizCount = 0;
	  newlw->list.visibleItemCount = ComputeVizCount(newlw);
	}
      else
        {
	  new_size_viz = TRUE;
	  newlw->list.LastSetVizCount = newlw->list.visibleItemCount;
	  newlw->list.visibleItemCount = oldlw->list.visibleItemCount;
        }
      redraw = TRUE;
    }

  if (XtIsSensitive(new_w) != XtIsSensitive(old))
    {
      /* CR 6412: Redraw all the time, not just when we've a Mom. */
      redraw = TRUE;

      if (! XtIsSensitive(new_w))
	{
	  DrawHighlight(newlw, newlw->list.CurrentKbdItem, FALSE);
	  newlw->list.Traversing = FALSE;
	}
    }

  /* Select color */
  if (newlw->list.selectColor != oldlw->list.selectColor)
    {
      if (newlw->list.selectColor == XmDEFAULT_SELECT_COLOR)
	{
	  _XmSelectColorDefault((Widget)newlw,
				XtOffsetOf(struct _XmListRec, list.selectColor),
				&val);
	  newlw->list.selectColor = *((Pixel*) val.addr);
	}
      else if (newlw->list.selectColor == XmHIGHLIGHT_COLOR)
	{
	  newlw->list.selectColor = newlw->primitive.highlight_color;
	}

      if (newlw->list.selectColor != oldlw->list.selectColor)
	{
	  redraw = TRUE;
	  MakeGC(newlw);
	}
    }

  /****************
   *
   * See if the Selected Items list or the Selected Positions list
   * has changed. If so, free up the old ones, and allocate the new.
   *
   ****************/
  if ((newlw->list.selectedItems != oldlw->list.selectedItems) ||
      (newlw->list.selectedItemCount != oldlw->list.selectedItemCount))
    {
      if (newlw->list.selectedItems && (newlw->list.selectedItemCount > 0))
        {
	  CopySelectedItems(newlw);
	  ClearSelectedList(oldlw);
	  if (newlw->list.selectedPositions == oldlw->list.selectedPositions)
	    newlw->list.selectedPositions = NULL;
	  else
	    CopySelectedPositions(newlw);
	  ClearSelectedPositions(oldlw);
	  reset_select = TRUE;
        }
      else if (newlw->list.selectedItemCount == 0)
        {
	  ClearSelectedList(oldlw);
	  ClearSelectedPositions(oldlw);
	  newlw->list.selectedItems = NULL;
	  newlw->list.selectedPositions = NULL;
	  reset_select = TRUE;
        }
      else if ((newlw->list.selectedItemCount > 0) &&
	       (newlw->list.selectedItems == NULL))
        {
	  XmeWarning((Widget) newlw, ListMessage14);
	  newlw->list.selectedItems = oldlw->list.selectedItems;
	  newlw->list.selectedItemCount = oldlw->list.selectedItemCount;
	  newlw->list.selectedPositions = oldlw->list.selectedPositions;
	  newlw->list.selectedPositionCount = oldlw->list.selectedPositionCount;
        }
      else
        {
	  XmeWarning((Widget) newlw, ListMessage13);
	  newlw->list.selectedItems = oldlw->list.selectedItems;
	  newlw->list.selectedItemCount = oldlw->list.selectedItemCount;
	  newlw->list.selectedPositions = oldlw->list.selectedPositions;
	  newlw->list.selectedPositionCount = oldlw->list.selectedPositionCount;
        }
    }
  else if ((newlw->list.selectedPositions != oldlw->list.selectedPositions) ||
	   (newlw->list.selectedPositionCount !=
	    oldlw->list.selectedPositionCount))
    {
      if (newlw->list.selectedPositions &&
	  (newlw->list.selectedPositionCount > 0))
        {
	  ClearSelectedList(oldlw);
	  ClearSelectedPositions(oldlw);
	  CopySelectedPositions(newlw);
	  reset_select = TRUE;
	  build_selected_list = TRUE;
        }
      else if ((newlw->list.selectedPositionCount > 0) &&
	       (newlw->list.selectedPositions == NULL))
        {
	  XmeWarning((Widget) newlw, ListMessage18);
	  newlw->list.selectedPositions = oldlw->list.selectedPositions;
	  newlw->list.selectedPositionCount = oldlw->list.selectedPositionCount;
        }
      else
        {
	  XmeWarning((Widget) newlw, ListMessage17);
	  newlw->list.selectedPositions = oldlw->list.selectedPositions;
	  newlw->list.selectedPositionCount = oldlw->list.selectedPositionCount;
        }
    }


  /*
   * If the item list has changed to valid data, free up the old and create
   * the new. If the item count went to zero, delete the old internal list.
   * If the count went < 0, or is > 0 with a NULL items list, complain.
   */
  if ((newlw->list.items != oldlw->list.items) ||
      (newlw->list.itemCount != oldlw->list.itemCount))
    {
      CopyItems(newlw);		/* Fix for CR 5571 */
      if (newlw->list.items && (newlw->list.itemCount > 0))
    	{
	  if (oldlw->list.items && (oldlw->list.itemCount > 0))
            {
	      j = oldlw->list.itemCount;
	      oldlw->list.itemCount = 0;
	      DeleteInternalElements(oldlw, NULL, 1, j);
	      oldlw->list.itemCount = j;
	      ClearItemList(oldlw);
            }
	  reset_select = TRUE;
	  newlw->list.LastItem = 0;
	  newlw->list.LastHLItem = 0;
	  newlw->list.InternalList = NULL;
	  if ((newlw->list.top_position + newlw->list.visibleItemCount) >
	      newlw->list.itemCount)
	    newlw->list.top_position =
	      MAX(newlw->list.itemCount - newlw->list.visibleItemCount, 0);

	  AddInternalElements(newlw, newlw->list.items, newlw->list.itemCount,
			      0, TRUE);
	  reset_max = new_size = TRUE;
	  newlw->list.XOrigin = 0;
	  newlw->list.CurrentKbdItem = 0;
	}
      else
	{
	  if (newlw->list.itemCount == 0)
            {
	      j = oldlw->list.itemCount;
	      oldlw->list.itemCount = 0;
	      DeleteInternalElements(oldlw, NULL, 1, j);
	      oldlw->list.itemCount = j;
	      ClearItemList(oldlw);
	      newlw->list.LastItem = 0;
	      newlw->list.LastHLItem = 0;
	      newlw->list.InternalList = NULL;
	      newlw->list.items = NULL;
	      reset_select = TRUE;
	      reset_max = new_size = TRUE;
	      if ((newlw->list.top_position + newlw->list.visibleItemCount) >
		  newlw->list.itemCount)
                newlw->list.top_position =
		  MAX(newlw->list.itemCount - newlw->list.visibleItemCount, 0);

	      newlw->list.XOrigin = 0;
	      newlw->list.CurrentKbdItem = 0;
            }
	  else
            {
	      if ((newlw->list.itemCount > 0) && (newlw->list.items == NULL))
                {
		  XmeWarning((Widget) newlw, ListMessage12);
		  newlw->list.items = oldlw->list.items;
		  newlw->list.itemCount = oldlw->list.itemCount;
                }
	      else
                {
		  XmeWarning((Widget) newlw, ListMessage6);
		  newlw->list.items = oldlw->list.items;
		  newlw->list.itemCount = oldlw->list.itemCount;
                }
            }
	}

    }

  if (newlw->primitive.highlight_color != oldlw->primitive.highlight_color ||
      newlw->primitive.highlight_pixmap != oldlw->primitive.highlight_pixmap)
    {
      MakeHighlightGC(newlw, (newlw->list.SelectionMode == XmADD_MODE));
    }

  if ((newlw->primitive.foreground != oldlw->primitive.foreground) ||
      (newlw->core.background_pixel != oldlw->core.background_pixel) ||
      (newlw->list.font != oldlw->list.font))
    {
      if (newlw->list.font == NULL)
	{
	  XmFontList font = XmeGetDefaultRenderTable(new_w, XmTEXT_FONTLIST);
	  newlw->list.font = XmFontListCopy(font);
	}
      else if (newlw->list.font != oldlw->list.font)
	{
	  newlw->list.font = XmFontListCopy(newlw->list.font);
	}

      if (newlw->list.font != oldlw->list.font)
	{
	  XmFontListFree(oldlw->list.font);
	  new_size = TRUE;
	  ResetExtents(newlw, True);
	  reset_max = FALSE;
	}

      MakeGC(newlw);
      redraw = TRUE;
    }

  if (newlw->list.top_position != oldlw->list.top_position)
    {
      if (newlw->list.top_position < -1)
    	{
	  newlw->list.top_position = oldlw->list.top_position;
	  XmeWarning((Widget) newlw, ListMessage15);
    	}
      else
        {
	  /* BEGIN OSF Fix CR 5740 */
	  if (newlw->list.top_position == -1)
	    newlw->list.top_position =
	      (newlw->list.itemCount ? newlw->list.itemCount - 1 : 0);
	  /* END OSF Fix CR 5740 */

          if (oldlw->list.Traversing)
	    DrawHighlight(oldlw, oldlw->list.CurrentKbdItem, FALSE);
	  DrawList(newlw, NULL, TRUE);
	  SetVerticalScrollbar(newlw);
        }
    }

  if (reset_select)
    {
      for (i = 0; i < newlw->list.itemCount; i++)
        {
	  newlw->list.InternalList[i]->selected =
	    OnSelectedList(newlw, newlw->list.items[i], i);
	  newlw->list.InternalList[i]->last_selected =
	    newlw->list.InternalList[i]->selected;
        }

      /* Only build this if selectedPositions was changed */
      if (build_selected_list)
	{
	  BuildSelectedList(newlw, TRUE);
	  UpdateSelectedPositions(newlw, newlw->list.selectedItemCount);
	}
      else
	{
	  UpdateSelectedPositions(newlw, RECOUNT_SELECTION);
	}

      if (!new_size)
	{
	  DrawList (newlw, NULL, TRUE);
	  SetSelectionParams(newlw);
	}
    }

  if (new_size || new_size_viz)
    {
      redraw = TRUE;
      SetDefaultSize(newlw, &width, &height, reset_max, reset_max);
      newlw->list.BaseX = (Position)newlw->list.margin_width +
	newlw->list.HighlightThickness +
	  newlw->primitive.shadow_thickness;

      newlw->list.BaseY = (Position)newlw->list.margin_height +
	newlw->list.HighlightThickness +
	  newlw->primitive.shadow_thickness;

      if (((newlw->list.SizePolicy != XmCONSTANT) ||
	  !(newlw->core.width)) && new_size)
	newlw->core.width = width;
      newlw->core.height = height;
    }

  if (!newlw->list.FromSetNewSize)
    {
      if (newlw->list.SizePolicy != XmVARIABLE)
	SetHorizontalScrollbar(newlw);
      SetVerticalScrollbar(newlw);
    }

  if (newlw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(newlw, &xmim_point);
      if (newlw->list.font != oldlw->list.font ||
	  oldlw->list.matchBehavior != XmQUICK_NAVIGATE)
	XmImVaSetValues((Widget)newlw, XmNspotLocation, &xmim_point,
			XmNfontList, newlw->list.font, NULL);
      else
	XmImVaSetValues((Widget)newlw, XmNspotLocation, &xmim_point, NULL);
    }

  return redraw;
}

/************************************************************************
 *									*
 * Destroy - destroy the list instance.  Free up all of our memory.	*
 *									*
 ************************************************************************/

static void
Destroy(Widget wid)
{
  XmListWidget lw = (XmListWidget) wid;
  int j;

  if (lw->list.drag_start_timer)
    XtRemoveTimeOut(lw->list.drag_start_timer);
  if (lw->list.DragID)
    XtRemoveTimeOut(lw->list.DragID);
  if (lw->list.NormalGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.NormalGC);
  if (lw->list.InverseGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.InverseGC);
  if (lw->list.HighlightGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.HighlightGC);
  if (lw->list.InsensitiveGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.InsensitiveGC);
  if (lw->list.scratchRend != NULL)
    XmRenditionFree(lw->list.scratchRend);

  if (lw->list.itemCount)
    {
      j = lw->list.itemCount;
      lw->list.itemCount = 0;
      DeleteInternalElements(lw, NULL, 1, j);
      lw->list.itemCount = j;
      ClearItemList(lw);
    }

  ClearSelectedList(lw);
  ClearSelectedPositions(lw);
  XmFontListFree(lw->list.font);

  XmImUnregister(wid);
}

/************************************************************************
 *									*
 *  QueryProc - Look at a new geometry and add/delete scrollbars as     *
 *	needed.								*
 *									*
 ************************************************************************/

static XtGeometryResult
QueryProc(Widget wid,
	  XtWidgetGeometry *request,
	  XtWidgetGeometry *ret)
{
  XmListWidget lw = (XmListWidget) wid;
  Dimension    MyWidth, MyHeight, NewWidth, NewHeight, sbWidth, sbHeight;
  Dimension    vizheight, lineheight, HSBheight = 0, VSBwidth = 0;
  Dimension    HSBht = 0 , VSBht = 0;
  Dimension    pad = 0, HSBbw = 0, VSBbw = 0;
  int          viz;
  Boolean      HasVSB, HasHSB;
  XtGeometryResult retval = XtGeometryYes;

  ret->request_mode = 0;


  /* If this is a request generated by our code, just return yes. */
  if (lw->list.FromSetSB)
    return XtGeometryYes;

  /*********
    This code doesn't work, I'll find out later why,
    for now just out back the 1.2 code, which access the internal
    of ScrolledW and ScrollBar.

    get the pad from our parent, whatever its class is.


    XtVaGetValues(lw->core.parent, XmNspacing, &pad, NULL);

    if (lw->list.hScrollBar) {
    XtVaGetValues((Widget)lw->list.hScrollBar,
    XmNhighlightThickness, &HSBht,
    XmNborderWidth, &HSBbw,
    XmNheight, &HSBheight, NULL);
    }
    HSBht *= 2;

    if (lw->list.vScrollBar) {
    XtVaGetValues((Widget)lw->list.vScrollBar,
    XmNhighlightThickness, &VSBht,
    XmNborderWidth, &VSBbw,
    XmNheight, &VSBwidth, NULL);
    }
    VSBht *= 2;
    ****************/

  pad = (lw->list.Mom ?
	 ((XmScrolledWindowWidget)(lw->list.Mom))->swindow.pad : 0);

  HSBht = (lw->list.hScrollBar ?
	   lw->list.hScrollBar->primitive.highlight_thickness * 2 : 0);
  HSBbw = (lw->list.hScrollBar ? lw->list.hScrollBar->core.border_width : 0);
  HSBheight = (lw->list.hScrollBar ? lw->list.hScrollBar->core.height : 0);
  VSBht = (lw->list.vScrollBar ?
	   lw->list.vScrollBar->primitive.highlight_thickness * 2 : 0);
  VSBwidth = (lw->list.vScrollBar ? lw->list.vScrollBar->core.width : 0);
  VSBbw = (lw->list.vScrollBar ? lw->list.vScrollBar->core.border_width : 0);
  /*--- end of bad code (but works:-) */


  /* If a preferred size query, make sure we use the last requested visible
   * item count for our basis. */
  if (request->request_mode == 0)
    {
      viz = lw->list.visibleItemCount;
      lw->list.LastSetVizCount = lw->list.visibleItemCount;
      SetDefaultSize(lw, &MyWidth, &MyHeight, True, True);
      lw->list.visibleItemCount = viz;
    }
  else
    SetDefaultSize(lw, &MyWidth, &MyHeight, True, True);

  /* If the request mode is zero, fill out out default height & width. */
  if ((request->request_mode == 0) ||
      !lw->list.InternalList)
    {
      ret->width = MyWidth;
      ret->height = MyHeight;
      ret->request_mode = (CWWidth | CWHeight);
      return XtGeometryAlmost;
    }

  /* If we're not scrollable, or don' have any scrollbars, return yes. */
  if ((!lw->list.Mom) ||
      (!lw->list.vScrollBar && !lw->list.hScrollBar))
    return XtGeometryYes;

  /****************
   *
   * Else we're being queried from a scrolled window - look at the
   * dimensions and manage/unmanage the scroll bars according to the
   * new size. The scrolled window will take care of the actual sizing.
   *
   ****************/

  if (request->request_mode & CWWidth)
    NewWidth = request->width;
  else
    NewWidth = lw->core.width;

  if (request->request_mode & CWHeight)
    NewHeight = request->height;
  else
    NewHeight = lw->core.height;

  /****************
   *
   * Look at the dimensions and calculate if we need a scrollbar. This can
   * get hairy in the boundry conditions - where the addition/deletion of
   * one scrollbar can affect the other.
   *
   ****************/
  if (((NewHeight < MyHeight) &&
       (NewWidth  < MyWidth))  ||
      (lw->list.ScrollBarDisplayPolicy == XmSTATIC))
    {
      HasVSB = HasHSB = TRUE;
    }
  else
    {
      /****************
       *
       * Else we have do do some hard work. See if there is a definite
       * need for a horizontal scroll bar; and set the availible
       * height accordingly. Then, figure out how many lines will fit
       * in the space. If that is less than the number of items. then
       * fire up the vertical scrollbar. Then check to see if the act
       * of adding the vsb kicked in the hsb.  Amazingly, it *seems*
       * to work right.
       *
       ****************/

      lineheight = lw->list.MaxItemHeight;

      {
	/* Don't let NewHeight underflow to become a large positive # */
	int borders = (2 * (lw->primitive.shadow_thickness +
			    lw->list.HighlightThickness +
			    lw->list.margin_height));

	if ((int)NewHeight <= borders)
	  NewHeight = 1;
	else
	  NewHeight -= borders;
      }

      if ((MyWidth > NewWidth) && (lw->list.SizePolicy != XmVARIABLE))
	{
	  /* Take the height of the horizontal SB into account, but */
	  /* don't allow sbHeight to underflow to a large positive number. */
	  int borders = HSBheight + HSBht + HSBbw*2 + pad;

	  if ((int)NewHeight <= borders)
	    sbHeight = 1;
	  else
	    sbHeight = NewHeight - borders;
	}
      else
	sbHeight = NewHeight;

      viz = 0;
      vizheight = lineheight;
      while (lineheight && vizheight <= sbHeight)
	{
	  vizheight += lineheight + lw->list.spacing;
	  viz++;
	}
      if (!viz)
	viz++;

      HasVSB = (lw->list.itemCount > viz);
      if (HasVSB)
	{
	  /* Take the width of the vertical SB into account, but don't */
	  /* allow sbWidth to underflow to a large positive number */
	  int borders = VSBwidth + VSBht + VSBbw*2 + pad;
	  if ((int)NewWidth <= borders)
	    sbWidth = 1;
	  else
	    sbWidth = NewWidth - borders;
	}
      else
	sbWidth = NewWidth;

      HasHSB = (MyWidth > sbWidth);
    }

  if (lw->list.vScrollBar)
    {
      if (HasVSB)
	XtManageChild((Widget) lw->list.vScrollBar);
      else
	XtUnmanageChild((Widget) lw->list.vScrollBar);
    }

  if (lw->list.hScrollBar)
    {
      if (HasHSB && (lw->list.SizePolicy != XmVARIABLE))
	XtManageChild((Widget) lw->list.hScrollBar);
      else
	XtUnmanageChild((Widget) lw->list.hScrollBar);
    }

  return retval;
}

/*
 * Dynamic default for ScrollBarDisplayPolicy based on the
 * type of CDE FileSB parent
 */
static void
ScrollBarDisplayPolicyDefault(Widget widget,
			      int offset, /* unused */
			      XrmValue *value)
{
  static unsigned char sb_display_policy;

  value->addr = (XPointer) &sb_display_policy;

  /* If this is a scrolledlist in a filesb */
  if (XmIsScrolledWindow(XtParent(widget)) &&
      XmIsFileSelectionBox(XtParent(XtParent(widget))))
    {
      XtEnum path_mode;

      XtVaGetValues(XtParent(XtParent(widget)),
		    XmNpathMode, &path_mode, NULL);

      if (path_mode == XmPATH_MODE_RELATIVE)
	sb_display_policy = XmAS_NEEDED;
      else
	sb_display_policy = XmSTATIC;
    }
  else
    sb_display_policy = XmAS_NEEDED;
}

/*
 * XmRCallProc routine for checking list.font before setting it to NULL
 * if no value is specified for both XmNrenderTable and XmNfontList.
 * If MouseMoved == True then function has been called twice on same widget, thus
 * resource needs to be set NULL, otherwise leave it alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmListWidget lw = (XmListWidget)wid;

  if (lw->list.MouseMoved)
	value->addr = NULL;
  else {
	value->addr = (char*)&(lw->list.font);
	lw->list.MouseMoved = True;
  }
}

/************************************************************************
 *                                                                      *
 * Conversion routines for XmNtopItemPostion.  Necessary because the    *
 * outside world is 1-based, and the inside world is 0-based.  Sigh.    *
 *                                                                      *
 ************************************************************************/
static void
CvtToExternalPos(Widget wid,
		 int offset,
		 XtArgVal *value)
{
  XmListWidget lw = (XmListWidget) wid;

  (*value) = (XtArgVal) (lw->list.top_position + 1);
}

static XmImportOperator
CvtToInternalPos(Widget wid,
		 int offset,
		 XtArgVal *value)
{
  (* (int *) value)--;

  return XmSYNTHETIC_LOAD;
}

/************************************************************************
 *									*
 *                           Visiual Routines				*
 *									*
 ************************************************************************/

/************************************************************************
 *									*
 * DrawListShadow - draw the shadow					*
 *									*
 ************************************************************************/
static void
DrawListShadow(XmListWidget w)
{
  XmeDrawShadows(XtDisplay(w), XtWindow(w),
		 w->primitive.bottom_shadow_GC, w->primitive.top_shadow_GC,
		 0, 0, (int)w->core.width, (int)w->core.height,
		 w->primitive.shadow_thickness,
		 XmSHADOW_OUT);
}

static Dimension
CalcVizWidth(XmListWidget lw)
{
  /* Don't allow underflow */
  int borders = 2 * ((int)lw->list.margin_width +
		     lw->list.HighlightThickness +
		     lw->primitive.shadow_thickness);

  if ((int)lw->core.width <= borders)
    return 1;
  else
    return lw->core.width - borders;
}

/************************************************************************
 *									*
 * DrawList - draw the contents of the list.				*
 *									*
 ************************************************************************/
static void
DrawList(XmListWidget lw,
	 XEvent *event,
	 Boolean all)
{
  Position y = 0;
  int      top, num;

  if (!XtIsRealized((Widget)lw))
    return;

  if (lw->list.items && lw->list.itemCount)
    {
      SetClipRect(lw);

      lw->list.BaseY = ((int)lw->list.margin_height +
			lw->list.HighlightThickness +
			lw->primitive.shadow_thickness);

      top = lw->list.top_position;
      num = top + lw->list.visibleItemCount;
      ASSIGN_MIN(num, lw->list.itemCount);

      DrawItems(lw, top, num, all);

      if (top <  num)
	y = LINEHEIGHTS(lw, num - top - 1) + lw->list.BaseY;
      y += lw->list.MaxItemHeight;

      {
	/* Don't allow underflow! */
	int available_height;

	if (lw->core.height <= (Dimension)lw->list.BaseY)
	  available_height = 1;
	else
	  available_height = lw->core.height - lw->list.BaseY;

	if (y < available_height)
	  XClearArea (XtDisplay (lw), XtWindow (lw), lw->list.BaseX, y,
		      CalcVizWidth(lw), (available_height - y), False);
      }

      if (lw->list.Traversing)
        {
	  if (lw->list.CurrentKbdItem >= lw->list.itemCount)
	    lw->list.CurrentKbdItem = lw->list.itemCount - 1;
	  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	    {
	      XPoint xmim_point;

	      GetPreeditPosition(lw, &xmim_point);
	      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
	    }
	  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
        }
    }
}

/************************************************************************
 *									*
 * DrawItem - Draw the specified item from the internal list.		*
 *									*
 ************************************************************************/

static void
DrawItem(Widget w,
	 int position)
{
  XmListWidget lw = (XmListWidget) w;

  if (!XtIsRealized((Widget)lw))
    return;

  if ((position >= lw->list.itemCount)  ||
      (position < lw->list.top_position)||
      (position >= (lw->list.top_position + lw->list.visibleItemCount)))
    return;

  if (lw->list.InternalList[position]->selected ==
      lw->list.InternalList[position]->LastTimeDrawn)
    return;

  SetClipRect(lw);

  DrawItems(lw, position, position + 1, TRUE);
}

/************************************************************************
 *									*
 * DrawItems - draw some list items.					*
 *									*
 ************************************************************************/
static void
DrawItems(XmListWidget lw,
	  int top,
	  int bot,
	  Boolean all)
{
  int      pos;
  Position x, y = 0;
  int	   width = CalcVizWidth(lw);
  GC       gc;

  if (LayoutIsRtoLP(lw))
    x = lw->list.BaseX + lw->list.XOrigin;
  else
    x = lw->list.BaseX - lw->list.XOrigin;

  for (pos = top; pos < bot; pos++)
    {
      y = LINEHEIGHTS(lw, pos - lw->list.top_position) + lw->list.BaseY;

      if (!all &&
	  (lw->list.InternalList[pos]->selected ==
	   lw->list.InternalList[pos]->LastTimeDrawn))
	break;


      lw->list.InternalList[pos]->LastTimeDrawn =
	lw->list.InternalList[pos]->selected;

      /* Need to pad dimensions by one because of X fill semantics. */
      XFillRectangle(XtDisplay(lw), XtWindow(lw),
		     ((lw->list.InternalList[pos]->selected) ?
		      lw->list.NormalGC : lw->list.InverseGC),
			  lw->list.BaseX, y - 1,
			  width + 1, lw->list.MaxItemHeight + 1);

      if (XtIsSensitive((Widget)lw))
	gc = ((lw->list.InternalList[pos]->selected) ?
	      lw->list.InverseGC : lw->list.NormalGC);
      else
	  {
		gc = lw->list.InsensitiveGC;
		_XmRendFG(lw->list.scratchRend) = _XmAssignInsensitiveColor((Widget)lw);
	  }

      /* CR 7281: Set rendition background too. */
      if ((lw->list.InternalList[pos]->selected) &&
	  (lw->list.selectColor == XmREVERSED_GROUND_COLORS))
	{
	  /* CR 7635: Fix selected insensitive item stippling. */
	  if (XtIsSensitive((Widget)lw))
	    {
	      _XmRendFG(lw->list.scratchRend) = lw->core.background_pixel;
	      _XmRendBG(lw->list.scratchRend) = lw->primitive.foreground;
	    }
	  else
	      _XmRendBG(lw->list.scratchRend) = lw->core.background_pixel;
	  _XmRendFGState(lw->list.scratchRend) = XmFORCE_COLOR;
	  _XmRendBGState(lw->list.scratchRend) = XmFORCE_COLOR;
	}
      else
	{
	  if (XtIsSensitive((Widget)lw))
	  {
		  _XmRendFG(lw->list.scratchRend) = lw->primitive.foreground;
		  _XmRendFGState(lw->list.scratchRend) = XmAS_IS;
		  _XmRendBG(lw->list.scratchRend) = lw->core.background_pixel;
		  _XmRendBGState(lw->list.scratchRend) = XmAS_IS;
	  }
	  else
		  _XmRendBG(lw->list.scratchRend) = lw->core.background_pixel;
	}

      _XmRendGC(lw->list.scratchRend) = gc;
#if USE_XFT
      _XmRendXftFG(lw->list.scratchRend) =
          _XmXftGetXftColor(XtDisplay(lw), _XmRendFG(lw->list.scratchRend));
#endif

if (!XtIsSensitive((Widget)lw))
{
	/*Draw shadow for insensitive text*/
	Pixel p;
	p = _XmRendFG(lw->list.scratchRend);
	_XmRendFG(lw->list.scratchRend) = lw->primitive.top_shadow_color;
	_XmStringRender(XtDisplay(lw),
		      XtWindow(lw),
		      lw->list.font,
		      lw->list.scratchRend,
		      (_XmString)lw->list.items[pos],
		      x + 1,
		      y + 1 + ((int)(lw->list.MaxItemHeight -
				   lw->list.InternalList[pos]->height) >> 1),
		      width,
		      XmALIGNMENT_BEGINNING,
		      lw->list.StrDir);
	_XmRendFG(lw->list.scratchRend) = p;
}
      /* CR 9204: Let _XmStringRender handle right-to-left drawing. */
      _XmStringRender(XtDisplay(lw),
		      XtWindow(lw),
		      lw->list.font,
		      lw->list.scratchRend,
		      (_XmString)lw->list.items[pos],
		      x,
		      y + ((int)(lw->list.MaxItemHeight -
				   lw->list.InternalList[pos]->height) >> 1),
		      width,
		      XmALIGNMENT_BEGINNING,
		      lw->list.StrDir);
    }
}

/************************************************************************
 *									*
 * DrawHighlight - Draw or clear the traversal highlight on an item.	*
 *									*
 ************************************************************************/

static void
DrawHighlight(XmListWidget lw,
	      int position,
	      Boolean on)
{
  Dimension  width, height, ht;
  Position   x, y;
  XRectangle rect;

  if (!XtIsRealized((Widget)lw) ||
      !lw->list.Traversing ||
      (lw->list.HighlightThickness < 1))
    return;

  ht = lw->list.HighlightThickness;
  x = lw->list.BaseX - ht;
  width = lw->core.width - 2 * ((int)lw->list.margin_width +
				lw->primitive.shadow_thickness);

  /****************
   *
   * First check for an "invisible" highlight...
   *
   ****************/
  if ((position < lw->list.top_position) ||
      (lw->list.items == NULL)           ||
      (lw->list.itemCount == 0)          ||
      (position >= (lw->list.top_position + lw->list.visibleItemCount)))
    {
      y = lw->list.BaseY - ht;
      height = lw->core.height - 2 * ((int)lw->list.margin_height +
				      lw->primitive.shadow_thickness);
    }
  else
    {
      if (position >= lw->list.itemCount)
	position = lw->list.itemCount - 1;
      y = (LINEHEIGHTS(lw, position - lw->list.top_position) +
	   lw->list.BaseY - ht);
      height = lw->list.MaxItemHeight + (2 * ht);
    }

  if (width <= 0 || height <= 0)
    return;

  {
    /* Set highlight clip */
    rect.x = x;
    rect.y = lw->list.BaseY - ht;
    rect.width = width;
    rect.height = lw->core.height - 2 * rect.y;
    XSetClipRectangles(XtDisplay(lw), lw->list.HighlightGC, 0, 0,
		       &rect, 1, Unsorted);
  }

  ht = lw->primitive.highlight_thickness;
  if (on)
    {
      if (lw->list.SelectionMode == XmADD_MODE) {
	ChangeHighlightGC(lw, True);
	_XmDrawHighlight(XtDisplay (lw), XtWindow (lw),
			 lw->list.HighlightGC,
			 x, y, width, height, ht,
			 LineDoubleDash);
      } else
	XmeDrawHighlight(XtDisplay (lw), XtWindow (lw),
			 lw->list.HighlightGC,
			 x, y, width, height, ht);
    }
  else
    {
      XmeClearBorder(XtDisplay (lw), XtWindow (lw),
		     x, y, width, height, ht);
    }

}

/************************************************************************
 *									*
 * SetClipRect - set a clipping rectangle for the visible area of the	*
 * list.								*
 *									*
 ************************************************************************/

static void
SetClipRect(XmListWidget widget)
{
  XmListWidget lw = widget;
  Position x,y, ht;
  Dimension w,h;
  XRectangle rect;

  ht = lw->list.HighlightThickness;
  x = lw->list.margin_width + ht + lw->primitive.shadow_thickness;
  y = lw->list.margin_height + ht + lw->primitive.shadow_thickness;
  w = ((int)lw->core.width <= 2 * x) ? 1 : (lw->core.width - (2 * x));
  h = ((int)lw->core.height <= 2 * y) ? 1 : (lw->core.height - (2 * y));

  rect.x = 0;
  rect.y = 0;
  rect.width = w;
  rect.height = h;

#if USE_XFT
    _XmXftSetClipRectangles(XtDisplay(lw), XtWindow(lw), x, y,
	                    &rect, 1);
#endif

  if (lw->list.NormalGC)
    XSetClipRectangles(XtDisplay(lw), lw->list.NormalGC, x, y,
		       &rect, 1, Unsorted);

  if (lw->list.InverseGC)
    XSetClipRectangles(XtDisplay(lw), lw->list.InverseGC, x, y,
		       &rect, 1, Unsorted);

  if (lw->list.InsensitiveGC)
    XSetClipRectangles(XtDisplay(lw), lw->list.InsensitiveGC, x, y,
		       &rect, 1, Unsorted);

  /* Set highlight clip in DrawHighlight */
}

/***************************************************************************
 *									   *
 * SetDefaultSize							   *
 * Take an instance of the list widget and figure out how big the list	   *
 * work area should be. This uses the last set visible item count, because *
 * that's really what we want for a default.                               *
 *									   *
 ***************************************************************************/

static void
SetDefaultSize(XmListWidget lw,
	       Dimension *width,
	       Dimension *height,
	       Boolean reset_max_width,
	       Boolean reset_max_height)
{
  int visheight, wideborder, viz, max_height = 0;
  XFontStruct *fs = (XFontStruct *)NULL;

  wideborder = 2 * (lw->primitive.shadow_thickness +
		    lw->list.HighlightThickness +
		    lw->list.margin_width);

  /* BEGIN OSF Fix CR 5460, 6014 */
  if (lw->list.LastSetVizCount)
    viz = lw->list.LastSetVizCount;
  else
    viz = lw->list.visibleItemCount;
  /* END OSF Fix CR 5460, 6014 */

  if (lw->list.itemCount == 0)
    {
#if USE_XFT
      XmRenderTableGetDefaultFontExtents(lw->list.font, &max_height, NULL, NULL);

      lw->list.MaxItemHeight = (Dimension)max_height;
      if (lw->list.MaxItemHeight == 0)
        lw->list.MaxItemHeight = 1;
#else
      (void)max_height;
      if (XmeRenderTableGetDefaultFont(lw->list.font, &fs))
	lw->list.MaxItemHeight = fs->ascent + fs->descent;
      else
	lw->list.MaxItemHeight = 1;
#endif
    }
  else if (reset_max_width || reset_max_height)
    {
      ResetExtents(lw, False);
    }

  if (viz > 0)
    visheight = (LINEHEIGHTS(lw, viz - 1) + lw->list.MaxItemHeight);
  else
    visheight = lw->list.MaxItemHeight;

  *height = visheight + (2 * (lw->primitive.shadow_thickness +
			      lw->list.HighlightThickness +
			      lw->list.margin_height));

  if (lw->list.itemCount == 0)
    lw->list.MaxWidth = visheight >> 1;

  /* If no list, but realized, stay the same width. */
  if ((lw->list.itemCount) || (!XtIsRealized((Widget)lw)))
    *width = lw->list.MaxWidth + wideborder;
  else
    *width = lw->core.width;
}

/************************************************************************
 *									*
 * MakeGC - Get the GC's for normal and inverse.			*
 *									*
 ************************************************************************/

static void
MakeGC(XmListWidget lw)
{
  XGCValues values;
  XtGCMask  modifyMask;
  XtGCMask  valueMask;
  XFontStruct *fs = (XFontStruct *) NULL;

  valueMask = GCForeground | GCBackground | GCClipMask | GCGraphicsExposures;

  if (lw->list.NormalGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.NormalGC);

  if (lw->list.InverseGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.InverseGC);

  if (lw->list.InsensitiveGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.InsensitiveGC);

  /*
   * This is sloppy - get the default font and use it for the GC.
   * The StringDraw routines will change it if needed.
   */
  if (XmeRenderTableGetDefaultFont(lw->list.font, &fs))
    values.font = fs->fid, valueMask |= GCFont;

  values.graphics_exposures = False;
  if (lw->list.selectColor == XmREVERSED_GROUND_COLORS)
    values.foreground = lw->primitive.foreground;
  else
    values.foreground = lw->list.selectColor;
  values.background = lw->core.background_pixel;
  values.clip_mask = None;

  /* CR 7663: SetClipRect() modifies the clip rectangles. */
  modifyMask = GCClipMask | GCClipXOrigin | GCClipYOrigin;

  lw->list.NormalGC = XtAllocateGC((Widget) lw, lw->core.depth,
				   valueMask, &values, modifyMask, 0);

  values.foreground = lw->core.background_pixel;
  values.background = lw->primitive.foreground;

  lw->list.InverseGC = XtAllocateGC((Widget) lw, lw->core.depth,
				    valueMask, &values, modifyMask, 0);

  values.background = lw->core.background_pixel;

  /*generally gray insensitive foreground (instead stipple)*/
  values.foreground = _XmAssignInsensitiveColor((Widget)lw);

  lw->list.InsensitiveGC = XtAllocateGC((Widget) lw, lw->core.depth,
					valueMask, &values, modifyMask, 0);
}

/************************************************************************
 *									*
 *  MakeHighlightGC - Get the graphics context used for drawing the	*
 *                    highlight border. I have to use my own because I  *
 *		      need to set a clip rectangle, and I don't want to *
 *		      do that on the standard one (it's cached and 	*
 *		      shared among instances - that's the same reason I *
 *		      have to use the X calls.)				*
 *									*
 ************************************************************************/

static void
MakeHighlightGC(XmListWidget lw,
		Boolean AddMode)
{
  XGCValues values;
  XtGCMask  valueMask;
  XtGCMask  modifyMask;

  valueMask = (GCForeground | GCBackground | GCLineWidth |
	       GCLineStyle | GCDashList);
  values.foreground = lw->primitive.highlight_color;
  values.background = lw->core.background_pixel;
  values.line_width = lw->primitive.highlight_thickness;
  values.dashes = MAX(values.line_width, 8);

  values.line_style = (AddMode) ? LineDoubleDash : LineSolid;

  if (lw->list.HighlightGC != NULL)
    XtReleaseGC((Widget) lw, lw->list.HighlightGC);

  modifyMask = (GCLineStyle | GCLineWidth | GCDashList |
		GCClipXOrigin | GCClipYOrigin | GCClipMask);
  lw->list.HighlightGC = XtAllocateGC((Widget) lw, lw->core.depth,
				      valueMask, &values, modifyMask, 0);
}

/************************************************************************
 *                                                                      *
 * ChangeHighlightGC - change the highlight GC for add mode.  If        *
 * AddMode is true, change the fill style to dashed.  Else set the      *
 * fill style to solid                                                  *
 *                                                                      *
 ************************************************************************/

static void
ChangeHighlightGC(XmListWidget lw,
		  Boolean AddMode)
{
  XtGCMask  valueMask;
  XGCValues values;

  valueMask = GCLineStyle | GCLineWidth | GCDashList;
  values.line_width = lw->primitive.highlight_thickness;
  values.dashes = MAX(values.line_width, 8);
  values.line_style = (AddMode) ? LineDoubleDash : LineSolid;

  if (lw->list.HighlightGC)
    XChangeGC (XtDisplay(lw), lw->list.HighlightGC, valueMask, &values);
}

/************************************************************************
 *									*
 * SetVerticalScrollbar - set up all the vertical scrollbar stuff.	*
 *									*
 * Set up on an item basis. Min is 0, max is ItemCount, origin is	*
 * top_position, extent is visibleItemCount.				*
 *									*
 ************************************************************************/

static Boolean
SetVerticalScrollbar(XmListWidget lw)
{
  int viz;
  XmNavigatorDataRec nav_data;
  Boolean was_managed, is_managed;

  if ((!lw->list.Mom) ||
      (!lw->list.vScrollBar) ||
      (lw->list.FromSetSB))
    return False;


  lw->list.FromSetSB = TRUE;
  viz = ComputeVizCount(lw);

  was_managed = XtIsManaged((Widget) lw->list.vScrollBar);
  if (lw->list.ScrollBarDisplayPolicy == XmAS_NEEDED)
    {
      if (((lw->list.itemCount <= viz) && (lw->list.top_position == 0)) ||
	  (lw->list.itemCount == 0))
	XtUnmanageChild((Widget) lw->list.vScrollBar);
      else
	XtManageChild((Widget) lw->list.vScrollBar);
    }
  else
    XtManageChild((Widget) lw->list.vScrollBar);
  is_managed = XtIsManaged((Widget) lw->list.vScrollBar);


  if (lw->list.items && lw->list.itemCount)
    {
      int vmax = lw->list.itemCount;
      int vOrigin = lw->list.top_position;
      int vExtent = MIN(lw->list.visibleItemCount, lw->list.itemCount);

      /* CR 8889: Size slider based on visible item count. */
      ASSIGN_MAX(vmax, vExtent + vOrigin);

      nav_data.value.y = vOrigin;
      nav_data.minimum.y = 0;
      nav_data.maximum.y = vmax;
      nav_data.slider_size.y = vExtent;
      nav_data.increment.y = 1;
      nav_data.page_increment.y = ((lw->list.visibleItemCount > 1) ?
				   lw->list.visibleItemCount - 1  : 1);

      nav_data.dimMask = NavigDimensionY;
      nav_data.valueMask = (NavValue | NavMinimum | NavMaximum |
			    NavSliderSize | NavIncrement | NavPageIncrement);
      _XmSFUpdateNavigatorsValue(XtParent((Widget)lw), &nav_data, True);
    }
  else if (XtIsManaged((Widget) lw->list.vScrollBar))
    {
      nav_data.value.y = 0;
      nav_data.minimum.y = 0;
      nav_data.maximum.y = 1;
      nav_data.slider_size.y = 1;
      nav_data.increment.y = 1;
      nav_data.page_increment.y = 1;

      nav_data.dimMask = NavigDimensionY;
      nav_data.valueMask = (NavValue | NavMinimum | NavMaximum |
			    NavSliderSize | NavIncrement | NavPageIncrement);
      _XmSFUpdateNavigatorsValue(XtParent((Widget)lw), &nav_data, True);
    }

  lw->list.FromSetSB = FALSE;

  return (was_managed != is_managed);
}

/************************************************************************
 *									*
 * SetHorizontalScrollbar - set up all the horizontal scrollbar stuff.	*
 *									*
 * This is set up differently than the vertical scrollbar. This is on a *
 * pixel basis, so redraws are kinda slow. Min is 0, max is (MaxWidth   *
 * + 2*border).								*
 *									*
 ************************************************************************/

static Boolean
SetHorizontalScrollbar(XmListWidget lw)
{
  Arg hSBArgs[1];
  int j = 0, listwidth;
  Dimension pginc;
  XmNavigatorDataRec nav_data;
  Boolean was_managed, is_managed;

  if ((!lw->list.Mom) ||
      (!lw->list.hScrollBar) ||
      (lw->list.FromSetSB))
    return False;

  lw->list.FromSetSB = TRUE;

  listwidth = lw->core.width - 2 * (int)(lw->list.margin_width +
					 lw->list.HighlightThickness +
					 lw->primitive.shadow_thickness);

  was_managed = XtIsManaged((Widget) lw->list.vScrollBar);
  if (lw->list.ScrollBarDisplayPolicy == XmAS_NEEDED)
    {
      /* CR 9663: Hide scrollbars in empty lists. */
      if ((lw->list.MaxWidth <= (Dimension)listwidth) ||
	  (lw->list.itemCount == 0))
	{
	  lw->list.BaseX = ((int) lw->list.margin_width +
			    lw->list.HighlightThickness +
			    lw->primitive.shadow_thickness);

	  lw->list.XOrigin = 0;
	  XtUnmanageChild((Widget) lw->list.hScrollBar);
	}
      else
	XtManageChild((Widget) lw->list.hScrollBar);
    }
  else
    XtManageChild((Widget) lw->list.hScrollBar);
  is_managed = XtIsManaged((Widget) lw->list.vScrollBar);


  if (lw->list.items && lw->list.itemCount)
    {
      if (LayoutIsRtoLP(lw))
	XtSetArg(hSBArgs[j], XmNprocessingDirection, XmMAX_ON_LEFT), j++;
      else
	XtSetArg(hSBArgs[j], XmNprocessingDirection, XmMAX_ON_RIGHT), j++;
      assert(j <= XtNumber(hSBArgs));
      XtSetValues((Widget) lw->list.hScrollBar, hSBArgs, j);

      lw->list.hmax = lw->list.MaxWidth + (lw->list.BaseX * 2);
      lw->list.hExtent = lw->core.width;
      ASSIGN_MAX(lw->list.XOrigin, 0);
      lw->list.hOrigin = lw->list.XOrigin;
      if ((lw->list.hExtent + lw->list.hOrigin) > lw->list.hmax)
	lw->list.hExtent = lw->list.hmax - lw->list.hOrigin;

      pginc = ((listwidth <= CHAR_WIDTH_GUESS) ?
	       1 : (listwidth - CHAR_WIDTH_GUESS));
      if (pginc > lw->core.width)
	pginc = 1;

      nav_data.value.x = lw->list.hOrigin;
      nav_data.minimum.x = lw->list.hmin;
      nav_data.maximum.x = lw->list.hmax;
      nav_data.slider_size.x = lw->list.hExtent;
      nav_data.increment.x = CHAR_WIDTH_GUESS;
      nav_data.page_increment.x = pginc;

      nav_data.dimMask = NavigDimensionX;
      nav_data.valueMask = NavValue|NavMinimum|NavMaximum|
	NavSliderSize|NavIncrement|NavPageIncrement;
      _XmSFUpdateNavigatorsValue(XtParent((Widget)lw), &nav_data, True);
    }
  else if (XtIsManaged((Widget) lw->list.hScrollBar))
    {
      nav_data.value.x = 0;
      nav_data.minimum.x = 0;
      nav_data.maximum.x = 1;
      nav_data.slider_size.x = 1;
      nav_data.increment.x = 1;
      nav_data.page_increment.x = 1;

      nav_data.dimMask = NavigDimensionX;
      nav_data.valueMask = NavValue|NavMinimum|NavMaximum|
	NavSliderSize|NavIncrement|NavPageIncrement;
      _XmSFUpdateNavigatorsValue(XtParent((Widget)lw), &nav_data, True);
    }

  lw->list.FromSetSB = FALSE;

  return (was_managed != is_managed);
}

/************************************************************************
 *									*
 * SetNewSize - see if we need a new size.  If so, do it.  If the	*
 * current item count is different from the desired count, calc a new	*
 * height and width.  Else just look at the width and change if needed.	*
 *                                                                      *
 * NOTE: THIS CAN ONLY BE CALLED FROM THE API ROUTINES, SINCE IT USES   *
 * SETVALUES.                                                           *
 *									*
 ************************************************************************/

static void
SetNewSize(XmListWidget lw,
	   Boolean reset_max_width,
	   Boolean reset_max_height,
	   Dimension old_max_height)
{
  Dimension width, height;
  Boolean resized = FALSE;

  lw->list.FromSetNewSize = TRUE;
  SetDefaultSize(lw, &width, &height, reset_max_width, reset_max_height);

  if (lw->list.SizePolicy == XmCONSTANT)
    width = lw->core.width;

  if ((width != lw->core.width) ||
      (height != lw->core.height))
    {
      Arg args[2];
      Cardinal nargs = 0;
      unsigned char units = lw->primitive.unit_type;
      Dimension old_width = XtWidth((Widget)lw);
      Dimension old_height = XtHeight((Widget)lw);

      lw->primitive.unit_type = XmPIXELS;
      XtSetArg(args[nargs], XmNwidth, (XtArgVal)width), nargs++;
      XtSetArg(args[nargs], XmNheight, (XtArgVal)height), nargs++;
      assert(nargs <= XtNumber(args));
      XtSetValues((Widget) lw, args, nargs);
      lw->primitive.unit_type = units;

      resized = ((old_width != XtWidth((Widget)lw)) ||
		 (old_height != XtHeight((Widget)lw)));
    }

  /* CR 9488: Redraw and update visible item count if resize fails. */
  if (!resized && (old_max_height != lw->list.MaxItemHeight))
    {
      lw->list.visibleItemCount = ComputeVizCount(lw);
      CleanUpList(lw, True);
      DrawList(lw, NULL, True);
    }

  lw->list.FromSetNewSize = FALSE;
}

/************************************************************************
 *									*
 * ResetExtents - recalculate the cumulative extents of the list items.	*
 * Called when the font or spacing changes.  Computing individual	*
 * dimenensions is just as expensive as computing both at once.		*
 *									*
 ************************************************************************/

static void
ResetExtents(XmListWidget lw,
	     Boolean recache_extents)
{
  int i;
  Dimension maxheight = 0;
  Dimension maxwidth = 0;

  if (!lw->list.InternalList || !lw->list.itemCount)
    return;

  for (i = 0; i < lw->list.itemCount; i++)
    {
      ElementPtr item = lw->list.InternalList[i];

      if (recache_extents)
	XmStringExtent(lw->list.font, lw->list.items[i],
		       &item->width, &item->height);

      ASSIGN_MAX(maxheight, item->height);
      ASSIGN_MAX(maxwidth, item->width);
    }

  lw->list.MaxItemHeight = maxheight;
  lw->list.MaxWidth = maxwidth;
}

/************************************************************************
 *									*
 * Item/Element Manupulation routines					*
 *									*
 ************************************************************************/

/* BEGIN OSF Fix CR 4656 */
/************************************************************************
 *									*
 * FixStartEnd - reset the (Old)StartItem and (Old)EndItem		*
 *	instance variables after a deletion based on the postion of 	*
 * 	the item deleted.						*
 *									*
 ************************************************************************/

static void
FixStartEnd(XmListWidget lw,
	    int pos,
	    int count,
	    int *start,
	    int *end)
{
  /* Range selections can leave start and end "backwards". */
  if (*start > *end)
    {
      int save = *start;
      *start = *end;
      *end = save;
    }

  if (*start < 0)
    *start = 0;
  if (*end >= lw->list.itemCount)
    *end = lw->list.itemCount - 1;

  /* No overlap, before the deleted range. */
  if (*end < pos)
    return;

  /* No overlap, after the deleted range. */
  if (*start >= (pos + count))
    {
      (*start) -= count;
      (*end) -= count;
      return;
    }

  /* Fixup the starting position. */
  if (*start > pos)
    *start = pos;

  /* Fixup the end position. */
  if (*end < (pos + count))
    *end = pos - 1;
  else
    *end -= count;

  /* Normalize empty selections. */
  if (*start > *end)
    *start = *end = 0;
}
/* END OSF Fix CR 4656 */

/***************************************************************************
 *									   *
 * AddInternalElements()						   *
 *									   *
 * Takes elements from the items list and adds them to the internal list.  *
 * NOTE: This code relies on the caller to insure that the list.itemCount  *
 * field reflects the new total size of the list.			   *
 *									   *
 ***************************************************************************/

static int
AddInternalElements(XmListWidget lw,
		    XmString *items,
		    int nitems,
		    int position,
		    Boolean selectable)
{
  int pos;
  ElementPtr new_el;
  int i;
  int nsel = 0;

  if (nitems <= 0)
    return nsel;

  /* CR 9663: Discard default width when we get real items. */
  if (lw->list.LastItem == 0)
    lw->list.MaxWidth = 0;

  if (position)
    pos = position - 1;
  else
    pos = lw->list.LastItem;

  lw->list.InternalList = (ElementPtr *)
    XtRealloc((char *)lw->list.InternalList,
	      (sizeof(Element *) * lw->list.itemCount));

  /* Make room in the InternalList for the new items. */
  if (pos < lw->list.LastItem)
    memmove((char*)(lw->list.InternalList + pos + nitems),
	    (char*)(lw->list.InternalList + pos),
	    (lw->list.LastItem - pos) * sizeof(ElementPtr));

  for (i = 0; i < nitems; i++)
    {
      new_el = (ElementPtr)XtMalloc(sizeof(Element));

      /* Store an alias for string in the internal table. */
      assert(items[i] == lw->list.items[pos]);
      new_el->length = UNKNOWN_LENGTH;
      XmStringExtent(lw->list.font, items[i], &new_el->width, &new_el->height);
      ASSIGN_MAX(lw->list.MaxWidth, new_el->width);
      ASSIGN_MAX(lw->list.MaxItemHeight, new_el->height);

      new_el->selected = (selectable && OnSelectedList(lw, items[i], pos));
      new_el->last_selected = new_el->selected;
      new_el->LastTimeDrawn = !new_el->selected;
      if (new_el->selected)
	nsel++;

      new_el->first_char = 0;

      lw->list.InternalList[pos] = new_el;
      pos++;
    }

  lw->list.LastItem += nitems;

  return nsel;
}

/***************************************************************************
 *									   *
 * DeleteInternalElements()				       		   *
 *									   *
 * Deletes elements from the internal list.  If position is 0, we look     *
 * for the specified string; if the string is NULL we look for specified   *
 * position.								   *
 * NOTE: This code relies on the caller to insure that the list.itemCount  *
 * field reflects the new total size of the list.			   *
 *									   *
 * ALSO NOTE that this function is sometimes called just after             *
 *    DeleteItems.   This function expects position to be ONE              *
 *    based, while the other expects position to be ZERO based.            *
 *                                                                         *
 ***************************************************************************/

static int
DeleteInternalElements(XmListWidget lw,
		       XmString string,
		       int position,
		       int count)
{
  Element *item;
  int curpos;
  int dsel = 0;
  int i;

  if (!position && string)
    position = ItemNumber(lw, string);
  if (!position)
    {
      XmeWarning((Widget) lw, ListMessage8);
      return 0;
    }

  curpos = position - 1;

  for (i = 0; i < count; i++)
    {
      item = lw->list.InternalList[curpos + i];
      if (item->selected)
	dsel--;
      XtFree((char *)item);
    }

  /* If we didn't delete the end of the list repack it. */
  if (curpos < lw->list.itemCount)
    memmove((char*)(lw->list.InternalList + curpos),
	    (char*)(lw->list.InternalList + curpos + count),
	    (lw->list.itemCount - curpos) * sizeof(ElementPtr));

  lw->list.LastItem -= count;

  /* BEGIN OSF Fix CR 4656 */
  /* Fix selection delimiters. */
  FixStartEnd(lw, curpos, count, &lw->list.StartItem, &lw->list.EndItem);
  FixStartEnd(lw, curpos, count, &lw->list.OldStartItem, &lw->list.OldEndItem);
  /* END OSF Fix CR 4656 */

  if (lw->list.itemCount)
    {
      lw->list.InternalList = (ElementPtr *)
	XtRealloc((char *) lw->list.InternalList,
		  (sizeof(Element *) * lw->list.itemCount));
    }
  else
    {
      XtFree((char*) lw->list.InternalList);
      lw->list.InternalList = NULL;
    }

  return dsel;
}

/***************************************************************************
 *									   *
 * DeleteInternalElementPositions                                          *
 *                                                                         *
 ***************************************************************************/

static int
DeleteInternalElementPositions(XmListWidget  lw,
			       int          *position_list,
			       int           position_count,
			       int           oldItemCount)
{
  ElementPtr  ptr;
  int         ix;
  int         jx;
  int         item_pos;
  Boolean     reset_width = FALSE;
  Boolean     reset_height = FALSE;
  int	      nsel = 0;

  /* See what caller can do to flag errors, if necessary,
   * when this information is not present. */
  if (!position_list || !position_count)
    return nsel;

  /* Prepare ourselves to do a series of deletes.   Scan the position_list
   * to free deleted positions.   Set each freed position to NULL.  Do not
   * try to refree already deleted positions.
   *
   * Any invalid position must be caught by the calling routine.   These
   * positions have to be marked with a -1 to be ignored.
   *
   * This function can work in tandem with DeleteItemPositions which
   * will reset list.itemCount (just as DeleteItem does).   This is
   * why we must have oldItemCount passed to us.
   */
  for (ix = 0; ix < position_count; ix++)
    {
      item_pos = position_list[ ix ] - 1;
      if (item_pos >= 0 && item_pos < oldItemCount)
	{
	  if ((ptr = lw->list.InternalList[item_pos]) != NULL)
	    {
	      reset_width |= (ptr->width >= lw->list.MaxWidth);
	      reset_height |= (ptr->height >= lw->list.MaxItemHeight);
	      if (ptr->selected)
		nsel--;

	      XtFree((char*) ptr);
	      lw->list.InternalList[item_pos] = NULL;
	      lw->list.LastItem--;

	      /* BEGIN OSF Fix CR 4656 */
	      /* Fix selection delimiters. */
	      FixStartEnd(lw,
			  item_pos, 1, &lw->list.StartItem, &lw->list.EndItem);

	      /* Fix old selection delimiters. */
	      FixStartEnd(lw, item_pos, 1,
			  &lw->list.OldStartItem,
			  &lw->list.OldEndItem);
	      /* END OSF Fix CR 4656 */
	    }
        }
    }

  /*
   * The fixups above may leave a non-existent selection if multiple
   * selected items are deleted from the end of the list.
   */
  if (oldItemCount > lw->list.itemCount)
    {
      FixStartEnd(lw, lw->list.itemCount, oldItemCount - lw->list.itemCount,
		  &lw->list.StartItem, &lw->list.EndItem);
      FixStartEnd(lw, lw->list.itemCount, oldItemCount - lw->list.itemCount,
		  &lw->list.OldStartItem, &lw->list.OldEndItem);
    }

  /* Re-pack InternalList in place. */
  jx = 0;
  for (ix = 0; ix < oldItemCount; ix++)
    {
      if (lw->list.InternalList[ ix ] != NULL)
        {
	  lw->list.InternalList[ jx ] = lw->list.InternalList[ ix ];
	  jx++;
        }
    }

  if (lw->list.itemCount)
    {
      lw->list.InternalList = (ElementPtr*)
	XtRealloc((char*) lw->list.InternalList,
		  (sizeof(ElementPtr) * lw->list.itemCount));
    }
  else
    {
      XtFree((char*) lw->list.InternalList);
      lw->list.InternalList = NULL;
    }

  /* The actual maximum width and height may not have changed. */
  if (reset_width && lw->list.itemCount &&
      (lw->list.InternalList[0]->width >= lw->list.MaxWidth))
    reset_width = FALSE;
  if (reset_height && lw->list.itemCount &&
      (lw->list.InternalList[0]->height >= lw->list.MaxItemHeight))
    reset_height = FALSE;
  if (reset_width || reset_height)
    ResetExtents(lw, False);

  return nsel;
}

/***************************************************************************
 *									   *
 * ReplaceInternalElement(lw, position, selected)                          *
 *									   *
 * Replaces the existing internal item with the specified new one. The new *
 * item is constructed by looking at the list.items - this means that the  *
 * external one has to be replaced first! Note that this does not reset    *
 * the CumHeight fields - it's up to the caller to do that.                *
 *									   *
 * Returns the change in the number of selected positions.		   *
 *									   *
 ***************************************************************************/

static int
ReplaceInternalElement(XmListWidget lw,
		       int position,
		       Boolean selectable)
{
  int	   curpos = position - 1;
  Element *item = lw->list.InternalList[curpos];
  int      dsel = (item->selected ? -1 : 0);
  XmString name = lw->list.items[curpos];

  /* The old name is an alias for an entry in the items list. */

  item->first_char = 0;
  item->length = UNKNOWN_LENGTH;
  XmStringExtent(lw->list.font, name, &item->width, &item->height);
  item->selected = (selectable && OnSelectedList(lw, name, curpos));
  item->last_selected = item->selected;
  item->LastTimeDrawn = !item->selected;

  ASSIGN_MAX(lw->list.MaxWidth, item->width);
  ASSIGN_MAX(lw->list.MaxItemHeight, item->height);

  dsel += (item->selected ? 1 : 0);
  return dsel;
}

/************************************************************************
 *									*
 * AddItems - add items to the item list at the specified position	*
 *									*
 ************************************************************************/

static void
AddItems(XmListWidget lw,
	 XmString *items,
	 int nitems,
	 int pos)
{
  int i;
  int TotalItems = lw->list.itemCount + nitems;

  lw->list.items = (XmString *)
    XtRealloc((char *) lw->list.items, (sizeof(XmString) * (TotalItems)));

  /* Make a gap in the array for the new items. */
  if (pos < lw->list.itemCount)
    memmove((char*) (lw->list.items + pos + nitems),
	    (char*) (lw->list.items + pos),
	    (lw->list.itemCount - pos) * sizeof(XmString));

  /* Insert the new items into the array. */
  for (i = 0; i < nitems; i++)
    lw->list.items[pos + i] = XmStringCopy(items[i]);

  lw->list.itemCount = TotalItems;
}

/************************************************************************
 *									*
 * DeleteItems - delete items from the item list.			*
 *									*
 *    Note that this function is sometimes called just before           *
 *    DeleteInternalElements.  This function expects position to be     *
 *    ZERO based, while the other expects position to be ONE based.     *
 *                                                                      *
 * ON DELETE, DO WE UPDATE MAXWIDTH??					*
 *                                                                      *
 ************************************************************************/

static void
DeleteItems(XmListWidget lw,
	    int nitems,
	    int pos)
{
  int i;
  int TotalItems;

  if ((lw->list.itemCount < 1) || (nitems <= 0))
    return;

  TotalItems = lw->list.itemCount - nitems;

  for (i = 0; i < nitems; i++)
    XmStringFree(lw->list.items[pos + i]);

  if (pos < TotalItems)
    memmove((char*) (lw->list.items + pos),
	    (char*) (lw->list.items + pos + nitems),
	    (TotalItems - pos) * sizeof(XmString));

  if (TotalItems)
    {
	lw->list.items = (XmString *) XtRealloc((char *) lw->list.items,
					TotalItems * sizeof(XmString));
    }
  else
    {
	/* Null out the list pointer, if we have deleted the last item. */
	XtFree((char *) lw->list.items);
	lw->list.items = NULL;
     }

  lw->list.itemCount = TotalItems;
}

/************************************************************************
 *									*
 * DeleteItemPositions                                                  *
 *									*
 ************************************************************************/

static void
DeleteItemPositions(XmListWidget  lw,
		    int          *position_list,
		    int           position_count,
		    Boolean	  track_kbd)
{
  int 	     TotalItems;
  int	     item_pos;
  int        ix;
  int        jx;
  XmString   item;

  if (lw->list.itemCount < 1)
    return;

  /* Prepare ourselves to do a series of deletes.   Scan the position_list
   * to free deleted positions.   Set each freed position to NULL.  Do not
   * try to refree already deleted positions.
   *
   * Any invalid position must be caught by the calling routine.   These
   * positions have to be marked with a -1 to be ignored.
   *
   * Re-pack "items" in place ignoring the previously freed positions.
   */
  TotalItems = lw->list.itemCount;

  for (ix = 0; ix < position_count; ix++)
    {
      item_pos = position_list[ ix ] - 1;
      if (item_pos >= 0 && item_pos < lw->list.itemCount)
	{
	  item = lw->list.items[item_pos];
	  if (item)
	    {
	      XmStringFree(item);
	      lw->list.items[item_pos] = NULL;
	      TotalItems--;

	      /* CR 9630:  XmListDeletePos and XmListDeletePositions */
	      /*	track the keyboard location cursor differently. */
	      if (track_kbd && (item_pos <= lw->list.CurrentKbdItem))
		{
		  lw->list.CurrentKbdItem--;
		  ASSIGN_MAX(lw->list.CurrentKbdItem, 0);
		  if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
		      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
		    lw->list.LastHLItem = lw->list.CurrentKbdItem;
		}
	    }
        }
    }

  jx = 0;
  for (ix = 0; ix < lw->list.itemCount; ix++)
    {
      if (lw->list.items[ ix ] != NULL)
        {
	  lw->list.items[ jx ] = lw->list.items[ ix ];
	  jx++;
        }
    }

  if (TotalItems)
    {
	lw->list.items = (XmString *) XtRealloc((char *) lw->list.items,
					TotalItems * sizeof(XmString));
    }
  else
    {
	/* Null out the list pointer, if we have deleted the last item. */
	XtFree((char *) lw->list.items);
	lw->list.items = NULL;
     }

  lw->list.itemCount = TotalItems;
}

/************************************************************************
 *									*
 * ReplaceItem - Replace an item at the specified position	        *
 *									*
 ************************************************************************/
static void
ReplaceItem(XmListWidget lw,
	    XmString item,
	    int pos)
{
  int i;
  pos--;

  XmStringFree(lw->list.items[pos]);
  lw->list.items[pos] = XmStringCopy(item);
  /*Selected items should be replaced also*/
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);

  for(i=0; i<lw->list.selectedItemCount; i++)
  {
      if(lw->list.selectedPositions[i]==pos+1) {
         XmStringFree(lw->list.selectedItems[i]);
	 lw->list.selectedItems[i]=XmStringCopy(item);
      }
  }
}


/***************************************************************************
 *									   *
 * ItemNumber - returns the item number of the specified item in the 	   *
 * external item list.							   *
 *									   *
 ***************************************************************************/

static int
ItemNumber(XmListWidget lw,
	   XmString item)
{
  int i;

  for (i = 0; i < lw->list.itemCount; i++)
    if (XmStringCompare(lw->list.items[i], item))
      return i + 1;

  return 0;
}

/***************************************************************************
 *									   *
 * ItemExists - returns TRUE if the specified item matches an item in the  *
 * List item list.							   *
 *									   *
 ***************************************************************************/

static int
ItemExists(XmListWidget lw,
	   XmString item)
{
  int i;

  for (i = 0; i < lw->list.itemCount; i++)
    if ((XmStringCompare(lw->list.items[i], item)))
      return TRUE;

  return FALSE;
}

/************************************************************************
 *									*
 * OnSelectedList - Returns TRUE if the given item is on the selected	*
 * items list, or, if selectedItems is empty, if the item is on the     *
 * selectedPositions list.						*
 *									*
 ************************************************************************/

static Boolean
OnSelectedList(
        XmListWidget lw,
        XmString item,
	int intern_pos)
{
  int i;

  /* Use selectedItems if applicable, else use selectedPositions */
  if (lw->list.selectedItems && (lw->list.selectedItemCount > 0))
    {
      for (i = 0; i < lw->list.selectedItemCount; i++)
	if (XmStringCompare(lw->list.selectedItems[i], item))
	  return TRUE;
    }
  else if ((lw->list.selectedPositions != NULL) &&
	   (lw->list.selectedPositionCount > 0))
    {
      for (i=0; i < lw->list.selectedPositionCount; i++)
	if (lw->list.selectedPositions[i] == (intern_pos + 1))
	  return TRUE;
    }

  return FALSE;
}

/************************************************************************
 *									*
 * CopyItems - Copy the item list into our space.			*
 *									*
 ************************************************************************/

static void
CopyItems(XmListWidget lw)
{
  int i;
  XmString *il;

  if (lw->list.items && lw->list.itemCount)
    {
      il = (XmString *)XtMalloc(sizeof(XmString) * (lw->list.itemCount));
      for (i = 0; i < lw->list.itemCount; i++)
	il[i] = XmStringCopy(lw->list.items[i]);

      lw->list.items = il;
    }
}

/************************************************************************
 *									*
 * CopySelectedItems - Copy the selected item list into our space.	*
 *									*
 ************************************************************************/

static void
CopySelectedItems(XmListWidget lw)
{
  int i;
  XmString *sl;


  if (lw->list.selectedItems && lw->list.selectedItemCount)
    {
      sl = (XmString *) XtMalloc(sizeof(XmString) *
				 (lw->list.selectedItemCount));
      for (i = 0; i < lw->list.selectedItemCount; i++)
	sl[i] = XmStringCopy(lw->list.selectedItems[i]);

      lw->list.selectedItems = sl;
    }
}

/************************************************************************
 *									*
 * CopySelectedPositions - Copy the selected position list.		*
 *									*
 ************************************************************************/

static void
CopySelectedPositions(XmListWidget lw)
{
  if (lw->list.selectedPositions && lw->list.selectedPositionCount)
    {
      int size = sizeof(int) * lw->list.selectedPositionCount;
      int *sl = (int *) XtMalloc(size);

      memcpy((char*)sl, (char*)lw->list.selectedPositions, size);
      lw->list.selectedPositions = sl;
    }
}

/************************************************************************
 *									*
 * ClearItemList - delete all elements from the item list, and		*
 * free the space associated with it.					*
 *									*
 ************************************************************************/

static void
ClearItemList(XmListWidget lw)
{
  int i;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  for (i = 0; i < lw->list.itemCount; i++)
    XmStringFree(lw->list.items[i]);
  XtFree((char *) lw->list.items);

  lw->list.itemCount = 0;
  lw->list.items = NULL;
  lw->list.LastItem = 0;
  lw->list.LastHLItem = 0;
  lw->list.top_position = 0;
  lw->list.CurrentKbdItem = 0;
  lw->list.XOrigin = 0;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }
}

/************************************************************************
 *									*
 * ClearSelectedPositions - delete all elements from the selected       *
 *	   positions list AND free the space associated with it.	*
 *									*
 ************************************************************************/

static void
ClearSelectedPositions(XmListWidget lw)
{
  if (!(lw->list.selectedPositions && lw->list.selectedPositionCount))
    return;

  XtFree((char*) lw->list.selectedPositions);

  lw->list.selectedPositionCount = 0;
  lw->list.selectedPositions = NULL;
}

/************************************************************************
 *									*
 * ClearSelectedList - delete all elements from the selected list       *
 * 			AND free the space associated with it.	        *
 *									*
 ************************************************************************/

static void
ClearSelectedList(
        XmListWidget lw)
{
  int i;

  if (!(lw->list.selectedItems && lw->list.selectedItemCount))
    return;

  for (i = 0; i < lw->list.selectedItemCount; i++)
    XmStringFree(lw->list.selectedItems[i]);
  XtFree((char *) lw->list.selectedItems);

  lw->list.selectedItemCount = 0;
  lw->list.selectedItems = NULL;
}

/************************************************************************
 *									*
 *  BuildSelectedList - traverse the element list and construct a list	*
 *		       of selected elements and indices.		*
 *									*
 *  NOTE: Must be called in tandem with and *AFTER* ClearSelectedList.	*
 *        Otherwise you have a memory leak...				*
 *									*
 ************************************************************************/

static void
BuildSelectedList(XmListWidget lw,
		  Boolean      commit)
{
  int i, j, count;
  Boolean sel;

  count = lw->list.itemCount;
  for (i = 0, j = 0; i < count; i++)
    {
      sel = lw->list.InternalList[i]->selected;
      if (sel)
	j++;

      /* Commit the selection */
      if (commit)
	lw->list.InternalList[i]->last_selected = sel;
    }

  lw->list.selectedItemCount = j;
  lw->list.selectedItems = NULL;
  if (j == 0)
    return;
  lw->list.selectedItems = (XmString *)XtMalloc(sizeof(XmString) * j);

  for (i = 0, j = 0; i < count; i++)
    {
      if (lw->list.InternalList[i]->selected)
	{
	  lw->list.selectedItems[j] = XmStringCopy(lw->list.items[i]);
	  j++;
        }
    }
}

/************************************************************************
 *									*
 *  BuildSelectedPositions - traverse the element list and construct    *
 *			     a list of selected positions.		*
 *									*
 *  NOTE: Must be called in tandem with and *AFTER* 		        *
 *        ClearSelectedPositions. Otherwise you have a memory leak...   *
 *									*
 ************************************************************************/

static void
BuildSelectedPositions(XmListWidget lw,
		       int count)
{
  int pos;
  int nsel = count;
  int nitems = lw->list.itemCount;

  if (nsel == RECOUNT_SELECTION)
    {
      for (pos = 0, nsel = 0; pos < nitems; pos++)
	if (lw->list.InternalList[pos]->selected)
	  nsel++;
    }

  lw->list.selectedPositionCount = nsel;
  if (nsel == 0)
    {
      lw->list.selectedPositions = NULL;
    }
  else
    {
      lw->list.selectedPositions = (int *) XtMalloc(sizeof(int) * nsel);

      for (pos = 0, nsel = 0; pos < nitems; pos++)
	{
	  if (lw->list.InternalList[pos]->selected)
	    {
	      lw->list.selectedPositions[nsel] = pos + 1;
	      nsel++;
	      if (nsel >= lw->list.selectedPositionCount)
		break;
	    }
	}
    }
}

/************************************************************************
 *									*
 *  UpdateSelectedList - Build a new selected list.			*
 *									*
 ************************************************************************/

static void
UpdateSelectedList(XmListWidget lw,
		   Boolean rebuild)
{
  if (rebuild)
    {
      ClearSelectedList(lw);
      BuildSelectedList(lw, TRUE);
    }

  /* Do we want to do this conditional on the selected item count
   * not being 0 or should we always take primary? */
  if ((lw->list.selectedItemCount > 0) &&
      (lw->list.PrimaryOwnership != XmOWN_NEVER))
    {
      /* We can take ownership of the primary if:
       *   1. ownership is XmOWN_ALWAYS
       *   2. ownership is XmOWN_MULTIPLE and more than one
       *      item is selected
       *   3. ownership is XmOWN_POSSIBLE_MULTIPLE and selection
       *      policy is EXTENDED
       *   4. ownership is XmOWN_POSSIBLE_MULTIPLE and selection
       *      policy is MULTIPLE
       */
      if (/* 1. */ (lw->list.PrimaryOwnership == XmOWN_ALWAYS) ||
	  /* 2. */ (lw->list.PrimaryOwnership == XmOWN_MULTIPLE &&
		    lw->list.selectedItemCount > 1) ||
	  /* 3. */ (lw->list.PrimaryOwnership == XmOWN_POSSIBLE_MULTIPLE &&
		    lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	  /* 4. */ (lw->list.PrimaryOwnership == XmOWN_POSSIBLE_MULTIPLE &&
		    lw->list.SelectionPolicy == XmMULTIPLE_SELECT))
	{
	  XmePrimarySource((Widget) lw, 0);
        }
    }
}

/***************************************************************************
 *									   *
 * UpdateSelectedPositions - Build a new selected positions list.	   *
 *									   *
 ***************************************************************************/

static void
UpdateSelectedPositions(XmListWidget lw,
			int count)
{
  ClearSelectedPositions(lw);
  BuildSelectedPositions(lw, count);
}

/***************************************************************************
 *									   *
 * ListSelectionChanged - a utility function that determines whether the   *
 * selection before the last selection activity and the current selection  *
 * differ.								   *
 *									   *
 ***************************************************************************/

static Boolean
ListSelectionChanged(XmListWidget w)
{
  int item;
/*
  int startitem;
  int enditem;
*/

  /* We can't simply compare the start and oldstart and compare
   * the end and oldend to see whether the boundaries have changed
   * because we don't know the state of all the items between
   * the boundaries. An easy way to do this is to form a union of
   * the old and current selection and loop through looking for
   * any selection state changes. So first find the boundaries
   * of the union ... */
/*
  startitem = MIN(w->list.StartItem, w->list.OldStartItem);
  enditem = MAX(w->list.EndItem, w->list.OldEndItem);
*/

  /* Search through the union of the old selection and the
   * new selection. If any element has had a change of
   * selection state, then the selection has changed. [Note: we
   * are actually searching through the whole list right now
   * but this should be optimized at some point.] */
  for (item = 0; item < w->list.itemCount; ++item)
    {
      if (w->list.InternalList[item]->selected !=
	  w->list.InternalList[item]->last_selected)
	{
	  return True;
	}
    }

  /* Everything was the same, so return False. */
  return False;
}


/************************************************************************
 *									*
 *             Event Handlers for the various selection modes		*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 * WhichItem - Figure out which element we are on. 			*
 *									*
 ************************************************************************/

static int
WhichItem(XmListWidget w,
	  Position EventY)
{
  XmListWidget lw = w;
  Position y = EventY;
  int item, lines;

  if (lw->list.Traversing && lw->list.KbdSelection)
    return lw->list.CurrentKbdItem;

  /* BEGIN OSF Fix CR 5081 */
  if (!lw->list.items)
    return -1;

  if (y <= (Position)(lw->list.BaseY - lw->list.HighlightThickness))
    { /* END OSF Fix CR 5081 */
      if (lw->list.top_position)
	return(-1);
      else
	return(0);
    }
  if ((Dimension) y > lw->core.height)
    {
      if ((lw->list.top_position + lw->list.visibleItemCount) >=
	  lw->list.itemCount)
	return (lw->list.itemCount - 1);
    }
  if (y >= (Position)(lw->core.height - lw->list.BaseY))
    return (lw->list.itemCount + 1);

  if ((int) (lw->list.MaxItemHeight + lw->list.spacing) == 0)
    return(-1);

  /* Calculate the line offset directly, since lines are fixed height. */
  lines = (((int) (y + lw->list.spacing) -
	    (int) (lw->list.BaseY + lw->list.HighlightThickness + 1)) /
	   (int) (lw->list.MaxItemHeight + lw->list.spacing));

  if (lines <= 0)
    item = lw->list.top_position;
  else if (lw->list.top_position + lines < lw->list.itemCount)
    item = lw->list.top_position + lines;
  else
    item = lw->list.itemCount;

  return item;
}

/************************************************************************
 *									*
 * SelectRange - Select/deselect the range between start and end.       *
 *              This does not set the last_selected flag.               *
 *									*
 ************************************************************************/

static void
SelectRange(XmListWidget lw,
	    int first,
	    int last,
	    Boolean select)
{
  int start, end;

  if (first <= last)
    {
      start = first;
      end = last;
    }
  else
    {
      start = last;
      end = first;
    }

  if (start < 0)
    start = 0;
  if (end >= lw->list.itemCount)
    end = lw->list.itemCount - 1;

  for (; start <= end; start++)
    {
      lw->list.InternalList[start]->selected = select;
      DrawItem((Widget) lw, start);
    }
}

/************************************************************************
 *									*
 * RestoreRange - Restore the range between start and end.              *
 *									*
 ************************************************************************/

static void
RestoreRange(XmListWidget lw,
	     int first,
	     int last,
	     Boolean dostart)
{
  int tmp, start, end;
  start = first; end = last;

  if (start > end)
    {
      tmp = start;
      start = end;
      end = tmp;
    }

  if (start < 0)
    start = 0;
  if (end >= lw->list.itemCount)
    end = lw->list.itemCount - 1;

  if (start < 0)
    start = 0;
  if (end >= lw->list.itemCount)
    end = lw->list.itemCount - 1;

  tmp = lw->list.StartItem;
  for (; start <= end; start++)
    if ((start != tmp) || dostart)
      {
	lw->list.InternalList[start]->selected =
	  lw->list.InternalList[start]->last_selected;
	DrawItem((Widget) lw, start);
      }
}

/************************************************************************
 *                                                                      *
 * ArrangeRange - This does all the necessary magic for movement in     *
 * extended selection mode.  This code handles all the various cases    *
 * for relationships between the start, end and current item, restoring *
 * as needed, and selecting the appropriate range.  This is called in   *
 * both the mouse and keyboard cases.                                   *
 *                                                                      *
 ************************************************************************/

static void
ArrangeRange(XmListWidget lw,
	     int item)
{
  int start = lw->list.StartItem;
  int end = lw->list.EndItem;
  int i = item;
  Boolean set = lw->list.InternalList[start]->selected;

  if (start < end)
    {
      if (i > end)
	SelectRange(lw, end, i, set);
      else if ((i < end) && (i >= start))
	{
	  /* CR 5676: Undo extended toggle drags properly. */
	  if (!set || (lw->list.Event & CTRLDOWN))
	    RestoreRange(lw, i + 1, end, FALSE);
	  else
	    SelectRange(lw, i + 1, end, FALSE);
	}
      else if (i <= start)
	{
	  /* CR 5676: Undo extended toggle drags properly. */
	  if (!set || (lw->list.Event & CTRLDOWN))
	    RestoreRange(lw, start, end, FALSE);
	  else
	    SelectRange(lw, start, end, FALSE);
	  SelectRange(lw, i, start, set);
	}
    }
  else if (start > end)
    {
      if (i <= end)
	SelectRange(lw, i, end, set);
      else if ((i > end) && (i <= start))
	{
	  /* CR 5676: Undo extended toggle drags properly. */
	  if (!set || (lw->list.Event & CTRLDOWN))
	    RestoreRange(lw, end, i - 1, FALSE);
	  else
	    SelectRange(lw, end, i - 1, FALSE);
	}
      else if (i >= start)
	{
	  /* CR 5676: Undo extended toggle drags properly. */
	  if (!set || (lw->list.Event & CTRLDOWN))
	    RestoreRange(lw, end, start, FALSE);
	  else
	    SelectRange(lw, end, start, FALSE);
	  SelectRange(lw, start, i, set);
	}
    }
  else
    SelectRange(lw, start, i, set);
}

/************************************************************************
 *									*
 * HandleNewItem - called when a new item is selected in browse or	*
 * extended select mode.  This does the deselection of previous items	*
 * and the autoselection, if enabled.					*
 *									*
 ************************************************************************/
static void
HandleNewItem(XmListWidget lw,
	      int item,
	      int olditem)
{
  int dir;

  if (lw->list.LastHLItem == item)
    return;

  if (item < 0 || item >= lw->list.itemCount)
    return;

  if (item < 0 || item >= lw->list.itemCount)
    return;

  switch(lw->list.SelectionPolicy)
    {
    case XmBROWSE_SELECT:
      lw->list.InternalList[lw->list.LastHLItem]->selected = FALSE;
      if (lw->list.LastHLItem != lw->list.CurrentKbdItem)
	lw->list.InternalList[lw->list.LastHLItem]->last_selected = FALSE;
      DrawItem((Widget) lw, lw->list.LastHLItem);
      lw->list.InternalList[item]->selected = TRUE;
      /* lw->list.InternalList[item]->last_selected = TRUE; */
      DrawItem((Widget) lw, item);
      lw->list.LastHLItem = item;
      lw->list.StartItem = item;
      lw->list.EndItem = item;
      if (lw->list.AutoSelect != XmNO_AUTO_SELECT)
	{
	  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  ClickElement(lw, NULL, FALSE);
	  lw->list.CurrentKbdItem = item;
	  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	    {
	      XPoint xmim_point;

	      GetPreeditPosition(lw, &xmim_point);
	      XmImVaSetValues((Widget)lw, XmNspotLocation,
			      &xmim_point, NULL);
	    }
	  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
	}
      break;

    case XmEXTENDED_SELECT:
      /* BEGIN OSF Fix CR 5954 */
      dir = (lw->list.LastHLItem < item) ? 1 : -1;
      while (lw->list.LastHLItem != item &&
	     lw->list.StartItem >= 0 && lw->list.EndItem >= 0 &&
	     lw->list.StartItem < lw->list.itemCount &&
	     lw->list.EndItem < lw->list.itemCount)
	{
	  lw->list.LastHLItem += dir;

	  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
	      (lw->list.DidSelection))
	    ClickElement(lw, NULL, FALSE);

	  ArrangeRange(lw, item);

	  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
	      (!lw->list.DidSelection))
	    ClickElement(lw, NULL, FALSE);

	  lw->list.EndItem += dir;
	}
      lw->list.DidSelection = TRUE;
      break;
    }
  /* END OSF Fix CR 5954 */
}

/************************************************************************
 *									*
 * HandleExtendedItem - called when a new item is selected via the      *
 * keyboard in  extended select mode.  This does the deselection of     *
 * previous items and handles some of the add mode actions.             *
 *									*
 ************************************************************************/

static void
HandleExtendedItem(XmListWidget lw,
		   int item)
{
  Boolean set;
  int i, start, end;

  if (lw->list.LastHLItem == item) return;

  if (item < 0 || item >= lw->list.itemCount)
    return;

  /* First the non-addmode case */
  if (lw->list.SelectionMode == XmNORMAL_MODE)
    {
      if (!(lw->list.Event & SHIFTDOWN))    /* And not shifted */
        {
	  lw->list.StartItem = item;
	  lw->list.EndItem = item;
	  lw->list.LastHLItem = item;
	  for (i = 0; i < lw->list.selectedPositionCount; i++)
	    {
	      int pos = lw->list.selectedPositions[i] - 1;
	      if (pos != item)
		{
		  lw->list.InternalList[pos]->last_selected =
		    lw->list.InternalList[pos]->selected;
		  lw->list.InternalList[pos]->selected = FALSE;
		  DrawItem((Widget) lw, pos);
		}
	    }
	  lw->list.InternalList[item]->last_selected =
	    lw->list.InternalList[item]->selected;
	  lw->list.InternalList[item]->selected = TRUE;
	  DrawItem((Widget) lw, item);
	  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
	      (lw->list.AutoSelectionType == XmAUTO_UNSET))
	    {
	      if (ListSelectionChanged(lw))
		lw->list.AutoSelectionType = XmAUTO_CHANGE;
	      else
		lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
	    }
	  ClickElement(lw, NULL, FALSE);
        }
      else                                /* Shifted */
        {
	  /* Save the current selection */
	  for (i = 0; i < lw->list.itemCount; i++)
	    lw->list.InternalList[i]->last_selected =
	      lw->list.InternalList[i]->selected;

	  if (lw->list.selectedItemCount == 0)
	    lw->list.StartItem = item;
	  set = lw->list.InternalList[lw->list.StartItem]->selected;
	  start = MIN(lw->list.StartItem, item);
	  end = MAX(lw->list.StartItem, item);

	  /* Deselect everything outside of the current range. */
	  for (i = 0; i < start; i++)
	    if (lw->list.InternalList[i]->selected)
	      {
		lw->list.InternalList[i]->selected = FALSE;
		DrawItem((Widget) lw, i);
	      }

	  for (i = end + 1; i < lw->list.itemCount; i++)
	    if (lw->list.InternalList[i]->selected)
	      {
		lw->list.InternalList[i]->selected = FALSE;
		DrawItem((Widget) lw, i);
	      }

	  lw->list.EndItem = item;
	  lw->list.LastHLItem = item;
	  SelectRange(lw, lw->list.StartItem, item, set);
	  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
	      (lw->list.AutoSelectionType == XmAUTO_UNSET))
	    {
	      if (ListSelectionChanged(lw))
		lw->list.AutoSelectionType = XmAUTO_CHANGE;
	      else
		lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
	    }
	  ClickElement(lw, NULL, FALSE);
        }
    }
  else                                    /* Add Mode next... */
    {
      if (lw->list.Event & SHIFTDOWN)     /* Shifted */
        {
	  ArrangeRange(lw, item);
	  lw->list.EndItem = item;
	  lw->list.LastHLItem = item;
	  ClickElement(lw, NULL, FALSE);
        }
    }
}

/************************************************************************
 *									*
 * VerifyMotion - event handler for motion within the list.		*
 *									*
 ************************************************************************/
static void
VerifyMotion(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget w = (XmListWidget) wid;
  int item;
  int interval = 100;
  XmListWidget lw = w;
  unsigned char OldLeaveDir = lw->list.LeaveDir;

  if (!(lw->list.Event & BUTTONDOWN) ||
      (lw->list.SelectionPolicy == XmSINGLE_SELECT)  ||
      (lw->list.SelectionPolicy == XmMULTIPLE_SELECT))
    return;


  /****************
   *
   * First, see if we're out of the window. If we are, and
   * if the direction is different than the last leave direction, fake a
   * leave window event. This allows you to drag out of the list, and then
   * futz around with the cursor outside of the window, and it will track
   * correctly.
   *
   ****************/
  if ((event->xmotion.x < (int)lw->core.width)  &&
      (event->xmotion.x > (int)lw->core.x)      &&
      (event->xmotion.y < (int)lw->core.height) &&
      (event->xmotion.y >(int)lw->core.y))
    {
      if (lw->list.DragID)
	{
	  XtRemoveTimeOut(lw->list.DragID);
	  lw->list.DragID = 0;
	}
    }
  else
    {
      if (LayoutIsRtoLP(lw)) {
	if (((event->xmotion.y >= (int)lw->core.height) &&
	     (lw->list.LeaveDir & TOPLEAVE)) ||
	    ((event->xmotion.y <= (int)lw->core.y) &&
	     (lw->list.LeaveDir & BOTTOMLEAVE))  ||
	    ((event->xmotion.x <= (int)lw->core.x) &&
	     (lw->list.LeaveDir & LEFTLEAVE))  ||
	    ((event->xmotion.x >= (int)lw->core.width) &&
	     (lw->list.LeaveDir & RIGHTLEAVE)))
	  {
	    if (lw->list.DragID)
	      {
		XtRemoveTimeOut(lw->list.DragID);
		lw->list.DragID = 0;
	      }
	    ListLeave((Widget) lw, event, params, num_params);
	    return;
	  }
      } else {
	if (((event->xmotion.y >= (int)lw->core.height) &&
	     (lw->list.LeaveDir & TOPLEAVE)) ||
	    ((event->xmotion.y <= (int)lw->core.y) &&
	     (lw->list.LeaveDir & BOTTOMLEAVE))  ||
	    ((event->xmotion.x <= (int)lw->core.x) &&
	     (lw->list.LeaveDir & RIGHTLEAVE))  ||
	    ((event->xmotion.x >= (int)lw->core.width) &&
	     (lw->list.LeaveDir & LEFTLEAVE)))
	  {
	    if (lw->list.DragID)
	      {
		XtRemoveTimeOut(lw->list.DragID);
		lw->list.DragID = 0;
	      }
	    ListLeave((Widget) lw, event, params, num_params);
	    return;
	  }
      }
    }

  lw->list.LeaveDir = 0;
  if (event->xmotion.y >= (int)lw->core.height)	/* Bottom */
    lw->list.LeaveDir |= BOTTOMLEAVE;
  if (event->xmotion.y <= (int)lw->core.y)	/* Top */
    lw->list.LeaveDir |= TOPLEAVE;
  if (LayoutIsRtoLP(lw)) {
    if (event->xmotion.x <= (int)lw->core.x)	/* Left */
      lw->list.LeaveDir |= RIGHTLEAVE;
    if (event->xmotion.x >= (int)lw->core.width)/* Right */
      lw->list.LeaveDir |= LEFTLEAVE;
  } else {
    if (event->xmotion.x <= (int)lw->core.x)	/* Left */
      lw->list.LeaveDir |= LEFTLEAVE;
    if (event->xmotion.x >= (int)lw->core.width)/* Right */
      lw->list.LeaveDir |= RIGHTLEAVE;
  }
  item = WhichItem(lw, event->xmotion.y);

  if (lw->list.LeaveDir)
    {
      if (lw->list.vScrollBar)
	XtVaGetValues((Widget)lw->list.vScrollBar,
		      XmNrepeatDelay, &interval, NULL);

      if (!lw->list.DragID ||
	  (OldLeaveDir != lw->list.LeaveDir))
	{
	  if (lw->list.DragID)
	    XtRemoveTimeOut(lw->list.DragID);
	  lw->list.DragID =
	    XtAppAddTimeOut(XtWidgetToApplicationContext((Widget) lw),
			    (unsigned long) interval,
			    BrowseScroll, (XtPointer) lw);
	}
    }

  if ((item == lw->list.LastHLItem) ||
      (item >= lw->list.itemCount)  ||
      (item < lw->list.top_position)||
      (item >= (lw->list.top_position + lw->list.visibleItemCount)))
    return;

  /* Ok, we have a new item. */
  lw->list.DownCount = 0;
  lw->list.DidSelection = FALSE;
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_MOTION;
  HandleNewItem(lw, item, lw->list.LastHLItem);
}


/***************************************************************************
 *									   *
 * Element Select - invoked on button down on a widget.			   *
 *									   *
 ***************************************************************************/
static void
SelectElement(Widget wid,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  Time interval;
  int i, item;
  int start = 0, end = 1;
  Boolean sel;

  if (!lw->list.itemCount)
    return;

  interval = (Time) lw->list.ClickInterval;

  item = WhichItem(lw, event->xbutton.y);

  if ((item >= (lw->list.top_position+lw->list.visibleItemCount)) ||
      (item < lw->list.top_position) ||
      (item >= lw->list.itemCount))
    return;

  lw->list.Event |= BUTTONDOWN;
  lw->list.LeaveDir = 0;

  if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    {
      if (lw->list.Event & SHIFTDOWN)
	lw->list.SelectionType = XmMODIFICATION;
      else if (lw->list.Event & CTRLDOWN)
	lw->list.SelectionType = XmADDITION;
      else lw->list.SelectionType = XmINITIAL;
    }

  /* Look for a double click. */
  if (!(lw->list.KbdSelection) &&	/* No more doubleclick from space... */
      (lw->list.DownTime != 0) &&
      (lw->list.DownCount > 0) &&
      (event->xbutton.time < (lw->list.DownTime + interval)))
    {
      lw->list.DownCount++;
      lw->list.DownTime = 0;
      return;
    }

  /* Else initialize the count variables. */
  lw->list.DownCount = 1;
  if (!(lw->list.KbdSelection))
    lw->list.DownTime = event->xbutton.time;
  lw->list.DidSelection = FALSE;

  /* Unselect the previous selection if needed. */
  sel = lw->list.InternalList[item]->selected;
  if (((lw->list.SelectionPolicy == XmSINGLE_SELECT)  ||
       (lw->list.SelectionPolicy == XmBROWSE_SELECT)  ||
       (lw->list.SelectionPolicy == XmEXTENDED_SELECT))  &&
      ((!lw->list.AppendInProgress)                      ||
       ((lw->list.SelectionMode == XmNORMAL_MODE)&&
	(lw->list.KbdSelection) &&
	(lw->list.SelectionPolicy == XmMULTIPLE_SELECT))))
    {
      for (i = 0; i < lw->list.itemCount; i++)
        {
	  lw->list.InternalList[i]->last_selected =
	    lw->list.InternalList[i]->selected;
	  if (lw->list.InternalList[i]->selected)
	    {
	      lw->list.InternalList[i]->selected = FALSE;
	      DrawItem((Widget) lw, i);
	    }
        }
    }

  if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    {
      if (lw->list.Event & SHIFTDOWN)
	sel = lw->list.InternalList[lw->list.StartItem]->selected;
      else if (lw->list.Event & CTRLDOWN)
	{
	  lw->list.InternalList[item]->selected =
	    !(lw->list.InternalList[item]->selected);
	}
      else if ((lw->list.Traversing) && (lw->list.SelectionMode == XmADD_MODE))
	{
	  lw->list.InternalList[item]->last_selected =
	    lw->list.InternalList[item]->selected;
	  lw->list.InternalList[item]->selected =
	    !lw->list.InternalList[item]->selected;
	}
      else
	{
	  lw->list.InternalList[item]->selected = TRUE;
	}
    }
  else if ((lw->list.SelectionPolicy == XmMULTIPLE_SELECT) &&
	   (lw->list.InternalList[item]->selected))
    {
      lw->list.InternalList[item]->selected = FALSE;
    }
  else if (((lw->list.SelectionPolicy == XmBROWSE_SELECT) ||
	    (lw->list.SelectionPolicy == XmSINGLE_SELECT)) &&
	   (lw->list.SelectionMode == XmADD_MODE))
    {
      lw->list.InternalList[item]->selected = !sel;
    }
  else
    {
      lw->list.InternalList[item]->selected = TRUE;
    }


  DrawItem((Widget) lw, item);
  XmProcessTraversal((Widget) lw, XmTRAVERSE_CURRENT);
  lw->list.LastHLItem = item;
  lw->list.OldEndItem = lw->list.EndItem;
  lw->list.EndItem = item;

  /* If in extended select mode, and we're appending, select the
   * new range. Look and see if we need to unselect the old range
   * (the cases where the selection endpoint goes from one side of the
   * start to the other.)
   */
  if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) &&
      (lw->list.Event & SHIFTDOWN))
    {
      start = lw->list.StartItem;
      end = lw->list.OldEndItem;
      i = item;
      if (start < end)
	{
	  if (i > end)
	    SelectRange(lw, end + 1, item, sel);
	  else if ((i < end) && (i >= start))
	    RestoreRange(lw, i + 1, end, FALSE);
	  else if (i < start)
	    {
	      if (sel)
		SelectRange(lw, start + 1, end, FALSE);
	      else
		RestoreRange(lw, start + 1, end, FALSE);
	      SelectRange(lw, item, start, sel);
	    }
	}
      if (start > end)
	{
	  if (i < end)
	    SelectRange(lw, item, end + 1, sel);
	  else if ((i > end) && (i <= start))
	    RestoreRange(lw, end, i - 1, FALSE);
	  else if (i > start)
	    {
	      if (sel)
		SelectRange(lw, end, start - 1, FALSE);
	      else
		RestoreRange(lw, end, start - 1, FALSE);
	      SelectRange(lw, start, item, sel);
	    }
	}
      if (start == end)
	SelectRange(lw, start, item, sel);
      if (lw->list.AutoSelect != XmNO_AUTO_SELECT)
	{
	  /* We only want to set the auto selection type if
	   * it hasn't been set already because there are a
	   * number of different ways we can get to this function,
	   * each of which could set this value. */
	  if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	    lw->list.AutoSelectionType = XmAUTO_BEGIN;
	  ClickElement(lw, NULL, FALSE);
	}
      return;
    }
  lw->list.OldStartItem = lw->list.StartItem;
  lw->list.StartItem = item;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
       (lw->list.SelectionPolicy == XmBROWSE_SELECT)))
    {
      /* We only want to set the auto selection type if
       * it hasn't been set already because there are a
       * number of different ways we can get to this function,
       * each of which could set this value. */
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_BEGIN;
      ClickElement(lw, NULL, FALSE);
    }
}

/***************************************************************************
 *									   *
 * KbdSelectElement - invoked on keyboard selection.			   *
 *									   *
 ***************************************************************************/
static void
KbdSelectElement(Widget wid,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.KbdSelection = TRUE;
  if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) &&
      (lw->list.SelectionMode == XmADD_MODE))
    {
      lw->list.Event |= CTRLDOWN;
      lw->list.AppendInProgress = TRUE;
    }

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_BEGIN;

  SelectElement((Widget) lw, event, params, num_params);
  lw->list.KbdSelection = FALSE;
}

/***************************************************************************
 *									   *
 * Element UnSelect - Handle the button up event.			   *
 *									   *
 ***************************************************************************/
static void
UnSelectElement(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item;

  if (!lw->list.itemCount)
    return;
  item = WhichItem(lw, event->xbutton.y);
  ASSIGN_MAX(item, lw->list.top_position);
  if (item > (lw->list.top_position + lw->list.visibleItemCount))
    item = (lw->list.top_position + lw->list.visibleItemCount - 1);
  if (item >= lw->list.itemCount)
    item = lw->list.itemCount - 1;

  if (!(lw->list.Event & BUTTONDOWN))
    return;

  if (!lw->list.KbdSelection)
    {
      lw->list.OldStartItem = lw->list.StartItem;
      lw->list.OldEndItem = lw->list.EndItem;
    }

  if (lw->list.Traversing)
    {
      if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	  (lw->list.SelectionPolicy == XmBROWSE_SELECT))
        {
	  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.CurrentKbdItem = item;
	  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
        }
      else
        {
	  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.CurrentKbdItem = lw->list.LastHLItem;
	  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
        }
    }
  else
    lw->list.CurrentKbdItem = item;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if ((lw->list.SelectionPolicy == XmBROWSE_SELECT) ||
	  (lw->list.SelectionPolicy == XmEXTENDED_SELECT))
	{
	  if (ListSelectionChanged(lw))
	    lw->list.AutoSelectionType = XmAUTO_CHANGE;
	  else
	    lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
	}
    }

  lw->list.Event = 0;

  /* CR 9442: Don't ClickElement twice in AutoSelect mode. */
  if (lw->list.DownCount > 1)
    DefaultAction(lw, event);
  else if ((lw->list.AutoSelect == XmNO_AUTO_SELECT) ||
	   (!lw->list.DidSelection))
    ClickElement(lw, event, FALSE);

  if (lw->list.AutoSelect != XmNO_AUTO_SELECT)
    {
      UpdateSelectedList(lw, TRUE);
      UpdateSelectedPositions(lw, lw->list.selectedItemCount);
    }

  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
  lw->list.AppendInProgress = FALSE;
}

/***************************************************************************
 *									   *
 * KbdUnSelectElement - invoked on keyboard selection.			   *
 *									   *
 ***************************************************************************/
static void
KbdUnSelectElement(Widget wid,
		   XEvent *event,
		   String *params,
		   Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.KbdSelection = TRUE;
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if (ListSelectionChanged(lw))
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  UnSelectElement((Widget) lw, event, params, num_params);
  lw->list.KbdSelection = FALSE;
  lw->list.AppendInProgress = FALSE;
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Shift Select								*
 *									*
 ************************************************************************/
static void
ExSelect(Widget wid,
	 XEvent *event,
	 String *params,
	 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= SHIFTDOWN;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_BEGIN;

  SelectElement((Widget) lw, event, params, num_params);
}

/************************************************************************
 *									*
 * Shift UnSelect							*
 *									*
 ************************************************************************/
static void
ExUnSelect(Widget wid,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  lw->list.AppendInProgress = FALSE;

  if (!(lw->list.Event & BUTTONDOWN) ||
      (lw->list.SelectionPolicy != XmEXTENDED_SELECT))
	{
  	lw->list.Event &= ~SHIFTDOWN;
	UnSelectElement((Widget)lw, event, params, num_params);
	return;
	}

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if (ListSelectionChanged(lw))
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  UnSelectElement((Widget) lw, event, params, num_params);
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * CtrlBtnSelect							*
 *									*
 ************************************************************************/
static void
CtrlBtnSelect(Widget wid,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /*
   * There's already an action for Ctrl-Btn1 when in Extended Select
   * mode, so we can't change that.
   */
  if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    CtrlSelect(wid, event, params, num_params);
  else
    XmProcessTraversal(wid, XmTRAVERSE_CURRENT);
}

/************************************************************************
 *									*
 * Ctrl Select								*
 *									*
 ************************************************************************/
static void
CtrlSelect(Widget wid,
	   XEvent *event,
	   String *params,
	   Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int i, j;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= (CTRLDOWN);
  lw->list.OldStartItem = lw->list.StartItem;
  lw->list.OldEndItem = lw->list.EndItem;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_BEGIN;

  /****************
   *
   * Since we know we are adding items to a selection, save the state of
   * the last selected range. This allows the rubberbanding and
   * shift-select functionality to work correctly.
   *
   ****************/
  i = MIN(lw->list.OldStartItem, lw->list.OldEndItem);
  i = MAX(i, 0);
  j = MAX(lw->list.OldStartItem, lw->list.OldEndItem);
  if ((i != 0) || (j != 0))
    for (; i <= j && i < lw->list.itemCount; i++)
      lw->list.InternalList[i]->last_selected =
	lw->list.InternalList[i]->selected;

  SelectElement((Widget)lw, event, params, num_params);
}

/************************************************************************
 *									*
 * Ctrl UnSelect							*
 *									*
 ************************************************************************/
static void
CtrlUnSelect(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  lw->list.AppendInProgress = FALSE;

  if (!(lw->list.Event & BUTTONDOWN) ||
      (lw->list.SelectionPolicy != XmEXTENDED_SELECT))
	{
	lw->list.Event &= ~CTRLDOWN;
	UnSelectElement((Widget)lw, event, params, num_params);
	return;
	}

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if (ListSelectionChanged(lw))
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  UnSelectElement((Widget)lw, event, params, num_params);
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Keyboard Shift Select						*
 *									*
 ************************************************************************/
static void
KbdShiftSelect(Widget wid,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= SHIFTDOWN;
  lw->list.OldStartItem = lw->list.StartItem;
  lw->list.OldEndItem = lw->list.EndItem;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_BEGIN;

  KbdSelectElement((Widget)lw, event, params, num_params);
}

/************************************************************************
 *									*
 * Keyboard Shift UnSelect						*
 *									*
 ************************************************************************/
static void
KbdShiftUnSelect(Widget wid,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (!(lw->list.Event & BUTTONDOWN) ||
      (lw->list.SelectionPolicy != XmEXTENDED_SELECT))
    return;

  lw->list.AppendInProgress = FALSE;
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if (ListSelectionChanged(lw))
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  KbdUnSelectElement((Widget)lw, event, params, num_params);
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Keyboard Ctrl Select							*
 *									*
 ************************************************************************/
static void
KbdCtrlSelect(Widget wid,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int i, j;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  if (lw->list.SelectionMode == XmNORMAL_MODE)
    {
      KbdSelectElement((Widget)lw, event, params, num_params);
      return;
    }

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= CTRLDOWN;
  lw->list.OldStartItem = lw->list.StartItem;
  lw->list.OldEndItem = lw->list.EndItem;

  /****************
   *
   * Since we know we are adding items to a selection, save the state of
   * the last selected range. This allows the rubberbanding and
   * shift-select functionality to work correctly.
   *
   ****************/
  i = MIN(lw->list.OldStartItem, lw->list.OldEndItem);
  i = MAX(i, 0);
  j = MAX(lw->list.OldStartItem, lw->list.OldEndItem);
  if ((i != 0) || (j != 0))
    for (; i <= j && i < lw->list.itemCount; i++)
      lw->list.InternalList[i]->last_selected =
	lw->list.InternalList[i]->selected;

  KbdSelectElement((Widget)lw, event, params, num_params);

}

/************************************************************************
 *									*
 * Keyboard Ctrl UnSelect			        		*
 *									*
 ************************************************************************/
static void
KbdCtrlUnSelect(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (!(lw->list.Event & BUTTONDOWN) ||
      (lw->list.SelectionPolicy != XmEXTENDED_SELECT))
    return;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    {
      if (ListSelectionChanged(lw))
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  if (lw->list.SelectionMode == XmNORMAL_MODE)
    {
      KbdUnSelectElement((Widget)lw, event, params, num_params);
      return;
    }

  lw->list.AppendInProgress = FALSE;
  KbdUnSelectElement((Widget)lw, event, params, num_params);
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Keyboard Activate                                                    *
 *									*
 ************************************************************************/
static void
KbdActivate(Widget wid,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  XmParentInputActionRec  p_event;
  int i;

  if (!lw->list.itemCount || !lw->list.items)
    return;

  lw->list.AppendInProgress = FALSE;


  if ((lw->list.SelectionPolicy == XmSINGLE_SELECT) ||
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      for (i = 0; i < lw->list.selectedPositionCount; i++)
        {
	  int pos = lw->list.selectedPositions[i] - 1;

	  lw->list.InternalList[pos]->selected = FALSE;
	  lw->list.InternalList[pos]->last_selected = FALSE;
	  DrawItem((Widget) lw, pos);
        }
    }

  if (lw->list.CurrentKbdItem >= lw->list.itemCount)
    lw->list.CurrentKbdItem = lw->list.itemCount - 1;

  lw->list.LastHLItem = lw->list.CurrentKbdItem;
  lw->list.InternalList[lw->list.CurrentKbdItem]->selected = TRUE;
  lw->list.InternalList[lw->list.CurrentKbdItem]->last_selected = TRUE;
  DrawItem((Widget) lw, lw->list.CurrentKbdItem);

  DefaultAction(lw, event);
  lw->list.Event = 0;
  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_ACTIVATE;
  p_event.event = event;	/* Pointer to XEvent. */
  p_event.params = params;	/* Or use what you have if   */
  p_event.num_params = num_params;/* input is from translation.*/

  _XmParentProcess(XtParent(lw), (XmParentProcessData) &p_event);
}

/************************************************************************
 *									*
 * Keyboard Cancel							*
 *									*
 ************************************************************************/
static void
KbdCancel(Widget wid,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  XmParentInputActionRec p_event;

  p_event.process_type = XmINPUT_ACTION;
  p_event.action = XmPARENT_CANCEL;
  p_event.event = event;	/* Pointer to XEvent. */
  p_event.params = params;	/* Or use what you have if   */
  p_event.num_params = num_params;/* input is from translation.*/

  if (!(lw->list.Event & BUTTONDOWN))		/* Only if not selecting */
    {
      if (_XmParentProcess(XtParent(lw), (XmParentProcessData) &p_event))
	return;
    }

  if (((lw->list.SelectionPolicy != XmEXTENDED_SELECT) &&
       (lw->list.SelectionPolicy != XmBROWSE_SELECT))  ||
      !(lw->list.Event & BUTTONDOWN))
    return;

  if (lw->list.DragID)
    {
      XtRemoveTimeOut(lw->list.DragID);
      lw->list.DragID = 0;
    }

  /* BEGIN OSF Fix CR 5644 */
  if (lw->list.previous_top_position != -1)
    {
      DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = lw->list.previous_top_position;
    }
  /* END OSF Fix CR 5644 */

  /* BEGIN OSF Fix CR 5117 */
  RestoreRange(lw, 0, lw->list.itemCount - 1, TRUE);
  /* END OSF Fix CR 5117 */

  lw->list.StartItem = lw->list.OldStartItem;
  lw->list.EndItem = lw->list.OldEndItem;
  lw->list.AppendInProgress = FALSE;
  lw->list.Event = 0;

  /* BEGIN OSF Fix CR 5644 */
  if (lw->list.top_position == lw->list.previous_top_position)
    {
      SetVerticalScrollbar(lw);
      SetHorizontalScrollbar(lw);
      DrawList(lw, NULL, TRUE);
      lw->list.previous_top_position = -1;
    }
  /* END OSF Fix CR 5644 */

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
       (lw->list.SelectionPolicy == XmBROWSE_SELECT)))
    {
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_CANCEL;
      ClickElement(lw, NULL, FALSE);
    }
}

/************************************************************************
 *									*
 * Keyboard toggle Add Mode                                             *
 *									*
 ************************************************************************/
static void
KbdToggleAddMode(Widget wid,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    {
      XmListSetAddMode((Widget) lw,
		       !(lw->list.SelectionMode == XmADD_MODE));
      ClickElement(lw, event, FALSE);
    }
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Keyboard Select All                                                  *
 *									*
 ************************************************************************/
static void
KbdSelectAll(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int i;
  Boolean selection_changed = FALSE;

  /* Do nothing on empty lists. */
  if (!lw->list.itemCount || !lw->list.items)
    return;

  lw->list.AppendInProgress = FALSE;
  if ((lw->list.SelectionPolicy != XmEXTENDED_SELECT) &&
      (lw->list.SelectionPolicy != XmMULTIPLE_SELECT))
    {
      for (i = 0; i < lw->list.selectedPositionCount; i++)
	{
	  int pos = lw->list.selectedPositions[i] - 1;

	  lw->list.InternalList[pos]->last_selected =
	    lw->list.InternalList[pos]->selected;
	  lw->list.InternalList[pos]->selected = FALSE;
	  DrawItem((Widget) lw, pos);
	}
      lw->list.LastHLItem = lw->list.CurrentKbdItem;

      /* If we are in browse selection mode and the current selected
       * item was not selected in the last_selected list, then the
       * selection has changed. Otherwise, we are simply selecting the
       * same item again. */
      if ((lw->list.SelectionPolicy == XmBROWSE_SELECT) &&
	  !lw->list.InternalList[lw->list.CurrentKbdItem]->last_selected)
	selection_changed = TRUE;

      lw->list.InternalList[lw->list.CurrentKbdItem]->selected = TRUE;
      lw->list.InternalList[lw->list.CurrentKbdItem]->last_selected = TRUE;
      DrawItem((Widget) lw, lw->list.CurrentKbdItem);
    }
  else if (lw->list.selectedPositionCount != lw->list.itemCount)
    {
      /* If any item is currently unselected, then the
       * selection is going to be different as a result of
       * doing an all select. */
      selection_changed = TRUE;

      for (i = 0; i < lw->list.itemCount; i++)
	if (!(lw->list.InternalList[i]->selected))
	  {
	    lw->list.InternalList[i]->last_selected =
	      lw->list.InternalList[i]->selected;
	    lw->list.InternalList[i]->selected = TRUE;
	    DrawItem((Widget) lw, i);
	  }
    }

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET) &&
      (lw->list.SelectionPolicy == XmEXTENDED_SELECT ||
       lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (selection_changed)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  ClickElement(lw, event, FALSE);
  lw->list.Event = 0;
}

/************************************************************************
 *									*
 * Keyboard DeSelect All                                                *
 *									*
 ************************************************************************/
static void
KbdDeSelectAll(Widget wid,
	       XEvent *event,
	       String *params,
	       Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int i, j;
  Boolean selection_changed = FALSE;

  /* Do nothing on empty lists. */
  if (!lw->list.itemCount || !lw->list.items)
    return;

  if (((lw->list.SelectionPolicy == XmSINGLE_SELECT) ||
       (lw->list.SelectionPolicy == XmBROWSE_SELECT)) &&
      (lw->list.SelectionMode == XmNORMAL_MODE))
    return;

  if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) &&
      (lw->list.SelectionMode == XmNORMAL_MODE)  &&
      (_XmGetFocusPolicy((Widget) lw) == XmEXPLICIT))
    j = lw->list.CurrentKbdItem;
  else
    j = (-1);

  lw->list.AppendInProgress = FALSE;
  for (i = 0; i < lw->list.selectedPositionCount; i++)
    {
      int pos = lw->list.selectedPositions[i] - 1;

      if (pos != j)
	{
	  /* If any item is currently selected, then the selection
	   * will be different based on the deselect all action. */
	  selection_changed = TRUE;
	  lw->list.InternalList[pos]->last_selected =
	    lw->list.InternalList[pos]->selected;
	  lw->list.InternalList[pos]->selected = FALSE;
	  DrawItem((Widget) lw, pos);
	}
    }

  /* If auto selection is enabled and we are in extended
   * or browse selection modes, then we need to set up the
   * auto selection type. If the auto selection type has
   * already been set by someone else, then don't change it. */
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET) &&
      (lw->list.SelectionPolicy == XmEXTENDED_SELECT ||
       lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (selection_changed)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      else
	lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
    }

  ClickElement(lw, event, FALSE);
  lw->list.Event = 0;
}

/***************************************************************************
 *									   *
 * DefaultAction - call the double click callback.			   *
 *									   *
 ***************************************************************************/

static void
DefaultAction(XmListWidget lw,
	      XEvent *event)
{
  XmListCallbackStruct cb;
  int item;
  int i, SLcount;

  item = lw->list.LastHLItem;
  lw->list.DidSelection = TRUE;

  if (item < 0 || item >= lw->list.itemCount)
    return;

  /* If there's a drag timeout, remove it so we don't see two selections. */
  if (lw->list.DragID)
    {
      XtRemoveTimeOut(lw->list.DragID);
      lw->list.DragID = 0;
    }

  if (lw->list.InternalList[item]->length == UNKNOWN_LENGTH)
    lw->list.InternalList[item]->length = XmCvtXmStringToByteStream(lw->list.items[item], NULL);

  cb.reason = XmCR_DEFAULT_ACTION;
  cb.event = event;
  cb.item_length = lw->list.InternalList[item]->length;
  cb.item_position = item + 1;
  cb.item = XmStringCopy(lw->list.items[item]);
  cb.selected_item_count = 0;
  cb.selected_items = NULL;
  cb.selected_item_positions = NULL;

  UpdateSelectedList(lw, TRUE);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);
  SLcount = lw->list.selectedItemCount;

  if (lw->list.selectedItems && lw->list.selectedItemCount)
    {
      cb.selected_items =
	(XmString *)ALLOCATE_LOCAL(sizeof(XmString) * SLcount);
      cb.selected_item_positions =
	(int *)ALLOCATE_LOCAL(sizeof(int) * SLcount);
      for (i = 0; i < SLcount; i++)
	{
	  cb.selected_items[i] = XmStringCopy(lw->list.selectedItems[i]);
	  cb.selected_item_positions[i] = lw->list.selectedPositions[i];
	}
    }
  cb.selected_item_count = SLcount;
  cb.auto_selection_type = lw->list.AutoSelectionType;

  XtCallCallbackList((Widget) lw, lw->list.DefaultCallback, &cb);

  lw->list.AutoSelectionType = XmAUTO_UNSET;
  for (i = 0; i < SLcount; i++)
    XmStringFree(cb.selected_items[i]);
  DEALLOCATE_LOCAL((char*)cb.selected_items);
  DEALLOCATE_LOCAL((char*)cb.selected_item_positions);
  XmStringFree(cb.item);

  lw->list.DownCount = 0;
}

/************************************************************************
 *									*
 * ClickElement - invoked for all selection actions other than double	*
 * click.  This fills out the callback record and invokes the		*
 * appropriate callback.						*
 *									*
 ************************************************************************/

static void
ClickElement(XmListWidget lw,
	     XEvent *event,
	     Boolean default_action)
{
  int item, SLcount, i;
  unsigned char selection_policy;
  XmListCallbackStruct cb;

  bzero((char*) &cb, sizeof(XmListCallbackStruct));
  item = lw->list.LastHLItem;
  lw->list.DidSelection = TRUE;

  if (item < 0 || item >= lw->list.itemCount)
    return;

  /* If there's a drag timeout, remove it so we don't see two selections. */
  if (lw->list.DragID)
    {
      XtRemoveTimeOut(lw->list.DragID);
      lw->list.DragID = 0;
    }

  assert(lw->list.itemCount && lw->list.InternalList);
  if (lw->list.InternalList[item]->length == UNKNOWN_LENGTH)
    lw->list.InternalList[item]->length = XmCvtXmStringToByteStream(lw->list.items[item], NULL);

  cb.event = event;
  cb.item_length = lw->list.InternalList[item]->length;
  cb.item_position = item + 1;
  cb.item = XmStringCopy(lw->list.items[item]);

  if (lw->list.AutoSelect != XmNO_AUTO_SELECT)
    {
      ClearSelectedList(lw);
      BuildSelectedList(lw, FALSE);   /* Don't commit in auto mode. Yuk. */
    }
  else
    UpdateSelectedList(lw, TRUE);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);
  SLcount = lw->list.selectedItemCount;

  /* A callback can change the policy. Use the saved value for alloc and free
     of the selected_items list. */
  selection_policy = lw->list.SelectionPolicy;

  if ((selection_policy == XmMULTIPLE_SELECT) ||
      (selection_policy == XmEXTENDED_SELECT))
    {
      if (lw->list.selectedItems && lw->list.selectedItemCount)
    	{
	  cb.selected_items =
	    (XmString *)ALLOCATE_LOCAL(sizeof(XmString) * SLcount);
	  cb.selected_item_positions =
	    (int *)ALLOCATE_LOCAL(sizeof(int) * SLcount);
	  for (i = 0; i < SLcount; i++)
	    {
	      cb.selected_items[i] = XmStringCopy(lw->list.selectedItems[i]);
	      cb.selected_item_positions[i] = lw->list.selectedPositions[i];
	    }
	}
    }
  /* BEGIN OSF Fix CR 4576 */
  cb.selected_item_count = SLcount;
  /* END OSF Fix CR 4576 */

  if (default_action)
    {
      cb.reason = XmCR_DEFAULT_ACTION;
      cb.auto_selection_type = lw->list.AutoSelectionType;
      XtCallCallbackList((Widget) lw, lw->list.DefaultCallback, &cb);
    }
  else
    {
      switch(selection_policy)
	{
	case XmSINGLE_SELECT:
	  cb.reason = XmCR_SINGLE_SELECT;
	  XtCallCallbackList((Widget) lw, lw->list.SingleCallback, &cb);
	  break;

	case XmBROWSE_SELECT:
	  cb.reason = XmCR_BROWSE_SELECT;
	  cb.auto_selection_type = lw->list.AutoSelectionType;
	  XtCallCallbackList((Widget) lw, lw->list.BrowseCallback, &cb);
	  break;

	case XmMULTIPLE_SELECT:
	  cb.reason = XmCR_MULTIPLE_SELECT;
	  XtCallCallbackList((Widget) lw, lw->list.MultipleCallback, &cb);
	  break;

	case XmEXTENDED_SELECT:
	  cb.reason = XmCR_EXTENDED_SELECT;
	  cb.selection_type = lw->list.SelectionType;
	  cb.auto_selection_type = lw->list.AutoSelectionType;
	  XtCallCallbackList((Widget) lw, lw->list.ExtendCallback, &cb);
	  break;
	}
    }

  /* Reset the AutoSelectionType. It may not actually be set to anything but
   * let's reset it in all cases just to be sure everthing remains clean. */
  lw->list.AutoSelectionType = XmAUTO_UNSET;

  if ((selection_policy == XmMULTIPLE_SELECT) ||
      (selection_policy == XmEXTENDED_SELECT))
    {
      if (SLcount)
    	{
	  if (cb.selected_items)
	    for (i = 0; i < SLcount; i++)
	      if (cb.selected_items[i])
		XmStringFree(cb.selected_items[i]);
	  DEALLOCATE_LOCAL((char *) cb.selected_items);
	  DEALLOCATE_LOCAL((char *) cb.selected_item_positions);
	}
    }

  XmStringFree(cb.item);
}

static void
GetPreeditPosition(XmListWidget lw,
		   XPoint *xmim_point)
{
  xmim_point->x = lw->list.BaseX;

  if (lw->list.CurrentKbdItem == lw->list.top_position)
    {
      if (lw->list.visibleItemCount <= 1) /* on top */
	xmim_point->y = lw->list.BaseY + lw->list.MaxItemHeight;
      else			/* below current item */
	xmim_point->y = lw->list.BaseY + 2*lw->list.MaxItemHeight +
	  lw->list.spacing;
    }
  else if (lw->list.CurrentKbdItem < lw->list.top_position ||
	   lw->list.CurrentKbdItem >=
	   lw->list.top_position + lw->list.visibleItemCount)
    {
      /* on top */
      xmim_point->y = lw->list.BaseY + lw->list.MaxItemHeight;
    }
  else
    {
      /* above current item */
      xmim_point->y = (lw->list.BaseY +
		       LINEHEIGHTS(lw, (lw->list.CurrentKbdItem -
					lw->list.top_position)) -
		       2 * lw->list.HighlightThickness);
    }
}

/************************************************************************
 *									*
 * ListFocusIn								*
 *									*
 ************************************************************************/
static void
ListFocusIn(Widget wid,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->primitive.traversal_on &&
      (_XmGetFocusPolicy((Widget) lw) == XmEXPLICIT) &&
      (event->xfocus.send_event))
    {
      lw->list.Traversing = TRUE;
      if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	{
	  XPoint xmim_point;

	  GetPreeditPosition(lw, &xmim_point);
	  XmImVaSetFocusValues(wid, XmNspotLocation, &xmim_point, NULL);
	}
    }

  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
  _XmPrimitiveFocusIn((Widget) lw, event, NULL, NULL);
}

/************************************************************************
 *									*
 * ListFocusOut								*
 *									*
 ************************************************************************/
static void
ListFocusOut(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (!(lw->list.Traversing))
    return;

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  lw->list.Traversing = FALSE;
  _XmPrimitiveFocusOut((Widget) lw, event, NULL, NULL);
}

/************************************************************************
 *									*
 * BrowseScroll - timer proc that scrolls the list if the user has left *
 *		the window with the button down. If the button has been *
 *		released, call the standard click stuff.		*
 *									*
 ************************************************************************/
static void
BrowseScroll(XtPointer closure,
	     XtIntervalId *id)
{
  XmListWidget lw = (XmListWidget) closure;
  int item, newitem;
  Boolean vLeave = TRUE;
  Boolean hLeave = TRUE;
  int interval = 100;
  int inc = 1;

  if (lw->list.DragID == 0)
    return;

  lw->list.DragID = 0;

  /* If the button went up, remove the timeout and call the selection
   * code. */
  if (!(lw->list.Event & BUTTONDOWN))
    {
      if (lw->list.DownCount > 1)
	DefaultAction(lw, NULL);
      else
	ClickElement(lw, NULL, FALSE);

      if (lw->list.Traversing)
        {
	  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.CurrentKbdItem = lw->list.LastHLItem;
	  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
        }
      else
	lw->list.CurrentKbdItem = lw->list.LastHLItem;

      if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	{
	  XPoint xmim_point;

	  GetPreeditPosition(lw, &xmim_point);
	  XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
	}
      return;
    }

  item = lw->list.LastHLItem;

  /* See if the user moved out the top of the list and there's another
   * element to go to. */
  if (lw->list.LeaveDir & TOPLEAVE)
    {
      if ((lw->list.top_position <= 0) ||
	  !(lw->list.vScrollBar))
	{
	  vLeave = TRUE;
	}
      else
        {
	  if (lw->list.Traversing)
	    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.top_position--;
	  item = lw->list.top_position;
	  vLeave = FALSE;
        }
    }

  /* Now see if we went off the end and need to scroll up. */
  if (lw->list.LeaveDir & BOTTOMLEAVE)
    {
      newitem = lw->list.top_position + lw->list.visibleItemCount;
      if ((newitem >= lw->list.itemCount) ||
	  !(lw->list.vScrollBar))
	{
	  vLeave = TRUE;
	}
      else
        {
	  if (lw->list.Traversing)
	    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.top_position++;
	  item = newitem;
	  vLeave = FALSE;
        }
    }

  /* Now see if we went off the right and need to scroll left. */
  if (lw->list.LeaveDir & LEFTLEAVE)
    {
      if ((lw->list.hOrigin <= 0) ||
	  !(lw->list.hScrollBar))
	{
	  hLeave = TRUE;
	}
      else
        {
	  if (lw->list.Traversing)
	    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  XtVaGetValues((Widget)lw->list.hScrollBar,
			XmNincrement, &inc, NULL);
	  lw->list.hOrigin -= inc;
	  lw->list.XOrigin = lw->list.hOrigin;
	  hLeave = FALSE;
        }
    }

  /* Now see if we went off the left and need to scroll right. */
  if (lw->list.LeaveDir & RIGHTLEAVE)
    {
      if ((lw->list.hOrigin >= lw->list.hmax - lw->list.hExtent) ||
	  !(lw->list.hScrollBar))
	{
	  hLeave = TRUE;
	}
      else
        {
	  if (lw->list.Traversing)
	    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  XtVaGetValues((Widget)lw->list.hScrollBar,
			XmNincrement, &inc, NULL);
	  lw->list.hOrigin += inc;
	  lw->list.XOrigin = lw->list.hOrigin;
	  hLeave = FALSE;
        }
    }

  if (vLeave && hLeave)
    return;
  if (!vLeave)
    SetVerticalScrollbar(lw);
  if (!hLeave)
    SetHorizontalScrollbar(lw);
  DrawList(lw, NULL, TRUE);

  if (lw->list.vScrollBar)
    XtVaGetValues((Widget)lw->list.vScrollBar,
		  XmNrepeatDelay, &interval, NULL);

  /* Ok, we have a new item. */
  lw->list.DownCount = 0;
  /* BEGIN OSF Fix CR 5954 */
  lw->list.DidSelection = FALSE;
  /* END OSF Fix CR 5954 */

  if (item != lw->list.LastHLItem)
    HandleNewItem(lw, item, lw->list.LastHLItem);
  XSync (XtDisplay (lw), False);
  lw->list.DragID = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)lw),
				    (unsigned long) interval,
				    BrowseScroll, (XtPointer) lw);
}

/************************************************************************
 *									*
 * ListLeave - If the user leaves in Browse or Extended Select mode	*
 *	       with the button down, set up a timer to scroll the list	*
 *	       elements.						*
 *									*
 ************************************************************************/
static void
ListLeave(Widget wid,
	  XEvent *event,
	  String *params,	/* unused */
	  Cardinal *num_params)	/* unused */
{
  XmListWidget lw = (XmListWidget) wid;
  int interval = 200;
  _XmToolTipLeave(wid, event, params, num_params);

  if ((_XmGetFocusPolicy((Widget) lw) == XmPOINTER) &&
      (lw->primitive.highlight_on_enter))
    {
      DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.Traversing = FALSE;
    }

  if (((lw->list.SelectionPolicy != XmBROWSE_SELECT) &&
       (lw->list.SelectionPolicy != XmEXTENDED_SELECT)) ||
      !(lw->list.Event & BUTTONDOWN))
    return;

  lw->list.LeaveDir = 0;
  if (event->xcrossing.y >= (int)lw->core.height)	/* Bottom */
    {
      lw->list.LeaveDir |= BOTTOMLEAVE;
      lw->list.previous_top_position = lw->list.top_position;
    }
  if (event->xcrossing.y <= (int)lw->core.y)		/* Top */
    {
      lw->list.LeaveDir |= TOPLEAVE;
      lw->list.previous_top_position = lw->list.top_position;
    }
  if (LayoutIsRtoLP(lw)) {
    if (event->xcrossing.x <= (int)lw->core.x)		/* Left */
      lw->list.LeaveDir |= RIGHTLEAVE;
    if (event->xcrossing.x >= (int)lw->core.width)	/* Right */
      lw->list.LeaveDir |= LEFTLEAVE;
  } else {
    if (event->xcrossing.x <= (int)lw->core.x)		/* Left */
      lw->list.LeaveDir |= LEFTLEAVE;
    if (event->xcrossing.x >= (int)lw->core.width)	/* Right */
      lw->list.LeaveDir |= RIGHTLEAVE;
  }
  if (lw->list.LeaveDir == 0)
    {
      lw->list.DragID = 0;
      return;
    }
  if (lw->list.vScrollBar)
    XtVaGetValues((Widget)lw->list.vScrollBar,
		  XmNinitialDelay, &interval, NULL);

  lw->list.DragID = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)lw),
				    (unsigned long) interval,
				    BrowseScroll, (XtPointer) lw);

  _XmPrimitiveLeave((Widget) lw, event, NULL, NULL);
}

/************************************************************************
 *									*
 * ListEnter - If there is a drag timeout, remove it.			*
 *									*
 ************************************************************************/
static void
ListEnter(Widget wid,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.DragID)
    {
      XtRemoveTimeOut(lw->list.DragID);
      lw->list.DragID = 0;
    }

  if ((_XmGetFocusPolicy((Widget) lw) == XmPOINTER) &&
      (lw->primitive.highlight_on_enter))
    {
      lw->list.Traversing = TRUE;
      DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
    }

  if ((_XmGetFocusPolicy((Widget) lw) == XmPOINTER) &&
      (lw->list.matchBehavior == XmQUICK_NAVIGATE))
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetFocusValues(wid, XmNspotLocation, &xmim_point, NULL);
    }

  _XmPrimitiveEnter((Widget) lw, event, NULL, NULL);
}

/************************************************************************
 *                                                                      *
 * MakeItemVisible - scroll the list (if needed) such that the given    *
 * item is visible                                                      *
 *                                                                      *
 ************************************************************************/

static void
MakeItemVisible(XmListWidget lw,
		int item)
{
  if (item < lw->list.top_position)
    {
      if (lw->list.vScrollBar)
	{
	  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	  lw->list.top_position = item;
	  DrawList(lw, NULL, TRUE);
	  SetVerticalScrollbar(lw);
	}
    }

  if (item >= (lw->list.top_position + lw->list.visibleItemCount))
    {
      if (!(lw->list.vScrollBar))
	return;
      DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = item - (lw->list.visibleItemCount - 1);
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
}

/************************************************************************
 *									*
 * PrevElement - called when the user hits Up arrow.			*
 *									*
 ************************************************************************/
static void
PrevElement(XmListWidget lw,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  int item, olditem;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  item = lw->list.CurrentKbdItem - 1;
  if (item < 0)
    return;

  if ((!lw->list.Mom) &&
      (item < lw->list.top_position))
    return;

  MakeItemVisible(lw, item);
  olditem = lw->list.CurrentKbdItem;
  DrawHighlight(lw, olditem, FALSE);
  lw->list.CurrentKbdItem = item;
  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      HandleNewItem(lw, item, olditem);
    }
  else if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	   (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    HandleExtendedItem(lw, item);

  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
}

/************************************************************************
 *									*
 * NextElement - called when the user hits Down arrow.			*
 *									*
 ************************************************************************/
static void
NextElement(XmListWidget lw,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  int item, olditem;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  item = lw->list.CurrentKbdItem + 1;
  if (item >= lw->list.itemCount)
    return;

  if ((!lw->list.Mom) &&
      (item >= (lw->list.top_position + lw->list.visibleItemCount)))
    return;

  MakeItemVisible(lw, item);
  olditem = lw->list.CurrentKbdItem;
  DrawHighlight(lw, olditem, FALSE);
  lw->list.CurrentKbdItem = item;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      HandleNewItem(lw, item, olditem);
    }
  else if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	   (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      HandleExtendedItem(lw, item);
    }

  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
}

/************************************************************************
 *									*
 * Normal Next Element							*
 *									*
 ************************************************************************/

static void
NormalNextElement(Widget wid,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = FALSE;
  lw->list.Event &= ~(SHIFTDOWN | CTRLDOWN | ALTDOWN);
  lw->list.SelectionType = XmINITIAL;
  NextElement(lw, event, params, num_params);
}

/************************************************************************
 *									*
 * Shift Next Element							*
 *									*
 ************************************************************************/

static void
ShiftNextElement(Widget wid,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= SHIFTDOWN;
  lw->list.SelectionType = XmMODIFICATION;

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_CHANGE;

  NextElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * Ctrl Next Element							*
 *									*
 ************************************************************************/

static void
CtrlNextElement(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= CTRLDOWN;
  lw->list.SelectionType = XmADDITION;
  NextElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * ExtendAdd Next Element						*
 *									*
 ************************************************************************/

static void
ExtendAddNextElement(Widget wid,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= (SHIFTDOWN | CTRLDOWN);
  lw->list.SelectionType = XmMODIFICATION;
  NextElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * Normal Prev Element							*
 *									*
 ************************************************************************/

static void
NormalPrevElement(Widget wid,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = FALSE;
  lw->list.Event &= ~(SHIFTDOWN | CTRLDOWN | ALTDOWN);
  lw->list.SelectionType = XmINITIAL;
  PrevElement(lw, event, params, num_params);
}

/************************************************************************
 *									*
 * Shift Prev Element							*
 *									*
 ************************************************************************/

static void
ShiftPrevElement(Widget wid,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= SHIFTDOWN;
  lw->list.SelectionType = XmMODIFICATION;
  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.AutoSelectionType == XmAUTO_UNSET))
    lw->list.AutoSelectionType = XmAUTO_CHANGE;
  PrevElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * Ctrl Prev Element							*
 *									*
 ************************************************************************/

static void
CtrlPrevElement(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= CTRLDOWN;
  lw->list.SelectionType = XmADDITION;
  PrevElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * ExtendAdd Prev Element						*
 *									*
 ************************************************************************/

static void
ExtendAddPrevElement(Widget wid,
		     XEvent *event,
		     String *params,
		     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (lw->list.SelectionPolicy != XmEXTENDED_SELECT)
    return;

  /* CR 6182:  Let actions work from accelerators. */
  if ((XtWindow((Widget) lw) == event->xany.window) &&
      !lw->list.Traversing)
    return;

  lw->list.AppendInProgress = TRUE;
  lw->list.Event |= (SHIFTDOWN | CTRLDOWN);
  lw->list.SelectionType = XmMODIFICATION;
  PrevElement(lw, event, params, num_params);
  lw->list.Event = 0;
  lw->list.AppendInProgress = FALSE;
}

/************************************************************************
 *									*
 * PrevPage - called when the user hits PgUp                            *
 *									*
 ************************************************************************/
static void
KbdPrevPage(Widget wid,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item, olditem, newtop;

  if (!(lw->list.items && lw->list.itemCount))
    return;
  if (lw->list.top_position == 0)
    return;
  if (!lw->list.Mom)
    return;

  newtop = lw->list.top_position - lw->list.visibleItemCount + 1;
  ASSIGN_MAX(newtop, 0);
  item = lw->list.CurrentKbdItem - lw->list.visibleItemCount + 1;
  ASSIGN_MAX(item, 0);
  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  olditem = lw->list.CurrentKbdItem;
  if (lw->list.vScrollBar)
    {
      lw->list.top_position = newtop;
      lw->list.CurrentKbdItem = item;
      if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	{
	  XPoint xmim_point;

	  GetPreeditPosition(lw, &xmim_point);
	  XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
	}
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
  else
    DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      HandleNewItem(lw, item, olditem);
    }
  else if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT)  ||
	   (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    HandleExtendedItem(lw, item);
}

/************************************************************************
 *									*
 * NextPage - called when the user hits PgDn                            *
 *									*
 ************************************************************************/
static void
KbdNextPage(Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item, olditem, newtop;

  if (!(lw->list.items && lw->list.itemCount))
    return;
  if (!lw->list.Mom)
    return;
  if (lw->list.top_position >=
      (lw->list.itemCount - lw->list.visibleItemCount))
    return;

  newtop = lw->list.top_position + (lw->list.visibleItemCount - 1);
  ASSIGN_MIN(newtop, (lw->list.itemCount - lw->list.visibleItemCount));
  item = lw->list.CurrentKbdItem + (lw->list.visibleItemCount - 1);
  if (item >= lw->list.itemCount)
    item = lw->list.itemCount - 1;
  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  olditem = lw->list.CurrentKbdItem;
  if (lw->list.vScrollBar)
    {
      lw->list.top_position = newtop;
      lw->list.CurrentKbdItem = item;
      if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	{
	  XPoint xmim_point;

	  GetPreeditPosition(lw, &xmim_point);
	  XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
	}
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
  else
    DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    {
      if (lw->list.AutoSelectionType == XmAUTO_UNSET)
	lw->list.AutoSelectionType = XmAUTO_CHANGE;
      HandleNewItem(lw, item, olditem);
    }
  else if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	   (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    HandleExtendedItem(lw, item);
}

/************************************************************************
 *                                                                      *
 * KbdLeftChar - called when user hits left arrow.                      *
 *                                                                      *
 ************************************************************************/
static void
KbdLeftChar(Widget wid,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int pos;

  if (!lw->list.Mom)
    return;

  if (LayoutIsRtoLP(lw)) {
    pos = lw->list.hOrigin + CHAR_WIDTH_GUESS;

    if ((lw->list.hExtent + pos) > lw->list.hmax)
      pos = lw->list.hmax - lw->list.hExtent;
  } else {
    pos = lw->list.hOrigin - CHAR_WIDTH_GUESS;
  }

  XmListSetHorizPos((Widget) lw, pos);
}

/************************************************************************
 *                                                                      *
 * KbdLeftPage - called when user hits ctrl left arrow.                 *
 *                                                                      *
 ************************************************************************/
static void
KbdLeftPage(Widget wid,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int pos;

  if (!lw->list.Mom)
    return;

  if (LayoutIsRtoLP(lw)) {
    pos = lw->list.hOrigin + (lw->core.width - CHAR_WIDTH_GUESS -
			      2 * (int)(lw->list.margin_width +
					lw->list.HighlightThickness +
					lw->primitive.shadow_thickness));
    if ((lw->list.hExtent + pos) > lw->list.hmax)
      pos = lw->list.hmax - lw->list.hExtent;
  } else {
    pos = lw->list.hOrigin - (lw->core.width - CHAR_WIDTH_GUESS -
			      2 * (int)(lw->list.margin_width +
					lw->list.HighlightThickness +
					lw->primitive.shadow_thickness));
  }
  XmListSetHorizPos((Widget) lw, pos);
}

/************************************************************************
 *                                                                      *
 * Begin Line - go to the beginning of the line                         *
 *                                                                      *
 ************************************************************************/
static void
BeginLine(Widget wid,
	  XEvent *event,
	  String *params,
	  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (!lw->list.Mom)
    return;

  XmListSetHorizPos((Widget) lw, 0);
}

/************************************************************************
 *                                                                      *
 * KbdRightChar - called when user hits right arrow.                    *
 *                                                                      *
 ************************************************************************/
static void
KbdRightChar(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int pos;

  if (!lw->list.Mom)
    return;

  if (LayoutIsRtoLP(lw)) {
    pos = lw->list.hOrigin - CHAR_WIDTH_GUESS;
  } else {
    pos = lw->list.hOrigin + CHAR_WIDTH_GUESS;

    if ((lw->list.hExtent + pos) > lw->list.hmax)
      pos = lw->list.hmax - lw->list.hExtent;
  }
  XmListSetHorizPos((Widget) lw, pos);
}

/************************************************************************
 *                                                                      *
 * KbdRightPage - called when user hits ctrl right arrow.               *
 *                                                                      *
 ************************************************************************/
static void
KbdRightPage(Widget wid,
	     XEvent *event,
	     String *params,
	     Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int pos;

  if (!lw->list.Mom)
    return;

  if (LayoutIsRtoLP(lw)) {
    pos = lw->list.hOrigin - (lw->core.width - CHAR_WIDTH_GUESS -
			      2 * (int)(lw->list.margin_width +
					lw->list.HighlightThickness +
					lw->primitive.shadow_thickness));
  } else {
    pos = lw->list.hOrigin + (lw->core.width - CHAR_WIDTH_GUESS -
			      2 * (int)(lw->list.margin_width +
					lw->list.HighlightThickness +
					lw->primitive.shadow_thickness));
    if ((lw->list.hExtent + pos) > lw->list.hmax)
      pos = lw->list.hmax - lw->list.hExtent;
  }

  XmListSetHorizPos((Widget) lw, pos);
}

/************************************************************************
 *                                                                      *
 * End Line - go to the end of the line                                 *
 *                                                                      *
 ************************************************************************/
static void
EndLine(Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  if (!lw->list.Mom)
    return;

  XmListSetHorizPos((Widget) lw, lw->list.hmax - lw->list.hExtent);
}

/************************************************************************
 *                                                                      *
 * TopItem - go to the top item                                         *
 *                                                                      *
 ************************************************************************/
static void
TopItem(Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int newtop;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  if (!lw->list.Mom)
    newtop = lw->list.top_position;
  else
    newtop = 0;

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  lw->list.CurrentKbdItem = newtop;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  XmListSetPos((Widget) lw, newtop + 1);
  if (lw->list.SelectionMode == XmNORMAL_MODE)
    XmListSelectPos((Widget) lw, newtop + 1, TRUE);
  lw->list.StartItem = newtop;
}

/************************************************************************
 *                                                                      *
 * EndItem - go to the bottom item                                      *
 *                                                                      *
 ************************************************************************/
static void
EndItem(Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int newbot;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  if (!lw->list.Mom)
    {
      newbot = (lw->list.top_position + lw->list.visibleItemCount - 1);
      if (newbot >= (lw->list.itemCount - 1))
	newbot = lw->list.itemCount - 1;
    }
  else
    newbot = lw->list.itemCount - 1;

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  lw->list.CurrentKbdItem = newbot;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  XmListSetBottomPos((Widget) lw, newbot + 1);
  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
  if (lw->list.SelectionMode == XmNORMAL_MODE)
    XmListSelectPos((Widget) lw, newbot + 1, TRUE);
}

/************************************************************************
 *                                                                      *
 * ExtendTopItem - Extend the selection to the top item			*
 *                                                                      *
 ************************************************************************/
static void
ExtendTopItem(Widget wid,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item, olditem;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  if ((lw->list.SelectionPolicy == XmBROWSE_SELECT) ||
      (lw->list.SelectionPolicy == XmSINGLE_SELECT))
    return;

  lw->list.Event |= (SHIFTDOWN);
  if (!lw->list.Mom)
    item = lw->list.top_position;
  else
    item = 0;

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  olditem = lw->list.CurrentKbdItem;
  lw->list.top_position = item;
  lw->list.CurrentKbdItem = item;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  DrawList(lw, NULL, TRUE);

  if (lw->list.vScrollBar)
    SetVerticalScrollbar(lw);

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    HandleNewItem(lw, item, olditem);
  else if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    HandleExtendedItem(lw, item);

  lw->list.Event = 0;
}

/************************************************************************
 *                                                                      *
 * ExtendEndItem - extend the selection to the bottom item		*
 *                                                                      *
 ************************************************************************/
static void
ExtendEndItem(Widget wid,
	      XEvent *event,
	      String *params,
	      Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item, newitem, olditem;

  if (!(lw->list.items && lw->list.itemCount))
    return;

  if ((lw->list.SelectionPolicy == XmBROWSE_SELECT) ||
      (lw->list.SelectionPolicy == XmSINGLE_SELECT))
    return;

  lw->list.Event |= (SHIFTDOWN);
  newitem = lw->list.itemCount - lw->list.visibleItemCount;
  ASSIGN_MAX(newitem, 0);
  item = lw->list.itemCount - 1;
  if (!lw->list.Mom)
    {
      newitem = lw->list.top_position;
      item = newitem + lw->list.visibleItemCount;
      if (item >= lw->list.itemCount)
	item = lw->list.itemCount - 1;
    }
  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  olditem = lw->list.CurrentKbdItem;
  lw->list.CurrentKbdItem = item;
  lw->list.top_position = newitem;

  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  DrawList(lw, NULL, TRUE);

  if (lw->list.vScrollBar)
    SetVerticalScrollbar(lw);

  if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
      (lw->list.SelectionPolicy == XmBROWSE_SELECT))
    HandleNewItem(lw, item, olditem);
  else if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    HandleExtendedItem(lw, item);

  lw->list.Event = 0;
}

/***************************************************************************
 *									   *
 * ListItemVisible - make the current keyboard item visible.  		   *
 *									   *
 ***************************************************************************/
static void
ListItemVisible(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  int item, percentage;

  if (!(lw->list.items && lw->list.itemCount))
    return;
  if (!lw->list.Mom)
    return;

  if (*num_params == 0)
    {
      item = WhichItem(lw, event->xbutton.y);
      if (item > 0)
	item -= lw->list.top_position;
      if ((item < 0) || (item >= lw->list.itemCount))
	return;
    }
  else
    {
      sscanf(*params, "%d", &percentage);
      if (percentage == 100) percentage--;
      item = (lw->list.visibleItemCount * percentage) /100;
    }

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);

  lw->list.top_position = lw->list.CurrentKbdItem - item;
  ASSIGN_MAX(lw->list.top_position, 0);

  DrawList(lw, NULL, TRUE);
  SetVerticalScrollbar(lw);
}

/***************************************************************************
 *									   *
 * ListCopyToClipboard - copy the current selected items to the clipboard. *
 *									   *
 *	This is a *sloow* process...					   *
 *									   *
 ***************************************************************************/
static void
ListCopyToClipboard(Widget wid,
		    XEvent *event,
		    String *params,
		    Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;

  /*
   * text to the clipboard */
  if (lw->list.selectedItemCount > 0)
    (void) XmeClipboardSource(wid, XmCOPY, 0);
}

static void
DragDropFinished(Widget w,		/* unused */
		 XtPointer closure,
		 XtPointer call_data)	/* unused */
{
  int i;
  XmListWidget lw = (XmListWidget)closure;
  XmListDragConvertStruct *ListDragConv = lw->list.drag_conv;

  for (i = 0; i < ListDragConv->num_strings; i++)
    XmStringFree(ListDragConv->strings[i]);

  XtFree((char *) ListDragConv->strings);
  XtFree((char *) ListDragConv);
}

/***************************************************************************
 *									   *
 * ListProcessDrag - drag the selected items				   *
 *									   *
 ***************************************************************************/
static void
ListProcessDrag(Widget wid,
		XEvent *event,
		String *params,		/* unused */
		Cardinal *num_params)	/* unused */
{
  XmListWidget lw = (XmListWidget) wid;
  int i;
  int item = 0;
  Widget drag_icon, dc;
  Arg args[10];
  int n, location_data;
  XmListDragConvertStruct *ListDragConv;

  /* Dequeue any pending drag initiation just to be safe. */
  if (lw->list.drag_start_timer)
    {
      XtRemoveTimeOut(lw->list.drag_start_timer);
      lw->list.drag_start_timer = 0;
      lw->list.drag_abort_action = NULL;
    }

  /* CR 5141: Don't allow multi-button drags. */
  if (event->xbutton.state &
      ~((Button1Mask >> 1) << event->xbutton.button) &
      (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))
    return;

  if (!(lw->list.items && lw->list.itemCount))
    return;
  item = WhichItem(lw, event->xbutton.y);
  if ((item < 0) || (item >= lw->list.itemCount))
    return;

  location_data = 0;

  lw->list.drag_conv = ListDragConv = (XmListDragConvertStruct *)
    XtMalloc(sizeof(XmListDragConvertStruct));

  ListDragConv->w = wid;

  if (lw->list.InternalList[item]->selected)
    {
      /* CR 8878: selectedItems may contain extraneous items. */
      ListDragConv->strings = (XmString *)
	XtMalloc(sizeof(XmString) * lw->list.selectedPositionCount);
      ListDragConv->num_strings = lw->list.selectedPositionCount;
      for (i = 0; i < lw->list.selectedPositionCount; i++) {
	ListDragConv->strings[i] =
	  XmStringCopy(lw->list.items[lw->list.selectedPositions[i] - 1]);
      }
    }
  else
    {
      ListDragConv->strings = (XmString *) XtMalloc(sizeof(XmString));
      ListDragConv->num_strings = 1;
      ListDragConv->strings[0] = XmStringCopy(lw->list.items[item]);
      location_data = item;
    }

  /* OK, now start the drag... */
  drag_icon = XmeGetTextualDragIcon(wid);

  n = 0;
  XtSetArg(args[n], XmNcursorForeground, lw->primitive.foreground), n++;
  XtSetArg(args[n], XmNcursorBackground, lw->core.background_pixel), n++;
  XtSetArg(args[n], XmNsourceCursorIcon, drag_icon), n++;
  XtSetArg(args[n], XmNdragOperations, XmDROP_COPY), n++;
  dc = XmeDragSource(wid, (XtPointer) (long) location_data, event, args, n);

  if (dc)
    XtAddCallback(dc, XmNdragDropFinishCallback, DragDropFinished, lw);
  else
    DragDropFinished(dc, lw, NULL);
}

/*
 * DragStart - begin a delayed drag.
 */
static void
DragStart(XtPointer closure,
	  XtIntervalId *id)	/* unused */
{
  XmListWidget lw = (XmListWidget) closure;

  lw->list.drag_start_timer = 0;
  lw->list.drag_abort_action = NULL;

  ListProcessDrag((Widget) lw, &lw->list.drag_event, NULL, NULL);
}

/************************************************************************
 *									*
 * ListProcessBtn1 - handle enableBtn1Transfer.  This action expects	*
 *	one parameter; the action to invoke if not dragging.  This is	*
 *	the "normal" behavior for Button1.				*
 *									*
 ************************************************************************/

static void
ListProcessBtn1(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XtEnum btn1_transfer;
  XmListWidget lw = (XmListWidget) wid;
  int item;
  XmDisplay dpy;

  if ((*num_params != 1) || !XmIsList(wid))
    return;

  dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  btn1_transfer = dpy->display.enable_btn1_transfer;
  switch (btn1_transfer)
    {
    case XmOFF:
      /* Invoke the normal action. */
      if (*num_params > 0)
        XtCallActionProc(wid, params[0], event, params, *num_params);
      break;

    case XmBUTTON2_ADJUST:
    case XmBUTTON2_TRANSFER:
      /* Invoke the normal action unless already considering dragging */
      /* or a ButtonPress over a selected item. */
      if (!lw->list.drag_start_timer &&
	  ((event->xany.type != ButtonPress) ||
	   ((item = WhichItem(lw, event->xbutton.y)) < 0) ||
	   (item >= lw->list.itemCount) ||
	   !OnSelectedList(lw, lw->list.items[item], item)))
	{
	  XtCallActionProc(wid, params[0], event, params, *num_params);
	}
      else
	{
	  switch(event->xany.type)
	    {
	    case ButtonPress:
	      /* Queue a drag on the first button press only. */
	      if ((!lw->list.drag_start_timer) &&
		  !(event->xbutton.state &
		    ~((Button1Mask >> 1) << event->xbutton.button) &
		    (Button1Mask | Button2Mask | Button3Mask |
		     Button4Mask | Button5Mask)))
		{
		  /* Delay starting the drag briefly. */
		  memcpy(&lw->list.drag_event, event,
			 sizeof(XButtonPressedEvent));
		  lw->list.drag_abort_action = params[0];
		  lw->list.drag_start_timer =
		    XtAppAddTimeOut(XtWidgetToApplicationContext(wid),
				    XtGetMultiClickTime(XtDisplay(wid)),
				    DragStart, (XtPointer) lw);
		}
	      else if (lw->list.drag_start_timer)
		{
		  XtRemoveTimeOut(lw->list.drag_start_timer);
		  lw->list.drag_start_timer = 0;

		  /* If we guessed wrong about starting a drag we should */
		  /* still attempt the original "normal" actions. */
		  XtCallActionProc(wid, lw->list.drag_abort_action,
				   &lw->list.drag_event, params, *num_params);
		  XtCallActionProc(wid, params[0], event, params, *num_params);
		  lw->list.drag_abort_action = NULL;
		}
	      break;

	    case ButtonRelease:
	      /* Cancel any pending drag. */
	      if (lw->list.drag_start_timer)
		{
		  XtRemoveTimeOut(lw->list.drag_start_timer);
		  lw->list.drag_start_timer = 0;

		  /* If we guessed wrong about starting a drag we should */
		  /* still attempt the original "normal" actions. */
		  XtCallActionProc(wid, lw->list.drag_abort_action,
				   &lw->list.drag_event, params, *num_params);
		  XtCallActionProc(wid, params[0], event, params, *num_params);
		  lw->list.drag_abort_action = NULL;
		}
	      break;

	    case MotionNotify:
	      /* Start a drag if we've moved far enough. */
	      if (lw->list.drag_start_timer)
		{
		  int dx = ((int)lw->list.drag_event.xbutton.x_root -
			    (int)event->xmotion.x_root);
		  int dy = ((int)lw->list.drag_event.xbutton.y_root -
			    (int)event->xmotion.y_root);

		  if ((ABS(dx) > MOTION_THRESHOLD) ||
		      (ABS(dy) > MOTION_THRESHOLD))
		    {
		      /* Start the drag now. */
		      if (lw->list.drag_start_timer)
			XtRemoveTimeOut(lw->list.drag_start_timer);
		      DragStart((XtPointer)lw, &lw->list.drag_start_timer);
		    }
		}
	      break;

	    default:
	      break;
	    }
	}
      break;

    default:
      assert(FALSE);
      break;
    }
}

/************************************************************************
 *									*
 * ListProcessBtn2 - handle enableBtn1Transfer.  This action expects	*
 *	one parameter; the action to invoke if not dragging.  This is	*
 *	the "alternate" behavior for Button2.				*
 *									*
 ************************************************************************/

static void
ListProcessBtn2(Widget wid,
		XEvent *event,
		String *params,
		Cardinal *num_params)
{
  XtEnum btn1_transfer;
  XmListWidget lw = (XmListWidget) wid;
  XmDisplay dpy;

  if ((*num_params != 1) || !XmIsList(wid))
    return;

  /* If there is a pending drag reject both actions. */
  if (lw->list.drag_start_timer)
    {
      XtRemoveTimeOut(lw->list.drag_start_timer);
      lw->list.drag_start_timer = 0;
      lw->list.drag_abort_action = NULL;
      return;
    }

  dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
  btn1_transfer = dpy->display.enable_btn1_transfer;
  switch (btn1_transfer)
    {
    case XmOFF:
    case XmBUTTON2_TRANSFER:
      /* Invoke the normal action by starting a drag immediately. */
      if (event->xany.type == ButtonPress)
	ListProcessDrag(wid, event, params, num_params);
      break;

    case XmBUTTON2_ADJUST:
      /* Invoke the alternate action. */
      XtCallActionProc(wid, params[0], event, params, *num_params);
      break;

    default:
      assert(FALSE);
      break;
    }
}

/***************************************************************************
 *									   *
 * ListQuickNavigate - navigate to an item				   *
 *									   *
 ***************************************************************************/
static void
ListQuickNavigate(Widget wid,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget) wid;
  char input_string[LIST_MAX_INPUT_SIZE + 1];
  int input_length;
  Status status_return;
  Boolean found = False;
  int i;
  wchar_t input_char;

  if (lw->list.matchBehavior != XmQUICK_NAVIGATE)
    return;

  /* Determine what was pressed. */
  input_length = XmImMbLookupString(wid, (XKeyEvent *) event, input_string,
				    LIST_MAX_INPUT_SIZE, (KeySym *) NULL,
				    &status_return);

  /* If there is more data than we can handle, bail out. */
  if (((status_return == XLookupChars) || (status_return == XLookupBoth)) &&
      (input_length > 0))
    {
      if (lw->list.itemCount > 0)
	{
	  /* Convert input to a wchar_t for easy comparison. */
	  (void) mbtowc(&input_char, NULL, 0);
	  (void) mbtowc(&input_char, input_string, input_length);

	  /* Search forward from the current position. */
	  for (i = lw->list.CurrentKbdItem + 1; i < lw->list.itemCount; i++)
	    if (CompareCharAndItem(lw, input_char, i))
	      {
		found = True;
		break;
	      }

	  /* Wrap around to the start of the list if necessary. */
	  if (!found)
	    {
	      for (i = 0; i <= lw->list.CurrentKbdItem; i++)
		if (CompareCharAndItem(lw, input_char, i))
		  {
		    found = True;
		    break;
		  }
	    }
	}

      if (!found)
	XBell(XtDisplay(wid), 0);
    }
}

/***************************************************************************
 *									   *
 * FirstChar - return the first wchar in an XmString.			   *
 *									   *
 ***************************************************************************/

static wchar_t
FirstChar(XmString string)
{
  /* This code is patterned on _XmStringGetTextConcat. */
  _XmStringContextRec stack_context;
  XmStringComponentType type;
  unsigned int len;
  XtPointer val;
  wchar_t result = 0;

  if (string != NULL)
    {
      memset(&stack_context, 0, sizeof stack_context);
      _XmStringContextReInit(&stack_context, string);

      (void) mbtowc(&result, NULL, 0);
      while((result == 0) &&
	    ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
					   &len, &val)) !=
	     XmSTRING_COMPONENT_END))
	{
	  switch( type)
	    {
	    case XmSTRING_COMPONENT_TEXT:
	    case XmSTRING_COMPONENT_LOCALE_TEXT:
	      if (len)
		(void) mbtowc(&result, (char*)val, len);
	      break;

	    case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	      if (len)
		result = *((wchar_t*) val);
	      break;

	    default:
	      break;
	    }
	}

      _XmStringContextFree(&stack_context);
    }

  return result;
}

/***************************************************************************
 *									   *
 * CompareCharAndItem 						   	   *
 *									   *
 ***************************************************************************/

static Boolean
CompareCharAndItem(XmListWidget lw,
		   wchar_t input_char,
		   int pos)
{
  if (lw->list.InternalList[pos]->first_char == 0)
    lw->list.InternalList[pos]->first_char = FirstChar(lw->list.items[pos]);

  if (input_char == lw->list.InternalList[pos]->first_char)
    {
      XmListSetKbdItemPos((Widget) lw, pos + 1);
      XmListSelectPos((Widget) lw, pos + 1, True);
      return True;
    }

  return False;
}

/***************************************************************************
 *									   *
 * ListConvert - Convert routine for dragNDrop.				   *
 *									   *
 ***************************************************************************/
static void
ListConvert(Widget w, XtPointer client_data,
	    XmConvertCallbackStruct *cs)
{
  enum { XmA_MOTIF_COMPOUND_STRING, XmACOMPOUND_TEXT, XmATEXT,
	 XmATARGETS, XmA_MOTIF_DROP, XmA_MOTIF_LOSE_SELECTION,
	 XmA_MOTIF_EXPORT_TARGETS, XmA_MOTIF_CLIPBOARD_TARGETS,
	 XmAUTF8_STRING,
	 NUM_ATOMS };
  static char *atom_names[] = {
    XmS_MOTIF_COMPOUND_STRING, XmSCOMPOUND_TEXT, XmSTEXT,
    XmSTARGETS, XmS_MOTIF_DROP, XmS_MOTIF_LOSE_SELECTION,
    XmS_MOTIF_EXPORT_TARGETS, XmS_MOTIF_CLIPBOARD_TARGETS,
    XmSUTF8_STRING };

  Atom atoms[XtNumber(atom_names)];
  Atom C_ENCODING = XmeGetEncodingAtom(w);
  int target_count = 0;
  int i;
  Atom type = None;
  XtPointer value = NULL;
  unsigned long size = 0;
  int format = 8;
  XmListWidget lw = (XmListWidget) w;
  XmListDragConvertStruct *ListDragConv = lw->list.drag_conv;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names), False, atoms);

  if (cs->target == atoms[XmATARGETS])
    {
      Atom *targs = XmeStandardTargets(w, 5, &target_count);

      value = (XtPointer) targs;
      targs[target_count++] = atoms[XmA_MOTIF_COMPOUND_STRING];
      targs[target_count++] = atoms[XmACOMPOUND_TEXT];
      targs[target_count++] = atoms[XmATEXT];
      targs[target_count++] = atoms[XmAUTF8_STRING];
      targs[target_count++] = C_ENCODING;
      if (XA_STRING != C_ENCODING)
	targs[target_count++] = XA_STRING;
      type = XA_ATOM;
      size = target_count;
      format = 32;
    }
  else if ((cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS]) ||
	   (cs->target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS]))
    {
      Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 5);
      int n = 0;

      value = (XtPointer) targs;
      targs[n++] = atoms[XmA_MOTIF_COMPOUND_STRING];
      targs[n++] = atoms[XmACOMPOUND_TEXT];
      targs[n++] = atoms[XmATEXT];
      targs[n++] = C_ENCODING;
      if (XA_STRING != C_ENCODING)
	targs[n++] = XA_STRING;
      format = 32;
      size = n;
      type = XA_ATOM;
      cs->status = XmCONVERT_DONE;
    }
  else if (cs->target == atoms[XmACOMPOUND_TEXT] ||
	   cs->target == atoms[XmA_MOTIF_COMPOUND_STRING] ||
	   cs->target == XA_STRING ||
	   cs->target == C_ENCODING ||
	   cs->target == atoms[XmATEXT] ||
	   cs->target == atoms[XmAUTF8_STRING])
    {
      XmString concat;
      XmString sep = XmStringSeparatorCreate();

      format = 8;

      if (cs->selection == atoms[XmA_MOTIF_DROP])
	{
	  int itemcount = ListDragConv->num_strings;
	  XmString *items = ListDragConv->strings;

	  concat = (itemcount ? XmStringCopy(items[0]) : NULL);
	  for (i = 1; i < itemcount; i++) {
	    concat = XmStringConcatAndFree(concat, XmStringCopy(sep));
	    concat = XmStringConcatAndFree(concat, XmStringCopy(items[i]));
	  }
	}
      else
	{
	  /* The selectedItems array may contain extraneous entries. */
	  int itemcount = lw->list.selectedPositionCount;
	  XmString *items = lw->list.items;
	  int *pos = lw->list.selectedPositions;

	  concat = (itemcount ? XmStringCopy(items[pos[0] - 1]) : NULL);
	  for (i = 1; i < itemcount; i++) {
	    concat = XmStringConcatAndFree(concat, XmStringCopy(sep));
	    concat = XmStringConcatAndFree(concat,
					   XmStringCopy(items[pos[i] - 1]));
	  }
	}

      if (cs->target == atoms[XmACOMPOUND_TEXT] ||
	  cs->target == C_ENCODING ||
	  cs->target == XA_STRING ||
	  cs->target == atoms[XmATEXT])
	{
	  if (concat != NULL)
	    value = XmCvtXmStringToCT(concat);
	  else
	    value = NULL;

	  type = atoms[XmACOMPOUND_TEXT];

	  if (value != NULL)
	    size = strlen((char*) value);
	  else
	    size = 0;

	  if (cs->target == XA_STRING)
	    {
	      XTextProperty tmp_prop;
	      int ret_status;

	      /* convert value to 8859.1 */
	      ret_status = XmbTextListToTextProperty(XtDisplay(w),
						     (char**) &value, 1,
						     (XICCEncodingStyle)
						     XStringStyle,
						     &tmp_prop);
	      XtFree((char*) value);
	      if (ret_status == Success || ret_status > 0)
		{
		  value = (XtPointer) tmp_prop.value;
		  type = XA_STRING;
		  size = tmp_prop.nitems;
		}
	      else
		{
		  value = NULL;
		  size = 0;
		}

	      /* If the target was TEXT then try and convert it.  If
	       * fully converted then we'll pass it back in locale
	       * text.  For locale text requests,  always pass
	       * back the converted text */
	    }
	  else if ((cs->target == atoms[XmATEXT] || cs->target == C_ENCODING) &&
		   (value != NULL))
	    {
	      char *cvt;
	      Boolean success;
	      cvt = _XmTextToLocaleText(w, value, type, format, size, &success);
	      if ((cvt != NULL && success) || cs->target == C_ENCODING)
		{
		  if (! success && cvt != NULL)
		    cs->flags |= XmCONVERTING_PARTIAL;
		  XtFree((char*) value);
		  value = cvt;
		  type = C_ENCODING;
		}
	    }
	}
#if XM_UTF8
      else if (cs->target == atoms[XmAUTF8_STRING])
	{
	    type = atoms[XmAUTF8_STRING];
	    value = XmCvtXmStringToUTF8String(concat);
	    if (value != NULL)
	        size = strlen((char*) value);
	    else
	        size = 0;
	}
#endif
      else
	{
	  size = XmCvtXmStringToByteStream(concat, (unsigned char**) &value);
	  type = atoms[XmA_MOTIF_COMPOUND_STRING];
	}

      XmStringFree(concat);
      XmStringFree(sep);
    }
  else if (cs->target == atoms[XmA_MOTIF_LOSE_SELECTION])
    {
      /* Deselect everything in the list since we lost
	 the primary selection. */
      XmListDeselectAllItems(w);
    }

  _XmConvertComplete(w, value, size, format, type, cs);
}

static void
ListPreDestProc(Widget w,
		XtPointer ignore, /* unused */
		XmDestinationCallbackStruct *cs)
{
  XmDropProcCallbackStruct *ds;
  Atom XA_MOTIF_DROP = XInternAtom(XtDisplay(w), XmS_MOTIF_DROP, False);
  int index;

  if (cs->selection != XA_MOTIF_DROP) return;

  /* If this is the result of a drop,  we can fill in location_data with
   * the apparent site */
  ds = (XmDropProcCallbackStruct *) cs->destination_data;

  index = XmListYToPos(w, ds->y);

  cs->location_data = (XtPointer) (long) index;
}

/************************************************************************
 *									*
 * Spiffy API entry points						*
 *									*
 ************************************************************************/

/************************************************************************
 *									*
 * XmListAddItem - add the item at the specified position.		*
 *									*
 ************************************************************************/

void
XmListAddItem(Widget w,
	      XmString item,
	      int pos)
{
  XmListWidget lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  APIAddItems(lw, &item, 1, pos, TRUE);
  _XmAppUnlock(app);
}

/***************************************************************************
 *									   *
 * APIAddItems - do all the work for the XmListAddItems and		   *
 * AddItemsUnselected functions.					   *
 *									   *
 ***************************************************************************/

static void
APIAddItems(XmListWidget lw,
	    XmString *items,
	    int item_count,
	    int pos,
	    Boolean select)
{
  int intern_pos = pos - 1;
  Boolean bot = FALSE;
  Boolean change_managed;
  Boolean selectable;
  int i;
  int nsel = lw->list.selectedPositionCount;
  Dimension old_max_height = lw->list.MaxItemHeight;

  if ((items == NULL) ||
      (item_count == 0))
    return;

  if ((intern_pos < 0) || (intern_pos >= lw->list.itemCount))
    {
      intern_pos = lw->list.itemCount;
      pos = lw->list.itemCount + 1;
      bot = TRUE;
    }

  if ((lw->list.Traversing) &&
      (intern_pos <= lw->list.CurrentKbdItem) &&
      !bot)
    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);

  selectable = (select &&
		(lw->list.SelectionPolicy != XmSINGLE_SELECT) &&
		(lw->list.SelectionPolicy != XmBROWSE_SELECT));

  AddItems(lw, items, item_count, intern_pos);
  nsel += AddInternalElements(lw, lw->list.items + intern_pos, item_count,
			      pos, selectable);

  if ((intern_pos <= lw->list.CurrentKbdItem) &&
      (lw->list.itemCount > 1) &&
      !bot)
    {
      lw->list.CurrentKbdItem += item_count;
      /* CR 5804:  Don't check lw->list.AutoSelect here. */
      if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	  (lw->list.SelectionPolicy == XmBROWSE_SELECT))
	lw->list.LastHLItem += item_count;
      if (lw->list.matchBehavior == XmQUICK_NAVIGATE) {
	XPoint xmim_point;

	GetPreeditPosition(lw, &xmim_point);
	XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
      }
    }

  /* CR 5833: Enforce single/browse selection_policy. */
  if (select && !selectable)
    {
      /* Find the last matching item. */
      assert(lw->list.selectedPositionCount <= 1);
      i = item_count;
      while (i-- > 0)
	{
	  if (OnSelectedList(lw, items[i], intern_pos + i))
	    {
	      /* Select the last of the matching new items. */
	      lw->list.InternalList[intern_pos + i]->selected = TRUE;
	      lw->list.InternalList[intern_pos + i]->last_selected = TRUE;
	      lw->list.InternalList[intern_pos + i]->LastTimeDrawn = FALSE;
	      nsel++;

	      /* Deselect the previous selected item. */
	      if (lw->list.selectedPositionCount > 0)
		{
		  int old_sel = lw->list.selectedPositions[0];
		  if (old_sel >= pos)
		    old_sel += item_count;

		  lw->list.InternalList[old_sel - 1]->selected = FALSE;
		  lw->list.InternalList[old_sel - 1]->last_selected = FALSE;
		  nsel--;

		  if (old_sel <= (lw->list.top_position +
				  lw->list.visibleItemCount))
		    DrawItem((Widget) lw, old_sel - 1);

		  UpdateSelectedList(lw, TRUE);
		}

	      break;
	    }
	}
    }

  /* CR 9443: Inserting before selected items requires update too. */
  if (select || (nsel != lw->list.selectedPositionCount) ||
      (nsel && (intern_pos < lw->list.selectedPositions[nsel - 1])))
    UpdateSelectedPositions(lw, nsel);

  if (intern_pos < (lw->list.top_position + lw->list.visibleItemCount))
    DrawList(lw, NULL, TRUE);

  SetNewSize(lw, False, False, old_max_height);
  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  change_managed = SetVerticalScrollbar(lw);
  if (change_managed && lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);

  /* SetTraversal(lw); */
}

/************************************************************************
 *									*
 * XmListAddItems - add the items starting at the specified position.   *
 *									*
 ************************************************************************/

void
XmListAddItems(Widget w,
	       XmString *items,
	       int item_count,
	       int pos)
{
  XmListWidget lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  APIAddItems(lw, items, item_count, pos, TRUE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListAddItemsUnselected - add the items starting at the specified   *
 *   The selected List is not checked.					*
 *									*
 ************************************************************************/

void
XmListAddItemsUnselected(Widget w,
			 XmString *items,
			 int item_count,
			 int pos)
{
  XmListWidget lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  APIAddItems(lw, items, item_count, pos, FALSE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListAddItemUnselected - add the item at the specified position.	*
 *     This does not check the selected list - the item is assumed to 	*
 *     be unselected.							*
 *									*
 ************************************************************************/

void
XmListAddItemUnselected(Widget w,
			XmString item,
			int pos)
{
  XmListWidget lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  APIAddItems(lw, &item, 1, pos, FALSE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeleteItem - delete the specified item from the list.		*
 *									*
 ************************************************************************/

void
XmListDeleteItem(Widget w,
		 XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  int item_pos;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1)
    {
      XmeWarning((Widget) lw, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  item_pos = ItemNumber(lw, item);
  if (item_pos < 1 || item_pos > lw->list.itemCount)
    {
      XmeWarning((Widget) lw, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  APIDeletePositions(lw, &item_pos, 1, TRUE);
  _XmAppUnlock(app);
}

/************************************************************************
 *                                                                      *
 * CleanUpList - redraw the list if the items go to 0, and check for    *
 *   traversal locations.                                               *
 *   ** NOTE: CAN ONLY BE USED FROM API DELETE ROUTINES **              *
 *                                                                      *
 ************************************************************************/

static void
CleanUpList(XmListWidget lw,
	    Boolean always)
{
  Dimension VertMargin, HorzMargin;

  /* Special case for deleting the last item */
  if (always || !lw->list.itemCount)
    {
      HorzMargin = lw->list.margin_width + lw->primitive.shadow_thickness;
      VertMargin = lw->list.margin_height + lw->primitive.shadow_thickness;

      if (XtIsRealized((Widget)lw))
	XClearArea(XtDisplay (lw), XtWindow (lw),
		   HorzMargin,
		   VertMargin,
		   lw->core.width - (2 * HorzMargin),
		   lw->core.height - (2 * VertMargin),
		   False);
    }
}

/************************************************************************
 *									*
 * XmListDeleteItems - delete the specified items from the list.	*
 *									*
 ************************************************************************/

void
XmListDeleteItems(Widget w,
		  XmString *items,
		  int item_count)
{
  XmListWidget lw = (XmListWidget) w;
  Boolean redraw = FALSE;
  Boolean reset_width = FALSE;
  Boolean reset_height = FALSE;
  Boolean rebuild_selection = FALSE;
  Dimension old_max_height = lw->list.MaxItemHeight;
  int item_pos;
  XmString *copy;

  int i;
  _XmWidgetToAppContext(w);

  if ((items == NULL) || (item_count == 0))
    return;

  _XmAppLock(app);

  if (lw->list.itemCount < 1)
    {
      XmeWarning((Widget) lw, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  /* Make a copy of items in case of XmNitems from w */
  copy = (XmString *)ALLOCATE_LOCAL(item_count * sizeof(XmString));
  for (i = 0; i < item_count; i++)
    copy[i] = XmStringCopy(items[i]);

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);

  for (i = 0; i < item_count; i++)
    {
      item_pos = ItemNumber(lw, copy[i]);
      if (item_pos < 1 || item_pos > lw->list.itemCount)
	XmeWarning(w, ListMessage8);
      else
        {
	  if (lw->list.CurrentKbdItem >= (item_pos - 1))
	    {
	      /* CR 8444: Don't let CurrentKbdItem go negative. */
	      if (lw->list.CurrentKbdItem > 0)
		lw->list.CurrentKbdItem--;
	    }

	  /* Fix for 2798 - If the LastHLItem is the item that
	   * has been deleted, decrement the LastHLItem. */
	  if ((lw->list.LastHLItem > 0) &&
	      (lw->list.LastHLItem == (item_pos - 1)))
	    lw->list.LastHLItem--;
	  /* End Fix 2798 */

	  /* change < to <= since item_pos starts with 1 not 0 */
	  if (item_pos <= (lw->list.top_position + lw->list.visibleItemCount))
	    redraw = TRUE;

	  reset_width |= (lw->list.InternalList[item_pos - 1]->width >=
			  lw->list.MaxWidth);
	  reset_height |= (lw->list.InternalList[item_pos - 1]->height >=
			   lw->list.MaxItemHeight);

	  DeleteItems(lw, 1, item_pos - 1);
	  rebuild_selection |= DeleteInternalElements(lw, NULL, item_pos, 1);
        }
    }

  UpdateSelectedList(lw, rebuild_selection);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);

  if (lw->list.itemCount)
    {
      if ((lw->list.itemCount - lw->list.top_position) <
	  lw->list.visibleItemCount)
        {
	  lw->list.top_position =
	    lw->list.itemCount - lw->list.visibleItemCount;
	  ASSIGN_MAX(lw->list.top_position, 0);
	  redraw = TRUE;
        }
    }
  else
    lw->list.top_position = 0;

  if (redraw)
    DrawList(lw, NULL, TRUE);

  CleanUpList(lw, False);

  /* We may not really need to reset width or height. */
  if (reset_width && lw->list.itemCount &&
      (lw->list.InternalList[0]->width >= lw->list.MaxWidth))
    reset_width = FALSE;
  if (reset_height && lw->list.itemCount &&
      (lw->list.InternalList[0]->height >= lw->list.MaxItemHeight))
    reset_height = FALSE;

  SetNewSize(lw, reset_width, reset_height, old_max_height);

  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);

  /* Free memory for copied list. */
  for (i = 0; i < item_count; i++)
    XmStringFree(copy[i]);
  DEALLOCATE_LOCAL((char *)copy);
  _XmAppUnlock(app);
}

static void
APIDeletePositions(XmListWidget lw,
		   int *positions,
		   int count,
		   Boolean track_kbd)
{
  Boolean redraw = FALSE;
  Boolean rebuild_selection;
  Boolean UpdateLastHL;
  int item_pos;
  int oldItemCount;
  int old_kbd = lw->list.CurrentKbdItem;
  Dimension old_max_height = lw->list.MaxItemHeight;

  int i;

  if ((positions == NULL) || (count == 0)) return;

  /* CR 5760:  Generate warnings for empty lists too. */
  if (lw->list.itemCount < 1)
    {
      XmeWarning((Widget)lw, ListMessage8);
      return;
    }

  /* CR 5804:  Don't check lw->list.AutoSelect here. */
  UpdateLastHL = ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
		  (lw->list.SelectionPolicy == XmBROWSE_SELECT));

  DrawHighlight (lw, lw->list.CurrentKbdItem, FALSE);

  /* Save itemCount because DeleteItemPositions recomputes value. */
  oldItemCount = lw->list.itemCount;

  for (i = 0; i < count; i++)
    {
      item_pos = positions[i];
      if ((item_pos < 1) || (item_pos > lw->list.itemCount))
        {
	  XmeWarning((Widget) lw, ListMessage8);
	  item_pos = positions[i] = -1;   /* mark position to be ignored */
        }
      else if (item_pos <= (lw->list.top_position + lw->list.visibleItemCount))
	redraw = TRUE;
    }

  DeleteItemPositions(lw, positions, count, track_kbd);
  rebuild_selection =
    DeleteInternalElementPositions(lw, positions, count, oldItemCount);

  if (lw->list.CurrentKbdItem >= lw->list.LastItem)
    {
      lw->list.CurrentKbdItem = lw->list.LastItem;
      ASSIGN_MAX(lw->list.CurrentKbdItem, 0);
      if (UpdateLastHL)
	lw->list.LastHLItem = lw->list.CurrentKbdItem;
    }

  UpdateSelectedList(lw, rebuild_selection);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);

  if (lw->list.itemCount)
    {
      if ((lw->list.itemCount - lw->list.top_position) <
	  lw->list.visibleItemCount)
        {
	  lw->list.top_position = lw->list.itemCount-lw->list.visibleItemCount;
	  ASSIGN_MAX(lw->list.top_position, 0);
	  redraw = TRUE;
        }
    }

  if ((lw->list.matchBehavior == XmQUICK_NAVIGATE) &&
      (redraw || (old_kbd != lw->list.CurrentKbdItem)))
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }

  if (redraw)
    DrawList(lw, NULL, TRUE);

  CleanUpList(lw, False);

  SetNewSize(lw, False, False, old_max_height);

  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);
}

/************************************************************************
 *									*
 * XmListDeletePositions - delete the specified positions from the list *
 *									*
 ************************************************************************/

void
XmListDeletePositions(Widget    w,
		      int      *position_list,
		      int       position_count)
{
  XmListWidget lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  APIDeletePositions(lw, position_list, position_count, FALSE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeletePos - delete the item at the specified position from the	*
 *	list.								*
 *									*
 ************************************************************************/

void
XmListDeletePos(Widget w,
		int pos)
{
  XmListWidget lw = (XmListWidget) w;
  int position;

  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  /* CR 9444: Allow 0 even though XmListDeletePositions doesn't. */
  position = (pos ? pos : lw->list.itemCount);

  APIDeletePositions(lw, &position, 1, TRUE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeleteItemsPos - delete the items at the specified position    *
 * from the list.							*
 *									*
 ************************************************************************/

void
XmListDeleteItemsPos(Widget w,
		     int item_count,
		     int pos)
{
  XmListWidget lw = (XmListWidget) w;
  int item_pos, last, new_top, old_kbd;
  Boolean reset_width = FALSE;
  Boolean reset_height = FALSE;
  Boolean rebuild_selection = FALSE;
  Dimension old_max_height;
  int i;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  old_max_height = lw->list.MaxItemHeight;

  /* CR 7270:  Deleting zero items is not an error. */
  if (item_count == 0) {
    _XmAppUnlock(app);
    return;
  }

  /* CR 5760:  Generate warnings for empty lists too. */
  if ((lw->list.itemCount < 1) || (item_count < 0))
    {
      XmeWarning(w, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  item_pos = pos - 1;

  if ((item_pos < 0)  ||
      (item_pos >= lw->list.itemCount))
    {
      XmeWarning((Widget) lw, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  if ((item_pos + item_count) >= lw->list.itemCount)
    item_count = lw->list.itemCount - item_pos;

  if (lw->list.Traversing)
    DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  old_kbd = lw->list.CurrentKbdItem;

  /****************
   *
   * Delete the elements. Because of the way the internal routines
   * work (they ripple up the list items after each call), we keep
   * deleting the "same" element for item_count times.
   *
   ****************/
  for (i = 0; i < item_count; i++)
    {
      reset_width |= (lw->list.InternalList[item_pos + i]->width >=
		      lw->list.MaxWidth);
      reset_height |= (lw->list.InternalList[item_pos + i]->height >=
		       lw->list.MaxItemHeight);
    }
  DeleteItems(lw, item_count, item_pos);
  rebuild_selection |= DeleteInternalElements(lw, NULL, pos, item_count);

  if (item_pos <= lw->list.CurrentKbdItem)
    {
      lw->list.CurrentKbdItem -= item_count;
      ASSIGN_MAX(lw->list.CurrentKbdItem, 0);

      if ((lw->list.SelectionPolicy == XmEXTENDED_SELECT) ||
	  (lw->list.SelectionPolicy == XmBROWSE_SELECT))
	lw->list.LastHLItem = lw->list.CurrentKbdItem;

      if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	{
	  XPoint xmim_point;

	  GetPreeditPosition(lw, &xmim_point);
	  XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
	}
    }

  UpdateSelectedList(lw, rebuild_selection);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);

  last = lw->list.top_position + lw->list.visibleItemCount;
  new_top = lw->list.top_position;
  if (lw->list.itemCount)
    {
      if (item_pos < new_top)
	{
	  new_top-= item_count;
	  ASSIGN_MAX(new_top, 0);
	}
      else if (item_pos < last)
	{
	  /* CR 5080 - Do not let new_top go negative.  Very, very bad
	   *                 things happen when it does. */
	  if ((last > lw->list.itemCount) &&
	      (new_top > 0))
	    {
	      new_top -= item_count;
	      ASSIGN_MAX(new_top, 0);
	    }
	}

      if (lw->list.top_position != new_top)
        {
	  DrawHighlight(lw, old_kbd, FALSE);
	  lw->list.top_position = new_top;
	  DrawList(lw, NULL, TRUE);
        }
      else if (item_pos < last)
	DrawList(lw, NULL, TRUE);
    }
  else
    lw->list.top_position = 0;

  CleanUpList(lw, False);

  /* We may not really need to reset width or height. */
  if (reset_width && lw->list.itemCount &&
      (lw->list.InternalList[0]->width >= lw->list.MaxWidth))
    reset_width = FALSE;
  if (reset_height && lw->list.itemCount &&
      (lw->list.InternalList[0]->height >= lw->list.MaxItemHeight))
    reset_height = FALSE;

  SetNewSize(lw, reset_width, reset_height, old_max_height);

  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);
  _XmAppUnlock(app);
}

/************************************************************************
 *                                                                      *
 * XmListDeleteAllItems - clear the list.                               *
 *                                                                      *
 ************************************************************************/

void
XmListDeleteAllItems(Widget w)
{
  XmListWidget lw = (XmListWidget) w;
  int j;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.items && (lw->list.itemCount > 0))
    {
      XmListDeselectAllItems(w);
      SetSelectionParams(lw);

      Dimension old_max_height = lw->list.MaxItemHeight;
      DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      j = lw->list.itemCount;
      lw->list.itemCount = 0;
      DeleteInternalElements(lw, NULL, 1, j);
      lw->list.itemCount = j;
      ClearItemList(lw);
      ClearSelectedList(lw);
      ClearSelectedPositions(lw);
      CleanUpList(lw, False);
      SetNewSize(lw, True, True, old_max_height);
      if (lw->list.SizePolicy != XmVARIABLE)
	SetHorizontalScrollbar(lw);
      SetVerticalScrollbar(lw);
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * APIReplaceItems - replace the given items with new ones.             *
 *									*
 ************************************************************************/

static void
APIReplaceItems(Widget w,
		XmString *old_items,
		int item_count,
		XmString *new_items,
		Boolean select)
{
  int i, j;
  XmListWidget lw = (XmListWidget) w;
  Boolean      redraw = FALSE;
  Dimension    old_max_width = lw->list.MaxWidth;
  Dimension    old_max_height = lw->list.MaxItemHeight;
  Boolean      reset_width = FALSE;
  Boolean      reset_height = FALSE;
  Boolean      replaced_first = FALSE;
  int	       nsel = lw->list.selectedPositionCount;

  if ((old_items == NULL)     ||
      (new_items == NULL)     ||
      (lw->list.items == NULL)||
      (item_count == 0))
    return;

  for (i = 0; i < item_count; i++)
    {
      for (j = 1; j <= lw->list.itemCount; j++)
	{
	  if (XmStringCompare(lw->list.items[j - 1], old_items[i]))
	    {
	      if (j <= (lw->list.top_position + lw->list.visibleItemCount))
		redraw = TRUE;

	      replaced_first |= (j == 1);
	      reset_width |=
		(lw->list.InternalList[j - 1]->width == old_max_width);
	      reset_height |=
		(lw->list.InternalList[j - 1]->height == old_max_height);

	      ReplaceItem(lw, new_items[i], j);
	      nsel += ReplaceInternalElement(lw, j, select);
	    }
	}
    }

  if (select || (nsel != lw->list.selectedPositionCount))
    UpdateSelectedPositions(lw, nsel);

  reset_width &= (old_max_width == lw->list.MaxWidth);
  if (reset_width && !replaced_first &&
      (lw->list.InternalList[0]->width == lw->list.MaxWidth))
    reset_width = FALSE;
  reset_height &= (old_max_height == lw->list.MaxItemHeight);
  if (reset_height && !replaced_first &&
      (lw->list.InternalList[0]->height == lw->list.MaxItemHeight))
    reset_height = FALSE;
  if (reset_width && reset_height)
    ResetExtents(lw, False);

  if (redraw)
    DrawList(lw, NULL, TRUE);

  SetNewSize(lw, False, False, old_max_height);
  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);
}

/************************************************************************
 *									*
 * XmListReplaceItems - replace the given items with new ones.          *
 *									*
 ************************************************************************/

void
XmListReplaceItems(Widget w,
		   XmString *old_items,
		   int item_count,
		   XmString *new_items)
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  APIReplaceItems(w, old_items, item_count, new_items, TRUE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListReplaceItemsUnselected - replace the given items with new ones.*
 *									*
 ************************************************************************/

void
XmListReplaceItemsUnselected(Widget w,
			     XmString *old_items,
			     int item_count,
			     XmString *new_items)
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  APIReplaceItems(w, old_items, item_count, new_items, FALSE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * APIReplaceItemsPos - replace the given items with new ones.          *
 *									*
 ************************************************************************/

static void
APIReplaceItemsPos(Widget w,
		   XmString *new_items,
		   int item_count,
		   int position,
		   Boolean select)
{
  XmListWidget lw = (XmListWidget) w;
  int intern_pos;
  int i;
  Dimension old_max_width = lw->list.MaxWidth;
  Dimension old_max_height = lw->list.MaxItemHeight;
  Boolean reset_width = FALSE;
  Boolean reset_height = FALSE;
  int nsel = lw->list.selectedPositionCount;

  if ((position < 1)          ||
      (new_items == NULL)     ||
      (lw->list.items == NULL)||
      (item_count == 0))
    return;

  intern_pos = position - 1;

  if ((intern_pos + item_count) > lw->list.itemCount)
    item_count = lw->list.itemCount - intern_pos;

  for (i = 0; i < item_count; i++, position++)
    {
      reset_width |=
	(lw->list.InternalList[position - 1]->width == old_max_width);
      reset_height |=
	(lw->list.InternalList[position - 1]->height == old_max_height);

      ReplaceItem(lw, new_items[i], position);
	  if (lw->list.selectedItems && lw->list.selectedItemCount > 0)
	  {
		  BuildSelectedList(lw,TRUE);
		  nsel += ReplaceInternalElement(lw, position, select);
	  }
    }

  if (select || (nsel != lw->list.selectedPositionCount))
    UpdateSelectedPositions(lw, nsel);

  reset_width &= (old_max_width == lw->list.MaxWidth);
  if (reset_width && (position > 1) &&
      (lw->list.InternalList[0]->width == lw->list.MaxWidth))
    reset_width = FALSE;
  reset_height &= (old_max_height == lw->list.MaxItemHeight);
  if (reset_height && (position > 1) &&
      (lw->list.InternalList[0]->height == lw->list.MaxItemHeight))
    reset_height = FALSE;
  if (reset_width && reset_height)
    ResetExtents(lw, False);

  if (intern_pos < (lw->list.top_position + lw->list.visibleItemCount))
    DrawList(lw, NULL, TRUE);

  SetNewSize(lw, False, False, old_max_height);
  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);
}

/************************************************************************
 *									*
 * XmListReplaceItemsPos - replace the given items at the specified     *
 *      position with new ones.                                         *
 *									*
 ************************************************************************/

void
XmListReplaceItemsPos(Widget w,
		      XmString *new_items,
		      int item_count,
		      int position)
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  APIReplaceItemsPos( w, new_items, item_count, position, TRUE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListReplaceItemsPosUnselected - replace the given items at the     *
 *    specified position with new ones.                                 *
 *									*
 ************************************************************************/

void
XmListReplaceItemsPosUnselected(Widget w,
				XmString *new_items,
				int item_count,
				int position)
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  APIReplaceItemsPos( w, new_items, item_count, position, FALSE);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListReplacePositions - Replace a set of items based on a list of   *
 *			    positions.					*
 *									*
 ************************************************************************/

void
XmListReplacePositions(Widget    w,
		       int      *position_list,
		       XmString *item_list,
		       int       item_count)
{
  int item_pos;
  int i;
  XmListWidget lw = (XmListWidget) w;
  Boolean redraw = FALSE;
  Dimension old_max_width;
  Dimension old_max_height;
  Boolean reset_width = FALSE;
  Boolean reset_height = FALSE;
  Boolean replaced_first = FALSE;
  int nsel;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  old_max_width = lw->list.MaxWidth;
  old_max_height = lw->list.MaxItemHeight;
  nsel = lw->list.selectedPositionCount;

  /* CR 5760:  Generate warnings for empty lists too. */
  if ((lw->list.itemCount < 1) &&
      (position_list || item_list || item_count))
    {
      if (position_list || item_count)
	XmeWarning(w, ListMessage8);
      _XmAppUnlock(app);
      return;
    }

  if ((position_list  == NULL)  ||
      (item_list      == NULL)  ||
      (lw->list.items == NULL)  ||
      (item_count     == 0)) {
    _XmAppUnlock(app);
    return;
  }

  for (i = 0; i < item_count; i++)
    {
      item_pos = position_list[i];

      if (item_pos < 1 || item_pos > lw->list.itemCount)
	XmeWarning((Widget) lw, ListMessage8);
      else
        {
	  if (item_pos <= (lw->list.top_position + lw->list.visibleItemCount))
	    redraw = TRUE;

	  replaced_first |= (item_pos == 1);
	  reset_width |=
	    (lw->list.InternalList[item_pos - 1]->width == old_max_width);
	  reset_height |=
	    (lw->list.InternalList[item_pos - 1]->height == old_max_height);

	  ReplaceItem(lw, item_list[i], item_pos);
	  nsel += ReplaceInternalElement(lw, item_pos, TRUE);
        }
    }

  UpdateSelectedPositions(lw, nsel);

  reset_width &= (old_max_width == lw->list.MaxWidth);
  if (reset_width && !replaced_first &&
      (lw->list.InternalList[0]->width == lw->list.MaxWidth))
    reset_width = FALSE;
  reset_height &= (old_max_height == lw->list.MaxItemHeight);
  if (reset_height && !replaced_first &&
      (lw->list.InternalList[0]->height == lw->list.MaxItemHeight))
    reset_height = FALSE;
  if (reset_width || reset_height)
    ResetExtents(lw, False);

  if (redraw)
    DrawList(lw, NULL, TRUE);

  SetNewSize(lw, False, False, old_max_height);
  if (lw->list.SizePolicy != XmVARIABLE)
    SetHorizontalScrollbar(lw);
  SetVerticalScrollbar(lw);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * APISelect - do the necessary selection work for the API select	*
 * routines								*
 *									*
 ************************************************************************/

static void
APISelect(XmListWidget lw,
	  int item_pos,
	  Boolean notify)
{
  int	i;

  /* Copy the current selection to the last selection */
  for (i = 0; i < lw->list.itemCount; i++)
    lw->list.InternalList[i]->last_selected =
      lw->list.InternalList[i]->selected;

  item_pos--;

  /* Unselect the previous selection if needed. */
  if (((lw->list.SelectionPolicy == XmSINGLE_SELECT) ||
       (lw->list.SelectionPolicy == XmBROWSE_SELECT) ||
       (lw->list.SelectionPolicy == XmEXTENDED_SELECT)))
    {
      for (i = 0; i < lw->list.selectedPositionCount; i++)
	{
	  int pos = lw->list.selectedPositions[i] - 1;
	  lw->list.InternalList[pos]->selected = FALSE;
	  DrawItem((Widget) lw, pos);
	}
    }
  if (lw->list.SelectionPolicy == XmEXTENDED_SELECT)
    lw->list.SelectionType = XmINITIAL;

  lw->list.InternalList[item_pos]->selected =
    ((lw->list.SelectionPolicy != XmMULTIPLE_SELECT) ||
     (!lw->list.InternalList[item_pos]->selected));

  DrawItem((Widget) lw, item_pos);
  lw->list.LastHLItem = item_pos;

  if (notify)
    {
      /* Some action functions eventually end up in this
       * function to select a single item and such (for instance,
       * TopItem and EndItem). Therefore, we need to setup some
       * auto selection stuff before invoking ClickElement. */
      if ((lw->list.AutoSelect != XmNO_AUTO_SELECT) &&
	  (lw->list.AutoSelectionType == XmAUTO_UNSET))
	{
	  if (ListSelectionChanged(lw))
	    lw->list.AutoSelectionType = XmAUTO_CHANGE;
	  else
	    lw->list.AutoSelectionType = XmAUTO_NO_CHANGE;
	}
      ClickElement(lw, NULL, FALSE);
    }
  else
    {
      UpdateSelectedList(lw, TRUE);
      UpdateSelectedPositions(lw, lw->list.selectedItemCount);
    }
}

/************************************************************************
 *                                                                      *
 * SetSelectionParams - update the selection parameters so that an API  *
 * selection looks the same as a user selection.                        *
 *                                                                      *
 ************************************************************************/

static void
SetSelectionParams(XmListWidget lw)
{
  int start, end, i;

  if (lw->list.items && lw->list.itemCount)
    {
      for (i = lw->list.itemCount - 1; i >= 0; i--)
	if (lw->list.InternalList[i]->selected)
	  {
	    end = i;
	    while (i && (lw->list.InternalList[i]->selected))
	      i--;

	    if ((i ==0) && (lw->list.InternalList[i]->selected))
	      start = i;
	    else
	      start = i + 1;

	    lw->list.OldEndItem = lw->list.EndItem;
	    lw->list.EndItem = end;
	    lw->list.OldStartItem = lw->list.StartItem;
	    lw->list.StartItem = start;
	    lw->list.LastHLItem = end;
	    if (lw->list.Traversing)
	      DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
	    lw->list.CurrentKbdItem = end;

	    if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
	      {
		XPoint xmim_point;

		GetPreeditPosition(lw, &xmim_point);
		XmImVaSetValues((Widget)lw, XmNspotLocation,
				&xmim_point, NULL);
	      }
	    if (lw->list.Traversing)
	      DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);
	    return;
	  }

      /* When we get here, there are no selected items in the list. */
      lw->list.OldEndItem = lw->list.EndItem;
      lw->list.EndItem = 0;
      lw->list.OldStartItem = lw->list.StartItem;
      lw->list.StartItem = 0;
      lw->list.LastHLItem = 0;
    }
}

/************************************************************************
 *									*
 * XmListSelectItem - select the given item and issue a callback if so	*
 * requested.								*
 *									*
 ************************************************************************/

void
XmListSelectItem(Widget w,
		 XmString item,
		 Boolean notify)
{
  XmListWidget  lw = (XmListWidget) w;
  int		   item_pos;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if ((item_pos = ItemNumber(lw, item)) != 0)
    {
      APISelect(lw, item_pos, notify);
      SetSelectionParams(lw);
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSelectPos - select the item at the given position and issue a  *
 * callback if so requested.						*
 *									*
 ************************************************************************/

void
XmListSelectPos(Widget w,
		int pos,
     		Boolean notify)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (pos >= 0 && pos <= lw->list.itemCount)
    {
      if (pos == 0)
	pos = lw->list.itemCount;
      APISelect(lw, pos, notify);
      SetSelectionParams(lw);
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeselectItem - deselect the given item and issue a callback if *
 * so requested.							*
 *									*
 ************************************************************************/

void
XmListDeselectItem(Widget w,
		   XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  int	i;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if ((i = ItemNumber(lw, item)) != 0)
    {
      i--;
      lw->list.InternalList[i]->last_selected = FALSE;
      if (lw->list.InternalList[i]->selected)
	{
	  lw->list.InternalList[i]->selected = FALSE;
	  UpdateSelectedList(lw, TRUE);
	  UpdateSelectedPositions(lw, lw->list.selectedItemCount);
	  DrawItem((Widget) lw, i);
	}
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeselectPos - deselect the item at the given position and issue*
 * a callback if so requested.						*
 *									*
 ************************************************************************/
void
XmListDeselectPos(Widget w,
		  int pos)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (pos >= 0 && pos <= lw->list.itemCount)
    {
      pos--;
      if (pos < 0)
	pos = lw->list.itemCount - 1;

      lw->list.InternalList[pos]->last_selected = FALSE;
      if (lw->list.InternalList[pos]->selected)
	{
	  lw->list.InternalList[pos]->selected = FALSE;
	  UpdateSelectedList(lw, TRUE);
	  UpdateSelectedPositions(lw, lw->list.selectedItemCount);
	  DrawItem((Widget) lw, pos);
	}
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListDeselectAllItems - hose the entire selected list		*
 *									*
 ************************************************************************/

void
XmListDeselectAllItems(Widget w)
{
  XmListWidget  lw = (XmListWidget) w;
  int  i;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (lw->list.selectedItemCount > 0)
    {
      for (i = 0; i < lw->list.selectedPositionCount; i++)
	{
	  int pos = lw->list.selectedPositions[i] - 1;

	  lw->list.InternalList[pos]->selected = FALSE;
	  lw->list.InternalList[pos]->last_selected = FALSE;
	  DrawItem((Widget) lw, pos);
	}

      ClearSelectedList(lw);
      ClearSelectedPositions(lw);
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSetPos - Make the specified position the top visible position	*
 * in the list.								*
 *									*
 ************************************************************************/

void
XmListSetPos(Widget w,
	     int pos)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (pos == 0)
    pos = lw->list.itemCount;

  if (pos > 0 && pos <= lw->list.itemCount)
    {
      pos--;
      if (lw->list.Traversing)
	DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = pos;
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSetBottomPos - Make the specified position the bottom visible 	*
 *                      position in the list.				*
 *									*
 ************************************************************************/

void
XmListSetBottomPos(Widget w,
		   int pos)
{
  XmListWidget  lw = (XmListWidget) w;
  int top;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (pos == 0)
    pos = lw->list.itemCount;

  if (pos > 0 && pos <= lw->list.itemCount)
    {
      top = pos - lw->list.visibleItemCount;
      ASSIGN_MAX(top, 0);
      if (top == lw->list.top_position) {
	_XmAppUnlock(app);
	return;
      }
      if (lw->list.Traversing)
	DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = top;
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
    _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSetItem - Make the specified item the top visible item 	*
 * in the list.								*
 *									*
 ************************************************************************/

void
XmListSetItem(Widget w,
	      XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  int	i;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if ((i = ItemNumber(lw, item)) != 0)
    {
      i--;
      if (i == lw->list.top_position) {
	_XmAppUnlock(app);
	return;
      }
      if (lw->list.Traversing)
	DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = i;
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
    _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSetBottomItem - Make the specified item the bottom visible 	*
 *                      position in the list.				*
 *									*
 ************************************************************************/

void
XmListSetBottomItem(Widget w,
		    XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  int i, top;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if ((i = ItemNumber(lw, item)) != 0)
    {
      top = i - lw->list.visibleItemCount;
      ASSIGN_MAX(top, 0);
      if (top == lw->list.top_position) {
	_XmAppUnlock(app);
	return;
      }
      if (lw->list.Traversing)
	DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
      lw->list.top_position = top;
      DrawList(lw, NULL, TRUE);
      SetVerticalScrollbar(lw);
    }
    _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListSetAddMode - Programatically update XmNselectionMode value     *
 *									*
 ************************************************************************/

void
XmListSetAddMode(Widget w,
     		 Boolean add_mode)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  /*  Can't be false for single or multiple */
  if ((!add_mode) &&
      ((lw->list.SelectionPolicy == XmSINGLE_SELECT) ||
       (lw->list.SelectionPolicy == XmMULTIPLE_SELECT))) {
    _XmAppUnlock(app);
    return;
  }

  /*  Can't be true for browse */
  if ((add_mode) && (lw->list.SelectionPolicy == XmBROWSE_SELECT)) {
    _XmAppUnlock(app);
    return;
  }

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  lw->list.SelectionMode = (add_mode ? XmADD_MODE : XmNORMAL_MODE);
  ChangeHighlightGC(lw, add_mode);
  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);

  /****************
   *
   * Funky hacks for Ellis: If we enter add mode with one item selected,
   * deselect the current one. If we leave add mode with no items selected,
   * select the current one.
   *
   * rgcote 8/23/93: Here's a little more background for the "Funky hacks".
   * In normal mode, one item must be selected at all times.  In add mode,
   * this is not the case. In add mode, a user wants to specify a set of
   * possibly discontiguous items to be the selection.  If, when going from
   * normal mode to add mode, more than one item is already selected then
   * we assume that the user started selecting the items and decided that
   * he/she needed to now select more items that are discontiguous from the
   * currently selected items.  On the other hand, if only one item was
   * selected, we assume that the single selected item is just the result
   * of normal mode forcing one item to be selected and we unselect that
   * single item.
   *
   ****************/
  if ((add_mode) &&
      (lw->list.itemCount != 0) &&
      (lw->list.SelectionPolicy == XmEXTENDED_SELECT) &&
      (lw->list.selectedPositionCount == 1) &&
      (lw->list.InternalList[lw->list.CurrentKbdItem]->selected))
    {
      lw->list.InternalList[lw->list.CurrentKbdItem]->selected = FALSE;
      lw->list.InternalList[lw->list.CurrentKbdItem]->last_selected = FALSE;
      DrawList(lw, NULL, TRUE);
      UpdateSelectedList(lw, TRUE);
      UpdateSelectedPositions(lw, lw->list.selectedItemCount);
    }
  else if ((!add_mode) &&
	   (lw->list.itemCount != 0) &&
	   (lw->list.SelectionPolicy == XmEXTENDED_SELECT) &&
	   (lw->list.selectedPositionCount == 0))
    {
      lw->list.InternalList[lw->list.CurrentKbdItem]->selected = TRUE;
      lw->list.InternalList[lw->list.CurrentKbdItem]->last_selected = TRUE;
      DrawList(lw, NULL, TRUE);
      UpdateSelectedList(lw, TRUE);
      UpdateSelectedPositions(lw, lw->list.selectedItemCount);
    }
    _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListItemExists - returns TRUE if the given item exists in the	*
 * list.								*
 *									*
 ************************************************************************/

Boolean
XmListItemExists(Widget w,
		 XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  Boolean exists;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return FALSE;
  }

  exists = ItemExists(lw, item);
  _XmAppUnlock(app);
  return exists;
}

/************************************************************************
 *									*
 * XmListItemPosition - returns the index (1-based) of the given item.  *
 * Returns 0 if not found.                                              *
 *									*
 ************************************************************************/

int
XmListItemPos(Widget w,
	      XmString item)
{
  XmListWidget  lw = (XmListWidget) w;
  int pos;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if (item == NULL) {
    _XmAppUnlock(app);
    return 0;
  }

  pos = ItemNumber(lw, item);
  _XmAppUnlock(app);
  return pos;
}

/************************************************************************
 *									*
 * XmListGetKbdItemPos - returns the index (1-based) of the current     *
 *                       keyboard item.                                 *
 * Returns 0 if not found.                                              *
 *									*
 ************************************************************************/

int
XmListGetKbdItemPos(Widget w)
{
  XmListWidget  lw = (XmListWidget) w;
  int pos;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if (lw->list.items == NULL) {
    _XmAppUnlock(app);
    return 0;
  }

  pos = lw->list.CurrentKbdItem + 1;
  _XmAppUnlock(app);
  return pos;
}

/************************************************************************
 *									*
 * XmListSetKbdItemPos - allows user to set the current keyboard item   *
 * Returns True  if successful.                                         *
 *         False if not                                                 *
 *									*
 ************************************************************************/

Boolean
XmListSetKbdItemPos(Widget w,
		    int    pos)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if ((lw->list.items == NULL)  ||
      (pos < 0)		  ||
      (pos > lw->list.itemCount)) {
    _XmAppUnlock(app);
    return (False);
  }

  if (pos == 0)
    pos = lw->list.itemCount;

  DrawHighlight(lw, lw->list.CurrentKbdItem, FALSE);
  lw->list.CurrentKbdItem = pos - 1;
  if (lw->list.matchBehavior == XmQUICK_NAVIGATE)
    {
      XPoint xmim_point;

      GetPreeditPosition(lw, &xmim_point);
      XmImVaSetValues((Widget)lw, XmNspotLocation, &xmim_point, NULL);
    }
  DrawHighlight(lw, lw->list.CurrentKbdItem, TRUE);

  MakeItemVisible(lw, lw->list.CurrentKbdItem); /* do we need to do this? */

  _XmAppUnlock(app);
  return TRUE;
}

/************************************************************************
 *									*
 * XmListGetMatchPos - returns the positions that an item appears at in *
 *	the list.  CALLER MUST FREE SPACE!                              *
 *									*
 ************************************************************************/

Boolean
XmListGetMatchPos(Widget w,
		  XmString item,
		  int **pos_list,
		  int *pos_count)
{
  XmListWidget  lw = (XmListWidget) w;
  int  i, *pos;
  int           j;
  _XmWidgetToAppContext(w);

  /* CR 7648: Be friendly and initialize the out parameters. */
  *pos_list = NULL;
  *pos_count = 0;

  _XmAppLock(app);

  if ((lw->list.items == NULL) ||
      (lw->list.itemCount <= 0)) {
    _XmAppUnlock(app);
    return FALSE;
  }

  pos = (int *)XtMalloc((sizeof(int) * lw->list.itemCount));
  j = 0;

  for (i = 0; i < lw->list.itemCount; i++)
    {
      if ((XmStringCompare(lw->list.items[i], item)))
	pos[j++] = (i + 1);
    }

  if (j == 0)
    {
      XtFree((char *)pos);
      _XmAppUnlock(app);
      return (FALSE);
    }
  pos = (int *)XtRealloc((char *) pos, (sizeof(int) * j));

  *pos_list = pos;
  *pos_count = j;

  _XmAppUnlock(app);
  return TRUE;
}

void
ListScrollUp(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
  XmListWidget  lw = (XmListWidget) wid;
  int value;
  int slider_size;
  int increment;
  int page_increment;
  if (lw->list.vScrollBar) {
    int new_value;
    XmScrollBarGetValues(((Widget)(lw->list.vScrollBar)), &value, &slider_size,
        &increment, &page_increment);
    new_value = value-increment;
    if (new_value < lw->list.vScrollBar->scrollBar.minimum)
      new_value = lw->list.vScrollBar->scrollBar.minimum;
    XmScrollBarSetValues(((Widget)(lw->list.vScrollBar)), new_value, slider_size,
        increment, page_increment, True);
  }
}

void
ListScrollDown(Widget wid, XEvent *event, String *params, Cardinal *num_params)
{
  XmListWidget lw = (XmListWidget)wid;
  int value;
  int slider_size;
  int increment;
  int page_increment;
  if (lw->list.vScrollBar) {
    int new_value;
    XmScrollBarGetValues(((Widget)(lw->list.vScrollBar)), &value, &slider_size,
        &increment, &page_increment);
    new_value = value+increment;
    if (new_value > lw->list.vScrollBar->scrollBar.maximum - slider_size)
      new_value = lw->list.vScrollBar->scrollBar.maximum - slider_size;
    XmScrollBarSetValues(((Widget)(lw->list.vScrollBar)), new_value, slider_size,
        increment, page_increment, True);
  }
}

/************************************************************************
 *									*
 * XmListGetSelectedPos - returns XmNselectedPositions and              *
 *	XmNselectedPositionCount.  CALLER MUST FREE SPACE!              *
 *									*
 ************************************************************************/

Boolean
XmListGetSelectedPos(Widget w,
		     int **pos_list,
		     int *pos_count)
{
  XmListWidget  lw = (XmListWidget) w;
  int *posList;
  int count;
  _XmWidgetToAppContext(w);

  /* CR 7648: Be friendly and initialize the out parameters. */
  *pos_list = NULL;
  *pos_count = 0;

  _XmAppLock(app);

  if (lw->list.items == NULL ||
      lw->list.itemCount <= 0 ||
      lw->list.selectedPositions == NULL ||
      lw->list.selectedPositionCount <= 0) {
    _XmAppUnlock(app);
    return FALSE;
  }

  posList = (int *) XtMalloc(sizeof(int) * lw->list.selectedPositionCount);
  count = lw->list.selectedPositionCount;

  memcpy((char*) posList,
	 (char*) lw->list.selectedPositions,
	 count * sizeof(int));

  *pos_list = posList;
  *pos_count = count;

  _XmAppUnlock(app);
  return TRUE;
}

/************************************************************************
 *									*
 * XmListSetHorizPos - move the hsb.					*
 *									*
 ************************************************************************/

void
XmListSetHorizPos(Widget w,
		  int position)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (lw->list.itemCount < 1) {
    _XmAppUnlock(app);
    return;
  }

  if (lw->list.hScrollBar)
    {
      ASSIGN_MAX(position, lw->list.hmin);

      if ((lw->list.hExtent + position) > lw->list.hmax)
	position = lw->list.hmax - lw->list.hExtent;

      if (position == lw->list.hOrigin) {
	_XmAppUnlock(app);
	return;
      }

      lw->list.hOrigin = position;
      lw->list.XOrigin = position;
      SetHorizontalScrollbar(lw);
      DrawList(lw, NULL, TRUE);
    }
  /* else
   *   XmeWarning((Widget) lw, ListMessage9);
   */
   _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListYToPos - return the index of the item underneath position Y    *
 *	returns 0 if there is no item at position Y.			*
 *									*
 ************************************************************************/

int
XmListYToPos(Widget w,
	     Position y)
{
  /* Remember to convert to the 1-based user world. */

  /* BEGIN OSF Fix CR 5081 */
  XmListWidget lw = (XmListWidget)w;
  int pos;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  /* BEGIN OSF Fix CR 5662 */
  if ((y < (Position)0) || (y >= (Position)(lw->core.height - lw->list.BaseY))) {
    /* END OSF Fix CR 5662 */
    _XmAppUnlock(app);
    return (0);
  }
  else {
    pos = WhichItem(lw, y) + 1;
    _XmAppUnlock(app);
    return pos;
  }
  /* END OSF Fix CR 5081 */
}

/************************************************************************
 *									*
 * XmListPosToBounds                                                    *
 *									*
 ************************************************************************/

Boolean
XmListPosToBounds(Widget      w,
		  int         position,
		  Position   *x,
		  Position   *y,
		  Dimension  *width,
		  Dimension  *height)
{
  XmListWidget lw;
  Dimension    ht;

  Position   ix;          /* values computed ahead...  */
  Position   iy;          /* ...of time...             */
  Dimension  iwidth;      /* ...for debugging...       */
  Dimension  iheight;     /* ...purposes               */
  _XmWidgetToAppContext(w);

  if (!XtIsRealized(w))
    return False;

  _XmAppLock(app);

  lw = (XmListWidget) w;

  /* BEGIN OSF Fix CR 5764 */
  /* Remember we're 0-based */
  if (position == 0)
    position = lw->list.itemCount - 1;
  else
    position--;
  /* END OSF Fix CR 5764 */

  if ((position >= lw->list.itemCount)    ||
      (position <  lw->list.top_position) ||
      (position >= (lw->list.top_position + lw->list.visibleItemCount))) {
    _XmAppUnlock(app);
    return (False);
  }

  ht = MAX((int)lw->list.HighlightThickness, 0);

  ix = lw->list.BaseX - ht;
  iwidth = lw->core.width - 2 * ((int)lw->list.margin_width +
				 lw->primitive.shadow_thickness);

  iy = LINEHEIGHTS(lw, position - lw->list.top_position) + lw->list.BaseY - ht;
  iheight = lw->list.MaxItemHeight + (2 * ht);

  if (x      != NULL)   *x      = ix;
  if (y      != NULL)   *y      = iy;
  if (height != NULL)   *height = iheight;
  if (width  != NULL)   *width  = iwidth;

  _XmAppUnlock(app);
  return True;
}

/************************************************************************
 *									*
 * XmListUpdateSelectedList - regen the selected items and positions    *
 *			      lists.					*
 *									*
 ************************************************************************/

void
XmListUpdateSelectedList(Widget w)
{
  XmListWidget  lw = (XmListWidget) w;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  UpdateSelectedList(lw, TRUE);
  UpdateSelectedPositions(lw, lw->list.selectedItemCount);
  _XmAppUnlock(app);
}

/************************************************************************
 *									*
 * XmListPosSelected - Return selection state of item at position	*
 *									*
 ************************************************************************/

Boolean
XmListPosSelected(Widget w,
		  int pos)
{
  int		int_pos;
  XmListWidget	lw = (XmListWidget) w;
  Boolean	selected;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if ((lw->list.items == NULL) || (pos < 0) || (pos > lw->list.itemCount)) {
    _XmAppUnlock(app);
    return False;
  }

  if (pos == 0)
    int_pos = lw->list.LastItem - 1;
  else
    int_pos = pos - 1;

  selected = lw->list.InternalList[int_pos]->selected;
  _XmAppUnlock(app);

  return selected;
}

/************************************************************************
 *									*
 * XmCreateList - hokey interface to XtCreateWidget.			*
 *									*
 ************************************************************************/

Widget
XmCreateList(Widget parent,
	     char *name,
	     ArgList args,
	     Cardinal argCount)
{
  return XtCreateWidget(name, xmListWidgetClass, parent, args, argCount);
}

Widget
XmVaCreateList(
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
                         xmListWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedList(
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
                         xmListWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/************************************************************************
 *									*
 * XmCreateScrolledList - create a list inside of a scrolled window.	*
 *									*
 ************************************************************************/

Widget
XmCreateScrolledList(Widget parent,
		     char *name,
		     ArgList args,
		     Cardinal argCount)
{
  Widget sw, lw;
  char *s;
  ArgList Args;
  Arg my_args[4];
  Cardinal nargs;

  s = (char*) ALLOCATE_LOCAL(XmStrlen(name) + 3); /* Name+"SW"+NULL */
  if (name)
    {
      strcpy(s, name);
      strcat(s, "SW");
    }
  else
    {
      strcpy(s, "SW");
    }

  nargs = 0;
  XtSetArg(my_args[nargs], XmNscrollingPolicy, XmAPPLICATION_DEFINED), nargs++;
  XtSetArg(my_args[nargs], XmNvisualPolicy, XmVARIABLE), nargs++;
  XtSetArg(my_args[nargs], XmNscrollBarDisplayPolicy, XmSTATIC), nargs++;
  XtSetArg(my_args[nargs], XmNshadowThickness, 0), nargs++;
  assert(nargs <= XtNumber(my_args));
  Args = XtMergeArgLists(args, argCount, my_args, nargs);
  sw = XtCreateManagedWidget(s , xmScrolledWindowWidgetClass, parent,
			     Args, argCount + nargs);
  DEALLOCATE_LOCAL(s);
  XtFree((char *) Args);

  lw = XtCreateWidget(name, xmListWidgetClass, sw, args, argCount);

  XtAddCallback (lw, XmNdestroyCallback, _XmDestroyParentCallback, NULL);
  return (lw);
}

