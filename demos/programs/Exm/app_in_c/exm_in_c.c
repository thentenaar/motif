/* $XConsortium: exm_in_c.c /main/5 1995/07/15 20:44:17 drk $ */
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


/*******************************************************************************
 *
 * AllExmDemo.c: This is a sample application written in C that instantiates
 *               all of the Exm widgets.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/TraitP.h>
#include <Exm/Simple.h>
#include <Exm/ExmString.h>
#include <Exm/StringTrans.h>
#include <Exm/CommandB.h>
#include <Exm/Grid.h>
#include <Exm/TabB.h>
#include <Exm/Panner.h>
#include <Exm/MenuB.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

void CreateMenus(Widget);
void ExerciseExmSimpleWidgets(Widget);
void ExerciseExmStringWidgets(Widget);
void ExerciseExmStringTransferWidgets(Widget);
void ExerciseExmCommandButtonWidgets(Widget);
void ExerciseExmTabButtonWidget(Widget);
void QuitCB(Widget, XtPointer, XtPointer);
void HelpCB(Widget, XtPointer, XtPointer);
void DanceCB(Widget, XtPointer, XtPointer);
void YesCB(Widget, XtPointer, XtPointer);
void NoCB(Widget, XtPointer, XtPointer);

Widget   top_level, MainWindow;
enum WidgetN {SimpleH=0, StringH, StringTransferH, CommandButtonH,
              TabButtonH, PannerH, MenuButtonH, GridH};

int
main(int argc, char **argv)
{
 static Widget  Frame1;
 XtAppContext   app_context;
 Arg            MainWindowArgs[6];
 Arg            args[12];
 Cardinal       n;
 Widget         Grid;
 Widget         Panner1;
 Widget         Label1;

   top_level = XtAppInitialize(&app_context, "Test", NULL, 0,
                               &argc, argv, NULL, NULL, 0);

   n=0;
   XtSetArg(MainWindowArgs[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
   XtSetArg(MainWindowArgs[n], XmNwidth, 400); n++;
   XtSetArg(MainWindowArgs[n], XmNheight, 400); n++;
   MainWindow = (Widget)XmCreateMainWindow(top_level, "MainWindow",
                                           MainWindowArgs, n);
   XtManageChild (MainWindow);

   CreateMenus(MainWindow);

 /* An ExmGrid manager widget will manage most of the Exm primitives. */
   Grid = XtVaCreateManagedWidget ("Grid",
                                   exmGridWidgetClass, MainWindow,
                                   XmNrows, 3,
                                   XmNcolumns, 4,
                                   NULL);
   XtAddCallback (Grid, XmNhelpCallback,
                  HelpCB, (XtPointer)GridH);

   ExerciseExmSimpleWidgets(Grid);
   ExerciseExmStringWidgets(Grid);
   ExerciseExmStringTransferWidgets(Grid);
   ExerciseExmTabButtonWidget(Grid);


 /* This section exercises the ExmPanner widget. */
   n=0;
   XtSetArg(args[n], XmNwidth, 150); n++;
   XtSetArg(args[n], XmNheight, 150); n++;
   XtSetArg(args[n], XmNscrolledWindowChildType, XmMESSAGE_WINDOW); n++;
   /* XtSetArg(args[n], ExmNresize, False); n++; */
   Panner1 = XtCreateWidget ("Panner1", exmPannerWidgetClass, MainWindow,
                              args, n);
   XtAddCallback(Panner1, XmNhelpCallback, HelpCB, (XtPointer)PannerH);
   XtManageChild(Panner1);

   {
    XmScrollFrameTrait  sf_trait =
        (XmScrollFrameTrait) XmeTraitGet((XtPointer)XtClass(MainWindow),
                                         XmQTscrollFrame);
    sf_trait->addNavigator(MainWindow, Panner1,
                           NavigDimensionX|NavigDimensionY);
   }


   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}


/*******************************************************************************
 *
 * CreateMenus:
 *      This function generates the menu bar and the submenus.
 *      It exercises the ExmMenuButton widget.
 *
 ******************************************************************************/
void
CreateMenus(Widget parent_of_menu_bar)
{
 static Widget  PullDownFromFile, PullDownFromHelp;
 static Widget  HelpWidget, menu_bar, QuitWidget;
 char           menu_button_name[24];
 static Widget  MenuButtonWidget[8];
 XmString       file_string, quit_string, help_cs, menu_as_a_cs,
                acc_string;
 int            menu_num;
 static char          *menu_help_string[] = {
                                 "ExmSimple",
                                 "ExmString",
                                 "ExmStringTransfer",
                                 "ExmCommandButton",
                                 "ExmTabButton",
                                 "ExmPanner",
                                 "ExmMenuButton",
                                 "ExmGrid"
                                };
 static char		menu_help_mnemonics[] = "ESTCBPMG";
 int            n;
 Arg            args[10] ;


 /* Create a MenuBar in MainWindow. */
   n = 0;
   menu_bar = (Widget)XmCreateMenuBar (parent_of_menu_bar, "menu_bar",
                                       args, n);
   XtManageChild (menu_bar);

 /* Create a context for the pulldown menu. */
   n = 0;
   PullDownFromFile = (Widget)XmCreatePulldownMenu (menu_bar, "file_pane", args, n);

 /* Place the "File" cascade button at the top of the pulldown. */
   file_string = XmStringCreateLocalized("File");
   XtVaCreateManagedWidget("File", xmCascadeButtonWidgetClass, menu_bar,
                           XmNlabelString, file_string,
			   XmNmnemonic, 'F',
                           XmNsubMenuId, PullDownFromFile,
                           NULL);

 /* Create the entries pulled down by pressing "File" */
   quit_string = XmStringCreateLocalized("Quit");
   acc_string = XmStringCreateLocalized("Alt-q");

   QuitWidget = XtVaCreateManagedWidget("Quit",
					exmMenuButtonWidgetClass,
					PullDownFromFile,
					ExmNcompoundString, quit_string,
					XmNmnemonic, 'Q',
					XmNacceleratorText, acc_string,
					XmNaccelerator, "Alt<Key>q",
					NULL);
   XtAddCallback (QuitWidget, XmNactivateCallback, QuitCB, NULL);
   XmStringFree(file_string);
   XmStringFree(quit_string);
   XmStringFree(acc_string);


 /* Create "Help" PulldownMenu with Help button.  */
   n = 0;
   PullDownFromHelp = (Widget)(XmCreatePulldownMenu (menu_bar, "help_pane",
                                                     args, n));

 /* Place the "Help" cascade button at the top of the pulldown. */
   help_cs = XmStringCreateLocalized("Help");
   HelpWidget = XtVaCreateManagedWidget("Help", xmCascadeButtonWidgetClass,
                                        menu_bar,
                                        XmNlabelString, help_cs,
					XmNmnemonic, 'H',
                                        XmNsubMenuId, PullDownFromHelp,
                                        NULL);
 /* Make sure that the "Help" button appears in the appropriate place within
    the menubar. */
   XtVaSetValues(menu_bar, XmNmenuHelpWidget, HelpWidget, NULL);


   for (menu_num=0; menu_num<8; menu_num++)   {
      char mnemonic_char;

      sprintf(menu_button_name, "Menu Button %d", menu_num);
      menu_as_a_cs = XmStringCreateLocalized(menu_help_string[menu_num]);
      mnemonic_char = menu_help_mnemonics[menu_num];
      MenuButtonWidget[menu_num] =
	XtVaCreateManagedWidget(menu_button_name,
                                exmMenuButtonWidgetClass, PullDownFromHelp,
                                ExmNcompoundString, menu_as_a_cs,
				XmNmnemonic, mnemonic_char,
                                NULL);
      XtAddCallback (MenuButtonWidget[menu_num], XmNactivateCallback,
                     HelpCB, (XtPointer)(intptr_t)menu_num);
      XmStringFree(menu_as_a_cs);
   }
}



/*******************************************************************************
 *
 * ExerciseExmSimpleWidgets:
 *       This function instantiates two ExmSimple widgets.
 *
 ******************************************************************************/
void
ExerciseExmSimpleWidgets(Widget parent_handle)
{
 Widget       Simple1, Simple2;
 Arg          args[10];
 Cardinal     n = 0;

 /* Instantiate two ExmSimple widgets. */
   Simple1 = ExmCreateSimple(parent_handle, "Simple1", args, 0);
   XtAddCallback(Simple1, XmNhelpCallback, HelpCB, (XtPointer)SimpleH);

   Simple2 = ExmCreateSimple(parent_handle, "Simple3", args, 0);
   XtVaSetValues (Simple2,
                  ExmNsimpleShape, ExmSHAPE_RECTANGLE,
                  XmNunitType,     Xm100TH_MILLIMETERS,
                  XmNsensitive,    False,
                  XmNmarginWidth,  300,
                  XmNmarginHeight, 2000,
                  NULL);
   XtAddCallback(Simple2, XmNhelpCallback, HelpCB, (XtPointer)SimpleH);

   XtManageChild(Simple1);  XtManageChild(Simple2);
}


/*******************************************************************************
 *
 * ExerciseExmStringWidgets:
 *       This function instantiates four ExmString widgets.
 *
 ******************************************************************************/
void
ExerciseExmStringWidgets(Widget parent_handle)
{
 Widget       String1, String2;
 XmString     a_compound_string;

  a_compound_string = XmStringLtoRCreate("A Multiline Compound String\nIn an\n\
ExmString widget.\0", XmFONTLIST_DEFAULT_TAG);


/* Instantiate two ExmString widgets. */
  String1 = XtVaCreateManagedWidget("String1",
                                    exmStringWidgetClass, parent_handle,
                                    ExmNcompoundString,  a_compound_string,
                                    NULL);

  String2 = XtVaCreateManagedWidget("String2",
                                    exmStringWidgetClass, parent_handle,
                                    XmNalignment,        XmALIGNMENT_BEGINNING,
                                    ExmNcompoundString,  a_compound_string,
                                    NULL);
}


/*******************************************************************************
 *
 * ExerciseExmStringTransferWidgets:
 *       This function instantiates two ExmStringTransfer widgets.
 *
 ******************************************************************************/
void
ExerciseExmStringTransferWidgets(Widget parent_handle)
{
 XmString     transfer_cstring_1, transfer_cstring_2;
 Widget       StringTransfer1, StringTransfer2;

  transfer_cstring_1 = XmStringCreateLocalized("ExmStringTransfer 1");
  transfer_cstring_2 = XmStringCreateLocalized("ExmStringTransfer 2");

  StringTransfer1 = XtVaCreateManagedWidget("StringTransfer1",
                             exmStringTransferWidgetClass, parent_handle,
                             XmNstringDirection,  XmSTRING_DIRECTION_L_TO_R,
                             ExmNcompoundString,  transfer_cstring_1,
                             NULL);
  XtAddCallback(StringTransfer1, XmNhelpCallback,
                HelpCB, (XtPointer)StringTransferH);

  StringTransfer2 = XtVaCreateManagedWidget("StringTransfer2",
                             exmStringTransferWidgetClass, parent_handle,
                             XmNalignment,        XmALIGNMENT_END,
                             ExmNcompoundString,  transfer_cstring_2,
                             NULL);
  XtAddCallback(StringTransfer2, XmNhelpCallback,
                HelpCB, (XtPointer)StringTransferH);
}


/*******************************************************************************
 *
 * ExerciseExmTabButtonWidget:
 *       This function instantiates one ExmTabButton widget and one
 *       XmNotebook function.  The ExmTabButton will serve as the tab
 *       of the XmNotebook.
 *
 ******************************************************************************/
void
ExerciseExmTabButtonWidget(Widget parent_handle)
{
 Widget  Notebook1, TabButton1;
 XmString  tab_button_string;

/* Instantiate an XmNotebook widget. */
  Notebook1 = XtVaCreateManagedWidget("Notebook1",
                                    xmNotebookWidgetClass, parent_handle,
                                    NULL);

/* Instantiate an ExmTabButton widget hanging off the XmNotebook. */
  tab_button_string = XmStringCreateLocalized("Tab But");
  TabButton1 = XtVaCreateManagedWidget("TabButton1",
                                    exmTabButtonWidgetClass, Notebook1,
                                    ExmNcompoundString, tab_button_string,
                                    ExmNopenSide, XmLEFT,
                                    NULL);
  XtAddCallback(TabButton1, XmNhelpCallback, HelpCB, (XtPointer)TabButtonH);
}


/*******************************************************************************
 *
 * ExerciseExmCommandButtonWidgets:
 *       This function instantiates two ExmCommandButton widgets.
 *
 ******************************************************************************/
void
ExerciseExmCommandButtonWidgets(Widget parent_handle)
{
 XmString  really_quit, yes, no, dance, help;
 Widget    String1, TemplateDialog;
 Widget    CommandButton1, CommandButton2, CommandButton3, CommandButton4;
 Arg       arg[3];

  really_quit = XmStringLtoRCreate("Do you really want to quit?\0",
                                             XmFONTLIST_DEFAULT_TAG);

  yes = XmStringLtoRCreate("Yes\0", XmFONTLIST_DEFAULT_TAG);
  no  = XmStringLtoRCreate("No\0", XmFONTLIST_DEFAULT_TAG);
  dance = XmStringLtoRCreate("No, I want to\ndance instead.\0",
                                             XmFONTLIST_DEFAULT_TAG);
  help = XmStringLtoRCreate("Help\0", XmFONTLIST_DEFAULT_TAG);


 /* Create a MessageBox to which we can attach ExmCommandButton's. */
  TemplateDialog = XmCreateTemplateDialog(parent_handle, "QuitNow?", NULL, 0);

  String1 = XtVaCreateManagedWidget("String1",
                                    exmStringWidgetClass,
                                    TemplateDialog,
				    XmNshadowThickness, 0,
                                    ExmNcompoundString,  really_quit,
                                    NULL);

  CommandButton1 = XtVaCreateManagedWidget("CommandButton1",
                                    exmCommandButtonWidgetClass,
                                    TemplateDialog,
                                    ExmNcompoundString,  yes,
                                    NULL);
  XtAddCallback(CommandButton1, XmNactivateCallback, YesCB, NULL);
  XtAddCallback(CommandButton1, XmNhelpCallback, HelpCB, (XtPointer)CommandButtonH);
 /* Make CommandButton1 the initial default button of the MessageBox. */
  XtVaSetValues(TemplateDialog,
                XmNdefaultButton, CommandButton1,
                NULL);

  CommandButton2 = XtVaCreateManagedWidget("CommandButton2",
                                    exmCommandButtonWidgetClass,
                                    TemplateDialog,
                                    ExmNcompoundString,  no,
                                    NULL);
  XtAddCallback(CommandButton2, XmNactivateCallback, NoCB, NULL);
  XtAddCallback(CommandButton2, XmNhelpCallback, HelpCB, (XtPointer)CommandButtonH);

  CommandButton3 = XtVaCreateManagedWidget("CommandButton3",
                                    exmCommandButtonWidgetClass,
                                    TemplateDialog,
                                    XmNalignment,        XmALIGNMENT_BEGINNING,
                                    ExmNcompoundString,  dance,
                                    NULL);
  XtAddCallback(CommandButton3, XmNactivateCallback, DanceCB, NULL);
  XtAddCallback(CommandButton3, XmNhelpCallback, HelpCB, (XtPointer)CommandButtonH);

  CommandButton4 = XtVaCreateManagedWidget("CommandButton4",
                                    exmCommandButtonWidgetClass,
                                    TemplateDialog,
                                    ExmNcompoundString,  help,
                                    NULL);
  XtAddCallback(CommandButton4, XmNactivateCallback, HelpCB, (XtPointer)CommandButtonH);
  XtAddCallback(CommandButton4, XmNhelpCallback, HelpCB, (XtPointer)CommandButtonH);

  XtManageChild(TemplateDialog);
}


/*******************************************************************************
 *
 * QuitCB:
 *     Callback that causes application to exit.
 *
 ******************************************************************************/
void
QuitCB(Widget w,
       XtPointer cd,
       XtPointer cb)
{
  ExerciseExmCommandButtonWidgets(top_level);
}



/*******************************************************************************
 *
 * HelpCB:
 *     Callback that causes application to exit.
 *
 ******************************************************************************/
void
HelpCB(Widget w,
       XtPointer cd,
       XtPointer cb)
{
 intptr_t       what_kind_of_help = (intptr_t)cd;
static char      *messages[] = {
"ExmSimple displays one oval or rectangle.\n",
"ExmString displays one compound string.\n",
"ExmStringTransfer demonstrates Drag and Drop.\
 To transfer text between two ExmStringTransfer widgets:\n\
   1. Place the cursor anywhere inside the widget, \n\
   2. Depress M2.\n\
   3. Move the cursor to another ExmStringTransferWidget.\n\
   4. Release M2.",
"ExmCommandButton displays a compound string.  \n\
This widget can serve as a button inside a dialog box.",
"ExmTabButton displays a compound string.  It can affix itself\n\
to one side of a suitable host widget (like an XmNotebook).",
"ExmPanner is a two-dimensional navigator widget.\n\
To navigate with the ExmPanner,\n\
  1. Place the cursor over the rectangular slider.\n\
  2. Depress M1.\n\
  3. Move the cursor around.  As you do, the viewport will change.\n\
  4. Release M1 to stop navigating.\n\
Note that there are additional ways to navigate.",
"ExmMenuButton displays a compound string.  This widget can serve\n\
as a button inside a menu.",
"ExmGrid is a sample Manager widget.  In this application, ExmGrid\n\
 manages two ExmSimple widgets, two ExmString widgets, two\n\
 ExmStringTransfer widgets, and one XmNotebook widget."
                         };
 XmString  message_as_a_cs;
 Widget    help_dialog;
 Arg       arg[3];

   message_as_a_cs = XmStringLtoRCreate(messages[(intptr_t)cd],
                                        XmFONTLIST_DEFAULT_TAG);

   XtSetArg(arg[0], XmNmessageString, message_as_a_cs);
   help_dialog = XmCreateMessageDialog(top_level, "message", arg, 1);
   XmStringFree(message_as_a_cs);

   XtManageChild(help_dialog);
}



/*******************************************************************************
 *
 * YesCB:
 *     Callback associated with CommandButton1.
 *
 ******************************************************************************/
void
YesCB(Widget w,
	   XtPointer cd,
	   XtPointer cb)
{
  exit(0);
}


/*******************************************************************************
 *
 * NoCB:
 *     Callback associated with CommandButton2.
 *
 ******************************************************************************/
void
NoCB(Widget w,
	   XtPointer cd,
	   XtPointer cb)
{
  /* no op */
}


/*******************************************************************************
 *
 * DanceCB:
 *     Callback associated with CommandButton3.
 *
 ******************************************************************************/
void
DanceCB(Widget w,
	    XtPointer cd,
	    XtPointer cb)
{
 XmString  everybody_dance_now;
 Arg       arg[2];
 Widget    MsgBox1;

 everybody_dance_now = XmStringCreateLocalized("Everybody Dance Now!");
 XtSetArg(arg[0], XmNmessageString, everybody_dance_now);

 MsgBox1 = XmCreateMessageDialog(w, "MsgBox1",  arg, 1);
 XmStringFree(everybody_dance_now);

 XtManageChild(MsgBox1);
}

