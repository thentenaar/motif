/* $XConsortium: container.c /main/5 1995/07/15 20:39:52 drk $ */
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

void   CreateContainer(Widget);
void   MakeACall(Widget, XtPointer, XtPointer);
extern void CreateMenus(Widget);
extern void HelpCB(Widget, XtPointer, XtPointer);
extern void QuitCB(Widget, XtPointer, XtPointer);
Widget top_level;
XtAppContext app_context;

#define APP_CLASS "XmdContainer"

static String fallbacks[] = {
"container.allowShellResize: True",
"container*fontList: fixed",
NULL
};

int
main(int    argc,
     char **argv)
{
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
   CreateContainer(main_window);

   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}


void
MakeACall(Widget        w,
          XtPointer     client_data,
          XtPointer     call_data)
{
  printf("Ring!\n");
}


void
CreateContainer(Widget parent_of_container)
{
 Widget  container1;
 Widget  president, vice_president, dir_of_sales, dir_of_rnd, dir_of_mfr;

   container1 = XtVaCreateWidget("Container",
   			xmContainerWidgetClass, parent_of_container,
   			XmNlayoutType, XmOUTLINE,
   			XmNselectionPolicy, XmBROWSE_SELECT,
   			XmNautomaticSelection, XmNO_AUTO_SELECT,
   			XmNentryViewType, XmSMALL_ICON,
   			NULL);
   XtAddCallback(container1, XmNselectionCallback, MakeACall,
                 (XtPointer)NULL);

   president = XtVaCreateManagedWidget("President",
   			xmIconGadgetClass, container1,
   			XmNoutlineState, XmEXPANDED,
                        NULL);
   vice_president = XtVaCreateManagedWidget("Vice-President",
   			xmIconGadgetClass, container1,
   			XmNentryParent, president,
   			XmNoutlineState, XmEXPANDED,
                        NULL);
   dir_of_sales = XtVaCreateManagedWidget("Director of Sales",
   			xmIconGadgetClass, container1,
   			XmNentryParent, vice_president,
                        NULL);
   dir_of_rnd = XtVaCreateManagedWidget("Director of R&D",
   			xmIconGadgetClass, container1,
   			XmNentryParent, vice_president,
                        NULL);
   dir_of_mfr = XtVaCreateManagedWidget("Director of Manufacturing",
   			xmIconGadgetClass, container1,
   			XmNentryParent, vice_president,
                        NULL);
   XtManageChild(container1);
}
