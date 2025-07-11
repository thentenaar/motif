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
static char rcsid[] = "$XConsortium: PIProto.c /main/8 1995/07/13 20:23:29 drk $"
#endif
#endif

#include	<testlib.h>
#include	<PITests.h>

#include	"PIProto.h" 
#include	"btn1.bmp"
#include	"btn2.bmp"


static int
ErrorHandle(Display *disp, XErrorEvent *err)
{
	char	msg[80];
	XGetErrorText(disp, err->error_code, msg, 80);
	printf("X Error code: %s\n", msg);
}


static void
CMEventHandler(Widget w, XtPointer client_data, XEvent *ev, 
			   Boolean *continue_to_dispatch)
{
	XClientMessageEvent	*cm_ev;

	if (ev->type == ClientMessage)
	{
		cm_ev = (XClientMessageEvent *)ev;

		if (cm_ev->message_type == proto_atom)
		{
			if (cm_ev->data.l[0] == delwin_atom)
			{
				printf("Close window message sent.\n");
				/*
				 * Check if ClientMessage sent for f.kill      WM_DELETE_WINDOW
				 */
				PIInfo[PIInfo_DW1].actual_return = (w == test_top);
			}
	
#ifdef	DONT
			if (cm_ev->data.l[0] == savylf_atom)
			{
				printf("Save yourself message sent.\n");
				/*
				 * Check ClientMessage sent for state change   WM_SAVE_YOURSELF
				 */
				PIInfo[PIInfo_SY].actual_return = (w == test_top2);
			}

			if (cm_ev->data.l[0] == takfoc_atom)
			{
				printf("Take Focus message sent.\n");
				/*
				 * Check ClientMessage sent for take focus        WM_TAKE_FOCUS
				 */
				PIInfo[PIInfo_TF].actual_return = (w == test_top2);
			}
#endif	/* DONT */

		}
		else
		if (cm_ev->message_type == motif_msg_atom)
		{
			printf("Motif Message message sent.\n");
			/*
			 * Check ClientMessage sent for motif message _MOTIF_WM_MESSAGE
			 */
			PIInfo[PIInfo_MM].actual_return =
				(
				(cm_ev->data.l[0] == MSG_NUMBER) ||
				(cm_ev->data.l[0] == MSG_NUMBER2) ||
				(cm_ev->data.l[0] == MSG_NUMBER3)
				)
				;
		}

	}
	else
	{
		printf("\nEventHandler, ev type = %d\n", ev->type);
	}
}


/*
 * This is here to handle callbacks from the _MOTIF_WM_MESSAGE message
 * in particular, the MSG_NUMBER send_msg
 */
static void
HandleMsgCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	printf("Motif Message callback hit.\n");
	/*
	 * Check callback sent for motif message      _MOTIF_WM_MESSAGE
	 */
	PIInfo[PIInfo_CB].actual_return = True;
}

/*
 * This is here to handle callbacks from the _MOTIF_WM_MESSAGE message
 * in particular, the MSG_NUMBER2 send_msg
 */
static void
HandleMsg2CB(Widget w, XtPointer client_data, XtPointer call_data)
{
	printf("Motif Message2 callback hit.\n");
#ifdef SHOW_P899

	if (iteration == VALID_TEST3)
	{
		/*
		 * Check callback sent for menu item 2 (added)   _MOTIF_WM_MENU
		 */
		PIInfo[PIInfo_CB2a].actual_return = True;
	}
	else if (iteration == VALID_TEST4)
	{
		/*
		 * Check callback sent for menu item 2 (replaced)_MOTIF_WM_MENU
		 */
		PIInfo[PIInfo_CB2r].actual_return = True;
	}

#else

	if (iteration == VALID_TEST3)
	{
		/*
		 * Check callback sent for menu item 2 (replaced)_MOTIF_WM_MENU
		 */
		PIInfo[PIInfo_CB2r].actual_return = True;
	}

#endif /* P899 */

	else
	{
		printf("HandleMsg2CB: error bad iteration = %d\n", iteration);
	}
}

/*
 * This is here to handle callbacks from the _MOTIF_WM_MESSAGE message
 * in particular, the MSG_NUMBER3 send_msg
 */
static void
HandleMsg3CB(Widget w, XtPointer client_data, XtPointer call_data)
{
	printf("Motif Message3 callback hit.\n");
	/*
	 * Check callback sent for menu item 3 (added)   _MOTIF_WM_MENU
	 */
	PIInfo[PIInfo_CB3].actual_return = True;
}


void
main(unsigned int argc, char **argv)
{

	Arg     args[10];
	int     n;
	int		i;

	XmString    message;
	XmString    cancel_label, ok_label, help_label;
	char    message_begin[120];


	n = 0;

    CommonTestInit(argc, argv);

    n = 0;
	XtSetArg(args[n], XmNwidth, 10); n++;
	XtSetArg(args[n], XmNheight, 10); n++;
	XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);

    CommonPause();
    
	XSetErrorHandler(ErrorHandle);
	/*
 	 * Initialize the PIInfo data values
  	 */
	for (i = 0; i < NUM_TESTS; i++)
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
	test1_btn1 = XmCreatePushButton(test_top, "test1_btn1", args, n);
	XtManageChild(test1_btn1);
	XtRealizeWidget(test_top);

/*
 * Create top level shell widget as parent for test_button2 widget
 */
	n = 0;
	XtSetArg(args[n], XtNallowShellResize, TRUE); n++;

	test_top2 = XtAppCreateShell("test_top2",			/* application name */
		 						"Test_top2",			/* application class */
		 						topLevelShellWidgetClass,	/* widget class */
		 						display,				/* display */
		 						args, n);				/* arguments */

/*
 * Create button2 widget
 */
	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	XtSetArg(args[n], XmNlabelPixmap, test2_btn1_icon); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	test2_btn1 = XmCreatePushButton(test_top2, "test2_btn1", args, n);
	XtManageChild(test2_btn1);
	XtRealizeWidget(test_top2);

	test_wdw = XtWindow(test_top);
	top_wdw = XtWindow(Shell1);
	test_wdw2 = XtWindow(test_top2);

	XtAddEventHandler(Shell1, NoEventMask, True, CMEventHandler, NULL);
	XtAddEventHandler(test_top, NoEventMask, True, CMEventHandler, NULL);
	XtAddEventHandler(test_top2, NoEventMask, True, CMEventHandler, NULL);

	/*
	 * Move the test_top shell window and the Shell1 window
	 */
	XMoveWindow(display, test_wdw, 100, 100);
	XMoveWindow(display, test_wdw2, 100, 300);

	/*
 	 * set protocols - WM_DELETE_WINDOW
  	 */
	proto_atom = XmInternAtom(display, "WM_PROTOCOLS", False);
	delwin_atom = XmInternAtom(display, "WM_DELETE_WINDOW", False);
	XmAddWMProtocols(test_top, &delwin_atom, 1);

#ifdef	DONT
/*
 * set protocols - WM_SAVE_YOURSELF
 */
		savylf_atom = XmInternAtom(display, "WM_SAVE_YOURSELF", False);
		XmAddWMProtocols(test_top2, &savylf_atom, 1);

/*
 * set protocols - WM_TAKE_FOCUS
 */
		takfoc_atom = XmInternAtom(display, "WM_TAKE_FOCUS", False);
		/* change it on test_wdw2 */
		XChangeProperty(display, test_wdw2,
			proto_atom, 
			proto_atom, 32, PropModeReplace,
			(unsigned char *)&takfoc_atom, 1);
#endif	/* DONT */

    CommonPause();
	iteration++;

	/*
 	 * Check if window is not deleted after f.kill WM_DELETE_WINDOW
  	 */
	ret_stat = XGetWindowAttributes(display, test_wdw, &win_attrs);
	/*
	 * Actual return is if the window is still mapped and
	 * recognizable.
	 */
	PIInfo[PIInfo_DW2].actual_return = ret_stat;

	/*
  	 * set protocols - _MOTIF_WM_MESSAGES
  	 */
	/* add to the menu with the _MOTIF_WM_MENU property */
	strcpy(menu_item, send_msg1);
	menu_atom = XmInternAtom(display, "_MOTIF_WM_MENU", False);
	XChangeProperty(display, test_wdw2, menu_atom, menu_atom, 8, 
					PropModeReplace, (unsigned char *)menu_item, 
					strlen(menu_item) + 1);
    /* add a message acceptance to widget */
	motif_msg_atom = XmInternAtom(display, "_MOTIF_WM_MESSAGES", False);
	XmAddWMProtocols(test_top2, &motif_msg_atom, 1);

	msg_num = MSG_NUMBER;
	XmAddProtocolCallback(test_top2, motif_msg_atom, (Atom)msg_num,
						  HandleMsgCB, NULL);

	PIUnmapWindow(test_wdw2);
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw2, &wm_set_hints);
	XMapWindow(display, test_wdw2);
	XMoveWindow(display, test_wdw2, 100, 300);

#ifdef	DONT
/*
 * Iconify test_wdw2, see if save_yourself works
 */
	/*
	 * Unmap the window to change state
	 */
	PIUnmapWindow(test_wdw2);
	/* 
	 * Map the window into iconic state
	 */
	wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = IconicState;
	XSetWMHints(display, test_wdw2, &wm_set_hints);
	XMapWindow(display, test_wdw2);
#endif /* DONT */

	CommonPause();
#ifdef SHOW_P899

	iteration++;

	/*
 	 * set protocols - _MOTIF_WM_MENU
  	 */
	/* add to the menu with the _MOTIF_WM_MENU property */
	strcpy(menu_item2, send_msg2);
	XChangeProperty(display, test_wdw2, menu_atom, menu_atom, 8, 
					PropModeAppend, (unsigned char *)menu_item2, 
					strlen(menu_item2) + 1);
	/*
 	 * Turn on acceptance of send_message2
  	 */
	msg_num = MSG_NUMBER2;
	XmAddProtocolCallback(test_top2, motif_msg_atom, (Atom)msg_num,
						  HandleMsg2CB, NULL);
	PIUnmapWindow(test_wdw2);
    wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw2, &wm_set_hints);
	XMapWindow(display, test_wdw2);
	XMoveWindow(display, test_wdw2, 100, 300);

	CommonPause();

#endif /* P899 */
	iteration++;

	/*
 	 * set protocols - _MOTIF_WM_MENU
  	 */
	/* add to the menu with the _MOTIF_WM_MENU property */
	strcpy(menu_item2, send_msg3);
	XChangeProperty(display, test_wdw2, menu_atom, menu_atom, 8, 
					PropModeReplace, (unsigned char *)menu_item2, 
					strlen(menu_item2) + 1);
#ifndef P899
	/*
 	 * Turn on acceptance of send_message2
  	 */
	msg_num = MSG_NUMBER2;
	XmAddProtocolCallback(test_top2, motif_msg_atom, (Atom)msg_num,
						  HandleMsg2CB, NULL);
#endif /* P899 */
	PIUnmapWindow(test_wdw2);
    wm_set_hints.flags = (StateHint);
	wm_set_hints.initial_state = NormalState;
	XSetWMHints(display, test_wdw2, &wm_set_hints);
	XMapWindow(display, test_wdw2);
	XMoveWindow(display, test_wdw2, 100, 300);

	CommonPause();
	iteration++;

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

