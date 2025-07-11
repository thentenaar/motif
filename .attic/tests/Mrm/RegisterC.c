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
static char rcsid[] = "$XConsortium: RegisterC.c /main/9 1995/07/14 10:52:08 drk $"
#endif
#endif

#include <MrmTest.h>
#include <testlib.h>

#include "Browse.h"

/*
 * Data for summary
 */

#define NUM_TESTS		2
#define TEST_DESCRIP_LENGTH	49

/*
 * Callbacks for Browse widget
 */

void		browse_help_proc();
void		browse_quit_proc();

/*
 * Convenience routine
 */

extern void	nyi_proc();


/*
 * Names to register with MrmRegisterNames
 */

static MrmRegisterArg reglist[] = 
{
    "browse_help_proc",		(caddr_t) browse_help_proc,
    "browse_quit_proc",		(caddr_t) browse_quit_proc,
};

/*
 * Local declarations
 */


struct TestInfo RegisterClassInfo[NUM_TESTS] = 
  {
    {"Register valid Motif widget class                ", MrmSUCCESS, 0},
    {"Fetch user-defined Motif widget (of valid class) ", MrmSUCCESS, 0},
  };


/*
 * External declarations
 */

extern String		summary();

Widget CreateDummy()
{
/* dummy */
}



void main (argc,argv)
int argc;
char **argv;
{ 	

    Arg		args[2];
    int		nargs;

    XmString	message;

    String	summary_msg;

    Screen		*screen;
    Colormap	cmap;

    Pixmap	images[5];
    int		num_images = 5;
    int		i;

    
    Pixel	image_background,
                image_foreground;

    Widget	browse = NULL;


    MrmHierarchy    s_MrmHierarchy;
    MrmType         class;
    String          filename_vec[1];
    char            uidname[80];
    String          testname;

    MrmCount        filename_num = 0;

    static int reglist_num =
      (sizeof(reglist) / sizeof(reglist[0]));




/*
 * Create top level shell widget as parent for test_box widget
 */

    CommonTestUilInit (argc,argv);
    MrmInitialize();


/*
 *  Open the UID hierarchy
 */

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
 * Register valid Browse widget class
 */
	RegisterClassInfo[0].actual_return =
	MrmRegisterClass(URMwcUnknown,
			 "Browse",
			 "XmCreateBrowse",
			 XmCreateBrowse,
			 browseWidgetClass);

/*
 * Register the names of the Browse widget callback routines with Mrm
 */
    
	if(MrmRegisterNames(reglist,		/* list of names	*/
			    reglist_num)	/* number of names	*/
	   != MrmSUCCESS)
	{
	    printf("Can't register Browse widget callbacks\n");
	}
    
/*
 * Get display, screen, colormap
 */

	display = XtDisplay(Shell1);
	
	screen = DefaultScreenOfDisplay(display);

	cmap = DefaultColormap(display, DefaultScreen(display));

/*
 * Fetch background and foreground colors for images
 */

	if(MrmFetchColorLiteral(s_MrmHierarchy,
				"yellow",
				display,
				cmap,
				&image_foreground) != MrmSUCCESS)
	{
	    printf("can't fetch the color yellow");
	}
    
	if(MrmFetchColorLiteral(s_MrmHierarchy,
				"red",
				display,
				cmap,
				&image_background) != MrmSUCCESS)
	{
	    printf("can't fetch the color red");
	}
    
/*
 * Fetch images (pixmaps)
 */
	
	if(MrmFetchIconLiteral(s_MrmHierarchy,
			       "blank_icon",
			       screen,
			       display,
			       image_foreground,
			       image_background,
			       &images[0]) != MrmSUCCESS)
	{
	    printf("can't fetch blank_icon");
	}
    
	if(MrmFetchIconLiteral(s_MrmHierarchy,
			       "test1_btn1_icon",
			       screen,
			       display,
			       image_foreground,
			       image_background,
			       &images[1]) != MrmSUCCESS)
	{
	    printf("can't fetch test1_btn1_icon");
	}
	
	if(MrmFetchIconLiteral(s_MrmHierarchy,
			       "test1_btn2_icon",
			       screen,
			       display,
			       image_foreground,
			       image_background,
			       &images[2]) != MrmSUCCESS)
	{
	    printf("can't fetch test1_btn2_icon");
	}
	
	if(MrmFetchIconLiteral(s_MrmHierarchy,
			       "test2_btn1_icon",
			       screen,
			       display,
			       image_foreground,
			       image_background,
			       &images[3]) != MrmSUCCESS)
	{
	    printf("can't fetch test2_btn1_icon");
	}
	
	if(MrmFetchIconLiteral(s_MrmHierarchy,
			       "test2_btn2_icon",
			       screen,
			       display,
			       image_foreground,
			       image_background,
			       &images[4]) != MrmSUCCESS)
	{
	    printf("can't fetch test2_btn2_icon");
	}
	
/*
 * Try to fetch Browse widget
 */

	RegisterClassInfo[1].actual_return = MrmFetchWidget(s_MrmHierarchy,
							    "browse",
							    Shell1,
							    &browse,
							    &class);

/*
 * Pass images to widget
 */

	nargs = 0;
	XtSetArg(args[nargs], XmNimages, images); nargs++;
	XtSetArg(args[nargs], XmNnumImages, num_images); nargs++;
	XtSetValues(browse, args, nargs);

/*
 * Manage Browse widget
 */

	XtManageChild(browse);
    
/*
 * Realize top level shell widget
 */

	XtRealizeWidget(Shell1);

    CommonPause();

    summary_msg = summary(NUM_TESTS,
			      TEST_DESCRIP_LENGTH,
			      RegisterClassInfo);
    
    fprintf (stdout, summary_msg); 

    CommonPause();

    /*
     * Test for CR 5573
     * MrmRegisterClass leaks memory if the same class
     * is registered more than once
     */
    for (i=0; i < 5; i++) {
	if (MrmRegisterClass((MrmType)0, NULL, "CreateDummy", CreateDummy, NULL)
	    != MrmSUCCESS)
	   printf("MrmRegisterClass failed for CreateDummy\n");
    }


    CommonPause();
	
}

/*
 * Browse Help procedure
 */

void browse_help_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{
  printf ("help callback invoked!\n");
}

/*
 * Browse Exit procedure
 */

void browse_quit_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{
  printf ("quit callback invoked!\n");
}
