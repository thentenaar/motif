/* $XConsortium: WMShellGeomM1.c /main/5 1995/07/15 21:06:55 drk $ */
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
/*
 *  file: WMShellGeomM1.c
 *
 *	This is a test to test out the variuos WMShell geometry related
 *	resources. Incodentally, it also shows a bug in RowColumn PIR 3261,
 *	about RowColumn not resizing its children to fit into its new size.
 *
 *	It also shows a bug in DialogShell PIR 3262. This is about not being
 *	able to set the various WMShell resources in the DialogShell through
 *	SetValues.
 *
 *  It also shows a bug apparently in scale PIR 3397. This is about the
 *	RowColumn not growing to accommodate its scale children when the top
 *	Shell is realized before thr RowColumn.
 *
 */

#include <X11/Xlib.h>
#include <testlib.h>


#define    NUM_SCALES   14
#define    MAX_NAME_LEN 100

/* The order should be maintained always */
#define BASEHEIGHT  0
#define BASEWIDTH   BASEHEIGHT+1
#define HEIGHTINC   BASEWIDTH+1
#define WIDTHINC    HEIGHTINC+1
#define ICONX       WIDTHINC+1
#define ICONY       ICONX+1
#define MAXASPECTX  ICONY+1
#define MAXASPECTY  MAXASPECTX+1
#define MINASPECTX  MAXASPECTY+1
#define MINASPECTY  MINASPECTX+1
#define MAXHEIGHT   MINASPECTY+1
#define MAXWIDTH    MAXHEIGHT+1
#define MINHEIGHT   MAXWIDTH+1
#define MINWIDTH    MINHEIGHT+1


struct scale_info {

    Widget  widget;
    char    titleString[MAX_NAME_LEN + 1];
    int     minValue;
    int     maxValue;

};

struct scale_info Scales[NUM_SCALES] = {

    { NULL, "baseHeight", 0, -1 },
    { NULL, "baseWidth",  0, -1 },
    { NULL, "heightInc",  1, -1 },
    { NULL, "widthInc",   1, -1 },
    { NULL, "iconX",      0, -1 },
    { NULL, "iconY",      0, -1 },
    { NULL, "maxAspectX", 1, -1 },
    { NULL, "maxAspectY", 1, -1 },
    { NULL, "minAspectX", 1, -1 },
    { NULL, "minAspectY", 1, -1 },
    { NULL, "maxHeight",  0, -1 },
    { NULL, "maxWidth",   0, -1 },
    { NULL, "minHeight",  0, -1 },
    { NULL, "minWidth",   0, -1 }

};

Arg scale_values[NUM_SCALES] = {

    { XmNbaseHeight, -1 },
    { XmNbaseWidth,  -1 },
    { XmNheightInc,  -1 },
    { XmNwidthInc,   -1 },
    { XmNiconX,      -1 },
    { XmNiconY,      -1 },
    { XmNmaxAspectX, -1 },
    { XmNmaxAspectY, -1 },
    { XmNminAspectX, -1 },
    { XmNminAspectY, -1 },
    { XmNmaxHeight,  -1 },
    { XmNmaxWidth,   -1 },
    { XmNminHeight,  -1 },
    { XmNminWidth,   -1 }

};

int         screenHeight, screenWidth;
Widget      Form;
Widget      WMShell_Resources;
Widget      Frame1;
Widget      RowCol1;
Widget      WhichShell_Pulldown;
Widget      WhichShell_Option;
Widget      Application_Shell, Top_Shell, Dialog_Shell, All_Shells;
Widget      applicationShell, topLevelShell, dialogShell;
Widget      application_btn, topLevel_btn;

XmFontList  font_list;
char        fontName[MAX_NAME_LEN + 1];
Bool        no_adjust = False;

Bool        show_scalebug = False;	/* PIR 3397 */

/*  Private Functions  */


static void     Application_ShellCB(Widget w, XtPointer client_data, 
                                    XtPointer call_data);
static void     Top_ShellCB(Widget w, XtPointer client_data, 
                            XtPointer call_data);
static void     Dialog_ShellCB(Widget w, XtPointer client_data, 
                               XtPointer call_data);
static void     All_ShellsCB(Widget w, XtPointer client_data, 
                             XtPointer call_data);
    
static void     CreateWMShellResources(Widget w);

static void     GetWMShellResources();
static void     SetWMShellResources(Widget widget);

static Widget   CreatePushButton(Widget parent, char *label);
static void     ProcessCommandArgs();
static void     get_next_word(char *source, char *dest);



void
main(unsigned int argc, char **argv)
{

    int         i;
    int         n;
    Arg         args[MAX_ARGS];
    XmString    tcs, tcs2;
#ifdef MOTIF1_1
	XmStringCharSet charset = (char *) XmSTRING_DEFAULT_CHARSET;
#else
	XmStringCharSet charset = (char *) XmFONTLIST_DEFAULT_TAG;
#endif


    CommonTestInit(argc, argv);

	fontName[0] = '\0';
    if (UserData != NULL)
		ProcessCommandArgs();

	if (fontName[0] == '\0')
        strcpy(fontName, "fixed");

    /* Init the max values for the scales */
    screenHeight = HeightOfScreen(screen);
    screenWidth = WidthOfScreen(screen);
    for (i = 0; i < NUM_SCALES; i++) {

        if (i == BASEWIDTH || i == MINASPECTX || i == MINWIDTH)
            Scales[i].maxValue = screenWidth / 2;
        else if (i == BASEHEIGHT || i == MINASPECTY || i == MINHEIGHT)
            Scales[i].maxValue = screenHeight / 2;
        else if (i == ICONX || i == MAXASPECTX || i == MAXWIDTH)
            Scales[i].maxValue = screenWidth;
        else if (i == ICONY || i == MAXASPECTY || i == MAXHEIGHT)
            Scales[i].maxValue = screenHeight;
        else if (i == WIDTHINC)
            Scales[i].maxValue = screenWidth / 4;
        else if (i == HEIGHTINC)
            Scales[i].maxValue = screenHeight / 4;

    }
            
    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
	XtSetValues(Shell1, args, n);

    /* Create Outer Form */

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);

    tcs = XmStringCreateLtoR("WMShell Resources:", charset);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList(fontName)); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;

    WMShell_Resources = XmCreateLabel(Form, "WMShell_Resources", args, n);
    XtManageChild(WMShell_Resources);

	if (show_scalebug == True)			/* PIR 3397 */
    	XtRealizeWidget(Shell1);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, WMShell_Resources); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
													/* Begin PIR3261 */
	if (no_adjust == True) {
    	XtSetArg(args[n], XmNadjustLast, False); n++;
    	XtSetArg(args[n], XmNbackground, CommonGetColor("white")); n++;
	}
													/* End PIR3261 */
	else
    	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    RowCol1 = XmCreateRowColumn(Form, "RowCol1", args, n);
    XtManageChild(RowCol1);

    CreateWMShellResources(RowCol1);

	if (show_scalebug == False)			/* PIR 3397 */
    	XtRealizeWidget(Shell1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));    n++;
    WhichShell_Pulldown = XmCreatePulldownMenu(Shell1, "WhichShell_Pulldown", 
                                               args, n);

    Application_Shell = CreatePushButton(WhichShell_Pulldown, 
										 "ApplicationShell");
    Top_Shell = CreatePushButton(WhichShell_Pulldown, "TopShell");
    Dialog_Shell = CreatePushButton(WhichShell_Pulldown, "DialogShell");
    All_Shells = CreatePushButton(WhichShell_Pulldown, "All Shells");

    XtAddCallback(Application_Shell, XmNactivateCallback, 
				  Application_ShellCB, NULL);
    XtAddCallback(Top_Shell, XmNactivateCallback, Top_ShellCB, NULL);
    XtAddCallback(Dialog_Shell, XmNactivateCallback, Dialog_ShellCB, NULL);
    XtAddCallback(All_Shells, XmNactivateCallback, All_ShellsCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNsensitive, False); n++;
    XtSetValues(Application_Shell, args, n);
    XtSetValues(Top_Shell, args, n);
    XtSetValues(Dialog_Shell, args, n);

    tcs = XmStringCreateLtoR("Apply Changes To         ", charset);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, Application_Shell); n++;
    XtSetArg(args[n], XmNsubMenuId, WhichShell_Pulldown); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowCol1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    WhichShell_Option = XmCreateOptionMenu(Form, "WhichShell_Option", 
                                           args, n);
    XtManageChild(WhichShell_Option);

    XmStringFree(tcs);

    /* 
     * Create an applicationShell.
     */

    n = 0;
    XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
    XtSetArg(args[n], XmNiconX, 200); n++;
    XtSetArg(args[n], XmNiconY, 200); n++;
    XtSetArg(args[n], XmNminHeight, 100); n++;
    XtSetArg(args[n], XmNminWidth, 100); n++;
    XtSetArg(args[n], XmNmaxHeight, 200); n++;
    XtSetArg(args[n], XmNmaxWidth, 200); n++;

    applicationShell = XtAppCreateShell("applicationShell", "ApplicationShell",
                                     	applicationShellWidgetClass, display,
                                     	args, n);
    n = 0;
    XtSetArg(args[n], XmNlabelString, 
             XmStringCreateSimple("applicationShell")); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    application_btn = XmCreatePushButton(applicationShell, 
										 "application_btn", args, n);
    XtManageChild(application_btn);
    XtRealizeWidget(applicationShell);

    n = 0;
    XtSetArg(args[n], XmNsensitive, True); n++;
    XtSetValues(Application_Shell, args, n);

    CommonPause();

    /* 
     * Create a topLevelShell.
     */

    n = 0;
    XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
    XtSetArg(args[n], XmNiconX, 200); n++;
    XtSetArg(args[n], XmNiconY, 200); n++;
    XtSetArg(args[n], XmNminHeight, 100); n++;
    XtSetArg(args[n], XmNminWidth, 100); n++;
    XtSetArg(args[n], XmNmaxHeight, 200); n++;
    XtSetArg(args[n], XmNmaxWidth, 200); n++;

    topLevelShell = XtAppCreateShell("topLevelShell", "TopLevelShell",
                                     topLevelShellWidgetClass, display,
                                     args, n);
    n = 0;
    XtSetArg(args[n], XmNlabelString, 
             XmStringCreateSimple("topLevelShell")); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    topLevel_btn = XmCreatePushButton(topLevelShell, "topLevel_btn", args, n);
    XtManageChild(topLevel_btn);
    XtRealizeWidget(topLevelShell);

    n = 0;
    XtSetArg(args[n], XmNsensitive, True); n++;
    XtSetValues(Top_Shell, args, n);

	CommonPause();

	/* 
	 *	Create a dialog shell
	 */
	n = 0;
	XtSetArg (args[n], XmNmessageString, 
			  XmStringCreateSimple ("This is a modeless dialog box"));
	XtSetArg (args[n], XmNtitle, "Modeless Dialog"); n++;
	XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
    XtSetArg(args[n], XmNminHeight, 100); n++;
    XtSetArg(args[n], XmNminWidth, 100); n++;
    XtSetArg(args[n], XmNmaxHeight, 200); n++;
    XtSetArg(args[n], XmNmaxWidth, 200); n++;
	dialogShell = XmCreateMessageDialog(Shell1, "dialogShell", args, n);
	XtManageChild(dialogShell);

    n = 0;
    XtSetArg(args[n], XmNsensitive, True); n++;
    XtSetValues(Dialog_Shell, args, n);

    CommonPause();

	CommonPause();

    XtAppMainLoop(app_context);

}


static void
CreateWMShellResources(Widget parent)

{

    int    i;
    Arg    args[MAX_ARGS];
    int    n;

    /* Create all the scales */

    for (i = 0; i < NUM_SCALES; i++) {

        n = 0;
        XtSetArg(args[n], XmNtitleString, 
                 XmStringCreateSimple(Scales[i].titleString)); n++;
        XtSetArg(args[n], XmNminimum, Scales[i].minValue); n++;
        XtSetArg(args[n], XmNmaximum, Scales[i].maxValue); n++;
        XtSetArg(args[n], XmNvalue, Scales[i].minValue); n++;

        XtSetArg(args[n], XmNshowValue, True); n++;
        XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNscaleMultiple, 10); n++;

        Scales[i].widget = XmCreateScale(parent, Scales[i].titleString,
                                         args, n);
        XtManageChild(Scales[i].widget);

    }

}


static void
Application_ShellCB(Widget w, XtPointer client_data, XtPointer call_data)
{

    if (applicationShell == NULL) {
        fprintf(stdout, "application Shell not created\n");
        return;
    }

    GetWMShellResources();
    SetWMShellResources(applicationShell);

    XtUnmapWidget(applicationShell);
    XSync(display, False);
    XtMapWidget(applicationShell);
    XSync(display, False);

}


static void
Top_ShellCB(Widget w, XtPointer client_data, XtPointer call_data)
{

    if (topLevelShell == NULL) {
        fprintf(stdout, "topLevel Shell not created\n");
        return;
    }

    GetWMShellResources();
    SetWMShellResources(topLevelShell);

    XtUnmapWidget(topLevelShell);
    XSync(display, False);
    XtMapWidget(topLevelShell);
    XSync(display, False);

}


													/* Begin PIR3262 */
static void
Dialog_ShellCB(Widget w, XtPointer client_data, XtPointer call_data)
{

    if (dialogShell == NULL) {
        fprintf(stdout, "Dialog Shell not created\n");
        return;
    }

    GetWMShellResources();
    SetWMShellResources(XtParent(dialogShell));

    XtUnmanageChild(dialogShell);
    XSync(display, False);
    XtManageChild(dialogShell);
    XSync(display, False);

}
													/* End PIR3262 */


static void
All_ShellsCB(Widget w, XtPointer client_data, XtPointer call_data)
{

    GetWMShellResources();

	if (applicationShell != NULL) {

        SetWMShellResources(applicationShell);
        XtUnmapWidget(applicationShell);
        XSync(display, False);
        XtMapWidget(applicationShell);

    }

	if (topLevelShell != NULL) {

        SetWMShellResources(topLevelShell);
        XtUnmapWidget(topLevelShell);
        XSync(display, False);
        XtMapWidget(topLevelShell);

    }

    if (dialogShell != NULL) {

        SetWMShellResources(XtParent(dialogShell));
    	XtUnmanageChild(dialogShell);
        XSync(display, False);
    	XtManageChild(dialogShell);
        XSync(display, False);

    }

}



static void
GetWMShellResources()

{

    int    i;
    Arg    args[MAX_ARGS];
    int    n;

    for (i = 0; i < NUM_SCALES; i++) {

        if (Scales[i].widget == NULL)
            scale_values[i].value = -1;
        else {

            n = 0;
            XtSetArg (args[n], XmNvalue, &scale_values[i].value); n++;
            XtGetValues (Scales[i].widget, args, n);

        }

    }

}


static void
SetWMShellResources(Widget shell)

{

    int    i;
    Arg    args[MAX_ARGS];
    int    n;

    n = 0;
    for (i = 0; i < NUM_SCALES; i++) {

        if (scale_values[i].value != -1)
            XtSetArg (args[n], scale_values[i].name, 
					  scale_values[i].value); n++;

    }

    XtSetValues (shell, args, n);

}


static Widget
CreatePushButton(Widget parent, char *label)

{
    Widget        widget;
    register int  n;
    Arg           args[3];
    XmString      tcs;
#ifdef MOTIF1_1
	XmStringCharSet charset = (char *) XmSTRING_DEFAULT_CHARSET;
#else
	XmStringCharSet charset = (char *) XmFONTLIST_DEFAULT_TAG;
#endif

    tcs = XmStringCreateLtoR(label, charset);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList(fontName)); n++;
    widget = XmCreatePushButton(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);

}


#define	WORD_LEN	32

static void
ProcessCommandArgs()
{

	char	*user_data;
	char	next_word[WORD_LEN + 1];

	user_data = UserData;

	while (*user_data != '\0') {

		get_next_word(user_data, next_word);
		user_data += strlen(next_word);

		if (strcmp(next_word, "-noadjust") == 0) {
			no_adjust = True;
			continue;
		}

		if (strcmp(next_word, "-F") == 0) {

			get_next_word(user_data, next_word);
			strcpy(fontName, next_word);
			user_data += strlen(next_word);
			continue;

		}
		if (strcmp(next_word, "-scalebug") == 0) {		/* PIR 3397 */
			show_scalebug = True;
			continue;
		}

	}

	free(UserData);

}


static void
get_next_word(char *source, char *dest)
{

	int	n;

	while (isspace(*source))
		source++;

	n = 0;
	while (!isspace(*source) && *source != '\0' && n < WORD_LEN) {
		*dest++ = *source++;
		n++;
	}
	*dest = '\0';

}
