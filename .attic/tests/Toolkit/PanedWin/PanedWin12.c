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
static char rcsid[] = "$XConsortium: PanedWin12.c /main/5 1995/08/29 15:22:48 drk $"
#endif
#endif


#include <testlib.h>
/* the following is an experiment which should work on all platforms*/
#if defined(__STDC__)
#include <stdlib.h>
#endif
#include <Xm/SashP.h>

#define RANGE		100
#define	BUTTONS		10
#define B_X		50
#define B_Y		50

/*  Global Variables  */

static Cardinal NewInsertOrder ();

/* Test Paned Window creation with a new insertPosition function. */

void  main(argc, argv)
int     argc;
char  **argv;
{
    register int  	n, i;
    Arg			args[MAX_ARGS];
    char		name[10];
    Widget		PW, PB, PB1, PB2, PB3, PB4, PB5;
    int			b_value;
    short               get_val;

    srand((int) time(NULL));

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNgeometry, "150x150+0+0");  n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg(args[n], XmNinsertPosition, (XtOrderProc *) NewInsertOrder); n++;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);

    XtSetValues(PW, args, n); 	/* Ignored at creation? */
	
    for (i = 0; i < BUTTONS; i++) 
      {
	b_value = rand() % RANGE;
	sprintf(name, "PB%2d:%3d", i, b_value);
	    
	n = 0;
	XtSetArg(args[n], XmNuserData, b_value); n++;
	XtSetArg(args[n], XmNwidth, 150); n++;
	PB = XmCreatePushButton(PW, name, args, n);
	XtManageChild(PB);
      }
	
    XtManageChild(PW);
    CommonPause();
    XtDestroyWidget(PW);

    /* 
     * Add tests for 1.2 XmNpositionIndex functionality
     */

#ifndef MOTIF1_1

    n = 0;
    PW = XmCreatePanedWindow(Shell1, "PW", args, n);
    
    /*
     * Put some children in it
     */
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++;
    PB1 = XmCreatePushButton(PW, "PB1", args, n);
    XtManageChild(PB1);
    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++;
    PB2 = XmCreatePushButton(PW, "PB2", args, n);
    XtManageChild(PB2);
    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++;
    PB3 = XmCreatePushButton(PW, "PB3", args, n);
    XtManageChild(PB3);
    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++;
    PB4 = XmCreatePushButton(PW, "PB4", args, n);
    XtManageChild(PB4);
    XtManageChild(PW);
    CommonPause();

    /*
     * Change the last child first
     */
    
    n = 0;
    XtSetArg(args[n], XmNpositionIndex, 0); n++;
    XtSetValues(PB4, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNpositionIndex, &get_val); n++;
    XtGetValues(PB4, args, n);
    printf("Value of PB4 XmNpositionIndex = %d\n", get_val);
     
    CommonPause();

    
    /*
     * Mix up the middle ones
     */

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, 1); n++;
    XtSetValues(PB3, args, n);

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, &get_val); n++;
    XtGetValues(PB3, args, n);
    printf("Value of PB3 XmNpositionIndex = %d\n", get_val);
    CommonPause();

    /*
     * Add a new child to the end
     */
    
    n = 0;
    XtSetArg(args[n], XmNpositionIndex, XmLAST_POSITION); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    PB5 = XmCreatePushButton(PW, "PB5", args, n);
    XtManageChild(PB5);

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, &get_val); n++;
    XtGetValues(PB5, args, n);
    printf("Value of PB5 XmNpositionIndex = %d\n", get_val);

    CommonPause();

    /*
     *  Set an illegal value for XmNpositionIndex and see if it ralphs
     */
    
    n = 0;
    XtSetArg(args[n], XmNpositionIndex, 7); n++;
    XtSetValues(PB5, args, n);

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, &get_val); n++;
    XtGetValues(PB5, args, n);
    printf("Value of PB5 XmNpositionIndex = %d\n", get_val);

    CommonPause();

    /*
     * Set XmNpositionIndex = -2 and see what happens.
     */

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, -2); n++;
    XtSetValues(PB4, args, n);

    n = 0;
    XtSetArg(args[n], XmNpositionIndex, &get_val); n++;
    XtGetValues(PB4, args, n);
    printf("Value of PB4 XmNpositionIndex = %d\n", get_val);

    CommonPause();

    CommonPause();  /* Exit */

#endif

    XtAppMainLoop(app_context);
}

/***************************************************************************
 *
 * static Cardinal
 * NewInsertOrder (w)
 *
 *   This function searches through the composite widget's list of children
 *   to find the correct insertion position for the new child.  If the
 *   new child is an ordinary child (not a subclass of XmSashWidget)
 *   the position returned will cause it to be inserted sorted based on the
 *   value of XmNuserData within the other
 *   ordinary children but before any Sashs; if the new child is a 
 *   sash the position returned will cause it to be inserted at the
 *   end of the list.  This procedure does not examine the arglist.
 *
 ************************************************************************/


static Cardinal NewInsertOrder (w)
Widget w;         /* Composite widget being inserted into */
{
   CompositeWidget cw = (CompositeWidget) XtParent(w);
   Cardinal i=0;

   if (XmIsSash(w) || XmIsSeparatorGadget(w)) 
     {
       while ((i < cw->composite.num_children) &&
	      (! XmIsSash(cw->composite.children[i])) &&
	      (! XmIsSeparatorGadget(cw->composite.children[i])))
	 i++;
     }
   else
     {
       while ((i < cw->composite.num_children) &&
	      ((long) ((XmPrimitiveWidget) w)->primitive.user_data < 
	       (long) ((XmPrimitiveWidget)
		      (cw->composite.children[i]))->primitive.user_data) &&
	      (! XmIsSash(cw->composite.children[i])) &&
	      (! XmIsSeparatorGadget(cw->composite.children[i])))
	 i++;
     }

   return (i);
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





