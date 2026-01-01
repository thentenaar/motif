#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/IconButton.h>
#include <Xm/ButtonBox.h>
#include <Xm/IconBox.h>
#include <Xm/Paned.h>
#include "small.bm"
#include <pixmaps/crab.xpm>
#include <pixmaps/eye.xpm>
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

void Exit(Widget , XtPointer , XtPointer );
void Explain(Widget , XtPointer client, XtPointer );
void ShowThisButton(Widget, XtPointer, XtPointer);
void CreateLabel(Widget , XtPointer , XtPointer );


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


/*
 *
 * The following define is essentially to assist in the
 * creation of properly formatted information dialogs.
 *
 * This is really a convenience thing which bears little
 * relevance to the actual working of the program.
 *
 */

#define OUTPUT_CODE "This is an %s inside an Icon Box.\n\
\n\
It was created using the following resource settings:\n\
\n\
iconPlacement = %s\n\
cellX         = %d\n\
cellY         = %d\n\
name          = %s"

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
    "*bform.width: 1100",
    "*bform.height: 600",
    "*background: grey",
    "*iconBox*background: white",
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*infolabel.marginWidth: 10",
    "*infolabel.marginHeight: 10",
    "*placeLabel.labelString: XmNiconPlacement",
    "*xLabel.labelString: XmNcellX",
    "*yLabel.labelString: XmNcellY",
    "*quitPB.labelString: Quit",
    "*pushButton.labelString: Explain",
    "*pushButton1.labelString: Explain",
    "*pushButton2.labelString: Explain",
    "*resform*XmPushButton.labelString: Explain...",
    NULL,
};

void Exit(Widget w, XtPointer client, XtPointer call)
{
    exit(0);
}

/*
 *
 * The following function goes through a lot of contortions
 * to output user friendly information.
 *
 * The logical gymnastics undertaken, however, help in
 * describing the internals of the widgets used.
 *
 */

void
ShowThisButton(Widget w, XtPointer client, XtPointer call)
{
    Widget info = NULL, temp;
    Arg args[5];
    Cardinal argcnt;
    XmIconPlacement place = XmIconNone;
    char place_str[50];
    XmString xmstring;
    short x, y;
    char message_string[BUFSIZ];
    String class_name = 0;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNiconPlacement, &place); argcnt++;
    XtSetArg(args[argcnt], XmNcellX, &x); argcnt++;
    XtSetArg(args[argcnt], XmNcellY, &y); argcnt++;
    XtGetValues(w, args, argcnt);

    switch(place) {
    case XmIconTop:
        sprintf(place_str, "XmIconTop");
        break;
    case XmIconLeft:
        sprintf(place_str, "XmIconLeft");
        break;
    case XmIconRight:
        sprintf(place_str, "XmIconRight");
        break;
    case XmIconBottom:
        sprintf(place_str, "XmIconBottom");
        break;
    case XmIconOnly:
        sprintf(place_str, "XmIconOnly");
        break;
    default:
    case XmIconNone:
        sprintf(place_str, "XmIconNone");
        break;
    }
    /*
     * Obtaining the class name here is a little "tricky"
     */
    if (XtClass(w) == xmIconButtonWidgetClass) {
        class_name = "XmIconButton";
    } else if (XtClass(w) == xmPushButtonWidgetClass) {
	{
        sprintf(place_str, "not applicable");
        class_name = "XmPushButton";
	}
    } else if (XtClass(w) == xmToggleButtonWidgetClass) {
	{
        sprintf(place_str, "not applicable");
        class_name = "XmToggleButton";
	}
    }

    sprintf(message_string, OUTPUT_CODE, class_name,
            place_str, x, y, XtName(w));

    xmstring = XmStringCreateLocalized(message_string);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Code Sample"); argcnt++;
    XtSetArg(args[argcnt], XmNmessageString, xmstring); argcnt++;
    info = XmCreateInformationDialog(w, "showcode", args, argcnt);

    XtUnmanageChild(XtNameToWidget(info, "Cancel"));
    XtUnmanageChild(XtNameToWidget(info, "Help"));
    XtManageChild(info);
    XmStringFree(xmstring);
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
    XmString xmstring =0 ;
    static Widget info = NULL;

    if (!info) {
        argcnt = 0;
        XtSetArg(args[argcnt], XmNtitle, "Explanation"); argcnt++;
        info = (Widget)XmCreateInformationDialog(w, "explain", args, argcnt);

        XtUnmanageChild(XtNameToWidget(info, "Cancel"));
        XtUnmanageChild(XtNameToWidget(info, "Help"));
    }

    switch ((intptr_t)client) {
    case 1:
        xmstring = XmStringCreateLocalized(
" This is an IconButton resource. \n\
\n\
 Specifies the location of the pixmap (icon) with respect \n\
to the displayed text.\n\
\n\
 This resource can take one of the following values: XmIconTop,\n\
 XmIconBottom, XmIconLeft, XmIconRight, XmIconNone, XmIconOnly. ");
        break;
    case 2:
        xmstring = XmStringCreateLocalized(
" This is an IconBox resource.\n\
\n\
 It specifies the location of a particular button in cell space\n\
 in the x direction.");
        break;
    case 3:
        xmstring = XmStringCreateLocalized(
" This is an IconBox resource.\n\
\n\
 It specifies the location of a particular button in cell space\n\
 in the y direction.");
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
    Arg args[15];
    Cardinal argcnt;
    Widget top, iconBoxframe, iconBox, infoframe, infolabel,
           big_pane, right_pane, placeRC, xRC, yRC,
           placeLabel, xLabel, yLabel,
           pushButton, pushButton1, pushButton2,
           iconButton1, iconButton2, iconButton3,
           iconButton4, iconButton5, iconPushButton6,
           iconToggleButton7, iconLabel,
           quitPB;
    Widget explain_scrolled, buttonBox;
    Pixmap icon_pixmap;
    XtAppContext app;

    XmString xmstring;

    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtitle, "Icon Box/Button Demo"); argcnt++;
    XtSetArg(args[argcnt], XmNallowShellResize, True); argcnt++;
    top = XtOpenApplication(
            &app,
            "IconBox",
            NULL,
            0,
            &argc,
            argv,
            fallbacks,
            sessionShellWidgetClass,
            args,
            argcnt);

    /* big pane */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    big_pane = XtCreateManagedWidget("bull",
		xmPanedWidgetClass, top, args, argcnt);

    iconBoxframe = XtVaCreateManagedWidget("inconBoxframe", xmFrameWidgetClass,
					big_pane, NULL );


    argcnt = 0;
    iconBox = XtVaCreateManagedWidget("iconBox", xmIconBoxWidgetClass,
				      iconBoxframe,
				      XmNallowResize, True,
				      XmNresizeToPreferred, True,
				      XmNpreferredPaneSize, 300,
				      XmNminimumCellWidth, 48,
				      XmNminimumCellHeight, 48,
				      NULL );
    /* right pane */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, False ); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    right_pane = XtCreateManagedWidget("rightpane", xmPanedWidgetClass,
				   big_pane, args,argcnt);



/*
 *  Take Note:
 *
 *  Creation of the IconBox widget.  Note the widget class used.
 *
 */


    xmstring = XmStringCreateLocalized(
"This is a demo of two unique Motif Widgets: the Icon Button and Icon Box.\n\
\n\
Labels, Push Buttons, and Toggle Buttons are limited because these\n\
widgets only allow EITHER labels OR pixmaps on the face but NOT BOTH.\n\
Icon Buttons relieve the developer of this restriction, allowing both, or either,\n\
pixmaps and labels on its face.\n\
\n\
Row Column and Form from Motif make it difficult to produce \"matrix\", or \n\
\"cell oriented\" layouts (in which the height and width are the same).\n\
The Icon Box places a constraint on all of its children, making each child\n\
the same width and height. Moreover, the Icon Box lets the developer\n\
specify which cell in the Icon Box each child should go.\n\
");

    infoframe = XtVaCreateManagedWidget("infoframe", xmFrameWidgetClass,
					right_pane, NULL );


    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAUTOMATIC ); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNresizeToPreferred, True); argcnt++;
    XtSetArg(args[argcnt], XmNpreferredPaneSize, 300); argcnt++;
    XtSetArg(args[argcnt], XmNallowResize, True); argcnt++;
    XtSetArg(args[argcnt], XmNscrollBarDisplayPolicy, XmAS_NEEDED ); argcnt++;
    explain_scrolled = XtCreateManagedWidget( "scrollExplain",
					     xmScrolledWindowWidgetClass,
					     infoframe, args, argcnt );

    infolabel = XtVaCreateManagedWidget("infolabel", xmLabelWidgetClass,
					explain_scrolled,
                               XmNx, 2, XmNy, 2,
                               XmNalignment, XmALIGNMENT_BEGINNING,
                               XmNlabelString, xmstring,
                               NULL);


    placeRC = XtVaCreateManagedWidget("placeRC", xmRowColumnWidgetClass,
                               right_pane,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNrowColumnType, XmWORK_AREA,
			  XmNskipAdjust, True,
			      XmNshowSash, False,
                               NULL);

    xRC = XtVaCreateManagedWidget("xRC", xmRowColumnWidgetClass,
                               right_pane,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNrowColumnType, XmWORK_AREA,
				  XmNskipAdjust, True,
				      XmNshowSash, False,
                               NULL);

    yRC = XtVaCreateManagedWidget("yRC", xmRowColumnWidgetClass, right_pane,
                               XmNorientation, XmHORIZONTAL,
                               XmNpacking, XmPACK_TIGHT,
                               XmNrowColumnType, XmWORK_AREA,
				  XmNskipAdjust, True,
				      XmNshowSash, False,
                               NULL);

    placeLabel = XtVaCreateManagedWidget("placeLabel", xmLabelWidgetClass,
                               placeRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    pushButton = XtVaCreateManagedWidget("pushButton", xmPushButtonWidgetClass,
                               placeRC,
                               XmNx, 82, XmNy, 3,
                               NULL);

    xLabel = XtVaCreateManagedWidget("xLabel", xmLabelWidgetClass,
                               xRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    pushButton1 = XtVaCreateManagedWidget("pushButton1",
                               xmPushButtonWidgetClass, xRC,
                               XmNx, 82, XmNy, 3,
                               NULL);

    yLabel = XtVaCreateManagedWidget("yLabel", xmLabelWidgetClass, yRC,
                               XmNx, 3, XmNy, 3,
                               NULL);

    pushButton2 = XtVaCreateManagedWidget("pushButton2",
                               xmPushButtonWidgetClass, yRC,
                               XmNx, 58, XmNy, 3,
                               NULL);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, True); argcnt++;
    buttonBox = XtCreateManagedWidget("buttonBox1",
				xmButtonBoxWidgetClass,
				right_pane, args, argcnt);

    quitPB = XtVaCreateManagedWidget("quitPB", xmPushButtonWidgetClass,
				     buttonBox, NULL);

/*
 *  Take Note:
 *
 *  Creation of the IconButton widgets.  Note the widget class used.
 *
 */
    icon_pixmap = XPM_PIXMAP( iconBox, crab );
    iconButton1 = XtVaCreateManagedWidget("XmIconTop",
                xmIconButtonWidgetClass, iconBox, XmNpixmap, icon_pixmap,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  XmNiconPlacement, XmIconTop,
					  NULL );
    XtAddCallback(iconButton1, XmNactivateCallback, ShowThisButton, NULL);


    icon_pixmap = XPM_PIXMAP( iconBox, crab );
    iconButton2 = XtVaCreateManagedWidget("XmIconBottom",
                xmIconButtonWidgetClass, iconBox,XmNpixmap, icon_pixmap,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  XmNiconPlacement, XmIconBottom,
					  NULL );
    XtAddCallback(iconButton2, XmNactivateCallback, ShowThisButton, NULL);
    icon_pixmap = XCreateBitmapFromData(XtDisplay(iconBox),
                                   RootWindowOfScreen(XtScreen(iconBox)),
                                   (char *)small_bits,
                                   small_width, small_height);

    iconButton3 = XtVaCreateManagedWidget("XmIconLeft",
					  xmIconButtonWidgetClass,
					  iconBox,XmNpixmap, icon_pixmap,
					  XmNiconPlacement, XmIconLeft,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  NULL );
    XtAddCallback(iconButton3, XmNactivateCallback, ShowThisButton, NULL);

    iconButton4 = XtVaCreateManagedWidget("XmIconRight",
                xmIconButtonWidgetClass, iconBox,XmNpixmap, icon_pixmap,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  XmNiconPlacement, XmIconRight,
					  NULL );
    XtAddCallback(iconButton4, XmNactivateCallback, ShowThisButton, NULL);

    icon_pixmap = XPM_PIXMAP( iconBox, stopsign );
    iconButton5 = XtVaCreateManagedWidget("stop",
                xmIconButtonWidgetClass, iconBox,XmNpixmap, icon_pixmap,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  XmNiconPlacement, XmIconOnly,
					  NULL );
    XtAddCallback(iconButton5, XmNactivateCallback, ShowThisButton, NULL);



    iconPushButton6 = XtVaCreateManagedWidget("Push",
                xmPushButtonWidgetClass, iconBox,XmNpixmap, icon_pixmap,
					  XmNcellX, XmIconBoxAnyCell,
					  XmNcellY, XmIconBoxAnyCell,
					  NULL );

    XtAddCallback(iconPushButton6, XmNactivateCallback, ShowThisButton, NULL);

    iconToggleButton7 = XtVaCreateManagedWidget("PushMe!",
						xmToggleButtonWidgetClass,
						iconBox,
						XmNcellX, XmIconBoxAnyCell,
						XmNcellY, XmIconBoxAnyCell,
						NULL );

    XtAddCallback(iconToggleButton7, XmNvalueChangedCallback,
                                     ShowThisButton, NULL);

    XtAddCallback(quitPB, XmNactivateCallback, Exit, (XtPointer)0);

    XtAddCallback(pushButton, XmNactivateCallback, Explain, (XtPointer) 1);

    XtAddCallback(pushButton1, XmNactivateCallback, Explain, (XtPointer) 2);

    XtAddCallback(pushButton2, XmNactivateCallback, Explain, (XtPointer) 3);

    XtRealizeWidget(top);

    XtAppMainLoop(app);
    return(0);
}
