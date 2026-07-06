#include <stdlib.h>

#include <Xm/XmAll.h>

/*
 * Fallbacks: font, label and position go together in this demo
 */
static String fallbacks[] = {
"?.toolTipEnable: True",
"?.toolTipPostDelay: 2000",
"?.toolTipPostDuration: 500",
"?.TipShell.TipLabel.background: blue",
"*ToolTipControl.messageString: ToolTip Control Panel",
"*ToolTipControl.messageAlignment: XmALIGNMENT_CENTER",
"*ToolTipSlider.slideInterval: 5",
"*toolTipEnable.indicatorOn: False",
"*toolTipEnable.shadowThickness: 2",
"*toolTipEnable.fillOnSelect: True",
"*toolTipEnable.rightAttachment: XmATTACH_FORM",
"*toolTipEnable.labelString: Enable Tips",
"*toolTipString: No tip defined",
"*toolTipEnable.toolTipString: toggle usage of tool-tips",
"*toolTipPostDelayLabel.labelString: Posting Delay",
"*toolTipPostDelay.toolTipString: tip posting delay\\nin mS",
"*toolTipPostDelay.columns: 8",
"*toolTipPostDurationLabel.labelString: Duration",
"*toolTipPostDuration.toolTipString: tip duration\\nin mS",
"*toolTipPostDuration.columns: 8",
"*Exit.toolTipString: no tip necessary",
NULL
};

typedef struct {
	Pixel modified_color;
} AppResources_t, *AppResourcesPtr;
AppResources_t AppResources;

static XtResource resources[] =
{
    {"modifiedColor", "ModifiedColor", XtRPixel, sizeof(Pixel), XtOffset(AppResourcesPtr, modified_color), XtRString, "red"},
};

static void
ToggleEnable(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget shell = (Widget)client_data;
XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct *)call_data;

    XtVaSetValues(shell,
    	XmNtoolTipEnable, cbs->set,
    	NULL);
}

static void
ValueChanged(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget shell = (Widget)client_data;
XmString s;
int value = 0;
XmTextPosition position;
char buf[6], *tmp = NULL;

    if (!XmStringEmpty((s = XmTextFieldGetXmString(w)))) {
        tmp = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmCHARSET_TEXT);
        value = atoi(tmp);
        XtFree(tmp);
    }

    XmStringFree(s);
    value = value < 0 ? 0 : value > 60000 ? 60000 : value;
    snprintf(buf, 6, "%i", value);
    s = XmStringGenerate(buf, NULL, XmCHARSET_TEXT, NULL);

    position = XmTextFieldGetInsertionPosition(w);
    XtRemoveCallback(w, XmNvalueChangedCallback, (XtCallbackProc)ValueChanged, shell);
    XmTextFieldSetXmString(w, s);
    XmStringFree(s);

    XtAddCallback(w, XmNvalueChangedCallback, (XtCallbackProc)ValueChanged, shell);
    XmTextFieldSetInsertionPosition(w, position);
}

static void
Activate(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget shell = (Widget)client_data;
XmString s;
String valueString = NULL;
String callback;
int value = 0;
Pixel background;

    if (!XmStringEmpty((s = XmTextFieldGetXmString(w)))) {
        valueString = XmStringUngenerate(s, NULL, XmUTF8_TEXT, XmCHARSET_TEXT);
        value = atoi(valueString);
        XtFree(valueString);
    }

    XmStringFree(s);
    XtVaGetValues(w, XmNuserData, &callback, NULL);
    XtVaSetValues(shell, callback, value, NULL);
    XtVaGetValues(XtParent(w), XmNbackground, &background, NULL);
    XtVaSetValues(w, XmNbackground, background, NULL);
}

static void
ModifyVerify(Widget w, XtPointer client_data, XtPointer call_data)
{
Widget shell = (Widget)client_data;
XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
int i;

    for (i = 0; i < cbs->text->length; i++)
    {
    	if (cbs->text->ptr[i] < '0' ||
    	    cbs->text->ptr[i] > '9')
	{
	    cbs->doit = False;
	    break;
	}
    }
    if (cbs->doit)
    {
    	XtVaSetValues(w,
	    XmNbackground, AppResources.modified_color,
	    NULL);
    }
}

static void
ToolTipPopup(Widget w, XtPointer client_data, XtPointer call_data)
{
    puts(client_data ? "popping up" : "popping down");
}

static void
cancelCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
	if ((w = XtNameToWidget(w, "Cancel")))
		XtSetSensitive(w, !XtIsSensitive(w));
}

int
main(int argc, char *argv[])
{
    XtAppContext        app_context;
    Widget              top_level, helloworld_main,
                        toolTipEnable;
    Widget toolTipPostDelay;
    Widget toolTipPostDuration;
    Widget exitButton;
    Widget MessageBox;
    Widget ToolTipLabel;

    /*
     * Initialize Xt and create a resizable shell
     */

    top_level = XtVaAppInitialize(&app_context, "test1",
				  NULL, 0, &argc, argv, fallbacks, NULL);

    XtGetApplicationResources(top_level, &AppResources,
    	resources, XtNumber(resources),
    	NULL, 0);
    /*
     *  Create the widget tree: first the bb parent.
     */
    MessageBox = XmCreateMessageBox(top_level, "ToolTipControl", NULL, 0);
    helloworld_main = XmCreateForm (MessageBox, "helloworld_main", NULL, 0);

    toolTipEnable = XmCreateToggleButton(helloworld_main, "toolTipEnable", NULL, 0);
    {
    Boolean enable;

    	XtVaGetValues(top_level,
	    XmNtoolTipEnable, &enable,
	    NULL);
	XmToggleButtonSetState(toolTipEnable, enable, False);
    }
    XtVaSetValues(toolTipEnable,
    	XmNtopAttachment, XmATTACH_FORM,
    	XmNleftAttachment, XmATTACH_FORM,
    	NULL);
    XtAddCallback(toolTipEnable, XmNvalueChangedCallback, (XtCallbackProc)ToggleEnable, top_level);
    XtManageChild(toolTipEnable);

    {
    Widget label, text;
    int delay;
    char buf[255];

	toolTipPostDelay = XmCreateForm(helloworld_main, "toolTipPostDelayForm", NULL, 0);
	XtVaSetValues(toolTipPostDelay,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, toolTipEnable,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
	label = XmCreateLabel(toolTipPostDelay, "toolTipPostDelayLabel", NULL, 0);
	text = XmCreateTextField(toolTipPostDelay, "toolTipPostDelay", NULL, 0);
	XtVaGetValues(top_level,
	    XmNtoolTipPostDelay, &delay,
	    NULL);
	sprintf(buf, "%i", delay);
	XtVaSetValues(text,
	    XmNuserData, XmNtoolTipPostDelay,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNvalue, buf,
	    NULL);
	XtAddCallback(text, XmNmodifyVerifyCallback, (XtCallbackProc)ModifyVerify, top_level);
	XtAddCallback(text, XmNvalueChangedCallback, (XtCallbackProc)ValueChanged, top_level);
	XtAddCallback(text, XmNactivateCallback, (XtCallbackProc)Activate, top_level);

	XtVaSetValues(label,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, text,
	    XmNbottomWidget, text,
	    XmNrightWidget, text,
	    XmNleftAttachment, XmATTACH_FORM,
	    NULL);
	XtManageChild(label);
	XtManageChild(text);
	XtManageChild(toolTipPostDelay);
    }

    {
    Widget label, text;
    int delay;
    char buf[255];

	toolTipPostDuration = XmCreateForm(helloworld_main, "toolTipPostDurationForm", NULL, 0);
	XtVaSetValues(toolTipPostDuration,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, toolTipPostDelay,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
	label = XmCreateLabel(toolTipPostDuration, "toolTipPostDurationLabel", NULL, 0);
	text = XmCreateTextField(toolTipPostDuration, "toolTipPostDuration", NULL, 0);
	XtVaGetValues(top_level,
	    XmNtoolTipPostDuration, &delay,
	    NULL);
	sprintf(buf, "%i", delay);
	XtVaSetValues(text,
	    XmNvalue, buf,
	    XmNuserData, XmNtoolTipPostDuration,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
	XtAddCallback(text, XmNmodifyVerifyCallback, (XtCallbackProc)ModifyVerify, top_level);
	XtAddCallback(text, XmNvalueChangedCallback, (XtCallbackProc)ValueChanged, top_level);
	XtAddCallback(text, XmNactivateCallback, (XtCallbackProc)Activate, top_level);

	XtVaSetValues(label,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, text,
	    XmNbottomWidget, text,
	    XmNrightWidget, text,
	    XmNleftAttachment, XmATTACH_FORM,
	    NULL);
	XtManageChild(label);
	XtManageChild(text);
	XtManageChild(toolTipPostDuration);
    }

    exitButton = XmVaCreatePushButtonGadget(helloworld_main, "Exit",
    	XmNtopAttachment, XmATTACH_WIDGET,
    	XmNtopWidget, toolTipPostDuration,
    	XmNleftAttachment, XmATTACH_FORM,
    	XmNrightAttachment, XmATTACH_FORM,
    	NULL);
    XtAddCallback(exitButton, XmNactivateCallback, (XtCallbackProc)exit, NULL);
    XtManageChild(exitButton);

    XtManageChild(helloworld_main);
    XtManageChild(MessageBox);
    XtAddCallback(MessageBox, XmNcancelCallback, (XtCallbackProc)cancelCallback, NULL);
    XtAddCallback(MessageBox, XmNokCallback, (XtCallbackProc)cancelCallback, NULL);

    ToolTipLabel = XmToolTipGetLabel(top_level);
    XtAddCallback(XtParent(ToolTipLabel), XmNpopupCallback, (XtCallbackProc)ToolTipPopup, (XtPointer)0);
    XtAddCallback(XtParent(ToolTipLabel), XmNpopdownCallback, (XtCallbackProc)ToolTipPopup, (XtPointer)1);
    /*
     *  Realize the toplevel widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(top_level);

    /*
     *  Loop and process events
     */

    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}
