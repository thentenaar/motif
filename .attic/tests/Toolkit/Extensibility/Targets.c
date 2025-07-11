/* $XConsortium: Targets.c /main/5 1995/07/15 21:03:56 drk $ */
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
/*
 * HISTORY
 */

/******************************************************************************
 *
 * Targets.c - Targets widget.  This widget is a subclass of ExmString
 * that demonstrates simple data transfer. This test is for the express
 * purpose of performing QATS target conversion test called "XmeTargets".
 * The test includes code that should not be included in generic tests,
 * specifically any of the validation code.
 *
 *****************************************************************************/

#include <Xm/XmP.h>   /* private header file for the XmPrimitive widget */
#include "TargetsP.h" /* private header file for ExmTargets widget */
#include <Xm/TransferT.h> /* header file for Transfer trait */
#include <Xm/TraitP.h>  /* header file for traits */
#include <Xm/SpecRenderT.h> /* header file for XmLABEL_RENDER_TABLE */

/* Declare all static functions. */
static void ClassPartInitialize(
                        WidgetClass widgetclass);
static void Initialize(
                        Widget request_w,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args );
static void Destroy (
                        Widget w );
static void StartHighlight( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void ExtendHighlight( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void MakePrimSelection(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void PrimCopy(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params);
static void RegisterDropSite( 
                        Widget w);
static void ConvertProc(
			Widget w,
			XtPointer client_data,
			XtPointer call_data);
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
static int ConvertCompoundString(
			Widget w,
			XmString cstring,
			Atom target,
			XtPointer *value,
			Atom *type,
			int *format,
			unsigned long *length,
			int *nchars);
static Atom SetSnapshot(
			Widget w,
			XmString cstring);
static Boolean GetSnapshot(
			Widget w,
			Atom distinguisher,
			XmString *cstring);
static Boolean FreeSnapshot(
			Widget w,
			Atom distinguisher);

/* No constants needed. */

char *print_target(), *print_selection();

/* Default translations for ExmTargets */
#define defaultTranslations "\
<Btn1Down>:		StartHighlight()\n\
<Btn1Motion>:		ExtendHighlight()\n\
<Btn1Up>:		MakePrimSelection()\n\
<Btn2Down>:		PrimCopy()\n\
<EnterWindow>:		PrimitiveEnter()\n\
<LeaveWindow>:		PrimitiveLeave()\n\
~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
<Key>osfActivate:	PrimitiveParentActivate()\n\
<Key>osfCancel:		PrimitiveParentCancel()\n\
<Key>osfHelp:		PrimitiveHelp()"

/* Actions for ExmTargets */
static XtActionsRec actionsList[] = {
       {"StartHighlight",       StartHighlight},
       {"ExtendHighlight",	ExtendHighlight},
       {"MakePrimSelection",	MakePrimSelection},
       {"PrimCopy",		PrimCopy},
};

/* Resources for the ExmTargets widget. */
static XtResource resources[] = 
{
  {
    XmNdestinationCallback,
    XmCCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    XtOffsetOf(ExmTargetsRec, targets.destination_callback),
    XmRCallback,
    (XtPointer) NULL
    },
};


/* No synthetic resources. */

externaldef (exmtargetsclassrec)
     ExmTargetsClassRec exmTargetsClassRec = {
  {    
    /* superclass */                 (WidgetClass)&exmStringClassRec,
    /* class_name */                 "ExmTargets",
    /* widget_size */                sizeof(ExmTargetsRec),
    /* class_initialize */           NULL,
    /* class_part_initialize */      ClassPartInitialize,
    /* class_inited */               FALSE,
    /* initialize */                 Initialize,
    /* initialize_hook */            NULL,
    /* realize */                    XtInheritRealize,
    /* actions */                    actionsList,
    /* num_actions */                XtNumber(actionsList),
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
    /* draw_visual */                ExmInheritDrawVisual,
    /* draw_shadow */                ExmInheritDrawShadow,
    /* create_text_gc */             ExmInheritCreateGC,
    /* destroy_text_gc */            ExmInheritDestroyGC,
    /* select_text_gc */             ExmInheritSelectGC,
    /* calc_visual_size */           ExmInheritCalcVisualSize,
    /* calc_widget_size */           ExmInheritCalcWidgetSize,
    /* reconfigure */                ExmInheritReconfigure,
    /* extension */                  NULL,
  },    
  { /* ExmString */  
    /* default_render_table_type */  XmLABEL_RENDER_TABLE,
    /* extension */                  NULL,
  },
  { /* ExmTargets */
    /* extension */		     NULL,
  },
};    

/* Establish the widget class name as an externally accessible symbol.
   Use the "externaldef" macro rather than the "extern" keyword. */
externaldef( exmtargetswidgetclass) WidgetClass
       exmTargetsWidgetClass =
       (WidgetClass) &exmTargetsClassRec;

/* No global representation type variables. */

/* Define trait structure variable. */
/* Any widget class that uses the Uniform Transfer Model must have the
 * Transfer trait.  Here we declare a variable to hold the trait structure. */

static XmTransferTraitRec StringTrT = {
  0,                           /* version */
  (XmConvertCallbackProc) ConvertProc,             /* convertProc */
  (XmDestinationCallbackProc) DestinationProc,     /* destinationProc */
  NULL,                        /* no destinationPreHookProc */
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
 /* Install the XmQTtransfer trait on the ExmTargets class and all
    its subclasses. */
   XmeTraitSet((XtPointer) widgetclass, XmQTtransfer,
	       (XtPointer) &StringTrT);
}

/******************************************************************************
 *
 *  Destroy
 *      Called by the Intrinsics whenever this widget is deallocated.
 *
 *****************************************************************************/
static void 
Destroy (
       Widget w
        )
{
 ExmTargetsWidget stw = (ExmTargetsWidget) w;
 ExmTargetsSnapshotStruct *sp, *next;

  /* Free memory in the widget instance used to hold snapshots. */
  sp = stw -> targets.snapshots;
  while (sp != NULL) {
    XmStringFree(sp -> value);
    next = sp -> next;
    XtFree((char *) sp);
    sp = next;
  }
  stw -> targets.snapshots =
    (ExmTargetsSnapshotStruct *) NULL;
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
 Arg args[2], dropSiteArgs[2];
 int n, nt, num_targets;

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

  /* validate the XmeDropSink call */
  n = 0;
  XtSetArg(dropSiteArgs[n], XmNnumImportTargets, &num_targets); n++;
  XmDropSiteRetrieve(w, dropSiteArgs, n);
  if (num_targets !=  nt) {
    printf("ERROR: in XmeDropSink call\n");
    printf("\tIncorrect number of targets in dropSite database: %d\
for widget: %d\n",
	   num_targets, w);
  }
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
  ExmTargetsWidget stw = (ExmTargetsWidget) new_w;

 /* Initialize internal instance data structures. */
   stw->targets.snap_counter = 0;
   stw->targets.snapshots = (ExmTargetsSnapshotStruct *) NULL;

 /* Register widget as a drop site. */
  RegisterDropSite(new_w);
}

/******************************************************************************
 *
 *  StartHighlight
 *      This function starts highlighting "selected" text for the primary
 * selection.
 *
 *****************************************************************************/
static void
StartHighlight( 
     Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;
  stw->targets.select_start_x = event->xbutton.x;
  stw->targets.select_start_y = event->xbutton.y;
}


/******************************************************************************
 *
 *  ExtendHighlight
 *      This function is called when the mouse is being dragged with the
 * first button depresssed.  During this time, the letters of text contained
 * in the selection are highlighted.
 *
 *****************************************************************************/
static void
ExtendHighlight( 
     Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;
}


/******************************************************************************
 *
 * MakePrimSelection
 *      This function is called when the user completes making a primary
 * selection after the desired text is highlighted.
 *
 *****************************************************************************/
static void
MakePrimSelection(
     Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;

  stw->targets.have_primary = XmePrimarySource(w, event->xbutton.time);
  if (!stw->targets.have_primary)
    printf("ERROR: XmePrimarySource failed\n");
}


/******************************************************************************
 *
 * PrimCopy
 *      This function is called to COPY the data from the current primary
 * selection.
 *
 *****************************************************************************/
static void
PrimCopy(
     Widget w,
     XEvent *event,
     String *params,
     Cardinal *num_params)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;
  XPoint  stuff_point;
  Time event_time = event ? event->xbutton.time :
    XtLastTimestampProcessed(XtDisplay(w));

  if (event) {
    stuff_point.x = event->xbutton.x;
    stuff_point.y = event->xbutton.y;
  }
  else
    stuff_point.x = stuff_point.y = 0;

  if (XmePrimarySink(w, XmCOPY, (XtPointer) &stuff_point, event_time)
      == False)
    printf("ERROR: XmePrimarySink failed during a COPY operation\n");
}

/*******************************************************************************
 *
 *  Trait Methods: 
 *      Provide the code for two methods defined by the
 *      Transfer trait.
 *
 ******************************************************************************/


/*******************************************************************************
 *
 *  ConvertProc
 *      Transfer trait convertProc.
 *      The toolkit calls this procedure after calling the XmNconvertCallback
 *      procedures whenever someone requests a conversion from the widget.
 *      The convertProc is passed an XmConvertCallbackStruct.  The convertProc
 *      fills in the status member and, if it does the conversion, the value,
 *      type, length, and format members, which represent the converted data.
 *
 ******************************************************************************/
static void
ConvertProc(
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;
  XmConvertCallbackStruct *cs = (XmConvertCallbackStruct *) call_data;
  Atom CLIPBOARD = XInternAtom(XtDisplay(w), "CLIPBOARD", False);
  Atom TEXT = XInternAtom(XtDisplay(w), "TEXT", False);
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
  Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
  Atom FOREGROUND = XInternAtom(XtDisplay(w), "FOREGROUND", False);
  Atom BACKGROUND = XInternAtom(XtDisplay(w), "BACKGROUND", False);
  Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
  Atom SNAPSHOT = XInternAtom(XtDisplay(w), "SNAPSHOT", False);
  Atom DONE = XInternAtom(XtDisplay(w), "DONE", False);
  Atom DELETE = XInternAtom(XtDisplay(w), "DELETE", False);
  Atom MOTIF_DROP = XInternAtom(XtDisplay(w), "_MOTIF_DROP", False);
  Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
  Atom MOTIF_EXPORT_TARGETS =
    XInternAtom(XtDisplay(w), "_MOTIF_EXPORT_TARGETS", False);
  Atom CLIPBOARD_IMMEDIATE =
    XInternAtom(XtDisplay(w), "_MOTIF_CLIPBOARD_TARGETS", False);
  Atom CLIPBOARD_DEFERRED =
    XInternAtom(XtDisplay(w), "_MOTIF_DEFERRED_CLIPBOARD_TARGETS", False);
  Atom CLASS = XInternAtom(XtDisplay(w), "CLASS", False);
  Atom CLIENT_WINDOW = XInternAtom(XtDisplay(w), "CLIENT_WINDOW", False);
  Atom COLORMAP = XInternAtom(XtDisplay(w), "COLORMAP", False);
  Atom NAME = XInternAtom(XtDisplay(w), "NAME", False);
  Atom TIMESTAMP = XInternAtom(XtDisplay(w), "TIMESTAMP", False);
  Atom MOTIF_RENDER_TABLE =
    XInternAtom(XtDisplay(w), "_MOTIF_RENDER_TABLE", False);
  Atom FILE = XInternAtom(XtDisplay(w), "FILE", False);
  int n, nchars;
  int cstatus;
  XmString cstring = (XmString) NULL;
  Boolean converted = False;
  Boolean is_snapshot = False;
  Atom type = None;
  XtPointer value = (XtPointer) NULL;
  unsigned long length = 0;
  int format = 8;

  /* We don't support links. */
  if (cs -> selection == CLIPBOARD && cs -> parm == (XtPointer) XmLINK) {
    cs -> status = XmCONVERT_REFUSE;
    return;
  }

  /* Decide the proper compound string to convert.  If the selection
   * is one of the standard ones, use the value of ExmNcompoundString.
   * If the selection is the distinguisher for a snapshot, get the
   * saved compound string from the snapshot. */
  if (cs -> selection == XA_PRIMARY || cs -> selection == XA_SECONDARY ||
      cs -> selection == CLIPBOARD || cs -> selection == MOTIF_DROP) {
    cstring = stw -> string.compound_string;
  } else {
    if (cs -> target == DONE) {
      /* A target of DONE for a snapshot selection means we can free
       * the snapshot.  If it's some other selection, we don't
       * understand it. */
      is_snapshot = FreeSnapshot(w, cs -> selection);
      if (!is_snapshot) {
	cs -> status = XmCONVERT_REFUSE;
	return;
      }
      converted = True;
    } else {
      is_snapshot = GetSnapshot(w, cs -> selection, &cstring);
      if (!is_snapshot) {
	/* Unknown selection; use ExmNcompoundString. */
	cstring = stw -> string.compound_string;
      }
    }
  }

  if (cs -> target == TARGETS) {
    /* We convert the standard targets, plus those to which we can
     * convert a compound string. */
    Atom *targs = XmeStandardTargets(w, 5, &n);
    int i, num_found=1;

    /* validate call to XmeStandardTargets */
    for (i=0; i<n; i++){
      if (targs[i] == BACKGROUND) num_found++;
      else if (targs[i] == CLASS) num_found++;
      else if (targs[i] == CLIENT_WINDOW) num_found++;
      else if (targs[i] == COLORMAP) num_found++;
      else if (targs[i] == FOREGROUND) num_found++;
      else if (targs[i] == NAME) num_found++;
      else if (targs[i] == TARGETS) num_found++;
      else if (targs[i] == TIMESTAMP) num_found++;
      else if (targs[i] == MOTIF_RENDER_TABLE) num_found++;
    }
    if (num_found != n)
      printf("ERROR: XmeStandardTargets - not all targets found\n");

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
    /* Put _MOTIF_COMPOUND_STRING onto the clipboard.
     * If we can convert the compound string completely to STRING,
     * also put STRING on the clipboard; otherwise, put COMPOUND_TEXT. */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 2);

    if (targs == NULL) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    n = 0;
    targs[n] = MOTIF_C_S; n++;
    cstatus = ConvertCompoundString(w, cstring, XA_STRING, &value, &type,
				    &format, &length, &nchars);
    if (cstatus == XmCONVERT_DONE && nchars == 0) {
      targs[n] = XA_STRING; n++;
    } else {
      targs[n] = COMPOUND_TEXT; n++;
    }

    if (cstatus == XmCONVERT_DONE) XtFree((char *) value);
    value = (XtPointer) targs;
    type = XA_ATOM;
    length = n;
    format = 32;
    converted = True;
  }

  if (cs -> target == CLIPBOARD_DEFERRED) {
    /* If the compound string is fully convertible to locale text,
     * include the locale atom in deferred clipboard targets.
     * If we didn't include COMPOUND_TEXT as an immediate target,
     * include it as a deferred target. */
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 2);

    if (targs == NULL) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    n = 0;
    if (LOCALE_ATOM != XA_STRING) {
      cstatus = ConvertCompoundString(w, cstring, LOCALE_ATOM, &value, &type,
				      &format, &length, &nchars);
      if (cstatus == XmCONVERT_DONE && nchars == 0) {
	targs[n] = LOCALE_ATOM; n++;
      }
      if (cstatus == XmCONVERT_DONE) XtFree((char *) value);
    }

    cstatus = ConvertCompoundString(w, cstring, XA_STRING, &value, &type,
				    &format, &length, &nchars);
    if (cstatus == XmCONVERT_DONE && nchars == 0) {
      targs[n] = COMPOUND_TEXT; n++;
    }

    if (cstatus == XmCONVERT_DONE) XtFree((char *) value);
    value = (XtPointer) targs;
    type = XA_ATOM;
    length = n;
    format = 32;
    converted = True;
  }

  if (cs -> target == SNAPSHOT) {
    /* Take a snapshot of the compound string, and return a unique
     * distinguisher atom for the snapshot. The requestor will use
     * the distinguisher as the selection when it wants to convert
     * the snapshot value later. */
    Atom distinguisher, *dp;
    distinguisher = SetSnapshot(w, cstring);
    if (distinguisher == None) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    dp = (Atom *) XtMalloc(sizeof(Atom));
    if (dp == NULL) {
      cs -> status = XmCONVERT_REFUSE;
      return;
    }
    *dp = distinguisher;
    value = (XtPointer) dp;
    type = XA_ATOM;
    length = 1;
    format = 32;
    converted = True;
  }

  if (cs -> target == DELETE) {
    Arg args[1];
    Cardinal n;
    XmString null_str;
    null_str = XmStringCreateLocalized("");
    n = 0;
    XtSetArg(args[n], ExmNcompoundString, null_str);  n++;
    XtSetValues(w, args, n);
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
    int i, num_targets;
    Boolean found = False;
    Atom *atom_ptr;
    if (cs -> status == XmCONVERT_MERGE) {
      /* A status of XmCONVERT_MERGE means that our value should be
       * merged with any value that may have come from the XmNconvertCallback
       * routines.  This should apply only to listlike targets, such as
       * TARGETS.  XmeConvertMerge does the merging. */
      num_targets = length;
      XmeConvertMerge(value, type, format, length, cs);

      /* validate the ConvertMerge call */
      if (cs->length != num_targets + 1)
        printf("ERROR: XmeConvertMerge - incorrect number of targets: %d\n",
	       cs->length);

      atom_ptr = (Atom *)cs->value;
      for (i=0; i<cs->length; i++){
	if (*atom_ptr == FILE) found = True;
	atom_ptr++;
      }
      if (!found)
	printf("ERROR: XmeConvertMerge - not all targets found\n");
      
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
    static Boolean check;

    if (cs->target == FOREGROUND) check = True;
    XmeStandardConvert(w, NULL, cs);

    /* validate StandardConvert call - expecting a FOREGROUND target */
    if (check) {
      if (cs->target != FOREGROUND)
	printf("ERROR: StandardConvert wrong target: %s\n",
	       print_target(cs->target,w));
      /* reset check */
      check = False;
    }

    if (cs -> value == NULL) {
      cs -> status = XmCONVERT_REFUSE;
    } else {
      cs -> status = XmCONVERT_DONE;
    }
  }
}


/*******************************************************************************
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
 ******************************************************************************/
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


/*******************************************************************************
 *
 *  SetSnapshot
 *      Takes a snapshot of the compound string, generates a distinguisher
 *      atom for the snapshot, and saves the snapshot in the widget instance.
 *      Returns the distinguisher atom or, if unsuccessful, None.
 *
 ******************************************************************************/
static Atom
SetSnapshot(
    Widget w,
    XmString cstring)
{
  ExmTargetsWidget stw = (ExmTargetsWidget) w;
  ExmTargetsSnapshotStruct *sp, *prev;
  int nchars;
  char dname[32];

  sp = prev = stw -> targets.snapshots;
  /* Find the end of the existing list of snapshots. */
  for (; sp != NULL; prev = sp, sp = sp -> next);
  sp = (ExmTargetsSnapshotStruct *)
    XtMalloc(sizeof(ExmTargetsSnapshotStruct));
  if (sp != NULL) {
    /* Generate a unique distinguisher atom.  We use the widget ID and
     * a monotonically increasing counter to make the atom unique. */
    nchars = sprintf(dname, "_EXM_SNAP_%lX_%X", (long) w,
		     stw -> targets.snap_counter++);
    if (nchars > 0) {
      /* Add this snapshot to the linked list. */
      sp -> distinguisher = XInternAtom(XtDisplay(w), dname, False);
      sp -> value = XmStringCopy(cstring);
      sp -> prev = prev;
      sp -> next = (ExmTargetsSnapshotStruct *) NULL;
      if (prev == NULL) {
	stw -> targets.snapshots = sp;
      } else {
	prev -> next = sp;
      }
      return sp -> distinguisher;
    } else {
      XtFree((char *) sp);
    }
  }
  return None;
}


/*******************************************************************************
 *
 *  GetSnapshot
 *      Looks up the given distinguisher atom in the list of snapshots.
 *      If a snapshot exists for the distinguisher, returns True and returns
 *      the cstring argument with the associated compound string.
 *      If no shapshot exists for the distinguisher, returns False.
 *
 ******************************************************************************/
static Boolean
GetSnapshot(
    Widget w,
    Atom distinguisher,
    XmString *cstring)
{
 ExmTargetsWidget stw = (ExmTargetsWidget) w;
 ExmTargetsSnapshotStruct *sp;
 int i;

  if (cstring == NULL) return False;
  sp = stw -> targets.snapshots;
  for (; sp != NULL && sp -> distinguisher != distinguisher; sp = sp -> next);
  if (sp != NULL) {
    *cstring = sp -> value;
    return True;
  } else {
    return False;
  }
}

/*******************************************************************************
 *
 *  FreeSnapshot
 *      Looks up the given distinguisher atom in the list of snapshots.
 *      If a snapshot exists for the distinguisher, frees the snapshot
 *      and returns True.
 *      If no shapshot exists for the distinguisher, returns False.
 *
 ******************************************************************************/
static Boolean
FreeSnapshot(
    Widget w,
    Atom distinguisher)
{
 ExmTargetsWidget stw = (ExmTargetsWidget) w;
 ExmTargetsSnapshotStruct *sp;

  sp = stw -> targets.snapshots;
  for (; sp != NULL && sp -> distinguisher != distinguisher; sp = sp -> next);
  if (sp != NULL) {
    /* Free the compound string, unlink the snapshot, and free the snapshot. */
    XmStringFree(sp -> value);
    if (sp -> prev != NULL) {
      sp -> prev -> next = sp -> next;
    }
    if (sp -> next != NULL) {
      sp -> next -> prev = sp -> prev;
    }
    if (sp -> prev == NULL && sp -> next == NULL) {
      stw -> targets.snapshots =
	(ExmTargetsSnapshotStruct *) NULL;
    }
    XtFree((char *) sp);
    return True;
  } else {
    return False;
  }
}

/*******************************************************************************
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
 ******************************************************************************/
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
    XtFree((char *) targets);
    if (target == None) {
      XmTransferDone(ds -> transfer_id, XmTRANSFER_DONE_FAIL);
      return;
    }
    /* TransferProc is called by XmTransferValue when the data is
     * ready.  TransferProc does the conversion to compound string. */
      XmTransferValue(ds -> transfer_id, target,
		    (XtCallbackProc) TransferProc, NULL, NULL);
  } else {
    /* For selections other than _MOTIF_DROP, ask for TARGETS.
     * TransferProc will handle the returned targets. */
      XmTransferValue(ds -> transfer_id, TARGETS,
		    (XtCallbackProc) TransferProc, NULL, NULL);
  }
}



/*******************************************************************************
 *
 *  TransferProc
 *      Called by XmTransferValue when the transferred data is available.
 *      The procedure is passed an XmSelectionCallbackStruct.
 *      It is responsible for converting the data to a compound string
 *      and inserting it into the widget.
 *
 ******************************************************************************/
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
		   (XtCallbackProc) TransferProc, NULL, NULL);
  }

  if (ss -> type == MOTIF_C_S) {
    /* For _MOTIF_COMPOUND_STRING, the data should be in ASN.1 format.
     * Convert to a compound string. */
 /* cstring = XmCvtByteStreamToXmString((unsigned char *) ss -> value, ss -> length); */
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


/*******************************************************************************
 *
 *  PreferredTarget
 *      Given a list of targets, returns the target we prefer to transfer
 *      as a destination.
 *      If we cannot handle any target in the list, returns None.
 *
 ******************************************************************************/
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



/*******************************************************************************
 *
 *  ExmCreateTargets
 *      Externally accessible function for creating a Targets widget.
 *
 ******************************************************************************/
Widget
ExmCreateTargets (
        Widget parent,
        char *name,
        Arg *arglist,
        Cardinal argCount
                )
{
  return (XtCreateWidget (name, exmTargetsWidgetClass,
			  parent, arglist, argCount));
}

/* utility functions for callback structures */

char *
print_target (Atom target, Widget w)
{
  Atom BACKGROUND = XInternAtom(XtDisplay(w), "BACKGROUND", False);
  Atom CLASS = XInternAtom(XtDisplay(w), "CLASS", False);
  Atom CLIENT_WINDOW = XInternAtom(XtDisplay(w), "CLIENT_WINDOW", False);
  Atom CLIPBOARD = XInternAtom(XtDisplay(w), "CLIPBOARD", False);
  Atom CLIPBOARD_IMMEDIATE =
    XInternAtom(XtDisplay(w), "_MOTIF_CLIPBOARD_TARGETS", False);
  Atom CLIPBOARD_DEFERRED =
    XInternAtom(XtDisplay(w), "_MOTIF_DEFERRED_CLIPBOARD_TARGETS", False);
  Atom COLORMAP = XInternAtom(XtDisplay(w), "COLORMAP", False);
  Atom COMPOUND_TEXT = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
  Atom DELETE = XInternAtom(XtDisplay(w), "DELETE", False);
  Atom DONE = XInternAtom(XtDisplay(w), "DONE", False);
  Atom FILE = XInternAtom(XtDisplay(w), "FILE", False);
  Atom FOREGROUND = XInternAtom(XtDisplay(w), "FOREGROUND", False);
  Atom LOCALE_ATOM = XmeGetEncodingAtom(w);
  Atom MOTIF_DROP = XInternAtom(XtDisplay(w), "_MOTIF_DROP", False);
  Atom MOTIF_C_S = XInternAtom(XtDisplay(w), "_MOTIF_COMPOUND_STRING", False);
  Atom MOTIF_EXPORT_TARGETS =
    XInternAtom(XtDisplay(w), "_MOTIF_EXPORT_TARGETS", False);
  Atom MOTIF_RENDER_TABLE =
    XInternAtom(XtDisplay(w), "_MOTIF_RENDER_TABLE", False);
  Atom NAME = XInternAtom(XtDisplay(w), "NAME", False);
  Atom SNAPSHOT = XInternAtom(XtDisplay(w), "SNAPSHOT", False);
  Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
  Atom TEXT = XInternAtom(XtDisplay(w), "TEXT", False);
  Atom TIMESTAMP = XInternAtom(XtDisplay(w), "TIMESTAMP", False);
  Atom XA_MOTIF_DROP = XInternAtom(XtDisplay(w), "_MOTIF_DROP", False);

  if (target == BACKGROUND) return("BACKGROUND");
  else if (target == CLASS) return("CLASS");
  else if (target == CLIENT_WINDOW) return("CLIENT_WINDOW");
  else if (target == CLIPBOARD) return("CLIPBOARD");
  else if (target == CLIPBOARD_IMMEDIATE) return("CLIPBOARD_IMMEDIATE");
  else if (target == CLIPBOARD_DEFERRED) return("CLIPBOARD_DEFERRED");
  else if (target == COMPOUND_TEXT) return("COMPOUND_TEXT");
  else if (target == DELETE) return("DELETE");
  else if (target == DONE) return("DONE");
  else if (target == FILE) return("FILE");
  else if (target == FOREGROUND) return("FOREGROUND");
  else if (target == LOCALE_ATOM) return("LOCALE_ATOM");
  else if (target == MOTIF_C_S) return("MOTIF_C_S");
  else if (target == MOTIF_DROP) return("MOTIF_DROP");
  else if (target == MOTIF_RENDER_TABLE) return("MOTIF_RENDER_TABLE");
  else if (target == MOTIF_EXPORT_TARGETS) return("MOTIF_EXPORT_TARGETS");
  else if (target == NAME) return("NAME");
  else if (target == NULL) return("NULL target");
  else if (target == SNAPSHOT) return("SNAPSHOT");
  else if (target == TARGETS) return("TARGETS");
  else if (target == TEXT) return("TEXT");
  else if (target == TIMESTAMP) return("TIMESTAMP");
  else if (target == XA_MOTIF_DROP) return("XA_MOTIF_DROP");
  else if (target == XA_STRING) return("XA_STRING");
  else {
    	printf("target value: %d",target);
    	return("unknown target");
      }
}

char *
print_selection (Widget w, XmConvertCallbackStruct *cs)
{
  Atom CLIPBOARD = XInternAtom(XtDisplay(w), "CLIPBOARD", False);
  Atom MOTIF_DROP = XInternAtom(XtDisplay(w), "_MOTIF_DROP", False);

  if (cs -> selection == XA_PRIMARY) return("XA_PRIMARY");
  else if (cs -> selection == XA_SECONDARY) return("XA_SECONDARY");
  else if (cs -> selection == CLIPBOARD) return("CLIPBOARD");
  else if (cs -> selection == MOTIF_DROP) return("MOTIF_DROP");
  else return("unknown selection type");
}
