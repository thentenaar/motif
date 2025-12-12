/* $XConsortium: notebook.c /main/5 1995/07/15 20:40:05 drk $ */
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
#include <stdio.h>
#include <Xm/XmAll.h>

void   CreateNotebook(Widget);
extern void CreateMenus(Widget);
extern void HelpCB(Widget, XtPointer, XtPointer);
extern void QuitCB(Widget, XtPointer, XtPointer);
Widget top_level;
XtAppContext app_context;

#define APP_CLASS "XmdNotebook"

int
main(int    argc,
     char **argv)
{
 Widget       main_window;

   XtSetLanguageProc(NULL, NULL, NULL);
   top_level = XtVaOpenApplication(&app_context, APP_CLASS,
                                NULL, 0, &argc, argv,
                                NULL,
                                sessionShellWidgetClass,
                                NULL);
   main_window = XtVaCreateManagedWidget("main_window",
                                xmMainWindowWidgetClass, top_level,
                                NULL);

   CreateMenus(main_window);
   CreateNotebook(main_window);

   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}


void
CreateNotebook(Widget parent_of_notebook)
{
#define PAGES_IN_NOTEBOOK 7
#define NUMBER_OF_MAJOR_TABS 2
 Widget notebook, frame;
 char buff[80];
 int i;
 static char *info[PAGES_IN_NOTEBOOK+1] = {
                                  "dummy",
                                  "apples are high in fiber",
                                  "bananas are high in Potassium",
                                  "oranges are high in Vitamin C",
                                  "celery",
                                  "lettuce",
                                  "sweet potato",
                                  "carrot"
                                 };

   notebook = XtVaCreateWidget("notebook", xmNotebookWidgetClass,
                               parent_of_notebook, NULL);

 /* Create the pages of the Notebook. */
   for (i=1; i<=PAGES_IN_NOTEBOOK; i++) {
      /* Create a frame on every page. */
	frame = XtVaCreateManagedWidget("frame",
                     xmFrameWidgetClass, notebook,
                     XmNnotebookChildType, XmPAGE,
                     XmNpageNumber,i,
		     NULL);

      /* Place the page contents (a string) on each page. */
	XtVaCreateManagedWidget(info[i],
                     xmLabelWidgetClass, frame,
		     NULL);
   }

   XtVaCreateManagedWidget("tropical only",
                     xmLabelWidgetClass, notebook,
                     XmNnotebookChildType, XmSTATUS_AREA,
                     XmNpageNumber, 2,
                     NULL);

 /* Create major tabs to divide the pages into categories. */
   XtVaCreateManagedWidget("fruits",
                xmPushButtonWidgetClass, notebook,
                XmNnotebookChildType, XmMAJOR_TAB,
                XmNpageNumber, 1,
                NULL);
   XtVaCreateManagedWidget("vegetables",
                xmPushButtonWidgetClass, notebook,
                XmNnotebookChildType, XmMAJOR_TAB,
                XmNpageNumber, 4,
                NULL);


 /* Create some minor tabs to divide the categories into
    subcategories. */
   XtVaCreateManagedWidget("green",
                xmPushButtonWidgetClass, notebook,
                XmNnotebookChildType, XmMINOR_TAB,
                XmNpageNumber, 4,
                NULL);
   XtVaCreateManagedWidget("orange",
                xmPushButtonWidgetClass, notebook,
                XmNnotebookChildType, XmMINOR_TAB,
                XmNpageNumber, 6,
                NULL);

   XtManageChild(notebook);

}
