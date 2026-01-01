/* $XConsortium: CommandB.c /main/5 1995/07/15 20:40:23 drk $ */
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
/************************************ WARNING **********************************
 *
 * ExmCommandButton is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way." 
 *
 ************************************ WARNING *********************************/


/*******************************************************************************
 *
 * CommandB.c - ExmCommandButton widget.  This widget displays a button that,
 *              when activated, initiates a callback.
 *              This widget demonstrates how to
 *                  * create Motif-style button visuals.
 *                  * establish Motif-style actions for buttons. 
 *                  * establish the XmQTactivatable trait.
 *                  * establish the XmQTtakesDefault trait.
 *                  * establish a simple Motif callback.
 *              See the "OSF/Motif Widget Writer's Guide" for details.
 *
 ******************************************************************************/

/* Include appropriate files. */
#include <Exm/CommandBP.h> /* private header file for ExmCommandButton widget */
#include <Xm/DrawP.h>  /* header file for Xme drawing functions */
#include <Xm/TraitP.h> /* header file for XmeTraitSet and XmeTraitGet. */
#include <Xm/ActivatableT.h> /* header file for XmQTactivatable trait */
#include <Xm/TakesDefT.h> /* header file for XmQTtakesDefault trait */
#include <Xm/SpecRenderT.h> /* header file for render table constants */

/* The VISUAL_DELAY constant holds a time in milliseconds. */
static const unsigned long  VISUAL_DELAY = 100;

/* Declare all static functions. */
static void Initialize (
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void ClassInitialize(void);
static void ClassPartInitialize(
                        WidgetClass widgetclass);
static void Destroy (
                        Widget w);
static Boolean SetValues (
                        Widget old_w,
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void DrawShadow ( 
                        Widget w);

static void ExmCommandButtonEnter (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmCommandButtonLeave (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmCommandButtonArmAndActivate (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmCommandButtonArm (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmCommandButtonActivate (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmCommandButtonDisarm (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void VisualDisarm (
                        XtPointer cd,
                        XtIntervalId *id);
static void ChangeCB(
                        Widget w,
                        XtCallbackProc activCB,
                        XtPointer closure,
                        Boolean setunset);
static void ShowAsDefault(
                        Widget w,
                        XtEnum state);

/* Define the translations for the ExmCommandButton widget. 
   The first four translations will be handled by the XmPrimitive
   widget; the last six translations will be handled by the
   ExmCommandButton widget itself. */
static char defaultTranslations[] =
"<EnterWindow>:		ExmCommandButtonEnter()\n\
<LeaveWindow>:		ExmCommandButtonLeave()\n\
~c <Btn1Down>:		ExmCommandButtonArm()\n\
<Btn1Up>:		ExmCommandButtonActivate() ExmCommandButtonDisarm()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfHelp:		PrimitiveHelp()\n\
:<Key>osfSelect:	ExmCommandButtonArmAndActivate()\n\
~s ~m ~a <Key>space:	ExmCommandButtonArmAndActivate()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()";

/* Declare the actions array. */
static XtActionsRec actions[] = {
    {"ExmCommandButtonEnter",		ExmCommandButtonEnter},
    {"ExmCommandButtonLeave",		ExmCommandButtonLeave},
    {"ExmCommandButtonArmAndActivate",	ExmCommandButtonArmAndActivate},
    {"ExmCommandButtonArm",		ExmCommandButtonArm},
    {"ExmCommandButtonActivate",	ExmCommandButtonActivate},
    {"ExmCommandButtonDisarm",		ExmCommandButtonDisarm}
};


/* Define the resources for the ExmCommandButton widget.  This widget supports
   one new resource: XmNactivateCallback.   In addition, this widget overrides 
   the default values of two resources in XmPrimitive:
   XmNshadowThickness and XmNtraversalOn. */
static XtResource resources[] = 
{
    {
	XmNshadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( ExmCommandButtonRec, primitive.shadow_thickness),
	XmRImmediate,
	(XtPointer) 3  
    },
    {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof (Boolean),
	XtOffsetOf( ExmCommandButtonRec, primitive.traversal_on),
	XmRImmediate, 
	(XtPointer) True
    },
    {
	XmNactivateCallback,
	XmCCallback,
	XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( ExmCommandButtonRec, command_button.activate_callback),
	XmRPointer, 
        (XtPointer) NULL
    },
};

/* ExmCommandButton does not treat any of its resources 
   as synthetic resources. */


/* Here is the primitive class extension record. */
static XmPrimitiveClassExtRec primClassExtRec = {
    /* next_extension */             NULL,
    /* record_type */                NULLQUARK,
    /* version */                    XmPrimitiveClassExtVersion,
    /* record_size */                sizeof(XmPrimitiveClassExtRec),
    /* widget_baseline */            XmInheritBaselineProc,
    /* widget_display_rect */        XmInheritDisplayRectProc, 
    /* widget_margins */             NULL,
};


/* Define the widget class record.  See Chapter 3 of the
   "OSF/Motif Widget Writer's Guide" for details. */
externaldef (exmcommandbuttonclassrec) ExmCommandButtonClassRec 
exmCommandButtonClassRec = {
  {    
    /* superclass */                 (WidgetClass)&exmStringClassRec,
    /* class_name */                 "ExmCommandButton",
    /* widget_size */                sizeof(ExmCommandButtonRec),
    /* class_initialize */           ClassInitialize,
    /* class_part_initialize */      ClassPartInitialize,
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    actions,
    /* num_actions */                XtNumber(actions),
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
    /* set_values */                 SetValues,
    /* set_values_hook */            NULL,
    /* set_values_almost */          XtInheritSetValuesAlmost,
    /* get_values_hook */            NULL,
    /* accept_focus */               NULL,
    /* version */                    XtVersion,
    /* callback_private */           NULL,
    /* tm_table */                   defaultTranslations,
    /* query_geometry */             XtInheritQueryGeometry,
    /* display_accelerator */        NULL,
    /* extension */                  NULL,
  },    
  { /* XmPrimitive */
    /* border_highlight */           XmInheritBorderHighlight,
    /* border_unhighlight */         XmInheritBorderUnhighlight,
    /* translations */               XtInheritTranslations,
    /* arm_and_activate */           ExmCommandButtonArmAndActivate,
    /* syn_resources */              NULL,
    /* num_syn_resources */          0,
    /* extension */                  (XtPointer)&primClassExtRec, 
  },    
  { /* ExmSimple */ 
    /* draw_visual */                ExmInheritDrawVisual,
    /* draw_shadow */                DrawShadow,
    /* create_gc */                  ExmInheritCreateGC,
    /* destroy_gc */                 ExmInheritDestroyGC,
    /* select_gc */                  ExmInheritSelectGC,
    /* calc_visual_size */           ExmInheritCalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */                ExmInheritReconfigure,
    /* extension */                  NULL,
  },    
  { /* ExmString */ 
    /* default_render_table_type */  XmBUTTON_RENDER_TABLE,
    /* extension */                  NULL,
  },    
  { /* ExmCommandButton */ 
    /* extension */                  NULL,
  },    
};    

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */
externaldef( exmcommandbuttonwidgetclass) WidgetClass
	exmCommandButtonWidgetClass = (WidgetClass) &exmCommandButtonClassRec;

/* Declare an Activatable Trait record variable. */
static const XmActivatableTraitRec commandButtonAT = {
  0,            /* version */
  ChangeCB,
};

/* Declare a Takes Default record variable. */
static const XmTakesDefaultTraitRec  commandButtonTDT = {
  0,            /* version */
  ShowAsDefault,
};

/*******************************************************************************
 *
 * Initialize:
 *     Called by the Intrinsics when this widget is instantiated.
 *
 ******************************************************************************/
static void 
Initialize (
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
           )
{
 ExmCommandButtonWidgetClass wc =(ExmCommandButtonWidgetClass)XtClass(new_w);
 ExmCommandButtonWidget nw = (ExmCommandButtonWidget) new_w;

 /* Initialize the three private fields of the ExmCommandButton widget. */
   nw->command_button.armed = False;
   nw->command_button.visual_armed = False;
   nw->command_button.visual_timer = 0;

 /* Reconfigure if necessary. 
    (This Reconfigure method is defined in the ExmSimple widget.) */
   if (wc->simple_class.reconfigure)
     (*(wc->simple_class.reconfigure))(exmCommandButtonWidgetClass, 
                                       new_w, NULL);
}


/************************************************************************
 *
 *  ClassInitialize
 *      Called by the Intrinsics the first time a widget of this class is 
 *      instantiated.
 *
 ************************************************************************/
static void 
ClassInitialize(void)
{
#ifndef USE_ORIGINAL_MOTIF_CODE
 /* Install the XmQTtakesDefault trait on CommandButton only. */
   XmeTraitSet((XtPointer) exmCommandButtonWidgetClass, XmQTtakesDefault, 
               (XtPointer) &commandButtonTDT);
#endif
}




/************************************************************************
 *
 *  ClassPartInitialize
 *      Called by the Intrinsics when this widget or a subclass of this
 *      widget is instantiated.  
 *
 ************************************************************************/
static void 
ClassPartInitialize(
        WidgetClass widgetclass )
{
 /* Install the XmQTactivatable trait on CommandButton and its subclasses. */
   XmeTraitSet((XtPointer) widgetclass, XmQTactivatable, 
               (XtPointer) &commandButtonAT);

#ifdef USE_ORIGINAL_MOTIF_CODE
 /* Install the XmQTtakesDefault trait on CommandButton and its subclasses. */
   XmeTraitSet((XtPointer) widgetclass, XmQTtakesDefault, 
               (XtPointer) &commandButtonTDT);
#endif
}


/*******************************************************************************
 *
 *  Destroy:
 *      Called by the Intrinsics when this widget is deallocated. 
 *
 ******************************************************************************/
static void 
Destroy (
        Widget w
        )
{
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;

 /* If the widget is deallocated during the middle of a time out, remove
    the time out.  (In other words, if we didn't do this, then we could
    only deallocate the widget after any time outs had expired. */ 
   if (cw->command_button.visual_timer != 0)
     XtRemoveTimeOut (cw->command_button.visual_timer);
}



/*******************************************************************************
 *
 *  SetValues:
 *      Called by the Intrinsics in response to a change in a resource value. 
 *
 ******************************************************************************/
static Boolean 
SetValues (
        Widget old_w,
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
          )
{
 ExmCommandButtonWidgetClass wc =(ExmCommandButtonWidgetClass)XtClass(new_w);
 ExmCommandButtonWidget nw = (ExmCommandButtonWidget)new_w;
 Boolean redisplayFlag = False;

 /* If the simple.reconfigure flag changes, trigger a redisplay. */ 
   if (nw->simple.need_to_reconfigure == True) {
     if (wc->simple_class.reconfigure)
       (*(wc->simple_class.reconfigure))(exmCommandButtonWidgetClass,
                                         new_w, old_w);
     redisplayFlag = True;
   }

   return (redisplayFlag);
}



/*******************************************************************************
 *
 *  DrawShadow:
 *      Called by the ExmCommandButtonEnter, ExmCommandButtonLeave, 
 *      ExmCommandButtonArmAndActivate, ExmCommandButtonArm, 
 *      ExmCommandButtonDisarm, and VisualDisarm methods. 
 *
 ******************************************************************************/
static void 
DrawShadow (
        Widget w
           )
{
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;
 unsigned int  shadow_type;

 /* This routine draws the "inner shadow" of the widget.  Unlike the other
    subclasses of \*LExmSimple\*O, the \*LExmCommandButton\*O draws two
    shadows.  One shadow, the one drawn by this routine, is always displayed.
    The other shadow is only displayed when the widget is the default button.

    The inner shadow is drawn inside the margins.  In other words, the inner
    shadow will hug the text it encloses.  The outer shadow is drawn by the
    ShowAsDefault trait method. */
   if (cw->core.width > 2 * cw->primitive.highlight_thickness &&
       cw->core.height > 2 * cw->primitive.highlight_thickness &&
       cw->primitive.shadow_thickness > 0) {
     XmeDrawShadows (XtDisplay (cw), XtWindow (cw),
   	             cw->primitive.top_shadow_GC,
   	             cw->primitive.bottom_shadow_GC,
   	             cw->primitive.highlight_thickness + 
                       cw->simple.margin_width,
   	             cw->primitive.highlight_thickness +
                       cw->simple.margin_height,
   	             cw->core.width - (2 * cw->primitive.highlight_thickness)
                       - (2 * cw->simple.margin_width),
   	             cw->core.height - (2 * cw->primitive.highlight_thickness)
                       - (2 * cw->simple.margin_height),
   	             cw->primitive.shadow_thickness,
                     cw->command_button.visual_armed ? XmSHADOW_ETCHED_IN :
                                                       XmSHADOW_ETCHED_OUT); 
   }

}



/*******************************************************************************
 *
 * ExmCommandButtonEnter:
 *     Called by the Intrinsics whenever the cursor enters a window.  
 *
 ******************************************************************************/
static void 
ExmCommandButtonEnter (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
            )
{
 ExmCommandButtonWidgetClass wc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;

   if (cw->command_button.armed == True) {
     /* In order for this code to be executed, the user had to have just 
        performed the following steps:
           1. moved the cursor into the widget window.
           2. armed the widget.
           3. moved the cursor out of the widget window.
           4. moved the cursor back into the widget window.  */
     cw->command_button.visual_armed = True;

     if (wc->simple_class.draw_shadow) 
       (*(wc->simple_class.draw_shadow)) (w);
   }

 /* Call the PrimitiveEnter function defined by the XmPrimitive widget. */
   XtCallActionProc (w, "PrimitiveEnter", event, params, *num_params);
}



/*******************************************************************************
 *
 * ExmCommandButtonLeave:
 *     Called by the Intrinsics whenever the cursor leaves the widget's window. 
 *
 ******************************************************************************/
static void 
ExmCommandButtonLeave (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
            )
{
 ExmCommandButtonWidgetClass wc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;

   if (cw->command_button.armed == True) {
     /* In order for this code to be executed, the user had to have:
           1. moved the cursor into the widget window.
           2. armed the widget.
           3. moved the cursor out of the widget window. */
     cw->command_button.visual_armed = False;

     if (wc->simple_class.draw_shadow) 
       (*(wc->simple_class.draw_shadow)) (w);
   }

   XtCallActionProc (w, "PrimitiveLeave", event, params, *num_params);
}



/*******************************************************************************
 *
 * ExmCommandButtonArmAndActivate:
 *    Called by the Intrinsics whenever the user presses <Key>osfSelect or
 *    <Key>space.
 *
 ******************************************************************************/
static void 
ExmCommandButtonArmAndActivate (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
               )

{
 ExmCommandButtonWidgetClass wc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;
 XmAnyCallbackStruct cb;

 /* Call DrawShadow. */ 
   cw->command_button.visual_armed = True;
   if (wc->simple_class.draw_shadow) 
     (*(wc->simple_class.draw_shadow)) (w);

 /* Eliminate any data waiting in the X Window's output buffer. */
   XFlush (XtDisplay (cw));

 /* Assuming that the XmNactivateCallback resource is set to True, call the 
    callback routine. */
   if (cw->command_button.activate_callback) {
     cb.reason = XmCR_ACTIVATE;
     cb.event = event;
     XtCallCallbackList ((Widget)cw, cw->command_button.activate_callback, &cb);
   }

 /* Provide a short delay prior to the appearance of any new windows created 
    by the callback.  The net effect is that the ExmCommandButton will appear
    to flash on and off immediately prior to the appearance of any window that
    may overwrite it. */ 
   if ((cw->core.being_destroyed == False) &&
       (cw->command_button.visual_timer == 0))
     cw->command_button.visual_timer = 
           XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)cw),
   			   (unsigned long) VISUAL_DELAY,
   			   VisualDisarm,
   			   (XtPointer)(cw)); /* pass this arg to VisualDisarm */
}



/*******************************************************************************
 *
 * ExmCommandButtonArm:
 *     Called by the Intrinsics in response to a <Btn1Down> event.
 *
 ******************************************************************************/
static void 
ExmCommandButtonArm (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
    )
{
 ExmCommandButtonWidgetClass wc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;
 
 /* Make this ExmCommandButton widget the current widget.  (Highlight its
    border, if appropriate. */
   XmProcessTraversal (w, XmTRAVERSE_CURRENT);

 /* Redraw the shadow to indicate arming. */
   cw->command_button.armed = True;
   cw->command_button.visual_armed = True;
   if (wc->simple_class.draw_shadow) 
     (*(wc->simple_class.draw_shadow))(w);
}



/*******************************************************************************
 *
 * ExmCommandButtonActivate:
 *     Called by the Intrinsics in response to a <Btn1Up> event.
 *
 ******************************************************************************/
static void 
ExmCommandButtonActivate (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
                     )
{
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)w;
 XmAnyCallbackStruct cb;

 if (cw->command_button.activate_callback) {
     cb.reason = XmCR_ACTIVATE;
     cb.event = event;
     XFlush (XtDisplay(cw));
     XtCallCallbackList (w, cw->command_button.activate_callback, &cb);
 }
}



/*******************************************************************************
 *
 * ExmCommandButtonDisarm:
 *    Called by the Intrinsics after every call to the Activate method. 
 *
 ******************************************************************************/
static void 
ExmCommandButtonDisarm (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
       )
{
 ExmCommandButtonWidgetClass cbwc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cbw = (ExmCommandButtonWidget)w;

 /* Change the appearance of the shadow to indicate that the widget is no
    longer armed. */ 
   cbw->command_button.armed = False;
   cbw->command_button.visual_armed = False;

   if (cbwc->simple_class.draw_shadow) 
     (*(cbwc->simple_class.draw_shadow)) (w);
}


/*******************************************************************************
 *
 * VisualDisarm:
 *     Called by ExmCommandButtonArmAndActivate. 
 *
 ******************************************************************************/
static void  
VisualDisarm (
        XtPointer client_data,
        XtIntervalId *id
             )
{
 ExmCommandButtonWidget cw = (ExmCommandButtonWidget)client_data;
 ExmCommandButtonWidgetClass wc = (ExmCommandButtonWidgetClass)XtClass(cw);
 
 /* Change back the shadow appearance at the end of the timeout period. */ 
   cw->command_button.visual_timer = 0;
   cw->command_button.visual_armed = False;

   if (XtIsRealized ((Widget)cw) && XtIsManaged ((Widget)cw))
     if (wc->simple_class.draw_shadow)
       (*(wc->simple_class.draw_shadow)) ((Widget)cw);

   XFlush (XtDisplay (cw));
}


/************************************************************************
 *
 *  Trait methods --------
 *      
 ************************************************************************/

/************************************************************************
 *
 *  ChangeCB:
 *       Called by a parent widget of ExmCommandButton to update the
 *       callback list. 
 *      
 ************************************************************************/
static void 
ChangeCB(
	 Widget w, 
	 XtCallbackProc activCB,
	 XtPointer closure,
	 Boolean setunset)
{
   if (setunset)   /* Add to the callback list. */
     XtAddCallback (w, XmNactivateCallback, activCB, closure);
   else            /* Remove from the callback list. */
     XtRemoveCallback (w, XmNactivateCallback, activCB, closure);
}


/************************************************************************
 *
 *  ShowAsDefault
 *	set up the default visual
 *      
 ************************************************************************/
static void 
ShowAsDefault(Widget w, 
	      XtEnum state)
{
 ExmCommandButtonWidgetClass cbwc = (ExmCommandButtonWidgetClass)XtClass(w);
 ExmCommandButtonWidget cbw = (ExmCommandButtonWidget)w;
 Position   start_x_of_outer_shadow,  start_y_of_outer_shadow; 
 Dimension  margin_push_out=0;
 Dimension  width_of_outer_shadow, height_of_outer_shadow; 
 int   dx, dy, width, height;
 GC    top_GC, bottom_GC;
 Dimension outer_shadow_thickness;
 int       outer_shadow_type;
 int       margins_were_pushed_out=0;
#define MARGIN_BETWEEN_HIGHLIGHT_AND_OUTER_SHADOW 2
 

   start_x_of_outer_shadow = cbw->primitive.highlight_thickness +
                               MARGIN_BETWEEN_HIGHLIGHT_AND_OUTER_SHADOW;
   start_y_of_outer_shadow = cbw->primitive.highlight_thickness +
                               MARGIN_BETWEEN_HIGHLIGHT_AND_OUTER_SHADOW;
   width_of_outer_shadow   = cbw->core.width - (2 * start_x_of_outer_shadow);
   height_of_outer_shadow  = cbw->core.height - (2 * start_y_of_outer_shadow);
   outer_shadow_thickness  = 3;

   switch (state) {
     case XmDEFAULT_READY:
         /* Push out the margins to make room for subsequent increases in the 
            shadow thickness.  The request to push out the margins will increase
            the size of the CommandButton widget assuming that its manager has
            the space to spare. */ 
          
          if (cbw->primitive.shadow_thickness < 5)
            margin_push_out = 5;
          else
            margin_push_out = cbw->primitive.shadow_thickness; 
          margins_were_pushed_out = 1;
          XtVaSetValues((Widget)cbw, 
                   XmNmarginWidth,  cbw->simple.margin_width + margin_push_out, 
                   XmNmarginHeight, cbw->simple.margin_height + margin_push_out, 
                   NULL); 
	  break ;
     case XmDEFAULT_ON :
         /* Draw an outer shadow.  The outer shadow is drawn outside the
            widget's margins but inside the border highlight. 
            The inner shadow is drawn by the DrawShadow method. */ 
          top_GC = cbw->primitive.top_shadow_GC;
          bottom_GC = cbw->primitive.bottom_shadow_GC;
          outer_shadow_type = cbw->command_button.visual_armed ? 
                                                    XmSHADOW_ETCHED_IN :
                                                    XmSHADOW_ETCHED_OUT; 
          XmeDrawShadows(XtDisplay(w), XtWindow(w),
                         top_GC, 
                         bottom_GC,
                         start_x_of_outer_shadow,
                         start_y_of_outer_shadow,
                         width_of_outer_shadow,
                         height_of_outer_shadow,
                         outer_shadow_thickness, 
                         outer_shadow_type);
	  break ;
     case XmDEFAULT_OFF :
        /* Erase the outer shadow when the widget is no longer the default. */
          XmeClearBorder(XtDisplay(w), XtWindow(w),
                         start_x_of_outer_shadow,
                         start_y_of_outer_shadow,
                         width_of_outer_shadow,
                         height_of_outer_shadow,
                         outer_shadow_thickness);
	  break ;
     case XmDEFAULT_FORGET :
     default :
        /* The widget is not a potential default button.  If XmDEFAULT_FORGET
           is called at some point after XmDEFAULT_READY was called, then
           we have to restore the margins back to their original size. */
          if (margins_were_pushed_out)
            XtVaSetValues((Widget)cbw, 
                     XmNmarginWidth,  cbw->simple.margin_width - margin_push_out,
                     XmNmarginHeight, cbw->simple.margin_height - margin_push_out,
                     NULL); 
	  break ;
    }
}
	


/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/

/*******************************************************************************
 *
 *  ExmCreateCommandButton:
 *      Called by an application.
 *
 ******************************************************************************/
Widget 
ExmCreateCommandButton (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                       )

{
 /* Convenience function to instantiate an ExmCommandButton widget. */
   return (XtCreateWidget(name,exmCommandButtonWidgetClass,
                          parent,arglist,argCount));
}

