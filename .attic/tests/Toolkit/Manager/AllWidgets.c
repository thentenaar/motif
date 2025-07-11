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
static char rcsid[] = "$XConsortium: AllWidgets.c /main/8 1995/07/13 18:25:04 drk $"
#endif
#endif

/*
 *	file:	AllWidgets.c
 *
 *	Program to create different manager widgets with/without children to 
 *	compare among other things, visuals between 1.1 and 1.2.
 *
 *	This test shows the following PIRS:
 *
 *	PIR4193
 *	PIR4194
 *	PIR4195
 *	PIR4197
 *	PIR4198
 *	PIR4199
 *	PIR4201
 *	PIR4202
 *	PIR4203
 *	PIR4204
 *	PIR4205
 *	PIR4206
 *	PIR4207
 *	PIR4210
 *	PIR4211
 *	PIR4212
 *
 */

#include <testlib.h>

#define	MAXARGS	20
#define	ITEMS	10
#define	VISIBLE	5

Widget	manager, weegee;

char *items[] = { "one", "two", "three", "four", "five",
	     "six", "seven", "eight", "nine", "ten"	};

enum { BULLETINBOARD, FORM, FRAME, PANEDWINDOW, ROWCOLUMN };

unsigned char	manager_needed;


void main(int argc, char **argv)
{
	XmString	List1[ITEMS];
	Arg			args[MAXARGS];
	Cardinal 	n;


	manager_needed = BULLETINBOARD;

	CommonTestInit(argc, argv);

    if (UserData != NULL)
		ProcessCommandArgs();

#ifdef MOTIF1_1
	n = 0;
	XtSetArg(args[n], XmNwidth, 50); n++;
	XtSetArg(args[n], XmNheight, 50); n++;
	XtSetValues(Shell1, args, n);
#endif

	n = 0;
	XtSetArg(args[n], XmNmarginWidth, 25); n++;
	XtSetArg(args[n], XmNwidth, 250); n++;
	XtSetArg(args[n], XmNheight, 200); n++;

	if (manager_needed == BULLETINBOARD)
		manager = XmCreateBulletinBoard(Shell1, "Manager", args, n);
	else if (manager_needed == FORM)
		manager = XmCreateForm(Shell1, "Manager", args, n);
	else if (manager_needed == FRAME)
		manager = XmCreateFrame(Shell1, "Manager", args, n);
	else if (manager_needed == PANEDWINDOW)
		manager = XmCreatePanedWindow(Shell1, "Manager", args, n);
	else if (manager_needed == ROWCOLUMN)
		manager = XmCreateRowColumn(Shell1, "Manager", args, n);

	XtManageChild(manager);

	XtRealizeWidget(Shell1);

	CommonPause();

	CommonPause();

	for (n=0; n < ITEMS; n++)
		List1[n] = XmStringCreateLtoR(items[n], XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNwidth, 150); n++;
	XtSetArg(args[n], XmNwidth, 100); n++;
	weegee = XmCreateArrowButton(manager, "ArrowB", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 150); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	weegee = XmCreateArrowButtonGadget(manager, "ArrowBG", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 150); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	weegee = XmCreateBulletinBoard(manager, "BulletinB", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();
	
	n = 0;
	XtSetArg(args[n], XmNlabelString, XmStringCreateSimple("Label")); n++;
	weegee = XmCreateLabel(manager, "Label", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, 
			 XmStringCreateSimple("LabelGadget")); n++;
	weegee = XmCreateLabelGadget(manager, "LabelG", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNitems, List1); n++;
	XtSetArg(args[n], XmNitemCount, ITEMS); n++;
	XtSetArg(args[n], XmNvisibleItemCount, VISIBLE); n++;
	weegee = XmCreateList(manager, "List", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, XmStringCreateSimple("PushButton")); n++;
	weegee = XmCreatePushButton(manager, "PushButton", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("PushButtonGadget")); n++;
	weegee = XmCreatePushButtonGadget(manager, "PushButtonG", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNtitleString, XmStringCreateSimple("Scale")); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNmaximum, 100); n++;
	XtSetArg(args[n], XmNvalue, 10); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	weegee = XmCreateScale(manager, "Scale", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNitems, List1); n++;
	XtSetArg(args[n], XmNitemCount, ITEMS); n++;
	XtSetArg(args[n], XmNvisibleItemCount, VISIBLE); n++;
	weegee = XmCreateScrolledList(manager, "ScrolledList", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();
	
	n = 0;
	XtSetArg(args[n], XmNrows, 2);	n++;
	XtSetArg(args[n], XmNscrollVertical, True);	n++;
	XtSetArg(args[n], XmNscrollHorizontal, True);	n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);	n++;
	weegee = XmCreateScrolledText(manager, "ScrolledText", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	XtSetArg(args[n], XmNwidth, 150); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	weegee = XmCreateScrolledWindow(manager, "ScrolledW", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	weegee = XmCreateSeparator(manager, "Separator", NULL, 0);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	weegee = XmCreateSeparatorGadget(manager, "SeparatorG", NULL, 0);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNrows, 2); n++;
	XtSetArg(args[n], XmNcolumns, 20); n++;
	XtSetArg(args[n], XmNscrollVertical, True); n++;
	XtSetArg(args[n], XmNscrollHorizontal, True); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);	n++;
	weegee = XmCreateText(manager, "Text", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNcolumns, 20); n++;
	weegee = XmCreateTextField(manager, "TextF", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, 
			 XmStringCreateSimple("ToggleButton")); n++;
	weegee = XmCreateToggleButton(manager, "ToggleButton", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("ToggleButtonGadget")); n++;
	weegee = XmCreateToggleButtonGadget(manager, "ToggleBG", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNlabelString, XmStringCreateSimple("DrawnButton")); n++;
	weegee = XmCreateDrawnButton(manager, "DrawnButton", args, n);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);
	XtDestroyWidget(manager);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 150); n++;
	XtSetArg(args[n], XmNheight, 200); n++;
	weegee = XmCreateDrawingArea(Shell1, "DrawingArea", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n=0;
	XtSetArg (args[n], XmNwidth, 250); n++;
	XtSetArg (args[n], XmNheight, 200); n++;
	weegee = XmCreateBulletinBoard(Shell1, "BulletinB", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n=0;
	XtSetArg (args[n], XmNwidth, 250); n++;
	XtSetArg (args[n], XmNheight, 200); n++;
	weegee = XmCreateForm(Shell1, "Form", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 250); n++;
	XtSetArg(args[n], XmNheight, 200); n++;
	weegee = XmCreateFrame(Shell1, "Frame", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 250); n++;
	XtSetArg(args[n], XmNheight, 200); n++;
	weegee = XmCreatePanedWindow(Shell1, "PanedWindow", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNwidth, 250); n++;
	XtSetArg(args[n], XmNheight, 200); n++;
	weegee = XmCreateRowColumn(Shell1, "RowCol1", args, n);
	XtRealizeWidget(weegee);
	XtManageChild(weegee);

	CommonPause();

	CommonPause();

	XtDestroyWidget(weegee);

	CommonPause();

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

		if (strcmp(next_word, "-bb") == 0) {
			manager_needed = BULLETINBOARD;
			continue;

		}

		if (strcmp(next_word, "-bb") == 0) {
			manager_needed = BULLETINBOARD;
			continue;

		}

		if (strcmp(next_word, "-fo") == 0) {
			manager_needed = FORM;
			continue;

		}

		if (strcmp(next_word, "-fr") == 0) {
			manager_needed = FRAME;
			continue;

		}

		if (strcmp(next_word, "-pw") == 0) {
			manager_needed = PANEDWINDOW;
			continue;

		}

		if (strcmp(next_word, "-rc") == 0) {
			manager_needed = ROWCOLUMN;
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
