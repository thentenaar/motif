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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Grab1.c /main/8 1995/07/13 19:05:13 drk $"
#endif
#endif

#include <testlib.h>
#include <X11/StringDefs.h>
#include <X11/Vendor.h>
#include <Xm/RowColumnP.h>

Widget rc0, rc1, rc2;
Widget frame1, frame2;
Widget arrow1, arrow2, arrow3, arrow5, arrow7, arrow9;
Widget arrow11;
Widget toggle1, toggle2;

XtActionProc Quit();
XtActionProc GotKey();
XtActionProc BGotKey();

XtActionsRec actions[] = {
   {"Quit", (XtActionProc)Quit},
   {"GotKey", (XtActionProc)GotKey},
   {"BGotKey", (XtActionProc)BGotKey}
};

static String translations =
   "~Ctrl ~Shift ~Meta<Key>q: Quit()\n\
    <Key>: GotKey()";

static String b_translations =
    "<Key>: BGotKey()";

XtTranslations Xlations;
XtTranslations BXlations;

/************************************************/


XtActionProc Quit ()

{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}

void PrintName (w)

   Widget w;

{
    printf ("Widget %s was activated\n", XrmQuarkToString(w->core.xrm_name));
}

void PrintName_2 (rc, client_data, d)

   Widget rc;
   XtPointer client_data;
   XtPointer d;

{
   XmRowColumnCallbackStruct * rc_data = (XmRowColumnCallbackStruct *) d;
    printf ("Widget %s was activated\n", 
             XrmQuarkToString(rc_data->widget->core.xrm_name));
}

static XtCallbackRec PrintName_CB[] = {
   {PrintName_2, NULL},
   {NULL, NULL}
};


AddAllTabGroups()
{
   /* Add in correct order */
   XmAddTabGroup(rc1);
   XmAddTabGroup(rc2);
}

XtActionProc GotKey (w, event)
   Widget w;
   XEvent * event;
{
   printf ("Widget %s received a key event\n", w->core.name);
}

XtActionProc BGotKey (w, event)
   Widget w;
   XEvent * event;
{
   printf ("Button %s received a key event\n", w->core.name);
}


/*************End of CALLBACK PROCEDURES****************/


void main (argc, argv)
unsigned int argc;
char **argv;

{
    Arg args[15];
    int n;
    XEvent event;
    KeyCode keycode;

    CommonTestInit(argc, argv);

/* Set new translations	*/
   XtAppAddActions(app_context, actions, XtNumber(actions));
   Xlations = XtParseTranslationTable(translations); 
   BXlations = XtParseTranslationTable(b_translations); 

    n = 0;
    XtSetArg (args[0], XtNallowShellResize, True);		n++;
    XtSetValues (Shell1, args, n) ;


   /****************************************************************/
   /*  Create the of the top-level frame and row-column manager    */
   /****************************************************************/

    n = 0;
    rc0 = XmCreateRowColumn(Shell1, "rc0", args, n);
    XtManageChild(rc0);


   /******************************************************/
   /*  Create the children of the row column manager     */
   /******************************************************/

    n = 0;
    XtSetArg (args[n], XmNshadowThickness, 2);		n++;
    frame1 = (Widget)XmCreateFrame(rc0, "frame1", args, n);
    XtManageChild(frame1);

    n = 0;
    rc1 = XmCreateRowColumn(frame1, "rc1", args, n);
    XtManageChild(rc1);

    n = 0;
    XtSetArg (args[n], XmNshadowThickness, 2);		n++;
    frame2 = (Widget)XmCreateFrame(rc0, "frame2", args, n);
    XtManageChild(frame2);

    n = 0;
    XtSetArg(args[n], XmNradioAlwaysOne, True);		n++;
    rc2 = XmCreateRadioBox(frame2, "rc2", args, n);
    XtManageChild(rc2);

   /****************************************************************/
   /*  Create the widget children of the second row-column manager  */
   /****************************************************************/

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   toggle1 = XmCreateToggleButtonGadget(rc2, "toggle1", args, n);
   XtManageChild(toggle1);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   toggle2 = XmCreateToggleButtonGadget(rc2, "toggle2", args, n);
   XtManageChild(toggle2);

   /****************************************************************/
   /*  Create the widget children of the first row-column manager  */
   /****************************************************************/

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow1 = XmCreatePushButtonGadget(rc1, "Button1", args, n);
   XtManageChild(arrow1);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow3 = XmCreatePushButtonGadget(rc1, "Button2", args, n);
   XtManageChild(arrow3);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow5 = XmCreatePushButtonGadget(rc1, "Button3", args, n);
   XtManageChild(arrow5);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow7 = XmCreatePushButtonGadget(rc1, "Button4", args, n);
   XtManageChild(arrow7);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow9 = XmCreatePushButtonGadget(rc1, "Button5", args, n);
   XtManageChild(arrow9);

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, 2);	n++;
   XtSetArg (args[n], XmNtraversalOn, True);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   arrow11 = XmCreatePushButton(rc1, "Button6", args, n);
   XtManageChild(arrow11);
/*
   XtOverrideTranslations(arrow11, BXlations); 
*/

   AddAllTabGroups(); 

   XtRealizeWidget (Shell1);

   XtOverrideTranslations(Shell1, Xlations); 

   /* Grab 'q' key for quiting	*/
   keycode = XKeysymToKeycode(XtDisplay(arrow11), 'q');
   XtGrabKey(Shell1, keycode, AnyModifier, False, GrabModeAsync, GrabModeAsync);

   /* Should generate a warning */
   keycode = XKeysymToKeycode(XtDisplay(Shell1), 'a');
   XtUngrabKey(arrow11, keycode, AnyModifier);

   XtGrabKey(Shell1, keycode, AnyModifier, True, GrabModeAsync, GrabModeAsync);
	CommonPause();
   XtUngrabKey(Shell1, keycode, AnyModifier);
	CommonPause();

   /* Grab same key twice */
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);

   /* Grab same key with different modifiers */
   keycode = XKeysymToKeycode(XtDisplay(arrow11), 'b');
   XtGrabKey(Shell1, keycode, AnyModifier, False, GrabModeAsync, GrabModeAsync);
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);
	XtUngrabKey(Shell1, keycode, ControlMask);

   /* Grab same key with different modifiers */
   keycode = XKeysymToKeycode(XtDisplay(arrow11), 'c');
   XtGrabKey(Shell1, keycode,AnyModifier,False, GrabModeAsync, GrabModeAsync);
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);

   /* Grab same key with different modifiers */
   keycode = XKeysymToKeycode(XtDisplay(arrow11), 'd');
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);
   XtGrabKey(Shell1, keycode,AnyModifier,False, GrabModeAsync, GrabModeAsync);
   XtUngrabKey(Shell1, keycode, ShiftMask);
   XtGrabKey(Shell1, keycode, ShiftMask, False, GrabModeAsync, GrabModeAsync);
   XtUngrabKey(Shell1, keycode, ControlMask);


   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();

   XtAppMainLoop(app_context);
}
