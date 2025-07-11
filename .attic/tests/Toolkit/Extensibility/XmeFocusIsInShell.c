/* $XConsortium: XmeFocusIsInShell.c /main/4 1995/07/15 21:04:35 drk $ */
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
#include <Xm/PushB.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>

Widget button, button2, extra_shell;
Display *display;

void
button_callback(w, client_data, cbs)
Widget w;
XtPointer client_data;
XmPushButtonCallbackStruct *cbs;
{
  if (XmeFocusIsInShell(button))
    printf("OK - focus is in button\n");
  else
    printf("ERROR - focus should be in button\n");

  /* make sure that focus is NOT in button2 which is in a
     different widget tree than button */
  if (XmeFocusIsInShell(button2))
    printf("ERROR - focus is in button2\n");
  else
    printf("OK - focus should not be in button2\n");
}

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  String             dirname;
  XmString           label_str;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  display = XtDisplay(Shell1);

  /* create a button for the user to push and subsequently check
     the funtion. */
  label_str = XmStringCreateSimple("Push Me");
  button = XtVaCreateManagedWidget("button",xmPushButtonWidgetClass, Shell1,
				   XmNlabelString, label_str,
				   XmNwidth, 100,
				   XmNheight, 100,
				   NULL);
  XmStringFree(label_str);
  XtAddCallback(button, XmNactivateCallback, button_callback, NULL);

  /* Create a second independent shell for this test */
  extra_shell = XtAppCreateShell(NULL,
				 "application_class",
				 applicationShellWidgetClass,
				 display,
				 NULL, 0);

  /* create a button */
  label_str = XmStringCreateSimple("Leave me alone.");
  button2 = XtVaCreateManagedWidget("button2",xmPushButtonWidgetClass,
				    extra_shell,
				    XmNlabelString, label_str,
				    NULL);
  XmStringFree(label_str);
  XtRealizeWidget(extra_shell);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
