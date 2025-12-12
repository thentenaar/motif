/* $TOG: Container.c /main/7 1997/05/02 10:09:20 dbl $ */
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
 * 
 */
/*
 * HISTORY
 */

/********************************* WARNING **********************************
 *
 * This widget is for educational purposes only.
 * Commercial applications should NOT instantiate this widget.
 * OSF makes no guarantees on the stability of this widget.
 *
 ********************************* WARNING *********************************/


/******************************************************************************
 *
 * Container.c - ExmContainer widget.  
 *
******************************************************************************/

/* Include appropriate files. */
#include <Xm/GadgetP.h> /* header file for gadget management functions */
#include <Xm/TraitP.h>  /* header file for trait functions */
#include <Xm/ContainerT.h>   /* container trait */
#include <Xm/ContItemT.h>    /* container item trait */
#include <Xm/DrawingA.h>   
#include <Xm/IconH.h>   
#include <Xm/ScrollFrameT.h>
#include <Xm/XmI.h>
#include <Xm/XmTabListI.h>
#include <Exm/ContainerP.h>

/* This one should be somewhere common */
/* The Max macro returns the higher of two input arguments. */ 
#define Max(x, y) (((x) > (y)) ? (x) : (y))

extern Dimension XmTabListGetPosition(Screen *,XmTabList, Cardinal);


/* Declare static functions. */
static void ButtonHandler(Widget wid, 
			  XtPointer data, 
			  XEvent * event,
			  Boolean *cont);
static	void GetFirstColumnWidth(
				 Widget		wid,
				 int		offset,	
				 XtArgVal	*value);
static	void GetDetailTabList(
				 Widget		wid,
				 int		offset,	
				 XtArgVal	*value);
static void SelectColorDefault(
			       Widget widget,
			       int offset,
			       XrmValue *value );
static void ClassPartInitialize(
                        WidgetClass widgetClass);
static	void			Initialize(
					Widget		rw,
					Widget          nw,
					ArgList         args,
					Cardinal        *num_args);
static	void			Destroy(
					Widget  wid);
static void Layout (
                        Widget wid,
                        Widget instigator);
static void CalcSize (
                        Widget wid,
                        Widget instigator,
                        Dimension *gridWidth,
                        Dimension *gridHeight);
static Boolean NeedRelayout (
                        Widget old_w,
                        Widget new_w);

static XmTabList GetDefaultTabList(
				   ExmContainerWidget cw,
				   Cardinal detail_order_count);
static XmTabList GetDumbTabList(
				Dimension tab_size,
				Cardinal asked_num_tab);

static	void ContainerGetValues(Widget w, 
				XmContainerData containerData);

static	Dimension GetDefaultFirstColumnWidth(
					     ExmContainerWidget cw,
					     Dimension fcw_init);
static Cardinal GetDefaultDetailCount( 
				      ExmContainerWidget cw,
				      Cardinal detail_count);


/* No translations and no actions. */


static XtResource resources[] =
{
    {
	ExmNfirstColumnWidth, 
	ExmCFirstColumnWidth, 
	XmRDimension, 
	sizeof (Dimension),
	XtOffsetOf(ExmContainerRec, container.first_column_width),
	XmRImmediate, 
        (XtPointer) 0
    },
    {
	ExmNdetailOrder, 
	ExmCDetailOrder, 
	XmRCardinalList, 
	sizeof (Cardinal *),
	XtOffsetOf(ExmContainerRec, container.detail_order),
	XmRImmediate, 
        (XtPointer) NULL
    },
    {
	ExmNdetailOrderCount, 
	ExmCDetailOrderCount, 
	XmRCardinal, 
	sizeof (Cardinal),
	XtOffsetOf(ExmContainerRec, container.detail_order_count),
	XmRImmediate, 
        (XtPointer) 0
    },
    {
	ExmNdetailTabList, 
	ExmCDetailTabList, 
	XmRTabList, 
	sizeof (XmTabList),
	XtOffsetOf(ExmContainerRec, container.detail_tablist),
	XmRImmediate, 
        (XtPointer) NULL
    },
    {
	ExmNdetailHeader, 
	ExmCDetailHeader, 
	XmRXmStringTable, 
	sizeof (XmStringTable),
	XtOffsetOf(ExmContainerRec, container.detail_header),
	XmRImmediate, 
        (XtPointer) NULL
    },
    {
	ExmNdetailHeaderCount, 
	ExmCDetailHeaderCount, 
	XmRCardinal, 
	sizeof (Cardinal),
	XtOffsetOf(ExmContainerRec, container.detail_header_count),
	XmRImmediate, 
        (XtPointer) 0
    },
    {
	ExmNselectColor, 
	ExmCSelectColor, 
	XmRPixel, 
	sizeof (Pixel), 
	XtOffsetOf(ExmContainerRec, container.select_color),
	XmRImmediate, (XtPointer) SelectColorDefault
   },
   {
	ExmNlayoutType, 
	ExmCLayoutType, 
	XmRLayoutType, 
	sizeof (unsigned char), 
	XtOffsetOf(ExmContainerRec, container.layout_type),
	XmRImmediate, (XtPointer) XmDETAIL
   },
};


static XmSyntheticResource syn_resources[] = 
{
    {
	ExmNfirstColumnWidth, 
	sizeof (Dimension),
	XtOffsetOf( ExmContainerRec, container.first_column_width),
	GetFirstColumnWidth, NULL
    },
    {
	ExmNdetailTabList, 
	sizeof (XmTabList),
	XtOffsetOf( ExmContainerRec, container.detail_tablist),
	GetDetailTabList, NULL
    },
 };

/* Define the widget class record.  See Chapter 4 of the 
   "OSF/Motif Widget Writer's Guide" for details. */ 
externaldef(exmcontainerclassrec) ExmContainerClassRec exmContainerClassRec = 
{ 
  { /* core_class */
    /* superclass */                 (WidgetClass) &exmGridClassRec, 
    /* class_name */                 "ExmContainer",
    /* widget_size */                sizeof(ExmContainerRec),
    /* class_initialize */           NULL,
    /* class_part_initialize */      ClassPartInitialize, 
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    NULL,
    /* num_actions */                0,
    /* resources */                  resources,
    /* num_resources */              XtNumber(resources),
    /* xrm_class */                  NULLQUARK,
    /* compress_motion */            TRUE,
    /* compress_exposure */          XtExposeCompressMaximal,
    /* compress_enterleave */        TRUE,
    /* visible_interest */           FALSE,
    /* destroy */                    Destroy,
    /* resize */                     XtInheritResize,
    /* expose */                     XtInheritExpose,
    /* set_values */                 NULL,
    /* set_values_hook */            NULL,
    /* set_values_almost */          XtInheritSetValuesAlmost,
    /* get_values_hook */            NULL,
    /* accept_focus */               NULL,
    /* version */                    XtVersion,
    /* callback_private */           NULL,
    /* tm_table */                   XtInheritTranslations,
    /* query_geometry */             XtInheritQueryGeometry,
    /* display_accelerator */        NULL,
    /* extension */                  NULL,
  },    
  { /* composite_class */
    /* geometry_manager */           XtInheritGeometryManager,
    /* change_managed */             XtInheritChangeManaged,
    /* insert_child */               XtInheritInsertChild,
    /* delete_child */               XtInheritDeleteChild,
    /* extension */                  NULL,
  },    
  { /* constraint_class */ 
    /* constraint_resources */       NULL,
    /* constraint_num_resources */   0,
    /* constraint_size */            sizeof(ExmGridConstraintRec), 
    /* constraint_initialize */      NULL,
    /* constraint_destroy */         NULL,
    /* constraint_set_values */      NULL,
    /* extension */                  NULL,
  },    
  { /* manager class */
    /* translations */               XtInheritTranslations,
    /* syn_resources */              syn_resources,
    /* num_syn_resources */          XtNumber(syn_resources),
    /* syn_constraint_resources */   NULL,
    /* num_syn_constraint_resources */ 0,
    /* parent_process */             XmInheritParentProcess,
    /* extension */                  NULL,
  },    
  { /* grid class */
    /* layout */                     Layout,
    /* calc_size */                  CalcSize,
    /* need_relayout */              NeedRelayout,
    /* extension */                  NULL,
  },    
  { /* container class */
    /* extension */                  NULL,
  }    
};    

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */
externaldef(exmcontainerwidgetclass) WidgetClass exmContainerWidgetClass =
    (WidgetClass) &exmContainerClassRec;



/* Container Trait record for Grid */
static XmContainerTraitRec containerCT = {
  0,		/* version */
  ContainerGetValues,
};



/************************************************************************
 * GetFirstColumnWidth
 ************************************************************************/
/*ARGSUSED*/
static	void
GetFirstColumnWidth(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
    ExmContainerWidget cw = (ExmContainerWidget) wid ;
    Dimension fcw = cw->container.first_column_width ;

    if (!fcw) {
	/* get the max of icon_width + their indentation */
	fcw = GetDefaultFirstColumnWidth(cw, 0);
    }

    *value = (XtArgVal)fcw;
}


/************************************************************************
 * GetDetailTabList
 ************************************************************************/
/*ARGSUSED*/
static	void
GetDetailTabList(
	Widget		wid,
	int		offset,	/* unused */
	XtArgVal	*value)
{
    ExmContainerWidget cw = (ExmContainerWidget) wid ;
    XmTabList tab_list = cw->container.detail_tablist ;

    if (!tab_list) {
	/* get the default one */
	Cardinal detail_order_count ;

	if (cw->container.detail_order_count != 0) {
	    detail_order_count = cw->container.detail_order_count ;
	} else {
	    detail_order_count = 
		GetDefaultDetailCount(cw, 0) ;
	}
	tab_list = GetDefaultTabList(cw, detail_order_count);
	/* callers should not free this memory */
    } 

    *value = (XtArgVal)tab_list;
}


/****************************************************************************
 *
 *  SelectColorDefault.
 *
 ****************************************************************************/
static void 
SelectColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmSELECT, offset, value);
}

/****************************************************************************
 *
 *  ClassPartInitialize:
 *      Called when this widget or a subclass of this widget is instantiated.
 *
 ****************************************************************************/
static void
ClassPartInitialize (
        WidgetClass widgetClass
                    )
{
    /* Install the XmQTcontainer trait on ExmContainer 
       and all its subclasses. */
   XmeTraitSet(widgetClass, XmQTcontainer, (XtPointer) &containerCT);
}


/************************************************************************
 * Initialize
 ************************************************************************/
/*ARGSUSED*/
static	void
Initialize(
	Widget		rw,
	Widget		nw,
	ArgList		args,	/* unused */
	Cardinal	*num_args) /* unused */
{
    ExmContainerWidget cw = (ExmContainerWidget)nw;
    ExmContainerWidget rcw = (ExmContainerWidget)rw;
    Arg sargs[10] ;
    Cardinal n = 0, i ;
    Widget header_parent = nw ;
    unsigned char scrollp = XmAPPLICATION_DEFINED ;
    XmScrollFrameTrait scrollFrameTrait;

    cw->container.icon_header = NULL ;

    if (cw->container.detail_header && 
	cw->container.detail_header_count) {

	/**** first make a copy of the detail header table */
	cw->container.detail_header = (XmStringTable) 
	    XtMalloc(cw->container.detail_header_count * sizeof(XmString));

	for (i=0; i<cw->container.detail_header_count; i++)
	    cw->container.detail_header[i] = 
		XmStringCopy(rcw->container.detail_header[i]);

	/**** deal with the IconHeader creation */
	scrollFrameTrait = (XmScrollFrameTrait) 
	    XmeTraitGet((XtPointer) XtClass(XtParent(nw)), XmQTscrollFrame);

	/* check that the parent is a scrollframe already initialize,
	   which also means an automatic scrolledwindow in Motif */
	if (scrollFrameTrait &&
	    scrollFrameTrait -> getInfo (XtParent(nw), NULL, NULL, NULL)) {

	    scrollp = XmAUTOMATIC ;
	    /* use an intermediate DA so that we can have margins */
	    n = 0 ;
	    XtSetArg(sargs[n], XmNmarginHeight, 
		     cw->grid.margin_height); n++ ;
	    XtSetArg(sargs[n], XmNmarginWidth, 
		     cw->grid.margin_width); n++ ;
	    XtSetArg(sargs[n], XmNscrolledWindowChildType,
		     XmSCROLL_HOR); n++;
	    XtSetArg(sargs[n], XmNbackground,
		     nw->core.background_pixel); n++; 
	    XtSetArg(sargs[n], XmNbackgroundPixmap,
		     nw->core.background_pixmap); n++; 
	    XtSetArg(sargs[n], XmNborderWidth,
		     nw->core.border_width); n++; 
	    XtSetArg(sargs[n], XmNforeground,
		     cw->manager.foreground); n++; 
	    header_parent = XmCreateDrawingArea(XtParent(nw),
						"HeaderDA", sargs, n);
	    XtManageChild(header_parent);
	}

	n = 0 ;
	/* this is required */
	XtSetArg(sargs[n], XmNcontainerID, nw); n++ ;

	/* this is optional */
	XtSetArg(sargs[n], XmNshadowThickness, 0); n++ ;
	XtSetArg(sargs[n], XmNtraversalOn, False); n++ ;

	XtSetArg(sargs[n], XmNlabelString, 
		 cw->container.detail_header[0]); n++ ;
	if (cw->container.detail_header_count > 1) {
	    XtSetArg(sargs[n], XmNdetail, 
		     &(cw->container.detail_header[1])); n++ ;
	}
	XtSetArg(sargs[n], XmNdetailCount, 
		 cw->container.detail_header_count - 1); n++ ;

	cw->container.icon_header = XmCreateIconHeader(header_parent,
						       "Header", sargs, n);
	XtManageChild(cw->container.icon_header);

	/**** Now we have to position the Container properly in the 
	   scrolling window if there is one */
	    
	if (scrollp == XmAUTOMATIC) {
	    cw->core.y = cw->container.icon_header->core.height ;
	    /* also resize the DA to remove the bottom margin */
	    XmeConfigureObject(header_parent, 
			       header_parent->core.x,
			       header_parent->core.y,
			       cw->container.icon_header->core.width + 
			       2*cw->grid.margin_height,
			       cw->container.icon_header->core.height + 
			       cw->grid.margin_height,
			       header_parent->core.border_width);
	} else {
	    /* if not scrolling, no need to keep it, it will
	       be dealt with as a regular kid */
	    cw->container.icon_header = NULL ;
	}
    }

    /**** copy the tablist */
    if (cw->container.detail_tablist) {
	cw->container.detail_tablist = 
	    XmTabListCopy(cw->container.detail_tablist, 0, 0);
    }


    /* add an event handler to play with selection */
    XtAddEventHandler(nw, ButtonPressMask, False, ButtonHandler, NULL);
	   
}

static void
ButtonHandler(Widget wid,
	      XtPointer data,
	      XEvent * event,
	      Boolean *cont)
{
    Widget item  ;
    XmContainerItemDataRec      cItemData;
    XmContainerItemTrait        cItemTrait;

    item = XmObjectAtPoint(wid, event->xbutton.x, event->xbutton.y);
    
    if (item) {
	if ((cItemTrait = (XmContainerItemTrait)
	     XmeTraitGet((XtPointer)XtClass(item),
			 XmQTcontainerItem))) {
	    cItemData.valueMask = ContItemVisualEmphasis;
	    cItemTrait->getValues(item, &cItemData);
	    if (cItemData.visual_emphasis == XmSELECTED)
		cItemData.visual_emphasis = XmNOT_SELECTED ;
	    else 
		cItemData.visual_emphasis = XmSELECTED ;
		
	    cItemTrait->setValues(item, &cItemData);
	}	
    }
}




/************************************************************************
 * Destroy
 ************************************************************************/
static	void
Destroy(
	Widget	wid)
{
    ExmContainerWidget cw = (ExmContainerWidget)wid;
    Cardinal i ;

    if (cw->container.icon_header) {
	/* that will destroy the icon header child as well */
	XtDestroyWidget(XtParent(cw->container.icon_header));
    }

    if (cw->container.detail_header && 
	cw->container.detail_header_count) {

	for (i=0; i<cw->container.detail_header_count; i++)
	    XmStringFree(cw->container.detail_header[i]);

	XtFree((char*)cw->container.detail_header);
    }
	
    if (cw->container.detail_tablist) {
	XmTabListFree(cw->container.detail_tablist);
    }
}


/*****************************************************************************
 *
 *  NeedRelayout:
 *      need to tell if any of the resource values change should cause
 *      a relayout. Must envelop the superclass method because it is
 *      only called once by the superclass defining the method.
 *
 ****************************************************************************/
static Boolean 
NeedRelayout (
        Widget old_w,
        Widget new_w 
          )
{
    ExmContainerWidget cw = (ExmContainerWidget)old_w;
    ExmContainerWidget nw = (ExmContainerWidget)new_w;
    Boolean need_relayout ;

    /* envelop the superclass method */
    need_relayout =
	((* ((ExmGridClassRec *) exmContainerClassRec.core_class.superclass)
	 ->grid_class.need_relayout) (old_w, new_w) ||
	(nw->container.first_column_width != 
	 cw->container.first_column_width ||
	 nw->container.detail_order != cw->container.detail_order ||
	 nw->container.detail_order_count != 
	 cw->container.detail_order_count ||
	 nw->container.detail_tablist != 
	 cw->container.detail_tablist));

	    
    return (need_relayout);
}


/***************************************************************************
 *
 *  Layout:
 *     Does all the placement of children.
 *     Instigator tells whether or not to resize all children.
 *
 *************************************************************************/
static void 
Layout (
        Widget wid,
        Widget instigator
       )
{
    ExmContainerWidget cw = (ExmContainerWidget) wid ;
    Cardinal i, num_managed = 0;
    int line_count ;
    Dimension width, height, baseline_first = 0, interspace_max = 0,
              pref_height, interspace;
    int height_under_prec = 0 ;
    Dimension * baselines ;
    Position x, y ;

     /**** a first pass to compute the interspace */
    for (i = 0; i < cw->composite.num_children; i++) {
	Widget ic = cw->composite.children[i];
	XtWidgetGeometry reply;

	if (!XtIsManaged(ic)) continue ;
	num_managed ++ ;

	/* ask for preferred height and baseline to the kid */
	
	XmWidgetGetBaselines(ic, &baselines, &line_count);
	if (!line_count) continue ;

	/* store the first baseline and height under */
	if (!baseline_first) {
	    baseline_first = baselines[0];
	    /* negative so that it gives 0 below, so that it
	       doesn't count in the max */
	    height_under_prec = -baseline_first ; 
	}

	interspace = height_under_prec + baselines[0] ;
	interspace_max = Max(interspace_max, interspace);	

	/* Get child's preferred height. */
        XtQueryGeometry (ic, NULL, &reply);
	pref_height = (reply.request_mode & CWHeight) ? 
	    reply.height : ic->core.height;

	height_under_prec = pref_height - baselines[0] ;

	XtFree((char*) baselines);

    }    

    /* now layout */

     /**** a second pass to actually do the layout */
    
    /* first deal with the icon header outside the container */
    if (cw->container.icon_header &&
	XtIsManaged(cw->container.icon_header)) {
	Widget header_parent ;

	XmeConfigureObject (cw->container.icon_header, 
			    cw->grid.margin_width, 
			    cw->grid.margin_height, 
			    cw->core.width - 2*cw->grid.margin_width, 
			    cw->container.icon_header->core.height,
			    cw->container.icon_header->core.border_width);

	/* also reconfigure the parent DA */
	header_parent = XtParent(cw->container.icon_header) ;
	XmeConfigureObject(header_parent, 
			   header_parent->core.x,
			   header_parent->core.y,
			   cw->core.width,
			   cw->container.icon_header->core.height + 
			   cw->grid.margin_height,
			   header_parent->core.border_width);
    }

    y = cw->grid.margin_height ;
    for (i = 0; i < cw->composite.num_children; i++) {
	Widget ic = cw->composite.children[i];
	ExmGridConstraint glc = ExmGridCPart (ic);

	if (LayoutIsRtoLM(cw))
	    x = cw->grid.margin_width ;
	else 
	    x = cw->grid.margin_width + glc->grid_margin_width_within_cell ;
	width = cw->core.width - 2*cw->grid.margin_width -
	    glc->grid_margin_width_within_cell;
	height = ic->core.height ;
	if (ic != instigator) {
	    XtConfigureWidget (ic, x, y, width, height, ic->core.border_width);
	}
	else {
	    ic->core.x = x; 
	    ic->core.y = y; 
	    ic->core.width = width; 
	    ic->core.height = height; 
	}
	XmWidgetGetBaselines(ic, &baselines, &line_count);
	if (line_count) y += baselines[0] + interspace_max ;

	if (i != cw->composite.num_children - 1) {
	    XmWidgetGetBaselines(cw->composite.children[i+1], 
				 &baselines, &line_count);
	    if (line_count) y -= baselines[0] ;
	}
    }
}

/****************************************************************************
 *
 *  CalcSize:
 *     Calculate the ideal size of the ExmGrid widget. 
 *     Only affect the returned size if it is 0   
 *
 ***************************************************************************/
static void 
CalcSize (
        Widget wid,
        Widget instigator,
        Dimension *width_ret,
        Dimension *height_ret)
{
    /* The Width of the Container is the sum of the first_column_width
       and the detail width.
       If the first_column_width is not set, we must compute it as the
       max of all the icon_width at their position (we set the fcw 
       temporarily so that it is not recomputed when the detailwidth are
       calculated.
       The detail width is the position of the last tab if there is one
       for the last detail_order, otherwise it's the max width of all 
       icon detail width.
       The Height of the container is n-1 time the max of height_under 
       baseline of i-1 + baseline of i , + baseline 0 + h_under n-1 */
    
    ExmContainerWidget cw = (ExmContainerWidget) wid ;
    Boolean fcw_set = False ;
    Cardinal i, num_managed = 0 ;
    int line_count ;
    Dimension height, baseline_first = 0, interspace_max = 0,
              pref_height, pref_width, interspace, detail_width;
    int height_under_prec = 0 ;
    Dimension * baselines ;
    Cardinal tab_count = 0 ;

    /**** first get the first column width */
    if (!cw->container.first_column_width) {

	cw->container.first_column_width = GetDefaultFirstColumnWidth(cw, 0);

	/* we don't want to keep this calculated first_column_width in cw,
	   so set a flag to reset it to 0 at the end */
	fcw_set = True ;
    }


    /**** now get the details part */
    if (cw->container.layout_type != XmDETAIL)
	detail_width = 0 ;
    else {
	if (cw->container.detail_tablist)
	    tab_count = XmTabListTabCount(cw->container.detail_tablist) ;
	if (cw->container.detail_order_count &&
	    tab_count >= cw->container.detail_order_count) {
	    detail_width = XmTabListGetPosition(XtScreen((Widget)cw),
				cw->container.detail_tablist, tab_count-1);
	} else {
	    XtWidgetGeometry reply;

	    /* get the max width of all details */
	    if (cw->container.icon_header &&
		XtIsManaged(cw->container.icon_header)) {
		Dimension header_width ;

		/* start with header's preferred width. */
		XtQueryGeometry (cw->container.icon_header, NULL, &reply);
		header_width = (reply.request_mode & CWWidth) ? 
		    reply.width : cw->container.icon_header->core.width;
		detail_width = header_width - 
		    /* remove the horiz indent */
		    (cw->container.first_column_width 
		     - cw->container.icon_header->core.x);
	    } else
		detail_width = 0 ;

	    for (i = 0; i < cw->composite.num_children; i++) {
		Widget ic = cw->composite.children[i];
		
		/* Get child's preferred width. */
		if (ic != instigator) {
		    XtQueryGeometry (ic, NULL, &reply);
		    pref_width = (reply.request_mode & CWWidth) ? 
			reply.width : ic->core.width;
		} else {
		    pref_width = ic->core.width ;
		}

		detail_width = Max(detail_width, 
				   pref_width - 
				   /* remove the horiz indent */
				   (cw->container.first_column_width 
				    - ic->core.x));
	    }
	}
    }

    /**** compute the ideal height */
    for (i = 0; i < cw->composite.num_children; i++) {
	Widget ic = cw->composite.children[i];
	XtWidgetGeometry reply;

	if (!XtIsManaged(ic)) continue ;
	num_managed ++ ;

	/* ask for preferred height and baseline to the kid */
	
	XmWidgetGetBaselines(ic, &baselines, &line_count);
	if (!line_count) continue ;

	/* store the first baseline and height under */
	if (!baseline_first) {
	    baseline_first = baselines[0];
	    /* negative so that it gives 0 below, so that it
	       doesn't count in the max */
	    height_under_prec = -baseline_first ; 
	}

	interspace = height_under_prec + baselines[0] ;
	interspace_max = Max(interspace_max, interspace);	

	/* Get child's preferred height. */
        XtQueryGeometry (ic, NULL, &reply);
	pref_height = (reply.request_mode & CWHeight) ? 
	    reply.height : ic->core.height;

	height_under_prec = pref_height - baselines[0] ;

	XtFree((char*) baselines);

    }    

    /* now sum up everything */
    height = baseline_first + interspace_max * (num_managed-1) +
	height_under_prec ;

    /*** do not set non null dimensions */
    if (*width_ret == 0) *width_ret = 
	cw->container.first_column_width + detail_width +
	    cw->grid.margin_width ;
    if (*height_ret == 0) *height_ret = height + 
	2*cw->grid.margin_height; 


    /*** set it back to 0, a getvalueshook will provide the real
       information */
    if (fcw_set) cw->container.first_column_width =  0 ;
}


/************************************************************************
 * GetDefaultFirstColumnWidth
 ************************************************************************/
/*ARGSUSED*/
static	Dimension
GetDefaultFirstColumnWidth(
			   ExmContainerWidget cw,
			   Dimension first_column_width)
{
    Dimension width ;
    Cardinal i ;
    XmContainerItemDataRec cont_item_data ;
    XmContainerItemTrait cont_item_trait ;

    /* Get the max icon width + indent of all managed kids */
    
    /* deal with icon_header outside container first */
    if (cw->container.icon_header &&
	XtIsManaged(cw->container.icon_header)) {

	/* start with header's icon width. */
	cont_item_trait = (XmContainerItemTrait) 
	    XmeTraitGet((XtPointer) XtClass(cw->container.icon_header), 
			XmQTcontainerItem) ;

	cont_item_data.valueMask = ContItemIconWidth ;
	cont_item_trait->getValues(cw->container.icon_header, 
				   &cont_item_data);
	first_column_width = Max(first_column_width, 
				 cw->grid.margin_width +
				 cont_item_data.icon_width) ;
    }

    for (i = 0; i < cw->composite.num_children; i++) {
	Widget ic = cw->composite.children[i];
	ExmGridConstraint glc = ExmGridCPart (ic);

	if (!XtIsManaged(ic)) continue ;
	
	/* if the child has the ContItem trait, use its icon_width,
	   othersize, use it's width */

	cont_item_trait = (XmContainerItemTrait) 
	    XmeTraitGet((XtPointer) XtClass(ic), XmQTcontainerItem) ;

	if (cont_item_trait) {
	    cont_item_data.valueMask = ContItemIconWidth ;
	    cont_item_trait->getValues(ic, &cont_item_data);
	    width = cont_item_data.icon_width ;
	} else {
	    width = ic->core.width ;
	}

	/* the inner cell margin width plays the outline
	   indentation role in this example widget */
	first_column_width = 
	    Max (first_column_width, 
		 cw->grid.margin_width + width 
		 + glc->grid_margin_width_within_cell);
    } 
	
    return first_column_width ;
}


/************************************************************************
 * GetDefaultDetailCount
 ************************************************************************/
/*ARGSUSED*/
static	Cardinal
GetDefaultDetailCount(
		      ExmContainerWidget cw,
		      Cardinal detail_count)
{
    Cardinal i ;
    XmContainerItemDataRec cont_item_data ;
    XmContainerItemTrait cont_item_trait ;

    /* Get the max detail_count of all managed kids */
    
    /* deal with icon_header outside container first */
    if (cw->container.icon_header &&
	XtIsManaged(cw->container.icon_header)) {

	/* start with header's detail_count */
	cont_item_trait = (XmContainerItemTrait) 
	    XmeTraitGet((XtPointer) XtClass(cw->container.icon_header), 
			XmQTcontainerItem) ;

	/* icon headert always has the trait */
	cont_item_data.valueMask = ContItemDetailCount ;
	cont_item_trait->getValues(cw->container.icon_header, 
				   &cont_item_data);
	detail_count = Max(detail_count, cont_item_data.detail_count) ;
    }

    for (i = 0; i < cw->composite.num_children; i++) {
	Widget ic = cw->composite.children[i];

	if (!XtIsManaged(ic)) continue ;
	
	/* if the child has the ContItem trait, use its detail_count */

	cont_item_trait = (XmContainerItemTrait) 
	    XmeTraitGet((XtPointer) XtClass(ic), XmQTcontainerItem) ;

	if (cont_item_trait) {
	    cont_item_data.valueMask = ContItemDetailCount ;
	    cont_item_trait->getValues(ic, &cont_item_data);
	    
	    detail_count = 
		Max (detail_count, cont_item_data.detail_count);
	} 
    }
	
    return detail_count ;
}



/****************************************************************************
 *
 *  GetDefaultTabList  
 *   (XmNwidth-2*XmNmarginWidth-XmNfirstColumnWidth)/XmNdetailOrderCount
 *
 ***************************************************************************/
static XmTabList 
GetDefaultTabList(
        ExmContainerWidget cw,
	Cardinal detail_order_count)
{
    XmTabList default_tablist = NULL ;

    /* if the Container has a valid size, a default tablist of
       size detail_order_count shall be returned in detail_tablist.  
       The tabs in this tablist shall be equal to:

	   (XmNwidth-2*XmNmarginWidth-XmNfirstColumnWidth)
	               /XmNdetailOrderCount                        

       If XmNwidth is not yet valid, a NULL detail_tablist 
       is returned, so that the icon can concat all its details 
       and get a reasonable size. */

    if (cw->core.width) {
	Dimension tab_size = 0 ;
	    
	if ((cw->core.width > (2*cw->grid.margin_width +
			       cw->container.first_column_width)) &&
	    detail_order_count) {
	    tab_size = (cw->core.width - 2*cw->grid.margin_width -
			cw->container.first_column_width) 
		/ detail_order_count ;
	    default_tablist = GetDumbTabList(tab_size,
						detail_order_count);
	} /* else return NULL as well */
    }
    
    return default_tablist ;
}


/****************************************************************************
 *
 *  GetDumbTabList  
 *    fixed space - lazy allocation .do not deallocate the returned value
 *
 ***************************************************************************/
static XmTabList 
GetDumbTabList(
        Dimension tab_size,
        Cardinal asked_num_tab)
{
    static int Num_tab = 0 ;
    static XmTab * Tab_pool = NULL ;
    static XmTabList Tab_list = NULL ;
    Cardinal i, prev_num_tab = Num_tab ;
    
    if (Num_tab < asked_num_tab) {
	Num_tab = asked_num_tab ;
	Tab_pool = (XmTab*) XtRealloc((char*)Tab_pool,
				      Num_tab*sizeof(XmTab));
    }

    /* create more tabs */
    for (i=prev_num_tab; i<Num_tab; i++)
	Tab_pool[i] =  XmTabCreate(0.0, XmPIXELS, XmABSOLUTE,
				   XmALIGNMENT_BEGINNING, XmS);

    /* update the values */
    for (i=0; i<asked_num_tab; i++)
	XmTabSetValue(Tab_pool[i], (float)tab_size*(i+1));

    /* free the previous tablist and return a new one.
       Not clear what is going to happen to the Tab_pool array,
       is it going to be copied or ref counted, but I won't free them... */
    if (Tab_list) XmTabListFree(Tab_list);
    Tab_list = XmTabListInsertTabs(NULL, Tab_pool, asked_num_tab, 0);

    return Tab_list ;
}



/*-- Trait methods --*/

/************************************************************************
 * ContainerGetValues
 * 
 ************************************************************************/
static	void
ContainerGetValues(Widget w, 
		  XmContainerData containerData)
{

    ExmContainerWidget cw = (ExmContainerWidget) w ;
    Boolean fcw_set = False ;

    /**** fill in detail_order */
    if (cw->container.layout_type == XmDETAIL) {
	if (cw->container.detail_order_count != 0) {
	    containerData->detail_order_count = 
		cw->container.detail_order_count ;
	    containerData->detail_order = cw->container.detail_order ;
	} else {
	    /* take the max detail count and return NULL for the
	       detail_order, so that the icon takes a default order */
	    if (containerData->valueMask & ContDetailOrder) {
		containerData->detail_order_count = 
		    GetDefaultDetailCount(cw, 0);
		containerData->detail_order = NULL ;
	    }
	}
    } else {
        containerData->detail_order_count = 0 ;
	containerData->detail_order = NULL ;
    }
    
    /**** if the resource firstColumnWidth is set, use it, otherwise,
          default it */

    if (cw->container.layout_type == XmDETAIL) {
	if (cw->container.first_column_width)
	    containerData->first_column_width = 
		cw->container.first_column_width ;
	else 
	    /* don't bother computing it if not asked */
	    if (containerData->valueMask & ContFirstColumnWidth) {
		/* set the container resource temporarily */
		cw->container.first_column_width =
		    containerData->first_column_width = 
			GetDefaultFirstColumnWidth(cw,
					containerData->first_column_width) ;
		fcw_set = True ;
	    }
    } else if (cw->container.layout_type == XmSPATIAL)
	containerData->first_column_width = 0;

    /**** if detail_tablist has been set, use it, else compute a default */
    if (cw->container.detail_tablist)
	containerData->detail_tablist = cw->container.detail_tablist ;
    else 
	/* go find a default only if needed */
    if (containerData->valueMask & ContDetailTabList) {
	containerData->detail_tablist = GetDefaultTabList(cw, 
					   containerData->detail_order_count);
    } 

    /**** get a value for this one - kinda random, just to test */
    if (cw->manager.navigation_type == XmTAB_GROUP)
	containerData->selection_mode = XmNORMAL_MODE ;
    else 
	containerData->selection_mode = XmADD_MODE ;

	
    /**** just to try out the trait protocol */
    if (cw->container.select_color == WhitePixelOfScreen(XtScreen(w)))
	containerData->valueMask &= ~ContSelectColor ;
    else
	containerData->select_color = cw->container.select_color ;

	
    /*** set first_column_width back to 0, a getvalueshook 
      will provided the real
      information */
    if (fcw_set) cw->container.first_column_width =  0 ;

}




/*****************************************************************************
 *
 *  ExmCreateContainer:
 *      Called by an application. 
 *
 ****************************************************************************/
Widget 
ExmCreateContainer (
        Widget parent,
        char *name,
        ArgList arglist, 
        Cardinal argcount
              )
{
 /* This is a convenience function to instantiate an ExmContainer widget. */
   return (XtCreateWidget (name, exmContainerWidgetClass, parent, 
                           arglist, argcount));
}

