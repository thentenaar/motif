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
static char rcsid[] = "$XConsortium: SeparGad1.c /main/8 1995/07/13 19:09:55 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.4              *
*       Fri Jun 30 13:18:18 1989        *
****************************************/


#include <testlib.h>

void  main(argc, argv)
int argc;
char **argv;
{
  Widget BBoard, Label1, Label2, Separator1, Separator2, Separator3;
  Widget Separator4, Label3, Label4;
  register int n;
  Arg args[MAX_ARGS];
  Dimension separator_margin;
  char trace_string[128];
  XmString tcs;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++; 
  XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
  XtSetValues(Shell1, args, n);
  XtRealizeWidget(Shell1);

  n = 0;
  BBoard = XmCreateBulletinBoard(Shell1, "BBoard", args, n);
  XtManageChild(BBoard);

  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  Label1 = XmCreateLabel(BBoard, "Label1", args, n);
  XtManageChild(Label1);
  
  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 30); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  Separator1 = XmCreateSeparatorGadget(BBoard, "Separator1", args, n);
  XtManageChild(Separator1);
  
  n = 0;
  XtSetArg(args[n], XmNmargin, &separator_margin); n++;
  XtGetValues(Separator1, args, n);
  sprintf(trace_string, "XtGetValues: Separator1 = %d\n\n",
	  separator_margin);

  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 70); n++;
  Label2 = XmCreateLabel(BBoard, "Label2", args, n);
  XtManageChild(Label2);
  
  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 90); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  Separator2 = XmCreateSeparatorGadget(BBoard, "Separator2", args, n);
  XtManageChild(Separator2);
  
  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 130); n++;
  Label3 = XmCreateLabel(BBoard, "Label3", args, n);
  XtManageChild(Label3);

  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 150); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  Separator3 = XmCreateSeparatorGadget(BBoard, "Separator3", args, n);
  XtManageChild(Separator3);
  
  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 190); n++;
  Label4 = XmCreateLabel(BBoard, "Label4", args, n);
  XtManageChild(Label4);

  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  Separator4 = XmCreateSeparatorGadget(BBoard, "Separator4", args, n);
  XtManageChild(Separator4);

  CommonPause();		

  n = 0;
  tcs = XmStringCreateLtoR("Shadow Etched Out", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetValues(Label1, args, n);
  XmStringFree (tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 100);  n++;
  XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);  n++;
  XtSetArg(args[n], XmNmargin, 25);  n++;
  XtSetValues(Separator1, args, n);

  n = 0;
  tcs = XmStringCreateLtoR("Double Line", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetValues(Label2, args, n);
  XmStringFree (tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 100);  n++;
  XtSetArg(args[n], XmNseparatorType, XmDOUBLE_LINE);  n++;
  XtSetValues(Separator2, args, n);

  n = 0;
  tcs = XmStringCreateLtoR("Double Dashed Line", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetValues(Label3, args, n);
  XmStringFree (tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 100);  n++;
  XtSetArg(args[n], XmNseparatorType, XmDOUBLE_DASHED_LINE);  n++;
  XtSetValues(Separator3, args, n);

  n = 0;
  tcs = XmStringCreateLtoR("Single Dashed Line", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetValues(Label4, args, n);
  XmStringFree (tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 100);  n++; 
  XtSetArg(args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);  n++;
  XtSetValues(Separator4, args, n);
  CommonPause();


  n = 0;
  tcs = XmStringCreateLtoR("Shadow Etched In", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetValues(Label1, args, n);
  XmStringFree (tcs);

  n = 0;
  XtSetArg(args[n], XmNwidth, 2); n++;
  XtSetArg(args[n], XmNmargin, 0); n++;
  XtSetArg(args[n], XmNheight, 40); n++;
  XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN);  n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
  XtSetValues(Separator1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNmargin, 15);  n++;
  XtSetArg(args[n], XmNheight, 6); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
  XtSetValues(Separator1, args, n);
  XtSetValues(Separator2, args, n);
  XtSetValues(Separator3, args, n);
  XtSetValues(Separator4, args, n);
  CommonPause();

  XtDestroyWidget(Label1);
  XtDestroyWidget(Label2);
  XtDestroyWidget(Label3);
  XtDestroyWidget(Label4);
  XtDestroyWidget(Separator1);
  XtDestroyWidget(Separator2);
  XtDestroyWidget(Separator3);
  XtDestroyWidget(Separator4);
  XtDestroyWidget(BBoard);
  CommonPause();
  XtAppMainLoop(app_context);
}
