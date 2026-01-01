/* $XConsortium: CxxCommand.cxx /main/4 1995/07/17 11:44:52 drk $ */
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
 * This widget is for educational purposes only.
 * Commercial applications should NOT instantiate this widget.
 * OSF makes no guarantees on the stability of this widget.
 *
 ************************************ WARNING *********************************/


/*******************************************************************************
 *
 * CxxCommand.cc - C++ ExmCommandButton widget.  
 *                 This widget displays a button that,
 *                 when activated, initiates a callback.
 *                 This widget demonstrates how to
 *                  * create Motif-style button visuals.
 *                  * establish Motif-style actions for buttons. 
 *                  * establish the XmQTactivatable trait.
 *                  * establish the XmQTtakesDefault trait.
 *                  * establish a simple Motif callback.
 *                 See the "OSF/Motif Widget Writer's Guide" for details.
 *
 ******************************************************************************/

/* Include appropriate files. */
#include "CxxCommand.h" /* header file for ExmCommandButton widget */
#include <Xm/DrawP.h>  /* header file for Xme drawing functions */
#include <Xm/TraitP.h> /* header file for Trait functions */
#include <Xm/ActivatableT.h> /* header file for XmQTactivatable trait */
#include <Xm/TakesDefT.h> /* header file for XmQTtakesDefault trait */
#include <Xm/SpecRenderT.h> /* header file for render table constants */

/* The VISUAL_DELAY constant holds a time in milliseconds. */
static const unsigned long VISUAL_DELAY = 100;

/* Define the resources for the ExmCxxCommandButton widget.  This widget 
   supports one new resource: XmNactivateCallback.  In addition,
   this widget overrides the default values of two resources in XmPrimitive:
   XmNshadowThickness and XmNtraversalOn. */
static XtResource resources[] = 
{
    {
	XmNshadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( ExmCxxCommand, primitive.shadow_thickness),
	XmRImmediate,
	(XtPointer) 3  
    },
    {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof (Boolean),
	XtOffsetOf( ExmCxxCommand, primitive.traversal_on),
	XmRImmediate, 
	(XtPointer) True
    },
    {
	XmNactivateCallback,
	XmCCallback,
	XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( ExmCxxCommand, command.activate_callback),
	XmRPointer, 
        (XtPointer) NULL
    }
};

/* ExmCxxCommandButton does not treat any of its resources 
   as synthetic resources. */



/***********
 * Actions *
 ***********/

class ExmCxxCommandActions {
    /*
     * This class is only meant to protect ExmCxxCommand data and methods.
     * By making this class a friend of ExmCxxCommand, one can invoque
     * protected and private members from the following methods without
     * losing the protection for the rest of the world.
     */
  public:
    
    static void Enter(Widget, XEvent*, String *params, Cardinal *nparams);
    static void Leave(Widget, XEvent*, String *params, Cardinal *nparams);
    static void ArmAndActivate(Widget, XEvent*,
			       String *params, Cardinal *nparams);
    static void VisualDisarm(XtPointer cd, XtIntervalId* id);
    static void Arm(Widget, XEvent*, String *params, Cardinal *nparams);
    static void Activate(Widget, XEvent*, String *params, Cardinal *nparams);
    static void Disarm(Widget, XEvent*, String *params, Cardinal *nparams);
};


/****************
 * Translations *
 ****************/

#define defaultTranslations "\
<EnterWindow>:		ExmCommandButtonEnter()\n\
<LeaveWindow>:		ExmCommandButtonLeave()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
<Key>osfActivate:	PrimitiveParentActivate()\n\
<Key>osfCancel:		PrimitiveParentCancel()\n\
<Key>osfHelp:		PrimitiveHelp()\n\
<Key>osfSelect:		ExmCommandButtonArmAndActivate()\n\
<Key>space: 		ExmCommandButtonArmAndActivate()\n\
<Btn1Down>:		ExmCommandButtonArm()\n\
<Btn1Up>:		ExmCommandButtonActivate() ExmCommandButtonDisarm()"


static XtActionsRec ActionsList[] = {
  {"ExmCommandButtonEnter",		ExmCxxCommandActions::Enter},
  {"ExmCommandButtonLeave",		ExmCxxCommandActions::Leave},
  {"ExmCommandButtonArmAndActivate",	ExmCxxCommandActions::ArmAndActivate},
  {"ExmCommandButtonArm",		ExmCxxCommandActions::Arm},
  {"ExmCommandButtonActivate",		ExmCxxCommandActions::Activate},
  {"ExmCommandButtonDisarm",		ExmCxxCommandActions::Disarm}
};

/************************************
 * Class declaration for CxxCommand * 
 ************************************/
ExmCxxCommandClass::ExmCxxCommandClass(char*		name,
				     WidgetClass	parentClass,
				     Cardinal		widgetSize,
				     XtProc		class_init,
				     XtActionList	actions,
				     Cardinal		num_actions,
				     XtResourceList	res,
				     Cardinal		num_res,
				     XtInitProc		cxx_cast)
    : ExmCxxStringClass(name, parentClass, widgetSize, class_init,
                        actions, num_actions, res, num_res, cxx_cast)
{
 // Override all core class record fields that need to be overrided. 
   core_class.tm_table			= defaultTranslations;
    
 // Override all primitive record fields that need to be overrided.
   // none

 // Override all primitive extension fields that need to be overrided.
   // none

 // Override all simple class record fields that need to be overrided.
   // none

 // Override all string class record fields that need to be overrided.
   default_render_table_type = XmBUTTON_RENDER_TABLE;
};

externaldef(exmCxxCommandclassrec)
ExmCxxCommandClass exmCxxCommandClassRec("ExmCxxCommand",
				       (WidgetClass) &exmCxxStringClassRec,
				       sizeof(ExmCxxCommand),
				       ExmCxxCommandClass::ClassInitialize,
				       ActionsList,
				       XtNumber(ActionsList),
				       resources,
				       XtNumber(resources),
				       ExmCxxCommand::_MakeCxxWidget);


externaldef(exmCxxCommandwidgetclass)
WidgetClass exmCxxCommandWidgetClass = (WidgetClass) &exmCxxCommandClassRec;

/* Declare an Activatable Trait record variable. */
static const XmActivatableTraitRec commandButtonAT = {
  0,            /* version */
  ExmCxxCommand::ChangeCB,
};

/* Declare a Takes Default record variable. */
static const XmTakesDefaultTraitRec  commandButtonTDT = {
  0,            /* version */
  ExmCxxCommand::ShowAsDefault,
};

/******************************************************************************
 *
 *  Destructor
 *      Called by the Intrinsics whenever this widget is deallocated.
 *
 *****************************************************************************/
ExmCxxCommand::~ExmCxxCommand() 
{
 // If the widget is deallocated during the middle of a time out, remove
 // the time out.  (In other words, if we didn't do this, then we could
 // only deallocate the widget after any time outs had expired. 
   if (command.visual_timer != 0)
     XtRemoveTimeOut(command.visual_timer);
}



/******************************************************************************
 *
 * ClassInitialize
 *
 *****************************************************************************/
void ExmCxxCommandClass::ClassInitialize() 
{
 // The following statement is not required in the C version of this widget;
 // however, you must provide this statement in the C++ version.
   exmCxxCommandClassRec.SetBaseClassExtensionQuark();
}


/******************************************************************************
 *
 *  _MakeCxxWidget
 *	Call `new' to make each widget a C++ object
 *
 *****************************************************************************/
void ExmCxxCommand::_MakeCxxWidget(Widget /*req*/, Widget new_w,
				  ArgList, Cardinal*) {
    ExmCxxCommandWidget dummy = IDLE_NEW(new_w) ExmCxxCommand;
}

/******************************************************************************
 *
 *  ClassPartInitialize
 *      Called by the Intrinsics when this widget or a subclass of this
 *      widget is instantiated.
 *
 ****************************************************************************/
void ExmCxxCommandClass::class_part_initialize() 
{
  ExmCxxStringClass::class_part_initialize();

  /* Install the XmQTactivatable trait on CommandButton and its subclasses. */
  XmeTraitSet((XtPointer) this, XmQTactivatable, 
	      (XtPointer) &commandButtonAT);

  /* Install the XmQTtakesDefault trait on CommandButton and its subclasses. */
  XmeTraitSet((XtPointer) this, XmQTtakesDefault, 
	      (XtPointer) &commandButtonTDT);
}



/************************************************************************
 *
 * initialize : virtual
 *
 ************************************************************************/
void ExmCxxCommand::initialize(Widget rw, ArgList args, Cardinal* nargs) 
{
 // Simulate chaining by calling the initialize method of ExmCxxCommand's
 // superclass (ExmCxxString).
   ExmCxxString::initialize(rw, args, nargs);
    
 // Initialize some fields.
   command.armed		= FALSE;
   command.visual_armed	= FALSE;
   command.visual_timer	= 0;

 // Reconfigure.
   reconfigure(exmCxxCommandWidgetClass, NULL);   
}


/************************************************************************
 *
 *  set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values() should start by
 *     calling <superclass>::set_values()
 *
 ************************************************************************/
Boolean ExmCxxCommand::set_values(Widget    old_w, 
                                  Widget    request_w,
				  ArgList   args, 
                                  Cardinal *nargs) 
{
 Boolean redisplayFlag = False;

 // Simulate chaining by calling the set_values method of this widget's
 // superclass (ExmCxxString).
   redisplayFlag = ExmCxxString::set_values(old_w, request_w, args, nargs);

 // If the simple.need_to_reconfigure flag changes, trigger a redisplay. 
   if (simple.need_to_reconfigure == True) {
     reconfigure(exmCxxCommandWidgetClass, old_w);
     redisplayFlag = True;
   }

   return (redisplayFlag);
}

/************************************************************************
 *
 *  DrawShadow
 *      Draw armed/disarmed shadow
 *
 ************************************************************************/
void ExmCxxCommand::draw_shadow() {
    if (core.width > 2 * primitive.highlight_thickness &&
	core.height > 2 * primitive.highlight_thickness &&
	primitive.shadow_thickness > 0) {
      XmeDrawShadows(XtDisplay(this), XtWindow(this),
		     primitive.top_shadow_GC,
		     primitive.bottom_shadow_GC,
		     primitive.highlight_thickness,
		     primitive.highlight_thickness,
		     core.width - 2 * primitive.highlight_thickness,
		     core.height - 2 * primitive.highlight_thickness,
		     primitive.shadow_thickness,
                     command.visual_armed ? XmSHADOW_ETCHED_IN :
		     		 	    XmSHADOW_ETCHED_OUT); 
    }
}

/**********************************************************************
 *
 * ArmAndActivate
 *
 *********************************************************************/
void ExmCxxCommandActions::ArmAndActivate(Widget w, XEvent *event,
					 String* /* params */,
					 Cardinal* /* nparams */) 
{
    ExmCxxCommandWidget cw = (ExmCxxCommandWidget) w;
    XmAnyCallbackStruct cb;
    
    cw->command.visual_armed = TRUE;
    cw->draw_shadow();
    
    XFlush (XtDisplay (cw));
    
    if (cw->command.activate_callback) {
	cb.reason = XmCR_ACTIVATE;
	cb.event = event;
	XtCallCallbackList((Widget) cw, cw->command.activate_callback, &cb);
    }
    
 // Provide a short delay prior to the appearance of any new windows created
 // by the callback.  The net effect is that the ExmCommandButton will appear
 // to flash on and off immediately prior to the appearance of any window that
 // may overwrite it. 
   if ((cw->core.being_destroyed == FALSE) &&
       (cw->command.visual_timer == 0))
     cw->command.visual_timer =
            XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)cw),
			    (unsigned long) VISUAL_DELAY,
			    ExmCxxCommandActions::VisualDisarm,
			    (XtPointer) (cw));
}


/**********************************************************************
 *
 * Enter
 *
 *********************************************************************/
void ExmCxxCommandActions::Enter(Widget w, XEvent *event,
				 String *params, Cardinal *nparams) 
{
  ExmCxxCommandWidget cw = (ExmCxxCommandWidget) w;

  if (cw->command.armed == True) {
    /* In order for this code to be executed, the user had to have just 
       performed the following steps:
       1. moved the cursor into the widget window.
       2. armed the widget.
       3. moved the cursor out of the widget window.
       4. moved the cursor back into the widget window. */
    cw->command.visual_armed = True;

    cw->draw_shadow(); 
  }

  // Call the PrimitiveEnter function defined by the XmPrimitive widget. 
  XtCallActionProc (w, "PrimitiveEnter", event, params, *nparams);
}


/**********************************************************************
 *
 * Leave
 *
 *********************************************************************/
void ExmCxxCommandActions::Leave(Widget w, XEvent *event,
				 String *params, Cardinal *nparams) 
{
  ExmCxxCommandWidget cw = (ExmCxxCommandWidget)w;

   if (cw->command.armed == True) {
     /* In order for this code to be executed, the user had to have:
	1. moved the cursor into the widget window.
	2. armed the widget.
	3. moved the cursor out of the widget window. */
     cw->command.visual_armed = False;

     cw->draw_shadow(); 
   }

  XtCallActionProc (w, "PrimitiveLeave", event, params, *nparams);
}


/******************************************************************************
 *
 * Arm:
 *     Called by the Intrinsics in response to a <Btn1Down> event.
 *
 *****************************************************************************/
void ExmCxxCommandActions::Arm(Widget w, XEvent* /* event */,
			      String* /* params */, Cardinal* /* nparams */) 
{
  ExmCxxCommandWidget cw = (ExmCxxCommandWidget) w;
    
  XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    
  cw->command.armed = TRUE;
  cw->command.visual_armed = TRUE;
  cw->draw_shadow();
}


/******************************************************************************
 *
 * Activate:
 *     Called by the Intrinsics in response to a <Btn1Up> event.
 *
 *****************************************************************************/
void ExmCxxCommandActions::Activate(Widget w, XEvent *event,
				   String* /*params*/, Cardinal* /*nparams*/) 
{
  ExmCxxCommandWidget cw = (ExmCxxCommandWidget) w;
  XmAnyCallbackStruct cb;
  Dimension bw = cw->core.border_width;
    
  // Activate the callback routine, but only if the activation event happened
  // somewhere within the widget's window.  Note that the widget's window
  // includes all the pixels of the widget including the border highlight
  // and the shadow. 
  if (cw->command.activate_callback) {
    cb.reason = XmCR_ACTIVATE;
    cb.event = event;
    XFlush(XtDisplay(cw));
    XtCallCallbackList((Widget) cw, cw->command.activate_callback,&cb);
  }
}

/******************************************************************************
 *
 * Disarm:
 *    Called by the Intrinsics after every call to the Activate method.
 *
 ******************************************************************************/
void ExmCxxCommandActions::Disarm(Widget w, XEvent*,
				 String* /*params*/, Cardinal* /*nparams*/) 
{
  ExmCxxCommandWidget cw = (ExmCxxCommandWidget) w;
    
  // Change the appearance of the shadow to indicate that the widget is no
  // longer armed. 
  cw->command.armed = FALSE;
  cw->command.visual_armed = FALSE;
  cw->draw_shadow();
}


/**********************************************************************
 *
 * VisualDisarm:
 *      Called by ArmAndActivate.
 *
 *********************************************************************/
void ExmCxxCommandActions::VisualDisarm(XtPointer cd, XtIntervalId* /*id*/) 
{
  ExmCxxCommand *cw = (ExmCxxCommand *) cd;
    
  // Change back the shadow appearance at the end of the timeout period.
  cw->command.visual_timer = 0;
  cw->command.visual_armed = FALSE;
    
  if (XtIsRealized((Widget) cw) && XtIsManaged((Widget) cw))
     cw->draw_shadow();
    
   XFlush(XtDisplay(cw));
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
void ExmCxxCommand::ChangeCB(Widget w, XtCallbackProc activCB,
			     XtPointer closure, Boolean setunset)
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
void ExmCxxCommand::ShowAsDefault(Widget w, XtEnum state)
{
  ExmCxxCommand *cbw = (ExmCxxCommand *)w;
  Position   start_x_of_outer_shadow,  start_y_of_outer_shadow; 
  Dimension  margin_push_out;
  Dimension  width_of_outer_shadow, height_of_outer_shadow; 
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
    outer_shadow_type = cbw->command.visual_armed ? 
      XmSHADOW_ETCHED_IN : XmSHADOW_ETCHED_OUT; 
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

/************************************************************************
 *
 *  ExmCreateCxxCommand
 *      Externally accessable function for creating a CxxCommand widget
 *      Called by an application
 *
 ************************************************************************/
Widget ExmCreateCxxCommand(Widget parent, char *name,
			   ArgList args, Cardinal nargs) 
{
  return (XtCreateWidget(name, exmCxxCommandWidgetClass, parent, args,nargs));
}
