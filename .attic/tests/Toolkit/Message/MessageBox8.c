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
static char rcsid[] = "$XConsortium: MessageBox8.c /main/8 1995/07/13 19:01:56 drk $"
#endif
#endif

#include <X11/IntrinsicP.h>

#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/MessageBP.h>

#include <testlib.h>

XmMessageBoxWidget  mbox;                        /*  MessageBox widget   */

static XmString str1 = NULL;
static XmString str2 = NULL;
static XmString str3 = NULL;

/* callbacks */

static void OkCB();
static XtCallbackRec ok_cb[] = {
    { OkCB, NULL},
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
    static   int minimize = False;

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
**      HelpCB        - callback for help button
*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer   client_data;                 /*  data from application   */
XtPointer   call_data;                   /*  data from widget class  */
{
    static Boolean flag=False;
    XtGeometryResult geoResult;
    XtWidgetGeometry intended;
    XtWidgetGeometry result;

    if (flag) { 
        XtQueryGeometry (w, NULL, &result);
        printf("query with NULL\n");
        printf("      result is (mode, x, y, w, h) %d %d %d %d %d\n", 
                result.request_mode, result.x, result.y, 
                result.width, result.height);
        printf("      current is %d %d %d %d\n", w->core.x, w->core.y,
                w->core.width, w->core.height);
        flag=False;
    }
    else {
        intended.request_mode = (CWX | CWY | CWWidth | CWHeight);
        intended.x = 50;
        intended.y = 75;
        intended.width = 300;
        intended.height = 500;
        XtQueryGeometry (w, &intended, &result);
        printf("query with 50, 75, 300, 500\n");
        printf("      result is (mode, x, y, w, h) %d %d %d %d %d\n", 
                result.request_mode, result.x, result.y, 
                result.width, result.height);
        printf("               current is  %d %d %d %d\n", w->core.x, w->core.y,
                w->core.width, w->core.height);
        flag=True;
    }
}


/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[10];
    register int    n;      
    Widget 	    Frame;

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);

    n = 0; 
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);

    /* create message dialog */

    str1 = XmStringCreate ("NoMinimize",  XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Minimize",  XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("DoQuery",  XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                     n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                 n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                   n++;
    XtSetArg (args[n], XmNhelpLabelString, str3);                 n++;
    XtSetArg (args[n], XmNokLabelString, str1);                   n++;
    XtSetArg (args[n], XmNminimizeButtons, False);                n++;

    mbox = (XmMessageBoxWidget) XmCreateMessageBox (Frame, "mbox", args, n);

    XtUnmanageChild(XmMessageBoxGetChild((Widget) mbox, 
		    XmDIALOG_CANCEL_BUTTON));
    XtManageChild ((Widget) mbox);

    XtRealizeWidget (Shell1);

    CommonPause();
    CommonPause();

    /*  process events  */
    XtAppMainLoop(app_context);
}
