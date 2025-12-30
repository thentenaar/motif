/* $TOG: Container.c /main/35 1999/07/29 09:16:07 mgreess $ */
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

/*
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>

#include <Xm/ClipWindowP.h>
#include <Xm/ContItemT.h>
#include <Xm/ContainerP.h>
#include <Xm/ContainerT.h>
#include <Xm/DisplayP.h>
#include <Xm/DragDrop.h>
#include <Xm/DrawP.h>
#include <Xm/DrawingA.h>
#include <Xm/GadgetP.h>
#include <Xm/IconH.h>
#include <Xm/PrimitiveP.h>
#include <Xm/PushBGP.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/TravConT.h>
#include <Xm/CareVisualT.h>
#include <Xm/PointInT.h>
#include <Xm/VaSimpleP.h>
#include "ColorI.h"
#include "GadgetUtiI.h"
#include "GeoUtilsI.h"
#include "IconGI.h"
#include "RepTypeI.h"
#include "TransferI.h"
#include "TraversalI.h"
#include "XmI.h"
#include "CareVisualTI.h"
#include "MessagesI.h"
#include "ClipWindTI.h"
#include <Xm/XmosP.h>                /* for bzero et al */

#define	ZERO_DIM	0
#define	DEFAULT_INDENTATION	40
#define	NO_CELL	-1
#define	OBNAME 	"OutlineButton"
#define	HEADERNAME "Header"
#define	DANAME "HeaderDA"
#define	INVALID_COUNT   32767
#define	INVALID_DIMENSION 32767
#define MOTION_THRESHOLD 3
/* Make this nicely divisible by 2 */
#define DRAG_STATE_SIZE 14

#define	WRONGPARAMS	_XmMMsgContainer_0000
#define MESSAGE1        _XmMMsgContainer_0001

enum {	ANY_FIT,
	EXACT_FIT,
	FORCE};

/* Useful macros */
#define _LEFT   0
#define _RIGHT  1
#define _UP     2
#define _DOWN   3
#define _FIRST  4
#define _LAST   5
#define _COLLAPSE 2
#define _EXPAND	  3
#define _IN     "In"
#define _OUT    "Out"
#define _ENTER  "Enter"
#define _LEAVE  "Leave"
#define _LINK   0
#define _MOVE   1
#define _COPY   2

#define	defaultTranslations	_XmContainer_defaultTranslations
#define	traversalTranslations	_XmContainer_traversalTranslations


/********    Static Function Declarations    ********/
static	void                    GetDetailHeader(
					Widget		wid,
					int		offset,
					XtArgVal	*value);
static	void                    GetDetailHeaderCount(
					Widget		wid,
					int		offset,
				        XtArgVal	*value);
static 	void			GetOutlineColumnWidth(
					Widget		wid,
					int		offset,
					XtArgVal	*value);
static	void			ClassPartInitialize(
					WidgetClass	wc);
static	void			Initialize(
					Widget		rw,
					Widget		nw,
					ArgList		args,
					Cardinal	*num_args);
static  void                    Destroy(
					Widget		wid);
static  void                    Resize(
					Widget		wid);
static  void                    Redisplay(
					Widget		wid,
					XEvent		*event,
					Region		region);
static  Boolean                 SetValues(
					Widget		cw,
					Widget		rw,
					Widget		nw,
					ArgList		args,
					Cardinal	*num_args);
static  void                    GetSize(
					Widget                  wid,
					Dimension * pwidth,
					Dimension * pheight);
static  XtGeometryResult        QueryGeometry(
					Widget		wid,
					XtWidgetGeometry *intended,
					XtWidgetGeometry *desired);
static  XtGeometryResult        GeometryManager(
					Widget		instigator,
					XtWidgetGeometry *desired,
					XtWidgetGeometry *allowed);
static  void                    ChangeManaged(
					Widget		wid);
static	void			ChangeManagedOutlineDetail(
					Widget		wid);
static	void			ChangeManagedSpatial(
					Widget          wid);
static	Boolean			RequestSpatialGrowth(
					Widget		wid,
					Widget		cwid);
static  void                    ConstraintInitialize(
					Widget		rcwid,
					Widget		ncwid,
					ArgList		args,
					Cardinal	*num_args);
static  void                    ConstraintDestroy(
					Widget		cwid);
static  Boolean                 ConstraintSetValues(
					Widget		ccwid,
					Widget		rcwid,
					Widget		ncwid,
					ArgList		args,
					Cardinal	*num_args);
static  Widget                  ObjectAtPoint(
					       Widget wid,
					       Position x, Position y);
static	Boolean			TestFitItem(
					Widget		wid,
					Widget		cwid,
					Position	x,
					Position	y);
static  Boolean	                PlaceItem(
					Widget		wid,
					Widget		cwid,
					unsigned char	fit_type);
static	Boolean			RemoveItem(
					Widget		wid,
					Widget		cwid);
static	void	                GetSpatialSize(
					Widget		wid,
					Dimension * pwidth,
					Dimension * pheight);
static	void			GetSpatialSizeCellAdjustment(
					Widget		wid,
					int		*width_in_cells,
					int		*height_in_cells,
					ContainerCwidCellInfo cwid_info,
					int		cwid_info_count);
static  void                    PlaceItemNone(
					Widget		wid,
					Widget		cwid,
					unsigned char	fit_type);
static  void                    PlaceItemGridCells(
					Widget          wid,
					Widget          cwid,
					unsigned char	fit_type);
static	int			GetCellFromCoord(
					Widget		wid,
					Position	x,
					Position	y);
static	XPoint *		GetCoordFromCell(
					Widget		wid,
					int		cell_idx,
					XPoint 		*point);
static	void			PlaceItemReset(
					Widget		wid);
static	void			PlaceCwid(
					Widget		cwid,
					Position	x,
					Position	y);
static	XPoint *		SnapCwid(
					Widget          cwid,
					Position        x,
					Position        y,
					XPoint		*point);
static	void			HideCwid(
					Widget		cwid);
static  void			ContainerNoop(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn1Down(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn1Motion(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn1Up(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn2Down(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn2Motion(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void			ContainerHandleBtn2Up(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void                    ContainerBeginSelect(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static  void                    ContainerButtonMotion(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerEndSelect(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerBeginToggle(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerEndToggle(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerBeginExtend(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerEndExtend(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerStartTransfer(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static	void			ContainerEndTransfer(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerPrimaryCopy(
                                        Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerPrimaryLink(
                                        Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerPrimaryMove(
                                        Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerCancel(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerSelect(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerExtend(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerMoveCursor(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerExtendCursor(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerToggleMode(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerSelectAll(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerDeselectAll(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerActivate(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static 	void			ContainerExpandOrCollapse(
					Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static  void                    ContainerConvertProc(
					Widget		wid,
					XtPointer	closure,
					XmConvertCallbackStruct	*cs);
static  void                    ContainerDestinationProc(
					Widget          wid,
					XtPointer       closure,
					XmDestinationCallbackStruct *cs);
static  void			ContainerDestPrehookProc(
					Widget          wid,
					XtPointer	closure,
					XmDestinationCallbackStruct *cs);
static	void			ContGetValues(
					Widget		wid,
					XmContainerData	contData);
static	unsigned char		GetVisualEmphasis(
					Widget		cwid);
static	void			SetVisualEmphasis(
					Widget		cwid,
					unsigned char	emphasis);
static	unsigned char		GetViewType(
					Widget		cwid);
static	void			SetViewType(
					Widget		cwid,
					unsigned char	viewtype);
static	Dimension		GetIconWidth(
					Widget		cwid);
static	Cardinal		GetDefaultDetailCount(
					Widget		wid);
static	XmTabList		SetDynamicTabList(
					Widget		wid);
static	XmTabList		GetDumbTabList(
					int		tab_size,
					Cardinal	asked_num_tab);
static	Dimension		GetDynFirstColWidth(
					Widget		wid);
static	void			ResizeIconWidths(
					Widget		wid);
static	void			Layout(
					Widget		wid);
static	void			RequestOutlineDetail(
					Widget		wid,
					XtWidgetGeometry * geo_desired);
static	void			LayoutOutlineDetail(
					Widget		wid);
static	void			LayoutSpatial(
					Widget		wid,
					Boolean		growth_req_allowed,
					CwidNode	stop_node);
static	void			SetCellSizes(
					Widget		wid);
static	void			SetLargeCellSizes(
					Widget		wid);
static	void			SetSmallCellSizes(
					Widget		wid);
static	void			SizeOutlineButton(
					Widget		wid);
static	void			UpdateGCs(
					Widget		wid);
static	void			CreateIconHeader(
					Widget		wid);
static	Widget		        GetRealIconHeader(
					Widget		wid);
static	void			UpdateIconHeader(
					Widget		wid,
					Boolean         count_only);
static	void			ChangeView(
					Widget		wid,
					unsigned char	view);
static	CwidNode 		NewNode(
					Widget		cwid);
static	void			InsertNode(
					CwidNode	node);
static	void			SeverNode(
					CwidNode	node);
static	void			DeleteNode(
					Widget		cwid);
static	CwidNode 		GetFirstNode(
					XmContainerWidget	cw);
static	CwidNode 		GetNextNode(
					CwidNode 	start_item);
static	Boolean			NodeIsActive(
					CwidNode	node);
static	CwidNode 		GetNextUpLevelNode(
					CwidNode 	start_item);
static  void                    StartSelect(
                                        Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static	Boolean			ProcessButtonMotion(
					Widget          wid,
					XEvent          *event,
					String          *params,
					Cardinal        *num_params);
static	Boolean			SelectAllCwids(
					Widget		wid);
static	Boolean			DeselectAllCwids(
					Widget		wid);
static	Boolean			MarkCwid(
        				Widget          cwid,
					Boolean		visual_only);
static	Boolean			UnmarkCwidVisual(
					Widget		cwid);
static	void			SetMarkedCwids(
					Widget		wid);
static	Boolean			ResetMarkedCwids(
					Widget		wid);
static	Boolean			MarkCwidsInRange(
					Widget		wid,
					Widget		cwid1,
					Widget		cwid2,
					Boolean		visual_only);
static	Boolean			MarkCwidsInMarquee(
					Widget		wid,
					Boolean		find_anchor,
					Boolean		visual_only);
static	Boolean			InMarquee(
					Widget		cwid);
static	void			RecalcMarquee(
					Widget		wid,
					Widget		cwid,
					Position	x,
					Position	y);
static	void			DrawMarquee(
					Widget		wid);
static	void			KBSelect(
					Widget		wid,
					XEvent		*event,
					String		*params,
					Cardinal	*num_params);
static	void			SetLocationCursor(
					Widget		cwid);
static	void			CalcNextLocationCursor(
					Widget		wid,
					String		direction);
static	Widget			RedirectTraversal(
                        		Widget		old_focus,
                        		Widget 		new_focus,
                        		unsigned int 	focus_policy,
                        		XmTraversalDirection direction,
                        		unsigned int 	pass) ;
static	CwidNode		GetLastTraversableChild(
					CwidNode	node);
static	CwidNode		GetPrevTraversableSibling(
					CwidNode	node);
static	CwidNode		GetPrevTraversableUplevel(
					CwidNode	node);
static	CwidNode		GetNextTraversableChild(
					CwidNode	node);
static	CwidNode		GetNextTraversableSibling(
					CwidNode	node);
static	CwidNode		GetNextTraversableUplevel(
					CwidNode	node);
static	CwidNode		GetNextTraversable(
					CwidNode	node);
static	Widget			GetLastTraversalWidget(
					XmContainerWidget cw,
					Widget		child,
					Boolean		wrap);
static	Widget			GetFirstTraversalWidget(
					XmContainerWidget cw,
					Widget		child,
					Boolean		wrap);
static	Widget			GetNextTraversalWidget(
					XmContainerWidget cw,
					Widget		child,
					Boolean		wrap);
static	Widget			GetPrevTraversalWidget(
					XmContainerWidget cw,
					Widget		child,
					Boolean		wrap);
static	void			MakeOutlineButton(
					Widget		cwid);
static	void			ChangeOutlineButtons(
					Widget		wid);
static	void			ExpandCwid(
					Widget		cwid);
static	void			CollapseCwid(
					Widget		cwid);
static	void			CallActionCB(
					Widget		cwid,
					XEvent		*event);
static	void			CallSelectCB(
					Widget		wid,
					XEvent		*event,
					unsigned char	auto_selection_type);
static	WidgetList		GetSelectedCwids(
					Widget		wid);
static	void			GetSelectedItems(
					Widget		wid,
					int		offset,
					XtArgVal	*value);
static	int			CompareInts(
					XmConst void	*p1,
					XmConst void	*p2);
static	int			Isqrt(
					int		n);
static	void			GainPrimary(
					Widget		wid,
					Time		timestamp);
static	void			ConvertRefuse(
					Widget		wid,
					XtPointer	closure,
					XmConvertCallbackStruct * cs);
static	void			DragStart(
					XtPointer	data,
					XtIntervalId *	id);
static	void			 OutlineButtonCallback(
					Widget		pbwid,
					XtPointer	client_data,
					XtPointer	call_data);
static  void			OutlineButtonAction(
					Widget          cwid,
					unsigned char   new_state,
					XEvent          *event);
static	void			MoveItemCallback(
					Widget		wid,
					XtPointer	client_data,
					XtPointer	call_data);
static	void			DropDoneCallback(
					Widget		wid,
					XtPointer	client_data,
					XtPointer	call_data);
static 	void			FreeLocationData(
					Widget		wid,
					XtEnum		op,
					XmTransferDoneCallbackStruct*);
static  Boolean                 SetupDrag(
                                        Widget          wid,
                                        XEvent          *event,
                                        String          *params,
                                        Cardinal        *num_params);
static void 			CheckSetRenderTable(
					Widget 		wid,
					int		offset,
					XrmValue 	*value);
static void			DefaultCollapsedPixmap(
					Widget		wid,
					int		offset,
					XrmValue *	value);
static void			EnterHandler(
					Widget		wid,
					XtPointer	closure,
					XEvent	        *event,
					Boolean       	*continue_to_dispatch);
static void			LeaveHandler(
					Widget		wid,
					XtPointer	closure,
					XEvent		*event,
					Boolean 	*continue_to_dispatch);
static void			ScrollProc(
					XtPointer	closure,
					XtIntervalId*   id);
static void 			ContainerResetDepths (XmContainerConstraint c);
static void FindMaxDepths (XmContainerConstraint c, Widget cw);
static Boolean			ContainerIsDescendant (Widget containerChild, Widget newEntryParent);
static void			ContainerResequenceNodes(XmContainerWidget cw, Widget entryParent);

/********    End Static Function Declarations    ********/


static	XtActionsRec	actionsList[] = {
{"ContainerBeginSelect",	(XtActionProc)	ContainerBeginSelect},
{"ContainerButtonMotion",	(XtActionProc)	ContainerButtonMotion},
{"ContainerEndSelect",		(XtActionProc)	ContainerEndSelect},
{"ContainerBeginToggle",	(XtActionProc)	ContainerBeginToggle},
{"ContainerEndToggle",		(XtActionProc)	ContainerEndToggle},
{"ContainerBeginExtend",	(XtActionProc)	ContainerBeginExtend},
{"ContainerEndExtend",	(XtActionProc)	ContainerEndExtend},
{"ContainerStartTransfer",	(XtActionProc)	ContainerStartTransfer},
{"ContainerEndTransfer",	(XtActionProc)	ContainerEndTransfer},
{"ContainerPrimaryCopy",	(XtActionProc)	ContainerPrimaryCopy},
{"ContainerPrimaryLink",	(XtActionProc)	ContainerPrimaryLink},
{"ContainerPrimaryMove",	(XtActionProc)	ContainerPrimaryMove},
{"ContainerCancel",		(XtActionProc)	ContainerCancel},
{"ContainerSelect",		(XtActionProc)	ContainerSelect},
{"ContainerExtend",		(XtActionProc)	ContainerExtend},
{"ContainerMoveCursor",		(XtActionProc)	ContainerMoveCursor},
{"ContainerExtendCursor",	(XtActionProc)	ContainerExtendCursor},
{"ContainerToggleMode",		(XtActionProc)	ContainerToggleMode},
{"ContainerSelectAll",		(XtActionProc)	ContainerSelectAll},
{"ContainerDeselectAll",	(XtActionProc)	ContainerDeselectAll},
{"ContainerActivate",		(XtActionProc)	ContainerActivate},
{"ContainerExpandOrCollapse",	(XtActionProc)	ContainerExpandOrCollapse},
{"ContainerNoop",		(XtActionProc)  ContainerNoop},
{"ContainerHandleBtn1Down",	(XtActionProc)  ContainerHandleBtn1Down},
{"ContainerHandleBtn1Motion",	(XtActionProc)  ContainerHandleBtn1Motion},
{"ContainerHandleBtn1Up",	(XtActionProc)  ContainerHandleBtn1Up},
{"ContainerHandleBtn2Down",	(XtActionProc)  ContainerHandleBtn2Down},
{"ContainerHandleBtn2Motion",	(XtActionProc)  ContainerHandleBtn2Motion},
{"ContainerHandleBtn2Up",	(XtActionProc)  ContainerHandleBtn2Up}
};


static	XtResource	resources[] =
{
    {
	XmNautomaticSelection,XmCAutomaticSelection,
	XmRAutomaticSelection,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.automatic),
	XmRImmediate,(XtPointer)XmAUTO_SELECT},
    {
	XmNcollapsedStatePixmap,XmCCollapsedStatePixmap,XmRDynamicPixmap,
	sizeof(Pixmap),
	XtOffsetOf(XmContainerRec, container.collapsed_state_pixmap),
	XmRCallProc, (XtPointer)DefaultCollapsedPixmap},
    {
	XmNconvertCallback,XmCCallback,XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(XmContainerRec,container.convert_cb),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdefaultActionCallback,XmCCallback,XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(XmContainerRec,container.default_action_cb),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdestinationCallback,XmCCallback,XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(XmContainerRec,container.destination_cb),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdetailColumnHeading,XmCDetailColumnHeading,XmRXmStringTable,
	sizeof(XmStringTable),
	XtOffsetOf(XmContainerRec,container.detail_heading),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdetailColumnHeadingCount,XmCDetailColumnHeadingCount,
	XmRCardinal,sizeof(Cardinal),
	XtOffsetOf(XmContainerRec, container.detail_heading_count),
	XmRImmediate,(XtPointer)0},
    {
	XmNdetailOrder,XmCDetailOrder,XmRCardinalList,
	sizeof(Cardinal *),
	XtOffsetOf(XmContainerRec,container.detail_order),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdetailOrderCount,XmCDetailOrderCount,XmRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(XmContainerRec,container.detail_order_count),
	XmRImmediate,(XtPointer)0},
    {
	XmNdetailTabList,XmCDetailTabList,XmRTabList,sizeof(XmTabList),
	XtOffsetOf(XmContainerRec,container.detail_tablist),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNentryViewType,XmCEntryViewType,XmREntryViewType,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.entry_viewtype),
	XmRImmediate,(XtPointer)XmANY_ICON},
    {
	XmNexpandedStatePixmap,XmCExpandedStatePixmap,XmRDynamicPixmap,
	sizeof(Pixmap),
	XtOffsetOf(XmContainerRec, container.expanded_state_pixmap),
	XmRString,(XtPointer)"expanded"},
    {
	XmNlargeCellHeight,XmCCellHeight,XmRVerticalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.large_cell_height),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNlargeCellWidth,XmCCellWidth,XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.large_cell_width),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNlayoutType,XmCLayoutType,XmRLayoutType,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.layout_type),
	XmRImmediate,(XtPointer)XmSPATIAL},
    {
	XmNmarginHeight,XmCMarginHeight,XmRVerticalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.margin_h),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNmarginWidth,XmCMarginWidth,XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.margin_w),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNoutlineButtonPolicy,XmCOutlineButtonPolicy,
	XmROutlineButtonPolicy,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.ob_policy),
	XmRImmediate,(XtPointer)XmOUTLINE_BUTTON_PRESENT},
    {
	XmNoutlineChangedCallback,XmCCallback,XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(XmContainerRec,container.outline_cb),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNoutlineColumnWidth,XmCOutlineColumnWidth,XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.first_col_width),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNoutlineIndentation,XmCOutlineIndentation,XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.outline_indent),
	XmRImmediate,(XtPointer)DEFAULT_INDENTATION},
    {
	XmNoutlineLineStyle,XmCLineStyle,XmRLineStyle,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.outline_sep_style),
	XmRImmediate,(XtPointer)XmSINGLE},
    {
	XmNprimaryOwnership,XmCPrimaryOwnership,XmRPrimaryOwnership,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.primary_ownership),
	XmRImmediate,(XtPointer)XmOWN_POSSIBLE_MULTIPLE},
    {
	"pri.vate","Pri.vate",XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(XmContainerRec,container.first_change_managed),
	XmRImmediate, (XtPointer) False},
    {
	XmNfontList,XmCFontList,XmRFontList,
	sizeof(XmRenderTable),
	XtOffsetOf(XmContainerRec,container.render_table),
	XmRCallProc, (XtPointer)CheckSetRenderTable},
    {
	XmNrenderTable,XmCRenderTable,XmRRenderTable,
	sizeof(XmRenderTable),
	XtOffsetOf(XmContainerRec,container.render_table),
	XmRCallProc, (XtPointer)CheckSetRenderTable},
    {
	XmNselectColor,XmCSelectColor,XmRSelectColor,sizeof(Pixel),
	XtOffsetOf(XmContainerRec,container.select_color),
	XmRImmediate,(XtPointer)XmREVERSED_GROUND_COLORS},
    {
	XmNselectedObjects,XmCSelectedObjects,XmRWidgetList,
	sizeof(WidgetList),
	XtOffsetOf(XmContainerRec,container.selected_items),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNselectedObjectCount,XmCSelectedObjectCount,XmRInt,
	sizeof(int),
	XtOffsetOf(XmContainerRec, container.selected_item_count),
	XmRImmediate,(XtPointer)0},
    {
	XmNselectionCallback,XmCCallback,XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf(XmContainerRec,container.selection_cb),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNselectionPolicy,XmCSelectionPolicy,XmRSelectionPolicy,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.selection_policy),
	XmRImmediate,(XtPointer)XmEXTENDED_SELECT},
    {
	XmNselectionTechnique,XmCSelectionTechnique,
	XmRSelectionTechnique,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec, container.selection_technique),
	XmRImmediate,(XtPointer)XmTOUCH_OVER},
    {
	XmNsmallCellHeight,XmCCellHeight,XmRVerticalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.small_cell_height),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNsmallCellWidth,XmCCellWidth,XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.small_cell_width),
	XmRImmediate,(XtPointer)ZERO_DIM},
    {
	XmNspatialIncludeModel,XmCSpatialIncludeModel,
	XmRSpatialIncludeModel,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.include_model),
	XmRImmediate,(XtPointer)XmAPPEND},
    {
	XmNspatialResizeModel,XmCSpatialResizeModel,
	XmRSpatialResizeModel,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.resize_model),
	XmRImmediate,(XtPointer)XmGROW_MINOR},
    {
	XmNspatialSnapModel,XmCSpatialSnapModel,
	XmRSpatialSnapModel,sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.snap_model),
	XmRImmediate,(XtPointer)XmNONE},
    {
	XmNspatialStyle,XmCSpatialStyle,XmRSpatialStyle,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerRec,container.spatial_style),
	XmRImmediate,(XtPointer)XmGRID},
};


static	XmSyntheticResource	syn_resources[] =
{
    {
	XmNlargeCellHeight,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.large_cell_height),
	XmeFromVerticalPixels,XmeToVerticalPixels},
    {
	XmNlargeCellWidth,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.large_cell_width),
	XmeFromHorizontalPixels,XmeToHorizontalPixels},
    {
	XmNmarginHeight,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.margin_h),
	XmeFromVerticalPixels,XmeToVerticalPixels},
    {
	XmNmarginWidth,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.margin_w),
	XmeFromHorizontalPixels,XmeToHorizontalPixels},
    {
	XmNoutlineIndentation,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.outline_indent),
	XmeFromHorizontalPixels,XmeToHorizontalPixels},
    {
	XmNselectedObjects,sizeof(WidgetList),
	XtOffsetOf(XmContainerRec,container.selected_items),
	GetSelectedItems,NULL},
    {
	XmNsmallCellHeight,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.small_cell_height),
	XmeFromVerticalPixels,XmeToVerticalPixels},
    {
	XmNsmallCellWidth,sizeof(Dimension),
	XtOffsetOf(XmContainerRec,container.small_cell_width),
	XmeFromHorizontalPixels,XmeToHorizontalPixels},
    {
	XmNdetailColumnHeading, sizeof(XmStringTable),
	XtOffsetOf(XmContainerRec,container.detail_heading),
	GetDetailHeader, NULL},
    {
	XmNdetailColumnHeadingCount, sizeof(Cardinal),
	XtOffsetOf(XmContainerRec, container.detail_heading_count),
	GetDetailHeaderCount, NULL},
    {
        XmNoutlineColumnWidth, sizeof(Dimension),
        XtOffsetOf(XmContainerRec,container.first_col_width),
        GetOutlineColumnWidth, NULL},
    };


/*  The constraint resource list  */
static 	XtResource 	constraints[] =
{
    {
	XmNentryParent,XmCWidget,XmRWidget,sizeof(Widget),
	XtOffsetOf(XmContainerConstraintRec,container.entry_parent),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNoutlineState,XmCOutlineState,XmROutlineState,
	sizeof(unsigned char),
	XtOffsetOf(XmContainerConstraintRec,container.outline_state),
	XmRImmediate,(XtPointer)XmCOLLAPSED},
    {
	XmNpositionIndex,XmCPositionIndex,XmRInt,sizeof(int),
	XtOffsetOf(XmContainerConstraintRec,container.position_index),
	XmRImmediate,(XtPointer)XmLAST_POSITION},
};

static XmManagerClassExtRec managerClassExtRec = {
    NULL,
    NULLQUARK,
    XmManagerClassExtVersion,
    sizeof(XmManagerClassExtRec),
    NULL,                               /* traversal_children */
    ObjectAtPoint                       /* object_at_point */
};

externaldef( xmcontainerclassrec) XmContainerClassRec	xmContainerClassRec =
{	/* CoreClassPart */
	{
	(WidgetClass) &xmManagerClassRec, /* superclass	*/
	"XmContainer",			/* class_name		*/
	sizeof(XmContainerRec),		/* widget_size		*/
	NULL,				/* class_initialize	*/
	ClassPartInitialize,		/* class_initialize     */
	FALSE,				/* class_inited		*/
	Initialize,			/* initialize		*/
	NULL,				/* initialize_hook	*/
	XtInheritRealize,		/* realize		*/
	actionsList,			/* actions		*/
	XtNumber(actionsList),		/* num_actions		*/
	resources,			/* resources		*/
	XtNumber(resources),		/* num_resources	*/
	NULLQUARK,			/* xrm_class		*/
	TRUE,				/* compress_motion	*/
	XtExposeCompressMultiple,	/* compress_exposure	*/
	TRUE,       			/* compress_enterleave 	*/
	FALSE,       			/* visible_interest	*/
	Destroy,			/* destroy		*/
	Resize,				/* resize		*/
	Redisplay,			/* expose		*/
	SetValues,			/* set_values		*/
	NULL,				/* set_values_hook	*/
	XtInheritSetValuesAlmost,	/* set_values_almost	*/
	NULL,				/* get_values_hook	*/
	NULL,				/* accept_focus		*/
	XtVersion,			/* version		*/
	NULL,				/* callback private	*/
	defaultTranslations,		/* tm_table		*/
	QueryGeometry,			/* query_geometry	*/
        NULL,                           /* display_accelerator  */
        NULL,                           /* extension		*/
	},
	/* Composite class fields				*/
	{
	GeometryManager,		/* geometry_manager     */
        ChangeManaged,			/* change_managed       */
        XtInheritInsertChild,		/* insert_child         */
        XtInheritDeleteChild,  		/* delete_child         */
        NULL,                 		/* extension		*/
	},
	/* Constraint class fields                              */
        {
        constraints,			/* resource list        */
        XtNumber(constraints),		/* num resources 	*/
        sizeof(XmContainerConstraintRec), /* constraint size 	*/
        ConstraintInitialize,		/* init proc            */
        ConstraintDestroy,		/* destroy proc         */
        ConstraintSetValues,		/* set values proc      */
        NULL,                           /* extension            */
        },
        /*  XmManager class fields                              */
        {
	traversalTranslations,		/* translations		*/
        syn_resources,                  /* get resources        */
        XtNumber(syn_resources),        /* num get_resources    */
        NULL,			       	/* get_cont_resources   */
        0,				/* num_get_cont_resources */
	XmInheritParentProcess,		/* parent_process	*/
        (XtPointer)&managerClassExtRec, /* extension            */
        },
	/* Container class fields */
	{
	TestFitItem,	                /* test_fit_item	*/
	PlaceItem,  	                /* place_item		*/
	RemoveItem, 	                /* remove_item		*/
	GetSpatialSize,                 /* get_spatial_size 	*/
	NULL,				/* extension		*/
	}
};


externaldef( xmcontainerwidgetclass) WidgetClass xmContainerWidgetClass
		= (WidgetClass) &xmContainerClassRec;


/* Transfer Trait record for Container */
static XmConst XmTransferTraitRec transferT = {
	0,				/* version */
	(XmConvertCallbackProc)		ContainerConvertProc,
	(XmDestinationCallbackProc)	ContainerDestinationProc,
	(XmDestinationCallbackProc)	ContainerDestPrehookProc,
};

/* Trait record for ContainerT */
static  XmConst XmContainerTraitRec containerT =
{
        0,              		/* version */
        (XmContainerGetValuesProc)	ContGetValues,
};

/* TraversalControl Trait record for Container */
static XmConst XmTraversalControlTraitRec traversalControlT =
{
  0,                            /* version */
  RedirectTraversal		/* redirect */
};



/* Context for drag icon information */
static XContext dragIconInfoContext = 0;

/* Data structure for drag icon info */
typedef struct _DragIconInfo {
  Widget	state;
  Widget	source;
} DragIconInfoRec, *DragIconInfo;




/************************************************************************
 * GetDetailHeader
 ************************************************************************/
static	void
GetDetailHeader(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
    XmContainerWidget cw = (XmContainerWidget) wid ;
    Widget icon_header ;
    Cardinal icon_detail_header_count, i ;
    XmStringTable detail_header = NULL, icon_detail_header ;
    XmString label_string ;

    /* If there is an icon_header, fetch the information out of it */
    if ((icon_header = GetRealIconHeader(wid)) != NULL) {

	/* we get memory owned by IconG here */
	XtVaGetValues (icon_header,
		       XmNlabelString, &label_string,
		       XmNdetail, &icon_detail_header,
		       XmNdetailCount, &icon_detail_header_count,
		       NULL);

	/* now we need to add the labelString itself, so malloc
	   something in the Container itself.
	   First free what is currently allocated. Last free is
	   done in Destroy */
 	if (cw->container.cache_detail_heading)
 	    XtFree((char*)cw->container.cache_detail_heading) ;
 	cw->container.cache_detail_heading = (XmStringTable)
  	    XtMalloc(sizeof(XmString) * (icon_detail_header_count + 1)) ;
  	for (i=0; i < icon_detail_header_count; i++)
 	    cw->container.cache_detail_heading[i+1] = icon_detail_header[i] ;
 	cw->container.cache_detail_heading[0] = label_string ;

 	detail_header = cw->container.cache_detail_heading;

  	/* app need not to free the returned array */
      }
     else
 	/* return real data set by the application, which we trust it
 	** has been maintaining
 	*/
 	detail_header = cw->container.detail_heading;

    *value = (XtArgVal)detail_header;
}



/************************************************************************
 * GetDetailHeaderCount
 ************************************************************************/
static	void
GetDetailHeaderCount(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
    Widget icon_header ;
    Cardinal detail_header_count = 0 ;

    /* If there is an icon_header, fetch the information out of it */
    if ((icon_header = GetRealIconHeader(wid)) != NULL) {

	XtVaGetValues (icon_header, XmNdetailCount, &detail_header_count,
		       NULL);

	detail_header_count += 1; /* +1 for the labelString itself */
    }

    else {
     	XmContainerWidget cw = (XmContainerWidget)wid;
 	detail_header_count = cw->container.saved_detail_heading_count;
    }

    *value = (XtArgVal)detail_header_count;
}

/************************************************************************
 * GetOutlineColumnWidth
 ************************************************************************/
static  void
GetOutlineColumnWidth(
        Widget          wid,
        int             offset, /* unused */
        XtArgVal        *value)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;

    /* Send 0 if dynamic, real_first_col_width otherwise */
    if (CtrIsDynamic(cw,FIRSTCW))
	*value = (XtArgVal)0;
    else
	*value = (XtArgVal)cw->container.real_first_col_width;
}

/************************************************************************
 * ClassPartInitialize (Core Method)
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass     wc)
{
    _XmFastSubclassInit(wc,XmCONTAINER_BIT);

    /* Allow inheritance for subclasses */
    if (wc != xmContainerWidgetClass)
      {
      XmContainerClass	this_class = (XmContainerClass)wc;
      XmContainerClass	super = (XmContainerClass)wc;

      if (this_class->container_class.test_fit_item ==
			    XmInheritSpatialTestFitProc)
	    this_class->container_class.test_fit_item =
		    super->container_class.test_fit_item;
      if (this_class->container_class.place_item ==
			    XmInheritSpatialPlacementProc)
	    this_class->container_class.place_item =
		    super->container_class.place_item;
      if (this_class->container_class.remove_item ==
			    XmInheritSpatialRemoveProc)
	    this_class->container_class.remove_item =
		    super->container_class.remove_item;
      if (this_class->container_class.get_spatial_size ==
			    XmInheritSpatialGetSize)
	    this_class->container_class.get_spatial_size =
		    super->container_class.get_spatial_size;
      }

  XmeTraitSet((XtPointer)wc,XmQTtransfer,(XtPointer)&transferT);
  XmeTraitSet((XtPointer)wc,XmQTcontainer,(XtPointer)&containerT);
  XmeTraitSet((XtPointer)wc,XmQTtraversalControl,(XtPointer)&traversalControlT);

}

/************************************************************************
 * Initialize (Core Method)
 ************************************************************************/

static  void
Initialize(
        Widget          rw,
        Widget          nw,
        ArgList         args, 		/* unused */
        Cardinal        *num_args) 	/* unused */
{
    XmContainerWidget	ncw = (XmContainerWidget)nw;
    XmContainerWidget	rcw = (XmContainerWidget)rw;
    int	n, i;
    Arg	wargs[10];
    Atom	targets[1];
    XmScrollFrameTrait	scrollFrameTrait;

    /*
     * Verify enumerated resources.
     */
    if (!XmRepTypeValidValue(XmRID_AUTOMATIC_SELECTION,
			     ncw->container.automatic,nw))
	ncw->container.automatic = XmAUTO_SELECT;
    if (!XmRepTypeValidValue(XmRID_ENTRY_VIEW_TYPE,
			     ncw->container.entry_viewtype,nw))
	ncw->container.entry_viewtype = XmANY_ICON;
    if (!XmRepTypeValidValue(XmRID_SPATIAL_INCLUDE_MODEL,
			     ncw->container.include_model,nw))
	ncw->container.include_model = XmAPPEND;
    if (!XmRepTypeValidValue(XmRID_LAYOUT_TYPE,
			     ncw->container.layout_type,nw))
	ncw->container.layout_type = XmSPATIAL;
    if (!XmRepTypeValidValue(XmRID_LINE_STYLE,
			     ncw->container.outline_sep_style,nw))
	ncw->container.outline_sep_style = XmSINGLE;
    if (!XmRepTypeValidValue(XmRID_OUTLINE_BUTTON_POLICY,
			     ncw->container.ob_policy,nw))
	ncw->container.ob_policy = XmOUTLINE_BUTTON_PRESENT;
    if (!XmRepTypeValidValue(XmRID_SPATIAL_STYLE,
			     ncw->container.spatial_style,nw))
	ncw->container.spatial_style = XmGRID;
    if (!XmRepTypeValidValue(XmRID_PRIMARY_OWNERSHIP,
			     ncw->container.primary_ownership,nw))
	ncw->container.primary_ownership = XmOWN_POSSIBLE_MULTIPLE;
    if (!XmRepTypeValidValue(XmRID_SPATIAL_RESIZE_MODEL,
			     ncw->container.resize_model,nw))
	ncw->container.resize_model = XmGROW_MINOR;
    if (!XmRepTypeValidValue(XmRID_SELECTION_POLICY,
			     ncw->container.selection_policy,nw))
	ncw->container.selection_policy = XmEXTENDED_SELECT;
    if (!XmRepTypeValidValue(XmRID_SELECTION_TECHNIQUE,
			     ncw->container.selection_technique,nw))
	ncw->container.selection_technique = XmTOUCH_OVER;
    if (!XmRepTypeValidValue(XmRID_SPATIAL_SNAP_MODEL,
			     ncw->container.snap_model,nw))
	ncw->container.snap_model = XmNONE;

    /*
     * deal with XmNdetailOrder & XmNdetailOrderCount
     */
    if (ncw->container.detail_order_count && ncw->container.detail_order) {
	ncw->container.detail_order = (Cardinal *)
	    XtMalloc(sizeof(Cardinal) * ncw->container.detail_order_count);
	for (i=0; i < ncw->container.detail_order_count; i++)
	    ncw->container.detail_order[i] =
		rcw->container.detail_order[i] ;
    }

    /*
     * Initialize internal variables.
     */
    ncw->container.first_node = NULL;
    ncw->container.last_node = NULL;
    ncw->container.self = False;
    ncw->container.create_cwid_type = CONTAINER_ICON;

    ncw->container.toggle_pressed = False;
    ncw->container.extend_pressed = False;
    ncw->container.ob_pressed = False;
    ncw->container.cancel_pressed = False;
    ncw->container.kaddmode =
	(CtrPolicyIsMULTIPLE(ncw) || CtrPolicyIsSINGLE(ncw));
    ncw->container.marquee_mode = !(CtrTechIsTOUCH_ONLY(ncw));
    ncw->container.marquee_drawn = False;

    ncw->container.selection_state = XmSELECTED;
    ncw->container.anchor_cwid = NULL;
    ncw->container.have_primary = False;

    ncw->container.last_click_time = 0;
    ncw->container.selecting = False;
    ncw->container.dynamic_resource = 0L;

    ncw->container.cache_detail_heading = NULL;

    /*
     * Get default Font.
     */
    if (ncw->container.render_table == NULL)
	{
	    XmFontList	defaultFont = NULL;

	    defaultFont = XmeGetDefaultRenderTable(nw,XmLABEL_FONTLIST);
	    ncw->container.render_table = XmFontListCopy(defaultFont);
	}
    else
	ncw->container.render_table =
	    XmFontListCopy(ncw->container.render_table);

    /*
     * Copy XmTablist if set, otherwise, keep track that it's dynamic.
     */
    if (ncw->container.detail_tablist == NULL)
	ncw->container.dynamic_resource |= TABLIST;
    if (ncw->container.detail_tablist != NULL)
	ncw->container.detail_tablist =
	    XmTabListCopy(ncw->container.detail_tablist,0,0);

    /*
     * Deal with setting of selectColor special cases
     */
    if (ncw->container.select_color == XmDEFAULT_SELECT_COLOR) {
        XrmValue val;

        _XmSelectColorDefault(nw, 0, &val);
        ncw->container.select_color = *((Pixel*) val.addr);
    } else
    if (ncw->container.select_color == XmHIGHLIGHT_COLOR) {
        ncw->container.select_color = ncw->manager.highlight_color ;
    }


     /*
     * Initialize GCs
     */
    ncw->container.normalGC = NULL;
    ncw->container.marqueeGC = NULL;
    UpdateGCs(nw);

    ncw->container.size_ob = NULL;
    SizeOutlineButton(nw);

    /* this flag is used during: expansion/collapse of outlinebutton
       to know if the first column can grow/shrink. If it
       was set, it can't, otherwise, a default is calculated whenever
       changemanaged is called and it is updated when state of
       expansion change. We need to mark the first col resource
       so that any change is caught during setvalues */
    if (ncw->container.first_col_width == 0)
	ncw->container.dynamic_resource |= FIRSTCW;
    ncw->container.real_first_col_width = ncw->container.first_col_width  ;
    ncw->container.first_col_width = INVALID_DIMENSION  ;

    ncw->container.saved_detail_heading_count = 0 ;
    ncw->container.icon_header = NULL;

    if ((ncw->container.detail_heading_count && !ncw->container.detail_heading)
	|| (!ncw->container.detail_heading_count
	    && ncw->container.detail_heading))
	{
	    XmeWarning ((Widget) ncw, MESSAGE1);
	}

    if (ncw->container.detail_heading_count > 0) {

	/* if detail heading resources specified, create icon_header */
	if (ncw->container.detail_heading) {
	    /* and point to right sized array ! */

	    CreateIconHeader(nw);
	    /* ncw->container.icon_header is a DA iff we are
	       in the automatic SW case.
	       ncw->container.icon_header is created unmanaged */

	    if (CtrLayoutIsDETAIL(ncw))
		XtManageChild(ncw->container.icon_header);
	    else
		XtUnmanageChild(ncw->container.icon_header);
	}

	/* saved the heading count in case the heading is set later on. */
	ncw->container.saved_detail_heading_count =
	    ncw->container.detail_heading_count;

	/* mark the detail_heading_count as invalid so
	   that any change will be trapped by SetValues */
	ncw->container.detail_heading_count = INVALID_COUNT ;
    }

    ncw->container.max_depth = 0;
    ncw->container.outline_segs = NULL;
    ncw->container.outline_seg_count = 0;

    /*
     * Register as Drop site.
     */
    n = 0;
    targets[0] = XInternAtom(XtDisplay(nw),XmS_MOTIF_DRAG_OFFSET,False);
    XtSetArg(wargs[n],XmNimportTargets,targets); n++;
    XtSetArg(wargs[n],XmNnumImportTargets,1); n++;
    XmeDropSink(nw,wargs,n);

    ncw->container.drag_context = (Widget) NULL;
    ncw->container.transfer_action = NULL;
    ncw->container.transfer_timer_id = 0;

    /*
     * Initialize prev width for Resize calculations
     *        & first_change_managed for initial sizing calculations.
     */
    ncw->container.prev_width = 0;
    ncw->container.first_change_managed = True;

    /*
     * Initialize GRID/CELLS
     */
    ncw->container.cells = NULL;
    ncw->container.cell_count = 0;
    ncw->container.next_free_cell = 0;
    ncw->container.current_width_in_cells = 0;
    ncw->container.current_height_in_cells = 0;
    ncw->container.cells_region = XCreateRegion();

    /*
     * Set Cell sizes.
     */
    ncw->container.large_cell_dim_fixed =
	((ncw->container.large_cell_height > 0) &&
	 (ncw->container.large_cell_width > 0));
    ncw->container.small_cell_dim_fixed =
	((ncw->container.small_cell_height > 0) &&
	 (ncw->container.small_cell_width > 0));
    SetCellSizes(nw);

    /*
     * init auto scroll related data
     */
    scrollFrameTrait = (XmScrollFrameTrait)
        XmeTraitGet((XtPointer)XtClass(XtParent(nw)),XmQTscrollFrame);
    if (scrollFrameTrait &&
        (scrollFrameTrait->getInfo(XtParent(nw),NULL,NULL,NULL))) {
	XtAddEventHandler(nw, EnterWindowMask, False, EnterHandler, NULL);
	XtAddEventHandler(nw, LeaveWindowMask, False, LeaveHandler, NULL);
    }
    ncw->container.scroll_proc_id = 0;
    ncw->container.LeaveDir = 0;
}


/************************************************************************
 * Destroy (Core Method)
 ************************************************************************/
static  void
Destroy(
    Widget  wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    XmScrollFrameTrait	scrollFrameTrait = (XmScrollFrameTrait)
	XmeTraitGet((XtPointer)XtClass(XtParent(wid)),XmQTscrollFrame);

    XmFontListFree(cw->container.render_table);


    XtReleaseGC(wid,cw->container.normalGC);
    XtReleaseGC(wid,cw->container.marqueeGC);

    if (cw->container.transfer_timer_id != 0)
	XtRemoveTimeOut(cw->container.transfer_timer_id);

    /* free the detail_heading array that might have been
       allocated during a getvalues */
    if (cw->container.cache_detail_heading)
	XtFree((char*)cw->container.cache_detail_heading);

    if (cw->container.detail_order_count && cw->container.detail_order) {
	XtFree((char*)cw->container.detail_order);
    }

    if (cw->container.detail_tablist)
	XmTabListFree(cw->container.detail_tablist);

    if (cw->container.outline_segs)
	XtFree((char*)cw->container.outline_segs);

    if (cw->container.cells)
	XtFree((char*)cw->container.cells);

    XDestroyRegion(cw->container.cells_region);

    if (scrollFrameTrait && cw->container.icon_header != NULL) {
	/* only destroy the icon_header if not a child of the
	   Container, the auto SW/DA case */
	assert (XtParent(cw->container.icon_header) != wid);
	/* that will destroy the icon header child as well */
	XtDestroyWidget(cw->container.icon_header);
    }
    /* else the icon_header is a child of the container and is
    ** already destroyed (which is why we can't refer to it until we
    ** know its lineage)
    */

    if (cw->container.scroll_proc_id)
	XtRemoveTimeOut(cw->container.scroll_proc_id);
}

/************************************************************************
 * Resize (Core Method)
 ************************************************************************/
static void
Resize(
    Widget 	wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;

    /*
     * The Outline layout is oblivious to size constraints
     * in the Left-to-Right layout direction.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw) && (!CtrLayoutIsDETAIL(cw))
			&& (!LayoutIsRtoLM(cw)))
	return;
    /*
     * One-dimensional Right-to_Left layouts are oblivious to
     * height changes; so, just return if the width hasn't changed.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw) &&
	(cw->core.width == cw->container.prev_width))
	return;

    /*
     * Save the current width for next time and reLayout.
     */
    cw->container.prev_width = cw->core.width;
    cw->container.self = True;
    Layout(wid);
    cw->container.self = False;

    /*
     * Outline lines will be moved in response to width change in a
     * Right-to_Left Outline Layout.  Better clear everything to erase
     * the old lines.  Also, details may be redrawn.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw))
	if (XtIsRealized((Widget)cw))
	        XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);
}

/************************************************************************
 * Redisplay (Core Method)
 * Special care must be taken to redraw the marquee which is drawn in XOR
 * mode. Normaly we should only need to draw the marquee with a clip
 * rectangle equal to the expose region. However, the XmIconGadgets do not
 * perform clipped redraw and redraw themselves entirely instead, because
 * its a pain to deal with both a clip rectangle and a possible clip mask.
 * So to avoid any problem, we must fully erase the marquee and then
 * redraw it.
 ************************************************************************/
static  void
Redisplay(
    Widget  wid,
    XEvent  *event,
    Region  region)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;

    /* Erase the marquee if there is any */
    if (cw->container.marquee_drawn) {
	DrawMarquee(wid);
	XSetRegion(XtDisplay(wid), cw->container.normalGC, region);
	XSetForeground(XtDisplay(wid), cw->container.normalGC,
		       cw->core.background_pixel);
	XFillRectangle(XtDisplay(wid), XtWindow(wid), cw->container.normalGC,
		       event->xexpose.x, event->xexpose.y,
		       event->xexpose.width, event->xexpose.height);
	XSetClipMask(XtDisplay(wid), cw->container.normalGC, None);
	XSetForeground(XtDisplay(wid), cw->container.normalGC,
		       cw->manager.foreground);
    }

    /*
     * If lines are present & we're in outline layout, draw the lines.
     */
    if (CtrDrawLinesOUTLINE(cw) && (cw->container.outline_seg_count > 0)) {
	XSetClipMask(XtDisplay(wid), cw->container.normalGC, None);
	XDrawSegments(XtDisplay(wid),XtWindow(wid),
		cw->container.normalGC,
		cw->container.outline_segs,
		cw->container.outline_seg_count);
	}

    /*
     * Redisplay all affected gadgets.
     */
    XmeRedisplayGadgets(wid,event,region);

    /* Redraw marquee when needed */
    if (cw->container.marquee_drawn)
	DrawMarquee(wid);
}

/************************************************************************
 * SetValues (Core Method)
 ************************************************************************/

static 	Boolean
SetValues(
    Widget          cw,
    Widget          rw,		/* unused */
    Widget          nw,
    ArgList         args,	/* unused */
    Cardinal        *num_args)	/* unused */
{
    XmContainerWidget	ccw = (XmContainerWidget)cw;
    XmContainerWidget	ncw = (XmContainerWidget)nw;
    Boolean         	container_need_layout = False;
    Boolean		spatial_need_layout = False;
    Boolean		outline_need_layout = False;
    Boolean		detail_need_layout = False;
    Boolean		need_expose = False;
    CwidNode		node;
    Widget		cwid;
    XmContainerConstraint c;
    XPoint 		snap_point;
    int			i;

    ncw->container.self = True;

    /*
     * Verify enumerated resources.
     */
    if (ncw->container.automatic != ccw->container.automatic)
	{
    	if (!XmRepTypeValidValue(XmRID_AUTOMATIC_SELECTION,
                    ncw->container.automatic,nw))
            ncw->container.automatic =
		    ccw->container.automatic;
	}
    if (ncw->container.entry_viewtype != ccw->container.entry_viewtype)
	{
    	if (!XmRepTypeValidValue(XmRID_ENTRY_VIEW_TYPE,
                    ncw->container.entry_viewtype,nw))
            ncw->container.entry_viewtype =
		    ccw->container.entry_viewtype;
 	}
    if (ncw->container.include_model != ccw->container.include_model)
	{
	if (!XmRepTypeValidValue(XmRID_SPATIAL_INCLUDE_MODEL,
                    ncw->container.include_model,nw))
            ncw->container.include_model =
		    ccw->container.include_model;
	}
    if (ncw->container.layout_type != ccw->container.layout_type)
	{
    	if (!XmRepTypeValidValue(XmRID_LAYOUT_TYPE,
                    ncw->container.layout_type,nw))
            ncw->container.layout_type =
		    ccw->container.layout_type;
	}
    if (ncw->container.outline_sep_style != ccw->container.outline_sep_style)
	{
    	if (!XmRepTypeValidValue(XmRID_LINE_STYLE,
                    ncw->container.outline_sep_style,nw))
            ncw->container.outline_sep_style =
		    ccw->container.outline_sep_style;
	}
    if (ncw->container.ob_policy != ccw->container.ob_policy)
	{
    	if (!XmRepTypeValidValue(XmRID_OUTLINE_BUTTON_POLICY,
		    ncw->container.ob_policy,nw))
	    ncw->container.ob_policy =
		    ccw->container.ob_policy;
	}
    if (ncw->container.spatial_style != ccw->container.spatial_style)
	{
    	if (!XmRepTypeValidValue(XmRID_SPATIAL_STYLE,
                    ncw->container.spatial_style,nw))
            ncw->container.spatial_style =
		    ccw->container.spatial_style;
	}
    if (ncw->container.primary_ownership != ccw->container.primary_ownership)
	{
    	if (!XmRepTypeValidValue(XmRID_PRIMARY_OWNERSHIP,
                    ncw->container.primary_ownership,nw))
            ncw->container.primary_ownership =
		    ccw->container.primary_ownership;
	}
    if (ncw->container.resize_model != ccw->container.resize_model)
	{
    	if (!XmRepTypeValidValue(XmRID_SPATIAL_RESIZE_MODEL,
		    ncw->container.resize_model,nw))
	    ncw->container.resize_model =
		    ccw->container.resize_model;
	}
    if (ncw->container.selection_policy != ccw->container.selection_policy)
	{
    	if (!XmRepTypeValidValue(XmRID_SELECTION_POLICY,
                    ncw->container.selection_policy,nw))
            ncw->container.selection_policy =
		    ccw->container.selection_policy;
	}
   if (ncw->container.selection_technique != ccw->container.selection_technique)
	{
    	if (!XmRepTypeValidValue(XmRID_SELECTION_TECHNIQUE,
                    ncw->container.selection_technique,nw))
            ncw->container.selection_technique =
		    ccw->container.selection_technique;
	}
    if (ncw->container.snap_model != ccw->container.snap_model)
	{
    	if (!XmRepTypeValidValue(XmRID_SPATIAL_SNAP_MODEL,
                    ncw->container.snap_model,nw))
            ncw->container.snap_model =
		    ccw->container.snap_model;
	}
    if (ncw->container.render_table == NULL)
	{
	    XmFontList	defaultFont = NULL;

	    defaultFont = XmeGetDefaultRenderTable(nw,XmLABEL_FONTLIST);
	    ncw->container.render_table = XmFontListCopy(defaultFont);
	}
    else
	ncw->container.render_table =
	    XmFontListCopy(ncw->container.render_table);

    if (ccw->container.render_table)
        XmRenderTableFree(ccw->container.render_table);


    /*
     * Check if we'll need layout.
     * we update 4 differents boolean and at the end we
     * decide if relayout is actually needed.
     */
    if (ncw->container.layout_type != ccw->container.layout_type)
	{
	if (CtrLayoutIsSPATIAL(ncw) && CtrSpatialStyleIsNONE(ncw))
	    need_expose = True;
	else
	    container_need_layout = True;
	}

    if (ncw->manager.string_direction != ccw->manager.string_direction)
	container_need_layout = True;

    if ((ncw->container.margin_h != ccw->container.margin_h) ||
        (ncw->container.margin_w != ccw->container.margin_w))
	container_need_layout = True;

    if ((ncw->container.spatial_style != ccw->container.spatial_style) &&
	(!(CtrLayoutIsSPATIAL(ncw) && CtrSpatialStyleIsNONE(ncw))))
	spatial_need_layout = True;

    /*
     * Change all IconGadget children's XmNentryType to match
     */
    if ((ncw->container.entry_viewtype != ccw->container.entry_viewtype) &&
	(ncw->container.entry_viewtype != XmANY_ICON)) {
	ChangeView(nw,ncw->container.entry_viewtype);
	container_need_layout = True;
    }
    else
	{
    	/*
    	 * If we're changing to XmSPATIAL from XmDETAIL || XmOUTLINE we need to
    	 * resize the Icons so they get rid of their "detail" room.
    	 */
    	if (CtrLayoutIsSPATIAL(ncw) && !CtrLayoutIsSPATIAL(ccw))
	    {
    	    XtWidgetGeometry    desired;

    	    node = ncw->container.first_node;
    	    while (node)
        	{
        	cwid = node->widget_ptr;
		/*
		 * Get and set preferred size
		*/
		XtQueryGeometry(cwid, NULL, &desired);
		XmeConfigureObject(cwid,cwid->core.x,cwid->core.y,
					desired.width,desired.height,0);
        	/*
        	 * Get the next cwid, whether it's managed or not
        	 */
		node = node->next_ptr;
		}
            }
	}

    if ((ncw->container.entry_viewtype != ccw->container.entry_viewtype) &&
        ((ncw->container.entry_viewtype == XmSMALL_ICON) ||
	 (ccw->container.entry_viewtype == XmSMALL_ICON)))
	{
	ncw->container.current_width_in_cells = 0;
	ncw->container.current_height_in_cells = 0;
	spatial_need_layout = True;
	}

    if (CtrLayoutIsSPATIAL(ncw) && !CtrSpatialStyleIsNONE(ncw) &&
        (CtrViewIsLARGE_ICON(ncw) || CtrViewIsANY_ICON(ncw)))
        {
	if ((ncw->container.large_cell_width
			!= ccw->container.large_cell_width) &&
	    (ncw->container.large_cell_width == 0))
	    ncw->container.large_cell_dim_fixed = False;
        if ((ncw->container.large_cell_height
			!= ccw->container.large_cell_height) &&
	    (ncw->container.large_cell_height == 0))
	    ncw->container.large_cell_dim_fixed = False;

        SetCellSizes(nw);
	if ((ncw->container.real_large_cellw
			!= ccw->container.real_large_cellw) ||
	    (ncw->container.real_large_cellh
			!= ccw->container.real_large_cellh))
	    {
            ncw->container.current_width_in_cells = 0;
	    ncw->container.current_height_in_cells = 0;
            spatial_need_layout = True;
	    }
        }

    if (CtrLayoutIsSPATIAL(ncw) && !CtrSpatialStyleIsNONE(ncw) &&
        CtrViewIsSMALL_ICON(ncw))
        {
	if ((ncw->container.small_cell_width
                        != ccw->container.small_cell_width) &&
            (ncw->container.small_cell_width == 0))
            ncw->container.small_cell_dim_fixed = False;
        if ((ncw->container.small_cell_height
                        != ccw->container.small_cell_height) &&
            (ncw->container.small_cell_height == 0))
            ncw->container.small_cell_dim_fixed = False;

        SetCellSizes(nw);
	if ((ncw->container.real_small_cellw
                        != ccw->container.real_small_cellw) ||
            (ncw->container.real_small_cellh
                        != ccw->container.real_small_cellh))
            {
	    ncw->container.current_width_in_cells = 0;
            ncw->container.current_height_in_cells = 0;
            spatial_need_layout = True;
	    }
        }

    /*
     * Deal with setting of special selectColor values.
     * No need to check for a change since if select_color is one of
     * the special value, it is always a change - only pixel
     * could have been there at that point in time
     */
    if (ncw->container.select_color == XmDEFAULT_SELECT_COLOR) {
        XrmValue val;

        _XmSelectColorDefault(nw, 0, &val);
        ncw->container.select_color = *((Pixel*) val.addr);
        /* must notify the children at this point */
	need_expose |= _XmNotifyChildrenVisual (cw, nw, VisualSelectColor);
    } else
    if (ncw->container.select_color == XmHIGHLIGHT_COLOR) {
        ncw->container.select_color = ncw->manager.highlight_color ;
        /* must notify the children at this point */
	need_expose |= _XmNotifyChildrenVisual (cw, nw, VisualSelectColor);
    }


    /*
     * Hide all non-level-0 children if we've switched to SPATIAL layout.
     */
    if (CtrLayoutIsSPATIAL(ncw) && !CtrLayoutIsSPATIAL(ccw))
	{
	for (i = 0; i < ncw->composite.num_children; i++)
	    {
	    cwid = ncw->composite.children[i];
	    c = GetContainerConstraint(cwid);
	    if (!CtrICON(cwid) || (c->entry_parent))
		HideCwid(cwid);
	    }
	}

    if ((ncw->container.collapsed_state_pixmap !=
	 ccw->container.collapsed_state_pixmap) ||
	(ncw->container.expanded_state_pixmap !=
	 ccw->container.expanded_state_pixmap)) {
	/* compute the size of the new outline buttons */
	SizeOutlineButton(nw);
	/* if the size has change, mark it for relayout */
	if ((ncw->container.ob_width != ccw->container.ob_width) ||
	    (ncw->container.ob_height != ccw->container.ob_height))
	    outline_need_layout = True;
	/* update all the buttons */
	ChangeOutlineButtons(nw);
    }

    /*
     * just Redisplay, if line style has changed.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(ncw) &&
        (ncw->container.outline_sep_style != ccw->container.outline_sep_style))
	need_expose = True;


    /*
     * Recalculate FirstColWidth.
     *
     */
    if (ncw->container.first_col_width != ccw->container.first_col_width) {
	if (!ncw->container.first_col_width) {
	    ncw->container.dynamic_resource |= FIRSTCW;
	} else {
	    ncw->container.dynamic_resource &= ~FIRSTCW;
	    ncw->container.real_first_col_width =
		ncw->container.first_col_width ;
	}
	outline_need_layout = True ;
	ncw->container.first_col_width = INVALID_DIMENSION ;
    }

    if ((ncw->container.ob_width != ccw->container.ob_width) ||
	(ncw->container.outline_indent != ccw->container.outline_indent) ||
	(ncw->container.ob_policy != ccw->container.ob_policy))
	outline_need_layout = True ;

    /*
     * deal with XmNdetailTabList
     */
    if ((ncw->container.detail_tablist != ccw->container.detail_tablist))
	{
	detail_need_layout = True ;

        if (ccw->container.detail_tablist)
	    XmTabListFree(ccw->container.detail_tablist) ;

	if (ncw->container.detail_tablist == NULL)
	    ncw->container.dynamic_resource |= TABLIST;
	else
	    ncw->container.dynamic_resource &= ~TABLIST;
	if (ncw->container.detail_tablist != NULL)
	    ncw->container.detail_tablist =
		XmTabListCopy(ncw->container.detail_tablist,0,0);
        }

    /*
     * deal with XmNdetailOrder & XmNdetailOrderCount changes
     */
    if ((ncw->container.detail_order !=
	 ccw->container.detail_order) ||
	(ncw->container.detail_order_count !=
	 ccw->container.detail_order_count)) {

	detail_need_layout = True ;

	if (ccw->container.detail_order_count
	    && ccw->container.detail_order) {
	    XtFree((char*)ccw->container.detail_order) ;
	}
	if (ncw->container.detail_order_count &&
	    ncw->container.detail_order) {
	    Cardinal * detail_order ;
	    detail_order = (Cardinal *)
		XtMalloc(sizeof(Cardinal) * ncw->container.detail_order_count);
	    for (i=0; i < ncw->container.detail_order_count; i++)
		detail_order[i] = ncw->container.detail_order[i] ;
	    ncw->container.detail_order = detail_order ;
	}
    }


    /*
     * Reset cell/placed info if Spatial Layout options
     * have changed.
     */
    if (CtrLayoutIsSPATIAL(ncw) &&
	((ncw->container.layout_type != ccw->container.layout_type) ||
         (ncw->container.spatial_style != ccw->container.spatial_style)))
	{
	if (((XmContainerWidgetClass)XtClass(ncw))->container_class.place_item)
	    (*((XmContainerWidgetClass)
		XtClass(ncw))->container_class.place_item)(nw,NULL,ANY_FIT);
	need_expose = True;
	}

    /*
     * Set selected items, if necessary.
     */
    if ((ncw->container.selected_item_count
				!= ccw->container.selected_item_count) ||
	(ncw->container.selected_items
				!= ccw->container.selected_items))
	{
	int	save_item_count;

	save_item_count = ncw->container.selected_item_count;
	ncw->container.selected_item_count =
	    ccw->container.selected_item_count;
	DeselectAllCwids(nw);	/* ncw->container.selected_item_count = 0 */
	if (ncw->container.selected_items != NULL)
	    {
	    for (i = 0; i < save_item_count; i++)
		/* MarkCwid increments ncw->container.selected_item_count */
		MarkCwid(ncw->container.selected_items[i],False);
	    ncw->container.selected_items = NULL;
	    }
	need_expose = True;
	}

    /*
     * Change GC's, if necessary.
     */
    if ((ncw->manager.foreground != ccw->manager.foreground) ||
	(ncw->core.background_pixel != ccw->core.background_pixel))
	UpdateGCs(nw);

    {
 	/*
 	** Deal with heading change
 	**
 	** There are two resources which affect whether or not the icon header
 	** is created and what is in it (not visibility): XmNdetailColumnHeading
 	** and XmNdetailColumnHeadingCount. The icon header is created if and
 	** only if we have string data in detail_heading and a positive !0 count
 	** in detail_heading_count. That much is simple. Complicating the issue
 	** (and necessary to preserve for compatibilty with subclasses) is the
 	** attempt both to catch a widget creation/destruction sequence in
	** which the widget data happens to be at the same address and also to
 	** catch the case in which one of the resources is set without the
 	** other; the idea is that the detail_heading_count is marked invalid
 	** and the real value is moved out to a cached space in the widget
 	** record. (An earlier scheme which did similar things to the
 	** detail_heading field was removed when it became clear that the
 	** memory-management issues were impossible to track correctly.)
 	** (This scheme works except in the case in which the user has set a
 	** new real count to be higher than the number of strings in the
 	** existing string table; however, this can be considered user error,
 	** as it is for other cases in which widgets use combined list/number
 	** resource pairs rather than a resource which is a structure which
 	** contains the two values.)
 	**
 	** What this means is that detail_heading has the real values NULL
 	** and !NULL, and the transitions which are valid for it are between
 	** those two values. But detail_heading_count has three possible
 	** values: 0, which indicates that there has never been a count
 	** assigned, or indicates that the current count is 0; INVALID_COUNT,
 	** which indicates that a real value has been set and that that value
 	** is now in saved_detail_heading_count; and some other non-zero value,
 	** which is a new value coming into this routine or into Initialize.
 	** Valid transitions are as usual, with INVALID_COUNT taking the place
 	** of regular !0 values.  Note that the transition to 0 is valid.
 	**
 	** At the end of the block, detail_heading is of course either NULL or
 	** !NULL and detail_heading_count is either 0 or INVALID_COUNT. The
 	** icon header exists when there is a valid count, either in this
 	** iteration or previously set, and valid data. We don't care whether
 	** or not the icon header is to be displayed; just indicate whether it
 	** was changed so that we know whether we need a relayout.
 	*/

#define NONE_CHANGED 	0
#define COUNT_CHANGED	(1<<0)
#define HEADING_CHANGED	(1<<1)
	unsigned char whichChanged = NONE_CHANGED;

#define NONE_VALID	0
#define COUNT_VALID	(1<<0)
#define HEADING_VALID	(1<<1)
#define BOTH_VALID	(COUNT_VALID | HEADING_VALID)
 	unsigned char whichValid = NONE_VALID;

 	if (ncw->container.detail_heading != ccw->container.detail_heading)
	    whichChanged |= HEADING_CHANGED;

 	if ((INVALID_COUNT != ncw->container.detail_heading_count)
	    && (ncw->container.detail_heading_count
		!= ncw->container.saved_detail_heading_count))
	    whichChanged |= COUNT_CHANGED;

 	if (NONE_CHANGED != whichChanged) {

	    if (ncw->container.detail_heading != NULL)
		whichValid |= HEADING_VALID;

	    if (HEADING_VALID & whichValid) {

		if (HEADING_CHANGED & whichChanged) {

		    /* We have a new set of heading strings.
		    ** If we have a heading but the count is invalid, then we
		    ** need to fetch it from one of the caches, so that the
		    ** user can set the heading and the count in separate
		    ** set-values invocations (really, we could just store the
		    ** value where it goes, except that we want to track a
		    ** change in the count in a memory-reuse case)
		    */

		    if (INVALID_COUNT == ncw->container.detail_heading_count) {

			if (ncw->container.icon_header) {
			    XtVaGetValues(GetRealIconHeader(nw), XmNdetailCount,
					  &(ncw->container.detail_heading_count),
					  NULL);
			    /* the labelString takes detail_heading[0] */
			    ncw->container.detail_heading_count++;
			}
			else
			    ncw->container.detail_heading_count =
				ncw->container.saved_detail_heading_count;
		    }
		    /* else we will use whatever value has been
		    ** set -- either 0 or a new value
		    */
		}
		/* else use existing set of strings below; count must
		** have changed
		*/
	    }
	    /* else if now NULL will result in destruction below */

	    /* changing a count without a valid heading doesn't require any work;
	    ** in fact, we don't need to track a change to the count -- we just
	    ** care about the current value or the value reconstructed above.
	    */
	    if ( (ncw->container.detail_heading_count != 0)
		&& (INVALID_COUNT != ncw->container.detail_heading_count) )
		whichValid |= COUNT_VALID;

	    if (BOTH_VALID == (BOTH_VALID & whichValid)) {

		/* we have valid data, and at least one resource changed */
		/* we need an icon header; either create or update it */
		assert(ncw->container.detail_heading);
		assert(ncw->container.detail_heading_count);
		assert(INVALID_COUNT != ncw->container.detail_heading_count);

		if (ccw->container.icon_header == NULL)
		    CreateIconHeader(nw);
		else {
		    /* pass False if there was a change in the strings as well
		    ** as the count, True if it's only a change in the detail
		    ** heading count.
		    */
		    UpdateIconHeader(nw,
			    (HEADING_CHANGED & whichChanged) ? False : True);
		}

		if (CtrLayoutIsDETAIL(ncw)) {
		    assert(ncw->container.icon_header);
		    XtManageChild(ncw->container.icon_header);

		    /* may be redundant if this is a new state */
		    detail_need_layout = True;
		}
	    }
	    else {
		/* we need no icon header; if it existed, destroy it */
		if (ncw->container.icon_header) {
		    XtDestroyWidget(ncw->container.icon_header);
		    ncw->container.icon_header = NULL;
		    if (CtrLayoutIsDETAIL(ncw))
			detail_need_layout = True;	/* may be redundant if
							   this is a new state */
		}
	    }

	    if (INVALID_COUNT != ncw->container.detail_heading_count)
		/* including 0 */
		ncw->container.saved_detail_heading_count =
		    ncw->container.detail_heading_count;

	    if (COUNT_VALID & whichValid)
		ncw->container.detail_heading_count = INVALID_COUNT;

	    /* else if it is 0 don't do anything -- let value stay; the
	    ** saved value is either 0 or a real !0 value but cannot be
	    ** INVALID_COUNT
	    */
  	}
 	/* else no change to either, so just continue with SetValues code */

#undef NONE_CHANGED
#undef COUNT_CHANGED
#undef HEADING_CHANGED
#undef NONE_VALID
#undef COUNT_VALID
#undef HEADING_VALID
    }

    if (CtrLayoutIsDETAIL(ncw) && !CtrLayoutIsDETAIL(ccw) &&
	(ncw->container.icon_header != NULL)) {
	detail_need_layout = True ;
	XtManageChild(ncw->container.icon_header);
    }

    if (!CtrLayoutIsDETAIL(ncw) && CtrLayoutIsDETAIL(ccw) &&
	(ncw->container.icon_header)) {
	outline_need_layout = True ;
	XtUnmanageChild(ncw->container.icon_header);
	if (XtParent(ncw->container.icon_header) != nw) {
	    Widget real_icon_header = GetRealIconHeader(nw);
	    /* reposition the container itself too */
	    nw->core.y -= real_icon_header->core.height ;
	}
    }



    /*
     * Update header margins, only if we are in the detail SW case
     * where the container.icon_header is the DA.
     */
    if ((ncw->container.margin_w != ccw->container.margin_w) ||
	(ncw->container.margin_h != ccw->container.margin_h) ||
	(ncw->manager.foreground != ccw->manager.foreground) ||
	(ncw->core.border_width != ccw->core.border_width) ||
	(ncw->core.background_pixel != ccw->core.background_pixel) ||
	(ncw->container.layout_type != ccw->container.layout_type) ||
	(ncw->core.background_pixmap != ccw->core.background_pixmap)) {

	if ((CtrLayoutIsDETAIL(ncw)) &&
	    (ncw->container.icon_header) &&
	    (XtParent(ncw->container.icon_header) != nw)) {

	    Widget real_icon_header = GetRealIconHeader(nw);

	    /* we need to update the margin on the DA, resize it
	       and reposition the container. */

	   /* if (0 == nw->core.y) */
	    nw->core.y = real_icon_header->core.height ;

	    XtVaSetValues(ncw->container.icon_header,
			  XmNmarginWidth, ncw->container.margin_w,
			  XmNmarginHeight, ncw->container.margin_h,
			  XmNbackground, ncw->core.background_pixel,
			  XmNbackgroundPixmap, ncw->core.background_pixmap,
			  XmNforeground, ncw->manager.foreground,
			  NULL);

	    XmeConfigureObject(ncw->container.icon_header, /* the DA */
			       ncw->container.icon_header->core.x,
			       ncw->container.icon_header->core.y,
			       real_icon_header->core.width +
			       2*ncw->container.margin_w,
			       real_icon_header->core.height +
			       ncw->container.margin_h,
			       ncw->core.border_width);
	}
    }

    /* need also the check for change in borderPixel, borderPixmap... */


    /*
     * Set internal variables that are based on resource values.
     */
    if (ncw->container.selection_policy != ccw->container.selection_policy)
	ncw->container.kaddmode =
		(CtrPolicyIsMULTIPLE(ncw) || CtrPolicyIsSINGLE(ncw));
    if (ncw->container.selection_technique !=
				ccw->container.selection_technique)
	ncw->container.marquee_mode =
	    (CtrTechIsTOUCH_ONLY(ncw) ? False : True);


    /** perform some logic */
    spatial_need_layout |= container_need_layout ;
    outline_need_layout |= container_need_layout ;
    detail_need_layout |= outline_need_layout ;


    if (CtrLayoutIsSPATIAL(ncw) && !CtrSpatialStyleIsNONE(ncw) &&
	!spatial_need_layout &&
	(ncw->container.snap_model != ccw->container.snap_model)) {
	node = GetFirstNode(ncw);
        while (node)
            {
	    (void)SnapCwid(node->widget_ptr, node->widget_ptr->core.x,
			node->widget_ptr->core.y, &snap_point);
	    XmeConfigureObject(node->widget_ptr,
		snap_point.x,snap_point.y,
		node->widget_ptr->core.width,node->widget_ptr->core.height,0);
            node = GetNextNode(node);
            }
	need_expose = True ;
    }

    if (((CtrLayoutIsOUTLINE_DETAIL(ncw) && outline_need_layout) ||
	(CtrLayoutIsDETAIL(ncw) && detail_need_layout) ||
	(CtrLayoutIsSPATIAL(ncw) && spatial_need_layout)))
	{
	XmClipWindowTrait clipWindowTrait;
	XmScrollFrameTrait scrollFrameTrait;
	Widget swparent;

	/* get our preferred size first */
	ncw->container.ideal_width = 0;
	ncw->container.ideal_height = 0;
	GetSize(nw,&ncw->container.ideal_width,&ncw->container.ideal_height);
	if (ncw->container.ideal_width && ncw->container.ideal_height && GetFirstNode(ncw)) {
	ncw->core.width = ncw->container.ideal_width;
	ncw->core.height = ncw->container.ideal_height;
	}
	Layout(nw);

	/*
	 * update any scrolled window above to reflect a new original
	 * y coordinate - CDExc19939 & CDExc23393
	 */

	/* find a scrolled window*/
	swparent = XtParent(ncw);
	clipWindowTrait = (XmClipWindowTrait)
		XmeTraitGet((XtPointer) XtClass(swparent), _XmQTclipWindow);
	if (clipWindowTrait != NULL) swparent = XtParent(swparent);

	scrollFrameTrait = (XmScrollFrameTrait)
		XmeTraitGet((XtPointer) XtClass(swparent), XmQTscrollFrame);

	/* if we have one, update the geom */
	if (scrollFrameTrait != NULL && scrollFrameTrait->version >= 1 &&
	    scrollFrameTrait->getInfo(swparent, NULL, NULL, NULL) &&
	    scrollFrameTrait->updateOrigGeom != NULL)
	{
		XtWidgetGeometry preferred;

		preferred.request_mode = CWY;
		preferred.y = ncw->core.y;

		scrollFrameTrait->updateOrigGeom(swparent,
						 (Widget) ncw,
						 &preferred);
	}

	need_expose = True ;
    	}

    ncw->container.self = False;
    return(need_expose);
}


/*
 * XmRCallProc routine for checking font before setting it to NULL
 * if no value is specified for both XmNrenderTable and XmNfontList.
 * If "first_change_managed" is True, this function has been called
 * twice on same widget, thus * resource needs to be set NULL, otherwise
 * leave it alone.
 */

static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmContainerWidget cw = (XmContainerWidget)wid;

  if (cw->container.first_change_managed) /* Been here before, so set resource = NULL */
	value->addr = NULL;
  else  {
	cw->container.first_change_managed = True;
	value->addr = (char*)&(cw->container.render_table);
  }
}


/*
 * XmRCallProc routine to determine the correct default collapsed
 * pixmap for this layout direction.
 */


static void
DefaultCollapsedPixmap(Widget wid,
		       int offset,
		       XrmValue *value)
{
  static Pixmap result;
  XmContainerWidget cw = (XmContainerWidget)wid;

  result =
    XmGetPixmapByDepth(XtScreen(wid),
		       (LayoutIsRtoLM(cw) ? "collapsed_rtol" : "collapsed"),
		       cw->manager.foreground,
		       cw->core.background_pixel,
		       cw->core.depth);

  value->size = sizeof(result);
  value->addr = (char *) &result;
}

/************************************************************************
 *
 *  QueryGeometry (Core Method)
 *
 ************************************************************************/
static XtGeometryResult
QueryGeometry(
        Widget widget,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    /* deal with user initial size setting */
    if (!XtIsRealized(widget))  {
	desired->width = XtWidth(widget) ;    /* might be 0 */
	desired->height = XtHeight(widget) ;  /* might be 0 */
    } else {
	/* always computes natural size afterwards */
	desired->width = 0 ;
	desired->height = 0 ;
    }

    GetSize (widget, &desired->width, &desired->height);

    /* this function will set CWidth and CHeight */
    return XmeReplyToQueryGeometry(widget, intended, desired) ;
}


/************************************************************************
 * GetSize
 *
 * Updates the following container instance variables:
 *	cw->container.ideal_width
 *	cw->container.ideal_height
 *	cw->container.first_column_width (if dynamic)
 ************************************************************************/
static  void
GetSize (
        Widget                  wid,
        Dimension * pwidth,
        Dimension * pheight)
{
    XmContainerWidget  cw = (XmContainerWidget)wid;
    CwidNode		node;
    Widget		cwid;
    XmContainerConstraint c;
    XtWidgetGeometry 	desired;
    XmTabList		save_dynamic_tablist = NULL;
    Position		save_x;


    if (CtrLayoutIsSPATIAL(cw))
	/*
	 * Get width & height from class method.
	 */
	{
	if (((XmContainerWidgetClass)
	     XtClass(wid))->container_class.get_spatial_size)
	    {
	    (*((XmContainerWidgetClass)
	       XtClass(wid))->container_class.get_spatial_size)
		(wid, pwidth, pheight);
	    cw->container.ideal_width = *pwidth;
	    cw->container.ideal_height = *pheight;
	    }
	return;
	}
    /*
     * Calculate ideal width/height for OUTLINE/DETAIL.
     */
    cw->container.ideal_width = 0;
    cw->container.ideal_height = cw->container.margin_h;

    /*
     * Find desired First Column width
     */
    if (CtrIsDynamic(cw,FIRSTCW)) {
        cw->container.real_first_col_width = GetDynFirstColWidth(wid);
    }

    /*
     * Save the current dynamic tablist and set to NULL, so Icon's
     * response to QueryGeo will not be based on current width.
     */
    if (CtrIsDynamic(cw,TABLIST))
	{
	save_dynamic_tablist = cw->container.detail_tablist;
	cw->container.detail_tablist = NULL;
	}

    /* header first. */
    if ((cwid = GetRealIconHeader(wid)) && (XtIsManaged(cwid)) &&
        ((XtParent(cwid) == wid) || (XtIsManaged(XtParent(cwid)))))
        {
        XtQueryGeometry(cwid, NULL, &desired);
	save_x = cwid->core.x;
	cwid->core.x = MAX(0,cwid->core.x);
	cw->container.ideal_width = MAX(cw->container.ideal_width,
					cwid->core.x + desired.width);
	cw->container.ideal_height += desired.height;
	cwid->core.x = save_x;
	}

    /* then the icons */
    node = GetFirstNode(cw);
    while(node)
	{
	cwid = node->widget_ptr;
	c = GetContainerConstraint(cwid);
	/*
	 This query depends on the current first column width
	 and the current x position.  It assumes L to R, since the
	 width/height would be the same.
	 */
 	save_x = cwid->core.x;
	cwid->core.x = cw->container.margin_w +
	    c->depth*cw->container.outline_indent
	    + ((CtrOB_PRESENT(cw))?cw->container.ob_width:0) ;
	XtQueryGeometry(cwid, NULL, &desired);
	cw->container.ideal_width = MAX(cw->container.ideal_width,
					cwid->core.x + desired.width);
	cw->container.ideal_height += desired.height;
	cwid->core.x = save_x;
	node = GetNextNode(node);
	}

    cw->container.ideal_width += cw->container.margin_w;
    cw->container.ideal_height += cw->container.margin_h;

    /* Reset dynamic XmTabList, if necessary */
    if (CtrIsDynamic(cw,TABLIST))
        cw->container.detail_tablist = save_dynamic_tablist;

    /* here if we are in the dynamic first column case and in detail,
       with dynamic tablist, we want to grow up front by the amount
       of the difference between the old first column and the new one.
       In outline, that should happen automatacilly, but in detail,
       if tablist is dynamic, we have to compute it here.

       [ samborn, 25-MAY-1999 ]

       This used to be done in {Expand,Collapse}Cwid, but moved it
       here.  The reason is that it needs to be applied to the ideal,
       not the current width in core. */
    if (CtrLayoutIsDETAIL(cw) && CtrIsDynamic(cw,TABLIST)) {
      cw->container.ideal_width += cw->container.real_first_col_width;
    }

    if (!(*pwidth)) *pwidth = cw->container.ideal_width;
    if (!(*pheight)) *pheight = cw->container.ideal_height;
}

/************************************************************************
 * GeometryManager (Composite Method)
 ************************************************************************/

static XtGeometryResult
GeometryManager(
    Widget 		cwid,
    XtWidgetGeometry 	*desired,
    XtWidgetGeometry 	*allowed) /* unused */
{
    Widget		wid = XtParent(cwid);
    XmContainerWidget	cw = (XmContainerWidget)wid;
    XmContainerConstraint c = GetContainerConstraint(cwid);
    Position		save_x,save_y;
    Dimension		save_width, save_height, save_border_width;
    unsigned char	save_include_model;

    /*
     * Position and changes always allowed for spatial None
     */
    if (CtrSpatialStyleIsNONE(cw)) {
      /* Move the child and save the original positions */
      if (desired->request_mode & CWX) {
	cwid->core.x = desired->x;
      }
      if (desired->request_mode & CWY) {
	cwid->core.y = desired->y;
      }
      if (desired->request_mode & CWWidth) {
	cwid->core.width = desired->width;
      }
      if (desired->request_mode & CWHeight) {
	cwid->core.height = desired->height;
      }
      if (desired->request_mode & CWBorderWidth) {
	cwid->core.border_width = desired->border_width;
      }

      /* Request new size */
      RequestSpatialGrowth(wid, cwid);

      return(XtGeometryYes);
    }

    /*
     * Called from inside Container (ChangeView routine)
     * Container will resize later, so do the job and
     * return XtGeometryYes.
     */
    if (cw->container.self ||
     /* No Spatial Layout methods? or not placed? or spatialStyleNONE?
      * Do whatever it wants.
      */
	(!(((XmContainerWidgetClass)XtClass(wid))->
	   container_class.remove_item)) ||
	(!(((XmContainerWidgetClass)XtClass(wid))->
	   container_class.place_item)) ||
	(!CtrItemIsPlaced(cwid))) {

	if (desired->request_mode & CWX)
	    cwid->core.x = desired->x;
	if (desired->request_mode & CWY)
	    cwid->core.y = desired->y;
	if (desired->request_mode & CWWidth)
	    cwid->core.width = desired->width;
	if (desired->request_mode & CWHeight)
	    cwid->core.height = desired->height;
	if (desired->request_mode & CWBorderWidth)
	    cwid->core.border_width = desired->border_width;
	return(XtGeometryYes);
    }

    /*
     * Position change allowed for Spatial layout && PlaceModel == CLOSEST
     */
    if (((desired->request_mode & CWX) || (desired->request_mode & CWY)) &&
	((!CtrLayoutIsSPATIAL(cw)) || !CtrIncludeIsCLOSEST(cw)))
	return(XtGeometryNo);

    save_x = c->user_x;
    save_y = c->user_y;
    save_width = cwid->core.width;
    save_height = cwid->core.height;
    save_border_width = cwid->core.border_width ;

    if (desired->request_mode & CWX)
	c->user_x = desired->x;
    if (desired->request_mode & CWY)
	c->user_y = desired->y;
    if (desired->request_mode & CWWidth)
	cwid->core.width = desired->width;
    if (desired->request_mode & CWHeight)
	cwid->core.height = desired->height;
    if (desired->request_mode & CWBorderWidth)
	cwid->core.border_width = desired->border_width;

    /*
     * Outline/Detail Layout case.
     * Request are always granted, relayout is done.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw)) {
	XtWidgetGeometry    geo_desired;

	geo_desired.width = 0;
	geo_desired.height = 0; /* means resize as you want */
	RequestOutlineDetail(wid, &geo_desired);

	return(XtGeometryYes);
    }

    (*((XmContainerWidgetClass)
	XtClass(wid))->container_class.remove_item)(wid,cwid);

    /*
     * Fake out include model, so we get placed in the same spot.
     */
    if (!CtrIncludeIsCLOSEST(cw))
	{
	c->user_x = cwid->core.x;
	c->user_y = cwid->core.y;
	}
    save_include_model = cw->container.include_model;
    cw->container.include_model = XmCLOSEST;
    (*((XmContainerWidgetClass)
	XtClass(wid))->container_class.place_item)(wid,cwid,EXACT_FIT);
    cw->container.include_model = save_include_model;
    if (CtrItemIsPlaced(cwid))
      return(XtGeometryYes);

    /*
     * See if we can grow and then re-layout to try again.
     */
    if (RequestSpatialGrowth(wid,cwid))
      LayoutSpatial(wid,False,NULL);

    cwid->core.width = save_width;
    cwid->core.height = save_height;
    cwid->core.border_width = save_border_width;
    if (CtrItemIsPlaced(cwid))
	return(XtGeometryYes);
    /*
     * Place the item back where it was.
     */
    if (CtrIncludeIsCLOSEST(cw))
	{
	c->user_x = save_x;
	c->user_y = save_y;
	}
    save_include_model = cw->container.include_model;
    cw->container.include_model = XmCLOSEST;
    (*((XmContainerWidgetClass)
        XtClass(wid))->container_class.place_item)(wid,cwid,FORCE);
    cw->container.include_model = save_include_model;
    return(XtGeometryNo);
}

/************************************************************************
 * ChangeManaged (Composite Method)
 ************************************************************************/
static void
ChangeManaged(
        Widget	wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    int                 i;
    Widget              cwid;
    XmContainerConstraint c;

    /*
     * If it's one of our changes (SetValues), let's leave.
     */
    if (cw->container.self) return;

    for (i = 0; i < cw->composite.num_children; i++)
        {
        cwid = cw->composite.children[i];
        c = GetContainerConstraint(cwid);
	if (CtrICON(cwid))
	    if (!NodeIsActive(c->node_ptr))
		HideCwid(cwid);
	if (CtrLayoutIsSPATIAL(cw) &&
	    (CtrOUTLINE_BUTTON(cwid) || CtrHEADER(cwid) || (c->entry_parent)))
	    HideCwid(cwid);
	}

    if (CtrLayoutIsOUTLINE_DETAIL(cw))
	ChangeManagedOutlineDetail(wid);
    else
	ChangeManagedSpatial(wid);
}

/************************************************************************
 * ChangeManagedOutlineDetail (Private Function)
 ************************************************************************/
static void
ChangeManagedOutlineDetail(
        Widget  wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    XtWidgetGeometry    geo_desired;

    if (!XtIsRealized(wid)) {
	geo_desired.width = cw->core.width ;
	geo_desired.height = cw->core.height ; /* can be 0 too */
    } else {
	geo_desired.width = 0;
	geo_desired.height = 0;
    }

    RequestOutlineDetail(wid, &geo_desired);
    cw->container.first_change_managed = False;
}

/************************************************************************
 * ChangeManagedSpatial (Private Function)
 ************************************************************************/
static void
ChangeManagedSpatial(
        Widget  wid)
{
  XmContainerWidget   cw = (XmContainerWidget)wid;
  Widget              cwid;
  XtWidgetGeometry	geo_desired;
  CwidNode		node;

  if ((CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw)))
    {
    if ((CtrViewIsSMALL_ICON(cw) && (!cw->container.small_cell_dim_fixed)) ||
        ((CtrViewIsLARGE_ICON(cw) || CtrViewIsANY_ICON(cw)) &&
		(!cw->container.large_cell_dim_fixed)))
	SetCellSizes(wid);
    }

  if (cw->container.first_change_managed)
    /*
     * First time through.  Get initial size from QueryGeo
     */
    {
      if (((XmContainerWidgetClass)
           XtClass(wid))->container_class.get_spatial_size) {

          Dimension width, height;

          if (!XtIsRealized(wid)) {
                width = cw->core.width; /* Can be 0 */
                height = cw->core.height;
          } else {
          width = 0;
          height = 0;
          }
          (*((XmContainerWidgetClass)
             XtClass(wid))->container_class.get_spatial_size)
              (wid, &width, &height);

          geo_desired.request_mode = (CWWidth | CWHeight);
          geo_desired.width = width;
          geo_desired.height = height;
          if (geo_desired.width < 1) geo_desired.width = cw->core.width ;
          if (geo_desired.height < 1) geo_desired.height = cw->core.height ;
          _XmMakeGeometryRequest(wid, &geo_desired);
      }
    cw->container.first_change_managed = False;
   }

  node = cw->container.first_node;
  while (node)
    /*
     * Now loop through the linked list to see who needs to be
     * removed from the Spatial Layout.
     */
    {
      cwid = node->widget_ptr;
      if ((!(XtIsManaged(cwid))) && CtrItemIsPlaced(cwid) &&
	  (((XmContainerWidgetClass)
	    XtClass(wid))->container_class.remove_item))
	(*((XmContainerWidgetClass)
	   XtClass(wid))->container_class.remove_item)(wid,cwid);
      node = node->next_ptr;
    }

  /*
   * Layout will handle placing newly-managed cwids
   */
  LayoutSpatial(wid,True,NULL);
}


/************************************************************************
 * RequestSpatialGrowth (Private Function)
 *
 * As opposed to GetSpatialSize which makes a calculation based upon
 * placing all icons in a configuration as close to a square as possible,
 * this function calculates only the additional size needed to accomodate
 * the one icon passed in as "cwid".  This routine also makes the
 * Geometry request.
 ************************************************************************/
static Boolean
RequestSpatialGrowth(
    Widget	wid,
    Widget	cwid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    XmContainerConstraint c;
    Boolean	grow_width_allowed = True;
    Boolean	grow_height_allowed = True;
    Dimension	width_increase = 0;
    Dimension	height_increase = 0;
    int		new_width,new_height;
    int		cell_width,cell_height;
    int		width_in_cells, height_in_cells;
    XtWidgetGeometry	geo_desired;

    /*
     * Must determine the constraints on growth from XmNresizeModel.
     * At Realize time, XmNspatialResizeModel is assumed to be XmGROW_BALANCED.
     * Note: XmNspatialResizeModel is only valid
     *			for XmNspatialStyle XmGRID || XmCELLS.
     */
    if (!CtrSpatialStyleIsNONE(cw))
	{
	if ((CtrResizeModelIsGROW_MINOR(cw)) && XtIsRealized(wid))
        {
	    if (CtrIsHORIZONTAL(cw))
            {
		grow_width_allowed = False;
            }
	    else
            {
		grow_height_allowed = False;
            }
        }
	if ((CtrResizeModelIsGROW_MAJOR(cw)) && XtIsRealized(wid))
        {
	    if (CtrIsHORIZONTAL(cw))
            {
		grow_height_allowed = False;
            }
	    else
            {
		grow_width_allowed = False;
            }

        }
	if ((!grow_width_allowed) && (!grow_height_allowed))
	    return(False);
    	}

    c = GetContainerConstraint(cwid);
    if (CtrSpatialStyleIsNONE(cw))
	{
	if (grow_width_allowed)
	    {
	    new_width =  cwid->core.x + cwid->core.width
					+ cw->container.margin_w;
	    if (new_width > cw->core.width)
		width_increase = new_width - cw->core.width;
	    }
	if (grow_height_allowed)
	    {
	    new_height = cwid->core.y + cwid->core.height
					+ cw->container.margin_h;
	    if (new_height > cw->core.height)
		height_increase = new_height - cw->core.height;
	    }
	}
    else
	{
	if (grow_width_allowed)
	    {
	    if (CtrViewIsSMALL_ICON(cw))
		cell_width = cw->container.real_small_cellw;
	    else
		cell_width = cw->container.real_large_cellw;
	    if (CtrSpatialStyleIsCELLS(cw))
		{
		width_in_cells = cwid->core.width / cell_width;
		if (cwid->core.width % cell_width)
		     width_in_cells++;
		width_increase = width_in_cells * cell_width;
		}
	    else
		width_increase = cell_width;
	    if (CtrIncludeIsCLOSEST(cw) &&
                (c->user_x > (cw->core.width - cw->container.margin_w)))
                {
                width_in_cells = (c->user_x
                                - (cw->core.width - cw->container.margin_w))
                                / cell_width;
                if (c->user_x - (cw->core.width - cw->container.margin_w)
                                % cell_width)
                     width_in_cells++;
                width_increase += (width_in_cells - 1) * cell_width;
                }
	    }
	if (grow_height_allowed)
	    {
	    if (CtrViewIsSMALL_ICON(cw))
		cell_height = cw->container.real_small_cellh;
	    else
		cell_height = cw->container.real_large_cellh;
	    if (CtrSpatialStyleIsCELLS(cw))
		{
		height_in_cells = cwid->core.height / cell_height;
		if (cwid->core.height % cell_height)
		    height_in_cells++;
		height_increase = height_in_cells * cell_height;
		}
	    else
		height_increase = cell_height;
	    if (CtrIncludeIsCLOSEST(cw) &&
                (c->user_y > (cw->core.height - cw->container.margin_h)))
                {
                height_in_cells = (c->user_y
                                - (cw->core.height - cw->container.margin_h))
                                / cell_height;
                if (c->user_y - (cw->core.height - cw->container.margin_h)
                                % cell_height)
                     height_in_cells++;
                height_increase += (height_in_cells - 1) * cell_height;
                }
	    }
	}

    geo_desired.request_mode = 0;
    if (width_increase != 0)
	{
	geo_desired.request_mode |= CWWidth;
	geo_desired.width = cw->core.width + width_increase;
	}
    if (height_increase != 0)
	{
	geo_desired.request_mode |= CWHeight;
	geo_desired.height = cw->core.height + height_increase;
	}
    if (geo_desired.request_mode == 0)
	return(False);
    /*
     * Make the geometry request (this will update the core width/height.)
     * We return True and it's the caller's responsibility to call
     * LayoutSpatial() so all the appropriate variables to keep up
     * with cell_count,etc. are updated.
     */
    return(_XmMakeGeometryRequest(wid,&geo_desired) == XtGeometryYes);
}

/************************************************************************
 * ConstraintInitialize (Constraint Method)
 ************************************************************************/

static void
ConstraintInitialize(
       Widget 		rcwid,		/* unused */
        Widget 		ncwid,
        ArgList 	args,		/* unused */
        Cardinal 	*num_args)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)XtParent(ncwid);
	XmContainerConstraint	nc = GetContainerConstraint(ncwid);
	XmContainerConstraint	pc;	/* parent's constraints */

	nc->related_cwid = NULL;
	nc->cwid_type = cw->container.create_cwid_type;
	if (!CtrICON(ncwid))
	    return;

	/*
	 * validate resource values
	 */
	if (!XmRepTypeValidValue(XmRID_OUTLINE_STATE,nc->outline_state,ncwid))
		nc->outline_state = XmCOLLAPSED;
	if (nc->position_index != XmLAST_POSITION)
		nc->position_index = MAX(0,nc->position_index);

	/* I can be my own grandpa but not my own descendant; pre-check */
	if (nc->entry_parent)
		if (ContainerIsDescendant(ncwid, nc->entry_parent))
			nc->entry_parent = NULL;

	/*
	 * Enforce margins.  Save initial x,y info.
	 */
	if (!LayoutIsRtoLM(cw))
	    ncwid->core.x = MAX(ncwid->core.x,(Position)cw->container.margin_w);
	ncwid->core.y = MAX(ncwid->core.y,(Position)cw->container.margin_h);
	nc->user_x = ncwid->core.x;
	nc->user_y = ncwid->core.y;

	/*
	 * Create new node for child and insert into list.
	 */
	InsertNode(NewNode(ncwid));

	/*
	 * Override XmNviewType
	 */
	if (!CtrViewIsANY_ICON(cw))
	    SetViewType(ncwid,cw->container.entry_viewtype);

        nc->selection_visual = GetVisualEmphasis(ncwid);
	if (nc->selection_visual == XmSELECTED)
		{
                cw->container.selected_item_count++;
		if (cw->container.anchor_cwid == NULL)
			cw->container.anchor_cwid = ncwid;
		}
	nc->selection_state = nc->selection_visual;
	if (nc->entry_parent)
		{
		pc = GetContainerConstraint(nc->entry_parent);
		nc->depth = pc->depth +1;
		cw->container.max_depth = MAX(cw->container.max_depth,
						nc->depth);
		if (pc->outline_state == XmEXPANDED)
			nc->visible_in_outline = pc->visible_in_outline;
		else
			nc->visible_in_outline = False;
		if ((!nc->visible_in_outline) ||
		    (!CtrLayoutIsOUTLINE_DETAIL(cw)))
			HideCwid(ncwid);
		}
	else
		{
		nc->visible_in_outline = True;
		nc->depth = 0;
		}

	/*
         * Initialize variables used by
	 * 	Container SpatialLayout methods.
         */
	nc->cell_idx = NO_CELL;
}

/************************************************************************
 * ConstraintDestroy (Constraint Method)
 ************************************************************************/
static	void
ConstraintDestroy(
	Widget	cwid)
{
	XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
        XmContainerConstraint   c = GetContainerConstraint(cwid);

        if (cwid == cw->container.anchor_cwid)
                cw->container.anchor_cwid = NULL;
	if (!CtrICON(cwid))
	    return;

		cw->container.icon_header = NULL;
	{
		CwidNode 	node = c->node_ptr->child_ptr;
		while (node)
		{
			Widget child = node->widget_ptr;
			XtVaSetValues(child, XmNentryParent, NULL, NULL);
			/* because the above operation has changed the
			** linked-list, we just pull the first child off the
			** list, rather than move along it
			*/
			node = c->node_ptr->child_ptr;
		}
	}

	DeleteNode(cwid);
	ContainerResequenceNodes(cw, c->entry_parent);
	if (c->selection_state == XmSELECTED)
		{
		unsigned char	save_state = cw->container.selection_state;

		cw->container.selection_state = XmNOT_SELECTED;
		MarkCwid(cwid,False);
		cw->container.selection_state = save_state;
		}
    if (XtIsRealized((Widget)cw))
	XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);
}

/************************************************************************
 * ConstraintSetValues (Constraint Method)
 ************************************************************************/

static Boolean
ConstraintSetValues(
	Widget		ccwid,
	Widget		rcwid,		/* unused */
	Widget		ncwid,
	ArgList		args,		/* unused */
	Cardinal	*num_args)	/* unused */
{
    XmContainerWidget       	cw = (XmContainerWidget)XtParent(ncwid);
    XmContainerConstraint	cc = GetContainerConstraint(ccwid);
    XmContainerConstraint   	nc = GetContainerConstraint(ncwid);
    XmContainerConstraint	pc;	/* Parent's Contraints */
    Boolean			need_layout = False;
    Boolean			need_expose = False;

    /*
     * SetValues called from inside Container - let's get out of here!
     */
    if (cw->container.self) return(False);

    if (!CtrICON(ncwid))
	return(False);

    /*
     * Validate resource values
     */
    if (nc->outline_state != cc->outline_state)
	{
    	if (!XmRepTypeValidValue(XmRID_OUTLINE_STATE,nc->outline_state,ncwid))
		nc->outline_state = cc->outline_state;
	}
    if (nc->position_index < 0)
	if (XmLAST_POSITION != nc->position_index)
		nc->position_index = cc->position_index;

    /* I can be my own grandpa but not my own descendant; pre-check */
    if (nc->entry_parent != cc->entry_parent)
	if (nc->entry_parent)
		if (ContainerIsDescendant(ncwid, nc->entry_parent))
			nc->entry_parent = cc->entry_parent;
    /*
     * Check if layout required.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw) &&
	((nc->entry_parent != cc->entry_parent) ||
	 (nc->position_index != cc->position_index) ||
         (nc->outline_state != cc->outline_state)))
	need_layout = True;

    /*
     * Check for change in selection status
     */
    nc->selection_visual = GetVisualEmphasis(ncwid);
    if (nc->selection_visual != cc->selection_visual)
	{
	if (nc->selection_visual == XmSELECTED)
	    {
	    cw->container.selected_item_count++;
	    if (cw->container.anchor_cwid == NULL)
		cw->container.anchor_cwid = ncwid;
	    }
	else
	    cw->container.selected_item_count--;
	nc->selection_state = nc->selection_visual;
	}

    /*
     * Changes in parentage or position.
     * If XmNpositionIndex has changed, but the "node" has no siblings,
     * 	then there's no need to change the node's position in the
     *	linked list.
     */
    if ((nc->entry_parent != cc->entry_parent) ||
        ((nc->position_index != cc->position_index) &&
         ((nc->node_ptr->prev_ptr) || (nc->node_ptr->next_ptr))))
	{
	SeverNode(nc->node_ptr);
	ContainerResequenceNodes(cw, cc->entry_parent);
	ContainerResequenceNodes(cw, nc->entry_parent);
	InsertNode(nc->node_ptr);
	}
    if (nc->entry_parent != cc->entry_parent)
    {
	if (nc->entry_parent == NULL)
	    {
	    nc->depth = 0;
	    nc->visible_in_outline = True;
	    ContainerResetDepths(nc);
	    }
	else
            {
            pc = GetContainerConstraint(nc->entry_parent);
            nc->depth = pc->depth +1;
	    ContainerResetDepths(nc);

		FindMaxDepths(cc, (Widget)cw);
            if (pc->outline_state == XmEXPANDED)
            	nc->visible_in_outline = pc->visible_in_outline;
	    else
	    	nc->visible_in_outline = False;
   		if ((!nc->visible_in_outline) ||
	   	 (!CtrLayoutIsOUTLINE_DETAIL(cw)))
	    	    HideCwid(ncwid);
	    }
    }

    if (nc->outline_state != cc->outline_state)
	OutlineButtonAction(ncwid, nc->outline_state, (XEvent*)NULL);

    /*
     * Change Spatial locations.
     */
    if (((ncwid->core.x != ccwid->core.x) ||
	 (ncwid->core.y != ccwid->core.y)) &&
	CtrLayoutIsSPATIAL(cw) &&
	(((XmContainerWidgetClass)
		XtClass((Widget)cw))->container_class.remove_item) &&
	(((XmContainerWidgetClass)
		XtClass((Widget)cw))->container_class.place_item))
	{
	unsigned char	save_include_model;
	unsigned char	save_snap_model;

	(*((XmContainerWidgetClass)
	    XtClass((Widget)cw))->container_class.remove_item)
			((Widget)cw,ncwid);
	save_include_model = cw->container.include_model;
	cw->container.include_model = XmCLOSEST;
	save_snap_model = cw->container.snap_model;
	cw->container.snap_model = XmNONE;
	(*((XmContainerWidgetClass)
	    XtClass((Widget)cw))->container_class.place_item)
			((Widget)cw,ncwid,FORCE);
	cw->container.include_model = save_include_model;
	cw->container.snap_model = save_snap_model;
	need_expose = True;
	}

    if (need_layout)
        {
	if (CtrLayoutIsOUTLINE_DETAIL(cw))
	    {
	    cw->container.ideal_width = 0;
	    cw->container.ideal_height = 0;
	    GetSize((Widget)cw,&cw->container.ideal_width,
				&cw->container.ideal_height);
	    cw->core.width = cw->container.ideal_width;
	    cw->core.height = cw->container.ideal_height;
	    }
        Layout((Widget)cw);
        need_expose = True;
        }

     if (need_expose && XtIsRealized((Widget)cw))
       XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);

     /* everything that needs to be done so far has been */
     return(False);
}

/************************************************************************
 * TestFitItem (Container Method)
 ************************************************************************/
static  Boolean
TestFitItem(
    Widget	wid,
    Widget	cwid,
    Position	x,
    Position	y)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    int	trial_cell;
    XPoint cell_coord;
    XmContainerConstraint cwidc = GetContainerConstraint(cwid);

    /*
     * Must be a 2-D layout.
     */
    if (CtrLayoutIsOUTLINE_DETAIL(cw))
	return(True);
    if (y < (Position)cw->container.margin_h)
	return(False);
    if (LayoutIsRtoLM(cw))
	{
	if (x > (Position)(cw->core.width - cw->container.margin_w))
	    return(False);
	}
    else
	if (x < (Position)cw->container.margin_w)
	    return(False);
    if (CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw))
	{
	  trial_cell = GetCellFromCoord(wid,x,y);
	  /* First,  we can always put a widget back where it
	     started,  but not if something else is there */
	  if (cw->container.cells[trial_cell] != 0) {
	    if (cwidc -> cell_idx == trial_cell)
	      return(True);
	    else
	      return(False);
	  }
	  (void)GetCoordFromCell(wid,trial_cell, &cell_coord);
	  if (CtrSpatialStyleIsCELLS(cw))
            if (XRectInRegion(cw->container.cells_region,
			      cell_coord.x,cell_coord.y,
			      cwid->core.width,cwid->core.height)
		!= RectangleOut)
	      return(False);
	}
    if (CtrSpatialStyleIsNONE(cw))
      {
	XtWidgetGeometry child_req;
	XtGeometryResult result;

	/* If we don't need to resize,  just return */
	if ((XtWidth(cw) >= (x + XtWidth(cwid) + cw -> container.margin_w)) &&
	    (XtHeight(cw) >= (y + XtHeight(cwid) + cw -> container.margin_h)))
	  return(True);

	/* Otherwise,  we'll try it via the geometry manager */
	child_req.request_mode = CWX | CWY;
	child_req.x = x;
	child_req.y = y;

	/* Make the request,  but always allow the drop */
	result = _XmMakeGeometryRequest(cwid, &child_req);
      }
    return(True);
}

/************************************************************************
 * PlaceItem (Container Method)
 ************************************************************************/
static	Boolean
PlaceItem(
    Widget	wid,
    Widget	cwid,
    unsigned char fit_type)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;

    /* cwid NULL means initialize */
    if (cwid == NULL) {
	PlaceItemReset(wid);
	return(True);
    }


    if (CtrItemIsPlaced(cwid))
	return(True);

    switch(cw->container.spatial_style)
    {
    case XmNONE:
	PlaceItemNone(wid,cwid,fit_type);
	break;
    case XmGRID:
    case XmCELLS:
	PlaceItemGridCells(wid,cwid,fit_type);
    }
    if ((!CtrItemIsPlaced(cwid)) && (!CtrSpatialStyleIsNONE(cw)))
	HideCwid(cwid);
    return(CtrItemIsPlaced(cwid));
}

/************************************************************************
 * RemoveItem (Container Method)
 ************************************************************************/
static  Boolean
RemoveItem(
    Widget          wid,
    Widget          cwid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    XmContainerConstraint c = GetContainerConstraint(cwid);
    XRectangle		cwid_rect;
    Region		cwid_region;

    if (!CtrItemIsPlaced(cwid))
	return(True);
    switch(cw->container.spatial_style)
    {
    case XmCELLS:
	cwid_rect.x = cwid->core.x;
	cwid_rect.y = cwid->core.y;
	cwid_rect.width = cwid->core.width;
	cwid_rect.height = cwid->core.height;
	cwid_region = XCreateRegion();
	XUnionRectWithRegion(&cwid_rect,cwid_region,cwid_region);
	XSubtractRegion(cw->container.cells_region,cwid_region,
			cw->container.cells_region);
	XDestroyRegion(cwid_region);
    case XmGRID:
	cw->container.cells[c->cell_idx]--;
    case XmNONE:
        c->cell_idx = NO_CELL;
    }
    return(True);
}

/************************************************************************
 * GetSpatialSize  (Container method)
 ************************************************************************/
static  void
GetSpatialSize (
        Widget                  wid,
        Dimension * pwidth,
        Dimension * pheight)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    CwidNode		node;
    XmContainerConstraint c;
    ContainerCwidCellInfo cwid_info;
    int			cwid_info_count;
    Dimension		trial_width = 0, trial_height = 0;
    Dimension		cell_width = 0,cell_height = 0;
    int			width_in_cells = 1,height_in_cells = 1;
    int			min_width_in_cells = 1,min_height_in_cells = 1;
    int			cwid_width_in_cells,cwid_height_in_cells;
    int			cell_count = 0;
    Dimension   width = 1, height = 1 ;

    if (CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw))
	{
	if (CtrViewIsSMALL_ICON(cw))
	    {
            cell_width = cw->container.real_small_cellw;
            cell_height = cw->container.real_small_cellh;
            }
	else
            {
            cell_width = cw->container.real_large_cellw;
            cell_height = cw->container.real_large_cellh;
            }
	}
    /*
     * Must allocate array to hold cwid information so we can use it to
     * calculate adjustments to ideal size when we're in XmCELLS.  Don't
     * really know how many we'll need, so allocate one for all children
     * of Container to be sure.
     */
    if (CtrSpatialStyleIsCELLS(cw))
	cwid_info = (ContainerCwidCellInfo)
		XtCalloc(cw->composite.num_children,
				sizeof(XmContainerCwidCellInfoRec));
    else
	cwid_info = NULL;
    cwid_info_count = 0;

    /*
     * Go through the linked list of cwids and lets determine
     * their geometry needs.
     */
    node = cw->container.first_node;
    while (node)
        {
	if (CtrSpatialStyleIsNONE(cw))
	    /*
	     * In this case, we care about the desired x,y coordinates of
	     * each child, so let's compute the smallest rectangle that
	     * would contain all of them and still fulfill their desires:
	     *     (trial_width X trial_height)
	     */
	    {
	    trial_width = MAX(trial_width,node->widget_ptr->core.x
					+ node->widget_ptr->core.width);
	    trial_height = MAX(trial_height,node->widget_ptr->core.y
					+ node->widget_ptr->core.height);
	    }
	else
	    if (CtrIncludeIsCLOSEST(cw))
	    	{
	    	c = GetContainerConstraint(node->widget_ptr);
	    	trial_width = MAX(trial_width,
				c->user_x + node->widget_ptr->core.width);
	    	trial_height = MAX(trial_height,
				c->user_y + node->widget_ptr->core.height);
	    	}
	if (CtrSpatialStyleIsGRID(cw))
	    /*
	     * CtrSpatialStyleIsGRID: One cell per cwid
	     */
	    cell_count++;
	if (CtrSpatialStyleIsCELLS(cw))
	    /*
	     * CtrSpatialStyleIsCELLS: Need enough cells to completely
	     *				contain all cwids.
	     */
	    {
	    cwid_width_in_cells = (node->widget_ptr)->core.width / cell_width;
	    if ((node->widget_ptr)->core.width % cell_width)
		cwid_width_in_cells++;
	    min_width_in_cells = MAX(min_width_in_cells,cwid_width_in_cells);
	    cwid_height_in_cells = (node->widget_ptr)->core.height
							/ cell_height;
	    if ((node->widget_ptr)->core.height % cell_height)
		cwid_height_in_cells++;
	    min_height_in_cells = MAX(min_height_in_cells,
					cwid_height_in_cells);
	    cell_count += cwid_width_in_cells * cwid_height_in_cells;
	    cwid_info[cwid_info_count].cwid_width_in_cells =
						cwid_width_in_cells;
	    cwid_info[cwid_info_count].cwid_height_in_cells =
						cwid_height_in_cells;
	    cwid_info_count++;
	    }
	node = GetNextNode(node);
	}
    if (CtrSpatialStyleIsNONE(cw) || CtrIncludeIsCLOSEST(cw))
	{
    	trial_width += 2 * cw->container.margin_w;
    	trial_height += 2 * cw->container.margin_h;
	}
    if (CtrSpatialStyleIsNONE(cw))
	{
	width = trial_width;
	height = trial_height;
	}
    else	/* CtrSpatialStyleIsGRID || CtrSpatialStyleIsCELLS */
	{
	/*
	 * Simple square will do.
	 */
	width_in_cells = Isqrt(cell_count);
	height_in_cells = width_in_cells;
	if (CtrSpatialStyleIsCELLS(cw))
	    /*
	     * Make adjustments to accomodate very wide or tall cwids.
	     * Then call GetSpatialSizeCellAdjustment() to do trial
	     *     placement and make furthur adjustments.
	     */
	    {
	    width_in_cells = MAX(width_in_cells,min_width_in_cells);
	    height_in_cells = MAX(height_in_cells,min_height_in_cells);
	    if (cwid_info != NULL)
		{
		GetSpatialSizeCellAdjustment(wid,&width_in_cells,
						&height_in_cells,
						cwid_info,cwid_info_count);
		XtFree((char*)cwid_info);
		}
	    }
	/*
	 * Add in the margins and calculate the pixels needed.
	 */
	width = width_in_cells * cell_width
				+ 2 * cw->container.margin_w;
	height = height_in_cells * cell_height
				+ 2 * cw->container.margin_h;
	if (CtrIncludeIsCLOSEST(cw))
	    /*
	     * Make adjustments to accomodate cwids' desired x,y placement.
	     */
	    {
	    width = MAX(width,trial_width);
	    height = MAX(height,trial_height);
	    }
	}
    if (!*pwidth) *pwidth = width ;
    if (!*pheight) *pheight = height ;
}


/************************************************************************
 * PlaceItemNone (Private Function)
 ************************************************************************/

static 	void
PlaceItemNone(
    Widget      wid,
    Widget      cwid,
    unsigned char fit_type)	/* unused */
{
    XmContainerWidget		cw = (XmContainerWidget)wid;
    XmContainerConstraint	c = GetContainerConstraint(cwid);

    PlaceCwid(cwid,cwid->core.x,cwid->core.y);
    /*
     * Mark the cwid as placed (any value except NO_CELL) if it's within
     * the Container boundaries.
     */
    if (((cwid->core.x + cwid->core.width) <=
				(cw->core.width - cw->container.margin_w)) &&
	((cwid->core.y + cwid->core.height) <=
				(cw->core.height - cw->container.margin_h)))
    	c->cell_idx = 1;
}

/************************************************************************
 * PlaceItemGridCells (Private Function)
 ************************************************************************/
static 	void
PlaceItemGridCells(
    Widget      wid,
    Widget      cwid,
    unsigned char fit_type)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  XmContainerConstraint c = GetContainerConstraint(cwid);
  int		start_cell;
  int	trial_cell = 0;
  int closest_cell = -1;
  Boolean	fits;
  XPoint 	cell_coord;
  XPoint 	place_point;
  XRectangle	cwidrect;

  if (CtrIncludeIsAPPEND(cw))
    trial_cell = cw->container.next_free_cell;
  if (CtrIncludeIsCLOSEST(cw))
    {
      closest_cell = GetCellFromCoord(wid,c->user_x,c->user_y);
      trial_cell = closest_cell;
      if (trial_cell >= cw->container.cell_count)
	{
	  if (fit_type == EXACT_FIT)
		return; /* exact fit or nothing! */
	  /* Adjust for current width/height boundaries */
	  trial_cell = GetCellFromCoord(wid,
				MIN(c->user_x,
		(Position)(cw->container.current_width_in_cells *
			(CtrViewIsSMALL_ICON(cw) ?
				cw->container.real_small_cellw :
				cw->container.real_large_cellw) - 1)),
				MIN(c->user_y,
		(Position)(cw->container.current_height_in_cells *
			(CtrViewIsSMALL_ICON(cw) ?
				cw->container.real_small_cellh :
				cw->container.real_large_cellh) - 1)));
	  if (trial_cell >= cw->container.cell_count)
		/* This shouldn't occur - but just in case */
		trial_cell = cw->container.next_free_cell;
	}
    }
  start_cell = trial_cell;
  if (trial_cell < cw->container.cell_count)
    {
      while (!CtrItemIsPlaced(cwid))
        {
          fits = False;
          if ((cw->container.cells[trial_cell] == 0) && (fit_type != FORCE))
	    {
	      if (CtrSpatialStyleIsGRID(cw))
	        fits = True;
	      if (CtrSpatialStyleIsCELLS(cw))
	        {
	          if (CtrIncludeIsCLOSEST(cw) && (trial_cell == closest_cell))
		      (void)SnapCwid(cwid,c->user_x,c->user_y, &place_point);
	          else
		    {
		      (void)GetCoordFromCell(wid,trial_cell, &cell_coord);
		      (void)SnapCwid(cwid,cell_coord.x,cell_coord.y,&place_point);
		    }
	          if ((XRectInRegion(cw->container.cells_region,
			  	     place_point.x,place_point.y,
				     cwid->core.width,cwid->core.height)
		       == RectangleOut) &&
		      (place_point.x + cwid->core.width <=
		       cw->core.width - cw->container.margin_w) &&
		      (place_point.y + cwid->core.height <=
		       cw->core.height - cw->container.margin_h))
		    fits = True;
	        }
	    }
          if (fits || (fit_type == FORCE))
	    {
	      cw->container.cells[trial_cell]++;
	      c->cell_idx = trial_cell;
	    }
          trial_cell++;
          if (trial_cell == cw->container.cell_count)
	    trial_cell = 0;
          if (start_cell == trial_cell)
	    break;
        }
      }
  if (!CtrItemIsPlaced(cwid))
    return;
  if (CtrIncludeIsAPPEND(cw))
    cw->container.next_free_cell = c->cell_idx + 1;
  if (CtrIncludeIsCLOSEST(cw) && (c->cell_idx == closest_cell))
      (void)SnapCwid(cwid,c->user_x,c->user_y, &place_point);
  else
    {
      (void)GetCoordFromCell(wid,c->cell_idx, &cell_coord);
      (void)SnapCwid(cwid,cell_coord.x,cell_coord.y, &place_point);
    }
  if (CtrSpatialStyleIsCELLS(cw))
    {
      cwidrect.x = place_point.x;
      cwidrect.y = place_point.y;
      cwidrect.width = cwid->core.width;
      cwidrect.height = cwid->core.height;
      XUnionRectWithRegion(&cwidrect,cw->container.cells_region,
			   cw->container.cells_region);
    }
  PlaceCwid(cwid,place_point.x,place_point.y);
}

/************************************************************************
 * GetCellFromCoord (Private Function)
 ************************************************************************/
static  int
GetCellFromCoord(
    Widget      wid,
    Position    x,
    Position    y)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    int	cell_width,cell_height;
    int	row,col;

    cell_width = (CtrViewIsSMALL_ICON(cw)) ?
        cw->container.real_small_cellw : cw->container.real_large_cellw;
    cell_height = (CtrViewIsSMALL_ICON(cw)) ?
        cw->container.real_small_cellh : cw->container.real_large_cellh;

    /*
     * Note: x & y parameters include Container margins.
     */
    if (!LayoutIsRtoLM(cw))
	x = MAX(0,x - cw->container.margin_w);
    y = MAX(0,y - cw->container.margin_h);
    row = y / cell_height;
    col = x / cell_width;
    if (LayoutIsRtoLM(cw))
	col = cw->container.current_width_in_cells - col - 1;
    if (CtrIsHORIZONTAL(cw))
	return(cw->container.current_width_in_cells * row + col);
    else
	return(cw->container.current_height_in_cells * col + row);
}

/************************************************************************
 * GetCoordFromCell (Private Function)
 ************************************************************************/
static 	XPoint *
GetCoordFromCell(
    Widget      wid,
    int		cell_idx,
    XPoint	*point)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    int	cell_width,cell_height;
    int	row,col;

    if (CtrIsHORIZONTAL(cw))
	{
	row = cell_idx / cw->container.current_width_in_cells;
	col = cell_idx - (row * cw->container.current_width_in_cells);
	}
    else
	{
	col = cell_idx / cw->container.current_height_in_cells;
	row = cell_idx - (col * cw->container.current_height_in_cells);
	}
    if (LayoutIsRtoLM(cw))
	col = cw->container.current_width_in_cells - col - 1;
    cell_width = (CtrViewIsSMALL_ICON(cw)) ?
        cw->container.real_small_cellw : cw->container.real_large_cellw;
    cell_height = (CtrViewIsSMALL_ICON(cw)) ?
        cw->container.real_small_cellh : cw->container.real_large_cellh;
    /*
     * Note: returned point must include Container Margins
     */
    point->x = col * cell_width;
    if (!LayoutIsRtoLM(cw))
	point->x += cw->container.margin_w;
    point->y = row * cell_height + cw->container.margin_h;
    return(point);
}

/************************************************************************
 * PlaceItemReset (Private Function)
 ************************************************************************/
static  void
PlaceItemReset(
    Widget          wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    CwidNode	node;
    int	cell_width = 0,cell_height = 0;
    int	width_in_cells = 0,height_in_cells = 0;

    /*
     * Deallocate old cell structure.
     */
    if ((cw->container.cells) &&
	(((XmContainerWidgetClass)XtClass(wid))->container_class.remove_item))
	{
	node = cw->container.first_node;
        while (node)
            {
	    if (CtrItemIsPlaced(node->widget_ptr))
		(*((XmContainerWidgetClass)
		XtClass(wid))->container_class.remove_item)
			(wid,node->widget_ptr);
	    node = GetNextNode(node);
	    }
	XtFree((char*)cw->container.cells);
	cw->container.cells = NULL;
    }

    if (CtrSpatialStyleIsNONE(cw))
	return;

    /*
     * Create new cell structure.
     */
    cell_width = (CtrViewIsSMALL_ICON(cw)) ?
	cw->container.real_small_cellw : cw->container.real_large_cellw;
    cell_height = (CtrViewIsSMALL_ICON(cw)) ?
	cw->container.real_small_cellh : cw->container.real_large_cellh;
    if (cw->core.width > cell_width + 2 * cw->container.margin_w)
	width_in_cells = (cw->core.width - 2 * cw->container.margin_w)
		/ cell_width;
    else
	width_in_cells = 1;
    if (cw->core.height > cell_height + 2 * cw->container.margin_h)
	height_in_cells = (cw->core.height - 2 * cw->container.margin_h)
		/ cell_height;
    else
	height_in_cells = 1;

    cw->container.cell_count = width_in_cells * height_in_cells;
    cw->container.cells = (int *)XtCalloc(cw->container.cell_count,sizeof(int));
    cw->container.next_free_cell = 0;
    cw->container.current_width_in_cells = width_in_cells;
    cw->container.current_height_in_cells = height_in_cells;

    if (CtrSpatialStyleIsGRID(cw))
	return;
    XSubtractRegion(cw->container.cells_region,cw->container.cells_region,
			cw->container.cells_region);
}

/************************************************************************
 * PlaceCwid (Private Function)
 ************************************************************************/
static  void
PlaceCwid(
    Widget  	cwid,
    Position	x,
    Position	y)
{
    XmContainerWidget		cw;

    if (cwid == NULL) return;
    cw = (XmContainerWidget)XtParent(cwid);
    /*
     * Adjust for margins
     */
    if (LayoutIsRtoLM(cw))
	{
	if ((cwid->core.width + (Position)cw->container.margin_w)
						>= cw->core.width)
	    cwid->core.x = MIN(cwid->core.x,cw->core.width
					- cwid->core.width
					- (Position)cw->container.margin_w);
	}
    else
	x = MAX(x,(Position)cw->container.margin_w);
    y = MAX(y,(Position)cw->container.margin_h);

    if ((x != cwid->core.x) || (y != cwid->core.y))
	XmeConfigureObject(cwid,x,y,cwid->core.width,cwid->core.height,0);
}

/************************************************************************
 * SnapCwid (Private Function)
 ************************************************************************/
static  XPoint *
SnapCwid(
    Widget      cwid,
    Position    x,
    Position    y,
    XPoint	*point)
{
    XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
    int			target_cell;
    XPoint 		target_cell_coord;
    int			cell_width = 0,cell_height = 0;
    int			width_in_cells = 0,height_in_cells = 0;

    target_cell = GetCellFromCoord((Widget)cw,x,y);
    (void)GetCoordFromCell((Widget)cw,target_cell, &target_cell_coord);
    point->x = target_cell_coord.x;
    point->y = target_cell_coord.y;

    if (CtrSnapModelIsSNAP(cw) && !LayoutIsRtoLM(cw))
	return(point);

    if (CtrViewIsSMALL_ICON(cw))
        {
        cell_width = cw->container.real_small_cellw;
        cell_height = cw->container.real_small_cellh;
        }
    else
	{
	cell_width = cw->container.real_large_cellw;
	cell_height = cw->container.real_large_cellh;
	}

    if (CtrSpatialStyleIsGRID(cw))
	width_in_cells = height_in_cells = 1;
    else
	/* CtrSpatialStyleIsCELLS(cw) */
	{
	width_in_cells = cwid->core.width / cell_width;
	if (cwid->core.width % cell_width)
	    width_in_cells++;
	height_in_cells = cwid->core.height / cell_height;
	if (cwid->core.height % cell_height)
	    height_in_cells++;
	}

    if (CtrSnapModelIsSNAP(cw))
	/* LayoutIsRtoLM(cw)  */
	{
	point->x += (width_in_cells * cell_width) - cwid->core.width;
	return(point);
	}

    if (CtrSnapModelIsNONE(cw))
	{
	point->x = MIN(x,
		target_cell_coord.x + (width_in_cells * cell_width) - 1);
	point->y = MIN(y,
		target_cell_coord.y + (height_in_cells * cell_height) - 1);
	return(point);
	}

    if (CtrSpatialStyleIsGRID(cw))
        {
	/* Adjust large items to 0 so we don't try to center them */
        width_in_cells = (cell_width < cwid->core.width) ? 0 : 1;
        height_in_cells = (cell_height < cwid->core.height) ? 0 : 1;
        }

    /* CtrSnapModelIsCENTER */
    /* If this is small icon view then the icon is placed centered
       up/down,  if this is large icon view then the icon is placed
       centered left/right */
    if (CtrViewIsSMALL_ICON(cw))
	{
      	if (height_in_cells != 0)
	    point->y += ((height_in_cells * cell_height)
					- cwid->core.height) / 2;
      	if (LayoutIsRtoLM(cw))
	    point->x += ((width_in_cells * cell_width)
					- cwid->core.width);
	}
    else
	{
	if (width_in_cells != 0)
	    point->x += ((width_in_cells * cell_width)
					- cwid->core.width) / 2;
	if (height_in_cells != 0)
	    point->y += (height_in_cells * cell_height)
					- cwid->core.height;
        }
    return(point);
}

/************************************************************************
 * HideCwid (Private Function)
 ************************************************************************/
static  void
HideCwid(
    Widget  cwid)
{
    XmContainerConstraint   c;
    CwidNode                node;
    CwidNode                child_node;

    if (cwid == NULL) return;
    c = GetContainerConstraint(cwid);

    XmeConfigureObject(
	cwid,(Position)(0 - ((Widget)cwid)->core.width),
	     (Position)(0 - ((Widget)cwid)->core.height),
	     ((Widget)cwid)->core.width,((Widget)cwid)->core.height,0);

    if (!CtrICON(cwid)) return;

    /*
     * Hide our button!
     */
    if (c->related_cwid)
        HideCwid(c->related_cwid);

    /*
     * If we're XmEXPANDED, then let's hide our children too.
     */
    if (c->outline_state != XmEXPANDED)
        return;
    node = c->node_ptr;
    child_node = node->child_ptr;
    while (child_node)
        {
        HideCwid(child_node->widget_ptr);
        c = GetContainerConstraint(child_node->widget_ptr);
        if (c->related_cwid)
            HideCwid(c->related_cwid);
        child_node = child_node->next_ptr;
        }
}

/************************************************************************
 * ContainerStartTransfer (Action Proc)
 ************************************************************************/
static	void
ContainerStartTransfer(
	Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget                  cwid;
	int			transfer_action;

	if (cw->container.selecting)
	  /* Don't allow drag while select in progress */
	  return;

	if (CtrLayoutIsOUTLINE_DETAIL(wid))
	    {
            cwid = (Widget)_XmInputForGadget(wid,
				event->xbutton.x,event->xbutton.y);
            if ((cwid) && CtrOUTLINE_BUTTON(cwid))
                {
                XtCallActionProc(wid,"ManagerGadgetDrag",
                    	            event,params,*num_params);
                return;
                }
            }
	/*
	 * Allocate transfer context record to temporarily hold
	 * event and operation argument until we can figure out whether
	 * this is a primary transfer or the start of a drag.
	 * If a context record already exists (might happen if there
	 * was a Btn2 double-click, for example), then just reuse it
	 * instead of malloc'ing a new one.
	 */
	if (cw->container.transfer_action == NULL)
		{
		cw->container.transfer_action = (ContainerXfrAction)
			XtCalloc(1,sizeof(XmContainerXfrActionRec));
		cw->container.transfer_action->event = (XEvent *)
			XtCalloc(1,sizeof(XEvent));
		}

	cw->container.transfer_action->wid = (Widget)cw;
	memcpy((void *)cw->container.transfer_action->event,(void *)event,
		sizeof(XEvent));
	cw->container.transfer_action->params = params;
	cw->container.transfer_action->num_params = num_params;

	if (num_params == 0 ||
	    _XmConvertActionParamToRepTypeId((Widget) cw,
			     XmRID_CONTAINER_START_TRANSFER_ACTION_PARAMS,
			     params[0], False, &transfer_action) == False)
	{
	    /* We couldn't convert the value. Just assume a value of _COPY. */
	    transfer_action = _COPY;
	}

	if (transfer_action == _LINK)
		cw->container.transfer_action->operation = XmLINK;
	else
		if (transfer_action == _MOVE)
			cw->container.transfer_action->operation = XmMOVE;
		else
			cw->container.transfer_action->operation = XmCOPY;

	/*
	 * Set off timer.
	 */
	if (cw->container.transfer_timer_id != 0)
		XtRemoveTimeOut(cw->container.transfer_timer_id);
	cw->container.transfer_timer_id = XtAppAddTimeOut(
			XtWidgetToApplicationContext((Widget)cw),
			XtGetMultiClickTime(XtDisplay((Widget)cw)),
			DragStart,(XtPointer)cw);
}

/************************************************************************
 * ContainerEndTransfer (Action Proc)
 ************************************************************************/

static  void
ContainerEndTransfer(
        Widget          wid,
        XEvent          *event,		/* unused */
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
  XmContainerWidget       cw = (XmContainerWidget)wid;

  /*
   * Kill the timeout.  If it already expired, then we didn't make
   * the interval for primary transfer, so return.
   */
  if (cw->container.transfer_timer_id != 0)
    XtRemoveTimeOut(cw->container.transfer_timer_id);
  else
    return;
  if (cw->container.transfer_action == NULL)
    return; 	/* No context record, shouldn't happen */
  if (cw->container.transfer_action->operation == XmLINK)
    ContainerPrimaryLink(wid,
			 cw->container.transfer_action->event,NULL,0);
  else
    if (cw->container.transfer_action->operation == XmMOVE)
      ContainerPrimaryMove(wid,
			   cw->container.transfer_action->event,NULL,0);
    else
      ContainerPrimaryCopy(wid,
			   cw->container.transfer_action->event,NULL,0);
  XtFree((char*)cw->container.transfer_action->event);
  XtFree((char*)cw->container.transfer_action);
  cw->container.transfer_action = NULL;
}

/************************************************************************
 * ContainerPrimaryCopy (Action Proc)
 ************************************************************************/

static  void
ContainerPrimaryCopy(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XPoint		*loc_data;

	/* WARNING: do not free the following memory in this module.  It
	 * will be freed in FreeLocationData, triggered at the end of
	 * the data transfer operation.
	 */
	loc_data = (XPoint *) XtMalloc(sizeof(XPoint));
	loc_data->x = event->xbutton.x;
	loc_data->y = event->xbutton.y;
        XmePrimarySink(wid,XmCOPY,
		       (XtPointer) loc_data,event->xbutton.time);
}

/************************************************************************
 * ContainerPrimaryLink (Action Proc)
 ************************************************************************/

static  void
ContainerPrimaryLink(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XPoint		*loc_data;

	/* WARNING: do not free the following memory in this module.  It
	 * will be freed in FreeLocationData, triggered at the end of
	 * the data transfer operation.
	 */
	loc_data = (XPoint *) XtMalloc(sizeof(XPoint));
	loc_data->x = event->xbutton.x;
	loc_data->y = event->xbutton.y;
        XmePrimarySink(wid,XmLINK,
		       (XtPointer) loc_data,event->xbutton.time);
}

/************************************************************************
 * ContainerPrimaryMove (Action Proc)
 ************************************************************************/

static  void
ContainerPrimaryMove(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XPoint		*loc_data;

	/* WARNING: do not free the following memory in this module.  It
	 * will be freed in FreeLocationData, triggered at the end of
	 * the data transfer operation.
	 */
	loc_data = (XPoint *) XtMalloc(sizeof(XPoint));
	loc_data->x = event->xbutton.x;
	loc_data->y = event->xbutton.y;
        XmePrimarySink(wid,XmMOVE,
		       (XtPointer) loc_data,event->xbutton.time);
}

/************************************************************************
 * Actions to dispatch based on button1 transfer options
 *
 * The first parameter is the default action to call if
 * enableBtn1Transfer is XmOFF
 *
 * If there is a second parameter,  it is COPY, MOVE or LINK.
 ************************************************************************/

static void
ContainerNoop(
        Widget          wid,	/* unused */
        XEvent          *event,	/* unused */
        String          *params, /* unused */
        Cardinal        *num_params) /* unused */
{
  /* Do nothing */
}

/*************************************************
 * ContainerHandleBtn1Down
 *
 * We do a drag if enableBtn1Transfer is not XmOFF
 * Otherwise we do a select.
 *
 * Also,  if nothing is selected we go ahead and do
 * a complete selection (Call ContainerBeginSelect,EndSelect)
 * so that we complete the selection before starting the
 * drag.
 *************************************************/
static void
ContainerHandleBtn1Down(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  XmContainerConstraint	cwidc;
  Widget		cwid = (Widget) NULL;
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if (*num_params < 2) {
    XmeWarning(wid, WRONGPARAMS);
    return;
  }

  cwid = ObjectAtPoint(wid, event->xbutton.x,event->xbutton.y);
  if (cwid != (Widget) NULL)
    cwidc = GetContainerConstraint(cwid);
  else
    cwidc = (XmContainerConstraint) NULL;

  if (dpy -> display.enable_btn1_transfer && cwid != (Widget) NULL
	&& !CtrOUTLINE_BUTTON(cwid)) {
    /* Select if over unselected item or background*/
    if (cwidc -> selection_state != XmSELECTED) {
      XtCallActionProc(wid, params[0], event, NULL, 0);
      ContainerEndSelect(wid,event,NULL,0);
    } else {
      SetupDrag(wid,event,params,num_params);
      cw->container.selecting = False;
    }
    if (cwidc -> selection_state == XmSELECTED)
      XtCallActionProc(wid, "ContainerStartTransfer", event, &params[1], 1);
    else
      XtCallActionProc(wid, params[0], event, NULL, 0);
  } else {
    XtCallActionProc(wid, params[0], event, NULL, 0);
  }
}

static void
ContainerHandleBtn1Motion(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if (*num_params < 1) {
    XmeWarning(wid, WRONGPARAMS);
    return;
  }

  /* If we're doing btn1 transfer,  then check to
     see if we've moved enough to start the drag */
  if (dpy->display.enable_btn1_transfer != XmOFF &&
      (! cw -> container.selecting))
    {
      int dx, dy;

      dx = event->xmotion.x - cw->container.anchor_point.x;
      dy = event->xmotion.y - cw->container.anchor_point.y;

      if (ABS(dx) >= MOTION_THRESHOLD ||
	  ABS(dy) >= MOTION_THRESHOLD) {
	/* Force the drag to start */
	DragStart((XtPointer) cw,
		  &cw -> container.transfer_timer_id);
      } else {
	return;
      }
    }
  else
    {
      /* Do the default action */
      XtCallActionProc(wid, params[0], event, NULL, 0);
    }
}

static void
ContainerHandleBtn1Up(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if (*num_params < 1) {
    XmeWarning(wid, WRONGPARAMS);
    return;
  }

  /* If we're doing btn1 transfer,  then remove the
     timer that starts the drag */
  if (dpy->display.enable_btn1_transfer) {
    if (cw->container.transfer_timer_id != 0)
      XtRemoveTimeOut(cw->container.transfer_timer_id);
    if (cw->container.drag_context != (Widget) NULL) {
      XmDragCancel(cw->container.drag_context);
      cw->container.drag_context = (Widget) NULL;
    }
  }

  XtCallActionProc(wid, params[0], event, NULL, 0);
}

static void
ContainerHandleBtn2Down(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if (*num_params < 2) {
    XmeWarning(wid, WRONGPARAMS);
    return;
  }

  if (dpy -> display.enable_btn1_transfer == XmBUTTON2_ADJUST)
    XtCallActionProc(wid, "ContainerBeginExtend", event, NULL, 0);
  else
    XtCallActionProc(wid, params[0], event, &params[1], 1);
}

static void
ContainerHandleBtn2Motion(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if (*num_params < 1) {
    XmeWarning(wid, WRONGPARAMS);
    return;
  }

  if (dpy->display.enable_btn1_transfer != XmBUTTON2_ADJUST &&
      (! cw -> container.selecting))
    {
      int dx, dy;

      dx = event->xmotion.x - cw->container.anchor_point.x;
      dy = event->xmotion.y - cw->container.anchor_point.y;

      if (ABS(dx) >= MOTION_THRESHOLD ||
	  ABS(dy) >= MOTION_THRESHOLD) {
	/* Force the drag to start */
	DragStart((XtPointer) cw,
		  &cw -> container.transfer_timer_id);
      } else {
	return;
      }
    }
  else
    XtCallActionProc(wid, params[0], event, NULL, 0);
}

static void
ContainerHandleBtn2Up(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmDisplay		dpy =
    (XmDisplay) XmGetXmDisplay(XtDisplay(wid));

  if ((dpy -> display.enable_btn1_transfer == XmBUTTON2_ADJUST) ||
      (!num_params) || (*num_params < 1))
    XtCallActionProc(wid, "ContainerEndExtend", event, NULL, 0);
  else
    XtCallActionProc(wid, params[0], event, NULL, 0);
}

/************************************************************************
 * ContainerBeginSelect (Action Proc)
 ************************************************************************/
static  void
ContainerBeginSelect(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  Widget		cwid = (Widget) NULL;

  cw->container.cancel_pressed = False;
  if (CtrLayoutIsOUTLINE_DETAIL(wid))
    {
    cwid = (Widget)_XmInputForGadget(wid,event->xbutton.x,event->xbutton.y);
    if ((cwid) && CtrOUTLINE_BUTTON(cwid))
	{
	XtCallActionProc(wid,"ManagerGadgetArm",event,params,*num_params);
	cw->container.ob_pressed = True;
	return;
	}
    }
  cw->container.extending_mode = (CtrPolicyIsMULTIPLE(cw));
  cw->container.selecting = True;
  StartSelect(wid,event,params,num_params);
}

/************************************************************************
 * ContainerButtonMotion (Action Proc)
 ************************************************************************/
static  void
ContainerButtonMotion(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Boolean			selection_changes;


	/* if we are outside the window update LeaveDir */
	if (cw->container.scroll_proc_id) {
	    Widget clip = XtParent(wid);
	    int rx, ry;		/* event coords relative to the clip */
	    rx = event->xmotion.x + (int)wid->core.x;
	    ry = event->xmotion.y + (int)wid->core.y;
	    if (rx <= (int)clip->core.x) {
		cw->container.LeaveDir |= LEFTLEAVE;
		cw->container.LeaveDir &= ~RIGHTLEAVE;
	    } else {
		cw->container.LeaveDir &= ~LEFTLEAVE;
		if (rx >= (int)clip->core.width)
		    cw->container.LeaveDir |= RIGHTLEAVE;
		else
		    cw->container.LeaveDir &= ~RIGHTLEAVE;
	    }
	    if (ry <= (int)clip->core.y) {
		cw->container.LeaveDir |= TOPLEAVE;
		cw->container.LeaveDir &= ~BOTTOMLEAVE;
	    } else {
		cw->container.LeaveDir &= ~TOPLEAVE;
		if (ry >= (int)clip->core.height)
		    cw->container.LeaveDir |= BOTTOMLEAVE;
		else
		    cw->container.LeaveDir &= ~BOTTOMLEAVE;
	    }
	    /* store relative coordinates of the event */
	    cw->container.last_xmotion_x = rx;
	    cw->container.last_xmotion_y = ry;
	}

	if (cw->container.cancel_pressed)
		return;
	if (cw->container.ob_pressed)
	    {
            XtCallActionProc(wid,"ManagerGadgetButtonMotion",
                    	            event,params,*num_params);
            return;
	    }
	if (CtrPolicyIsSINGLE(wid))
		return;
	if ((cw->container.extend_pressed) && (CtrLayoutIsSPATIAL(cw)))
		return;
	selection_changes = ProcessButtonMotion(wid,event,params,num_params);
	cw->container.no_auto_sel_changes |= selection_changes;
	if (CtrIsAUTO_SELECT(cw) && selection_changes)
		CallSelectCB(wid,event,XmAUTO_MOTION);
}

/************************************************************************
 * ContainerEndSelect (Action Proc)
 ************************************************************************/
static  void
ContainerEndSelect(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  Boolean			selection_changes;

  /* first remove TimeOutProc if any */
  cw->container.LeaveDir = 0;
  if (cw->container.scroll_proc_id) {
      XtRemoveTimeOut(cw->container.scroll_proc_id);
      cw->container.scroll_proc_id = 0;
  }

  cw->container.selecting = False;
  if (cw->container.cancel_pressed)
    return;
  if (cw->container.toggle_pressed)
    {
      ContainerEndToggle(wid,event,params,num_params);
      return;
    }
  if (cw->container.extend_pressed)
    {
      ContainerEndExtend(wid,event,params,num_params);
      return;
    }
  if (cw->container.ob_pressed)
    {
      XtCallActionProc(wid,"ManagerGadgetActivate",
		       event,params,*num_params);
      cw->container.ob_pressed = False;
      return;
    }
  if (CtrPolicyIsSINGLE(cw))
    {
      GainPrimary(wid,event->xbutton.time);
      CallSelectCB(wid,event,XmAUTO_UNSET);
      return;
    }

  selection_changes = ProcessButtonMotion(wid,event,params,num_params);
  cw->container.no_auto_sel_changes |= selection_changes;
  if (cw->container.marquee_drawn)
    {
      DrawMarquee(wid);
      cw->container.marquee_drawn = False;
      if (XtIsRealized(wid))
	XClearArea(XtDisplay(wid),XtWindow(wid),
		   cw->container.marquee_smallest.x,
		   cw->container.marquee_smallest.y,
		   cw->container.marquee_largest.x,
		   cw->container.marquee_largest.y,True);
    }
  if (cw->container.anchor_cwid)
    {
      if (!cw->container.kaddmode) {
	Boolean set_cursor = False;
	Widget current_focus = XmGetFocusWidget(wid);

	/* Only move the focus if the old focus child is no
	   longer in the set.  We can tell by looking
	   at the constraint record off the current focus
	   widget */
	if (current_focus != (Widget) NULL &&
	    XtParent(current_focus) == wid) {
	  XmContainerConstraint c =
	    GetContainerConstraint(current_focus);
	  if (c->selection_visual != XmSELECTED)
	    set_cursor = True;
	} else {
	  set_cursor = True;
	}
	if (set_cursor)
	  SetLocationCursor(cw->container.anchor_cwid);
      }
      if (CtrPolicyIsBROWSE(cw))
	cw->container.no_auto_sel_changes |=
	  MarkCwid(cw->container.anchor_cwid,False);
      else
	/* CtrPolicyIsMULTIPLE || CtrPolicyIsEXTENDED */
	SetMarkedCwids(wid);
    }
  GainPrimary(wid,event->xbutton.time);
  if (CtrIsAUTO_SELECT(cw))
    if (selection_changes)
      CallSelectCB(wid,event,XmAUTO_CHANGE);
    else
      CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
  else
    CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * ContainerBeginToggle (Action Proc)
 ************************************************************************/
static  void
ContainerBeginToggle(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget                  cwid;

	if (CtrLayoutIsOUTLINE_DETAIL(wid))
	    {
            cwid = (Widget)_XmInputForGadget(wid,
				event->xbutton.x,event->xbutton.y);
            if ((cwid) && CtrOUTLINE_BUTTON(cwid))
            	{
                XtCallActionProc(wid,"ManagerGadgetTraverseCurrent",
                    	            event,params,*num_params);
		cw->container.ob_pressed = True;
                return;
		}
            }
	cw->container.toggle_pressed = True;
	cw->container.cancel_pressed = False;
	if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
		return;
	cw->container.extending_mode = True;
	cw->container.selecting = True;
	StartSelect(wid,event,params,num_params);
}

/************************************************************************
 * ContainerEndToggle (Action Proc)
 ************************************************************************/
static  void
ContainerEndToggle(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;

	cw->container.toggle_pressed = False;
	cw->container.selecting = False;
	if (cw->container.cancel_pressed)
		return;
        if (cw->container.ob_pressed)
                {
                cw->container.ob_pressed = False;
                return;
                }
	if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
		return;
	ContainerEndSelect(wid,event,params,num_params);
}

/************************************************************************
 * ContainerBeginExtend (Action Proc)
 ************************************************************************/
static  void
ContainerBeginExtend(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			current_cwid;

	if (CtrLayoutIsOUTLINE_DETAIL(wid))
	    {
	    current_cwid = (Widget)_XmInputForGadget(wid,
				event->xbutton.x,event->xbutton.y);
	    if ((current_cwid) && CtrOUTLINE_BUTTON(current_cwid))
		{
		XtCallActionProc(wid,"ManagerGadgetArm",
				    event,params,*num_params);
		cw->container.ob_pressed = True;
		return;
		}
	    }
	cw->container.extend_pressed = True;
	cw->container.cancel_pressed = False;
	if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
	 	return;
	if (CtrLayoutIsSPATIAL(cw))
		return;
	current_cwid = ObjectAtPoint(wid,event->xbutton.x,event->xbutton.y);
	/* Handle ObjectAtPoint returning an outline button */
	if ((current_cwid) && CtrOUTLINE_BUTTON(current_cwid))
	  current_cwid = NULL;

	SetLocationCursor(current_cwid);
	if (current_cwid == NULL)
		return;
	if (cw->container.anchor_cwid == NULL)
		return;
	if (!cw->container.extending_mode)
                DeselectAllCwids(wid);
	(void) MarkCwidsInRange(wid,cw->container.anchor_cwid,
				current_cwid,(Boolean)True);
	if (CtrIsAUTO_SELECT(cw))
		CallSelectCB(wid,event,XmAUTO_BEGIN);
	cw->container.selecting = True;
}

/************************************************************************
 * ContainerEndExtend (Action Proc)
 ************************************************************************/
static  void
ContainerEndExtend(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Boolean			selection_changes;

	cw->container.extend_pressed = False;
	cw->container.selecting = False;
	if (cw->container.cancel_pressed)
		return;
	if (cw->container.ob_pressed)
                {
                XtCallActionProc(wid,"ManagerGadgetActivate",
                                event,params,*num_params);
                cw->container.ob_pressed = False;
                return;
                }
	if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
                return;
	if (CtrLayoutIsSPATIAL(cw))
                return;
	selection_changes = ProcessButtonMotion(wid,event,params,num_params);
	if (cw->container.marquee_drawn)
                {
                DrawMarquee(wid);
                cw->container.marquee_drawn = False;
                if (XtIsRealized(wid))
                        XClearArea(XtDisplay(wid),XtWindow(wid),
                                cw->container.marquee_smallest.x,
                                cw->container.marquee_smallest.y,
                                cw->container.marquee_largest.x,
                                cw->container.marquee_largest.y,True);
                }
	SetMarkedCwids(wid);
	GainPrimary(wid,event->xbutton.time);
	if (CtrIsAUTO_SELECT(cw))
		if (selection_changes)
                        CallSelectCB(wid,event,XmAUTO_CHANGE);
                else
                        CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
	else
		CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * ContainerCancel (Action Proc)
 ************************************************************************/
static  void
ContainerCancel(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Boolean			selection_changes = False;

	if (cw->container.ob_pressed)
		{
		XtCallActionProc(wid,"ManagerParentCancel",
					event,params,*num_params);
		cw->container.ob_pressed = False;
		return;
		}
	cw->container.toggle_pressed = False;
	cw->container.extend_pressed = False;
	if (CtrPolicyIsSINGLE(cw))
		return;
	cw->container.cancel_pressed = True;
	if (CtrPolicyIsBROWSE(cw))
		{
		if (cw->container.anchor_cwid)
			{
			selection_changes = UnmarkCwidVisual(
						cw->container.anchor_cwid);
			if (CtrIsAUTO_SELECT(cw) && selection_changes)
				{
				GainPrimary(wid,event->xbutton.time);
				CallSelectCB(wid,event,XmAUTO_CANCEL);
				}
			}
		return;
		}
	/* CtrPolicyIsMULTIPLE || CtrPolicyIsEXTENDED */
	selection_changes = ResetMarkedCwids(wid);
	if (cw->container.marquee_drawn)
                {
                DrawMarquee(wid);
                cw->container.marquee_drawn = False;
                if (XtIsRealized(wid))
                        XClearArea(XtDisplay(wid),XtWindow(wid),
                                cw->container.marquee_smallest.x,
                                cw->container.marquee_smallest.y,
                                cw->container.marquee_largest.x,
                                cw->container.marquee_largest.y,True);
                }
	if (CtrIsAUTO_SELECT(cw) && selection_changes)
		{
		GainPrimary(wid,event->xbutton.time);
		CallSelectCB(wid,event,XmAUTO_CANCEL);
		}
}

/************************************************************************
 * ContainerSelect (Action Proc)
 ************************************************************************/
static  void
ContainerSelect(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			focus_cwid = XmGetFocusWidget(wid);

	if (CtrLayoutIsOUTLINE_DETAIL(wid) && focus_cwid &&
		(focus_cwid != wid) && CtrOUTLINE_BUTTON(focus_cwid))
                    {
		    XtCallActionProc(wid,"ManagerGadgetSelect",
				event,params,*num_params);
                    return;
                    }
	cw->container.extending_mode = cw->container.kaddmode;
	KBSelect(wid,event,params,num_params);
}

/************************************************************************
 * ContainerExtend (Action Proc)
 ************************************************************************/

static  void
ContainerExtend(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			focus_cwid = XmGetFocusWidget(wid);

	if ((focus_cwid == wid) || (focus_cwid == NULL))
	    return;
	if (CtrOUTLINE_BUTTON(focus_cwid))
	    return;
        if (CtrLayoutIsSPATIAL(cw))
            return;
        if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
            return;
	if (!(cw->container.kaddmode))
	    cw->container.no_auto_sel_changes |= DeselectAllCwids(wid);
	cw->container.no_auto_sel_changes |=
			MarkCwidsInRange(wid,cw->container.anchor_cwid,
					focus_cwid,(Boolean)False);
	GainPrimary(wid,event->xbutton.time);
	if (CtrIsAUTO_SELECT(cw))
		{
		CallSelectCB(wid,event,XmAUTO_BEGIN);
		CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
		}
	else
		if (cw->container.no_auto_sel_changes)
			CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * ContainerMoveCursor (Action Proc)
 ************************************************************************/
static  void
ContainerMoveCursor(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			focus_cwid;

	if (*num_params == 0) return;
	if (_XmGetFocusPolicy(wid) == XmPOINTER)
	    return;
	CalcNextLocationCursor(wid,params[0]);
	focus_cwid = XmGetFocusWidget(wid);
	if (focus_cwid && (focus_cwid != wid) && CtrOUTLINE_BUTTON(focus_cwid))
	    return;
	if (cw->container.kaddmode)
	    return;
	cw->container.extending_mode = False;
	KBSelect(wid,event,params,num_params);
}

/************************************************************************
 * ContainerExtendCursor (Action Proc)
 ************************************************************************/
static  void
ContainerExtendCursor(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	if (*num_params == 0) return;
	if (_XmGetFocusPolicy(wid) == XmPOINTER)
	    return;
	if (CtrLayoutIsSPATIAL(wid))
	    return;
	if (CtrPolicyIsSINGLE(wid) || CtrPolicyIsBROWSE(wid))
	    return;
	CalcNextLocationCursor(wid,params[0]);
	ContainerExtend(wid,event,params,num_params);
}

/************************************************************************
 * ContainerToggleMode (Action Proc)
 ************************************************************************/

static  void
ContainerToggleMode(
        Widget          wid,
        XEvent          *event,		/* unused */
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    Widget		focus_cwid = XmGetFocusWidget(wid);

    if (CtrPolicyIsEXTENDED(cw))
      cw->container.kaddmode = !cw->container.kaddmode;
    if (XtIsRealized(wid) && focus_cwid && (focus_cwid != wid))
        XClearArea(XtDisplay(wid),XtWindow(wid),
                        focus_cwid->core.x,
                        focus_cwid->core.y,
                        focus_cwid->core.width,
                        focus_cwid->core.height,
                        True);
}

/************************************************************************
 * ContainerSelectAll (Action Proc)
 ************************************************************************/
static  void
ContainerSelectAll(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;

	if (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw))
		{
		ContainerSelect(wid,event,params,num_params);
		return;
		}
	cw->container.no_auto_sel_changes |= SelectAllCwids(wid);
	GainPrimary(wid,event->xbutton.time);
	if (CtrIsAUTO_SELECT(cw))
		{
		CallSelectCB(wid,event,XmAUTO_BEGIN);
		CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
		}
	else
		if (cw->container.no_auto_sel_changes)
                	CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * ContainerDeselectAll (Action Proc)
 ************************************************************************/

static  void
ContainerDeselectAll(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;

	cw->container.no_auto_sel_changes |= DeselectAllCwids(wid);
	GainPrimary(wid,event->xbutton.time);
	if (CtrIsAUTO_SELECT(cw) && (!CtrPolicyIsSINGLE(cw)))
		{
		CallSelectCB(wid,event,XmAUTO_BEGIN);
		CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
		}
	else
		if (cw->container.no_auto_sel_changes)
			CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * ContainerActivate (Action Proc)
 ************************************************************************/
static  void
ContainerActivate(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
	Widget			focus_cwid = XmGetFocusWidget(wid);

        if ((focus_cwid == wid) || (focus_cwid == NULL))
            return;
        if (CtrLayoutIsOUTLINE_DETAIL(wid) && CtrOUTLINE_BUTTON(focus_cwid))
                {
                XtCallActionProc(wid,"ManagerParentActivate",
                                event,params,*num_params);
                return;
                }
	CallActionCB(focus_cwid,event);
}


/************************************************************************
 * ContainerExpandOrCollapse (Action Proc)
 ************************************************************************/
static  void
ContainerExpandOrCollapse(
        Widget          wid,
        XEvent          *event,
        String          *params,
        Cardinal        *num_params)
{
    XmContainerWidget cw = (XmContainerWidget)wid;
    XmContainerConstraint c;
    Widget focus_cwid;
    int state_to;
    unsigned char new_state;

    if (!num_params || *num_params != 1 || !params)
	return;

    if ( ((focus_cwid=XmGetFocusWidget(wid)) == NULL)
      || (wid != XtParent(focus_cwid))
      || (CtrLayoutIsSPATIAL(cw))
      || (CtrOUTLINE_BUTTON(focus_cwid)))
        return;

    if (_XmConvertActionParamToRepTypeId((Widget) cw,
		     XmRID_CONTAINER_EXPAND_OR_COLLAPSE_ACTION_PARAMS,
		     params[0], False, &state_to) == False)
	return;

    c = GetContainerConstraint(focus_cwid);

    /* check LtoR and reduce to _COLLAPSE and _EXPAND values */
    if ((state_to == _COLLAPSE)
     || (state_to == _LEFT && (! LayoutIsRtoLM(cw)))
     || (state_to == _RIGHT && (LayoutIsRtoLM(cw))))
	    new_state = XmCOLLAPSED;
    else
	    new_state = XmEXPANDED;

    /* check if there is anything to do */
    if (new_state == c->outline_state)
	return;

    OutlineButtonAction(focus_cwid, new_state, NULL);
}


/************************************************************************
 * ContainerConvertProc (Trait Method)
 ************************************************************************/
static  void
ContainerConvertProc(
	Widget		wid,
	XtPointer	closure,
	XmConvertCallbackStruct *cs)
{
  enum {
    XmA_MOTIF_LOSE_SELECTION,		XmA_MOTIF_EXPORT_TARGETS,
    XmA_MOTIF_CLIPBOARD_TARGETS,	XmA_COMPOUND_TEXT,
    XmA_MOTIF_COMPOUND_STRING,		XmA_MOTIF_DRAG_OFFSET,
    XmA_MOTIF_DROP,			XmA_TARGETS,
    XmAUTF8_STRING,			NUM_ATOMS };
  static char *atom_names[] = {
    XmS_MOTIF_LOSE_SELECTION,		XmS_MOTIF_EXPORT_TARGETS,
    XmS_MOTIF_CLIPBOARD_TARGETS,	XmSCOMPOUND_TEXT,
    XmS_MOTIF_COMPOUND_STRING,		XmS_MOTIF_DRAG_OFFSET,
    XmS_MOTIF_DROP,			XmSTARGETS,
    XmSUTF8_STRING,
  };

  XmContainerWidget	cw = (XmContainerWidget)wid;
  WidgetList		items = NULL;
  int			item_count = 0;
  XtPointer		value = NULL;
  unsigned int		length = 0;
  int			format = 0;
  Atom			type = 0;
  Atom			atoms[XtNumber(atom_names)];

  /* Get Atom values from cache. */
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(wid), atom_names, XtNumber(atom_names), False, atoms);

  if (cs->target == atoms[XmA_MOTIF_LOSE_SELECTION])
    {
      cw->container.have_primary = False;
      cs->value = NULL;
      cs->length = 0;
      cs->type = 0;
      cs->status = XmCONVERT_DONE;
      return;
    }
  if ((cs->target == atoms[XmA_TARGETS]) ||
      (cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS]) ||
      (cs->target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS]))
    {
      Atom	*targargs;
      int	n = 0;

      if (cs -> target == atoms[XmA_TARGETS])
	targargs = XmeStandardTargets(wid,6,&n);
      else
	targargs  = (Atom*) XtMalloc(sizeof(Atom) * 6);
      targargs[n++] = XA_PIXMAP;
      targargs[n++] = atoms[XmAUTF8_STRING];
      targargs[n++] = atoms[XmA_COMPOUND_TEXT];
      targargs[n++] = atoms[XmA_MOTIF_COMPOUND_STRING];
      if (cw->container.drag_context != (Widget) NULL)
	targargs[n++] = atoms[XmA_MOTIF_DRAG_OFFSET];
      value = (XtPointer)targargs;
      format = 32;
      length = n;
      type = XA_ATOM;
    } else if (cs->target == atoms[XmA_MOTIF_DRAG_OFFSET])
      {
	short	*offset_args = (short *)XtCalloc(2,sizeof(short));
	int	n = 0;

	value = (XtPointer)offset_args;
	offset_args[n++] = cw->container.drag_offset_x;
	offset_args[n++] = cw->container.drag_offset_y;
	format = 16;
	length = n;
	type = XA_INTEGER;
      } else if ((cs->target == XA_PIXMAP) ||
		 (cs->target == atoms[XmA_MOTIF_COMPOUND_STRING]) ||
		 (cs->target == atoms[XmAUTF8_STRING]) ||
		 (cs->target == atoms[XmA_COMPOUND_TEXT]))
	{
	  if ((cs->selection == atoms[XmA_MOTIF_DROP]) && (cs->location_data))
            {
	      items = (WidgetList)XtMalloc(sizeof(Widget));
	      items[0] = (Widget)cs->location_data;
	      item_count = 1;
            }
	  else
            {
	      if ((item_count = cw->container.selected_item_count) == 0)
		ConvertRefuse(wid,closure,cs);
	      items = GetSelectedCwids(wid);
            }
	}
  if (cs->target == XA_PIXMAP)
    {
      Arg             wargs[10];
      int             n,i;
      Pixmap		item_pm;
      Pixmap *        return_pm = (Pixmap *)
	XtCalloc(item_count,sizeof(Pixmap));
      int	      return_pm_count = 0;

      for (i = 0; i < item_count; i++)
	{
	  n = 0;
	  if (GetViewType(items[0]) == XmSMALL_ICON)
	    XtSetArg(wargs[n],XmNsmallIconPixmap,&item_pm);
	  else
	    XtSetArg(wargs[n],XmNlargeIconPixmap,&item_pm);
	  n++;
	  item_pm = XmUNSPECIFIED_PIXMAP;
	  XtGetValues(items[i],wargs,n);
	  if (item_pm != XmUNSPECIFIED_PIXMAP)
	    return_pm[return_pm_count++] = item_pm;
	}
      format = 32;
      type = XA_PIXMAP;
      value = (XtPointer)return_pm;
      length = return_pm_count;
    } else if ((cs->target == atoms[XmA_MOTIF_COMPOUND_STRING]) ||
	       (cs->target == atoms[XmA_COMPOUND_TEXT]))
      {
	Arg		wargs[10];
	int		n,i;
	XmString	item_xmstr;
	XmString	return_xmstr = XmStringCreateLocalized("");

	n = 0;
	XtSetArg(wargs[n],XmNlabelString,&item_xmstr); n++;
	for (i = 0; i < item_count; i++)
	  {
	    /* CR 7669: Concatenate efficiently and free strings. */
	    item_xmstr = NULL;
	    XtGetValues(items[i],wargs,n);
	    if (i > 0)
	      return_xmstr = XmStringConcatAndFree(return_xmstr,
						   XmStringSeparatorCreate());
	    return_xmstr = XmStringConcatAndFree(return_xmstr, item_xmstr);
	  }
	format = 8;
	if (cs->target == atoms[XmA_MOTIF_COMPOUND_STRING])
	  {
	    type = atoms[XmA_MOTIF_COMPOUND_STRING];
	    length = XmCvtXmStringToByteStream(return_xmstr,
					       (unsigned char **)&(value));
	  }
	else if (cs->target == atoms[XmA_COMPOUND_TEXT])
	  {
	    type = atoms[XmA_COMPOUND_TEXT];
	    value = XmCvtXmStringToCT(return_xmstr);
	    if (value != NULL)
	    	length = strlen((char*) value);
	    else
	    	length = 0;
	  }
#if XM_UTF8
	else if (cs->target == atoms[XmAUTF8_STRING])
	  {
	    type = atoms[XmAUTF8_STRING];
	    value = XmCvtXmStringToUTF8String(return_xmstr);
	    if (value != NULL)
	    	length = strlen((char*) value);
	    else
	    	length = 0;
	  }
#endif
	XmStringFree(return_xmstr);
      }
  if (items)
	XtFree((char*) items);
  _XmConvertComplete(wid, value, length, format, type, cs);
}

/************************************************************************
 * ContainerDestinationProc (Trait Method)
 ************************************************************************/

static  void
ContainerDestinationProc(
        Widget          wid,
        XtPointer       closure,	/* unused */
        XmDestinationCallbackStruct *cs)
{
	enum { XmA_MOTIF_DROP, XmA_MOTIF_DRAG_OFFSET, NUM_ATOMS };
	static char *atom_names[] = { XmS_MOTIF_DROP, XmS_MOTIF_DRAG_OFFSET };

	XmContainerWidget		cw = (XmContainerWidget)wid;
	XmDropProcCallbackStruct *	dropproc_cs;
	Atom atoms[XtNumber(atom_names)];

	/*
	** In case of a primary transfer operation where a location_data
	** has been allocated, a done proc to be called when
	** the data transfer is complete to free the location_data
	*/
	if (cs->selection == XA_PRIMARY && cs->location_data)
	    XmeTransferAddDoneProc(cs->transfer_id, FreeLocationData);

	/* If we aren't sensitive,  don't allow transfer */
	if (! wid -> core.sensitive ||
	    ! wid -> core.ancestor_sensitive)
	  XmTransferDone(cs -> transfer_id, XmTRANSFER_DONE_FAIL);

	/* Get Atom values from cache. */
	assert(XtNumber(atom_names) == NUM_ATOMS);
	XInternAtoms(XtDisplay(wid), atom_names, XtNumber(atom_names),
		     False, atoms);

	if (cs->selection != atoms[XmA_MOTIF_DROP])
		return;
	if (cw->container.drag_context == (Widget) NULL)
		return;
	if (!CtrLayoutIsSPATIAL(cw))
		{
		cw->container.drag_context = (Widget) NULL;
		return;
		}
	dropproc_cs = (XmDropProcCallbackStruct *)cs->destination_data;
	cw->container.dropspot.x = dropproc_cs->x;
	cw->container.dropspot.y = dropproc_cs->y;
	XmTransferValue(cs->transfer_id,
			atoms[XmA_MOTIF_DRAG_OFFSET],
			(XtCallbackProc)MoveItemCallback,
			(XtPointer)&cw->container.dropspot,cs->time);
}

/************************************************************************
 * ContainerDestPrehookProc (Trait Method)
 ************************************************************************/

static  void
ContainerDestPrehookProc(
        Widget          wid,
	XtPointer	closure, /* unused */
        XmDestinationCallbackStruct *cs)
{
  XmContainerWidget		cw = (XmContainerWidget)wid;
  XmDropProcCallbackStruct *	dropproc_cs;
  XPoint			*loc_data;

  /* For a PRIMARY selection, don't null out the location_data.
   * It can point to allocated memory, that will be free when
   * transfer is done.
   */
  if (cs->selection != XA_PRIMARY)
      cs->location_data = NULL;

  if (cs->selection == XInternAtom(XtDisplay(wid),XmS_MOTIF_DROP,False))
    {
      loc_data = (XPoint *) XtMalloc(sizeof(XPoint));
      dropproc_cs = (XmDropProcCallbackStruct *) cs->destination_data;
      loc_data -> x = dropproc_cs->x;
      loc_data -> y = dropproc_cs->y;
      cs->location_data = (XtPointer) loc_data;
      XmeTransferAddDoneProc(cs->transfer_id, FreeLocationData);
    }
}

/************************************************
 * Free data allocated for destination callback
 ************************************************/

static 	void
FreeLocationData(Widget wid,	/* unused */
		 XtEnum ignore_op, /* unused */
		 XmTransferDoneCallbackStruct* cs)
{
  XmDestinationCallbackStruct *ds;

  ds = _XmTransferGetDestinationCBStruct(cs -> transfer_id);

  XtFree((char*) ds -> location_data);

  ds -> location_data = NULL;
}

/************************************************************************
 * ContGetValues (Trait Method)
 ************************************************************************/
static  void
ContGetValues(
    Widget		wid,
    XmContainerData	contData)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;

    if (CtrLayoutIsOUTLINE_DETAIL(cw))
	{
	if (CtrLayoutIsDETAIL(cw))
	    {
	    if (cw->container.detail_order_count)
	 	{
		contData->detail_order_count = cw->container.detail_order_count;
		contData->detail_order = cw->container.detail_order;
		/* detail_order might be NULL here */
	    	}
	    else
		/* take the max detail count and return NULL for the
		   detail_order, so that the icon takes a default order */
		if (contData->valueMask & ContDetailOrder)
		    {
		    contData->detail_order_count =
			MAX (GetDefaultDetailCount(wid),
			     contData->detail_order_count);
		    contData->detail_order = NULL ;
		    }
	    contData->detail_tablist = cw->container.detail_tablist;
 	    }
	else
	    {
	    contData->detail_order = NULL;
	    contData->detail_order_count = 0;
	    contData->detail_tablist = NULL;
	    }

	/* we want to return a valid firstcolumnwidth even in pure outline,
	   so that clipping can happen */
	if (cw->container.real_first_col_width == 0)
	    /* Use what was passed in so we always get something */
	    contData->first_column_width = contData->first_column_width;
	else
	    contData->first_column_width = cw->container.real_first_col_width;
	contData->first_column_width += cw->container.margin_w;
    	}
    else
	{
	contData->detail_order = NULL;
	contData->detail_order_count = 0;
	contData->detail_tablist = NULL;
	contData->first_column_width = 0;  /* really the flag that
					      says we're in spatial */
    	}
    if ((cw->container.kaddmode) && (_XmGetFocusPolicy(wid) == XmEXPLICIT))
        contData->selection_mode = XmADD_MODE;
    else
        contData->selection_mode = XmNORMAL_MODE;
    contData->select_color = cw->container.select_color;
}

/************************************************************************
 * GetVisualEmphasis (Private Function)
 ************************************************************************/
static  unsigned char
GetVisualEmphasis(
    Widget              cwid)
{
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;

    if ((cItemTrait = (XmContainerItemTrait)
        XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) == NULL)
        return(XmNOT_SELECTED);
    cItemData.valueMask = ContItemVisualEmphasis;
    cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
    return(cItemData.visual_emphasis);
}

/************************************************************************
 * SetVisualEmphasis (Private Function)
 ************************************************************************/
static  void
SetVisualEmphasis(
    Widget  		cwid,
    unsigned char	emphasis)
{
    XmContainerWidget		cw = (XmContainerWidget)XtParent(cwid);
    XmContainerItemDataRec	cItemData;
    XmContainerItemTrait	cItemTrait;

    if ((cItemTrait = (XmContainerItemTrait)
	XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) == NULL)
	return;
    cItemData.valueMask = ContItemVisualEmphasis;
    cItemData.visual_emphasis = emphasis;
    cw->container.self = True;
    cItemTrait->setValues(cwid,(XmContainerItemData)&cItemData);
    cw->container.self = False;
}

/************************************************************************
 * GetViewType (Private Function)
 ************************************************************************/
static  unsigned char
GetViewType(
    Widget              cwid)
{
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;

    if ((cItemTrait = (XmContainerItemTrait)
        XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) == NULL)
        return(XmLARGE_ICON);
    cItemData.valueMask = ContItemViewType;
    cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
    return(cItemData.view_type);
}

/************************************************************************
 * SetViewType (Private Function)
 ************************************************************************/
static  void
SetViewType(
    Widget              cwid,
    unsigned char       viewtype)
{
    XmContainerWidget           cw = (XmContainerWidget)XtParent(cwid);
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;

    if ((cItemTrait = (XmContainerItemTrait)
        XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) == NULL)
        return;
    cItemData.valueMask = ContItemViewType;
    cItemData.view_type = viewtype;
    cw->container.self = True;
    cItemTrait->setValues(cwid,(XmContainerItemData)&cItemData);
    cw->container.self = False;
}

/************************************************************************
 * GetIconWidth (Private Function)
 ************************************************************************/
static  Dimension
GetIconWidth(
    Widget              cwid)
{
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;

    if ((cItemTrait = (XmContainerItemTrait)
        XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) == NULL)
        return(cwid->core.width);
    cItemData.valueMask = ContItemIconWidth;
    cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
    return(cItemData.icon_width);
}

/************************************************************************
* GetDefaultDetailCount (Private Function)
 ************************************************************************/
static  Cardinal
GetDefaultDetailCount(
    Widget      wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    XmContainerItemDataRec	cItemData;
    XmContainerItemTrait	cItemTrait;
    Widget	cwid;
    CwidNode	node;
    Cardinal	detail_count = 0;

    /*
     * Get the IconHeader's detail_count first
     */
    if ((cwid = GetRealIconHeader(wid)) && (XtIsManaged(cwid)) &&
	((XtParent(cwid) == wid) || (XtIsManaged(XtParent(cwid))))) {

	cItemTrait = (XmContainerItemTrait)
		XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem);
	cItemData.valueMask = ContItemDetailCount;
	cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
	detail_count = MAX(detail_count,cItemData.detail_count);
    }
    /*
     * Get MAX detail count of all Icons now
     */
    node = GetFirstNode(cw);
    while (node)
	{
	cwid = node->widget_ptr;
	if ((cItemTrait = (XmContainerItemTrait)
            XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) != NULL)
	    {
	    cItemData.valueMask = ContItemDetailCount;
	    cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
	    detail_count = MAX(detail_count,cItemData.detail_count);
	    }
	node = GetNextNode(node);
	}
    return(detail_count);
}


/************************************************************************
* SetDynamicTabList (Private Function)
 ************************************************************************/
static  XmTabList
SetDynamicTabList(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    int tab_size;
    Cardinal		detail_order_count;

    if (!CtrIsDynamic(cw,TABLIST)) return(NULL);	/* Just in case */

    /*
     * Always free the previous XmTabList
     */
    if (CtrIsDynamic(cw,TABLIST) && (cw->container.detail_tablist))
	{
	XmTabListFree(cw->container.detail_tablist);
	cw->container.detail_tablist = NULL;
	}
    /*
     * If the Container has a valid size, a default tablist of
     * size detail_order_count shall be returned in detail_tablist.
     * The tabs in this tablist shall be equal to:
     *
     *          (XmNwidth-2*XmNmarginWidth-XmNfirstColumnWidth)
     *                  /XmNdetailOrderCount
     *
     * If XmNwidth is not yet valid, a NULL detail_tablist
     * is returned, so that the icon can concat all its details
     * and get a reasonable size.
     */
    if (cw->core.width == 0) return(NULL);

    tab_size = cw->core.width - (cw->container.margin_w * 2)
                                - cw->container.real_first_col_width;
    detail_order_count = (cw->container.detail_order_count) ?
	cw->container.detail_order_count : GetDefaultDetailCount(wid);
    if (tab_size <= 0)
        return(NULL);
    if (detail_order_count == 0)
	return(NULL);
    tab_size /= detail_order_count;
    if (tab_size <=30) return NULL ; /* HACKKKK */

    cw->container.detail_tablist = GetDumbTabList(tab_size,detail_order_count);
    return(cw->container.detail_tablist);
}

/************************************************************************
* GetDumbTabList (Private Function)
 ************************************************************************/
static  XmTabList
GetDumbTabList(
    int         tab_size,
    Cardinal    asked_num_tab)
{
    static int Num_tab = 0 ;
    static XmTab * Tab_pool = NULL ;
    XmTabList Tab_list = NULL ;
    Cardinal i, prev_num_tab = Num_tab ;

    _XmProcessLock();
    if (Num_tab < asked_num_tab)
        {
        Num_tab = MAX(asked_num_tab,100);	/* HACKKKK */
        Tab_pool = (XmTab*) XtRealloc((char*)Tab_pool,Num_tab*sizeof(XmTab));
        }

    /* create more tabs */
    for (i=prev_num_tab; i<Num_tab; i++)
        Tab_pool[i] =  XmTabCreate(0.0, XmPIXELS, XmABSOLUTE,
                                   XmALIGNMENT_BEGINNING, XmS);

    /* update the values */
    for (i=0; i<asked_num_tab; i++)
        XmTabSetValue(Tab_pool[i], (float)tab_size*(i+1));

    /*
     * Return a new tablist.
     */
    Tab_list = XmTabListInsertTabs(NULL, Tab_pool, asked_num_tab, 0);
    _XmProcessUnlock();

    return Tab_list ;
}

/************************************************************************
* GetDynFirstColWidth (Private Function)
 ************************************************************************/
static 	Dimension
GetDynFirstColWidth(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;
    XmContainerConstraint	c;
    Widget      cwid;
    CwidNode    node;
    Dimension	cwid_fcw = 0;
    Dimension	fcw = 0;

    /*
     * Get the IconHeader's icon_width
     */
    if ((cwid = GetRealIconHeader(wid)) && (XtIsManaged(cwid)) &&
	((XtParent(cwid) == wid) || (XtIsManaged(XtParent(cwid))))) {

	cItemTrait = (XmContainerItemTrait)
                XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem);
        cItemData.valueMask = ContItemIconWidth;
        cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
	fcw = MAX(fcw,cItemData.icon_width);
        }
    /*
     * Get MAX icon_width of all Icons
     */
    node = GetFirstNode(cw);
    while (node)
        {
        cwid = node->widget_ptr;
        if ((cItemTrait = (XmContainerItemTrait)
            XmeTraitGet((XtPointer)XtClass(cwid),XmQTcontainerItem)) != NULL)
            {
	    cItemData.valueMask = ContItemIconWidth;
            cItemTrait->getValues(cwid,(XmContainerItemData)&cItemData);
	    cwid_fcw = cItemData.icon_width;
            }
	else
	    /*
	     * Just use Gadget width, if it doesn't have the trait.
	     */
	    cwid_fcw = cwid->core.width;

	c = GetContainerConstraint(cwid);
	cwid_fcw += cw->container.ob_width +
	    c->depth * cw->container.outline_indent;

	fcw = MAX(fcw,cwid_fcw);
        node = GetNextNode(node);
    }

    return(fcw);
}

/************************************************************************
 * ResizeIconWidths (Private Function)
 * This function gets the preferred width of all icons based on the
 * current width of Container (this affects dynamic tablists) and
 * resizes them all to the max.
 ************************************************************************/
static  void
ResizeIconWidths(
    Widget  wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    CwidNode		node;
    Widget		cwid;
    XmContainerConstraint c;
    XtWidgetGeometry	desired;
    Position max_x = 0 ;

    /*** get the maximum right edge first */
    if (cw->core.width == cw->container.ideal_width)
	max_x = cw->container.ideal_width - cw->container.margin_w;
    else
	{
	if (CtrIsDynamic(cw,TABLIST))
	    max_x = MAX(cw->container.margin_w
				+ cw->container.real_first_col_width,
			cw->core.width - cw->container.margin_w);
	else
	    {
	    /* header first. */
	    if ((cwid = GetRealIconHeader(wid)) && (XtIsManaged(cwid)) &&
		((XtParent(cwid) == wid) || (XtIsManaged(XtParent(cwid)))))
		{
		XtQueryGeometry(cwid, NULL, &desired);
		max_x = MAX(max_x, cwid->core.x + desired.width);
		}
	    /* then the icons */
	    node = GetFirstNode(cw);
	    while(node)
		{
		cwid = node->widget_ptr;
		c = GetContainerConstraint(cwid);
		/* This query depends on the current first column width,
		   and the current x position.
		 */
		cwid->core.x = cw->container.margin_w +
		    c->depth*cw->container.outline_indent
		    + ((CtrOB_PRESENT(cw))?cw->container.ob_width:0) ;
		if (LayoutIsRtoLM(cw))
		    cwid->core.x = (Position)cw->core.width
                        	- (Position)cwid->core.width - cwid->core.x;
		XtQueryGeometry(cwid, NULL, &desired);
		max_x = MAX(max_x, cwid->core.x + desired.width);
		node = GetNextNode(node);
		}
	    }
	}

    /*** Then resize */
    if ((cwid = GetRealIconHeader(wid)) && (XtIsManaged(cwid)) &&
	((XtParent(cwid) == wid) || (XtIsManaged(XtParent(cwid)))))
	{
	XtQueryGeometry(cwid, NULL, &desired);
    	XmeConfigureObject(cwid, cwid->core.x, cwid->core.y,
			   max_x - cwid->core.x,
			   cwid->core.height, cwid->core.border_width);
	/* also recongifure the DA parent if any */
	if (XtParent(cwid) != wid) {
	    XmeConfigureObject(cw->container.icon_header, /* the DA */
			       cw->container.icon_header->core.x,
			       cw->container.icon_header->core.y,
			       cwid->core.width + 2*cw->container.margin_w,
			       cwid->core.height + cw->container.margin_h,
			       cw->core.border_width);
	}
    }

    node = GetFirstNode(cw);
    while(node) {
	cwid = node->widget_ptr;
	c = GetContainerConstraint(cwid);
        cwid->core.x = cw->container.margin_w +
                c->depth*cw->container.outline_indent
                + ((CtrOB_PRESENT(cw))?cw->container.ob_width:0) ;
	cwid->core.width = max_x - cwid->core.x;
	if (LayoutIsRtoLM(cw))
	    cwid->core.x = (Position)cw->core.width
			- (Position)cwid->core.width - cwid->core.x;
	XmeConfigureObject(cwid, cwid->core.x, cwid->core.y,
			   cwid->core.width,
			   cwid->core.height, cwid->core.border_width);

	node = GetNextNode(node);
    }
}

/************************************************************************
 * Layout (Private Function)
 ************************************************************************/
static  void
Layout(
        Widget  wid)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;

  if (CtrLayoutIsOUTLINE_DETAIL(cw))
    LayoutOutlineDetail(wid);
  else
    LayoutSpatial(wid,False,NULL);
}

/************************************************************************
 * RequestOutlineDetail (Private Function)
 ************************************************************************/
static  void
RequestOutlineDetail(
        Widget  wid,
        XtWidgetGeometry * geo_desired)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    Dimension saved_width = 0 ;

    /* deal with special requirements for width */
    if (geo_desired->width) {
      saved_width = cw->core.width;
      cw->core.width = geo_desired->width;
    }

    /* let's find out about our preferred size */
    cw->container.ideal_width = 0;
    cw->container.ideal_height = 0;
    GetSize(wid,&cw->container.ideal_width,&cw->container.ideal_height);
    geo_desired->request_mode = (CWWidth | CWHeight);

    /* see if a constraint was placed on the preferred size */
    if (!(geo_desired->width))
	geo_desired->width = cw->container.ideal_width;
    else
	cw->core.width = saved_width ;

    if (!(geo_desired->height))
	geo_desired->height = cw->container.ideal_height;

    if (geo_desired->width < 1) geo_desired->width = cw->core.width ;
    if (geo_desired->height < 1) geo_desired->height = cw->core.height ;
    _XmMakeGeometryRequest(wid,geo_desired);
    if (XtIsRealized((Widget)cw))
	XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);
    LayoutOutlineDetail(wid);
    cw->container.prev_width = cw->core.width;
}

/************************************************************************
 * LayoutOutlineDetail (Private Function)
 ************************************************************************/
static	void
LayoutOutlineDetail(
	Widget	wid)
{
    XmContainerWidget		cw = (XmContainerWidget)wid;
    XmContainerConstraint	c;
    XmContainerConstraint	pc;

    Position		x,y,ob_y;
    CwidNode		node;
    Widget		cwid;
    int                 n_outline_segs,seg_idx;
    XPoint *            point_at_depth = NULL;
    int                 i;

    if (CtrLayoutIsDETAIL(cw))
	{
	if (CtrIsDynamic(cw,TABLIST))
	    SetDynamicTabList(wid);
	ResizeIconWidths(wid);
	}

	/*
	** assert(c->depth == pc->depth + 1);
	** for children with entry-parents, and otherwise
	** assert(c->depth == 0);
	*/

    /*
     * Create array of XSegments to hold line descriptions for Outline.
     */
    n_outline_segs = seg_idx = 0;
    node = GetFirstNode(cw);
    while (node)
	/*
	 * So, how many XSegments do we need?
	 */
	{
	c = GetContainerConstraint(node->widget_ptr);
	if (c->depth != 0)
	n_outline_segs += 2;
	node = GetNextNode(node);
	}
    if (cw->container.outline_seg_count != n_outline_segs)
	{
	if (cw->container.outline_segs)
	    XtFree((char*)cw->container.outline_segs);
	cw->container.outline_segs = (XSegment *)XtCalloc
				(n_outline_segs,sizeof(XSegment));
	cw->container.outline_seg_count = n_outline_segs;
	}
    point_at_depth = (XPoint *)XtCalloc((cw->container.max_depth + 1),
					sizeof(XPoint));

    y = cw->container.margin_h;
    if (CtrLayoutIsDETAIL(cw) && cw->container.icon_header) {
	/*
	 * Position the header/Drawing-area.
	 * assert: cw->container.icon_header is valid
	 */
	Widget real_icon_header = GetRealIconHeader(wid),
	       header_parent = XtParent(real_icon_header) ;

	/* only if it is managed */
	if (XtIsManaged(real_icon_header) &&
	    ((header_parent == wid) ||
	     (XtIsManaged(header_parent)))) {

	    if (header_parent != wid) {
		/* auto SW case : DA is the parent of the header */
		/* set y of the container so that it glue the
		   bottom of the real icon header *inside* the DA.
		   So no margin_h added here, container must start
		   underneath the DA/icon header because it has a margin
		   height that need to be hidden (replaced by the DA one) */
		if (0 == cw->core.y)
			cw->core.y = real_icon_header->core.height ;

		/* resize the DA to remove the bottom margin */
		XmeConfigureObject(header_parent,
			       header_parent->core.x,
			       header_parent->core.y,
			       real_icon_header->core.width +
			       2*cw->container.margin_w,
			       real_icon_header->core.height +
			       cw->container.margin_h,
			       cw->core.border_width);
	    } else
		{
		XmeConfigureObject(cw->container.icon_header,
				cw->container.margin_w,
				cw->container.margin_h,
				cw->container.icon_header->core.width,
				cw->container.icon_header->core.height,0);
		y += cw->container.icon_header->core.height;
		}
	}
    }

    node = GetFirstNode(cw);
    while (node)
	{
	cw->container.last_node = node;
	cwid = node->widget_ptr;
	c = GetContainerConstraint(cwid);

	if (LayoutIsRtoLM(cw))
	    x = cw->core.width - cw->container.margin_w
			- c->depth * cw->container.outline_indent;
	else
	    x = cw->container.margin_w
			+ c->depth * cw->container.outline_indent;
	ob_y = y + (cwid->core.height - cw->container.ob_height) / 2;
	if (CtrOB_PRESENT(cw) && LayoutIsRtoLM(cw))
            x -= cw->container.ob_width;
	if ((node->child_ptr) && CtrOB_PRESENT(cw))
	    {
	    if (!c->related_cwid)
	    	MakeOutlineButton(cwid);
	    /*
	     * we need to hide the outline button if they appear
	     * after the first column limit
	     */
	    if ((!LayoutIsRtoLM(cw) && ((x + cw->container.ob_width) >
					(cw->container.margin_w
					 + cw->container.real_first_col_width)))
				 ||
		(LayoutIsRtoLM(cw) && (x < (Position)(cw->core.width
					- cw->container.margin_w
					- cw->container.real_first_col_width))))
		HideCwid(c->related_cwid) ;
	    else
	     	XmeConfigureObject(c->related_cwid,x,ob_y,
					cw->container.ob_width,
					cw->container.ob_height,0);
	    }
	else
	    if (c->related_cwid)
	        {
	        XtDestroyWidget(c->related_cwid);
	        c->related_cwid = NULL;
		}
	if (CtrOB_PRESENT(cw) && !LayoutIsRtoLM(cw))
	    x += cw->container.ob_width;

        /*
	 * we don't want the x position to be bigger than
	 * the firstcolumnwidth, otherwize the arithmetic in
	 * Icong get confused. Hackk
	 */
	if (LayoutIsRtoLM(cw))
	    x = MAX(x,(Position)(cw->core.width - cw->container.margin_w
			- cw->container.real_first_col_width + 1));
	else
	    x = MIN(x,(Position)(cw->container.margin_w
			+ cw->container.real_first_col_width - 1));

	/*
	 * Calculate Outline lines
	 */
        if (point_at_depth[c->depth].x == 0)
	    {
            point_at_depth[c->depth].x = cw->container.margin_w
                                + cw->container.outline_indent * c->depth;
	    }
	if (CtrOB_PRESENT(cw))
            point_at_depth[c->depth].y = ob_y + cw->container.ob_height;
        else
            point_at_depth[c->depth].y = y + cwid->core.height;

        if (c->entry_parent)
            {
            pc = GetContainerConstraint(c->entry_parent);
            if (CtrOB_PRESENT(cw))
            {
              /* Optimally, the line would be drawn from the center of the Outline Button (OB)
              ** causing problems when indentation is small. So we drop the line
              ** MIN (midpoint of OB, midpoint of OB left & child left
              */
              cw->container.outline_segs[seg_idx].x1 =
                MIN((point_at_depth[pc->depth].x + cw->container.ob_width/2),
                    (point_at_depth[pc->depth].x +point_at_depth[c->depth].x)/2 );
            }
            else
            {
              /* Optimally, the line would be drawn from the center of the parent
              ** causing problems when parent labelString is too long. So we drop the
              ** line MIN (midpoint of parent, midpoint of parent left & child left edge)
              */
              cw->container.outline_segs[seg_idx].x1 =
                 MIN((point_at_depth[pc->depth].x + GetIconWidth(c->entry_parent)/2),
                     (point_at_depth[pc->depth].x +point_at_depth[c->depth].x)/2 );
            }
            cw->container.outline_segs[seg_idx].y1 =
                                        point_at_depth[pc->depth].y;
	    cw->container.outline_segs[seg_idx].x2 =
					cw->container.outline_segs[seg_idx].x1;
            cw->container.outline_segs[seg_idx].y2 =
                                        y + cwid->core.height/2;
            cw->container.outline_segs[seg_idx + 1].x1 =
                                        cw->container.outline_segs[seg_idx].x2;
            cw->container.outline_segs[seg_idx + 1].y1 =
                                        cw->container.outline_segs[seg_idx].y2;
	    cw->container.outline_segs[seg_idx + 1].x2 = cw->container.margin_w
				+ cw->container.outline_indent * c->depth;
	    if (CtrOB_PRESENT(cw) && (!c->related_cwid))
		cw->container.outline_segs[seg_idx + 1].x2 +=
				cw->container.ob_width;
            cw->container.outline_segs[seg_idx + 1].y2 =
                                cw->container.outline_segs[seg_idx].y2;
            seg_idx += 2;
            }

        if (LayoutIsRtoLM(cw))
	    x = x - (Position)cwid->core.width;
	XmeConfigureObject(cwid,x,y,cwid->core.width,cwid->core.height,0);

	y += cwid->core.height;
	node = GetNextNode(node);
	}

    /*
     * Free the Malloc'd point_at_depth.
     * If Left-to-Right, reverse all the x1's & x2's and adjust.
     */
    if (point_at_depth)
	XtFree((char*)point_at_depth);
    for (i = 0; i < cw->container.outline_seg_count; i++)
	{
	int	max_x = (int)cw->container.real_first_col_width
			+ (int)cw->container.margin_w;

	/* we don't want the lines to be drawn after the first
	   column width, so we clip here the x positions of the
	   segments */
	cw->container.outline_segs[i].x1 = MIN(max_x,
				cw->container.outline_segs[i].x1);
	cw->container.outline_segs[i].x2 = MIN(max_x,
				cw->container.outline_segs[i].x2);
        }

    if (LayoutIsRtoLM(cw)) {
	int	adjust;

	adjust = (int)cw->core.width - (int)cw->container.ideal_width;
	for (i = 0; i < cw->container.outline_seg_count; i++) {
	    cw->container.outline_segs[i].x1 = MAX(0,
				cw->container.ideal_width
				- cw->container.outline_segs[i].x1 + adjust);
	    cw->container.outline_segs[i].x2 = MAX(0,
				cw->container.ideal_width
				- cw->container.outline_segs[i].x2 + adjust);
	}
    }
}

/************************************************************************
 * LayoutSpatial (Private Function)
 ************************************************************************/
static  void
LayoutSpatial(
     Widget  wid,
     Boolean growth_req_allowed,
     CwidNode stop_node)
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  int		cell_width,cell_height;
  int		width_in_cells,height_in_cells;
  CwidNode	node;

    if (!((XmContainerWidgetClass)XtClass(wid))->container_class.place_item)
	return;
    if (CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw))
      	/*
      	 * Let's see if things have changed enough so that we have to reset.
      	 */
      	{
      	cell_width = (CtrViewIsSMALL_ICON(cw)) ?
	    cw->container.real_small_cellw : cw->container.real_large_cellw;
      	cell_height = (CtrViewIsSMALL_ICON(cw)) ?
	    cw->container.real_small_cellh : cw->container.real_large_cellh;
      	width_in_cells = (cw->core.width - 2 * cw->container.margin_w)
					/ cell_width;
        height_in_cells = (cw->core.height - 2 * cw->container.margin_h)
					/ cell_height;
      	if (((width_in_cells != cw->container.current_width_in_cells) &&
	     (CtrIsHORIZONTAL(wid)))  ||
	    ((height_in_cells != cw->container.current_height_in_cells) &&
	     (CtrIsVERTICAL(wid))))
	  (*((XmContainerWidgetClass)
	     XtClass(wid))->container_class.place_item)(wid,NULL,ANY_FIT);
      	else
	    /*
	     * Just grow the number of cells if only the Minor dimension
	     * has changed.
	     */
	    {
	    cw->container.current_width_in_cells = width_in_cells;
	    cw->container.current_height_in_cells = height_in_cells;
	    if ((width_in_cells * height_in_cells) > cw->container.cell_count)
	    	{
	      	int	i,old_cell_count = cw->container.cell_count;

	      	cw->container.cell_count = width_in_cells * height_in_cells;
	      	if (CtrIsHORIZONTAL(cw)) /* For "clipped" items */
		    cw->container.cell_count += height_in_cells;
	      	else
		    cw->container.cell_count += width_in_cells;
	      	cw->container.cells =
			(int *)XtRealloc((char *)cw->container.cells,
				 (sizeof(int) * (cw->container.cell_count)));
	      	for (i = old_cell_count; i < cw->container.cell_count; i++)
		    cw->container.cells[i] = 0;
	        }
	    }
        }

    node = GetFirstNode(cw);
    while (node)
    	{
      	Widget cwid = node -> widget_ptr;

      	cw->container.last_node = node;
      	if (CtrItemIsPlaced(cwid))
	    PlaceCwid(cwid,cwid->core.x,cwid->core.y);
      	else
	    {
	    /* Not placed */
	    (*((XmContainerWidgetClass)
	       XtClass(wid))->container_class.place_item)
	  			(wid,cwid,
				 (growth_req_allowed ? EXACT_FIT : ANY_FIT));
	    if (!CtrItemIsPlaced(cwid) && (growth_req_allowed))
	  	/*
	  	 * See if we can grow and then try again.
	  	 */
		{
	  	if (RequestSpatialGrowth(wid,cwid))
		    /*
		     * If RequestSpatialGrowth succeeds it will update
		     * core width/height.  Now, call LayoutSpatial again
		     * so it can update info to keep track of cells, etc.
		     * and can place the item we just got growth for.
		     * Don't set the "growth_req_allowed" parameter or
		     * we could recurse forever.
		     */
		    LayoutSpatial(wid,False,node);
		else	/* try again, but don't be so picky */
		    {
		    (*((XmContainerWidgetClass)
			XtClass(wid))->container_class.place_item)
					(wid,cwid,ANY_FIT);
		    if (!CtrItemIsPlaced(cwid))
			HideCwid(cwid);
		    }
		}
	    }
        if ((stop_node) && (node == stop_node))
	    /*
	     * This is just a little hack to keep from having to traverse
	     * through all the nodes when we recursively call LayoutSpatial().
	     */
	    return;
	node = GetNextNode(node);
    	}
}

/************************************************************************
 * SetCellSizes (Private Function)
 ************************************************************************/
static  void
SetCellSizes(
    Widget	wid)
{
    if (!CtrSpatialStyleIsGRID(wid) && !CtrSpatialStyleIsCELLS(wid))
	return;
    if (CtrViewIsSMALL_ICON(wid))
	SetSmallCellSizes(wid);
    else
	SetLargeCellSizes(wid);
}

/************************************************************************
 * SetSmallCellSizes (Private Function)
 ************************************************************************/
static  void
SetSmallCellSizes(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    CwidNode		node;

    if (!CtrDynamicSmallCellHeight(cw))
	cw->container.real_small_cellh = cw->container.small_cell_height;
    if (!CtrDynamicSmallCellWidth(cw))
	cw->container.real_small_cellw = cw->container.small_cell_width;
    if (!CtrDynamicSmallCellHeight(cw) && !CtrDynamicSmallCellWidth(cw))
	return;

    if ((cw->container.first_node) && (cw->container.small_cell_dim_fixed))
	return;
    cw->container.small_cell_dim_fixed = True;

    if (cw->container.first_node)
    {
        node = cw->container.first_node;
	if (CtrDynamicSmallCellHeight(cw))
	    cw->container.real_small_cellh =
			cw->container.first_node->widget_ptr->core.height;
	if (CtrDynamicSmallCellWidth(cw))
            cw->container.real_small_cellw =
			cw->container.first_node->widget_ptr->core.width;
        while (node)
            {
            if (CtrSpatialStyleIsGRID(cw))
                /*
                 * GRID: cell should be size of largest cwid.
                 */
                {
		if (CtrDynamicSmallCellHeight(cw))
		    cw->container.real_small_cellh =
			MAX(	cw->container.real_small_cellh,
				node->widget_ptr->core.height);
		if (CtrDynamicSmallCellWidth(cw))
		    cw->container.real_small_cellw =
			MAX(	cw->container.real_small_cellw,
				node->widget_ptr->core.width);
                }
            else
                {
                /*
                 * CELLS: cell should be size of smallest cwid.
                 */
		if (CtrDynamicSmallCellHeight(cw))
		    cw->container.real_small_cellh =
			MIN(	cw->container.real_small_cellh,
				node->widget_ptr->core.height);
		if (CtrDynamicSmallCellWidth(cw))
		    cw->container.real_small_cellw =
			MIN(	cw->container.real_small_cellw,
				node->widget_ptr->core.width);
                }
            node = GetNextNode(node);
            }
        }
    else
	{
	if (CtrDynamicSmallCellHeight(cw))
	    cw->container.real_small_cellh =
		MAX(10,(int) (.02 * HeightOfScreen(XtScreen((Widget)cw))));
	if (CtrDynamicSmallCellWidth(cw))
	    cw->container.real_small_cellw =
		MAX(10,(int) (.02 * WidthOfScreen(XtScreen((Widget)cw))));
        }
}

/************************************************************************
 * SetLargeCellSizes (Private Function)
 ************************************************************************/
static  void
SetLargeCellSizes(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    CwidNode            node;

    if (!CtrDynamicLargeCellHeight(cw))
        cw->container.real_large_cellh = cw->container.large_cell_height;
    if (!CtrDynamicLargeCellWidth(cw))
        cw->container.real_large_cellw = cw->container.large_cell_width;
    if (!CtrDynamicLargeCellHeight(cw) && !CtrDynamicLargeCellWidth(cw))
        return;

    if ((cw->container.first_node) && (cw->container.large_cell_dim_fixed))
        return;

    if (cw->container.first_node)
    {
	cw->container.large_cell_dim_fixed = True;
        node = cw->container.first_node;
	if (CtrDynamicLargeCellHeight(cw))
            cw->container.real_large_cellh =
                        cw->container.first_node->widget_ptr->core.height;
	if (CtrDynamicLargeCellWidth(cw))
           cw->container.real_large_cellw =
                        cw->container.first_node->widget_ptr->core.width;
        while (node)
            {
            if (CtrSpatialStyleIsGRID(cw))
                /*
                 * GRID: cell should be size of largest cwid.
                 */
                {
		if (CtrDynamicLargeCellHeight(cw))
                    cw->container.real_large_cellh =
                        MAX(    cw->container.real_large_cellh,
                                node->widget_ptr->core.height);
		if (CtrDynamicLargeCellWidth(cw))
                    cw->container.real_large_cellw =
                        MAX(    cw->container.real_large_cellw,
                                node->widget_ptr->core.width);
                }
            else
                {
                /*
                 * CELLS: cell should be size of smallest cwid.
                 */
		if (CtrDynamicLargeCellHeight(cw))
                    cw->container.real_large_cellh =
                        MIN(    cw->container.real_large_cellh,
                                node->widget_ptr->core.height);
		if (CtrDynamicLargeCellWidth(cw))
                    cw->container.real_large_cellw =
                        MIN(    cw->container.real_large_cellw,
                                node->widget_ptr->core.width);
                }
            node = GetNextNode(node);
            }
        }
    else
        {
	if (CtrDynamicLargeCellHeight(cw))
            cw->container.real_large_cellh =
                MAX(20,(int) (.04 * HeightOfScreen(XtScreen((Widget)cw))));
	if (CtrDynamicLargeCellWidth(cw))
            cw->container.real_large_cellw =
                MAX(20,(int) (.04 * WidthOfScreen(XtScreen((Widget)cw))));
        }
}

/************************************************************************
 * SizeOutlineButton (Private Function)
 ************************************************************************/
static	void
SizeOutlineButton(
    Widget	wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    Arg			wargs[10];
    int			n;
    Dimension		width,height;

    /*
     * Create dummy outline button so we can get size.
     */
    n = 0;
    XtSetArg(wargs[n],XmNlabelType,XmPIXMAP); n++;
    XtSetArg(wargs[n],XmNlabelPixmap,cw->container.expanded_state_pixmap); n++;
    XtSetArg(wargs[n],XmNmarginWidth,0); n++;
    XtSetArg(wargs[n],XmNmarginHeight,0); n++;
    cw->container.self = True;
    if (cw->container.size_ob)
	XtSetValues(cw->container.size_ob,wargs,n);
    else
	{
	cw->container.create_cwid_type = CONTAINER_OUTLINE_BUTTON;
        cw->container.size_ob = XtCreateWidget(OBNAME,
                xmPushButtonGadgetClass,(Widget)cw,wargs,n);
	cw->container.create_cwid_type = CONTAINER_ICON;
	}
    XtVaGetValues(cw->container.size_ob,XmNwidth,&cw->container.ob_width,
	XmNheight,&cw->container.ob_height,NULL);
    n = 0;
    XtSetArg(wargs[n],XmNlabelType,XmPIXMAP); n++;
    XtSetArg(wargs[n],XmNlabelPixmap,
	     cw->container.collapsed_state_pixmap); n++;
    XtSetValues(cw->container.size_ob,wargs,n);
    cw->container.self = False;
    XtVaGetValues(cw->container.size_ob,
		  XmNwidth,&width,XmNheight,&height,NULL);
    cw->container.ob_width = MAX(cw->container.ob_width,width);
    cw->container.ob_height = MAX(cw->container.ob_height,height);
}

/************************************************************************
 * UpdateGCs (Private Function)
 ************************************************************************/
static	void
UpdateGCs(
	Widget	wid)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	XGCValues		values;
	XtGCMask		valueMask, unusedMask;

	/*
	 * Free any previous GC's
	 */
	if (cw->container.normalGC)
		XtReleaseGC(wid,cw->container.normalGC);
	if (cw->container.marqueeGC)
		XtReleaseGC(wid,cw->container.marqueeGC);
	/*
	 * Get GC's
	 */
	valueMask = GCForeground | GCBackground | GCGraphicsExposures;
	unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;
	values.foreground = cw->manager.foreground;
	values.background = cw->core.background_pixel;
	values.graphics_exposures = False;
	cw->container.normalGC = XtAllocateGC(wid, 0, valueMask, &values,
						GCClipMask | GCForeground, unusedMask);

	valueMask = GCForeground | GCSubwindowMode | GCFunction;
	values.foreground = cw->core.background_pixel ^
				cw->manager.foreground;
	values.subwindow_mode = IncludeInferiors;
	values.function = GXxor;
	cw->container.marqueeGC = XtAllocateGC(wid, 0, valueMask, &values,
						GCClipMask, 0);
}

/************************************************************************
 * GetSpatialSizeCellAdjustment (Private Function)
 * 	This routine does a trial placement of all items using the
 *	information in the array passed in.  If not all items fit,
 *	it adds 1 to width_in_cells & height_in_cells and calls itself
 *	recursively until everything fits.  This routine assumes
 *	placement goes left-to-right-top-to-bottom, but the calculations
 *	arrived at should work for all layoutDirections.
 ************************************************************************/
static  void
GetSpatialSizeCellAdjustment(
    Widget	wid,
    int		*parm_width_in_cells,
    int		*parm_height_in_cells,
    ContainerCwidCellInfo cwid_info,
    int		cwid_info_count)
{
    int				width_in_cells = *parm_width_in_cells;
    int				height_in_cells = *parm_height_in_cells;
    Boolean			all_cwids_fit;
    Boolean			cwid_placed;
    Boolean			this_spot_fits;
    Boolean 			*cell_occupied;
    int				i,start_row,start_col,row,col;

    cell_occupied = (Boolean *)XtCalloc((width_in_cells * height_in_cells),
						sizeof(Boolean));
    i = 0;
    all_cwids_fit = True;
    while ((i < cwid_info_count) && (all_cwids_fit))
      /*
       * Keep placing all the cwids into the cell_occupied[] grid until
       * one doesn't fit.
       */
      {
      start_col = 0;
      cwid_placed = False;
      while ((start_col < width_in_cells) && (!cwid_placed))
        {
	start_row = 0;
	while ((start_row < height_in_cells) && (!cwid_placed))
	  {
	  /*
	   * Cycle through all the [col,row] combinations in the
	   * cell_occupied[] grid until we find one that the current cwid
	   * will fit into.
	   */
	  this_spot_fits = True;
	  col = start_col;
	  while ((col < start_col + cwid_info[i].cwid_width_in_cells) &&
				(this_spot_fits))
	    {
	    row = start_row;
	    while ((row < start_row + cwid_info[i].cwid_height_in_cells) &&
				(this_spot_fits))
	      /*
	       * Test all the cells needed by this cwid at this particular
	       * spot in the cell_occupied[] grid until we see that one is
	       * out-of-bounds or already occupied.
	       */
	      {
	      this_spot_fits = ((col < width_in_cells) &&
				(row < height_in_cells) &&
			     (!(cell_occupied[col * width_in_cells + row])));
	      row++;
	      }
	    col++;
	    }
	  if (this_spot_fits)	/* Mark everything as occupied */
	    for (col = start_col;
		 col < start_col + cwid_info[i].cwid_width_in_cells;
		 col++)
		for (row = start_row;
		     row < start_row + cwid_info[i].cwid_height_in_cells;
		     row++)
		  cell_occupied[col * width_in_cells + row] = True;
	  cwid_placed = this_spot_fits;
	  start_row++;
	  }
	start_col++;
	}
      all_cwids_fit = cwid_placed;
      i++;
      }
    XtFree((char*)cell_occupied);
    if (!all_cwids_fit)
	/*
	 * Not big enough for all the cwids, increment width/height and
	 * try it again.
	 */
	{
	width_in_cells++;
	height_in_cells++;
	GetSpatialSizeCellAdjustment(wid,&width_in_cells,&height_in_cells,
					cwid_info,cwid_info_count);
	*parm_width_in_cells = width_in_cells;
	*parm_height_in_cells = height_in_cells;
	}
}


/************************************************************************
 * CreateIconHeader (Private Function)
 ************************************************************************/
static  void
CreateIconHeader(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    Widget header_parent = wid ;
    Arg                 sargs[10];
    Cardinal              n = 0;
    unsigned char scrollp = XmAPPLICATION_DEFINED ;

#define NO_PARENT 	0
#define SWINDOW_PARENT	1
#define CWINDOW_PARENT	2
    int inScrolledWindow = NO_PARENT;

    cw->container.self = True;
    cw->container.create_cwid_type = CONTAINER_HEADER;


    /*
     * Set up args for DrawingArea and Create.
     */

    {
    XmScrollFrameTrait	scrollFrameTrait = (XmScrollFrameTrait)
        XmeTraitGet((XtPointer)XtClass(XtParent(wid)),XmQTscrollFrame);
    if ((NULL != scrollFrameTrait) && scrollFrameTrait->getInfo(XtParent(wid),NULL,NULL,NULL))
	inScrolledWindow = SWINDOW_PARENT;
    if (NO_PARENT == inScrolledWindow)
	{
	XmClipWindowTrait clipWindowTrait = (XmClipWindowTrait)
	        XmeTraitGet((XtPointer)XtClass(XtParent(wid)),_XmQTclipWindow);
        if (NULL != clipWindowTrait)
		inScrolledWindow = CWINDOW_PARENT;
	}
    }

    if (NO_PARENT != inScrolledWindow)
    {
	Widget sWindow = (SWINDOW_PARENT == inScrolledWindow) ? XtParent(wid) : XtParent(XtParent(wid));
	scrollp = XmAUTOMATIC ;
	/* use an intermediate DA so that we can have margins */
	n = 0 ;
	XtSetArg(sargs[n], XmNscrolledWindowChildType, XmSCROLL_HOR); n++;

	XtSetArg(sargs[n], XmNmarginHeight,
		 cw->container.margin_h); n++ ;
	XtSetArg(sargs[n], XmNmarginWidth,
		 cw->container.margin_w); n++ ;
	XtSetArg(sargs[n], XmNforeground, cw->manager.foreground); n++;
	XtSetArg(sargs[n], XmNbackground, cw->core.background_pixel); n++;
	XtSetArg(sargs[n], XmNbackgroundPixmap,
		 cw->core.background_pixmap); n++;
	XtSetArg(sargs[n], XmNborderWidth, cw->core.border_width); n++;
	XtSetArg(sargs[n], XmNborderColor, cw->core.border_pixel); n++;
	XtSetArg(sargs[n], XmNborderPixmap, cw->core.border_pixmap); n++;
	XtSetArg(sargs[n], XmNtraversalOn, False); n++;
	header_parent = XmCreateDrawingArea(sWindow, DANAME, sargs, n);
    }

    /*
     * Set up args for IconHeader and Create.
     */
    n = 0;
    XtSetArg(sargs[n],XmNcontainerID,wid); n++;
    XtSetArg(sargs[n],XmNshadowThickness,0); n++;
    XtSetArg(sargs[n],XmNtraversalOn,False); n++;
    /* not really necessary */
    XtSetArg(sargs[n],XmNlargeIconPixmap, XmUNSPECIFIED_PIXMAP); n++;
    XtSetArg(sargs[n],XmNsmallIconPixmap, XmUNSPECIFIED_PIXMAP); n++;

    assert(cw->container.detail_heading_count > 0) ;

    XtSetArg(sargs[n],XmNlabelString,
	     cw->container.detail_heading[0]); n++;
    if (cw->container.detail_heading_count > 1) {
        XtSetArg(sargs[n],XmNdetail,&(cw->container.detail_heading[1]));
        n++;
    }
    XtSetArg(sargs[n],XmNdetailCount,
	     cw->container.detail_heading_count - 1); n++;

    cw->container.icon_header =
	XmCreateIconHeader(header_parent, HEADERNAME, sargs,n);

    if (scrollp == XmAUTOMATIC) {
	/* set y of the container so that it glue the
	   bottom of the Icon header *inside* the DA, that
	   is, it overlap the DA */
	 if (CtrLayoutIsDETAIL(cw))
	   /* if (0 == cw->core.y) */
	     cw->core.y = cw->container.icon_header->core.height ;

	/* also resize the DA to remove the bottom margin */
	XmeConfigureObject(header_parent,
			   header_parent->core.x,
			   header_parent->core.y,
			   cw->container.icon_header->core.width +
			   2*cw->container.margin_w,
			   cw->container.icon_header->core.height +
			   cw->container.margin_h,
			   header_parent->core.border_width);

	/* manage the child inside */
	XtManageChild(cw->container.icon_header);
	/* keep the reference to the DA parent, since that's the one
	   to manage/unmanaged all the time */
	cw->container.icon_header = header_parent ;
    }
    /* if not scrolling, no need to do anything it, it will
       be dealt with as a regular kid */

    /* set the internal creation flag down */
    cw->container.create_cwid_type = CONTAINER_ICON;
    cw->container.self = False;
}

/************************************************************************
 * GetRealIconHeader (Private Function)
 ************************************************************************/
static  Widget
GetRealIconHeader(
    Widget      wid)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;

    if (!cw->container.icon_header) return NULL;

    /* determine the real icon_header id */
    if (XtParent(cw->container.icon_header) != wid)
	/* we are in the SW auto case where a DA was created as
	   the parent of the icon_header and store in container
	   icon_header. */
	return
	    ((CompositeWidget)(cw->container.icon_header))
		->composite.children[0] ;
    else
	return cw->container.icon_header ;
}


/************************************************************************
 * UpdateIconHeader (Private Function)
 ************************************************************************/
static  void
UpdateIconHeader(
    Widget      wid,
    Boolean count_only)
{
    XmContainerWidget   cw = (XmContainerWidget)wid;
    Arg                 wargs[10];
    int                 n;
    Widget icon_header = GetRealIconHeader(wid);

    cw->container.self = True ;

    /*
     * Set up args for IconHeader.
     */
    assert (cw->container.detail_heading_count > 0);

    n = 0;
    if (!count_only) {
	XtSetArg(wargs[n],XmNlabelString,
		 cw->container.detail_heading[0]); n++;
	if (cw->container.detail_heading_count > 1) {
	    XtSetArg(wargs[n],XmNdetail,&(cw->container.detail_heading[1]));
	    n++;
	}
    }

    XtSetArg(wargs[n],XmNdetailCount,
			cw->container.detail_heading_count - 1); n++;
    XtSetValues(icon_header,wargs,n);

    cw->container.self = True ;
}

/************************************************************************
 * ChangeView (Private Function)
 ************************************************************************/
static	void
ChangeView(
    Widget		wid,
    unsigned char	view)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    CwidNode		node;
    Widget              cwid;

    node = cw->container.first_node;
    while (node)
	{
	cwid = node->widget_ptr;
	SetViewType(cwid,view);
	/* setting a new view type will have the child request
	   and get its preferred size */

	/*
	 * Get the next cwid, whether it's visible or not
	 */
	if (node->child_ptr)
	    node = node->child_ptr;
	else
	    if (node->next_ptr)
		node = node->next_ptr;
	    else
		node = GetNextUpLevelNode(node);
	}
}

/************************************************************************
 * NewNode (Private Function)
 ************************************************************************/
static  CwidNode
NewNode(
	Widget	cwid)
{
	XmContainerConstraint	c = GetContainerConstraint(cwid);
	CwidNode		new_node;

	/*
         * Create the new CwidNode structure.
         */
        new_node = (CwidNode)XtCalloc(1,sizeof(XmCwidNodeRec));
        new_node->widget_ptr = cwid;	/* node ----> widget link */
        c->node_ptr = new_node;         /* widget --> node link */
	return(new_node);
}

/************************************************************************
 * InsertNode (Private Function)
 ************************************************************************/
static	void
InsertNode(
	CwidNode	node)
{
	XmContainerWidget	cw;
	XmContainerConstraint	c;
	XmContainerConstraint	pc;	/* parent's constraints */
	XmContainerConstraint	sc;	/* sibling's constraints */
	Widget		cwid;
	CwidNode 	prev_node;
	CwidNode	next_node;
	CwidNode	parent_node;
	int count = 0;		/* resequence as we go along so that
				** XmNpositionIndex always reflects the child's
				** position within all widgets which name the
				** same widget as their XmNentryParent; no
				** widgets have the same value for
				** XmNpositionIndex; XmNpositionIndex has no
				** gaps
				*/

	cwid = node->widget_ptr;
	cw = (XmContainerWidget)XtParent(cwid);
	c = GetContainerConstraint(cwid);

	/*
	 * Find the first cwid within XmNentryParent, NULL if none.
	 */
	if (c->entry_parent == NULL)
	    {
	    parent_node = NULL;
	    prev_node = cw->container.first_node;
	    }
	else
	    {
	    pc = GetContainerConstraint(c->entry_parent);
	    parent_node = pc->node_ptr;
	    prev_node = parent_node->child_ptr;
	    }

	if (prev_node == NULL)
	    {
	    /*
	     * Assume this is the first and only cwid within XmNentryParent.
	     */
	    next_node = NULL;
	    }
	else
	{
	    /*
	     * Chain through this level until we find the correct position.
	     */
	    next_node = prev_node;
	    prev_node = NULL;
	    while (next_node)
		{
		sc = GetContainerConstraint(next_node->widget_ptr);
		if ((c->position_index != XmLAST_POSITION) &&
			    (c->position_index <= sc->position_index))
				break;
		sc->position_index = count++;
		prev_node = next_node;
		next_node = next_node->next_ptr;
		}
	}

	c->position_index = count++;

	/*
	 * Insert the node into the linked list.
	 */
	node->parent_ptr = parent_node;
	if ((node->prev_ptr = prev_node) != NULL)
		prev_node->next_ptr = node;
	if ((node->next_ptr = next_node) != NULL)
		next_node->prev_ptr = node;
	if ((parent_node) && (node->prev_ptr == NULL))
		parent_node->child_ptr = node;

	/* if we've done other parts right, we shouldn't need to do this part */
	next_node = node->next_ptr;
	while (next_node)
	{
		sc = GetContainerConstraint(next_node->widget_ptr);
		sc->position_index = count++;
		next_node = next_node->next_ptr;
	}

	/*
	 * Update cw->container.first_node, if we're now the first node.
	 */
	if (node->next_ptr == cw->container.first_node)
		cw->container.first_node = node;
}

/************************************************************************
 * SeverNode (Private Function)
 ************************************************************************/
static	void
SeverNode(
	CwidNode	node)
{
	XmContainerWidget	cw;
	CwidNode		parent_node;
	CwidNode		prev_node;
	CwidNode		next_node;

	if (node == NULL) return;

	cw = (XmContainerWidget)XtParent(node->widget_ptr);

	/*
	 * Find new cw->container.first_node, if we're now the first node.
	 */
	if (node == cw->container.first_node)
		{
		if (node->next_ptr)
			cw->container.first_node = node->next_ptr;
		else
			cw->container.first_node = GetNextUpLevelNode(node);
		}
	/*
	 * If we're child #1, fix up parent-child pointers.
	 */
	if ((node->prev_ptr == NULL) && (node->parent_ptr))
		{
		parent_node = node->parent_ptr;
		parent_node->child_ptr = node->next_ptr;
		}

	/*
	 * Unlink node from the linked list.
	 */
	if (node->prev_ptr)
		{
		prev_node = node->prev_ptr;
		prev_node->next_ptr = node->next_ptr;
		}
	if (node->next_ptr)
		{
		next_node = node->next_ptr;
		next_node->prev_ptr = node->prev_ptr;
		}
}

/************************************************************************
 * DeleteNode (Private Function)
 ************************************************************************/
static  void
DeleteNode(
        Widget  cwid)
{
        XmContainerConstraint   c = GetContainerConstraint(cwid);
        CwidNode        target_node;
        CwidNode        child_node;
        CwidNode        target_child;

        if ((target_node = c->node_ptr) == NULL) return;

        /*
         * Delete any children first.
         */
        child_node = target_node->child_ptr;
        while (child_node)
                {
                target_child = child_node;
                child_node = child_node->next_ptr;
                DeleteNode(target_child->widget_ptr);
                }
        SeverNode(target_node);
        XtFree((char*)target_node);
        c->node_ptr = NULL;
        c->visible_in_outline = False;
}

/************************************************************************
 * GetFirstNode (Private Function)
 ************************************************************************/
static	CwidNode
GetFirstNode(
    XmContainerWidget	cw)
{
  if (NodeIsActive(cw->container.first_node))
      return cw->container.first_node;
  else
      return GetNextNode(cw->container.first_node) ;
}


/************************************************************************
 * GetNextNode (Private Function)
 ************************************************************************/
static	CwidNode
GetNextNode(
    CwidNode 	start_node)
{
    XmContainerWidget	cw;
    CwidNode 		node;

    if (!start_node)
	return(NULL);
    cw = (XmContainerWidget)XtParent(start_node->widget_ptr);

    if (CtrLayoutIsSPATIAL(cw))
	{
	node = start_node->next_ptr;
	while (node)
	    {
	    if (NodeIsActive(node))
		return(node);
	    node = node->next_ptr;
	    }
	return(NULL);
	}

    /* depth-first search of tree */
    if (NodeIsActive(start_node) && (start_node->child_ptr))
	node = start_node->child_ptr;
    else
	if (start_node->next_ptr)
	    node = start_node->next_ptr;
	else
	    node = GetNextUpLevelNode(start_node);
    while (node)
	{
	if (NodeIsActive(node))
	    return(node);
	if (node->next_ptr)
	    node = node->next_ptr;
	else
	    node = GetNextUpLevelNode(node);
	}
    return(NULL);
}

/************************************************************************
 * NodeIsActive (Private Function)
 ************************************************************************/
static Boolean
NodeIsActive(
    CwidNode	node)
{
    XmContainerWidget   cw;
    XmContainerConstraint c;

    if (!node)
	return(False);
    if (!XtIsManaged(node->widget_ptr))
	return(False);
    cw = (XmContainerWidget)XtParent(node->widget_ptr);
    if (CtrLayoutIsOUTLINE_DETAIL(cw))
	{
	c = GetContainerConstraint(node->widget_ptr);
	if (!c->visible_in_outline)
	    return(False);
	}
    return(True);
}


/************************************************************************
 * GetNextUpLevelNode (Private Function)
 ************************************************************************/
static  CwidNode
GetNextUpLevelNode(
        CwidNode	start_node)
{
	CwidNode 	node;

	node = start_node;
	while (node)
		{
		node = node->parent_ptr;
		if (node)
			if (node->next_ptr)
				return(node->next_ptr);
		}
	return(NULL);
}

static void ContainerResetDepths
	(XmContainerConstraint	c)
{
	CwidNode 	node = c->node_ptr->child_ptr;

	while (node)
	{
		Widget child = node->widget_ptr;
		XmContainerConstraint cc = GetContainerConstraint(child);
		cc->depth = c->depth + 1;
		ContainerResetDepths(cc);
		node = node->next_ptr;
	}
}

static void FindMaxDepths(XmContainerConstraint	c, Widget w)
{
	XmContainerWidget cw = (XmContainerWidget)w;
	CwidNode node = c->node_ptr->child_ptr;
	while (node) {
		Widget child = node->widget_ptr;
		XmContainerConstraint cc = GetContainerConstraint(child);
		cw->container.max_depth = MAX(cw->container.max_depth, cc->depth);
		FindMaxDepths(cc, (Widget)cw);
		node = node->next_ptr;
	}
}

/* true if newEntryParent (or any other widget) is a descendant of the target;
** determine by looking upward in the tree for a match. Returns True for the
** widget itself.
*/
static Boolean ContainerIsDescendant
	(Widget containerChild, Widget newEntryParent)
{
	XmContainerConstraint nepc = GetContainerConstraint(newEntryParent);
	CwidNode node = nepc->node_ptr;
	while (node)
	{
		if (node->widget_ptr == containerChild)
			return True;
		node = node->parent_ptr;
	}
	return False;
}

static void ContainerResequenceNodes
	(XmContainerWidget cw, Widget entryParent)
{
	XmContainerConstraint c;
	CwidNode 	node;
	int count = 0;

	if (entryParent)
	{
 		c = GetContainerConstraint(entryParent);
 		node = c->node_ptr->child_ptr;
	}
	else
	{
		node = cw->container.first_node;
	}

	while (node)
	{
		c = GetContainerConstraint(node->widget_ptr);
		c->position_index = count++;
		node = node->next_ptr;
	}
}


/************************************************************************
 * StartSelect (Private Function)
 ************************************************************************/
static  void
StartSelect(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			current_cwid;
	XmContainerConstraint	c = NULL;

	current_cwid = ObjectAtPoint(wid,event->xbutton.x,event->xbutton.y);
	/* Handle ObjectAtPoint returning an outline button */
	if ((current_cwid) && CtrOUTLINE_BUTTON(current_cwid))
	  current_cwid = NULL;

	cw->container.no_auto_sel_changes = False;

	/*
	 * Setup for marque drawing and multiclick time.
	 * Will return true if XmNdefaultActionCallback invoked.
	 */
        if (SetupDrag(wid, event, params, num_params))
	    return;

	if (CtrPolicyIsSINGLE(cw))
		{
		  /* CR 8400 - clicking on selected item should
		     unselect in SINGLE mode */

		  /* Fix by Metro Link.
		     First, get the constraints if current_cwid is valid. */
		  if (current_cwid)
		    c = GetContainerConstraint(current_cwid);

		  /* Now check to see if the *item* is selected. */
		  if (current_cwid
		      && current_cwid == cw->container.anchor_cwid &&
		      c->selection_visual == XmSELECTED &&
		      cw->container.selection_state == XmSELECTED) {
		    /* Unselect if clicking on a selected item */
		    cw->container.selection_state = XmNOT_SELECTED;
		    MarkCwid(current_cwid,False);
		    cw->container.anchor_cwid = NULL;
		    return;
		  }
		cw->container.no_auto_sel_changes |= DeselectAllCwids(wid);
		cw->container.anchor_cwid = current_cwid;
		if (cw->container.anchor_cwid == NULL)
			return;
		cw->container.no_auto_sel_changes |=
				MarkCwid(cw->container.anchor_cwid,False);
		SetLocationCursor(cw->container.anchor_cwid);
		return;
		}
	if (CtrPolicyIsBROWSE(cw))
		{
                if (current_cwid != cw->container.anchor_cwid)
			{
			cw->container.no_auto_sel_changes |=
					DeselectAllCwids(wid);
			cw->container.anchor_cwid = current_cwid;
			if (cw->container.anchor_cwid)
				{
				cw->container.no_auto_sel_changes |=
					MarkCwid(cw->container.anchor_cwid,
							True);
				SetLocationCursor(cw->container.anchor_cwid);
				}
			}
		if (CtrIsAUTO_SELECT(cw))
			CallSelectCB(wid,event,XmAUTO_BEGIN);
		return;
		}
	if (!cw->container.extending_mode)
		cw->container.no_auto_sel_changes |= DeselectAllCwids(wid);
	cw->container.anchor_cwid = current_cwid;
	if (cw->container.anchor_cwid)
	  SetLocationCursor(cw->container.anchor_cwid);

	if (CtrTechIsTOUCH_OVER(cw))
        {
		if (cw->container.anchor_cwid == NULL)
			cw->container.marquee_mode = True;
		else
			cw->container.marquee_mode = False;
        }
	if (cw->container.anchor_cwid == NULL)
		{
		if (CtrIsAUTO_SELECT(cw))
			CallSelectCB(wid,event,XmAUTO_BEGIN);
		cw->container.started_in_anchor = False;
		return;
		}
	else
		cw->container.started_in_anchor = True;
	if (cw->container.extending_mode)
		{
		c = GetContainerConstraint(cw->container.anchor_cwid);
		if (c->selection_state == XmSELECTED)
			cw->container.selection_state = XmNOT_SELECTED;
		else
			cw->container.selection_state = XmSELECTED;
		}
	cw->container.no_auto_sel_changes |=
			MarkCwid(cw->container.anchor_cwid,True);
	if (CtrIsAUTO_SELECT(cw))
		CallSelectCB(wid,event,XmAUTO_BEGIN);
	if ((CtrTechIsMARQUEE_ES(cw) || CtrTechIsMARQUEE_EB(cw)) &&
	    (!CtrLayoutIsDETAIL(cw)))
		{
		XSetClipMask(XtDisplay(wid), cw->container.marqueeGC, None);
		RecalcMarquee(wid,cw->container.anchor_cwid,
				event->xbutton.x,event->xbutton.y);
		DrawMarquee(wid);
		cw->container.marquee_drawn = True;
		}
}


static  Boolean
SetupDrag(Widget wid,
	  XEvent *event,
	  String *params,	/* unused */
	  Cardinal *num_params)	/* unused */
{
  XmContainerWidget	cw = (XmContainerWidget)wid;
  Widget		current_cwid;
  int			multi_click_time;
  Time			click_time;

  /* Figure out double clicking */
  current_cwid = ObjectAtPoint(wid,event->xbutton.x,event->xbutton.y);
  /* Handle ObjectAtPoint returning an outline button */
  if ((current_cwid) && CtrOUTLINE_BUTTON(current_cwid))
    current_cwid = NULL;

  multi_click_time = XtGetMultiClickTime(XtDisplay(wid));
  click_time = event->xbutton.time;
  if ((cw->container.anchor_cwid == current_cwid) &&
      ((click_time - cw->container.last_click_time) < multi_click_time))
    {
      cw->container.last_click_time = click_time;
      if (cw->container.anchor_cwid)
	CallActionCB(cw->container.anchor_cwid,event);
      cw->container.cancel_pressed = True;
      return(True);
    }

  cw->container.last_click_time = event->xbutton.time;

  cw->container.anchor_point.x = event->xbutton.x;
  cw->container.marquee_smallest.x = event->xbutton.x;
  cw->container.marquee_largest.x = event->xbutton.x;
  cw->container.anchor_point.y = event->xbutton.y;
  cw->container.marquee_smallest.y = event->xbutton.y;
  cw->container.marquee_largest.y = event->xbutton.y;
  return(False);
}

/************************************************************************
 * ProcessButtonMotion (Private Function)
 ************************************************************************/
static  Boolean
ProcessButtonMotion(
        Widget          wid,
        XEvent          *event,
        String          *params,	/* unused */
        Cardinal        *num_params)	/* unused */
{
        XmContainerWidget       cw = (XmContainerWidget)wid;
        Widget                  current_cwid;
	Boolean			selection_changes = False;
	Boolean			find_anchor = False;
	XmContainerConstraint	c;

        current_cwid = ObjectAtPoint(wid,event->xbutton.x,event->xbutton.y);
	/* Handle ObjectAtPoint returning an outline button */
	if ((current_cwid) && CtrOUTLINE_BUTTON(current_cwid))
	  current_cwid = NULL;

        if (CtrPolicyIsBROWSE(cw))
                {
		if ((cw->container.extend_pressed) ||
		    (cw->container.toggle_pressed))
			return(False);
                if (current_cwid == cw->container.anchor_cwid)
                        return(False);
                if (cw->container.anchor_cwid)
			{
			cw->container.selection_state = XmNOT_SELECTED;
                        selection_changes = MarkCwid(cw->container.anchor_cwid,
							False);
			cw->container.selection_state = XmSELECTED;
			}
		if (current_cwid)
			selection_changes |= MarkCwid(current_cwid,True);
                cw->container.anchor_cwid = current_cwid;
                return(selection_changes);
                }
	/* CtrPolicyIsMULTIPLE || CtrPolicyIsEXTENDED */
	/* CUA Random Selection Technique */
	if (CtrLayoutIsSPATIAL(cw) && (!cw->container.marquee_mode))
		{
		if (current_cwid== NULL)
			return(False);
		if (cw->container.anchor_cwid == NULL)
			{
			/* Gotta have a reference point for toggling */
			c = GetContainerConstraint(current_cwid);
			if (c->selection_visual == XmSELECTED)
				cw->container.selection_state = XmNOT_SELECTED;
			else
				cw->container.selection_state = XmSELECTED;
			cw->container.anchor_cwid = current_cwid;
			}
		return(MarkCwid(current_cwid,True));
		}
	/* CUA Range Selection Technique */
	if (CtrLayoutIsDETAIL(cw) ||
	   (CtrLayoutIsOUTLINE_DETAIL(cw) && !cw->container.marquee_mode))
		return(MarkCwidsInRange(wid,cw->container.anchor_cwid,
					current_cwid,(Boolean)True));
	/* CUA Marquee Selection Technique */
	find_anchor = (cw->container.anchor_cwid == NULL);
	RecalcMarquee(wid,current_cwid,event->xbutton.x,event->xbutton.y);
	selection_changes = MarkCwidsInMarquee(wid,find_anchor,True);
	DrawMarquee(wid);
	cw->container.marquee_drawn = True;
	return(selection_changes);
}

/************************************************************************
 *
 *  ObjectAtPoint method
 *	Given a composite widget and an (x, y) coordinate, see if the
 *	(x, y) lies within one of the object (gadget or widget) contained
 *      within the composite.
 *
 ************************************************************************/
static	Widget
ObjectAtPoint(Widget wid, Position x, Position y)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	CwidNode		node;
	Widget			prev_cwid;
	XmGadget		g;
	XmPointInTrait          pointInTrait ;

	node = cw->container.first_node;
	prev_cwid = NULL;
	while (node) {
	  XmContainerConstraint c;

	  g = (XmGadget)node->widget_ptr;
	  c = GetContainerConstraint((Widget) g);

	  /* If we are in OUTLINE_DETAIL,  then check for hit in outline
	     button,  if there is one */
	  if (CtrLayoutIsOUTLINE_DETAIL(cw) &&
	      c -> related_cwid != (Widget) NULL) {
	    Widget outline_button = c -> related_cwid;

	    pointInTrait = (XmPointInTrait)
	      XmeTraitGet((XtPointer)XtClass(outline_button),XmQTpointIn) ;
	    if (pointInTrait) {
	      if (pointInTrait->pointIn(outline_button, x, y))
		return outline_button;
	    } else
	      if ((XtX(outline_button) <= x) &&
		  (XtX(outline_button) + XtWidth(outline_button) >= x) &&
		  (XtY(outline_button) <= y) &&
		  (XtY(outline_button) + XtHeight(outline_button) >= y))
		return(outline_button);
	  }

	  pointInTrait = (XmPointInTrait)
	    XmeTraitGet((XtPointer)XtClass(node->widget_ptr),XmQTpointIn) ;

	  if ((y < g->rectangle.y) && CtrLayoutIsOUTLINE_DETAIL(cw))
	    return(prev_cwid);

	  if (pointInTrait && ! CtrLayoutIsOUTLINE_DETAIL(cw)) {
	    if (pointInTrait->pointIn(node->widget_ptr, x, y))
	      return node->widget_ptr ;
	  } else
	    if ((g->rectangle.x <= x) &&
		(x <= g->rectangle.x + g->rectangle.width) &&
		(g->rectangle.y <= y) &&
		(y <= g->rectangle.y + g->rectangle.height))
	      return(node->widget_ptr);

	  if ((g->rectangle.y + g->rectangle.height > cw->core.height) &&
	      CtrLayoutIsOUTLINE_DETAIL(cw))
	    return(NULL);

	  if ((g->rectangle.x <= x) &&
	      (x <= g->rectangle.x + g->rectangle.width))
	    prev_cwid = node->widget_ptr;
	  else
	    prev_cwid = NULL;

	  node = GetNextNode(node);
	}
	return(NULL);
}

/************************************************************************
 * SelectAllCwids (Private Function)
 ************************************************************************/
static 	Boolean
SelectAllCwids(
	Widget	wid)
{
        XmContainerWidget       cw = (XmContainerWidget)wid;
        CwidNode  	        node;
	Boolean			selection_changes = False;

	/*
	 * Mark all visible cwids as XmSELECTED
	 */
	cw->container.selection_state = XmSELECTED;
        node = cw->container.first_node;
        while (node)
                {
		selection_changes |= MarkCwid(node->widget_ptr,False);
                node = GetNextNode(node);
                }
	return(selection_changes);
}

/************************************************************************
 * DeselectAllCwids (Private Function)
 ************************************************************************/
static	Boolean
DeselectAllCwids(
	Widget  wid)
{
	XmContainerWidget       cw = (XmContainerWidget)wid;
        CwidNode  	        node;
	Boolean			selection_changes = False;

	if (cw->container.selected_item_count == 0)
		{
		cw->container.selection_state = XmSELECTED;
		return(False);
		}

        /*
         * Mark all visible cwids as XmNOT_SELECTED
         */
        cw->container.selection_state = XmNOT_SELECTED;
        node = cw->container.first_node;
        while (node)
                {
		selection_changes |= MarkCwid(node->widget_ptr,False);
		node = GetNextNode(node);
		if (cw->container.selected_item_count == 0)
			{
			cw->container.selection_state = XmSELECTED;
			return(selection_changes);
			}
		}
        /*
         * Didn't work.  There must be some hidden child cwids that are
         * XmSELECTED.  We'll have to traverse the entire list.
         */
        node = cw->container.first_node;
        while (node)
                {
		selection_changes |= MarkCwid(node->widget_ptr,False);
		if (cw->container.selected_item_count == 0)
			{
			cw->container.selection_state = XmSELECTED;
			return(selection_changes);
			}
                /*
                 * depth-first search of tree, whether the cwids are
                 * visible or not.
                 */
                if (node->child_ptr)
                        node = node->child_ptr;
                else
                        if (node->next_ptr)
                                node = node->next_ptr;
                        else
                                node = GetNextUpLevelNode(node);
                }
	cw->container.selection_state = XmSELECTED;
	return(selection_changes);
}

/************************************************************************
 * MarkCwid (Private Function)
 ************************************************************************/
static	Boolean
MarkCwid(
    Widget		cwid,
    Boolean		visual_only)
{
    XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
    XmContainerConstraint c = GetContainerConstraint(cwid);
    Boolean		selection_changes = False;

    if (XtIsSensitive(cwid))
      	{
      	if (cw->container.selection_state != c->selection_visual)
	    {
	    c->selection_visual = cw->container.selection_state;
	    SetVisualEmphasis(cwid,c->selection_visual);
	    if (c->selection_visual == XmSELECTED)
	     	cw->container.selected_item_count++;
	    else
	    	cw->container.selected_item_count--;
	    selection_changes = True;
	    }
      	if (!visual_only)
	    c->selection_state = c->selection_visual;
	}
    return(selection_changes);
}

/************************************************************************
 * UnmarkCwidVisual (Private Function)
 ************************************************************************/
static	Boolean
UnmarkCwidVisual(
        Widget          cwid)
{
        XmContainerWidget       cw = (XmContainerWidget)XtParent(cwid);
        XmContainerConstraint   c = GetContainerConstraint(cwid);

	/*
	 * If the item visual matches its select state, or the item
	 * visual doesn't match our current select/unselect action,
	 * then return.
	 * Otherwise, restore the item's visual to its select state
	 * and add to/remove from the selected items list.
	 */
	if ((c->selection_visual == c->selection_state) ||
	    (c->selection_visual != cw->container.selection_state))
		return(False);
	c->selection_visual = c->selection_state;
	SetVisualEmphasis(cwid,c->selection_visual);
	if (c->selection_visual == XmSELECTED)
                        cw->container.selected_item_count++;
                else
                        cw->container.selected_item_count--;
	return(True);
}

/************************************************************************
 * SetMarkedCwids (Private Function)
 ************************************************************************/
static	void
SetMarkedCwids(
	Widget	wid)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	CwidNode 		node;
        XmContainerConstraint	c;

	node = cw->container.first_node;
        while (node)
                {
		c = GetContainerConstraint(node->widget_ptr);
		c->selection_state = c->selection_visual;
                node = GetNextNode(node);
                }
}

/************************************************************************
 * ResetMarkedCwids (Private Function)
 ************************************************************************/
static	Boolean
ResetMarkedCwids(
	Widget	wid)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	CwidNode 		node;
	Boolean			selection_changes = False;

	node = cw->container.first_node;
	while (node)
		{
		selection_changes |= UnmarkCwidVisual(node->widget_ptr);
		node = GetNextNode(node);
		}
	return(selection_changes);
}

/************************************************************************
 * MarkCwidsInRange (Private Function)
 ************************************************************************/
static  Boolean
MarkCwidsInRange(
	Widget	wid,
        Widget  cwid1,
	Widget	cwid2,
	Boolean	visual_only)
{
	XmContainerWidget       cw = (XmContainerWidget)wid;
        CwidNode		node;
	Boolean			cwid1_found = False;
	Boolean			cwid2_found = False;
	Boolean			marking_started = False;
	Boolean			marking_ended = False;
	Boolean			selection_changes = False;

	node = cw->container.first_node;
	if (cwid1 == NULL)
		if ((cwid1 = cwid2) == NULL) return(False);
	if (cwid2 == NULL)
		if ((cwid2 = cwid1) == NULL) return(False);
	while (node)
		{
		if (node->widget_ptr == cwid1)
			cwid1_found = True;
		if (node->widget_ptr == cwid2)
			cwid2_found = True;
		if ((cwid1_found || cwid2_found) && (!marking_started))
			marking_started = True;
		if (marking_started && (!marking_ended))
			selection_changes |= MarkCwid(node->widget_ptr,
						visual_only);
		else
			selection_changes |= UnmarkCwidVisual(node->widget_ptr);
		if (cwid1_found && cwid2_found && marking_started)
			marking_ended = True;
		node = GetNextNode(node);
		}
	return(selection_changes);
}

/************************************************************************
 * MarkCwidsInMarquee (Private Function)
 ************************************************************************/
static  Boolean
MarkCwidsInMarquee(
        Widget  wid,
	Boolean	find_anchor,
        Boolean visual_only)
{
	XmContainerWidget       cw = (XmContainerWidget)wid;
        CwidNode		node;
        Boolean                 selection_changes = False;
	XmContainerConstraint	c;

	node = cw->container.first_node;
	while (node)
		{
		if (InMarquee(node->widget_ptr))
			{
			if (find_anchor)
				{
				cw->container.anchor_cwid = node->widget_ptr;
				c = GetContainerConstraint(node->widget_ptr);
				if (c->selection_state == XmSELECTED)
					cw->container.selection_state
							= XmNOT_SELECTED;
				else
					cw->container.selection_state
							= XmSELECTED;
				find_anchor = False;
				}
			selection_changes |= MarkCwid(node->widget_ptr,
						visual_only);
			}
		else
			selection_changes |= UnmarkCwidVisual(node->widget_ptr);
		node = GetNextNode(node);
		}
	return(selection_changes);
}

/************************************************************************
 * InMarquee (Private Function)
 ************************************************************************/
static	Boolean
InMarquee(
	Widget	cwid)
{
	XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
	Position		cwid_x,cwid_y,cwid_xw,cwid_yh;
	Dimension		width,height;

	XtVaGetValues(cwid,XmNx,&cwid_x,XmNy,&cwid_y,
		XmNwidth,&width,XmNheight,&height,NULL);
	cwid_xw = cwid_x + (Position)width;
	cwid_yh = cwid_y + (Position)height;
	return(	(cwid_x >= cw->container.marquee_start.x) &&
		(cwid_y >=  cw->container.marquee_start.y) &&
		(cwid_xw <= cw->container.marquee_end.x) &&
		(cwid_yh <= cw->container.marquee_end.y));
}

/************************************************************************
 * RecalcMarquee (Private Function)
 ************************************************************************/
static	void
RecalcMarquee(
	Widget		wid,
	Widget		cwid,
	Position	x,
	Position	y)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Dimension		width,height;
	Position		anchor_x,anchor_y,anchor_xw,anchor_yh;
	Position		cwid_x,cwid_y,cwid_xw,cwid_yh;

	/*
	 * Erase any marquee that is currently drawn.
	 */
	if (cw->container.marquee_drawn)
		{
		DrawMarquee(wid);
		cw->container.marquee_drawn = False;
		}
	/*
	 * Set the new marquee start and end points depending upon
	 * the XmNselectionTechnique resource.
	 * If XmMARQUEE_EXTEND_BOTH, use the anchor_cwid and current_cwid
	 *      corners as endpoints (if they are given).
	 * If XmMARQUEE_EXTEND_START, use the anchor_cwid corners (if given)
	 * 	and the current pointer position as endpoints.
	 * If XmMARQUEE, use the original anchor points and the current
	 *      pointer position, given by the x & y parameters.
	 * container.marquee_start.x & marquee_start.y always marks upper left.
	 * container.marquee_end.x & marquee_end.y always marks lower right.
	 */
	switch(cw->container.selection_technique)
	{
	case	XmMARQUEE_EXTEND_BOTH:
			if ((cw->container.started_in_anchor) && (cwid))
				{
				XtVaGetValues(cw->container.anchor_cwid,
					XmNx,&anchor_x,XmNy,&anchor_y,
					XmNwidth,&width,
					XmNheight,&height,NULL);
				anchor_xw = anchor_x + (Position)width;
				anchor_yh = anchor_y + (Position)height;
				/* CR9113: adjust marquee to encompass child */
				if(anchor_x > 0) anchor_x--, width++;
				if(anchor_y > 0) anchor_y--, height++;

				XtVaGetValues(cwid,XmNx,&cwid_x,XmNy,&cwid_y,
					XmNwidth,&width,
					XmNheight,&height,NULL);
				/* CR9113: adjust marquee to encompass child */
				if(cwid_x > 0) cwid_x--, width++;
				if(cwid_y > 0) cwid_y--, height++;

				cwid_xw = cwid_x + (Position)width;
				cwid_yh = cwid_y + (Position)height;
				cw->container.marquee_start.x =
					MIN(anchor_x,cwid_x);
				cw->container.marquee_start.y =
					MIN(anchor_y,cwid_y);
				cw->container.marquee_end.x =
					MAX(anchor_xw,cwid_xw);
				cw->container.marquee_end.y =
					MAX(anchor_yh,cwid_yh);
				break;
				}
			if (cw->container.started_in_anchor)
				cwid = cw->container.anchor_cwid;
			if ((!cw->container.started_in_anchor) && (cwid))
				{
				x = cw->container.anchor_point.x;
				y = cw->container.anchor_point.y;
				}
	case	XmMARQUEE_EXTEND_START:
			if CtrTechIsMARQUEE_ES(cw)
                        {
				if (cw->container.started_in_anchor)
					cwid = cw->container.anchor_cwid;
				else
					cwid = NULL;
                        }
			if (cwid)
				{
				XtVaGetValues(cwid,XmNx,&cwid_x,
					XmNy,&cwid_y,XmNwidth,&width,
					XmNheight,&height,NULL);
				/* CR9113: adjust marquee to encompass child */
				if(cwid_x > 0) cwid_x--, width++;
				if(cwid_y > 0) cwid_y--, height++;

				cwid_xw = cwid_x + (Position)width;
				cwid_yh = cwid_y + (Position)height;
				if (x <= cwid_xw)
					{
					cw->container.marquee_start.x =
						MIN(x,cwid_x);
					cw->container.marquee_end.x = cwid_xw;
					}
				else
					{
					cw->container.marquee_start.x = cwid_x;
					cw->container.marquee_end.x =
						MAX(x,cwid_xw);
					}
				if (y <= cwid_yh)
					{
					cw->container.marquee_start.y =
						MIN(y,cwid_y);
					cw->container.marquee_end.y = cwid_yh;
					}
				else
					{
					cw->container.marquee_start.y = cwid_y;
					cw->container.marquee_end.y =
						MAX(y,cwid_yh);
					}
				break;
				}
	case	XmMARQUEE:
	case	XmTOUCH_OVER:
			cw->container.marquee_start.x =
				MIN(x,cw->container.anchor_point.x);
			cw->container.marquee_start.y =
				MIN(y,cw->container.anchor_point.y);
			cw->container.marquee_end.x =
				MAX(x,cw->container.anchor_point.x);
			cw->container.marquee_end.y =
				MAX(y,cw->container.anchor_point.y);
			break;
	default:
			return;
	}
	/*
	 * Keep track of the largest area covered by the marquee.
	 */
	cw->container.marquee_smallest.x = MIN(cw->container.marquee_start.x,
				cw->container.marquee_smallest.x);
	cw->container.marquee_smallest.y = MIN(cw->container.marquee_start.y,
				cw->container.marquee_smallest.y);
	cw->container.marquee_largest.x = MAX(cw->container.marquee_end.x,
				cw->container.marquee_largest.x);
	cw->container.marquee_largest.y = MAX(cw->container.marquee_end.y,
				cw->container.marquee_largest.y);
}

/************************************************************************
 * DrawMarquee (Private Function)
 ************************************************************************/
static  void
DrawMarquee(
        Widget          wid)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Dimension		width,height;

	/*
	 * If the container widget is realized, draw the marquee rectangle
	 * using the gc parameter.
	 */
	if (!XtIsRealized(wid))
		return;
	width = (Dimension)(cw->container.marquee_end.x
				- cw->container.marquee_start.x);
	height = (Dimension)(cw->container.marquee_end.y
				- cw->container.marquee_start.y);
	XDrawRectangle(XtDisplay(wid),XtWindow(wid),cw->container.marqueeGC,
		cw->container.marquee_start.x,cw->container.marquee_start.y,
		width,height);
}

/************************************************************************
 * KBSelect (Private Function)
 ************************************************************************/
static  void
KBSelect(
	Widget		wid,
	XEvent		*event,
	String		*params,	/* unused */
	Cardinal	*num_params)	/* unused */
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	Widget			focus_cwid = XmGetFocusWidget(wid);
	XmContainerConstraint	c;

	if ((focus_cwid == wid) || (focus_cwid == NULL))
		return;
	cw->container.no_auto_sel_changes = False;
	c = GetContainerConstraint(focus_cwid);
	cw->container.anchor_cwid = focus_cwid;
	if ((!cw->container.extending_mode || CtrPolicyIsSINGLE(cw)) &&
	    ((cw->container.selected_item_count > 1) ||
	     (c->selection_state != XmSELECTED)))
		cw->container.no_auto_sel_changes |= DeselectAllCwids(wid);
	if (cw->container.extending_mode)
		{
		if (c->selection_state == XmSELECTED)
			cw->container.selection_state = XmNOT_SELECTED;
		else
			cw->container.selection_state = XmSELECTED;
		}
	cw->container.no_auto_sel_changes |= MarkCwid(focus_cwid,False);
	GainPrimary(wid,event->xbutton.time);
	if (CtrIsAUTO_SELECT(cw))
		{
		CallSelectCB(wid,event,XmAUTO_BEGIN);
		CallSelectCB(wid,event,XmAUTO_NO_CHANGE);
		}
	else
		if (cw->container.no_auto_sel_changes)
			CallSelectCB(wid,event,XmAUTO_UNSET);
}

/************************************************************************
 * SetLocationCursor (Private Function)
 ************************************************************************/
static	void
SetLocationCursor(
    Widget	cwid)
{
    if (cwid == NULL)
	return;
    if (cwid == XmGetFocusWidget(XtParent(cwid)))
	return;
    XmProcessTraversal(cwid,XmTRAVERSE_CURRENT);
}


/************************************************************************
 * CalcNextLocationCursor (Private Function)
 ************************************************************************/
static void
CalcNextLocationCursor(
    Widget  wid,
    String  direction)
{
    XmContainerWidget cw = (XmContainerWidget)wid;
    int move_to;
    Widget w;

    if (_XmConvertActionParamToRepTypeId((Widget) cw,
                         XmRID_CONTAINER_CURSOR_ACTION_PARAMS,
                         direction, False, &move_to) == False)
    {
        /* We couldn't convert the value. Just assume a value of _FIRST. */
        move_to = _FIRST;
    }

    if (CtrLayoutIsSPATIAL(cw))
        {
        if (move_to == _LEFT)
            XmProcessTraversal(wid, XmTRAVERSE_LEFT);
        else if (move_to == _RIGHT)
            XmProcessTraversal(wid, XmTRAVERSE_RIGHT);
        else if (move_to == _UP)
            XmProcessTraversal(wid, XmTRAVERSE_UP);
        else if (move_to == _DOWN)
            XmProcessTraversal(wid, XmTRAVERSE_DOWN);
        else if (move_to == _FIRST)
            XmProcessTraversal(wid, XmTRAVERSE_HOME);
        else if (move_to == _LAST)
            XmProcessTraversal(cw->container.last_node->widget_ptr,
                                XmTRAVERSE_CURRENT);
        }
    else /* CtrLayoutIsOUTLINE_DETAIL(cw) */
        {
        if (move_to == _LEFT)
            XmProcessTraversal(wid, XmTRAVERSE_LEFT);
        else if (move_to == _RIGHT)
            XmProcessTraversal(wid, XmTRAVERSE_RIGHT);
        else if (move_to == _UP)
            XmProcessTraversal(wid, XmTRAVERSE_UP);
        else if (move_to == _DOWN)
            XmProcessTraversal(wid, XmTRAVERSE_DOWN);
        else if (move_to == _FIRST)
            XmProcessTraversal(wid, XmTRAVERSE_HOME);
        else if (move_to == _LAST)
	    {
            w = GetLastTraversalWidget(cw,
				cw->container.first_node->widget_ptr, False);
	    if (w)
		XmProcessTraversal(w, XmTRAVERSE_CURRENT);
	    }
        }
}


/************************************************************************
 * RedirectTraversal (ControlTraversal Trait Method)
 ************************************************************************/
static Widget
RedirectTraversal(
    Widget               old_focus,
    Widget               new_focus,
    unsigned int         focus_policy,
    XmTraversalDirection direction,
    unsigned int         pass)
{
    XmContainerWidget cw;
    Widget to_widget = new_focus;
    Boolean wrap;

  if ( (old_focus == NULL)
    || (focus_policy != XmEXPLICIT)
    || (   direction != XmTRAVERSE_PREV
	&& direction != XmTRAVERSE_NEXT
	&& direction != XmTRAVERSE_LEFT
	&& direction != XmTRAVERSE_RIGHT
	&& direction != XmTRAVERSE_UP
	&& direction != XmTRAVERSE_DOWN
	&& direction != XmTRAVERSE_HOME))
    return new_focus;

    if ( ((cw=(XmContainerWidget)XtParent(old_focus)) == NULL)
      || (!XmIsContainer(cw))
      || (CtrLayoutIsSPATIAL(cw))
      || (CtrOUTLINE_BUTTON(old_focus)))
	return new_focus;

    wrap = !XmIsClipWindow((XmClipWindowWidget)XtParent(cw));

    switch (direction)
	{
	case XmTRAVERSE_PREV:
	case XmTRAVERSE_LEFT:
	case XmTRAVERSE_UP:
	    to_widget = GetPrevTraversalWidget(cw, old_focus, wrap);
	    break;
	case XmTRAVERSE_NEXT:
	case XmTRAVERSE_RIGHT:
	case XmTRAVERSE_DOWN:
	    to_widget = GetNextTraversalWidget(cw, old_focus, wrap);
	    break;
	case XmTRAVERSE_HOME:
	    to_widget = GetFirstTraversalWidget(cw, old_focus, wrap);
	    break;
	default:
	    break;
	} /* switch */

    return (to_widget ? to_widget : old_focus);
}



/************************************************************************
 * GetLastTraversableChild (Private Function)
 ************************************************************************/
static CwidNode
GetLastTraversableChild(
    CwidNode 		node)
{
    CwidNode last_node = NULL;
    CwidNode child_node;
    CwidNode recu_node;

    if (node && (child_node=node->child_ptr))
	{
	while (child_node)
	    {
	    if (NodeIsActive(child_node))
		{
		if (XtIsSensitive(child_node->widget_ptr))
		    last_node = child_node;
		if ((recu_node=GetLastTraversableChild(child_node)))
		    last_node = recu_node;
		}
	    child_node = child_node->next_ptr;
	    }
	}
    return(last_node);
}


/************************************************************************
 * GetPrevTraversableSibling (Private Function)
 ************************************************************************/
static CwidNode
GetPrevTraversableSibling(
    CwidNode 		node)
{
    CwidNode prev_node = NULL;
    CwidNode recu_node;

    if (node && (prev_node=node->prev_ptr))
	{
        while (prev_node)
	    {
	     if (NodeIsActive(prev_node))
		{
		if ((recu_node=GetLastTraversableChild(prev_node)))
		    {
		    prev_node = recu_node;
		    break;
		    }
		if (XtIsSensitive(prev_node->widget_ptr))
		    break;
	    	}
	    prev_node = prev_node->prev_ptr;
	    }
	}
    return(prev_node);
}


/************************************************************************
 * GetPrevTraversableUplevel (Private Function) (Private Function)
 ************************************************************************/
static  CwidNode
GetPrevTraversableUplevel(
        CwidNode	node)
{
    CwidNode 	prev_node = NULL;
    CwidNode 	parent_node;
    CwidNode 	recu_node;

    if (node && (parent_node=node->parent_ptr))
	{
	while (parent_node)
	    {
	    if (NodeIsActive(parent_node))
		{
		if (XtIsSensitive(parent_node->widget_ptr))
		    {
		    prev_node = parent_node;
		    break;
		    }
		if ((recu_node=GetPrevTraversableSibling(parent_node)))
		    {
		    prev_node = recu_node;
		    break;
		    }
		}
	    parent_node = parent_node->parent_ptr;
	    }
	}

    return(prev_node);
}


/************************************************************************
 * GetNextTraversableChild (Private Function)
 ************************************************************************/
static CwidNode
GetNextTraversableChild(
    CwidNode 		node)
{
    CwidNode next_node = NULL;
    CwidNode recu_node;

    if (node && (next_node=node->child_ptr))
	{
        while (next_node)
	    {
	    if (NodeIsActive(next_node))
	    	{
		if (XtIsSensitive(next_node->widget_ptr))
		    break;
		if ((recu_node=GetNextTraversableChild(next_node)))
		    {
		    next_node = recu_node;
		    break;
		    }
		}
	    next_node = next_node->next_ptr;
	    }
	}
    return(next_node);
}


/************************************************************************
 * GetNextTraversableSibling (Private Function)
 ************************************************************************/
static CwidNode
GetNextTraversableSibling(
    CwidNode 		node)
{
    CwidNode next_node = NULL;
    CwidNode recu_node;

    if (node && (next_node=node->next_ptr))
	{
        while (next_node)
	    {
	    if (NodeIsActive(next_node))
		{
		if (XtIsSensitive(next_node->widget_ptr))
		    break;
		if ((recu_node=GetNextTraversableChild(next_node)))
		    {
		    next_node = recu_node;
		    break;
		    }
		}
	    next_node = next_node->next_ptr;
	    }
	}
    return(next_node);
}


/************************************************************************
 * GetNextTraversableUplevel (Private Function)
 ************************************************************************/
static  CwidNode
GetNextTraversableUplevel(
        CwidNode	node)
{
    CwidNode 	next_node = NULL;
    CwidNode 	parent_node;

    if (node && (parent_node=node->parent_ptr))
	{
	while (parent_node && (next_node == NULL))
	    {
	    next_node = GetNextTraversableSibling(parent_node);
	    if (next_node == NULL)
		parent_node = parent_node->parent_ptr;
	    }
	}

    return(next_node);
}


/************************************************************************
 * GetNextTraversable (Private Function)
 ************************************************************************/
static CwidNode
GetNextTraversable(
    CwidNode 		node)
{
    CwidNode next_node = NULL;

    if (node == NULL)
	return(NULL);

    next_node = GetNextTraversableChild(node);

    if (next_node == NULL)
        next_node = GetNextTraversableSibling(node);

    if (next_node == NULL)
        next_node = GetNextTraversableUplevel(node);

    return(next_node);
}


/************************************************************************
 * GetLastTraversalWidget (Private Function)
 ************************************************************************/
static  Widget
GetLastTraversalWidget(
    XmContainerWidget   cw,
    Widget              child,
    Boolean             wrap)
{
    CwidNode    last_node = NULL;
    CwidNode    temp_node;
    CwidNode    child_node;

    if (cw == NULL || child == NULL)
        return(NULL);

    child_node = cw->container.first_node;

    while (child_node && !NodeIsActive(child_node))
        child_node = child_node->next_ptr;

    if (child_node)
        {
	last_node = child_node;
        while ((temp_node=GetNextTraversableSibling(last_node)))
            last_node = temp_node;
        if (last_node && (temp_node=GetLastTraversableChild(last_node)))
            last_node = temp_node;
        if (last_node == NULL && XtIsSensitive(child_node->widget_ptr))
	    last_node = child_node;
        }

    if (last_node && !XmIsTraversable(last_node->widget_ptr))
        last_node = NULL;

    return(last_node ? last_node->widget_ptr : NULL);
}


/************************************************************************
 * GetFirstTraversalWidget (Private Function)
 ************************************************************************/
static  Widget
GetFirstTraversalWidget(
    XmContainerWidget   cw,
    Widget              child,
    Boolean             wrap)
{
    CwidNode    first_node = NULL;
    CwidNode    child_node;

    if (cw == NULL || child == NULL)
        return(NULL);

    child_node = cw->container.first_node;

    while (child_node && !NodeIsActive(child_node))
        child_node = child_node->next_ptr;

    if (child_node)
	{
        if (XtIsSensitive(child_node->widget_ptr))
	    first_node = child_node;
        else
            first_node = GetNextTraversable(child_node);
	}

    if (first_node && !XmIsTraversable(first_node->widget_ptr))
        first_node = NULL;

    return(first_node ? first_node->widget_ptr : NULL);
}


/************************************************************************
 * GetNextTraversalWidget (Private Function)
 ************************************************************************/
static	Widget
GetNextTraversalWidget(
    XmContainerWidget	cw,
    Widget 		child,
    Boolean		wrap)
{
    CwidNode	child_node;
    CwidNode	next_node;

    if (cw == NULL || child == NULL)
	return(NULL);

    child_node = GetContainerConstraint(child)->node_ptr;

    next_node = GetNextTraversable(child_node);

    if (next_node == NULL && wrap)
	return(GetFirstTraversalWidget(cw, child, wrap));

    if (next_node && !XmIsTraversable(next_node->widget_ptr))
        next_node = NULL;

    return(next_node ? next_node->widget_ptr : NULL);
}


/************************************************************************
 * GetPrevTraversalWidget (Private Function)
 ************************************************************************/
static	Widget
GetPrevTraversalWidget(
    XmContainerWidget	cw,
    Widget 		child,
    Boolean		wrap)
{
    CwidNode	child_node;
    CwidNode	prev_node;
    CwidNode	last_node;

    if (cw == NULL || child == NULL)
	return(NULL);

    child_node = GetContainerConstraint(child)->node_ptr;

    prev_node = GetPrevTraversableSibling(child_node);

    if (prev_node == NULL)
	prev_node = GetPrevTraversableUplevel(child_node);

    if (prev_node == NULL && wrap)
	return(GetLastTraversalWidget(cw, child, wrap));

    if (prev_node && !XmIsTraversable(prev_node->widget_ptr))
        prev_node = NULL;

    return(prev_node ? prev_node->widget_ptr : NULL);
}


/************************************************************************
 * MakeOutlineButton (Private Function)
 ************************************************************************/
static	void
MakeOutlineButton(
	Widget	cwid)
{
    XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
    XmContainerConstraint	c = GetContainerConstraint(cwid);
    XmContainerConstraint	obc;
    Pixmap			pm;

    cw->container.self = True;
    if (c->outline_state == XmEXPANDED)
	pm = cw->container.expanded_state_pixmap;
    else
	pm = cw->container.collapsed_state_pixmap;
    cw->container.create_cwid_type = CONTAINER_OUTLINE_BUTTON;
    c->related_cwid = XtVaCreateWidget(OBNAME,
				       	xmPushButtonGadgetClass,(Widget)cw,
				       	XmNlabelType,XmPIXMAP,
				       	XmNlabelPixmap,pm,
                                        XmNtraversalOn,False,
					XmNmarginWidth,0,
					XmNmarginHeight,0,
				       	NULL);
    XtAddCallback(c->related_cwid,XmNactivateCallback,
		  (XtCallbackProc)OutlineButtonCallback,
		  (XtPointer)cwid);
    obc = GetContainerConstraint(c->related_cwid);
    obc->related_cwid = cwid;
    XtManageChild(c->related_cwid);
    cw->container.create_cwid_type = CONTAINER_ICON;
    cw->container.self = False;

#undef NO_PARENT
#undef SWINDOW_PARENT
#undef CWINDOW_PARENT
}

/************************************************************************
 * ChangeOutlineButton (Private Function)
 ************************************************************************/
static	void
ChangeOutlineButtons(
    Widget	wid)
{
    XmContainerWidget		cw = (XmContainerWidget)wid;
    int				i;
    Widget			cwid;
    XmContainerConstraint	c;
    Pixmap			pm;
    Arg				wargs[2];
    int				n;

    for (i = 0; i < cw->composite.num_children; i++)
	{
	cwid = cw->composite.children[i];
	if (CtrOUTLINE_BUTTON(cwid))
	    {
	    c = GetContainerConstraint(cwid);
	    if (c->outline_state == XmEXPANDED)
		pm = cw->container.expanded_state_pixmap;
	    else
		pm = cw->container.collapsed_state_pixmap;
	    n = 0;
	    XtSetArg(wargs[n],XmNlabelPixmap,pm); n++;
	    cw->container.self = True;
	    XtSetValues(cwid,wargs,n);
            cw->container.self = False;
	    }
	}
}


/************************************************************************
 * ExpandCwid (Private Function)
 ************************************************************************/
static	void
ExpandCwid(
    Widget	cwid)
{
    XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
    XmContainerConstraint c = GetContainerConstraint(cwid);
    CwidNode 		node;
    CwidNode 		child_node;
    XtWidgetGeometry geo_desired ;

    if (c->related_cwid == NULL)
	return;
    cw->container.self = True;
    XtVaSetValues(c->related_cwid,
	XmNlabelPixmap,cw->container.expanded_state_pixmap,NULL);
    cw->container.self = False;

    c->outline_state = XmEXPANDED;
    node = c->node_ptr;
    child_node = node->child_ptr;
    if (child_node == NULL)
	return;
    while (child_node)
	{
	c = GetContainerConstraint(child_node->widget_ptr);
	c->visible_in_outline = True;
	child_node = child_node->next_ptr;
	}

    geo_desired.width = 0;
    geo_desired.height = 0;

    RequestOutlineDetail ((Widget)cw, &geo_desired);

    /* need to redraw lines correctly */
    if (XtIsRealized((Widget)cw))
	XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);
}

/************************************************************************
 * CollapseCwid (Private Function)
 ************************************************************************/
static  void
CollapseCwid(
    Widget  cwid)
{
    XmContainerWidget   cw = (XmContainerWidget)XtParent(cwid);
    XmContainerConstraint c = GetContainerConstraint(cwid);
    CwidNode		node;
    CwidNode		child_node;
    XtWidgetGeometry geo_desired ;

    if (c->related_cwid == NULL)
	return;
    cw->container.self = True;
    XtVaSetValues(c->related_cwid,
                XmNlabelPixmap,cw->container.collapsed_state_pixmap,NULL);
    cw->container.self = False;

    c->outline_state = XmCOLLAPSED;
    node = c->node_ptr;
    child_node = node->child_ptr;
    if (child_node == NULL)
	return;
    while (child_node)
	{
        c = GetContainerConstraint(child_node->widget_ptr);
        c->visible_in_outline = False;
	HideCwid(child_node->widget_ptr);
	if (c->related_cwid)
	    HideCwid(c->related_cwid);
	child_node = child_node->next_ptr;
        }

    geo_desired.width = 0;
    geo_desired.height = 0;

    RequestOutlineDetail ((Widget)cw, &geo_desired);

    /* need to redraw lines correctly */
    if (XtIsRealized((Widget)cw))
	XClearArea(XtDisplay((Widget)cw),XtWindow((Widget)cw),0,0,0,0,True);
}

/************************************************************************
 * CallActionCB (Private Function)
 ************************************************************************/
static	void
CallActionCB(
	Widget	cwid,
	XEvent	*event)
{
	XmContainerWidget	cw = (XmContainerWidget)XtParent(cwid);
	XmContainerConstraint	c = GetContainerConstraint(cwid);
	XmContainerSelectCallbackStruct	cbs;

	if (!(XtHasCallbacks((Widget)cw,XmNdefaultActionCallback)
				== XtCallbackHasSome))
		return;
	if (!XtIsSensitive(cwid))
		return;
	cbs.reason = XmCR_DEFAULT_ACTION;
	cbs.event = event;
	if (c->selection_state == XmSELECTED)
		{
		cbs.selected_items = GetSelectedCwids((Widget)cw);
		cbs.selected_item_count = cw->container.selected_item_count;
		}
	else
		{
		cbs.selected_items = (WidgetList)XtMalloc(sizeof(Widget));
		cbs.selected_items[0] = cwid;
		cbs.selected_item_count = 1;
		}
	cbs.auto_selection_type = XmAUTO_UNSET;
	XtCallCallbackList((Widget)cw,cw->container.default_action_cb,&cbs);
	XtFree((char*)cbs.selected_items);
}

/************************************************************************
 * CallSelectCB (Private Function)
 ************************************************************************/
static  void
CallSelectCB(
	Widget		wid,
	XEvent		*event,
	unsigned char	auto_selection_type)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	XmContainerSelectCallbackStruct	cbs;

	if (!(XtHasCallbacks(wid,XmNselectionCallback)
				== XtCallbackHasSome))
		return;
	cbs.selected_items = NULL;
	cbs.selected_item_count = 0;

	switch(cw->container.selection_policy)
	{
	case	XmSINGLE_SELECT:
			cbs.reason = XmCR_SINGLE_SELECT;
	case	XmBROWSE_SELECT:
			if (CtrPolicyIsBROWSE(cw))
				cbs.reason = XmCR_BROWSE_SELECT;
			if (cw->container.anchor_cwid)
				{
				cbs.selected_items = (WidgetList)
						XtMalloc(sizeof(Widget));
        			cbs.selected_items[0] =
					cw->container.anchor_cwid;
				cbs.selected_item_count = 1;
				}
			break;
	case	XmMULTIPLE_SELECT:
			cbs.reason = XmCR_MULTIPLE_SELECT;
	case	XmEXTENDED_SELECT:
			if (CtrPolicyIsEXTENDED(cw))
				cbs.reason = XmCR_EXTENDED_SELECT;
			cbs.selected_items = GetSelectedCwids(wid);
			cbs.selected_item_count =
				cw->container.selected_item_count;
			break;
	}
	cbs.event = event;
	cbs.auto_selection_type = auto_selection_type;
	XtCallCallbackList(wid,cw->container.selection_cb,&cbs);
	if (cbs.selected_items)
		XtFree((char*)cbs.selected_items);
}

/************************************************************************
 * GetSelectedCwids (Private Function)
 ************************************************************************/
static  WidgetList
GetSelectedCwids(
        Widget          wid)
{
        XmContainerWidget       cw = (XmContainerWidget)wid;
	WidgetList		selected_items;
	CwidNode 		node;
	unsigned int		tally = 0;
	XmContainerConstraint	c;

	if (cw->container.selected_item_count == 0)
		return(NULL);

	selected_items = (WidgetList)XtMalloc
			(cw->container.selected_item_count * sizeof(Widget));
	/*
	 * Search through all the visible items first - it'll work 99% of
	 * the time and it's faster than searching through all the items.
	 */
	node = cw->container.first_node;
	while (node)
		{
		c = GetContainerConstraint(node->widget_ptr);
		if (c->selection_visual == XmSELECTED)
			{
			selected_items[tally] = node->widget_ptr;
			tally++;
			if (tally == cw->container.selected_item_count)
				return(selected_items);
			}
		node = GetNextNode(node);
		}
	/*
	 * Didn't work.  There must be some hidden child items that are
	 * XmSELECTED.  We'll have to traverse the entire list.
	 */
	tally = 0;
	node = cw->container.first_node;
	while (node)
		{
		c = GetContainerConstraint(node->widget_ptr);
		if (c->selection_visual == XmSELECTED)
			{
			selected_items[tally] = node->widget_ptr;
			tally++;
			if (tally == cw->container.selected_item_count)
				return(selected_items);
			}
		/*
		 * depth-first search of tree, whether the items are
		 * visible or not.
		 */
		if (node->child_ptr)
			node = node->child_ptr;
		else
			if (node->next_ptr)
				node = node->next_ptr;
			else
				node = GetNextUpLevelNode(node);
		}
	return(NULL);
}

/************************************************************************
 * GetSelectedItems (Private Synthetic Resource Function)
 ************************************************************************/
static	void
GetSelectedItems(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
	WidgetList	selected_items;

	selected_items = GetSelectedCwids(wid);
	*value = (XtArgVal)selected_items;
}

/************************************************************************
 * CompareInts (Private qsort Function)
 ************************************************************************/
static	int
CompareInts(
        XmConst void    *p1,
        XmConst void    *p2)
{
        int     i1 = * (int *)p1;
        int     i2 = * (int *)p2;

	/*
	 * A "qsort" function that simply compares two integers
	 */
        return(i1 - i2);
}

/************************************************************************
 * Isqrt (Private Function)
 ************************************************************************/
static 	int
Isqrt(
    int	n)
{
    /*
     * Integer Square Root (using Newton's Method) - rounded up
     */
    int current_answer,next_trial;

    if (n <= 1)
        return(n);
    current_answer = n;
    next_trial = n/2;
    while(current_answer > next_trial)
        {
        current_answer = next_trial;
        next_trial = (next_trial + n/next_trial)/2;
        }
    if (current_answer * current_answer < n)
        current_answer++;
    return(current_answer);
}

/************************************************************************
 * GainPrimary (Private Function)
 ************************************************************************/
static  void
GainPrimary(
	Widget	wid,
	Time	timestamp)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;

	if (cw->container.primary_ownership == XmOWN_NEVER)
		return;
	if ((cw->container.primary_ownership == XmOWN_POSSIBLE_MULTIPLE) &&
	    (CtrPolicyIsSINGLE(cw) || CtrPolicyIsBROWSE(cw)))
		return;
	if ((cw->container.primary_ownership == XmOWN_MULTIPLE) &&
	    (cw->container.selected_item_count <= 1))
		return;
	if (cw->container.selected_item_count == 0)
		return;
	cw->container.have_primary = XmePrimarySource(wid,timestamp);
}

/************************************************************************
 * ConvertRefuse (Private Function)
 ************************************************************************/
static  void
ConvertRefuse(
        Widget          wid,		/* unused */
        XtPointer       closure,	/* unused */
        XmConvertCallbackStruct *cs)
{
	cs->value = NULL;
	cs->length = 0;
	cs->type = 0;
        cs->status = XmCONVERT_REFUSE;
}

/************************************************************************
 * DragStart (Private Function)
 ************************************************************************/
static	void
DragStart(
    XtPointer	data,
    XtIntervalId *	id)	/* unused */
{
    XmContainerWidget	cw = (XmContainerWidget)data;
    XmGadget		g;
    Arg			wargs[20];
    int			n;
    Pixmap		pixmap = XmUNSPECIFIED_PIXMAP;
    Pixmap		mask = XmUNSPECIFIED_PIXMAP;
    Widget		drag_icon = (Widget) 0;
    unsigned char	vis_state = XmNOT_SELECTED;
    XtPointer		loc_data;
    Widget		dc;
    Widget              diParent = XmGetXmDisplay(XtDisplay(cw));
    DragIconInfo	dragIconInfo = NULL;
    int			offsetx, offsety;
    Pixel		bg, fg;

    cw->container.transfer_timer_id = 0;
    if (cw->container.transfer_action == NULL)
	return;		/* No context record, shouldn't happen */

    cw->container.druggee = ObjectAtPoint((Widget)cw,
			cw->container.transfer_action->event->xbutton.x,
			cw->container.transfer_action->event->xbutton.y);
    /* Handle ObjectAtPoint returning an outline button */
    if ((cw->container.druggee) && CtrOUTLINE_BUTTON(cw->container.druggee))
      cw->container.druggee = NULL;

    if (cw->container.druggee == NULL)
	{
	  XtFree((char*)cw->container.transfer_action->event);
	  XtFree((char*)cw->container.transfer_action);
	  cw->container.transfer_action = NULL;
	  return;
	}
    g = (XmGadget)cw->container.druggee;

    offsetx = cw->container.transfer_action->event->xbutton.x - g->rectangle.x;
    offsety = cw->container.transfer_action->event->xbutton.y - g->rectangle.y;

    cw->container.drag_offset_x = offsetx;
    cw->container.drag_offset_y = offsety;

    /* Get the pixmap if there is one from the IconGadget or other
       Container savvy object */
    n = 0;
    if (GetViewType(cw->container.druggee) == XmSMALL_ICON)
	{
	XtSetArg(wargs[n],XmNsmallIconPixmap,&pixmap); n++;
	XtSetArg(wargs[n],XmNsmallIconMask,&mask); n++;
	}
    else
	{
        XtSetArg(wargs[n],XmNlargeIconPixmap,&pixmap); n++;
	XtSetArg(wargs[n],XmNlargeIconMask,&mask); n++;
	}
    XtGetValues(cw->container.druggee,wargs,n);
    vis_state = GetVisualEmphasis(cw->container.druggee);

    _XmProcessLock();
    if (dragIconInfoContext == 0)
      dragIconInfoContext = XUniqueContext();
    _XmProcessUnlock();

    /* Note that there is one of these per display.  That way we
       can just do a XtSetValues and not recreate the state
       XmDragIcon constantly */
    if (XFindContext(XtDisplay(cw), None, dragIconInfoContext,
		     (XPointer*) &dragIconInfo) == XCNOENT ||
	dragIconInfo == (DragIconInfo) NULL) {
      Pixmap cross;
      Pixmap cross_m;
      GC tempgc;
      Arg args[10];
      int midpoint = DRAG_STATE_SIZE / 2, farpoint = DRAG_STATE_SIZE;

      dragIconInfo = (DragIconInfo) XtMalloc(sizeof(DragIconInfoRec));
      XSaveContext(XtDisplay(cw), None, dragIconInfoContext,
		   (char*) dragIconInfo);
      dragIconInfo -> source = (Widget) NULL;
      cross = XCreatePixmap(XtDisplay(cw),XtWindow(cw),farpoint,farpoint,1);
      cross_m = XCreatePixmap(XtDisplay(cw),XtWindow(cw),farpoint,farpoint,1);
      tempgc = XCreateGC(XtDisplay(cw), cross, 0L, NULL);
      /* Draw a plus sign */
      XSetForeground(XtDisplay(cw), tempgc, 0);
      XFillRectangle(XtDisplay(cw), cross, tempgc, 0, 0, farpoint, farpoint);
      XFillRectangle(XtDisplay(cw), cross_m, tempgc, 0, 0, farpoint, farpoint);
      XSetForeground(XtDisplay(cw), tempgc, 1);
      XSetLineAttributes(XtDisplay(cw), tempgc, 2, LineSolid,
			 CapButt, JoinMiter);
      XDrawLine(XtDisplay(cw), cross, tempgc, midpoint, 0, midpoint, farpoint);
      XDrawLine(XtDisplay(cw), cross, tempgc, 0, midpoint, farpoint, midpoint);
      XSetLineAttributes(XtDisplay(cw), tempgc, 6, LineSolid,
			 CapButt, JoinMiter);
      XDrawLine(XtDisplay(cw),cross_m,tempgc,midpoint,0,midpoint,farpoint);
      XDrawLine(XtDisplay(cw),cross_m,tempgc,0,midpoint,farpoint,midpoint);
      XFreeGC(XtDisplay(cw), tempgc);
      n = 0;
      XtSetArg(args[n], XmNpixmap, cross); n++;
      XtSetArg(args[n], XmNmask, cross_m); n++;
      XtSetArg(args[n], XmNheight, farpoint); n++;
      XtSetArg(args[n], XmNwidth, farpoint); n++;
      dragIconInfo -> state = XmCreateDragIcon(diParent, "stateIcon", args, n);
    }

    n = 0;
    XtSetArg(wargs[n],XmNforeground,&fg); n++;
    XtSetArg(wargs[n],XmNbackground,&bg); n++;
    XtGetValues((Widget) g, wargs, n);

    /* We're dragging from an icon gadget or other object with a
       pixmap obtained above.  We use the pixmap as the source icon.
       The actual XmDragIcon object is cached to avoid constantly
       recreating and destroying the objects,  and this is done on
       a per display basis. */
    n = 0;
    if (pixmap != XmUNSPECIFIED_PIXMAP)
	{
	  unsigned int rh, rw;
	  int rd;
	  int ix, iy;

	  _XmIconGadgetIconPos((Widget) g, &ix, &iy);
	  /* Fix the offset amounts */
	  offsetx -= ix;
	  offsety -= iy;
	  /* Adjust the state icon position */
	  XtSetArg(wargs[n],XmNoffsetX, offsetx); n++;
	  XtSetArg(wargs[n],XmNoffsetY, offsety); n++;
	  XtSetValues(dragIconInfo->state, wargs, n);

	  /* now create or fix up the source icon */
	  XmeGetPixmapData(XtScreen(cw), pixmap,
			   NULL, &rd, NULL, NULL, NULL, NULL, &rw, &rh);

	  n = 0;
	  XtSetArg(wargs[n],XmNpixmap,pixmap); n++;
	  XtSetArg(wargs[n],XmNmask,mask); n++;
	  XtSetArg(wargs[n],XmNheight, rh); n++;
	  XtSetArg(wargs[n],XmNwidth, rw); n++;
	  XtSetArg(wargs[n],XmNdepth, rd); n++;
	  if (dragIconInfo -> source == (Widget) NULL) {
	    drag_icon = XmCreateDragIcon(diParent,"dragIcon",wargs,n);
	    dragIconInfo -> source = drag_icon;
	  } else {
	    drag_icon = dragIconInfo -> source;
	    XtSetValues(drag_icon, wargs, n);
	  }
	  n = 0;
	  XtSetArg(wargs[n],XmNsourcePixmapIcon,drag_icon); n++;
	  XtSetArg(wargs[n],XmNstateCursorIcon,dragIconInfo->state); n++;
	}
    XtSetArg(wargs[n],XmNdragOperations,
	     cw->container.transfer_action->operation); n++;
    if (vis_state == XmSELECTED)
        loc_data = NULL;
    else
        loc_data = (XtPointer)cw->container.druggee;

    XtSetArg(wargs[n],XmNcursorBackground,bg); n++;
    XtSetArg(wargs[n],XmNcursorForeground,fg); n++;
    dc = XmeDragSource((Widget)cw,loc_data,
			cw->container.transfer_action->event,wargs,n);
    if (dc)
        XtAddCallback(dc,XmNdropFinishCallback,DropDoneCallback,cw);
    cw->container.drag_context = (Widget) dc;
    XtFree((char*)cw->container.transfer_action->event);
    XtFree((char*)cw->container.transfer_action);
    cw->container.transfer_action = NULL;
}

/************************************************************************
 * OutlineButtonCallback (Private Callback Function)
 ************************************************************************/
static  void
OutlineButtonCallback(
        Widget          pbwid,	/* unused */
        XtPointer       client_data,
        XtPointer       call_data)
{
    Widget cwid = (Widget)client_data;
    XmAnyCallbackStruct * pbcbs = (XmAnyCallbackStruct *)call_data;
    XmContainerConstraint c = GetContainerConstraint(cwid);

    if (c->outline_state == XmCOLLAPSED)
	OutlineButtonAction(cwid, XmEXPANDED, pbcbs->event);
    else /* if (c->outline_state == XmEXPANDED) */
	OutlineButtonAction(cwid, XmCOLLAPSED, pbcbs->event);
}


/************************************************************************
 * OutlineButtonAction (Private Callback Function)
 ************************************************************************/
static void
OutlineButtonAction(
        Widget          cwid,
	unsigned char	new_state,
        XEvent		*event)
{
    XmContainerWidget cw = (XmContainerWidget)XtParent(cwid);
    XmContainerConstraint c = GetContainerConstraint(cwid);
    XmGadget g = (XmGadget)cwid;
    XmContainerOutlineCallbackStruct cbs;
    unsigned char state_before_callback;

    /*
     * call the XmNoutlineChangedCallback callback
     */
    if (new_state == XmCOLLAPSED)
	cbs.reason = XmCR_COLLAPSED;
    else /* if (new_state == XmEXPANDED) */
	cbs.reason = XmCR_EXPANDED;
    cbs.event = event;
    cbs.item = cwid;
    cbs.new_outline_state = new_state;
    c->outline_state = new_state;

    state_before_callback = c->outline_state;
    XtCallCallbackList((Widget)cw,cw->container.outline_cb,&cbs);

    /* in case user destroys child  */
    if (g->object.being_destroyed)
	return;

    /* verify value returned */
    if ((cbs.new_outline_state != XmCOLLAPSED)
     && (cbs.new_outline_state != XmEXPANDED))
	cbs.new_outline_state = state_before_callback;

    /* in case user has called SetValues in XmNoutlineChangedCallback */
    if (c->outline_state != state_before_callback)
	return;

    /* the user wants to go back to the previous state */
    if (cbs.new_outline_state != state_before_callback)
	{
	c->outline_state = cbs.new_outline_state;
	return;
	}

    if (c->outline_state == XmCOLLAPSED)
	    CollapseCwid(cwid);
    else
	    ExpandCwid(cwid);
}


/************************************************************************
 * MoveItemCallback (Private Callback Function)
 ************************************************************************/

/* Create the multipliers to try the surrounding spots for cell or
   grid mode */
static int x_deltas[] = { 0, -1,  0,  1, -1,  1, -1,  0,  1};
static int y_deltas[] = { 0, -1, -1, -1,  0,  0,  1,  1,  1};

static  void
MoveItemCallback(
        Widget          wid,
        XtPointer       client_data,
        XtPointer       call_data)
{
  XmContainerWidget		cw = (XmContainerWidget)wid;
  XPoint *			dropspot = (XPoint *)client_data;
  XmSelectionCallbackStruct * 	cs =
    (XmSelectionCallbackStruct *)call_data;
  XmContainerConstraint		c;
  unsigned char			save_include_model = XmCLOSEST;
  Widget cwid = cw->container.druggee;
  XmDestinationCallbackStruct *   ds =
    _XmTransferGetDestinationCBStruct(cs->transfer_id);
  XPoint			*offset = (XPoint *) cs -> value;

  if (cwid == NULL)
    return;
  c = GetContainerConstraint(cwid);

  /* Offset the dropspot by the returned data */
  dropspot->x -= offset->x;
  dropspot->y -= offset->y;

  if (((XmContainerWidgetClass)
       XtClass(wid))->container_class.test_fit_item ) {
    XmSpatialTestFitProc test_item;
    Boolean status;

    test_item = (XmSpatialTestFitProc)
      ((XmContainerWidgetClass) XtClass(wid))
	->container_class.test_fit_item;

    if (CtrSpatialStyleIsGRID(wid) || CtrSpatialStyleIsCELLS(wid)) {
      int pos;
      int dw, dh;
      int trial_x = 0, trial_y =0 ;

      if (CtrViewIsLARGE_ICON(wid) || CtrViewIsANY_ICON(wid)) {
	dw = cw -> container.real_large_cellw;
	dh = cw -> container.real_large_cellh;
      } else {
	dw = cw -> container.real_small_cellw;
	dh = cw -> container.real_small_cellh;
      }

      /* Try all the array positions until one is found.  Start
	 at the center and work around left to right,  top to
	 bottom */
      status = False;

      for(pos = 0; pos < 9; pos++) {
	trial_x = dropspot->x + x_deltas[pos] * dw;
	trial_y = dropspot->y + y_deltas[pos] * dh;
	status = test_item(wid, cwid, trial_x, trial_y);
	if (status) break;
      }

      if (status && pos < 9) {
	dropspot->x = trial_x;
	dropspot->y = trial_y;
      }
    } else {
      /* Just try the direct spot */
      status = test_item(wid,cwid,dropspot->x,dropspot->y);
    }

    if (! status) {
      XmTransferDone(cs->transfer_id,XmTRANSFER_DONE_FAIL);
      return;
    }
  }
  if (((XmContainerWidgetClass)
       XtClass(wid))->container_class.remove_item)
    (*((XmContainerWidgetClass)
       XtClass(wid))->container_class.remove_item)
      (wid,cwid);

  /* Clear old placement */
  XClearArea(XtDisplay(wid),XtWindow(wid),
	     cwid->core.x,cwid->core.y,
	     cwid->core.width,cwid->core.height, True);

  if (CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw))
    /*
     * Fake the include model so it pays attention to core.x & core.y
     */
    {
      save_include_model = cw->container.include_model;
      cw->container.include_model = XmCLOSEST;
    }

  XmeConfigureObject(cwid, dropspot->x, dropspot->y,
		     cwid->core.width, cwid->core.height,
		     cwid->core.border_width);

  c->user_x = dropspot->x;
  c->user_y = dropspot->y;

  if (((XmContainerWidgetClass)
       XtClass(wid))->container_class.place_item)
    (*((XmContainerWidgetClass)
       XtClass(wid))->container_class.place_item)
      (wid,cwid,FORCE);
  if (CtrSpatialStyleIsGRID(cw) || CtrSpatialStyleIsCELLS(cw))
    cw->container.include_model = save_include_model;
  /* If we've gotten down here,  and we came from the
     same widget, and we've succeeded,  so cancel
     the drop effect,  moving is enough (and faster) */
  if (ds -> flags & XmCONVERTING_SAME)
    XmTransferValue(cs->transfer_id,
		    XInternAtom(XtDisplay(wid),
				XmS_MOTIF_CANCEL_DROP_EFFECT, False),
		    (XtCallbackProc)NULL,NULL,
		    XtLastTimestampProcessed(XtDisplay(wid)));
}

/************************************************************************
 * DropDoneCallback (Private Callback Function)
 ************************************************************************/
static  void
DropDoneCallback(
        Widget          wid,		/* unused */
        XtPointer       client_data,
        XtPointer       call_data)	/* unused */
{
        XmContainerWidget       cw = (XmContainerWidget)client_data;

	cw->container.drag_context = (Widget) NULL;
}


/************************************************************************
 *									*
 * EnterHandler - If there is a drag timeout, remove it.		*
 *									*
 ************************************************************************/

static void
EnterHandler(
	Widget wid,
        XtPointer closure,
    	XEvent *event,
        Boolean	*continue_to_dispatch)
{
    XmContainerWidget       cw = (XmContainerWidget)wid;

    cw->container.LeaveDir = 0;
    if (cw->container.scroll_proc_id) {
	XtRemoveTimeOut(cw->container.scroll_proc_id);
	cw->container.scroll_proc_id = 0;
    }
}

/************************************************************************
 *									*
 * LeaveHandler - If the user leaves in Marquee Select mode set up a    *
 *	          timer to scroll the container.			*
 *									*
 ************************************************************************/

static void
LeaveHandler(
	Widget wid,
        XtPointer closure,
    	XEvent *event,
        Boolean	*continue_to_dispatch)
{
    XmContainerWidget       cw = (XmContainerWidget)wid;
    Widget                  clip = XtParent(wid);
    int rx, ry;			/* event coords relative to the clip */
    int interval = 200;

    if (!cw->container.selecting || CtrPolicyIsSINGLE(cw))
	return;

    /* first lets see which direction we left the window */
    cw->container.LeaveDir = 0;
    rx = event->xcrossing.x + (int)wid->core.x;
    ry = event->xcrossing.y + (int)wid->core.y;
    if (rx <= (int)clip->core.x)
	cw->container.LeaveDir |= LEFTLEAVE;
    else if (rx >= (int)clip->core.width)
	cw->container.LeaveDir |= RIGHTLEAVE;
    if (ry <= (int)clip->core.y)
	cw->container.LeaveDir |= TOPLEAVE;
    else if (ry >= (int)clip->core.height)
	cw->container.LeaveDir |= BOTTOMLEAVE;

    /* then add a TimeOutProc to handle the auto scroll */
    cw->container.scroll_proc_id =
	XtAppAddTimeOut(XtWidgetToApplicationContext(wid),
			(unsigned long) interval, ScrollProc, (XtPointer) cw);
}

/************************************************************************
 *									*
 * ScrollProc - timer proc that scrolls the container if the user has   *
 *		left the window with the button down. If the button has *
 *		been released, call the standard click stuff.		*
 *									*
 ************************************************************************/

static void
ScrollProc(
	XtPointer closure,
	XtIntervalId *id)
{
    Widget                  wid = (Widget) closure;
    XmContainerWidget       cw = (XmContainerWidget)closure;
    Widget sf;
    XmScrollFrameTrait scrollFrameTrait;
    XmNavigatorTrait navigatorTrait;
    XmNavigatorDataRec nav_data;
    Cardinal i, num_nav_list;
    Widget *nav;
    int interval = 100;
    XEvent event;
    Boolean selection_changes;

    if (cw->container.scroll_proc_id == 0)
	return;

    cw->container.scroll_proc_id = 0;

    /* since we've got reparented by the clip the scrollFrame is 2 levels up */
    sf = XtParent(XtParent((Widget)cw));
    scrollFrameTrait = (XmScrollFrameTrait)
	XmeTraitGet((XtPointer)XtClass(sf), XmQTscrollFrame);
    if (!(scrollFrameTrait &&
	  (scrollFrameTrait->getInfo(sf,NULL,&nav,&num_nav_list))))
	return;			/* this should never happen */

    for (i=0; i < num_nav_list; i++, nav++) {
	navigatorTrait = (XmNavigatorTrait)
	    XmeTraitGet((XtPointer)XtClass(*nav), XmQTnavigator);
	/* get the current position */
	nav_data.valueMask =
	    NavValue | NavIncrement | NavMinimum | NavMaximum | NavSliderSize;
	bzero(&nav_data,sizeof(XmNavigatorDataRec));
	navigatorTrait->getValue(*nav, &nav_data);

	/* compute the new position */
	if (cw->container.LeaveDir & BOTTOMLEAVE)
	    nav_data.value.y += nav_data.increment.y;
	else if (cw->container.LeaveDir & TOPLEAVE)
	    nav_data.value.y -= nav_data.increment.y;
	if (cw->container.LeaveDir & LEFTLEAVE)
	    nav_data.value.x -= nav_data.increment.x;
	else if (cw->container.LeaveDir & RIGHTLEAVE)
	    nav_data.value.x += nav_data.increment.x;

	if (nav_data.value.y < nav_data.minimum.y)
	    nav_data.value.y = nav_data.minimum.y;
	if (nav_data.value.y > (nav_data.maximum.y - nav_data.slider_size.y))
	    nav_data.value.y = nav_data.maximum.y - nav_data.slider_size.y;
	if (nav_data.value.x < nav_data.minimum.x)
	    nav_data.value.x = nav_data.minimum.x;
	if (nav_data.value.x > (nav_data.maximum.x - nav_data.slider_size.x))
	    nav_data.value.x = nav_data.maximum.x - nav_data.slider_size.x;

	/* and set it  */
	nav_data.valueMask = NavValue;
        navigatorTrait->setValue(*nav, &nav_data, True);
    }

    /* fake a button event using the last event coordinates */
    event.xbutton.x = cw->container.last_xmotion_x - (int)wid->core.x;
    event.xbutton.y = cw->container.last_xmotion_y - (int)wid->core.y;
    selection_changes = ProcessButtonMotion(wid, &event, NULL, 0);
    cw->container.no_auto_sel_changes |= selection_changes;
    if (CtrIsAUTO_SELECT(cw) && selection_changes)
	CallSelectCB(wid, NULL, XmAUTO_MOTION);

    /* reset timer proc */
    cw->container.scroll_proc_id =
	XtAppAddTimeOut(XtWidgetToApplicationContext(wid),
			(unsigned long) interval, ScrollProc, (XtPointer) cw);
}

/************************************************************************
 * XmContainerCut (Public Function)
 ************************************************************************/
Boolean
XmContainerCut(
	Widget	wid,
	Time	timestamp)
{
	XmContainerWidget       cw = (XmContainerWidget)wid;
	Boolean			status;

	_XmWidgetToAppContext(wid);
	_XmAppLock(app);

	if (cw->container.selected_item_count == 0) {
		_XmAppUnlock(app);
                return(False);
	}
        if (!cw->container.have_primary) {
		_XmAppUnlock(app);
                return(False);
	}
	status = XmeClipboardSource(wid,XmMOVE,timestamp);
	_XmAppUnlock(app);
	return(status);
}

/************************************************************************
 * XmContainerCopy (Public Function)
 ************************************************************************/
Boolean
XmContainerCopy(
        Widget  wid,
        Time    timestamp)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
        Boolean         	status;

	_XmWidgetToAppContext(wid);
	_XmAppLock(app);

	if (cw->container.selected_item_count == 0) {
		_XmAppUnlock(app);
		return(False);
	}
	if (!cw->container.have_primary) {
		_XmAppUnlock(app);
		return(False);
	}
        status = XmeClipboardSource(wid,XmCOPY,timestamp);
	_XmAppUnlock(app);
        return(status);
}

/************************************************************************
 * XmContainerPaste (Public Function)
 ************************************************************************/
Boolean
XmContainerPaste(
	Widget	wid)
{
	return(XmeClipboardSink(wid,XmCOPY,NULL));
}

/************************************************************************
 * XmContainerCopyLink (Public Function)
 ************************************************************************/
Boolean
XmContainerCopyLink(
        Widget  wid,
        Time    timestamp)
{
        XmContainerWidget       cw = (XmContainerWidget)wid;
        Boolean         	status;

	_XmWidgetToAppContext(wid);
	_XmAppLock(app);

	if (cw->container.selected_item_count == 0) {
		_XmAppUnlock(app);
                return(False);
	}
        if (!cw->container.have_primary) {
		_XmAppUnlock(app);
                return(False);
	}
        status = XmeClipboardSource(wid,XmLINK,timestamp);
	_XmAppUnlock(app);
        return(status);
}

/************************************************************************
 * XmContainerPasteLink (Public Function)
 ************************************************************************/
Boolean
XmContainerPasteLink(
	Widget  wid)
{
	return(XmeClipboardSink(wid,XmLINK,NULL));
}

/************************************************************************
 * XmContainerGetItemChildren (Public Function)
 ************************************************************************/
int
XmContainerGetItemChildren(
	Widget		wid,
	Widget		item,
	WidgetList	*item_children)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	XmContainerConstraint	c;
	CwidNode	node;
	CwidNode	first_child_node;
	WidgetList	clist;
	int		i,clist_count;

	_XmWidgetToAppContext(wid);

	_XmAppLock(app);
	if (item == NULL)
		{
		if (cw->container.first_node == NULL)
		    {
			_XmAppUnlock(app);
			return(0);
		    }
		first_child_node = cw->container.first_node;
		}
	else
		{
		if (XtParent(item) != wid)
		   {
			_XmAppUnlock(app);
			return(0);
		   }
		c = GetContainerConstraint(item);
		node = c->node_ptr;
		if (node->child_ptr == NULL)
		   {
			_XmAppUnlock(app);
			return(0);
		   }
		first_child_node = node->child_ptr;
		}
	clist_count = 1;
	node = first_child_node;
	while (node->next_ptr)
		{
		clist_count++;
		node = node->next_ptr;
		}
	clist = (WidgetList)XtMalloc(clist_count * sizeof(Widget));
	node = first_child_node;
	for (i = 0; i < clist_count; i++)
		{
		clist[i] = node->widget_ptr;
		node = node->next_ptr;
		}
	*item_children = clist;
	_XmAppUnlock(app);
	return(clist_count);
}

/************************************************************************
 *  XmContainerRelayout (Public Function)
 ************************************************************************/
void
XmContainerRelayout(
    Widget	wid)
{
    XmContainerWidget	cw = (XmContainerWidget)wid;
    _XmWidgetToAppContext(wid);

    if (!XtIsRealized(wid))
      /* Don't need to relayout,  will be right anyway */
      return;

    _XmAppLock(app);
    if (CtrLayoutIsOUTLINE_DETAIL(cw)) {
	_XmAppUnlock(app);
	return;
    }
    if (CtrSpatialStyleIsNONE(cw)) {
	_XmAppUnlock(app);
	return;
    }
    if (!((XmContainerWidgetClass)XtClass(wid))->container_class.place_item) {
	_XmAppUnlock(app);
	return;
    }

    /* Reset grid/cell information */
    (*((XmContainerWidgetClass)XtClass(wid))->container_class.place_item)
           					(wid,NULL,ANY_FIT);
    /* Relayout - no geometry changes */
    LayoutSpatial(wid,False,NULL);

    if (XtIsRealized(wid))
	XClearArea(XtDisplay(wid),XtWindow(wid),0,0,0,0,True);
    _XmAppUnlock(app);
}

/************************************************************************
 *  XmContainerReorder (Public Function)
 ************************************************************************/
void
XmContainerReorder(
	Widget		wid,
	WidgetList	cwid_list,
	int		cwid_count)
{
	XmContainerWidget	cw = (XmContainerWidget)wid;
	XmContainerConstraint	c;
	Widget			pcwid;
	int *			pi_list;
	int			i, pi_count;

	_XmWidgetToAppContext(wid);

	if (cwid_count <= 1)
		return;
	_XmAppLock(app);
	c = GetContainerConstraint(cwid_list[0]);
	pcwid = c->entry_parent;
	pi_list = (int *)XtMalloc(cwid_count * sizeof(int));
	pi_count = 0;
	for (i=0; i < cwid_count; i++)
		{
		c = GetContainerConstraint(cwid_list[i]);
		if (c->entry_parent == pcwid)
			{
			pi_list[pi_count] = c->position_index;
			pi_count++;
			}
		}
	qsort(pi_list,pi_count,sizeof(int),CompareInts);
	pi_count = 0;
	for (i=0; i < cwid_count; i++)
		{
		c = GetContainerConstraint(cwid_list[i]);
		if (c->entry_parent == pcwid)
			{
			c->position_index = pi_list[pi_count];
			pi_count++;
			SeverNode(c->node_ptr);
			ContainerResequenceNodes(cw, c->entry_parent);
			InsertNode(c->node_ptr);
			}
		}
	XtFree((char*)pi_list);

	/*
	 * Outline & Detail Layouts must always remain in order.
	 */
	if (CtrLayoutIsOUTLINE_DETAIL(cw))
		Layout(wid);
	/*
	 * Outline lines may have changed location, erase the old ones.
	 */
	if (CtrDrawLinesOUTLINE(cw) && XtIsRealized((Widget)cw))
	     XClearArea(XtDisplay(wid),XtWindow(wid),0,0,0,0,True);
	_XmAppUnlock(app);
}

/************************************************************************
 *  XmCreateContainer (Public Function)
 ************************************************************************/
Widget
XmCreateContainer(
        Widget          parent,
        String		name,
        ArgList         arglist,
        Cardinal        argcount)
{
	/*
	 * Create an instance of a container and return the widget id.
	 */
   return(XtCreateWidget(name,xmContainerWidgetClass,parent,arglist,argcount));
}

Widget
XmVaCreateContainer(
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
                         xmContainerWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedContainer(
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
                         xmContainerWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

