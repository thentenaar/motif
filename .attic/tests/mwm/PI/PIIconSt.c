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
static char rcsid[] = "$XConsortium: PIIconSt.c /main/10 1995/07/13 20:20:41 drk $"
#endif
#endif

#include	<testlib.h>
#include	<X11/Xatom.h>
#include	<PITests.h>

#include	"PIIconSt.h" 
#include	"btn1.bmp"


													/*Begin PIR3720*/
XTextProperty	iconProp;
char	*IconName1 = "Test\nPIR3720";
char	*IconName2 = "Test\rPIR3720";
													/*End PIR3720*/


void
main(unsigned int argc, char **argv)
{

	Arg     args[10];
	int     n;
	int		i;


    CommonTestInit(argc, argv);
	
    /* 
     * allocate icon_size structure
     */

    icon_size = XAllocIconSize();

    n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);

    CommonPause();
	/*
 	 * Initialize the PIInfo data values
  	 */
	for (i = 0; i < NUM_TESTS; i++)
		PIInfo[i].actual_return = False;

/*
    
/*
 * Create top level shell widget as parent for test_button widget
 */
	n = 0;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;

	test_top = XtAppCreateShell("test_top",				/* application name */
		 						"Test_top",				/* application class */
		 						topLevelShellWidgetClass,	/* widget class */
		 						display,				/* display */
		 						args, n);				/* arguments */

/*
 * Create button1 widget
 */
	display = XtDisplay(test_top);
	screen = DefaultScreenOfDisplay(display);
	test1_btn1_icon = XCreatePixmapFromBitmapData(display, rootWindow,
								btn1_bits, btn1_width,
								btn1_height,
								CommonGetColor("red"),
								CommonGetColor("yellow"),
								DefaultDepth(display, DefaultScreen(display)));
	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNlabelPixmap, test1_btn1_icon); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	test1_btn1 = XmCreatePushButton(test_top, "test1_btn1", args, n);
	XtManageChild(test1_btn1);
	XtRealizeWidget(test_top);

	test_wdw = XtWindow(test_top);
	top_wdw = XtWindow(Shell1);

	/*
	 * Move the test_top shell window and the Shell1 window
	 */
	XMoveWindow(display, test_wdw, 100, 100);

	/*
 	 * Get icon size information for root window      WM_ICON_SIZE
  	 */
	test_atom = XmInternAtom(display, "WM_ICON_SIZE", False);

	XGetIconSizes (display, rootWindow, &icon_size, &count_return);

    PIInfo[PIInfo_cnt].actual_return = (
		(count_return) &&
		(icon_size->min_width != 0) &&
		(icon_size->min_height != 0) &&
		(icon_size->max_width >= icon_size->min_width) &&
		(icon_size->max_height >= icon_size->min_height) &&
		(icon_size->width_inc != 0) &&
		(icon_size->height_inc != 0));
	PIInfo_cnt++;

    CommonPause();

	/*
 	 * Get state info for test window (NormalState)      WM_STATE
  	 */
	XSync(display, False);      /* make sure all windows are mapped */
	test_atom = XmInternAtom(display, "WM_STATE", False);
	XGetWindowProperty(display, test_wdw, test_atom, 0, 100, False, 
					   AnyPropertyType, &new_type, &new_format, 
					   &new_nitems, &new_bytes_after,
					   (unsigned char **)(&state_info));
	PIInfo[PIInfo_cnt].actual_return = (
		(new_format) &&
		(state_info->state == NormalState) &&
		(state_info->icon != (Window)NULL));
	PIInfo_cnt++;
	/*
	 * Iconify the window: Unmap the window to change state
	 */
	PIUnmapWindow(test_wdw);
    /*
	 * Put the window in iconic state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = IconicState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMapWindow(display, test_wdw);

	CommonPause();

	/*
	 * Unmap the window again to change state
	 */
	PIUnmapWindow(test_wdw);

	/*
	 * Put the window in normal state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMoveWindow(display, test_wdw, 100, 100);
	XMapWindow(display, test_wdw);

	CommonPause();
													/*Begin PIR3720*/
	test_atom = XmInternAtom(display, "XA_WM_ICON_NAME", False);
	iconProp.value = (unsigned char *)IconName1;
	iconProp.encoding = XA_STRING;
	iconProp.format = 8;
	iconProp.nitems = strlen(IconName1);
	XSetWMIconName(display, test_wdw, &iconProp);

	/*
	 * Unmap the window again to change state
	 */
	PIUnmapWindow(test_wdw);

	/*
	 * Put the window in iconic state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = IconicState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMapWindow(display, test_wdw);

	CommonPause();

	/*
	 * Unmap the window again to change state
	 */
	PIUnmapWindow(test_wdw);

	/*
	 * Put the window in normal state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMoveWindow(display, test_wdw, 100, 100);
	XMapWindow(display, test_wdw);

	/* Try with CR in IconName instead of NEWLINE */
	test_atom = XmInternAtom(display, "XA_WM_ICON_NAME", False);
	iconProp.value = (unsigned char *)IconName2;
	iconProp.encoding = XA_STRING;
	iconProp.format = 8;
	iconProp.nitems = strlen(IconName2);
	XSetWMIconName(display, test_wdw, &iconProp);

	/*
	 * Unmap the window again to change state
	 */
	PIUnmapWindow(test_wdw);

	/*
	 * Put the window in iconic state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = IconicState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMapWindow(display, test_wdw);
													/*End PIR3720*/

	CommonPause();

	CommonPause();									/* Show PIR4040 */

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

