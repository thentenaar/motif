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
static char rcsid[] = "$XConsortium: DrawnBtn2.c /main/8 1995/07/13 17:29:03 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*       Wed Mar 29 15:48:35 1989        *
****************************************/

#include <testlib.h>
#include <signal.h>
#include "dp.bmp"

Widget  RowColumn1 = NULL;
Widget  DrawnButton1 = NULL;
Widget  Separator1 = NULL;
Widget  DrawnButton2 = NULL;
Widget  Separator2 = NULL;
Widget  DrawnButton3 = NULL;
Widget  Separator3 = NULL;
Widget  DrawnButton4 = NULL;
Widget  Separator4 = NULL;

void ReDrawdb4();

Pixmap    nopanic;
GC    	  dbgc;
XGCValues dbgcv;

/************ Call Back for resize events *************/

void ReSizeCB( w, client_data, call_data)
Widget  w;
XtPointer client_data; 
XtPointer call_data;
{
  Arg    wargs[MAX_ARGS];
  int n;
  Dimension w_width, w_height;

  /* Get new size of widget */
  n=0;
  XtSetArg(wargs[n],XmNwidth,&w_width); n++;
  XtSetArg(wargs[n],XmNheight,&w_height); n++;
  XtGetValues(w,wargs,n);

  printf("Resized width & height is %d, %d\n",w_width, w_height);
}

/*********************** MAIN STARTS HERE **************************/
void  main (argc, argv)
int     argc;
char  **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           tcs;
  XEvent             DummyEvent;

  XtArgVal  getValue();
  void      quit();

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 400);  n++;
  XtSetValues(Shell1, args, n);
  

  XtRealizeWidget(Shell1);


  n = 0;
  XtSetArg(args[n], XmNwidth, 160);  n++;
  XtSetArg(args[n], XmNheight, 400);  n++;
  XtSetArg(args[n], XmNmarginWidth, 5);  n++;
  XtSetArg(args[n], XmNmarginHeight, 5);  n++;
  RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);

  n = 0;
  XtSetArg(args[n], XmNwidth, 150);  n++;
  XtSetArg(args[n], XmNheight, 25);  n++;
  tcs = XmStringLtoRCreate("Drawn_Button", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNpushButtonEnabled, TRUE);  n++;
  DrawnButton1 = XmCreateDrawnButton(RowColumn1, "DrawnButton1", args, n);
  XtManageChild(DrawnButton1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNseparatorType, XmDOUBLE_DASHED_LINE);  n++;
  Separator1 = XmCreateSeparator(RowColumn1, "Separator1", args, n);
  XtManageChild(Separator1);
  
  n = 0;
  XtSetArg(args[n], XmNwidth, 150);  n++;
  XtSetArg(args[n], XmNheight, 25);  n++;
  tcs = XmStringLtoRCreate("Drawn_Button", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNpushButtonEnabled, FALSE);  n++;
  DrawnButton2 = XmCreateDrawnButton(RowColumn1, "DrawnButton2", args, n);
  XtManageChild(DrawnButton2);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNseparatorType, XmDOUBLE_DASHED_LINE);  n++;
  Separator2 = XmCreateSeparator(RowColumn1, "Separator2", args, n);
  XtManageChild(Separator2);

  n = 0;
  XtSetArg(args[n], XmNwidth, 150);  n++;
  XtSetArg(args[n], XmNheight, 25);  n++;
  tcs = XmStringLtoRCreate("Drawn_Button", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNpushButtonEnabled, FALSE);  n++;
  XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);  n++;
  DrawnButton3 = XmCreateDrawnButton(RowColumn1, "DrawnButton3", args, n);
  XtManageChild(DrawnButton3);
  XmStringFree(tcs);

  /* add resize callback */
  XtAddCallback(DrawnButton1,XmNresizeCallback,ReSizeCB,NULL);
  XtManageChild(RowColumn1); 

  CommonPause();
  CommonPause();
  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(DrawnButton1);
  XtDestroyWidget(DrawnButton2);
  XtDestroyWidget(DrawnButton3);
  XtDestroyWidget(Separator1);
  XtDestroyWidget(Separator2);
}

