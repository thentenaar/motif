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
static char rcsid[] = "$TOG: CascadeB.c /main/27 1999/08/11 14:26:35 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"		/* ShellP.h doesn't define externalref. */
#include <X11/ShellP.h>
#include <X11/keysymdef.h>
#include <Xm/BaseClassP.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuStateP.h>
#include <Xm/MenuT.h>
#include <Xm/RowColumnP.h>
#include <Xm/TearOffP.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "CascadeBI.h"
#include "CascadeBGI.h"
#include "LabelI.h"
#include "GadgetUtiI.h"
#include "MenuStateI.h"
#include "MenuProcI.h"
#include "MenuUtilI.h"
#include "MessagesI.h"
#include "PrimitiveI.h"
#include "RCMenuI.h"
#include "TearOffI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"

#define CASCADE_PIX_SPACE    4	 /* pixels between label and bit map */
#define MAP_DELAY_DEFAULT   180
#define EVENTS              ((unsigned int) (ButtonPressMask | \
			     ButtonReleaseMask | EnterWindowMask | \
			     LeaveWindowMask))


#define WRONGPARENT	_XmMMsgCascadeB_0000
#define WRONGSUBMENU	_XmMMsgCascadeB_0001
#define WRONGMAPDELAY	_XmMMsgCascadeB_0002


/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void BorderHighlight(
                        Widget wid) ;
static void BorderUnhighlight(
                        Widget wid) ;
static void DrawShadow(
                        register XmCascadeButtonWidget cb) ;
static void DrawCascade(
                        register XmCascadeButtonWidget cb) ;
static void Redisplay(
                        register Widget cb,
                        XEvent *event,
                        Region region) ;
static void Arm(
                        register XmCascadeButtonWidget cb) ;
static void ArmAndPost(
                        XmCascadeButtonWidget cb,
                        XEvent *event) ;
static void ArmAndActivate(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void Disarm(
                        register XmCascadeButtonWidget cb,
                        Boolean unpost) ;
static void PostTimeout(
                        XtPointer closure,
                        XtIntervalId *id) ;
static void DelayedArm(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void CheckDisarm(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void StartDrag(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void Select(
                        register XmCascadeButtonWidget cb,
                        XEvent *event,
                        Boolean doCascade) ;
static void DoSelect(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void KeySelect(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void MenuBarSelect(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void MenuBarEnter(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void MenuBarLeave(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void CleanupMenuBar(
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
static void PopdownGrandChildren(
                        XmRowColumnWidget rowcol) ;
static void Cascading(
                        Widget w,
                        XEvent *event) ;
static void Popup(
                        Widget cb,
                        XEvent *event) ;
static void size_cascade(
                        XmCascadeButtonWidget cascadebtn) ;
static void position_cascade(
                        XmCascadeButtonWidget cascadebtn) ;
static void setup_cascade(
                        XmCascadeButtonWidget cascadebtn,
                        Boolean adjustWidth,
                        Boolean adjustHeight) ;
static void Destroy(
                        Widget wid) ;
static void Resize(
                        Widget cb) ;
static Boolean SetValuesPrehook(
			Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
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
static void GetArmGC(
                        XmCascadeButtonWidget cb) ;
static void GetBackgroundGC(
                        XmCascadeButtonWidget cb) ;
static void Initialize(
                        Widget w_req,
                        Widget w_new,
                        ArgList args,
                        Cardinal *num_args) ;

/********    End Static Function Declarations    ********/


/*
 * event translation tables for cascadebutton.  There are different
 * ones for the different menus a cascadebutton widget can appear in
 */

static XtTranslations menubar_events_parsed;

#define menubar_events	_XmCascadeB_menubar_events

static XtTranslations p_events_parsed;

#define p_events 	_XmCascadeB_p_events

static XtActionsRec action_table [] =
{
    {"DelayedArm",	DelayedArm},
    {"CheckDisarm",	CheckDisarm},
    {"StartDrag",	StartDrag},
    {"DoSelect", 	DoSelect},
    {"KeySelect",	KeySelect},
    {"MenuBarSelect",	MenuBarSelect},
    {"MenuBarEnter",	MenuBarEnter},
    {"MenuBarLeave",	MenuBarLeave},
    {"MenuButtonTakeFocus", _XmMenuButtonTakeFocus },
    {"MenuButtonTakeFocusUp", _XmMenuButtonTakeFocusUp },
    {"CleanupMenuBar",	CleanupMenuBar},
    {"Help",		_XmCBHelp},
};


static XtResource resources[] =
{
    {	XmNactivateCallback,
	XmCCallback,
	XmRCallback,
	sizeof (XtCallbackList),
	XtOffsetOf( struct _XmCascadeButtonRec, cascade_button.activate_callback),
	XmRCallback,
	NULL
    },
    {	XmNcascadingCallback,
	XmCCallback,
	XmRCallback,
	sizeof (XtCallbackList),
	XtOffsetOf( struct _XmCascadeButtonRec, cascade_button.cascade_callback),
	XmRCallback,
	NULL
    },
    {	XmNsubMenuId,
	XmCMenuWidget,				/* submenu */
	XmRMenuWidget,
	sizeof (Widget),
	XtOffsetOf( struct _XmCascadeButtonRec, cascade_button.submenu),
	XmRMenuWidget,
	(XtPointer) NULL
    },
    {	XmNcascadePixmap,
	XmCPixmap,
	XmRDynamicPixmap,
	sizeof(Pixmap),
	XtOffsetOf( struct _XmCascadeButtonRec, cascade_button.cascade_pixmap),
	XmRImmediate,
	(XtPointer) XmUNSPECIFIED_PIXMAP
    },
    {   XmNmappingDelay,
	XmCMappingDelay,
	XmRInt,
	sizeof (int),
	XtOffsetOf( struct _XmCascadeButtonRec, cascade_button.map_delay),
	XmRImmediate,
	(XtPointer) MAP_DELAY_DEFAULT
    },
    {
        XmNshadowThickness,
        XmCShadowThickness,
        XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmCascadeButtonRec, primitive.shadow_thickness),
        XmRCallProc,
        (XtPointer) _XmSetThickness
    },
    {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf( struct _XmPrimitiveRec, primitive.traversal_on),
	XmRImmediate,
	(XtPointer) True
    },
    {
        XmNhighlightThickness,
        XmCHighlightThickness,
        XmRHorizontalDimension,
        sizeof (Dimension),
        XtOffsetOf( struct _XmPrimitiveRec, primitive.highlight_thickness),
        XmRCallProc,
        (XtPointer) _XmSetThickness
    },
    {
 	XmNmarginWidth,
	XmCMarginWidth,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _XmLabelRec, label.margin_width),
	XmRImmediate,
	(XtPointer) XmINVALID_DIMENSION
    },
};

/*
 * static initialization of the cascade button widget class record,
 * must do each field
 */
static XmBaseClassExtRec       cascadeBBaseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    InitializePrehook,                        /* initialize prehook   */
    SetValuesPrehook,			      /* set_values prehook   */
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
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};


externaldef(xmcascadebuttonclassrec) XmCascadeButtonClassRec xmCascadeButtonClassRec =
{
    {			/* core class record */
	(WidgetClass) &xmLabelClassRec,		/* superclass ptr	*/
	"XmCascadeButton",			/* class_name	*/
	sizeof (XmCascadeButtonWidgetRec),	/* size of Pulldown widget */
	ClassInitialize,			/* class init proc */
	ClassPartInitialize,			/* chained class init */
	FALSE,					/* class is not init'ed */
	Initialize,				/* widget init proc */
	NULL,					/* init_hook proc */
    	XtInheritRealize,       		/* widget realize proc */
    	action_table,				/* class action table */
    	XtNumber (action_table),
	resources,				/* this class's resource list*/
	XtNumber (resources),			/*  "	  " resource_count */
    	NULLQUARK,				/* xrm_class	        */
    	TRUE,					/* compress motion */
    	XtExposeCompressMaximal,		/* compress exposure */
	TRUE,					/* compress enter-leave */
   	FALSE,					/* no VisibilityNotify */
	Destroy,				/* class destroy proc */
	Resize,					/* class resize proc */
	Redisplay,				/* expose proc */
	SetValues,				/* set_value proc */
	NULL,					/* set_value_hook proc */
	XtInheritSetValuesAlmost,		/* set_value_almost proc */
	NULL,					/* get_values_hook */
	NULL,					/* class accept focus proc */
	XtVersion,				/* current version */
    	NULL,					/* callback offset list */
    	NULL,					/* default translation table */
						  /* this is manually set */
	XtInheritQueryGeometry,			/* query geo proc */
	NULL,				        /* display accelerator*/
	(XtPointer)&cascadeBBaseClassExtRec,	/* extension */
    },
	{
			/* Primitive Class record */
	BorderHighlight,			/* border_highlight */
	BorderUnhighlight,			/* border_uhighlight */
	XtInheritTranslations,		        /* translations */
	ArmAndActivate,				/* arm & activate */
	NULL,					/* get resources */
	0,					/* num get_resources */
	NULL,	                                /* extension */
	},
    {			/* Label Class record */
	XmInheritWidgetProc,		        /* set override callback */
	XmInheritMenuProc,		        /* menu procedures       */
	XtInheritTranslations,		        /* menu traversal xlation */
	NULL,					/* extension */
    },
    {			/* cascade_button class record */
        NULL,					/* extension */
    }
};

/*
 * now make a public symbol that points to this class record
 */

externaldef(xmcascadebuttonwidgetclass) WidgetClass xmCascadeButtonWidgetClass = (WidgetClass) &xmCascadeButtonClassRec;

/* Menu Savvy trait record */
static XmMenuSavvyTraitRec MenuSavvyRecord = {
    /* version: */
    -1,
    NULL,
    NULL,
    NULL,
    _XmCBNameActivate,
};


/*
 * parse the translation tables for the different menutypes
 */
static void
ClassInitialize( void )
{
    menubar_events_parsed  = XtParseTranslationTable (menubar_events);
    p_events_parsed	   = XtParseTranslationTable (p_events);

   /* set up base class extension quark */
   cascadeBBaseClassExtRec.record_type = XmQmotif;
}

/*
 * set up fast subclassing
 */

static void
ClassPartInitialize(
        WidgetClass wc )
{
  _XmFastSubclassInit (wc, XmCASCADE_BUTTON_BIT);

  /* Install the menu savvy trait record,  copying fields from XmLabel */
  _XmLabelCloneMenuSavvy (wc, &MenuSavvyRecord);
}


/*
 * The button is armed (does not pop up submenus).
 */
static void
BorderHighlight(
        Widget wid )
{
      Arm ((XmCascadeButtonWidget) wid);
}


/*
 * The button is disarmed (does not pop down submenus).
 */
static void
BorderUnhighlight(
        Widget wid )
{
    XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid;
    Boolean popdown;

    if (Lab_IsMenupane(cb) &&
	(((XmManagerWidget)XtParent(cb))->manager.active_child == wid) &&
	CB_Submenu(cb))
    {
	XmMenuShellWidget mshell =
	    (XmMenuShellWidget) XtParent(CB_Submenu(cb));

	if ((mshell->composite.children[0] == CB_Submenu(cb)) &&
	    (XmIsMenuShell(mshell)) &&
	    (mshell->shell.popped_up))
	{
	    popdown = True;
	}
	else
	    popdown = False;
    }
    else
	popdown = False;

    Disarm ((XmCascadeButtonWidget) wid, popdown);
}


/*
 * Draw the 3D shadow around the widget if its is armed.
 */
static void
DrawShadow(
        register XmCascadeButtonWidget cb )
{
  XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay((Widget) cb));
  Boolean etched_in = dpy -> display.enable_etched_in_menu;

  if (CB_IsArmed(cb))
    {
      if (XtIsRealized((Widget)cb))
      {
	 XmeDrawShadows (XtDisplay (cb), XtWindow (cb),
			cb->primitive.top_shadow_GC,
			cb->primitive.bottom_shadow_GC,
			cb->primitive.highlight_thickness,
			cb->primitive.highlight_thickness,
			cb->core.width - 2 *
			cb->primitive.highlight_thickness,
			cb->core.height - 2 *
			cb->primitive.highlight_thickness,
			cb->primitive.shadow_thickness,
			etched_in ? XmSHADOW_IN : XmSHADOW_OUT);
      }
   }
}


static void
DrawCascade(
        register XmCascadeButtonWidget cb )
{
   if ((CB_HasCascade(cb)) && (CB_Cascade_width(cb) != 0))
   {
      Pixmap pixmap ;
      int depth ;

      pixmap = CB_IsArmed(cb) &&
	  (CB_ArmedPixmap(cb) != XmUNSPECIFIED_PIXMAP) ?
	      CB_ArmedPixmap(cb) : CB_CascadePixmap(cb) ;

      XmeGetPixmapData(XtScreen(cb),
		       pixmap,
		       NULL,
		       &depth,
		       NULL, NULL,
		       NULL, NULL,
		       NULL, NULL);

      if (depth == cb->core.depth)
	  XCopyArea (XtDisplay(cb),
		     pixmap,
		     XtWindow(cb),
		     cb->label.normal_GC, 0, 0,
		     CB_Cascade_width(cb), CB_Cascade_height(cb),
		     CB_Cascade_x(cb), CB_Cascade_y(cb));
      else
      if (depth == 1)
	  XCopyPlane (XtDisplay(cb),
		      pixmap,
		      XtWindow(cb),
		      cb->label.normal_GC, 0, 0,
		      CB_Cascade_width(cb), CB_Cascade_height(cb),
		      CB_Cascade_x(cb), CB_Cascade_y(cb), 1);

  }
}

/*
 * redisplay the widget
 */
static void
Redisplay(
        register Widget cb,
        XEvent *event,
        Region region )
{
    Pixel tmpc;

    if (XtIsRealized (cb))
    {
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
	GC tmpGC = NULL;
	XtExposeProc expose;

	if (etched_in) {
		if (CB_IsArmed(cb))
		    XFillRectangle(XtDisplay(cb), XtWindow(cb), CB_ArmGC(cb),
				   0, 0, cb->core.width, cb->core.height);
		else
	        XClearArea(XtDisplay(cb), XtWindow(cb),
				0, 0, cb->core.width, cb->core.height, False);
#if USE_XFT
	} else if (Lab_MenuType(cb) != XmWORK_AREA) { /* adeed with XFT support */
        XClearArea(XtDisplay(cb), XtWindow(cb),
			0, 0, cb->core.width, cb->core.height, False);
#endif
	}
	if (etched_in && CB_IsArmed(cb)) {
	    Pixel junk, select_pix;
	    Boolean replaceGC = False;

	    XmGetColors(XtScreen(cb), cb->core.colormap,
			cb->core.background_pixel,
			&junk, &junk, &junk, &select_pix);

	    if (select_pix ==
		((XmCascadeButtonWidget)cb)->primitive.foreground) {
		replaceGC = True;
		tmpGC = ((XmCascadeButtonWidget)cb)->label.normal_GC;
		((XmCascadeButtonWidget)cb)->label.normal_GC =
		    CB_BackgroundGC(cb);
	    }

        tmpc = cb->core.background_pixel;
        XSetWindowBackground(XtDisplay(cb), XtWindow(cb), select_pix);

	    _XmProcessLock();
	    expose = xmLabelClassRec.core_class.expose;
	    _XmProcessUnlock();
	    (*expose)((Widget) cb, event, region);

	    XSetWindowBackground(XtDisplay(cb), XtWindow(cb), tmpc);
	    if (cb->core.background_pixmap != XmUNSPECIFIED_PIXMAP)
	    	XSetWindowBackgroundPixmap(XtDisplay(cb), XtWindow(cb),
	    			cb->core.background_pixmap);

	    if (replaceGC)
		((XmCascadeButtonWidget)cb)->label.normal_GC = tmpGC;
	}
	else {

	    /* Label class does most of the work */

	    _XmProcessLock();
	    expose = xmLabelClassRec.core_class.expose;
	    _XmProcessUnlock();
	    (*expose)(cb, event, region);
	}

	DrawCascade((XmCascadeButtonWidget) cb);
	DrawShadow ((XmCascadeButtonWidget) cb);
    }
}


/*
 * Arming the cascadebutton consists of setting the armed bit
 * and drawing the 3D shadow.
 */
static void
Arm(
        register XmCascadeButtonWidget cb )
{
  if (!CB_IsArmed(cb))
    {
      XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
      Boolean etched_in = dpy->display.enable_etched_in_menu;

      CB_SetArmed(cb, TRUE);

      if (etched_in)
	  Redisplay((Widget) cb, NULL, NULL);
      else {
	  DrawCascade(cb);
	  DrawShadow (cb);
      }
    }

  XmProcessTraversal((Widget) cb, XmTRAVERSE_CURRENT);
}



/*
 * Post the submenu and then arm the button.  The arming is done
 * second so that the post can be quickly as possible.
 */
static void
ArmAndPost(
        XmCascadeButtonWidget cb,
        XEvent *event )
{
   if (!CB_IsArmed(cb))
   {
      _XmCascadingPopup ((Widget) cb, event, TRUE);
      Arm (cb);
   }
}




/*
 * class function to cause the cascade button to be armed and selected
 */
/*ARGSUSED*/
static void
ArmAndActivate(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
    XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(cb);
    XmMenuSystemTrait menuSTrait;
    Time _time;

    /* check if event has been processed  - if it's NULL, override processing */
    if (event && !_XmIsEventUnique(event))
       return;

    _time = _XmGetDefaultTime(wid, event);

    menuSTrait = (XmMenuSystemTrait)
      XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);

    if (menuSTrait == NULL) {
      /* We're in trouble.  This isn't a valid menu that we're
	 in and Arm and Activate has been called.  Abort ! */
      return;
    }

    switch (Lab_MenuType (cb))
    {
       case XmMENU_BAR:
       {
          ShellWidget myShell = NULL;

          /* Shared menupanes require some additional checks */
          if (CB_Submenu(cb))
              myShell = (ShellWidget)XtParent(CB_Submenu(cb));

          if (myShell &&
	      XmIsMenuShell(myShell) &&         /* not torn ?! */
	      (myShell->shell.popped_up) &&
	      (myShell->composite.children[0] == CB_Submenu(cb)) &&
	      (cb == (XmCascadeButtonWidget)RC_CascadeBtn(CB_Submenu(cb))))
          {
	     menuSTrait -> popdown((Widget) parent, event);
	     Disarm (cb, FALSE);
	  }
          else
          {
             /* call the cascading callbacks first thing */
             Cascading ((Widget) cb, event);

	     /*
	      * check if the traversing flag is set true.  This indicates
	      * that we are in a traverse and don't want to activate if
	      * there is no submenu attached.  Set during KDown in menubar.
	      */
	     if (CB_Traversing(cb) && !CB_Submenu(cb))
		 return;

             if (! RC_IsArmed (parent))
	     {
		_XmMenuFocus((Widget) parent, XmMENU_BEGIN, _time);

		menuSTrait -> arm((Widget) cb);
	     }
             else
	       menuSTrait -> menuBarCleanup((Widget) parent);

             /* do the select without calling the cascading callbacks again */
             Select (cb, event, FALSE);

	     /* To support menu replay, keep the pointer in sync mode */
	     XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

             if (CB_Submenu(cb))
             {
                /*
                 * if XmProcessTraversal() fails, it's possible that the pane
                 * has no traversable children, so reset the focus to the pane.
                 */
                if (!XmProcessTraversal(CB_Submenu(cb), XmTRAVERSE_CURRENT))
		{
		   /* Must clear focus path first for shared menushells.
		    * Otherwise, moving the focus back will have old stale
		    * (old) focus_item.
		    */
		   _XmClearFocusPath(CB_Submenu(cb));
                   XtSetKeyboardFocus(XtParent(CB_Submenu(cb)), CB_Submenu(cb));
		}
             }
             else
             {
	        menuSTrait -> disarm((Widget) parent);
		_XmMenuFocus(XtParent(cb), XmMENU_END, _time);
                XtUngrabPointer( (Widget) cb, _time);
             }
          }

          break;
       }

       case XmMENU_PULLDOWN:
       case XmMENU_POPUP:
       {
	  /* In case the tear off is active but not armed or grabbed */
	  menuSTrait -> tearOffArm((Widget) parent);
          Select (cb, event, TRUE);
	  if (CB_Submenu(cb))
	  {
             /*
              * if XmProcessTraversal() fails, it's possible that the pane
              * has no traversable children, so reset the focus to the pane.
              */
             if (!XmProcessTraversal(CB_Submenu(cb), XmTRAVERSE_CURRENT))
	     {
		/* Must clear focus path first for shared menushells.
		 * Otherwise, moving the focus back will have old stale
		 * (old) focus_item.
		 */
		_XmClearFocusPath(CB_Submenu(cb));
                XtSetKeyboardFocus(XtParent(CB_Submenu(cb)), CB_Submenu(cb));
	     }
	  }
          break;
       }
    }
    /* Record so spring loaded DispatchEvent() doesn't recall this routine.  */
    if (event)
       _XmRecordEvent(event);
}


/*
 * disarm the menu.  This may include popping down any submenu that is up or
 * removing the timeout to post a submenu
 */
static void
Disarm(
        register XmCascadeButtonWidget cb,
        Boolean unpost )
{
   Widget rowcol = XtParent (cb);

   if (CB_IsArmed(cb))
   {
      CB_SetArmed(cb, FALSE);

      /* popdown any posted submenus */
      if (unpost && RC_PopupPosted(rowcol))
      {
	  (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	     menu_shell_class.popdownEveryone))(RC_PopupPosted(rowcol),NULL,
						NULL, NULL);
      }

      /* if a delayed arm is pending, remove it */
      if (cb->cascade_button.timer)
      {
         XtRemoveTimeOut (cb->cascade_button.timer);
         cb->cascade_button.timer = 0;
      }

      /* if the shadow is drawn and the menupane is not going down, erase it */
      if ((! RC_PoppingDown(rowcol)) || RC_TornOff(rowcol))
      {
	 if (XtIsRealized ((Widget)cb))
	 {
	     /* etched in menu button */
	     XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(cb));
	     Boolean etched_in = dpy->display.enable_etched_in_menu;

	     if (etched_in)
		 Redisplay((Widget) cb, NULL, NULL);
	     else
		 XmeClearBorder (XtDisplay (cb), XtWindow (cb),
				 cb->primitive.highlight_thickness,
				 cb->primitive.highlight_thickness,
				 cb->core.width - 2 *
				 cb->primitive.highlight_thickness,
				 cb->core.height - 2 *
				 cb->primitive.highlight_thickness,
				 cb->primitive.shadow_thickness);
	 }
      }
      DrawCascade(cb);
   }
}


/*
 * called when the post delay timeout occurs.
 */
/*ARGSUSED*/
static void
PostTimeout(
        XtPointer closure,
        XtIntervalId *id)	/* unused */
{
        XmCascadeButtonWidget cb = (XmCascadeButtonWidget) closure ;

   if (cb->cascade_button.timer)
   {
      cb->cascade_button.timer = 0;

      _XmCascadingPopup ((Widget) cb, NULL, TRUE);
   }
}


/*
 * set the timer to post the submenu if a leave event does
 * not occur first.
 */
/*ARGSUSED*/
static void
DelayedArm(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   if ((! CB_IsArmed(cb)) &&
       (((XmMenuShellWidget) XtParent(XtParent(cb)))->shell.popped_up) &&
       _XmGetInDragMode((Widget) cb))
   {
      if (cb->cascade_button.map_delay <= 0)
	 ArmAndPost (cb, event);

      else
      {
 	 /* To fix CR 8172,  the following two lines were reversed.
	    Because calling Arm seems to cause a focus change (temporary)
	    out of the widget,  the timer was incorrectly removed and
	    the menu wouldn't post. */
         Arm(cb);
         cb->cascade_button.timer =
               XtAppAddTimeOut(XtWidgetToApplicationContext( (Widget) cb),
			       (unsigned long) cb->cascade_button.map_delay,
                                                 PostTimeout, (XtPointer) cb) ;
      }
   }
}


/*
 * if traversal is not on and the mouse
 * has not entered its cascading submenu, disarm the
 * cascadebutton.
 */
/*ARGSUSED*/
static void
CheckDisarm(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   register XmMenuShellWidget submenushell;
   XEnterWindowEvent * entEvent = (XEnterWindowEvent *) event;

   if (_XmGetInDragMode((Widget) cb) &&
       (/* !ActiveTearOff || */ event->xcrossing.mode == NotifyNormal))
   {
      if ((CB_IsArmed(cb)) &&
          (CB_Submenu(cb)))
      {
         submenushell = (XmMenuShellWidget) XtParent (CB_Submenu(cb));

         if (submenushell->shell.popped_up)
         {
            if ((entEvent->x_root >= submenushell->core.x) &&
                (entEvent->x_root <  submenushell->core.x +
                                     submenushell->core.width +
                                     (submenushell->core.border_width << 1)) &&
                (entEvent->y_root >= submenushell->core.y) &&
                (entEvent->y_root <  submenushell->core.y +
                                     submenushell->core.height +
	   			     (submenushell->core.border_width << 1)))

  	        /* then we are in the cascading submenu, don't disarm */
 	        return;

             /*
              * When kick-starting a cascading menu from a tear off, we grab
              * the pointer to the parent rc when the cascade has the focus
              * (In StartDrag().  A leave window event is generated (with
              * mode = NotifyGrab) which we don't wish to recognize.
              */
             if ((entEvent->mode == NotifyGrab) &&
                 !XmIsMenuShell(XtParent(XtParent(cb))))
                return;
         }
      }
      Disarm (cb, TRUE);
   }
}


/*
 * post submenu and disable menu's traversal.  The order of these
 * function calls is critical.
 */
/*ARGSUSED*/
static void
StartDrag(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   Boolean validButton;
   XmRowColumnWidget parent = (XmRowColumnWidget)XtParent(cb);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass((Widget) parent), XmQTmenuSystem);

   /* If no menu system trait then parent isn't a menu as it
      should be */
   if (menuSTrait == NULL) return;

   /* Start with posted submenu bit reset */
   CB_SetWasPosted(cb, FALSE);

   if (CB_Submenu(cb) &&
       RC_IsArmed ((XmRowColumnWidget) CB_Submenu(cb))) {
     CB_SetWasPosted(cb, TRUE);
   }

   /*
    * make sure the shell is popped up, this takes care of a corner case
    * that can occur with rapid pressing of the mouse button
    */
   if (Lab_IsMenupane(cb) &&
       (!((XmMenuShellWidget) XtParent(parent))->shell.popped_up))
   {
      /* To support menu replay, keep the pointer in sync mode */
      XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

      return;
   }

   validButton = menuSTrait -> verifyButton((Widget) parent, event);

   if (validButton)
   {
      /* In case the tear off is active but not armed or grabbed */
      menuSTrait -> tearOffArm((Widget) parent);

      _XmSetInDragMode((Widget) cb, True);

      _XmCascadingPopup ((Widget) cb, event, TRUE);
      Arm (cb);

      /* record event so MenuShell does not process it */
      _XmRecordEvent (event);
   }

   /* To support menu replay, keep the pointer in sync mode */
   XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);
}


/*
 * do the popup (either w/ or w/o the cascade callbacks).
 * If there is not a submenu, bring down the menu system.
 */
static void
Select(
        register XmCascadeButtonWidget cb,
        XEvent *event,
        Boolean doCascade )
{
   XmAnyCallbackStruct cback;
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(cb)), XmQTmenuSystem);
   if (menuSTrait == NULL) return;

   if (CB_WasPosted(cb)) {
     Disarm(cb, TRUE);
     if ((CB_Submenu(cb) != NULL) && (Lab_MenuType(cb) == XmMENU_BAR))
       _XmMenuPopDown(XtParent((Widget) cb), event, NULL);
     return;
   }

   _XmCascadingPopup ((Widget) cb, event, doCascade);

   /*
    * check if there is a submenu here in case this changed during
    * the cascading callbacks
    */
   if (CB_Submenu(cb) == NULL)
   {
      menuSTrait -> popdown(XtParent(cb), event);

      Disarm (cb, FALSE);

      menuSTrait -> disarm(XtParent(cb));

      cback.event = event;
      cback.reason = XmCR_ACTIVATE;

      if (menuSTrait != NULL)
      {
	menuSTrait -> entryCallback(XtParent(cb), (Widget) cb, &cback);
      }

      if ((! cb->label.skipCallback) &&
	  (cb->cascade_button.activate_callback))
      {

	XtCallCallbackList ((Widget) cb, cb->cascade_button.activate_callback, &cback);
      }
    }
   else
     {
       Arm(cb);
     }
}

/*
 * if there is a submenu, enable traversal.
 * call select to do the work
 */
/*ARGSUSED*/
static void
DoSelect(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   register XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   Boolean validButton;
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);
   if (menuSTrait == NULL) return;

   /* To support menu replay, keep the pointer in sync mode */
   XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

   if (event && event -> type == ButtonRelease &&
       event -> xbutton.button == 2)
     return;

   if (!CB_IsArmed(cb))
      return;

   if ((Lab_MenuType(cb) == XmMENU_BAR) && !RC_IsArmed (XtParent(cb)))
      return;

   /*
    * make sure the shell is popped up, this takes care of a corner case
    * that can occur with rapid pressing of the mouse button
    */
   if (Lab_IsMenupane(cb) &&
       (!((XmMenuShellWidget) XtParent(XtParent(cb)))->shell.popped_up))
   {
      return;
   }

   validButton = menuSTrait -> verifyButton(XtParent(cb), event);

   if (validButton)
   {
      Select (cb, event, (Boolean)(CB_Submenu(cb) != NULL));

      /* don't let the menu shell widget process this event */
      _XmRecordEvent (event);

      _XmSetInDragMode((Widget) cb, False);

      if (CB_Submenu(cb))
      {
         /*
          * if XmProcessTraversal() fails, it's possible that the pane
          * has no traversable children, so reset the focus to the pane.
          */
         if (!XmProcessTraversal(CB_Submenu(cb), XmTRAVERSE_CURRENT))
	 {
	    /* Must clear focus path first for shared menushells.
	     * Otherwise, moving the focus back will have old stale
	     * (old) focus_item.
	     */
	    _XmClearFocusPath(CB_Submenu(cb));
            XtSetKeyboardFocus(XtParent(CB_Submenu(cb)), CB_Submenu(cb));
	 }
      }
      else
      {
	 /* Move this call into Select().
	  *
	  * (* xmLabelClassRec.label_class.menuProcs) (XmMENU_DISARM,
	  *					    XtParent(cb));
	  */

         if (Lab_MenuType(cb) == XmMENU_BAR)
         {
	    _XmMenuFocus(XtParent(cb), XmMENU_END, CurrentTime);
            XtUngrabPointer( (Widget) cb, CurrentTime);
         }
      }
   }
}

/*
 * if the menu system traversal is enabled, do a select
 */
/*ARGSUSED*/
static void
KeySelect(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;

   if (!_XmGetInDragMode((Widget) cb) &&
       (RC_IsArmed(XtParent(cb)) ||
	(RC_Type(XtParent(cb)) != XmMENU_BAR &&
	 !XmIsMenuShell(XtParent(XtParent(cb))))))
      (* (((XmCascadeButtonClassRec *)(cb->core.widget_class))->
		primitive_class.arm_and_activate))
			((Widget) cb, event, NULL, NULL);
}


/*
 * If the menu system is not active, arm it and arm this cascadebutton
 * else start the drag mode
 */
static void
MenuBarSelect(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   Boolean validButton;
   Time _time = _XmGetDefaultTime(wid, event);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait)
     XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);
   if (menuSTrait == NULL) return;

   CB_SetWasPosted(cb, FALSE);

   if (RC_IsArmed ((XmRowColumnWidget) XtParent(cb)))
   {
      /* Cleanup the PM menubar mode, if enabled */
      menuSTrait -> menuBarCleanup(XtParent(cb));

      if (!CB_Submenu(cb))
      {
	 _XmMenuFocus(XtParent(cb), XmMENU_MIDDLE, _time);
      }

      StartDrag ((Widget) cb, event, param, num_param);
   }

   else
   {
      /* XAllowEvents() is called here because StartDrag also calls it */
      /* To support menu replay, keep the pointer in sync mode */
      XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

      validButton = menuSTrait -> verifyButton(XtParent(cb), event);

      if (validButton)
      {
         /*
         * Don't post the menu if the menu cannot control grabs!
         */

	 if (_XmMenuGrabKeyboardAndPointer(XtParent(cb), _time) != GrabSuccess)
         {
	     _XmRecordEvent (event);
	     return;
	 }

	 _XmMenuFocus(XtParent(cb), XmMENU_BEGIN, _time);

	 menuSTrait -> arm((Widget) cb);

         _XmSetInDragMode((Widget) cb, True);

         _XmCascadingPopup ((Widget) cb, event, TRUE);

	 if (!CB_Submenu(cb))
	 {
	    /*
	     * since no submenu is posted, check if the grab has occured
	     * and if not, do the pointer grab now.
	     */
	    if (RC_BeingArmed(XtParent(cb)))
	    {

               _XmGrabPointer(XtParent(cb), True, EVENTS,
                  GrabModeAsync, GrabModeAsync, None,
		  XmGetMenuCursor(XtDisplay(cb)), _time);

	       RC_SetBeingArmed(XtParent(cb), False);
	    }
	 }

          /* To support menu replay, keep the pointer in sync mode */
          XAllowEvents(XtDisplay(cb), SyncPointer, CurrentTime);

	 /* record so that menuShell doesn't process this event */
	 _XmRecordEvent (event);
      }
   }
}


/*
 * If the menu is active, post submenu and arm.
 */
/*ARGSUSED*/
static void
MenuBarEnter(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        register XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   XmRowColumnWidget rc = (XmRowColumnWidget)XtParent(cb);

   if (RC_IsArmed(rc) && !CB_IsArmed(cb) && _XmGetInDragMode((Widget) cb))
   {
      if (!CB_Submenu(cb))
      {
	 _XmMenuFocus((Widget) rc, XmMENU_MIDDLE,
		      _XmGetDefaultTime(wid, event));
      }

      _XmCascadingPopup ((Widget) cb, event, TRUE);
      Arm(cb);
   }
}


/*
 * unless our submenu is posted or traversal is on, disarm
 */
/*ARGSUSED*/
static void
MenuBarLeave(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
   register XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
   XmMenuShellWidget submenuShell;

   if (RC_IsArmed (XtParent (cb)))
   {
      /* Reset this bit so that we don't unpost if the user
	 reenters the cascade button */
      CB_SetWasPosted(cb, FALSE);

      if (CB_Submenu(cb))
      {
         submenuShell = (XmMenuShellWidget) XtParent(CB_Submenu(cb));

         if (submenuShell->shell.popped_up)
            return;
      }

      if (_XmGetInDragMode((Widget) cb))
         Disarm (cb, TRUE);
   }
}

/*
 * Cleanup the menubar, if its in the PM traversal mode
 */
/*ARGSUSED*/
static void
CleanupMenuBar(
        Widget wid,
        XEvent *event,
        String *param,
        Cardinal *num_param )
{
        XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
    XmRowColumnWidget parent = (XmRowColumnWidget)XtParent(cb);

    if (RC_IsArmed(parent))
    {
        (* ((XmRowColumnWidgetClass) XtClass(parent))->
             row_column_class.armAndActivate) ( (Widget) parent,
                          (XEvent *) NULL, (String *) NULL, (Cardinal *) NULL);
	_XmRecordEvent(event);
    }
}


/*
 * CascadeButton Widget and Gadget help routine - first bring down the
 * menu and then do the help callback.
 */
void
_XmCBHelp(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   XmRowColumnWidget parent = (XmRowColumnWidget) XtParent(w);

   if (RC_Type(parent) == XmMENU_BAR)
   {
      /* Cannot call CleanupMenubar() 'cause it calls _XmRecordEvent */
      if (RC_IsArmed(parent))
      {
	 (* ((XmRowColumnWidgetClass) XtClass(parent))->
	      row_column_class.armAndActivate) ( (Widget) parent,
			   (XEvent *) NULL, (String *) NULL, (Cardinal *) NULL);
      }
   }

   else if ((RC_Type(parent) == XmMENU_PULLDOWN) ||
            (RC_Type(parent) == XmMENU_POPUP))
   {
      (*(((XmMenuShellClassRec *) xmMenuShellWidgetClass)->
          menu_shell_class.popdownDone)) (XtParent(parent), event,
                                                           params, num_params);
   }

   if (XmIsGadget(w))
      _XmSocorro(w, event, params, num_params);
   else
      _XmPrimitiveHelp( w, event, params, num_params) ;
}


/*
 * When moving between a shared menupane, we only want to unpost the
 * descendant panes, not the shared one.
 * We only need to check the first popup child, since the menushell
 * has always forced the popped up shell to be the first child.
 */
static void
PopdownGrandChildren(
        XmRowColumnWidget rowcol )
{
   CompositeWidget menuShell;

   if ((menuShell = (CompositeWidget) RC_PopupPosted(rowcol)) == NULL)
       return;

   if ((menuShell = (CompositeWidget)
	RC_PopupPosted (menuShell->composite.children[0])) != NULL)
   {
      (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
	 menu_shell_class.popdownEveryone))( (Widget) menuShell, NULL,
                                                                   NULL, NULL);
   }
}


/*
 * call the cascading callbacks.  The cb parameter can be either a
 * cascadebutton widget or gadget.
 */
static void
Cascading(
        Widget w,
        XEvent *event )
{
    XmAnyCallbackStruct cback;

    cback.reason = XmCR_CASCADING;
    cback.event = event;

    if (XmIsCascadeButton(w))
    {
       XmCascadeButtonWidget cb = (XmCascadeButtonWidget)w;
       XmRowColumnWidget submenu = (XmRowColumnWidget) CB_Submenu(cb);

       /* if the submenu is already up, just return */
       /* In case of shared menupanes, check the cascade button attachment */

       if (submenu)
       {
           XmMenuShellWidget ms = (XmMenuShellWidget) XtParent(submenu);
	   if (XmIsMenuShell(ms) &&
	       ms->shell.popped_up &&
 	       ms->composite.children[0] == (Widget) submenu &&
 	       submenu->row_column.cascadeBtn == (Widget) cb)
           {
	      return;
           }
       }
       XtCallCallbackList ((Widget) cb, cb->cascade_button.cascade_callback, &cback);
    }
    else
    {
       XmCascadeButtonGadget cb = (XmCascadeButtonGadget)w;
       XmRowColumnWidget submenu = (XmRowColumnWidget) CBG_Submenu(cb);

       /* if the submenu is already up, just return */
       if (submenu)
       {
           XmMenuShellWidget ms = (XmMenuShellWidget) XtParent(submenu);
	   if (XmIsMenuShell(ms) &&
	       ms->shell.popped_up &&
 	       ms->composite.children[0] == (Widget) submenu &&
 	       submenu->row_column.cascadeBtn == (Widget) cb)
           {
	      return;
           }
       }

       XtCallCallbackList ((Widget) cb, cb->cascade_button.cascade_callback, &cback);
    }
}


/*
 * call the cascading callbacks and the popup any submenu.  This is called
 * by both the cascadebutton widget and gadget.
 */
void
_XmCascadingPopup(
        Widget cb,
        XEvent *event,
        Boolean doCascade )
{
   /* We must make sure the tear off to menushell restoration/callbacks are
    * called before the cascading callback.  Exclude the pane in case Popup()
    * tries to restore it back to a transient if shared and already posted.
    */
   XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(cb));
   XmExcludedParentPaneRec *excPP =
		&(((XmDisplayInfo *)(dd->display.displayInfo))->excParentPane);

   if (!excPP->pane)
   {
      excPP->pane_list_size = 4;
      excPP->pane = (Widget *)XtMalloc(sizeof(Widget) * excPP->pane_list_size);
   }

   if (XmIsCascadeButtonGadget(cb))
      *(excPP->pane) = CBG_Submenu(cb);
   else
      *(excPP->pane) = CB_Submenu(cb);

   if (*(excPP->pane))
   {
      excPP->num_panes = 1;

      if (RC_TornOff(*(excPP->pane)) &&
          !XmIsMenuShell(XtParent(*(excPP->pane))))
      {
 	 /* If a subpane is already posted and it is not the pane that
 	  * will be posted from cb.  Then it must be lowered so that
 	  * its tear off can be repainted.
 	  */
 	 if (RC_PopupPosted(XtParent(cb)))
 	 {
 	    XmRowColumnWidget postedPane = (XmRowColumnWidget)
 	       ((CompositeWidget)RC_PopupPosted(XtParent(cb)))->
 	          composite.children[0];

 	    if ((Widget)postedPane != *(excPP->pane))
 	    {
 	       _XmLowerTearOffObscuringPoppingDownPanes( (Widget)postedPane,
 		  *(excPP->pane));
 	    }
 	 }
	 _XmRestoreTearOffToMenuShell(*(excPP->pane), event);
      }
   }

   if (doCascade)
       Cascading (cb, event);
   Popup (cb, event);
}

/*
 * pop up the pulldown menu associated with this cascadebutton
 */
static void
Popup(
        Widget cb,
        XEvent *event )
{
    Widget oldActiveChild;
    Boolean popped_up = False;
    register XmRowColumnWidget   submenu;
    XmMenuShellWidget shell = NULL;
    register XmRowColumnWidget	parent   = (XmRowColumnWidget) XtParent (cb);
    XmMenuSystemTrait menuSTrait;
    XmDisplay dd = (XmDisplay)XmGetXmDisplay(XtDisplay(cb));
    XmExcludedParentPaneRec *excPP =
		&(((XmDisplayInfo *)(dd->display.displayInfo))->excParentPane);

    menuSTrait = (XmMenuSystemTrait)
      XmeTraitGet((XtPointer) XtClass((Widget) parent), XmQTmenuSystem);
    if (menuSTrait == NULL) return;

    if (XmIsCascadeButtonGadget(cb))
       submenu = (XmRowColumnWidget) CBG_Submenu(cb);
    else
       submenu = (XmRowColumnWidget) CB_Submenu(cb);

    /* if its already up, popdown submenus and then return */
    if (submenu &&
	(shell = (XmMenuShellWidget)XtParent(submenu)) &&
	XmIsMenuShell(shell) &&
	(popped_up = shell->shell.popped_up))
    {

        /* Just in case the menu shell is being shared.
	 * Shell's 0th child is currently posted submenu.  In case of shared
	 * menupanes we must check to make sure that it is not posted from
	 * the same cascade button before popping down.
	 * Also this is as good a time as any to clear have_traversal field
	 * of submenu's active child.  Updating this internal state allows
	 * this gadget to highlight next time the submenu is posted.
	 */
        if ((XmRowColumnWidget)shell->composite.children[0] == submenu)
	{
	   if (cb == RC_CascadeBtn(submenu))
	   {
	       if (RC_PopupPosted(submenu))
		   (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
		      menu_shell_class.popdownEveryone))
		       (RC_PopupPosted(submenu),NULL,NULL, NULL);

              if (submenu->manager.active_child)
              {
                 /* update visible focus/highlighting */
                 if (XmIsPrimitive(submenu->manager.active_child))
                 {
                     (*(((XmPrimitiveClassRec *)XtClass(submenu->manager.
                     active_child))->primitive_class.border_unhighlight))
                     (submenu->manager.active_child);
                 }
                 else if (XmIsGadget(submenu->manager.active_child))
                 {
                     (*(((XmGadgetClassRec *)XtClass(submenu->manager.
                     active_child))->gadget_class.border_unhighlight))
                     (submenu->manager.active_child);
                 }
                 /* update internal focus state */
                 _XmClearFocusPath((Widget) submenu);
              }
	      *(excPP->pane) = NULL;
	      excPP->num_panes = 0;
	      return;
	   }
	   else
	   {
	      oldActiveChild = submenu->manager.active_child;
	      if (oldActiveChild && XmIsGadget(oldActiveChild))
		 ((XmGadget)oldActiveChild)->gadget.have_traversal = False;
	   }
	}
    }

    if (XtIsManaged ((Widget)parent))
    {
        if ((RC_Type(parent) == XmMENU_BAR) && !RC_IsArmed (parent))
	   return;

        /*
         * If the old active child for the menupane was a cascadeB gadget,
         * and it did not have its submenu posted, then
         * we need to manually send it FocusOut notification, since
         * when we managed our submenu, the active_child field for
         * our parent was set to us, and the parent now no longer knows
         * who previously had the focus.
         */
        oldActiveChild = parent->manager.active_child;
        if (oldActiveChild &&
            (oldActiveChild != (Widget)cb) &&
            XmIsCascadeButtonGadget(oldActiveChild) &&
            CBG_Submenu(oldActiveChild) &&
            (((XmMenuShellWidget)XtParent(CBG_Submenu(oldActiveChild)))->
               shell.popped_up == False))
        {
            parent->manager.active_child = NULL;
            _XmDispatchGadgetInput((Widget) oldActiveChild, NULL,
                                                            XmFOCUS_OUT_EVENT);
            ((XmGadget)oldActiveChild)->gadget.have_traversal = False;
        }
        else
	  /*
	   * Fix for CR 5683 - If the RC_CascadeBtn == cb, then the menupane
	   *		       should not be popped down (it probably already
	   *		       is popped down), so do not pop down the
	   *		       menupane (it messes up the traversal)
	   */
	if (!submenu ||
	    !popped_up ||
	    (RC_PopupPosted(parent) != (Widget)shell) ||
	    (submenu && RC_CascadeBtn(submenu) &&
	     (RC_CascadeBtn(submenu) != cb) &&
	     ((Widget)parent == XtParent(RC_CascadeBtn(submenu)))) )
        {
	   /* popdown all visible subpanes of this parent when:
	    * - moving to a button without a submenu
	    * - between non-shared menushells
	    *   = then menushell will not be popped_up
	    *   = the old shell is different than the new shell
	    *     (old shell nonshared, new shell shared)
	    * - special case when same pane attached to > 1 cb in same parent
	    */
	    if (RC_PopupPosted(parent))
	    {
		(*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
		   menu_shell_class.popdownEveryone))
		    (RC_PopupPosted(parent),NULL,NULL, NULL);
	    }

	   /* Focus is not being handled perfectly for tear offs whose parent
	    * is a top level shell.  So force cascade unhighlighting here.
	    */
	   if (NULL != oldActiveChild &&
	       oldActiveChild != cb &&
	       ((parent->row_column.type == XmMENU_PULLDOWN) ||
		(parent->row_column.type == XmMENU_POPUP)) &&
	       !XmIsMenuShell(XtParent(parent)))
	       XmCascadeButtonHighlight(oldActiveChild, FALSE);
        }
        else
        {
           /*
            * Handle shared menupanes */
           PopdownGrandChildren (parent);
        }

	/* We don't allow the possibility of the submenu to be restored
	 * from the menushell back to the transient shell during the
	 * previous popdown code.  This occurs when the tear off is shared
	 * and previously posted.
	 */
	*(excPP->pane) = NULL;
	excPP->num_panes = 0;

	if (submenu)
	{
           if (((ShellWidget)XtParent(submenu))->composite.num_children == 1)
	   {
	      menuSTrait -> cascade((Widget) submenu, cb, event);

	      /* Map the window first to sync up the server in case the
	       * menushell was previously shared
	       */
	      XMapWindow(XtDisplay(submenu), XtWindow(submenu));
	      XtManageChild( (Widget) submenu);
	   }
           else
           {
	      /* We will call menuprocs XmMENU_CASCADING from
	       * popupSharedMenuShell routine so that it occurs between
	       * shared menupane window configurations.
	       */
              (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
                   menu_shell_class.popupSharedMenupane))(cb,
                                                      (Widget) submenu, event);
           }
	   /* So help is delivered correctly when in drag mode */
	   if (_XmGetInDragMode((Widget)cb))
	      XtSetKeyboardFocus((Widget)submenu, None);
	}
    }
}


/*
 * get the cascade size set up
 */
static void
size_cascade(
        XmCascadeButtonWidget cascadebtn )
{
    Window rootwin;
    int x,y;					       /* must be int */
    unsigned int width, height, border, depth;	       /* must be int */

    if (CB_CascadePixmap(cascadebtn) != XmUNSPECIFIED_PIXMAP)
    {
       XGetGeometry(XtDisplay(cascadebtn), CB_CascadePixmap(cascadebtn),
		    &rootwin, &x, &y, &width, &height,
		    &border, &depth);

       CB_Cascade_width(cascadebtn) = (Dimension) width;
       CB_Cascade_height(cascadebtn) = (Dimension) height;
    }
    else
    {
       CB_Cascade_width(cascadebtn) = 0;
       CB_Cascade_height(cascadebtn) = 0;
    }
}


/*
 * set up the cascade position.
 */
static void
position_cascade(
        XmCascadeButtonWidget cascadebtn )
{
   Dimension buffer;

   if (CB_HasCascade(cascadebtn))
   {
      if (LayoutIsRtoLP(cascadebtn))
         CB_Cascade_x(cascadebtn) = cascadebtn->primitive.highlight_thickness +
                                    cascadebtn->primitive.shadow_thickness +
                                    Lab_MarginWidth(cascadebtn);
      else
	CB_Cascade_x(cascadebtn) = XtWidth (cascadebtn) -
                               cascadebtn->primitive.highlight_thickness -
                               cascadebtn->primitive.shadow_thickness -
			       Lab_MarginWidth(cascadebtn) -
                               CB_Cascade_width(cascadebtn);

      buffer = cascadebtn->primitive.highlight_thickness +
             cascadebtn->primitive.shadow_thickness +
             Lab_MarginHeight(cascadebtn);

      CB_Cascade_y(cascadebtn) = buffer +
                               ((XtHeight(cascadebtn) -  2*buffer) -
                                CB_Cascade_height(cascadebtn)) / 2;
   }
   else
   {
      CB_Cascade_y(cascadebtn) = 0;
      CB_Cascade_x(cascadebtn) = 0;
   }
}


/*
 * set up the cascade size and location
 */
static void
setup_cascade(
        XmCascadeButtonWidget cascadebtn,
        Boolean adjustWidth,
        Boolean adjustHeight )
{
   Dimension delta;

   if (CB_HasCascade(cascadebtn))
   {
      /*
       *  modify the size of the cascadebutton to acommadate the cascade, if
       *  needed.  The cascade should fit inside MarginRight.
       */
      if (LayoutIsRtoLP(cascadebtn))
      {
        if ((CB_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE) >
             Lab_MarginLeft(cascadebtn))
        {
            delta = CB_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE -
                Lab_MarginLeft(cascadebtn);
            Lab_MarginLeft(cascadebtn) += delta;

            if (adjustWidth)
                XtWidth(cascadebtn) += delta;

            else
            {
               if (cascadebtn->label.alignment == XmALIGNMENT_BEGINNING)
                   Lab_TextRect_x(cascadebtn) += delta;
               else if (cascadebtn->label.alignment == XmALIGNMENT_CENTER)
                   Lab_TextRect_x(cascadebtn) += delta/2;
            }
        }

      }
      else
      {
	if ((CB_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE) >
	    Lab_MarginRight(cascadebtn))
	  {
	    delta = CB_Cascade_width(cascadebtn) + CASCADE_PIX_SPACE -
	      Lab_MarginRight(cascadebtn);
	    Lab_MarginRight(cascadebtn) += delta;

	    if (adjustWidth)
	      XtWidth(cascadebtn) += delta;

	    else
	      {
		if (cascadebtn->label.alignment == XmALIGNMENT_END)
		  Lab_TextRect_x(cascadebtn) -= delta;
		else if (cascadebtn->label.alignment == XmALIGNMENT_CENTER)
		  Lab_TextRect_x(cascadebtn) -= delta/2;
	      }
	  }
      }

      /*
       * the cascade height should fit inside of
       * TextRect + marginTop + marginBottom
       */
      delta = CB_Cascade_height(cascadebtn) +
	  2 * (Lab_MarginHeight(cascadebtn) +
	       cascadebtn->primitive.shadow_thickness +
	       cascadebtn->primitive.highlight_thickness);

      if (delta > XtHeight(cascadebtn))
      {
	 delta -= XtHeight(cascadebtn);
	 Lab_MarginTop(cascadebtn) += delta/2;
	 Lab_TextRect_y(cascadebtn) += delta/2;
	 Lab_MarginBottom(cascadebtn) += delta - (delta/2);

	 if (adjustHeight)
	     XtHeight(cascadebtn) += delta;
      }
   }

   position_cascade(cascadebtn);
}


/*
 * Destroy the widget
 */
static void
Destroy(
        Widget wid )
{
  XmCascadeButtonWidget cb = (XmCascadeButtonWidget) wid ;
  XmRowColumnWidget submenu = (XmRowColumnWidget) CB_Submenu(cb);
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(wid)), XmQTmenuSystem);


  /*
   * If the armed pixmap exists, both pixmaps must be cached arrows
   */
  if (CB_ArmedPixmap(cb))
    {
	_XmProcessLock();
	_XmArrowPixmapCacheDelete((XtPointer) CB_ArmedPixmap(cb));
	_XmArrowPixmapCacheDelete((XtPointer) CB_CascadePixmap(cb));
	_XmProcessUnlock();
    }

  /*
   * break the submenu link
   */
  if (submenu != NULL && menuSTrait != NULL)
    menuSTrait -> recordPostFromWidget((Widget) submenu, (Widget) cb, FALSE);

  if (cb->cascade_button.timer)
    XtRemoveTimeOut (cb->cascade_button.timer);

  /* Release the GCs */

  XtReleaseGC (wid, CB_ArmGC(wid));
  XtReleaseGC (wid, CB_BackgroundGC(wid));

}


/*
 * routine to resize a cascade button, called by the parent
 * geometery manager
 */
static void
Resize(
        Widget cb )
{
    /*
     */
     if (cb)
     {
	XtWidgetProc resize;

	/* Label class does it's work */

	_XmProcessLock();
        resize = xmLabelClassRec.core_class.resize;
	_XmProcessUnlock();
	(* resize) (cb);

	/* move the cascade too */
	position_cascade ((XmCascadeButtonWidget) cb);
     }
}

/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
SetValuesPrehook(
        Widget cw,		/* unused */
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmCascadeButtonWidget new_w = (XmCascadeButtonWidget) nw ;

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (new_w->label.font == NULL)
    new_w->label.font = XmeGetDefaultRenderTable (nw, XmBUTTON_FONTLIST);

  return False;
}

/*
 * Set Values
 */
/*ARGSUSED*/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  XmCascadeButtonWidget old = (XmCascadeButtonWidget) cw ;
  XmCascadeButtonWidget requested = (XmCascadeButtonWidget) rw ;
  XmCascadeButtonWidget new_w = (XmCascadeButtonWidget) nw ;
  Boolean flag = FALSE;
  Boolean adjustWidth = FALSE;
  Boolean adjustHeight = FALSE;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass(XtParent(cw)), XmQTmenuSystem);

  if (old->primitive.foreground != new_w->primitive.foreground
    || old->core.background_pixel != new_w->core.background_pixel) {
      GetBackgroundGC(new_w);
  }

  if ((CB_Submenu(new_w)) &&
      ((! XmIsRowColumn(CB_Submenu(new_w))) ||
       (RC_Type(CB_Submenu(new_w)) != XmMENU_PULLDOWN)))
    {
      CB_Submenu(new_w) = NULL;
      XmeWarning( (Widget)new_w, WRONGSUBMENU);
    }

    if (new_w->cascade_button.map_delay < 0)
    {
       new_w->cascade_button.map_delay = old->cascade_button.map_delay;
       XmeWarning( (Widget)new_w, WRONGMAPDELAY);
    }

    /* if there is a change to submenu, notify menu system */
    if (CB_Submenu(old) != CB_Submenu(new_w))
    {
      /* We must pass nw as the parameter to recordPostFromWidget
       * because old is a copy!  The call to recordPostFromWidget() does
       * a widget ID comparison and we must pass the real widget (nw).
       */

      if (CB_Submenu(old) && menuSTrait)
	menuSTrait -> recordPostFromWidget(CB_Submenu(old), nw, FALSE);

      if (CB_Submenu(new_w) && menuSTrait)
	menuSTrait -> recordPostFromWidget(CB_Submenu(new_w), nw, TRUE);
    }

    /* don't let traversal be changed */
    if (Lab_MenuType(new_w) == XmMENU_BAR)
	new_w->primitive.traversal_on = TRUE;

    /* handle the cascade pixmap indicator */
    else if (Lab_IsMenupane(new_w))
    {
       /* don't let traversal be changed */
       new_w->primitive.traversal_on = TRUE;

       if ((new_w->label.recompute_size)  || (requested->core.width <= 0))
	  adjustWidth = TRUE;

       if ((new_w->label.recompute_size)  || (requested->core.height <= 0))
	  adjustHeight = TRUE;

       /* get new pixmap size */
       if (CB_CascadePixmap(old) != CB_CascadePixmap (new_w))
       {
	  if (CB_ArmedPixmap(old) != XmUNSPECIFIED_PIXMAP)
	  {
	     _XmProcessLock();
	     _XmArrowPixmapCacheDelete((XtPointer) CB_ArmedPixmap(old));
	     _XmArrowPixmapCacheDelete((XtPointer) CB_CascadePixmap(old));
	     _XmProcessUnlock();
	  }
	  CB_ArmedPixmap(new_w) = XmUNSPECIFIED_PIXMAP;
	  size_cascade (new_w);
       } else
          if ( ((CB_CascadePixmap(new_w) ==  XmUNSPECIFIED_PIXMAP) &&
                  (!CB_Submenu(old) && CB_Submenu(new_w))) ||
               ((CB_ArmedPixmap(old) != XmUNSPECIFIED_PIXMAP) &&
                  ((Lab_TextRect_height(old) != Lab_TextRect_height(new_w)) ||
		   (old->primitive.foreground != new_w->primitive.foreground) ||
		   (old->core.background_pixel !=
		      new_w->core.background_pixel))))
	  {
	     _XmProcessLock();
	     _XmArrowPixmapCacheDelete((XtPointer) CB_ArmedPixmap(old));
	     _XmArrowPixmapCacheDelete((XtPointer) CB_CascadePixmap(old));
	     CB_ArmedPixmap(new_w) = XmUNSPECIFIED_PIXMAP;
	     CB_CascadePixmap(new_w) = XmUNSPECIFIED_PIXMAP;
	     _XmCreateArrowPixmaps((Widget) new_w);
	     _XmProcessUnlock();
	     size_cascade (new_w);
	  }

       /*
        * resize widget if cascade appeared or disappeared, or if the
	* cascade pixmap changed size.
	*/
       if ((CB_CascadePixmap (old) != CB_CascadePixmap (new_w))  ||
	   (old->label.label_type != new_w->label.label_type) ||
	   (old->cascade_button.submenu != new_w->cascade_button.submenu))
       {
	  setup_cascade (new_w, adjustWidth, adjustHeight);

	  /* if there wasn't a cascade, and still isn't, don't redraw */
	  if (old->cascade_button.submenu || new_w->cascade_button.submenu)
	      flag = TRUE;
       }

       /* make sure that other changes did not scrunch our pixmap */
       else if (new_w->cascade_button.submenu)
       {
	  if ((new_w->primitive.highlight_thickness !=
	       old->primitive.highlight_thickness)               ||
	      (new_w->primitive.shadow_thickness !=
	       old->primitive.shadow_thickness)                  ||
	      (Lab_MarginRight (new_w) != Lab_MarginRight (old))   ||
	      (Lab_MarginHeight (new_w) != Lab_MarginHeight (old)) ||
	      (Lab_MarginTop (new_w) != Lab_MarginTop (old))	 ||
	      (Lab_MarginBottom (new_w) != Lab_MarginBottom (old)))
	  {
	     setup_cascade (new_w,adjustWidth, adjustHeight);
	     flag = TRUE;
	  }

	  else if ((Lab_MarginWidth(new_w) != Lab_MarginWidth(old)) ||
		   (new_w->core.width != old->core.width)           ||
		   (new_w->core.height != old->core.height))

	  {
	     position_cascade (new_w);
	     flag = TRUE;
	  }
       }
    }

    return (flag);
}

/************************************************************
 *
 * InitializePrehook
 *
 * Put the proper translations in core_class tm_table so that
 * the data is massaged correctly
 *
 ************************************************************/
/*ARGSUSED*/
static void
InitializePrehook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  unsigned char type;
  XmMenuSystemTrait menuSTrait;
  XmCascadeButtonWidget bw = (XmCascadeButtonWidget) new_w ;

  _XmSaveCoreClassTranslations (new_w);

  menuSTrait = (XmMenuSystemTrait)
    XmeTraitGet((XtPointer) XtClass((Widget) XtParent(new_w)), XmQTmenuSystem);

  if (menuSTrait != NULL)
    type = menuSTrait->type(XtParent(new_w));
  else
    type = XmWORK_AREA;

  _XmProcessLock();
  if (type == XmMENU_PULLDOWN ||
      type == XmMENU_POPUP)
    new_w->core.widget_class->core_class.tm_table = (String) p_events_parsed;
  else
    new_w->core.widget_class->core_class.tm_table =(String)menubar_events_parsed;
  _XmProcessUnlock();

  /* CR 2990: Use XmNbuttonFontList as the default font. */
  if (bw->label.font == NULL)
    bw->label.font = XmeGetDefaultRenderTable (new_w, XmBUTTON_FONTLIST);
}

/************************************************************
 *
 * InitializePosthook
 *
 * restore core class translations
 *
 ************************************************************/
/*ARGSUSED*/
static void
InitializePosthook(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
  _XmRestoreCoreClassTranslations (new_w);
}

/************************************************************************
 *
 *  GetArmGC
 *     Get the graphics context used for filling in background of the
 *     cascade button when armed.
 *
 ************************************************************************/

static void
GetArmGC(
        XmCascadeButtonWidget cb )
{
  XGCValues values;
  XtGCMask  valueMask;
  Pixel     junk, select_pixel;

  XmGetColors(XtScreen(cb), cb->core.colormap, cb->core.background_pixel,
	      &junk, &junk, &junk, &select_pixel);

  valueMask = GCForeground | GCBackground | GCGraphicsExposures;

  values.foreground = select_pixel;
  values.background = cb->primitive.foreground;
  values.graphics_exposures = False;

  CB_ArmGC(cb) = XtGetGC ((Widget) cb, valueMask, &values);
}

/************************************************************************
 *
 *  GetBackgroundGC
 *     Get the graphics context used for filling in background of
 *     the cascade button when not armed.
 *
 ************************************************************************/
static void
GetBackgroundGC(
        XmCascadeButtonWidget cb )
{
  XGCValues       values;
  XtGCMask        valueMask;
  XFontStruct     *fs;

  valueMask = GCForeground | GCBackground | GCFont | GCGraphicsExposures;

  values.foreground = cb->core.background_pixel;
  values.background = cb->primitive.foreground;
  values.graphics_exposures = False;

  if (XmeRenderTableGetDefaultFont(cb->label.font, &fs))
    values.font = fs->fid;
  else
    valueMask &= ~GCFont;

  CB_BackgroundGC(cb) = XtGetGC ((Widget) cb, valueMask, &values);
}

/*
 * Initialize
 */
/*ARGSUSED*/
static void
Initialize(
        Widget w_req,
        Widget w_new,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmCascadeButtonWidget  req = (XmCascadeButtonWidget) w_req ;
    XmCascadeButtonWidget  new_w = (XmCascadeButtonWidget) w_new ;
    Boolean adjustWidth = FALSE;
    Boolean adjustHeight = FALSE;
    XmMenuSystemTrait menuSTrait;

    XmRowColumnWidget    submenu = (XmRowColumnWidget) CB_Submenu(new_w);
    XmRowColumnWidget    parent = (XmRowColumnWidget) XtParent(new_w);

    menuSTrait = (XmMenuSystemTrait)
      XmeTraitGet((XtPointer) XtClass((Widget) parent), XmQTmenuSystem);

    if ((! XmIsRowColumn (parent)) &&
	((Lab_MenuType(new_w) == XmMENU_PULLDOWN) ||
	 (Lab_MenuType(new_w) == XmMENU_POPUP)    ||
	 (Lab_MenuType(new_w) == XmMENU_BAR)))
    {
       XmeWarning( (Widget)new_w, WRONGPARENT);
    }

    /* if menuProcs is not set up yet, try again */
    _XmProcessLock();
    if (xmLabelClassRec.label_class.menuProcs == NULL)
	xmLabelClassRec.label_class.menuProcs =
	    (XmMenuProc) _XmGetMenuProcContext();
    _XmProcessUnlock();

    /* CR 7651: Clear before setting. */
    new_w->cascade_button.armed = 0;
    CB_SetArmed(new_w, FALSE);
    new_w->cascade_button.timer = 0;
    CB_SetTraverse (new_w, FALSE);
    CB_SetWasPosted (new_w, FALSE);
    CB_ArmedPixmap(new_w) = XmUNSPECIFIED_PIXMAP;

    /*
     * if the user did not specify a margin width, set the default.
     * The menubar cbs have a larger margin.
     */
    if (Lab_MarginWidth(req) == XmINVALID_DIMENSION)
    {
       if (Lab_MenuType(new_w) == XmMENU_BAR)
          Lab_MarginWidth(new_w) = 6;
       else
          Lab_MarginWidth(new_w) = 2;
    }



    if (submenu &&
	(! XmIsRowColumn(submenu) ||
	 (RC_Type(submenu) != XmMENU_PULLDOWN)))
    {
       submenu = NULL;
       XmeWarning( (Widget)new_w, WRONGSUBMENU);
    }

    if (new_w->cascade_button.map_delay < 0)
    {
       new_w->cascade_button.map_delay = MAP_DELAY_DEFAULT;
       XmeWarning( (Widget)new_w, WRONGMAPDELAY);
    }

    /* call submenu's class function to set the link */
    if (submenu != NULL && menuSTrait != NULL)
      menuSTrait -> recordPostFromWidget((Widget) submenu, (Widget) new_w, TRUE);

   if (submenu && (CB_CascadePixmap(new_w) == XmUNSPECIFIED_PIXMAP)) {
      _XmProcessLock();
      _XmCreateArrowPixmaps((Widget) new_w);
      _XmProcessUnlock();
   }

    if (Lab_IsMenupane(new_w))
    {
      if (req->core.width <= 0)
	adjustWidth = TRUE;

      if (req->core.height <= 0)
	adjustHeight = TRUE;

      /* get pixmap size and set up widget to allow room for it */
      size_cascade (new_w);
      setup_cascade (new_w, adjustWidth, adjustHeight);
    }

    new_w->primitive.traversal_on = TRUE;

    /* Initialize GCs for armed button select and background only */
    GetArmGC (new_w);
    GetBackgroundGC (new_w);

}

/*
 *************************************************************************
 *
 * Public Routines
 *
 *************************************************************************
 */
Widget
XmCreateCascadeButton(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    Widget cb;

    cb = XtCreateWidget(name, xmCascadeButtonWidgetClass, parent, al, ac);

    return (cb);
}

Widget
XmVaCreateCascadeButton(
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
                         xmCascadeButtonWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedCascadeButton(
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
                         xmCascadeButtonWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/*
 * This routine is called for both cascadebutton gadgets and widgets.
 * The button is armed or disarmed but it does not pop up or down submenus.
 */
void
XmCascadeButtonHighlight(
        Widget cb,
        Boolean highlight )
{
  XtAppContext app;

  if (NULL == cb) return;
  app = XtWidgetToApplicationContext(cb);
  _XmAppLock(app);
   if ((cb) && XmIsCascadeButton(cb))
   {
      if (highlight)
         Arm ((XmCascadeButtonWidget) cb);

      else
         Disarm ((XmCascadeButtonWidget) cb, FALSE);
   }

   else if ((cb) && XmIsCascadeButtonGadget(cb))
      XmCascadeButtonGadgetHighlight ((Widget) cb, highlight);
  _XmAppUnlock(app);
}
