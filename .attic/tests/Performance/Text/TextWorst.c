/* $XConsortium: TextWorst.c /main/4 1995/07/15 21:02:36 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

/*
 *	file: TextWorst.c
 *
 *	Has a Multi line Scrolled Text widget under Form. The XmNcolumns resource 
 *	and XmNrows resource are being set to fill in the entire screen. The
 *  wordWrap is off. By default, the text buffer is filled with the character
 *  'n' in "6x10" font if available, otherwise in "fixed" font.
 *
 *	Once the buffer is filled in, through the controls provided, the 
 *  buffer can be filled into the Text widget, it can be scrolled to the
 *  end, and then scrolled back to the beginning. For each of these
 *  operations, time taken is measured, and reported into the file
 *  TextWorst.out in the directory from where the test was run, or if
 *  that file can not be created in the current directory, it will be
 *  created under /tmp. The purpose of this test is to determine the
 *  worst performance of XmText widget.
 *
 */

/*  Standard C headers  */
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

/*  X headers  */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <testlib.h>

#define	NUM_COLUMNS		80
#define	NUM_ROWS		24
#define	MAX_LINE_LEN	255
#define	MAX_NAME_LEN	255

#define	TEXT1_X		10
#define	TEXT1_Y		10

#define DELTA_SHELL_WIDTH 	10
#define DELTA_SHELL_HEIGHT 	30
#define DELTA_BOTTOM_ROW 	50

#define DELTA_TEXT_WIDTH 	50
#define DELTA_TEXT_HEIGHT 	100

#define C_CHAR  'n'

#define NUM_PAGES	4


static void	fill_text_buf();
static void  	fillTextCB();
static void  	quitCB();
static void  	scrollToEndCB();
static void  	scrollToBeginCB();
static void 	PrintLapseTime();


/*  Global Variables  */

char infile_name[MAX_NAME_LEN + 1];
char *text_string;
char *small_text_string = "SMALL TEXT";

Widget Form1;
Widget Text1;
Widget fillText;
Widget scrollToBegin;
Widget scrollToEnd;

int	total_lines = 0;
Boolean use_word_wrap = False;

XmFontList	newfontlist;
XFontStruct	*newfont;

Arg args[MAX_ARGS];
int n;
FILE	*in_fp;
FILE	*outfp;
int		in_fd;


main (argc, argv)
int argc;
char **argv;
{

	Widget	quit;
	char        name[80];
	int		screen_number;
	int		displayWidth, displayHeight, numColumns, numRows;
	int		shellWidth, shellHeight;
	int		bottom_rowY;

	struct timeval act_time1, act_time2;
	struct timezone zone_time1, zone_time2;


	infile_name[0] = '\0';

	/*  initialize toolkit  */
    CommonTestInit(argc, argv);

	screen_number = XScreenNumberOfScreen(screen);
	displayWidth = XDisplayWidth(display, screen_number);
	displayHeight = XDisplayHeight(display, screen_number);
	bottom_rowY = displayHeight - DELTA_BOTTOM_ROW;
	shellWidth = displayWidth - DELTA_SHELL_WIDTH;
	shellHeight = displayHeight - DELTA_SHELL_HEIGHT;
	numColumns = (displayWidth - DELTA_TEXT_WIDTH)/6;
	numRows = (displayHeight - DELTA_TEXT_HEIGHT)/10;

printf("displayWidth = %d displayHeight = %d\n", displayWidth, displayHeight);
printf("shellWidth = %d shellHeight = %d\n", shellWidth, shellHeight);
printf("numColumns = %d numRows = %d\n", numColumns, numRows);
printf("bottom_rowY = %d\n", bottom_rowY);
fflush(stdout);

	if (UserData != NULL)
		ProcessCommandArgs();

    sprintf(name, "%s.out", argv[0]);
    if ((outfp = fopen(name, "w+")) == NULL) {

        fprintf(stderr, "Can not open output file %s.out. Trying in /tmp\n",
                argv[0]);
        sprintf(name, "/tmp/%s.out", argv[0]);
        if ((outfp = fopen(name, "w+")) == NULL) {
            fprintf(stderr, "Can not open output file /tmp/%s.out. Exiting\n",
                    argv[0]);
            exit(0);
        }

	}

    n = 0;
	XtSetArg(args[n], XtNwidth, shellWidth);  n++;
	XtSetArg(args[n], XtNheight, shellHeight);  n++;
    XtSetValues(Shell1, args, n);

/*	XtRealizeWidget(Shell1); */

	/* create main form */
	n = 0;

	Form1 = XmCreateForm (Shell1, "Form1", args, n);

	XtManageChild (Form1);

	/* Read the text file into a buffer */
	fill_text_buf(numRows, numColumns, NUM_PAGES);

	/* create multi line text widget */
	n = 0;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	newfontlist = CommonGetFontList("6x10");
	XtSetArg(args[n], XmNfontList, newfontlist);  n++;
	XtSetArg(args[n], XmNcolumns, numColumns);  n++;
	XtSetArg(args[n], XmNrows, numRows);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNscrollHorizontal, False); n++;
	XtSetArg(args[n], XmNeditable, False); n++;
    if (use_word_wrap == True) {
		XtSetArg(args[n], XmNwordWrap, True); n++;
	}
    Text1 = XmCreateScrolledText(Form1, "Text1", args, n);
	XtManageChild(Text1);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, bottom_rowY); n++;
	quit = XmCreatePushButtonGadget(Form1, "Quit", args, n);
	XtManageChild(quit);
	XtAddCallback(quit, XmNactivateCallback, quitCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 150); n++;
	XtSetArg(args[n], XmNy, bottom_rowY); n++;
	fillText = XmCreatePushButtonGadget(Form1, "Fill Text", args, n);
	XtManageChild(fillText);
	XtAddCallback(fillText, XmNactivateCallback, fillTextCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 300); n++;
	XtSetArg(args[n], XmNy, bottom_rowY); n++;
	scrollToEnd = XmCreatePushButtonGadget(Form1, "Scroll To End", args, n);
	XtManageChild(scrollToEnd);
	XtAddCallback(scrollToEnd, XmNactivateCallback, scrollToEndCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 450); n++;
	XtSetArg(args[n], XmNy, bottom_rowY); n++;
	scrollToBegin = XmCreatePushButtonGadget(Form1, "Scroll To Begin", args, n);
	XtManageChild(scrollToBegin);
	XtAddCallback(scrollToBegin, XmNactivateCallback, scrollToBeginCB, NULL);

	if (use_word_wrap == True)
		fprintf(outfp, "wordWrap is ON\n");
	else
		fprintf(outfp, "wordWrap is OFF\n");

	XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
	XtAppMainLoop(app_context);
}


static void PrintLapseTime(fp, first,second)
FILE	*fp;
struct timeval *first, *second;
{
     struct timeval lapsed;

     if (first->tv_usec > second->tv_usec) {
        second->tv_usec += 1000000;
        second->tv_sec--;
     }

     lapsed.tv_usec = second->tv_usec - first->tv_usec;
     lapsed.tv_sec = second->tv_sec - first->tv_sec;

     fprintf(fp, "lapsed time = %d.%06d sec\n\n",
            lapsed.tv_sec, lapsed.tv_usec%1000000);
     fflush(fp);

}


static void  quitCB(w, client_data, call_data)
Widget	w;
caddr_t  client_data;
caddr_t  call_data;
{

	int	i;

	XtFree(text_string);
	XtDestroyWidget(Shell1);
	fflush(outfp);
	fclose(outfp);
	exit(0);

}


static void  fillTextCB(w, client_data, call_data)
Widget	w;
caddr_t  client_data;
caddr_t  call_data;
{

	struct timeval act_time1, act_time2;
	struct timezone zone_time1, zone_time2;


	if (Text1 == NULL)
		return;

	fprintf(outfp, "Total Number of Lines in the textfile : %d\n\n", 
			total_lines);

	gettimeofday(&act_time1, &zone_time1);
	XmTextSetString(Text1, text_string);
	gettimeofday(&act_time2, &zone_time2);
	fprintf(outfp, "Time lapsed to set TextString : \n");
	PrintLapseTime(outfp, &act_time1, &act_time2);

	n = 0;
    XtSetArg(args[n], XmNsensitive, True); n++;
	XtSetValues(scrollToEnd, args, n);

}


static void  scrollToEndCB(w, client_data, call_data)
Widget	w;
caddr_t  client_data;
caddr_t  call_data;
{

	struct timeval act_time1, act_time2;
	struct timezone zone_time1, zone_time2;


	if (Text1 == NULL)
		return;

	gettimeofday(&act_time1, &zone_time1);
	XmTextScroll(Text1, total_lines - NUM_ROWS);
	gettimeofday(&act_time2, &zone_time2);
	fprintf(outfp, "Time lapsed scrolling to end of textfile : \n");
	PrintLapseTime(outfp, &act_time1, &act_time2);

	n = 0;
        XtSetArg(args[n], XmNsensitive, False); n++;
	XtSetValues(scrollToEnd, args, n);
	n = 0;
        XtSetArg(args[n], XmNsensitive, True); n++;
	XtSetValues(scrollToBegin, args, n);


}


static void  scrollToBeginCB(w, client_data, call_data)
Widget	w;
caddr_t  client_data;
caddr_t  call_data;
{

	struct timeval act_time1, act_time2;
	struct timezone zone_time1, zone_time2;


	if (Text1 == NULL)
		return;

	gettimeofday(&act_time1, &zone_time1);
	XmTextScroll(Text1, -(total_lines - NUM_ROWS));
	gettimeofday(&act_time2, &zone_time2);
	fprintf(outfp, "Time lapsed scrolling to beginning of textfile : \n");
	PrintLapseTime(outfp, &act_time1, &act_time2);

	n = 0;
        XtSetArg(args[n], XmNsensitive, False); n++;
	XtSetValues(scrollToBegin, args, n);
	n = 0;
        XtSetArg(args[n], XmNsensitive, True); n++;
	XtSetValues(scrollToEnd, args, n);

}


static void	fill_text_buf(num_lines, num_columns, num_pages)
int num_lines;
int num_columns;
int num_pages;
{
	
    int	i, j, k;
    char	*temp_string;

    if (text_string != NULL) {
	XtFree(text_string);
	text_string = NULL;
    }

    text_string = XtMalloc((num_lines * num_columns * num_pages) + 1);
    temp_string = text_string;
    
    for (i = 0; i < num_pages; i++) {
	for (j = 0; j < num_lines; j++) {
	    for (k = 0; k < num_columns - 1; k++) {
			
		*temp_string++ = C_CHAR;
	    }
	    *temp_string++ = '\n';
	    total_lines++;
	}
	*temp_string++ = '\n';
	total_lines++;      
    }
    *temp_string = '\0';

}

#define WORD_LEN    32

ProcessCommandArgs()
{

    char    *user_data;
    char    next_word[WORD_LEN + 1];
    int     num_spaces;

    user_data = UserData;

    while (*user_data != '\0') {

        get_next_word(user_data, next_word, &num_spaces);
        user_data += (strlen(next_word) + num_spaces);

		if (strcmp(next_word, "-use_word_wrap") == 0) {
			use_word_wrap = True;
			continue;
		}

    }

    free(UserData);

}


get_next_word(source, dest, spaces)
char    *source;
char    *dest;
int     *spaces;
{

    int n;
    int space_count;

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

