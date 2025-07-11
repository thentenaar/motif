/* $TOG: RIPanedWin3.c /main/5 1999/01/14 18:38:27 jff $ */
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
/*
 *  Create a vpaned in unit type of font_units, and get the sizes to ensure
 *  they correspond with the initial values.
 *
 *  Create a set of push buttons within the vpane using inches as the
 *  unit type and setting various resolution independent data.  Print
 *  out the data from a get values on each button.
 */

#include <signal.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Forward declarations  */

static void flushEvents();

DumpButtonData (w)
Widget w;
{
   Arg args[10];
   int n;

   Dimension width, height, shadowThickness, highlightThickness;
   short marginWidth, marginHeight;

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);		n++;
   XtSetArg (args[n], XmNheight, &height);		n++;
   XtSetArg (args[n], XmNshadowThickness, &shadowThickness);		n++;
   XtSetArg (args[n], XmNhighlightThickness, &highlightThickness);	n++;
   XtSetArg (args[n], XmNmarginWidth, &marginWidth);	n++;
   XtSetArg (args[n], XmNmarginHeight, &marginHeight);		n++;
   XtGetValues (w, args, n);

   printf ("Data for Button\n");
   printf ("width = %d,   height = %d\n", width, height);
   printf ("shadow = %d,   highlight = %d\n", shadowThickness, highlightThickness);
   printf ("margin width = %d,   margin height = %d\n\n",
            marginWidth, marginHeight);
   
}

/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;
{
   Widget vpane;
   Widget pbuttons[10];
   Arg args[10];
   int n, i;
   Dimension width, height;

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   /*  The vpaned creation.  */

   n = 0;
   XtSetArg (args[n], XmNunitType, Xm100TH_FONT_UNITS);	n++;
   XtSetArg (args[n], XmNwidth, 3000);	n++;
   XtSetArg (args[n], XmNheight, 4000);	n++;
   vpane = XmCreatePanedWindow (Shell1, "vpane", args, n);
   XtManageChild (vpane);

   flushEvents (XtDisplay(Shell1), 0);

   XtRealizeWidget (Shell1);

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);	n++;
   XtSetArg (args[n], XmNheight, &height);	n++;
   XtGetValues (vpane, args, n);
   printf ("width = %d,  height = %d\n\n", width, height);

   CommonPause();

   /*  Create a set of buttons as children of the vpane  */

   n = 0;
   XtSetArg (args[n], XmNunitType, Xm100TH_FONT_UNITS);	n++;
   XtSetArg (args[n], XmNwidth, 3000);       	n++;
   XtSetArg (args[n], XmNheight, 1000);	        n++;
   XtSetArg (args[n], XmNshadowThickness, 50);	n++;
   XtSetArg (args[n], XmNhighlightThickness, 100);	n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   pbuttons[0] = 
      XmCreatePushButtonGadget(vpane, "pushbuttongadget1", args, n);
   XtManageChild(pbuttons[0]);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 3000);       	n++;
   XtSetArg (args[n], XmNmarginHeight, 100);	        n++;
   XtSetValues (pbuttons[0], args, n);

   DumpButtonData(pbuttons[0]);

   CommonPause();

   /*  Create a set of buttons as children of the vpane  */

   n = 0;
   XtSetArg (args[n], XmNunitType, Xm100TH_FONT_UNITS);	n++;  
   XtSetArg (args[n], XmNwidth, 3000);       	n++;
   XtSetArg (args[n], XmNheight, 750);		n++;
   XtSetArg (args[n], XmNshadowThickness, 50);	n++;
   XtSetArg (args[n], XmNhighlightThickness, 100);	n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   XtSetArg (args[n], XmNmarginWidth, 3000);       	n++;
   XtSetArg (args[n], XmNmarginHeight, 100);	        n++;
   pbuttons[1] = 
      XmCreatePushButtonGadget(vpane, "pushbuttongadget2", args, n);
   XtManageChild(pbuttons[1]);

   DumpButtonData(pbuttons[1]);

   CommonPause();

   /*  Create a set of buttons as children of the vpane  */

   n = 0;
   XtSetArg (args[n], XmNunitType, Xm100TH_FONT_UNITS);	n++;
   XtSetArg (args[n], XmNwidth, 3000);       	n++;
   XtSetArg (args[n], XmNheight, 500);	        n++;
   XtSetArg (args[n], XmNshadowThickness, 50);	n++;
   XtSetArg (args[n], XmNhighlightThickness, 100);	n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   pbuttons[2] = 
      XmCreatePushButtonGadget(vpane, "pushbuttongadget3", args, n);
   XtManageChild (pbuttons[2]);

   DumpButtonData(pbuttons[2]);

   CommonPause();

   /*  Create a set of buttons as children of the vpane  */

   n = 0;
   XtSetArg (args[n], XmNunitType, Xm100TH_FONT_UNITS);	n++;  
   XtSetArg (args[n], XmNwidth, 3000);       	n++;
   XtSetArg (args[n], XmNheight, 1000);	        n++;
   XtSetArg (args[n], XmNshadowThickness, 50);	n++;
   XtSetArg (args[n], XmNhighlightThickness, 100);	n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);	n++;
   pbuttons[3] = 
      XmCreatePushButtonGadget(vpane, "pushbuttongadget4", args, n);
   XtManageChild (pbuttons[3]);
   DumpButtonData(pbuttons[3]);
   CommonPause();
   XtAppMainLoop(app_context);
}

static void flushEvents(display, delay)
Display * display;
int delay;
{
   XEvent event;
   while (XPending(display)) {
      XNextEvent(display, &event);
      XtDispatchEvent(&event);
   }
   sleep(delay);
}

