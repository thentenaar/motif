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
static char rcsid[] = "$XConsortium: QuestionDia1.c /main/8 1995/07/13 19:02:55 drk $"
#endif
#endif


#include <signal.h>

#include <X11/IntrinsicP.h>

#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/MessageBP.h>

#include <testlib.h>


XmMessageBoxWidget  mbox;                        /*  MessageBox widget   */
Widget          shell;              		 /*  Dialog Shell        */

static XmString str1 = NULL;
static XmString str2 = NULL;
static XmString str3 = NULL;


/* callbacks */

static void OkCB();
static XtCallbackRec ok_cb[] = {
    {  OkCB, NULL},
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
    Arg      args[10];
    register int n;

    n=0;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_WARNING); 	n++;
    XtSetValues (w, args, n);
}


/*-------------------------------------------------------------
**      CancelCB        - callback for cancel button
*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;

    n = 0;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_WORKING); 	n++;
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

    n = 0;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_INFORMATION); 	n++;
    XtSetValues (w, args, n);
}


/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[10];           /*  arg list            */
    register int    n;                  /*  arg count           */
    /* for test for PIR 4070 */
    Widget textfield, bb, button[3];
    int i;
    char buf[32];

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg (args[n], XmNwidth, 10);                     n++;
    XtSetArg (args[n], XmNheight, 10);                    n++;
    XtSetValues(Shell1, args, n);
    
    /* create message dialog */

    str1 = XmStringCreate ("Warning",  XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Working",  XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("Information", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                     n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);             n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                 n++;
    XtSetArg (args[n], XmNokLabelString, str1);                   n++;
    XtSetArg (args[n], XmNcancelLabelString, str2);               n++;
    XtSetArg (args[n], XmNhelpLabelString, str3);                 n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                   n++;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_OK_BUTTON); n++;

    mbox = (XmMessageBoxWidget)
            XmCreateQuestionDialog (Shell1, "mbox", args, n);


    XtManageChild ((Widget) mbox);
    XtRealizeWidget (Shell1);

    CommonPause();

    /* begin test for PIR 4070 */

    n = 0;
    XtSetArg (args[n], XmNdefaultButtonType, XmDIALOG_HELP_BUTTON); 	n++;
    XtSetArg (args[n], XmNbackground, CommonGetColor ("thistle")); 	n++;
    XtSetValues ((Widget) mbox, args, n);

    CommonPause();

    /* because the default button always tracks the focus in 1.2,
       the above is not the right test. You need to have another type
       of widget with the focus before the set values. */
    /* Widget textfield, bb, button[3]; */

    XtDestroyWidget ((Widget) mbox);
    n = 0;
    bb = XmCreateBulletinBoardDialog (Shell1, "bb", args, n);
    
    for (i = 0; i <= 2; i++) {
	sprintf (buf, "button%d", i);
	n = 0;
        XtSetArg (args[n], XmNx, 200); n++;
	XtSetArg (args[n], XmNy, i*100); n++;
	XtSetArg (args[n], XmNdefaultButtonShadowThickness, 2); n++;
	button[i] = XmCreatePushButton (bb, buf, args, n);
    }

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, button[1]); n++;
    XtSetValues (bb, args, n);

    n = 0;
    XtSetArg (args[n], XmNx, 0); n++;
    XtSetArg (args[n], XmNy, 0); n++;
    textfield = XmCreateTextField (bb, "textfield", args, n);
    XtManageChild (textfield);

    XtManageChildren (button, 3);
    XtManageChild (bb);

    CommonPause ();

    /*  process events  */
    XtAppMainLoop(app_context);
}
