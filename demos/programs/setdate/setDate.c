/* $XConsortium: setDate.c /main/6 1996/11/08 16:40:07 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef CSRG_BASED
#include <langinfo.h>
#endif
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/SpinB.h>
#include <Xm/TextF.h>

XtAppContext context;
#define APP_CLASS "XmdSetDate"

struct tm *thetime;
time_t timer;

Widget action, exitpb;
void ValueChanged(Widget, XtPointer, XtPointer);
void QuitAppl(Widget, XtPointer, XtPointer);
void Doit(Widget, XtPointer, XtPointer);

char * fallback_resources[] = {
"*SpinBoxs.orientation: XmHORIZONTAL",
"*SpinBoxs.packing:	XmPACK_TIGHT",
"*label2.labelString:	Date:",
"*label3.labelString:	Time:",
"*XmSpinBox.alignment:		XmALIGNMENT_END",
"*XmSpinBox.arrowLayout:	XmARROWS_SPLIT",
"*dateCommand:		echo %s",
NULL
};

int dy, yr, mn,  hr, mi;
Widget day, month, year, hour, minute;
char *dateCommand = NULL;

int main(int argc, char **argv)
{
  Widget        shell, panedw, rc, rc2, sb1, sb2;
  Widget	label, label2;
  Arg args[20];
  int argcount;
  int i;
  XmString months[12];
  char *monthstr[12];
  char buf[10];
  char *str;

  shell     = XtVaAppInitialize(&context, APP_CLASS, NULL, 0, &argc, argv,
				fallback_resources, NULL);

  time(&timer);
  thetime = localtime(&timer);
  dy = thetime -> tm_mday;
  yr = thetime -> tm_year + 1900;
  mn = thetime -> tm_mon;
  hr = thetime -> tm_hour;
  mi = thetime -> tm_min;
  
  panedw    = XmCreatePanedWindow(shell, "BBoard", NULL, 0);
  XtManageChild(panedw);

  /* 
  argcount = 0;
  XtSetArg(args[argcount], XmNlabelType, XmSTRING), argcount++;
  XtSetArg(args[argcount], XmNlabelString,
	   XmStringCreate("Set Current Date and Time", 
			  XmSTRING_DEFAULT_CHARSET)), argcount++;
  label = XmCreateLabel(panedw, "label1", args, argcount);
  XtManageChild(label);
  */

  str = XGetDefault(XtDisplay(panedw), APP_CLASS, "dateCommand");
  if (str == NULL)
    str = XGetDefault(XtDisplay(panedw), argv[0], "dateCommand");
  if (str == NULL)
    dateCommand = "echo Default command, the date is: %s";
  else
    dateCommand = str;

  rc = XmCreateRowColumn(panedw, "SpinBoxs", NULL, 0);
  XtManageChild(rc);
  
  label = XmCreateLabel(rc, "label2", NULL, 0);
  XtManageChild(label);
  
  sb1 = XmCreateSpinBox(rc, "DateSB", NULL, 0);
  XtAddCallback(sb1, XmNvalueChangedCallback, ValueChanged, NULL);
  XtManageChild(sb1);

  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNspinBoxChildType, XmNUMERIC), argcount++;
  XtSetArg(args[argcount], XmNmaximumValue, 31), argcount++;
  XtSetArg(args[argcount], XmNminimumValue, 1), argcount++;
  XtSetArg(args[argcount], XmNposition, dy - 1), argcount++;
  XtSetArg(args[argcount], XmNcolumns, 3), argcount++;
  day = XmCreateTextField(sb1, "Day", args, argcount);
  XtManageChild(day);
   
#ifndef CSRG_BASED
  months[0]  = XmStringCreate(nl_langinfo(MON_1), XmSTRING_DEFAULT_CHARSET);
  months[1]  = XmStringCreate(nl_langinfo(MON_2), XmSTRING_DEFAULT_CHARSET);
  months[2]  = XmStringCreate(nl_langinfo(MON_3), XmSTRING_DEFAULT_CHARSET);
  months[3]  = XmStringCreate(nl_langinfo(MON_4), XmSTRING_DEFAULT_CHARSET);
  months[4]  = XmStringCreate(nl_langinfo(MON_5), XmSTRING_DEFAULT_CHARSET);
  months[5]  = XmStringCreate(nl_langinfo(MON_6), XmSTRING_DEFAULT_CHARSET);
  months[6]  = XmStringCreate(nl_langinfo(MON_7), XmSTRING_DEFAULT_CHARSET);
  months[7]  = XmStringCreate(nl_langinfo(MON_8), XmSTRING_DEFAULT_CHARSET);
  months[8]  = XmStringCreate(nl_langinfo(MON_9), XmSTRING_DEFAULT_CHARSET);
  months[9]  = XmStringCreate(nl_langinfo(MON_10), XmSTRING_DEFAULT_CHARSET);
  months[10]  = XmStringCreate(nl_langinfo(MON_11), XmSTRING_DEFAULT_CHARSET);
  months[11]  = XmStringCreate(nl_langinfo(MON_12), XmSTRING_DEFAULT_CHARSET);
#else
  months[0]  = XmStringCreate("January", XmSTRING_DEFAULT_CHARSET);
  months[1]  = XmStringCreate("February", XmSTRING_DEFAULT_CHARSET);
  months[2]  = XmStringCreate("March", XmSTRING_DEFAULT_CHARSET);
  months[3]  = XmStringCreate("April", XmSTRING_DEFAULT_CHARSET);
  months[4]  = XmStringCreate("May", XmSTRING_DEFAULT_CHARSET);
  months[5]  = XmStringCreate("June", XmSTRING_DEFAULT_CHARSET);
  months[6]  = XmStringCreate("July", XmSTRING_DEFAULT_CHARSET);
  months[7]  = XmStringCreate("August", XmSTRING_DEFAULT_CHARSET);
  months[8]  = XmStringCreate("September", XmSTRING_DEFAULT_CHARSET);
  months[9]  = XmStringCreate("October", XmSTRING_DEFAULT_CHARSET);
  months[10]  = XmStringCreate("November", XmSTRING_DEFAULT_CHARSET);
  months[11]  = XmStringCreate("December", XmSTRING_DEFAULT_CHARSET);
#endif

  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNvalues, months), argcount++;
  XtSetArg(args[argcount], XmNnumValues, 12), argcount++;
  XtSetArg(args[argcount], XmNposition, mn), argcount++;
  XtSetArg(args[argcount], XmNcolumns, 12), argcount++;
  month = XmCreateTextField(sb1, "MonthSB", args, argcount);
  XtManageChild(month);

  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNspinBoxChildType, XmNUMERIC), argcount++;
  XtSetArg(args[argcount], XmNmaximumValue, 2020), argcount++;
  XtSetArg(args[argcount], XmNminimumValue, 1992), argcount++;
  XtSetArg(args[argcount], XmNposition, yr-1992), argcount++;
  XtSetArg(args[argcount], XmNcolumns, 5), argcount++;
  year = XmCreateTextField(sb1, "YearSB", args, argcount);
  XtManageChild(year);

  argcount = 0;
  label = XmCreateLabel(rc, "label3", args, argcount);
  XtManageChild(label);
  
  argcount = 0;
  XtSetArg(args[argcount], XmNy, 50), argcount++;
  sb2 = XmCreateSpinBox(rc, "TimeSB", args, argcount);
  XtAddCallback(sb2, XmNvalueChangedCallback, ValueChanged, NULL);
  XtManageChild(sb2);

  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNspinBoxChildType, XmNUMERIC), argcount++;
  XtSetArg(args[argcount], XmNmaximumValue, 23), argcount++;
  XtSetArg(args[argcount], XmNminimumValue, 0), argcount++;
  XtSetArg(args[argcount], XmNposition, hr), argcount++;
  XtSetArg(args[argcount], XmNcolumns, 3), argcount++;
  hour = XmCreateTextField(sb2, "Hour", args, argcount);
  XtManageChild(hour);
  
  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNlabelType, XmSTRING), argcount++;
  XtSetArg(args[argcount], XmNlabelString,
	   XmStringCreate(":", 
			  XmSTRING_DEFAULT_CHARSET)), argcount++;
  label = XmCreateLabel(sb2, "label4", args, argcount);
  XtManageChild(label);
   
  argcount = 0;
  XtSetArg(args[argcount], XmNpositionType, XmPOSITION_INDEX), argcount++;
  XtSetArg(args[argcount], XmNspinBoxChildType, XmNUMERIC), argcount++;
  XtSetArg(args[argcount], XmNmaximumValue, 59), argcount++;
  XtSetArg(args[argcount], XmNminimumValue, 0), argcount++;
  XtSetArg(args[argcount], XmNposition, mi), argcount++;
  XtSetArg(args[argcount], XmNcolumns, 3), argcount++;
  minute = XmCreateTextField(sb2, "Minute", args, argcount);
  XtManageChild(minute);

  argcount = 0;
  XtSetArg(args[argcount], XmNorientation, XmHORIZONTAL), argcount++;
  XtSetArg(args[argcount], XmNpacking, XmPACK_TIGHT), argcount++;
  rc2 = XmCreateRowColumn(panedw, "SpinBoxs2", args, argcount);
  XtManageChild(rc2);

  argcount = 0;
  action = XmCreatePushButton(rc2, "SetDate", args, argcount);
  XtManageChild(action);
  XtAddCallback(action, XmNactivateCallback, Doit, "OK");

  argcount = 0;
  exitpb = XmCreatePushButton(rc2, "Quit", args, argcount);
  XtManageChild(exitpb);
  XtAddCallback(exitpb, XmNactivateCallback, QuitAppl, "OK");

  XtRealizeWidget(shell);

  XtAppMainLoop(context);

  return 0;    /* make compiler happy */
}

void ValueChanged(Widget w, XtPointer client_data, XtPointer data)
{
XmSpinBoxCallbackStruct *call;

  call = (XmSpinBoxCallbackStruct *)data;

  if (call->widget == day)
    dy = call->position + 1;
  else if (call->widget == month)
    mn = call->position;
  else if (call->widget == year)
    yr = call->position + 1992;
  else if (call->widget == hour)
    hr = call->position;
  else if (call->widget == minute)
    mi = call->position;

}

void QuitAppl(Widget w, XtPointer i, XtPointer data)
{
  XmPushButtonCallbackStruct *e = (XmPushButtonCallbackStruct *) data;
  exit(0);
}

void Doit(Widget w, XtPointer client_data, XtPointer data)
{
  XmPushButtonCallbackStruct *e = (XmPushButtonCallbackStruct *) data;
  char buffer[80];
  char date[32];

  sprintf(date, "%02d%02d%02d%02d%02d", (mn + 1), dy, 
	  hr, mi, yr - (100 * (yr / 100)));
  sprintf(buffer, dateCommand, date);
  system(buffer);
}


