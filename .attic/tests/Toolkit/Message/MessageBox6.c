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
static char rcsid[] = "$XConsortium: MessageBox6.c /main/8 1995/07/13 19:00:40 drk $"
#endif
#endif

#include <X11/IntrinsicP.h>
#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/MessageBP.h>

#include <testlib.h>

XmMessageBoxWidget  mbox;                        /*  MessageBox widget   */

static int manage = False;
static  XmString str1 = NULL;
static  XmString str2 = NULL;
static  XmString str3 = NULL;
static  XmString str4 = NULL;
static  XmString str5 = NULL;
static  XmString str6 = NULL;
static  XmString str7 = NULL;
static  XmString str8 = NULL;
static  XmString str9 = NULL;


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


/*#### OkCB ####*/
static void OkCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg      args[10];
    register int n;
    Widget   child=NULL;

    if (manage) {
        n=0;
        XtSetArg (args[n], XmNokLabelString, str1); n++;
        XtSetValues (w, args, n);
        manage = False;
    }
    else {
        n=0;
        XtSetArg (args[n], XmNokLabelString, str5); n++;
        XtSetValues (w, args, n);
        manage = True;
    }
}


/*#### CancelCB ####*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_SYMBOL_LABEL);
    if (child) {
        if (manage) XtManageChild (child);
        else XtUnmanageChild (child);
    }
}


/*#### HelpCB ####*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_MESSAGE_LABEL);
    if (child)  {
            if (manage) XtManageChild (child);
            else XtUnmanageChild (child);
    }
}


/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[20];
    register int    n;
    Widget	    Frame;

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);

    n = 0;
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);
    
    /* create compound string */
    str1 = XmStringCreate ("Unmanage", XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Help", XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("Symbol", XmSTRING_DEFAULT_CHARSET);
    str4 = XmStringLtoRCreate ("Toggle Unmanage/Manage and select a button",
                                XmSTRING_DEFAULT_CHARSET);
    str5 = XmStringCreate ("Manage", XmSTRING_DEFAULT_CHARSET);
    str6 = XmStringCreate ("Ok", XmSTRING_DEFAULT_CHARSET);
    str7 = XmStringCreate ("Message", XmSTRING_DEFAULT_CHARSET);
    str8 = XmStringCreate ("Center", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                         n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);                 n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                     n++;
    XtSetArg (args[n], XmNmessageString, str4);                       n++;
    XtSetArg (args[n], XmNokLabelString, str1);                       n++;
    XtSetArg (args[n], XmNcancelLabelString, str3);                   n++;
    XtSetArg (args[n], XmNhelpLabelString, str7);                     n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                       n++;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_ERROR);                n++;
    XtSetArg (args[n], XmNwidth, 150);                                n++;
    XtSetArg (args[n], XmNheight, 300);                               n++;
    XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE);               n++;

    mbox = (XmMessageBoxWidget) XmCreateMessageBox (Frame, "mbox", args, n);
    XtManageChild ((Widget) mbox);

    XtRealizeWidget (Shell1);

    CommonPause();
    CommonPause();

    /*  process events  */
    XtAppMainLoop(app_context);
}
