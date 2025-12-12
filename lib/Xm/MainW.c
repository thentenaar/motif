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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: MainW.c /main/20 1996/10/17 15:21:07 cde-osf $"
#endif
#endif

#include "XmI.h"
#include <Xm/SeparatoGP.h>  /* just access the position/dimension fields,*/
#include <Xm/ScrollBarP.h>  /*   could live without that if needed */
#include <Xm/MainWP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/MenuT.h>
#include <Xm/TraitP.h>
#include "MessagesI.h"
#include "RepTypeI.h"
#include "GeoUtilsI.h"

#define MWMessage1	_XmMMsgMainW_0000
#define MWMessage2	_XmMMsgMainW_0001

#define ExistManaged( wid)      (wid && XtIsManaged( wid))

#define DEFAULT_SIZE 50

/********    Static Function Declarations    ********/

static void ClassPartInitialize(
                        WidgetClass wc) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void DeleteChild(
                        Widget w) ;
static void InsertChild(
                        Widget w) ;
static void Layout(
                        XmMainWindowWidget mw) ;
static void Resize(
                        Widget wid) ;
static void GetSize(
                        XmMainWindowWidget mw,
	                Dimension *pwidth,
                        Dimension *pheight) ;
static XtGeometryResult GeometryManager(
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged(
                        Widget wid) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetVertRects(
			Widget sw,
			XRectangle ** vrect,
			Cardinal * num_vrect);

static void CheckKids(
			XmMainWindowWidget mw);

/********    End Static Function Declarations    ********/




/************************************************************************
 *									*
 * Main Window Resources						*
 *									*
 ************************************************************************/

static XtResource resources[] =
{
    {
	XmNcommandWindow, XmCCommandWindow, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmMainWindowRec, mwindow.CommandWindow),
	XmRImmediate, NULL
    },
    {
	XmNcommandWindowLocation, XmCCommandWindowLocation,
        XmRCommandWindowLocation, sizeof(unsigned char),
        XtOffsetOf(XmMainWindowRec, mwindow.CommandLoc),
	XmRImmediate, (XtPointer) XmCOMMAND_ABOVE_WORKSPACE
    },
    {
	XmNmenuBar, XmCMenuBar, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmMainWindowRec, mwindow.MenuBar),
	XmRImmediate, NULL
    },
    {
	XmNmessageWindow, XmCMessageWindow, XmRWidget, sizeof(Widget),
        XtOffsetOf(XmMainWindowRec, mwindow.Message),
	XmRImmediate, NULL
    },
    {
        XmNmainWindowMarginWidth, XmCMainWindowMarginWidth,
        XmRHorizontalDimension, sizeof (Dimension),
        XtOffsetOf(XmMainWindowRec, mwindow.margin_width),
	XmRImmediate, (XtPointer) 0
    },
    {
        XmNmainWindowMarginHeight, XmCMainWindowMarginHeight,
        XmRVerticalDimension, sizeof (Dimension),
        XtOffsetOf(XmMainWindowRec, mwindow.margin_height),
	XmRImmediate, (XtPointer) 0
    },
    {
	XmNshowSeparator, XmCShowSeparator, XmRBoolean, sizeof(Boolean),
        XtOffsetOf(XmMainWindowRec, mwindow.ShowSep),
	XmRImmediate, (XtPointer) False
    }
};

/****************
 *
 * Resolution independent resources
 *
 ****************/

static XmSyntheticResource syn_resources[] =
{
   { XmNmainWindowMarginWidth,
     sizeof (Dimension),
     XtOffsetOf(XmMainWindowRec, mwindow.margin_width),
     XmeFromHorizontalPixels, XmeToHorizontalPixels },

   { XmNmainWindowMarginHeight,
     sizeof (Dimension),
     XtOffsetOf(XmMainWindowRec, mwindow.margin_height),
     XmeFromVerticalPixels, XmeToVerticalPixels },

};

/*******************************************/
/*  Declaration of class extension records */
/*******************************************/

static XmScrolledWindowClassExtRec scrolled_windowClassExtRec = {
    NULL,
    NULLQUARK,
    XmScrolledWindowClassExtVersion,
    sizeof(XmScrolledWindowClassExtRec),
    NULL,                              /* inherit get_hor_rects */
    GetVertRects,                      /* overide get_vert_rects */
};


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

externaldef(xmmainwindowclassrec) XmMainWindowClassRec xmMainWindowClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &xmScrolledWindowClassRec,
    /* class_name         */    "XmMainWindow",
    /* widget_size        */    sizeof(XmMainWindowRec),
    /* class_initialize   */    NULL,
    /* class_partinit     */    ClassPartInitialize,
    /* class_inited       */	False,
    /* initialize         */    Initialize,
    /* Init hook	  */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	True,
    /* compress_exposure  */	XtExposeCompressSeries,
    /* compress_enterleave*/	True,
    /* visible_interest   */    False,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    SetValues,
    /* set values hook    */    NULL,
    /* set values almost  */    XtInheritSetValuesAlmost,
    /* get values hook    */    NULL,
    /* accept_focus       */    NULL,
    /* Version            */    XtVersion,
    /* PRIVATE cb list    */    NULL,
    /* tm_table		  */    XtInheritTranslations,
    /* query_geometry     */    NULL,
    /* display_accelerator*/    NULL,
    /* extension          */    NULL,
  },
  {
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	InsertChild,
    /* delete_child	  */	DeleteChild,
    /* Extension          */    NULL,
  },{
/* Constraint class Init */
    NULL,
    0,
    sizeof (XmScrolledWindowConstraintRec),
    NULL,
    NULL,
    NULL,
    NULL

  },
/* Manager Class */
   {
      XtInheritTranslations,    		/* translations        */
      syn_resources,				/* get resources      	  */
      XtNumber(syn_resources),			/* num get_resources 	  */
      NULL,					/* get_cont_resources     */
      0,					/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           */
   },

 {
/* Scrolled Window class */
    (XtPointer) &scrolled_windowClassExtRec,    /* auto drag extension */
 },
 {
/* Main Window class - just the extension pointer */
     /* extension */            (XtPointer) NULL
 }
};

externaldef(xmmainwindowwidgetclass) WidgetClass
             xmMainWindowWidgetClass = (WidgetClass)&xmMainWindowClassRec;




/************************************************************************
 *									*
 *  ClassPartInitialize - Set up the fast subclassing.			*
 *									*
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
   _XmFastSubclassInit (wc, XmMAIN_WINDOW_BIT);
}


/************************************************************************
 *									*
 *  Initialize								*
 *									*
 ************************************************************************/
/* ARGSUSED */
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    XmMainWindowWidget new_w = (XmMainWindowWidget) nw ;
    int   n;
    Arg loc_args[20];


    /* First, undo our superclass defaulting to a real size in
       AUTOMATIC mode, because MainWindow can build a real size
       out of its children in AUTOMATIC */
    if (new_w->swindow.ScrollPolicy == XmAUTOMATIC) {
	if ((rw->core.width == 0) && new_w->core.width)
	    new_w->core.width = 0 ;
	if ((rw->core.height == 0) && new_w->core.height)
	    new_w->core.height = 0 ;
    }

    if (!XmRepTypeValidValue(XmRID_COMMAND_WINDOW_LOCATION,
			    new_w->mwindow.CommandLoc, nw))
        new_w->mwindow.CommandLoc = XmCOMMAND_ABOVE_WORKSPACE;

    n = 0;
    XtSetArg (loc_args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg (loc_args[n], XmNscrolledWindowChildType, XmSEPARATOR); n++;

    new_w->mwindow.ManagingSep = True ;

    new_w->mwindow.Sep1 = (XmSeparatorGadget)
	XtCreateManagedWidget("Separator1", xmSeparatorGadgetClass,
			      nw, loc_args, n);
    new_w->mwindow.Sep2 = (XmSeparatorGadget)
	XtCreateManagedWidget("Separator2", xmSeparatorGadgetClass,
			      nw, loc_args, n);
    new_w->mwindow.Sep3 = (XmSeparatorGadget)
	XtCreateManagedWidget("Separator3", xmSeparatorGadgetClass,
			      nw, loc_args, n);

    new_w->mwindow.ManagingSep = False ;

    /* override the SW setting here */
    new_w->swindow.XOffset = new_w->mwindow.margin_width;
    new_w->swindow.YOffset = new_w->mwindow.margin_height;
    new_w->swindow.WidthPad = new_w->mwindow.margin_width;
    new_w->swindow.HeightPad = new_w->mwindow.margin_height;
}


/************************************************************************
 *									*
 *  DeleteChild								*
 *									*
 ************************************************************************/
/* ARGSUSED */
static void
DeleteChild(
        Widget child)
{
    XmMainWindowWidget mw = (XmMainWindowWidget) XtParent(child);
    CompositeWidgetClass superclass = (CompositeWidgetClass)
	                xmMainWindowClassRec.core_class.superclass ;
    XtWidgetProc      delete_child;

    /* update our own internals first */
    if (child == mw->mwindow.CommandWindow)
        mw->mwindow.CommandWindow = NULL;
    if (child == mw->mwindow.MenuBar)
        mw->mwindow.MenuBar = NULL;
    if (child == mw->mwindow.Message)
        mw->mwindow.Message = NULL;

    _XmProcessLock();
    delete_child = superclass->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)(child);
}

static void
CheckKids(
        XmMainWindowWidget mw )
{

    /* do a sanity check */
    if( mw->swindow.WorkWindow != NULL &&
       mw->swindow.WorkWindow->core.being_destroyed ) {
	mw->swindow.WorkWindow = NULL;
    }
    if( mw->swindow.hScrollBar != NULL &&
       mw->swindow.hScrollBar->core.being_destroyed ) {
	mw->swindow.hScrollBar = NULL;
    }
    if( mw->swindow.vScrollBar != NULL &&
       mw->swindow.vScrollBar->core.being_destroyed ) {
	mw->swindow.vScrollBar = NULL;
    }
    if( mw->mwindow.CommandWindow != NULL &&
       mw->mwindow.CommandWindow->core.being_destroyed ) {
	mw->mwindow.CommandWindow = NULL;
    }
    if( mw->mwindow.MenuBar != NULL &&
       mw->mwindow.MenuBar->core.being_destroyed ) {
	mw->mwindow.MenuBar = NULL;
    }
    if( mw->mwindow.Message != NULL &&
       mw->mwindow.Message->core.being_destroyed ) {
	mw->mwindow.Message = NULL;
    }
}



/************************************************************************
 *									*
 *  InsertChild								*
 *									*
 ************************************************************************/
static void
InsertChild(
        Widget w )
{
    CompositeWidgetClass superclass = (CompositeWidgetClass)
	                xmMainWindowClassRec.core_class.superclass ;
    XmMainWindowWidget   mw = (XmMainWindowWidget ) w->core.parent;
    XmScrolledWindowConstraint nc = GetSWConstraint(w);
    XtWidgetProc insert_child;

    if (!XtIsRectObj(w)) return;


    /* Try to guess the nature of the child_type .
       If we're lucky, fine, otherwise, something bad might happens: the
       scrolledwindow can take it as a workwindow and possibly reparents
       it to the clipwindow.
       In the absence of a set childType constraint resource set,
       there is not much we can do to avoid the problem */
    /* Note: auto created Separator were already labelled in Initialize */

    if (nc->child_type == (unsigned char) RESOURCE_DEFAULT) {
	XmMenuSystemTrait menuSTrait;

	if ((menuSTrait = (XmMenuSystemTrait)
	     XmeTraitGet ((XtPointer) XtClass(w), XmQTmenuSystem)) != NULL) {
	    if (menuSTrait->type(w) == XmMENU_BAR &&
		!mw->mwindow.MenuBar) {
		/* If it's a menubar, and we don't have one yet, use it. */
		nc->child_type = XmMENU_BAR ;
	    }
	}  else

	if (XmIsCommandBox(w)) {
	    if (!mw->mwindow.CommandWindow)   {
		/* If it's a command, and we don't have one, get it */
		nc->child_type = XmCOMMAND_WINDOW ;
	    }
	} else

	    /* new in 2.0 */
	if (XmIsMessageBox(w)) {
	    if (!mw->mwindow.Message)   {
		nc->child_type = XmMESSAGE_WINDOW ;
	    }
	}
    }

    if (nc->child_type == XmMENU_BAR) {
	mw->mwindow.MenuBar = w;
    } else
    if (nc->child_type == XmCOMMAND_WINDOW) {
	mw->mwindow.CommandWindow = w;
    } else
    if (nc->child_type == XmMESSAGE_WINDOW) {
	mw->mwindow.Message = w;
    }

    /* call ScrolledWindow InsertChild directly, since it does nothing
       to the MainWindow known childType */
    _XmProcessLock();
    insert_child = superclass->composite_class.insert_child;
    _XmProcessUnlock();
    (*insert_child)(w);

}


/************************************************************************
 *									*
 * Layout - Layout the main window.					*
 *                                                                      *
 *									*
 ************************************************************************/
static void
Layout(
        XmMainWindowWidget mw )
{
    Position mbx,mby, cwx,cwy, swy, mwx, mwy, sepy, sep2y = 0;
    Dimension mbwidth, mbheight, cwwidth= 0, cwheight;
    Dimension MyXpad, MyYpad, mwwidth, mwheight;
    Dimension	bw = 0, sep2h, sep3h;
    XtWidgetGeometry  desired, preferred;
    int tmp ; /* used for checking negative Dimension value */


    CheckKids(mw);

/****************
 *
 * Query the kids - and we have definite preferences as to their sizes.
 * The Menubar gets top billing - we tell it it how wide it is going to be ,
 * and let it have whatever height it wants. The command box gets to stay
 * it's current height, but has to go to the new width. The scrolled window
 * gets the leftovers.
 *
 ****************/
    MyXpad = mw->mwindow.margin_width;
    MyYpad = mw->mwindow.margin_height;

    mw->swindow.XOffset = MyXpad;
    mw->swindow.YOffset = MyYpad;
    mw->swindow.HeightPad = mw->mwindow.margin_height;
    mw->swindow.WidthPad = mw->mwindow.margin_width;

    cwx = MyXpad;
    cwy = swy = MyYpad;
    mw->mwindow.ManagingSep = True;
    if (ExistManaged(mw->mwindow.MenuBar))
    {
	bw = mw->mwindow.MenuBar->core.border_width;
	mbx = MyXpad;
	mby = MyYpad;
	tmp = mw->core.width - (2 * (MyXpad + bw));
	if (tmp <= 0) mbwidth = 10; else  mbwidth = tmp ;
	mbheight = mw->mwindow.MenuBar->core.height;

	desired.x = mbx;
	desired.y = mby;
	desired.border_width = bw;
        desired.width = mbwidth;
        desired.height = mbheight;
        desired.request_mode = (CWWidth);
        if (XtQueryGeometry(mw->mwindow.MenuBar, &desired, &preferred)
	    != XtGeometryYes) {
   	    bw = preferred.border_width;
	    mbheight = preferred.height;
        }
        XmeConfigureObject(mw->mwindow.MenuBar, mbx, mby,
			   mbwidth, mbheight,bw);

        if (mw->mwindow.ShowSep)
        {
	    XtManageChild((Widget) mw->mwindow.Sep1);
            XmeConfigureObject( (Widget) mw->mwindow.Sep1, 0,
			       mby + mbheight + (2 * bw),
	        	       mw->core.width,
			       mw->mwindow.Sep1->rectangle.height, 0);
            cwy = swy = mw->mwindow.Sep1->rectangle.y +
		mw->mwindow.Sep1->rectangle.height ;
        }
        else
        {
            XtUnmanageChild((Widget) mw->mwindow.Sep1);
            cwy = swy = mby + mbheight + (2 * bw);
        }
    }
    else
    {
	XtUnmanageChild((Widget) mw->mwindow.Sep1);
    }

    if (ExistManaged(mw->mwindow.CommandWindow))
    {
        bw = mw->mwindow.CommandWindow->core.border_width;
	tmp = mw->core.width - (2 * (MyXpad + bw));
	if (tmp <= 0) cwwidth = 10; else cwwidth = tmp ;
	cwheight = mw->mwindow.CommandWindow->core.height;

	desired.x = cwx;
	desired.y = cwy;
	desired.border_width = bw;
        desired.width = cwwidth;
        desired.height = cwheight;
        desired.request_mode = (CWWidth);
        if (XtQueryGeometry(mw->mwindow.CommandWindow, &desired, &preferred)
            != XtGeometryYes)
        {
   	    bw = preferred.border_width;
	    cwheight = preferred.height;
        }

        if ((cwheight + cwy + (2 * bw)) > (mw->core.height - MyYpad)) {
	    tmp = mw->core.height - (2 * bw) - MyYpad - cwy;
	    if (tmp <= 0) cwheight = 10 ; else cwheight = tmp;
	}

        if (mw->mwindow.ShowSep)
            sep2h = mw->mwindow.Sep2->rectangle.height;
        else
            sep2h = 0;

        sep2y = (cwy +  cwheight) + 2 * bw;
        swy = sep2y + sep2h ;

        if (mw->mwindow.CommandLoc == XmCOMMAND_BELOW_WORKSPACE)
        {
            mby = swy;
            sep2y = cwy + (mw->core.height - swy - MyYpad);
            swy = cwy;
            mw->swindow.HeightPad = sep2h + cwheight
		+ mw->mwindow.margin_height;
            if (mw->mwindow.ShowSep)
                cwy = sep2y + mw->mwindow.Sep2->rectangle.height;
            else
                cwy = sep2y;
        }
    }
    else
    {
	XtUnmanageChild((Widget) mw->mwindow.Sep2);
        sep2h = 0;
        cwheight = 0;
    }

    if (ExistManaged(mw->mwindow.Message))
    {
        bw = mw->mwindow.Message->core.border_width;
	mwx = MyXpad;
	tmp = mw->core.width - (2 * (MyXpad + bw));
	if (tmp <= 0) mwwidth = 10 ; else mwwidth = tmp ;
	mwheight = mw->mwindow.Message->core.height;

	desired.x = mwx;
	desired.y = swy;
	desired.border_width = bw;
        desired.width = mwwidth;
        desired.height = mwheight;
        desired.request_mode = (CWWidth);
        if (XtQueryGeometry(mw->mwindow.Message, &desired, &preferred)
            != XtGeometryYes)
        {
   	    bw = preferred.border_width;
	    mwheight = preferred.height;
        }
        if (mw->mwindow.ShowSep)
            sep3h = mw->mwindow.Sep3->rectangle.height;
        else
            sep3h = 0;

        sepy = mw->core.height - mwheight - (2 * bw) -
	    mw->mwindow.margin_height - sep3h;
        mwy = sepy + sep3h;

        if (mw->mwindow.CommandLoc == XmCOMMAND_BELOW_WORKSPACE)
        {
            mw->swindow.HeightPad = sep2h + cwheight + sep3h + mwheight
		+ mw->mwindow.margin_height;
            sep2y -= (sep3h + mwheight);
            cwy -= (sep3h + mwheight);
        }
        else
            mw->swindow.HeightPad = sep3h + mwheight
		+ mw->mwindow.margin_height;

        XmeConfigureObject(mw->mwindow.Message, mwx, mwy,
			   mwwidth, mwheight, bw);
        if (mw->mwindow.ShowSep)
        {
	    XtManageChild((Widget) mw->mwindow.Sep3);
            XmeConfigureObject( (Widget) mw->mwindow.Sep3,
			       0, sepy, mw->core.width,
                               mw->mwindow.Sep3->rectangle.height, 0);
        }
        else
            XtUnmanageChild((Widget) mw->mwindow.Sep3);
    }
    else
    {
	XtUnmanageChild((Widget) mw->mwindow.Sep3);
    }

    if (ExistManaged(mw->mwindow.CommandWindow))
    {
        XmeConfigureObject( mw->mwindow.CommandWindow,
			   cwx, cwy, cwwidth, cwheight, bw);
        if (mw->mwindow.ShowSep)
        {
	    XtManageChild((Widget) mw->mwindow.Sep2);
            XmeConfigureObject((Widget) mw->mwindow.Sep2,
			       0, sep2y, mw->core.width,
                               mw->mwindow.Sep2->rectangle.height, 0);
        }
        else
            XtUnmanageChild((Widget) mw->mwindow.Sep2);
    }

    mw->swindow.YOffset = swy;
    mw->mwindow.ManagingSep = False;
}

/************************************************************************
 *                                                                      *
 *  Relayout the main window.				*
 *									*
 ************************************************************************/
static void
Resize(
        Widget wid )
{
    CompositeWidgetClass superclass = (CompositeWidgetClass)
	                xmMainWindowClassRec.core_class.superclass ;
    XtWidgetProc resize;

    Layout((XmMainWindowWidget) wid);

    /* call our superclass layout now that MainWindow has updated
       some internal positional fields: offset, pads */
    _XmProcessLock();
    resize = superclass->core_class.resize;
    _XmProcessUnlock();
    (*resize)(wid);
}



/************************************************************************
 *									*
 * GetSize - compute the size of the Main window to enclose all the	*
 * visible widgets.							*
 *									*
 ************************************************************************/
static void
GetSize(
        XmMainWindowWidget mw,
        Dimension *pwidth,
        Dimension *pheight)
{
    Dimension	    newWidth,newHeight;
    XmScrollBarWidget	hsb = mw->swindow.hScrollBar,
                        vsb = mw->swindow.vScrollBar;
    Widget 	    w;
    Dimension	    hsheight = 0, vmwidth = 0,
		    ht = mw->manager.shadow_thickness  * 2,
		    hsbht = 0, vsbht = 0;
    Dimension	    width, MyXpad, MyYpad;
    XtWidgetGeometry  preferred;


    MyXpad = mw->mwindow.margin_width * 2;
    MyYpad = mw->mwindow.margin_height * 2;


    /* what id to use for the sw frame */
   if (mw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED)
        w = mw->swindow.WorkWindow;
    else
        w = (Widget)mw->swindow.ClipWindow;


    /* note: first time through, all relevant values are 0, but we need to
    ** take account of the preferred size anyway
    */
    if (ExistManaged((Widget) vsb) &&
        ((0 == mw->core.width) || ((Dimension)vsb->core.x < mw->core.width)))  /* needed */
    {
       	vsbht = 2 * vsb->primitive.highlight_thickness;
	vmwidth = vsb->core.width + mw->swindow.pad +
	          (2 * vsb->primitive.highlight_thickness);
    }

    if (ExistManaged((Widget) hsb) &&
        ((0 == mw->core.height) || ((Dimension)hsb->core.y < mw->core.height)))  /* needed */
    {
       	hsbht = 2 * hsb->primitive.highlight_thickness;
	hsheight = hsb->core.height + mw->swindow.pad +
		   (2 * hsb->primitive.highlight_thickness);
    }

/****************
 *
 * Use the work window as the basis for our height. If the mode is
 * constant, and we are not realized, use the areawidth and areaheight
 * variables instead of the clipwindow width and height, since they are a
 * match for the workspace until the swindow is realized.
 *
 ****************/

    if (ExistManaged(w))
    {
        if ((mw->swindow.ScrollPolicy == XmAUTOMATIC) &&
	    !XtIsRealized((Widget)mw))
	{
  	    newWidth = mw->swindow.AreaWidth + (w->core.border_width * 2) +
		       hsbht + vmwidth + ht + MyXpad;
            newHeight = mw->swindow.AreaHeight + (w->core.border_width * 2) +
		        vsbht + hsheight + ht + MyYpad;
        }
	else
	{
            XtQueryGeometry(w, NULL, &preferred);
	    newWidth = preferred.width + (w->core.border_width * 2) +
		       hsbht + vmwidth + ht + MyXpad;
            newHeight = preferred.height  + (w->core.border_width * 2) +
		        vsbht + hsheight + ht + MyYpad;
	}
    }
    else
    {
	newWidth = MyXpad;
        newHeight = MyYpad;
    }

    /* Take the max width, add the height of the other kids */

    if (ExistManaged(mw->mwindow.CommandWindow))
    {
        XtQueryGeometry(mw->mwindow.CommandWindow, NULL, &preferred);
        width = preferred.width + MyXpad +
	        (2 * mw->mwindow.CommandWindow->core.border_width);
    	if (newWidth < width) newWidth = width;
	newHeight += preferred.height +
  	            (2 * mw->mwindow.CommandWindow->core.border_width);
        if (mw->mwindow.Sep2 && mw->mwindow.ShowSep)
	    newHeight += mw->mwindow.Sep2->rectangle.height;

    }

    if (ExistManaged(mw->mwindow.MenuBar))
    {
        XtQueryGeometry(mw->mwindow.MenuBar, NULL, &preferred);
        width = preferred.width + MyXpad +
	        (2 * mw->mwindow.MenuBar->core.border_width);
    	if (newWidth < width) newWidth = width;
	newHeight += preferred.height +
  	            (2 * mw->mwindow.MenuBar->core.border_width);
        if (mw->mwindow.Sep1  && mw->mwindow.ShowSep)
	    newHeight += mw->mwindow.Sep1->rectangle.height;
    }

    if (ExistManaged(mw->mwindow.Message))
    {
        XtQueryGeometry(mw->mwindow.Message, NULL, &preferred);
        width = preferred.width +  MyXpad +
	        (2 * mw->mwindow.Message->core.border_width);
    	if (newWidth < width) newWidth = width;
	newHeight += preferred.height +
  	            (2 * mw->mwindow.Message->core.border_width);
        if (mw->mwindow.Sep3 && mw->mwindow.ShowSep)
	    newHeight += mw->mwindow.Sep3->rectangle.height;

    }

    if (!*pwidth) *pwidth = newWidth ;
    if (!*pheight) *pheight = newHeight ;

    /* might still be null */
    if (!*pwidth) *pwidth = DEFAULT_SIZE ;
    if (!*pheight) *pheight = DEFAULT_SIZE ;

}

/************************************************************************
 *									*
 *  GeometryManager							*
 *									*
 ************************************************************************/
static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
    CompositeWidgetClass superclass = (CompositeWidgetClass)
	                xmMainWindowClassRec.core_class.superclass ;
    XmMainWindowWidget mw = (XmMainWindowWidget ) w->core.parent;
    XtGeometryResult res;
    Dimension	    newWidth,newHeight, OldHeight;
    Dimension	    bw;
    XtWidgetGeometry  parent_request ;
    XtWidgetGeometry  desired, preferred;
    XtWidgetProc resize;


    CheckKids(mw);

/****************
 *
 * If it's not a mainwindow kid, let the scrolled window deal with it.
 * If it's from the workwindow, and the width changed, resize the menubar
 * and ask for a new height so my layout routine doesn't clip the workwindow.
 *
 ****************/
    if (w != mw->mwindow.MenuBar &&
        w != mw->mwindow.Message &&
        w != mw->mwindow.CommandWindow &&
        w != (Widget )mw->mwindow.Sep1 &&
        w != (Widget) mw->mwindow.Sep2 &&
        w != (Widget) mw->mwindow.Sep3) {

	/* this is the only case of geometry manager enveloping that
	   I know of in Motif */

	XtGeometryHandler geo_mgr;
	_XmProcessLock();
	geo_mgr = superclass->composite_class.geometry_manager;
	_XmProcessUnlock();

        res = (*geo_mgr)(w, request, reply);

        if (res == XtGeometryYes) {

	    Widget mb = mw->mwindow.MenuBar;

	    if ((w == mw->swindow.WorkWindow) &&
                (request->request_mode & CWWidth) &&
                mb && XtIsManaged(mb)) {
                desired.x = mb->core.x;
	        desired.y = mb->core.y;
	        desired.border_width = mb->core.border_width;
                desired.width = mw->core.width -
                                (2 * mw->mwindow.margin_width);
                desired.height = mb->core.height;
                desired.request_mode = (CWWidth);
                XtQueryGeometry(mw->mwindow.MenuBar, &desired, &preferred);
                if (preferred.height != mb->core.height) {
                    parent_request.request_mode = CWWidth | CWHeight;
		    if (request->request_mode & XtCWQueryOnly)
			parent_request.request_mode |= XtCWQueryOnly;
		    parent_request.width = mw->core.width ;
		    parent_request.height = newHeight = mw->core.height -
			(mb->core.height - (2 * mb->core.border_width)) +
			    preferred.height + (2 *preferred.border_width);
                    if (XtMakeGeometryRequest((Widget) mw,
					      &parent_request, NULL)
                        == XtGeometryYes) {
			if (!(request->request_mode & XtCWQueryOnly))
			    XmeConfigureObject(mw->mwindow.MenuBar,
					       mb->core.x, mb->core.y,
					       preferred.width, preferred.height,
					       preferred.border_width);
			else return XtGeometryYes ;
		    }
		}
	    }
	    _XmProcessLock();
	    resize = XtCoreProc(mw, resize);
	    _XmProcessUnlock();
	    (*resize)((Widget)mw) ;
	}
	return(res);
    }

    /** Disallow any X or Y changes for MainW children **/
    if ((request -> request_mode & CWX || request -> request_mode & CWY))
	return(XtGeometryNo);


    if(request->request_mode & CWBorderWidth)
	bw = request->border_width;
    else
        bw = w->core.border_width;

    if (request->request_mode & CWWidth)
	newWidth = request->width + 2 * (bw + mw->mwindow.margin_width);
    else
        newWidth = mw->core.width ;

    /* grow only in width */
    if (newWidth <= mw->core.width) newWidth = mw->core.width;

/****************
*
 * Margins are already included in the old width & height
 *
 ****************/
     if(request->request_mode & CWHeight)
         newHeight = mw->core.height -
	             (w->core.height - (2 * w->core.border_width)) +
	    	     request->height + 2 * bw;
    else
         newHeight = mw->core.height;

    OldHeight = mw->core.height;

    parent_request.request_mode = CWWidth | CWHeight;
    if (request->request_mode & XtCWQueryOnly)
	parent_request.request_mode |= XtCWQueryOnly;
    parent_request.width = newWidth ;
    parent_request.height = newHeight;

    res = XtMakeGeometryRequest((Widget) mw, &parent_request, NULL) ;

    if (res == XtGeometryYes) {
	if (!(request->request_mode & XtCWQueryOnly)) {
	    if(request->request_mode & CWWidth)
		w->core.width = request->width;
	    if(request->request_mode & CWHeight)
		w->core.height = request->height;

	    mw->swindow.YOffset = mw->swindow.YOffset +
		(newHeight - OldHeight);

	    _XmProcessLock();
	    resize = XtCoreProc(mw, resize);
	    _XmProcessUnlock();
	    (*resize) ((Widget)mw) ;
	}
    }

    return(res);
}



/************************************************************************
 *									*
 *  ChangeManaged - called whenever there is a change in the managed	*
 *		    set.						*
 *									*
 ************************************************************************/
static void
ChangeManaged(
        Widget wid )
{
    XmMainWindowWidget mw = (XmMainWindowWidget) wid ;
    XtWidgetGeometry desired ;
    CompositeWidget cw = (CompositeWidget) mw->swindow.ClipWindow;
    Widget   w;
    register int i;
    XtWidgetProc resize;

    if (mw->mwindow.ManagingSep || mw->swindow.FromResize) return;

    CheckKids(mw);

/****************
 *
 * This is an ugly bit of work... It's possible for the clip window to get
 * "extra" kids that really want to be mainwindow widgets.
 *
 ****************/
    if ((mw->swindow.ScrollPolicy == XmAUTOMATIC) &&
        (cw->composite.num_children > 1) &&
	(mw->swindow.WorkWindow != NULL)) {

	/* loop over the clip window child list and treat the bogus */
	for (i = 0; i < cw->composite.num_children; i++) {
	    XmScrolledWindowConstraint swc ;
	    int j ;

	    w = cw->composite.children[i];
	    swc = GetSWConstraint(w);

	    /* only those kind are allowed as clipwindow kids */
	    if ((swc->child_type != XmWORK_AREA) &&
		(swc->child_type != XmSCROLL_HOR) &&
		(swc->child_type != XmSCROLL_VERT) &&
		(swc->child_type != XmNO_SCROLL)) {

		/* add it to the main window child list. first increase
		   the list if needed- Gee, I wish I remember what made
		   me keep this hacky code around... */
		if (mw->composite.num_children ==
		    mw->composite.num_slots)  {
		    mw->composite.num_slots +=  (mw->composite.num_slots
						 / 2) + 2;
		    mw->composite.children = (WidgetList) XtRealloc(
					(char *) mw->composite.children,
					(unsigned) (mw->composite.num_slots)
						* sizeof(Widget));
		}
		mw->composite.children[mw->composite.num_children++] = w;
		w->core.parent = (Widget) mw;

		/* remove it from the clipwindow child list by
		   moving all the siblings that comes after it
		   one slot down */
		for (j = i+1; j < cw->composite.num_children; j++) {
		    cw->composite.children[i-1] = cw->composite.children[i] ;
		}
		cw->composite.num_children -- ;
	    }
	}
   }

    if (!XtIsRealized(wid))  {
	desired.width = XtWidth(wid) ;   /* might be 0 */
	desired.height = XtHeight(wid) ; /* might be 0 */
    } else {
	desired.width = 0 ;
	desired.height = 0 ;
    }

    GetSize(mw, &desired.width, &desired.height);
    desired.request_mode = (CWWidth | CWHeight);
    (void) _XmMakeGeometryRequest(wid, &desired);

    _XmProcessLock();
    resize = XtCoreProc(mw, resize);
    _XmProcessUnlock();
    (*resize) (wid) ;

    XmeNavigChangeManaged(wid);
}


/************************************************************************
 *									*
 *  SetValues								*
 *									*
 ************************************************************************/
/*ARGSUSED*/
static Boolean
SetValues(
        Widget cw,
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmMainWindowWidget current = (XmMainWindowWidget) cw ;
    XmMainWindowWidget new_w = (XmMainWindowWidget) nw ;
    Boolean flag = False;

    CheckKids(new_w);

    /* somehow, this used not to create problem in 1.2,
       some apps did setvalue of XmNmenubar to itself ?
       check that and change back */
    if ((new_w->mwindow.MenuBar != current->mwindow.MenuBar) &&
        (new_w->mwindow.MenuBar == nw)) {
	new_w->mwindow.MenuBar = current->mwindow.MenuBar;
    }

    /* fix for 8990: these warnings must be here for bc... */
    if ((new_w->mwindow.MenuBar != current->mwindow.MenuBar) &&
        (new_w->mwindow.MenuBar == NULL)) {
        XmeWarning( (Widget) new_w, MWMessage1);
	new_w->mwindow.MenuBar = current->mwindow.MenuBar;
    }

    if ((new_w->mwindow.CommandWindow != current->mwindow.CommandWindow) &&
        (new_w->mwindow.CommandWindow == NULL)) {
        XmeWarning( (Widget) new_w, MWMessage2);
	new_w->mwindow.CommandWindow = current->mwindow.CommandWindow;
    }

    /* first deal with the layout attributes, and set up a flag */

    /* There is a potential bug here: if the change in margin
       concur with a change on some other stuff, like separator
       or a new child, and the getSize call return the same size,
       no resize call will be generated by Xt.
       A way to fix that is to check this no change in size
       and to fake a request.. maybe not worth. */

    if ((new_w->mwindow.margin_width != current->mwindow.margin_width) ||
	(new_w->mwindow.margin_height != current->mwindow.margin_height) ||
	(new_w->mwindow.ShowSep != current->mwindow.ShowSep)) {
	flag = True;
    }

    if ((new_w->mwindow.CommandLoc != current->mwindow.CommandLoc) &&
        (XmRepTypeValidValue(XmRID_COMMAND_WINDOW_LOCATION,
			     new_w->mwindow.CommandLoc, (Widget) new_w))) {
        XtWidgetProc resize;
	_XmProcessLock();
	resize = XtCoreProc(nw, resize);
	_XmProcessUnlock();
	(*resize) (nw) ;
    }
    else
        new_w->mwindow.CommandLoc = current->mwindow.CommandLoc;


    /* At InsertChild time, a lot of bad things might have happened.
       The command window, messagewindow and work window, which we have
       no real way to identify at that time, might have been mixed up.
       (MenuBar and application ScrollBars shouldn't be a problem)
       The first unidentifiable kid will take the workwindow slot (and
       possibly be reparented), and the followers will be just inserted
       in the child list without reparenting (to the clipwindow in
       AUTO mode I mean).
       After creation time, the application will use XtSetValues to set up
       things correctly (except changing the workwindow in AUTO,
       which is not allowed).
       The requirement, if a childType resource isn't provided,
       is that the workwindow be created first, at least in AUTO mode
       where the reparenting happens */



    if ((new_w->mwindow.MenuBar != current->mwindow.MenuBar) ||
        (new_w->mwindow.Message != current->mwindow.Message) ||
        (new_w->mwindow.CommandWindow != current->mwindow.CommandWindow ) ||
        (new_w->swindow.hScrollBar != current->swindow.hScrollBar) ||
	(new_w->swindow.vScrollBar != current->swindow.vScrollBar) ||
	(new_w->swindow.WorkWindow != current->swindow.WorkWindow ) ||
        (flag)) {
	/* set our core geometry to the needed size -
	   no resizePolicy here...
	   Change in child type can happen before realize time, before
	   change managed has been called, and we don't want to set up
	   a size for the main window at this point, since its children
	   size haven't been set up yet */
	if (XtIsRealized((Widget)new_w)) {
	    Dimension width = 0, height = 0 ;
	    GetSize (new_w, &width, &height);
	    new_w->core.width = width ;
	    new_w->core.height = height ;
	}
    }

    return (False);
}


/************************************************************************
 *									*
 * GetAutoDragVertRects	class methods					*
 *									*
 ************************************************************************/

static void
GetVertRects(
	     Widget sw,
	     XRectangle ** vrect,
	     Cardinal * num_vrect)
{
    Widget w ;
    XmMainWindowWidget mw = (XmMainWindowWidget) sw ;

    *num_vrect = 2 ;
    *vrect = (XRectangle *) XtMalloc(sizeof(XRectangle) * (*num_vrect)) ;

    /* The vertical rectangles are the ones that vertically auto scroll,
       they are defined by areas on the top and bottom of the
       workarea, e.g. the margins, the spacing, the scrollbars
       and the shadows */

    /* Both rects are computed using only the relative work_area or
       clipwindow (in AUTO) location within the scrolled window:
       this is the area between the widget and its parent frame.

       Then they need to be translated into the scrollbar coord system. */

    /* what id to use for the sw child frame */
   if (mw->swindow.ScrollPolicy == XmAPPLICATION_DEFINED) {
       w = mw->swindow.WorkWindow;
       if (!w) w = sw ; /* fallback */
   } else
        w = (Widget) mw->swindow.ClipWindow;


    /* the vertical rectangle are more complex to compute than for
       the SW case because we cannot go blindy to the SW boundary,
       we have to stop at the next sibling window boundary */

    /* We have to consider all case of existing/managed menubar, command,
       or message area, with the command up or down too. */

    if (!ExistManaged(mw->mwindow.MenuBar) &&
	!ExistManaged(mw->mwindow.CommandWindow)) {
	(*vrect)[0].y = 0 ;
	(*vrect)[0].height = w->core.y ;
    } else
    if (ExistManaged(mw->mwindow.MenuBar) &&
	!ExistManaged(mw->mwindow.CommandWindow)) {
	(*vrect)[0].y = mw->mwindow.MenuBar->core.y +
	    mw->mwindow.MenuBar->core.height ;
	(*vrect)[0].height = w->core.y - mw->mwindow.MenuBar->core.y -
	    mw->mwindow.MenuBar->core.height ;
    } else
    if (ExistManaged(mw->mwindow.MenuBar) &&
	ExistManaged(mw->mwindow.CommandWindow) &&
	(mw->mwindow.CommandLoc == XmCOMMAND_ABOVE_WORKSPACE)) {
	(*vrect)[0].y = mw->mwindow.CommandWindow->core.y +
	    mw->mwindow.CommandWindow->core.height ;
	(*vrect)[0].height = w->core.y - mw->mwindow.CommandWindow->core.y -
	    mw->mwindow.CommandWindow->core.height ;
    }


    /* The first rectangle is the one that makes the scrollbar goes up */
    (*vrect)[0].x = w->core.x - mw->swindow.vScrollBar->core.x ;
    /* just translate to the scrollbar coordinate */
    (*vrect)[0].y -= mw->swindow.vScrollBar->core.y ;
    (*vrect)[0].width = w->core.width ;

    /* The second rectangle is the one that makes the scrollbar goes down */

    (*vrect)[1].x = (*vrect)[0].x ;
    (*vrect)[1].y = w->core.y + w->core.height
	- mw->swindow.vScrollBar->core.y ;
    (*vrect)[1].width = (*vrect)[0].width ;

    if (!ExistManaged(mw->mwindow.CommandWindow) &&
	!ExistManaged(mw->mwindow.Message)) {
	(*vrect)[1].height = mw->core.height - (*vrect)[1].y ;
    } else
    if (ExistManaged(mw->mwindow.CommandWindow) &&
	(mw->mwindow.CommandLoc == XmCOMMAND_BELOW_WORKSPACE)) {
	(*vrect)[1].height = mw->mwindow.CommandWindow->core.y -
	    w->core.y - w->core.height;
    } else
    if (ExistManaged(mw->mwindow.Message)) {
	(*vrect)[1].height = mw->mwindow.Message->core.y -
	    w->core.y - w->core.height;
    }

}

/************************************************************************
 *									*
 * Public API Functions							*
 *									*
 ************************************************************************/

/************************************************************************
 *									*
 * XmMainWindowSetAreas - set a new children set.				*
 *	-to be deprecated in favor of XtSetValues			*
 *      -doesn't even handle message window                             *
 *									*
 ************************************************************************/
void
XmMainWindowSetAreas(
        Widget w,
        Widget menu,
        Widget command,
        Widget hscroll,
        Widget vscroll,
        Widget wregion )
{
     Arg args[5] ;
     Cardinal    n;

     n = 0;
     if (menu) {
	 XtSetArg (args[n], XmNmenuBar, menu); n++;
     }
     if (command) {
	 XtSetArg (args[n], XmNcommandWindow, command); n++;
     }
     if (hscroll) {
	 XtSetArg (args[n], XmNhorizontalScrollBar, hscroll); n++;
     }
     if (vscroll) {
	 XtSetArg (args[n], XmNverticalScrollBar, vscroll); n++;
     }
     if (wregion) {
	 XtSetArg (args[n], XmNworkWindow, wregion); n++;
     }
     XtSetValues(w, args, n);
}


/************************************************************************
 *									*
 * XmMainWindowSep1, 2 and 3                                            *
 *   - return the id of the top seperator widget.	                *
 *   - to be deprecated in favor of using XtNameToWidget               *
 *									*
 *									*
 ************************************************************************/
Widget
XmMainWindowSep1(
        Widget w )
{
    XmMainWindowWidget   mw = (XmMainWindowWidget) w;
    Widget separator;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    separator =  (Widget) mw->mwindow.Sep1;
    _XmAppUnlock(app);

    return separator;
}

Widget
XmMainWindowSep2(
        Widget w )
{
    XmMainWindowWidget   mw = (XmMainWindowWidget) w;
    Widget separator;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    separator = (Widget) mw->mwindow.Sep2;
    _XmAppUnlock(app);

    return separator;
}


Widget
XmMainWindowSep3(
        Widget w )
{
    XmMainWindowWidget   mw = (XmMainWindowWidget) w;
    Widget separator;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    separator = (Widget) mw->mwindow.Sep3;
    _XmAppUnlock(app);

    return separator;
}


/************************************************************************
 *									*
 * XmCreateMainWindow -                                         	*
 *									*
 ************************************************************************/
Widget
XmCreateMainWindow(
        Widget parent,
        char *name,
        ArgList args,
        Cardinal argCount )
{
    return (XtCreateWidget(name, xmMainWindowWidgetClass, parent,
			     args, argCount ) );
}

Widget
XmVaCreateMainWindow(
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
                         xmMainWindowWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedMainWindow(
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
                         xmMainWindowWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}
