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
static char rcsid[] = "$XConsortium: PanedWin8.c /main/4 1995/07/13 19:07:48 drk $"
#endif
#endif


static void DoXY ();

#define	BUTTONS		3
#define B_X		50
#define B_Y		50
#include <testlib.h>

/* Test what happens when initial X and Y of children are set and 
   when XmNseparatorOn is set false. */

void  main(argc, argv)
int     argc;
char  **argv;
{
    Boolean       	trace = False;
    register int  	n, i;
    Arg			args[MAX_ARGS];

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNgeometry, "150x150+0+0");  n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);

    for (i = 0; i < BUTTONS; i++) 
      {
	DoXY(i);
      }
    
    CommonPause();
   
    XtAppMainLoop(app_context);
}

static void DoXY(i)
int i;
{
  register int  	n, j;
  Arg			args[MAX_ARGS];
  Widget		PW, PB;

  /* Set X. */
  n = 0;
  XtSetArg(args[n], XmNseparatorOn, False); n++; 
  PW = XmCreatePanedWindow(Shell1, "PW", args, n);
  
  for (j = 0; j < BUTTONS; j++) 
    {
      n = 0;
      PB = XmCreatePushButton(PW, "PB1", args, n);
      XtManageChild(PB);
    }

  XtManageChild(PW);

  CommonPause();
  
  XtDestroyWidget(PW);
  
  /* Set Y. */
  n = 0;
  PW = XmCreatePanedWindow(Shell1, "PW", args, n);
  
  for (j = 0; j < BUTTONS; j++)
    {
      n = 0;
      
      if (j == i) 
	{
	  XtSetArg(args[n], XmNy, B_Y); n++;  
	}
      
      PB = XmCreatePushButton(PW, "PB1", args, n);

      XtManageChild(PB);
    }

  XtManageChild(PW);
  
  CommonPause();
  
  XtDestroyWidget(PW);
  
  /* Set both. */
  n = 0;
  PW = XmCreatePanedWindow(Shell1, "PW", args, n);
  
  for (j = 0; j < BUTTONS; j++)
    {
      n = 0;

      if (j == 1) 
	{
	  XtSetArg(args[n], XmNx, B_X); n++;  
	  XtSetArg(args[n], XmNy, B_Y); n++;  
	}
      
      PB = XmCreatePushButton(PW, "PB1", args, n);

      XtManageChild(PB);
    }
  
  XtManageChild(PW);

  CommonPause();
  
  XtDestroyWidget(PW);
}

static void ReportSize (w)
Widget w;
{
  register int	n;
  Arg		args[2];
  Dimension	width, height;
    
  n = 0;
  XtSetArg(args[n], XmNwidth, &width); n++;
  XtSetArg(args[n], XmNheight, &height); n++;
  XtGetValues(w, args, n);
  
  printf("%dX%d\n", (int)width, (int)height);
}

