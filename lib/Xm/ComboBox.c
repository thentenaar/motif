/* $TOG: ComboBox.c /main/16 1999/07/28 10:59:19 vipin $ */
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

/* ComboBox.c */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>		/* for bzero et al */
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <Xm/AccTextT.h>
#include <Xm/ArrowB.h>
#include <Xm/ComboBoxP.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/GrabShellP.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VendorS.h>
#include <Xm/VaSimpleP.h>
#include "GeoUtilsI.h"
#include "MenuShellI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "XmI.h"
#include "XmStringI.h"

#define	NOKIDS		_XmMMsgComboBox_0000
#define	TYPEWARNING	_XmMMsgComboBox_0001
#define	MISSINGKID	_XmMMsgComboBox_0004
#define	UNMANAGEDKID	_XmMMsgComboBox_0005
#define	MBWARNING	_XmMMsgComboBox_0006
#define	WRONGPARAMS	_XmMMsgComboBox_0007
#define	WRONGWIDGET	_XmMMsgComboBox_0008
#define SELECTBADITEM	_XmMMsgComboBox_0009
#define SETBADITEM	_XmMMsgComboBox_0010
#define DELETEBADPOS	_XmMMsgComboBox_0011
#define NOTACOMBOBOX	_XmMMsgComboBox_0012
#define NOSETKIDS	_XmMMsgComboBox_0013
#define MODEWARNING	_XmMMsgComboBox_0014

#define default_translations	_XmComboBox_defaultTranslations

#define	LIST_CHILD_NAME		"List"
#define	TEXT_CHILD_NAME		"Text"
#define	SHELL_CHILD_NAME	"GrabShell"

/* From GrabShell.c */
#define Events	(EnterWindowMask | LeaveWindowMask | ButtonPressMask | \
		 ButtonReleaseMask)

/* CB_ShellState() values. */
enum { POPPED_DOWN, POPPING_UP, POPPED_UP, POPPING_DOWN };

/********    Static Function Declarations    ********/

/** Widget Class Methods **/
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static void ClassInitialize (void);
static void ClassPartInitialize (WidgetClass wc);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager (Widget,
					 XtWidgetGeometry *,
					 XtWidgetGeometry *);
static void InsertChild (Widget child);
static void Resize (Widget widget);
static void Redisplay (Widget widget, XEvent *event, Region region);
static XtGeometryResult QueryGeometry (Widget,
				       XtWidgetGeometry *,
				       XtWidgetGeometry *);
static void ChangeManaged (Widget widget);
static void Destroy (Widget widget);
static void ConstraintDestroy (Widget widget);
static Boolean ComboBoxParentProcess (Widget wid, XmParentProcessData event);

/** Event Handlers **/
static void PopupEH (Widget, XtPointer, XEvent *, Boolean *);
static void SBBtnDownEH (Widget, XtPointer, XEvent *, Boolean *);
static void SBBtnUpEH (Widget, XtPointer, XEvent *, Boolean *);

/** Callbacks **/
static void TextChangedCB (Widget, XtPointer, XtPointer);
static void ListSelectionCB (Widget, XtPointer, XtPointer);
static void ShellPopupCB (Widget, XtPointer, XtPointer);
static void ShellPopdownCB (Widget, XtPointer, XtPointer);
static void FocusMovedCB (Widget, XtPointer, XtPointer);

/** Action Routines **/
static void CBArmAndDropDownList (Widget, XEvent *, String *, Cardinal *);
static void CBDisarm (Widget, XEvent *, String *, Cardinal *);
static void CBDropDownList (Widget, XEvent *, String *, Cardinal *);
static void CBFocusIn (Widget, XEvent *, String *, Cardinal *);
static void CBFocusOut (Widget, XEvent *, String *, Cardinal *);
static void CBCancel (Widget, XEvent *, String *, Cardinal *);
static void CBActivate (Widget, XEvent *, String *, Cardinal *);
static void CBListAction (Widget, XEvent *, String *, Cardinal *);
static void CBTextFocusOut (Widget, XEvent *, String *, Cardinal *);

/** Default resource value call procs. **/
static void CheckSetRenderTable (Widget, int, XrmValue *);

/** Synthetic Resource access methods **/
static XmImportOperator CBSetSelectedItem (Widget, int, XtArgVal *);
static XmImportOperator CBSetSelectedPos (Widget, int, XtArgVal *);
static void CBGetSelectedItem (Widget, int, XtArgVal *);
static void CBGetSelectedPos (Widget, int, XtArgVal *);
static void CBGetColumns (Widget, int, XtArgVal *);
static void CBGetItems (Widget, int, XtArgVal *);
static void CBGetItemCount (Widget, int, XtArgVal *);
static void CBGetVisibleItemCount (Widget, int, XtArgVal *);

/** Additional Functions **/
static Boolean PopdownList (Widget cb, XEvent *event);
static void CreateChildren (Widget, ArgList, Cardinal *);
static Widget CreateEditBox (Widget, String, Widget, ArgList, Cardinal *);
static Widget CreateScrolledList (Widget, String, Widget, ArgList, Cardinal *);
static Widget CreatePulldown (Widget, String, Widget, ArgList, Cardinal *);
static void ComputeSize (Widget, Dimension, Dimension, Dimension*, Dimension*);
static void DoLayout (Widget);
static void GetThickness (Widget, Dimension *, Dimension *);
static void GetArrowGC (Widget);
static void DrawArrow (Widget, Boolean);
static void DrawShadows (Widget);
static Boolean Hit (XButtonEvent*, XRectangle);
static void HighlightBorder (Widget w);
static void UnhighlightBorder (Widget w);
static XmComboBoxWidget FindComboBox (Widget w);
static void CallSelectionCallbacks (Widget w, XEvent *event);
static void SetHitArea (Widget w);
static Boolean ReduceResources (Widget widget, Dimension *width,
				Dimension *height, Mask flags);
static int Reduce (Dimension *size, int max_change, int min_size);
static void GetIdealTextSize (Widget cb, int *width, int *height);
static Dimension GetDefaultArrowSize (Widget cb);
static XmString GetEditBoxValue (Widget cb);
static void SetEditBoxValue (Widget cb, XmString value);

/********    End Static Function Declarations    ********/


#define POPUP_EVENT_MASK		\
	(EnterWindowMask | ButtonPressMask | ButtonReleaseMask)

#define	MINIMUM_SHADOWTHICKNESS		1
#define	MINIMUM_HIGHLIGHTTHICKNESS	2
#define	MINIMUM_ARROWSPACE		1
#define MINTXT				1
#define MINLIST				1

#define	DEFAULT_MARGINWIDTH 		2
#define	DEFAULT_MARGINHEIGHT 		2

#define	HIGHLIGHT_THICKNESS		((Mask) (1<<0))
#define	SHADOW_THICKNESS		((Mask) (1<<1))
#define	ARROW_SPACING			((Mask) (1<<2))
#define	MARGIN_WIDTH			((Mask) (1<<3))
#define	MARGIN_HEIGHT			((Mask) (1<<4))
#define	ARROW_SIZE			((Mask) (1<<5))

#define DEFAULT_ARROW_SCALING	0.75
#define	SQRT3_OVER_2		0.8660254037844

#define XmUNSPECIFIED_COLUMNS		((short) XmUNSPECIFIED_COUNT)
#define	XmUNSPECIFIED_ITEMS		((XmStringTable) XmUNSPECIFIED)


/*
 * These instance fields have special non-obvious meanings, and are
 * used for passing synthetic resource values to and from children.
 * The value of these fields is NOT maintained -- always query the
 * child for the current value.
 */

#define CBS_Columns(w)	     (((XmComboBoxWidget)(w))->combo_box.columns)
#define CBS_ItemCount(w)     (((XmComboBoxWidget)(w))->combo_box.item_count)
#define CBS_Items(w) 	     (((XmComboBoxWidget)(w))->combo_box.items)
#define CBS_SelectedItem(w)  (((XmComboBoxWidget)(w))->combo_box.selected_item)
#define CBS_SelectedPosition(w)		\
	(((XmComboBoxWidget)(w))->combo_box.selected_position)
#define CBS_VisibleItemCount(w)		\
	(((XmComboBoxWidget)(w))->combo_box.visible_item_count)


#define	Offset(field) XtOffsetOf(XmComboBoxRec, field)

static XtResource resources[] =
{
  {
    XmNarrowSize, XmCArrowSize, XmRHorizontalDimension,
    sizeof(Dimension), Offset(combo_box.arrow_size),
    XmRImmediate, (XtPointer)XmINVALID_DIMENSION
  },
  {
    XmNarrowSpacing, XmCArrowSpacing, XmRHorizontalDimension,
    sizeof(Dimension), Offset(combo_box.arrow_spacing),
    XmRImmediate, (XtPointer)XmINVALID_DIMENSION
  },
  {
    XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(combo_box.highlight_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  },
  {
    XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
    sizeof(Dimension), Offset(combo_box.margin_height),
    XmRImmediate, (XtPointer)DEFAULT_MARGINHEIGHT
  },
  {
    XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension), Offset(combo_box.margin_width),
    XmRImmediate, (XtPointer)DEFAULT_MARGINWIDTH
  },
  {
    XmNmatchBehavior, XmCMatchBehavior, XmRMatchBehavior,
    sizeof(unsigned char), Offset(combo_box.match_behavior),
    XmRImmediate, (XtPointer)XmINVALID_MATCH_BEHAVIOR
  },
  {
    XmNnavigationType, XmCNavigationType, XmRNavigationType,
    sizeof(unsigned char), Offset(manager.navigation_type),
    XmRImmediate, (XtPointer)XmSTICKY_TAB_GROUP
  },
  {
    XmNselectionCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), Offset(combo_box.selection_callback),
    XmRCallback, NULL
  },
  {
    XmNselectedItem, XmCSelectedItem, XmRXmString,
    sizeof(XmString), Offset(combo_box.selected_item),
    XmRImmediate, (XtPointer)NULL
  },
  {
    XmNselectedPosition, XmCSelectedPosition, XmRInt,
    sizeof(int), Offset(combo_box.selected_position),
    XmRImmediate, (XtPointer)XmINVALID_POSITION
  },
  {
    XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), Offset(manager.shadow_thickness),
    XmRCallProc, (XtPointer) _XmSetThickness
  },
  {
    XmNcomboBoxType, XmCComboBoxType, XmRComboBoxType,
    sizeof(unsigned char), Offset(combo_box.type),
    XtRImmediate, (XtPointer)XmCOMBO_BOX
  },
/* Borrow the "text_changed" instance field .. ! */
  {
    "pri.vate", "Pri.vate", XmRBoolean,
    sizeof(Boolean), Offset(combo_box.text_changed),
    XmRImmediate, (XtPointer) False
  },
  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmRenderTable), Offset(combo_box.render_table),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },
  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), Offset(combo_box.render_table),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },
  {
    XmNlist, XmCList, XmRWidget,
    sizeof(Widget), Offset(combo_box.list),
    XmRImmediate, (XtPointer)NULL
  },
  {
    XmNtextField, XmCTextField, XmRWidget,
    sizeof(Widget), Offset(combo_box.edit_box),
    XmRImmediate, (XtPointer)NULL
  },
  {
    XmNitems, XmCItems, XmRXmStringTable,
    sizeof(XmStringTable), Offset(combo_box.items),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_ITEMS
  },
  {
    XmNitemCount, XmCItemCount, XmRInt,
    sizeof(int), Offset(combo_box.item_count),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_COUNT
  },
  {
    XmNvisibleItemCount, XmCVisibleItemCount, XmRInt,
    sizeof(int), Offset(combo_box.visible_item_count),
    XmRImmediate, (XtPointer)10
  },
  {
    XmNcolumns, XmCColumns, XmRShort,
    sizeof(short), Offset(combo_box.columns),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_COLUMNS
  },
  {
    XmNpositionMode, XmCPositionMode, XmRPositionMode,
    sizeof(XtEnum), Offset(combo_box.position_mode),
    XmRImmediate, (XtPointer)XmZERO_BASED
  }
};

/*
 * Resolution independent resources and resources needing special processing.
 */

static XmSyntheticResource syn_resources[] =
{
  {
    XmNselectedItem, sizeof(XmString), Offset(combo_box.selected_item),
    CBGetSelectedItem, CBSetSelectedItem
  },
  {
    XmNselectedPosition, sizeof(int), Offset(combo_box.selected_position),
    CBGetSelectedPos, CBSetSelectedPos
  },
  {
    XmNmarginWidth, sizeof(Dimension), Offset(combo_box.margin_width),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },
  {
    XmNmarginHeight, sizeof(Dimension), Offset(combo_box.margin_height),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },
  {
    XmNhighlightThickness, sizeof(Dimension),
    Offset(combo_box.highlight_thickness),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },
  {
    XmNshadowThickness, sizeof(Dimension), Offset(manager.shadow_thickness),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },
  {
    XmNarrowSize, sizeof(Dimension), Offset(combo_box.arrow_size),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },
  {
    XmNarrowSpacing, sizeof(Dimension), Offset(combo_box.arrow_spacing),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },
  {
    XmNcolumns, sizeof(short), Offset(combo_box.columns),
    CBGetColumns, NULL
  },
  {
    XmNitems, sizeof(XmStringTable), Offset(combo_box.items),
    CBGetItems, NULL
  },
  {
    XmNitemCount, sizeof(int), Offset(combo_box.item_count),
    CBGetItemCount, NULL
  },
  {
    XmNvisibleItemCount, sizeof(int), Offset(combo_box.visible_item_count),
    CBGetVisibleItemCount, NULL
  }
};
#undef Offset

static XtAccelerators parsed_accelerators;
static XtAccelerators parsed_list_accelerators;
static XtTranslations parsed_list_translations;
static XtTranslations parsed_text_focus_translations;

static XtActionsRec actionsList[] = {
  { "CBArmAndDropDownList",	CBArmAndDropDownList	},
  { "CBDisarm",			CBDisarm		},
  { "CBDropDownList",     	CBDropDownList		},
  { "CBFocusIn",		CBFocusIn		},
  { "CBFocusOut",		CBFocusOut		},
  { "CBCancel",		 	CBCancel		},
  { "CBActivate",		CBActivate		},
  { "CBListAction",		CBListAction		},
  { "CBTextFocusOut",		CBTextFocusOut		}
};

/* Class Record Initialization */
externaldef(xmcomboboxclassrec) XmComboBoxClassRec xmComboBoxClassRec =
{
  {
    /* core_class fields */
    (WidgetClass) &xmManagerClassRec,	/* superclass             */
    "XmComboBox",                       /* class_name             */
    sizeof(XmComboBoxRec),              /* widget_size            */
    ClassInitialize,			/* class_initialize       */
    ClassPartInitialize,                /* class_part_initialize  */
    FALSE,                              /* class_inited           */
    Initialize,             		/* initialize             */
    NULL,                               /* initialize_hook        */
    XtInheritRealize,                   /* realize                */
    actionsList,                  	/* actions                */
    XtNumber(actionsList),		/* num_actions            */
    (XtResourceList)resources,          /* resources              */
    XtNumber(resources),                /* num_resources          */
    NULLQUARK,                          /* xrm_class              */
    True,                               /* compress_motion        */
    XtExposeCompressMaximal | 		/* compress_exposure      */
        XtExposeNoRegion,
    FALSE,                              /* compress_enterleave    */
    FALSE,                              /* visible_interest       */
    Destroy,                   		/* destroy                */
    Resize,     			/* resize                 */
    Redisplay,    			/* expose                 */
    SetValues,         			/* set_values             */
    NULL,                               /* set_values_hook        */
    XtInheritSetValuesAlmost,           /* set_values_almost      */
    NULL,                      		/* get_values_hook        */
    NULL,               		/* accept_focus           */
    XtVersion,				/* version                */
    NULL,                               /* callback_private       */
    default_translations,		/* tm_table               */
    QueryGeometry,   			/* Query Geometry proc    */
    NULL,                               /* disp accelerator       */
    NULL                                /* extension              */
  },
  {
    /* composite_class fields */
    GeometryManager, 			/* geometry_manager       */
    ChangeManaged,   			/* change_managed         */
    InsertChild,              		/* insert_child           */
    XtInheritDeleteChild,	        /* delete_child           */
    NULL                     		/* extension              */
  },
  {
    /* constraint_class fields */
    NULL,				/* constraint resource    */
    0,        				/* number of constraints  */
    sizeof(XmManagerConstraintRec),	/* size of constraint     */
    NULL,				/* initialization         */
    ConstraintDestroy,			/* destroy proc           */
    NULL,				/* set_values proc        */
    NULL				/* extension              */
  },
  {
    /* manager_class fields */
    XtInheritTranslations,		/* translations           */
    syn_resources,			/* syn_resources          */
    XtNumber(syn_resources),		/* num_syn_resources      */
    NULL,				/* syn_cont_resources     */
    0,					/* num_syn_cont_resources */
    ComboBoxParentProcess,		/* parent_process         */
    NULL				/* extension              */
  },
  {
    /* combo_box class fields */
    NULL                             	/* extension		  */
  }
};


externaldef(xmcomboboxwidgetclass) WidgetClass xmComboBoxWidgetClass =
       (WidgetClass) &xmComboBoxClassRec;


/* ------------- WIDGET CLASS METHODS ---------- */


/*
 * Initialize()
 *	Called by the Intrinsics when a new ComboBox is created.
 */
static void
Initialize(Widget    request,	/* unused */
	   Widget    new_w,
	   ArgList   args,
	   Cardinal *num_args)
{
  XmComboBoxWidget newcb = (XmComboBoxWidget)new_w;
  Widget	   ancestor;
  int i;
  Cardinal num_child_args;
  ArgList child_args;

  /* Setup internal state. */
  CB_ShellState(newcb) = POPPED_DOWN;
  newcb->combo_box.scrolling = FALSE;
  newcb->combo_box.vsb = NULL;
  newcb->combo_box.hsb = NULL;
  CB_TextChanged(newcb) = FALSE;

  /* Set defaults */
  CB_Highlighted(newcb) = FALSE;
  CB_ArrowPressed(newcb) = FALSE;
  CB_ScrolledW(newcb) = CB_ListShell(newcb) = 0;
  newcb->combo_box.ideal_ebwidth = newcb->combo_box.ideal_ebheight = 0;
  CB_HitRect(newcb).width = 0;
  CB_HitRect(newcb).height = 0;
  CB_HitRect(newcb).x = 0;
  CB_HitRect(newcb).y = 0;

  newcb->combo_box.arrow_shadow_width = newcb->manager.shadow_thickness;

  if (newcb->core.accelerators == NULL)
    newcb->core.accelerators = parsed_accelerators;

  /* Validate XmNpositionMode. */
  if (!XmRepTypeValidValue(XmRID_POSITION_MODE,
			   CB_PositionMode(newcb), (Widget) newcb))
    {
      CB_PositionMode(newcb) = XmZERO_BASED;
    }

  /* The list child cannot be set. */
  if (CB_List(newcb))
    {
      CB_List(newcb) = NULL;
      XmeWarning(new_w, NOSETKIDS);
    }

  /* The editbox child cannot be set. */
  if (CB_EditBox(newcb))
    {
      CB_EditBox(newcb) = NULL;
      XmeWarning(new_w, NOSETKIDS);
    }

  /* Get arrow GC. */
  GetArrowGC((Widget) newcb);

  if (CB_ArrowSpacing(newcb) == XmINVALID_DIMENSION)
    {
      CB_ArrowSpacing(newcb) = CB_MarginWidth(newcb);
      /* ArrowSize set dynamically in DoLayout() */
    }

  if (CB_MatchBehavior(newcb) == XmINVALID_MATCH_BEHAVIOR)
    {
      if (CB_Type(newcb) == XmDROP_DOWN_LIST)
	CB_MatchBehavior(newcb) = XmQUICK_NAVIGATE;
      else
	CB_MatchBehavior(newcb) = XmNONE;
    }
  else if ((CB_MatchBehavior(newcb) == XmQUICK_NAVIGATE) &&
	   (CB_Type(newcb) != XmDROP_DOWN_LIST))
    {
      CB_MatchBehavior(newcb) = XmNONE;
      XmeWarning(new_w, MBWARNING);
    }

  /* Get a real render table. */
  if (CB_RenderTable(newcb) == NULL)
    CB_RenderTable(newcb) = XmeGetDefaultRenderTable(new_w, XmTEXT_FONTLIST);
  CB_RenderTable(newcb) = XmFontListCopy(CB_RenderTable(newcb));

  /* Ignore XmNheight resource value for descedants */
  num_child_args = 0;
  child_args = (ArgList) XtMalloc(sizeof(Arg) * *num_args);
  for (i = 0; i < *num_args; i++)
	  if (strcmp(args[i].name, "height")) {
		  child_args[num_child_args] = args[i];
	  	  num_child_args++;
	  }
  /* Create the widgets that make up a ComboBox. */
  CreateChildren(new_w, child_args, &num_child_args);
  XtFree((char *) child_args);

  /* Walk up hierarchy to find vendor shell. */
  ancestor = XtParent(new_w);
  while (ancestor && !XmIsVendorShell(ancestor))
    ancestor = XtParent(ancestor);

  /* Setup focus moved callback so ComboBox can draw highlighting rect. */
  if (ancestor)
    XmeAddFocusChangeCallback(ancestor, FocusMovedCB, (XtPointer) new_w);

  /* Since we cannot add children to a ComboBox, we can consider it
     as a Primitive, and set its preferred width/height. */
  if (XtWidth(new_w) == 0 || XtHeight(new_w) == 0)
    ComputeSize((Widget) new_w, 0, 0, &(XtWidth(new_w)), &(XtHeight(new_w)));

  /* Reset synthetic child resources so SetValues can detect all changes. */
  CBS_Columns(newcb) = XmUNSPECIFIED_COLUMNS;
  CBS_ItemCount(newcb) = XmUNSPECIFIED_COUNT;
  CBS_Items(newcb) = XmUNSPECIFIED_ITEMS;
  CBS_VisibleItemCount(newcb) = XmUNSPECIFIED_COUNT;

  /* Reset fields whose value we ignore after creation. */
  CBS_SelectedPosition(newcb) = XmINVALID_POSITION;
  CBS_SelectedItem(newcb) = NULL;
}

/*
 * ClassInitialize()
 *	This routine is invoked only once.
 */

static void
ClassInitialize(void)
{
  /* Parse the default translation and accelerator tables */
  parsed_accelerators =
    XtParseAcceleratorTable(_XmComboBox_defaultAccelerators);
  parsed_list_accelerators =
    XtParseAcceleratorTable(_XmComboBox_dropDownComboBoxAccelerators);
  parsed_list_translations =
    XtParseTranslationTable(_XmComboBox_dropDownListTranslations);
  parsed_text_focus_translations =
    XtParseTranslationTable(_XmComboBox_textFocusTranslations);
}

/*
 * ClassPartInitialize()
 *	Set up the fast subclassing.  Invoked for every (sub)class of
 * ComboBox.
 */
static void
ClassPartInitialize(WidgetClass wc)
{
  _XmFastSubclassInit (wc, XmCOMBO_BOX_BIT);
}

/*
 * InsertChild()
 *	Called by the intrinsics when a new child is added to a ComboBox.
 * Generate an error message if an application tries to insert widgets
 * into the ComboBox.
 */

static void
InsertChild(Widget child)
{
  XmComboBoxWidget cb = (XmComboBoxWidget) XtParent(child);
  XtWidgetProc insert_child;

  /*
   * If the ComboBox has already created its children, the application
   * is trying to add another child, which is an error.
   */
  if (cb->composite.num_children > 2)
    XmeWarning((Widget)cb, NOKIDS);

  else {
    /* Call manager InsertChild to update child info. */
  _XmProcessLock();
  insert_child = ((XmManagerWidgetClass) xmManagerWidgetClass)
			->composite_class.insert_child;
  _XmProcessUnlock();
  (*insert_child)(child);
  }
}


/*
 * SetValues()
 *	Called by Intrinsics when XtSetValues is called on the ComboBox.
 */
static Boolean
SetValues(Widget    current,
	  Widget    request,	/* unused */
	  Widget    new_w,
	  ArgList   user_args,	/* unused */
	  Cardinal *num_args)	/* unused */
{
  XmComboBoxWidget curcb = (XmComboBoxWidget)current;
  XmComboBoxWidget newcb = (XmComboBoxWidget)new_w;
  Boolean	   resize = FALSE;
  Boolean	   dolayout = FALSE;
  Boolean	   redisplay = FALSE;
  Cardinal	   nlist = 0;
  Arg		   list_args[10];
  Cardinal	   nshell = 0;
  Arg		   shell_args[10];
  Cardinal	   neditbox = 0;
  Arg		   editbox_args[10];
  XmFontList	   old_render_table = NULL;

  /* The position_mode cannot be changed after creation. */
  if (CB_PositionMode(curcb) != CB_PositionMode(newcb))
    {
      CB_PositionMode(newcb) = CB_PositionMode(curcb);
      XmeWarning(current, MODEWARNING);
    }

  /* The ComboBox type cannot change after creation. */
  if (CB_Type(curcb) != CB_Type(newcb))
    {
      CB_Type(newcb) = CB_Type(curcb);
      XmeWarning(current, TYPEWARNING);
    }

  /* The list child cannot be set. */
  if (CB_List(curcb) != CB_List(newcb))
    {
      CB_List(newcb) = CB_List(curcb);
      XmeWarning(current, NOSETKIDS);
    }

  /* The editbox child cannot be set. */
  if (CB_EditBox(curcb) != CB_EditBox(newcb))
    {
      CB_EditBox(newcb) = CB_EditBox(curcb);
      XmeWarning(current, NOSETKIDS);
    }

  /* Validate XmNmatchBehavior and propagate it to the list. */
  if (CB_MatchBehavior(curcb) != CB_MatchBehavior(newcb))
    {
      if (CB_Type(curcb) != XmDROP_DOWN_LIST &&
	  CB_MatchBehavior(newcb) == XmQUICK_NAVIGATE)
	{
	  CB_MatchBehavior(newcb) = CB_MatchBehavior(curcb);
	  XmeWarning(current, MBWARNING);
	}
      else if (CB_Type(curcb) == XmDROP_DOWN_LIST)
	{
	  XtSetArg(list_args[nlist], XmNmatchBehavior, CB_MatchBehavior(newcb)),
	    nlist++;
	}
    }

  /* Propagate XmNcolumns to the edit box. */
  if (CBS_Columns(newcb) != XmUNSPECIFIED_COLUMNS)
    {
      XtSetArg(editbox_args[neditbox], XmNcolumns, CBS_Columns(newcb)),
        neditbox++;
      CBS_Columns(newcb) = XmUNSPECIFIED_COLUMNS;
      resize = dolayout = redisplay = TRUE;
    }

  /* Propagate XmNitems to the list. */
  if (CBS_Items(newcb) != XmUNSPECIFIED_ITEMS)
    {
      XtSetArg(list_args[nlist], XmNitems, CBS_Items(newcb)), nlist++;
      CBS_Items(newcb) = XmUNSPECIFIED_ITEMS;
    }

  /* Propagate XmNitemCount to the list. */
  if (CBS_ItemCount(newcb) != XmUNSPECIFIED_COUNT)
    {
      XtSetArg(list_args[nlist], XmNitemCount, CBS_ItemCount(newcb)), nlist++;
      CBS_ItemCount(newcb) = XmUNSPECIFIED_COUNT;
    }

  /* Propagate XmNvisibleItemCount to the list. */
  if (CBS_VisibleItemCount(newcb) != XmUNSPECIFIED_COUNT)
    {
      XtSetArg(list_args[nlist], XmNvisibleItemCount,
	       CBS_VisibleItemCount(newcb)), nlist++;
      CBS_VisibleItemCount(newcb) = XmUNSPECIFIED_COUNT;
    }

  /* Propagate our XmNborderWidth to the popup list shell. */
  if (XtBorderWidth(curcb) != XtBorderWidth(newcb))
    {
      if (CB_Type(curcb) != XmCOMBO_BOX)
	{
	  XtSetArg(shell_args[nshell], XmNborderWidth, XtBorderWidth(newcb)),
	    nshell++;

	  redisplay = TRUE;
	}
    }

  /* Propagate XmNrenderTable to all children. */
  if (CB_RenderTable(curcb) != CB_RenderTable(newcb))
    {
      if (CB_RenderTable(newcb) == NULL)
	CB_RenderTable(newcb) = XmeGetDefaultRenderTable(new_w,
							 XmTEXT_FONTLIST);
      CB_RenderTable(newcb) = XmFontListCopy(CB_RenderTable(newcb));

      XtSetArg(editbox_args[neditbox], XmNrenderTable, CB_RenderTable(newcb)),
        neditbox++;
      XtSetArg(list_args[nlist], XmNrenderTable, CB_RenderTable(newcb)),
        nlist++;

      /* Free the old render table after we update the children. */
      old_render_table = CB_RenderTable(curcb);
    }

  /* Process resources that affect layout. */
  if ((CB_HighlightThickness(curcb) != CB_HighlightThickness(newcb)) ||
      (CB_MarginWidth(curcb) != CB_MarginWidth(newcb)) ||
      (CB_MarginHeight(curcb) != CB_MarginHeight(newcb)) ||
      (CB_RenderTable(curcb) != CB_RenderTable(newcb)))
    {
      resize = dolayout = redisplay = TRUE;
    }

  if (MGR_ShadowThickness(curcb) != MGR_ShadowThickness(newcb))
    {
      resize = dolayout = redisplay = TRUE;

      if (CB_Type(newcb) != XmDROP_DOWN_LIST)
	{
	  XtSetArg(editbox_args[neditbox], XmNshadowThickness,
		   MGR_ShadowThickness(newcb)), neditbox++;
	}

      if (CB_Type(curcb) != XmCOMBO_BOX)
	{
	  XtSetArg(shell_args[nshell], XmNshadowThickness,
		   MGR_ShadowThickness(newcb)), nshell++;
	}
    }

  if ((CB_ArrowSpacing(curcb) != CB_ArrowSpacing(newcb)) ||
      (CB_ArrowSize(curcb) != CB_ArrowSize(newcb)))
    {
      if (CB_Type(curcb) != XmCOMBO_BOX)
	{
	  resize = dolayout = redisplay = TRUE;
	}
    }

  /* Propagate shadow GC changes to the popup list shell. */
  if ((curcb->manager.top_shadow_color !=
       newcb->manager.top_shadow_color) ||
      (curcb->manager.top_shadow_pixmap !=
       newcb->manager.top_shadow_pixmap) ||
      (curcb->manager.bottom_shadow_color !=
       newcb->manager.bottom_shadow_color) ||
      (curcb->manager.bottom_shadow_pixmap !=
       newcb->manager.bottom_shadow_pixmap))
    {
      if (CB_Type(curcb) != XmCOMBO_BOX)
	{
	  XtSetArg(shell_args[nshell], XmNtopShadowColor,
		   newcb->manager.top_shadow_color), nshell++;
	  XtSetArg(shell_args[nshell], XmNbottomShadowColor,
		   newcb->manager.bottom_shadow_color), nshell++;
	  XtSetArg(shell_args[nshell], XmNtopShadowPixmap,
		   newcb->manager.top_shadow_pixmap), nshell++;
	  XtSetArg(shell_args[nshell], XmNbottomShadowPixmap,
		   newcb->manager.bottom_shadow_pixmap), nshell++;
	}
    }

  if (XtBackground(curcb) != XtBackground(newcb))
    {
      if (CB_Type(newcb) != XmCOMBO_BOX)
	{
	  XtReleaseGC((Widget) newcb, newcb->combo_box.arrow_GC);
	  GetArrowGC((Widget) newcb);

	  /* Core won't request redisplay if a background pixmap is used. */
	  redisplay = TRUE;
	}
    }

  if (XtIsSensitive((Widget)curcb) != XtIsSensitive((Widget)newcb))
    {
      redisplay = TRUE;
    }


  /* Push all the accrued updates onto the children. */
  assert(nshell <= XtNumber(shell_args));
  if (nshell && CB_ListShell(newcb))
    XtSetValues(CB_ListShell(newcb), shell_args, nshell);

  assert(nlist <= XtNumber(list_args));
  if (nlist && CB_List(newcb))
    XtSetValues(CB_List(newcb), list_args, nlist);

  assert(neditbox <= XtNumber(editbox_args));
  if (neditbox && CB_EditBox(newcb))
    XtSetValues(CB_EditBox(newcb), editbox_args, neditbox);

  if (old_render_table)
	XmFontListFree(old_render_table);


  /* Recompute our ideal size. */


  if ( resize)
    {
      if (XtIsRealized((Widget)newcb))
          XtWidth(newcb) = XtHeight(newcb) = 0;

      ComputeSize((Widget)newcb, 0, 0, &(XtWidth(newcb)), &(XtHeight(newcb)));
    }

  /*
   * Recompute our layout, if realized.
   * We could check and not resize and add a SetValuesAlmost (like the
   * one in Label) that would call Resize if the parent denies the request.
   */
  if (XtIsRealized((Widget)new_w) && dolayout)
    DoLayout(new_w);

  return redisplay;
}

/* ReduceResources()
 *	Called from ChangeManaged when some resources need to be
 * diminished in order to satisfy a resource request.
 */
static Boolean
ReduceResources(Widget     widget,
		Dimension *width,
		Dimension *height,
		Mask	   flags)
{
  XmComboBoxWidget newcb = (XmComboBoxWidget)widget;
  int delta;

  /* Space is stolen from resources in this order:
   *	MarginHeight, MarginWidth, ArrowSpacing,
   *	ShadowThickness, HighlightThickness
   */

  /* Reduce width.  Try resources in order. */
  if (*width)
    {
      if (!(flags & MARGIN_WIDTH))
	*width -= Reduce(&CB_MarginWidth(newcb), *width, 0);
    }
  if (*width)
    {
      if (!(flags & ARROW_SPACING))
	{
	  assert(CB_Type(newcb) != XmCOMBO_BOX);
	  *width -= Reduce(&CB_ArrowSpacing(newcb), *width,
			    MINIMUM_ARROWSPACE);
	}
    }
  if (*width)
    {
      if (!(flags & SHADOW_THICKNESS))
	{
	  delta = Reduce(&MGR_ShadowThickness(newcb), *width,
			 MINIMUM_SHADOWTHICKNESS);
	  *width -= delta;

	  /* This affects height too. */
	  if (*height)
	    Reduce(height, delta, 0);

	  /* Keep the shell's shadowThickness in sync. */
	  if (CB_Type(newcb) != XmCOMBO_BOX)
	    {
	      Arg      args[1];
	      Cardinal n = 0;

	      XtSetArg(args[n], XmNshadowThickness,
		       MGR_ShadowThickness(newcb)), n++;
	      assert(n <= XtNumber(args));
	      XtSetValues(CB_ListShell(newcb), args, n);
	    }
	}
    }
  if (*width)
    {
      if (!(flags & HIGHLIGHT_THICKNESS))
	{
	  delta = Reduce(&CB_HighlightThickness(newcb), *width,
			 MINIMUM_HIGHLIGHTTHICKNESS);
	  *width -= delta;

	  /* This affects height too. */
	  if (*height)
	    Reduce(height, delta, 0);
	}
    }

  /* Reduce height.  Try resouces in order. */
  if (*height)
    {
      if (!(flags & MARGIN_HEIGHT))
	*height -= Reduce(&CB_MarginHeight(newcb), *height, 0);
    }
  if (*height)
    {
      if (!(flags & SHADOW_THICKNESS))
	*height -= Reduce(&MGR_ShadowThickness(newcb), *height,
			  MINIMUM_SHADOWTHICKNESS);
    }
  if (*height)
    {
      if (!(flags & HIGHLIGHT_THICKNESS))
	*height -= Reduce(&CB_HighlightThickness(newcb), *height,
			  MINIMUM_HIGHLIGHTTHICKNESS);
    }

  return (!*width && !*height);
}

/*
 * Reduce()
 *	A helper for ReduceResources, this routine will decrement a
 * single resource.
 */

static int
Reduce(Dimension *size,
       int	  max_change,
       int	  min_size)
{
  int change = 0;

  if (*size > min_size)
    {
      change = MIN(*size - min_size, max_change);
      *size -= change;
    }

  return change;
}

/*
 * Resize()
 *	Sizes and places the children of the ComboBox depending on its
 * type:  XmCOMBO_BOX, XmDROP_DOWN_COMBO_BOX, or XmDROP_DOWN_LIST.
 * The instrinsics call Expose() immediately after calling this method.
 */

static void
Resize(Widget widget)
{
  /* CR 6911: Need to erase old margins, arrow spacing, and arrow areas. */
  /* CR 7255: Erase old margins too. */
  if (XtIsRealized(widget))
    XClearWindow (XtDisplay(widget), XtWindow(widget));

  DoLayout(widget);
}


static void
CheckMinimalSize(
	  Widget widget,
	  Dimension * pwidth,
	  Dimension * pheight)

{
    XmComboBoxWidget cb = (XmComboBoxWidget) widget;
    Dimension min_height, min_width;

    min_height = min_width =
	2 * (MINIMUM_SHADOWTHICKNESS + MINIMUM_HIGHLIGHTTHICKNESS) + MINTXT;
    if (CB_Type(cb) != XmCOMBO_BOX) {
	if (CB_ArrowSize(cb) == XmINVALID_DIMENSION)
	    CB_ArrowSize(cb) = GetDefaultArrowSize((Widget)cb);
	min_width += MINIMUM_ARROWSPACE + (int)CB_ArrowSize(cb);
    }
    else
	min_height += MINLIST;

    *pwidth = MAX(min_width, *pwidth);
    *pheight = MAX(min_height, *pheight);
}

/*
 * ChangeManaged()
 *	Called whenever there is a change in the managed set.
 */

static void
ChangeManaged(Widget widget)
{
  XmComboBoxWidget cb = (XmComboBoxWidget) widget;
  XtWidgetGeometry desired;
  Dimension thickW, thickH, width, height;
  Dimension widthXcess = 0, heightXcess = 0;

  desired.request_mode = 0;
  if (!XtIsRealized((Widget)cb))
    {
      /* Only attempt to change non-specified sizes. */
      desired.width = XtWidth(cb);   /* might be 0 */
      desired.height = XtHeight(cb); /* might be 0 */
    }
  else
    {
      desired.width = 0;
      desired.height = 0;
    }

  /* Compute our desired size.
   *	If user has set width or height, use it
   *	else use preferred sized of children
   */
  if (desired.width == 0 || desired.height == 0)
      ComputeSize((Widget)cb, 0, 0, &desired.width, &desired.height);
  else
      CheckMinimalSize((Widget)cb, &desired.width, &desired.height);

  /* Make request to parent*/
  desired.request_mode = (CWWidth | CWHeight);
  _XmMakeGeometryRequest((Widget) cb, &desired);

  /* Determine if not enough space due to resources requested */
  GetThickness((Widget) cb, &thickW, &thickH);
  width = 2*thickW + MINTXT;
  height = 2*thickH + MINTXT;
  if (CB_Type(cb) != XmCOMBO_BOX)
    width += CB_ArrowSize(cb) + CB_ArrowSpacing(cb);
  else
    height += MINLIST;

  if (width > XtWidth(cb))
    widthXcess = width - XtWidth(cb);
  if (height > XtHeight(cb))
    heightXcess = height - XtHeight(cb);

  if (widthXcess || heightXcess)
    {
      /* take it off the resources
       * this will reduce each to their minimum value if needed
       */
      if (widthXcess || heightXcess)
	ReduceResources((Widget)cb, &widthXcess, &heightXcess, 0);
    }

  /* Layout the widget */
  DoLayout(widget);
}

/*
 * GeometryManager()
 *	Handle geometry change requests from our children.
 */

static XtGeometryResult
GeometryManager(Widget		  mychild,
		XtWidgetGeometry *request,
		XtWidgetGeometry *reply)
{
    XmComboBoxWidget cb = (XmComboBoxWidget) XtParent(mychild);
    XtWidgetGeometry my_request;
    int width, height ; /* make them int so that we deal with negative */
    XtGeometryResult res ;
    Dimension almost_list_width = 0 ;

    /*
     * Reject anything requests a change of position.  We don't even
     * consider "almosting" what goes with it, i.e. a size request.
     */
    if (request->request_mode & (CWX | CWY))
	return XtGeometryNo ;

    my_request.request_mode = 0;
    width = XtWidth(cb) ;
    height = XtHeight(cb) ;

    /*
     * Try to honor a child's request to change size by passing the
     * decision to our parent.  For now we don't compromise or check
     * the other child's preferred size, only our own minimal size.
     */

    if (request->request_mode & XtCWQueryOnly)
	my_request.request_mode |= XtCWQueryOnly;

    if (request->request_mode & CWWidth) {
	my_request.request_mode |= CWWidth;
	width += (int)request->width - (int)XtWidth(mychild);
	if (mychild == CB_EditBox(cb))
	    cb->combo_box.ideal_ebwidth = request->width;
	else {
	    /*
	     * Compromise rather than letting the list shrink smaller
	     * than the Text's preferred width.
	     */
	    Dimension thickW, thickH;

	    GetThickness((Widget)cb, &thickW, &thickH);
	    if (!cb->combo_box.ideal_ebwidth)
		GetIdealTextSize((Widget)cb, &cb->combo_box.ideal_ebwidth,
				 NULL);
	    if (width < cb->combo_box.ideal_ebwidth
		+ 2 * (thickW + XtBorderWidth(CB_EditBox(cb)))) {
		width = cb->combo_box.ideal_ebwidth
		    + 2 * (thickW + XtBorderWidth(CB_EditBox(cb))) ;
		almost_list_width = cb->combo_box.ideal_ebwidth ;
		my_request.request_mode |= XtCWQueryOnly;
	    }
	}
    }

    if (request->request_mode & CWHeight) {
	my_request.request_mode |= CWHeight;
	height += (int)request->height - (int)XtHeight(mychild);
	if (mychild == CB_EditBox(cb))
	    cb->combo_box.ideal_ebheight = request->height;
    }

    if (request->request_mode & CWBorderWidth) {
	my_request.request_mode |= (CWWidth | CWHeight);
	width += 2*((int)request->border_width - (int)XtBorderWidth(mychild));
	height += 2*((int)request->border_width - (int)XtBorderWidth(mychild));
    }

    if (width > 0) my_request.width = (Dimension) width ;
    if (height > 0) my_request.height = (Dimension) height ;

    CheckMinimalSize((Widget)cb, &my_request.width, &my_request.height);

    res = XtMakeGeometryRequest((Widget)cb, &my_request, NULL);
    if (res == XtGeometryYes) {
	if (!(my_request.request_mode & XtCWQueryOnly))	    {
	    XtWidgetProc resize;
	    if (request->request_mode & CWWidth)
		XtWidth(mychild) = request->width;
	    if (request->request_mode & CWHeight)
		XtHeight(mychild) = request->height;
	    if (request->request_mode & CWBorderWidth)
		XtBorderWidth(mychild) = request->border_width;

	    _XmProcessLock();
	    resize = XtCoreProc(cb, resize);
	    _XmProcessUnlock();
	    (*resize)((Widget) cb) ;
	} else
        if (almost_list_width) {
	    reply->request_mode = request->request_mode ;
	    reply->width = almost_list_width ;
	    reply->height = request->height ;
	    reply->border_width = request->border_width ;
	    return XtGeometryAlmost ;
	}
	return XtGeometryYes;
    } else
	return XtGeometryNo;
}



/*
 * Redisplay()
 *	General redisplay function called on exposure events.
 */
static void
Redisplay(Widget widget,
	  XEvent *event,	/* unused */
	  Region region)	/* unused */
{
  XmComboBoxWidget cb = (XmComboBoxWidget) widget;

  if (XtIsRealized(widget))
    {
      if (CB_Type(cb) != XmCOMBO_BOX)
	DrawArrow(widget, CB_ArrowPressed(cb));
      DrawShadows(widget);

      /* CR 6356: Refresh the highlight border too. */
      if (CB_Highlighted(cb))
	HighlightBorder(widget);
      else
	UnhighlightBorder(widget);
    }
}

/*
 * Destroy()
 *	Called by the Intrinsics when a ComboBox widget is destroyed.
 */

static void
Destroy(Widget widget)
{
  XmComboBoxWidget cb = (XmComboBoxWidget) widget;
  Widget           ancestor;

  if (CB_Type(widget) != XmCOMBO_BOX)
    XtRemoveEventHandler(widget, POPUP_EVENT_MASK, FALSE,
			 PopupEH, (XtPointer)widget);

  /* The remove focus moved callback from the vendor shell. */
  ancestor = widget;
  while (ancestor && !XmIsVendorShell(ancestor))
    ancestor = XtParent(ancestor);

  if (ancestor && !ancestor->core.being_destroyed)
    XmeRemoveFocusChangeCallback(ancestor, FocusMovedCB, (XtPointer) widget);

  if (cb->combo_box.arrow_GC)
    XtReleaseGC(widget, cb->combo_box.arrow_GC);

  XmFontListFree(CB_RenderTable(cb));
}

/*
 * QueryGeometry()
 */

static XtGeometryResult
QueryGeometry(Widget		widget,
	      XtWidgetGeometry *intended,
	      XtWidgetGeometry *desired)
{
  /* Deal with user initial size setting. */
  if (!XtIsRealized(widget))
    {
      desired->width = XtWidth(widget);    /* might be 0 */
      desired->height = XtHeight(widget);  /* might be 0 */
    }
  else
    {
      /* Always computes natural size afterwards. */
      desired->width = 0;
      desired->height = 0;
    }

  ComputeSize(widget, 0, 0, &desired->width, &desired->height);

  /* This function will set CWidth and CHeight. */
  return XmeReplyToQueryGeometry(widget, intended, desired);
}

/*
 * ConstraintDestroy()
 */

static void
ConstraintDestroy(Widget child)
{
  XmComboBoxWidget cb;

  if (!XtIsRectObj (child))
    return;

  cb = (XmComboBoxWidget) XtParent(child);

  if (child == CB_EditBox(cb))
    {
      CB_EditBox(cb) = NULL;
    }
  else if (child == CB_ScrolledW(cb))
    {
      CB_ScrolledW(cb) = NULL;
      CB_List(cb) = NULL;
      cb->combo_box.vsb = NULL;
      cb->combo_box.hsb = NULL;
    }
  else if (child == CB_ListShell(cb))
    {
      CB_ListShell(cb) = NULL;
      CB_ScrolledW(cb) = NULL;
      CB_List(cb) = NULL;
      cb->combo_box.vsb = NULL;
      cb->combo_box.hsb = NULL;
    }
}

/*
 * ComboBoxParentProcess()
 */

static Boolean
ComboBoxParentProcess(Widget		  wid,
		      XmParentProcessData event)
{
  XmComboBoxWidget     cb = (XmComboBoxWidget) wid;
  XmGrabShellWidget    gs = NULL;
  Boolean 	       cont = TRUE;
  int		       count = 0;
  Cardinal 	       n = 0;
  Arg		       args[2];

  if ((event->any.process_type == XmINPUT_ACTION) &&
      ((event->input_action.action == XmPARENT_ACTIVATE)  ||
       (event->input_action.action == XmPARENT_CANCEL)))
    {
      if (CB_Type(cb) != XmCOMBO_BOX)
	{
	  gs = (XmGrabShellWidget)CB_ListShell(cb);
	  if (gs && (CB_ShellState(cb) == POPPED_UP))
	    {
	      PopdownList((Widget)cb, event->input_action.event);
	      CBDisarm((Widget)cb, event->input_action.event, NULL, NULL);
	      cont = FALSE;
	    }
	}

      if (event->input_action.action == XmPARENT_ACTIVATE)
        {
	  /*
	   * If the event occurred in the text widget and the popup shell
	   * is up, call the defaultAction callbacks on the List.
	   * We can't call the action routine for List that will do this
	   * because it will pass the Activate back to us and we'll end
	   * up looping.  Only call them if there are items in the list -
	   * this is what list does in KbdActivate().
	   */

	  /* CR 9484: Always compute pos. */
	  XmString item = GetEditBoxValue((Widget) cb);
	  int pos = XmListItemPos(CB_List(cb), item);

	  n = 0;
	  XtSetArg(args[n], XmNitemCount, &count), n++;
	  assert(n <= XtNumber(args));
	  XtGetValues(CB_List(cb), args, n);

	  if (((CB_Type(cb) == XmCOMBO_BOX) || !cont) && count)
	    {
	      XmListCallbackStruct call_data;

	      bzero((char*) &call_data, sizeof(XmListCallbackStruct));
	      if (pos)
		{
		  call_data.item		    = XmStringCopy(item);
		  call_data.item_length             = XmStringLength(item);
		  call_data.item_position	    = pos;
		  call_data.selected_item_count     = 1;
		  call_data.selected_item_positions = &pos;
		  call_data.selected_items	    = &item;
		}
	      call_data.reason = XmCR_DEFAULT_ACTION;
	      call_data.event = event->input_action.event;

	      XtCallCallbacks(CB_List(cb), XmNdefaultActionCallback,
			      (XtPointer)&call_data);

	      XmStringFree(call_data.item);
	    }

	  XmStringFree(item);

	  /* CR 6552: Fixup the list selection. */
	  if (pos)
	    XmListSelectPos(CB_List(cb), pos, FALSE);
	  else
	    XmListDeselectAllItems(CB_List(cb));

	  /* Call the selection callbacks. */
	  CallSelectionCallbacks((Widget)cb, event->input_action.event);
	}
    }

  if (cont)
    return _XmParentProcess(XtParent(cb), event);
  else
    return True;
}

/* ------------- CALLBACKS ---------- */


/*
 * TextChangedCB()
 *	Callback function invoked when the edit box value is changed.
 */
static void
TextChangedCB(Widget    widget,	/* unused */
	      XtPointer client_data,
	      XtPointer call_data) /* unused */
{
  XmComboBoxWidget cb = (XmComboBoxWidget)client_data;

  /* Remember to generate a selection callback when focus moves. */
  CB_TextChanged(cb) = TRUE;
}

/*
 * ListSelectionCB()
 *	Callback function called when an item is selected from the list.
 */
static void
ListSelectionCB(Widget widget,	/* unused */
		XtPointer client_data,
		XtPointer call_data)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)client_data;
  XmString item;
  int top, vis_items;
  Cardinal n;
  Arg args[3];
  XmListCallbackStruct *cb_data = (XmListCallbackStruct *)call_data;

  if (!CB_EditBox(cb))
    {
      XmeWarning((Widget)cb, MISSINGKID);
      return;
    }

  /* If the EditBox does not contain the selected text, set it. */
  item = GetEditBoxValue((Widget) cb);
  if (! XmStringCompare(item, cb_data->item))
    SetEditBoxValue((Widget) cb, cb_data->item);
  XmStringFree(item);

  /* If the selected item is not viewable, scroll the list. */
  n = 0;
  XtSetArg(args[n], XmNtopItemPosition, &top), n++;
  XtSetArg(args[n], XmNvisibleItemCount, &vis_items), n++;
  assert(n <= XtNumber(args));
  XtGetValues(CB_List(cb), args, n);

  if ((cb_data->item_position < top) ||
      (cb_data->item_position >= (top + vis_items)))
    XmListSetBottomItem(CB_List(cb), cb_data->item);

  CallSelectionCallbacks((Widget)cb, cb_data->event);

  if (cb_data->event &&
      (cb_data->event->type == ButtonPress ||
       cb_data->event->type == ButtonRelease))
    {
      if (CB_Type(cb) != XmCOMBO_BOX)
	{
	  PopdownList((Widget)cb, cb_data->event);

	  /* CR 6147: If we drag straight from the arrow to a list */
	  /*	item we won't get a normal disarm callback. */
	  CBDisarm((Widget)cb, cb_data->event, NULL, NULL);
	}
    }
}

/*
 * ShellPopupCB()
 *	Called when the grab shell is posted.  Fixup focus.
 */
static void
ShellPopupCB(Widget    widget,	/* unused */
	     XtPointer client_data,
	     XtPointer call_data) /* unused */
{
  XmComboBoxWidget cb = (XmComboBoxWidget)client_data;

  CB_ShellState(cb) = POPPED_UP;

  (void) XmProcessTraversal(CB_List(cb), XmTRAVERSE_CURRENT);
}

/*
 * ShellPopdownCB()
 *	Called when the grab shell is unposted.
 */
static void
ShellPopdownCB(Widget    widget,    /* unused */
	       XtPointer client_data,
	       XtPointer call_data) /* unused */
{
  XmComboBoxWidget cb = (XmComboBoxWidget)client_data;
  XmDisplay disp = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
  Window old_focus;
  int old_revert;

  /* CR 9887: List may not have seen the BtnUp event... */
  XtCallActionProc(CB_List(cb), "ListKbdCancel", NULL, NULL, 0);

  /* Re-enable drag and drop. */
  disp->display.userGrabbed = False;

  CB_ShellState(cb) = POPPED_DOWN;
  cb->combo_box.scrolling = FALSE;

  /* In click-to-type mode we can't lose focus, and won't get a */
  /*	focus-in event when the grabshell pops down. */
  XGetInputFocus(XtDisplay(cb), &old_focus, &old_revert);
  if (old_revert != RevertToParent)
    {
      CBFocusOut((Widget)cb, NULL, NULL, NULL);

      /* CR 7122: Tell the edit box to stop blinking now. */
      if (CB_Type(cb) == XmDROP_DOWN_COMBO_BOX)
	{
	  XEvent focus_event;

	  focus_event.xfocus.type = FocusOut;
	  focus_event.xfocus.send_event = True;
	  XtCallActionProc(CB_EditBox(cb), "focusOut", &focus_event, NULL, 0);
	}
    }
}

/*
 * FocusMovedCB()
 *	Called from VendorShell every time focus moves.
 */
static void
FocusMovedCB(Widget    widget,	/* unused */
	     XtPointer client_data,
	     XtPointer call_data)
{
  XmFocusMovedCallbackStruct *callback =
    (XmFocusMovedCallbackStruct *) call_data;
  XmComboBoxWidget cb = (XmComboBoxWidget) client_data;
  Boolean have_focus, getting_focus;

  /* Some other focus-moved callback has aborted this shift. */
  if (!callback->cont)
    return;

  have_focus = CB_Highlighted(cb);
  getting_focus = (((callback->new_focus == NULL) &&
		    (CB_ShellState(cb) != POPPED_DOWN)) ||
		   (callback->new_focus == (Widget)cb) ||
		   (callback->new_focus == CB_EditBox(cb)) ||
		   (callback->new_focus == CB_ScrolledW(cb)) ||
		   (callback->new_focus == CB_List(cb)) ||
		   ((cb->combo_box.hsb != NULL) &&
		    (callback->new_focus == cb->combo_box.hsb)) ||
		   ((cb->combo_box.vsb != NULL) &&
		    (callback->new_focus == cb->combo_box.vsb)));

  /* CR 9868: In XmPOINTER mode focus goes to weird places. */
  if (!getting_focus && (_XmGetFocusPolicy((Widget) cb) == XmPOINTER))
    {
      Window root, child;
      int root_x, root_y, win_x, win_y;
      unsigned int mod_mask;

      if (CB_ShellState(cb) == POPPED_UP)
	getting_focus = TRUE;
      else if ((callback->new_focus == XtParent(cb)) &&
	       XQueryPointer(XtDisplay(cb), XtWindow(cb), &root, &child,
			     &root_x, &root_y, &win_x, &win_y, &mod_mask) &&
	       (win_x >= 0) && (win_x < XtWidth(cb)) &&
	       (win_y >= 0) && (win_y < XtHeight(cb)))
	getting_focus = TRUE;
    }

  if (have_focus && !getting_focus)
    {
      CBFocusOut((Widget)cb, callback->event, NULL, NULL);

      /* Is this necessary? */
      if (CB_ShellState(cb) == POPPED_UP)
	{
	  PopdownList((Widget)cb, callback->event);
	  CBDisarm((Widget)cb, callback->event, NULL, NULL);
	}
    }
  else if (!have_focus && getting_focus)
    {
      CBFocusIn((Widget)cb, callback->event, NULL, NULL);
    }
}

/* ------------- ACTION ROUTINES ---------- */


/*
 * CBArmAndDropDownList()
 *	Handle button down action in widget.  First determine if hit
 * is in an arrow, otherwise ignore the event.  If it is in an arrow,
 * then supply visual feedback and perform the appropriate action.
 */
static void
CBArmAndDropDownList(Widget widget,
		     XEvent *event,
		     String *params,		/* unused */
		     Cardinal *num_params)	/* unused */
{
  XmComboBoxWidget cb = FindComboBox(widget);
  XmGrabShellWidget gs = (XmGrabShellWidget)CB_ListShell(cb);

  /* Return if this is a replay of the unpost event. */
  if (gs && (event->xbutton.time == gs->grab_shell.unpost_time))
    return;

  /* Ignore the event if this is a replay */
  if (! _XmIsEventUnique(event))
    return;

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  /* CR 9833: Attempt to grab focus. */
  XmProcessTraversal((Widget)cb, XmTRAVERSE_CURRENT);

  if ((CB_Type(cb) != XmCOMBO_BOX) &&
      Hit((XButtonEvent *)event, CB_HitRect(cb)))
    {
      CB_ArrowPressed(cb) = TRUE;
      DrawArrow((Widget)cb, CB_ArrowPressed(cb));
      CBDropDownList((Widget)cb, event, NULL, NULL);
    }
}

/*
 * CBDisarm()
 *	Handle button up action and undo CBArmAndDropDownList.
 */
static void
CBDisarm(Widget widget,
	 XEvent *event,		/* unused */
	 String *params,	/* unused */
	 Cardinal *num_params)	/* unused */
{
  XmComboBoxWidget cb = FindComboBox(widget);

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  if (CB_Type(cb) != XmCOMBO_BOX)
    {
      if (CB_ArrowPressed(cb))
	{
	  CB_ArrowPressed(cb) = FALSE;
	  DrawArrow((Widget)cb, CB_ArrowPressed(cb));
	}
    }
}

/*
 * CBDropDownList()
 *	Action to post/unpost the drop down list.
 */

static void
CBDropDownList(Widget    widget,
	       XEvent   *event,
	       String   *params,
	       Cardinal *num_params)
{
  XmComboBoxWidget cb = FindComboBox(widget);

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  if ((CB_Type(cb) != XmCOMBO_BOX))
    {
      XmGrabShellWidget gs = (XmGrabShellWidget)CB_ListShell(cb);
      if (gs && (CB_ShellState(cb) == POPPED_DOWN))
	{
	  XmDisplay disp = (XmDisplay) XmGetDisplay(widget);
	  Arg args[3];
	  Cardinal n;
	  int tmp;
	  Position root_x, root_y, shell_x, shell_y;
	  Dimension shell_width;

	  XtTranslateCoords((Widget)cb, XtX(cb), XtY(cb), &root_x, &root_y);

	  shell_x = root_x - XtX(cb) + CB_HighlightThickness(cb) -
	    XtBorderWidth(CB_ListShell(cb));
	  shell_y = root_y + XtHeight(cb) - CB_HighlightThickness(cb) -
	    XtY(cb);

	  /* Try to position the shell on the screen. */
	  tmp = WidthOfScreen(XtScreen(cb)) - XtWidth(CB_ListShell(cb));
	  tmp = MIN(tmp, shell_x);
	  shell_x = MAX(0, tmp);
	  tmp = HeightOfScreen(XtScreen(cb)) - XtHeight(CB_ListShell(cb));
	  tmp = MIN(tmp, shell_y);
	  shell_y = MAX(0, tmp);

	  /* CR 8446: The shell width may have changed unexpectedly. */
	  shell_width = XtWidth(cb) - 2 * CB_HighlightThickness(cb);

	  n = 0;
	  XtSetArg(args[n], XmNx, shell_x), n++;
	  XtSetArg(args[n], XmNy, shell_y), n++;
	  XtSetArg(args[n], XmNwidth, shell_width), n++;
	  assert(n <= XtNumber(args));
	  XtSetValues(CB_ListShell(cb), args, n);

	  CB_ShellState(cb) = POPPING_UP;
	  cb->combo_box.scrolling = FALSE;

	  /* Don't let drag and drop confuse things. */
	  disp->display.userGrabbed = True;

	  /* Record the post time */
	  gs->grab_shell.post_time = event->xbutton.time;

	  /* Record the event to prevent popdown on replay */
	  _XmRecordEvent(event);
	  _XmPopupSpringLoaded(CB_ListShell(cb));
	}
      else /* shell is popped up */
	{
	  PopdownList((Widget)cb, event);
	  CBDisarm((Widget)cb, event, params, num_params);
	}
    }
}

/*
 * CBFocusIn()
 *	Action routine to draw focus highlighting.
 */
static void
CBFocusIn(Widget    widget,
	  XEvent   *event,	/* unused */
	  String   *params,	/* unused */
	  Cardinal *num_params)	/* unused */
{
  XmComboBoxWidget cb = FindComboBox(widget);

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  HighlightBorder((Widget)cb);
}

/*
 * CBFocusOut()
 *	Action routine to erase focus highlighting.
 */
static void
CBFocusOut(Widget    widget,
	   XEvent   *event,
	   String   *params,	 /* unused */
	   Cardinal *num_params) /* unused */
{
  XmComboBoxWidget cb = FindComboBox(widget);

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  UnhighlightBorder((Widget)cb);

  if (CB_TextChanged(cb))
    CallSelectionCallbacks((Widget)cb, event);
}

/*
 * CBTextFocusOut()
 *	Action routine to fake the text field cursor into blinking.
 */
static void
CBTextFocusOut(Widget    widget,
	       XEvent   *event,
	       String   *params,
	       Cardinal *num_params)
{
  XmComboBoxWidget cb = FindComboBox(widget);

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  /* CR 7122: Suppress text focus-out events when the grab shell is */
  /*	posted, so that the insertion cursor will continue to blink. */
  if ((CB_Type(cb) != XmDROP_DOWN_COMBO_BOX) ||
      (CB_ShellState(cb) != POPPED_UP))
    {
      XtCallActionProc(CB_EditBox(cb), "focusOut", event, params,
		       (num_params ? *num_params : 0));
    }
}

/*
 * CBActivate()
 *	Action routine called when the list is activated.
 */

static void
CBActivate(Widget    widget,
	   XEvent   *event,
	   String   *params,
	   Cardinal *num_params)
{
  XmComboBoxWidget cb = FindComboBox(widget);
  XmParentInputActionRec p_event;

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  p_event.process_type = XmINPUT_ACTION;
  p_event.action       = XmPARENT_ACTIVATE;
  p_event.event        = event;
  p_event.params       = params;
  p_event.num_params   = num_params;

  ComboBoxParentProcess((Widget)cb, (XmParentProcessData) &p_event);
}

/*
 * CBCancel()
 *	Action invoked from the drop down list.
 */

static void
CBCancel(Widget    widget,
	 XEvent   *event,
	 String   *params,
	 Cardinal *num_params)
{
  XmComboBoxWidget cb = FindComboBox(widget);
  XmParentInputActionRec p_event;

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  p_event.process_type = XmINPUT_ACTION;
  p_event.action       = XmPARENT_CANCEL;
  p_event.event        = event;
  p_event.params       = params;
  p_event.num_params   = num_params;

  ComboBoxParentProcess((Widget)cb, (XmParentProcessData) &p_event);
}

/*
 * CBListAction()
 *	Generic action to perform operations on the list.
 */

static void
CBListAction(Widget    widget,
	     XEvent   *event,
	     String   *params,
	     Cardinal *num_params)
{
  /* This enum matches the order of the string constants in RepType.c */
  enum { UP, DOWN, PREVPAGE, NEXTPAGE, BEGINDATA, ENDDATA };

  XmComboBoxWidget cb = FindComboBox(widget);
  int direction;

  if (!cb)
    {
      XmeWarning((Widget)cb, WRONGWIDGET);
      return;
    }

  if (!num_params || (*num_params != 1) || !params)
    {
      XmeWarning((Widget) cb, WRONGPARAMS);
      return;
    }

  if (_XmConvertActionParamToRepTypeId
          ((Widget) cb, XmRID_COMBO_BOX_LIST_ACTION_ACTION_PARAMS,
	   params[0], False, &direction) == False)
    {
      /* Unknown value.  A warning should already have been printed. */
      return;
    }

  switch (direction)
    {
    case UP:
    case DOWN:
      {
	int *pos, count, num_items;
	Cardinal n;
	Arg args[3];

	/* Find the current number of items and selected position. */
	n = 0;
	XtSetArg(args[n], XmNitemCount, &num_items),	     n++;
	XtSetArg(args[n], XmNselectedPositions, &pos),	     n++;
	XtSetArg(args[n], XmNselectedPositionCount, &count), n++;
	assert(n <= XtNumber(args));
	XtGetValues(CB_List(cb), args, n);

	if (count)
	  {
	    switch (direction)
	      {
	      case UP:
		if (*pos >= 1)
		  XmListSelectPos(CB_List(cb), *pos - 1, TRUE);
		break;

	      case DOWN:
		if (*pos < num_items)
		  XmListSelectPos(CB_List(cb), *pos + 1, TRUE);
		else if (*pos == num_items)
		  XmListSelectPos(CB_List(cb), 1, TRUE);
		break;

	      default:
		assert(FALSE);
	      }
	  }
	else if (num_items)
	  {
	    XmListSelectPos(CB_List(cb), 1, TRUE);
	  }
      }
      break;

    case PREVPAGE:
      if ((CB_Type(cb) == XmCOMBO_BOX) ||
	  (CB_ShellState(cb) == POPPED_UP))
	XtCallActionProc(CB_List(cb), "ListPrevPage", event, NULL, 0);
      break;

    case NEXTPAGE:
      if ((CB_Type(cb) == XmCOMBO_BOX) ||
	  (CB_ShellState(cb) == POPPED_UP))
	XtCallActionProc(CB_List(cb), "ListNextPage", event, NULL, 0);
      break;

    case BEGINDATA:
      XtCallActionProc(CB_List(cb), "ListBeginData", event, NULL, 0);
      break;

    case ENDDATA:
      XtCallActionProc(CB_List(cb), "ListEndData", event, NULL, 0);
      break;

    default:
      assert(FALSE);
    }
}

/*
 * PopdownList()
 *	Internal utility to unpost the grabshell.
 */

static Boolean
PopdownList(Widget cb,
	    XEvent *event)
{
  Widget gs = CB_ListShell(cb);

  /* Popping down while in the process of popping up causes X errors. */
  if (gs && XmIsGrabShell(gs) && (CB_ShellState(cb) == POPPED_UP))
    {
      CB_ShellState(cb) = POPPING_DOWN;

      XtCallActionProc(gs, "GrabShellPopdown", event, NULL, 0);
      return TRUE;
    }

  return FALSE;
}

/* ------------- EVENT HANDLERS ---------- */


/*
 * PopupEH()
 *	An XtEventHandler for the popup shell (the drop-down list).
 */
static void
PopupEH(Widget    widget,	/* unused */
	XtPointer client_data,
	XEvent   *event,
	Boolean  *dispatch)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)client_data;

  switch (event->type)
    {
    case ButtonRelease:
      CBDisarm((Widget)cb, event, NULL, NULL);

      /* CR 9899: Only discard matched pairs of scrollbar button events. */
      /*	Should combo_box.scrolling be a counter??? */
      if (cb->combo_box.scrolling)
	*dispatch = cb->combo_box.scrolling = FALSE;
      break;

    case ButtonPress:
      /* Press & release in the scrollbar shouldn't popdown the list. */
      if ((cb->combo_box.vsb &&
	   XtIsRealized(cb->combo_box.vsb) &&
	   (event->xbutton.window == XtWindow(cb->combo_box.vsb))) ||
	  (cb->combo_box.hsb &&
	   XtIsRealized(cb->combo_box.hsb) &&
	   (event->xbutton.window == XtWindow(cb->combo_box.hsb))))
	cb->combo_box.scrolling = TRUE;
      break;

    case EnterNotify:
      if (CB_ArrowPressed(cb))
	XtCallActionProc(CB_List(cb), "ListBeginSelect", event, NULL, 0);
      break;

    default:
      /* This shouldn't happen. */
      break;
    }
}

/*
 * CR 6925: The following two event handlers are used to coordinate
 * grabs between the scrollbar and the grab shell in dropdown lists.
 * In this case the existing active grab started by the grab shell
 * will interfere with the passive grab started by X when the user
 * presses a button within the scrollbar.
 *
 * To deal with the problem, SBBtnDownEH will do an XtGrabPointer
 * to transfer the grab to the scrollbar and SBBtnUpEH will cause
 * the grab to return to the grab shell.
 */
static void
SBBtnDownEH(Widget    w,
	    XtPointer client_data,
	    XEvent   *event,
	    Boolean  *cont)	/* unused */
{
  XmGrabShellWidget shell = (XmGrabShellWidget) client_data;

  XtGrabPointer(w, False, Events | PointerMotionMask | ButtonMotionMask,
		GrabModeAsync, GrabModeAsync,
		None, shell->grab_shell.cursor, event->xbutton.time);
}

static void
SBBtnUpEH(Widget    w,		/* unused */
	  XtPointer client_data,
	  XEvent   *event,
	  Boolean  *cont)	/* unused */
{
  XmGrabShellWidget shell = (XmGrabShellWidget) client_data;

  /* Note that this regrab to the grab shell will need to be changed
   * if the kind of grab that the grabshell imposes changes.
   */
  XtGrabPointer((Widget) shell, shell->grab_shell.owner_events,
		Events,
		shell->grab_shell.grab_style, GrabModeAsync,
		None, shell->grab_shell.cursor, event->xbutton.time);
  if (shell->grab_shell.grab_style == GrabModeSync)
    XAllowEvents(XtDisplay(shell), SyncPointer, event->xbutton.time);
}

/*
 * FindComboBox()
 *	An internal utility routine to traverse up the widget
 * hierarchy until a ComboBox is found.
 */

static XmComboBoxWidget
FindComboBox(Widget widget)
{
  Widget cb = widget;

  while (cb && !XmIsComboBox(cb))
    cb = XtParent(cb);

  return (XmComboBoxWidget) cb;
}

/*
 * CallSelectionCallbacks()
 *	Utility routine to invoke the ComboBox selection callback.
 */

static void
CallSelectionCallbacks(Widget  widget,
		       XEvent *event)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  XmComboBoxCallbackStruct call_data;
  XmString item;
  int pos;

  /* The user knows about this change. */
  CB_TextChanged(cb) = FALSE;

  item = GetEditBoxValue((Widget) cb);

  /* Implement bogus zero-based positions for DtComboBox compatibility. */
  pos = XmListItemPos(CB_List(cb), item);
  if ((CB_PositionMode(cb) == XmZERO_BASED) && (pos > 0))
    --pos;

  /* Call callback list */
  call_data.item_or_text  = item;
  call_data.item_position = pos;
  call_data.reason        = XmCR_SELECT;
  call_data.event         = event;
  XtCallCallbackList((Widget)cb, CB_SelectionCB(cb), (XtPointer)&call_data);

  XmStringFree(item);
}

/* ------------- DEFAULT RESOURCE VALUE CALLPROCS ---------- */

/*
 * XmRCallProc routine for checking font before setting it to NULL
 * if no value is specified for both XmNrenderTable and XmNfontList.
 * I'm usurping the text_changed field to act as a flag to indicate that
 * this function has been called twice on same widget -> implying that
 * resource needs to be set NULL, otherwise leave it alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)wid;
  if (cb->combo_box.text_changed) /* Already been here once, so set resource = NULL */
	value->addr = NULL;
  else {
	cb->combo_box.text_changed = True;
	value->addr = (char*) &CB_RenderTable(cb);
  }
}


/* ------------- SYNTHETIC RESOURCE ACCESS METHODS ------------- */


/*
 * CBSetSelectedItem()
 *	A synthetic resource import procedure.  This procedure can get
 * called at create time BEFORE ComboBox's Initialize() has been
 * called and children have been created.  Make sure children have
 * been created before doing anything.
 */
static XmImportOperator
CBSetSelectedItem(Widget    widget,
		  int       offset, /* unused */
		  XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  XmString new_value = (XmString) *value;
  int pos = 0;

  if (!cb->composite.num_children)
    return XmSYNTHETIC_NONE;

  pos = XmListItemPos(CB_List(cb), new_value);
  if (pos > 0)
    {
      XmListSelectPos(CB_List(cb), pos, TRUE);
    }
  else
    {
      XmString item = GetEditBoxValue((Widget) cb);
      if (! XmStringCompare(item, new_value))
	{
	  XmListDeselectAllItems(CB_List(cb));
	  SetEditBoxValue((Widget) cb, new_value);
	}
      XmStringFree(item);
    }

  return XmSYNTHETIC_NONE;
}

/*
 * CBGetSelectedItem()
 *	A synthetic resource export procedure.
 */
static void
CBGetSelectedItem(Widget    widget,
		  int       offset, /* unused */
		  XtArgVal *value)
{
  *value = (XtArgVal) GetEditBoxValue(widget);
}

/*
 * CBSetSelectedPos()
 *	A synthetic resource import procedure.
 */
static XmImportOperator
CBSetSelectedPos(Widget    widget,
		 int       offset, /* unused */
		 XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int *selPosns = NULL, curpos = 0;
  Cardinal n;
  Arg args[3];
  int new_pos;

  /*
   * This procedure can get called at create time BEFORE ComboBox's
   * Initialize() has been called and children have been created.
   * Make sure children have been created before doing anything.
   */
  if (!cb->composite.num_children)
    return XmSYNTHETIC_NONE;

  /* Get current (1-based) list selectedPos */
  n = 0;
  XtSetArg(args[n], XmNselectedPositions, &selPosns), n++;
  assert(n <= XtNumber(args));
  XtGetValues(CB_List(cb), args, n);
  if (selPosns)
    curpos = *selPosns;

  /* Implement bogus zero-based positions for DtComboBox compatibility. */
  new_pos = (int)*value;
  if (CB_PositionMode(cb) == XmZERO_BASED)
    new_pos++;

  if (curpos != new_pos)
    XmListSelectPos(CB_List(cb), new_pos, TRUE);

  return XmSYNTHETIC_NONE;
}

/*
 * CBGetSelectedPos()
 *	A synthetic resource export procedure.
 */
static void
CBGetSelectedPos(Widget    widget,
		 int       offset, /* unused */
		 XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[2];
  Cardinal nargs;
  int *pos, count;
  int result;

  /* Using XmListGetSelectedPos would copy the positions array. */
  nargs = 0;
  XtSetArg(args[nargs], XmNselectedPositions, &pos),		nargs++;
  XtSetArg(args[nargs], XmNselectedPositionCount, &count),	nargs++;
  assert(nargs <= XtNumber(args));
  XtGetValues(CB_List(cb), args, nargs);

  /* Implement bogus zero-based positions for DtComboBox compatibility. */
  result = (count > 0) ? *pos : 0;
  if ((CB_PositionMode(cb) == XmZERO_BASED) && (result > 0))
    --result;

  *value = result;
}

/*
 * CBGetColumns()
 *	A synthetic resource export procedure.
 */
static void
CBGetColumns(Widget    widget,
	     int       offset,	/* unused */
	     XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[1];
  Cardinal nargs;
  short columns = 0;

  /* Fetch the value from the child widget. */
  if (CB_EditBox(cb))
    {
      nargs = 0;
      XtSetArg(args[nargs], XmNcolumns, &columns),	nargs++;
      assert(nargs <= XtNumber(args));
      XtGetValues(CB_EditBox(cb), args, nargs);
    }

  *value = (XtArgVal)columns;
}

/*
 * CBGetItems()
 *	A synthetic resource export procedure.
 */
static void
CBGetItems(Widget    widget,
	   int       offset,	/* unused */
	   XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[1];
  Cardinal nargs;
  XmStringTable items = NULL;

  /* Fetch the value from the child widget. */
  if (CB_List(cb))
    {
      nargs = 0;
      XtSetArg(args[nargs], XmNitems, &items),	nargs++;
      assert(nargs <= XtNumber(args));
      XtGetValues(CB_List(cb), args, nargs);
    }

  *value = (XtArgVal)items;
}

/*
 * CBGetItemCount()
 *	A synthetic resource export procedure.
 */
static void
CBGetItemCount(Widget    widget,
	       int       offset, /* unused */
	       XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[1];
  Cardinal nargs;
  int count = 0;

  /* Fetch the value from the child widget. */
  if (CB_List(cb))
    {
      nargs = 0;
      XtSetArg(args[nargs], XmNitemCount, &count),	nargs++;
      assert(nargs <= XtNumber(args));
      XtGetValues(CB_List(cb), args, nargs);
    }

  *value = (XtArgVal)count;
}

/*
 * CBGetVisibleItemCount()
 *	A synthetic resource export procedure.
 */
static void
CBGetVisibleItemCount(Widget    widget,
		      int       offset, /* unused */
		      XtArgVal *value)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[1];
  Cardinal nargs;
  int viz_count = 0;

  /* Fetch the value from the child widget. */
  if (CB_List(cb))
    {
      nargs = 0;
      XtSetArg(args[nargs], XmNvisibleItemCount, &viz_count),	nargs++;
      assert(nargs <= XtNumber(args));
      XtGetValues(CB_List(cb), args, nargs);
    }

  *value = (XtArgVal)viz_count;
}

/* ------------- ADDITIONAL FUNCTIONS ---------- */


/*
 * HighlightBorder()
 */

static void
HighlightBorder(Widget w)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;

  CB_Highlighted(cb) = TRUE;

  if ((XtWidth(cb) == 0) ||
      (XtHeight(cb) == 0) ||
      (CB_HighlightThickness(cb) == 0))
    return;

  XmeDrawHighlight(XtDisplay(cb), XtWindow(cb), cb->manager.highlight_GC, 0, 0,
		   XtWidth(cb), XtHeight(cb), CB_HighlightThickness(cb));
}

/*
 * UnhighlightBorder()
 */

static void
UnhighlightBorder(Widget w)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;

  CB_Highlighted(cb) = FALSE;

  if ((XtWidth(w) == 0) ||
      (XtHeight(w) == 0) ||
      (CB_HighlightThickness(cb) == 0))
    return;

  XmeDrawHighlight(XtDisplay(cb), XtWindow(cb), cb->manager.background_GC,
		   0, 0, XtWidth(w), XtHeight(w), CB_HighlightThickness(cb));
}

/*
 * DoLayout()
 *	Computes layout and sizes of children given a size for their
 * parent.  Called from SetValues, Resize and ChangeManaged.
 */

static void
DoLayout(Widget widg)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widg;
  Dimension	   ebW = 0, ebH = 0, thickW = 0, thickH = 0;
  Dimension	   listW = 0, listH = 0 ;
  Dimension	   diff, htLeft = 0;
  XtWidgetGeometry geom, replygeom;
  XtGeometryResult result;

  /* Make sure all our children are around */
  if (!CB_EditBox(cb))
    XmeWarning(widg, MISSINGKID);
  else if (!XtIsManaged(CB_EditBox(cb)))
    {
      XmeWarning(widg, UNMANAGEDKID);
      return;
    }

  if (!CB_List(cb))
    XmeWarning(widg, MISSINGKID);
  else if (!XtIsManaged(CB_List(cb)))
    {
      XmeWarning(widg, UNMANAGEDKID);
      return;
    }

  GetThickness(widg, &thickW, &thickH);
  ebW = XtWidth(cb) - 2 * (thickW + XtBorderWidth(CB_EditBox(cb)));
  if (CB_Type(cb) != XmCOMBO_BOX)
    {
      /* These are the drop down types */
      Arg args[1];
      Cardinal nargs;

      ebH = XtHeight(cb) - 2 * (thickH + XtBorderWidth(CB_EditBox(cb)));

      if (CB_ArrowSize(cb) == XmINVALID_DIMENSION)
	CB_ArrowSize(cb) = GetDefaultArrowSize((Widget)cb);
      SetHitArea((Widget)cb);
      ebW -= (CB_ArrowSpacing(cb) + CB_ArrowSize(cb));

      /* Realize the shell so these values will be maintained. */
      if (!XtIsRealized(CB_ListShell(cb)))
	XtRealizeWidget(CB_ListShell(cb));

      /* CR 7676: These numbers don't make sense, but they work? */
      listW = XtWidth(cb) - 2 * CB_HighlightThickness(cb);
      if(XtWidth(cb) <= 2 * CB_HighlightThickness(cb)) listW=1;
      nargs = 0;
      XtSetArg(args[nargs], XmNwidth, listW), nargs++;
      assert(nargs <= XtNumber(args));
      XtSetValues(CB_ListShell(cb), args, nargs);
    }
  else
    {
      Dimension new_width = XtWidth(CB_ScrolledW(cb));
      Dimension new_height = XtHeight(CB_ScrolledW(cb));

      htLeft = XtHeight(cb) -
	2 * (XtBorderWidth(CB_EditBox(cb)) + XtBorderWidth(CB_ScrolledW(cb)) +
	     CB_HighlightThickness(cb) + MGR_ShadowThickness(cb) +
	     CB_MarginHeight(cb));
      if (!cb->combo_box.ideal_ebheight)
	GetIdealTextSize((Widget)cb, NULL, &cb->combo_box.ideal_ebheight);

      /* If ideal edit box height fits, use it. */
      ebH = MIN(htLeft - MINLIST, cb->combo_box.ideal_ebheight);
      listW = XtWidth(cb) - 2 * (thickW + XtBorderWidth(CB_ScrolledW(cb)));
      listH = htLeft - ebH;

      /* Ask ScrolledW if this size is okay */
      geom.request_mode = 0;
      geom.request_mode |= CWWidth;
      geom.request_mode |= CWHeight;
      geom.width = listW;
      geom.height = listH;
      switch (XtQueryGeometry(CB_ScrolledW(cb), &geom, &replygeom))
	{
	case XtGeometryAlmost:
	  if (replygeom.request_mode & CWHeight)
	    {
	      /* see if EditBox will shrink. */
	      listH = replygeom.height;
	      diff = abs(geom.height - replygeom.height);
	      geom.request_mode = 0;
	      geom.request_mode |= CWHeight;
	      geom.height = ebH - diff;
	      result = XtQueryGeometry(CB_EditBox(cb), &geom, NULL);
	      if (result == XtGeometryYes)
		{
		  ebH = geom.height; /* will be resized below */
		  new_width = listW;
		  new_height = listH;
		}
	    }
	  break;

	case XtGeometryYes:
	case XtGeometryNo:
	default:
	  /* No compromises.  These are the new dimensions. */
	  new_width = listW;
	  new_height = listH;
	}

      XmeConfigureObject(CB_ScrolledW(cb),
			 thickW,
			 (thickH + ebH +
			  2 * XtBorderWidth(CB_EditBox(cb)) +
			  XtBorderWidth(CB_ScrolledW(cb))),
			 new_width, new_height,
			 XtBorderWidth(CB_ScrolledW(cb)));
    }

  {
    Position new_x, new_y;

    if ((CB_Type(cb) != XmCOMBO_BOX) && LayoutIsRtoLM(cb))
      {
	new_x = (thickW + CB_ArrowSize(cb) + CB_ArrowSpacing(cb));
	new_y = thickH ;
      }
    else
      {
	new_x = thickW ;
	new_y = thickH ;
      }

    XmeConfigureObject(CB_EditBox(cb), new_x, new_y,
		       ebW, ebH, XtBorderWidth(CB_EditBox(cb)));
  }
}

/*
 * ComputeSize()
 *	Determines the width and height of a ComboBox.
 */

static void
ComputeSize(Widget w,
	    Dimension editW,
	    Dimension editH,
	    Dimension *width,
	    Dimension *height)
{
  Dimension cbWidth, cbHeight;
  Dimension thickW, thickH;
  XmComboBoxWidget cb = (XmComboBoxWidget)w;
  int textWidth, textHeight;

  /* Size of ComboBox
   *	width  = (text width) + [arrow space & width] + (surrounding space)
   *	height = (text height) + (surrounding space) + [list height]
   */

  GetThickness(w, &thickW, &thickH);

  /* Compute size of textfield */
  if (!editW || !editH)
    {
      /* Ask for text child's preferred size */
      GetIdealTextSize((Widget)cb, &textWidth, &textHeight);
    }
  else
    {
      textWidth = editW;
      textHeight = editH;
    }
  cb->combo_box.ideal_ebwidth = textWidth;
  cb->combo_box.ideal_ebheight = textHeight;
  cbWidth = textWidth + 2 * (thickW + XtBorderWidth(CB_EditBox(cb)));
  cbHeight = textHeight + 2 * (thickH + XtBorderWidth(CB_EditBox(cb)));

  /* Adjust dimensions taking optional children into account. */
  if (CB_Type(cb) == XmCOMBO_BOX)
    {
      /* For a COMBO_BOX add in the height of the list. */
      XtWidgetGeometry pref;
      (void) XtQueryGeometry(CB_ScrolledW(cb), NULL, &pref);
      cbHeight += (pref.height + 2 * XtBorderWidth(CB_ScrolledW(cb)));
    }
  else
    {
      /* For other types consider the arrow dimensions. */
      if (CB_ArrowSize(cb) == XmINVALID_DIMENSION)
	CB_ArrowSize(cb) = (int) ((float) textHeight * DEFAULT_ARROW_SCALING);

      cbWidth += CB_ArrowSize(cb) + CB_ArrowSpacing(cb);
      if (CB_ArrowSize(cb) > textHeight)
	cbHeight += (CB_ArrowSize(cb) - textHeight);
    }

  /* Preserve existing sizes. */
  if (! *width)
    *width = MAX(cbWidth, 1);
  if (! *height)
    *height = MAX(cbHeight, 1);
}

/*
 * GetIdealTextSize()
 */

static void
GetIdealTextSize(Widget w,
		 int *width,
		 int *height)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;
  XtWidgetGeometry text_pref, list_pref;

  /* Ask for text child's preferred size */
  (void) XtQueryGeometry(CB_EditBox(cb), NULL, &text_pref);

  /* Ask for list child's preferred size */
  (void) XtQueryGeometry(CB_ScrolledW(cb), NULL, &list_pref);

  if (width)
    *width = MAX(text_pref.width, list_pref.width);
  if (height)
    *height = text_pref.height;
}

/*
 * GetDefaultArrowSize()
 */

static Dimension
GetDefaultArrowSize(Widget w)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;

  if (!cb->combo_box.ideal_ebheight)
    GetIdealTextSize((Widget)cb, NULL, &cb->combo_box.ideal_ebheight);

  return((int) ((float) cb->combo_box.ideal_ebheight * DEFAULT_ARROW_SCALING));
}

/*
 * SetHitArea()
 *	Sets hit area for arrow.
 */

static void
SetHitArea(Widget w)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;
  Dimension	   thickW = 0, thickH = 0, ebH;

  assert(CB_Type(cb) != XmCOMBO_BOX);

  GetThickness(w, &thickW, &thickH);
  ebH = XtHeight(cb) - 2 * (thickH + XtBorderWidth(CB_EditBox(cb)));

  if (CB_ArrowSize(cb) == XmINVALID_DIMENSION)
    CB_ArrowSize(cb) = GetDefaultArrowSize((Widget)cb);

  CB_HitRect(cb).width = CB_ArrowSize(cb);
  CB_HitRect(cb).height = ebH;
  if (LayoutIsRtoLM(cb))
    {
      CB_HitRect(cb).x = thickW;
      CB_HitRect(cb).y = thickH;
    }
  else
    {
      CB_HitRect(cb).x = XtWidth(cb) - thickW - CB_ArrowSize(cb);
      CB_HitRect(cb).y = thickH;
    }
}

/*
 * GetThickness()
 *	Gets "thickness" resources, i.e. shadow thickness, marginW, etc.
 */

static void
GetThickness(Widget widg,
	     Dimension *width,
	     Dimension *height)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widg;

  if (width)
    *width = cb->combo_box.margin_width + cb->combo_box.highlight_thickness +
	cb->manager.shadow_thickness;
  if (height)
    *height = cb->combo_box.margin_height +
	cb->combo_box.highlight_thickness + cb->manager.shadow_thickness;
}

/*
 * GetArrowGC()
 *	Create the GC for drawing sensitive arrows.
 */

static void
GetArrowGC(Widget widget)
{
  XmComboBoxWidget cb = (XmComboBoxWidget) widget;
  XGCValues	   values;
  XtGCMask	   mask;

  /* Only plain ComboBoxes have arrows. */
  if (CB_Type(cb) != XmCOMBO_BOX)
    {
      mask = 0;
      values.foreground = XtBackground(widget), mask |= GCForeground;
      values.graphics_exposures = False, 	mask |= GCGraphicsExposures;
      cb->combo_box.arrow_GC = XtGetGC(widget, mask, &values);
    }
  else
    {
      cb->combo_box.arrow_GC = NULL;
    }
}

/*
 * DrawArrow()
 */

static void
DrawArrow(Widget widget,
	  Boolean pressed)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int newbox, size, max_height, excess;
  int x, y, w, h;

  assert(CB_Type(cb) != XmCOMBO_BOX);

  if (CB_ArrowSize(cb) == XmINVALID_DIMENSION)
    CB_ArrowSize(cb) = GetDefaultArrowSize((Widget) cb);

  /* CR 6912: Reduce size to fit the actual space available. */
  max_height = XtHeight(cb) -
    2 * (CB_MarginHeight(cb) + CB_HighlightThickness(cb) +
	 cb->manager.shadow_thickness  + XtBorderWidth(CB_EditBox(cb)));
  if (CB_ArrowSize(cb) > max_height)
    {
      excess = CB_ArrowSize(cb) - max_height;
      size = max_height;
    }
  else
    {
      excess = 0;
      size = CB_ArrowSize(cb);
    }
  newbox = (int) (size * SQRT3_OVER_2);

  /* CR 6890: Center the arrow within the box. */
  x = CB_HitRect(cb).x + (size - newbox + excess) / 2;
  y = CB_HitRect(cb).y + (CB_HitRect(cb).height - size - 1) / 2;
  w = h = newbox;

  XmeDrawArrow(XtDisplay(widget), XtWindow(widget),
	       (pressed ?
		cb->manager.bottom_shadow_GC : cb->manager.top_shadow_GC),
	       (pressed ?
		cb->manager.top_shadow_GC : cb->manager.bottom_shadow_GC),
	       (XtIsSensitive(widget) ?
		cb->combo_box.arrow_GC : cb->manager.background_GC),
	       x, y, w, h, cb->combo_box.arrow_shadow_width, XmARROW_DOWN);

  y += newbox;
  h = size - newbox;

  XmeDrawShadows(XtDisplay(widget), XtWindow(widget),
		 cb->manager.top_shadow_GC, cb->manager.bottom_shadow_GC,
		 x, y, w, h, cb->combo_box.arrow_shadow_width, XmSHADOW_OUT);
}

/*
 * DrawShadows()
 */

static void
DrawShadows(Widget widget)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int offset = CB_HighlightThickness(cb);

  XmeDrawShadows(XtDisplay(widget), XtWindow(widget),
		 cb->manager.top_shadow_GC,
		 cb->manager.bottom_shadow_GC,
		 offset, offset,
		 XtWidth(widget) - 2 * offset,
		 XtHeight(widget) - 2 * offset,
		 MGR_ShadowThickness(cb),
		 XmSHADOW_OUT);
}

/*
 * CreateChildren()
 *	Called by the ComboBox Widget's Initialize proc. to create
 * the children that make up a ComboBox.
 */

static void
CreateChildren(Widget    widget,
	       ArgList   arglist,
	       Cardinal *num_args)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg		   loc_args[10];
  Cardinal	   n_args;

  /*
   * N.B.:  Do not change order of creation!  The CB_EditBox() macro
   *	and code in CreateScrolledList() both rely on this order.
   */

  /* Create the editbox. */
  CreateEditBox(widget, TEXT_CHILD_NAME, widget, arglist, num_args);
  XtAddCallback(CB_EditBox(cb), XmNvalueChangedCallback,
		TextChangedCB, (XtPointer) cb);

  /* Create the popup shell if appropriate. */
  if (CB_Type(cb) != XmCOMBO_BOX)
    {
      CB_ListShell(cb) =
	CreatePulldown(widget, SHELL_CHILD_NAME, widget, arglist, num_args);

      XtAddCallback(CB_ListShell(cb), XmNpopupCallback, ShellPopupCB,
		    (XtPointer) cb);
      XtAddCallback(CB_ListShell(cb), XmNpopdownCallback, ShellPopdownCB,
		    (XtPointer) cb);

      /* Add Event Handler for pointer events in the popup */
      XtAddEventHandler(CB_ListShell(cb), POPUP_EVENT_MASK, FALSE,
			PopupEH, (XtPointer)cb);
    }

  /* Create the list. */
  CB_List(cb) =
    CreateScrolledList(CB_ListShell(cb) ? CB_ListShell(cb) : widget,
		       LIST_CHILD_NAME, widget, arglist, num_args);
  XtAddCallback(CB_List(cb), XmNbrowseSelectionCallback,
		(XtCallbackProc)ListSelectionCB, (XtPointer)cb);

  /* CR 9868: Direct events properly in pointer focus mode. */
  if (_XmGetFocusPolicy((Widget) cb) == XmPOINTER)
    {
      XtSetKeyboardFocus((Widget) cb, CB_EditBox(cb));
      if (CB_ListShell(cb))
	XtSetKeyboardFocus(CB_ListShell(cb), CB_List(cb));
    }

  /* Setup all the accelerators and translations. */
  switch (CB_Type(cb))
    {
    case XmCOMBO_BOX:
      /* The standard ComboBox accelerators will handle this case. */
      break;

    case XmDROP_DOWN_LIST:
      XtOverrideTranslations(CB_List(cb), parsed_list_translations);
      break;

    case XmDROP_DOWN_COMBO_BOX:
      n_args = 0;
      XtSetArg(loc_args[n_args], XmNaccelerators,
	       parsed_list_accelerators), n_args++;
      assert(n_args <= XtNumber(loc_args));
      XtSetValues(CB_EditBox(cb), loc_args, n_args);
      XtInstallAccelerators(CB_List(cb), CB_EditBox(cb));
      XtOverrideTranslations(CB_EditBox(cb), parsed_text_focus_translations);
      break;

    default:
      assert(False);
    }

  /* Install the standard ComboBox accelerators everywhere. */
  XtInstallAccelerators(CB_List(cb), (Widget) cb);
  XtInstallAccelerators(CB_EditBox(cb), (Widget) cb);

  /* Add event handlers to the scrollbar in the scrolled list widget */
  if (CB_Type(cb) != XmCOMBO_BOX)
    {
      Widget sb;

      sb = cb->combo_box.vsb;
      if (sb != (Widget) NULL)
	{
	  XtInsertEventHandler(sb, ButtonPressMask, False, SBBtnDownEH,
			       (XtPointer) CB_ListShell(cb), XtListHead);
	  XtInsertEventHandler(sb, ButtonReleaseMask, False, SBBtnUpEH,
			       (XtPointer) CB_ListShell(cb), XtListHead);
	}

      sb = cb->combo_box.hsb;
      if (sb != (Widget) NULL)
	{
	  XtInsertEventHandler(sb, ButtonPressMask, False, SBBtnDownEH,
			       (XtPointer) CB_ListShell(cb), XtListHead);
	  XtInsertEventHandler(sb, ButtonReleaseMask, False, SBBtnUpEH,
			       (XtPointer) CB_ListShell(cb), XtListHead);
	}
    }
}

/*
 * CreateEditBox()
 *	Creates the EditBox child of the ComboBox.
 */

static Widget
CreateEditBox(Widget    parent,
	      String    name,
	      Widget    w,
	      ArgList   arglist,
	      Cardinal *num_args)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)w;
  Widget   	   text_widget;
  Arg      	   loc_args[15];
  Cardinal 	   nloc;
  ArgList  	   merged_args;
  char		   *item = NULL;

  /*
   * The combo-box itself was already created with this argument
   * list, so there is no need to separate "optional" and "required"
   * resources.  Our instance field values will be identical if we
   * override any duplicates.
   */
  nloc = 0;

  if (CBS_Columns(cb) != XmUNSPECIFIED_COLUMNS)
    {
      XtSetArg(loc_args[nloc], XmNcolumns, CBS_Columns(cb)), nloc++;
    }

  if (CBS_SelectedItem(cb))
    {
      item = _XmStringGetTextConcat(CBS_SelectedItem(cb));
      if (item)
	XtSetArg(loc_args[nloc], XmNvalue, item), nloc++;
    }

  XtSetArg(loc_args[nloc], XmNrenderTable, CB_RenderTable(cb)), nloc++;
  XtSetArg(loc_args[nloc], XmNnavigationType, XmNONE), nloc++;
  XtSetArg(loc_args[nloc], XmNhighlightThickness, 0), nloc++;
  XtSetArg(loc_args[nloc], XmNborderWidth, 0), nloc++;
  if (CB_Type(cb) == XmDROP_DOWN_LIST)
    {
      XtSetArg(loc_args[nloc], XmNeditable, FALSE), nloc++;
      XtSetArg(loc_args[nloc], XmNcursorPositionVisible, FALSE), nloc++;
      XtSetArg(loc_args[nloc], XmNshadowThickness, 0), nloc++;
    }
  else
    {
      XtSetArg(loc_args[nloc], XmNeditable, TRUE), nloc++;
      XtSetArg(loc_args[nloc], XmNeditMode, XmSINGLE_LINE_EDIT), nloc++;
      XtSetArg(loc_args[nloc], XmNcursorPositionVisible, TRUE), nloc++;
    }
  assert(nloc <= XtNumber(loc_args));

  /* Create the edit box. */
  merged_args = XtMergeArgLists(arglist, *num_args, loc_args, nloc);
  text_widget = XmCreateTextField(parent, name, merged_args, *num_args + nloc);
  XtFree((char *)merged_args);
  if (item)
    XtFree(item);

  XtManageChild(text_widget);
  assert(cb->composite.children[0] == text_widget);
  CB_EditBox(cb) = text_widget;

  return text_widget;
}

/*
 * CreatePulldown()
 *	Creates the pulldown shell for the list if the ComboBox is
 * a XmDROP_DOWN_COMBO_BOX or a XmDROP_DOWN_LIST.
 */
static Widget
CreatePulldown(Widget    parent,
	       String    name,
	       Widget    w,	/* unused */
	       ArgList   arglist,
	       Cardinal *num_args)
{
  Widget   shell;
  Arg      args[4];
  ArgList  merged_args;
  Cardinal n;

  n = 0;
  XtSetArg(args[n], XmNlayoutDirection, LayoutM(parent)), n++;
  XtSetArg(args[n], XmNownerEvents, True), n++;
  XtSetArg(args[n], XmNgrabStyle, GrabModeSync), n++;
  assert(n <= XtNumber(args));
  merged_args = XtMergeArgLists(arglist, *num_args, args, n);
  shell = XtCreatePopupShell(name, xmGrabShellWidgetClass, parent,
			     merged_args, n + *num_args);
  XtFree((char*)merged_args);

  return shell;
}

/*
 * CreateScrolledList()
 *	Creates the Scrolled List child of the ComboBox.  If the
 * ComboBox is a XmDROP_DOWN_COMBO_BOX or a XmDROP_DOWN_LIST it
 * creates the list in a popup shell.
 */

static Widget
CreateScrolledList(Widget    parent,
		   String    name,
		   Widget    w,
		   ArgList   arglist,
		   Cardinal *num_args)
{
  Cardinal n;
  Arg loc_args[16];
  Widget list;
  XmComboBoxWidget cb = (XmComboBoxWidget)w;
  ArgList merged_args;
  int pos, nitems = 0;
  Boolean setpos = FALSE;
  XmString *items;

  n = 0;

  if (CBS_Items(cb) != XmUNSPECIFIED_ITEMS)
    {
      XtSetArg(loc_args[n], XmNitems, CBS_Items(cb)), n++;
    }

  if (CBS_ItemCount(cb) != XmUNSPECIFIED_COUNT)
    {
      XtSetArg(loc_args[n], XmNitemCount, CBS_ItemCount(cb)), n++;
    }

  if (CBS_VisibleItemCount(cb) != XmUNSPECIFIED_COUNT)
    {
      XtSetArg(loc_args[n], XmNvisibleItemCount, CBS_VisibleItemCount(cb)), n++;
    }

  if (CBS_SelectedItem(cb))
    {
      XtSetArg(loc_args[n], XmNselectedItems, &CBS_SelectedItem(cb)), n++;
      XtSetArg(loc_args[n], XmNselectedItemCount, 1), n++;
    }
  else
    {
      if (CBS_SelectedPosition(cb) == XmINVALID_POSITION)
	pos = 1;  /* Issue 138 */
      else if (CB_PositionMode(cb) == XmZERO_BASED)
	pos = CBS_SelectedPosition(cb) + 1;
      else
	pos = CBS_SelectedPosition(cb);
      XtSetArg(loc_args[n], XmNselectedPositions, &pos), n++;
      XtSetArg(loc_args[n], XmNselectedPositionCount, 1), n++;
      setpos = TRUE;
    }

  XtSetArg(loc_args[n], XmNrenderTable, CB_RenderTable(cb)), n++;
  if (CB_Type(cb) == XmCOMBO_BOX)
    XtSetArg(loc_args[n], XmNtraversalOn, FALSE), n++;
  XtSetArg(loc_args[n], XmNhighlightThickness,
	   ((CB_Type(cb) == XmDROP_DOWN_LIST) ? 2 : 0)), n++;
  XtSetArg(loc_args[n], XmNborderWidth, 0), n++;
  XtSetArg(loc_args[n], XmNnavigationType, XmNONE), n++;
  XtSetArg(loc_args[n], XmNselectionPolicy, XmBROWSE_SELECT), n++;
  XtSetArg(loc_args[n], XmNlistSizePolicy, XmVARIABLE), n++;
  XtSetArg(loc_args[n], XmNspacing, 0), n++;
  XtSetArg(loc_args[n], XmNvisualPolicy, XmVARIABLE), n++;
  assert(n <= XtNumber(loc_args));
  merged_args = XtMergeArgLists(arglist, *num_args, loc_args, n);

  list = XmCreateScrolledList(parent, name, merged_args, n + *num_args);
  XtManageChild(list);
  XtFree((char *)merged_args);
  CB_ScrolledW(cb) = XtParent(list);

  n = 0;
  XtSetArg(loc_args[n], XmNhorizontalScrollBar, &cb->combo_box.hsb), n++;
  XtSetArg(loc_args[n], XmNverticalScrollBar, &cb->combo_box.vsb), n++;
  assert(n <= XtNumber(loc_args));
  XtGetValues(CB_ScrolledW(cb), loc_args, n);

  n = 0;
  XtSetArg(loc_args[n], XmNshadowThickness, 0), n++;
  assert(n <= XtNumber(loc_args));
  XtSetValues(list, loc_args, n);

  if (setpos)
    {
      n = 0;
      XtSetArg(loc_args[n], XmNitems, &items), n++;
      XtSetArg(loc_args[n], XmNitemCount, &nitems), n++;
      assert(n <= XtNumber(loc_args));
      XtGetValues(list, loc_args, n);

      /* CR 7064: Don't try to set the edit box value if no items exist. */
      if (nitems && (nitems >= pos))
	{
	  if (pos > 0)
	    pos--;

	  SetEditBoxValue((Widget) cb, items[pos]);
	}
    }

  return list;
}

/*
 * Hit()
 *	Decide whether a button event happened within a particular XRectangle.
 */

static Boolean
Hit(XButtonEvent* event,
    XRectangle r)
{
  if (event == NULL)
    return False;
  else
    return ((r.x <= event->x) && (event->x <= (r.x + r.width)) &&
	    (r.y <= event->y) && (event->y <= (r.y + r.height)));
}

/*
 * GetEditBoxValue()
 *	Retrieve the XmString value of the edit box.
 */

static XmString
GetEditBoxValue(Widget cb)
{
  Widget	       edit_box = CB_EditBox(cb);
  XmAccessTextualTrait textTrait;

  textTrait = (XmAccessTextualTrait)
    XmeTraitGet((XtPointer) XtClass(edit_box), XmQTaccessTextual);

  if (textTrait)
    return (XmString) textTrait->getValue(edit_box, XmFORMAT_XmSTRING);
  else
    return NULL;
}

/*
 * SetEditBoxValue()
 *	Set the XmString value of the edit box.
 */

static void
SetEditBoxValue(Widget   cb,
		XmString value)
{
  Widget 	       edit_box = CB_EditBox(cb);
  XmAccessTextualTrait textTrait;

  textTrait = (XmAccessTextualTrait)
    XmeTraitGet((XtPointer) XtClass(edit_box), XmQTaccessTextual);

  textTrait->setValue(edit_box, value, XmFORMAT_XmSTRING);
}

/* ------------- CONVENIENCE FUNCTIONS ---------- */


Widget
XmCreateComboBox(Widget   parent,
		 char    *name,
		 ArgList  args,
		 Cardinal num_args)
{
  return XtCreateWidget(name, xmComboBoxWidgetClass, parent, args, num_args);
}

Widget
XmCreateDropDownComboBox(Widget   parent,
			 char    *name,
			 ArgList  args,
			 Cardinal num_args)
{
  Arg loc_args[5];
  Cardinal n;
  ArgList merged_args;
  Widget result;

  n = 0;
  XtSetArg(loc_args[n], XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX), n++;
  assert(n <= XtNumber(loc_args));

  merged_args = XtMergeArgLists(args, num_args, loc_args, n);
  result = XtCreateWidget(name, xmComboBoxWidgetClass, parent,
			  merged_args, n + num_args);
  XtFree((char *)merged_args);

  return result;
}

Widget
XmCreateDropDownList(Widget   parent,
		     char    *name,
		     ArgList  args,
		     Cardinal num_args)
{
  Arg loc_args[5];
  Cardinal n;
  ArgList merged_args;
  Widget result;

  n = 0;
  XtSetArg(loc_args[n], XmNcomboBoxType, XmDROP_DOWN_LIST), n++;
  assert(n <= XtNumber(loc_args));

  merged_args = XtMergeArgLists(args, num_args, loc_args, n);
  result = XtCreateWidget(name, xmComboBoxWidgetClass, parent,
			  merged_args, n + num_args);
  XtFree((char *)merged_args);

  return result;
}

Widget
XmVaCreateComboBox(
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
                         xmComboBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedComboBox(
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
                         xmComboBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/*
 * XmComboBoxAddItem
 *	Convenience function added for CDE compatibility.  Add an item
 * to the ComboBox list.
 */

void
XmComboBoxAddItem(Widget   widget,
		  XmString item,
		  int      pos,
		  Boolean  unique)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmIsComboBox(widget))
    {
      XmeWarning(widget, NOTACOMBOBOX);
      _XmAppUnlock(app);
      return;
    }
  else if (!CB_List(cb)) {
    _XmAppUnlock(app);
    return;
  }

  /* If requested, scan for duplicate items. */
  if (unique && item && XmListItemExists(CB_List(cb), item)) {
    _XmAppUnlock(app);
    return;
  }

  /* Add the item to the list and update our selected_position. */
  XmListAddItemUnselected(CB_List(cb), item, pos);
  XmComboBoxUpdate(widget);
  _XmAppUnlock(app);
}

/*
 * XmComboBoxDeletePos
 *	Convenience function added for CDE compatibility.  Delete the
 * list item at the indicated position.
 */

void
XmComboBoxDeletePos(Widget widget,
		    int    pos)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int selpos;
  int nitems;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmIsComboBox(widget))
    {
      XmeWarning(widget, NOTACOMBOBOX);
      _XmAppUnlock(app);
      return;
    }
  else if (!CB_List(cb)) {
    _XmAppUnlock(app);
    return;
  }

  /* Validate the position. */
  {
    Arg args[10];
    Cardinal nargs;
    int *positions;
    int count;

    nargs = 0;
    XtSetArg(args[nargs], XmNitemCount, &nitems),		nargs++;
    XtSetArg(args[nargs], XmNselectedPositions, &positions),	nargs++;
    XtSetArg(args[nargs], XmNselectedPositionCount, &count),	nargs++;
    assert(nargs <= XtNumber(args));
    XtGetValues(CB_List(widget), args, nargs);

    /* Ensure that we're using a one-based position. */
    selpos = (count > 0) ? *positions : 0;
  }

  /* DtComboBoxDeletePos rejects pos == 0, but we allow it. */
  if ((pos < 0) || (pos > nitems) || (nitems <= 0))
    {
      XmeWarning(widget, DELETEBADPOS);
      _XmAppUnlock(app);
      return;
    }

  /* Delete this item. */
  XmListDeletePos(CB_List(cb), pos);

  /* If our selected item just got deleted select seomthing else. */
  if ((pos ? pos : nitems) == selpos)
    {
      if (nitems > 1)
	{
	  /* Invoke the callbacks now. */
	  XmListSelectPos(CB_List(cb), selpos, True);
	}
      else
	{
	  /* The list is now empty. */
	  CB_TextChanged(cb) = FALSE;
	  XmComboBoxUpdate(widget);
	  CB_TextChanged(cb) = FALSE;
	}
    }

  _XmAppUnlock(app);
}

/*
 * XmComboBoxSelectItem
 *	Convenience function added for CDE compatibility.  Select the
 * indicated item.  Do not invoke the callbacks or scroll the list.
 */

void
XmComboBoxSelectItem(Widget   widget,
		     XmString item)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int pos;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmIsComboBox(widget))
    {
      XmeWarning(widget, NOTACOMBOBOX);
      _XmAppUnlock(app);
      return;
    }
  else if (!CB_List(cb)) {
    _XmAppUnlock(app);
    return;
  }


  /* Calculate the position of this item. */
  pos = XmListItemPos(CB_List(cb), item);

  if (pos > 0)
    {
      /* Select the indicated item. */
      XmListDeselectAllItems(CB_List(cb));
      XmListSelectPos(CB_List(cb), pos, FALSE);

      /* Discard user changes to the edit box and suppress all callbacks. */
      CB_TextChanged(cb) = FALSE;
      XmComboBoxUpdate(widget);
      CB_TextChanged(cb) = FALSE;
    }
  else
    XmeWarning(widget, SELECTBADITEM);

  _XmAppUnlock(app);
}

/*
 * XmComboBoxSetItem
 *	Convenience function added for CDE compatibility.  Select the
 * indicated item and force it to be visible.  Do not invoke the callbacks.
 */

void
XmComboBoxSetItem(Widget   widget,
		  XmString item)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  int pos;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmIsComboBox(widget))
    {
      XmeWarning(widget, NOTACOMBOBOX);
      _XmAppUnlock(app);
      return;
    }
  else if (!CB_List(cb)) {
    _XmAppUnlock(app);
    return;
  }

  /* Calculate the position of this item. */
  pos = XmListItemPos(CB_List(cb), item);

  if (pos > 0)
    {
      /* Scroll the item to the top and select it. */
      XmListSetPos(CB_List(cb), pos);
      XmListSelectPos(CB_List(cb), pos, FALSE);

      /* Discard user changes to the edit box and suppress all callbacks. */
      CB_TextChanged(cb) = FALSE;
      XmComboBoxUpdate(widget);
      CB_TextChanged(cb) = FALSE;
    }
  else
    XmeWarning(widget, SETBADITEM);

  _XmAppUnlock(app);

}
 /* Jim Added this function to appease the Combo Box Symbol God */
XmString
XmCombinationBoxGetValue(Widget widget)
{
    return (GetEditBoxValue(widget));
}

/*
 * XmComboBoxUpdate()
 *	Resynchronize internal data structures after an application
 * updates our children directly.
 */

void
XmComboBoxUpdate(Widget widget)
{
  XmComboBoxWidget cb = (XmComboBoxWidget)widget;
  Arg args[10];
  Cardinal nargs;
  XmString *items;
  int icount;
  int *pos;
  int pcount;

  _XmWidgetToAppContext(widget);
  _XmAppLock(app);

  if (!XmIsComboBox(widget))
    {
      XmeWarning(widget, NOTACOMBOBOX);
      _XmAppUnlock(app);
      return;
    }
  else if (!CB_List(cb)) {
    _XmAppUnlock(app);
    return;
  }

  /* CR 8445: If no text entry is in progress echo the new list selection. */
  if (!CB_TextChanged(cb))
    {
      /* The selected item and position of the list may have changed. */
      nargs = 0;
      XtSetArg(args[nargs], XmNselectedPositions, &pos), 	nargs++;
      XtSetArg(args[nargs], XmNselectedPositionCount, &pcount), nargs++;
      XtSetArg(args[nargs], XmNitems, &items),			nargs++;
      XtSetArg(args[nargs], XmNitemCount, &icount),		nargs++;
      assert(nargs <= XtNumber(args));
      XtGetValues(CB_List(cb), args, nargs);

      if ((pcount > 0) && (icount > 0))
	SetEditBoxValue((Widget) cb, items[pos[0] - 1]);
    }
    _XmAppUnlock(app);
}
