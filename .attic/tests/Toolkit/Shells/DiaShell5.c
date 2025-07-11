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
static char rcsid[] = "$XConsortium: DiaShell5.c /main/7 1995/07/13 19:20:15 drk $"
#endif
#endif
/****************************************
*   Automatic test program generation   *
*              Version 1.4              *
*       Thu May 18 07:49:47 1989        *
****************************************/


#include "DiaShell3.h"
#include <X11/ShellP.h>
VendorShellWidget	DialogShell1;
Widget	BulletinBoard1,
	PushButton1,
	PushButton2;
Widget  Scale;

/* forward declarations */
void Quit();
static XtCallbackProc QuitCB();


/* callback procedures */

static XtCallbackProc PostPrimaryModalDialog (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

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

static XtCallbackProc PostFullAppModalDialog (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

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
    XtSetArg (args[n], XmNmessageString, 
XmStringCreateSimple ("While this box is up, you should not be able to use either Shell1 or Shell2 nor any other dialog box."
)); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Full Application Modal Dialog"); n++;
/*    XtSetArg (args[n], XmNmwmInpuMode, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++; */
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
              n++;
    Dialog2 = XmCreateMessageDialog(parent, "Dialog1", args, n);
    if (delay) sleep (delay);
    XtManageChild (Dialog2);

}

static XtCallbackProc PostSystemModalDialog (w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

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
    XtSetArg (args[n], XmNmessageString, XmStringCreateSimple ("While this box is up, you should not be able to use any other application nor any other part of this client.")); n++;
    XtSetArg (args[n], XmNautoUnmanage, True); n++;
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "Full Application Modal Dialog"); n++;
/*    XtSetArg (args[n], XmNmwmInpuMode, XmDIALOG_PRIMARY_APPLICATION_MODAL);
              n++; */
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL);
              n++;
    Dialog3 = XmCreateMessageDialog(parent, "Dialog1", args, n);
    if (delay) sleep (delay);
    XtManageChild (Dialog3);

}


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    Boolean       trace = False;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Widget        Shell2;
    Widget        BB1, BB2;
    Widget        DB, DB2, exitButton;
    Widget        test1, test1a, test2, test3;


    signal(SIGHUP,  Quit);
    signal(SIGINT,  Quit);
    signal(SIGQUIT, Quit);

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "XMclient",
                            NULL, 0, &argc, argv);
    if (display == NULL) {
        fprintf(stderr, "%s:  Can't open display\n", argv[0]);
        exit(1);
    }

    GetOptions(argc, argv, &trace);

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True);  n++;
    XtSetArg(args[n], XmNtitle, "Shell 1"); n++;
    Shell1 = XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
                              display, args, n);

    n = 0;
    XtSetArg(args[n], XmNmappedWhenManaged, True);  n++;
    XtSetArg(args[n], XmNtitle, "Shell 2"); n++;
    Shell2 = XtCreatePopupShell("Shell 2", topLevelShellWidgetClass, Shell1,
			      args, n);
   
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
    XtAddCallback (exitButton, XmNactivateCallback, Quit, NULL);

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
    XtSetArg (args[n], XmNdefaultButton, DB2); n++;
    XtSetValues (BB2, args, n);
    
    XtRealizeWidget(Shell1);

    XtRealizeWidget(Shell2);
    XtPopup(Shell2, XtGrabNone);

    XtAppMainLoop(app_context);
}



static void  GetOptions(argc, argv, trace)
    int          argc;
    char       **argv;
    Boolean     *trace;
{
    register int i;


    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'v')
            *trace = True;
        else {
            fprintf(stderr, "\nusage:  %s [-v]\n", PROGNAME);
            fprintf(stderr, "        The -v option enables trace output.\n\n");
            exit(1);
        }
    }
}



static void  Quit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}


static void  Pause()
{
    ContinuePopup = CreateContinuePopup();

    XtPopup(ContinuePopup, XtGrabNone);

    wait = True;
    while (wait)
        FlushEvents();
    Pause_cnt++;
}


static Widget CreateContinuePopup()
{
    Widget        popup;
    Widget        rowcol;
    Widget        message;
    Widget        exit_button, continue_button;
    Arg           args[MAX_ARGS];
    register int  n;
    XmString	  message_field, continue_field, exit_field;
    static XtTranslations tm_parsed, btn_parsed;
    static	  char tm[] =
"#override \n\
<Btn3Down>:MenuPopup(Next_event) \n\
<Btn3Up>:MenuPopdown(Next_event)";
    static 	char btn_tm[] =
"#override\n\
:<Key>p:MenuPopdown() \n\
:<Key>P:MenuPopdown(Next_event) \n\
:<Btn1Down>:Noop() \n\
:<Btn1Up>:Noop()";

    tm_parsed = XtParseTranslationTable(tm);

    XtOverrideTranslations(Shell1, tm_parsed);

    btn_parsed = XtParseTranslationTable(btn_tm);
    /*  create popup shell  */
    n = 0;
    XtSetArg(args[n], XtNgeometry, "+500+0");  n++;

    popup = XtCreatePopupShell("Next_event", topLevelShellWidgetClass,
                               Shell1, args, n);

    /*  create a frame for the pushbutton  */
    n = 0;
    XtSetArg(args[n], XmNmarginWidth,  12);  n++;
    XtSetArg(args[n], XmNmarginHeight, 12);  n++;
    rowcol = XmCreateRowColumn(popup, "frame", args, n);
    XtManageChild(rowcol);

    /*  create a Message Box and Control Continue and Exit */
    n = 0;
    message_field = XmStringCreateLtoR(Pause_events[Pause_cnt], XmSTRING_DEFAULT_CHARSET);
    continue_field = XmStringCreateLtoR("Continue", XmSTRING_DEFAULT_CHARSET);
    exit_field = XmStringCreateLtoR("Exit", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtranslations, btn_parsed);  n++;
    XtSetArg(args[n], XmNmessageString, message_field); n++;
    XtSetArg(args[n], XmNokLabelString, continue_field); n++;
    XtSetArg(args[n], XmNcancelLabelString, exit_field); n++;
    message = XmCreateMessageBox(rowcol, "msgbox", args, n);
    XmStringFree(continue_field);
    XmStringFree(message_field);
    XmStringFree(exit_field);
    XtManageChild(message);
    continue_button = XmMessageBoxGetChild(message, XmDIALOG_OK_BUTTON);
    exit_button = XmMessageBoxGetChild(message, XmDIALOG_CANCEL_BUTTON);
    XtAddCallback(continue_button, XmNactivateCallback, ContinueCB, popup);
    XtAddCallback(exit_button, XmNactivateCallback, QuitCB, popup);

    return(popup);

}

static void  FlushEvents()
{
    XEvent event;

    while (XtAppPending(app_context)) {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }
}



static XtCallbackProc  ContinueCB(w, client_data, call_data)
    Widget   w;
    caddr_t  client_data;
    caddr_t  call_data;
{
    wait = False;
    XtPopdown((Widget)client_data);
}


static XtCallbackProc  QuitCB(w, client_data, call_data)
    Widget   w;
    caddr_t  client_data;
    caddr_t  call_data;

{
	printf("Exiting...Please Standby...\n");
	exit(0);
}


