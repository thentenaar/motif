#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/ColorS.h>
#include <Xm/Paned.h>
#include <Xm/ButtonBox.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/*
 *
 * The following defines help in clarifying the data being passed.
 *
 */

#define EXPLAIN_MODE 1
#define EXPLAIN_NAME 2
#define EXPLAIN_MAR 3
#define EXPLAIN_RGB 4

void Exit(Widget w, XtPointer client, XtPointer call);
void Explain(Widget w, XtPointer client, XtPointer call);
void GetColor(Widget w, XtPointer client, XtPointer call);

/*
 *
 * Structure to pass data back and forth using a callbacks' client_data
 *
 */

typedef struct _Holder{
    Widget colorBtn;
    Widget colorWidget;
    XColor the_color;
    Colormap the_map;
    } holderStruct;
    
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
    "*colorSelector*background: white",
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel.marginWidth: 10",
    "*infolabel.marginHeight: 10",
    "*buttonBox1*XmPushButton.labelString: Explain...",
    "*rightpane*colModeLabel.labelString: XmNcolorMode",
    "*rightpane*colNameLabel.labelString: XmNcolorName",
    "*rightpane*marLabel.labelString: XmNmarginWidth, XmNmarginHeight",
    "*rightpane*rgbFileLabel.labelString: XmNrgbFile",
    "*quitPB.labelString: Quit",
    "*colorPB.labelString: Color",
    "*buttonBox1*XmLabel.alignment: XmALIGNMENT_BEGINNING",
    NULL,
};

void Exit(Widget w, XtPointer client, XtPointer call)
{
    exit(0);
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
    XmString xmstring=NULL;
    static Widget info = NULL;

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

    switch ((intptr_t)client) {
    case EXPLAIN_MODE:
        xmstring = XmStringCreateLtoR(
"This resouce may be either XmListMode and XmScaleMode. This\n\
determines what mode the Color Selector should use when it is\n\
created.\n\
\n\
List Mode presents a listing of colors built up from the\n\
XmNrgbFile resource. Scale Mode displays three sliders that\n\
adjust Red, Green, and Blue color values.", 
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case EXPLAIN_NAME:
        xmstring = XmStringCreateLtoR(
"This resource controls the name of the color that is displayed\n\
to the user. Using XtGetValues on this resource returns the name\n\
of the current color. Using XtSetValues on this resource sets the\n\
current color.\n\
\n\
This resource is of type String.",
                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case EXPLAIN_MAR:
        xmstring = XmStringCreateLtoR(
"The Color Selector is made up of many children. This resource\n\
adjusts the remaining space between a) each of the children in the\n\
Color Selector, and b) the outside children and the edge of the\n\
Color Selector.",

                                      XmSTRING_DEFAULT_CHARSET);
        break;
    case EXPLAIN_RGB:
        xmstring = XmStringCreateLtoR(
"The name of a file that contains valid color names and their\n\
appropriate values.\n\
\n\
All duplicates are removed, and the list is sorted, before it is\n\
shown to the user.",

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

static XmString
CreateHelpArea()
{
  XmString xmstr = 
    XmStringCreateLtoR(
"The Motif Color Selector is an interface to the colors \n\
available on a display.\n\
\n\
In Scale Mode, the position of the sliders \n\
determines RGB values of the current color, and displays the \n\
pixel value in the colored area at the bottom.\n\
In List Mode, a list of all the named colors in the current\n\
rgb.txt file are shown. Selecting from this list updates\n\
the color display area with the associated pixel value.\n\
The ColorSelector will attempt to show the closest approximation\n\
to the color selected in one mode when switching to another\n\
if it cannot find an exact match .\n\
\n\
A developer may get the current color value programmatically\n\
through XtGetValues. An example of how this is used is shown\n\
by the selecting the Color Button at the bottom of the right \n\
half of the demo. The value obtained from the ColorSelector\n\
was used to set the backgroundColor of the Color Button.\n\
\n\
Resources defined by the Color Selector are shown below. For more\n\
information on a resource, select the Explain button to \n\
its right.", XmSTRING_DEFAULT_CHARSET);

  return( xmstr );
}



/*
 *
 * This function demonstrates the use of the new widget.  
 * The color generated by the Color Selector Widget is used
 * to change the background of the push button calling this 
 * routine.
 *
 */

void GetColor(Widget w, XtPointer client, XtPointer call)
{
    Arg args[5];
    Cardinal argcnt;
    String color_name;

    holderStruct holder = *(holderStruct *)client;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNcolorName, &color_name); argcnt++;
    XtGetValues(holder.colorWidget, args, argcnt);

    XParseColor(XtDisplay(w), holder.the_map, color_name, &(holder.the_color));
    XAllocColor(XtDisplay(w), holder.the_map, &(holder.the_color));

    argcnt = 0;
    XtSetArg(args[argcnt], XmNbackground, holder.the_color.pixel); argcnt++;
    XtSetValues(holder.colorBtn, args, argcnt);

}
int
main(int argc, char **argv)
{
    Arg args[5];
    Cardinal argcnt;
    Widget top, colorframe, colorSelector, infoframe, infolabel,
    buttonBox, rbuttonBox, lbuttonBox, colModeLabel, pushButton,
    big_pane, right_pane, left_pane, explain_scrolled, 
    colNameLabel, marLabel, rgbFileLabel, quitPB, colorPB;
    XtAppContext app;

    XmString      xmstring;
    Colormap      default_map;
    XColor        the_color;
    unsigned long mask;
    
    holderStruct holder;
    
    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Color Selector Demo"); argcnt++;
    XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
    top = XtOpenApplication(
            &app, 
            "ColorSelector", 
            NULL, 
            0,
            &argc, 
            argv, 
            fallbacks,  
            sessionShellWidgetClass, 
            args, argcnt);

    default_map = DefaultColormapOfScreen(XtScreen(top));

    /* big pane */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    big_pane = XtCreateManagedWidget("bull",
		xmPanedWidgetClass, top, args, argcnt);

    colorframe = XtVaCreateManagedWidget("colorframe", xmFrameWidgetClass,
					 big_pane, 
					 XmNshowSash, False,
					 NULL);

    colorSelector = XtVaCreateManagedWidget("colorSelector",
					    xmColorSelectorWidgetClass, 
					    colorframe, NULL);

    /* right pane */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, False ); argcnt++;    
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    right_pane = XtCreateManagedWidget("rightpane", xmPanedWidgetClass,
				   big_pane, args,argcnt);

    infoframe = XtVaCreateManagedWidget("infoframe", xmFrameWidgetClass, 
					right_pane, 
					XmNshowSash, False,
					XmNresizeToPreferred, True,
					XmNpreferredPaneSize, 300,
					XmNallowResize, True, NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAUTOMATIC ); argcnt++;
    XtSetArg(args[argcnt], XmNscrollBarDisplayPolicy, XmAS_NEEDED ); 
    argcnt++;
    explain_scrolled = XtCreateManagedWidget( "scrollExplain", 
					     xmScrolledWindowWidgetClass,
					     infoframe, args, argcnt );
    
    xmstring = CreateHelpArea();

    infolabel = XtVaCreateManagedWidget("infolabel", xmLabelWidgetClass,
					explain_scrolled,
					XmNalignment, XmALIGNMENT_BEGINNING,
					XmNlabelString, xmstring,
					NULL);
    
    /* Resources and their associated Explain buttons */

				       
    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillAll); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, True); argcnt++;
    buttonBox = XtCreateManagedWidget("buttonBox1",xmButtonBoxWidgetClass,
				      right_pane, args, argcnt);
    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillMinor); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    lbuttonBox = XtCreateManagedWidget("buttonBoxl", xmButtonBoxWidgetClass,
				       buttonBox, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillMinor); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    rbuttonBox = XtCreateManagedWidget("buttonBoxr",
				xmButtonBoxWidgetClass,
				buttonBox, args, argcnt);

    colModeLabel = XtVaCreateManagedWidget("colModeLabel",
					   xmLabelWidgetClass,
					   lbuttonBox, NULL);

    pushButton = XtVaCreateManagedWidget("pushButton",
					 xmPushButtonWidgetClass,
					 rbuttonBox, NULL);
    XtAddCallback(pushButton, XmNactivateCallback, Explain, 
		  (XtPointer)EXPLAIN_MODE);


    colNameLabel = XtVaCreateManagedWidget("colNameLabel",
					   xmLabelWidgetClass,
					   lbuttonBox, NULL);

    pushButton = XtVaCreateManagedWidget("pushButton1",
					  xmPushButtonWidgetClass, 
					  rbuttonBox, NULL);
    XtAddCallback(pushButton, XmNactivateCallback, Explain, 
		  (XtPointer)EXPLAIN_NAME);

    marLabel = XtVaCreateManagedWidget("marLabel", xmLabelWidgetClass, 
					   lbuttonBox, NULL);

    pushButton = XtVaCreateManagedWidget("pushButton2",
					  xmPushButtonWidgetClass, 
					  rbuttonBox, NULL);
    XtAddCallback(pushButton, XmNactivateCallback, Explain, 
		  (XtPointer)EXPLAIN_MAR);

    rgbFileLabel = XtVaCreateManagedWidget("rgbFileLabel",
					   xmLabelWidgetClass,
					   lbuttonBox, NULL);

    pushButton = XtVaCreateManagedWidget("pushButton3",
					  xmPushButtonWidgetClass, 
					  rbuttonBox, NULL);

    XtAddCallback(pushButton, XmNactivateCallback, Explain, 
		  (XtPointer)EXPLAIN_RGB);

                           
/*
 *  Take Note:
 *
 * The following push button demostrates the point in using
 * the colorSelector widget. 
 *
 */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, True); argcnt++;
    buttonBox = XtCreateManagedWidget("buttonBox2",
				xmButtonBoxWidgetClass,
				right_pane, args, argcnt);

    colorPB = XtVaCreateManagedWidget("Color", xmPushButtonWidgetClass,
				      buttonBox, NULL );

    quitPB = XtVaCreateManagedWidget("quitPB", xmPushButtonWidgetClass, 
				      buttonBox, NULL );


    /* finished all the interface pieces, now do all the rest */

    XAllocColorCells(XtDisplay(top), default_map, False, 
                               &mask, 0, &the_color.pixel, 1);
    holder.colorBtn = colorPB;
    holder.colorWidget = colorSelector;
    holder.the_color = the_color;
    holder.the_map = default_map;

    XtAddCallback(colorPB, XmNactivateCallback, GetColor, &holder);

    XtAddCallback(quitPB, XmNactivateCallback, Exit, (XtPointer)0);

    XtRealizeWidget(top);

    XtAppMainLoop(app);
    return(0);
}


