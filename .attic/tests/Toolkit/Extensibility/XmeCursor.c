/* $XConsortium: XmeCursor.c /main/4 1995/07/15 21:04:21 drk $ */
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
static char rcsid[] = ""
#endif
#endif

#include <testlib.h>
#include <time.h>
#include <Xm/PushB.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>
#define BUFSIZE 40
#define MSLEEP 999999L

void
Sleep(w, client_data, cbs)
Widget w;
XtPointer client_data, cbs;
{
  time_t             tval;
  struct tm          *tmptr;
  char               buf[BUFSIZE], buf2[BUFSIZE];
  int                start, stop, duration;

  /* Start a timer test */
  tval = time(NULL);
  tmptr = localtime(&tval);
  strftime(buf,BUFSIZE, "%S", tmptr);

  /* sleep for 4 seconds approximately */
  if (XmeMicroSleep(MSLEEP) == -1)
    printf("ERROR: XmeMicroSleep failed\n");
  if (XmeMicroSleep(MSLEEP) == -1)
    printf("ERROR: XmeMicroSleep failed\n");
  if (XmeMicroSleep(MSLEEP) == -1)
    printf("ERROR: XmeMicroSleep failed\n");
  if (XmeMicroSleep(MSLEEP) == -1)
    printf("ERROR: XmeMicroSleep failed\n");

  tval = time(NULL);
  tmptr = localtime(&tval);
  strftime(buf2,BUFSIZE, "%S", tmptr);

  /* verify the results (either 4 or 5 is correct) */
  sscanf(buf,"%d",&start);
  sscanf(buf2,"%d",&stop);
  if (stop < start)
    stop += 60;
  duration = stop - start;

  if (duration != 4 && duration !=5) 
    printf("ERROR: sleep duration is not in the correct range\n");
  else
    printf("OK: sleep test passes\n");
}

void
PushMe(w, client_data, cbs)
Widget w;
XtPointer client_data, cbs;
{
  printf("Button Press received\n");
}

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  int                status;
  Arg                args[MAX_ARGS];
  String             dirname;
  Widget             rc, button, button2;
  Display            *display;
  Window             window;
  Cursor             null_cursor;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  /* set up push button widgets */
  n = 0;
  rc = XmCreateRowColumn(Shell1, "RowC", args, n);
  XtManageChild(rc);
  window = XtWindow(rc);
  display = XtDisplay(rc);

  /* create buttons for test */
  n = 0;
  button = XmCreatePushButton(rc, "Start Sleep", args, n);
  button2 = XmCreatePushButton(rc, "Validate", args, n);
  XtAddCallback(button, XmNactivateCallback, Sleep, (XtPointer)1);
  XtAddCallback(button2, XmNactivateCallback, PushMe, NULL);
  XtManageChild(button);
  XtManageChild(button2);
  CommonPause();

  /* reset the callback */
  XtRemoveCallback(button, XmNactivateCallback, Sleep, (XtPointer)1);
  XtAddCallback(button, XmNactivateCallback, PushMe, (XtPointer)2);

  /* set Null cursor */
  null_cursor = XmeGetNullCursor(Shell1);
  status = XDefineCursor(display, window, null_cursor);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
