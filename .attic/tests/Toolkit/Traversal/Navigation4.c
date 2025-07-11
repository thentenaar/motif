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
static char rcsid[] = "$XConsortium: Navigation4.c /main/6 1995/07/13 19:43:22 drk $"
#endif
#endif

	/* test for PIR 1304 */

#include <X11/Xlib.h>

#include <testlib.h>

extern Widget InstructionBox;

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
Widget help_button;

/* important - this holds state of who has the focus currently. In 1.2, we
   can switch to using fn... */
Widget current_focus;

/* forward declarations */

void SetFocus ();
void MoveFocus ();
void MoveFocusOnParent ();

static XtCallbackRec SetFocusRec[] = { {SetFocus, (caddr_t) NULL },
				       {NULL, NULL} };

/*******************************************************/


void main (argc, argv)
unsigned int argc;
char **argv;

{
    Arg args[15];
    int n;
    Widget child[20];
    XmString tcs;
    Boolean flag;  

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
/*	 XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++; */
	 Frame1 = XmCreateFrame (bb, "Frame1", args, n);
	 XtManageChild (Frame1);

/* Let's make this Frame contain a RowColumn with three buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
/*	 XtSetArg (args[n], XmNnavigationType, XmNONE); n++; */
    rc1 = XmCreateRowColumn(Frame1, "rc1", (ArgList) args, n);
    XtManageChild(rc1);

/* Create the children of the row column manager. The children are
	buttons. */

	 n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, SetFocusRec); n++;
   btn1 = XmCreatePushButton(rc1, "btn1",(ArgList)args, n);
   XtManageChild(btn1);
   btn2 = XmCreatePushButton(rc1, "btn2", (ArgList) args, n);
   XtManageChild(btn2);
   btn3 = XmCreatePushButton(rc1, "btn3", (ArgList) args, n);
   XtManageChild(btn3);
/* Lets do a toggle now... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle1", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
    toggle1 = XmCreateToggleButtonGadget(rc1,"toggle1",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle1);

/* Create the other Frame/Children combos. */
/* First the Frame */
	 n = 0;
	 XtSetArg(args[n], XmNx, 100); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
/*	 XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++; */
	 Frame2 = XmCreateFrame (bb, "Frame2", args, n);
	 XtManageChild (Frame2);
/* Let's make this Frame contains a RowColumn with two buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
/*	 XtSetArg (args[n], XmNnavigationType, XmNONE); n++; */
    rc2 = XmCreateRowColumn(Frame2, "rc2", (ArgList) args, n);
    XtManageChild(rc2);


/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, SetFocusRec); n++;
   misc1 = XmCreatePushButton(rc2, "misc1", (ArgList) args, n);
   XtManageChild(misc1);
   misc2 = XmCreatePushButton(rc2, "misc2", (ArgList) args, n);
   XtManageChild(misc2);
/* Lets do a couple of  toggles now... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle21", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
    toggle21 = XmCreateToggleButtonGadget(rc2,"toggle21",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle21);
/* and another... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle22", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
    toggle22 = XmCreateToggleButtonGadget(rc2,"toggle22",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle22);


/*Two down, many to go... */

	 n = 0;
	 XtSetArg(args[n], XmNx, 200); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
/*	 XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++; */
	 Frame3 = XmCreateFrame (bb, "Frame3", args, n);
	 XtManageChild (Frame3);
/* Let's make this Frame contain a RowColumn with six buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
/*	 XtSetArg (args[n], XmNnavigationType, XmNONE); n++; */
    rc3 = XmCreateRowColumn(Frame3, "rc3", (ArgList) args, n);
    XtManageChild(rc3);


/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, SetFocusRec); n++;
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
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
    toggle31 = XmCreateToggleButtonGadget(rc3,"toggle31",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle31);
    n = 0;
    tcs = XmStringLtoRCreate("Toggle32", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
    toggle32 = XmCreateToggleButtonGadget(rc3,"toggle32",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle32);
    n = 0;
    tcs = XmStringLtoRCreate("Toggle33", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
      toggle33 = XmCreateToggleButtonGadget(rc3,"toggle33",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);
    XtManageChild(toggle33);


	 n = 0;
	 XtSetArg(args[n], XmNx, 375); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
/*	 XtSetArg (args[n], XmNnavigationType<, XmTAB_GROUP); n++; */
	 Frame4 = XmCreateFrame (bb, "Frame4", args, n);
	 XtManageChild (Frame4);
/* Let's make this Frame contain a RowColumn with two buttons and two toggles.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
/* 	 XtSetArg (args[n], XmNnavigationType, XmNONE); n++; */
    rc4 = XmCreateRowColumn(Frame4, "rc4", (ArgList) args, n);
    XtManageChild(rc4);

/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, SetFocusRec); n++;
   btn41 = XmCreatePushButton(rc4, "btn41", (ArgList) args, n);
   XtManageChild(btn41);
   btn42 = XmCreatePushButton(rc4, "btn42", (ArgList) args, n);
   XtManageChild(btn42);
   n = 0;
   tcs = XmStringLtoRCreate("Toggle41", XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   XtSetArg(args[n], XmNindicatorOn, True);  n++;
   XtSetArg(args[n], XmNvalueChangedCallback, SetFocusRec); n++;
   toggle41 = XmCreateToggleButtonGadget(rc4,"toggle41",(ArgList) args, n);
   if(tcs) XmStringFree(tcs);
   XtManageChild(toggle41);


	n = 0;
	XtSetArg (args[n], XmNx, 485); n++;
	XtSetArg (args[n], XmNy, 10); n++;
	XtSetArg(args[n], XmNborderWidth, 5); n++;
/* 	XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++; */
	Frame5 = XmCreateFrame (bb, "Frame5", args, n);
	XtManageChild (Frame5);
	n = 0;
        XtSetArg(args[n], XmNwidth, 135);  n++;
        XtSetArg(args[n], XmNheight,135);  n++;
	XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
   /*  XtSetArg (args[n], XmNnavigationType, XmNONE); n++; */
        XtSetArg(args[n], XmNactivateCallback, SetFocusRec); n++;

	Draw1 = XmCreateArrowButton (Frame5,"Draw1", args, n);
	XtManageChild (Draw1);

	

/* Now lets make two more RCs with nothing on them! */


	 n = 0;
	 XtSetArg (args[n], XmNx, 650); n++;
	 XtSetArg (args[n], XmNy, 10); n++;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNheight, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
	 Frame6 = XmCreateFrame (bb, "Frame6", args, n);
	 XtManageChild (Frame6);
	 n = 0;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNwidth, 20); n++;
    rc6 = XmCreateRowColumn(Frame6, "rc6", (ArgList) args, n);
    XtManageChild(rc6);

	 n = 0;
	 XtSetArg (args[n], XmNx, 700); n++;
	 XtSetArg (args[n], XmNy, 10); n++;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNheight, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
	 Frame7 = XmCreateFrame (bb, "Frame7", args, n);
	 XtManageChild (Frame7);
	 n = 0;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
    rc7 = XmCreateRowColumn(Frame7, "rc7", (ArgList) args, n);
    XtManageChild(rc7);


    XtRealizeWidget (Shell1);
    CommonPause ();   /* just tabbing around */
    
    /* fire up Help button on Pause dialog to help us test */
  
    help_button = XmMessageBoxGetChild (InstructionBox, XmDIALOG_HELP_BUTTON);
    XtSetSensitive (help_button, True);
    n = 0;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Traverse")); n++;
    XtSetValues (help_button, args, n);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_DOWN);

    CommonPause ();  /* TRAVERSE_DOWN anywhere*/

    XtSetSensitive (help_button, False);

    flag = XmProcessTraversal (rc2, XmTRAVERSE_DOWN);
    if (!flag) printf ("Traversal test failed!\n");
    CommonPause ();  /* programmatic DOWN on rc2 parent of misc2 */

    XtSetSensitive (help_button, True);
    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_DOWN);
    XtAddCallback (help_button, XmNactivateCallback,
		      MoveFocusOnParent, (XtPointer) XmTRAVERSE_DOWN);
    CommonPause ();  /* programmatic DOWN on parent anywhere */

    XtSetSensitive (help_button, False);
    flag = XmProcessTraversal (bb, XmTRAVERSE_DOWN);
    if (!flag) printf ("Traversal test failed!\n");
    CommonPause ();  /* programmatic DOWN on bb */

    flag = XmProcessTraversal (btn3, XmTRAVERSE_DOWN);
    if (!flag) printf ("Traversal test failed!\n");
    CommonPause ();  /* programmatic down on btn3 */

    XtSetSensitive (help_button, True);
    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocusOnParent, (XtPointer) XmTRAVERSE_DOWN);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_HOME);
    CommonPause ();  /* TRAVERSE_HOME */

    XmProcessTraversal (bb, XmTRAVERSE_HOME);
    CommonPause ();  /* programmatic DOWN on bb */  

    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_HOME);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_LEFT);
    CommonPause ();  /* TRAVERSE_LEFT */  

    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_LEFT);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_NEXT);
    CommonPause ();  /* TRAVERSE_NEXT */
  
    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_NEXT);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_NEXT_TAB_GROUP);
    CommonPause ();  /* TRAVERSE_NEXT_TAB_GROUP */

    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_NEXT_TAB_GROUP);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_PREV_TAB_GROUP);
    CommonPause ();  /* TRAVERSE_PREV_TAB_GROUP */

    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_PREV_TAB_GROUP);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_RIGHT);
    CommonPause ();  /* TRAVERSE_RIGHT */

    XtRemoveCallback (help_button, XmNactivateCallback, 
		      MoveFocus, (XtPointer) XmTRAVERSE_RIGHT);
    XtAddCallback (help_button, XmNactivateCallback, 
		   MoveFocus, (XtPointer) XmTRAVERSE_UP);
    CommonPause ();  /* TRAVERSE_UP */

    XtSetSensitive (help_button, False);
  
    CommonPause ();  /* exit */


    XtAppMainLoop(app_context);
}

void SetFocus (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
  printf ("Setting focus to %s\n", XtName (w));
  current_focus = w;
  fflush (stdout);
}

void MoveFocus (w, client_data, call_data)
Widget w;
int client_data;
XtPointer call_data;

{
  printf ("Moving focus from %s\n", XtName (current_focus));
  XmProcessTraversal (current_focus, client_data);
  fflush (stdout);
}

void MoveFocusOnParent (w, client_data, call_data)
Widget w;
int client_data;
XtPointer call_data;

{
  printf ("Moving focus from %s\n", XtName(XtParent(current_focus)));
  XmProcessTraversal (XtParent(current_focus), client_data);
  fflush (stdout);
}
