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
static char rcsid[] = "$XConsortium: PIColormap.c /main/7 1995/07/13 20:18:29 drk $"
#endif
#endif
#include	<X11/Xatom.h>

#include	<testlib.h>



#include	<PITests.h>
#include	"PIColormap.h"
#include	"btn1.bmp"
#include	"btn2.bmp"


Widget popupShell;
void popup(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
    XtPopup(popupShell, XtGrabNone);
}

void popdown(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
    XtPopdown(popupShell);
}

void colormap(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
    Window windows[2];
    windows[0] = XtWindow(popupShell);
    windows[1] = XtWindow(Shell1);
    XSetWMColormapWindows(XtDisplay(Shell1), XtWindow(Shell1), windows, 2);
}

/*
 * Button callback
 */
void button_proc(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;

{

	cm_list = XListInstalledColormaps(display, rootWindow, &cm_num);

/*
 * Check if cmap1 is listed as installed    WM_COLORMAP_WINDOWS
 */
	if (iteration == VALID_TEST1)
		PIInfo[PIInfo_CMstart + VALID_TEST1].actual_return =
												(cm_list[0] == cmap1);
/*
 * Check if cmap2 is listed as installed    WM_COLORMAP_WINDOWS
 */
	else if (iteration == VALID_TEST2)
		PIInfo[PIInfo_CMstart + VALID_TEST2].actual_return =
			(cm_list[0] == cmap2);
/*
 * Check if def_cmap is listed as installed WM_COLORMAP_WINDOWS
 */
	else if (iteration == VALID_TEST3)
		PIInfo[PIInfo_CMstart + VALID_TEST3].actual_return =
			(cm_list[0] == def_cmap);
	else
		printf("Error, Hit PushButton callback, cmap is: %d\n", cm_list[0]);


	XFree(cm_list);

}






main(argc, argv)
unsigned int	argc;
char	*argv[];
{

	Arg     args[10];
	int     n;
	int		i;
        Widget rc, button;
	XmString    message;

    CommonTestInit(argc, argv);
    
    n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);

/*
 *  Realize the Shell1 widget.
 */

	XtRealizeWidget(Shell1);

    CommonPause();
	/*
	 * Initialize the callbacks actual returns
	 */
	for (i = (PIInfo_CMstart + VALID_TEST1);
		 i < (PIInfo_CMstart + VALID_TEST3); 
		 i++)
		PIInfo[i].actual_return = False;
    
/*
 * Create top level shell widget as parent for test_button widgets
 */
	n = 0;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;

	test_top = XtAppCreateShell("test_top",				/* application name */
		 						"Test_top",				/* application class */
		 						topLevelShellWidgetClass,	/* widget class */
		 						display,				/* display */
		 						args, n);				/* arguments */

/*
 * Create a Form box
 */
	n = 0;
	test_box = XmCreateForm(test_top, "test_box", args, n);
	XtManageChild(test_box);
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
	test2_btn1_icon = XCreatePixmapFromBitmapData(display, rootWindow,
								btn2_bits, btn2_width,
								btn2_height,
								CommonGetColor("yellow"),
								CommonGetColor("red"),
								DefaultDepth(display, DefaultScreen(display)));
	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNlabelPixmap, test1_btn1_icon); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	test1_btn1 = XmCreatePushButton(test_box, "test1_btn1", args, n);
	XtManageChild(test1_btn1);
	XtAddCallback(test1_btn1, XmNarmCallback, button_proc, NULL);
	XtRealizeWidget(test_top);

	test_wdw = XtWindow(test_top);
	top_wdw = XtWindow(Shell1);
	button_wdw	= XtWindow(test1_btn1);
	def_scrn = DefaultScreen(display);

	/*
	 * Move the test_top shell window and the Shell1 window
	 */
	XMoveWindow(display, test_wdw, 100, 100);
	XMoveWindow(display, top_wdw, 300, 100);

    /*
	 * Initialize some structures and read in
  	 * an initial color set to work from.
	 */
	ncolors = DisplayCells(display, def_scrn);
	def_cmap = DefaultColormap(display, def_scrn);

    /* initialize a color set to use for new colormaps */
	for (i=0; i < DisplayCells(display, 0); i++) {
		colors[i].pixel = i;
		colors[i].flags = DoRed | DoGreen | DoBlue;
	}
	XQueryColors(display, def_cmap, colors, ncolors);

    /*
	 * make a new colormap - cmap1
	 */
	cmap1 = XCreateColormap(display, rootWindow,
							DefaultVisual(display, def_scrn), AllocAll);

	/*
	 * Set the colormap attributes in each window differently
	 */
	XSetWindowColormap(display, button_wdw, cmap1);
	XSetWindowColormap(display, test_wdw, def_cmap);

	test_atom = XmInternAtom(display, "WM_COLORMAP_WINDOWS", False);
	wdw_list[0] = button_wdw;
	wdw_list[1] = test_wdw;
	XChangeProperty(display, test_wdw, test_atom, XA_WINDOW, 32, 
					PropModeReplace, (unsigned char *)wdw_list, 2);

	/*
	 * Initialize the callbacks actual returns 
	 */
	for (i = (PIInfo_CMstart + VALID_TEST1);
		 i < (PIInfo_CMstart + VALID_TEST3);
		 i++)
		PIInfo[i].actual_return = False;

/*
 * Check if def_cmap is initially installed WM_COLORMAP_WINDOWS
 */
	cm_list = XListInstalledColormaps(display, rootWindow, &cm_num);
	PIInfo[PIInfo_INIT].actual_return = (cm_list[0] == def_cmap);
	XFree(cm_list);
	PIInfo_cnt++;

    CommonPause();
	iteration++;

/*
 * Create button2 widget
 */
	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNlabelPixmap, test2_btn1_icon); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 130); n++;
	test2_btn1 = XmCreatePushButton(test_box, "test2_btn1", args, n);
	XtManageChild(test2_btn1);
	XtAddCallback(test2_btn1, XmNarmCallback, button_proc, NULL);

	button_wdw2	= XtWindow(test2_btn1);

	/*
	 * make another colormap - cmap2
	 */
	cmap2 = XCreateColormap(display, rootWindow,
							DefaultVisual(display, def_scrn), AllocAll);
	/*
	 * copy the colors into the second new cmap,
	 * shifted one more position
	 */
	new_colors[ncolors - 1].pixel = ncolors;
	new_colors[ncolors - 1] = colors[1];
	new_colors[ncolors - 2].pixel = ncolors - 1;
	new_colors[ncolors - 2] = colors[0];
	for (i=0; i < (ncolors - 2); i++) {
		new_colors[i] = colors[i + 2];
		new_colors[i].pixel = i;
	}
	XStoreColors(display, cmap2, new_colors, ncolors);

	/*
	 * Set the colormap attributes in this window
	 */
	XSetWindowColormap(display, button_wdw2, cmap2);

	/*
	 * Put button_wdw2 in as first in window list
	 */
	test_atom = XmInternAtom(display, "WM_COLORMAP_WINDOWS", False);
	wdw_list[0] = button_wdw2;
	wdw_list[1] = test_wdw;
	XChangeProperty(display, test_wdw, test_atom, XA_WINDOW, 32, 
					PropModeReplace, (unsigned char *)wdw_list, 2);

	CommonPause();
	iteration++;

	/*
	 * Put the Shell1 window back at top of window list
	 */
	test_atom = XmInternAtom(display, "WM_COLORMAP_WINDOWS", False);
	wdw_list[0] = test_wdw;
	wdw_list[0] = test_wdw;
	wdw_list[1] = button_wdw;
	wdw_list[2] = button_wdw2;
	XChangeProperty(display, test_wdw, test_atom, XA_WINDOW, 32, 
					PropModeReplace, (unsigned char *)wdw_list, 3);

	CommonPause();

    XtDestroyWidget (test_top);

   
    /* test for CR 5183 */

    rc = XmCreateRowColumn(Shell1, "rc", NULL, 0);
    XtManageChild(rc);
    button = XmCreatePushButton(rc, "Popup", NULL, 0);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, popup, NULL);
    button = XmCreatePushButton(rc, "Popdown", NULL, 0);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, popdown, NULL);
    button = XmCreatePushButton(rc, "Colormap", NULL, 0);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, colormap, NULL);
    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++ ;
    XtSetArg(args[n], XmNheight, 100); n++ ;
    XtSetArg(args[n], XmNx, 100); n++ ;
    XtSetArg(args[n], XmNy, 100); n++ ;
    popupShell = XtCreatePopupShell("popup", overrideShellWidgetClass,
                                    Shell1, args, n);
    
    /* the popup shell must be realized so that XSetWMColormapWindows is
     * guaranteed to have a window
     */
    XtRealizeWidget(popupShell);
    CommonPause();

    /* end test for CR 5183 */

/*
 *  Loop and process events.
 */


	/*
 	 * Put up a message box with the results of the test.
 	 */
	display_summary(NUM_TESTS, TEST_DESCRIP_LENGTH, PIInfo);

	CommonPause();



	XtAppMainLoop(app_context);

}

