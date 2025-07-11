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
static char rcsid[] = "$XConsortium: DNDList1.c /main/9 1995/07/14 11:54:15 drk $"
#endif
#endif
/*
 *	File:	DNDList1.c
 *
 *	A program to be run as one of the clients to test Drag and Drop
 *
 */

#include <testlib.h>

#define NITEMS 9

/*  Global Variables  */

char *Istrings[] = {
    "DrawnButton",
	"Label",
    "List",
	"PushButton",
    "Separator",
	"ArrowButton",
    "ScrollBar",
	"Text",
    "ToggleButton"
};

Widget    List1;

Boolean	use_instruction_box;

void  main (argc, argv)
int		argc;
char	**argv;
{

    register int       	n;
    Arg                	args[MAX_ARGS];
    XmString           	ItemList[NITEMS];


	use_instruction_box = True;

    CommonTestInit(argc, argv);

	if (UserData != NULL)
		ProcessCommandArgs();

    /* convert the plain strings to fancy Xstrings */
    for (n = 0; n < NITEMS; n++)
		ItemList[n] = XmStringLtoRCreate(Istrings[n], 
										 XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlistSpacing, 5); n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2); n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2); n++;
    XtSetArg(args[n], XmNitems, ItemList); n++;
    XtSetArg(args[n], XmNitemCount, NITEMS); n++;
    XtSetArg(args[n], XmNvisibleItemCount, NITEMS); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;
    List1 = XmCreateList(Shell1, "List1", args, n);
    XtManageChild(List1);

    for (n = 0; n < NITEMS; n++)
		XmStringFree(ItemList[n]);

    XtRealizeWidget(Shell1);

	if (use_instruction_box == True) {

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
