/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * tree.c
 *
 */

/**************************************************************
 *		INCLUDE FILES
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>

#include <Xm/Tree.h>
#include <Xm/ButtonBox.h>
#include <Xm/Outline.h>
#include <Xm/Hierarchy.h>
#include <Xm/IconButton.h>
#include <Xm/Paned.h>

#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/ScrolledW.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>

#include <pixmaps/porsche.xpm>
#include <pixmaps/stopsign.xpm>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/**************************************************************
External functions
**************************************************************/
#ifdef NeedFunctionPrototypes
#define ARGLIST(p)	(
#define ARG(a, b)	a b,
#define GRA(a, b)	a b)
#else
#define ARGLIST(p)	p
#define ARG(a, b)	a b;
#define GRA(a, b)	a b;
#endif
extern Pixmap XPM_PIXMAP
ARGLIST((w, pixmapName))
ARG(Widget, w)
GRA(char **, pixmapName);


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
static void OrientationCB(Widget, XtPointer, XtPointer);
static void ConnectStyleCB(Widget, XtPointer, XtPointer);
static void CompressStyleCB(Widget, XtPointer, XtPointer);
static void ScaleCB(Widget, XtPointer, XtPointer);

/**************************************************************
 *		GLOBALS
 **************************************************************/
static Widget G_tree = NULL;
static Pixmap close_folder, open_folder;
static Widget core, composite, constraint, primitive, manager;
static char* resources[]= {XmNhorizontalNodeSpace,XmNhorizontalDelta,XmNverticalNodeSpace,XmNverticalDelta};

/**************************************************************
 *		SHOW CODE
 **************************************************************/
#define CODE "static void\n\
BuildHierarchy(Widget parent)\n\
{\n\
    Widget tree, hierarchy;\n\
\n\
    tree = XtCreateManagedWidget(\"hierarchy\", xiTreeWidgetClass, \n\
                                 parent, NULL, (Cardinal) 0);\n\
\n\
    core = CreateNode(tree, NULL, \"Core\", XmOpen);\n\
\n\
    primitive = CreateNode(tree, core, \"Primitive\", XmOpen);\n\
    {\n\
        (void) CreateNode(tree, primitive, \"Icon Button\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, primitive, \"Data Field\", XmAlwaysOpen);\n\
    }\n\
\n\
    composite = CreateNode(tree, core, \"Composite\", XmOpen);\n\
    constraint = CreateNode(tree, composite, \"Constraint\", XmOpen);\n\
\n\
    manager = CreateNode(tree, constraint, \"Manager\", XmOpen);\n\
    {\n\
        (void) CreateNode(tree, manager, \"Button Box\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Color Selector\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Column\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Combination Box\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Dialog Box\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Extended List\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Font Selector\", XmAlwaysOpen);\n\
        hierarchy = CreateNode(tree, manager, \"Hierarchy\", XmOpen);\n\
        {\n\
            (void) CreateNode(tree, hierarchy, \"Tree\", XmAlwaysOpen);\n\
            (void) CreateNode(tree, hierarchy, \"Outline\", XmAlwaysOpen);\n\
        }\n\
        (void) CreateNode(tree, manager, \"Icon Box\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Pixmap Editor\", XmAlwaysOpen);\n\
        (void) CreateNode(tree, manager, \"Stretch Widget\", XmAlwaysOpen);\n\
    }\n\
}\n\
\n\
static Widget\n\
CreateNode(Widget w_parent, Widget parent_node, char * name, \n\
           XmHierarchyNodeState state)\n\
{\n\
    Arg args[10];\n\
    Cardinal num_args;\n\
    Widget w;\n\
    XmString xmstring;\n\
\n\
    xmstring = XmStringCreateSimple(name);\n\
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
\n\
    return(w);\n\
}"

/**************************************************************
 *		RESOURCE FALLBACKS
 **************************************************************/
static String fallbacks[] = {
    "*background: grey",
    "*treeWidget.background: white",
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*hypelabel*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*show_code*fontList: -*-courier-*-r-normal-*-*-*-*-*-*-*-*-*",
    "*show_pb.labelString: Show Layout Code...",
    "*explain_pb.labelString: Other Resources...", 
    "*ladder_lab.labelString: Connect Style",
    "*ladder.labelString: XmTreeLadder",
    "*direct.labelString: XmTreeDirect",
    "*compress_lab.labelString: Compress Style",
    "*leaves.labelString: XmTreeCompressLeaves",
    "*none.labelString: XmTreeCompressNone",
    "*all.labelString: XmTreeCompressAll",
    "*callback_tog.labelString: Node State Callback:",
    "*callback_lab.labelString: Not Added",
    "*callback_lab.background: wheat",
    "*changePixTog.labelString: Folder Pixmaps:",
    "*changePixLab.labelString: Default",
    "*changePixLab.background: wheat",
    "*autoclose_tog.labelString: Auto Close",
    "*orientation.labelString: Orientation",
    "*horizontal.labelString: XmHORIZONTAL",
    "*vertical.labelString: XmVERTICAL",
    "*quit_pb.labelString: Quit",
    "*XmLabel.alignment: XmALIGNMENT_BEGINNING",
    "*XmToggleButton*alignment: XmALIGNMENT_BEGINNING",
    "*treeWidget*XmPushButton.background: Wheat",
    "*horizontalNodeSpace.labelString: Horizontal Node Space",
    "*horizontalDelta.labelString: Horizontal Delta",
    "*verticalNodeSpace.labelString: Vertical Node Space",
    "*verticalDelta.labelString: Vertical Delta",
    NULL,
};


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
    Widget top, big_pane;
    Arg args[10];
    Cardinal num_args;
    XtAppContext app_con;
    Widget scrolled, right_pane;
    
    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 

    num_args = 0;
    XtSetArg(args[num_args], XmNtitle, "Tree Demo"); num_args++;
    XtSetArg(args[num_args], XmNallowShellResize, True); num_args++;
    top = XtOpenApplication(
            &app_con, 
            "Treedemo", 
            NULL, 
            0, 
            &argc, 
            argv,
            fallbacks, 
            sessionShellWidgetClass, 
            args, 
            num_args);

    num_args = 0;
    big_pane = XtVaCreateManagedWidget("bigPane", xmPanedWidgetClass, 
				   top, 
				   XmNorientation, XmHORIZONTAL,
				   NULL); 

#if 0
    scrolled = XtVaCreateManagedWidget( "scrollTree", 
				       xmScrolledWindowWidgetClass,
				       big_pane, 
				       XmNresizeToPreferred, True,
				       XmNpreferredPaneSize, 500,
				       XmNallowResize, True,
				       XmNscrollingPolicy, XmAUTOMATIC,
				       XmNshowSash, True,
				       XmNscrollBarDisplayPolicy, 
				       XmAS_NEEDED,
				       NULL );
#else
	scrolled=big_pane;
#endif

    BuildHierarchy(scrolled, xmTreeWidgetClass);

    /* right pane */
    right_pane = XtVaCreateManagedWidget("rightPane", xmPanedWidgetClass,
					 big_pane, 
					 XmNskipAdjust, False,
					 XmNorientation, XmVERTICAL,
					 NULL );
    

    scrolled = XtVaCreateManagedWidget( "scrollMessage", 
				       xmScrolledWindowWidgetClass,
				       right_pane, 
				       XmNscrollingPolicy, XmAUTOMATIC,
				       XmNscrollBarDisplayPolicy, 
				       XmAS_NEEDED,
				       XmNpaneMaximum, 200,
				       XmNallowResize, True,
				       NULL );
    WriteUpHype(scrolled);
    MakeControlPanel(right_pane);

    /* Create the pixmaps which will be used later */

    open_folder = XPM_PIXMAP( top, porsche );
    close_folder = XPM_PIXMAP( top, stopsign );

    XtRealizeWidget(top);
    XtAppMainLoop(app_con);
    return 0;
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
    Widget tree, hierarchy;

    G_tree = tree = XtCreateManagedWidget("treeWidget", class, 
					  parent, NULL, (Cardinal) 0);
    XtVaSetValues(G_tree, XmNunitType, XmPIXELS, NULL);

    core = CreateNode(tree, NULL, "Core", XmOpen);

    primitive = CreateNode(tree, core, "Primitive", XmOpen);
    {
	(void) CreateNode(tree, primitive, "Icon Button", XmAlwaysOpen);
	(void) CreateNode(tree, primitive, "Data Field", XmAlwaysOpen);
    }

    composite = CreateNode(tree, core, "Composite", XmOpen);
    constraint = CreateNode(tree, composite, "Constraint", XmOpen);

    manager = CreateNode(tree, constraint, "Manager", XmOpen);
    {
	(void) CreateNode(tree, manager, "Button Box", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Color Selector", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Column", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Combination Box", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Dialog Box", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Extended List", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Font Selector", XmAlwaysOpen);
	hierarchy = CreateNode(tree, manager, "Hierarchy", XmOpen);
        {
	    (void) CreateNode(tree, hierarchy, "Tree", XmAlwaysOpen);
	    (void) CreateNode(tree, hierarchy, "Outline", XmAlwaysOpen);
	}
	(void) CreateNode(tree, manager, "Icon Box", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Pixmap Editor", XmAlwaysOpen);
	(void) CreateNode(tree, manager, "Stretch Widget", XmAlwaysOpen);
    }
}

static void NewChildCB(Widget w, XtPointer client, XtPointer call)
{
	CreateNode((Widget)client, w, "New Child", XmAlwaysOpen);
}
static void getNodesCB(Widget w, XtPointer client, XtPointer call)
{
	WidgetList list = XmHierarchyGetChildNodes(w);
	int i;
	
	if (!list)
		printf ("Widget '%s' has no node children.\n",XtName(w));
	else
	for (i=0; list[i]; i++)
	{
		printf ("Widget '%s' has a node child '%s'.\n",XtName(w),
			XtName(list[i]));
	}
	XtFree((char*)list);

	{

#if 1
#define wingdogs_width 32
#define wingdogs_height 32
static unsigned char wingdogs_bits[] = {
   0x60, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00,
   0x20, 0x0f, 0x00, 0x00, 0x40, 0x3e, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x80,
   0x84, 0xff, 0x00, 0xc0, 0x86, 0xff, 0x00, 0xc0, 0x07, 0xff, 0x21, 0xe0,
   0x0f, 0xfe, 0x23, 0xf0, 0x0f, 0xfe, 0x23, 0xfc, 0x1d, 0xfe, 0x13, 0xfe,
   0x39, 0xfc, 0x13, 0xff, 0x3f, 0xfc, 0x83, 0xff, 0x9f, 0xfc, 0xc1, 0xff,
   0x0f, 0xfe, 0xe0, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff,
   0xfc, 0xff, 0xff, 0x7f, 0xf6, 0xff, 0xff, 0x1f, 0xfb, 0xff, 0xff, 0x07,
   0xf8, 0xfd, 0xff, 0x03, 0xbc, 0xf9, 0xff, 0x01, 0x3c, 0xf9, 0xff, 0x01,
   0x3e, 0xf0, 0xf7, 0x00, 0x1f, 0xe0, 0x77, 0x00, 0x1f, 0x80, 0x77, 0x00,
   0x8f, 0x00, 0x6f, 0x00, 0xc7, 0x00, 0x6e, 0x80, 0x07, 0x00, 0x7c, 0x80,
   0x0d, 0x00, 0xf8, 0x80, 0x1f, 0x00, 0xf0, 0x01};
	    Window root = RootWindowOfScreen(XtScreen(w));
	Pixmap pixmap = 
		XCreateBitmapFromData(XtDisplay(w),root, (char *)wingdogs_bits,
                            wingdogs_width, wingdogs_height);
	XtVaSetValues(w,XmNnodeCloseFolderPixmap, pixmap, NULL);
#endif
	}
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

    xmstring = XmStringCreateSimple(name);
    
    num_args = 0;
    XtSetArg(args[num_args], XmNlabelString, xmstring); num_args++;
    XtSetArg(args[num_args], XmNnodeState, state); num_args++;
    XtSetArg(args[num_args], XmNparentNode, parent_node); num_args++;
    XtSetArg(args[num_args], XmNlineStyle, 99); num_args++;
    
    w = XtCreateManagedWidget(name, xmPushButtonWidgetClass,
			      w_parent, args, num_args);

    XtAddCallback(w, XmNactivateCallback, NewChildCB, w_parent);

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
    
    xmstring = XmStringCreateLtoR(
"The Motif Tree Widget displays hierarchical data in a tree layout with a Motif\n\
look and feel. The Tree widget displayed below contains entries corresponding\n\
to Motif widgets. The nodes are XmPushButtons, but the tree can accept any type of widget.\n\
\n\
The Tree Widget uses \"constraints\" to arrange the children with the right layout.\n\
To see the code to generate the tree below, press \"Show Layout Code...\".\n\
\n\
There are two Connect Styles, XmTreeDirect and XmTreeLadder. To see the\n\
difference in the way the tree is displayed in each of these modes, \n\
select the Connect Style option.\n\
\n\
There are two orientations, XmHORIZONTAL and XmVERTICAL. To see the\n\
difference in the way the tree is displayed in each of these modes, \n\
select the Orientation option.\n\
\n\
There are three compression styles, XmTreeCompressLeaves, XmTreeCompressAll \n\
and XmTreeCompressNone. To see the difference in the way the tree is displayed in each of these modes, \n\
set the orientation to XmVERTICAL and select the Compress Style option.\n\
\n\
A developer may add a Node State Callback, which gets fired off each\n\
time a node changes state. Set the Node State Callback toggle to \n\
selected. This will add the callback to the tree. Now open or close any \n\
node in the tree and see what happens. It can be annoying, so don't leave\n\
it on !\n\
\n\
Color pixmaps may be used instead of the Default images that look like\n\
little folders. Select the Folder Pixmaps toggle to see how the tree\n\
might look with user defined pixmaps.\n\
\n\
Folders may be forced to close all their descendents folder buttons when\n\
they become closed. Set the Auto Close toggle button to selected.\n\
Close one of the nodes close to the root. When you open it again, you \n\
will see that all its children are closed.\n\
\n\
If you press on a node itself, you will see a list of its node children\n\
echoed to standard output.\n\
\n\
Press \"Other Resources...\" to see the resources that manipulate the Tree's look and feel.",
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

	ok_xmstring = XmStringCreateSimple("OK");

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Show Code..."); argcnt++;
	/* XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++; */
	XtSetArg(args[argcnt], XmNokLabelString, ok_xmstring); argcnt++;
	XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
	info = XmCreateTemplateDialog(G_tree, "show_code", args, argcnt);

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

	xmstring = XmStringCreateLtoR(
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
XmNlineColor allows the programmer to specify the color of the lines that\n\
connect the nodes.\n\
\n\
XmNnodeStateCallback provides a way to receive feedback when any node is\n\
opened or closed. To use this callback, press the Node State Callback toggle.",
				      XmSTRING_DEFAULT_CHARSET);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
	XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
	info = XmCreateInformationDialog(G_tree, "explain", args, argcnt);

	temp = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(temp);
	temp = XmMessageBoxGetChild(info, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(temp);
	XmStringFree(xmstring);
    }

    XtManageChild(info);
}

/*
 * Function Name: ConnectStyleCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void ConnectStyleCB(Widget w, XtPointer client, XtPointer call)
{
	XtPointer userData;
	XtVaGetValues(w,XmNuserData,&userData,NULL);
	XtVaSetValues(G_tree,XmNconnectStyle,userData,NULL);
}

/*
 * Function Name: CompressStyleCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void CompressStyleCB(Widget w, XtPointer client, XtPointer call)
{
	XtPointer userData;
	XtVaGetValues(w,XmNuserData,&userData,NULL);
	XtVaSetValues(G_tree,XmNcompressStyle,userData,NULL);
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

    XmStringGetLtoR(xmstring, XmFONTLIST_DEFAULT_TAG, &name);

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

    xmstring = XmStringCreateSimple(buf);
    
    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Node State Changed"); argcnt++;
    XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
    info = XmCreateInformationDialog(w, "nodechange", args, argcnt);
    
    temp = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild(temp);
    temp = XmMessageBoxGetChild(info, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(temp);

    XmStringFree(xmstring);
    XtFree(name);

    XtManageChild(info);
}

static void ChangePixmap(Widget w, XtPointer client, XtPointer call)
{
  Widget label = (Widget)client;

  static Boolean TreeUsingPixmaps = False;

  if (TreeUsingPixmaps){
    XtVaSetValues( G_tree, XmNopenFolderPixmap, XmUNSPECIFIED_PIXMAP,
		  XmNcloseFolderPixmap, XmUNSPECIFIED_PIXMAP, NULL );
    XtVaSetValues( label, XmNlabelString, XmStringCreateSimple("Default"),
		  NULL);
  }
  else {
    XtVaSetValues( G_tree, XmNopenFolderPixmap, open_folder,
		  XmNcloseFolderPixmap, close_folder, NULL );
    XtVaSetValues( label, XmNlabelString,
		  XmStringCreateSimple("User Defined"), NULL);
  }

  TreeUsingPixmaps = !TreeUsingPixmaps;

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
	XtAddCallback(G_tree, XmNnodeStateCallback, NodeStateCB, NULL);
	xmstring = XmStringCreateSimple("Added");
    } else {
	XtRemoveAllCallbacks(G_tree, XmNnodeStateCallback);
	xmstring = XmStringCreateSimple("Not Added");
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
    Arg args[5];
    Cardinal argcnt;

    argcnt = 0;
    if (XmToggleButtonGetState(w)) {
	XtSetArg(args[argcnt], XmNautoClose, True); argcnt++;
    } else {
	XtSetArg(args[argcnt], XmNautoClose, False); argcnt++;
    }
    XtSetValues(G_tree, args, argcnt);
}

/*
 * Function Name: OrientationCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void
OrientationCB(Widget w, XtPointer client, XtPointer call)
{
	XtPointer userData;
	XtVaGetValues(w,XmNuserData,&userData,NULL);
	XtVaSetValues(G_tree,XmNorientation,userData,NULL);
}


/*
 * Function Name: ScaleCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
static void
ScaleCB(Widget w, XtPointer client, XtPointer call)
{
	XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *)call;

	XtVaSetValues(G_tree, resources[(intptr_t)client], cbs->value, NULL);
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
    exit(0);
}


/*
 * Function Name: MakeControlPanel
 * Description:   
 * Arguments:     
 * Returns:       nothing
 */
static
void MakeControlPanel(Widget right_pane)
{
    Arg args[5];
    Cardinal argcnt;
    Widget tog, lab, pb, bbox, leftBbox, rightBbox;

	/* this is a clever use of the ButtonBox -- if two of them have to
	** fill the same vertical space and each has "equalSize" set, then
	** each row will be lined up with the item in the other box
	*/

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, True); argcnt++;
    bbox = XtCreateManagedWidget("buttonBox1",
				xmButtonBoxWidgetClass,
				right_pane, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    leftBbox = XtCreateManagedWidget("buttonBox1",
				xmButtonBoxWidgetClass,
				bbox, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    rightBbox = XtCreateManagedWidget("buttonBox1",
				xmButtonBoxWidgetClass,
				bbox, args, argcnt);

    XtVaCreateManagedWidget("ladder_lab", 
				  xmLabelWidgetClass,
				  leftBbox, NULL);


	{
	Widget option = XmCreateOptionMenu(rightBbox, "option", NULL,0);
	Widget pulldown = XmCreatePulldownMenu(option, "pulldown", NULL, 0);
	Widget direct, ladder;
	XmTreeConnectStyle connectStyle = XmTreeDirect;

	XtManageChild(direct = XmCreatePushButton(pulldown, "direct", NULL, 0));
	XtVaSetValues(direct, XmNuserData, XmTreeDirect, NULL);
	XtManageChild(ladder = XmCreatePushButton(pulldown, "ladder", NULL, 0));
	XtVaSetValues(ladder, XmNuserData, XmTreeLadder, NULL);
	XtAddCallback(direct,XmNactivateCallback, ConnectStyleCB, NULL);
	XtAddCallback(ladder,XmNactivateCallback, ConnectStyleCB, NULL);
	XtVaSetValues(option,XmNsubMenuId,pulldown,NULL);

	XtVaGetValues(G_tree, XmNconnectStyle, &connectStyle, NULL);
	if (XmTreeDirect == connectStyle)
		XtVaSetValues(option,XmNmenuHistory,direct,NULL);
	else
		XtVaSetValues(option,XmNmenuHistory,ladder,NULL);

	XtManageChild(option);
	}


    XtVaCreateManagedWidget("compress_lab", 
				  xmLabelWidgetClass,
				  leftBbox, NULL);


	{
	Widget option = XmCreateOptionMenu(rightBbox, "option", NULL,0);
	Widget pulldown = XmCreatePulldownMenu(option, "pulldown", NULL, 0);
	Widget leaves, all, none;
	XmTreeCompressStyle compressStyle = XmTreeCompressLeaves;

	XtManageChild(leaves = XmCreatePushButton(pulldown, "leaves", NULL, 0));
	XtVaSetValues(leaves, XmNuserData, XmTreeCompressLeaves, NULL);
	XtManageChild(all = XmCreatePushButton(pulldown, "all", NULL, 0));
	XtVaSetValues(all, XmNuserData, XmTreeCompressAll, NULL);
	XtManageChild(none = XmCreatePushButton(pulldown, "none", NULL, 0));
	XtVaSetValues(none, XmNuserData, XmTreeCompressNone, NULL);
	XtAddCallback(leaves,XmNactivateCallback, CompressStyleCB, NULL);
	XtAddCallback(all,XmNactivateCallback, CompressStyleCB, NULL);
	XtAddCallback(none,XmNactivateCallback, CompressStyleCB, NULL);
	XtVaSetValues(option,XmNsubMenuId,pulldown,NULL);

	XtVaGetValues(G_tree, XmNcompressStyle, &compressStyle, NULL);
	if (XmTreeCompressLeaves == compressStyle)
		XtVaSetValues(option,XmNmenuHistory,leaves,NULL);
	else if (XmTreeCompressAll == compressStyle)
		XtVaSetValues(option,XmNmenuHistory,all,NULL);
	else 
		XtVaSetValues(option,XmNmenuHistory,none,NULL);

	XtManageChild(option);
	}


    tog = XtVaCreateManagedWidget("callback_tog", 
				  xmToggleButtonWidgetClass,
				  leftBbox, NULL);
    
    lab = XtVaCreateManagedWidget("callback_lab",
					   xmLabelWidgetClass,
					   rightBbox, NULL);

    XtAddCallback(tog, XmNvalueChangedCallback, 
		  CallbackTogCB, (XtPointer) lab);


    tog = XtVaCreateManagedWidget("changePixTog", 
				  xmToggleButtonWidgetClass,
				  leftBbox, NULL);
    
    lab = XtVaCreateManagedWidget("changePixLab",
				  xmLabelWidgetClass,
				  rightBbox, NULL);
    
    XtAddCallback(tog, XmNvalueChangedCallback, ChangePixmap, 
		  (XtPointer) lab);


    tog = XtVaCreateManagedWidget("autoclose_tog", 
				  xmToggleButtonWidgetClass,
				  leftBbox, NULL);

	{
	Boolean set;
	XtVaGetValues(G_tree, XmNautoClose, &set, NULL);
    	XtVaSetValues(tog, XmNset, set, NULL);
	}

    XtAddCallback(tog, XmNvalueChangedCallback, 
		  AutoCloseTogCB, NULL);

    /* this is just to line things up nicely */ 
    (void)XtVaCreateManagedWidget("dummy", xmLabelWidgetClass,rightBbox,
				  XmNlabelType, XmPIXMAP, 
				  XmNlabelPixmap, XmUNSPECIFIED_PIXMAP, 
				  NULL);

    XtVaCreateManagedWidget("orientation", 
				  xmLabelWidgetClass,
				  leftBbox, NULL);

	{
	Widget option = XmCreateOptionMenu(rightBbox, "option", NULL,0);
	Widget pulldown = XmCreatePulldownMenu(option, "pulldown", NULL, 0);
	Widget horizontal, vertical;
	unsigned char orientation = XmHORIZONTAL;

	XtManageChild(horizontal = XmCreatePushButton(pulldown, "horizontal", NULL, 0));
	XtVaSetValues(horizontal, XmNuserData, XmHORIZONTAL, NULL);
	XtManageChild(vertical = XmCreatePushButton(pulldown, "vertical", NULL, 0));
	XtVaSetValues(vertical, XmNuserData, XmVERTICAL, NULL);
	XtAddCallback(horizontal,XmNactivateCallback, OrientationCB, NULL);
	XtAddCallback(vertical,XmNactivateCallback, OrientationCB, NULL);
	XtVaSetValues(option,XmNsubMenuId,pulldown,NULL);

	XtVaGetValues(G_tree, XmNorientation, &orientation, NULL);
	if (XmHORIZONTAL == orientation)
		XtVaSetValues(option,XmNmenuHistory,horizontal,NULL);
	else
		XtVaSetValues(option,XmNmenuHistory,vertical,NULL);

	XtManageChild(option);
	}

	{
	int i;
	for (i=0;i<XtNumber(resources);i++)
		{
		Arg args[8];
		int n = 0;
		Dimension val;
		Widget scale;

    		XtVaCreateManagedWidget(resources[i], xmLabelWidgetClass,
				  leftBbox, NULL);
		XtSetArg(args[n], XmNminimum, 1); n++;
		XtSetArg(args[n], XmNmaximum, 100); n++;
		XtSetArg(args[n], XmNshowValue, True); n++;
		XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
		XtVaGetValues(G_tree, resources[i], &val, NULL);
		XtSetArg(args[n], XmNvalue, val); n++;
		scale = XmCreateScale(rightBbox, resources[i], args, n);
		XtAddCallback(scale, XmNvalueChangedCallback, ScaleCB, (XtPointer)(intptr_t)i);
		XtManageChild(scale);
		}
	}


    /* Second Row */

    bbox = XtVaCreateManagedWidget("buttonBox1",
				   xmButtonBoxWidgetClass,
				   right_pane, 
				   XmNequalSize, True,
				   XmNfillOption, XmFillNone,
				   XmNorientation, XmHORIZONTAL,
				   XmNshowSash, False,
				   XmNskipAdjust, True,
				   NULL );

    pb = XtVaCreateManagedWidget("show_pb", xmPushButtonWidgetClass,
					    bbox, NULL);

    XtAddCallback(pb, XmNactivateCallback, ShowCB, NULL);

    pb = XtVaCreateManagedWidget("explain_pb",
				       xmPushButtonWidgetClass,
				       bbox, NULL);
    XtAddCallback(pb, XmNactivateCallback, ExplainCB, NULL);

    bbox = XtVaCreateManagedWidget("buttonBox1",
				   xmButtonBoxWidgetClass,
				   right_pane, 
				   XmNfillOption, XmFillNone,
				   XmNorientation, XmHORIZONTAL,
				   XmNshowSash, False,
				   XmNskipAdjust, True,
				   NULL );

    pb = XtVaCreateManagedWidget("quit_pb", xmPushButtonWidgetClass,
				      bbox, NULL );

    XtAddCallback(pb, XmNactivateCallback, QuitCB, NULL);
}
