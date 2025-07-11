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
static char rcsid[] = "$XConsortium: PISizeHints.c /main/6 1995/07/13 20:24:21 drk $"
#endif
#endif

#include	<testlib.h>
#include	<PITests.h>

#include	"PISizeHints.h" 
#include	"btn1.bmp"


static void	ApplySizeValues(w, client_data, call_data)
Widget	w;
XtPointer	client_data, call_data;
{

	Arg	args[MAX_ARGS];
	int	n;
	int	min_width, min_height, max_width, max_height;
	int	base_width, base_height;
	int	width_inc, height_inc;

	n = 0;
	XtSetArg (args[n], XmNvalue, &min_width); n++;
	XtGetValues (min_widthScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &min_height); n++;
	XtGetValues (min_heightScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &max_width); n++;
	XtGetValues (max_widthScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &max_height); n++;
	XtGetValues (max_heightScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &base_width); n++;
	XtGetValues (base_widthScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &base_height); n++;
	XtGetValues (base_heightScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &width_inc); n++;
	XtGetValues (width_incScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &height_inc); n++;
	XtGetValues (height_incScale, args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, max_width); n++;
	XtSetArg(args[n], XmNheight, max_height); n++;
	XtSetValues(test_top, args, n);

	PIUnmapWindow(test_wdw);
	/* Set the new values on test_wdw */
	wm_set_size.flags = (PPosition | PSize | PMinSize | PMaxSize |
						 PResizeInc | PBaseSize);

	wm_set_size.x = 100;
	wm_set_size.y = 100;
	wm_set_size.width = 100;
	wm_set_size.height = 100;

	wm_set_size.min_width = min_width;
	wm_set_size.min_height = min_height;
	wm_set_size.max_width = max_width;
	wm_set_size.max_height = max_height;
	wm_set_size.base_width = base_width;
	wm_set_size.base_height = base_height;
	wm_set_size.width_inc = width_inc;
	wm_set_size.height_inc = height_inc;

	XSetWMNormalHints(display, test_wdw, &wm_set_size);

	XMapWindow(display, test_wdw);
    XMoveWindow(display, test_wdw, 100, 100);

	return;

}


static void	ApplyAspectValues(w, client_data, call_data)
Widget	w;
XtPointer	client_data, call_data;
{

	Arg	args[MAX_ARGS];
	int	n;
	int	min_aspect_x, min_aspect_y, max_aspect_x, max_aspect_y;

	n = 0;
	XtSetArg (args[n], XmNvalue, &min_aspect_x); n++;
	XtGetValues (min_aspect_xScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &min_aspect_y); n++;
	XtGetValues (min_aspect_yScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &max_aspect_x); n++;
	XtGetValues (max_aspect_xScale, args, n);

	n = 0;
	XtSetArg (args[n], XmNvalue, &max_aspect_y); n++;
	XtGetValues (max_aspect_yScale, args, n);

	PIUnmapWindow(test_wdw);
	/* Set the new values on test_wdw */
	wm_set_size.flags = PSize | PAspect;

	wm_set_size.x = 100;
	wm_set_size.y = 100;
	wm_set_size.width = 100;
	wm_set_size.height = 100;

	wm_set_size.min_aspect.x = min_aspect_x;
	wm_set_size.min_aspect.y = min_aspect_y;
	wm_set_size.max_aspect.x = max_aspect_x;
	wm_set_size.max_aspect.y = max_aspect_y;

	XSetWMNormalHints(display, test_wdw, &wm_set_size);
	XMapWindow(display, test_wdw);
    XMoveWindow(display, test_wdw, 100, 100);

	return;

}


main(argc, argv)
unsigned int	argc;
char	*argv[];
{

	Arg     args[MAX_ARGS];
	int     n;
	int		i;

	XmString    message;

    CommonTestInit(argc, argv);

    n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);

    CommonPause();

/*
    
/*
 * Create top level shell widget as parent for test1_btn1 widget
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

	screenWidth = WidthOfScreen(screen);
	screenHeight = HeightOfScreen(screen);

	n = 0;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
	XtSetArg(args[n], XmNtitle, "Control Panel"); n++;
	XtSetArg(args[n], XtNgeometry, "+100+100"); n++;
	control_panel = XtAppCreateShell("control_panel", NULL,
		 							 topLevelShellWidgetClass,
		 							 display, args, n);

	controlBB = XmCreateBulletinBoard (control_panel, "controlBB", args, n);
	XtManageChild(controlBB);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Min_width")); n++;
	XtSetArg(args[n], XmNmaximum, (screenWidth / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	min_widthScale = XmCreateScale (controlBB, "min_widthScale", args, n);
	XtManageChild(min_widthScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Min_height")); n++;
	XtSetArg(args[n], XmNmaximum, (screenHeight / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	min_heightScale = XmCreateScale (controlBB, "min_heightScale", args, n);
	XtManageChild(min_heightScale);


	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNvalue, (screenWidth / 2)); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Max_width")); n++;
	XtSetArg(args[n], XmNmaximum, screenWidth); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	max_widthScale = XmCreateScale (controlBB, "max_widthScale", args, n);
	XtManageChild(max_widthScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNvalue, (screenHeight / 2)); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Max_height")); n++;
	XtSetArg(args[n], XmNmaximum, screenHeight); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	max_heightScale = XmCreateScale (controlBB, "max_heightScale", args, n);
	XtManageChild(max_heightScale);


	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 300); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Base_width")); n++;
	XtSetArg(args[n], XmNmaximum, (screenWidth / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	base_widthScale = XmCreateScale (controlBB, "base_widthScale", args, n);
	XtManageChild(base_widthScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 300); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Base_height")); n++;
	XtSetArg(args[n], XmNmaximum, (screenHeight / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	base_heightScale = XmCreateScale (controlBB, "base_heightScale", args, n);
	XtManageChild(base_heightScale);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 400); n++;
	XtSetArg(args[n], XmNvalue, 1); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Width_inc")); n++;
	XtSetArg(args[n], XmNmaximum, (screenWidth / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 5); n++;
	width_incScale = XmCreateScale (controlBB, "width_incScale", args, n);
	XtManageChild(width_incScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 400); n++;
	XtSetArg(args[n], XmNvalue, 1); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Height_inc")); n++;
	XtSetArg(args[n], XmNmaximum, (screenHeight / 2)); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 5); n++;
	height_incScale = XmCreateScale (controlBB, "height_incScale", args, n);
	XtManageChild(height_incScale);

    n = 0;
	XtSetArg (args[n], XmNx, 10); n++;
	XtSetArg (args[n], XmNy, 500); n++;
	applyButton = XmCreatePushButtonGadget (controlBB, "Apply", args, n);
	XtManageChild (applyButton);
	XtAddCallback (applyButton, XmNactivateCallback, ApplySizeValues, NULL);

	XtRealizeWidget(control_panel);
	CommonPause();

	XtDestroyWidget(min_widthScale);
	XtDestroyWidget(min_heightScale);
	XtDestroyWidget(max_widthScale);
	XtDestroyWidget(max_heightScale);
	XtDestroyWidget(base_widthScale);
	XtDestroyWidget(base_heightScale);
	XtDestroyWidget(width_incScale);
	XtDestroyWidget(height_incScale);
	XtDestroyWidget(applyButton);
	XtDestroyWidget(controlBB);
	XtUnrealizeWidget(control_panel);

	controlBB = XmCreateBulletinBoard (control_panel, "controlBB", args, n);
	XtManageChild(controlBB);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Min_aspect_x")); n++;
	XtSetArg(args[n], XmNmaximum, screenWidth); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	min_aspect_xScale = XmCreateScale (controlBB, "min_aspect_xScale", args, n);
	XtManageChild(min_aspect_xScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 100); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Min_apsect_y")); n++;
	XtSetArg(args[n], XmNmaximum, screenHeight); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	min_aspect_yScale = XmCreateScale (controlBB, "min_aspect_yScale", args, n);
	XtManageChild(min_aspect_yScale);


	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Max_aspect_x")); n++;
	XtSetArg(args[n], XmNmaximum, screenWidth); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	max_aspect_xScale = XmCreateScale (controlBB, "max_aspect_xScale", args, n);
	XtManageChild(max_aspect_xScale);

	n = 0;
	XtSetArg(args[n], XmNx, 200); n++;
	XtSetArg(args[n], XmNy, 200); n++;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], XmNtitleString,
			 XmStringCreateSimple ("Max_aspect_y")); n++;
	XtSetArg(args[n], XmNmaximum, screenHeight); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleMultiple, 10); n++;
	max_aspect_yScale = XmCreateScale (controlBB, "max_aspect_yScale", args, n);
	XtManageChild(max_aspect_yScale);

    n = 0;
	XtSetArg (args[n], XmNx, 10); n++;
	XtSetArg (args[n], XmNy, 300); n++;
	applyButton = XmCreatePushButtonGadget (controlBB, "Apply", args, n);
	XtManageChild (applyButton);
	XtAddCallback (applyButton, XmNactivateCallback, ApplyAspectValues, NULL);

	XtRealizeWidget(control_panel);
	CommonPause();

	XtDestroyWidget(min_aspect_xScale);
	XtDestroyWidget(min_aspect_yScale);
	XtDestroyWidget(max_aspect_xScale);
	XtDestroyWidget(max_aspect_yScale);
	XtDestroyWidget(applyButton);
	XtDestroyWidget(controlBB);

	CommonPause();

/*
 *  Loop and process events.
 */

	XtAppMainLoop(app_context);

}

