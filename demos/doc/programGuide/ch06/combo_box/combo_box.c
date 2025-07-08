/* $XConsortium: combo_box.c /main/5 1995/07/15 20:39:34 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <stdio.h>
#include <Xm/XmAll.h>

void   CreateComboBox(Widget);
extern void CreateMenus(Widget);
extern void SelectionCB(Widget, XtPointer, XtPointer); 
extern void HelpCB(Widget, XtPointer, XtPointer); 
extern void QuitCB(Widget, XtPointer, XtPointer);
Widget top_level; 

#define APP_CLASS "XmdComboBox"

static String fallbacks[] = {
"combobox.allowShellResize: True",
"combobox*fontList: fixed",
NULL
};

int
main(int    argc,
     char **argv)
{
 XtAppContext app_context;
 Widget       main_window;

   XtSetLanguageProc(NULL, NULL, NULL);
   top_level = XtVaOpenApplication(&app_context, APP_CLASS, 
                                NULL, 0, &argc, argv, 
                                fallbacks,
                                sessionShellWidgetClass, 
                                NULL);
   main_window = XtVaCreateManagedWidget("main_window", 
                                xmMainWindowWidgetClass, top_level,
                                NULL);

   CreateMenus(main_window);
   CreateComboBox(main_window);

   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}



void
CreateComboBox(Widget parent_of_combo_box)
{
#define NUM_LIST_ITEMS 5
   Widget          comboBox;
   Arg             args[10];
   Cardinal        n, i;
   XmString        ListItem[NUM_LIST_ITEMS];
   static char    *ListString[] = { "kiwi", 
                                    "raspberry", 
                                    "carambola", 
                                    "litchi",
                                    "coconut" };
            
   /* Create a list of XmStrings for the ComboBox List child */
     for (i=0; i < NUM_LIST_ITEMS; i++)
       ListItem[i] = XmStringCreate (ListString[i], XmSTRING_DEFAULT_CHARSET);


   /* Create a ComboBox of type XmDROP_DOWN_COMBO_BOX. */
   /* Resources passed to ComboBox are passed on to the
    * children of ComboBox.  So, in the argument list
    * below, the resources, XmNitems, and XmNitemCount
    * will be passed on to the List child of ComboBox.  */
   n=0; 
   XtSetArg (args[n], XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX); n++;
   XtSetArg (args[n], XmNarrowSpacing, 5); n++;
   XtSetArg (args[n], XmNitems, ListItem); n++;
   XtSetArg (args[n], XmNitemCount, NUM_LIST_ITEMS); n++;
   comboBox = XmCreateComboBox (parent_of_combo_box, "ComboBox", args, n);
   XtManageChild (comboBox);
   XtAddCallback (comboBox, XmNselectionCallback, SelectionCB, 
      (XtPointer)NULL);

   /* Example of manipulating a child widget directly to set the
    * visibleItemCount on the list.  */
   n=0;
   XtSetArg (args[n], XmNvisibleItemCount, 5); n++;
   XtSetValues (XtNameToWidget (comboBox,"*List"), args, n);  
}



void
SelectionCB (Widget w, 
                   XtPointer client_data, 
                   XtPointer call_data)
{
 XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *)call_data;
 XmStringCharSet	charset;
 XmStringDirection	direction;
 XmStringContext	context;
 Boolean		separator;
 char                  *item;

   /* This callback procedure prints the item that was just selected. */

   /* Retrieve the selected text string from XmString. */
     item = XmStringUnparse(cb->item_or_text, NULL, XmCHARSET_TEXT,
                            XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
     printf ("ComboBox SelectionCB: item = %s\n\n", item);
}
