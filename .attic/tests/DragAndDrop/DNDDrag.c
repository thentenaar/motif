/* $XConsortium: DNDDrag.c /main/5 1995/07/15 21:00:05 drk $ */
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
#include <testlib.h>



void
StartDrag (Widget w, XtPointer client_data, XtPointer call_data)
{
   XmDragStartCallbackStruct *DragStruct = (XmDragStartCallbackStruct *)call_data
;
   if (strcmp (UserData,"1") == 0)
     {
       DragStruct->doit = False;
       printf ("Sorry, can't drag!\n");
     }
   else if (strcmp (UserData,"2") == 0)
       DragStruct->doit = True;

}


void  
main (int argc, char **argv)
{

    int n;
    Arg args[MAX_ARGS];
    XmString tcs;
    Widget Frame1;
    Widget Label1;
    Widget xmDisplay;


    CommonTestInit(argc, argv);

    if (UserData == NULL)
      UserData = "1";

    xmDisplay = XmGetXmDisplay (display);

    XtAddCallback (xmDisplay,XmNdragStartCallback,StartDrag,(XtPointer)NULL);
    
    n = 0;
    Frame1 = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame1);


    
    n = 0;
    tcs = XmStringCreate("Non-draggable label", XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
    Label1 = XmCreateLabel(Frame1, "Label1", args, n);
    XtManageChild(Label1);

    XmStringFree(tcs);

    XtRealizeWidget (Shell1);

    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);

  }


