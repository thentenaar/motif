/* $TOG: MenuB.c /main/6 1999/05/13 15:33:01 mgreess $ */
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
 *  ExmMenuButton is a demonstration widget.  OSF provides this widget
 *  solely to teach programmers how to write their own Motif widgets.
 *  OSF does not support this widget in any way
 *
 ************************************ WARNING *********************************/


/*******************************************************************************
 *
 * MenuB.c - ExmMenuButton widget.  This widget serves as a menu 
 *           button.  It has some features of the XmPushButton widget.
 *           This widget can be a child of a RowColumn widget, 
 *           assuming that the XmNrowColumnType resource of the 
 *           RowColumn is set to something other than XmWORK_AREA. 
 *           ExmMenuButton is a subclass of ExmString and displays
 *           one compound string. 
 *           The ExmMenuButton widget demonstrates how to  
 *               * install the XmQTmenuSavvy trait. 
 *               * call many of the trait methods of the XmQTmenuSystem 
 *                 trait. 
 *            See the "OSF/Motif Widget Writer's Guide" for details.
 *
*******************************************************************************/


/* Include appropriate header files. */
#include <Xm/Screen.h>
#include <Xm/CascadeB.h>
#include <Xm/DrawP.h>
#include <Xm/ManagerP.h>
#include <X11/ShellP.h>
#include <Xm/TraitP.h>
#include <Xm/MenuT.h>
#include <Xm/SpecRenderT.h> /* header file for XmBUTTON_RENDER_TABLE */
#include <Xm/UniqueEvnI.h>
#include <X11/keysymdef.h>
#include <Exm/MenuBP.h>

/* The VISUAL_DELAY holds a time in milliseconds. */
XmConst unsigned long VISUAL_DELAY=100;

/* Declare all static functions. */
static void Initialize (
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void ClassInitialize(void);
static void Destroy (
                        Widget w);
static Boolean SetValues (
                        Widget old_w,
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args);
static void BorderHighlight (
                        Widget w);
static void BorderUnhighlight (
                        Widget w);
static void DrawShadow (
                        Widget w);
static void ArmAndActivate (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void VisualDisarm (
                        XtPointer cd,
                        XtIntervalId *id);
static void BtnDown (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void BtnUp (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void MenuButtonEnter (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void MenuButtonLeave (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void MenuButtonHelp (
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void DisableCallback(
                        Widget w,
                        XtEnum value);
static char* GetAccelerator(
                        Widget w);
static KeySym GetMnemonic(
                        Widget w);
static char* GetActivateCBName(
                        void);
static void DrawVisual( 
                        Widget w);
static void CalcVisualSize ( 
                        Widget w);


/* Define translations. */
static char defaultTranslations[] =
"<EnterWindow>:		MenuButtonEnter()\n\
<LeaveWindow>:		MenuButtonLeave()\n\
<BtnDown>:		BtnDown()\n\
<BtnUp>:		BtnUp()\n\
:<Key>osfActivate:	ArmAndActivate()\n\
:<Key>osfCancel:	MenuEscape()\n\
:<Key>osfHelp:		MenuButtonHelp()\n\
~s ~m ~a <Key>Return:	ArmAndActivate()\n\
~s ~m ~a <Key>space:	ArmAndActivate()";

static char menuTraversalTranslations[] =
"<Unmap>:		Unmap()\n\
<FocusOut>:		FocusOut()\n\
<FocusIn>:		FocusIn()\n\
:<Key>osfCancel:	MenuEscape()\n\
:<Key>osfLeft:		MenuTraverseLeft()\n\
:<Key>osfRight:		MenuTraverseRight()\n\
:<Key>osfUp:		MenuTraverseUp()\n\
:<Key>osfDown:		MenuTraverseDown()";


/* The following actions will be handled by code inside this file. */ 
static XtActionsRec Actions[] = {
	{"ArmAndActivate",	ArmAndActivate},
	{"BtnDown",		BtnDown},
	{"BtnUp",		BtnUp},
	{"MenuButtonEnter",	MenuButtonEnter},
	{"MenuButtonLeave",	MenuButtonLeave},
	{"MenuButtonHelp",	MenuButtonHelp}
};


/* Define the resources.  The first four resource definitions are 
   do not define new resources, they simply provide new default values
   for superclass resources.  The only new resource is XmNactivateCallback. */ 
static XtResource resources[] = 
{
    {
	XmNshadowThickness,
	XmCShadowThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( ExmMenuButtonRec, primitive.shadow_thickness),
	XmRImmediate,
	(XtPointer) 2
    },
    {
	XmNhighlightThickness,
	XmCHighlightThickness,
	XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( ExmMenuButtonRec, primitive.highlight_thickness),
	XmRImmediate,
	(XtPointer) 0
    },
    {
	XmNtraversalOn,
	XmCTraversalOn,
	XmRBoolean,
	sizeof (Boolean),
	XtOffsetOf( ExmMenuButtonRec, primitive.traversal_on),
	XmRImmediate, 
	(XtPointer) True
    },
    {
        XmNalignment,
        XmCAlignment,
        XmRAlignment,
        sizeof(unsigned char),
        XtOffsetOf( ExmStringRec,string.alignment),
        XmRImmediate,
        (XtPointer) XmALIGNMENT_BEGINNING
    },

    {
	XmNactivateCallback,
	XmCCallback,
	XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( ExmMenuButtonRec, menu_button.activate_callback),
	XmRPointer, 
        (XtPointer) NULL
    },

    {
        XmNmnemonic, 
	XmCMnemonic, 
	XmRKeySym,
	sizeof(KeySym), 
	XtOffsetOf( ExmMenuButtonRec, menu_button.mnemonic),
	XmRImmediate, 
	(XtPointer) XK_VoidSymbol
    },

    {
        XmNaccelerator,
	XmCAccelerator,
	XmRString,
	sizeof(char *), 
	XtOffsetOf(ExmMenuButtonRec, menu_button.accelerator),
	XmRImmediate, 
	(XtPointer) NULL
    },

    {
        XmNacceleratorText,
	XmCAcceleratorText,
	XmRXmString,
	sizeof(XmString), 
	XtOffsetOf(ExmMenuButtonRec, menu_button.accelerator_text),
	XmRImmediate,
	(XtPointer) NULL
    },

    {
        XmNmnemonicCharSet, 
	XmCMnemonicCharSet, 
	XmRString,
	sizeof(XmStringCharSet), 
	XtOffsetOf(ExmMenuButtonRec, menu_button.mnemonic_charset),
	XmRImmediate, 
	(XtPointer) XmFONTLIST_DEFAULT_TAG    
    }
};

/* No synthetic resources. */


/* Here is the class record. */
externaldef (exmmenubuttonclassrec) ExmMenuButtonClassRec exmMenuButtonClassRec = {
  {
    /* superclass */                 (WidgetClass)&exmStringClassRec,
    /* class_name */                 "ExmMenuButton",
    /* widget_size */                sizeof(ExmMenuButtonRec),
    /* class_initialize */           ClassInitialize,
    /* class_part_initialize */      NULL,
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    Actions,
    /* num_actions */                XtNumber(Actions),
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
    /* border_highlight */           BorderHighlight,
    /* border_unhighlight */         BorderUnhighlight,
    /* translations */               menuTraversalTranslations,
#if 0
    /* translations */               XtInheritTranslations,
#endif
    /* arm_and_activate */           ArmAndActivate,
    /* syn_resources */              NULL,
    /* num_syn_resources */          0,
    /* extension */                  NULL,
  },    
  { /* ExmSimple */     
    /* draw_visual */                DrawVisual,
    /* draw_shadow */                DrawShadow,
    /* create_text_gc */             ExmInheritCreateGC,
    /* destroy_text_gc */            ExmInheritDestroyGC,
    /* select_text_gc */             ExmInheritSelectGC,
    /* calc_visual_size */           CalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */		     ExmInheritReconfigure,
    /* extension */                  NULL,
  },    
  { /* ExmString */   
    /* default_render_table_type */  XmBUTTON_RENDER_TABLE,
    /* extension */                  NULL,
  },    
  { /* ExmMenuButton */  
    /* extension */                  NULL,
  }    
};


/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */
externaldef( exmmenubuttonwidgetclass) WidgetClass
	exmMenuButtonWidgetClass = (WidgetClass) &exmMenuButtonClassRec;


/* Define trait structure variables. */
/* We will need to tell XmRowColumn that ExmMenuButton can be a menu child.
   We will do that be setting the XmQTmenuSavvy trait on ExmMenuButton.
   In order to set this trait, we must define a trait record variable
   as follows: */ 
static XmConst XmMenuSavvyTraitRec menuSavvyTraitRec = {
  0,					 	     /* Version */
  (XmMenuSavvyDisableProc) DisableCallback,	     /* disableCallback	  */
  GetAccelerator,				     /* getAccelerator	  */
  GetMnemonic,					     /* getMnemonic	  */
  GetActivateCBName,				     /* getActivateCBName */
};


/******************************************************************************
 *
 * ClassInitialize:
 *     Called by the Intrinsics the first time a widget of this class is 
 *     instantiated. 
 *
 *****************************************************************************/
static void 
ClassInitialize(
        void
               )
{
  /* Install the menuSavvy trait so that the menu will know
     that this class has the right actions for menus */
  XmeTraitSet(exmMenuButtonWidgetClass, XmQTmenuSavvy, 
              (XtPointer) &menuSavvyTraitRec);
}


/******************************************************************************
 *
 * Initialize
 *
 *****************************************************************************/
static void 
Initialize (
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
           )
{
  ExmMenuButtonWidgetClass wc =(ExmMenuButtonWidgetClass)XtClass(new_w);
  ExmMenuButtonWidget nw = (ExmMenuButtonWidget)new_w;
 
  nw->menu_button.armed = False;
  nw->menu_button.visual_timer = 0;
  nw->menu_button.skip_callback = False;
  
  /* Force traversal on,  the menu system will break otherwise */
  nw->primitive.traversal_on = True;
  nw->primitive.highlight_thickness = 0;
  
  if (wc->simple_class.calc_visual_size)
    (*(wc->simple_class.calc_visual_size))(new_w);
  
  if (wc->simple_class.reconfigure)
    (*(wc->simple_class.reconfigure))(exmMenuButtonWidgetClass, new_w, NULL);
  
  if (nw->menu_button.accelerator_text != NULL)
    nw->menu_button.accelerator_text = 
      XmStringCopy(nw->menu_button.accelerator_text);
}



/******************************************************************************
 *
 *  Destroy
 *
 *****************************************************************************/
static void 
     Destroy (
	      Widget w
	      )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  if (mw->menu_button.visual_timer != 0)
    XtRemoveTimeOut (mw->menu_button.visual_timer);
}





/******************************************************************************
 *
 *  SetValues
 *
 *****************************************************************************/
static Boolean 
SetValues (
        Widget old_w,
        Widget request_w,
        Widget new_w,
        ArgList args,
        Cardinal *num_args
          )
{
  ExmMenuButtonWidgetClass wc =(ExmMenuButtonWidgetClass)XtClass(new_w);
  ExmMenuButtonWidget nw = (ExmMenuButtonWidget) new_w;
  ExmMenuButtonWidget cw = (ExmMenuButtonWidget) old_w;
  Boolean redisplayFlag = False;
  
  /* should we force these ? - probably to protect from *highlightThickness*/
  nw->primitive.highlight_thickness = 0;
  nw->primitive.traversal_on = True;
  
  if (wc->simple_class.reconfigure) {
    (*(wc->simple_class.reconfigure))(exmMenuButtonWidgetClass, new_w, old_w);
    redisplayFlag = True;
  }
  if (nw->menu_button.accelerator_text != NULL)
    nw->menu_button.accelerator_text = 
      XmStringCopy(nw->menu_button.accelerator_text);
  
  if ((nw->menu_button.accelerator_text != cw->menu_button.accelerator_text) ||
      (nw->menu_button.mnemonic != cw->menu_button.mnemonic))
    redisplayFlag = True;
  
  return (redisplayFlag);
}



/******************************************************************************
 *
 *  BorderHighlight
 *
 *****************************************************************************/
static void 
BorderHighlight (
        Widget w
                )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  if (mw->menu_button.armed == True) return;

#ifndef USE_ORIGINAL_MOTIF_CODE
  XmeDrawShadows (XtDisplay (mw), XtWindow (mw),
		  mw->primitive.top_shadow_GC,
		  mw->primitive.bottom_shadow_GC,
		  mw->primitive.highlight_thickness,
		  mw->primitive.highlight_thickness,
		  mw->core.width - 2 * mw->primitive.highlight_thickness,
		  mw->core.height - 2 * mw->primitive.highlight_thickness,
		  mw->primitive.shadow_thickness, XmSHADOW_OUT);
#else
  mw->menu_button.armed = True;
  
  if (wc->simple_class.draw_shadow)
    (*(wc->simple_class.draw_shadow)) ((Widget) mw);
#endif
}


/******************************************************************************
 *
 *  BorderUnhighlight:
 *      Called when ...
 *
 *****************************************************************************/
static void 
BorderUnhighlight (
        Widget w
                  )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  if (mw->menu_button.armed == False) 
    return;

#ifndef USE_ORIGINAL_MOTIF_CODE  
  XmeClearBorder (XtDisplay (mw), XtWindow(mw),
		  mw->primitive.highlight_thickness,
		  mw->primitive.highlight_thickness,
		  mw->core.width - 2 * mw->primitive.highlight_thickness,
		  mw->core.height - 2 * mw->primitive.highlight_thickness,
		  mw->primitive.shadow_thickness);
#else
  mw->menu_button.armed = False;
  
  if (wc->simple_class.draw_shadow)
    (*(wc->simple_class.draw_shadow)) ((Widget) mw);
#endif
}


/******************************************************************************
 *
 *  DrawVisual
 *
 *****************************************************************************/
static void 
DrawVisual (
        Widget w
           )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  /* If the compound string is not NULL and if there is enough space in the
     widget to draw at least a little portion of the compound string, then 
     render the string with XmStringDraw. */
  if (mw->string.compound_string &&
      (mw->simple.visual.width != 0) &&
      (mw->simple.visual.height != 0)) {
    if (mw->menu_button.mnemonic != XK_VoidSymbol) {
      char mnemonic_str[2];
      XmString tmp;

      mnemonic_str[0] = mw->menu_button.mnemonic & 0xFF;
      mnemonic_str[1] = 0;

      tmp = XmStringCreate(mnemonic_str,
			   mw->menu_button.mnemonic_charset);
      XmStringDrawUnderline(XtDisplay(mw), XtWindow(mw),
			    mw->string.render_table, 
			    mw->string.compound_string,
			    wc->simple_class.select_gc(w),
			    mw->simple.visual.x, mw->simple.visual.y,
			    mw->simple.visual.width, mw->string.alignment,
			    mw->primitive.layout_direction, NULL,
			    tmp); 
      XmStringFree(tmp);
    } else {
      XmStringDraw(XtDisplay(mw), XtWindow(mw),
		   mw->string.render_table, 
		   mw->string.compound_string,
		   wc->simple_class.select_gc(w),
		   mw->simple.visual.x, mw->simple.visual.y,
		   mw->simple.visual.width, mw->string.alignment,
		   mw->primitive.layout_direction, NULL); 
    }
    
    /* Draw the accelerator text */
    if (mw->menu_button.accelerator_text) {
      Dimension height, width, offset;

      offset = mw->primitive.highlight_thickness +
	       mw->primitive.shadow_thickness;
      XmStringExtent (mw->string.render_table, 
		      mw->menu_button.accelerator_text, &width, &height);
      XmStringDraw(XtDisplay(mw), XtWindow(mw),
		   mw->string.render_table, 
		   mw->menu_button.accelerator_text,
		   wc->simple_class.select_gc(w),
		   XtWidth(w) - width - offset,
		   mw->simple.visual.y,
		   mw->simple.visual.width, mw->string.alignment,
		   mw->primitive.layout_direction, NULL); 
    }

    XmeClearBorder(XtDisplay(mw), XtWindow(mw),
		   (int)0, (int)0, 
		   (Dimension)mw->core.width, (Dimension)mw->core.height, 
		   (Dimension)(mw->primitive.highlight_thickness +
			       mw->primitive.shadow_thickness)
                   );
  }
}


/******************************************************************************
 *
 * CalcVisualSize
 *
 *****************************************************************************/
static void 
CalcVisualSize (
        Widget w
               )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  /* Ideally, how much space would the compound string require? */
  if (mw->string.compound_string && 
      !XmStringEmpty(mw->string.compound_string))
    XmStringExtent (mw->string.render_table, mw->string.compound_string,
		    &(mw->simple.visual.width), &(mw->simple.visual.height));
  else {
    /* The string is empty, so it requires no space. */
    mw->simple.visual.width = 0;
    mw->simple.visual.height = 0;
  }
  
  /* Now add the space for the accelerator text */
  if (mw->menu_button.accelerator_text) {
    Dimension height, width;
    int h_unit;
    Widget scr = XmGetXmScreen(XtScreen(w));
    Arg args[1];
    
    XtSetArg(args[0], XmNhorizontalFontUnit, &h_unit); 
    XtGetValues(scr, args, 1);
    
    XmStringExtent (mw->string.render_table, 
		    mw->menu_button.accelerator_text, &width, &height);
    mw->simple.visual.width += width + (2 * h_unit);
    if (height > mw->simple.visual.height) 
      mw->simple.visual.height = height;
  }
}


/************************************************************************
 *
 *  DrawShadow
 *
 ************************************************************************/
static void 
     DrawShadow (
		 Widget w
		 )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  if (mw->core.width > 2 * mw->primitive.highlight_thickness &&
      mw->core.height > 2 * mw->primitive.highlight_thickness &&
      mw->primitive.shadow_thickness > 0) {
    
    if (mw->menu_button.armed)
      XmeDrawShadows (XtDisplay (mw), XtWindow (mw),
		      mw->primitive.top_shadow_GC,
		      mw->primitive.bottom_shadow_GC,
		      mw->primitive.highlight_thickness,
		      mw->primitive.highlight_thickness,
		      mw->core.width - 2 * mw->primitive.highlight_thickness,
		      mw->core.height - 2 * mw->primitive.highlight_thickness,
		      mw->primitive.shadow_thickness, XmSHADOW_OUT);
    else
      XmeClearBorder (XtDisplay (mw), XtWindow(mw),
		      mw->primitive.highlight_thickness,
		      mw->primitive.highlight_thickness,
		      mw->core.width - 2 * mw->primitive.highlight_thickness,
		      mw->core.height - 2 * mw->primitive.highlight_thickness,
		      mw->primitive.shadow_thickness);
  }
}



/**********************************************************************
 *
 * MenuButtonEnter
 *
 *********************************************************************/
static void 
     MenuButtonEnter (
		      Widget w,
		      XEvent *event,
		      String *params,
		      Cardinal *num_params 
		      )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  XmMenuSystemTrait menuSTrait;
  int status;
  
  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(w)), XmQTmenuSystem);

  if (! menuSTrait) return;

  status = menuSTrait -> status(XtParent(w));

  if ((((ShellWidget) XtParent(XtParent(mw)))->shell.popped_up) &&
      XmIsInDragMode(status)) {
    
    menuSTrait -> childFocus(w);

    mw->menu_button.armed = True;
    
    if (wc->simple_class.draw_shadow)
      (*(wc->simple_class.draw_shadow)) ((Widget) mw);
  }
}



/**********************************************************************
 *
 * MenuButtonLeave
 *
 *********************************************************************/
static void 
MenuButtonLeave (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
                )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  XmMenuSystemTrait menuSTrait;
  int status;

  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(w)), XmQTmenuSystem);

  if (! menuSTrait) return;

  status = menuSTrait -> status(XtParent(w));

  if (XmIsInDragMode(status) &&
      mw->menu_button.armed &&
      event->xcrossing.mode == NotifyNormal) {

    mw->menu_button.armed = False;
    
    if (wc->simple_class.draw_shadow)
      (*(wc->simple_class.draw_shadow)) ((Widget) mw);
    }
}



/**********************************************************************
 *
 * ArmAndActivate
 *
 *********************************************************************/
static void 
ArmAndActivate (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
               )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  Boolean already_armed = mw->menu_button.armed;
  XmAnyCallbackStruct cb;
  Boolean parent_is_torn = False;
  Boolean torn_has_focus = False;
  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(w)), XmQTmenuSystem);

  if (menuSTrait == NULL) {
    /* We're in trouble.  This isn't a valid menu that we're 
       in and Arm and Activate has been called.  Abort ! */
    return;
  }

  if (!XmIsMenuShell(XtParent(XtParent(mw)))) {
    parent_is_torn = True;
    /* Because the pane is torn and the parent is a transient shell,
     * the shell's focal point should be valid
     * (as opposed to getting it from a MenuShell).
     */
    if (XmeFocusIsInShell((Widget)mw)) {
      if (!XtIsSensitive((Widget)mw) || (!XtIsManaged((Widget)mw)))
	return;
      torn_has_focus = True;
    }
  }
  else {
    parent_is_torn = False;
  }

  mw->menu_button.armed = False;

  if (parent_is_torn && !torn_has_focus) {
    /* Freeze tear off visuals in case accelerators are not in
     * same context
     */
    menuSTrait -> reparentToTearOffShell(XtParent(w), event);
  }

  if (torn_has_focus)
    menuSTrait -> popdown(XtParent(mw), event);
  else
    menuSTrait -> buttonPopdown(XtParent(mw), event);

  /* if its in a torn off menu pane, show depressed button briefly */
  if (torn_has_focus) {
    /* Set the focus here. */
    XmProcessTraversal ((Widget)mw, XmTRAVERSE_CURRENT);
    /* this should be done through draw-shadow!!!! */
    XmeDrawShadows (XtDisplay(mw), XtWindow(mw),
		    mw->primitive.bottom_shadow_GC, mw->primitive.top_shadow_GC,
		    mw->primitive.highlight_thickness,
		    mw->primitive.highlight_thickness,
		    mw->core.width - 2 * mw->primitive.highlight_thickness,
		    mw->core.height - 2 * mw->primitive.highlight_thickness,
		    mw->primitive.shadow_thickness, XmSHADOW_OUT);
  }

  XFlush (XtDisplay(mw));

  /* If the parent is a Menu, set the lastSelectToplevel before the arm.
   * It's ok if this is recalled later.
   */
  menuSTrait -> getLastSelectToplevel(XtParent(mw));

  cb.reason = XmCR_ACTIVATE;
  cb.event = event;

  /* if the parent is a Menu, notify it about the select */
  menuSTrait -> entryCallback(XtParent(mw), (Widget) mw, &cb);

  if (! mw -> menu_button.skip_callback && 
      mw->menu_button.activate_callback) {
    XFlush (XtDisplay (mw));
    XtCallCallbackList ((Widget)mw, mw->menu_button.activate_callback, &cb);
  }

  mw->menu_button.armed = False;

  if (torn_has_focus) {
    /* Leave the focus widget in an armed state */
    mw->menu_button.armed = True;
  } 
  else {
    menuSTrait -> reparentToTearOffShell(XtParent(mw), event);
  }

  /*
   * If the button is still around, show it released, after a short delay.
   * This is done if the button is outside of a menus, or if in a torn
   * off menupane.
   */
  if (torn_has_focus &&
      mw->core.being_destroyed == False &&
      mw->menu_button.visual_timer == 0) {
    mw->menu_button.visual_timer = XtAppAddTimeOut(
				XtWidgetToApplicationContext((Widget)mw),
				(unsigned long) VISUAL_DELAY, 
                                 VisualDisarm,
				(XtPointer)(mw));
  }
}

/**********************************************************************
 *
 * VisualDisarm:
 *     
 *
 *********************************************************************/
static void 
VisualDisarm (
        XtPointer cd,
        XtIntervalId *id
             )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)cd;
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(mw);

  mw->menu_button.visual_timer = 0;

  if (XtIsRealized((Widget)mw) && XtIsManaged((Widget)mw)) {
    /* When rapidly clicking, the focus may have moved away from this
       widget, so check before changing the shadow. */
    if (XmeFocusIsInShell((Widget)mw) &&
	(XmGetFocusWidget((Widget)mw) == (Widget)mw) ) {
      /* in a torn off menu, redraw shadows */

      /* Draw a Motif-style shadow. */ 
      XmeDrawShadows (XtDisplay(mw), XtWindow(mw),
		      mw->primitive.top_shadow_GC,
		      mw->primitive.bottom_shadow_GC,
		      mw->primitive.highlight_thickness,
		      mw->primitive.highlight_thickness,
		      mw->core.width - 
		      (2 * mw->primitive.highlight_thickness),
		      mw->core.height - 
		      (2 * mw->primitive.highlight_thickness),
		      mw->primitive.shadow_thickness,	
		      XmSHADOW_OUT);
    }
    XFlush (XtDisplay(mw));
  }
}



/**********************************************************************
 *
 * BtnDown
 *
 *********************************************************************/
static void 
BtnDown (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
        )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  XmAnyCallbackStruct cb;

  Boolean validButton = False;
  Boolean already_armed;
  ShellWidget popup;
  int	status;

  XmMenuSystemTrait menuSTrait;

  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(mw)), XmQTmenuSystem);

  if (menuSTrait == NULL) {
    /* We're in trouble.  This isn't a valid menu that we're in.  Abort! */ 
    return;
  }

  XAllowEvents (XtDisplay(mw), SyncPointer, CurrentTime);

  if (event && (event->type == ButtonPress))
    validButton = menuSTrait -> verifyButton(XtParent(mw), event);

  if (!validButton) return;

  menuSTrait -> controlTraversal(XtParent(mw), True);

  status = menuSTrait -> status(XtParent(w));

  /* Popdown other popus that may be up */
  if (!(popup = (ShellWidget) menuSTrait -> getPopupPosted (XtParent(mw)))) {
    if (!XmIsMenuShell (XtParent(XtParent(mw)))) {
      /* In case tear off not armed and no grabs in place, do it now.
       * Ok if already armed and grabbed - nothing done.
       */
      menuSTrait -> tearOffArm(XtParent(mw));
    }
  }

  if (popup) {
    Widget aw;

    if (popup->shell.popped_up)
      menuSTrait -> popdownEveryone((Widget) popup, event);

    /* If active_child is a cascade (highlighted), unhighlight it.  */
    aw = ((XmManagerWidget)XtParent(mw))->manager.active_child;
    if (aw && ((XmIsCascadeButton(aw) || XmIsCascadeButtonGadget(aw))))
      XmCascadeButtonHighlight (aw, False);
  }

  /* Set focus to this pushbutton.  This must follow the possible
   * unhighlighting of the CascadeButton else it'll screw up active_child.
   */
  (void)XmProcessTraversal( (Widget)mw, XmTRAVERSE_CURRENT);

  /* get the location cursor - get consistent with Gadgets */

  already_armed = mw->menu_button.armed;
  mw->menu_button.armed = True;
  _XmRecordEvent(event);
}



/**********************************************************************
 *
 * BtnUp
 *
 *********************************************************************/
static void 
BtnUp (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
      )
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  Widget parent = XtParent(mw);
  XmAnyCallbackStruct cb;
  Boolean flushDone = False;
  Boolean validButton = False;
  Boolean popped_up;
  Widget shell = XtParent(XtParent(mw));
  XmMenuSystemTrait menuSTrait;
  
  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(w)), XmQTmenuSystem);
  
  if (menuSTrait == NULL) {
    /* We're in trouble.  This isn't a valid menu that we're 
       in and Arm and Activate has been called.  Abort ! */
    return;
  }
  
  if (event && (event->type == ButtonRelease))
    validButton =  menuSTrait -> verifyButton(parent, event);
  
  if (!validButton || (mw->menu_button.armed == False))
    return;
  
  mw->menu_button.armed = False;
  
  if (!XmIsMenuShell(shell))
    popped_up = menuSTrait -> popdown(parent, event);
  else
    popped_up = menuSTrait -> buttonPopdown(parent, event);
  
  _XmRecordEvent(event);

  /* XmMENU_POPDOWN left the menu posted on button click - don't activate! */
  if (popped_up) return;
  
  cb.reason = XmCR_ACTIVATE;
  cb.event = event;
  
  menuSTrait -> entryCallback(parent, (Widget) mw, &cb);
  
  if (! mw -> menu_button.skip_callback && 
      mw->menu_button.activate_callback) {
    XFlush (XtDisplay(mw));
    flushDone = True;
    XtCallCallbackList ((Widget)mw, mw->menu_button.activate_callback, &cb);
  }
  
  /* If the original shell does not indicate an active menu, but rather a
   * tear off pane, leave the button in an armed state.  Also, briefly
   * display the button as depressed to give the user some feedback of
   * the selection.
   */
  
  if (!XmIsMenuShell(shell)) {
    XmeDrawShadows (XtDisplay (mw), XtWindow (mw),
		    mw->primitive.bottom_shadow_GC,
		    mw->primitive.top_shadow_GC,
		    mw->primitive.highlight_thickness,
		    mw->primitive.highlight_thickness,
		    mw->core.width - 2 * mw->primitive.highlight_thickness,
		    mw->core.height - 2 * mw->primitive.highlight_thickness,
		    mw->primitive.shadow_thickness, XmSHADOW_OUT);
    
    XFlush (XtDisplay (mw));
    flushDone = True;
    
    if (mw->core.being_destroyed == False &&
	mw->menu_button.visual_timer == 0) {
      mw->menu_button.visual_timer = XtAppAddTimeOut (
						      XtWidgetToApplicationContext((Widget)mw),
						      (unsigned long)VISUAL_DELAY,
						      VisualDisarm,
						      (XtPointer)mw);
    }
    
    mw->menu_button.armed = True;
  }
  else {
    menuSTrait -> reparentToTearOffShell(XtParent(mw), event);
  }
  
  menuSTrait -> controlTraversal(XtParent(mw), False);
  
  /* For the benefit of tear off menus, we must set the focus item
   * to this button.  In normal menus, this would not be a problem
   * because the focus is cleared when the menu is unposted.
   */
  
  if (!XmIsMenuShell(shell))
    XmProcessTraversal ((Widget)mw, XmTRAVERSE_CURRENT);
}



/**********************************************************************
 *
 * MenuButtonHelp
 *
 *********************************************************************/
static void 
MenuButtonHelp (
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params
               )
{
  ExmMenuButtonWidgetClass wc = (ExmMenuButtonWidgetClass)XtClass(w);
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  XmMenuSystemTrait menuSTrait;
  
  menuSTrait = (XmMenuSystemTrait) 
    XmeTraitGet((XtPointer) XtClass(XtParent(w)), XmQTmenuSystem);
  
  if (menuSTrait == NULL) {
    /* We're in trouble.  This isn't a valid menu that we're 
       in and Arm and Activate has been called.  Abort ! */
    return;
  }
  
  menuSTrait -> buttonPopdown(XtParent(mw), event);
  
  XtCallActionProc ((Widget)mw, "PrimitiveHelp", event, params, *num_params);
  
  menuSTrait -> reparentToTearOffShell(XtParent(mw), event);
}


/***********************************************************/
/* Trait method which allows the menu system to control    */
/* the button's activate callback.  This is used in	   */
/* conjunction with the entryCallback in RowColumn.  The   */
/* RowColumn callback (or other menu which supports the	   */
/* menuSavvy trait) is called via the 			   */
/* childsActivateCallback in the menuSystem trait.	   */
/***********************************************************/

static void
DisableCallback(
     Widget w, 
     XtEnum value)
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  mw->menu_button.skip_callback = !( value == XmENABLE_ACTIVATE);
}


static char*
GetAccelerator(Widget w)
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  return(mw -> menu_button.accelerator);
}


static KeySym 
GetMnemonic(Widget w)
{
  ExmMenuButtonWidget mw = (ExmMenuButtonWidget)w;
  
  return(mw -> menu_button.mnemonic);
}

static char* GetActivateCBName() 
{
  return(XmNactivateCallback);
}


/************************************************************************
 *
 *  ExmCreateMenuButton
 *      Externally accessable function for creating a MenuButton widget
 *
 ************************************************************************/
Widget 
ExmCreateMenuButton (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                    )
{
    return (XtCreateWidget(name,exmMenuButtonWidgetClass,
				parent,arglist,argCount));
}

