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
static char rcsid[] = "$XConsortium: BBoard8.c /main/4 1995/07/13 18:27:35 drk $"
#endif
#endif
    
#include <testlib.h>

Widget option;
    
static int caseNumber = 0;

static void DoCase(w)
Widget w;
{
    switch(caseNumber)  {
      case 0:
	printf("Add one\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Add one", NULL, 0));
	break;
      case 1:
	printf("Add Two\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Add Two", NULL, 0));
	break;
      case 2:
	printf("Unmanage Option Label\n");
	XtUnmanageChild(XmOptionLabelGadget(option));
	break;
      case 3:
	printf("Add Three\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Add Three", NULL, 0));
	break;
      case 4:
	printf("Add four\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Add four", NULL, 0));
	break;
      case 5:
	printf("Adding five\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Adding five", NULL, 0));
	break;
      case 6:
	printf("Add six\n");
	XtManageChild(XmCreatePushButtonGadget(w, "Add six", NULL, 0));
	break;
      defaults:
	exit(0);
    }	
    caseNumber++;
}


main(argc, argv)
int argc;
char *argv[];
{
    Widget BulletinBoard1, Pulldown1, pb[3];
    Arg args[MAX_ARGS];
    int n, i;
    char str[8];

    CommonTestInit(argc, argv);
        
    n=0;
    XtSetArg(args[n],XmNallowShellResize, True);	n++;
    XtSetArg(args[n],XmNwidth, 400); 			n++;
    XtSetArg(args[n],XmNheight, 300); 			n++;
    XtSetValues(Shell1, args, n);
    
    n=0;
    BulletinBoard1 = XmCreateBulletinBoard(Shell1, "BulletinBoard1", args, n);
    
    n=0;
    Pulldown1 = XmCreatePulldownMenu(BulletinBoard1, "Pulldown1", args, n);
    for (i = 0; i < 3; i++)  {
	n = 0;
	sprintf(str, "PUSH%d", i+1);
	pb[i] = XmCreatePushButtonGadget(Pulldown1, str, args, n);
    }	
    XtManageChildren(pb, 3);
    
    n=0;
    XtSetArg(args[n], XmNsubMenuId, Pulldown1);		n++;
    option = XmCreateOptionMenu(BulletinBoard1, "option", args, n);
    
    XtManageChild(BulletinBoard1);
    XtManageChild(option);

    XtRealizeWidget(Shell1);

    for(i=0; i<7; i++)  {
	DoCase(Pulldown1);
	CommonPause();
    }

    CommonPause();

    XtAppMainLoop(app_context);
}
