#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#include <Xm/ButtonBox.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

Widget bbox;

void Exit(Widget w, XtPointer client, XtPointer call);
void Equal(Widget w, XtPointer client, XtPointer call);
void Fill(Widget w, XtPointer client, XtPointer call);
void Width(Widget w, XtPointer client, XtPointer call);
void OnlyNum(Widget w, XtPointer client, XtPointer call);
void Orient(Widget w, XtPointer client, XtPointer call);
void Explain(Widget w, XtPointer client, XtPointer call);

/*
 *
 * These are fallbacks that are designed to ensure the
 * program works properly in case the resource file is
 * not picked up.
 *
 */

static String fallbacks[] = {
    /*
     * General fallback resources.
     */ 

    "*background: grey", 
    "*bbox*background: white",
    "*bform.height: 575",
    "*bbox.width: 450",
    "*bbox.height: 65",
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel.marginWidth: 10",
    "*infolabel.marginHeight: 10",
    "*boxSizeLabel.labelString: XmNequalSize",
    "*fillOptionLabel.labelString: XmNfillOption",
    "*widLabel.labelString: XmNmarginWidth",
    "*htLabel.labelString: XmNmarginHeight",
    "*orientLabel.labelString: XmNorientation",
    "*toggle1.labelString: True",
    "*toggle2.labelString: False",
    "*toggle3.labelString: XmHORIZONTAL",
    "*toggle4.labelString: XmVERTICAL",
    "*resform*XmPushButton.labelString: Explain...",
    "*resform*pulldownMenu.nonePB.labelString: XmFillNone",
    "*resform*pulldownMenu.minorPB.labelString: XmFillMinor",
    "*resform*pulldownMenu.majorPB.labelString: XmFillMajor",
    "*resform*pulldownMenu.allPB.labelString: XmFillAll",
    "*quitPB.labelString: Quit",
    NULL,
};

void Exit(Widget w, XtPointer client, XtPointer call)
{
    exit(0);
}

/*
 *
 * This function forces the buttons to be of equal size.
 *
 */

void Equal(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    Boolean equal;

    if ((intptr_t)client == 1) 
        equal = True;
    else
        equal = False;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, equal); argcnt++;
    XtSetValues(bbox, args, argcnt);
}

/*
 *
 * This function forces the margins along the sides (width)
 * or along the top and bottom (height) to be of a particular
 * size.
 *
 */

void Width(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    char *val;
    int margin = 0;

    val = XmTextFieldGetString(w);
    margin = atoi(val);

    argcnt = 0;
    if ((intptr_t)client == 1)
    {
        XtSetArg(args[argcnt], XmNmarginHeight, (short) margin); argcnt++;
    }
    else
    {
        XtSetArg(args[argcnt], XmNmarginWidth, (short) margin); argcnt++;
    }
    XtSetValues(bbox, args, argcnt);

    XtFree(val);
}

/*
 *
 * Depending on the resource, the buttons either fill the box
 * along the minor axis (minor), the major axis (major), neither (none)
 * or completely fill the box (all).
 *
 */

void Fill(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;

    argcnt = 0;
    switch ((intptr_t)client) {
    case 1:
           XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
           XtSetValues(bbox, args, argcnt);
           break;
    case 2:
           XtSetArg(args[argcnt], XmNfillOption, XmFillMinor); argcnt++;
           XtSetValues(bbox, args, argcnt);
           break;
    case 3:
           XtSetArg(args[argcnt], XmNfillOption, XmFillMajor); argcnt++;
           XtSetValues(bbox, args, argcnt);
           break;
    case 4:
           XtSetArg(args[argcnt], XmNfillOption, XmFillAll); argcnt++;
           XtSetValues(bbox, args, argcnt);
           break;
    }
}

/*
 * Validation of input.
 *
 * A common routine to ensure that the only values entered
 * in the text box are all numeric.
 *
 */

void OnlyNum(Widget w, XtPointer client, XtPointer call)
{
    XmTextVerifyCallbackStruct *verify_struct =
        (XmTextVerifyCallbackStruct *) call;

    if (verify_struct->text->ptr)
        if (!isdigit(*verify_struct->text->ptr))
            verify_struct->doit = False;
}

/*
 *
 * Specifies the orientation of the buttons in the box.
 *
 */

void Orient(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    unsigned char orient;

    if ((intptr_t)client == 1)
        orient = XmVERTICAL;
    else
        orient = XmHORIZONTAL;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, orient); argcnt++;
    XtSetValues(bbox, args, argcnt);
}

/*
 *
 * Stuff needed for the demo program to output info
 *
 */

void Explain(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    XmString xmstring = NULL;
    static Widget info = NULL;
    intptr_t explain = (intptr_t)client;

    if (info == NULL) {
        Widget temp;

        argcnt = 0;
        XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
        info = XmCreateInformationDialog(w, "explain", args, argcnt);

        temp = XmMessageBoxGetChild(info, XmDIALOG_CANCEL_BUTTON);
        XtUnmanageChild(temp);
        temp = XmMessageBoxGetChild(info, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(temp);
    }

    switch (explain) {
    case 1:
        xmstring = XmStringCreateLtoR(
"The equalSize resource specifies whether children of the Button\n\
Box have equal heights and widths. The height and width chosen for\n\
all the children is the height and width of the largest child.",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case 2:
        xmstring = XmStringCreateLtoR(
"The fillOption resource sets four kinds of fill options: FillNone,\n\
FillMajor, FillMinor, and FillAll. Fill specifies how to use any extra\n\
space left over once all children have been sized. This resource usually\n\
expands children in a reasonable manner.\n\
\n\
FillMajor expands children in the direction of the orientation resource.\n\
FillMinor expands children in the opposite direction of the orientation \
resource.\n\
FillAll expands children in both major and minor directions.\n\
FillNone doesn't expand children.",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case 3:
        xmstring = XmStringCreateLtoR(
"marginWidth is the number of spacing pixels to the left and to the right\n\
of the children in the Button Box.",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case 4:
        xmstring = XmStringCreateLtoR(
"marginHeight is the number of spacing pixels on top and on bottom of the \n\
children in the Button Box.",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case 5:
        xmstring = XmStringCreateLtoR(
"orientation specifies whether the children of the Button Box are placed\n\
in a row or a column. HORIZONTAL places children in a row (major dimension\n\
is width, minor dimension is height). VERTICAL places children in a column\n\
(major dimension is height, minor dimension is width).",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    default:
        printf("explaining NOTHING\n");
        break;
    }

    argcnt = 0;
    XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
    XtSetValues(info, args, argcnt);

    XmStringFree(xmstring);

    XtManageChild(info);
}
int
main(int argc, char **argv)
{
    Arg args[5];
    Cardinal argcnt;
    Widget top, bboxframe, infoframe, infolabel, resframe, resform, 
           bboxSizeRC, boxSizeLabel, togl_btn1, togl_btn2, pushButton, radioRC, 
           fillOptionRC, fillOptionLabel, fillOptionMenu, nonePB, minorPB, 
           majorPB, allPB, menuShell, pulldownMenu, pushButton1,widRC, 
           widLabel, widText, htRC, htLabel, htText, pushButton2, orientRC, 
           orientLabel, pushButton3, pushButton4, quitPB, bbox_PB1, bbox_PB2, 
           bbox_PB3, bbox_PB4, togl_btn3, togl_btn4, orientRadio, bform;

    XtAppContext app;

    XmString xmstring;
    
    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Button Box Demo"); argcnt++;
    XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
    top = XtOpenApplication(
            &app, 
            "Buttonbox", 
            NULL, 
            0, 
            &argc, 
            argv, 
            fallbacks, 
            sessionShellWidgetClass,
            args, 
            argcnt);

    bform = XtVaCreateManagedWidget("bform", xmFormWidgetClass, top, 
                          XmNresizePolicy, XmRESIZE_GROW, NULL);

    bboxframe = XtVaCreateManagedWidget("bboxframe", xmFrameWidgetClass,bform,
                               XmNtopAttachment, XmATTACH_FORM,
                               XmNbottomAttachment, XmATTACH_FORM,
                               XmNtopOffset, 10,
                               XmNbottomOffset, 10,
                               XmNx, 10, XmNy, 10,
                               NULL);

	{
	Widget dbbox = XmCreateFormDialog(top,"frame",NULL,0);

/*
 *
 *  Take Note:
 * 
 *  Creation of the widget being demoed.
 *
 */

    bbox = XtVaCreateManagedWidget("bbox",
                               xmButtonBoxWidgetClass, dbbox,
                               XmNx, 2, XmNy, 2,
                               NULL);

	XtVaSetValues(bbox,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);
	XtManageChild(dbbox);
	}

/*
 *
 * The following widgets created are buttons to fill the box.
 * This is to assist in the demonstration of the buttonBox's
 * utility.
 *
 */

    bbox_PB1 = XmVaCreateManagedPushButton(bbox, "FirstButton",
                NULL);

    bbox_PB2 = XmVaCreateManagedPushButton(bbox, "SecondButton",
                NULL);
 
    bbox_PB3 = XmVaCreateManagedPushButton(bbox, "ThirdButton",
                NULL);
 
    bbox_PB4 = XtCreateManagedWidget("LastButton",
                xmPushButtonWidgetClass,
                bbox,
                NULL,
                0);

    xmstring = XmStringCreateLtoR(
"The Motif Button Box widget can arrange buttons into single rows and columns\n\
much more easily than the Motif Row Column or Motif Form!\n\
(And two ButtonBoxes paired together do a great job of lining items up.)\n\
\n\
The four buttons in the other window are inside an Motif Button Box. Try \
resizing\n\
this window! Try modifying the Button Box's resources using the area below.\n\
\n\
Press the \"Explain...\" button to find out more about a particular resource.",
                                  XmSTRING_DEFAULT_CHARSET);

    infoframe = XtVaCreateManagedWidget("infoframe", xmFrameWidgetClass, bform,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNrightAttachment, XmATTACH_POSITION,
                               XmNrightPosition, 95,
                               XmNtopOffset, 0,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 10,
                               XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
                               XmNtopWidget, bboxframe,
                               XmNleftAttachment, XmATTACH_WIDGET,
                               XmNleftWidget, bboxframe,
                               NULL);

    resframe = XtVaCreateManagedWidget("resframe", xmFrameWidgetClass, bform,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNrightAttachment, XmATTACH_POSITION,
                               XmNrightPosition, 95,
                               XmNtopOffset, 9,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 0,
                               XmNtopAttachment, XmATTACH_WIDGET,
                               XmNtopWidget, infoframe,
                               XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
                               XmNleftWidget, infoframe,
                               NULL);

    quitPB = XmVaCreatePushButton(bform, "quitPB",
                               XmNtopAttachment,     XmATTACH_NONE,
                               XmNleftAttachment,   XmATTACH_WIDGET,
                               XmNleftWidget,       bboxframe,
                               XmNbottomAttachment, XmATTACH_FORM,
                               XmNtopOffset,         0,
                               XmNbottomOffset,     10,
                               XmNleftOffset,      300,
                               XmNwidth, 100,
                               NULL);
    XtManageChild(quitPB);

    infolabel = XtVaCreateManagedWidget("infolabel", xmLabelWidgetClass,
                               infoframe,
                               XmNx, 2, XmNy, 2,
                               XmNalignment, XmALIGNMENT_BEGINNING,
                               XmNlabelString, xmstring,
                               NULL);


    resform = XtVaCreateManagedWidget("resform", xmFormWidgetClass, resframe,
                               XmNresizePolicy, XmRESIZE_GROW, NULL);

    bboxSizeRC = XtVaCreateManagedWidget("bboxSizeRC", xmRowColumnWidgetClass,
                               resform,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNtopAttachment, XmATTACH_FORM,
                               XmNleftAttachment, XmATTACH_FORM,
                               XmNrightAttachment, XmATTACH_NONE,
                               XmNtopOffset, 10,
                               XmNleftOffset, 10,
                               XmNrowColumnType, XmWORK_AREA,
                               NULL);

    fillOptionRC = XtVaCreateManagedWidget("fillOptionRC", 
                               xmRowColumnWidgetClass, resform,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNleftAttachment, XmATTACH_FORM,
                               XmNtopOffset, 10,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 10,
                               XmNrowColumnType, XmWORK_AREA,
                               XmNtopAttachment, XmATTACH_WIDGET,
                               XmNtopWidget, bboxSizeRC,
                               NULL);

    widRC = XtVaCreateManagedWidget("widRC", xmRowColumnWidgetClass, resform,
                               XmNorientation, XmHORIZONTAL,
                               XmNtopAttachment, XmATTACH_WIDGET,
                               XmNtopWidget, fillOptionRC,
                               XmNpacking, XmPACK_TIGHT,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNleftAttachment, XmATTACH_FORM,
                               XmNtopOffset, 10,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 10,
                               XmNrowColumnType, XmWORK_AREA,
                               NULL);

    htRC = XtVaCreateManagedWidget("htRC", xmRowColumnWidgetClass, resform,
                               XmNorientation, XmHORIZONTAL,
                               XmNtopAttachment, XmATTACH_WIDGET,
                               XmNtopWidget, widRC,
                               XmNpacking, XmPACK_TIGHT,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNleftAttachment, XmATTACH_FORM,
                               XmNtopOffset, 10,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 10,
                               XmNrowColumnType, XmWORK_AREA,
                               NULL);

    orientRC = XtVaCreateManagedWidget("orientRC", xmRowColumnWidgetClass, 
                               resform,
                               XmNtopAttachment, XmATTACH_WIDGET,
                               XmNtopWidget, htRC,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNbottomAttachment, XmATTACH_NONE,
                               XmNleftAttachment, XmATTACH_FORM,
                               XmNtopOffset, 10,
                               XmNbottomOffset, 0,
                               XmNleftOffset, 10,
                               XmNrowColumnType, XmWORK_AREA,
                               NULL);

    boxSizeLabel = XtVaCreateManagedWidget("boxSizeLabel", xmLabelWidgetClass,
                               bboxSizeRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNx, 82); argcnt++;
    XtSetArg(args[argcnt], XmNy, 3); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    radioRC = XmCreateRadioBox(bboxSizeRC,"radioRC", args, argcnt);

    togl_btn1 = XtVaCreateManagedWidget("toggle1", xmToggleButtonWidgetClass,
                               radioRC, NULL);

    togl_btn2 = XtVaCreateManagedWidget("toggle2", xmToggleButtonWidgetClass,
                               radioRC, NULL);

    XtManageChild(radioRC);

    pushButton = XtVaCreateManagedWidget("pushButton",
                               xmPushButtonWidgetClass, bboxSizeRC,
                               XmNx, 265, XmNy, 3,
                               NULL);

    fillOptionLabel = XtVaCreateManagedWidget("fillOptionLabel", 
                               xmLabelWidgetClass, fillOptionRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNx, 100); argcnt++;
    XtSetArg(args[argcnt], XmNy, 3); argcnt++;
    fillOptionMenu = XmCreateOptionMenu(fillOptionRC, "fillOptionMenu", 
                               args, argcnt);
    XtManageChild(fillOptionMenu);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNwidth, 1); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 1); argcnt++;
    menuShell = XtCreatePopupShell("menuShell", xmMenuShellWidgetClass,
                fillOptionRC, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_PULLDOWN); argcnt++;
    XtSetArg(args[argcnt], XmNx, 0); argcnt++;
    XtSetArg(args[argcnt], XmNy, 0); argcnt++;
    XtSetArg(args[argcnt], XmNwidth, 54); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 88); argcnt++;
    pulldownMenu = XtCreateWidget("pulldownMenu", xmRowColumnWidgetClass,
                menuShell, args, argcnt);

    nonePB = XtVaCreateManagedWidget("nonePB", xmPushButtonWidgetClass, 
                pulldownMenu, NULL);

    minorPB = XtVaCreateManagedWidget("minorPB", xmPushButtonWidgetClass,
                pulldownMenu, NULL);

    majorPB = XtVaCreateManagedWidget("majorPB", xmPushButtonWidgetClass,
                pulldownMenu, NULL);

    allPB = XtVaCreateManagedWidget("allPB", xmPushButtonWidgetClass,
                pulldownMenu, NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu); argcnt++;
    XtSetValues(fillOptionMenu, args, argcnt);

    pushButton1 = XtVaCreateManagedWidget("pushButton1",
                               xmPushButtonWidgetClass, fillOptionRC,
                               XmNx, 262, XmNy, 3,
                               NULL);

    widLabel = XtVaCreateManagedWidget("widLabel", xmLabelWidgetClass, widRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    widText = XtVaCreateManagedWidget("widText", xmTextFieldWidgetClass,
                widRC, XmNx, 76, XmNy, 3, NULL);

    pushButton2 = XtVaCreateManagedWidget("pushButton2",
                               xmPushButtonWidgetClass, widRC,
                               XmNx, 217, XmNy, 3,
                               NULL);

    htLabel = XtVaCreateManagedWidget("htLabel", xmLabelWidgetClass, htRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    htText = XtVaCreateManagedWidget("htText", xmTextFieldWidgetClass,
                htRC, XmNx, 76, XmNy, 3, NULL);

    pushButton3 = XtVaCreateManagedWidget("pushButton3",
                               xmPushButtonWidgetClass, htRC,
                               XmNx, 217, XmNy, 3,
                               NULL);

    orientLabel = XtVaCreateManagedWidget("orientLabel", xmLabelWidgetClass,
                               orientRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNx, 82); argcnt++;
    XtSetArg(args[argcnt], XmNy, 3); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    orientRadio = XmCreateRadioBox(orientRC,"orientRadio", args, argcnt);

    togl_btn3 = XtVaCreateManagedWidget("toggle3", xmToggleButtonWidgetClass,
                               orientRadio, NULL);

    togl_btn4 = XtVaCreateManagedWidget("toggle4", xmToggleButtonWidgetClass,
                               orientRadio, NULL);

	{
	unsigned char orientation;
	XtVaGetValues(bbox, XmNorientation, &orientation, NULL);
	if (XmHORIZONTAL == orientation)
		XmToggleButtonSetState(togl_btn3, True, False);
	else
		XmToggleButtonSetState(togl_btn4, True, False);
	}

    XtManageChild(orientRadio);

    pushButton4 = XtVaCreateManagedWidget("pushButton4",
                               xmPushButtonWidgetClass, orientRC,
                               XmNx, 82, XmNy, 3,
                               NULL);
                           
    XtAddCallback(quitPB, XmNactivateCallback, Exit, (XtPointer)0);

    XtAddCallback(pushButton, XmNactivateCallback, Explain, (XtPointer)1);

    XtAddCallback(pushButton1, XmNactivateCallback, Explain, (XtPointer)2);

    XtAddCallback(pushButton2, XmNactivateCallback, Explain, (XtPointer)3);

    XtAddCallback(pushButton3, XmNactivateCallback, Explain, (XtPointer)4);

    XtAddCallback(pushButton4, XmNactivateCallback, Explain, (XtPointer)5);

    XtAddCallback(togl_btn1, XmNvalueChangedCallback, Equal, (XtPointer)1);

    XtAddCallback(togl_btn2, XmNvalueChangedCallback, Equal, (XtPointer)0);

    XtAddCallback(nonePB, XmNactivateCallback, Fill, (XtPointer)1);

    XtAddCallback(minorPB, XmNactivateCallback, Fill, (XtPointer)2);

    XtAddCallback(majorPB, XmNactivateCallback, Fill, (XtPointer)3);

    XtAddCallback(allPB, XmNactivateCallback, Fill, (XtPointer)4);

    XtAddCallback(htText, XmNactivateCallback, Width, (XtPointer)1);
    XtAddCallback(htText, XmNmodifyVerifyCallback, OnlyNum, (XtPointer)0);

    XtAddCallback(widText, XmNactivateCallback, Width, (XtPointer)0);
    XtAddCallback(widText, XmNmodifyVerifyCallback, OnlyNum, (XtPointer)0);

    XtAddCallback(togl_btn4, XmNvalueChangedCallback, Orient, (XtPointer)1);

    XtAddCallback(togl_btn3, XmNvalueChangedCallback, Orient, (XtPointer)0);

    XtRealizeWidget(top);

    XtAppMainLoop(app);
    
    /* appease compiler warning god */
    return (0); 
}
