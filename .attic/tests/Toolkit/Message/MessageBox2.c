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
static char rcsid[] = "$XConsortium: MessageBox2.c /main/9 1995/07/13 18:59:03 drk $"
#endif
#endif

#include <testlib.h>

Widget 		mbox;                        /*  MessageBox widget   */
Widget  	dbox; 		             /*  Question Dialog     */
Boolean		isSet;
Widget          shell;                       

static  XmString  str1 = NULL;
static  XmString  str2 = NULL;
static  XmString  str3 = NULL;
static  XmString  str4 = NULL;

/* callbacks */
static void OkCB();
static XtCallbackRec ok_cb[] = {
    {  OkCB, NULL},
    { NULL,                  NULL}
};
static void CancelCB();
static XtCallbackRec cancel_cb[] = {
    {  CancelCB, NULL},
    { NULL,                  NULL}
};
static void HelpCB();
static XtCallbackRec help_cb[] = {
    {  HelpCB, NULL},
    { NULL,                NULL}
};


/*#### OkCB ####*/
static void OkCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n = 0;
    if (!isSet)
    {
	XtSetArg (args[n], XmNdialogType, XmDIALOG_WORKING);	n++;
	isSet = True;
    }
    XtSetArg (args[n], XmNmessageAlignment, XmALIGNMENT_BEGINNING);      n++;
    XtSetValues (w, args, n);
}


/*#### CancelCB ####*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n=0;
    XtSetArg (args[n], XmNmessageAlignment, XmALIGNMENT_CENTER);      n++;
    XtSetValues (w, args, n);
}


/*#### HelpCB ####*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n=0;
    XtSetArg (args[n], XmNmessageAlignment, XmALIGNMENT_END);      n++;
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


    /* create compound string */
    str1 = XmStringCreate ("Beginning", XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Center", XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("End", XmSTRING_DEFAULT_CHARSET);
    str4 = XmStringLtoRCreate ("Face piles of trials with smiles.\nIt riles them to believe\nthat you perceive the web they weave,\nand keep on thinking free.",
                                XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                         n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);                 n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                     n++;
    XtSetArg (args[n], XmNmessageString, str4);                       n++;
    XtSetArg (args[n], XmNokLabelString, str1);                       n++;
    XtSetArg (args[n], XmNcancelLabelString, str2);                   n++;
    XtSetArg (args[n], XmNhelpLabelString, str3);                     n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                       n++;

    isSet = False;

    mbox = XmCreateMessageBox (Frame, "mbox", args, n);

    XtManageChild (mbox);
    XtRealizeWidget (Shell1);

    CommonPause();

    isSet = False;
    
    dbox = XmCreateQuestionDialog (Shell1, "dbox", args, n);
    XtManageChild (dbox);

    CommonPause();

    /*  process events  */

    XtAppMainLoop(app_context);
}
