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
static char rcsid[] = "$XConsortium: RowCol3.c /main/8 1995/07/13 18:33:28 drk $"
#endif
#endif



#include <testlib.h>

#ifndef MAXKID
#define MAXKID 20
#endif

Widget             BBoard;
Widget          RowColumn;
Widget        Kid[MAXKID];

static void            die();
static void            RunTest();
static void            CreateRowColumn();
static void            DestroyRowColumn();
static void	       PrintWidgetSize();

void  main(argc, argv)
int     argc;
char  **argv;
{
  register int  n;
  Arg           args[MAX_ARGS];
  
  
  CommonTestInit(argc, argv);

  RunTest(Shell1);

  XtRealizeWidget(Shell1);    
  XtAppMainLoop(app_context);
}

static void RunTest(Shell1)
Widget Shell1;
{
  register int  i;
  register int  n;
  Arg           args[MAX_ARGS];
  char          label[80];
  
    
    CreateRowColumn(Shell1);
    
    CommonPause();
    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);                  n++;
    XtSetValues(RowColumn, args, n);
    
    for (i = 0; i < MAXKID; i++) {
      n = 0;
      XtSetArg(args[n], XmNx,  100);                              n++;
      XtSetValues(Kid[i], args, n);
    }
   
    PrintWidgetSize(RowColumn); 
    CommonPause();
    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);                 n++;
    XtSetArg(args[n], XmNnumColumns, 4);                          n++;
    XtSetValues(RowColumn, args, n);
    
    for (i = 0; i < MAXKID; i++) {
      n = 0;
      XtSetArg(args[n], XmNwidth,  100);                          n++;
      XtSetArg(args[n], XmNheight, 100);                          n++;
      XtSetValues(Kid[i], args, n);
    }
    
    PrintWidgetSize(RowColumn); 
    CommonPause();
    
    
    DestroyRowColumn();
  
}

static void CreateRowColumn(Shell1)
Widget Shell1;
{
  register int  i;
  register int  n;
  Arg           args[MAX_ARGS];
  char          label[80];

  n = 0;
  XtSetArg(args[n], XmNdefaultPosition, False);                  n++;
  BBoard = XmCreateBulletinBoardDialog(Shell1, "RowCol3", args, n);

  n = 0;
  XtSetArg(args[n], XmNpacking,    XmPACK_COLUMN);               n++;
  XtSetArg(args[n], XmNnumColumns, 30);                          n++;
  XtSetArg (args[n], XmNtraversalOn, True); 			 n++;
  RowColumn = XmCreateRowColumn(BBoard, "RowColumn", args, n);
  
  for (i = 0; i < MAXKID; i++) {
    sprintf(label, "Child%d", i);
    Kid[i] = XmCreatePushButton(RowColumn, label, NULL, 0);
  }
  
  XtManageChildren(Kid, MAXKID);
  
  XtManageChild(RowColumn);
  XtManageChild(BBoard);
  PrintWidgetSize(BBoard); 
}

static void DestroyRowColumn()
{
  XtDestroyWidget(RowColumn);
}






static void PrintWidgetSize (widget)
Widget widget;

{	
	Dimension w, h;
 	Arg args[2];

	XtSetArg (args[0], XmNwidth, &w); 
	XtSetArg (args[1], XmNheight, &h);
	XtGetValues (widget, args, 2);

	printf ("%s    width: %d, height: %d\n", XtName (widget), 
						 (int) w, (int) h);

}








