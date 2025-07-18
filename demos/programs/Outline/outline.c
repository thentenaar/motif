/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * outline.c
 *
 * Almost a direct copy of the tree.c demo code
 *
 */

/**************************************************************
 *		INCLUDE FILES
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>

#include <Xm/Tree.h>
#include <Xm/Outline.h>
#include <Xm/Hierarchy.h>
#include <Xm/IconButton.h>

#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>

/**************************************************************
 *		STATIC FUNCTION DECLARATIONS
 **************************************************************/
static Widget CreateNode(Widget, Widget, char *, XmHierarchyNodeState);
static void BuildHierarchy(Widget, WidgetClass);
static void WriteUpHype(Widget);
static void MakeControlPanel(Widget);
static void ShowCB(Widget, XtPointer, XtPointer);
static void ExplainCB(Widget, XtPointer, XtPointer);
static void CallbackTogCB(Widget, XtPointer, XtPointer);
static void QuitCB(Widget, XtPointer, XtPointer);
void InitializePanel(Widget);

/**************************************************************
 *		GLOBALS
 **************************************************************/
static Widget G_outline = NULL;
static int done = 0;

/**************************************************************
 *		SHOW CODE
 **************************************************************/
#define CODE "void\n\
BuildHierarchy(Widget parent)\n\
{\n\
    Widget outline, hierarchy, w1, w2, w3;\n\
\n\
    outline = XtCreateManagedWidget(\"outline_widget\", xmOutlineWidgetClass,\n\
              parent, NULL, (Cardinal) 0);\n\
\n\
    w1 = CreateNode(outline, NULL, \"Widgets\", XmOpen);\n\
\n\
    w2 = CreateNode(outline, w1, \"Writing Widgets\", XmOpen);\n\
    {\n\
        (void) CreateNode(outline, w2, \"Widget Data Structures\", XmAlwaysOpen);\n\
        (void) CreateNode(outline, w2, \"Structure Naming Conventions\", XmAlwaysOpen);\n\
        w3 = CreateNode(outline, w2, \"Writing Header Files\", XmOpen);\n\
    }\n\
\n\
    (void) CreateNode(outline, w3, \"Writing Private Header Files\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"Writing Public Header Files\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"Internal Header Files\", XmAlwaysOpen);\n\
\n\
    w3 = CreateNode(outline, w2, \"The Base Widget Classes\", XmOpen);\n\
    (void) CreateNode(outline, w3, \"The Core Class Structure\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"The Core Instance Structure\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"The Composite Class Structure\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"The Composite Instance Structure\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"The Constraint Class Structure\", XmAlwaysOpen);\n\
    (void) CreateNode(outline, w3, \"The Constraint Instance Structure\", XmAlwaysOpen);\n\
}\n\
\n\
Widget\n\
CreateNode(Widget w_parent, Widget parent_node, char * name, \n\
           XmHierarchyNodeState state)\n\
{\n\
    Arg args[10];\n\
    Cardinal num_args;\n\
    Widget w;\n\
    XmString xmstring;\n\
\n\
    xmstring = XmStringCreateLocalized(name);\n\
    \n\
    num_args = 0;\n\
    XtSetArg(args[num_args], XmNlabelString, xmstring); num_args++;\n\
    XtSetArg(args[num_args], XmNnodeState, state); num_args++;\n\
    XtSetArg(args[num_args], XmNparentNode, parent_node); num_args++;\n\
    \n\
    w = XtCreateManagedWidget(name, xmPushButtonWidgetClass,\n\
                              w_parent, args, num_args);\n\
\n\
    XmStringFree(xmstring);\n\
    return(w);\n\
}"

/**************************************************************
 *		RESOURCE FALLBACKS
 **************************************************************/
static String fallbacks[] = {
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*hypelabel*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*show_code*fontList: -*-courier-*-r-normal-*-*-*-*-*-*-*-*-*",
    "*show_pb.labelString: Show Layout Code...",
    "*explain_pb.labelString: Other Resources...",
    "*rc_1*XmSeparator*orientation: XmVERTICAL",
    "*ladder_tog.labelString: Connect Style:",
    "*callback_tog.labelString: Node State Callback:",
    "*autoclose_tog.labelString: Auto Close",
    "*connect_tog.labelString: Connect Nodes",
    "*quit_pb.labelString: Quit",
    NULL,
};

/*
 * Function Name: InitializePanel
 * Description:   Checks the resources at start up and sets their
 *                labels and toggles appropriately
 * Arguments:     pane - area containing the buttons and toggles
 * Returns:       Nothing
 *
 */
void
InitializePanel(Widget pane)
{
    Arg args[10];
    Cardinal argcnt;
    Boolean auto_close;
    Boolean connect;
    XmString xmstring;
    XtCallbackStatus status;
    Widget lab_temp, tog_temp;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNautoClose, &auto_close); argcnt++;
    XtSetArg(args[argcnt], XmNconnectNodes, &connect); argcnt++;
    XtGetValues(G_outline, args, argcnt);

    /*
     * Initialize Node State Callback Toggle and Label
     */
    lab_temp = XtNameToWidget(pane, "*callback_lab");
    if (lab_temp == NULL) {
	fprintf(stderr, "InitializePane: cannot find callback_lab\n");
	exit(1);
    }
    tog_temp = XtNameToWidget(pane, "*callback_tog");
    if (tog_temp == NULL) {
	fprintf(stderr, "InitializePane: cannot find callback_tog\n");
	exit(1);
    }

    status = XtHasCallbacks(G_outline, XmNnodeStateCallback);
    switch(status) {
    case XtCallbackHasSome:
	XmToggleButtonSetState(tog_temp, True, False);
	xmstring = XmStringCreateLocalized("Added");
	break;
    case XtCallbackNoList:
    case XtCallbackHasNone:
    default:
	XmToggleButtonSetState(tog_temp, False, False);
	xmstring = XmStringCreateLocalized("Not Added");
	break;
    }
    argcnt = 0;
    XtSetArg(args[argcnt], XmNlabelString, xmstring); argcnt++;
    XtSetValues(lab_temp, args, argcnt);
    XmStringFree(xmstring);

    /*
     * Initialize Auto Close Toggle
     */
    tog_temp = XtNameToWidget(pane, "*autoclose_tog");
    if (tog_temp == NULL) {
	fprintf(stderr, "InitializePane: cannot find autoclose_tog\n");
	exit(1);
    }
	XmToggleButtonSetState(tog_temp, auto_close, False);

    tog_temp = XtNameToWidget(pane, "*connect_tog");
    if (tog_temp == NULL) {
	fprintf(stderr, "InitializePane: cannot find connecct_tog\n");
	exit(1);
    }
	XmToggleButtonSetState(tog_temp, connect, False);
}

/*
 * Function Name: main
 * Description:
 * Arguments:     the usual suspects
 * Returns:       nothing
 *
 */
int
main(int argc, char **argv)
{
    Widget top, pane;
    Arg args[10];
    Cardinal num_args;
    XtAppContext app_con;

    num_args = 0;
    XtSetArg(args[num_args], XmNtitle, "Outline Demo"); num_args++;
    XtSetArg(args[num_args], XmNallowShellResize, True); num_args++;
    top = XtAppInitialize(&app_con, "Treedemo",
			  NULL, 0, &argc, argv,
			  fallbacks, args, num_args);

    {
    Widget sw;
    num_args = 0;
    pane = XtCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				 top, args, num_args);
    WriteUpHype(pane);

    num_args = 0;
    XtSetArg(args[num_args], XmNscrollingPolicy, XmAUTOMATIC); num_args++;
    XtSetArg(args[num_args], XmNheight, 500); num_args++;
    sw = XtCreateManagedWidget("pane", xmScrolledWindowWidgetClass,
				 pane, args, num_args);

    BuildHierarchy(sw, xmOutlineWidgetClass);
    MakeControlPanel(pane);

    InitializePanel(pane);

    }

    XtRealizeWidget(top);

	/*	Process events, unwrapping correctly.  */
	while (!done)
	{
		XEvent event;
		XtAppNextEvent(app_con, &event);
        	XtDispatchEvent(&event);
	}
	XtDestroyWidget(top);
	XtDestroyApplicationContext(app_con);
	exit(0);
}

/**************************************************************
 *		STATIC FUNCTIONS
 **************************************************************/
/*
 * Function Name: BuildHierarchy
 * Description:
 * Arguments:
 * Returns:       Nothing
 *
 */
static void
BuildHierarchy(Widget parent, WidgetClass class)
{
    Widget outline, hierarchy, w1, w2, w3;

    G_outline = outline = XtCreateManagedWidget("outline_widget", class,
					  parent, NULL, (Cardinal) 0);

    w1 = CreateNode(outline, NULL, "Widgets", XmOpen);

    w2 = CreateNode(outline, w1, "Writing Widgets", XmOpen);
    {
	(void) CreateNode(outline, w2, "Widget Data Structures", XmAlwaysOpen);
	(void) CreateNode(outline, w2, "Structure Naming Conventions", XmAlwaysOpen);
	w3 = CreateNode(outline, w2, "Writing Header Files", XmOpen);
    }

    (void) CreateNode(outline, w3, "Writing Private Header Files", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "Writing Public Header Files", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "Internal Header Files", XmAlwaysOpen);

    w3 = CreateNode(outline, w2, "The Base Widget Classes", XmOpen);
    (void) CreateNode(outline, w3, "The Core Class Structure", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "The Core Instance Structure", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "The Composite Class Structure", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "The Composite Instance Structure", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "The Constraint Class Structure", XmAlwaysOpen);
    (void) CreateNode(outline, w3, "The Constraint Instance Structure", XmAlwaysOpen);
}

static void NewChildCB(Widget w, XtPointer client, XtPointer call)
{
	static int count = 0;
	char buffer[30];
	sprintf (buffer, "New Child %d", count++);
	CreateNode((Widget)client, w, buffer, XmAlwaysOpen);
}

/*
 * Function Name: CreateNode
 * Description:
 * Arguments:
 * Returns:       Widget
 *
 */
static Widget
CreateNode(Widget w_parent, Widget parent_node, char * name,
	   XmHierarchyNodeState state)
{
    Arg args[10];
    Cardinal num_args;
    Widget w;
    XmString xmstring;

    xmstring = XmStringCreateLocalized(name);

    num_args = 0;
    XtSetArg(args[num_args], XmNlabelString, xmstring); num_args++;
    XtSetArg(args[num_args], XmNnodeState, state); num_args++;
    XtSetArg(args[num_args], XmNparentNode, parent_node); num_args++;

    w = XtCreateManagedWidget(name, xmPushButtonWidgetClass,
			      w_parent, args, num_args);

    XtAddCallback(w,XmNactivateCallback,NewChildCB,w_parent);

    XmStringFree(xmstring);
    return(w);
}

/*
 * Function Name: WriteUpHype
 * Description:   Prints up the "Hype" message in a label
 * Arguments:     the usual suspects
 * Returns:       nothing
 */
static
void WriteUpHype(Widget parent)
{
    Arg args[5];
    Cardinal argcnt;
    Widget w;
    XmString xmstring;

    xmstring = XmStringLtoRCreate(
"The Motif Outline Widget displays hierarchical data in an outline layout with a Motif\n\
look and feel. The Outline widget displayed below has several Motif PushButtons (the\n\
Outline can accept any type of widget); press one to add a new child.\n\
\n\
The Outline Widget uses \"constraints\" to arrange the children with the right layout.\n\
To see the code to generate the tree below, press \"Show Layout Code...\".\n\
\n\
The toggles below set different resources on the Outline widget itself.\n\
\n\
Press \"Other Resources...\" for more information on the various resources.",
				  XmSTRING_DEFAULT_CHARSET);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNmarginHeight, 10); argcnt++;
    XtSetArg(args[argcnt], XmNmarginWidth, 10); argcnt++;
    XtSetArg(args[argcnt], XmNalignment, XmALIGNMENT_BEGINNING); argcnt++;
    XtSetArg(args[argcnt], XmNlabelString, xmstring); argcnt++;
    w = XtCreateManagedWidget("hypelabel", xmLabelWidgetClass,
			      parent, args, argcnt);

    XmStringFree(xmstring);

}

/*
 * Function Name: ShowCB
 * Description:
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void ShowCB(Widget w, XtPointer client, XtPointer call)
{
    static Widget info = NULL;

    if (info == NULL) {
	Arg args[5];
	Cardinal argcnt;
	Widget temp;
	XmString ok_xmstring;

	ok_xmstring = XmStringCreateLocalized("OK");

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Show Code..."); argcnt++;
	/* XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++; */
	XtSetArg(args[argcnt], XmNokLabelString, ok_xmstring); argcnt++;
	XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
	info = XmCreateTemplateDialog(G_outline, "show_code", args, argcnt);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNcolumns, 60); argcnt++;
	XtSetArg(args[argcnt], XmNrows, 20); argcnt++;
	XtSetArg(args[argcnt], XmNvalue, CODE); argcnt++;
	XtSetArg(args[argcnt], XmNeditable, False); argcnt++;
	XtSetArg(args[argcnt], XmNeditMode, XmMULTI_LINE_EDIT); argcnt++;
	temp = XmCreateScrolledText(info, "show_text", args, argcnt);
	XtManageChild(temp);

	XmStringFree(ok_xmstring);
    }

    XtManageChild(info);
}

/*
 * Function Name: ExplainCB
 * Description:
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void ExplainCB(Widget w, XtPointer client, XtPointer call)
{
    static Widget info = NULL;

    if (info == NULL) {
	Arg args[5];
	Cardinal argcnt;
	Widget temp;
	XmString xmstring;

	xmstring = XmStringLtoRCreate(
"The Motif Tree and Outline widget actually derive behavior from the\n\
Hierarchy Widget. The Hierarchy widget provides resources that specify\n\
the relationships between children.\n\
\n\
XmNautoClose specifies whether the Tree or Outline automatically closes\n\
any nodes if a parent node is closed. To see the behavior, press the Auto\n\
Close toggle, then close and reopen the Core.\n\
\n\
XmNnodeState is a constraint resource on a child of the Tree or Outline.\n\
It determines whether a node is Open, Closed, Hidden, or Always Visible.\n\
Use \"Show Layout Code...\" to see how this demo used this resource.\n\
\n\
XmNparentNode specifies this node's parent. This resource determines the\n\
node's logical parent (the widget instance tree says the node is a child of\n\
the Tree or Outline, but the parentNode will specify where on the layout\n\
the node will appear. Use \"Show Layout Code...\" to see how this demo used\n\
this resource.\n\
\n\
XmNnodeStateCallback provides a way to receive feedback when any node is\n\
opened or closed. To use this callback, press the Node State Callback toggle.",
				      XmSTRING_DEFAULT_CHARSET);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
	XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
	info = XmCreateInformationDialog(G_outline, "explain", args, argcnt);
	XtUnmanageChild(XtNameToWidget(info, "Cancel"));
	XtUnmanageChild(XtNameToWidget(info, "Help"));
	XmStringFree(xmstring);
    }

    XtManageChild(info);
}

/*
 * Function Name: NodeStateCB
 * Description:
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void NodeStateCB(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    Widget info, temp;
    XmHierarchyNodeStateData *node_data = (XmHierarchyNodeStateData *) call;
    XmString xmstring;
    char *name;
    char buf[100];

    argcnt = 0;
    XtSetArg(args[argcnt], XmNlabelString, &xmstring); argcnt++;
    XtGetValues(node_data->widget, args, argcnt);

    name = XmStringUnparse(xmstring, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (node_data->state == XmOpen)
	sprintf(buf, "%s has switched to the XmOpen state.", name);
    else if (node_data->state == XmClosed)
	sprintf(buf, "%s has switched to the XmClosed state.", name);
    else if (node_data->state == XmAlwaysOpen)
	sprintf(buf, "%s has switched to the XmAlwaysOpen state.", name);
    else if (node_data->state == XmHidden)
	sprintf(buf, "%s has switched to the XmHidden state.", name);
    else
	sprintf(buf, "%s has switched node state.", name);

    xmstring = XmStringCreateLocalized(buf);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Node State Changed"); argcnt++;
    XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
    info = XmCreateInformationDialog(w, "nodechange", args, argcnt);
	XtUnmanageChild(XtNameToWidget(info, "Cancel"));
	XtUnmanageChild(XtNameToWidget(info, "Help"));
    XmStringFree(xmstring);
    XtFree(name);
    XtManageChild(info);
}

/*
 * Function Name: CallbackTogCB
 * Description:
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void CallbackTogCB(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    Widget lab = (Widget) client;
    XmString xmstring;

    if (XmToggleButtonGetState(w)) {
	XtAddCallback(G_outline, XmNnodeStateCallback, NodeStateCB, NULL);
	xmstring = XmStringCreateLocalized("Added");
    } else {
	XtRemoveAllCallbacks(G_outline, XmNnodeStateCallback);
	xmstring = XmStringCreateLocalized("Not Added");
    }

    argcnt = 0;
    XtSetArg(args[argcnt], XmNlabelString, xmstring); argcnt++;
    XtSetValues(lab, args, argcnt);
    XmStringFree(xmstring);
}

/*
 * Function Name: AutoCloseTogCB
 * Description:
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void AutoCloseTogCB(Widget w, XtPointer client, XtPointer call)
{
  XtVaSetValues(G_outline, XmNautoClose, XmToggleButtonGetState(w), NULL);
}

static void ConnectTogCB(Widget w, XtPointer client, XtPointer call)
{
  XtVaSetValues(G_outline, XmNconnectNodes, XmToggleButtonGetState(w), NULL);
}


/*
 * Function Name: QuitCB
 * Description:   Exits the program
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void
QuitCB(Widget w, XtPointer client, XtPointer call)
{
	done = 1;
}

/*
 * Function Name: MakeControlPanel
 * Description:   Prints up the "Hype" message in a label
 * Arguments:     the usual suspects
 * Returns:       nothing
 */
static
void MakeControlPanel(Widget parent)
{
    Arg args[5];
    Cardinal argcnt;
    Widget big_rc, rc_1, rc_2, show_pb, explain_pb, ladder_tog, ladder_lab,
    callback_tog, callback_lab, quit_pb, autoclose_tog, sep, connect_tog;

    /* Big Vertical Row Column for the control panel */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    XtSetArg(args[argcnt], XmNpacking, XmPACK_TIGHT); argcnt++;
    big_rc = XtCreateManagedWidget("big_rc", xmRowColumnWidgetClass,
				   parent, args, argcnt);

    /* First Row */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNpacking, XmPACK_TIGHT); argcnt++;
    rc_1 = XtCreateManagedWidget("rc_1", xmRowColumnWidgetClass,
				 big_rc, args, argcnt);

    argcnt = 0;
    callback_tog = XtCreateManagedWidget("callback_tog",
					 xmToggleButtonWidgetClass,
					 rc_1, args, argcnt);
    argcnt = 0;
    callback_lab = XtCreateManagedWidget("callback_lab", xmLabelWidgetClass,
					 rc_1, args, argcnt);
    XtAddCallback(callback_tog, XmNvalueChangedCallback,
		  CallbackTogCB, (XtPointer) callback_lab);

    argcnt = 0;
    sep = XtCreateManagedWidget("sep_two", xmSeparatorWidgetClass,
				rc_1, args, argcnt);

    argcnt = 0;
    autoclose_tog = XtCreateManagedWidget("autoclose_tog",
					  xmToggleButtonWidgetClass,
					  rc_1, args, argcnt);
    XtAddCallback(autoclose_tog, XmNvalueChangedCallback,
		  AutoCloseTogCB, NULL);

    argcnt = 0;
    sep = XtCreateManagedWidget("sep_two", xmSeparatorWidgetClass,
				rc_1, args, argcnt);

    argcnt = 0;
    connect_tog = XtCreateManagedWidget("connect_tog",
					  xmToggleButtonWidgetClass,
					  rc_1, args, argcnt);
    XtAddCallback(connect_tog, XmNvalueChangedCallback,
		  ConnectTogCB, NULL);


    /* Third Row */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNpacking, XmPACK_TIGHT); argcnt++;
    rc_2 = XtCreateManagedWidget("rc_2", xmRowColumnWidgetClass,
				 big_rc, args, argcnt);

    argcnt = 0;
    show_pb = XtCreateManagedWidget("show_pb", xmPushButtonWidgetClass,
				    rc_2, args, argcnt);
    XtAddCallback(show_pb, XmNactivateCallback, ShowCB, NULL);

    argcnt = 0;
    explain_pb = XtCreateManagedWidget("explain_pb", xmPushButtonWidgetClass,
				       rc_2, args, argcnt);
    XtAddCallback(explain_pb, XmNactivateCallback, ExplainCB, NULL);

    argcnt = 0;
    quit_pb = XtCreateManagedWidget("quit_pb", xmPushButtonWidgetClass,
				    rc_2, args, argcnt);

    XtAddCallback(quit_pb, XmNactivateCallback, QuitCB, NULL);
}
