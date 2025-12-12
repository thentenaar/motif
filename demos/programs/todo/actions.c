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

#ifdef REV_INFO
#ifndef lint
static char *rcsid = "$TOG: actions.c /main/7 1997/05/02 10:01:40 dbl $";
#endif
#endif

#include <stdlib.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Notebook.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/BulletinB.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Exm/TabB.h>

#include "page.h"

extern Page pages[];
Widget editDialog = 0, deleteDialog = 0;

void SetPage(int);
void AdjustPages(int, int);
void FixPages();
void PageChange(Widget w, XtPointer i, XmNotebookCallbackStruct *cs);
Page AllocPage();
char* Trim(char*);
extern void SaveDB(char*);

extern Widget shell, textw, labelw, notebook;
extern int maxpages, currentPage, modified;

void 
NewPage(Widget w, XtPointer i, XmPushButtonCallbackStruct *cs) 
{
  Arg args[2];

  if (modified && pages[currentPage] != NULL) {
    if (pages[currentPage] -> page != NULL) 
      XtFree(pages[currentPage] -> page);
     pages[currentPage] -> page = XmTextGetString(textw);
  }
  AdjustPages(currentPage, 1);
  pages[currentPage] = AllocPage();
  FixPages();
  XtSetArg(args[0], XmNcurrentPageNumber, (currentPage + 1));
  XtSetArg(args[1], XmNlastPageNumber, (maxpages + 1));
  XtSetValues(notebook, args, 2);
  SetPage(currentPage);
}

void 
DoDeletePage(Widget w, XtPointer i, XmPushButtonCallbackStruct *cs) 
{
  Arg args[2];

  if (pages[currentPage] != NULL) {
    if (pages[currentPage] -> page != NULL) XtFree(pages[currentPage] -> page);
    if (pages[currentPage] -> minorPB != (Widget) 0)
      XtDestroyWidget(pages[currentPage] -> minorPB);
    if (pages[currentPage] -> majorPB != (Widget) 0)
      XtDestroyWidget(pages[currentPage] -> majorPB);
    XtFree((XtPointer) pages[currentPage]);
  }
  pages[currentPage] = NULL;
  AdjustPages(currentPage, -1);

  /* If there are no more pages left,  then create a blank one */
  if (maxpages < 0) {
    pages[0] = AllocPage();
    pages[0] -> page = XtMalloc(2);
    pages[0] -> page[0] = 0;
    maxpages = 0;
  }

  FixPages();
  XtSetArg(args[0], XmNcurrentPageNumber, (currentPage + 1));
  XtSetArg(args[1], XmNlastPageNumber, (maxpages + 1));
  XtSetValues(notebook, args, 2);
  SetPage(currentPage);
}

void 
DeletePage(Widget w, XtPointer i, XmPushButtonCallbackStruct *cs) 
{
  if (deleteDialog == 0) {
    Arg args[10];
    XmString okLabel, promptText, dialogTitle; 

    dialogTitle = XmStringCreateLocalized("Page Delete Dialog");
    okLabel = XmStringCreateLocalized("Delete");
    promptText = XmStringCreateLocalized("Really delete this page?");
    XtSetArg(args[0], XmNokLabelString, okLabel);
    XtSetArg(args[1], XmNmessageString, promptText);
    XtSetArg(args[2], XmNdialogTitle, dialogTitle);
    XtSetArg(args[3], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
    deleteDialog = XmCreateMessageDialog(shell, "DeletePage", 
					args, 4);
    XtAddCallback(deleteDialog, XmNokCallback, 
		  (XtCallbackProc) DoDeletePage, NULL);
    XmStringFree(dialogTitle);
    XmStringFree(okLabel);
    XmStringFree(promptText);
  }

  XtManageChild(deleteDialog);
}

Widget labelEditW, majorTabW, minorTabW;

void 
DoEditPage(Widget w, XtPointer ig, XmPushButtonCallbackStruct *cs) 
{
  char *temp;
  XmString tstr;
  Arg args[5];
  int i;

  pages[currentPage] -> label = XmTextFieldGetString(labelEditW);

  if (pages[currentPage] -> minorTab != NULL)
    XtFree(pages[currentPage] -> minorTab);
  temp = XmTextGetString(minorTabW);
  temp = Trim(temp);
  if (strlen(temp) > 0)
    pages[currentPage] -> minorTab = temp;
  else {
    XtFree(temp);
    pages[currentPage] -> minorTab = NULL;
    if (pages[currentPage] -> minorPB)
      XtUnmanageChild(pages[currentPage] -> minorPB);
  }

  if (pages[currentPage] -> majorTab != NULL)
    XtFree(pages[currentPage] -> majorTab);
  temp = XmTextGetString(majorTabW);
  temp = Trim(temp);
  if (strlen(temp) > 0) 
    pages[currentPage] -> majorTab = temp;
  else {
    XtFree(temp);
    pages[currentPage] -> majorTab = NULL;
    if (pages[currentPage] -> majorPB)
      XtUnmanageChild(pages[currentPage] -> majorPB);
  }

  if (pages[currentPage] -> majorTab != NULL) {
    if (pages[currentPage] -> majorPB == (Widget) 0) {
      i = 0;
      XtSetArg(args[i], XmNpageNumber, currentPage + 1); i++;
      XtSetArg(args[i], XmNnotebookChildType, XmMAJOR_TAB); i++;
      XtSetArg(args[i], XmNshadowThickness, 1); i++;
      pages[currentPage] -> majorPB =
	ExmCreateTabButton(notebook, "atab", args, i);
    }
    tstr = XmStringGenerate(pages[currentPage] -> majorTab, NULL, 
			    XmCHARSET_TEXT, NULL);
    XtSetArg(args[0], ExmNcompoundString, tstr);
    XtSetValues(pages[currentPage] -> majorPB, args, 1);
    XtManageChild(pages[currentPage] -> majorPB);
  }

  if (pages[currentPage] -> minorTab != NULL) {
    if (pages[currentPage] -> minorPB == (Widget) 0) {
      i = 0;
      XtSetArg(args[i], XmNpageNumber, currentPage + 1); i++;
      XtSetArg(args[i], XmNnotebookChildType, XmMINOR_TAB); i++;
      XtSetArg(args[i], XmNshadowThickness, 1); i++;
      pages[currentPage] -> minorPB =
	ExmCreateTabButton(notebook, "atab", args, i);
    }
    tstr = XmStringGenerate(pages[currentPage] -> minorTab, NULL, 
			    XmCHARSET_TEXT, NULL);
    XtSetArg(args[0], ExmNcompoundString, tstr);
    XtSetValues(pages[currentPage] -> minorPB, args, 1);
    XtManageChild(pages[currentPage] -> minorPB);
  }

  /* Get contents before update */
  XtFree(pages[currentPage] -> page);
  pages[currentPage] -> page = XmTextGetString(textw);

  SetPage(currentPage);
}

void 
EditPage(Widget w, XtPointer i, XmPushButtonCallbackStruct *cs) 
{
  if (pages[currentPage] == NULL) return;

  if (editDialog == 0) {
    Arg args[10];
    XmString promptText, dialogTitle, temp; 
    XmString okLabel, cancelLabel;
    Widget label, rc1, rc2;

    dialogTitle = XmStringCreateLocalized("Page Edit Dialog");
    promptText = XmStringCreateLocalized("Really edit this page?");
    okLabel = XmStringCreateLocalized("OK");
    cancelLabel = XmStringCreateLocalized("Cancel");
    XtSetArg(args[0], XmNselectionLabelString, promptText);
    XtSetArg(args[1], XmNdialogTitle, dialogTitle);
    XtSetArg(args[2], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
    XtSetArg(args[3], XmNokLabelString, okLabel);
    XtSetArg(args[4], XmNcancelLabelString, cancelLabel);
    editDialog = XmCreateTemplateDialog(shell, "EditPage", 
					args, 5);
    XtAddCallback(editDialog, XmNokCallback,
		  (XtCallbackProc) DoEditPage, NULL);

    XmStringFree(dialogTitle);
    XmStringFree(promptText);
    XmStringFree(okLabel);
    XmStringFree(cancelLabel);
    
    XtSetArg(args[0], XmNorientation, XmVERTICAL); 
    rc1 = XmCreateRowColumn(editDialog, "rc", args, 1);
    XtManageChild(rc1);

    XtSetArg(args[0], XmNorientation, XmHORIZONTAL);
    rc2 = XmCreateRowColumn(rc1, "rc2", args, 1);
    temp = XmStringCreateLocalized("Label:");
    XtSetArg(args[0], XmNwidth, 100);
    XtSetArg(args[1], XmNlabelString, temp);
    label = XmCreateLabelGadget(rc2, "label", args, 2);
    XtManageChild(label);
    XmStringFree(temp);
    XtManageChild(rc2);

    labelEditW = XmCreateTextField(rc2, "editTF", NULL, 0);
    XtManageChild(labelEditW);

    XtSetArg(args[0], XmNorientation, XmHORIZONTAL);
    rc2 = XmCreateRowColumn(rc1, "rc2", args, 1);
    temp = XmStringCreateLocalized("Major Tab:");
    XtSetArg(args[0], XmNwidth, 100);
    XtSetArg(args[1], XmNlabelString, temp);
    label = XmCreateLabelGadget(rc2, "label", args, 2);
    XtManageChild(label);
    XmStringFree(temp);

    majorTabW = XmCreateTextField(rc2, "majorTF", NULL, 0);
    XtManageChild(majorTabW);
    XtManageChild(rc2);

    XtSetArg(args[0], XmNorientation, XmHORIZONTAL);
    rc2 = XmCreateRowColumn(rc1, "rc2", args, 1);
    temp = XmStringCreateLocalized("Minor Tab:");
    XtSetArg(args[0], XmNwidth, 100);
    XtSetArg(args[1], XmNlabelString, temp);
    label = XmCreateLabelGadget(rc2, "label", args, 2);
    XtManageChild(label);
    XmStringFree(temp);

    minorTabW = XmCreateTextField(rc2, "minorTF", NULL, 0);
    XtManageChild(minorTabW);
    XtManageChild(rc2);
  }

  if (pages[currentPage] -> label != NULL)
    XmTextFieldSetString(labelEditW, pages[currentPage] -> label);
  else
    XmTextFieldSetString(labelEditW, "");

  if (pages[currentPage] -> majorTab != NULL)
    XmTextSetString(majorTabW, pages[currentPage] -> majorTab);
  else
    XmTextSetString(majorTabW, "");

  if (pages[currentPage] -> minorTab != NULL)
    XmTextSetString(minorTabW, pages[currentPage] -> minorTab);
  else
    XmTextSetString(minorTabW, "");

  XtManageChild(editDialog);
}

void 
SaveIt(Widget w, char *i, XmPushButtonCallbackStruct *e) 
{
  SaveDB(options.todoFile);
}

void 
SetPage(int pageNumber) 
{
  XmString tmp;
  char buf[80];
  Arg args[5];

  currentPage = pageNumber;
  if (pageNumber <= maxpages) {
    XtSetArg(args[0], XmNpageNumber, (pageNumber + 1));
    XtSetValues(XtParent(textw), args, 1);
  }
  if (pages[pageNumber] != NULL) {
    if (pages[pageNumber] -> page != NULL)
      XtSetArg(args[0], XmNvalue, pages[pageNumber] -> page);
    else
      XtSetArg(args[0], XmNvalue, "");
    XtSetValues(textw, args, 1);

    if (pages[pageNumber] -> label != NULL)
      tmp = XmStringCreateLocalized(pages[pageNumber] -> label);
    else {
      sprintf(buf, "Page %d", pageNumber + 1);
      tmp = XmStringCreateLocalized(buf);
    }
  }
  else {
    XtSetArg(args[0], XmNvalue, "");
    XtSetValues(textw, args, 1);
    sprintf(buf, "Page %d (Bad Page)", pageNumber + 1);
    tmp = XmStringCreateLocalized(buf);
  }

  XmTextSetTopCharacter(textw, pages[pageNumber] -> lasttoppos);
  XmTextSetInsertionPosition(textw, pages[pageNumber] -> lastcursorpos);
    
  XtSetArg(args[0], XmNpageNumber, (pageNumber + 1));
  XtSetArg(args[1], XmNlabelString, tmp);
  XtSetValues(labelw, args, 2);
  XmStringFree(tmp);
}

void 
PageChange(Widget w, XtPointer i, XmNotebookCallbackStruct *cs) 
{
  if (modified && pages[currentPage] != NULL) {
    if (pages[currentPage] -> page != NULL) 
      XtFree(pages[currentPage] -> page);
    pages[currentPage] -> page = XmTextGetString(textw);
    pages[currentPage] -> lasttoppos = XmTextGetTopCharacter(textw);
    pages[currentPage] -> lastcursorpos = XmTextGetInsertionPosition(textw);
  }

  SetPage(cs -> page_number - 1);
}

void 
AdjustPages(int startpage, int ins) 
{
  int i;

  /* ins is either +1 or -1 for insert or delete a page */

  if (ins > 0) {
    for(i = maxpages; i >= startpage; i--)
      pages[i + 1] = pages[i];
    maxpages += 1;
  } else {
    for(i = startpage; i <= maxpages; i++)
      pages[i] = pages[i + 1];
    maxpages -= 1;
  }
}

void FixPages() {
  int i;
  Arg args[2];

  /* Now scan the pages and fix the tabs */
  for (i = 0; i <= maxpages; i++) {
    XtSetArg(args[0], XmNpageNumber, (i + 1));
    if (pages[i] -> majorPB != (Widget) 0)
      XtSetValues(pages[i] -> majorPB, args, 1);
    if (pages[i] -> minorPB != (Widget) 0)
      XtSetValues(pages[i] -> minorPB, args, 1);
  }
}

/* This function removes leading and trailing whitespace.  It
   deallocates the original string and returns a brand new string */

char* 
Trim(char* str)
{
  char *new;
  int first, last;
  int length;

  if (str == NULL) return(XtNewString(""));

  length = strlen(str);

  for(first = 0; first < length && isspace(str[first]); first++);
  for(last = length - 1; last > first && isspace(str[last]); last--);

  if (! isspace(str[last])) last++; /* Last needs to be adjusted */

  if (first == last) /* Empty */
    {
      free(str);
      return(strdup(""));
    }
  else
    {
      new = XtMalloc(last - first + 1); /* Don't forget last 0 */
      strncpy(new, &str[first], last - first);
      new[last - first] = 0;
      free(str);
      return(new);
    }
}
