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
static char rcsid[] = "$XConsortium: MessageBox3.c /main/9 1995/07/13 18:59:27 drk $"
#endif
#endif

/*  include files  */

#include <X11/IntrinsicP.h>
#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/MessageBP.h>

#include <testlib.h>

XmMessageBoxWidget  mbox;                        /*  MessageBox widget   */
XmMessageBoxWidget  dbox;                        /*  Information Dialog  */

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
    Arg          args[10];
    register int n;
    Widget   child=NULL;
    Pixmap   pix;

    pix = ((XmMessageBoxWidget)w)->message_box.symbol_pixmap;

    child = XmMessageBoxGetChild (w, XmDIALOG_OK_BUTTON);
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmPIXMAP);   n++;
    XtSetArg (args[n], XmNlabelPixmap, pix);      n++;
    XtSetValues (child, args, n);

    child = XmMessageBoxGetChild (w, XmDIALOG_CANCEL_BUTTON);
    n = 0;
    XtSetArg (args[n], XmNlabelString, str6);      n++;
    XtSetValues (child, args, n);

    child = XmMessageBoxGetChild (w, XmDIALOG_HELP_BUTTON);
    n = 0;
    XtSetArg (args[n], XmNlabelString, str7);      n++;
    XtSetValues (child, args, n);
}


/*#### CancelCB ####*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_SYMBOL_LABEL);
    n = 0;
    XtSetArg (args[n], XmNlabelType, XmSTRING);      	n++;
    XtSetArg (args[n], XmNlabelString, str8);      	n++;
    XtSetValues (child, args, n);
}


/*#### HelpCB ####*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_MESSAGE_LABEL);
    n = 0;
    XtSetArg (args[n], XmNlabelString, str9);      n++;
    XtSetValues (child, args, n);
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
    
    /* create compound string */
    str1 = XmStringCreate ("Buttons", XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Symbol", XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("Message", XmSTRING_DEFAULT_CHARSET);
    str4 = XmStringLtoRCreate ("Select a button to change a\nchild/children via that child itself\n(ie. XmMessageBoxGetChild facility)", XmSTRING_DEFAULT_CHARSET);
    str5 = XmStringCreate ("NewButtons", XmSTRING_DEFAULT_CHARSET);
    str6 = XmStringCreate ("NewSymbol", XmSTRING_DEFAULT_CHARSET);
    str7 = XmStringCreate ("NewMessage", XmSTRING_DEFAULT_CHARSET);
    str8 = XmStringLtoRCreate ("StringIcon", XmSTRING_DEFAULT_CHARSET);
    str9 = XmStringCreate ("Here is a new message!", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                         n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);                 n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                     n++;
    XtSetArg (args[n], XmNmessageString, str4);                       n++;
    XtSetArg (args[n], XmNokLabelString, str1);                       n++;
    XtSetArg (args[n], XmNcancelLabelString, str2);                   n++;
    XtSetArg (args[n], XmNhelpLabelString, str3);                     n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                       n++;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_INFORMATION);          n++;

    mbox = (XmMessageBoxWidget) XmCreateMessageBox (Frame, "mbox", args, n);

    XtManageChild ((Widget) mbox);
    XtRealizeWidget ((Widget) Shell1);

    CommonPause();

    dbox = 
      (XmMessageBoxWidget) XmCreateInformationDialog (Shell1, "dbox", args, n);
    XtManageChild ((Widget) dbox);

    CommonPause();

    /*  process events  */
    XtAppMainLoop(app_context);
}
