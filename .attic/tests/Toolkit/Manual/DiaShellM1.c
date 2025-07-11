/* $XConsortium: DiaShellM1.c /main/4 1995/07/15 21:05:53 drk $ */
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
#include <X11/Shell.h>
#include <Xm/MwmUtil.h>

/* forward declarations */
static void DestroyCB();					/* CR4909 */


Widget  Scale;

Widget	FirstDialog;						/* CR4909 */
Widget	FirstShell;							/* CR4909 */

Widget  ParentShell1, ParentShell2, DialogShell1;
int     count = 0;

											/* Begin CR4909 */
char	*first_msg =
	"This is here to show CR 4909. This is a Primary Modal.\nWhen this window is up, TopShell2 should accept input.\n Otherwise, the bug is not fixed. Click on Cancel to destroy this.";
char	*second_msg = 
	"This is here just to show that CR 4909 is a problem only the first time. Click on Cancel button.";
											/* End CR4909 */
char	*primary_msg = 
	"While this box is up, you should not be able to use its parent";
char	*full_msg = 
	"While this box is up, you should not be able to use either Shell1 \
or Shell2 nor any other dialog box.";
char	*system_msg =
	"While this box is up, you should not be able to use any other \
application nor any other part of this client.";

/* callback procedures */

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
			  XmStringCreateSimple (primary_msg)); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Primary Application Modal Dialog"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++;
    Dialog1 = XmCreateMessageDialog(parent, "Dialog1", args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostPrimaryModalDialog, Dialog1);
    if (delay) 
		sleep (delay);
    XtManageChild (Dialog1);
	if (parent == ParentShell2 && count == 0) {
		DialogShell1 = Dialog1;
		count = 1;
	}

}

static void PostFullAppModalDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

    Widget        parent, Dialog2;
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
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple(full_msg)); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Full Application Modal Dialog"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
              n++;
    Dialog2 = XmCreateMessageDialog(parent, "Dialog1", args, n);
    if (delay) 
		sleep (delay);
    XtManageChild (Dialog2);

}

static void PostSystemModalDialog (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

    Widget        parent, Dialog3;
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
			  XmStringCreateSimple (system_msg)); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "System Modal Dialog"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); n++;
    Dialog3 = XmCreateMessageDialog(parent, "Dialog1", args, n);
    if (delay) 
		sleep (delay);
    XtManageChild (Dialog3);

}

										/*BeginPIR2839*/
static void IconifyShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	Window	window;
	int		status;

	window = XtWindow((Widget)client_data);
	status = XIconifyWindow(display, window, DefaultScreen(display));

}

static void PopdownShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtPopdown((Widget)(client_data));

}

static void PopupShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtPopup((Widget)client_data, XtGrabNone);

}
										/*EndPIR2839*/

										/*BeginPIR2362*/
static void UnmanageShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtUnmanageChild((Widget)client_data);

}

static void ManageShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtManageChild((Widget)(client_data));

}

static void UnmanageDBox (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtUnmanageChild(DialogShell1);

}

static void ManageDBox (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtManageChild(DialogShell1);

}

/*EndPIR2362*/

/* CR 3622 */
static void UnrealizeShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtUnrealizeWidget ((Widget)(client_data));

}

static void RealizeShell (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtRealizeWidget ((Widget)(client_data));

}

/* End CR 3622 */		   


void  main(argc, argv)
    int     argc;
    char  **argv;
{

    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Widget        TopShell1, TopShell2;
    Widget        BulletinBoard1, BulletinBoard2;
    Widget        DefaultButton1, DefaultButton2;
    Widget        PrimaryModal1, PrimaryModal2, 
				  FullAppModal1, FullAppModal2, SystemModal1, SystemModal2;
	Widget        Iconify2, Popdown2, Popup2;        /*PIR2839*/
	Widget        Unmanage2, Manage2;  /*PIR2362*/
        Widget        Unrealize1, Realize1;  /* CR 3622 */

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNmappedWhenManaged, True);  n++;
    XtSetArg(args[n], XmNallowShellResize, True);  n++;
    XtSetArg(args[n], XmNtitle, "TopShell1"); n++;
    TopShell1 = XtCreatePopupShell("TopShell1", topLevelShellWidgetClass, 
								   Shell1, args, n);
	ParentShell1 = TopShell1;

	FirstShell = TopShell1;

    n = 0;
    XtSetArg(args[n], XmNmappedWhenManaged, True);  n++;
    XtSetArg(args[n], XmNtitle, "TopShell2");         n++;
    XtSetArg(args[n], XtNgeometry, "+500+0");       n++;
    TopShell2 = XtCreatePopupShell("TopShell2", topLevelShellWidgetClass, 
								   Shell1, args, n);
	ParentShell2 = TopShell2;
   
    /* Shell 1 children */

    n = 0;
    BulletinBoard1 = XmCreateBulletinBoard (TopShell1, "BulletinBoard1", 
											args, n);
    XtManageChild (BulletinBoard1);
    
    n = 0;
    XtSetArg (args[n], XmNx, 300); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("DefaultButton")); n++;
    DefaultButton1 = XmCreatePushButton (BulletinBoard1, "DefaultButton1", 
										 args, n);	  
    XtManageChild (DefaultButton1);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, DefaultButton1); n++;
    XtSetValues (BulletinBoard1, args, n);

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
    Scale = XmCreateScale (BulletinBoard1, "Scale", args, n);
    XtManageChild (Scale);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Primary Modal from TopShell1")); n++;
    PrimaryModal1 = XmCreatePushButton (BulletinBoard1, "PrimaryModal1", 
										args, n);	  
    XtManageChild (PrimaryModal1);
    XtAddCallback (PrimaryModal1, XmNactivateCallback, PostPrimaryModalDialog, 
				   TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Primary Modal from TopShell2")); n++;
    PrimaryModal2 = XmCreatePushButton (BulletinBoard1, "PrimaryModal2", 
										args, n);	  
    XtManageChild (PrimaryModal2);
    XtAddCallback (PrimaryModal2, XmNactivateCallback, PostPrimaryModalDialog, 
				   TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Full App Modal from TopShell1")); n++;
    FullAppModal1 = XmCreatePushButton (BulletinBoard1, "FullAppModal1", 
										args, n);	  
    XtManageChild (FullAppModal1);
    XtAddCallback (FullAppModal1,  XmNactivateCallback, PostFullAppModalDialog, 
				   TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 250); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create Full App Modal from TopShell2")); n++;
    FullAppModal2 = XmCreatePushButton (BulletinBoard1, "FullAppModal2", 
										args, n);	  
    XtManageChild (FullAppModal2);
    XtAddCallback (FullAppModal2,  XmNactivateCallback, PostFullAppModalDialog, 
				   TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 300); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create System Modal from TopShell1")); n++;
    SystemModal1 = XmCreatePushButton (BulletinBoard1, "SystemModal1", 
									   args, n);	  
    XtManageChild (SystemModal1);
    XtAddCallback (SystemModal1, XmNactivateCallback, PostSystemModalDialog, 
				   TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 350); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Create System Modal from TopShell2")); n++;
    SystemModal2 = XmCreatePushButton (BulletinBoard1, "SystemModal2", 
									   args, n);	  
    XtManageChild (SystemModal2);
    XtAddCallback (SystemModal2, XmNactivateCallback, PostSystemModalDialog, 
				   TopShell2);

										/*Begin PIR2839*/
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 400); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Iconify TopShell2 by XIconifyWindow")); n++;
    Iconify2 = XmCreatePushButton (BulletinBoard1, "Iconify2", args, n);	  
    XtManageChild (Iconify2);
    XtAddCallback (Iconify2, XmNactivateCallback, IconifyShell, TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 450); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Popdown TopShell2 by XtPopdown")); n++;
    Popdown2 = XmCreatePushButton (BulletinBoard1, "Popdown2", args, n);	  
    XtManageChild (Popdown2);
    XtAddCallback (Popdown2, XmNactivateCallback, PopdownShell, TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 500); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Popup TopShell2 by XtPopup")); n++;
    Popup2 = XmCreatePushButton (BulletinBoard1, "Popup2", args, n);	  
    XtManageChild (Popup2);
    XtAddCallback (Popup2, XmNactivateCallback, PopupShell, TopShell2);

    /*End PIR2839*/


																				/*Begin PIR2362*/
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 550); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Unmanage TopShell2 by XtUnmanage")); n++;
    Unmanage2 = XmCreatePushButton (BulletinBoard1, "Unmanage2", args, n);	  
    XtManageChild (Unmanage2);
    XtAddCallback (Unmanage2, XmNactivateCallback, UnmanageShell, TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 600); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Manage TopShell2 by XtManage")); n++;
    Manage2 = XmCreatePushButton (BulletinBoard1, "Manage2", args, n);	  
    XtManageChild (Manage2);
    XtAddCallback (Manage2, XmNactivateCallback, ManageShell, TopShell2);
										/*End PIR2362*/

 /* Begin CR 3622 */
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 650); n++;
    XtSetArg (args[n], XmNlabelString,
       XmStringCreateSimple ("Unrealize TopShell2 by XtUnrealizeWidget")); n++;
    Unrealize1 = XmCreatePushButton (BulletinBoard1, "Unrealize1", args, n);
    XtManageChild (Unrealize1);
    XtAddCallback (Unrealize1, XmNactivateCallback, UnrealizeShell, TopShell2);


    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 700); n++;
    XtSetArg (args[n], XmNlabelString,
       XmStringCreateSimple ("Realize TopShell2 by XtRealizeWidget")); n++;
    Realize1 = XmCreatePushButton (BulletinBoard1, "Realize1", args, n);
    XtManageChild (Realize1);
    XtAddCallback (Realize1, XmNactivateCallback, RealizeShell, TopShell2);

  /* End CR 3622 */

    /* Shell 2 children */

    n = 0;
    BulletinBoard2 = XmCreateBulletinBoard (TopShell2, "BulletinBoard2", 
											args, n);    
    XtManageChild (BulletinBoard2);

    n = 0;
    XtSetArg (args[n], XmNx, 200); n++;
    XtSetArg (args[n], XmNy, 50); n++;
    DefaultButton2 = XmCreatePushButtonGadget (BulletinBoard2, 
											   "DefaultButton2", args, n);
    XtManageChild (DefaultButton2);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, DefaultButton2); n++;
    XtSetValues (BulletinBoard2, args, n);
    
    XtRealizeWidget(TopShell1);
    XtPopup(TopShell1, XtGrabNone);
    XtRealizeWidget(TopShell2);
    XtPopup(TopShell2, XtGrabNone);

											/* Begin CR4909 */
    n = 0;
    /* message box resources */
#ifdef MOTIF1_1
    tcs = XmStringCreateLtoR(first_msg, XmSTRING_DEFAULT_CHARSET);
#else
    tcs = XmStringCreateLtoR(first_msg, XmFONTLIST_DEFAULT_TAG);
#endif
    XtSetArg (args[n], XmNmessageString, tcs);n++;
    XtSetArg (args[n], XmNautoUnmanage, False); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Primary Modal"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++;
    FirstDialog = XmCreateMessageDialog(TopShell1, "FirstDialog", args, n);

	XtAddCallback(FirstDialog, XmNcancelCallback, DestroyCB, NULL);

    XtManageChild (FirstDialog);

	XmStringFree(tcs);
											/* End CR4909 */

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

        CommonPause();

        CommonPause();


      XtAppMainLoop(app_context);
}

											/* Begin CR4909 */
static void DestroyCB(w, client_data, call_data)
Widget	w;
XtPointer	client_data;
XtPointer	call_data;
{

	int	n;
	Arg	args[MAX_ARGS];
	Widget SecondDialog;
	XmString	tcs;


	XtUnmanageChild(w);
	XtDestroyWidget(w);

    n = 0;
    /* message box resources */
#ifdef MOTIF1_1
    tcs = XmStringCreateLtoR(second_msg, XmSTRING_DEFAULT_CHARSET);
#else
    tcs = XmStringCreateLtoR(second_msg, XmFONTLIST_DEFAULT_TAG);
#endif
    XtSetArg (args[n], XmNmessageString, tcs);n++;

    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Primary Modal 2"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++;
    SecondDialog = XmCreateMessageDialog(FirstShell, "SecondDialog", args, n);

    XtManageChild (SecondDialog);

	XmStringFree(tcs);

}
											/* End CR4909 */
