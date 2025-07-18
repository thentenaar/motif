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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: main.c /main/8 1997/03/31 13:54:57 dbl $"
#endif
#endif

#include "main.h"

/* ===============================================================
 *   The Done Callback. Set the locale to the selected one
 *	in the radio box
 */

static void DoneCallback(Widget button,
			 XtPointer client_data, XtPointer call_data)

{
   ViewPtr this = (ViewPtr) client_data;
   Widget b;

   XtVaGetValues(locale_box, XmNmenuHistory, &b, NULL);
   XtVaGetValues(b, XmNuserData, &SelectedLocale, NULL);
   LocaleName = LocaleTable[CurrentOS][SelectedLocale];
   printf ("language selected\n");
}

/* ===============================================================
 *    Create a dialog box to verify the OS and choose the locale
 */
static Widget CreateLocaleBox(ViewPtr this, OS os, String os_name)
{
   Widget select_box, pick_form;
   Widget framed[2], frame;
   Widget languages[MaxLanguage+1] ;
   Widget oss[MaxOS+1] ;
   int i, lang;
   Arg args[20];
   int count;
   int n;
   Widget lang_frame, os_frame;
   char s[255];
   int def_lang = 0;
   XmString msg;

   n = 0;
   XtSetArg(args[n], XmNmessageString,
	    XmStringCreateLocalized("Select a language, Verify your OS"));
   n++;
   XtSetArg(args[n], XmNokLabelString, XmStringCreateLocalized("Done"));
   n++;
   select_box = XmCreateTemplateDialog(theWidgetRoot, "search_box", args, n);
   XtAddCallback(select_box,
		 XmNokCallback, (XtCallbackProc) DoneCallback,
		 (XtPointer) this);
   n = 0;
   pick_form = XmCreateForm(select_box, "form", args, n);
   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM) ; n++;
   lang_frame = XmCreateFrame(pick_form, "lang_frame", args, n);
   n = 0;
   XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD); n++;
   XtSetArg(args[n], XmNlabelString, XmStringCreateLocalized("Language")); n++;
   framed[0] = XmCreateLabel(lang_frame, "title", args, n);
   n = 0;
   XtSetArg(args[n], XmNframeChildType, XmFRAME_WORKAREA_CHILD); n++;
   XtSetArg(args[n], XmNisAligned, True); n++;
   XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
   XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
   XtSetArg(args[n], XmNentryClass, xmToggleButtonWidgetClass); n++;
   XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
   framed[1] = XmCreateRadioBox(lang_frame, "lang_box", args, n);
   locale_box = framed[1];
   def_lang = 0;
   for (count= 0, i = 0; i < MaxLanguage ; i++) {
      if (SupportedLocale [os][i]) {
	 if (i == French)
	   def_lang = French;
	 n = 0;
	 XtSetArg(args[n], XmNlabelString,
		  XmStringCreateLocalized(LangList[i])); n++;
	 XtSetArg(args[n], XmNuserData, i); n++;
	 languages[count] =
	   XmCreateToggleButton(framed[1], LangList[i], args, n);
	 ++count;
      }
      else printf("%s does not support %s\n", OSList[os], LangList[i]);
   }
   XtManageChildren(languages, count);
   XtManageChildren(framed, 2);
   XtManageChild(lang_frame);
   XmToggleButtonSetState(languages[def_lang], True, True);

   n = 0;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, lang_frame); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM) ; n++;
   if (os < MaxOS)
     {
	sprintf(s, "Your Operating System is %s", os_name);
	msg = XmStringCreateLocalized(s);
	XtSetArg(args[n], XmNlabelString, msg); n++;
	os_frame = XmCreateLabel(pick_form, "title", args, n);
     }

   else {
      os_frame = XmCreateFrame(pick_form, "os_frame", args, n);
      n = 0;
      XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD); n++;
      XtSetArg(args[n],
	       XmNlabelString, XmStringCreateLocalized("Operating System"));
      n++;
      framed[0] = XmCreateLabel(os_frame, "title", args, n);
      n = 0;
      XtSetArg(args[n], XmNframeChildType, XmFRAME_WORKAREA_CHILD); n++;
      XtSetArg(args[n], XmNisAligned, True); n++;
      XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
      XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
      XtSetArg(args[n], XmNentryClass, xmToggleButtonWidgetClass); n++;
      XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
      framed[1] =
	os_box = XmCreateRadioBox(os_frame, "os_box", args, n);
      n = 0;
      XtSetArg(args[n], XmNlabelString,
	       XmStringCreateLocalized(OSList[i])); n++;
      for (i = 0; i < MaxOS ; i++) {
	 n = 0;
	 XtSetArg(args[n], XmNuserData, i); n++;
	 XtSetArg(args[n], XmNlabelString,
		  XmStringCreateLocalized(OSList[i])); n++;
	 oss[i] = XmCreateToggleButton(framed[1], OSList[i], args, n);
      }
      n = 0;
      XtSetArg(args[n], XmNuserData, MaxOS); n++;
      XtSetArg(args[n], XmNlabelString,
	       XmStringCreateLocalized("Other"));
      n++;
      oss[MaxOS] = XmCreateToggleButton(framed[1],
					OSList[i], args, n);
      XtManageChildren(oss, MaxOS+1);
      XtManageChildren(framed, 2);
      XmToggleButtonSetState(oss[MaxOS-1], True, True);
   }
   XtManageChild(os_frame);
   XtManageChild(pick_form);
   return select_box;
}

/* ===============================================================
 *    Create a dialog box to verify the OS and choose the locale
 */
static void SelectLocale(ViewPtr this, OS os, String os_name)
{
   Widget select_box = NULL;

   if (locale_box == NULL)
     select_box = CreateLocaleBox(this, os, os_name);
   XtManageChild(select_box);

}

/* ===============================================================
 *    Main: Initialize, create the application shell and loop
 * Check that the language specified is
 * supported by fileview and libX and libC.
 */
int main(int argc, char *argv[])
{
#define C_locale_name "C"
   char **p1, **p2;
   int i;
   ViewPtr this;
   XEvent event;
   String new_locale;
   char msg[128];
   String lang = getenv("LANG");
   OS os;
   struct utsname os_def;
   char filename[255];
   Language l;

/*
 * init widget names
 */
   new_pane = "new_pane";
   kill_pane = "kill_pane";
   search = "search";

   /* save argc and argv */
   save_argc = argc;
   save_argv = (char **) XtCalloc(argc, sizeof(char *));
   for ( p1 = argv, p2 = save_argv, i = argc; i-- > 0 ; )
     *p2++ = *p1++ ;
   /* alloc the object for the primary view */
   this = (ViewPtr) XtCalloc(sizeof(View), 1);

   XtToolkitInitialize();
   MrmInitialize();
   theContext = XtCreateApplicationContext();

   uname(&os_def);
   for (os = 0; os < MaxOS ; os++)
     if (strcmp(os_def.sysname, OSList[os]) == 0)
	 break;
   CurrentOS = os;
   for (l = 1;  l < MaxLanguage; l++) {
      if (setlocale(LC_ALL, LocaleTable[os][l]) == NULL
	  ||  ! XSupportsLocale()) {
	 sprintf(msg, "Locale `%s' not supported by C and/or X library",
		 LocaleTable[os][l]);
	 XtAppWarning(theContext, msg);
	 SupportedLocale [CurrentOS][SelectedLocale] = False;
      }
   }
   setlocale(LC_ALL, "C");
   theDisplay = XtOpenDisplay(theContext, NULL, myName, myClass,
			      NULL, 0, &argc, argv);

   theWidgetRoot = XtAppCreateShell(myName, myClass,
				    applicationShellWidgetClass,
				    theDisplay, NULL, 0);

   XtGetApplicationResources(theWidgetRoot, &FileViewResources,
			     FileViewResourcesSpecs,
			     XtNumber(FileViewResourcesSpecs),
			     (ArgList) NULL, 0);

   do {
      SelectLocale(this, os, os_def.sysname);
      do {
	 XtAppNextEvent(theContext, &event);
	 XtDispatchEvent(&event);
      } while (LocaleName == NULL);
      new_locale = setlocale(LC_ALL, LocaleName);
      if (new_locale == NULL ||  ! XSupportsLocale() )
	{
	   SupportedLocale [CurrentOS][SelectedLocale] = False;
	   new_locale = NULL;
	}
      else if (! XSetLocaleModifiers(""))
	XtAppWarning(theContext,
		     "X locale modifiers not supported, using default");

   } while ( new_locale == NULL);

   XtDestroyWidget(theWidgetRoot);
   XtCloseDisplay(theDisplay);
   XtSetLanguageProc(theContext, (XtLanguageProc) MyLanguageProc, theContext);
   theDisplay = XtOpenDisplay(theContext, NULL, myName, myClass,
			      NULL, 0, &argc, argv);

   theWidgetRoot = XtAppCreateShell(myName, myClass,
				    applicationShellWidgetClass,
				    theDisplay, NULL, 0);


   if (strcmp(FileViewResources.version, "NoValue") == 0) {
      Screen * screen = XtScreenOfObject(theWidgetRoot);
      XrmDatabase db = XtScreenDatabase(screen);

      strcpy(filename, LangList[SelectedLocale]);
      strcat(filename, ".df");
      printf("default %s\n", filename);
      XrmCombineFileDatabase(filename, &db, True);
   }
   strcpy(filename, LangList[SelectedLocale]);
   strcat(filename, ".uid");
   uid_files[0] = filename;
   if (MrmOpenHierarchyPerDisplay(theDisplay, 1, uid_files,
				   NULL, &theUIDdatabase)
        != MrmSUCCESS)
       /* || (MrmRegisterNames(regvec, regnum) != MrmSUCCESS)) */
     {
        fprintf (stderr,
                 "Cannot open hierarchy defined by %s\n", uid_files[0]);
     }

   NewFileShell(theWidgetRoot, this, save_argc, save_argv);
   XtPopup(this->shell, XtGrabNone);
   /* XtRealizeWidget(this->text_source); */
   XtAppMainLoop(theContext);

   return 0;    /* make compiler happy */
}

/* ===============================================================
 * The language proc.
 * Note: Xt guarantees that xnl is not NULL. No need to test it.
 * Return a language string usable by XtResolvePathname.
 */
static String MyLanguageProc(Display *dpy, String xnl, XtAppContext ac)
{
   (void)dpy;
   (void)xnl;
   (void)ac;
   return setlocale(LC_CTYPE, NULL);
}

/* ===============================================================
 *   Create a new top level shell.
 * 	If primary is true create a quit entry,
 *	else a close entryn
 */
static ViewPtr NewFileShell(Widget parent, ViewPtr this,
			   int argc, char *argv[])
{
   Widget mw, children[4], menubar, entry;
   Arg args[20];
   String names[8];
   XtCallbackProc procs[8];
   XtPointer private[8];
   int n;
   Dimension width, height;
   WidgetClass class;
   Bool primary = True;

#define SetMenuEntry(k,l,p,d) names[k]=l;procs[k]=p;private[k]=d;

   n = 0;
   XtSetArg(args[n], XmNallowShellResize, True); n++;
   if (this == NULL) {
      this = (ViewPtr) XtCalloc(sizeof(View), 1);
      XtSetArg(args[n], XmNtitle, "File" ); n++;
      class = topLevelShellWidgetClass;
   }
   else {
      XtSetArg(args[n], XmNargv, argv); n++;
      XtSetArg(args[n], XmNargc, argc); n++;
      XtSetArg(args[n], XmNtitle, "File View Main"); n++;
      class = applicationShellWidgetClass;
   }
   this->shell = XtCreatePopupShell("primary", class, parent, args, n);
/*
 * Create the main window, the menubar and the pane
 * Store the view in userData for further use.
 */
   n = 0;
   XtSetArg(args[n], XmNuserData, this); n++;
   mw = XmCreateMainWindow(this->shell, "main", args, n);
   children[0] = XmCreateMenuBar(mw, "menubar", NULL, 0);

   n = 0;
   SetMenuEntry(n, "open", (XtCallbackProc) OpenFileCallback,
		(XtPointer) this); n++;
   SetMenuEntry(n, "opennew", (XtCallbackProc) OpenNewShellCallback,
		(XtPointer) this); n++;
   SetMenuEntry(n, NULL, NULL, NULL); n++;
   if (primary) {
      SetMenuEntry(n, "exit",  (XtCallbackProc) ExitCallback,
		   (XtPointer) parent);
   }
   else {
      SetMenuEntry(n, "close",  (XtCallbackProc) CloseCallback,
		   (XtPointer) this);
   }
   n++;

   (void) CreateMenuBarEntry(children[0], "file", names, procs, private, n);

   n = 0;
   SetMenuEntry(n, new_pane, (XtCallbackProc) NewPaneCallback,
		(XtPointer) this); n++;
   SetMenuEntry(n, kill_pane,(XtCallbackProc) KillPaneCallback,
		(XtPointer) this); n++;
   SetMenuEntry(n, search, (XtCallbackProc) FindCallback,
		(XtPointer) this); n++;
   this->view_cascade =
     CreateMenuBarEntry(children[0], "view", names, procs, private, n);
   SetSensitive(this->view_cascade, new_pane, False);
   SetSensitive(this->view_cascade, kill_pane, False);
   SetSensitive(this->view_cascade, search, False);

   SetMenuEntry(0, "help_view", (XtCallbackProc) HelpCallback,
		(XtPointer) this);
   entry = CreateMenuBarEntry(children[0], "help", names, procs, private, 1);

   n = 0;
   XtSetArg(args[n], XmNmenuHelpWidget, entry); n++;
   XtSetValues(children[0], args, n);
   XtManageChild(children[0]);

   n = 0;
   width = WidthOfScreen(XtScreenOfObject(mw)) / 2 ;
   height = HeightOfScreen(XtScreenOfObject(mw)) * 2 / 3 ;
   XtSetArg(args[n], XmNwidth, width); n++;
   XtSetArg(args[n], XmNheight, height); n++;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
   children[1] = XmCreateForm(mw, "work", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM) ; n++;
   XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM) ; n++;
   this->paned_window = XmCreatePanedWindow(children[1], "panes", args, n);
   XtManageChild(this->paned_window);
   n = 0;
   this->text_source = XmCreateText(children[1],   /* the form */
				    "textsource", args, n);
   n = 0;
   this->path = children[2] = XmCreateLabel(mw, "filename", args, n);

   n = 0;
   XtSetArg(args[n], XmNmenuBar, children[0]); n++;
   XtSetArg(args[n], XmNworkWindow, children[1]); n++;
   XtSetArg(args[n], XmNmessageWindow, children[2]); n++;
   XtSetValues(mw, args, n);
   XtManageChildren(children, 3);
   XtManageChild(mw);
   return this;
}

/* ===============================================================
 *   Create a cascade and a pulldown entry into a menubar,
 * 	from a list of push button specs.
 *   If the label is NULL, create a separator instead of a push button.
 */
static Widget CreateMenuBarEntry(Widget menubar, String entry, String names[],
				 XtCallbackProc procs[],
				 XtPointer private[], int count)
{
   Widget menu;
   Widget child;
   Arg args[8];
   int n;
   int i;
   char menu_name[64];

   n = 0;
   XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
   menu = XmCreatePulldownMenu(menubar,
			       strcat(strcpy(menu_name, entry), "Menu"),
			       args, n);
   for(i = 0; i < count; i++) {
      n = 0;
      if (names[i] == NULL)
	child = XmCreateSeparator(menu, "sep", args, n);
      else {
	 child = XmCreatePushButton(menu, names[i], args, n);
	 if (procs[i] != NULL)
	   XtAddCallback(child, XmNactivateCallback, procs[i], private[i]);
      }
      XtManageChild(child);
   }
   n = 0;
   XtSetArg(args[n], XmNsubMenuId, menu); n++;
   child = XmCreateCascadeButton(menubar, entry, args, n);
   XtManageChild(child);
   return child;
}

/* ===============================================================
 *   Routine to change menu bar items sensitivity.
 *   Takes a cascade button, extract the pulldown, find the item.
 */

void SetSensitive(Widget cascade, String item, Boolean sensitive)
{
   Widget menu, button;

   XtVaGetValues(cascade, XmNsubMenuId, &menu, NULL);
   button = XtNameToWidget(menu, item);
   XtSetSensitive(button, sensitive);
}

/* ===============================================================
 *   The Exit Callback. The root widget is passed as client data.
 *	Close properly. Exit.
 */

static void ExitCallback(Widget button, Widget root,
			 XmPushButtonCallbackStruct *call_data)
{
   Display * dpy = XtDisplay(root);

   XtDestroyWidget(root);
   XtCloseDisplay(dpy);
   exit(0);
}

/* ===============================================================
 *   The Close Callback. The root widget is passed as client data.
 *	Close properly. Exit.
 */

static void CloseCallback(Widget button, ViewPtr this,
			  XmPushButtonCallbackStruct *call_data)
{

   printf("close callback\n");
   XtDestroyWidget(this->shell);
   XtFree((char *) this);
}

/* ===================================================================
 * The Help callback: Show an information message box.
 */
static void HelpCallback(Widget	widget, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data)
{
  static Widget HelpDialog = NULL;

  if ( HelpDialog == NULL ) {
     Arg args[8];
     int n = 0;
     XmString help1, help2, help3, help_msg;

     help1 = XmStringConcatAndFree(FetchString(this, "help_file"),
				   XmStringSeparatorCreate());
     help2 = XmStringConcatAndFree(FetchString(this, "help_view"),
				   XmStringSeparatorCreate());
     help_msg = XmStringConcatAndFree(XmStringConcatAndFree(help1, help2),
				      FetchString(this, "help_search"));

     XtSetArg(args[n], XmNmessageString, help_msg);
     n++;
     HelpDialog = XmCreateInformationDialog(this->shell,
					    "helpdialog", args, n);
     XmStringFree(help_msg);
  }
  XtManageChild(HelpDialog);
}

/* ===============================================================
 *   The Open New Shell Callback.
 * The primary View object is passed in the client_data
 *	Open a new shell and a new file selected by a file selection box.
 */

static void OpenNewShellCallback(Widget widget, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data)
{
   XmPushButtonCallbackStruct dummy_data;
   ViewPtr view;

   view = NewFileShell(theWidgetRoot, NULL, 0, NULL);
   XtPopup(view->shell, XtGrabNone);
}

/* ===============================================================
 *   The Open File Callback. The View object is passed in the client_data
 *	Open a new file selected by a file selection box.
 */

static void OpenFileCallback(Widget widget, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data)
{
   Arg args[8];
   int n = 0;

   if (this->fsb == NULL) {
      this->fsb = CreateFileSelectionBox(this);
      XtManageChild(this->fsb);
   }
   else if (XtIsManaged(this->fsb))
     XtPopup(XtParent(this->fsb), XtGrabNone);
   else XtManageChild(this->fsb);
}

/* ===============================================================
 *   Create File Selection Box
 */
static Widget CreateFileSelectionBox(ViewPtr this)
{
   Arg args[8];
   int n = 0;
   Widget fsb;

   XtSetArg(args[n], XmNallowShellResize, True); n++;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW);  n++;
/*
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
     n++;
 */
   fsb = XmCreateFileSelectionDialog(this->shell, "fsb", args, n);
   XtAddCallback(fsb, XmNokCallback, (XtCallbackProc) FileOKCallback, this);
   XtAddCallback(fsb, XmNcancelCallback, (XtCallbackProc) FileCancelCallback,
		 this);
   return fsb;
}

/* ===================================================================
 * Pop down the fsb.
 */
static void FileCancelCallback(Widget fsb, ViewPtr this,
			XmFileSelectionBoxCallbackStruct *call_data)
{
   XtPopdown(XtParent(this->fsb));
}

/* =====================================================================
 * Fetch an string from Mrm database
 */

XmString FetchString(ViewPtr this, String name)
{
   MrmCode code;
   XmString fetched;

   if (MrmFetchLiteral(theUIDdatabase, name, theDisplay,
		       (XtPointer) &fetched, (MrmCode *) &code) == MrmSUCCESS)
     return fetched;

   ViewError(this, XmStringCreateLocalized("Can't fetch from database: "),
		  XmStringCreateLocalized(name));
   return XmStringCreateLocalized("String Not Found");
}

/* =====================================================================
 * Show application modal error dialogue box
 */

void ViewError(ViewPtr this, XmString s1, XmString s2)
{
   XmString msg;

   msg = XmStringConcatAndFree(s1, s2);

   if (this->error_box == NULL) {
      Arg args[8];
      int n = 0;

      XtSetArg(args[n], XmNdialogStyle,
	       XmDIALOG_FULL_APPLICATION_MODAL);
      n++;
      this->error_box = XmCreateErrorDialog(this->shell, "error", args, n);
   }
   XtVaSetValues(this->error_box, XmNmessageString, msg, NULL);
   XtManageChild(this->error_box);
   XmStringFree(msg);
}

/* =====================================================================
 * Show application modal error dialogue box
 */

void ViewWarning(ViewPtr this, XmString s1, XmString s2)
{
   XmString msg;

   msg = XmStringConcatAndFree(s1, s2);

   if (this->warning_box == NULL) {
      Arg args[8];
      int n = 0;

      XtSetArg(args[n], XmNdialogStyle,
	       XmDIALOG_FULL_APPLICATION_MODAL);
      n++;
      this->warning_box = XmCreateWarningDialog(this->shell,
						"warning", args, n);
   }
   XtVaSetValues(this->warning_box, XmNmessageString, msg, NULL);
   XtManageChild(this->warning_box);
   XmStringFree(msg);
}
