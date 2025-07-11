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
static char rcsid[] = "$XConsortium: main.c /main/8 1995/07/14 10:53:35 drk $"
#endif
#endif

#include <MrmTest.h>

/*
 * Callback routines
 */

extern void	continue_proc();
extern void	exit_proc();
extern void	help_proc();
extern void	nyi_proc();

/*
 * Convenience routines
 */

extern void	error_proc();

/*
 * Names to register with MrmRegisterNames
 */

static MrmRegisterArg reglist[] = 
{
    "continue_proc",	(caddr_t) continue_proc,
    "exit_proc",	(caddr_t) exit_proc,
    "help_proc",	(caddr_t) help_proc,
    "nyi_proc",		(caddr_t) help_proc,
};

static int reglist_num =
(sizeof(reglist) / sizeof(reglist[0]));

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;

Widget		toplevel = NULL,
		widgetmain = NULL;

String		testname;

/*
 * Main routine
 */

main(argc, argv)
     int argc;
     char *argv[];
{
    Arg		args[10];
    int		nargs;

    XmString	message;
    
    String	filename_vec[1];
    MrmCount	filename_num = 0;

    char	message_begin[120],
    		uidname[80];

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.
 */

    testname = argv[0];

    toplevel = XtInitialize(testname,		/* application name	   */
			    "genericclass",	/* application class	   */
			    NULL, 0,		/* options		   */
			    &argc, argv);	/* command line parameters */

    nargs = 0;
    XtSetArg(args[nargs], XmNallowShellResize, TRUE); nargs++;
    XtSetValues(toplevel, args, nargs) ;

/*
 *  Open the UID hierarchy
 */

    sprintf(uidname, "%s.uid", testname);
    filename_vec[filename_num] = uidname; filename_num++;

    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy)	/* ptr to returned id	*/
       != MrmSUCCESS) 
    {
	error_proc("Can't open hierarchy\n");
    }

/*
 * Register the names of the callback routines with Mrm
 */
    
    if(MrmRegisterNames(reglist,	/* list of names	*/
			reglist_num)	/* number of names	*/
       != MrmSUCCESS)
    {
	error_proc("Can't register names\n");
    }
    
/*
 *  Fetch and create the topmost widget and any children.  For these tests,
 * this will always be an XmMessageBox widget.
 */

/*
 * String to be displayed.
 */

    sprintf(message_begin, "This is the %s test.\nPress the continue button to start the test.", argv[0]);

    message = XmStringCreateLtoR(message_begin, XmSTRING_DEFAULT_CHARSET);
    
    nargs = 0;
    XtSetArg(args[nargs], XmNmessageString, message); nargs++;

    if(MrmFetchWidgetOverride(s_MrmHierarchy,	/* id of uid hierarchy	   */
			      "S_MAIN_WIDGET",	/* uil name of widget	   */
			      toplevel,		/* parent of widget	   */
			      NULL,		/* override name	   */
			      args,		/* override arg list	   */
			      nargs,		/* number of override args */
			      &widgetmain,	/* ptr to widget id	   */
			      &class)		/* ptr to widget class	   */
       != MrmSUCCESS)
    {
	error_proc("Can't fetch main widget\n");
    }

/*
 *  Manage the topmost widget in the UID hierarchy.
 */

    XtManageChild(widgetmain);
    
/*
 *  Realize the toplevel widget.
 */

    XtRealizeWidget(toplevel);

/*
 *  Loop and process events.
 */

    XtMainLoop();
}
