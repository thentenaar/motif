/* $XConsortium: XmeFocusChangeCallback.c /main/4 1995/07/15 21:04:31 drk $ */
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

void
FocusCB(w, client_data, cbs)
Widget w;
XtPointer client_data, cbs;
{
  printf("New Focus Change Callback has been called\n");
}

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           xmstr;
  Widget             vshell, button;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetArg(args[n], XmNmappedWhenManaged,False); n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  n =0;
  vshell = XtVaCreateWidget("button", vendorShellWidgetClass, Shell1, NULL);
  XtManageChild(vshell);

  xmstr = XmStringCreateSimple ("Validate");
  button = XtVaCreateWidget("button", xmPushButtonWidgetClass, vshell, NULL);
  XtSetArg(args[n], XmNlabelString, xmstr);
  XtSetValues(button, args, 1);
  XmStringFree(xmstr);
  XtManageChild(button);
  CommonPause();

  /* add a new focus change callback */
  XmeAddFocusChangeCallback(vshell, FocusCB, NULL);
  CommonPause();

  /* remove the new focus change callback */
  XmeRemoveFocusChangeCallback(vshell, FocusCB, NULL);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
