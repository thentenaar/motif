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
static char rcsid[] = "$XConsortium: HierN.c /main/8 1995/07/14 10:49:34 drk $"
#endif
#endif
#include <MrmTest.h>

/*
 * This is an adaption of RegisterN which tests use of hierarchy-based
 * name registration. It does not attempt to test all variations of
 * registration as RegisterN does; it confirms that names registered in
 * a hierarchy can only be found in that hierarchy.
 */

/*
 * Name of uid file
 */
String		filename_vec[] =
{
    "RegisterN.uid"
};

MrmCount	filename_num =
(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Callback routines
 */

void		test1_1_proc(),
		test1_2_proc(),
		test2_1_proc(),
		test2_2_proc();

/*
 * Convenience routine
 */

void		error_proc();

/*
 * Names registered globally
 */
static MrmRegisterArg reglist_1[] = 
{
    "test1_proc",	(caddr_t) test1_1_proc,
    "test5_var",	(caddr_t) 5
};
static int reglist_1_num =
(sizeof(reglist_1) / sizeof(reglist_1[0]));

/*
 * Names registered in hierarchy 1
 */
static MrmRegisterArg reglist_2[] = 
{
    "test2_proc",	(caddr_t) test2_1_proc,
    "test3_var",	(caddr_t) 0
};
static int reglist_2_num =
(sizeof(reglist_2) / sizeof(reglist_2[0]));

/*
 * Names registered in hierarchy 2
 */
static MrmRegisterArg reglist_3[] = 
{
    "test4_var",	(caddr_t) 4
};
static int reglist_3_num =
(sizeof(reglist_3) / sizeof(reglist_3[0]));


/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy1;
MrmHierarchy	s_MrmHierarchy2;
MrmType		class;
MrmCode		*type;

void	test1_1_proc(),
	test2_1_proc(),
	test1_2_proc(),
	test2_2_proc();

int	test3_var,
	test4_var,
	test5_var;

String	test6_var;

int	test1_1_called = 0,
	test2_1_called = 0,
	test1_2_called = 0,
	test2_2_called = 0;

int	test3_1_expect = 0,
	test4_1_expect = 4,
	test5_1_expect = 5,
	test3_2_expect = 999,
	test4_2_expect = 888,
	test5_2_expect = 777;

short	test3_1_return,
	test4_1_return,
	test5_1_return,
	test3_2_return,
	test4_2_return,
	test5_2_return;

int	errors = 0,
	total_errors = 0;

/*
 * Main routine
 */

main(argc, argv)
     int argc;
     char *argv[];
{
    Arg		args[10];
    int		nargs;

    Widget	toplevel = NULL,
    		widgetmain_1 = NULL,
		widgetmain_2 = NULL;

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.
 */

    toplevel = XtInitialize("HierNames",	/* application name        */
			    "genericclass",	/* application class	   */
			    NULL, 0,		/* options		   */
			    &argc, argv);	/* command line parameters */

    nargs = 0;
    XtSetArg(args[nargs], XtNallowShellResize, TRUE); nargs++;
    XtSetValues(toplevel, args, nargs) ;

/*
 *  Open the Mrm hierarchies; both use the same file.
 */
    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy1)	/* ptr to returned id	*/
       != MrmSUCCESS) 
	{
	error_proc("Can't open hierarchy 1\n");
	}
    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy2)	/* ptr to returned id	*/
       != MrmSUCCESS) 
	{
	error_proc("Can't open hierarchy 2\n");
	}

/*
 * Register the names of the callback routines and variables with Mrm
 */
    if(MrmRegisterNames(reglist_1,	/* list of names	*/
			reglist_1_num)	/* number of names	*/
       != MrmSUCCESS)
	{
	error_proc("Can't register names\n");
	}
    if(MrmRegisterNamesInHierarchy(s_MrmHierarchy1,
				   reglist_2,
				   reglist_2_num)
       != MrmSUCCESS)
	{
	error_proc("Can't register names in hierarchy 1\n");
	}
    if(MrmRegisterNamesInHierarchy(s_MrmHierarchy2,
				   reglist_3,
				   reglist_3_num)
       != MrmSUCCESS)
	{
	error_proc("Can't register names in hierarchy 2\n");
	}

/*
 * Fetch widget with callbacks test1_1_proc() and test2_1_proc() in
 * each hierarchy. hierarchy 1 should have test1 and not test2, and
 * hierarchy 2 should have test2 and not test1.
 */
    fprintf(stdout,
	    "MrmFetchWidget for hierarchy 1: should produce warnings:\n");
    fprintf(stdout,"\t test4_var - MrmNOT_FOUND\n");
    fprintf(stdout,"\n");
    if(MrmFetchWidget(s_MrmHierarchy1,	/* id of uid hierarchy	   */
		      "S_MAIN_WIDGET_1",/* uil name of widget	   */
		      toplevel,		/* parent of widget	   */
		      &widgetmain_1,	/* ptr to widget id	   */
		      &class)		/* ptr to widget class	   */
       != MrmSUCCESS)
	{
	error_proc("Can't fetch main widget\n");
	}
    fprintf(stdout,"\n\n");

    fprintf(stdout,
	    "MrmFetchWidget for hierarchy 2: should produce warnings:\n");
    fprintf(stdout,"\t test3_var - MrmNOT_FOUND\n");
    fprintf(stdout,"\t test2_proc - MrmNOT_FOUND\n");
    fprintf(stdout,"\n");
    if(MrmFetchWidget(s_MrmHierarchy2,	/* id of uid hierarchy	   */
		      "S_MAIN_WIDGET_2",/* uil name of widget	   */
		      toplevel,		/* parent of widget	   */
		      &widgetmain_2,	/* ptr to widget id	   */
		      &class)		/* ptr to widget class	   */
       != MrmSUCCESS)
	{
	error_proc("Can't fetch main widget\n");
	}
    fprintf(stdout,"\n\n");

/*
 * Call the callback routines for widgetmain_1. test1 should exist globally,
 * and test2 should exist in hierarchy 1.
 */
    test1_1_called = 0;
    test2_1_called = 0;
    XtCallCallbacks(widgetmain_1, XmNactivateCallback, NULL);
    XtCallCallbacks(widgetmain_1, XmNarmCallback, NULL);
    
/*
 * See if test1_1_proc() and test2_1_proc() were called
 */
    if ( !test1_1_called )
	{
	errors++;
	fprintf(stdout, "HierN: failed to properly register callback routine test1_1_proc in hierarchy 1.\n");
	}
    if( !test2_1_called )
	{
	errors++;
	fprintf(stdout, "HierN: failed to properly register callback routine test2_1_proc() in hierarchy 1.\n");
	}
    
/*
 * Call the callback routines for widgetmain_2. test1 should still exist,
 * and test2 should not.
 */
    test1_1_called = 0;
    test2_1_called = 0;
    XtCallCallbacks(widgetmain_2, XmNactivateCallback, NULL);
    XtCallCallbacks(widgetmain_2, XmNarmCallback, NULL);
    
/*
 * See if test1_1_proc() and test2_1_proc() were called
 */
    if ( !test1_1_called )
	{
	errors++;
	fprintf(stdout, "HierN: failed to properly register callback routine test1_1_proc in hierarchy 2.\n");
	}
    if( test2_1_called )
	{
	errors++;
	fprintf(stdout, "HierN: failed to properly register callback routine test2_1_proc() in hierarchy 2.\n");
	}
    
/*
 * Get the values associated with widgetmain_1 which were set using the
 * values of test[345]_var
 */

    nargs = 0;
    XtSetArg(args[nargs], XmNmarginBottom, &test3_1_return ); nargs++;
    XtSetArg(args[nargs], XmNmarginTop, &test4_1_return ); nargs++;
    XtSetArg(args[nargs], XmNmarginLeft, &test5_1_return ); nargs++;
    XtGetValues(widgetmain_1, args, nargs);
	
/*
 * See if the values were set correctly. test3 and test5 should have
 * been fetched.
 */

    if(test3_1_return != test3_1_expect)
	{
	errors++;
	fprintf(stdout,"HierN: failed to properly register literal test3_1_var.\n");
	}
    
    if(test4_1_return == test4_1_expect)
	{
	errors++;
	fprintf(stdout,"HierN: failed to properly register literal test4_1_var.\n");
	}

    if(test5_1_return != test5_1_expect)
	{
	errors++;
	fprintf(stdout,"HierN: failed to properly register literal test5_1_var.\n");
	}

/*
 * Print total number of errors for part 1 of this test
 */

    if(errors)
    {
	total_errors += errors;

	fprintf(stdout, "\nPart 1 of the HierN test failed with %d errors\n\n",
		errors);
    }

    if(total_errors)
    {
	fprintf(stdout, "HierN: test failed.\n");
    }
    else
    {
	fprintf(stdout, "HierN: all tests passed.\n");
    }
}

/*
 * Callback routines
 */

void test1_1_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{ 	
    test1_1_called++;
}

void test2_1_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{ 	
    test2_1_called++;
}

void test1_2_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{ 	
    test1_2_called++;
}

void test2_2_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{ 	
    test2_2_called++;
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

