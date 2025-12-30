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

#include <stdlib.h>
#include <Xm/AccTextT.h>
#include <Xm/ActivatableT.h>
#include <Xm/ArrowBG.h>
#include <Xm/CareVisualT.h>
#include <Xm/JoinSideT.h>
#include <Xm/NavigatorT.h>
#include <Xm/NotebookP.h>
#include <Xm/PushBGP.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/SpinB.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/TravConT.h>
#include <Xm/UnhighlightT.h>
#include <Xm/VendorSEP.h>
#include <Xm/VaSimpleP.h>
#include "CareVisualTI.h"
#include "ColorI.h"
#include "GeoUtilsI.h"
#include "GMUtilsI.h"
#include "MessagesI.h"
#include "RegionI.h"
#include "RepTypeI.h"
#include "ScrollFramTI.h"
#include "XmI.h"

#define MESSAGE0     _XmMMsgNotebook_0000
#define MESSAGE1     _XmMMsgMotif_0001


/* Under SunOS /usr/include/sys/sysmacros.h (included by types.h) */
/*	thoughtfully grabs part of our name space. */
#ifdef major
#undef major
#endif

#ifdef minor
#undef minor
#endif


/*****************************************************************************
 *                                                                           *
 *                        constants & useful macros                          *
 *                                                                           *
 *****************************************************************************/


/* names of notebook internal children */
#define MAJOR_TAB_NEXT_NAME     "MajorTabScrollerNext"
#define MAJOR_TAB_PREV_NAME     "MajorTabScrollerPrevious"
#define MINOR_TAB_NEXT_NAME     "MinorTabScrollerNext"
#define MINOR_TAB_PREV_NAME     "MinorTabScrollerPrevious"
#define PAGE_SCROLLER_NAME      "PageScroller"
#define NB_TEXT_FIELD_NAME 	"NBTextField"

/* status values for notebook.scroller_status */
#define DEFAULT_NONE    	0	/* default is not created yet */
#define DEFAULT_CREATE  	1	/* default being created */
#define DEFAULT_USED    	2	/* default is being used */
#define DEFAULT_GONE    	3	/* default is destroyed */

/* Notebook position value */
#define LEFT                    0
#define RIGHT                   1
#define TOP                     2
#define BOTTOM                  4

/* tab traversal value */
#define _HOME                   0
#define _END                    1
#define _NEXT                   2
#define _PREVIOUS               3
#define	_FIRST_VISIBLE		4	/* For KNextTab/KPrevTab, scrolling */
#define	_LAST_VISIBLE		5	/* For tab scrolling */
#define	_CURRENT_VISIBLE	6	/* For tab group traversal */

/* status values for drawing minor tabs */
#define TAB_DRAW        0               /* tab is to be drawn */
#define TAB_DRAWING     1               /* tab is being drawn */
#define TAB_DRAWN       2               /* tab has been drawn */


/* XmNotebook default values */
#define FIRST_PAGE_NUMBER_DEFAULT	1
#define BACK_PAGE_NUMBER_DEFAULT	2
#define BACK_PAGE_SIZE_DEFAULT		8
#define BINDING_WIDTH_DEFAULT		25
#define MARGIN_WIDTH_DEFAULT		0
#define MARGIN_HEIGHT_DEFAULT		0
#define MAJOR_TAB_SPACING_DEFAULT	3
#define MINOR_TAB_SPACING_DEFAULT	3
#define TAB_SCROLLER_WIDTH_DEFAULT	20
#define TAB_SCROLLER_HEIGHT_DEFAULT	20
#define NOTEBOOK_MINIMUM_WIDTH		16
#define NOTEBOOK_MINIMUM_HEIGHT		16
#define MIN_DRAWABLE_SPIRAL_SIZE	6

/* macros */

#define NB_IS_CHILD_NONE(x) \
        ((unsigned char)x == XmNONE)
#define NB_IS_CHILD_PAGE(x) \
        ((unsigned char)x == XmPAGE)
#define NB_IS_CHILD_MAJOR(x) \
        ((unsigned char)x == XmMAJOR_TAB)
#define NB_IS_CHILD_MINOR(x) \
        ((unsigned char)x == XmMINOR_TAB)
#define NB_IS_CHILD_TAB(x) \
        (((unsigned char)x == XmMAJOR_TAB) \
        || ((unsigned char)x == XmMINOR_TAB))
#define NB_IS_CHILD_STATUS(x) \
        ((unsigned char)x == XmSTATUS_AREA)
#define NB_IS_CHILD_PAGE_SCROLLER(x) \
        ((unsigned char)x == XmPAGE_SCROLLER)
#define NB_IS_CHILD_MAJOR_SCROLLER(x) \
        (((unsigned char)x == XmMAJOR_TAB_SCROLLER) \
        || ((unsigned char)x == XmTAB_SCROLLER))
#define NB_IS_CHILD_MINOR_SCROLLER(x) \
        (((unsigned char)x == XmMINOR_TAB_SCROLLER) \
        || ((unsigned char)x == XmTAB_SCROLLER))


/* Layout defines */
#define NB_IS_VISIBLE(w) \
      ((XtIsManaged(w) && \
      ((w)->core.x > -(int)((w)->core.width + (w)->core.border_width * 2)) && \
      ((w)->core.y > -(int)((w)->core.height + (w)->core.border_width * 2))))

#define	NB_IS_HIDDEN(w) \
    (((w)->core.x <= -(int)((w)->core.width + (w)->core.border_width * 2)) && \
     ((w)->core.y <= -(int)((w)->core.height + (w)->core.border_width * 2)))

#define NB_ENFORCE_SIGN(s,x) ((s) > 0 ? (x) : -(x))

				/* Fixed placement of non-top minor tabs */
#define NB_MINOR_TAB_STEP(x) ((x) /2)

				/* Max dimension of major tab area, i.e.,
				   max of tab and tab scroller dimension */
#define NB_MAJOR_MAX(n,x,y) (MAX((x), (y)))
 /* MAX(0, (n)->notebook.shadow_thickness - (n)->notebook.back_page_size)) */

				/* Max dimension of minor tab area, i.e.,
				   max of tab (minus fixed placement) and
				   tab scroller dimension */
#define NB_MINOR_MAX(n,x,y) \
 (MAX((int)((x) - NB_MINOR_TAB_STEP((n)->notebook.back_page_size)),(int)(y)))
 /* MAX(0, (n)->notebook.shadow_thickness - (n)->notebook.back_page_size)) */


/* Retrieve trait defines */
#define NB_SCROLL_FRAME(x) \
    ((XmScrollFrameTrait)XmeTraitGet((XtPointer)XtClass((Widget)(x)), \
    XmQTscrollFrame))

#define NB_ACTIVATABLE(x) \
    ((XmActivatableTrait)XmeTraitGet((XtPointer)XtClass((Widget)(x)), \
    XmQTactivatable))

#define NB_NAVIGATOR(x) \
    ((XmNavigatorTrait)XmeTraitGet((XtPointer)XtClass((Widget)(x)), \
    XmQTnavigator))

#define NB_JOINSIDE(x) \
    ((XmJoinSideTrait)XmeTraitGet((XtPointer)XtClass((Widget)(x)), \
    XmQTjoinSide))


/* Is trait defines */
#define NB_IS_CHILD_ACTIVATABLE(x) \
    (((XmeTraitGet((XtPointer)XtClass(x),XmQTactivatable)) != NULL) \
    ? True : False)

#define NB_IS_CHILD_NAVIGATOR(x) \
    (((XmeTraitGet((XtPointer)XtClass(x),XmQTnavigator)) != NULL) \
    ? True : False)

#define NB_IS_CHILD_JOINSIDE(x) \
    (((XmeTraitGet((XtPointer)XtClass(x),XmQTjoinSide)) != NULL) \
    ? True : False)

#define NB_IS_CHILD_ACCESSTEXTUAL(x) \
    (((XmeTraitGet((XtPointer)XtClass(x),XmQTaccessTextual)) != NULL) \
    ? True : False)


#define NotebookConstraint(w) \
        (&((XmNotebookConstraintPtr) (w)->core.constraints)->notebook)


/* If the original dimension is negative, then offset the original
   position by that negative amount and make it the new position. Also
   make the new dimension be the positive original dimension. So for
   instance, if the x value is 20 and the width is -10, then make the x
   value be 10 and the width be positive 10. If the original dimension
   is not negative, then just set the new values to be the same as the
   old values. */

#define ADJUST_NEGATIVE_DIMENSION(orig_pos, orig_dim, new_pos, new_dim) \
        if (orig_dim < 0) \
        { \
	    new_pos = orig_pos + orig_dim; \
	    new_dim = (Dimension) (orig_dim * -1) ; \
	} \
        else \
        { \
	    new_pos = orig_pos; \
	    new_dim = (Dimension) orig_dim; \
	}


#define manager_translations _XmNotebook_manager_translations


/*****************************************************************************
 *                                                                           *
 *                               global variables                            *
 *                                                                           *
 *****************************************************************************/


static XtAccelerators TabAcceleratorsParsed;


/*****************************************************************************
 *                                                                           *
 *                           Static function declarations                    *
 *                                                                           *
 *****************************************************************************/



/* synthetic hook */
static void GetDefaultFrameBackground(
                        Widget g,
                        int offset,
                        XrmValue *value) ;
static void GetDefaultBackPageBackground(
                        Widget g,
                        int offset,
                        XrmValue *value) ;
static void GetDefaultBackPagePos(
			Widget w,
			int offset,
			XrmValue *value) ;
static void FromOrientationPixels(
			Widget w,
                        int offset,
			XtArgVal *value) ;
static XmImportOperator ToOrientationPixels(
			Widget w,
			int offset,
			XtArgVal *value) ;
static void FromOrientationOppositePixels(
			Widget w,
			int offset,
			XtArgVal *value) ;
static XmImportOperator ToOrientationOppositePixels(
			Widget w,
			int offset,
			XtArgVal *value) ;
/* core class method */
static void ClassInitialize (
			void) ;
static void ClassPartInitialize(
			WidgetClass wc) ;
static void Initialize(
			Widget rw,
			Widget nw,
			ArgList args,
			Cardinal *num) ;
static void Destroy(
			Widget w) ;
static void Resize(
			Widget w) ;
static void Redisplay(
			Widget w,
			XEvent *e,
			Region region) ;
static Boolean SetValues(
			Widget ow,
			Widget rw,
			Widget nw,
			ArgList args,
			Cardinal *num) ;
static XtGeometryResult QueryGeometry(
			Widget w,
			XtWidgetGeometry *intended,
			XtWidgetGeometry *desired) ;
static XtGeometryResult GeometryManager(
			Widget instigator,
			XtWidgetGeometry *desired,
			XtWidgetGeometry *reply) ;
static void ChangeManaged(
			Widget w) ;
static void InsertChild(
			Widget child) ;
static void DeleteChild(
			Widget child) ;
static void ConstraintInitialize(
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num) ;
static void CreateTabScrollers(
			XmNotebookWidget nb) ;
static void SetVisualConfig(
			XmNotebookWidget nb) ;
static Boolean UpdateJoinSide(
			XmNotebookWidget nb,
			Widget child,
			unsigned char child_type,
			Dimension shadow_thickness) ;
static void UpdateJoinSideChildren(
			XmNotebookWidget nb,
			Dimension shadow_thickness) ;
static void SetPageScroller(
			XmNotebookWidget nb) ;
static void LayoutChildren(
			XmNotebookWidget nb,
			Widget instigator) ;
static void LayoutPages(
			XmNotebookWidget nb,
			Widget instigator) ;
static void LayoutMajorTabs(
			XmNotebookWidget nb,
			Widget instigator) ;
static void LayoutMinorTabs(
			XmNotebookWidget nb,
			Widget instigator) ;
static void ResetTopPointers(
			XmNotebookWidget nb,
			unsigned char reason,
			int scroll) ;
static void GetFrameGCs(
			XmNotebookWidget nb) ;
static void GetBackpageGCs(
			XmNotebookWidget nb) ;
static void DrawBinding(
			XmNotebookWidget nb,
			XExposeEvent *event,
			Region region) ;
static void MakeSpiralPixmap(
			XmNotebookWidget nb,
			Dimension width,
			Dimension height) ;
static void DrawPixmapBinding(
			XmNotebookWidget nb,
			Dimension x,
			Dimension y,
			Dimension width,
			Dimension height,
                        Pixmap pixmap) ;
static void DrawFrameShadow(
			XmNotebookWidget nb,
			XExposeEvent *event,
			Region region) ;
static void DrawBackPages(
			XmNotebookWidget nb,
			XExposeEvent *event,
			Region region) ;
static Boolean NewPreferredGeometry(
			XmNotebookWidget nb,
			Widget instigator,
			XtWidgetGeometry *desired,
			XtWidgetGeometry *preferred) ;
static void AdjustGeometry(
			XmNotebookWidget nb,
			Widget instigator,
                	XtWidgetGeometry *desired) ;
static Boolean SetLastPageNumber (
			XmNotebookWidget nb,
    			int last_page_number) ;
static Boolean AssignDefaultPageNumber(
			XmNotebookWidget nb) ;
static Boolean ConstraintSetValues(
			Widget old_w,
			Widget req,
			Widget new_w,
			ArgList args,
			Cardinal *num) ;
static void SetActiveChildren(
			XmNotebookWidget nb) ;
static void RepositionChild(
			XmNotebookWidget nb,
			Widget child) ;
static void SortChildren(
			XmNotebookWidget nb) ;
static int GetNextAvailPageNum(
			XmNotebookWidget nb,
			int last) ;
static Widget GetChildWidget(
			XmNotebookWidget nb,
			int page_number,
			unsigned char child_type) ;
static void GotoPage(
			XmNotebookWidget nb,
			int page_number,
			XEvent *event,
			int reason) ;
static void ShowChild(
			Widget child,
			Widget instigator,
			int x,
			int y,
			int width,
			int height) ;
static void HideChild(
			Widget child,
			Widget instigator) ;
static void HideShadowedTab(
			XmNotebookWidget nb,
			Widget child) ;
static void FlipTabs(
			Widget w,
			XtPointer data,
			XtPointer call_data) ;
static void TabPressed(
			Widget w,
			XtPointer data,
			XtPointer call_data) ;
static void TraverseTab(
			Widget w,
			XEvent *event,
			String *params,
			Cardinal *num_params) ;
static Widget GetNextTab(
			XmNotebookWidget nb,
			unsigned char child_type,
			int page_number,
			unsigned char direction) ;
static Boolean MaxIsRightUp(
			XmNotebookWidget nb,
			unsigned char child_type) ;
static void ScrollFrameInit(
			Widget w,
			XtCallbackProc moveCB,
			Widget scrollable) ;
static Boolean ScrollFrameGetInfo(
			Widget w,
			Cardinal *dimension,
			Widget **nav_list,
			Cardinal *num_nav_list) ;
static void AddNavigator(
			Widget w,
			Widget nav,
			Mask dimMask) ;
static void RemoveNavigator(
			Widget w,
			Widget nav) ;
static void PageMove(
			Widget w,
			XtPointer data,
			XtPointer call_data) ;
static Widget RedirectTraversal(
			Widget old_focus,
			Widget new_focus,
			unsigned int focus_policy,
			XmTraversalDirection direction,
			unsigned int pass) ;
static void UpdateNavigators(
			XmNotebookWidget nb) ;
static GC GetUnhighlightGC(
			Widget w,
			Widget child) ;
static unsigned char  GetDefaultBackPagePosAgain(
			Widget w);


/*****************************************************************************
 *                                                                           *
 *               		Action Records				     *
 *                                                                           *
 *****************************************************************************/


static XtActionsRec tab_actions_list[] =
{
    {"TraverseTab",		TraverseTab},
};


/*****************************************************************************
 *                                                                           *
 *                		Resources                                    *
 *                                                                           *
 *****************************************************************************/


static XtResource resources[]  =
{
    {
	XmNcurrentPageNumber,
	XmCCurrentPageNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(XmNotebookRec, notebook.current_page_number),
	XmRImmediate,
	(XtPointer)XmUNSPECIFIED_PAGE_NUMBER
    },
    {
	XmNfirstPageNumber,
	XmCFirstPageNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(XmNotebookRec, notebook.first_page_number),
	XmRImmediate,
	(XtPointer)FIRST_PAGE_NUMBER_DEFAULT,
    },
    {
	XmNlastPageNumber,
	XmCLastPageNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(XmNotebookRec, notebook.last_page_number),
	XmRImmediate,
	(XtPointer)XmUNSPECIFIED_PAGE_NUMBER
    },
    {
	XmNorientation,
	XmCOrientation,
        XmROrientation,
        sizeof(unsigned char),
        XtOffsetOf(XmNotebookRec, notebook.orientation),
        XmRImmediate,
        (XtPointer)XmHORIZONTAL
    },
    {
	XmNframeBackground,
	XmCFrameBackground,
	XmRPixel,
	sizeof(Pixel),
	XtOffsetOf(XmNotebookRec, notebook.frame_background),
	XmRCallProc,
	(XtPointer) GetDefaultFrameBackground
    },
    {
	XmNbackPagePlacement,
        XmCBackPagePlacement,
        XmRScrollBarPlacement,
        sizeof(unsigned char),
        XtOffsetOf(XmNotebookRec, notebook.back_page_pos),
        XmRCallProc,
        (XtPointer)GetDefaultBackPagePos
    },
    {
	XmNbackPageNumber,
	XmCBackPageNumber,
	XmRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(XmNotebookRec, notebook.back_page_number),
	XmRImmediate,
	(XtPointer)BACK_PAGE_NUMBER_DEFAULT
    },
    {
	XmNbackPageSize,
	XmCBackPageSize,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.back_page_size),
	XmRImmediate,
	(XtPointer)BACK_PAGE_SIZE_DEFAULT
    },
    {
	XmNbackPageForeground,
        XmCBackPageForeground,
        XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmNotebookRec, notebook.back_page_foreground),
        XmRCallProc,
        (XtPointer)_XmForegroundColorDefault
    },
    {
	XmNbackPageBackground,
        XmCBackPageBackground,
        XmRPixel,
        sizeof(Pixel),
        XtOffsetOf(XmNotebookRec, notebook.back_page_background),
        XmRCallProc,
        (XtPointer)GetDefaultBackPageBackground
    },
    {
	XmNbindingType,
        XmCBindingType,
        XmRBindingType,
        sizeof(unsigned char),
        XtOffsetOf(XmNotebookRec, notebook.binding_type),
        XmRImmediate,
        (XtPointer)XmSPIRAL
    },
    {
	XmNbindingPixmap,
	XmCBindingPixmap,
        XmRDynamicPixmap,
        sizeof(Pixmap),
        XtOffsetOf(XmNotebookRec, notebook.binding_pixmap),
        XmRImmediate,
        (XtPointer)XmUNSPECIFIED_PIXMAP
    },
    {
	XmNbindingWidth,
	XmCBindingWidth,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.binding_width),
	XmRImmediate,
	(XtPointer)BINDING_WIDTH_DEFAULT
    },
    {
	XmNinnerMarginWidth,
	XmCInnerMarginWidth,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.margin_width),
	XmRImmediate,
	(XtPointer)MARGIN_WIDTH_DEFAULT
    },
    {
	XmNinnerMarginHeight,
	XmCInnerMarginHeight,
	XmRVerticalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.margin_height),
	XmRImmediate,
	(XtPointer)MARGIN_HEIGHT_DEFAULT
    },
    {
	XmNmajorTabSpacing,
	XmCMajorTabSpacing,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.major_spacing),
	XmRImmediate,
	(XtPointer)MAJOR_TAB_SPACING_DEFAULT
    },
    {
	XmNminorTabSpacing,
	XmCMinorTabSpacing,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.minor_spacing),
	XmRImmediate,
	(XtPointer)MINOR_TAB_SPACING_DEFAULT
    },
    {
	XmNframeShadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(XmNotebookRec, notebook.shadow_thickness),
	XmRCallProc,
	(XtPointer) _XmSetThickness
    },
    {
	XmNpageChangedCallback,
        XmCCallback,
        XmRCallback,
        sizeof(XtCallbackList),
        XtOffsetOf(XmNotebookRec, notebook.page_change_callback),
        XmRPointer,
        (XtPointer)NULL
    }
};


static XtResource constraint_resources[] =
{
    {
	XmNnotebookChildType,
	XmCNotebookChildType,
        XmRNotebookChildType,
	sizeof(unsigned char),
	XtOffsetOf(XmNotebookConstraintRec, notebook.child_type),
	XmRImmediate,
	(XtPointer)XmNONE
    },
    {
	XmNpageNumber,
	XmCPageNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(XmNotebookConstraintRec, notebook.page_number),
	XmRImmediate,
	(XtPointer)XmUNSPECIFIED_PAGE_NUMBER
    },
    {
	XmNresizable,
	XmCResizable,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(XmNotebookConstraintRec, notebook.resizable),
	XmRImmediate,
	(XtPointer)True
    }
};


static XmSyntheticResource syn_resources[] =
{
    {
        XmNbackPageSize,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.back_page_size),
	XmeFromHorizontalPixels,XmeToHorizontalPixels
    },
    {
        XmNbindingWidth,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.binding_width),
	FromOrientationPixels,ToOrientationPixels
    },
    {
        XmNinnerMarginWidth,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.margin_width),
        XmeFromHorizontalPixels,XmeToHorizontalPixels
    },
    {
        XmNinnerMarginHeight,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.margin_height),
        XmeFromVerticalPixels,XmeToVerticalPixels
    },
    {
        XmNmajorTabSpacing,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.major_spacing),
	FromOrientationOppositePixels,ToOrientationOppositePixels
    },
    {
        XmNminorTabSpacing,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.minor_spacing),
	FromOrientationPixels,ToOrientationPixels
    },
    {
        XmNframeShadowThickness,
        sizeof(Dimension),
        XtOffsetOf(XmNotebookRec, notebook.shadow_thickness),
        XmeFromHorizontalPixels,XmeToHorizontalPixels
    }
};


/*****************************************************************************
 *                                                                           *
 *                    Notebook class record definition  		     *
 *                                                                           *
 *****************************************************************************/


externaldef(xmnotebookclassrec) XmNotebookClassRec xmNotebookClassRec =
{
    /* core class record */
    {
	(WidgetClass)&xmManagerClassRec,    /* superclass */
        "XmNotebook",              	    /* class_name */
        sizeof(XmNotebookRec),		    /* widget_size */
        ClassInitialize,                    /* class_initialize */
        ClassPartInitialize,                /* class_part_initialize */
        FALSE,                              /* class_inited */
        Initialize,                         /* initialize */
        NULL,                               /* initialize_hook */
        XtInheritRealize,	    	    /* realize */
        tab_actions_list,                   /* actions */
        XtNumber(tab_actions_list),         /* num_actions */
        resources,                          /* resources */
        XtNumber(resources),                /* num_resources */
        NULLQUARK,                          /* xrm_class */
        TRUE,                               /* compress_motion */
        XtExposeCompressMaximal, 	    /* compress_exposure */
        TRUE,                               /* compress enterleave */
        TRUE,                               /* visible_interest */
        Destroy,                            /* destroy */
        Resize,                             /* resize */
        Redisplay,                          /* expose */
        SetValues,                          /* set_values */
        NULL,                               /* set_values_hook */
        XtInheritSetValuesAlmost,           /* set_value_almost */
        NULL,                               /* get_values_hook */
        NULL,                               /* accept focus */
        XtVersion,                          /* current version */
        NULL,                               /* callback private */
        XtInheritTranslations,              /* translation table */
        QueryGeometry,                      /* query_geometry */
        NULL,                               /* display_accelerator */
        NULL				    /* extension */
    },
    /* Composite class record */
    {
        GeometryManager,		    /* childrens geometry_manager */
        ChangeManaged,     		    /* change_managed */
	InsertChild,		    	    /* insert_child */
        DeleteChild,			    /* delete_child */
        NULL                    	    /* extension */
    },
    /* Constraint class record */
    {
	constraint_resources,               /* constraint resource */
	XtNumber(constraint_resources),     /* number of constraints */
	sizeof(XmNotebookConstraintRec),    /* size of constraint */
	ConstraintInitialize,         	    /* initialization */
	NULL,                  		    /* destroy proc */
	ConstraintSetValues,          	    /* set_values proc */
	NULL                         	    /* extension */
    },
    /* Manager class record */
    {
        manager_translations,   	    /* translations */
        syn_resources,         		    /* syn_resources */
        XtNumber(syn_resources),	    /* num_syn_resources */
	NULL,			     	    /* get_cont_resources   */
	0, 				    /* num_get_cont_resources */
        XmInheritParentProcess,  	    /* parent_process */
        NULL                    	    /* extension */
    },
    /* Notebook class record */
    {
	NULL				    /* unused */
    }
};


externaldef(xmnotebookwidgetclass) WidgetClass xmNotebookWidgetClass =
	(WidgetClass) &xmNotebookClassRec;


/* Trait record for Notebook scrollFrame */
static XmConst XmScrollFrameTraitRec notebookSFT =
{
    1,				/* version */
    ScrollFrameInit,		/* initialize */
    ScrollFrameGetInfo,		/* get trait info */
    AddNavigator,		/* add navigator method */
    RemoveNavigator,		/* remove navigator method */
    NULL			/* update original geometry */
};


/* Trait record for Notebook traversalControl */
static XmConst XmTraversalControlTraitRec notebookTCT =
{
  0,				/* version */
  RedirectTraversal		/* redirect */
};


/* Trait record for Notebook specifyUnhighlight */
static XmConst XmSpecifyUnhighlightTraitRec notebookSUT =
{
  0,				/* version */
  GetUnhighlightGC		/* getUnhighlightGC */
};




/*****************************************************************************
 *                                                                           *
 *			    Core method functions			     *
 *                                                                           *
 *****************************************************************************/


/*- ClassInitialize ---------------------------------------------------------

	This routine is invoked only once.

-----------------------------------------------------------------------------*/
static void
ClassInitialize(void)
{
    /* Parse the default accelerator table */
    TabAcceleratorsParsed =
                XtParseAcceleratorTable( _XmNotebook_TabAccelerators );
}


/*- ClassPartInitialize -----------------------------------------------------

	Set up the fast subclassing for the widget and install traits

-----------------------------------------------------------------------------*/
static void
ClassPartInitialize (
    WidgetClass wc)
{
    _XmFastSubclassInit(wc, XmNOTEBOOK_BIT);

    XmeTraitSet((XtPointer)wc,XmQTscrollFrame,(XtPointer)&notebookSFT);
    XmeTraitSet((XtPointer)wc,XmQTtraversalControl,(XtPointer)&notebookTCT);
    XmeTraitSet((XtPointer)wc,XmQTspecifyUnhighlight,(XtPointer)&notebookSUT);
}


/*- Initialize --------------------------------------------------------------

	Initialize method

-----------------------------------------------------------------------------*/
static void
Initialize (
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num)
{
    XmNotebookWidget new_w = (XmNotebookWidget)nw;
    XmScrollFrameTrait scroll_frameT;

    /* XmNaccelerators */
    (void)rw;
    (void)args;
    (void)num;
    new_w->core.accelerators = TabAcceleratorsParsed;

    /* XmNorientation */
    if (!XmRepTypeValidValue(XmRID_ORIENTATION,new_w->notebook.orientation,nw))
	new_w->notebook.orientation = XmHORIZONTAL;

    /* XmNbackPagePlacement */
    if (!XmRepTypeValidValue(XmRID_SCROLL_BAR_PLACEMENT,
			new_w->notebook.back_page_pos, nw))
	new_w->notebook.back_page_pos = GetDefaultBackPagePosAgain(nw);

    /* XmNbindingType */
    if (!XmRepTypeValidValue(XmRID_BINDING_TYPE,
			new_w->notebook.binding_type, nw))
	new_w->notebook.binding_type = XmSOLID;

    /* set tab and binding positions */
    SetVisualConfig(new_w);

    /* page number resources */

    /* fix BUX 1183 in notebook widget, the real_back_page_number is used
     * for division, however it is never set unless the back_page_number
     * changed (in SetValues).
     */
    ASSIGN_MIN(new_w->notebook.real_back_page_number,
                 (Dimension)(new_w->notebook.back_page_size /2));
    ASSIGN_MAX(new_w->notebook.real_back_page_number, 1);

    if (new_w->notebook.current_page_number == XmUNSPECIFIED_PAGE_NUMBER)
       new_w->notebook.current_page_number = new_w->notebook.first_page_number;
    if (new_w->notebook.last_page_number == XmUNSPECIFIED_PAGE_NUMBER)
    {
       new_w->notebook.last_page_number = new_w->notebook.first_page_number;
       new_w->notebook.dynamic_last_page_num = True;
    }
    else
	new_w->notebook.dynamic_last_page_num = False;
    new_w->notebook.last_alloc_num = new_w->notebook.first_page_number;

    /* intialize variables for layouting tabs */
    new_w->notebook.first_major = NULL;
    new_w->notebook.old_top_major = NULL;
    new_w->notebook.top_major = NULL;
    new_w->notebook.last_major = NULL;
    new_w->notebook.first_minor = NULL;
    new_w->notebook.old_top_minor = NULL;
    new_w->notebook.top_minor = NULL;
    new_w->notebook.last_minor = NULL;
    new_w->notebook.constraint_child = NULL;

   /* shadow thickness state for current page major and minor tab */
    new_w->notebook.major_shadow_thickness = (Dimension) 0;
    new_w->notebook.minor_shadow_thickness = (Dimension) 0;
    new_w->notebook.in_setshadow = False;
    new_w->notebook.major_shadow_child = NULL;
    new_w->notebook.minor_shadow_child = NULL;

    /* other misc. variables */
    new_w->notebook.which_tab = XmMAJOR_TAB;
    new_w->notebook.scroller_status = DEFAULT_NONE;
    new_w->notebook.need_scroller = XmNONE;
    new_w->notebook.scroller = NULL;
    new_w->notebook.scroller_child = NULL;
    new_w->notebook.in_callback = False;
    new_w->notebook.frame_gc = NULL;
    new_w->notebook.binding_gc = NULL;
    new_w->notebook.background_gc = NULL;
    new_w->notebook.foreground_gc = NULL;
    new_w->notebook.spiral_pixmap = XmUNSPECIFIED_PIXMAP;


    /* initialize the scroll_frame and change_managed flags */
    new_w->notebook.scroll_frame_data = NULL;
    new_w->notebook.first_change_managed = True;

    /* initialize the scrollFrame with move callback (subclass trait check) */

    if (((scroll_frameT=NB_SCROLL_FRAME(nw)) != NULL)
	&& (scroll_frameT->init != NULL) )
	scroll_frameT->init(nw, PageMove, nw);

    /* creating GCs */
    GetFrameGCs(new_w);
    GetBackpageGCs(new_w);

    /* create tab scrollers */
    CreateTabScrollers(new_w);
    SetPageScroller(new_w);

}


/*- Destroy -----------------------------------------------------------------

	Destroy the widget and its children

-----------------------------------------------------------------------------*/
static void
Destroy (
    Widget w)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    /* release all GCs */
    if (nb->notebook.frame_gc)
        XtReleaseGC(w, nb->notebook.frame_gc);
    if (nb->notebook.binding_gc)
        XtReleaseGC(w,nb->notebook.binding_gc);
    if (nb->notebook.foreground_gc)
        XtReleaseGC(w,nb->notebook.foreground_gc);
    if (nb->notebook.background_gc)
        XtReleaseGC(w,nb->notebook.background_gc);

    /* release spiral pixmap */
    if (nb->notebook.spiral_pixmap != XmUNSPECIFIED_PIXMAP
	&& nb->notebook.spiral_pixmap != XmNONE)
        XFreePixmap(XtDisplay(w), nb->notebook.spiral_pixmap);

    /* release scroll frame data */
    if (nb->notebook.scroll_frame_data != NULL)
	XtFree((char*)nb->notebook.scroll_frame_data);
}


/*- Resize -----------------------------------------------------------------

	Resize the widget

-----------------------------------------------------------------------------*/
static void
Resize (
    Widget w)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    /* Adjust notebook children sizes based on notebook size */
    AdjustGeometry(nb, NULL, NULL);

    /* Layout children based on notebook sizes */
    LayoutChildren(nb, NULL);

    if (XtIsRealized(w))
	XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}


/*- Redisplay ---------------------------------------------------------------

	Method for expose event

-----------------------------------------------------------------------------*/
static void
Redisplay (
    Widget w,
    XEvent *e,
    Region region)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;
    XExposeEvent *event = (XExposeEvent *)e;
    Widget child;			/* fix for CR8959, hack for CR9805 */
    int i;				/* fix for CR8959, hack for CR9805 */
    int total_major_tabs,total_minor_tabs;
    XmNotebookConstraint nc;
    total_major_tabs = 0;
    for (i = 0; i < nb->composite.num_children; i++)
        {
        child = nb->composite.children[i];
        nc = NotebookConstraint(child);
        if (NB_IS_CHILD_MAJOR(nc->child_type))
                total_major_tabs++;
        }
    if (total_major_tabs > 0 )
    {
	XtManageChild(nb->notebook.prev_major);
	XtManageChild(nb->notebook.next_major);
    }
    else
    {
	XtUnmanageChild(nb->notebook.prev_major);
	XtUnmanageChild(nb->notebook.next_major);
    }
    total_minor_tabs = 0;
    for (i = 0; i < nb->composite.num_children; i++)
        {
        child = nb->composite.children[i];
        nc = NotebookConstraint(child);
        if (NB_IS_CHILD_MAJOR(nc->child_type))
                total_minor_tabs++;
        }
    if (total_minor_tabs > 0 )
    {
	XtManageChild(nb->notebook.prev_minor);
	XtManageChild(nb->notebook.next_minor);
    }
    else
    {
	XtUnmanageChild(nb->notebook.prev_minor);
	XtUnmanageChild(nb->notebook.next_minor);
    }
    /* draw the background */
    DrawBackPages(nb, event, region);
    DrawBinding(nb, event, region);
    DrawFrameShadow(nb, event, region);

    /* display gadgets */
    XmeRedisplayGadgets(w,e,region);

    /* Work-around due to PushBG not updating border highlight
       -- to be removed when PushBG is fixed, see CR9805 */
    for (i=0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	if ( (NB_IS_CHILD_TAB(NotebookConstraint(child)->child_type))
	  && (NotebookConstraint(child)->active)
	  && (XmIsPushButtonGadget((child))))
	    {
	  (*(xmPushButtonGadgetClassRec.gadget_class.border_unhighlight))((child));
	    }
	}
}


/*- SetValues ---------------------------------------------------------------

        Set notebook widget resources

-----------------------------------------------------------------------------*/
static Boolean
SetValues (
    Widget ow,
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *num)
{
    XmNotebookWidget old = (XmNotebookWidget) ow;
    XmNotebookWidget new_w = (XmNotebookWidget) nw;
    Boolean redraw = False;
    Boolean updateJoin = False;
    Boolean updateVisual = False;
    Boolean relayout = False;
    Mask visualFlag = NoVisualChange;

    /* XmNorientation */
    (void)rw;
    (void)args;
    (void)num;
    if (new_w->notebook.orientation != old->notebook.orientation)
        {
        if (!XmRepTypeValidValue(XmRID_ORIENTATION,
			new_w->notebook.orientation, nw))
           new_w->notebook.orientation = old->notebook.orientation;
        }

    /* XmNbackPagePlacement */
    if (new_w->notebook.back_page_pos != old->notebook.back_page_pos)
        {
	if (!XmRepTypeValidValue(XmRID_SCROLL_BAR_PLACEMENT,
			new_w->notebook.back_page_pos, nw))
            new_w->notebook.back_page_pos = old->notebook.back_page_pos;
        }

    /* XmNbindingType */
    if (new_w->notebook.binding_type != old->notebook.binding_type)
	{
	if (!XmRepTypeValidValue(XmRID_BINDING_TYPE,
			new_w->notebook.binding_type, nw))
            new_w->notebook.binding_type = old->notebook.binding_type;
        }

    /* XmNcurrentPageNumber */
    /* current page must be in the page number range */
    ASSIGN_MAX(new_w->notebook.current_page_number,
		new_w->notebook.first_page_number);
    ASSIGN_MIN(new_w->notebook.current_page_number,
		new_w->notebook.last_page_number);

    /* XmNlastPageNumber */
    /* set static last page number, if the last page number is set */
    if (new_w->notebook.last_page_number != old->notebook.last_page_number)
	new_w->notebook.dynamic_last_page_num = False;

    /* XmNfirstPageNumber */
    /* re-determine active children, if page number range has been changed */
    if (new_w->notebook.first_page_number != old->notebook.first_page_number
	|| new_w->notebook.last_page_number != old->notebook.last_page_number)
	{
	/* Update navigators, unless GotoPage() will */
        if (new_w->notebook.current_page_number ==
				old->notebook.current_page_number)
	    UpdateNavigators(new_w);
	SetActiveChildren(new_w);
	}

    /* if XmNcurrentPageNumber is changed, go to the new page */
    if (new_w->notebook.current_page_number !=
				old->notebook.current_page_number)
	{
	int page_number;

	page_number = new_w->notebook.current_page_number;
	new_w->notebook.current_page_number = old->notebook.current_page_number;
	GotoPage(new_w, page_number, NULL, XmCR_NONE);
	}

    /*
     * Notebook visuals
     */

    /* XmNorientation, XmNbackPagePlacement */
    if (new_w->notebook.orientation != old->notebook.orientation
	|| new_w->notebook.back_page_pos != old->notebook.back_page_pos)
	{
	updateVisual = True;
	updateJoin = True;
	}

    /* XmNframeShadowThickness */
    if (new_w->notebook.shadow_thickness != old->notebook.shadow_thickness)
	updateJoin = True;

    /* XmNbackPageNumber */
    if (new_w->notebook.back_page_number != old->notebook.back_page_number)
	{
    	/* set the real back page number */
    	new_w->notebook.real_back_page_number =
					new_w->notebook.back_page_number;
	ASSIGN_MIN(new_w->notebook.real_back_page_number,
		   (Dimension)(new_w->notebook.back_page_size /2));
	ASSIGN_MAX(new_w->notebook.real_back_page_number, 1);
	if (new_w->notebook.real_back_page_number !=
					old->notebook.real_back_page_number)
	    redraw = True;
	}

    /* XmNframeBackground */
    if (new_w->notebook.frame_background != old->notebook.frame_background
	|| new_w->manager.foreground != old->manager.foreground)
	{
	GetFrameGCs(new_w);
	visualFlag |= (VisualHighlightColor|VisualHighlightPixmap);
	}

    /* XmNbackPageBackground, XmNbackPageForeground */
    if (new_w->notebook.back_page_background !=
				    old->notebook.back_page_background
	|| new_w->notebook.back_page_foreground !=
				    old->notebook.back_page_foreground)
	GetBackpageGCs(new_w);

    /* Set new size, if necessary */
    if (XtIsRealized((Widget)new_w) &&
	(new_w->notebook.orientation != old->notebook.orientation
	|| new_w->notebook.back_page_pos != old->notebook.back_page_pos
	|| new_w->notebook.back_page_size != old->notebook.back_page_size
	|| new_w->notebook.binding_type != old->notebook.binding_type
	|| new_w->notebook.binding_width != old->notebook.binding_width
	|| new_w->notebook.first_page_number != old->notebook.first_page_number
        || new_w->notebook.last_page_number != old->notebook.last_page_number
	|| new_w->notebook.margin_width != old->notebook.margin_width
	|| new_w->notebook.margin_height != old->notebook.margin_height
	|| new_w->notebook.major_spacing != old->notebook.major_spacing
	|| new_w->notebook.minor_spacing != old->notebook.minor_spacing
	|| new_w->notebook.shadow_thickness != old->notebook.shadow_thickness))
	{
	relayout = True;
	redraw = True;
	}

    /* if visual is changed, redraw */
    if (new_w->notebook.frame_background != old->notebook.frame_background
	|| new_w->notebook.binding_pixmap != old->notebook.binding_pixmap
	|| new_w->notebook.back_page_foreground !=
				old->notebook.back_page_foreground
	|| new_w->notebook.back_page_background !=
				old->notebook.back_page_background)
	{
	redraw = True;
	}


    if (updateVisual)
	SetVisualConfig(new_w);

    if (updateJoin)
	UpdateJoinSideChildren (new_w, (Dimension) 0);

    if (relayout)
	{
	Dimension save_width, save_height;
	XtWidgetGeometry preferred_geo;

	/* Store new w/h, restore old w/h, and adjust geometry to the old */
	save_width = XtWidth(new_w);
	save_height = XtHeight(new_w);
	XtWidth(new_w) = XtWidth(old);
	XtHeight(new_w) = XtHeight(old);

	/* Adjust notebook's children type sizes to old size */
	AdjustGeometry(new_w, NULL, NULL);

	/* Layout Children based on old notebook sizes */
	LayoutChildren(new_w, NULL);

	/* Get preferred size unless app changed both width & height */
	preferred_geo.width  = save_width;
	preferred_geo.height = save_height;
	if ((XtWidth(old) == save_width) || (XtHeight(old) == save_height))
	    NewPreferredGeometry(new_w, NULL, NULL, &preferred_geo);

	/* App request for width takes precedence over NewPreferredGeometry */
	if (XtWidth(old) != save_width)
	    XtWidth(new_w) = save_width;
	else
	    XtWidth(new_w) = preferred_geo.width;

	/* App request for height takes precedence over NewPreferredGeometry */
	if (XtHeight(old) != save_height)
	    XtHeight(new_w) = save_height;
	else
	    XtHeight(new_w) = preferred_geo.height;
	}

    if (visualFlag)
	redraw |= _XmNotifyChildrenVisual (ow, nw, visualFlag);


    return(redraw);
}


/*- QueryGeometry -----------------------------------------------------------

        Method for the composite widget's QueryGeometry

	Returns:
	    preferred size if realized, else user/application w/h values

-----------------------------------------------------------------------------*/
static XtGeometryResult
QueryGeometry (
    Widget w,
    XtWidgetGeometry *intended,
    XtWidgetGeometry *desired)
{

    /* Get the preferred w/h based on children preferred sizes */
    NewPreferredGeometry((XmNotebookWidget)w, NULL, NULL, desired);

    /* Handle user initial size setting */
    if (!XtIsRealized(w))
	{
	if (XtWidth(w))
	    desired->width = XtWidth(w);
	if (XtHeight(w))
	    desired->height = XtHeight(w);
	}

    /* Return result, XmeReplyToQueryGeometry will set CWidth and CHeight */
    return(XmeReplyToQueryGeometry(w, intended, desired));
}



/*****************************************************************************
 *                                                                           *
 *                         Composite Class Methods                           *
 *                                                                           *
 *****************************************************************************/


/*- GeometryManager ---------------------------------------------------------

        Method for the composite widget's GeometryManager

-----------------------------------------------------------------------------*/
static XtGeometryResult
GeometryManager(
    Widget instigator,
    XtWidgetGeometry *desired,
    XtWidgetGeometry *reply)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(instigator);
    XmNotebookConstraint nc = NotebookConstraint(instigator);
    XtGeometryResult result = XtGeometryNo;
    XtWidgetGeometry myrequest, myallowed;

    /* handle changes during ConstraintSetValues layout */
    (void)reply;
    if ( (nb->notebook.constraint_child == instigator)
      && ((IsX(desired)) && (IsY(desired))))
	{
	nb->notebook.constraint_child = NULL;
	XtX(instigator) = desired->x;
	XtY(instigator) = desired->y;
        if (IsWidth(desired))
	    XtWidth(instigator) = desired->width;
        if (IsHeight(desired))
	    XtHeight(instigator) = desired->height;
	return XtGeometryYes;
	}

    /* only for resizing request */
    if ((nc->resizable || NB_IS_CHILD_TAB(nc->child_type))
        && (desired->request_mode & (CWWidth|CWHeight|CWBorderWidth))
	&& !nb->notebook.in_setshadow)
	{
	/* Determine preferred geometry based on instigator preferrences */
	NewPreferredGeometry(nb, instigator, desired, &myrequest);

	/*
	 * Prepare resize request (as needed) to the parent
	 */
        myrequest.request_mode = 0;
	if (IsQueryOnly(desired))
	    myrequest.request_mode |= XtCWQueryOnly;
        if (IsWidth(desired) && XtWidth(nb) != myrequest.width)
            myrequest.request_mode |= CWWidth;
        if (IsHeight(desired) && XtHeight(nb) != myrequest.height)
            myrequest.request_mode |= CWHeight;
        if (IsBorder(desired) && XtBorderWidth(nb) != myrequest.border_width)
            myrequest.border_width |= CWBorderWidth;

        /* ask parent, only if notebook resize request is needed */
        if (myrequest.request_mode)
	    {
	    if (NB_IS_CHILD_TAB(nc->child_type))
	      result = XtGeometryYes;
	    else /* nc->resizable */
            {
	    result = XtMakeGeometryRequest((Widget)nb, &myrequest, &myallowed);

	    /* Parent unable to completely fulfill request */
	    if (result == XtGeometryAlmost)
	    result = XtGeometryNo;
	    }
	    }

	/* Update the geometry, if necessary */
	if (result == XtGeometryYes && !IsQueryOnly(desired))
	    {
	    /* Adjust notebook's children type sizes to new size */
	    AdjustGeometry(nb, instigator, desired);

	    /* Layout Children based on new notebook size */
	    LayoutChildren(nb, instigator);

	    if (XtIsRealized((Widget)nb))
		XClearArea(XtDisplay((Widget)nb),XtWindow((Widget)nb),
			0,0,0,0,True);
	    }
	}
    return result;
}


/*- ChangeManaged -----------------------------------------------------------

        Method for the composite widget's ChangeManaged

        Something changed in the set of managed children, so place
        the children and change the form widget size to reflect new size,
        if possible.

-----------------------------------------------------------------------------*/
static void
ChangeManaged (
    Widget w)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;
    XtWidgetGeometry preferred;

    /* ChangeManaged recursively called by SetPageScroller; return */
    if (nb->notebook.scroller_status == DEFAULT_CREATE)
	return;

    /* if this is the first time through, set the page scroller */
    if (nb->notebook.scroller_status == DEFAULT_NONE)
	{
	SetPageScroller(nb);
	UpdateNavigators(nb);
	}

    /* assign default page number to those who don't have */
    AssignDefaultPageNumber(nb);

    /* invoke XmNpageChangedCallback for initial setting of current page */
    if (nb->notebook.first_change_managed
	&& XtHasCallbacks(w, XmNpageChangedCallback) == XtCallbackHasSome)
	{
	XmNotebookCallbackStruct cbs;

	cbs.reason           = XmCR_NONE;
	cbs.event            = NULL;
	cbs.page_number      = nb->notebook.current_page_number;
	cbs.page_widget      = GetChildWidget(nb,
				nb->notebook.current_page_number, XmPAGE);
	cbs.prev_page_number = XmUNSPECIFIED_PAGE_NUMBER;
	cbs.prev_page_widget = NULL;

	/* Mark that the callback is being called and when it finishes */
	nb->notebook.in_callback = True;
	XtCallCallbackList(w, nb->notebook.page_change_callback, &cbs);
	nb->notebook.in_callback = False;
	}

    /* sort children by their page number and update their activity */
    SortChildren(nb);

    /*
     * Determine preferred notebook size based on new managed child
     */

    /* Check if current size is the preferred size */
    if (NewPreferredGeometry(nb, NULL, NULL, &preferred))
    {
	/* Prepare parent geometry request */
	preferred.request_mode = 0;
        if (XtIsRealized((Widget)nb) || XtWidth(nb) == 0)
            preferred.request_mode |= CWWidth;
        if (XtIsRealized((Widget)nb) || XtHeight(nb) == 0)
            preferred.request_mode |= CWHeight;

	/* Make parent geometry request, if necessary */
	if (preferred.request_mode)
	    _XmMakeGeometryRequest((Widget)nb, &preferred);

	/* Clear notebook area otherwise binder and backpage don't update. */
	if (XtIsRealized((Widget)nb) && !nb->notebook.first_change_managed)
	    XClearArea(XtDisplay(nb), XtWindow(nb), 0, 0, 0, 0, True);
    }

    /* Adjust notebook's children type sizes (based on notebook size) */
    AdjustGeometry(nb, NULL, NULL);

    /* Layout Children (based on notebook size) */
    LayoutChildren(nb, NULL);

    XmeNavigChangeManaged(w);
    nb->notebook.first_change_managed = False;
}


/*- InsertChild -------------------------------------------------------------

        Method for the composite widget's InsertChild

-----------------------------------------------------------------------------*/
static void
InsertChild (
    Widget child)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(child);
    XmNotebookConstraint nc = NotebookConstraint(child);
    XmScrollFrameTrait scroll_frameT;
    XtWidgetProc insert_child;

    /* check if trying to insert an invalid object */
    if (!XtIsRectObj(child))
	return;

    /* call manager's InsertChild method */
    _XmProcessLock();
    insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)
			->composite_class.insert_child;
    _XmProcessUnlock();
    (*insert_child)(child);

    /* add some keyboard traversal stuff */
    switch (nc->child_type)
	{
	case XmPAGE_SCROLLER:
	    /* First: take care of previous page scroller */

	    /* default page scroller not yet made, so just put this one in */
	    if (nb->notebook.scroller_status == DEFAULT_NONE)
	       nb->notebook.scroller_status = DEFAULT_GONE;

	    /* remove previously created default page scroller */
	    else if (nb->notebook.scroller_status == DEFAULT_USED)
		{
		nb->notebook.scroller_status = DEFAULT_CREATE;

		/* remove navigator, if page scroller is a navigator */
		if (NB_IS_CHILD_NAVIGATOR(nb->notebook.scroller))
		    {
		    if ((scroll_frameT=NB_SCROLL_FRAME(nb)) != NULL
			&& scroll_frameT->removeNavigator != NULL)
			scroll_frameT->removeNavigator((Widget) nb,
							nb->notebook.scroller);
		    }
		XtDestroyWidget(nb->notebook.scroller);

		nb->notebook.scroller_status = DEFAULT_GONE;
		}

	    /* remove previously created non-default page scroller */
	    else if (nb->notebook.scroller_status == DEFAULT_GONE
		  && nb->notebook.scroller != NULL)
		{
		nb->notebook.scroller_status = DEFAULT_CREATE;

		/* remove navigator, if page scroller is a navigator */
		if (NB_IS_CHILD_NAVIGATOR(nb->notebook.scroller))
		    {
		    if ((scroll_frameT=NB_SCROLL_FRAME(nb)) != NULL
                        && scroll_frameT->removeNavigator != NULL)
			scroll_frameT->removeNavigator((Widget)nb,
							nb->notebook.scroller);
		    }
		XtUnmanageChild(nb->notebook.scroller);

		nb->notebook.scroller_status = DEFAULT_GONE;
		}

	    /* Second: take care of this new page scroller */

	    /* update scroller's translation table */
	    nb->notebook.scroller = child;

	    /* add navigator, if page scroller has navigator trait */
	    if (NB_IS_CHILD_NAVIGATOR(nb->notebook.scroller))
		{
		if ((scroll_frameT = NB_SCROLL_FRAME(nb)) != NULL
		    && scroll_frameT->addNavigator != NULL)
			scroll_frameT->addNavigator((Widget)nb,
						    child, NavigDimensionX);
		}

	    /* update navigators so this one get initial info */
            UpdateNavigators(nb);

	    break;

	case XmPAGE:
	case XmSTATUS_AREA:
            XtVaSetValues(child, XmNnavigationType, XmTAB_GROUP, NULL);
	    break;

	case XmMAJOR_TAB:
	case XmMINOR_TAB:
	    /* Install activiation callback */
	    {
	    XmActivatableTrait activeT;

	    if ((activeT=NB_ACTIVATABLE(child)) != NULL
		&& activeT->changeCB != NULL)
		activeT->changeCB(child, TabPressed,
		    (XtPointer)(unsigned long)(NB_IS_CHILD_MAJOR(nc->child_type)
		    ? XmCR_MAJOR_TAB : XmCR_MINOR_TAB), True);
	    }

	    /* Update JoinSide information */
	    (void) UpdateJoinSide(nb, child, nc->child_type, (Dimension) 0);

	    /* Install accelerators */
	    if (!XmIsGadget(child))
                XtInstallAccelerators(child, (Widget)nb);
	    break;
	}
}


/*- DeleteChild -------------------------------------------------------------

        Method for the composite widget's DeleteChild

-----------------------------------------------------------------------------*/
static void
DeleteChild (
    Widget child)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(child);
    XmNotebookConstraint nc = NotebookConstraint(child);
    XmScrollFrameTrait scroll_frameT;
    XtWidgetProc delete_child;

    /* clear child pointers in Notebook */
    switch (nc->child_type)
	{
        case XmPAGE_SCROLLER:
	    if (nb->notebook.scroller == child)
		{
		/* remove navigator */
		if (NB_IS_CHILD_NAVIGATOR(nb->notebook.scroller))
		    {
		    if ((scroll_frameT=NB_SCROLL_FRAME(nb)) != NULL
			&& scroll_frameT->removeNavigator != NULL)
			scroll_frameT->removeNavigator((Widget)nb,
						    nb->notebook.scroller);
		    }
		/* Mark that no page scroller is established */
		nb->notebook.scroller = NULL;
		nb->notebook.scroller_status = DEFAULT_GONE;
		}
	    break;
	case XmMAJOR_TAB_SCROLLER:
	    nb->notebook.next_major = NULL;
	    nb->notebook.prev_major = NULL;
	    break;
	case XmMINOR_TAB_SCROLLER:
	    nb->notebook.next_minor = NULL;
	    nb->notebook.prev_minor = NULL;
	    break;
	}

    /* call manager's DeleteChild method */
    _XmProcessLock();
    delete_child = ((XmManagerWidgetClass)xmManagerWidgetClass)
			->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)(child);
}



/*****************************************************************************
 *                                                                           *
 *                        Constraint Class Methods                           *
 *                                                                           *
 *****************************************************************************/


/*- ConstraintInitialize ----------------------------------------------------

        Initialize method for Notebook's constraint class

	A default page number will be provided when the widget is managed.

-----------------------------------------------------------------------------*/
static void
ConstraintInitialize(
    Widget req,
    Widget new_w,
    ArgList args,
    Cardinal *num)
{
    XmNotebookConstraint nc = NotebookConstraint(new_w);

    /* check for invalid object */
    (void)req;
    (void)args;
    (void)num;
    if (!XtIsRectObj(new_w))
        return;

    /* validate the child type */
    if (nc->child_type != XmMAJOR_TAB_SCROLLER
	&& nc->child_type != XmMINOR_TAB_SCROLLER
	&& !XmRepTypeValidValue(XmRID_NB_CHILD_TYPE, nc->child_type, new_w))
        nc->child_type = XmNONE;

    /* convert to internal */
     if (nc->child_type == XmNONE)
        {
	if (NB_IS_CHILD_ACTIVATABLE(new_w))
	    nc->child_type = XmMAJOR_TAB;
	else if (NB_IS_CHILD_ACCESSTEXTUAL(new_w))
	    nc->child_type = XmSTATUS_AREA;
	else if (NB_IS_CHILD_NAVIGATOR(new_w))
	    nc->child_type = XmPAGE_SCROLLER;
	else
	    nc->child_type = XmPAGE;
	}
}


/*- ConstraintSetValues -----------------------------------------------------

        SetValue method for notebook's constraint class

-----------------------------------------------------------------------------*/
static Boolean
ConstraintSetValues (
    Widget old_w,
    Widget req,
    Widget new_w,
    ArgList args,
    Cardinal *num)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(new_w);
    XmNotebookConstraint old_nc = NotebookConstraint(old_w);
    XmNotebookConstraint new_nc = NotebookConstraint(new_w);
    Boolean redraw = False;
    Boolean need_reset = False;

    /* check for invalid object */
    (void)req;
    (void)args;
    (void)num;
    if (!XtIsRectObj(new_w))
	return False;

    /* XmNnotebookChildType is only CG, not Settable */
    if (new_nc->child_type != old_nc->child_type) {
	new_nc->child_type = old_nc->child_type;
	XmeWarning(new_w, MESSAGE0);
    }

    /* if the page number is changed, sort again */
    /* by just repositioning that child */
    if (new_nc->page_number != old_nc->page_number)
	{
	/* check for last page number change */
	if (nb->notebook.dynamic_last_page_num)
	    {
	    if (new_nc->page_number > nb->notebook.last_page_number
	    && XtIsManaged(new_w)
	    && (NB_IS_CHILD_PAGE(new_nc->child_type)
		|| NB_IS_CHILD_TAB(new_nc->child_type)
		|| NB_IS_CHILD_STATUS(new_nc->child_type)))
		need_reset = SetLastPageNumber(nb, new_nc->page_number);
	    else if (old_nc->page_number == nb->notebook.last_page_number)
		need_reset = AssignDefaultPageNumber(nb);
	    }

	/* re-sort and check child list */
	RepositionChild(nb, new_w);
	SetActiveChildren(nb);

	/* check for changes that affect layout */
	if (!nb->notebook.in_callback)
	    {
	    switch(new_nc->child_type)
		{
		case XmPAGE:
		case XmSTATUS_AREA:
		if (nb->notebook.current_page_number == old_nc->page_number
		|| nb->notebook.current_page_number == new_nc->page_number)
		    {
		    if (need_reset)
			ResetTopPointers(nb, XmNONE, 0);
		    nb->notebook.constraint_child = new_w;
		    LayoutPages(nb, NULL);
		    redraw = True;
		    }
		    break;
		case XmMAJOR_TAB:
		    ResetTopPointers(nb, XmNONE, 0);
		    nb->notebook.constraint_child = new_w;
		    LayoutMajorTabs(nb, NULL);
		    LayoutMinorTabs(nb, NULL);
		    redraw = True;
		    break;
		case XmMINOR_TAB:
		    ResetTopPointers(nb, XmNONE, 0);
		    nb->notebook.constraint_child = new_w;
		    LayoutMinorTabs(nb, NULL);
		    redraw = True;
		    break;
		} /* switch */
	    } /* if */
	} /* if */

    return(redraw);
}



/*****************************************************************************
 *                                                                           *
 *			  Initializing & Resource Managing		     *
 *                                                                           *
 *****************************************************************************/


/*- GetDefaultBackPagePos ---------------------------------------------------

	Get the default back page position

-----------------------------------------------------------------------------*/
static void
GetDefaultFrameBackground(
    Widget w,
    int offset,
    XrmValue *value)
{
	(void)offset;
	value->addr = (XtPointer)&w->core.background_pixel;
}

static void
GetDefaultBackPageBackground(
    Widget w,
    int offset,
    XrmValue *value)
{
  XmNotebookWidget nb = (XmNotebookWidget)w;
  static Pixel pixel;

  (void)offset;
  value->addr = (XtPointer)&pixel;

  XmGetColors(XtScreen(w), nb->core.colormap, nb->notebook.frame_background,
	      NULL, NULL, NULL, &pixel);
}

static void
GetDefaultBackPagePos (
    Widget w,
    int offset,
    XrmValue *value)
{
    static unsigned char back_page_pos;
    XmNotebookWidget nb = (XmNotebookWidget)w;

    /* initialize the notebook layout default */
    (void)offset;
    if (LayoutIsRtoLM(w))
	{
	if (nb->notebook.orientation == XmVERTICAL)
	    /* RIGHT_TO_LEFT and VERTICAL */
	    back_page_pos = XmBOTTOM_LEFT;
	else
	    /* RIGHT_TO_LEFT and HORIZONTAL */
	    back_page_pos = XmBOTTOM_LEFT;
	}
    else
	{
	 if (nb->notebook.orientation == XmVERTICAL)
	    /* LEFT_TO_RIGHT and VERTICAL */
	    back_page_pos = XmBOTTOM_RIGHT;
	else
	    /* LEFT_TO_RIGHT and HORIZONTAL */
	    back_page_pos = XmBOTTOM_RIGHT;
	}
    /*
     * This will have to change when BOTTOM_TO_TOP is supported for
     * XmNlayoutDirection.
     */

    value->addr = (XPointer) &back_page_pos;
}

/* Same logic as the GetDefaultBackPagePos default proc ... rewritten
 * without the static variable - for use from the Initialize proc .
 */
static unsigned char
GetDefaultBackPagePosAgain (
    Widget w)
{
    unsigned char back_page_pos;
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (LayoutIsRtoLM(w))
	{
	if (nb->notebook.orientation == XmVERTICAL)
	    /* RIGHT_TO_LEFT and VERTICAL */
	    back_page_pos = XmBOTTOM_LEFT;
	else
	    /* RIGHT_TO_LEFT and HORIZONTAL */
	    back_page_pos = XmBOTTOM_LEFT;
	}
    else
	{
	 if (nb->notebook.orientation == XmVERTICAL)
	    /* LEFT_TO_RIGHT and VERTICAL */
	    back_page_pos = XmBOTTOM_RIGHT;
	else
	    /* LEFT_TO_RIGHT and HORIZONTAL */
	    back_page_pos = XmBOTTOM_RIGHT;
	}

    return back_page_pos;
}

/*- FromOrientationPixels ---------------------------------------------------

        Determines orientation and calls appropriate converter.

-----------------------------------------------------------------------------*/
static void
FromOrientationPixels (
    Widget w,
    int offset,
    XtArgVal *value)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if ((int)(*value) < 0)
	*value = 0;
    if (nb->notebook.orientation == XmHORIZONTAL)
	XmeFromHorizontalPixels(w, offset, value);
    else
	XmeFromVerticalPixels(w, offset, value);
}


/*- ToOrientationPixels ----------------------------------------------------

        Determines orientation and calls appropriate converter.

-----------------------------------------------------------------------------*/
static XmImportOperator
ToOrientationPixels (
    Widget w,
    int offset,
    XtArgVal *value)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (nb->notebook.orientation == XmHORIZONTAL)
        return(XmeToHorizontalPixels(w, offset, value));
    else
        return(XmeToVerticalPixels(w, offset, value));
}


/*- FromOrientationOppositePixels -------------------------------------------

        Determines orientation and calls appropriate converter.

-----------------------------------------------------------------------------*/
static void
FromOrientationOppositePixels (
    Widget w,
    int offset,
    XtArgVal *value)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if ((int)(*value) < 0)
        *value = 0;
    if (nb->notebook.orientation == XmHORIZONTAL)
	XmeFromVerticalPixels(w, offset, value);
    else
        XmeFromHorizontalPixels(w, offset, value);
}


/*- ToOrientationOppositePixels --------------------------------------------

        Determines orientation and calls appropriate converter.

-----------------------------------------------------------------------------*/
static XmImportOperator
ToOrientationOppositePixels (
    Widget w,
    int offset,
    XtArgVal *value)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (nb->notebook.orientation == XmHORIZONTAL)
	return(XmeToVerticalPixels(w, offset, value));
    else
        return(XmeToHorizontalPixels(w, offset, value));
}


/*- CreateTabScrollers -----------------------------------------------------

	Create Notebook's tab scrollers

-----------------------------------------------------------------------------*/
static void
CreateTabScrollers (
    XmNotebookWidget nb)
{
    /* creating major tab scrollers */
    nb->notebook.next_major = XtVaCreateManagedWidget(MAJOR_TAB_NEXT_NAME,
                xmArrowButtonGadgetClass, (Widget)nb,
                XmNwidth, TAB_SCROLLER_WIDTH_DEFAULT,
                XmNheight, TAB_SCROLLER_HEIGHT_DEFAULT,
                XmNnotebookChildType, XmMAJOR_TAB_SCROLLER,
		XmNtraversalOn, False,
                NULL);
    ((XmActivatableTrait)XmeTraitGet((XtPointer)
		XtClass((Widget)nb->notebook.next_major),XmQTactivatable))->
		changeCB(nb->notebook.next_major,FlipTabs,NULL,True);

    nb->notebook.prev_major = XtVaCreateManagedWidget(MAJOR_TAB_PREV_NAME,
                xmArrowButtonGadgetClass, (Widget)nb,
                XmNwidth, TAB_SCROLLER_WIDTH_DEFAULT,
                XmNheight, TAB_SCROLLER_HEIGHT_DEFAULT,
                XmNnotebookChildType, XmMAJOR_TAB_SCROLLER,
		XmNtraversalOn, False,
                NULL);
    ((XmActivatableTrait)XmeTraitGet((XtPointer)
                XtClass((Widget)nb->notebook.prev_major),XmQTactivatable))->
                changeCB(nb->notebook.prev_major,FlipTabs,NULL,True);

    /* creating minor tab scrollers */
    nb->notebook.next_minor = XtVaCreateManagedWidget(MINOR_TAB_NEXT_NAME,
                xmArrowButtonGadgetClass, (Widget)nb,
                XmNwidth, TAB_SCROLLER_WIDTH_DEFAULT,
                XmNheight, TAB_SCROLLER_HEIGHT_DEFAULT,
                XmNnotebookChildType, XmMINOR_TAB_SCROLLER,
		XmNtraversalOn, False,
                NULL);
    ((XmActivatableTrait)XmeTraitGet((XtPointer)
                XtClass((Widget)nb->notebook.next_minor),XmQTactivatable))->
                changeCB(nb->notebook.next_minor,FlipTabs,NULL,True);

    nb->notebook.prev_minor = XtVaCreateManagedWidget(MINOR_TAB_PREV_NAME,
                xmArrowButtonGadgetClass, (Widget)nb,
                XmNwidth, TAB_SCROLLER_WIDTH_DEFAULT,
                XmNheight, TAB_SCROLLER_HEIGHT_DEFAULT,
                XmNnotebookChildType, XmMINOR_TAB_SCROLLER,
		XmNtraversalOn, False,
                NULL);
    ((XmActivatableTrait)XmeTraitGet((XtPointer)
                XtClass((Widget)nb->notebook.prev_minor),XmQTactivatable))->
                changeCB(nb->notebook.prev_minor,FlipTabs,NULL,True);
}


/*- SetVisualConfig ---------------------------------------------------------

        Sets Notebook visual configuration private variables.

-----------------------------------------------------------------------------*/
static void
SetVisualConfig (
    XmNotebookWidget nb)
{
    /* set tab and binding position variables */
    if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
        nb->notebook.orientation == XmHORIZONTAL)
    {
        nb->notebook.major_pos = RIGHT;
        nb->notebook.minor_pos = BOTTOM;
        nb->notebook.binding_pos = LEFT;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
        nb->notebook.orientation == XmVERTICAL)
    {
        nb->notebook.major_pos = BOTTOM;
        nb->notebook.minor_pos = RIGHT;
        nb->notebook.binding_pos = TOP;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
        nb->notebook.orientation == XmHORIZONTAL)
    {
        nb->notebook.major_pos = LEFT;
        nb->notebook.minor_pos = BOTTOM;
        nb->notebook.binding_pos = RIGHT;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
        nb->notebook.orientation == XmVERTICAL)
    {
        nb->notebook.major_pos = BOTTOM;
        nb->notebook.minor_pos = LEFT;
        nb->notebook.binding_pos = TOP;
    }
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
        nb->notebook.orientation == XmHORIZONTAL)
    {
        nb->notebook.major_pos = RIGHT;
        nb->notebook.minor_pos = TOP;
        nb->notebook.binding_pos = LEFT;
    }
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
        nb->notebook.orientation == XmVERTICAL)
    {
        nb->notebook.major_pos = TOP;
        nb->notebook.minor_pos = RIGHT;
        nb->notebook.binding_pos = BOTTOM;
    }
    else if (nb->notebook.back_page_pos == XmTOP_LEFT &&
        nb->notebook.orientation == XmHORIZONTAL)
    {
        nb->notebook.major_pos = LEFT;
        nb->notebook.minor_pos = TOP;
        nb->notebook.binding_pos = RIGHT;
    }
    else if (nb->notebook.back_page_pos == XmTOP_LEFT &&
        nb->notebook.orientation == XmVERTICAL)
    {
        nb->notebook.major_pos = TOP;
        nb->notebook.minor_pos = LEFT;
        nb->notebook.binding_pos = BOTTOM;
    }
}


/*- UpdateJoinSide ------------------------------------------------------------

        Update tab's JoinSide Trait to match current visual configuration.

	Uses instance state:
		major_pos
		minor_pos
	Returns
	    True, if the child is major or minor tab and holds the
	    JoinSide trait.  Otherwise, returns False.

-----------------------------------------------------------------------------*/
static Boolean
UpdateJoinSide (
    XmNotebookWidget nb,
    Widget child,
    unsigned char child_type,
    Dimension shadow_thickness)
{
    XmJoinSideTrait joinsideT;          /* child JoinSide trait */
    unsigned char side_of_book;         /* side of notebook tab is on */
    unsigned char side_to_join;         /* side to join */

   /* Determine side of nookbook tabs reside */
    if (NB_IS_CHILD_MAJOR(child_type))
	side_of_book = nb->notebook.major_pos;
    else if (NB_IS_CHILD_MINOR(child_type))
	side_of_book = nb->notebook.minor_pos;
    else
	return(False);

   /* Update JoinSide trait infomation, unless trait not found */
    if ((joinsideT=NB_JOINSIDE(child)) != NULL
	&& joinsideT->setValue != NULL)
	{
       /* Convert notebook position to JoinSide position */
	switch (side_of_book)
	    {
	    case RIGHT:
		side_to_join = XmLEFT;
		break;
	    case BOTTOM:
		side_to_join = XmTOP;
		break;
	    case LEFT:
		side_to_join = XmRIGHT;
		break;
	    case TOP:
	    default:
		side_to_join = XmBOTTOM;
		break;
	    }
	/* Invoke JoinSideSetValues */
	joinsideT->setValue(child, side_to_join, shadow_thickness);
	return(True);
	}

    return(False);
}


/*- UpdateJoinSideChildren --------------------------------------------------

        Update JoinSide aware children of new shadow_thickness or JoinSide.

-----------------------------------------------------------------------------*/
static void
UpdateJoinSideChildren (
    XmNotebookWidget nb,
    Dimension shadow_thickness)
{
    Widget cw;				/* child widget */
    unsigned char ct;			/* child constraint type */
    int i;

    for (i=0; i < nb->composite.num_children; i++)
	{
	cw = nb->composite.children[i];
        ct = NotebookConstraint(cw)->child_type;
	if (NB_IS_CHILD_TAB(ct))
	    (void) UpdateJoinSide(nb, cw, ct, shadow_thickness);
	}
}


/*- SetPageScroller ---------------------------------------------------------

	Set the visual configuration of the page scroller.

	It creates the default page scroller if no page scroller
	is created yet (consisting of a SpinBox with TextField child).

-----------------------------------------------------------------------------*/
static void
SetPageScroller (
    XmNotebookWidget nb)
{

    /* Only create default page scroller if none currently exists */
    if (nb->notebook.scroller_status != DEFAULT_NONE)
    	return;

    /* Mark that we are in the process of creating the default */
    nb->notebook.scroller_status = DEFAULT_CREATE;

    nb->notebook.scroller = XtVaCreateManagedWidget(PAGE_SCROLLER_NAME,
		  xmSpinBoxWidgetClass, (Widget)nb,
		  XmNarrowLayout, XmARROWS_SPLIT,
		  XmNnotebookChildType, XmPAGE_SCROLLER,
		  NULL);

    nb->notebook.scroller_child = XtVaCreateManagedWidget(NB_TEXT_FIELD_NAME,
		  xmTextFieldWidgetClass, nb->notebook.scroller,
		  XmNspinBoxChildType, XmNUMERIC,
		  XmNcolumns, 6,
		  XmNmarginHeight, 2,
		  XmNcursorPositionVisible, False,
		  XmNeditable, False,
		  XmNtraversalOn, True,
		  NULL);

    /* Mark that the default page scroller has been established */
    nb->notebook.scroller_status = DEFAULT_USED;
}



/*****************************************************************************
 *                                                                           *
 *			   Child Layout Functions			     *
 *                                                                           *
 *****************************************************************************/


/*- LayoutChildren ----------------------------------------------------------

        Function to layout Notebook's children

-----------------------------------------------------------------------------*/
static void
LayoutChildren (
    XmNotebookWidget nb,
    Widget instigator)
{
    /* reset top pointers of tabs */
    ResetTopPointers(nb, XmNONE, 0);

    /* layout children */
    LayoutPages(nb, instigator);
    LayoutMajorTabs(nb, instigator);
    LayoutMinorTabs(nb, instigator);
}


/*- LayoutPages -------------------------------------------------------------

	Function to layout Page, Status, and Page Scroller.

	Uses instance state:
		current_page_number,
		orientation, and
		precalculated sizes of each child type.

        Called from:
	    LayoutChildren(), when performing complete relayout
	    ConstraintSetValues(), when child page number changes
	    GotoPage(), when notebook.current_page_number changes

-----------------------------------------------------------------------------*/
static void
LayoutPages (
    XmNotebookWidget nb,
    Widget instigator)
{
    XmNotebookConstraint nc;
    Widget child;
    int i;
    Dimension x, y, x1, y1, x2;

    /* get the page's x, y position */
    x = nb->notebook.margin_width + nb->notebook.shadow_thickness + 1;
    y = nb->notebook.margin_height + nb->notebook.shadow_thickness + 1;
    if (nb->notebook.binding_pos == LEFT)
	x += nb->notebook.real_binding_width;
    else if (nb->notebook.binding_pos == TOP)
	y += nb->notebook.real_binding_width;
    if (nb->notebook.major_pos == LEFT)
	x += NB_MAJOR_MAX(nb, nb->notebook.major_width,
	    nb->notebook.major_scroller_width) + nb->notebook.back_page_size;
    else if (nb->notebook.major_pos == TOP)
	y += NB_MAJOR_MAX(nb, nb->notebook.major_height,
	    nb->notebook.major_scroller_height) + nb->notebook.back_page_size;
    if (nb->notebook.minor_pos == LEFT)
	x += NB_MINOR_MAX(nb, nb->notebook.minor_width,
	    nb->notebook.minor_scroller_width) + nb->notebook.back_page_size;
    else if (nb->notebook.minor_pos == TOP)
	y += NB_MINOR_MAX(nb, nb->notebook.minor_height,
	    nb->notebook.minor_scroller_height) + nb->notebook.back_page_size;

    /* get the status area and the page scroller's position */
    y1 = y + nb->notebook.page_height + nb->notebook.margin_height;
    if (nb->notebook.back_page_pos == XmTOP_RIGHT ||
	nb->notebook.back_page_pos == XmBOTTOM_RIGHT)
    {
	x1 = x;
	x2 = x + nb->notebook.page_width - nb->notebook.scroller_width;
    }
    else
    {
        x1 = x + nb->notebook.page_width - nb->notebook.status_width;
	x2 = x;
    }

    /* resize pages & status */
    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);
        if (   NB_IS_CHILD_PAGE(nc->child_type)
	    || NB_IS_CHILD_STATUS(nc->child_type) )
	    {
	    if (nc->active &&
		nc->page_number == nb->notebook.current_page_number)
		{
		if (NB_IS_CHILD_PAGE(nc->child_type))
		    ShowChild(child, instigator, x, y,
				nb->notebook.page_width,
				nb->notebook.page_height);
		else if (NB_IS_CHILD_STATUS(nc->child_type))
		    ShowChild(child, instigator, x1, y1,
				nb->notebook.status_width,
				nb->notebook.status_height);
		}
	    else
		{
	        /* This is to get HideChild to function properly on
		   widgets such as Form which fail to properly set
		   default width/height to a non-zero value */
		if (nb->notebook.first_change_managed
		    && (!XtWidth(child) && !XtHeight(child)))
		   {
		   XtWidth(child) = 10;
		   XtHeight(child) = 10;
		   HideChild(child, instigator);
		   XtWidth(child) = 0;
                   XtHeight(child) = 0;
		   }
		else
		   HideChild(child, instigator);
		}
	    }
	}

    /* Show the page scroller */
    if (nb->notebook.scroller != NULL)
    {
	child = nb->notebook.scroller;
	ShowChild(child, instigator, x2, y1,
			nb->notebook.scroller_width,
			nb->notebook.scroller_height);
    }
}


/*- LayoutMajorTabs ---------------------------------------------------------

        Function to layout major tabs

-----------------------------------------------------------------------------*/
static void
LayoutMajorTabs (
    XmNotebookWidget nb,
    Widget instigator)
{
    XmNotebookConstraint nc;		/* constraint pointer */
    Widget child;			/* a major tab widget */
    Widget join_child = NULL;		/* top major and joinSide tab */
    int x, y, tmpx, tmpy;		/* tab's geometry */
    int tab_count;			/* # of tabs shown */
    int margin_w, margin_h;		/* temporary margin value */
    int sx1, sy1, sx2, sy2;		/* tab scroller position */
    int fixed;				/* back page fixed position */
    int limit;				/* the last possible position */
    int delta, spacing;			/* major tab spacing */
    int first_major, top_major;		/* page# of first and top major tabs */
    unsigned char prev, next;		/* for tab scroller arrow direction */
    Boolean gray_prev, gray_next;	/* True if need to gray out scrollers */
    int i;
    unsigned char direction;
    int total_major_tabs, top_tab_count = 0, current_tab = 0;
    /*
     * Initialize
     */
    first_major = nb->notebook.first_major ?
		NotebookConstraint(nb->notebook.first_major)->page_number :
		nb->notebook.first_page_number - 1;
    top_major = nb->notebook.top_major ?
		NotebookConstraint(nb->notebook.top_major)->page_number :
		nb->notebook.first_page_number - 1;
    gray_prev = gray_next = True;
    sx1 = sy1 = sx2 = sy2 = 0;

    /*
     * Determine tab margin spacing and tab staggering
     */
    spacing = MAX(nb->notebook.major_spacing, nb->notebook.shadow_thickness);
    delta = nb->notebook.back_page_size / nb->notebook.real_back_page_number;

    /*
     * Determine tab positions
     */
    if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
        nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
        x = nb->notebook.real_binding_width + nb->notebook.frame_width;
	y = nb->notebook.back_page_size;
	fixed = x + nb->notebook.back_page_size;
	limit = nb->notebook.frame_height -
		nb->notebook.major_height -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
	    sx1 = sx2 = fixed;
            sy1 = y;
            sy2 = sy1 + nb->notebook.frame_height -
		  nb->notebook.major_scroller_height;
	    limit = nb->notebook.frame_height -
		    MAX(0, (int)(nb->notebook.major_scroller_height -
			   nb->notebook.back_page_size)) -
		    nb->notebook.major_height -
		    nb->notebook.shadow_thickness;
	    y += nb->notebook.major_scroller_height + spacing;
	    }
	else
	    y += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_UP;
        next = XmARROW_DOWN;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
	     nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
        x = nb->notebook.back_page_size;
        y = nb->notebook.real_binding_width + nb->notebook.frame_height;
        fixed = y + nb->notebook.back_page_size;
        limit = nb->notebook.frame_width -
                nb->notebook.major_width -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sy1 = sy2 = fixed;
            sx1 = x;
            sx2 = sx1 + nb->notebook.frame_width -
 		  nb->notebook.major_scroller_width;
            limit = nb->notebook.frame_width -
		    MAX(0, (int)(nb->notebook.major_scroller_width -
			   nb->notebook.back_page_size)) -
		    nb->notebook.major_width -
		    nb->notebook.shadow_thickness;
            x += nb->notebook.major_scroller_width + spacing;
	    }
	else
	    x += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_LEFT;
        next = XmARROW_RIGHT;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
	     nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
	margin_w = NB_MAJOR_MAX(nb, nb->notebook.major_width,
				nb->notebook.major_scroller_width) +1;
        x = margin_w + nb->notebook.back_page_size - nb->notebook.major_width;
        y = nb->notebook.back_page_size;
        fixed = x - nb->notebook.back_page_size;
        limit = nb->notebook.frame_height -
                nb->notebook.major_height -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = margin_w - nb->notebook.minor_scroller_width;
            sy1 = y;
            sy2 = y + nb->notebook.frame_height -
		  nb->notebook.major_scroller_height;
            limit = nb->notebook.frame_height -
		    MAX(0, (int)(nb->notebook.major_scroller_height -
			   nb->notebook.back_page_size)) -
		    nb->notebook.major_height -
		    nb->notebook.shadow_thickness;
            y += nb->notebook.major_scroller_height + spacing;
	    }
	else
	    y += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_UP;
        next = XmARROW_DOWN;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
             nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
	margin_w = NB_MINOR_MAX(nb, nb->notebook.minor_width,
		   	 	nb->notebook.minor_scroller_width);
        x = margin_w + nb->notebook.frame_width - nb->notebook.major_width;
        y = nb->notebook.real_binding_width + nb->notebook.frame_height;
        fixed = y + nb->notebook.back_page_size;
        limit = margin_w + nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sy1 = sy2 = fixed;
	    sx1 = margin_w + nb->notebook.frame_width -
		  nb->notebook.major_scroller_width;
	    sx2 = margin_w;
            limit = margin_w +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.major_scroller_width) +
		    nb->notebook.shadow_thickness;
            x -= nb->notebook.major_scroller_width + spacing;
	    }
	else
	    x -= spacing;

	/* set arrow direction resource */
	prev = XmARROW_RIGHT;
	next = XmARROW_LEFT;
	}
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
        nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
	margin_h = NB_MINOR_MAX(nb, nb->notebook.minor_height,
				nb->notebook.minor_scroller_height);
        x = nb->notebook.real_binding_width + nb->notebook.frame_width;
        y = margin_h + nb->notebook.frame_height - nb->notebook.major_height;
        fixed = x + nb->notebook.back_page_size;
        limit = margin_h + nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = fixed;
            sy1 = margin_h + nb->notebook.frame_height -
		  nb->notebook.minor_scroller_height;
            sy2 = margin_h;
            limit = margin_h +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.major_scroller_height) +
		    nb->notebook.shadow_thickness;
            y -= nb->notebook.major_scroller_height + spacing;
	    }
	else
	    y -= spacing;

        /* set arrow direction resource */
        prev = XmARROW_DOWN;
        next = XmARROW_UP;
	}
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
             nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
	margin_h = NB_MAJOR_MAX(nb, nb->notebook.major_height,
		nb->notebook.major_scroller_height) +1;
        x = nb->notebook.back_page_size;
        y = margin_h + nb->notebook.back_page_size - nb->notebook.major_height;
        fixed = margin_h - nb->notebook.major_height;
        limit = nb->notebook.frame_width -
		nb->notebook.major_width -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sy1 = sy2 = margin_h - nb->notebook.major_scroller_height;
            sx1 = x;
            sx2 = nb->notebook.frame_width +
		  nb->notebook.back_page_size -
		  nb->notebook.major_scroller_width;
            limit = nb->notebook.frame_width -
		    MAX(0, (int)(nb->notebook.major_scroller_width -
			   nb->notebook.back_page_size)) -
		    nb->notebook.major_width -
		    nb->notebook.shadow_thickness;
            x += nb->notebook.major_scroller_width + spacing;
	    }
	else
	    x += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_LEFT;
        next = XmARROW_RIGHT;
	}
    else if (nb->notebook.back_page_pos == XmTOP_LEFT &&
             nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
        margin_w = NB_MAJOR_MAX(nb, nb->notebook.major_width,
				nb->notebook.major_scroller_width) +1;
	margin_h = NB_MINOR_MAX(nb, nb->notebook.minor_height,
				nb->notebook.minor_scroller_height) +1;
        x = margin_w + nb->notebook.back_page_size - nb->notebook.major_width;
        y = margin_h + nb->notebook.frame_height - nb->notebook.major_height;
        fixed = x - nb->notebook.back_page_size;
        limit = margin_h +
		nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = margin_w - nb->notebook.minor_scroller_width;
            sy1 = margin_h + nb->notebook.frame_height -
		nb->notebook.major_scroller_height;
            sy2 = margin_h;
            limit = margin_h +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.major_scroller_height) +
		    nb->notebook.shadow_thickness;
            y -= nb->notebook.major_scroller_height + spacing;
	    }
	else
	    y -= spacing;

        /* set arrow direction resource */
        prev = XmARROW_DOWN;
        next = XmARROW_UP;
	}
    else
	{
        /* set starting position */
        margin_w = NB_MINOR_MAX(nb, nb->notebook.minor_width,
				nb->notebook.minor_scroller_width) +1;
	margin_h = NB_MAJOR_MAX(nb, nb->notebook.major_height,
				nb->notebook.major_scroller_height) +1;
        x = margin_w + nb->notebook.frame_width - nb->notebook.major_width;
        y = margin_h + nb->notebook.back_page_size - nb->notebook.major_height;
        fixed = y - nb->notebook.back_page_size;
        limit = margin_w + nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    {
	    sy1 = sy2 = margin_h - nb->notebook.major_scroller_height;
	    sx1 = margin_w + nb->notebook.frame_width -
		nb->notebook.major_scroller_width;
            sx2 = margin_w;
            limit = margin_w +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.major_scroller_width) +
		    nb->notebook.shadow_thickness;
            x -= nb->notebook.major_scroller_width + spacing;
	    }
	else
	    x -= spacing;

        /* set arrow direction resource */
        prev = XmARROW_RIGHT;
        next = XmARROW_LEFT;
	}



    /*
     * Deal with previous top and current top tab shadowing
     */

    /* Inform old top major to restore shadow_thickness */
    if (nb->notebook.old_top_major != NULL
	&& nb->notebook.old_top_major != nb->notebook.top_major)
	{
	/* Protect against geometry requests during join changes */
	nb->notebook.in_setshadow = True;

	/* Inform join-aware tab else restore shadow thickness */
	if (!UpdateJoinSide(nb, nb->notebook.old_top_major,
			    XmMAJOR_TAB, (Dimension)0))
	    {
	    /* Hide the old top major shadow */
	    HideShadowedTab(nb, nb->notebook.old_top_major);

	    /* Restore child shadow thickness */
	    XtVaSetValues(nb->notebook.old_top_major,
		    XmNshadowThickness, nb->notebook.major_shadow_thickness,
		    NULL);
	    }

	/* Release protection against geometry requests */
	nb->notebook.in_setshadow = False;
	}


    /* Store & set new top major's shadow */
    if (nb->notebook.top_major != NULL)
	{
	/* Hide the top major shadow, as it may be moving (shown or hidden) */
	HideShadowedTab(nb, nb->notebook.top_major);

	/* Protect against geometry requests during join changes */
	nb->notebook.in_setshadow = True;

	/* Set shadow thickness of new top tab */
	if (!UpdateJoinSide(nb, nb->notebook.top_major,
			XmMAJOR_TAB, nb->notebook.shadow_thickness))
	    {
	    /* Save shadow_thickness of top tab, unless we already have */
	    if (nb->notebook.top_major != nb->notebook.major_shadow_child)
		{
		XtVaGetValues(nb->notebook.top_major,
		    XmNshadowThickness, &(nb->notebook.major_shadow_thickness),
		    NULL);
		nb->notebook.major_shadow_child = nb->notebook.top_major;
		}
	    else
	      {
		/* CR 9903: The application may have set shadow_thickness. */
		Dimension current = 0;
		XtVaGetValues(nb->notebook.top_major,
			      XmNshadowThickness, &current,
			      NULL);
		if (current != 0)
		  nb->notebook.major_shadow_thickness = current;
	      }

	    /* Notebook will draw shadow for non-JoinSide trait tabs */
	    XtVaSetValues(nb->notebook.top_major,
		    XmNshadowThickness, 0,
		    NULL);
	    }
	else
	    join_child = nb->notebook.top_major;

	/* Release protection against geometry requests */
	nb->notebook.in_setshadow = False;
	}
    else
	/* There is no top tab so advance to the first back page line */
	{
	if (nb->notebook.major_pos == RIGHT)
	    x += delta;
	else if (nb->notebook.major_pos == LEFT)
	    x -= delta;
	else if (nb->notebook.major_pos == BOTTOM)
	    y += delta;
	else /* if (nb->notebook.major_pos == TOP) */
	    y -= delta;
	}
    /*
     * FIX for CDExc23791
     */
    total_major_tabs = 0;
    for (i = 0; i < nb->composite.num_children; i++)
        {
        child = nb->composite.children[i];
        nc = NotebookConstraint(child);
        if (NB_IS_CHILD_MAJOR(nc->child_type))
                total_major_tabs++;
        if ( child == nb->notebook.top_major)
                top_tab_count = total_major_tabs;
        }
    /*
     * Layout major tabs
     */
    tab_count = 0;
    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);

        if (NB_IS_CHILD_MAJOR(nc->child_type))
	    {
	    if (!nc->active)
		HideChild(child, instigator);
	    else if (nc->page_number < first_major)
		{
		HideChild(child, instigator);
		gray_prev = False;
		}
	    else if ((nb->notebook.minor_pos == BOTTOM && y > limit)
		     || (nb->notebook.minor_pos == TOP && y < limit)
		     || (nb->notebook.minor_pos == RIGHT && x > limit)
		     || (nb->notebook.minor_pos == LEFT && x < limit))
		{
		HideChild(child, instigator);
		gray_next = False;
		}
	    else
	     {
               /* Display the tab */
               if (  current_tab < top_tab_count - 1 )
                 {
                 /*
                  * FIX for CDExc23791
                  */
                 if (nb->notebook.orientation == XmHORIZONTAL)
                   {
                   if (nb->notebook.major_pos == RIGHT)
                     tmpx = x + ( total_major_tabs + 1 - top_tab_count + current_tab) * delta;
                   else
                     tmpx = x - ( total_major_tabs + 1 - top_tab_count + current_tab) * delta;
                   tmpy = y;
                   }
                  else
                  {
                    tmpx = x;
                    if (nb->notebook.major_pos == TOP)
                      tmpy = y - (total_major_tabs + 1 - top_tab_count + current_tab) * delta;
                    else
                      tmpy = y + (total_major_tabs + 1 - top_tab_count + current_tab) * delta;
                  }
                }
              else
                {
                tmpx = x;
                tmpy = y;
                }

		if (/*(nb->notebook.orientation == XmHORIZONTAL &&
		    nc->page_number < top_major)
		    ||*/ (nb->notebook.major_pos == RIGHT && tmpx > fixed)
		    || (nb->notebook.major_pos == LEFT && tmpx < fixed))
		    {
		    tmpx = fixed;
		    }
		else if (/*(nb->notebook.orientation == XmVERTICAL &&
		     nc->page_number < top_major)
		    ||*/ (nb->notebook.major_pos == BOTTOM && tmpy > fixed)
		    || (nb->notebook.major_pos == TOP && tmpy < fixed))
		    {
		    tmpy = fixed;
		    }

		/* move joinSide top tab over to align with frame */
		if ((join_child && (join_child == child))
		||  (nb->notebook.top_major
			&& (nb->notebook.top_major == child)))
		    {
		    if (nb->notebook.major_pos == LEFT)
			tmpx = tmpx + nb->notebook.shadow_thickness;
		    else if (nb->notebook.major_pos == RIGHT)
			tmpx = tmpx - nb->notebook.shadow_thickness;
		    else if (nb->notebook.major_pos == TOP)
			tmpy = tmpy + nb->notebook.shadow_thickness;
		    else /* (nb->notebook.major_pos == BOTTOM) */
			tmpy = tmpy - nb->notebook.shadow_thickness;
		    }

		/* Make tab visible */
		ShowChild(child, instigator, tmpx, tmpy,
			    nb->notebook.major_width,
			    nb->notebook.major_height);
		tab_count++;
                /* Calculate the next position */
		if (nb->notebook.minor_pos == BOTTOM)
                    y += nb->notebook.major_height + spacing;
		else if (nb->notebook.minor_pos == TOP)
		    y -= nb->notebook.major_height + spacing;
		else if (nb->notebook.minor_pos == RIGHT)
                    x += nb->notebook.major_width + spacing;
		else /* (nb->notebook.minor_pos == LEFT) */
		    x -= nb->notebook.major_width + spacing;

		}

            /* to the next back page line */
	    if (nc->page_number >= top_major)
		{
		if (nb->notebook.major_pos == RIGHT)
                    x += delta;
		else if (nb->notebook.major_pos == LEFT)
		    x -= delta;
		else if (nb->notebook.major_pos == BOTTOM)
		    y += delta;
		else
		    y -= delta;
		}
		current_tab++;
	} /* if */
    } /* for */


    /*
     * Display the tab scrolling tab, if neccessary
     */
    if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller) && tab_count)
	{

	/* set scroller's arrow directions */
	XtVaGetValues(nb->notebook.prev_major,
	     XmNarrowDirection, &direction, NULL);
	if (direction != prev)
             XtVaSetValues(nb->notebook.prev_major,
                XmNarrowDirection, prev, NULL);
	XtVaGetValues(nb->notebook.next_major,
	     XmNarrowDirection, &direction, NULL);
	if (direction != next)
           XtVaSetValues(nb->notebook.next_major,
             XmNarrowDirection, next, NULL);

	/* Gray out scrollers, if necessary */
	if (gray_prev)
	    XtSetSensitive(nb->notebook.prev_major, False);
	else
	    XtSetSensitive(nb->notebook.prev_major, True);

	if (gray_next)
	    XtSetSensitive(nb->notebook.next_major, False);
	else
	    XtSetSensitive(nb->notebook.next_major, True);

	/* show the scrollers */
        ShowChild(nb->notebook.prev_major, instigator, sx1, sy1,
			nb->notebook.major_scroller_width,
			nb->notebook.major_scroller_height);
        ShowChild(nb->notebook.next_major, instigator, sx2, sy2,
			nb->notebook.major_scroller_width,
			nb->notebook.major_scroller_height);
	}
    else
	{
        HideChild(nb->notebook.prev_major, instigator);
        HideChild(nb->notebook.next_major, instigator);
	}
}


/*- LayoutMinorTabs ---------------------------------------------------------

        Function to layout minor tabs

-----------------------------------------------------------------------------*/
static void
LayoutMinorTabs (
    XmNotebookWidget nb,
    Widget instigator)
{
    XmNotebookConstraint nc;		/* constraint pointer */
    Widget child;			/* a major tab widget */
    Widget join_child = NULL;		/* top minor and joinSide tab */
    int x, y, tmpx, tmpy;		/* tab's geometry */
    int tab_count;                      /* # of tabs shown */
    int backpage_h;			/* temporary backpage value */
    int margin_w, margin_h;		/* temporary margin value */
    int sx1, sy1, sx2, sy2;		/* tab scroller position */
    int fixed;				/* back page fixed position */
    int limit;				/* the last possible position */
    int spacing;			/* major tab spacing */
    int status;				/* minor tab drawing status */
    int top_major;			/* page# of the top major tab */
    int first_minor, top_minor;		/* page# of first and top minor tabs */
    unsigned char prev, next;		/* for tab scroller arrow direction */
    Boolean gray_prev, gray_next;	/* True if need to gray out scrollers */
    int i;
    unsigned char direction;
    int offset = 0;
    /*
     * Initialize
     */
    top_major = nb->notebook.top_major
		? NotebookConstraint(nb->notebook.top_major)->page_number
		: nb->notebook.first_page_number - 1;
    first_minor = nb->notebook.first_minor
		? NotebookConstraint(nb->notebook.first_minor)->page_number
		: nb->notebook.first_page_number - 1;
    top_minor = nb->notebook.top_minor
		? NotebookConstraint(nb->notebook.top_minor)->page_number
		: nb->notebook.first_page_number - 1;
    gray_prev = gray_next = True;
    sx1 = sy1 = sx2 = sy2 = 0;

    if ( nb->notebook.shadow_thickness == 0 )
       offset = 1;

    /*
     * Determine tap spacing
     */
    spacing = MAX(nb->notebook.minor_spacing, nb->notebook.shadow_thickness);

    /*
     * Determine tab positions
     */
    if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT
        && nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
        x = MAX((nb->notebook.real_binding_width + 1 + offset),
		nb->notebook.back_page_size);
	y = nb->notebook.frame_height;
	fixed = y + NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
	limit = nb->notebook.real_binding_width +
		nb->notebook.frame_width -
		nb->notebook.minor_width -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
	    sy1 = sy2 = y + nb->notebook.back_page_size;
	    sx1 = x;
	    sx2 = nb->notebook.real_binding_width +
		  nb->notebook.frame_width +
		  nb->notebook.back_page_size -
		  nb->notebook.minor_scroller_width;
	    limit = nb->notebook.real_binding_width +
		    nb->notebook.frame_width -
		    nb->notebook.minor_width -
		    nb->notebook.shadow_thickness -
		    MAX(0, (int)(nb->notebook.minor_scroller_width -
			   nb->notebook.back_page_size));
	    x += nb->notebook.minor_scroller_width + spacing;
	    }
	else
	    x += nb->notebook.shadow_thickness;

	/* set arrow direction resource */
	prev = XmARROW_LEFT;
	next = XmARROW_RIGHT;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT
	     && nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
	backpage_h = 1;
	if (nb->notebook.back_page_size > nb->notebook.real_binding_width)
	    backpage_h = nb->notebook.back_page_size -
			 nb->notebook.real_binding_width;
        x = nb->notebook.frame_width;
        y = nb->notebook.real_binding_width + backpage_h + offset;
        fixed = x + NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
        limit = nb->notebook.real_binding_width +
		nb->notebook.frame_height -
		nb->notebook.minor_height -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = x + nb->notebook.back_page_size;
            sy1 = y;
            sy2 = nb->notebook.real_binding_width +
		  nb->notebook.frame_height +
		  nb->notebook.back_page_size -
		  nb->notebook.minor_scroller_height;
            limit = nb->notebook.real_binding_width +
		    nb->notebook.frame_height -
		    MAX(0, (int)(nb->notebook.minor_scroller_height -
			   nb->notebook.back_page_size)) -
		    nb->notebook.minor_height -
		    nb->notebook.shadow_thickness;
            y += nb->notebook.minor_scroller_height + spacing;
	    }
	else
	    y += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_UP;
        next = XmARROW_DOWN;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT
	     && nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
	margin_w = NB_MAJOR_MAX(nb, nb->notebook.major_width,
				nb->notebook.major_scroller_width);
        x = margin_w +
	    nb->notebook.back_page_size +
	    nb->notebook.frame_width -
	    MAX(0, (int)(nb->notebook.back_page_size -
		   nb->notebook.real_binding_width)) -
	    nb->notebook.minor_width - offset;
        y = nb->notebook.frame_height;
        fixed = y + NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
	limit = margin_w +
		nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sy1 = sy2 = y + nb->notebook.back_page_size;
            sx1 = margin_w + nb->notebook.back_page_size +
	 	  nb->notebook.frame_width -
		  MAX(0, (int)(nb->notebook.back_page_size -
		         nb->notebook.real_binding_width)) -
		  nb->notebook.minor_scroller_width;
            sx2 = margin_w;
            limit = margin_w +
		    MAX(nb->notebook.minor_scroller_width,
			nb->notebook.back_page_size) +
		    nb->notebook.shadow_thickness;
            x -= nb->notebook.minor_scroller_width + spacing;
	    }
	else
	    x -= nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_RIGHT;
        next = XmARROW_LEFT;
	}
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT
             && nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
	margin_w = NB_MINOR_MAX(nb, nb->notebook.minor_width,
				nb->notebook.minor_scroller_width) +1;
	x = margin_w + nb->notebook.back_page_size - nb->notebook.minor_width;
        y = MAX((nb->notebook.real_binding_width + 1 + offset),
		nb->notebook.back_page_size);
	fixed = margin_w +
		NB_MINOR_TAB_STEP(nb->notebook.back_page_size) -
		nb->notebook.minor_width;
        limit = nb->notebook.real_binding_width +
		nb->notebook.frame_height -
		nb->notebook.minor_height -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = margin_w - nb->notebook.minor_scroller_width;
            sy1 = y;
            sy2 = nb->notebook.real_binding_width +
		  nb->notebook.frame_height +
		  nb->notebook.back_page_size -
		  nb->notebook.minor_scroller_height;
            limit = nb->notebook.real_binding_width +
		    nb->notebook.frame_height -
		    MAX(0, (int)(nb->notebook.minor_scroller_height -
			   nb->notebook.back_page_size)) -
		    nb->notebook.minor_height -
		    nb->notebook.shadow_thickness;
            y += nb->notebook.minor_scroller_height + spacing;
	    }
	else
	    y += nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_UP;
        next = XmARROW_DOWN;
	}
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT
        && nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
        margin_h = NB_MINOR_MAX(nb, nb->notebook.minor_height,
				nb->notebook.minor_scroller_height) + 1;
        x = MAX((nb->notebook.real_binding_width + 1 + offset),
		nb->notebook.back_page_size);
        y = margin_h + nb->notebook.back_page_size - nb->notebook.minor_height;
        fixed = margin_h + NB_MINOR_TAB_STEP(nb->notebook.back_page_size) -
		nb->notebook.minor_height;
        limit = nb->notebook.real_binding_width +
		nb->notebook.frame_width -
		nb->notebook.minor_width -
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sy1 = sy2 = margin_h - nb->notebook.minor_scroller_height;
            sx1 = x;
            sx2 = nb->notebook.real_binding_width +
		  nb->notebook.frame_width +
		  nb->notebook.back_page_size -
		  nb->notebook.minor_scroller_width;
            limit = nb->notebook.real_binding_width +
		    nb->notebook.frame_width -
		    MAX(0, (int)(nb->notebook.minor_scroller_width -
			   nb->notebook.back_page_size)) -
		    nb->notebook.minor_width -
		    nb->notebook.shadow_thickness;
	    x += nb->notebook.minor_scroller_width + spacing;
	    }
	else
	    x += nb->notebook.shadow_thickness;

	/* set arrow direction resource */
	prev = XmARROW_LEFT;
	next = XmARROW_RIGHT;
	}
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT
             && nb->notebook.orientation == XmVERTICAL)
	{
        /* set starting position */
	backpage_h = 0;
	if (nb->notebook.back_page_size > nb->notebook.real_binding_width)
	    backpage_h = nb->notebook.back_page_size -
			 nb->notebook.real_binding_width;
	margin_h = NB_MAJOR_MAX(nb, nb->notebook.major_height,
				nb->notebook.major_scroller_height);
        x = nb->notebook.frame_width;
        y = margin_h + nb->notebook.back_page_size +
		nb->notebook.frame_height -
		nb->notebook.minor_height - backpage_h - offset;
        fixed = x + NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
        limit = margin_h + nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = x + nb->notebook.back_page_size;
            sy1 = margin_h + nb->notebook.back_page_size +
		  nb->notebook.frame_height -
		  nb->notebook.minor_scroller_height - backpage_h;
            sy2 = margin_h;
            limit = margin_h +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.minor_scroller_height) +
		    nb->notebook.shadow_thickness;
            y -= (nb->notebook.minor_scroller_height + spacing);
	    }
	else
	    y -= nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_DOWN;
        next = XmARROW_UP;
	}
    else if (nb->notebook.back_page_pos == XmTOP_LEFT
             && nb->notebook.orientation == XmHORIZONTAL)
	{
        /* set starting position */
        margin_w = NB_MAJOR_MAX(nb, nb->notebook.major_width,
				nb->notebook.major_scroller_width);
        margin_h = NB_MINOR_MAX(nb, nb->notebook.minor_height,
				    nb->notebook.minor_scroller_height) +1;
        x = margin_w +
	    nb->notebook.back_page_size +
	    nb->notebook.frame_width -
	    MAX(0, (int)(nb->notebook.back_page_size -
		   nb->notebook.real_binding_width)) -
	    nb->notebook.minor_width - offset;
        y = margin_h + nb->notebook.back_page_size - nb->notebook.minor_height;
        fixed = y - NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
        limit = margin_w +
		nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
        {
            sy1 = sy2 = margin_h - nb->notebook.minor_scroller_height;
            sx1 = margin_w +
		  nb->notebook.back_page_size +
		  nb->notebook.frame_width -
		  MAX(0, (int)(nb->notebook.back_page_size -
		         nb->notebook.real_binding_width)) -
		  nb->notebook.minor_scroller_width;
	    sx2 = margin_w;
	    limit = margin_w +
		    MAX(nb->notebook.minor_scroller_width,
			   nb->notebook.back_page_size) +
		    nb->notebook.shadow_thickness;
            x -= nb->notebook.minor_scroller_width + spacing;
        }
	else
	    x -= nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_RIGHT;
        next = XmARROW_LEFT;
	}
    else /* if (nb->notebook.back_page_pos == XmTOP_LEFT
	    && nb->notebook.orientation == XmVERTICAL) */
	{
        /* set starting position */
        margin_w = NB_MINOR_MAX(nb, nb->notebook.minor_width,
				nb->notebook.minor_scroller_width) + 1;
        margin_h = NB_MAJOR_MAX(nb, nb->notebook.major_height,
				nb->notebook.major_scroller_height);
        x = margin_w + nb->notebook.back_page_size - nb->notebook.minor_width;
        y = margin_h +
	    nb->notebook.back_page_size +
	    nb->notebook.frame_height -
	    nb->notebook.minor_height -
	    MAX(0, (int)(nb->notebook.back_page_size -
		   nb->notebook.real_binding_width)) - offset;
        fixed = x - NB_MINOR_TAB_STEP(nb->notebook.back_page_size);
        limit = margin_h + nb->notebook.back_page_size +
		nb->notebook.shadow_thickness;
        if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller))
	    {
            sx1 = sx2 = margin_w - nb->notebook.minor_scroller_width;
            sy1 = margin_h +
		  nb->notebook.back_page_size +
		  nb->notebook.frame_height -
		  MAX(0, (int)(nb->notebook.back_page_size -
		         nb->notebook.real_binding_width)) -
		  nb->notebook.minor_scroller_height;
            sy2 = margin_h;
            limit = margin_h +
		    MAX(nb->notebook.back_page_size,
		        nb->notebook.minor_scroller_height) +
		    nb->notebook.shadow_thickness;
            y -= nb->notebook.minor_scroller_height + spacing;
	    }
	else
	    y -= nb->notebook.shadow_thickness;

        /* set arrow direction resource */
        prev = XmARROW_DOWN;
        next = XmARROW_UP;
	}

    /*
     * Deal with previous top and current top tab shadowing
     */

    /* Inform old top minor to restore shadow_thickness */
    if (nb->notebook.old_top_minor != NULL
	&& nb->notebook.old_top_minor != nb->notebook.top_minor)
        {
        /* Protect against geometry requests during join changes */
        nb->notebook.in_setshadow = True;

        /* Inform join-aware tab else restore shadow thickness */
        if (!UpdateJoinSide(nb, nb->notebook.old_top_minor,
				XmMINOR_TAB, (Dimension)0))
	    {
	    /* Hide the old top minor shadow */
	    HideShadowedTab(nb, nb->notebook.old_top_minor);

	    /* Restore the old top minor shadow thickness */
            XtVaSetValues(nb->notebook.old_top_minor,
                    XmNshadowThickness, nb->notebook.minor_shadow_thickness,
                    NULL);
	    }

        /* Release protection against geometry requests */
        nb->notebook.in_setshadow = False;
        }


    /* Store & set new top minor's shadow */
    if (nb->notebook.top_minor != NULL)
        {
	/* Hide the top minor shadow, as it may be moving (shown or hidden) */
	HideShadowedTab(nb, nb->notebook.top_minor);

        /* Protect against geometry requests during join changes */
        nb->notebook.in_setshadow = True;

        /* Set shadow thickness of new top tab */
        if (!UpdateJoinSide(nb, nb->notebook.top_minor,
				XmMINOR_TAB, nb->notebook.shadow_thickness))
            {
            /* Save shadow_thickness of top tab, unless we already have */
	    if (nb->notebook.top_minor != nb->notebook.minor_shadow_child)
		{
		/* Save shadow_thickness of top tab, unless already saved */
		XtVaGetValues(nb->notebook.top_minor,
                    XmNshadowThickness, &(nb->notebook.minor_shadow_thickness),
                    NULL);
		nb->notebook.minor_shadow_child = nb->notebook.top_minor;
		}
	    else
	      {
		/* CR 9903: The application may have set shadow_thickness. */
		Dimension current = 0;
		XtVaGetValues(nb->notebook.top_minor,
			      XmNshadowThickness, &current,
			      NULL);
		if (current != 0)
		  nb->notebook.minor_shadow_thickness = current;
	      }

            /* Notebook will draw shadow */
            XtVaSetValues(nb->notebook.top_minor,
                    XmNshadowThickness, 0,
                    NULL);
            }
	else
	    join_child = nb->notebook.top_minor;

        /* Release protection against geometry requests */
	nb->notebook.in_setshadow = False;
        }


    /*
     * Layout major tabs
     */
    status = TAB_DRAW;
    tab_count = 0;
    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);

	if (first_minor < nb->notebook.first_page_number
	    || (nc->active
		&& NB_IS_CHILD_MAJOR(nc->child_type)
		&& (status == TAB_DRAWING || nc->page_number > first_minor)))
	    status = TAB_DRAWN;

        if (NB_IS_CHILD_MINOR(nc->child_type))
	    {
            if (!nc->active || status == TAB_DRAWN)
                HideChild(child, instigator);

            else if (nc->page_number < first_minor)
		{
		if (nc->page_number >= top_major)
		    gray_prev = False;
                HideChild(child, instigator);
		}
            else if ((nb->notebook.binding_pos == LEFT && x > limit)
		    || (nb->notebook.binding_pos == RIGHT && x < limit)
		    || (nb->notebook.binding_pos == TOP && y > limit)
		    || (nb->notebook.binding_pos == BOTTOM && y < limit))
		{
                gray_next = False;
                HideChild(child, instigator);
		}
	    else
		{
                /* Display the tab */
                status = TAB_DRAWING;
		tmpx = x;
		tmpy = y;
                if (nc->page_number != top_minor)
		    {
		    if (nb->notebook.orientation == XmHORIZONTAL)
			tmpy = fixed;
		    else
			tmpx = fixed;
		    }

		/* move joinSide top tab over to align with frame */
		if ((join_child && (join_child == child))
		||  (nb->notebook.top_minor
			&& (nb->notebook.top_minor == child)))
		    {
		    if (nb->notebook.minor_pos == LEFT)
			tmpx = tmpx + nb->notebook.shadow_thickness;
		    else if (nb->notebook.minor_pos == RIGHT)
			tmpx = tmpx - nb->notebook.shadow_thickness;
		    else if (nb->notebook.minor_pos == TOP)
			tmpy = tmpy + nb->notebook.shadow_thickness;
		    else if (nb->notebook.minor_pos == BOTTOM)
			tmpy = tmpy - nb->notebook.shadow_thickness;
		    }

		/* Make tab visible */
		ShowChild(child, instigator, tmpx, tmpy,
			    nb->notebook.minor_width,
			    nb->notebook.minor_height);
		tab_count++;

                /* Calculate the next position */
		if (nb->notebook.binding_pos == LEFT)
		    x += nb->notebook.minor_width + spacing;
		else if (nb->notebook.binding_pos == RIGHT)
		    x -= nb->notebook.minor_width + spacing;
		else if (nb->notebook.binding_pos == TOP)
		    y += nb->notebook.minor_height + spacing;
		else
		    y -= nb->notebook.minor_height + spacing;

		}
	    }
	}


    /* display the tab scrolling tab if neccessary */
    if (NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller) && tab_count)
	{

        /* set scroller's arrow directions */
        XtVaGetValues(nb->notebook.prev_minor,
                XmNarrowDirection, &direction, NULL);
        if (direction != prev)
            XtVaSetValues(nb->notebook.prev_minor,
                XmNarrowDirection, prev, NULL);
        XtVaGetValues(nb->notebook.next_minor,
                XmNarrowDirection, &direction, NULL);
        if (direction != next)
            XtVaSetValues(nb->notebook.next_minor,
                XmNarrowDirection, next, NULL);

        /* gray out scrollers if necessary */
        if (gray_prev)
            XtSetSensitive(nb->notebook.prev_minor, False);
        else
            XtSetSensitive(nb->notebook.prev_minor, True);
        if (gray_next)
            XtSetSensitive(nb->notebook.next_minor, False);
        else
            XtSetSensitive(nb->notebook.next_minor, True);

        /* show the scrollers */
        ShowChild(nb->notebook.prev_minor, instigator, sx1, sy1,
                        nb->notebook.minor_scroller_width,
                        nb->notebook.minor_scroller_height);
        ShowChild(nb->notebook.next_minor, instigator, sx2, sy2,
                        nb->notebook.minor_scroller_width,
                        nb->notebook.minor_scroller_height);
	}
    else
	{
        HideChild(nb->notebook.prev_minor, instigator);
        HideChild(nb->notebook.next_minor, instigator);
	}
}


/*- ResetTopPointers --------------------------------------------------------

    Function to calculate and update needed information to perform
    layout for major and minor tabs, and the tab scroller.

    Uses instance state:
	notebook.current_page_number

    Updates instance state:

	notebook.first_major 	- page# of the first visible major tab
	notebook.first_minor 	- page# of the first visible minor tab
	notebook.top_major 	- page# of the currently active major tab
	notebook.top_minor 	- page# of the currently active minor tab
	notebook.need_scroller	- tab scroller necessary, where
				  XmNONE indicates none,
				  XmMAJOR_TAB_SCROLLER indicates major only,
				  XmMINOR_TAB_SCROLLER indicates minor only,
				  XmTAB_SCROLLER indicates both are needed.

    Called from:
	LayoutChildren(), when performing layout
	GotoPage(), when changing the top pages
	FlipTabs(), when tab scrolling are activated
	TraverseTab(), when tab traverse actions are activated


    Parameters:
	IN reason 		- Why was this routine called?
		where
		XmNONE		= reset all information
		XmPAGE 		= reset due to a page navigation (general)
		XmPAGE_SCROLLER = reset due to navigating via the page scroller
		XmMAJOR_TAB 	= reset due to activating a minor tab
		XmMINOR_TAB 	= reset due to activating a major tab
		XmMAJOR_TAB_SCROLLER = reset due to major scroller activation
		XmMINOR_TAB_SCROLLER = reset due to minor scroller activation

	IN scroll 		- What type of scrolling
		where
		_NEXT		= scroll to the next major/minor tab
		_PREV		= scroll to the previous major/minor tab
		_HOME		= scroll to the first major/minor tab
		_END		= scroll to the last major/minor tab
		(only used for reason == NB_IS_CHILD_MAJOR_SCROLLER or
		NB_IS_CHILD_MINOR_SCROLLER)

-----------------------------------------------------------------------------*/
static void
ResetTopPointers (
    XmNotebookWidget nb,
    unsigned char reason,
    int scroll)
{
    XmNotebookConstraint nc;		/* constraint pointer */
    Widget child;			/* a tab widget */
    int i;				/* index */
    int spacing;			/* tab spacing */
    int half;
    int count;				/* temporary counting */
    Dimension frame_dim,
	      tab_dim,
	      scroller_dim; 		/* temporary w/h */

    /* major tab information */
    Widget start_major;		/* the very first major tab */
    Widget top_major;		/* the currently major tab */
    Widget next_major;		/* the next major tab of first_major */
    Widget prev_major;		/* the prev major tab of first_major */
    int first_major_page;	/* page# of the first visible major tab */
    int top_major_page;		/* page# of the current major tab */
    int top_major_idx;		/* top major tab's index in the child array */
    int num_major;		/* # of major tabs */
    int num_visible_major;	/* # of visible major tabs */
    int num_rest_major;		/* # of next major tabs of the current page */
    int num_next_major;		/* # of next major tabs of first_major */
    int num_prev_major;		/* # of prev major tabs of first_major */

    /* minor tab information */
    Widget start_minor;		/* the first minor tab in the current major */
    Widget top_minor;		/* the current minor tab */
    Widget next_minor;		/* the next minor tab of first_minor */
    Widget prev_minor;		/* the prev minor tab of first_minor */
    int first_minor_page;	/* page# of the first visible minor tab */
    int top_minor_page;		/* page# of the current minor tab */
    int top_minor_idx;		/* top minor tab's index in the child array */
    int start_minor_idx;	/* start minor tab's index in the child array */
    int end_minor_idx;		/* end minor tab's index in the child array */
    int num_minor;		/* # of minor tabs */
    int num_visible_minor;	/* # of visible minor tabs */
    int num_rest_minor;		/* # of next minor tabs of the current page */
    int num_next_minor;		/* # of next minor tabs of first_minor */
    int num_prev_minor;		/* # of prev minor tabs of first_minor */


    /*
     * Calculate general tab information, based on the current_page_number.
     */

    /* Major tab */
    first_major_page = nb->notebook.first_major
		? NotebookConstraint(nb->notebook.first_major)->page_number
		: nb->notebook.first_page_number - 1;
    top_major_page = nb->notebook.first_page_number - 1;
    top_major_idx = -1;
    start_major = top_major = next_major = prev_major = NULL;
    num_major = num_rest_major = num_next_major = num_prev_major = 0;

    for (i=0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
        nc = NotebookConstraint(child);
	if (nc->active && NB_IS_CHILD_MAJOR(nc->child_type))
	    {
	    if (!start_major)
		/* Mark the very first tab */
		start_major = child;

	    if (nc->page_number <= nb->notebook.current_page_number)
		{
		/* Mark the closest tab to the current page */
		top_major = child;
		top_major_page = nc->page_number;
		top_major_idx = i;
		}
	    else
		/* Count of tabs past the current page  */
		num_rest_major++;

	    if (nc->page_number < first_major_page)
		{
		/* Mark the closest previous tab to the first visible tab */
		prev_major = child;
		/* Count of tabs prior to the current page  */
		num_prev_major++;
		}
	    else if (nc->page_number > first_major_page)
		{
		if (!num_next_major)
		    /* Mark the closest next tab to the current page */
		    next_major = child;
		/* Count of tabs after the current page */
		num_next_major++;
		}

	    /* Count of all tabs */
	    num_major++;
	    }
	}

    /* Minor tab */
    first_minor_page = nb->notebook.first_minor
		? NotebookConstraint(nb->notebook.first_minor)->page_number
		: nb->notebook.first_page_number - 1;
    top_minor_page = nb->notebook.first_page_number - 1;
    top_minor_idx = -1;
    start_minor_idx = end_minor_idx = -1;
    start_minor = top_minor = next_minor = prev_minor = NULL;
    num_minor = num_rest_minor = num_next_minor = num_prev_minor = 0;
    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
        nc = NotebookConstraint(child);
	if (nc->active)
	    {
	    /* Break out when next major tab section is reached */
            if (   nc->page_number > top_major_page
		&& NB_IS_CHILD_MAJOR(nc->child_type) )
                break;

	    /* This minor tab is in the current major tab section */
            if (   nc->page_number >= top_major_page
		&& NB_IS_CHILD_MINOR(nc->child_type) )
		{
		if (!num_minor)
		    {
		    /* Mark the first tab in this section */
		    start_minor = child;
		    start_minor_idx = i;
		    }
		if (nc->page_number <= nb->notebook.current_page_number)
		    {
		    /* Mark the closest tab to the current page */
		    top_minor = child;
		    top_minor_page = nc->page_number;
		    top_minor_idx = i;
		    }
		else
		    {
		    /* Count of tabs past the current page */
		    num_rest_minor++;
		    }
		if (nc->page_number < first_minor_page)
		    {
		    /* Mark the closest previous tab to the first visible tab */
		    prev_minor = child;
		    num_prev_minor++;
		    }
		else if (nc->page_number > first_minor_page)
		    {
		    if (!num_next_minor)
			{
			/* Mark the closest next tab to the current page */
			next_minor = child;
			}
		    /* Count of tabs after the current page */
		    num_next_minor++;
		    }

		end_minor_idx = i;
		num_minor++;
		}
	    }
	}


    /*
     * Find the number of possibly visible tabs (num_visible_xxx), and
     * find whether the tab scroller is needed (notebook.need_scroller)
     */
    nb->notebook.need_scroller = XmNONE;

    /* Major tab */
    if (nb->notebook.orientation == XmHORIZONTAL)
	{
	frame_dim 	= nb->notebook.frame_height;
	tab_dim 	= MAX(nb->notebook.major_height, 1); /* Avoid div 0 */
	scroller_dim 	= nb->notebook.major_scroller_height;
	}
    else /* (nb->notebook.orientation == XmVERTICAL) */
	{
	frame_dim 	= nb->notebook.frame_width;
	tab_dim 	= MAX(nb->notebook.major_width, 1); /* Avoid div 0 */
	scroller_dim 	= nb->notebook.major_scroller_width;
	}
    spacing = MAX(nb->notebook.major_spacing, nb->notebook.shadow_thickness);
    num_visible_major = (int)(frame_dim -
			 nb->notebook.back_page_size -
			 nb->notebook.shadow_thickness) /
			 (int)(tab_dim + spacing);
    if (num_visible_major < num_major)
	{
	nb->notebook.need_scroller = XmMAJOR_TAB_SCROLLER;
	num_visible_major = (int)(frame_dim -
			     nb->notebook.back_page_size -
			     scroller_dim -
			     MAX(0, (int)(scroller_dim -
				    nb->notebook.back_page_size)) -
			     nb->notebook.shadow_thickness ) /
			     (int)(tab_dim + spacing);
	}

    /* Minor tab */
    if (nb->notebook.orientation == XmHORIZONTAL)
	{
	frame_dim 	= nb->notebook.frame_width;
	tab_dim 	= MAX(nb->notebook.minor_width, 1); /* Avoid div 0 */
	scroller_dim 	= nb->notebook.minor_scroller_width;
	}
    else /* (nb->notebook.orientation == XmVERTICAL) */
	{
	frame_dim 	= nb->notebook.frame_height;
	tab_dim 	= MAX(nb->notebook.minor_height, 1); /* Avoid div 0 */
	scroller_dim 	= nb->notebook.minor_scroller_height;
	}
    spacing = MAX(nb->notebook.minor_spacing, nb->notebook.shadow_thickness);
    num_visible_minor 	= (int)(frame_dim -
			 MAX(0, (int)(nb->notebook.back_page_size -
				nb->notebook.real_binding_width)) -
			 nb->notebook.shadow_thickness) /
			 (int)(tab_dim + spacing);
    if (num_visible_minor < num_minor)
	{
	if (NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller))
	    nb->notebook.need_scroller = XmTAB_SCROLLER;
	else
	    nb->notebook.need_scroller = XmMINOR_TAB_SCROLLER;
	num_visible_minor = (int)(frame_dim -
			     MAX(0, (int)(nb->notebook.back_page_size -
				    nb->notebook.real_binding_width)) -
			     scroller_dim -
			     MAX(0, (int)(scroller_dim -
				 nb->notebook.back_page_size)) -
			     nb->notebook.shadow_thickness) /
				 (int)(tab_dim + spacing);
	}


    /*
     * If tab scrolling, then determine first tab but leave top tab alone.
     */

    /* Major tab */
    if (NB_IS_CHILD_MAJOR_SCROLLER(reason))
    {
	if (scroll == _NEXT)
	    {
	    /* If there is a next non-visible tab(s), move up one */
	    if (num_next_major >= num_visible_major)
		nb->notebook.first_major = next_major;
	    }
	else if (scroll == _PREVIOUS)
	    {
	    /* If there is a prev non-visible tab(s), move back one */
	    if (num_prev_major > 0)
		nb->notebook.first_major = prev_major;
	    }
	else if (scroll == _HOME)
	    {
	    /* If there is a starting tab, move to it */
	    if (start_major)
		nb->notebook.first_major = start_major;
	    }
	else if (scroll == _END)
	    {
	    /* Back up from the last tab "num_visible_major" tabs */
	    for (count = num_visible_major,i = nb->composite.num_children -1;
		i >= 0 && count > 0; i--)
		{
		child = nb->composite.children[i];
		nc = NotebookConstraint(child);
		if (nc->active && NB_IS_CHILD_MAJOR(nc->child_type))
		    {
			nb->notebook.first_major = child;
			count--;
		    }
		}
	    }
	return;
    }

    /* Minor tab */
    if (NB_IS_CHILD_MINOR_SCROLLER(reason))
	{
	if (scroll == _NEXT)
	    {
	    if (num_next_minor >= num_visible_minor)
		nb->notebook.first_minor = next_minor;
	    }
	else if (scroll == _PREVIOUS)
	    {
	    if (num_prev_minor > 0)
		nb->notebook.first_minor = prev_minor;
	    }
	else if (scroll == _HOME)
	    {
		nb->notebook.first_minor = start_minor;
	    }
	else if (scroll == _END)
	    {
	    for (count = num_visible_minor,i = end_minor_idx;
		i >= 0 && i >= start_minor_idx && count > 0; i--)
		{
		child = nb->composite.children[i];
		nc = NotebookConstraint(child);
		if (nc->active && NB_IS_CHILD_MINOR(nc->child_type))
		    {
			nb->notebook.first_minor = child;
			count--;
		    }
		}
	    }
	    return;
	}


    /*
     * If going to a new page, then determine if we need to scroll
     * the tabs to make the top major/minor tabs visible
     */
    if (NB_IS_CHILD_PAGE(reason)
	|| NB_IS_CHILD_PAGE_SCROLLER(reason)
	|| NB_IS_CHILD_MAJOR(reason)
	|| NB_IS_CHILD_MINOR(reason))
	{
	if (top_major && (top_major != nb->notebook.top_major))
	    {
	    if (NB_IS_VISIBLE(top_major))
		reason = XmMAJOR_TAB;
	    }
	else if (top_minor && (top_minor != nb->notebook.top_minor))
	    {
	    if (NB_IS_VISIBLE(top_minor))
		reason = XmMINOR_TAB;
	    }
	}

    /*
     * Set new top major/minor tabs
     */
    nb->notebook.old_top_major = nb->notebook.top_major;
    nb->notebook.top_major = top_major;

    nb->notebook.old_top_minor = nb->notebook.top_minor;
    nb->notebook.top_minor = top_minor;


    /*
     * Determine new first tab for the new top tab.
     * If apparently activated by a minor tab, we don't need to do anything
     * because the top major and minor must be visible .
     * If apparently activated by a major tab, we don't need to do anything.
     */

    /* A page change NOT caused by a major tab activiation */
    if (!NB_IS_CHILD_MAJOR(reason) && !NB_IS_CHILD_MINOR(reason))
	{
	/* Set the first tab to the starting tab, if we do not need a page
	   scroller or if the calculated top tab page number is less than
	   the first page number */
        if (!NB_IS_CHILD_MAJOR_SCROLLER(nb->notebook.need_scroller)
	    || (top_major_page <= nb->notebook.first_page_number))
	    nb->notebook.first_major = start_major;

	/* Set the first tab to the calculated top tab, if it is the
	   only one that might be visible */
	else if (num_visible_major <= 1)
	    nb->notebook.first_major = top_major;

	/* Set the first tab so that the top tab is near the center of the
	   visible tabs */
	else
	    {
	    half = num_visible_major /2;
	    half += MAX(((num_visible_major - 1) /2) - num_rest_major, 0);
	    nb->notebook.first_major = top_major;
	    for (i = top_major_idx - 1; i >= 0 && half > 0; i--)
		{
		child = nb->composite.children[i];
        	nc = NotebookConstraint(child);
                if (nc->active && NB_IS_CHILD_MAJOR(nc->child_type))
		    {
		    nb->notebook.first_major = child;
		    half--;
		    }
		}
	    }
	}

    /* Minor tab */
    if (!(NB_IS_CHILD_MINOR(reason)))
	{
	/* Set the first tab to the starting tab in this tab section, if we
	   do not need a page scroller or if the calculated top tab
	   page number is less than the first page number */
        if (!NB_IS_CHILD_MINOR_SCROLLER(nb->notebook.need_scroller)
	    || (top_minor_page <= nb->notebook.first_page_number))
	    nb->notebook.first_minor = start_minor;

	/* Set the first tab to the calculated top tab, if it is the
	   only one that might be visible */
	else if (num_visible_minor <= 1)
	    nb->notebook.first_minor = top_minor;

	/* Set the first tab so that the top tab is near the center of the
	   visible tabs */
        else
	    {
	    half = num_visible_minor /2;
	    half += MAX(((num_visible_minor - 1) /2) - num_rest_minor, 0);
	    nb->notebook.first_minor = top_minor;
            for (i = top_minor_idx - 1; i >= 0 && half > 0; i--)
		{
		child = nb->composite.children[i];
                nc = NotebookConstraint(child);
	        if (nc->active)
		    {
                    if (NB_IS_CHILD_MAJOR(nc->child_type))
			break;
                    else if (NB_IS_CHILD_MINOR(nc->child_type))
			{
			nb->notebook.first_minor = child;
			half--;
			}
		    }
		}
	    }
	}
}



/*****************************************************************************
 *                                                                           *
 *			      Drawing Functions 			     *
 *                                                                           *
 *****************************************************************************/


/*- GetFrameGCs -------------------------------------------------------------

        Get the GC's for drawing notebook frame and binding

-----------------------------------------------------------------------------*/
static void
GetFrameGCs (
    XmNotebookWidget nb)
{
    XGCValues values;
    XtGCMask mask, dynamicMask;

    if (nb->notebook.frame_gc)
        XtReleaseGC((Widget)nb, nb->notebook.frame_gc);
    if (nb->notebook.binding_gc)
        XtReleaseGC((Widget)nb, nb->notebook.binding_gc);

    dynamicMask = GCForeground;
    mask = GCForeground | GCBackground | GCLineWidth;
    values.foreground = nb->manager.foreground;
    values.background = nb->notebook.frame_background;
    values.line_width = 1;
    nb->notebook.frame_gc = XtAllocateGC((Widget)nb, 0, mask, &values,
					 dynamicMask, 0);

    mask = GCForeground | GCBackground;
    dynamicMask = (GCFillStyle | GCTile | GCStipple | GCTileStipXOrigin |
		   GCTileStipYOrigin);
    values.foreground = nb->manager.foreground;
    values.background = nb->notebook.frame_background;
    nb->notebook.binding_gc = XtAllocateGC((Widget)nb, 0, mask, &values,
					   dynamicMask, 0);
}


/*- GetBackpageGCs ---------------------------------------------------------

        Get the GC's for drawing notebook backpages. These two may be
	shared with other widgets.

-----------------------------------------------------------------------------*/
static void
GetBackpageGCs (
    XmNotebookWidget nb)
{
    XGCValues values;
    XtGCMask mask, dynamicMask, unusedMask;

    if (nb->notebook.foreground_gc)
        XtReleaseGC((Widget)nb, nb->notebook.foreground_gc);
    if (nb->notebook.background_gc)
        XtReleaseGC((Widget)nb, nb->notebook.background_gc);

    /* Will often be the same as HighlightGC in List */
    mask = GCForeground;
    dynamicMask = (GCLineWidth | GCLineStyle |
		   GCClipMask | GCClipXOrigin | GCClipYOrigin);
    unusedMask = GCBackground | GCDashList;
    values.foreground = nb->notebook.back_page_foreground;
    nb->notebook.foreground_gc = XtAllocateGC((Widget)nb, 0, mask, &values,
					      dynamicMask, unusedMask);

    /* Will often be the same as InverseGC in List */
    mask |= GCGraphicsExposures;
    dynamicMask = GCClipMask | GCClipXOrigin | GCClipYOrigin;
    unusedMask = GCBackground | GCFont;
    values.foreground = nb->notebook.back_page_background;
    values.graphics_exposures = FALSE;
    nb->notebook.background_gc = XtAllocateGC((Widget)nb, 0, mask, &values,
					      dynamicMask, unusedMask);

}


/*- MakeSpiralPixmap -------------------------------------------------------

    Makes a spiral binding pixmap.

    Uses instance state:
	binding placement	(XmNorientation and XmNbackPagePlacement)

    Updates instance state:
	spiral_pixmap

    Parameters:
	IN width   		- binding area width
	IN height  		- binding area height

-----------------------------------------------------------------------------*/
static void
MakeSpiralPixmap (
    XmNotebookWidget nb,
    Dimension width,
    Dimension height)
{
    int rx=0, ry=0, rw=0, rh=0;         /* rectangle values for binding surface */
    int sx=0, sy=0, sw=0, sh=0, sd=0;     /* spiral values */
    int a1=0, a2=0, a3=0, a4=0;           /* spiral angle values */
    int hx=0, hy=0, hd=0;                 /* hole values */
    int lx1=0, ly1=0, lx2=0, ly2=0;       /* line values for binding edge */
    int pw=0, ph=0;                       /* pixmap size */
    int gap=0;                            /* gap between spirals */
    int div;                              /* division of binding width */
    int i;
    Pixmap pixmap;

    /* check binding width for reasonable values */
    if (width < MIN_DRAWABLE_SPIRAL_SIZE || height < MIN_DRAWABLE_SPIRAL_SIZE)
        return;

    /*
     * Determine spiral component values
     */
    if (nb->notebook.binding_pos == LEFT)
        {
        div = width / 3;
        gap = div / 2;

        pw = width;
        ph = div + gap;

        sx = 0;
        sy = gap / 2;
        sw = div * 2;
        sh = div;
        sd = sh / 4;

        a1 =  270 - 20;
        a2 = -270;
        a3 =  90 - 20;
        a4 =  110;

        hx = sx + sw - sd;
        hy = sy + (sh / 2) - sd +1;
        hd = MIN(div, sd * 2);

        rx = div;
        ry = 0;
        rw = width - div;
        rh = ph;

        lx1 = div;
        ly1 = 0;
        lx2 = lx1;
        ly2 = ph;
        }
    else if (nb->notebook.binding_pos == RIGHT)
        {
        div = width / 3;
        gap = div / 2;

        pw = width;
        ph = div + gap;

        sx = div - 1;
        sy = gap / 2;
        sw = div * 2;
        sh = div;
        sd = sh / 4;

        a1 =  275;
        a2 =  270;
        a3 =  90 - 10;
        a4 =  100;

        hx = sx - (sd / 2);
        hy = sy + (sh / 2) - sd +1;
        hd = MIN(div, sd * 2);

        rx = 0;
        ry = 0;
        rw = pw - div;
        rh = ph;

        lx1 = pw - div;
        ly1 = 0;
        lx2 = lx1;
        ly2 = rh;
        }
    else if (nb->notebook.binding_pos == TOP)
        {
        div = height / 3;
        gap = div / 2;

        pw = div + gap;
        ph = height;

        sx = gap / 2;
        sy = 0;
        sw = div;
        sh = div * 2;
        sd = sw / 4;

        a1 =  0 + 15;
        a2 =  270;
        a3 =  90;
        a4 =  110;

        hx = sx + (sw / 2) - sd +1;
        hy = sh - sd;
        hd = MIN(div, sd * 2);

        rx = 0;
        ry = div;
        rw = pw;
        rh = height - div;

        lx1 = 0;
        ly1 = div;
        lx2 = pw;
        ly2 = ly1;
        }
    else if (nb->notebook.binding_pos == BOTTOM)
        {
        div = height / 3;
        gap = div / 2;

        pw = div + gap;
        ph = height;

        sx = gap / 2;
        sy = div -1;
        sw = div;
        sh = (div * 2);
        sd = sw / 4;

        a1 =  360 - 5;
        a2 =  -295;
        a3 =  90 + 8;
        a4 =  110;

        hx = sx + (sw / 2);
        hy = sy;
        hd = MIN(div, sd * 2);

        rx = 0;
        ry = 0;
        rw = pw;
        rh = ph - div;

        lx1 = 0;
        ly1 = ph - div;
        lx2 = pw;
        ly2 = ly1;
        }

    /*
     * Create spiral pixmap, if previous one exists destroy it
     */
    if (nb->notebook.spiral_pixmap != XmUNSPECIFIED_PIXMAP
    &&  nb->notebook.spiral_pixmap != XmNONE)
            XFreePixmap(XtDisplay(nb), nb->notebook.spiral_pixmap);

    pixmap = nb->notebook.spiral_pixmap =
        XCreatePixmap(XtDisplay(nb), XtWindow(nb), pw, ph, nb->core.depth);

    /*
     * Scribble in spiral pixmap
     */
    /* Fill pixmap with notebook background */
    XFillRectangle(XtDisplay(nb), pixmap, nb->manager.background_GC,
                0, 0, pw, ph);

    /* draw binding surface */
    XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
                nb->notebook.frame_background);
    XFillRectangle(XtDisplay(nb), pixmap, nb->notebook.frame_gc,
                rx, ry, rw, rh);

    /* draw line along binding surface */
    XSetClipMask(XtDisplay(nb), nb->notebook.foreground_gc, None);
    XSetLineAttributes(XtDisplay(nb), nb->notebook.foreground_gc, 1,
                LineSolid, CapRound, JoinMiter);
    XDrawLine(XtDisplay(nb), pixmap, nb->notebook.foreground_gc,
                lx1, ly1, lx2, ly2);

    /* draw hole in binding surface with top/bottom shadows */
    XFillArc(XtDisplay(nb), pixmap, nb->manager.background_GC,
                hx, hy, hd, hd, 0, 360 * 64);
    XDrawArc(XtDisplay(nb), pixmap, nb->manager.bottom_shadow_GC,
                hx, hy, hd, hd, 225 * 64, -180 * 64);
    XDrawArc(XtDisplay(nb), pixmap, nb->manager.top_shadow_GC,
                hx, hy, hd, hd,  45 * 64, -180 * 64);

    /* draw spiral with top/bottom shadows */
    XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
                nb->manager.foreground);
    XSetLineAttributes(XtDisplay(nb), nb->notebook.frame_gc, 1,
                LineSolid, CapRound, JoinMiter);
    for(i=1; i < sd; i++)
        XDrawArc(XtDisplay(nb), pixmap, nb->notebook.frame_gc,
                sx +i, sy +i, sw -i, sh -i, a1 * 64, a2 * 64);
    XSetLineAttributes(XtDisplay(nb), nb->notebook.frame_gc, MAX(0,sd -2),
                LineSolid, CapRound, JoinMiter);
    XDrawArc(XtDisplay(nb), pixmap, nb->notebook.frame_gc,
                sx +(sd/2), sy +(sd/2), sw, sh, a3 * 64, a4 * 64);
    XDrawArc(XtDisplay(nb), pixmap, nb->manager.top_shadow_GC,
                sx, sy, sw, sh, a1 * 64, a2 * 64);
    XDrawArc(XtDisplay(nb), pixmap, nb->manager.bottom_shadow_GC,
                sx +sd, sy +sd, sw -sd, sh -sd, a1 * 64, a2 * 64);
}




/*- DrawBinding -------------------------------------------------------------

        Draw the Notebook binding

-----------------------------------------------------------------------------*/
static void
DrawBinding(
    XmNotebookWidget nb,
    XExposeEvent *event,
    Region region)
{
    Dimension x, y, width, height;

    /* check binding type resource and binding width */
    (void)event;
    if (nb->notebook.binding_type == XmNONE
	|| nb->notebook.real_binding_width <= 0)
	return;

    /* get the binding area */
    x = y = 0;
    if (nb->notebook.major_pos == LEFT)
        x += NB_MAJOR_MAX(nb, nb->notebook.major_width,
	    	nb->notebook.major_scroller_width) +
		nb->notebook.back_page_size + nb->notebook.frame_width;
    else if (nb->notebook.major_pos == TOP)
        y += NB_MAJOR_MAX(nb, nb->notebook.major_height,
		nb->notebook.major_scroller_height) +
		nb->notebook.back_page_size + nb->notebook.frame_height;
    if (nb->notebook.minor_pos == LEFT)
        x += NB_MINOR_MAX(nb, nb->notebook.minor_width,
		nb->notebook.minor_scroller_width) +
		nb->notebook.back_page_size;
    else if (nb->notebook.minor_pos == TOP)
        y += NB_MINOR_MAX(nb, nb->notebook.minor_height,
		nb->notebook.minor_scroller_height) +
		nb->notebook.back_page_size;
    if (nb->notebook.orientation == XmHORIZONTAL)
	{
	width = nb->notebook.real_binding_width;
	height = nb->notebook.frame_height - 1;
	y += 1;
	}
    else
	{
	width = nb->notebook.frame_width - 1;
	height = nb->notebook.real_binding_width;
	x += 1;
	}

    /* draw the binding if applicable */
    if (XRectInRegion(region, x, y, width, height))
	{
	switch (nb->notebook.binding_type)
	    {
	    case XmSOLID:
	        XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
			       nb->manager.foreground);
		XFillRectangle(XtDisplay(nb), XtWindow(nb),
			       nb->notebook.frame_gc, x, y, width, height);
	        break;
	    case XmSPIRAL:
                MakeSpiralPixmap (nb, width, height);
	        DrawPixmapBinding(nb, x, y, width, height,
				    nb->notebook.spiral_pixmap);
	        break;
	    case XmPIXMAP:
	        DrawPixmapBinding(nb, x, y, width, height,
				    nb->notebook.binding_pixmap);
	        break;
	    case XmPIXMAP_OVERLAP_ONLY:
	        DrawPixmapBinding(nb, x, y, width, height,
				    nb->notebook.binding_pixmap);
	        break;
	    } /* switch */
	} /* if */
}



/*- DrawPixmapBinding -------------------------------------------------------

        Draw the pixmap binding

-----------------------------------------------------------------------------*/
static void
DrawPixmapBinding (
    XmNotebookWidget nb,
    Dimension x,
    Dimension y,
    Dimension width,
    Dimension height,
    Pixmap pixmap)
{
    int depth;
    int x_origin, y_origin;
    XGCValues values;
    XtGCMask mask;

    /* no pixmap? don't draw */
    if (pixmap == XmUNSPECIFIED_PIXMAP || pixmap == XmNONE)
	return;

    /* get Pixmap depth */
    XmeGetPixmapData(XtScreen(nb), pixmap,
			NULL, &depth, NULL, NULL, NULL, NULL, NULL, NULL);

    /* creating the gc */
    if (depth == 1)
	{
	mask = GCFillStyle | GCStipple;
	values.fill_style = FillStippled;
	values.stipple = pixmap;
	}
    else
	{
	mask = GCFillStyle | GCTile;
	values.fill_style = FillTiled;
	values.tile = pixmap;
	}
    XChangeGC(XtDisplay(nb), nb->notebook.binding_gc, mask, &values);

    /*
     * set TSOrigin
     */
    if (nb->notebook.binding_pos == LEFT || nb->notebook.binding_pos == TOP)
        x_origin = x, y_origin = y;
    else if (nb->notebook.binding_pos == RIGHT)
        x_origin = x + width - 1, y_origin = y;
    else /* if (nb->notebook.binding_pos == BOTTOM) */
        x_origin = x, y_origin = y + height;

    XSetTSOrigin(XtDisplay(nb), nb->notebook.binding_gc, x_origin, y_origin);

    /* display the pixmap binding */
    XFillRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.binding_gc,
		    x, y, width -1, height - 1);
}


/*- DrawFrameShadow ---------------------------------------------------------

        Draw the Notebook frame shadow

-----------------------------------------------------------------------------*/
static void
DrawFrameShadow(
    XmNotebookWidget nb,
    XExposeEvent *event,
    Region region)
{
    Dimension x, y, width, height;
    Region shadow_region;
    XRectangle rect;

    /* get the page's x, y position */
    (void)event;
    (void)region;

    x = y = 1;
    if (nb->notebook.binding_pos == LEFT)
        x += nb->notebook.real_binding_width;
    else if (nb->notebook.binding_pos == TOP)
        y += nb->notebook.real_binding_width;
    if (nb->notebook.major_pos == LEFT)
        x += NB_MAJOR_MAX(nb, nb->notebook.major_width,
            nb->notebook.major_scroller_width) + nb->notebook.back_page_size;
    else if (nb->notebook.major_pos == TOP)
        y += NB_MAJOR_MAX(nb, nb->notebook.major_height,
            nb->notebook.major_scroller_height) + nb->notebook.back_page_size;
    if (nb->notebook.minor_pos == LEFT)
        x += NB_MINOR_MAX(nb, nb->notebook.minor_width,
            nb->notebook.minor_scroller_width) + nb->notebook.back_page_size;
    else if (nb->notebook.minor_pos == TOP)
        y += NB_MINOR_MAX(nb, nb->notebook.minor_height,
            nb->notebook.minor_scroller_height) + nb->notebook.back_page_size;
    width = nb->notebook.frame_width - 2;
    height = nb->notebook.frame_height - 2;

    XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
		   nb->notebook.frame_background);

    /* draw the frame shadow */
    if (nb->notebook.shadow_thickness)
    {
	/* draw the shadow */
	/* creating the shadow region */
	shadow_region = XCreateRegion();

	/* adding the frame area to the shadow region */
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	XFillRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.frame_gc,
			x, y, width + 1, height + 1);
	XUnionRectWithRegion(&rect, shadow_region, shadow_region);

	/* adding the top major tab area to the shadow region */
	if (nb->notebook.top_major
	    && NB_IS_VISIBLE(nb->notebook.top_major)
	    && !NB_IS_CHILD_JOINSIDE(nb->notebook.top_major))
	{
	    rect.x = nb->notebook.top_major->core.x -
		     nb->notebook.shadow_thickness;
	    rect.y = nb->notebook.top_major->core.y -
		     nb->notebook.shadow_thickness;
	    rect.width = nb->notebook.top_major->core.width +
		     nb->notebook.shadow_thickness * 2 - 1;
	    rect.height = nb->notebook.top_major->core.height +
		     nb->notebook.shadow_thickness * 2 - 1;
	    /* ensure x/y of rect does not go negative */
	    /* this can be removed when _XmRegionDrawShadow is fixed
		to allow negative y values */
            if( rect.x < 0)
                {
                rect.width -= rect.x;
                rect.x = 0;
                }
            if( rect.y < 0)
                {
                rect.height -= rect.y;
                rect.y = 0;
                }

	    XUnionRectWithRegion(&rect, shadow_region, shadow_region);
	}

	/* adding the top minor tab area to the shadow region */
	if (nb->notebook.top_minor
	    && NB_IS_VISIBLE(nb->notebook.top_minor)
	    && !NB_IS_CHILD_JOINSIDE(nb->notebook.top_minor))
	{
	    rect.x = nb->notebook.top_minor->core.x -
		     nb->notebook.shadow_thickness;
	    rect.y = nb->notebook.top_minor->core.y -
		     nb->notebook.shadow_thickness;
	    rect.width = nb->notebook.top_minor->core.width +
		     nb->notebook.shadow_thickness * 2 - 1;
	    rect.height = nb->notebook.top_minor->core.height +
		     nb->notebook.shadow_thickness * 2 - 1;
	    /* ensure x/y of rect does not go negative */
	    /* this can be removed when _XmRegionDrawShadow is fixed
		to allow negative y values */
            if( rect.x < 0)
                {
                rect.width -= rect.x;
                rect.x = 0;
                }
            if( rect.y < 0)
                {
                rect.height -= rect.y;
                rect.y = 0;
                }
	    XUnionRectWithRegion(&rect, shadow_region, shadow_region);
	}

	/* draw the shadow */
	_XmRegionDrawShadow(XtDisplay(nb), XtWindow(nb),
			    nb->manager.top_shadow_GC,
			    nb->manager.bottom_shadow_GC,
			    (XmRegion)shadow_region, nb->core.border_width,
			    nb->notebook.shadow_thickness, XmSHADOW_OUT);
	XDestroyRegion(shadow_region);
    }
    else
    {
	XFillRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.frame_gc,
			x, y, width, height);
	XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
		       nb->manager.foreground);
	XDrawRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.frame_gc,
			x, y, width, height);
	if (nb->notebook.top_major
	    && NB_IS_VISIBLE(nb->notebook.top_major)
	    && !NB_IS_CHILD_JOINSIDE(nb->notebook.top_major))
	{
	    rect.x = nb->notebook.top_major->core.x - 1;
	    rect.y = nb->notebook.top_major->core.y - 1;
	    rect.width = nb->notebook.top_major->core.width + 1;
	    rect.height = nb->notebook.top_major->core.height + 1;
	    /* ensure x/y of rect does not go negative */
	    /* this can be removed when _XmRegionDrawShadow is fixed
		to allow negative y values */
            if( rect.x < 0)
                {
                rect.width -= rect.x;
                rect.x = 0;
                }
            if( rect.y < 0)
                {
                rect.height -= rect.y;
                rect.y = 0;
                }

	XDrawRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.frame_gc,
			rect.x, rect.y, rect.width, rect.height);
	}

	/* adding the top minor tab area to the shadow region */
	if (nb->notebook.top_minor
	    && NB_IS_VISIBLE(nb->notebook.top_minor)
	    && !NB_IS_CHILD_JOINSIDE(nb->notebook.top_minor))
	{
	    rect.x = nb->notebook.top_minor->core.x - 1;
	    rect.y = nb->notebook.top_minor->core.y - 1;
	    rect.width = nb->notebook.top_minor->core.width + 1;
	    rect.height = nb->notebook.top_minor->core.height + 1;
	    /* ensure x/y of rect does not go negative */
	    /* this can be removed when _XmRegionDrawShadow is fixed
		to allow negative y values */
            if( rect.x < 0)
                {
                rect.width -= rect.x;
                rect.x = 0;
                }
            if( rect.y < 0)
                {
                rect.height -= rect.y;
                rect.y = 0;
                }
	XDrawRectangle(XtDisplay(nb), XtWindow(nb), nb->notebook.frame_gc,
			rect.x, rect.y, rect.width, rect.height);
	}
    }

}


/*- DrawBackPages -----------------------------------------------------------

	Draw Notebook back pages

-----------------------------------------------------------------------------*/
static void
DrawBackPages (
    XmNotebookWidget nb,
    XExposeEvent *event,
    Region region)
{
    int delta, back;	/* backpage foreground & background deltas */
    int binding_width;	/* the real binding width */
    int dx, dy;		/* delta-x and delta-y */
    int mx, my;		/* maximum delta-x and delta-y */
    int x, y, limit;	/* temporary variables */
    XPoint p[5];	/* foreground line pattern */
    XPoint q[3];	/* background line pattern */

    /*
     * initialize
     */
    (void)event;
    delta = nb->notebook.back_page_size / nb->notebook.real_back_page_number;

    /* draw backpages on the frame, which is not necessarily along the
       entire binding */
    if (nb->notebook.binding_type == XmPIXMAP)
	binding_width = MAX(nb->notebook.real_binding_width,
			nb->notebook.binding_width);
    else
	binding_width = MIN(nb->notebook.real_binding_width,
			nb->notebook.binding_width);

    /* find back page line pattern */
    if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
	nb->notebook.orientation == XmHORIZONTAL)
    {
	p[1].x = nb->notebook.real_binding_width - binding_width;
	p[1].y = nb->notebook.frame_height;
	p[2].x = binding_width + nb->notebook.frame_width;
	p[2].y = 0;
	p[3].x = 0;
	p[3].y = - nb->notebook.frame_height;
	p[4].x = - delta;
	p[4].y = 0;
	p[0].x = p[1].x;
	p[0].y = p[1].y - delta;
	x = 0;
	y = delta;
	dx = dy = delta;
	mx = my = nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_RIGHT &&
	nb->notebook.orientation == XmVERTICAL)
    {
	p[1].x = nb->notebook.frame_width;
	p[1].y = nb->notebook.real_binding_width - binding_width;
	p[2].x = 0;
	p[2].y = binding_width + nb->notebook.frame_height;
	p[3].x = - nb->notebook.frame_width;
	p[3].y = 0;
	p[4].x = 0;
	p[4].y = - delta;
	p[0].x = p[1].x - delta;
	p[0].y = p[1].y;
	x = delta;
	y = 0;
	dx = dy = delta;
	mx = my = nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
	nb->notebook.orientation == XmHORIZONTAL)
    {
	p[1].x = NB_MAJOR_MAX(nb, nb->notebook.major_width,
		nb->notebook.major_scroller_width) +
		nb->notebook.back_page_size + nb->notebook.frame_width +
		binding_width - 1;
	p[1].y = nb->notebook.frame_height;
	p[2].x = - (nb->notebook.frame_width + binding_width);
	p[2].y = 0;
	p[3].x = 0;
	p[3].y = - nb->notebook.frame_height;
	p[4].x = delta;
	p[4].y = 0;
	p[0].x = p[1].x;
	p[0].y = p[1].y - delta;
	x = 0;
	y = delta;
	dx = - delta;
	dy = delta;
	mx = - nb->notebook.back_page_size;
	my = nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmBOTTOM_LEFT &&
	nb->notebook.orientation == XmVERTICAL)
    {
	p[1].x = nb->notebook.back_page_size +
		NB_MINOR_MAX(nb, nb->notebook.minor_width,
		nb->notebook.minor_scroller_width) - 1;
	p[1].y = nb->notebook.real_binding_width - binding_width;
	p[2].x = 0;
	p[2].y = binding_width + nb->notebook.frame_height;
	p[3].x = nb->notebook.frame_width;
	p[3].y = 0;
	p[4].x = 0;
	p[4].y = - delta;
	p[0].x = p[1].x + delta;
	p[0].y = p[1].y;
	x = - delta;
	y = 0;
	dx = - delta;
	dy = delta;
	mx = - nb->notebook.back_page_size;
	my = nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
	nb->notebook.orientation == XmHORIZONTAL)
    {
	p[1].x = nb->notebook.real_binding_width - binding_width;
	p[1].y = nb->notebook.back_page_size +
		NB_MINOR_MAX(nb, nb->notebook.minor_height,
		nb->notebook.minor_scroller_height) - 1;
	p[2].x = binding_width + nb->notebook.frame_width;
	p[2].y = 0;
	p[3].x = 0;
	p[3].y = nb->notebook.frame_height;
	p[4].x = - delta;
	p[4].y = 0;
	p[0].x = p[1].x;
	p[0].y = p[1].y + delta;
	x = 0;
	y = - delta;
	dx = delta;
	dy = - delta;
	mx = nb->notebook.back_page_size;
	my = - nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmTOP_RIGHT &&
	nb->notebook.orientation == XmVERTICAL)
    {
	p[1].x = nb->notebook.frame_width;
	p[1].y = nb->notebook.back_page_size + nb->notebook.frame_height +
		binding_width + NB_MAJOR_MAX(nb, nb->notebook.major_height,
		nb->notebook.major_scroller_height) - 1;
	p[2].x = 0;
	p[2].y = - (binding_width + nb->notebook.frame_height);
	p[3].x = - nb->notebook.frame_width;
	p[3].y = 0;
	p[4].x = 0;
	p[4].y = delta;
	p[0].x = p[1].x - delta;
	p[0].y = p[1].y;
	x = delta;
	y = 0;
	dx = delta;
	dy = - delta;
	mx = nb->notebook.back_page_size;
	my = - nb->notebook.back_page_size;
    }
    else if (nb->notebook.back_page_pos == XmTOP_LEFT &&
	nb->notebook.orientation == XmHORIZONTAL)
    {
	p[1].x = nb->notebook.back_page_size + nb->notebook.frame_width +
		binding_width + NB_MAJOR_MAX(nb, nb->notebook.major_width,
		nb->notebook.major_scroller_width) - 1;
	p[1].y = nb->notebook.back_page_size +
		NB_MINOR_MAX(nb, nb->notebook.minor_height,
		nb->notebook.minor_scroller_height) - 1;
	p[2].x = - (binding_width + nb->notebook.frame_width);
	p[2].y = 0;
	p[3].x = 0;
	p[3].y = nb->notebook.frame_height;
	p[4].x = delta;
	p[4].y = 0;
	p[0].x = p[1].x;
	p[0].y = p[1].y + delta;
	x = 0;
	y = - delta;
	dx = - delta;
	dy = - delta;
	mx = - nb->notebook.back_page_size;
	my = - nb->notebook.back_page_size;
    }
    else
    {
	p[1].x = nb->notebook.back_page_size +
		NB_MINOR_MAX(nb, nb->notebook.minor_width,
		nb->notebook.minor_scroller_width) - 1;
	p[1].y = nb->notebook.back_page_size + nb->notebook.frame_height +
		binding_width + NB_MAJOR_MAX(nb, nb->notebook.major_height,
		nb->notebook.major_scroller_height) -1;
	p[2].x = 0;
	p[2].y = - (binding_width + nb->notebook.frame_height);
	p[3].x = nb->notebook.frame_width;
	p[3].y = 0;
	p[4].x = 0;
	p[4].y = delta;
	p[0].x = p[1].x + delta;
	p[0].y = p[1].y;
	x = - delta;
	y = 0;
	dx = - delta;
	dy = - delta;
	mx = - nb->notebook.back_page_size;
	my = - nb->notebook.back_page_size;
    }

    /* set the clipping region if necessary */
    XSetRegion(XtDisplay(nb), nb->notebook.foreground_gc, region);
    XSetRegion(XtDisplay(nb), nb->notebook.background_gc, region);
    XSetClipOrigin(XtDisplay(nb), nb->notebook.foreground_gc, 0, 0);
    XSetClipOrigin(XtDisplay(nb), nb->notebook.background_gc, 0, 0);

    /* draw the first page outline */
    XSetLineAttributes(XtDisplay(nb), nb->notebook.foreground_gc, 1,
			LineSolid, CapButt, JoinMiter);
    if (nb->notebook.binding_type == XmSPIRAL ||
	nb->notebook.binding_type == XmPIXMAP ||
	nb->notebook.binding_type == XmSOLID ||
	nb->notebook.binding_type == XmNONE ||
	nb->notebook.binding_type == XmPIXMAP_OVERLAP_ONLY)
    {
	Dimension width, height;
	/* We can't pass a negative width or height to the draw call.
	   Since it appears that draw functions later in the code need
	   the p values set as above, let's check whether the height and
	   width here are negative and adjust the draw coordinates
	   as appropriate. */

	if (nb->notebook.orientation == XmHORIZONTAL)
	{
	    ADJUST_NEGATIVE_DIMENSION(p[1].x, p[2].x, x, width);
	    ADJUST_NEGATIVE_DIMENSION(p[1].y, p[3].y, y, height);
	    XDrawRectangle(XtDisplay(nb), XtWindow(nb),
			   nb->notebook.foreground_gc,
			   x, y, width, height);
	}
	else
	{
	    ADJUST_NEGATIVE_DIMENSION(p[1].x, p[3].x, x, width);
	    ADJUST_NEGATIVE_DIMENSION(p[1].y, p[2].y, y, height);
	    XDrawRectangle(XtDisplay(nb), XtWindow(nb),
			   nb->notebook.foreground_gc,
			   x, y, width, height);
	}
    }

    /* for XmSOLID, XmPIXMAP, and XmPIXMAP_OVERLAP_ONLY */
    if (nb->notebook.binding_type == XmSOLID ||
	nb->notebook.binding_type == XmPIXMAP ||
	nb->notebook.binding_type == XmPIXMAP_OVERLAP_ONLY)
    {
	/* draw back page lines */
	x = p[1].x;
	y = p[1].y;
	limit = nb->notebook.back_page_size;
	while (limit > 0)
	{
	    /* draw backpage foreground */
	    XDrawLines(XtDisplay(nb), XtWindow(nb), nb->notebook.foreground_gc,
			&(p[1]), 4, CoordModePrevious);

	    /* draw backpage background */
	    if (delta >= 2 && limit > delta - 2)
	    {
		back = delta - 1;
		q[0].x = p[1].x;
		q[0].y = p[1].y;
		q[1].x = p[2].x;
		q[1].y = p[2].y;
		q[2].x = p[3].x;
		q[2].y = p[3].y;
		q[0].x += NB_ENFORCE_SIGN(dx, 1);
		q[0].y += NB_ENFORCE_SIGN(dy, 1);
		if (q[2].x) q[2].x += dx;
		if (q[2].y) q[2].y += dy;
		while (back > 0)
		{
		    XDrawLines(XtDisplay(nb), XtWindow(nb),
				nb->notebook.background_gc,
				q, 3, CoordModePrevious);
		    q[0].y += NB_ENFORCE_SIGN(dy, 1);
		    q[0].x += NB_ENFORCE_SIGN(dx, 1);
		    if (q[2].x) q[2].x -= NB_ENFORCE_SIGN(dx, 1);
		    if (q[2].y) q[2].y -= NB_ENFORCE_SIGN(dy, 1);
		    back--;
		}
	    }
	    p[1].x += dx;
	    p[1].y += dy;
	    limit -= delta;
	}

	/* draw the last page line */
	p[0].x = x;
	p[0].y = y;
	p[1].x = mx;
	p[1].y = my;
	XSetLineAttributes(XtDisplay(nb), nb->notebook.foreground_gc, 2,
			LineSolid, CapButt, JoinMiter);
	XDrawLines(XtDisplay(nb), XtWindow(nb), nb->notebook.foreground_gc,
			p, 5, CoordModePrevious);
    }

    /* for XmNONE and XmSPIRAL */
    else
    {
	/* draw back page lines */
	p[1].x = x;
	p[1].y = y;
	x = p[0].x;
	y = p[0].y;
	limit = nb->notebook.back_page_size;
	while (limit > 0)
	{
	    /* draw backpage foreground */
	    XDrawLines(XtDisplay(nb), XtWindow(nb), nb->notebook.foreground_gc,
			p, 5, CoordModePrevious);

	    /* draw backpage background */
	    if (delta >= 2 && limit > delta - 2)
	    {
		back = delta - 1;
		q[0].x = p[0].x + p[1].x;
		q[0].y = p[0].y + p[1].y;
		q[1].x = p[2].x;
		q[1].y = p[2].y;
		q[2].x = p[3].x;
		q[2].y = p[3].y;
		q[0].x += NB_ENFORCE_SIGN(dx, 1);
		q[0].y += NB_ENFORCE_SIGN(dy, 1);
		if (q[1].x)
		    {
		    q[0].x += dx;
		    q[1].x -= dx;
		    }
		if (q[1].y)
		    {
		    q[0].y += dy;
		    q[1].y -= dy;
		    }
		if (q[2].x) q[2].x += dx;
		if (q[2].y) q[2].y += dy;
		while (back > 0)
		{
		    XDrawLines(XtDisplay(nb), XtWindow(nb),
				nb->notebook.background_gc,
				q, 3, CoordModePrevious);
		    if (q[1].x)
			{
			q[0].y += NB_ENFORCE_SIGN(dy, 1);
			q[1].x += NB_ENFORCE_SIGN(dx, 1);
			}
		    if (q[1].y)
			{
			q[0].x += NB_ENFORCE_SIGN(dx, 1);
			q[1].y += NB_ENFORCE_SIGN(dy, 1);
			}
		    if (q[2].x) q[2].x -= NB_ENFORCE_SIGN(dx, 1);
		    if (q[2].y) q[2].y -= NB_ENFORCE_SIGN(dy, 1);
		    back--;
		}
	    }
	    p[0].x += dx;
	    p[0].y += dy;
	    limit -= delta;
	}

	/* draw the last page line */
	p[0].x = x + mx;
	p[0].y = y + my;
	XSetLineAttributes(XtDisplay(nb), nb->notebook.foreground_gc, 2,
			LineSolid, CapButt, JoinMiter);
	XDrawLines(XtDisplay(nb), XtWindow(nb), nb->notebook.foreground_gc,
			p, 5, CoordModePrevious);
    }

}



/*****************************************************************************
 *                                                                           *
 *			      Geometry Functions			     *
 *                                                                           *
 *****************************************************************************/


/*- CalcGeoInfo ------------------------------------------------------------

    Calculate Notebook's preferred geometry.

    Updates instance state:
	page_width
	page_height
	status_width
	status_height
	major_width
	major_height
	minor_width
	minor_height
	scroller_width
	scroller_height
	major_scroller_width
	major_scroller_height
	minor_scroller_width
	minor_scroller_height
	frame_width
	frame_height
	real_binding_width
	real_back_page_number,	if adjust is True

    Parameters:
	IN instigator  		- child widget requesting geo request
	IN desired     		- instigator's requested geometry
	IN adjust		- flag to indicate whether instance geometry
			    	  variables used for layout should be set.
	OUT preferred_width,
	OUT preferred_height	- notebook w/h based on children sizes

-----------------------------------------------------------------------------*/
static void
CalcGeoInfo (
    XmNotebookWidget nb,
    Widget instigator,
    XtWidgetGeometry *desired,
    Dimension *preferred_width,
    Dimension *preferred_height,
    Boolean adjust)
{
    XmNotebookConstraint nc;
    Dimension width, height;
    Widget child;
    XtWidgetGeometry preferred;
    int i;
    unsigned int w, h;
    Dimension	page_width = 0,page_height = 0;
    Dimension	status_width = 0,status_height = 0;
    Dimension	major_width = 0,major_height = 0;
    Dimension	minor_width = 0,minor_height = 0;
    Dimension	scroller_width = 0,scroller_height = 0;
    Dimension	major_scroller_width = 0,major_scroller_height = 0;
    Dimension	minor_scroller_width = 0,minor_scroller_height = 0;
    Dimension	frame_width,frame_height;
    Dimension	real_binding_width,real_back_page_number;

    /* get width and height of children */
    for (i = 0; i < nb->composite.num_children; i++)
    {
	/* ask preferred size */
	child = nb->composite.children[i];

	if (child == instigator)
	    {
	    width = IsWidth(desired)
				? desired->width
				: XtWidth(instigator);
	    width += (IsBorder(desired)
				? desired->border_width
				: XtBorderWidth(instigator)) * 2;
	    height = IsHeight(desired)
				? desired->height
				: XtHeight(instigator);
	    height += (IsBorder(desired)
				? desired->border_width
				: XtBorderWidth(instigator)) * 2;
	    }
	else
	    {
	    XtQueryGeometry(child, NULL, &preferred);
	    width = (preferred.request_mode & CWWidth)
				? preferred.width
				: XtWidth(child) + child->core.border_width*2;
	    height = (preferred.request_mode & CWHeight)
				? preferred.height
				: XtHeight(child) + child->core.border_width*2;
	    }

	/* get the maximum */
	if (XtIsManaged(child))
	{
	    nc = NotebookConstraint(child);
	    switch (nc->child_type)
	    {
		case XmPAGE:
		    ASSIGN_MAX(page_width, width);
		    ASSIGN_MAX(page_height, height);
		    break;
		case XmSTATUS_AREA:
		    ASSIGN_MAX(status_width, width);
		    ASSIGN_MAX(status_height, height);
		    break;
		case XmMAJOR_TAB:
		    ASSIGN_MAX(major_width, width);
		    ASSIGN_MAX(major_height, height);
		    break;
		case XmMINOR_TAB:
		    ASSIGN_MAX(minor_width, width);
		    ASSIGN_MAX(minor_height, height);
		    break;
		case XmPAGE_SCROLLER:
		    ASSIGN_MAX(scroller_width, width);
		    ASSIGN_MAX(scroller_height, height);
		    break;
		case XmMAJOR_TAB_SCROLLER:
		    ASSIGN_MAX(major_scroller_width, width);
		    ASSIGN_MAX(major_scroller_height, height);
		    break;
		case XmMINOR_TAB_SCROLLER:
		    ASSIGN_MAX(minor_scroller_width, width);
		    ASSIGN_MAX(minor_scroller_height, height);
		    break;
	    }
	}
    }

    /* adjust page's width */
    page_width = MAX((int)page_width, (int)(status_width + nb->notebook.margin_width +
					scroller_width));

    /* set the real binding width */
    real_binding_width = nb->notebook.binding_width;
    switch (nb->notebook.binding_type)
    {
	case XmNONE:
	    real_binding_width = 0;
	    break;
	case XmSPIRAL:
	    real_binding_width = ((int)(nb->notebook.binding_width * 3))/2;
	    break;
	case XmPIXMAP:
	    if (nb->notebook.binding_pixmap != XmUNSPECIFIED_PIXMAP
		&& nb->notebook.binding_pixmap != XmNONE)
	    {
		w = h = 0;
    	        XmeGetPixmapData(XtScreen(nb),nb->notebook.binding_pixmap,
           		             NULL,NULL,NULL,NULL,NULL,NULL,&w,&h);
		if (nb->notebook.orientation == XmHORIZONTAL)
		    {
		    ASSIGN_MAX(real_binding_width, w);
		    }
		else
		    {
		    ASSIGN_MAX(real_binding_width, h);
		    }
	    }
	    break;
	/* Note: in case of XmPIXMAP_OVERLAP_ONLY binding_width is used
	   so we don't care about the Pixmap size here */
    }

    /* Calculate the real back page number */
    real_back_page_number = nb->notebook.back_page_number;
    ASSIGN_MIN(real_back_page_number, (Dimension)(nb->notebook.back_page_size /2));
    ASSIGN_MAX(real_back_page_number, 1);

    /* Calculate status w/h based on page,margin,& scroller w/h */
    status_width = MAX(0,(int)
		(page_width - nb->notebook.margin_width - scroller_width));
    status_height = scroller_height = MAX(status_height,scroller_height);

    /* Calculate the notebook frame size based on page size,shadows,& margins*/
    frame_width = page_width +
		  (nb->notebook.shadow_thickness * 2) +
                  (nb->notebook.margin_width * 2) + 1;
    frame_height = page_height + status_height +
                  (nb->notebook.shadow_thickness * 2) +
                  (nb->notebook.margin_height * 3) + 1;

    /*
     * Return calculated notebook's preferred size
     */
    /* add frame and back pages area */
    *preferred_width = frame_width + nb->notebook.back_page_size;
    *preferred_height = frame_height + nb->notebook.back_page_size;

    /* add binding and tab area */
    if (nb->notebook.orientation == XmHORIZONTAL)
	{
	*preferred_width += real_binding_width +
			NB_MAJOR_MAX(nb, major_width, major_scroller_width);
	*preferred_height +=
			NB_MINOR_MAX(nb, minor_height, minor_scroller_height);
	}
    else
	{
	*preferred_width +=
			NB_MINOR_MAX(nb, minor_width, minor_scroller_width);
	*preferred_height += real_binding_width +
			NB_MAJOR_MAX(nb, major_height, major_scroller_height);
	}


    /*
     * Set notebook geometry, if adjusting
     */
    if (adjust)
    {
	nb->notebook.page_width = page_width;
	nb->notebook.page_height = page_height;
	nb->notebook.status_width = status_width;
	nb->notebook.status_height = status_height;
	nb->notebook.major_width = major_width;
	nb->notebook.major_height = major_height;
	nb->notebook.minor_width = minor_width;
	nb->notebook.minor_height = minor_height;
	nb->notebook.scroller_width = scroller_width;
	nb->notebook.scroller_height = scroller_height;
	nb->notebook.major_scroller_width = major_scroller_width;
	nb->notebook.major_scroller_height = major_scroller_height;
	nb->notebook.minor_scroller_width = minor_scroller_width;
	nb->notebook.minor_scroller_height = minor_scroller_height;
	nb->notebook.frame_width = frame_width;
	nb->notebook.frame_height = frame_height;
	nb->notebook.real_binding_width = real_binding_width;
	nb->notebook.real_back_page_number = real_back_page_number;
    }
}


/*- NewPreferredGeometry ----------------------------------------------------

    Get Notebook's preferred geometry

    Parameters:
	IN instigator		- child widget requesting geo request
	IN desired		- instigator's requested geometry
	OUT preferred		- w/h based on child's preferred sizes

-----------------------------------------------------------------------------*/
static Boolean
NewPreferredGeometry (
    XmNotebookWidget nb,
    Widget instigator,
    XtWidgetGeometry *desired,
    XtWidgetGeometry *preferred)
{
    Dimension preferred_width, preferred_height;

    /* calculate preferred geometry information */
    CalcGeoInfo(nb, instigator, desired,
	    &preferred_width, &preferred_height, FALSE);

    preferred->width = preferred_width;
    preferred->height = preferred_height;
    preferred->request_mode = CWHeight | CWWidth;

    return(!((preferred->width == nb->core.width) &&
	     (preferred->height == nb->core.height)));
}


/*- AdjustGeometry ----------------------------------------------------------

        Adjust notebook's children sizes based on the Notebook's width
	and height.

	The minimum width needed is determined by the following:
	   1. page and status are minimized until they reach XmNbackPageWidth
	   2. binding is minimized
	   3. major tabs are minimized
	   4. page scroller is minimized
	   5. back pages and frame are clipped

	The minimum height needed is determined by the following:
	   1. page is minimized
	   2. status and page scroller are minimized
	   3. minor tabs are minimized
	   4. back pages and frame are clipped

-----------------------------------------------------------------------------*/
static void
AdjustGeometry (
    XmNotebookWidget nb,
    Widget instigator,
    XtWidgetGeometry *desired)
{
    Dimension ideal_width,ideal_height;

    /* the value min is the minimum width/height that the Notebook can be
       after a component is reduced to its minimum */
    Dimension min;

    /* calculate preferred geometry information */
    CalcGeoInfo(nb, instigator, desired, &ideal_width, &ideal_height, TRUE);

    /* adjust children's width */
    /* Notebook's width is bigger than ideal width */
    if (nb->core.width > ideal_width)
	nb->notebook.page_width += nb->core.width - ideal_width;

    /* Notebook's width is smaller than ideal width */
    else if (nb->core.width < ideal_width)
    {
	/* minimize the page area */
	min = ideal_width - nb->notebook.page_width +
	      nb->notebook.scroller_width;
        if (nb->core.width >= min)
	    nb->notebook.page_width = nb->core.width -
			(min - nb->notebook.scroller_width);
	else if (nb->core.width < min)
	{
	    nb->notebook.page_width = nb->notebook.scroller_width;
	    if (nb->notebook.orientation == XmHORIZONTAL)
	    {
		/* minimize the binding area */
		min -= nb->notebook.real_binding_width;
		if (nb->core.width >= min)
		    nb->notebook.real_binding_width = nb->core.width - min;
		else
		{
		    /* minimize major tabs */
		    nb->notebook.real_binding_width = 0;
		    min -= nb->notebook.major_width;
		    if (nb->core.width > min)
			nb->notebook.major_width = nb->core.width - min;
		    else
			nb->notebook.major_width = 0;
		}
	    }
	    else
	    {
		/* minimize minor tabs */
		min -= nb->notebook.minor_width;
		if (nb->core.width > min)
		    nb->notebook.minor_width = nb->core.width - min;
		else
		    nb->notebook.minor_width = 0;
	    }
	}
    }

    /* adjust children's height */
    /* Notebook's height is bigger than ideal height */
    if (nb->core.height > ideal_height)
	nb->notebook.page_height += nb->core.height - ideal_height;

    /* Notebook's height is smaller than ideal height */
    else if (nb->core.height < ideal_height)
    {
	/* minimize the page area */
	min = ideal_height - nb->notebook.page_height;
        if (nb->core.height >= min)
	    nb->notebook.page_height = nb->core.height - min;
	else if (nb->core.height < min)
	{
	    /* minimize the page scroller area */
	    nb->notebook.page_height = 0;
	    min -= nb->notebook.scroller_height;
	    if (nb->core.height >= min)
		nb->notebook.scroller_height = nb->notebook.status_height =
			nb->core.height - min;
	    else
	    {
		nb->notebook.scroller_height = nb->notebook.status_height = 0;
		if (nb->notebook.orientation == XmVERTICAL)
		{
		    /* minimize the binding area */
		    min -= nb->notebook.real_binding_width;
		    if (nb->core.height >= min)
			nb->notebook.real_binding_width = nb->core.height - min;
		    else
		    {
			/* minimize major tabs */
			nb->notebook.real_binding_width = 0;
			min -= nb->notebook.major_height;
			if (nb->core.height > min)
			    nb->notebook.major_height = nb->core.height - min;
			else
			    nb->notebook.major_height = 0;
		    }
		}
		else
		{
		    /* minimize minor tabs */
		    min -= nb->notebook.minor_height;
		    if (nb->core.height > min)
			nb->notebook.minor_height = nb->core.height - min;
		    else
			nb->notebook.minor_height = 0;
		}
	    }
	}
    }

    /* Calculate status w/h based on page,margin,& scroller w/h */
    nb->notebook.status_width = MAX(0,(int)(nb->notebook.page_width -
	nb->notebook.margin_width - nb->notebook.scroller_width));
    nb->notebook.status_height = nb->notebook.scroller_height =
	MAX(nb->notebook.status_height, nb->notebook.scroller_height);

    /* Calculate the notebook frame size based on page size,shadows,& margins*/
      nb->notebook.frame_width = nb->notebook.page_width +
                   (nb->notebook.shadow_thickness * 2) +
                   (nb->notebook.margin_width * 2) + 1;
      nb->notebook.frame_height = nb->notebook.page_height + nb->notebook.status_height +
                   (nb->notebook.shadow_thickness * 2) +
                   (nb->notebook.margin_height * 3) + 1;

}



/*****************************************************************************
 *                                                                           *
 *			   Child Managing Functions			     *
 *                                                                           *
 *****************************************************************************/


/*- SetLastPageNumber ----------------------------------------------------

    Sets XmNlastPageNumber if no explicit last page number was set,
    and updates navigators.

    Uses instance state:
	dynamic_last_page_num		,True, if no explicit setting

    Called from:
	AssignDefaultPageNumber		,when children are managed
	ConstraintSetValues		,when a child page # is changed

    Return:
	True, if last_page_number changed. Otherwise, False.

-----------------------------------------------------------------------------*/
static Boolean
SetLastPageNumber (
    XmNotebookWidget nb,
    int last_page_number)
{
    if (nb->notebook.dynamic_last_page_num
    && last_page_number > nb->notebook.last_page_number)
	{
	nb->notebook.last_page_number = last_page_number;
	UpdateNavigators(nb);
	return(True);
	}
    return(False);
}


/*- AssignDefaultPageNumber -------------------------------------------------

        assign a default page number to those children who did not get
	from the application.

	When a page is managed without a page number, the Notebook provides
	the page with the smallest unallocated page number that is greater
	than or equal to the first page number and is greater than the last
	allocated page number. When a tab or a status area is managed without
	a page number, the widget gets the page number of the most recently
	managed page that does not have the same type of child. If the page
	does have the same type of child, however, the Notebook associates the
	child with the next page whose page number is one greater than the
	most recently managed one. The Notebook, then, assumes the new page
	number is occupied. The next subsequent page without a page number
	will not receive this number. The default page number that the Notebook
	generates can exceed the last page number, which makes those page
	inaccessable by the user.

-----------------------------------------------------------------------------*/
static Boolean
AssignDefaultPageNumber (
    XmNotebookWidget nb)
{
    XmNotebookConstraint nc;
    Widget child;
    int i, last_page_number;

    /* initialize */
    last_page_number = nb->notebook.first_page_number;

    /* for all children */
    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);

	/* for all managed children */
	if (XtIsManaged(child))
	    {
	    /* assign a default page number */
	    if (nc->page_number == XmUNSPECIFIED_PAGE_NUMBER)
		{
		if (NB_IS_CHILD_PAGE(nc->child_type))
		    {
		    nc->page_number = GetNextAvailPageNum(nb, i);
		    nb->notebook.last_alloc_num = nc->page_number;
		    }
		else if (NB_IS_CHILD_MAJOR(nc->child_type) ||
		         NB_IS_CHILD_MINOR(nc->child_type) ||
		         NB_IS_CHILD_STATUS(nc->child_type))
		    {
		    if (GetChildWidget(nb, nb->notebook.last_alloc_num,
					    nc->child_type))
			{
			nc->page_number = GetNextAvailPageNum(nb, i);
			nb->notebook.last_alloc_num = nc->page_number;
			}
		    else
			nc->page_number = nb->notebook.last_alloc_num;
		    }
		}
	    else
		nb->notebook.last_alloc_num = nc->page_number;

	    /* update the last_page_number */
	    if (NB_IS_CHILD_PAGE(nc->child_type)
	    || NB_IS_CHILD_TAB(nc->child_type)
	    || NB_IS_CHILD_STATUS(nc->child_type))
		{
		ASSIGN_MAX(last_page_number, nc->page_number);
		}
	    } /* if */
	} /* for */
    return(SetLastPageNumber(nb, last_page_number));
}


/*- SetActiveChildren ------------------------------------------------------

    Determine which children should be active.

    Active children are all managed scrollers and managed pages, status
    areas, and tabs which are within the page number range and not
    duplicated or later matched ones if duplicated.

    Uses instance state:
	first_page_number
	last_page_number

    Updates child constraint "active" field
	FALSE, if child is NOT managed or
		  child page # less than notebook.first_page_number or
		  child page # greater than notebook.last_page_number or
		  child page # duplicated by a child of the same type
	TRUE,  otherwise


-----------------------------------------------------------------------------*/
static void
SetActiveChildren (
    XmNotebookWidget nb)
{
    Widget child;
    XmNotebookConstraint nc;
    XmNotebookConstraint last = NULL;		/* initial previous child */
    unsigned char type = XmNONE;		/* initial previous type */
    int num  = XmUNSPECIFIED_PAGE_NUMBER;	/* initial previous page */
    int i;

    for (i = 0; i < nb->composite.num_children; i++)
	{
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);
	if (XtIsManaged(child)
	    && nc->page_number >= nb->notebook.first_page_number
	    && nc->page_number <= nb->notebook.last_page_number)
	    {
	    if (last)
		last->active = !(nc->child_type == type
				&& nc->page_number == num);
	    last = nc;
	    type = nc->child_type;
	    num = nc->page_number;
	    }
	else /* NOT managed or NOT (1st page) <= (page #) <= (last page #) */
	    {
	    nc->active = False;
	    }
	}
    if (last)
	last->active = True;
}


/*- CompareChildren ---------------------------------------------------------

        Compares children, used for qsort() in SortChildren.

-----------------------------------------------------------------------------*/
static int
CompareChildren (
    XmConst void *a,
    XmConst void *b)
{
    XmNotebookConstraint ac = NotebookConstraint(*((Widget *)a));
    XmNotebookConstraint bc = NotebookConstraint(*((Widget *)b));
    int cmp;

    /* Compare page numbers */
    cmp = ac->page_number - bc->page_number;

    /* Compare child types, if page numbers are same */
    if (!cmp)
	cmp = ac->child_type - bc->child_type;

    /* Compare position in the array, if child types are the same */
    if (!cmp)
	cmp = (unsigned long)a - (unsigned long)b;

    return cmp;
}


/*- SortChildren ------------------------------------------------------------

        Sorts children by page number and type.

	Assumptions:
		Previously created children are already sorted,
		and newly created children are at the end of the
		composite.children list.

-----------------------------------------------------------------------------*/
static void
SortChildren (
    XmNotebookWidget nb)
{


    qsort(nb->composite.children, nb->composite.num_children,
            sizeof(Widget), CompareChildren);

    SetActiveChildren(nb);
}


/*- RepositionChild ---------------------------------------------------------

        Repositions a childs position in composite array using an
        insertion method.

-----------------------------------------------------------------------------*/
static void
RepositionChild (
    XmNotebookWidget nb,
    Widget child)
{
    XmNotebookConstraint rnc = NotebookConstraint(child);
    XmNotebookConstraint nc;            /* temp comparator value */
    Widget w;                           /* temp comparator value */
    int cmp;                            /* temp comparator value */
    int cur_pos = -1;                   /* position of changing child */
    int ins_pos = -1;                   /* position to insert child */
    int i;                              /* counter */

    /* nothing to do, if there is only one child in array */
    if (nb->composite.num_children == 1)
        return;

    for (i = 0; i < nb->composite.num_children; i++)
        {
        w = nb->composite.children[i];
        nc = NotebookConstraint(w);
        if (rnc == nc)
            cur_pos = i;
        else if (ins_pos < 0)
            {
            cmp = rnc->page_number - nc->page_number;
            if (!cmp) cmp = rnc->child_type - nc->child_type;
            if (!cmp) cmp = (unsigned long)child - (unsigned long)w;
            if (cmp < 0)
                ins_pos = i; /* should be inserted before this element */
            }
        } /* for */

    /* error, unable to locate repositioning child in array??? */
    if (cur_pos < 0)
        return;

    /* found no one less than, move others down, and insert at last position */
    if (ins_pos < 0)
        {
        for (i = cur_pos; i < nb->composite.num_children -1; i++)
            nb->composite.children[i] = nb->composite.children[i+1];
        nb->composite.children[nb->composite.num_children -1] = child;
        }
    /* found new position above, move others down, and then insert */
    else if (cur_pos < ins_pos)
        {
        for (i = cur_pos; i < ins_pos -1; i++)
            nb->composite.children[i] = nb->composite.children[i+1];
        nb->composite.children[ins_pos -1] = child;
        }
    /* found new position below, move others up, and then insert */
    else if (cur_pos > ins_pos)
        {
        for (i = cur_pos; i > ins_pos; i--)
            nb->composite.children[i] = nb->composite.children[i-1];
        nb->composite.children[ins_pos] = child;
        }
}


/*- GetNextAvailPageNum -----------------------------------------------------

        Finds out the next available page number for allocating it to
	a newly created page or other child.

-----------------------------------------------------------------------------*/
static int
GetNextAvailPageNum(
    XmNotebookWidget nb,
    int last)
{
    XmNotebookConstraint nc;
    Widget child;
    int i, avail;

    avail = nb->notebook.last_alloc_num;
    for (i = 0; i < last; i++)
    {
	child = nb->composite.children[i];
	nc = NotebookConstraint(child);
	if (XtIsManaged(child) && nc->page_number == avail)
	    avail++;
    }
    return avail;
}


/*- GetChildWidget ---------------------------------------------------------

        Finds the later matched child widget of the specified type.
	It returns NULL if not found.

-----------------------------------------------------------------------------*/
static Widget
GetChildWidget(
    XmNotebookWidget nb,
    int page_number,
    unsigned char child_type)
{
    XmNotebookConstraint nc;
    Widget child, this_w;
    int i;

    this_w = NULL;
    for (i = 0; i < nb->composite.num_children; i++)
    {
        child = nb->composite.children[i];
        nc = NotebookConstraint(child);
	if (nc->page_number == page_number && nc->child_type == child_type)
	    {
	    this_w = child;
	    }
	else
	    {
	    /* Skip remaining array once last matching child is found */
	    if (this_w)
		break;
	    }
    }
    return this_w;
}


/*- GotoPage ---------------------------------------------------------------

        Places the specified page on top.

	If the specified page IS currently on top nothing is done.
	Otherwise,
	    - the XmNpageChangedCallback is called. During which
	      notebook.in_callback is set to True. This is checked
	      in XmNotebook:ConstraintSetValues() to avoid performing
	      layout at that time.
	    - navigators are updated
	    - relayout is performed

	Called from:
		SetValues(), when notebook.current_page_number is set
		TabPressed(), when tabs are activated
		PageMove(), when ScrollFrame trait navigation is triggered

-----------------------------------------------------------------------------*/
static void
GotoPage(
    XmNotebookWidget nb,
    int page_number,
    XEvent *event,
    int reason)
{
    XmNotebookCallbackStruct call_value;
    Dimension save_width, save_height;
    Widget old_top_major, old_first_major;
    Widget old_top_minor, old_first_minor;
    int prev_page = nb->notebook.current_page_number;


    /* Don't bother when the page number is out of the page number
       range, or if it is same as the old page number */
    if (   page_number == nb->notebook.current_page_number
	|| page_number < nb->notebook.first_page_number
	|| page_number > nb->notebook.last_page_number)
	return;

    /* Save the notebook's size before invoking the callback */
    save_width = XtWidth(nb);
    save_height = XtHeight(nb);

    /* Set the NEW current page number */
    nb->notebook.current_page_number = page_number;

    /* Invoke the XmNpageChangedCallback, if any exist */
    if ((XtHasCallbacks((Widget)nb, XmNpageChangedCallback)
			    == XtCallbackHasSome))
	{
	call_value.reason 	    = reason;
	call_value.event 	    = event;
	call_value.page_number 	    = page_number;
	call_value.page_widget 	    = GetChildWidget(nb, page_number, XmPAGE);
	call_value.prev_page_number = prev_page;
	call_value.prev_page_widget = GetChildWidget(nb, prev_page, XmPAGE);

	/* Mark that the callback is being called and when it finishes */
	nb->notebook.in_callback = True;
	XtCallCallbackList((Widget)nb, nb->notebook.page_change_callback,
			    &call_value);
	nb->notebook.in_callback = False;
	}

    /* Tell all navigators about our page change */
    UpdateNavigators(nb);

    /* If there is any visual change caused by the callback then
	relayout and redisplay children */
    if (save_width != XtWidth(nb) || save_height != XtHeight(nb))
	{
        LayoutChildren(nb, NULL);
	if (XtIsRealized((Widget)nb))
            XClearArea(XtDisplay(nb),XtWindow(nb),0,0,0,0,True);
	}
    /* Otherwise just relayout children */
    else
	{
	/* Save top tab pointers */
	old_top_major = nb->notebook.top_major;
	old_first_major = nb->notebook.first_major;
	old_top_minor = nb->notebook.top_minor;
	old_first_minor = nb->notebook.first_minor;

	/* Reset tab pointers */
	ResetTopPointers(nb, XmPAGE, 0);

	/* Layout children */
	LayoutPages(nb, NULL);

	if (   old_top_major != nb->notebook.top_major
	    || old_first_major != nb->notebook.first_major)
	    LayoutMajorTabs(nb, NULL);

	if (   old_top_minor != nb->notebook.top_minor
	    || old_first_minor != nb->notebook.first_minor)
	    LayoutMinorTabs(nb, NULL);
	}


}



/*****************************************************************************
 *                                                                           *
 *                            utility functions                              *
 *                                                                           *
 *****************************************************************************/


/*- ShowChild ---------------------------------------------------------------

        Change the dimensional aspects of a child, and display it.

-----------------------------------------------------------------------------*/
static void
ShowChild (
    Widget child,
    Widget instigator,
    int x,
    int y,
    int width,
    int height)
{
    int border_width;

    /* adjust width and height and calculate border_width */
    border_width = child->core.border_width;
    width -= border_width * 2;
    height -= border_width * 2;
    /* Since width and height can potentially be 0, need to check
       that they aren't negative after subtracting the border */
    if (width <= 0)
	width = 1, border_width = 0;
    if (height <= 0)
	height = 1, border_width = 0;

    /* Configure the object */
    if (child == instigator)
	{
	/* if this child is making a geometry request */
	XtX(child) = x;
	XtY(child) = y;
	XtWidth(child) = width;
	XtHeight(child) = height;
	}
    else
	{
	/* otherwise, configure the child */
	XmeConfigureObject(child,
				(Position)x,
				(Position)y,
				(Dimension)width,
				(Dimension)height,
				(Dimension)border_width);
	}
}


/*- HideChild ---------------------------------------------------------------

        Hide a child by placing it to invisible place.

-----------------------------------------------------------------------------*/
static void
HideChild (
    Widget child,
    Widget instigator)
{
    int x = - (int)(child->core.width + child->core.border_width * 2);
    int y = - (int)(child->core.height + child->core.border_width * 2);

    /* if the child is already invisible, don't bother */
    if (!NB_IS_VISIBLE(child))
	return;

    /* place the child to (x, y) */
    if (child == instigator)
	{
	/* if child is making a geometry request */
	XtX(child) = (Position)x;
	XtY(child) = (Position)y;
	}
    else
	{
	/* otherwise, configure the child */
	XmeConfigureObject(child,
			(Position)x,
			(Position)y,
			child->core.width,
			child->core.height,
			child->core.border_width);
	}
}


/*- HideShadowedTab ---------------------------------------------------------

        Clear tab child and its surrounding frame shadow

-----------------------------------------------------------------------------*/
static void
HideShadowedTab (
    XmNotebookWidget nb,
    Widget child)
{
    int x, y, width, height;

    if (XtIsRealized((Widget)nb) && child && NB_IS_VISIBLE(child))
    {
        x = child->core.x - nb->notebook.shadow_thickness;
        y = child->core.y - nb->notebook.shadow_thickness;
        width = child->core.width + nb->notebook.shadow_thickness * 2 + 1;
        height = child->core.height + nb->notebook.shadow_thickness * 2 + 1;
	if ( nb->notebook.shadow_thickness )
          XClearArea(XtDisplay(nb), XtWindow(nb), x, y, width, height, True);
	else
          XClearArea(XtDisplay(nb), XtWindow(nb), x - 1 , y - 1, width + 1, height + 1, True);
    }
}



/*****************************************************************************
 *                                                                           *
 *                             callback functions                            *
 *                                                                           *
 *****************************************************************************/


/*- FlipTabs ----------------------------------------------------------------

        callback function for events for tab scrolling

-----------------------------------------------------------------------------*/
static void
FlipTabs(
    Widget w,
    XtPointer data,
    XtPointer call_data)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(w);
    Widget old_first_major, old_first_minor;
    Widget cfw = XmGetFocusWidget((Widget)nb);

    /* save tab pointers */
    (void)data;
    (void)call_data;
    old_first_major = nb->notebook.first_major;
    old_first_minor = nb->notebook.first_minor;

    /* request to reset notebook.first_major */
    if (w == nb->notebook.next_major)
	ResetTopPointers(nb, XmMAJOR_TAB_SCROLLER, _NEXT);
    else if (w == nb->notebook.prev_major)
	ResetTopPointers(nb, XmMAJOR_TAB_SCROLLER, _PREVIOUS);
    else if (w == nb->notebook.next_minor)
	ResetTopPointers(nb, XmMINOR_TAB_SCROLLER, _NEXT);
    else if (w == nb->notebook.prev_minor)
	ResetTopPointers(nb, XmMINOR_TAB_SCROLLER, _PREVIOUS);

    /* redraw tabs if necessary */
    if (old_first_major != nb->notebook.first_major)
	LayoutMajorTabs(nb, NULL);
    if (old_first_minor != nb->notebook.first_minor)
	LayoutMinorTabs(nb, NULL);

    /*
     * If focus was a tab widget that is now hidden due to tab scrolling then
     * move focus to the next visible tab
     */
    if (cfw && XtParent(cfw) == (Widget)nb)
	{
	unsigned char ct = NotebookConstraint(cfw)->child_type;

	if (NB_IS_HIDDEN(cfw) && NB_IS_CHILD_TAB(ct))
	    {
	    if ((w == nb->notebook.next_major) && NB_IS_CHILD_MAJOR(ct))
		XmProcessTraversal(
		    (Widget) GetNextTab(nb, XmMAJOR_TAB, 0, _FIRST_VISIBLE),
		    XmTRAVERSE_CURRENT);
	    else if (w == nb->notebook.prev_major && NB_IS_CHILD_MAJOR(ct))
		XmProcessTraversal(
		    (Widget) GetNextTab(nb, XmMAJOR_TAB, 0, _LAST_VISIBLE),
		    XmTRAVERSE_CURRENT);
	    else if (w == nb->notebook.next_minor && NB_IS_CHILD_MINOR(ct))
		XmProcessTraversal(
		    (Widget) GetNextTab(nb, XmMINOR_TAB, 0, _FIRST_VISIBLE),
		    XmTRAVERSE_CURRENT);
	    else if (w == nb->notebook.prev_minor && NB_IS_CHILD_MINOR(ct))
		XmProcessTraversal(
		    (Widget) GetNextTab(nb, XmMINOR_TAB, 0, _LAST_VISIBLE),
		    XmTRAVERSE_CURRENT);
	    }
	}
}


/*- TabPressed --------------------------------------------------------------

        callback function for events for tab push-buttons.

-----------------------------------------------------------------------------*/
static void
TabPressed (
    Widget w,
    XtPointer data,
    XtPointer call_data)
{
    XmNotebookWidget nb = (XmNotebookWidget)XtParent(w);
    XmNotebookConstraint nc = NotebookConstraint(w);
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    int reason = (unsigned long)data;

    /* move to the selected page */
    GotoPage(nb, nc->page_number, cbs->event, reason);
}



/*****************************************************************************
 *                                                                           *
 *                Keyboard Traversal Functions & Action Procs                *
 *                                                                           *
 *****************************************************************************/


/*- TraverseTab -------------------------------------------------------------

        action for moving the focus on tabs

-----------------------------------------------------------------------------*/
static void
TraverseTab(
    Widget w,
    XEvent *event,
    String *params,
    Cardinal *num_params)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;
    XmNotebookConstraint nc;
    Widget first, last, next, prev, child = NULL;
    int traverse_to;

    /* Check error conditions */
    (void)event;
    if (nb && XmIsNotebook(nb))
        child = XmGetFocusWidget(w);
    else
        while (nb && !(XmIsNotebook(nb)))
           {
           child = (Widget)nb;
           nb = (XmNotebookWidget)XtParent(child);
           }
    if (!nb || !child)
        return;

    if (!num_params || (*num_params != 1) || !params)
	{
        XmeWarning(w, MESSAGE1);
        return;
	}

    /* Only valid for major or minor tabs */
    if (!(nc = NotebookConstraint(child))
	|| !(NB_IS_CHILD_MAJOR(nc->child_type)
	   || NB_IS_CHILD_MINOR(nc->child_type)))
        return;

    if (_XmConvertActionParamToRepTypeId((Widget) nb,
				 XmRID_NOTEBOOK_TRAVERSE_TAB_ACTION_PARAMS,
				 params[0], False, &traverse_to) == False)
	traverse_to = _HOME;


    /*
     * Make the traversal
     */
    if (traverse_to == _HOME)
	{
	/* Major HOME traversal */
	first = GetNextTab(nb,nc->child_type,nc->page_number,_HOME);
	if ((first) && NB_IS_HIDDEN(first))
	    {
	    if (NB_IS_CHILD_MAJOR(nc->child_type))
	    	{
		ResetTopPointers(nb,XmMAJOR_TAB_SCROLLER,_HOME);
	        LayoutMajorTabs(nb, NULL);
	        }
	    else
	        {
		ResetTopPointers(nb,XmMINOR_TAB_SCROLLER,_HOME);
		LayoutMinorTabs(nb, NULL);
		}
	    }
	if (first)
	    XmProcessTraversal(first,XmTRAVERSE_CURRENT);
	}

    else if (traverse_to == _END)
	{
	last = GetNextTab(nb,nc->child_type,nc->page_number,_END);
	if ((last) && NB_IS_HIDDEN(last))
	    {
	    if (NB_IS_CHILD_MAJOR(nc->child_type))
	    	{
		ResetTopPointers(nb,XmMAJOR_TAB_SCROLLER,_END);
	    	LayoutMajorTabs(nb, NULL);
	    	}
	    else
	    	{
		ResetTopPointers(nb,XmMINOR_TAB_SCROLLER,_END);
	    	LayoutMinorTabs(nb, NULL);
	    	}
	    }
	if (last)
	    XmProcessTraversal(last,XmTRAVERSE_CURRENT);
	}

    else if (traverse_to == _PREVIOUS)
	{
	prev = GetNextTab(nb,nc->child_type,nc->page_number,_PREVIOUS);
	if ((prev) && NB_IS_HIDDEN(prev))
	    {
	    if (NB_IS_CHILD_MAJOR(nc->child_type))
		FlipTabs(
		    (MaxIsRightUp(nb,nc->child_type)) ?
		    nb->notebook.prev_major : nb->notebook.next_major,
		    NULL,NULL);
	    else
		FlipTabs(
		    (MaxIsRightUp(nb,nc->child_type)) ?
		    nb->notebook.prev_minor : nb->notebook.next_minor,
		    NULL,NULL);
	    }
	if (prev)
	    XmProcessTraversal(prev, XmTRAVERSE_CURRENT);
	}

    else if (traverse_to == _NEXT)
	{
	next = GetNextTab(nb,nc->child_type,nc->page_number,_NEXT);
        if ((next) && NB_IS_HIDDEN(next))
            {
            if (NB_IS_CHILD_MAJOR(nc->child_type))
                FlipTabs(
		    (MaxIsRightUp(nb,nc->child_type)) ?
		    nb->notebook.next_major : nb->notebook.prev_major,
                    NULL,NULL);
            else
                FlipTabs(
                    (MaxIsRightUp(nb,nc->child_type)) ?
		    nb->notebook.next_minor : nb->notebook.prev_minor,
                    NULL,NULL);
            }
	if (next)
	    XmProcessTraversal(next, XmTRAVERSE_CURRENT);
	}
}


/*- RedirectTraversal -------------------------------------------------------

        redirect traversal control trait

			Focus From      Focus To        Focus Request
	NEXT_TAB_GROUP  <anything>      MINOR           _CURRENT_VISIBLE MAJOR
			MAJOR           <anything>      _CURRENT_VISIBLE MINOR
	PREV_TAB_GROUP <anything>       MAJOR           _CURRENT_VISIBLE MINOR
			MINOR           <anything>      _CURRENT_VISIBLE MAJOR


-----------------------------------------------------------------------------*/
static Widget
RedirectTraversal(
    Widget 		old_focus,
    Widget	       	new_focus,
    unsigned int	focus_policy,
    XmTraversalDirection direction,
    unsigned int	pass)
{
    unsigned char to_type = XmMAJOR_TAB, from_type = XmMAJOR_TAB;
    Widget to_child, from_child;
    Widget to_parent, from_parent;
    Widget new_focus_widget;

    /* If we're in pointer focus mode there's nothing to be done. */
    (void)pass;
    if ((focus_policy != XmEXPLICIT)
     || ((direction != XmTRAVERSE_NEXT_TAB_GROUP) &&
	 (direction != XmTRAVERSE_PREV_TAB_GROUP)))
    return new_focus;

    /* Determine nearest notebook parent for the target. */
    to_parent = NULL;
    if ((to_child=new_focus))
	{
	if ((to_parent=XtParent(to_child)) == NULL)
	    return new_focus;
	while (!XmIsNotebook(to_parent))
	    {
	    to_child = to_parent;
	    if ((to_parent=XtParent(to_child)) == NULL)
		break;
	    }
	}

    /* Determine target child type. */
    if (to_parent)
	to_type = NotebookConstraint(to_child)->child_type;

    /* Determine nearest notebook parent for the source. */
    from_parent = NULL;
    if ((from_child=old_focus))
	{
        if ((from_parent = XtParent(from_child)) == NULL)
	    return new_focus;
	while (!XmIsNotebook(from_parent))
	    {
	    from_child = from_parent;
	    if ((from_parent=XtParent(from_child)) == NULL)
		break;
	    }
	}

    /* Determine source child type. */
    if (from_parent)
	from_type = NotebookConstraint(from_child)->child_type;

    /*
     * Determine if we need to force traversal between major and
     * minor tabs (traverse to nearest tab to the current page)
     *
     * Note: If TO is NULL, then it is assumed that there is only
     * one TAB GROUP i.e., the Major/Minor tabs TAB GROUP
     */
    new_focus_widget = NULL;

    if (direction == XmTRAVERSE_NEXT_TAB_GROUP)
	{
	/* If FROM Minor with a NULL TO then GOTO Major */
        if (to_child == NULL && (from_parent && (from_type == XmMINOR_TAB)))
	    new_focus_widget = GetNextTab((XmNotebookWidget)from_parent,
					    XmMAJOR_TAB,0,_CURRENT_VISIBLE);

	/* If FROM Major with any TO then GOTO Minor */
	else if (from_parent && (from_type == XmMAJOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)from_parent,
					    XmMINOR_TAB,0,_CURRENT_VISIBLE);

	/* If *NOT* FROM Major and TO Minor then GOTO Major */
	else if (to_parent && (to_type == XmMINOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)to_parent,
					    XmMAJOR_TAB,0,_CURRENT_VISIBLE);

	/* If TO Major with any FROM then GOTO Major */
	else if (to_parent && (to_type == XmMAJOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)to_parent,
                                            XmMAJOR_TAB,0,_CURRENT_VISIBLE);
	}
    else /* if (direction == XmTRAVERSE_PREV_TAB_GROUP) */
	{
	/* If FROM Major with a NULL TO then GOTO Minor */
        if (to_child == NULL && (from_parent && (from_type == XmMAJOR_TAB)))
	    new_focus_widget = GetNextTab((XmNotebookWidget)from_parent,
					    XmMINOR_TAB,0,_CURRENT_VISIBLE);

        /* If FROM Minor with any TO and then GOTO Major */
	else if (from_parent && (from_type == XmMINOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)from_parent,
					    XmMAJOR_TAB,0,_CURRENT_VISIBLE);

	/* If *NOT* FROM Minor with TO Major then GOTO Major */
	else if (to_parent && (to_type == XmMAJOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)to_parent,
					    XmMINOR_TAB,0,_CURRENT_VISIBLE);

	/* If to a Minor then traverse to a Minor nearest the current page */
        else if (to_parent && (to_type == XmMINOR_TAB))
	    new_focus_widget = GetNextTab((XmNotebookWidget)to_parent,
                                            XmMINOR_TAB,0,_CURRENT_VISIBLE);
	}

    if (new_focus_widget && XmIsTraversable(new_focus_widget))
	return new_focus_widget;

    return new_focus;
}

/*- GetNextTab --------------------------------------------------------------

        Get next tab to set focus to.

	In Parameters:
		child_type	XmMAJOR_TAB or XmMINOR_TAB
		page_number	page number of interest
		direction	traversal direction
        returns
		next major || minor tab (can be NULL)

-----------------------------------------------------------------------------*/
static Widget
GetNextTab (
    XmNotebookWidget nb,
    unsigned char child_type,
    int page_number,
    unsigned char direction)
{
    XmNotebookConstraint nc;
    Widget child;
    int i;
    Widget target;
    unsigned char target_dir;
    unsigned char target_child_type;
    Boolean  target_found;
    int top_major_page;

    if (NB_IS_CHILD_MAJOR(child_type) || NB_IS_CHILD_MINOR(child_type))
	target_child_type = child_type;
    else
	return(NULL);	/* Bad parameter */

    if (MaxIsRightUp(nb,target_child_type))
	target_dir = direction;
    else
	switch(direction)
	{
	case _PREVIOUS:
	    target_dir = _NEXT;
	    break;
	case _NEXT:
	    target_dir = _PREVIOUS;
	    break;
	default:
	    target_dir = direction;
	}

    /*
     * Attempt to return top tab
     */
    if (target_dir == _CURRENT_VISIBLE)
	{
	if (NB_IS_CHILD_MAJOR(child_type))
	    {
	    if ( (nb->notebook.top_major != NULL)
	      && (NB_IS_VISIBLE(nb->notebook.top_major)))
		return nb->notebook.top_major;
	    else
		target_dir = _FIRST_VISIBLE;
	    }
	else if (NB_IS_CHILD_MINOR(child_type))
	    {
	    if (  (nb->notebook.top_minor != NULL)
	       && (NB_IS_VISIBLE(nb->notebook.top_minor)))
		return nb->notebook.top_minor;
	    else
		target_dir = _FIRST_VISIBLE;
	    }
	}

    i = 0;
    target = NULL;
    target_found = False;
    if (NB_IS_CHILD_MAJOR(target_child_type))
	{
	while((!target_found) && (i < nb->composite.num_children))
	  {
	  child = nb->composite.children[i];
	  nc = NotebookConstraint(child);
	  if ((nc->active) && (NB_IS_CHILD_MAJOR(nc->child_type)))
	    switch (target_dir)
	    {
	    case _HOME:
		target = child;
		target_found = True;
		break;
	    case _FIRST_VISIBLE:
		if (NB_IS_VISIBLE(child))
		    {
		    target = child;
		    target_found = True;
		    }
		break;
	    case _LAST_VISIBLE:
		if (NB_IS_VISIBLE(child))
		    {
		    target = child;
		    }
		break;
	    case _PREVIOUS:
		if (nc->page_number < page_number)
		    target = child;
		else
		    target_found = True;
		break;
	    case _NEXT:
		if (nc->page_number > page_number)
		    {
		    target = child;
		    target_found = True;
		    }
		break;
            case _END:
		target = child;
	    } /* switch */
	    i++;
	  }   /* while  */
	}     /* if     */
    else /* NB_IS_CHILD_MINOR(target_child_type) */
	{
        top_major_page = nb->notebook.top_major ?
                NotebookConstraint(nb->notebook.top_major)->page_number :
                nb->notebook.first_page_number - 1;

	while((!target_found) && (i < nb->composite.num_children))
          {
          child = nb->composite.children[i];
          nc = NotebookConstraint(child);
	  if (NB_IS_CHILD_MAJOR(nc->child_type)
	  && (nc->page_number > top_major_page))
	    target_found = True;
	  else
	    if ((nc->active)
		&& (NB_IS_CHILD_MINOR(nc->child_type))
		&& (nc->page_number >= top_major_page))
              switch (target_dir)
              {
	      case _HOME:
		target = child;
		target_found = True;
	    	break;
	      case _FIRST_VISIBLE:
		if (NB_IS_VISIBLE(child))
		    {
		    target = child;
		    target_found = True;
		    }
		break;
	      case _LAST_VISIBLE:
		if (NB_IS_VISIBLE(child))
		    {
		    target = child;
		    }
		break;
	      case _PREVIOUS:
		if (nc->page_number < page_number)
		    target = child;
		else
		    target_found = True;
		break;
	      case _NEXT:
		if (nc->page_number > page_number)
		    {
		    target = child;
		    target_found = True;
		    }
		break;
	      case _END:
		target = child;
	      } /* switch */
	    i++;
	  }     /* while  */
	}       /* else   */
    return(target);
}


/*- MaxIsRightUp ------------------------------------------------------------

        returns
                True,
                    if "<Key>osfRight" and "<Key>osfDown" mean "next"
                    and "<Key>osfLeft" and "<Key>osfUp" mean "prev"

                False,
                    if "<Key>osfLeft" and "<Key>osfUp" mean "next"
                    and "<Key>osfRight" and "<Key>osfDown" mean "prev"

-----------------------------------------------------------------------------*/
static Boolean
MaxIsRightUp (
    XmNotebookWidget nb,
    unsigned char child_type)
{
    return( (
		(NB_IS_CHILD_MAJOR(child_type))
	    &&  (
		    (nb->notebook.back_page_pos == XmBOTTOM_RIGHT)
		||  (
			(nb->notebook.back_page_pos == XmBOTTOM_LEFT)
		    &&  (nb->notebook.orientation == XmHORIZONTAL))
		||
		    (
			(nb->notebook.back_page_pos == XmTOP_RIGHT)
		    &&  (nb->notebook.orientation == XmVERTICAL))))
	||  (
		(NB_IS_CHILD_MINOR(child_type))
	    &&  (
		    (nb->notebook.back_page_pos == XmBOTTOM_RIGHT)
		||  (
			(nb->notebook.back_page_pos == XmBOTTOM_LEFT)
		    &&  (nb->notebook.orientation == XmVERTICAL))
		||  (
			(nb->notebook.back_page_pos == XmTOP_RIGHT)
		    &&  (nb->notebook.orientation == XmHORIZONTAL)))));
}



/*****************************************************************************
 *                                                                           *
 *                          Trait Stuff			      		     *
 *                                                                           *
 *****************************************************************************/


/*- ScrollFrameInit ---------------------------------------------------------

	ScrollFrame init trait method

-----------------------------------------------------------------------------*/
static void
ScrollFrameInit (
    Widget w,
    XtCallbackProc moveCB,
    Widget scrollable)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (nb->notebook.scroll_frame_data != NULL)
	return;

    nb->notebook.scroll_frame_data = (XmScrollFrameData)
        XtMalloc(sizeof(XmScrollFrameDataRec));

    nb->notebook.scroll_frame_data->num_nav_list = 0 ;
    nb->notebook.scroll_frame_data->nav_list = NULL ;
    nb->notebook.scroll_frame_data->num_nav_slots = 0 ;

    nb->notebook.scroll_frame_data->move_cb = moveCB ;
    nb->notebook.scroll_frame_data->scrollable = scrollable ;
}


/*- ScrollFrameGetInfo ------------------------------------------------------

        ScrollFrame getInfo trait method
	Notebook is a 1 dimensional frame.

-----------------------------------------------------------------------------*/
static Boolean
ScrollFrameGetInfo (
    Widget w,
    Cardinal *dimension,
    Widget **nav_list,
    Cardinal *num_nav_list)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (dimension) *dimension = 1;  /* one dimensional frame */

    if (nb->notebook.scroll_frame_data != NULL)
	{
        if (nav_list)
	    *nav_list = nb->notebook.scroll_frame_data->nav_list;
        if (num_nav_list)
            *num_nav_list = nb->notebook.scroll_frame_data->num_nav_list;
	}

    return( nb->notebook.scroll_frame_data != NULL );
}


/*- AddNavigator ------------------------------------------------------------

        ScrollFrame addNavigator trait method

-----------------------------------------------------------------------------*/
static void
AddNavigator (
    Widget w,
    Widget nav,
    Mask dimMask)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;
    if (nb->notebook.scroll_frame_data != NULL)
	_XmSFAddNavigator(w, nav, dimMask, nb->notebook.scroll_frame_data);
}


/*- RemoveNavigator ---------------------------------------------------------

        ScrollFrame removeNavigator trait method

-----------------------------------------------------------------------------*/
static void
RemoveNavigator (
    Widget w,
    Widget nav)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;

    if (nb->notebook.scroll_frame_data != NULL)
	_XmSFRemoveNavigator(w, nav, nb->notebook.scroll_frame_data);
}


/*- PageMove ----------------------------------------------------------------

	Callback for the value changes of navigators.

-----------------------------------------------------------------------------*/
static void
PageMove (
    Widget w,
    XtPointer data,
    XtPointer call_data)
{
    /* w is a navigator widget */
    XmNotebookWidget nb = (XmNotebookWidget)data;
    XmNavigatorDataRec nav_data;
    XmNavigatorTrait navigatorT;
    int reason;

    /* Get the new navigator value using the trait getValue */
    (void)call_data;
    nav_data.valueMask = NavValue;

    /* Check to ensure navigator is ok */
    if ((navigatorT=NB_NAVIGATOR(w)) == NULL || navigatorT->getValue == NULL)
	return;
    navigatorT->getValue(w, &nav_data);

    /* Determine the callback reason */
    if (nav_data.value.x > nb->notebook.current_page_number)
        reason = XmCR_PAGE_SCROLLER_INCREMENT;
    else if (nav_data.value.x < nb->notebook.current_page_number)
        reason = XmCR_PAGE_SCROLLER_DECREMENT;
    else
        reason = XmCR_NONE;

    /* Look at the kind of navigator and make the appropriate move */
    if ((nav_data.dimMask & NavigDimensionX) && (reason != XmCR_NONE))
	GotoPage(nb, nav_data.value.x, NULL, reason);

}


/*- UpdateNavigators --------------------------------------------------------

        Update navigators due to page change

-----------------------------------------------------------------------------*/
static void
UpdateNavigators (
    XmNotebookWidget nb)
{
    XmNavigatorDataRec nav_data;

    /* update navigators */
    nav_data.value.x = nb->notebook.current_page_number;
    nav_data.minimum.x = nb->notebook.first_page_number;
    nav_data.maximum.x = nb->notebook.last_page_number + 1;
    nav_data.slider_size.x = 1;
    nav_data.increment.x = 1;
    nav_data.page_increment.x = 1;
    nav_data.dimMask = NavigDimensionX;
    nav_data.valueMask = NavValue | NavMinimum | NavMaximum |
	NavSliderSize | NavIncrement | NavPageIncrement;

    _XmSFUpdateNavigatorsValue((Widget)nb, &nav_data, True);

    /* Control arrow sensitivity if using default page scroller */
    if (nb->notebook.scroller_status == DEFAULT_USED)
	{
	if ((nb->notebook.current_page_number ==nb->notebook.last_page_number)
	&& (nb->notebook.current_page_number ==nb->notebook.first_page_number))
	   XtVaSetValues(nb->notebook.scroller_child,XmNarrowSensitivity,
		XmARROWS_INSENSITIVE, NULL);
	else if (nb->notebook.current_page_number ==
					    nb->notebook.last_page_number)
	   XtVaSetValues(nb->notebook.scroller_child,XmNarrowSensitivity,
		XmARROWS_DECREMENT_SENSITIVE, NULL);
	else if (nb->notebook.current_page_number ==
					    nb->notebook.first_page_number)
	   XtVaSetValues(nb->notebook.scroller_child,XmNarrowSensitivity,
		XmARROWS_INCREMENT_SENSITIVE, NULL);
	else
	   XtVaSetValues(nb->notebook.scroller_child,XmNarrowSensitivity,
		XmARROWS_SENSITIVE, NULL);
	}
}


/*- GetUnhighlightGC --------------------------------------------------------

        ScrollFrame removeNavigator trait method

-----------------------------------------------------------------------------*/
static GC
GetUnhighlightGC (
    Widget w,
    Widget child)
{
    XmNotebookWidget nb = (XmNotebookWidget)w;
    XmNotebookConstraint nc;
    GC background_GC = NULL;

    if (child)
	{
	nc = NotebookConstraint(child);
	switch (nc->child_type)
	    {
	    case XmMAJOR_TAB:
		if (nb->notebook.top_major == child)
		    {
		    XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
				    nb->notebook.frame_background);
		    background_GC = nb->notebook.frame_gc;
		    }
		else
		  background_GC = nb->manager.background_GC;
		break;

	    case XmMINOR_TAB:
		if (nb->notebook.top_minor == child)
		    {
		    XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
				    nb->notebook.frame_background);
		    background_GC = nb->notebook.frame_gc;
		    }
		else
		    background_GC = nb->manager.background_GC;
		break;

	    case XmPAGE:
	    case XmPAGE_SCROLLER:
	    case XmSTATUS_AREA:
		XSetForeground(XtDisplay(nb), nb->notebook.frame_gc,
			       nb->notebook.frame_background);
		background_GC = nb->notebook.frame_gc;
		break;

	    case XmMAJOR_TAB_SCROLLER:
	    case XmMINOR_TAB_SCROLLER:
		background_GC = nb->manager.background_GC;
		break;

	    }
	}

    return(background_GC);
}


/*****************************************************************************
 *                                                                           *
 *                               Public Functions                            *
 *                                                                           *
 *****************************************************************************/


XmNotebookPageStatus
XmNotebookGetPageInfo (
    Widget notebook,
    int page_number,
    XmNotebookPageInfo *page_info)
{
    XmNotebookWidget nb = (XmNotebookWidget)notebook;
    XmNotebookConstraint nc;
    Widget child;
    Widget page, status, major_tab, minor_tab;
    XmNotebookPageStatus result;
    int i;
    _XmWidgetToAppContext(notebook);

    _XmAppLock(app);

    /* initialize */
    page = status = major_tab = minor_tab = NULL;
    result = XmPAGE_EMPTY;

    /* searching for the page */
    for (i = 0; i < nb->composite.num_children; i++)
    {
        child = nb->composite.children[i];
        nc = NotebookConstraint(child);

	if (nc->page_number > page_number)
	    break;
	switch (nc->child_type)
	    {
		case XmPAGE:
		    if (nc->page_number == page_number)
		    {
		        if (nc->active)
			{
			    page = child;
			    if (result == XmPAGE_EMPTY)
			        result = XmPAGE_FOUND;
		        }
			else
			    result = XmPAGE_DUPLICATED;
		    }
		    break;
		case XmSTATUS_AREA:
		    if ((nc->active) && (nc->page_number == page_number))
			status = child;
		    break;
		case XmMAJOR_TAB:
		    if (nc->active)
			major_tab = child;
		    break;
		case XmMINOR_TAB:
		    if (nc->active)
			minor_tab = child;
		    break;
	    }
    }

    /* see if the page number is invalid */
    if (page_number < nb->notebook.first_page_number ||
	page_number > nb->notebook.last_page_number)
	result = XmPAGE_INVALID;

    /* fill the XmNotebookPageInfo struct */
    page_info->page_number = page_number;
    page_info->page_widget = page;
    page_info->status_area_widget = status;
    page_info->major_tab_widget = major_tab;
    page_info->minor_tab_widget = minor_tab;

    _XmAppUnlock(app);
    return result;
}


Widget
XmCreateNotebook(
        Widget          parent,
        String		name,
        ArgList         arglist,
        Cardinal        argcount)
{
        /*
         * Create an instance of a notebook and return the widget id.
         */
   return(XtCreateWidget(name,xmNotebookWidgetClass,parent,arglist,argcount));
}

Widget
XmVaCreateNotebook(
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
                         xmNotebookWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedNotebook(
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
                         xmNotebookWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

