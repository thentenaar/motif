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
static char rcsid[] = "$XConsortium: PIMwmInfo.c /main/8 1995/07/13 20:22:11 drk $"
#endif
#endif

#include	<testlib.h>
#include	<PITests.h>

#include	"PIMwmInfo.h"


int	ErrorHandle(dpy, err_event)
Display	*dpy;
XErrorEvent	*err_event;
{

	char    msg[80];

	XGetErrorText(dpy, err_event->error_code, msg, 80);
	printf("X Error code: %s\n", msg);

	/*
	 * Window was not found, so result is False.
	 */
	PIInfo[PIInfo_cnt].actual_return = False;

}


main(argc, argv)
unsigned int	argc;
char	*argv[];
{

	Arg     args[10];
	int     n;
	int		i;

	n = 0;

    CommonTestInit(argc, argv);

    n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);

	XSetErrorHandler(ErrorHandle);

	top_wdw = XtWindow(Shell1);

    CommonPause();
	/*
 	 * Initialize the PIInfo data values
  	 */
	for (i = 0; i < NUM_TESTS; i++)
		PIInfo[i].actual_return = False;

/*
	/*
	 * Get the MwmInfo flags
	 */
	test_atom = XmInternAtom(display, "_MOTIF_WM_INFO", False);

	XGetWindowProperty(display, rootWindow, test_atom, 0, 100, False, 
					   AnyPropertyType, &new_type, &new_format, 
					   &new_nitems, &new_bytes_after,
					   (unsigned char **)(&mwm_get_info));

	/*
 	 * Check validity of info flags
 	 */
	PIInfo[PIInfo_cnt].actual_return =
		(
		( mwm_get_info->flags == MWM_INFO_STARTUP_STANDARD ) ||
		( mwm_get_info->flags == MWM_INFO_STARTUP_CUSTOM ));
	PIInfo_cnt++;

	/*
 	 * Check validity of wm_window while mwm running
 	 */
	/* Proper result here is TRUE */
	if (mwm_get_info->wm_window) {
		/*
		 * If XRaiseWindow() succeeds, then the property's 
		 * function IS working - the window SHOULD be valid
		 * while the window manager is running.
		 *
		 * So, we preload the result to be True, but if the
		 * XRaiseWindow() fails, the error handler will reset
		 * the result to False (an error condition).
		 */
		PIInfo[PIInfo_cnt].actual_return = True;
		XRaiseWindow(display, mwm_get_info->wm_window);
	}
	else {
		/*
		 * The window was NULL, so wm must not be running or
		 * is broken, and the window is not valid.  The result
		 * is False.
		 */
		PIInfo[PIInfo_cnt].actual_return = False;
	}
	XSync(display, False);
	PIInfo_cnt++;
    
    CommonPause();
/*
 * Put up a message box with the results of the test.
 */
	display_summary(NUM_TESTS, TEST_DESCRIP_LENGTH, PIInfo);

	CommonPause();

/*
 *  Loop and process events.
 */

	XtAppMainLoop(app_context);

}

