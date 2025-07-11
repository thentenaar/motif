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
static char rcsid[] = "$XConsortium: CommandLine.c /main/11 1995/07/14 10:55:39 drk $"
#endif
#endif
#include "testlib.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#if defined(__STDC__)
#include <stdlib.h>
#include <unistd.h>
#endif

#define DEFAULT_FONT    "fixed"
#define TEST_FONT       "fixed"
#define PAUSE_LEN       60
#define USER_DATA_LEN   64

Boolean		 Automation = False;		/* Automated Regression?? */
Boolean		 AutoRecord = False;		/* Record Automated Screens */
Boolean		 AutoBatch = True;		/* Batch mode for playback */
Boolean		 AutoManual = False;		/* Manual mode for playback */
Boolean		 AutoNoCheck = False;		/* Don't check visuals */
Boolean		 AutoTrace = False;		/* Run in Trace mode */
Boolean		 AutoAltVisualFile = False;	/* Alternate visual file name */
Boolean		 alternate_script = False;	/* Use testname for scripts */
Boolean		 alternate_output = False;	/* Use testname for output */
Boolean		 AutoFullOutput = False;	/* full output or not */
Boolean		 AutoDelay = False;		/* Delay after each command */
Boolean		 AutoWindowDumps = False;	/* produce xwd window dumps */
Boolean		 AutoFailPixel = False;		/* use a different fail pixel */
char		 AutoFailColor[25] = { "red" }; /* failure pixel color */
int		 AutoDelayCycles;		/* number of delay cycles */
char             script_file_name[MAX_FILE];  	/* Alternate script file */
char             script_file_name_orig[MAX_FILE]; /* Alternate script file */
char             output_file_name[MAX_FILE];  	/* Alternate prt file */
char             full_output_file_name[MAX_FILE]; /* Alternate prt file (PATH)*/
char		 AutoAltVisualFileName[MAX_FILE]; /* Alternate visual file */
FILE		 *input, *output; 		/* redefine stdin and stdout */

Boolean          instructions = True;		/* Use instructions?? */
char             *UserData;		        /* Passable character data */
char             *instruct_file_name;           /* Alternate instruction file */
char             *instruct_file_name_orig;      /* Alternate instruction file */
int              pause_len = PAUSE_LEN;		/* Width of MessageBox */
char             test_font[MAX_FONT_LEN];	/* Font for MessageBox */
char             default_font[MAX_FONT_LEN];	/* Default Font for failures */



/*************************************************************************
*									 *
*	Name:		CommonUsage()				     	 *
*									 *
*	Purpose:	This function will print to stdout the current   *
*			flags that are valid for the test suite 	 *
*									 *
*	Returns:	Nothing						 *
*									 *
*	Current Flags:							 *
*			-l  : Disables Message Box explanations of test  *
*			-I  : Read in alternate instruction file. The    *
*			      Current file read is <test_name>.Dat 	 *
*			-h  : Prints the usage message		         *
*			-w  : Controls the width of the Message Box	 *
*			-u  : User Data. Input that can be given to be   *
*			      used by testers programs.			 *
*			-f  : Default Font. Specify a font you would     *
*			      like to use if your request for a font     *
*			      does not succeed (font is not available).  *
*			-p  : MessageBox font. Specify the font you wish *
*			      the instruction's MessageBox to be.	 *
*									 *
*			The Following flags are for Automatic Regression *
*									 *
*			-a  : generate full error reporting on visuals   *
*			-r  : Record image file (Automated Regression)   *
*			-m  : Interactive comparison of visuals    	 *
*			-c  : Comparisons ignored			 *
*			-C  : Alternate failure color			 *
*			-D  : Delay after each command n times		 *
*			-W  : Produce Window Dumps file for failures	 *
*			-S  : Alternative name for script file		 *
*			-O  : Alternative name for output file		 *
*			-V  : Alternative name for visual file		 *
*			-T  : Trace Option - print commands executed	 *
*									 *
*************************************************************************/

void CommonUsage(char *test_name)

{
    fprintf(stdout, 
	    "Usage: %s:\n", test_name);
    fprintf(stdout,
	    "          [-I <instruction_file_name>] [-l <no_instructions>]\n");
    fprintf(stdout,
	    "          [-w <width_of_instructions>] [-u <string_user_data>]\n");
    fprintf(stdout,
	    "          [-f <default_font>] [-p <pause_instructions_font>]\n");
    fprintf(stdout,
	    "          [-help] [X Options]\n");
    fprintf(stdout,
	    "          Automation Flags:\n");
    fprintf(stdout,
	    "          [-a <full_error_reporting_on_visuals>]\n");
    fprintf(stdout,
	    "          [-r <record_automated_images>]\n");
    fprintf(stdout,
	    "          [-m <interactive_comparison_of_visuals>]\n");
    fprintf(stdout,
	    "          [-c <comparisons_ignored>]\n");
    fprintf(stdout,
	    "          [-C <alternate_failure_color>]\n");
    fprintf(stdout,
	    "          [-W <produce_xwd_window_dumps_for_failures>]\n");
    fprintf(stdout,
	    "          [-D [number_of_delay_cycles] <delay_after_command>]\n");
    fprintf(stdout,
	    "          [-T <trace_mode>]\n");
    fprintf(stdout,
	    "          [-S <alternative_script_filename>]\n");
    fprintf(stdout,
	    "          [-O <alternative_output_filename>]\n");
    fprintf(stdout,
	    "          [-V <alternative_visual_filename>]\n");

}


/*************************************************************************
*									 *
*	Name:		CommonGetOptions()				 *
*									 *
* 	Purpose:	This function will extract from the command all  *
*			options used by the test suite and will set all  *
*			appropriate flags for those argumennts. All      *
*			arguments that are not valid for the test suite  *
*			are passed on so that X can properly act on 	 *
*			them. This routine will also set the instruction *
*			file name to be used, User Data, help flag, and  *
*			width of the Message Box used for instructions.	 *
*									 *
*	Current Flags:							 *
*			-l  : Disables Message Box explanations of test  *
*			-I  : Read in alternate instruction file. The    *
*			      Current file read is <test_name>.Dat 	 *
*			-h  : Prints the usage message		         *
*			-w  : Controls the width of the Message Box	 *
*			-u  : User Data. Input that can be given to be   *
*			      used by testers programs.			 *
*			-f  : Default Font. Specify a font you would     *
*			      like to use if your request for a font     *
*			      does not succeed (font is not available).  *
*			-p  : MessageBox font. Specify the font you wish *
*			      the instruction's MessageBox to be.	 *
*									 *
*			The Following flags are for Automatic Regression *
*									 *
*			-a  : full error reporting on visuals            *
*			-r  : Record image file (Automated Regression)   *
*			-m  : Interactive comparison of visuals    	 *
*			-c  : Comparisons ignored			 *
*			-C  : Alternate failure color			 *
*			-D  : Delay after each command n times		 *
*			-W  : Produce Window Dumps file for failures	 *
*			-S  : Alternative name for script file		 *
*			-O  : Alternative name for output file		 *
*			-V  : Alternative name for visual file		 *
*			-T  : Trace Option - print commands executed	 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/
/* globals */

extern char *optarg;             /* supplied by getopt */
extern int optind, opterr;

void  CommonGetOptions (int *argc, char **argv)

{ 
     int c;
     char 	*char_val;
     char	test_name[MAX_FILE];
     Boolean    name_given = False;
     Boolean 	test_font_used = False;
     Boolean 	default_font_used = False;
     int	fd_stdout, fd_stderr, ret;
     char       *visual_path = NULL;
     char 	*getenv();

     static int errflag = 0;

     /* initializations */

     instructions = True;
     pause_len = 0;
     UserData = NULL;
     instruct_file_name = NULL;

     opterr = 0;            /* disable automatic warnings */

     strcpy (test_name, argv[0]);

     if (*argc > 1) {
       while ((c = getopt
		       (*argc, argv, "alhrmcWC:D:TV:S:O:w:u:f:p:I:")) != EOF) {
       switch (c) {
          case 'S':
			if (strlen (optarg) >= (MAX_FILE - 1)) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
                        alternate_script = True;
		        strcpy (script_file_name, optarg);
         		break;

	  case 'O':
			if (strlen (optarg) >= (MAX_FILE - 1)) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			alternate_output = True;
			strcpy (output_file_name, optarg);
			break;
	  case 'V':	
			if (strlen (optarg) >= (MAX_FILE - 1)) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			AutoAltVisualFile = True;
			strcpy (AutoAltVisualFileName, optarg);
			break;
	  case 'C':	strcpy (AutoFailColor, optarg);
			break;
	  case 'D':	
			AutoDelay = True;
			AutoDelayCycles = atoi(optarg);
			break;
	  case 'T':	
			AutoTrace = True;
			break;
	  case 'W':	
			AutoWindowDumps = True;
			break;
	  case 'r':
			AutoRecord = True;
			AutoBatch = False;
			break;
	  case 'm':
			AutoManual = True;
			AutoBatch = False;
			break;
	  case 'c':
			AutoNoCheck = True;
			AutoBatch = False;
			break;
	  case 'a':	
			AutoFullOutput = True;
			break;
	  case 'p':
			if (strlen (optarg) >= MAX_FONT_LEN) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			test_font_used = True;
			strcpy (test_font, optarg);
			break;

	  case 'I':
			if (strlen (optarg) >= (MAX_FILE - 1)) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			name_given = True;
			instruct_file_name = (char *) malloc((sizeof (char)) *
							     MAX_FILE);
			strcpy (instruct_file_name, optarg);
			break;

	  case 'f':	
			if (strlen (optarg) >= MAX_FONT_LEN) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			default_font_used = True;
			strcpy (default_font, optarg);
			break;

	  case 'u':	
			if (strlen (optarg) >= USER_DATA_LEN) {
			  printf ("Error: file name %s too long\n",
				  optarg);
			  errflag++;
			  break;
			}
			UserData = (char *) malloc((sizeof(char))
						    * USER_DATA_LEN);
			strcpy (UserData, optarg);
			break;
	  case 'l':
     			instructions = False;
               		break;
	  case 'h':
    			CommonUsage(test_name);
     			exit(0);
			break;

	  case 'w':
			pause_len = atoi (optarg);
			if (!pause_len) {
			  printf 
			    ("Illegal or zero value provided for width.\n");
			  printf ("Using default %d.\n, PAUSE_LEN");
			  pause_len = PAUSE_LEN;
			}
			break;

          } /*switch */
       if (optind >= *argc) break;
     } /* while getopt */
   } /* if *argc >= 1 */
     
    if (errflag) {
      CommonUsage(test_name);
      exit (1);
    }
     		
    if (!name_given) {
	instruct_file_name = (char *) malloc((sizeof (char)) *
			      MAX_FILE);
        strcpy (instruct_file_name, test_name);
        strcat (instruct_file_name, ".Dat");

	instruct_file_name_orig = (char *) malloc((sizeof (char)) *
			      MAX_FILE);
        strcpy (instruct_file_name_orig, test_name);
        strcat (instruct_file_name_orig, ".dat");
    }

    if (!default_font_used) 
	strcpy(default_font, DEFAULT_FONT);

    if (!test_font_used) 
	strcpy(test_font, TEST_FONT);

    if (pause_len == 0)
	pause_len = PAUSE_LEN;

#ifdef DEBUG
    printf ("Using pause width %d\n", pause_len);
#endif

    if (!alternate_output) {
	strcpy(output_file_name, test_name);
	strcat(output_file_name, ".prt");
    }

    /* If AUTOVAPTH environment variable is set, then use that
       path to write .prt files */

    if (AutoRecord) {
    	visual_path = getenv("AUTOVPATH");
    	if (visual_path != NULL) {
		strcpy(full_output_file_name, visual_path);
		if (visual_path[strlen(visual_path) - 1] != '/')
			strcat(full_output_file_name, "/");
		strcat(full_output_file_name, output_file_name);
		strcpy(output_file_name, full_output_file_name);
    	}
    }

    /* Open .prt file for STDOUT and STDERR */

    output = freopen(output_file_name, "w", stdout);
    if (output == NULL) {
	fprintf(stderr, "Redefine of STDOUT to %s failed - Exiting..\n",
	        output_file_name);
	exit(1);
    }

    if (!alternate_script) {
	strcpy(script_file_name, test_name);
	strcat(script_file_name, ".Scr");

	strcat(script_file_name_orig, test_name);
	strcat(script_file_name_orig, ".scr");
    }

    /* Open .Scr file. If .scr exists and .Scr does not, open .scr,
       use it for input, and inform the user */

    input = freopen(script_file_name, "r", stdin);
    if (input == NULL) {
	input = freopen(script_file_name_orig, "r", stdin);
	if (input == NULL) {
		fprintf(stderr, 
			"Redefine of STDIN to %s failed\n", script_file_name);
		fprintf(stderr, 
			"   - check validity of %s - Exiting...\n",
							    script_file_name);
		exit(1);
	}
	else
		fprintf(stdout,
			"Using %s, please create a %s file\n", 
			script_file_name_orig, script_file_name);
    }

    fflush(stdout);
    fflush(stderr);

    /* redirect both STDOUT and STDERR to same file, use dup2() to
       perform this */

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    ret = dup2(fileno(stdout), fileno(stderr));

    if (ret == -1) {
	fprintf(stderr, "dup2 of STDOUT and STDERR failed - Exiting..\n");
	exit(1);
    }

}
