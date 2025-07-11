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
static char rcsid[] = "$XConsortium: Navigation13.c /main/7 1995/07/13 19:41:43 drk $"
#endif
#endif


#include <X11/Xlib.h>
#include <testlib.h>
#define MGR_CT 14
#define PRIM_CT 20
#define TEXT_CT 2
#define LABEL_CT 4

/* forward declarations */
#ifndef MOTIF1_1
static void PrintInfoProc();
void PrintResults();
void TopLevelFocus (Widget, XtPointer, XtPointer);
#endif

void main (argc, argv)
unsigned int argc;
char **argv;

{
    Arg args[15];
    int n;
    Widget child[20];
    XmString tcs;
    Widget managerWidgets[MGR_CT];
    Widget primitiveWidgets[PRIM_CT];
    Widget textWidgets[TEXT_CT];
    Widget labelWidgets[LABEL_CT];
    int mgrCt, primCt, textCt;
    static XtActionsRec newAction[] = {"PrintInfo", PrintInfoProc};

    Widget test_returns [2];
    Widget expected_returns[2];

    static char newMgrXl[] = 
		"~m ~a <Key>Tab: PrintInfo() ManagerGadgetNextTabGroup()";
    static char newPrimXl[] = 
		"~m ~a <Key>Tab: PrintInfo() PrimitiveNextTabGroup()";
    static char newTextXl[] = 
		"~m ~a <Key>Tab: PrintInfo() next-tab-group()";
    int i;

    mgrCt = 0;
    primCt = 0;
    textCt = 0;

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
     
    managerWidgets[0] = 
      XmCreateBulletinBoard(Shell1, "bb", (ArgList) args, 0);
    mgrCt++;

#ifndef MOTIF1_1
    XtAddCallback (managerWidgets[0], XmNfocusCallback, TopLevelFocus, NULL);
#endif

/* We want to test navigation across different widgets on the Bulletin
	Board as well as Navigation within the Frames within the BB, thus
	we are creating a set of three frames within the Bulleting Board,
	and they will each have a set of buttons within them.  */



	 n = 0;
	 XtSetArg(args[n], XmNx, 10); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 managerWidgets[1] = XmCreateFrame (managerWidgets[0], 
					      "Frame1", args, n);
         mgrCt++;

/* Let's make this Frame contain a RowColumn with three buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
         XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
         XtSetArg(args[n], XmNnumColumns, 1); n++;
         managerWidgets[2] = 
	   XmCreateRowColumn(managerWidgets[1], 
			     "RC1", (ArgList) args, n);
         mgrCt++;

#ifndef MOTIF1_1
	n = 0;
        XtSetArg(args[n], XmNlabelString, 
		XmStringCreateSimple ("RC1"));                         n++;
        XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);        n++;
        XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
        labelWidgets[0] = XmCreateLabel(managerWidgets[1], "label0", args, n);
        XtManageChild(labelWidgets[0]);
#endif

/* Create the children of the row column manager. The children are
	buttons. */

	 n = 0;
         XtSetArg(args[n], XmNtraversalOn, True); n++;
         XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
         XtSetArg(args[n], XmNhighlightThickness, 2); n++;
         primitiveWidgets[0]  = 
	   XmCreatePushButton(managerWidgets[2], 
			      "btn1",(ArgList)args, n);
         primitiveWidgets[1] 
	   = XmCreatePushButton(managerWidgets[2], 
				"btn2", (ArgList) args, n);
         primitiveWidgets[2] = 
	   XmCreatePushButton(managerWidgets[2], 
			      "btn3", (ArgList) args, n);

         n = 0;
         tcs = XmStringLtoRCreate("Toggle1", XmSTRING_DEFAULT_CHARSET);
         XtSetArg(args[n], XmNlabelString, tcs);  n++;
         XtSetArg(args[n], XmNindicatorOn, True);  n++;
         primitiveWidgets[3] = 
	   XmCreateToggleButtonGadget(managerWidgets[2], 
				      "ToggleButtnG1",(ArgList) args, n);
         if(tcs) XmStringFree(tcs);

/* Create the other Frame/Children combos. */
/* First the Frame */
	 n = 0;
	 XtSetArg(args[n], XmNx, 100); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 managerWidgets[3] = XmCreateFrame (managerWidgets[0], 
						  "Frame2", args, n);
         mgrCt++;

/* Let's make this Frame contain a RowColumn with two buttons. */
	 n = 0;
         XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
         XtSetArg(args[n], XmNnumColumns, 1); n++;
         managerWidgets[4] = 
	    XmCreateRowColumn(managerWidgets[3], 
	                      "RC2", (ArgList) args, n);
	 mgrCt++;

#ifndef MOTIF1_1
	n = 0;
        XtSetArg(args[n], XmNlabelString, 
		XmStringCreateSimple ("RC2"));                         n++;
        XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);        n++;
        XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
        labelWidgets[1] = XmCreateLabel(managerWidgets[3], "label1", args, n);
        XtManageChild(labelWidgets[1]);
#endif

/* create the  children now. */
         n = 0;
         XtSetArg(args[n], XmNtraversalOn, True); n++;
         XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
         XtSetArg(args[n], XmNhighlightThickness, 2); n++;
         primitiveWidgets[4] =
	   XmCreatePushButton(managerWidgets[4], 
			      "misc2", (ArgList) args, n);
         primitiveWidgets[5] = 
	   XmCreatePushButton(managerWidgets[4], 
			      "misc1", (ArgList) args, n);

/* Lets do a couple of  toggles now... */
         n = 0;
         tcs = XmStringLtoRCreate("Toggle21", XmSTRING_DEFAULT_CHARSET);
         XtSetArg(args[n], XmNlabelString, tcs);  n++;
         XtSetArg(args[n], XmNindicatorOn, True);  n++;
         primitiveWidgets[6] = 
	   XmCreateToggleButtonGadget(managerWidgets[4], 
				      "Toggle21",(ArgList) args, n);
         if(tcs) XmStringFree(tcs);
/* and another... */
         n = 0;
         tcs = XmStringLtoRCreate("Toggle22", XmSTRING_DEFAULT_CHARSET);
         XtSetArg(args[n], XmNlabelString, tcs);  n++;
         XtSetArg(args[n], XmNindicatorOn, True);  n++;
         primitiveWidgets[7] = 
	   XmCreateToggleButtonGadget(managerWidgets[4], 
				      "Toggle22",(ArgList) args, n);
         if(tcs) XmStringFree(tcs);
/*Two down, many to go... */

	 n = 0;
	 XtSetArg(args[n], XmNx, 200); n++;
	 XtSetArg(args[n], XmNy, 10); n++;
	 managerWidgets[5] = XmCreateFrame (managerWidgets[0], 
						  "Frame3", args, n);
         mgrCt++;
/* Let's make this Frame contain a RowColumn with six buttons.
	All the buttons will be added to the Tab Group. */

	 n = 0;
         XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
         XtSetArg(args[n], XmNnumColumns, 2); n++;
         managerWidgets[6] = 
	   XmCreateRowColumn(managerWidgets[5], 
			     "RC3", (ArgList) args, n);
         mgrCt++;

#ifndef MOTIF1_1
	n = 0;
        XtSetArg(args[n], XmNlabelString, 
		XmStringCreateSimple ("RC3"));                         n++;
        XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);        n++;
        XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
        labelWidgets[2] = XmCreateLabel(managerWidgets[5], "label2", args, n);
        XtManageChild(labelWidgets[2]);
#endif

/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   primitiveWidgets[8]
     = XmCreatePushButton(managerWidgets[6], 
			  "3btn1", (ArgList) args, n);
   primitiveWidgets[9]
     = XmCreatePushButton(managerWidgets[6], 
			  "3btn2", (ArgList) args, n);
   primitiveWidgets[10]
     = XmCreatePushButton(managerWidgets[6],
			  "3btn3", (ArgList) args, n);
   primitiveWidgets[11]
     = XmCreatePushButton(managerWidgets[6],
			  "3btn4", (ArgList) args, n);
   primitiveWidgets[12]
     = XmCreatePushButton(managerWidgets[6],
			  "3btn5", (ArgList) args, n);
   primitiveWidgets[13]
     = XmCreatePushButton(managerWidgets[6],
			  "3btn6", (ArgList) args, n);

/* Toggle another... */
    n = 0;
    tcs = XmStringLtoRCreate("Toggle31", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    primitiveWidgets[14] = 
      XmCreateToggleButtonGadget(managerWidgets[6],
				 "Toggle31",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);

    n = 0;
    tcs = XmStringLtoRCreate("Toggle32", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    primitiveWidgets[15]
      = XmCreateToggleButtonGadget(managerWidgets[6],
				   "Toggle32",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);

    n = 0;
    tcs = XmStringLtoRCreate("Toggle33", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNindicatorOn, True);  n++;
    primitiveWidgets[16]
      = XmCreateToggleButtonGadget(managerWidgets[6],
				   "Toggle33",(ArgList) args, n);
    if(tcs) XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNx, 375); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    managerWidgets[7] = XmCreateFrame (managerWidgets[0], 
					     "Frame4", args, n);
    mgrCt++;

/* Let's make this Frame contain a RowColumn with two buttons and two toggles.
	All the buttons will be added to the Tab Group. */

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    managerWidgets[8] = 
      XmCreateRowColumn(managerWidgets[7], 
			"RC4", (ArgList) args, n);
    mgrCt++;

#ifndef MOTIF1_1
	n = 0;
        XtSetArg(args[n], XmNlabelString, 
		XmStringCreateSimple ("RC4"));                         n++;
        XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);        n++;
        XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
        labelWidgets[3] = XmCreateLabel(managerWidgets[7], "label3", args, n);
        XtManageChild(labelWidgets[3]);
#endif

/* create the  children now. */
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   primitiveWidgets[17] 
     = XmCreatePushButton(managerWidgets[8],
			  "btn41", (ArgList) args, n);
   primitiveWidgets[18] 
     = XmCreatePushButton(managerWidgets[8],
			  "btn42", (ArgList) args, n);

   n = 0;
   tcs = XmStringLtoRCreate("Toggle41", XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   XtSetArg(args[n], XmNindicatorOn, True);  n++;
   primitiveWidgets[19]
     = XmCreateToggleButtonGadget(managerWidgets[8],
				  "Toggle41",(ArgList) args, n);
   if(tcs) XmStringFree(tcs);

/* Now lets make two more RCs with nothing on them! */

   n = 0;
   XtSetArg (args[n], XmNx, 485); n++;
   XtSetArg (args[n], XmNy, 10); n++;
   XtSetArg(args[n], XmNborderWidth, 5); n++;
   XtSetArg(args[n], XmNheight, 5); n++;
   XtSetArg(args[n], XmNwidth, 5); n++;
   managerWidgets[9] = XmCreateFrame (managerWidgets[0], 
					    "Frame5", args, n);
   mgrCt++;

   n = 0;
   XtSetArg(args[n], XmNborderWidth, 5); n++;
   XtSetArg(args[n], XmNwidth, 20); n++;
   managerWidgets[10] = 
     XmCreateRowColumn(managerWidgets[9], 
		       "RC5", (ArgList) args, n);
   mgrCt++;

   n = 0;
   XtSetArg (args[n], XmNx, 550); n++;
   XtSetArg (args[n], XmNy, 10); n++;
   XtSetArg(args[n], XmNborderWidth, 5); n++;
   XtSetArg(args[n], XmNheight, 5); n++;
   XtSetArg(args[n], XmNwidth, 5); n++;
   managerWidgets[11] = XmCreateFrame (managerWidgets[0], 
					    "Frame6", args, n);
   mgrCt++;

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 5); n++;
    XtSetArg(args[n], XmNwidth, 5); n++;
    managerWidgets[12] 
      = XmCreateRowColumn(managerWidgets[11],
			  "RC6", (ArgList) args, n);

    n = 0;
    XtSetArg (args[n], XmNx, 600); n++;
    XtSetArg (args[n], XmNy, 10); n++;
    managerWidgets[13] = XmCreateForm (managerWidgets[0], 
					    "Form1", args, n);
    mgrCt++;

    n = 0;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
    /* navigationType is XmTAB_GROUP by default */
    textWidgets[0] = XmCreateText (managerWidgets[13], 
					  "text1", args, n);

    n = 0;
    XtSetArg (args[n], XmNcolumns, 11); n++;
    /* navigationType is XmTAB_GROUP by default */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, textWidgets[0]); n++;
    textWidgets[1] = XmCreateTextField (managerWidgets[13], 
					  "text2", args, n);

    for (i = 0; i <= MGR_CT -1; i++)
      XtManageChild (managerWidgets[i]);
    for (i = 0; i <= PRIM_CT -1; i++)
      XtManageChild (primitiveWidgets[i]);
    for (i = 0; i <= TEXT_CT -1; i++)
      XtManageChild (textWidgets[i]);

    XtRealizeWidget (Shell1);

#ifndef MOTIF1_1
    /* begin tests for XmGetFocusWidget and XmGetTabGroup */

    /* first, create an action routine which will call the functions
       under test before calling the normal action routines for Tab.
       We need to reset all the managers and primitives as follows:
          --- existing translations ---
       all managers          ~m ~a <Key>Tab:ManagerGadgetNextTabGroup()\n\
       text                  ~m ~a <Key>Tab:next-tab-group()\n\
       primitive             ~m ~a <Key>Tab:PrimitiveNextTabGroup()\n\
          --- new translations ---
     */

     XtAppAddActions (app_context, newAction, 1);

     /* do an translation override for every widget */
    
    for (i = 0; i <= MGR_CT - 1; i++)
      XtOverrideTranslations (managerWidgets[i],
			      XtParseTranslationTable (newMgrXl));

    for (i = 0; i <= PRIM_CT - 1; i++)
      if (!XmIsGadget(primitiveWidgets[i]) )
         XtOverrideTranslations (primitiveWidgets[i],
			      XtParseTranslationTable (newPrimXl));

    for (i = 0; i <= TEXT_CT - 1; i++)
      XtOverrideTranslations (textWidgets[i],
			      XtParseTranslationTable (newTextXl));

    CommonPause();

#endif


    XtDestroyWidget (managerWidgets[0]);
    test_returns[0] = XmGetTabGroup (Shell1);
    test_returns[1] = XmGetFocusWidget (Shell1);
    expected_returns[0] = NULL;
    expected_returns[1] = NULL;

    /* tests 1 and 2 */
    printf ("NO TRAVERSIBLE WIDGETS OR TAB GROUPS\n");
    PrintResults (test_returns, expected_returns);
    printf ("\n");

    n = 0;
    managerWidgets[0] = XmCreateBulletinBoard (Shell1, "bb", args, n);
    XtManageChild (managerWidgets[0]);
    test_returns[0] = XmGetTabGroup (managerWidgets[0]);
    test_returns[1] = XmGetFocusWidget (Shell1);
    expected_returns[0] = managerWidgets[0];
    expected_returns[1] = NULL;
 
    /* tests 3 and 4 */
    printf ("MANAGER ONLY\n");
    PrintResults (test_returns, expected_returns);
    printf ("\n");

    XtDestroyWidget (managerWidgets[0]);
    n = 0;
    XtSetArg (args[n], XmNnavigationType, XmNONE); n++;
    managerWidgets[0] = XmCreateBulletinBoard (Shell1, "bb", args, n);
    XtManageChild (managerWidgets[0]);
    test_returns[0] = XmGetTabGroup (Shell1);
    test_returns[1] = XmGetFocusWidget (Shell1);
    expected_returns[0] = NULL;
    expected_returns[1] = NULL;

    /* tests 5 and 6 */
    printf ("NON-TAB-GROUP MANAGER ONLY\n");
    PrintResults (test_returns, expected_returns);
    printf ("\n");

    XtDestroyWidget (managerWidgets[0]);
    n = 0;
    managerWidgets[0] = XmCreateBulletinBoard (Shell1, "bb", args, n);
    XtManageChild (managerWidgets[0]);
    n = 0;
    primitiveWidgets[0] = XmCreatePushButton (managerWidgets[0], "pb", args, n);    XtSetSensitive (primitiveWidgets[0], False);
    XtManageChild (primitiveWidgets[0]);

    test_returns[0] = XmGetTabGroup (primitiveWidgets[0]);
    test_returns[1] = XmGetFocusWidget (Shell1);
    expected_returns[0] = managerWidgets[0];
    expected_returns[1] = NULL;

    /* tests 7 and 8 */
    printf ("INSENSITIVE CHILD\n");
    PrintResults (test_returns, expected_returns);
    printf ("\n");
    fflush (stdout);

    CommonPause();
    CommonPause(); /* exit */

    XtAppMainLoop(app_context);
}


#ifndef MOTIF1_1
static void PrintInfoProc (w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;

{
  if (w) {
    printf ("Widget %s had the focus.\n", 
		XtName(XmGetFocusWidget(w)));
    printf ("Leaving tab group %s\n",
		XtName(XmGetTabGroup(w)));
  }
  fflush (stdout);

}
void TopLevelFocus (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget fw;
 
  fw = XmGetFocusWidget(w);
  if (fw) {
    printf ("FOCUS IN: on %s\n", XtName(fw));
    fflush (stdout);
  }
}

#endif

void PrintResults (Widget *test_results, Widget *expected_results)
{
	static int counter = 1;
	int i;

	for (i = 0; i <= 1; i++) {
        	printf ("Test %d:\n", counter++);
		printf ("Expected: ");
		if (expected_results[i]) 
			printf ("%s\t", XtName (expected_results[i]));
		else    printf ("NULL\t");
		printf ("Actual: ");
		if (test_results[i])
			printf ("%s\t", XtName (test_results[i]));
		else    printf ("NULL\t");
		printf ("Result: ");
		printf ("%s\n", expected_results[i] == test_results[i] ?
				"Passed" : "FAILED");
	}
        fflush (stdout);
}




