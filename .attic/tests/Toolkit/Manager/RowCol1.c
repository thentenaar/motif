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
static char rcsid[] = "$XConsortium: RowCol1.c /main/8 1995/07/13 18:33:07 drk $"
#endif
#endif



#include "testlib.h"

#define WIDTH 50

Widget Shell, RowColumn, Child[6];

static void EntryCallbackProc (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  printf ("The entry callback being called\n");
}

static void die();
static void RunTest();
static void CreateRowColumn();
static void DestroyRowColumn();

void  main(argc, argv)
int argc;
char **argv;
{
  register int  n;
  Arg args[MAX_ARGS];
  
  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);


  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;

  CreateRowColumn(Shell1, args, n);
  CommonPause();
  DestroyRowColumn();
  CreateRowColumn(Shell1, args, n);
  CommonPause();
  DestroyRowColumn();

  n  = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_NONE); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;

  CreateRowColumn(Shell1, args, n);
  CommonPause();
  DestroyRowColumn();
  CreateRowColumn(Shell1, args, n);
  CommonPause();
  DestroyRowColumn();
  
  XtRealizeWidget(Shell1);    
  XtAppMainLoop(app_context);
}

static void CreateRowColumn(Shell1, RCArgList, numRCArgs)
Widget Shell1;
Arg *RCArgList;
int numRCArgs;
{
  register int  n;
  register int  i;
  Arg           ChildArgList[MAX_ARGS];
  Arg           args[MAX_ARGS];
  static  short         flag = 1;
  Dimension width;
  char buf[16];
  
  n = 0;
  XtSetArg(args[n], XmNx, 200);                                  n++;
  XtSetArg(args[n], XmNy, 200);                                  n++;
  XtSetArg(args[n], XmNallowShellResize, True);                  n++;
  Shell = XmCreateDialogShell(Shell1, "RowCol1", args, n);
  
  RowColumn = XmCreateRowColumn(Shell, "RowColumn1", RCArgList, numRCArgs);
  XtAddCallback (RowColumn, XmNentryCallback, EntryCallbackProc, NULL);
  XtManageChild(RowColumn);
  

    { 
      for (i = 0; i <= 5; i++)
	{
          n = 0;
	  sprintf (buf, "Child%d", i);
	  Child[i] = XmCreatePushButton(RowColumn, buf, ChildArgList , n);
	  XtManageChild (Child[i]);
	  flag = 2;
        }
    }
    { 
      for (i = 0; i <= 5; i++)
	{   
	  n = 0;
	  sprintf (buf, "Child%d", i);
	  XtSetArg(ChildArgList[n], XmNwidth, WIDTH); n++;
	  XtSetArg(ChildArgList[n], XmNrecomputeSize, False);  n++;
	  Child[i] = XmCreatePushButton(RowColumn, buf, ChildArgList , n);
	  printf ("Width requested by child %d is %d\n", i, WIDTH);
	  XtManageChild (Child[i]);

	  n = 0;
	  XtSetArg(ChildArgList[n], XmNwidth, &width); n++;
	  XtGetValues (Child[i], ChildArgList, n);
	  printf("Width returned by GetValues for child %d is %d\n", 
		 i, (int)width);
	  flag = 1;
        }
    }
  

  XtRealizeWidget(Shell);
}

static void DestroyRowColumn()
{
  XtDestroyWidget(Shell);
}
