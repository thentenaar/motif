/* $XConsortium: ChRedef.h /main/4 1995/07/15 21:01:15 drk $ */
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

#ifndef TCMENU
/* Redefinitions of globals in files that are distinct from the shared
 * library versions.  These are needed to avoid conflicts with the shared
 * libraries.
 */
#define xmRowColumnWidgetClass TCxmRowColumnWidgetClass
#define XmCreateRadioBox TCXmCreateRadioBox
#define XmCreateRowColumn TCXmCreateRowColumn
#define XmCreateWorkArea TCXmCreateWorkArea
#define XmCreatePopupMenu TCXmCreatePopupMenu
#define XmCreatePulldownMenu TCXmCreatePulldownMenu
#define XmCreateOptionMenu TCXmCreateOptionMenu
#define XmCreateMenuBar TCXmCreateMenuBar
#define XmMenuPosition TCXmMenuPosition
#define XmOptionLabelGadget TCXmOptionLabelGadget
#define XmOptionButtonGadget TCXmOptionButtonGadget
#define XmGetPostedFromWidget TCXmGetPostedFromWidget
#define _XmMenuFocus TC_XmMenuFocus
#define _XmGetActiveTopLevelMenu TC_XmGetActiveTopLevelMenu
#define XmAddToPostFromList TCXmAddToPostFromList
#define XmRemoveFromPostFromList TCXmRemoveFromPostFromList
#define _XmButtonEventStatus TC_XmButtonEventStatus
#define _XmRC_GetMnemonicCharSet TC_XmRC_GetMnemonicCharSet
#define _XmRC_GetMenuAccelerator TC_XmRC_GetMenuAccelerator
#define _XmRC_GetLabelString TC_XmRC_GetLabelString
#define xmRowColumnClassRec TCxmRowColumnClassRec
#define _XmMatchBtnEvent toolchest_XmMatchBtnEvent
#define xmMenuShellWidgetClass toolchest_xmMenuShellWidgetClass
#define _XmEnterRowColumn toolchest_XmEnterRowColumn
#define XmCreateMenuShell toolchest_XmCreateMenuShell
#define xmMenuShellClassRec toolchest_xmMenuShellClassRec
#define xmCascadeButtonWidgetClass TCxmCascadeButtonWidgetClass
#define _XmArrowPixmapCacheDelete TC_XmArrowPixmapCacheDelete
#define _XmCascadingPopup TC_XmCascadingPopup
#define _XmButtonPopdownChildren TC_XmButtonPopdownChildren
#define _XmCBHelp TC_XmCBHelp
#define _XmCreateArrowPixmaps TC_XmCreateArrowPixmaps
#define XmCreateCascadeButton TCXmCreateCascadeButton
#define XmCascadeButtonHighlight TCXmCascadeButtonHighlight
#define xmCascadeButtonClassRec TCxmCascadeButtonClassRec
#endif
