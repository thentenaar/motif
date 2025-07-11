/* $XConsortium: DiaShellM2.c /main/4 1995/07/15 21:05:58 drk $ */
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


#include <testlib.h>

/* callback procedures */
static void  quitCB();


Widget	BulletinBoard1,
	PushButton1,
	PushButton2;
Widget  Scale;

Boolean		  modal_bug = False;		/*PIR3421*/


										/*Begin PIR2753*/

static void PostModelessDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    Widget        parent, Dialog1, spawnButton;
    int n;
    Arg args[10];
    XmString tcs;
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple ("While this box is up, you should be able to input anywhere else")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNhelpLabelString, XmStringCreateSimple ("Spawn Dialog Child")); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Modeless Dialog"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS);
              n++;
    Dialog1 = XmCreateMessageDialog(parent, "Dialog1", args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostModelessDialog, Dialog1);
    if (delay) sleep (delay);
    XtManageChild (Dialog1);

}
										/*End PIR2753*/
static void PostPrimaryModalDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    Widget        parent, Dialog1, spawnButton;
    int n;
    Arg args[10];
    XmString tcs;
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
XmStringCreateSimple ("While this box is up, you should not be able to use its parent")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNhelpLabelString, XmStringCreateSimple ("Spawn Dialog Child")); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Primary Application Modal Dialog"); n++;
/*    XtSetArg (args[n], XmNmwmInpuMode, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++; */
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++;
    Dialog1 = XmCreateMessageDialog(parent, "Dialog1", args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, PostPrimaryModalDialog, Dialog1);
    if (delay) sleep (delay);
    XtManageChild (Dialog1);

}

static void PostFullAppModalDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

    Widget        parent, Dialog2, spawnButton;
    int n;
    Arg args[10];
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
XmStringCreateSimple ("While this box is up, you should not be able to use either Shell1 or Shell2 nor any other dialog box.")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
	XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Full Application Modal Dialog"); n++;
/*    XtSetArg (args[n], XmNmwmInpuMode, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++; */
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
              n++;
    Dialog2 = XmCreateMessageDialog(parent, "Dialog1", args, n);
/* make HELP button spawn a child */
	spawnButton = XmMessageBoxGetChild (Dialog2, XmDIALOG_HELP_BUTTON);
	XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostFullAppModalDialog, Dialog2);

    if (delay) sleep (delay);
    XtManageChild (Dialog2);

}

static void PostSystemModalDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

    Widget        parent, Dialog3, spawnButton;
    int n;
    Arg args[10];
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple ("While this box is up, you should not be able to use any other application nor any other part of this client.")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
	XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;

    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "System Modal Dialog"); n++;
/*    XtSetArg (args[n], XmNmwmInputMode, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++; */
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL);
              n++;
    Dialog3 = XmCreateMessageDialog(parent, "Dialog1", args, n);
/* make HELP button spawn a child */
	spawnButton = XmMessageBoxGetChild (Dialog3, XmDIALOG_HELP_BUTTON);
	XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostSystemModalDialog, Dialog3);
    if (delay) sleep (delay);
    XtManageChild (Dialog3);

}

										/*Begin PIR1953*/

static void DestroyWorkingDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    XtDestroyWidget((Widget) client_data);
}


static void PostWorkingDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    Widget        parent, DialogW, destroyButton;
    int n;
    Arg args[10];
    XmString tcs;
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple ("This is Working Dialog")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNhelpLabelString, XmStringCreateSimple ("Destroy Working Dialog")); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Working Dialog"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS);
              n++;
    DialogW = XmCreateWorkingDialog(parent, "DialogW", args, n);

/* make HELP button spawn a child */
    destroyButton = XmMessageBoxGetChild (DialogW, XmDIALOG_HELP_BUTTON);
    XtAddCallback (destroyButton, XmNactivateCallback, 
				   DestroyWorkingDialog, DialogW);
    if (delay) sleep (delay);
    XtManageChild (DialogW);

}
										/*End PIR1953*/

										/*Begin PIR2145*/

static void DestroyPopupShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    XtDestroyWidget((Widget) client_data);
}

static void PostPopupShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

    Widget        parent, Shell3, BB3, DB3;
    int n;
    Arg args[10];
    int delay;

    parent = (Widget) client_data;
    /* get the posting delay from the Scale*/
    n = 0;
    XtSetArg (args[n], XmNvalue, &delay); n++;
    XtGetValues (Scale, args, n);

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple ("Testing pop up shell and destroy widget.")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Popup Shell"); n++;

    Shell3 = XtCreatePopupShell("Shell 3", topLevelShellWidgetClass, parent,
                              args, n);
    n = 0;
    BB3 = XmCreateBulletinBoard (Shell3, "BB3", args, n);    
    XtManageChild (BB3);

    n = 0;
    XtSetArg (args[n], XmNx, 200); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    DB3 = XmCreatePushButtonGadget (BB3, "Destroy", args, n);
    XtManageChild (DB3);
    XtAddCallback (DB3, XmNactivateCallback, DestroyPopupShell, Shell3);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, DB3); n++;
    XtSetValues (BB3, args, n);

    if (delay) sleep (delay);
    XtPopup(Shell3, XtGrabNone);
}
										/*End PIR2145*/


static void  quitCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}


											/*Begin PIR3421*/
static void destroyDialogCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

	XtDestroyWidget((Widget) client_data);

}
											/*End PIR3421*/


void  main(argc, argv)
    int     argc;
    char  **argv;
{

    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Widget        Shell2;
    Widget        BB1, BB2;
    Widget        DB, DB2, exitButton;
    Widget        test1, test1a, test2, test3;
	Widget        test1b;               /*PIR2753*/
	Widget        test1c;               /*PIR2145*/
	Widget        test2a;
	Widget        test2b;
	Widget        FullAppDialog;							/*PIR3421*/
    Widget        okButton, cancelButton, destroyButton;	/*PIR3421*/


    CommonTestInit(argc, argv);
    
    if (UserData != NULL)				/*PIR3421*/
		ProcessCommandArgs();


    n = 0;
    XtSetArg(args[n], XmNtitle, "Shell 2");    n++;
    XtSetArg(args[n], XtNgeometry, "+500+0");  n++;
    Shell2 = XtAppCreateShell(argv[0], NULL, topLevelShellWidgetClass,
			      display, args, n);
   
    /* Shell 1 children */

    n = 0;
    BB1 = XmCreateBulletinBoard (Shell1, "BB1", args, n);
    XtManageChild (BB1);
    
    n = 0;
    XtSetArg (args[n], XmNx, 300); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Default Button")); n++;
    DB = XmCreatePushButton (BB1, "DB", args, n);	  
    XtManageChild (DB);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, DB); n++;
    XtSetValues (BB1, args, n);

    n = 0;
    XtSetArg (args[n], XmNx, 300); n++;
    XtSetArg (args[n], XmNy, 10); n++;
    exitButton = XmCreatePushButtonGadget (BB1, "Exit", args, n);
    XtManageChild (exitButton);
    XtAddCallback (exitButton, XmNactivateCallback, quitCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNx, 12); n++;
    XtSetArg(args[n], XmNy, 20); n++;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], XmNtitleString,
       XmStringCreateSimple ("Posting Delay")); n++;
    XtSetArg(args[n], XmNmaximum, 120); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    Scale = XmCreateScale (BB1, "Scale", args, n);
    XtManageChild (Scale);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Primary Modal from Shell2")); n++;
    test1 = XmCreatePushButton (BB1, "test1", args, n);	  
    XtManageChild (test1);
    XtAddCallback (test1, XmNactivateCallback, PostPrimaryModalDialog, Shell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Primary Modal from Shell1")); n++;
    test1a = XmCreatePushButton (BB1, "test1a", args, n);	  
    XtManageChild (test1a);
    XtAddCallback (test1a, XmNactivateCallback, PostPrimaryModalDialog, Shell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Full App Modal Dialog")); n++;
    test2 = XmCreatePushButton (BB1, "test2", args, n);	  
    XtManageChild (test2);
    XtAddCallback (test2,  XmNactivateCallback, PostFullAppModalDialog, Shell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 250); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create System Modal Dialog")); n++;
    test3 = XmCreatePushButton (BB1, "test3", args, n);	  
    XtManageChild (test3);
    XtAddCallback (test3, XmNactivateCallback, PostSystemModalDialog, Shell2);

										/*Begin PIR2753*/
    n = 0;									
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 300); n++;
    XtSetArg (args[n], XmNlabelString,
			  XmStringCreateSimple ("Create Modeless Dialog from Shell1")); n++;
    test1b = XmCreatePushButton (BB1, "test1b", args, n);	  
    XtManageChild (test1b);
    XtAddCallback (test1b, XmNactivateCallback, PostModelessDialog, Shell1);
										/*End PIR2753*/

										/*Begin PIR2145*/
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 350); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Popup Shell from Shell2")); n++;
    test1c = XmCreatePushButton (BB1, "test1c", args, n);	  
    XtManageChild (test1c);
    XtAddCallback (test1c, XmNactivateCallback, PostPopupShell, Shell2);
										/*End PIR2145*/

    /* Shell 2 children */

    n = 0;
    BB2 = XmCreateBulletinBoard (Shell2, "BB2", args, n);    
    XtManageChild (BB2);

    n = 0;
    XtSetArg (args[n], XmNx, 200); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    DB2 = XmCreatePushButtonGadget (BB2, "DB2", args, n);
    XtManageChild (DB2);

	n = 0;
	XtSetArg (args[n], XmNx, 10); n++;
	XtSetArg (args[n], XmNy, 100); n++;
	XtSetArg (args[n], XmNlabelString,
              XmStringCreateSimple ("Create Modeless Dialog from Shell2")); n++;
	test2a = XmCreatePushButton (BB2, "test2a", args, n);
	XtManageChild (test2a);
    XtAddCallback (test2a, XmNactivateCallback, PostModelessDialog, Shell2);

	n = 0;
	XtSetArg (args[n], XmNx, 10); n++;
	XtSetArg (args[n], XmNy, 150); n++;
	XtSetArg (args[n], XmNlabelString,
              XmStringCreateSimple ("Create Working Dialog from Shell2")); n++;
	test2b = XmCreatePushButton (BB2, "test2b", args, n);
	XtManageChild (test2b);
    XtAddCallback (test2b, XmNactivateCallback, PostWorkingDialog, Shell2);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, DB2); n++;
    XtSetValues (BB2, args, n);

											/*Begin PIR3421*/
	if (modal_bug == True) {

    	n = 0;
    	XtSetArg (args[n], XmNmessageString, 
				  XmStringCreateSimple ("This is a FullApp Modal dialog")); 
				  n++;

    	XtSetArg (args[n], XmNtitle, "FullApp Modal Dialog"); n++;
    	XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); 
				  n++;
		XtSetArg (args[n], XmNhelpLabelString, 
			  	  XmStringCreateSimple ("Destroy Me")); n++;
    	FullAppDialog = XmCreateMessageDialog(Shell1, "FullAppDialog", args, n);

    	cancelButton = XmMessageBoxGetChild (FullAppDialog, 
											 XmDIALOG_CANCEL_BUTTON);
		XtUnmanageChild(cancelButton);
    	okButton = XmMessageBoxGetChild (FullAppDialog, 
										 XmDIALOG_OK_BUTTON);
		XtUnmanageChild(okButton);

    	destroyButton = XmMessageBoxGetChild (FullAppDialog, 
											  XmDIALOG_HELP_BUTTON);
    	XtAddCallback (destroyButton, XmNactivateCallback, destroyDialogCB, 
				   	   FullAppDialog);

    	XtManageChild (FullAppDialog);

	}
											/*End PIR3421*/
    
    XtRealizeWidget(Shell1);
    XtRealizeWidget(Shell2);

	CommonPause();

    XtAppMainLoop(app_context);

}


											/*Begin PIR3421*/
#define	WORD_LEN	32

ProcessCommandArgs()
{

	char	*user_data;
	char	next_word[WORD_LEN + 1];

	user_data = UserData;

	while (*user_data != '\0') {

		get_next_word(user_data, next_word);
		user_data += strlen(next_word);

		if (strcmp(next_word, "-modalbug") == 0) {
			modal_bug = True;
			continue;
		}

	}

	free(UserData);

}


get_next_word(source, dest)
char	*source;
char	*dest;
{

	int	n;

	while (isspace(*source))
		source++;

	n = 0;
	while (!isspace(*source) && *source != '\0' && n < WORD_LEN) {
		*dest++ = *source++;
		n++;
	}
	*dest = '\0';

}
											/*End PIR3421*/
