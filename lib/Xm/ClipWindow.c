/* $TOG: ClipWindow.c /main/12 1999/10/13 16:16:13 mgreess $ */
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
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/ClipWindowP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VirtKeysP.h>	/* for XmeVirtualToActualKeysyms */
#include <Xm/TraitP.h>
#include "RepTypeI.h"
#include "ScrolledWI.h"
#include "TraversalI.h"
#include "XmI.h"
#include "ClipWindTI.h"

/********    Static Function Declarations    ********/

static String GetRealTranslations( 
                        Display *dpy,
			XmConst _XmBuildVirtualKeyStruct *keys,
                        int num_keys) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass w_class) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Resize( 
                        Widget w) ;
static void Redisplay( 
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static XtGeometryResult GeometryManager( 
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged( 
                        Widget wid) ;
static void DeleteChild( 
                        Widget w) ;
static XmNavigability WidgetNavigable( 
                        Widget wid) ;
static void ActionGrab( 
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;

/********    End Static Function Declarations    ********/



/**************
 *
 *  Translation tables management for ClipWindow:
 *  ---------------------------------------------
 *  We need 2 because of the grabactions stuff that requires a real
 *  one up front and the event dispatch process that requires a virtual one.
 *  The real one is derived from the virtual one in GetRealTranslation.
 *  It is set in Initialize, before realize time.
 *  The virtual one is set up once per widget in Redisplay.
 *  One the Xt problem is fixed, setting the _XmClipWindowTranslationTable
 *  directly should be enough (+ still registering the grab). The
 *  override cuisine + the getreal will go away.
 **************/


/**************
 *
 * Only one action that takes as parameter the name of the action
 *  to call in the scrolledwindow action list (where the paging is done)
 * dependency: the SW<move> names in ActionGrab have to
 *  match the action name in the ScrolledWindow.
 *
 **************/

static XtActionsRec actions[] =
{
 {"ActionGrab",    ActionGrab}, 
};


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,				/* InitializePrehook	*/
    NULL,				/* SetValuesPrehook	*/
    NULL,				/* InitializePosthook	*/
    NULL,				/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    NULL,				/* secondaryCreate	*/
    NULL,               		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    NULL,				/* getValuesPrehook	*/
    NULL,				/* getValuesPosthook	*/
    NULL,                               /* classPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    NULL                                /* focusChange          */
};

externaldef( xmclipwindowclassrec) XmClipWindowClassRec xmClipWindowClassRec =
{
   {			/* core_class fields      */
      (WidgetClass) &xmDrawingAreaClassRec,	/* superclass         */
      "XmClipWindow",				/* class_name         */
      sizeof(XmClipWindowRec),			/* widget_size        */
      ClassInitialize,	        		/* class_initialize   */
      ClassPartInitialize,			/* class_part_init    */
      FALSE,					/* class_inited       */
      Initialize,       			/* initialize         */
      NULL,					/* initialize_hook    */
      XtInheritRealize,			        /* realize            */
      actions,				        /* actions	      */
      XtNumber(actions),	                /* num_actions	      */
      NULL,				        /* resources          */
      0,			                /* num_resources      */
      NULLQUARK,				/* xrm_class          */
      TRUE,					/* compress_motion    */
      FALSE,					/* compress_exposure  */
      TRUE,					/* compress_enterlv   */
      FALSE,					/* visible_interest   */
      NULL,			                /* destroy            */
      Resize,        			        /* resize             */
      Redisplay,	       		        /* expose             */
      NULL,                		        /* set_values         */
      NULL,					/* set_values_hook    */
      XtInheritSetValuesAlmost,	        	/* set_values_almost  */
      NULL,					/* get_values_hook    */
      NULL,					/* accept_focus       */
      XtVersion,				/* version            */
      NULL,					/* callback_private   */
      NULL,                     	        /* tm_table           */
      NULL,                    	                /* query_geometry     */
      NULL,             	                /* display_accelerator*/
      (XtPointer)&baseClassExtRec,              /* extension          */
   },
   {		/* composite_class fields */
      GeometryManager,    	                /* geometry_manager   */
      ChangeManaged,	                	/* change_managed     */
      XtInheritInsertChild,			/* insert_child       */
      DeleteChild,     		                /* delete_child       */
      NULL,                                     /* extension          */
   },

   {		/* constraint_class fields */
      NULL,					/* resource list        */   
      0,					/* num resources        */   
      sizeof(XmManagerConstraintRec),		/* constraint size      */   
      NULL,					/* init proc            */   
      NULL,					/* destroy proc         */   
      NULL,					/* set values proc      */   
      NULL,                                     /* extension            */
   },

   {		/* manager_class fields */
      NULL,			                /* translations           */
      NULL,				        /* syn_resources      	  */
      0,			                /* num_get_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           */    
   },

   {		/* DrawingArea class - none */     
      0						/* mumble */
   },

   {		/* ClipWindow class - none */     
      NULL					/* extension */
   }	
};

externaldef( xmclipwindowwidgetclass) WidgetClass xmClipWindowWidgetClass
                                       = (WidgetClass) &xmClipWindowClassRec ;



/************************************************************************
 *                                                                      *
 * GetRealTranslations - Builds up a "real" translation table out of    *
 * virtual keysyms.                                                     *
 * Well, not exactly out of a virtual translation table, but rather a 
 * special struct, a _XmBuildVirtualKeyStruct that maches the 
 * clipwindow virtual translation found in Transltns.c:

   ":c <Key>osfBeginLine:ActionGrab(SWTopLine)\n\
      :<Key>osfBeginLine:ActionGrab(SWBeginLine)\n\
      :c <Key>osfEndLine:ActionGrab(SWBottomLine)\n\
        :<Key>osfEndLine:ActionGrab(SWEndLine)\n\
       :<Key>osfPageLeft:ActionGrab(SWLeftPage)\n\
       :c <Key>osfPageUp:ActionGrab(SWLeftPage)\n\
      :<Key>osfPageRight:ActionGrab(SWRightPage)\n\
     :c <Key>osfPageDown:ActionGrab(SWRightPage)\n\
         :<Key>osfPageUp:ActionGrab(SWUpPage)\n\
       :<Key>osfPageDown:ActionGrab(SWDownPage)";

 * This is just because it is simpler that way, we don't have to
 * parse a translation table string from scratch.
 *
 * cons: anytime the above changes in Transltns.c, ClipWindowKeys, 
 * defined below, has to be updated.
 *
 ************************************************************************/

/*  _XmBuildVirtualKeyStruct is declared in XmP.h */

static XmConst _XmBuildVirtualKeyStruct ClipWindowKeys[] = {
     {ControlMask, XmVosfBeginLine, "ActionGrab(SWTopLine)\n"},
     {0,           XmVosfBeginLine, "ActionGrab(SWBeginLine)\n"},
     {ControlMask, XmVosfEndLine,   "ActionGrab(SWBottomLine)\n"},
     {0,           XmVosfEndLine,   "ActionGrab(SWEndLine)\n"},
     {0,           XmVosfPageLeft,  "ActionGrab(SWLeftPage)\n"},
     {ControlMask, XmVosfPageUp,    "ActionGrab(SWLeftPage)\n"},
     {0,           XmVosfPageRight, "ActionGrab(SWRightPage)\n"},
     {ControlMask, XmVosfPageDown,  "ActionGrab(SWRightPage)\n"},
     {0,           XmVosfPageUp,    "ActionGrab(SWUpPage)\n"},
     {0,           XmVosfPageDown,  "ActionGrab(SWDownPage)\n"}
};

#define MAX_CLIPWINDOW_TM_SIZE 1000  /* roughly 10lines * 100chars */

static String
GetRealTranslations(
        Display *dpy,
        XmConst _XmBuildVirtualKeyStruct *keys,
        int num_keys )
{
  static char buf[MAX_CLIPWINDOW_TM_SIZE]; /* memory used externally */
  char *tmp = buf;
  char *keystring;
  Cardinal i;
  int num_vkeys;
  XmKeyBinding vkeys;
  KeySym    keysym;
  Modifiers mods;
  
  *tmp = '\0';
  for (i = 0; i < num_keys; i++) {
    keysym = XStringToKeysym(keys[i].key);
    if (keysym == NoSymbol) break;
    
    /* A virtual keysym may map to multiple real keysyms. */
    num_vkeys = XmeVirtualToActualKeysyms(dpy, keysym, &vkeys);
    while (--num_vkeys >= 0)
      {
	keystring = XKeysymToString(vkeys[num_vkeys].keysym);
	if (!keystring) break;
	
	/* this is why the struct is simpler than a pure translation parser,
	   we have to merge the modifiers */
	mods = vkeys[num_vkeys].modifiers | keys[i].mod;
	
	if (mods & ControlMask)
	  strcat(tmp, "Ctrl ");
	if (mods & ShiftMask)
	  strcat(tmp, "Shift ");
	if (mods & Mod1Mask)
	  strcat(tmp, "Mod1 "); /* "Alt" may not be always right */
	
	strcat(tmp,"<Key>");
	strcat(tmp, keystring);
	strcat(tmp,": ");
	strcat(tmp, keys[i].action); /* actions contain line separators. */

	tmp += strlen(tmp);
	assert((tmp - buf) < MAX_CLIPWINDOW_TM_SIZE);
      }

    XtFree((char*) vkeys);
  }
  
  return buf;
}


/****************************************************************/
static XtTranslations ClipWindowTranslations = NULL;
static void 
ClassInitialize( void )
{   
    /* can't do that static in the base extextion record */
    baseClassExtRec.record_type = XmQmotif ;
    
    /* register the grab action related to the paging. at this
       point the widget has a real translation table */
    XtRegisterGrabAction(ActionGrab,  True, KeyPressMask | KeyReleaseMask, 
			 GrabModeAsync, GrabModeAsync); 

    ClipWindowTranslations = XtParseTranslationTable(_XmClipWindowTranslationTable);
}

/****************************************************************/
static XmConst XmClipWindowTraitRec clipWindowData = {
     0                         /* version         */
};

static void 
ClassPartInitialize(
        WidgetClass w_class )
{   
    _XmFastSubclassInit(w_class, XmCLIP_WINDOW_BIT) ;

    /* Install the clipWindow trait for all subclasses as well. */
    XmeTraitSet((XtPointer)w_class, _XmQTclipWindow, (XtPointer)&clipWindowData);
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
    XmClipWindowWidget cw = (XmClipWindowWidget) nw ;
    static XtTranslations ClipWindowXlations = NULL;

    /* force some DA resources */
    cw->manager.shadow_thickness = 0 ;
    cw->core.border_width = 0 ;
    cw->drawing_area.margin_width = 0 ;
    cw->drawing_area.margin_height = 0 ;

    /* I could leave this one for more bc support,
       but it will force me to special case the DA SetValues method ... 
    cw->drawing_area.resize_policy = XmRESIZE_SWINDOW ; */ 

    cw->clip_window.flags = 0 ;
    /* flags used in the ClipWindow to track that
       the widget hasn't gotten yet its real translation table.
       This will be changed in Redisplay, after realization, and the flag
       will be reset to 1 there */

    /* Here we create the real translation table out of the virtual one,
       parse it and attach it to the clipwindow */
    /* This has to be done in Initialize because we need the particular
       display of the widget, not a default display (which is available
       in ClassInitialize) */

    /* do the parsing only once */
    
    if (!ClipWindowXlations) {
	ClipWindowXlations = XtParseTranslationTable(
					GetRealTranslations(
					        XtDisplay(nw),
						ClipWindowKeys,
						XtNumber(ClipWindowKeys)));
    }

    XtOverrideTranslations(nw, ClipWindowXlations);

    /* need for the RtoL win gravity resize support */
    cw->clip_window.old_width = cw->core.width ;
}

/*************************************************************************
 *
 *  Resize: implement north east win gravity for all kids.
 *          I wish X would do it for me, but setting win_gravity
 *          on a window only affect this window...
 *
 *************************************************************************/
static void 
Resize(
        Widget w )
{
    XtWidgetProc resize;

    /* When layout is rtol,
       implement win gravity north east for all kids */

    if (LayoutIsRtoLM(w)) {
	XmClipWindowWidget cw = (XmClipWindowWidget) w ;
	Cardinal i ;

	for (i = 0; i < cw->composite.num_children; i++) {
	    Widget child ;
	    child = cw->composite.children[i] ;

	    if (XtIsManaged(child)) { 
		/* we want the distance from the x of the child to the
		   width of the clipwindow to remain constant */
		Dimension d ;
		d = cw->clip_window.old_width - child->core.x ;
		XmeConfigureObject(child, 
				   cw->core.width  - d,
				   child->core.y,
				   child->core.width, 
				   child->core.height,
				   child->core.border_width);
	    }
	}
	cw->clip_window.old_width = cw->core.width ;
    }

    /* For compatibility invoke the drawing area's XmNresizeCallback. */
    _XmProcessLock();
    resize = ((CoreWidgetClass) xmClipWindowClassRec.core_class.superclass)->
	core_class.resize;
   _XmProcessUnlock();
    (* resize) (w);
}

  
/************************************************************************
 *									*
 *  Redisplay - 	                                                *
 *									*
 ************************************************************************/
static void 
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    XmClipWindowWidget cw = (XmClipWindowWidget) wid ;
    XtExposeProc expose;


    /* Here we're doing something once for every instance of ClipWindow.
       if the clipwindow translation hasn't been override yet, do it:
       install the virtual translation */
    /* There are other way to track that moment, like use an exposecallback 
       that removes itself right away, or a map notify action, this one
       is clean and simple, no hackery of the translation table, not much 
       additional code */
    if (cw->clip_window.flags == 0) {
	cw->clip_window.flags = 1 ;
	
    	XtOverrideTranslations (wid, ClipWindowTranslations);
    }
    

    /* envelop our superclass expose method */
    _XmProcessLock();
    expose = ((CoreWidgetClass) xmClipWindowClassRec.core_class.superclass)->
	core_class.expose;   
    _XmProcessUnlock();
    (* expose)  (wid, event, region);
}


/****************************************************************
 *
 * GeometryManager:
 *      Always accpet the change and notify the ScrollFrame parent 
 *
 ****************************************************************/
 
/*ARGSUSED*/
static XtGeometryResult 
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply ) /* unused */
{
    /* notify the navigators of the change thru
       the appropriate Trait's methods */
    Widget clip = XtParent(w);

    if (IsQueryOnly(request)) return XtGeometryYes;

    /* The clipwindow is making the change to the child geometry */
    if (IsX(request)) w->core.x = request->x;
    if (IsY(request)) w->core.y = request->y;
    if (IsWidth(request)) w->core.width = request->width;
    if (IsHeight(request)) w->core.height = request->height;
    if (IsBorder(request)) w->core.border_width = request->border_width;

    /* notify the ScrolledWindow parent by using a direct call */
    _XmSWNotifyGeoChange(XtParent(clip), w, request);

    return XtGeometryYes;
}


/****************************************************************
 *
 * ChangeManaged
 *
 *
 ****************************************************************/
static void 
ChangeManaged(
        Widget wid )
{
    /* notify the ScrolledWindow */
    _XmSWNotifyGeoChange(XtParent(wid), NULL, NULL);

    XmeNavigChangeManaged(wid) ;
}

/************************************************************************
 *									*
 *  DeleteChild: track the workwindow death				*
 *									*
 ************************************************************************/
static void 
DeleteChild(
        Widget child )
{
    XtWidgetProc delete_child;

    /* looks like we need a Trait here as well, at least we don't
       need the 1.2 destroy callback stuff anymore */
    XmScrolledWindowWidget sw;

    sw = (XmScrolledWindowWidget)child->core.parent->core.parent;
    
    if (child == sw->swindow.WorkWindow)
        sw->swindow.WorkWindow = NULL;

    /* remove it from the child list by enveloping */
    _XmProcessLock();
    delete_child = ((CompositeWidgetClass) 
		    xmClipWindowClassRec.core_class.superclass)->
			composite_class.delete_child;
    _XmProcessUnlock();
    (* delete_child) (child);
}


/****************************************************************
 * Xm private class method
 ****************/
static XmNavigability
WidgetNavigable(
        Widget wid)
{   
    /* do we really need this method ? */
    if(    XtIsSensitive(wid)
       &&  ((XmManagerWidget) wid)->manager.traversal_on    )
	{   
	    XmNavigationType nav_type = ((XmManagerWidget) wid)
		->manager.navigation_type ;

	    if(    (nav_type == XmSTICKY_TAB_GROUP)
	       ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	       ||  (    (nav_type == XmTAB_GROUP)
		    &&  !_XmShellIsExclusive( wid))    )
		{
		    return XmDESCENDANTS_TAB_NAVIGABLE ;
		}
	    return XmDESCENDANTS_NAVIGABLE ;
	}
    return XmNOT_NAVIGABLE ;

}


/************************************************************************
 *                                                                      *
 * Grab routine - The first param is the name of the action             *
 *                in the ScrolledWindow parent
 *                                                                      *
 ************************************************************************/
static void 
ActionGrab(
        Widget wid,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
    XtCallActionProc(XtParent(wid), params[0], event, params, *num_params);
}


