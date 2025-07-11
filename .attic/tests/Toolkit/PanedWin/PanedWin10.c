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
static char rcsid[] = "$XConsortium: PanedWin10.c /main/4 1995/07/13 19:05:52 drk $"
#endif
#endif


static void DoXY ();

#include <testlib.h>

#define	BUTTONS		3
#define B_WIDTH		100
#define B_HEIGHT	100
#define B_X		50
#define B_Y		50

/*  Global Variables  */

void  main(argc, argv)
int     argc;
char  **argv;
{
    register int  	n, i, j;
    Arg			args[MAX_ARGS];
    Widget		PW, PBarray[BUTTONS];
    Boolean	        resizable;
    

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNgeometry, "150x150+0+0");  n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);

    /* Create the Paned Window and the buttons. */
    n = 0;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, B_WIDTH / 2 ); n++; 
    XtSetArg(args[n], XmNheight, B_HEIGHT / 2 ); n++;  
    XtSetArg(args[n], XmNallowResize, False); n++; 

    for (i = 0; i < BUTTONS; i++) 
      {
	PBarray[i] = XmCreatePushButton(PW, "PB", args, n);
	XtManageChild(PBarray[i]);
      }
    
    XtManageChild(PW);  
    
    CommonPause();

    /* Try to resize each in turn with and without allowResize set. */

    /* With allowResize false. */
    for (i = 0; i < BUTTONS; i++)
      {
	n = 0;
	XtSetArg(args[n], XmNwidth, B_WIDTH); n++;
	XtSetArg(args[n], XmNheight, B_HEIGHT); n++;
	XtSetValues(PBarray[i], args, n);
	
      }

    CommonPause();

    /* Setting allowResize True for each button in turn. */

    for (j = 1; j <= BUTTONS; j++)
      {
	n = 0;
	XtSetArg(args[n], XmNallowResize, True); n++;
	XtSetValues(PBarray[j-1], args, n);
	    
	/* Increasing the size of each button. */
	for (i = 0; i < BUTTONS; i++)
	  {
	    n = 0;
	    XtSetArg(args[n], XmNwidth, 2 * B_WIDTH); n++;
	    XtSetArg(args[n], XmNheight, 2 * B_HEIGHT); n++;
	    XtSetValues(PBarray[i], args, n);
	
	  }

	CommonPause();
      }
    
    /* Setting allowResize False for each button in turn. */

    for (j = 1; j <= BUTTONS; j++)
      {
	n = 0;
	XtSetArg(args[n], XmNallowResize, False); n++;
	XtSetValues(PBarray[j-1], args, n);
	    
	/* Decreasing the size of each button. */
	for (i = 0; i < BUTTONS; i++)
	  {
	    n = 0;
	    XtSetArg(args[n], XmNwidth, B_WIDTH / 2 ); n++;
	    XtSetArg(args[n], XmNheight, B_HEIGHT / 2 ); n++;
	    XtSetValues(PBarray[i], args, n);
	
	  }

	CommonPause();
      }
    
    /* "Move" each in turn. */
    for (i = 0; i < BUTTONS; i++)
      {
	n = 0;
	XtSetArg(args[n], XmNx, B_X); n++;
	XtSetArg(args[n], XmNy, B_Y); n++;
	XtSetValues(PBarray[i], args, n);
	
	CommonPause();
      }

    XtDestroyWidget(PW);

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

