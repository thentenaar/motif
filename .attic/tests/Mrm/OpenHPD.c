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
static char rcsid[] = "$XConsortium: OpenHPD.c /main/5 1995/07/14 10:50:26 drk $"
#endif
#endif

#include <MrmTest.h> 

/* 
 * Data for summary
 */

#define NUM_TESTS		9
#define TEST_DESCRIP_LENGTH	60

struct TestInfo OpenHierarchyInfo[NUM_TESTS] = 
{
    {"Open a valid hierarchy with complete relative pathname      ",
				MrmSUCCESS,	0},
    {"Open a valid hierarchy from three different files           ",
				MrmSUCCESS,	0},
    {"Open a valid hierarchy using $UIDPATH with 1 element        ",
				MrmSUCCESS,	0},
    {"Open a valid hierarchy using $UIDPATH with >1 elements      ",
				MrmSUCCESS,	0},
    {"Open a nonexistent filename                                 ",
				MrmNOT_FOUND,	0},
    {"Open an invalid file (not a .uid file)                      ",
				MrmFAILURE,	0},
    {"Open a file/hierarchy by referring to it WITH a .uid suffix ",
				MrmSUCCESS,	0},
    {"Open a hierarchy from three files with one bad .uid file    ",
				MrmNOT_FOUND,	0},
    {"Open a hierarchy from three files with one nonexistent file ",
				MrmNOT_FOUND,	0}
};
int			OH_info_cnt = 0;

/*
 * External declarations
 */

extern MrmHierarchy	s_MrmHierarchy;
extern MrmType		class;

extern Widget		toplevel,
					widgetmain;

extern String		summary();

/*
 * Local declarations
 */

#define VALID_TEST1	1
#define VALID_TEST2	2
#define FINISH		3

#define	MEM_LOOP_CNT	10

Widget		test_top,
			test_box,
			OH_test[8];
int		OH_test_cnt = 0;
int		iteration = 1;
Display *display;

FILE	*OH_logfile;
#define	LOGFILE_NAME	"OH_log"

void continue_proc(w, tag, reason)
     Widget		w;
     int		*tag;
     unsigned long	*reason;
{ 	

    String	msg_valid_tst1 = "There should now be four label widgets.\nThe first should say\n\"OpenH_vh is completed.\"\nThe next three should say\n\"OpenH_mf1 is completed.\"\n\"OpenH_mf2 is completed.\"\n\"OpenH_mf3 is completed.\"\n\nWARNING: The next phase of this test may perform\na VERY LARGE number of disk file operations.\nDO NOT CONTINUE if this will disrupt other people's work!\n\nPress the continue button to continue the test.";

    String	msg_valid_tst2 = "There should now be two new label widgets.\nThe first should say\n\"OpenH_vhev is completed.\"\nand the second should say\n\"OpenH_vhws is completed.\"\nThe other three label widgets should remain unchanged.\nThey should say\n\"OpenH_mf1 is completed.\"\n\"OpenH_mf2 is completed.\"\n\"OpenH_mf3 is completed.\"\n\nNote that there should be a large number of Mrm Toolkit Warning\nmessages on stdout/stderr about MrmFAILUREs.\n\nPress the continue button to continue the test.";

    Arg		args[4];
    int		nargs;

    XmString	message;

    String	summary_msg;

	MrmHierarchy	OpenHierarchy[3];
	String		OH_filename_vec[3];
	MrmCount	OH_filename_num = 0;

	char	uidname[3][80];
	int		fetch_return;
	int		i;


	display = XtDisplay(widgetmain);

    if(iteration == VALID_TEST1)
    {
		iteration++;
	
	/*
	 * Create top level shell widget as parent for test_box widget
	 */
		nargs = 0;
		XtSetArg(args[nargs], XtNallowShellResize, TRUE); nargs++;
	
		test_top =
		XtAppCreateShell("test_top",			/* application class */
				 "TEST_TOP",					/* application name  */
				 topLevelShellWidgetClass,		/* widget class	     */
				 XtDisplay(widgetmain),			/* display		     */
				 args, nargs);					/* arguments	     */
	
	/*
	 * Fetch valid test_box widget
	 */
		test_box = NULL;
		MrmFetchWidget(
					s_MrmHierarchy,
					"test_box",
					test_top,
					&test_box,
					&class);
	
		XtManageChild(test_box);
		XtRealizeWidget(test_top);

	/*
	 * Open a valid hierarchy with a full local path name
	 */
		sprintf(uidname[0], "OpenH_vh");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
	
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_vh",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Open a valid hierarchy from three different files
	 */
		OH_filename_num = 0;
		sprintf(uidname[0], "OpenH_mf1");
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
		sprintf(uidname[1], "OpenH_mf2");
		OH_filename_vec[OH_filename_num] = uidname[1]; OH_filename_num++;
		sprintf(uidname[2], "OpenH_mf3");
		OH_filename_vec[OH_filename_num] = uidname[2]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
	
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_mf1",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_mf2",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_mf3",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Replace the instruction message with "valid test 1" message
	 */
	
		message = XmStringCreateLtoR(msg_valid_tst1, XmSTRING_DEFAULT_CHARSET);
	
		nargs = 0;
		XtSetArg(args[nargs], XmNmessageString, message); nargs++;
		XtSetValues(widgetmain, args, nargs);
		XmStringFree(message);
	}
    else if(iteration == VALID_TEST2)
    {
		iteration++;
	
	/*
	 * Open a valid hierarchy using the environment variable UIDPATH to
	 * help decipher the name.
	 */

	/* set up the $UIDPATH var */
		putenv("UIDPATH=./OpenHLang/%U\
%S");			

		sprintf(uidname[0], "OpenH_vhev");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_vhev",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;

	/*
	 * Open a valid hierarchy using the environment variable UIDPATH to
	 * help decipher the name.  UIDPATH has more than one element in it.
	 */

  	 /* set up the $UIDPATH var */
		putenv("UIDPATH=./OpenHLang/%U:./OpenHLang/%U\
%S");	

		sprintf(uidname[0], "OpenH_evl");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_vhevl",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
		putenv("UIDPATH=%U");	/* set up the $UIDPATH var */
	
	/*
	 * Open a nonexistent filename
	 */
		sprintf(uidname[0], "OpenH_nef");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Open an invalid file (not a .uid file)
	 */
		sprintf(uidname[0], "OpenH_if");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Open a valid file/hierarchy by referring to it WITH a .uid suffix 
	 * on the filename.
	 */
		sprintf(uidname[0], "OpenH_vhws.uid");
		OH_filename_num = 0;
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidget(
							OpenHierarchy[0],
							"OH_vhws",
							test_box,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Open a hierarchy from three files with one bad .uid file
	 */
		OH_filename_num = 0;
		sprintf(uidname[0], "OpenH_mf1");
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
		sprintf(uidname[1], "OpenH_if");
		OH_filename_vec[OH_filename_num] = uidname[1]; OH_filename_num++;
		sprintf(uidname[2], "OpenH_mf3");
		OH_filename_vec[OH_filename_num] = uidname[2]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
	
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			message = XmStringCreateLtoR("OH_mf1 *A* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_mf1",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			message = XmStringCreateLtoR("OH_if *A* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_if",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			message = XmStringCreateLtoR("OH_mf3 *A* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_mf3",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Open a hierarchy from three files with one nonexistent file
	 */
		OH_filename_num = 0;
		sprintf(uidname[0], "OpenH_mf1");
		OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
		sprintf(uidname[1], "OpenH_nef");
		OH_filename_vec[OH_filename_num] = uidname[1]; OH_filename_num++;
		sprintf(uidname[2], "OpenH_mf3");
		OH_filename_vec[OH_filename_num] = uidname[2]; OH_filename_num++;
	
		OpenHierarchy[0] = NULL;
		OpenHierarchyInfo[OH_info_cnt].actual_return = 	
						MrmOpenHierarchyPerDisplay(
							display,
							OH_filename_num,
							OH_filename_vec,
							NULL,
							&OpenHierarchy[0]);
	
		if (OpenHierarchyInfo[OH_info_cnt].actual_return == MrmSUCCESS)
		{
			/* 
			 * For each widget, change the label string and the 
			 * position so that it is visually distinguished from 
			 * the other instances of the same widget.
			 */
			message = XmStringCreateLtoR("OH_mf1 *B* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			XtSetArg(args[nargs], XmNtopOffset, 62); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_mf1",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			message = XmStringCreateLtoR("OH_nef *B* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			XtSetArg(args[nargs], XmNtopOffset, 87); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_nef",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			message = XmStringCreateLtoR("OH_mf3 *B* is completed.  ",
							XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, message); nargs++;
			XtSetArg(args[nargs], XmNtopOffset, 112); nargs++;
			OH_test[OH_test_cnt] = NULL;
			fetch_return = MrmFetchWidgetOverride(
							OpenHierarchy[0],
							"OH_mf3",
							test_box,
							NULL,
							args,
							nargs,
							&OH_test[OH_test_cnt],
							&class);
			if (fetch_return == MrmSUCCESS)
			{
				XtManageChild(OH_test[OH_test_cnt]);
				OH_test_cnt++;
			}
			XmStringFree(message);

			MrmCloseHierarchy(OpenHierarchy[0]);
		}
		OH_info_cnt++;
	
	/*
	 * Replace the instruction message with "valid test 2" message
	 */
	
		message = XmStringCreateLtoR(msg_valid_tst2, XmSTRING_DEFAULT_CHARSET);
	
		nargs = 0;
		XtSetArg(args[nargs], XmNmessageString, message); nargs++;
		XtSetValues(widgetmain, args, nargs);
		XmStringFree(message);
    }
    else if(iteration == FINISH)
    {
		iteration++;
	
	/*
	 * Replace the instruction message with test end message
	 */
	
		summary_msg = summary(NUM_TESTS,
				      TEST_DESCRIP_LENGTH,
				      OpenHierarchyInfo);

		fprintf (stdout, summary_msg);

		message = XmStringCreateLtoR(summary_msg, XmSTRING_DEFAULT_CHARSET);
		XtFree(summary_msg);
	
		nargs = 0;
		XtSetArg(args[nargs], XmNmessageString, message); nargs++;
		XtSetValues(widgetmain, args, nargs);
		XmStringFree(message);
    }
}
