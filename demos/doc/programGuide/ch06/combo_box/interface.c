/* $XConsortium: interface.c /main/4 1995/07/15 20:39:38 drk $ */
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
#include <stdlib.h>
#include <Xm/XmAll.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

void CreateMenus(Widget);
void HelpCB(Widget, XtPointer, XtPointer);
void QuitCB(Widget, XtPointer, XtPointer);
extern Widget top_level;
extern XtAppContext app_context;

/**************************************************************************
CreateMenus: This function generates the menu bar and the submenus.
**************************************************************************/
void
CreateMenus(Widget parent_of_menu_bar)
{
 XmString   file, help;
 Widget     menubar, FilePullDown, HelpPullDown;
 Widget     overview, quit, Help1;

 /* Create the menubar itself. */
   file = XmStringCreateLocalized("File");
   help = XmStringCreateLocalized("Help");

   menubar      = (Widget)XmCreateMenuBar(parent_of_menu_bar, "menubar",
                                          NULL, 0);
   FilePullDown = (Widget)XmCreatePulldownMenu(menubar, "FilePullDown",
                                               NULL, 0);
   HelpPullDown = (Widget)XmCreatePulldownMenu(menubar, "HelpPullDown",
                                                 NULL, 0);

 /******************************FILE*********************************/
    XtVaCreateManagedWidget("File", xmCascadeButtonWidgetClass, menubar,
                             XmNlabelString, file,
                             XmNmnemonic, 'F',
                             XmNsubMenuId, FilePullDown,
                             NULL);
    quit = XtVaCreateManagedWidget("Quit", xmPushButtonGadgetClass,
                                    FilePullDown, NULL);
    XtAddCallback(quit, XmNactivateCallback, QuitCB, NULL);


 /******************************HELP*********************************/
    Help1 = XtVaCreateManagedWidget("Help", xmCascadeButtonWidgetClass,
                             menubar,
                             XmNlabelString, help,
                             XmNmnemonic, 'H',
                             XmNsubMenuId, HelpPullDown,
                             NULL);
    XtVaSetValues(menubar, XmNmenuHelpWidget, Help1, NULL);
    overview = XtVaCreateManagedWidget("Overview", xmPushButtonGadgetClass,
                                    HelpPullDown, NULL);
    XtAddCallback(overview, XmNactivateCallback, HelpCB, (XtPointer)1);

    XmStringFree(file);
    XmStringFree(help);

    XtManageChild(menubar);
}



/*********************************************************************
HelpCB: This function is called when the user requests help.  This
        function displays a Message DialogBox.
*********************************************************************/
void
HelpCB(Widget   w,
       XtPointer cd,
       XtPointer cb
      )
{
 char      help_string[400];
 XmString  hs_as_cs;
 Widget    dialog_general_help;
 Arg       arg[3];

 sprintf(help_string,
"This program demonstrates how to use an XmComboBox in an application.\n\
You can make a selection by typing it directly into the TextField\n\
or by selecting the item from the DropDown list.  To make the list\n\
drop down, click on the arrow that appears next to the TextField.");

   hs_as_cs = XmStringLtoRCreate(help_string,
                                 XmFONTLIST_DEFAULT_TAG);

   XtSetArg(arg[0], XmNmessageString, hs_as_cs);
   dialog_general_help = (Widget)XmCreateMessageDialog(top_level,
                                             "message", arg, 1);
   XmStringFree(hs_as_cs);

   switch ((intptr_t)cd)  {
     case 1: XtManageChild(dialog_general_help);
             break;
     default: /* no other help */
             break;
   }

}

/*******************************************************************************
QuitCB: Exit
*******************************************************************************/
void
QuitCB(Widget w, XtPointer cd, XtPointer cb)
{
	(void)w;
	(void)cd;
	(void)cb;
	XtAppSetExitFlag(app_context);
}

