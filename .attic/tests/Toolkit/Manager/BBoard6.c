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
static char rcsid[] = "$XConsortium: BBoard6.c /main/8 1995/07/13 18:26:57 drk $"
#endif
#endif



#include <testlib.h>

Widget         Shell;
Widget         BulletinBoard;
Widget         Child1;
Widget         Child2;
Widget         Child3;
Widget         Child4;

static void            die();
static void            RunTest();
static void            CreateBulletinBoard();
static void            DestroyBulletinBoard();

void  main(argc, argv)
int     argc;
char **argv;
{
  register int  n;
  Arg           args[MAX_ARGS];
  
  CommonTestInit(argc, argv);

  RunTest(Shell1);

  XtRealizeWidget(Shell1);    
  XtAppMainLoop(app_context);
}


static void RunTest(shell)
Widget shell;
{
  register int  i;
  register int  n;
  Arg           args[MAX_ARGS];
  Dimension  mw; 
    
    CreateBulletinBoard(shell);
    
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNx, 0);                                    n++;
    XtSetArg(args[n], XmNy, 0);                                    n++;
    XtSetValues(Child1, args, n);
        
    n = 0;
    XtSetArg(args[n], XmNx, 200);                                  n++;
    XtSetArg(args[n], XmNy, 0);                                    n++;
    XtSetValues(Child2, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNx, 0);                                    n++;
    XtSetArg(args[n], XmNy, 200);                                  n++;
    XtSetValues(Child3, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNx, 200);                                  n++;
    XtSetArg(args[n], XmNy, 200);                                  n++;
    XtSetValues(Child4, args, n);
    
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);                              n++;
    XtSetArg(args[n], XmNheight, 100);                             n++;
    XtSetValues(Child1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);                              n++;
    XtSetArg(args[n], XmNheight, 100);                             n++;
    XtSetValues(Child2, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);                              n++;
    XtSetArg(args[n], XmNheight, 100);                             n++;
    XtSetValues(Child3, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100);                              n++;
    XtSetArg(args[n], XmNheight, 100);                             n++;
    XtSetValues(Child4, args, n);
    
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 200);                              n++;
    XtSetArg(args[n], XmNheight, 200);                             n++;
    XtSetValues(Child1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 200);                              n++;
    XtSetArg(args[n], XmNheight, 200);                             n++;
    XtSetValues(Child2, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 200);                              n++;
    XtSetArg(args[n], XmNheight, 200);                             n++;
    XtSetValues(Child3, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 200);                              n++;
    XtSetArg(args[n], XmNheight, 200);                             n++;
    XtSetValues(Child4, args, n);
    
    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNwidth, 300);                              n++;
    XtSetArg(args[n], XmNheight, 300);                             n++;
    XtSetValues(Child1, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 300);                              n++;
    XtSetArg(args[n], XmNheight, 300);                             n++;
    XtSetValues(Child2, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 300);                              n++;
    XtSetArg(args[n], XmNheight, 300);                             n++;
    XtSetValues(Child3, args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 300);                              n++;
    XtSetArg(args[n], XmNheight, 300);                             n++;
    XtSetValues(Child4, args, n);

    n = 0;
    XtSetArg (args[n], XmNmarginWidth, &mw); n++;
    XtGetValues (BulletinBoard, args, n);
    printf ("Margin width of bulletin board is %d\n", (int) mw);
    fflush (stdout);
    
    CommonPause();

    DestroyBulletinBoard();
    
}


static void CreateBulletinBoard(parent)
Widget parent;
{
  register int  n;
  Arg           args[MAX_ARGS];
  
  n = 0;
  XtSetArg(args[n], XmNx, 200);                                  n++;
  XtSetArg(args[n], XmNy, 200);                                  n++;
  XtSetArg(args[n], XmNallowShellResize, True);                  n++;
  Shell = XmCreateDialogShell(parent, "BBoard6", args, n);
  
  n = 0;
  XtSetArg(args[n], XmNautoUnmanage, False);                     n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY);              n++;
  XtSetArg(args[n], XmNallowOverlap, True);                      n++;
  BulletinBoard = XmCreateBulletinBoard(Shell, "BBoard", args, n);
  
  n = 0;
  XtSetArg(args[n], XmNx, 0);                                    n++;
  XtSetArg(args[n], XmNy, 0);                                    n++;
  Child1 = XmCreatePushButton(BulletinBoard, "Child1", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 100);                                  n++;
  XtSetArg(args[n], XmNy, 0);                                    n++;
  Child2 = XmCreatePushButton(BulletinBoard, "Child2", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 0);                                    n++;
  XtSetArg(args[n], XmNy, 100);                                  n++;
  Child3 = XmCreatePushButton(BulletinBoard, "Child3", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 100);                                  n++;
  XtSetArg(args[n], XmNy, 100);                                  n++;
  Child4 = XmCreatePushButton(BulletinBoard, "Child4", args, n);
  
  XtManageChild(Child1);
  XtManageChild(Child2);
  XtManageChild(Child3);
  XtManageChild(Child4);
  
  XtManageChild(BulletinBoard);
  XtRealizeWidget(Shell);
}

static void DestroyBulletinBoard()
{
  XtDestroyWidget(Shell);
}

