/* $TOG: IconG.c /main/28 1999/01/26 15:30:46 mgreess $ */
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

#include <Xm/AccColorT.h>
#include <Xm/CareVisualT.h>
#include <Xm/ContItemT.h>
#include <Xm/ContainerT.h>
#include <Xm/PointInT.h>
#include <Xm/DrawP.h>
#include <Xm/IconGP.h>
#include <Xm/TraitP.h>		/* for XmeTraitSet */
#include <Xm/XmosP.h>
#include <Xm/VaSimpleP.h>
#include "BaseClassI.h"
#include "CacheI.h"
#include "ColorI.h"
#include "DrawI.h"
#include "ExtObjectI.h"
#include "ImageCachI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "SyntheticI.h"
#include "TravActI.h"
#include "XmI.h"
#include "XmTabListI.h"
#include "XmosI.h"
#include "IconGI.h"
#include <Xm/XmP.h>


/* spacing between the line and the detail string.
   Those could be moved as plain resources later. */
#define DEFAULT_LABEL_MARGIN_WIDTH	2
#define DEFAULT_LABEL_MARGIN_HEIGHT	2
#define DEFAULT_HOR_SPACING		4

/* This macro should probably be put in XmI.h and used everywhere */
#define PIXMAP_VALID(pix) \
    (pix != XmUNSPECIFIED_PIXMAP && pix != None)

#define SHOW_DETAIL(wid, container_data) \
    (IG_Detail(wid) && IG_DetailCount(wid) && \
     (container_data)->detail_order_count)

#define INVALID_DIMENSION ((Dimension)-1)

/********    Static Function Declarations    ********/
       /* XmRCallProcs */
static void 			CheckSetRenderTable(
					Widget 		wid,
					int		offset,
					XrmValue 	*value);
       /* Converters */
static Boolean CvtStringToIconPixmap(
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal,
                        XtPointer *closure_ret) ;

        /* Hooks */
static	void			GetLabelString(
					Widget		wid,
					int		offset,
					XtArgVal	*value);
	/* BaseClass methods */
static void SecondaryObjectCreate(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPrehook(
                        Widget oldParent,
                        Widget refParent,
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPrehook(
                        Widget newParent,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPosthook(
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPosthook(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static int IconGCacheCompare(
                        XtPointer A,
                        XtPointer B) ;
static Cardinal GetIconGClassSecResData(
                        WidgetClass w_class,
                        XmSecondaryResourceData **data_rtn) ;
static XtPointer GetIconGClassSecResBase(
                        Widget widget,
                        XtPointer client_data) ;
	/* RectObj methods */
static	void			ClassInitialize( void );
static	void			ClassPartInitialize(
					WidgetClass	wc);
static	void			Initialize(
					Widget		rw,
					Widget          nw,
					ArgList         args,
					Cardinal        *num_args);
static	void			Destroy(
					Widget  wid);
static	void			Redisplay(
					Widget		wid,
					XEvent		*event,
					Region		region);
static	Boolean			SetValues(
					Widget		cw,
					Widget		rw,
					Widget		nw,
					ArgList		args,
					Cardinal	*num_args);
static	XtGeometryResult	QueryGeometry(
					Widget		wid,
					XtWidgetGeometry *intended,
					XtWidgetGeometry *reply);
	/* XmGadget methods */
static  void 			HighlightBorder(
						Widget w) ;
static  void 			UnhighlightBorder(
						Widget w) ;
static	void			InputDispatch(
					Widget		wid,
					XEvent		*event,
					Mask		event_mask);
static	Boolean			GetBaselines(
					Widget		wid,
					Dimension	**baselines,
					int		*line_count);
static	void			MarginsProc(
					Widget		w,
				        XmBaselineMargins *margins_rec);
static	Boolean			GetDisplayRect(
					Widget		wid,
					XRectangle	*displayrect);
	/* XmIconGadget methods */
	/* Action procs */
	/* Internal functions */
static	void                    GetLabelXY(
					   Widget		wid,
					   Position * x_ret,
					   Position * y_ret);
static	Position                GetLargeIconX(
					      Widget		wid);
static	Position                GetSmallIconY(
					      Widget		wid);
static	Dimension		GetIconLabelWidth(
					Widget		wid);
static	Dimension		GetIconLabelHeight(
					Widget		wid);
static Cardinal                 GetShapeInfo(
					     Widget wid,
					     Position large_icon_x,
					     Position small_icon_y,
					     Position label_x,
					     Position label_y,
					     Dimension first_column_width,
					     Dimension ht,
					     XPoint * points);
static  void                    GetContainerData(
					Widget wid,
					XmContainerData container_data);
static	void			ChangeHighlightGC(
						  Widget	wid,
						  XtEnum selection_mode,
						  int    line_width);
static	void			UpdateSelectGCs(
						Widget		wid,
						Pixel select_color);
static	void			UpdateGCs(
					Widget		wid);
static	XmStringTable           GetStringTableReOrdered(
							XmStringTable st,
							Cardinal st_count,
							Cardinal * order,
							Cardinal order_count);
static	void                    GetStringTableExtent(
						     Screen * screen,
						     XmStringTable st,
						     Cardinal st_count,
						     XmRenderTable render_table,
						     XmTabList tab_list,
						     Dimension hor_spacing,
						     Dimension * width_ret,
						     Dimension * height_ret,
						     Position * baseline_ret);
static	void			GetSize(
					Widget		wid,
					Dimension	*ideal_width,
					Dimension	*ideal_height);

	/* Trait methods */
static	void ContItemSetValues(Widget w,
			       XmContainerItemData contItemData);

static	void ContItemGetValues(Widget w,
			       XmContainerItemData contItemData);

static  Boolean HandleRedraw (Widget kid,
			      Widget cur_parent,
			      Widget new_parent,
			      Mask visual_flag);
static  void GetColors(Widget widget,
		       XmAccessColorData color_data);

static  Boolean PointIn(Widget widget,
		        Position x, Position y);

/********    End Static Function Declarations    ********/



/*** Xt trivia: How do you manage a flag that tells you that something
     happened during a resource conversion ?
     We need a association outside the widget instance for the
     OwnIconMask flag. This is because the converter sets this flag
     and the widget Initialize cannot initialize it afterward without
     overiding it.
     So we use the XContext manager for that.
     (I could probably have used private resource too...)  ***/

/* those are created in ClassInitialize and filled by the
   IconConverter. */
static XContext 	largeIconContext = 0;
static XContext		smallIconContext = 0;

static XPointer dummy;
#define OwnLargeMask(widget) \
        (XFindContext(XtDisplay(widget),  \
		     (Window) widget,   \
		     largeIconContext,  \
		     &dummy) == 0)
#define OwnSmallMask(widget) \
        (XFindContext(XtDisplay(widget),  \
		     (Window) widget,   \
		     smallIconContext,  \
		     &dummy) == 0)


#define MESSAGE0	_XmMMsgPixConv_0000

/***** The resources of this class */
static	XtResource		resources[] =
{
    /* The following hackery is a way in Xt to see if a
       widget has been initialized. We need to know whether or not
       the gadget cache is valid. We use "." in the name so that
       an end user cannot access it from a resource file.
       With that in place, we just need to check IG_Cache(w) != NULL
       to see if the gadget has been initialized. */
    {
	XmNdotCache, XmCDotCache, XmRPointer, sizeof(XtPointer),
	XtOffset(XmIconGadget, icong.cache),
	XtRImmediate, (XtPointer) NULL},
    {
	XmNlabelString,XmCXmString,XmRXmString,sizeof(XmString),
	XtOffset(XmIconGadget,icong.label_string),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNlargeIconMask,XmCIconMask,XmRBitmap,sizeof(Pixmap),
	XtOffset(XmIconGadget,icong.large_icon_mask),
	XmRImmediate,(XtPointer)XmUNSPECIFIED_PIXMAP},
    {
	XmNlargeIconPixmap,XmCIconPixmap,XmRLargeIconPixmap,sizeof(Pixmap),
	XtOffset(XmIconGadget,icong.large_icon_pixmap),
	XmRImmediate,(XtPointer)XmUNSPECIFIED_PIXMAP},
    {
	XmNsmallIconMask,XmCIconMask,XmRBitmap,sizeof(Pixmap),
	XtOffset(XmIconGadget,icong.small_icon_mask),
	XmRImmediate,(XtPointer)XmUNSPECIFIED_PIXMAP},
    {
	XmNsmallIconPixmap,XmCIconMask,XmRSmallIconPixmap,sizeof(Pixmap),
	XtOffset(XmIconGadget,icong.small_icon_pixmap),
	XmRImmediate,(XtPointer)XmUNSPECIFIED_PIXMAP},
    {
	XmNviewType,XmCViewType,XmRViewType,sizeof(unsigned char),
	XtOffset(XmIconGadget,icong.viewtype),
	XmRImmediate,(XtPointer)XmLARGE_ICON},
    {
	XmNvisualEmphasis,XmCVisualEmphasis,XmRVisualEmphasis,
	sizeof(unsigned char),
	XtOffset(XmIconGadget,icong.visual_emphasis),
	XmRImmediate,(XtPointer)XmNOT_SELECTED},
    {
	XmNdetail,XmCDetail,XmRXmStringTable,
	sizeof(XmStringTable),
	XtOffset(XmIconGadget,icong.detail),
	XmRImmediate,(XtPointer)NULL},
    {
	XmNdetailCount,XmCDetailCount,XmRCardinal,sizeof(Cardinal),
	XtOffset(XmIconGadget,icong.detail_count),
	XmRImmediate,(XtPointer)0},
   {
	"pri.vate","Pri.vate",XmRBoolean,
	sizeof(Boolean),
	XtOffset(XmIconGadget, icong.check_set_render_table),
	XmRImmediate, (XtPointer) False},
};


static XtResource cache_resources[] =
{
   {
     XmNfontList, XmCFontList, XmRFontList,
     sizeof(XmRenderTable),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.render_table),
     XmRCallProc,(XtPointer)CheckSetRenderTable
   },
   {
     XmNrenderTable, XmCRenderTable, XmRRenderTable,
     sizeof(XmRenderTable),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.render_table),
     XmRCallProc,(XtPointer)CheckSetRenderTable
   },
   {
     XmNbackground, XmCBackground, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.background),
     XmRCallProc, (XtPointer) _XmBackgroundColorDefault
   },
   {
     XmNforeground, XmCForeground, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.foreground),
     XmRCallProc, (XtPointer) _XmForegroundColorDefault
   },
   {
     XmNtopShadowColor, XmCTopShadowColor, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.top_shadow_color),
     XmRCallProc, (XtPointer) _XmTopShadowColorDefault
   },
   {
     XmNbottomShadowColor, XmCBottomShadowColor, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.bottom_shadow_color),
     XmRCallProc, (XtPointer) _XmBottomShadowColorDefault
   },
   {
     XmNhighlightColor, XmCHighlightColor, XmRPixel,
     sizeof (Pixel),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.highlight_color),
     XmRCallProc, (XtPointer) _XmHighlightColorDefault
   },
   {
     XmNbackgroundPixmap, XmCBackgroundPixmap, XmRPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.background_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },
   {
     XmNtopShadowPixmap, XmCTopShadowPixmap, XmRTopShadowPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.top_shadow_pixmap),
     XmRCallProc, (XtPointer) _XmTopShadowPixmapDefault
   },
   {
     XmNbottomShadowPixmap, XmCBottomShadowPixmap, XmRBottomShadowPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.bottom_shadow_pixmap),
     XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
   },
   {
     XmNhighlightPixmap, XmCHighlightPixmap, XmRHighlightPixmap,
     sizeof (Pixmap),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.highlight_pixmap),
     XmRCallProc, (XtPointer) _XmHighlightPixmapDefault
   },
   {
     XmNalignment, XmCAlignment, XmRAlignment, sizeof(unsigned char),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.alignment),
     XmRImmediate, (XtPointer) XmALIGNMENT_CENTER
   },
   {
     XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.spacing),
     XmRImmediate, (XtPointer) 4
   },
   {
     XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
     sizeof(Dimension),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.margin_width),
     XmRImmediate, (XtPointer) 2
   },
   {
     XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
     sizeof(Dimension),
     XtOffsetOf(XmIconGCacheObjRec, icon_cache.margin_height),
     XmRImmediate, (XtPointer) 2
   },
};

static	XmSyntheticResource	syn_resources[] =
{
    {
	XmNlabelString,sizeof(XmString),
	XtOffset(XmIconGadget,icong.label_string),GetLabelString,NULL
	},
};

static	XmSyntheticResource	cache_syn_resources[] =
{
    {
      XmNspacing, sizeof(Dimension),
      XtOffsetOf(XmIconGCacheObjRec, icon_cache.spacing),
      XmeFromHorizontalPixels, XmeToHorizontalPixels
    },
    {
      XmNmarginWidth, sizeof(Dimension),
      XtOffsetOf(XmIconGCacheObjRec, icon_cache.margin_width),
      XmeFromHorizontalPixels, XmeToHorizontalPixels
    },
    {
      XmNmarginHeight, sizeof(Dimension),
      XtOffsetOf(XmIconGCacheObjRec, icon_cache.margin_height),
      XmeFromVerticalPixels, XmeToVerticalPixels
    },
};


/* ext rec static initialization */
externaldef(xmicongcacheobjclassrec)
XmIconGCacheObjClassRec xmIconGCacheObjClassRec =
{
  {
    /* superclass         */    (WidgetClass) &xmExtClassRec,
    /* class_name         */    "XmIconGadget",
    /* widget_size        */    sizeof(XmIconGCacheObjRec),
    /* class_initialize   */    NULL,
    /* chained class init */    NULL,
    /* class_inited       */    False,
    /* initialize         */    NULL,
    /* initialize hook    */    NULL,
    /* realize            */    NULL,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    cache_resources,
    /* num_resources      */    XtNumber(cache_resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    False,
    /* compress_exposure  */    False,
    /* compress enter/exit*/    False,
    /* visible_interest   */    False,
    /* destroy            */    NULL,
    /* resize             */    NULL,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set values hook    */    NULL,
    /* set values almost  */    NULL,
    /* get values hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback offsetlst */    NULL,
    /* default trans      */    NULL,
    /* query geo proc     */    NULL,
    /* display accelerator*/    NULL,
    /* extension record   */    NULL,
  },

  {
    /* synthetic resources */   cache_syn_resources,
    /* num_syn_resources   */   XtNumber(cache_syn_resources),
    /* extension           */   NULL,
  }
};

static XmBaseClassExtRec   iconGBaseClassExtRec = {
    NULL,    					/* next_extension        */
    NULLQUARK,					/* record_typ            */
    XmBaseClassExtVersion,			/* version               */
    sizeof(XmBaseClassExtRec),			/* record_size           */
    XmInheritInitializePrehook,			/* initializePrehook     */
    SetValuesPrehook,				/* setValuesPrehook      */
    InitializePosthook,				/* initializePosthook    */
    SetValuesPosthook,				/* setValuesPosthook     */
    (WidgetClass)&xmIconGCacheObjClassRec,	/* secondaryObjectClass  */
    SecondaryObjectCreate,		        /* secondaryObjectCreate */
    GetIconGClassSecResData,	                /* getSecResData */
    {0},			                /* Other Flags           */
    GetValuesPrehook,				/* getValuesPrehook      */
    GetValuesPosthook,				/* getValuesPosthook     */
    NULL,                                       /* classPartInitPrehook */
    NULL,                                       /* classPartInitPosthook*/
    NULL,                                       /* ext_resources        */
    NULL,                                       /* compiled_ext_resources*/
    0,                                          /* num_ext_resources    */
    FALSE,                                      /* use_sub_resources    */
    XmInheritWidgetNavigable,                   /* widgetNavigable      */
    XmInheritFocusChange,                       /* focusChange          */
};

static XmCacheClassPart IconGClassCachePart = {
    {NULL, 0, 0},        /* head of class cache list */
    _XmCacheCopy,       /* Copy routine     */
    _XmCacheDelete,     /* Delete routine   */
    IconGCacheCompare,    /* Comparison routine   */
};


static XmGadgetClassExtRec GadClassExtRec = {
    NULL,
    NULLQUARK,
    XmGadgetClassExtVersion,
    sizeof(XmGadgetClassExtRec),
    GetBaselines,			/* widget_baseline */
    GetDisplayRect,			/* widget_display_rect */
    MarginsProc,                        /* widget_margins */
};


externaldef( xmicongadgetclassrec) XmIconGadgetClassRec	xmIconGadgetClassRec =
{	/* RectObjClassPart */
    {
	(WidgetClass) &xmGadgetClassRec, /* superclass		*/
	"XmIconGadget",			/* class_name		*/
	sizeof (XmIconGadgetRec),	/* widget_size		*/
	ClassInitialize,		/* class_initialize	*/
	ClassPartInitialize,		/* class_part_initialize*/
	False,				/* class_inited		*/
	Initialize,			/* initialize		*/
	NULL,				/* initialize_hook	*/
	NULL,				/* realize		*/
	NULL,				/* actions		*/
	0,				/* num_actions		*/
	resources,			/* resources		*/
	XtNumber (resources),		/* num_resources	*/
	NULLQUARK,			/* xrm_class		*/
	True,				/* compress_motion	*/
	True,				/* compress_exposure	*/
	True,				/* compress_enterleave	*/
	False,				/* visible_interest	*/
	Destroy,		 	/* destroy		*/
	NULL,				/* resize		*/
	Redisplay,			/* expose		*/
	SetValues,			/* set_values		*/
	NULL,				/* set_values_hook	*/
	XtInheritSetValuesAlmost,	/* set_values_almost	*/
	NULL,				/* get_values_hook	*/
	NULL,				/* accept_focus		*/
	XtVersion,			/* version		*/
	NULL,				/* callback private	*/
	NULL,				/* tm_table		*/
	QueryGeometry,			/* query_geometry	*/
	NULL,				/* display_accelerator	*/
	(XtPointer)&iconGBaseClassExtRec,	/* extension	*/
    },

    /* XmGadget Class Part */
    {
	HighlightBorder,		        /* border_highlight	*/
	UnhighlightBorder,		        /* border_unhighlight	*/
	NULL,					/* arm_and_activate	*/
	InputDispatch,				/* input_dispatch	*/
	NULL,				        /* visual_change	*/
	syn_resources,				/* get_resources	*/
	XtNumber(syn_resources),		/* num_get_resources	*/
	&IconGClassCachePart,		        /* class_cache_part	*/
	(XtPointer)&GadClassExtRec,	        /* extension		*/
    },

	/* XmIconGadget Class Part */
    {
	NULL,		/* get_container_parent	*/
	NULL,		/* extension	*/
    },
};

externaldef(xmicongadgetclass) WidgetClass
	xmIconGadgetClass=(WidgetClass)&xmIconGadgetClassRec;



/* ContainerItem Trait record for IconGadget */

static const XmContainerItemTraitRec iconCIT = {
  0,		/* version */
  ContItemSetValues,
  ContItemGetValues,
};

/* CareVisual Trait record for IconGadget */

static const XmCareVisualTraitRec iconCVT = {
    0,		/* version */
    HandleRedraw,
};

/* Access Colors Trait record for IconGadget */

static const XmAccessColorsTraitRec iconACT = {
  0,			/* version */
  GetColors
};

/* Point In Trait record for IconGadget */

static const XmPointInTraitRec iconPIT = {
  0,			/* version */
  PointIn,
};

/******** for the special IconPixmap/Mask converter */
static XtConvertArgRec largeIconArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)(int)XmLARGE_ICON, 0}
};

static XtConvertArgRec smallIconArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)(int)XmSMALL_ICON, 0}
};


/*** XmRCallProcs ***/
/*
 * XmRCallProc routine for checking icon_cache.render_table before setting
 * it to NULL. If "check_set_render_table" is True, then function has
 * been called twice on same widget, thus resource needs to be set NULL,
 * otherwise leave it alone.
 */
static void
CheckSetRenderTable(Widget wid,
		    int offset,
		    XrmValue *value)
{
  XmIconGadget ig = (XmIconGadget)wid;

  /* Check if been here before */
  if (ig->icong.check_set_render_table)
      value->addr = NULL;
  else {
      ig->icong.check_set_render_table = True;
      value->addr = (char*)&(IG_RenderTable(ig));
  }
}

/************************************************************************
 *
 *  FetchPixmap
 *
 ************************************************************************/
static void
FetchPixmap(
        Widget widget,
        String image_name,
        unsigned char res_type,
        Pixmap * pixmap)
{
   int depth ;
   XtPointer mask_addr ;
   XmAccessColorDataRec acc_color_rec;
   XmAccessColorsTrait access_colors_trait ;

   depth = - XtParent(widget)->core.depth;
   /* this is the convention used by XmGetPixmapByDepth, pass
      a negative depth means do bitmap if you can */

   /* always called when the cache is valid and the colors
      can be returned */
   access_colors_trait = (XmAccessColorsTrait)
       XmeTraitGet((XtPointer)XtClass(widget), XmQTaccessColors) ;
   access_colors_trait->getColors(widget, &acc_color_rec);

   *pixmap = _XmGetScaledPixmap (XtScreen(widget), widget,
				 image_name, &acc_color_rec,
				 depth, FALSE, 0, 0, 0);

   if (*pixmap == XmUNSPECIFIED_PIXMAP) {
       return ;
   }

   /* now we see if a mask is to be fetched too */

   if (res_type == XmLARGE_ICON) {
       mask_addr = (XtPointer)&(IG_LargeIconMask(widget)) ;
   } else {
       mask_addr = (XtPointer)&(IG_SmallIconMask(widget)) ;
   }

   /* fetch only if a mask has not been specified in the resource slot */
   if (*(Pixmap *)mask_addr == XmUNSPECIFIED_PIXMAP) {
       /* Fetch the mask out of image_name and ask for it.
	  ImageCache:
	  The mask_name returned is the same as the one used by the
	  ImageCache reader to cache the mask associated with an XPM file,
	  so we will get it from the cache if an XPM file with a mask was
	  specified for image_name. When an XPM file with a mask
	  in it is read, the mask is cached with mask_name. */

       *(Pixmap *)mask_addr = XmeGetMask(XtScreen(widget), image_name);
       /* mark that we have to destroy the mask */
       if (*(Pixmap *)mask_addr != XmUNSPECIFIED_PIXMAP) {
	   if (res_type == XmLARGE_ICON) {
	       XSaveContext(XtDisplay(widget),
			    (Window) widget,
			    largeIconContext,
			    (XPointer) True);
	   } else {
	       XSaveContext(XtDisplay(widget),
			    (Window) widget,
			    smallIconContext,
			    (XPointer) True);
	   }
       }
   }
}


 /************************************************************************
 *
 *  CvtStringToIconPixmap
 *
 ************************************************************************/
static Boolean
CvtStringToIconPixmap(
        Display *dpy,
        XrmValue *args,
        Cardinal *numArgs,
        XrmValue *fromVal,
        XrmValue *toVal,
        XtPointer *closure_ret)	/* unused */
{
   Pixmap pixmap = XmUNSPECIFIED_PIXMAP;
   String image_name = (String) (fromVal->addr);
   Widget widget ;
   unsigned char res_type;


   if (*numArgs != 2) {
       XtAppWarningMsg (XtDisplayToApplicationContext(dpy),
			"wrongParameters", "cvtStringToPixmap",
			"XtToolkitError", MESSAGE0,
			(String *) NULL, (Cardinal *)NULL);
       return False;
   }

   /* CR 7568: Set widget before we use it. */
   widget = *((Widget *) args[0].addr);
   /* get back the resource type: LARGE or SMALL_ICON */
   res_type = (unsigned char) (int) (long) args[1].addr;

   if (XmeNamesAreEqual(image_name, "none")) {
       pixmap = None ;
       _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
	     XmDestroyPixmap(XtScreen(widget), pixmap) ;)
   }

   if (XmeNamesAreEqual(image_name, XmSunspecified_pixmap)) {
       pixmap = XmUNSPECIFIED_PIXMAP ;
       _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
	     XmDestroyPixmap(XtScreen(widget), pixmap) ;)
   }

   /* First we have to check if the gadget has been initialized
      before trying to create the pixmap. We need the colors
      and if the cache is not yet created, we have to delay
      the conversion. */
   if (!IG_Cache(widget)) {
       pixmap = XmDELAYED_PIXMAP;
       /* we need to stach away the name of the pixmap resource,
	  since we'll need it in Initialize, when XmDELAYED_PIXMAP is
	  treated, and XtGetSubResources is not going to work for
	  XtVaTypeArg time resource, which are not in the database. */
       if (res_type == XmLARGE_ICON)
	   IG_LargePixmapName(widget) = image_name ;
       else
	   IG_SmallPixmapName(widget) = image_name ;

       _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
	     XmDestroyPixmap(XtScreen(widget), pixmap) ;)
   }

   /* fetch the pixmap */
   FetchPixmap(widget, image_name, res_type, &pixmap);

   if (pixmap == XmUNSPECIFIED_PIXMAP) {
       XtDisplayStringConversionWarning(dpy, image_name,
					"Large/SmallIconPixmap");
       return False;
   }


   _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
		       XmDestroyPixmap(XtScreen(widget), pixmap) ;)
}



/************************************************************************
 * GetLabelString
 ************************************************************************/
static	void
GetLabelString(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
    XmString	string = NULL;

    if (IG_LabelString(wid))
	string = XmStringCopy(IG_LabelString(wid));
    *value = (XtArgVal)string;
}


/************************************************************************
*
*  SecondaryObjectCreate
*
************************************************************************/
static void
SecondaryObjectCreate(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
  XmBaseClassExt              *cePtr;
  XmWidgetExtData             extData;
  WidgetClass                 wc;
  Cardinal                    size;
  XtPointer                   newSec, reqSec;

  cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);

  wc = (*cePtr)->secondaryObjectClass;
  size = wc->core_class.widget_size;

  newSec = _XmExtObjAlloc(size);
  reqSec = _XmExtObjAlloc(size);


/*
 *  Update pointers in instance records now so references to resources
 * in the cache record will be valid for use in CallProcs.
 */

  IG_Cache(new_w) = &(((XmIconGCacheObject)newSec)->icon_cache);
  IG_Cache(req) = &(((XmIconGCacheObject)reqSec)->icon_cache);

    /*
     * fetch the resources in superclass to subclass order
     */
  XtGetSubresources(new_w,
                    newSec,
                    NULL, NULL,
                    wc->core_class.resources,
                    wc->core_class.num_resources,
                    args, *num_args );

  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
  extData->widget = (Widget)newSec;
  extData->reqWidget = (Widget)reqSec;

  ((XmIconGCacheObject)newSec)->ext.extensionType = XmCACHE_EXTENSION;
  ((XmIconGCacheObject)newSec)->ext.logicalParent = new_w;

  _XmPushWidgetExtData(new_w, extData,
                      ((XmIconGCacheObject)newSec)->ext.extensionType);
   memcpy(reqSec, newSec, size);
}


/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/
static void
InitializePosthook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData     ext;
    XmIconGadget sw = (XmIconGadget)new_w;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    _XmProcessLock();
    IG_Cache(sw) = (XmIconGCacheObjPart *)
      _XmCachePart( IG_ClassCachePart(sw),
                    (XtPointer) IG_Cache(sw),
                    sizeof(XmIconGCacheObjPart));

    /*
     * might want to break up into per-class work that gets explicitly
     * chained. For right now, each class has to replicate all
     * superclass logic in hook routine
     */

    /*
     * free the req subobject used for comparisons
     */
    _XmPopWidgetExtData((Widget) sw, &ext, XmCACHE_EXTENSION);
    _XmExtObjFree((XtPointer) ext->widget);
    _XmExtObjFree((XtPointer) ext->reqWidget);
    _XmProcessUnlock();
    XtFree( (char *) ext);

}

/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/
static Boolean
SetValuesPrehook(
        Widget oldParent,
        Widget refParent,
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             extData;
    XmBaseClassExt              *cePtr;
    WidgetClass                 ec;
    XmIconGCacheObject     newSec, reqSec;
    Cardinal                    size;

    _XmProcessLock();
    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;
    newSec = (XmIconGCacheObject)_XmExtObjAlloc(size);
    reqSec = (XmIconGCacheObject)_XmExtObjAlloc(size);
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

    memcpy( &(newSec->icon_cache),
            IG_Cache(newParent),
            sizeof(XmIconGCacheObjPart));

    extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
    extData->widget = (Widget)newSec;
    extData->reqWidget = (Widget)reqSec;
    _XmPushWidgetExtData(newParent, extData, XmCACHE_EXTENSION);

    XtSetSubvalues((XtPointer)newSec,
                   ec->core_class.resources,
                   ec->core_class.num_resources,
                   args, *num_args);

    memcpy((XtPointer)reqSec, (XtPointer)newSec, size);

    IG_Cache(newParent) = &(((XmIconGCacheObject)newSec)->icon_cache);
    IG_Cache(refParent) =
	      &(((XmIconGCacheObject)extData->reqWidget)->icon_cache);

    _XmExtImportArgs((Widget)newSec, args, num_args);

    return FALSE;
}


/************************************************************************
 *
 *  GetValuesPrehook
 *
 ************************************************************************/
static void
GetValuesPrehook(
        Widget newParent,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             extData;
    XmBaseClassExt              *cePtr;
    WidgetClass                 ec;
    XmIconGCacheObject     newSec;
    Cardinal                    size;

    cePtr = _XmGetBaseClassExtPtr(XtClass(newParent), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;
    size = ec->core_class.widget_size;

    _XmProcessLock();
    newSec = (XmIconGCacheObject)_XmExtObjAlloc(size);
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

    memcpy( &(newSec->icon_cache),
            IG_Cache(newParent),
            sizeof(XmIconGCacheObjPart));

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
GetValuesPosthook(
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData ext;

    _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

    _XmProcessLock();
    _XmExtObjFree((XtPointer) ext->widget);
    _XmProcessUnlock();
    XtFree( (char *) ext);
}


/************************************************************************
 *
 *  SetValuesPosthook
 *
 ************************************************************************/
static Boolean
SetValuesPosthook(
        Widget current,
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmWidgetExtData             ext;

    /*
     * - register parts in cache.
     * - update cache pointers
     * - and free req
     */

    /* assign if changed! */
    _XmProcessLock();
    if (!IconGCacheCompare((XtPointer) IG_Cache(new_w),
			  (XtPointer) IG_Cache(current)))
    {
          /* delete the old one */
	_XmCacheDelete( (XtPointer) IG_Cache(current));

	IG_Cache(new_w) = (XmIconGCacheObjPart *)
            _XmCachePart(IG_ClassCachePart(new_w),
                         (XtPointer) IG_Cache(new_w),
                         sizeof(XmIconGCacheObjPart));
     } else
	 IG_Cache(new_w) = IG_Cache(current);

    _XmPopWidgetExtData(new_w, &ext, XmCACHE_EXTENSION);

    _XmExtObjFree((XtPointer) ext->widget);
    _XmExtObjFree((XtPointer) ext->reqWidget);
    _XmProcessUnlock();

    XtFree( (char *) ext);

    return FALSE;
}

/*----------------
| RectObj methods |
----------------*/


/************************************************************************
 * ClassInitialize
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
    iconGBaseClassExtRec.record_type = XmQmotif;

   /* Install the special converters for pixmap/mask */
    XtSetTypeConverter (XmRString, XmRLargeIconPixmap,
			CvtStringToIconPixmap,
			largeIconArgs, XtNumber(largeIconArgs),
			(XtCacheNone | XtCacheRefCount), NULL);

    XtSetTypeConverter (XmRString, XmRSmallIconPixmap,
			CvtStringToIconPixmap,
			smallIconArgs, XtNumber(smallIconArgs),
			(XtCacheNone | XtCacheRefCount), NULL);

    largeIconContext = XUniqueContext();
    smallIconContext = XUniqueContext();
}


/************************************************************************
 * ClassPartInitialize
 *
 ************************************************************************/
static void
ClassPartInitialize(
	WidgetClass	wc)
{
    _XmFastSubclassInit(wc, XmICONGADGET_BIT);

    /* Install the containerItem trait for me and all subclasses */
    XmeTraitSet((XtPointer) wc, XmQTcontainerItem, (XtPointer)&iconCIT);

    /* Install the care Visual trait for me and all subclasses */
    XmeTraitSet((XtPointer) wc, XmQTcareParentVisual, (XtPointer)&iconCVT);

   /* Install the accessColors trait for all subclasses as well. */
    XmeTraitSet((XtPointer) wc, XmQTaccessColors, (XtPointer)&iconACT);

   /* Install the pointIn trait for all subclasses as well. */
    XmeTraitSet((XtPointer) wc, XmQTpointIn, (XtPointer)&iconPIT);
}


/************************************************************************
 * Initialize
 ************************************************************************/
static	void
Initialize(
	Widget		rw,
	Widget		nw,
	ArgList		args,	/* unused */
	Cardinal	*num_args) /* unused */
{
    XmIconGadget	new_ig = (XmIconGadget)nw;
    unsigned int w = 0, h = 0;
    Cardinal i ;

    /* XmNviewType */
    if (!XmRepTypeValidValue(XmRID_VIEW_TYPE,IG_ViewType(nw),nw))
	IG_ViewType(nw) = XmLARGE_ICON;


    /* XmNvisualEmphasis */
    if (!XmRepTypeValidValue(XmRID_VISUAL_EMPHASIS,
			     IG_VisualEmphasis(nw),nw))
	IG_VisualEmphasis(nw) = XmNOT_SELECTED;


    /* XmNalignment */
    if (!XmRepTypeValidValue(XmRID_ALIGNMENT, IG_Alignment(nw), nw))
	IG_Alignment(nw) = XmALIGNMENT_CENTER;


    /* XmNrenderTable */
    if (IG_RenderTable(nw) == NULL) {
	XmRenderTable	defaultRT = NULL;

	XtVaGetValues(XtParent(nw), XmNrenderTable, &defaultRT, NULL);
	if (defaultRT == NULL)
	    defaultRT = XmeGetDefaultRenderTable(nw, XmLABEL_FONTLIST);
	IG_RenderTable(nw) = XmRenderTableCopy(defaultRT, NULL, 0);
    }
    else
	IG_RenderTable(nw) = XmRenderTableCopy(IG_RenderTable(nw), NULL, 0);


    /* XmNlabelString */
    if (!IG_LabelString(nw)) {
	IG_LabelString(nw) =  XmeGetLocalizedString (
				(char *) NULL, nw, XmNlabelString,
				XrmQuarkToString(new_ig->object.xrm_name));
    } else
	IG_LabelString(nw) = XmStringCopy(IG_LabelString(nw));



    /* XmNdetail */
    if (IG_Detail(nw) && IG_DetailCount(nw)) {
	IG_Detail(nw) = (XmStringTable)
	    XtMalloc(IG_DetailCount(nw) * sizeof(XmString));

	for (i=0; i<IG_DetailCount(nw); i++)
	    IG_Detail(nw)[i] = XmStringCopy(IG_Detail(rw)[i]);
    }


    /* get the label size */
    if (!XmStringEmpty(IG_LabelString(nw)))
	XmStringExtent(IG_RenderTable(nw), IG_LabelString(nw),
		       &(IG_LabelRectWidth(nw)),
		       &(IG_LabelRectHeight(nw)));
    else {
	IG_LabelRectWidth(nw) = 0 ;
	IG_LabelRectHeight(nw) = 0 ;
    }
    /* put the margins in the label size, we'll remove them when
       it's time to draw the label */
    IG_LabelRectWidth(nw) += 2*DEFAULT_LABEL_MARGIN_WIDTH ;
    IG_LabelRectHeight(nw) += 2*DEFAULT_LABEL_MARGIN_HEIGHT  ;


    /* before doing anything with the pixmap, check if we have to
       re-ask for a conversion */
    if (IG_LargeIconPixmap(nw) == XmDELAYED_PIXMAP) {
	/* this test means that a conversion was asked for
	   but failed because the colors were not accessible
	   prior to Initialize, because the cache wasn't there yet.
	   We have to try again from here. */
	FetchPixmap(nw, IG_LargePixmapName(nw), XmLARGE_ICON,
		    &(IG_LargeIconPixmap(nw)));
	if (IG_LargeIconPixmap(nw) == XmUNSPECIFIED_PIXMAP) {
	    XtDisplayStringConversionWarning(XtDisplay(nw),
					     IG_LargePixmapName(nw),
					     "Large/SmallIconPixmap");
	}
    }


    if (IG_SmallIconPixmap(nw) == XmDELAYED_PIXMAP) {
	FetchPixmap(nw, IG_SmallPixmapName(nw), XmSMALL_ICON,
		    &(IG_SmallIconPixmap(nw)));
	if (IG_SmallIconPixmap(nw) == XmUNSPECIFIED_PIXMAP) {
	    XtDisplayStringConversionWarning(XtDisplay(nw),
					     IG_SmallPixmapName(nw),
					     "Large/SmallIconPixmap");
	}
    }


    /* get the large icon size */
    if (PIXMAP_VALID(IG_LargeIconPixmap(nw)))
	XmeGetPixmapData(XtScreen(nw),
			 IG_LargeIconPixmap(nw),
			 NULL,
			 NULL,
			 NULL, NULL,
			 NULL, NULL,
			 &w, &h);
    else {
	w = h = 0 ;
    }

    IG_LargeIconRectWidth(nw)  = (Dimension)w;
    IG_LargeIconRectHeight(nw) = (Dimension)h;

    /* get the small icon size */
    if (PIXMAP_VALID(IG_SmallIconPixmap(nw)))
	XmeGetPixmapData(XtScreen(nw),
			 IG_SmallIconPixmap(nw),
			 NULL,
			 NULL,
			 NULL, NULL,
			 NULL, NULL,
			 &w, &h);
    else {
	w = h = 0 ;
    }

    IG_SmallIconRectWidth(nw)  = (Dimension)w;
    IG_SmallIconRectHeight(nw) = (Dimension)h;

    /* turn None in unspecified, since it is the only value
       Container supports */
    if (IG_LargeIconPixmap(nw) == None)
	IG_LargeIconPixmap(nw) = XmUNSPECIFIED_PIXMAP ;
    if (IG_SmallIconPixmap(nw) == None)
	IG_SmallIconPixmap(nw) = XmUNSPECIFIED_PIXMAP ;

    /*****
      we want the mask to act as a max for the size of the
      overall pixmap .
      We need to get the mask sizes first.

    if (IG_LargeMaskWidth(nw))
	IG_LargeIconRectWidth(nw)  = MIN(IG_LargeIconRectWidth(nw),
					 IG_LargeMaskWidth(nw)) ;

    if (IG_LargeMaskHeight(nw))
	IG_LargeIconRectHeight(nw)  = MIN(IG_LargeIconRectHeight(nw),
					  IG_LargeMaskHeight(nw)) ;

    if (IG_SmallMaskWidth(nw))
	IG_SmallIconRectWidth(nw)  = MIN(IG_SmallIconRectWidth(nw),
					 IG_SmallMaskWidth(nw)) ;

    if (IG_SmallMaskHeight(nw))
	IG_SmallIconRectHeight(nw)  = MIN(IG_SmallIconRectHeight(nw),
					  IG_SmallMaskHeight(nw)) ;
	*****/

    /* undo our superclass size check */
    if (((XmGadget)rw)->rectangle.width == 0)
	new_ig->rectangle.width = 0;

    if (((XmGadget)rw)->rectangle.height == 0)
	new_ig->rectangle.height = 0;

    /* if a size has been specified (not 0), it won't be altered
       by the GetSize function */
    GetSize(nw, &new_ig->rectangle.width, &new_ig->rectangle.height);


    IG_NormalGC(nw) = NULL;
    IG_BackgroundGC(nw) = NULL;
    IG_InsensitiveGC(nw) = NULL;
    IG_ShadowGC(nw) = NULL;
    IG_TopShadowGC(nw) = NULL;
    IG_BottomShadowGC(nw) = NULL;
    IG_HighlightGC(nw) = NULL;
    IG_InverseGC(nw) = NULL;
    IG_SelectedGC(nw) = NULL;  /* otherwize UpdateGCs frees them */
    UpdateGCs(nw);


    new_ig->gadget.event_mask =  XmHELP_EVENT |
        XmFOCUS_IN_EVENT | XmFOCUS_OUT_EVENT | XmENTER_EVENT | XmLEAVE_EVENT ;
}


/************************************************************************
 * Destroy
 ************************************************************************/
static	void
Destroy(
	Widget	wid)
{
    Cardinal i ;

    if (IG_RenderTable(wid) != NULL) XmRenderTableFree(IG_RenderTable(wid));

    if (IG_LabelString(wid) != NULL) XmStringFree(IG_LabelString(wid));

    if (IG_Detail(wid) && IG_DetailCount(wid)) {
	for (i=0; i<IG_DetailCount(wid); i++)
	    XmStringFree(IG_Detail(wid)[i]);
	XtFree((char*)IG_Detail(wid));

    }

    if (OwnLargeMask(wid)) {
	if (PIXMAP_VALID(IG_LargeIconMask(wid)) )
	    XmDestroyPixmap(XtScreen(wid),IG_LargeIconMask(wid));
    }

    if (OwnSmallMask(wid)) {
	if (PIXMAP_VALID(IG_SmallIconMask(wid)))
	    XmDestroyPixmap(XtScreen(wid),IG_SmallIconMask(wid));
    }
    /* the IconPixmap is freed by the converter destructor */

    XtReleaseGC(XtParent(wid),IG_NormalGC(wid));
    XtReleaseGC(XtParent(wid),IG_InsensitiveGC(wid));
    XtReleaseGC(XtParent(wid),IG_ShadowGC(wid));
    XtReleaseGC(XtParent(wid),IG_BackgroundGC(wid));
    XtReleaseGC(XtParent(wid),IG_SelectedGC(wid));
    if (IG_InverseGC(wid)) XtReleaseGC(XtParent(wid),IG_InverseGC(wid));
    XtReleaseGC(XtParent(wid),IG_TopShadowGC(wid));
    XtReleaseGC(XtParent(wid),IG_BottomShadowGC(wid));
    XtReleaseGC(XtParent(wid),IG_HighlightGC(wid));

    _XmProcessLock();
    _XmCacheDelete((XtPointer) IG_Cache(wid));
    _XmProcessUnlock();

}



/************************************************************************
 * GetLabelXY
 *  from origin of gadget
 ************************************************************************/
static	void
GetLabelXY(
	Widget		wid,
	Position * x_ret,
        Position * y_ret)
{
    Dimension ist = IG_ShadowThickness(wid);
    Position label_x = ist;
    Position label_y = ist;

#define HAS_PIXMAP(wid) \
    (((IG_ViewType(wid) == XmLARGE_ICON) && \
      (PIXMAP_VALID(IG_LargeIconPixmap(wid)))) || \
     ((IG_ViewType(wid) == XmSMALL_ICON) && \
      (PIXMAP_VALID(IG_SmallIconPixmap(wid)))))

#define SPACING(wid) (HAS_PIXMAP(wid) ? IG_Spacing(wid) : 0)

#define HAS_MASK(wid) \
    (((IG_ViewType(wid) == XmSMALL_ICON) && \
      (PIXMAP_VALID(IG_SmallIconMask(wid)))) ||\
     ((IG_ViewType(wid) == XmLARGE_ICON) && \
      (PIXMAP_VALID(IG_LargeIconMask(wid)))))

    if (IG_ViewType(wid) == XmLARGE_ICON) {

	if ((IG_Alignment(wid) == XmALIGNMENT_CENTER) && !HAS_MASK(wid) &&
	    (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid)))
	    label_x += (IG_LargeIconRectWidth(wid) - IG_LabelRectWidth(wid))/2;
	else if ((IG_Alignment(wid) == XmALIGNMENT_CENTER) && HAS_MASK(wid) &&
		 (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid) + 2*ist))
	    label_x += (IG_LargeIconRectWidth(wid) -
			IG_LabelRectWidth(wid) - 2*ist)/2;
	else if ((IG_Alignment(wid) == XmALIGNMENT_END) && !HAS_MASK(wid) &&
		 (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid)))
	    label_x += IG_LargeIconRectWidth(wid) - IG_LabelRectWidth(wid);
	else if ((IG_Alignment(wid) == XmALIGNMENT_END) && HAS_MASK(wid) &&
		 (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid) + 2*ist))
	    label_x +=
		IG_LargeIconRectWidth(wid) - IG_LabelRectWidth(wid) - 2*ist;

	label_y += IG_LargeIconRectHeight(wid) + SPACING(wid);

    } else { /* XmSMALL_ICON */

	label_x += IG_SmallIconRectWidth(wid) + SPACING(wid);

	if (!HAS_MASK(wid) &&
	    (IG_SmallIconRectHeight(wid) > IG_LabelRectHeight(wid)))
	    label_y += (IG_SmallIconRectHeight(wid) -
			IG_LabelRectHeight(wid))/2 ;
	else if (HAS_MASK(wid) &&
	       (IG_SmallIconRectHeight(wid) > IG_LabelRectHeight(wid) + 2*ist))
	    label_y += (IG_SmallIconRectHeight(wid) -
			IG_LabelRectHeight(wid) - 2*ist)/2 ;
    }
    label_x += IG_MarginWidth(wid);
    /* test LayoutIsRtoLG(wid) here */
    if (LayoutIsRtoLG(wid)) {
	label_x = XtWidth(wid) - label_x - IG_LabelRectWidth(wid)
	    - IG_HLThickness(wid);
    } else {
	label_x += IG_HLThickness(wid) ;
    }

    if (x_ret) *x_ret = label_x;
    if (y_ret) *y_ret = label_y + IG_HLThickness(wid) + IG_MarginHeight(wid);

}




/************************************************************************
 * GetLargeIconX
 ************************************************************************/
static	Position
GetLargeIconX(
	Widget		wid)
{
    /* no test LayoutIsRtoLG(wid) here */
    Dimension ist = IG_ShadowThickness(wid);
    Position large_x = IG_HLThickness(wid) + IG_MarginWidth(wid);

    if ((IG_Alignment(wid) == XmALIGNMENT_CENTER) && !HAS_MASK(wid) &&
	(IG_LabelRectWidth(wid) > IG_LargeIconRectWidth(wid)))
	large_x += (IG_LabelRectWidth(wid) - IG_LargeIconRectWidth(wid)) /2;
    else if ((IG_Alignment(wid) == XmALIGNMENT_CENTER) && HAS_MASK(wid) &&
	     (IG_LabelRectWidth(wid) + 2*ist > IG_LargeIconRectWidth(wid)))
	large_x +=
	    (IG_LabelRectWidth(wid) + 2*ist - IG_LargeIconRectWidth(wid)) /2;
    else if ((IG_Alignment(wid) == XmALIGNMENT_END) && !HAS_MASK(wid) &&
	     (IG_LargeIconRectWidth(wid) < IG_LabelRectWidth(wid)))
	large_x += IG_LabelRectWidth(wid) - IG_LargeIconRectWidth(wid);
    else if ((IG_Alignment(wid) == XmALIGNMENT_END) && HAS_MASK(wid) &&
	     (IG_LargeIconRectWidth(wid) < IG_LabelRectWidth(wid) + 2*ist))
	large_x += IG_LabelRectWidth(wid) + 2*ist - IG_LargeIconRectWidth(wid);

    if (!HAS_MASK(wid))
	large_x += ist;

    return large_x;
}

/************************************************************************
 * GetLargeIconY
 ************************************************************************/
/* the shadow thickness is only incorporated when there is no mask */
#define GetLargeIconY(wid) (IG_HLThickness(wid) + IG_MarginHeight(wid) + \
			    (HAS_MASK(wid) ? 0 : IG_ShadowThickness(wid)))

/************************************************************************
 * GetSmallIconX
 ************************************************************************/
/* the shadow thickness is only incorporated when there is no mask */
#define GetSmallIconX(wid) (IG_HLThickness(wid) + IG_MarginWidth(wid) + \
			    (HAS_MASK(wid) ? 0 : IG_ShadowThickness(wid)))

/************************************************************************
 * GetSmallIconY
 ************************************************************************/
static	Position
GetSmallIconY(
	Widget		wid)
{
    Dimension ist = IG_ShadowThickness(wid);
    Position small_y = IG_HLThickness(wid) + IG_MarginHeight(wid);

    if (!HAS_MASK(wid) &&
	(IG_LabelRectHeight(wid) > IG_SmallIconRectHeight(wid)))
	small_y += (IG_LabelRectHeight(wid) - IG_SmallIconRectHeight(wid))/2;
    else if (HAS_MASK(wid) &&
	     (IG_LabelRectHeight(wid) + 2*ist > IG_SmallIconRectHeight(wid)))
	small_y += (IG_LabelRectHeight(wid) + 2*ist -
		    IG_SmallIconRectHeight(wid))/2;
    if (!HAS_MASK(wid))
	small_y += ist;

    return small_y;
}


/************************************************************************
 * GetIconLabelWidth
 *  including the shadows, the spacing, and the margin
 ************************************************************************/
static	Dimension
GetIconLabelWidth(
	Widget		wid)
{
    Dimension width = 2*IG_MarginWidth(wid);

    if (IG_ViewType(wid) == XmLARGE_ICON) {
	if (!HAS_MASK(wid))
	    width += MAX(IG_LargeIconRectWidth(wid),
			 IG_LabelRectWidth(wid)) + 2*IG_ShadowThickness(wid);
	else
	    width += MAX(IG_LargeIconRectWidth(wid),
			 IG_LabelRectWidth(wid) + 2*IG_ShadowThickness(wid));
    } else { /* XmSMALL_ICON */
	width += IG_SmallIconRectWidth(wid) + IG_LabelRectWidth(wid) +
	    2*IG_ShadowThickness(wid) + SPACING(wid);
    }
    return width;
}



/************************************************************************
 * GetIconLabelHeight
 *  including the shadows, the possible spacing, and the margin
 ************************************************************************/
static	Dimension
GetIconLabelHeight(
	Widget		wid)
{
    Dimension height = 2*IG_MarginHeight(wid);

    if (IG_ViewType(wid) == XmLARGE_ICON) {
	height += IG_LargeIconRectHeight(wid) + IG_LabelRectHeight(wid) +
	    2*IG_ShadowThickness(wid) + SPACING(wid);
    } else {
	if (!HAS_MASK(wid))
	    height += MAX(IG_SmallIconRectHeight(wid),
			  IG_LabelRectHeight(wid)) + 2*IG_ShadowThickness(wid);
	else
	    height += MAX(IG_SmallIconRectHeight(wid),
			  IG_LabelRectHeight(wid) + 2*IG_ShadowThickness(wid));
    }
    return height;
}


/***********
 * GetShapeInfo.
 *  This return the points that defines the shadow rectangle.
 *  Since this function is used for both shadow and highlight,
 *  the ht parameter is used to differentiate both case. It is -1
 *  for the highlight case and the real highlightthickness for
 *  the shadow case.
 *  The coordinates passed in should not have integrate any RtoL
 *  layout yet, since the function is doing mirroring at the end.
 *
 * The function returns the number of valid points (8 points in
 *  the generic case or only 2 points for a rectangle).
 *
 **********/
static Cardinal
GetShapeInfo(
        Widget wid,
        Position large_icon_x,
        Position small_icon_y,
        Position label_x,
        Position label_y,
        Dimension first_column_width,
        Dimension ht,
        XPoint * points)
{
    Dimension ist = IG_ShadowThickness(wid), rht = IG_HLThickness(wid);
    Cardinal i, n = 8;
    XmIconGadget ig = (XmIconGadget) wid ;
    Boolean highlight_case ;
    Dimension mh, mw, rmh = IG_MarginHeight(wid), rmw = IG_MarginWidth(wid);
    Dimension maxX;

    if (ht == INVALID_DIMENSION) {
	/* If we're doing the highlight case, mark the dimension
	   as zero but remember it so that the shadow is not drawn
	   later on in this routine */
	ht = 0 ;
	highlight_case = True ;
	mw = mh = 0;
    } else {
	highlight_case = False ;
	mw = rmw;
	mh = rmh;
    }

    /* then first treat the simple case where either the pixmap or
       the label is missing */
    points[0].x = ht + mw;
    points[0].y = ht + mh;
    points[1].x = 2*ist + 2*rht - ht + 2*rmw - mw;
    points[1].y = 2*ist + 2*rht - ht + 2*rmh - mh;

    if (XmStringEmpty(IG_LabelString(wid))) { /* no label */
	if ((IG_ViewType(wid) == XmLARGE_ICON) &&
	    (PIXMAP_VALID(IG_LargeIconPixmap(wid)))) {
	    points[1].x += IG_LargeIconRectWidth(wid) ;
	    points[1].y += IG_LargeIconRectHeight(wid) ;
	}
	if ((IG_ViewType(wid) == XmSMALL_ICON) &&
	    (PIXMAP_VALID(IG_SmallIconPixmap(wid)))) {
	    points[1].x += IG_SmallIconRectWidth(wid) ;
	    points[1].y += IG_SmallIconRectHeight(wid) ;
	}
	n = 2 ;
    } else  {/* a label */
	if (!HAS_PIXMAP(wid)) { /* but no pixmap */
	    points[1].x += IG_LabelRectWidth(wid) ;
	    points[1].y += IG_LabelRectHeight(wid) ;
	    n = 2 ;
	}
    }

    if (n == 8 && IG_ViewType(wid) == XmLARGE_ICON) {
	/* point #0 is top left corner of label */
	points[0].x = label_x - ist - rht + ht - rmw + mw;
	points[0].y = ht + mh + IG_LargeIconRectHeight(wid);
	if ((!HAS_MASK(wid) &&
	     (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid))) ||
	    (HAS_MASK(wid) &&
	     (IG_LargeIconRectWidth(wid) > IG_LabelRectWidth(wid) + 2*ist))) {
	    points[0].y += 2*rht - 2*ht + 2*rmh - 2*mh;
	    if (!HAS_MASK(wid)) points[0].y += 2*ist -1;
	    else if (highlight_case) points[0].y -= 1;
	    if (!highlight_case && HAS_MASK(wid))
		points[0].y += SPACING(wid);
	} else
	    points[0].y += SPACING(wid);
	points[1].x = large_icon_x - (ist + rht - ht + rmw - mw);
	if (highlight_case && HAS_MASK(wid)) points[1].x += ist;
	points[1].y = points[0].y ;
	points[2].x = points[1].x ;
	points[2].y = ht + mh;
	points[3].x = large_icon_x +
	    IG_LargeIconRectWidth(wid) + ist + rht - ht + rmw - mw -1;
	if (highlight_case && HAS_MASK(wid)) points[3].x -= ist;
	points[3].y = points[2].y ;
	points[4].x = points[3].x ;
	points[4].y = points[0].y;
	points[5].x = label_x +
	    IG_LabelRectWidth(wid) + ist + rht - ht + rmw - mw -1;
	if (!highlight_case && HAS_MASK(wid) && ist) points[5].x += 1 ;
	points[5].y = points[0].y ;
	points[6].x = points[5].x ;
	points[6].y = IG_LargeIconRectHeight(wid) + IG_LabelRectHeight(wid) +
	    2*ist + 2*rht - ht + 2*rmh - mh + SPACING(wid) -1;
	if (!highlight_case && HAS_MASK(wid) && ist) points[6].y += 1 ;
	points[7].x = points[0].x ;
	points[7].y = points[6].y ;
    } else if (n == 8) { /* SMALL_ICON */
	/* point #0 is top left corner of pixmap */
	points[0].x = ht + mw;
	points[0].y = small_icon_y - ist - rht + ht - rmh + mh;
        if (highlight_case && HAS_MASK(wid)) points[0].y += ist;
	points[1].x = IG_SmallIconRectWidth(wid) + ht + mw;
	if ((!HAS_MASK(wid) &&
	    (IG_SmallIconRectHeight(wid) > IG_LabelRectHeight(wid))) ||
	    (HAS_MASK(wid) &&
	     (IG_SmallIconRectHeight(wid) > IG_LabelRectHeight(wid) + 2*ist))){
	    points[1].x += 2*rht - 2*ht + 2*rmw - 2*mw;
	    if (!HAS_MASK(wid)) points[1].x += 2*ist -1;
	    else if (highlight_case) points[1].x -= 1;
	    if (!highlight_case && HAS_MASK(wid))
		points[1].x += SPACING(wid);
	} else
	    points[1].x += SPACING(wid);
	points[1].y = points[0].y;
	points[2].x = points[1].x;
	points[2].y = label_y - ist - rht + ht - rmh + mh;
	points[3].x = IG_SmallIconRectWidth(wid) + SPACING(wid) +
	    IG_LabelRectWidth(wid) + 2*ist + 2*rht - ht + 2*rmw - mw -1;
	if (highlight_case && HAS_MASK(wid)) points[3].x -= 1 ;
	points[3].y = points[2].y;
	points[4].x = points[3].x;
	points[4].y = label_y + IG_LabelRectHeight(wid) +
	    ist + rht - ht + rmh - mh -1;
	if (!highlight_case && HAS_MASK(wid) && ist) points[4].y += 1;
	points[5].x = points[1].x;
	points[5].y = points[4].y;
	points[6].x = points[5].x;
	points[6].y = small_icon_y +
	    IG_SmallIconRectHeight(wid) + ist + rht - ht + rmh - mh -1;
	if (highlight_case && HAS_MASK(wid)) points[6].y -= ist;
	points[7].x = points[0].x ;
	points[7].y = points[6].y;
    }

    /* now treat the case where the pixmap is present
       but it has a mask: no shadow around the pixmap, just the
       label, if there is a label of course.
       Only aply for the shadow, not for the highlight, so we
       checked that using the ht parameter at the beginning */
    if (HAS_MASK(wid) && !highlight_case) {
	if (XmStringEmpty(IG_LabelString(wid))) {
	    return 0 ;
	} else
	if (n == 8) {
	    /* uses the points already  computed for the polygon shadow case */
	    if (IG_ViewType(wid) == XmLARGE_ICON) {
		points[1].x = points[6].x ;
		points[1].y = points[6].y ;
	    } else {
		points[0].x = points[2].x ;
		points[0].y = points[2].y ;
		points[1].x = points[4].x ;
		points[1].y = points[4].y ;
	    }
	    n = 2 ;
	}
    }

    /* put the points in the frame */
    if (highlight_case)
	maxX = ig->rectangle.width - ht - mw;
    else
	maxX = MIN(ig->rectangle.width - ht - mw, first_column_width);
    for (i=0; i<n; i++) {
	if (points[i].x > maxX)
	    points[i].x = maxX;
	if (points[i].y > ig->rectangle.height - ht - mh)
	    points[i].y = ig->rectangle.height - ht - mh;

	/* add the gadget position in its parent */
	points[i].x += ig->rectangle.x ;
	points[i].y += ig->rectangle.y ;
    }

    /* test LayoutIsRtoLG(wid) here */

     if (LayoutIsRtoLG(wid) ) {
	 /* mirror the x position */
	 for (i = 0; i < n; i++)
	     points[i].x = 2*ig->rectangle.x + XtWidth(wid) - points[i].x -1;
	 /* + in the rectangle case, switch x because of XmeDrawShadow
	    required left-upper right-bottom point order */
	 if (n == 2) {
	     Position save_x = points[1].x ;
	     points[1].x = points[0].x +1;
	     points[0].x = save_x +1;
	 }
    }

    return n ;
}


/***********
 * GetContainerData
 **********/
static void
GetContainerData(
        Widget wid,
        XmContainerData container_data)
{
    XmIconGadgetClass igc = (XmIconGadgetClass) XtClass(wid);
    Widget container_id ;
    XmContainerTrait container_trait ;
    XmIconGadget 	ig = (XmIconGadget)wid;

    /**** initialize the trait struct */
    /* need to set detail_order_count and first_column so that
       container gets a min base. The problem we're trying to solve here
       is when the first kid gets created, when it computes its preferred
       size from its Initialize method, the container cannot come up
       for a valid dynamic detail_order_count or first_column_width
       since it doesn't have any child in it yet (this one hasn't
       been inserted yet: Initialize < InsertChild in Xt). So we
       pass the current kid value and the Container will use them
       as min, or default if you want. */
    container_data->detail_order_count = IG_DetailCount(wid);
    container_data->first_column_width =
	IG_HLThickness(wid) + GetIconLabelWidth(wid) - IG_MarginWidth(wid);

    container_data->selection_mode = XmNORMAL_MODE ;
    container_data->detail_order = NULL ;
    container_data->detail_tablist = NULL ;
    container_data->select_color = XmREVERSED_GROUND_COLORS ;

    /*** get the Container information using the trait */
    /* first get the widget id from which to fetch the trait */
    container_id = (igc->icong_class.get_container_parent)?
	(*(igc->icong_class.get_container_parent))(wid) : XtParent(wid);
    /* then get the trait info */
    container_trait = (XmContainerTrait) XmeTraitGet((XtPointer)
						     XtClass(container_id),
						     XmQTcontainer) ;
    if (container_trait)
	container_trait->getValues(container_id, container_data);
    else return ;

    /*** detail_order_count might be 0: mean no detail displayed,
      even if the icon has some.
      detail_order might be NULL: mean if there is detail to display,
      we'll use a default order [1..N] when we need it.
      detail_tablist might be null, mean concat for the kid.
      (XmContainer returns NULL unless it can't come up with a reasonable
       tablist due to its size being NULL or too small)
      first_column_width might be 0, which mean spatial: use icon/label only */


    /* a return of 0 for first column width means we are
       in spatial, in this case, don't use rectangle.x at all */
    if (!container_data->first_column_width) {
	container_data->first_column_width =
	    IG_HLThickness(wid) + GetIconLabelWidth(wid) - IG_MarginWidth(wid);
    } else {

	/* remove the current hor indentation */
	if (LayoutIsRtoLG(wid)) {
	    if (XtWidth(XtParent(wid))) {
		if ((Position)container_data->first_column_width >
		    (XtWidth(XtParent(wid)) - ig->rectangle.width -
		     ig->rectangle.x)) {
		    container_data->first_column_width -=
			(XtWidth(XtParent(wid)) - ig->rectangle.width -
			 ig->rectangle.x) ;
		} else {
		    container_data->first_column_width =
			IG_HLThickness(wid) + IG_MarginWidth(wid);
		}
	    } else { /* parent not sized yet */
		if ((Position)container_data->first_column_width >
		    ig->rectangle.x)
		    container_data->first_column_width -= ig->rectangle.x ;
		else
		    container_data->first_column_width =
			IG_HLThickness(wid) + IG_MarginWidth(wid);
	    }
	} else /* regular case: not RtoL*/
	    /* here we have to worry of special cases where
	       the first_column_width and the position of the kid
	       are extreme */
	    if (((Position)container_data->first_column_width >
		 ig->rectangle.x)
		&& ig->rectangle.x >= 0) {
		container_data->first_column_width -= ig->rectangle.x ;
		if (container_data->first_column_width <
		          IG_HLThickness(wid) + IG_MarginWidth(wid))
		    container_data->first_column_width
			= IG_HLThickness(wid) + IG_MarginWidth(wid);
	    }
	    else
		container_data->first_column_width =
		    IG_HLThickness(wid) + IG_MarginWidth(wid);
    }

    /*** leaving this routine:
       detail_order_count might be 0: no detail to display
       detail_order might be NULL. : use implicit order
       container_data->detail_tablist might be NULL: use concat
       container_data->first_column_width is:
        in spatial, Container returned first_column_width=0, and
        we turned it in GetContainerData into icon_width (included highlight).
        in outline/detail, first_column_width is the clipping width
        from the current position of the icon to the start of
        the detail rendering (included highlight too). */
}



/************************************************************************
 * Redisplay
 * This is the main routine of this baby.
 ************************************************************************/
static	void
Redisplay(
	Widget	wid,
	XEvent	*event,		/* unused */
	Region	region)		/* unused */
{
    XmIconGadget 	ig = (XmIconGadget)wid;
    GC		gc;
    GC		background_gc;
    Dimension	pm_width,pm_height;
    XRectangle 	clip_rect;
    Boolean		clip_set = False;
    int depth;
    XmContainerDataRec container_data ;
    Position large_icon_x = 0, small_icon_y = 0, small_icon_x,
             label_x = 0, label_y = 0 ;
    Dimension w, h, ist = IG_ShadowThickness(wid), ht = IG_HLThickness(wid);
    Dimension mw = IG_MarginWidth(wid), mh = IG_MarginHeight(wid);
    Cardinal i ;
    XPoint shad_points[8] ;

    /* a gc for the ink (text only) */
    if (!XtIsSensitive(wid))
        gc = IG_InsensitiveGC(wid);
    else
        gc = IG_NormalGC(wid);

    /**** invert the background gc for selected view */
    if (IG_VisualEmphasis(wid) == XmSELECTED) {
	background_gc = IG_SelectedGC(wid);

	/* if inverse_gc has been set, it holds the parent background
	   as its foreground (ink). Use it when in selected mode */
    	if (IG_InverseGC(wid))
	    gc = IG_InverseGC(wid);
    } else {
	background_gc = IG_BackgroundGC(wid);
    }


    /**** get the container information */
    container_data.valueMask = ContAllValid ;
    GetContainerData(wid, &container_data);

    /**** clear the background */
    /* in detail mode, clear the entire gadget area using
       background_gc (which, depending on the selected mode is the
       selectedGC or the BackgroundGC of the icon), while in
       iconlabel only, only clear the iconlabel using cleararea
       (and we will later clear the label part using background_gc) */
    if (SHOW_DETAIL(wid, &container_data)) {
        XSetClipMask(XtDisplay(wid), background_gc, None);
	XFillRectangle(XtDisplay(wid), XtWindow(wid), background_gc,
		       ig->rectangle.x, ig->rectangle.y,
		       ig->rectangle.width, ig->rectangle.height);
    } else {
        XSetClipMask(XtDisplay(wid), IG_BackgroundGC(wid), None);
    }



    /**** render the pixmap first */
    if ((IG_ViewType(wid) == XmLARGE_ICON) &&
	(PIXMAP_VALID(IG_LargeIconPixmap(wid)))) {

	large_icon_x = GetLargeIconX(wid); /* no rtl yet */

	XmeGetPixmapData(XtScreen(wid),
			 IG_LargeIconPixmap(wid),
			 NULL,
			 &depth,
			 NULL, NULL,
			 NULL, NULL,
			 NULL, NULL);

	pm_width = IG_LargeIconRectWidth(wid);
	if (large_icon_x + pm_width >
	    MIN(ig->rectangle.width, container_data.first_column_width)) {
	    pm_width = MIN(ig->rectangle.width - 2*ht,
			   container_data.first_column_width - ht);
	    pm_width -= MIN(pm_width, (Dimension)large_icon_x);

	    if (LayoutIsRtoLG(wid))
		large_icon_x = ig->rectangle.width -
		    MIN(ig->rectangle.width - 2*ht,
			container_data.first_column_width - ht) ;
	} else {
	    if (LayoutIsRtoLG(wid))
		large_icon_x = ig->rectangle.width - large_icon_x - pm_width ;
	}

	pm_height = IG_LargeIconRectHeight(wid) ;
	if (ht + ist + pm_height > ig->rectangle.height - 2*ht)
	    pm_height = ig->rectangle.height - ist - ht;

	/* clip with the mask if any */
	if (PIXMAP_VALID(IG_LargeIconMask(wid))) {

	    XSetClipMask(XtDisplay(wid), IG_NormalGC(wid),
			 IG_LargeIconMask(wid));

	    XSetClipOrigin(XtDisplay(wid), IG_NormalGC(wid),
			   ig->rectangle.x + large_icon_x,
			   ig->rectangle.y + ht + mh);
	} else {
	    XSetClipMask(XtDisplay(wid), IG_NormalGC(wid),None);
	}

	if (depth == XtParent(wid)->core.depth)
	    XCopyArea(XtDisplay(wid),IG_LargeIconPixmap(wid),
		      XtWindow(wid), IG_NormalGC(wid), 0,0,
		      pm_width, pm_height,
		      ig->rectangle.x + large_icon_x,
		      ig->rectangle.y + GetLargeIconY(wid));
	else
	    if (depth == 1)
		XCopyPlane(XtDisplay(wid),IG_LargeIconPixmap(wid),
			   XtWindow(wid), IG_NormalGC(wid),
			   0,0,
			   pm_width, pm_height,
			   ig->rectangle.x + large_icon_x,
			   ig->rectangle.y + GetLargeIconY(wid), 1);
    }

    if ((IG_ViewType(wid) == XmSMALL_ICON) &&
	(PIXMAP_VALID(IG_SmallIconPixmap(wid)))) {

	small_icon_y = GetSmallIconY(wid);

	XmeGetPixmapData(XtScreen(wid),
			 IG_SmallIconPixmap(wid),
			 NULL,
			 &depth,
			 NULL, NULL,
			 NULL, NULL,
			 NULL, NULL);

	pm_width = IG_SmallIconRectWidth(wid);
	small_icon_x = GetSmallIconX(wid);

	if (small_icon_x + pm_width  > MIN(ig->rectangle.width - 2*ht,
				  container_data.first_column_width - 2*ht)) {
	    pm_width = MIN(ig->rectangle.width,
			   container_data.first_column_width) - 2*ht;
	    pm_width -= MIN(pm_width, (Dimension)small_icon_x);

	    if (LayoutIsRtoLG(wid))
		small_icon_x = ig->rectangle.width + 2*ht -
		    MIN(ig->rectangle.width,
			container_data.first_column_width) ;
	} else {
	    if (LayoutIsRtoLG(wid))
		small_icon_x = ig->rectangle.width - small_icon_x - pm_width ;
	}

	pm_height = IG_SmallIconRectHeight(wid);
	if (small_icon_y + pm_height > ig->rectangle.height - 2*ht)
	    pm_height = ig->rectangle.height - small_icon_y;

	/* clip with the mask if any */
	if (PIXMAP_VALID(IG_SmallIconMask(wid))) {
	    XSetClipMask(XtDisplay(wid), IG_NormalGC(wid),
			 IG_SmallIconMask(wid));
	    XSetClipOrigin(XtDisplay(wid), IG_NormalGC(wid),
			   ig->rectangle.x + small_icon_x,
			   ig->rectangle.y + small_icon_y);
	} else {
	    XSetClipMask(XtDisplay(wid), IG_NormalGC(wid),None);
	}

	if (depth == XtParent(wid)->core.depth)
	    XCopyArea(XtDisplay(wid),IG_SmallIconPixmap(wid),
		      XtWindow(wid), IG_NormalGC(wid),0,0,
		      pm_width,pm_height,
		      ig->rectangle.x + small_icon_x,
		      ig->rectangle.y + small_icon_y);
	else
	    if (depth == 1)
		XCopyPlane(XtDisplay(wid),IG_SmallIconPixmap(wid),
			   XtWindow(wid), IG_NormalGC(wid),0,0,
			   pm_width,pm_height,
			   ig->rectangle.x + small_icon_x,
			   ig->rectangle.y + small_icon_y, 1);
    }

    clip_rect.y = ig->rectangle.y + ht + mh;
    clip_rect.width = MIN(ig->rectangle.width - 2*ht - 2*mw,
			  container_data.first_column_width - ht - mw);
    clip_rect.height = ig->rectangle.height - 2*ht - 2*mh;

    if (LayoutIsRtoLG(wid)) {
	clip_rect.x = ig->rectangle.x + ig->rectangle.width -
	    ht - clip_rect.width - mw;
    } else {
	clip_rect.x = ig->rectangle.x + ht + mw;
    }

    XSetClipRectangles(XtDisplay(wid),gc,0,0,&clip_rect,1,Unsorted);
    XSetClipRectangles(XtDisplay(wid),background_gc,0,0,&clip_rect,1,Unsorted);


    /**** then draw the label part of the icon */
    GetLabelXY(wid, &label_x, &label_y) ;

    if (!XmStringEmpty(IG_LabelString(wid))) {
	XFillRectangle(XtDisplay(wid), XtWindow(wid), background_gc,
		       ig->rectangle.x + label_x,
		       ig->rectangle.y + label_y,
		       IG_LabelRectWidth(wid),
		       IG_LabelRectHeight(wid));

	/* if we are in the inverse_color case, we need to draw
	   the string with parent background ink, and
	   parent foreground as back color (The previous fillrectangle
	   took care of that last part). gc has been set
	   up to IG_InverseGC at the beginning of this routine,
	   so what's left is the forcing of this ink/foreground */

	/*Draw shadow for insensitive text*/
	if (!XtIsSensitive(wid)) {
		XmStringDraw(XtDisplay(wid),XtWindow(wid),
		      IG_RenderTable(wid),IG_LabelString(wid), IG_ShadowGC(wid),
		      ig->rectangle.x + label_x+1 + DEFAULT_LABEL_MARGIN_WIDTH,
		      ig->rectangle.y + label_y+1 + DEFAULT_LABEL_MARGIN_HEIGHT,
		      IG_LabelRectWidth(wid) - 2*DEFAULT_LABEL_MARGIN_WIDTH,
		      XmALIGNMENT_BEGINNING,
		      LayoutG(wid), NULL);
	}

	XmStringDraw(XtDisplay(wid),XtWindow(wid),
		      IG_RenderTable(wid),IG_LabelString(wid), gc,
		      ig->rectangle.x + label_x + DEFAULT_LABEL_MARGIN_WIDTH,
		      ig->rectangle.y + label_y + DEFAULT_LABEL_MARGIN_HEIGHT,
		      IG_LabelRectWidth(wid) - 2*DEFAULT_LABEL_MARGIN_WIDTH,
		      XmALIGNMENT_BEGINNING,
		      LayoutG(wid), NULL);
    }

    XSetClipMask(XtDisplay(wid),background_gc,None);

    /**** now the polygon shadow around the icon+label, or the
          square shadow around a masked pixmap or a no pixmap icon */

    /* only draw shadows if there is something inside */
    if (container_data.first_column_width) {
	Cardinal n ;

	/* undo the ltr layout for label_x, because getshapeinfo
	   does it */
	if (LayoutIsRtoLG(wid)) {
	    label_x = XtWidth(wid) - IG_LabelRectWidth(wid) - label_x ;
	}
	n = GetShapeInfo(wid, GetLargeIconX(wid), small_icon_y,
			 label_x, label_y,
			 container_data.first_column_width, ht,
			 &shad_points[0]) ;
	/* only draw the polygon shadow if there is no mask
	   and an existing pixmap */
	if (n == 2) {
	    XmeDrawShadows(XtDisplay(wid),XtWindow(wid),
			   IG_TopShadowGC(wid), IG_BottomShadowGC(wid),
			   shad_points[0].x,  shad_points[0].y,
			   shad_points[1].x - shad_points[0].x,
			   shad_points[1].y - shad_points[0].y,
			   ist, XmSHADOW_OUT);
	}
	else if (n == 8) {
	    XmeDrawPolygonShadow (XtDisplay(wid),XtWindow(wid),
				  IG_TopShadowGC(wid), IG_BottomShadowGC(wid),
				  shad_points, n, ist, XmSHADOW_OUT);
	}
    }

    /**** then comes the details rendering */

    if (SHOW_DETAIL(wid, &container_data)&&
	container_data.first_column_width >= ht + mw) {
	unsigned int tab_count = 0 ;
	Dimension detail_x, detail_y;
	XmStringTable new_detail ;
	int lab_baseline = 0, detail_baseline ;

	/* get the detail table to be displayed */
	new_detail =
	    GetStringTableReOrdered(IG_Detail(wid), IG_DetailCount(wid),
				    container_data.detail_order,
				    container_data.detail_order_count);

	if (container_data.detail_tablist)
	    tab_count = XmTabListTabCount(container_data.detail_tablist) ;
	/* the extra tabs are ignored */
	tab_count = MIN(tab_count, container_data.detail_order_count);
	/* tab_count might be 0 at this point, but it won't be treated as
	   a special case */

	if (IG_LabelString(wid))
	    lab_baseline = XmStringBaseline(IG_RenderTable(wid),
					    IG_LabelString(wid)) ;

	/* detail_x is in gadget relative coordinate */
	detail_x = container_data.first_column_width;

	for (i = 0 ; i < IG_DetailCount(wid) ; i++) {
	    Position next_tab_x = 0 ;

	    w = 0 ;

	    detail_x += DEFAULT_HOR_SPACING ;

	    if (container_data.detail_tablist)
		next_tab_x = container_data.first_column_width +
		    _XmTabListGetPosition(XtScreen(wid),
					 container_data.detail_tablist,
					 XmPIXELS, i);
	    if (new_detail[i]) {

		/* if we have a tab, use it, don't bother to call an
		   expensive string extent. */
		if (i < tab_count) {
		    clip_rect.width = MIN (next_tab_x - detail_x,
					   ig->rectangle.width - 2*ht - 2*mw -
					   detail_x);
		    clip_rect.x = ig->rectangle.x + detail_x;
		    if (LayoutIsRtoLG(wid))
			clip_rect.x = ig->rectangle.x + ig->rectangle.width
			    - detail_x - clip_rect.width;

		    clip_rect.y = ig->rectangle.y + ht + mh;
		    clip_rect.height = ig->rectangle.height - 2*ht - 2*mh;
		    XSetClipRectangles(XtDisplay(wid),gc,0,0,&clip_rect,1,
				       Unsorted);
		} else {
		    XmStringExtent(IG_RenderTable(wid), new_detail[i], &w, &h);
		    if (!clip_set) {
			clip_rect.x = ig->rectangle.x + ht + mw;
			clip_rect.y = ig->rectangle.y + ht + mh;
			clip_rect.width = ig->rectangle.width - 2*ht - 2*mw;
			clip_rect.height = ig->rectangle.height - 2*ht - 2*mh;
			XSetClipRectangles(XtDisplay(wid),gc,0,0,&clip_rect,1,
					   Unsorted);
			/* only set this clip once */
			clip_set = True ;
		    }
		}

		detail_baseline = XmStringBaseline(IG_RenderTable(wid),
						    new_detail[i]);
		detail_y = label_y + DEFAULT_LABEL_MARGIN_HEIGHT +
		    lab_baseline - detail_baseline;

		XmStringDraw(XtDisplay(wid),XtWindow(wid),
			     IG_RenderTable(wid), new_detail[i], gc,
			     (LayoutIsRtoLG(wid)) ?
			     (ig->rectangle.x + ig->rectangle.width - detail_x
			      - ((i < tab_count) ? clip_rect.width : w))
			     : (ig->rectangle.x + detail_x),
			     ig->rectangle.y + detail_y,
			     ((i < tab_count) ? clip_rect.width : w),
			     XmALIGNMENT_BEGINNING,
			     LayoutG(wid),
			     NULL);  /* clip is done in gc */

	    }

	    if (i < tab_count) {
		detail_x = next_tab_x;
	    } else {
		detail_x += w ;
	    }
	}
    }

    /**** draw the highlight if needed */
    if (ig->gadget.highlighted) {
	if(((XmGadgetClass) XtClass(wid))->gadget_class.border_highlight){
	    (*(((XmGadgetClass) XtClass(wid))
	       ->gadget_class.border_highlight))(wid) ;
	}
    }


}


/************************************************************************
 * SetValues
 ************************************************************************/
static	Boolean
SetValues(
	Widget		cw,
	Widget		rw,
	Widget		nw,
	ArgList		args,	/* unused */
	Cardinal	*num_args) /* unused */
{
    Boolean			Relayout = False;
    Boolean			Redraw = False;
    unsigned int w, h ;
    Cardinal i ;


    if (IG_ViewType(nw) != IG_ViewType(cw)) {
	if (XmRepTypeValidValue(XmRID_VIEW_TYPE,IG_ViewType(nw),nw))
	    Relayout = Redraw = True;
	else
	    IG_ViewType(nw) = IG_ViewType(cw);
    }

    if (IG_VisualEmphasis(nw) != IG_VisualEmphasis(cw)) {
	if (XmRepTypeValidValue(XmRID_VISUAL_EMPHASIS,
				IG_VisualEmphasis(nw),nw))
	    Redraw = True;
	else
	    IG_VisualEmphasis(nw) = IG_VisualEmphasis(cw);
    }

    if (IG_Alignment(nw) != IG_Alignment(cw)) {
	if (XmRepTypeValidValue(XmRID_ALIGNMENT, IG_Alignment(nw), nw))
	    Relayout = Redraw = True;
	else
	    IG_Alignment(nw) = IG_Alignment(cw);
    }

    if ((IG_Background(nw) != IG_Background(cw)) ||
	(IG_Foreground(nw) != IG_Foreground(cw)) ||
	(IG_TopShadowColor(nw) != IG_TopShadowColor(cw)) ||
	(IG_BottomShadowColor(nw) != IG_BottomShadowColor(cw)) ||
	(IG_HighlightColor(nw) != IG_HighlightColor(cw)) ||
	(IG_BackgroundPixmap(nw) != IG_BackgroundPixmap(cw)) ||
	(IG_TopShadowPixmap(nw) != IG_TopShadowPixmap(cw)) ||
	(IG_BottomShadowPixmap(nw) != IG_BottomShadowPixmap(cw)) ||
	(IG_HighlightPixmap(nw) != IG_HighlightPixmap(cw))) {
	UpdateGCs(nw);
    }

    if (IG_RenderTable(nw) != IG_RenderTable(cw)) {
	XmRenderTableFree(IG_RenderTable(cw));
	IG_RenderTable(nw) = XmRenderTableCopy(IG_RenderTable(nw), NULL, 0);
	UpdateGCs(nw);

	if (!XmStringEmpty(IG_LabelString(nw)))
	    XmStringExtent(IG_RenderTable(nw), IG_LabelString(nw),
			    &(IG_LabelRectWidth(nw)),
			    &(IG_LabelRectHeight(nw)));

	IG_LabelRectWidth(nw) += 2*DEFAULT_LABEL_MARGIN_WIDTH ;
	IG_LabelRectHeight(nw) += 2*DEFAULT_LABEL_MARGIN_HEIGHT  ;

	Relayout = Redraw = True;
    }

    if (IG_LabelString(nw) != IG_LabelString(cw)) {
	XmStringFree(IG_LabelString(cw));
	IG_LabelString(nw) = XmStringCopy(IG_LabelString(nw));

	if (!XmStringEmpty(IG_LabelString(nw)))
	    XmStringExtent(IG_RenderTable(nw), IG_LabelString(nw),
			    &(IG_LabelRectWidth(nw)),
			    &(IG_LabelRectHeight(nw)));
	else {
	    IG_LabelRectWidth(nw) = 0 ;
	    IG_LabelRectHeight(nw) = 0 ;
	}
	IG_LabelRectWidth(nw) += 2*DEFAULT_LABEL_MARGIN_WIDTH ;
	IG_LabelRectHeight(nw) += 2*DEFAULT_LABEL_MARGIN_HEIGHT  ;

	Relayout = Redraw = True;
    }

    if (IG_LargeIconMask(nw) != IG_LargeIconMask(cw)) {
	if (OwnLargeMask(cw)) {
	    XDeleteContext(XtDisplay(nw),
			   (Window) nw,
			   largeIconContext) ;
	    if (PIXMAP_VALID(IG_LargeIconMask(cw)))
		XmDestroyPixmap(XtScreen(cw),
				IG_LargeIconMask(cw));
	}
	if (IG_ViewType(nw) == XmLARGE_ICON)
	    Relayout = Redraw = True;
    }

    if (IG_LargeIconPixmap(nw) != IG_LargeIconPixmap(cw)) {
	if (IG_ViewType(nw) == XmLARGE_ICON)
	    Relayout = Redraw = True;

	/* if the new icon is different, refetch the sizes */
	if (PIXMAP_VALID(IG_LargeIconPixmap(nw)))
	    XmeGetPixmapData(XtScreen(nw),
			     IG_LargeIconPixmap(nw),
			     NULL,
			     NULL,
			     NULL, NULL,
			     NULL, NULL,
			     &w, &h);
	else {
	    w = h = 0 ;
	}

	IG_LargeIconRectWidth(nw)  = (unsigned short)w;
	IG_LargeIconRectHeight(nw) = (unsigned short)h;
    }

    if (IG_SmallIconMask(nw) != IG_SmallIconMask(cw)) {
	if (OwnSmallMask(cw)) {
	    XDeleteContext(XtDisplay(nw),
			   (Window) nw,
			   smallIconContext) ;
	    if (PIXMAP_VALID(IG_SmallIconMask(cw)))
		XmDestroyPixmap(XtScreen(cw),
				IG_SmallIconMask(cw));
	}
	if (IG_ViewType(nw) == XmSMALL_ICON)
	    Relayout = Redraw = True;
    }

    if (IG_SmallIconPixmap(nw) != IG_SmallIconPixmap(cw)) {
	if (IG_ViewType(nw) == XmSMALL_ICON)
	    Relayout = Redraw = True;

	if (PIXMAP_VALID(IG_SmallIconPixmap(nw)))
	    XmeGetPixmapData(XtScreen(nw),
			     IG_SmallIconPixmap(nw),
			     NULL,
			     NULL,
			     NULL, NULL,
			     NULL, NULL,
			     &w, &h);
	else {
	    w = h = 0 ;
	}

	IG_SmallIconRectWidth(nw)  = (unsigned short)w;
	IG_SmallIconRectHeight(nw) = (unsigned short)h;
    }

    if (IG_Detail(nw) != IG_Detail(cw)) {
	/* new XmNdetail copy in */

	/* first free the current detail table and strings if present */
	if (IG_Detail(cw) && IG_DetailCount(cw)) {
	    for (i=0; i<IG_DetailCount(cw); i++)
		XmStringFree(IG_Detail(cw)[i]);
	    XtFree((char*)IG_Detail(cw));
	}

	/* now copy */
	if (IG_Detail(nw) && IG_DetailCount(nw)) {
	    IG_Detail(nw) = (XmStringTable)
		XtMalloc(IG_DetailCount(nw) * sizeof(XmString));

	    for (i=0; i<IG_DetailCount(nw); i++)
		IG_Detail(nw)[i] = XmStringCopy(IG_Detail(rw)[i]);
	}
	Relayout = Redraw = True;
    }

    if (IG_DetailCount(nw) != IG_DetailCount(cw)) {
	Relayout = Redraw = True;
    }

    if (LayoutG(nw) != LayoutG(cw) || XtIsSensitive(nw) != XtIsSensitive(cw)) {
	Redraw = True;
    }

    if ((Relayout) ||
	(IG_Spacing(nw) != IG_Spacing(cw)) ||
	(IG_MarginWidth(nw) != IG_MarginWidth(cw)) ||
	(IG_MarginHeight(nw) != IG_MarginHeight(cw)) ||
	(IG_HLThickness(nw) != IG_HLThickness(cw)) ||
	(IG_ShadowThickness(nw) != IG_ShadowThickness(cw))) {
	if (IG_RecomputeSize(nw)) {
	    /* if a specific size has not been requested at the same time
	       just forget the current size */
	    if (rw->core.width == cw->core.width)
		nw->core.width = 0;
	    if (rw->core.height == cw->core.height)
		nw->core.height = 0;
	}

	GetSize(nw, &nw->core.width, &nw->core.height);
    }


    return(Redraw);
}


/************************************************************************
 * QueryGeometry
 ************************************************************************/
static	XtGeometryResult
QueryGeometry(
	Widget			wid,
	XtWidgetGeometry	*intended,
	XtWidgetGeometry	*desired)
{

    if (IG_RecomputeSize(wid) == False) {
        desired->width = XtWidth(wid) ;
	desired->height = XtHeight(wid) ;
    } else {
	desired->width = 0 ;
	desired->height = 0 ;
	GetSize(wid, &desired->width, &desired->height);
	/* the above asks for the containertrait firstColumnWidth */
    }

    return XmeReplyToQueryGeometry(wid, intended, desired) ;
}


/*-----------------
| XmGadget methods |
-----------------*/


/************************************************************************
 * HighlightBorder
 ************************************************************************/
static void
HighlightBorder(
        Widget w )
{
    XmIconGadget ig = (XmIconGadget) w ;
    XmContainerDataRec container_data ;
    Dimension ht = IG_HLThickness(w) ;

    /* test LayoutIsRtoLG(wid) here */

    ig->gadget.highlighted = True ;
    ig->gadget.highlight_drawn = True ;

    if(ig->rectangle.width == 0 || ig->rectangle.height == 0
       || ig->gadget.highlight_thickness == 0) return ;

    /* The highlight is different depending if there is a detail
       or not. With a detail, it's a rectangular shadow,
       otherwise it's drawn around the icon+label */

    /**** get the container information */
    container_data.valueMask = ContFirstColumnWidth | ContSelectionMode ;
    GetContainerData(w, &container_data);

    XSetClipMask(XtDisplay(w), IG_HighlightGC(w), None);

    if (SHOW_DETAIL(w, &container_data)) {
        ChangeHighlightGC(w, container_data.selection_mode, ht);

	if (container_data.selection_mode == XmADD_MODE)
	  _XmDrawHighlight(XtDisplay(w),XtWindow(w),
			   IG_HighlightGC(w),
			   ig->rectangle.x,  ig->rectangle.y,
			   ig->rectangle.width, ig->rectangle.height,
			   ht, LineDoubleDash);
	else
	  XmeDrawHighlight(XtDisplay(w),XtWindow(w),
			   IG_HighlightGC(w),
			   ig->rectangle.x,  ig->rectangle.y,
			   ig->rectangle.width, ig->rectangle.height,
			   ht);

    } else {
	Position label_x, label_y ;
	XPoint points[8] ;

	/* do the polygon highlight around the icon + label part */
	GetLabelXY(w, &label_x, &label_y) ;

	/* undo the ltr layout for label_x, because getshapeinfo
	   does it */
	if (LayoutIsRtoLG(w)) {
	    label_x = XtWidth(w) - IG_LabelRectWidth(w) - label_x ;
	}

	if (GetShapeInfo(w, GetLargeIconX(w), GetSmallIconY(w),
			 label_x, label_y,
			 container_data.first_column_width, INVALID_DIMENSION,
			 points) == 2) {
	  /* only 2 points have been set, that's the simple case */
	  if (container_data.selection_mode == XmADD_MODE) {
	    ChangeHighlightGC(w, container_data.selection_mode, ht);
	    _XmDrawHighlight(XtDisplay(w),XtWindow(w),
			     IG_HighlightGC(w),
			     points[0].x,  points[0].y,
			     points[1].x - points[0].x,
			     points[1].y - points[0].y,
			     ht, LineDoubleDash);
	  } else
	    XmeDrawHighlight(XtDisplay(w),XtWindow(w),
			     IG_HighlightGC(w),
			     points[0].x,  points[0].y,
			     points[1].x - points[0].x,
			     points[1].y - points[0].y,
			     ht);
	} else {
	    ChangeHighlightGC(w, container_data.selection_mode, 1);
	    XmeDrawPolygonShadow (XtDisplay(w),XtWindow(w),
				  IG_HighlightGC(w), IG_HighlightGC(w),
				  points, 8, ht, XmSHADOW_OUT);
	}
    }

}

/************************************************************************
 * UnhighlightBorder
 ************************************************************************/
static void
UnhighlightBorder(
        Widget w )
{
    XmIconGadget ig = (XmIconGadget) w ;
    XmContainerDataRec container_data ;
    Dimension ht = IG_HLThickness(w) ;
    GC background_gc ;

    /* test LayoutIsRtoLG(wid) here */

    ig->gadget.highlighted = False ;
    ig->gadget.highlight_drawn = False ;

    if(ig->rectangle.width == 0 || ig->rectangle.height == 0
       || ig->gadget.highlight_thickness == 0) return ;

    /* unhighlight has to use the current selected background,
       it cannot call the superclass unhighlight as in highlight
       because Gadget does a simple clearborder. */

    /**** get the container information */
    container_data.valueMask = ContFirstColumnWidth ;
    GetContainerData(w, &container_data);

    if(XmIsManager (XtParent(w)))  {
	background_gc = ((XmManagerWidget)XtParent(w))
	    ->manager.background_GC ;
    } else {
	XSetClipMask(XtDisplay(w), IG_BackgroundGC(w), None);
	background_gc = IG_BackgroundGC(w) ;
    }


    if (SHOW_DETAIL(w, &container_data)) {
	/* unhighlight the entire gadget area */
	XmeDrawHighlight(XtDisplay(w),XtWindow(w),
			 background_gc,
			 ig->rectangle.x,  ig->rectangle.y,
			 ig->rectangle.width, ig->rectangle.height,
			 ht);
    } else {
	Position label_x, label_y ;
	XPoint points[8] ;

	/* do the polygon unhighlight around the icon + label part */
	GetLabelXY(w, &label_x, &label_y) ;

	/* undo the ltr layout for label_x, because getshapeinfo
	   does it */
	if (LayoutIsRtoLG(w)) {
	    label_x = XtWidth(w) - IG_LabelRectWidth(w) - label_x ;
	}

	if (GetShapeInfo(w, GetLargeIconX(w), GetSmallIconY(w),
			 label_x, label_y,
			 container_data.first_column_width, INVALID_DIMENSION,
			 points) == 2) {
	    /* only 2 points have been set, that's the simple case */
	    XmeDrawHighlight(XtDisplay(w),XtWindow(w),
			     background_gc,
			     points[0].x,  points[0].y,
			     points[1].x - points[0].x,
			     points[1].y - points[0].y,
			     ht);
	} else {
	    XmeDrawPolygonShadow (XtDisplay(w),XtWindow(w),
				  background_gc, background_gc,
				  points, 8, ht, XmSHADOW_OUT);
	}

    }
}



/*******************************************************************
 *
 *  IconGCacheCompare
 *
 *******************************************************************/
static int
IconGCacheCompare(
        XtPointer A,
        XtPointer B )
{
    XmIconGCacheObjPart *icon_inst = (XmIconGCacheObjPart *) A ;
    XmIconGCacheObjPart *icon_cache_inst = (XmIconGCacheObjPart *) B ;

    if((icon_inst-> render_table == icon_cache_inst->render_table) &&
       (icon_inst-> selected_GC == icon_cache_inst->selected_GC) &&
       (icon_inst-> inverse_GC == icon_cache_inst->inverse_GC) &&
       (icon_inst-> normal_GC == icon_cache_inst->normal_GC) &&
       (icon_inst-> background_GC == icon_cache_inst->background_GC) &&
       (icon_inst-> insensitive_GC == icon_cache_inst->insensitive_GC) &&
       (icon_inst-> top_shadow_GC == icon_cache_inst->top_shadow_GC) &&
       (icon_inst-> bottom_shadow_GC == icon_cache_inst->bottom_shadow_GC) &&
       (icon_inst-> highlight_GC == icon_cache_inst->highlight_GC) &&
       (icon_inst-> background == icon_cache_inst->background) &&
       (icon_inst-> foreground == icon_cache_inst->foreground) &&
       (icon_inst-> top_shadow_color == icon_cache_inst->top_shadow_color) &&
       (icon_inst-> highlight_color == icon_cache_inst->highlight_color) &&
       (icon_inst-> top_shadow_pixmap == icon_cache_inst->background_pixmap) &&
       (icon_inst-> background_pixmap == icon_cache_inst->top_shadow_pixmap) &&
       (icon_inst-> highlight_pixmap == icon_cache_inst->highlight_pixmap) &&
       (icon_inst-> bottom_shadow_color ==
	    icon_cache_inst->bottom_shadow_color) &&
       (icon_inst-> bottom_shadow_pixmap ==
	    icon_cache_inst->bottom_shadow_pixmap) &&
       (icon_inst-> alignment == icon_cache_inst->alignment) &&
       (icon_inst-> spacing == icon_cache_inst->spacing) &&
       (icon_inst-> margin_width == icon_cache_inst->margin_width) &&
       (icon_inst-> margin_height == icon_cache_inst->margin_height)  )
       return 1;
    else
       return 0;
 }


/****************************************************
 *   Functions for manipulating Secondary Resources.
 *********************************************************/
/*
 * GetIconGClassSecResData()
 *    Create a XmSecondaryResourceDataRec for each secondary resource;
 *    Put the pointers to these records in an array of pointers;
 *    Return the pointer to the array of pointers.
 *	client_data = Address of the structure in the class record which
 *	  represents the (template of ) the secondary data.
 */
static Cardinal
GetIconGClassSecResData(
        WidgetClass w_class,	/* unused */
        XmSecondaryResourceData **data_rtn )
{   int arrayCount;
    XmBaseClassExt  bcePtr;
    String  resource_class, resource_name;
    XtPointer  client_data;

    bcePtr = &(iconGBaseClassExtRec );
    client_data = NULL;
    resource_class = NULL;
    resource_name = NULL;
    arrayCount =
      _XmSecondaryResourceData ( bcePtr, data_rtn, client_data,
                resource_name, resource_class,
                GetIconGClassSecResBase);
    return (arrayCount);

}

/*
 * GetIconGClassResBase ()
 *   retrun the address of the base of resources.
 *  If client data is the same as the address of the secndary data in the
 *	class record then send the base address of the cache-resources for this
 *	instance of the widget.
 * Right now we  do not try to get the address of the cached_data from
 *  the Gadget component of this instance - since Gadget class does not
 *	have any cached_resources defined. If later secondary resources are
 *	defined for Gadget class then this routine will have to change.
 */
static XtPointer
GetIconGClassSecResBase(
        Widget widget,
        XtPointer client_data )	/* unused */
{	XtPointer  widgetSecdataPtr;

	widgetSecdataPtr = (XtPointer) (IG_Cache(widget));


    return (widgetSecdataPtr);
}

/************************************************************************
 * InputDispatch
 ************************************************************************/
static	void
InputDispatch(
	Widget	wid,
	XEvent	*event,
	Mask	event_mask)
{

    if (event_mask & XmHELP_EVENT) _XmSocorro(wid,event,NULL,NULL);
    else if (event_mask & XmFOCUS_IN_EVENT)
	_XmFocusInGadget (wid, event, NULL, NULL);
    else if (event_mask & XmFOCUS_OUT_EVENT)
	_XmFocusOutGadget (wid, event, NULL, NULL);
    else if (event_mask & XmENTER_EVENT)
	_XmEnterGadget (wid, event, NULL, NULL);
    else if (event_mask & XmLEAVE_EVENT)
	_XmLeaveGadget (wid, event, NULL, NULL);
}


/************************************************************************
 * GetBaselines
 ************************************************************************/
static	Boolean
GetBaselines(
	Widget		wid,
	Dimension	**baselines,
	int		*line_count)
{
    Dimension *	base_array;
    Position label_y ;

    *line_count = 1;
    base_array = (Dimension *)XtMalloc(sizeof(Dimension));

    GetLabelXY(wid, NULL, &label_y);

    if (IG_LabelString(wid) == NULL) {
	base_array[0] = IG_HLThickness(wid) + label_y ;
    } else {
	base_array[0] = IG_HLThickness(wid) + label_y
	    + DEFAULT_LABEL_MARGIN_HEIGHT
	    + XmStringBaseline(IG_RenderTable(wid), IG_LabelString(wid));
    }

    *baselines = base_array;
    return(True);
}

/************************************************************************
 * GetDisplayRect
 ************************************************************************/
static	Boolean
GetDisplayRect(
	Widget		wid,
	XRectangle	*displayrect)
{
    Dimension w = 0, h = 0 ;

    (*displayrect).x = IG_HLThickness(wid);
    (*displayrect).y = IG_HLThickness(wid);
    GetSize(wid, &w, &h);
    (*displayrect).width = w - 2*IG_HLThickness(wid);
    (*displayrect).height = h - 2*IG_HLThickness(wid) ;

    return(True);
}

/************************************************************************
 * MarginsProc
 ************************************************************************/
static	void
MarginsProc(
	Widget		w,
	XmBaselineMargins *margins_rec)
{
    if (margins_rec->get_or_set == XmBASELINE_GET) {
      margins_rec->margin_top = DEFAULT_LABEL_MARGIN_HEIGHT;
      margins_rec->margin_bottom = DEFAULT_LABEL_MARGIN_HEIGHT;
      margins_rec->shadow = IG_ShadowThickness(w);
      margins_rec->highlight = IG_HLThickness(w);
      margins_rec->text_height = IG_LabelRectHeight(w);
      margins_rec->margin_height = DEFAULT_LABEL_MARGIN_HEIGHT;
    }
}

/*---------------------
| XmIconGadget methods |
---------------------*/

/*------------
| ActionProcs |
------------*/

/*-------------------
| Internal functions |
-------------------*/

/************************************************************************
 * ChangeHighlightGC
 ************************************************************************/
static	void
ChangeHighlightGC(
	Widget	wid,
        XtEnum selection_mode,
	int line_width)
{
    XtGCMask  valueMask;
    XGCValues	values;

    valueMask = GCLineStyle | GCLineWidth | GCDashList | GCCapStyle ;
    values.line_width = line_width;
    values.dashes = MAX(IG_HLThickness(wid), 8);
    values.cap_style = CapProjecting;
    values.line_style = (selection_mode == XmADD_MODE)
			    ? LineDoubleDash
			    : LineSolid;

    XChangeGC(XtDisplay(wid), IG_HighlightGC(wid), valueMask, &values);
}


/************************************************************************
 * UpdateSelectGCs
 ************************************************************************/
static	void
UpdateSelectGCs(
	Widget	wid,
        Pixel select_color)
{
    XGCValues	values;
    XtGCMask	valueMask;
    XFontStruct	*fs = (XFontStruct *)NULL;
    XtGCMask  modifyMask = GCClipMask | GCClipXOrigin | GCClipYOrigin;

   if (IG_SelectedGC(wid))
	XtReleaseGC(XtParent(wid),IG_SelectedGC(wid));
    if (IG_InverseGC(wid))
	XtReleaseGC(XtParent(wid),IG_InverseGC(wid));

    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.graphics_exposures = FALSE;

    /* we need a font becasue the inverse gc is going to be
       used to render some text */
    if (XmeRenderTableGetDefaultFont(IG_RenderTable(wid), &fs)) {
	values.font = fs->fid;
	valueMask |= GCFont;
    }

    /* the select color can take the special value XmREVERSED_GROUND_COLORS,
       which means use parent background as ink for text and parent
       foreground as back for the icon rendering */

    values.background = IG_Foreground(wid) ;
    if (select_color != XmREVERSED_GROUND_COLORS) {
	values.foreground = select_color ;
	IG_InverseGC(wid) = NULL ;
    } else {
	/* we need a GC to hold the parent background as ink */
	XtVaGetValues(XtParent(wid),
		      XmNbackground, &(values.foreground), NULL);
	IG_InverseGC(wid) = XtAllocateGC(XtParent(wid),
					 XtParent(wid)->core.depth,
					 valueMask, &values, modifyMask, 0);

	/* get the foreground for the inversed selected background */
	values.background = IG_Background(wid) ;
	XtVaGetValues(XtParent(wid),
		      XmNforeground, &(values.foreground), NULL);

    }

    IG_SelectedGC(wid) = XtAllocateGC(XtParent(wid),
				      XtParent(wid)->core.depth,
				      valueMask, &values, modifyMask, 0);
}


/************************************************************************
 * UpdateGCs
 ************************************************************************/
static	void
UpdateGCs(
	Widget	wid)
{
    XGCValues	values;
    XtGCMask	valueMask;
    XFontStruct	*fs = (XFontStruct *)NULL;
    XmContainerDataRec container_data ;
    Pixel select_color;
    XtGCMask  modifyMask = GCClipMask | GCClipXOrigin | GCClipYOrigin;

    if (IG_NormalGC(wid))
	XtReleaseGC(XtParent(wid),IG_NormalGC(wid));
    if (IG_InsensitiveGC(wid))
	XtReleaseGC(XtParent(wid),IG_InsensitiveGC(wid));
    if (IG_BackgroundGC(wid))
	XtReleaseGC(XtParent(wid),IG_BackgroundGC(wid));
    if (IG_TopShadowGC(wid))
	XtReleaseGC(XtParent(wid),IG_TopShadowGC(wid));
    if (IG_BottomShadowGC(wid))
	XtReleaseGC(XtParent(wid),IG_BottomShadowGC(wid));
    if (IG_HighlightGC(wid))
	XtReleaseGC(XtParent(wid),IG_HighlightGC(wid));

    /** normal gc **/
    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.foreground = IG_Foreground(wid) ;
    values.background = IG_Background(wid) ;
    values.graphics_exposures = FALSE;

    if (XmeRenderTableGetDefaultFont(IG_RenderTable(wid), &fs)) {
	values.font = fs->fid;
	valueMask |= GCFont;
    }

    IG_NormalGC(wid) = XtAllocateGC(XtParent(wid),
					 XtParent(wid)->core.depth,
					 valueMask, &values, modifyMask, 0);

    /** background gc **/
    values.foreground = IG_Background(wid) ;
    values.background = IG_Foreground(wid) ;

    if (PIXMAP_VALID(IG_BackgroundPixmap(wid))) {
	int depth ;

	XmeGetPixmapData(XtScreen(wid),IG_BackgroundPixmap(wid) ,
			 NULL,
			 &depth,
			 NULL, NULL, NULL, NULL, NULL, NULL);

	if (depth == 1) {
	    valueMask |= GCFillStyle | GCStipple ;
	    values.fill_style = FillOpaqueStippled;
	    values.stipple = IG_BackgroundPixmap(wid);
	} else {
	    valueMask |= GCFillStyle | GCTile ;
	    values.fill_style = FillTiled;
	    values.tile = IG_BackgroundPixmap(wid);
	}

    }

    IG_BackgroundGC(wid) =  XtAllocateGC(XtParent(wid),
					 XtParent(wid)->core.depth,
					 valueMask, &values, modifyMask, 0);

    /** selected gcs **/
    /* use a select color from the trait if possible,
     otherwise default to select background of the parent
     (or is it of the container logical parent ? not really
     important for the header case is not selectable... */

    /* get the container information */
    /* I need the selection mode for the highlight init value */
    container_data.valueMask = ContSelectColor | ContSelectionMode;
    GetContainerData(wid, &container_data);

    if (container_data.valueMask & ContSelectColor) {
	select_color = container_data.select_color;
    } else {
	select_color = XmREVERSED_GROUND_COLORS ;
    }

    UpdateSelectGCs(wid, select_color) ;


    /** insensitive gc **/

	/*generally gray insensitive foreground (instead stipple)*/
    values.foreground = _XmAssignInsensitiveColor( wid);
    values.background = IG_Background(wid) ;
    valueMask |= GCFillStyle;
    values.fill_style = FillSolid;

    IG_InsensitiveGC(wid) = XtAllocateGC(XtParent(wid),
					 XtParent(wid)->core.depth,
					 valueMask, &values, modifyMask, 0);

	/*light shadow for insensitive text (instead stipple)*/
	values.foreground = IG_TopShadowColor(wid);
	IG_ShadowGC(wid) = XtAllocateGC(XtParent(wid),
					XtParent(wid)->core.depth,
					 valueMask, &values, modifyMask, 0);

    /** highlight **/

    valueMask = (GCForeground | GCBackground | GCLineWidth |
		 GCLineStyle | GCDashList);
    modifyMask = (GCLineStyle | GCLineWidth | GCDashList |
		  GCClipXOrigin | GCClipYOrigin | GCClipMask);

    values.foreground = IG_HighlightColor(wid);
    XtVaGetValues(XtParent(wid), XmNbackground, &(values.background), NULL);
    values.line_width = IG_HLThickness(wid);
    values.dashes = MAX(values.line_width, 8);
    values.line_style = (container_data.selection_mode == XmADD_MODE) ?
      LineDoubleDash : LineSolid;

    IG_HighlightGC(wid) = XtAllocateGC(XtParent(wid),
				       XtParent(wid)->core.depth,
				       valueMask, &values,
				       modifyMask, 0);

    /** topshadow, bottomshadow gc */
    IG_TopShadowGC(wid) =
	_XmGetPixmapBasedGC (wid,
			     IG_TopShadowColor(wid),
			     IG_Background(wid),
			     IG_TopShadowPixmap(wid));

     IG_BottomShadowGC(wid) =
	_XmGetPixmapBasedGC (wid,
			     IG_BottomShadowColor(wid),
			     IG_Background(wid),
			     IG_BottomShadowPixmap(wid));

}



/************************************************************************
 * GetStringTableReOrdered.
 *  lazy alloc/filling using realloc
 * --- Never free the returned array.---
 ************************************************************************/
static	XmStringTable
GetStringTableReOrdered(
	XmStringTable st,
        Cardinal st_count,
        Cardinal * order,
        Cardinal order_count)
{
    static XmString * Default_st = NULL ;
    static Cardinal Max_st_count = 0;
    Cardinal i, count ;

    if (!order_count || !st_count) return NULL ;

    /* here we are filling up a new string table out of an existing
       one and a new order table. Take only the minimum number of both */
    count = MIN(order_count, st_count);

    if (count > Max_st_count) {
	Max_st_count = MAX(count, 33);
	Default_st = (XmStringTable) XtRealloc((char*) Default_st,
					Max_st_count * sizeof(XmString));
    }

    for (i = 0; i < count; i++) {
	if (order) {
	    if (order[i] <= st_count)
		Default_st[i] = st[order[i] - 1];
	    else
		Default_st[i] = NULL ;
	} else {
	    Default_st[i] = st[i];
	}
    }

    return Default_st ;
    /* This is realloced memory, be sure that no one is keeping
       reference to this stuff longer enough for it to be realloced again */
}


/************************************************************************
 * GetStringTableExtent
 *
	1> If the XmTablist has exactly as many entries as
	   XmNdetailOrderCount, the last entry is clipped, and consequently,
	   all IconGadgets will ask for the same size.
	2> If the XmTablist is greater than XmNdetailOrderCount, the extra
	   tabs are ignored
	3> if the XmTablist is XmNdetailOrderCount-1, then the last detail
	   is not clipped, but extends as far as needed.  The IconGadget
	   geometry request will be for the full size.  The Container
	   will ask for the maximum width requested by all its IconGadgets.
	4> If the XmTablist is less than XmNdetailOrderCount-1, then,
	   concatenate the extra details at the end and request
	   the additional width.

************************************************************************/
static	void
GetStringTableExtent(
	Screen * screen,
        XmStringTable st,
        Cardinal st_count,
        XmRenderTable render_table,
        XmTabList tab_list,
        Dimension hor_spacing,  /* for the tab-less concat details */
        Dimension * width_ret,
        Dimension * height_ret,
        Position * baseline_ret)
{
    Cardinal i ;
    Dimension w, h, baseline ;
    unsigned int tab_count = 0 ;
    Dimension height_under_max = 0 ;

    *baseline_ret = 0 ;
    *width_ret = 0 ;
    *height_ret = 0 ;
    if (tab_list) tab_count = XmTabListTabCount(tab_list) ;

    if (st == NULL || !st_count) return ;

    /* the extra tabs are ignored */
    tab_count = MIN(tab_count, st_count);

    /* the width is given by the last tab position + the remaining items */
    if (tab_count) *width_ret = _XmTabListGetPosition(screen,
						     tab_list,
						     XmPIXELS, tab_count-1);
    /* the height is the sum of max baseline + max height under baseline */
    for (i = 0; i < st_count; i++) {
	if (st[i]) {
	    XmStringExtent(render_table, st[i], &w, &h);
	    baseline = XmStringBaseline(render_table, st[i]);
	} else {
	    h = 0 ;
	    w = 0 ;
	    baseline = 0 ;
	}
	height_under_max = MAX(height_under_max, h - baseline);
	*baseline_ret = MAX(*baseline_ret, (Position)baseline);
	if (i >= tab_count) *width_ret += w + hor_spacing;
    }

    *height_ret = *baseline_ret + height_under_max ;
}



/************************************************************************
 * GetSize
 ************************************************************************/
static	void
GetSize(
	Widget		wid,
	Dimension *	ret_width,
	Dimension *	ret_height)
{
    XmContainerDataRec container_data ;
    Dimension	ht = IG_HLThickness(wid);
    Dimension mw = IG_MarginWidth(wid), mh = IG_MarginHeight(wid);
    int ideal_width, ideal_height;
    Dimension detail_width, detail_height;
    XmStringTable new_detail ;
    Position label_y, lab_baseline, detail_baseline ;

    /* get the container information */
    container_data.valueMask = ContAllValid ;
    GetContainerData(wid, &container_data);


    /**** first the size without the detail */
    ideal_width = GetIconLabelWidth(wid) ;
    ideal_height = GetIconLabelHeight(wid) ;


    if (SHOW_DETAIL(wid, &container_data)) {
	/* get the detail table to be displayed */
	new_detail =
	    GetStringTableReOrdered(IG_Detail(wid), IG_DetailCount(wid),
				    container_data.detail_order,
				    container_data.detail_order_count);
	/* ask for its extent using the provided container tab list */
	/* new_detail might be NULL at this point, extent should
	   return 0 size if so. some new_detail[i] might also be NULL,
	   in which case extent should go to the next tab. note that
	   tab might  also be NULL. */
	GetStringTableExtent(XtScreen(wid),
			     new_detail, MIN(container_data.detail_order_count,
					     IG_DetailCount(wid)),
			     IG_RenderTable(wid),
			     container_data.detail_tablist,
			     DEFAULT_HOR_SPACING,
			     &detail_width, &detail_height, &detail_baseline);
	/* width in detail is sum of first_column_width (where
	   the x has been removed already) + the detail width */
	ideal_width = container_data.first_column_width + (int)detail_width
	    + mw - ht;
	/* the baseline have to be taken into account in the calculation
	   of the height needed */
	GetLabelXY(wid, NULL, &label_y);
	lab_baseline = XmStringBaseline(IG_RenderTable(wid),
					 IG_LabelString(wid));
	ideal_height = MAX(ideal_height,
			   label_y - ht + DEFAULT_LABEL_MARGIN_HEIGHT +
			   lab_baseline - mh -
			   detail_baseline + detail_height);
    }

    /* do not set non null dimensions */
    if (*ret_width == 0) *ret_width = ideal_width + 2*ht;
    if (*ret_height == 0) *ret_height = ideal_height + 2*ht;

}


/*-------------------
| Trait methods     |
-------------------*/
/************************************************************************
 * ContItemSetValues
 *
 * Deal with a container setting new attributes on me
 ************************************************************************/
static	void
ContItemSetValues(Widget w,
		  XmContainerItemData contItemData)
{
    XtExposeProc    expose;

    /* here there is a match between the containerItem and the IconGadget
       resource values, because we are doing both at the same time and it's
       convenient. Others IconGadget kind might have to map our
       viewType, visualEmphasis values onto their corresponding types */

    if (contItemData->valueMask & ContItemViewType)
	XtVaSetValues(w, XmNviewType, contItemData->view_type, NULL);

    if (contItemData->valueMask & ContItemVisualEmphasis) {
	IG_VisualEmphasis(w) = contItemData->visual_emphasis ;

	if (XtIsRealized(XtParent(w)))
	{
	  _XmProcessLock();
	  expose = w->core.widget_class->core_class.expose;
	  _XmProcessUnlock();

	  (* (expose)) (w, NULL, NULL);
	}
    }
}


/************************************************************************
 * ContItemGetValues
 *
 ************************************************************************/
static	void
ContItemGetValues(Widget w,
		  XmContainerItemData contItemData)
{

    if (contItemData->valueMask & ContItemViewType)
	contItemData->view_type = IG_ViewType(w);
    if (contItemData->valueMask & ContItemVisualEmphasis)
	contItemData->visual_emphasis = IG_VisualEmphasis(w);
    if (contItemData->valueMask & ContItemIconWidth) {
	contItemData->icon_width = 2*IG_HLThickness(w) + GetIconLabelWidth(w);
    }
    if (contItemData->valueMask & ContItemDetailCount) {
	contItemData->detail_count = IG_DetailCount(w);
    }

}


static Boolean
HandleRedraw (
	Widget kid,
	Widget cur_parent,	/* unused */
	Widget new_parent,	/* unused */
	Mask visual_flag)
{
    XmIconGadget ig = (XmIconGadget) kid ;
    Boolean redraw = False;
    XmIconGCacheObjPart oldCopy;


    if (visual_flag & VisualSelectColor) {
	XmContainerDataRec container_data ;
	Pixel select_color;

	/* this is all shared data, so we need to make a copy
	   because changing any field */
	_XmProcessLock();
	_XmCacheCopy((XtPointer) IG_Cache(ig), (XtPointer) &oldCopy,
		     sizeof(XmIconGCacheObjPart));
	_XmCacheDelete ((XtPointer) IG_Cache(ig));
	_XmProcessUnlock();
	IG_Cache(ig) = &oldCopy;


	/* use a select color from the trait if possible,
	   otherwise default to select background of the parent
	   (or is it of the container logical parent ? not really
	   important for the header case is not selectable... */

	/* get the container information */
	/* I need the selection mode for the highlight init value */
	container_data.valueMask = ContSelectColor ;
	GetContainerData(kid, &container_data);

	if (container_data.valueMask & ContSelectColor) {
	    select_color = container_data.select_color;
	} else {
	    select_color = XmREVERSED_GROUND_COLORS ;
	}

	UpdateSelectGCs(kid, select_color) ;

	redraw = True;

	/* now cache back the new version */
	_XmProcessLock();
	IG_Cache(ig) = (XmIconGCacheObjPart *)
	    _XmCachePart( IG_ClassCachePart(ig),
			 (XtPointer) IG_Cache(ig),
			 sizeof(XmIconGCacheObjPart));
	_XmProcessUnlock();

    }

    return redraw ;
}


static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
    XmContainerDataRec container_data ;

    /* since we use our own private converter, no real
       need to check for validity here, we already do it
       in the converter itself, but it doesn't hurt
       and if we ever change IconG to use a generic pixmap
       converter (solve the mask pixmap setting), it will work
       all by itself */

    if (IG_Cache(w)) { /* mean it's valid */
	color_data->valueMask = AccessForeground | AccessBackgroundPixel |
	    AccessHighlightColor | AccessTopShadowColor |
		AccessBottomShadowColor | AccessSelectColor;

	color_data->background = IG_Background(w);
	color_data->foreground = IG_Foreground(w);
	color_data->highlight_color = IG_Foreground(w);
	color_data->top_shadow_color = IG_TopShadowColor(w);
	color_data->bottom_shadow_color = IG_BottomShadowColor(w);

	container_data.valueMask = ContSelectColor ;

	/*** get the Container select color using the trait */
	{
	  XmIconGadgetClass igc = (XmIconGadgetClass) XtClass(w);
	  Widget container_id ;
	  XmContainerTrait container_trait ;

	  container_id = (igc->icong_class.get_container_parent) ?
	    (*(igc->icong_class.get_container_parent))(w) : XtParent(w);

	  container_trait = (XmContainerTrait)
	    XmeTraitGet((XtPointer)XtClass(container_id), XmQTcontainer) ;

	  /* remove message about uninitialized memory read */
	  container_data.first_column_width = 0;
	  container_data.select_color = XmREVERSED_GROUND_COLORS;
	  if (container_trait)
	    container_trait->getValues(container_id, &container_data);
	}
	if (container_data.valueMask & ContSelectColor) {
	    color_data->select_color = container_data.select_color;
	} else {
	    color_data->select_color = XmREVERSED_GROUND_COLORS ;
	}

    } else { /* cannot access the color of the gadget,
		because the cache is not yet present.
		Return that value, so that the converter
		(probably the one calling it)
		can take the appropriate step */
	color_data->valueMask = AccessColorInvalid ;
    }
}


static Boolean
PointIn(Widget wid,
	Position x,
        Position y)
{
    XmContainerDataRec container_data ;

    /* x, y in parent coordinates system */

    /* first check that point is in bbox */
    if (!(x >= wid->core.x && y >= wid->core.y &&
	x < wid->core.x + wid->core.width    &&
	y < wid->core.y + wid->core.height))
	return False;

    /* if we have detail then it's enough to answer yes */
    container_data.valueMask = ContAllValid ;
    GetContainerData(wid, &container_data);
    if (SHOW_DETAIL(wid, &container_data))
	return True;
    else {
	Position label_x, label_y;
	XPoint points[8];
	Cardinal n;

	/* now check if it's not in empty space at the corners */
	GetLabelXY(wid, &label_x, &label_y);

	/* undo the ltr layout for label_x, because getshapeinfo does it */
	if (LayoutIsRtoLG(wid)) {
	    label_x = XtWidth(wid) - IG_LabelRectWidth(wid) - label_x;
	}
	n = GetShapeInfo(wid, GetLargeIconX(wid), GetSmallIconY(wid),
			 label_x, label_y,
			 container_data.first_column_width, INVALID_DIMENSION,
			 points);
	if (n == 2) {
	    return (x >= points[0].x && x <= points[1].x &&
		    y >= points[0].y && y <= points[1].y);
	} else if (n == 8) {
	    if (IG_ViewType(wid) == XmLARGE_ICON) {
		Cardinal p0, p2, p3, p5;
		if (!LayoutIsRtoLG(wid)) {
		    p0 = 0; p2 = 2; p3 = 3; p5 = 5;
		} else {
		    p0 = 5; p2 = 3; p3 = 2; p5 = 0;
		}
		return (y <= points[1].y && /* pixmap row */
			x >= points[p2].x && x <= points[p3].x) ||
			    (y >= points[1].y && /* label row */
			     x >= points[p0].x && x <= points[p5].x);
	    } else {		/* SMALL_ICON */
		return (((!LayoutIsRtoLG(wid) && x <= points[1].x) ||
			 (LayoutIsRtoLG(wid) && x >= points[1].x)) &&
			/* pixmap column */
			y >= points[0].y && y <= points[7].y) ||
			    (((!LayoutIsRtoLG(wid) && x >= points[1].x) ||
			      (LayoutIsRtoLG(wid) && x <= points[1].x)) &&
			     /* label row */
			     y >= points[2].y && y <= points[5].y);
	    }
	}
	return True;		/* actually we should never get there */
    }
}


/*-------------------
| _Xm functions      |
--------------------*/
void
_XmIconGadgetIconPos(Widget wid, int *x, int *y)
{
  XmIconGadget	ig = (XmIconGadget)wid;
  Position icon_x;

  if (ig -> icong.viewtype == XmSMALL_ICON) {
    icon_x = GetSmallIconX(wid);
    if (LayoutIsRtoLG(wid))
	icon_x = XtWidth(wid) - icon_x - IG_SmallIconRectWidth(wid);
    *x = icon_x;
    *y = GetSmallIconY(wid);
  } else {
    icon_x = GetLargeIconX(wid);
    if (LayoutIsRtoLG(wid))
	icon_x = XtWidth(wid) - icon_x - IG_LargeIconRectWidth(wid);
    *x = icon_x;
    *y = GetLargeIconY(wid);
  }
}


/*-------------------
| External functions |
-------------------*/
/************************************************************************
 * XmCreateIconGadget
 *
 ************************************************************************/
Widget
XmCreateIconGadget(
	Widget		parent,
	char		*name,
	ArgList		arglist,
	Cardinal	argcount)
{
    return(XtCreateWidget(name,xmIconGadgetClass,parent,arglist,argcount));
}

Widget
XmVaCreateIconGadget(
        Widget parent,
        char *name,
        ...)
{
    Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmIconGadgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedIconGadget(
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
                         xmIconGadgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
