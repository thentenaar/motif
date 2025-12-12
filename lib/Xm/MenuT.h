/* $XConsortium: MenuT.h /main/5 1995/07/15 20:53:03 drk $ */
/*
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
 * HISTORY
 */

#ifndef _XmMenuT_H
#define _XmMenuT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Quick fix for Linux-ARM where "arm" is the #define symbol */
#ifdef arm
# undef arm
#endif

/* Menu System Traits */
externalref XrmQuark XmQTmenuSystem;
externalref XrmQuark XmQTmenuSavvy;

/* Trait structures and typedefs, place typedefs first */

/* Used by the disable callback method of the menu savvy trait */
typedef enum {
    XmDISABLE_ACTIVATE,   /* defer to the entryCallback */
    XmENABLE_ACTIVATE     /* invoke own activate callback */
} XmActivateState;

/* Menu trait typedefs */

typedef int (*XmMenuSystemWidgetProc)(Widget);
typedef Boolean (*XmMenuSystemVerifyProc)(Widget, XEvent*);
typedef void (*XmMenuSystemControlTraversalProc)(Widget, Boolean);
typedef void (*XmMenuSystemCascadeProc)(Widget, Widget, XEvent*);
typedef void (*XmMenuSystemPositionProc)(Widget, XEvent*);
typedef Boolean (*XmMenuSystemPopdownProc)(Widget, XEvent*);
typedef void (*XmMenuSystemEntryCallbackProc)(Widget, Widget, XtPointer);
typedef Boolean (*XmMenuSystemUpdateHistoryProc)(Widget, Widget, Boolean);
typedef void (*XmMenuSystemUpdateBindingsProc)(Widget, int);
typedef void (*XmMenuSystemRecordPostFromWidgetProc)(Widget, Widget, Boolean);
typedef void (*XmMenuSystemDisarmProc)(Widget);
typedef Widget (*XmMenuSystemPopupPostedProc)(Widget);
typedef void (*XmMenuSavvyDisableProc)(Widget, XmActivateState);
typedef char* (*XmMenuSavvyGetAcceleratorProc)(Widget);
typedef KeySym (*XmMenuSavvyGetMnemonicProc)(Widget);
typedef char* (*XmMenuSavvyGetActivateCBNameProc)();
#define XmMenuSystemTypeProc		XmMenuSystemWidgetProc
#define XmMenuSystemStatusProc		XmMenuSystemWidgetProc
#define XmMenuSystemGetPostedFromWidgetProc	XmMenuSystemDisarmProc
#define XmMenuSystemArmProc		XmMenuSystemDisarmProc
#define XmMenuSystemMenuBarCleanupProc	XmMenuSystemDisarmProc
#define XmMenuSystemTearOffArmProc	XmMenuSystemDisarmProc
#define XmMenuSystemReparentProc	XmMenuSystemPositionProc
#define XmMenuSystemPopdownAllProc	XmMenuSystemPositionProc
#define XmMenuSystemChildFocusProc	XmMenuSystemDisarmProc

/* XmTmenuProcTrait */

/* Version 0: initial release. */

typedef struct _XmMenuSystemTraitRec
{
  int					version;		/* 0 */
  XmMenuSystemTypeProc			type;
  XmMenuSystemStatusProc		status;
  XmMenuSystemCascadeProc		cascade;
  XmMenuSystemVerifyProc		verifyButton;
  XmMenuSystemControlTraversalProc	controlTraversal;
  XmMenuSystemMenuBarCleanupProc	menuBarCleanup;
  XmMenuSystemPopdownProc		popdown;
  XmMenuSystemPopdownProc		buttonPopdown;
  XmMenuSystemReparentProc		reparentToTearOffShell;
  XmMenuSystemReparentProc		reparentToMenuShell;
  XmMenuSystemArmProc			arm;
  XmMenuSystemDisarmProc		disarm;
  XmMenuSystemTearOffArmProc		tearOffArm;
  XmMenuSystemEntryCallbackProc		entryCallback;
  XmMenuSystemUpdateHistoryProc		updateHistory;
  XmMenuSystemGetPostedFromWidgetProc	getLastSelectToplevel;
  XmMenuSystemPositionProc		position;
  XmMenuSystemUpdateBindingsProc	updateBindings;
  XmMenuSystemRecordPostFromWidgetProc	recordPostFromWidget;
  XmMenuSystemPopdownAllProc		popdownEveryone;
  XmMenuSystemChildFocusProc		childFocus;
  XmMenuSystemPopupPostedProc		getPopupPosted;
} XmMenuSystemTraitRec, *XmMenuSystemTrait;

/* XmTmenuSavvyTrait */

/* Version 0: initial release. */

typedef struct _XmMenuSavvyTraitRec
{
  int					version;		/* 0 */
  XmMenuSavvyDisableProc		disableCallback;
  XmMenuSavvyGetAcceleratorProc 	getAccelerator;
  XmMenuSavvyGetMnemonicProc		getMnemonic;
  XmMenuSavvyGetActivateCBNameProc	getActivateCBName;
} XmMenuSavvyTraitRec, *XmMenuSavvyTrait;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMenuT_H */
