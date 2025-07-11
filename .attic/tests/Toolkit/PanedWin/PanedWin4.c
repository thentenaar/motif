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
static char rcsid[] = "$XConsortium: PanedWin4.c /main/4 1995/07/13 19:06:57 drk $"
#endif
#endif


#include <testlib.h>
#include <X11/StringDefs.h>
Widget  PanedWindow1 = NULL;
Widget  PushButton1 = NULL;
Widget  PushButton2 = NULL;
Widget  PushButton3 = NULL;

void  main (argc, argv)
int     argc;
char  **argv;
{
    int                delay = 0;
    Boolean            mywait = False;
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs = NULL;
    XWindowAttributes  info;
    XEvent             DummyEvent;

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 400);  n++;
    XtSetArg(args[n], XmNheight,300);  n++;
    XtSetArg(args[n], XmNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);
  
    XmRegisterConverters();
#ifdef  SYNCHRONIZE
    XSynchronize(display, 1);
#endif  /*  SYNCHRONIZE  */

    XtRealizeWidget(Shell1);

    mywait = True;
    while (mywait) {
        XGetWindowAttributes(XtDisplay(Shell1), XtWindow(Shell1), &info);
        if (info.map_state == IsViewable) mywait = False;
        else sleep(1);
    }

    flushEvents(delay);

    n = 0;
    PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1",
                    args, n);
    XtManageChild(PanedWindow1);

    n = 0;
    PushButton1 = XmCreatePushButton(PanedWindow1, "PushButton1",
                    args, n);
    XtManageChild(PushButton1);

    n = 0;
    PushButton2 = XmCreatePushButton(PanedWindow1, "PushButton2",
                    args, n);
    XtManageChild(PushButton2);
    XmStringFree(tcs);

    n = 0;
    PushButton3 = XmCreatePushButton(PanedWindow1, "PushButton3",
                    args, n);
    XtManageChild(PushButton3);
    XmStringFree(tcs);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNallowResize, False);  n++;
    XtSetValues(PushButton1, args, n);

    n = 0;
    XtSetArg(args[n], XmNpaneMinimum, 20);  n++;
    XtSetArg(args[n], XmNpaneMaximum, 50);  n++;
    XtSetValues(PushButton2, args, n);

    n = 0;
    XtSetArg(args[n], XmNskipAdjust, False);  n++;
    XtSetValues(PushButton3, args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 10);   n++;
    XtSetValues(PanedWindow1, args, n);

    CommonPause();

    XtDestroyWidget (PanedWindow1);
    n = 0;
    PanedWindow1 = XmCreatePanedWindow(Shell1, "PanedWindow1",
                    args, n);
    XtManageChild(PanedWindow1);

    n = 0;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNwidth, 100); n++;
    PushButton1 = XmCreatePushButton(PanedWindow1, "PushButton1",
                    args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg (args[n], XmNheight, 100); n++;
    PushButton2 = XmCreatePushButton(PanedWindow1, "PushButton2",
                    args, n);
    XtManageChild(PushButton2);
    XmStringFree(tcs);

    n = 0;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNpaneMinimum, 50); n++;
    XtSetArg (args[n], XmNskipAdjust, True); n++;
    PushButton3 = XmCreatePushButton(PanedWindow1, "PushButton3",
                    args, n);
    XtManageChild(PushButton3);    

    CommonPause();
  
    XtAppMainLoop(app_context);
}

flushEvents(delay)
int    delay;
{
    XEvent event;


    XSync(display, False);
    while (XPending(display)) {
        XNextEvent(display, &event);
        XtDispatchEvent(&event);
    }

    sleep(delay);
}
