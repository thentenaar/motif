/* $XConsortium: TextPerfIn.c /main/5 1995/07/15 21:10:24 drk $ */
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

/***********************************************************
    TextPerf.scr refers to the number of lines in this file.
    If the number of lines in this file change, TextPerf.scr
    must change as well.
 ***********************************************************/

#include <stdio.h>
#include <ctype.h>
#include <Xm/XmP.h>
#include <X11/Xutil.h>
#include <X11/ShellP.h>
#include <Xm/MenuShellP.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/CascadeBGP.h>
#include <Xm/ToggleBP.h>
#include <Xm/ToggleBGP.h>
#include <Xm/RowColumnP.h>

#define UNDEFINED_TYPE -1
#define POST_TIME_OUT	3 /* sec */ * 1000

#define Double(x)       ((x) << 1)
#define Half(x)         ((x) >> 1)

#define IsSensitive(r)      XtIsSensitive(r)
#define IsManaged(w)        XtIsManaged(w)
#define IsNull(p)       ((p) == NULL)

#define PackTight(m)        (RC_Packing (m) == XmPACK_TIGHT)
#define PackColumn(m)       (RC_Packing (m) == XmPACK_COLUMN)
#define PackNone(m)         (RC_Packing (m) == XmPACK_NONE)

#define Asking(i)       ((i) == 0)
#define IsVertical(m)   \
       (((XmRowColumnWidget) (m))->row_column.orientation == XmVERTICAL)
#define IsHorizontal(m) \
       (((XmRowColumnWidget) (m))->row_column.orientation == XmHORIZONTAL)
#define IsAligned(m)    \
       (((XmRowColumnWidget) (m))->row_column.do_alignment)

#define IsPopup(m)     \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_POPUP)
#define IsPulldown(m)  \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_PULLDOWN)
#define IsOption(m)    \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_OPTION)
#define IsBar(m)       \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_BAR)
#define IsWorkArea(m)  \
    (((XmRowColumnWidget) (m))->row_column.type == XmWORK_AREA)
#define IsRadio(m)     \
    ((((XmRowColumnWidget) (m))->row_column.type == XmWORK_AREA) && \
         (((XmRowColumnWidget) (m)->row_column.radio)))
#define IsHelp(m,w)     ((w) == RC_HelpPb (m))

#define WasManaged(w)  \
    (((XmRowColumnConstraintRec *) ((w)->core.constraints))-> \
     row_column.was_managed)

#define BX(b)           ((b)->x)
#define BY(b)           ((b)->y)
#define BWidth(b)       ((b)->width)
#define BHeight(b)      ((b)->height)
#define BBorder(b)      ((b)->border_width)

#define SetPosition(b,x,y)  { BX (b) = x;  BY (b) = y; }

#define ChangeMargin(margin,new,sum)    \
    if ((margin) != new)        \
    {               \
        sum += new - (margin);  \
        (margin) = new;     \
    }

#define ForAllChildren(m, i, q)     \
    for (i = 0, q = m->composite.children; \
     i < m->composite.num_children;     \
     i++, q++)

#define ForManagedChildren(m, i, q)  \
    for (i = 0, q = m->composite.children; \
     i < m->composite.num_children;     \
     i++, q++)          \
                    \
    if (XtIsManaged(*q))

     
/* Warning Messages */
#define BadWidthMsg \
    "Attempt to set width to zero: defaulting to 16"
#define BadWidthSVMsg \
    "Attempt to set width to zero ignored"
#define BadHeightMsg \
    "Attempt to set height to zero: defaulting to 16"
#define BadHeightSVMsg \
    "Attempt to set height to zero ignored"
#define BadPopupHelpMsg \
    "XmNhelpWidget not used by PopUps: forced to NULL"
#define BadPulldownHelpMsg \
    "XmNhelpWidget not used by Pulldowns: forced to NULL"
#define BadOptionHelpMsg \
    "XmNhelpWidget not used by Option menus: forced to NULL"
#define BadWorkAreaHelpMsg \
    "XmNhelpWidget not used by Work Areas: forced to NULL"
#define BadTypeMsg \
    "Unknown value of XmNrowColumnType: defaulting to WorkArea"
#define BadTypeParentMsg \
    "Widget hierarchy not appropriate for this XmNrowColumnType:\n\
defaulting to WorkArea"
#define BadTypeSVMsg \
    "Attempt to change XmNrowColumnType after initialization: ignored"
#define BadOrientationMsg \
    "Unknown value of XmNorientation: using default instead"
#define BadOrientationSVMsg \
    "Attempt to set XmNorientation to unknown value ignored"
#define BadPackingMsg \
    "Unknown value of XmNpacking: using default instead"
#define BadPackingSVMsg \
    "Attempt to set XmNpacking to unknown value ignored"
#define BadAlignmentMsg \
    "Unknown value of XmNentryAlignment: using default instead"
#define BadAlignmentSVMsg \
    "Attempt to set XmNentryAlignment to unknown value ignored"
#define BadMenuBarHomogenousSVMsg \
    "Attempt to set XmNisHomogenous to FALSE for a RowColumn widget of type \
     XmMENU_BAR ignored"
#define BadMenuBarEntryClassSVMsg \
    "Attempt to change XmNentryClass for a RowColumn widget of type \
     XmMENU_BAR ignored"
#define BadPulldownWhichButtonSVMsg \
    "Attempt to change XmNwhichButton via XtSetValues for a RowColumn widget \
     of type XmMENU_PULLDOWN ignored"
#define BadPulldownMenuPostSVMsg \
    "Attempt to change XmNmenuPost via XtSetValues for a RowColumn widget \
     of type XmMENU_PULLDOWN ignored"
#define BadMenuPostMsg \
    "Attempt to set XmNpostMenu to an illegal value ignored"

#define BadShadowThicknessSVMsg \
    "Attempt to change XmNshadowThickness for a RowColumn widget not of type \
     XmMENU_PULLDOWN or XmMENU_POPUP ignored"
#define BadOptionOrientationSVMsg \
    "Attempt to change XmNorientation for a RowColumn widget of type \
     XmMENU_OPTION ignored"

#define WrongMenuChildMsg \
    "Attempt to add wrong type child to a menu (i.e. RowColumn) widget"
#define WrongChildMsg \
    "Attempt to add wrong type child to a homogeneous RowColumn widget"


/*
 * forward declarations
 */

                        /* class support procs */
static Boolean          SetValues();
static int              AddKid();
static XtGeometryResult QueryGeometry();
static void             Redisplay();
static void             ManagedSetChanged();
static void             ClassInitialize();
static void             ClassPartInitialize();
static void             Initialize();
static void             Resize();
static XtGeometryResult GeometryManager();
static void             RemoveChild();
static void             Realize();
static void             Destroy();
static void             ConstraintDestroy();
static void             ConstraintInitialize();
static void             _XmMenuTraversalHandler();
static void             MoveLeft();
static void             MoveRight();
static void             Move_Up_Down();
static void             FindPrevMenuBarCascade();
static void             FindNextMenuBarCascade();
static void             FindPrevMenuBarItem();
static void             FindNextMenuBarItem();
static void             DoNothing();
static void 		PreparePostFromList();


                        /* action procs */
static void _XmMenuBtnUp();
static void _XmMenuBtnDown();
static void _XmGetGadget();
static void _XmRC_Unmap();
static void _XmRC_Enter();
static void _XmRC_FocusIn();
static void _XmRC_FocusOut();
static void _XmRC_GadgetEscape();
static void _XmRC_GadgetTraverseLeft();
static void _XmRC_GadgetTraverseRight();
static void _XmRC_GadgetTraverseUp();
static void _XmRC_GadgetTraverseDown();
static Boolean ShouldDispatchFocusOut();

static void _XmMenuTraverseLeft();
static void _XmMenuTraverseRight();
static void _XmMenuTraverseUp();
static void _XmMenuTraverseDown();
static void _XmMenuEscape();
static void _XmMenuUnmap();
static void _XmMenuFocusOut();
static void _XmMenuFocusIn();
static void Noop();

void _XmGetActiveTopLevelMenu();
void XmAddToPostFromList ();
void XmRemoveFromPostFromList ();
static void AddToPostFromList ();
static void RemoveFromPostFromList ();

                       /* RowColumnClass methods */
static void PositionMenu();
static void SetCascadeField();
static void ArmAndActivate();
static void MenuPopDown();
static void MenuArm();
static void SetMenuTraversal();
static void MenuDisarm();
static void MenuBarCleanup();
static void MenuProcedureEntry();

                        /* utility procs */
static void FixEventBindings();
static void FixVisual();
static void FixCallback();
static void LocatePulldown();
static void PreferredSize();

static void ChildsActivateCallback();
static void EntryFired();
static void AdaptToSize();
static void DoMarginAdjustment();
static void SetMenuHistory();
static void Layout();

static void DoProcessMenuTree();
static int  _XmMatchInKeyboardList();
static Boolean _XmAllWidgetsAccessible();

static Boolean CheckKey();
static Boolean ProcessKey();
static void GrabKeyOnAssocWidgets ();

static int OnPostFromList();
static void GetTopManager();
static void ProcessMenuTree();
static void ProcessSingleWidget();
static void AddToKeyboardList();
static void RemoveFromKeyboardList();
static void KeyboardInputHandler();
static void SetTraversal();
static void UpdateOptionMenuCBG ();
static Widget find_first_managed_child();

extern Widget _XmGetTabGroup();
extern Widget _XmFindTopMostShell();
extern void _XmGadgetArm();
extern void _XmGadgetActivate();
extern void _XmGadgetSelect();

/*
 * event translation tables for a menu widget, we use the parameters to
 * signal that this widget invoking the action proc is the menu, not a
 * child of the menu
 */

static XtTranslations menu_traversal_parsed;
#ifndef MCCABE
static char menu_traversal_table [] =
   "<Key>osfHelp:	Help()\n\
    <Key>osfLeft:	MenuGadgetTraverseLeft()\n\
    <Key>osfRight:	MenuGadgetTraverseRight()\n\
    <Key>osfUp:		MenuGadgetTraverseUp()\n\
    <Key>osfDown:	MenuGadgetTraverseDown()\n\
    <Unmap>:		MenuUnmap()\n\
    <FocusIn>:		MenuFocusIn()\n\
    <FocusOut>:		MenuFocusOut()\n\
    <EnterWindow>Normal:	MenuEnter()";
#else
static char menu_traversal_table [];
#endif

static XtTranslations option_parsed;
#ifndef MCCABE
static char option_table [] =
    "<Key>osfSelect: 	ManagerGadgetSelect()\n\
     <Key>osfActivate:	ManagerGadgetSelect()\n\
     <Key>osfHelp:	Help()\n\
     ~Shift ~Meta ~Alt<Key>Return:	ManagerGadgetSelect()\n\
     ~Shift ~Meta ~Alt<Key>space:	ManagerGadgetSelect()\n\
     <BtnDown>:		MenuBtnDown()\n\
     <BtnUp>:		MenuBtnUp()";
#else
static char option_table [];
#endif

static XtTranslations menu_parsed;
#ifndef MCCABE
static char menu_table [] =
    "<Key>osfSelect: 	ManagerGadgetSelect()\n\
     <Key>osfActivate:	ManagerGadgetSelect()\n\
     <Key>osfHelp:	Help()\n\
     <Key>osfCancel: 	MenuGadgetEscape()\n\
     ~Shift ~Meta ~Alt<Key>Return:	ManagerGadgetSelect() \n\
     ~Shift ~Meta ~Alt<Key>space:	ManagerGadgetSelect() \n\
     <BtnDown>:		MenuBtnDown()\n\
     <BtnUp>:		MenuBtnUp()";
#else
static char menu_table [];
#endif


/* this is used exclusively for the help system */
static XmGadget ActiveGadgetChild = NULL;


/*
 * action binding table for row column widget
 */

static XtActionsRec action_table [] =
{
    {"Help",                (XtActionProc) _XmManagerHelp},
    {"MenuBtnDown",	    (XtActionProc) _XmMenuBtnDown},
    {"MenuBtnUp",	    (XtActionProc) _XmMenuBtnUp},
    {"PulldownBtnDown",     (XtActionProc) _XmMenuBtnDown},
    {"PulldownBtnUp",       (XtActionProc) _XmMenuBtnUp},
    {"PopupBtnDown",        (XtActionProc) _XmMenuBtnDown},
    {"PopupBtnUp",          (XtActionProc) _XmMenuBtnUp},
    {"MenuBarBtnDown",      (XtActionProc) _XmMenuBtnDown},
    {"MenuBarBtnUp",        (XtActionProc) _XmMenuBtnUp},
    {"WorkAreaBtnDown",     (XtActionProc) _XmGadgetArm},
    {"WorkAreaBtnUp",       (XtActionProc) _XmGadgetActivate},

    /* One step from removal in 1.2 */
    {"_MenuGetGadget",       (XtActionProc) _XmGetGadget},

    {"FocusOut",            (XtActionProc) _XmMenuFocusOut},
    {"FocusIn",             (XtActionProc) _XmMenuFocusIn},
    {"Unmap",               (XtActionProc) _XmMenuUnmap},
    {"Noop",                (XtActionProc) Noop},
    {"MenuTraverseLeft",    (XtActionProc) _XmMenuTraverseLeft},
    {"MenuTraverseRight",   (XtActionProc) _XmMenuTraverseRight},
    {"MenuTraverseUp",      (XtActionProc) _XmMenuTraverseUp},
    {"MenuTraverseDown",    (XtActionProc) _XmMenuTraverseDown},
    {"MenuEscape",	    (XtActionProc) _XmMenuEscape},

    {"MenuFocusIn",         (XtActionProc) _XmRC_FocusIn},
    {"MenuFocusOut",        (XtActionProc) _XmRC_FocusOut},
    {"MenuUnmap",           (XtActionProc) _XmRC_Unmap},
    {"MenuEnter",           (XtActionProc) _XmRC_Enter},

    {"MenuGadgetReturn",         (XtActionProc) _XmGadgetSelect},
    {"MenuGadgetEscape",         (XtActionProc) _XmRC_GadgetEscape},
    {"MenuGadgetTraverseLeft",   (XtActionProc) _XmRC_GadgetTraverseLeft},
    {"MenuGadgetTraverseRight",  (XtActionProc) _XmRC_GadgetTraverseRight},
    {"MenuGadgetTraverseUp",     (XtActionProc) _XmRC_GadgetTraverseUp},
    {"MenuGadgetTraverseDown",   (XtActionProc) _XmRC_GadgetTraverseDown},

    { NULL,                     NULL}
};


/*
 * define the resourse stuff for a rowcolumn widget
 */

static int        resource_0_int      = 0;
static Widget     resource_0_widget   = 0;
static Boolean    resource_False_boolean  = 0;
static Boolean    resource_True_boolean  = 1;
static short      resource_1_short    = 1;
static Dimension  resource_min_width   = 16;  /* 'cuz it's the size of */
static Dimension  resource_min_height  = 16;  /* a hot spot... */
static unsigned char resource_type        = XmWORK_AREA;
static unsigned char resource_alignment   = XmALIGNMENT_BEGINNING;
static unsigned char resource_packing     = XmNO_PACKING;
static unsigned char resource_orient      = XmNO_ORIENTATION;

static XtResource resources[]  =  
{
    {   XmNresizeWidth,
        XmCResizeWidth,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset(XmRowColumnWidget, row_column.resize_width),
        XmRImmediate,
        (caddr_t) TRUE
    },
    {   XmNresizeHeight,
        XmCResizeHeight,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset(XmRowColumnWidget, row_column.resize_height),
        XmRImmediate,
        (caddr_t) TRUE
    },
    {   XmNwhichButton,
        XmCWhichButton,
        XmRWhichButton,
        sizeof(unsigned int),
        XtOffset(XmRowColumnWidget, row_column.postButton),
        XmRImmediate,
        (caddr_t) -1,
    },
    {   XmNmenuPost,
        XmCMenuPost,
        XmRString,
        sizeof(String),
        XtOffset(XmRowColumnWidget, row_column.menuPost),
        XmRString,
        NULL,
    },
    {   XmNadjustLast,
        XmCAdjustLast,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset(XmRowColumnWidget, row_column.adjust_last),
        XmRImmediate,
        (caddr_t) TRUE,
    },
    {   XmNmarginWidth, 
        XmCMarginWidth, 
        XmRHorizontalDimension, 
        sizeof (Dimension),
        XtOffset (XmRowColumnWidget, row_column.margin_width), 
        XmRImmediate, 
        (caddr_t) XmINVALID_DIMENSION
    },
    {   XmNmarginHeight, 
        XmCMarginHeight, 
        XmRVerticalDimension, 
        sizeof (Dimension),
        XtOffset (XmRowColumnWidget, row_column.margin_height), 
        XmRImmediate, 
        (caddr_t) XmINVALID_DIMENSION
    },
    {   XmNentryCallback,
        XmCCallback, 
        XmRCallback, 
        sizeof (caddr_t),
        XtOffset (XmRowColumnWidget, row_column.entry_callback), 
        XmRCallback, 
        NULL
    },
    {   XmNmapCallback, 
        XmCCallback, 
        XmRCallback,
        sizeof (caddr_t),
        XtOffset (XmRowColumnWidget, row_column.map_callback), 
        XmRCallback, 
        NULL
    },
    {   XmNunmapCallback, 
        XmCCallback, 
        XmRCallback, 
        sizeof (caddr_t),
        XtOffset (XmRowColumnWidget, row_column.unmap_callback), 
        XmRCallback, 
        NULL
    },
    {   XmNorientation, 
        XmCOrientation, 
        XmROrientation, 
        sizeof(unsigned char),
        XtOffset (XmRowColumnWidget, row_column.orientation), 
        XmROrientation, 
        (caddr_t) &resource_orient
    },
    {   XmNspacing, 
        XmCSpacing, 
        XmRHorizontalDimension, 
        sizeof(Dimension),
        XtOffset (XmRowColumnWidget, row_column.spacing), 
        XmRImmediate, 
        (caddr_t) XmINVALID_DIMENSION
    },
    {   XmNentryBorder,         /* border width of all the */
        XmCEntryBorder,         /* entries, always uniform */
        XmRHorizontalDimension, 
        sizeof(Dimension),
        XtOffset (XmRowColumnWidget, row_column.entry_border), 
        XmRImmediate, 
	(caddr_t) 0
    },
    {   XmNisAligned,           /* T/F, do all entrys have */
        XmCIsAligned,           /* same alignment */
        XmRBoolean, 
        sizeof(Boolean),
        XtOffset(XmRowColumnWidget, row_column.do_alignment),
        XmRBoolean, 
        (caddr_t) &resource_True_boolean
    },
    {   XmNentryAlignment,          /* how entries are to be */
        XmCAlignment,               /* aligned */
        XmRAlignment, 
        sizeof(unsigned char),
        XtOffset(XmRowColumnWidget, row_column.entry_alignment),
        XmRAlignment, 
        (caddr_t) &resource_alignment
    },
    {   XmNadjustMargin,            /* should all entries have */
        XmCAdjustMargin,            /* the same label margins */
        XmRBoolean, 
        sizeof(Boolean),
        XtOffset(XmRowColumnWidget, row_column.adjust_margin),
        XmRBoolean, 
        (caddr_t) &resource_True_boolean
    },
    {   XmNpacking,         /* how to pack menu entries */
        XmCPacking,         /* Tight, Column, None */
        XmRPacking,
        sizeof (unsigned char),
        XtOffset(XmRowColumnWidget, row_column.packing),
        XmRPacking,
        (caddr_t) &resource_packing
    },
    {   XmNnumColumns,          /* if packing columnar then */
        XmCNumColumns,          /* this is how many */
        XmRShort,
        sizeof (short),
        XtOffset(XmRowColumnWidget, row_column.num_columns),
        XmRShort, 
        (caddr_t) &resource_1_short
    },
    {   XmNradioBehavior,           /* should the menu enforce */
        XmCRadioBehavior,           /* toggle button exclusivity, */
        XmRBoolean,             /* ie, radio buttons */
        sizeof (Boolean),
        XtOffset(XmRowColumnWidget, row_column.radio),
        XmRBoolean, 
        (caddr_t) &resource_False_boolean
    },
    {   XmNradioAlwaysOne,          /* should there always be one */
        XmCRadioAlwaysOne,          /* radio button on. */
        XmRBoolean,
        sizeof (Boolean),
        XtOffset(XmRowColumnWidget, row_column.radio_one),
        XmRBoolean, 
        (caddr_t) &resource_True_boolean
    },
    {   XmNisHomogeneous,           /* should we enforce the */
        XmCIsHomogeneous,           /* rule that only one type of */
        XmRBoolean,             /* entry is allow in the menu */
        sizeof (Boolean),
        XtOffset(XmRowColumnWidget, row_column.homogeneous),
        XmRBoolean, 
        (caddr_t) &resource_False_boolean
    },
    {   XmNentryClass,              /* if enforcing homogeneous */
        XmCEntryClass,              /* menu, this tells the class */
        XmRInt,
        sizeof (WidgetClass),
        XtOffset(XmRowColumnWidget, row_column.entry_class),
        XmRWidgetClass, 
        (caddr_t) NULL
    },
    {   XmNrowColumnType,       /* warning - non-standard resource */
        XmCRowColumnType, 
        XmRRowColumnType, 
        sizeof(unsigned char),
        XtOffset (XmRowColumnWidget, row_column.type), 
        XmRRowColumnType, 
        (caddr_t) &resource_type
    },
    {   XmNmenuHelpWidget,          /* which widget is the help */
        XmCMenuWidget,              /* widget */
        XmRMenuWidget, 
        sizeof (Widget),
        XtOffset (XmRowColumnWidget, row_column.help_pushbutton), 
        XmRMenuWidget, 
        (caddr_t) &resource_0_widget
    },
    {   XmNlabelString,               /* option menus have a label */
        XmCString, 
        XmRXmString, 
        sizeof(_XmString),
        XtOffset (XmRowColumnWidget, row_column.option_label), 
        XmRImmediate, 
        (caddr_t)NULL
    },
    {   XmNsubMenuId,               /* option menus have built-in */
        XmCMenuWidget,              /* submenu */
        XmRMenuWidget, 
        sizeof (Widget),
        XtOffset (XmRowColumnWidget, row_column.option_submenu), 
        XmRMenuWidget, 
        (caddr_t) &resource_0_widget
    },
    {   XmNmenuHistory,         /* pretend a subwidget fired */
        XmCMenuWidget,              /* off, used to pre-load the */
        XmRMenuWidget,              /* option menu and popup menu */
        sizeof (Widget),            /* mouse/muscle memory */
        XtOffset (XmRowColumnWidget, row_column.memory_subwidget), 
        XmRMenuWidget, 
        (caddr_t) &resource_0_widget
    },
    {   XmNpopupEnabled,            /* are accelerator enabled */
        XmCPopupEnabled,            /* in the popup menu? */
        XmRBoolean,
        sizeof (Boolean),
        XtOffset(XmRowColumnWidget, row_column.popup_enabled),
        XmRBoolean, 
        (caddr_t) &resource_True_boolean
    },
    {   XmNmenuAccelerator,         /* popup menu accelerator */
        XmCAccelerators,
        XmRString,
        sizeof (char *),
        XtOffset(XmRowColumnWidget, row_column.menu_accelerator),
        XmRString, 
        (caddr_t) ""
    },
    {   XmNmnemonic,                /* option menu mnemonic */
        XmCMnemonic,
        XmRKeySym,
        sizeof (KeySym),
        XtOffset(XmRowColumnWidget, row_column.mnemonic),
        XmRImmediate, 
        (caddr_t) NULL
    },
    {
	XmNmnemonicCharSet,
	XmCMnemonicCharSet,
	XmRString,
	sizeof(XmStringCharSet),
	XtOffset(XmRowColumnWidget,row_column.mnemonicCharSet),
	XmRImmediate,
	(caddr_t) XmSTRING_DEFAULT_CHARSET
    },
    {
        XmNshadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension, 
	sizeof (Dimension),
	XtOffset (XmRowColumnWidget, manager.shadow_thickness),
	XmRImmediate,
	(caddr_t) XmINVALID_DIMENSION
    },
    {
	XmNpostFromList,
	XmCPostFromList,
	XmRWidgetList,
	sizeof (Widget *),
	XtOffset (XmRowColumnWidget, row_column.postFromList),
	XmRWidgetList,
	(caddr_t) NULL,
    },
    {
        XmNpostFromCount,
	XmCPostFromCount,
	XmRInt,
	sizeof (int),
	XtOffset (XmRowColumnWidget, row_column.postFromCount),
	XmRImmediate,
	(caddr_t) -1
    },
    {
	XmNnavigationType, 
	XmCNavigationType, 
	XmRNavigationType, 
	sizeof (unsigned char),
	XtOffset(XmManagerWidget, manager.navigation_type),
	XmRImmediate, 
	(caddr_t) XmTAB_GROUP,
    },
};


static XmSyntheticResource get_resources[] = 
{
    {   XmNspacing,
        sizeof(Dimension),
        XtOffset(XmRowColumnWidget,row_column.spacing),
        _XmFromHorizontalPixels,
        _XmToHorizontalPixels,
    },
    {   XmNmarginHeight,
        sizeof(Dimension),
        XtOffset (XmRowColumnWidget, row_column.margin_height),
        _XmFromVerticalPixels,
        _XmToVerticalPixels,
    },
    {   XmNmarginWidth,
        sizeof(Dimension),
        XtOffset (XmRowColumnWidget, row_column.margin_width), 
        _XmFromHorizontalPixels,
        _XmToHorizontalPixels,
    },
    {
        XmNentryBorder,
        sizeof(Dimension),
        XtOffset (XmRowColumnWidget, row_column.entry_border), 
        _XmFromHorizontalPixels,
        _XmToHorizontalPixels,
    },
};





/*
 * static initialization of the row column widget class record, must do
 * each field
 */

externaldef(xmrowcolumnclassrec) XmRowColumnClassRec xmRowColumnClassRec = 
{
    {                   /* core class record */
        (WidgetClass)&xmManagerClassRec, /* superclass ptr */
        "XmRowColumn",                 /* class_name */
        sizeof (XmRowColumnWidgetRec), /* size of widget instance */
        ClassInitialize,               /* class init proc */
        ClassPartInitialize,           /* class part init */
        FALSE,                         /* class is not init'ed */
        Initialize,                    /* widget init proc*/
        NULL,                          /* init_hook proc */
        Realize,                       /* widget realize proc */
        action_table,                  /* class action table */
        XtNumber (action_table),
        resources,                     /* this class's resource list */
        XtNumber (resources),          /*  "     " resource_count */
        NULLQUARK,                     /* xrm_class            */
        TRUE,                          /* don't compress motion */
        XtExposeCompressMaximal,       /* do compress exposure */
        FALSE,                         /* don't compress enter-leave */
        FALSE,                         /* no VisibilityNotify */
        Destroy,                       /* class destroy proc */
        Resize,                        /* class resize proc */
        Redisplay,                        /* class expose proc */
        SetValues,                     /* class set_value proc */
        NULL,                          /* set_value_hook proc */
        XtInheritSetValuesAlmost,      /* set_value_almost proc */
        NULL,                          /* get_values_hook */
        NULL,                          /* class accept focus proc */
        XtVersion,                     /* current version */
        NULL,                          /* callback offset list */
        NULL,                          /* translation table */
        QueryGeometry,                /* query geo proc */
        NULL,                          /* display accelerator */
        NULL,                          /* extension */
    },
    {                  /* composite class record */
        GeometryManager,        /* childrens geo mgr proc */
        ManagedSetChanged,     /* set changed proc */
        (XtArgsProc)AddKid,     /* add a child */
        RemoveChild,            /* remove a child */
        NULL,                    /* extension */
    },
    {                  /* constraint class record */
        NULL,                    /* constraint resources */
        0,                       /* constraint resource_count */
        sizeof(XmRowColumnConstraintRec),  /* constraint_size */
        ConstraintInitialize,    /* initialize */
        ConstraintDestroy,       /* destroy */
        NULL,                    /* set_values */
        NULL,                    /* extension */
    },
    {                  /* manager class record */
        XtInheritTranslations,   /* translations */
        get_resources,           /* syn_resources */
        XtNumber(get_resources), /* num_syn_resources */
        NULL,                    /* syn_constraint_resources */
        0,                       /* num_syn_constraint_resources */
        XmInheritParentProcess,  /* parent_process         */
        NULL,                    /* extension */
    },
    {                  /* row column class record */
        MenuProcedureEntry,	 /* proc to interface with menu widgets */
        ArmAndActivate,          /* proc to arm&activate menu */
        _XmMenuTraversalHandler, /* traversal handler */
        NULL,                    /* extension */
    }
};






/*
 * now make a public symbol that points to this class record
 */

externaldef(xmrowcolumnwidgetclass) WidgetClass xmRowColumnWidgetClass = 
   (WidgetClass) &xmRowColumnClassRec;

static Widget lastSelectToplevel = NULL;

/* REQUIRES FIX FOR MULTI SCREEN!!! */
XmButtonEventStatusRec _XmButtonEventStatus = {-1,FALSE,TRUE, NULL};

/*ARGSUSED*/
static XtTimerCallbackProc PostTimeOut (popup, id)
XmRowColumnWidget popup;
XtIntervalId *id;
{  
  if (_XmButtonEventStatus.waiting_to_be_managed)
     MenuPopDown (popup, &_XmButtonEventStatus.event);
  _XmButtonEventStatus.waiting_to_be_managed = FALSE;
}


/*
 * ButtonEventHandler is inserted at the head of the event handlers.  We must
 * pre-verify the events that popup a menupane.  When the application manages
 * the popup menupane, MenuShell's managed_set_changed(), checks the
 * verification.
 */
/* ARGSUSED */
static void ButtonEventHandler(w, popup, event)
Widget w;
XmRowColumnWidget popup;
XEvent * event;
{
   XButtonEvent *xbutton_event = (XButtonEvent *)event;

   _XmButtonEventStatus.time = xbutton_event->time;
   _XmButtonEventStatus.verified = _XmMatchBtnEvent( event,
      RC_PostEventType(popup), RC_PostButton(popup), RC_PostModifiers(popup));

   if (xbutton_event->type == ButtonPress) 
   {
      _XmButtonEventStatus.waiting_to_be_managed = TRUE;
      XtAppAddTimeOut(XtWidgetToApplicationContext(popup), 
	  (unsigned long) POST_TIME_OUT, 
	  (XtTimerCallbackProc) PostTimeOut, (caddr_t) popup);
      _XmButtonEventStatus.event = *xbutton_event;
   }
}

static void AddHandlersToPostFromWidget (popup, widget)
Widget popup;
Widget widget;
{
   Cursor cursor;
   
   cursor = XmGetMenuCursor(XtDisplay(popup));

   XtInsertEventHandler(widget, ButtonPressMask|ButtonReleaseMask,
                     False, ButtonEventHandler, popup, XtListHead);

   XtAddEventHandler(widget, KeyPressMask|KeyReleaseMask,
		     False, KeyboardInputHandler, popup);

   /*
    * Add an event handler on the associated widget for ButtonRelease
    * events.  This is so that a quick press/release pair does not get
    * lost if the release is processed before our pointer grab is made.
    * This will guarantee that the associated widget gets the button
    * release event; it would be discarded if the widget was not selecting
    * for button release events.
    */
   XtAddEventHandler(widget, ButtonReleaseMask,
		     False, DoNothing, NULL);

   /* 
    * Must add a passive grab, so that owner_events is set to True
    * when the button grab is activated; this is so that enter/leave
    * events get dispatched by the server to the client.
    */
   XtGrabButton (widget, RC_PostButton(popup), RC_PostModifiers(popup), 
      TRUE, ButtonReleaseMask, GrabModeSync, GrabModeSync, None, cursor);
}

static void RemoveHandlersFromPostFromWidget (popup, widget)
Widget popup;
Widget widget;
{
   XtRemoveEventHandler(widget,	ButtonPressMask|ButtonReleaseMask,
			False, ButtonEventHandler, popup);

   XtRemoveEventHandler(widget,	KeyPressMask|KeyReleaseMask,
			False, KeyboardInputHandler, popup);

   XtRemoveEventHandler(widget, ButtonReleaseMask,
			False, DoNothing, NULL);

   /* Remove our passive grab */
   XtUngrabButton (widget, RC_PostButton(popup), AnyModifier); 
}


/*
 * Add the Popup Menu Event Handlers needed for posting and accelerators
 */
static void AddPopupEventHandlers (pane)
XmRowColumnWidget pane;
{
   int i;
   
   /* to myself for gadgets */
   XtAddEventHandler(pane, KeyPressMask|KeyReleaseMask,
		     False, KeyboardInputHandler, pane);

   /* Add to Our shell parent */
   XtAddEventHandler(XtParent(pane), KeyPressMask|KeyReleaseMask,
		     False, KeyboardInputHandler, pane);

   /* add to all of the widgets in the postFromList*/
   for (i=0; i < pane->row_column.postFromCount; i++)
   {
      AddHandlersToPostFromWidget (pane, pane->row_column.postFromList[i]);
   }
}

/*
 * Remove the Popup Menu Event Handlers needed for posting and accelerators
 */
static void RemovePopupEventHandlers (pane)
XmRowColumnWidget pane;
{
   int i;
   
   /* Remove it from us */
   XtRemoveEventHandler(pane, KeyPressMask|KeyReleaseMask,
			False, KeyboardInputHandler, pane);

   /* Remove it from our shell parent */
   XtRemoveEventHandler(XtParent(pane), KeyPressMask|KeyReleaseMask,
			False, KeyboardInputHandler, pane);

   /* Remove it from the postFrom widgets */
   for (i=0; i < pane->row_column.postFromCount; i++)
   {
      RemoveHandlersFromPostFromWidget (pane,
					pane->row_column.postFromList[i]);
   }
}



/*
 * Destroy the widget, and any subwidgets there are
 */

static void Destroy (m)
XmRowColumnWidget m;
{
   Widget topManager;
   int i;
   
   XtRemoveAllCallbacks (m, XmNentryCallback);
   XtRemoveAllCallbacks (m, XmNmapCallback);
   XtRemoveAllCallbacks (m, XmNunmapCallback);

   XtFree(MGR_KeyboardList(m));
   /*
    * If we had added any event handlers for processing accelerators or
    * mnemonics, then we must remove them now.
    */
   if (IsPopup(m))
   {
      if (RC_PopupEnabled(m))
	  RemovePopupEventHandlers (m);
   }
   else if (IsOption(m) || IsBar(m))
   {
      /* Remove it from the associated widget */
      GetTopManager (m, &topManager);
      XtRemoveEventHandler(topManager, KeyPressMask|KeyReleaseMask,
			   False, KeyboardInputHandler, m);

      /* Remove it from us */
      XtRemoveEventHandler(m, KeyPressMask|KeyReleaseMask,
			   False, KeyboardInputHandler, m);

   }

   /*
    * If we're still connected to a cascade button, then we need to break
    * that link, so that the cascade button doesn't attempt to reference
    * us again, and also so that accelerators and mnemonics can be cleared up.
    */
   else
   {
      Arg args[1];

      for (i=0; i < m->row_column.postFromCount; i++)
      {
	 if (! m->row_column.postFromList[i]->core.being_destroyed)
	 {
	    XtSetArg (args[0], XmNsubMenuId, NULL);
	    XtSetValues (m->row_column.postFromList[i], args, 1);
	 }
      }
   }

   /* free allocated postFromList for popups and pulldowns */
   if (IsPopup(m) || IsPulldown(m))
       XtFree (m->row_column.postFromList);

   if ((IsPopup(m) && RC_PopupEnabled(m))  ||
       (IsBar(m) && RC_MenuAccelerator(m)) ||
       (IsOption(m) && RC_Mnemonic(m)))
   {
      Cardinal num_children;

      /*
       * By the time we reach here, our children are destroyed, but
       * the children's list is bogus; so we need to temporarily zero
       * out our num_children field, so DoProcessMenuTree() will not
       * attempt to process our children.
       */
      num_children = m->composite.num_children;
      m->composite.num_children = 0;
      DoProcessMenuTree(m, XmDELETE);
      m->composite.num_children = num_children;
   }
}


/*
 * Destroy any keyboard grabs/entries for the child
 */
static void ConstraintDestroy (w)
Widget w;
{
   if (!XtIsRectObj(w)) return;

   DoProcessMenuTree(w, XmDELETE);
}


/*
 * do all the stuff needed to make a subwidget of a menu work correctly
 */
static int fix_widget (m, w)
    XmRowColumnWidget m;
    Widget w;
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



/*
 * Add a child to this row column widget
 */
static int AddKid (w)
Widget w;
{
    XmRowColumnWidget m = (XmRowColumnWidget) XtParent(w);

    if (!IsWorkArea(m) /* it's a menu */ &&
        (XtClass(w) != xmLabelGadgetClass) && (XtIsRectObj(w)) &&
        !XmIsPushButtonGadget(w) &&
        !XmIsCascadeButtonGadget(w) &&
        !XmIsToggleButtonGadget(w) &&
        !XmIsSeparatorGadget(w) &&
        (XtClass(w) != xmLabelWidgetClass) &&
        !XmIsPushButton(w) &&
        !XmIsCascadeButton(w) &&
        !XmIsToggleButton(w) &&
        !XmIsSeparator(w))
        _XmWarning(m,WrongMenuChildMsg);

    /*
     * if the rowcolumn is homogeneous, make sure that class matches
     * the entry class.  Two exceptions are made:  1) if the entry class is
     * CascadeButton or CascadeButtonGadget, either of those classes are
     * allowed. 2) if the entry class is ToggleButton or ToggleButtonGadget,
     * either of those classes are allowed.
     */
    if (XtIsRectObj(w) && RC_IsHomogeneous(m) && 
	(RC_EntryClass(m) != XtClass(w)))
    {
       if (! ((RC_EntryClass(m) == xmCascadeButtonWidgetClass &&
	       XmIsCascadeButtonGadget(w)) ||
	      (RC_EntryClass(m) == xmCascadeButtonGadgetClass &&
	       XmIsCascadeButton(w)) ||
	      (RC_EntryClass(m) == xmToggleButtonGadgetClass &&
	       XmIsToggleButton(w)) ||
	      (RC_EntryClass(m) == xmToggleButtonWidgetClass &&
	       XmIsToggleButtonGadget(w))))
       {
	   
	  _XmWarning (m, WrongChildMsg);
       }
    }
       
    /*
     * use composite class insert proc to do all the dirty work
     */
    (*((XmManagerWidgetClass)xmManagerWidgetClass)->composite_class.
                 insert_child) (w);

    /*
     * now change the subwidget so that it acts the way we want it to
     */
    fix_widget (m, w);

    return (TRUE);
}




/*
 * delete a single widget from a parent widget
 */
static void RemoveChild (child)
Widget    child;
{
   XmRowColumnWidget m = (XmRowColumnWidget) XtParent(child);

    if (child == RC_HelpPb (m)) 
       RC_HelpPb (m) = NULL;

    else if (child == RC_MemWidget(m))
    {
       RC_MemWidget(m) = NULL;
    }

    /*
     * If this child is in a top level menupane, then we want to remove
     * the event handler we added for catching keyboard input.
     */
    if (XtIsWidget(child) &&
	((IsPopup(m) || IsBar(m) || IsPulldown(m)) && 
	  XmIsLabel(child) && (child->core.widget_class != xmLabelWidgetClass)))
    {
	   XtRemoveEventHandler(child, KeyPressMask|KeyReleaseMask, False,
				KeyboardInputHandler, m);
    }

    /*
     * use composite class insert proc to do all the dirty work
     */
    (*((CompositeWidgetClass)compositeWidgetClass)->composite_class.
                delete_child) (child);
}


/*
 * The set of our managed children changed, so maybe change the size of the
 * row column widget to fit them; there is no instigator of this change, and 
 * ignore any dimensional misfit of the row column widget and the entries, 
 * which is a result of our geometry mgr being nasty.  Get it laid out.
 */
static void ManagedSetChanged (m)
XmRowColumnWidget m;
{
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
	 any_changed = TRUE;
      }
      
      WasManaged(*q) = IsManaged(*q);
   }

   if (!any_changed)
   {
      /* Must have been a popup child -- we don't really care */
      return;
   }

   DoMarginAdjustment (m);
   
   /*
    * find out what size we need to be with the current set of kids
    */
   PreferredSize (m, &w, &h);

   /*
    * now decide if the menu needs to change size
    */
   if ((w != XtWidth (m)) || (h != XtHeight (m)))
   {
      XtWidgetGeometry menu_desired, menu_allowed;
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

      XtMakeGeometryRequest(m,&menu_desired,&menu_allowed);
   }

   /*
    * if we get to here the row column widget has been changed and his 
    * window has been resized, so effectively we need to do a Resize.
    */

   AdaptToSize (m, NULL, NULL);
   
   /*	Clear shadow if necessary. */
   
   if (m->row_column.old_shadow_thickness)
       _XmClearShadowType (m, m->row_column.old_width, 
			   m->row_column.old_height, 
			   m->row_column.old_shadow_thickness, 0);
   
   m->row_column.old_width = m->core.width;
   m->row_column.old_height = m->core.height;
   m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

   _XmNavigChangeManaged(m);
}                       


/*
 * make the row column widget appear
 */
static void Realize (m, window_mask, window_attributes)
XmRowColumnWidget m;
Mask *window_mask;
XSetWindowAttributes *window_attributes;
{
   if (IsOption(m))
   {
      XmRowColumnWidget sm = (XmRowColumnWidget) RC_OptionSubMenu(m);
      int i;
      Dimension w=0, h=0;

      if (!IsNull(sm))
      {
	 /* if there is no memory widget, set it up */
	 if (!RC_MemWidget(m))
	 {
	    if (!RC_MemWidget(sm))
	    {
	       /* submenu does not have a memory widget */
	       for(i = 0;i < sm->composite.num_children; i++)
	       {
		  if (XtIsManaged(sm->composite.children[i]))
		  {
		     /* choose first managed child as memory widget */
		     RC_MemWidget(m) = sm->composite.children[i];
		     RC_MemWidget(sm) = RC_MemWidget(m);
		     break;
		  }
	       }
	    }
	    else
		/* use submenu's memory widget */
		RC_MemWidget(m) = RC_MemWidget(sm);

	    /* update option menu label */
	    if ( RC_MemWidget(m) )	/* in case it doesn't exist! */
	    {
	       for (i = 0; i < m->composite.num_children; i++)
	       {
		  if (XmIsCascadeButtonGadget(m->composite.children[i]))
		  {
		     UpdateOptionMenuCBG (m->composite.children[i],
					  RC_MemWidget(m));
		     break;
		  }
	       }
	    }
	 }
	 
	 /* find out what size we need to be */
	 PreferredSize (m, &w, &h);

	 /* now decide if the menu needs to change size */
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

	    XtMakeGeometryRequest(m, &menu_desired,&menu_allowed);
	 }

	 AdaptToSize (m, NULL, NULL);
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
      XtAddEventHandler(m, OwnerGrabButtonMask, False, Noop, NULL);
   }

   /*
    * Don't propagate events for row column widgets
    * and set bit gravity to NW
    */
   (*window_mask) |= CWDontPropagate | CWBitGravity;
   window_attributes->bit_gravity = NorthWestGravity;

   window_attributes->do_not_propagate_mask = ButtonPressMask| 
       ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask;

   XtCreateWindow ( m, InputOutput, CopyFromParent, *window_mask, 
		   window_attributes);

   /*
    * Keep menus which are a child of shell widgets mapped at all times.
    * Mapping is now done by the menu shell widget.
    */
   
   if (XmIsMenuShell (XtParent(m)))
       m->core.mapped_when_managed = FALSE;
}


/*
 * utilities for setvalue procs
 */
static Boolean do_entry_stuff (old, new)
XmRowColumnWidget old, new;
{
    XtWidgetGeometry desired;

    Boolean need_expose = FALSE;

    if ((RC_EntryBorder (old) != RC_EntryBorder (new)) &&
        (RC_EntryBorder(new)))
    {
        Widget *p;
        int i;

        desired.request_mode = CWBorderWidth;
        desired.border_width = RC_EntryBorder (new);

        ForAllChildren (new, i, p)
        {
            _XmConfigureObject(*p,(*p)->core.x,(*p)->core.y,
                (*p)->core.width, (*p)->core.height,
                desired.border_width);
        }

        need_expose = TRUE;
    }

    if ((RC_EntryAlignment (old) != RC_EntryAlignment (new)) && 
        (IsAligned (new)) &&
        (!IsOption(new)))
    {
        Widget *p;
        Arg al[2];
        int i;

        XtSetArg (al[0], XmNalignment, RC_EntryAlignment(new));

        ForAllChildren (new, i, p)
        {
            XtSetValues (*p, al, 1);
        }

        need_expose  = TRUE;
    }
    
    return (need_expose);
}

static void do_size (old, new)
XmRowColumnWidget old, new;
{
    Widget *p;
    int i;
    int orient = RC_Orientation (old) != RC_Orientation (new);
    Dimension w;
    Dimension h;

    if (orient)                 /* flip all the separator */
    {                       /* widgets too */
        Arg al[2];
        int ac = 0;

        XtSetArg (al[ac], XmNorientation, 
            (IsVertical (new) ? XmHORIZONTAL : XmVERTICAL));

        ForAllChildren (new, i, p)
        {
            if (XmIsSeparator(*p) || XmIsSeparatorGadget(*p))
            XtSetValues (*p, al, 1);
        }
    }

    if ((!XtWidth(new))  || (XtWidth (new) != XtWidth(old))   ||
        (!XtHeight(new)) || (XtHeight (new) != XtHeight(old)) ||
        (orient          || 
        ((IsPopup(new) || IsPulldown(new) || IsBar(new)) && 
            (MGR_ShadowThickness(new) != MGR_ShadowThickness(old))) ||
        (RC_EntryBorder (old) != RC_EntryBorder (new)) || 
        (RC_MarginW     (old) != RC_MarginW     (new)) || 
        (RC_MarginH     (old) != RC_MarginH     (new)) || 
        (RC_Spacing     (old) != RC_Spacing     (new)) || 
        (RC_Packing     (old) != RC_Packing     (new)) || 
        (RC_NCol        (old) != RC_NCol        (new)) || 
        (RC_AdjLast     (old) != RC_AdjLast     (new)) || 
        (RC_AdjMargin   (old) != RC_AdjMargin   (new)) || 
        (RC_HelpPb      (old) != RC_HelpPb      (new))))
    {
        if (!RC_ResizeWidth(new) && RC_ResizeHeight(new))
        {
            w = new->core.width;
            h = 0;
        }
        else if (RC_ResizeWidth(new) && !RC_ResizeHeight(new))
        {
            w = 0;
            h = new->core.height;
        }
        else if (RC_ResizeWidth(new) && RC_ResizeHeight(new))
        {
            w = 0;
            h = 0;
        }
        else
        {
            AdaptToSize(new,NULL,NULL);
            return;
        }

        PreferredSize (new, &w, &h);

        AdaptToSize(new,NULL,NULL);

        XtWidth(new) = w;
        XtHeight(new) = h;
    }
}


static Boolean set_values_non_popup (old, new)
XmRowColumnWidget   old;            /* the old state widget */
XmRowColumnWidget   new;            /* the new, real widget */
{
    Widget child;
    Arg args[4];
    int n;
    Boolean need_expose = FALSE;

    if (IsBar(new))
        new->manager.traversal_on = old->manager.traversal_on;

    /* fdt : should this only be done for a menubar?? */
    need_expose |= RC_HelpPb (old) != RC_HelpPb (new);

    /*
     * If we are an option menu, then we must check to see if our mnemonic
     * has changed.  If we're a menubar, then see if our accelerator has
     * changed.
     */
    if (IsOption(new))
    {
       if (RC_OptionSubMenu(new) != RC_OptionSubMenu(old)) {
          XtSetArg(args[0], XmNsubMenuId, RC_OptionSubMenu(new));
          if (child = XmOptionButtonGadget(new))
             XtSetValues(child, args, 1);

          if (child = find_first_managed_child(RC_OptionSubMenu(new), FIRST_BUTTON)) {
             RC_MemWidget (new) = child;
          }
       }
       if (RC_MemWidget (old) != RC_MemWidget (new))
          SetMenuHistory (new, RC_MemWidget (new));

       n = 0;
       if (RC_OptionLabel(new) != RC_OptionLabel(old)) {
          XtSetArg(args[n], XmNlabelString, RC_OptionLabel(new)); n++;
          XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
       }
       if ((RC_Mnemonic(new) != RC_Mnemonic(old)) ||
           (RC_MnemonicCharSet(new) != RC_MnemonicCharSet(old)))
       {
          XtSetArg(args[n], XmNmnemonic, RC_Mnemonic(new)); n++;
          XtSetArg(args[n], XmNmnemonicCharSet, RC_MnemonicCharSet(new)); n++;
       }
       if (n && (child = XmOptionLabelGadget(new)))
          XtSetValues(child, args, n);
       DoProcessMenuTree(new, XmREPLACE);
    }
    else if (IsBar(new) && (RC_MenuAccelerator(new) != RC_MenuAccelerator(old)))
    {
        if (RC_MenuAccelerator(new))
        {
            RC_MenuAccelerator(new) = (String)strcpy(XtMalloc( XmStrlen(
                RC_MenuAccelerator(new)) + 1), RC_MenuAccelerator(new));
        }
        DoProcessMenuTree(new, XmREPLACE);
        if (RC_MenuAccelerator(old))
           XtFree(RC_MenuAccelerator(old));
    }

   /*
    * Moved here in case Option Menu geometry changed
    */
    need_expose |= do_entry_stuff (old, new);
    do_size (old, new);

    return (need_expose);
}



static Boolean set_values_popup (old, new)
XmRowColumnWidget   old;            /* the old state widget */
XmRowColumnWidget   new;            /* the new, real widget */
{
    int need_expose  = FALSE;

    new->manager.traversal_on = old->manager.traversal_on;

    need_expose |= do_entry_stuff (old, new);
    do_size (old, new);

    if ((XtX (old) != XtX (new)) ||     /* signal the shell that it */
        (XtY (old) != XtY (new)))       /* had better move itself */
    {                                   /* to the menu's location */
        RC_SetWidgetMoved (new, TRUE);      /* and that it has to move */
        RC_SetWindowMoved (new, TRUE);      /* the menu's window back */
    }

    /*
     * If we are a popup menu, then we need to check the 
     * state of the popupEnabled resource; we may need to add or remove the 
     * event handler we use to catch accelerators and mnemonics.
     */
    if (IsPopup(new))
    {
       if (RC_PopupEnabled(new) != RC_PopupEnabled(old))
       {
          if (RC_PopupEnabled(new))
          {
	     AddPopupEventHandlers(new);
             DoProcessMenuTree(new, XmADD);
          }
          else
          {
	     RemovePopupEventHandlers (new);
             DoProcessMenuTree(new, XmDELETE);
          }
       }

       /* See if our accelerator has changed */
       if (RC_PopupEnabled(new) &&
           (RC_MenuAccelerator(new) != RC_MenuAccelerator(old)))
       {
          if (RC_MenuAccelerator(new))
          {
             RC_MenuAccelerator(new) = (String)strcpy(XtMalloc( XmStrlen(
                 RC_MenuAccelerator(new)) + 1), RC_MenuAccelerator(new));
          }
          DoProcessMenuTree(new, XmREPLACE);
          if (RC_MenuAccelerator(old))
             XtFree(RC_MenuAccelerator(old));
       }
    }

    return (need_expose);
}



/* 
 * Empty event handler, added to the associated widget for a popup menu.
 * Forces the widget to select for button release events.
 */
/* ARGSUSED */
static void DoNothing (w, client_data, event)
Widget w;
caddr_t client_data;
XEvent * event;
{
}

static void set_values_passive_grab (old, new)
XmRowColumnWidget   old;            /* the old state widget */
XmRowColumnWidget   new;            /* the new, real widget */
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
            if (XtIsRealized(old->row_column.postFromList[i]))
            {
               XtUngrabButton (old->row_column.postFromList[i], 
		  RC_PostButton(old), RC_PostModifiers(old));
            }
         }

         if (RC_PopupEnabled(new))
         {
            cursor = XmGetMenuCursor(XtDisplay(new));

            /* add to all of the widgets in the postFromList*/
            for (i=0; i < new->row_column.postFromCount; i++)
            {
               /*
                * Must add a passive grab, so that owner_events is
                * set to True when the button grab is activated
                * this is so that enter/leave
                * events get dispatched by the server to the client.
                */
	       XtGrabButton (new->row_column.postFromList[i], 
		  RC_PostButton(new), RC_PostModifiers(new), 
		  TRUE, ButtonReleaseMask, GrabModeSync, GrabModeSync, 
		  None, cursor);
            }
         }
      }
   }
}


static Boolean SetValues (old, req, new)
XmRowColumnWidget    old;           /* the real widget */
XmRowColumnWidget    req;           /* after arglist */
XmRowColumnWidget    new;           /* after superclasses */
{
   int i;
   int need_expose = FALSE;

   if (!XtWidth(req))
   {
      _XmWarning(req,BadWidthSVMsg);
      XtWidth(req) = 16;
   }

   if (!XtHeight(req))
   {
      _XmWarning(req,BadHeightSVMsg);
      XtHeight(req) = 16;
   }

   switch (RC_Orientation(req))
   {
    case XmVERTICAL:
    case XmHORIZONTAL:
      break;
    case XmNO_ORIENTATION:
    default:
      _XmWarning(req,BadOrientationSVMsg);
      RC_Orientation(new) = RC_Orientation(old);
      break;
   }

   switch (RC_Packing(req))
   {
    case XmNO_PACKING:
    case XmPACK_TIGHT:
    case XmPACK_COLUMN:
    case XmPACK_NONE:
      break;
    default:
      _XmWarning(req,BadPackingSVMsg);
      RC_Packing(new) = RC_Packing(old);
      break;
   }

   if (RC_Type(req) != RC_Type(old))
   {
      /* Type CANNOT be changed after initialization */
      _XmWarning(new,BadTypeSVMsg);
      RC_Type(new) = RC_Type(old);
   }

   switch (RC_EntryAlignment(req))
   {
    case XmALIGNMENT_BEGINNING:
    case XmALIGNMENT_CENTER:
    case XmALIGNMENT_END:
      break;
    default:
      _XmWarning(req,BadAlignmentSVMsg);
      RC_EntryAlignment(new) = RC_EntryAlignment(old);
      break;
   }

   if (IsBar(new))
   {
      if (RC_IsHomogeneous(req) != RC_IsHomogeneous(old))
      {
	 /* can't change this for menu bars */
	 _XmWarning(new,BadMenuBarHomogenousSVMsg);
	 RC_IsHomogeneous(new) = TRUE;
      }
      if (RC_EntryClass(req) != RC_EntryClass(old))
      {
	 /* can't change this for menu bars */
	 _XmWarning(new,BadMenuBarEntryClassSVMsg);
	 RC_EntryClass(new) = xmCascadeButtonWidgetClass;
      }
   }

   if (RC_MenuPost(new) != RC_MenuPost(old))
   {
      if (IsPulldown(new))
      {
         /* MenuPost cannot be changed via SetValues for Pulldowns */
         _XmWarning(new,BadPulldownMenuPostSVMsg);
         /* just in case WhichButton was set */
         RC_PostButton(new) = RC_PostButton(old);
      }
      else
      {
         if (_XmMapBtnEvent(RC_MenuPost(new), &RC_PostEventType(new),
               &RC_PostButton(new), &RC_PostModifiers(new)) == FALSE)
         {
            _XmWarning(new,BadMenuPostMsg);
            /* Do Nothing - No change to postButton/Modifiers/EventType */
         }
         else
            set_values_passive_grab(old, new);
      }
   }
   else   /* For backwards compatibility... */
      if (RC_PostButton(new) != RC_PostButton(old))
      {
         if (IsPulldown(new))
         {
            /* WhichButton cannot be changed via SetValues for Pulldowns */
            _XmWarning(new,BadPulldownWhichButtonSVMsg);
            RC_PostButton(new) = RC_PostButton(old);
         }
         else
         {
            RC_PostModifiers(new) = AnyModifier;
            RC_PostEventType(new) = ButtonPress;
            set_values_passive_grab(old, new);
         }
      }


   /*
    * Shadow thickness is forced to zero for all types except
    * pulldown, popup, and menubar
    */
   if (IsPulldown(new) || IsPopup(new) || IsBar(new))
   {
      if (MGR_ShadowThickness(req) != MGR_ShadowThickness(old))
	  need_expose |= TRUE;
   }

   else if (MGR_ShadowThickness(req) != MGR_ShadowThickness(old))
   {
      _XmWarning(new,BadShadowThicknessSVMsg);
      MGR_ShadowThickness(new) = 0;
   }

   if (IsOption(new) &&
       (RC_Orientation(req) != RC_Orientation(old)))
   {
      _XmWarning(new,BadOptionOrientationSVMsg);
      RC_Orientation(new) = XmHORIZONTAL;
   }

   /* postFromList changes, popups and pulldowns only */
   if (IsPopup(new) || IsPulldown(new))
   {
      if ((new->row_column.postFromList != old->row_column.postFromList) ||
	  (new->row_column.postFromCount != old->row_column.postFromCount))
      {
	 /* use temp - postFromCount decremented in RemoveFromPostFromList() */
	 int cnt;
	 if (old->row_column.postFromList)
	 {
	    cnt = old->row_column.postFromCount;
	    for (i=0; i < cnt; i++)
	    {
	       RemoveHandlersFromPostFromWidget(new,
		  old->row_column.postFromList[i]);
	    }
	    XtFree(old->row_column.postFromList);
	 }

         PreparePostFromList(new);
      }
   }
	 
   if (IsBar (new) || IsWorkArea (new) || IsOption (new))
       need_expose |= set_values_non_popup (old, new);
   else
       need_expose |= set_values_popup (old, new);
   
   return (need_expose);
}


static char *GetRealKey(rc, str)
XmRowColumnWidget rc;
char *str;
{
   KeySym   keysym;
   Modifiers mods;
   char tmp[128];
   char *ks;
    
   keysym = XStringToKeysym(str);
   if (keysym == NoSymbol) 
      return(NULL);
            
   _XmVirtualToActualKeysym(XtDisplay(rc), keysym, &keysym, &mods);

   if (!(ks = XKeysymToString(keysym)))
      return(NULL);

   tmp[0] = '\0';
   if (mods & ControlMask)
      strcpy(tmp, "Ctrl ");

   if (mods & ShiftMask)
      strcat(tmp, "Shift ");
            
   strcat(tmp,"<KeyUp>");
   strcat(tmp, ks);

   return(XtNewString(tmp));
}

static void MenuBarInitialize (bar)
XmRowColumnWidget bar;
{
    Widget topManager;

    RC_IsHomogeneous(bar) = TRUE;
    RC_EntryClass(bar) = xmCascadeButtonWidgetClass;
    RC_SetArmed(bar,FALSE);
    bar->manager.traversal_on = False;
    bar->row_column.lastSelectToplevel = (Widget) bar;

    if (RC_PostButton(bar) == -1)
        RC_PostButton(bar) = Button1;
    

    if (RC_Packing(bar) == XmNO_PACKING)
        RC_Packing(bar) = XmPACK_TIGHT;

    if (RC_Orientation(bar) == XmNO_ORIENTATION)
        RC_Orientation(bar) = XmHORIZONTAL;

    if (RC_Spacing(bar) == XmINVALID_DIMENSION)
	RC_Spacing(bar) = 0;

    if (bar->core.tm.translations == NULL)
        bar->core.tm.translations = menu_parsed;

    XtOverrideTranslations(bar, menu_traversal_parsed);
    
    if (RC_MenuAccelerator(bar) && (*RC_MenuAccelerator(bar) == '\0'))
        if (!(RC_MenuAccelerator(bar) = GetRealKey(bar, "osfMenuBar")))
	      RC_MenuAccelerator(bar) = "<KeyUp>F10";

    /*
     * Add an event handler to both us and the associated widget; we
     * need one in case we have gadget children.
     */
    GetTopManager (bar, &topManager);
    XtAddEventHandler(bar, KeyPressMask|KeyReleaseMask,
        False, KeyboardInputHandler, bar);
    XtAddEventHandler(topManager, KeyPressMask|KeyReleaseMask,
        False, KeyboardInputHandler, bar);
    
    if (RC_MenuAccelerator(bar))
        DoProcessMenuTree(bar, XmADD);
}

/*
 * prepare postFromList: if its at its default state, its parent should
 * be in the list.  If a list has been specified but the count has not,
 * then set the count to 0.  This is only useful for Popup and Pulldown panes.
 */
static void PreparePostFromList(rowcol)
XmRowColumnWidget rowcol;
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
      /* use temp - postFromCount incremented in AddToPostFromList() */
      int cnt = rowcol->row_column.postFromCount;
      /* reset the postFromCount for correct AddToPostFromList() assignment */
      rowcol->row_column.postFromCount = 0;	

      for (i=0; i < cnt; i++)
      {
	 XmAddToPostFromList (rowcol, tempPtr[i]);
      }
   }
   else if (forceParent)
   {
      /* no postFromList, then parent of Popup is on this list */
      rowcol->row_column.postFromList[0] = XtParent(XtParent(rowcol));
   }
}

static void PopupInitialize (popup)
XmRowColumnWidget popup;
{
   popup->manager.traversal_on = False;
   popup->row_column.lastSelectToplevel = (Widget) popup;
   
   if (RC_PostButton(popup) == -1)
       RC_PostButton(popup) = Button3;
    
   if (RC_Packing(popup) == XmNO_PACKING)
       RC_Packing(popup) = XmPACK_TIGHT;

   if (RC_Orientation(popup) == (char) XmNO_ORIENTATION)
       RC_Orientation(popup) = XmVERTICAL;

   if (RC_HelpPb(popup) != NULL)
   {
      _XmWarning(popup, BadPopupHelpMsg);
      RC_HelpPb(popup) = NULL;
   }
   
   if (RC_Spacing(popup) == XmINVALID_DIMENSION)
       RC_Spacing(popup) = 0;

   if (popup->core.tm.translations == NULL)
       popup->core.tm.translations = menu_parsed;

   XtOverrideTranslations(popup, menu_traversal_parsed);

   /* If no accelerator specified, use the default */
   if (RC_MenuAccelerator(popup) && (*RC_MenuAccelerator(popup) == '\0'))
      if (!(RC_MenuAccelerator(popup) = GetRealKey(popup, "osfMenu")))
         RC_MenuAccelerator(popup) = "<KeyUp>F4";

   /* Save a copy of the accelerator string */
   if (RC_MenuAccelerator(popup))
      RC_MenuAccelerator(popup) = 
         (String) strcpy (XtMalloc(XmStrlen(RC_MenuAccelerator(popup)) + 1),
         RC_MenuAccelerator(popup));

   PreparePostFromList(popup);
    
   /* Add event handlers to all appropriate widgets */
   if (RC_PopupEnabled(popup))
   {
      AddPopupEventHandlers (popup);

      /* Register all accelerators */
      DoProcessMenuTree(popup, XmADD);
   }
}

static void PulldownInitialize (pulldown)
XmRowColumnWidget pulldown;
{
    pulldown->manager.traversal_on = False;
    pulldown->row_column.lastSelectToplevel = (Widget) NULL;

    if (RC_Packing(pulldown) == XmNO_PACKING)
        RC_Packing(pulldown) = XmPACK_TIGHT;

    if (RC_Orientation(pulldown) == (char) XmNO_ORIENTATION)
        RC_Orientation(pulldown) = XmVERTICAL;

    if (RC_HelpPb(pulldown) != NULL)
    {
        _XmWarning(pulldown, BadPulldownHelpMsg);
        RC_HelpPb(pulldown) = NULL;
    }

    if (RC_Spacing(pulldown) == XmINVALID_DIMENSION)
        RC_Spacing(pulldown) = 0;
    
    if (pulldown->core.tm.translations == NULL)
        pulldown->core.tm.translations = menu_parsed;

    XtOverrideTranslations(pulldown, menu_traversal_parsed);

    RC_MenuAccelerator(pulldown) = NULL;
    PreparePostFromList(pulldown);

    /* add event handler to myself for gadgets */
    XtAddEventHandler(pulldown, KeyPressMask|KeyReleaseMask,
		      False, KeyboardInputHandler, pulldown);
}
       

static void OptionInitialize (option)
XmRowColumnWidget option;
{
    char b[200];
    int n;
    Arg args[15];
    Widget topManager;
    Widget child;

    if (RC_HelpPb(option) != NULL)
    {
        _XmWarning(option, BadOptionHelpMsg);
        RC_HelpPb(option) = NULL;
    }

    RC_Packing(option) = XmPACK_TIGHT;
    RC_NCol(option) = 2;
    RC_Orientation(option) = XmHORIZONTAL;
    option->row_column.lastSelectToplevel = (Widget) option;

    if (RC_PostButton(option) == -1)
        RC_PostButton(option) = Button1;
    
    if (RC_Spacing(option) == XmINVALID_DIMENSION)
        RC_Spacing(option) = 3;

    if (option->core.tm.translations == NULL)
        option->core.tm.translations = option_parsed;

    XtOverrideTranslations(option,
         ((XmManagerClassRec *)XtClass(option))->manager_class.translations);

    /* Create the label widget portion of the option menu */
    n = 0;
    XtSetArg(args[n], XmNlabelString, RC_OptionLabel(option)); n++;
    child = XmCreateLabelGadget(option,"",args,n);
    XtManageChild (child);

    /* Create the cascade button widget portion of the option menu */
    sprintf (b, "%s_cascadeBtn", option->core.name);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, RC_OptionSubMenu(option)); n++;
    XtSetArg(args[n], XmNshadowThickness, MGR_ShadowThickness(option)); n++;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNmarginLeft, 0); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNmarginTop, 0); n++;
    XtSetArg(args[n], XmNmarginBottom, 0); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    child = XmCreateCascadeButtonGadget(option,b,args,n);
    XtManageChild (child);

    RC_MenuAccelerator(option) = NULL;

    /* Add event handlers for catching keyboard input */
    GetTopManager (option, &topManager);
    XtAddEventHandler(option, KeyPressMask|KeyReleaseMask,
        False, KeyboardInputHandler, option);
    XtAddEventHandler(topManager, KeyPressMask|KeyReleaseMask,
        False, KeyboardInputHandler, option);

    if (RC_Mnemonic(option))
        DoProcessMenuTree(option, XmADD);

    /* This forces tab group - should probably make this a dynamic default! */
    option->manager.navigation_type = XmNONE;
}

static void WorkAreaInitialize (work)
XmRowColumnWidget work;
{
    MGR_ShadowThickness(work) = 0;

    if (RC_PostButton(work) == -1)
        RC_PostButton(work) = Button1;
    
    if (RC_Packing(work) == XmNO_PACKING)
        RC_Packing(work) = XmPACK_TIGHT;

    if (RC_Orientation(work) == (char) XmNO_ORIENTATION)
        RC_Orientation(work) = XmVERTICAL;

    if (RC_HelpPb(work) != NULL)
    {
        _XmWarning(work, BadWorkAreaHelpMsg);
        RC_HelpPb(work) = NULL;
    }

    if (work->row_column.radio &&
        (RC_Packing(work) == XmNO_PACKING))
        RC_Packing(work) = XmPACK_TIGHT;

    if (RC_Spacing(work) == XmINVALID_DIMENSION)
        RC_Spacing(work) = 3;

    if (work->core.tm.translations == NULL)
        work->core.tm.translations =
	    (XtTranslations) xmManagerClassRec.core_class.tm_table;

    XtOverrideTranslations(work, ((XmManagerClassRec *)XtClass(work))->
                          manager_class.translations);

    RC_MenuAccelerator(work) = NULL;
    
}

/*
 * Initialize a row column widget
 */

static void Initialize (req, m)
XmRowColumnWidget req;              /* build from arglist */
XmRowColumnWidget m;              /* after superclass */
{
    if (!XtWidth(req))
    {
        XtWidth(m) = 16;
    }

    if (!XtHeight(req))
    {
        XtHeight(m) = 16;
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

    switch (RC_Orientation(req))
    {
        case XmNO_ORIENTATION:
        case XmVERTICAL:
        case XmHORIZONTAL:
        break;
        default:
            _XmWarning(m,BadOrientationMsg);
            RC_Orientation(m) = XmNO_ORIENTATION;
        break;
    }

    switch (RC_Packing(req))
    {
        case XmNO_PACKING:
        case XmPACK_TIGHT:
        case XmPACK_COLUMN:
        case XmPACK_NONE:
        break;
        default:
            _XmWarning(m,BadPackingMsg);
            RC_Packing(m) = XmNO_PACKING;
        break;
    }

    switch (RC_Type(req))
    {
        case XmWORK_AREA:
        case XmMENU_BAR:
        case XmMENU_OPTION:
            break;
        case XmMENU_POPUP:
        case XmMENU_PULLDOWN:
	    if (!XmIsMenuShell(XtParent(req)) ||
		!XtParent(XtParent(req)))
	    {
		_XmWarning(m,BadTypeParentMsg);
		RC_Type(m) = XmWORK_AREA;
	    }
	    break;
        default:
            _XmWarning(m,BadTypeMsg);
            RC_Type(m) = XmWORK_AREA;
            break;
    }

    switch (RC_EntryAlignment(req))
    {
        case XmALIGNMENT_BEGINNING:
        case XmALIGNMENT_CENTER:
        case XmALIGNMENT_END:
        break;
        default:
            _XmWarning(m,BadAlignmentMsg);
            RC_EntryAlignment(m) = XmALIGNMENT_BEGINNING;
        break;
    }

    RC_CascadeBtn(m) = NULL;
    RC_Boxes(m) = NULL;

    RC_SetExpose (m, TRUE);             /* and ready to paint gadgets */
    RC_SetWidgetMoved  (m, TRUE);       /* and menu and shell are not */
    RC_SetWindowMoved  (m, TRUE);       /* in synch, positiongally */
    RC_SetArmed  (m, TRUE);             /* are always armed */
    RC_SetPoppingDown  (m, FALSE);      /* not popping down */
    RC_PopupPosted(m) = NULL;		/* no popup submenus posted */

    /* create the menu cursor for this display, if there isn't one already */
    _XmCreateMenuCursor(m);

    if (m->manager.shadow_thickness == XmINVALID_DIMENSION)
	m->manager.shadow_thickness = 2;

    m->row_column.old_width = XtWidth(m);
    m->row_column.old_height = XtHeight(m);
    m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

    /* Post initialization for whichButton - done before PopupInitialize
     * because RC_PostModifiers used in eventual XtGrabButton()
     */
    RC_PostModifiers(m) = AnyModifier;
    RC_PostEventType(m) = ButtonPress;

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

    /* allow menuPost override */
    if ((RC_MenuPost(m) != NULL) && !IsPulldown(m)) {
        if (_XmMapBtnEvent(RC_MenuPost(m), &RC_PostEventType(m),
            &RC_PostButton(m), &RC_PostModifiers(m)) == FALSE)
        {
            _XmWarning(m,BadMenuPostMsg);
        }
    }



    SetMenuHistory (m, RC_MemWidget (m));
}

/* ARGSUSED */
static void ConstraintInitialize (req,new)
Widget req;             /* build from arglist */
Widget new;             /* after superclass */
{

    if (!XtIsRectObj(new)) return;

    WasManaged(new) = False;
}


/*
 * the main create section, mostly just tacks on the type to the arg
 * list
 */

static Widget create (p, name, old_al, old_ac, type, is_radio)
Widget  p;                  /* parent widget */
char   *name;
ArgList old_al;
Cardinal     old_ac;
int     type;               /* menu kind to create */
int     is_radio;               /* the radio flag */
{
    Arg al[50];
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
        Arg s_al[25];
        XmMenuShellWidget pop = NULL;
        Widget pw;
        int s_ac = 0;
        char b[200];

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
            XtSetArg (s_al[s_ac], XmNwidth,             5);     s_ac++;
            XtSetArg (s_al[s_ac], XmNheight,        5);     s_ac++;
            XtSetArg (s_al[s_ac], XmNallowShellResize, TRUE);   s_ac++;
            XtSetArg (s_al[s_ac], XtNoverrideRedirect, TRUE);   s_ac++;

            sprintf (b, "popup_%s", name);

            pop = (XmMenuShellWidget)XtCreatePopupShell(b, 
                     xmMenuShellWidgetClass, pw, s_al, s_ac);

            /* Mark the shell as having been created by us */
            pop->menu_shell.private_shell = True;
        }

        m = XtCreateWidget (name, xmRowColumnWidgetClass, pop, al, ac);
    }
    else
        m = XtCreateWidget (name, xmRowColumnWidgetClass, p, al, ac);

    return (m);
}


/*
 *************************************************************************
 *
 * Public Routines                                                        
 *
 *************************************************************************
 */

void XmMenuPosition (Widget p, XButtonPressedEvent *event)
{
   PositionMenu (p, event);
}


Widget XmCreateRowColumn (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, UNDEFINED_TYPE, FALSE));
}

Widget XmCreateWorkArea (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmWORK_AREA, FALSE));
}


Widget XmCreateRadioBox (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmWORK_AREA, TRUE));
}


Widget XmCreateOptionMenu (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmMENU_OPTION, FALSE));
}

Widget XmOptionLabelGadget (Widget m)
{
   int i;
   Widget child;
   
   if (XmIsRowColumn(m) && IsOption(m))
   {
      XmRowColumnWidget rowcol = (XmRowColumnWidget) m;
      
      for (i = 0; i < rowcol->composite.num_children; i++)
      {
	 child = rowcol->composite.children[i];

	 if (XtClass(child) == xmLabelGadgetClass)
	     return (child);
      }
   }

   /* did not find a label gadget in the child list */
   return (NULL);
}

Widget XmOptionButtonGadget (Widget m)
{
   int i;
   Widget child;
   
   if (XmIsRowColumn(m) && IsOption(m))
   {
      XmRowColumnWidget rowcol = (XmRowColumnWidget) m;
      
      for (i = 0; i < rowcol->composite.num_children; i++)
      {
	 child = rowcol->composite.children[i];

	 if (XmIsCascadeButtonGadget(child))
	     return (child);
      }
   }

   /* did not find a cascadebuttongadget in the child list */
   return (NULL);
}


Widget XmCreateMenuBar (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmMENU_BAR, FALSE));
}


Widget XmCreatePopupMenu (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmMENU_POPUP, FALSE));
}


Widget XmCreatePulldownMenu (Widget p, char *name, ArgList al, Cardinal ac)
{
    return (create (p, name, al, ac, XmMENU_PULLDOWN, FALSE));
}

void XmAddToPostFromList (XmRowColumnWidget menu, Widget widget)
{
   Arg args[1];

   /* only continue if its a vailid widget and a popup or pulldown menu */
   if (! XmIsRowColumn(menu) ||
       ! (IsPopup(menu) || IsPulldown(menu)) ||
       ! widget)
       return;
   
   if (OnPostFromList(menu, widget) == -1)
   {
      if (IsPulldown(menu))
      {
	 XtSetArg (args[0], XmNsubMenuId, menu);
	 XtSetValues (widget, args, 1);
      }
      else 
      {
	 AddToPostFromList (menu, widget);
	 AddHandlersToPostFromWidget (menu, widget);
      }
   }
}

void XmRemoveFromPostFromList (XmRowColumnWidget menu, Widget widget)
{
   Arg args[1];

   /* only continue if its a vailid widget and a popup or pulldown menu */
   if (! XmIsRowColumn(menu) ||
       ! (IsPopup(menu) || IsPulldown(menu)) ||
       ! widget)
       return;
	
   if ((OnPostFromList(menu, widget)) == -1)
   {
      if (IsPulldown(menu))
      {
	 XtSetArg (args[0], XmNsubMenuId, NULL);
	 XtSetValues (widget, args, 1);
      }
      else 
      {
	 RemoveFromPostFromList (menu, widget);
	 RemoveHandlersFromPostFromWidget (menu, widget);
      }
   }
}

/*
 * Return the widget which the menu was posted from.  If this is in a popup,
 * it is the widget which initiated the post (via positioning & managing or
 * via armAndActivate).  If it is in a pulldown from a menubar or option menu,
 * then the returned widget is the menubar or option menu.
 */
Widget XmGetPostedFromWidget (Widget menu)
{
   Widget toplevel = 
     ((XmRowColumnWidget) menu)->row_column.lastSelectToplevel;

   if (XmIsRowColumn(menu))
   {
      if (IsPopup(toplevel))
      {
	 /* active widget is kept in cascadeBtn field for popups */
	 return (RC_CascadeBtn(toplevel));
      }
      else
	 return (toplevel);
   }
   return (NULL);
}


/*
 * class initialization
 */

/*
 * needed for funky menubar mode so that the traversal can be restored
 * to the correct tabgroup when we are done.
 */
static Widget tabGroup;

static void ClassInitialize ()
{
   /*
    * parse the various translation tables
    */

   menu_parsed   = XtParseTranslationTable (menu_table);
   option_parsed    = XtParseTranslationTable (option_table);
   menu_traversal_parsed = XtParseTranslationTable (menu_traversal_table);

   /* initialize the menu cursor context */
   _XmInitializeMenuCursor();

   /* set up the menu procedure entry for button children to access */
   _XmSaveMenuProcContext( (caddr_t) MenuProcedureEntry);
   
   tabGroup = NULL;
}


static void ClassPartInitialize (rcc)
    XmRowColumnWidgetClass  rcc;
{
    _XmFastSubclassInit(rcc,XmROW_COLUMN_BIT);
}


/***************************************************************************
 *
 *
 * next section is action routines, these are called by the row column event
 * handler in response to events.  The handler parses its way through
 * the actions table calling these routines as directed.  It is these
 * routines which will typically decide the application should be
 * called via a callback.  This is where callback reasons are produced.
 *
 */

    

static XmRowColumnWidget find_menu (w)
Widget w;
{
    if (XmIsRowColumn(w))
        return ((XmRowColumnWidget) w);        /* row column itself */
    else
        return ((XmRowColumnWidget) XtParent (w)); /* subwidget */
}



/*
 * popdown anything that should go away
 */

static void MenuPopDown (w, event)
Widget  w;
XEvent * event;
{
   XmRowColumnWidget rc = find_menu(w);

   /*
    * find the top of this cascade and popdown anything posted on it
    */
   _XmGetActiveTopLevelMenu (rc, &rc);

   if (IsPopup(rc))
       (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	  menu_shell_class.popdownDone))(rc, event);
       
   else if (RC_PopupPosted(rc))
   {
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	 menu_shell_class.popdownDone))(RC_PopupPosted(rc), event);

      /* This kludges a fix so that F10 followed by mnemonic selection
       * doesn't call MenuFocusIn and leave the cascade button highlighted.
       */
      if (IsBar(rc))
	 (void)XmProcessTraversal(rc, XmTRAVERSE_CURRENT);
   }
}


static void MenuArm (w)
Widget  w;
{
    XmRowColumnWidget m = find_menu(w);

    if (!RC_IsArmed(m))
    {
        if (IsBar(m))
        {
            /*
             * Menubars need their own exclusive/SL grab, so that they will
             * still get input even when a cascade button without a submenu
             * has the focus.
             */
            _XmSetTransientFlag(m, True);
            XtAddGrab(m, True, True);
            RC_SetBeingArmed(m, True);
        }
        RC_SetArmed (m, True);
    }
}



static void MenuDisarm (w)
Widget  w;
{
    XmRowColumnWidget m = find_menu(w);

    if (RC_IsArmed(m))
    {
        if (IsBar(m))
        {
            _XmSetTransientFlag(m, False);
            XtRemoveGrab(m);
            RC_SetBeingArmed(m, False);
        }

        RC_SetArmed (m, FALSE);
    }
}




/**********************************************************************
 *
 * next section knows how to composite row column entries
 */

static void FixEventBindings (m, w)
XmRowColumnWidget  m;       /* row column (parent) widget */
Widget      w;          /* subwidget */
{
   if (XtIsWidget(w) &&
       ((IsPopup(m) || IsBar(m) || IsPulldown(m)) && 
	 XmIsLabel(w) && (w->core.widget_class != xmLabelWidgetClass)))
   {
      XtAddEventHandler(w, KeyPressMask|KeyReleaseMask, False,
			KeyboardInputHandler, m);

   }

   /* set up accelerators and mnemonics */
   ProcessSingleWidget (w, XmADD);
}


/*
 * Action routines for controlling traversal.
 */

static void KickOnTraversal (m)
XmRowColumnWidget m;
{
   XEvent event;
   Widget menupane;

   if (XmIsRowColumn(m))
   {
      if (RC_PopupPosted(m))
      {
	 menupane =
	     ((XmMenuShellWidget) RC_PopupPosted(m))->composite.children[0];

	 event.type = FocusIn;
	 event.xfocus.send_event = True;
	 (void) XmProcessTraversal (menupane, XmTRAVERSE_CURRENT);
	 _XmManagerFocusInInternal(menupane, &event);

	 if (!IsBar(m))
	     m->manager.active_child = RC_CascadeBtn(menupane);
	 return;
      }
       
      /* Set it to ourselves */
      event.type = FocusIn;
      event.xfocus.send_event = True;
      (void) XmProcessTraversal (m, XmTRAVERSE_CURRENT);
      _XmManagerFocusInInternal(m, &event);
   }
}


static void KickOffTraversal (m)
XmRowColumnWidget m;
{
    Widget rc;
    XmGadget gadget;
    CompositeWidget shell;
    CompositeWidget oldShell = NULL;

    shell = (CompositeWidget) RC_PopupPosted(m);

    /* find last posted menushell */
    while (oldShell != shell)
    {
       oldShell = shell;

       if (RC_PopupPosted(shell->composite.children[0]))
       {
	  shell =
	      (CompositeWidget) RC_PopupPosted(shell->composite.children[0]);
       }
    }

    if (shell)
	rc = shell->composite.children[0];
    else
	rc = (Widget) m;

    /* 
     * Clear focus entries for each posted menupane.
     */
    while (rc && 
           ((RC_Type(rc) == XmMENU_POPUP) || 
            (RC_Type(rc) == XmMENU_PULLDOWN)))
    {
       /*
	* Because a gadget will not get focus events, and because the
	* parent may also not get one (because of a toolkit optimization),
	* we need to handle gadget specially.
	*/
       gadget = (XmGadget)((XmManagerWidget)rc)->manager.active_child;

       /* Inform the gadget that it should unhilite & clean itself up */
       if (gadget && XmIsGadget(gadget))
       {
	  /* Direct focus to rc to handle focus events for gadgets */
	  _XmSetFocusResetFlag(rc, True); 
	  (void) XmProcessTraversal (rc, XmTRAVERSE_CURRENT);
	  _XmSetFocusResetFlag(rc, False);

	  if (ShouldDispatchFocusOut(gadget))
	  {
	     /* have to set active_child to NULL for CascadeBG to unhighlight */
             ((XmManagerWidget)rc)->manager.active_child = NULL;

	     _XmDispatchGadgetInput(gadget, NULL, XmFOCUS_OUT_EVENT);
	     gadget->gadget.have_traversal = False;
	  }
       }

       /* clears the focus_item so that next TraverseToChild() will work */
       _XmClearFocusPath(rc);

       if (! IsPopup(rc) && RC_CascadeBtn(rc))
	   rc = XtParent(RC_CascadeBtn(rc));

       else
	   rc = NULL;
    }
}


/*
 * When we drop back into traversal mode, we need to set the active child
 * field for each visible menupane.
 */

static void SetActiveChildren (toplevel)
XmRowColumnWidget toplevel;
{
   XmRowColumnWidget rc;
   XmRowColumnWidget subpane;

   if (IsOption(toplevel))
   {
      /* Link option submenu and its memory widget */
      rc = (XmRowColumnWidget)toplevel->row_column.option_submenu;
      rc->manager.active_child = (Widget)RC_MemWidget(rc);
      return;
   }

   while (1)
   {
      if (RC_PopupPosted(toplevel))
      {
	 subpane = (XmRowColumnWidget)
            ((CompositeWidget) RC_PopupPosted(toplevel))->composite.children[0];
	    
	 if (! IsBar(toplevel))
	 {
	    toplevel->manager.active_child = RC_CascadeBtn(subpane);
	 }

	 toplevel = subpane;
      }
      else
      {
	 /* No shells popped up; use first available widget */
	 toplevel->manager.active_child = NULL;
	 return;
      }
   }
}

/*
 * Class function used to enable or disable traversal.  This is called
 * when the menu system is active.
 */

static void SetMenuTraversal (m, on)
XmRowColumnWidget m;
Boolean on;
{
   XmRowColumnWidget topLevel;
   XmRowColumnWidget pane;

   _XmGetActiveTopLevelMenu (m, &topLevel);
   
   if (on)
   {
      /* The order here is important */
      SetActiveChildren(topLevel);
      SetTraversal(topLevel, True);
      KickOnTraversal(m);
   }
   else
   {
      /* Do something only if traversal is currently enabled */
      if (((IsBar(topLevel) || IsPopup(topLevel)) &&
	   topLevel->manager.traversal_on) ||
	  (IsOption(topLevel) &&
	   (pane = (XmRowColumnWidget)topLevel->row_column.option_submenu) &&
	   (pane->manager.traversal_on)))
      {
          /* The order here is important */
          KickOffTraversal(m);
          SetTraversal(topLevel, False);
      }
   }
}


/*
 * Class function which is used to clean up the menubar when it is leaving
 * the mode where the user has pressed F10 to start traversal in the
 * menubar.
 */

static void MenuBarCleanup (rc)
XmRowColumnWidget rc;
{
    /*
     * We can tell if this mode is active by looking at the 'active_child'
     * field in the menubar; it should only be set when we are in this mode.
     */
    if (rc->manager.active_child)
    {
        if (XmIsPrimitive(rc->manager.active_child))
        {
            (*(((XmPrimitiveClassRec *)XtClass(rc->manager.active_child))->
              primitive_class.border_unhighlight))(rc->manager.active_child);
        }

        else if (XmIsGadget(rc->manager.active_child))
        {
            (*(((XmGadgetClassRec *)XtClass(rc->manager.active_child))->
              gadget_class.border_unhighlight))(rc->manager.active_child);
        }

        _XmSetFocusResetFlag(rc, True);
        (void) XmProcessTraversal (rc, XmTRAVERSE_CURRENT);
        _XmSetFocusResetFlag(rc, False);
        rc->manager.active_child = NULL;
    }

    /*
     * restore the tabgroup to the widget that had the focus BEFORE the
     * menubar mode was entered.
     */
    if (tabGroup)
    {
       (void) XmProcessTraversal (tabGroup, XmTRAVERSE_CURRENT);
       tabGroup = NULL;
    }
}


/* ARGSUSED */
void _XmMenuFocus (Widget w, int operation, Time _time)
{
    static int	oldFocus = NULL;
    static int	oldRevert = 0;
 
   switch (operation)
      {
	case XmMENU_END:
	  if (oldFocus)
	    {
		XSetInputFocus(XtDisplay(w),
			       oldFocus,
			       oldRevert,
			       CurrentTime);
		oldFocus = oldRevert = NULL;
		XtUngrabKeyboard(w, CurrentTime);
	    }
	  break;
	case XmMENU_BEGIN:
	  /* We must grab the keyboard before the InputFocus is set for mwm
	   * to work correctly.
	   */
	  XtGrabKeyboard(w, True, GrabModeSync, GrabModeSync, CurrentTime);
	  XGetInputFocus(XtDisplay(w), &oldFocus, &oldRevert);
	  XSetInputFocus(XtDisplay(w), XtWindow(w), oldRevert,
			 CurrentTime);

	  /* These two round trips are to support the broken >= R4 server
	   * which does not unfreeze properly when XAllowEvents is called
	   * with AsyncBoth
	   */
	  XAllowEvents(XtDisplay(w), AsyncKeyboard, CurrentTime);
	  XAllowEvents(XtDisplay(w), AsyncPointer, CurrentTime);
	  XFlush(XtDisplay(w));

	  break;
	case XmMENU_MIDDLE:
	  XSetInputFocus(XtDisplay(w), XtWindow(w), oldRevert,
			 CurrentTime);
	  break;
      }
}


/*
 * Class function which is invoked when the post accelerator is received
 * for a popup menu or the menubar, or the post mnemonic is received for 
 * an option menu.
 */

static void ArmAndActivate (m, event)
XmRowColumnWidget m;
XKeyPressedEvent * event;
{
   int i;
   XmCascadeButtonWidget child;
   XRectangle visRect;
   Cursor cursor;

   if (IsPopup(m))
   {
      if (!XtIsManaged(m))
      {
	 Position x, y;

	 /* the posted from widget is saved in RC_CascadeBtn */
	 RC_CascadeBtn(m) = XtWindowToWidget(XtDisplay(m), event->window);
	 
	 /* Position & post menupane; then enable traversal */
	 RC_SetWidgetMoved(m, True);
	 /* Position the pane off of the parent of this rowcolumn's menushell.
	  * Place it in the upper left corner.
	  */
	 XtTranslateCoords(XtParent(XtParent(m)), 0, 0, &x, &y);

	 /* Verify popup for MenuShell's manage_set_changed() */
	 _XmButtonEventStatus.time = event->time;
	 _XmButtonEventStatus.verified = True;

	 XtX(m) = x;
	 XtY(m) = y;
	 XtManageChild(m);
	 SetActiveChildren(m);
	 SetTraversal(m, True);
	 KickOnTraversal(m);
      }
      else
      {
         /* Let the menushell widget clean things up */
         (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
            menu_shell_class.popdownDone))(XtParent(m), event);
      }
   }
   else if (IsOption(m))
   {
      XmGadget g = (XmGadget) XmOptionButtonGadget(m);

      /* Let the cascade button gadget do the work */
      (*(((XmGadgetClassRec *)XtClass(g))->gadget_class.
          arm_and_activate)) (g, event);
   }
   else if (IsBar(m))
   {
      if (RC_IsArmed(m))
      {
         /*
          * If the menubar is already armed, then F10 unposts and disarms
          * the menu system.  Handle PM menubar mode specially.
          */
         if (!m->row_column.popupPosted)
         {
            /* No submenus posted; must clean up ourselves */
            MenuDisarm(m);

	    _XmMenuFocus(m, XmMENU_END, CurrentTime);
            XtUngrabKeyboard(m, CurrentTime);
            XtUngrabPointer(m, CurrentTime);

            SetMenuTraversal(m, False);
            MenuBarCleanup(m);
         }
         else
         {
            /* Submenus are posted; let MenuPopDown() clean up */
            MenuPopDown(m, event);
         }
      }
      else
      {
         /*
          * If the menubar is not armed, then look to see if there is a
          * cascade button with a submenu; if none found, then return.
          * Otherwise, highlight and arm that button.
          */

         _XmCreateVisibilityRect(m, &visRect);
         for (i = 0; i < m->composite.num_children; i++)
         {
	    child = (XmCascadeButtonWidget)m->composite.children[i];

	    /* You can't traverse to a button which has no submenu */
	    if ((XmIsCascadeButton(child) && (CB_Submenu(child) == NULL)) ||
		(XmIsCascadeButtonGadget(child) &&
		 (CBG_Submenu(child) == NULL)))
                continue;

	    if (_XmTestTraversability(child, &visRect))
                break;
         }

         /* See if we found one */
         if (i >= m->composite.num_children)
            return;

         /*
          * Menubars need their own exclusive/SL grab, so that they will
          * still get input even when a cascade button without a submenu
          * has the focus.
          *
          * We can't call MenuArm() here, because it does the pointer grab
          * before our keyboard grab.
          */
         _XmSetTransientFlag(m, True);
         XtAddGrab(m, True, True);
         RC_SetArmed(m, True);

         /* Force the application to unhilite itself */
         m->manager.active_child = m->composite.children[i];

	 /*
	  * save the current tabgroup so that it can be restored after
	  * the menubar traversal mode is finished.
	  */
	 tabGroup = _XmGetTabGroup(m);
         (void) XmProcessTraversal (m, XmTRAVERSE_CURRENT);

         if (XtIsWidget(m->manager.active_child))
            (void) XmProcessTraversal (m->manager.active_child, XmTRAVERSE_CURRENT);
	 
	 cursor = XmGetMenuCursor(XtDisplay(m));
	 _XmMenuFocus(m, XmMENU_BEGIN, CurrentTime);
         XtGrabPointer (m, True, ButtonPressMask | ButtonReleaseMask |
			EnterWindowMask | LeaveWindowMask,
			GrabModeAsync, GrabModeAsync, None, cursor,
			CurrentTime);
         SetTraversal(m, True);

         if (XmIsPrimitive(m->manager.active_child))
         {
            (*(((XmPrimitiveClassRec *)XtClass(m->manager.active_child))->
                primitive_class.border_highlight)) (m->manager.active_child);
         }
         else if (XmIsGadget(m->manager.active_child))
         {
            (*(((XmGadgetClassRec *)XtClass(m->manager.active_child))->
                gadget_class.border_highlight)) (m->manager.active_child);
         }
      }
   }
   else if (IsPulldown(m))	/* Catch the Escape in a cascading menu! */
   {
      /* Let the menushell widget clean things up */
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
         menu_shell_class.popdownOne))(XtParent(m), event);
   }
}


/*
 * The following functions are used to manipulate lists of keyboard events
 * which are of interest to the menu system; i.e. accelerators and mnemonics.
 * The top level component within a menu system (the menubar, or the popup
 * menupane, or the option menu, or the work area menu) keeps a list of
 * the events it cares about.  Items are only added to the list when the
 * associated menu component has a complete path to the top level component.
 * Items are removed when the complete path is broken.
 *
 * The times at which a complete path may be formed is 1) When a button
 * in a menupane becomes managed, when a pulldown menupane is attached
 * to a cascading button, when the application sets the popupEnabled
 * resource to True, or when an option menu is created.  A complete path 
 * may be broken when 1) a button in a menupane is unmanaged, a pulldown 
 * menupane is detached from a cascading button, when the application clears 
 * the popupEnabled resource or an option menu is destroyed.
 *
 * The event handler for catching keyboard events is added by the row column
 * widget during its initialize process, and removed when the row column
 * widget is destroyed.  Keyboard grabs, which are needed to make accelerators
 * work, are added either when the accelerator is registered, or when the 
 * associated widget is realized; grabs cannot be added to a widget which 
 * does not have a window!
 */ 


/*
 * This function is used both by the row column widget, and components which
 * are contained within a menu (toggle, pushbutton and cascadebutton).  The
 * row column widget uses it to process a tree of menu components; when a
 * menupane is linked to a cascade button, the new menupane, along with any
 * submenus cascading from it, will be processed.  The row column widget
 * will use the XmADD or XmDELETE mode to to this.  When a menu component
 * needs to change its accelerator or mnemonics, it will use the XmREPLACE
 * mode.
 *
 * When this function is used to delete a tree of keyboard actions, the
 * link between the menupane at the root (parameter 'w') and the cascade
 * button it is attached to must not yet have been broken.  This allows
 * the function to trace up the hierarchy and find the top level widget.
 */

static void DoProcessMenuTree (w, mode)
Widget w;
int    mode;                    /* Add, Replace or Delete */
{
   /*
    * Depending upon the type of widget 'w' is, we may end up adding/deleting
    * keyboard actions for no widgets, the specified widget only, or the
    * specified widget and all others which cascade from it.
    */
   if (XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w))
   {
      /* If our parent is not a row column, then abort */
      if (XmIsRowColumn(XtParent(w)))
      {
         if (IsOption(XtParent(w)))
         {
            if (mode == XmREPLACE)
               return;

            /*
             * A cascade button in an option menu does not have an
             * accelerator or a mnemonic associated with it.  However,
             * its submenu may, so we need to process it.
             */
            if (XmIsCascadeButtonGadget(w))
                w = (Widget)CBG_Submenu(w);
         }
         else if (IsWorkArea(XtParent(w)))
         {
            /*
             * Since work area menus do not support cascade buttons
             * with submenus, we will treat this like a pushbutton.
             */
            if (mode == XmREPLACE)
            {  
               /* Remove old one, if it exists */
               ProcessSingleWidget(w, XmDELETE);
               mode = XmADD;
            }

            ProcessSingleWidget(w, mode);
            return;
         }
         else if (IsBar(XtParent(w)) || 
                  IsPopup(XtParent(w)) ||
                  IsPulldown(XtParent(w)))
         {
            if (mode == XmREPLACE)
            {
               /* When replacing, don't worry about submenus */
               ProcessSingleWidget(w, XmDELETE);
               ProcessSingleWidget(w, XmADD);
               return;
            }

            /*
             * If we are in a menubar, a popup menupane or a pulldown
             * menupane, then we need to not only modify our keyboard 
             * event, but also any which are defined in our submenu.
             */
            ProcessSingleWidget(w, mode);

            if (XmIsCascadeButtonGadget(w))
                w = (Widget)CBG_Submenu(w);
            else
                w = (Widget)CB_Submenu(w);
         }
      }
   }
   else if (XmIsToggleButtonGadget(w) || XmIsToggleButton(w) ||
            XmIsPushButtonGadget(w) || XmIsPushButton(w))
   {
      if (mode == XmREPLACE)
      {
         /* Remove old one */
         ProcessSingleWidget(w, XmDELETE);
         mode = XmADD;
      }

      /*
       * In both of these cases, we need only modify the keyboard
       * event associated with this widget.
       */
      ProcessSingleWidget(w, mode);
      return;
   }
   else if (XmIsRowColumn(w))
   {
      /*
       * When the popupEnabled resource is enabled for a popup menupane,
       * we need to add the accelerator associated with the menu, followed 
       * by all keyboard events associated with submenus.  The reverse
       * happens when the resource is disabled.
       *
       * When an option menu is created, we will add its posting mnemonic;
       * its submenu is taken care of when it is attached to the cascading
       * button.
       */
      if (IsPopup(w))
      {
         if (mode == XmREPLACE)
         {
            /* Don't worry about submenus during a replace operation */
            ProcessSingleWidget(w, XmDELETE);
            ProcessSingleWidget(w, XmADD);
            return;
         }

         ProcessSingleWidget(w, mode);
      }
      else if (IsOption(w) || IsBar(w))
      {
         if (mode == XmREPLACE)
         {
            ProcessSingleWidget(w, XmDELETE);
            mode = XmADD;
         }

         ProcessSingleWidget(w, mode);
         return;
      }
   }
   else
   {
      /* Unknown widget type; do nothing */
      return;
   }

   /* Process any submenus */
   ProcessMenuTree(w, mode);
}


/*
 * Given a row column widget, all keyboard events are processed
 * for the items within the row column widget, and then recursively
 * for any submenus cascading from this row column widget.
 */

static void ProcessMenuTree (w,  mode)
XmRowColumnWidget w;
int mode;
{
   int i;
   Widget child;

   if (w == NULL)
      return;

   for (i = 0; i < w->composite.num_children; i++)
   {
      if (XtIsManaged((child = w->composite.children[i])))
      {
         ProcessSingleWidget(child, mode);

         if (XmIsCascadeButtonGadget(child))
         {
            ProcessMenuTree(CBG_Submenu(child), mode);
         }
         else if (XmIsCascadeButton(child))
         {
            ProcessMenuTree(CB_Submenu((XmCascadeButtonWidget)child), mode);
         }
      }
   }
}


/*
 * This function adds/deletes the mnemonic and/or accelerator associated
 * with the specified widget.  The work that is done is dependent both
 * on the widget in question, and sometimes the parent of the widget.
 *
 * When adding a keyboard event, we first check the component to see if
 * it has any keyboard events defined; if not, then nothing is done.  However,
 * when removing a keyboard event, we simply attempt to remove the entry for
 * the specified widget; we can't check to see if the widget had one defined,
 * because the instance structure may no longer contain the information.
 */

static void ProcessSingleWidget (w, mode)
Widget w;
int mode;
{
   char buf[20];
   Arg args[2];
   Widget child;

   if (XmIsCascadeButtonGadget(w))
   {
      XmCascadeButtonGadget c = (XmCascadeButtonGadget)w;

      if (XmIsRowColumn(XtParent(w)) &&
          (IsBar(XtParent(w))))
      {
         if (mode == XmADD)
         {
            /* Menubar mnemonics are prefixed with the Mod1 modifier */
            if (LabG_Mnemonic(c) != NULL)
            {
               strcpy (buf, "Mod1<KeyUp>");
	       strcat (buf,  XKeysymToString(LabG_Mnemonic(c)));

	       /* although this is technically a mnemonic, it will be 
                * treated like an acclerator */ 
               AddToKeyboardList(w, buf, True, False);

	       /* save it again as a mnemonic so that it is available w/o
                * the Mod1 when the menu system is posted */
               strcpy (buf, "<KeyUp>");
	       strcat (buf,  XKeysymToString(LabG_Mnemonic(c)));
               AddToKeyboardList(w, buf, False, True);

            }
         }
         else
            RemoveFromKeyboardList(w);
      }
      else
      {
         if (mode == XmADD)
         {
            /* All other mnemonics are done without any modifiers */
            if (LabG_Mnemonic(c) != NULL)
            {
               strcpy (buf, "<KeyUp>");
	       strcat (buf,  XKeysymToString(LabG_Mnemonic(c)));
               AddToKeyboardList(w, buf, False, True);
            }
         }
         else
            RemoveFromKeyboardList(w);
      }
   }
   else if (XmIsCascadeButton(w))
   {
      XmCascadeButtonWidget c = (XmCascadeButtonWidget)w;

      if (XmIsRowColumn(XtParent(w)) &&
          (IsBar(XtParent(w))))
      {
         if (mode == XmADD)
         {
            /* Menubar mnemonics are prefixed with the Mod1 modifier */
            if (Lab_Mnemonic(c) != NULL)
            {
               strcpy (buf, "Mod1<KeyUp>");
	       strcat (buf,  XKeysymToString(Lab_Mnemonic(c)));

	       /* save as an accelerator since it is available anytime */
               AddToKeyboardList(w, buf, True, False);

	       /* save again as a mnemonic so its available w/o Mod1
		* once the menu system is active */ 
               strcpy (buf, "<KeyUp>");
	       strcat (buf,  XKeysymToString(Lab_Mnemonic(c)));
               AddToKeyboardList(w, buf, False, True);
            }
         }
         else
            RemoveFromKeyboardList(w);
      }
      else
      {
         if (mode == XmADD)
         {
            /* All other mnemonics are done without any modifiers */
            if (Lab_Mnemonic(c) != NULL)
            {
               strcpy (buf, "<KeyUp>");
	       strcat (buf,  XKeysymToString(Lab_Mnemonic(c)));
               AddToKeyboardList(w, buf, False, True);
            }
         }
         else
            RemoveFromKeyboardList(w);
      }
   }
   else if (XmIsToggleButtonGadget(w) ||
            XmIsPushButtonGadget(w))
   {
      XmLabelGadget l = (XmLabelGadget) w;

      if (mode == XmADD)
      {
         /* These can have both an accelerator and a mnemonic */
         if (LabG_Mnemonic(l) != NULL)
         {
            strcpy (buf, "<KeyUp>");
	    strcat (buf,  XKeysymToString(LabG_Mnemonic(l)));
            AddToKeyboardList(w, buf, False, True);
         }

         if (LabG_Accelerator(l) && (strlen(LabG_Accelerator(l)) > 0))
         {
            AddToKeyboardList(w, LabG_Accelerator(l), True, False);
         }
      }
      else
         RemoveFromKeyboardList(w);
   }
   else if (XmIsToggleButton(w) ||
            XmIsPushButton(w))
   {
      XmLabelWidget l = (XmLabelWidget) w;

      if (mode == XmADD)
      {
         /* These can have both an accelerator and a mnemonic */
         if (Lab_Mnemonic(l) != NULL)
         {
            strcpy (buf, "<KeyUp>");
	    strcat (buf,  XKeysymToString(Lab_Mnemonic(l)));
            AddToKeyboardList(w, buf, False, True);
         }

         if (Lab_Accelerator(l) && (strlen(Lab_Accelerator(l)) > 0))
         {
            AddToKeyboardList(w, Lab_Accelerator(l), True, False);
         }
      }
      else
         RemoveFromKeyboardList(w);
   }
   else if (XmIsRowColumn(w))
   {
      XmRowColumnWidget m = (XmRowColumnWidget) w;

      if (IsPopup(m) || IsBar(m))
      {
         /* 
          * Popup Menus and the menubar may have an accelerator associated 
          * with them 
          */
         if (mode == XmADD)
         {
            if (RC_MenuAccelerator(m) && (strlen(RC_MenuAccelerator(m)) > 0))
            {
               AddToKeyboardList(w, RC_MenuAccelerator(m), True, False);
            }
         }
         else
            RemoveFromKeyboardList(w);
      }
      else if (IsOption(m))
      {
         /* Option menus may have a mnemonics associated with them */
         if (mode == XmADD)
         {
            if (RC_Mnemonic(m))
            {
               strcpy (buf, "Mod1<KeyUp>");
	       strcat (buf,  XKeysymToString(RC_Mnemonic(m)));
               AddToKeyboardList(w, buf, True, True);
            
               /* Tell the label gadget */
               XtSetArg(args[0], XmNmnemonic, RC_Mnemonic(m));
               XtSetArg(args[1], XmNmnemonicCharSet, RC_MnemonicCharSet(m));
	       if (child = XmOptionLabelGadget(m))
		   XtSetValues(child, args, 2);
            }
         }
         else
         {
            RemoveFromKeyboardList(w);
            
            /* Tell the label gadget */
            if (child = XmOptionLabelGadget(m))
            {
               XtSetArg(args[0], XmNmnemonic, '\0');
	       XtSetValues(child, args, 1);
            }
         }
      }
   }
}


/*
 * This function actually does the work of converting the accelerator
 * or mnemonic string into a workable format, and registering the keyboard 
 * grab, if possible.
 */

static void AddToKeyboardList (w, kbdEvent, needGrab, isMnemonic)
Widget w;
char * kbdEvent;
Boolean needGrab;
Boolean isMnemonic;
{
   Widget rowcol;
   unsigned int eventType;
   KeySym keysym;
   unsigned int modifiers;
   KeyCode detail;
   XmKeyboardData * list;
   int i;

   if ((kbdEvent == NULL) ||
       (_XmMapKeyEvent(kbdEvent, &eventType, &keysym, &modifiers) == False))
   {
      return;
   }

   /* Convert keysym to keycode; needed by X grab call */
   if ((detail = XKeysymToKeycode(XtDisplay(w), keysym)) == NoSymbol)
   {
      return;
   }

   if (XmIsRowColumn(w))
       rowcol = w;
   else
       rowcol = XtParent(w);
   
   /* Add to the list of keyboard entries */
   if (MGR_NumKeyboardEntries(rowcol) >= MGR_SizeKeyboardList(rowcol))
   {
      /* Grow list */
      MGR_SizeKeyboardList(rowcol) += 10;
      MGR_KeyboardList(rowcol) = 
             (XmKeyboardData *)XtRealloc(MGR_KeyboardList(rowcol), 
                  (MGR_SizeKeyboardList(rowcol) * sizeof(XmKeyboardData)));
   }

   list = MGR_KeyboardList(rowcol);
   i = MGR_NumKeyboardEntries(rowcol);
   list[i].eventType = eventType;
   list[i].keysym = keysym;
   list[i].key = detail;
   list[i].modifiers = isMnemonic ? 
      (modifiers &  ~(ShiftMask | LockMask)) : modifiers;
   list[i].component = w;
   list[i].needGrab = needGrab;
   list[i].isMnemonic = isMnemonic;
   MGR_NumKeyboardEntries(rowcol)++;

   if (needGrab)
   {
      GrabKeyOnAssocWidgets (rowcol, detail, modifiers);
   }
}


/*
 * This function removes all keyboard entries associated with a particular
 * component within a row column widget.
 */

static void RemoveFromKeyboardList (w)
Widget w;
{
   Widget rowcol;
   XmKeyboardData * list;
   int count;
   int i, j;

   if (XmIsRowColumn(w))
       rowcol = w;
   else
       rowcol = XtParent(w);

   list = MGR_KeyboardList(rowcol);
   count = MGR_NumKeyboardEntries(rowcol);
   
   for (i = 0; i < count; )
   {
      if (list[i].component == w)
      {
	 /* NOTE that the ungrabs on the associate widgets are not done
	  * for completeness, they probably should be.  The problem is that
	  * it is difficult to tell whether an item should really be
	  * ungrabbed since the sharing of menupanes could mean that this
	  * item exists somewhere else on this hierarchy. 
	  */

         /* Move the rest of the entries up 1 slot */
         for (j = i; j < count -1; j++)
	     list[j] = list[j+1];

         MGR_NumKeyboardEntries(rowcol) = MGR_NumKeyboardEntries(rowcol)-1;
         count--;
      }
      else
         i++;
   }
}


/*
 * This function searches the list of keyboard events associated with the
 * specified  row column widget to see if any of them match the
 * passed in X event.  This function can be called multiple times, to get
 * all entries which match.
 */

static int _XmMatchInKeyboardList (rowcol, event, startIndex)
XmRowColumnWidget rowcol;
XKeyEvent * event;
int startIndex;
{
   XmKeyboardData * list = MGR_KeyboardList(rowcol);
   int count = MGR_NumKeyboardEntries(rowcol);
   int i;

   if (list == NULL)
      return(-1);

   for (i = startIndex; i < count; i++)
   {
      /*
       * We want to ignore shift and shift-lock for mnemonics.  So, OR the 
       * event's two bits with the (previously two bits initialized to zero) 
       * list.modifier
       */
      if (_XmMatchKeyEvent(event, list[i].eventType, list[i].key, 
           list[i].isMnemonic ? 
	      list[i].modifiers | (event->state & (ShiftMask | LockMask)) :
	      list[i].modifiers)) 
      {
         return(i);
      }
   }

   /* No match */
   return (-1);
}

/*
 * search the postFromList and return the index of the found widget.  If it
 * is not found, return -1
 */
static int OnPostFromList (menu, widget)
XmRowColumnWidget menu;
Widget widget;
{
   int i;

   for (i = 0; i < menu->row_column.postFromCount; i++)
   {
      if (menu->row_column.postFromList[i] == widget)
	  return (i);
   }

   return (-1);
}
   
/*
 * Useful for MenuBars and Option Menus to determine where to set up the
 * event handlers and grabs.
 */
static void GetTopManager (w, topManager)
Widget w;
Widget * topManager;
{
   while (XmIsManager(XtParent(w)))
       w = XtParent(w);

   * topManager = w;
}

/*
 * Returns the toplevel menu widget in an acive menu hierarchy.
 *
 * This function is only useful when the menu system is active.  That is
 * the only time that the CascadeBtn field in the RowColumn in guarrenteed
 * to be valid.  
 */
void _XmGetActiveTopLevelMenu (XmRowColumnWidget w, XmRowColumnWidget *topLevel)
{
   /*
    * find toplevel by following up the chain. Popups use CascadeBtn to
    * keep the active widget in the postFromList.
    */
   while (RC_CascadeBtn(w) && (!IsPopup(w)))
       w = (XmRowColumnWidget) XtParent(RC_CascadeBtn(w));

   * topLevel = w;
} 

/*
 * set up the grabs on the appropriate assoc widgets.  For a popup, this
 * is all of the widgets on the postFromList.  For a menubar and option
 * menu, this is the top manager widget in their hierarchy.  For a
 * pulldown, the assoc widgets can only be determined by following the
 * chain up the postFromList.
 */
 
static void GrabKeyOnAssocWidgets (rowcol, detail, modifiers)
XmRowColumnWidget rowcol;
KeyCode detail;
unsigned int modifiers;
{
   Widget topManager;
   int i;
   
   if (IsPopup(rowcol))
   {
      for (i=0; i < rowcol->row_column.postFromCount; i++)
	  XtGrabKey(rowcol->row_column.postFromList[i], detail, modifiers,
		    False, GrabModeAsync, GrabModeAsync);
   }
   else if (IsBar(rowcol) || IsOption(rowcol))
   {
      GetTopManager (rowcol, &topManager);
      XtGrabKey(topManager, detail, modifiers,
		False, GrabModeAsync, GrabModeAsync);
   }
   else if (IsPulldown(rowcol))
   {
      for (i=0; i<rowcol->row_column.postFromCount; i++)
	  GrabKeyOnAssocWidgets (XtParent(rowcol->row_column.postFromList[i]),
				 detail, modifiers);
   }
}  
       

/*
 * Given a menupane, this function traverses down through any posted submenus,
 * setting the state of the traversal_on field to the indicated value.
 */

static void SetTraversal (m, traversalOn)
XmRowColumnWidget m;
Boolean traversalOn;
{
   if (m == NULL)
      return;

   if (!IsOption(m))
      m->manager.traversal_on = traversalOn;

   if (XmIsMenuShell(XtParent(m)))
      MS_FocusPolicy(XtParent(m)) = traversalOn ? XmEXPLICIT : XmPOINTER;

   if (RC_PopupPosted(m))
       SetTraversal(((CompositeWidget)RC_PopupPosted(m))->
		    composite.children[0], traversalOn);
}

/*
 * This is the event handler which catches, verifies and dispatches all
 * accelerators and mnemonics defined for a given menu hierarchy.  It
 * is attached to the menu's associated widget, along with an assortment
 * of other widgets.
 */

static void KeyboardInputHandler (reportingWidget, topLevel, event)
Widget reportingWidget;
XmRowColumnWidget topLevel;
XEvent * event;
{
   /* Process the event only if not already processed */
   if (!_XmIsEventUnique(event))
      return;

   if (IsBar(topLevel) || IsOption(topLevel))
       if (! _XmAllWidgetsAccessible(topLevel))
	   return;

   /* 
    * XmGetPostFromWidget() requires help to identify the topLevel widget
    * when a menupane is posted via accelerators.
    */
   if (IsBar(topLevel) || IsOption(topLevel))
      lastSelectToplevel = (Widget) topLevel;
   else if (IsPopup(topLevel))
   {
      lastSelectToplevel = reportingWidget;	/* popup */
   }
   else
      lastSelectToplevel = NULL;
   
   ProcessKey (topLevel, event);

   lastSelectToplevel = NULL;	/* reset toplevel "accelerator" state to NULL */
}

/*
 * try to find a match in the menu for the key event.   Cascade down the
 * submenus if necessary
 */
static Boolean ProcessKey (rowcol, event)
XmRowColumnWidget rowcol;
XEvent * event;
{
   Boolean found = FALSE;
   int i;
   Widget child;

   /* Try to use it on the current rowcol */
   if (! CheckKey (rowcol, event))
   {
      /* not used, try moving down the cascade */
      for (i=0; (i < rowcol->composite.num_children) && (! found); i++)
      {
	 child = rowcol->composite.children[i];

	 /* only check sensitive and managed cascade buttons */
	 if (XtIsSensitive(child) && XtIsManaged(child))
	 {
	    if (XmIsCascadeButtonGadget(child))
	    {
	       if (CBG_Submenu(child))
	       {
		   /* Build the menu cascade for menuHistory */
		   RC_CascadeBtn(CBG_Submenu(child)) = child;
		   found = ProcessKey
		       (((XmCascadeButtonGadget)child)->cascade_button.submenu,
			event);
	       }
	    }
	    else if (XmIsCascadeButton(child))
	    {
	       if (CB_Submenu(child))
	       {
		   RC_CascadeBtn(CB_Submenu(child)) = child;
		   found = ProcessKey
		       (((XmCascadeButtonWidget)child)->cascade_button.submenu,
			event);
	       }
	    }
	 }
      }
      return (found);
   }
   else
       return (True);
}

/*
 * Check if the key event is used in the rowcol
 */
static Boolean CheckKey (rowcol, event)
XmRowColumnWidget rowcol;
XEvent * event;
{
   int menu_index = 0;
   XmKeyboardData * entry;
   ShellWidget shell;
   
   /* Process all matching key events */
   while ((menu_index = _XmMatchInKeyboardList(rowcol, event, menu_index)) != -1)
   {
      entry = MGR_KeyboardList(rowcol) + menu_index;

      /* Ignore this entry if it is not accessible to the user */
      if (XmIsRowColumn(entry->component))
      {
	 /*
	  * Rowcols are not accessible if they are insensitive or
	  * if menubars or optionmenus are unmanaged.
	  */
	 if (! XtIsSensitive(entry->component) ||
	     ((RC_Type(entry->component) != XmMENU_POPUP) &&
	      (RC_Type(entry->component) != XmMENU_PULLDOWN) &&
	      (! XtIsManaged(entry->component))))
	 {
	    menu_index++;
	    continue;
	 }
      }

      /* buttons are not accessible if they are insensitive or unmanaged */
      else if (! XtIsSensitive(entry->component) ||
	       ! XtIsManaged(entry->component))
      {
	 menu_index++;
	 continue;
      }

      /* 
       * For a mnemonic, the associated component must be visible, and
       * it must be in the last menupane posted.
       * This only needs to be checked for a popup or pulldown menu pane.
       */
      if (entry->isMnemonic)
      {
         if ((XmIsLabel(entry->component) || 
              XmIsLabelGadget(entry->component)))
	 {
	    if (IsBar(XtParent(entry->component)) &&
		! RC_PopupPosted(XtParent(entry->component)) &&
		((XmManagerWidget) XtParent(entry->component))->
		     manager.active_child == NULL)
	    {
	      menu_index++;
	      continue;
	    }

	    else if (IsPopup(XtParent(entry->component)) ||
		     IsPulldown(XtParent(entry->component)))
	      {
		/* See if the associated shell is visible */
		shell = (ShellWidget)XtParent(XtParent(entry->component));
		
		/*
		 * Verify the pane is popped up, and the active pane is our 
		 * parent (this is necessary because of shared menupanes.
		 */
		if ((!shell->shell.popped_up) ||
		    (shell->composite.children[0] != 
		     XtParent(entry->component)))
		  {
		    menu_index++;
		    continue;
		  }

		/* Verify we are the last pane */
		if (RC_PopupPosted(shell->composite.children[0]))
		  {
		    menu_index++;
		    continue;
		  }
	      }
	  }
         else if (XmIsRowColumn(entry->component))
         {
	    /*
	     * Ignore the posting mnemonic for an option menu, if its
	     * menupane is already posted.
	     */
	    if (RC_PopupPosted(entry->component))
	    {
	       menu_index++;
	       continue;
	    }
	 }
      }

      /* Perform the action associated with the keyboard event */
      if (XmIsPrimitive(entry->component))
      {
         XmPrimitiveClassRec * prim;

         prim = (XmPrimitiveClassRec *)XtClass(entry->component);
         (*(prim->primitive_class.arm_and_activate)) (entry->component, event);
      }
      else if (XmIsGadget(entry->component))
      {
         XmGadgetClassRec * gadget;

         gadget = (XmGadgetClassRec *)XtClass(entry->component);
         (*(gadget->gadget_class.arm_and_activate)) (entry->component, event);
      }
      else if (XmIsRowColumn(entry->component))
      {
         XmRowColumnClassRec * rc;

         rc = (XmRowColumnClassRec *)XtClass(entry->component);
         (*(rc->row_column_class.armAndActivate)) (entry->component, event);
      }

      /* used the key */
      _XmRecordEvent(event);
      return (True);
   }

   /* did not use the key */
   return (False);
}


static void AddToPostFromList (m, widget)
XmRowColumnWidget m;
Widget widget;
{
   
   if (m->row_column.postFromListSize == m->row_column.postFromCount)
   {
      /* increase the size to fit the new one and one more */
      m->row_column.postFromListSize += 2;
      m->row_column.postFromList = (Widget *)
	  XtRealloc (m->row_column.postFromList,
		     m->row_column.postFromListSize * sizeof(Widget));
   }

   m->row_column.postFromList[m->row_column.postFromCount++] = widget;
}

static void RemoveFromPostFromList (m, widget)
XmRowColumnWidget m;
Widget widget;
{
   int i;
   Boolean found = False;

   for (i=0; i < m->row_column.postFromCount; i++)
   {
      if (!found)
      {
	 if (widget == m->row_column.postFromList[i])
	 {
	    /* remove this entry */
	    found = True;
	 }
      }
      else
	  m->row_column.postFromList[i-1] = m->row_column.postFromList[i];
   }
   if (found)
      m->row_column.postFromCount--;
}


/*
 * This is a class function exported by the RowColumn widget.  It is used
 * by the CascadeButton widget to signal that a menupane has either been
 * attached to a cascade button widget, or detached from a cascade button
 * widget.
 */

static void SetCascadeField (m, cascadeBtn, attach)
XmRowColumnWidget m;
Widget cascadeBtn;
Boolean attach;
{
   int mode;

   if (attach)
   {
      mode = XmADD;

      if (OnPostFromList (m, cascadeBtn) != -1)
	  /* already in the list, this means no work to do */
	  return;

      AddToPostFromList (m, cascadeBtn);
      
      /* if being attached to an option menu, set the option menus submenu */
      if (RC_Type(XtParent(cascadeBtn)) == XmMENU_OPTION)
	  RC_OptionSubMenu(XtParent(cascadeBtn)) = (Widget) m;
   }

   else
   {
      mode = XmDELETE;
      RemoveFromPostFromList (m, cascadeBtn);

      /* if being removed from an option menu, set the option menus submenu */
      if (RC_Type(XtParent(cascadeBtn)) == XmMENU_OPTION)
	  RC_OptionSubMenu(XtParent(cascadeBtn)) = (Widget) NULL;
   }

   /* process the accelerators and mnemonics */
   DoProcessMenuTree(m, mode);

}


/*
 * This function determines if the widget to which a menu is 
 * attached is accessible to the user.  The widget is considered
 * accessible if it, and its ancestors, are both sensitive and
 * managed.  This is useful for MenuBars and Option Menus only.
 */

static Boolean _XmAllWidgetsAccessible (w)
Widget w;
{
   while (w && XtParent(w) && !XtIsShell(w))
   {
      if (!XtIsSensitive(w) || !XtIsManaged(w) || !w->core.mapped_when_managed)
         return (False);

      w = XtParent(w);
   }

   return (True);
}


/*
 * Button Action Procs
 */

static void _XmMenuBtnDown (w, event)
XmRowColumnWidget  w;
XButtonPressedEvent *event;
{
   XmGadget gadget;

   if (! _XmMatchBtnEvent( event, RC_PostEventType(w), RC_PostButton(w),
			  RC_PostModifiers(w))  ||
       ! _XmIsEventUnique(event))
       return;

   /* Overload _XmButtonEventStatus's time for MenuShell's managed_set_changed 
    * routine to determine if an option menu is trying to post using BSelect 
    * Click.  _XmButtonEventStatus's verified is irrelevant.
    */
   if (IsOption(w)) 
   {
      _XmButtonEventStatus.time = event->time;
   }

   /*
    * It's possible that this event has been passed to this widget/window
    * due to a grabbed pointer.  If so, it's probable that it doesn't belong 
    * to any gadget children of the event's window (widget) where the pointer
    * is currently positioned.
    */
   if (w->core.window == event->window)
      gadget = _XmInputInGadget(w, event->x, event->y);
   else
      gadget = NULL;

   if (gadget != NULL)
   {
      _XmDispatchGadgetInput(gadget, event, XmARM_EVENT);
   }
/*
 * We'll popdown the menubar on the button up transition.  Don't set
 * menu traversal off for the armed menubar.  This it to make
 * sure that the up event is used by the menubar and not allowed to
 * accidentally be passed on to a possibly unarmed widget for activation.
 */
   else if (!(IsBar(w) && RC_IsArmed(w)))
       SetMenuTraversal(w, False);
}

static void _XmMenuBtnUp (w, event)
XmRowColumnWidget  w;
XButtonPressedEvent *event;
{
   XmGadget gadget;
   
   if (! _XmIsEventUnique(event) || 
       ! _XmMatchBtnEvent( event, XmIGNORE_EVENTTYPE, RC_PostButton(w),
       RC_PostModifiers(w)) ||
       (IsBar(w) && ! RC_IsArmed(w)))
       return;

   if (w->core.window == event->window)
      gadget = _XmInputInGadget(w, event->x, event->y);
   else
      gadget = NULL;

   if (gadget != NULL)
   {
      _XmDispatchGadgetInput(gadget, event, XmACTIVATE_EVENT);
   }

   else if (IsBar(w))
   {
      /* Only drop in here when no other widget took the event */
      MenuPopDown(w, event);
      MenuBarCleanup(w);
      MenuDisarm(w);
      _XmMenuFocus(w, XmMENU_END, CurrentTime);
      XtUngrabPointer(w, CurrentTime);
   }
}


static void UpdateOptionMenuCBG (cbg, memWidget)
Widget cbg;
Widget memWidget;
{
   char *thing = NULL, *thing_name = NULL;
   int thing_type;
   Boolean freeThing = FALSE;
   Arg al[4];
   int ac = 0;

   if (XmIsLabelGadget(memWidget))
   {
      XmLabelGadget lg = (XmLabelGadget) memWidget;

      if (LabG_IsText (lg))
      {
	 thing = (char *) _XmStringCreateExternal(LabG_Font(lg),
						  LabG__label(lg));
	 thing_type = XmSTRING;
	 thing_name = XmNlabelString;
	 freeThing = True;
      }
      else
      {
	 thing = (char *) LabG_Pixmap(lg);
	 thing_type = XmPIXMAP;
	 thing_name = XmNlabelPixmap;
      }
   }
   else if (XmIsLabel(memWidget))
   {
      XmLabelWidget lw = (XmLabelWidget) memWidget;
      
      if (Lab_IsText (lw))
      {
	 thing = (char *) _XmStringCreateExternal(lw->label.font,
						  lw->label._label);
	 thing_type = XmSTRING;
	 thing_name = XmNlabelString;
	 freeThing = True;
      }
      else
      {
	 thing = (char *)lw->label.pixmap;
	 thing_type = XmPIXMAP;
	 thing_name = XmNlabelPixmap;
      }
   }

   if ( ! IsNull (thing))
   {
      XtSetArg (al[ac], XmNlabelType, thing_type);    ac++;
      XtSetArg (al[ac], thing_name,  thing);      ac++;
      XtSetValues (cbg, al, ac);
   }

   if (freeThing)
       XmStringFree(thing);
}	


static int is_in_widget_list (m, w)
register XmRowColumnWidget m;
RectObj w;
{
    register Widget *q;
    register int i;

    if ((m == NULL) || (w == NULL)) return (FALSE);

    for (i = 0, q = m->composite.children;
         i < m->composite.num_children;
         i++, q++) 

        if ((*q == (Widget) w) && IsManaged (*q)) return (TRUE);

    return (FALSE);
}


static int in_menu (search_m, parent_m, child, w)
XmRowColumnWidget search_m;
XmRowColumnWidget *parent_m;
RectObj child;
Widget *w;
{
    if (is_in_widget_list (search_m, child))
    {
        *parent_m = search_m;
        *w = (Widget) child;
        return (TRUE);
    }

    return (FALSE);
}



static Boolean search_menu (search_m, parent_m, child, w)
XmRowColumnWidget search_m;
XmRowColumnWidget *parent_m;
RectObj child;
Widget *w;
{
    register Widget *q;
    register int i;

    if ( ! in_menu (search_m, parent_m, child, w))
    {
        for (i = 0, q = search_m->composite.children;
             i < search_m->composite.num_children;
             i++, q++) 
        {
            if (XtIsManaged(*q))
            {
                if (XmIsCascadeButtonGadget(*q))
                {
                    XmCascadeButtonGadget p = 
                        (XmCascadeButtonGadget) *q;

                    if (CBG_Submenu(p) &&
                        search_menu (CBG_Submenu(p),parent_m,child,w)) 
                    {
                        return (TRUE);
                    }
                }
                else if (XmIsCascadeButton(*q))
                {
                    XmCascadeButtonWidget p =
                        (XmCascadeButtonWidget) *q;

                    if (CB_Submenu(p) &&
                        search_menu (CB_Submenu(p),parent_m,child,w)) 
                    {
                        return (TRUE);
                    }
                }
            }
        }
        return (FALSE);
    }
    return (TRUE);
}


static void lota_magic (m, child, parent_m, w)
XmRowColumnWidget m;
RectObj child;
XmRowColumnWidget *parent_m;
Widget *w;
{
    *parent_m = NULL;
    *w = NULL;

    search_menu (m, parent_m, child, w);
}

/*
 * called by the buttons to verify that the passed in event is one that
 * should be acted upon.  This is called through the menuProcs handle
 */
static void VerifyMenuButton (w, event, valid)
Widget w;
XEvent * event;
Boolean * valid;
{
   *valid = event &&  _XmMatchBtnEvent( event, XmIGNORE_EVENTTYPE,
			 RC_PostButton(w), RC_PostModifiers(w)); 
}

/*
 * This routine is called at Initialize or SetValues time.  It updates
 * the memory widget in the current rowcolumn and up to the top level(s)
 * menus.  If there is a postFromList on the pulldown, it goes up each
 * branch.  If an option menu is found at the top, then its cascadebutton
 * is updated with the latest stuff.
 */
static void UpdateMenuHistory (menu, child)
XmRowColumnWidget menu;
Widget child;
{
   int i;
   
   RC_MemWidget(menu) = child;

   if (IsOption(menu))
   {
      for (i = 0; i < menu->composite.num_children; i++)
      {
	 if (XmIsCascadeButtonGadget(menu->composite.children[i]))
	 {
	    UpdateOptionMenuCBG (menu->composite.children[i], child);
	    return;
	 }
      }
   }
   else if (IsPulldown(menu))
   {
      for (i=0; i < menu->row_column.postFromCount; i++)
      {
	 UpdateMenuHistory (XtParent(menu->row_column.postFromList[i]),
			    child);
      }
   }
}

/*
 * this is a mess... the menu spec'd is the menu to set the history for;
 * the child spec'd is the child who we are pretending fired-off.   The
 * problem is the child may be in any sub-menu of this cascade.  This is
 * called by Initialize or SetValues.
 */

static void SetMenuHistory (m, child)
XmRowColumnWidget m;
RectObj child;
{
   XmRowColumnWidget parent_m;
   Widget w;

   if (IsNull (child))
       return;

   /* make sure that the child is in the menu hierarchy */
   lota_magic (m, child, &parent_m, &w);

   if (w)
       UpdateMenuHistory (parent_m, w);       
}


static void all_off_except (m, w)
XmRowColumnWidget m;
Widget w;
{
    register Widget *q;
    register int i;

    if (w)  /* then all widgets except this one go off */
    {
        ForManagedChildren (m, i, q)
        {
            if (*q != w)
            {
                if (XmIsToggleButtonGadget(*q))
                {
                   if (XmToggleButtonGadgetGetState (*q))
                        XmToggleButtonGadgetSetState (*q, FALSE, TRUE);
                }
                else if (XmIsToggleButton(*q))
                {
                   if (XmToggleButtonGetState (*q))
                        XmToggleButtonSetState (*q, FALSE, TRUE);
                }
            }
        }
    }
}


static int no_toggles_on (m)
XmRowColumnWidget m;
{
    register Widget *q;
    register int i;

    ForManagedChildren (m, i, q)
    {
        if (XmIsToggleButtonGadget(*q))
        {
            if (XmToggleButtonGadgetGetState (*q)) 
               return (FALSE);
        }
        else if (XmIsToggleButton(*q))
        {
            if (XmToggleButtonGetState (*q)) 
               return (FALSE);
        }
    }

    return (TRUE);
}


/* 
 * note that this is potentially recursive, setting the state of a 
 * toggle in this row column widget may re-enter this routine...
 */

static void RadioBehaviorAndMenuHistory (m, w)
XmRowColumnWidget m;
Widget w;
{
   XmRowColumnWidget menu;
   Widget cb;
   Boolean done = FALSE;
   
   if (! IsManaged(w))
       return;
   
   if (RC_RadioBehavior(m))
   {
      if (XmIsToggleButtonGadget(w))
      {
	 /* turned on */
	 if (XmToggleButtonGadgetGetState (w)) 
	     all_off_except (m, w);

	 /* he turned off */
	 else  
	 {
            if (RC_RadioAlwaysOne(m))
                if (no_toggles_on (m))
		    /* can't allow that */
                    XmToggleButtonGadgetSetState (w, TRUE, TRUE);
	 }
      }
      else if (XmIsToggleButton (w))
      {
	 /* turned on */
	 if (XmToggleButtonGetState (w)) 
	     all_off_except (m, w);

	 /* turned off */
	 else
	 {
            if (RC_RadioAlwaysOne(m))
                if (no_toggles_on (m))  
		    /* can't allow that */
                    XmToggleButtonSetState (w, TRUE, TRUE);
	 }
      }
      
      /* record for posterity */
      RC_MemWidget (m) = w; 
   }

   /* record the mouse memory and history widget all the way up the cascade */
   menu = m;
   while ( ! done)
   {
      RC_MemWidget (menu) = w;
      
      if (! IsPopup(menu) && RC_CascadeBtn(menu))
      {
	cb = RC_CascadeBtn(menu);
	menu = (XmRowColumnWidget) XtParent (cb);
      }

      else
	  done = TRUE;
   }

   /* option menu cascade button gadget must be updated */
   if (IsOption(menu))
       UpdateOptionMenuCBG (cb, w);
}


static char * which_callback (w)
Widget w;
{
    if (XmIsPushButtonGadget(w) || XmIsPushButton(w) || 
        XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w) ||
	XmIsDrawnButton(w))
       return (XmNactivateCallback);

    if (XmIsToggleButtonGadget(w) || XmIsToggleButton(w))
       return (XmNvalueChangedCallback);

    return (NULL);
}

/*
 * This routine is used to emulate the override callback functionality that
 * was available in the R3 library used by Xm and to do the radio behavior
 * and menu history functionality for RowColumns.  The buttons call this
 * function through the MenuProcs interface.
 */

static void ChildsActivateCallback (rowcol, child, call_value)
XmRowColumnWidget rowcol;
Widget child;
Opaque call_value;
{
   Arg arg[1];
   int i;
   XtCallbackList callbacks;
   XmRowColumnWidget topLevel;
   char *c = which_callback (child);       /* which callback to use */

  /* 
   * Save the last selected toplevel in each menupane.
   * "lastSelectToplevel" only set for accelerators via KeyboardInputHandler().
   * Do this before entrycallback because the application may call
   * XmGetPostedFromWidget() from there.
   */
   if (lastSelectToplevel)
      rowcol->row_column.lastSelectToplevel = lastSelectToplevel;
   else
   {
      _XmGetActiveTopLevelMenu (rowcol, &topLevel);
      rowcol->row_column.lastSelectToplevel = (Widget) topLevel;
   }

   if (rowcol->row_column.entry_callback != NULL)
   {
      XtSetArg (arg[0], c, &callbacks);
      XtGetValues (child, arg, 1);

      /* make sure the all of the drawing has been done before the callback */
      XFlush (XtDisplay (rowcol));

      /*
       * cycle through the list and call the entry fired routine for each
       * entry in this callback list, sending in the data for each entry.
       * If the list is NULL, or empty, call the entry fired function once.
       */
      if ((callbacks == NULL) || (callbacks[0].callback == NULL))
	  EntryFired (child, NULL, call_value);

      else
      {
	 for (i=0; callbacks[i].callback != NULL; i++)
	    EntryFired (child, callbacks[i].closure, call_value);
      }
   }
   else
       /* no entry callback, but must do radio behavior & menu history */
       EntryFired (child, NULL, call_value);
}
        
/*
 * This is the callback for widgets which are composited into row column
 * widgets.  It notifies the menu that some individual widget fired off; 
 * this allows * the row column widget to tell the application if it wants 
 * to know.  also to do various other automagical things
 */

static void EntryFired (w, client_data, callback)
Widget   w;                 /* ptr to widget that fired */
caddr_t  client_data;
XmAnyCallbackStruct *callback;
{
    XmRowColumnWidget m = (XmRowColumnWidget) XtParent (w);
    XmRowColumnCallbackStruct mr;

    mr.reason       = XmCR_ACTIVATE;    /* menu activated */
    mr.widget       = w;
    mr.data         = client_data;
    mr.callbackstruct   = (char *) callback;  /* subwidget structure */
    mr.event            = callback->event;

    XtCallCallbackList ((Widget) NULL, m->row_column.entry_callback, &mr);

    RadioBehaviorAndMenuHistory (m, w);
}


/*************************************************************************
 * 
 * this section is all the layout stuff, the whole thing is messy because
 * it has to operate in two different modes, one: a read-only mode which
 * is nice for making decisions about the size of the row column vs. the size
 * of the children.  two: a change everything mode which implements the
 * change.
 *
 * further complicated by the xtoolkit restriction that a subwidget making
 * a geo request (referred to as the 'instigator') of the row column may not 
 * have his resize proc called but all other widget children must.
 *
 * this is done by building a set of XtWidgetGeometry request blocks, one
 * for each child (widget and gadget), which holds the changes we would like to 
 * make for this child.  If needed then another pass is made over the requests 
 * to actually implement the changes.
 */


/*
 * count the widest & tallest entry dimensions
 * and compute entries per row/column
 */

static int get_info (m, w, h, items_per)
XmRowColumnWidget m;
Dimension *w, *h;
int *items_per;
{
    XtWidgetGeometry *b;
    int i, n = 0;;

    *w = *h = 0;

    for (i=0; RC_Boxes (m) [i].kid != NULL; i++)
    {
       b = &(RC_Boxes (m) [i].box);
       n++;

       if (*w < BWidth  (b))
	   *w = BWidth (b);
       if (*h < BHeight (b))
	   *h = BHeight (b);
    }

    *items_per = n / RC_NCol (m);       /* calc column size */

    if ((n % RC_NCol (m)) != 0)         /* some left overs */
        (*items_per)++;             /* add another row/col */
}


/*
 * Make sure that entries in the right most column/row extend all the 
 * way to the right/bottom edge of the row column widget.  This keeps 
 * 'dead space' in the row column widget to a minimum.  For single 
 * column widgets, the only column is the right most.  
 *
 */

static int adjust_last (m, start_i, w, h)
XmRowColumnWidget m;
int start_i;
Dimension w, h;
{
   XmKidGeometry kg = RC_Boxes (m);
   XtWidgetGeometry *b;
   Dimension a;
   register Dimension subtrahend;

   for ( ; kg [start_i].kid != NULL; start_i++)
   {
      b = &(kg[start_i].box);

      if (IsVertical (m))
      {
         subtrahend = MGR_ShadowThickness(m) + RC_MarginW (m) + BX (b)
	     + Double (BBorder (b));

	 /* if w (rowcol width) is greater than subtrahend (the smallest
	  * width of the child, we'll guarantee at least a width of 1.
	  */
	 if (w > subtrahend) 
	     BWidth (b) = w-subtrahend;
      }
      else
      {
         subtrahend =  MGR_ShadowThickness(m) + RC_MarginH (m) + BY (b)
	     + Double (BBorder (b));

	 if (h > subtrahend) 
	     BHeight (b) = h-subtrahend;
      }
   }
}


/*
 * decide exactly the dimensions of the row column widget we will return to 
 * an asking caller based on the accumulated layout information.
 */

static void set_asking (m, m_width, m_height, b, max_x, max_y, x, y, w, h)
XmRowColumnWidget m;
Dimension   *m_width, *m_height;            /* if 0 then caller's asking */
Dimension    b;
Position     max_x, max_y, x, y;
Dimension    w, h;
{
    if (IsVertical (m))             /* tell caller what he wants */
    {
        if (Asking (*m_width)) 
            *m_width =   x + w + b      /* right edge of last child */
                   + MGR_ShadowThickness(m)
                   + RC_MarginW (m);    /* plus margin on right */
    
        if (Asking (*m_height))
            *m_height =   max_y         /* last unused y */
                - RC_Spacing (m)    /* up by unused spacing */
                + MGR_ShadowThickness(m)
                + RC_MarginH (m);   /* plus margin on bottom */
    }
    else
    {
        if (Asking (*m_width)) 
            *m_width = max_x - RC_Spacing (m) 
                + MGR_ShadowThickness(m)
                + RC_MarginW (m);

        if (Asking (*m_height))
            *m_height = y + h + b
                + MGR_ShadowThickness(m)
                + RC_MarginH (m);
    }
}




/*
 * Decide where to put the help child.  He better be the last one
 * 'cuz we may trash the x, y's
 */

static void calc_help (m, m_width, m_height, b, max_x, max_y, x, y, w, h)
XmRowColumnWidget m;
Dimension *m_width, *m_height,            /* if 0 then caller's asking */
           b;
Position    max_x, max_y, *x, *y;
Dimension   w, h;
{
   register Dimension subtrahend;

   if (IsVertical (m))             /* glue to bottom edge of ... */
   {
      if (Asking (*m_height))
      {
	 if (RC_NCol (m) == 1)       /* just use max_y */
	     *y = max_y;
	 else                /* go up from max_y */
	 {
	     subtrahend = RC_Spacing (m) + h + b;
	     *y = (max_y > subtrahend) ? max_y - subtrahend : 0; 
	 }
      }
      else
      {   
	  subtrahend = MGR_ShadowThickness(m) + RC_MarginH (m) + h + b;
	  *y = (*m_height > subtrahend) ? *m_height - subtrahend : 0;
      }
   }
   else                    /* glue to right edge of ... */
   {
      if (Asking (*m_width))
      {
	 if (RC_NCol (m) == 1)
	     *x = max_x;
	 else
	 {
	     subtrahend = RC_Spacing (m) + w + b;
	     *x = (max_x > subtrahend) ? max_x - subtrahend : 0;
	 }
      }
      else
      {
	 subtrahend = MGR_ShadowThickness(m) + RC_MarginW (m) + w + b;
	 *x = (*m_width > subtrahend) ? *m_width - subtrahend : 0;
      }
   }
}


/*
 * figure out where all the children of a column style widget go.  The
 * border widths are already set.  
 *
 * In columnar mode, all heights and widths are identical.  They are the
 * size of the largest item.
 *
 * For vertical widgets the items are laid out in columns, going down the
 * first column and then down the second.  For horizonatal, think of the
 * columns as rows. 
 *
 * By convention incoming row column size can be zero, indicating a request
 * for preferred size, this means lay it out and record the needed size.
 *
 * NOTE: the layout is predicated on the help child being the last one since
 * it messes up the x, y for a following child.
 */

static int layout_column (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
    XmKidGeometry kg = RC_Boxes (m);
    XtWidgetGeometry *bx;
    Position x, y, max_x = 0, max_y = 0;
    int items_per_column,
        child_i   = 0,              /* which child we are doing */
        col_c   = 0,                /* items in col being done */
        start_i = 0;                /* index of first item in col */
    Dimension w, h;
    Dimension b = Double (RC_EntryBorder (m));

    /* loc of first item */
    x   = MGR_ShadowThickness(m) + RC_MarginW (m);
    y   = MGR_ShadowThickness(m) + RC_MarginH (m); 

    get_info (m, &w, &h, &items_per_column);

    for (child_i=0; kg [child_i].kid != NULL; child_i++)
    {
        bx = &(kg[child_i].box);
        if (!RC_EntryBorder(m) && XtIsWidget(kg[child_i].kid))
             b = Double(kg[child_i].kid->core.border_width);

        BWidth  (bx) = w;           /* all have same dimensions */
        BHeight (bx) = h;

        if (++col_c > items_per_column)     /* start a new column */
        {
	   if (IsVertical (m))         /* calc loc of new column */
	   {
	      x += w + b + RC_Spacing (m);    /* to the right */

	      /*back at top of menu */
	      y = MGR_ShadowThickness(m) + RC_MarginH (m);
	   }
	   else                /* calc loc of new row */
	   {
	      /* back to left edge */
	      x = MGR_ShadowThickness(m) + RC_MarginW (m);
	      /* down a row */
	      y += h + b + RC_Spacing (m);
	   }

	   col_c = 1;              /* already doing this one */
	   start_i = child_i;          /* record index */
        }

        if (IsHelp (m, ((Widget) kg[child_i].kid))) 
            calc_help (m, m_width, m_height, b, max_x, max_y, &x, &y, w, h);

        SetPosition (bx, x, y);         /* plunk him down */

        if (IsVertical (m))         /* get ready for next item */
            y += h + b + RC_Spacing (m);
        else
            x += w + b + RC_Spacing (m);

        if (max_y < y)
	    max_y = y;       /* record for use later */
        if (max_x < x)
	    max_x = x;
     }

    set_asking (m, m_width, m_height, b, max_x, max_y, x, y, w, h);
    
    if (RC_AdjLast(m))
        adjust_last (m, start_i, *m_width, *m_height);
}


/*
 * do a vertical tight (non-column) layout.
 *
 * In a tight layout one dimension of the items is left alone and the other
 * is kept uniform.  In a vertical row column widgets, the widths of each child 
 * are uniform for each column, the heights are never changed.  In a horiz 
 * row column widget, the widths are never changed and the heights are kept 
 * uniform for each row.
 *
 * It gets messy w.r.t. the help child because we don't know if there will
 * be room in the last column/row for it.  If there isn't room then a whole
 * new column/row has to be added.
 *
 * NOTE: the layout is predicated on the help child being the last one since
 * it messes up the x, y for a following child.
 */

static int layout_vertical_tight (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
    XmKidGeometry kg = RC_Boxes (m);
    XtWidgetGeometry *bx;
    Position x, y, max_y = 0;
    Dimension h;
    Dimension w = 0;                /* widest item width in col */
    int child_i = 0,
       start_i = 0;                /* index of first item in col */
    Dimension
        b = Double (RC_EntryBorder (m));

    /* first item location */
    x = MGR_ShadowThickness(m) + RC_MarginW (m);
    y = MGR_ShadowThickness(m) + RC_MarginH (m);
        
    for (child_i=0; kg [child_i].kid != NULL; child_i++)
    {
       bx = &(kg[child_i].box);
       if (!RC_EntryBorder(m) && XtIsWidget(kg[child_i].kid))
	   b = Double(kg[child_i].kid->core.border_width);

       h = BHeight (bx) + b;           /* calc this item's height */

       if (((y + h) > *m_height) && 
	   ( ! Asking (*m_height)) &&
	   (child_i))
       {                   /* start new column */
	  while (start_i < child_i)
	      kg[start_i++].box.width = w;    /* set uniform width */

	  x += w + b 
	      + MGR_ShadowThickness(m)
		  + RC_MarginW (m);       /* go right and */
            
	  y = MGR_ShadowThickness(m)
	      + RC_MarginH (m);           /* back to top of menu */
	  w = BWidth (bx);            /* reset for new column */
       }

       if (IsHelp (m, ((Widget) kg[child_i].kid))) 
	   calc_help (m, m_width, m_height, b, 0, max_y, &x, &y, w, h);

       SetPosition (bx, x, y);

       if (w < BWidth (bx))
	   w = BWidth (bx);

       y += h + RC_Spacing (m);        /* loc of next item */

       if (max_y < y)
	   max_y = y;       /* record for use later */
    }

    set_asking (m, m_width, m_height, b, 0, max_y, x, y, w, h);

    if (RC_AdjLast(m))
        adjust_last (m, start_i, *m_width, *m_height);
    else
	while (start_i < child_i)
	    kg[start_i++].box.width = w;    /* set uniform width */
}


static int layout_horizontal_tight (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
   XmKidGeometry kg = RC_Boxes (m);
   XtWidgetGeometry *bx;
   Position x, y, max_x = 0;
   Dimension w;
   Dimension h = 0;                   /* tallest item height in row */
   int child_i = 0,
     start_i = 0;                /* index of first item in row */
   Dimension b = Double (RC_EntryBorder (m));

   /* first item location */
   x = MGR_ShadowThickness(m) + RC_MarginW (m);
   y = MGR_ShadowThickness(m) + RC_MarginH (m);
        
   for (child_i=0; kg [child_i].kid != NULL; child_i++)
   {
      bx = &(kg[child_i].box);
      if (!RC_EntryBorder(m) && XtIsWidget(kg[child_i].kid))
	  b = Double(kg[child_i].kid->core.border_width);

      w = BWidth (bx) + b;            /* item's width */

      if (((x + w) > *m_width) && 
	  ( ! Asking (*m_width)) &&
	  (child_i))
      {                   /* start a new row */
	 while (start_i < child_i)
	     kg[start_i++].box.height = h;   /* set uniform height */

	 x = MGR_ShadowThickness(m) 
	     + RC_MarginW (m);       /* left edge of menu */
	 y += h + b 
	     + MGR_ShadowThickness(m) 
		 + RC_MarginH (m);       /* down to top of next row */
	 h = BHeight (bx);           /* reset for this row */
      }
      
      if (IsHelp (m, ((Widget) kg[child_i].kid))) 
	  calc_help (m, m_width, m_height, b, max_x, 0, &x, &y, w, h);
      
      SetPosition (bx, x, y);

      if (h < BHeight (bx))
	  h = BHeight (bx);

      x += w + RC_Spacing (m);        /* loc of next item */

      if (max_x < x)
	  max_x = x;       /* record for use later */
   }

   set_asking (m, m_width, m_height, b, max_x, 0, x, y, w, h);
   
   if (RC_AdjLast(m))
       adjust_last (m, start_i, *m_width, *m_height);
   else
       while (start_i < child_i)
	   kg[start_i++].box.height = h;   /* set uniform height */
}


static int layout_vertical (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
   if (PackColumn (m))
       layout_column (m, m_width, m_height);
   else
       layout_vertical_tight (m, m_width, m_height);
}


static int layout_horizontal (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
   if (PackColumn (m))
       layout_column (m, m_width, m_height);
   else
       layout_horizontal_tight (m, m_width, m_height);
}

/*
 * wrap a box around the entries, used with packing mode of none.
 *
 * we ignore negative positioning, ie. only worry about being wide enough
 * for the right edge of the rightmost entry (similarly for height)
 */

static int bound_entries (m, m_width, m_height)
XmRowColumnWidget m;
Dimension *m_width, *m_height;            /* if 0 then caller's asking */
{
    XtWidgetGeometry *b;
    XmKidGeometry kg = RC_Boxes (m);
    int i = 0;
    Dimension w, max_w = 0, max_h = 0;
    Dimension bw = Double(RC_EntryBorder(m));

    for (i=0; kg [i].kid != NULL; i++)
    {
        b = &(kg[i].box);
        if (!RC_EntryBorder(m) && XtIsWidget(kg[i].kid))
             bw = Double(kg[i].kid->core.border_width);

        if (Asking (*m_width))
        {
            w = BWidth (b) + bw + BX (b);
            if (w > max_w)
		max_w = w;
        }

        if (Asking (*m_height))
        {
            w = BHeight (b) + Double (bw) + BY (b);
            if (w > max_h)
		max_h = w;
        }
    }

    if (Asking (*m_width))
	*m_width  = max_w;
    if (Asking (*m_height))
	*m_height = max_h;
}

/*

   This routine (option_layout) does the layout for option menus.
   This layout is a tiled layout with the option label widget to the
   left of the option button widget.

*/
/* ARGSUSED */
static void option_layout (menu, width, height)
XmRowColumnWidget menu;
Dimension *width, *height;
{
   Dimension h;
   XmKidGeometry kg;
   XtWidgetGeometry    *label_box, *button_box;
   register int i; 
   register Dimension c_width=0; 
   register Dimension c_height=0;
   register XmRowColumnWidget p = (XmRowColumnWidget) RC_OptionSubMenu(menu);
   Widget cb = XmOptionButtonGadget(menu);

   /* Find the interesting boxes */

   kg = RC_Boxes(menu);
   label_box = &(kg[0].box);
   button_box = &(kg[1].box);

   if (p && p->composite.num_children)
   {
      for(i=0; i < p->composite.num_children; i++)
	  if (XtIsManaged(p->composite.children[i]))
	  {
	     if ((p->composite.children[i])->core.width > c_width)
		 c_width = (p->composite.children[i])->core.width;
	     if ((p->composite.children[i])->core.height > c_height)
		 c_height = (p->composite.children[i])->core.height;
	  }
      /*
       * make sure the width of the cascade button will hold the longest
       * label plus the cascade pixmap.
       */
      BWidth(button_box) = c_width + Double(G_HighlightThickness(cb))
	  + LabG_MarginRight(cb);
      BHeight(button_box) = c_height + Double(G_HighlightThickness(cb));
   }
   else
   {
      /* Option menu draws a toggle indicator with a childless submenu */
      if (p && !p->composite.num_children && IsOption(menu))
	 BWidth(button_box) = LabG_TextRect(cb).width +
	         LabG_MarginRight(cb) + LabG_MarginLeft(cb) +
	         (2*(LabG_MarginWidth(cb) + 
		    G_ShadowThickness(cb) + G_HighlightThickness(cb)));
      else
	 BWidth(button_box) = XtWidth(cb);
      BHeight(button_box) = XtHeight(cb);
   }

   /* Set the height to the highest of the two */

   h = MAX(BHeight(label_box),BHeight(button_box));
   BHeight(label_box) = h;
   BHeight(button_box) = h;

   /* The label box is placed at... */

   BX(label_box) = RC_MarginW(menu);
   BY(label_box) = RC_MarginH(menu);

   /* The button is placed just next to the label */

   BX(button_box) = BX(label_box) + BWidth(label_box) + RC_Spacing(menu);
   BY(button_box) = RC_MarginH(menu);
}

static void Layout (m, w, h)
XmRowColumnWidget m;
Dimension *w, *h;
{
   if (IsOption(m))
       option_layout(m,w,h);
   else if (IsVertical (m))
       layout_vertical (m, w, h);
   else
       layout_horizontal (m, w, h);
}

   
/**************************************************************************
 *
 * class support procedures
 */

/*
 * position then row column widget where it wants to be; normally, this
 * is used only for popup or pulldown menupanes.
 */

static void PositionMenu (m, event)
register XmRowColumnWidget  m;
XButtonPressedEvent *event;
{
    XmRowColumnWidget root = NULL;
    XmCascadeButtonWidget p;

    if (m == NULL) 
       return;

    switch (m->row_column.type)
    {
        case XmMENU_OPTION:
        case XmMENU_BAR:
        case XmWORK_AREA:
            break;

        /*
         * remaining cases take advantage of the fact that positioning of
         * a popup shells' only child is not normal.  Just change the widget
         * itself, when popped up the shell will put itself at this location
         * and move the child to 0,0.  Saves a bunch of thrashing about.
         */

        case XmMENU_POPUP:          /* position near mouse */
            XtX (m) = event->x_root;
            XtY (m) = event->y_root;
            RC_SetWidgetMoved (m, TRUE);

	    /* popups keep active widget in postFromLIst in cascadeBtn field */
	    RC_CascadeBtn(m) = XtWindowToWidget (XtDisplay(m),
						 event->window);
            break;

        case XmMENU_PULLDOWN:
            p = (XmCascadeButtonWidget) m->row_column.cascadeBtn;

            if (p != NULL) 
               root = (XmRowColumnWidget) XtParent (p);
            else
               return;

            if (! XmIsRowColumn(root)) 
               root = NULL;

            LocatePulldown(
                root,           /* menu above */
                p,              /* pulldown linking the two */
                m,              /* menu to position */
                event);         /* event causing pulldown */

            if ((XtX (m) == XtX (XtParent(m))) &&
                (XtY (m) == XtY (XtParent(m))))
            {
               XtX (m) = 0;
               XtY (m) = 0;
            }
            else
               RC_SetWidgetMoved (m, TRUE);

            break;
    }
}

static Widget find_first_managed_child(m, first_button)
CompositeWidget m;
Boolean first_button;
{
    register Widget *kid = m->composite.children;
    register int i = 0;
    register int n = m->composite.num_children;

    while( (i < n) && 
	   (!XtIsManaged(*kid) || 
	   (first_button && 
	    !(XmIsPushButton(*kid) ||
	      XmIsPushButtonGadget(*kid) ||
	      XmIsToggleButton(*kid) ||
	      XmIsToggleButtonGadget(*kid)) 
	   )) )
        kid++, i++;

    if (i >= n)
        return(NULL);
    else
        return(*kid);
}

/*
 * called by cascadebutton (or CBG) before cascading a menu.  The interface
 * is through the menuProcs handle.
 */
static void PrepareToCascade (cb, submenu, event)
Widget cb;
XmRowColumnWidget submenu;
XEvent * event;
{
   RC_CascadeBtn(submenu) = cb;
   RC_PostButton(submenu) = RC_PostButton(XtParent(cb));
   RC_PostModifiers(submenu) = RC_PostModifiers(XtParent(cb));
   RC_PostEventType(submenu) = RC_PostEventType(XtParent(cb));
   RC_PopupPosted(XtParent(cb)) = XtParent(submenu);

   if (IsOption(XtParent(cb)))
       RC_MemWidget(submenu) = RC_MemWidget(XtParent(cb));
   
   PositionMenu (submenu, event);

   submenu->manager.traversal_on =
       ((XmManagerWidget)XtParent(cb))->manager.traversal_on;

   if (!submenu->manager.traversal_on)
       submenu->manager.active_child = NULL;

   MS_FocusPolicy(XtParent(submenu)) =
       submenu->manager.traversal_on ? XmEXPLICIT : XmPOINTER;
}

/* ARGSUSED */
static void LocatePulldown (root, p, m, event)
XmRowColumnWidget root;       /* menu holding pulldown */
XmCascadeButtonWidget p;      /* cascadebutton doing the pulling */
XmRowColumnWidget m;          /* sub-menu to position */
{   
    Position x, y, x1, y1;

    if (root == NULL) 
      return;

    x = y = 0;                  /* punt, don't know */

    if (IsOption (root))           /* near hot spot */
    {                              /* of option button (p) */
        if (! XtIsRealized (m))  
           XtRealizeWidget (m);

        x = 0;
        y = RC_MemWidget(m) ? (Half (XtHeight (p)) - (XtY(RC_MemWidget(m)) +
            Half(XtHeight(RC_MemWidget(m))))) : XtY(p);
    }   

    else if (IsBar (root))  /* aligned with left edge of */
    {                       /* cascade button, below cascade button */
        x = 0;
        y = XtHeight (p);
    }

    else if (XmIsCascadeButtonGadget(p) && CBG_HasCascade(p))
    {
        Widget first;
        first = find_first_managed_child(m, ANY_CHILD);

	/* gadget; have to use parent as base for coordinates */
        x = XtX(p) + CBG_Cascade_x(p) + CBG_Cascade_width(p);
        y = XtY(p) + CBG_Cascade_y(p) + Half(CBG_Cascade_height(p))
            - (first ? (XtY(first) + Half(XtHeight(first))) : 0);

	/* cast only to eliminate warning */
	p = (XmCascadeButtonWidget)XtParent(p);

    }

    else if (XmIsCascadeButton(p) && CB_HasCascade(p))
    {
        Widget first;
        first = find_first_managed_child(m, ANY_CHILD);

        x = CB_Cascade_x(p) + CB_Cascade_width(p);
        y = CB_Cascade_y(p) + Half(CB_Cascade_height(p))
            - (first ? (XtY(first) + Half(XtHeight(first))) : 0);
    }

    /*
     * Take these co-ords which are in the cascade button 
     * widget's co-ord system and convert them to the root 
     * window co-ords.
     */
    XtTranslateCoords(p, x, y, &x1, &y1);

    XtX (m) = x1;
    XtY (m) = y1;
}


/*
 * lay out the row column widget to find it's best size, this is a 
 * read-only operation
 */

/* ARGSUSED */
static void think_about_option_size(m, w, h, instigator, request)
register XmRowColumnWidget m;
Dimension *w, *h;
Widget instigator;
XtWidgetGeometry *request;  
{
   register int i; 
   Dimension width=0, height=0;
   register XmRowColumnWidget p = (XmRowColumnWidget) RC_OptionSubMenu(m);
   Widget cb = XmOptionButtonGadget(m);

   /*
    * The box list was built in the order that the children were
    * created, so the label is first and the cascade button is second
    * (any other children are erroneous).
    */
    
   RC_Boxes(m)[0].box.x = RC_MarginW(m);
   RC_Boxes(m)[0].box.y = RC_MarginH(m);
   *w = RC_MarginW(m) + RC_Boxes(m)[0].box.width + RC_Spacing(m);
   *h = RC_Boxes(m)[0].box.height;

   /*
    * Cascade Button should really prefer to be the size neccessary to
    * display the submenu data, but that's not implemented yet so we
    * just have to kludge a bit.
    */

   if (p && p->composite.num_children)
   {
      for(i=0; i < p->composite.num_children; i++)
      {
         if (XtIsManaged(p->composite.children[i]))
         {
            if ((p->composite.children[i])->core.width > width)
		width = (p->composite.children[i])->core.width;
	    if ((p->composite.children[i])->core.height >height)
		height =(p->composite.children[i])->core.height;
         }
      }

      /*
       * make sure the width of the cascade button will hold the longest
       * label plus the cascade pixmap.
       */
      width += LabG_MarginRight(cb);
   }
   else
   {
      /* Option menu draws a toggle indicator with a childless submenu */
      if (p && !p->composite.num_children && IsOption(m))
	 width = LabG_TextRect(cb).width +
	         LabG_MarginRight(cb) + LabG_MarginLeft(cb) + 
	         (2*(LabG_MarginWidth(cb) +
		    G_ShadowThickness(cb)));
      else
         width = RC_Boxes(m)[1].box.width;
      height = RC_Boxes(m)[1].box.height;
   }

   height += Double(G_HighlightThickness(cb));

   RC_Boxes(m)[1].box.x = *w;
   RC_Boxes(m)[1].box.y = RC_MarginH(m);
   RC_Boxes(m)[1].box.width = width;
   RC_Boxes(m)[1].box.height = height;

   *w += width + RC_MarginW(m) + Double(G_HighlightThickness(cb));
       

   if (height > *h)
   {
      RC_Boxes(m)[0].box.height = height;
      *h = height;
   }

   *h += Double(RC_MarginH(m));
}


static void think_about_size (m, w, h, instigator, request)
register XmRowColumnWidget m;
Dimension *w, *h;
Widget instigator;
XtWidgetGeometry *request;  
{
    if (IsOption(m))
    {
        think_about_option_size(m, w, h, instigator, request);
        return;
    }

    if (PackNone (m))
        bound_entries (m, w, h);
    else
        Layout (m, w, h);

    if (!RC_ResizeWidth(m))  *w = XtWidth  (m);
    if (!RC_ResizeHeight(m)) *h = XtHeight (m);

    if (!RC_ResizeHeight(m) && !RC_ResizeWidth(m))
        return;

    if (*w < resource_min_width)
	*w = resource_min_width;
    if (*h <  resource_min_height)
	*h = resource_min_height;
}

static void PreferredSize (m, w, h)
XmRowColumnWidget m;
Dimension *w, *h;
{
   /*
    * get array built for both widgets and gadgets layout is based only on 
    * this array, adjust width margins &  adjust height margins
    */
   RC_Boxes(m)=
       (XmKidGeometry)_XmGetKidGeo (m, NULL, NULL, RC_EntryBorder(m),
				    RC_EntryBorder (m),
				    (IsVertical (m) && RC_DoMarginAdjust (m)),
				    (IsHorizontal (m) &&
				     RC_DoMarginAdjust (m)),
				    RC_HelpPb (m),
				    XmGET_PREFERRED_SIZE);

   think_about_size (m, w, h, NULL, NULL);

   XtFree (RC_Boxes(m));
}



static XtGeometryResult QueryGeometry (m, intended, reply)
XmRowColumnWidget m;            /* me */
XtWidgetGeometry *intended, *reply;
{
    Dimension w, h;

    if (GMode (intended) & ( ~ (CWWidth | CWHeight)))
        return (XtGeometryYes);

    /*
     * pre-load the reply with input values or flag to preferred_size proc
     * that we are asking for a value
     */

    if ((GMode (intended) & CWWidth) &&
        !(GMode (intended) & CWHeight))
    {
        w = intended->width;
        h = 0;

        PreferredSize (m, &w, &h);

        GMode(reply) = (CWHeight | CWWidth);
        reply->width = w;
        reply->height = h;

        return(XtGeometryAlmost);
    }
    else if ((GMode (intended) & CWHeight) &&
        !(GMode (intended) & CWWidth))
    {
        w = 0;
        h = intended->width;

        PreferredSize (m, &w, &h);

        GMode(reply) = (CWHeight | CWWidth);
        reply->width = w;
        reply->height = h;

        return(XtGeometryAlmost);
    }
    else /* both width and height */
    {
        w = 0;
        h = 0;

        PreferredSize (m, &w, &h);

        if ((w == m->core.width) && (h == m->core.height))
        {
            if (GMode(intended) == (CWWidth | CWHeight))
                return(XtGeometryNo);
            else
            {
                GMode(reply) = (CWHeight | CWWidth);
                reply->width = w;
                reply->height = h;
                return(XtGeometryAlmost);
            }
        }
        else if ((w == intended->width) && (h == intended->height))
            return(XtGeometryYes);
        else
        {
            GMode(reply) = (CWHeight | CWWidth);
            reply->width = w;
            reply->height = h;
            return(XtGeometryAlmost);
        }
    }
}


/*
 * Layout the row column widget to fit it's current size; ignore possible 
 * non-fitting of the entries into a too small row column widget.
 *
 * Don't forget the instigator,
 */
  
static void AdaptToSize (m, instigator, request)
XmRowColumnWidget m;
Widget instigator;
XtWidgetGeometry *request;
{
   Dimension w = XtWidth (m);
   Dimension h = XtHeight (m);

   /*
    * get array built for both widgets and gadgets,
    * layout is based only on this array,
    * adjust width margins and  adjust height margins
    */
   RC_Boxes(m) =
       (XmKidGeometry)_XmGetKidGeo (m, instigator, request, RC_EntryBorder(m),
				    RC_EntryBorder (m),
				    (IsVertical (m) && RC_DoMarginAdjust (m)),
				    (IsHorizontal (m) &&
				     RC_DoMarginAdjust (m)),
				    RC_HelpPb (m),
				    (IsOption(m) ? XmGET_ACTUAL_SIZE :
				     XmGET_PREFERRED_SIZE));

   if (!PackNone (m)) 
       Layout (m, &w, &h);
   
   _XmSetKidGeo (RC_Boxes(m), instigator);
   
   XtFree (RC_Boxes(m));
}


/*
 * Resize the row column widget, and any subwidgets there are.
 * Since the gravity is set to NW, handle shrinking when there may not
 * be a redisplay.
 */

static void Resize(m)
XmRowColumnWidget m;
{
   Boolean		draw_shadow = False;

   /* clear the shadow if its needed (will check if its now larger) */
   _XmClearShadowType (m, m->row_column.old_width,
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

   AdaptToSize (m, NULL, NULL);

   if (draw_shadow && XtIsRealized (m) && m->manager.shadow_thickness )
       /* pop-out not pop-in */
       _XmDrawShadow (XtDisplay (m), XtWindow (m),
		      m->manager.top_shadow_GC,
		      m->manager.bottom_shadow_GC,
		      m->manager.shadow_thickness,
		      0, 0, m->core.width, m->core.height);
}


/*
 * class Redisplay proc 
 */

static void Redisplay (m, event, region)
XmRowColumnWidget m;
XEvent *event;
Region region;
{
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

        _XmRedisplayGadgets(m,event,region);

        if (IsPopup (m) || IsPulldown (m) || IsBar(m))
        {
            if (MGR_ShadowThickness(m))
                _XmDrawShadow (XtDisplay (m), XtWindow (m),
                    /* pop-out not pop-in */
                    m->manager.top_shadow_GC,
                    m->manager.bottom_shadow_GC,
                    m->manager.shadow_thickness,
                    0, 0, m->core.width, m->core.height);
        }
    }

    RC_SetExpose (m, TRUE);
}


/*
 * Geometry manager for subwidgets of a row column widget; be accomdating, 
 * try to say yes, and then deal with our parent's geometry mgr.
 *
 * I'm not even going to try to figure out what to do with an Almost
 * response from the row column widget's parent; just take it as a 'No'.
 */

static XtGeometryResult GeometryManager (instigator, desired, allowed)
Widget  instigator;
XtWidgetGeometry *desired, *allowed;
{
   XmRowColumnWidget m = (XmRowColumnWidget) XtParent(instigator);
   Dimension w = 0;
   Dimension h = 0;
   XtGeometryResult result = XtGeometryYes;

   /*
    * find out what size we need to be with this new widget
    */
   RC_Boxes(m) = (XmKidGeometry)
       _XmGetKidGeo (m, instigator, desired,
		     RC_EntryBorder(m), RC_EntryBorder (m),
		     (IsVertical (m) && RC_DoMarginAdjust (m)),
		     (IsHorizontal (m) && RC_DoMarginAdjust (m)),
		     RC_HelpPb (m), XmGET_PREFERRED_SIZE);

   think_about_size(m,&w,&h,instigator,desired);

   if (IsOption(m))
   {
      if (instigator == XmOptionButtonGadget(m))
      {
	 /* 
	  * Grow only
	  */
         if ((desired->request_mode & CWWidth) &&
	     (desired->width < RC_Boxes(m)[1].box.width))
         {
	    allowed->width = RC_Boxes(m)[1].box.width;
	    allowed->height = RC_Boxes(m)[1].box.height;
	    allowed->request_mode = (CWHeight | CWWidth);
	    result = XtGeometryAlmost;
         }

         if ((desired->request_mode & CWHeight) &&
	     (desired->height < RC_Boxes(m)[1].box.height))
         {
	    allowed->width = RC_Boxes(m)[1].box.width;
	    allowed->height = RC_Boxes(m)[1].box.height;
	    allowed->request_mode = (CWHeight | CWWidth);
	    result = XtGeometryAlmost;
         }

         if (result != XtGeometryYes)
         {
            XtFree(RC_Boxes(m));
            return(result);
         }
      }

      if (instigator == XmOptionLabelGadget(m))
      {
	 /*
	  * Can't get shorter
	  */
         if ((desired->request_mode & CWHeight) &&
	     (desired->height < RC_Boxes(m)[0].box.height))
         {
	    allowed->width = RC_Boxes(m)[0].box.width;
	    allowed->height = RC_Boxes(m)[0].box.height;
	    allowed->request_mode = (CWHeight | CWWidth);
	    result = XtGeometryAlmost;
         }

         if (result != XtGeometryYes)
         {
            XtFree(RC_Boxes(m));
            return(result);
         }
      }
   }

   /*
    * now decide if the menu needs to change size.
    */

   XtFree(RC_Boxes(m));
   
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

      result = XtMakeGeometryRequest(m,&menu_desired,&menu_allowed);
      
      switch (result)
      {
       case XtGeometryAlmost:
       case XtGeometryNo:          /* give it up, don't change */
	 return (XtGeometryNo);      /* us or our children */
       default: /* fall out */
	 break;
      }
   }

   AdaptToSize(m,instigator,desired);

   /*	Clear shadow if necessary. */
   
   if (m->row_column.old_shadow_thickness)
       _XmClearShadowType (m, m->row_column.old_width, 
			   m->row_column.old_height, 
			   m->row_column.old_shadow_thickness, 0);

   m->row_column.old_width = m->core.width;
   m->row_column.old_height = m->core.height;
   m->row_column.old_shadow_thickness = m->manager.shadow_thickness;

   return(XtGeometryDone);
}


/*
 * fix the visual attributes of the subwidget to be what we like
 *
 *  1.  make border width uniform
 *  2.  maybe set the label alignment
 */

static void FixVisual (m, w)
XmRowColumnWidget m;
Widget    w;
{
   Arg al[10];
   int ac;
   
   if (RC_EntryBorder(m))
   {
      _XmConfigureObject(w,w->core.x,w->core.y, 
			 w->core.width, w->core.height, RC_EntryBorder(m));
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

static void FixCallback (m, w)
XmRowColumnWidget  m;
Widget    w;
{
   char *c = which_callback (w);       /* which callback to use */

   if (c == NULL)
       return;          /* can't do it to this guy */

   if (m->row_column.entry_callback)
   {
      /* 
       * Disable the buttons activate callbacks
       */
      if (XmIsLabelGadget(w))
	  (*(((XmLabelGadgetClassRec *)XtClass(w))->
	     label_class.setOverrideCallback)) (w);
      else
	  (*(((XmLabelClassRec *)XtClass(w))->
	     label_class.setOverrideCallback)) (w);
   }
}


static void XmGetMenuKidMargins (m, width, height, left, right, top, bottom)
XmRowColumnWidget m;
Dimension *width, *height, *left, *right, *top, *bottom;
{
   register int i;

    *width = *height = *left = *right = *top = *bottom = 0;

    for (i = 0; i < m->composite.num_children; i++)
    {
        Widget p = (Widget) m->composite.children[i]; 

        if (IsManaged (p))
        {
            if (XmIsLabelGadget(p))
            {
                register XmLabelGadget lg = (XmLabelGadget) p;

                if (LabG_MarginWidth (lg) > *width)  
                       *width  = LabG_MarginWidth  (lg);

                if (LabG_MarginHeight(lg) > *height) 
                       *height = LabG_MarginHeight (lg);

                if (LabG_MarginLeft  (lg) > *left)   
                       *left   = LabG_MarginLeft   (lg);

                if (LabG_MarginRight (lg) > *right)  
                       *right  = LabG_MarginRight  (lg);

                if (LabG_MarginTop   (lg) > *top)    
                       *top = LabG_MarginTop (lg);

                if (LabG_MarginBottom(lg) > *bottom) 
                       *bottom = LabG_MarginBottom (lg);
            }
            else if (XmIsLabel(p))
            {
                register XmLabelWidget lw = (XmLabelWidget) p;

                if (Lab_MarginWidth (lw) > *width)  
                       *width  = Lab_MarginWidth  (lw);

                if (Lab_MarginHeight(lw) > *height) 
                       *height = Lab_MarginHeight (lw);

                if (Lab_MarginLeft  (lw) > *left)   
                       *left   = Lab_MarginLeft   (lw);

                if (Lab_MarginRight (lw) > *right)  
                       *right  = Lab_MarginRight  (lw);

                if (Lab_MarginTop   (lw) > *top)    
                       *top = Lab_MarginTop (lw);

                if (Lab_MarginBottom(lw) > *bottom) 
                       *bottom = Lab_MarginBottom (lw);
            }
        }
    }
}

/*
 * Toggle buttons have this thingy hanging off the left of the
 * widget, before the text.  This dimension is known as the MarginLeft.
 * Pulldown's have hot spots in the MarginRight, accelerators go in the
 * marginRight also.
 *
 * For generality's sake we should insure that all
 * of the current label subclass widgets in the menu have the 
 * margins set to the same value.  
 */

static void DoMarginAdjustment (m)
XmRowColumnWidget m;
{
    register Widget *p;
    register int i; 
    Dimension m_w, m_h, m_l, m_r, m_t, m_b;
    Dimension w, h;

    if ((!RC_DoMarginAdjust (m)) || (IsOption(m)))
       return;

    /*
     * this should almost be part
     * of the layout process, except this requires a setvalue not a resize...
     */

    XmGetMenuKidMargins (m, &m_w, &m_h, &m_l, &m_r, &m_t, &m_b);

    ForManagedChildren (m, i, p)
    {
        if (XmIsLabelGadget(*p))
        {
            XmLabelGadget q;
            /*
             * If in a popup or pulldown pane,
             * don't do labels; i.e. only do buttons.
             */
            if (((*p)->core.widget_class == xmLabelGadgetClass) &&
                (IsPulldown(m) || IsPopup(m)))
                continue;

            w = XtWidth  (*p);
            h = XtHeight (*p);

            q = (XmLabelGadget) (*p);

            if (IsVertical (m)) 
            {
	       /* change horiz margins to  be uniform */
	       if (LabG_MarginLeft(q) != m_l)
	       {
		  w += LabG_MarginLeft(q) - m_l;
		  _XmAssignLabG_MarginLeft(q, m_l);
	       }

	       if (LabG_MarginRight(q) != m_r)
	       {
		  w += LabG_MarginRight(q) - m_r;
		  _XmAssignLabG_MarginRight(q, m_r);
	       }

	       if (LabG_MarginWidth(q) != m_w)
	       {
		  w += LabG_MarginWidth(q) - m_w;
		  _XmAssignLabG_MarginWidth(q, m_w);
	       }
	       
	       _XmReCacheLabG(q);

	       if (q->rectangle.width != w) 
	       {
		  _XmConfigureObject(q,q->rectangle.x,q->rectangle.y,
				     w, q->rectangle.height,
				     q->rectangle.border_width);
	       }
            }
            else 
            {
	       /* change vert margins */
	       if (LabG_MarginTop(q) != m_t)
	       {
		  h += LabG_MarginTop(q) - m_t;
		  _XmAssignLabG_MarginTop(q, m_t);
	       }
	       
	       if (LabG_MarginBottom(q) != m_b)
	       {
		  h += LabG_MarginBottom(q) - m_b;
		  _XmAssignLabG_MarginBottom(q, m_b);
	       }
	       
	       if (LabG_MarginHeight(q) != m_h)
	       {
		  h += LabG_MarginHeight(q) - m_h;
		  _XmAssignLabG_MarginHeight(q, m_h);
	       }
	       
	       _XmReCacheLabG(q);

	       if (q->rectangle.width != h) 
	       {
		  _XmConfigureObject(q,q->rectangle.x,q->rectangle.y,
				     q->rectangle.width, h,
				     q->rectangle.border_width);
	       }
            }
	 }
        else if (XmIsLabel(*p))
        {
            XmLabelWidget lw;
            /*
             * If in a popup or pulldown pane,
             * don't do labels; i.e. only do buttons.
             */
            if (((*p)->core.widget_class == xmLabelWidgetClass) &&
                (IsPulldown(m) || IsPopup(m)))
                continue;

            w = XtWidth  (*p);
            h = XtHeight (*p);

            lw = (XmLabelWidget) (*p);

            if (IsVertical (m)) /* change horiz margins to */
            {                   /* be uniform */
               ChangeMargin (Lab_MarginLeft  (lw), m_l, w);
               ChangeMargin (Lab_MarginRight (lw), m_r, w);
               ChangeMargin (Lab_MarginWidth (lw), m_w, w);

                if (XtWidth (lw) != w) 
                {
                    _XmConfigureObject(lw,lw->core.x,lw->core.y,
                        w, lw->core.height,
                        lw->core.border_width);
                }
            }
            else            /* change vert margins */
            {
                ChangeMargin (Lab_MarginTop (lw), m_t, h);
                ChangeMargin (Lab_MarginBottom (lw), m_b, h);
                ChangeMargin (Lab_MarginHeight (lw), m_h, h);

                if (XtHeight (lw) != h) 
                {
                    _XmConfigureObject(lw,lw->core.x,lw->core.y,
                        lw->core.width, h,
                        lw->core.border_width);
                }
            }
        }
    }
}


/*
 * Action routines specific to traversal.
 */

/* ARGSUSED */
static void _XmMenuUnmap (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   /*
    * Typically, when a cascade button is in a transient menupane, we just
    * want to ignore unmap requests.  However, when it is in a menubar, we
    * want it handled normally.
    */
   if (RC_Type(XtParent(cb)) == XmMENU_BAR)
      _XmPrimitiveUnmap(cb, event);
}



/* ARGSUSED */
static void _XmMenuFocusOut (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
    /*
     * During traversal, when the focus is moved to the next menupane,
     * the cascade button to which the menupane is attached will get a
     * FocusOut event, thus causing it to unhighlight.  To prevent this,
     * we will discard the FocusOut event if we are moving to a new submenu.
     */
    if (!ShouldDispatchFocusOut(cb))
        return;

    /* Forward the event for normal processing */
    _XmPrimitiveFocusOut(cb, event);
}



/*
 * The normal primitive FocusIn procedure does nothing if a transient
 * operation is happening.  Since we are part of the transient operation,
 * and we still want focus to work, we need to use the internal function.
 */
/* ARGSUSED */
static void _XmMenuFocusIn (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   /* Forward the event for normal processing */
   _XmPrimitiveFocusInInternal(cb, event);
}



/* ARGSUSED */
static void Noop (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   /*
    * Do nothing; the purpose is to override the actions taken by the
    * Primitive translations.
    */
}


static void MenuTraverse (w, event, direction)
XmCascadeButtonWidget w;
XEvent * event;
int     direction;
{
   XmManagerWidget parent;

   /*
    * The case may occur where the reporting widget is in fact the
    * RowColumn widget, and not a child.  This will occur if the
    * RowColumn has not traversable children.
    */
   if (XmIsRowColumn(w))
      parent = (XmManagerWidget) w;
   else if (XmIsRowColumn(XtParent(w)))
      parent = (XmManagerWidget) XtParent(w);
   else
      return;

   if ((RC_Type(parent) == XmMENU_POPUP) || 
       (RC_Type(parent) == XmMENU_PULLDOWN) ||
       (RC_Type(parent) == XmMENU_BAR))
   {
      _XmRecordEvent(event);
      (*(((XmRowColumnWidgetClass)XtClass(parent))->row_column_class.
          traversalHandler))(parent, w, direction);
   }
}


/* ARGSUSED */
static void _XmMenuTraverseLeft (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmRowColumnWidget rc;

   if (_XmIsEventUnique(event))
   {
      if (XmIsRowColumn(rc = (XmRowColumnWidget)XtParent(cb)) && 
	  !IsBar(rc) &&
	  (rc->row_column.orientation == XmHORIZONTAL))
	 MenuTraverse(cb, event, XmTRAVERSE_UP);
      else
	 MenuTraverse(cb, event, XmTRAVERSE_LEFT);
   }
}



/* ARGSUSED */
static void _XmMenuTraverseRight (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmRowColumnWidget rc;

   if (_XmIsEventUnique(event))
   {
      if (XmIsRowColumn(rc = (XmRowColumnWidget)XtParent(cb)) && 
	  !IsBar(rc) &&
	  (rc->row_column.orientation == XmHORIZONTAL))
	 MenuTraverse(cb, event, XmTRAVERSE_DOWN);
      else
	 MenuTraverse(cb, event, XmTRAVERSE_RIGHT);
   }
}



/* ARGSUSED */
static void _XmMenuTraverseUp (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmRowColumnWidget rc;

   if (_XmIsEventUnique(event))
   {
      if (XmIsRowColumn(rc = (XmRowColumnWidget)XtParent(cb)) && 
	  !IsBar(rc) &&
	  (rc->row_column.orientation == XmHORIZONTAL))
	 MenuTraverse(cb, event, XmTRAVERSE_LEFT);
      else
	 MenuTraverse(cb, event, XmTRAVERSE_UP);
   }
}



/* ARGSUSED */
static void _XmMenuTraverseDown (cb, event, param, num_param)
XmCascadeButtonWidget cb;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmRowColumnWidget rc;

   if (_XmIsEventUnique(event))
   {
      if (XmIsRowColumn(rc = (XmRowColumnWidget)XtParent(cb)) && 
	  !IsBar(rc) &&
	  (rc->row_column.orientation == XmHORIZONTAL))
	 MenuTraverse(cb, event, XmTRAVERSE_RIGHT);
      else
	 MenuTraverse(cb, event, XmTRAVERSE_DOWN);
   }
}


/* ARGSUSED */
static void _XmMenuEscape (w, event)
Widget w;
XEvent  *event;
{
   /* Process the event only if not already processed */
   if (!_XmIsEventUnique(event))
      return;

   /* Let the menushell widget clean things up */
   (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
      menu_shell_class.popdownOne))(w, event);
}

static void _XmRC_GadgetTraverseDown (rc, event, param, num_param)
XmRowColumnWidget rc;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseDown(gadget, event, param, num_param);
}



static void _XmRC_GadgetTraverseUp (rc, event, param, num_param)
XmRowColumnWidget rc;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseUp(gadget, event, param, num_param);
}



static void _XmRC_GadgetTraverseLeft (rc, event, param, num_param)
XmRowColumnWidget rc;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   /*
    * If there is not active child, then this RowColumn has
    * not traversable children, so it's fielding traversal
    * requests itself.
    */
   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseLeft(gadget, event, param, num_param);
   else if (gadget == NULL)
      _XmMenuTraverseLeft(rc, event, param, num_param);
}



static void _XmRC_GadgetTraverseRight (rc, event, param, num_param)
XmRowColumnWidget rc;
XEvent  *event;
char   **param;
int     *num_param;
{
   XmGadget gadget = (XmGadget)rc->manager.active_child;

   /*
    * If there is not active child, then this RowColumn has
    * not traversable children, so it's fielding traversal
    * requests itself.
    */
   if (gadget && XmIsGadget(gadget))
      _XmMenuTraverseRight(gadget, event, param, num_param);
   else if (gadget == NULL)
      _XmMenuTraverseRight(rc, event, param, num_param);
}


/*
 * In case we've moved into our out of a gadget, we need to take care
 * of the highlighting ourselves, since the gadget will not get a focus
 * event.
 */

static void GadgetCleanup (rc, oldActiveChild)
XmRowColumnWidget rc;
XmGadget oldActiveChild;
{
    XmGadget newActiveChild = (XmGadget)rc->manager.active_child;

    if (oldActiveChild != newActiveChild)
    {
        if (XmIsGadget(oldActiveChild))
        {
            _XmDispatchGadgetInput(oldActiveChild, NULL, XmFOCUS_OUT_EVENT);
            oldActiveChild->gadget.have_traversal = False;
        }
    }
}



static void _XmMenuTraversalHandler (rc, pw, direction)
XmRowColumnWidget rc;
Widget pw;
int direction;
{
   Widget currentActiveChild;

   if (!rc->manager.traversal_on)
      return;

   switch (direction)
   {
      case XmTRAVERSE_UP:
      {
         if (!IsBar(rc))
         {
             currentActiveChild = rc->manager.active_child;
             _XmProcessTraversal(pw, XmTRAVERSE_PREV, True);
             GadgetCleanup(rc, currentActiveChild);
         }
         else
             Move_Up_Down (rc, pw);
         break;
      }

      case XmTRAVERSE_DOWN:
      {
         if (!IsBar(rc))
         {
             currentActiveChild = rc->manager.active_child;
             _XmProcessTraversal(pw, XmTRAVERSE_NEXT, True);
             GadgetCleanup(rc, currentActiveChild);
         }
         else
             Move_Up_Down (rc, pw);
         break;
      }

      case XmTRAVERSE_LEFT:
      {
         MoveLeft(rc, pw);
         break;
      }

      case XmTRAVERSE_RIGHT:
      {
         MoveRight(rc, pw);
         break;
      }
   }
}


/*
 * When the PM menubar mode is active, either the up or down arrow will
 * cause us to post the menupane associated with the active cascade button
 * in the menubar.
 */

static void Move_Up_Down (rc, pw)
XmRowColumnWidget rc;
Widget pw;
{
    if (rc->manager.active_child == NULL)
        return;

    if (XmIsPrimitive(pw))
    {
        XmPrimitiveClassRec * prim;
      
        prim = (XmPrimitiveClassRec *)XtClass(pw);
        (*(prim->primitive_class.arm_and_activate)) (pw, NULL);
    }

    else if (XmIsGadget(pw))
    {
        XmGadgetClassRec * gad;
      
        gad = (XmGadgetClassRec *)XtClass(pw);
        (*(gad->gadget_class.arm_and_activate)) (pw, NULL);
    }
}

/* ARGSUSED */
static void MoveLeft (rc, pw)
XmRowColumnWidget rc;
Widget pw;
{
    if (IsBar(rc))
    {
        /* Move to the previous item in the menubar */
        FindPrevMenuBarItem(rc);
    }
    else if (IsPulldown(rc))
    {
        /*
         * If we're the topmost pulldown menupane, then unpost and move to
         * the next available item in the menubar, and post its submenu.
         */
        if (RC_Type(XtParent(RC_CascadeBtn(rc))) == XmMENU_BAR)
        {
            FindPrevMenuBarCascade(XtParent(RC_CascadeBtn(rc)));
        }
        else if (RC_Type(XtParent(RC_CascadeBtn(rc))) != XmMENU_OPTION)
        {
            /* Simply unpost the last menupane */
            (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
              menu_shell_class.popdownEveryone)) (XtParent(rc), NULL);
        }
    }
}


static void MoveRight (rc, pw)
XmRowColumnWidget rc;
Widget pw;
{
   XmRowColumnWidget topLevel;
   
   /*
    * If the primitive widget is a cascade button, then post its
    * submenu, if there is one.
    */
   if (!IsBar(rc) && XmIsCascadeButtonGadget(pw) && CBG_Submenu(pw)) 
   {
      (*(((XmGadgetClassRec *)XtClass(pw))->gadget_class.
	 arm_and_activate)) (pw, NULL);
   }
   else if (!IsBar(rc) && XmIsCascadeButton(pw) && CB_Submenu(pw))
   {
      (*(((XmPrimitiveClassRec *)XtClass(pw))->primitive_class.
	 arm_and_activate)) (pw, NULL);
   }
   else if (IsBar(rc))
   {
      /* Move to the next item in the menubar */
      FindNextMenuBarItem(rc);
   }
   else if (IsPulldown(rc))
   {
      /*
       * For most buttons, requesting to move right is a Noop.  The
       * exception is when we are working with a menubar.  Then, we
       * want to unpost the current menu hierarchy, and post the
       * hierarchy associated with the next button in the menubar.
       */
      _XmGetActiveTopLevelMenu (rc, &topLevel);
      if (RC_Type(topLevel) == XmMENU_BAR)
      {
	 FindNextMenuBarCascade(topLevel);
      }
   }
}


/*
 * Find the next cascade button in the menubar which can be traversed to.
 */

static void FindNextMenuBarItem (menubar)
XmRowColumnWidget menubar;
{
   Widget child;
    register int i, j;
    int upper_limit;
    Widget active_child;
    XRectangle visRect;

    /* 
     * We're not in the PM menubar mode if we don't have an active child.
     */
    if (menubar->manager.active_child == NULL)
        return;

    _XmCreateVisibilityRect(menubar, &visRect);
    upper_limit = menubar->composite.num_children;
    active_child = menubar->manager.active_child;

    /* Find the index of the currently active item */
    for (i = 0; i < upper_limit; i++)
    {
       if (menubar->composite.children[i] == active_child)
          break;
    }

    /* Start looking at the next child */
    for (j = 0, i++; j < upper_limit - 1; j++, i++)
    {
        /* Wrap, if necessary */
        if (i >= upper_limit)
           i = 0;

        child = menubar->composite.children[i];

        /* You can't traverse to a button which has no submenu */
        if ((XmIsCascadeButton(child) && (CB_Submenu(child) == NULL)) ||
            (XmIsCascadeButtonGadget(child)) && (CBG_Submenu(child) == NULL))
           continue;

        if (_XmTestTraversability(child, &visRect))
        {
            /* Unhighlight the old active child */
            if (XmIsPrimitive(active_child))
            {
               XmPrimitiveClassRec * prim;
      
               prim = (XmPrimitiveClassRec *)XtClass(active_child);
               (*(prim->primitive_class.border_unhighlight)) (active_child);
            }

            else if (XmIsGadget(active_child))
            {
               XmGadgetClassRec * gadget;
      
               gadget = (XmGadgetClassRec *)XtClass(active_child);
               (*(gadget->gadget_class.border_unhighlight)) (active_child);
            }

            /* Highlight the child */
            menubar->manager.active_child = child;
	    (void) XmProcessTraversal (child, XmTRAVERSE_CURRENT);

            if (XmIsPrimitive(child))
            {
               XmPrimitiveClassRec * prim;
      
               prim = (XmPrimitiveClassRec *)XtClass(child);
               (*(prim->primitive_class.border_highlight)) (child);
            }
            else if (XmIsGadget(child))
            {
               XmGadgetClassRec * gadget;
      
               gadget = (XmGadgetClassRec *)XtClass(child);
               (*(gadget->gadget_class.border_highlight)) (child);
            }
            return;
        }
    }
}


/*
 * Find the previous cascade button in the menubar which can be traversed to.
 */

static void FindPrevMenuBarItem(menubar)
XmRowColumnWidget menubar;
{
    Widget child;
    register int i, j;
    int upper_limit;
    Widget active_child;
    XRectangle visRect;

    /* We're not in the PM menubar mode if we don't have an active child */
    if (menubar->manager.active_child == NULL)
        return;

    _XmCreateVisibilityRect(menubar, &visRect);
    upper_limit = menubar->composite.num_children;
    active_child = menubar->manager.active_child;

    /* Find the index of the currently active item */
    for (i = 0; i < upper_limit; i++)
    {
        if (menubar->composite.children[i] == active_child)
            break;
    }

    /* Start looking at the previous child */
    for (j = 0, --i; j < upper_limit - 1; j++, --i)
    {
        /* Wrap, if necessary */
        if (i < 0)
           i = upper_limit - 1;

        child = menubar->composite.children[i];

        /* You can't traverse to a button which has no submenu */
        if ((XmIsCascadeButton(child) && (CB_Submenu(child) == NULL)) ||
            (XmIsCascadeButtonGadget(child)) && (CBG_Submenu(child) == NULL))
           continue;

        if (_XmTestTraversability(child, &visRect))
        {
            /* Unhighlight the old active child */
            if (XmIsPrimitive(active_child))
            {
               XmPrimitiveClassRec * prim;
      
               prim = (XmPrimitiveClassRec *)XtClass(active_child);
               (*(prim->primitive_class.border_unhighlight)) (active_child);
            }
            else if (XmIsGadget(active_child))
            {
               XmGadgetClassRec * gadget;
      
               gadget = (XmGadgetClassRec *)XtClass(active_child);
               (*(gadget->gadget_class.border_unhighlight)) (active_child);
            }

            /* Highlight the new active child */
            menubar->manager.active_child = (Widget)child;
	    (void) XmProcessTraversal (child, XmTRAVERSE_CURRENT);

            if (XmIsPrimitive(child))
            {
               XmPrimitiveClassRec * prim;
      
               prim = (XmPrimitiveClassRec *)XtClass(child);
               (*(prim->primitive_class.border_highlight)) (child);
            }
            else if (XmIsGadget(child))
            {
               XmGadgetClassRec * gadget;
      
               gadget = (XmGadgetClassRec *)XtClass(child);
               (*(gadget->gadget_class.border_highlight)) (child);
            }
            return;
        }
    }
}


/*
 * Find the next hierarchy in the menubar which can be traversed to.
 */

static void FindNextMenuBarCascade (menubar)
XmRowColumnWidget menubar;
{
   Widget child;
   register int i, j;
   int upper_limit;
   XRectangle visRect;
   ShellWidget shell;

   _XmCreateVisibilityRect(menubar, &visRect);
   upper_limit = menubar->composite.num_children;

   /* 
    * Determine which child is popped up.
    */
   shell = (ShellWidget) RC_PopupPosted(menubar);
   child = RC_CascadeBtn(shell->composite.children[0]);

   /* Find the index of the currently active item */
   for (i = 0; i < upper_limit; i++)
   {
      if (menubar->composite.children[i] == child)
	  break;
   }

   /* Start looking at the next child */
   for (j = 0, i++; j < upper_limit - 1; j++, i++)
   {
      /* Wrap, if necessary */
      if (i >= upper_limit)
	  i = 0;

      child = menubar->composite.children[i];
      
      if (XmIsCascadeButtonGadget(child))
      {
	 /* You can't traverse to a button which has no submenu */
	 if (XmIsCascadeButtonGadget(child) && (CBG_Submenu(child) == NULL))
	     continue;
	 
	 if (_XmTestTraversability(child, &visRect))
	 {
	    XmGadgetClassRec * gadget;
	    
	    gadget = (XmGadgetClassRec *)XtClass(child);
	    (*(gadget->gadget_class.arm_and_activate)) (child, NULL);
	    return;
	 }
      }

      else if (XmIsCascadeButton(child))
      {
	 /* You can't traverse to a button which has no submenu */
	 if (XmIsCascadeButton(child) && (CB_Submenu(child) == NULL))
	     continue;

	 if (_XmTestTraversability(child, &visRect))
	 {
	    XmPrimitiveClassRec * prim;
      
	    prim = (XmPrimitiveClassRec *)XtClass(child);
	    (*(prim->primitive_class.arm_and_activate)) (child, NULL);
	    return;
	 }
      }
   }
}


/*
 * Find the previous hierarchy in the menubar which can be traversed to.
 */

static void FindPrevMenuBarCascade (menubar)
XmRowColumnWidget menubar;
{
    Widget child;
    register int i, j;
    int upper_limit;
    XRectangle visRect;
    ShellWidget shell;

    _XmCreateVisibilityRect(menubar, &visRect);
    upper_limit = menubar->composite.num_children;

    /* Determine which child is popped up */
    shell = (ShellWidget) RC_PopupPosted(menubar);
    child = RC_CascadeBtn(shell->composite.children[0]);

    /* Find the index of the currently active item */
    for (i = 0; i < upper_limit; i++)
    {
        if (menubar->composite.children[i] == child)
           break;
    }

    /* Start looking at the previous child */
    for (j = 0, --i; j < upper_limit - 1; j++, --i)
    {
        /* Wrap, if necessary */
        if (i < 0)
           i = upper_limit - 1;

	child = menubar->composite.children[i];
	
	if (XmIsCascadeButtonGadget(child))
	{
	   /* You can't traverse to a button which has no submenu */
	   if (CBG_Submenu(child) == NULL)
	       continue;

	   if (_XmTestTraversability(child, &visRect))
	   {
	      XmGadgetClassRec * gadget;
      
	      gadget = (XmGadgetClassRec *)XtClass(child);
	      (*(gadget->gadget_class.arm_and_activate)) (child, NULL);
	      return;
	   }
	}
	else if (XmIsCascadeButton (child))
	{
	   /* You can't traverse to a button which has no submenu */
	   if (CB_Submenu(child) == NULL)
	       continue;

	   if (_XmTestTraversability(child, &visRect))
	   {
	      XmPrimitiveClassRec * prim;
      
	      prim = (XmPrimitiveClassRec *)XtClass(child);
	      (*(prim->primitive_class.arm_and_activate)) (child, NULL);
	      return;
	   }
	}
    }
}


/* ARGSUSED */
static void _XmRC_FocusIn (rc, event, params, num_params)
XmRowColumnWidget  rc;
XEvent      *event;
char    **params;
int     *num_params;
{
   /*
    * For popup and pulldown menupanes, we want to ignore focusIn request
    * which occur when we are not visible.
    */

   if (IsBar(rc))
      _XmManagerFocusIn(rc, event);
   else if ((((XmMenuShellWidget)XtParent(rc))->shell.popped_up) &&
            (rc->manager.traversal_on))
      _XmManagerFocusInInternal(rc, event);
}


/* ARGSUSED */
static void _XmRC_FocusOut (rc, event, params, num_params)
XmRowColumnWidget  rc;
XEvent      *event;
char    **params;
int     *num_params;
{
    _XmManagerFocusOut(rc, event);
}



/* ARGSUSED */
static void _XmRC_Unmap (rc, event, params, num_params)
XmRowColumnWidget  rc;
XEvent      *event;
char    **params;
int     *num_params;
{
   /*
    * For popup and pulldown menupanes, we never care about being notified
    * when we are unmapped.  For menubars, we want normal unmapping 
    * processing to occur.
    */

   if (IsBar(rc))
      _XmManagerUnmap(rc, event);
}



static void _XmRC_Enter (rc, event)
XmRowColumnWidget  rc;
XEvent      *event;
{
   if (IsBar(rc) && RC_IsArmed(rc))
       return;

   _XmManagerEnter(rc, event);
}


/*
 * Catch an 'Escape' which occurs within a gadget, and bring down the
 * menu system.
 */
static void _XmRC_GadgetEscape (rc, event)
XmRowColumnWidget  rc;
XEvent      *event;
{
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
            ArmAndActivate(rc, NULL);
    }
    else
    {
        /* Let the menushell widget clean things up */
        (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
          menu_shell_class.popdownOne))(XtParent(rc), event);
    }

   _XmRecordEvent(event);
}



/*
 * When the user requests help (F1) for a gadget in a menu, by the time
 * the menu widget gets notified, the menu system has already been cleaned
 * up; the information describing which gadget was active is lost.  So, we
 * will grab this information before disabling the menu system.
 */

/* ARGSUSED */
static void _XmGetGadget (rc, event, params, num_params)
XmRowColumnWidget  rc;
XEvent      *event;
char    **params;
int     *num_params;
{
   if ((rc->manager.active_child) && XmIsGadget(rc->manager.active_child))
      ActiveGadgetChild = (XmGadget)rc->manager.active_child;
   else
      ActiveGadgetChild = NULL;
}




/*
 * During traversal, when the focus is moved to the next menupane,
 * the cascade button to which the menupane is attached will get a
 * FocusOut event, thus causing it to unhighlight.  To prevent this,
 * we will discard the FocusOut event if we are moving to a new submenu.
 */

static Boolean ShouldDispatchFocusOut (widget)
Widget widget;
{
   XmManagerWidget parent = (XmManagerWidget)XtParent(widget);

   if (XmIsRowColumn(parent) && 
       ((RC_Type(parent) == XmMENU_BAR) ||
        (RC_Type(parent) == XmMENU_POPUP) || 
        (RC_Type(parent) == XmMENU_PULLDOWN)))
   {
       if (XmIsCascadeButton(widget))
       {
           if (CB_Submenu(widget) &&
             ((XmMenuShellWidget)XtParent(CB_Submenu(widget)))->shell.popped_up)
           {
               return(False);
           }
       }
       else if (XmIsCascadeButtonGadget(widget))
       {
           if (CBG_Submenu(widget) &&
               ((XmMenuShellWidget)XtParent(CBG_Submenu(widget)))->shell.
                                                                  popped_up)
           {
               return(False);
           }
       }
   }

   return (True);
}


/*
 * this entry is set in label and label gadget's class field so that
 * all communication from the buttons to the RowColumn are done through
 * this entry and then revectored to the appropriate routine.
 */
static void MenuProcedureEntry (proc, widget, flag, data, data2)
int proc;
Widget widget;
Boolean flag;
caddr_t data;
caddr_t data2;
{
   switch (proc)
   {
     case XmMENU_CASCADING:
       PrepareToCascade(widget, data, data2);
       break;
			
     case XmMENU_POPDOWN:
       MenuPopDown (widget, data);
       break;

     case XmMENU_SHELL_POPDOWN:
       (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	  menu_shell_class.popdownEveryone))(widget, data);
       break;

     case XmMENU_BUTTON:
       VerifyMenuButton (widget, data, data2);
       break;
       
     case XmMENU_CALLBACK:
       /* data points to the widget which was activated */
       ChildsActivateCallback (widget, data, data2);
       break;
       
     case XmMENU_TRAVERSAL:
       /* data points to a boolean */
       SetMenuTraversal (widget, flag);
       break;

     case XmMENU_SUBMENU:
       SetCascadeField (widget, data, flag);
       break;

     case XmMENU_PROCESS_TREE:
       DoProcessMenuTree (widget, XmREPLACE);
       break;

     case XmMENU_ARM:
       MenuArm (widget);
       break;

     case XmMENU_DISARM:
       MenuDisarm (widget);
       break;

     case XmMENU_BAR_CLEANUP:
       MenuBarCleanup (widget);
       break;

     default:
       break;
  }
}



