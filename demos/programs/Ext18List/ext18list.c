/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * MultiList.c
 *
 */

/**************************************************************
 *		INCLUDE FILES
 **************************************************************/
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/MultiList.h>
#include "extlist.h"
#include <pixmaps/crab.xpm>
#include <pixmaps/stopsign.xpm>
#include <pixmaps/porsche.xpm>
#include <pixmaps/clown.xbm>


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
 *		GLOBALS
 **************************************************************/
Widget G_extlist = NULL;
Pixmap porsche_pix;
Pixmap stopsign_pix;
Pixmap crab_pix;
Pixmap clown_pix;

PlayerData players[] = {
/*   Name      AB   R    H   RBI   AVG   HR */
{ PPORSCHE, "Mo Vaughn", 159, 30,  55, 33, 346, 11 },
{ PCLOWN, "Tim Naehring", 133, 25, 43, 27, 323, 6 },
{ PSTOP, "Scott Cooper", 139, 24, 43, 30, 309, 9 },
{ PCLOWN, "John Valentin", 78, 12, 22, 11, 282, 2 },
{ PSTOP, "Carlos Rodriguez", 39, 2, 11, 4, 282, 1},
{ PCLOWN, "Otis Nixon", 148, 23, 41, 13, 277, 0},
{ PSTOP, "Scott Fletcher", 80, 13, 21, 8, 263, 2},
{ PCLOWN, "Andre Dawson", 134, 19, 33, 27, 246, 10},
{ PPORSCHE, "Damon Berryhill", 72, 4, 17, 6, 236, 1},
{ PPORSCHE, "Billy Hatcher", 139, 23, 32, 15, 230, 1},
{ PCLOWN, "Mike Greenwell", 151, 31, 35, 25, 232, 7},
{ PSTOP, "Rich Rowland", 28, 1, 6, 6, 214, 1},
{ PCLOWN, "Lee Tinsley", 34, 10, 7, 2, 206, 0},
{ PPORSCHE, "Dave Valle", 72, 5, 11, 4, 153, 1},
};

/**************************************************************
 *		FORWARD DECLARATIONS
 **************************************************************/
Widget Createform(Widget, DemoInfo);
static void QuitCB(Widget, XtPointer, XtPointer);

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
    "*background: grey",
    "*rightPane*background: grey",
    "*fontList: -*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
    "*explain_label*fontList: -*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
    "*explain_label*marginWidth: 10",
    "*explain_label*marginHeight: 10",
    "*rem_label.alignment: XmALIGNMENT_BEGINNING",
    "*rem_label.labelString: Remove Player",
    "*add_label.labelString: Add Player   ",
    "*add_label.alignment: XmALIGNMENT_BEGINNING",
    "*rem_pb.labelString: No Selected Player",
    "*unsel_pb.labelString: Unselect Current",
    "*firstRowCol.labelString: Go to Row 2, 1",
    "*toggleFind.labelString: Toggle Finder",
    "*quit_pb.labelString: Quit",
    "*MultiList.title: Red Sox Line Up Card",
    "*multilistWidget*title: Line Up",
    "*playerone.labelString: Mo Vaughn",
    "*playertwo.labelString: Tim Naehring",
    "*playerthree.labelString: Scott Cooper",
    "*playerfour.labelString: John Valentin",
    "*playerfive.labelString: Carlos Rodriguez",
    "*playersix.labelString: Otis Nixon",
    "*playerseven.labelString: Scott Fletcher",
    "*playereight.labelString: Andre Dawon",
    "*playernine.labelString: Damon Berryhill",
    "*playerten.labelString: Billy Hatcher",
    "*playereleven.labelString: Mike Greenwell",
    "*playertwelve.labelString: Rich Rowland",
    "*playerthirteen.labelString: Lee Tinsley",
    "*playerfourteen.labelString: Dave Valle",
    "*ext18listWidget*background: White",
    "*list*background: White",
    NULL,
};

/**************************************************************
 *		CALLBACKS
 **************************************************************/
/*
 * Function Name: InitializeData
 * Description:
 * Arguments:
 * Returns:       nothing
 *
 */
void InitializeData(Widget shell)
{
  porsche_pix = XPM_PIXMAP(shell, porsche);
  crab_pix = XPM_PIXMAP(shell, crab);
  stopsign_pix = XPM_PIXMAP(shell, stopsign);
  clown_pix = XCreateBitmapFromData(XtDisplay(shell),
				RootWindowOfScreen(XtScreen(shell)),
				(char *)clown_bits, clown_width, clown_height);

}

/**************************************************************
 *		PUBLIC (GLOBAL) CODE
 **************************************************************/
/*
 * Function Name: main
 * Description:
 * Arguments:     the usual suspects
 * Returns:       nothing
 *
 */
int main(int argc, char *argv[])
{
    Widget top, demo;
    XtAppContext app;
    DemoInfo demo_info = (DemoInfo)XtMalloc( sizeof(DemoStruct));

    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);


    top = XtOpenApplication(
            &app,
            "MultiList",
            NULL,
            0,
            &argc,
            argv,
            fallbacks,
            sessionShellWidgetClass,
            NULL, 0);

    demo = Createform(top, demo_info);

    XtManageChild(demo);
    InitializeData(top);
    XtRealizeWidget(top);
    XtAppMainLoop(app);
    return 0;
}

