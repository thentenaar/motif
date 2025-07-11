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
static char rcsid[] = "$XConsortium: DNDText1.c /main/9 1995/07/14 11:55:02 drk $"
#endif
#endif
/*
 *	file: DNDText1.c
 *
 *	Has a Multi line Text widget under Form. The XmNcolumns resource is being
 *	set to 80 at creation time, and XmNrows to 24. A program to be run as one
 *	of the clients in a Drag And Drop transaction.
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


static void	quitCB();


/*  Global Variables  */
char	*test_string = "This is a test string";
Widget	Form1;
Widget	Text1;

Boolean	use_instruction_box;


main (argc, argv)
int argc;
char **argv;
{
	XFontStruct	*newfont;
	XmFontList	newfontlist;
	Widget		quit_button;
	Arg			args[20];
	int			n;


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
	Form1 = XmCreateForm(Shell1, "Form1", args, n);
	XtManageChild(Form1);

	/* create multi line text widget */
	newfont = XLoadQueryFont(XtDisplay(Shell1),"9x15");
	if (newfont != NULL)
		newfontlist = XmFontListCreate(newfont, XmSTRING_DEFAULT_CHARSET);
	n = 0;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNverifyBell, False); n++;
	XtSetArg(args[n], XmNvalue, test_string); n++;
	if (newfont != NULL)
		XtSetArg(args[n], XmNfontList, newfontlist);  n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNrows, NUM_ROWS);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	Text1 = XmCreateText(Form1, "Text1", args, n);
	XtManageChild(Text1);

	XmTextSetTopCharacter(Text1, (XmTextPosition)TEXT1_TOPCHAR);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 200); n++;
	quit_button = XmCreatePushButtonGadget(Form1, "Quit", args, n);
	XtManageChild(quit_button);
	XtAddCallback(quit_button, XmNactivateCallback, quitCB, NULL);

	XtRealizeWidget(Shell1);

	if (use_instruction_box == True) {

		CommonPause();

	}

	XtAppMainLoop(app_context);

}


static void	quitCB(w, client_data, call_data)
Widget		w;
XtPointer	client_data;
XtPointer	call_data;
{

	printf("Exiting...Please Standby...\n");
	XtDestroyWidget(Shell1);
	fflush(stdout);

	exit(0);

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
