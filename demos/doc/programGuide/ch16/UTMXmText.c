/* $XConsortium: UTMXmText.c /main/4 1995/07/15 20:40:10 drk $ */
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

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/CascadeB.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Transfer.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

void MakeTextWidgets(Widget);
void ConvertCallback(Widget, XtPointer, XtPointer);
void DestinationCallback(Widget, XtPointer, XtPointer);
void TransferProc(Widget, XtPointer, XtPointer);
void CreateMenus(Widget);
void HelpCB(Widget, XtPointer, XtPointer);
void QuitCB(Widget, XtPointer, XtPointer);
void OutputAnAtomName(Widget, Atom);
void ListAllTheTargets(Widget, Atom *, unsigned long);

Widget toplevel;
XtAppContext app_context;

/******************************************************************
main:
******************************************************************/
int
main(int argc, char **argv)
{
 static Widget  MainWindow;
 Widget         Frame1;

    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);

    toplevel = XtOpenApplication(&app_context, "Test", NULL, 0,
                                 &argc, argv, NULL,sessionShellWidgetClass,
                                 NULL, 0);

   MainWindow = XtVaCreateManagedWidget("MainWindow1",
                                    xmMainWindowWidgetClass, toplevel,
                                        NULL);

   CreateMenus(MainWindow);

   Frame1 = XtVaCreateManagedWidget("Frame1",
                                    xmFrameWidgetClass, MainWindow,
                                    NULL);
   MakeTextWidgets(Frame1);

   XtRealizeWidget(toplevel);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}


/******************************************************************
MakeTextWidgets: Instantiate two text widgets both managed by the
same RowColumn widget.
******************************************************************/
void
MakeTextWidgets(Widget   parent)
{
 Widget        RC1;
 Widget        Text1, Text2;

   RC1 = XtVaCreateManagedWidget("RC1", xmRowColumnWidgetClass, parent,
                                    NULL);

   Text1 = XtVaCreateManagedWidget("Text1", xmTextWidgetClass, RC1,
              XmNeditMode,  XmMULTI_LINE_EDIT,
              XmNrows,      10,
              XmNcolumns,   36,
              XmNvalue,     "DESTINATION: ONLY UPPERCASE HERE.",
                                    NULL);
 /* Attach an XmNdestinationCallback procedure to Text1. */
   XtAddCallback(Text1, XmNdestinationCallback, DestinationCallback, NULL);


   Text2 = XtVaCreateManagedWidget("Text2", xmTextWidgetClass, RC1,
              XmNeditMode,  XmMULTI_LINE_EDIT,
              XmNrows,      10,
              XmNcolumns,   36,
              XmNvalue,     "Source: Responsible for converting.",
                                    NULL);
 /* Attach an XmNconvertCallback procedure to Text2. */
   XtAddCallback(Text2, XmNconvertCallback, ConvertCallback, NULL);
}


/**********************************************************************
ConvertCallback
**********************************************************************/
void
ConvertCallback(Widget  w,
                     XtPointer ignore,
                     XtPointer call_data)
{
 XmConvertCallbackStruct  *ccs = (XmConvertCallbackStruct *)call_data;
 char    *selected_text;
 char    *copy_of_selected_text;
 Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
 Atom _MOTIF_CLIPBOARD_TARGETS = XInternAtom(XtDisplay(w),
                              "_MOTIF_CLIPBOARD_TARGETS", False);
 Atom MYTEXT = XInternAtom(XtDisplay(w), "MYTEXT", False);
 int   n=0;
 Atom *targs = (Atom *)XtMalloc(sizeof(Atom) * 2);

  printf("\n\nNow in ConvertCallback.\n");
  printf("\tSelection: ");
  OutputAnAtomName((Widget)w, ccs->selection);
  printf("\tTarget: ");
  OutputAnAtomName((Widget)w, ccs->target);

  if ((ccs->target == TARGETS) ||
     (ccs->target == _MOTIF_CLIPBOARD_TARGETS)) {

    printf("ConvertCallback: Adding MYTEXT to the list of targets.\n");
  /* use targs to hold a list of targets that my application can
     convert. */
    targs[n] = MYTEXT; n++;
    ccs->value = (XtPointer) targs;
    ccs->type = XA_ATOM;
    ccs->format = 32;
    ccs->length = n;
    ccs->status = XmCONVERT_MERGE;
  }
  else if (ccs->target == MYTEXT)  {
  /* Get the selection. */
    selected_text = XmTextGetSelection(w);
    copy_of_selected_text = selected_text;

  /* Convert any lowercase letters in the selection to uppercase. */
    while (*selected_text)  {
       if (islower(*selected_text))
         *selected_text = toupper(*selected_text);
         selected_text++;
    }

  /* Place the converted text into the XmConvertCallbackStruct. */
    ccs->value = copy_of_selected_text;
    ccs->type = ccs->target;
    ccs->format = 8;
    ccs->length = strlen(copy_of_selected_text);
    ccs->status = XmCONVERT_DONE;
  }
}



/**********************************************************************
DestinationCallback:
**********************************************************************/
void
DestinationCallback(Widget  w,
                        XtPointer ignore,
                        XtPointer call_data)
{
 XmDestinationCallbackStruct *dcs =
           (XmDestinationCallbackStruct *)call_data;
 Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
 Atom MYTEXT = XInternAtom(XtDisplay(w), "MYTEXT", False);

 printf("\n\nNow in DestinationCallback\n");
 printf("\tSelection: ");
 OutputAnAtomName ((Widget)w, dcs->selection);

 /* Ask the source to return a list of all the targets supported. */
 XmTransferValue(dcs->transfer_id, TARGETS, (XtCallbackProc)TransferProc,
                  NULL, XtLastTimestampProcessed(XtDisplay(w)));
}


/**********************************************************************
TransferProc: Called by UTM whenever a conversion routine completes
a conversion that was initiated by an XmTransferValue call.
**********************************************************************/
void
TransferProc(Widget  w,
             XtPointer ignore,
             XtPointer call_data)
{
 XmSelectionCallbackStruct *scs =
       (XmSelectionCallbackStruct *) call_data;
 Atom TARGETS = XInternAtom(XtDisplay(w), "TARGETS", False);
 Atom MYTEXT = XInternAtom(XtDisplay(w), "MYTEXT", False);
 Atom  *targets = (Atom *)scs->value;
 int    MYTEXT_is_supported = 0;
 unsigned long    n;

   printf("\n\nNow in TransferProc.\n");

   if ((scs->target == TARGETS) && (scs->type == XA_ATOM))  {
     printf("TransferProc: target is TARGETS.\n");
     printf("Number of supported targets is %ld\n", scs->length);
     ListAllTheTargets(w, targets, scs->length);
     for (n=0; n<=scs->length; n++)  {
  /* Look through list of returned TARGETS to see if MYTEXT is there. */
        if (targets[n] == MYTEXT)
          MYTEXT_is_supported = 1;
     }

     if (MYTEXT_is_supported)
       printf("TransferProc: Requesting conversion of MYTEXT.\n");
       XmTransferValue(scs->transfer_id, MYTEXT,
                   (XtCallbackProc)TransferProc, NULL,
                   XtLastTimestampProcessed(XtDisplay(w)));
   }

  if (scs->target == MYTEXT) {
    XmTextPosition current_insertion_position;

    printf("TransferProc: source has converted MYTEXT.\n");
    current_insertion_position = XmTextGetInsertionPosition(w);
    XmTextInsert(w, current_insertion_position, (char *)scs->value);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_SUCCEED);
  }
}


/**********************************************************************
OutputAnAtomName
**********************************************************************/
void
OutputAnAtomName(Widget w,
            Atom target)
{
 char  *AtomName = malloc(sizeof(char *) * 34);

  AtomName = XGetAtomName(XtDisplay(w), target);
  printf("\t%s\n", AtomName);
}


/**********************************************************************
ListAllTheTargets
**********************************************************************/
void
ListAllTheTargets(Widget w,
                  Atom *targets,
                  unsigned long number_of_targets)
{
 int n;

  printf("Here are all the targets:\n");
  for (n=0; n<number_of_targets; n++)
     OutputAnAtomName(w, targets[n]);
}


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
HelpCB
*********************************************************************/
void
HelpCB(Widget   w,
       XtPointer cd,
       XtPointer cb
      )
{
 char      help_string[500];
 XmString  hs_as_cs;
 Widget    dialog_general_help;
 Arg       arg[3];

 sprintf(help_string,
"This program demonstrates how to add a convert callback\n\
and a destination callback to an application.\n\
You should try to transfer text from the 'Source' Text widget to\n\
the 'DESTINATION' text widget.  The text that you transfer\n\
will be converted to all uppercase when it arrives in the\n\
destination widget.  You can transfer the text via drag and drop,\n\
clipboard, secondary, or primary selections.");

   hs_as_cs = XmStringLtoRCreate(help_string,
                                 XmFONTLIST_DEFAULT_TAG);

   XtSetArg(arg[0], XmNmessageString, hs_as_cs);
   dialog_general_help = (Widget)XmCreateMessageDialog(toplevel,
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

