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
static char rcsid[] = "$XConsortium: Navigation3.c /main/8 1995/07/13 19:43:03 drk $"
#endif
#endif

	/* test for PIR 1281 */

#include <X11/Xlib.h>

#include <testlib.h>


/*************Some Globals***********************/

Widget bb;
Widget Frame1, Frame2, Frame3, Frame4, Frame5, Frame6, Frame7;
Widget Draw1, Draw2; 
Widget rc1, rc2, rc3, rc4, rc5, rc6, rc7;
Widget btn1, btn2, btn3;
Widget toggle1;
Widget misc1, misc2;
Widget toggle21, toggle22;
Widget btn31, btn32, btn33, btn34, btn35, btn36;
Widget toggle31, toggle32, toggle33;
Widget btn41, btn42;
Widget toggle41, toggle42;

/********************* Some supporting routines ******************/


void toggleGCB();

static XtCallbackRec valuechangedCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };
static XtCallbackRec armCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };
static XtCallbackRec disarmCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };


void toggleGCB(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
    printf("ToggleG Callback Structure:\n    Reason:   ");
    switch( ((XmToggleButtonCallbackStruct*)call_data)->reason) {
        case XmCR_VALUE_CHANGED: printf("XmCR_VALUE_CHANGED\n"); break;
        case XmCR_ARM:          printf("XmCR_ARM\n"); break;
        case XmCR_DISARM:       printf("XmCR_DISARM\n"); break;
        default:                printf("Oops - unknown callback reason!\n");
                                break;
    }
}

AddAllTabGroups()
{
   Arg args[2];
   int trav_order = 1;
   char buf[8];

   /* Add in correct order */
   XmAddTabGroup(btn41);
   XmAddTabGroup(misc1);
   XmAddTabGroup(btn33);
   XmAddTabGroup(btn1);
   XmAddTabGroup(btn2);

   /* make the tab group widgets a different color */
   /* label them with their traversal order */

   XtSetArg (args[0], XmNbackground, CommonGetColor("red"));

#ifndef MOTIF1_1
   /* this is a BUG FIX in 1.2 which does result in a behavior
      change. The previously non-traversable widgets can now
      be traversed by this automatically-created tab group, and
      it so happens this tab group is handled FIRST. */
   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (btn31, args, 2);
#endif

   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (btn41, args, 2);
   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (misc1, args, 2);
   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (btn33, args, 2);
   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (btn1, args, 2);
   sprintf (buf, "No. %d", trav_order++);
   XtSetArg (args[1], XmNlabelString,
	     XmStringCreateSimple (buf));
   XtSetValues (btn2, args, 2);

   
}


/*******************************************************/


void main (argc, argv)
unsigned int argc;
char **argv;

{
    Arg args[15];
    int n;
    Widget child[20];
    XmString tcs;


    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
 
    bb = XmCreateBulletinBoard(Shell1, "bb", (ArgList) args, 0);
    XtManageChild(bb);

/* We want to test navigation across different widgets on the Bulletin
	Board as well as Navigation within the Frames within the BB, thus
	we are creating a set of three frames within the Bulleting Board,
	and they will each have a set of buttons within them.  */


	 n = 0;
	 XtSetArg(args[n], XmNx, 10); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 Frame1 = XmCreateFrame (bb, "Frame1", args, n);
	 XtManageChild (Frame1);

/* Let's make this Frame contain a RowColumn with three buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    rc1 = XmCreateRowColumn(Frame1, "rc1", (ArgList) args, n);
    XtManageChild(rc1);

/* Create the children of the row column manager. The children are
	buttons. */

	 n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) NULL); n++;
   btn1 = XmCreatePushButton(rc1, "btn1",(ArgList)args, n);
   XtManageChild(btn1);
   n--;
   XtSetArg(args[n],XmNactivateCallback,(XtArgVal) NULL); n++;
   btn2 = XmCreatePushButton(rc1, "btn2", (ArgList) args, n);
   XtManageChild(btn2);
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) NULL); n++;
   btn3 = XmCreatePushButton(rc1, "btn3", (ArgList) args, n);
   XtManageChild(btn3);
/* Lets do a toggle now... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle1", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle1 = XmCreateToggleButtonGadget(rc1,"toggle1",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle1);







/* Create the other Frame/Children combos. */
/* First the Frame */
	 n = 0;
	 XtSetArg(args[n], XmNx, 100); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 Frame2 = XmCreateFrame (bb, "Frame2", args, n);
	 XtManageChild (Frame2);
/* Let's make this Frame contain a RowColumn with two buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    rc2 = XmCreateRowColumn(Frame2, "rc2", (ArgList) args, n);
    XtManageChild(rc2);


/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   misc1 = XmCreatePushButton(rc2, "misc1", (ArgList) args, n);
   XtManageChild(misc1);
   misc2 = XmCreatePushButton(rc2, "misc2", (ArgList) args, n);
   XtManageChild(misc2);
/* Lets do a couple of  toggles now... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle21", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle21 = XmCreateToggleButtonGadget(rc2,"toggle21",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle21);
/* and another... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle22", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle22 = XmCreateToggleButtonGadget(rc2,"toggle22",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle22);


/*Two down, many to go... */

	 n = 0;
	 XtSetArg(args[n], XmNx, 200); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 Frame3 = XmCreateFrame (bb, "Frame3", args, n);
	 XtManageChild (Frame3);
/* Let's make this Frame contain a RowColumn with six buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    rc3 = XmCreateRowColumn(Frame3, "rc3", (ArgList) args, n);
    XtManageChild(rc3);


/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   btn31 = XmCreatePushButton(rc3, "btn31", (ArgList) args, n);
   XtManageChild(btn31);
   btn32 = XmCreatePushButton(rc3, "btn32", (ArgList) args, n);
   XtManageChild(btn32);
   btn33 = XmCreatePushButton(rc3, "btn33", (ArgList) args, n);
   XtManageChild(btn33);
   btn34 = XmCreatePushButton(rc3, "btn34", (ArgList) args, n);
   XtManageChild(btn34);
   btn35 = XmCreatePushButton(rc3, "btn35", (ArgList) args, n);
   XtManageChild(btn35);
   btn36 = XmCreatePushButton(rc3, "btn36", (ArgList) args, n);
   XtManageChild(btn36);
/* Toggle another... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle31", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle31 = XmCreateToggleButtonGadget(rc3,"toggle31",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle31);
    n = 0;
    tcs = XmStringLtoRCreate("Toggle32", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle32 = XmCreateToggleButtonGadget(rc3,"toggle32",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle32);
    n = 0;
    tcs = XmStringLtoRCreate("Toggle33", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
    XtSetArg(args[n], XmNarmCallback, armCR);  n++;
    XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
    toggle33 = XmCreateToggleButtonGadget(rc3,"toggle33",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle33);


	 n = 0;
	 XtSetArg(args[n], XmNx, 375); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 Frame4 = XmCreateFrame (bb, "Frame4", args, n);
	 XtManageChild (Frame4);
/* Let's make this Frame contain a RowColumn with two buttons and two toggles.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    rc4 = XmCreateRowColumn(Frame4, "rc4", (ArgList) args, n);
    XtManageChild(rc4);

/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   btn41 = XmCreatePushButton(rc4, "btn41", (ArgList) args, n);
   XtManageChild(btn41);
   btn42 = XmCreatePushButton(rc4, "btn42", (ArgList) args, n);
   XtManageChild(btn42);
   n = 0;
   tcs = XmStringLtoRCreate("Toggle41", XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   XtSetArg(args[n], XmNindicatorOn, True);  n++;
   XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
   XtSetArg(args[n], XmNarmCallback, armCR);  n++;
   XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
   toggle41 = XmCreateToggleButtonGadget(rc4,"toggle41",(ArgList) args, n);
   if(tcs) XmStringFree(tcs);
   XtManageChild(toggle41);


	n = 0;
	XtSetArg (args[n], XmNx, 485); n++;
	XtSetArg (args[n], XmNy, 10); n++;
	XtSetArg(args[n], XmNborderWidth, 5); n++;
	Frame5 = XmCreateFrame (bb, "Frame5", args, n);
	XtManageChild (Frame5);
	n = 0;
   XtSetArg(args[n], XmNwidth, 135);  n++;
   XtSetArg(args[n], XmNheight,135);  n++;
	XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
	Draw1 = XmCreateArrowButton (Frame5,"Draw1", args, n);
	XtManageChild (Draw1);
	

/* Now lets make two more RCs with nothing on them! */


    XtRealizeWidget (Shell1);
    AddAllTabGroups ();

    CommonPause ();
    CommonPause (); /* end */
    XtAppMainLoop(app_context);
}












