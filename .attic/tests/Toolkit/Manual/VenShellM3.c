/* $XConsortium: VenShellM3.c /main/5 1995/07/15 21:06:51 drk $ */
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


#include <testlib.h>

Widget button0,  shell1, shell2, shell3, button1, button2, button3;

static void 
button_activate(Widget w, XtPointer client, XtPointer data)
{
    int button_num = *((int *)client);

    switch (button_num) {
      case 1 :
	  XtPopup (shell1, XtGrabNone);
	  XtAddGrab (Shell1, TRUE, FALSE);
	  XtPopup (shell2, XtGrabNone);
	  XtRemoveGrab (Shell1);
	  XtPopup (shell3, XtGrabNone);
	  break;
      default :
	  break;
    }
}

void
main(int argc, char *argv[])
{
 Arg args[10];
 int n;
 static int i = 1;

  
 CommonTestInit (argc,argv);

 n=0;
 button0 = (Widget) XmCreatePushButton(Shell1, "Pop 'em up", args,n);
 XtAddCallback(button0, XmNactivateCallback, button_activate, (XtPointer)&i);
 XtManageChild(button0);

 n=0;
 shell1 = XtAppCreateShell("shell1", "Shell1", topLevelShellWidgetClass, 
			   display,args,n);

 n=0;
 button1 = (Widget) XmCreatePushButton(shell1, "Button1", args,n);
 XtManageChild (button1);

 shell2 = XtAppCreateShell("shell2", "Shell2", topLevelShellWidgetClass, 
			   display,args,n);


 n=0;
 button2 = (Widget) XmCreatePushButton(shell2, "Button2", args,n);
 XtManageChild (button2);

 shell3 = XtAppCreateShell("shell3", "Shell3", topLevelShellWidgetClass, 
			   display,args,n);

 
 n=0;
 button3 = (Widget) XmCreatePushButton(shell3, "Button3", args,n);
 XtManageChild (button3);

 XtRealizeWidget(Shell1);

 CommonPause();

 XtAppMainLoop(app_context);
}
