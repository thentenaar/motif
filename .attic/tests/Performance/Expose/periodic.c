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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: periodic.c /main/7 1995/07/14 12:00:23 drk $"
#endif
#endif

#include <testlib.h>
#include <Mrm/MrmPublic.h>                   /* Mrm Toolkit */

#define MAX_ARGS  100

#define K_scale_widget           1
#define K_drawnbutton_widget     2
#define K_togglebutton_widget    3
#define K_vtext1_widget          4
#define K_vtext2_widget          5
#define K_popup_menu_widget      6

static Widget Scale_1,Drawnbutton_1,Togglebutton_1,VText_1,VText_2,PopMenu_1,PMenu_1;
static Widget Label1, Separator1, PushBtn1, ToggleBtn1;

static MrmHierarchy	s_MrmHierarchy;		/* MRM database hierarch id */
static char		*vec[]={"periodic.uid"};
						/* MRM database file list   */
static MrmCode		class ;

static void quit_button_activate();
static void foo_activate();
static void create_callback();
static void zippy_null();
static void postmenu ();

static MrmCount		regnum = 4 ;
static MrmRegisterArg	regvec[] = {
	{"quit_button_activate",(caddr_t)quit_button_activate},
	{"foo_activate",(caddr_t)foo_activate},
	{"create_callback",(caddr_t)create_callback},
	{"zippy_null",(caddr_t)zippy_null}
	};

static Widget  CreateLabel(label, parent)
    char   *label;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    widget = XmCreateLabel(parent, "label", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}


static Widget  CreatePushButton(label, parent)
    char   *label;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    widget = XmCreatePushButton(parent, "pushButton", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}

static Widget  CreateToggle(label, parent)
    char   *label;
    Widget  parent;
{
    Widget        widget;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;


    n = 0;
    tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    widget = XmCreateToggleButton(parent, "toggle", args, n);
    XtManageChild(widget);
    XmStringFree(tcs);

    return(widget);
}

/*
 *  Main program
 */
int main(argc, argv)
unsigned int argc;
char **argv;
{
     /*
     *  Declare the variables to contain the two widget ids
     */
    Widget periodic_table_main;
    Arg args[1] ;
	int	n;
    int         status;

    /*
     *  Initialize the MRM
     */

    MrmInitialize ();

    /*
     *  Initialize the toolkit.  This call returns the id of the "Shell1"
     *  widget.  The applications "main" widget must be the only child
     *  of this widget.
     */

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg (args[n], XtNallowShellResize, FALSE) ; n++;
    XtSetValues(Shell1, args, n);
    

    /*
     *  Define the Mrm.hierarchy (only 1 file)
     */

    if (MrmOpenHierarchy (1,    /* number of files	    */
			vec, 			    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_MrmHierarchy)    /* ptr to returned id   */
			!= MrmSUCCESS) {
		printf ("can't open hierarchy\n"); 
		exit(1);
	}

    /*
     * 	Register our callback routines so that the resource manager can 
     * 	resolve them at widget-creation time.
     */

    if (MrmRegisterNames (regvec, regnum) != MrmSUCCESS) {
		printf("can't register names\n");
		exit(1);
	}

    /*
     *  Call MRM to fetch and create the pushbutton and its container
     */

    if (MrmFetchWidget (s_MrmHierarchy, "periodic_table_main",
						Shell1, &periodic_table_main, &class) != MrmSUCCESS) {
    	printf("can't fetch interface\n");
		exit(1);
	}

    /*
     *  Make the Shell1 widget "manage" the main window (or whatever the
     *  the uil defines as the topmost widget).  This will
     *  cause it to be "realized" when the Shell1 widget is "realized"
     */

    XtManageChild(periodic_table_main);
    
    /*
     *  Realize the Shell1 widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(Shell1);

    /*
    **  PopupMenu1
    */
    PMenu_1 = XmCreatePopupMenu(PopMenu_1, "popMenu", NULL, 0);
    XtAddEventHandler(PopMenu_1, ButtonPressMask, False, postmenu, PMenu_1);

    /*
    **  Menu1
    */
    Label1       = CreateLabel("POPUP MENU", PMenu_1);

	n = 0;
    XtSetArg(args[n], XmNseparatorType, XmDOUBLE_LINE); n++;
    Separator1   = XmCreateSeparator(PMenu_1, "separator", args, n);

    PushBtn1     = CreatePushButton("pushbutton", PMenu_1);

    ToggleBtn1   = CreateToggle("togglebutton", PMenu_1);


    /*
     *  Loop and process events
     */

    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}

static void zippy_null( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
}

#define fatal 1
static void  postmenu (w, popup, event)
    Widget         w;
    Widget         popup;
    XButtonEvent  *event;
{
    int         status;
    if (event->button != Button3)
        return;

    XmMenuPosition(PMenu_1, event);
    XtManageChild (PMenu_1);
}

/*
 * All widgets that are created call back to this procedure.
 */

static void create_callback(w, tag, reason)
    Widget w;
    int *tag;
    unsigned long *reason;
{

    /*  For internationalization ease, we capture a few strings from the
     *  widgets themselves.  We could go out and fetch them as needed but
     *  since we use these all the time, this method if more efficient.
     */
    switch (*tag) {
        case K_scale_widget: 
	    Scale_1 = w;
            break;
        case K_drawnbutton_widget: 
	    Drawnbutton_1 = w;
            break;
        case K_togglebutton_widget: 
	    Togglebutton_1 = w;
            break;
        case K_vtext1_widget: 
	    VText_1 = w;
            break;
        case K_vtext2_widget: 
	    VText_2 = w;
            break;
        case K_popup_menu_widget: 
	    PopMenu_1 = w;
            break;
        default: 
            break;
    }
}

static void foo_activate( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    Arg args[2];
	int	n;
    int scale_value;
    Boolean toggle_state;

	n = 0;
    XtSetArg( args[n], XmNvalue, &scale_value); n++;
    XtGetValues( Scale_1, args, n );
	n = 0;
    XtSetArg( args[n], XmNset, &toggle_state); n++;
    XtGetValues( Togglebutton_1, args, n );

    if ((scale_value == 13) && (toggle_state == True)) {
	n = 0;
	XtSetArg( args[n], XmNvalue, "OSF Motif Credits"); n++;
	XtSetValues( VText_1, args, n );
	n = 0;
	XtSetArg( args[n], XmNvalue,
"DEV: Vania, Ellis, Mike,\n\
..Alka, Al, Scott,\n\
..Daniel, and Paul\n\
REL: Ron and Martha\n\
Q/A: Libby, Tom, Ken,\n\
..Carl, and Dany\n\
DOC: Ken and Bob\n\
Moral Support:\n\
Mary Chung's Restaurant\n"); n++;
	XtSetValues( VText_2, args, n );
    }

}

static void quit_button_activate( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    Arg args[2];
	int	n;

    static int call_count = 1;

    call_count += 1 ;
    switch ( call_count )
        {
        case 1:
			  n = 0;
/*            XtSetArg( args[n], XmNlabelString,
                XmStringLtoRCreate("Goodbye\nWorld!","")); n++;
		XtSetValues( Text_1, args, n );          */
            break ;
        case 2:
            exit(1);
            break ;
        }
}
