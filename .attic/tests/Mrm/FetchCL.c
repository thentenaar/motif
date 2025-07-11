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
static char rcsid[] = "$XConsortium: FetchCL.c /main/9 1995/07/14 10:46:51 drk $"
#endif
#endif

#include <testlib.h>
#include <MrmTest.h>

/*
 * Data for summary
 */

#define NUM_TESTS		7
#define TEST_DESCRIP_LENGTH	38


struct TestInfo FetchColorLiteralInfo[NUM_TESTS] = 
{
    {"Fetch non-exported color              ", MrmNOT_FOUND,	0},
    {"Fetch nonexistent color               ", MrmNOT_FOUND,	0},
    {"Fetch color from nonexistent hierarchy", MrmBAD_HIERARCHY,0},
    {"Fetch valid color                     ", MrmSUCCESS,	0},
    {"Fetch valid color                     ", MrmSUCCESS,	0},
    {"Fetch valid color                     ", MrmSUCCESS,	0},
    {"Fetch valid color                     ", MrmSUCCESS,	0},
};




/*
 * External declarations
 */

extern String		summary();


void main (argc, argv)
int argc;
char **argv;
{ 	


    Arg		args[2];
    int		nargs;

    Pixel	blue,
		white,
    		red,
    		yellow,
    		bad_color;


    String	summary_msg;

    int		i;

    Colormap	cmap;

    Widget	button1 = NULL,
		button2 = NULL,
		test_box = NULL;


    String testname;

    String	filename_vec[1];
    MrmCount	filename_num = 0;

    char	uidname[80];
    		

    MrmHierarchy	s_MrmHierarchy;
    MrmType	        class;





    CommonTestUilInit (argc,argv);

    MrmInitialize();


   /* tell the user what test it is. */

    

    testname = argv[0];
    sprintf(uidname, "%s.uid", testname);
    filename_vec[filename_num] = uidname; filename_num++;

    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy)	/* ptr to returned id	*/
       != MrmSUCCESS) 
    {
	printf("Can't open hierarchy\n");
    }

    
    CommonPause();



/*
 * Get display, colormap
 */

		
	cmap = DefaultColormap(display, DefaultScreen(display));

/*
 * Try to fetch a color which is not exported
 */

	FetchColorLiteralInfo[0].actual_return = 
	MrmFetchColorLiteral(s_MrmHierarchy,
			    "bad_color1",
			     display,
			     cmap,
			     &bad_color);
	
/*
 * Try to fetch a color which is nonexistent
 */

	FetchColorLiteralInfo[1].actual_return = 
	MrmFetchColorLiteral(s_MrmHierarchy,
			    "bad_color2",
			     display,
			     cmap,
			     &bad_color);
	
/*
 * Try to fetch a color from a NULL hierarchy
 */

	FetchColorLiteralInfo[2].actual_return = 
	MrmFetchColorLiteral(NULL,
			    "red",
			     display,
			     cmap,
			     &bad_color);

/*
 * Fetch test_box widget
 */




    if(MrmFetchWidget         (s_MrmHierarchy,	/* id of uid hierarchy	   */
			      "test_box",	/* uil name of widget	   */
			      Shell1,		/* parent of widget	   */
			      &test_box,	/* ptr to widget id	   */
			      &class)		/* ptr to widget class	   */
       != MrmSUCCESS)
      {
	printf ("Can't fetch test_box widget!\n");
      }


/*
 * Get ID's of button widgets
 */

	button1 = XtNameToWidget(test_box, "button1");
	button2 = XtNameToWidget(test_box, "button2");

/*
 * Fetch the colors for the buttons for valid test 1
 */

	FetchColorLiteralInfo[3].actual_return =
	MrmFetchColorLiteral(s_MrmHierarchy,
			     "red",
			     display,
			     cmap,
			     &red);
	
	FetchColorLiteralInfo[4].actual_return =
	MrmFetchColorLiteral(s_MrmHierarchy,
			     "yellow",
			     display,
			     cmap,
			     &yellow);

/*
 * Set background and foreground colors for button widgets
 */

	nargs = 0;
	XtSetArg(args[nargs], XmNbackground, red); nargs++;
	XtSetArg(args[nargs], XmNforeground, yellow); nargs++;
	XtSetValues(button1, args, nargs);
	XtSetValues(button2, args, nargs);

/*
 * Manage test_box widget
 */

	XtManageChild(test_box);

/*
 * Realize top level shell widget
 */

	XtRealizeWidget(Shell1);

CommonPause();


/*
 * Fetch the colors for the buttons for valid test 1
 */

	FetchColorLiteralInfo[5].actual_return =
	MrmFetchColorLiteral(s_MrmHierarchy,
			     "blue",
			     display,
			     cmap,
			     &blue);
	
	FetchColorLiteralInfo[6].actual_return =
	MrmFetchColorLiteral(s_MrmHierarchy,
			     "white",
			     display,
			     cmap,
			     &white);

/*
 * Set background and foreground colors for button widgets
 */

	nargs = 0;

	XtSetArg(args[nargs], XmNbackground, blue ); nargs++;
	XtSetArg(args[nargs], XmNforeground, white ); nargs++;
	XtSetValues(button1, args, nargs);
	XtSetValues(button2, args, nargs);
	
CommonPause();

/*
 * Replace the instruction message with test end message
 */

	summary_msg = summary(NUM_TESTS,
			      TEST_DESCRIP_LENGTH,
			      FetchColorLiteralInfo);

	fprintf (stdout, summary_msg);


CommonPause();
}







