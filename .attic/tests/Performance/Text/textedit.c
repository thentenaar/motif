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
static char rcsid[] = "$XConsortium: textedit.c /main/7 1995/07/13 17:26:33 drk $"
#endif
#endif
/*
 *	file: textedit.c
 *
 *	Has a Multi line Text widget under Form. The XmNcolumns resource is being
 *	set to 80 at creation time, and XmNrows to 24. By default, wordWrap is on.
 */

/*  Standard C headers  */
#include <math.h>

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <testlib.h>

#define	TEXT1_TOPCHAR	1
#define	NUM_COLUMNS	80
#define	NUM_ROWS	24

#define	MAX_NUM_LINES	200
#define	MAX_BUF_SIZE	40000
#define	MAX_LINE_SIZE	110

#define	MAX_COMM_INDX	7
#define	MAX_LINES	200
#define	MIN_COLS	30
#define	MAX_COLS	100
#define	NUM_INDX	14
#define	MAX_NUM_SPACE	6

char	buffer[150];
#define	TEXT1_X		10
#define	TEXT1_Y		10

static void	fill_text_buf();
static void  quitCB();

/*  Global Variables  */

char	*common_buf[] = {

	"30 CHARS!!",
	"40 CHARS XXXXXXXXX!!",
	"50 CHARS XXXXXXXX 1XXXXXXXXX!!",
	"60 CHARS XXXXXXXX 1XXXXXXXX 2XXXXXXXXX!!",
	"70 CHARS XXXXXXXX 1XXXXXXXX 2XXXXXXXX 3XXXXXXXXX!!",
	"80 CHARS XXXXXXXX 1XXXXXXXX 2XXXXXXXX 3XXXXXXXX 4XXXXXXXXX!!",
	"90 CHARS XXXXXXXX 1XXXXXXXX 2XXXXXXXX 3XXXXXXXX 4XXXXXXXX 5XXXXXXXXX!!",
	"100 CHARS XXXXXXXX 1XXXXXXXX 2XXXXXXXX 3XXXXXXXX 4XXXXXXXX 5XXXXXXXX 6XXXXXXXX!!"

};

char	*common1 = "This is Line #";

char text_string[MAX_BUF_SIZE + 1];

Widget Form1;
Widget Text1;
Arg args[20];
int n;

void myQuit()
{
	printf("Begin exiting ... please standby ... \n");
	XtDestroyWidget(Shell1);
	fflush(stdout);
	exit(0);
}


main (argc, argv)
int argc;
char **argv;
{
	Widget SWin;
	Display			*display;
	XFontStruct	*newfont;
	XmFontList	newfontlist;
	Widget	quit_button;
	FILE	*infp;
	char	tmpbuf[MAX_LINE_SIZE+1];
	int		num_lines, line_num, num_cols;
	char	c;
	int		word_wrap_val;
	Bool	word_wrap, force_wrap;

	text_string[0] = buffer[0] = '\0';
	num_lines = MAX_NUM_LINES;
	num_cols = MAX_COLS;
	word_wrap_val = 0;
	word_wrap = force_wrap = False;

	if (argc > 1)
		while (--argc > 0) {

			argv++;
			c = **argv;
			if (c != '-')
				continue;
			c = *(*argv + 1);

			switch(c) {

				case 'c':
					num_cols = atoi(*++argv);
					break;
				case 'r':
					num_lines = atoi(*++argv);
					break;
				case 'w':
					word_wrap_val = atoi(*++argv);
					force_wrap = True;
					break;
				case 'h':
					print_usage();
					exit(0);
				default:
					break;

			}
			argc--;
		}
	if (num_lines <= 0 || num_lines > MAX_NUM_LINES)
		num_lines = MAX_NUM_LINES;
	if (num_cols <= 0 || num_cols > MAX_COLS)
		num_cols = MAX_COLS;
	if (force_wrap == True)
		word_wrap = (word_wrap_val == 0) ? False : True;
	else
		word_wrap = True;

	fill_text_buf(num_lines, num_cols);


	/*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    n = 0;
	XtSetArg(args[n], XtNwidth, 800);  n++;
	XtSetArg(args[n], XtNheight, 450);  n++;
    XtSetValues(Shell1, args, n);
    

	/* create main form */
	n = 0;
	Form1 = XmCreateForm (Shell1, "Form1", args, n);
	XtManageChild (Form1);

	/* create multi line text widget */
	n = 0;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	XtSetArg(args[n], XmNverifyBell, False); n++;
	XtSetArg(args[n], XmNvalue, text_string); n++;
	newfont = XLoadQueryFont(XtDisplay(Shell1),"9x15");
	newfontlist = XmFontListCreate(newfont, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNfontList, newfontlist);  n++;
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	num_lines = (num_lines > NUM_ROWS) ? NUM_ROWS : num_lines;
	XtSetArg(args[n], XmNrows, num_lines);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNwordWrap, word_wrap);  n++;
	Text1 = XmCreateText(Form1, "Text1", args, n);
	XtManageChild(Text1);

	XmTextSetTopCharacter(Text1, (XmTextPosition)TEXT1_TOPCHAR);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
	quit_button = XmCreatePushButtonGadget(Form1, "Quit", args, n);
	XtManageChild(quit_button);
	XtAddCallback(quit_button, XmNactivateCallback, quitCB, NULL);

	XtRealizeWidget(Shell1);

	XtAppMainLoop(app_context);
}


static void  quitCB(w, client_data, call_data)
Widget	w;
XtPointer  client_data;
XtPointer  call_data;
{
	printf("Exiting...Please Standby...\n");
	XtDestroyWidget(Shell1);
	fflush(stdout);
	exit(0);
}


static void	fill_text_buf(num_rows, num_cols)
int	num_rows,
	num_cols;
{

	int		i, j;
	char	numbuf[6];
	int		first_time = 1;
	int	common_indx;
	int		init_len;

	common_indx = (num_cols - MIN_COLS) / 10;
	if (common_indx < 0 || common_indx > MAX_COMM_INDX)
		common_indx = MAX_COMM_INDX;

	strcpy(buffer, common1);
	strcat(buffer, common_buf[common_indx]);
	init_len = strlen(buffer);

	for (i = 0; i < num_rows; i++) {

		sprintf(numbuf, "%d", i+1);
		insert_chars(numbuf, first_time);
		strcat(text_string, buffer);
		if (first_time)
			first_time = 0;
		for (j = 0; j < 5; j++)
			numbuf[j] = '\0';
	}

}

insert_chars(new, first)
char	*new;
int		first;
{

	int	i, j, len;
	char	save[150];

	len = strlen(new);
	if (first)
		strcpy(save, &buffer[NUM_INDX]);
	else
		strcpy(save, &buffer[NUM_INDX + MAX_NUM_SPACE]);

	for (i = NUM_INDX,j = 0; j < len ; i++, j++)
		buffer[i] = new[j];

	if (len < MAX_NUM_SPACE) 
		for (j = 0; j < MAX_NUM_SPACE - len; j++)
			buffer[i++] = ' ';

	for (j = 0; save[j] != '\0'; j++, i++)
		buffer[i] = save[j];
	if (first)
		buffer[i++] = '\n';
	buffer[i] = '\0';

}


print_usage()
{
	printf("Usage: textedit [-c ncols] [-r nrows] [-w wrap_value] [-h]\n");
	printf("\tWhere - \n");
	printf("\t\tncols - no. of columns(chars) displayed in the text widget\n");
	printf("\t\tnrows - no. of rows of text displayed at a time\n");
	printf("\t\twrap_value - 0=wordwrap is False; otherwise wordwrap is True\n");
	printf("\t\t\"-h\" - will display this message\n");
}
