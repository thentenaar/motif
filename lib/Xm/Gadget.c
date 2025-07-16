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
static char rcsid[] = "$TOG: Gadget.c /main/17 1997/07/07 11:38:57 cshi $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <ctype.h>
#include <stdio.h>
#include <X11/Intrinsic.h>	/* <X11/Shell.h> doesn't define externalref */
#include <X11/IntrinsicP.h>	/* <X11/ShellP.h> needs it. */
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <Xm/AccColorT.h>
#include <Xm/BaseClassP.h>
#include <Xm/DrawP.h>
#include <Xm/ExtObjectP.h>
#include <Xm/LayoutT.h>
#include <Xm/ManagerP.h>
#include <Xm/TraitP.h>
#include <Xm/UnitTypeT.h>
#include <Xm/UnhighlightT.h>
#include "XmI.h"
#include "BaseClassI.h"
#include "GadgetI.h"
#include "MessagesI.h"
#include "ResIndI.h"
#include "RepTypeI.h"
#include "SyntheticI.h"
#include "TraitI.h"
#include "TraversalI.h"
#include "ToolTipI.h"



#define INVALID_UNIT_TYPE 255

#define MESSAGE1	_XmMMsgGadget_0000

/********    Static Function Declarations    ********/

static void GetHighlightColor(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void GetTopShadowColor(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void GetBottomShadowColor(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void ClassInitialize( void ) ;
static void ClassPartInit(
                        WidgetClass g) ;
static void SecondaryObjectCreate(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget w) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void BorderHighlight(
                        Widget w) ;
static void BorderUnhighlight(
                        Widget w) ;
static void FocusChange(
                        Widget wid,
                        XmFocusChange change) ;
static XmNavigability WidgetNavigable(
                        Widget wid) ;

static XmDirection GetDirection(Widget);
static void GetColors(Widget widget,
		      XmAccessColorData color_data);
static unsigned char GetUnitType(Widget widget);

static void GetToolTipString (Widget wid,
               int resource, /* unused */
               XtArgVal * value);

static XmImportOperator SetToolTipString (Widget wid,
               int resource, /* unused */
               XtArgVal * value);

/********    End Static Function Declarations    ********/



/*  Resource definitions for Subclasses of Gadget */

static XtResource resources[] =
{
   {
     XmNx, XmCPosition, XmRHorizontalPosition, sizeof(Position),
     XtOffsetOf( struct _WidgetRec, core.x), XmRImmediate, (XtPointer) 0
   },

   {
     XmNy, XmCPosition, XmRVerticalPosition, sizeof(Position),
     XtOffsetOf( struct _WidgetRec, core.y), XmRImmediate, (XtPointer) 0
   },

   {
     XmNwidth, XmCDimension, XmRHorizontalDimension, sizeof(Dimension),
     XtOffsetOf( struct _WidgetRec, core.width), XmRImmediate, (XtPointer) 0
   },

   {
     XmNheight, XmCDimension, XmRVerticalDimension, sizeof(Dimension),
     XtOffsetOf( struct _WidgetRec, core.height), XmRImmediate, (XtPointer) 0
   },

   {
     XmNborderWidth, XmCBorderWidth, XmRHorizontalDimension, sizeof(Dimension),
     XtOffsetOf( struct _WidgetRec, core.border_width), XmRImmediate, (XtPointer) 0
   },

   {
     XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof (Boolean),
     XtOffsetOf( struct _XmGadgetRec, gadget.traversal_on),
     XmRImmediate, (XtPointer) True
   },

   {
     XmNhighlightOnEnter, XmCHighlightOnEnter, XmRBoolean, sizeof (Boolean),
     XtOffsetOf( struct _XmGadgetRec, gadget.highlight_on_enter),
     XmRImmediate, (XtPointer) False
   },

   {
     XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof (Dimension), XtOffsetOf( struct _XmGadgetRec, gadget.highlight_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof (Dimension), XtOffsetOf( struct _XmGadgetRec, gadget.shadow_thickness),
     XmRCallProc, (XtPointer) _XmSetThickness
   },

   {
     XmNunitType, XmCUnitType, XmRUnitType, sizeof (unsigned char),
     XtOffsetOf( struct _XmGadgetRec, gadget.unit_type),
     XmRCallProc, (XtPointer) _XmUnitTypeDefault
   },

   {
     XmNnavigationType, XmCNavigationType, XmRNavigationType, sizeof (unsigned char),
     XtOffsetOf( struct _XmGadgetRec, gadget.navigation_type),
     XmRImmediate, (XtPointer) XmNONE
   },

   {
     XmNhelpCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
     XtOffsetOf( struct _XmGadgetRec, gadget.help_callback),
     XmRPointer, (XtPointer) NULL
   },

   {
     XmNuserData, XmCUserData, XmRPointer, sizeof(XtPointer),
     XtOffsetOf( struct _XmGadgetRec, gadget.user_data),
     XmRPointer, (XtPointer) NULL
   },
   {
     XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
     sizeof(XmDirection),
     XtOffsetOf(XmGadgetRec, gadget.layout_direction),
     XmRCallProc, (XtPointer) _XmDirectionDefault
   },
};


/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
   { XmNx,
     sizeof (Position),
     XtOffsetOf( struct _RectObjRec, rectangle.x),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels },

   { XmNy,
     sizeof (Position),
     XtOffsetOf( struct _RectObjRec, rectangle.y),
     XmeFromVerticalPixels,
     XmeToVerticalPixels },

   { XmNwidth,
     sizeof (Dimension),
     XtOffsetOf( struct _RectObjRec, rectangle.width),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels },

   { XmNheight,
     sizeof (Dimension),
     XtOffsetOf( struct _RectObjRec, rectangle.height),
     XmeFromVerticalPixels,
     XmeToVerticalPixels },

   { XmNhighlightThickness,
     sizeof (Dimension),
     XtOffsetOf( struct _XmGadgetRec, gadget.highlight_thickness),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels },

   { XmNshadowThickness,
     sizeof (Dimension),
     XtOffsetOf( struct _XmGadgetRec, gadget.shadow_thickness),
     XmeFromHorizontalPixels,
     XmeToHorizontalPixels },

   { XmNhighlightColor,
     sizeof (Pixel),
     XtOffsetOf( struct _XmGadgetRec, object.parent),
     GetHighlightColor,
     NULL },

   { XmNtopShadowColor,
     sizeof (Pixel),
     XtOffsetOf( struct _XmGadgetRec, object.parent),
     GetTopShadowColor,
     NULL },

   { XmNbottomShadowColor,
     sizeof (Pixel),
     XtOffsetOf( struct _XmGadgetRec, object.parent),
     GetBottomShadowColor,
     NULL },
   { XmNtoolTipString,
     0,
     0,
     GetToolTipString, SetToolTipString },
};

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,                               /* Initialize Prehook  */
    NULL,                               /* SetValues Prehook   */
    NULL,                               /* Initialize Posthook */
    NULL,                               /* SetValues PostHook  */
    NULL,                               /* SecondaryObjectClass */
    SecondaryObjectCreate,              /* SecondaryObjectCreate */
    NULL,		                /* getSecRes data	*/
    {0},				/* fastSubclass flags	*/
    NULL,                               /* GetValues Prehook   */
    NULL,                               /* GetValues Posthook   */
    NULL,                               /* classPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    FocusChange,                        /* focusChange          */
};

static XmGadgetClassExtRec  _XmGadClassExtRec = {
    NULL,
    NULLQUARK,
    XmGadgetClassExtVersion,
    sizeof(XmGadgetClassExtRec),
    NULL,                               /* widget_baseline */
    NULL,                               /* widget_display_rect */
    NULL,                               /* widget_margins */
};

/*  The gadget class record definition  */

externaldef(xmgadgetclassrec) XmGadgetClassRec xmGadgetClassRec =
{
   {
      (WidgetClass) &rectObjClassRec,   /* superclass	         */
      "XmGadget",                       /* class_name	         */
      sizeof(XmGadgetRec),              /* widget_size	         */
      ClassInitialize,                  /* class_initialize      */
      ClassPartInit,                    /* class part initialize */
      False,                            /* class_inited          */
      Initialize,                       /* initialize	         */
      NULL,                             /* initialize_hook       */
      NULL,	                        /* realize	         */
      NULL,				/* actions               */
      0,				/* num_actions	         */
      resources,                        /* resources	         */
      XtNumber(resources),              /* num_resources         */
      NULLQUARK,                        /* xrm_class	         */
      True,                             /* compress_motion       */
      True,                             /* compress_exposure     */
      True,                             /* compress_enterleave   */
      False,                            /* visible_interest      */
      Destroy,                          /* destroy               */
      NULL,                             /* resize                */
      NULL,				/* expose                */
      SetValues,                        /* set_values	         */
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      _XmGadgetGetValuesHook,           /* get_values_hook       */
      NULL,                             /* accept_focus	         */
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,				/* display_accelerator   */
      (XtPointer)&baseClassExtRec,      /* extension             */
   },

   {
      BorderHighlight,		        /* border_highlight   */
      BorderUnhighlight,		/* border_unhighlight */
      NULL,				/* arm_and_activate   */
      NULL,				/* input_dispatch     */
      NULL,				/* visual_change      */
      syn_resources,			/* syn resources      */
      XtNumber(syn_resources),		/* num_syn_resources  */
      NULL,				/* cache_part	      */
      (XtPointer)&_XmGadClassExtRec,	/* extension          */
   }
};

externaldef(xmgadgetclass) WidgetClass xmGadgetClass =
		           (WidgetClass) &xmGadgetClassRec;


/* Access Colors Trait record for Gadget */

static XmConst XmAccessColorsTraitRec gadACT = {
  0,			/* version */
  GetColors
};

/* Unit Type Trait record for Gadget */

static XmConst XmSpecUnitTypeTraitRec gadUTT = {
  0,			/* version */
  GetUnitType
};

static XmConst XmSpecifyLayoutDirectionTraitRec gadLDT = {
  0,			/* version */
  GetDirection
};

/************************************************************************
 *
 *  The following functions are synthetic hooks.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
GetHighlightColor(
        Widget w,
	int offset,		/* unused */
	XtArgVal *value )
{
    XmManagerWidget mw = (XmManagerWidget) XtParent(w);

    *value = (XtArgVal) mw->manager.highlight_color;
}

/*ARGSUSED*/
static void
GetTopShadowColor(
        Widget w,
	int offset,		/* unused */
	XtArgVal *value )
{
    XmManagerWidget mw = (XmManagerWidget) XtParent(w);

    *value = (XtArgVal) mw->manager.top_shadow_color;
}

/*ARGSUSED*/
static void
GetBottomShadowColor(
        Widget w,
	int offset,		/* unused */
	XtArgVal *value )
{
    XmManagerWidget mw = (XmManagerWidget) XtParent(w);

    *value = (XtArgVal) mw->manager.bottom_shadow_color;
}

/************************************************************************
 *
 *  ClassInitialize
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
   _XmInitializeExtensions();
   _XmInitializeTraits();
   baseClassExtRec.record_type = XmQmotif;
}


/************************************************************************
 *
 *  ClassPartInit
 *	Used by subclasses of gadget to inherit class record procedures.
 *
 ************************************************************************/
static void
ClassPartInit(
        WidgetClass g )
{
    static Boolean first_time = TRUE;
    XmGadgetClass wc = (XmGadgetClass) g;
    XmGadgetClass super = (XmGadgetClass) wc->rect_class.superclass;
    XmGadgetClassExt              *wcePtr, *scePtr;

   wcePtr = _XmGetGadgetClassExtPtr(wc, NULLQUARK);

   if (((WidgetClass)wc != xmGadgetClass) &&
       (*wcePtr)) {

       scePtr = _XmGetGadgetClassExtPtr(super, NULLQUARK);

       if ((*wcePtr)->widget_baseline == XmInheritBaselineProc)
	   (*wcePtr)->widget_baseline = (*scePtr)->widget_baseline;

       if ((*wcePtr)->widget_display_rect == XmInheritDisplayRectProc)
	   (*wcePtr)->widget_display_rect  = (*scePtr)->widget_display_rect;

       if ((*wcePtr)->widget_margins == XmInheritMarginsProc)
	   (*wcePtr)->widget_margins  = (*scePtr)->widget_margins;

   }

    if (wc->gadget_class.border_highlight == XmInheritWidgetProc)
	wc->gadget_class.border_highlight =
           super->gadget_class.border_highlight;

    if (wc->gadget_class.border_unhighlight == XmInheritWidgetProc)
	wc->gadget_class.border_unhighlight =
	   super->gadget_class.border_unhighlight;

    if (wc->gadget_class.arm_and_activate == XmInheritArmAndActivate)
        wc->gadget_class.arm_and_activate =
           super->gadget_class.arm_and_activate;

    if (wc->gadget_class.input_dispatch == XmInheritInputDispatch)
        wc->gadget_class.input_dispatch =
           super->gadget_class.input_dispatch;

    if (wc->gadget_class.visual_change == XmInheritVisualChange)
        wc->gadget_class.visual_change =
           super->gadget_class.visual_change;

   _XmFastSubclassInit (g, XmGADGET_BIT);

    if (first_time)
    {
        _XmReOrderResourceList(xmGadgetClass, XmNunitType, NULL);
        first_time = FALSE;
    }

   _XmBuildGadgetResources((WidgetClass) wc);

   /* Install the direction trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTspecifyLayoutDirection, (XtPointer)&gadLDT);

   /* Install the accessColors trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&gadACT);

   /* Install the unit type trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTspecifyUnitType, (XtPointer)&gadUTT);
}

/************************************************************************
*
*  SecondaryObjectCreate
*
************************************************************************/
/* ARGSUSED */
static void
SecondaryObjectCreate(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmBaseClassExt              *cePtr;
    Arg                         myArgs[1];
    ArgList                     mergedArgs;

    XtSetArg(myArgs[0] ,XmNlogicalParent, new_w);

    if (*num_args)
       mergedArgs = XtMergeArgLists(args, *num_args, myArgs, XtNumber(myArgs));
    else
       mergedArgs = myArgs;


    cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);
    (void) XtCreateWidget(XtName(new_w),
                         (*cePtr)->secondaryObjectClass,
			 XtParent(new_w) ? XtParent(new_w) : new_w,
			 mergedArgs, *num_args + 1);

    if (mergedArgs != myArgs)
      XtFree( (char *) mergedArgs);
}


/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    XmGadget request = (XmGadget) rw ;
    XmGadget 			gw = (XmGadget) nw;
    XmBaseClassExt              *cePtr;
    XtInitProc                  secondaryCreate;
    XmString                    tool_tip_string;

    static XtResource subresources[] =
    {
        {
            XmNtoolTipString, XmCToolTipString, XmRXmString,
            sizeof(XmString), 0,
            XmRImmediate, (XtPointer) NULL
        },
    };

   if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE, gw->gadget.unit_type,
                                                             (Widget) gw)    )
   {
      gw->gadget.unit_type = XmPIXELS;
   }


   /*  Convert the fields from unit values to pixel values  */

   _XmGadgetImportArgs( (Widget) gw, args, num_args);
   _XmGadgetImportSecondaryArgs( (Widget) gw, args, num_args);

    cePtr = _XmGetBaseClassExtPtr(XtClass(gw), XmQmotif);

    if ((*cePtr) &&
	(*cePtr)->secondaryObjectClass &&
	(secondaryCreate = (*cePtr)->secondaryObjectCreate))
      (*secondaryCreate)( (Widget) request, (Widget) gw, args, num_args);

    XtGetSubresources(nw, &tool_tip_string, NULL, NULL,
         subresources, XtNumber(subresources),
	 args, *num_args);

    XmSetToolTipString(nw, tool_tip_string);

   gw->gadget.event_mask = 0;
   gw->gadget.have_traversal = FALSE ;
   gw->gadget.highlighted = FALSE ;
   gw->gadget.highlight_drawn = FALSE ;

   if(    (gw->gadget.navigation_type != XmDYNAMIC_DEFAULT_TAB_GROUP)
       && !XmRepTypeValidValue( XmRID_NAVIGATION_TYPE,
                                  gw->gadget.navigation_type, (Widget) gw)    )
   {   gw->gadget.navigation_type = XmNONE ;
       }

   _XmNavigInitialize ((Widget) request, (Widget) gw, args, num_args);

   gw->gadget.have_traversal = FALSE ;

   /*  Check the geometry information for the widget  */

   if (request->rectangle.width == 0)
      gw->rectangle.width += gw->gadget.highlight_thickness * 2 +
                             gw->gadget.shadow_thickness * 2;

   if (request->rectangle.height == 0)
      gw->rectangle.height += gw->gadget.highlight_thickness * 2 +
                              gw->gadget.shadow_thickness * 2;


   /*  Force the border width to 0  */

   gw->rectangle.border_width = 0;

   return ;
   }




/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/
static void Destroy(Widget w)
{
   XmGadget g = (XmGadget)w;

   _XmNavigDestroy(w);
   _XmToolTipRemove(w);
}

/************************************************************************
 *
 *  SetValues
 *     Perform and updating necessary for a set values call.
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
        XmGadget cur = (XmGadget) cw ;
        XmGadget req = (XmGadget) rw ;
        XmGadget new_w = (XmGadget) nw ;
   Boolean returnFlag;

    if (XtIsSensitive(cw) != XtIsSensitive(nw))
    {
    	if (!XtIsSensitive(nw))
    	{
	    _XmToolTipLeave(nw, NULL, NULL, NULL);
    	}
    }

    if (XmGetToolTipString(nw) != XmGetToolTipString(cw))
    {
        XmSetToolTipString(cw, XmGetToolTipString(nw));
    }

   /* CR 7124: XmNlayoutDirection is a CG resource. */
   if (cur->gadget.layout_direction != new_w->gadget.layout_direction)
     {
       XmeWarning((Widget) new_w, MESSAGE1);
       new_w->gadget.layout_direction = cur->gadget.layout_direction;
     }

   if(    cur->gadget.navigation_type != new_w->gadget.navigation_type    )
     {
       if(    !XmRepTypeValidValue( XmRID_NAVIGATION_TYPE,
			        new_w->gadget.navigation_type, (Widget) new_w)    )
	 {
	   new_w->gadget.navigation_type = cur->gadget.navigation_type ;
	 }
     }
   returnFlag = _XmNavigSetValues ((Widget) cur, (Widget) req, (Widget) new_w,
				                               args, num_args);
   /*  Validate changed data.  */

   if(    !XmRepTypeValidValue( XmRID_UNIT_TYPE, new_w->gadget.unit_type,
                                                             (Widget) new_w)    )
   {
      new_w->gadget.unit_type = cur->gadget.unit_type;
   }


   /*  Convert the necessary fields from unit values to pixel values  */

   _XmGadgetImportArgs((Widget) new_w, args, num_args);

   /*  Check for resize conditions  */

   if (cur->gadget.shadow_thickness != new_w->gadget.shadow_thickness ||
       cur->gadget.highlight_thickness != new_w->gadget.highlight_thickness)
      returnFlag = True;


   /*  Force the border width to 0  */

   new_w->rectangle.border_width = 0;

   if(    new_w->gadget.highlight_drawn
      &&  (    !XtIsSensitive( (Widget) new_w)
	   ||  (    cur->gadget.highlight_on_enter
		&&  !(new_w->gadget.highlight_on_enter)
		&&  (_XmGetFocusPolicy( (Widget) new_w) == XmPOINTER)))    )
     {
       if(    ((XmGadgetClass) XtClass( new_w))
	                                 ->gadget_class.border_unhighlight    )
	 {
	   (*(((XmGadgetClass) XtClass( new_w))
	                   ->gadget_class.border_unhighlight))( (Widget) new_w) ;
	 }
     }

   /*  Return a flag which may indicate that a redraw needs to occur.  */

   return (returnFlag);
}


/**********************************************************************
 *
 *  _XmBuildGadgetResources
 *	Build up the gadget's synthetic resource processing list
 *	by combining the super classes with this class.
 *
 **********************************************************************/
void
_XmBuildGadgetResources(
        WidgetClass c )
{
	XmGadgetClass wc = (XmGadgetClass) c ;
	XmGadgetClass sc;
	XmBaseClassExt *classExtPtr;
	XmExtClassRec *secondaryObjClass;
	WidgetClass secObjSuperClass;

	_XmProcessLock();

	sc = (XmGadgetClass) wc->rect_class.superclass;

	_XmInitializeSyntheticResources(wc->gadget_class.syn_resources,
		wc->gadget_class.num_syn_resources);

	/*
	 * RectObj has no synthetic resources to incorporate.
	 */
	if (sc != (XmGadgetClass) rectObjClass)
	{
		_XmBuildResources (&(wc->gadget_class.syn_resources),
			&(wc->gadget_class.num_syn_resources),
			sc->gadget_class.syn_resources,
			sc->gadget_class.num_syn_resources);
	}

	classExtPtr = _XmGetBaseClassExtPtr(c, XmQmotif);
	secondaryObjClass = (XmExtClassRec *)
		((*classExtPtr)->secondaryObjectClass);

	/*
	 * Not all gadgets have secondary objects.
	 */
	if (secondaryObjClass == NULL) {_XmProcessUnlock(); return; }

	secObjSuperClass = secondaryObjClass->object_class.superclass;

	/*
	 * secondary object resources have already been inited when they are
	 * inherited
	 */
	if (!secObjSuperClass->core_class.class_inited) {
	    _XmInitializeSyntheticResources(
		secondaryObjClass->ext_class.syn_resources,
		secondaryObjClass->ext_class.num_syn_resources);
	    secObjSuperClass->core_class.class_inited = True;
	}

	/*
	 * ExtObject has no synthetic resources to incorporate.
	 */
	if (secObjSuperClass != (WidgetClass) xmExtObjectClass)
	{
		_XmBuildResources (
			&(secondaryObjClass->ext_class.syn_resources),
			&(secondaryObjClass->ext_class.num_syn_resources),
			((XmExtClassRec *)secObjSuperClass)
				->ext_class.syn_resources,
			((XmExtClassRec *)secObjSuperClass)
				->ext_class.num_syn_resources);
	}
	_XmProcessUnlock();
}

static void
BorderHighlight(
        Widget w )
{
    XmGadget g ;

    g = (XmGadget) w ;

    g->gadget.highlighted = True ;
    g->gadget.highlight_drawn = True ;

    if(    g->rectangle.width == 0 || g->rectangle.height == 0
        || g->gadget.highlight_thickness == 0    )
    {
        return ;
        }

    XmeDrawHighlight( XtDisplay( (Widget) g), XtWindow( (Widget) g),
           ((XmManagerWidget)(g->object.parent))->manager.highlight_GC,
             g->rectangle.x, g->rectangle.y, g->rectangle.width,
               g->rectangle.height, g->gadget.highlight_thickness) ;
    return ;
    }

static void
BorderUnhighlight(
        Widget w )
{
    XmGadget g = (XmGadget) w ;
    XmSpecifyUnhighlightTrait UnhighlightT;
    GC manager_background_GC;

    g->gadget.highlighted = False ;
    g->gadget.highlight_drawn = False ;

    if ( g->rectangle.width == 0
	|| g->rectangle.height == 0
        || g->gadget.highlight_thickness == 0)
	{
        return ;
        }

    /* If unhighlight trait in parent use specified GC, else just clear area */
    if ( XmIsManager(g->object.parent)
        && ((UnhighlightT=(XmSpecifyUnhighlightTrait)XmeTraitGet((XtPointer)
	    XtClass(g->object.parent), XmQTspecifyUnhighlight)) != NULL)
	&& (UnhighlightT->getUnhighlightGC != NULL))
	{
	/* Get and use parent's specified background GC for unhighlighting */
	manager_background_GC =
			UnhighlightT->getUnhighlightGC( g->object.parent, w);
        XmeDrawHighlight( XtDisplay( g), XtWindow( g),
			    manager_background_GC,
			    g->rectangle.x, g->rectangle.y,
			    g->rectangle.width, g->rectangle.height,
			    g->gadget.highlight_thickness) ;
	}
    else
	{
	XmeClearBorder( XtDisplay( g), XtWindow( g),
			g->rectangle.x, g->rectangle.y,
			g->rectangle.width, g->rectangle.height,
			g->gadget.highlight_thickness) ;
	}

    return ;
    }

static void
FocusChange(
        Widget wid,
        XmFocusChange change)
{
  /* Enter/Leave is called only in pointer mode,
   * Focus in/out only called in explicit mode.
   */
  switch(    change    )
    {
    case XmENTER:
      if(    !(((XmGadget) wid)->gadget.highlight_on_enter)    )
	{
	  break ;
	}
      /* Drop through. */
    case XmFOCUS_IN:
      if(    change == XmFOCUS_IN    ) /* Because of drop-though. */
	{
	  ((XmGadget) wid)->gadget.have_traversal = TRUE ;
	}
      if(    ((XmGadgetClass) XtClass( wid))
                                           ->gadget_class.border_highlight    )
        {
	  (*(((XmGadgetClass) XtClass( wid))
                                      ->gadget_class.border_highlight))( wid) ;
	}
      break ;
    case XmLEAVE:
      if(    !(((XmGadget) wid)->gadget.highlight_on_enter)    )
	{
	  break ;
	}
      /* Drop through. */
    case XmFOCUS_OUT:
      if(    change == XmFOCUS_OUT    ) /* Because of drop-though. */
	{
	  ((XmGadget) wid)->gadget.have_traversal = FALSE ;
	}
      if(    ((XmGadgetClass) XtClass( wid))
                                         ->gadget_class.border_unhighlight    )
        {
	  (*(((XmGadgetClass) XtClass( wid))
                                    ->gadget_class.border_unhighlight))( wid) ;
	}
      break ;
    }
  return ;
}

static XmNavigability
WidgetNavigable(
        Widget wid)
{
  if(    XtIsSensitive(wid)
     &&  ((XmGadget) wid)->gadget.traversal_on    )
    {
      XmNavigationType nav_type = ((XmGadget) wid)->gadget.navigation_type ;

      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
	 ||  (    (nav_type == XmTAB_GROUP)
	      &&  !_XmShellIsExclusive( wid))    )
	{
	  return XmTAB_NAVIGABLE ;
	}
      return XmCONTROL_NAVIGABLE ;
    }
  return XmNOT_NAVIGABLE ;
}



static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
    XmManagerWidget parent = (XmManagerWidget) XtParent(w);

    color_data->valueMask = AccessForeground | AccessBackgroundPixel |
	AccessHighlightColor | AccessTopShadowColor | AccessBottomShadowColor;
    color_data->background = parent->core.background_pixel;
    color_data->foreground = parent->manager.foreground;
    color_data->highlight_color = parent->manager.highlight_color;
    color_data->top_shadow_color = parent->manager.top_shadow_color;
    color_data->bottom_shadow_color = parent->manager.bottom_shadow_color;
}

static unsigned char
GetUnitType(Widget w)
{
    return ((XmGadget) w)->gadget.unit_type ;
}

static XmDirection
GetDirection(Widget w)
{
  return ((XmGadget)(w))->gadget.layout_direction;
}

static void
GetToolTipString(Widget wid,
                 int resource, /* unused */
                 XtArgVal * value)
{
    XmString string = XmGetToolTipString (wid);
    *value = (XtArgVal) string;
}

XmImportOperator
SetToolTipString(Widget wid,
                 int resource, /* unused */
                 XtArgVal * value)
{
    XmSetToolTipString (wid, (XmString)*value);
    return XmSYNTHETIC_NONE;
}
