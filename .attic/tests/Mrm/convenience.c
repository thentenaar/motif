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
static char rcsid[] = "$XConsortium: convenience.c /main/8 1995/07/14 10:53:23 drk $"
#endif
#endif

#include <MrmTest.h>

/*
 * Structures declared in main.c file
 */

extern MrmHierarchy	s_MrmHierarchy;
extern Widget		widgetmain;
extern MrmType		class;

/*
 * Local declarations
 */

void	nyi_proc();

/*
 * Error handling
 */

void error_proc(error_string)
     String	error_string;
{
    fprintf(stdout,"%s\n", error_string);
    fflush(stdout);
}

/*
 * Exit
 */

void exit_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{
    fprintf(stdout,"Exiting\n");
    fflush(stdout);
    MrmCloseHierarchy(s_MrmHierarchy);
    exit(0);
}

/*
 * Help
 */

void help_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{
    nyi_proc(w, "help", NULL);
}

/*
 * Not Yet Implemented - fetches InformationDialog widget
 */

void nyi_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{

    Arg		args[2];
    int		nargs;
    char	buf[80];
    XmString	message;
    Widget	nyi = NULL;
    
    sprintf(buf, "The %s utility is not yet implemented", tag);
    message = XmStringCreateLtoR(buf, XmSTRING_DEFAULT_CHARSET);

    nargs = 0;
    XtSetArg(args[nargs], XmNmessageString, message); nargs++;

    if(MrmFetchWidgetOverride(s_MrmHierarchy,	/* id of uid hierarchy	   */
			      "nyi",		/* uil name of widget	   */
			      widgetmain,	/* parent of widget	   */
			      NULL,		/* override name	   */
			      args,		/* override arg list	   */
			      nargs,		/* number of override args */
			      &nyi,		/* ptr to widget id	   */
			      &class)		/* ptr to widget class	   */
       != MrmSUCCESS)
    {
	error_proc("Can't fetch nyi widget\n");
    }
    else
    {
	XtManageChild(nyi);
    }
}
