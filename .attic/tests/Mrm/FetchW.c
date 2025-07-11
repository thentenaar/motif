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
static char rcsid[] = "$XConsortium: FetchW.c /main/9 1995/07/14 10:48:34 drk $"
#endif
#endif

#include <MrmTest.h>
#include <testlib.h>

/*
 * Data for summary
 */

#define NUM_TESTS		5
#define TEST_DESCRIP_LENGTH	41



/*
 * External declarations
 */




extern String		summary();

/*
 * Local declarations
 */

struct TestInfo FetchWidgetInfo[NUM_TESTS] = 
{
    {"Fetch valid widget                       ", MrmSUCCESS,	0},
    {"Fetch nonexistent widget                 ", MrmNOT_FOUND,	0},
    {"Fetch previously fetched widget          ", MrmSUCCESS,	0},
    {"Fetch valid widget                       ", MrmSUCCESS,	0},
    {"Fetch valid widget                       ", MrmSUCCESS,	0},
};




void main(argc,argv)
       int argc;
       char **argv;

{ 	


MrmHierarchy	s_MrmHierarchy;
MrmType		class;

Widget		test_top = NULL,
		test_box = NULL,
		button1 = NULL,
		button2 = NULL,
		badwidget1 = NULL,
		badwidget2 = NULL;

String      filename_vec[1];
char        uidname[80];
String      testname;

Arg	    args[2];
int	    nargs;

String	    summary_msg;


MrmCount    filename_num=0;







   MrmInitialize();
   CommonTestUilInit (argc,argv);


   testname = argv[0];
   sprintf(uidname, "%s.uid", testname);
   filename_vec[filename_num] = uidname; filename_num++;

    if(MrmOpenHierarchy(filename_num,           /* number of files      */
                        filename_vec,           /* files                */
                        NULL,                   /* os_ext_list (null)   */
                        &s_MrmHierarchy)        /* ptr to returned id   */
       != MrmSUCCESS) 
    {
        printf("Can't open hierarchy\n");

      }

CommonPause();

/*
 * Fetch vaild test_box widget
 */
	FetchWidgetInfo[0].actual_return = MrmFetchWidget(s_MrmHierarchy,
							  "test_box",
							  Shell1,
							  &test_box,
							  &class);

/*
 * Manage button widgets
 */

	XtManageChild(test_box);
    
/*
 * Realize top level shell widget
 */

	XtRealizeWidget(Shell1);

   CommonPause();			       
/*
 * Try to fetch a nonexistent widget
 */

	FetchWidgetInfo[1].actual_return = MrmFetchWidget(s_MrmHierarchy,
							  "badwidget1",
							  Shell1,
							  &badwidget1,
							  &class);



/*
 * Fetch second two button widgets
 */

	FetchWidgetInfo[3].actual_return = MrmFetchWidget(s_MrmHierarchy,
							  "button3",
							  test_box,
							  &button1,
							  &class);

	FetchWidgetInfo[4].actual_return = MrmFetchWidget(s_MrmHierarchy,
							  "button4",
							  test_box,
							  &button2,
							  &class);
/*
 * Try to fetch a widget which has already been fetched
 */

	badwidget1 = NULL;
	badwidget1 = XtNameToWidget(test_box, "button1");
	
	FetchWidgetInfo[2].actual_return = MrmFetchWidget(s_MrmHierarchy,
							  "button1",
							  Shell1,
							  &badwidget1,
							  &class);

/*
 * Manage button widgets
 */

	XtManageChild(button1);
	XtManageChild(button2);
	
  CommonPause();

/*
 * Replace the instruction message with test end message
 */

	summary_msg = summary(NUM_TESTS,
			      TEST_DESCRIP_LENGTH,
			      FetchWidgetInfo);

	fprintf (stdout, summary_msg);

	CommonPause();
}
