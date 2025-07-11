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
static char rcsid[] = "$XConsortium: DialogRes.c /main/8 1995/07/13 19:20:22 drk $"
#endif
#endif

/*
 *	File:	DialogRes.c
 *
 *	Test created to show PIRs 2427 and 2133
 */

#include <testlib.h>


Widget	Form;
Widget	Dialog1;
Widget	DialogShell1;
Widget	Border, ResizeH, Title, SystemMenu, Minimize, Maximize;
Widget	Size, Move, Iconify, MaximumSize, Close;
Widget	Decor_All, Func_All;
Widget	PostDialog;
Widget	ApplyDecorations, ApplyFunctions;

static XtEventHandler CMEventHandler();

Atom	proto_atom, delwin_atom;
Boolean	decor_all_value, func_all_value;


static void  applyDecorationsCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

	Widget	shell;
	Arg		args[MAX_ARGS];
	int		n;
	int		decorations;
	Boolean	border_value, resizeh_value, title_value, menu_value,
			minimize_value, maximize_value;

	n = 0;
	XtSetArg(args[n], XmNset, &border_value); n++;
	XtGetValues(Border, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &resizeh_value); n++;
	XtGetValues(ResizeH, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &title_value); n++;
	XtGetValues(Title, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &menu_value); n++;
	XtGetValues(SystemMenu, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &minimize_value); n++;
	XtGetValues(Minimize, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &maximize_value); n++;
	XtGetValues(Maximize, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &decor_all_value); n++;
	XtGetValues(Decor_All, args, n);

	decorations = 0;

	if (decor_all_value == True)
		decorations |= MWM_DECOR_ALL;
	if (border_value == True)
		decorations |= MWM_DECOR_BORDER;
	if (resizeh_value == True)
		decorations |= MWM_DECOR_RESIZEH;
	if (title_value == True)
		decorations |= MWM_DECOR_TITLE;
	if (menu_value == True)
		decorations |= MWM_DECOR_MENU;
	if (minimize_value == True)
		decorations |= MWM_DECOR_MINIMIZE;
	if (maximize_value == True)
		decorations |= MWM_DECOR_MAXIMIZE;

	n = 0;
	XtSetArg(args[n], XmNmwmDecorations, decorations); n++;
	XtSetValues(DialogShell1, args, n);

	XtUnmapWidget(DialogShell1);
	XtMapWidget(DialogShell1);

	return;

}


static void  applyFunctionsCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

	Arg		args[MAX_ARGS];
	int		n;
	int		functions;
	Boolean	size_value, move_value, iconify_value, maximumSize_value,
			close_value;

	n = 0;
	XtSetArg(args[n], XmNset, &size_value); n++;
	XtGetValues(Size, args, n);

	n = 0; 
	XtSetArg(args[n], XmNset, &move_value); n++; 
	XtGetValues(Move, args, n); 

	n = 0;
	XtSetArg(args[n], XmNset, &iconify_value); n++;
	XtGetValues(Iconify, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &maximumSize_value); n++;
	XtGetValues(MaximumSize, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &close_value); n++;
	XtGetValues(Close, args, n);

	n = 0;
	XtSetArg(args[n], XmNset, &func_all_value); n++;
	XtGetValues(Func_All, args, n);


	functions = 0;

	if (func_all_value == True)
		functions |= MWM_FUNC_ALL;
	if (size_value == True)
		functions |= MWM_FUNC_RESIZE;
	if (move_value == True)
		functions |= MWM_FUNC_MOVE;
	if (iconify_value == True)
		functions |= MWM_FUNC_MINIMIZE;
	if (maximumSize_value == True)
		functions |= MWM_FUNC_MAXIMIZE;
	if (close_value == True)
		functions |= MWM_FUNC_CLOSE;

	n = 0;
	XtSetArg(args[n], XmNmwmFunctions, functions); n++;
	XtSetValues(DialogShell1, args, n);

	XtUnmapWidget(DialogShell1);
	XtMapWidget(DialogShell1);

	return;

}


											/*Begin PIR2133*/
static XtEventHandler CMEventHandler(w, client_data, ev)
Widget		w;
XtPointer	client_data;
XEvent		*ev;
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
			}
		}
	
	}
}
											/*End PIR2133*/

static void destroyDialogCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

	int	n;
	Arg	args[MAX_ARGS];


	XtDestroyWidget(Dialog1);

	n = 0;
	XtSetArg(args[n], XmNsensitive, True); n++;
	XtSetValues(PostDialog, args, n);

	/* Set all the toggle buttons to default (unset) value */

	n = 0;
	XtSetArg(args[n], XmNset, False); n++;

	XtSetValues(Border, args, n);
	XtSetValues(ResizeH, args, n);
	XtSetValues(SystemMenu, args, n);
	XtSetValues(Title, args, n);
	XtSetValues(Minimize, args, n);
	XtSetValues(Maximize, args, n);
	XtSetValues(Decor_All, args, n);

	if (ApplyDecorations != NULL) {

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ApplyDecorations, args, n);

	}
	/*
	 * If ApplyFunctions != NULL that means the control panel
	 * has been extended to show the mwmFunctions related 
	 * resource toggle buttons. So, unset them also.
	 */
	if (ApplyFunctions != NULL) {

		n = 0;
		XtSetArg(args[n], XmNsensitive, False); n++;
		XtSetValues(ApplyFunctions, args, n);

		n = 0;
		XtSetArg(args[n], XmNset, False); n++;

		XtSetValues(Size, args, n);
		XtSetValues(Move, args, n);
		XtSetValues(Iconify, args, n);
		XtSetValues(MaximumSize, args, n);
		XtSetValues(Close, args, n);
		XtSetValues(Func_All, args, n);

	}

}


static void postDialogCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
    Widget		parent, destroyButton;
    int 		n;
    Arg 		args[MAX_ARGS];
    Widget		cancelButton, okButton;
    XmString label_string;

    parent = (Widget)client_data;



    label_string = XmStringCreateSimple ("This is a modeless dialog");

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, label_string);
			  
    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, "DialogShell1"); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Destroy Me")); n++;
	XtSetArg(args[n], XmNautoUnmanage, False); n++;
    Dialog1 = XmCreateMessageDialog(parent, "DialogShell1", args, n);
    XmStringFree(label_string);

    cancelButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(cancelButton);
    okButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_OK_BUTTON);
	XtUnmanageChild(okButton);

    destroyButton = XmMessageBoxGetChild (Dialog1, XmDIALOG_HELP_BUTTON);
    XtAddCallback (destroyButton, XmNactivateCallback, destroyDialogCB, 
				   Dialog1);

    XtManageChild (Dialog1);

	DialogShell1 = XtParent(Dialog1);

											/*Begin PIR2133*/
    XtAddEventHandler(DialogShell1, NoEventMask, True, 
			(XtEventHandler) CMEventHandler, (XtPointer) NULL);

	/*
 	 * set protocols - WM_DELETE_WINDOW
 	 */

	proto_atom = XmInternAtom(display, "WM_PROTOCOLS", False);
	delwin_atom = XmInternAtom(display, "WM_DELETE_WINDOW", False);
	XmAddWMProtocols(DialogShell1, &delwin_atom, 1);
											/*End PIR2133*/

	n = 0;
	XtSetArg(args[n], XmNsensitive, False); n++;
	XtSetValues(PostDialog, args, n);

	if (ApplyDecorations != NULL) {

		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(ApplyDecorations, args, n);

	}
	if (ApplyFunctions != NULL) {

		n = 0;
		XtSetArg(args[n], XmNsensitive, True); n++;
		XtSetValues(ApplyFunctions, args, n);

	}

}


static Widget  CreateToggle(label, parent)
char   *label;
Widget  parent;
{
	Widget        widget;
	register int  n;
	Arg           args[3];
	XmString      tcs;

	n = 0;
	tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tcs);  n++;
	XtSetArg(args[n], XmNvisibleWhenOff, True); n++;
	widget = XmCreateToggleButton(parent, label, args, n);
	XtManageChild(widget);
	XmStringFree(tcs);

	return(widget);

}


void  main(argc, argv)
int     argc;
char	**argv;
{

	int  		n;
	Arg			args[MAX_ARGS];
	Widget		RowCol1, RowCol2;
	Widget		WindowDecorations, WindowFunctions;
	Widget		Dialog1;
	XmString        label_string;

	CommonTestInit(argc, argv);

    /* Create Outer Form */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);


    label_string = XmStringCreateSimple ("Post Dialog");
    n = 0;
	XtSetArg(args[n], XmNlabelString,label_string); n++;
			 
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
	PostDialog = XmCreatePushButton (Form, "PostDialog", args, n);
    XtManageChild (PostDialog);
    XtAddCallback (PostDialog, XmNactivateCallback, postDialogCB, 
				   Shell1);
    XmStringFree (label_string);



    label_string = XmStringCreateSimple ("Window Decorations:");
    n = 0;
    XtSetArg(args[n], XmNlabelString, label_string); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, PostDialog); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;

    WindowDecorations = XmCreateLabel(Form, "WindowDecorations", args, n);
    XtManageChild(WindowDecorations);
    XmStringFree (label_string);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, WindowDecorations); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;

    RowCol1 = XmCreateRowColumn(Form, "RowCol1", args, n);
    XtManageChild(RowCol1);

	Border = CreateToggle("Border", RowCol1);
	ResizeH = CreateToggle("ResizeH", RowCol1);
	Title = CreateToggle("Title", RowCol1);
	SystemMenu = CreateToggle("SystemMenu", RowCol1);
	Minimize = CreateToggle("Minimize", RowCol1);
	Maximize = CreateToggle("Maximize", RowCol1);
	Decor_All = CreateToggle("Decor_All", RowCol1);

	n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowCol1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
	XtSetArg(args[n], XmNlabelString,
			 XmStringCreateSimple ("ApplyDecorations")); n++;
    XtSetArg(args[n], XmNsensitive, False); n++;
	ApplyDecorations = XmCreatePushButton (Form, "ApplyDecorations", args, n);
    XtManageChild (ApplyDecorations);
    XtAddCallback (ApplyDecorations, XmNactivateCallback, applyDecorationsCB, 
				   Shell1);

	XtRealizeWidget(Shell1);

	CommonPause();

	CommonPause();

	CommonPause();

    label_string = XmStringCreateSimple ("Window Functions:");
    n = 0;
    XtSetArg(args[n], XmNlabelString, label_string); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, ApplyDecorations); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;

    WindowFunctions = XmCreateLabel(Form, "WindowFunctions", args, n);
    XtManageChild(WindowFunctions);
    XmStringFree (label_string);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, WindowFunctions); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;

    RowCol2 = XmCreateRowColumn(Form, "RowCol2", args, n);
    XtManageChild(RowCol2);

	Size = CreateToggle("Size", RowCol2);
	Move = CreateToggle("Move", RowCol2);
	Iconify = CreateToggle("Iconify", RowCol2);
	MaximumSize = CreateToggle("MaximumSize", RowCol2);
	Close = CreateToggle("Close", RowCol2);
	Func_All = CreateToggle("Func_All", RowCol2);

    label_string = XmStringCreateSimple ("ApplyFunctions");
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowCol2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    XtSetArg(args[n], XmNlabelString, label_string); n++;
    ApplyFunctions = XmCreatePushButton (Form, "ApplyFunctions", args, n);
    XtManageChild (ApplyFunctions);
    XtAddCallback (ApplyFunctions, XmNactivateCallback, applyFunctionsCB, 
				   Shell1);
    XmStringFree (label_string);

    CommonPause();

    CommonPause();

    CommonPause();

    CommonPause();

    XtAppMainLoop(app_context);

}

