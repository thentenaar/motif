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
static char rcsid[] = "$XConsortium: DNDLabel1.c /main/9 1995/07/14 11:53:50 drk $"
#endif
#endif
/*
 *	File:	DNDLabel1.c
 *
 *	A program to be run as one of the clients to test Drag and Drop
 *
 */

#include <testlib.h>

/*  Global Variables  */
Widget    RowColumn1;
Widget    Label1;

Boolean	use_instruction_box;



static char dragTranslations[] =
"#override \
  Ctrl<Key>d: ProcessDrag()";



void  main (argc, argv)
int		argc;
char	**argv;
{

    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs;
    XtTranslations     parsed_xlations;


	use_instruction_box = True;

    CommonTestInit(argc, argv);

	if (UserData != NULL)
		ProcessCommandArgs();
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetValues(Shell1, args, n);

    XtRealizeWidget(Shell1);

    /* create and realize a rowcolumn container widget */
    n = 0;
    XtSetArg(args[n], XmNisAligned, FALSE);  n++;
    RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);
    XtManageChild(RowColumn1);

    /* make a string label widget */
    n = 0;
    tcs = XmStringLtoRCreate("Two-Line\nLabel String", 
							 XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
    Label1 = XmCreateLabel(RowColumn1, "Label1", args, n);
    XtManageChild(Label1);

	XmStringFree(tcs);

	if (use_instruction_box == True) {

		CommonPause();
		CommonPause();

     parsed_xlations = XtParseTranslationTable(dragTranslations);

     n=0;
     XtSetArg (args[n], XmNtranslations, parsed_xlations); n++;
     XtSetValues (Label1, args, n);

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

