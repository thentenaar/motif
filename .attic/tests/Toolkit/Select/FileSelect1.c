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
static char rcsid[] = "$TOG: FileSelect1.c /main/10 1999/05/28 15:45:39 jff $"
#endif
#endif

#include <testlib.h>
#include <unistd.h>

#define TARGET_DIR "/tmp/FS/fourteencharac/fourteencharac/fourteencharac/"

static void	GoneCB();
static void 	OkApplyHelpCB();
Widget    		FileSelectionBox1;

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  	n;
    Arg         	args[MAX_ARGS];
    XmString    	tcs, tcs2;
    Widget		CancelW, filelist;

/* Ensure target directory for FSB is set up properly */

    if ( access(TARGET_DIR,F_OK | R_OK) ) {
	printf("Target directory %s is not set up\n",TARGET_DIR);
	exit(1);
    }

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  			n++;
    XtSetArg(args[n], XmNheight, 300);  			n++; 
    XtSetValues(Shell1, args, n);
    
    XtRealizeWidget(Shell1);

    tcs = XmStringCreateSimple(TARGET_DIR);

    tcs2 = XmStringCreateSimple("b*");

    n = 0;
    XtSetArg(args[n], XmNdirMask, tcs);  			n++;
    XtSetArg(args[n], XmNdirSpec, NULL); 			n++;
    XtSetArg(args[n], XmNpattern, tcs2);			n++;
    FileSelectionBox1 = XmCreateFileSelectionBox(Shell1,
				    "FileSelectionBox1", args, n);
    XtManageChild(FileSelectionBox1);

    XmStringFree(tcs);
    XmStringFree(tcs2);

    /* change some resources */

    tcs = XmStringLtoRCreate("Look For:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNwidth, 350); 				n++; 
    XtSetArg(args[n], XmNheight, 300); 				n++; 
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE);	n++;
    XtSetArg(args[n], XmNfilterLabelString, tcs);	n++;
    XtSetValues(FileSelectionBox1, args, n);

    XmStringFree(tcs);

    /* add a callback to the Cancel button to destroy the world */

    CancelW = XmFileSelectionBoxGetChild(FileSelectionBox1,
		      			 XmDIALOG_CANCEL_BUTTON);
    XtAddCallback(CancelW,XmNactivateCallback, GoneCB, NULL);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    XtAddCallback(FileSelectionBox1, XmNokCallback, OkApplyHelpCB, NULL);
    XtAddCallback(FileSelectionBox1, XmNhelpCallback, OkApplyHelpCB, NULL);
    XtAddCallback(FileSelectionBox1, XmNapplyCallback, OkApplyHelpCB, NULL);

    CommonPause();

/* Test for CR 4269 */
/* This test originally used MallocEnable, MallocLeakInfo and
 * MallocDisable to determine memory leaks.
 */

    n = 0;
    XtSetArg(args[n], XmNinitialResourcesPersistent, False); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetValues(Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNinitialResourcesPersistent, False); n++;
    XtSetValues(FileSelectionBox1, args, n);

    filelist = XmFileSelectionBoxGetChild(FileSelectionBox1, XmDIALOG_LIST);

    XmListSelectPos(filelist, 1, True);
    XmListDeselectPos(filelist, 1);

    CommonPause();

/* End Test for  CR 4269 */


/* Begin Test for Pir2161 */

    tcs = XmStringCreateSimple("/tmp/FS/");

    n = 0;
    XtSetArg(args[n], XmNdirMask, tcs); 		n++;
    XtSetValues(FileSelectionBox1, args, n);

    XmStringFree(tcs);

    CommonPause();

/* End Test for Pir2161 */

/* Test for Pir 4110 */

    n = 0;
    XtSetArg(args[n], XmNkeyboardFocusPolicy, XmPOINTER); 	n++;
    XtSetValues(Shell1, args, n);

/* End Test for Pir 4110 */


    CommonPause();

    XtAppMainLoop(app_context);
}


static void GoneCB(w, client_data, call_data )
    Widget  w;
    XtPointer client_data;
    XtPointer call_data;
{
    /* callback for when user clicks the cancel button */
    
    Widget dad;

    dad = XtParent(w);		/* get parent of cancel button */
    XtDestroyWidget(dad);	/* and parent */
}


static void OkApplyHelpCB(w, client_data, call_data)
    Widget  w;
    XtPointer client_data;
    XtPointer call_data;
{
    /* callback for Ok, Apply, or Help button */

    int reason;

    reason = ((XmFileSelectionBoxCallbackStruct *)call_data)->reason;
    switch (reason) {
	case XmCR_OK:	
			printf("Ok Callback triggered: reason XmCR_OK\n");
			break;
	case XmCR_HELP:	
			printf("Help Callback triggered: reason XmCR_HELP\n");
			break;
	case XmCR_APPLY: 
			printf("Apply Callback triggered: reason XmCR_APPLY\n");
			break;
	default:	printf("Invalid reason\n");
			break;
    }
}
