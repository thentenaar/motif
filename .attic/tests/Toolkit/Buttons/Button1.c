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
static char rcsid[] = "$XConsortium: Button1.c /main/6 1995/07/13 17:28:44 drk $"
#endif
#endif


#include <testlib.h>

void arm();
void disarm();
void expose();

int main(argc, argv)
unsigned int argc;
char *argv[];
{
  Widget rc, arr, pb, tog;
  Arg args[20] ;
  int n;
  XEvent *ev;

  ev = (XEvent *) XtCalloc (1, sizeof(XEvent));

  CommonTestInit(argc, argv);
  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True);  n++;
  XtSetArg(args[n], XmNwidth, 300);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetValues(Shell1, args, n);

  n = 0;
  XtSetArg(args[n], XmNwidth, 200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtManageChild(rc = XmCreateRowColumn(Shell1, "bulletin", args, n));

  n=0;
  XtSetArg(args[n], XmNheight, 100);  n++;
  XtManageChild(arr=XmCreateArrowButton(rc, "ArrowButton", args, n));
  XtManageChild(pb=XmCreatePushButton(rc, "PushButton", args, n));
  XtManageChild(tog=XmCreateToggleButton(rc, "ToggleButton", args, n));

  XtRealizeWidget(Shell1);

  CommonPause();

  /*  Arm */
  printf("=> ARM %s\n", XtName((Widget)arr));
  printf("=> ARM %s\n", XtName((Widget)pb));
  printf("=> ARM %s\n", XtName((Widget)tog)); 
  XtCallActionProc((Widget)arr, "Arm", ev, NULL, 5000);
  XtCallActionProc((Widget)pb, "Arm", ev, NULL, 5000);
  XtCallActionProc((Widget)tog, "Arm", ev, NULL, 5000);
  CommonPause();

/*  Activate */
  printf("=> ACTIVATE %s\n", XtName((Widget)arr));
  printf("=> ACTIVATE %s\n", XtName((Widget)pb));
  printf("=> ACTIVATE %s\n", XtName((Widget)tog)); 
  XtCallActionProc((Widget)arr, "Activate", ev, NULL, 5000);
  XtCallActionProc((Widget)pb, "Activate", ev, NULL, 5000);
  XtCallActionProc((Widget)tog, "Select", ev, NULL, 5000);
  CommonPause();

  /*  Disarm */
  printf("=> DISARM %s\n", XtName((Widget)arr));
  printf("=> DISARM %s\n", XtName((Widget)pb));
  printf("=> DISARM %s\n", XtName((Widget)tog));
  XtCallActionProc((Widget)arr, "Disarm", ev, NULL, 5000);
  XtCallActionProc((Widget)pb, "Disarm", ev, NULL, 5000);
  XtCallActionProc((Widget)tog, "Disarm", ev, NULL, 5000);
  CommonPause();

  /*  Expose */

  printf("=> EXPOSE %s\n", XtName((Widget)arr));
  printf("=> EXPOSE %s\n", XtName((Widget)pb));
  printf("=> EXPOSE %s\n", XtName((Widget)tog));
  XClearArea(XtDisplayOfObject((Widget)arr), XtWindowOfObject((Widget)arr), 
		0, 0, 0, 0, True);
  XClearArea(XtDisplayOfObject((Widget)pb), XtWindowOfObject((Widget)pb), 
		0, 0, 0, 0, True);
  XClearArea(XtDisplayOfObject((Widget)tog), XtWindowOfObject((Widget)tog), 
		0, 0, 0, 0, True);

  CommonPause();
  XtFree((char *) ev);

  XtMainLoop();
}


