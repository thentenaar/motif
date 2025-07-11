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
static char rcsid[] = "$XConsortium: FetchIL.c /main/9 1995/07/14 10:47:29 drk $"
#endif
#endif

#include <MrmTest.h>
#include <testlib.h>

/*
 * Data for summary
 */

#define NUM_TESTS		7
#define TEST_DESCRIP_LENGTH	38



/*
 * External declarations
 */

extern String		summary();


/*
 * Local declarations
 */
   struct TestInfo FetchIconLiteralInfo[NUM_TESTS] = 
{
    {"Fetch non-exported icon              ", MrmNOT_FOUND,	0},
    {"Fetch nonexistent icon               ", MrmNOT_FOUND,	0},
    {"Fetch icon from nonexistent hierarchy", MrmBAD_HIERARCHY,	0},
    {"Fetch valid icon                     ", MrmSUCCESS,	0},
    {"Fetch valid icon                     ", MrmSUCCESS,	0},
    {"Fetch valid icon                     ", MrmSUCCESS,	0},
    {"Fetch valid icon                     ", MrmSUCCESS,	0},
};



void main (argc, argv)
int argc;
char **argv;
{ 	


    Arg		args[2];
    int		nargs;


    String	summary_msg;

    int		i;

    Colormap	cmap;

    Widget	button1 = NULL,
		button2 = NULL,
		test_box = NULL;


    String      testname;

    String	filename_vec[1];
    MrmCount	filename_num = 0;

    char	uidname[80];
    		

    MrmHierarchy	s_MrmHierarchy;
    MrmType	        class;




    Pixmap	btn1_pix,
		btn2_pix,
		bad_pix;

    Pixel	btn1_fg_color = 0L,
		btn1_bg_color = 0L,
		btn2_fg_color = 0L,
		btn2_bg_color = 0L,
		white,
		black;



    Screen		*screen;




  CommonTestUilInit (argc,argv);

  MrmInitialize();

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
 * Get screen
 */
	
	screen = DefaultScreenOfDisplay(display);

/*
 * Get pixels for white & black
 */

	white = WhitePixel(display, DefaultScreen(display));
	black = BlackPixel(display, DefaultScreen(display));
	
/*
 * Try to fetch an icon which is not exported
 */

	FetchIconLiteralInfo[0].actual_return = 
	MrmFetchIconLiteral(s_MrmHierarchy,	/* id of uid hierarchy	*/
			    "bad_test1_icon",	/* uil name of widget	*/
			    screen,		/* screen		*/
			    display,		/* display		*/
			    white,		/* foreground color	*/
			    black,		/* background color	*/
			    &bad_pix);		/* pixmap		*/
	
/*
 * Try to fetch an icon which is nonexistent
 */

	FetchIconLiteralInfo[1].actual_return =
	MrmFetchIconLiteral(s_MrmHierarchy,
			    "bad_test2_icon",
			    screen,
			    display,
			    white,
			    black,
			    &bad_pix);
	
/*
 * Try to fetch an icon from a NULL hierarchy
 */

	FetchIconLiteralInfo[2].actual_return =
	MrmFetchIconLiteral(NULL,
			    "test1_btn1_icon",
			    screen,
			    display,
			    white,
			    black,
			    &bad_pix);


  



/*
 * Fetch test_box widget
 */

	if(MrmFetchWidget(s_MrmHierarchy,	/* id of uid hierarchy	   */
			  "test_box",		/* uil name of widget	   */
			  Shell1,		/* parent of widget	   */
			  &test_box,		/* ptr to widget id	   */
			  &class)		/* ptr to widget class	   */
	   != MrmSUCCESS)
	{
	    printf("Can't fetch test_box widget\n");
	}

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
 * Get ID's of button widgets
 */

	button1 = XtNameToWidget(test_box, "button1");
	button2 = XtNameToWidget(test_box, "button2");

/*
 * Get background and foreground colors for button widgets
 */

	nargs = 0;
	XtSetArg(args[nargs], XmNbackground, &btn1_bg_color ); nargs++;
	XtSetArg(args[nargs], XmNforeground, &btn1_fg_color ); nargs++;
	XtGetValues(button1, args, nargs);
	
	nargs = 0;
	XtSetArg(args[nargs], XmNbackground, &btn2_bg_color ); nargs++;
	XtSetArg(args[nargs], XmNforeground, &btn2_fg_color ); nargs++;
	XtGetValues(button2, args, nargs);
	
/*
 * Fetch the icons for each button for valid test 1
 */

	FetchIconLiteralInfo[3].actual_return =
	MrmFetchIconLiteral(s_MrmHierarchy,
			    "test1_btn1_icon",
			    screen,
			    display,
			    btn1_fg_color,
			    btn1_bg_color,
			    &btn1_pix);
	
	FetchIconLiteralInfo[4].actual_return =
	MrmFetchIconLiteral(s_MrmHierarchy,
			    "test1_btn2_icon",
			    screen,
			    display,
			    btn2_fg_color,
			    btn2_bg_color,
			    &btn2_pix);
	

/*
 * Replace the labels for both buttons with the valid test 1 icons
 */

	nargs = 0;
	XtSetArg(args[nargs], XmNlabelPixmap, btn1_pix); nargs++;
	XtSetValues(button1, args, nargs);
			       
	nargs = 0;
	XtSetArg(args[nargs], XmNlabelPixmap, btn2_pix); nargs++;
	XtSetValues(button2, args, nargs);
			       

	CommonPause();

/*
 * Fetch the icons for each button for valid test 2
 */

	FetchIconLiteralInfo[5].actual_return =
	MrmFetchIconLiteral(s_MrmHierarchy,
			    "test2_btn1_icon",
			    screen,
			    display,
			    btn1_fg_color,
			    btn1_bg_color,
			    &btn1_pix);
	
	FetchIconLiteralInfo[6].actual_return =
	MrmFetchIconLiteral(s_MrmHierarchy,
			    "test2_btn2_icon",
			    screen,
			    display,
			    btn2_fg_color,
			    btn2_bg_color,
			    &btn2_pix);
	
/*
 * Replace the labels for both buttons with the valid test 1 icons
 */

	nargs = 0;
	XtSetArg(args[nargs], XmNlabelPixmap, btn1_pix); nargs++;
	XtSetValues(button1, args, nargs);
			       
	nargs = 0;
	XtSetArg(args[nargs], XmNlabelPixmap, btn2_pix); nargs++;
	XtSetValues(button2, args, nargs);
			       
	
    CommonPause();


	summary_msg = summary(NUM_TESTS,
			      TEST_DESCRIP_LENGTH,
			      FetchIconLiteralInfo);

	fprintf (stdout, summary_msg);

   CommonPause();

}

