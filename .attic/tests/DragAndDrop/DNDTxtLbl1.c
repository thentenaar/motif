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
static char rcsid[] = "$XConsortium: DNDTxtLbl1.c /main/8 1995/07/14 11:55:24 drk $"
#endif
#endif
/*
 *	file: TextLabel1.c
 *
 *	Has a two line Label widget under Form. Also has a Multi line Text 
 *	widget under Form. The XmNcolumns resource is being set to 50 at 
 *	creation time, and XmNrows to 10. A program to be run either as one
 *	of the clients in a Drag And Drop transaction, or as both the initiator
 *	and the receiver for testing Drag And Drop transaction.
 */

/*  Standard C headers  */
#include <math.h>

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <testlib.h>


#define	TEXT1_TOPCHAR	1
#define	NUM_COLUMNS	50
#define	NUM_ROWS	10

#define	TEXT1_X		10
#define	TEXT1_Y		10


/*  Global Variables  */

char	*test_string = "This is a test string";

Widget	Form1;
Widget 	Text1;
Widget 	Label1;
Widget 	Label2;
Widget  PushB1;

Boolean	use_instruction_box;


main (argc, argv)
int argc;
char **argv;
{
	XFontStruct	*newfont;
	XmFontList	newfontlist;
    XmString    tcs;
	Arg 		args[MAX_ARGS];
	int 		n;


	use_instruction_box = True;

    CommonTestInit(argc, argv);

	if (UserData != NULL)
		ProcessCommandArgs();
    
    n = 0;
	XtSetArg(args[n], XtNwidth, 500);  n++;
	XtSetArg(args[n], XtNheight, 250);  n++;
    XtSetValues(Shell1, args, n);
    
	/* create main form */
	n = 0;
	Form1 = XmCreateForm (Shell1, "Form1", args, n);
	XtManageChild (Form1);

    /* make a string label widget */
    n = 0;
    tcs = XmStringLtoRCreate("Two-Line\nLabel String",
	  						 XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;

    Label1 = XmCreateLabel(Form1, "Label1", args, n);
    XtManageChild(Label1);

	XmStringFree(tcs);

	/* create multi line text widget */
	n = 0;
	XtSetArg(args[n], XmNverifyBell, False); n++;
	XtSetArg(args[n], XmNvalue, test_string); n++;
	newfont = XLoadQueryFont(XtDisplay(Shell1),"9x15");
	newfontlist = XmFontListCreate(newfont, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNfontList, newfontlist);  n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNrows, NUM_ROWS);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNtopWidget, Label1); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
						
	Text1 = XmCreateText(Form1, "Text1", args, n);
	XtManageChild(Text1);

	XmTextSetTopCharacter(Text1, (XmTextPosition)TEXT1_TOPCHAR);

	XtRealizeWidget(Shell1);

	if (use_instruction_box == True) {

		CommonPause();

		CommonPause();

    	/* make a new label widget */
    	n = 0;
    	tcs = XmStringLtoRCreate("One line", XmSTRING_DEFAULT_CHARSET);
    	XtSetArg(args[n], XmNlabelString, tcs);  n++;
    	XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    	XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopOffset, 5); n++;
		XtSetArg(args[n], XmNtopWidget, Label1); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNleftOffset, 5); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightOffset, 5); n++;
		XtSetArg(args[n], XmNborderWidth, 5); n++;
	
    	Label2 = XmCreateLabel(Form1, "Label2", args, n);
    	XtManageChild(Label2);

		XmStringFree(tcs);

		n = 0;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopOffset, 5); n++;
		XtSetArg(args[n], XmNtopWidget, Label2); n++;
		XtSetValues(Text1, args, n);

		CommonPause();
    	n = 0;
    	tcs = XmStringLtoRCreate("", XmSTRING_DEFAULT_CHARSET);
    	XtSetArg(args[n], XmNlabelString, tcs);  n++;
    	XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    	XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
    	XtSetArg(args[n], XmNheight, 30); n++;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopOffset, 5); n++;
		XtSetArg(args[n], XmNtopWidget, Label2); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNleftOffset, 5); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightOffset, 5); n++;
		XtSetArg(args[n], XmNborderWidth, 5); n++;
	
    	PushB1 = XmCreatePushButton(Form1, "PushB1", args, n);
    	XtManageChild(PushB1);

		XmStringFree(tcs);

		n = 0;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopOffset, 5); n++;
		XtSetArg(args[n], XmNtopWidget, PushB1); n++;
		XtSetValues(Text1, args, n);

	CommonPause();
	CommonPause();


	}

	XtAppMainLoop(app_context);

}


#define	WORD_LEN	32

ProcessCommandArgs()
{

	char	*user_data;
	char	next_word[WORD_LEN + 1];
	int     num_spaces;

	user_data = UserData;

	while (*user_data != '\0') {

		get_next_word(user_data, next_word, &num_spaces);
		user_data += (strlen(next_word) + num_spaces);

		if (strcmp(next_word, "-noinstruct") == 0) {
			use_instruction_box = False;
			continue;

		}

	}

	free(UserData);

}


get_next_word(source, dest, spaces)
char	*source;
char	*dest;
int		*spaces;
{

	int	n;
	int	space_count;

	space_count = 0;

	while (isspace(*source)) {
		source++;
		space_count++;
	}

	n = 0;
	while (!isspace(*source) && *source != '\0' && n < WORD_LEN) {
		*dest++ = *source++;
		n++;
	}
	*dest = '\0';
	*spaces = space_count;

}
