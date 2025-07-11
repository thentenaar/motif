/* $XConsortium: MwmUI4.c /main/5 1995/07/15 21:14:17 drk $ */
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

#define NUM_WINDOWS 12

void
main (int argc, char **argv)
{
  Widget TestShell1, TestShell2, TestShell3;
  Widget TopLevelShell[10];
  Widget PB1, PB2, PB3;
  Widget TopLevelPB[10];
  Widget MesDialog1;
  Widget Text1;
  int n,i;
  Arg args[10];
  char buf[32];

  CommonTestInit(argc,argv);


  n=0;
  XtSetArg (args[n], XmNheight, 200); n++;
  XtSetArg (args[n], XmNwidth, 200); n++;
  TestShell1 = XtCreatePopupShell ("TestWindow1",
				 topLevelShellWidgetClass,
				 Shell1, args, n);

  n=0;
  PB1 = XmCreatePushButton (TestShell1, "PB1", args, n);
  XtManageChild (PB1);

  XtPopup (TestShell1,XtGrabNone);

  CommonPause();


  n=0;
  XtSetArg (args[n], XmNheight, 200); n++;
  XtSetArg (args[n], XmNwidth, 200); n++;
  TestShell2 = XtCreatePopupShell ("TestWindow2", 
				 topLevelShellWidgetClass,
				 Shell1, args, n);

  n=0;
  PB2 = XmCreatePushButton (TestShell2, "PB2", args, n);
  XtManageChild (PB2);

  XtPopup (TestShell2,XtGrabNone);


  n=0;
  XtSetArg (args[n], XmNheight, 200); n++;
  XtSetArg (args[n], XmNwidth, 200); n++;
  TestShell3 = XtCreatePopupShell ("TestWindow3", 
				 topLevelShellWidgetClass,
				 Shell1, args, n);

  n=0;
  PB3 = XmCreatePushButton (TestShell3, "PB3", args, n);
  XtManageChild (PB3);

  XtPopup (TestShell3,XtGrabNone);



  CommonPause();

  
  n=0;
  MesDialog1= XmCreateMessageDialog (TestShell1,"MesDialog1",
                                     args, n);
  XtManageChild (MesDialog1);
  
  CommonPause();
  CommonPause();

  

  


  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();

  XtAppMainLoop (app_context);

}

