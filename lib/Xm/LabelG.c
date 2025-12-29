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
static char rcsid[] = "$TOG: LabelG.c /main/24 1999/01/26 15:31:18 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <X11/xpm.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/Xatom.h>
#include <X11/keysymdef.h>
#include <Xm/AccColorT.h>
#include <Xm/AccTextT.h>
#include <Xm/BaseClassP.h>
#include <Xm/CacheP.h>
#include <Xm/CareVisualT.h>
#include <Xm/CascadeBGP.h>
#include <Xm/DisplayP.h>
#include <Xm/DragC.h>
#include <Xm/DragIcon.h>
#include <Xm/DragIconP.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/ScreenP.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "ColorI.h"
#include "ExtObjectI.h"
#include "GadgetUtiI.h"
#include "LabelGI.h"
#include "MessagesI.h"
#include "MenuProcI.h"
#include "PixConvI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "SyntheticI.h"
#include "TravActI.h"
#include "XmI.h"
#include "XmosI.h"
#include "XmStringI.h"
#include <Xm/XmP.h>
#include <Xm/ColorI.h>

#if USE_XFT
#include "XmRenderTI.h"
#include <X11/Xft/Xft.h>
#endif

#define Pix(w)      LabG_Pixmap(w)
#define Pix_insen(w)    LabG_PixmapInsensitive(w)

/* Warning Messages */

#define CS_STRING_MESSAGE   _XmMMsgLabel_0003
#define ACC_MESSAGE     _XmMMsgLabel_0004

/********    Static Function Declarations    ********/

static Pixmap GetTopShadowPixmapDefault(Widget);
static Pixmap GetLabelHighlightPixmapDefault(Widget);
static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass cl);
static void SecondaryObjectCreate(Widget req, Widget new_w,
ArgList args, Cardinal *num_args);
static void InitializePosthook(Widget req, Widget new_w,
ArgList args, Cardinal *num_args);
static Boolean SetValuesPrehook(Widget oldParent,
Widget refParent,
Widget newParent,
ArgList args,
Cardinal *num_args);
static void GetValuesPrehook(Widget newParent,
ArgList args,
Cardinal *num_args);
static void GetValuesPosthook(Widget new_w,
ArgList args,
Cardinal *num_args);
static Boolean SetValuesPosthook(Widget current,
Widget req,
Widget new_w,
ArgList args,
Cardinal *num_args);
static void BorderHighlight(Widget w);
static void SetNormalGC(XmLabelGadget lw);
static void Resize(Widget wid);
static void Initialize(Widget req, Widget new_w,
ArgList args, Cardinal *num_args);
static XtGeometryResult QueryGeometry(Widget wid,
XtWidgetGeometry *intended,
XtWidgetGeometry *reply);
static void Destroy(Widget w);
static void Redisplay(Widget wid, XEvent *event, Region region);
static Boolean SetValues(Widget cw, Widget rw, Widget nw,
ArgList args, Cardinal *num_args);
static void InputDispatch(Widget wid, XEvent *event, Mask event_mask);
static void Help(Widget w, XEvent *event);
static void GetLabelString(Widget wid, int offset, XtArgVal *value);
static void GetAccelerator(Widget wid, int offset, XtArgVal *value);
static void GetAcceleratorText(Widget wid, int offset, XtArgVal *value);
static XmStringCharSet _XmStringCharsetCreate(XmStringCharSet stringcharset);
static void GetMnemonicCharset(Widget wid, int resource, XtArgVal *value);
static void QualifyLabelLocalCache(XmLabelGadget w);
static void SetOverrideCallback(Widget w);
static Cardinal GetLabelBGClassSecResData(WidgetClass w_class,
XmSecondaryResourceData **data_rtn);
static XtPointer GetLabelClassResBase(Widget widget, XtPointer client_data);
static void SetValuesAlmost(Widget cw, Widget nw,
XtWidgetGeometry *request, XtWidgetGeometry *reply);
static Boolean XmLabelGadgetGetBaselines(Widget wid,
Dimension **baselines,
int *line_count);
static Boolean XmLabelGadgetGetDisplayRect(Widget w,
XRectangle *displayrect);
static void XmLabelGadgetMarginsProc(Widget w,
XmBaselineMargins *margins_rec);
static Widget GetPixmapDragIcon(Widget w);
static void SetGadgetActivateCallbackState(Widget w, XmActivateState state);
static void CheckSetRenderTable(Widget wid, int offset, XrmValue *value);
static void FromPaddingPixels(Widget, int, XtArgVal *);
static XmImportOperator ToPaddingPixels(Widget, int, XtArgVal*);
static Boolean HandleRedraw(Widget kid,
Widget cur_parent,
Widget new_parent,
Mask visual_flag);
static void InitNewColorBehavior(XmLabelGadget lw);
static void DealWithColors(XmLabelGadget lw);
static void DealWithPixmaps(XmLabelGadget lw);
static void InitNewPixmapBehavior(XmLabelGadget lw);
static char* GetLabelGadgetAccelerator(Widget);
static KeySym GetLabelGadgetMnemonic(Widget);
static void GetColors(Widget widget, XmAccessColorData color_data);
/********    End Static Function Declarations    ********/

void _XmLabelConvert(Widget w, XtPointer ignore, XmConvertCallbackStruct*);

/* Transfer trait record */
static XmConst XmTransferTraitRec LabelGTransfer =
{
    0,                                            /* version		  */
    (XmConvertCallbackProc) _XmLabelConvert,      /* convertProc		  */
    NULL,                                         /* destinationProc	  */
    NULL                                          /* destinationPreHookProc */
};

/* Menu Savvy trait record */
static XmConst XmMenuSavvyTraitRec MenuSavvyGadgetRecord =
{
    0,                                            /* version	       */
    SetGadgetActivateCallbackState,               /* disableCallback   */
    GetLabelGadgetAccelerator,                    /* getAccelerator    */
    GetLabelGadgetMnemonic,                       /* getMnemonic       */
    NULL                                          /* getActivateCBName */
};

/* Used to be:
      extern XmConst XmAccessTextualTraitRec _XmLabel_AccessTextualRecord;
   but that was giving a linkage undefined with g++.
   It looks like a G++ bug. */
extern XmAccessTextualTraitRec _XmLabel_AccessTextualRecord;

static XmConst XmCareVisualTraitRec LabelGCVT =
{
    0,                                            /* version */
    HandleRedraw                                  /* redraw */
};

/* Access Colors Trait record for label gadget */
static XmConst XmAccessColorsTraitRec labACT =
{
    0,                                            /* version */
    GetColors,                                    /* getColors */
    NULL                                          /* setColors */
};

/*
 * Uncached resources for Label Gadget
 */

static XtResource resources[] =
{
    /* The following hackery is a way in Xt to see if a widget has been
     * initialized.  We need to know whether or not the gadget cache is
     * valid.  We use "." in the name so that an end user cannot access
     * it from a resource file.  With that in place, we just need to
     * check label.cache != NULL to see if the gadget has been
     * initialized and that we can access the colors for instance. */
    {
        XmNdotCache, XmCDotCache, XmRPointer,
        sizeof(XtPointer), XtOffsetOf(XmLabelGadgetRec, label.cache),
        XtRImmediate, (XtPointer) NULL
    },

    {
        XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
        sizeof(Dimension), XtOffsetOf(XmLabelGadgetRec, gadget.shadow_thickness),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNlabelPixmap, XmCLabelPixmap, XmRDynamicPixmap,
        sizeof(Pixmap), XtOffsetOf(XmLabelGadgetRec, label.pixmap),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },

    {
        XmNlabelInsensitivePixmap, XmCLabelInsensitivePixmap, XmRDynamicPixmap,
        sizeof(Pixmap), XtOffsetOf(XmLabelGadgetRec, label.pixmap_insen),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },

    {
        XmNlabelString, XmCXmString, XmRXmString,
        sizeof(XmString), XtOffsetOf(XmLabelGadgetRec, label._label),
        XmRImmediate, (XtPointer) NULL
    },

    {
        "pri.vate","Pri.vate", XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XmLabelGadgetRec,
        label.check_set_render_table),
        XmRImmediate, (XtPointer) False
    },

    {
        XmNfontList, XmCFontList, XmRFontList,
        sizeof(XmFontList), XtOffsetOf(XmLabelGadgetRec, label.font),
        XmRCallProc, (XtPointer)CheckSetRenderTable
    },

    {
        XmNrenderTable, XmCRenderTable, XmRRenderTable,
        sizeof(XmRenderTable), XtOffsetOf(XmLabelGadgetRec, label.font),
        XmRCallProc, (XtPointer)CheckSetRenderTable
    },

    {
        XmNmnemonic, XmCMnemonic, XmRKeySym,
        sizeof(KeySym), XtOffsetOf(XmLabelGadgetRec, label.mnemonic),
        XmRImmediate, (XtPointer) XK_VoidSymbol
    },

    {
        XmNmnemonicCharSet, XmCMnemonicCharSet, XmRString,
        sizeof(XmStringCharSet), XtOffsetOf(XmLabelGadgetRec,label.mnemonicCharset),
        XmRImmediate, (XtPointer)  XmFONTLIST_DEFAULT_TAG
    },

    {
        XmNaccelerator, XmCAccelerator, XmRString,
        sizeof(char *), XtOffsetOf(XmLabelGadgetRec, label.accelerator),
        XmRImmediate, (XtPointer) NULL
    },

    {
        XmNacceleratorText, XmCAcceleratorText, XmRXmString,
        sizeof(XmString), XtOffsetOf(XmLabelGadgetRec, label._acc_text),
        XmRImmediate, (XtPointer) NULL
    },

    {
        XmNtraversalOn, XmCTraversalOn, XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XmGadgetRec, gadget.traversal_on),
        XmRImmediate, (XtPointer) False
    },

    {
        XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
        sizeof(Dimension), XtOffsetOf(XmGadgetRec, gadget.highlight_thickness),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNpixmapPlacement, XmCPixmapPlacement, XmRPixmapPlacement,
        sizeof(XmPixmapPlacement), XtOffsetOf(XmLabelGadgetRec, label.pixmap_placement),
        XmRImmediate, (XtPointer) XmPIXMAP_LEFT
    },

    {
        XmNpixmapTextPadding, XmCSpace, XmRVerticalDimension,
        sizeof(Dimension), XtOffsetOf(XmLabelGadgetRec, label.pixmap_text_padding),
        XmRImmediate, (XtPointer) 2
    }
};

/* These 2 slots are needed for the delayed conversion. */
static XtResource label_pixmap_resource[] =
{
    {
        XmNlabelPixmap, XmCLabelPixmap, XmRDynamicPixmap,
        sizeof(Pixmap), XtOffsetOf(XmLabelGadgetRec, label.pixmap),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    }
};

static XtResource label_pixmap_insen_resource[] =
{
    {
        XmNlabelInsensitivePixmap, XmCLabelInsensitivePixmap, XmRDynamicPixmap,
        sizeof(Pixmap), XtOffsetOf(XmLabelGadgetRec, label.pixmap_insen),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    }
};

/*
 * Cached resources for Label Gadget
 */
static XtResource cache_resources[] =
{
    {
        XmNlabelType, XmCLabelType, XmRLabelType,
        sizeof(unsigned char),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.label_type),
        XmRImmediate, (XtPointer) XmSTRING
    },

    {
        XmNalignment, XmCAlignment, XmRAlignment,
        sizeof(unsigned char),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.alignment),
        XmRImmediate, (XtPointer) XmALIGNMENT_CENTER
    },

    {
        XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_width),
        XmRImmediate, (XtPointer) 2
    },

    {
        XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_height),
        XmRImmediate, (XtPointer) 2
    },

    {
        XmNmarginLeft, XmCMarginLeft, XmRHorizontalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_left),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNmarginRight, XmCMarginRight, XmRHorizontalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_right),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNmarginTop, XmCMarginTop, XmRVerticalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_top),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNmarginBottom, XmCMarginBottom, XmRVerticalDimension,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_bottom),
        XmRImmediate, (XtPointer) 0
    },

    {
        XmNrecomputeSize, XmCRecomputeSize, XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.recompute_size),
        XmRImmediate, (XtPointer) True
    },

    {
        XmNstringDirection, XmCStringDirection, XmRStringDirection,
        sizeof(unsigned char),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.string_direction),
        XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
    },

    {
        XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
        sizeof(XmDirection),
        XtOffsetOf(XmGadgetRec, gadget.layout_direction),
        XmRImmediate, (XtPointer) XmDEFAULT_DIRECTION
    },

    {
        XmNbackground, XmCBackground, XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.background),
        XmRImmediate, (XtPointer) INVALID_PIXEL
    },

    {
        XmNforeground, XmCForeground, XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.foreground),
        XmRImmediate, (XtPointer) INVALID_PIXEL
    },

    {
        XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.top_shadow_color),
        XmRImmediate, (XtPointer) INVALID_PIXEL
    },

    {
        XmNtopShadowPixmap, XmCTopShadowPixmap, XmRNoScalingDynamicPixmap,
        sizeof(Pixmap),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.top_shadow_pixmap),
        XmRImmediate, (XtPointer) INVALID_PIXMAP
    },

    {
        XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.bottom_shadow_color),
        XmRImmediate, (XtPointer) INVALID_PIXEL
    },

    {
        XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRNoScalingDynamicPixmap,
        sizeof(Pixmap),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.bottom_shadow_pixmap),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
    },

    {
        XmNhighlightColor, XmCHighlightColor, XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.highlight_color),
        XmRImmediate, (XtPointer) INVALID_PIXEL
    },

    {
        XmNhighlightPixmap, XmCHighlightPixmap, XmRNoScalingDynamicPixmap,
        sizeof(Pixmap),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.highlight_pixmap),
        XmRImmediate, (XtPointer) INVALID_PIXMAP
    }
};

/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
    {
        XmNlabelString,
        sizeof(XmString), XtOffsetOf(XmLabelGadgetRec, label._label),
        GetLabelString, NULL
    },

    {
        XmNaccelerator,
        sizeof(String), XtOffsetOf(XmLabelGadgetRec, label.accelerator),
        GetAccelerator, NULL
    },

    {
        XmNacceleratorText,
        sizeof(XmString), XtOffsetOf(XmLabelGadgetRec, label._acc_text),
        GetAcceleratorText, NULL
    },

    {
        XmNmnemonicCharSet,
        sizeof(XmStringCharSet), XtOffsetOf(XmLabelGadgetRec,label.mnemonicCharset),
        GetMnemonicCharset, NULL
    },

    {
        XmNpixmapTextPadding,
	sizeof(Dimension), XtOffsetOf(XmLabelGadgetRec, label.pixmap_text_padding),
        FromPaddingPixels, (XmImportProc) ToPaddingPixels
    }
};

static XmSyntheticResource cache_syn_resources[] =
{
    {
        XmNmarginWidth,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_width),
        XmeFromHorizontalPixels, XmeToHorizontalPixels
    },

    {
        XmNmarginHeight,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_height),
        XmeFromVerticalPixels, XmeToVerticalPixels
    },

    {
        XmNmarginLeft,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_left),
        XmeFromHorizontalPixels, XmeToHorizontalPixels
    },

    {
        XmNmarginRight,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_right),
        XmeFromHorizontalPixels, XmeToHorizontalPixels
    },

    {
        XmNmarginTop,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_top),
        XmeFromVerticalPixels, XmeToVerticalPixels
    },

    {
        XmNmarginBottom,
        sizeof(Dimension),
        XtOffsetOf(XmLabelGCacheObjRec, label_cache.margin_bottom),
        XmeFromVerticalPixels, XmeToVerticalPixels
    }
};

static XmCacheClassPart LabelClassCachePart =
{
    {                                             /* head of class cache list */
        NULL, 0, 0
    },
    _XmCacheCopy,                                 /* Copy routine		*/
    _XmCacheDelete,                               /* Delete routine	*/
    _XmLabelCacheCompare                          /* Comparison routine 	*/
};

static XmBaseClassExtRec labelBaseClassExtRec =
{
    NULL,                                         /* Next extension         */
    NULLQUARK,                                    /* record type XmQmotif   */
    XmBaseClassExtVersion,                        /* version                */
    sizeof(XmBaseClassExtRec),                    /* size                   */
    XmInheritInitializePrehook,                   /* initialize prehook     */
    SetValuesPrehook,                             /* set_values prehook     */
    InitializePosthook,                           /* initialize posthook    */
    SetValuesPosthook,                            /* set_values posthook    */
    (WidgetClass)&xmLabelGCacheObjClassRec,       /* secondary class        */
    SecondaryObjectCreate,                        /* creation proc          */
    GetLabelBGClassSecResData,                    /* getSecResData          */
    {                                             /* fast subclass          */
        0
    },
    GetValuesPrehook,                             /* get_values prehook     */
    GetValuesPosthook,                            /* get_values posthook    */
    NULL,                                         /* classPartInitPrehook   */
    NULL,                                         /* classPartInitPosthook  */
    NULL,                                         /* ext_resources          */
    NULL,                                         /* compiled_ext_resources */
    0,                                            /* num_ext_resources      */
    FALSE,                                        /* use_sub_resources      */
    XmInheritWidgetNavigable,                     /* widgetNavigable        */
    XmInheritFocusChange                          /* focusChange            */
};

externaldef (xmlabelgcacheobjclassrec)
XmLabelGCacheObjClassRec xmLabelGCacheObjClassRec =
{
    {
        (WidgetClass) &xmExtClassRec,             /* superclass          */
        "XmLabelGadget",                          /* class_name          */
        sizeof(XmLabelGCacheObjRec),              /* widget_size         */
        NULL,                                     /* class_initialize    */
        NULL,                                     /* chained class init  */
        False,                                    /* class_inited        */
        NULL,                                     /* initialize          */
        NULL,                                     /* initialize hook     */
        NULL,                                     /* realize             */
        NULL,                                     /* actions             */
        0,                                        /* num_actions         */
        cache_resources,                          /* resources           */
        XtNumber(cache_resources),                /* num_resources       */
        NULLQUARK,                                /* xrm_class           */
        False,                                    /* compress_motion     */
        False,                                    /* compress_exposure   */
        False,                                    /* compress enter/exit */
        False,                                    /* visible_interest    */
        NULL,                                     /* destroy             */
        NULL,                                     /* resize              */
        NULL,                                     /* expose              */
        NULL,                                     /* set_values          */
        NULL,                                     /* set values hook     */
        NULL,                                     /* set values almost   */
        NULL,                                     /* get values hook     */
        NULL,                                     /* accept_focus        */
        XtVersion,                                /* version             */
        NULL,                                     /* callback offsetlst  */
        NULL,                                     /* default trans       */
        NULL,                                     /* query geo proc      */
        NULL,                                     /* display accelerator */
        NULL                                      /* extension record    */
    },

    {
        cache_syn_resources,                      /* synthetic resources */
        XtNumber(cache_syn_resources),            /* num_syn_resources   */
        NULL                                      /* extension           */
    }
};

/* This is the class record that gets set at compile/link time  */
/* this is what is passed to the widget create routine as the   */
/* the class.  All fields must be inited at compile time.       */

static XmGadgetClassExtRec _XmLabelGadClassExtRec =
{
    NULL,                                         /* next_extension	*/
    NULLQUARK,                                    /* record_type		*/
    XmGadgetClassExtVersion,                      /* version		*/
    sizeof(XmGadgetClassExtRec),                  /* record_size		*/
    XmLabelGadgetGetBaselines,                    /* widget_baseline	*/
    XmLabelGadgetGetDisplayRect,                  /* widget_display_rect	*/
    XmLabelGadgetMarginsProc,                     /* widget_margins */
};

externaldef (xmlabelgadgetclassrec)
XmLabelGadgetClassRec xmLabelGadgetClassRec =
{
    {
        (WidgetClass) &xmGadgetClassRec,          /* superclass	     */
        "XmLabelGadget",                          /* class_name	     */
        sizeof(XmLabelGadgetRec),                 /* widget_size	     */
        ClassInitialize,                          /* class_initialize    */
        ClassPartInitialize,                      /* chained class init  */
        False,                                    /* class_inited	     */
        Initialize,                               /* initialize	     */
        NULL,                                     /* initialize hook     */
        NULL,                                     /* realize	     */
        NULL,                                     /* actions	     */
        0,                                        /* num_actions	     */
        resources,                                /* resources	     */
        XtNumber(resources),                      /* num_resources	     */
        NULLQUARK,                                /* xrm_class	     */
        True,                                     /* compress_motion     */
        XtExposeCompressMaximal,                  /* compress_exposure   */
        True,                                     /* compress enter/exit */
        False,                                    /* visible_interest    */
        Destroy,                                  /* destroy	     */
        Resize,                                   /* resize		     */
        Redisplay,                                /* expose		     */
        SetValues,                                /* set_values	     */
        NULL,                                     /* set values hook     */
        SetValuesAlmost,                          /* set values almost   */
        NULL,                                     /* get values hook     */
        NULL,                                     /* accept_focus	     */
        XtVersion,                                /* version	     */
        NULL,                                     /* callback offsetlst  */
        NULL,                                     /* default trans	     */
        QueryGeometry,                            /* query geo proc	     */
        NULL,                                     /* display accelerator */
        (XtPointer)&labelBaseClassExtRec          /* extension record    */
    },

    {                                             /* XmGadgetClassPart  */
        BorderHighlight,                          /* border_highlight   */
        XmInheritBorderUnhighlight,               /* border_unhighlight */
        NULL,                                     /* arm_and_activate   */
        InputDispatch,                            /* input dispatch     */
        XmInheritVisualChange,                    /* visual_change      */
        syn_resources,                            /* syn resources      */
        XtNumber(syn_resources),                  /* num syn_resources  */
        &LabelClassCachePart,                     /* class cache part   */
        (XtPointer)&_XmLabelGadClassExtRec        /* extension          */
    },

    {                                             /* XmLabelGadgetClassPart   */
        SetOverrideCallback,                      /* override_callback        */
        NULL,                                     /* menu procedure interface */
        NULL                                      /* extension record         */
    }
};

externaldef(xmlabelgadgetclass) WidgetClass xmLabelGadgetClass =
(WidgetClass) &xmLabelGadgetClassRec;

static Pixmap
GetTopShadowPixmapDefault(Widget widget)
{
    XmLabelGadget lg = (XmLabelGadget) widget;
    XmManagerWidget mw = (XmManagerWidget)XtParent(lg);
    Pixmap result = XmUNSPECIFIED_PIXMAP;

    if (LabG_TopShadowColor(lg) == LabG_Background(lg))
        result = XmGetPixmapByDepth (XtScreen (lg), XmS50_foreground,
            LabG_TopShadowColor(lg),
            LabG_Foreground(lg),
            mw->core.depth);

    else if (DefaultDepthOfScreen (XtScreen (widget)) == 1)
        result = XmGetPixmapByDepth (XtScreen (lg), XmS50_foreground,
                LabG_TopShadowColor(lg),
                LabG_Background(lg),
                mw->core.depth);

    return result;
}


static Pixmap
GetLabelHighlightPixmapDefault(Widget widget)
{
    XmLabelGadget lg = (XmLabelGadget) widget;
    XmManagerWidget mw = (XmManagerWidget)XtParent(lg);
    Pixmap result = XmUNSPECIFIED_PIXMAP;

    if (LabG_HighlightColor(lg) == LabG_Background(lg))
        result = XmGetPixmapByDepth (XtScreen (lg), XmS50_foreground,
            LabG_HighlightColor(lg),
            LabG_Foreground(lg),
            mw->core.depth);

    return result;
}


/*******************************************************************
 *
 *  _XmLabelCacheCompare
 *
 *******************************************************************/

int
_XmLabelCacheCompare(
XtPointer A,
XtPointer B)
{
    XmLabelGCacheObjPart *label_inst = (XmLabelGCacheObjPart *) A;
    XmLabelGCacheObjPart *label_cache_inst = (XmLabelGCacheObjPart *) B;

    if ((label_inst->label_type == label_cache_inst->label_type) &&
        (label_inst->alignment == label_cache_inst->alignment) &&
        (label_inst->string_direction == label_cache_inst->string_direction) &&
        (label_inst->margin_height == label_cache_inst->margin_height) &&
        (label_inst->margin_width == label_cache_inst->margin_width) &&
        (label_inst->margin_left == label_cache_inst->margin_left) &&
        (label_inst->margin_right == label_cache_inst->margin_right) &&
        (label_inst->margin_top == label_cache_inst->margin_top) &&
        (label_inst->margin_bottom == label_cache_inst->margin_bottom) &&
        (label_inst->recompute_size == label_cache_inst->recompute_size) &&
        (label_inst->skipCallback == label_cache_inst->skipCallback) &&
        (label_inst->menu_type == label_cache_inst->menu_type) &&
        (label_inst->background_GC == label_cache_inst->background_GC) &&
        (label_inst->top_shadow_GC == label_cache_inst->top_shadow_GC) &&
        (label_inst->bottom_shadow_GC == label_cache_inst->bottom_shadow_GC) &&
        (label_inst->highlight_GC == label_cache_inst->highlight_GC) &&
        (label_inst->foreground == label_cache_inst->foreground) &&
        (label_inst->background == label_cache_inst->background) &&
        (label_inst->top_shadow_color == label_cache_inst->top_shadow_color) &&
        (label_inst->top_shadow_pixmap == label_cache_inst->top_shadow_pixmap) &&
        (label_inst->bottom_shadow_color ==
        label_cache_inst->bottom_shadow_color) &&
        (label_inst->bottom_shadow_pixmap ==
        label_cache_inst->bottom_shadow_pixmap) &&
        (label_inst->highlight_color == label_cache_inst->highlight_color) &&
        (label_inst->highlight_pixmap == label_cache_inst->highlight_pixmap))
        return 1;
    else
        return 0;
}


/***********************************************************
 *
 *  ClassInitialize
 *
 ************************************************************/

static void
ClassInitialize(void)
{
    labelBaseClassExtRec.record_type = XmQmotif;

    /* Install menu savvy on just this class */
    XmeTraitSet((XtPointer) &xmLabelGadgetClassRec,
        XmQTmenuSavvy, (XtPointer) &MenuSavvyGadgetRecord);

}


void
_XmLabelGCloneMenuSavvy(WidgetClass wc,
XmMenuSavvyTrait mst)
{
    /* Modify and reinstall menu savvy trait */
    if (mst->version == -1)
    {
        mst->version = MenuSavvyGadgetRecord.version;
        mst->disableCallback = MenuSavvyGadgetRecord.disableCallback;
        mst->getAccelerator = MenuSavvyGadgetRecord.getAccelerator;
        mst->getMnemonic = MenuSavvyGadgetRecord.getMnemonic;
    }

    /* Install the new record */
    XmeTraitSet((XtPointer) wc, XmQTmenuSavvy, (XtPointer) mst);
}


/************************************************************************
 *
 *  ClassPartInitialize
 *	Processes the class fields which need to be inherited.
 *
 ************************************************************************/

static void
ClassPartInitialize(WidgetClass cl)
{
    register XmLabelGadgetClass wc = (XmLabelGadgetClass) cl;
    XmLabelGadgetClass super = (XmLabelGadgetClass)wc->rect_class.superclass;
    XmGadgetClassExt *wcePtr, *scePtr;

    if (wc->label_class.setOverrideCallback == XmInheritSetOverrideCallback)
        wc->label_class.setOverrideCallback =
            super->label_class.setOverrideCallback;

    if (wc->rect_class.resize == XmInheritResize)
        wc->rect_class.resize = super->rect_class.resize;

    wcePtr = _XmGetGadgetClassExtPtr(wc, NULLQUARK);
    scePtr = _XmGetGadgetClassExtPtr(super, NULLQUARK);

    if ((*wcePtr)->widget_baseline == XmInheritBaselineProc)
        (*wcePtr)->widget_baseline = (*scePtr)->widget_baseline;

    if ((*wcePtr)->widget_display_rect == XmInheritDisplayRectProc)
        (*wcePtr)->widget_display_rect  = (*scePtr)->widget_display_rect;

    _XmFastSubclassInit (((WidgetClass)wc), XmLABEL_GADGET_BIT);

    /* Install transfer trait */
    XmeTraitSet((XtPointer)cl, XmQTtransfer, (XtPointer) &LabelGTransfer);
    XmeTraitSet((XtPointer) cl, XmQTaccessTextual, (XtPointer)
        &_XmLabel_AccessTextualRecord);

    /* Install the careParentVisual trait for all subclasses as well. */
    XmeTraitSet((XtPointer)cl, XmQTcareParentVisual, (XtPointer)&LabelGCVT);

    /* Install the accessColors trait for all subclasses as well. */
    XmeTraitSet((XtPointer)cl, XmQTaccessColors, (XtPointer)&labACT);
}


/************************************************************************
 *
 *  SecondaryObjectCreate
 *
 ************************************************************************/

static void
SecondaryObjectCreate(Widget req,
Widget new_w,
ArgList args,
Cardinal *num_args)
{
    XmBaseClassExt *cePtr;
    XmWidgetExtData extData;
    WidgetClass   wc;
    Cardinal  size;
    XtPointer newSec, reqSec;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);

    wc = (*cePtr)->secondaryObjectClass;
    size = wc->core_class.widget_size;

    newSec = _XmExtObjAlloc(size);
    reqSec = _XmExtObjAlloc(size);
    _XmProcessUnlock();

    /*
     * Update pointers in instance records now so references to resources
     * in the cache record will be valid for use in CallProcs.
     * CallProcs are invoked by XtGetSubresources().
     */
    LabG_Cache(new_w) = &(((XmLabelGCacheObject)newSec)->label_cache);
    LabG_Cache(req) = &(((XmLabelGCacheObject)reqSec)->label_cache);

    /*
     * Fetch the resources in superclass to subclass order
     */

    XtGetSubresources(new_w, newSec, NULL, NULL,
        wc->core_class.resources,
        wc->core_class.num_resources,
        args, *num_args);

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;

    ((XmLabelGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
    ((XmLabelGCacheObject)newSec)->ext.logicalParent = new_w;

    _XmPushWidgetExtData((Widget) new_w, extData,
        ((XmLabelGCacheObject)newSec)->ext.extensionType);
    memcpy(reqSec, newSec, size);
}


/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/

static void
InitializePosthook(Widget req,
Widget new_w,
ArgList args,
Cardinal *num_args)
{
    XmWidgetExtData     ext;
    XmLabelGadget lw = (XmLabelGadget)new_w;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    _XmProcessLock();
    LabG_Cache(lw) = (XmLabelGCacheObjPart *)
        _XmCachePart(LabG_ClassCachePart(lw),
        (XtPointer) LabG_Cache(lw),
        sizeof(XmLabelGCacheObjPart));

    /*
     * Might want to break up into per-class work that gets explicitly
     * chained.  For right now, each class has to replicate all
     * superclass logic in hook routine.
     */

    /*
     * free the req subobject used for comparisons
     */
    _XmPopWidgetExtData((Widget) lw, &ext, XmCACHE_EXTENSION);
    _XmExtObjFree((XtPointer)ext->widget);
    _XmExtObjFree((XtPointer)ext->reqWidget);
    _XmProcessUnlock();
    XtFree((char *) ext);
}


/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/

static Boolean
SetValuesPrehook(Widget oldParent,
Widget refParent,
Widget newParent,
ArgList args,
Cardinal *num_args)
{
    XmWidgetExtData     extData;
    XmBaseClassExt      *cePtr;
    WidgetClass         ec;
    XmLabelGCacheObject newSec, reqSec;
    Cardinal        size;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);
    reqSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);
    _XmProcessUnlock();

    newSec->object.self = (Widget)newSec;
    newSec->object.widget_class = ec;
    newSec->object.parent = XtParent(newParent);
    newSec->object.xrm_name = newParent->core.xrm_name;
    newSec->object.being_destroyed = False;
    newSec->object.destroy_callbacks = NULL;
    newSec->object.constraints = NULL;

    newSec->ext.logicalParent = newParent;
    newSec->ext.extensionType = XmCACHE_EXTENSION;

    memcpy(&(newSec->label_cache),
        LabG_Cache(newParent),
        sizeof(XmLabelGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    XtSetSubvalues((XtPointer)newSec,
        ec->core_class.resources,
        ec->core_class.num_resources,
        args, *num_args);

    _XmExtImportArgs((Widget)newSec, args, num_args);

    memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

    LabG_Cache(newParent) = &((newSec)->label_cache);
    LabG_Cache(refParent) = &((reqSec)->label_cache);

    return FALSE;
}


/************************************************************************
 *
 *  GetValuesPrehook
 *
 ************************************************************************/

static void
GetValuesPrehook(Widget newParent,
ArgList args,
Cardinal *num_args)
{
    XmWidgetExtData     extData;
    XmBaseClassExt      *cePtr;
    WidgetClass         ec;
    XmLabelGCacheObject newSec;
    Cardinal            size;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    newSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);
    _XmProcessUnlock();

    newSec->object.self = (Widget)newSec;
    newSec->object.widget_class = ec;
    newSec->object.parent = XtParent(newParent);
    newSec->object.xrm_name = newParent->core.xrm_name;
    newSec->object.being_destroyed = False;
    newSec->object.destroy_callbacks = NULL;
    newSec->object.constraints = NULL;

    newSec->ext.logicalParent = newParent;
    newSec->ext.extensionType = XmCACHE_EXTENSION;

    memcpy(&(newSec->label_cache),
        LabG_Cache(newParent),
        sizeof(XmLabelGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    XtGetSubvalues((XtPointer)newSec,
        ec->core_class.resources,
        ec->core_class.num_resources,
        args, *num_args);

    _XmExtGetValuesHook((Widget)newSec, args, num_args);
}


/************************************************************************
 *
 *  GetValuesPosthook
 *
 ************************************************************************/

static void
GetValuesPosthook(Widget new_w,
ArgList args,
Cardinal *num_args)
{
    XmWidgetExtData ext;

    _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

    _XmProcessLock();
    _XmExtObjFree((XtPointer)ext->widget);
    _XmProcessUnlock();

    XtFree((char *) ext);
}


/************************************************************************
 *
 *  SetValuesPosthook
 *
 ************************************************************************/

static Boolean
SetValuesPosthook(Widget current,
Widget req,
Widget new_w,
ArgList args,
Cardinal *num_args)
{
    XmWidgetExtData ext;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    /* assign if changed! */
    _XmProcessLock();
    if (!_XmLabelCacheCompare((XtPointer)LabG_Cache(new_w),
        (XtPointer)LabG_Cache(current)))

    {
        /* delete the old one */
        _XmCacheDelete((XtPointer) LabG_Cache(current));
        LabG_Cache(new_w) = (XmLabelGCacheObjPart *)
            _XmCachePart(LabG_ClassCachePart(new_w),
            (XtPointer) LabG_Cache(new_w),
            sizeof(XmLabelGCacheObjPart));
    }
    else
        LabG_Cache(new_w) = LabG_Cache(current);

    _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

    _XmExtObjFree((XtPointer)ext->widget);
    _XmExtObjFree((XtPointer)ext->reqWidget);

    XtFree((char *) ext);
    _XmProcessUnlock();

    return FALSE;
}


static void
BorderHighlight(Widget w)
{
    XmLabelGadget lg = (XmLabelGadget) w;

    if (lg->rectangle.width == 0 ||
        lg->rectangle.height == 0 ||
        lg->gadget.highlight_thickness == 0)
    {
        return;
    }

    lg->gadget.highlighted = True;
    lg->gadget.highlight_drawn = True;

    /* CR 7330: Use XmeDrawHighlight, not _XmDrawHighlight. */
    XmeDrawHighlight(XtDisplay((Widget) lg), XtWindow((Widget) lg),
        LabG_HighlightGC(lg),
        lg->rectangle.x, lg->rectangle.y,
        lg->rectangle.width, lg->rectangle.height,
        lg->gadget.highlight_thickness);
}


/************************************************************************
 *
 *  SetNormalGC
 *	Create the normal and insensitive GC's for the gadget.
 *
 ************************************************************************/

static void
SetNormalGC(XmLabelGadget lw)
{
    XGCValues       values;
    XtGCMask        valueMask, dynamicMask;
    XmManagerWidget mw;
    XFontStruct     *fs = (XFontStruct *) NULL;

    mw = (XmManagerWidget) XtParent(lw);

    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    dynamicMask = GCClipXOrigin | GCClipYOrigin | GCClipMask;

    values.foreground = LabG_Foreground(lw);
    values.background = LabG_Background(lw);
    values.graphics_exposures = FALSE;

    if (XmeRenderTableGetDefaultFont(LabG_Font(lw), &fs))
    {
        valueMask |= GCFont;
        values.font = fs->fid;
    }

    LabG_NormalGC(lw) = XtAllocateGC((Widget) mw, 0, valueMask, &values,
        dynamicMask, 0);

    /*generally gray insensitive foreground (instead stipple)*/
    values.foreground = _XmAssignInsensitiveColor((Widget)lw);
    values.background = LabG_Background(lw);
    LabG_InsensitiveGC(lw) = XtAllocateGC((Widget) mw, 0, valueMask, &values,
        dynamicMask, 0);

    /*light shadow for insensitive text (instead stipple)*/
    values.foreground = LabG_TopShadowColor(lw);
    LabG_ShadowGC(lw)  = XtAllocateGC((Widget) mw, 0, valueMask, &values,
	dynamicMask, 0);
}


/************************************************************************
 *
 *  _XmLabelSetBackgroundGC
 *     Get the graphics context used for drawing the shadows.
 *
 ************************************************************************/

void
_XmLabelSetBackgroundGC(XmLabelGadget lw)
{
    XGCValues   values;
    XtGCMask    valueMask;
    XmManagerWidget mw;
    XFontStruct     *fs = (XFontStruct *) NULL;

    mw = (XmManagerWidget) XtParent(lw);

    if (lw->label.fill_bg_box != _XmALWAYS_FILL_BG_BOX)
    {
        if ((mw->core.background_pixel != LabG_Background(lw)) &&
            (mw->core.background_pixmap == XmUNSPECIFIED_PIXMAP))
            lw->label.fill_bg_box = _XmFILL_BG_BOX;
        else
            lw->label.fill_bg_box = _XmPLAIN_BG_BOX;
    }

    valueMask = GCForeground | GCBackground | GCGraphicsExposures | GCClipMask;
    values.foreground = LabG_Background(lw);
    values.background = LabG_Foreground(lw);
    values.graphics_exposures = FALSE;
    values.clip_mask = None;

    /* CR 8980: Use parent's background_pixmap if possible. */
    if (mw->core.background_pixmap != XmUNSPECIFIED_PIXMAP)
    {
        int depth;

        XmeGetPixmapData(XtScreen((Widget)lw), mw->core.background_pixmap,
            NULL, &depth, NULL, NULL, NULL, NULL, NULL, NULL);

        if (depth == 1)
        {
            valueMask |= GCFillStyle | GCStipple ;
            values.fill_style = FillOpaqueStippled;
            values.stipple = mw->core.background_pixmap;
        }
        else
        {
            valueMask |= GCFillStyle | GCTile ;
            values.fill_style = FillTiled;
            values.tile = mw->core.background_pixmap;
        }
    }

    if (XmeRenderTableGetDefaultFont(LabG_Font(lw), &fs))
    {
        valueMask |= GCFont;
        values.font = fs->fid;
    }

    LabG_BackgroundGC(lw) = XtGetGC ((Widget) mw, valueMask, &values);
}


/************************************************************************
 *
 * _XmCalcLabelGDimensions()
 *   Calculates the dimensionsof the label text and pixmap, and updates
 *   the TextRect fields appropriately. Called at Initialize and SetValues.
 *   This is also called by the subclasses to recalculate label dimensions.
 *
 *************************************************************************/

void
_XmCalcLabelGDimensions(Widget wid)
{
    XmLabelGadget newlw = (XmLabelGadget) wid;
    Dimension dw, dh;
    unsigned int w = 0 , h = 0;

    /* initialize TextRect width and height to 0, reset if needed */

    LabG_AccTextRect(newlw).width = 0;
    LabG_AccTextRect(newlw).height = 0;
    LabG_StringRect(newlw).x = 0;
    LabG_StringRect(newlw).y = 0;
    LabG_StringRect(newlw).width = 0;
    LabG_StringRect(newlw).height = 0;
    LabG_PixmapRect(newlw).x = 0;
    LabG_PixmapRect(newlw).y = 0;
    LabG_PixmapRect(newlw).width = 0;
    LabG_PixmapRect(newlw).height = 0;

    if (LabG_IsPixmap(newlw) || LabG_IsPixmapAndText(newlw))
    {
        if (XtIsSensitive(wid))
        {
            if (Pix (newlw) != XmUNSPECIFIED_PIXMAP)
            {
                XmeGetPixmapData(XtScreen(newlw), Pix(newlw),
                    NULL, NULL, NULL, NULL, NULL, NULL,
                    &w, &h);

                LabG_PixmapRect(newlw).width = (unsigned short) w;
                LabG_PixmapRect(newlw).height = (unsigned short) h;
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

                LabG_PixmapRect(newlw).width = (unsigned short) w;
                LabG_PixmapRect(newlw).height = (unsigned short) h;
            }
        }
    }

    if (LabG_IsText (newlw) || LabG_IsPixmapAndText(newlw))
    {
        /* If we have a string then size it.  */
        if (!XmStringEmpty (LabG__label(newlw)))
        {
            XmStringExtent (LabG_Font(newlw), LabG__label(newlw), &dw, &dh);
            LabG_StringRect(newlw).width = (unsigned short)dw;
            LabG_StringRect(newlw).height = (unsigned short)dh;
        }
    }

    _XmLabelGCalcTextRect(wid);

    if (LabG__acceleratorText(newlw) != NULL)
    {
        /* If we have a string then size it. */
        if (!XmStringEmpty (LabG__acceleratorText(newlw)))
        {
            XmStringExtent(LabG_Font(newlw),
                    LabG__acceleratorText(newlw), &dw, &dh);
            LabG_AccTextRect(newlw).width = (unsigned short)dw;
            LabG_AccTextRect(newlw).height = (unsigned short)dh;
        }
    }
}

void
_XmLabelGCalcTextRect(Widget wid)
{
  LabG_TextRect(wid).width = 0;
  LabG_TextRect(wid).height = 0;

  if (LabG_IsPixmap(wid))
    {
      LabG_TextRect(wid).width = LabG_PixmapRect(wid).width;
      LabG_TextRect(wid).height = LabG_PixmapRect(wid).height;
    }
  else if (LabG_IsText(wid))
    {
      LabG_TextRect(wid).width = LabG_StringRect(wid).width;
      LabG_TextRect(wid).height = LabG_StringRect(wid).height;
    }
  else if (LabG_IsPixmapAndText(wid))
    {
      if (LabG_PixmapPlacement(wid) == XmPIXMAP_TOP ||
          LabG_PixmapPlacement(wid) == XmPIXMAP_BOTTOM)
	{
          LabG_TextRect(wid).height = LabG_PixmapRect(wid).height +
          	LabG_StringRect(wid).height + LabG_PixmapTextPadding(wid);
          LabG_TextRect(wid).width =
          	MAX(LabG_StringRect(wid).width, LabG_PixmapRect(wid).width);
	}
      else if (LabG_PixmapPlacement(wid) == XmPIXMAP_LEFT ||
	       LabG_PixmapPlacement(wid) == XmPIXMAP_RIGHT)
        {
          LabG_TextRect(wid).width = LabG_PixmapRect(wid).width +
          	LabG_StringRect(wid).width + LabG_PixmapTextPadding(wid);
          LabG_TextRect(wid).height =
          	MAX(LabG_StringRect(wid).height, LabG_PixmapRect(wid).height);
        }

      if (LabG_PixmapPlacement(wid) == XmPIXMAP_TOP)
        {
	  LabG_PixmapRect(wid).y = 0;
	  LabG_StringRect(wid).y =
          	LabG_PixmapRect(wid).height + LabG_PixmapTextPadding(wid);
        }
      else if (LabG_PixmapPlacement(wid) == XmPIXMAP_BOTTOM)
        {
          LabG_StringRect(wid).y = 0;
          LabG_PixmapRect(wid).y =
          	LabG_StringRect(wid).height + LabG_PixmapTextPadding(wid);
        }
      else if ((LabG_PixmapPlacement(wid) == XmPIXMAP_RIGHT &&
	        LayoutIsRtoLG(wid)) ||
               (LabG_PixmapPlacement(wid) == XmPIXMAP_LEFT &&
                !LayoutIsRtoLG(wid)))
	{
          LabG_PixmapRect(wid).x = 0;
          LabG_StringRect(wid).x =
          	LabG_PixmapRect(wid).width + LabG_PixmapTextPadding(wid);
	}
      else if ((LabG_PixmapPlacement(wid) == XmPIXMAP_LEFT &&
                LayoutIsRtoLG(wid)) ||
               (LabG_PixmapPlacement(wid) == XmPIXMAP_RIGHT &&
                !LayoutIsRtoLG(wid)))
	{
	   LabG_StringRect(wid).x = 0;
	   LabG_PixmapRect(wid).x =
           	LabG_StringRect(wid).width + LabG_PixmapTextPadding(wid);
	}

      if (LabG_PixmapPlacement(wid) == XmPIXMAP_RIGHT ||
	  LabG_PixmapPlacement(wid) == XmPIXMAP_LEFT)
        {
          LabG_PixmapRect(wid).y =
          	(LabG_TextRect(wid).height - LabG_PixmapRect(wid).height) / 2;
          LabG_StringRect(wid).y =
          	(LabG_TextRect(wid).height - LabG_StringRect(wid).height) / 2;
        }
      else
        {
          if (LabG_Alignment(wid) == XmALIGNMENT_CENTER)
            {
              LabG_PixmapRect(wid).x =
	        (LabG_TextRect(wid).width - LabG_PixmapRect(wid).width) / 2;
              LabG_StringRect(wid).x =
              	(LabG_TextRect(wid).width - LabG_StringRect(wid).width) / 2;
            }
          else if ((LabG_Alignment(wid) == XmALIGNMENT_END &&
                     !LayoutIsRtoLG(wid)) ||
                    (LabG_Alignment(wid) == XmALIGNMENT_BEGINNING &&
                     LayoutIsRtoLG(wid)))
            {
              LabG_PixmapRect(wid).x =
              	LabG_TextRect(wid).width - LabG_PixmapRect(wid).width;
              LabG_StringRect(wid).x =
              	LabG_TextRect(wid).width - LabG_StringRect(wid).width;
            }
        }
    }
}


/************************************************************************
 *
 *  Resize
 *	Sets new width, new height, and new label.TextRect
 *	appropriately. It is called by Initialize and SetValues.
 *
 ************************************************************************/

static void
Resize(Widget wid)
{
    XmLabelGadget newlw = (XmLabelGadget) wid;
    int leftx, rightx;

    /* increase margin width if necessary to accomodate accelerator text */
    if (LabG__acceleratorText(newlw) != NULL)
    {
        if (LayoutIsRtoLG(newlw))
        {
            if (LabG_MarginLeft(newlw) <
                LabG_AccTextRect(newlw).width + LABELG_ACC_PAD)
            {
                int delta = LabG_AccTextRect(newlw).width + LABELG_ACC_PAD -
                    LabG_MarginLeft(newlw);
                newlw->label.acc_left_delta += delta;
                LabG_MarginLeft(newlw) += delta;
            }
        }
        else
        {
            if (LabG_MarginRight(newlw) <
                LabG_AccTextRect(newlw).width + LABELG_ACC_PAD)
            {
                int delta = LabG_AccTextRect(newlw).width + LABELG_ACC_PAD -
                    LabG_MarginRight(newlw);
                newlw->label.acc_right_delta += delta;
                LabG_MarginRight(newlw) += delta;
            }
        }
    }
    /* Has a width been specified?  */
    if (newlw->rectangle.width == 0)
        newlw->rectangle.width =
            LabG_TextRect(newlw).width +
            LabG_MarginLeft(newlw) + LabG_MarginRight(newlw) +
            (2 * (LabG_MarginWidth(newlw) +
            newlw->gadget.highlight_thickness
            + newlw->gadget.shadow_thickness));

    leftx = newlw->gadget.highlight_thickness +
        newlw->gadget.shadow_thickness + LabG_MarginWidth(newlw) +
        LabG_MarginLeft(newlw);

    rightx = newlw->rectangle.width - newlw->gadget.highlight_thickness -
        newlw->gadget.shadow_thickness - LabG_MarginWidth(newlw) -
        LabG_MarginRight(newlw);

    switch (LabG_Alignment(newlw))
    {
        case XmALIGNMENT_BEGINNING:
            if (LayoutIsRtoLG(newlw))
                LabG_TextRect(newlw).x = rightx - LabG_TextRect(newlw).width;
            else
                LabG_TextRect(newlw).x = leftx;
            break;

        case XmALIGNMENT_END:
            if (LayoutIsRtoLG(newlw))
                LabG_TextRect(newlw).x = leftx;
            else
                LabG_TextRect(newlw).x = rightx - LabG_TextRect(newlw).width;
            break;

        default:
            /* XmALIGNMENT_CENTER */
            LabG_TextRect(newlw).x = leftx +
                (rightx - leftx - (int)LabG_TextRect(newlw).width) / 2;
            break;
    }

    /*  Has a height been specified?  */
    if (newlw->rectangle.height == 0)
        newlw->rectangle.height = MAX(LabG_TextRect(newlw).height,
            LabG_AccTextRect(newlw).height)
            + LabG_MarginTop(newlw)
            + LabG_MarginBottom(newlw)
            + (2 * (LabG_MarginHeight(newlw)
            + newlw->gadget.highlight_thickness
            + newlw->gadget.shadow_thickness));

    LabG_TextRect(newlw).y =
        (short) (newlw->gadget.highlight_thickness
        + newlw->gadget.shadow_thickness
        + LabG_MarginHeight(newlw) + LabG_MarginTop(newlw) +
        ((int) (newlw->rectangle.height - LabG_MarginTop(newlw)
        - LabG_MarginBottom(newlw)
        - (2 * (LabG_MarginHeight(newlw)
        + newlw->gadget.highlight_thickness
        + newlw->gadget.shadow_thickness))
        - LabG_TextRect(newlw).height) / 2));

    if (LabG__acceleratorText(newlw) != NULL)
    {
        Dimension  base_label, base_accText, diff;

        if (LayoutIsRtoLG(newlw))
            LabG_AccTextRect(newlw).x =  newlw->rectangle .x +
                (newlw->gadget.highlight_thickness +
                newlw->gadget.shadow_thickness +
                LabG_MarginWidth(newlw));
        else
            LabG_AccTextRect(newlw).x = (newlw->rectangle.width -
                newlw->gadget.highlight_thickness -
                newlw->gadget.shadow_thickness -
                LabG_MarginWidth(newlw) -
                LabG_MarginRight(newlw) +
                LABELG_ACC_PAD);

        LabG_AccTextRect(newlw).y = newlw->gadget.highlight_thickness
            + newlw->gadget.shadow_thickness
            + LabG_MarginHeight(newlw) + LabG_MarginTop(newlw) +
            ((int) (newlw->rectangle.height - LabG_MarginTop(newlw)
            - LabG_MarginBottom(newlw)
            - (2 * (LabG_MarginHeight(newlw)
            + newlw->gadget.highlight_thickness
            + newlw->gadget.shadow_thickness))
            - LabG_AccTextRect(newlw).height) / 2);

        /* make sure the label and accelerator text line up */
        /* when the fonts are different */

        if (LabG_IsText (newlw))
        {
            base_label =
                XmStringBaseline (LabG_Font(newlw), LabG__label(newlw));
            base_accText =
                XmStringBaseline (LabG_Font(newlw),
                LabG__acceleratorText(newlw));

            if (base_label > base_accText)
            {
                diff = base_label - base_accText;
                LabG_AccTextRect(newlw).y = LabG_TextRect(newlw).y + diff - 1;
            }
            else if (base_label < base_accText)
            {
                diff = base_accText - base_label;
                LabG_TextRect(newlw).y = LabG_AccTextRect(newlw).y + diff - 1;
            }
        }
    }

    /* Set core dimensions so we don't get a Toolkit error. */
    if (newlw->rectangle.width == 0)
        newlw->rectangle.width = 1;
    if (newlw->rectangle.height == 0)
        newlw->rectangle.height = 1;
}


/************************************************************************
 *
 *  Initialize
 *	This is the widget's instance initialize routine.  It is
 *	called once for each widget.
 *  Changes: Treat label, pixmap, labeltype, mnemoniccharset as independently
 *	setable resource.
 ************************************************************************/

static void
Initialize(Widget req,
Widget new_w,
ArgList args,
Cardinal *num_args)
{
    XmMenuSystemTrait menuSTrait;

    XmLabelGadget lw = (XmLabelGadget) new_w;
    XmLabelGadget rw = (XmLabelGadget) req;

    lw->label.baselines = NULL;

    /* Before doing anything with the pixmap, check if we have to
     * re-ask for a conversion */
    if (Pix  (new_w) == XmDELAYED_PIXMAP)
    {
        /* This test means that a conversion was asked for
         * but failed because the colors were not accessible
         * prior to Initialize, because the cache wasn't there yet.
         * We have to try again from here. */

        XtGetSubresources(new_w, new_w,
            NULL, NULL,
            label_pixmap_resource, 1,
            args, *num_args);
    }

    if (Pix_insen (new_w) == XmDELAYED_PIXMAP)
    {
        XtGetSubresources(new_w, new_w,
            NULL, NULL,
            label_pixmap_insen_resource, 1,
            args, *num_args);
    }

    /* If menuProcs is not set up yet, try again */
    _XmProcessLock();
    if (xmLabelGadgetClassRec.label_class.menuProcs == NULL)
        xmLabelGadgetClassRec.label_class.menuProcs =
            (XmMenuProc) _XmGetMenuProcContext();
    _XmProcessUnlock();

    LabG_SkipCallback(new_w) = FALSE;

    /* Check for Invalid enumerated types */

    if (!XmRepTypeValidValue(XmRID_LABEL_TYPE, LabG_LabelType(new_w), new_w))
        LabG_LabelType(new_w) = XmSTRING;

    if (!XmRepTypeValidValue(XmRID_ALIGNMENT, LabG_Alignment(new_w), new_w))
        LabG_Alignment(new_w) = XmALIGNMENT_CENTER;

    if (!XmRepTypeValidValue(XmRID_PIXMAP_PLACEMENT, LabG_PixmapPlacement(new_w), new_w))
        LabG_PixmapPlacement(new_w) = XmPIXMAP_LEFT;

    #ifndef NO_XM_1_2_BC
    /*
     * Some pre-Motif 2.0 XmManager subclasses may be bypassing the
     * synthetic resouce GetValues hook and passing us the manager's raw
     * string_direction field (which is now a layout_direction).  Fixup
     * the common/simple cases.
     */
    switch (LabG_StringDirection(lw))
    {
        case XmLEFT_TO_RIGHT:
        case XmRIGHT_TO_LEFT:
            /* These string directions are erroneous uses of layout directions. */
            LabG_StringDirection(lw) =
                XmDirectionToStringDirection(LabG_StringDirection(lw));
            break;
        default:
            /* Other unknown layout directions will still get a warning. */
            break;
    }
    #endif

    /* If layout_direction is set, it overrides string_direction.
     * If string_direction is set, but not layout_direction, use
     *	string_direction value.
     * If neither is set, get from parent.
     */
    if (lw->gadget.layout_direction != XmDEFAULT_DIRECTION)
    {
        if (LabG_StringDirection(lw) == XmDEFAULT_DIRECTION)
            LabG_StringDirection(lw) =
                XmDirectionToStringDirection(lw->gadget.layout_direction);
    }
    else if (LabG_StringDirection(lw) != XmDEFAULT_DIRECTION)
    {
        lw->gadget.layout_direction =
            XmStringDirectionToDirection(LabG_StringDirection(lw));
    }
    else
    {
        lw->gadget.layout_direction = _XmGetLayoutDirection(XtParent(new_w));
        LabG_StringDirection(lw) =
            XmDirectionToStringDirection(lw->gadget.layout_direction);
    }

    if (!XmRepTypeValidValue(XmRID_STRING_DIRECTION,
        LabG_StringDirection(new_w), new_w))
        LabG_StringDirection(new_w) = XmSTRING_DIRECTION_L_TO_R;

    /* Make a local copy of the font list */
    if (LabG_Font(new_w) == NULL)
    {
        /* CR 2990:  Let subclasses choose their own default font. */
        LabG_Font(new_w) = XmeGetDefaultRenderTable (new_w, XmLABEL_FONTLIST);
    }
    LabG_Font(new_w) = XmFontListCopy(LabG_Font(new_w));

    menuSTrait = (XmMenuSystemTrait)
        XmeTraitGet((XtPointer) XtClass((Widget) XtParent(new_w)), XmQTmenuSystem);

    if (menuSTrait != (XmMenuSystemTrait) NULL)
        LabG_MenuType(new_w) = menuSTrait->type(XtParent(new_w));
    else
        LabG_MenuType(new_w) = XmWORK_AREA;

    /*  Handle the label string :
     *   If no label string is given accept widget's name as default.
     *     convert the widgets name to an XmString before storing;
     *   else
     *     save a copy of the given string.
     *     If the given string is not an XmString issue an warning.
     */

    if (LabG__label(new_w) == NULL)
    {
                                                  /* reserved */
        LabG__label(new_w) = XmeGetLocalizedString ((char *) NULL,
            (Widget) lw,
            XmNlabelString,
            XrmQuarkToString(lw->object.xrm_name));
    }
    else if (XmeStringIsValid((XmString) LabG__label(new_w)))
    {
        LabG__label(new_w) = XmStringCopy((XmString) LabG__label(new_w));
    }
    else
    {
        XmeWarning((Widget) lw, CS_STRING_MESSAGE);
        LabG__label(new_w) =
            XmStringCreateLocalized(XrmQuarkToString(lw->object.xrm_name));
    }

    /*
     * Convert the given mnemonicCharset to the internal Xm-form.
     */
    if (LabG_MnemonicCharset(new_w) != NULL)
    {
        LabG_MnemonicCharset (new_w) =
            _XmStringCharsetCreate (LabG_MnemonicCharset (new_w));
    }
    else
        LabG_MnemonicCharset (new_w) =
            _XmStringCharsetCreate (XmFONTLIST_DEFAULT_TAG);

    /* Accelerators are currently only supported in menus */
    if ((LabG__acceleratorText(new_w) != NULL) && LabG_IsMenupane(new_w))
    {
        if (XmeStringIsValid((XmString) LabG__acceleratorText(new_w)))
        {
            /* Copy the input string into local space if not a Cascade Button. */
            if (XmIsCascadeButtonGadget(new_w))
                LabG__acceleratorText(new_w) = NULL;
            else
                LabG__acceleratorText(new_w)=
                    XmStringCopy((XmString) LabG__acceleratorText(new_w));
        }
        else
        {
            XmeWarning((Widget) lw, ACC_MESSAGE);
            LabG__acceleratorText(new_w) = NULL;
        }
    }
    else
        LabG__acceleratorText(new_w) = NULL;

    if ((LabG_Accelerator(new_w) != NULL) && LabG_IsMenupane(new_w))
    {
        /* Copy the accelerator into local space */
        LabG_Accelerator(lw) = XtNewString(LabG_Accelerator(new_w));
    }
    else
        LabG_Accelerator(lw) = NULL;

    LabG_SkipCallback(lw) = FALSE;

    lw->label.acc_left_delta = 0;
    lw->label.acc_right_delta = 0;

    /*  If zero width and height was requested by the application,  */
    /*  reset new's width and height to zero to allow Resize()     */
    /*  to operate properly.                                        */

    if (rw->rectangle.width == 0)
        lw->rectangle.width = 0;

    if (rw->rectangle.height == 0)
        lw->rectangle.height = 0;

    /* CR 6267:  Suppress highlight thickness before sizing also. */
    if ((LabG_MenuType(new_w) == XmMENU_POPUP) ||
        (LabG_MenuType(new_w) == XmMENU_PULLDOWN) ||
        (LabG_MenuType(new_w) == XmMENU_BAR))
        lw->gadget.highlight_thickness = 0;

    _XmCalcLabelGDimensions(new_w);

    /* CR 7283: Can't use the resize method pointer here because */
    /* 	subclasses haven't been initialized.		       */
    Resize((Widget) lw);

    DealWithColors(lw);
    DealWithPixmaps(lw);

    /* Initialize only; set properly in _XmLabelSetBackgroundGC(). */
    lw->label.fill_bg_box = _XmPLAIN_BG_BOX;

    SetNormalGC(lw);
    _XmLabelSetBackgroundGC (lw);
    LabG_HighlightGC(lw) =
        _XmGetPixmapBasedGC (XtParent(new_w),
        LabG_HighlightColor(lw),
        LabG_Background(lw),
        LabG_HighlightPixmap(lw));
    LabG_TopShadowGC(lw) =
        _XmGetPixmapBasedGC (XtParent(new_w),
        LabG_TopShadowColor(lw),
        LabG_Background(lw),
        LabG_TopShadowPixmap(lw));
    LabG_BottomShadowGC(lw) =
        _XmGetPixmapBasedGC (XtParent(new_w),
        LabG_BottomShadowColor(lw),
        LabG_Background(lw),
        LabG_BottomShadowPixmap(lw));

    /*  Force the label traversal flag when in a menu  */
    if ((XtClass(lw) == xmLabelGadgetClass) &&
        ((LabG_MenuType(new_w) == XmMENU_POPUP) ||
        (LabG_MenuType(new_w) == XmMENU_PULLDOWN) ||
        (LabG_MenuType(new_w) == XmMENU_OPTION)))
    {
        lw->gadget.traversal_on = False;
        lw->gadget.highlight_on_enter = False;
    }

    /*  Initialize the interesting input types.  */
    lw->gadget.event_mask = XmHELP_EVENT | XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT
        | XmENTER_EVENT | XmLEAVE_EVENT | XmBDRAG_EVENT;
}


/*
 * DealWithColors
 *	Deal with compatibility.
 */

static void
DealWithColors(XmLabelGadget lw)
{
    XmManagerWidget mw = (XmManagerWidget) XtParent(lw);

    /*
     * If the gadget color is set to the tag value or it is the
     * same as the manager color; bc mode is enabled otherwise
     * initialize like a widget.
     */
    if ((LabG_Background(lw) == INVALID_PIXEL ||
        LabG_Background(lw) == mw->core.background_pixel) &&
        (LabG_Foreground(lw) == INVALID_PIXEL ||
        LabG_Foreground(lw) == mw->manager.foreground) &&
        (LabG_TopShadowColor(lw) == INVALID_PIXEL ||
        LabG_TopShadowColor(lw) == mw->manager.top_shadow_color) &&
        (LabG_BottomShadowColor(lw) == INVALID_PIXEL ||
        LabG_BottomShadowColor(lw) == mw->manager.bottom_shadow_color) &&
        (LabG_HighlightColor(lw) == INVALID_PIXEL ||
        LabG_HighlightColor(lw) == mw->manager.highlight_color))
    {
        LabG_Background(lw) = mw->core.background_pixel;
        LabG_Foreground(lw) = mw->manager.foreground;
        LabG_TopShadowColor(lw) = mw->manager.top_shadow_color;
        LabG_BottomShadowColor(lw) = mw->manager.bottom_shadow_color;
        LabG_HighlightColor(lw) = mw->manager.highlight_color;
    }
    else
    {
        InitNewColorBehavior(lw);
    }
}


/*
 * InitNewColorBehavior
 *	Initialize colors like a widget.  These are CallProcs so
 * they should be called with a correct offset. However offset
 * isn't used by these functions.  Even so I supply offset.
 * You make the call.
 */

static void
InitNewColorBehavior(XmLabelGadget lw)
{
    XrmValue value;

    value.size = sizeof(Pixel);

    if (LabG_Background(lw) == INVALID_PIXEL)
    {
        _XmBackgroundColorDefault
            ((Widget) lw,
            XtOffsetOf(struct _XmLabelGCacheObjRec, label_cache.background),
            &value);
        memcpy((char*) &LabG_Background(lw), value.addr, value.size);
    }

    if (LabG_Foreground(lw) == INVALID_PIXEL)
    {
        _XmForegroundColorDefault
            ((Widget) lw,
            XtOffsetOf(struct _XmLabelGCacheObjRec, label_cache.foreground),
            &value);
        memcpy((char*) &LabG_Foreground(lw), value.addr, value.size);
    }

    if (LabG_TopShadowColor(lw) == INVALID_PIXEL)
    {
        _XmTopShadowColorDefault
            ((Widget) lw,
            XtOffsetOf(struct _XmLabelGCacheObjRec, label_cache.top_shadow_color),
            &value);
        memcpy((char*) &LabG_TopShadowColor(lw), value.addr, value.size);
    }

    if (LabG_BottomShadowColor(lw) == INVALID_PIXEL)
    {
        _XmBottomShadowColorDefault
            ((Widget) lw,
            XtOffsetOf(struct _XmLabelGCacheObjRec,
            label_cache.bottom_shadow_color),
            &value);
        memcpy((char*) &LabG_BottomShadowColor(lw), value.addr, value.size);
    }

    if (LabG_HighlightColor(lw) == INVALID_PIXEL)
    {
        _XmHighlightColorDefault
            ((Widget) lw,
            XtOffsetOf(struct _XmLabelGCacheObjRec, label_cache.highlight_color),
            &value);
        memcpy((char*) &LabG_HighlightColor(lw), value.addr, value.size);
    }
}


/*
 * DealWithPixmaps
 *	Deal with compatibility.  If any resource is set initialize
 * like a widget otherwise get everything from the parent.
 */

static void
DealWithPixmaps(XmLabelGadget lw)
{
    XmManagerWidget mw = (XmManagerWidget) XtParent(lw);

    if ((LabG_TopShadowPixmap(lw) == INVALID_PIXMAP ||
        LabG_TopShadowPixmap(lw) == mw->manager.top_shadow_pixmap) &&
        (LabG_HighlightPixmap(lw) == INVALID_PIXMAP ||
        LabG_HighlightPixmap(lw) == mw->manager.highlight_pixmap))
    {
        LabG_TopShadowPixmap(lw) = mw->manager.top_shadow_pixmap;
        LabG_HighlightPixmap(lw) = mw->manager.highlight_pixmap;
    }
    else
    {
        InitNewPixmapBehavior(lw);
    }
}


/*
 * InitNewPixmapBehavior
 *	Initialize colors like a widget.
 */

static void
InitNewPixmapBehavior(XmLabelGadget lw)
{
    if (LabG_TopShadowPixmap(lw) == INVALID_PIXMAP)
        LabG_TopShadowColor(lw) = GetTopShadowPixmapDefault((Widget)lw);

    if (LabG_HighlightPixmap(lw) == INVALID_PIXMAP)
        LabG_HighlightPixmap(lw) = GetLabelHighlightPixmapDefault((Widget)lw);
}


/************************************************************************
 *
 *  QueryGeometry
 *
 ************************************************************************/

static XtGeometryResult
QueryGeometry(Widget wid,
XtWidgetGeometry *intended,
XtWidgetGeometry *reply)
{
    XmLabelGadget lg = (XmLabelGadget) wid;
    reply->request_mode = 0;

    /* Don't really know what to do with queries about x,y,border,stacking.
     * Since we are interpreting unset bits as a request for information
     * (asking about neither height or width does the old 0-0 request)
     * a caller asking about x,y should not get back width and height,
     * especially since it is an expensive operation.  So x, y, border, stack
     * all return No, this indicates we'd prefer to remain as is.  Parent
     * is free to change it anyway...
     */

    if (GMode(intended) & ~(CWWidth | CWHeight))
        return XtGeometryNo;

    if (LabG_RecomputeSize(lg) == FALSE)
        return XtGeometryNo;

    /* pre-load the reply with input values */

    reply->request_mode = (CWWidth | CWHeight);

    reply->width = LabG_TextRect(lg).width +
        (2 * (LabG_MarginWidth(lg) +
        lg->gadget.highlight_thickness +
        lg->gadget.shadow_thickness)) +
        LabG_MarginLeft(lg) +
        LabG_MarginRight(lg);

    if (reply->width == 0)
        reply->width = 1;

    reply->height = MAX(LabG_TextRect(lg).height,
        LabG_AccTextRect(lg).height) +
        (2 * (LabG_MarginHeight(lg) +
        lg->gadget.highlight_thickness +
        lg->gadget.shadow_thickness)) +
        LabG_MarginTop(lg) +
        LabG_MarginBottom(lg);

    if (reply->height == 0)
        reply->height = 1;

    if ((IsWidth(intended) && (reply->width != intended->width)) ||
        (IsHeight(intended) && (reply->height != intended->height)) ||
        (GMode(intended) != GMode(reply)))
    {
        return XtGeometryAlmost;
    }
    else
    {
        reply->request_mode = 0;
        return XtGeometryYes;
    }
}


/************************************************************************
 *
 *  Destroy
 *	Free up the label gadget allocated space.  This includes
 *	the label, and GC's.
 *
 ************************************************************************/

static void
Destroy(Widget w)
{
    if (LabG__label(w) != NULL)
        XmStringFree (LabG__label(w));
    if (LabG__acceleratorText(w) != NULL)
        XmStringFree (LabG__acceleratorText(w));
    XtFree (LabG_Accelerator(w));
    if (LabG_Font(w)  != NULL)
        XmFontListFree (LabG_Font(w));
    XtFree (LabG_MnemonicCharset (w));

    XtFree ((char*) ((XmLabelGadget)w)->label.baselines);

    XtReleaseGC (XtParent(w), LabG_NormalGC(w));
    XtReleaseGC (XtParent(w), LabG_InsensitiveGC(w));
	XtReleaseGC (XtParent(w), LabG_ShadowGC(w));
    XtReleaseGC (XtParent(w), LabG_BackgroundGC(w));
    XtReleaseGC (XtParent(w), LabG_HighlightGC(w));
    XtReleaseGC (XtParent(w), LabG_TopShadowGC(w));
    XtReleaseGC (XtParent(w), LabG_BottomShadowGC(w));

    /* CR 6571: Free cache *after* making all references. */
    _XmProcessLock();
    _XmCacheDelete((XtPointer) LabG_Cache(w));
    _XmProcessUnlock();
}


static void
LabelDrawBackground(Widget wid,
XEvent *event,                                    /* unused */
Region region,                                    /* unused */
LRectangle *background_box)
{
    XmLabelGadget lw = (XmLabelGadget) wid;

    switch (lw->label.fill_bg_box)
    {
        case _XmPLAIN_BG_BOX:
            return;

        case _XmFILL_BG_BOX:
        case _XmALWAYS_FILL_BG_BOX:
        default:
            break;
    }

    /*
     * Background_box is a parameter because subclasses like
     * PushBG and ToggleBG need to be able to adjust this rectangle.
     */

    if (background_box->width < 0)
        background_box->width = 0;

    if (background_box->height < 0)
        background_box->height = 0;

    XFillRectangle(XtDisplay(lw),
        XtWindow((Widget) lw),
        LabG_BackgroundGC(lw),
        background_box->x,
        background_box->y,
        background_box->width,
        background_box->height);
}


/************************************************************************
 *
 *  Redisplay
 *
 ************************************************************************/

static void
Redisplay(Widget wid,
XEvent *event,
Region region)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    LRectangle background_box;

    background_box.x = lw->rectangle.x + LabG_Highlight(lw);
    background_box.y = lw->rectangle.y + LabG_Highlight(lw);
    background_box.width = lw->rectangle.width - (2 * LabG_Highlight(lw));
    background_box.height = lw->rectangle.height - (2 * LabG_Highlight(lw));

    _XmRedisplayLabG(wid, event, region, &background_box);
}

void
_XmRedisplayLabG(Widget wid,
XEvent *event,
Region region,
LRectangle *background_box)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    GC gc;
    GC clipgc = NULL;
    XRectangle clip_rect;
    Dimension availW, availH, marginal_width, marginal_height, max_text_height;

    if (!XtIsRealized(wid)) return ;

    if (LabG_IsMenupane(lw))
    {
        ShellWidget mshell = (ShellWidget) XtParent(XtParent(lw));
        if (! mshell->shell.popped_up)
            return;
    }

    availH = lw->rectangle.height;
    availW = lw->rectangle.width;

    /*
     * Don't count MarginWidth to be consistent with Label Widget.
     *
     * Adjust definitions of temporary variables
     */
    marginal_width = LabG_MarginLeft(lw) + LabG_MarginRight(lw) +
        (2 * (lw->gadget.highlight_thickness + lw->gadget.shadow_thickness));

    marginal_height = LabG_MarginTop(lw) + LabG_MarginBottom(lw) +
        (2 * (lw->gadget.highlight_thickness + lw->gadget.shadow_thickness));

    max_text_height = MAX(LabG_TextRect(lw).height, LabG_AccTextRect(lw).height);

    if (XtIsSensitive(wid))
        clipgc = LabG_NormalGC(lw);
    else
        clipgc = LabG_InsensitiveGC(lw);

    /* Clip should include critical margins (see Label.c) */
    if (availH < (marginal_height + max_text_height) ||
        availW < (marginal_width + LabG_TextRect(lw).width))
    {
        clip_rect.x = lw->rectangle.x + lw->gadget.highlight_thickness +
            lw->gadget.shadow_thickness + LabG_MarginLeft(lw);
        clip_rect.y = lw->rectangle.y + lw->gadget.highlight_thickness +
            lw->gadget.shadow_thickness + LabG_MarginTop(lw);

        /* Don't allow negative dimensions */
        if (availW > marginal_width)
            clip_rect.width = availW - marginal_width;
        else
            clip_rect.width = 0;

        if (availH > marginal_height)
            clip_rect.height = availH - marginal_height;
        else
            clip_rect.height = 0;

        XSetClipRectangles(XtDisplay(lw), clipgc, 0,0, &clip_rect, 1, Unsorted);
#if USE_XFT
        _XmXftSetClipRectangles(XtDisplay(lw), XtWindow(lw), 0, 0, &clip_rect, 1);
#endif
    } else
    {
    XSetClipMask (XtDisplay (lw), clipgc, None);
#if USE_XFT
	XftDraw	*draw = _XmXftDrawCreate(XtDisplay(lw), XtWindow(lw));
	XftDrawSetClip(draw, NULL);
#endif
    }

#if USE_XFT
    {
    int width, height;

    int x = lw->rectangle.x + LabG_TextRect(lw).x + LabG_StringRect(lw).x;
    int y = lw->rectangle.y + LabG_TextRect(lw).y + LabG_StringRect(lw).y;

    if (LabG_StringRect(lw).width < availW - marginal_width)
    	width = LabG_StringRect(lw).width;
    else
    	width = availW - marginal_width;

    if (LabG_StringRect(lw).height < availH - marginal_height)
    	height = LabG_StringRect(lw).height;
    else
    	height = availH - marginal_height;

    XFillRectangle(XtDisplay(lw), XtWindow(lw), LabG_BackgroundGC(lw),
		x, y, width, height);
    }
#endif

    /*  Draw the pixmap or text  */
    LabelDrawBackground((Widget)lw, event, region, background_box);

    if (LabG_IsPixmap(lw) || LabG_IsPixmapAndText(lw))
    {
        int depth;

        /* moved before the condition: LabelDrawBackground((Widget)lw, event, region, background_box); */
        if (XtIsSensitive(wid))
        {
            if (Pix (lw) != XmUNSPECIFIED_PIXMAP)
            {
                gc = LabG_NormalGC(lw);

                XmeGetPixmapData(XtScreen(lw), Pix(lw), NULL, &depth,
                    NULL, NULL, NULL, NULL, NULL, NULL);

                if (depth == XtParent(lw)->core.depth)
                    XCopyArea (XtDisplay(lw), Pix(lw), XtWindow(lw), gc, 0, 0,
                        LabG_PixmapRect(lw).width, LabG_PixmapRect(lw).height,
                        lw->rectangle.x + LabG_TextRect(lw).x +
				LabG_PixmapRect(lw).x,
                        lw->rectangle.y + LabG_TextRect(lw).y +
				LabG_PixmapRect(lw).y);
                else if (depth == 1)
                    XCopyPlane (XtDisplay(lw), Pix(lw), XtWindow(lw), gc, 0, 0,
                            LabG_PixmapRect(lw).width,
			    LabG_PixmapRect(lw).height,
                            lw->rectangle.x + LabG_TextRect(lw).x +
				LabG_PixmapRect(lw).x,
                            lw->rectangle.y + LabG_TextRect(lw).y +
				LabG_PixmapRect(lw).y,
			    1);
            }
        }
        else
        {
            Pixmap pix_use = Pix_insen (lw) ;

            if (pix_use == XmUNSPECIFIED_PIXMAP)
		Pix_insen(lw) = pix_use = _XmConvertToBW(wid, Pix(lw));
            if (pix_use != XmUNSPECIFIED_PIXMAP)
            {
                gc = LabG_InsensitiveGC(lw);

                XmeGetPixmapData(XtScreen(lw), pix_use, NULL, &depth,
                    NULL, NULL, NULL, NULL, NULL, NULL);

                if (depth == XtParent(lw)->core.depth)
                    XCopyArea (XtDisplay(lw), pix_use, XtWindow(lw), gc, 0, 0,
                        LabG_PixmapRect(lw).width, LabG_PixmapRect(lw).height,
                        lw->rectangle.x + LabG_TextRect(lw).x +
				LabG_PixmapRect(lw).x,
                        lw->rectangle.y + LabG_TextRect(lw).y +
				LabG_PixmapRect(lw).y);
                else if (depth == 1)
                    XCopyPlane (XtDisplay(lw), pix_use, XtWindow(lw), gc, 0, 0,
                            LabG_PixmapRect(lw).width,
			    LabG_PixmapRect(lw).height,
                            lw->rectangle.x + LabG_TextRect(lw).x + LabG_PixmapRect(lw).x,
                            lw->rectangle.y + LabG_TextRect(lw).y + LabG_PixmapRect(lw).y,
			    1);

                if (pix_use == Pix(lw)) {
                    XSetFillStyle(XtDisplay(lw), gc, FillStippled);
                    XSetStipple(XtDisplay(lw), gc, _XmGetInsensitiveStippleBitmap((Widget)lw));
                    XFillRectangle(XtDisplay(lw), XtWindow(lw), gc,
                       lw->rectangle.x + LabG_TextRect(lw).x +
                       LabG_PixmapRect(lw).x,
                       lw->rectangle.y + LabG_TextRect(lw).y +
                       LabG_PixmapRect(lw).y,
                       LabG_PixmapRect(lw).width,
                       LabG_PixmapRect(lw).height);
                    XSetFillStyle(XtDisplay(lw), gc, FillSolid);
                }
            }
        }
    }

    if ((LabG_IsText(lw) || LabG_IsPixmapAndText(lw)) && (LabG__label(lw) != NULL))
    {
        /* TODO this clears the pixmap, but this is needed to draw background */
        /* LabelDrawBackground((Widget)lw, event, region, background_box); */

        if (LabG_Mnemonic(lw) != XK_VoidSymbol)
        {
            /* CR 5181: Convert the mnemonic keysym to a character string. */
            char tmp[MB_LEN_MAX * 2];
            XmString underline;

            tmp[_XmOSKeySymToCharacter(LabG_Mnemonic(lw), NULL, tmp)] = '\0';
            underline = XmStringCreate(tmp, LabG_MnemonicCharset(lw));
			if (XtIsSensitive(wid) )
			{
				/*Draw normal text*/
				XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_NormalGC(lw),
						lw->rectangle.x + LabG_TextRect(lw).x + LabG_StringRect(lw).x,
						lw->rectangle.y + LabG_TextRect(lw).y + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width, LabG_Alignment(lw),
						LayoutG(lw), NULL, underline);
			}
			else
			{
				/*Draw shadow for insensitive text*/
				XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_ShadowGC(lw),
						lw->rectangle.x+1 + LabG_TextRect(lw).x + LabG_StringRect(lw).x,
						lw->rectangle.y+1 + LabG_TextRect(lw).y + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width, LabG_Alignment(lw),
						LayoutG(lw), NULL, underline);
				/*Draw insensitive text*/
				XmStringDrawUnderline(XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_InsensitiveGC(lw),
						lw->rectangle.x + LabG_TextRect(lw).x + LabG_StringRect(lw).x,
						lw->rectangle.y + LabG_TextRect(lw).y + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width, LabG_Alignment(lw),
						LayoutG(lw), NULL, underline);
			}
            XmStringFree(underline);
        }
        else
		{
			if (XtIsSensitive(wid) )
			{
				/*Draw normal text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_NormalGC(lw),
						lw->rectangle.x + LabG_TextRect(lw).x + LabG_StringRect(lw).x,
						lw->rectangle.y + LabG_TextRect(lw).y + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width,
						LabG_Alignment(lw), LayoutG(lw), NULL);
			}
			else
			{
				/*Draw shadow for insensitive text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_ShadowGC(lw),
						lw->rectangle.x + LabG_TextRect(lw).x+1 + LabG_StringRect(lw).x,
						lw->rectangle.y + LabG_TextRect(lw).y+1 + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width,
						LabG_Alignment(lw), LayoutG(lw), NULL);
				/*Draw insensitive text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__label(lw),
						LabG_InsensitiveGC(lw),
						lw->rectangle.x + LabG_TextRect(lw).x + LabG_StringRect(lw).x,
						lw->rectangle.y + LabG_TextRect(lw).y + LabG_StringRect(lw).y,
						LabG_StringRect(lw).width,
						LabG_Alignment(lw), LayoutG(lw), NULL);
			}
		}
    }

    if (LabG__acceleratorText(lw) != NULL)
    {
        /* Since accelerator text is drawn by moving in from the right,
         * it is possible to overwrite label text when there is clipping,
         * Therefore draw accelerator text only if there is enough
         * room for everything */

        if ((lw->rectangle.width) >=
            (2 * (lw->gadget.highlight_thickness +
            lw->gadget.shadow_thickness +
            LabG_MarginWidth(lw)) +
            LabG_MarginLeft(lw) + LabG_TextRect(lw).width +
            LabG_MarginRight(lw)))
        {
			if (XtIsSensitive(wid) )
			{
				/*Draw normal text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__acceleratorText(lw),
						LabG_NormalGC(lw),
						lw->rectangle.x + LabG_AccTextRect(lw).x,
						lw->rectangle.y + LabG_AccTextRect(lw).y,
						LabG_AccTextRect(lw).width, XmALIGNMENT_END,
						LayoutG(lw), NULL);
			}
			else
			{
				/*Draw shadow for insensitive text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__acceleratorText(lw),
						LabG_ShadowGC(lw) ,
						lw->rectangle.x + LabG_AccTextRect(lw).x+1,
						lw->rectangle.y + LabG_AccTextRect(lw).y+1,
						LabG_AccTextRect(lw).width, XmALIGNMENT_END,
						LayoutG(lw), NULL);
				/*Draw insensitive text*/
				XmStringDraw (XtDisplay(lw), XtWindow(lw),
						LabG_Font(lw), LabG__acceleratorText(lw),
						LabG_InsensitiveGC(lw),
						lw->rectangle.x + LabG_AccTextRect(lw).x,
						lw->rectangle.y + LabG_AccTextRect(lw).y,
						LabG_AccTextRect(lw).width, XmALIGNMENT_END,
						LayoutG(lw), NULL);
			}
        }
    }

    /* Redraw the proper highlight  */
    if (! LabG_IsMenupane(lw) && LabG_MenuType(lw) != XmMENU_BAR)
    {
        if (lw->gadget.highlighted)
        {
            (*((XmGadgetClass) XtClass(lw))->gadget_class.border_highlight)
                ((Widget) lw);
        }
    }
}


/************************************************************************
 *
 *  SetValues
 *	This routine will take care of any changes that have been made
 *
 ************************************************************************/

static Boolean
SetValues(Widget cw,
Widget rw,
Widget nw,
ArgList args,                                     /* unused */
Cardinal *num_args)                               /* unused */
{
    XmLabelGadget current = (XmLabelGadget) cw;
    XmLabelGadget req = (XmLabelGadget) rw;
    XmLabelGadget new_w = (XmLabelGadget) nw;
    Boolean flag = False;
    Boolean newstring = False;
    Boolean ProcessFlag = FALSE;
    Boolean CleanupFontFlag = FALSE;
    Boolean Call_Resize = False;
    XmMenuSystemTrait menuSTrait;

    /* Invalidate the basline cache if necessary. */
    if ((LabG__label(new_w) != LabG__label(current)) ||
        (LabG_Font(new_w) != LabG_Font(current)))
    {
        if (new_w->label.baselines)
        {
            XtFree ((char*) new_w->label.baselines);
            new_w->label.baselines = NULL;
        }
    }

    /*  If the label has changed, make a copy of the new label,  */
    /*  and free the old label.                                  */

    if (LabG__label(new_w)!= LabG__label(current))
    {
        newstring = True;
        if (LabG__label(new_w) == NULL)
        {
            LabG__label(new_w) =
                XmStringCreateLocalized(XrmQuarkToString (current->object.xrm_name));
        }
        else
        {
            if (XmeStringIsValid((XmString) LabG__label(new_w)))
            {
                LabG__label(new_w) = XmStringCopy((XmString) LabG__label(new_w));
            }
            else
            {
                XmeWarning((Widget) new_w, CS_STRING_MESSAGE);
                LabG__label(new_w) =
                    XmStringCreateLocalized(XrmQuarkToString(new_w->object.xrm_name));
            }
        }

        XmStringFree(LabG__label(current));
        LabG__label(current)= NULL;
        LabG__label(req)= NULL;
    }

    if (LabG_MarginRight(new_w) != LabG_MarginRight(current))
        new_w->label.acc_right_delta = 0;
    if (LabG_MarginLeft(new_w) != LabG_MarginLeft(current))
        new_w->label.acc_left_delta = 0;

    if ((LabG__acceleratorText(new_w) != LabG__acceleratorText(current)) &&
        LabG_IsMenupane(new_w))
    {
        /* BEGIN OSF Fix pir 1098 */
        newstring = TRUE;
        /* END OSF Fix pir 1098 */
        if (LabG__acceleratorText(new_w) != NULL)
        {
            if (XmeStringIsValid((XmString) LabG__acceleratorText(new_w)))
            {
                if ((XmIsCascadeButtonGadget(new_w)) &&
                    (LabG__acceleratorText(new_w) != NULL))
                    LabG__acceleratorText(new_w) = NULL;
                else
                    LabG__acceleratorText(new_w) =
                        XmStringCopy(LabG__acceleratorText(new_w));
                XmStringFree(LabG__acceleratorText(current));
                LabG__acceleratorText(current)= NULL;
                LabG__acceleratorText(req)= NULL;
            }
            else
            {
                XmeWarning((Widget) new_w, ACC_MESSAGE);
                LabG__acceleratorText(new_w) = NULL;
                XmStringFree(LabG__acceleratorText(current));
                LabG__acceleratorText(current)= NULL;
                LabG__acceleratorText(req)= NULL;
            }
        }
        /* BEGIN OSF Fix pir 1098 */
        else if (LabG__acceleratorText(current))
        {
            /* CR 3481:  Don't blindly force the margin back to 0; */
            /*	try to preserve the user specified value. */
            if (LayoutIsRtoLG(new_w))
            {
                LabG_MarginLeft(new_w) -= new_w->label.acc_left_delta;
                new_w->label.acc_left_delta = 0;
            }
            else
            {
                LabG_MarginRight(new_w) -= new_w->label.acc_right_delta;
                new_w->label.acc_right_delta = 0;
            }
        }
        /* END OSF Fix pir 1098 */
    }
    else
        LabG__acceleratorText(new_w) = LabG__acceleratorText(current);

    if (LabG_Font(new_w) != LabG_Font(current))
    {
        CleanupFontFlag = True;
        if (LabG_Font(new_w) == NULL)
        {
            /* CR 2990: let subclasses pick their own default fonts. */
            LabG_Font(new_w) =
                XmeGetDefaultRenderTable((Widget) new_w, XmLABEL_FONTLIST);
        }
        LabG_Font(new_w) = XmFontListCopy (LabG_Font(new_w));
    }

    /*  Reinitialize the interesting input types.  */

    new_w->gadget.event_mask = XmHELP_EVENT;

    new_w->gadget.event_mask |=
        XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT |
        XmBDRAG_EVENT;

    if ((LabG_MenuType(new_w) == XmMENU_POPUP) ||
        (LabG_MenuType(new_w) == XmMENU_PULLDOWN) ||
        (LabG_MenuType(new_w) == XmMENU_BAR))
        new_w->gadget.highlight_thickness = 0;

    if (!XmRepTypeValidValue(XmRID_LABEL_TYPE, LabG_LabelType(new_w),
        (Widget) new_w))
    {
        LabG_LabelType(new_w) = LabG_LabelType(current);
    }

    if (!XmRepTypeValidValue(XmRID_PIXMAP_PLACEMENT, LabG_PixmapPlacement(new_w),
        (Widget) new_w))
    {
        LabG_PixmapPlacement(new_w) = LabG_PixmapPlacement(current);
    }

    if (LayoutG(new_w) != LayoutG(current))
    {
        /* If no new margins specified swap them */
        if ((LayoutIsRtoLG(current) != LayoutIsRtoLG(new_w)) &&
            (LabG_MarginLeft(current)  == LabG_MarginLeft(new_w)) &&
            (LabG_MarginRight(current) == LabG_MarginRight(new_w)))
        {
            LabG_MarginLeft(new_w)  = LabG_MarginRight(current);
            LabG_MarginRight(new_w) = LabG_MarginLeft(current);
        }
        flag = TRUE;
    }

    /* ValidateInputs(new_w); */

    if (((LabG_IsText(new_w) || LabG_IsPixmapAndText(new_w)) &&
        (newstring || (LabG_Font(new_w) != LabG_Font(current)))) ||
        ((LabG_IsPixmap(new_w) || LabG_IsPixmapAndText(new_w)) &&
        ((LabG_Pixmap(new_w) != LabG_Pixmap(current)) ||
        (LabG_PixmapInsensitive(new_w) != LabG_PixmapInsensitive(current)) ||
        /* When you have different sized pixmaps for sensitive and */
        /* insensitive states and sensitivity changes, */
        /* the right size is chosen. (osfP2560) */
        (XtIsSensitive(nw) != XtIsSensitive(cw)))) ||
	(LabG_IsPixmapAndText(new_w) &&
	LabG_PixmapPlacement(new_w) != LabG_PixmapPlacement(current)) ||
        (LabG_LabelType(new_w) != LabG_LabelType(current)))
    {
        /* CR 9179: back out CR 5419 changes. */
        _XmCalcLabelGDimensions((Widget) new_w);

        if (LabG_RecomputeSize(new_w))
        {
            if (req->rectangle.width == current->rectangle.width)
                new_w->rectangle.width = 0;
            if (req->rectangle.height == current->rectangle.height)
                new_w->rectangle.height = 0;
        }

        Call_Resize = True;

        flag = True;
    }

    if ((LabG_Alignment(new_w)!= LabG_Alignment(current)) ||
        (LayoutG(new_w) != LayoutG(current)))
    {
        if (!XmRepTypeValidValue(XmRID_ALIGNMENT, LabG_Alignment(new_w),
            (Widget) new_w))
        {
            LabG_Alignment(new_w) = LabG_Alignment(current);
        }

        Call_Resize = True;

        flag = True;
    }

    if ((LabG_MarginHeight(new_w) != LabG_MarginHeight(current)) ||
        (LabG_MarginWidth(new_w) != LabG_MarginWidth(current)) ||
        (LabG_MarginRight(new_w) != LabG_MarginRight(current)) ||
        (LabG_MarginLeft(new_w)!= LabG_MarginLeft(current)) ||
        (LabG_MarginTop(new_w)!= LabG_MarginTop(current)) ||
        (LabG_MarginBottom(new_w)!= LabG_MarginBottom(current)) ||
        (new_w->gadget.shadow_thickness != current->gadget.shadow_thickness) ||
        (new_w->gadget.highlight_thickness !=
        current->gadget.highlight_thickness) ||
        ((new_w->rectangle.width <= 0) || (new_w->rectangle.height <= 0)) ||
        (LabG_PixmapTextPadding(new_w) != LabG_PixmapTextPadding(current)))
    {
        if (LabG_RecomputeSize(new_w))
        {
            if (req->rectangle.width == current->rectangle.width)
                new_w->rectangle.width = 0;
            if (req->rectangle.height == current->rectangle.height)
                new_w->rectangle.height = 0;
        }

        Call_Resize = True;
        flag = True;
    }

    /* Resize is called only if we need to calculate the dimensions or */
    /* coordinates  for the string.				      */

    if (Call_Resize)
    {

        XtWidgetProc resize;

        _XmProcessLock();
        resize = (((XmLabelGadgetClassRec *)(new_w->object.widget_class))->
            rect_class.resize);
        _XmProcessUnlock();

        (* (resize)) ((Widget) new_w);
    }

    /* If the sensitivity has changed then we must redisplay. */
    if (XtIsSensitive(nw) != XtIsSensitive(cw))
    {
        flag = True;
    }

    /*  Force the traversal flag when in a menu.  */
    if ((XtClass(new_w) == xmLabelGadgetClass) &&
        ((LabG_MenuType(new_w) == XmMENU_POPUP) ||
        (LabG_MenuType(new_w) == XmMENU_PULLDOWN) ||
        (LabG_MenuType(new_w) == XmMENU_OPTION)))
    {
        new_w->gadget.traversal_on = False;
        new_w->gadget.highlight_on_enter = False;
    }

    if (LabG_Font(new_w) != LabG_Font(current) ||
        LabG_Foreground(new_w) != LabG_Foreground(current) ||
        LabG_Background(new_w) != LabG_Background(current))
    {
        /* Recreate the GC's if the font has been changed */
        XtReleaseGC (XtParent (current), LabG_NormalGC(current));
        XtReleaseGC (XtParent (current), LabG_InsensitiveGC(current));
        SetNormalGC(new_w);
        flag = True;
    }

    /*
     * The test for foreground is done here to allow for subclasses
     * to use this gc in a graphix op that may reference the background
     * field of the GC (i.e. in this gc background is set to LabG_Foreground.
     */

    if (LabG_Background(new_w) != LabG_Background(current) ||
        LabG_Foreground(new_w) != LabG_Foreground(current))
    {
        XtReleaseGC (XtParent (current), LabG_BackgroundGC(current));
        _XmLabelSetBackgroundGC(new_w);
        flag = True;
    }

    if (LabG_TopShadowColor(new_w) != LabG_TopShadowColor(current) ||
        LabG_TopShadowPixmap(new_w) != LabG_TopShadowPixmap(current))
    {
        XtReleaseGC (XtParent (current), LabG_TopShadowGC(current));
        LabG_TopShadowGC(new_w) =
            _XmGetPixmapBasedGC (XtParent(nw),
            LabG_TopShadowColor(new_w),
            LabG_Background(new_w),
            LabG_TopShadowPixmap(new_w));
        flag = True;
    }

    if (LabG_BottomShadowColor(new_w) != LabG_BottomShadowColor(current) ||
        LabG_BottomShadowPixmap(new_w) != LabG_BottomShadowPixmap(current))
    {
        XtReleaseGC (XtParent (current), LabG_BottomShadowGC(current));
        LabG_BottomShadowGC(new_w) =
            _XmGetPixmapBasedGC (XtParent(nw),
            LabG_BottomShadowColor(new_w),
            LabG_Background(new_w),
            LabG_BottomShadowPixmap(new_w));
        flag = True;
    }

    if (LabG_HighlightColor(new_w) != LabG_HighlightColor(current) ||
        LabG_HighlightPixmap(new_w) != LabG_HighlightPixmap(current))
    {
        XtReleaseGC (XtParent (current), LabG_HighlightGC(current));
        LabG_HighlightGC(new_w) =
            _XmGetPixmapBasedGC (XtParent(nw),
            LabG_HighlightColor(new_w),
            LabG_Background(new_w),
            LabG_HighlightPixmap(new_w));
        flag = True;
    }

    if ((LabG_MenuType(new_w) != XmWORK_AREA) &&
        (LabG_Mnemonic(new_w) != LabG_Mnemonic(current)))
    {
        /* New grabs only required if mnemonic changes */
        ProcessFlag = TRUE;
        if (LabG_LabelType(new_w) == XmSTRING ||
	   LabG_LabelType(new_w) == XmPIXMAP_AND_STRING)
            flag = TRUE;
    }

    if (LabG_MnemonicCharset(new_w) != LabG_MnemonicCharset(current))
    {
        if (LabG_MnemonicCharset(new_w))
            LabG_MnemonicCharset(new_w) =
                _XmStringCharsetCreate(LabG_MnemonicCharset (new_w));
        else
            LabG_MnemonicCharset(new_w) =
                _XmStringCharsetCreate(XmFONTLIST_DEFAULT_TAG);

        if (LabG_MnemonicCharset (current) != NULL)
            XtFree(LabG_MnemonicCharset(current));

        if (LabG_LabelType(new_w) == XmSTRING ||
	   LabG_LabelType(new_w) == XmPIXMAP_AND_STRING)
            flag = TRUE;
    }

    if (LabG_IsMenupane(new_w) &&
        (LabG_Accelerator(new_w) != LabG_Accelerator(current)))
    {
        if (LabG_Accelerator(new_w) != NULL)
        {
            /* Copy the accelerator into local space */
            LabG_Accelerator(new_w) = XtNewString(LabG_Accelerator(new_w));
        }

        if (LabG_Accelerator(current) != NULL)
            XtFree(LabG_Accelerator(current));

        LabG_Accelerator(current) = NULL;
        LabG_Accelerator(req) = NULL;
        ProcessFlag = TRUE;
    }
    else
        LabG_Accelerator(new_w) = LabG_Accelerator(current);

    menuSTrait = (XmMenuSystemTrait)
        XmeTraitGet((XtPointer) XtClass(XtParent(new_w)), XmQTmenuSystem);

    if (ProcessFlag && menuSTrait != NULL)
        menuSTrait->updateBindings((Widget)new_w, XmREPLACE);

    if (flag && (LabG_MenuType(new_w) == XmMENU_PULLDOWN) &&
        menuSTrait != NULL)
        menuSTrait->updateHistory(XtParent(new_w), (Widget) new_w, True);

    if (CleanupFontFlag)
        if (LabG_Font(current)) XmFontListFree(LabG_Font(current));

    return flag;
}


/*
 * So complications.  HandleRedraw is a wrapper that calls _XmLabelGCVTRedraw
 * which does the work for the trait stuff.
 *
 * _XmLabelGCVTRedraw returns a boolean which determines if the child, not the
 * parent should be redrawn.
 *
 * This is done to serve as a form of inheritance.  ToggleBG needs all the
 * LabelG stuff changed plus some of its own state.  Therefore ToggleBG
 * will call _XmLabelGCVTRedraw.
 */

static Boolean
HandleRedraw (Widget kid,
Widget cur_parent,
Widget new_parent,
Mask visual_flag)
{
    Boolean redraw = False;
    XtExposeProc  expose;

    /* Don't change with parent.  New Gadget behavior has been enabled. */
    redraw = _XmLabelGCVTRedraw (kid,  cur_parent, new_parent, visual_flag);

    _XmProcessLock();
    expose = ((XmLabelGadgetClassRec *)(XtClass(kid)))->rect_class.expose;
    _XmProcessUnlock();

    if (redraw)
    {
        if (! XtIsRealized(kid))
        {
            return redraw;
        }
        else
        {
            (* (expose)) ((Widget)kid, NULL, (Region) NULL);
        }
    }
    return False;
}


Boolean
_XmLabelGCVTRedraw(Widget kid,
Widget cur_parent,
Widget new_parent,
Mask visual_flag)
{
    XmLabelGadget lw = (XmLabelGadget) kid;
    XmManagerWidget mw = (XmManagerWidget) new_parent;
    XmManagerWidget curmw = (XmManagerWidget) cur_parent;
    Boolean redraw = False, do_normal = False, do_background = False;
    XmLabelGCacheObjPart oldCopy;

    /*
     * Since we are here the instance record is going to be changed.
     * So break this out out the cache, make the changes and reinsert
     * below.
     */

    _XmProcessLock();
    _XmCacheCopy((XtPointer) LabG_Cache(lw), (XtPointer) &oldCopy,
        sizeof(XmLabelGCacheObjPart));
    _XmCacheDelete ((XtPointer) LabG_Cache(lw));
    _XmProcessUnlock();
    LabG_Cache(lw) = &oldCopy;

    if ((visual_flag & VisualBackgroundPixel) &&
        (LabG_Background(lw) == curmw->core.background_pixel))
    {
        redraw = do_background = do_normal = True;
        LabG_Background(lw) = mw->core.background_pixel;
    }

    if (visual_flag & VisualBackgroundPixmap)
    {
        redraw = do_background = True;
    }

    if ((visual_flag & VisualForeground) &&
        (LabG_Foreground(lw) == curmw->manager.foreground))
    {
        redraw = do_normal = True;
        LabG_Foreground(lw) = mw->manager.foreground;
    }

    if (do_background)
    {
        XtReleaseGC (XtParent(lw), LabG_BackgroundGC(lw));
        _XmLabelSetBackgroundGC((XmLabelGadget)lw);
    }

    if (do_normal)
    {
        XtReleaseGC (XtParent(lw), LabG_NormalGC(lw));
        XtReleaseGC (XtParent(lw), LabG_InsensitiveGC(lw));
        SetNormalGC((XmLabelGadget)lw);
    }

    if (visual_flag & (VisualTopShadowColor | VisualTopShadowPixmap))
    {
        XtReleaseGC (XtParent(lw), LabG_TopShadowGC(lw));

        if(LabG_TopShadowColor(lw) == curmw->manager.top_shadow_color)
            LabG_TopShadowColor(lw) = mw->manager.top_shadow_color;

        if(LabG_TopShadowPixmap(lw) == curmw->manager.top_shadow_pixmap &&
            (LabG_TopShadowPixmap(lw) != XmUNSPECIFIED_PIXMAP
            || LabG_TopShadowColor(lw) == curmw->manager.top_shadow_color))
            LabG_TopShadowPixmap(lw) = mw->manager.top_shadow_pixmap;

        LabG_TopShadowGC(lw) =
            _XmGetPixmapBasedGC (XtParent(lw),
            LabG_TopShadowColor(lw),
            LabG_Background(lw),
            LabG_TopShadowPixmap(lw));

        redraw = True;
    }

    if (visual_flag & (VisualBottomShadowColor | VisualBottomShadowPixmap))
    {
        XtReleaseGC (XtParent(lw), LabG_BottomShadowGC(lw));

        if(LabG_BottomShadowColor(lw) == curmw->manager.bottom_shadow_color)
            LabG_BottomShadowColor(lw) = mw->manager.bottom_shadow_color;

        if(LabG_BottomShadowPixmap(lw) == curmw->manager.bottom_shadow_pixmap &&
            (LabG_BottomShadowPixmap(lw) != XmUNSPECIFIED_PIXMAP
            || LabG_BottomShadowColor(lw) == curmw->manager.bottom_shadow_color))
            LabG_BottomShadowPixmap(lw) = mw->manager.bottom_shadow_pixmap;

        LabG_BottomShadowGC(lw) =
            _XmGetPixmapBasedGC (XtParent(lw),
            LabG_BottomShadowColor(lw),
            LabG_Background(lw),
            LabG_BottomShadowPixmap(lw));

        redraw = True;
    }

    if (visual_flag & (VisualHighlightColor | VisualHighlightPixmap))
    {
        XtReleaseGC (XtParent(lw), LabG_HighlightGC(lw));

        if(LabG_HighlightColor(lw) == curmw->manager.highlight_color)
            LabG_HighlightColor(lw) = mw->manager.highlight_color;

        if(LabG_HighlightPixmap(lw) == curmw->manager.highlight_pixmap &&
            (LabG_HighlightPixmap(lw) != XmUNSPECIFIED_PIXMAP
            || LabG_HighlightColor(lw) == curmw->manager.highlight_color))
            LabG_HighlightPixmap(lw) = mw->manager.highlight_pixmap;

        LabG_HighlightGC(lw) =
            _XmGetPixmapBasedGC (XtParent(lw),
            LabG_HighlightColor(lw),
            LabG_Background(lw),
            LabG_HighlightPixmap(lw));

        redraw = True;
    }

    _XmProcessLock();
    LabG_Cache(lw) = (XmLabelGCacheObjPart *)
        _XmCachePart(LabG_ClassCachePart(lw), (XtPointer) LabG_Cache(lw),
        sizeof(XmLabelGCacheObjPart));
    _XmProcessUnlock();
    return redraw;
}


/************************************************************************
 *
 *  InputDispatch
 *     This function catches input sent by a manager and dispatches it
 *     to the individual routines.
 *
 ************************************************************************/

static void
InputDispatch(Widget wid,
XEvent *event,
Mask event_mask)
{
    XmLabelGadget lg = (XmLabelGadget) wid;

    if (event_mask & XmHELP_EVENT)
        Help ((Widget) lg, event);
    else if (event_mask & XmENTER_EVENT)
        _XmEnterGadget ((Widget) lg, event, NULL, NULL);
    else if (event_mask & XmLEAVE_EVENT)
        _XmLeaveGadget ((Widget) lg, event, NULL, NULL);
    else if (event_mask & XmFOCUS_IN_EVENT)
        _XmFocusInGadget ((Widget) lg, event, NULL, NULL);
    else if (event_mask & XmFOCUS_OUT_EVENT)
        _XmFocusOutGadget ((Widget) lg, event, NULL, NULL);
    else if (event_mask & XmBDRAG_EVENT)
        _XmProcessDrag ((Widget) lg, event, NULL, NULL);
}


/************************************************************************
 *
 *  Help
 *	This routine is called if the user made a help selection
 *      on the widget.
 *
 ************************************************************************/

static void
Help(Widget w,
XEvent *event)
{
    XmLabelGadget lg = (XmLabelGadget) w;
    XmMenuSystemTrait menuSTrait;

    menuSTrait = (XmMenuSystemTrait)
        XmeTraitGet((XtPointer) XtClass(XtParent(lg)), XmQTmenuSystem);

    if (LabG_IsMenupane(lg) && (menuSTrait != NULL))
        menuSTrait->popdown(XtParent(lg), event);

    _XmSocorro(w, event, NULL, NULL);
}


/************************************************************************
 *
 *  GetLabelString
 *     This is a get values hook function that returns the external
 *     form of the label string from the internal form.
 *
 ***********************************************************************/

static void
GetLabelString(Widget wid,
int offset,
XtArgVal *value)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    XmString string;

    string = XmStringCopy(LabG__label(lw));

    *value = (XtArgVal) string;
}


/************************************************************************
 *
 *  GetAccelerator
 *     This is a get values hook function that returns a copy
 *     of the accelerator string.
 *
 ***********************************************************************/

static void
GetAccelerator(Widget wid,
int offset,
XtArgVal *value)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    String string;

    string = XtNewString(LabG_Accelerator(lw));

    *value = (XtArgVal) string;
}


/************************************************************************
 *
 *  GetAcceleratorText
 *     This is a get values hook function that returns the external
 *     form of the accelerator text from the internal form.
 *
 ***********************************************************************/

static void
GetAcceleratorText(Widget wid,
int offset,
XtArgVal *value)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    XmString string;

    string = XmStringCopy(LabG__acceleratorText(lw));

    *value = (XtArgVal) string;
}


/************************************************************************
 *
 *  _XmStringCharsetCreate
 *
 ************************************************************************/

static XmStringCharSet
_XmStringCharsetCreate(XmStringCharSet stringcharset)
{
    return (XmStringCharSet) XtNewString((char*) stringcharset);
}


/************************************************************************
 *
 *  GetMnemonicCharset
 *     This is a get values hook function that returns the external
 *     form of the mnemonicCharset from the internal form.
 *  : Returns a string containg the mnemonicCharset.
 *    Caller must free the string .
 *
 ***********************************************************************/

static void
GetMnemonicCharset(Widget wid,
int resource,                                     /* unused */
XtArgVal *value)
{
    XmLabelGadget lw = (XmLabelGadget) wid;
    char *cset;
    int   size;

    cset = NULL;
    if (LabG_MnemonicCharset (lw))
    {
        size = strlen (LabG_MnemonicCharset (lw));
        if (size > 0)
            cset = (char *) (_XmStringCharsetCreate(LabG_MnemonicCharset (lw)));
    }

    *value = (XtArgVal) cset;
}


/************************************************************************
 *
 *  Caching Assignment help
 *     These routines are for manager widgets that go into Label's
 *     fields and set them, instead of doing a SetValues.
 *
 ************************************************************************/

static XmLabelGCacheObjPart local_cache;
static Boolean local_cache_inited = FALSE;

/*
 * QualifyLabelLocalCache
 *  Checks to see if local cache is set up
 */
static void
QualifyLabelLocalCache(XmLabelGadget w)
{
    if (!local_cache_inited)
    {
        local_cache_inited = TRUE;
        ClassCacheCopy(LabG_ClassCachePart(w))
            (LabG_Cache(w), &local_cache, sizeof(local_cache));
    }
}


/************************************************************************
 *
 * _XmReCacheLabG()
 * Check to see if ReCaching is necessary as a result of fields having
 * been set by a mananger widget. This routine is called by the
 * manager widget in their SetValues after a change is made to any
 * of Label's cached fields.
 *
 ************************************************************************/
void _XmReCacheLabG(Widget wid)
{
    XmLabelGadget lw = (XmLabelGadget) wid;

    _XmProcessLock();
    if (local_cache_inited &&
        (!_XmLabelCacheCompare((XtPointer)&local_cache,
        (XtPointer)LabG_Cache(lw))))
    {
        /* Delete the old one. */
        _XmCacheDelete((XtPointer) LabG_Cache(lw));
        LabG_Cache(lw) = (XmLabelGCacheObjPart *)
            _XmCachePart(LabG_ClassCachePart(lw),
            (XtPointer) &local_cache, sizeof(local_cache));
    }
    local_cache_inited = FALSE;
    _XmProcessUnlock();
}


void _XmAssignLabG_MarginHeight(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_height = value;
        _XmProcessUnlock();
}


void _XmAssignLabG_MarginWidth(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_width = value;
        _XmProcessUnlock();
}


void _XmAssignLabG_MarginLeft(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_left = value;
        _XmProcessUnlock();
}

void _XmAssignLabG_MarginRight(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_right = value;
        _XmProcessUnlock();
}

void _XmAssignLabG_MarginTop(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_top = value;
        _XmProcessUnlock();
}

void _XmAssignLabG_MarginBottom(XmLabelGadget lw, Dimension value)
{
    _XmProcessLock();
        QualifyLabelLocalCache(lw);
        local_cache.margin_bottom = value;
        _XmProcessUnlock();
}

/************************************************************************
 *
 *  SetGadgetActivateCallbackState
 *
 * This function is used as the method of the menuSavvy trait. It is
 * used by menu savvy parents to set whether or not the child will
 * invoke its own activate callback or whether it will defer to the
 * entryCallback of the parent.
 *
 ************************************************************************/
static void SetGadgetActivateCallbackState(Widget wid, XmActivateState state)
{
    XmLabelGCacheObjPart localCache;

        _XmQualifyLabelLocalCache(&localCache, (XmLabelGadget)wid);

        switch (state)
    {
        case XmDISABLE_ACTIVATE:
            localCache.skipCallback = True;
                break;

            case XmENABLE_ACTIVATE:
            localCache.skipCallback = False;
                break;
    }

    _XmReCacheLabG_r(&localCache, (XmLabelGadget)wid);
}


/************************************************************************
 *
 *  SetOverrideCallback
 *
 * Used by subclasses.  If this is set true, then there is a RowColumn
 * parent with the entryCallback resource set.  The subclasses do not
 * do their activate callbacks, instead the RowColumn callbacks are called
 * by RowColumn.
 ************************************************************************/
static void SetOverrideCallback(Widget w)
{
    XmLabelGCacheObjPart localCache;

        _XmQualifyLabelLocalCache(&localCache, (XmLabelGadget)w);
        localCache.skipCallback= True;
        _XmReCacheLabG_r(&localCache, (XmLabelGadget)w);
}

/************************************************************************
 *
 *  XmCreateLabelGadget()
 *  XmVaCreateLabelGadget()
 *  XmVaCreateManagedLabelGadget()
 *
 *	Externally accessable function for creating a label gadget.
 *
 ************************************************************************/
Widget XmCreateLabelGadget(Widget parent, char *name, Arg *arglist, Cardinal argCount)
{
    return XtCreateWidget(name, xmLabelGadgetClass, parent, arglist, argCount);
}

Widget XmVaCreateLabelGadget(Widget parent, char *name, ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmLabelGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;

}
Widget XmVaCreateManagedLabelGadget(Widget parent, char *name, ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmLabelGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;

}

/*
 *  GetLabelBGClassSecResData ()
 *    Class function to be called to copy secondary resource for external
 *  use.  i.e. copy the cached resources and send it back.
 */
static Cardinal GetLabelBGClassSecResData(WidgetClass w_class, XmSecondaryResourceData **data_rtn)
{
    int        arrayCount;
        XmBaseClassExt bcePtr;
        String     resource_class, resource_name;
        XtPointer  client_data;

        _XmProcessLock();
        bcePtr = &(labelBaseClassExtRec);
        client_data = NULL;
        resource_class = NULL;
        resource_name = NULL;
        arrayCount =
        _XmSecondaryResourceData(bcePtr, data_rtn, client_data,
        resource_name, resource_class,
        GetLabelClassResBase);
        _XmProcessUnlock();
        return arrayCount;
}

/*
 * GetLabelClassResBase ()
 *   retrun the address of the base of resources.
 *   - Not yet implemented.
 */
static XtPointer GetLabelClassResBase(Widget widget,
                                      XtPointer client_data) /* unused */
{
    XtPointer widgetSecdataPtr;
        int       labg_cache_size = sizeof(XmLabelGCacheObjPart);
        char     *cp;

        widgetSecdataPtr = (XtPointer) (XtMalloc (labg_cache_size +1));

        _XmProcessLock();
        if (widgetSecdataPtr)
    {
        cp = (char *) widgetSecdataPtr;
            memcpy(cp, LabG_Cache(widget), labg_cache_size);
    }

    _XmProcessUnlock();
        return widgetSecdataPtr;
}

static void SetValuesAlmost(Widget cw, /* unused */
                            Widget nw,
                            XtWidgetGeometry *request,
                            XtWidgetGeometry *reply)
{
    XmLabelGadget new_w = (XmLabelGadget) nw;
        XtWidgetProc resize;

        _XmProcessLock();
        resize = ((XmLabelGadgetClassRec *)(new_w->object.widget_class))->
        rect_class.resize;
        _XmProcessUnlock();

        (* (resize)) ((Widget) new_w);
        *request = *reply;
}


static void GetColors(Widget w, XmAccessColorData color_data)
{
    if (LabG_Cache(w))
    {
        color_data->valueMask = AccessForeground | AccessBackgroundPixel |
            AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
            color_data->background = LabG_Background(w);
            color_data->foreground = LabG_Foreground(w);
            color_data->highlight_color = LabG_HighlightColor(w);
            color_data->top_shadow_color = LabG_TopShadowColor(w);
            color_data->bottom_shadow_color = LabG_BottomShadowColor(w);
    }
    else
    {
        color_data->valueMask = AccessColorInvalid;
    }
}

/************************************************************************
 *
 * XmLabelGadgetGetBaselines
 *
 * A Class function which when called returns True, if the widget has
 * a baseline and also determines the number of pixels from the y
 * origin to the first line of text and assigns it to the variable
 * being passed in.
 *
 ************************************************************************/
static Boolean XmLabelGadgetGetBaselines(Widget wid, Dimension **baselines, int *line_count)
{
    XmLabelGadget lw = (XmLabelGadget)wid;
        Cardinal count;
        int delta;

        if (LabG_IsPixmap(wid))
        return False;

    /* Compute raw baselines if unavailable. */
        if (lw->label.baselines == NULL)
    {
        _XmStringGetBaselines(LabG_Font(lw), LabG__label(lw),
            &(lw->label.baselines), &count);
            assert(lw->label.baselines != NULL);

        /* Store the current offset in an extra location. */
            lw->label.baselines = (Dimension*)
            XtRealloc((char*) lw->label.baselines, (count+1) * sizeof(Dimension));
            lw->label.baselines[count] = 0;
    }
    else
    {
        count = XmStringLineCount(LabG__label(lw));
    }

    /* Readjust offsets if necessary. */
    delta = LabG_TextRect_y(lw) - lw->label.baselines[count];
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
 * XmLabelGadgetGetDisplayRect
 *
 * A Class function which returns true if the widget being passed in
 * has a display rectangle associated with it. It also determines the
 * x,y coordinates of the character cell or pixmap relative to the origin
 * and the width and height in pixels of the smallest rectangle that encloses
 * the text or pixmap. This is assigned to the variable being passed in
 *
 ***********************************************************************/
static Boolean XmLabelGadgetGetDisplayRect(Widget w, XRectangle *displayrect)
{
    XmLabelGadget wid = (XmLabelGadget) w;

        displayrect->x = wid->label.TextRect.x;
        displayrect->y = wid->label.TextRect.y;
        displayrect->width = wid->label.TextRect.width;
        displayrect->height = wid->label.TextRect.height;

        return TRUE;
}

/************************************************************************
 *
 * XmLabelGadgetMarginsProc
 *
 ***********************************************************************/
static void XmLabelGadgetMarginsProc(Widget w, XmBaselineMargins *margins_rec)
{
    XmLabelGCacheObjPart localCache;

        if (margins_rec->get_or_set == XmBASELINE_SET)
    {
        _XmQualifyLabelLocalCache(&localCache, (XmLabelGadget)w);
            _XmAssignLabG_MarginTop_r((&localCache), margins_rec->margin_top);
            _XmAssignLabG_MarginBottom_r((&localCache), margins_rec->margin_bottom);
            _XmReCacheLabG_r(&localCache, (XmLabelGadget)w);
    }
    else
    {
        margins_rec->margin_top = LabG_MarginTop(w);
            margins_rec->margin_bottom = LabG_MarginBottom(w);
            margins_rec->shadow = LabG_Shadow(w);
            margins_rec->highlight = LabG_Highlight(w);
            margins_rec->text_height = LabG_TextRect_height(w);
            margins_rec->margin_height = LabG_MarginHeight(w);
    }
}

static Widget GetPixmapDragIcon(Widget w)
{
    XmLabelGadget lw = (XmLabelGadget) w;
        Arg args[10];
        int n = 0;
        Widget drag_icon;
        Widget screen_object = XmGetXmScreen(XtScreen(w));
        unsigned int wid, hei;
        int d;

    /* it's a labelPixmap, use directly the pixmap */

        XmeGetPixmapData(XtScreen(lw), Pix(lw), NULL, &d,
        NULL, NULL, NULL, NULL, &wid, &hei);

        n = 0;
        XtSetArg(args[n], XmNhotX, 0),             n++;
        XtSetArg(args[n], XmNhotY, 0),             n++;
        XtSetArg(args[n], XmNwidth, wid),          n++;
        XtSetArg(args[n], XmNheight, hei),             n++;
        XtSetArg(args[n], XmNmaxWidth, wid),           n++;
        XtSetArg(args[n], XmNmaxHeight, hei),          n++;
        XtSetArg(args[n], XmNdepth, d),            n++;
        XtSetArg(args[n], XmNpixmap, Pix(lw)),         n++;
        XtSetArg(args[n], XmNforeground, LabG_Background(lw)), n++;
        XtSetArg(args[n], XmNbackground, LabG_Foreground(lw)), n++;
        assert(n <= XtNumber(args));
        drag_icon = XtCreateWidget("drag_icon", xmDragIconObjectClass,
        screen_object, args, n);
        return drag_icon;
}

void _XmProcessDrag(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    XmLabelGadget lw = (XmLabelGadget) w;
        Widget drag_icon;
        Arg args[10];
        int n;
        XmManagerWidget mw;
        Time _time = _XmGetDefaultTime(w, event);
        XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(w));

        mw = (XmManagerWidget) XtParent(lw);

        if (LabG_IsMenupane(w))
        XAllowEvents(XtDisplay(mw), SyncPointer, _time);

    /* Disallow drag if this is a cascade button and armed - Hack alert */
        if (XmIsCascadeButtonGadget(w) && CBG_IsArmed(w)) return;

    /* CDE - allow user to not drag labels and label subclasses
       also,  disable drag if enable_btn1_transfer is set to
       BUTTON2_ADJUST and the trigger was button2 */
        if (! dpy -> display.enable_unselectable_drag ||
        (dpy -> display.enable_btn1_transfer == XmBUTTON2_ADJUST &&
        event && event -> xany.type == ButtonPress &&
        event -> xbutton.button == 2)) return;

        n = 0;
        XtSetArg(args[n], XmNcursorBackground, LabG_Background(lw)), n++;
        XtSetArg(args[n], XmNcursorForeground, LabG_Foreground(lw)), n++;

    /* If it's a labelPixmap, only specify the pixmap icon */
        if (LabG_IsPixmap(lw) && (Pix(lw) != XmUNSPECIFIED_PIXMAP))
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

/*
 * XmRCallProc routine for checking label.font before setting it to NULL
 * If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */
static void CheckSetRenderTable(Widget wid, int offset, XrmValue *value)
{
    XmLabelGadget lw = (XmLabelGadget)wid;

    /* Check if been here before */
        if (lw->label.check_set_render_table)
        value->addr = NULL;
        else
    {
        lw->label.check_set_render_table = True;
            value->addr = (char*)&(lw->label.font);
    }
}

/**************************************************************************
 * FromPaddingPixels
 *
 * Converts from pixels to current unit type does either horiz or vert
 * depending on icon placement.
 *  widget - the icon button widget.
 *  offset, value - passed to correct function based on orientation.
 **************************************************************************/
static void FromPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    switch(LabG_PixmapPlacement(widget)) {
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
static XmImportOperator ToPaddingPixels(Widget widget, int offset, XtArgVal *value)
{
    switch(LabG_PixmapPlacement(widget)) {
    case XmPIXMAP_TOP:
    case XmPIXMAP_BOTTOM:
	return(XmeToVerticalPixels(widget, offset, value));
    default:
	return(XmeToHorizontalPixels(widget, offset, value));
    }
}

static char *GetLabelGadgetAccelerator(Widget w)
{
    if (XtClass(w) == xmLabelGadgetClass)
        return NULL;
        else
        return LabG_Accelerator(w);
}

static KeySym GetLabelGadgetMnemonic(Widget w)
{
    if (XtClass(w) == xmLabelGadgetClass)
        return XK_VoidSymbol;
        else
        return LabG_Mnemonic(w);
}

void _XmQualifyLabelLocalCache(XmLabelGCacheObjPart *_local_cache,
                               XmLabelGadget w)
{
    _XmProcessLock();
        ClassCacheCopy(LabG_ClassCachePart(w))
        (LabG_Cache(w),
        _local_cache,
        sizeof(XmLabelGCacheObjPart)
        );
        _XmProcessUnlock();
}


void _XmReCacheLabG_r(XmLabelGCacheObjPart *_local_cache, XmLabelGadget w)
{
    _XmProcessLock();
        if (!_XmLabelCacheCompare(_local_cache, (XtPointer)LabG_Cache(w)))
    {
        _XmCacheDelete((XtPointer)LabG_Cache(w));
            LabG_Cache(w) = (XmLabelGCacheObjPart *)_XmCachePart(
            LabG_ClassCachePart(w),
            _local_cache,
            sizeof(XmLabelGCacheObjPart));
    }
    _XmProcessUnlock();
}

