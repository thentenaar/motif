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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: MenuShell.c /main/24 1999/07/08 16:49:59 vipin $"
#endif
#endif

#include <X11/IntrinsicP.h>
#include <X11/Composite.h>
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/LayoutT.h>
#include <Xm/MenuT.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include "BaseClassI.h"
#include "GadgetUtiI.h"
#include "MapEventsI.h"
#include "MenuShellI.h"
#include "MenuStateI.h"
#include "MenuUtilI.h"
#include "MessagesI.h"
#include "RCMenuI.h"
#include "RowColumnI.h"
#include "TearOffI.h"
#include "TraversalI.h"
#include "UniqueEvnI.h"
#include "VendorSI.h"
#include "ResIndI.h"
#include "XmI.h"
#include <Xm/SlideC.h>

#define Events ((unsigned int) (ButtonPressMask | ButtonReleaseMask | \
		EnterWindowMask | LeaveWindowMask))

/* Warning messages */
#define ChildMsg	_XmMMsgMenuShell_0000
#define ManageMsg	_XmMMsgMenuShell_0001
#define MESSAGE2        _XmMMsgMenuShell_0002
#define MESSAGE3        _XmMMsgMenuShell_0003
#define MESSAGE4        _XmMMsgMenuShell_0004
#define MESSAGE5        _XmMMsgMenuShell_0005
#define MESSAGE6        _XmMMsgMenuShell_0006
#define MESSAGE7        _XmMMsgMenuShell_0007
#define MESSAGE8        _XmMMsgMenuShell_0008
#define MESSAGE9        _XmMMsgMenuShell_0009

#define default_translations	_XmMenuShell_translations


/********    Static Function Declarations    ********/

static void _XmFastExpose( 
                        register XmManagerWidget rowcol) ;
static void _XmFastPopdown( 
                        XmMenuShellWidget shell) ;
static void PostMenuShell( 
                        XmMenuShellWidget menuShell,
                        XtGrabKind grab_kind,
#if NeedWidePrototypes
                        int spring_loaded) ;
#else
                        Boolean spring_loaded) ;
#endif /* NeedWidePrototypes */
static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void Initialize( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void StructureNotifyHandler( 
                        Widget wid,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *continue_to_dispatch) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Resize( 
                        Widget wid) ;
static void DeleteChild( 
                        Widget widget) ;
static void InsertChild( 
                        Widget widget) ;
static void ForceMenuPaneOnScreen( 
                        register XmRowColumnWidget rowcol,
                        register Position *x,
                        register Position *y) ;
static void PopupSharedMenuShell( 
                        Widget cbwid,
                        Widget smwid,
                        XEvent *event) ;
static XtGeometryResult GeometryManager( 
                        Widget wid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged( 
                        Widget w) ;
static void Popdown( 
                        XmMenuShellWidget menushell,
                        XEvent *event) ;
static void PopdownKids( 
                        XmMenuShellWidget menushell,
                        XEvent *event) ;
static int SkipPopdown( 
                        XmCascadeButtonWidget cascade) ;
static void PopdownOne( 
                        Widget widget,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void PopdownEveryone( 
                        Widget widget,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void PopdownDone( 
                        Widget widget,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void ClearTraversalInternal( 
                        XmMenuShellWidget menushell,
                        XEvent *event) ;
static void Destroy( 
                        Widget wid) ;
static Widget _XmFindPopup( 
                        Widget widget,
                        String name) ;
static void _XmMenuPopupAction( 
                        Widget widget,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static void _XmMenuPopdownAction( 
                        Widget widget,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
static XmFontList GetTable(Widget wid,
			   XtEnum type);
static void CheckSetRenderTables(Widget wid,
				int offset,
				XrmValue *value); 
static XmDirection GetDirection(Widget);

/********    End Static Function Declarations    ********/

static XmConst char tk_error[] = "XtToolkitError" ;

static XtActionsRec actionsList[] = 
{
    {"MenuShellPopdownOne", PopdownOne},
    {"MenuShellPopdownDone", PopdownDone},
    {"XtMenuPopup", _XmMenuPopupAction},
    {"XtMenuPopdown", _XmMenuPopdownAction},
    {"MenuEscape",      _XmMenuEscape},
    {"ClearTraversal",       _XmClearTraversal}
};
static XtResource resources[] =
{
   {     XmNdefaultFontList,
         XmCDefaultFontList,
         XmRFontList,
         sizeof(XmFontList),
         XtOffsetOf( struct _XmMenuShellRec, menu_shell.default_font_list),
         XmRString,
         (XtPointer) NULL
   },

   {      XmNlabelFontList,
          XmCLabelFontList,
          XmRLabelRenderTable,
          sizeof(XmFontList),
          XtOffsetOf( struct _XmMenuShellRec, menu_shell.label_font_list),
          XmRCallProc,
          (XtPointer) CheckSetRenderTables
   },

   {      XmNbuttonFontList,
          XmCButtonFontList,
          XmRButtonRenderTable,
          sizeof(XmFontList),
          XtOffsetOf( struct _XmMenuShellRec, menu_shell.button_font_list),
          XmRCallProc,
          (XtPointer) CheckSetRenderTables
   },

   {      XmNlabelRenderTable,
          XmCLabelRenderTable,
          XmRLabelRenderTable,
          sizeof(XmRenderTable),
          XtOffsetOf( struct _XmMenuShellRec, menu_shell.label_font_list),
          XmRCallProc,
          (XtPointer)CheckSetRenderTables
   },

   {      XmNbuttonRenderTable,
          XmCButtonRenderTable,
          XmRButtonRenderTable,
          sizeof(XmRenderTable),
          XtOffsetOf( struct _XmMenuShellRec, menu_shell.button_font_list),
          XmRCallProc,
          (XtPointer)CheckSetRenderTables
   },

   {
          XmNlayoutDirection, 
	  XmCLayoutDirection, 
	  XmRDirection,
          sizeof(XmDirection), 
	  XtOffsetOf( struct _XmMenuShellRec, menu_shell.layout_direction),
	  XmRCallProc, 
	  (XtPointer) _XmDirectionDefault
   },

   {    /* default visual dynamically, see _XmDefaultVisualResources */   
          XtNvisual, 
	  XtCVisual, 
          XtRVisual, sizeof(Visual*),
	  XtOffsetOf(ShellRec, shell.visual), 
	  XtRImmediate, (XtPointer)INVALID_VISUAL
   },
   {
          XmNanimate, 
	  XmCAnimate, 
	  XmRBoolean, sizeof(Boolean), 
	  XtOffsetOf( struct _XmMenuShellRec, menu_shell.animate),
	  XmRImmediate, (XtPointer) False
   },           
};


static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    (XtInitProc)NULL,			/* InitializePrehook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPrehook	*/
    (XtInitProc)NULL,			/* InitializePosthook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    (XtInitProc)NULL,			/* secondaryCreate	*/
    (XmGetSecResDataFunc)NULL,	        /* getSecRes data	*/
    { 0 },				/* fastSubclass flags	*/
    (XtArgsProc)NULL,			/* get_values_prehook	*/
    (XtArgsProc)NULL,			/* get_values_posthook	*/

	/* ... */
};



externaldef(xmmenushellclassrec) XmMenuShellClassRec xmMenuShellClassRec = 
{
    {
        (WidgetClass) & overrideShellClassRec,  /* superclass */
        "XmMenuShell",                  /* class_name */
        sizeof (XmMenuShellWidgetRec),  /* widget size */
        ClassInitialize,                /* Class Initializer */
        ClassPartInitialize,            /* chained class init */
        FALSE,
        Initialize,                     /* instance init proc */
        (XtArgsProc)NULL,               /* init_hook proc */
        XtInheritRealize,               /* instance realize proc */
        actionsList,                    /* actions */
        XtNumber(actionsList),          /* num_actions */
        resources,                      /* resource list */
        XtNumber(resources),            /* resource_count */
        NULLQUARK,                      /* xrm_class */
        True,                           /* do compress_motion */
        XtExposeCompressMaximal,	/* do compress_exposure */
        TRUE,                           /* do compress enter-leave */
        FALSE,                          /* do have visible_interest */
        Destroy,                           /* instance widget destroyb */
        Resize,                         /* resize */
        XtInheritExpose,                /* expose */
        SetValues,                      /* set values proc */
        (XtArgsFunc)NULL,               /* set values hook proc */
        XtInheritSetValuesAlmost,       /* set_values_almost proc */
        (XtArgsProc)NULL,               /* get_values_hook */
        (XtAcceptFocusProc)NULL,        /* accept_focus proc */
        XtVersion,                      /* current version */
        NULL,                           /* callback offset    */
        default_translations,		/* translation table */
        (XtGeometryHandler)NULL,        /* query geo proc */
        (XtStringProc)NULL,             /* disp accelerator */
	(XtPointer)&baseClassExtRec,    /* extension */
    },
    {                       /* composite class record */
/* BEGIN OSF fix pir 2131 */
        GeometryManager,       		/* geo mgr */
/* END OSF fix pir 2131 */
        ChangeManaged,            /* have our own */
        InsertChild,                    /* and this too */
        DeleteChild,
	NULL,                           /* Extension */
    },
    {                       /* shell class record */
        NULL,                           /* extension */
    },
    {                       /* override shell class record */
        NULL,                           /* extension */
    },
    {                       /* menushell class record */
        PopdownOne,
	PopdownEveryone,
        PopdownDone,
        PopupSharedMenuShell,
        NULL,                           /* extension */
    },
};

externaldef(xmmenushellwidgetclass) WidgetClass xmMenuShellWidgetClass = 
   (WidgetClass) &xmMenuShellClassRec;


/* Trait record for MenuShell specify render table */

static XmConst XmSpecRenderTraitRec menushellSRT = {
  0,		/* version */
  GetTable,
};

static XmConst XmSpecifyLayoutDirectionTraitRec LayoutDirection = {
  0,			/* version */
  GetDirection
};

/* Save pointers for renderTable XmRCallProc */
static XmMenuShellWidget check_set_save = NULL;
static int check_set_offset1 = 0;
static int check_set_offset2 = 0;

/*
 * When using an override redirect window, it is safe to draw to the
 * window as soon as you have mapped it; you need not wait for exposure
 * events to arrive.  So ... to force menupanes to post quickly, we will
 * redraw all of the items now, and ignore the exposure events we receive
 * later.
 */
static void 
_XmFastExpose(
        register XmManagerWidget rowcol )
{
   register int i;
   register Widget child;

   /* Process the menupane */
   RC_SetExpose(rowcol, True);
   (*(XtClass(rowcol)->core_class.expose))((Widget) rowcol, NULL, NULL);

   /* Process each windowed child */
   for (i = 0; i < rowcol->composite.num_children; i++)
   {
      child = rowcol->composite.children[i];

      if (XtIsWidget(child) && 
          XtIsManaged(child))
      {
         (*(XtClass(child)->core_class.expose))(child, NULL, NULL);
      }
   }

   XFlush(XtDisplay(rowcol));

   /* Set one-shot, so menupane will ignore next expose */
   RC_SetExpose(rowcol, False);
}



/*
 * When unposting a group of cascading menupanes, we will first unmap all
 * of the MenuShell widgets, and then take care of the other work needed
 * to get the job done.  We do it this way, so that we can give the user
 * the fastest possible feedback.
 */
static void 
_XmFastPopdown(
        XmMenuShellWidget shell )
{
   if (RC_PopupPosted(shell->composite.children[0]))
       _XmFastPopdown((XmMenuShellWidget) RC_PopupPosted(shell->composite.children[0]));
   
   XtUnmapWidget(shell);
}


        
static void
slideCancel(Widget w, Widget slide)
{
    XtRemoveCallback(w, XmNunmapCallback, (XtCallbackProc)slideCancel, slide);
    XtDestroyWidget(slide);
}

static void
slideFinish(Widget slide, Widget w)
{
    XtRemoveCallback(w, XmNunmapCallback, (XtCallbackProc)slideCancel, slide);
}


static void
_XmPopupI(
    Widget      widget,
    XtGrabKind  grab_kind,
    Boolean     spring_loaded
    )
{
    register ShellWidget shell_widget = (ShellWidget) widget;

    if (! XtIsShell(widget)) {
	XtAppErrorMsg(XtWidgetToApplicationContext(widget),
		      "invalidClass", "xmPopup", tk_error, MESSAGE2,
		      (String *)NULL, (Cardinal *)NULL);
    }

    if (! shell_widget->shell.popped_up) {
	XtGrabKind call_data = grab_kind;
	XtCallCallbacks(widget, XtNpopupCallback, (XtPointer)&call_data);
	shell_widget->shell.popped_up = TRUE;
	shell_widget->shell.grab_kind = grab_kind;
	shell_widget->shell.spring_loaded = spring_loaded;
	if (shell_widget->shell.create_popup_child_proc != NULL) {
	    (*(shell_widget->shell.create_popup_child_proc))(widget);
	}
	if (grab_kind == XtGrabExclusive) {
	    _XmAddGrab(widget, TRUE, spring_loaded);
	} else if (grab_kind == XtGrabNonexclusive) {
	    _XmAddGrab(widget, FALSE, spring_loaded);
	}
	XtRealizeWidget(widget);	
        if (XmIsMenuShell(shell_widget) && ((XmMenuShellWidget)shell_widget)->menu_shell.animate)
	{
	    if (XmIsRowColumn(shell_widget->composite.children[0]))
	    {
	    Dimension width, height;
	    XtWidgetGeometry geo;
	    Widget slider;

		XtQueryGeometry(widget, NULL, &geo);
		width = geo.width;
		height = geo.height;
		switch (RC_Type(shell_widget->composite.children[0]))
		{
		case XmMENU_POPUP:
		    XtResizeWidget(widget, 1, 1, XtBorderWidth(widget));
		    break;
		case XmMENU_PULLDOWN:
		    XtResizeWidget(widget, width, 1, XtBorderWidth(widget));
		    break;
		default:
		    break;
		}
		slider = XtVaCreateWidget("MenuSlider", xmSlideContextWidgetClass, XmGetXmDisplay(XtDisplay(widget)),
		    XmNslideWidget, widget,
		    XmNslideDestWidth, width,
		    XmNslideDestHeight, height,
		    NULL);
		XtAddCallback(slider, XmNslideFinishCallback, (XtCallbackProc)slideFinish, shell_widget->composite.children[0]);
		XtAddCallback(shell_widget->composite.children[0], XmNunmapCallback, (XtCallbackProc)slideCancel, slider);
	    }
	}
	XMapRaised(XtDisplay(widget), XtWindow(widget));
    } else
	XRaiseWindow(XtDisplay(widget), XtWindow(widget));

} /* _XmPopupI */

void
_XmPopupSpringLoaded(
        Widget shell)
{   
  _XmPopupI( shell, XtGrabExclusive, True) ;
} 

void
_XmPopup(
        Widget shell,
        XtGrabKind grab_kind)
{   
  _XmPopupI( shell, grab_kind, FALSE) ;
} 

/*
 * Post the requested menu shell widget,
 */
static void 
PostMenuShell(
        XmMenuShellWidget menuShell,
        XtGrabKind grab_kind,
#if NeedWidePrototypes
        int spring_loaded )
#else
        Boolean spring_loaded )
#endif /* NeedWidePrototypes */
{
   XEvent event;
   XEvent *lastEvent;

   /* Remember the next event from the queue
      so we don't popdown on the replay */
   event.xbutton.type = ButtonPress;
   if ((lastEvent = XtLastEventProcessed(XtDisplay((Widget) menuShell)))
	== (XEvent *)NULL)
   {
      event.xbutton.serial = 
        XLastKnownRequestProcessed(XtDisplay((Widget) menuShell));
   }
   else
   {
      event.xbutton.serial = lastEvent->xany.serial;
   }
   event.xbutton.send_event = 0;
   event.xbutton.time = 
     XtLastTimestampProcessed(XtDisplay((Widget) menuShell));
   event.xbutton.display = XtDisplay((Widget)menuShell);
   _XmRecordEvent((XEvent *) &event);

   if (spring_loaded)
     {   
       /* XmPopupSpringLoaded ((Widget) menuShell); */
       /* Since the modality implementation requires the use of
        *   _XmAddGrab() instead of XtAddGrab(), XtPopupSpringLoaded()
        *   must be replaced with _XmPopupSpringLoaded(), which uses
        *   the appropriate AddGrab routine.
        */
       _XmPopupSpringLoaded( (Widget) menuShell) ;
     } 
   else
       _XmPopup ((Widget) menuShell, grab_kind);

   /* mark the row column as NOT popping down */
   RC_SetPoppingDown(menuShell->composite.children[0], False);
}

/*
 * Class Initialize.  Set up fast subclassing.
 */
static void 
ClassInitialize( void )
{
   _XmInitializeExtensions();
   baseClassExtRec.record_type = XmQmotif;
}

static XmDirection 
GetDirection(Widget w)
{
  return ((XmMenuShellWidget)w)->menu_shell.layout_direction;
}

static void 
ClassPartInitialize(
        WidgetClass wc )
{

   _XmFastSubclassInit (wc, XmMENU_SHELL_BIT);

    /* Install the specifyrendertable trait for all subclasses */
    XmeTraitSet((XtPointer)wc, XmQTspecifyRenderTable, 
		(XtPointer) &menushellSRT);
    XmeTraitSet(wc, XmQTspecifyLayoutDirection, (XtPointer) &LayoutDirection);

}
 


/*
 * Initialize routine
 */
/* ARGSUSED */
static void 
Initialize(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmFontList defaultFont;
    XmMenuShellWidget ms = (XmMenuShellWidget) new_w;

    new_w->core.background_pixmap = None;

    XtBorderWidth (new_w) = 0;            /* Style Guide says so */

    ms->menu_shell.focus_data = (XmFocusData)_XmCreateFocusData();

    ms->menu_shell.focus_policy = XmEXPLICIT;

    ms->shell.allow_shell_resize = TRUE;


    /* get reasonable defaults for visual, depth and colormap */
    _XmDefaultVisualResources(new_w);

    
    /* Fix 7961 - realization ordering causing badmatch */
    XtRealizeWidget (new_w);

    /* Assume, for now, that the application created this shell */
    ms->menu_shell.private_shell = False;

    defaultFont =  ms->menu_shell.button_font_list;

    if ( !defaultFont )
    {
	/* backward compatibility */
	defaultFont =  ms->menu_shell.default_font_list; 

	if ( !defaultFont )
	    defaultFont = XmeGetDefaultRenderTable( (Widget) ms, 
					    XmBUTTON_FONTLIST);
    }

    ms->menu_shell.button_font_list = XmFontListCopy (defaultFont);

    defaultFont =  ms->menu_shell.label_font_list;

    if ( !defaultFont )
    {
	/* backward compatibility */
	defaultFont =  ms->menu_shell.default_font_list; 
	if ( !defaultFont )
	    defaultFont = XmeGetDefaultRenderTable( (Widget) ms,
						XmLABEL_FONTLIST);
    }

    ms->menu_shell.label_font_list = XmFontListCopy (defaultFont);   

    if(ms->menu_shell.default_font_list != NULL)
        ms->menu_shell.default_font_list =
            XmFontListCopy(ms->menu_shell.default_font_list);   

    _XmSetSwallowEventHandler((Widget) ms, True);

    /*
     * Take over EventHandler in Shell, to dump the configure notify
     * events since it is not smart enough to ignore stale events.
     */
    XtInsertEventHandler((Widget) ms, (EventMask) StructureNotifyMask, True,
			 StructureNotifyHandler, (XtPointer) NULL,
			 XtListHead);

}

/* ARGSUSED */
static void 
StructureNotifyHandler(
        Widget wid,
        XtPointer closure,
        XEvent *event,
        Boolean *continue_to_dispatch )
{
    /*
     * This event handler is inserted at the head of the list so that
     * the configure notify events will be ignored.  This is because the
     * configures are done directly when needed, not when the event handler
     * is called.
     */
    if (event->type ==  ConfigureNotify)
    {
	/* all the configure notify events should be ignored */
	*continue_to_dispatch = False;
    }
}

/*
 * set values
 *
 * Don't allow the allowShellResize flag to be set false
 */
/* ARGSUSED */
static Boolean 
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
   XmMenuShellWidget new_w = (XmMenuShellWidget) nw ;
   XmMenuShellWidget old_w = (XmMenuShellWidget) cw ;
	XmFontList defaultFont;

   /* CR 7124: XmNlayoutDirection is a CG resource. */
   if (old_w->menu_shell.layout_direction != 
       new_w->menu_shell.layout_direction)
     {
       XmeWarning(nw, MESSAGE9);
       new_w->menu_shell.layout_direction = old_w->menu_shell.layout_direction;
     }

	if (new_w->menu_shell.button_font_list != 
		old_w->menu_shell.button_font_list)
	{
		XmFontListFree(old_w->menu_shell.button_font_list);
		defaultFont = new_w->menu_shell.button_font_list;
		if (!defaultFont)
			{
			defaultFont = new_w->menu_shell.default_font_list;
			if (!defaultFont )
			   defaultFont = XmeGetDefaultRenderTable( (Widget) new_w,
                                            XmBUTTON_FONTLIST);
			}
	new_w->menu_shell.button_font_list = XmFontListCopy (defaultFont);
	}

	if (new_w->menu_shell.label_font_list != 
		old_w->menu_shell.label_font_list)
	{
		XmFontListFree(old_w->menu_shell.label_font_list);
		defaultFont = new_w->menu_shell.label_font_list;
		if (!defaultFont)
			{
			defaultFont = new_w->menu_shell.default_font_list;
			if (!defaultFont )
			   defaultFont = XmeGetDefaultRenderTable( (Widget) new_w,
                                            XmLABEL_FONTLIST);
			}
	new_w->menu_shell.label_font_list = XmFontListCopy (defaultFont);
	}

   new_w->shell.allow_shell_resize = TRUE;
   return (TRUE);
}


/*
 * Resize our first child
 *
 * The statement pertaining to shared menupanes is commented out because
 * if the user is rapidly moving thru the menubar, an old configure notify
 * event may come in, cause the shell to resize the wrong child.
 */
static void 
Resize(
        Widget wid )
{
   XmMenuShellWidget ms = (XmMenuShellWidget) wid ;

   if (((ms->composite.num_children == 1) && 
        (XtIsManaged(ms->composite.children[0]))))
/*
     || ((ms->composite.num_children > 1) && (ms->shell.popped_up)))
*/
   {
      Widget child = ms->composite.children[0];

      XmeConfigureObject(child, -ms->core.border_width, -ms->core.border_width,
	 ms->core.width, ms->core.height, ms->core.border_width);
   }
}


/*
 * DeleteChild routine
 */
static void 
DeleteChild(
        Widget widget )
{
   XmMenuShellWidget parent = (XmMenuShellWidget)XtParent(widget);

   /* Remove the child as our tab group */
   XmRemoveTabGroup(widget);

   /* Let composite class finish the work */
   (*(((CompositeWidgetClass)compositeWidgetClass)->composite_class.
          delete_child))(widget);

   /*
    * If we had multiple children, but we've now dropped down to 1 child,
    * then we need to unmanage the child.
    */
   if (parent->composite.num_children == 1)
   { 
       Widget baseChild = parent->composite.children[0];
       if (!parent->shell.popped_up &&
                !(XmIsRowColumn(baseChild) && RC_TearOffActive(baseChild)))
           baseChild->core.managed = False;
       XMapWindow(XtDisplay(parent), XtWindow(baseChild));
   }
   else if ((parent->composite.num_children == 0) && 
            (parent->menu_shell.private_shell) &&
            (!parent->core.being_destroyed))
   {
       /* Destroy private shells, when they have no more children */
       XtDestroyWidget((Widget) parent);
   }
}

/*
 * InsertChild routine
 */
static void 
InsertChild(
        Widget widget )
{
    CompositeWidget parent = (CompositeWidget) widget->core.parent;
    int num_real_children, i;

    if (XmIsRowColumn (widget))
    {
       if ((RC_Type(widget) == XmMENU_PULLDOWN) ||
	   (RC_Type(widget) == XmMENU_POPUP))
       {
	  XtAddEventHandler (widget, EnterWindowMask, FALSE,
			     _XmEnterRowColumn, FALSE);
       }

       /*
        * add to composite list
        */

       (*((CompositeWidgetClass)compositeWidgetClass)->composite_class.
              insert_child) (widget);

       XtRealizeWidget(widget);

       /* Find the number of children excluding those being destroyed.
        * This fixes a problem with menus being dynamically destroyed
	* and added.  Before this fix the new menu was never mapped 
	* because the menu being destroyed was still counted as a child.
	*/
       for (i = 0, num_real_children = 0;
	    i < parent->composite.num_children; i++)
	 if (!parent->composite.children[i]->core.being_destroyed)
	   num_real_children++;

       /*
        * If only one child, then we will manage and unmanage it when we
        * want it to post or unpost.  If there are more than one child, then
        * we will have them always managed, and we will use a different
        * technique for posting and unposting.
        */
       if (num_real_children == 1)
       {
          XtSetKeyboardFocus(( Widget) parent, widget);
       }
       else if (num_real_children == 2)
       {
         /* if one is torn off, they don't have the same parent, so manage
         ** separately.
         */
         if (parent->composite.children[0] != parent->composite.children[1]) 
           {
             XtManageChild(parent->composite.children[0]);
             XtManageChild(parent->composite.children[1]);
           } 
         else 
           {
             XtManageChildren(parent->composite.children, 2);
           }
       }
       else
       {
          XtManageChild(widget);
       }
       if (num_real_children == 1)
       {
	  XMapWindow(XtDisplay(widget), XtWindow(widget));
       }
    }
    else
       XmeWarning(widget, ChildMsg);
}


static void 
ForceMenuPaneOnScreen(
        register XmRowColumnWidget rowcol,
        register Position *x,
        register Position *y )
{
   Position rightEdgeOfMenu, bottomEdgeOfMenu;
   Dimension dispWidth, dispHeight;
   Widget pulldown_button = RC_CascadeBtn(rowcol);
   Dimension RowColBorderWidth = rowcol->core.border_width << 1;
   Dimension CascadeBorderWidth = 0;

   if (pulldown_button)
      CascadeBorderWidth = pulldown_button->core.border_width << 1;

   /* Force the rowcol to be completely visible */

   rightEdgeOfMenu = *x + RowColBorderWidth + rowcol->core.width;
   bottomEdgeOfMenu = *y + RowColBorderWidth + rowcol->core.height;
   dispWidth = WidthOfScreen (XtScreen(rowcol));
   dispHeight = HeightOfScreen (XtScreen(rowcol));

   /*
    * For OPTION menus, if the submenu is [partially] offscreen, offset it
    * off the button.
    */
   if (pulldown_button && XtParent(pulldown_button) &&
	XmIsRowColumn(XtParent(pulldown_button)) &&
       (RC_Type(XtParent(pulldown_button)) == XmMENU_OPTION))
   {
      Position old_x = *x;
      if (bottomEdgeOfMenu >= (Position)dispHeight)
      {
          *y = dispHeight - rowcol->core.height - RowColBorderWidth - 1;
          if (LayoutIsRtoLM(rowcol))
             *x = old_x - rowcol->core.width - (rowcol->core.border_width <<1);
          else
	    *x = old_x + pulldown_button->core.width + CascadeBorderWidth;
	  rightEdgeOfMenu = *x + RowColBorderWidth + rowcol->core.width;
          bottomEdgeOfMenu = *y + RowColBorderWidth + rowcol->core.height;
      }

      if (*y < 0)
      {
          *y = 0;

	  /* Consider CascadeBtn as well as RowCol width to allow multi
	   * column RowColumn
	   */
          if (LayoutIsRtoLM(rowcol))
             *x = old_x - rowcol->core.width - (rowcol->core.border_width <<1);
          else
	    *x = old_x + pulldown_button->core.width + CascadeBorderWidth;
	  rightEdgeOfMenu = *x + RowColBorderWidth + rowcol->core.width;
          bottomEdgeOfMenu = *y + RowColBorderWidth + rowcol->core.height;
      }

      if (rightEdgeOfMenu >= (Position)dispWidth)
      {
	  *x = old_x - rowcol->core.width + RowColBorderWidth;
	  rightEdgeOfMenu = *x + RowColBorderWidth + rowcol->core.width;
      }

      if (*x < 0)
      {
          if (LayoutIsRtoLM(rowcol))
             *x = old_x + pulldown_button->core.width + CascadeBorderWidth;
          else
	    *x = old_x + pulldown_button->core.width + CascadeBorderWidth;
	  rightEdgeOfMenu = *x + RowColBorderWidth + rowcol->core.width;
      }
   }

   /*
    * If the submenu is offscreen force it completely on.
    */
   if (rightEdgeOfMenu >= (Position)dispWidth)
       *x -= (rightEdgeOfMenu - dispWidth + 1);

   if (bottomEdgeOfMenu >= (Position)dispHeight)
   {
      if (pulldown_button && XtParent(pulldown_button) &&
		XmIsRowColumn(XtParent(pulldown_button)) &&
	  (RC_Type(XtParent(pulldown_button)) == XmMENU_BAR))
      {
	 Position y_temp = *y;

	 /* this menu pane is being pulled down from a bar */
	 /* it doesn't fit, so we have to place it above   */
	 /* the bar -- if it will fit                      */

	 y_temp -= (CascadeBorderWidth
		    + pulldown_button->core.height
		    + RowColBorderWidth
		    + rowcol->core.height + 1);
			
	 if (y_temp > 0)
	     *y = y_temp;
      }
      else
	  *y -= (bottomEdgeOfMenu - dispHeight + 1);
   }

   /* Make sure that the top left corner os on screen! */
   if (*x < 0)
       *x = 0;

   if (*y < 0)
       *y = 0;
}

/*
 * Method for posting a multi-paned menushell
 */
static void 
PopupSharedMenuShell(
        Widget cbwid,
        Widget smwid,
        XEvent *event )
{
   XmCascadeButtonWidget cascadebtn = (XmCascadeButtonWidget) cbwid ;
   XmRowColumnWidget submenu = (XmRowColumnWidget) smwid ;
   register XmMenuShellWidget popup = (XmMenuShellWidget) XtParent(submenu);
   XmRowColumnWidget parent_menu;
   Position x, y;
   Dimension height, width;
   int _index = 0;
   register int i;
   Boolean popped_up = popup->shell.popped_up;
   XmRowColumnWidget old_rowcol = NULL;
   XmCascadeButtonWidget old_cascadebtn = NULL;
   XmMenuState mst = _XmGetMenuState((Widget)submenu);
   Time _time = _XmGetDefaultTime(cbwid, event);
   XmMenuSystemTrait menuSTrait;

   menuSTrait = (XmMenuSystemTrait) 
     XmeTraitGet((XtPointer) XtClass((Widget) submenu), XmQTmenuSystem);

   /* Find out which child is trying to get posted */
   for (i = 0; i < popup->composite.num_children; i++)
   {
      if (popup->composite.children[i] == (Widget)submenu)
      {
	 _index = i;
	 break;
      }
   }

   /* Swap only if the submenu is not already child[0] */
   if (_index != 0) 
   {
      /* Swap places in the children list */
      old_rowcol = (XmRowColumnWidget) popup->composite.children[0];
      old_cascadebtn = (XmCascadeButtonWidget)RC_CascadeBtn(old_rowcol);

      RC_SetPoppingDown(old_rowcol, True);
      menuSTrait -> disarm((Widget) old_rowcol);
      /* RC_SetArmed (old_rowcol, False); */
      popup->composite.children[_index] = (Widget)old_rowcol;
      popup->composite.children[0] = (Widget)submenu;
      RC_SetPoppingDown(submenu, False);

      if (RC_TornOff(old_rowcol))
	 _XmRestoreTearOffToToplevelShell((Widget)old_rowcol, event);
      else
	 XUnmapWindow(XtDisplay(old_rowcol), XtWindow(old_rowcol));
   } else
      if (cascadebtn != (XmCascadeButtonWidget)
	      RC_CascadeBtn(popup->composite.children[0]))
      {
	 /* If the 2 different cascade buttons popped up this submenu,
	  * unmap it in preparation for the 'move'.  Also save the old
	  * cascade so it can be unhighlighted.
	  */
         old_cascadebtn = (XmCascadeButtonWidget)RC_CascadeBtn(submenu);
	 XUnmapWindow(XtDisplay(submenu), XtWindow(submenu));
      }

   if (popped_up ||
       (old_cascadebtn && RC_TornOff(XtParent(old_cascadebtn)) && 
	!XmIsMenuShell(XtParent(XtParent(old_cascadebtn)))))
   {
      XmCascadeButtonHighlight( (Widget) old_cascadebtn, False);
   }

   if (submenu->core.being_destroyed)
       return;

   /* It seems that menupanes that share their menushell are always managed.
    * When reposting a tear off, the restoration code unmanages the pane.
    * So set it here!
    */
   submenu->core.managed = TRUE;

   if (menuSTrait)
     menuSTrait -> cascade((Widget) submenu, (Widget) cascadebtn, event);

   /*
    * Set us as the active tab group for our parent
    */
   _XmSetActiveTabGroup(popup->menu_shell.focus_data, (Widget)submenu);

   /*
    * get the shell widget in sync with the menu widget
    * We keep the menu at 0,0 and shell at the menu's position.
    * the menu's position is just used as a place to keep shell's loc
    *
    * NOTE: the menu may be moved down slightly to take into account the
    * tear off control.
    */

   /* Always adjust to the size of the menupane */
   width = XtWidth(submenu);
   height = XtHeight(submenu);

   if (RC_WidgetHasMoved (submenu))     /* but make sure it didn't */
   {                                    /* just move to where it */
                                        /* is now.  Happens with */
                                        /* pulldowns a lot */
      x = XtX(submenu);
      y = XtY(submenu);
      
      ForceMenuPaneOnScreen(submenu,&x,&y);
      
      XtX (submenu) = XtY (submenu) = (-1 * XtBorderWidth(submenu));

      if (RC_WindowHasMoved(submenu))
      {
	 XMoveWindow (XtDisplay(submenu), XtWindow(submenu), 
		      XtX(submenu), XtY(submenu));
	 RC_SetWindowMoved (submenu, FALSE);
      }

      RC_SetWidgetMoved (submenu, FALSE);
   }
   else
   {
      x = XtX(popup);
      y = XtY(popup);
   }
   
   XmeConfigureObject((Widget) popup, x, y, width, height, popup->core.border_width);

   XMapWindow(XtDisplay(submenu), XtWindow(submenu));

   if (popped_up && 
       !((old_rowcol == submenu) && (cascadebtn == old_cascadebtn)))
      _XmCallRowColumnUnmapCallback((Widget)old_rowcol, event);

   _XmCallRowColumnMapCallback((Widget)submenu, event);

   /*
    * if there is a tear off control, set the initial focus to the first
    * real child.  If none of the children is traversible, let the
    * traversal code set it to the toc.
    */
   if (RC_TearOffControl(submenu) && XtIsManaged(RC_TearOffControl(submenu)))
   {
       for (i=0; i < submenu->composite.num_children; i++)
       {
	   if (XmIsTraversable(submenu->composite.children[i]))
	   {
	       _XmSetInitialOfTabGroup( (Widget) submenu,
				       submenu->composite.children[i]);
	       break;
	   }
       }
   }

   if (menuSTrait) menuSTrait -> arm((Widget) submenu);

   if (popped_up)
      _XmFastExpose((XmManagerWidget) submenu);
   else
   {
      /*  
       * we are the root of the chain and better do the exclusive grab
       */
      parent_menu = (XmRowColumnWidget) XtParent (cascadebtn);

      if (RC_Type(parent_menu) == XmMENU_OPTION)
      {

	 /* Don't post the menu if the menu cannot control grabs! Note:
          * grabs done twice. You cannot grab on a non viewable window.
          */
         if (_XmMenuGrabKeyboardAndPointer((Widget)parent_menu, _time) !=
              GrabSuccess)
         {
            return;
         }

	 /* Remember the time in case this is just a BSelect Click */
	 if ((event->type == ButtonPress) || (event->type == ButtonRelease))
	    mst->MS_LastManagedMenuTime = event->xbutton.time;

         PostMenuShell(popup, XtGrabExclusive, True);
         _XmFastExpose((XmManagerWidget) submenu);

	 _XmMenuFocus(XtParent(submenu), XmMENU_BEGIN, _time);

           /** the real grab ***/
         _XmMenuGrabKeyboardAndPointer((Widget)submenu, _time);
      }
      else
      {
	 if ((RC_Type(parent_menu) == XmMENU_BAR) &&
	     (RC_BeingArmed(parent_menu)))
	 {
	    /* Don't post the menu if the menu cannot control grabs! */
	    if (_XmMenuGrabKeyboardAndPointer((Widget)parent_menu, _time) !=
		GrabSuccess)
	    {
		return;
	    }
									      
	    RC_SetBeingArmed(parent_menu, False);
	 } 

	 PostMenuShell(popup, XtGrabNonexclusive, False);
	 _XmFastExpose((XmManagerWidget) submenu);

	 /* We always must move the focuse for the leaf menu pane */
	 _XmMenuFocus(XtParent(submenu), XmMENU_MIDDLE, _time);
      }
   }

   /*
    * now highlight the pulldown entry widget that
    * pulled down the menu
    */
   XmCascadeButtonHighlight ((Widget) cascadebtn, TRUE);

   if (popped_up)
   {
      XmGadget activeChild;
      
      activeChild = (XmGadget)old_rowcol->manager.active_child;
      
      /*
       * If the active child was a gadget, then we need to 
       * handle the sending of the focus out ourselves.
       */
      if (activeChild && XmIsGadget(activeChild))
      {
	 _XmDispatchGadgetInput((Widget) activeChild, NULL, 
	    XmFOCUS_OUT_EVENT);
      }
   }
}                       

/* BEGIN OSF fix pir 2131 */
/* This is ripped off from Xt/Shell.c and modified to also deal with X
and Y
 * requests.
 */
/*
 * This is gross, I can't wait to see if the change happened so I will
ask
 * the window manager to change my size and do the appropriate X work.
 * I will then tell the requester that he can.  Care must be taken
because
 * it is possible that some time in the future the request will be
 * asynchronusly denied and the window reverted to it's old size/shape.
*/

static XtGeometryResult 
GeometryManager( 
	Widget wid,
	XtWidgetGeometry *request,
	XtWidgetGeometry *reply )
{
  ShellWidget shell = (ShellWidget)(wid->core.parent);
  XtWidgetGeometry my_request;

  if(shell->shell.allow_shell_resize == FALSE && XtIsRealized(wid))
    return(XtGeometryNo);

  if(!XtIsRealized((Widget)shell)){
    if (request->request_mode & (CWX | CWY)) {
      return(XtGeometryNo);
    }
    *reply = *request;
    if(request->request_mode & CWWidth)
      wid->core.width = shell->core.width = request->width;
    if(request->request_mode & CWHeight)
      wid->core.height = shell->core.height = request->height;
    if(request->request_mode & CWBorderWidth)
      wid->core.border_width = shell->core.border_width =
        request->border_width;
    return(XtGeometryYes);
  }

  /* %%% worry about XtCWQueryOnly */
  my_request.request_mode = 0;
  if (request->request_mode & CWX) {
    my_request.x = request->x;
    my_request.request_mode |= CWX;
  }
  if (request->request_mode & CWY) {
    my_request.y = request->y;
    my_request.request_mode |= CWY;
  }
  if (request->request_mode & CWWidth) {
    my_request.width = request->width;
    my_request.request_mode |= CWWidth;
  }
  if (request->request_mode & CWHeight) {
    my_request.height = request->height;
    my_request.request_mode |= CWHeight;
  }
  if (request->request_mode & CWBorderWidth) {
    my_request.border_width = request->border_width;
    my_request.request_mode |= CWBorderWidth;
  }
  /*
   * Don't make geometry request if the shared menupane has a
   * visible pulldown, which isn't the current pulldown widget. This prevents
   * another pulldown menu to resize the menu shell's width and height, thus
   * dynamically resizing the visible pulldown.
   */
  if ((wid != shell->composite.children[0])
      || (XtMakeGeometryRequest((Widget)shell, &my_request, NULL)
	       == XtGeometryYes)) {
    if (request->request_mode & CWX) {
      wid->core.x = 0;
    }
    if (request->request_mode & CWY) {
      wid->core.y = 0;
    }
    if (request->request_mode & CWWidth) {
      wid->core.width = request->width;
    }
    if (request->request_mode & CWHeight) {
      wid->core.height = request->height;
    }
    if (request->request_mode & CWBorderWidth) {
      wid->core.border_width = request->border_width;
      wid->core.x = wid->core.y = -request->border_width;
    }
    return XtGeometryYes;
  } else return XtGeometryNo;
}
/* END OSF fix pir 2131 */


/*
 * ChangeManaged
 */
static void 
ChangeManaged(
        Widget w )
{
   register XmMenuShellWidget popup = (XmMenuShellWidget) w;
   XmRowColumnWidget parent_menu;
   Position x, y;
   Dimension height, width;
   XmCascadeButtonWidget cascadebtn;
   register Widget child;
   register XmRowColumnWidget rowcol = 
       (XmRowColumnWidget)popup->composite.children[0];
   int i;
   XmMenuState mst = _XmGetMenuState((Widget)w);
   XmMenuSystemTrait menuSTrait;
   Time _time = XtLastTimestampProcessed(XtDisplay(w));

   mst->RC_ButtonEventStatus.waiting_to_be_managed = FALSE;

   /* Don't handle multi-paned shells here */
   if (popup->composite.num_children > 1)
       return;

   if (rowcol->core.being_destroyed)
       return;

   if (XtIsManaged ((Widget)rowcol)) 
   {
      if  ((RC_Type(rowcol) == XmMENU_PULLDOWN) &&
	   (! RC_CascadeBtn(rowcol)))
      {
	 /*
	  * this pulldown does not have a complete path to
	  * a toplevel pane, so it can not be managed
	  */
	 XmeWarning(w, ManageMsg);
	 XtUnmanageChild ((Widget) rowcol);
	 return;
      } 
      else  
         if (RC_Type(rowcol) == XmMENU_POPUP) 
         {
	    /* Verify MenuPost/WhichButton */
            if ((mst->RC_ButtonEventStatus.time ==
                    XtLastTimestampProcessed(XtDisplay(rowcol))) &&
                 !mst->RC_ButtonEventStatus.verified)
	    {
	       mst->RC_ButtonEventStatus.verified = False;
	       XtUnmanageChild ((Widget) rowcol);
	       return;
	    }
	    /* Remember the time in case this is just a BMenu Click */
	    mst->MS_LastManagedMenuTime = mst->RC_ButtonEventStatus.time;
	 }

      /* Set us as the active tab group for our parent */
      _XmSetActiveTabGroup(popup->menu_shell.focus_data, (Widget)rowcol);
	 
      /* make the callback now to give the application a crack at
       * repositioning the pane.
       */
      _XmCallRowColumnMapCallback((Widget)rowcol, 
	 (XEvent *)&mst->RC_ButtonEventStatus.event);
      
      /*
       * get the shell widget in sync with the menu widget
       * We keep the menu at 0,0 and shell at the menu's position.
       * the menu's position is just used as a place to keep shell's loc
       *
       * If a tear off control is relevant, move the pane down to allow for
       * its height.
       */

      width = XtWidth(rowcol);
      height = XtHeight(rowcol);
      
      if (RC_WidgetHasMoved (rowcol))   
      {                                 
	 x = XtX(rowcol);
	 y = XtY(rowcol);
	      
         ForceMenuPaneOnScreen(rowcol,&x,&y);
	 
	 XtX (rowcol) = XtY (rowcol) = (-1 * XtBorderWidth(rowcol));

	 if (RC_WindowHasMoved(rowcol))
	 {
	    XMoveWindow (XtDisplay(rowcol), XtWindow(rowcol), 
			 XtX(rowcol), XtY(rowcol));
	    RC_SetWindowMoved (rowcol, FALSE);
	 }
	      
	 RC_SetWidgetMoved (rowcol, FALSE);
      }
      else
      {
	 x = XtX(popup);
	 y = XtY(popup);
      }
	 
      XmeConfigureObject((Widget) popup, x, y, width, height,
			popup->core.border_width);
	 
      /*
       * if there is a tear off control, set the initial focus to the first
       * real child.  If none of the children is traversible, let the
       * traversal code set it to the toc.
       */
      if (RC_TearOffControl(rowcol) && XtIsManaged(RC_TearOffControl(rowcol)))
      {
	  for (i=0; i < rowcol->composite.num_children; i++)
	  {
	      if (XmIsTraversable(rowcol->composite.children[i]))
	      {
		  _XmSetInitialOfTabGroup( (Widget) rowcol,
					  rowcol->composite.children[i]);
		  break;
	      }
	  }
      }
      
      menuSTrait = (XmMenuSystemTrait) 
	XmeTraitGet((XtPointer) XtClass((Widget) rowcol), XmQTmenuSystem);

      menuSTrait -> arm((Widget) rowcol);

      /*
       * now figure out exactly how to pop it up
       */
	 
      switch (RC_Type(rowcol))
      {
       case XmMENU_POPUP:

	 /* Don't post the menu if the menu cannot control grabs! Note:
	  * grabs done twice. You cannot grab on a non viewable window.
	  */
	 if (RC_CascadeBtn(rowcol))     /* Doggone loophole! */
	 {
	   if (_XmMenuGrabKeyboardAndPointer(RC_CascadeBtn(rowcol), _time) !=
		 GrabSuccess)
	   {
		 /** this is the fix for cmvc 6688 (CDExc15902) ***/
	     XtUnmanageChild((Widget) rowcol);
		 /** end of fix for 6688 ****/
	     return;
	   }
	 }

	 PostMenuShell((XmMenuShellWidget) w, XtGrabExclusive, True);
	 _XmFastExpose((XmManagerWidget) rowcol);

	       /** the real grab ***/
         _XmMenuGrabKeyboardAndPointer((Widget)rowcol, _time);
	 _XmMenuFocus(XtParent(rowcol), XmMENU_BEGIN, _time);

	 /* To support menu replay, keep the pointer in sync mode */
	 XAllowEvents(XtDisplay(rowcol), SyncPointer, CurrentTime);

	 menuSTrait -> controlTraversal((Widget) rowcol, FALSE);
	 break;

       case XmMENU_PULLDOWN:
	 cascadebtn = (XmCascadeButtonWidget) RC_CascadeBtn(rowcol);
	 parent_menu = (XmRowColumnWidget) XtParent (cascadebtn);

	 /* Make sure the correct item gets the focus */
	 if (!_XmGetInDragMode((Widget) rowcol))
	 {
	    if (RC_MemWidget(rowcol) && 
		(RC_Type(parent_menu) == XmMENU_OPTION))
	    {
	       if (XtParent(RC_MemWidget(rowcol)) == (Widget)rowcol)
		  _XmSetInitialOfTabGroup( (Widget) rowcol,
					  RC_MemWidget(rowcol));

	       else
	       {
		  /* Find our link to the memory widget */
		  child = (Widget)RC_MemWidget(rowcol);
		  while (child)
		  {
		     if (XtParent(child) == (Widget)rowcol)
			 break;
		     
		     child = (Widget)RC_CascadeBtn(XtParent(child));
		  }
		  rowcol->manager.active_child =  child;
	       }
	    }
	    else
		rowcol->manager.active_child = NULL;
	    
	    if ((parent_menu->manager.active_child != (Widget)cascadebtn) &&
		((RC_Type(parent_menu) == XmMENU_POPUP) ||
		 (RC_Type(parent_menu) == XmMENU_PULLDOWN)))
	    {
	       /* Move the focus to the child */
	       _XmMgrTraversal((Widget) cascadebtn, XmTRAVERSE_CURRENT);
	    }
	 }
	 
	 if (RC_Type(parent_menu) == XmMENU_OPTION)
	 {

	    /* Don't post the menu if the menu cannot control grabs! Note:
	     * grabs done twice. You cannot grab on a non viewable window.
	     */
	    if (_XmMenuGrabKeyboardAndPointer((Widget)parent_menu, _time) !=
			GrabSuccess)
	    {
	        XtUnmanageChild((Widget) rowcol);
	        return;
	    }

	    /* Remember the time in case this is just a BSelect Click */
	    mst->MS_LastManagedMenuTime = mst->RC_ButtonEventStatus.time;

	    PostMenuShell((XmMenuShellWidget) w, XtGrabExclusive, True);
	    _XmFastExpose((XmManagerWidget) rowcol);
	      
	    /* Removed fixing bug #1260 (Yuriy Syrota, 2004-07-29)
	    _XmMenuFocus(XtParent(rowcol), XmMENU_BEGIN, _time); */

                    /**the real grab *****/
            _XmMenuGrabKeyboardAndPointer((Widget)rowcol, _time);
	    _XmMenuFocus(XtParent(rowcol), XmMENU_BEGIN, _time);

	    /* To support menu replay, keep the pointer in sync mode */
	    XAllowEvents(XtDisplay(rowcol), SyncPointer, CurrentTime);
	 }
	 else
	 {
	    /* For a menubar, put the pointer grab on the menubar. */
	    if ((RC_Type(parent_menu) == XmMENU_BAR) &&
		(RC_BeingArmed(parent_menu)))
	    {
	       if (_XmMenuGrabKeyboardAndPointer((Widget)parent_menu, _time)
					 !=GrabSuccess) 
	       {
	           XtUnmanageChild((Widget) rowcol);
		   return;
	       }

	       RC_SetBeingArmed(parent_menu, False);
	    }

	    PostMenuShell((XmMenuShellWidget) w, XtGrabNonexclusive, False);
	    _XmFastExpose((XmManagerWidget) rowcol);

	    /* We always must grab the keyboard for the leaf menu pane */
	    _XmMenuFocus(XtParent(rowcol), XmMENU_MIDDLE, _time);
	    XtSetKeyboardFocus(XtParent(rowcol), (Widget) rowcol);
	    
	    /*
	     * now highlight the pulldown entry widget that
	     * pulled down the menu
	     */
	    XmCascadeButtonHighlight ((Widget) cascadebtn, TRUE);
	    break;
	 }
      }
   }
   else
   {
      /* popdown anything that is still up */
      _XmMenuFocus(w, XmMENU_END, _time);
      
      (* (((XmMenuShellClassRec *)(popup->
	 core.widget_class))->menu_shell_class.popdownEveryone))
	    ((Widget) popup, NULL, NULL, NULL);
      
      if (RC_Type(rowcol) == XmMENU_POPUP)
      {
	 XtUngrabPointer(w, _time);
      }
   }
   mst->RC_ButtonEventStatus.verified = False;
}                       


void
_XmPopdown(
        Widget widget)
{   
  register ShellWidget shell_widget = (ShellWidget) widget;

  if (! XtIsShell(widget)) {
    XtAppErrorMsg(XtWidgetToApplicationContext(widget),
		  "invalidClass", "xmPopdown", tk_error, MESSAGE3,
		  (String *)NULL, (Cardinal *)NULL);
  }

  if (shell_widget->shell.popped_up) {
    XtGrabKind grab_kind = shell_widget->shell.grab_kind ;
    XWithdrawWindow( XtDisplay( shell_widget), XtWindow( shell_widget),
		    XScreenNumberOfScreen( XtScreen( shell_widget))) ;
    if(    grab_kind != XtGrabNone    )
      {
	_XmRemoveGrab((Widget) shell_widget) ;
      }
    shell_widget->shell.popped_up = FALSE ;
    XtCallCallbacks((Widget) shell_widget, XtNpopdownCallback, (XtPointer) &grab_kind) ;
  } 
}

/*
 * actually popdown a menuShell widget, also flag as unmanaged
 * its menu child and unhighlight the cascadebutton that popped up this
 * widget
 */
static void 
Popdown(
        XmMenuShellWidget menushell,
        XEvent *event )
{
    XmRowColumnWidget  rowcol = (XmRowColumnWidget) 
                                 menushell->composite.children[0];
    XmMenuSystemTrait menuSTrait;

    if (menushell->shell.popped_up)
    {
        /* mark the row column as popping down */
        RC_SetPoppingDown (rowcol, True);

        /* XmPopdown ((Widget) menushell); */
        /* Since the MenuShell may be popped up spring-loaded, 
         *   and since XmPopdown will remove the grab using XtRemoveGrab
         *   if the shell is spring loaded, and since we must use
         *   _XmRemoveGrab because of the modality implementation, we
         *   must use our own version of _XmPopdown to pop down the
         *   MenuShell.
         */
        _XmPopdown( (Widget) menushell) ;

	menuSTrait = (XmMenuSystemTrait) 
	  XmeTraitGet((XtPointer) XtClass((Widget) rowcol), XmQTmenuSystem);
	menuSTrait -> disarm((Widget) rowcol);

        ClearTraversalInternal(menushell, event);

        /* Don't unmanage if we're sharing the menu shell */
        if (menushell->composite.num_children == 1)
           rowcol->core.managed = FALSE;

	_XmCallRowColumnUnmapCallback((Widget)rowcol, event);

	/* Restore tear offs - except for Popups 'cause the ungrabs are 
	 * yet to occur
	 */
	if ((RC_Type(rowcol) != XmMENU_POPUP) || RC_TornOff(rowcol))
	   _XmRestoreTearOffToToplevelShell((Widget)rowcol, event);
    }
}


/*
 * Popdown all the popup kids of this widget, do bottom to top popdown.
 */
static void 
PopdownKids(
        XmMenuShellWidget menushell,
        XEvent *event )
{
   ShellWidget subShell;

   if ((subShell = (ShellWidget)
	RC_PopupPosted(menushell->composite.children[0])) != NULL)
   {
      Widget rowcol = subShell->composite.children[0];

      /* mark this row colum as popping down */
      RC_SetPoppingDown (rowcol, True);
    
      PopdownKids ((XmMenuShellWidget) subShell, event);
      Popdown ((XmMenuShellWidget) subShell, event);
   }
}


/*
 * This routine determines if there is an enter event pending which will
 * just pop back up the row column that is about to be popped down.
 */
static int 
SkipPopdown(
        XmCascadeButtonWidget cascade )
{
    XEvent  event;

    /*
     * check if an enter event is pending.  
     */
    if (XPending (XtDisplay (cascade)))
    {
        XPeekEvent (XtDisplay (cascade), &event);

        if (event.type == EnterNotify)
        {
            XEnterWindowEvent * enterevent = (XEnterWindowEvent *) &event;

            if (XtWindow (cascade) == enterevent->window)
                     return (TRUE);
        }
    }

    return (FALSE);
}


/* 
 * event handler for entering on a row column widget.  The widget must
 * be either a pulldown or popup menu.
 */
/* ARGSUSED */
void 
_XmEnterRowColumn(
        Widget widget,
        XtPointer closure,
        XEvent *event,
        Boolean *cont )
{
    XmRowColumnWidget cascade;
    Widget cascadebtn;
    Time _time = _XmGetDefaultTime(widget, event);
    
    XmRowColumnWidget rowcol = (XmRowColumnWidget) widget;
    XmMenuShellWidget menushell = (XmMenuShellWidget) XtParent(rowcol);
    XEnterWindowEvent *enterevent = (XEnterWindowEvent *) event;

    /* 
     * Ignore popdown requests if traversal is on, or we're no longer
     * visible, or we don't have any submenus up.
     */
    if (!_XmGetInDragMode(widget)         ||
	(!menushell->shell.popped_up)     ||
	(! RC_PopupPosted(rowcol)))
	return;

    cascade = (XmRowColumnWidget)
	((CompositeWidget) RC_PopupPosted(rowcol))->composite.children[0];
    
    cascadebtn = RC_CascadeBtn (cascade);

    /*
     * Make sure that popdown should not be skipped.  It should be skipped
     * if the mouse has moved into the cascadebutton (widget or gadget)
     * that will just pop it back up again.
     */
    if (XmIsCascadeButtonGadget(cascadebtn) &&
        (cascadebtn == XmObjectAtPoint( (Widget) rowcol,
				       enterevent->x, enterevent->y)))
    {
       return;
    }
    else if (XmIsCascadeButton(cascadebtn))
    {
       if (SkipPopdown ((XmCascadeButtonWidget) cascadebtn))
	  return;
       else
       {
	  Position x, y;

	  XtTranslateCoords(cascadebtn, 0, 0, &x, &y);
	  if ( (enterevent->x_root >= x)  && 
	       (enterevent->x_root < (x + XtWidth(cascadebtn))) &&
	       (enterevent->y_root >= y)  && 
	       (enterevent->y_root < (y + XtHeight(cascadebtn))))
	  {
	     return;
	  }
       }
    }

    _XmMenuFocus(XtParent(rowcol), XmMENU_MIDDLE, _time);
    PopdownKids (menushell, event);
}


static void 
PopdownOne(
        Widget widget,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmMenuShellWidget ms = (XmMenuShellWidget)widget;
    XmMenuShellWidget tms;
    XmRowColumnWidget rowcol;

    /* Find a menu shell, this may be a menushell, rowcol or button */
    /* OR (!) a top level shell! */
    while (ms && !XtIsShell(ms))
         ms = (XmMenuShellWidget)XtParent(ms);

    if (ms && !XmIsMenuShell(ms))
    {
       /* prevent coredumps when pulldown menu isn't realized yet */
       if (ms->core.managed)
           _XmDismissTearOff((Widget) ms, (XtPointer) event, (XtPointer) NULL);
       return;
    }

    if (ms == NULL)
        return;

    /* Find the toplevel MenuShell 
     */
    _XmGetActiveTopLevelMenu (ms->composite.children[0], (Widget *) &rowcol);
    tms = ms;
    if (RC_Type(rowcol) == XmMENU_BAR)
        tms = (XmMenuShellWidget) RC_PopupPosted(rowcol);
    else if ((RC_Type(rowcol) == XmMENU_POPUP) ||
	     ((RC_Type(rowcol) == XmMENU_PULLDOWN) &&
	      !XmIsMenuShell(XtParent( rowcol))))
    {
        tms = (XmMenuShellWidget) XtParent(rowcol);
	if (!XmIsMenuShell(tms))	/* torn! */
	{
	   /* if the next pane up is the torn popup, bring down entire menu */
	   if (rowcol == (XmRowColumnWidget)XtParent(
	     RC_CascadeBtn(ms->composite.children[0])))
	      ms = tms = (XmMenuShellWidget)RC_ParentShell(rowcol);
	}
    }

    if (ms == tms)
       (* (((XmMenuShellClassRec *)(ms->core.widget_class))->
		menu_shell_class.popdownDone))
		   (widget, event, params, num_params);
    else
    {
          (* (((XmMenuShellClassRec *)(ms->core.widget_class))->
		menu_shell_class.popdownEveryone))
			((Widget) ms, event, params, num_params);
    }

    if (event)
       _XmRecordEvent(event);
}
    

/*
 * Class function used to unpost all menupanes
 */

/*
 * popdown this all the popup children, and then me, ie. bottom to top
 *
 * called from ChangeManaged and PopdownDone
 */
/*ARGSUSED*/
static void 
PopdownEveryone(
        Widget widget,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    Widget rowcol;
    XmMenuShellWidget shell = (XmMenuShellWidget) widget;

    /* Ignore this event, if already processed */
    if ( event && !_XmIsEventUnique(event))
       return;
    /* 
     * If only a portion of the hierarchy is being popped down, then
     * make sure the keyboard grab gets reset to the right window.
     */
    rowcol = shell->composite.children[0];

    /* Only reset the focus if the this is not a tear off or the cascaded
     * submenu's parent is not a tear off.
     */
    if ((RC_Type(rowcol) == XmMENU_PULLDOWN) &&
        XmIsMenuShell(XtParent(rowcol)) &&
        RC_CascadeBtn(rowcol) &&
        XmIsMenuShell(XtParent(XtParent(RC_CascadeBtn(rowcol)))) )
    {
       _XmMenuFocus(XtParent(XtParent(RC_CascadeBtn(rowcol))),
		    XmMENU_MIDDLE, _XmGetDefaultTime(widget, event));
    }

    /* mark this row colum as popping down */
    RC_SetPoppingDown (rowcol, True);

    if (shell->shell.popped_up) 
    {
       if (XmIsMenuShell(shell))
       {
	  _XmFastPopdown((XmMenuShellWidget) widget);
	  XFlush(XtDisplay(widget));
	  PopdownKids ((XmMenuShellWidget) widget, event); /* do our kids */
	  Popdown ((XmMenuShellWidget) widget ,event);     /* do us */
       }
       else	/* torn - don't popdown the popup! */
       {
	  PopdownKids ((XmMenuShellWidget) widget, event); /* do our kids */
       }
    }
}


/*
 * Class function used to unpost all menupanes.  
 * Note that to catch BSelect in a gadget and popdown correctly, widget param
 * must be the LEAF node menushell, rowcol, or button.
 */
static void 
PopdownDone(
        Widget widget,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XmMenuShellWidget ms = (XmMenuShellWidget)widget;
    XmRowColumnWidget  rowcol;
    XmMenuState mst = _XmGetMenuState((Widget)widget);
    Time _time = _XmGetDefaultTime(widget, event);
    XmMenuSystemTrait menuSTrait;

    /* Ignore this event, if already processed */
    if (event && !_XmIsEventUnique(event))
       return;

    /* Find a menu shell, this may be a menushell, rowcol or button */
    while (ms && !XmIsMenuShell(ms))
         ms = (XmMenuShellWidget)XtParent(ms);

    if (ms == NULL)
	return;

    _XmGetActiveTopLevelMenu (ms->composite.children[0], (Widget *) &rowcol);

    menuSTrait = (XmMenuSystemTrait) 
      XmeTraitGet((XtPointer) XtClass((Widget) rowcol), XmQTmenuSystem);

    /* We used to check the event,  but not anymore (CUA) */
    /* if (event && ((event->xbutton.type==ButtonPress) || */
    /*     (event->xbutton.type==ButtonRelease))) */
    /* { */
    /*    if ( !(_XmMatchBtnEvent( event, XmIGNORE_EVENTTYPE, */
    /*             RC_PostButton(rowcol), RC_PostModifiers(rowcol)) || */
    /*            _XmMatchBSelectEvent(ms->composite.children[0], event )) ) */
    /*    { */
    /* To support menu replay, keep the pointer in sync mode */
    /*       XAllowEvents(XtDisplay(rowcol), SyncPointer, _time); */
    /*       return; */
    /*    } */
    /* } */

    /* Take menus out of drag mode in either case of click into traversal or
     * unpost, unless the menu is a torn-off pane.
     */
    if ((RC_Type(rowcol) == XmMENU_BAR) ||
	(RC_Type(rowcol) == XmMENU_OPTION) ||
	XmIsMenuShell(XtParent(rowcol)))
    {
	_XmSetInDragMode((Widget) rowcol, False);
    }

    /* This is to enable Option and Popup Menus to post on BMenu/BSelect Click.
     * By this time we know that the button is valid.  We need to check
     * if the post event occured within a short time span.  Next we force 
     * the menu into traversal mode.  And, of course, we return immediately
     * without popping down the pane.
     */
     if (RC_popupMenuClick(rowcol) &&
	 event &&
	 ((event->type == ButtonPress) || (event->type == ButtonRelease)) &&
	 ((event->xbutton.time - mst->MS_LastManagedMenuTime) < 
	  XtGetMultiClickTime(XtDisplay(ms))))	/* or 150 ms? */
     {
	if (RC_Type(rowcol) == XmMENU_OPTION)
	{
	   if (!XmProcessTraversal(RC_MemWidget(rowcol), XmTRAVERSE_CURRENT))
	      XmProcessTraversal(RC_OptionSubMenu(rowcol), XmTRAVERSE_CURRENT);

	   /* To support menu replay, keep the pointer in sync mode */
	   XAllowEvents(XtDisplay(rowcol), SyncPointer, CurrentTime);
	   return;
	}
	else
	   if (!rowcol->manager.highlighted_widget)
	   {
              XmProcessTraversal((Widget) rowcol, XmTRAVERSE_CURRENT);

	      /* To support menu replay, keep the pointer in sync mode */
	      XAllowEvents(XtDisplay(rowcol), SyncPointer, CurrentTime);
	      return;
	   }
     }

    /* 
     * If in a menubar or popup, get the toplevel menushell 
     * CHECK OPTION MENU esp w/ cascading submenus, JAY!
     */
    if (RC_Type(rowcol) == XmMENU_BAR)
    {
	/* It's possible that we got here via an accelerator and the menu
	 * isn't popped up - so ms could be NULL!
	 */
	if ( (ms = (XmMenuShellWidget) RC_PopupPosted(rowcol)) == NULL )
	   return;
    } 
    else if ((RC_Type(rowcol) == XmMENU_POPUP) || RC_TornOff(rowcol))
	ms = (XmMenuShellWidget) XtParent(rowcol);

    _XmMenuFocus( (Widget) ms, XmMENU_END, _time);
    
    if (XmIsMenuShell(ms))
    {
       (* (((XmMenuShellClassRec *)(ms->
	  core.widget_class))->menu_shell_class.popdownEveryone))
	     ((Widget) ms, event, params, num_params);
    }
    else
    {
       (* (((XmMenuShellClassRec *)(RC_ParentShell(rowcol)->
	  core.widget_class))->menu_shell_class.popdownEveryone))
	     ((Widget) ms, event, params, num_params);
    }

    /* if its a menubar, clean it up and disarm it */
    if (RC_Type(rowcol) == XmMENU_BAR)
    {
      menuSTrait -> menuBarCleanup((Widget) rowcol);
    } 
    else if ((RC_Type(rowcol) == XmMENU_POPUP) && RC_TornOff(rowcol))
	_XmRestoreTearOffToToplevelShell((Widget)rowcol, event);

    menuSTrait -> disarm((Widget) rowcol);

    /* Cleanup grabs */
    XtUngrabPointer((Widget) ms, _time);
}

/*ARGSUSED*/
static void
ClearTraversalInternal(
        XmMenuShellWidget menushell,
        XEvent *event)		/* unused */
{
   Widget activeChild;
   XmRowColumnWidget rowcol;

   rowcol = (XmRowColumnWidget)menushell->composite.children[0];

   /* Clean up traversal */
   activeChild = rowcol->manager.active_child;
   if (activeChild)
   {
      if (XmIsGadget(activeChild))
      {
	 (*((XmLabelGadgetClass)XtClass(activeChild))->
	    gadget_class.border_unhighlight) (activeChild);
      } 
      else
      {
	 (*((XmLabelWidgetClass)XtClass(activeChild))->
	    primitive_class.border_unhighlight) (activeChild);
      }
   }

   /*
    * (Internal state update) Clear focus from unposting submenu.
    */
   _XmClearFocusPath((Widget) rowcol);

   if ((RC_Type(rowcol) != XmMENU_POPUP) && RC_CascadeBtn(rowcol))
   {
/*
We can't discern whether this is in traversal mode (or in traversal mode going
into drag mode) so don't do the unhighlighting here.  Push it up a level to
PushB and ToggleB and ???
*/
      /* This unhighlight is a dragmode only requirement.  */
      if (_XmGetInDragMode((Widget)rowcol))
         XmCascadeButtonHighlight (RC_CascadeBtn(rowcol), FALSE);
      RC_PopupPosted (XtParent(RC_CascadeBtn(rowcol))) = FALSE;
   }
}


/*
 * XmRCallProc routine for checking MenuShell render table resources
 * before setting them to NULL if no value is specified
 * for both XmN<foo>renderTable and XmN<foo>fontList.
 * If save == msw and offset matches a previously set offset, then the
 * function has been called twice on same widget and resource offset, thus
 * resource needs to be set NULL, otherwise leave it alone.
 */
/* ARGSUSED */
static void 
CheckSetRenderTables(Widget wid,
		     int offset,
		     XrmValue *value )
{
  XmMenuShellWidget msw = (XmMenuShellWidget)wid;

  if (check_set_save == msw)
    {
      if ((offset == check_set_offset1) ||
	  (offset == check_set_offset2))
	{
	  value->addr = NULL;
	}
      else if (check_set_offset2 == 0)
	{
	  check_set_offset2 = offset;
	  value->addr = ((char *)msw + offset);
	}
    }
  else
    {
      check_set_offset1 = offset;
      value->addr = ((char *)msw + offset);
      check_set_offset2 = 0;
    }
  
  check_set_save = msw;
}


/****************************************************************
 *
 * Trait method for specify render table 
 *
 **************************************************************/
static XmFontList 
GetTable(
	  Widget wid,
	  XtEnum type)
{
    XmMenuShellWidget menu = (XmMenuShellWidget) wid ;

    switch(type) {
    case XmLABEL_RENDER_TABLE : return menu->menu_shell.label_font_list ;
    case XmBUTTON_RENDER_TABLE : return menu->menu_shell.button_font_list ;
    case XmTEXT_RENDER_TABLE : return menu->menu_shell.default_font_list ;
    }
   
    return NULL ;
}



/*
 * Clear traversal in the associated menu hierarchy
 */
/*ARGSUSED*/
void 
_XmClearTraversal(
        Widget wid,
        XEvent *event,
        String *params,		/* unused */
        Cardinal *num_params )	/* unused */
{
    XmMenuShellWidget ms = (XmMenuShellWidget) wid ;
    XmRowColumnWidget rowcol;
    XmMenuState mst = _XmGetMenuState((Widget)wid);
   
    if (!_XmIsEventUnique(event))
	return;

    /* if a valid event */
    if ((ms->composite.num_children != 0) &&
	(rowcol = (XmRowColumnWidget)ms->composite.children[0]) &&
	(_XmMatchBtnEvent( event, RC_PostEventType(rowcol),
			  RC_PostButton(rowcol),
			  RC_PostModifiers(rowcol)) ||
	(_XmMatchBSelectEvent((Widget)rowcol, event ))))
    {
	XButtonPressedEvent *xbutton = (XButtonPressedEvent *) event;

	/*
	 * if this is the Popup's posting event, ignore it.  It is just
	 * being sent to the spring loaded widget by Xt's dispatch
	 * after the post.  This may not have been recorded.
	 *
	 * This is also reached through an Mwm edge case. Be sure to 
	 * thaw the queue or pointer deadlock occurs.
	 */
	if ((RC_Type(rowcol) == XmMENU_POPUP) &&
	     (mst->MS_LastManagedMenuTime == xbutton->time))
	{
	    XAllowEvents(XtDisplay(ms), SyncPointer, 
			 _XmGetDefaultTime(wid, event));
	    return;
	}

	_XmHandleMenuButtonPress (ms->composite.children[0], event);
    }
    else
       XAllowEvents(XtDisplay(ms), SyncPointer, CurrentTime);
}


/*
 * Public Routine
 */
Widget 
XmCreateMenuShell(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
   return (XtCreatePopupShell (name, xmMenuShellWidgetClass, parent, al, ac));
}

static void 
Destroy(
        Widget wid )
{
    XmMenuShellWidget ms = (XmMenuShellWidget) wid ;
    _XmDestroyFocusData(ms->menu_shell.focus_data);
    if (ms->menu_shell.default_font_list != NULL) 
       XmFontListFree (ms->menu_shell.default_font_list);

    if (ms->menu_shell.button_font_list != NULL) 
       XmFontListFree (ms->menu_shell.button_font_list);

    if (ms->menu_shell.label_font_list != NULL) 
       XmFontListFree (ms->menu_shell.label_font_list);    

    /* Clear pointers for renderTable XmRCallProc */
    check_set_save = NULL;
    check_set_offset1 = 0;
    check_set_offset2 = 0;
}

static Widget
_XmFindPopup(
    Widget widget,
    String name)
{
    register Cardinal i;
    register XrmQuark q;
    register Widget w;

    q = XrmStringToQuark(name);

    for (w=widget; w != NULL; w=w->core.parent)
	for (i=0; i<w->core.num_popups; i++)
	    if (w->core.popup_list[i]->core.xrm_name == q)
		return w->core.popup_list[i];

    return NULL;
}

static void
_XmMenuPopupAction(
    Widget widget,
    XEvent *event,
    String *params,
    Cardinal *num_params)
{
    Boolean spring_loaded;
    register Widget popup_shell;

    if (*num_params != 1) {
	XtAppWarningMsg(XtWidgetToApplicationContext(widget),
			"invalidParameters", "xtMenuPopupAction", tk_error,
			MESSAGE4, (String *)NULL, (Cardinal *)NULL);
	return;
    }

    if (event->type == ButtonPress)
	spring_loaded = True;
    else if (event->type == KeyPress || event->type == EnterNotify)
	spring_loaded = False;
    else {
	XtAppWarningMsg(XtWidgetToApplicationContext(widget),
			"invalidPopup","unsupportedOperation",tk_error,
			MESSAGE5, (String *)NULL, (Cardinal *)NULL);
	spring_loaded = False;
      }

    popup_shell = _XmFindPopup(widget, params[0]);
    if (popup_shell == NULL) {
	XtAppWarningMsg(XtWidgetToApplicationContext(widget),
			"invalidPopup","xtMenuPopup",tk_error,
			MESSAGE6, params, num_params);
	return;
    }

    if (spring_loaded) _XmPopupI(popup_shell, XtGrabExclusive, TRUE);
    else _XmPopupI(popup_shell, XtGrabNonexclusive, FALSE);
}

/*ARGSUSED*/
static void
_XmMenuPopdownAction(
    Widget widget,
    XEvent *event,		/* unused */
    String *params,
    Cardinal *num_params)
{
    Widget popup_shell;

    if (*num_params == 0) {
	_XmPopdown(widget);
    } else if (*num_params == 1) {
	popup_shell = _XmFindPopup(widget, params[0]);
	if (popup_shell == NULL) {
            XtAppWarningMsg(XtWidgetToApplicationContext(widget),
			    "invalidPopup","xtMenuPopdown",tk_error,
			    MESSAGE7, params, num_params);
	    return;
	}
	_XmPopdown(popup_shell);
    } else {
	XtAppWarningMsg(XtWidgetToApplicationContext(widget),
			"invalidParameters","xtMenuPopdown", tk_error,
			MESSAGE8, (String *)NULL, (Cardinal *)NULL);
    }
}

