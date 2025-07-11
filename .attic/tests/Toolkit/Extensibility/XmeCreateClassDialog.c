/* $XConsortium: XmeCreateClassDialog.c /main/4 1995/07/15 21:04:17 drk $ */
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

Widget rc, button, list;

char *items[] = {
  "File","Open","Quit"
};

void
goodbye(w, client_data, cbs)
Widget w;
XtPointer client_data, cbs;
{
  XtPopdown(XtParent(w));
}

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n, cnt = XtNumber(items);
  Arg                args[MAX_ARGS];
  String             dirname;
  XmStringTable      str_list;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  n = 0;
  rc = XmCreateRowColumn(Shell1, "rc", args, n);

  n = 0;
  XtSetArg(args[n], XmNwidth,  160);  n++;
  XtSetArg(args[n], XmNheight, 80);  n++;
  button = XmeCreateClassDialog(xmPushButtonWidgetClass,
				rc, "button", args, n);
  XtAddCallback(button, XmNactivateCallback, goodbye, NULL);
  XtManageChild(button);
  CommonPause();

  /* prepare string list for list widget */
  str_list = (XmStringTable)XtMalloc(cnt * sizeof(XmString *));
  for (n=0; n<cnt; n++)
    str_list[n] = XmStringCreateSimple(items[n]);

  n = 0;
  XtSetArg(args[n], XmNwidth,  160);  n++;
  XtSetArg(args[n], XmNvisibleItemCount, cnt);  n++;
  XtSetArg(args[n], XmNitemCount, cnt);  n++;
  XtSetArg(args[n], XmNitems, str_list);  n++;
  list = XmeCreateClassDialog(xmListWidgetClass,
			      rc, "list", args, n);
  XtAddCallback(list, XmNdefaultActionCallback, goodbye, NULL);
  XtManageChild(list);

  for (n=0; n<cnt; n++)
    XmStringFree(str_list[n]);
  XtFree(str_list);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
