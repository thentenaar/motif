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
static char rcsid[] = "$XConsortium: Separator.c /main/14 1996/03/25 17:53:11 barstow $"
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <X11/keysymdef.h>   
#include <X11/IntrinsicP.h>
#include "XmI.h"
#include <Xm/RowColumnP.h>
#include <Xm/SeparatorP.h>
#include "RepTypeI.h"
#include <Xm/TraitP.h>
#include <Xm/MenuT.h>
#include <Xm/DrawP.h>
#include <Xm/VaSimpleP.h>


/********    Static Function Declarations    ********/

static void ClassInitialize() ;
static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetSeparatorGC( 
                        XmSeparatorWidget mw) ;
static void Redisplay( 
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Destroy( 
                        Widget wid) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;

/********    End Static Function Declarations    ********/

/* Menu Savvy trait record */
static XmConst XmMenuSavvyTraitRec MenuSavvySeparatorRecord = {
    0,				/* version */
    NULL,			/* disableCallback */
    NULL,			/* getAccelerator */
    NULL,			/* getMnemonic */
    NULL,			/* getActivateCBName */
};


/* Definition for resources that need special processing in get values */

static XmSyntheticResource syn_resources[] = 
{
   {
      XmNmargin, 
      sizeof (Dimension),
      XtOffsetOf( struct _XmSeparatorRec, separator.margin),
      XmeFromHorizontalPixels,
      XmeToHorizontalPixels
   },
};


/*  Resource list for Separator  */

static XtResource resources[] = 
{
   {
      XmNseparatorType, XmCSeparatorType, XmRSeparatorType, sizeof (unsigned char),
      XtOffsetOf( struct _XmSeparatorRec, separator.separator_type),
      XmRImmediate, (XtPointer) XmSHADOW_ETCHED_IN
   },

   {
      XmNmargin, 
      XmCMargin,
      XmRHorizontalDimension, 
      sizeof (Dimension),
      XtOffsetOf( struct _XmSeparatorRec, separator.margin),
      XmRImmediate, (XtPointer)  0
   },

   {
      XmNorientation, XmCOrientation, XmROrientation, sizeof (unsigned char),
      XtOffsetOf( struct _XmSeparatorRec, separator.orientation),
      XmRImmediate, (XtPointer) XmHORIZONTAL
   },
   {
     XmNtraversalOn,
     XmCTraversalOn,
     XmRBoolean,
     sizeof (Boolean),
     XtOffsetOf( struct _XmPrimitiveRec, primitive.traversal_on),
     XmRImmediate, (XtPointer) FALSE
   },
   {
     XmNhighlightThickness,
     XmCHighlightThickness,
     XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf( struct _XmPrimitiveRec, primitive.highlight_thickness),
     XmRImmediate, (XtPointer) 0
   },
   {
     XmNshadowThickness,
     XmCShadowThickness,
     XmRHorizontalDimension,
     sizeof (Dimension),
     XtOffsetOf( struct _XmPrimitiveRec, primitive.shadow_thickness),
     XmRImmediate, (XtPointer) 2
   },
};


/*  The Separator class record definition  */

externaldef(xmseparatorclassrec) XmSeparatorClassRec xmSeparatorClassRec =
{
   {
      (WidgetClass) &xmPrimitiveClassRec, /* superclass	 	 */
      "XmSeparator",                      /* class_name	         */	
      sizeof(XmSeparatorRec),             /* widget_size         */	
      ClassInitialize,			/* class_initialize      */    
      ClassPartInitialize,              /* class_part_initialize */
      FALSE,                            /* class_inited          */	
      Initialize,                       /* initialize	         */	
      (XtArgsProc)NULL,                 /* initialize_hook       */
      XtInheritRealize,                 /* realize	         */	
      NULL,                             /* actions               */	
      0,                                /* num_actions    	 */	
      resources,                        /* resources	         */	
      XtNumber (resources),             /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      XtExposeCompressSeries |          /* compress_exposure     */	
	  XtExposeNoRegion,
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      Destroy,                          /* destroy               */	
      (XtWidgetProc)NULL,               /* resize                */	
      Redisplay,                        /* expose                */	
      SetValues,                        /* set_values	         */	
      (XtArgsFunc)NULL,                 /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      (XtArgsProc)NULL,                 /* get_values_hook       */
      (XtAcceptFocusProc)NULL,          /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      (XtGeometryHandler)NULL,          /* query_geometry        */
      (XtStringProc)NULL,               /* display_accelerator   */
      NULL,                             /* extension             */
   },

   {
      (XtWidgetProc)NULL,               /* Primitive border_highlight   */
      (XtWidgetProc)NULL,               /* Primitive border_unhighlight */
      NULL,                             /* translations                 */
      (XtActionProc)NULL,               /* arm_and_activate             */
      syn_resources,                    /* syn resources                */
      XtNumber(syn_resources),	        /* num syn_resources            */
      NULL,                             /* extension                    */
   },

   {
      (XtPointer) NULL,                 /* extension                    */
   }
};

externaldef(xmseparatorwidgetclass) WidgetClass xmSeparatorWidgetClass =
				   (WidgetClass) &xmSeparatorClassRec;

/************************************************************************
 *
 *  ClassInitialize 
 *
 ************************************************************************/
static void 
ClassInitialize( void )
{
    /* Install the menu savvy trait. */
    XmeTraitSet((XtPointer) xmSeparatorWidgetClass, XmQTmenuSavvy,
		(XtPointer) &MenuSavvySeparatorRecord);
}

/************************************************************************
 *
 *  ClassPartInitialize
 *     Set up the fast subclassing for the widget
 *
 ************************************************************************/
static void 
ClassPartInitialize(
        WidgetClass wc )
{
   _XmFastSubclassInit (wc, XmSEPARATOR_BIT);
}

      
/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
   XmSeparatorWidget request = (XmSeparatorWidget) rw ;
   XmSeparatorWidget new_w = (XmSeparatorWidget) nw ;
   new_w -> primitive.traversal_on = FALSE; 

   /* Force highlightThickness to zero if in a menu. */
   if (XmIsRowColumn(XtParent(new_w)) &&
       ((RC_Type(XtParent(new_w)) == XmMENU_PULLDOWN) ||
        (RC_Type(XtParent(new_w)) == XmMENU_POPUP)))
     new_w->primitive.highlight_thickness = 0;

   if(    !XmRepTypeValidValue( XmRID_SEPARATOR_TYPE,
                              new_w->separator.separator_type, (Widget) new_w)    )
   {
      new_w -> separator.separator_type = XmSHADOW_ETCHED_IN;
   }

   if(    !XmRepTypeValidValue( XmRID_ORIENTATION,
                                 new_w->separator.orientation, (Widget) new_w)    )
   {
      new_w -> separator.orientation = XmHORIZONTAL;
   }

   if (new_w->separator.orientation == XmHORIZONTAL)
   {
      if (request -> core.width == 0)
	 new_w -> core.width = 2 * new_w -> primitive.highlight_thickness +2;

      if (request -> core.height == 0)
      {
	 new_w -> core.height = 2 * new_w -> primitive.highlight_thickness;

	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE)
	    new_w -> core.height += 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.height += new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE)
	    new_w -> core.height += 5;
	 else
	    if (new_w -> core.height == 0)
	       new_w -> core.height = 1;
      }
   }
   
   if (new_w->separator.orientation == XmVERTICAL)
   {
      if (request -> core.height == 0)
	 new_w -> core.height = 2 * new_w -> primitive.highlight_thickness +2;

      if (request -> core.width == 0)
      {
	 new_w -> core.width = 2 * new_w -> primitive.highlight_thickness;

	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE)
	    new_w -> core.width += 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.width += new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE)
	    new_w -> core.width += 5;
	 else
	    if (new_w -> core.width == 0)
	       new_w -> core.width = 1;
      }
   }
   
   /*  Get the drawing graphics contexts.  */

   GetSeparatorGC (new_w);
}




/************************************************************************
 *
 *  GetSeparatorGC
 *     Get the graphics context used for drawing the separator.
 *
 ************************************************************************/
static void 
GetSeparatorGC(
        XmSeparatorWidget mw )
{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground;

   values.foreground = mw -> primitive.foreground;
   values.background = mw -> core.background_pixel;

   if (mw -> separator.separator_type == XmSINGLE_DASHED_LINE ||
       mw -> separator.separator_type == XmDOUBLE_DASHED_LINE)
   {
      valueMask = valueMask | GCLineStyle;
      values.line_style = LineDoubleDash;
   }

   mw -> separator.separator_GC = XtGetGC ((Widget) mw, valueMask, &values);
}




/************************************************************************
 *
 *  Redisplay
 *     Invoke the application exposure callbacks.
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
     XmSeparatorWidget mw = (XmSeparatorWidget) wid ;

    XmeDrawSeparator(XtDisplay(wid), XtWindow(wid),
                  mw->primitive.top_shadow_GC,
                  mw->primitive.bottom_shadow_GC,
                  mw->separator.separator_GC,
                  mw->primitive.highlight_thickness,
                  mw->primitive.highlight_thickness,
                  mw->core.width - 2*mw->primitive.highlight_thickness,
                  mw->core.height - 2*mw->primitive.highlight_thickness,
                  mw->primitive.shadow_thickness,
                  mw->separator.margin,
                  mw->separator.orientation,
                  mw->separator.separator_type);
}




/************************************************************************
 *
 *  Destroy
 *	Remove the callback lists.
 *
 ************************************************************************/
static void 
Destroy(
        Widget wid )
{
   XtReleaseGC (wid, ((XmSeparatorWidget) wid)->separator.separator_GC);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean 
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
        XmSeparatorWidget current = (XmSeparatorWidget) cw ;
        XmSeparatorWidget request = (XmSeparatorWidget) rw ;
        XmSeparatorWidget new_w = (XmSeparatorWidget) nw ;
   Boolean flag = FALSE;   

   /*
    * We never allow our traversal flags to be changed during SetValues();
    * this is enforced by our superclass.
    */

   /*  Force traversal_on to FALSE */
   new_w -> primitive.traversal_on = FALSE;

   /* Force highlightThickness to zero if in a menu. */
   if (XmIsRowColumn(XtParent(new_w)) &&
       ((RC_Type(XtParent(new_w)) == XmMENU_PULLDOWN) ||
        (RC_Type(XtParent(new_w)) == XmMENU_POPUP)))
     new_w->primitive.highlight_thickness = 0;
 
   if(    !XmRepTypeValidValue( XmRID_SEPARATOR_TYPE,
                              new_w->separator.separator_type, (Widget) new_w)    )
   {
      new_w -> separator.separator_type = current -> separator.separator_type;
   }

   if(    !XmRepTypeValidValue( XmRID_ORIENTATION,
                                 new_w->separator.orientation, (Widget) new_w)    )
   {
      new_w -> separator.orientation = current -> separator.orientation;
   }

   if (new_w -> separator.orientation == XmHORIZONTAL)
   {
      if (request -> core.width == 0)
	 new_w -> core.width = 2 * new_w->primitive.highlight_thickness + 2;

      if (request -> core.height == 0)
      {
	 new_w -> core.height = 2 * new_w -> primitive.highlight_thickness;

	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE)
	    new_w -> core.height += 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.height += new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE)
	    new_w -> core.height += 5;
	 else
	    if (new_w -> core.height == 0)
	       new_w -> core.height = 1;
      }

      if ((new_w -> separator.separator_type != current -> separator.separator_type ||
           new_w -> primitive.shadow_thickness != current -> primitive.shadow_thickness ||
           new_w -> primitive.highlight_thickness != current -> primitive.highlight_thickness) &&
	   request -> core.height == current -> core.height)
      {
	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE) 
	    new_w -> core.height = 2 * new_w -> primitive.highlight_thickness + 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.height = 2 * new_w -> primitive.highlight_thickness +
				       new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE) 
	    new_w -> core.height = 2 * new_w -> primitive.highlight_thickness + 5;
      }
   } 

   if (new_w -> separator.orientation == XmVERTICAL)
   {
      if (request -> core.height == 0)
	 new_w -> core.height = 2 * new_w->primitive.highlight_thickness + 2;

      if (request -> core.width == 0)
      {
	 new_w -> core.width = 2 * new_w -> primitive.highlight_thickness;

	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE)
	    new_w -> core.width += 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.width += new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE)
	    new_w -> core.width += 5;
	 else
	    if (new_w -> core.width == 0)
	       new_w -> core.width = 1;
      }

      if ((new_w -> separator.separator_type != current -> separator.separator_type ||
           new_w -> primitive.shadow_thickness != current -> primitive.shadow_thickness ||
           new_w -> primitive.highlight_thickness != current -> primitive.highlight_thickness) &&
	   request -> core.width == current -> core.width)
      {
	 if (new_w -> separator.separator_type == XmSINGLE_LINE ||
	     new_w -> separator.separator_type == XmSINGLE_DASHED_LINE) 
	    new_w -> core.width = 2 * new_w -> primitive.highlight_thickness + 3;
	 else if (new_w -> separator.separator_type == XmSHADOW_ETCHED_IN ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_IN_DASH ||
		  new_w -> separator.separator_type == XmSHADOW_ETCHED_OUT_DASH)
	    new_w -> core.width = 2 * new_w -> primitive.highlight_thickness +
				       new_w -> primitive.shadow_thickness;
	 else if (new_w -> separator.separator_type == XmDOUBLE_LINE ||
		  new_w -> separator.separator_type == XmDOUBLE_DASHED_LINE) 
	    new_w -> core.width = 2 * new_w -> primitive.highlight_thickness + 5;
      }
   } 

   if (new_w -> separator.orientation != current -> separator.orientation ||
       new_w -> separator.margin != current -> separator.margin ||
       new_w -> primitive.shadow_thickness != current -> primitive.shadow_thickness) 
      flag = TRUE;

   if (new_w -> separator.separator_type != current -> separator.separator_type  ||
       new_w -> core.background_pixel != current -> core.background_pixel    ||
       new_w -> primitive.foreground != current -> primitive.foreground)
   {
      XtReleaseGC ((Widget) new_w, new_w -> separator.separator_GC);
      GetSeparatorGC (new_w);
      flag = TRUE;
   }

   return (flag);
}

/************************************************************************
 *
 *  XmCreateSeparator()
 *  XmVaCreateSeparator()
 *  XmVaCreateManagedSeparator()
 *	Create an instance of a separator and return the widget id.
 *
 ************************************************************************/
Widget 
XmCreateSeparator(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return (XtCreateWidget (name, xmSeparatorWidgetClass, 
                           parent, arglist, argcount));
}
Widget 
XmVaCreateSeparator(
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
                         xmSeparatorWidgetClass, 
                         parent, False, 
                         var, count);
    va_end(var);   
    return w;
    
}
Widget 
XmVaCreateManagedSeparator(
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
                         xmSeparatorWidgetClass, 
                         parent, True, 
                         var, count);
    va_end(var);   
    return w;
    
}
