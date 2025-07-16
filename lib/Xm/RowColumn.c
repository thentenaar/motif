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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: RowColumn.c /main/25 1998/07/22 15:41:49 mgreess $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include "XmI.h"
#include <Xm/BaseClassP.h>
#include <Xm/DisplayP.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/ShellP.h>
#include <X11/keysym.h>
#include <Xm/AtomMgr.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/DrawP.h>
#include <Xm/LabelP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuT.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/SeparatoGP.h>
#include <Xm/SeparatorP.h>
#include <Xm/TearOffBP.h>
#include <Xm/ToggleBGP.h>
#include <Xm/ToggleBP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/VirtKeysP.h>
#include "GeoUtilsI.h"
#include "GMUtilsI.h"
#include "LabelGI.h"
#include "ManagerI.h"
#include "MapEventsI.h"
#include "MenuProcI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "MessagesI.h"
#include "RCLayoutI.h"
#include "RCMenuI.h"
#include "RepTypeI.h"
#include "RowColumnI.h"
#include "ScreenI.h"
#include "TearOffI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "RCHookI.h"

/********    Static Function Declarations    ********/

static void Destroy(
                        Widget w) ;
static void ConstraintDestroy(
                        Widget w) ;
static void FixWidget(
                        XmRowColumnWidget m,
                        Widget w) ;
static Cardinal InsertPosition(
                        Widget w) ;
static void InsertChild(
                        Widget w) ;
static void DeleteChild(
                        Widget child) ;
static void ChangeManaged(
                        Widget wid) ;
static void Realize(
                        Widget wid,
                        XtValueMask *window_mask,
                        XSetWindowAttributes *window_attributes) ;
static Boolean DoEntryStuff(
                        XmRowColumnWidget old,
                        XmRowColumnWidget new_w) ;
static void DoSize(
                        XmRowColumnWidget old,
                        XmRowColumnWidget new_w) ;
static Boolean set_values_non_popup(
                        XmRowColumnWidget old,
                        XmRowColumnWidget new_w) ;
static Boolean set_values_popup(
                        XmRowColumnWidget old,
                        XmRowColumnWidget new_w) ;
static void set_values_passive_grab(
                        XmRowColumnWidget old,
                        XmRowColumnWidget new_w) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static XtGeometryResult QueryGeometry(
                        Widget wid,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *reply);
static XtGeometryResult GeometryManager(
                        Widget instigator,
                        XtWidgetGeometry *desired,
                        XtWidgetGeometry *allowed) ;
static char * GetRealKey(
                        XmRowColumnWidget rc,
                        char *str) ;
static void MenuBarInitialize(
                        XmRowColumnWidget bar) ;
static void PreparePostFromList(
                        XmRowColumnWidget rowcol) ;
static void PopupInitialize(
                        XmRowColumnWidget popup) ;
static void PulldownInitialize(
                        XmRowColumnWidget pulldown) ;
static void OptionInitialize(
                        XmRowColumnWidget option) ;
static void WorkAreaInitialize(
                        XmRowColumnWidget work) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void ConstraintInitialize(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean ConstraintSetValues(
                        Widget old,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Widget create(
                        Widget p,
                        char *name,
                        ArgList old_al,
                        Cardinal old_ac,
                        int type,
                        int is_radio) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass rcc) ;
static void InitializePrehook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean TraversalChildren(
                        Widget wid,
                        Widget **childList,
                        Cardinal *numChildren) ;
static void FixEventBindings(
                        XmRowColumnWidget m,
                        Widget w) ;
static Widget FindFirstManagedChild(
                       CompositeWidget m,
                        Boolean first_button) ;
static void Resize(
                        Widget wid) ;
static void Redisplay(
                        Widget w,
                        XEvent *event,
                        Region region) ;
static void FixVisual(
                        XmRowColumnWidget m,
                        Widget w) ;
static void FixCallback(
                        XmRowColumnWidget m,
                        Widget w) ;
static void ActionNoop(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void EventNoop(
                        Widget reportingWidget,
                        XtPointer data,
                        XEvent *event,
                        Boolean *cont) ;
static void MenuFocusIn(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void MenuFocusOut(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void MenuUnmap(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void MenuEnter(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void GadgetEscape(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void GetMnemonicCharSet(
                        Widget wid,
                        int resource,
                        XtArgVal *value) ;
static void GetMenuAccelerator(
                        Widget wid,
                        int resource,
                        XtArgVal *value) ;
static void GetMenuPost(
                        Widget wid,
                        int resource,
                        XtArgVal *value) ;
static void GetLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetTearOffTitle(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static XmNavigability WidgetNavigable(
                        Widget wid) ;

/********    End Static Function Declarations    ********/

/* Traits Declarations */

extern XmMenuSystemTraitRec _XmRC_menuSystemRecord;


/*
 * event translation tables for a menu widget, we use the parameters to
 * signal that this widget invoking the action proc is the menu, not a
 * child of the menu
 */

static XtTranslations menu_traversal_parsed;
#define menu_traversal_table 	_XmRowColumn_menu_traversal_table

static XtTranslations bar_parsed;
#define bar_table	_XmRowColumn_bar_table

static XtTranslations option_parsed;
#define option_table	_XmRowColumn_option_table

static XtTranslations menu_parsed;
#define menu_table 	_XmRowColumn_menu_table

/*
 * auto create child names
 */

#define TEAROFF_CONTROL "TearOffControl"
#define OPTION_LABEL "OptionLabel"
#define OPTION_BUTTON "OptionButton"
#define POPUP_PREFIX "popup_%s"

/*
 * default sizes
 */

#define DEFAULT_WIDTH 16
#define DEFAULT_HEIGHT 16

#define UNSET_POSTBUTTON	((unsigned int) -1)

/*
 * action binding table for row column widget
 */

static XtActionsRec actions [] =
{
    {"Help",                _XmManagerHelp},
    {"MenuHelp",            _XmMenuHelp},
    {"MenuBtnDown",	    _XmMenuBtnDown},
    {"MenuBtnUp",	    _XmMenuBtnUp},
    {"PulldownBtnDown",     _XmMenuBtnDown},
    {"PulldownBtnUp",       _XmMenuBtnUp},
    {"PopupBtnDown",        _XmMenuBtnDown},
    {"PopupBtnUp",          _XmMenuBtnUp},
    {"MenuBarBtnDown",      _XmMenuBtnDown},
    {"MenuBarBtnUp",        _XmMenuBtnUp},
    {"WorkAreaBtnDown",     _XmGadgetArm},
    {"WorkAreaBtnUp",       _XmGadgetActivate},

    {"MenuBarGadgetSelect", _XmMenuBarGadgetSelect},
    {"MenuGadgetTraverseCurrent", _XmMenuGadgetTraverseCurrent},
    {"MenuGadgetTraverseCurrentUp", _XmMenuGadgetTraverseCurrentUp},
    {"MenuGadgetDrag", _XmMenuGadgetDrag},

    {"FocusOut",            _XmMenuFocusOut},
    {"FocusIn",             _XmMenuFocusIn},
    {"Unmap",               _XmMenuUnmap},
    {"Noop",                ActionNoop},
    {"MenuTraverseLeft",    _XmMenuTraverseLeft},
    {"MenuTraverseRight",   _XmMenuTraverseRight},
    {"MenuTraverseUp",      _XmMenuTraverseUp},
    {"MenuTraverseDown",    _XmMenuTraverseDown},
    {"MenuEscape",	    _XmMenuEscape},

    {"MenuFocusIn",         MenuFocusIn},
    {"MenuFocusOut",        MenuFocusOut},
    {"MenuUnmap",           MenuUnmap},
    {"MenuEnter",           MenuEnter},

    {"MenuGadgetReturn",         _XmGadgetSelect},
    {"MenuGadgetEscape",         GadgetEscape},
    {"MenuGadgetTraverseLeft",   _XmRC_GadgetTraverseLeft},
    {"MenuGadgetTraverseRight",  _XmRC_GadgetTraverseRight},
    {"MenuGadgetTraverseUp",     _XmRC_GadgetTraverseUp},
    {"MenuGadgetTraverseDown",   _XmRC_GadgetTraverseDown}
};


/*
 * define the resourse stuff for a rowcolumn widget
 */

static XtResource resources[]  =
{
    {   XmNresizeWidth,
        XmCResizeWidth,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.resize_width),
        XmRImmediate,
        (XtPointer) TRUE
    },
    {   XmNresizeHeight,
        XmCResizeHeight,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.resize_height),
        XmRImmediate,
        (XtPointer) TRUE
    },
    {   XmNwhichButton,
        XmCWhichButton,
        XmRWhichButton,
        sizeof(unsigned int),
        XtOffsetOf( struct _XmRowColumnRec, row_column.postButton),
        XmRImmediate,
        (XtPointer) UNSET_POSTBUTTON,
    },
    {   XmNmenuPost,
        XmCMenuPost,
        XmRString,
        sizeof(String),
        XtOffsetOf( struct _XmRowColumnRec, row_column.menuPost),
        XmRString,
        NULL,
    },
    {   XmNadjustLast,
        XmCAdjustLast,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.adjust_last),
        XmRImmediate,
        (XtPointer) TRUE,
    },
    {   XmNmarginWidth,
        XmCMarginWidth,
        XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.margin_width),
        XmRImmediate,
        (XtPointer) XmINVALID_DIMENSION
    },
    {   XmNmarginHeight,
        XmCMarginHeight,
        XmRVerticalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.margin_height),
        XmRImmediate,
        (XtPointer) XmINVALID_DIMENSION
    },
    {   XmNentryCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_callback),
        XmRCallback,
        NULL
    },
    {   XmNmapCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmRowColumnRec, row_column.map_callback),
        XmRCallback,
        NULL
    },
    {   XmNunmapCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmRowColumnRec, row_column.unmap_callback),
        XmRCallback,
        NULL
    },
    {   XmNorientation,
        XmCOrientation,
        XmROrientation,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.orientation),
        XmRImmediate,
        (XtPointer)XmNO_ORIENTATION
    },
    {   XmNspacing,
        XmCSpacing,
        XmRHorizontalDimension,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.spacing),
        XmRImmediate,
        (XtPointer) XmINVALID_DIMENSION
    },
    {   XmNentryBorder,         /* border width of all the */
        XmCEntryBorder,         /* entries, always uniform */
        XmRHorizontalDimension,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_border),
        XmRImmediate,
	(XtPointer) 0
    },
    {   XmNisAligned,           /* T/F, do all entrys have */
        XmCIsAligned,           /* same alignment */
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.do_alignment),
        XmRImmediate,
        (XtPointer)TRUE
    },
    {   XmNentryAlignment,          /* how entries are to be */
        XmCAlignment,               /* aligned */
        XmRAlignment,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_alignment),
        XmRImmediate,
        (XtPointer)XmALIGNMENT_BEGINNING
    },
    {   XmNadjustMargin,            /* should all entries have */
        XmCAdjustMargin,            /* the same label margins */
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.adjust_margin),
        XmRImmediate,
        (XtPointer)TRUE
    },
    {   XmNpacking,         /* how to pack menu entries */
        XmCPacking,         /* Tight, Column, None */
        XmRPacking,
        sizeof (unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.packing),
        XmRImmediate,
        (XtPointer)XmNO_PACKING
    },
    {   XmNnumColumns,          /* if packing columnar then */
        XmCNumColumns,          /* this is how many */
        XmRShort,
        sizeof (short),
        XtOffsetOf( struct _XmRowColumnRec, row_column.num_columns),
        XmRImmediate,
        (XtPointer)1
    },
    {   XmNradioBehavior,           /* should the menu enforce */
        XmCRadioBehavior,           /* toggle button exclusivity, */
        XmRBoolean,             /* ie, radio buttons */
        sizeof (Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.radio),
        XmRImmediate,
        (XtPointer)FALSE
    },
    {   XmNradioAlwaysOne,          /* should there always be one */
        XmCRadioAlwaysOne,          /* radio button on. */
        XmRBoolean,
        sizeof (Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.radio_one),
        XmRImmediate,
        (XtPointer)TRUE
    },
    {   XmNisHomogeneous,           /* should we enforce the */
        XmCIsHomogeneous,           /* rule that only one type of */
        XmRBoolean,             /* entry is allow in the menu */
        sizeof (Boolean),
        XtOffsetOf( struct _XmRowColumnRec, row_column.homogeneous),
        XmRImmediate,
        (XtPointer)FALSE
    },
    {   XmNentryClass,              /* if enforcing homogeneous */
        XmCEntryClass,              /* menu, this tells the class */
        XmRWidgetClass,
        sizeof (WidgetClass),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_class),
        XmRWidgetClass,
        (XtPointer) NULL
    },
    {   XmNrowColumnType,       /* warning - non-standard resource */
        XmCRowColumnType,
        XmRRowColumnType,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.type),
        XmRImmediate,
        (XtPointer)XmWORK_AREA
    },
    {   XmNmenuHelpWidget,          /* which widget is the help */
        XmCMenuWidget,              /* widget */
        XmRMenuWidget,
        sizeof (Widget),
        XtOffsetOf( struct _XmRowColumnRec, row_column.help_pushbutton),
        XmRImmediate,
        (XtPointer)NULL
    },
    {   XmNlabelString,               /* option menus have a label */
        XmCXmString,
        XmRXmString,
        sizeof(XmString),
        XtOffsetOf( struct _XmRowColumnRec, row_column.option_label),
        XmRXmString,
        (XtPointer)NULL
    },
    {   XmNsubMenuId,               /* option menus have built-in */
        XmCMenuWidget,              /* submenu */
        XmRMenuWidget,
        sizeof (Widget),
        XtOffsetOf( struct _XmRowColumnRec, row_column.option_submenu),
        XmRImmediate,
        (XtPointer)NULL
    },
    {   XmNmenuHistory,         /* pretend a subwidget fired */
        XmCMenuWidget,              /* off, used to pre-load the */
        XmRMenuWidget,              /* option menu and popup menu */
        sizeof (Widget),            /* mouse/muscle memory */
        XtOffsetOf( struct _XmRowColumnRec, row_column.memory_subwidget),
        XmRImmediate,
        (XtPointer)NULL
    },
    {   XmNpopupEnabled,            /* are accelerator enabled */
        XmCPopupEnabled,            /* in the popup menu? */
        XmREnum,
        sizeof (XtEnum),
        XtOffsetOf( struct _XmRowColumnRec, row_column.popup_enabled),
        XmRImmediate,
        (XtPointer) 1
    },
    {   XmNmenuAccelerator,         /* popup menu accelerator */
        XmCAccelerators,
        XmRString,
        sizeof (char *),
        XtOffsetOf( struct _XmRowColumnRec, row_column.menu_accelerator),
        XmRString,
        (XtPointer) ""
    },
    {   XmNmnemonic,                /* option menu mnemonic */
        XmCMnemonic,
        XmRKeySym,
        sizeof (KeySym),
        XtOffsetOf( struct _XmRowColumnRec, row_column.mnemonic),
        XmRImmediate,
        (XtPointer) XK_VoidSymbol
    },
    {
	XmNmnemonicCharSet,
	XmCMnemonicCharSet,
	XmRString,
	sizeof(XmStringCharSet),
	XtOffsetOf( struct _XmRowColumnRec,row_column.mnemonicCharSet),
	XmRImmediate,
	(XtPointer) XmFONTLIST_DEFAULT_TAG
    },
    {
        XmNshadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _XmRowColumnRec, manager.shadow_thickness),
	XmRImmediate,
	(XtPointer) XmINVALID_DIMENSION
    },
    {
	XmNpostFromList,
	XmCPostFromList,
	XmRWidgetList,
	sizeof (Widget *),
	XtOffsetOf( struct _XmRowColumnRec, row_column.postFromList),
	XmRWidgetList,
	(XtPointer) NULL,
    },
    {
        XmNpostFromCount,
	XmCPostFromCount,
	XmRInt,
	sizeof (int),
	XtOffsetOf( struct _XmRowColumnRec, row_column.postFromCount),
	XmRImmediate,
	(XtPointer) -1
    },
    {
	XmNnavigationType,
	XmCNavigationType,
	XmRNavigationType,
	sizeof (unsigned char),
	XtOffsetOf( struct _XmManagerRec, manager.navigation_type),
	XmRImmediate,
	(XtPointer) XmDYNAMIC_DEFAULT_TAB_GROUP,
    },
    {   XmNentryVerticalAlignment,          /* how entries are to be */
        XmCVerticalAlignment,               /* aligned */
        XmRVerticalAlignment,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_vertical_alignment),
        XmRImmediate,
        (XtPointer)XmALIGNMENT_CENTER
    },
    {   XmNtearOffModel,
        XmCTearOffModel,
        XmRTearOffModel,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmRowColumnRec, row_column.TearOffModel),
        XmRImmediate,
        (XtPointer)XmTEAR_OFF_DISABLED
    },
    {   XmNtearOffMenuActivateCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmRowColumnRec, row_column.tear_off_activated_callback),
        XmRCallback,
        NULL
    },
    {   XmNtearOffMenuDeactivateCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmRowColumnRec, row_column.tear_off_deactivated_callback),
        XmRCallback,
        NULL
    },
    {
	XtNinsertPosition,
	XtCInsertPosition,
	XtRFunction,
	sizeof(XtOrderProc),
        XtOffsetOf(XmRowColumnWidgetRec, composite.insert_position),
	XtRImmediate,
	(XtPointer) InsertPosition
    },
    {
        XmNtearOffTitle,
	XmCTearOffTitle,
	XmRXmString,
	sizeof (XmString),
	XtOffsetOf(XmRowColumnWidgetRec, row_column.tear_off_title),
	XmRString,
	(XtPointer) NULL
	},
};

static XtResource constraint_resources[] = {
    {
       XmNpositionIndex,
       XmCPositionIndex,
       XmRShort,
       sizeof(short),
       XtOffsetOf(XmRowColumnConstraintRec, row_column.position_index),
       XmRImmediate,
       (XtPointer) XmLAST_POSITION
    },
};


static XmSyntheticResource syn_resources[] =
{
    {
        XmNmnemonicCharSet,
        sizeof(XmStringCharSet),
        XtOffsetOf( struct _XmRowColumnRec,row_column.mnemonicCharSet),
        GetMnemonicCharSet,
        NULL,
    },
    {
        XmNmenuAccelerator,
        sizeof(char *),
        XtOffsetOf( struct _XmRowColumnRec, row_column.menu_accelerator),
        GetMenuAccelerator,
        NULL,
    },
    {
        XmNmenuPost,
        sizeof(String),
        XtOffset(XmRowColumnWidget, row_column.menuPost),
        GetMenuPost,
        NULL,
    },
    {   XmNlabelString,               /* option menus have a label */
        sizeof(XmString),
        XtOffsetOf( struct _XmRowColumnRec, row_column.option_label),
        GetLabelString,
        NULL,
    },
    {   XmNspacing,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec,row_column.spacing),
        XmeFromHorizontalPixels,
        XmeToHorizontalPixels,
    },
    {   XmNmarginHeight,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.margin_height),
        XmeFromVerticalPixels,
        XmeToVerticalPixels,
    },
    {   XmNmarginWidth,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.margin_width),
        XmeFromHorizontalPixels,
        XmeToHorizontalPixels,
    },
    {
        XmNentryBorder,
        sizeof(Dimension),
        XtOffsetOf( struct _XmRowColumnRec, row_column.entry_border),
        XmeFromHorizontalPixels,
        XmeToHorizontalPixels,
    },
    {   XmNtearOffTitle,
        sizeof(XmString),
        XtOffsetOf( struct _XmRowColumnRec, row_column.tear_off_title),
        GetTearOffTitle,
        NULL,
    },
};



/*
 * static initialization of the row column widget class record, must do
 * each field
 */

static XmBaseClassExtRec       baseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    InitializePrehook,                        /* initialize prehook   */
    XmInheritSetValuesPrehook,                /* set_values prehook   */
    InitializePosthook,                       /* initialize posthook  */
    XmInheritSetValuesPosthook,               /* set_values posthook  */
    XmInheritClass,                           /* secondary class      */
    XmInheritSecObjectCreate,                 /* creation proc        */
    XmInheritGetSecResData,                   /* getSecResData        */
    {0},                                      /* fast subclass        */
    XmInheritGetValuesPrehook,                /* get_values prehook   */
    XmInheritGetValuesPosthook,               /* get_values posthook  */
    NULL,                                     /* classPartInitPrehook */
    NULL,                                     /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    WidgetNavigable,                          /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};

static XmManagerClassExtRec managerClassExtRec = {
    NULL,
    NULLQUARK,
    XmManagerClassExtVersion,
    sizeof(XmManagerClassExtRec),
    TraversalChildren,                              /* traversal_children */
    XmInheritObjectAtPointProc
};

externaldef(xmrowcolumnclassrec) XmRowColumnClassRec xmRowColumnClassRec =
{
    {                   /* core class record */
        (WidgetClass)&xmManagerClassRec, /* superclass ptr */
        "XmRowColumn",                 /* class_name */
        sizeof (XmRowColumnRec),       /* size of widget instance */
        ClassInitialize,               /* class init proc */
        ClassPartInitialize,           /* class part init */
        FALSE,                         /* class is not init'ed */
        Initialize,                    /* widget init proc*/
        _XmRCColorHook,                          /* init_hook proc */
        Realize,                       /* widget realize proc */
        actions,                       /* class action table */
        XtNumber (actions),
        resources,                     /* this class's resource list */
        XtNumber (resources),          /*  "     " resource_count */
        NULLQUARK,                     /* xrm_class            */
        TRUE,                          /* don't compress motion */
        XtExposeCompressMaximal,       /* do compress exposure */
        FALSE,                         /* don't compress enter-leave */
        FALSE,                         /* no VisibilityNotify */
        Destroy,                       /* class destroy proc */
        Resize,                        /* class resize proc */
        Redisplay,                     /* class expose proc */
        SetValues,                     /* class set_value proc */
        NULL,                          /* set_value_hook proc */
        XtInheritSetValuesAlmost,      /* set_value_almost proc */
        NULL,                          /* get_values_hook */
        NULL,                          /* class accept focus proc */
        XtVersion,                     /* current version */
        NULL,                          /* callback offset list */
        NULL,                          /* translation table */
        QueryGeometry,                 /* query geo proc */
        NULL,                          /* display accelerator */
        (XtPointer)&baseClassExtRec,	/* extension */
    },
    {                  /* composite class record */
        GeometryManager,               /* childrens geo mgr proc */
        ChangeManaged,                 /* set changed proc */
        InsertChild,                   /* add a child */
        DeleteChild,                   /* remove a child */
        NULL,                          /* extension */
    },
    {                  /* constraint class record */
        constraint_resources,    /* constraint resources */
        XtNumber(constraint_resources),    /* constraint resource_count */
        sizeof(XmRowColumnConstraintRec),  /* constraint_size */
        ConstraintInitialize,    /* initialize */
        ConstraintDestroy,       /* destroy */
	ConstraintSetValues,     /* set_values */
        NULL,                    /* extension */
    },
    {                  /* manager class record */
        XtInheritTranslations,   /* translations */
        syn_resources,           /* syn_resources */
        XtNumber(syn_resources), /* num_syn_resources */
        NULL,                    /* syn_constraint_resources */
        0,                       /* num_syn_constraint_resources */
        XmInheritParentProcess,  /* parent_process         */
        (XtPointer)&managerClassExtRec,     /* extension */

    },
    {                  /* row column class record */
        _XmRCMenuProcedureEntry, /* proc to interface with menu widgets */
        _XmRCArmAndActivate,     /* proc to arm&activate menu */
        _XmMenuTraversalHandler, /* traversal handler */
        NULL,                    /* extension */
    }
};


/*
 * now make a public symbol that points to this class record
 */

externaldef(xmrowcolumnwidgetclass) WidgetClass xmRowColumnWidgetClass =
   (WidgetClass) &xmRowColumnClassRec;


/*
 * Destroy the widget, and any subwidgets there are
 */
static void
Destroy(
        Widget w )
{
   XmRowColumnWidget m = (XmRowColumnWidget) w;
   Widget topManager;
   int i;

   if (RC_TornOff(m))
   {
      if (!XmIsMenuShell(XtParent(m)))
      {
	 _XmDestroyTearOffShell(XtParent(m));

	 /* See TearOff.c:  Possible to destroy the tearoff
	  * while its still up.  It doesn't have to be dismissed
	  * by the user as was assumed originally.
	  * So if its destroyed, remove the callbacks as is
	  * done in _XmDismissTearOff().
	  */
         XtRemoveAllCallbacks (
		m->row_column.tear_off_lastSelectToplevel,
		XmNdestroyCallback);

	 /* Quick switch the parent for MenuShell's DeleteChild to work.
	  * We're taking for granted that the Transient shell's deletechild
	  * has already done everything it needs to do
	  */
	 m->core.parent = RC_ParentShell(m);

	 if (XmIsMenuShell(RC_ParentShell(m))) {
	    XtWidgetProc delete_child;
	    _XmProcessLock();
	    delete_child = ((CompositeWidgetClass)
	    			RC_ParentShell(m)->core.widget_class)->
	       			composite_class.delete_child;
	    _XmProcessUnlock();
	    (*delete_child)((Widget)m);
	 }
      } else
         if (RC_ParentShell(m))
	 {
	    _XmDestroyTearOffShell(RC_ParentShell(m));
	 }
   }

   if (RC_TearOffControl(m))
   {
      XtDestroyWidget(RC_TearOffControl(m));
      /* Unnecessary... RC_TearOffControl(new_w) = NULL; */
   }

   /*
    * If we had added any event handlers for processing accelerators or
    * mnemonics, then we must remove them now.
    */
   if (IsPopup(m))
   {
      if (RC_PopupEnabled(m))
	  _XmRC_RemovePopupEventHandlers (m);

      /* If a timer is present, we're going to take for granted that the
       * pending grab belongs to this popup because no other popup can have
       * intervened during this grab.
       */
      if (m->row_column.popup_workproc)
      {
	 XtRemoveWorkProc(m->row_column.popup_workproc);
	 /* Ungrab and reset the ButtonEventStatus record */
	 _XmRC_PostTimeOut( (XtPointer) m );
      }

      /* Remove attach_widget destroy callbacks to update this popup's
       * postFromList
       */
      for (i=0; i < m->row_column.postFromCount; i++) {
	if (! m->row_column.postFromList[i]->core.being_destroyed)
	  {
	    XtRemoveCallback(m->row_column.postFromList[i], XtNdestroyCallback,
			     (XtCallbackProc)_XmRC_RemoveFromPostFromListOnDestroyCB, (XtPointer)m);
	  }
      }
   }
   else if (IsOption(m) || IsBar(m))
   {
      /* Remove it from the associated widget */
      _XmRCGetTopManager ((Widget) m, &topManager);
      XtRemoveEventHandler(topManager, KeyPressMask|KeyReleaseMask,
			   False, _XmRC_KeyboardInputHandler, m);

      /* Remove it from us */
      XtRemoveEventHandler( (Widget) m, KeyPressMask|KeyReleaseMask,
			   False, _XmRC_KeyboardInputHandler, (XtPointer) m);

   }

   /*
    * If we're still connected to a cascade button, then we need to break
    * that link, so that the cascade button doesn't attempt to reference
    * us again, and also so that accelerators and mnemonics can be cleared up.
    */
   else
   {
      Arg args[1];

      for (i=m->row_column.postFromCount-1; i >= 0; i--)
	  /* reverse order; side-effect below is to change list
	     in _XmRC_RemoveFromPostFromList */
      {
	  XtSetArg (args[0], XmNsubMenuId, NULL);
	  XtSetValues (m->row_column.postFromList[i], args, 1);
      }
   }

   if ((IsPopup(m) && RC_PopupEnabled(m))  ||
       (IsBar(m) && RC_MenuAccelerator(m)) ||
       (IsOption(m) && (RC_Mnemonic(m) != XK_VoidSymbol)))
   {
      Cardinal num_children;

      /*
       * By the time we reach here, our children are destroyed, but
       * the children's list is bogus; so we need to temporarily zero
       * out our num_children field, so _XmRC_DoProcessMenuTree() will not
       * attempt to process our children.
       */
      num_children = m->composite.num_children;
      m->composite.num_children = 0;
      _XmRC_DoProcessMenuTree((Widget) m, XmDELETE);
      m->composite.num_children = num_children;
   }

   /* free postFromList for popups and pulldowns, zero postFromCount
    * Moved after DoProcessMenuTree() so RemoveFromKeyboardList() can
    * detect if this was a shared menupane.
    */
   if (IsPopup(m) || IsPulldown(m))
   {
      XtFree ( (char *) m->row_column.postFromList);
      m->row_column.postFromCount = 0;
   }

   /* After _XmRC_DoProcessMenuTree() so RemoveFromKeyboardList() works properly. */
   XtFree((char *) MGR_KeyboardList(m));

   /* Free menuPost string. */
   if (!IsPulldown(m) && RC_MenuPost(m)) XtFree(RC_MenuPost(m));

   /* Free accelerator string */
   if ((IsPopup(m) && RC_MenuAccelerator(m)) ||
       (IsBar(m) && RC_MenuAccelerator(m)))
      XtFree(RC_MenuAccelerator(m));
}


/*
 * Destroy any keyboard grabs/entries for the child
 */
static void
ConstraintDestroy(
        Widget w )
{
   if (!XtIsRectObj(w)) return;

   _XmRC_DoProcessMenuTree(w, XmDELETE);
}


/*
 * do all the stuff needed to make a subwidget of a menu work correctly
 */
static void
FixWidget(
        XmRowColumnWidget m,
        Widget w )
{
    /*
     * now patchup the event binding table for the subwidget so that
     * it acts the way we want it to
     */

    FixEventBindings (m, w);

    /*
     * and patch the visual aspects of the subwidget
     */

    FixVisual (m, w);

    /*
     * and patch the callback list so that we will be called whenever
     * he fires off
     */

    FixCallback (m, w);
}



static Cardinal
InsertPosition(Widget w)
{
    XmRowColumnWidget rc = (XmRowColumnWidget) XtParent(w);

    /* if a positionIndex has been specified and is a correct value,
     * use it as the new position for this child
     */
    if (RCIndex(w) != XmLAST_POSITION) {
      if ((RCIndex(w) >= 0) && (RCIndex(w) <= rc->composite.num_children)) {
          return RCIndex(w) ;
      }
    }

    /* otherwise, return the default (we'll set its value in the instance field
       and reset the others index values for syblins at the end of InsertChild */
    /* Note: the tearoffcontrol doesn't go thru this function, since
       InsertChild filter its case */
    return rc->composite.num_children ;
}


/*
 * Add a child to this row column widget
 */
static void
InsertChild(
        Widget w )
{
    XmRowColumnWidget m = (XmRowColumnWidget) XtParent(w);
    Widget *p ;
    int i ;

    /* Special case the hidden tear off control */
    if (RC_FromInit(m))
    {
	/* can't let XmLAST_POSITION in, this value is used in
	 * geometry manager to denote a special case
	 */
	RCIndex(w) = 0 ;
	return;
    }

    if (!IsWorkArea(m) /* it's a menu */ &&
        !(XmeTraitGet((XtPointer) XtClass(w), XmQTmenuSavvy) != NULL))
      XmeWarning( (Widget) m, WrongMenuChildMsg);

    /*
     * if the rowcolumn is homogeneous, make sure that class matches
     * the entry class.  Three exceptions are made:  1) if the entry class is
     * CascadeButton or CascadeButtonGadget, either of those classes are
     * allowed. 2) if the entry class is ToggleButton or ToggleButtonGadget,
     * either of those classes are allowed.  3) if the entry class is
     * PushButton or PushButtonGadget, either of those classes are allowed.
     */
    if (XtIsRectObj(w) && RC_IsHomogeneous(m) && RC_EntryClass(m) &&
	(RC_EntryClass(m) != XtClass(w)))
    {
      /* CR 7807: using _XmIsFastSubclass checks is subtly wrong???
       *	If an application asks for a specific subclass of
       * XmToggleButtonWidget, then using plain XmToggleButtonGadget
       * children should generate warnings.  But this is a very rare
       * condition, so we'll ignore it unless someone complains.  The
       * benefit of not linking in all the other widgets is significant.
       */

      if (!((_XmIsFastSubclass(RC_EntryClass(m), XmCASCADE_BUTTON_BIT) &&
	     XmIsCascadeButtonGadget(w)) ||
	    (_XmIsFastSubclass(RC_EntryClass(m), XmCASCADE_BUTTON_GADGET_BIT) &&
	     XmIsCascadeButton(w)) ||
	    (_XmIsFastSubclass(RC_EntryClass(m), XmTOGGLE_BUTTON_BIT) &&
	     XmIsToggleButtonGadget(w)) ||
	    (_XmIsFastSubclass(RC_EntryClass(m), XmTOGGLE_BUTTON_GADGET_BIT) &&
	     XmIsToggleButton(w)) ||
	    (_XmIsFastSubclass(RC_EntryClass(m), XmPUSH_BUTTON_BIT) &&
	     XmIsPushButtonGadget(w)) ||
	    (_XmIsFastSubclass(RC_EntryClass(m), XmPUSH_BUTTON_GADGET_BIT) &&
	     XmIsPushButton(w))))
       {
	  XmeWarning( (Widget) m, WrongChildMsg);
       }
    }

    /*
     * use composite class insert proc to do all the dirty work
     */
    {
       XtWidgetProc insert_child;
       _XmProcessLock();
       insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)->
				composite_class.insert_child;
       _XmProcessUnlock();
       (*insert_child)(w);
    }

    /*
     * now change the subwidget so that it acts the way we want it to
     */
    FixWidget (m, w);

    /*
     * re-set the correct positionIndex values for everybody if
     * the new kid has been inserted in the list instead of put at the end
     */
    if(    RCIndex( w) == XmLAST_POSITION    )
      {
        RCIndex( w) = m->composite.num_children - 1 ;
      }
    if (RCIndex(w) != (m->composite.num_children - 1))
      {
        i = RCIndex( w) ;
        p = m->composite.children + i ;
        while(    ++i < m->composite.num_children    )
          {
            ++p ;
            RCIndex(*p) = i ;
          }
      }
    /*
     * if in a torn off menu pane, then add the event handlers for tear offs
     */
    if (RC_TornOff(m) && !XmIsMenuShell(XtParent(m)))
	_XmAddTearOffEventHandlers ((Widget) m);

    /*
     * Fix for CR 5401 - If the RowColumn is a radiobox, check to see if the
     *		     menu history has been set.  If it has, the user has
     *		     forced this selection or another child has already been
     *		     managed.  If not, then this is the first managed child
     *		     and it should be loaded into the menu history.
     */
    if (IsRadio(m) && (RC_MemWidget(m) == (Widget) NULL))
      RC_MemWidget(m) = w;

    return;
}

/*
 * "child" is no longer a valid memory widget (menu history).  We must
 * reset any option menus who are currently referring it.  A recursive
 * search up the widget tree is necessary in case shared menupanes are
 * utilized.
 */
static void
ResetMatchingOptionMemWidget(
	XmRowColumnWidget menu,
	Widget child )
{
   int i;

   if (IsPulldown(menu))
   {
      for (i=0; i < menu->row_column.postFromCount; i++)
      {
         ResetMatchingOptionMemWidget(
	    (XmRowColumnWidget) XtParent(menu->row_column.postFromList[i]),
	    child);
      }
   }
   else if (IsOption(menu) && (child == RC_MemWidget(menu)))
      {
	 Widget cb;

	 /* Fix CR 7609 */
 	 if (RC_OptionSubMenu(menu) && RC_MemWidget(RC_OptionSubMenu(menu))) {
 	    RC_MemWidget(menu) = RC_MemWidget(RC_OptionSubMenu(menu));
 	 } else {
	   RC_MemWidget(menu) =
	     FindFirstManagedChild((CompositeWidget) RC_OptionSubMenu(menu),
				   True);
	   /* For what it's worth - nothing in SharedMenupanes */
	   if (RC_OptionSubMenu(menu))
	     RC_MemWidget(RC_OptionSubMenu(menu)) = RC_MemWidget(menu);
	 }
	 if ((cb = XmOptionButtonGadget( (Widget) menu)) != NULL)
	    _XmRC_UpdateOptionMenuCBG (cb, RC_MemWidget(menu));
      }
}

/*
 * delete a single widget from a parent widget
 */
static void
DeleteChild(
        Widget child )
{
   XmRowColumnWidget m = (XmRowColumnWidget) XtParent(child);
   Widget *p ;
   int i ;

   /* Fix CR 7982,  tear off control not in child list,  so ignore */
    if (child == RC_TearOffControl(m) ) return;

    if (child == RC_HelpPb (m))
       RC_HelpPb (m) = NULL;

    else if (child == RC_MemWidget(m))
      RC_MemWidget(m) = NULL;

    /*
     * If this child is in a top level menupane, then we want to remove
     * the event handler we added for catching keyboard input.
     */
    if (XtIsWidget(child) &&
	((IsPopup(m) || IsBar(m) || IsPulldown(m)) &&
	  XmIsLabel(child) && (child->core.widget_class != xmLabelWidgetClass)))
    {
	   XtRemoveEventHandler(child, KeyPressMask|KeyReleaseMask, False,
				_XmRC_KeyboardInputHandler, m);
    }

    /*
     * use composite class delete proc to do all the dirty work
     */
    {
       XtWidgetProc delete_child;
       _XmProcessLock();
       delete_child = ((CompositeWidgetClass)compositeWidgetClass)->
				composite_class.delete_child;
       _XmProcessUnlock();
       (*delete_child)(child);
    }

   /*
    * Re-set the correct positionIndex values for everybody if
    * the new kid was not deleted from the end of the list.
    * Composite class delete_child has already decremented num_chidren!
    */
   if (RCIndex(child) != m->composite.num_children)
     ForAllChildren (m, i, p)
       {
	 RCIndex(*p) = i ;
       }

   /* If this child is any option menu's menu history, reset
    * that option menu's menu history.
    */
   ResetMatchingOptionMemWidget(m, child);
}


/*
 * The set of our managed children changed, so maybe change the size of the
 * row column widget to fit them; there is no instigator of this change, and
 * ignore any dimensional misfit of the row column widget and the entries,
 * which is a result of our geometry mgr being nasty.  Get it laid out.
 */
static void
ChangeManaged(
        Widget wid )
{
  XmRowColumnWidget m = (XmRowColumnWidget) wid ;
  Widget  *q;
  int i;
  Dimension w = 0;
  Dimension h = 0;
  Boolean any_changed = FALSE;

  /*
   * We have to manage the "was_managed" field of the
   * constraint record.
   */

  ForAllChildren(m, i, q)
    {
      if (WasManaged(*q) != IsManaged(*q))
	{
	  _XmRC_ProcessSingleWidget(*q, IsManaged(*q) ? XmADD: XmDELETE);
	  any_changed = TRUE;
	}

      WasManaged(*q) = IsManaged(*q);
    }

  if (RC_TearOffControl(m))
    {
      if (WasManaged(RC_TearOffControl(m)) != IsManaged(RC_TearOffControl(m)))
	any_changed = TRUE;

      WasManaged(RC_TearOffControl(m)) = IsManaged(RC_TearOffControl(m));
    }

  if (!any_changed)
    {
      /* Must have been a popup child -- we don't really care */
      return;
    }

  if ((PackColumn(m) && (IsVertical(m) || IsHorizontal(m))) ||
      (PackTight(m) && IsHorizontal(m)))
    {
      ForManagedChildren(m, i, q)
	{
	  if (XmIsGadget(*q) || XmIsPrimitive(*q))
	    {
	      XmBaselineMargins textMargins;

	      textMargins.margin_top = SavedMarginTop(*q);
	      textMargins.margin_bottom = SavedMarginBottom(*q);
	      _XmRC_SetOrGetTextMargins(*q, XmBASELINE_SET, &textMargins);

	    }
	}
    }

  _XmRCDoMarginAdjustment (m);

  /*
   * find out what size we need to be with the current set of kids
   */
  _XmRCPreferredSize (m, &w, &h);

  /*
   * now decide if the menu needs to change size
   */
  if ((w != XtWidth (m)) || (h != XtHeight (m)))
    {
      XtWidgetGeometry menu_desired;
      menu_desired.request_mode = 0;

      if (w != XtWidth (m))
	{
	  menu_desired.width =  w;
	  menu_desired.request_mode |= CWWidth;
	}

      if (h != XtHeight (m))
	{
	  menu_desired.height = h;
	  menu_desired.request_mode |= CWHeight;
	}

      /* use a function that always accepts Almost */
      _XmMakeGeometryRequest( (Widget) m, &menu_desired);
    }

  /*
   * if we get to here the row column widget has been changed and his
   * window has been resized, so effectively we need to do a Resize.
   */

  _XmRCAdaptToSize (m, NULL, NULL);

  /*	Clear shadow if necessary. */

  if (m->row_column.old_shadow_thickness)
    _XmClearShadowType( (Widget) m, m->row_column.old_width,
		       m->row_column.old_height,
		       m->row_column.old_shadow_thickness, 0);

  /* and redraw it for shrinking size */
  if (XtIsRealized ((Widget)m) && m->manager.shadow_thickness )
    XmeDrawShadows (XtDisplay (m), XtWindow (m),
		    m->manager.top_shadow_GC,
		    m->manager.bottom_shadow_GC,
		    0, 0, m->core.width, m->core.height,
		    m->manager.shadow_thickness,
		    XmSHADOW_OUT);

  m->row_column.old_width = m->core.width;
  m->row_column.old_height = m->core.height;
  m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

  XmeNavigChangeManaged( (Widget) m);
}


/*
 * make the row column widget appear
 */
static void
Realize(
        Widget wid,
        XtValueMask *window_mask,
        XSetWindowAttributes *window_attributes )
{
  XmRowColumnWidget m = (XmRowColumnWidget) wid ;

  if (IsOption(m))
    {
      XmRowColumnWidget sm = (XmRowColumnWidget) RC_OptionSubMenu(m);
      Dimension w=0, h=0;

      if (!IsNull(sm))
	{
	  if (RC_MemWidget(m))
	    {
	      Widget cb;

	      /* Set the Option Menu's Cascade Button */
	      if ((cb = XmOptionButtonGadget( (Widget) m)) != NULL)
		_XmRC_UpdateOptionMenuCBG (cb, RC_MemWidget(m));
	    }
	  else /* if there is no memory widget, set it up */
	    ResetMatchingOptionMemWidget(m, NULL);

	  /* find out what size we need to be */
	  _XmRCPreferredSize (m, &w, &h);

	  /* now decide if the menu needs to change size */
	  if ((w != XtWidth (m)) || (h != XtHeight (m)))
	    {
	      XtWidgetGeometry menu_desired;

	      menu_desired.request_mode = 0;

	      if (w != XtWidth (m))
		{
		  menu_desired.width = w;
		  menu_desired.request_mode |= CWWidth;
		}

	      if (h != XtHeight (m))
		{
		  menu_desired.height = h;
		  menu_desired.request_mode |= CWHeight;
		}
	      /* use a function that always accepts Almost */
	      _XmMakeGeometryRequest( (Widget) m, &menu_desired);
	    }

	  _XmRCAdaptToSize (m, NULL, NULL);
	}
    }

   /* fix menu window so that any button down is OwnerEvent true. */
   if (!IsWorkArea(m))
   {
      /*
       * Originally, we simply set the OwnerGrabButtonMask in our
       * event mask.  Unfortunately, if the application ever modifies
       * our translations or adds an event handler which caused the
       * intrinsics to regenerate our X event mask, this bit was
       * lost.  So .. we add a dummy event handler for this mask bit,
       * thus guaranteeing that it is always part of our event mask.
       */
      window_attributes->event_mask |= OwnerGrabButtonMask;
      XtAddEventHandler( (Widget) m, OwnerGrabButtonMask, False,
                                                              EventNoop, NULL);
   }

   /*
    * Don't propagate events for row column widgets
    * and set bit gravity to NW
    */
   (*window_mask) |= CWDontPropagate | CWBitGravity;
   window_attributes->bit_gravity = NorthWestGravity;

   window_attributes->do_not_propagate_mask = ButtonPressMask|
       ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask;

   XtCreateWindow ( (Widget) m, InputOutput, CopyFromParent, *window_mask,
		   window_attributes);

   /*
    * Keep menus which are a child of shell widgets mapped at all times.
    * Mapping is now done by the menu shell widget.
    */

   if (XmIsMenuShell (XtParent(m)))
       m->core.mapped_when_managed = FALSE;

   if (RC_TearOffControl(m))
   {
      if (!XtIsRealized(RC_TearOffControl(m)))
	 XtRealizeWidget(RC_TearOffControl(m));
      XtMapWidget(RC_TearOffControl(m));
   }
}


/*
 * utilities for setvalue procs
 */
static Boolean
DoEntryStuff(
        XmRowColumnWidget old,
        XmRowColumnWidget new_w )
{
    XtWidgetGeometry desired;

    Boolean need_expose = FALSE;

    if (RC_EntryBorder (old) != RC_EntryBorder (new_w))
    {
        Widget *p;
        int i;

        desired.request_mode = CWBorderWidth;
        desired.border_width = RC_EntryBorder (new_w);

        ForAllChildren (new_w, i, p)
        {
	    /* fix for 7660, setting entryborder before realize time
	       has the width and height moved from 0 to 1, a side-effect
	       of XmeConfigureObject, and then the widget loses its
	       preferred geometry. A better fix might be to remove the
	       check for 0 in XmeConfigureObject, but this might
	       be a behavior compatibility issue */
	    if (XtIsRealized(*p))
		XmeConfigureObject( *p,(*p)->core.x,(*p)->core.y,
				   (*p)->core.width, (*p)->core.height,
				   desired.border_width);
	    else
		(*p)->core.border_width = desired.border_width;

        }

        need_expose = TRUE;
    }

    if ((RC_EntryAlignment (old) != RC_EntryAlignment (new_w)) &&
        (IsAligned (new_w)) &&
        (!IsOption(new_w)))
    {
        Widget *p;
        Arg al[2];
        int i;

        XtSetArg (al[0], XmNalignment, RC_EntryAlignment(new_w));

        ForAllChildren (new_w, i, p)
        {
            XtSetValues (*p, al, 1);
        }

        need_expose  = TRUE;
    }

    if ((RC_EntryVerticalAlignment (old) != RC_EntryVerticalAlignment (new_w)) &&
        (!IsOption(new_w)))
    need_expose = TRUE;

    return (need_expose);
}

static void
DoSize(
        XmRowColumnWidget old,
        XmRowColumnWidget new_w )
{
    Widget *p;
    int i;
    int orient = RC_Orientation (old) != RC_Orientation (new_w);
    Dimension w;
    Dimension h;

    if (orient)                 /* flip all the separator */
    {                       /* widgets too */
        Arg al[2];
        int ac = 0;

        XtSetArg (al[ac], XmNorientation,
            (IsVertical (new_w) ? XmHORIZONTAL : XmVERTICAL));

        ForAllChildren (new_w, i, p)
        {
            if (XmIsSeparator(*p) || XmIsSeparatorGadget(*p))
            XtSetValues (*p, al, 1);
        }
    }

    if ((!XtWidth(new_w))  || (XtWidth (new_w) != XtWidth(old))   ||
        (!XtHeight(new_w)) || (XtHeight (new_w) != XtHeight(old)) ||
        (orient          ||
        ((IsPopup(new_w) || IsPulldown(new_w) || IsBar(new_w)) &&
            (MGR_ShadowThickness(new_w) != MGR_ShadowThickness(old))) ||
        (RC_EntryBorder (old)           != 	   RC_EntryBorder (new_w)) ||
        (RC_MarginW     (old)           != 	   RC_MarginW     (new_w)) ||
        (RC_MarginH     (old)           != 	   RC_MarginH     (new_w)) ||
        (RC_Spacing     (old)           != 	   RC_Spacing     (new_w)) ||
        (RC_Packing     (old)           != 	   RC_Packing     (new_w)) ||
        (RC_NCol        (old)           != 	   RC_NCol        (new_w)) ||
        (RC_AdjLast     (old)           != 	   RC_AdjLast     (new_w)) ||
        (RC_AdjMargin   (old)           != 	   RC_AdjMargin   (new_w)) ||
        (RC_EntryVerticalAlignment(old) !=         RC_EntryVerticalAlignment(new_w)) ||
        (RC_HelpPb      (old)           !=	   RC_HelpPb      (new_w))))
    {
	if (RC_AdjMargin(old) != RC_AdjMargin   (new_w))
	   _XmRCDoMarginAdjustment(new_w);

        if (!RC_ResizeWidth(new_w) && RC_ResizeHeight(new_w))
        {
            w = new_w->core.width;
            h = 0;
        }
        else if (RC_ResizeWidth(new_w) && !RC_ResizeHeight(new_w))
        {
            w = 0;
            h = new_w->core.height;
        }
        else if (RC_ResizeWidth(new_w) && RC_ResizeHeight(new_w))
        {
            w = 0;
            h = 0;
        }
        else
        {
            _XmRCAdaptToSize(new_w,NULL,NULL);
            return;
        }

        _XmRCPreferredSize (new_w, &w, &h);

        XtWidth(new_w) = w;
        XtHeight(new_w) = h;

	_XmRCAdaptToSize(new_w,NULL,NULL);
    }
}


static Boolean
set_values_non_popup(
        XmRowColumnWidget old,
        XmRowColumnWidget new_w )
{
    Widget child;
    Arg args[4];
    int n;
    Boolean need_expose = FALSE;

    /* fdt : should this only be done for a menubar?? */
    need_expose |= RC_HelpPb (old) != RC_HelpPb (new_w);

    /*
     * If we are an option menu, then we must check to see if our mnemonic
     * has changed.  If we're a menubar, then see if our accelerator has
     * changed.
     */
    if (IsOption(new_w))
    {
       if (RC_OptionSubMenu(new_w) != RC_OptionSubMenu(old)) {
	  _XmRC_CheckAndSetOptionCascade(new_w); /* CR 4346 */

          XtSetArg(args[0], XmNsubMenuId, RC_OptionSubMenu(new_w));
          if ((child = XmOptionButtonGadget( (Widget) new_w)) != NULL)
             XtSetValues(child, args, 1);

	  if (!RC_MemWidget(new_w) || (RC_MemWidget(old) == RC_MemWidget(new_w)))
	  {
	     if ((child = FindFirstManagedChild(
		 (CompositeWidget) RC_OptionSubMenu(new_w), FIRST_BUTTON)) != NULL)
	     {
		RC_MemWidget (new_w) = child;
	     }
	  }
       }
       if (RC_MemWidget (old) != RC_MemWidget (new_w))
       {
          _XmRC_SetOptionMenuHistory (new_w, (RectObj) RC_MemWidget (new_w));
	  _XmRC_UpdateOptionMenuCBG (XmOptionButtonGadget((Widget)new_w),
	     RC_MemWidget (new_w));
       }

       n = 0;
       if (RC_OptionLabel(new_w) != RC_OptionLabel(old)) {
          XtSetArg(args[n], XmNlabelString, RC_OptionLabel(new_w)); n++;
          XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
       }
       if (RC_MnemonicCharSet(new_w) != RC_MnemonicCharSet(old))
       {
          XtSetArg(args[n], XmNmnemonicCharSet, RC_MnemonicCharSet(new_w)); n++;
       }
       if (n && (child = XmOptionLabelGadget( (Widget) new_w)))
          XtSetValues(child, args, n);
       _XmRC_DoProcessMenuTree((Widget) new_w, XmREPLACE);
    }
    else if (IsBar(new_w) && (RC_MenuAccelerator(new_w) != RC_MenuAccelerator(old)))
    {
        if (RC_MenuAccelerator(new_w))
        {
            RC_MenuAccelerator(new_w) = (String)strcpy(XtMalloc( XmStrlen(
                RC_MenuAccelerator(new_w)) + 1), RC_MenuAccelerator(new_w));
        }
        _XmRC_DoProcessMenuTree((Widget) new_w, XmREPLACE);
        if (RC_MenuAccelerator(old))
           XtFree(RC_MenuAccelerator(old));
    }

   /*
    * Moved here in case Option Menu geometry changed
    */
    need_expose |= DoEntryStuff (old, new_w);
    DoSize (old, new_w);

    return (need_expose);
}

static Boolean
set_values_popup(
        XmRowColumnWidget old,
        XmRowColumnWidget new_w )
{
    int need_expose  = FALSE;
    Arg args[4];
    int n = 0;

    need_expose |= DoEntryStuff (old, new_w);
    DoSize (old, new_w);

    if ((XtX (old) != XtX (new_w)) ||     /* signal the shell that it */
        (XtY (old) != XtY (new_w)))       /* had better move itself */
    {                                   /* to the menu's location */
        RC_SetWidgetMoved (new_w, TRUE);      /* and that it has to move */
        RC_SetWindowMoved (new_w, TRUE);      /* the menu's window back */
    }

    /*
     * If we are a popup menu, then we need to check the
     * state of the popupEnabled resource; we may need to add or remove the
     * event handler we use to catch accelerators and mnemonics.
     */
    if (IsPopup(new_w))
    {
       if (RC_PopupEnabled(new_w) != RC_PopupEnabled(old))
       {
          if (RC_PopupEnabled(new_w))
          {
	    /* If this was enabled before with a different value,
	       then first remove the popup handlers,  then reinstall */
	    if (RC_PopupEnabled(old) != XmPOPUP_DISABLED)
	      _XmRC_RemovePopupEventHandlers(new_w);
	    /* Now reinstall */
	    _XmRC_AddPopupEventHandlers(new_w);
	    _XmRC_DoProcessMenuTree( (Widget) new_w, XmADD);
          }
          else
          {
	    _XmRC_RemovePopupEventHandlers (new_w);
	    _XmRC_DoProcessMenuTree( (Widget) new_w, XmDELETE);
          }
       }

       /* See if our accelerator has changed */
       if (RC_MenuAccelerator(new_w) != RC_MenuAccelerator(old))
       {
          if (RC_MenuAccelerator(new_w))
          {
             RC_MenuAccelerator(new_w) = (String)strcpy(XtMalloc( XmStrlen(
                 RC_MenuAccelerator(new_w)) + 1), RC_MenuAccelerator(new_w));
          }

	  if (RC_PopupEnabled(new_w))
	     _XmRC_DoProcessMenuTree( (Widget) new_w, XmREPLACE);

          if (RC_MenuAccelerator(old))
             XtFree(RC_MenuAccelerator(old));
       }
    }

    /* For both pulldowns and popups */
       if (RC_TearOffModel(old) != RC_TearOffModel(new_w))
       {
	  if ((RC_TearOffModel(new_w) != XmTEAR_OFF_DISABLED) &&
	      !RC_TearOffControl(new_w))
	  {
	     XmTearOffButtonWidget tw;

	     /* prevent RowColumn: InsertChild() from inserting tear off button
	      * into child list.
	      */
	     RC_SetFromInit(new_w, TRUE);
	     tw = (XmTearOffButtonWidget) XtCreateWidget(TEAROFF_CONTROL,
		    xmTearOffButtonWidgetClass, (Widget)new_w, args, n);
 	     RC_TearOffControl(new_w) = (Widget) tw;
	     RC_SetFromInit(new_w, FALSE);

	     /* Catching this bogus case is rediculous, but still, who knows
	      * what the app developer from DSU will do?
	      * So, like the Init method, we'll put off realize/manage until
	      * later (RowColumn.c: Realize()) if the submenu isn't realized.
 	      *
 	      * If the menu is tear_off_activated, then you better not manage
 	      * the tear off control or it shows up in the tear off menu!
	      */
 	     if (XmIsMenuShell(XtParent(new_w)))
	       {
		 if (XtIsRealized((Widget)new_w))
		   {
		     XtRealizeWidget(RC_TearOffControl(new_w));
		     XtManageChild(RC_TearOffControl(new_w));
		   } else
		     RC_TearOffControl(new_w)->core.managed = TRUE;
	       }
	   }
	  else
	     if ((RC_TearOffModel(new_w) == XmTEAR_OFF_DISABLED) &&
		 RC_TearOffControl(new_w))
	     {
		XtUnmanageChild(RC_TearOffControl(new_w)); /* otherwise ChangeManaged is too clever and doesn't catch change if tear-off goes away */
		XtDestroyWidget(RC_TearOffControl(new_w));
		RC_TearOffControl(new_w) = NULL;
		/* If the model has changed while the tear-off is visible, take
		** it down. For consistency with other non-interactive forms
		** of takedown (DELETE messages and destruction), use the common
		** routine, which results in application callbacks to e.g.
		** XmNtearOffMenuDeactivateCallback (with a NULL event).
		*/
	   	if(RC_TornOff(new_w))
   			_XmDismissTearOff(XtParent(new_w), NULL, NULL);
	     }
       }
       if ((old->core.background_pixel != new_w->core.background_pixel) &&
           RC_TearOffControl(new_w))
       {
          XtSetArg(args[0], XmNbackground, new_w->core.background_pixel);
          XtSetValues(RC_TearOffControl(new_w), args, 1);
       }

       /* Update title if menu torn off */
       if ((RC_TearOffTitle(new_w) != RC_TearOffTitle(old))
	   && RC_TornOff(old)) {
	 XmeSetWMShellTitle(RC_TearOffTitle(new_w),
				     (Widget) XtParent(new_w));
       }

    return (need_expose);
}

static void
set_values_passive_grab(
        XmRowColumnWidget old,
        XmRowColumnWidget new_w )
{
   int i;
   Cursor cursor;

   if (IsPopup(old))
   {
      /* Keep our passive grab up to date. */
      if (RC_PopupEnabled(old))
      {
         /* Remove it from the postFrom widgets */
         for (i=0; i < old->row_column.postFromCount; i++)
         {
	   /* Remove our passive grab */
	   XtUngrabButton (old->row_column.postFromList[i],
			   RC_PostButton(old), RC_PostModifiers(old));
         }

         if (RC_PopupEnabled(new_w))
         {
            cursor = _XmGetMenuCursorByScreen(XtScreen(new_w));

            /* add to all of the widgets in the postFromList*/
            for (i=0; i < new_w->row_column.postFromCount; i++)
            {
               /*
                * Must add a passive grab, so that owner_events is
                * set to True when the button grab is activated
                * this is so that enter/leave
                * events get dispatched by the server to the client.
                */
	       XtGrabButton (new_w->row_column.postFromList[i],
			     RC_PostButton(new_w), RC_PostModifiers(new_w),
			     TRUE, (unsigned int) ButtonReleaseMask,
			     GrabModeSync, GrabModeSync, None, cursor);
            }
         }
      }
   }
}

static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
        XmRowColumnWidget old = (XmRowColumnWidget) cw ;
        XmRowColumnWidget req = (XmRowColumnWidget) rw ;
        XmRowColumnWidget new_w = (XmRowColumnWidget) nw ;
   int i;
   int need_expose = FALSE;

   if (!XtWidth(req))
   {
      XmeWarning( (Widget) new_w, BadWidthSVMsg);
      XtWidth(new_w) = XtWidth(old);
   }

   if (!XtHeight(req))
   {
      XmeWarning( (Widget) new_w, BadHeightSVMsg);
      XtHeight(new_w) = XtHeight(old);
   }

   if (!XmRepTypeValidValue( XmRID_ORIENTATION, RC_Orientation(new_w),
       (Widget)new_w))
   {
      RC_Orientation(new_w) = RC_Orientation(old);
   }

   if (!XmRepTypeValidValue( XmRID_PACKING, RC_Packing(new_w),
       (Widget)new_w))
   {
      RC_Packing(new_w) = RC_Packing(old);
   }

   if (RC_Type(req) != RC_Type(old))
   {
      /* Type CANNOT be changed after initialization */
      XmeWarning( (Widget) new_w, BadTypeSVMsg);
      RC_Type(new_w) = RC_Type(old);
   }

   if (!XmRepTypeValidValue( XmRID_ALIGNMENT, RC_EntryAlignment(new_w),
       (Widget)new_w))
   {
      RC_EntryAlignment(new_w) = RC_EntryAlignment(old);
   }

   if (!XmRepTypeValidValue( XmRID_VERTICAL_ALIGNMENT, RC_EntryVerticalAlignment(new_w),
       (Widget)new_w))
   {
      RC_EntryVerticalAlignment(new_w) = RC_EntryVerticalAlignment(old);
   }

   if (IsBar(new_w))
   {
      if (RC_IsHomogeneous(req) != RC_IsHomogeneous(old))
      {
	 /* can't change this for menu bars */
	 XmeWarning( (Widget) new_w, BadMenuBarHomogenousSVMsg);
	 RC_IsHomogeneous(new_w) = TRUE;
      }
      if (RC_EntryClass(req) != RC_EntryClass(old))
      {
	 /* can't change this for menu bars */
	 XmeWarning( (Widget) new_w, BadMenuBarEntryClassSVMsg);
	 RC_EntryClass(new_w) = xmCascadeButtonWidgetClass;
      }
   }

   /* CR 7807: Make sure the EntryClass's fast subclass bits are set. */
   if (RC_EntryClass(req) != RC_EntryClass(old))
     XtInitializeWidgetClass(RC_EntryClass(req));

   if (RC_MenuPost(new_w) != RC_MenuPost(old))
   {
      if (IsPulldown(new_w))
      {
         /* MenuPost cannot be changed via SetValues for Pulldowns */
         XmeWarning( (Widget) new_w, BadPulldownMenuPostSVMsg);
         /* just in case WhichButton was set */
         RC_PostButton(new_w) = RC_PostButton(old);
      }
      else
      {
         if (_XmMapBtnEvent(RC_MenuPost(new_w), &RC_PostEventType(new_w),
               &RC_PostButton(new_w), &RC_PostModifiers(new_w)) == FALSE)
         {
            XmeWarning( (Widget) new_w, BadMenuPostMsg);
            /* Do Nothing - No change to postButton/Modifiers/EventType */
         }
         else if (RC_MenuPost(new_w))
            RC_MenuPost(new_w) = XtNewString(RC_MenuPost(new_w));
         set_values_passive_grab(old, new_w);
         if (RC_MenuPost(old)) XtFree(RC_MenuPost(old));
      }
   }
   else   /* For backwards compatibility... */
      if (RC_PostButton(new_w) != RC_PostButton(old))
      {
         if (IsPulldown(new_w))
         {
            /* WhichButton cannot be changed via SetValues for Pulldowns */
            XmeWarning( (Widget) new_w, BadPulldownWhichButtonSVMsg);
            RC_PostButton(new_w) = RC_PostButton(old);
         }
         else
         {
            RC_PostModifiers(new_w) = AnyModifier;
            RC_PostEventType(new_w) = ButtonPress;
            set_values_passive_grab(old, new_w);
         }
      }


   /*
    * Shadow thickness is forced to zero for all types except
    * pulldown, popup, and menubar
    */
   if (IsPulldown(new_w) || IsPopup(new_w) || IsBar(new_w))
   {
      if (MGR_ShadowThickness(req) != MGR_ShadowThickness(old))
	  need_expose |= TRUE;
   }

   else if (MGR_ShadowThickness(req) != MGR_ShadowThickness(old))
   {
      XmeWarning( (Widget) new_w, BadShadowThicknessSVMsg);
      MGR_ShadowThickness(new_w) = 0;
   }

/* BEGIN OSF fix pir 2429 */
   if (IsOption(new_w) &&
       (RC_IsHomogeneous(req) != RC_IsHomogeneous(old)))
   {
      XmeWarning((Widget)new_w, BadOptionIsHomogeneousSVMsg);
      RC_IsHomogeneous(new_w) = FALSE;
   }
/* END OSF fix pir 2429 */


   /* postFromList changes, popups and pulldowns only */
   if (IsPopup(new_w) || IsPulldown(new_w))
   {
      if ((new_w->row_column.postFromList != old->row_column.postFromList) ||
	  (new_w->row_column.postFromCount != old->row_column.postFromCount))
      {
	 /* use temp - postFromCount decremented in RemoveFromPostFromList() */
	 int cnt;
	 if (old->row_column.postFromList)
	 {
	    cnt = old->row_column.postFromCount;
	    for (i=0; i < cnt; i++)
	    {
	       _XmRC_RemoveHandlersFromPostFromWidget((Widget) new_w,
		  old->row_column.postFromList[i]);
	    }
	    XtFree( (char *) old->row_column.postFromList);
	 }

         PreparePostFromList(new_w);
      }
   }

   /* Copy new value,  free old value,
      update real title in set_values_popup */
   if ((RC_TearOffTitle(new_w) != RC_TearOffTitle(old))) {
     XmStringFree(RC_TearOffTitle(old));
     RC_TearOffTitle(new_w) = XmStringCopy(RC_TearOffTitle(new_w));
   }

   if (IsBar (new_w) || IsWorkArea (new_w) || IsOption (new_w))
       need_expose |= set_values_non_popup (old, new_w);
   else
       need_expose |= set_values_popup (old, new_w);

   return (need_expose);
}



/************************************************************************
 *
 *  QueryGeometry
 *
 ************************************************************************/
static XtGeometryResult
QueryGeometry(
        Widget widget,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    XmRowColumnWidget m = (XmRowColumnWidget) widget ;
    Dimension width = 0, height = 0 ;

    /* first determine what is the desired size, using the resizeWidth
       and resizeHeight resource and the intended value */
    if (GMode(intended) & CWWidth) width = intended->width;
    if (GMode(intended) & CWHeight) height = intended->height;

    if (!RC_ResizeWidth(m)) width = XtWidth(widget) ;
    if (!RC_ResizeHeight(m)) height = XtHeight(widget) ;

    _XmRCPreferredSize (m, &width, &height);

    desired->width = width ;
    desired->height = height ;

    /* deal with user initial size setting */
    /****************
      Comment that out for now, too fragile...
    if (!XtIsRealized(widget))  {
	if (XtWidth(widget) != 0) desired->width = XtWidth(widget) ;
	if (XtHeight(widget) != 0) desired->height = XtHeight(widget) ;
    } ****************/

    return XmeReplyToQueryGeometry(widget, intended, desired) ;
}




/************************************************************************
 *
 *  GeometryManager class method
 *
 ************************************************************************/
static XtGeometryResult
GeometryManager(
        Widget instigator,
        XtWidgetGeometry *desired,
        XtWidgetGeometry *allowed )
{
   XmRowColumnWidget m = (XmRowColumnWidget) XtParent(instigator);
   Dimension w = 0;
   Dimension h = 0;
   XtGeometryResult result = XtGeometryYes;

   /* First treat the special case resulting from a change in positionIndex */
   if (RCIndex(instigator) == XmLAST_POSITION) {
                    /* set in ConstraintSetValues */
       int i ;

       /* first reset the value of positionIndex to its real value */
       for (i = 0 ; i < m->composite.num_children; i++)
         if (m->composite.children[i] == instigator) {
             RCIndex(instigator) = i ;
             break ;
         }

       /* then accept the desired change */
       if (IsX(desired) && desired->x >= 0)
         instigator->core.x = desired->x;
       if (IsY(desired) && desired->y >= 0)
         instigator->core.y = desired->y;
       if (IsHeight(desired) && desired->height > 0)
         instigator->core.height = desired->height;
       if (IsWidth(desired) && desired->width > 0)
         instigator->core.width = desired->width;
      return XtGeometryYes;
    }

   /*
    * find out what size we need to be with this new widget
    */
   RC_Boxes(m) = (XmRCKidGeometry)
       _XmRCGetKidGeo( (Widget) m, instigator, desired,
		     RC_EntryBorder(m), RC_EntryBorder (m),
		     (IsVertical (m) && RC_DoMarginAdjust (m)),
		     (IsHorizontal (m) && RC_DoMarginAdjust (m)),
		     RC_HelpPb (m),
		     RC_TearOffControl(m),
		     XmGET_PREFERRED_SIZE);

   _XmRCThinkAboutSize(m, &w, &h, instigator, desired);

   if (IsOption(m))
   {
      XmRCKidGeometry kg;
      Widget omcb = XmOptionButtonGadget((Widget)m);
      Widget oml = XmOptionLabelGadget((Widget)m);

      /* _XmRCGetKidGeo() does not allocate a geometry box for unmanaged
       * children.
       */
      for(kg=RC_Boxes(m); (kg->kid != NULL) && (kg->kid != omcb); kg++)
	/*EMPTY*/;

      if (kg->kid && (instigator == omcb)) {

         /*
	   * Fix for 5388 - Check to see if the OptionLabel is Managed.
	   * If it is base comparisons off of RC_Boxes(m)[1].
	   * If not, base comparisons off of RC_Boxes(m)[0]
	   */

          XtWidgetGeometry *button_box;

          if (!XtIsManaged(XmOptionLabelGadget( (Widget) m))) {
	      button_box = &(RC_Boxes(m)[0].box);
          } else {
	      button_box = &(RC_Boxes(m)[1].box);
          }

  	 /*
  	  * Grow only
  	  */
	  if ((desired->request_mode & CWWidth) &&
	      (desired->width < BWidth(button_box))) {
	      allowed->width = BWidth(button_box);
	      allowed->height = BHeight(button_box);
	      allowed->request_mode = (CWHeight | CWWidth);
	      result = XtGeometryAlmost;
	  }

	  if ((desired->request_mode & CWHeight) &&
	      (desired->height < BHeight(button_box))) {
	      allowed->width = BWidth(button_box);
	      allowed->height = BHeight(button_box);
	      allowed->request_mode = (CWHeight | CWWidth);
	      result = XtGeometryAlmost;
	  }

         if (result != XtGeometryYes)
         {
            XtFree( (char *) RC_Boxes(m));
            return(result);
         }
      }

      for(kg=RC_Boxes(m); (kg->kid != NULL) && (kg->kid != oml); kg++)
	/*EMPTY*/;

      if (kg->kid && (instigator == oml))
      {
	 /*
	  * Can't get shorter
	  */
         if ((desired->request_mode & CWHeight) &&
	     (desired->height < kg->box.height))
         {
	    allowed->width = kg->box.width;
	    allowed->height = kg->box.height;
	    allowed->request_mode = (CWHeight | CWWidth);
	    result = XtGeometryAlmost;
         }

         if (result != XtGeometryYes)
         {
            XtFree( (char *) RC_Boxes(m));
            return(result);
         }
      }
   }

   /*
    * now decide if the menu needs to change size.
    */

   XtFree( (char *) RC_Boxes(m));

   if ((w != XtWidth (m)) || (h != XtHeight (m)))
   {
      XtWidgetGeometry menu_desired, menu_allowed;
      menu_desired.request_mode = 0;

      if (w != XtWidth (m))
      {
	 menu_desired.width = w;
	 menu_desired.request_mode |= CWWidth;
      }

      if (h != XtHeight (m))
      {
	 menu_desired.height = h;
	 menu_desired.request_mode |= CWHeight;
      }

      /* Check for the query only bit. */
      if (desired->request_mode & XtCWQueryOnly)
         menu_desired.request_mode |= XtCWQueryOnly;

      result = XtMakeGeometryRequest( (Widget) m,&menu_desired,&menu_allowed);

      switch (result)
      {
       case XtGeometryAlmost:
       case XtGeometryNo:
          if (((XtWidth(m) < w) && desired->request_mode & CWWidth) \
		  || ((XtHeight(m) < h) && desired->request_mode & CWHeight))
          return (XtGeometryNo);
          break;
       default: /* fall out */
	 break;
      }
   }

   /* Check for the query only bit. */
   if (!(desired->request_mode & XtCWQueryOnly))
   {
       _XmRCAdaptToSize(m,instigator,desired);

       /* Clear shadow if necessary. */
       if (m->row_column.old_shadow_thickness &&
      ( (m->row_column.old_width != m->core.width)
      ||(m->row_column.old_height != m->core.height)
      ||(m->row_column.old_shadow_thickness != m->manager.shadow_thickness) ) )
	   _XmClearShadowType( (Widget) m, m->row_column.old_width,
			      m->row_column.old_height,
			      m->row_column.old_shadow_thickness, 0);

       m->row_column.old_width = m->core.width;
       m->row_column.old_height = m->core.height;
       m->row_column.old_shadow_thickness = m->manager.shadow_thickness;
   }
   return (XtGeometryYes);
}


static char *
GetRealKey(
        XmRowColumnWidget rc,
        char *str )
{
   KeySym   keysym;
   Modifiers mods;
   char buf[1000];
   char *tmp = buf;
   char *ks;
   int num_keys;
   XmKeyBinding keys;

   keysym = XStringToKeysym(str);
   if (keysym == NoSymbol)
      return(NULL);

   *tmp = '\0';
   num_keys = XmeVirtualToActualKeysyms(XtDisplay(rc), keysym, &keys);
   while (--num_keys >= 0)
     if ((ks = XKeysymToString(keys[num_keys].keysym)) != NULL)
       {
	 mods = keys[num_keys].modifiers;
	 if (mods & ControlMask)
	   strcpy(tmp, "Ctrl ");

	 if (mods & ShiftMask)
	   strcat(tmp, "Shift ");

	 if (mods & Mod1Mask)
	   strcat(tmp, "Alt ");

	 strcat(tmp,"<KeyUp>");
	 strcat(tmp, ks);

	 if (num_keys > 0)
	   strcat(tmp, ", ");
	 tmp += strlen(tmp);
       }
   XtFree((char*) keys);

   if (tmp != buf)
     return XtNewString(buf);
   else
     return NULL;
}

static void
MenuBarInitialize(
        XmRowColumnWidget bar )
{
    Widget topManager;

    RC_IsHomogeneous(bar) = TRUE;
    RC_EntryClass(bar) = xmCascadeButtonWidgetClass;
    bar->manager.traversal_on = False;
    bar->row_column.lastSelectToplevel = (Widget) bar;

    if (RC_PostButton(bar) == UNSET_POSTBUTTON)
        RC_PostButton(bar) = Button1;


    if (RC_Packing(bar) == XmNO_PACKING)
        RC_Packing(bar) = XmPACK_TIGHT;

    if (RC_Orientation(bar) == XmNO_ORIENTATION)
        RC_Orientation(bar) = XmHORIZONTAL;

    if (RC_Spacing(bar) == XmINVALID_DIMENSION)
	RC_Spacing(bar) = 0;

    XtOverrideTranslations((Widget) bar, menu_traversal_parsed);

    if (RC_MenuAccelerator(bar))
    {
       if (*RC_MenuAccelerator(bar) == '\0')
       {
	  if (!(RC_MenuAccelerator(bar) = GetRealKey(bar, "osfMenuBar")))
	     RC_MenuAccelerator(bar) = XtNewString("<KeyUp>F10");
       }
       else   /* Save a copy of the accelerator string */
	  RC_MenuAccelerator(bar) = XtNewString(RC_MenuAccelerator(bar));
    }

    /*
     * Add an event handler to both us and the associated widget; we
     * need one in case we have gadget children.
     */
    _XmRCGetTopManager ((Widget) bar, &topManager);
    XtAddEventHandler( (Widget) bar, KeyPressMask|KeyReleaseMask,
        False, _XmRC_KeyboardInputHandler, (XtPointer) bar);
    XtAddEventHandler( (Widget) topManager, KeyPressMask|KeyReleaseMask,
        False, _XmRC_KeyboardInputHandler, (XtPointer) bar);

    if (RC_MenuAccelerator(bar))
        _XmRC_DoProcessMenuTree( (Widget) bar, XmADD);

    if (bar->manager.navigation_type == XmDYNAMIC_DEFAULT_TAB_GROUP)
        bar->manager.navigation_type = XmSTICKY_TAB_GROUP;
}

/*
 * prepare postFromList: if its at its default state, its parent should
 * be in the list.  If a list has been specified but the count has not,
 * then set the count to 0.  This is only useful for Popup and Pulldown panes.
 */
static void
PreparePostFromList(
        XmRowColumnWidget rowcol )
{
   Widget * tempPtr;
   Boolean forceParent = FALSE;
   int i;

   if (rowcol->row_column.postFromCount < 0)
   {
      if (IsPopup(rowcol) && rowcol->row_column.postFromList == NULL)
      {
	 /* default state for popups, set to parent */
	 rowcol->row_column.postFromCount = 1;
	 forceParent = True;
      }
      else
	  /* user provided a list but no count, default count to 0 */
	  rowcol->row_column.postFromCount = 0;
   }

   /* malloc enough space for 1 more addition to the list */
   rowcol->row_column.postFromListSize = rowcol->row_column.postFromCount + 1;

   tempPtr = rowcol->row_column.postFromList;
   rowcol->row_column.postFromList = (Widget *)
       XtMalloc (rowcol->row_column.postFromListSize * sizeof(Widget));

   if (tempPtr)
   {
      /* use temp - postFromCount incremented in _XmRC_AddToPostFromList() */
      int cnt = rowcol->row_column.postFromCount;
      /* reset the postFromCount for correct _XmRC_AddToPostFromList()
	 assignment */
      rowcol->row_column.postFromCount = 0;

      for (i=0; i < cnt; i++)
      {
	 _XmRC_AddToPostFromList ( rowcol, tempPtr[i]);
      }
   }
   else if (forceParent)
   {
      /* no postFromList, then parent of Popup is on this list */
      rowcol->row_column.postFromList[0] = XtParent(XtParent(rowcol));
   }
}

static void
PopupInitialize(
        XmRowColumnWidget popup )
{
   Arg args[4];
   int n = 0;

   popup->row_column.lastSelectToplevel = (Widget) popup;

   if (RC_PostButton(popup) == UNSET_POSTBUTTON)
       RC_PostButton(popup) = Button3;

   if (RC_Packing(popup) == XmNO_PACKING)
       RC_Packing(popup) = XmPACK_TIGHT;

   if (RC_Orientation(popup) == (char) XmNO_ORIENTATION)
       RC_Orientation(popup) = XmVERTICAL;

   if (RC_HelpPb(popup) != NULL)
   {
      XmeWarning( (Widget)popup, BadPopupHelpMsg);
      RC_HelpPb(popup) = NULL;
   }

   if (RC_Spacing(popup) == XmINVALID_DIMENSION)
       RC_Spacing(popup) = 0;

   XtOverrideTranslations( (Widget) popup, menu_traversal_parsed);

   /* If no accelerator specified, use the default */
   if (RC_MenuAccelerator(popup))
   {
      if (*RC_MenuAccelerator(popup) == '\0')
      {
	 if (!(RC_MenuAccelerator(popup) = GetRealKey(popup, "osfMenu")))
	    RC_MenuAccelerator(popup) = XtNewString("Shift<KeyUp>F10");
      }
      else   /* Save a copy of the accelerator string */
	 RC_MenuAccelerator(popup) = XtNewString(RC_MenuAccelerator(popup));
   }

   PreparePostFromList(popup);

   /* Add event handlers to all appropriate widgets */
   if (RC_PopupEnabled(popup))
   {
      _XmRC_AddPopupEventHandlers (popup);

      /* Register all accelerators */
      _XmRC_DoProcessMenuTree( (Widget) popup, XmADD);
   }

   if (RC_TearOffModel(popup) != XmTEAR_OFF_DISABLED)
   {
      /* prevent RowColumn: InsertChild() from inserting tear off button
       * into child list.
       */
      RC_SetFromInit(popup, TRUE);
      RC_TearOffControl(popup) =
	 XtCreateWidget(TEAROFF_CONTROL, xmTearOffButtonWidgetClass,
	    (Widget)popup, args, n);

      RC_SetFromInit(popup, FALSE);
      /* Can't call XtManageChild() 'cause popup's not realized yet */
      RC_TearOffControl(popup)->core.managed = TRUE;
   }
   popup->row_column.popup_workproc = 0;
}

static void
PulldownInitialize(
        XmRowColumnWidget pulldown )
{
    Arg args[4];
    int n = 0;

    pulldown->row_column.lastSelectToplevel = (Widget) NULL;

    if (RC_Packing(pulldown) == XmNO_PACKING)
        RC_Packing(pulldown) = XmPACK_TIGHT;

    if (RC_Orientation(pulldown) == (char) XmNO_ORIENTATION)
        RC_Orientation(pulldown) = XmVERTICAL;

    if (RC_HelpPb(pulldown) != NULL)
    {
        XmeWarning( (Widget)pulldown, BadPulldownHelpMsg);
        RC_HelpPb(pulldown) = NULL;
    }

    if (RC_Spacing(pulldown) == XmINVALID_DIMENSION)
        RC_Spacing(pulldown) = 0;

    XtOverrideTranslations((Widget) pulldown, menu_traversal_parsed);

    RC_MenuAccelerator(pulldown) = NULL;
    PreparePostFromList(pulldown);

    /* add event handler to myself for gadgets */
    XtAddEventHandler( (Widget) pulldown, KeyPressMask|KeyReleaseMask,
		      False, _XmRC_KeyboardInputHandler, (XtPointer) pulldown);

    if (RC_TearOffModel(pulldown) != XmTEAR_OFF_DISABLED)
    {
       /* prevent RowColumn: InsertChild() from inserting separator into child
	* list.
	*/
       RC_SetFromInit(pulldown, TRUE);
       RC_TearOffControl(pulldown) =
	  XtCreateWidget(TEAROFF_CONTROL, xmTearOffButtonWidgetClass,
	     (Widget)pulldown, args, n);

       RC_SetFromInit(pulldown, FALSE);
       /* Can't call XtManageChild() 'cause pulldown's not realized yet */
       RC_TearOffControl(pulldown)->core.managed = TRUE;
    }
}

static void
OptionInitialize(
        XmRowColumnWidget option )
{
    int n;
    Arg args[4];
    Widget topManager;
    Widget child;
    XmString empty_string = NULL ;

    /* BEGIN OSFfix pir 2121 */
    MGR_ShadowThickness(option) = 0;
    /* END OSFfix pir 2121 */

    if (RC_HelpPb(option) != NULL)
    {
        XmeWarning( (Widget)option, BadOptionHelpMsg);
        RC_HelpPb(option) = NULL;
    }

    RC_Packing(option) = XmPACK_TIGHT;
/* BEGIN OSF fix pir 2429 */
    RC_IsHomogeneous(option) = FALSE;
    if (RC_Orientation(option) == (char) XmNO_ORIENTATION)
       RC_Orientation(option) = XmHORIZONTAL;
/* END OSF fix pir 2429 */
    option->row_column.lastSelectToplevel = (Widget) option;

    if (RC_PostButton(option) == UNSET_POSTBUTTON)
        RC_PostButton(option) = Button1;

    if (RC_Spacing(option) == XmINVALID_DIMENSION)
        RC_Spacing(option) = 3;

    {
      XtTranslations translations;
      _XmProcessLock();
      translations = (XtTranslations) ((XmManagerClassRec *)XtClass(option))->
			manager_class.translations;
      _XmProcessUnlock();
      XtOverrideTranslations((Widget)option, translations);
    }

    /* Create the label widget portion of the option menu */
    n = 0;

    /* fix for 5235 */
    if (RC_OptionLabel(option)) {
 	XtSetArg(args[n], XmNlabelString, RC_OptionLabel(option)); n++;
    } else {
 	/* if NULL, OPTION_LABEL will be used as default label, and we
 	   want an empty string */
 	/* Note: since this resource "labelString" in C only in the AES,
 	   no need to add a synthetic getvalue that will return NULL instead
 	   of this empty_string */
 	empty_string = XmStringCreateLocalized(XmS);
 	XtSetArg(args[n], XmNlabelString, empty_string); n++;
    }


    if (RC_MnemonicCharSet(option))
    {
	XtSetArg(args[n], XmNmnemonicCharSet, RC_MnemonicCharSet(option)); n++;
    }
    child = XmCreateLabelGadget( (Widget) option,OPTION_LABEL,args,n);
    XtManageChild (child);

    if (empty_string != NULL) XmStringFree(empty_string);
    /* end fix for 5235 */

    /* Create the cascade button widget portion of the option menu */
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, RC_OptionSubMenu(option)); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;

    /*
     * set recomputeSize false:  the option menu continually recalculates
     * the best size for this button.  By setting this false, we prevent
     * geometry requests every time the label is updated to the most
     * recent label.  This also allows for the user to do a setvalues
     * on the buttons width or height and it will be honored, as long as
     * it is big enough to handle the largest button in the pulldown menu.
     */
    XtSetArg(args[n], XmNrecomputeSize, FALSE); n++;
    child = XmCreateCascadeButtonGadget((Widget)option,OPTION_BUTTON,args,n);
    XtManageChild (child);

    RC_MenuAccelerator(option) = NULL;

    /* Add event handlers for catching keyboard input */
    _XmRCGetTopManager ((Widget) option, &topManager);
    XtAddEventHandler( (Widget) option, KeyPressMask|KeyReleaseMask,
        False, _XmRC_KeyboardInputHandler, (XtPointer) option);
    XtAddEventHandler( (Widget) topManager, KeyPressMask|KeyReleaseMask,
        False, _XmRC_KeyboardInputHandler, (XtPointer) option);

    if (RC_Mnemonic(option) != XK_VoidSymbol)
        _XmRC_DoProcessMenuTree( (Widget) option, XmADD);

    if (option->manager.navigation_type == XmDYNAMIC_DEFAULT_TAB_GROUP)
       option->manager.navigation_type = XmNONE;
}

static void
WorkAreaInitialize(
        XmRowColumnWidget work )
{
    MGR_ShadowThickness(work) = 0;

    if (RC_PostButton(work) == UNSET_POSTBUTTON)
        RC_PostButton(work) = Button1;

    if (work->row_column.radio)
    {
       if (RC_Packing(work) == XmNO_PACKING)
          RC_Packing(work) = XmPACK_COLUMN;

       if (RC_EntryClass(work) == NULL)
          RC_EntryClass(work) = xmToggleButtonGadgetClass;
    }
    else
       if (RC_Packing(work) == XmNO_PACKING)
	   RC_Packing(work) = XmPACK_TIGHT;

    if (RC_Orientation(work) == (char) XmNO_ORIENTATION)
        RC_Orientation(work) = XmVERTICAL;

    if (RC_HelpPb(work) != NULL)
    {
        XmeWarning( (Widget)work, BadWorkAreaHelpMsg);
        RC_HelpPb(work) = NULL;
    }

    if (RC_Spacing(work) == XmINVALID_DIMENSION)
        RC_Spacing(work) = 3;

    {
      XtTranslations translations;
      _XmProcessLock();
      translations = (XtTranslations) ((XmManagerClassRec *)XtClass(work))->
			manager_class.translations;
      _XmProcessUnlock();
      XtOverrideTranslations((Widget)work, translations);
    }

    RC_MenuAccelerator(work) = NULL;

    if (work->manager.navigation_type == XmDYNAMIC_DEFAULT_TAB_GROUP)
        work->manager.navigation_type = XmTAB_GROUP;
}

/*
 * Initialize a row column widget
 */
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    XmRowColumnWidget req = (XmRowColumnWidget) rw ;
    XmRowColumnWidget m = (XmRowColumnWidget) nw ;
    Boolean CallNavigInitAgain = FALSE ;

    if (!XtWidth(req))
    {
        XtWidth(m) = DEFAULT_WIDTH;
    }

    if (!XtHeight(req))
    {
        XtHeight(m) = DEFAULT_HEIGHT;
    }

    if (IsPulldown(m) || IsPopup(m))
    {
	if (RC_MarginW(m) == XmINVALID_DIMENSION)
	    RC_MarginW(m) = 0;
	if (RC_MarginH(m) == XmINVALID_DIMENSION)
	    RC_MarginH(m) = 0;
    } else
    {
	if (RC_MarginW(m) == XmINVALID_DIMENSION)
	    RC_MarginW(m) = 3;
	if (RC_MarginH(m) == XmINVALID_DIMENSION)
	    RC_MarginH(m) = 3;
    }

    if ((RC_Orientation(nw) != XmNO_ORIENTATION) &&
        !XmRepTypeValidValue( XmRID_ORIENTATION, RC_Orientation(nw),
	 (Widget)nw))
    {
       RC_Orientation(nw) = XmNO_ORIENTATION;
    }

    if ((RC_Packing(nw) != XmNO_PACKING) &&
        !XmRepTypeValidValue( XmRID_PACKING, RC_Packing(nw),
	 (Widget)nw))
    {
       RC_Packing(nw) = XmNO_PACKING;
    }

    if (!XmRepTypeValidValue( XmRID_ROW_COLUMN_TYPE, RC_Type(nw),
	(Widget)nw))
    {
       RC_Type(nw) = XmWORK_AREA;
    }
    else
       if ((RC_Type(req) == XmMENU_POPUP) || (RC_Type(req) == XmMENU_PULLDOWN))
       {
	    if (!XmIsMenuShell(XtParent(req)) ||
		!XtParent(XtParent(req)))
	    {
		XmeWarning( (Widget)m,BadTypeParentMsg);
		RC_Type(m) = XmWORK_AREA;
	    }
       }

    if (!XmRepTypeValidValue( XmRID_ALIGNMENT, RC_EntryAlignment(nw),
	(Widget)nw))
    {
       RC_EntryAlignment(nw) = XmALIGNMENT_BEGINNING;
    }

    if (!XmRepTypeValidValue( XmRID_VERTICAL_ALIGNMENT, RC_EntryVerticalAlignment(nw),
        (Widget)nw))
    {
       RC_EntryVerticalAlignment(nw) = XmALIGNMENT_CENTER;
    }

    RC_CascadeBtn(m) = NULL;
    RC_Boxes(m) = NULL;

    m->row_column.armed = 0;
    RC_SetExpose (m, TRUE);             /* and ready to paint gadgets */
    RC_SetWidgetMoved  (m, TRUE);       /* and menu and shell are not */
    RC_SetWindowMoved  (m, TRUE);       /* in synch, positiongally */
    RC_SetArmed  (m, FALSE);
    RC_SetPoppingDown  (m, FALSE);      /* not popping down */
    RC_PopupPosted(m) = NULL;		/* no popup submenus posted */

    RC_TearOffControl(m) = NULL;

    m->row_column.to_state = 0;
    RC_SetFromInit(m, FALSE);
    RC_SetTornOff(m, FALSE);
    RC_SetTearOffActive(m, FALSE);
    RC_SetFromResize(m, FALSE);

    RC_popupMenuClick(m) = TRUE;

    if (m->manager.shadow_thickness == XmINVALID_DIMENSION) {
	XrmValue xrm_value;

	/* Depend on the enableThinThickness Display resource. */
	_XmSetThickness((Widget) m, (int) 0, &xrm_value);
	m->manager.shadow_thickness = *((Dimension *) xrm_value.addr);
    }

    m->row_column.old_width = XtWidth(m);
    m->row_column.old_height = XtHeight(m);
    m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

    /* Post initialization for whichButton - done before PopupInitialize
     * because RC_PostModifiers used in eventual XtGrabButton()
     */
    RC_PostModifiers(m) = AnyModifier;
    RC_PostEventType(m) = ButtonPress;

    /* allow menuPost override */
    if ((RC_MenuPost(m) != NULL) && !IsPulldown(m)) {
        if (_XmMapBtnEvent(RC_MenuPost(m), &RC_PostEventType(m),
            &RC_PostButton(m), &RC_PostModifiers(m)) == FALSE)
        {
            XmeWarning( (Widget)m,BadMenuPostMsg);
        }
	RC_MenuPost(m) = XtNewString(RC_MenuPost(m));
    }

    if (m->manager.navigation_type == XmDYNAMIC_DEFAULT_TAB_GROUP)
      {
        /* Call _XmNavigInitialize a second time only if XmNnavigationType
         * is XmDYNAMIC_DEFAULT_TAB_GROUP, which causes the first call
         * to _XmNavigInitialize to do nothing.
         */
        CallNavigInitAgain = TRUE ;
      }
    if (IsBar(m))
        MenuBarInitialize(m);
    else if (IsPopup(m))
        PopupInitialize(m);
    else if (IsPulldown(m))
        PulldownInitialize(m);
    else if (IsOption(m))
        OptionInitialize(m);
    else
        WorkAreaInitialize(m);

    /* CR 7807: Make sure the EntryClass's fast subclass bits are set. */
    if (RC_EntryClass(m) != NULL)
      XtInitializeWidgetClass(RC_EntryClass(m));

    if (m->manager.navigation_type == XmDYNAMIC_DEFAULT_TAB_GROUP)
        m->manager.navigation_type = XmTAB_GROUP;

    if(    CallNavigInitAgain    )
      {
        _XmNavigInitialize( rw, nw, args, num_args) ;
      }

    if (IsOption(m))
	_XmRC_SetOptionMenuHistory (m, (RectObj) RC_MemWidget (m));
    else
	_XmRC_SetMenuHistory (m, (RectObj) RC_MemWidget (m));

    if (!IsWorkArea(m) && (XmIsManager(XtParent(m))))
    {
       /* save the parent's accelerator widget.  Force it to NULL if this RC
	* is a menu so Manager.c: ConstraintInitialize() doesn't overwrite
	* menu's "accelerators".  This is ack-awwful!
	*/
       m->manager.accelerator_widget =
	  ((XmManagerWidget)XtParent(m))->manager.accelerator_widget;
       ((XmManagerWidget)XtParent(m))->manager.accelerator_widget = NULL;
    }

    /* Copy tearofftitle if given */
    if (RC_TearOffTitle(m) != NULL)
      RC_TearOffTitle(m) = XmStringCopy(RC_TearOffTitle(m));
}

static void
ConstraintInitialize(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{

    if (!XtIsRectObj(new_w)) return;

    WasManaged(new_w) = False;

    if (XmIsGadget(new_w) || XmIsPrimitive(new_w))
    {
      XmBaselineMargins textMargins;

      _XmRC_SetOrGetTextMargins(new_w, XmBASELINE_GET, &textMargins);
      SavedMarginTop(new_w) = textMargins.margin_top;
      SavedMarginBottom(new_w) = textMargins.margin_bottom;
    }
    /*
     * Restore parent of parent's accelerator widget.
     * accelerator widget should be null if parent's parent is not a manager!
     */
    if (((XmManagerWidget)XtParent(new_w))->manager.accelerator_widget)
    {
       ((XmManagerWidget)XtParent(XtParent(new_w)))->manager.accelerator_widget =
	  ((XmManagerWidget)XtParent(new_w))->manager.accelerator_widget;
       ((XmManagerWidget)XtParent(new_w))->manager.accelerator_widget = NULL;
    }
}

static Boolean
ConstraintSetValues(
                  Widget old,
                  Widget req,	/* unused */
                  Widget new_w,
                  ArgList args,	/* unused */
                  Cardinal *num_args) /* unused */
{
    XmRowColumnWidget rc = (XmRowColumnWidget) XtParent(new_w);
    register Widget tmp;
    int i ;
    XtWidgetGeometry current ;
    Boolean margins_changed;

    /* RCIndex (old) is valid in [0, num_children-1] and should stay valid */
    /* Note: the tearoffcontrol should not change its index value,
             undefined behavior */

    if (!XtIsRectObj(new_w)) return(FALSE);

    /* CR 7038: Readjust margins if they have changed. */
    if (XmIsLabelGadget(old))
      {
	margins_changed =
	  ((LabG_MarginTop(old) != LabG_MarginTop(new_w)) ||
	   (LabG_MarginBottom(old) != LabG_MarginBottom(new_w)) ||
	   (LabG_MarginLeft(old) != LabG_MarginLeft(new_w)) ||
	   (LabG_MarginRight(old) != LabG_MarginRight(new_w)));
      }
    else if (XmIsLabel(old))
      {
	margins_changed =
	  ((Lab_MarginTop(old) != Lab_MarginTop(new_w)) ||
	   (Lab_MarginBottom(old) != Lab_MarginBottom(new_w)) ||
	   (Lab_MarginLeft(old) != Lab_MarginLeft(new_w)) ||
	   (Lab_MarginRight(old) != Lab_MarginRight(new_w)));
      }
    else
      margins_changed = FALSE;
    if (margins_changed)
      _XmRCDoMarginAdjustment(rc);

    if (RCIndex(old) != RCIndex(new_w)) {

      /* special public value */
      if (RCIndex(new_w) == XmLAST_POSITION)
          RCIndex(new_w) = rc->composite.num_children - 1 ;

      if ((RCIndex(new_w) < 0) ||
          (RCIndex(new_w) >= rc->composite.num_children)) {
          RCIndex(new_w) = RCIndex(old) ;
      } else {
          int inc ;       /* change the configuration of the children list:
             put the requesting child at the new position and
             shift the others as needed (2 cases here) */
          tmp = rc->composite.children[RCIndex(old)] ;
          if (RCIndex(new_w) < RCIndex(old)) inc = -1 ; else inc = 1 ;

          for (i = RCIndex(old)  ; i != RCIndex(new_w) ; i+=inc) {
              rc->composite.children[i] = rc->composite.children[i+inc];
              RCIndex(rc->composite.children[i]) = i ;
          }
          rc->composite.children[RCIndex(new_w)] = tmp ;

          /* save the current geometry of the child */
	  current.x = XtX(new_w) ;
	  current.y = XtY(new_w) ;
	  current.width = XtWidth(new_w) ;
	  current.height = XtHeight(new_w) ;
	  current.border_width = XtBorderWidth(new_w) ;

          /* re-layout, move the child and possibly change the rc size */
          WasManaged(new_w) = False ; /* otherwise, changemanaged just exits */

	  /* call the ChangeManaged of the real superclass */
	  ChangeManaged((Widget) rc) ;

          /* as we have changed the position/size of this child, next
             step after this setvalues chain is the geometry manager
             request. We need to tell the geometry manager that
             this request is to be always honored. As the positionIndex field
             itself is self-computable, we can use it to track this
             case. We set it to a magic value here, and in the geometry
             manager, we'll have to reset it to its correct value by
             re-computing it - adding a field in the instance is another way
             for doing that, clever but more expensive */
         if ((current.x != XtX(new_w)) ||
	     (current.width != XtWidth(new_w)) ||
	     (current.height != XtHeight(new_w)) ||
	     (current.border_width != XtBorderWidth(new_w)))
            RCIndex(new_w) = XmLAST_POSITION ;

	  return (True);
      }
    }
    return (False);
}

/*
 * the main create section, mostly just tacks on the type to the arg
 * list
 */
static Widget
create(
        Widget p,               /* parent widget */
        char *name,
        ArgList old_al,
        Cardinal old_ac,
        int type,               /* menu kind to create */
        int is_radio )          /* the radio flag */
{
    Arg al[256];
    Widget m;
    int i, ac = 0;

    if (is_radio)               /* get ours in ahead of the */
    {                           /* caller's, so his override */
        XtSetArg (al[ac], XmNpacking, XmPACK_COLUMN);    ac++;
        XtSetArg (al[ac], XmNradioBehavior, is_radio); ac++;
        XtSetArg (al[ac], XmNisHomogeneous, TRUE); ac++;
        XtSetArg (al[ac], XmNentryClass, xmToggleButtonGadgetClass); ac++;
    }

    for (i=0; i<old_ac; i++) al[ac++] = old_al[i];  /* copy into our list */

    if (type != UNDEFINED_TYPE)
    {
       XtSetArg (al[ac], XmNrowColumnType,  type);
       ac++;
    }

    /*
     * decide if we need to build a popup shell widget
     */

    if ((type == XmMENU_PULLDOWN) || (type == XmMENU_POPUP))
    {
        Arg s_al[256];
        XmMenuShellWidget pop = NULL;
        Widget pw;
        int s_ac = 0;

        /*
         * if this is a pulldown of a pulldown or popup then the parent
         * should really be the shell of the parent not the indicated
         * parent, this keeps the cascade tree correct
         */

        if ((XtParent(p) != NULL) && XmIsMenuShell(XtParent (p)))
            pw = XtParent (p);
        else
            pw = p;

        /*
         * Shared menupanes are supported for all menu types but the option
         * menu.  If this is not an option menupane, then see if a shell is
         * already present; if so, then we'll use it.
         */
        if (XmIsRowColumn(p) && (IsBar(p) || IsPopup(p) || IsPulldown(p)))
        {
            for (i = 0; i < pw->core.num_popups; i++)
            {
                if ((XmIsMenuShell(pw->core.popup_list[i])) &&
                   (((XmMenuShellWidget)pw->core.popup_list[i])->menu_shell.
                                                            private_shell) &&
                   (!(pw->core.popup_list[i])->core.being_destroyed))
                {
                    pop = (XmMenuShellWidget)pw->core.popup_list[i];
                    break;
                }
            }
        }

        /* No shell - create a new one */
        if (pop == NULL)
        {
            char *b;

            /* should pass in the old al */
            for (i=0; i<old_ac; i++) s_al[s_ac++] = old_al[i];

            XtSetArg (s_al[s_ac], XmNwidth,             5);     s_ac++;
            XtSetArg (s_al[s_ac], XmNheight,        5);     s_ac++;
            XtSetArg (s_al[s_ac], XmNallowShellResize, TRUE);   s_ac++;
            XtSetArg (s_al[s_ac], XtNoverrideRedirect, TRUE);   s_ac++;
            XtSetArg (s_al[s_ac], XtNancestorSensitive, TRUE);  s_ac++;

	    b = XtMalloc(strlen(POPUP_PREFIX) + strlen(name) + 1);
            sprintf (b, POPUP_PREFIX, name);

            pop = (XmMenuShellWidget)XtCreatePopupShell(b,
                     xmMenuShellWidgetClass, pw, s_al, s_ac);

            /* Mark the shell as having been created by us */
            pop->menu_shell.private_shell = True;

	    XtFree(b);
        }

        m = XtCreateWidget ( name, xmRowColumnWidgetClass, (Widget) pop, al, ac);
    }
    else
        m = XtCreateWidget (name, xmRowColumnWidgetClass, (Widget) p, al, ac);

    return (m);
}


/*
 *************************************************************************
 *
 * Public Routines
 *
 *************************************************************************
 */

Widget
XmCreateRowColumn(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
   Widget w;
   _XmWidgetToAppContext(p);

   _XmAppLock(app);
    w = create (p, name, al, ac, UNDEFINED_TYPE, FALSE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmVaCreateRowColumn(
        Widget parent,
        char *name,
        ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmRowColumnWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedRowColumn(
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
                         xmRowColumnWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
Widget
XmCreateWorkArea(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
   Widget w;
   _XmWidgetToAppContext(p);

   _XmAppLock(app);
    w = create (p, name, al, ac, XmWORK_AREA, FALSE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmCreateRadioBox(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
   Widget w;
   _XmWidgetToAppContext(p);

   _XmAppLock(app);
    w = create (p, name, al, ac, XmWORK_AREA, TRUE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmCreateOptionMenu(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
   Widget w;
   _XmWidgetToAppContext(p);

   _XmAppLock(app);
    w = create (p, name, al, ac, XmMENU_OPTION, FALSE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmOptionLabelGadget(
        Widget m )
{
   int i;
   Widget child;
   _XmWidgetToAppContext(m);

   _XmAppLock(app);

   if (XmIsRowColumn(m) && IsOption(m))
   {
      XmRowColumnWidget rowcol = (XmRowColumnWidget) m;

      if (rowcol->core.being_destroyed)  {
	 _XmAppUnlock(app);
         return NULL;
      }

      for (i = 0; i < rowcol->composite.num_children; i++)
      {
	 child = rowcol->composite.children[i];

	 if (XtClass(child) == xmLabelGadgetClass) {
	     _XmAppUnlock(app);
	     return (child);
	 }
      }
   }

   _XmAppUnlock(app);
   /* did not find a label gadget in the child list */
   return (NULL);
}

Widget
XmOptionButtonGadget(
        Widget m )
{
   int i;
   Widget child;
   _XmWidgetToAppContext(m);

   _XmAppLock(app);
   if (XmIsRowColumn(m) && IsOption(m))
   {
      XmRowColumnWidget rowcol = (XmRowColumnWidget) m;

      if (rowcol->core.being_destroyed)  {
	 _XmAppUnlock(app);
	 return NULL;
      }

      for (i = 0; i < rowcol->composite.num_children; i++)
      {
	 child = rowcol->composite.children[i];

	 if (XmIsCascadeButtonGadget(child)) {
	     _XmAppUnlock(app);
	     return (child);
	 }
      }
   }

   _XmAppUnlock(app);
   /* did not find a cascadebuttongadget in the child list */
   return (NULL);
}

Widget
XmCreateMenuBar(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
    Widget w;
    _XmWidgetToAppContext(p);

    _XmAppLock(app);
    w = create (p, name, al, ac, XmMENU_BAR, FALSE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmCreatePopupMenu(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
    Widget w;
    _XmWidgetToAppContext(p);

    _XmAppLock(app);
    w = create (p, name, al, ac, XmMENU_POPUP, FALSE);
    _XmAppUnlock(app);
    return w;
}

Widget
XmCreatePulldownMenu(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
    Widget w;
    _XmWidgetToAppContext(p);

    _XmAppLock(app);
    w =  create (p, name, al, ac, XmMENU_PULLDOWN, FALSE);
    _XmAppUnlock(app);
    return w;
}


/*
 * class initialization
 */
static void
ClassInitialize( void )
{
   /*
    * parse the various translation tables
    */

   menu_parsed   = XtParseTranslationTable (menu_table);
   bar_parsed    = XtParseTranslationTable (bar_table);
   option_parsed    = XtParseTranslationTable (option_table);
   menu_traversal_parsed = XtParseTranslationTable (menu_traversal_table);

   /* set up base class extension quark */
   baseClassExtRec.record_type = XmQmotif;

   /* set up the menu procedure entry for button children to access */
   _XmSaveMenuProcContext( (XtPointer) _XmRCMenuProcedureEntry);

   /* Trait records */
   XmeTraitSet((XtPointer) xmRowColumnWidgetClass, XmQTmenuSystem,
	       (XtPointer) &_XmRC_menuSystemRecord);
}

static void
ClassPartInitialize(
        WidgetClass rcc )
{
    _XmFastSubclassInit(rcc,XmROW_COLUMN_BIT);

    /* Allow inheritance for subclasses */
    if ((WidgetClass) rcc != xmRowColumnWidgetClass) {
      XmRowColumnClass this_class = (XmRowColumnClass) rcc;
      XmRowColumnClass super = (XmRowColumnClass) rcc -> core_class.superclass;

      if (this_class -> row_column_class.menuProcedures ==
	  XmInheritMenuProceduresProc)
	this_class -> row_column_class.menuProcedures =
	  super -> row_column_class.menuProcedures;
      if (this_class -> row_column_class.armAndActivate ==
	  XmInheritArmAndActivateProc)
	this_class -> row_column_class.armAndActivate =
	  super -> row_column_class.armAndActivate;
      if (this_class -> row_column_class.traversalHandler ==
	  XmInheritMenuTraversalProc)
	this_class -> row_column_class.traversalHandler =
	  super -> row_column_class.traversalHandler;
    }
}

/************************************************************
 *
 * InitializePrehook
 *
 * Put the proper translations in core_class tm_table so that
 * the data is massaged correctly
 *
 ************************************************************/
static void
InitializePrehook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  _XmSaveCoreClassTranslations (new_w);

  _XmProcessLock();
  if ((RC_Type(new_w) == XmMENU_PULLDOWN) ||
      (RC_Type(new_w) == XmMENU_POPUP))
  {
      new_w->core.widget_class->core_class.tm_table = (String) menu_parsed;
  }
  else if (RC_Type(new_w) == XmMENU_OPTION)
  {
      new_w->core.widget_class->core_class.tm_table = (String) option_parsed;
  }
  else if (RC_Type(new_w) == XmMENU_BAR)
  {
      new_w->core.widget_class->core_class.tm_table = (String) bar_parsed;
  }
  else
      new_w->core.widget_class->core_class.tm_table =
	  xmManagerClassRec.core_class.tm_table;
  _XmProcessUnlock();
}


/************************************************************
 *
 * InitializePosthook
 *
 * restore core class translations
 *
 ************************************************************/
static void
InitializePosthook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  _XmRestoreCoreClassTranslations (new_w);
}

/*
 * Class method for traversal.  If there is a tear off control visible,
 * it must be put into a list so that the traversal code can find it.
 */
static Boolean
TraversalChildren (
        Widget wid ,
        Widget ** childList ,
        Cardinal * numChildren )
{
    XmRowColumnWidget rc = (XmRowColumnWidget) wid;
    int i;

    if (RC_TearOffControl(rc))
    {
     /*
      * add the TOC to the children list
      */
     *childList =
       (WidgetList) XtMalloc(sizeof(Widget) * (rc->composite.num_children+1));

     (*childList)[0] = RC_TearOffControl(rc);

     for (i=1; i <= rc->composite.num_children; i++)
     {
       (*childList)[i] = rc->composite.children[i-1];
     }

     *numChildren = rc->composite.num_children+1;

     return (True);
 }
   else
       return (False);
}


/**********************************************************************
 *
 * next section knows how to composite row column entries
 */
static void
FixEventBindings(
        XmRowColumnWidget m,    /* row column (parent) widget */
        Widget w )              /* subwidget */
{
   if (XtIsWidget(w) &&
       ((IsPopup(m) || IsBar(m) || IsPulldown(m)) &&
	 XmIsLabel(w) && (w->core.widget_class != xmLabelWidgetClass)))
   {
      XtAddEventHandler(w, KeyPressMask|KeyReleaseMask, False,
			_XmRC_KeyboardInputHandler, m);

   }

   /* set up accelerators and mnemonics */
   _XmRC_ProcessSingleWidget (w, XmADD);
}


/*****************************************************************************
 *
 * RowColumn's map and unmap callbacks funneled through here.
 *
 *****************************************************************************/

void
_XmCallRowColumnMapCallback(
	Widget wid,
	XEvent *event )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XmRowColumnCallbackStruct callback;

   if (!RC_Map_cb(rc))
      return;

   callback.reason = XmCR_MAP;
   callback.event  = event;
   callback.widget = NULL;      /* these next two fields are spec'd NULL */
   callback.data   = NULL;
   callback.callbackstruct = NULL;
   XtCallCallbackList ((Widget)rc, RC_Map_cb(rc), &callback);
}

void
_XmCallRowColumnUnmapCallback(
	Widget wid,
	XEvent *event )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid;
   XmRowColumnCallbackStruct callback;

   if (!RC_Unmap_cb(rc))
      return;

   callback.reason = XmCR_UNMAP;
   callback.event  = event;
   callback.widget = NULL;      /* these next two fields are spec'd NULL */
   callback.data   = NULL;
   callback.callbackstruct = NULL;
   XtCallCallbackList ((Widget)rc, RC_Unmap_cb(rc), &callback);
}


/**************************************************************************
 *
 * class support procedures
 */


static Widget
FindFirstManagedChild(
        CompositeWidget m,
        Boolean first_button )
{
    register Widget *kid;
    register int i = 0;
    register int n;

    if (!m)
       return(NULL);

    kid = m->composite.children;
    n = m->composite.num_children;

    /* This used to use XmIsPushButton(Gadget) and XmIsToggleButton(Gadget) */
    while( (i < n) &&
 	  ((*kid)->core.being_destroyed ||
	   (!XtIsManaged(*kid) ||
	   (first_button &&
	    !(XmIsTraversable(*kid))
	   ))) )
        kid++, i++;

    if (i >= n)
        return(NULL);
    else
        return(*kid);
}

/*
 * Resize the row column widget, and any subwidgets there are.
 * Since the gravity is set to NW, handle shrinking when there may not
 * be a redisplay.
 */
static void
Resize(
        Widget wid )
{
        XmRowColumnWidget m = (XmRowColumnWidget) wid ;
   Boolean		draw_shadow = False;

   RC_SetFromResize(m, TRUE);


   if ( (m->row_column.old_width != m->core.width)
      ||(m->row_column.old_height != m->core.height)
      ||(m->row_column.old_shadow_thickness != m->manager.shadow_thickness) )
   /* clear the shadow if its needed (will check if its now larger) */
   _XmClearShadowType( (Widget) m, m->row_column.old_width,
		       m->row_column.old_height,
		       m->row_column.old_shadow_thickness, 0);

   /*
    * if it is now smaller, redraw the shadow since there may not be a
    * redisplay - DON'T draw shadows for OPTION MENUS!
    */
   if (!IsOption(m) &&
       (m->row_column.old_height > m->core.height ||
        m->row_column.old_width > m->core.width))
       draw_shadow = True;

   m->row_column.old_width = m->core.width;
   m->row_column.old_height = m->core.height;
   m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

   _XmRCAdaptToSize (m, NULL, NULL);

   if (draw_shadow && XtIsRealized ((Widget)m) && m->manager.shadow_thickness )
       /* pop-out not pop-in */
     XmeDrawShadows (XtDisplay (m), XtWindow (m),
		     m->manager.top_shadow_GC,
		     m->manager.bottom_shadow_GC,
		     0, 0, m->core.width, m->core.height,
		     m->manager.shadow_thickness,
		     XmSHADOW_OUT);

   RC_SetFromResize(m, FALSE);
}


/*
 * class Redisplay proc
 */
static void
Redisplay(
        Widget w,
        XEvent *event,
        Region region )
{
    XmRowColumnWidget m = (XmRowColumnWidget) w;
    XEvent tempEvent;

    /* Ignore exposures generated as we unpost */
    if ((IsPopup (m) || IsPulldown (m)) &&
        !((XmMenuShellWidget)XtParent(m))->shell.popped_up)
    {
       RC_SetExpose (m, TRUE);
       return;
    }

    if (RC_DoExpose (m))            /* a one-shot set on popup */
    {                               /* so we ignore next expose */

        if (event == NULL)          /* Fast exposure is happening */
        {
            event = &tempEvent;
            event->xexpose.x = 0;
            event->xexpose.y = 0;
            event->xexpose.width = m->core.width;
            event->xexpose.height = m->core.height;
        }

        XmeRedisplayGadgets( (Widget) m, event, region);

        if (IsPopup (m) || IsPulldown (m) || IsBar(m))
        {
            if (MGR_ShadowThickness(m))
                XmeDrawShadows (XtDisplay (m), XtWindow (m),
                    /* pop-out not pop-in */
                    m->manager.top_shadow_GC,
                    m->manager.bottom_shadow_GC,
                    0, 0, m->core.width, m->core.height,
                    m->manager.shadow_thickness,
		    XmSHADOW_OUT);
        }
    }

    RC_SetExpose (m, TRUE);
}



/*
 * fix the visual attributes of the subwidget to be what we like
 *
 *  1.  make border width uniform
 *  2.  maybe set the label alignment
 */
static void
FixVisual(
        XmRowColumnWidget m,
        Widget w )
{
   Arg al[10];
   int ac;

   if (RC_EntryBorder(m))
   {
       /* fix for 7660, setting entryborder before realize time
	       has the width and height moved from 0 to 1, a side-effect
	       of XmeConfigureObject, and then the widget loses its
	       preferred geometry. A better fix might be to remove the
	       check for 0 in XmeConfigureObject, but this might
	       be a behavior compatibility issue */
       if (XtIsRealized(w))
	   XmeConfigureObject(w, w->core.x,w->core.y,
			      w->core.width, w->core.height,
			      RC_EntryBorder(m)) ;
       else
	   w->core.border_width = RC_EntryBorder(m);
   }

   if (IsOption(m))
       return;

   if (XmIsLabelGadget(w))
   {
      if (IsAligned (m))
      {
	 if (IsWorkArea(m) ||
	     ((w->core.widget_class != xmLabelWidgetClass) &&
	      (w->core.widget_class != xmLabelGadgetClass)))

	 {
	    ac = 0;
	    XtSetArg (al[ac], XmNalignment, RC_EntryAlignment (m));
	    ac++;
	    XtSetValues (w, al, ac);
	 }
      }
   }
   else if (XmIsLabel (w))
   {
      if (IsAligned (m))
      {
	 if ((w->core.widget_class != xmLabelWidgetClass) ||
	     IsWorkArea(m))
	 {
	    ac = 0;
	    XtSetArg (al[ac], XmNalignment, RC_EntryAlignment (m));
	    ac++;
	    XtSetValues (w, al, ac);
	 }
      }
   }
}

/*
 * If an entryCallback exists, set a flag in the buttons to not do
 * their activate callbacks.
 */
static void
FixCallback(
        XmRowColumnWidget m,
        Widget w )
{
  XmMenuSavvyTrait menuSavvyRec;
  char *c;

  menuSavvyRec = (XmMenuSavvyTrait)
    XmeTraitGet((XtPointer) XtClass(w), XmQTmenuSavvy);

  if (menuSavvyRec == (XmMenuSavvyTrait) NULL ||
      menuSavvyRec -> getActivateCBName == NULL)
    return;

  c = menuSavvyRec -> getActivateCBName();

  if (c == NULL)
    return;          /* can't do it to this guy */

   if (m->row_column.entry_callback)
   {
     /*
      * Disable the buttons activate callbacks
      */
     if (menuSavvyRec->disableCallback != (XmMenuSavvyDisableProc) NULL )
       (menuSavvyRec->disableCallback)(w, XmDISABLE_ACTIVATE);
   }
}

static void
ActionNoop(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   /*
    * Do nothing; the purpose is to override the actions taken by the
    * Primitive translations.
    */
}

static void
EventNoop(
        Widget reportingWidget,
        XtPointer data,
        XEvent *event,
        Boolean *cont )
{
   /*
    * Do nothing; the purpose is to override the actions taken by the
    * Primitive translations.
    */
}

static void
MenuFocusIn(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   /*
    * For popup and pulldown menupanes, we want to ignore focusIn request
    * which occur when we are not visible.
    */

   if (IsBar(rc))
      _XmManagerFocusIn( (Widget) rc, event, NULL, NULL);
   else if ((((XmMenuShellWidget)XtParent(rc))->shell.popped_up) &&
	    !_XmGetInDragMode((Widget) rc))
      _XmManagerFocusInInternal( (Widget) rc, event, NULL, NULL);
}

static void
MenuFocusOut(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
  XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
  _XmManagerFocusOut( (Widget) rc, event, NULL, NULL);
  /* Attempt to fix locking up problem with system modal
     dialogs.  CR 6795 */
  XAllowEvents(XtDisplay(wid), SyncPointer, CurrentTime);
}

static void
MenuUnmap(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   /*
    * For popup and pulldown menupanes, we never care about being notified
    * when we are unmapped.  For menubars, we want normal unmapping
    * processing to occur.
    */

   if (IsBar(wid))
      _XmManagerUnmap( wid, event, params, num_params);
}

static void
MenuEnter(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   if (IsBar(rc) && RC_IsArmed(rc))
       return;

   _XmManagerEnter( (Widget) rc, event, NULL, NULL);
}


/*
 * Catch an 'Escape' which occurs within a gadget, and bring down the
 * menu system.
 */
static void
GadgetEscape(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
   /* Process the event only if not already processed */
   if (!_XmIsEventUnique(event))
      return;

    if (IsBar(rc))
    {
        /*
         * We're in the PM menubar mode, so let our own arm and activate
         * procedure clean things up .
         */
        if (RC_IsArmed(rc))
            (* (((XmRowColumnClassRec *) (rc->core.widget_class))->
				row_column_class.armAndActivate))
					((Widget) rc, event, NULL, NULL);
    }
    else
    {
        /* Let the menushell widget clean things up */
        (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
          menu_shell_class.popdownOne))(XtParent(rc), event, NULL, NULL);
    }

   _XmRecordEvent(event);
}

/*
 * Copy the String in XmNmnemonicCharSet before returning it to the user.
 */
static void
GetMnemonicCharSet(
            Widget wid,
            int resource,	/* unused */
            XtArgVal *value)
{
  String        data ;
  Arg		al[1];
  Widget	label;

  label = XmOptionLabelGadget(wid);

  if (label)
  {
     XtSetArg(al[0], XmNmnemonicCharSet, &data);
     XtGetValues(label, al, 1);

     *value = (XtArgVal) data;
  }
  else
     *value = (XtArgVal) NULL;

  return ;
}

/*
 * Copy the String in XmNmenuAccelerator before returning it to the user.
 */
static void
GetMenuAccelerator(
            Widget wid,
            int resource,	/* unused */
            XtArgVal *value )
{
  String        data ;
  XmRowColumnWidget rc  = (XmRowColumnWidget) wid;

  if (rc->row_column.menu_accelerator != NULL) {
     data = (String)XtMalloc(strlen(RC_MenuAccelerator(rc)) + 1);
     strcpy(data, RC_MenuAccelerator(rc));
     *value = (XtArgVal) data ;
   }
  else *value = (XtArgVal) NULL;

  return ;
}

/*
 * Copy the String in XmNmenuPost before returning it to the user.
 */
static void
GetMenuPost(
       Widget wid,
       int resource,		/* unused */
       XtArgVal * value )
{
   XmRowColumnWidget rc = (XmRowColumnWidget) wid ;

   if (rc->row_column.menuPost != NULL)
   {
      *value = (XtArgVal) XtNewString(RC_MenuPost(rc)) ;
   }
   else *value = (XtArgVal) NULL;

   return ;
}

/*
 * Copy the XmString in XmNlabelString before returning it to the user.
 */
static void
GetLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
  XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
  XmString        data ;

  data = XmStringCopy(RC_OptionLabel(rc));
  *value = (XtArgVal) data ;

  return ;
}

/*
 * Copy the XmString in XmNtearOffTitle before returning it to the user.
 */
static void
GetTearOffTitle(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
  XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
  XmString        data ;
/****************/

  data = XmStringCopy(RC_TearOffTitle(rc));
  *value = (XtArgVal) data ;

  return ;
}

static XmNavigability
WidgetNavigable(
        Widget wid)
{
  XmNavigationType nav_type = ((XmManagerWidget) wid)
	                                            ->manager.navigation_type ;
  /* Need to make sure that XmDYNAMIC_DEFAULT_TAB_GROUP causes
   * return value of XmNOT_NAVIGABLE, so that initial call
   * to _XmNavigInitialize from Manager Initialize does nothing.
   */
  if(    XtIsSensitive(wid)
     &&  ((XmManagerWidget) wid)->manager.traversal_on
     &&  (nav_type != XmDYNAMIC_DEFAULT_TAB_GROUP)    )
    {
      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	 ||  (    (nav_type == XmTAB_GROUP)
	      &&  !_XmShellIsExclusive( wid))    )
	{
	  return XmDESCENDANTS_TAB_NAVIGABLE ;
	}
      return XmDESCENDANTS_NAVIGABLE ;
    }
  return XmNOT_NAVIGABLE ;
}
