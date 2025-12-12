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
#ifndef _XmDisplayP_h
#define _XmDisplayP_h

#include <Xm/DesktopP.h>
#include <Xm/VendorSEP.h>
#include <Xm/DropSMgr.h>
#include <Xm/Display.h>
#include <Xm/ScreenP.h>

/* A little incest */
#include <Xm/DragCP.h>
#include <Xm/VirtKeysP.h>

#include <Xm/TearOffP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Widget (*XmDisplayGetDisplayProc)(Display *);


typedef struct {
    XmDisplayGetDisplayProc GetDisplay;
    XtPointer               extension;
} XmDisplayClassPart;

/* 
 * we make it a appShell subclass so it can have it's own instance
 * hierarchy
 */
typedef struct _XmDisplayClassRec{
    CoreClassPart      		core_class;
    CompositeClassPart 		composite_class;
    ShellClassPart  		shell_class;
    WMShellClassPart   		wm_shell_class;
    VendorShellClassPart 	vendor_shell_class;
    TopLevelShellClassPart 	top_level_shell_class;
    ApplicationShellClassPart 	application_shell_class;
    XmDisplayClassPart		display_class;
} XmDisplayClassRec;

typedef struct _XmModalDataRec{
    Widget                      wid;
    XmVendorShellExtObject	ve;
    XmVendorShellExtObject	grabber;
    Boolean			exclusive;
    Boolean			springLoaded;
} XmModalDataRec, *XmModalData;

typedef struct {
    unsigned char		dragInitiatorProtocolStyle;
    unsigned char		dragReceiverProtocolStyle;

    unsigned char		userGrabbed; /* flag for menu vs dnd */

    WidgetClass			dragContextClass;
    WidgetClass			dropTransferClass;
    WidgetClass			dropSiteManagerClass;
    XmDragContext		activeDC;
    XmDropSiteManagerObject	dsm;
    Time			lastDragTime;
    Window			proxyWindow;

    XmModalData			modals;
    Cardinal			numModals;
    Cardinal			maxModals;
    XtPointer			xmim_info;

    String			bindingsString;
    XmVKeyBindingRec		*bindings;
    XKeyEvent			*lastKeyEvent;			 /* unused */
    unsigned char		keycode_tag[XmKEYCODE_TAG_SIZE]; /* unused */

    int				shellCount;
    XtPointer			displayInfo;	/* extension */
    XtPointer                   user_data;
    int                         motif_version ;
    XtEnum                      enable_warp ;
    Cardinal			num_bindings;
    XtCallbackList		dragStartCallback;
    XtCallbackList		noFontCallback;
    XtCallbackList		noRenditionCallback;
    Boolean			displayHasShapeExtension;

    XtEnum			enable_btn1_transfer ;
    Boolean			enable_button_tab ;
    Boolean			enable_etched_in_menu;
    Boolean			default_button_emphasis;
    Boolean			enable_toggle_color;
    Boolean			enable_toggle_visual;
    Boolean			enable_drag_icon;
    Boolean			enable_unselectable_drag;
    Boolean                     enable_thin_thickness;
    Boolean			enable_multi_key_bindings;
} XmDisplayPart, *XmDisplayPartPtr;

typedef struct _XmDisplayInfo {
  /* so much for information hiding */
  Cursor	SashCursor;		/* Sash.c */
  Widget	destinationWidget;	/* Dest.c */
  Cursor	TearOffCursor;		/* TearOff.c */
  XtPointer	UniqueStamp;		/* UniqueEvnt.c */
  XmExcludedParentPaneRec excParentPane;/* TearOff.c */
  unsigned short resetFocusFlag;	/* TravAct.c */
  Boolean	traversal_in_progress;  /* Traversal.c */
} XmDisplayInfo;

typedef struct _XmDisplayRec{
    CorePart 		core;
    CompositePart 	composite;
    ShellPart 		shell;
    WMShellPart		wm;
    VendorShellPart	vendor;
    TopLevelShellPart 	topLevel;
    ApplicationShellPart application;
    XmDisplayPart	display;
} XmDisplayRec;

externalref XmDisplayClassRec 	xmDisplayClassRec;

externalref String _Xm_MOTIF_DRAG_AND_DROP_MESSAGE ;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDisplayP_h */
/* DON'T ADD STUFF AFTER THIS #endif */

