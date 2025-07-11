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
static char rcsid[] = "$XConsortium: Scrollbar5.c /main/6 1995/07/13 19:15:09 drk $"
#endif
#endif

/************************************************************************
 *
 *  Create a scrollbar with a full set of callbacks.
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

   CommonTestInit(argc, argv);
    
   frame = 
      XmCreateFrame(Shell1, "frame", NULL, 0);
   XtManageChild(frame);
   n = 0;

   XtSetArg(args[n], XmNwidth, 50); n++;
   XtSetArg(args[n], XmNheight, 250); n++;
   scrollbar = XmCreateScrollBar(frame, "scrollbar", args, n);
   XtManageChild(scrollbar);

   XtAddCallback(scrollbar, XmNvalueChangedCallback,  ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNincrementCallback,     ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNdecrementCallback,     ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNpageIncrementCallback, ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNpageDecrementCallback, ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNtoTopCallback,         ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNtoBottomCallback,      ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNdragCallback,          ScrollCallback, NULL);
   XtAddCallback(scrollbar, XmNhelpCallback,          ScrollCallback, NULL);

   XtRealizeWidget(Shell1);

   CommonPause();
   CommonPause();

   /* Change Orientation to Horizontal and resume testing */

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNwidth, 250); n++;
   XtSetArg(args[n], XmNheight, 50); n++;
   XtSetArg(args[n] , XmNvalue, 0); n++;
   XtSetValues(scrollbar, args, n);

   CommonPause();
   CommonPause();

   XtDestroyWidget(scrollbar);
   XtDestroyWidget(frame);
   CommonPause();
   XtAppMainLoop(app_context);
}

void ScrollCallback(sbw, closure, call_data)
XmScrollBarWidget sbw;
caddr_t closure;
caddr_t call_data;
{
  int value;
  int reason;
  int pixel;

  XmScrollBarCallbackStruct * call_value = 
    (XmScrollBarCallbackStruct *) call_data;

  reason = call_value -> reason;
  value = call_value -> value;
  pixel = call_value -> pixel;

  switch(reason)
   {
      case XmCR_VALUE_CHANGED:
         printf("Reason = XmCR_VALUE_CHANGED,	Value = %d\n", value);
      break;

      case XmCR_INCREMENT:
         printf("Reason = XmCR_INCREMENT,	Value = %d\n", value);
      break;

      case XmCR_DECREMENT:
         printf("Reason = XmCR_DECREMENT,	Value = %d\n", value);
      break;

      case XmCR_PAGE_INCREMENT:
         printf("Reason = XmCR_PAGE_INCREMENT,	Value = %d\n", value);
      break;

      case XmCR_PAGE_DECREMENT:
         printf("Reason = XmCR_PAGE_DECREMENT,	Value = %d\n", value);
      break;

      case XmCR_TO_TOP:
         printf("Reason = XmCR_TO_TOP,	Value = %d,	Pixel = %d\n",
                 value, pixel);
      break;

      case XmCR_TO_BOTTOM:
         printf("Reason = XmCR_TO_BOTTOM,	Value = %d,	Pixel = %d\n",
                 value, pixel);
      break;

      case XmCR_DRAG:
            printf("Reason = XmCR_DRAG,	Value = %d\n", value);
      break;

      case XmCR_HELP:
         printf("Reason = XmCR_HELP,	Value = %d\n", value);
      break;

      default:
         printf("Hit the default, incorrect reason sent!!\n");
      break;

   }
}












