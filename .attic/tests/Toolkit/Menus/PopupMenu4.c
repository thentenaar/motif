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
static char rcsid[] = "$XConsortium: PopupMenu4.c /main/9 1996/02/28 15:39:27 schuldy $"
#endif
#endif


#include <testlib.h>
#include <Xm/Screen.h>

Widget popup11, popup12, popup21, popup22, popup31, popup32, popup;

static Boolean dyn_flag = False;

void menu_handler(Widget w, XtPointer tag, XEvent *event, Boolean *c)
{
  if (!dyn_flag)
    switch (event->xbutton.button) {
    case Button3:
      printf("MenuHandler: button %d\n", event->xbutton.button);
      XmMenuPosition(popup32, (XButtonPressedEvent *) event);
      XtManageChild(popup32);
      break;
    }
  else
    switch (event->xbutton.button) {
    case Button1:
      printf("MenuHandler: button %d\n", event->xbutton.button);
      XmMenuPosition(popup32, (XButtonPressedEvent *) event);
      XtManageChild(popup32);
      break;
    }    
}

void main(argc, argv)
int argc;
char *argv[];
{
  Widget label, label2, rc, bb,push1, xmscreen;
  Arg args[10];
  int n;

  /* initialize toolkit; classname is XMcommon; sets global Shell1  */

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);

  /* Pause 1 */


  n = 0;
  rc = XmCreateRowColumn(Shell1, "rc", args, n);

  n = 0;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  XtSetArg(args[n], XmNborderWidth, 3); n++;
  label2 = XmCreateLabel(rc, "Test for XmNunpostBehavior", args, n);

  XtManageChild(label2);
  XtManageChild(rc);

  XtAddEventHandler(label2, ButtonPressMask, FALSE, menu_handler, 0);

  XtSetArg(args[0], XmNmenuPost, "<Btn3>");
  popup22 = XmCreatePopupMenu(label2, "popup22", args, 1);

  XtSetArg(args[0], XmNmenuPost, "<Btn3>");
  popup32 = XmCreatePopupMenu(label2, "popup32", args, 1);

  XtManageChild(XmCreatePushButton(popup22, "Dog", NULL, 0));
  XtManageChild(XmCreatePushButton(popup22, "Cat", NULL, 0));
  XtManageChild(XmCreatePushButton(popup22, "Bird", NULL, 0));

  XtManageChild(XmCreatePushButton(popup32, "Pink", NULL, 0));
  XtManageChild(XmCreatePushButton(popup32, "Black", NULL, 0));
  XtManageChild(XmCreatePushButton(popup32, "Red", NULL, 0));

  XtRealizeWidget(Shell1);

  CommonPause();

  CommonPause();

  /* Get the XmScreen to set XmNunpostBehavior */

  xmscreen = XmGetXmScreen(screen);

  n = 0;
  XtSetArg(args[n], XmNunpostBehavior, XmUNPOST); n++;
  XtSetValues(xmscreen, args, n);

  CommonPause();

  dyn_flag = True;
  n = 0;
  XtSetArg(args[n], XmNmenuPost, "<Btn1>"); n++;
  XtSetValues(popup32, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNunpostBehavior, XmUNPOST_AND_REPLAY); n++;
  XtSetValues(xmscreen, args, n);

  CommonPause();

  CommonPause();   /* Exit */

  XtAppMainLoop(app_context); 
}




