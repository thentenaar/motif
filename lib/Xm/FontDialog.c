/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#if USE_XFT
#include <fontconfig/fontconfig.h>
#include <X11/Xft/Xft.h>
#endif

#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/RepType.h>
#include <Xm/BulletinB.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/FontDialog.h>
#include "BulletinBI.h"
#include "GeoUtilsI.h"
#include "XmRenderTI.h"
#include "RepTypeI.h"
#include "HashI.h"
#include "FontDialogP.h"

/**
 * Font property tree
 *
 * family ---> styles ---> sizes
 */
struct font_prop {
	XmStringTable list;    /**< List (of families / styles / sizes) */
	XmRendition  *rend;    /**< One rendition per fam+style+size    */
	XmHashTable   ht;      /**< Hash table against list             */
	Cardinal      cnt;     /**< Number of entries in list           */
	Cardinal      n_child; /**< Number of children                  */
	struct font_prop **children;
};

static const char *default_sizes[14] = {
	"8",  "9",  "10", "11", "12", "13", "14",
	"16", "18", "20", "24", "36", "48", "72"
};

static void ClassPartInitialize(WidgetClass wc);
static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num_args);
static void Destroy(Widget w);
static Boolean SetValues(Widget cur, Widget req, Widget new,
                         ArgList args, Cardinal *num_args);
static void DeleteChild(Widget child);
static XmGeoMatrix GeoMatrixCreate(Widget w, Widget inst, XtWidgetGeometry *desired);

#if USE_XFT
static void load_fontconfig(Widget w, struct font_prop *info, int sz);
#endif
static void load_corefonts(Widget w, struct font_prop *info, int sz);
static void font_prop_destroy(struct font_prop *p);
static void update_sample(XmFontDialogWidget fd);
static void font_select(Widget w, XtPointer client, XtPointer call);
static void style_select(Widget w, XtPointer client, XtPointer call);
static void size_select(Widget w, XtPointer client, XtPointer call);
static void button_proc(Widget w, XtPointer client, XtPointer call);

/**
 * Synthetics: return a copy of a label's string.
 */
static void get_it(Widget w, int offset, XtArgVal *val)
{
	Arg arg;
	XmString label = NULL;
	XmFontDialogWidget fd = (XmFontDialogWidget)w;

	XtSetArg(arg, XmNlabelString, &label);
	switch (offset) {
	case XtOffsetOf(XmFontDialogRec, fontdlg.font_string):
		w = fd->fontdlg.font_label;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.style_string):
		w =  fd->fontdlg.style_label;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.size_string):
		w = fd->fontdlg.size_label;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.sample_title):
		w = fd->fontdlg.sample_label;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.sample_text):
		w = fd->fontdlg.sample;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.ok_string):
		w = fd->fontdlg.ok_button;
		break;
	case XtOffsetOf(XmFontDialogRec, fontdlg.cancel_string):
		w = fd->fontdlg.cancel_button;
		break;
	default:
		w = NULL;
	}

	if (w) XtGetValues(w, &arg, 1);
	*val = (XtArgVal)label;
}

/**
 * We'll pick these up in SetValues.
 */
static XmImportOperator set_it(Widget w, int offset, XtArgVal *val)
{
	(void)w;
	(void)offset;
	(void)val;
	return XmSYNTHETIC_LOAD;
}

/**
 * Just to be sure we return a copy
 */
static XmSyntheticResource synthetics[] = {
{
	XmNfontString, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.font_string),
	get_it,
	set_it
},
{
	XmNstyleString, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.style_string),
	get_it,
	set_it
},
{
	XmNsizeString, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.size_string),
	get_it,
	set_it
},
{
	XmNsampleTitle, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.sample_title),
	get_it,
	set_it
},
{
	XmNsampleText, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.sample_text),
	get_it,
	set_it
},
{
	XmNokLabelString, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.ok_string),
	get_it,
	set_it
},
{
	XmNcancelLabelString, sizeof(XmString),
	XtOffsetOf(XmFontDialogRec, fontdlg.cancel_string),
	get_it,
	set_it
}};

static XtResource resources[] = {
{
	XmNdialogType, XmCDialogType, XmRSelectionType,
	sizeof(unsigned char), XtOffsetOf(XmFontDialogRec, fontdlg.type),
	XmRImmediate, (XtPointer)XmDIALOG_FONT
},
{ /* Xft or X core fonts */
	XmNsource, XmCSource, XmRFontSource,
	sizeof(unsigned char), XtOffsetOf(XmFontDialogRec, fontdlg.source),
	XmRImmediate, (XtPointer)XmFONT_SOURCE_XFT
},
{ /* If using corefonts, use >= 96 dpi fonts only */
	XmNprefer100dpi, XmCprefer100dpi, XmRBoolean,
	sizeof(Boolean), XtOffsetOf(XmFontDialogRec, fontdlg.prefer_100dpi),
	XmRImmediate, (XtPointer)True
},
{
	XmNvisibleItemCount, XmCVisibleItemCount, XmRInt,
	sizeof(int), XtOffsetOf(XmFontDialogRec, fontdlg.visible_item_count),
	XmRImmediate, (XtPointer)6
},
{
	XmNfontString, XmCfontString, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.font_string),
	XtRString, "Font:"
},
{
	XmNstyleString, XmCstyleString, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.style_string),
	XtRString, "Style:"
},
{
	XmNsizeString, XmCSizeString, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.size_string),
	XtRString, "Size:"
},
{
	XmNsampleTitle, XmCsampleTitle, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.sample_title),
	XtRString, "Sample"
},
{
	XmNsampleText, XmCsampleText, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.sample_text),
	XtRString, "AaBbYyZz"
},
{
	XmNokLabelString, XmCOkLabelString, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.ok_string),
	XmRImmediate, NULL
},
{
	XmNcancelLabelString, XmCCancelLabelString, XmRXmString,
	sizeof(XmString), XtOffsetOf(XmFontDialogRec, fontdlg.cancel_string),
	XmRImmediate, NULL
},
{
	XmNokCallback, XmCCallback, XmRCallback,
	sizeof(XtCallbackList), XtOffsetOf(XmFontDialogRec, fontdlg.ok_callback),
	XmRImmediate, NULL
},
{
	XmNcancelCallback, XmCCallback, XmRCallback,
	sizeof(XtCallbackList), XtOffsetOf(XmFontDialogRec, fontdlg.cancel_callback),
	XmRImmediate, NULL
}};

XmFontDialogClassRec xmFontDialogClassRec = {
{ /* core */
	(WidgetClass)&xmBulletinBoardClassRec, /* superclass */
	"XmFontDialog",              /* class_name           */
	sizeof(XmFontDialogRec),     /* size                 */
	NULL,                        /* class_init           */
	ClassPartInitialize,         /* class_part_init      */
	False,                       /* class initialized?   */
	Initialize,                  /* initialize           */
	NULL,                        /* initialize_notify    */
	XtInheritRealize,            /* realize              */
	NULL,                        /* actions              */
	0,                           /* num_actions          */
	resources,                   /* resources            */
	XtNumber(resources),         /* resource_count       */
	NULLQUARK,                   /* xrm_class            */
	True,                        /* compress_motion      */
	XtExposeCompressMaximal,     /* compress_exposure    */
	True,                        /* compress_enterleave  */
	False,                       /* visible_interest     */
	Destroy,                     /* destroy              */
	XtInheritResize,             /* resize               */
	XtInheritExpose,             /* expose               */
	SetValues,                   /* set_values           */
	NULL,                        /* set_values_hook      */
	XtInheritSetValuesAlmost,    /* set_values_almost    */
	NULL,                        /* get_values_hook      */
	NULL,                        /* accept_focus         */
	XtVersion,                   /* intrinsics version   */
	NULL,                        /* callback offsets     */
	XtInheritTranslations,       /* tm_table             */
	XtInheritGeometryManager,    /* query_geometry       */
	NULL,                        /* screen_accelerator   */
	NULL,                        /* extension            */
},
{ /* composite */
	XtInheritGeometryManager,    /* geometry manager     */
	XtInheritChangeManaged,      /* changed proc         */
	XtInheritInsertChild,        /* insert_child         */
	DeleteChild,                 /* delete_child         */
	NULL,                        /* extension            */
},
{ /* constraint */
	NULL,                        /* additional_res.      */
	0,                           /* n_additional_res.    */
	0,                           /* constraint_rec_size  */
	NULL,                        /* constraint_init      */
	NULL,                        /* constraint_destroy   */
	NULL,                        /* constraint_setvalue  */
	NULL,                        /* extension            */
},
{ /* manager */
	XtInheritTranslations,       /* default translations */
	synthetics,                  /* sym_resources        */
	XtNumber(synthetics),        /* num_syn_resources    */
	NULL,                        /* syn_cont_resources   */
	0,                           /* n_syn_cont_resources */
	XmInheritParentProcess,      /* parent_process       */
	NULL,                        /* extension            */
},
{ /* bulletin_board */
	False,                       /* always_install_accel */
	GeoMatrixCreate,             /* geo_matrix_create    */
	XmInheritFocusMovedProc,     /* focus_moved_proc     */
	NULL,                        /* extension            */
},
{ /* font_dialog */
	NULL                         /* extension            */
}};

WidgetClass xmFontDialogWidgetClass = (WidgetClass)&xmFontDialogClassRec;

/**
 * I can't imagine a case for subclassing this. However, the other dialogs
 * present themselves with the "fast subclass" mechanism, so we'll do
 * the same for parity.
 */
static void ClassPartInitialize(WidgetClass wc)
{
	_XmFastSubclassInit(wc, XmFONT_DIALOG_BIT);
}

/**
 * Instance initialization
 */
static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num_args)
{
	int sz;
	Cardinal i;
	Arg arg[5];
	XmRenderTable rt;
	struct font_prop *info;
	XmFontDialogWidget fd = (XmFontDialogWidget)new;

	(void)req;
	if (!XmRepTypeValidValue(XmRID_DIALOG_TYPE, fd->fontdlg.type, new))
		fd->fontdlg.type = XmDIALOG_FONT;

	fd->fontdlg.selected_font  = 1;
	fd->fontdlg.selected_style = 1;
	fd->fontdlg.selected_size  = 1;
	fd->fontdlg.sample         = NULL;

	/* Determine our default pixelsize */
	if (!(rt = fd->bulletin_board.label_font_list))
		rt = XmeGetDefaultRenderTable(new, XmLABEL_FONTLIST);
	sz = 1 + XmStringBaseline(rt, fd->fontdlg.sample_text);

	/* Build font list  */
	info     = (struct font_prop *)XtCalloc(1, sizeof *info);
	info->ht = _XmAllocHashTable(1024, XmHashCompareXmStringLower, XmHashXmStringLower);

	/**
	 * The user can select whether to source from Fontconfig or corefonts,
	 * but we have to be sure to load something.
	 */
#if USE_XFT
	if (fd->fontdlg.source == XmFONT_SOURCE_XFT)
		load_fontconfig(new, info, sz);
#else
	fd->fontdlg.source = XmFONT_SOURCE_X;
#endif

	if (fd->fontdlg.source == XmFONT_SOURCE_X)
		load_corefonts(new, info, sz);
	fd->fontdlg.info = info;

	/**
	 * Labels
	 */
	XtSetArg(arg[0], XmNborderWidth, 0);
	XtSetArg(arg[1], XmNtraversalOn, False);
	XtSetArg(arg[2], XmNalignment, XmALIGNMENT_BEGINNING);
	XtSetArg(arg[3], XmNlabelString, fd->fontdlg.font_string);
	fd->fontdlg.font_label = XmCreateLabelGadget(new, "Font", arg, 4);

	XtSetArg(arg[3], XmNlabelString, fd->fontdlg.style_string);
	fd->fontdlg.style_label = XmCreateLabelGadget(new, "Style", arg, 4);

	XtSetArg(arg[3], XmNlabelString, fd->fontdlg.size_string);
	fd->fontdlg.size_label = XmCreateLabelGadget(new, "Size", arg, 4);

	/**
	 * Lists
	 */
	XtSetArg(arg[0], XmNitems,            info->list);
	XtSetArg(arg[1], XmNitemCount,        info->cnt);
	XtSetArg(arg[2], XmNvisibleItemCount, fd->fontdlg.visible_item_count);
	XtSetArg(arg[3], XmNselectionPolicy,  XmSINGLE_SELECT);
	XtSetArg(arg[4], XmNnavigationType,   XmSTICKY_TAB_GROUP);
	fd->fontdlg.font = XmCreateScrolledList(new, "FontList", arg, 5);
	XmListSelectPos(fd->fontdlg.font, fd->fontdlg.selected_font, False);
	XtManageChild(fd->fontdlg.font);

	XtSetArg(arg[0], XmNitems,           info->children[0]->list);
	XtSetArg(arg[1], XmNitemCount,       info->children[0]->cnt);
	fd->fontdlg.style = XmCreateScrolledList(new, "StyleList", arg, 5);
	XmListSelectPos(fd->fontdlg.style, fd->fontdlg.selected_style, False);
	XtManageChild(fd->fontdlg.style);

	XtSetArg(arg[0], XmNitems,           info->children[0]->children[0]->list);
	XtSetArg(arg[1], XmNitemCount,       info->children[0]->children[0]->cnt);
	fd->fontdlg.size = XmCreateScrolledList(new, "SizeList", arg, 5);
	XmListSelectPos(fd->fontdlg.size, fd->fontdlg.selected_size, False);
	XtManageChild(fd->fontdlg.size);

	/**
	 * Sample frame
	 */
	XtSetArg(arg[0], XmNshadowType, XmSHADOW_ETCHED_IN);
	fd->fontdlg.sample_frame = XmCreateFrame(new, "SampleFrame", arg, 1);

	XtSetArg(arg[0], XmNborderWidth, 0);
	XtSetArg(arg[1], XmNtraversalOn, False);
	XtSetArg(arg[2], XmNlabelString, fd->fontdlg.sample_title);
	XtSetArg(arg[3], XmNchildType,   XmFRAME_TITLE_CHILD);
	XtSetArg(arg[4], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	fd->fontdlg.sample_label = XmCreateLabelGadget(
		fd->fontdlg.sample_frame, "SampleFrameTitle", arg, 5
	);
	XtManageChild(fd->fontdlg.sample_label);

	/**
	 * Separator + Buttons
	 */
	XtSetArg(*arg, XmNhighlightThickness, 0);
	fd->fontdlg.separator = XmCreateSeparatorGadget(new, "Separator", arg, 1);
	fd->fontdlg.ok_button = _XmBB_CreateButtonG(
		new, fd->fontdlg.ok_string, "OK", XmOkStringLoc
	);

	fd->fontdlg.cancel_button = _XmBB_CreateButtonG(
		new, fd->fontdlg.cancel_string, "Cancel", XmCancelStringLoc
	);

	/**
	 * List Callbacks
	 */
	XtAddCallback(fd->fontdlg.font,  XmNsingleSelectionCallback, font_select,  fd);
	XtAddCallback(fd->fontdlg.style, XmNsingleSelectionCallback, style_select, fd);
	XtAddCallback(fd->fontdlg.size,  XmNsingleSelectionCallback, size_select,  fd);

	/**
	 * Button callbacks
	 */
	XtRemoveAllCallbacks(fd->fontdlg.ok_button, XmNactivateCallback);
	XtRemoveAllCallbacks(fd->fontdlg.cancel_button, XmNactivateCallback);
	XtAddCallback(fd->fontdlg.ok_button,     XmNactivateCallback,
	              button_proc, (XtPointer)XmDIALOG_OK_BUTTON);
	XtAddCallback(fd->fontdlg.cancel_button, XmNactivateCallback,
	              button_proc, (XtPointer)XmDIALOG_CANCEL_BUTTON);

	/* Update the sample label */
	XtManageChildren(fd->composite.children, fd->composite.num_children);
	update_sample(fd);

	/* The Ok button should be the default button */
	BB_DefaultButton(fd) = fd->fontdlg.ok_button;
	_XmBulletinBoardSetDynDefaultButton(new, fd->fontdlg.ok_button);

	/* Initial focus should be on the font list */
	fd->manager.initial_focus = fd->fontdlg.font;
}

static void Destroy(Widget w)
{
	Cardinal i, j;
	XmFontDialogWidget fd = (XmFontDialogWidget)w;
	struct font_prop *info;

	if (fd->fontdlg.rend) {
		XmRenditionDematerialize(*_XmRTRenditions(fd->fontdlg.rend));
		XmRenderTableFree(fd->fontdlg.rend);
	}

	font_prop_destroy((struct font_prop *)fd->fontdlg.info);
}

static Boolean SetValues(Widget cur_w, Widget req, Widget new_w,
                         ArgList args, Cardinal *num_args)
{
	Arg arg[3];
	XmFontDialogWidget new = (XmFontDialogWidget)new_w;
	XmFontDialogWidget cur = (XmFontDialogWidget)cur_w;

	(void)req;
	(void)args;
	(void)num_args;

	BB_InSetValues(new) = True;
	if (new->fontdlg.visible_item_count != cur->fontdlg.visible_item_count) {
		XtSetArg(arg[0], XmNvisibleItemCount, new->fontdlg.visible_item_count);
		if (new->fontdlg.font)  XtSetValues(new->fontdlg.font,  arg, 1);
		if (new->fontdlg.style) XtSetValues(new->fontdlg.style, arg, 1);
		if (new->fontdlg.size)  XtSetValues(new->fontdlg.size,  arg, 1);
	}

	if (new->fontdlg.font_string != cur->fontdlg.font_string) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.font_string);
		if (new->fontdlg.font_label) {
			XtSetValues(new->fontdlg.font_label, arg, 1);
			new->fontdlg.font_string = NULL;
		}
	}

	if (new->fontdlg.style_string != cur->fontdlg.style_string) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.style_string);
		if (new->fontdlg.style_label) {
			XtSetValues(new->fontdlg.style_label, arg, 1);
			new->fontdlg.style_string = NULL;
		}
	}

	if (new->fontdlg.size_string != cur->fontdlg.size_string) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.size_string);
		if (new->fontdlg.size_label) {
			XtSetValues(new->fontdlg.size_label, arg, 1);
			new->fontdlg.size_string = NULL;
		}
	}

	if (new->fontdlg.sample_title != cur->fontdlg.sample_title) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.sample_title);
		if (new->fontdlg.sample_label) {
			XtSetValues(new->fontdlg.sample_label, arg, 1);
			new->fontdlg.sample_title = NULL;
		}
	}

	if (new->fontdlg.sample_text != cur->fontdlg.sample_text) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.sample_text);
		if (new->fontdlg.sample)
			XtSetValues(new->fontdlg.sample, arg, 1);
	}

	if (new->fontdlg.ok_string != cur->fontdlg.ok_string) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.ok_string);
		if (new->fontdlg.ok_button) {
			XtSetValues(new->fontdlg.ok_button, arg, 1);
			new->fontdlg.ok_string = NULL;
		}
	}

	if (new->fontdlg.cancel_string != cur->fontdlg.cancel_string) {
		XtSetArg(arg[0], XmNlabelString, new->fontdlg.cancel_string);
		if (new->fontdlg.cancel_button) {
			XtSetValues(new->fontdlg.cancel_button, arg, 1);
			new->fontdlg.cancel_string = NULL;
		}
	}

	if (new->fontdlg.type != cur->fontdlg.type) {
		XmeWarning(new_w, "Dialog type cannot be modified.");
		new->fontdlg.type = cur->fontdlg.type;
	}

	BB_InSetValues(new) = False;
	_XmBulletinBoardSizeUpdate(new_w);
	return False;
}

static void DeleteChild(Widget child)
{
	XtWidgetProc del;
	XmFontDialogWidget fd;

	if (!XtIsRectObj(child))
		goto out;

	fd = (XmFontDialogWidget)XtParent(child);
	if (child == fd->fontdlg.font)          fd->fontdlg.font          = NULL;
	if (child == fd->fontdlg.font_label)    fd->fontdlg.font_label    = NULL;
	if (child == fd->fontdlg.style)         fd->fontdlg.style         = NULL;
	if (child == fd->fontdlg.style_label)   fd->fontdlg.style_label   = NULL;
	if (child == fd->fontdlg.size)          fd->fontdlg.size          = NULL;
	if (child == fd->fontdlg.size_label)    fd->fontdlg.size_label    = NULL;
	if (child == fd->fontdlg.sample)        fd->fontdlg.sample        = NULL;
	if (child == fd->fontdlg.sample_frame)  fd->fontdlg.sample_frame  = NULL;
	if (child == fd->fontdlg.sample_label)  fd->fontdlg.sample_label  = NULL;
	if (child == fd->fontdlg.separator)     fd->fontdlg.separator     = NULL;
	if (child == fd->fontdlg.ok_button)     fd->fontdlg.ok_button     = NULL;
	if (child == fd->fontdlg.cancel_button) fd->fontdlg.cancel_button = NULL;

out:
	_XmProcessLock();
	del = ((XmBulletinBoardWidgetClass)xmBulletinBoardWidgetClass)->composite_class.delete_child;
	_XmProcessUnlock();

	(*del)(child);
}

static Boolean NoGeoRequest(XmGeoMatrix geo)
{
	return BB_InSetValues(geo->composite) &&
	       XtClass(geo->composite) == xmFontDialogWidgetClass;
}

/**
 * Align our list labels with our list boxes
 */
static void fix_labels(XmGeoMatrix geo, int action,
                       XmGeoMajorLayout layout, XmKidGeometry row)
{
	int i;

	(void)layout;
	if (row && action == XmGEO_PRE_SET) {
		for (i = 0; i < 3; i++)
			row[i].box.x = row[4 + i].box.x;
	}
}

/**
 * Create our geometry boxes row by row, and figure our layout.
 */
static XmGeoMatrix GeoMatrixCreate(Widget w, Widget inst, XtWidgetGeometry *desired)
{
	XmGeoMatrix geo;
	XmGeoRowLayout layout;
	XmKidGeometry box;
	XmFontDialogWidget fd = (XmFontDialogWidget)w;

	geo = _XmGeoMatrixAlloc(5, fd->composite.num_children, 0);
	geo->composite      = w;
	geo->instigator     = inst;
	geo->no_geo_request = NoGeoRequest;
	geo->margin_w       = BB_MarginWidth(fd)  + fd->manager.shadow_thickness;
	geo->margin_h       = BB_MarginHeight(fd) + fd->manager.shadow_thickness;
	if (desired) geo->instig_request = *desired;

	layout = &geo->layouts->row;
	box    = geo->boxes;

	/* Labels */
	if (LayoutIsRtoLM(fd)) {
		if (_XmGeoSetupKid(box, fd->fontdlg.size_label))  ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.style_label)) ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.font_label))  ++box;
	} else {
		if (_XmGeoSetupKid(box, fd->fontdlg.font_label))  ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.style_label)) ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.size_label))  ++box;
	}

	layout->fix_up        = fix_labels;
	layout->fill_mode     = XmGEO_CENTER;
	layout->fit_mode      = XmGEO_PACK;
	layout->space_between = BB_MarginWidth(fd);
	layout->space_above   = BB_MarginHeight(fd);

	/**
	 * Move to the next row by leaving an empty XmKidGeometry and
	 * advancing the layout.
	 */
	++box;
	++layout;

	/* Lists */
	if (LayoutIsRtoLM(fd)) {
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.size)))  ++box;
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.style))) ++box;
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.font)))  ++box;
	} else {
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.font)))  ++box;
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.style))) ++box;
		if (_XmGeoSetupKid(box, XtParent(fd->fontdlg.size)))  ++box;
	}

	layout->fit_mode      = XmGEO_PACK;
	layout->even_height   = True;
	layout->space_between = BB_MarginWidth(fd);
	++box;
	++layout;

	/* Sample frame */
	if (_XmGeoSetupKid(box, fd->fontdlg.sample_frame)) {
		layout->space_above = BB_MarginHeight(fd);
		layout->fill_width  = True;
		box += 2;
		++layout;
	}

	/* Separator */
	if (_XmGeoSetupKid(box, fd->fontdlg.separator)) {
		layout->fix_up      = _XmSeparatorFix;
		layout->space_above = BB_MarginHeight(fd);
		box += 2;
		++layout;
	}

	/* Buttons */
	if (LayoutIsRtoLM(fd)) {
		if (_XmGeoSetupKid(box, fd->fontdlg.cancel_button)) ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.ok_button))     ++box;
	} else {
		if (_XmGeoSetupKid(box, fd->fontdlg.ok_button))     ++box;
		if (_XmGeoSetupKid(box, fd->fontdlg.cancel_button)) ++box;
	}

	layout->fill_mode     = XmGEO_CENTER;
	layout->fit_mode      = XmGEO_WRAP;
	layout->even_width    = True;
	layout->even_height   = True;
	layout->space_above   = BB_MarginHeight(fd);
	(++layout)->end       = True;
	return geo;
}

#if USE_XFT
/**
 * Load a Xft font. The rendition props will get filled on load.
 */
static XmRendition load_xft(Widget w, const String family, String style)
{
	Arg args[4];
	XmRendition r;

	r = XmRenditionCreate(w, XmS, NULL, 0);
	XtSetArg(args[0], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(args[1], XmNfontName, family);
	XtSetArg(args[2], XmNloadModel, XmLOAD_LAZY);
	if (style) XtSetArg(args[3], XmNfontStyle, style);
	XmRenditionUpdate(r, args, 3 + !!style);
	return r;
}
#endif

/**
 * Load a XFontSet
 */
static XmRendition load_fontset(Widget w, const String xlfd)
{
	Arg args[3];
	XmRendition r;

	r = XmRenditionCreate(w, XmS, NULL, 0);
	XtSetArg(args[0], XmNfontType, XmFONT_IS_FONTSET);
	XtSetArg(args[1], XmNfontName, xlfd);
	XtSetArg(args[2], XmNloadModel, XmLOAD_LAZY);
	XmRenditionUpdate(r, args, 3);
	return r;
}

/**
 * Compare two XmStrings, assuming that they're both latin-ish.
 *
 * TODO: Revisit this when we have proper collation around XmString.
 */
static int cmp(const XmString a, const XmString b, Boolean num)
{
	int result;
	String x, y;

	if (a == b)   return 0;
	if (!a && b)  return -1;
	if (b  && !a) return 1;

	x = XmStringUngenerate(a, NULL, XmCHARSET_TEXT, XmUTF8_TEXT);
	y = XmStringUngenerate(b, NULL, XmCHARSET_TEXT, XmUTF8_TEXT);
	result = num ? atoi(x) - atoi(y) : strcmp(x, y);
	XtFree((XtPointer)x);
	XtFree((XtPointer)y);
	return result;
}

/**
 * Binary insertion sort
 *
 * This is perfect for the style / size lists as they're either small or
 * tend to be nearly sorted. This may be a little slow for the font list
 * if the user has a ton of fonts installed.
 *
 * If num is True, we assume this is a list of integers in string form
 * and compare accordingly.
 */
static Cardinal insert(XmStringTable a, Cardinal len, XmString item, Boolean num)
{
	Cardinal lo, mid, hi;

	/* Fast path for tail append */
	assert(a && item);
	if (!len || cmp(a[len - 1], item, num) <= 0) {
		a[len] = item;
		return len;
	}

	/* Find our place in the list */
	lo = 0;
	hi = len;
	while (hi > lo) {
		mid = lo + ((hi - lo) >> 1);
		if (cmp(a[mid], item, num) <= 0) lo = mid + 1;
		else hi = mid;
	}

	memmove(a + lo + 1, a + lo, (len - lo) * sizeof(XmString));
	a[lo] = item;
	return lo;
}

/**
 * Append a font property leaf to our tree
 */
static void append_leaf(Widget w, struct font_prop *node, XmString k,
                        const String family, const String style, Boolean xft)
{
	Cardinal pos;
	XmRendition r;

	assert(!node->children);
	node->list = (XmStringTable)XtRealloc(
		(XtPointer)node->list,
		(node->cnt + 1) * sizeof(XmString)
	);

	node->rend = (XmRendition *)XtRealloc(
		(XtPointer)node->rend,
		(node->cnt + 1) * sizeof(XmRendition)
	);

#if USE_XFT
	if (xft) r = load_xft(w, family, style);
	else
#endif
	{
		r = load_fontset(w, family);
	}

	pos = insert(node->list, node->cnt, k, True);
	if (pos < node->cnt)
		memmove(node->rend + pos + 1, node->rend + pos, (node->cnt - pos) * sizeof(XmRendition));
	node->rend[pos] = r;
	_XmAddHashEntry(node->ht, (XmHashKey)k, (XtPointer)True);
	node->cnt++;
}

/**
 * Append a font property (family/style/size) node to our tree
 */
static struct font_prop *append_prop(struct font_prop *node, XmString k)
{
	Cardinal pos;
	struct font_prop *child = NULL;

	node->list = (XmStringTable)XtRealloc(
		(XtPointer)node->list,
		(node->cnt + 1) * sizeof(XmString)
	);

	child = (struct font_prop *)XtCalloc(1, sizeof *child);
	node->children = (struct font_prop **)XtRealloc(
		(XtPointer)node->children,
		(node->cnt + 1) * sizeof *node->children
	);

	child->ht = _XmAllocHashTable(32, XmHashCompareXmStringLower, XmHashXmStringLower);
	pos       = insert(node->list, node->cnt, k, False);

	if (pos < node->cnt)
		memmove(node->children + pos + 1, node->children + pos, (node->cnt - pos) * sizeof *node->children);
	node->children[pos] = child;
	_XmAddHashEntry(node->ht, (XmHashKey)k, child);
	node->cnt++;
	return child;
}

static void font_prop_destroy(struct font_prop *p)
{
	Cardinal i;

	for (i = 0; i < p->cnt; i++)
		XmStringFree(p->list[i]);

	if (p->rend) {
		for (i = 0; i < p->cnt; i++)
			XmRenditionFree(p->rend[i]);
	}

	if (p->children) {
		for (i = 0; i < p->cnt; i++)
			font_prop_destroy(p->children[i]);
	}

	_XmFreeHashTable(p->ht);
	XtFree((XtPointer)p->rend);
	XtFree((XtPointer)p->children);
	XtFree((XtPointer)p->list);
	XtFree((XtPointer)p);
}

#if USE_XFT
/**
 * Load fonts and styles from fontconfig, appending them to our font info
 * struct.
 */
static void load_fontconfig(Widget w, struct font_prop *info, int sz)
{
	int i, j, l, bsz;
	double psz;
	Cardinal k;
	String tmp;
	XmString q, x;
	FcFontSet *fs, *fsi, *fss;
	FcPattern *pattern, *p2;
	FcObjectSet *os, *os2;
	FcChar8 *s, *s2;
	FcBool scalable;
	struct font_prop *style, *size;

	if (!(os = FcObjectSetBuild(FC_FAMILY, NULL)))
		return;

	if (!(pattern = FcPatternCreate())) {
		FcObjectSetDestroy(os);
		return;
	}

	/* Enumerate families */
	fs = FcFontList(NULL, pattern, os);
	FcObjectSetDestroy(os);
	FcPatternDestroy(pattern);
	if (!fs || !fs->nfont)
		return;

	os = FcObjectSetBuild(FC_STYLE, FC_FAMILY, FC_SCALABLE, NULL);
	for (i = 0; i < fs->nfont; i++) {
		if (FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &s) != FcResultMatch)
			continue;

		/* Enumerate styles */
		pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, s, NULL);
		fsi     = FcFontList(NULL, pattern, os);
		if (!fsi) {
			FcPatternDestroy(pattern);
			continue;
		}

		if (!fsi->nfont) {
			FcFontSetDestroy(fsi);
			FcPatternDestroy(pattern);
			continue;
		}

		/* Prepare our family string */
		q = XmStringCreate((String)s, (XmStringTag)"UTF-8");
		x = XmStringNormalize(q, XM_CODEPOINT_NORM_NFC);
		XmStringFree(q);

		if (!(style = _XmGetHashEntry(info->ht, (XmHashKey)x))) {
			style = append_prop(info, x);
		} else {
			XmStringFree(x);
			FcFontSetDestroy(fsi);
			FcPatternDestroy(pattern);
			continue;
		}

		/* Take the first style in the list */
		for (j = 0; j < fsi->nfont; j++) {
			if (FcPatternGetString(fsi->fonts[j], FC_STYLE, 0, &s2) != FcResultMatch)
				continue;

			/* If we ain't got it, add it */
			q = XmStringCreate((String)s2, (XmStringTag)"UTF-8");
			x = XmStringNormalize(q, XM_CODEPOINT_NORM_NFC);
			XmStringFree(q);
			if (_XmGetHashEntry(style->ht, (XmHashKey)x)) {
				XmStringFree(x);
				continue;
			}

			/* For scalable, use the default sizes */
			scalable = True;
			size     = append_prop(style, x);
			FcPatternGetBool(fsi->fonts[j], FC_SCALABLE, 0, &scalable);

			if (scalable == FcTrue) {
				for (k = 0; k < XtNumber(default_sizes); k++) {
					bsz = strlen((String)s) + strlen(default_sizes[k]) + 7;
					tmp = XtMalloc(bsz);
					snprintf(tmp, bsz, "%s:size=%s", s, default_sizes[k]);
					q = XmStringCreate((String)default_sizes[k], (XmStringTag)"UTF-8");
					append_leaf(w, size, q, tmp, (String)s2, True);
					XtFree(tmp);
				}
			} else {
				/**
				 * This is probably a bitmap font, so FC_SIZE will be
				 * unreliable. Some bitmap fonts get wrapped in such a
				 * way that only the pixelsize is correctly reported.
				 * We'll assume 100 DPI, since fontconfig doesn't tell
				 * us what the  DPI value is for the underlying font.
				 */
				os2 = FcObjectSetBuild(FC_PIXEL_SIZE, NULL);
				p2  = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, s, FC_STYLE, FcTypeString, s2, NULL);
				fss = FcFontList(NULL, pattern, os2);
				for (l = 0; l < fss->nfont; l++) {
					FcPatternGetDouble(fss->fonts[l], FC_PIXEL_SIZE, 0, &psz);

					/* Pixel to Point size */
					tmp = XtMalloc(3);
					snprintf(tmp, 3, "%d", (int)(psz * 72. / 100.));
					q = XmStringCreate(tmp, (XmStringTag)"UTF-8");
					XtFree(tmp);

					if (_XmGetHashEntry(size->ht, (XmHashKey)q))
						continue;

					/* ... and the pattern */
					bsz = strlen((String)s) + 15;
					tmp = XtMalloc(bsz);
					snprintf(tmp, bsz, "%s:pixelsize=%d", s, (int)psz);
					append_leaf(w, size, q, tmp, (String)s2, True);
					XtFree(tmp);
				}

				FcFontSetDestroy(fss);
				FcPatternDestroy(p2);
				FcObjectSetDestroy(os2);
			}
		}

		FcFontSetDestroy(fsi);
		FcPatternDestroy(pattern);
	}

	FcObjectSetDestroy(os);
	FcFontSetDestroy(fs);
}
#endif /* USE_XFT */

/**
 * Ignore X errors, in case of BadAtom
 */
static int ignore_x_errors(Display *disp, XErrorEvent *event)
{
	(void)disp;
	(void)event;
    return 0;
}

/**
 * Marshal X fonts into our info struct
 *
 * This can be quite an expensive operation.
 */
static void load_corefonts(Widget w, struct font_prop *info, int sz)
{
	int i, j, bsz, points, res, count = 0, dash[8];
	char **names       = NULL;
	XFontStruct *finfo = NULL;
	struct font_prop *style, *size;
	Atom FOUNDRY, FAMILY_NAME, WEIGHT_NAME, SLANT, SETWIDTH_NAME;
	Atom POINT_SIZE, RESOLUTION_X;
	String foundry, family, weight, slant, setwidth, tmp;
	XErrorHandler olderr;
	XmString q;
	Boolean use_weight, use_slant;
	XmFontDialogWidget fd;
	Display *d = XtDisplay(w);
	static const char *empty = "";

	fd = (XmFontDialogWidget)w;
	FOUNDRY       = XInternAtom(d, "FOUNDRY", False);
	FAMILY_NAME   = XInternAtom(d, "FAMILY_NAME", False);
	WEIGHT_NAME   = XInternAtom(d, "WEIGHT_NAME", False);
	SLANT         = XInternAtom(d, "SLANT", False);
	SETWIDTH_NAME = XInternAtom(d, "SETWIDTH_NAME", False);
	POINT_SIZE    = XInternAtom(d, "POINT_SIZE", False);
	RESOLUTION_X  = XInternAtom(d, "RESOLUTION_X", False);

	/**
	 * 10k is intentional. According to the xlsfont source, even
	 * Solaris 8 had more than 9k.
	 */
	names = XListFontsWithInfo(d, "*", 10000, &count, &finfo);
	if (!names || !count)
		return;

	/* Just in case we get some bad atoms... */
	olderr = XSetErrorHandler(ignore_x_errors);

	for (i = 0; i < count; i++) {
		foundry  = NULL;
		family   = NULL;
		weight   = NULL;
		slant    = NULL;
		setwidth = NULL;
		points   = 0;
		res      = 0;

		/* Grab our properties (strings should be XPCS) */
		for (j = 0; j < finfo[i].n_properties; j++) {
			if (finfo[i].properties[j].name == FOUNDRY)
				foundry = XGetAtomName(d, finfo[i].properties[j].card32);
			if (finfo[i].properties[j].name == FAMILY_NAME)
				family = XGetAtomName(d, finfo[i].properties[j].card32);
			if (finfo[i].properties[j].name == WEIGHT_NAME)
				weight = XGetAtomName(d, finfo[i].properties[j].card32);
			if (finfo[i].properties[j].name == SLANT)
				slant = XGetAtomName(d, finfo[i].properties[j].card32);
			if (finfo[i].properties[j].name == SETWIDTH_NAME)
				setwidth = XGetAtomName(d, finfo[i].properties[j].card32);
			if (finfo[i].properties[j].name == POINT_SIZE)
				points = finfo[i].properties[j].card32 / 10;
			if (finfo[i].properties[j].name == RESOLUTION_X)
				res = finfo[i].properties[j].card32;
		}

		/* Ignore outline fonts / familyless fonts / nil family */
		if (!points || !foundry || !family || !strcmp(family + 1, "il"))
			goto next;

		/* Ignore 75 DPI fonts if requested */
		if (res < 96 && fd->fontdlg.prefer_100dpi)
			goto next;

		/* Remove the pixel size, etc. */
		memset(dash, 0, sizeof dash);
		for (tmp = names[i] + 1; *tmp; tmp++) {
			if (*tmp == '-') {
				dash[0] = dash[1];
				dash[1] = dash[2];
				dash[2] = dash[3];
				dash[3] = dash[4];
				dash[4] = dash[5];
				dash[5] = dash[6];
				dash[6] = dash[7];
				dash[7] = (int)(tmp - names[i]);
			}
		}

		/* Ignore proportional fonts */
		if (!memcmp(names[i] + dash[0], "-0-0-", 5))
			goto next;
		sprintf(names[i] + dash[0] + 1, "*-%d-%d-*", points * 10, res);

		/* See if we have this family already */
		bsz = (foundry ? strlen(foundry) : 0) +
		      (family  ? strlen(family)  : 0) + 2;
		tmp = XtMalloc(bsz);
		snprintf(tmp, bsz, "%s %s", foundry, family);
		q = XmStringCreate(tmp, XmFONTLIST_DEFAULT_TAG);
		XtFree(tmp);

		if (!(style = _XmGetHashEntry(info->ht, (XmHashKey)q)))
			style = append_prop(info, q);
		else XmStringFree(q);

		/* Compose style string (ex: "Regular" / "Bold Italic") */
		if (slant && (*slant == 'O' || *slant == 'o' || *slant == 'I' || *slant == 'i')) {
			XFree(slant);
			slant = malloc(7);
			memcpy(slant, "Italic", 7);
		}

		/* We don't want "Bold Italic Regular" */
		if ((weight && (*weight == 'b' || *weight == 'B')) ||
		    (slant  && *slant == 'I')) {
			if (!setwidth) setwidth = malloc(1);
			*setwidth = '\0';
		}

		if (!setwidth || (setwidth && (*setwidth == 'N' || *setwidth == 'n'))) {
			XFree(setwidth);
			setwidth = malloc(8);
			memcpy(setwidth, "Regular", 8);
		}

		use_weight = weight && (*weight != 'M' && *weight != 'm' &&
		                        *weight != 'r' && *weight != 'R');
		use_slant  = slant  && (*slant  != 'R' && *slant  != 'r');
		bsz        = (use_weight ? strlen(weight) + 1 : 0) +
		             (use_slant  ? strlen(slant)  + 1 : 0) +
		             strlen(setwidth);

		tmp = XtMalloc(bsz + 1);
		if (bsz > 1) {
			snprintf(tmp, bsz + 1, "%s%s%s%s%s",
			              use_weight ? weight : empty,
			              use_weight ? " "    : empty,
			              use_slant  ? slant  : empty,
			              use_slant  ? " "    : empty,
			              setwidth);
		}
		q = XmStringCreate(tmp, XmFONTLIST_DEFAULT_TAG);
		XtFree(tmp);

		/* See if we have this style already */
		if (!(size = _XmGetHashEntry(style->ht, (XmHashKey)q)))
			size = append_prop(style, q);
		else XmStringFree(q);

		/* Add this size if we don't have it */
		tmp = XtMalloc(3);
		snprintf(tmp, 3, "%d", points);
		q = XmStringCreate(tmp, XmFONTLIST_DEFAULT_TAG);
		XtFree(tmp);

		if (!_XmGetHashEntry(size->ht, (XmHashKey)q))
			append_leaf(w, size, q, names[i], NULL, False);
		else XmStringFree(q);

next:
		XFree(foundry);
		XFree(family);
		XFree(weight);
		XFree(slant);
		XFree(setwidth);
	}

	XSetErrorHandler(olderr);
	XFreeFontInfo(names, finfo, count);
}

/**
 * List callback cascade machinery
 */
static void update_sample(XmFontDialogWidget fd)
{
	Arg args[6];
	XmRendition r;
	XmString s = NULL;
	struct font_prop *info, *font, *style;

	if (!fd || !(info = (struct font_prop *)fd->fontdlg.info))
		return;

	if (fd->fontdlg.sample) {
		XtDestroyWidget(fd->fontdlg.sample);
		fd->fontdlg.sample = NULL;
	}

	font  = info->children[fd->fontdlg.selected_font  - 1];
	style = font->children[fd->fontdlg.selected_style - 1];
	r     = style->rend[fd->fontdlg.selected_size     - 1];

	if (fd->fontdlg.rend) {
		XmRenditionDematerialize(*_XmRTRenditions(fd->fontdlg.rend));
		*_XmRTRenditions(fd->fontdlg.rend) = r;
	} else {
		fd->fontdlg.rend = XmRenderTableAddRenditions(NULL, &r, 1, 0);
		_XmRendRefcountDec(r);
	}

	/**
	 * Sample label
	 */
	XtSetArg(args[0], XmNtraversalOn,  False);
	XtSetArg(args[1], XmNmarginTop,    5);
	XtSetArg(args[2], XmNmarginBottom, 5);
	XtSetArg(args[3], XmNmarginLeft,   5);
	XtSetArg(args[4], XmNmarginRight,  5);
	XtSetArg(args[5], XmNalignment,    XmALIGNMENT_CENTER);
	fd->fontdlg.sample = XmCreateLabelGadget(
		fd->fontdlg.sample_frame, "Sample", args, 6
	);

	if (!XmRenditionMaterialize(r)) {
		s = XmStringCreate(
			(String)"Failed to load the requested font",
			(XmStringTag)"UTF-8"
		);
		XtSetArg(args[0], XmNlabelString, s);
		XtSetValues(fd->fontdlg.sample, args, 1);
	} else {
		XtSetArg(args[0], XmNlabelString, fd->fontdlg.sample_text);
		XtSetArg(args[1], XmNrenderTable, fd->fontdlg.rend);
		XtSetValues(fd->fontdlg.sample, args, 2);
	}
	XtManageChild(fd->fontdlg.sample);
	XmStringFree(s);

	/* Ensure the frame redraws */
	if (XtIsRealized(fd->fontdlg.sample_frame)) {
		XClearArea(XtDisplay(fd), XtWindow(fd->fontdlg.sample_frame),
		           0, 0, 0, 0, True);
	}
}

static void font_select(Widget w, XtPointer client, XtPointer call)
{
	Arg arg[3];
	int *pos_list = NULL;
	struct font_prop *info, *font, *style;
	XmListCallbackStruct *cb = (XmListCallbackStruct *)call;
	XmFontDialogWidget    fd = (XmFontDialogWidget)client;

	if (!fd || !(info = (struct font_prop *)fd->fontdlg.info))
		return;

	fd->fontdlg.selected_font = cb->item_position;
	font = info->children[fd->fontdlg.selected_font - 1];

	/**
	 * Update the style and size lists.
	 *
	 * The List widget will reselect the previously selected string
	 * if it's present in the new set of items. Otherwise, we'll select
	 * the first item in the list.
	 */
	XtSetArg(arg[0], XmNitems,     font->list);
	XtSetArg(arg[1], XmNitemCount, font->cnt);
	XtSetArg(arg[2], XmNvisibleItemCount, fd->fontdlg.visible_item_count);
	XtSetValues(fd->fontdlg.style, arg, 3);
	XtVaGetValues(fd->fontdlg.style, XmNselectedPositions, &pos_list, NULL);

	if (pos_list)
		fd->fontdlg.selected_style = *pos_list;
	else {
		fd->fontdlg.selected_style = 1;
		XmListSelectPos(fd->fontdlg.style, 1, False);
	}

	style = font->children[fd->fontdlg.selected_style - 1];
	XtSetArg(arg[0], XmNitems,     style->list);
	XtSetArg(arg[1], XmNitemCount, style->cnt);
	XtSetArg(arg[2], XmNvisibleItemCount, fd->fontdlg.visible_item_count);
	XtSetValues(fd->fontdlg.size, arg, 3);
	XtVaGetValues(fd->fontdlg.size, XmNselectedPositions, &pos_list, NULL);

	if (pos_list)
		fd->fontdlg.selected_size = *pos_list;
	else {
		fd->fontdlg.selected_size = 1;
		XmListSelectPos(fd->fontdlg.size, 1, False);
	}

	update_sample(fd);
}

static void style_select(Widget w, XtPointer client, XtPointer call)
{
	Arg arg[3];
	Boolean in_sv;
	int *pos_list = NULL;
	struct font_prop *info, *font, *style;
	XmListCallbackStruct *cb = (XmListCallbackStruct *)call;
	XmFontDialogWidget    fd = (XmFontDialogWidget)client;

	if (!fd || !(info = (struct font_prop *)fd->fontdlg.info))
		return;

	fd->fontdlg.selected_style = cb->item_position;
	font  = info->children[fd->fontdlg.selected_font - 1];
	style = font->children[fd->fontdlg.selected_style - 1];

	XtSetArg(arg[0], XmNitems,     style->list);
	XtSetArg(arg[1], XmNitemCount, style->cnt);
	XtSetArg(arg[2], XmNvisibleItemCount, fd->fontdlg.visible_item_count);
	XtSetValues(fd->fontdlg.size, arg, 3);
	XtVaGetValues(fd->fontdlg.size, XmNselectedPositions, &pos_list, NULL);

	if (pos_list)
		fd->fontdlg.selected_size = *pos_list;
	else {
		fd->fontdlg.selected_size = 1;
		XmListSelectPos(fd->fontdlg.size, 1, False);
	}

	update_sample(fd);
}

static void size_select(Widget w, XtPointer client, XtPointer call)
{
	XmListCallbackStruct *cb = (XmListCallbackStruct *)call;
	XmFontDialogWidget    fd = (XmFontDialogWidget)client;

	if (!fd)
		return;

	fd->fontdlg.selected_size = cb->item_position;
	update_sample(fd);
}

/**
 * Button event
 *
 * Return a clone of the selected rendition when the OK button is pressed,
 * nothing on Cancel. The returned rendition is setup for deferred loading,
 * meaning that it will be loaded automatically on its first use.
 */
static void button_proc(Widget w, XtPointer client, XtPointer call)
{
	XmFontDialogWidget fd;
	XmFontDialogCallbackStruct fd_cb;
	XmRendition r;
	struct font_prop *info, *font, *style;
	XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *)call;

	fd = (XmFontDialogWidget)XtParent(w);
	if (!fd || !(info = (struct font_prop *)fd->fontdlg.info))
		return;

	memset(&fd_cb, 0, sizeof fd_cb);
	fd_cb.event = cb->event;

	switch ((intptr_t)client) {
	case XmDIALOG_OK_BUTTON:
		if (!fd->fontdlg.ok_callback)
			break;

		font  = info->children[fd->fontdlg.selected_font  - 1];
		style = font->children[fd->fontdlg.selected_style - 1];
		r     = style->rend[fd->fontdlg.selected_size     - 1];
		r     = _XmRenditionCopy(r, False);

		/**
		 * _XmRenditionCopy() / CopyInto() refs the Xft font, but doesn't
		 * generate new copies of X fonts.
		 */
#if USE_XFT
		if (_XmRendXftFont(r))
			XftFontClose(_XmRendDisplay(r), _XmRendXftFont(r));
		_XmRendXftFont(r) = NULL;
		_XmRendFont(r)    = NULL;
#endif
		if (XmRenditionMaterialize(r))
			_XmRendLoadModel(r) = XmLOAD_IMMEDIATE;
		else _XmRendLoadModel(r) = XmLOAD_DEFERRED;
		_XmRendRefcount(r) = 1;

		fd_cb.reason    = XmCR_OK;
		fd_cb.rendition = r;
		XtCallCallbackList((Widget)fd, fd->fontdlg.ok_callback, &fd_cb);
		break;
	case XmDIALOG_CANCEL_BUTTON:
		if (!fd->fontdlg.cancel_callback)
			break;

		fd_cb.reason = XmCR_CANCEL;
		XtCallCallbackList((Widget)fd, fd->fontdlg.cancel_callback, &fd_cb);
		break;
	}

	if (fd->bulletin_board.shell && fd->bulletin_board.auto_unmanage)
		XtUnmanageChild((Widget)fd);
}

/**
 * Widget creation routines
 */
Widget XmCreateFontDialog(Widget parent, char *name, ArgList args, Cardinal cnt)
{
	Widget w;
	ArgList new;

	new = (ArgList)XtMalloc((cnt + 1) * sizeof(Arg));
	if (cnt) memcpy(new, args, cnt * sizeof(Arg));
	XtSetArg(new[cnt], XmNdialogType, XmDIALOG_FONT);
	w = XmeCreateClassDialog(xmFontDialogWidgetClass, parent, name, new, cnt + 1);
	XtFree((XtPointer)new);
	return w;
}

Widget XmVaCreateFontDialog(Widget parent, char *name, ...)
{
	Widget ret;
	int cnt;
	va_list lst;

	va_start(lst, name);
	cnt = XmeCountVaListSimple(lst);
	va_end(lst);

	va_start(lst, name);
	ret = XmeVLCreateWidget(name, xmFontDialogWidgetClass, parent, False,
	                        lst, cnt);
	va_end(lst);
	return ret;
}

Widget XmVaCreateManagedFontDialog(Widget parent, char *name, ...)
{
	Widget ret;
	int cnt;
	va_list lst;

	va_start(lst, name);
	cnt = XmeCountVaListSimple(lst);
	va_end(lst);

	va_start(lst, name);
	ret = XmeVLCreateWidget(name, xmFontDialogWidgetClass, parent, True,
	                        lst, cnt);
	va_end(lst);
	return ret;
}

