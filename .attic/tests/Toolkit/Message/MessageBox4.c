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
static char rcsid[] = "$XConsortium: MessageBox4.c /main/9 1995/07/13 18:59:51 drk $"
#endif
#endif

#include <X11/IntrinsicP.h>

#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/MessageBP.h>

#include <testlib.h>
#include "MessageBox4.h"

XmMessageBoxWidget  mbox;                        /*  MessageBox widget   */
XmMessageBoxWidget  dbox;                        /*  Information Dialog  */

static XmString str1 = NULL;
static XmString str2 = NULL;
static XmString str3 = NULL;


/* callbacks */

static void OkCB();
static XtCallbackRec ok_cb[] = {
    { OkCB, NULL},
    { NULL,                  NULL}
};

static void CancelCB();
static XtCallbackRec cancel_cb[] = {
    { CancelCB, NULL},
    { NULL,                  NULL}
};

static void HelpCB();
static XtCallbackRec help_cb[] = {
    { HelpCB, NULL},
    { NULL,                NULL}
};


/*-------------------------------------------------------------
**      OkCB        - callback for ok button
*/
static void OkCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    static int minimize = False;

    if (minimize) {
        n=0;
        XtSetArg (args[n], XmNminimizeButtons, False); n++;
        XtSetArg (args[n], XmNokLabelString, str1); n++;
        XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
        XtSetValues (w, args, n);
        minimize = False;
    }
    else {
        n=0;
        XtSetArg (args[n], XmNminimizeButtons, True); n++;
        XtSetArg (args[n], XmNokLabelString, str2); n++;
        XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
        XtSetValues (w, args, n);
        minimize = True;
    }
}



/*-------------------------------------------------------------
**      CancelCB        - callback for cancel button
*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n=0;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
    XtSetValues (w, args, n);
}



/*-------------------------------------------------------------
**      HelpCB        - callback for help button
*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n=0;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_HELP_BUTTON); n++;
    XtSetValues (w, args, n);
}


/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[10];           
    register int    n;                 
    Widget	    Frame;


    /*  initialize toolkit  */
    CommonTestInit(argc, argv);

    n = 0;
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);

    /* create message dialog */

    str1 = XmStringCreate ("Minimize",  XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("NoMinimize",  XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("Push to default...", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                     n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);             n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                 n++;
    XtSetArg (args[n], XmNmessageString, str3);                   n++;
    XtSetArg (args[n], XmNokLabelString, str1);                   n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                   n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;
    XtSetArg (args[n], XmNminimizeButtons, False);                n++;

    mbox = (XmMessageBoxWidget) XmCreateMessageBox (Frame, "mbox", args, n);
    XtManageChild ((Widget) mbox);

    XtRealizeWidget (Shell1);

    CommonPause();

    dbox = 
	  (XmMessageBoxWidget) XmCreateQuestionDialog (Shell1, "dbox", args, n);
    XtManageChild ((Widget) dbox);

    CommonPause();

    /*  process events  */
    XtAppMainLoop(app_context);
}
