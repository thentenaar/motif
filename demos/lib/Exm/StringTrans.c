/* $XConsortium: StringTrans.c /main/6 1996/11/20 15:37:29 drk $ */
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
 * ExmStringTransfer is a demonstration widget.  OSF provides this widget
 * solely to teach programmers how to write their own Motif widgets.
 * OSF does not support this widget in any way.
 *
 ************************************ WARNING *********************************/


/*******************************************************************************
 *
 * StringTrans.c - ExmStringTransfer widget.  This widget is a subclass
 *                 of ExmString that demonstrates UTM data transfer.
 *                 See the "OSF/Motif Widget Writer's Guide" for details.
 *
 ******************************************************************************/

#include <Xm/XmP.h>           /* private header file for XmPrimitive */
#include <Exm/StringTransP.h> /* private header file for ExmStringTransfer */
#include <Xm/Screen.h>        /* for screen information */
#include <Xm/TraitP.h>        /* for traits */
#include <Xm/TransferT.h>     /* for XmQTtransfer trait */
#include <Xm/SpecRenderT.h>   /* for XmLABEL_RENDER_TABLE */
#include <Xm/DrawP.h>         /* for XmeClearBorder */

/* Declare all static functions. */
static void ClassPartInitialize(
                        WidgetClass widgetclass);
static void Initialize(
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void DrawVisual (
                        Widget w);
static void CreateGC ( 
                        Widget w );
static void RegisterDropSite( 
                        Widget w);
static void ExmStringTransferMoveFocus(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmStringTransferProcessDrag( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmStringTransferCopyPrimary( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmStringTransferCopyClipboard( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmStringTransferPastePrimary( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExmStringTransferPasteClipboard( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ConvertProc(
			Widget w,
			XtPointer client_data,
			XtPointer call_data);
static int ConvertCompoundString(
			Widget w,
			XmString cstring,
			Atom target,
			XtPointer *value,
			Atom *type,
			int *format,
			unsigned long *length,
			int *nchars);
static void DestinationProc(
			Widget w,
			XtPointer client_data,
			XtPointer call_data);
static void TransferProc(
			Widget w,
			XtPointer client_data,
			XtPointer call_data);
static Atom PreferredTarget(
			Widget w,
			Atom *targets,
			Cardinal num_targets);

/* No constants needed. */

/* Default translations for ExmStringTransfer */
static char defaultTranslations[] = 
"<EnterWindow>:		PrimitiveEnter()\n\
<LeaveWindow>:		PrimitiveLeave()\n\
c <Btn1Down>:		ExmStringTransferMoveFocus()\n\
<Btn1Down>:		ExmStringTransferMoveFocus()ExmStringTransferCopyPrimary()\n\
<Btn2Down>:		ExmStringTransferProcessDrag()\n\
:<Key>osfPaste:		ExmStringTransferPasteClipboard()\n\
:<Key>osfCut:		ExmStringTransferCopyClipboard()\n\
:s <Key>osfInsert:	ExmStringTransferPasteClipboard()\n\
:s <Key>osfDelete:	ExmStringTransferCopyClipboard()\n\
:<Key>osfActivate:	PrimitiveParentActivate()\n\
:<Key>osfCancel:	PrimitiveParentCancel()\n\
:<Key>osfHelp:		PrimitiveHelp()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()";

/* Actions for ExmStringTransfer */
static XtActionsRec actions[] = {
  {"ExmStringTransferMoveFocus",	ExmStringTransferMoveFocus},
  {"ExmStringTransferProcessDrag", 	ExmStringTransferProcessDrag},
  {"ExmStringTransferCopyPrimary",	ExmStringTransferCopyPrimary},
  {"ExmStringTransferPastePrimary",	ExmStringTransferPastePrimary},
  {"ExmStringTransferCopyClipboard",	ExmStringTransferCopyClipboard},
  {"ExmStringTransferPasteClipboard",	ExmStringTransferPasteClipboard}
};

/* Resources for the ExmStringTransfer widget. */
static XtResource resources[] = 
{
  /* XmPrimitive */
  {
    XmNtraversalOn, 
    XmCTraversalOn, 
    XmRBoolean, 
    sizeof (Boolean), 
    XtOffsetOf(XmPrimitiveRec, primitive.traversal_on),
    XmRImmediate, 
    (XtPointer) True /* Restore the default */
  },
  /* ExmStringTransfer */
  {
    XmNdestinationCallback,
    XmCCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(ExmStringTransferRec, string_transfer.destination_callback),
    XmRCallback,
    (XtPointer) NULL
  },
};


/* No synthetic resources. */

externaldef (exmstringtransferclassrec)
     ExmStringTransferClassRec exmStringTransferClassRec = {
  {    
    /* superclass */                 (WidgetClass)&exmStringClassRec,
    /* class_name */                 "ExmStringTransfer",
    /* widget_size */                sizeof(ExmStringTransferRec),
    /* class_initialize */           NULL,
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
    /* destroy */                    NULL,
    /* resize */                     XtInheritResize,
    /* expose */                     XtInheritExpose,
    /* set_values */                 NULL,
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
    /* arm_and_activate */           NULL,
    /* syn_resources */              NULL, 
    /* num_syn_resources */          0, 
    /* extension */                  NULL,
  },    
  { /* ExmSimple */                  
    /* draw_visual */                DrawVisual,
    /* draw_shadow */                ExmInheritDrawShadow,
    /* create_gc */                  CreateGC,
    /* destroy_gc */                 ExmInheritDestroyGC,
    /* select_gc */                  ExmInheritSelectGC,
    /* calc_visual_size */           ExmInheritCalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */                ExmInheritReconfigure,
    /* extension */                  NULL,
  },    
  { /* ExmString */  
    /* default_render_table_type */  XmLABEL_RENDER_TABLE,
    /* extension */                  NULL,
  },
  { /* ExmStringTransfer */
    /* extension */		     NULL,
  },
};    

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro for OS-independent global definitions. */
externaldef( exmstringtransferwidgetclass) WidgetClass
       exmStringTransferWidgetClass =
       (WidgetClass) &exmStringTransferClassRec;

/* No global representation type variables. */

/* Define trait structure variables. */
/* Any widget class that uses the Uniform Transfer Model must have the
 * Transfer trait.  Here we declare a variable to hold the trait structure. */
static const XmTransferTraitRec StringTrT = {
  0,                           /* version */
  (XmConvertCallbackProc) ConvertProc,         /* convertProc */
  (XmDestinationCallbackProc) DestinationProc, /* destinationProc */
  NULL,                                        /* no destinationPreHookProc */
};


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
  /* Install the XmQTtransfer trait on the ExmStringTransfer class and all
     its subclasses. */
  XmeTraitSet((XtPointer) widgetclass, XmQTtransfer,
	      (XtPointer) &StringTrT);
  
}


/******************************************************************************
 *
 *  Initialize
 *      Called when this widget is instantiated.
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
  ExmStringTransferWidget stw = (ExmStringTransferWidget) new_w;
  
  /* Initialize internal instance data structures. */
  stw->string_transfer.own_primary = False;
  
  /* Register widget as a drop site. */
  RegisterDropSite(new_w);
}

/******************************************************************************
 *
 *  DrawVisual:
 *     Called by ExmSimple whenever it needs to draw or redraw the
 *     widget visual.  Used here to do the highlighting for own_primary.
 *
 *****************************************************************************/
static void 
DrawVisual(Widget w)
{
  ExmStringTransferWidgetClass wc = (ExmStringTransferWidgetClass)XtClass(w);
  ExmStringTransferWidget sw = (ExmStringTransferWidget)w;
  GC use_gc = wc->simple_class.select_gc(w);
  
  /* If the compound string is not NULL and if there is enough space in the
     widget to draw at least a little portion of the compound string, then 
     render the string with XmStringDraw. */
  if (sw->string.compound_string &&
      (sw->simple.visual.width != 0) &&
      (sw->simple.visual.height != 0)) {
    XtGCMask mask = GCForeground | GCBackground;
    XGCValues values;
    if (sw->string_transfer.own_primary) {
      values.foreground = sw->core.background_pixel;
      values.background = sw->primitive.foreground;
      XChangeGC(XtDisplay(sw), use_gc, mask, &values);
      XmStringDrawImage (XtDisplay(sw), XtWindow(sw),
			 sw->string.render_table, 
			 sw->string.compound_string,
			 use_gc,
			 sw->simple.visual.x, sw->simple.visual.y,
			 sw->simple.visual.width, sw->string.alignment,
			 sw->primitive.layout_direction, NULL);
    } else {
      values.foreground = sw->primitive.foreground;
      values.background = sw->core.background_pixel;
      XChangeGC(XtDisplay(sw), use_gc, mask, &values);
      XmStringDrawImage (XtDisplay(sw), XtWindow(sw),
			 sw->string.render_table, 
			 sw->string.compound_string,
			 use_gc,
			 sw->simple.visual.x, sw->simple.visual.y,
			 sw->simple.visual.width, sw->string.alignment,
			 sw->primitive.layout_direction, NULL); 
    }
    XmeClearBorder(XtDisplay(sw), XtWindow(sw),
		   (int)0, (int)0, 
		   (Dimension)sw->core.width, (Dimension)sw->core.height, 
		   (Dimension)(sw->primitive.highlight_thickness +
			       sw->primitive.shadow_thickness)
                   );
  }
}

/*******************************************************************************
 *
 *  CreateGC:
 *      Called by the Initialize method of the base class (ExmSimple).
 *
 ******************************************************************************/
static void 
CreateGC (
        Widget w
         )
{
 ExmStringWidget sw = (ExmStringWidget)w;
 XGCValues    values;
 XtGCMask     valueMask, dynamicMask;
 XFontStruct *fs = (XFontStruct *) NULL;
 Arg          args[2];
 Pixmap       insensitiveStippleBitmap;

 /* This function creates two GC's: one to render a sensitive widget
    and the other to render an insensitive widget. */

 /* This widget will change background and foreground of the text with the 
    selection state. Therefore, foreground and background should be dynamic 
    values */
   dynamicMask = GCForeground | GCBackground;

 /* First, create the sensitive GC (named normal_gc). */ 
   valueMask = GCForeground | GCBackground | GCGraphicsExposures;
   values.foreground = sw->primitive.foreground;
   values.background = sw->core.background_pixel;
   values.graphics_exposures = False;

 /* In order to set the GCFont field of the GC, we must gather XFontStruct 
    information out of the render table. This is only to get a reasonable
    initial value. XmStringDraw will pick the necessary fonts from the
    render table, so we will not need to update the GC when the render 
    table changes. */ 
   if (XmeRenderTableGetDefaultFont(sw->string.render_table, &fs)) {
     values.font = fs->fid;
     valueMask |= GCFont;
   }

   sw->simple.normal_gc = XtAllocateGC ((Widget)sw, 0, /* use widget's depth */
					valueMask, &values, dynamicMask, 0);

 /* Next, create the insensitive GC.  This GC will share the same
    foreground, background, font, and graphics exposures as the sensitive
    GC, but will hold a different fill style and stipple pattern. */ 
   valueMask |= GCFillStyle | GCStipple;
   values.fill_style = FillStippled;

/* Gather the Motif-appropriate insensitive stipple bitmap. */
   XtSetArg(args[0], XmNinsensitiveStippleBitmap, &insensitiveStippleBitmap);
   XtGetValues(XmGetXmScreen(XtScreen(w)), args, 1);
   values.stipple = insensitiveStippleBitmap;

   sw->simple.insensitive_gc = XtAllocateGC ((Widget)sw, 0, /* use widget's depth */
					     valueMask, &values, dynamicMask, 0);
}



/******************************************************************************
 *
 *  RegisterDropSite
 *      Called by Initialize.  Registers this widget as a drop site.
 *
 *****************************************************************************/
static void
RegisterDropSite(
     Widget w)
{
  Atom TEXT = XInternAtom(XtDisplay(w), "TEXT", False);
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
  Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
  Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
  Atom targets[5];
  Arg args[2];
  int n, nt;
  
  /* Set up import targets.  These are the targets from which we can
     generate a compound string when a drop is made. */
  nt = 0;
  targets[nt++] = MOTIF_C_S;
  targets[nt++] = COMPOUND_TEXT;
  targets[nt++] = LOCALE_ATOM;
  if (LOCALE_ATOM != XA_STRING) {
    targets[nt++] = XA_STRING;
  }
  if (LOCALE_ATOM != TEXT) {
    targets[nt++] = TEXT;
  }
  /* If you add any more targets, bump the array size. */
  
  n = 0;
  XtSetArg(args[n], XmNimportTargets, targets); n++;
  XtSetArg(args[n], XmNnumImportTargets, nt); n++;
  XmeDropSink(w, args, n);
}


/******************************************************************************
 *
 *  ExmStringTransferMoveFocus
 *      Move the keyboard focus to this widget
 *
 *****************************************************************************/
static void
ExmStringTransferMoveFocus(
     Widget w, 
     XEvent *event, 
     String *params, 
     Cardinal *num_params)
{
  XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}


/******************************************************************************
 *
 *  ExmStringTransferProcessDrag
 *      Starts a drag operation.
 *
 *****************************************************************************/
static void
ExmStringTransferProcessDrag( 
     Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
 ExmStringTransferWidget stw = (ExmStringTransferWidget) w;
 Arg args[4];
 Cardinal n;
  
  if (! stw -> string_transfer.own_primary) {
    ExmStringTransferPastePrimary(w, event, params, num_params);
    return;
  }
  
  /* Initialize DragContext resources.  We want the drag icon to
     indicate text, and we support only the COPY operation. */
  n = 0;
  /* Normal drag and drop behavior will be to use the foreground
     and background of the widget when creating the cursor or 
     or the pixmap drag icon */
  XtSetArg(args[n], XmNcursorBackground, stw->core.background_pixel);  n++;
  XtSetArg(args[n], XmNcursorForeground, stw->primitive.foreground);  n++;
  /* We use the default text drag icon,  the same as used in
     the standard Motif widgets.  */
  XtSetArg(args[n], XmNsourceCursorIcon, XmeGetTextualDragIcon(w));  n++; 
  XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++;
  (void) XmeDragSource(w, NULL, event, args, n);
}


/******************************************************************************
 *
 *  ExmStringTransferCopyPrimary
 *      Take ownership of the PRIMARY selection
 *
 *****************************************************************************/
static void
ExmStringTransferCopyPrimary(
     Widget w, 
     XEvent *event, 
     String *params, 
     Cardinal *num_params)
{
 Time time;
 ExmStringTransfer stw = (ExmStringTransfer) w;

  /* First we must obtain a timestamp.  This is required for
     ICCCM compliant ownership of a selection.  We use
     XtLastTimestampProcessed which holds the timestamp
     from the last event the Intrinsics saw with a timestamp. */
  time = XtLastTimestampProcessed(XtDisplay(w));

  /* Own the primary selection.  Indicate this to the user by 
     reversing the foreground and background in the text rendition
     (eventually) */
  stw -> string_transfer.own_primary = True;

  /* Once we call XmePrimarySource,  the widget's transfer trait
     convert method will get called if a destination wishes to 
     obtain the PRIMARY selection */
  XmePrimarySource(w, time);

  /* Update the widget to show the new state */
  stw -> core.widget_class -> core_class.expose(w, NULL, NULL);
}


/******************************************************************************
 *
 *  ExmStringTransferCopyClipboard
 *      Put the current widget's data on the clipboard
 *
 *****************************************************************************/
static void
ExmStringTransferCopyClipboard(
     Widget w, 
     XEvent *event, 
     String *params, 
     Cardinal *num_params)
{
  Time time;

  /* First we must obtain a timestamp.  This is required for
     ICCCM compliant ownership of a selection.  We use
     XtLastTimestampProcessed which holds the timestamp
     from the last event the Intrinsics saw with a timestamp. */
  time = XtLastTimestampProcessed(XtDisplay(w));

  /* When we call XmeClipboardSource,  either the Motif clipboard
     will request the current selection data or an external 
     clipboard manager will obtain the data.  When the data is
     obtained,  the CLIPBOARD selection will be owned by the
     data holder */
  XmeClipboardSource(w, XmCOPY, time);
}


/******************************************************************************
 *
 *  ExmStringTransferPastePrimary
 *      Paste the contents of the PRIMARY selection
 *
 *****************************************************************************/
static void
ExmStringTransferPastePrimary(
     Widget w, 
     XEvent *event, 
     String *params, 
     Cardinal *num_params)
{
  Time time;
  unsigned int op;

  /* First we must obtain a timestamp.  This is required for
     ICCCM compliant ownership of a selection.  We use
     XtLastTimestampProcessed which holds the timestamp
     from the last event the Intrinsics saw with a timestamp. */
  time = XtLastTimestampProcessed(XtDisplay(w));

  /* We determine the right operation to perform by looking
     at the modifiers present */
  if (event -> xbutton.state & ShiftMask) {
    if (event -> xbutton.state & ControlMask)
      op = XmLINK;
    else
      op = XmMOVE;
  } else
    op = XmCOPY;

  /* Calling XmePrimarySink will start the process of requesting
     the PRIMARY selection to be pasted using the transfer trait
     destination callback for this widget class */
  XmePrimarySink(w, op, NULL, time);
}


/******************************************************************************
 *
 *  ExmStringTransferPasteClipboard
 *      Paste the contents of the CLIPBOARD selection
 *
 *****************************************************************************/
static void
ExmStringTransferPasteClipboard(
     Widget w, 
     XEvent *event, 
     String *params, 
     Cardinal *num_params)
{
  /* Calling XmeClipboardSink will start the process of requesting
     the CLIPBOARD selection to be pasted using the transfer trait
     destination callback for this widget class */
  XmeClipboardSink(w, XmCOPY, NULL);
}


/******************************************************************************
 *
 *  Trait Methods: 
 *      Provide two of the trait methods defined by the XmQTtransfer trait.
 *
 *****************************************************************************/

/******************************************************************************
 *
 *  ConvertProc
 *      Transfer trait convertProc.
 *      The toolkit calls this procedure after calling the XmNconvertCallback
 *      procedures whenever someone requests a conversion from the widget.
 *      The convertProc is passed an XmConvertCallbackStruct.  The convertProc
 *      fills in the status member and, if it does the conversion, the value,
 *      type, length, and format members, which represent the converted data.
 *
 *****************************************************************************/
static void
ConvertProc(
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
 ExmStringTransferWidget stw = (ExmStringTransferWidget) w;
 XmConvertCallbackStruct *cs = (XmConvertCallbackStruct *) call_data;
 int n, nchars;
 int cstatus;
 XmString cstring = stw -> string.compound_string;
 Boolean converted = False;
 Atom type = None;
 XtPointer value = (XtPointer) NULL;
 unsigned long length = 0;
 int format = 8;
 /* Setup the needed Atoms.  This must be done each time as it is
    never known which display a particular widget will belong to.
    Atom values are constant only on individual displays. */
 Atom CLIPBOARD = XInternAtom(XtDisplay(w), XmSCLIPBOARD, False);
 Atom TEXT = XInternAtom(XtDisplay(w), XmSTEXT, False);
 Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False);
 Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
 Atom FOREGROUND = XInternAtom(XtDisplay(w), "FOREGROUND", False);
 Atom BACKGROUND = XInternAtom(XtDisplay(w), "BACKGROUND", False);
 Atom TARGETS = XInternAtom(XtDisplay(w), XmSTARGETS, False);
 Atom MOTIF_DROP = XInternAtom(XtDisplay(w), XmS_MOTIF_DROP, False);
 Atom MOTIF_C_S = XInternAtom(XtDisplay(w), XmS_MOTIF_COMPOUND_STRING, False);
 Atom MOTIF_EXPORT_TARGETS =
   XInternAtom(XtDisplay(w), XmS_MOTIF_EXPORT_TARGETS, False);
 Atom CLIPBOARD_IMMEDIATE =
   XInternAtom(XtDisplay(w), XmS_MOTIF_CLIPBOARD_TARGETS, False);
 Atom LOSE_SELECTION =
    XInternAtom(XtDisplay(w), XmS_MOTIF_LOSE_SELECTION, False);

  /* We don't support links,  so refuse clipboard convert 
     requests which specify XmLINK in the parm field */
  if (cs -> selection == CLIPBOARD && cs -> parm == (XtPointer) XmLINK) {
    cs -> status = XmCONVERT_REFUSE;
    return;
  }

  /* The primary selection has been lost.  Cleanup the widget
     visuals */
  if (cs -> selection == XA_PRIMARY && cs -> target == LOSE_SELECTION) {
    stw -> string_transfer.own_primary = False;
    stw -> core.widget_class -> core_class.expose(w, NULL, NULL);
    return;
  }

  if (cs -> target == TARGETS) {
    /* We convert the standard targets, plus those to which we can
     * convert a compound string. */
    Atom *targs = XmeStandardTargets(w, 5, &n);

    targs[n] = MOTIF_C_S; n++;
    targs[n] = COMPOUND_TEXT; n++;
    targs[n] = LOCALE_ATOM; n++;
    if (LOCALE_ATOM != XA_STRING) {
      targs[n] = XA_STRING; n++;
    }
    if (LOCALE_ATOM != TEXT) {
      targs[n] = TEXT; n++;
    }

    value = (XtPointer) targs;
    type = XA_ATOM;
    length = n;
    format = 32;
    converted = True;
  }

  if (cs -> target == MOTIF_EXPORT_TARGETS) {
    /* Allow dragging of targets to which we can convert a compound
     * string, plus FOREGROUND and BACKGROUND.  These two will be
     * converted by XmeStandardConvert. */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 7);

    if (targs == NULL) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    n = 0;
    targs[n] = MOTIF_C_S; n++;
    targs[n] = COMPOUND_TEXT; n++;
    targs[n] = LOCALE_ATOM; n++;
    if (LOCALE_ATOM != XA_STRING) {
      targs[n] = XA_STRING; n++;
    }
    if (LOCALE_ATOM != TEXT) {
      targs[n] = TEXT; n++;
    }
    targs[n] = FOREGROUND; n++;
    targs[n] = BACKGROUND; n++;

    value = (XtPointer) targs;
    type = XA_ATOM;
    length = n;
    format = 32;
    converted = True;
  }

  if (cs -> target == CLIPBOARD_IMMEDIATE) {
    /* Put all targets which have no loss of data on the clipboard
       This will always include _MOTIF_COMPOUND_STRING and COMPOUND_TEXT.
       We also put the locale encoding and STRING (the locale encoding
       of ISO8859-1) on the clipboard providing that they are completely
       convertable (no loss of data) */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 4);

    if (targs == NULL) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    n = 0;
    targs[n] = MOTIF_C_S; n++;

    /* Check to make sure we can convert to STRING */
    cstatus = ConvertCompoundString(w, cstring, XA_STRING, &value, &type,
				    &format, &length, &nchars);
    if (cstatus == XmCONVERT_DONE && nchars == 0) {
      targs[n] = XA_STRING; n++;
    } 

    /* If the locale is not STRING,  then make sure we can convert
       the locale and, if so, put it on the clipboard */
    if (LOCALE_ATOM != XA_STRING) {
      cstatus = ConvertCompoundString(w, cstring, LOCALE_ATOM, &value, &type,
				      &format, &length, &nchars);
      if (cstatus == XmCONVERT_DONE && nchars == 0) {
	targs[n] = LOCALE_ATOM; n++;
      }
      if (cstatus == XmCONVERT_DONE) XtFree((char *) value);
    }

    targs[n] = COMPOUND_TEXT; n++;

    if (cstatus == XmCONVERT_DONE) XtFree((char *) value);
    value = (XtPointer) targs;
    type = XA_ATOM;
    length = n;
    format = 32;
    converted = True;
  }

  if (cs -> target == MOTIF_C_S ||
      cs -> target == COMPOUND_TEXT || cs -> target == TEXT ||
      cs -> target == LOCALE_ATOM || cs -> target == XA_STRING) {
    /* Convert the compound string to the appropriate target. */
    cstatus = ConvertCompoundString(w, cstring, cs -> target, &value, &type,
				    &format, &length, &nchars);
    if (cstatus == XmCONVERT_REFUSE) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    if (cstatus == XmCONVERT_DONE) {
      converted = True;
    }
  }   

  if (converted) {  
    /* Converted */
    if (cs -> status == XmCONVERT_MERGE) {
      /* A status of XmCONVERT_MERGE means that our value should be
       * merged with any value that may have come from the XmNconvertCallback
       * routines.  This should apply only to listlike targets, such as
       * TARGETS.  XmeConvertMerge does the merging. */
      XmeConvertMerge(value, type, format, length, cs);
      XtFree((char *) value);
    } else {
      /* Fill in the callback struct members. */
      cs -> type = type;
      cs -> value = value;
      cs -> length = length;
      cs -> format = format;
    }
    cs -> status = XmCONVERT_DONE;
  } else {
    /* We didn't understand the target.  Try XmeStandardConvert. */
    XmeStandardConvert(w, NULL, cs);
    if (cs -> value == NULL) cs -> status = XmCONVERT_REFUSE;
    else cs -> status = XmCONVERT_DONE;
  }

}


/******************************************************************************
 *
 *  ConvertCompoundString
 *      Called by the transfer trait convertProc to convert a compound
 *      string to an appropriate target.
 *      This function returns a conversion status to indicate whether or
 *      not it was able to convert the compound string:
 *          XmCONVERT_DONE    -- successful conversion
 *          XmCONVERT_REFUSE  -- error in converting
 *          XmCONVERT_DEFAULT -- unrecognized target
 *      If successful, it returns the value, type, format, and length
 *      arguments, suitable for insertion into an XmConvertCallbackStruct.
 *      It also returns the nchars argument indicating how many unconverted
 *      characters it produced in the event of a partial conversion.
 *      If the returned value is other than XmCONVERT_DONE, the returned
 *      arguments are undefined.
 *
 *****************************************************************************/
static int
ConvertCompoundString(
    Widget w,
    XmString cstring,
    Atom target,
    XtPointer *value,
    Atom *type,
    int *format,
    unsigned long *length,
    int *nchars)
{
 Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
 Atom TEXT = XInternAtom(XtDisplay(w), "TEXT", False);
 Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
 Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
 Boolean converted = False;
 char ** strings;
 int count, nchars1;

  if (value == NULL || type == NULL || format == NULL ||
      length == NULL || nchars == NULL) return XmCONVERT_REFUSE;

  if (target == MOTIF_C_S) {
    /* For _MOTIF_COMPOUND_STRING, convert to ASN.1 format, and we're done. */
    *type = MOTIF_C_S;
    *format = 8;
    *length = XmCvtXmStringToByteStream(cstring, (unsigned char **) value);
    *nchars = 0;
    converted = True;
  }

  if (target == COMPOUND_TEXT || target == TEXT ||
      target == LOCALE_ATOM || target == XA_STRING) {
    char *ct;
    XTextProperty text_prop;
    *type = target;
    *format = 8;
    *length = 0;
    *value = (XtPointer) NULL;
    *nchars = 0;

    if (cstring == NULL) {
      /* If the compound string is NULL, return a value of NULL and a
       * length of 0, and assert that the conversion was successful. */
      if (target == TEXT) *type = LOCALE_ATOM;
      converted = True;

    } else {

      /* For any of these four targets, the first thing we must do is
       * convert the compound string to compound text. */
      ct = XmCvtXmStringToCT(cstring);
      if (ct == NULL) {
	/* error in conversion to CT */
	return XmCONVERT_REFUSE;
      }

      if (target == COMPOUND_TEXT) {
	/* For COMPOUND_TEXT, return the compound text.
	 * Note: There is no way, using documented functions, to obtain
	 * the length of the converted compound text, other than to
	 * assume that it contains no embedded NULL bytes and call strlen().
	 * It is theoretically possible for compound text to include
	 * embedded NULLs, but we'll ignore that possibility. */
	*value = (XtPointer) ct;
	*length = strlen(ct);	/* Don't include NULL byte */
	converted = True;

      } else {

	/* For a target of TEXT, STRING, or the locale atom, the next
	 * step is to convert the compound text into locale text.
	 * We use XmbTextPropertyToTextList to do that. */
	text_prop.encoding = COMPOUND_TEXT;
	text_prop.format = 8;
	text_prop.value = (unsigned char *) ct;
	text_prop.nitems = strlen(ct); /* Don't include NULL byte */
	*nchars = nchars1 = XmbTextPropertyToTextList(XtDisplay(w), &text_prop,
						      &strings, &count);
	if (nchars1 <= 0) nchars1 = 0;
	if (*nchars == XNoMemory || *nchars == XLocaleNotSupported ||
	    *nchars == XConverterNotFound) {
	  /* error in conversion to locale text */
	  XtFree(ct);
	  return XmCONVERT_REFUSE;
	}

	if (target == TEXT) {
	  /* For TEXT, if we completely converted the compound text to
	   * locale text, return the locale text and the locale atom.
	   * XmbTextListToTextProperty takes care of converting the
	   * locale text to a set of NULL-separated elements, and it
	   * computes the length of the value.
	   * If we only partially converted the compound text to locale
	   * text, we return the compound text and COMPOUND_TEXT. */
	  if (*nchars == Success) {
	    *type = LOCALE_ATOM;
	    *nchars = XmbTextListToTextProperty(XtDisplay(w), strings, count,
						XTextStyle, &text_prop);
	    XtFree(ct);
	    XFreeStringList(strings);
	    if (*nchars == XNoMemory || *nchars == XLocaleNotSupported ||
		*nchars == XConverterNotFound) {
	      /* error in conversion to locale text */
	      return XmCONVERT_REFUSE;
	    }

	    *value = (XtPointer) XtMalloc(text_prop.nitems + 1);
	    if (*value == NULL) {
	      XFree(text_prop.value);
	      return XmCONVERT_REFUSE;
	    }
	    (void) memcpy(*value, text_prop.value, text_prop.nitems + 1);
	    XFree(text_prop.value);
	    *length = text_prop.nitems;
	  } else {
	    XFreeStringList(strings);
	    *type = COMPOUND_TEXT;
	    *value = (XtPointer) ct;
	    *length = strlen(ct); /* Don't include NULL byte */
	  }
	  converted = True;
	}

	if (target == LOCALE_ATOM && !converted) {
	  /* For the locale atom, return the locale text and the
	   * locale atom, even if the compound text was only
	   * partially converted to locale text. */
	  *nchars = XmbTextListToTextProperty(XtDisplay(w), strings, count,
					      XTextStyle, &text_prop);
	  XtFree(ct);
	  XFreeStringList(strings);
	  if (*nchars == XNoMemory || *nchars == XLocaleNotSupported ||
	      *nchars == XConverterNotFound) {
	    /* error in conversion to locale text */
	    return XmCONVERT_REFUSE;
	  }

	  *value = (XtPointer) XtMalloc(text_prop.nitems + 1);
	  if (*value == NULL) {
	    XFree(text_prop.value);
	    return XmCONVERT_REFUSE;
	  }
	  (void) memcpy(*value, text_prop.value, text_prop.nitems + 1);
	  XFree(text_prop.value);
	  *length = text_prop.nitems;
	  *nchars = nchars1 + (*nchars <= 0 ? 0 : *nchars);
	  converted = True;
	}

	if (target == XA_STRING && !converted) {
	  /* For STRING, use XmbTextListToTextProperty to convert the
	   * locale text to STRING, and return the result.
	   * Note that the locale atom might be STRING, so we might
	   * already have done the conversion above. */
	  *nchars = XmbTextListToTextProperty(XtDisplay(w), strings, count,
					      XStringStyle, &text_prop);
	  XtFree(ct);
	  XFreeStringList(strings);
	  if (*nchars == XNoMemory || *nchars == XLocaleNotSupported ||
	      *nchars == XConverterNotFound) {
	    /* error in conversion to string */
	    return XmCONVERT_REFUSE;
	  }

	  *value = (XtPointer) XtMalloc(text_prop.nitems + 1);
	  if (*value == NULL) {
	    XFree(text_prop.value);
	    return XmCONVERT_REFUSE;
	  }
	  (void) memcpy(*value, text_prop.value, text_prop.nitems + 1);
	  XFree(text_prop.value);
	  *length = text_prop.nitems;
	  *nchars = nchars1 + (*nchars <= 0 ? 0 : *nchars);
	  converted = True;
	}
      }
    }
  }

  /* For a successful conversion, return XmCONVERT_DONE.
   * If we didn't recognize the target, return XmCONVERT_DEFAULT. */
  if (converted) return XmCONVERT_DONE;
  else return XmCONVERT_DEFAULT;
}


/******************************************************************************
 *
 *  DestinationProc
 *      Transfer trait destinationProc.
 *      The toolkit calls this procedure after calling the
 *      XmNdestinationCallback procedures whenever the widget is the
 *      destination for data transfer.
 *      The destinationProc is passed an XmDestinationCallbackStruct.
 *      The destinationProc typically calls XmTransferValue to retrieve
 *      the data or XmTransferDone to finish the transfer.
 *
 *****************************************************************************/
static void
DestinationProc(
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
 XmDestinationCallbackStruct *ds = (XmDestinationCallbackStruct *) call_data;
 Atom XA_MOTIF_DROP = XInternAtom(XtDisplay(w), "_MOTIF_DROP", False);
 Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
 Cardinal num_targets, n;
 Arg args[2];
 Atom target, *targets;

  if (ds -> operation == XmLINK) {
    /* We don't support links. */
    XmTransferDone(ds -> transfer_id, XmTRANSFER_DONE_FAIL);
    return;
  }

  if (ds -> selection == XA_MOTIF_DROP) {
    XmDropProcCallbackStruct *cb = 
      (XmDropProcCallbackStruct *) ds -> destination_data;
      
    /* No support for drop help. */
    if (cb -> dropAction == XmDROP_HELP ||
	cb -> operation == XmDROP_NOOP) {
      XmTransferDone(ds -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }

    /* Get the list of export targets from the DragContext.  If
     * this list contains a target we can convert to a compound
     * string, transfer the value. */
    n = 0;
    XtSetArg(args[n], XmNexportTargets, &targets); n++;
    XtSetArg(args[n], XmNnumExportTargets, &num_targets); n++;
    XtGetValues(cb -> dragContext, args, n);
    target = PreferredTarget(w, targets, num_targets);
    if (target == None) {
      XmTransferDone(ds -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }
    /* TransferProc is called by XmTransferValue when the data is
     * ready.  TransferProc does the conversion to compound string. */
      XmTransferValue(ds -> transfer_id, target,
		      (XtCallbackProc) TransferProc, NULL, 0);
  } else {
    /* For selections other than _MOTIF_DROP, ask for TARGETS.
     * TransferProc will handle the returned targets. */
      XmTransferValue(ds -> transfer_id, TARGETS,
		      (XtCallbackProc) TransferProc, NULL, 0);
  }
}

/******************************************************************************
 *
 *  TransferProc
 *      Called by XmTransferValue when the transferred data is available.
 *      The procedure is passed an XmSelectionCallbackStruct.
 *      It is responsible for converting the data to a compound string
 *      and inserting it into the widget.
 *
 *****************************************************************************/
static void
TransferProc(
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
 XmSelectionCallbackStruct *ss = (XmSelectionCallbackStruct *) call_data;
 Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
 Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
 Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
 Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
 XmString cstring = (XmString) NULL;
 Boolean transferred = False;

  if (ss -> target == TARGETS && ss -> type == XA_ATOM) {
    /* For TARGETS, if the returned target list contains one we can
     * convert to a compound string, request conversion to that target. */
    Atom target = PreferredTarget(w, (Atom *) ss -> value, ss -> length);
    if (target == None) {
      XtFree((char *) ss -> value);
      ss -> value = (XtPointer) NULL;
      XmTransferDone(ss -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }
    XmTransferValue(ss -> transfer_id, target,
		    (XtCallbackProc) TransferProc, NULL, 0);
  }

  if (ss -> type == MOTIF_C_S) {
    /* For _MOTIF_COMPOUND_STRING, the data must be in ASN.1 format.
       Convert to a compound string.
       cstring = XmCvtByteStreamToXmString((unsigned char *) ss -> value, 
       |                                                     ss -> length); */
    cstring = XmCvtByteStreamToXmString((unsigned char *) ss -> value);
    transferred = True;
  }

  if (ss -> type == COMPOUND_TEXT) {
    /* Convert compound text to a compound string.
     * Note that XmCvtCTToXmString does not convert a list of compound text
     * strings, so we will get only the first if there's more than one.
     * XmCvtCTToXmString expects a NULL-terminated compound text string,
     * so add a trailing NULL. */
    char *string;
    string = XtMalloc(ss -> length + 1);
    if (string == NULL) {
      XtFree((char *) ss -> value);
      ss -> value = (XtPointer) NULL;
      return;
    }
    (void) memcpy(string, ss -> value, ss -> length);
    string[ss -> length] = '\0';
    cstring = XmCvtCTToXmString(string);
    XtFree(string);
    transferred = True;
  }

  if (ss -> type == XA_STRING || ss -> type == LOCALE_ATOM) {
    /* Convert either a XA_STRING (ISO8859-1 encoding) or the specific
       locale encoding.  This will be the current locale of the 
       application */
    XmString cstring1, sep, oldstring;
    char **strings;
    int nchars, count, i;
    XTextProperty text_prop;
    text_prop.value = (unsigned char *) ss -> value;
    text_prop.encoding = ss -> type;
    text_prop.format = ss -> format;
    text_prop.nitems = ss -> length;

    if (ss -> type == XA_STRING) {
      /* For STRING, create a compound string with an explicit tag
       * and a type of XmCHARSET_TEXT. */
      nchars = (int) XTextPropertyToStringList(&text_prop, &strings, &count);
      if (nchars == 0) {
	/* error in conversion to list of strings */
	XtFree((char *) ss -> value);
	ss -> value = (XtPointer) NULL;
	return;
      }
      sep = XmStringSeparatorCreate();
      for (i = 0; i < count; i++) {
	oldstring = cstring;
	cstring1 = XmStringGenerate(strings[i],
				    (XmStringTag) XmSTRING_ISO8859_1,
				    XmCHARSET_TEXT, NULL);
	cstring = XmStringConcat(oldstring, cstring1);
	XmStringFree(oldstring);
	XmStringFree(cstring1);
	if (i < count - 1) {
	  oldstring = cstring;
	  cstring = XmStringConcat(oldstring, sep);
	  XmStringFree(oldstring);
	}
      }
      XmStringFree(sep);
      XFreeStringList(strings);
      transferred = True;
    }

    if (ss -> type == LOCALE_ATOM && !transferred) {
      /* For locale text (unless the locale atom is STRING), create a
       * compound string with a tag of _MOTIF_DEFAULT_LOCALE and a type
       * of XmMULTIBYTE_TEXT. */
      nchars = XmbTextPropertyToTextList(XtDisplay(w), &text_prop,
					 &strings, &count);
      if (nchars == XNoMemory || nchars == XLocaleNotSupported ||
	  nchars == XConverterNotFound) {
	/* error in conversion to locale text */
	XtFree((char *) ss -> value);
	ss -> value = (XtPointer) NULL;
	return;
      }
      sep = XmStringSeparatorCreate();
      for (i = 0; i < count; i++) {
	oldstring = cstring;
	cstring1 = XmStringGenerate(strings[i], NULL, XmMULTIBYTE_TEXT, NULL);
	cstring = XmStringConcat(oldstring, cstring1);
	XmStringFree(oldstring);
	XmStringFree(cstring1);
	if (i < count - 1) {
	  oldstring = cstring;
	  cstring = XmStringConcat(oldstring, sep);
	  XmStringFree(oldstring);
	}
      }
      XmStringFree(sep);
      XFreeStringList(strings);
      transferred = True;
    }
  }

  if (transferred) {
    /* We have a compound string.  Use it as the new value of
     * ExmNcompoundString. */
    Arg args[1];
    Cardinal n;
    n = 0;
    XtSetArg(args[n], ExmNcompoundString, cstring);  n++;
    XtSetValues(w, args, n);
  }

  XmStringFree(cstring);
  XtFree((char *) ss -> value);
  ss -> value = (XtPointer) NULL;
}


/******************************************************************************
 *
 *  PreferredTarget
 *      Given a list of targets, returns the target we prefer to transfer
 *      as a destination.
 *      If we cannot handle any target in the list, returns None.
 *
 *****************************************************************************/
static Atom
PreferredTarget(
     Widget w,
     Atom *targets,
     Cardinal num_targets)
{
 Atom TEXT = XInternAtom(XtDisplay(w), "TEXT", False);
 Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
 Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
 Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
 int n;
 int cs_index = -1;
 int ct_index = -1;
 int locale_index = -1;
 int string_index = -1;
 int text_index = -1;

  for (n = 0; n < num_targets; n++) {
    if (targets[n] == MOTIF_C_S) cs_index = n;
    if (targets[n] == COMPOUND_TEXT) ct_index = n;
    if (targets[n] == TEXT) text_index = n;
    if (targets[n] == LOCALE_ATOM) locale_index = n;
    if (targets[n] == XA_STRING) string_index = n;
  }

  /* The preferred order of targets:
   * If the locale atom is present, use this order:
   *     _MOTIF_COMPOUND_STRING
   *     TEXT
   *     COMPOUND_TEXT
   *     locale atom
   *     STRING
   * If the locale atom is not present, use this order:
   *     _MOTIF_COMPOUND_STRING
   *     COMPOUND_TEXT
   *     STRING
   */
  if (locale_index >= 0) {
    if (cs_index >= 0) return targets[cs_index];
    if (text_index >= 0) return targets[text_index];
    if (ct_index >= 0) return targets[ct_index];
    if (locale_index >= 0) return targets[locale_index];
    if (string_index >= 0) return targets[string_index];
  } else {
    if (cs_index >= 0) return targets[cs_index];
    if (ct_index >= 0) return targets[ct_index];
    if (string_index >= 0) return targets[string_index];
  }

  return None;
}



/******************************************************************************
 *
 *  ExmCreateStringTransfer
 *      Externally accessible function for creating a StringTransfer widget.
 *
 *****************************************************************************/
Widget
ExmCreateStringTransfer (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                )
{
  return (XtCreateWidget (name, exmStringTransferWidgetClass,
			  parent, arglist, argCount));
}

