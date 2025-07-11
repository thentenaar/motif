/* $XConsortium: RIArrowBtn4.c /main/4 1995/07/15 21:08:26 drk $ */
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

/**************************************************************************
 *
 *  Create an arrow using inches and setting the width and height. 
 *  Reset the size, change the unit type, set the size again.
 *
 **************************************************************************/

#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>

/*  Forward declarations  */

static void flushEvents();

unsigned char UnitType[4] = { Xm1000TH_INCHES, 
			      Xm100TH_POINTS,
  			      Xm100TH_MILLIMETERS,
			      Xm100TH_FONT_UNITS };

void main (argc, argv)
int argc;
char **argv;
{
  Widget 	  arrow;
  Arg 		  args[20];
  int 		  n;
  Dimension	  width, height;
  unsigned char  unitType;
  int		  i;



   /*  initialize toolkit  */
  CommonTestInit(argc, argv);
    

  n = 0;
  XtSetArg(args[n], XmNwidth, 100);		n++;
  XtSetArg(args[n], XmNheight, 100);		n++;
  arrow = XmCreateArrowButton (Shell1, "arrow", args, n);
  XtManageChild (arrow);

  XtRealizeWidget (Shell1);
  flushEvents (XtDisplay(Shell1), 2);

  /*  Get the size of the widget in unit values  */
  n = 0;
  XtSetArg (args[n], XmNunitType, &unitType);	n++;
  XtSetArg (args[n], XmNwidth, &width);		n++;
  XtSetArg (args[n], XmNheight, &height);		n++;
  XtGetValues (arrow, args, n);
  PrintUnitType (unitType);
  printf("Width = %d , Height = %d\n\n", width, height);

  CommonPause();

  for (i = 0 ; i < 4; i++)
    {
      n = 0;
      XtSetArg (args[n], XmNunitType, UnitType[i]); n++;
      XtSetValues (arrow, args, n);

      /*  Get the size of the widget in unit values  */
      n = 0;
      XtSetArg (args[n], XmNunitType, &unitType);	n++;
      XtSetArg (args[n], XmNwidth, &width);		n++;
      XtSetArg (args[n], XmNheight, &height);		n++;
      XtGetValues (arrow, args, n);
      PrintUnitType (unitType);
      printf("Width = %d , Height = %d\n\n", width, height);

      CommonPause();
    }

  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(arrow);
}

PrintUnitType(UnitType)
int UnitType;
{

  switch (UnitType) {
  case 0:	
    printf ("unitType = %s\n", "XmPIXELS");
    break;
  case 1:
    printf ("unitType = %s\n", "Xm100TH_MILLIMETERS");
    break;
  case 2:
    printf ("unitType = %s\n", "Xm1000TH_INCHES");
    break;
  case 3:
    printf ("unitType = %s\n", "Xm100TH_POINTS");
    break;
  case 4: 
    printf ("unitType = %s\n", "Xm100TH_FONT_UNITS");
    break;
  default:
    printf ("Not a valid Unit Type\n");
  };
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
