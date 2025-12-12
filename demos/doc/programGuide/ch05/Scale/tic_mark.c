/* $XConsortium: tic_mark.c /main/5 1995/07/15 20:39:30 drk $ */
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
 *
 */
/*
 * HISTORY
 */
#include <stdio.h>
#include <Xm/XmAll.h>

void        CreateScale(Widget);
extern void CreateMenus(Widget);
extern void HelpCB(Widget, XtPointer, XtPointer);
extern void QuitCB(Widget, XtPointer, XtPointer);

Widget top_level;
 XtAppContext app_context;

#define APP_CLASS "XmdScale"

static String fallbacks[] = {
"scale1.allowShellResize: True",
"scale1.value: 20",
NULL
};

int
main(int    argc,
     char **argv)
{
   Widget main_window;

   XtSetLanguageProc(NULL, NULL, NULL);
   top_level = XtVaOpenApplication(&app_context, APP_CLASS,
                                NULL, 0, &argc, argv,
                                fallbacks,sessionShellWidgetClass,
                                NULL);
   main_window = XtVaCreateManagedWidget("main_window",
                                xmMainWindowWidgetClass, top_level,
                                NULL);

   CreateMenus(main_window);
   CreateScale(main_window);

   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);

   return 0;    /* make compiler happy */
}


void
CreateScale(Widget parent_of_scale)
{
#define BOTTOM_OF_THERMOMETER -20
#define TOP_OF_THERMOMETER 40
#define NUM_TICS (((TOP_OF_THERMOMETER - BOTTOM_OF_THERMOMETER) / 10) + 1)
 Widget          scale1, tics[NUM_TICS];
 Arg             args[10];
 Cardinal        i, n;
 unsigned char   scale_orientation, tic_orientation;
 Dimension       tic_long_dim = 10, tic_short_dim = 5;
 Dimension       tic_width, tic_height;
 char            tic_name[10];

 /* Build the Scale. */
   scale1 = XtVaCreateManagedWidget("scale1",
                 xmScaleWidgetClass, parent_of_scale,
                 XmNslidingMode, XmTHERMOMETER,
                 XmNeditable, True,
                 XmNshowValue, True,
                 XmNminimum, BOTTOM_OF_THERMOMETER,
                 XmNmaximum, TOP_OF_THERMOMETER,
                 NULL);

 /* Now build the tic marks. */
   n = 0;
   XtSetArg(args[n], XmNorientation, &scale_orientation);   n++;
   XtGetValues(scale1, args, n);
   if (scale_orientation == XmHORIZONTAL) {
       tic_orientation = XmVERTICAL;
       tic_width = tic_short_dim;
       tic_height = tic_long_dim;
   } else {
       tic_orientation = XmHORIZONTAL;
       tic_width = tic_long_dim;
       tic_height = tic_short_dim;
   }

   for (i = 0; i < NUM_TICS; i++) {
       sprintf(tic_name, "tic_%d", i);
       n = 0;
       XtSetArg(args[n], XmNseparatorType, XmSINGLE_LINE);  n++;
       XtSetArg(args[n], XmNorientation, tic_orientation);  n++;
       XtSetArg(args[n], XmNwidth, tic_width);              n++;
       XtSetArg(args[n], XmNheight, tic_height);            n++;
       tics[i] = XmCreateSeparatorGadget(scale1, tic_name,
                                           args, n);
   }

   XtManageChildren(tics, NUM_TICS);
}
