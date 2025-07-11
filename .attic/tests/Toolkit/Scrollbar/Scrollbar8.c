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
static char rcsid[] = "$TOG: Scrollbar8.c /main/7 1999/01/15 12:36:34 jff $"
#endif
#endif

/************************************************************************
 *
 *  Create a vertical scrollbar and test out the XmGetScrollBarValues
 *  XmSetScrollBarValues external functions.
 *
 ************************************************************************/

#include <X11/StringDefs.h>
#include <testlib.h>

extern void ScrollCallback();

void main(argc, argv)
unsigned int argc;
char **argv;
{
  Widget frame;
  Widget scrollbar;
  Arg args[10];
  int n;
  int i;
  int value, slider_size, increment, page_increment;
  Boolean notify = False;

  CommonTestInit(argc, argv);
    
  frame = XmCreateFrame(Shell1, "frame", NULL, 0);
  XtManageChild(frame);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);		n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_TOP);	n++;
  XtSetArg(args[n], XmNheight, 250);				n++;
  XtSetArg(args[n], XmNwidth, 50);				n++;
  scrollbar = XmCreateScrollBar(frame, "scrollbar", args, n);
  XtManageChild(scrollbar);

  XtAddCallback(scrollbar, XmNvalueChangedCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar, XmNhelpCallback,          ScrollCallback, NULL);

  XtRealizeWidget(Shell1);

  value = 0;
  slider_size = 10;
  increment = 1;
  page_increment = 10;

  for(i = 0; i < 5; i ++)
    {
      XmScrollBarSetValues(scrollbar, value, slider_size, 
			   increment, page_increment, notify);
      if(notify == False) notify = True;
      else notify = False;
      XmScrollBarGetValues(scrollbar, &value, &slider_size, 
                            &increment, &page_increment);
      printf("value = %d,  slider_size = %d,  incr = %d,  page_incr = %d\n\n",
              value, slider_size, increment, page_increment);
      value += 10;
      slider_size += 10;
      increment += 4;
      page_increment += 5;
      CommonPause();
    }

  XtDestroyWidget(scrollbar);
  XtDestroyWidget(frame);

  CommonPause();

  XtAppMainLoop(app_context);
}

void ScrollCallback(sbw, closure, call_data)
XmScrollBarWidget sbw;
XtPointer closure;
XtPointer call_data;
{
   int value;
   int reason;
   int pixel;
   Arg args[10];
   int n;


   XmScrollBarCallbackStruct * call_value = 
     (XmScrollBarCallbackStruct *) call_data;

   reason = call_value -> reason;
   value = call_value -> value;
   pixel = call_value -> pixel;


   switch(reason)
   {
      case XmCR_VALUE_CHANGED:
         printf("Reason = XmCR_VALUE_CHANGED,	Value = %d", value);
      break;

      case XmCR_HELP:
         printf("Reason = XmCR_HELP,	Value = %d\n", value);
      break;

      default:
         printf("Hit the default, incorrect reason sent!!\n");
      break;

   }

   n = 0;
   XtSetArg(args[n], XmNvalue, &value);	n++;
   XtGetValues((Widget) sbw, args, n);
   printf(",   GetValues value = %d\n", value);
}
