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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: DialogS.c /main/17 1996/03/25 17:50:11 barstow $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/DialogSP.h>
#include <Xm/DialogSEP.h>
#include <Xm/BaseClassP.h>
#include <Xm/DialogSavvyT.h>
#include <Xm/TraitP.h>
#include "BaseClassI.h"
#include "MessagesI.h"
#include "XmImI.h"


#define MSG1	_XmMMsgDialogS_0000


#define HALFDIFF(a, b) ((((Position)a) - ((Position)b))/2)

#define TotalWidth(w)   (XtWidth  (w) + (2 * (XtBorderWidth (w))))
#define TotalHeight(w)  (XtHeight (w) + (2 *(XtBorderWidth (w))))



/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInit( 
                        WidgetClass wc) ;
static Widget GetRectObjKid( 
                        CompositeWidget p) ;
static void Initialize( 
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues( 
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InsertChild( 
                        Widget w) ;
static void GetDefaultPosition( 
                        Widget child,
                        Widget parent,
                        Position *xRtn,
                        Position *yRtn) ;
static void ChangeManaged( 
                        Widget wid) ;
static XtGeometryResult GeometryManager( 
                        Widget wid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;

/********    End Static Function Declarations    ********/


static XmBaseClassExtRec	myBaseClassExtRec = {
    NULL,				/* Next extension	*/
    NULLQUARK,				/* record type XmQmotif	*/
    XmBaseClassExtVersion,		/* version		*/
    sizeof(XmBaseClassExtRec),		/* size			*/
    XmInheritInitializePrehook,		/* initialize prehook	*/
    XmInheritSetValuesPrehook,		/* set_values prehook	*/
    XmInheritInitializePosthook,	/* initialize posthook	*/
    XmInheritSetValuesPosthook,		/* set_values posthook	*/
    (WidgetClass)&xmDialogShellExtClassRec,/* secondary class	*/
    XmInheritSecObjectCreate,		/* secondary create	*/
    NULL,				/* getSecRes data	*/
    {0}				/* fast subclass	*/
};


externaldef(xmdialogshellclassrec)
XmDialogShellClassRec xmDialogShellClassRec = {
    {					    /* core class record */
	
	(WidgetClass) & transientShellClassRec,	/* superclass */
	"XmDialogShell", 		/* class_name */
	sizeof(XmDialogShellWidgetRec), /* widget_size */
	ClassInitialize,		/* class_initialize proc */
	ClassPartInit,			/* class_part_initialize proc */
	FALSE, 				/* class_inited flag */
	Initialize, 			/* instance initialize proc */
	NULL, 				/* init_hook proc */
	XtInheritRealize,		/* realize widget proc */
	NULL, 				/* action table for class */
	0, 				/* num_actions */
	NULL,	 			/* resource list of class */
	0,		 		/* num_resources in list */
	NULLQUARK, 			/* xrm_class ? */
	FALSE, 				/* don't compress_motion */
        XtExposeCompressSeries,	 	/* compressed exposure */
	FALSE, 				/* do compress enter-leave */
	FALSE, 				/* do have visible_interest */
	NULL, 				/* destroy widget proc */
	XtInheritResize, 		/* resize widget proc */
	NULL, 				/* expose proc */
	SetValues, 			/* set_values proc */
	NULL, 				/* set_values_hook proc */
	XtInheritSetValuesAlmost, 	/* set_values_almost proc */
	NULL, 				/* get_values_hook */
	NULL, 				/* accept_focus proc */
	XtVersion, 			/* current version */
	NULL, 				/* callback offset    */
	XtInheritTranslations, 		/* default translation table */
	XtInheritQueryGeometry, 	/* query geometry widget proc */
	NULL, 				/* display accelerator    */
	(XtPointer)&myBaseClassExtRec,	/* extension record      */
    },
    { 					/* composite class record */
	GeometryManager,                /* geometry_manager */
	ChangeManaged, 			/* change_managed		*/
	InsertChild,			/* insert_child			*/
	XtInheritDeleteChild, 		/* from the shell */
	NULL, 				/* extension record      */
    },
    { 					/* shell class record */
	NULL, 				/* extension record      */
    },
    { 					/* wm shell class record */
	NULL, 				/* extension record      */
    },
    { 					/* vendor shell class record */
	NULL,				/* extension record      */
    },
    { 					/* transient class record */
	NULL, 				/* extension record      */
    },
    { 					/* our class record */
	NULL, 				/* extension record      */
    },
};


/*
 * now make a public symbol that points to this class record
 */

externaldef(xmdialogshellwidgetclass)
    WidgetClass xmDialogShellWidgetClass = (WidgetClass)&xmDialogShellClassRec;
    

static void 
ClassInitialize( void )
{
  Cardinal                    wc_num_res, sc_num_res, wc_unique_res;
  XtResource                  *merged_list;
  int                         i, j, k;
  XtResourceList              uncompiled, res_list;
  Cardinal                    num;

/**************************************************************************
   VendorExt and  DialogExt resource lists are being merged into one
   and assigned to xmDialogShellExtClassRec. This is for performance
   reasons, since, instead of two calls to XtGetSubResources() XtGetSubvaluse()
   and XtSetSubvalues() for both the superclass and the widget class, now
   we have just one call with a merged resource list.

****************************************************************************/

  wc_num_res = xmDialogShellExtClassRec.object_class.num_resources ;

  wc_unique_res = wc_num_res - 1; /* XmNdeleteResponse has been defined */
                                  /* in VendorSE  */

  sc_num_res = xmVendorShellExtClassRec.object_class.num_resources;

  merged_list = (XtResource *)XtMalloc((sizeof(XtResource) * (wc_unique_res +
                                                                 sc_num_res)));

  _XmTransformSubResources(xmVendorShellExtClassRec.object_class.resources,
                           sc_num_res, &uncompiled, &num);

  for (i = 0; i < num; i++)
  {

  merged_list[i] = uncompiled[i];

  }

  XtFree((char *)uncompiled);

  res_list = xmDialogShellExtClassRec.object_class.resources;

  for (i = 0, j = num; i < wc_num_res; i++)
  {

   k = 0; 
   while ((k < sc_num_res) &&
	  (strcmp(merged_list[k].resource_name,res_list[i].resource_name) != 0))
     k++;
   if ((k < sc_num_res) &&
       (strcmp(merged_list[k].resource_name, res_list[i].resource_name) == 0))
     merged_list[k] = res_list[i];
   else
   {
     merged_list[j] =
        xmDialogShellExtClassRec.object_class.resources[i];
     j++;
   }
  }

  xmDialogShellExtClassRec.object_class.resources = merged_list;
  xmDialogShellExtClassRec.object_class.num_resources =
                wc_unique_res + sc_num_res ;

  xmDialogShellExtObjectClass->core_class.class_initialize();

  myBaseClassExtRec.record_type = XmQmotif;
}

/************************************************************************
 *
 *  ClassPartInit
 *    Set up the fast subclassing for the widget.
 *
 ************************************************************************/
static void 
ClassPartInit(
        WidgetClass wc )
{
   _XmFastSubclassInit(wc, XmDIALOG_SHELL_BIT);
}

static Widget 
GetRectObjKid(
        CompositeWidget p )
{
    Cardinal	i;
    Widget	*currKid;

    for (i = 0, currKid = p->composite.children;
	 i < p->composite.num_children; i++, currKid++) {
	if ((XtIsRectObj( *currKid)
	     /* The Input Method child is a CoreClass object; ignore it. */
	     && ((*currKid)->core.widget_class != coreWidgetClass)) ||
	    XmeTraitGet((XtPointer) XtClass(*currKid), XmQTdialogShellSavvy)) {
	    return (*currKid);
	} 
    }
    return NULL;
}


/************************************************************************
 *
 *  Initialize
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Initialize(
        Widget request,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    if (XtWidth  (new_w) <= 0)  XtWidth  (new_w) = 5;
    if (XtHeight (new_w) <= 0)  XtHeight (new_w) = 5;
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean 
SetValues(
        Widget current,
        Widget request,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    Widget child ;
    XmDialogSavvyTrait trait ;

    if(!current->core.mapped_when_managed 
       && new_w->core.mapped_when_managed) {   
        if((child = GetRectObjKid ((CompositeWidget) new_w))
	   && !child->core.being_destroyed    ) {   
	    if ((trait = (XmDialogSavvyTrait)
		 XmeTraitGet((XtPointer) XtClass(child), 
			     XmQTdialogShellSavvy)) != NULL) {
		trait->callMapUnmapCB(child, True);	/* call Map callback */
	    }
            XtPopup(new_w, XtGrabNone) ;
	} 
    } 

    return (FALSE);
}

static void 
InsertChild(
        Widget w )
{
    CompositeWidget p = (CompositeWidget) XtParent (w);
    XtWidgetProc insert_child;
   
    /*
     * Make sure we only have a rectObj, a VendorObject, and
     *   maybe an Input Method (CoreClass) object as children.
     */
    if (!XtIsRectObj(w)) return;
    else
	{
	    if(    (w->core.widget_class != coreWidgetClass)
                /* The Input Method child is a CoreClass object. */
                && GetRectObjKid( p)    )
	      {
		/* we need _XmError() too! */
		  XtError(MSG1);
	      }
	    else
	      {   /*
		   * make sure we're realized so people won't core dump when 
		   *   doing incorrect managing prior to realize
		   */
		  XtRealizeWidget((Widget) p);
	      }
	}
    _XmProcessLock();
    insert_child = ((CompositeWidgetClass) compositeWidgetClass)
				      ->composite_class.insert_child;
    _XmProcessUnlock();
    (*insert_child)(w);
    return ;
}

static void 
GetDefaultPosition(
        Widget child,
        Widget parent,
        Position *xRtn,
        Position *yRtn )
{
    Display 	*disp;
    int 	max_w, max_h;
    Position 	x, y;

    x = HALFDIFF(XtWidth(parent), XtWidth(child));
    y = HALFDIFF(XtHeight(parent), XtHeight(child));
    
    /* 
     * find root co-ords of the parent's center
     */
    if (XtIsRealized (parent))
      XtTranslateCoords(parent, x, y, &x, &y);
    
    /*
     * try to keep the popup from dribbling off the display
     */
    disp = XtDisplay (child);
    max_w = DisplayWidth  (disp, DefaultScreen (disp));
    max_h = DisplayHeight (disp, DefaultScreen (disp));
    
    if ((x + (int)TotalWidth  (child)) > max_w) 
      x = max_w - TotalWidth  (child);
    if ((y + (int)TotalHeight (child)) > max_h) 
      y = max_h - TotalHeight (child);
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    *xRtn = x;
    *yRtn = y;
}



/*
 * border width and size and location are ty...
 *
 * 1. We allow the border width of a XmDialogShell child to change
 *    size arbitrarily.
 *
 * 2. The border width of the shell widget tracks the child's
 *    at all times, exactly.
 *
 * 3. The width of the shell is kept exactly the same as the
 *    width of the child at all times.
 *
 * 4. The child is always positioned at the location
 *    (- child_border, - child_border).
 *
 * the net result is the child has a border width which is always
 * what the user asked for;  but none of it is ever seen, it's all
 * clipped by the shell (parent).  The user sees the border
 * of the shell which is the size he set the child's border to.
 *
 * In the DEC window manager world the window manager does
 * exactly the same thing with the window it puts around the shell.
 * Hence the shell and child have a border width just as the user
 * set but the window manager overrides that and only a single
 * pixel border is displayed.  In a non-wm environment the child 
 * appears to have a border width, in reality this is the shell
 * widget border.  You wanted to know...
 */
static void 
ChangeManaged(
        Widget wid )
{
    XmDialogShellWidget shell = (XmDialogShellWidget) wid ;
    /*
     *  If the child went to unmanaged, call XtPopdown.
     *  If the child went to managed, call XtPopup.
     */
    
    Widget	 child;
    XmWidgetExtData		extData = 
	_XmGetWidgetExtData((Widget) shell, XmSHELL_EXTENSION);
    XmVendorShellExtObject ve;
    XmDialogSavvyTrait trait ;

    if(extData==NULL)
    {
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
        return;
    }

    ve = (XmVendorShellExtObject)extData->widget;

    if (((child = GetRectObjKid((CompositeWidget) shell)) == NULL) ||
	(child->core.being_destroyed))
	return;



    trait = (XmDialogSavvyTrait) 
      XmeTraitGet((XtPointer) XtClass(child), XmQTdialogShellSavvy) ;
    
    /* MANAGED Case first ********/
    if (child->core.managed)  {
	XtWidgetGeometry	request;
	Position		kidX, kidY;
	Dimension		kidBW;
	Boolean		defaultPosition = True;

	/*
	 * temporary workaround for setkeyboard focus |||
	 */
	if (child != ve->vendor.old_managed)
	    {
		XtSetKeyboardFocus((Widget)shell, (Widget)child);
		ve->vendor.old_managed = (Widget)child;
	    }

	/* 
	 * if the child isn't realized, then we need to realize it
	 * so we have a valid size. It will get created as a result
	 * so we  zero out it's position info so it'll
	 * be okay and then restore it.
	 */
	if (!XtIsRealized(child)) {
	    kidX = XtX(child);
	    kidY = XtY(child);
	    kidBW = XtBorderWidth(child);
		
	    XtX(child) = 0;
	    XtY(child) = 0;
	    XtBorderWidth(child) = 0;
		
	    XtRealizeWidget(child);
		
	    XtX(child) = kidX;
	    XtY(child) = kidY;
	    XtBorderWidth(child) = kidBW;
	}
	  
	else if (trait) {
	    /*  
	     *  Move the window to 0,0
	     *  but don't tell the widget.  It thinks it's where
	     *  the shell is...
	     */
	    if ((XtX(child) != 0) || (XtY(child) != 0))
		XMoveWindow (XtDisplay(child), XtWindow(child), 0, 0);
	}

	/*
	 * map callback should occur BEFORE child default positioning
	 * otherwise, widgets such as fileselection using map callback for
	 * correct sizing have default positioning done before the widget 
	 * grows to its correct dimensions
	 */

	if(shell->core.mapped_when_managed && trait ) { 
	    trait->callMapUnmapCB(child, True);	 /* call Map callback */
	}	

	/* 
	 * Make sure that the shell has the same common parameters as 
	 * its child.  Then move the child so that the shell will 
	 * correctly surround it.
	 */
	request.request_mode = 0;
	
	if (trait) {
	    XtVaGetValues(child, XmNdefaultPosition, &defaultPosition, NULL);

	    if (defaultPosition && (ve->vendor.externalReposition)) {
		defaultPosition = False;
		XtVaSetValues(child, XmNdefaultPosition, False, NULL);
	    }
	}

	if (XtX(child) && trait) {
	    kidX = XtX(child);
	    XtX(child) = 0;
	} else
	    kidX = XtX(shell);
	
	if (XtY(child) && trait) {
	    kidY = XtY(child);
	    XtY(child) = 0;
	} else
	    kidY = XtY(shell);

	if (XtBorderWidth(child) && trait) {
	    kidBW = XtBorderWidth(child);
	    XtBorderWidth(child) = 0;
	} else
	    kidBW = XtBorderWidth(shell);
	

	if (XtWidth (child) != XtWidth (shell)) {
	    request.request_mode |= CWWidth;
	    request.width = XtWidth(child);
	}

	if (XtHeight (child) + ve->vendor.im_height != XtHeight (shell)) {
	    request.request_mode |= CWHeight;
	    request.height = XtHeight(child) + ve->vendor.im_height;
	}
	
	if (trait) {
	    if (defaultPosition)  {
		GetDefaultPosition(child,
				   XtParent(shell),
				   &request.x,
				   &request.y);
		if (request.x != kidX) request.request_mode |= CWX;
		if (request.y != kidY) request.request_mode |= CWY;
	    } else {
		if (kidX != XtX(shell)) {
		    request.request_mode |= CWX;
		    if (kidX == XmDIALOG_SAVVY_FORCE_ORIGIN)
			request.x = 0;
		    else
			request.x = kidX;
		}
		if (kidY != XtY(shell)) {
		    request.request_mode |= CWY;
		    if (kidY == XmDIALOG_SAVVY_FORCE_ORIGIN)
			request.y = 0;
		    else
			request.y = kidY;
		}
	    }
	} else {
	    if (kidX != XtX(shell)) {
		request.request_mode |= CWX;
		request.x = kidX;
	    }
	    if (kidY != XtY(shell)) {
		request.request_mode |= CWY;
		request.y = kidY;
	    }
	    if (kidBW != XtBorderWidth(shell)) {
		request.request_mode |= CWBorderWidth;
		request.border_width = kidBW;
	    }
	}

	if (request.request_mode) {
	    unsigned int old_height = ve->vendor.im_height;
	    XtMakeGeometryRequest((Widget) shell, &request, &request);
	    _XmImResize((Widget)shell);
            if (ve->vendor.im_height != old_height)
            {
               request.request_mode = CWHeight;
               request.height = XtHeight(child) + ve->vendor.im_height;
               XtMakeGeometryRequest((Widget) shell, &request, &request);
               _XmImResize((Widget)shell);
            }
	}
	
	/*
	 * the grab_kind is handled in the popup_callback
	 */
	if(shell->core.mapped_when_managed    ) {   
	    XtPopup  ((Widget) shell, XtGrabNone);
	} 
    }


    /*
     * CHILD BEING UNMANAGED
     */
    else {
        int i, j;
	/*
	 * Fix for CR5043, CR5758 and CR8825 -
	 * For nested Dialog Shells, it is necessary to unmanage
	 * dialog shell popups of the child of this dialog shell.
	 */
	for (i = 0; i < child->core.num_popups; i++) {
	  if (XmIsDialogShell(child->core.popup_list[i])) {
	    XmDialogShellWidget next_shell = 
	      (XmDialogShellWidget)(child->core.popup_list[i]);

	    for (j = 0; j < next_shell->composite.num_children; j++) {
	      XtUnmanageChild(next_shell->composite.children[j]);
	    }
	  }
	}
	/* End Fix CR5043, CR5758 and CR8825 */
	    
	/*
	 * take it down and then tell user
	 */
	    
	XtPopdown((Widget) shell);
	    
	if(trait ) { 
	    trait->callMapUnmapCB(child, False); /* call UnMap callback */
	}	
    }

    XmeNavigChangeManaged((Widget) shell);
}                       


/************************************************************************
 *
 *  GeometryManager
 *
 ************************************************************************/
/*ARGSUSED*/
static XtGeometryResult 
GeometryManager(
        Widget wid,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply ) /* unused */
{
    ShellWidget 	shell = (ShellWidget)(wid->core.parent);
    XtWidgetGeometry 	my_request;
    XmVendorShellExtObject ve;
    XmWidgetExtData   extData;

    extData = _XmGetWidgetExtData((Widget)shell, XmSHELL_EXTENSION);
    if(extData==NULL)
    {
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returning NULL pointer.");
#endif
        return XtGeometryNo;
    }

    ve = (XmVendorShellExtObject) extData->widget;

    if(!(shell->shell.allow_shell_resize) && XtIsRealized(wid) &&
       (request->request_mode & (CWWidth | CWHeight | CWBorderWidth)))
      return(XtGeometryNo);

    
    /*
     * Because of our klutzy API we mimic position requests on the
     * dialog to ourselves. 
     * We cannot check for the trait here since it isn't done only for 
     * BB. DialogShell GM behavior is to always follow position requests
     * even if the child is not dialogShellSavvy.
     */

    my_request.request_mode = 0;

    /* %%% worry about XtCWQueryOnly */
    if (request->request_mode & XtCWQueryOnly)
      my_request.request_mode |= XtCWQueryOnly;

    /* Here we have a tricky bit of code.
       If the SetValues on the bb child position was 0,
       which is always the current position of the bb, Xt will
       not see a change and therefore not trigerred a geometry request.
       So BB (or any dialogShellSavvy child) has to catch this case
       and change the position request to use a special value,
       XmDIALOG_SAVVY_FORCE_ORIGIN, to notify the Dialog that it wants
       to move in 0 */

    if (request->request_mode & CWX) {
	if (request->x == XmDIALOG_SAVVY_FORCE_ORIGIN)
	  my_request.x = 0;
	else
	  my_request.x = request->x;
	my_request.request_mode |= CWX;
    }
    if (request->request_mode & CWY) {
	if (request->y == XmDIALOG_SAVVY_FORCE_ORIGIN)
	  my_request.y = 0;
	else
	  my_request.y = request->y;
	my_request.request_mode |= CWY;
    }

    if (request->request_mode & CWWidth) {
	my_request.width = request->width;
	my_request.request_mode |= CWWidth;
    }
    if (request->request_mode & CWHeight) {
        if (!ve->vendor.im_height)
          _XmImResize((Widget)shell); /* updates im_height */
	my_request.height = request->height + ve->vendor.im_height;
	my_request.request_mode |= CWHeight;
    }
    if (request->request_mode & CWBorderWidth) {
	my_request.border_width = request->border_width;
	my_request.request_mode |= CWBorderWidth;
    }

    if (XtMakeGeometryRequest((Widget)shell, &my_request, NULL)
	== XtGeometryYes) {
          if (!(request->request_mode & XtCWQueryOnly)) {
	      /* just report the size changes to the kid, not
		 the dialog position itself, but reply yes
		 anyway */
	      if (my_request.request_mode & CWWidth)
		  wid->core.width = my_request.width ;
	      _XmImResize((Widget)shell);
	      if (my_request.request_mode & CWHeight)
		  wid->core.height = my_request.height - ve->vendor.im_height;
	  }
	  return XtGeometryYes;
      } else 
	  return XtGeometryNo;
}


/*
 *************************************************************************
 *
 * Public creation entry points
 *
 *************************************************************************
 */
/*
 * low level create entry points
 */
Widget 
XmCreateDialogShell(
        Widget p,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return (XtCreatePopupShell(name, xmDialogShellWidgetClass, p, al, ac));
}

/****************************************************************************
 * this suffix is added to dialog shells created by Xm convenience routines *
 * so that, for example, a call to create a form dialog named f generates a *
 * dialog shell named f_popup in addition to a form named f                 *
 ****************************************************************************/

#define XmDIALOG_SUFFIX		"_popup"
#define XmDIALOG_SUFFIX_SIZE	6


/****************************************************************
 * This convenience function creates a DialogShell and a given class
 *   child of the shell; returns the child widget.
 ****************/
Widget 
XmeCreateClassDialog(
	WidgetClass w_class,
	Widget ds_p,
        String name,
        ArgList bb_args,
        Cardinal bb_n )
{   
    Widget		bb ;		/*  child	*/
    Widget		ds ;		/*  DialogShell		*/
    ArgList		ds_args ;	/*  arglist for shell	*/
    char *              ds_name ;

    if (!name) name = "";

    /*	Create DialogShell parent.
    */
    ds_name = XtMalloc( (strlen(name)+XmDIALOG_SUFFIX_SIZE+1) * sizeof(char) ) ;
    strcpy( ds_name, name) ;
    strcat( ds_name, XmDIALOG_SUFFIX) ;

    ds_args = (ArgList) XtMalloc( sizeof( Arg) * (bb_n + 1)) ;
    memcpy( ds_args, bb_args, (sizeof( Arg) * bb_n)) ;
    XtSetArg( ds_args[bb_n], XmNallowShellResize, True) ; 
    ds = XmCreateDialogShell( ds_p, ds_name, ds_args, bb_n + 1) ;

    XtFree( (char *) ds_args);
    XtFree( ds_name) ;

    /*	Create the widget.
    */
    bb = XtCreateWidget(name, w_class, ds, bb_args, bb_n) ;

    /*	Add callback to destroy DialogShell parent.
    */
    XtAddCallback(bb, XmNdestroyCallback, _XmDestroyParentCallback, 
		  (XtPointer) NULL) ;
    /*	Return child.
    */
    return( bb) ;
}


