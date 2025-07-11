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
static char rcsid[] = "$XConsortium: Cutpaste4a.c /main/7 1995/07/13 17:56:29 drk $"
#endif
#endif

#include <testlib.h>

#define NULL 0
#define TRUE 1

static int	big = 0;
static int	by_name = 0;
static long	private_id = 0;
static long	data_id = 0;

static char	*set_text = "initial text";
static char	*recopy_text = "recopy";

static char	*format_name = "STRING";
static int	length_text = 15;
static int	count_text = 0;
static unsigned long max_length = 0;

static int	y_position = 5;
static int	t_position = 90;
static int	l_position = 5;

static Boolean	copy_started;
static Boolean	retr_started;

static XmString	cs;
static XmString	csv;

static void	cut_by_name_callback();
static void	drawbuttons();
static void	begin_copy();
static void	start_copy();
static void	copy_to();
static void	end_copy();
static void	start_copy_from();
static void	copy_from();
static void	end_copy_from();
static void	cancel_copy();
static void	undo_copy();
static void	copy_by_name();
static void	inq_next_format();
static void	inq_next_count();
static void	inq_next_length();
static void	list_pending();
static void	lock_unlock();
static void	display_copy_warning();
static void	display_retr_warning();
static void	ok_callback();

static void	begin_copy_test();
static void	start_copy_test();
static void	copy_by_name_test();
static void	undo_copy_test();
static void	cancel_copy_test();
static void	start_copy_from_test();
static void	mult_copy_from_test();
static void	inquire_test();

static XtCallbackRec	begin_copy_callback[3] = {begin_copy_test, NULL, NULL};
static XtCallbackRec	start_copy_callback[3] = {start_copy_test, NULL, NULL};
static XtCallbackRec	copy_by_name_callback[3] =
					{copy_by_name_test, NULL, NULL};
static XtCallbackRec	undo_copy_callback[3] = {undo_copy_test, NULL, NULL};
static XtCallbackRec	cancel_copy_callback[3] =
					{cancel_copy_test, NULL, NULL};
static XtCallbackRec	start_copy_from_callback[3] =
					{start_copy_from_test, NULL, NULL};
static XtCallbackRec	mult_copy_from_callback[3] =
					{mult_copy_from_test, NULL, NULL};
static XtCallbackRec	inquire_callback[3]  = {inquire_test, NULL, NULL};
static XtCallbackRec	copy_ok_callback[3]  = {ok_callback, NULL, NULL};
static XtCallbackRec	retrieve_ok_callback[3]  = {ok_callback, NULL, NULL};
static XtCallbackRec	dd_ok_callback[3]  = {ok_callback, NULL, NULL};
static XtCallbackRec	lock_ok_callback[3]  = {ok_callback, NULL, NULL};

static XrmOptionDescRec table[] =
{
    {"-v",	"value",		XrmoptionSepArg, NULL},
    {"-c",	"numwidgets",		XrmoptionSepArg, NULL},
    {"-sync",	"syncit",		XrmoptionNoArg,  "on"},
    {"-w",	"Text.wordWrap",	XrmoptionNoArg,  "on"},
    {"-nw",	"Text.wordWrap",	XrmoptionNoArg,  "off"},
};

static char		*value;
static long		itemid;
static int		locked;
static Window		window;
XmStringCharSet		charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;

static Widget		w,
			panedwindow,
			workdialog, 
			setdialog,
			functdialog,
			worktext,
			settext,
			nametext,
			lengthtext,
			counttext,
			maxlength,
			pending,
			buttons[8];


main(argc, argv)
     unsigned int	argc;
     char		*argv[];
{
    Arg	args[20];
    int	n;
    int	i;


    copy_started = False;
    retr_started = False;

    CommonTestInit(argc, argv);

    panedwindow = XmCreatePanedWindow(Shell1, "panedwindow", NULL, 0);
    XtManageChild(panedwindow);

    cutpastetester(panedwindow);
    
    XtRealizeWidget(Shell1);

    window = XtWindow(panedwindow);

    XSynchronize(display, TRUE);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}

cutpastetester(parent)
     Widget	parent;
{
    Arg		args[20];
    Widget	widgetlist[50];
    WidgetList	widgetList = widgetlist;
    int		n;
    int		i,
    		widgetindex;

    locked = 0;
    big = 0;
    by_name = 0;
    widgetindex = 0;

/*
 * XmBulletinBoard: parent for pushbuttons, text and label widgets
 */

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    setdialog = XmCreateBulletinBoard(parent, "set", args, n);
    XtManageChild(setdialog);

/*
 * XmBulletinBoard: Used for various messages
 */

    n = 0;
    XtSetArg(args[n], XmNheight, 100); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    workdialog = XmCreateBulletinBoard(parent, "work", args, n);
    XtManageChild(workdialog);

/*
 * XmText: Text to be cut
 */

    n = 0;
    XtSetArg(args[n], XmNx, t_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNcolumns, 15); n++;
    XtSetArg(args[n], XmNrows, 1); n++;
    settext = XmCreateText(setdialog, "name", args, n);

    widgetlist[widgetindex] = settext;
    widgetindex++;

/*
 * XmLabel: "to/from text"
 */

    cs = XmStringLtoRCreate("to/from text:", charset);

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

    y_position += 40;

/*
 * XmText: Format name
 */

    n = 0;
    XtSetArg(args[n], XmNx, t_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNcolumns, 15); n++;
    XtSetArg(args[n], XmNrows, 1); n++;
    nametext = XmCreateText(setdialog, "name", args, n);

    widgetlist[widgetindex] = nametext;
    widgetindex++;

/*
 * XmLabel: "format name"
 */

    cs = XmStringLtoRCreate("format name:", charset);

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

    y_position += 40;

/*
 * XmText: Length
 */

    n = 0;
    XtSetArg(args[n], XmNx, t_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNrows, 1); n++;
    lengthtext = XmCreateText(setdialog, "name", args, n);

    widgetlist[widgetindex] = lengthtext;
    widgetindex++;

/*
 * XmLabel: "length"
 */

    cs = XmStringLtoRCreate("length:", charset);

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

    y_position += 40;

/*
 * XmText: Count
 */

    n = 0;
    XtSetArg(args[n], XmNx, t_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNrows, 1); n++;
    counttext = XmCreateText(setdialog, "name", args, n);

    widgetlist[widgetindex] = counttext;
    widgetindex++;

    cs = XmStringLtoRCreate("count:", charset);

/*
 * XmLabel: "count"
 */

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

    y_position += 40;

/*
 * XmText: Max length
 */

    n = 0;
    XtSetArg(args[n], XmNx, t_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNcolumns, 10); n++;
    XtSetArg(args[n], XmNrows, 1); n++;
    maxlength = XmCreateText(setdialog, "name", args, n);

    widgetlist[widgetindex] = maxlength;
    widgetindex++;

/*
 * XmLabel: "max length"
 */

    cs = XmStringLtoRCreate("max length:", charset);

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

    y_position += 40;

/*
 * XmLabel: "pending"
 */

    cs = XmStringLtoRCreate("pending:", charset);

    n = 0;
    XtSetArg(args[n], XmNx, l_position); n++;
    XtSetArg(args[n], XmNy, y_position); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    w = XmCreateLabel(setdialog, "name", args, n);

    widgetlist[widgetindex] = w;
    widgetindex++;

/*
 * Manage children
 */

    XtManageChildren(widgetList, widgetindex);

/*
 * Create pushbuttons
 */
 
    drawbuttons();

}


static void drawbuttons()
{
    Arg args[20];
    int n;
    int	button_number = 0;

    n = 0;
    XtSetArg(args[n], XmNwidth, 130); n++;
    XtSetArg(args[n], XmNx, 200); n++;
    XtSetArg(args[n], XmNy, 8); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_WORK_AREA); n++;
    functdialog = XmCreateForm(setdialog, "funct",  args, n);

/*
 * XmPushButton: Begin Copy Test
 */

    cs = XmStringLtoRCreate("Begin Copy", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, begin_copy_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"begin_copy_test",
						args, n);

    button_number++;

/*
 * XmPushButton: Start Copy Test
 */

    cs = XmStringLtoRCreate("Start Copy", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, start_copy_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"start_copy_test",
						args, n);

    button_number++;

/*
 * XmPushButton: Copy By Name Test
 */

    cs = XmStringLtoRCreate("Copy By Name", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, copy_by_name_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,
						"copy_by_name_test", args,
						n);

    button_number++;

/*
 * XmPushButton: Undo Copy Test
 */

    cs = XmStringLtoRCreate("Undo Copy", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, undo_copy_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"undo_copy_test",
						args, n);

    button_number++;

/*
 * XmPushButton: Cancel Copy Test
 */

    cs = XmStringLtoRCreate("Cancel Copy", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, cancel_copy_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"cancel_copy_test",
						args, n);

    button_number++;

/*
 * XmPushButton: Retrieve Test
 */

    cs = XmStringLtoRCreate("Retrieve", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, start_copy_from_callback);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 15); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"copy_from_test",
						args, n);

    button_number++;

/*
 * XmPushButton: Multiple Retrieve Test
 */

    cs = XmStringLtoRCreate("Multiple Retrieve", charset);

    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, mult_copy_from_callback);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 3); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,
						"mult_copy_from_test", args,
						n);

    button_number++;

/*
 * XmPushButton: Inquire Test
 */

    cs = XmStringLtoRCreate("Inquire Status", charset);


    n = 0;
    XtSetArg(args[n], XmNx, 0); n++;
    XtSetArg(args[n], XmNy, 0); n++;
    XtSetArg(args[n], XmNlabelString, cs); n++;
    XtSetArg(args[n], XmNactivateCallback, inquire_callback); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, buttons[button_number - 1]); n++;
    XtSetArg(args[n], XmNtopOffset, 15); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    buttons[button_number] = XmCreatePushButton(functdialog,"inquire_test",
						args, n);

    button_number++;

    XtManageChildren(buttons, button_number);

    XtManageChild(functdialog);
}

/*
 * Test copy to clipboard using XmClipboardBeginCopy
 */

static void begin_copy_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    begin_copy(widget, client_data, call_data);
    copy_to(widget, client_data, call_data);
    end_copy(widget, client_data, call_data);
    
}

/*
 * Test copy to clipboard using XmClipboardStartCopy
 */

static void start_copy_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    by_name = 1;
    
    start_copy(widget, client_data, call_data);
    copy_to(widget, client_data, call_data);
    end_copy(widget, client_data, call_data);
    
    by_name = 0;
    
}

/*
 * Test copy by name
 */

static void copy_by_name_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    lock_unlock();

    copy_by_name(widget, client_data, call_data);

    lock_unlock();
}

/*
 * Test undo copy
 */

static void undo_copy_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    big = 1;

    undo_copy(widget, client_data, call_data);

    big = 0;
}

/*
 * Test cancel copy
 */

static void cancel_copy_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    set_text = XmTextGetString(settext);
    
    start_copy(widget, client_data, call_data);
    copy_to(widget, client_data, call_data);
    cancel_copy(widget, client_data, call_data);
}

/*
 * Test copy from clipboard
 */

static void start_copy_from_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    start_copy_from(widget, client_data, call_data);
    copy_from(widget, client_data, call_data);
    end_copy_from(widget, client_data, call_data);
}

/*
 * Test multiple copy from clipboard
 */

static void mult_copy_from_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    start_copy(widget, client_data, call_data);
    copy_to(widget, client_data, call_data);
    end_copy(widget, client_data, call_data);
    
    start_copy_from(widget, client_data, call_data);
    copy_from(widget, client_data, call_data);
    copy_from(widget, client_data, call_data);
    end_copy_from(widget, client_data, call_data);
}

/*
 * Test inquiry routines
 */

static void inquire_test(widget, client_data, call_data)
     Widget	*widget;
     XtPointer	client_data;
     XtPointer	call_data;
{
    list_pending(widget, client_data, call_data);
    inq_next_count(widget, client_data, call_data);
    inq_next_length(widget, client_data, call_data);
    inq_next_format(widget, client_data, call_data);
}


static void begin_copy(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    XmString set_cs;
    int status;

    set_cs = XmStringLtoRCreate("Clipboard6", charset);

    status = XmClipboardBeginCopy(display, window, set_cs, workdialog,
				  cut_by_name_callback, &itemid);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
    copy_started = True;
}


static void start_copy(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    XmString set_cs;
    int status;
    XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *)call_data;
    Time timestamp = cb->event->xbutton.time;

    set_cs = XmStringCreateLtoR(set_text, charset);

    status = XmClipboardStartCopy(display, window, set_cs, timestamp,
				  workdialog, cut_by_name_callback, &itemid);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
    copy_started = True;
}


static void copy_to(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char *set_char, *priv_char, *length_char;
    int dummy;
    char *buffer, data_char[20];
    int status, set_length, buffer_size;
    int i;

    if(!copy_started) {
         display_copy_warning();
	 return;
    }

    if(big)
    {
	buffer_size = 263000;
    }else{
        buffer_size = 500;
    }

    if(!by_name)
    {
	buffer = XtMalloc(buffer_size);
	for(i=0; i<buffer_size; i++) buffer[i]='a';
	buffer[buffer_size-2] = 'b';
	buffer[buffer_size-1] = '\0';
    }
    if(big)
    {
        set_char = buffer;
        set_length = buffer_size;
    }else{
        if(by_name) 
        {
	    set_length = length_text;
        }else{
    	set_char = set_text;
    	set_length = strlen(set_char);
        }
    }

    if(by_name) set_char = 0;

    status = XmClipboardCopy(display, window, itemid, format_name, set_char, 
			     set_length, private_id, &data_id);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
}


static void end_copy(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    int status;

    if(!copy_started) {
         display_copy_warning();
	 return;
    }
    status = XmClipboardEndCopy(display, window, itemid);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
    copy_started = False;
}


static void start_copy_from(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    int status;
    XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *)call_data;
    Time timestamp = cb->event->xbutton.time;

    status = XmClipboardStartRetrieve(display, window, timestamp);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }

    retr_started = True;
}


static void copy_from(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char *set_char, *name_char;
    char priv_char[20];
    int status, buffer_size;
    unsigned long copy_length;

    if(big) 
	buffer_size = 263000;
    else
	buffer_size = 12;

    set_char = XtMalloc(buffer_size + 1);

    status = XmClipboardRetrieve(display, window, format_name, set_char,
				 buffer_size, &copy_length, &private_id);

    if(status == ClipboardLocked) 
    { 
	XtFree(set_char);
	display_lock_message();
	return;
    }

    if(buffer_size > 20 && copy_length > 20) 
    {
	set_char[20] = '\0';
    }else{
	if(copy_length < buffer_size)
	{
	    set_char[copy_length] = '\0';
	}else{
	    set_char[buffer_size] = '\0';
	}
    }
    set_text = set_char;
}


static void end_copy_from(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    int status;


    if(!retr_started) {
         display_retr_warning();
	 return;
    }

    status = XmClipboardEndRetrieve(display, window);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
    retr_started = False;
}


static void cancel_copy(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{

    if(!copy_started) {
         display_copy_warning();
	 return;
    }

    XmClipboardCancelCopy(display, window, itemid);

    copy_started = False;
}


static void copy_by_name(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char *set_char, *priv_char, *data_char;
    int dummy;
    int status;

    set_char = recopy_text;
    
    if(strlen(set_char) == 0) set_char = 0;
    status = XmClipboardCopyByName(display, window, data_id, set_char,
				   strlen(set_char), private_id);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
}


static void undo_copy(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    int status;

    status = XmClipboardUndoCopy(display, window);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }
}


static void inq_next_format(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char *n_char, buffer[100];
    unsigned long bufferlength;
    unsigned long outlength;
    int n, dummy;
    int status;

    bufferlength = 100;

    status = XmClipboardInquireFormat(display, window, count_text, buffer,
				      bufferlength, 
				      &outlength);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }

    buffer[(int) outlength] = '\0';

    XmTextSetString(nametext, buffer);
}


static void inq_next_length(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char length_char[20];
    unsigned long length;
    int status;

    status = XmClipboardInquireLength(display, window, format_name, &length);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }

    sprintf(length_char, "%d", length);
    XmTextSetString(lengthtext, length_char);

}


static void inq_next_count(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    char count_char[20];
    int status;

    status = XmClipboardInquireCount(display, window, &count_text,
				     &max_length);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }

    sprintf(count_char, "%d", count_text);
    XmTextSetString(counttext, count_char);
    sprintf(count_char, "%d", (int) max_length);
    XmTextSetString(maxlength, count_char);
}


static void list_pending(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    Arg args[10];
    int n;
    int i;
    unsigned long count;
    int status;
    XmClipboardPendingList list, listptr;
    char *charptrlist[20];
    char charstorage[500];
    char *ptr = charstorage, *tmp;

    status = XmClipboardInquirePendingItems(display, window, format_name,
					    &list, &count);

    if(status == ClipboardLocked) 
    { 
    	display_lock_message();
    	return;
    }

    listptr = list;

    for(i = 0; i < count; i++)
    {
    	charptrlist[i] = ptr;
        sprintf(ptr, "%d", listptr->DataId);
    	tmp = ptr;
    	ptr = tmp + strlen(ptr);
        sprintf(ptr, "   %d", listptr->PrivateId);
    	tmp = ptr;
    	ptr = tmp + strlen(ptr);

	charptrlist[i] = (char *)XmStringCreateLtoR(charptrlist[i], charset);

    	listptr = listptr + 1;

    }

    if(count > 0)
    {
        n = 0;
	XtSetArg(args[n], XmNx, t_position); n++;
	XtSetArg(args[n], XmNy, y_position); n++;
        XtSetArg(args[n], XmNvisibleItemCount, count); n++;
        XtSetArg(args[n], XmNitemCount, count); n++;
        XtSetArg(args[n], XmNitems, charptrlist); n++;
	pending = XmCreateList(setdialog, "name", args, n);
	XtManageChild(pending);
    }
}


static void lock_unlock(widget, client_data, call_data)
Widget *widget;
XtPointer client_data;
XtPointer call_data;
{
    int status;

    if(locked == 1)
    {
    	locked = 0;
    	status = XmClipboardUnlock(display, window, 0);

	if(status == ClipboardLocked) 
	{ 
	    display_lock_message();
    	    locked = 1;
	}
    }else{
    	locked = 1;
    	status = XmClipboardLock(display, window);

	if(status == ClipboardLocked) 
	{ 
	    display_lock_message();
            locked = 0;
	}
    }
}


static void ok_callback(widget, client_data, call_data)
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
	XtUnmanageChild(widget);
}


display_lock_message()
{
    XmString cs;
    Arg args[5];
    int n;
    Widget children[2];

    cs = XmStringLtoRCreate("Clipboard Locked, try again later", charset);

    n = 0;
    XtSetArg(args[n], XmNmessageString, cs); n++;
    XtSetArg(args[n], XmNokCallback, lock_ok_callback); n++;
    w = XmCreateMessageBox(workdialog, "locked", args, n);

    children[0] = XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
    children[1] = XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON);

    XtUnmanageChildren(children, 2);

    XtManageChild(w);

}


static void display_copy_warning()
{
    XmString cs;
    Arg args[5];
    int n;
    Widget children[2];

    cs = XmStringLtoRCreate("Warning: XmStartCopy or XmBeginCopy must be called first", charset);

    n = 0;
    XtSetArg(args[n], XmNmessageString, cs); n++;
    XtSetArg(args[n], XmNokCallback, copy_ok_callback); n++;
    w = XmCreateMessageBox(workdialog, "locked", args, n);

    children[0] = XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
    children[1] = XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON);

    XtUnmanageChildren(children, 2);

    XtManageChild(w);
}


static void display_retr_warning()
{
    XmString cs;
    Arg args[5];
    int n;
    Widget children[2];

    cs = XmStringLtoRCreate("Warning: XmStartRetrieve must be called first",
			    charset);
    n = 0;
    XtSetArg(args[n], XmNmessageString, cs); n++;
    XtSetArg(args[n], XmNautoUnmanage, cs); n++;
    XtSetArg(args[n], XmNokCallback, retrieve_ok_callback); n++;
    w = XmCreateMessageBox(workdialog, "locked", args, n);

    children[0] = XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
    children[1] = XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON);

    XtUnmanageChildren(children, 2);

    XtManageChild(w);
}


static void cut_by_name_callback(widget, dataid, privateid, reason)
Widget widget;
long *dataid;
long *privateid;
int *reason;
{
    XmString cs;
    char data_char[20];
    char private_char[20];
    char *set_char;
    int status, buffer_size;
    Arg args[10];
    int n;
    Widget children[2];
    int i;

    set_char = recopy_text;
    
    if(*reason == XmCR_CLIPBOARD_DATA_REQUEST)
    {
	if(big) 
	{
	    buffer_size = 263000;
	    set_char = XtMalloc(buffer_size);
	    for(i=0; i<buffer_size; i++) set_char[i]='a';
	    set_char[buffer_size-2] = 'b';
	    set_char[buffer_size-1] = '\0';
	}else{
	    buffer_size = strlen(set_char);
	    if(buffer_size == 0) set_char = 0;
	}

	status = XmClipboardCopyByName(display, window, *dataid,
				       set_char, buffer_size, *privateid);
    }

    if(*reason == XmCR_CLIPBOARD_DATA_DELETE)
    {

    	cs = XmStringLtoRCreate("Data Delete Message Recieved", charset);
   
	n = 0;
        XtSetArg(args[n], XmNmessageString, cs); n++;
        XtSetArg(args[n], XmNokCallback, dd_ok_callback); n++;
    	w = XmCreateMessageBox(workdialog, "message", args, n);
    	children[0] = XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
    	children[1] = XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON);

    	XtUnmanageChildren(children, 2);

        XtManageChild(w);
    }
}
