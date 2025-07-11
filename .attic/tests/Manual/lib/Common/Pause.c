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
static char rcsid[] = "$XConsortium: Pause.c /main/10 1995/07/14 11:59:23 drk $"
#endif
#endif
#include "testlib.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


#define MAX_LINE        4096

#ifdef REGRESSION
#define CONTINUE_STRING " Press either Pass or Fail to continue testing."
#define END_STRING      \
		" If no instructions press Exit, else Pass/Fail and Exit"
#else
#define CONTINUE_STRING " Press the Continue Button for more testing."
#define END_STRING      " Test Finished -- Exit Please."
#endif /* REGRESSION */

extern Boolean startup_time;
extern char		*CompleteTestName;


Widget        	InstructionBox; 	/* Global Widget for Instruction 
					   MessageBox */
FILE     	*output_file;  		/* Global summary output file */
static Widget	ContinuePopup = NULL;   /* Shell to contain MessageBox */
static Boolean 	continue_created = False;
static char     line[MAX_LINE];		/* Pre-Formatted Input */
static char     line_format[MAX_LINE];  /* Formmated Input */
static Boolean	event_wait;		/* wait for events in queue */


extern void   CommonCheckEvent(XEvent *event, Widget shell);
void   FlushEvents();
static char   *GetInstructionsFromFile();
static Widget CreateContinuePopup();
static void   ContinueCB(Widget w, caddr_t client_data,
			 caddr_t call_data);
static void   ContinueCBWrapper(Widget w, caddr_t client_data,
			 caddr_t call_data);
static void   QuitCB(Widget w, caddr_t client_data,
		     caddr_t call_data);


#ifdef REGRESSION
/* some global symbols */
static int num_panels = 0;
static int num_panels_passed = 0;
static int num_panels_failed = 0;
#endif /* REGRESSION */

static int last_panel_flag = 0;

/*************************************************************************
*									 *
*	Name:		CommonPause()					 *
*									 *
* 	Purpose:	To Allow the writer and the user of the test to  *
*			stop at designed points in the test and give     *
*		        directions on what the users should minipulate   *
*			and what the expected action are going to be.    *
*			The Pause configuration will be a Message Box    *
*			located at (500, 0) which will contain 		 *
*			instructions on what the tester wants the user   *
*			to do.						 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

void  CommonPause()
{
     XmString 	message_XmString;
     Arg	args[1];
     int 	n = 0;
     int	i, size;
     static int panel_num = 1;
     char	buf[32];

	 purify_newleaks();
     purify_clear_newleaks();

     if (instructions) {

         if (!continue_created)
     	    ContinuePopup = CreateContinuePopup();

         GetInstructionsFromFile();
         message_XmString = XmStringCreateLtoR(line_format,
     					   XmFONTLIST_DEFAULT_TAG);

         n = 0;
         XtSetArg(args[n], XmNmessageString, message_XmString); 	n++;
         XtSetValues(ContinuePopup, args, n);

         XtPopup(XtParent(ContinuePopup), XtGrabNone);

	 /* number the panels for easier reporting of bugs */
	 sprintf (buf, "Instruction Panel %d", panel_num++);

	 n = 0;
	 XtSetArg (args[n], XmNtitle, buf); n++;
	 XtSetValues (XtParent(ContinuePopup), args, n);

         XmStringFree(message_XmString);

	 size = strlen(line_format);
	 for (i = 0 ; i < size ; i++)
		line_format[i] = '\0';

	 size = strlen(line);
	 for (i = 0 ; i < size ; i++)
		 line[i] = '\0';

         event_wait = True;

         while (event_wait)
     	    FlushEvents();
     }


}


/*************************************************************************
*									 *
*	Name:		FlushEvents()					 *
*									 *
*	Purpose:	Flush all events from the Pending Queue		 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

void  FlushEvents()
{
     XEvent event;

     /* change for Pir 2478 - blocking select() */

if (startup_time) 
     do {
        XtAppNextEvent(app_context, &event);
        CommonCheckEvent(&event, Shell1);
        XtDispatchEvent(&event);
    } while (XtAppPending(app_context));

  else 
     do {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    } while (XtAppPending(app_context));

}


/*************************************************************************
*									 *
* 	Name: 		CreateContinuePopup()				 *
*									 *
*	Purpose:	To intially create a MessageBox Dialog for use   *
*                       in Motif test suite. This routine will create    *
*			a MessageDialog and set callbacks to exit the    *
*			test or continue with the next phase in the test *
*			It also loads test_font for the font of the      *
*			Dialog (if avaiable).				 *
*									 *
*	Returns:	This function will return the MessageDialog to   *
*			be used in the rest of the test.		 *
*								  	 *
*************************************************************************/

static Widget CreateContinuePopup()
{
    Arg           args[10];
    int           n;
    Widget        exit_button,
    	      	  help_button,
                  continue_button;
    XmString      message_string,
                  continue_string,
                  exit_string,
		  fail_string;
    XmFontList    fontlist;
    Widget	  PopupShell;
#ifdef REGRESSION
    Widget	  FailButton;
#endif /* REGRESSION */

    fontlist = CommonGetFontList(test_font);

#ifdef REGRESSION
    continue_string = XmStringCreateLtoR("Pass",
    				   XmFONTLIST_DEFAULT_TAG);
#else
    continue_string = XmStringCreateLtoR("Continue", 
    				   XmFONTLIST_DEFAULT_TAG);
#endif /* REGRESSION */

    exit_string = XmStringCreateLtoR("Exit", 
    				   XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XtNgeometry, "+500+0"); 			n++;
    XtSetArg(args[n], XtNallowShellResize, True);		n++;
    PopupShell = XtCreatePopupShell("Test Instructions", 
				    topLevelShellWidgetClass, Shell1, 
				    args, n);

    n = 0;
    XtSetArg(args[n], XmNokLabelString, continue_string); 	n++;
    XtSetArg(args[n], XmNcancelLabelString, exit_string); 	n++;
    XtSetArg(args[n], XmNbuttonFontList, fontlist); 		n++;
    XtSetArg(args[n], XmNlabelFontList, fontlist);		n++;
    XtSetArg(args[n], XmNshadowThickness, 10);			n++;
    XtSetArg(args[n], XmNmarginWidth, 15);			n++;
    XtSetArg(args[n], XmNmarginHeight, 15);			n++;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION);	n++;
    InstructionBox = XmCreateMessageBox(PopupShell, "InstructionBox", args, n);

#ifdef REGRESSION
    fail_string = XmStringGenerate("Fail", XmFONTLIST_DEFAULT_TAG,
                    XmCHARSET_TEXT, NULL );

    n = 0;
    XtSetArg (args[n], XmNlabelString, fail_string);		n++;
    XtSetArg (args[n], XmNuserData, 0); 			n++;
    FailButton = XmCreatePushButton (InstructionBox, "FailButton", args, n);
    XtManageChild (FailButton);

    XmStringFree(fail_string);
#endif /* REGRESSION */

    XtManageChild(InstructionBox);

    XmStringFree(continue_string);
    XmStringFree(exit_string);

    continue_button = XmMessageBoxGetChild(InstructionBox, XmDIALOG_OK_BUTTON);
    exit_button = XmMessageBoxGetChild(InstructionBox, XmDIALOG_CANCEL_BUTTON);
    help_button = XmMessageBoxGetChild(InstructionBox, XmDIALOG_HELP_BUTTON);

#ifdef REGRESSION
    n = 0;
    XtSetArg (args[n], XmNuserData, 1); n++;
    XtSetValues (continue_button, args, n);

    XtAddCallback(continue_button, XmNactivateCallback, 
 		  (XtCallbackProc)ContinueCBWrapper, InstructionBox);
    XtAddCallback(FailButton, XmNactivateCallback, 
 		  (XtCallbackProc)ContinueCBWrapper, InstructionBox);
#else
    XtAddCallback(continue_button, XmNactivateCallback, 
 		  (XtCallbackProc)ContinueCB, InstructionBox);
#endif /* REGRESSION */

    XtAddCallback(exit_button, XmNactivateCallback, 
 		  (XtCallbackProc)QuitCB, InstructionBox);

    n = 0;
    XtSetArg(args[n], XmNsensitive, False);			n++;
    XtSetValues(help_button, args, n);

    continue_created = True;

    return(InstructionBox);
}


/*************************************************************************
*									 *
*	Name:		ContinueCB()					 *
*									 *
*	Purpose:	A Callback routine which will Unmanage the 	 *
*			Message Dialog so that it can read the next test *
*			case associated with this test.			 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

static void ContinueCB(Widget w, caddr_t client_data, 
		       caddr_t call_data)

{
     event_wait = False;
     XtPopdown(XtParent((Widget)client_data));
}

/*************************************************************************
*									 *
*	Name:		ContinueCBWrapper()				 *
*									 *
*	Purpose:	Replace normal ContinueCB(), keep records of     *
*			pass/fail per panel for regression testing.      *
*			Also contains functionality of ContinueCB().     *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

static void ContinueCBWrapper (Widget w, caddr_t client_data, 
		       caddr_t call_data)

{
     /* keeping track stuff */
     int id;
     Arg args[1];

     /* we're here, so a new panel */
#ifdef REGRESSION
     num_panels++;

     /* the userData of w tells which button it was */

     XtSetArg (args[0], XmNuserData, &id); 
     XtGetValues (w, args, 1);

     if (id) 	/* then we passed */
	num_panels_passed++;
     else num_panels_failed++;
#endif /* REGRESSION */
     
     /* regular ContinueCB stuff */
     if (!last_panel_flag) {
     	event_wait = False;
     	XtPopdown(XtParent((Widget)client_data));
     }
}

/*************************************************************************
*									 *
*	Name:		QuitCB()					 *
*									 *
*	Purpose:	A Callback routine which will exit the test. It  *
*			does this with simple exit(0).			 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

static void QuitCB(Widget w, caddr_t client_data,
		   caddr_t call_data)
{
  float a,b;

  XmAnyCallbackStruct *cbs;
  XEvent *xevent;
  
  if (call_data != NULL) {
	cbs = (XmAnyCallbackStruct *) call_data;
  	if (cbs) xevent = (XEvent *)cbs->event;
  	if (xevent->xbutton.state & ControlMask) {
    		CommonDumpHierarchy (Shell1, stdout);
    		return;
        }
  }
  printf("Exiting Test...Please Standby...\n");

     /*
      * Terminate malloc processing.  Will do a malloc_dump
      *  if enabled.
      */

#ifdef REGRESSION
  if (num_panels) {
	

        /* dump summary */
        fprintf (output_file, "------------------------------------\n");
        fprintf (output_file, "   Test results: %s             \n", CompleteTestName);
        fprintf (output_file, "   Passed        Failed      Total  \n");
        fprintf (output_file, "     %d            %d          %d   \n", 
						       num_panels_passed,
      						       num_panels_failed,
	                                               num_panels);
        a = (float)num_panels_passed/num_panels;
        b = (float)num_panels_failed/num_panels;
        fprintf (output_file,"%9.2f%%%11.2f%%            \n", a*100, b*100);
        fprintf (output_file, "------------------------------------\n\n");


        fflush (stdout);
	fclose (output_file);
     }
#endif /* REGRESSION */

     MallocExit(0);   /*  0 indicates normal exit */

	 purify_newleaks();
     purify_clear_newleaks();

     exit(0);
}


/*************************************************************************
*									 *
*	Name:		GetInstructionsFromFile()			 *
*									 *
*	Purpose:	A routine that will open a file that contains    *
*			instructions for the given test. The name of     *
*			this file will be determined by the name of the  *
*			test with a .dat extension. Once this file is    *
*			opened, instructions will be extracted. Data     *
*			will be extracted be reading the input file      *
*			character by character and checking for special  *
*			characters. Once these characters have been      *
*			processed a formatted string of chars will be    *
*			created to be converted to an XmString and to be *
*			displayed in the Message Dialog.		 *
*									 *
*	Special Chars:	\   : escape next character			 *
*			\t  : remove all tabs subsitute with 1 space     *
*			\n  : strip all newlines let code reformat       *
*			' ' : remove multiple spaces let code reformat   *
*			#)  : beginning of test case. # subsituted to    *
*			      test case number.				 *
*			C)  : Add standard continue statment to data     *
*		    	      stream. End current screen		 *
*			@)  : Indented format. Format without a test     *
*			      case number.				 *
*			!)  : No format. No formatting except width 	 *
*			      formatting will be performed.		 *
*									 *	
*    	Returns:	One "Frame" of instructions.			 *
*									 *
*	Note:		One "Frame" of instructions must not exceed more *
*			than 9 cases.					 *
*									 *
*************************************************************************/

static char *GetInstructionsFromFile()

{
     static FILE	*instruct_file;
     static Boolean	instruction_opened = False;
     static Boolean	end_data = False;
     static Boolean	no_format;
     int		array_index = 0;
     int		array_index_format = 0;
     int 		test_number = 1;
     int		count, i, len;
     Boolean		end_line;
     Boolean		char_read = False;
     Boolean		Bextended;
     char		c, c2;
     char		temp_s[10];
     int                line_length;

     /* Open file to read instructions */

     if (!instruction_opened) {
         if (!(instruct_file = fopen(instruct_file_name, "r"))) {
            if (!(instruct_file = fopen(instruct_file_name_orig, "r"))) {
                printf("Failure Opening: %s\n", instruct_file_name);
                exit(0);
            }
            else {
                fprintf(stderr,
                        "Using %s, please create a %s file\n",
                        instruct_file_name_orig, instruct_file_name);
                instruction_opened = True;
            }
          }
          else
            instruction_opened = True;
     }


     /* Start first pass through the input data. Identify special
	characters and get the input string into one flat stream of
	data */

     if (((c  =  (char) fgetc(instruct_file)) == EOF) || end_data) {
	fprintf(stderr, "Pause information exhausted\n");
	exit(-1); 
     }
  
     end_line = False;
     Bextended = False;
     no_format = False;
     array_index = 0;

     while (!end_line && c != EOF) {
     	switch (c) {
		/* End of Instructions */
		case 'E' :	c2 =  (char) fgetc(instruct_file);
                                if (c2 == ')') {
					if (Bextended)
						line[array_index++] = '\n';
					end_line = True;
					end_data = True;
					line[array_index++] = '\n';
					line[array_index++] = '\n';
					strcat(line, END_STRING);
					last_panel_flag = 1;
				}
				else {
                                         line[array_index++] = c;
                                         line[array_index++] = c2;
                                }
                                break;
		/* End of one Panel of Instructons */
		case 'C' :	c2 =  (char) fgetc(instruct_file);
				if (c2 == ')') {
                                        end_line = True;
					line[array_index++] = '\n';
					line[array_index++] = '\n';
					strcat(line, CONTINUE_STRING);
				}
				else {
					 line[array_index++] = c;
					 line[array_index++] = c2;
				}
				break;
		/* Beginning of a line of instructions within a panel */
		case '#' :	c2 =  (char) fgetc(instruct_file);
				if (c2 == ')') {
					if (Bextended)
						line[array_index++] = '\n';
					if (!no_format)
						line[array_index++] = '\n';
					line[array_index++] = '#';
					line[array_index++] = ')';
				}
				else {
					line[array_index++] = c;
					line[array_index++] = c2;
				}
				Bextended = False;
				no_format = False;
				break;
		/* Beginning of Bextended data case - indent lines */
		case '@':	c2 =  (char) fgetc(instruct_file);
				if (c2 == ')') {
					Bextended = True;
					line[array_index++] = '\n';
					if (!no_format)
						line[array_index++] = '\n';
					for (i = 0; i < 2 ; i++)
						line[array_index++] = ' ';
				}
				else {
					line[array_index++] = c;
					line[array_index++] = c2;
				}
				no_format = False;
				break;
		/* Start a no formatting block */
		case '!':	c2 =  (char) fgetc(instruct_file);
				if (c2 == ')')  {
					line[array_index++] = '\n';
					no_format = True;
				}
				else {
					line[array_index++] = c;
					line[array_index++] = c2;
				}
				break;
		/* Strip newlines, let second pass format */
		case '\n':	if (no_format) 
					line[array_index++] = '\n';
				else {
					c2 = ' ';
					char_read = True;
				}
				break;
		/* Strip tabs, let second pass format */
	   	case '\t':	line[array_index++] = ' ';
				break;
		/* Escape next character */
		case '\\':	c2 =  (char) fgetc(instruct_file);
				if (c2 != 'n')
					line[array_index++] = c2;
				if (c2 == 'n' && no_format)
					line[array_index++] = '\n';
				break;
		/* Remove all spaces, let second pass format */
		case ' ':	c2 =  (char) fgetc(instruct_file);
				if (!no_format) {
					while (c2 == ' ')
						c2 = (char)fgetc(instruct_file);
					if (c2 != '\n')
						line[array_index++] = ' ';
				}
				else {
					line[array_index++] = ' ';
					while (c2 == ' ') {
						c2 = (char)fgetc(instruct_file);
						line[array_index++] = ' ';
					}
				}
				char_read = True;
				break;
		/* Normal characters */
		default:	if (isdigit(c)) {
					sprintf(temp_s, "%c", c);
					line[array_index++] = temp_s[0];
				}
				else 
					line[array_index++] = c;
				break;
	}
	if (char_read) {
		c = c2;
		char_read = False;
	}
	else
        	c  =  (char) fgetc(instruct_file);
     }

     /* Begin second pass. Format the first data string and put
	formatted information into a second string */

     array_index = 0;
     array_index_format = 0;
     count = 1;
     line_length = strlen(line);
     while (array_index < line_length) {
	if (line[array_index++] == '#') {
		if (line[array_index] == ')') {
			sprintf(temp_s, "%d", test_number++);
			line_format[array_index_format++] = temp_s[0];
			line_format[array_index_format++] = ')';
			array_index++;
			count = 1;
		}
		else {
			count += 2;
			line_format[array_index_format++] =
						line[array_index - 1];
			line_format[array_index_format++] = 
						line[array_index++];
		}
	}
	else  {
		if (line[array_index - 1] == '\n') {
			line_format[array_index_format++] = 
						line[array_index - 1];
			count = 1;
		}
		else {
			line_format[array_index_format++] = 
						line[array_index - 1];
			count++;
		}
	}
	if ((count % pause_len) == 0) {
	        while (array_index < line_length &&
		        line[array_index] != ' ' &&
			line[array_index] != '\n') {
			line_format[array_index_format++] = 
						line[array_index++];
		}
		if ((line[array_index + 1] != '\n' ) &&
		    (line[array_index + 1] != '#')) {
			line_format[array_index_format++] = '\n';
			line_format[array_index_format++] = ' ';
			line_format[array_index_format++] = ' ';
			line_format[array_index_format++] = ' ';
			array_index++;
		}
       	}		
     }

     line_format[array_index_format] = '\n';

     return(line_format);
}
