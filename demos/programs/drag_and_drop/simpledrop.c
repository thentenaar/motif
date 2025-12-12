/* $XConsortium: simpledrop.c /main/6 1996/04/21 18:50:33 drk $ */
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
 *
 */
/*
 * HISTORY
 */

/*
 * A Simple Drag And Drop Demo
 *
 *    Created: Vania JOLOBOFF / January 1994
 * ======================================================================
 *
 */

/*
 * ===================== include ========================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <Xm/XmAll.h>

/* ======================================================================
 *  General Types
 */

typedef enum {closed, open} State ;

/*
 * The demo basic data structure
 */

typedef struct _Airport {
   XtAppContext context;
   Display * display;
   int screen_width;
   int screen_height;
   State state;
   Widget main; /* the main window */
   Widget drop_area; /* the drop_area drawing area */
   Widget help_dialog;
   Widget error_dialog;
   Widget warning_dialog;
   Widget msg_area;
   Position drop_x;
   Position drop_y;
} DropDemoRec, *DropDemo;

/* ======================================================================
 * Global constants and variables
 */

Atom StringAtom;
Atom PixmapAtom;
Atom CTAtom;

/* ============================ prototypes  ======================= */

static void Warning(DropDemo this, String s1, String s2);
/* issue a warning to user */

static Widget MenuBarEntry(Widget menubar, String entry, String labels[],
		XtCallbackProc procs[], XtPointer private[], int count);

static void HelpCallback(Widget button, XtPointer client_data, XtPointer cbs);

static void ExitCallback(Widget	button, XtPointer client_data, XtPointer cbs);

static void SomethingDropped(Widget dc, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data);
/* drop proc callback on airport. Something is landing */

static void TransferDone(Widget transfer, XtPointer client_data,
		   Atom * selection, Atom * type, XtPointer value,
		   unsigned long *length, int *format);
/* Xt Selection Callback. Transfer flight number from source */

/* =====================================================================
 * Show application modal warning dialogue box with specified messages
 */

static void Warning(DropDemo this, String s1, String s2)
{
   XmString msg;
   char tmp[512];

   if (s1 == NULL)
     msg = XmStringCreateLocalized(s2);
   else if (s2 == NULL)
     msg = XmStringCreateLocalized(s1);
   else {
      strncat(strcpy(tmp, s1), s2, sizeof(tmp) - strlen(s1));
      msg = XmStringCreateLocalized(tmp);
   }

   if (this->warning_dialog == NULL) {
      Arg args[8];
      int n = 0;

      XtSetArg(args[n], XmNdialogStyle,
	       XmDIALOG_FULL_APPLICATION_MODAL);
      n++;
      this->warning_dialog = XmCreateWarningDialog(this->main,
						"warning", args, n);
   }
   printf("warning %s\n", tmp);
   XtVaSetValues(this->warning_dialog, XmNmessageString, msg, NULL);
/*   XmStringFree(msg); */
   printf("managing dialog\n");
   XtManageChild(this->warning_dialog);
}

/* ===============================================================
 *   Create a cascade and a pulldown entry into a menubar,
 * 	from a list of push button specs.
 *   If the label is NULL, create a separator instead of a push button.
 */

static Widget MenuBarEntry(Widget menubar, String entry, String labels[],
			   XtCallbackProc procs[], XtPointer private[],
			   int count)
{
   Widget menu;
   Widget kid;
   Arg args[8];
   int n;
   int i;
   char name[128];

   n = 0;
   menu = XmCreatePulldownMenu(menubar,
			       strcat(strcpy(name, entry), "Menu"),
			       args, n);
   for( i = 0; i < count; i++) {
      n = 0;
      if (labels[i] == NULL) {
	 kid = XmCreateSeparator(menu, "sep", args, n);
      }
      else {
	 kid = XmCreatePushButton(menu, labels[i], args, n);
	 if (procs[i] != NULL)
	   XtAddCallback(kid, XmNactivateCallback, procs[i], private[i]);
      }
      XtManageChild(kid);
   }
   n = 0;
   XtSetArg(args[n], XmNlabelString, XmStringCreateLocalized(entry)); n++;
   XtSetArg(args[n], XmNsubMenuId, menu); n++;
   kid = XmCreateCascadeButton(menubar, entry, args, n);
   XtManageChild(kid);
   return kid;
}

/* ==============================================================
 *    Main loop
 */

int main(int argc, char *argv[])
{
#define MAIN_CHILDREN		3
#define FORM_CHILDREN		2
#define SetMenuEntry(k,l,p,d) labels[k]=l;procs[k]=p;private[k]=d;
#define NUM_TARGETS 3
   Atom targets[NUM_TARGETS];
   Widget theWidgetRoot;
   Widget main_kid[MAIN_CHILDREN];
   Widget form_kid[FORM_CHILDREN];
   Widget framed[2];
   Widget entry;
   static char myClass[] = "DropDemo";
   Arg args[10];
   int n ;
   String labels[4];
   XtCallbackProc procs[4];
   XtPointer private[4];
   DropDemo this;


/*
 * Initialize
 */
   this = (DropDemo) XtCalloc(sizeof(DropDemoRec), 1);
   this->state = closed;

   theWidgetRoot = XtVaAppInitialize(&this->context, myClass,
				     NULL, 0, &argc, argv, NULL, NULL,
				     XmNallowShellResize, True,
				     NULL);
   this->display = XtDisplay(theWidgetRoot);
   StringAtom = XmInternAtom(this->display, "STRING", False);
   PixmapAtom = XmInternAtom(this->display, "PIXMAP", False);
   CTAtom = XmInternAtom(this->display, "COMPOUND_TEXT", False);
/*
 * Create the widget structure.
 * Create the Main Window, the menubar and the pulldown menus
 * Then a scale and a drawing area inside a frame inside a form
 */
   n = 0;
   this->main = XmCreateMainWindow(theWidgetRoot, "main", args, n);

/*
 * Create the menu bar with a File and Help entries
 */
   n = 0;
   XtSetArg (args[n], XmNscrolledWindowChildType,  XmMENU_BAR); n++ ;
   main_kid[0] = XmCreateMenuBar(this->main, "menubar", args, n);

   n = 0;
   SetMenuEntry(n, "Exit", (XtCallbackProc)ExitCallback, (XtPointer)this );
   n++;
   entry = MenuBarEntry(main_kid[0], "File", labels, procs, private, n);
   n = 0;
   SetMenuEntry(0, "Tutorial", (XtCallbackProc)HelpCallback, (XtPointer)this);
   n++;
   entry = MenuBarEntry(main_kid[0], "Help", labels, procs, private, n);
   n = 0;
   XtSetArg(args[n], XmNmenuHelpWidget, entry); n++;
   XtSetValues(main_kid[0], args, n);
/*
 * Create the work area as a Form containing a Scale and a Frame
 */
   n = 0;
   this->screen_width = WidthOfScreen(XtScreenOfObject(this->main));
   this->screen_height = HeightOfScreen(XtScreenOfObject(this->main));
   XtSetArg(args[n], XmNwidth, this->screen_width*3/7); n++;
   XtSetArg(args[n], XmNheight, this->screen_height*3/7); n++;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
   XtSetArg (args[n], XmNscrolledWindowChildType, XmWORK_AREA); n++ ;
   main_kid[1] = XmCreateForm(this->main, "work_area", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNvalue, 0); n++;
   XtSetArg(args[n], XmNmaximum, this->screen_width*3/7); n++;
   XtSetArg(args[n], XmNminimum, 0); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNtitleString,
		      XmStringCreateLocalized("Number To Drag")); n++;
   form_kid[0] = XmCreateScale(main_kid[1], "scale", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET) ; n++;
   XtSetArg(args[n], XmNtopWidget, form_kid[0]) ; n++;
   XtSetArg(args[n], XmNbottomAttachment,XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM) ; n++;
   form_kid[1] = XmCreateFrame(main_kid[1], "frame", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
   framed[0] = XmCreateLabel(form_kid[1], "drop_here", args, n);
/*
 * Create the dropping area
 */
   n = 0;
   this->drop_area =
     framed[1] = XmCreateBulletinBoard(form_kid[1], "da", args, n);
   XtVaSetValues(this->drop_area, XmNuserData, this, NULL);
   n = 0;
   targets[0] = StringAtom;
   targets[1] = PixmapAtom;
   targets[2] = CTAtom;
   XtSetArg(args[n], XmNimportTargets, targets); n++;
   XtSetArg(args[n], XmNnumImportTargets, NUM_TARGETS); n++;
   XtSetArg(args[n], XmNdragProc, NULL); n++;
   XtSetArg(args[n], XmNdropProc, SomethingDropped); n++;
   XmDropSiteRegister(this->drop_area, args, n);

   XtManageChildren(framed, 2);
   XtManageChildren(form_kid, FORM_CHILDREN);

   XtManageChildren(main_kid, 2);
   XtManageChild(this->main);

   XtRealizeWidget(theWidgetRoot);
   XtAppMainLoop(this->context);

   return 0;    /* make compiler happy */
}

/* ===============================================================
 *   The Exit Callback.  Exit properly.
 */

static void ExitCallback(Widget	button, XtPointer client_data, XtPointer cbs)
{
   DropDemo this = (DropDemo) client_data;

   XtCloseDisplay(this->display);
   exit(0);
}

/* ======================================================================
 *  Help Callback. Display help message.
 */

static void HelpCallback(Widget button, XtPointer client_data, XtPointer cbs)
{
   DropDemo this = (DropDemo) client_data;
   static char helpmsg[] =
     "Please drag the value of the scale into the drop area";

  if ( this->help_dialog == NULL ) {
     Arg args[8];
     int n = 0;

     XtSetArg(args[n], XmNmessageString,
	      XmStringCreateLocalized(helpmsg)); n++;
     this->help_dialog = XmCreateInformationDialog(button,
					    "helpdialog", args, n);
  }
  XtManageChild(this->help_dialog);
}

/* ===============================================================
 * Drop proc callback of the drop site.
 * If the drop site is invalid, issue a warning
 * If the drop site is valid, we know the target is at least one of
 * 	String, CT or Pixmap.
 * Pick one of them only in this order of preference.
 */

static void SomethingDropped(Widget dropSite, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data)
{
#define NUMREC 2
   DropDemo this;
   Arg args[8];
   Arg get_args[4];
   Atom * exports;
   XmDropTransferEntryRec target_data[NUMREC];
   unsigned char status = '\0';
   Widget dc = call_data->dragContext;
   int n = 0;
   int i = 0;
   Cardinal num_targets = 0;
   Cardinal num_transfer = 0;

   XtVaGetValues(dropSite, XmNuserData, &this, NULL);
   XtSetArg(get_args[i], XmNexportTargets, &exports);  i++;
   XtSetArg(get_args[i], XmNnumExportTargets, &num_targets); i++;
   XtGetValues(dc, get_args, i);
   if (call_data->dropSiteStatus == XmVALID_DROP_SITE) {
      call_data->operation = XmDROP_COPY;
      this->drop_x = call_data->x;
      this->drop_y = call_data->y;
      for (i = 0; i < num_targets; i++ )
	if (exports[i] == StringAtom
	    || exports[i] == CTAtom
	    || exports[i] == PixmapAtom) {
	   status = XmTRANSFER_SUCCESS;
	   num_transfer = 1;
	   target_data[0].client_data = (XtPointer) this;
	   target_data[0].target = exports[i];
	   XtSetArg(args[n], XmNtransferProc, TransferDone);  n++;
	   XtSetArg(args[n], XmNdropTransfers, target_data);  n++;
	   break;
	}
   }
   else     {
      Warning(this, "Non Identified Object is Dropped. ",
	      num_targets > 0 ? XmGetAtomName(this->display,exports[0]) : "");
      status = XmTRANSFER_FAILURE;
   }
   XtSetArg(args[n], XmNnumDropTransfers, num_transfer);  n++;
   XtSetArg(args[n], XmNtransferStatus, status);  n++;
   XmDropTransferStart(call_data->dragContext, args, n);
#undef NUMREC
}

/* ===============================================================
 * The selection callback proc after drop transfer.
 * Bytes have been received. Check they are OK
 * If data received is corrupted, issue a warning.
 */

static void TransferDone(Widget transfer, XtPointer client_data,
			 Atom * selection, Atom * type, XtPointer value,
			 unsigned long *length, int *format)
{
   Arg args[16];
   int n = 0;
   Widget label;
   DropDemo this = (DropDemo) client_data;
   XmString string = NULL;
   char msg[256];


   if (*format == 8)
     {
	if (*type == StringAtom)
	  string = XmStringCreateLocalized((char *)value);
	else if (*type == CTAtom)
	  string = XmCvtCTToXmString( (char *) value );
      XtSetArg(args[n], XmNlabelString, string); n++;
     }
   else if (*type == PixmapAtom && *format == 32) {
      Pixmap pixmap = (Pixmap) value;
      XtSetArg(args[n], XmNbackgroundPixmap, pixmap); n++;
   }
   else {
      XtVaSetValues(transfer,
		    XmNtransferStatus, XmTRANSFER_FAILURE,
		    XmNnumDropTransfers, 0,
		    NULL);
      sprintf(msg, "Type %s format %d",
	      XmGetAtomName(this->display, *type), *format);
      Warning(this, "Dropped data is corrupted. ", msg);
      return;
   }
   XtSetArg(args[n], XmNx, this->drop_x); n++;
   XtSetArg(args[n], XmNy, this->drop_y); n++;
   label = XmCreateLabel(this->drop_area, "label", args, n);
   XtManageChild(label);
   if (string != NULL)
      XmStringFree(string);
}
