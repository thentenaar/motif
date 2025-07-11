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
static char rcsid[] = "$TOG: CallUil.c /main/5 1999/11/23 16:50:58 jff $"
#endif
#endif
#include "CallUil.h"
				
static MrmCode		class;

/* Register all callbacks */

static MrmRegisterArg reglist[] = {
  {"exit_test", (caddr_t)exit_test},
  {"Get_Text", (caddr_t)Get_Text},
  {"Change_Font", (caddr_t)Change_Font},
  {"Place_Hunt", (caddr_t)Place_Hunt},
  {"Hunt_String", (caddr_t)Hunt_String},
  {"Place_FsBox", (caddr_t)Place_FsBox},
  {"Get_File", (caddr_t)Get_File}
};

static int reglist_num = (sizeof (reglist) / sizeof (reglist[0]));

/*
 *  Main program
 */
int main(argc, argv)
int argc;
char **argv;

{
    Arg arglist[1];

/*
 * Test for CR 5534 - set up dummy signal handlers before calling Uil
 */
    void * temp;

    signal( SIGBUS, dummy_handler );
    signal( SIGSYS, dummy_handler );
    signal( SIGFPE, dummy_handler );

    /*
     *  Initialize the Mrm
     */

    MrmInitialize ();

    /*
     *  Initialize the toolkit.  This call returns the id of the "toplevel"
     *  widget.  The applications "main" widget must be the only child
     *  of this widget.
     */

    toplevel = XtInitialize(
	argv[1],	 		/* application name */
	"genericclass",                 /* application class */
	NULL, 0,                        /* options */
	&argc, argv);                   /* command line parameters */

    XtSetArg (arglist[0], XtNallowShellResize, TRUE) ;
    XtSetValues (toplevel, arglist, 1) ;

/* Open three uil files for compilation. First will have an error, second a  */
/* warning, and the third will compile successfully. Before each compilation */
/* information will be given on each compilation.			     */

    Pause();
    Create_Uid_File(rootname_err,"Continue for UIL compilation warning test");
    Pause();
    Create_Uid_File(rootname_warn,"Continue for UIL successful compilation test");
    Pause();
    Create_Uid_File(rootname_corr,"Continue for Xlist utility");
    Pause();

    signal( SIGBUS, SIG_DFL );
    signal( SIGSYS, SIG_DFL );
    signal( SIGFPE, SIG_DFL );

    printf("-- The uid file being opened is %s -- \n\n", uid_file);
    vec[0] = uid_file;

/* Try to Open file specified on command line and get all input */

    Get_Text_Input(argv, argc);

    /*
     *  Define the Mrm hierarchy (only 1 file)
     */

    if (MrmOpenHierarchy (1,			    /* number of files	    */
			vec, 			    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_MrmHierarchy)	    /* ptr to returned id   */
			!= MrmSUCCESS) 
			{
				printf ("Can't open hierarchy\n");
     				exit(1);
			}

    /*
     *		Register all Mrm functions
     */	


    if (MrmRegisterNames (reglist, reglist_num)
			!= MrmSUCCESS)
			{
			    printf("Can't register names\n");
			    exit(1);
			}

    /*
     *  Call Mrm to fetch main widget
     */

    if (MrmFetchWidget (s_MrmHierarchy,
			"widgetmain",
			toplevel,
			&widgetmain,
			&class)
			!= MrmSUCCESS)
			{
			    printf("Can't fetch interface\n");
			    exit(1);
			}

    /*
     *  Make the toplevel widget "manage" the main window (or whatever the
     *  the uil defines as the topmost widget).  This will
     *  cause it to be "realized" when the toplevel widget is "realized"
     */

    XtManageChild(widgetmain);
    
    /*
     *  Realize the toplevel widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(toplevel);


    /* XmAddTabGroup(widgetmain); */

    Get_Text();

    /*
     *  Loop and process events
     */

    XtMainLoop();
}

/*****************************************************************************/
/*	Procedure Create_Uid_File - Use the callable UIL compiler to create  */
/*	a uid file. Check all error conditionals and print a report out to   */
/*	STDOUT. The compilation process will also produce a listing file     */
/*	with machine instruction output.				     */
/*****************************************************************************/

static void Create_Uid_File(root_uil_name, continue_mess)
char root_uil_name[MAX_LEN];
char continue_mess[MAX_LEN];
{
Uil_command_type        command_desc;
Uil_compile_desc_type   compile_desc;
Uil_status_type         compile_stat;
int			user_mess_data = 1;
int			user_stat_data = 1;
char			*include_dirs[3];
char			listing_file[MAX_LEN];
char			uil_file[MAX_LEN];
char			tmp[MAX_LEN];
void			(*dummy_pointer)();
void			(*handler_check)();

/* clear the message string */

	strcpy(mess_string,"");

/* get proper extensions for UIL, UID, and listing files */

	strcpy(listing_file, root_uil_name);
	strcat(listing_file, ".lis");
	strcpy(uid_file, root_uil_name);
	strcat(uid_file, ".uid");
	strcpy(uil_file, root_uil_name);
	strcat(uil_file, ".uil");

/* Set up command description structure */

	command_desc.resource_file = uid_file;
    	command_desc.listing_file = listing_file;
    	command_desc.include_dir_count = 3;
	command_desc.include_dir = (char **)include_dirs;
    	command_desc.source_file = uil_file;
#ifndef MOTIF1_1
	command_desc.use_setlocale_flag = 0;
	command_desc.database_flag = 0;
#endif

/* define inclusion path */

	include_dirs[0] = ".";
	include_dirs[1] = "../../../clients/uil/";
	include_dirs[2] = "/tmp/";

/* give info on what files you want created */

	command_desc.listing_file_flag = 1;    /* create a listing */
     	command_desc.resource_file_flag = 1;   /* create a UID file */
     	command_desc.machine_code_flag = 1;    /* create machine code */
     	command_desc.report_info_msg_flag = 1; /* report info messages */
     	command_desc.report_warn_msg_flag = 1; /* report all warnings */
	command_desc.parse_tree_flag = 0;      /* do not create a parse tree */
    	command_desc.status_update_delay = 300;  /* set proper delay */

	sprintf(tmp,"The following values have been returned by UIL compilation\n\n");
	strcat(mess_string, tmp);

/* Call uil compiler with appropriate paramters */

	compile_stat = Uil(&command_desc, 
		     &compile_desc, 
		     MessageCB, 
		     (char *) &user_mess_data,
		     StatusCB, 
		     (char *)&user_stat_data);
	
  	sprintf(tmp, "\nThe version of the compiler is %d\n", compile_desc.compiler_version);
	strcat(mess_string, tmp);
	sprintf(tmp, "The version number of the structures is %d\n\n", compile_desc.data_version);
	strcat(mess_string, tmp);


/* check status and print out appropraite messages */

	sprintf(tmp,"%d Calls to Status Callback\n%d Calls to Message Callback\n\n",user_stat_data - 1, user_mess_data - 1);
	strcat(mess_string, tmp);

	if (compile_stat == Uil_k_success_status)
		sprintf(tmp, "Callable UIL Operation Successful\n\n");
	else if (compile_stat == Uil_k_info_status)
		sprintf(tmp, "Callable UIL Operation Successul, informational message returned\n\n");
	else if (compile_stat == Uil_k_warning_status)
		sprintf(tmp, "Callable UIL Operation Successul, warning message returned\n\n");
	else if (compile_stat == Uil_k_error_status)
		sprintf(tmp, "Callable UIL Operation failure due to error\n\n");
	else if (compile_stat == Uil_k_severe_status)
		sprintf(tmp, "Callable UIL Operation failure due to severe error\n\n");
	strcat(mess_string, tmp);

/*
 * Test for CR 5534 - make sure that the signal handlers are the dummy_handler
 *                    after calling Uil
 */
        dummy_pointer = dummy_handler;
        handler_check = signal( SIGBUS, dummy_handler );
        if (handler_check != dummy_pointer)
          sprintf(tmp, "SIGBUS handler was NOT restored\n");
        else
          sprintf(tmp, "SIGBUS handler was restored\n");
        strcat(mess_string, tmp);

        handler_check = signal( SIGSYS, dummy_handler );
        if (handler_check != dummy_pointer)
          sprintf(tmp, "SIGSYS handler was NOT restored\n");
        else
          sprintf(tmp, "SIGSYS handler was restored\n");

        strcat(mess_string, tmp);

        handler_check = signal( SIGFPE, dummy_handler );
        if (handler_check != dummy_pointer)
          sprintf(tmp, "SIGFPE handler was NOT restored\n\n");
        else
          sprintf(tmp, "SIGFPE handler was restored\n\n");
        strcat(mess_string, tmp);

	strcat(mess_string, continue_mess);
}

/***************************************************************************/
/*	Procedure MessageCB - This procedure will print to STDOUT          */
/*	information on the compilation process. Specifically if errors or  */
/*	warnings appear.					    	   */
/***************************************************************************/

Uil_status_type MessageCB(message_user_data, msg_number, msg_severity,
                          msg_text, src_text, ptr_text, loc_text,
                          message_summary)
int     *message_user_data;  /* user message number */
int     msg_number;     /* constant to identify the error */
int     msg_severity;   /* severity level */
char    *msg_text;      /* text of message */
char    *src_text;      /* text of source line on which error occured */
char    *ptr_text;      /* source column information */
char    *loc_text;      /* location line */
unsigned int (*message_summary)[Uil_k_max_status+1];
{
char	tmp[1000];
int 	i;

/* remove tabs - compound string does not convert tabs to sensible characters */

	for (i = 0 ; i < strlen(loc_text); i++)
		if (loc_text[i] == '\t')
			loc_text[i] = ' ';
	for (i = 0 ; i < strlen(src_text); i++)
		if (src_text[i] == '\t')
			src_text[i] = ' ';

    	sprintf(tmp, "Message #%d -\n", *message_user_data);
	strcat(mess_string, tmp);
	sprintf(tmp, "      Error constant is: %d\n      Severity Level is: %d\n", msg_number, msg_severity);
	strcat(mess_string, tmp);
	sprintf(tmp, "      Message is: %s\n      Error on %s - %s\n", msg_text, loc_text, src_text);
	strcat(mess_string, tmp);
    	*message_user_data = *message_user_data + 1;
    	return Uil_k_continue;
}

/***************************************************************************/
/*	Procedure StatusCB - print to STDOUT information the status of the */
/*	compilation process. This summary will print the source file being */
/*	worked on, the number of lines, and the percentage completed       */
/***************************************************************************/

Uil_status_type StatusCB(status_user_data, percent_complete, lines_processed,
                         current_file, message_summary)
int		*status_user_data;
unsigned int	percent_complete;
unsigned int	lines_processed;
char		*current_file;
unsigned int	(*message_summary)[Uil_k_max_status+1];
{

	char	tmp[1000];

	sprintf(tmp, "Status #%d -- Lines: %d, File: %s, Percentage: %d%%\n",
            *status_user_data, lines_processed, current_file, percent_complete);
	strcat(mess_string, tmp);
    	*status_user_data = *status_user_data + 1;
    	return Uil_k_continue;

}

/*****************************************************************************/
/*	Procedure exit_test - will exit test when menu item is chosen        */
/*****************************************************************************/

static void exit_test( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
	printf("Test finished -- Exiting....\n");
	exit(0);
}

/*****************************************************************************/
/*	Procedure Get_Text - Will get text widget from heirarchy and set     */
/*	the new text value.						     */
/*****************************************************************************/

static void Get_Text()
{
    static Arg args[1];
    Widget widget = NULL;

    if ((widget = XtNameToWidget(widgetmain,"BB1.ScrolledWin.Text_Area")) == NULL)
	{
		printf("widget returned NULL on TEXT_AREA\n");
		exit(0);
	}
    XtSetArg(args[0], XmNvalue, value_string);
    XtSetValues(widget, args, 1); 
}

/*****************************************************************************/
/*	Procedure Change_Font - Will receive a font name from the callback   */
/*	and set the font for the text area.				     */
/*****************************************************************************/

static void Change_Font( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
Arg args[1];
XmFontList font1;
XFontStruct *fnt;
Widget widget2 = NULL;

    if ((widget2 = XtNameToWidget(widgetmain,"BB1.ScrolledWin.Text_Area")) == NULL)
	{
		printf("widget returned NULL on TEXT_AREA\n");
		exit(0);
	}
    if (strcmp(tag,"fixed") == 0)
	fnt = XLoadQueryFont(XtDisplay(toplevel), "fixed");
    else if (strcmp(tag,"variable") == 0)
	fnt = XLoadQueryFont(XtDisplay(toplevel), "variable");
    else if (strcmp(tag,"8x13bold") == 0)
	fnt = XLoadQueryFont(XtDisplay(toplevel), "8x13bold");
    else if (strcmp(tag,"6x12") == 0)
	fnt = XLoadQueryFont(XtDisplay(toplevel), "6x12");
    else
	printf("Font not available\n");
    font1 = XmFontListCreate(fnt, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNfontList, font1);
    XtSetValues(widget2, args, 1); 
}

/****************************************************************************/
/*	Procedure Place_Hunt - Will place on the screen a Prompt Dialog for */
/*	the selection of a hunt string.                                     */
/****************************************************************************/

static void Place_Hunt( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
Widget widget2 = NULL;

if (MrmFetchWidget (s_MrmHierarchy,
                        "Hunt_info",
                        toplevel,
                        &widget2,
                        &class)
                        != MrmSUCCESS)
                        {
                            printf("Can't fetch interface\n");
                            exit(1);
                        }
XtManageChild(widget2);
}

/****************************************************************************/
/*	Procedure Place_FsBox - will place on the screen a File Selection   */
/*	Dialog for entering a new filename to be displayed.                 */
/****************************************************************************/

static void Place_FsBox( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
Widget widget2 = NULL;

if (MrmFetchWidget (s_MrmHierarchy,
                        "File_Open",
                        toplevel,
                        &widget2,
                        &class)
                        != MrmSUCCESS)
                        {
                            printf("Can't fetch interface\n");
                            exit(1);
                        }
XtManageChild(widget2);
}

/***************************************************************************/
/*	Procedure Hunt_String - Will get the selection string from the     */
/*	Prompt Dialog and send that string to Change_Position which will   */
/*	reset the cursor to the correct position.			   */
/***************************************************************************/

static void Hunt_String( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
Arg args[1];
XmString string;
char *reg_string;

XtSetArg(args[0], XmNtextString, &string);
XtGetValues(widget, args, 1);
reg_string = cs_to_rs(string);
Change_Position(reg_string);
XtDestroyWidget(widget);
}

/****************************************************************************/
/*	Procedure Get_File - will get a file name from the File Selection   */
/*	Box and send that to Get_Text_Input_New which will open the new     */
/*	file and redisplay the text in the text widget.			    */
/****************************************************************************/

static void Get_File( widget, tag, callback_data )
Widget  widget;
char    *tag;
XmAnyCallbackStruct *callback_data;
{
Arg args[1];
XmString string;
char *reg_string;

XtSetArg(args[0], XmNtextString, &string);
XtGetValues(widget, args, 1);
reg_string = cs_to_rs(string);
Get_Text_Input_New(reg_string);
Get_Text();
XtDestroyWidget(widget);
}

/*****************************************************************************/
/*	Procedure Change_Position - will get the ID of the Text widget,      */
/*	then get the position of the hunt string in the text and reset       */
/*	the cursor to that position.	          			     */
/*****************************************************************************/

static void Change_Position( string )
char string[20];
{
Arg args[1];
int position;
Widget widget = NULL;

    if ((widget = XtNameToWidget(widgetmain,"BB1.ScrolledWin.Text_Area")) == NULL)
	{
		printf("widget returned NULL on TEXT_AREA\n");
		exit(0);
	}
    position = Hunt(string);
    XtSetArg(args[0], XmNcursorPosition, position);
    XtSetValues(widget, args, 1); 
}

/*****************************************************************************/
/* 	Procedure Get_Text_Input - will read from the filename given on the  */
/*	command line all it's data and store that data in the array called   */
/*	value_string.							     */
/*****************************************************************************/

Get_Text_Input(argv, argc)
char **argv;
int argc;
{
FILE *fp;
int c;
int x = 0;

if (argc == 2)
 	{
	if ((fp = fopen(argv[1],"r")) == 0)
		printf("Open failed\n");
	else
		{
		while((c = getc(fp)) != EOF)
			value_string[x++] = c;
		value_string[x] = '\0';
		}
	}
else
	printf("No file name specified - Use Open File..\n");
}
	
/*****************************************************************************/
/*	Procedure Get_Text_Input_New - will get info from a file specified   */
/*	in the File Selection Box.					     */
/*****************************************************************************/

Get_Text_Input_New(fname)
char fname[20];
{
FILE *fp;
int c;
int x = 0;
int i;

for (i = 0 ; i < strlen(value_string) ; i++)
	value_string[i] = '\0';
if ((fp = fopen(fname,"r")) == 0)
	{
	printf("Open failed\n");
	exit(0);
	}
else
	{
	while((c = getc(fp)) != EOF)
		value_string[x++] = c;
	value_string[x] = '\0';
	}
}

/*****************************************************************************/
/*	Procedure Hunt - Will search for a string within value_string and    */
/*	return the position of the start of the string in value_string.      */
/*****************************************************************************/

Hunt(string)
char string[20];
{
int i = 0;
int max_size_vs, max_size_ms;
int index = 0;

max_size_vs = strlen(value_string);
max_size_ms = strlen(string);
i = cur_pos;
while (cur_pos < max_size_vs)
	{
	while (string[index] == value_string[cur_pos]) 
		{
		cur_pos++;
		index++;
		}
	if (index == max_size_ms)
		return(cur_pos - max_size_ms + 1);
	else
		index = 0;
	cur_pos++;
	}
	cur_pos = 0;
	return(i);
}

/****************************************************************************/
/*	Procedure cs_to_rs - will convert a compound string to a regular    */
/*	string.								    */
/****************************************************************************/

static char *cs_to_rs(cs)
XmString cs;
{
  XmStringContext context;
  XmStringCharSet charset;
  XmStringDirection direction;
  Boolean separator;
  char *primitive_string;
  XmStringInitContext(&context,cs);
  XmStringGetNextSegment(context,&primitive_string,
	   &charset,&direction,&separator);
  XmStringFreeContext(context);
  return(primitive_string);
}

/***************************************************************************/
/* 	Procedure Pause - Will put up a messageDialog and set the message  */
/*	field to error or warning conditions returned by the Callable UIL  */
/*	compiler. Also used to display instructions to the user.	   */
/***************************************************************************/

static void Pause()
{
Arg args[4];
XmString str1, str2;
Widget mbox;
XmFontList font1;
XFontStruct *fnt;

str1 = XmStringLtoRCreate (mess_string, XmSTRING_DEFAULT_CHARSET);
str2 = XmStringLtoRCreate ("Continue", XmSTRING_DEFAULT_CHARSET);
fnt = XLoadQueryFont(XtDisplay(toplevel), "variable");
font1 = XmFontListCreate(fnt, XmSTRING_DEFAULT_CHARSET);
XtSetArg(args[0], XmNmessageString, str1);
XtSetArg(args[1], XmNokLabelString, str2);
XtSetArg(args[2], XmNlabelFontList, font1);
XtSetArg(args[3], XmNtextFontList, font1);
mbox = (Widget) XmCreateMessageDialog (toplevel, "mbox", args, 4);
XtManageChild(mbox);
XtAddCallback(mbox, XmNokCallback, Continue, NULL);
wait = True;
while (wait)
    FlushEvents();
}

/****************************************************************************/
/*	Procedure FlushEvents - will flush all queued events.               */
/****************************************************************************/

static void  FlushEvents()
{
    XEvent event;

    while (XtPending()) {
        XtNextEvent(&event);
        XtDispatchEvent(&event);
    }
}

/****************************************************************************/
/*	Procedure Continue - Return from message Box sets wait and goes to  */
/*	next part of the test.					            */
/****************************************************************************/

static void Continue(w, client_data, call_data)
Widget   w;
caddr_t  client_data;
caddr_t  call_data;
{
wait = False;
}

/****************************************************************************/
/*      Procedure dummy_handler - Dummy routine to see if signal handlers   */
/*      are restored by Uil.						    */
/****************************************************************************/

void dummy_handler(l_error)
  int l_error;
{
  printf("In dummy_handler()");
}
