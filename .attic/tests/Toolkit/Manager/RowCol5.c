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
static char rcsid[] = "$XConsortium: RowCol5.c /main/6 1995/07/13 18:33:55 drk $"
#endif
#endif



#include <testlib.h>

#define MAXKID 10

Widget BBoard;
Widget RowColumn;
Widget Kid[MAXKID], newKid1, newKid2;

static void CreateRowColumn();

void  main(argc, argv)
int argc;
char **argv;
{
  register int  n;
  register int i;
  Arg args[MAX_ARGS];
  char label[80];
    
  CommonTestInit(argc, argv);

  CreateRowColumn(Shell1);
  XtRealizeWidget(Shell1);    

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, XmLAST_POSITION); n++;
  for (i = MAXKID - 2; i >= 0; i--)
    XtSetValues(Kid[i], args, n);
    
  CommonPause();
	

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 3); n++;
  newKid1 = XmCreatePushButton(RowColumn, "NewKid1", args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 9); n++;
  newKid2 = XmCreatePushButton(RowColumn, "NewKid2", args, n);
  CommonPause();
    
  
  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 0); n++;
  XtSetValues(newKid1, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, 1); n++;
  XtSetValues(newKid2, args, n);

  n = 0;
  XtSetArg(args[n], XmNpositionIndex, XmLAST_POSITION); n++;
  XtSetValues(Kid[5], args, n);

  CommonPause();

  XtDestroyWidget(RowColumn);
  
  CommonPause();

  XtAppMainLoop(app_context);
}


static void CreateRowColumn(Widget Shell1)
{
  register int i;
  register int n;
  Arg args[MAX_ARGS];
  char label[80];


  n = 0;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  BBoard = XmCreateBulletinBoardDialog(Shell1, "RowCol3", args, n);

  n = 0;
  XtSetArg(args[n], XmNpacking,    XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 30); n++;
  XtSetArg (args[n], XmNtraversalOn, True); n++;
  RowColumn = XmCreateRowColumn(BBoard, "RowColumn", args, n);

  n = 0;  
  for (i = 0; i < MAXKID; i++) {
    sprintf(label, "Child%d", i);
    Kid[i] = XmCreatePushButton(RowColumn, label, args, n);
  }
  
  XtManageChildren(Kid, MAXKID);
  
  XtManageChild(RowColumn);
  XtManageChild(BBoard);
}






