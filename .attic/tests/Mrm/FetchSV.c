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
static char rcsid[] = "$XConsortium: FetchSV.c /main/9 1995/07/14 10:48:13 drk $"
#endif
#endif

#include <testlib.h>
#include <MrmTest.h> 

/* 
 * Data for summary
 */


#define NUM_TESTS		6
#define TEST_DESCRIP_LENGTH	64


/*
 * External declarations
 */

extern String		summary();


/*
 * Local declarations
 */

struct TestInfo FetchSetValueInfo[NUM_TESTS] = 
	{
	  {"Fetch undefined background color - Salmon                       ",
                                MrmNOT_FOUND,   0},
	  {"Fetch one valid value, shadow thickness for button2             ",
                                MrmSUCCESS,     0},
	  {"Fetch value from bad hierarchy, shadow thickness                ",
                                MrmBAD_HIERARCHY,       0},
	  {"Fetch two valid values, topshadow=blue, bottomshadow=black      ",
                                MrmSUCCESS,     0},
	  {"Fetch two values, one found, topshadow=white, bottomshadow=Coral",
                                MrmPARTIAL_SUCCESS,     0},
	  {"Fetch a value not valid for widget, selectColor for pushbutton  ",
                                MrmSUCCESS,     0},
 };



void main (argc,argv)
int argc;
char **argv;

{


      int		FSV_info_cnt = 0;
      Widget	test_box,
                buttons[3],
		badwidget1;

      Arg		args[4];
      int		nargs;

  
      String	        summary_msg;
      Pixel	        fg, bg;
      int		widgetwidth = 0;



      String      filename_vec[1];
      MrmCount    filename_num = 0;
      String      testname;

      char        uidname[80];
                

      MrmHierarchy        s_MrmHierarchy;
      MrmType             class;







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
 * Fetch valid test_box widget
 */
	test_box = NULL;
	MrmFetchWidget( s_MrmHierarchy,
			"test_box",
			Shell1,
			&test_box,
			&class);
/*
 * Fetch undefined background color - Salmon
 */
	nargs = 0;
	XtSetArg(args[nargs], XmNbackground, "Salmon"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						s_MrmHierarchy,
						buttons[0],
						args,
						nargs);
	FSV_info_cnt++;

/*
 * Fetch one valid value, shadow thickness for button2
 */
	buttons[0] = NULL;
	MrmFetchWidget(
				s_MrmHierarchy,
				"button2",
				test_box,
				&buttons[0],
				&class);
	nargs = 0;
	XtSetArg(args[nargs], XmNshadowThickness, "wideshadow"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						s_MrmHierarchy,
						buttons[0],
						args,
						nargs);
	FSV_info_cnt++;


	XtManageChild(test_box);
	XtManageChild(buttons[0]);
	XtRealizeWidget(Shell1);

/*
 * Fetch value from bad hierarchy, shadow thickness
 */
	nargs = 0;
	XtSetArg(args[nargs], XmNshadowThickness, "wideshadow"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						NULL,
						buttons[0],
						args,
						nargs);
	FSV_info_cnt++;

        CommonPause();

/*
 * Fetch second two button widgets
 */

/* 
 * Fetch two valid values, topshadow=blue, bottomshadow=black
 */
	buttons[1] = NULL;
        MrmFetchWidget(s_MrmHierarchy,
		       "button3",
			test_box,
			&buttons[1],
		        &class);
	nargs = 0;
	XtSetArg(args[nargs], XmNtopShadowColor, "blue"); nargs++;
	XtSetArg(args[nargs], XmNbottomShadowColor, "black"); nargs++;
	XtSetArg(args[nargs], XmNshadowThickness, "wideshadow"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						s_MrmHierarchy,
						buttons[1],
						args,
						nargs);
	FSV_info_cnt++;


/* 
 * Fetch two values, one found, topshadow=white, bottomshadow=Coral
 */
	buttons[2] = NULL;
	MrmFetchWidget( s_MrmHierarchy,
			"button4",
			test_box,
			&buttons[2],
			&class);
	nargs = 0;
	XtSetArg(args[nargs], XmNtopShadowColor, "white"); nargs++;
	XtSetArg(args[nargs], XmNbottomShadowColor, "Coral"); nargs++;
	XtSetArg(args[nargs], XmNshadowThickness, "wideshadow"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						s_MrmHierarchy,
						buttons[2],
						args,
						nargs);
	FSV_info_cnt++;

/* 
 * Fetch a value not valid for widget, selectColor for pushbutton
 */
	nargs = 0;
	XtSetArg(args[nargs], XmNselectColor, "white"); nargs++;
	FetchSetValueInfo[FSV_info_cnt].actual_return = MrmFetchSetValues(
						s_MrmHierarchy,
						buttons[2],
						args,
						nargs);
	FSV_info_cnt++;

/*
 * Manage button widgets
 */

	XtManageChildren(&buttons[1], 2);
	
        CommonPause();

/*
 * Replace the instruction message with test end message
 */

	summary_msg = summary(NUM_TESTS,
			      TEST_DESCRIP_LENGTH,
			      FetchSetValueInfo);

	fprintf (stdout, summary_msg);

        CommonPause();
}
