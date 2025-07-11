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
static char rcsid[] = "$XConsortium: CloseH.c /main/10 1995/07/14 10:46:24 drk $"
#endif
#endif

#include <MrmTest.h> 
#include <testlib.h>

/* 
 * Data for summary
 */

#define NUM_TESTS    4
#define TEST_DESCRIP_LENGTH    	60

struct TestInfo CloseHierarchyInfo[NUM_TESTS] = 
{
    {"Close a valid hierarchy                                     ",
        MrmSUCCESS,	0},
    {"Close an empty hierarchy                                    ",
        MrmBAD_HIERARCHY,	0},
#ifdef	BADTEST
    {"Close a already closed hierarchy                            ",
        MrmBAD_HIERARCHY,	0},
#endif 	/* BADTEST */
    {"Close a garbage hierarchy                                   ",
        MrmBAD_HIERARCHY,	0},
    {"Close a valid hierarchy opened from three files             ",
        MrmSUCCESS,	0},
};
int    CH_info_cnt = 0;

/*
 * External declarations
 */
extern String	summary();

/*
 * Local declarations
 */
FILE	*CH_logfile;
#define	LOGFILE_NAME	"CH_log"

void main (argc,argv)
int argc;
char **argv;
{ 



    Arg	args[4];
    int	nargs;

    XmString	message;

    String	summary_msg;

    MrmHierarchy	OpenHierarchy[3];
    int			emptyhier = 1;
    String		OH_filename_vec[3];
    MrmCount	OH_filename_num = 0;

    char	uidname[3][80];
    int		fetch_return, open_return;
    int		i;


    MrmHierarchy	s_MrmHierarchy;
    MrmType		class;

    char	uidnameMain[80];

    

    String          filename_vec[1];
    MrmCount        filename_num = 0;

    String          testname;



    Widget	test_box,
	        CH_test[8];

    int	        CH_test_cnt = 0;





    CommonTestUilInit(argc,argv);
    MrmInitialize();


     /* 
        * Open the Mrm hierarchy
        */
    testname=argv[0];

     sprintf(uidnameMain, "%s.uid", testname);
     filename_vec[filename_num] = uidnameMain; filename_num++;

     if(MrmOpenHierarchy(filename_num,       /* number of files      */
                         filename_vec,       /* files                */
                         NULL,               /* os_ext_list (null)   */
                         &s_MrmHierarchy)    /* ptr to returned id   */
      != MrmSUCCESS) 
         {
            printf("Can't open hierarchy\n");
         }


    

    MrmFetchWidget(
                       s_MrmHierarchy,
                       "test_box",
		       Shell1,
                       &test_box,
                       &class);
    
    XtManageChild(test_box);
    XtRealizeWidget(Shell1);

    CommonPause();

    /*
     * Close a valid hierarchy
     */
    sprintf(uidname[0], "OpenH_vh");
    OH_filename_num = 0;
    OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
    
    OpenHierarchy[0] = NULL;
    open_return = MrmOpenHierarchy(
                               OH_filename_num,
                               OH_filename_vec,
                               NULL,
                               &OpenHierarchy[0]);
    
    if (open_return == MrmSUCCESS)
     {
       CH_test[CH_test_cnt] = NULL;
       fetch_return = MrmFetchWidget(
                               OpenHierarchy[0],
                               "OH_vh",
                               test_box,
                               &CH_test[CH_test_cnt],
                               &class);
       if (fetch_return == MrmSUCCESS)
        {
          XtManageChild(CH_test[CH_test_cnt]);
          CH_test_cnt++;
        }
     }
     CloseHierarchyInfo[CH_info_cnt].actual_return =
                 MrmCloseHierarchy(OpenHierarchy[0]);
     CH_info_cnt++;
    
    /*
     * Close an empty hierarchy
     */
	/* just pass it an address that has junk in it */
     CloseHierarchyInfo[CH_info_cnt].actual_return =
         MrmCloseHierarchy((MrmHierarchy)&emptyhier);
     CH_info_cnt++;

#ifdef    BADTEST
    /*
     * Close a already closed hierarchy
     */

     CloseHierarchyInfo[CH_info_cnt].actual_return =
            MrmCloseHierarchy(OpenHierarchy[0]);
     CH_info_cnt++;
#endif /* BADTEST */
    
    /*
     * Close a garbage hierarchy
     */
     OH_filename_num = 0;
     sprintf(uidname[0], "OpenH_if");
     OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
    
     OpenHierarchy[0] = NULL;
     open_return = MrmOpenHierarchy(
                               OH_filename_num,
                               OH_filename_vec,
                               NULL,
                               &OpenHierarchy[0]);
    
      if (open_return == MrmSUCCESS)
        {    /* This will never happen, not a valid hierarchy */
            CH_test[CH_test_cnt] = NULL;
            fetch_return = MrmFetchWidget(
                               OpenHierarchy[0],
                               "OH_if",
                               test_box,
                               &CH_test[CH_test_cnt],
                               &class);
            if (fetch_return == MrmSUCCESS)
            {
                XtManageChild(CH_test[CH_test_cnt]);
                CH_test_cnt++;
            }
        }
      CloseHierarchyInfo[CH_info_cnt].actual_return =
            MrmCloseHierarchy(OpenHierarchy[0]);
      CH_info_cnt++;
    

      CommonPause();
   
    
    /*
     * Close a valid hierarchy opened from three files
     */
     OH_filename_num = 0;
     sprintf(uidname[0], "OpenH_mf1");
     OH_filename_vec[OH_filename_num] = uidname[0]; OH_filename_num++;
     sprintf(uidname[1], "OpenH_mf2");
     OH_filename_vec[OH_filename_num] = uidname[1]; OH_filename_num++;
     sprintf(uidname[2], "OpenH_mf3");
     OH_filename_vec[OH_filename_num] = uidname[2]; OH_filename_num++;
    
     OpenHierarchy[0] = NULL;
     open_return = MrmOpenHierarchy(
                               OH_filename_num,
                               OH_filename_vec,
                               NULL,
                               &OpenHierarchy[0]);
     if (open_return == MrmSUCCESS)
       {
         CH_test[CH_test_cnt] = NULL;
         fetch_return = MrmFetchWidget(
                            OpenHierarchy[0],
                            "OH_mf1",
                            test_box,
                            &CH_test[CH_test_cnt],
                            &class);
          if (fetch_return == MrmSUCCESS)
            {
                XtManageChild(CH_test[CH_test_cnt]);
                CH_test_cnt++;
            }
         CH_test[CH_test_cnt] = NULL;
         fetch_return = MrmFetchWidget(
                            OpenHierarchy[0],
                            "OH_mf2",
                            test_box,
                            &CH_test[CH_test_cnt],
                            &class);
         if (fetch_return == MrmSUCCESS)
            {
                XtManageChild(CH_test[CH_test_cnt]);
                CH_test_cnt++;
            }
         CH_test[CH_test_cnt] = NULL;
         fetch_return = MrmFetchWidget(
                            OpenHierarchy[0],
                            "OH_mf3",
                            test_box,
                            &CH_test[CH_test_cnt],
                            &class);
         if (fetch_return == MrmSUCCESS)
            {
                XtManageChild(CH_test[CH_test_cnt]);
                CH_test_cnt++;
            }
        }
        CloseHierarchyInfo[CH_info_cnt].actual_return =
            MrmCloseHierarchy(OpenHierarchy[0]);
        CH_info_cnt++;

        CommonPause();
    
        summary_msg = summary(NUM_TESTS,
                      TEST_DESCRIP_LENGTH,
                      CloseHierarchyInfo);

        fprintf (stdout, summary_msg);

        CommonPause();
}
