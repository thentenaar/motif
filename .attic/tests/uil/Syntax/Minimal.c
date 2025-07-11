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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Minimal.c /main/7 1995/07/13 20:40:19 drk $"
#endif
#endif
#include <stdio.h>


#include <Mrm/MrmAppl.h>                   /* Motif Toolkit */
#include <Xm/Label.h>
#include <Xm/LabelP.h>
#include <Xm/BulletinB.h>
#include <Xm/BulletinBP.h>
#include <Xm/PushB.h>
#include <Xm/PushBP.h>

static MrmHierarchy	s_MrmHierarchy;		/* MRM database hierarch id */
static char		*vec[]={"Minimal.uid"};
						/* MRM database file list   */
static MrmCode		class ;

static void helloworld_button_activate();

static MrmCount		regnum = 1 ;
static MrmRegisterArg	regvec[] = {
	{"helloworld_button_activate",(caddr_t)helloworld_button_activate}
	};

/*
 *  Main program
 */
int main(argc, argv)
int argc;
char **argv;
{
     /*
     *  Declare the variables to contain the two widget ids
     */
    Widget toplevel, hellominimalmain;
    Arg arglist[1] ;

    /*
     *  Initialize the MRM. We only register the required classes.
     */

    MrmRegisterClass
	(0, NULL, "XmCreateBulletinBoard", XmCreateBulletinBoard,
	 (WidgetClass) &xmBulletinBoardClassRec);
    MrmRegisterClass
	(0, NULL, "XmCreatePushButton", XmCreatePushButton,
	 (WidgetClass) &xmPushButtonClassRec);
    MrmRegisterClass
	(0, NULL, "XmCreateLabel", XmCreateLabel,
	 (WidgetClass) &xmLabelClassRec);


    /*
     *  Initialize the toolkit.  This call returns the id of the "toplevel"
     *  widget.  The applications "main" widget must be the only child
     *  of this widget.
     */

    toplevel = XtInitialize(
	"Minimal Files",		/* application name */
	"helloworldclass",              /* application class */
	NULL, 0,                        /* options */
	&argc, argv);                   /* command line parameters */

    XtSetArg (arglist[0], XtNallowShellResize, TRUE) ;
    XtSetValues (toplevel, arglist, 1) ;

    /*
     *  Define the Mrm.hierarchy (only 1 file)
     */

    if (MrmOpenHierarchy (1,			    /* number of files	    */
			vec, 			    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_MrmHierarchy)	    /* ptr to returned id   */
			!= MrmSUCCESS) {
	printf ("can't open hierarchy\n");
     }

    /*
     * 	Register our callback routines so that the resource manager can 
     * 	resolve them at widget-creation time.
     */

    if (MrmRegisterNames (regvec, regnum)
			!= MrmSUCCESS)
			    printf("can't register names\n");

    /*
     *  Call MRM to fetch and create the pushbutton and its container
     */

    if (MrmFetchWidget (s_MrmHierarchy,
			"hellominimal_main",
			toplevel,
			&hellominimalmain,
			&class)
			!= MrmSUCCESS)
			    printf("can't fetch interface\n");

    /*
     *  Make the toplevel widget "manage" the main window (or whatever the
     *  the uil defines as the topmost widget).  This will
     *  cause it to be "realized" when the toplevel widget is "realized"
     */

    XtManageChild(hellominimalmain);
    
    /*
     *  Realize the toplevel widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(toplevel);

    /*
     *  Loop and process events
     */

    XtMainLoop();

    /* UNREACHABLE */
    return (0);
}

static void helloworld_button_activate( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    Arg arglist[2];

    static int call_count = 0;

    printf("In hello_world_button_activate\n");

    call_count += 1 ;
    switch ( call_count )
        {
        case 1:
            XtSetArg( arglist[0], XmNlabelString,
                XmStringLtoRCreate("Goodbye\nMinimal!",""));
            XtSetValues( widget, arglist, 1 );
            break ;
        case 2:
            exit(1);
            break ;
        }
}
