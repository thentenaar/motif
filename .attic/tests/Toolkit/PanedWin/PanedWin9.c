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
static char rcsid[] = "$XConsortium: PanedWin9.c /main/4 1995/07/13 19:07:59 drk $"
#endif
#endif


#include <testlib.h>

/* Tests setting the various constraints in Paned Window. */
#define CASES		4
#define	BUTTONS		3
#define B_WIDTH		100
#define B_HEIGHT	100
#define W_MARGIN	25

/* Internal routines. */
static void DoTheButtons();
static void ReportSize();

void  main(argc, argv)
int     argc;
char  **argv;
{
    register int  	n, i;
    Arg			args[MAX_ARGS], ONargs[MAX_ARGS], OFFargs[MAX_ARGS];
    int			ONn, OFFn;
    Widget		PW, PBarray[BUTTONS];
    
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNgeometry, "150x150+0+0");  n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);
    
    /**	 These tests are for 1 each of 3 buttons in turn. 	**/

    /* Create the Paned Window and the buttons. */
    n = 0;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, B_WIDTH); n++;
    XtSetArg(args[n], XmNheight, 50); n++;
    for (i = 0; i < BUTTONS; i++) 
      {
	PBarray[i] = XmCreatePushButton(PW, "PB", args, n);
	XtManageChild(PBarray[i]);
      }
    
    XtManageChild(PW);
    
    CommonPause();

    /* Max set. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMaximum, 100); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMaximum, 1000); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);
    
    /* Min set. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMinimum, 100); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMinimum, 1); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);
    
    /* Min < 0. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMinimum, -100); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMinimum, 1); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);
    

    /* Max > min. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMaximum, 200); ONn++;
    XtSetArg(ONargs[ONn], XmNpaneMinimum, 100); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMaximum, 1000); OFFn++;
    XtSetArg(OFFargs[OFFn], XmNpaneMinimum, 1); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);
    

    /* Max < min. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMaximum, 100); ONn++;
    XtSetArg(ONargs[ONn], XmNpaneMinimum, 200); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMaximum, 1000); OFFn++;
    XtSetArg(OFFargs[OFFn], XmNpaneMinimum, 1); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);
    

    /* Max = min. */
    ONn = 0;
    XtSetArg(ONargs[ONn], XmNpaneMaximum, 100); ONn++;
    XtSetArg(ONargs[ONn], XmNpaneMinimum, 100); ONn++;

    OFFn = 0;
    XtSetArg(OFFargs[OFFn], XmNpaneMaximum, 1000); OFFn++;
    XtSetArg(OFFargs[OFFn], XmNpaneMinimum, 1); OFFn++;

    DoTheButtons(PBarray, ONargs, ONn, OFFargs, OFFn);

    /* Max = min for all buttons. */
    for (i = 0; i < BUTTONS; i++) 
      {
	XtSetValues(PBarray[i], ONargs, ONn);
      }
    
    CommonPause();
  
    XtAppMainLoop(app_context);
}


static void DoTheButtons(buttons, ONargs, ONn, OFFargs, OFFn)
     Widget 	buttons[];
     Arg	ONargs[];
     int	ONn;
     Arg	OFFargs[];
     int	OFFn;
{
  register int	i, j;
  
    /* Do all cases for 1 to BUTTONS. */
    for (i = 0; i < BUTTONS; i++) 
      {
	/* Set the values in the buttons. */
	for (j = 0; j < BUTTONS; j++) 
	  {
	    if (i == j) 
	      {
		XtSetValues(buttons[j], ONargs, ONn);
	      }
	    else 
	      {
		XtSetValues(buttons[j], OFFargs, OFFn);
	      }		
	  }

	CommonPause();
      }

  /* Reset the last button. */
  XtSetValues(buttons[BUTTONS - 1], OFFargs, OFFn);
  
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

