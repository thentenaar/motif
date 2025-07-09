/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * paned.c
 *
 */

/**************************************************************
 *		INCLUDE FILES
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/Paned.h>
#include <Xm/ToggleB.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/**************************************************************
 *		GLOBALS
 **************************************************************/
Widget G_paned = NULL;
Widget G_form = NULL;

/**************************************************************
 *		FORWARD DECLARATIONS
 **************************************************************/
Widget Createform(Widget);
Widget Createpaned(Widget);

/**************************************************************
 *		DEFINES
 **************************************************************/

/**************************************************************
 *		FALLBACKS
 **************************************************************/
static String fallbacks[] = {
    /*
     * General fallback resources.
     */
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*hype_label*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*orient_label.labelString: Orientation:",
    "*horiz_tog.labelString: XmHORIZONTAL",
    "*vert_tog.labelString: XmVERTICAL",
    "*sep_label.labelString: Show Separator:",
    "*sep_on_tog.labelString: True",
    "*sep_off_tog.labelString: False",
    "*other_res.labelString: Other Resources...",
    "*cons_res.labelString: Constraint Resources...",
    "*quit_pb.labelString: Quit",
    "*XmPaned*XmForm*XmLabel.labelString: Motif Paned Widget",
    "*XmPaned*XmForm*XmToggleButton.labelString: Show Sash",
    "*XmPaned*XmForm*sash_three_tog.labelString: Pane Demo",
    
    NULL,
};

/**************************************************************
 *		PUBLIC (GLOBAL) CODE
 **************************************************************/

/*
 * Function Name: InitControls
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
InitControls()
{
    Arg args[5];
    Cardinal argcnt;
    Widget w;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    XtSetArg(args[argcnt], XmNseparatorOn, True); argcnt++;
    XtSetValues(G_paned, args, argcnt);

    w = XtNameToWidget(G_form, "*vert_tog");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find vert_tog");
	exit(1);
    }
    XmToggleButtonSetState(w, True, False);

    w = XtNameToWidget(G_form, "*sep_on_tog");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find sep_on_tog");
	exit(1);
    }
    XmToggleButtonSetState(w, True, False);

    w = XtNameToWidget(G_paned, "*pane_form_zero");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find pane_form_zero");
	exit(1);
    }
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, True); argcnt++;
    XtSetValues(w, args, argcnt);

    w = XtNameToWidget(G_paned, "*sash_one_tog");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find sash_one_tog");
	exit(1);
    }
    XmToggleButtonSetState(w, True, False);

    w = XtNameToWidget(G_paned, "*pane_form_one");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find pane_form_one");
	exit(1);
    }
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, True); argcnt++;
    XtSetValues(w, args, argcnt);

    w = XtNameToWidget(G_paned, "*sash_two_tog");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find sash_two_tog");
	exit(1);
    }
    XmToggleButtonSetState(w, True, False);

    w = XtNameToWidget(G_paned, "*pane_form_three");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find pane_form_three");
	exit(1);
    }
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, True); argcnt++;
    XtSetValues(w, args, argcnt);

    w = XtNameToWidget(G_paned, "*sash_three_tog");
    if (w == NULL) {
	fprintf(stderr, "InitControls: Unable to find sash_three_tog");
	exit(1);
    }
    XmToggleButtonSetState(w, True, False);

}

/*
 * Function Name: SashValChCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
SashValChCB(Widget w, XtPointer client, XtPointer call)
{
    /* 
     * BE CAREFUL...ASSUMES THIS WIDGET IS CHILD OF FORM, WHICH IS CHILD
     * OF A XI PANED
     */
    Arg args[5];
    Cardinal argcnt;
    Widget parent = XtParent(w);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, XmToggleButtonGetState(w)); argcnt++;
    XtSetValues(parent, args, argcnt);
}
/*
 * Function Name: SepValChCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
SepValChCB(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    Boolean val = !!client;

    if (!XmToggleButtonGetState(w))
	return;
    
    argcnt = 0;
    XtSetArg(args[argcnt], XmNseparatorOn, val); argcnt++;
    XtSetValues(G_paned, args, argcnt);
}

/*
 * Function Name: OrientChValCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
OrientChValCB(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    unsigned char orient = (uintptr_t)client & 0xff;
    
    if (!XmToggleButtonGetState(w))
	return;
    
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, orient); argcnt++;
    XtSetValues(G_paned, args, argcnt);
}

/*
 * Function Name: ConstraintResCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
ConstraintResCB(Widget w, XtPointer client, XtPointer call)
{
    static Widget info = NULL;

    if (info == NULL) {
	Arg args[5];
	Cardinal argcnt;
	Widget temp;
	XmString xmstring;

	xmstring = XmStringCreateLtoR(
"The Motif Paned Widget adds \"constraint\" widgets to any children that are\n\
inside it. Here's the list of available constraint resources:\n\
\n\
     Resource (Type)\n\
     allowResize (Boolean) - Allows/Disallows children geometry changes\n\
     paneMaximum, paneMinimum (Dimension)\n\
     preferredPaneSize (Dimension)\n\
     resizeToPreferred (Boolean)\n\
     showSash (Boolean) - To see behavior, press \"Show Sash\" toggle\n\
     skipAdjust (Boolean)",
				      XmSTRING_DEFAULT_CHARSET);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
	XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
	info = XmCreateInformationDialog(w, "explain", args, argcnt);

	temp = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(temp);
	temp = XmMessageBoxGetChild(info, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(temp);
	XmStringFree(xmstring);
    }
    XtManageChild(info);
}

/*
 * Function Name: OtherResCB
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
OtherResCB(Widget w, XtPointer client, XtPointer call)
{
    static Widget info = NULL;

    if (info == NULL) {
	Arg args[5];
	Cardinal argcnt;
	Widget temp;
	XmString xmstring;

	xmstring = XmStringCreateLtoR(
"The Motif Paned widget provides other resources. Here's a partial\n\
list of available resources:\n\
\n\
     Resource (Type)\n\
     cursor (Cursor) - Cursor to use over Motif Paned\n\
     marginHeight, marginWidth (Dimension)\n\
     sashHeight (Position)\n\
     sashShawdowThickness (Dimension)",
			      XmSTRING_DEFAULT_CHARSET);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
	XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
	info = XmCreateInformationDialog(w, "explain", args, argcnt);

	temp = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(temp);
	temp = XmMessageBoxGetChild(info, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(temp);
	XmStringFree(xmstring);
    }

    XtManageChild(info);
}


/*
 * Function Name: CreateLabel
 * Description:   
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
CreateLabel(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    XmString xmstring;
    
    xmstring = XmStringCreateLtoR(
"The Motif Paned Widget extends the capabilities of the standard Paned\n\
Window that comes with Motif.\n\
\n\
The Motif Paned Widget offers horizontal as well as vertical orientation,\n\
(the Motif Paned widget only offers a vertical orientation). To see the\n\
two forms of orienation, press the XmHORIZONTAL or XmVERTICAL toggles.\n\
\n\
The Motif Paned Widget can be used as a \"smarter\" Row Column widget, by\n\
turning off the separators between panes. Press the True or False toggles\n\
to see this behavior.",
				  XmSTRING_DEFAULT_CHARSET);
     
    argcnt = 0;
    XtSetArg(args[argcnt], XmNmarginHeight, 10); argcnt++;
    XtSetArg(args[argcnt], XmNmarginWidth, 10); argcnt++;
    XtSetArg(args[argcnt], XmNalignment, XmALIGNMENT_BEGINNING); argcnt++;
    XtSetArg(args[argcnt], XmNlabelString, xmstring); argcnt++;
    XtSetValues(w, args, argcnt);
    
    XmStringFree(xmstring);

}

/*
 * Function Name: QuitCB
 * Description:   Exits the program
 * Arguments:     This is an XtCallback
 * Returns:       Nothing
 */
void
QuitCB(Widget w, XtPointer client, XtPointer call)
{
    exit(0);
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
    Arg args[5];
    Cardinal argcnt;
    Widget top, paned_top, w;
    XtAppContext app;
    
    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 

    argcnt = 0;
    XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
    XtSetArg(args[argcnt], XmNtitle, "Motif Paned Widget Demo"); argcnt++;
    top = XtOpenApplication(&app, "Paned", NULL, 0,
			    &argc, argv, fallbacks, 
                            sessionShellWidgetClass, 
                            args, argcnt);

    G_form = Createform(top);
    XtManageChild(G_form);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
    XtSetArg(args[argcnt], XmNtitle, "Motif Paned Widget Demo"); argcnt++;
    paned_top = XtAppCreateShell("paned_top", "Paned_Top",
				topLevelShellWidgetClass, XtDisplay(top),
				args, argcnt);

    G_paned = Createpaned(paned_top);
    XtManageChild(G_paned);

    InitControls();

    XtRealizeWidget(top);

    XtRealizeWidget(paned_top);

    XtAppMainLoop(app);
    return(0);
}

