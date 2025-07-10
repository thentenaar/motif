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
static char rcsid[] = "$TOG: Label.c /main/26 1997/06/18 17:40:00 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/keysymdef.h>
#include <Xm/AccTextT.h>
#include <Xm/AtomMgr.h>
#include <Xm/BaseClassP.h>
#include <Xm/CascadeBP.h>
#include <Xm/DisplayP.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/DragIconP.h>
#include <Xm/DrawP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "GadgetUtiI.h"
#include "GMUtilsI.h"
#include "LabelGI.h"
#include "LabelI.h"
#include "MessagesI.h"
#include "MenuProcI.h"
#include "PrimitiveI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "TransferI.h"
#include "TravActI.h"
#include "XmI.h"
#include "XmosI.h"
#include "XmStringI.h"
#include <Xm/XpmP.h>
#include <string.h>
#include <Xm/XmP.h>
#ifdef FIX_1381
#include <Xm/ColorI.h>
#endif
#ifdef FIX_1521
#if USE_XFT
#include "XmRenderTI.h"
#include <X11/Xft/Xft.h>
#endif
#endif

#define FIX_1442
#define FIX_1484
#define FIX_1504

#ifdef FIX_1504
#define FIX_1639
#endif

#define Pix(w)			((w)->label.pixmap)
#define Pix_insen(w)		((w)->label.pixmap_insen)

/* Warning Messages */

#define CS_STRING_MESSAGE	_XmMMsgLabel_0003
#define ACC_MESSAGE		_XmMMsgLabel_0004

/********    Static Function Declarations    ********/

static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass c);
static void InitializePrehook(Widget req, Widget new_w,
			      ArgList args, Cardinal *num_args);
static void InitializePosthook(Widget req, Widget new_w,
			       ArgList args, Cardinal *num_args);
static void SetNormalGC(XmLabelWidget lw);
static void Resize(Widget wid);
static void Initialize(Widget req, Widget new_w,
		       ArgList args, Cardinal *num_args);
static XtGeometryResult QueryGeometry(Widget wid,
				      XtWidgetGeometry *intended,
				      XtWidgetGeometry *reply);
static void Destroy(Widget w);
static void Redisplay(Widget wid, XEvent *event, Region region);
static void Enter(Widget wid, XEvent *event,
		  String *params, Cardinal *num_params);
static void Leave(Widget wid, XEvent *event,
		  String *params, Cardinal *num_params);
static Boolean SetValues(Widget cw, Widget rw, Widget nw,
			 ArgList args, Cardinal *num_args);
static void SetOverrideCallback(Widget wid);
static void Help(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void GetLabelString(Widget wid, int resource, XtArgVal *value);
static void GetAccelerator(Widget wid, int resource, XtArgVal *value);
static void GetAcceleratorText(Widget wid, int resource, XtArgVal *value);
static XmStringCharSet _XmStringCharSetCreate(XmStringCharSet stringcharset);
static void GetMnemonicCharSet(Widget wid, int resource, XtArgVal *value);
static void SetValuesAlmost(Widget cw, Widget nw,
			    XtWidgetGeometry *request, XtWidgetGeometry *reply);
static Boolean XmLabelGetDisplayRect(Widget w, XRectangle *displayrect);
static Boolean XmLabelGetBaselines(Widget wid,
				   Dimension **baselines,
				   int *line_count);
static void XmLabelMarginsProc(Widget w,
			       XmBaselineMargins *margins_rec);
static Widget GetPixmapDragIcon(Widget w);
static void ProcessDrag(Widget w, XEvent *event,
                        String *params, Cardinal *num_params);
static void SetActivateCallbackState(Widget w, XmActivateState state);
static void CheckSetRenderTable(Widget wid, int offset, XrmValue *value);
static void FromPaddingPixels(Widget, int, XtArgVal *);
static XmImportOperator ToPaddingPixels(Widget, int, XtArgVal*);
static XtPointer LabelGetValue(Widget, int);
static void LabelSetValue(Widget, XtPointer, int);
static int LabelPreferredValue(Widget);
static char* GetLabelAccelerator(Widget);
static KeySym GetLabelMnemonic(Widget);
static XtPointer ConvertToEncoding(Widget, char*, Atom, unsigned long *,
				 Boolean *);
/********    End Static Function Declarations    ********/


void _XmLabelConvert(Widget w, XtPointer ignore, XmConvertCallbackStruct*);

/* Transfer trait record */
static XmConst XmTransferTraitRec LabelTransfer = {
  0, 						/* version		  */
  (XmConvertCallbackProc) _XmLabelConvert,	/* convertProc		  */
  NULL,						/* destinationProc	  */
  NULL						/* destinationPreHookProc */
};

/* Menu Savvy trait record */
static XmConst XmMenuSavvyTraitRec MenuSavvyRecord = {
  0,						     /* version		  */
  (XmMenuSavvyDisableProc) SetActivateCallbackState, /* disableCallback	  */
  GetLabelAccelerator,				     /* getAccelerator	  */
  GetLabelMnemonic,				     /* getMnemonic	  */
  (XmMenuSavvyGetActivateCBNameProc) NULL,	     /* getActivateCBName */
};

/* Access Textual trait record */
XmAccessTextualTraitRec _XmLabel_AccessTextualRecord = {
  0,				/* version         */
  LabelGetValue,		/* getValueMethod  */
  LabelSetValue,		/* setValuesMethod */
  LabelPreferredValue,		/* preferredFormat */
};


/* Default translations and action recs */

static XtTranslations default_parsed;

#define defaultTranslations	_XmLabel_defaultTranslations

static XtTranslations menu_parsed;

#define menuTranslations	_XmLabel_menuTranslations


static XtActionsRec ActionsList[] = {
  { "Enter",		Enter		},
  { "Leave",		Leave		},
  { "Help",		Help		},
  { "ProcessDrag",	ProcessDrag	}
};


/* Here are the translations used by the subclasses for menu traversal */
/* The matching actions are defined in RowColumn.c                     */

#define menu_traversal_events	_XmLabel_menu_traversal_events

static XtResource resources[] =
{
  {
    XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, primitive.shadow_thickness),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNalignment, XmCAlignment, XmRAlignment,
    sizeof(unsigned char), XtOffsetOf(XmLabelRec, label.alignment),
    XmRImmediate, (XtPointer) XmALIGNMENT_CENTER
  },

  {
    XmNlabelType, XmCLabelType, XmRLabelType,
    sizeof(unsigned char), XtOffsetOf(XmLabelRec, label.label_type),
    XmRImmediate, (XtPointer) XmSTRING
  },

  {
    XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_width),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_height),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNmarginLeft, XmCMarginLeft, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_left),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNmarginRight, XmCMarginRight, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_right),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNmarginTop, XmCMarginTop, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_top),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNmarginBottom, XmCMarginBottom, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.margin_bottom),
    XmRImmediate, (XtPointer) 0
  },

  {
      "pri.vate","Pri.vate",XmRBoolean,
      sizeof(Boolean), XtOffsetOf(XmLabelRec, label.check_set_render_table),
      XmRImmediate, (XtPointer) False
  },

  {
    XmNfontList, XmCFontList, XmRFontList,
    sizeof(XmFontList), XtOffsetOf(XmLabelRec, label.font),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNrenderTable, XmCRenderTable, XmRRenderTable,
    sizeof(XmRenderTable), XtOffsetOf(XmLabelRec, label.font),
    XmRCallProc, (XtPointer)CheckSetRenderTable
  },

  {
    XmNlabelPixmap, XmCLabelPixmap, XmRDynamicPixmap,
    sizeof(Pixmap), XtOffsetOf(XmLabelRec, label.pixmap),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNlabelInsensitivePixmap, XmCLabelInsensitivePixmap, XmRDynamicPixmap,
    sizeof(Pixmap), XtOffsetOf(XmLabelRec, label.pixmap_insen),
    XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
  },

  {
    XmNlabelString, XmCXmString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmLabelRec, label._label),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNmnemonic, XmCMnemonic, XmRKeySym,
    sizeof(KeySym), XtOffsetOf(XmLabelRec, label.mnemonic),
    XmRImmediate, (XtPointer) XK_VoidSymbol
  },

   {
     XmNmnemonicCharSet, XmCMnemonicCharSet, XmRString,
     sizeof(XmStringCharSet), XtOffsetOf(XmLabelRec, label.mnemonicCharset),
     XmRImmediate, (XtPointer) XmFONTLIST_DEFAULT_TAG
   },

  {
    XmNaccelerator, XmCAccelerator, XmRString,
    sizeof(char *), XtOffsetOf(XmLabelRec, label.accelerator),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNacceleratorText, XmCAcceleratorText, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmLabelRec, label._acc_text),
    XmRImmediate, (XtPointer) NULL
  },

 {
   XmNrecomputeSize, XmCRecomputeSize, XmRBoolean,
   sizeof(Boolean), XtOffsetOf(XmLabelRec, label.recompute_size),
   XmRImmediate, (XtPointer) True
 },

 {
   XmNstringDirection, XmCStringDirection, XmRStringDirection,
   sizeof(unsigned char), XtOffsetOf(XmLabelRec, label.string_direction),
   XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
 },

 {
   XmNtraversalOn, XmCTraversalOn, XmRBoolean,
   sizeof(Boolean), XtOffsetOf(XmPrimitiveRec, primitive.traversal_on),
   XmRImmediate, (XtPointer) False
  },

  {
    XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
    sizeof(Dimension), XtOffsetOf(XmPrimitiveRec, primitive.highlight_thickness),
    XmRImmediate, (XtPointer) 0
  },
#ifndef XM_PART_BC
  {
    XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
    sizeof(XmDirection), XtOffsetOf(XmPrimitiveRec, primitive.layout_direction),
    XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
  },
#endif
  {
    XmNpixmapPlacement, XmCPixmapPlacement, XmRPixmapPlacement,
    sizeof(XmPixmapPlacement), XtOffsetOf(XmLabelRec, label.pixmap_placement),
    XmRImmediate, (XtPointer) XmPIXMAP_LEFT
  },
  {
    XmNpixmapTextPadding, XmCSpace, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmLabelRec, label.pixmap_text_padding),
    XmRImmediate, (XtPointer) 2
  }
};

/* Definition for resources that need special processing in get values. */

static XmSyntheticResource syn_resources[] =
{
  {
    XmNmarginWidth, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_width),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },

  {
    XmNmarginHeight, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_height),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },

  {
    XmNmarginLeft, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_left),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },

  {
    XmNmarginRight, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_right),
    XmeFromHorizontalPixels, XmeToHorizontalPixels
  },

  {
    XmNmarginTop, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_top),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },

  {
    XmNmarginBottom, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.margin_bottom),
    XmeFromVerticalPixels, XmeToVerticalPixels
  },

  {
    XmNlabelString, sizeof(XmString),
    XtOffsetOf(XmLabelRec, label._label),
    GetLabelString, NULL
  },

  {
    XmNmnemonicCharSet, sizeof(XmStringCharSet),
    XtOffsetOf(XmLabelRec, label.mnemonicCharset),
    GetMnemonicCharSet, NULL
  },

  {
    XmNaccelerator, sizeof(String),
    XtOffsetOf(XmLabelRec, label.accelerator),
    GetAccelerator, NULL
  },

  {
    XmNacceleratorText, sizeof(XmString),
    XtOffsetOf(XmLabelRec, label._acc_text),
    GetAcceleratorText, NULL
  },

  {
    XmNpixmapTextPadding, sizeof(Dimension),
    XtOffsetOf(XmLabelRec, label.pixmap_text_padding),
    FromPaddingPixels, (XmImportProc) ToPaddingPixels
  }
};

static XmBaseClassExtRec labelBaseClassExtRec = {
  NULL,				/* Next extension         */
  NULLQUARK,			/* record type XmQmotif   */
  XmBaseClassExtVersion,	/* version                */
  sizeof(XmBaseClassExtRec),	/* size                   */
  InitializePrehook,		/* initialize prehook     */
  XmInheritSetValuesPrehook,	/* set_values prehook     */
  InitializePosthook,		/* initialize posthook    */
  XmInheritSetValuesPosthook,	/* set_values posthook    */
  XmInheritClass,		/* secondary class        */
  XmInheritSecObjectCreate,	/* creation proc          */
  XmInheritGetSecResData,	/* getSecResData          */
  { 0 },			/* fast subclass          */
  XmInheritGetValuesPrehook,	/* get_values prehook     */
  XmInheritGetValuesPosthook,	/* get_values posthook    */
  NULL,				/* classPartInitPrehook   */
  NULL,				/* classPartInitPosthook  */
  NULL,				/* ext_resources          */
  NULL,				/* compiled_ext_resources */
  0,				/* num_ext_resources      */
  FALSE,			/* use_sub_resources      */
  XmInheritWidgetNavigable,	/* widgetNavigable        */
  XmInheritFocusChange		/* focusChange            */
};

static XmPrimitiveClassExtRec _XmLabelPrimClassExtRec = {
  NULL,					/* next_extension      */
  NULLQUARK,				/* record_type         */
  XmPrimitiveClassExtVersion,		/* version             */
  sizeof(XmPrimitiveClassExtRec),	/* record_size         */
  XmLabelGetBaselines,			/* widget_baseline     */
  XmLabelGetDisplayRect,		/* widget_display_rect */
  XmLabelMarginsProc,			/* widget_margins      */
};

externaldef (xmlabelclassrec) XmLabelClassRec xmLabelClassRec = {
  {
    (WidgetClass) &xmPrimitiveClassRec,	/* superclass	       */
    "XmLabel",				/* class_name	       */
    sizeof(XmLabelRec),			/* widget_size	       */
    ClassInitialize,			/* class_initialize    */
    ClassPartInitialize,		/* chained class init  */
    FALSE,				/* class_inited	       */
    Initialize,				/* initialize	       */
    NULL,				/* initialize hook     */
    XtInheritRealize,			/* realize	       */
    ActionsList,			/* actions	       */
    XtNumber(ActionsList),		/* num_actions	       */
    resources,				/* resources	       */
    XtNumber(resources),		/* num_resources       */
    NULLQUARK,				/* xrm_class	       */
    TRUE,				/* compress_motion     */
    XtExposeCompressMaximal,		/* compress_exposure   */
    TRUE,				/* compress enter/exit */
    FALSE,				/* visible_interest    */
    Destroy,				/* destroy	       */
    Resize,				/* resize	       */
    Redisplay,				/* expose	       */
    SetValues,				/* set_values	       */
    NULL,				/* set values hook     */
    SetValuesAlmost,			/* set values almost   */
    NULL,				/* get values hook     */
    NULL,				/* accept_focus	       */
    XtVersion,				/* version	       */
    NULL,				/* callback offsetlst  */
    NULL,				/* default trans       */
    QueryGeometry,			/* query geo proc      */
    NULL,				/* display accelerator */
    (XtPointer)&labelBaseClassExtRec	/* extension record    */
  },

  { /* XmPrimitiveClassPart */
    XmInheritBorderHighlight,		 /* border_highlight   */
    XmInheritBorderUnhighlight,		 /* border_unhighlight */
    XtInheritTranslations,		 /* translations       */
    NULL,				 /* arm_and_activate   */
    syn_resources,			 /* syn resources      */
    XtNumber(syn_resources),		 /* num syn_resources  */
    (XtPointer)&_XmLabelPrimClassExtRec, /* extension          */
  },

  { /* XmLabelClassPart */
    SetOverrideCallback,	/* override_callback             */
    NULL,			/* menu procedure interface      */
    NULL,			/* translations                  */
    NULL			/* extension record              */
  }
};

externaldef(xmlabelwidgetclass) WidgetClass xmLabelWidgetClass =
				(WidgetClass) &xmLabelClassRec;

/*********************************************************************
 *
 * ClassInitialize
 *       This is the class initialization routine.  It is called only
 *       the first time a widget of this class is initialized.
 *
 ********************************************************************/

/*ARGSUSED*/
static void
ClassInitialize(void)
{
  /* Parse the various translation tables */
  menu_parsed	 = XtParseTranslationTable(menuTranslations);
  default_parsed = XtParseTranslationTable(defaultTranslations);

  /* Set up base class extension quark */
  labelBaseClassExtRec.record_type = XmQmotif;

  xmLabelClassRec.label_class.translations =
    (String) (XtParseTranslationTable(menu_traversal_events));

  /* Install menu savvy on just this class */
  XmeTraitSet((XtPointer) &xmLabelClassRec,
	      XmQTmenuSavvy, (XtPointer) &MenuSavvyRecord);
}

void
_XmLabelCloneMenuSavvy(WidgetClass wc,
		       XmMenuSavvyTrait mst)
{
  /* Modify and reinstall menu savvy trait */
  if (mst->version == -1)
    {
      mst->version = MenuSavvyRecord.version;
      mst->disableCallback = MenuSavvyRecord.disableCallback;
      mst->getAccelerator = MenuSavvyRecord.getAccelerator;
      mst->getMnemonic = MenuSavvyRecord.getMnemonic;
    }

  /* Install the new record */
  XmeTraitSet((XtPointer) wc, XmQTmenuSavvy, (XtPointer) mst);
}

char* _XmCBNameActivate()
{
  return XmNactivateCallback;
}

char* _XmCBNameValueChanged()
{
  return XmNvalueChangedCallback;
}

/************************************************************
 *
 * InitializePosthook
 *	Restore core class translations.
 *
 ************************************************************/

/*ARGSUSED*/
static void
InitializePosthook(Widget req,		/* unused */
		   Widget new_w,
		   ArgList args,	/* unused */
		   Cardinal *num_args)	/* unused */
{
  _XmRestoreCoreClassTranslations (new_w);
}

/*********************************************************************
 *
 *  ClassPartInitialize
 *      Processes the class fields which need to be inherited.
 *
 ************************************************************************/
static void
ClassPartInitialize(WidgetClass c)
{
  register XmLabelWidgetClass wc = (XmLabelWidgetClass) c;
  XmLabelWidgetClass super = (XmLabelWidgetClass)wc->core_class.superclass;

  if (wc->label_class.setOverrideCallback == XmInheritSetOverrideCallback)
    wc->label_class.setOverrideCallback =
      super->label_class.setOverrideCallback;

  if (wc->label_class.translations == XtInheritTranslations)
    wc->label_class.translations = super->label_class.translations;

  _XmFastSubclassInit (c, XmLABEL_BIT);


  /* Install traits */
  XmeTraitSet((XtPointer) c, XmQTtransfer, (XtPointer) &LabelTransfer);
  XmeTraitSet((XtPointer) c, XmQTaccessTextual,
	      (XtPointer) &_XmLabel_AccessTextualRecord);
}

/************************************************************
 *
 * InitializePrehook
 *	Put the proper translations in core_class tm_table so
 * that the data is massaged correctly.
 *
 ************************************************************/

/*ARGSUSED*/
static void
InitializePrehook(Widget req,		/* unused */
		  Widget new_w,
		  ArgList args,		/* unused */
		  Cardinal *num_args)	/* unused */
{
  unsigned char type;
  XmMenuSystemTrait menuSTrait;

  _XmProcessLock();
  if (new_w->core.widget_class->core_class.tm_table != NULL) {
    _XmProcessUnlock();
    return;
  }

  _XmSaveCoreClassTranslations (new_w);

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass((Widget) XtParent(new_w)), XmQTmenuSystem);

  if (menuSTrait != (XmMenuSystemTrait) NULL)
    type = menuSTrait->type(XtParent(new_w));
  else
    type = XmWORK_AREA;

  if (type == XmWORK_AREA)
    new_w->core.widget_class->core_class.tm_table = (String) default_parsed;
  else
    new_w->core.widget_class->core_class.tm_table = (String) menu_parsed;
  _XmProcessUnlock();
}

/************************************************************************
 *
 *  SetNormalGC
 *      Create the normal and insensitive GC's for the gadget.
 *
 ************************************************************************/

static void
SetNormalGC(XmLabelWidget lw)
{
  XGCValues       values;
  XtGCMask        valueMask, dynamicMask;
  XFontStruct     *fs = (XFontStruct *) NULL;

  valueMask = GCForeground | GCBackground | GCGraphicsExposures;
  dynamicMask = GCClipMask | GCClipXOrigin | GCClipYOrigin;

  values.foreground = lw->primitive.foreground;
  values.background = lw->core.background_pixel;
  values.graphics_exposures = False;

  if (XmeRenderTableGetDefaultFont(lw->label.font, &fs))
    values.font = fs->fid, valueMask |= GCFont;

  lw->label.normal_GC = XtAllocateGC((Widget) lw, 0, valueMask, &values,
				     dynamicMask, 0);

#ifdef FIX_1381
  /*generally gray insensitive foreground (instead stipple)*/
  values.foreground =  _XmAssignInsensitiveColor((Widget)lw);
  values.background = lw->core.background_pixel;
#else
  valueMask |= GCFillStyle | GCStipple;
  values.foreground = lw->core.background_pixel;
  values.background = lw->primitive.foreground;
  values.fill_style = FillOpaqueStippled;
  values.stipple = _XmGetInsensitiveStippleBitmap((Widget) lw);
#endif

  lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, valueMask, &values,
					  dynamicMask, 0);
#ifdef FIX_1381
  /*light shadow for insensitive text (instead stipple)*/
  values.foreground = lw->primitive.top_shadow_color;
  lw->label.shadow_GC = XtAllocateGC((Widget) lw, 0, valueMask, &values,
					  dynamicMask, 0);
#endif

}

/************************************************************************
 *
 * _XmCalcLabelDimensions()
 *   Calculates the dimensions of the label text and pixmap, and updates
 *   the TextRect fields appropriately. Called at Initialize and SetValues.
 *   Also called by subclasses to recalculate label dimensions.
 *
 ************************************************************************/

void
_XmCalcLabelDimensions(Widget wid)
{
  XmLabelWidget newlw = (XmLabelWidget) wid;
  XmLabelPart  *lp = &(newlw->label);
  unsigned int  w = 0, h = 0;


  /* Initialize TextRect width and height to 0, change later if needed */
  lp->acc_TextRect.width = 0;
  lp->acc_TextRect.height = 0;
  lp->StringRect.x = 0;
  lp->StringRect.y = 0;
  lp->StringRect.width = 0;
  lp->StringRect.height = 0;
  lp->PixmapRect.x = 0;
  lp->PixmapRect.y = 0;
  lp->PixmapRect.width = 0;
  lp->PixmapRect.height = 0;

  if (Lab_IsPixmap(newlw) || Lab_IsPixmapAndText(newlw))
    {
      /* change NULL pixmap to refer to XmUNSPECIFIED_PIXMAP */
      if (Pix(newlw) == (Pixmap)None)
        Pix(newlw) = XmUNSPECIFIED_PIXMAP;
      if (Pix_insen(newlw) == (Pixmap)None)
        Pix_insen(newlw) = XmUNSPECIFIED_PIXMAP;

      /* Is a pixmap so find out how big it is. */
      if (XtIsSensitive(wid))
	{
	  if (Pix(newlw) != XmUNSPECIFIED_PIXMAP)
	    {
	      XmeGetPixmapData(XtScreen(newlw), Pix(newlw),
			       NULL, NULL, NULL, NULL, NULL, NULL,
			       &w, &h);

	      lp->PixmapRect.width = (unsigned short) w;
	      lp->PixmapRect.height = (unsigned short) h;
	    }
	}
      else
	{
	  Pixmap pix_use = Pix_insen (newlw) ;

	  if (pix_use == XmUNSPECIFIED_PIXMAP)
	      pix_use = Pix(newlw);

	  if (pix_use != XmUNSPECIFIED_PIXMAP)
	    {
	      XmeGetPixmapData(XtScreen(newlw), pix_use,
			       NULL, NULL, NULL, NULL, NULL, NULL,
			       &w, &h);

	      lp->PixmapRect.width = (unsigned short) w;
	      lp->PixmapRect.height = (unsigned short) h;
	    }
	}
    }

  if (Lab_IsText(newlw) || Lab_IsPixmapAndText(newlw))
    {
      Dimension w, h;

      if (!XmStringEmpty (lp->_label))
	{
	  /* If we have a string then size it. */
	  XmStringExtent(lp->font, lp->_label, &w, &h);
	  lp->StringRect.width = (unsigned short)w;
	  lp->StringRect.height = (unsigned short)h;
	}
    }

  _XmLabelCalcTextRect(wid);

 if (lp->_acc_text != NULL)
   {
     Dimension w, h;

     /* If we have a string then size it. */
     if (!XmStringEmpty (lp->_acc_text))
       {
         XmStringExtent(lp->font, lp->_acc_text, &w, &h);
         lp->acc_TextRect.width = (unsigned short)w;
         lp->acc_TextRect.height = (unsigned short)h;
       }
   }
}

void
_XmLabelCalcTextRect(Widget wid)
{
  XmLabelWidget newlw = (XmLabelWidget) wid;
  XmLabelPart  *lp = &(newlw->label);

  lp->TextRect.width = 0;
  lp->TextRect.height = 0;

  if (Lab_IsPixmap(newlw))
    {
      lp->TextRect.width = lp->PixmapRect.width;
      lp->TextRect.height = lp->PixmapRect.height;
    }
  else if (Lab_IsText(newlw))
    {
      lp->TextRect.width = lp->StringRect.width;
      lp->TextRect.height = lp->StringRect.height;
    }
  else if (Lab_IsPixmapAndText(newlw))
    {
      if (lp->pixmap_placement == XmPIXMAP_TOP ||
          lp->pixmap_placement == XmPIXMAP_BOTTOM)
	{
          lp->TextRect.height =	lp->PixmapRect.height +
          	lp->StringRect.height + lp->pixmap_text_padding;
          lp->TextRect.width = MAX(lp->StringRect.width, lp->PixmapRect.width);
	}
      else if (lp->pixmap_placement == XmPIXMAP_LEFT ||
	       lp->pixmap_placement == XmPIXMAP_RIGHT)
        {
          lp->TextRect.width = lp->PixmapRect.width +
          	lp->StringRect.width + lp->pixmap_text_padding;
          lp->TextRect.height = MAX(lp->StringRect.height, lp->PixmapRect.height);
        }

      if (lp->pixmap_placement == XmPIXMAP_TOP)
        {
	  lp->PixmapRect.y = 0;
	  lp->StringRect.y = lp->PixmapRect.height + lp->pixmap_text_padding;
        }
      else if (lp->pixmap_placement == XmPIXMAP_BOTTOM)
        {
          lp->StringRect.y = 0;
          lp->PixmapRect.y = lp->StringRect.height + lp->pixmap_text_padding;
        }
      else if ((lp->pixmap_placement == XmPIXMAP_RIGHT &&
	        LayoutIsRtoLP(wid)) ||
               (lp->pixmap_placement == XmPIXMAP_LEFT &&
                !LayoutIsRtoLP(wid)))
	{
          lp->PixmapRect.x = 0;
          lp->StringRect.x = lp->PixmapRect.width + lp->pixmap_text_padding;
	}
      else if ((lp->pixmap_placement == XmPIXMAP_LEFT &&
                LayoutIsRtoLP(wid)) ||
               (lp->pixmap_placement == XmPIXMAP_RIGHT &&
                !LayoutIsRtoLP(wid)))
	{
	   lp->StringRect.x = 0;
	   lp->PixmapRect.x = lp->StringRect.width + lp->pixmap_text_padding;
	}

      if (lp->pixmap_placement == XmPIXMAP_RIGHT ||
	    lp->pixmap_placement == XmPIXMAP_LEFT)
        {
          lp->PixmapRect.y = (lp->TextRect.height - lp->PixmapRect.height) / 2;
          lp->StringRect.y = (lp->TextRect.height - lp->StringRect.height) / 2;
        }
      else
        {
          if (lp->alignment == XmALIGNMENT_CENTER)
            {
              lp->PixmapRect.x =
	        (lp->TextRect.width - lp->PixmapRect.width) / 2;
              lp->StringRect.x =
              	(lp->TextRect.width - lp->StringRect.width) / 2;
            }
          else if ((lp->alignment == XmALIGNMENT_END &&
                     !LayoutIsRtoLP(wid)) ||
                    (lp->alignment == XmALIGNMENT_BEGINNING &&
                     LayoutIsRtoLP(wid)))
            {
              lp->PixmapRect.x = lp->TextRect.width - lp->PixmapRect.width;
              lp->StringRect.x = lp->TextRect.width - lp->StringRect.width;
            }
        }
    }
}

/************************************************************************
 *
 *  Resize
 *      Sets new width, new height, and new label.TextRect
 *      appropriately. This routine is called by Initialize and
 *      SetValues.
 *
 ************************************************************************/

static void
Resize(Widget wid)
{
  XmLabelWidget newlw = (XmLabelWidget) wid;
  XmLabelPart *lp = &(newlw->label);
  int leftx, rightx;

  /* Increase margin width if necessary to accomodate accelerator text. */
  if (lp->_acc_text != NULL)
    {
      if (LayoutIsRtoLP(newlw))
	{
          if (lp->margin_left < (Dimension) (lp->acc_TextRect.width + LABEL_ACC_PAD) )
	    {
	      int delta =
		lp->acc_TextRect.width + LABEL_ACC_PAD - lp->margin_left;
	      lp->acc_left_delta += delta;
	      lp->margin_left += delta;
	    }
	}
      else
	{
	  if (lp->margin_right < (Dimension) (lp->acc_TextRect.width + LABEL_ACC_PAD) )
	    {
	      int delta =
		lp->acc_TextRect.width + LABEL_ACC_PAD - lp->margin_right;
	      lp->acc_right_delta += delta;
	      lp->margin_right += delta;
	    }
	}
    }

  /* Has a width been specified?  */
  if (newlw->core.width == 0)
    newlw->core.width = (Dimension)
      lp->TextRect.width +
	lp->margin_left + lp->margin_right +
	  (2 * (lp->margin_width
		+ newlw->primitive.highlight_thickness
		+ newlw->primitive.shadow_thickness));

  leftx = (newlw->primitive.highlight_thickness +
	   newlw->primitive.shadow_thickness +
	   lp->margin_width + lp->margin_left);

  rightx = (newlw->core.width -
	    (newlw->primitive.highlight_thickness +
	     newlw->primitive.shadow_thickness +
	     lp->margin_width + lp->margin_right));

  switch (lp->alignment)
    {
    case XmALIGNMENT_BEGINNING:
      if (LayoutIsRtoLP(newlw))
	lp->TextRect.x = rightx - lp->TextRect.width;
      else
	lp->TextRect.x = leftx;
      break;

    case XmALIGNMENT_END:
      if (LayoutIsRtoLP(newlw))
	lp->TextRect.x = leftx;
      else
	lp->TextRect.x = rightx - lp->TextRect.width;
      break;

    default:
      /* CR 9737: Be careful about casting here, since rounding during */
      /*	division on Suns depends on the sign. */
      lp->TextRect.x = leftx + (rightx - leftx - (int)lp->TextRect.width) / 2;
      break;
    }

  /* Has a height been specified? */
  if (newlw->core.height == 0)
    newlw->core.height = (Dimension)
      MAX(lp->TextRect.height, lp->acc_TextRect.height) +
	lp->margin_top +
	  lp->margin_bottom
	    + (2 * (lp->margin_height
		    + newlw->primitive.highlight_thickness
		    + newlw->primitive.shadow_thickness));

  lp->TextRect.y =  (short) (newlw->primitive.highlight_thickness
			     + newlw->primitive.shadow_thickness
			     + lp->margin_height + lp->margin_top +
			     ((int) (newlw->core.height - lp->margin_top
			       - lp->margin_bottom
			       - (2 * (lp->margin_height
				       + newlw->primitive.highlight_thickness
				       + newlw->primitive.shadow_thickness))
			       - lp->TextRect.height) / 2));

  if (lp->_acc_text != NULL)
    {
      Dimension  base_label, base_accText, diff;

      if (LayoutIsRtoLP(newlw))
	lp->acc_TextRect.x = newlw->primitive.highlight_thickness +
	  newlw->primitive.shadow_thickness +
	    newlw->label.margin_width;
      else
	lp->acc_TextRect.x = (short) newlw->core.width -
	  newlw->primitive.highlight_thickness -
	    newlw->primitive.shadow_thickness -
	      newlw->label.margin_width -
		newlw->label.margin_right + LABEL_ACC_PAD;

      lp->acc_TextRect.y =
	(short) (newlw->primitive.highlight_thickness
		 + newlw->primitive.shadow_thickness
		 + lp->margin_height + lp->margin_top +
		 ((int) (newlw->core.height - lp->margin_top
		   - lp->margin_bottom
		   - (2 * (lp->margin_height
			   + newlw->primitive.highlight_thickness
			   + newlw->primitive.shadow_thickness))
				       - lp->acc_TextRect.height) / 2));

      /* make sure the label and accelerator text line up */
      /* when the fonts are different */

      if (Lab_IsText(newlw) || Lab_IsPixmapAndText(newlw))
	{
	  base_label = XmStringBaseline (lp->font, lp->_label);
	  base_accText = XmStringBaseline (lp->font, lp->_acc_text);

	  if (base_label > base_accText)
	    {
	      diff = base_label - base_accText;
	      lp->acc_TextRect.y = (short) lp->TextRect.y + diff - 1;
	    }
	  else if (base_label < base_accText)
	    {
	      diff = base_accText - base_label;
	      lp->TextRect.y = (short) lp->acc_TextRect.y + diff - 1;
	    }
	}
    }

  if (newlw->core.width == 0)    /* set core width and height to a */
    newlw->core.width = 1;       /* default value so that it doesn't */
  if (newlw->core.height == 0)   /* generate a Toolkit Error */
    newlw->core.height = 1;
}

/************************************************************
 *
 * Initialize
 *    This is the widget's instance initialize routine.  It is
 *    called once for each widget
 *
 ************************************************************/
/*ARGSUSED*/
static void
Initialize(
        Widget req,
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args)	/* unused */
{
  XmLabelWidget lw = (XmLabelWidget) new_w;
  XmMenuSystemTrait menuSTrait;
  XtTranslations	trans;

  lw->label.baselines = NULL;
  lw->label.computing_size = FALSE;

  /* if menuProcs is not set up yet, try again */
  if (xmLabelClassRec.label_class.menuProcs == NULL)
    xmLabelClassRec.label_class.menuProcs =
      (XmMenuProc) _XmGetMenuProcContext();

  /* Check for Invalid enumerated types */

  if (!XmRepTypeValidValue(XmRID_LABEL_TYPE, lw->label.label_type, (Widget) lw))
    {
      lw->label.label_type = XmSTRING;
    }

  if (!XmRepTypeValidValue(XmRID_ALIGNMENT, lw->label.alignment, (Widget) lw))
    {
      lw->label.alignment = XmALIGNMENT_CENTER;
    }

  if (!XmRepTypeValidValue(XmRID_PIXMAP_PLACEMENT, lw->label.pixmap_placement, (Widget) lw))
    {
      lw->label.pixmap_placement = XmPIXMAP_LEFT;
    }
#ifndef NO_XM_1_2_BC
  /*
   * Some pre-Motif 2.0 XmManager subclasses may be bypassing the
   * synthetic resouce GetValues hook and passing us the manager's raw
   * string_direction field (which is now a layout_direction).  Fixup
   * the common/simple cases.
   */
  switch (lw->label.string_direction)
    {
    case XmLEFT_TO_RIGHT:
    case XmRIGHT_TO_LEFT:
      /* These string directions are erroneous uses of layout directions. */
      lw->label.string_direction =
	XmDirectionToStringDirection(lw->label.string_direction);
      break;
    default:
      /* Other unknown layout directions will still get a warning. */
      break;
    }
#endif

  /* If layout_direction is set, it overrides string_direction.
   * If string_direction is set, but not layout_direction, use
   *	string_direction value.
   * If neither is set, get from parent
   */
  if (XmPrim_layout_direction(lw) != XmDEFAULT_DIRECTION) {
    if (lw->label.string_direction == XmDEFAULT_DIRECTION)
      lw->label.string_direction =
	XmDirectionToStringDirection(XmPrim_layout_direction(lw));
  } else if (lw->label.string_direction != XmDEFAULT_DIRECTION) {
    XmPrim_layout_direction(lw) =
      XmStringDirectionToDirection(lw->label.string_direction);
  } else {
    XmPrim_layout_direction(lw) = _XmGetLayoutDirection(XtParent(new_w));
    lw->label.string_direction =
      XmDirectionToStringDirection(XmPrim_layout_direction(lw));
  }

  if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION,
			   lw->label.string_direction, (Widget) lw))
    {
      lw->label.string_direction = XmSTRING_DIRECTION_L_TO_R;
    }

  /* Make a local copy of the font list */
  if (lw->label.font == NULL)
    {
      /* CR 2990: Let subclasses choose their own default font. */
      lw->label.font = XmeGetDefaultRenderTable (new_w, XmLABEL_FONTLIST);
    }
  lw->label.font = XmFontListCopy(lw->label.font);

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(new_w)), XmQTmenuSystem);

  /* get menu type and which button */
  if (menuSTrait != (XmMenuSystemTrait) NULL)
    lw->label.menu_type = menuSTrait->type(XtParent(new_w));
  else
    lw->label.menu_type = XmWORK_AREA;

  /*  Handle the label string :
   *   If label is the constant XmUNSPECIFIED, creates a empty XmString.
   *    (this is used by DrawnB instead of a default conversion that leads
   *     to a leak when someone provides a real label)
   *   If no label string is given accept widget's name as default.
   *     convert the widgets name to an XmString before storing;
   *   else
   *     save a copy of the given string.
   *     If the given string is not an XmString issue an warning.
   */
  if (lw->label._label == (XmString) XmUNSPECIFIED)
    {
      lw->label._label = XmeGetLocalizedString ((char *) NULL, /* reserved */
						(Widget) lw,
						XmNlabelString,
						"");
    }
  else if (lw->label._label == NULL)
    {
      lw->label._label = XmeGetLocalizedString ((char *) NULL, /* reserved */
						(Widget) lw,
						XmNlabelString,
						lw->core.name);
    }
  else if (XmeStringIsValid(lw->label._label))
    {
      lw->label._label= XmStringCopy(lw->label._label);
    }
  else
    {
      XmeWarning((Widget) lw, CS_STRING_MESSAGE);
      lw->label._label = XmStringCreateLocalized(lw->core.name);
    }

  /*
   * Convert the given mnemonicCharset to the internal Xm-form.
   */
  if  (lw->label.mnemonicCharset != NULL)
    lw->label.mnemonicCharset =
      _XmStringCharSetCreate (lw->label.mnemonicCharset);
  else
    lw->label.mnemonicCharset =
      _XmStringCharSetCreate (XmFONTLIST_DEFAULT_TAG);

  /* Accelerators are currently only supported in menus */
  if ((lw->label._acc_text != NULL) && Lab_IsMenupane(lw))
    {
      if (XmeStringIsValid((XmString) lw->label._acc_text))
        {
	  /*
	   * Copy the input string into local space, if
	   * not a Cascade Button
	   */
	  if (XmIsCascadeButton(lw))
	    lw->label._acc_text = NULL;
	  else
	    lw->label._acc_text =
	      XmStringCopy((XmString) lw->label._acc_text);
        }
      else
        {
	  XmeWarning((Widget) lw, ACC_MESSAGE);
	  lw->label._acc_text = NULL;
        }
    }
  else
    lw->label._acc_text = NULL;


  if ((lw->label.accelerator != NULL) && Lab_IsMenupane(lw))
    {
      /* Copy the accelerator into local space */
      lw->label.accelerator = XtNewString(lw->label.accelerator);
    }
  else
    lw->label.accelerator = NULL;

  lw->label.skipCallback = FALSE;

  lw->label.acc_right_delta = 0;
  lw->label.acc_left_delta = 0;

  /*  If zero width and height was requested by the application,  */
  /*  reset new_w's width and height to zero to allow Resize()   */
  /*  to operate properly.                                        */

  if (req->core.width == 0)
    lw->core.width = 0;

  if (req->core.height == 0)
    lw->core.height = 0;

  /* CR 6267:  Suppress highlight thickness before sizing also. */
  if ((lw->label.menu_type == XmMENU_POPUP) ||
      (lw->label.menu_type == XmMENU_PULLDOWN) ||
      (lw->label.menu_type == XmMENU_BAR))
    lw->primitive.highlight_thickness = 0;

  _XmCalcLabelDimensions(new_w);

  /* CR 7283: We can't use the resize method pointer, because */
  /*	subclasses haven't been initialized yet.	      */
  Resize((Widget) lw);

  SetNormalGC(lw);

  /* Force the label traversal flag when in a menu. */

  if ((XtClass(lw) == xmLabelWidgetClass) &&
      ((lw->label.menu_type == XmMENU_POPUP) ||
       (lw->label.menu_type == XmMENU_PULLDOWN) ||
       (lw->label.menu_type == XmMENU_OPTION)))
    {
      lw->primitive.traversal_on = FALSE;
      lw->primitive.highlight_on_enter = FALSE;
    }

  /* if in menu, override with menu traversal translations */
  if ((lw->label.menu_type == XmMENU_POPUP) ||
      (lw->label.menu_type == XmMENU_PULLDOWN) ||
      (lw->label.menu_type == XmMENU_BAR) ||
      (lw->label.menu_type == XmMENU_OPTION))
    {

      _XmProcessLock();
      trans = (XtTranslations)
	  ((XmLabelClassRec *)XtClass(lw))->label_class.translations;
      _XmProcessUnlock();

      XtOverrideTranslations((Widget) lw, trans);
    }
  else
    {

      _XmProcessLock();
      trans = (XtTranslations)
	  ((XmPrimitiveClassRec *) XtClass(lw))->primitive_class.translations;
      _XmProcessUnlock();

      /* Otherwise override with primitive traversal translations */
      XtOverrideTranslations((Widget) lw, trans);
    }
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
        XtWidgetGeometry *desired)
{
  XmLabelWidget lw = (XmLabelWidget) widget;

  if (lw->label.recompute_size == FALSE)
    {
      desired->width = XtWidth(widget);
      desired->height = XtHeight(widget);
    }
  else
    {
      desired->width = (Dimension) lw->label.TextRect.width +
	(2 * (lw->label.margin_width +
	      lw->primitive.highlight_thickness +
	      lw->primitive.shadow_thickness)) +
		lw->label.margin_left +
		  lw->label.margin_right;
      if (desired->width == 0)
	desired->width = 1;

      desired->height = (Dimension) MAX(lw->label.TextRect.height,
					lw->label.acc_TextRect.height)
	+ (2 * (lw->label.margin_height +
		lw->primitive.highlight_thickness +
		lw->primitive.shadow_thickness)) +
		  lw->label.margin_top +
		    lw->label.margin_bottom;
      if (desired->height == 0)
	desired->height = 1;
    }

  return XmeReplyToQueryGeometry(widget, intended, desired);
}

/************************************************************************
 *
 *  Destroy
 *      Free up the label gadget allocated space.  This includes
 *      the label, and GC's.
 *
 ************************************************************************/
static void
Destroy(
        Widget w)
{
  XmLabelWidget lw = (XmLabelWidget) w;

  if (lw->label._label != NULL)
    XmStringFree (lw->label._label);

  if (lw->label._acc_text != NULL)
    XmStringFree (lw->label._acc_text);

  if (lw->label.accelerator != NULL)
    XtFree (lw->label.accelerator);

  if (lw->label.font != NULL)
    XmFontListFree (lw->label.font);

  if (lw->label.mnemonicCharset != NULL)
    XtFree (lw->label.mnemonicCharset);

  if (lw->label.baselines != NULL)
    XtFree ((char*) lw->label.baselines);

  XtReleaseGC ((Widget) lw, lw->label.normal_GC);
  XtReleaseGC ((Widget) lw, lw->label.insensitive_GC);
#ifdef FIX_1381
  XtReleaseGC ((Widget) lw, lw->label.shadow_GC);
#endif
}

/************************************************************************
 *
 *  Redisplay
 *

 ***********************************************************************/
static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region)
{
  XmLabelWidget lw = (XmLabelWidget) wid;
  GC gc;
  GC clipgc = NULL;
  XRectangle clip_rect;
  XmLabelPart *lp;
  Dimension availW, availH, marginal_width, marginal_height, max_text_height;
  int depth;

  lp = &(lw->label);

  /*
   * Set a clipping area if the label will overwrite critical margins.
   * A critical margin is defined to be "margin_{left,right,
   * top,bottom} margin" because it used for other things
   * such as default_button_shadows and accelerators.   Note that
   * overwriting the "margin_{width,height}" margins is allowed.
   */
  availH = lw->core.height;
  availW = lw->core.width;

  /* Adjust definitions of temporary variables */
  marginal_width = lp->margin_left + lp->margin_right +
    (2 * (lw->primitive.highlight_thickness +
	  lw->primitive.shadow_thickness));

  marginal_height = lp->margin_top + lp->margin_bottom +
    (2 * (lw->primitive.highlight_thickness +
	  lw->primitive.shadow_thickness));

  max_text_height = MAX(lp->TextRect.height, lp->acc_TextRect.height);

  if (XtIsSensitive(wid))
    clipgc = lp->normal_GC;
  else
    clipgc = lp->insensitive_GC;

  if ((availH < (Dimension)(marginal_height + max_text_height)) ||
      (availW < (Dimension)(marginal_width + lp->TextRect.width)))
    {
      clip_rect.x = lw->primitive.highlight_thickness +
	lw->primitive.shadow_thickness + lp->margin_left;
      clip_rect.y = lw->primitive.highlight_thickness +
	lw->primitive.shadow_thickness + lp->margin_top;

      /* Don't allow negative dimensions. */
      if (availW > marginal_width)
	clip_rect.width = availW - marginal_width;
      else
	clip_rect.width = 0;

      if (availH > marginal_height)
	clip_rect.height = availH - marginal_height;
      else
	clip_rect.height = 0;

      XSetClipRectangles(XtDisplay(lw), clipgc, 0,0, &clip_rect, 1, Unsorted);
#ifdef FIX_1521
#if USE_XFT
      _XmXftSetClipRectangles(XtDisplay(lw), XtWindow(lw), 0, 0, &clip_rect, 1);
#endif
#endif
    } else
    {
      XSetClipMask (XtDisplay (lw), clipgc, None);
#ifdef FIX_1521
#if USE_XFT
      XftDraw* draw = _XmXftDrawCreate(XtDisplay(lw), XtWindow(lw));
      XftDrawSetClip(draw, NULL);
#endif
#endif
    }

#if USE_XFT
    /* it is needed to clear anti-aliased text before draw it again */
    if ((Lab_IsText (lw) || Lab_IsPixmapAndText(lw)) && (lp->_label != NULL)
		&& lp->TextRect.width > 0 && lp->TextRect.height > 0)
      {
        XClearArea(XtDisplay(lw), XtWindow(lw),
			lp->TextRect.x,
			lp->TextRect.y,
 			lp->TextRect.width,
			lp->TextRect.height, False);
     }
#endif

  if (Lab_IsPixmap(lw) || Lab_IsPixmapAndText(lw))
    {
      if (XtIsSensitive(wid))
	{
	  if (Pix (lw) != XmUNSPECIFIED_PIXMAP)
	    {
	      gc = lp->normal_GC;

	      XmeGetPixmapData(XtScreen(lw), Pix(lw), NULL, &depth,
			       NULL, NULL, NULL, NULL, NULL, NULL);

	      if (depth == lw->core.depth)
		XCopyArea (XtDisplay(lw), Pix(lw), XtWindow(lw), gc, 0, 0,
			   lp->PixmapRect.width,
			   lp->PixmapRect.height,
			   lp->TextRect.x + lp->PixmapRect.x,
			   lp->TextRect.y + lp->PixmapRect.y);
	      else if (depth == 1)
		XCopyPlane (XtDisplay(lw), Pix(lw), XtWindow(lw),
			    gc, 0, 0,
			    lp->PixmapRect.width,
			    lp->PixmapRect.height,
			    lp->TextRect.x + lp->PixmapRect.x,
			    lp->TextRect.y + lp->PixmapRect.y,
			    1);
	    }
	}
      else
	{
	  Pixmap pix_use = Pix_insen (lw) ;

	  if (pix_use == XmUNSPECIFIED_PIXMAP)
#ifdef FIX_1381
		Pix_insen(lw) = pix_use = _XmConvertToBW(wid, Pix(lw));
#else
		pix_use = Pix(lw);
#endif
	  if (pix_use != XmUNSPECIFIED_PIXMAP)
	    {
	      gc = lp->insensitive_GC;

	      XmeGetPixmapData(XtScreen(lw), pix_use, NULL, &depth,
			       NULL, NULL, NULL, NULL, NULL, NULL);

	      if (depth == lw->core.depth)
		XCopyArea (XtDisplay(lw), pix_use, XtWindow(lw),
			   gc, 0, 0,
			   lp->PixmapRect.width,
			   lp->PixmapRect.height,
			   lp->TextRect.x + lp->PixmapRect.x,
			   lp->TextRect.y + lp->PixmapRect.y);
	      else if (depth == 1)
		XCopyPlane (XtDisplay(lw), pix_use, XtWindow(lw),
			    gc, 0, 0,
			    lp->PixmapRect.width,
			    lp->PixmapRect.height,
			    lp->TextRect.x + lp->PixmapRect.x,
			    lp->TextRect.y + lp->PixmapRect.y,
			    1);

#ifndef FIX_1381
	      /* if no insensitive pixmap but a regular one, we need
 		 to do the stipple manually, since copyarea doesn't */
 	      if (pix_use == Pix(lw)) {
 		  /* need fill stipple, not opaque */
 		  XSetFillStyle(XtDisplay(lw), gc, FillStippled);
 		  XFillRectangle(XtDisplay(lw), XtWindow(lw), gc,
			lp->TextRect.x + lp->PixmapRect.x,
			lp->TextRect.y + lp->PixmapRect.y,
 			lp->PixmapRect.width,
			lp->PixmapRect.height);
 		  XSetFillStyle(XtDisplay(lw), gc, FillOpaqueStippled);

 	      }
#endif
#ifdef FIX_1505
	      if (pix_use == Pix(lw)) {
		  XSetFillStyle(XtDisplay(lw), gc, FillStippled);
		  XSetStipple(XtDisplay(lw), gc, _XmGetInsensitiveStippleBitmap((Widget)lw));
		  XFillRectangle(XtDisplay(lw), XtWindow(lw), gc,
			lp->TextRect.x + lp->PixmapRect.x,
			lp->TextRect.y + lp->PixmapRect.y,
			lp->PixmapRect.width,
			lp->PixmapRect.height);
		  XSetFillStyle(XtDisplay(lw), gc, FillSolid);

	      }
#endif
	    }
	}
    }

  if ((Lab_IsText (lw) || Lab_IsPixmapAndText(lw)) && (lp->_label != NULL))
    {
      if (lp->mnemonic != XK_VoidSymbol)
	{
	  /* CR 5181: Convert the mnemonic keysym to a character string. */
	  char tmp[MB_LEN_MAX * 2];
	  XmString underline;

 	  tmp[_XmOSKeySymToCharacter(lp->mnemonic, NULL, tmp)] = '\0';
	  underline = XmStringCreate(tmp, lp->mnemonicCharset);

#ifdef FIX_1381
	  if (XtIsSensitive(wid) )
	  {
		  /*Draw normal text*/
		  XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
				  lp->font, lp->_label,
				  lp->normal_GC,
				  lp->TextRect.x + lp->StringRect.x,
				  lp->TextRect.y + lp->StringRect.y,
				  lp->StringRect.width,
				  lp->alignment,
				  XmPrim_layout_direction(lw), NULL,
				  underline);
	  }
	  else
	  {
		  /*Draw shadow for insensitive text*/
		  XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
				  lp->font, lp->_label,
				  lp->shadow_GC,
				  lp->TextRect.x+1 + lp->StringRect.x,
				  lp->TextRect.y+1 + lp->StringRect.y,
				  lp->StringRect.width,
				  lp->alignment,
				  XmPrim_layout_direction(lw), NULL,
				  underline);
		  /*Draw insensitive text*/
		  XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
				  lp->font, lp->_label,
				  lp->insensitive_GC,
				  lp->TextRect.x + lp->StringRect.x,
				  lp->TextRect.y + lp->StringRect.y,
				  lp->StringRect.width,
				  lp->alignment,
				  XmPrim_layout_direction(lw), NULL,
				  underline);

	  }
#else
	  XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
				lp->font, lp->_label,
				(XtIsSensitive(wid) ?
				 lp->normal_GC : lp->insensitive_GC),
				lp->TextRect.x + lp->StringRect.x,
				lp->TextRect.y + lp->StringRect.y,
				lp->StringRect.width,
				lp->alignment,
				XmPrim_layout_direction(lw), NULL,
				underline);
#endif
	  XmStringFree(underline);
	}
      else
#ifdef FIX_1381
	{
	  if (XtIsSensitive(wid) )
	  {
	  /*Draw normal text*/
	  XmStringDraw (XtDisplay(lw), XtWindow(lw),
			  lp->font, lp->_label,
  			  lp->normal_GC,
			  lp->TextRect.x + lp->StringRect.x,
			  lp->TextRect.y + lp->StringRect.y,
			  lp->StringRect.width,
			  lp->alignment,
			  XmPrim_layout_direction(lw), NULL);
	  }
	else
	  {
	    /*Draw shadow for insensitive text*/
	    XmStringDraw (XtDisplay(lw), XtWindow(lw),
			  lp->font, lp->_label,
			  lp->shadow_GC,
			  lp->TextRect.x+1 + lp->StringRect.x,
			  lp->TextRect.y+1 + lp->StringRect.y,
			  lp->StringRect.width,
			  lp->alignment,
			  XmPrim_layout_direction(lw), NULL);
	    /*Draw insensitive text*/
	    XmStringDraw (XtDisplay(lw), XtWindow(lw),
			  lp->font, lp->_label,
			  lp->insensitive_GC,
			  lp->TextRect.x + lp->StringRect.x,
			  lp->TextRect.y + lp->StringRect.y,
			  lp->StringRect.width,
			  lp->alignment,
			  XmPrim_layout_direction(lw), NULL);
	  }
	}
#else
	XmStringDraw (XtDisplay(lw), XtWindow(lw),
		       lp->font, lp->_label,
		       (XtIsSensitive(wid) ?
			lp->normal_GC : lp->insensitive_GC),
		       lp->TextRect.x + lp->StringRect.x,
		       lp->TextRect.y + lp->StringRect.y,
		       lp->StringRect.width,
		       lp->alignment,
		       XmPrim_layout_direction(lw), NULL);
#endif

#ifndef FIX_1381
#if USE_XFT
      if (!XtIsSensitive(wid))
        {
          XSetFillStyle(XtDisplay(lw), lp->insensitive_GC, FillStippled);
          XFillRectangle(XtDisplay(lw), XtWindow(lw), lp->insensitive_GC,
			lp->TextRect.x + lp->StringRect.x,
			lp->TextRect.y + lp->StringRect.y,
 			lp->StringRect.width,
			lp->StringRect.height);
          XSetFillStyle(XtDisplay(lw), lp->insensitive_GC, FillOpaqueStippled);
        }
#endif
#endif

    }

  if (lp->_acc_text != NULL)
    {
      /* Since accelerator text  is drawn by moving in from the right,
       * it is possible to overwrite label text when there is clipping,
       * Therefore draw accelerator text only if there is enough
       * room for everything
       */

      if (lw->core.width >= (Dimension)
	  (2 * (lw->primitive.highlight_thickness +
		lw->primitive.shadow_thickness +
		lp->margin_width) +
	   lp->margin_left + lp->TextRect.width + lp->margin_right))
	     {
#ifdef FIX_1381
		  if (XtIsSensitive(wid) )
		  {
			  /*Draw normal text*/
			  XmStringDraw (XtDisplay(lw), XtWindow(lw),
					  lp->font, lp->_acc_text,
     				   lp->normal_GC,
					  lp->acc_TextRect.x, lp->acc_TextRect.y,
					  lp->acc_TextRect.width, XmALIGNMENT_END,
					  XmPrim_layout_direction(lw), NULL);
		  }
		  else
		  {
			/*Draw shadow for insensitive text*/
			XmStringDraw (XtDisplay(lw), XtWindow(lw),
					  lp->font, lp->_acc_text,
					   lp->shadow_GC,
					  lp->acc_TextRect.x+1, lp->acc_TextRect.y+1,
					  lp->acc_TextRect.width, XmALIGNMENT_END,
					  XmPrim_layout_direction(lw), NULL);
			/*Draw insensitive text*/
			XmStringDraw (XtDisplay(lw), XtWindow(lw),
					  lp->font, lp->_acc_text,
					   lp->insensitive_GC,
					  lp->acc_TextRect.x, lp->acc_TextRect.y,
					  lp->acc_TextRect.width, XmALIGNMENT_END,
					  XmPrim_layout_direction(lw), NULL);
		  }
#else
	XmStringDraw (XtDisplay(lw), XtWindow(lw),
		       lp->font, lp->_acc_text,
		       (XtIsSensitive(wid) ?
			lp->normal_GC : lp->insensitive_GC),
		       lp->acc_TextRect.x, lp->acc_TextRect.y,
		       lp->acc_TextRect.width, XmALIGNMENT_END,
		       XmPrim_layout_direction(lw), NULL);
#endif
	     }
    }

  /* Redraw the proper highlight  */
  if (! Lab_IsMenupane(lw) && Lab_MenuType(lw) != XmMENU_BAR)
    {
      XtExposeProc expose;

      _XmProcessLock();
      expose = (xmPrimitiveClassRec.core_class.expose);
      _XmProcessUnlock();

      /* Envelop our superclass expose method */
      (*(expose))((Widget) lw, event, region);
    }
}

/**********************************************************************
 *
 * Enter
 *
 *********************************************************************/

static void
Enter(Widget wid,
      XEvent *event,
      String *params,
      Cardinal *num_params)
{
  XmLabelWidget w = (XmLabelWidget) wid;

  if (w->primitive.highlight_on_enter)
    _XmPrimitiveEnter (wid, event, params, num_params);
}

/**********************************************************************
 *
 * Leave
 *
 *********************************************************************/

static void
Leave(Widget wid,
      XEvent *event,
      String *params,
      Cardinal *num_params)
{
  XmLabelWidget w = (XmLabelWidget) wid;

  if (w->primitive.highlight_on_enter)
    _XmPrimitiveLeave ((Widget) w, event, params, num_params);
}

/************************************************************************
 *
 * SetValues
 *	This routine will take care of any changes that have been made.
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
SetValues(Widget cw,
	  Widget rw,
	  Widget nw,
	  ArgList args,		/* unused */
	  Cardinal *num_args)	/* unused */
{
  XmLabelWidget current = (XmLabelWidget) cw;
  XmLabelWidget req = (XmLabelWidget) rw;
  XmLabelWidget new_w = (XmLabelWidget) nw;
  Boolean flag = FALSE;
  Boolean newstring = FALSE;
  XmLabelPart        *newlp, *curlp, *reqlp;
  Boolean ProcessFlag = FALSE;
  Boolean CleanupFontFlag = FALSE;
  Boolean Call_Resize = False;
  XmMenuSystemTrait menuSTrait;

  /* Get pointers to the label parts  */
  newlp = &(new_w->label);
  curlp = &(current->label);
  reqlp = &(req->label);


  /* Discard the baseline cache if it is no longer valid. */
  if ((newlp->_label != curlp->_label) ||
      (newlp->font != curlp->font))
    {
      if (newlp->baselines)
	{
	  XtFree ((char*) newlp->baselines);
	  newlp->baselines = NULL;
	}
    }

  /* If the label has changed, make a copy of the new label */
  /* and free the old label.                                */

  if (newlp->_label != curlp->_label)
    {
      newstring = TRUE;
      if (newlp->_label == NULL)
	{
          newlp->_label = XmStringCreateLocalized(new_w->core.name);
	}
      else
	{
	  if (XmeStringIsValid((XmString) newlp->_label))
	    newlp->_label = XmStringCopy((XmString) newlp->_label);
	  else
	    {
	      XmeWarning((Widget) new_w, CS_STRING_MESSAGE);

	      newlp->_label = XmStringCreateLocalized(new_w->core.name);
	    }
	}

      XmStringFree(curlp->_label);
      curlp->_label= NULL;
      reqlp->_label= NULL;
    }


  if (newlp->margin_right != curlp->margin_right)
    newlp->acc_right_delta = 0;
  if (newlp->margin_left != curlp->margin_left)
    newlp->acc_left_delta = 0;

  if ((newlp->_acc_text != curlp->_acc_text) &&
      Lab_IsMenupane(new_w))
    {
      /* BEGIN OSF Fix pir 1098 */
      newstring = TRUE;
      /* END OSF Fix pir 1098 */
      if (newlp->_acc_text != NULL)
	{
	  if (XmeStringIsValid((XmString) newlp->_acc_text))
	    {
	      if ((XmIsCascadeButton (new_w)) && (newlp->_acc_text != NULL))
		newlp->_acc_text = NULL;
	      else
		newlp->_acc_text = XmStringCopy((XmString) newlp->_acc_text);

	      XmStringFree(curlp->_acc_text);
	      curlp->_acc_text= NULL;
	      reqlp->_acc_text= NULL;
	    }
	  else
	    {
	      XmeWarning((Widget) new_w, ACC_MESSAGE);
	      newlp->_acc_text = NULL;
	      curlp->_acc_text= NULL;
	      reqlp->_acc_text= NULL;
	    }
	}
      /* BEGIN OSF Fix pir 1098 */
      else if (curlp->_acc_text)
	{
	  /* CR 3481:  Don't blindly force the margin back to 0; */
	  /* 	try to preserve the user specified value. */
	  if (LayoutIsRtoLP(new_w))
	    {
	      newlp->margin_left -= newlp->acc_left_delta;
	      newlp->acc_left_delta = 0;
	    }
	  else
	    {
	      newlp->margin_right -= newlp->acc_right_delta;
	      newlp->acc_right_delta = 0;
	    }
	}
      /* END OSF Fix pir 1098 */
    }
  else
    newlp->_acc_text = curlp->_acc_text;


  if (newlp->font != curlp->font)
    {
      CleanupFontFlag = True;
      if (newlp->font == NULL)
	{
	  /* CR 2990:  Let subclasses determine default fonts. */
	  newlp->font =
	    XmeGetDefaultRenderTable((Widget) new_w, XmLABEL_FONTLIST);
	}
      newlp->font = XmFontListCopy (newlp->font);

    }

  if ((new_w->label.menu_type == XmMENU_POPUP) ||
      (new_w->label.menu_type == XmMENU_PULLDOWN) ||
      (new_w->label.menu_type == XmMENU_BAR))
    new_w->primitive.highlight_thickness = 0;

  if (!XmRepTypeValidValue(XmRID_LABEL_TYPE, new_w->label.label_type,
			   (Widget) new_w))
    {
      new_w->label.label_type = current->label.label_type;
    }
#ifdef FIX_1484
  if (!XmRepTypeValidValue(XmRID_PIXMAP_PLACEMENT, new_w->label.pixmap_placement,
#else
  if (!XmRepTypeValidValue(XmRID_LABEL_TYPE, new_w->label.pixmap_placement,
#endif
			   (Widget) new_w))
    {
      new_w->label.pixmap_placement = current->label.pixmap_placement;
    }

  if (LayoutP(new_w) != LayoutP(current))
    {
      /* if no new margins specified swap them */
      if ((LayoutIsRtoLP(current) != LayoutIsRtoLP(new_w)) &&
	  (Lab_MarginLeft(current)  == Lab_MarginLeft(new_w)) &&
	  (Lab_MarginRight(current) == Lab_MarginRight(new_w)))
	{
	  Lab_MarginLeft(new_w)  = Lab_MarginRight(current);
	  Lab_MarginRight(new_w) = Lab_MarginLeft(current);
	}
      flag = TRUE;
    }

  /* ValidateInputs(new_w); */
#ifdef FIX_1504
  _XmCalcLabelDimensions((Widget) new_w);
  Boolean pixmap_size_changed = ((newlp->PixmapRect.width
      != curlp->PixmapRect.width) || (newlp->PixmapRect.height
      != curlp->PixmapRect.height));
#endif

  if (((Lab_IsText(new_w) || Lab_IsPixmapAndText(new_w)) &&
       ((newstring) ||
	(newlp->font != curlp->font))) ||
      ((Lab_IsPixmap(new_w) || Lab_IsPixmapAndText(new_w)) &&
       ((newlp->pixmap != curlp->pixmap) ||
	(newlp->pixmap_insen  != curlp->pixmap_insen) ||
	/* When you have different sized pixmaps for sensitive and */
	/* insensitive states and sensitivity changes, */
	/* the right size is chosen. (osfP2560) */
#ifdef FIX_1504
	(XtIsSensitive(nw) != XtIsSensitive(cw))) && pixmap_size_changed) ||
#else
	(XtIsSensitive(nw) != XtIsSensitive(cw)))) ||
#endif
      (Lab_IsPixmapAndText(new_w) &&
	(newlp->pixmap_placement != curlp->pixmap_placement)) ||
      (newlp->label_type != curlp->label_type))
    {
      /* CR 9179: Redo CR 5419 changes. */

      if (newlp->recompute_size)
	{
	  if (req->core.width == current->core.width)
	    new_w->core.width = 0;
	  if (req->core.height == current->core.height)
	    new_w->core.height = 0;
	}

#ifndef FIX_1504
      _XmCalcLabelDimensions((Widget) new_w);
#endif

      Call_Resize = True;

      flag = True;
    }
#ifdef FIX_1639
    /* Consider a case when new Pixmap has same size */
    else if ((Lab_IsPixmap(new_w) || Lab_IsPixmapAndText(new_w)) &&
             ((newlp->pixmap != curlp->pixmap) ||
	          (newlp->pixmap_insen  != curlp->pixmap_insen) ||
	          (XtIsSensitive(nw) != XtIsSensitive(cw))))
    {
      flag = True;
    }
#endif


  if ((newlp->alignment != curlp->alignment) ||
      (XmPrim_layout_direction(new_w) !=
       XmPrim_layout_direction(current)))
    {
      if (!XmRepTypeValidValue(XmRID_ALIGNMENT, new_w->label.alignment,
			       (Widget) new_w))
	{
	  new_w->label.alignment = current->label.alignment;
	}

      Call_Resize = True;

      flag = True;
    }

  if ((newlp->margin_height != curlp->margin_height) ||
      (newlp->margin_width != curlp->margin_width) ||
      (newlp->margin_left != curlp->margin_left) ||
      (newlp->margin_right != curlp->margin_right) ||
      (newlp->margin_top != curlp->margin_top) ||
      (newlp->margin_bottom != curlp->margin_bottom) ||
      (new_w->primitive.shadow_thickness !=
       current->primitive.shadow_thickness) ||
      (new_w->primitive.highlight_thickness !=
       current->primitive.highlight_thickness) ||
      ((new_w->core.width <= 0) || (new_w->core.height <= 0)) ||
      (newlp->pixmap_text_padding != curlp->pixmap_text_padding))
    {
      if (!XmRepTypeValidValue(XmRID_ALIGNMENT, new_w->label.alignment,
			       (Widget) new_w))
        {
	  new_w->label.alignment = current->label.alignment;
        }

      if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION,
			       new_w->label.string_direction, (Widget) new_w))
        {
	  new_w->label.string_direction = current->label.string_direction;
        }

      if (newlp->recompute_size)
        {
          if (req->core.width == current->core.width)
            new_w->core.width = 0;
          if (req->core.height == current->core.height)
            new_w->core.height = 0;
        }

      Call_Resize = True;

      flag = True;
    }


  /* Resize is called only if we need to calculate the dimensions or */
  /* coordinates  for the string.				     */
  if (Call_Resize)
    {
      XtWidgetProc resize;

      /* CR 5419: Suppress redundant calls to DrawnB's resize callbacks. */
      Boolean was_computing = newlp->computing_size;

      newlp->computing_size = TRUE;

      _XmProcessLock();
      resize = new_w->core.widget_class->core_class.resize;
      _XmProcessUnlock();

      (* (resize)) ((Widget) new_w);
      newlp->computing_size = was_computing;
    }



  /*
   * If sensitivity of the label has changed then we must redisplay
   *  the label.
   */
  if (XtIsSensitive(nw) != XtIsSensitive(cw))
    {
      flag = TRUE;
    }

  if ((new_w->primitive.foreground != current->primitive.foreground) ||
      (new_w->core.background_pixel != current->core.background_pixel) ||
      (newlp->font != curlp->font))
    {
      XtReleaseGC((Widget) current, current->label.normal_GC);
      XtReleaseGC((Widget) current, current->label.insensitive_GC);
      SetNormalGC(new_w);
      flag = TRUE;
    }

  /*  Force the traversal flag when in a menu.  */

  if ((XtClass(new_w) == xmLabelWidgetClass) &&
      ((new_w->label.menu_type == XmMENU_POPUP) ||
       (new_w->label.menu_type == XmMENU_PULLDOWN) ||
       (new_w->label.menu_type == XmMENU_OPTION)))
    {
      new_w->primitive.traversal_on = FALSE;
      new_w->primitive.highlight_on_enter = FALSE;
    }

  if (new_w->primitive.traversal_on &&
      (new_w->primitive.traversal_on != current->primitive.traversal_on) &&
      new_w->core.tm.translations)
    {

     XtTranslations trans;

     if ((new_w->label.menu_type == XmMENU_POPUP) ||
	  (new_w->label.menu_type == XmMENU_PULLDOWN) ||
	  (new_w->label.menu_type == XmMENU_BAR) ||
	  (new_w->label.menu_type == XmMENU_OPTION))
	{

	  _XmProcessLock();
	  trans = (XtTranslations)
	      ((XmLabelClassRec *)XtClass(new_w))->label_class.translations;
	  _XmProcessUnlock();
	  if (trans) XtOverrideTranslations((Widget)new_w, trans);
	}
      else
	{

	  _XmProcessLock();
	  trans = (XtTranslations)
	     ((XmLabelClassRec*) XtClass(new_w))->primitive_class.translations;
	  _XmProcessUnlock();
	  if (trans) XtOverrideTranslations ((Widget)new_w, trans);
	}
    }

  if ((new_w->label.menu_type != XmWORK_AREA) &&
      (new_w->label.mnemonic != current->label.mnemonic))
    {
      /* New grabs only required if mnemonic changes */
      ProcessFlag = TRUE;
      if (new_w->label.label_type == XmSTRING ||
          new_w->label.label_type == XmPIXMAP_AND_STRING)
	flag = TRUE;
    }

  if (new_w->label.mnemonicCharset != current->label.mnemonicCharset)
    {
      if (new_w->label.mnemonicCharset)
	new_w->label.mnemonicCharset =
	  _XmStringCharSetCreate(new_w->label.mnemonicCharset);
      else
	new_w->label.mnemonicCharset =
	  _XmStringCharSetCreate(XmFONTLIST_DEFAULT_TAG);

      if (current->label.mnemonicCharset != NULL)
	XtFree (current->label.mnemonicCharset);

      if (new_w->label.label_type == XmSTRING ||
          new_w->label.label_type == XmPIXMAP_AND_STRING)
	flag = TRUE;
    }

  if (Lab_IsMenupane(new_w) &&
      (new_w->label.accelerator != current->label.accelerator))
    {
      if (newlp->accelerator != NULL)
	{
	  /* Copy the accelerator into local space */
	  newlp->accelerator = XtNewString(newlp->accelerator);
	}

      XtFree(curlp->accelerator);
      curlp->accelerator = NULL;
      reqlp->accelerator = NULL;
      ProcessFlag = TRUE;
    }
  else
    newlp->accelerator = curlp->accelerator;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(current)), XmQTmenuSystem);

  if (ProcessFlag && menuSTrait != NULL)
    menuSTrait->updateBindings((Widget)new_w, XmREPLACE);

  if (flag &&
      (new_w->label.menu_type == XmMENU_PULLDOWN) && menuSTrait != NULL)
    menuSTrait->updateHistory(XtParent(new_w), (Widget) new_w, True);

  if (CleanupFontFlag)
    if (curlp->font) XmFontListFree(curlp->font);

  return flag;
}

/************************************************************************
 *
 *  SetActivateCallbackState
 *
 * This function is used as the method of the menuSavvy trait. It is
 * used by menu savvy parents to set whether or not the child will
 * invoke its own activate callback or whether it will defer to the
 * entryCallback of the parent.
 *
 ************************************************************************/

static void
SetActivateCallbackState(Widget          wid,
			 XmActivateState state)
{
  XmLabelWidget w = (XmLabelWidget) wid;

  switch (state)
    {
    case XmDISABLE_ACTIVATE:
      w->label.skipCallback = True;
      break;

    case XmENABLE_ACTIVATE:
      w->label.skipCallback = False;
      break;
    }
}

/************************************************************************
 *
 * SetOverrideCallback
 *	Used by subclasses.  If this is set true, then there is a RowColumn
 * parent with the entryCallback resource set.  The subclasses do not
 * do their activate callbacks, instead the RowColumn callbacks are called
 * by RowColumn.
 ************************************************************************/

static void
SetOverrideCallback(Widget wid)
{
  XmLabelWidget w = (XmLabelWidget) wid;

  w->label.skipCallback = True;
}

/************************************************************************
 *
 *  Help
 *      This routine is called if the user made a help selection
 *      on the widget.
 *
 ************************************************************************/

static void
Help(Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
  XmLabelWidget lw = (XmLabelWidget) w;
  Widget parent = XtParent(lw);
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(lw)), XmQTmenuSystem);

  if (Lab_IsMenupane(lw) && menuSTrait != NULL)
    menuSTrait->popdown((Widget) parent, event);

  _XmPrimitiveHelp((Widget) w, event, params, num_params);
}

/************************************************************************
 *
 * GetLabelString
 *	This is a get values hook function that returns the external
 * form of the label string from the internal form.
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
GetLabelString(Widget wid,
	       int resource,	/* unused */
	       XtArgVal *value)
{
  XmLabelWidget lw = (XmLabelWidget) wid;
  XmString string;

  string = XmStringCopy(lw->label._label);

  *value = (XtArgVal) string;
}

/************************************************************************
 *
 *  GetAccelerator
 *     This is a get values hook function that returns a copy
 *     of the accelerator string.
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
GetAccelerator(Widget wid,
	       int resource,	/* unused */
	       XtArgVal *value)
{
  XmLabelWidget lw = (XmLabelWidget) wid;
  String string;

  string = XtNewString(Lab_Accelerator(lw));

  *value = (XtArgVal) string;
}

/************************************************************************
 *
 *  GetAcceleratorText
 *     This is a get values hook function that returns the external
 *     form of the accelerator text from the internal form.
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
GetAcceleratorText(Widget wid,
		   int resource, /* unused */
		   XtArgVal *value)
{
  XmLabelWidget lw = (XmLabelWidget) wid;
  XmString string;

  string = XmStringCopy(lw->label._acc_text);

  *value = (XtArgVal) string;
}

/************************************************************************
 *
 *  XmCreateLabel()
 *  XmVaCreateLabel()
 *  XmVaCreateManagedLabel()
 *      Externally accessable function for creating a label gadget.
 *
 ************************************************************************/

Widget
XmCreateLabel(Widget parent,
	      char *name,
	      Arg *arglist,
	      Cardinal argCount)
{
  return XtCreateWidget(name,xmLabelWidgetClass,parent,arglist,argCount);
}

Widget
XmVaCreateLabel(
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
                         xmLabelWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}

Widget
XmVaCreateManagedLabel(
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
                         xmLabelWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}


static XmStringCharSet
_XmStringCharSetCreate(XmStringCharSet stringcharset)
{
  return (XmStringCharSet) XtNewString((char*) stringcharset);
}

/************************************************************************
 *
 * GetMnemonicCharSet
 *	 This is a get values hook function that returns the external
 * form of the mnemonicCharSet from the internal form.  Returns a
 * string containg the mnemonicCharSet.  Caller must free the string.
 ***********************************************************************/

/*ARGSUSED*/
static void
GetMnemonicCharSet(Widget wid,
		   int resource, /* unused */
		   XtArgVal *value)
{
  XmLabelWidget lw = (XmLabelWidget) wid;
  char *cset;
  int   size;

  cset = NULL;
  if (lw->label.mnemonicCharset)
    {
      size = strlen (lw->label.mnemonicCharset);
      if (size > 0)
	cset = (char *) (_XmStringCharSetCreate(lw->label.mnemonicCharset));
    }

  *value = (XtArgVal) cset;
}

/*ARGSUSED*/
static void
SetValuesAlmost(Widget cw,	/* unused */
		Widget nw,
		XtWidgetGeometry *request,
		XtWidgetGeometry *reply)
{
  XmLabelWidget new_w = (XmLabelWidget) nw;
  XtWidgetProc resize;

  _XmProcessLock();
  resize = new_w->core.widget_class->core_class.resize;
  _XmProcessUnlock();

  (* (resize)) ((Widget) new_w);
  *request = *reply;
}

/************************************************************************
 *
 * XmLabelGetDisplayRect
 *	A Class function which returns True if the widget being passed in
 * has a display rectangle associated with it.  It also determines the
 * x,y coordinates of the character cell or pixmap relative to the origin,
 * and the width and height in pixels of the smallest rectangle that encloses
 * the text or pixmap.  This is assigned to the variable being passed in
 *
 ***********************************************************************/

static Boolean
XmLabelGetDisplayRect(Widget w,
		      XRectangle *displayrect)
{
  XmLabelWidget wid = (XmLabelWidget) w;

  (*displayrect).x = wid->label.TextRect.x;
  (*displayrect).y = wid->label.TextRect.y;
  (*displayrect).width = wid->label.TextRect.width;
  (*displayrect).height = wid->label.TextRect.height;

  return TRUE;
}

/************************************************************************
 *
 * XmLabelGetBaselines
 *	A Class function which when called returns True, if the widget
 * has a baseline and also determines the number of pixels from the y
 * origin to the first line of text and assigns it to the variable
 * being passed in.
 *
 ************************************************************************/

static Boolean
XmLabelGetBaselines(Widget wid,
		    Dimension **baselines,
		    int *line_count)
{
  XmLabelWidget lw = (XmLabelWidget)wid;
  Cardinal count;
  int delta;

  if (Lab_IsPixmap(wid))
    return False;

  /* Compute raw baselines if unavailable. */
  if (lw->label.baselines == NULL)
    {
      _XmStringGetBaselines(lw->label.font, lw->label._label,
			    &(lw->label.baselines), &count);
      assert(lw->label.baselines != NULL);

      /* Store the current offset in an extra location. */
      lw->label.baselines = (Dimension*)
	XtRealloc((char*) lw->label.baselines, (count+1) * sizeof(Dimension));
      lw->label.baselines[count] = 0;
    }
  else
    {
      count = XmStringLineCount(lw->label._label);
    }

  /* Readjust offsets if necessary. */
  delta = Lab_TextRect_y(lw) - lw->label.baselines[count];
  if (delta)
    {
      int tmp;
      for (tmp = 0; tmp <= count; tmp++)
	lw->label.baselines[tmp] += delta;
    }

  /* Copy the cached data. */
  *line_count = count;
  *baselines = (Dimension*) XtMalloc(*line_count * sizeof(Dimension));
  memcpy((char*) *baselines, (char*) lw->label.baselines,
	 *line_count * sizeof(Dimension));

  return True;
}
/************************************************************************
 *
 * XmLabelMarginsProc
 *
 ***********************************************************************/

/* ARGSUSED */
static void
XmLabelMarginsProc(Widget w,
		   XmBaselineMargins *margins_rec)
{

  if (margins_rec->get_or_set == XmBASELINE_SET) {
    Lab_MarginTop(w) = margins_rec->margin_top;
    Lab_MarginBottom(w) = margins_rec->margin_bottom;
  } else {
    margins_rec->margin_top = Lab_MarginTop(w);
    margins_rec->margin_bottom = Lab_MarginBottom(w);
    margins_rec->shadow = Lab_Shadow(w);
    margins_rec->highlight = Lab_Highlight(w);
    margins_rec->text_height = Lab_TextRect_height(w);
    margins_rec->margin_height = Lab_MarginHeight(w);
  }
}


static Widget
GetPixmapDragIcon(Widget w)
{
  XmLabelWidget lw = (XmLabelWidget) w;
  Arg args[10];
  int n;
  Widget drag_icon;
  Widget screen_object = XmGetXmScreen(XtScreen(w));
  unsigned int width, height;
  int depth;

  /* It's a labelPixmap, use directly the pixmap */

  XmeGetPixmapData(XtScreen(lw), Pix(lw), NULL, &depth,
		   NULL, NULL, NULL, NULL, &width, &height);

  n = 0;
  XtSetArg(args[n], XmNhotX, 0),				n++;
  XtSetArg(args[n], XmNhotY, 0),				n++;
  XtSetArg(args[n], XmNwidth, width),				n++;
  XtSetArg(args[n], XmNheight, height),				n++;
  XtSetArg(args[n], XmNmaxWidth, width),			n++;
  XtSetArg(args[n], XmNmaxHeight, height),			n++;
  XtSetArg(args[n], XmNdepth, depth),				n++;
  XtSetArg(args[n], XmNpixmap, Pix(lw)),			n++;
  XtSetArg(args[n], XmNforeground, lw->core.background_pixel),	n++;
  XtSetArg(args[n], XmNbackground, lw->primitive.foreground),	n++;
  assert(n <= XtNumber(args));
  drag_icon = XtCreateWidget("drag_icon", xmDragIconObjectClass,
			     screen_object, args, n);
  return drag_icon;
}

/*ARGSUSED*/
static void
ProcessDrag(Widget w,
	    XEvent *event,
	    String *params,
	    Cardinal *num_params)
{
  XmLabelWidget lw = (XmLabelWidget) w;
  Widget drag_icon;
  Arg args[10];
  int n;
  Time _time = _XmGetDefaultTime(w, event);
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

  if (Lab_IsMenupane(w))
    XAllowEvents(XtDisplay(w), SyncPointer, _time);

  /* Disallow drag if this is a cascade button and armed - Hack alert */
  if (XmIsCascadeButton(w) && CB_IsArmed(w)) return;

  /* CDE - allow user to not drag labels and label subclasses
     also,  disable drag if enable_btn1_transfer is set to
     BUTTON2_ADJUST and the trigger was button2 */
  if (! dpy -> display.enable_unselectable_drag ||
      (dpy -> display.enable_btn1_transfer == XmBUTTON2_ADJUST &&
       event && event -> xany.type == ButtonPress &&
       event -> xbutton.button == 2)) return;

  /* CR 5141: Don't allow multi-button drags; they just cause confusion. */
  if (event && ! (event->xbutton.state &
	 ~((Button1Mask >> 1) << event->xbutton.button) &
	 (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask)))
    {
      n = 0;
      XtSetArg(args[n], XmNcursorBackground, lw->core.background_pixel), n++;
      XtSetArg(args[n], XmNcursorForeground, lw->primitive.foreground),  n++;

      /* If it's a labelPixmap, only specify the pixmap icon */
      if (Lab_IsPixmap(lw) && (Pix(lw) != XmUNSPECIFIED_PIXMAP))
	{
	  drag_icon = GetPixmapDragIcon(w);
	  XtSetArg(args[n], XmNsourcePixmapIcon, drag_icon), n++;
	}
      else
	{
	  drag_icon = XmeGetTextualDragIcon(w);
	  XtSetArg(args[n], XmNsourceCursorIcon, drag_icon), n++;
	}
      XtSetArg(args[n], XmNdragOperations, XmDROP_COPY), n++;
      (void) XmeDragSource(w, NULL, event, args, n);
    }
}

/*ARGSUSED*/
void
_XmLabelConvert(Widget w,
		XtPointer ignore,
		XmConvertCallbackStruct *cs)
{
  enum { XmA_MOTIF_COMPOUND_STRING, XmACOMPOUND_TEXT, XmATEXT,
	 XmATARGETS, XmA_MOTIF_DROP, XmABACKGROUND, XmAFOREGROUND,
	 XmAPIXEL, XmA_MOTIF_EXPORT_TARGETS,
	 XmA_MOTIF_CLIPBOARD_TARGETS,
#if XM_UTF8
	 XmAUTF8_STRING,
#endif
	 NUM_ATOMS };
  static char *atom_names[] = {
    XmS_MOTIF_COMPOUND_STRING, XmSCOMPOUND_TEXT, XmSTEXT,
    XmSTARGETS, XmS_MOTIF_DROP, XmIBACKGROUND, XmIFOREGROUND,
    XmIPIXEL, XmS_MOTIF_EXPORT_TARGETS, XmS_MOTIF_CLIPBOARD_TARGETS,
#if XM_UTF8
    XmSUTF8_STRING
#endif
    };

  Atom atoms[XtNumber(atom_names)];
  Atom C_ENCODING;
  int target_count = 0;
  Atom type = None;
  XtPointer value = NULL;
  unsigned long size = 0;
  int format = 8;
  XmString label_string;
  Pixmap label_pixmap;
  Boolean is_pixmap, is_text;

  if (w == (Widget) NULL)
    {
      cs->status = XmCONVERT_REFUSE;
      return;
    }

  C_ENCODING = XmeGetEncodingAtom(w);
  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplayOfObject(w), atom_names, XtNumber(atom_names),
	       False, atoms);

  if (cs->selection != atoms[XmA_MOTIF_DROP])
    {
      cs->status = XmCONVERT_REFUSE;
      return;
    }

  if (XtIsWidget(w))
    {
      label_string = ((XmLabelWidget) w)->label._label;
      label_pixmap = ((XmLabelWidget) w)->label.pixmap;
      is_pixmap = Lab_IsPixmap(w) || Lab_IsPixmapAndText(w);
      is_text = Lab_IsText(w) || Lab_IsPixmapAndText(w);
    }
  else
    {
      label_string = ((XmLabelGadget) w)->label._label;
      label_pixmap = ((XmLabelGadget) w)->label.pixmap;
      is_pixmap = LabG_IsPixmap(w) || LabG_IsPixmapAndText(w);;
      is_text = LabG_IsText(w) || LabG_IsPixmapAndText(w);
    }

  if (cs->target == atoms[XmATARGETS] ||
      cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS] ||
      cs->target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS])
    {
      Atom *targs;

      if (cs->target == atoms[XmATARGETS]) {
	targs = XmeStandardTargets(w, 7, &target_count);
      } else {
	target_count = 0;
	targs = (Atom *) XtMalloc(sizeof(Atom) * 7);
      }

      value = (XtPointer) targs;

      if (is_pixmap)
	{
	  targs[target_count] = XA_PIXMAP; target_count++;
	}

      if (is_text)
	{
	  XtPointer temp;
	  unsigned long length;
	  char* ctext;
	  Boolean success;

	  ctext = XmCvtXmStringToCT(label_string);
	  targs[target_count] = atoms[XmA_MOTIF_COMPOUND_STRING]; target_count++;
	  targs[target_count] = atoms[XmACOMPOUND_TEXT]; target_count++;
	  targs[target_count] = atoms[XmATEXT]; target_count++;
#if XM_UTF8
	  if (C_ENCODING != XA_STRING && C_ENCODING != atoms[XmAUTF8_STRING]) {
	    temp = ConvertToEncoding(w, ctext, C_ENCODING, &length, &success);
	    if (success) {
	      targs[target_count] = C_ENCODING;
	      target_count++;
	    }
	    XtFree((char*) temp);
	  }
#endif
	  temp = ConvertToEncoding(w, ctext, XA_STRING, &length, &success);
	  if (success) {
	    targs[target_count] = XA_STRING;
	    target_count++;
	  }
	  XtFree((char*) temp);
	  XtFree((char*) ctext);
#if XM_UTF8
	  ctext = XmCvtXmStringToUTF8String(label_string);
	  if (ctext) {
	    targs[target_count] = atoms[XmAUTF8_STRING];
	    target_count++;
	  }
	  XtFree((char*) ctext);
#endif
	}
      type = XA_ATOM;
      size = target_count;
      format = 32;
    }

  if (cs->target == atoms[XmA_MOTIF_COMPOUND_STRING])
    {
      type = atoms[XmA_MOTIF_COMPOUND_STRING];
      format = 8;
      size = XmCvtXmStringToByteStream(label_string,
				       (unsigned char **) &value);
    }
  else if (cs->target == atoms[XmACOMPOUND_TEXT] ||
	   cs->target == atoms[XmATEXT] ||
	   cs->target == XA_STRING ||
	   cs->target == C_ENCODING)
    {
      type = atoms[XmACOMPOUND_TEXT];
      format = 8;
      value = XmCvtXmStringToCT(label_string);
      if (value != NULL)
	size = strlen((char*) value);
      else
	size = 0;

      if (cs->target == XA_STRING)
	{
	  Boolean success;

	  value = ConvertToEncoding(w, (char*) value,
				    XA_STRING, &size, &success);

	  if (value != NULL && success == False)
	    cs->flags |= XmCONVERTING_PARTIAL;
	  type = XA_STRING;
	}
      else if ((cs->target == atoms[XmATEXT] ||
		cs->target == C_ENCODING) &&
	       value != NULL)
	{
	  char *cvt;
	  Boolean success;

	  cvt = (char*) ConvertToEncoding(w, (char*) value,
					  C_ENCODING, &size, &success);

	  if (cvt != NULL && success == False)
	    cs->flags |= XmCONVERTING_PARTIAL;

	  if (cvt != NULL && success)
	    {
	      /* 100% ok */
	      XtFree((char*) value);
	      value = cvt;
	      type = C_ENCODING;
	    }
	  else
	    {
	      /* If the request was C_ENCODING,  then return only C_ENCODING,
	       * not atoms[XmACOMPOUND_TEXT] */
	      if (cs->target == C_ENCODING)
		{
		  XtFree((char*) value);
		  value = cvt;
		  type = C_ENCODING;
		}
	      else
		if (cvt != NULL) XtFree(cvt);
	      type = atoms[XmACOMPOUND_TEXT];
	    }
	}
#if XM_UTF8
    } else if (cs->target == atoms[XmAUTF8_STRING]) {
      type = atoms[XmAUTF8_STRING];
      format = 8;
      value = XmCvtXmStringToUTF8String(label_string);
      if (value != NULL)
	size = strlen((char*) value);
      else
	size = 0;
#endif
    }

  if (cs->target == XA_PIXMAP)
    {
      /* Get widget's pixmap */
      Pixmap *pix;

      pix = (Pixmap *) XtMalloc(sizeof(Pixmap));
      *pix = label_pixmap;
      /* value, type, size, and format must be set */
      value = (XtPointer) pix;
      type = XA_DRAWABLE;
      size = 1;
      format = 32;
    }

  if (cs->target == atoms[XmABACKGROUND])
    {
      /* Get widget's background */
      Pixel *background;

      background = (Pixel *) XtMalloc(sizeof(Pixel));
      if (XtIsWidget(w))
	*background = ((XmLabelWidget) w)->core.background_pixel;
      else
	*background = LabG_Background(w);
      /* value, type, size, and format must be set */
      value = (XtPointer) background;
      type = atoms[XmAPIXEL];
      size = 1;
      format = 32;
    }

  if (cs->target == atoms[XmAFOREGROUND])
    {
      /* Get widget's foreground */
      Pixel *foreground;

      foreground = (Pixel *) XtMalloc(sizeof(Pixel));
      if (XtIsWidget(w))
	*foreground = ((XmLabelWidget) w)->primitive.foreground;
      else
	*foreground = LabG_Foreground(w);
      /* value, type, size, and format must be set */
      value = (XtPointer) foreground;
      type = atoms[XmAPIXEL];
      size = 1;
      format = 32;
    }

  if (cs->target == XA_COLORMAP)
    {
      /* Get widget's colormap */
      Colormap *colormap;

      colormap = (Colormap *) XtMalloc(sizeof(Colormap));
      if (XtIsWidget(w))
	*colormap = w->core.colormap;
      else
	*colormap = XtParent(w)->core.colormap;
      /* value, type, size, and format must be set */
      value = (XtPointer) colormap;
      type = XA_COLORMAP;
      size = 1;
      format = 32;
    }

  _XmConvertComplete(w, value, size, format, type, cs);
}

/**************************************************************************
 * ConvertToEncoding
 *
 * This contains conversion code which is needed more than once in
 * _XmLabelConvert.  The first calls are to check for 100% conversion
 * in _MOTIF_EXPORT_TARGETS and _MOTIF_CLIPBOARD_TARGETS.
 * The second are for the real conversion
 **************************************************************************/
static XtPointer
ConvertToEncoding(Widget w, char* str, Atom encoding,
		  unsigned long *length, Boolean *flag)
{
  XtPointer rval = NULL;
  XTextProperty tmp_prop;
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False);
#if XM_UTF8
  Atom UTF8_STRING = XInternAtom(XtDisplay(w), XmSUTF8_STRING, False);
#endif
  int ret_status;

  if (encoding == XA_STRING) {
    /* convert value to 8859.1 */
    ret_status =
      XmbTextListToTextProperty(XtDisplay(w), (char**) &str, 1,
				(XICCEncodingStyle)XStringStyle,
				&tmp_prop);

    if (ret_status == Success || ret_status > 0)
      {
	rval = (XtPointer) tmp_prop.value;
	*length = tmp_prop.nitems;
      }
    else
      {
	rval = NULL;
	*length = 0;
      }

    *flag = (ret_status == Success);
#if XM_UTF8
  } else if (encoding == UTF8_STRING) {
    /* convert value to UTF8 */
    ret_status =
      Xutf8TextListToTextProperty(XtDisplay(w), (char**) &str, 1,
				(XICCEncodingStyle)XUTF8StringStyle,
				&tmp_prop);

    if (ret_status == Success || ret_status > 0)
      {
	rval = (XtPointer) tmp_prop.value;
	*length = tmp_prop.nitems;
      }
    else
      {
	rval = NULL;
	*length = 0;
      }

    *flag = (ret_status >= Success);
#endif
  } else {
    /* Locale encoding */
    /* Fix for Bug 1117 - if str is null then a SEGVIOLATION occures
     * in strlen.
     */
#ifdef FIX_1442
    *length = str ? strlen(str) : 0;
#else
    *length = str ? strlen(rval) : 0;
#endif

    rval = _XmTextToLocaleText(w, (XtPointer) str,
			       COMPOUND_TEXT, 8,
			       *length,
			       flag);
  }

  return(rval);
}

/**************************************************************************
 * FromPaddingPixels
 *
 * Converts from pixels to current unit type does either horiz or vert
 * depending on icon placement.
 *  widget - the icon button widget.
 *  offset, value - passed to correct function based on orientation.
 **************************************************************************/

static void
FromPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    XmLabelWidget iw = (XmLabelWidget) widget;

    switch(iw->label.pixmap_placement) {
    case XmPIXMAP_TOP:
    case XmPIXMAP_BOTTOM:
	XmeFromVerticalPixels(widget, offset, value);
	break;
    default:			/* everything else is horiz. */
	XmeFromHorizontalPixels(widget, offset, value);
	break;
    }
}

/**************************************************************************
 * ToPaddingPixels
 *
 * Converts from pixels to current unit type does either horiz or vert
 * depending on icon placement.
 *  widget - the icon button widget.
 *  offset, value - passed to correct function based on orientation.
 * Returns the import order from _XmTo{Horizontal, Vertical}Pixels.
 **************************************************************************/

static XmImportOperator
ToPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    XmLabelWidget iw = (XmLabelWidget) widget;

    switch(iw->label.pixmap_placement) {
    case XmPIXMAP_TOP:
    case XmPIXMAP_BOTTOM:
	return(XmeToVerticalPixels(widget, offset, value));
    default:
	return(XmeToHorizontalPixels(widget, offset, value));
    }
}

/*
 * XmRCallProc routine for checking label.font before setting it to NULL
 * If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */

/*ARGSUSED*/
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmLabelWidget lw = (XmLabelWidget)wid;

  /* Check if been here before */
  if (lw->label.check_set_render_table)
      value->addr = NULL;
  else {
      lw->label.check_set_render_table = True;
      value->addr = (char*)&(lw->label.font);
  }

}

static XtPointer
LabelGetValue(Widget w, int type)
{
  XmString value;

  XtVaGetValues(w, XmNlabelString, &value, NULL);

  if (type == XmFORMAT_XmSTRING)
    {
      return (XtPointer) value;
    }
  else if (type == XmFORMAT_MBYTE ||
	   type == XmFORMAT_WCS)
    {
      XtPointer temp;
      temp = (type == XmFORMAT_MBYTE) ?
	_XmStringUngenerate(value, NULL,
			    XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT) :
	_XmStringUngenerate(value, NULL,
			    XmWIDECHAR_TEXT, XmWIDECHAR_TEXT);
      XmStringFree(value);
      return temp;
    }
  else
    return NULL;
}

static void
LabelSetValue(Widget w, XtPointer value, int type)
{
  Arg args[1];
  Cardinal nargs;
  XmString temp;
  Boolean freetemp = True;

  if (type == XmFORMAT_XmSTRING)
    {
      temp = (XmString) value;
      freetemp = False;
    }
  else
    {
      if (type == XmFORMAT_WCS)
	{
	  int length;
	  char *str;
	  wchar_t *str2;
	  str2 = (wchar_t *) value;

	  /* Get length of wchar string */
	  length = 0;
	  while (str2[length] != 0)
	    length++;
	  str = (char*) XtMalloc(MB_CUR_MAX * length);
	  wcstombs(str, str2, length * MB_CUR_MAX);
	  XtFree((char*) value);
	  value = str;
	}
      temp = XmStringCreateLocalized((char*) value);
    }

  nargs = 0;
  XtSetArg(args[nargs], XmNlabelString, temp), nargs++;
  assert(nargs <= XtNumber(args));
  XtSetValues(w, args, nargs);

  if (freetemp)
    XmStringFree(temp);
}

/*ARGSUSED*/
static int
LabelPreferredValue(Widget w)	/* unused */
{
  return XmFORMAT_XmSTRING;
}

static char*
GetLabelAccelerator(Widget w)
{
  if (XtClass(w) == xmLabelWidgetClass)
    return NULL;
  else
    return Lab_Accelerator(w);
}

static KeySym
GetLabelMnemonic(Widget w)
{
  if (XtClass(w) == xmLabelWidgetClass)
    return XK_VoidSymbol;
  else
    return Lab_Mnemonic(w);
}

