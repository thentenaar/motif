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
static char rcsid[] = "$XConsortium: PanedWin11.c /main/4 1995/07/13 19:06:04 drk $"
#endif
#endif


#include <testlib.h>

#define CASES		4
#define	BUTTONS		3
#define B_WIDTH		100
#define B_HEIGHT	100
#define B_X		50
#define B_Y		50

/* Tests the effect of varios ways of changing the state of the child:
   unmap, map, unmanage, manage, destroy.
*/

void  main(argc, argv)
int     argc;
char  **argv;
{
    Boolean       	trace = False;
    register int  	n, i, j;
    Arg			args[MAX_ARGS], ONargs[MAX_ARGS], OFFargs[MAX_ARGS];
    int			ONn, OFFn;
    Widget		PW, PBarray[BUTTONS];

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNgeometry, "150x150+0+0"); n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);
    
    /**	 These tests are for 1 each of 3 buttons in turn. 	**/

    /* Create the Paned Window and the buttons. */
    n = 0;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);

    n = 0;
    for (i = 0; i < BUTTONS; i++) 
      {
	PBarray[i] = XmCreatePushButton(PW, "PB", args, n);
	XtManageChild(PBarray[i]);
      }
    
    XtManageChild(PW);
    
    CommonPause();

    /* Unmap and remap each in turn. */
    for (i = 0; i < BUTTONS; i++)
      {
	n = 0;
	XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
	XtSetValues(PBarray[i], args, n);
	
	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNmappedWhenManaged, True); n++;
	XtSetValues(PBarray[i], args, n);

      }
    
    /* Manage and unmanage each in turn. */
    for (i = 0; i < BUTTONS; i++)
      {
	XtUnmanageChild(PBarray[i]);
	
	CommonPause();

	XtManageChild(PBarray[i]);
	
      }
    
    CommonPause();
    
    /* Destroy each of the three buttons in turn, recreating 
       window each time. */
    for (i = 0; i < BUTTONS; i++) 
      {
	n = 0;
	PW = XmCreatePanedWindow(Shell1, "PW", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, B_WIDTH); n++;
	XtSetArg(args[n], XmNheight, 50); n++;
	for (j = 0; j < BUTTONS; j++) 
	  {
	    PBarray[j] = XmCreatePushButton(PW, "PB", args, n);
	    XtManageChild(PBarray[j]);
	  }
    
	XtManageChild(PW);
    
	CommonPause();
	XtDestroyWidget(PBarray[i]);
	
	CommonPause();
      }
     CommonPause();
      
    XtAppMainLoop(app_context);
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

