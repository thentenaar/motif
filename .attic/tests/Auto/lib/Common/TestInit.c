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
static char rcsid[] = "$XConsortium: TestInit.c /main/12 1996/06/25 10:08:28 jakku $"
#endif
#endif

#include <AutoMessages.h>
#include <signal.h>
#include <X11/IntrinsicP.h>
#include "testlib.h"
#include "mvslib.h"

#define  MAX_CLASS_LEN 	256

XtAppContext     app_context;	/* Global Application Context */
Display          *display;	/* Global Display variable */
Widget           Shell1;	/* Global TopLevel Application Shell */
Widget           UTMShell;      /* Global TopLevel Shell for UTM */
Widget           UTMDrawingArea;  /* Global TopLevel DrawingArea for UTM */
Window		 rootWindow;	/* Global Toplevel Root Window */
Screen		 *screen;	/* Global TopLevel screen */
Atom _MOTIF_WM_AUTOMATION;
Atom WM_S0;
MvsWidgetInfoRecord *Shell_info;	/* Top level Shell for Automation */



extern void msg_trace();
extern void msg_error();
extern void msg_warning();
extern void UTMDestinationProc();

extern FILE *output;
extern FILE *input;
extern FILE *mvsImageFd;

/* extern varaibles from CommonExtraResources */

extern Arg       CommonExtraArgs[MAX_ARGS];
extern Cardinal  CommonExtraN;

char CommonAppClass[MAX_CLASS_LEN] = "XMcommon";

static void             AutoExitSignal();

#ifdef DEBUG_AUTOMATION

static int ProtoError(Display *dpy, XErrorEvent *ev)
{
    
    fprintf(stderr, "Proto Error\n");
    exit(0);
    
}

#endif




static void
AutoWmInit()
{
     Arg args[10];
     int n;

     /* Atoms for use by automation */

     _MOTIF_WM_AUTOMATION = 
       XmInternAtom (display, "_MOTIF_WM_AUTOMATION", False);
      WM_S0 = 
       XmInternAtom (display, "WM_S0", False);

     /* This is a DrawingArea widget created so that we can use UTM for
        window manager commands. Part of the work needed to add a target
        to mwm that automation can use. DrawingArea supports the 
        destinationCallback. The callback will actually be added in
        AutoWmCmd.c  */

     
     n=0;
     XtSetArg (args[n], XmNwidth, 10); n++;
     XtSetArg (args[n], XmNheight, 10); n++;
     XtSetArg (args[n], XmNmappedWhenManaged, False); n++;
     UTMShell = XtCreatePopupShell ("UTMShell", 
				    topLevelShellWidgetClass, Shell1,
				    args, n);
     XtRealizeWidget (UTMShell);
     UTMDrawingArea = XmCreateDrawingArea (UTMShell, "DrawingArea", args, n);
     XtAddCallback (UTMDrawingArea, XmNdestinationCallback, UTMDestinationProc,
		    (XtPointer)NULL);
}


/*************************************************************************
*                                                                        *
*       Name:           _CommonTestInit()                                *
*								 	 *
*	Purpose:	This routine does the real work of		 *
*			 CommonTestInit and CommonTest18NInit.		 *
*									 *
*	Return:		Nothing						 *
*									 *
*************************************************************************/

static void
_CommonTestInit(int argc, char **argv, XtLanguageProc proc, XtPointer client_data)
{
     int 	n, i, display_w, display_h;
     Arg 	args[MAX_ARGS];

     Automation = True;

     /* trap segv and other signals so that input/output/visual
	files can be closed properly */

     signal(SIGHUP,  AutoExitSignal);
     signal(SIGINT,  AutoExitSignal);
     signal(SIGQUIT, AutoExitSignal);
     signal(SIGKILL, AutoExitSignal);
     signal(SIGSEGV, AutoExitSignal);
     signal(SIGFPE,  AutoExitSignal);
     signal(SIGILL,  AutoExitSignal);
     signal(SIGTERM, AutoExitSignal);
     signal(SIGABRT, AutoExitSignal);
     signal(SIGBUS,  AutoExitSignal);
     signal(SIGSYS,  AutoExitSignal);

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
			
     MallocInit();

#ifdef DEBUG_AUTOMATION
     XSetErrorHandler(ProtoError);
#endif

     XtToolkitInitialize();
     app_context = XtCreateApplicationContext();

     /*
      * Register the language procedure called to set the locale.
      */
     if (proc != (XtLanguageProc) NULL)
       XtSetLanguageProc(NULL, proc, client_data);

     mvsDisplay = XtOpenDisplay(app_context, NULL, argv[0], CommonAppClass,
     			        NULL, 0, &argc, argv);

     if (mvsDisplay == NULL) {
     	fprintf(stderr, "%s:  Can't open display\n", argv[0]);
      	exit(1);
     }

     CommonGetOptions(&argc, argv);

     InitTestAutomation(argc, argv);

     mvsDetailLevel = INFO | FAILURE | ERROR | WARNING;

     display = mvsDisplay;

     rootWindow = XDefaultRootWindow(display);

     /** THIS IS AN UGLY KLUDGE THAT IS NEEDED BECAUSE OF A BUG IN TEXT. TEXT
         DECIDES THAT IF THE DEFAULT MULTICLICK TIME IS 200 (WHICH IS THE
	 DEFAULT FOR THE RESOURCE) THAT IT SHOULD USE 500 INSTEAD. WE NEED
	 TO PREVENT TEXT FROM MAKING THIS DECISION BY MAKING SURE THE MULTICLICK
	 TIME IS NOT THE DEFAULT VALUE. **/
     XtSetMultiClickTime(display, XtGetMultiClickTime(display) + 1);

     xisBeginSession(mvsDisplay, app_context, MAX_OBJECT,
	     	     mvsGenerateObjects, mvsGetObjectRegionName,
               	     mvsGetClassName, mvsWindowToWidget,
               	     msg_trace, msg_warning, msg_error);
     xisSavePointer();

     /* warp pointer to bottom right of display just in case
      * the a previous test failed and left the pointer in
      * a bad place on the display
      */
     display_w = XDisplayWidth(mvsDisplay, 0);
     display_h = XDisplayHeight(mvsDisplay, 0);
     XWarpPointer(mvsDisplay, None, None, 0, 0, 0, 0, display_w, display_h);

     visBeginVisual(mvsDisplay, msg_trace, msg_warning, msg_error);
     mvsRootWidgetInfo = mvsInitRootWidgetInfo();

     /* Create the Toplevel Application Shell */

     XtSetArg(args[n], XmNgeometry, "=125x125+0+0"); 			n++;
     XtSetArg(args[n], XmNallowShellResize, True);  			n++;
     Shell_info = mvsCreateWidget(iXmApplicationShellWidgetClass,
			          mvsRootWidgetInfo, argv[0], 
				  DontUseCommonArgs, args, n,
				  applicationShellWidgetClass);

     Shell1 = Shell_info->widget;

/*     XtRealizeWidget(Shell1); */

     mvsInitFonts();
     xisInitKeyCodes(Shell1);

     screen = XtScreen(Shell1);

     /* Init byteOrder */
     AutoInitByteOrderChar();

     /* do setup for window manager automation */
     AutoWmInit();


	 /**TEMP FIX**/

     /* Once we are done initializing everything, we want to tell xisSynchronize
	that it is safe to start using a widget instead of using a window, like
	it has been doing up until this point. For details about why we want 
	it to be using a widget, see the file Synth/events/Synchronize.c */
     xisResetSyncWindow();
}

/*************************************************************************
*                                                                        *
*       Name:           CommonTestInit()                                 *
*								 	 *
*	Purpose:	This routine will Initialize Xt, create a app    *
*			context, create a connnection to the display     *
*			and create an toplevel application shell. Shell1 *
*			and display are global variable and will be used *
*			in other routines.				 *
*			When compiling for Automated Regression, this    *
*			will initialize the bare bones hierarchy for     *
*			for Automation, and will initialize all visuals, *
*			fonts, and callbacks.				 *
*									 *
*	Return:		Nothing						 *
*									 *
*************************************************************************/

void CommonTestInit(int argc, char **argv)
{
    _CommonTestInit(argc, argv, NULL, NULL /* no i18n initialization */ );
}
 /*************************************************************************
*                                                                        *
*       Name:           CommonTestI18NInit()                             *
*								 	 *
*	Purpose:	This routine will Initialize Xt, create an app   *
*			context, register the language procedure called  *
*                       to set the locale, create a connnection to the   *
*                       display and create an toplevel application shell.*
*			Shell1 and display are global variable and will  *
*                       be used in other routines.			 *
*			When compiling for Automated Regression, this    *
*			will initialize the bare bones hierarchy for     *
*			for Automation, and will initialize all visuals, *
*			fonts, and callbacks.				 *
*									 *
*	Return:		Nothing						 *
*									 *
*************************************************************************/

void CommonTestI18NInit(int argc, char **argv, XtLanguageProc proc, 
			XtPointer client_data)

{
    _CommonTestInit(argc, argv, proc, client_data  /* initialize i18n */);
}

	
static void  AutoExitSignal(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
     char sigstring[256];

     switch (sig) {
	case SIGHUP:	AutoMessage(_AutoMessages[SIGMSG1]);
			break;
	case SIGINT:	AutoMessage(_AutoMessages[SIGMSG2]);
			break;
	case SIGQUIT:	AutoMessage(_AutoMessages[SIGMSG3]);
			break;
	case SIGKILL:	AutoMessage(_AutoMessages[SIGMSG4]);
			break;
	case SIGSEGV:	AutoMessage(_AutoMessages[SIGMSG5]);
			break;
	case SIGFPE:	AutoMessage(_AutoMessages[SIGMSG6]);
			break;
	case SIGILL:	AutoMessage(_AutoMessages[SIGMSG7]);
			break;
	case SIGTERM:	AutoMessage(_AutoMessages[SIGMSG8]);
			break;
	case SIGABRT:	AutoMessage(_AutoMessages[SIGMSG9]);
			break;
	case SIGBUS:	AutoMessage(_AutoMessages[SIGMSG10]);
			break;
	case SIGSYS:	AutoMessage(_AutoMessages[SIGMSG11]);
			break;
	default:	AutoMessage(_AutoMessages[SIGMSG12]);
			break;
     }


     /* flush stdout */

     fflush(stdout);

     /* Close all Input/Output files */

     fclose(input);
     fclose(output);
     fclose(mvsImageFd);

     /* Exit Test */

     exit(0);

}

























