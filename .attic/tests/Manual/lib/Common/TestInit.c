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
static char rcsid[] = "$XConsortium: TestInit.c /main/10 1995/07/14 11:59:29 drk $"
#endif
#endif
#include "testlib.h"
#include <sys/timeb.h>

#define  MAX_CLASS_LEN  256

XtAppContext     app_context;	/* Global Application Context */
Display          *display;	/* Global Display variable */
Widget           Shell1;	/* Global TopLevel Application Shell */
Window		 rootWindow;	/* Global Toplevel Root Window */
Screen		 * screen;	/* Global TopLevel screen */

/* extern varaibles from CommonExtraResources */

extern Arg       CommonExtraArgs[MAX_ARGS];
extern Cardinal  CommonExtraN;
extern char		*CompleteTestName;

/* extern variable from CommonGetOptions */
extern Boolean   startup_time;
extern void  CommonGetTime();

char CommonAppClass[MAX_CLASS_LEN] = "XMcommon";

/* forward declarations */

static void             ManualSignalQuit();
static void 		ManualRegressionSignal();


/*************************************************************************
*                                                                        *
*       Name:           CommonTestInit()                                 *
*								 	 *
*	Purpose:	This routine will Initialize Xt, create a app    *
*			context, create a connnection to the display     *
*			and create an toplevel application shell. Shell1 *
*			and display are global variable and will be used *
*			in other routines.				 *
*									 *
*	Return:		Nothing						 *
*									 *
*************************************************************************/

void CommonTestInit(int argc, char **argv)

{
     int 	n, i;
     Arg 	args[MAX_ARGS];

#ifdef REGRESSION
     signal(SIGHUP,  ManualRegressionSignal);
     signal(SIGINT,  ManualRegressionSignal);
     signal(SIGQUIT, ManualRegressionSignal);
     signal(SIGKILL, ManualRegressionSignal);
     signal(SIGBUS,  ManualRegressionSignal);
     signal(SIGSEGV, ManualRegressionSignal);

     /* the following was suggested to handle cases where
	user exits via Mwm Close, but it loops infinitely.
	Fix some day. */

     /* atexit (ManualRegressionSignal); */
	
	/* open output file; if impossible, use stdout */

	output_file = fopen ("RUN_output", "a");
	if (output_file == NULL) {
		printf ("Error opening output file.\n");
		output_file = stdout;
	}

#else
     signal(SIGHUP,  ManualSignalQuit);
     signal(SIGINT,  ManualSignalQuit);
     signal(SIGQUIT, ManualSignalQuit);
     signal(SIGKILL, ManualSignalQuit);
     signal(SIGBUS,  ManualSignalQuit);
     signal(SIGSEGV, ManualSignalQuit);

     /* atexit (ManualSignalQuit); */

#endif


     n = 0;

     /*
        merge in Extra arguments in the args list for the creation
        of the application shell. Args are set by CommonExtraResources().

        Special Case: If the CommonExtraArgs[i].name is "CommonAppClass"
        then we want to change the name of the ApplicationClass and we
        will extract that value from CommonExtraArgs[i].value

     */

     if (CommonExtraN > 0) {
        for (i = 0 ; i < CommonExtraN; i++) {
                if (strcmp(CommonExtraArgs[i].name, "CommonAppClass") == 0)
                        strcpy(CommonAppClass, 
 			       (char *) CommonExtraArgs[i].value);
                else {
                        args[n].name = CommonExtraArgs[i].name;
                        args[n++].value = CommonExtraArgs[i].value;
                }
        }
     }

     /*
      *  Initialize malloc checking.  Note: a side effect of this
      *  is that the above signals will be redefined to call 
      *  MallocExit.
     */

     MallocInit();

     CommonGetOptions(&argc, argv);

     if (startup_time)
	 CommonGetTime();

     XtToolkitInitialize();
     app_context = XtCreateApplicationContext();

     display = XtOpenDisplay(app_context, NULL, argv[0], CommonAppClass,
     				NULL, 0, &argc, argv);
     if (display == NULL) {
      	  fprintf(stderr, "%s:  Can't open display\n", argv[0]);
       	  exit(1);
     }

     rootWindow = XDefaultRootWindow(display);
	
     XtSetArg(args[n], XmNallowShellResize, True);  	n++;
     Shell1 = XtAppCreateShell(argv[0], CommonAppClass, 
		          applicationShellWidgetClass, display, args, n);

     screen = XtScreen(Shell1);

}

/*************************************************************************
*                                                                        *
*       Name:           CommonTestI18NInit()                             *
*								 	 *
*	Purpose:	This routine will Initialize Xt, create an       *
*                       application context, register the language       *
*                       procedure called to set the locale, create a     *
*			connnection to the display and create a          *
*			toplevel application shell.                      *
*			Shell1 and display are global variable and will  *
*                       be used in other routines.			 *
*									 *
*	Return:		Nothing						 *
*									 *
*************************************************************************/

void CommonTestI18NInit(int argc, char **argv, XtLanguageProc proc, 
			XtPointer client_data) 

{
     int 	n, i;
     Arg 	args[MAX_ARGS];

#ifdef REGRESSION
     signal(SIGHUP,  ManualRegressionSignal);
     signal(SIGINT,  ManualRegressionSignal);
     signal(SIGQUIT, ManualRegressionSignal);
     signal(SIGKILL, ManualRegressionSignal);
     signal(SIGBUS,  ManualRegressionSignal);
     signal(SIGSEGV, ManualRegressionSignal);

     /* the following was suggested to handle cases where
	user exits via Mwm Close, but it loops infinitely.
	Fix some day. */

     /* atexit (ManualRegressionSignal); */
	
	/* open output file; if impossible, use stdout */

	output_file = fopen ("RUN_output", "a");
	if (output_file == NULL) {
		printf ("Error opening output file.\n");
		output_file = stdout;
	}

#else
     signal(SIGHUP,  ManualSignalQuit);
     signal(SIGINT,  ManualSignalQuit);
     signal(SIGQUIT, ManualSignalQuit);
     signal(SIGKILL, ManualSignalQuit);
     signal(SIGBUS,  ManualSignalQuit);
     signal(SIGSEGV, ManualSignalQuit);

     /* atexit (ManualSignalQuit); */

#endif


     n = 0;

     /*
        merge in Extra arguments in the args list for the creation
        of the application shell. Args are set by CommonExtraResources().

        Special Case: If the CommonExtraArgs[i].name is "CommonAppClass"
        then we want to change the name of the ApplicationClass and we
        will extract that value from CommonExtraArgs[i].value

     */

     if (CommonExtraN > 0) {
        for (i = 0 ; i < CommonExtraN; i++) {
                if (strcmp(CommonExtraArgs[i].name, "CommonAppClass") == 0)
                        strcpy(CommonAppClass, 
 			       (char *) CommonExtraArgs[i].value);
                else {
                        args[n].name = CommonExtraArgs[i].name;
                        args[n++].value = CommonExtraArgs[i].value;
                }
        }
     }

     /*
      *  Initialize malloc checking.  Note: a side effect of this
      *  is that the above signals will be redefined to call 
      *  MallocExit.
     */

     MallocInit();

     XtToolkitInitialize();
     app_context = XtCreateApplicationContext();
     
     /*
      * Register the language procedure called to set the locale.
      */

     XtSetLanguageProc(NULL, proc, client_data);

     display = XtOpenDisplay(app_context, NULL, argv[0], CommonAppClass,
     				NULL, 0, &argc, argv);
     if (display == NULL) {
      	  fprintf(stderr, "%s:  Can't open display\n", argv[0]);
       	  exit(1);
     }

     CommonGetOptions(&argc, argv);

     rootWindow = XDefaultRootWindow(display);
	
     XtSetArg(args[n], XmNallowShellResize, True);  	n++;
     Shell1 = XtAppCreateShell(argv[0], CommonAppClass, 
		          applicationShellWidgetClass, display, args, n);

     screen = XtScreen(Shell1);

}
	
static void  ManualSignalQuit(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
     char sigstring[256];

     switch (sig) {
	case SIGHUP:	strcpy(sigstring, "hangup");
			break;
	case SIGINT:	strcpy(sigstring, "interrupt");
			break;
	case SIGQUIT:	strcpy(sigstring, "quit");
			break;
	case SIGKILL:	strcpy(sigstring, "kill");
			break;
	case SIGSEGV:	strcpy(sigstring, "segmentation violation");
			break;
	case SIGBUS:	strcpy(sigstring, "bus error");
			break;
	default:	strcpy(sigstring, "undetermined");
			break;
     }
     printf("%s signal received: Begin exiting ... please standby ... \n",
	    sigstring);

     fflush(stdout);

     exit(0);
}

static void  ManualRegressionSignal(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
     char sigstring[256];

     switch (sig) {
	case SIGHUP:	strcpy(sigstring, "hangup");
			break;
	case SIGINT:	strcpy(sigstring, "interrupt");
			break;
	case SIGQUIT:	strcpy(sigstring, "quit");
			break;
	case SIGKILL:	strcpy(sigstring, "kill");
			break;
	case SIGSEGV:	strcpy(sigstring, "segmentation violation");
			break;
	case SIGBUS:	strcpy(sigstring, "bus error");
			break;
	default:	strcpy(sigstring, "undetermined");
			break;
     }

     printf("WARNING: %s signal received from test %s\n            Begin exiting ... please standby ... \n", sigstring, TestName);

     /* flush stdout */

     fflush(stdout);

     fprintf(output_file, "WARNING: %s signal received from test %s\n            Begin exiting ... please standby ... \n", sigstring, CompleteTestName);

     /* flush output_file */

     fflush(output_file);

     /* even on an interrupt, write out summary */

     if (InstructionBox != NULL) 
     	XtCallCallbacks (XmMessageBoxGetChild(InstructionBox,
                                              XmDIALOG_CANCEL_BUTTON),
                         XmNactivateCallback, NULL);
     else
	exit(0);

}


