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
static char rcsid[] = "$XConsortium: MainWin2.c /main/9 1995/07/13 19:46:45 drk $"
#endif
#endif

/************************************************************************
 *
 * Create a MainWindow, with a RowColumn button box as the command window
 *
 * Add a message area label widget
 *
 * change the command window location
 *
 ************************************************************************/

#include <signal.h>
#include <testlib.h>

void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget main_id;
   Widget bb_id;
   Widget label_id;
   Widget command_id;
   Widget rc_id;
   Widget pb1_id;
   Widget pb2_id;
   Widget pb3_id;
   Widget pb4_id;

   Arg args[10];
   int n;
   int i;

   CommonTestInit(argc, argv);
    
   n = 0;
   XtSetArg(args[n], XmNwidth, 400); 			n++;
   XtSetArg(args[n], XmNheight, 400);			n++;
   XtSetArg(args[n], XmNshowSeparator, True);		n++;
   XtSetArg(args[n], XmNcommandWindowLocation, XmCOMMAND_ABOVE_WORKSPACE); n++;
   main_id = XmCreateMainWindow(Shell1, "main", args, n);
   XtManageChild(main_id);

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); 	n++;
   rc_id = XmCreateRowColumn(main_id, "rc", args, n);
   XtManageChild(rc_id);

   n = 0;
   pb1_id = XmCreatePushButton(rc_id, "pb1", args, n);
   XtManageChild(pb1_id);

   pb2_id = XmCreatePushButton(rc_id, "pb2", args, n);
   XtManageChild(pb2_id);

   pb3_id = XmCreatePushButton(rc_id, "pb3", args, n);
   XtManageChild(pb3_id);

   pb4_id = XmCreatePushButton(rc_id, "pb4", args, n);
   XtManageChild(pb4_id);

   n = 0;
   XtSetArg(args[n], XmNwidth, 300); 			n++;
   XtSetArg(args[n], XmNheight, 300); 			n++;
   bb_id = XmCreateBulletinBoard(main_id, "bb", args, n);
   XtManageChild(bb_id);

   /* test case for PIR 4140 */
   XmMainWindowSetAreas (main_id, NULL, rc_id, NULL, NULL, bb_id);

   XtRealizeWidget(Shell1);

   CommonPause();
   /* end of test case for PIR 4140 */

   label_id = XmCreateLabel(main_id, "Message Area", NULL, 0);
   XtManageChild(label_id);

   n = 0;
   XtSetArg(args[n], XmNmessageWindow, label_id); 	n++;
   XtSetValues(main_id, args, n);

   CommonPause();

   XtSetArg(args[n], XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE); n++;
   XtSetValues(main_id, args, n);

   CommonPause();

   XtAppMainLoop(app_context);
}
