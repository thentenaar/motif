/* $XConsortium: TextScroll.c /main/4 1995/07/15 21:02:31 drk $ */
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
#define	MAX_LINE_LEN	512
#define	MAX_NAME_LEN	255

#define	DEFAULT_IN_FILE1	"./RowCol1_1.c"
#define	DEFAULT_IN_FILE2	"./WordWrap.dat"

#define	TEXT1_X		10
#define	TEXT1_Y		10


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
Boolean show_perf_bug = False;
Boolean word_wrap_on = False;
Boolean test_word_wrap = False;


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

	struct timeval act_time1, act_time2;
	struct timezone zone_time1, zone_time2;


	infile_name[0] = '\0';

	/*  initialize toolkit  */
    CommonTestInit(argc, argv);

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
	XtSetArg(args[n], XtNwidth, 800);  n++;
	XtSetArg(args[n], XtNheight, 500);  n++;
    XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);
    

	/* create main form */
	n = 0;
	Form1 = XmCreateForm (Shell1, "Form1", args, n);
	XtManageChild (Form1);

	/* Read the text file into a buffer */
	fill_text_buf();

	/* create multi line text widget */
	n = 0;
	XtSetArg(args[n], XmNx, TEXT1_X); n++;
	newfont = XLoadQueryFont(XtDisplay(Shell1),"9x15");
	if (newfont != NULL) {
		newfontlist = XmFontListCreate(newfont, XmSTRING_DEFAULT_CHARSET);
		XtSetArg(args[n], XmNfontList, newfontlist);  n++;
	}
	XtSetArg(args[n], XmNcolumns, NUM_COLUMNS);  n++;
	XtSetArg(args[n], XmNrows, NUM_ROWS);  n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNscrollHorizontal, False); n++;
	XtSetArg(args[n], XmNeditable, False); n++;
    if (word_wrap_on == True) {
		XtSetArg(args[n], XmNwordWrap, True); n++;
	}
    if (show_perf_bug == True) {
    	XtSetArg(args[n], XmNresizeWidth, True); n++;
    	XtSetArg(args[n], XmNresizeHeight, True); n++;
	}
    Text1 = XmCreateScrolledText(Form1, "Text1", args, n);
    if (show_perf_bug == True) {

        gettimeofday(&act_time1, &zone_time1);
        XmTextSetString(Text1, text_string);
        gettimeofday(&act_time2, &zone_time2);
        fprintf(outfp, "Time lapsed to set TextString before Manage : \n");
        PrintLapseTime(outfp, &act_time1, &act_time2);

    }

	XtManageChild(Text1);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
	quit = XmCreatePushButtonGadget(Form1, "Quit", args, n);
	XtManageChild(quit);
	XtAddCallback(quit, XmNactivateCallback, quitCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 150); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
        if (show_perf_bug == True) {
                XtSetArg(args[n], XmNsensitive, False); n++;
        }
	fillText = XmCreatePushButtonGadget(Form1, "Fill Text", args, n);
	XtManageChild(fillText);
	XtAddCallback(fillText, XmNactivateCallback, fillTextCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 300); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
        if (show_perf_bug == False) {
                XtSetArg(args[n], XmNsensitive, False); n++;
        }
	scrollToEnd = XmCreatePushButtonGadget(Form1, "Scroll To End", args, n);
	XtManageChild(scrollToEnd);
	XtAddCallback(scrollToEnd, XmNactivateCallback, scrollToEndCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 450); n++;
	XtSetArg(args[n], XmNy, TEXT1_Y + 400); n++;
        if (show_perf_bug == False) {
                XtSetArg(args[n], XmNsensitive, False); n++;
        }
	scrollToBegin = XmCreatePushButtonGadget(Form1, "Scroll To Begin", args, n);
	XtManageChild(scrollToBegin);
	XtAddCallback(scrollToBegin, XmNactivateCallback, scrollToBeginCB, NULL);

	if (word_wrap_on == True)
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


static void	fill_text_buf()
{

	struct stat	stat_buf;
	int	filesize, bytes_read;
	char	line[MAX_LINE_LEN + 1];

	if (infile_name[0] == '\0')
        {
            if ( test_word_wrap )
		strcpy(infile_name, DEFAULT_IN_FILE2);
            else
		strcpy(infile_name, DEFAULT_IN_FILE1);
        }

	/* Read the file into a text buffer */
	if ((in_fp = fopen(infile_name, "r")) == NULL) {
		fprintf(stderr, "Can not open %s\n", infile_name);
		exit(0);
	}

	in_fd = fileno(in_fp);
	fstat(in_fd, &stat_buf);
	filesize = stat_buf.st_size;

	text_string = (char *)XtMalloc(filesize + 1);
	bytes_read = fread(text_string, sizeof(char ), filesize, in_fp);
	if (bytes_read != filesize) {
		printf("filesize = %d bytes_read = %d\n", filesize, bytes_read);
		fclose(in_fp);
		exit(0);
	}
	*(text_string + filesize) = '\0';

	rewind(in_fp);
	while (fgets(line, MAX_LINE_LEN, in_fp) != NULL)
		total_lines++;

	fclose(in_fp);

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

        if (strcmp(next_word, "-f") == 0) {

        	get_next_word(user_data, next_word, &num_spaces);
        	user_data += (strlen(next_word) + num_spaces);
			strcpy(infile_name, next_word);
            continue;

        }
		if (strcmp(next_word, "-use_word_wrap") == 0) {
			word_wrap_on = True;
			continue;
		}
		if (strcmp(next_word, "-test_word_wrap_on") == 0) {
			test_word_wrap = True;
			word_wrap_on = True;
			continue;
		}
		if (strcmp(next_word, "-test_word_wrap_off") == 0) {
			test_word_wrap = True;
			word_wrap_on = False;
			continue;
		}
		if (strcmp(next_word, "-show_perf_bug") == 0) {
			show_perf_bug = True;
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
