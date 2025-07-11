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
static char rcsid[] = "$TOG: PIHints.c /main/10 1997/05/02 16:02:30 dbl $"
#endif
#endif

#include	<testlib.h>
#include	<PITests.h>

#include	"PIHints.h" 
#include	"btn1.bmp"


main(argc, argv)
unsigned int	argc;
char	*argv[];
{

	Arg     args[10];
	int     n;
	int		i;

	XmString    message;

    CommonTestInit(argc, argv);

    wm_get_size = XAllocSizeHints();

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
 * Create top level shell widget as parent for test1_btn1 widget
 */
	n = 0;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;

	test_top = XtAppCreateShell("test_top",	   /* application name */
		 		     "Test_top",  /* application class */
		 		   topLevelShellWidgetClass, /* widget class */
		 		   display,		     /* display */
		 	           args, n);		  /* arguments */      
	
			
		  

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
	test1_btn1 = XmCreatePushButton(test_top, "test1_btn1", args, n);
	XtManageChild(test1_btn1);
	XtRealizeWidget(test_top);

	test_wdw = XtWindow(test_top);
	top_wdw = XtWindow(Shell1);
	button_wdw = XtWindow(test1_btn1);


	/*
	 * Move the test_top shell window and the Shell1 window
	 */

	XMoveWindow(display, test_wdw, 100, 100);

	/*
 	 * Change Normal Size Hints - position, size (WM_NORMAL_HINTS)
  	 */
	test_atom = XmInternAtom(display, "WM_NORMAL_HINTS", False);
	wm_set_size.flags = (PPosition | PSize);
	wm_set_size.x = wm_set_size.y = 250;
	wm_set_size.width = wm_set_size.height = 100;
	XSetWMNormalHints (display, test_wdw, &wm_set_size);


	/*
 	 * Change Normal Hints - mins/maxs width/height
  	 */
	test_atom = XmInternAtom(display, "WM_NORMAL_HINTS", False);
	wm_set_size.flags = (PMinSize | PMaxSize);
	wm_set_size.min_width = wm_set_size.min_height = 200;
	wm_set_size.max_width = wm_set_size.max_height = 380;
	XSetWMNormalHints (display, test_wdw, &wm_set_size);


    /*
     * Unmap and remap the window to change state
     */

	PIUnmapWindow(test_wdw); 
	wm_set_hints.flags = (StateHint);
        wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMapWindow(display, test_wdw);
        XMoveWindow(display, test_wdw, 100, 100);  


	

	XGetWMNormalHints (display, test_wdw, wm_get_size, &supplied_return);
	/*
 	 * Get Normal Size Hints - position, size (WM_NORMAL_HINTS)
  	 */
	PIInfo[PIInfo_cnt].actual_return = ((wm_get_size->x ==  wm_set_size.x)
					   &&(wm_get_size->y == wm_set_size.y) 
			       && (wm_get_size->width == wm_set_size.width) 
			      &&  (wm_get_size->height == wm_set_size.height));

        PIInfo_cnt++;
	
	/*
 	 * Get Normal Hints - mins/maxs width/height
  	 */
	PIInfo[PIInfo_cnt].actual_return = 
		(
		(wm_get_size->min_width == wm_set_size.min_width) &&
                (wm_get_size->min_height == wm_set_size.min_height) &&
		(wm_get_size->max_width == wm_set_size.max_width) &&
		(wm_get_size->max_height == wm_set_size.max_height));
	PIInfo_cnt++;

	CommonPause();


	/*
 	 * Create another simple window to play with the XWMHints stuff
 	 */

	hints_wdw = XCreateSimpleWindow(display, DefaultRootWindow(display),
					100, 300, 150, 30, 5, 0,
					CommonGetColor("white"));
	XMapWindow(display, hints_wdw);
	XSync(display, False);
	XStoreName(display, hints_wdw, hints_winname);
	XSetIconName(display, hints_wdw, hints_winname);

	/*
 	 * Change WM Hints - icon pixmap for window WM_HINTS
 	 */
	hints_icon_pm = XCreatePixmapFromBitmapData(display, 
						    rootWindow,
						    PIIcon_bits, PIIcon_width,
						    PIIcon_height, 1, 0, 1);
	hints_icon_msk_pm = XCreatePixmapFromBitmapData(display, rootWindow,
							PIIcon_msk_bits,
						        PIIcon_msk_width,
						        PIIcon_msk_height,
							1, 1, 1);
	/*
	 * Unmap the window to change state
	 */
	PIUnmapWindow(hints_wdw);
	wm_set_hints.flags = (StateHint | IconPixmapHint | IconMaskHint);
	wm_set_hints.initial_state = IconicState;
	wm_set_hints.icon_pixmap = hints_icon_pm;
	wm_set_hints.icon_mask = hints_icon_msk_pm;
	XSetWMHints (display, hints_wdw, &wm_set_hints);
	
	/* 
	 * Map the window into iconic state
	 */
	XMapWindow(display, hints_wdw);


	wm_get_hints = XGetWMHints (display, hints_wdw);
	PIInfo[PIInfo_cnt].actual_return =
		((wm_get_hints->icon_pixmap == wm_set_hints.icon_pixmap) &&
		(wm_get_hints->icon_mask == wm_set_hints.icon_mask));
	PIInfo_cnt++;

	CommonPause();	

	/*
 	 * Change Normal Hints - width/height increments
 	 */

	wm_set_size.flags=(PResizeInc);
	wm_set_size.width_inc = wm_set_size.height_inc = 20;
	XSetWMNormalHints (display, test_wdw, &wm_set_size);

	/*
 	 * Change Normal Hints - min/max aspect ratios
 	 */

	test_atom = XmInternAtom(display, "WM_NORMAL_HINTS", False);
	wm_set_size.flags=(PAspect);
	wm_set_size.min_aspect.x = 2;
	wm_set_size.min_aspect.y = 1;
	wm_set_size.max_aspect.x = 4;
	wm_set_size.max_aspect.y = 1;
	XSetWMNormalHints (display, test_wdw, &wm_set_size);

	/*
	 * Unmap and remap the window to change state
	 */

	PIUnmapWindow(test_wdw);
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw, &wm_set_hints);
	XMapWindow(display, test_wdw);
	XMoveWindow(display, test_wdw, 100, 100);

	XGetWMNormalHints (display, test_wdw, wm_get_size, &supplied_return);

     /*
      * Get Normal Hints - width/height increments
      */
	PIInfo[PIInfo_cnt].actual_return =
		(
		(wm_get_size->width_inc == wm_set_size.width_inc) &&
		(wm_get_size->height_inc == wm_set_size.height_inc));
	PIInfo_cnt++;

	/*
 	 * Get Normal Hints - min/max aspect ratios
 	 */
	PIInfo[PIInfo_cnt].actual_return =
		(
		(wm_get_size->min_aspect.x == wm_set_size.min_aspect.x) &&
		(wm_get_size->min_aspect.y == wm_set_size.min_aspect.y) &&
		(wm_get_size->max_aspect.x == wm_set_size.max_aspect.x) &&
		(wm_get_size->max_aspect.y == wm_set_size.max_aspect.y));
	PIInfo_cnt++;
	/* 
 	 * Change WM Hints - icon window for window            WM_HINTS
 	 */
	hints_icon_wdw = XCreateSimpleWindow(display, rootWindow, 0, 0, 50, 
					     50,5, 1, CommonGetColor("black"));

	/* 
	 * put window from IconicState to WithdrawnState
	 */

	PIUnmapWindow(hints_wdw);
	wm_set_hints.flags = (StateHint | IconWindowHint | IconPositionHint);
	wm_set_hints.initial_state = IconicState;
	wm_set_hints.icon_window = hints_icon_wdw;
	wm_set_hints.icon_x = wm_set_hints.icon_y = 20;	/* or as close as the grid allows */
	XSetWMHints(display, hints_wdw, &wm_set_hints);

	/*
	 * revive from the WithdrawnState to IconicState
	 */

	XMapWindow(display, hints_wdw);
	wm_get_hints = XGetWMHints(display,hints_wdw);
	PIInfo[PIInfo_cnt].actual_return =
		((wm_get_hints->icon_window == wm_set_hints.icon_window));
	PIInfo_cnt++;

#ifdef	DONT
	CommonPause();

	/* 
 	 * Change WM Hints - input focus for window            WM_HINTS
 	 */
	/* 
	 * put window from IconicState to WithdrawnState
	 */
	PIUnmapWindow(hints_wdw);

	test_atom = XmInternAtom(display, "WM_HINTS", False);
	wm_set_hints.flags = (StateHint | InputHint);
	wm_set_hints.initial_state = NormalState;
	wm_set_hints.input = True;
	XSetWMHints(display, hints_wdw, &wm_set_hints);
	/*
	 * revive from the WithdrawnState to NormalState
	 */
	XMapWindow(display, hints_wdw);
	XMoveWindow(display, hints_wdw, 100, 300);

	wm_get_hints = XGetWMHints (display, hints_wdw);

	PIInfo[PIInfo_cnt].actual_return =
		((wm_get_hints->icon_window == wm_set_hints.icon_window));
	PIInfo_cnt++;

#endif	/* DONT */

	CommonPause();

    n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	XtSetValues(Shell1, args, n);

	win_attrs.background_pixel = CommonGetColor("black");

	black_window = XCreateWindow(display, rootWindow, 200, 10, 100, 100, 1,
							   CopyFromParent, CopyFromParent, CopyFromParent,
							   CWBackPixel, &win_attrs);

	transient_for_window = top_wdw;

	XSetTransientForHint(display, black_window, transient_for_window);

	PIUnmapWindow(top_wdw);
	XMapWindow(display, top_wdw);

	XMapWindow(display, black_window);
	XStoreName(display, black_window, black_winname);
	if (XGetTransientForHint(display, black_window, &new_trans_for_window)
		== 0)
		fprintf(stderr, "XGetTransientForHint call 1 failed\n");

	PIInfo[PIInfo_cnt].actual_return =
		((new_trans_for_window == transient_for_window));
	PIInfo_cnt++;

	CommonPause();

	transient_for_window = test_wdw;

	XSetTransientForHint(display, black_window, transient_for_window);

	PIUnmapWindow(black_window);
	XMapWindow(display, black_window);

	if (XGetTransientForHint(display, black_window, &new_trans_for_window)
		== 0)
		fprintf(stderr, "XGetTransientForHint call 2 failed\n");
	PIInfo[PIInfo_cnt].actual_return =
		((new_trans_for_window == transient_for_window));
	PIInfo_cnt++;

	CommonPause();

	win_attrs.background_pixel = CommonGetColor("white");

	white_window = XCreateWindow(display, rootWindow, 200, 200, 100, 100, 1,
							   CopyFromParent, CopyFromParent, CopyFromParent,
							   CWBackPixel, &win_attrs);

	XMapWindow(display, white_window);
	XStoreName(display, white_window, white_winname);

	transient_for_window = black_window;

	XSetTransientForHint(display, white_window, transient_for_window);

	XMapWindow(display, white_window);
	if (XGetTransientForHint(display, white_window, &new_trans_for_window)
		== 0)
		fprintf(stderr, "XGetTransientForHint call 3 failed\n");

	PIInfo[PIInfo_cnt].actual_return =
		((new_trans_for_window == transient_for_window));
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









