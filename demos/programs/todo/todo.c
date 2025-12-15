/**
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
static char *rcsid = "$XConsortium: todo.c /main/6 1995/07/14 09:46:43 drk $";
#endif
#endif

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
/* X include files */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
/* Motif include files */
#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/FileSB.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/Notebook.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
/* Demo include files */
#include <Xmd/Help.h>
#include <Xmd/Menus.h>
#include <Xmd/Print.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "page.h"

#define APP_CLASS "XmdTodo"

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))

static char * fallback_resources[] = {
"*text.rows: 24",
"*text.columns: 80",
"*print_manager.printerList: lp,./todo.txt",
"*help_manager.helpFile: todo",
"*notebook.frameShadowThickness: 2",
"*notebook.bindingType:	XmSPIRAL",
NULL
};

/* Options */
OptionsRec options;

#define Offset(field) XtOffsetOf(OptionsRec, field)

XtResource resources[] = {
  {"todoFile", "TodoFile", XtRString, sizeof(String),
    Offset(todoFile), XtRImmediate, NULL}
};

#undef Offset

XrmOptionDescRec optionDesc[] = {
  {"-todoFile", "*todoFile", XrmoptionSepArg, (XtPointer) NULL}
};

static void QuitAppl(Widget, char *, XmPushButtonCallbackStruct *);
static void TextChanged(Widget, XtPointer, XtPointer);

void manageCB(Widget, Widget, XtPointer);
void PresentFDialog(Widget, XtPointer, XmPushButtonCallbackStruct*);
void New(Widget, char*, XmPushButtonCallbackStruct *);
void Save(Widget, char*, XmFileSelectionBoxCallbackStruct *);
void Open(Widget, char*, XmFileSelectionBoxCallbackStruct *);
void Print(Widget, char*, XmdPrintCallbackStruct *);
void SaveIt(Widget, char*, XmPushButtonCallbackStruct *);
void PageChange(Widget, XtPointer, XmNotebookCallbackStruct *);
void NewPage(Widget, XtPointer, XmPushButtonCallbackStruct *);
void DeletePage(Widget, XtPointer, XmPushButtonCallbackStruct *);
void EditPage(Widget, XtPointer, XmPushButtonCallbackStruct *);
void SetPage(int);
void help_cb(Widget, XtPointer, XmAnyCallbackStruct *);

extern void ReadDB(char*);
extern void SaveDB(char*);

extern Page pages[];

Widget shell, notebook, textw, labelw;
Widget help_widget, file_dialog;
int currentPage = 1;
int modified;
extern int maxpages;
struct passwd *user;

int main(int argc, char* argv[])
{
  Widget mainw, menubar;
  Widget *file_menu, *edit_menu, *selected_menu, *help_menu, temp;
  Widget print_widget;
  Cardinal size;
  XtAppContext context;
  Arg args[10];
  int n, i;
  XmString tmp;
  char temppath[256];

  if (argc == 2 && strcmp(argv[1], "-help") == 0) {
    printf("Usage: todo [-todoFile pathname]\n");
    return(0);
  }

  shell		= XtVaAppInitialize(&context, APP_CLASS,
				    optionDesc, XtNumber(optionDesc),
				    &argc, argv,
				    fallback_resources, NULL);

  XtGetApplicationResources(shell, (XtPointer) &options,
			    resources, XtNumber(resources), (Arg *) NULL, 0);

  mainw		= XmCreateMainWindow(shell, "mainw", NULL, 0);
  XtManageChild(mainw);

  menubar	= XmCreateMenuBar(mainw, "menub", NULL, 0);
  XtManageChild(menubar);

  n = 0;
  XtSetArg(args[n], XmNcurrentPageNumber, 1); n++;
  XtSetArg(args[n], XmNlastPageNumber, 100); n++;
  notebook	= XmCreateNotebook(mainw, "notebook", args, n);
  XtManageChild(notebook);
  XtAddCallback(notebook, XmNpageChangedCallback,
		(XtCallbackProc) PageChange, NULL);

  n = 0;
  XtSetArg(args[n], XmNmenuBar, menubar); n++;
  XtSetValues(mainw, args, n);

  n = 0;
  XtSetArg(args[n], XmNpageNumber, 1); n++;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  textw		= XmCreateScrolledText(notebook, "text", args, n);
  XtManageChild(textw);
  XtAddCallback(textw, XmNvalueChangedCallback, TextChanged, NULL);

  n = 0;
  XtSetArg(args[n], XmNnotebookChildType, XmSTATUS_AREA); n++;
  XtSetArg(args[n], XmNpageNumber, 1); n++;
  labelw	= XmCreateLabel(notebook, "label", args, n);
  XtManageChild(labelw);

  XmdCreateMenu(FILE_MENU, menubar, &file_menu, &size);
  XtAddCallback(file_menu[FILE_NEW], XmNactivateCallback,
		(XtCallbackProc) New, NULL);
  XtAddCallback(file_menu[FILE_OPEN], XmNactivateCallback,
		(XtCallbackProc) PresentFDialog, (XtPointer) Open);
  XtAddCallback(file_menu[FILE_EXIT], XmNactivateCallback,
		(XtCallbackProc) QuitAppl, NULL);
  XtAddCallback(file_menu[FILE_SAVE_AS], XmNactivateCallback,
		(XtCallbackProc) PresentFDialog, (XtPointer) Save);
  XtAddCallback(file_menu[FILE_SAVE], XmNactivateCallback,
		(XtCallbackProc) SaveIt, NULL);

  XmdCreateMenu(SELECTED_MENU, menubar, &selected_menu, &size);
  XtUnmanageChildren(selected_menu, size);
  XtManageChild(selected_menu[SELECTED_PROPERTIES]);
  XtAddCallback(selected_menu[SELECTED_PROPERTIES],
		XmNactivateCallback, (XtCallbackProc) EditPage, NULL);
  XtManageChild(selected_menu[SELECTED_NEW]);
  XtAddCallback(selected_menu[SELECTED_NEW],
		XmNactivateCallback, (XtCallbackProc) NewPage, NULL);
  XtManageChild(selected_menu[SELECTED_DELETE]);
  XtAddCallback(selected_menu[SELECTED_DELETE],
  		XmNactivateCallback, (XtCallbackProc) DeletePage, NULL);

  XmdCreateMenu(HELP_MENU, menubar, &help_menu, &size);
  XtUnmanageChildren(help_menu, size);
  XtManageChild(help_menu[HELP_OVERVIEW]);

  print_widget = XmdCreatePrintDialog(shell, "print_manager", NULL, 0);
  XtAddCallback(print_widget, XmdNprintCallback, (XtCallbackProc) Print, NULL);
  tmp = XmStringCreateLocalized("About Printing");

  XtAddCallback(file_menu[FILE_PRINT], XmNactivateCallback,
		(XtCallbackProc) manageCB, (XtPointer) print_widget);

  help_widget = XmdCreateHelpDialog(shell, "help_manager", NULL, 0);
  XtAddCallback(help_menu[HELP_OVERVIEW], XmNactivateCallback,
		(XtCallbackProc) help_cb, (XtPointer) 0);
  XtAddCallback(print_widget, XmNhelpCallback,
		(XtCallbackProc) help_cb, (XtPointer) 1);

  n = 0;
  XtSetArg(args[n], XmNautoUnmanage, True); n++;
  file_dialog = XmCreateFileSelectionDialog(shell, "file_dialog", args, n);

  user = getpwuid(getuid());
  for (i = 0; i < MAXPAGES; i++) {
    pages[i] = NULL;
  }

  if (options.todoFile == NULL) {
    strcpy(temppath, user -> pw_dir);
    strcat(temppath, "/.todo");
    options.todoFile = XtNewString(temppath);
  } else {
    /* Copy the string for consistency */
    options.todoFile = XtNewString(options.todoFile);
  }

  XtVaSetValues(shell, XmNtitle, options.todoFile,
		XmNtitleEncoding, XA_STRING, NULL, NULL);
  ReadDB(options.todoFile);
  SetPage(0);

  XtRealizeWidget(shell);
  XtAppMainLoop(context);
  return 0;    /* make compiler happy */
}

static void QuitAppl(Widget w, char *i, XmPushButtonCallbackStruct *e)
{
	exit(EXIT_SUCCESS);
}

static void TextChanged(Widget w, XtPointer i, XtPointer i2)
{
	modified = 1;
}

void manageCB(Widget widget, Widget w_to_manage, XtPointer callback)
{
	(void)callback;
	if (w_to_manage)
		XtManageChild(w_to_manage);
}

void
PresentFDialog(Widget w, XtPointer cb, XmPushButtonCallbackStruct *cs)
{
  XtRemoveAllCallbacks(file_dialog, XmNokCallback);
  XtAddCallback(file_dialog, XmNokCallback, (XtCallbackProc) cb, NULL);
  XtManageChild(file_dialog);
}

void
New(Widget w, char* ignore, XmPushButtonCallbackStruct *cs)
{
  char buf[128];
  char *str;
  Boolean found = False;
  int i = 0;

  while(! found) {
    sprintf(buf, "untitled%d.todo", i++);
    found = access(buf, F_OK) != 0;
  }

  str = XtNewString(buf);
  ReadDB(str);
  XtFree(options.todoFile);
  options.todoFile = str;
  XtVaSetValues(shell, XmNtitle, str, XmNtitleEncoding,
		XA_STRING, NULL, NULL);
  SetPage(0);
}

void
Save(Widget w, char *i, XmFileSelectionBoxCallbackStruct *i2)
{
  char *str;
  if ((str = XmStringUnparse(i2->value, NULL, XmCHARSET_TEXT,
			    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL))) {
    SaveDB(str);
    XtFree(options.todoFile);
    options.todoFile = str;
    XtVaSetValues(shell, XmNtitle, str, XmNtitleEncoding,
		  XA_STRING, NULL, NULL);
  }
}

void
Open(Widget w, char *i, XmFileSelectionBoxCallbackStruct *i2)
{
  char *str;
  if ((str = XmStringUnparse(i2->value, NULL, XmCHARSET_TEXT,
			    XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL))) {
    ReadDB(str);
    XtFree(options.todoFile);
    options.todoFile = str;
    XtVaSetValues(shell, XmNtitle, str, XmNtitleEncoding,
		  XA_STRING, NULL, NULL);
  }
}

void
help_cb(Widget w, XtPointer item, XmAnyCallbackStruct *cb)
{
  XtManageChild(help_widget);
  XmdGotoHelpItem(w, (intptr_t)item, help_widget);
}

void
Print(Widget w, char *ignore, XmdPrintCallbackStruct *cb)
{
  int i;
  FILE *temp;
  int from, to;

  temp = fopen("/tmp/.todoout", "w");

  if (cb -> first == cb -> last &&
      cb -> first == 0) {
    from = 0;
    to = maxpages - 1;
  } else {
    from = MAX(0, cb -> first - 1);
    to = MIN(maxpages, cb -> last - 1);
  }

  for (i = from; i <= to; i++) {
    if (pages[i] -> label != NULL) {
      fprintf(temp, "Subject: %s\n", pages[i] -> label);
      fprintf(temp, "---------------------------\n\n\n");
    }
    fprintf(temp, "%s", pages[i] -> page);
    if (i != (maxpages - 1)) fprintf(temp, "\f");
  }

  fclose(temp);
  XmdPrintDocument("/tmp/.todoout", cb);
}
