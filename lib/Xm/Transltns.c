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
static char rcsid[] = "$TOG: Transltns.c /main/24 1999/08/09 18:34:51 mgreess $"
#endif
#endif

#include <Xm/XmP.h>
#include <Xm/TransltnsP.h>

/* This is the new-style translation table, which is used with
 * versions of libXt that have the :-production translation fix
 * (fix-trackers seq 2797, or MIT public patch 24).
 *
 * Translations have been cleaned up and reorganized as follows:
 *  - Sort translations based roughly on the event type.  Canonical
 *	order is: Map/Unmap, Enter/Leave, Focus, Btn, Key.
 *  - Within the <Key> translations group by key, putting osfMumble
 *	keys first.
 *  - Prefix osf key translations with ':', and reorder as necessary
 *	to remove ~mod.
 */


/*** ArrowB.c ***/
externaldef(translations) const char _XmArrowB_defaultTranslations[] = "\
<EnterWindow>:			Enter()\n\
<LeaveWindow>:			Leave()\n\
c<Btn1Down>:			ButtonTakeFocus()\n\
~c<Btn1Down>:			Arm()\n\
~c<Btn1Down>,~c<Btn1Up>:	Activate() Disarm()\n\
~c<Btn1Down>(2+):		MultiArm()\n\
~c<Btn1Up>(2+):			MultiActivate()\n\
~c<Btn1Up>:			Activate() Disarm()\n\
:<Key>osfActivate:		PrimitiveParentActivate()\n\
:<Key>osfCancel:		PrimitiveParentCancel()\n\
:<Key>osfSelect:		ArmAndActivate()\n\
:<Key>osfHelp:			Help()\n\
~s ~m ~a <Key>Return:		PrimitiveParentActivate()\n\
~s ~m ~a <Key>space:		ArmAndActivate()";

/*** BulletinB.c ***/
externaldef(translations) const char _XmBulletinB_defaultTranslations[] = "\
<BtnMotion>:			ManagerGadgetButtonMotion()\n\
c<Btn1Down>:			ManagerGadgetTraverseCurrent()\n\
~c<Btn1Down>:			ManagerGadgetArm()\n\
~c<Btn1Down>,~c<Btn1Up>:	ManagerGadgetActivate()\n\
~c<Btn1Up>:			ManagerGadgetActivate()\n\
~c<Btn1Down>(2+):		ManagerGadgetMultiArm()\n\
~c<Btn1Up>(2+):			ManagerGadgetMultiActivate()\n\
<Btn2Down>:			ManagerGadgetDrag()\n\
:<Key>osfHelp:			ManagerGadgetHelp()\n\
:<Key>osfActivate:		ManagerParentActivate()\n\
:<Key>osfCancel:		ManagerParentCancel()\n\
:<Key>osfSelect:		ManagerGadgetSelect()\n\
<Key>space:			ManagerGadgetSelect()\n\
<Key>Return:			ManagerParentActivate()\n\
<Key>:				ManagerGadgetKeyInput()";

/*** CascadeB.c ***/
externaldef(translations) const char _XmCascadeB_menubar_events[] = "\
<EnterWindow>Normal:	MenuBarEnter()\n\
<LeaveWindow>Normal:	MenuBarLeave()\n\
<Btn2Down>:		ProcessDrag()\n\
c<Btn2Up>:		DoSelect()\n\
c<BtnDown>:		MenuButtonTakeFocusUp()\n\
c<BtnUp>:		MenuButtonTakeFocusUp()\n\
~c<BtnDown>:		MenuBarSelect()\n\
~c<BtnUp>:		DoSelect()\n\
:<Key>osfSelect:	KeySelect()\n\
:<Key>osfActivate:	KeySelect()\n\
:<Key>osfHelp:		Help()\n\
:<Key>osfCancel:	CleanupMenuBar()\n\
~s<Key>Return:		KeySelect()\n\
~s<Key>space:		KeySelect()";

externaldef(translations) const char _XmCascadeB_p_events[] = "\
<EnterWindow>:		DelayedArm()\n\
<LeaveWindow>:		CheckDisarm()\n\
<Btn2Down>:		ProcessDrag()\n\
c<Btn2Up>:		DoSelect()\n\
c<BtnDown>:		MenuButtonTakeFocus()\n\
c<BtnUp>:		MenuButtonTakeFocusUp()\n\
~c<BtnDown>:		StartDrag()\n\
~c<BtnUp>:		DoSelect()\n\
:<Key>osfSelect:	KeySelect()\n\
:<Key>osfActivate:	KeySelect()\n\
:<Key>osfHelp:		Help()\n\
:<Key>osfCancel:	CleanupMenuBar()\n\
~s<Key>Return:		KeySelect()\n\
~s<Key>space:		KeySelect()";


/*** Display.c ***/
/*
 * Although adding Ctrl<Key>s as a binding for osfCancel would
 * simplify this table, it would break applications with translations
 * for that event.  Instead we duplicate the CDE1.0 bindings, with
 * additions for the new widgets.
 */
externaldef(translations) const char _XmDisplay_baseTranslations[] = "\
*XmArrowButton.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmBulletinBoard.baseTranslations:\
    \043override\
	c<Key>s:	ManagerParentCancel()\n\
*XmCascadeButton.baseTranslations:\
    \043override\
	c<Key>s:	CleanupMenuBar()\n\
*XmComboBox*baseTranslations:\
    \043override\
	c<Key>s:	CBCancel()\n\
*XmContainer.baseTranslations:\
    \043override\
	c<Key>s:	ContainerCancel()\n\
*XmDragContext.baseTranslations:\
    \043override\
	c<Key>s:	CancelDrag()\n\
*XmDrawingArea.baseTranslations:\
    \043override\
	c<Key>s:	DrawingAreaInput() ManagerParentCancel()\n\
*XmDrawnButton.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmFrame.baseTranslations:\
    \043override\
	c<Key>s:	ManagerParentCancel()\n\
*XmLabel.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmList.baseTranslations:\
    \043override\
	c<Key>s:	ListKbdCancel()\n\
*XmManager.baseTranslations:\
    \043override\
	c<Key>s:	ManagerParentCancel()\n\
*XmMenuShell.baseTranslations:\
    \043override\
	c<Key>s:	MenuEscape()\n\
*XmPrimitive.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmPushButton.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmRowColumn.baseTranslations:\
    \043override\
	c<Key>s:	ManagerParentCancel()\n\
*XmSash.baseTranslations:\
    \043override\
	c<Key>s:	PrimitiveParentCancel()\n\
*XmScrollBar.baseTranslations:\
    \043override\
	c<Key>s:	CancelDrag()\n\
*XmScrolledWindow.baseTranslations:\
    \043override\
	c<Key>s:	ManagerParentCancel()\n\
*XmTextField.baseTranslations:\
    \043override\
	c<Key>s:	process-cancel()\\n\
	c<Key>x:	cut-clipboard()\\n\
	c<Key>c:	copy-clipboard()\\n\
	c<Key>v:	paste-clipboard()\n\
*XmText.baseTranslations:\
\043override\
	c<Key>s:	process-cancel()\\n\
	c<Key>x:	cut-clipboard()\\n\
	c<Key>c:	copy-clipboard()\\n\
	c<Key>v:	paste-clipboard()\n\
*XmToggleButton.baseTranslations:\
\043override\
	c<Key>s:	PrimitiveParentCancel()\n\
";


/*** DragC.c ***/
externaldef(translations) const char _XmDragC_defaultTranslations[] = "\
Button1<Enter>:		DragMotion()\n\
Button1<Leave>:		DragMotion()\n\
Button1<Motion>:	DragMotion()\n\
Button2<Enter>:		DragMotion()\n\
Button2<Leave>:		DragMotion()\n\
Button2<Motion>:	DragMotion()\n\
<Btn2Up>:		FinishDrag()\n\
<Btn1Up>:		FinishDrag()\n\
<BtnDown>:		IgnoreButtons()\n\
<BtnUp>:		IgnoreButtons()\n\
<Key>Return:		FinishDrag()\n\
:<Key>osfActivate:	FinishDrag()\n\
:<Key>osfCancel:	CancelDrag()\n\
:<Key>osfHelp:		HelpDrag()\n\
:<Key>osfUp:		DragKey(Up)\n\
:<Key>osfDown:		DragKey(Down)\n\
:<Key>osfLeft:		DragKey(Left)\n\
:<Key>osfRight:		DragKey(Right)\n\
:<KeyUp>:		DragKey(Update)\n\
:<KeyDown>:		DragKey(Update)";

/*** DrawingA.c ***/
externaldef(translations) const char _XmDrawingA_defaultTranslations[] = "\
<BtnMotion>:		ManagerGadgetButtonMotion()\n\
c<Btn1Down>:		ManagerGadgetTraverseCurrent()\n\
~c<Btn1Down>:		DrawingAreaInput() ManagerGadgetArm()\n\
~c<Btn1Down>,~c<Btn1Up>:DrawingAreaInput() ManagerGadgetActivate()\n\
~c<Btn1Up>:		DrawingAreaInput() ManagerGadgetActivate()\n\
~c<Btn1Down>(2+):	DrawingAreaInput() ManagerGadgetMultiArm()\n\
~c<Btn1Up>(2+):		DrawingAreaInput() ManagerGadgetMultiActivate()\n\
<Btn2Down>:		DrawingAreaInput() ManagerGadgetDrag()\n\
<BtnDown>:		DrawingAreaInput()\n\
<BtnUp>:		DrawingAreaInput()\n\
:<Key>osfActivate:	DrawingAreaInput() ManagerParentActivate()\n\
:<Key>osfCancel:	DrawingAreaInput() ManagerParentCancel()\n\
:<Key>osfHelp:		DrawingAreaInput() ManagerGadgetHelp()\n\
:<Key>osfSelect:	DrawingAreaInput() ManagerGadgetSelect()\n\
~s ~m ~a <Key>Return:	DrawingAreaInput() ManagerParentActivate()\n\
<Key>Return:		DrawingAreaInput() ManagerGadgetSelect()\n\
<Key>space:		DrawingAreaInput() ManagerGadgetSelect()\n\
<KeyDown>:		DrawingAreaInput() ManagerGadgetKeyInput()\n\
<KeyUp>:		DrawingAreaInput()";

externaldef(translations) const char _XmDrawingA_traversalTranslations[] = "\
<EnterWindow>:		ManagerEnter()\n\
<LeaveWindow>:		ManagerLeave()\n\
<FocusOut>:		ManagerFocusOut()\n\
<FocusIn>:		ManagerFocusIn()\n\
:<Key>osfUp:		DrawingAreaInput() ManagerGadgetTraverseUp()\n\
:<Key>osfDown:		DrawingAreaInput() ManagerGadgetTraverseDown()\n\
:<Key>osfLeft:		DrawingAreaInput() ManagerGadgetTraverseLeft()\n\
:<Key>osfRight:		DrawingAreaInput() ManagerGadgetTraverseRight()\n\
:<Key>osfBeginLine:	DrawingAreaInput() ManagerGadgetTraverseHome()\n\
s<Key>Tab:		DrawingAreaInput() ManagerGadgetPrevTabGroup()\n\
~s<Key>Tab:		DrawingAreaInput() ManagerGadgetNextTabGroup()";

/*** DrawnB.c ***/
externaldef(translations) const char _XmDrawnB_defaultTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
c<Btn1Down>:		ButtonTakeFocus()\n\
~c<Btn1Down>:		Arm()\n\
~c<Btn1Down>,~c<Btn1Up>:Activate() Disarm()\n\
~c<Btn1Down>(2+):	MultiArm()\n\
~c<Btn1Up>(2+):		MultiActivate()\n\
~c<Btn1Up>:		Activate() Disarm()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfSelect:	ArmAndActivate()\n\
:<Key>osfHelp:		Help()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

externaldef(translations) const char _XmDrawnB_menuTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
c<Btn1Down>:		MenuButtonTakeFocus()\n\
c<Btn1Up>:		MenuButtonTakeFocusUp()\n\
~c<BtnDown>:		BtnDown()\n\
~c<BtnUp>:		BtnUp()\n\
:<Key>osfSelect:	ArmAndActivate()\n\
:<Key>osfActivate:	ArmAndActivate()\n\
:<Key>osfCancel:	MenuEscape()\n\
:<Key>osfHelp:		Help()\n\
~s ~m ~a <Key>Return:	ArmAndActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

/*** Frame.c ***/
/*
 * XmFrame has inherited XmManager's translations since OSF/Motif 1.1.
 * Since binary compatibility is *not* transitive (it is only
 * promised for one major release) we can reclaim this storage now.
 */
externaldef(translations) const char _XmFrame_defaultTranslations[] = "";

/*** Label.c ***/
externaldef(translations) const char _XmLabel_defaultTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
<Btn2Down>:		ProcessDrag()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfHelp:		Help()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()";

externaldef(translations) const char _XmLabel_menuTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
<Btn2Down>:		ProcessDrag()\n\
:<Key>osfHelp:		Help()";

externaldef(translations) const char _XmLabel_menu_traversal_events[] = "\
<Unmap>:		Unmap()\n\
<FocusOut>:		FocusOut()\n\
<FocusIn>:		FocusIn()\n\
:<Key>osfCancel:	MenuEscape()\n\
:<Key>osfLeft:		MenuTraverseLeft()\n\
:<Key>osfRight:		MenuTraverseRight()\n\
:<Key>osfUp:		MenuTraverseUp()\n\
:<Key>osfDown:		MenuTraverseDown()";

/*** List.c ***/
externaldef(translations) const char _XmList_ListXlations1[] = "\
<Unmap>:			PrimitiveUnmap()\n\
<Enter>:			ListEnter()\n\
<Leave>:			ListLeave()\n\
<FocusIn>:			ListFocusIn()\n\
<FocusOut>:			ListFocusOut()\n\
<Btn1Motion>:			ListProcessBtn1(ListButtonMotion)\n\
s ~m ~a <Btn1Down>:		ListProcessBtn1(ListBeginExtend)\n\
s ~m ~a <Btn1Up>:		ListProcessBtn1(ListEndExtend)\n\
~c ~s ~m ~a <Btn1Down>:		ListProcessBtn1(ListBeginSelect)\n\
~c ~s ~m ~a <Btn1Up>:		ListProcessBtn1(ListEndSelect)\n\
c ~s ~m ~a <Btn1Down>:		ListProcessBtn1(ListBeginToggle)\n\
c ~s ~m ~a <Btn1Up>:		ListProcessBtn1(ListEndToggle)\n\
c ~s ~m a <Btn1Down>:		ListProcessDrag()\n\
~c s ~m a <Btn1Down>:		ListProcessDrag()\n\
<Btn2Down>:			ListProcessBtn2(ListBeginExtend)\n\
<Btn2Motion>:			ListProcessBtn2(ListButtonMotion)\n\
<Btn2Up>:			ListProcessBtn2(ListEndExtend)\n\
:s c <Key>osfBeginLine:		ListBeginDataExtend()\n\
:c <Key>osfBeginLine:		ListBeginData()\n\
:<Key>osfBeginLine:		ListBeginLine()\n\
:s c <Key>osfEndLine:		ListEndDataExtend()\n\
:c <Key>osfEndLine:		ListEndData()\n\
:<Key>osfEndLine:		ListEndLine()\n\
:<Key>osfPageLeft:		ListLeftPage()\n\
:c <Key>osfPageUp:		ListLeftPage()\n\
:<Key>osfPageUp:		ListPrevPage()\n\
:<Key>osfPageRight:		ListRightPage()\n\
:c <Key>osfPageDown:		ListRightPage()\n\
:<Key>osfPageDown:		ListNextPage()\n";

externaldef(translations) const char _XmList_ListXlations2[] = "\
:s <KeyDown>osfSelect:		ListKbdBeginExtend()\n\
:<KeyDown>osfSelect:		ListKbdBeginSelect()\n\
:s <KeyUp>osfSelect:		ListKbdEndExtend()\n\
:<KeyUp>osfSelect:		ListKbdEndSelect()\n\
:<Key>osfSelectAll:		ListKbdSelectAll()\n\
:<Key>osfDeselectAll:		ListKbdDeSelectAll()\n\
:<Key>osfActivate:		ListKbdActivate()\n\
:<Key>osfAddMode:		ListAddMode()\n\
:<Key>osfHelp:			PrimitiveHelp()\n\
:<Key>osfCancel:		ListKbdCancel()\n\
:c <Key>osfLeft:		ListLeftPage()\n\
:<Key>osfLeft:			ListLeftChar()\n\
:c <Key>osfRight:		ListRightPage()\n\
:<Key>osfRight:			ListRightChar()\n\
:s <Key>osfUp:			ListExtendPrevItem()\n\
:<Key>osfUp:			ListPrevItem()\n\
:s <Key>osfDown:		ListExtendNextItem()\n\
:<Key>osfDown:			ListNextItem()\n\
:c <Key>osfInsert:		ListCopyToClipboard()\n\
:<Key>osfCopy:			ListCopyToClipboard()\n\
~s c ~m ~a <Key>slash:		ListKbdSelectAll()\n\
~s c ~m ~a <Key>backslash:	ListKbdDeSelectAll()\n\
s ~m ~a <Key>Tab:		PrimitivePrevTabGroup()\n\
~m ~a <Key>Tab:			PrimitiveNextTabGroup()\n\
~s ~m ~a <Key>Return:		ListKbdActivate()\n\
~s ~m ~a <KeyDown>space:	ListKbdBeginSelect()\n\
~s ~m ~a <KeyUp>space:		ListKbdEndSelect()\n\
s ~m ~a <KeyDown>space:		ListKbdBeginExtend()\n\
s ~m ~a <KeyUp>space:		ListKbdEndExtend()\n\
<Key>:				ListQuickNavigate()\n\
~s ~c ~m ~a <Btn4Down>,<Btn4Up>:ListScrollUp() ListScrollUp() ListScrollUp()\n\
~s ~c ~m ~a <Btn5Down>,<Btn5Up>:ListScrollDown() ListScrollDown() ListScrollDown()\n\
<Btn4Down>,<Btn4Up>:		ListScrollUp() ListScrollUp() ListScrollUp() ListScrollUp() ListScrollUp() ListScrollUp()\n\
<Btn5Down>,<Btn5Up>:		ListScrollDown() ListScrollDown() ListScrollDown() ListScrollDown() ListScrollDown() ListScrollDown()";

/*** Manager.c ***/
externaldef(translations)
const char _XmManager_managerTraversalTranslations[] = "\
<EnterWindow>:			ManagerEnter()\n\
<LeaveWindow>:			ManagerLeave()\n\
<FocusOut>:			ManagerFocusOut()\n\
<FocusIn>:			ManagerFocusIn()\n\
:<Key>osfBeginLine:		ManagerGadgetTraverseHome()\n\
:<Key>osfUp:			ManagerGadgetTraverseUp()\n\
:<Key>osfDown:			ManagerGadgetTraverseDown()\n\
:<Key>osfLeft:			ManagerGadgetTraverseLeft()\n\
:<Key>osfRight:			ManagerGadgetTraverseRight()\n\
s ~m ~a <Key>Tab:		ManagerGadgetPrevTabGroup()\n\
~m ~a <Key>Tab:			ManagerGadgetNextTabGroup()";

externaldef(translations) const char _XmManager_defaultTranslations[] = "\
<BtnMotion>:			ManagerGadgetButtonMotion()\n\
c<Btn1Down>:			ManagerGadgetTraverseCurrent()\n\
~c<Btn1Down>:			ManagerGadgetArm()\n\
~c<Btn1Down>,~c<Btn1Up>:	ManagerGadgetActivate()\n\
~c<Btn1Up>:			ManagerGadgetActivate()\n\
~c<Btn1Down>(2+):		ManagerGadgetMultiArm()\n\
~c<Btn1Up>(2+):			ManagerGadgetMultiActivate()\n\
<Btn2Down>:			ManagerGadgetDrag()\n\
:<Key>osfActivate:		ManagerParentActivate()\n\
:<Key>osfCancel:		ManagerParentCancel()\n\
:<Key>osfSelect:		ManagerGadgetSelect()\n\
:<Key>osfHelp:			ManagerGadgetHelp()\n\
~s ~m ~a <Key>Return:		ManagerParentActivate()\n\
~s ~m ~a <Key>space:		ManagerGadgetSelect()\n\
<Key>:				ManagerGadgetKeyInput()";

/*** MenuShell.c ***/
externaldef(translations) const char _XmMenuShell_translations [] = "\
<Key>osfCancel:			MenuEscape()\n\
<BtnDown>:			ClearTraversal()\n\
<BtnUp>:			MenuShellPopdownDone()";

/*** Primitive.c ***/
externaldef(translations) const char _XmPrimitive_defaultTranslations[] = "\
<Unmap>:		PrimitiveUnmap()\n\
<FocusIn>:		PrimitiveFocusIn()\n\
<FocusOut>:		PrimitiveFocusOut()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfBeginLine:	PrimitiveTraverseHome()\n\
:<Key>osfUp:		PrimitiveTraverseUp()\n\
:<Key>osfDown:		PrimitiveTraverseDown()\n\
:<Key>osfLeft:		PrimitiveTraverseLeft()\n\
:<Key>osfRight:		PrimitiveTraverseRight()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
s ~m ~a <Key>Tab:	PrimitivePrevTabGroup()\n\
~m ~a <Key>Tab:		PrimitiveNextTabGroup()";

/*** PushB.c ***/
externaldef(translations) const char _XmPushB_defaultTranslations[] = "\
<EnterWindow>:			Enter()\n\
<LeaveWindow>:			Leave()\n\
c<Btn1Down>:			ButtonTakeFocus()\n\
~c<Btn1Down>:			Arm()\n\
~c<Btn1Down>,~c<Btn1Up>:	Activate() Disarm()\n\
~c<Btn1Down>(2+):		MultiArm()\n\
~c<Btn1Up>(2+):			MultiActivate()\n\
~c<Btn1Up>:			Activate() Disarm()\n\
~c<Btn2Down>:			ProcessDrag()\n\
:<Key>osfActivate:		PrimitiveParentActivate()\n\
:<Key>osfCancel:		PrimitiveParentCancel()\n\
:<Key>osfSelect:		ArmAndActivate()\n\
:<Key>osfHelp:			Help()\n\
~s ~m ~a <Key>Return:		PrimitiveParentActivate()\n\
~s ~m ~a <Key>space:		ArmAndActivate()";

externaldef(translations) const char _XmPushB_menuTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
<Btn2Down>:		ProcessDrag()\n\
c<Btn1Down>:		MenuButtonTakeFocus()\n\
c<Btn1Up>:		MenuButtonTakeFocusUp()\n\
~c<BtnDown>:		BtnDown()\n\
~c<BtnUp>:		BtnUp()\n\
:<Key>osfSelect:	ArmAndActivate()\n\
:<Key>osfActivate:	ArmAndActivate()\n\
:<Key>osfCancel:	MenuEscape()\n\
:<Key>osfHelp:		Help()\n\
~s ~m ~a <Key>Return:	ArmAndActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

/*** RowColumn.c ***/
externaldef(translations) const char _XmRowColumn_menu_traversal_table[] = "\
<Unmap>:		MenuUnmap()\n\
<EnterWindow>Normal:	MenuEnter()\n\
<FocusIn>:		MenuFocusIn()\n\
<FocusOut>:		MenuFocusOut()\n\
:<Key>osfHelp:		MenuHelp()\n\
:<Key>osfLeft:		MenuGadgetTraverseLeft()\n\
:<Key>osfRight:		MenuGadgetTraverseRight()\n\
:<Key>osfUp:		MenuGadgetTraverseUp()\n\
:<Key>osfDown:		MenuGadgetTraverseDown()";

externaldef(translations) const char _XmRowColumn_option_table[]= "\
<Btn2Down>:		MenuGadgetDrag()\n\
c<Btn2Up>:		MenuBtnUp()\n\
c<Btn1Down>:		MenuGadgetTraverseCurrent()\n\
c<Btn1Up>:		MenuGadgetTraverseCurrentUp()\n\
~c<BtnDown>:		MenuBtnDown()\n\
~c<BtnUp>:		MenuBtnUp()\n\
:<Key>osfActivate:	ManagerParentActivate()\n\
:<Key>osfCancel:	ManagerParentCancel()\n\
:<Key>osfSelect:	ManagerGadgetSelect()\n\
:<Key>osfHelp:		MenuHelp()\n\
~s ~m ~a <Key>Return:	ManagerParentActivate()\n\
~s ~m ~a <Key>space:	ManagerGadgetSelect()";

externaldef(translations) const char _XmRowColumn_bar_table[]= "\
<Btn2Down>:		MenuGadgetDrag()\n\
c<Btn2Up>:		MenuBtnUp()\n\
c<Btn1Down>:		MenuGadgetTraverseCurrent()\n\
c<Btn1Up>:		MenuGadgetTraverseCurrentUp()\n\
~c<BtnDown>:		MenuBtnDown()\n\
~c<BtnUp>:		MenuBtnUp()\n\
:<Key>osfSelect:	MenuBarGadgetSelect()\n\
:<Key>osfActivate:	MenuBarGadgetSelect()\n\
:<Key>osfHelp:		MenuHelp()\n\
:<Key>osfCancel:	MenuGadgetEscape()\n\
~s ~m ~a <Key>Return:	MenuBarGadgetSelect()\n\
~s ~m ~a <Key>space:	MenuBarGadgetSelect()";

externaldef(translations) const char _XmRowColumn_menu_table[]= "\
c<Btn2Up>:		MenuBtnUp()\n\
c<Btn1Down>:		MenuGadgetTraverseCurrent()\n\
c<Btn1Up>:		MenuGadgetTraverseCurrentUp()\n\
~c<BtnDown>:		MenuBtnDown()\n\
~c<BtnUp>:		MenuBtnUp()\n\
:<Key>osfSelect:	ManagerGadgetSelect()\n\
:<Key>osfActivate:	ManagerGadgetSelect()\n\
:<Key>osfHelp:		MenuHelp()\n\
:<Key>osfCancel:	MenuGadgetEscape()\n\
~s ~m ~a <Key>Return:	ManagerGadgetSelect()\n\
~s ~m ~a <Key>space:	ManagerGadgetSelect()";

/*** Sash.c ***/
externaldef(translations) const char _XmSash_defTranslations[] = "\
<Unmap>:			PrimitiveUnmap()\n\
<EnterWindow>:			enter()\n\
<LeaveWindow>:			leave()\n\
<FocusIn>:			SashFocusIn()\n\
<FocusOut>:			SashFocusOut()\n\
~c ~s ~m ~a <Btn1Down>:		SashAction(Start)\n\
~c ~s ~m ~a <Btn1Motion>:	SashAction(Move)\n\
~c ~s ~m ~a <Btn1Up>:		SashAction(Commit)\n\
~c ~s ~m ~a <Btn2Down>:		SashAction(Start)\n\
~c ~s ~m ~a <Btn2Motion>:	SashAction(Move)\n\
~c ~s ~m ~a <Btn2Up>:		SashAction(Commit)\n\
:<Key>osfActivate:		PrimitiveParentActivate()\n\
:<Key>osfCancel:		PrimitiveParentCancel()\n\
:<Key>osfHelp:			Help()\n\
:c <Key>osfUp:			SashAction(Key,LargeIncr,Up)\n\
:<Key>osfUp:			SashAction(Key,DefaultIncr,Up)\n\
:c <Key>osfRight:		SashAction(Key,LargeIncr,Right)\n\
:<Key>osfRight:			SashAction(Key,DefaultIncr,Right)\n\
:c <Key>osfDown:		SashAction(Key,LargeIncr,Down)\n\
:<Key>osfDown:			SashAction(Key,DefaultIncr,Down)\n\
:c <Key>osfLeft:		SashAction(Key,LargeIncr,Left)\n\
:<Key>osfLeft:			SashAction(Key,DefaultIncr,Left)\n\
~s ~m ~a <Key>Return:		PrimitiveParentActivate()\n\
s ~m ~a <Key>Tab:		PrevTabGroup()\n\
~m ~a <Key>Tab:			NextTabGroup()";

/*** ScrollBar.c ***/
externaldef(translations) const char _XmScrollBar_defaultTranslations[] = "\
<Unmap>:			PrimitiveUnmap()\n\
<Enter>:			PrimitiveEnter()\n\
<Leave>:			PrimitiveLeave()\n\
<FocusIn>:			PrimitiveFocusIn()\n\
<FocusOut>:			PrimitiveFocusOut()\n\
~s ~c ~m ~a <Btn1Down>:		Select()\n\
<Btn1Up>:		        Release()\n\
~s ~c ~m ~a Button1<PtrMoved>:	Moved()\n\
~s ~c ~m ~a <Btn2Down>:		Select()\n\
<Btn2Up>:		        Release()\n\
~s ~c ~m ~a Button2<PtrMoved>:	Moved()\n\
~s c ~m ~a <Btn1Down>:		TopOrBottom()\n\
:<Key>osfActivate:		PrimitiveParentActivate()\n\
:<Key>osfCancel:		CancelDrag()\n\
:<Key>osfBeginLine:		TopOrBottom()\n\
:<Key>osfEndLine:		TopOrBottom()\n\
:<Key>osfPageLeft:		PageUpOrLeft(1)\n\
:c <Key>osfPageUp:		PageUpOrLeft(1)\n\
:<Key>osfPageUp:		PageUpOrLeft(0)\n\
:<Key>osfPageRight:		PageDownOrRight(1)\n\
:c <Key>osfPageDown:		PageDownOrRight(1)\n\
:<Key>osfPageDown:		PageDownOrRight(0)\n\
:<Key>osfHelp:			PrimitiveHelp()\n\
:c <Key>osfUp:			PageUpOrLeft(0)\n\
:<Key>osfUp:			IncrementUpOrLeft(0)\n\
:c <Key>osfDown:		PageDownOrRight(0)\n\
:<Key>osfDown:			IncrementDownOrRight(0)\n\
:c <Key>osfLeft:		PageUpOrLeft(1)\n\
:<Key>osfLeft:			IncrementUpOrLeft(1)\n\
:c <Key>osfRight:		PageDownOrRight(1)\n\
:<Key>osfRight:			IncrementDownOrRight(1)\n\
~s ~m ~a <Key>Return:		PrimitiveParentActivate()\n\
s ~m ~a <Key>Tab:		PrimitivePrevTabGroup()\n\
~m ~a <Key>Tab:			PrimitiveNextTabGroup()\n\
~s ~c ~m ~a <Btn4Down>,<Btn4Up>:IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(1) IncrementUpOrLeft(1) IncrementUpOrLeft(1)\n\
~s ~c ~m ~a <Btn5Down>,<Btn5Up>:IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(1) IncrementDownOrRight(1) IncrementDownOrRight(1)\n\
<Btn4Down>,<Btn4Up>:		IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(0) IncrementUpOrLeft(1) IncrementUpOrLeft(1) IncrementUpOrLeft(1) IncrementUpOrLeft(1) IncrementUpOrLeft(1) IncrementUpOrLeft(1)\n\
<Btn5Down>,<Btn5Up>:		IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(0) IncrementDownOrRight(1) IncrementDownOrRight(1) IncrementDownOrRight(1) IncrementDownOrRight(1) IncrementDownOrRight(1)IncrementDownOrRight(1)";

/*** ScrolledW.c ***/
externaldef(translations)
const char _XmScrolledW_ScrolledWindowXlations[] = "\
<EnterWindow>:		ManagerEnter()\n\
<FocusOut>:		ManagerFocusOut()\n\
<FocusIn>:		ManagerFocusIn()\n\
<Btn2Down>:		ManagerGadgetDrag()\n\
:<Key>osfActivate:	ManagerParentActivate()\n\
:<Key>osfCancel:	ManagerParentCancel()\n\
:c <Key>osfBeginLine:	SWTopLine()\n\
:<Key>osfBeginLine:	SWBeginLine()\n\
:c <Key>osfEndLine:	SWBottomLine()\n\
:<Key>osfEndLine:	SWEndLine()\n\
:<Key>osfPageLeft:	SWLeftPage()\n\
:c <Key>osfPageUp:	SWLeftPage()\n\
:<Key>osfPageUp:	SWUpPage()\n\
:<Key>osfPageRight:	SWRightPage()\n\
:c <Key>osfPageDown:	SWRightPage()\n\
:<Key>osfPageDown:	SWDownPage()\n\
:<Key>osfHelp:		ManagerGadgetHelp()\n\
:<Key>osfUp:		ManagerGadgetTraverseUp()\n\
:<Key>osfDown:		ManagerGadgetTraverseDown()\n\
:<Key>osfLeft:		ManagerGadgetTraverseLeft()\n\
:<Key>osfRight:		ManagerGadgetTraverseRight()\n\
~s ~m ~a <Key>Return:	ManagerParentActivate()\n\
s ~m ~a <Key>Tab:	ManagerGadgetPrevTabGroup()\n\
~m ~a <Key>Tab:		ManagerGadgetNextTabGroup()";

/* N.B.: This string is hard-coded in ClipWindow.c:ClipWindowKeys! */
externaldef(translations) const char _XmClipWindowTranslationTable[] = "\
:c <Key>osfBeginLine:	ActionGrab(SWTopLine)\n\
:<Key>osfBeginLine:	ActionGrab(SWBeginLine)\n\
:c <Key>osfEndLine:	ActionGrab(SWBottomLine)\n\
:<Key>osfEndLine:	ActionGrab(SWEndLine)\n\
:<Key>osfPageLeft:	ActionGrab(SWLeftPage)\n\
:c <Key>osfPageUp:	ActionGrab(SWLeftPage)\n\
:<Key>osfPageUp:	ActionGrab(SWUpPage)\n\
:<Key>osfPageRight:	ActionGrab(SWRightPage)\n\
:c <Key>osfPageDown:	ActionGrab(SWRightPage)\n\
:<Key>osfPageDown:	ActionGrab(SWDownPage)";


/*** SelectioB.c ***/
externaldef(translations)
const char _XmSelectioB_defaultTextAccelerators[] = "\
\043override\n\
:<Key>osfUp:		SelectionBoxUpOrDown(0)\n\
:<Key>osfDown:		SelectionBoxUpOrDown(1)\n\
:<Key>osfBeginLine:	SelectionBoxUpOrDown(2)\n\
:<Key>osfEndLine:	SelectionBoxUpOrDown(3)\n\
:<Key>osfRestore:	SelectionBoxRestore()\n\
s c ~m ~a <Key>space:	SelectionBoxRestore()";

/*** TearOffB.c ***/
externaldef(translations) const char _XmTearOffB_overrideTranslations[] = "\
<Btn2Down>:		BDrag()\n\
<BtnUp>:		BActivate()\n\
:<Key>osfSelect:	KActivate()\n\
:<Key>osfActivate:	KActivate()\n\
~s ~m ~a <Key>Return:	KActivate()\n\
~s ~m ~a <Key>space:	KActivate()";

/*** TextF.c ***/
externaldef(translations) const char _XmTextF_EventBindings1[] = "\
<Unmap>:		unmap()\n\
<Enter>:		enter()\n\
<Leave>:		leave()\n\
<FocusIn>:		focusIn()\n\
<FocusOut>:		focusOut()\n\
~c ~s ~m ~a <Btn1Down>:	process-bselect(grab-focus)\n\
c ~s ~m ~a <Btn1Down>:	process-bselect(move-destination)\n\
~c s ~m ~a <Btn1Down>:	process-bselect(extend-start)\n\
~c ~m ~a <Btn1Motion>:	process-bselect(extend-adjust)\n\
~c ~m ~a <Btn1Up>:	process-bselect(extend-end)\n\
c ~s ~m a <Btn1Down>:	process-bselect-event(process-bdrag, process-bselect)\n\
c ~s ~m a <Btn1Motion>:	process-bselect-event(secondary-adjust, process-bselect)\n\
c ~s ~m a <Btn1Up>:	process-bselect-event(copy-to, process-bselect)\n\
~c s ~m a <Btn1Down>:	process-bselect-event(process-bdrag, process-bselect)\n\
~c s ~m a <Btn1Motion>:	process-bselect-event(secondary-adjust, process-bselect)\n\
~c s ~m a <Btn1Up>:	process-bselect-event(move-to, process-bselect)\n\
~m ~a <Btn1Up>:	        process-bselect()\n\
m ~a <Btn1Down>:	process-bselect()\n\
~m a <Btn1Down>:	process-bselect()\n\
<Btn2Down>:		process-bdrag-event(extend-start, process-bdrag)\n\
m ~a <Btn2Motion>:	process-bdrag-event(extend-adjust,secondary-adjust)\n\
~m a <Btn2Motion>:	process-bdrag-event(extend-adjust,secondary-adjust)\n\
<Btn2Motion>:	        process-bdrag-event(extend-adjust)\n\
s c <Btn2Up>:		process-bdrag-event(extend-end, link-to)\n\
~s <Btn2Up>:		process-bdrag-event(extend-end, copy-to)\n\
~c <Btn2Up>:		process-bdrag-event(extend-end, move-to)\n\
:m <Key>osfPrimaryPaste:cut-primary()\n\
:a <Key>osfPrimaryPaste:cut-primary()\n\
:<Key>osfPrimaryPaste:	copy-primary()\n\
:m <Key>osfCut:		cut-primary()\n\
:a <Key>osfCut:		cut-primary()\n\
:<Key>osfCut:		cut-clipboard()\n\
:<Key>osfPaste:		paste-clipboard()\n\
:m <Key>osfCopy:	copy-primary()\n\
:a <Key>osfCopy:	copy-primary()\n\
:<Key>osfCopy:		copy-clipboard()\n\
:s <Key>osfBeginLine:	beginning-of-line(extend)\n\
:<Key>osfBeginLine:	beginning-of-line()\n\
:s <Key>osfEndLine:	end-of-line(extend)\n\
:<Key>osfEndLine:	end-of-line()\n\
:s <Key>osfPageLeft:	page-left(extend)\n\
:<Key>osfPageLeft:	page-left()\n\
:s c<Key>osfPageUp:	page-left(extend)\n\
:c <Key>osfPageUp:	page-left()\n\
:s <Key>osfPageRight:	page-right(extend)\n\
:<Key>osfPageRight:	page-right()\n";

externaldef(translations) const char _XmTextF_EventBindings2[] = "\
:s c <Key>osfPageDown:	page-right(extend)\n\
:c <Key>osfPageDown:	page-right()\n\
:<Key>osfClear:		clear-selection()\n\
:<Key>osfBackSpace:	delete-previous-character()\n\
:s m <Key>osfDelete:	cut-primary()\n\
:s a <Key>osfDelete:	cut-primary()\n\
:s <Key>osfDelete:	cut-clipboard()\n\
:c <Key>osfDelete:	delete-to-end-of-line()\n\
:<Key>osfDelete:	delete-next-character()\n\
:c m <Key>osfInsert:	copy-primary()\n\
:c a <Key>osfInsert:	copy-primary()\n\
:s <Key>osfInsert:	paste-clipboard()\n\
:c <Key>osfInsert:	copy-clipboard()\n\
:<Key>osfInsert:	toggle-overstrike()\n\
:s <Key>osfSelect:	key-select()\n\
:<Key>osfSelect:	set-anchor()\n\
:<Key>osfSelectAll:	select-all()\n\
:<Key>osfDeselectAll:	deselect-all()\n\
:<Key>osfActivate:	activate()\n\
:<Key>osfAddMode:	toggle-add-mode()\n\
:<Key>osfHelp:		Help()\n\
:<Key>osfCancel:	process-cancel()\n\
:s c <Key>osfLeft:	backward-word(extend)\n\
:c <Key>osfLeft:	backward-word()\n\
:s <Key>osfLeft:	key-select(left)\n\
:<Key>osfLeft:		backward-character()\n\
:s c <Key>osfRight:	forward-word(extend)\n\
:c <Key>osfRight:	forward-word()\n\
:s <Key>osfRight:	key-select(right)\n\
:<Key>osfRight:		forward-character()\n\
:<Key>osfUp:		traverse-prev()\n\
:<Key>osfDown:		traverse-next()\n";

externaldef(translations) const char _XmTextF_EventBindings3[] = "\
c ~m ~a <Key>slash:	select-all()\n\
c ~m ~a <Key>backslash:	deselect-all()\n\
s ~m ~a <Key>Tab:	prev-tab-group()\n\
~m ~a <Key>Tab:		next-tab-group()\n\
~s ~m ~a <Key>Return:	activate()\n\
c ~s ~m ~a <Key>space:	set-anchor()\n\
c s ~m ~a <Key>space:	key-select()\n\
s ~c ~m ~a <Key>space:	self-insert()\n\
<Key>:			self-insert()";

/*** TextIn.c ***/
externaldef(translations) const char _XmTextIn_XmTextEventBindings1[] = "\
<Unmap>:		unmap()\n\
<EnterWindow>:		enter()\n\
<LeaveWindow>:		leave()\n\
<FocusIn>:		focusIn()\n\
<FocusOut>:		focusOut()\n\
~c ~s ~m ~a <Btn1Down>:	process-bselect(grab-focus)\n\
c ~s ~m ~a <Btn1Down>:	process-bselect(move-destination)\n\
~c s ~m ~a <Btn1Down>:	process-bselect(extend-start)\n\
~c ~m ~a <Btn1Motion>:	process-bselect(extend-adjust)\n\
~c ~m ~a <Btn1Up>:	process-bselect(extend-end)\n\
c ~s ~m a <Btn1Down>:	process-bselect-event(process-bdrag, process-bselect)\n\
c ~s ~m a <Btn1Motion>:	process-bselect-event(secondary-adjust, process-bselect)\n\
c ~s ~m a <Btn1Up>:	process-bselect-event(copy-to, process-bselect)\n\
~c s ~m a <Btn1Down>:	process-bselect-event(process-bdrag, process-bselect)\n\
~c s ~m a <Btn1Motion>:	process-bselect-event(secondary-adjust, process-bselect)\n\
~c s ~m a <Btn1Up>:	process-bselect-event(move-to, process-bselect)\n\
~m ~a <Btn1Up>:		process-bselect()\n\
m ~a <Btn1Down>:	process-bselect()\n\
~m a <Btn1Down>:	process-bselect()\n\
<Btn2Down>:		process-bdrag-event(extend-start, process-bdrag)\n\
m ~a <Btn2Motion>:	process-bdrag-event(extend-adjust, secondary-adjust)\n\
~m a <Btn2Motion>:	process-bdrag-event(extend-adjust, secondary-adjust)\n\
<Btn2Motion>:	        process-bdrag-event(extend-adjust)\n\
s c <Btn2Up>:		process-bdrag-event(extend-end, link-to)\n\
~s <Btn2Up>:		process-bdrag-event(extend-end, copy-to)\n\
~c <Btn2Up>:		process-bdrag-event(extend-end, move-to)\n\
:m <Key>osfPrimaryPaste:cut-primary()\n\
:a <Key>osfPrimaryPaste:cut-primary()\n\
:<Key>osfPrimaryPaste:	copy-primary()\n\
:m <Key>osfCut:		cut-primary()\n\
:a <Key>osfCut:		cut-primary()\n\
:<Key>osfCut:		cut-clipboard()\n\
:<Key>osfPaste:		paste-clipboard()\n\
:m <Key>osfCopy:	copy-primary()\n\
:a <Key>osfCopy:	copy-primary()\n\
:<Key>osfCopy:		copy-clipboard()\n\
:s c <Key>osfBeginLine:	beginning-of-file(extend)\n\
:c <Key>osfBeginLine:	beginning-of-file()\n\
:s <Key>osfBeginLine:	beginning-of-line(extend)\n\
:<Key>osfBeginLine:	beginning-of-line()\n\
:s c <Key>osfEndLine:	end-of-file(extend)\n\
:c <Key>osfEndLine:	end-of-file()\n\
:s <Key>osfEndLine:	end-of-line(extend)\n\
:<Key>osfEndLine:	end-of-line()\n\
:s <Key>osfPageLeft:	page-left(extend)\n\
:<Key>osfPageLeft:	page-left()\n\
:s c <Key>osfPageUp:	page-left(extend)\n\
:c <Key>osfPageUp:	page-left()\n\
:s <Key>osfPageUp:	previous-page(extend)\n\
:<Key>osfPageUp:	previous-page()\n\
:s <Key>osfPageRight:	page-right(extend)\n\
:<Key>osfPageRight:	page-right()\n";

externaldef(translations) const char _XmTextIn_XmTextEventBindings2[] = "\
:s c <Key>osfPageDown:	page-right(extend)\n\
:c <Key>osfPageDown:	page-right()\n\
:s <Key>osfPageDown:	next-page(extend)\n\
:<Key>osfPageDown:	next-page()\n\
:<Key>osfClear:		clear-selection()\n\
:<Key>osfBackSpace:	delete-previous-character()\n\
:s m <Key>osfDelete:	cut-primary()\n\
:s a <Key>osfDelete:	cut-primary()\n\
:s <Key>osfDelete:	cut-clipboard()\n\
:c <Key>osfDelete:	delete-to-end-of-line()\n\
:<Key>osfDelete:	delete-next-character()\n\
:c m <Key>osfInsert:	copy-primary()\n\
:c a <Key>osfInsert:	copy-primary()\n\
:s <Key>osfInsert:	paste-clipboard()\n\
:c <Key>osfInsert:	copy-clipboard()\n\
:<Key>osfInsert:	toggle-overstrike()\n\
:s <Key>osfSelect:	key-select()\n\
:<Key>osfSelect:	set-anchor()\n\
:<Key>osfSelectAll:	select-all()\n\
:<Key>osfDeselectAll:	deselect-all()\n\
:<Key>osfActivate:	activate()\n\
:<Key>osfAddMode:	toggle-add-mode()\n\
:<Key>osfHelp:		Help()\n\
:<Key>osfCancel:	process-cancel()\n\
:s c <Key>osfLeft:	backward-word(extend)\n\
:c <Key>osfLeft:	backward-word()\n\
:s <Key>osfLeft:	key-select(left)\n\
:<Key>osfLeft:		backward-character()\n\
:s c <Key>osfRight:	forward-word(extend)\n\
:c <Key>osfRight:	forward-word()\n\
:s <Key>osfRight:	key-select(right)\n\
:<Key>osfRight:		forward-character()\n\
:s c <Key>osfUp:	backward-paragraph(extend)\n\
:c <Key>osfUp:		backward-paragraph()\n\
:s <Key>osfUp:		process-shift-up()\n\
:<Key>osfUp:		process-up()\n\
:s c <Key>osfDown:	forward-paragraph(extend)\n\
:c <Key>osfDown:	forward-paragraph()\n\
:s <Key>osfDown:	process-shift-down()\n\
:<Key>osfDown:		process-down()\n";

externaldef(translations) const char _XmTextIn_XmTextEventBindings3[] = "\
    c ~m ~a <Key>slash:		select-all()\n\
    c ~m ~a <Key>backslash:	deselect-all()\n\
 s  c ~m ~a <Key>Tab:		prev-tab-group()\n\
~s  c ~m ~a <Key>Tab:		next-tab-group()\n\
 s ~c ~m ~a <Key>Tab:		process-tab(Prev)\n\
~s ~c ~m ~a <Key>Tab:		process-tab(Next)\n\
~s  c ~m ~a <Key>Return:	activate()\n\
~s ~c ~m ~a <Key>Return:	process-return()\n\
~s  c ~m ~a <Key>space:		set-anchor()\n\
 s  c ~m ~a <Key>space:		key-select()\n\
 s ~c ~m ~a <Key>space:		self-insert()\n\
<Key>:				self-insert()\n\
~s ~c ~m ~a <Btn4Down>,<Btn4Up>:scroll-one-line-down() scroll-one-line-down() scroll-one-line-down()\n\
~s ~c ~m ~a <Btn5Down>,<Btn5Up>:scroll-one-line-up() scroll-one-line-up() scroll-one-line-up()\n\
<Btn4Down>,<Btn4Up>:		scroll-one-line-down() scroll-one-line-down() scroll-one-line-down() scroll-one-line-down() scroll-one-line-down() scroll-one-line-down() scroll-one-line-down()\n\
<Btn5Down>,<Btn5Up>:		scroll-one-line-up() scroll-one-line-up() scroll-one-line-up() scroll-one-line-up() scroll-one-line-up() scroll-one-line-up() scroll-one-line-up()";

externaldef(translations) const char _XmTextIn_XmTextVEventBindings[] = "\
:s c <Key>osfLeft:forward-paragraph(extend)\n\
:c <Key>osfLeft:forward-paragraph()\n\
:s <Key>osfLeft:process-shift-left()\n\
:<Key>osfLeft:process-left()\n\
:s c <Key>osfRight:backward-paragraph(extend)\n\
:c <Key>osfRight:backward-paragraph()\n\
:s <Key>osfRight:process-shift-right(right)\n\
:<Key>osfRight:process-right()\n\
:s c <Key>osfUp:backward-word(extend)\n\
:c <Key>osfUp:backward-word()\n\
:s <Key>osfUp:key-select(up)\n\
:<Key>osfUp:backward-character()\n\
:s c <Key>osfDown:forward-word(extend)\n\
:c <Key>osfDown:forward-word()\n\
:s <Key>osfDown:key-select(down)\n\
:<Key>osfDown:forward-character()\n\
:s <Key>osfPageLeft:next-page(extend)\n\
:<Key>osfPageLeft:next-page()\n\
:s <Key>osfPageUp:page-up(extend)\n\
:<Key>osfPageUp:page-up()\n\
:s <Key>osfPageRight:previous-page(extend)\n\
:<Key>osfPageRight:previous-page()\n\
:s <Key>osfPageDown:page-down(extend)\n\
:<Key>osfPageDown:page-down()";


/*** ToggleB.c ***/
externaldef(translations) const char _XmToggleB_defaultTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
c<Btn1Down>:		ButtonTakeFocus()\n\
~c<Btn1Down>:		Arm()\n\
~c<Btn1Up>:		Select() Disarm()\n\
<Btn2Down>:		ProcessDrag()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfSelect:	ArmAndActivate()\n\
:<Key>osfHelp:		Help()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

externaldef(translations) const char _XmToggleB_menuTranslations[] = "\
<EnterWindow>:		Enter()\n\
<LeaveWindow>:		Leave()\n\
<Btn2Down>:		ProcessDrag()\n\
c<Btn1Down>:		MenuButtonTakeFocus()\n\
c<Btn1Up>:		MenuButtonTakeFocusUp()\n\
~c<BtnDown>:		BtnDown()\n\
~c<BtnUp>:		BtnUp()\n\
:<Key>osfSelect:	ArmAndActivate()\n\
:<Key>osfActivate:	ArmAndActivate()\n\
:<Key>osfHelp:		Help()\n\
:<Key>osfCancel:	MenuEscape()\n\
~s ~m ~a <Key>Return:	ArmAndActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

/*** VirtKeys.c ***/

/* Do not abbreviate meta, ctrl, shift, lock, alt, etc. */
externaldef(translations) const char _XmVirtKeys_fallbackBindingString[] = "\
osfCancel:<Key>Escape,<Key>Cancel\n\
osfLeft:<Key>Left,<Key>KP_Left\n\
osfUp:<Key>Up,<Key>KP_Up\n\
osfRight:<Key>Right,<Key>KP_Right\n\
osfDown:<Key>Down,<Key>KP_Down\n\
osfEndLine:<Key>End,<Key>KP_End\n\
osfBeginLine:<Key>Home,<Key>Begin,<Key>KP_Home\n\
osfPageUp:<Key>Prior,<Key>KP_Prior\n\
osfPageDown:<Key>Next,<Key>KP_Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete,<Key>KP_Delete\n\
osfInsert:<Key>Insert,<Key>KP_Insert\n\
osfAddMode:Shift<Key>F8\n\
osfHelp:<Key>F1,<Key>Help\n\
osfMenu:Shift<Key>F10,<Key>Menu\n\
osfMenuBar:<Key>F10,Shift<Key>Menu\n\
osfSelect:<Key>Select\n\
osfActivate:<Key>KP_Enter,<Key>Execute\n\
osfClear:<Key>Clear\n\
osfUndo:<Key>Undo\n\
osfSwitchDirection:Alt<Key>Return,Alt<Key>KP_Enter";

/*"Acorn Computers Ltd"
* Acorn RISC iX versions 1.0->1.2 running on Acorn R140, R225, R260
* (all national keyboard variants)*/

externaldef(translations) const char _XmVirtKeys_acornFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:Alt <Key>Right\n\
osfBeginLine:Alt <Key>Left\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfActivate:<Key>KP_Enter\n\
osfCopy:<Key>Select";


/*"Apollo Computer Inc."*/

externaldef(translations) const char _XmVirtKeys_apolloFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>apRightBar\n\
osfBeginLine:<Key>apLeftBar\n\
osfPageLeft:<Key>apLeftBox\n\
osfPageRight:<Key>apRightBox\n\
osfPageUp:<Key>apUpBox\n\
osfPageDown:<Key>apDownBox\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>apCharDel\n\
osfInsert:<Key>Select\n\
osfAddMode:Shift<Key>F8\n\
osfHelp:<Key>Help\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfCopy:<Key>apCopy\n\
osfCut:<Key>apCut\n\
osfPaste:<Key>apPaste\n\
osfUndo:<Key>Undo";


/*"Data General Corporation Rev 04"
* AViiON */

externaldef(translations) const char _XmVirtKeys_dgFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*"DECWINDOWS DigitalEquipmentCorp."*/

externaldef(translations) const char _XmVirtKeys_decFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:Alt<Key>Right\n\
osfBeginLine:Alt<Key>Left\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>Delete\n\
osfDelete:<Key>DRemove\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift<Key>F8\n\
osfHelp:<Key>Help\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfSelect:<Key>Select\n\
osfActivate:<Key>KP_Enter\n\
osfPrimaryPaste:<Key>F14";


/*"Double Click Imaging, Inc. KeyX"
* for the version of KeyX running on 386 AT bus compatibles. */

externaldef(translations) const char _XmVirtKeys_dblclkFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*"Hewlett-Packard Company" */

externaldef(translations) const char _XmVirtKeys_hpFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift<Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfSelect:<Key>Select\n\
osfClear:<Key>Clear\n\
osfUndo:<Key>Undo\n\
osfPrimaryPaste:Alt Ctrl<Key>Insert";


/*"International Business Machines"
* for AIX/PS2 and RS/6000 systems */

externaldef(translations) const char _XmVirtKeys_ibmFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*  Intergraph keyboard support        */
/* Intergraph */

externaldef(translations) const char _XmVirtKeys_ingrFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:Alt<Key>Right\n\
osfBeginLine:Alt<Key>Left\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift<Key>F8\n\
osfHelp:<Key>Help\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*"Megatek Corporation"
* Megatek X-Cellerator */

externaldef(translations) const char _XmVirtKeys_megatekFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>R13\n\
osfBeginLine:<Key>F27\n\
osfPageUp:<Key>F29\n\
osfPageDown:<Key>F35\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>Help\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfCopy:<Key>F16\n\
osfCut:<Key>F20\n\
osfPaste:<Key>F18\n\
osfUndo:<Key>F14";


/*"Motorola Inc. (Microcomputer Division)" */
/* (c) Copyright 1990 Motorola Inc. */
/* Motorola provides these key bindings as is,
	with no guarantees or warranties implied.
	Motorola is under no obligation to support,
	update, or extend these key bindings for
	future releases. */

externaldef(translations) const char _XmVirtKeys_motorolaFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*"Silicon Graphics Inc." */

externaldef(translations) const char _XmVirtKeys_sgiFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfActivate:<Key>KP_Enter\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";


/*"Siemens Munich by SP-4's Hacker Crew"
* Siemens WX200 system */

externaldef(translations) const char _XmVirtKeys_siemensWx200FallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>Cancel\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete,<Key>F29\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>Help,<Key>F1\n\
osfActivate:<Key>KP_Enter\n\
osfMenu:<Key>Menu,Shift <Key>F10\n\
osfMenuBar:<Key>F10";


/*"Siemens Munich (SP-4's hacker-clan)"
* Siemens 9733 system */

externaldef(translations) const char _XmVirtKeys_siemens9733FallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete_char\n\
osfInsert:<Key>Insert_char\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>Help\n\
osfMenu:<Key>Linefeed\n\
osfMenuBar:<Key>F10";


/* "Sun Microsystems, Inc." */

externaldef(translations) const char _XmVirtKeys_sunFallbackBindingString[] = "\
osfActivate:<Key>KP_Enter\n\
osfCancel:<Key>Escape\n\
osfHelp:<Key>Help,<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfBeginLine:<Key>Home\n\
osfEndLine:<Key>End\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfUndo:<Key>Undo\n\
osfAddMode:Shift<Key>F8\n\
osfCopy:<Key>SunCopy\n\
osfCut:<Key>SunCut\n\
osfPaste:<Key>SunPaste";


/*"Tektronix, Inc." */

externaldef(translations) const char _XmVirtKeys_tekFallbackBindingString[] = "\
osfCancel:<Key>Escape\n\
osfLeft:<Key>Left\n\
osfUp:<Key>Up\n\
osfRight:<Key>Right\n\
osfDown:<Key>Down\n\
osfEndLine:<Key>End\n\
osfBeginLine:<Key>Home\n\
osfPageUp:<Key>Prior\n\
osfPageDown:<Key>Next\n\
osfBackSpace:<Key>BackSpace\n\
osfDelete:<Key>Delete\n\
osfInsert:<Key>Insert\n\
osfAddMode:Shift <Key>F8\n\
osfHelp:<Key>F1\n\
osfMenu:Shift<Key>F10\n\
osfMenuBar:<Key>F10";

