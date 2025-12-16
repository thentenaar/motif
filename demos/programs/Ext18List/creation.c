/*
 * MOTIF include files needed for widget creation.
 */
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/MultiList.h>
#include <Xm/ButtonBox.h>
#include <Xm/Paned.h>
#include <Xm/MenuShell.h>
#include <Xm/ScrolledW.h>
#include "extlist.h"

static void PlayerOptionMenu( Widget, DemoInfo );

/*
 * Function: RegisterConverters()
 *     Registers all the converters for all widgets.
 */
static void RegisterConverters(void)
{
    XtInitializeWidgetClass(xmPushButtonWidgetClass);
    XtInitializeWidgetClass(xmToggleButtonWidgetClass);
    XtInitializeWidgetClass(xmLabelWidgetClass);
    XtInitializeWidgetClass(xmFrameWidgetClass);
    XtInitializeWidgetClass(xmFormWidgetClass);
    XtInitializeWidgetClass(xmRowColumnWidgetClass);
    XtInitializeWidgetClass(xmMultiListWidgetClass);
    XtInitializeWidgetClass(xmPanedWidgetClass);
    XtInitializeWidgetClass(xmButtonBoxWidgetClass);
    XtInitializeWidgetClass(topLevelShellWidgetClass);
    XtInitializeWidgetClass(xmMenuShellWidgetClass);
}

/*
 * Function prototypes for routines
 * located in utilities file.
 */
extern XtPointer CONVERT();
extern void MENU_POST();


/*
 * Callback procedure declarations
 */
extern Widget CreateExtListCB(Widget parent);
extern void RemCB(Widget w, XtPointer client, XtPointer call);
extern void ToggleFindArea(Widget w, XtPointer client, XtPointer call);
extern void FirstRowCol(Widget w, XtPointer client, XtPointer call);
extern void UnselCB(Widget w, XtPointer client, XtPointer call);
extern void QuitCB(Widget w, XtPointer client, XtPointer call);
extern void UpdateRemLabelStr(Widget w,XtPointer client, XtPointer call);
extern void ChoosePlayerCB(Widget w,XtPointer client, XtPointer call);
extern void CreateLabel(Widget w, XtPointer client, XtPointer call);

/*
 * Function: Createform()
 *        Create form hierarchy of widgets.
 */
Widget
 Createform( Widget parent, DemoInfo demo_info)
{
    Arg    	args[512];
    Cardinal   	argcnt;
    Boolean   	argok;
    Widget 	retval;
    Widget	big_pane,right_pane;
    Widget	listframe;
    Widget	extlist;
    Widget	lineup_frame;
    Widget	lineup_row;
    Widget	add_rc;
    Widget	rem_rc;
    Widget	elist_op_frame;
    Widget	elist_op_row;
    Widget	unsel_pb;
    Widget	quit_pb;
    Widget	explain_frame, explain_scrolled ;
    Widget	explain_label;

    argok = False;

    RegisterConverters();

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    big_pane = XtCreateManagedWidget("pane",xmPanedWidgetClass, parent, args,
				     argcnt);

    /* the left pane */

    argcnt = 0;
    listframe = XtCreateManagedWidget("listframe", xmFrameWidgetClass,
				      big_pane, args, argcnt);


    /* this is an interesting routine to read through in the file
       callbacks-c.c. It shows how to initialize the extended list
       */

    demo_info->extlist = CreateExtListCB(listframe );
    XtAddCallback( demo_info->extlist, XmNsingleSelectionCallback,
		  UpdateRemLabelStr, (XtPointer)demo_info );

    /* create the right pane */

    right_pane = XtVaCreateManagedWidget("rigntPane",xmPanedWidgetClass,
				       big_pane, NULL );

    /* Create the Scrolled Help Text Area */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAUTOMATIC ); argcnt++;
    XtSetArg(args[argcnt], XmNscrollBarDisplayPolicy, XmAS_NEEDED ); argcnt++;
    XtSetArg(args[argcnt], XmNresizeToPreferred, True ); argcnt++;
    XtSetArg(args[argcnt], XmNpreferredPaneSize, 300 ); argcnt++;
    explain_scrolled = XtCreateManagedWidget( "scrollExplain",
					     xmScrolledWindowWidgetClass,
					     right_pane, args, argcnt );

    explain_label = XtCreateManagedWidget("explain_label",
		xmLabelWidgetClass, explain_scrolled, NULL, 0 );

    CreateLabel(explain_label, (XtPointer)0, (XtPointer)0);


    /* Create the controls area */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNskipAdjust, True ); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False ); argcnt++;
    lineup_frame = XtCreateManagedWidget("lineup_frame", xmFrameWidgetClass,
					 right_pane, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmVERTICAL); argcnt++;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillMajor ); argcnt++;
    lineup_row = XtCreateManagedWidget("lineup_row", xmButtonBoxWidgetClass,
		lineup_frame, args, argcnt );

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillMajor ); argcnt++;
    add_rc = XtCreateManagedWidget("add_rc", xmButtonBoxWidgetClass,
		lineup_row, args, argcnt );

    (void) XtCreateManagedWidget("add_label", xmLabelWidgetClass, add_rc,
				 NULL, 0 );

    PlayerOptionMenu(add_rc, demo_info);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNequalSize, True); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillMajor ); argcnt++;
    rem_rc = XtCreateManagedWidget("rem_rc", xmButtonBoxWidgetClass,
		lineup_row, args, argcnt );

    (void)XtCreateManagedWidget("rem_label", xmLabelWidgetClass, rem_rc,
				 NULL, 0 );

    demo_info->rem_pb = XtCreateManagedWidget("rem_pb",
					      xmPushButtonWidgetClass, rem_rc,
					      NULL, 0 );

    XtAddCallback(demo_info->rem_pb, XmNactivateCallback, RemCB,
		  (XtPointer)demo_info);

    /* next group */
    argcnt = 0;
    XtSetArg(args[argcnt], XmNskipAdjust, True ); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False ); argcnt++;
    lineup_frame = XtCreateManagedWidget("lineup_frame", xmFrameWidgetClass,
					 right_pane, args, argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone ); argcnt++;
    add_rc = XtCreateManagedWidget("quit_rc", xmButtonBoxWidgetClass,
				   lineup_frame, args, argcnt );
    unsel_pb = XtCreateManagedWidget("unsel_pb", xmPushButtonWidgetClass,
				   add_rc, NULL, 0 );

    XtAddCallback(unsel_pb, XmNactivateCallback, UnselCB,
		  (XtPointer)demo_info);

    unsel_pb = XtCreateManagedWidget("firstRowCol", xmPushButtonWidgetClass,
				   add_rc, NULL, 0 );

    XtAddCallback(unsel_pb, XmNactivateCallback, FirstRowCol,
		  (XtPointer)demo_info);

    unsel_pb = XtCreateManagedWidget("toggleFind", xmToggleButtonWidgetClass,
				   add_rc, NULL, 0 );

    XtAddCallback(unsel_pb, XmNarmCallback, ToggleFindArea,
		  (XtPointer)demo_info);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNfillOption, XmFillNone ); argcnt++;
    XtSetArg(args[argcnt], XmNskipAdjust, True ); argcnt++;
    XtSetArg(args[argcnt], XmNshowSash, False ); argcnt++;
    add_rc = XtCreateManagedWidget("quit_rc", xmButtonBoxWidgetClass,
				   right_pane, args, argcnt );

    quit_pb = XtCreateManagedWidget("quit_pb",	xmPushButtonWidgetClass,
				    add_rc, NULL, 0);

    XtAddCallback(quit_pb, XmNactivateCallback, QuitCB, (XtPointer)0);

    return( big_pane );
}


static void
PlayerOptionMenu( Widget parent, DemoInfo demo_info )
{
    Arg    	args[512];
    Cardinal   	argcnt;
    Widget	optionMenu;
    Widget	menuShell;
    Widget	pulldownMenu;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_OPTION); argcnt++;
    XtSetArg(args[argcnt], XmNx, 3); argcnt++;
    XtSetArg(args[argcnt], XmNy, 3); argcnt++;
    optionMenu = XtCreateManagedWidget("optionMenu",
		xmRowColumnWidgetClass,	parent,	args,	argcnt);


    argcnt = 0;
    XtSetArg(args[argcnt], XmNwidth, 1); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 1); argcnt++;
    menuShell = XtCreatePopupShell("menuShell",
		xmMenuShellWidgetClass,
		XtParent(optionMenu),
		args,
		argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_PULLDOWN); argcnt++;
    XtSetArg(args[argcnt], XmNx, 0); argcnt++;
    XtSetArg(args[argcnt], XmNy, 0); argcnt++;
    XtSetArg(args[argcnt], XmNwidth, 108); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 298); argcnt++;
    pulldownMenu = XtCreateWidget("pulldownMenu",
		xmRowColumnWidgetClass,
		menuShell,
		args,
		argcnt);

    argcnt = 0;
    demo_info->player[0] = XtCreateManagedWidget("playerone",
						 xmPushButtonWidgetClass,
						 pulldownMenu,
						 args,
						 argcnt);

    XtAddCallback(demo_info->player[0], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[1] = XtCreateManagedWidget("playertwo",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[1], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[2] = XtCreateManagedWidget("playerthree",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[2], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[3] = XtCreateManagedWidget("playerfour",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[3], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[4] = XtCreateManagedWidget("playerfive",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[4], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[5] = XtCreateManagedWidget("playersix",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[5], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[6] = XtCreateManagedWidget("playerseven",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[6], XmNactivateCallback, ChoosePlayerCB,
		  		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[7] = XtCreateManagedWidget("playereight",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[7], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );


    argcnt = 0;
    demo_info->player[8] = XtCreateManagedWidget("playernine",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[8], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[9] = XtCreateManagedWidget("playerten",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[9], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[10] = XtCreateManagedWidget("playereleven",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[10], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[11] = XtCreateManagedWidget("playertwelve",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[11], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[12] = XtCreateManagedWidget("playerthirteen",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[12], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    demo_info->player[13] = XtCreateManagedWidget("playerfourteen",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(demo_info->player[13], XmNactivateCallback, ChoosePlayerCB,
		  (XtPointer)demo_info );

    argcnt = 0;
    XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu); argcnt++;
    XtSetValues(optionMenu, args, argcnt);

}
