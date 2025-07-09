/* $XConsortium: simple_drop.c /main/4 1995/07/15 20:40:19 drk $ */
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
/******************************************************************
This program demonstrates how to add support to an XmDrawingArea
widget so that a pixmap can be dropped into it 
******************************************************************/

#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/CascadeB.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/Transfer.h>
#include <Xm/TransferP.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

void CreateMenus(Widget);
int  Play(Widget);
void HelpCB(Widget, XtPointer, XtPointer);
void QuitCB(Widget, XtPointer, XtPointer);
void DestinationCallback(Widget, XtPointer, XtPointer);
void TransferProc(Widget, XtPointer, XtPointer);
void OutputAnAtomName(Widget, Atom); 
void ListAllTheTargets(Widget, Atom *, unsigned long);

Widget  toplevel;


/******************************************************************
main: 
******************************************************************/
int 
main(int argc, char **argv)
{
    static Widget  MainWindow; 
    XtAppContext   app_context;
    Widget         Frame1, RC1, Label1, DrawingArea1;
    Pixmap         pixmap;
    GC             gc;

    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 

 
    toplevel = XtOpenApplication(&app_context, "Test", NULL, 0,
                                &argc, argv, NULL, 
                                sessionShellWidgetClass,
                                NULL, 0);

    MainWindow = XtVaCreateManagedWidget("MainWindow1", 
                                    xmMainWindowWidgetClass, toplevel,
                                        NULL);
                     
    CreateMenus(MainWindow);

    RC1 = XtVaCreateManagedWidget("RC1", xmRowColumnWidgetClass, 
                                 MainWindow, NULL);

    if (!argv[1]) {
        printf("usage: %s bitmap-file\n", *argv);
        exit(1);
    }

   /* Load bitmap given in argv[1] */
    pixmap = XmGetPixmap(XtScreen(toplevel), argv[1],
        BlackPixelOfScreen(XtScreen(toplevel)),
        WhitePixelOfScreen(XtScreen(toplevel)));

    if (pixmap == XmUNSPECIFIED_PIXMAP) {
        printf("can't create pixmap from %s\n", argv[1]);
        exit(1);
    }

  /* Now instantiate an XmLabel widget that displays pixmap. */ 
    Label1 = XtVaCreateManagedWidget("Label1", 
        xmLabelWidgetClass, RC1,
        XmNlabelType,   XmPIXMAP,
        XmNlabelPixmap, pixmap,
        NULL);

    Frame1 = XtVaCreateManagedWidget("Frame1",
        xmFrameWidgetClass, RC1,
        XmNshadowThickness, 3,        
        NULL); 

    DrawingArea1 = XtVaCreateManagedWidget("DrawingArea1", 
        xmDrawingAreaWidgetClass, Frame1,
        XmNwidth, 150,
        XmNheight, 150,
        XmNresizePolicy, XmRESIZE_NONE,
        NULL);
    XmeDropSink(DrawingArea1, NULL, 0);
    XtAddCallback(DrawingArea1, XmNdestinationCallback, 
                  DestinationCallback, NULL);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return 0;    /* make compiler happy */
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
 Atom _MOTIF_EXPORT_TARGETS = XInternAtom(XtDisplay(w), 
                             "_MOTIF_EXPORT_TARGETS", False);

  printf("\n\nNow in Destination Callback.\n");
  printf("\tSelection: ");
  OutputAnAtomName ((Widget)w, dcs->selection);

 /* Ask the source to return a list of all the export targets that
    it knows how to convert. */
 XmTransferValue(dcs->transfer_id, _MOTIF_EXPORT_TARGETS, 
                 (XtCallbackProc)TransferProc, NULL,
                 XtLastTimestampProcessed(XtDisplay(w)));
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
 Atom _MOTIF_EXPORT_TARGETS = XInternAtom(XtDisplay(w), 
                             "_MOTIF_EXPORT_TARGETS", False);
 Atom PIXMAP = XInternAtom(XtDisplay(w), "PIXMAP", False);
 Atom  *targets = (Atom *)scs->value;
 int    PIXMAP_is_supported = 0;
 unsigned long    n;
 Widget Label2;

   printf("\n\nNow in TransferProc.\n"); 
   printf("The source has converted: ");
   OutputAnAtomName(w, scs->target);

   if ((scs->target == _MOTIF_EXPORT_TARGETS) && (scs->type == XA_ATOM))  {
     printf("TransferProc: target is TARGETS.\n");
     printf("Number of supported targets is %ld\n", scs->length);
     ListAllTheTargets(w, targets, scs->length);
     for (n=0; n<=scs->length; n++)  {
  /* Look through list of returned TARGETS to see if PIXMAP is there. */
        if (targets[n] == PIXMAP)  {
          printf("The source knows how to convert PIXMAP.\n");
          PIXMAP_is_supported = 1; 
        }
     }

     if (PIXMAP_is_supported)  {
       printf("TransferProc: Asking for PIXMAP.\n");
       XmTransferValue(scs->transfer_id, PIXMAP,
                   (XtCallbackProc)TransferProc, NULL,
                   XtLastTimestampProcessed(XtDisplay(w)));
     }
   }

  if ((scs->target == PIXMAP)) { 
    Pixmap        transferred_pixmap = *(Pixmap*) scs->value;
    Pixmap        copy_of_transferred_pixmap;
    Window        root_return;
    int           x, y;
    unsigned int  width, height;
    unsigned int  border_width, depth;
    GC            gc;
    XtGCMask      valueMask;
    XGCValues     values;

    printf("TransferProc: source has transferred the PIXMAP.\n");
    printf("We need to paste it into the DrawingArea.\n");

  /* It is better to display a copy of the returned pixmap than
     to display the returned pixmap itself.  The following code
     creates the copy. */
    XGetGeometry(XtDisplay(w), (Drawable)transferred_pixmap, 
                 &root_return, &x, &y, &width, &height, 
                 &border_width, &depth);
    copy_of_transferred_pixmap = 
                 XCreatePixmap(XtDisplay(w), XtWindow(w), width, 
                               height, depth);
    valueMask = GCFunction;
    values.function = GXcopy;
    
    gc = XtGetGC(w, valueMask, &values);
    XCopyArea(XtDisplay(w), transferred_pixmap, 
              copy_of_transferred_pixmap, gc, x, y, 
              width, height, x, y);
    
    Label2 = XtVaCreateManagedWidget("Label2", xmLabelWidgetClass, w,
        XmNlabelType,   XmPIXMAP,
        XmNlabelPixmap, copy_of_transferred_pixmap, 
        NULL);
    XtReleaseGC(w, gc);
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
   file = XmStringCreateSimple("File");
   help = XmStringCreateSimple("Help");
   
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
 char      help_string[400]; 
 XmString  hs_as_cs; 
 Widget    dialog_general_help; 
 Arg       arg[3];

 sprintf(help_string, 
"This program demonstrates how to add a destination callback\n\
to an application.\n\
You should drag the displayed pixmap from the Label widget\n\
and drop it inside the framed DrawingArea widget.");

   hs_as_cs = XmStringCreateLtoR(help_string, 
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
  exit(1);
}
