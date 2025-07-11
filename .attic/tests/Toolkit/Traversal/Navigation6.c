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
static char rcsid[] = "$XConsortium: Navigation6.c /main/6 1995/07/13 19:43:58 drk $"
#endif
#endif

	/* test for PIR 1314 */

#include <X11/Xlib.h>
#include <testlib.h>

/* forward declarations */
void CheckString();
void ForceFocusOnText1();
void focusoutCB();
static void focusout();
static void focusin();
static void SetTraversalOn();

/*************Some Globals***********************/

Widget bb;
Widget Frame1, Frame2, Frame3, Frame4, Frame5, Frame6, Form1;
Widget rc1, rc2, rc3, rc4, rc5, rc6;
Widget btn1, btn2, btn3;
Widget toggle1;
Widget misc1, misc2;
Widget toggle21, toggle22;
Widget btn31, btn32, btn33, btn34, btn35, btn36;
Widget toggle31, toggle32, toggle33;
Widget btn41, btn42;
Widget toggle41, toggle42;
Widget text1, text2, text3, text4;


/********************* Some supporting routines ******************/


void toggleGCB();

static XtCallbackRec valuechangedCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };
static XtCallbackRec armCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };
static XtCallbackRec disarmCR[] = {{ toggleGCB, (caddr_t)NULL},
                                           { NULL, NULL }, };

void toggleGCB(w, client_data, call_data)
Widget     w;
XtPointer  client_data;
XtPointer  call_data;
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
	and they will each have a set of buttons within them.  
     */

	 n = 0;
	 XtSetArg(args[n], XmNx, 10); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame1 = XmCreateFrame (bb, "Frame1", args, n);
	 XtManageChild (Frame1);

/* Let's make this Frame contain a RowColumn with three buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     XtSetArg(args[n], XmNnumColumns, 1); n++;
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
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
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame2 = XmCreateFrame (bb, "Frame2", args, n);
	 XtManageChild (Frame2);
/* Let's make this Frame contain a RowColumn with two buttons.
	All the buttons will be added to the Tab Group. */

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
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
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame3 = XmCreateFrame (bb, "Frame3", args, n);
	 XtManageChild (Frame3);
/* Let's make this Frame contain a RowColumn with six buttons.
	All the buttons will be added to the Tab Group. */

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
	XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
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
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame4 = XmCreateFrame (bb, "Frame4", args, n);
	 XtManageChild (Frame4);
/* Let's make this Frame contain a RowColumn with two buttons and two toggles.
	All the buttons will be added to the Tab Group. */

	 n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
	 XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
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

/* Now lets make two more RCs with nothing on them! */

	 n = 0;
	 XtSetArg (args[n], XmNx, 485); n++;
	 XtSetArg (args[n], XmNy, 10); n++;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNheight, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
         XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame5 = XmCreateFrame (bb, "Frame5", args, n);
	 XtManageChild (Frame5);
	 n = 0;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNwidth, 20); n++;
	 XtSetArg(args[n], XmNnavigationType, XmNONE); n++;
    rc5 = XmCreateRowColumn(Frame5, "rc5", (ArgList) args, n);
    XtManageChild(rc5);

	 n = 0;
	 XtSetArg (args[n], XmNx, 550); n++;
	 XtSetArg (args[n], XmNy, 10); n++;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNheight, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
         XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;
	 Frame6 = XmCreateFrame (bb, "Frame6", args, n);
	 XtManageChild (Frame6);
	 n = 0;
	 XtSetArg(args[n], XmNborderWidth, 5); n++;
	 XtSetArg(args[n], XmNwidth, 5); n++;
	 XtSetArg(args[n], XmNnavigationType, XmNONE); n++;
    rc6 = XmCreateRowColumn(Frame6, "rc6", (ArgList) args, n);
    XtManageChild(rc6);

    XtRealizeWidget (Shell1);

    CommonPause ();

    /* begin test for PIR 2188 */
    
    XmProcessTraversal (btn42, XmTRAVERSE_CURRENT);
    CommonPause();

    XmProcessTraversal (bb, XmTRAVERSE_CURRENT);
    CommonPause();

    n = 0;
    XtSetArg (args[n], XmNx, 600); n++;
    XtSetArg (args[n], XmNy, 10); n++;
    XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++;
    Form1 = XmCreateForm (bb, "Form1", args, n);

    n = 0;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
    /* navigationType is XmTAB_GROUP by default */
    text1 = XmCreateText (Form1, "text1", args, n);

    XtAddCallback (text1, XmNlosingFocusCallback, CheckString, NULL); 
    XtAddCallback (btn42, XmNactivateCallback, ForceFocusOnText1, text1);

    n = 0;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    /* navigationType is XmTAB_GROUP by default */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, text1); n++;
    text2 = XmCreateText (Form1, "text2", args, n);
    XtManageChild (text1);
    XtManageChild (text2);
    XtManageChild (Form1);

    XmProcessTraversal (Form1, XmTRAVERSE_CURRENT);
    SetTraversalOn(False);
    CommonPause();
    CommonPause();

    /* end test for PIR 2188 */
    /* begin test for PIR 3646 */

    n = 0;
    XtSetArg (args[n], XmNsensitive, True); n++;
    XtSetArg (args[n], XmNvalue, ""); n++;
    XtSetValues (text1, args, n);

    n = 0;
    XtSetArg (args[n], XmNvalue, ""); n++;
    XtSetValues (text2, args, n);

    XtRemoveCallback (text1, XmNlosingFocusCallback, CheckString, NULL);
    XtRemoveCallback (text2, XmNlosingFocusCallback, CheckString, NULL);

    XtAddCallback(text1, XmNlosingFocusCallback, focusout, (caddr_t)1);
    XtAddCallback(text2, XmNlosingFocusCallback, focusout, (caddr_t)0);
    XtAddCallback(text1, XmNfocusCallback, focusin, (caddr_t)1);
    XtAddCallback(text2, XmNfocusCallback, focusin, (caddr_t)0);

    CommonPause();
    /* end test for PIR 3646 */

    /* begin test for PIR 2446 */
    n = 0;
    XtSetArg (args[n], XmNvalue, "Text 1"); n++;
    XtSetValues (text1, args, n);

    n = 0;
    XtSetArg (args[n], XmNvalue, "Text 2"); n++;
    XtSetValues (text2, args, n);
  
    XtAddCallback (text1, XmNlosingFocusCallback, CheckString, NULL);
    XtAddCallback (text2, XmNlosingFocusCallback, CheckString, NULL);

    /* create a couple more to make it more obvious */
    n = 0;
    XtSetArg (args[n], XmNvalue, "Text 3"); n++;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    /* navigationType is XmTAB_GROUP by default */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, text2); n++;
    text3 = XmCreateText (Form1, "text3", args, n);

    n = 0;
    XtSetArg (args[n], XmNvalue, "Text 4"); n++;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    /* navigationType is XmTAB_GROUP by default */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, text3); n++;
    text4 = XmCreateText (Form1, "text4", args, n);

    XtManageChild (text3);
    XtManageChild (text4);

    XtSetSensitive (text1, True);

    if (!XtIsSensitive (text1))
      printf ("Hey! I wasn't set sensitive!\n");
    else 
      printf ("text1 is sensitive\n");

    XtAddCallback        (text1,XmNlosingFocusCallback ,focusoutCB,text1);
    XtAddCallback        (text2,XmNlosingFocusCallback ,focusoutCB,text1);
    XtAddCallback        (text3,XmNlosingFocusCallback ,focusoutCB,text1);
    XtAddCallback        (text4,XmNlosingFocusCallback ,focusoutCB,text1);

    CommonPause();
    
    /* end test for PIR 2446 */

    XtAppMainLoop(app_context);
}

void focusoutCB (w, client, event)
Widget w;
XtPointer client, event;
{
   if (!XmProcessTraversal((Widget)client,XmTRAVERSE_CURRENT))
      XtWarning("focusoutCB: XmProcessTraversal failed");
}

void CheckString (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
  /* hee hee. Check for secret password and if incorrect, force focus
     back to widget */

  char * input;
  static int counter = 0;

  input = XmTextGetString (w);

  if (strcmp (input, "insensitive") == 0)
    XtSetSensitive (w, False);
    
  if (strcmp (input, "meow") != 0) {
    XmProcessTraversal (w, XmTRAVERSE_CURRENT);
    printf ("%d: You didn't guess the password.\n", ++counter);
  }
  else
  {
    SetTraversalOn(True);
    XmProcessTraversal (w, XmTRAVERSE_NEXT);
  }


  XtFree (input);

}

void ForceFocusOnText1 (w, client_data, call_data)
Widget w, client_data;
XtPointer call_data;

{
  XmProcessTraversal (client_data, XmTRAVERSE_CURRENT);

}


/******* Callback section for PIR 3646 test *******/

/*** error msg routine ***/
static int lock_msg = 0; /* just one error at time */

static void delete_dialog(w, client, reason)
Widget w;
XtPointer client;
XtPointer reason;
{
	XtDestroyWidget(XtParent((Widget)client));
	lock_msg = 0;
}


static void create_error_msg(w, title, message)
Widget w;
String title;
String message;
{
	Arg			wargs[ 10 ];
	Cardinal	nwargs;
	Widget		w_box;
	XmString my_message, my_title;

	if(lock_msg == 1)
		return;
	else
		lock_msg = 1;

	my_title   = XmStringCreate( title,   XmSTRING_DEFAULT_CHARSET );
	my_message = XmStringCreate( message, XmSTRING_DEFAULT_CHARSET );

	nwargs = 0;
	XtSetArg( wargs[ nwargs ], XmNdialogTitle,   my_title ); nwargs++;
	XtSetArg( wargs[ nwargs ], XmNmessageString, my_message ); nwargs++;
	XtSetArg( wargs[ nwargs ], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL ); nwargs++;
	w_box = XmCreateErrorDialog( w, "w_box", wargs, nwargs );

	XtAddCallback( w_box, XmNokCallback, delete_dialog, w_box );

	XtUnmanageChild( XmMessageBoxGetChild( w_box, XmDIALOG_CANCEL_BUTTON ));
	XtUnmanageChild( XmMessageBoxGetChild(w_box, XmDIALOG_HELP_BUTTON));

	nwargs = 0;
	XtSetArg (wargs[nwargs], XmNx, 0); nwargs++;
	XtSetArg (wargs[nwargs], XmNy, 0); nwargs++;
	XtSetValues (XtParent(w_box), wargs, nwargs);

	XtManageChild( w_box );
	/* XtPopup(XtParent(w_box), XtGrabExclusive ); */
}

static int last_visited;

static void focusin(w, client, call)
Widget w;
int client;
XmAnyCallbackStruct* call;
{
	String s;
	int len;
	if (client == 1 && last_visited == 0)  {
            /* Going into field 1 fm field 0 (tab or mouse) */
			s = XmTextGetString(text1);
			if ( (s == (String) NULL) || (strlen(s) == 0) ) {
				XmProcessTraversal(text1, XmTRAVERSE_CURRENT);
				create_error_msg(w, "Error!", "This field is mandatory!");
			}
	}
	
}

static void focusout(w, client, call)
Widget w;
int client;
XmAnyCallbackStruct* call;
{

	last_visited = client;
}

static void STO(widget, status)
  Widget widget;
  Boolean status;
{
  int n, num_children;
  Widget *children;
  Arg args[2];

  if (XmIsManager(widget))
  {
    n = 0;
    XtSetArg(args[n], XmNchildren, &children); n++;
    XtSetArg(args[n], XmNnumChildren, &num_children); n++;
    XtGetValues(widget, args, n);

    for (n = 0; n < num_children; n++)
      STO(children[n], status);
  }

  if ((widget != text1) && (widget != Form1) && (widget != bb))
  {
    XtSetArg(args[0], XmNtraversalOn, status);
    XtSetValues(widget, args, 1);
  }
}
     
static void SetTraversalOn(status)
  Boolean status;
{
  STO(bb, status);
} 
