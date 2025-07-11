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
static char rcsid[] = "$XConsortium: Scrollbar11.c /main/7 1995/07/13 19:13:56 drk $"
#endif
#endif

/************************************************************************
 *
 * Create 2 scroll bars. One with MAX_ON_LEFT, the other with MAX_ON_RIGHT
 * Check the keybindings work
 *
 * Repeat for Vertical scrollbar with MAX_ON_TOP, MAX_ON_BOTTOM
 *
 ************************************************************************/

#include <X11/StringDefs.h>
#include <testlib.h>

static void ScrollCallback();

static void ScrollCallback(sbw, closure, call_data)
Widget sbw;
XtPointer closure;
XtPointer call_data;

{
   int value;
   int reason;
   XmScrollBarCallbackStruct 
          *call_value=(XmScrollBarCallbackStruct *) call_data;

   reason = call_value -> reason;
   value = call_value -> value;

   switch(reason)
   {
      case XmCR_VALUE_CHANGED:
         printf("XmScrollbar(XmCR_VALUE_CHANGED,%d)\n", value);
         break;
      case XmCR_INCREMENT:
         printf("XmScrollbar(XmCR_INCREMENT,%d)\n", value);
         break;
      case XmCR_DECREMENT:
         printf("XmScrollbar(XmCR_DECREMENT,%d)\n", value);
         break;
      case XmCR_PAGE_INCREMENT:
         printf("XmScrollbar(XmCR_PAGE_INCREMENT,%d)\n", value);
         break;
      case XmCR_PAGE_DECREMENT:
         printf("XmScrollbar(XmCR_PAGE_DECREMENT,%d)\n", value);
         break;
      case XmCR_TO_TOP:
         printf("XmScrollbar(XmCR_TO_TOP,%d)\n", value);
         break;
      case XmCR_TO_BOTTOM:
         printf("XmScrollbar(XmCR_TO_BOTTOM,%d)\n", value);
         break;
      case XmCR_HELP:
         printf("XmScrollbar(XmCR_HELP,%d)\n", value);
         break;
      default:
         printf("Unexpected Callback reason!\n");
         break;
   }
}

void main(argc, argv)
unsigned int argc;
char **argv;
{
  Widget bb, bb2;
  Widget scrollbar1;
  Widget scrollbar2;
  Arg args[10];
  int n, i;

  CommonTestInit(argc, argv);

  bb = XmCreateBulletinBoard(Shell1, "bb", NULL, 0);
  XtManageChild(bb);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg(args[n], XmNprocessingDirection,XmMAX_ON_TOP); n++;
  XtSetArg(args[n], XmNx, 20); n++;
  XtSetArg(args[n], XmNy, 20); n++;
  XtSetArg(args[n], XmNwidth, 30); n++;
  XtSetArg(args[n], XmNheight, 400);		n++;
  XtSetArg(args[n], XmNtraversalOn, True); 	n++;
  XtSetArg(args[n], XmNhighlightThickness, 2);		n++;
  XtSetArg(args[n], XmNhighlightOnEnter, True);		n++;
  scrollbar1 = XmCreateScrollBar(bb, "scrollbar1", args, n);
  XtManageChild(scrollbar1);
  XtAddCallback(scrollbar1, XmNvalueChangedCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNincrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNdecrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNpageIncrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNpageDecrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNtoTopCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar1, XmNtoBottomCallback,  ScrollCallback, NULL);
  n = 0;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM); n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);		n++;
  XtSetArg(args[n], XmNx, 70); n++;
  XtSetArg(args[n], XmNy, 20); n++;
  XtSetArg(args[n], XmNwidth, 30); n++;
  XtSetArg(args[n], XmNheight, 400);		n++;
  XtSetArg(args[n], XmNtraversalOn, True); 	n++;
  XtSetArg(args[n], XmNhighlightThickness, 2);		n++;
  XtSetArg(args[n], XmNhighlightOnEnter, True);		n++;
  scrollbar2 = XmCreateScrollBar(bb, "scrollbar2", args, n);
  XtManageChild(scrollbar2);

  XtAddCallback(scrollbar2, XmNvalueChangedCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNincrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNdecrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNpageIncrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNpageDecrementCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNtoTopCallback,  ScrollCallback, NULL);
  XtAddCallback(scrollbar2, XmNtoBottomCallback,  ScrollCallback, NULL);

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  printf("changing to orientation = HORIZONTAL\n");
  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);	n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
  XtSetArg(args[n], XmNx, 20); n++;
  XtSetArg(args[n], XmNy, 20); n++;
  XtSetArg(args[n], XmNwidth, 400); n++;
  XtSetArg(args[n], XmNheight, 30);		n++;
  XtSetValues(scrollbar1, args, n);
  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);	n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
  XtSetArg(args[n], XmNx, 20); n++;
  XtSetArg(args[n], XmNy, 70); n++;
  XtSetArg(args[n], XmNwidth, 400); n++;
  XtSetArg(args[n], XmNheight, 30);		n++;
  XtSetValues(scrollbar2, args, n);

  CommonPause();

  XtDestroyWidget(scrollbar2);

  /*** Test case for CR 6607 ***/

  n = 0;
  XtSetArg(args[n], XmNx, 0); n++;
  XtSetArg(args[n], XmNy, 70); n++;
  bb2 = XmCreateBulletinBoard(bb, "bb2", args, n);
  XtManageChild(bb2);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL);	n++;
  XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_LEFT); n++;
  XtSetArg(args[n], XmNx, 15); n++;
  XtSetArg(args[n], XmNy, 0); n++;
  XtSetArg(args[n], XmNwidth, 400); n++;
  XtSetArg(args[n], XmNheight, 30);		n++;
  XtSetArg(args[n], XmNtraversalOn, True); 	n++;
  XtSetArg(args[n], XmNhighlightThickness, 2);		n++;
  XtSetArg(args[n], XmNhighlightOnEnter, True);		n++;
  scrollbar2 = XmCreateScrollBar(bb2, "scrollbar2", args, n);
  XtManageChild(scrollbar2);

  XtSetSensitive(scrollbar1, False);
  XtSetSensitive(bb2, False);

  CommonPause();
  CommonPause();

  XtDestroyWidget(scrollbar2);
  XtDestroyWidget(bb2);

  /*** End test case for CR 6607 ***/

  XtDestroyWidget(scrollbar1);
  XtDestroyWidget(bb);

  XtAppMainLoop(app_context);
}
