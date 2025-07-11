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
static char rcsid[] = "$XConsortium: Values.c /main/4 1995/07/13 20:35:28 drk $"
#endif
#endif

/*      This Test will produce failures. The reason for this is that    */
/*      indentifiers and values pass different types. Identifiers       */
/*      pass Pointer to an integer and values pass the integer value    */
/*	to MrmFetchLiteral. Pir 2089.					*/

#include <stdio.h>

#include <Mrm/MrmAppl.h>

#define EXPECTED_VALUE		1
#define EXPECTED_IDENTIFIER	1

/*
 * Name of uid file
 */

String		filename_vec[] =
{
    "Values.uid"
};

MrmCount	filename_num =
(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Convenience routine
 */

void		error_proc();
void            create_proc_iden();
void            create_proc_value();

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;
MrmCode		type;

Widget		toplevel = NULL;
Widget          widgetmain = NULL;

Display		*dsp;
Colormap	cmap;

int		return_value;

/* MRMRegisterArg idList; */
static MrmRegisterArg reglist[] = {
  {"CBdata0", (caddr_t)1},
  {"create_proc_iden", (caddr_t)create_proc_iden},
  {"create_proc_value", (caddr_t)create_proc_value}
};

static int reglist_num = (sizeof (reglist) / sizeof (reglist[0]));

main(argc, argv)
     int argc;
     char *argv[];
{ 	
    int	i,
    	errors = 0;

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.  This call returns the id of the "toplevel" widget.
 */

    toplevel = XtInitialize("Simple",		/* application name	   */
			    "genericclass",	/* application class	   */
			    NULL, 0,		/* options		   */
			    &argc, argv);	/* command line parameters */

/*
 *  Open the Mrm hierarchy
 */

    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy)	/* ptr to returned id	*/
       != MrmSUCCESS) 
    {
	error_proc("Can't open hierarchy\n");
    }

    /*
     *		Register all Mrm functions
     */	


    if (MrmRegisterNames (reglist, reglist_num)
			!= MrmSUCCESS)
			{
			    printf("Can't register names\n");
			    exit(1);
			}

/*
 * Get display, colormap
 */

    dsp = XtDisplay(toplevel);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));

/*
 * Print header for test results
 */

    fprintf(stdout,"\n\nValue Test Results:\n\n");
    fprintf(stdout,"         Test\t  Identifier \t    Value    \tStatus\n");
    fprintf(stdout,"         ----\t-------------\t-------------\t-----\n");

/*
 * Fetch UIL Boolean literals and test against equivalent Xt value
 */
	if(MrmFetchLiteral(s_MrmHierarchy,
			   "CBdata1",
			   dsp,
			   (XtPointer ) &return_value,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch the value literal");
	}



        if (MrmFetchWidget (s_MrmHierarchy,
			"S_MAIN_WIDGET",
			toplevel,
			&widgetmain,
			&class)
			!= MrmSUCCESS)
			{
			    printf("Can't fetch interface\n");
			    exit(1);
			}

/*
 * Close Mrm Hierarchy
 */

    MrmCloseHierarchy(s_MrmHierarchy);

}

/*
 * Error handling
 */

void error_proc(error_string)
     String	error_string;
{
    fprintf(stdout,"Mrm error: %s\n", error_string);
    fflush(stdout);
}


void create_proc_value(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

{

int x;

x =  *(int *)client_data;

fprintf (stdout, "%13s\t%13d\t%13d\t%s", "Comparison", EXPECTED_VALUE, x, 
                          ((EXPECTED_VALUE == x) ? "Passed\n" : "Failed\n"));

}

void create_proc_iden(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;

{

int x;

x = (int) client_data;

fprintf (stdout, "%13s\t%13d\t%13d\t%s", "Comparison", EXPECTED_IDENTIFIER, x, 
                       ((EXPECTED_IDENTIFIER == x) ? "Passed\n" : "Failed\n"));

}

