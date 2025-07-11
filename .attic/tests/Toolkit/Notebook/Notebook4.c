/* $XConsortium: Notebook4.c /main/5 1995/07/15 21:07:38 drk $ */
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
 * 
 */
/*
 * HISTORY
 */


#include <testlib.h>


#define NUMARGS 12
#define NUM_PAGE 12
#define NUM_STATUS_AREA 12
#define NUM_MAJOR_TAB 3
#define NUM_MINOR_TAB 3

/* This test takes its XmNtranslations resource value from the defaults file 
   ./tests/Toolkit/Notebook.defaults. */

void
main (int argc, char **argv)
{
	Widget Parent;
	Widget Notebook1;
	Widget Page[NUM_PAGE];
	Widget DupPage;
	Widget StatusArea[NUM_STATUS_AREA];
	Widget MajorTab[NUM_MINOR_TAB];
	Widget MinorTab[NUM_MINOR_TAB];
	Arg args[NUMARGS];
	int n,i,page_no;
	
	char buf[32];

	XmNotebookPageInfo page_info1, page_info2, page_info3, page_info4;
	XmNotebookPageStatus result1, result2, result3, result4;


	CommonTestInit(argc,argv);

	
	


	n=0;
	XtSetArg (args[n], XmNwidth, 300); n++;
	XtSetArg (args[n], XmNheight, 300); n++;
	Parent = XmCreateFrame(Shell1, "Parent", args, n);
	XtManageChild (Parent);

	n=0;
	XtSetArg (args[n], XmNfirstPageNumber, 1); n++;
	XtSetArg (args[n], XmNlastPageNumber, 5); n++;
	Notebook1 = XmCreateNotebook (Parent, "Notebook1", args, n);
	XtManageChild (Notebook1);

        for (i=0; i< NUM_PAGE; i++)
	  {
	    page_no = i+1;
	    sprintf (buf, "Page%d", page_no);
	    n=0;
	    XtSetArg(args[n], XmNnotebookChildType, XmPAGE); n++;
	    XtSetArg(args[n], XmNpageNumber, page_no); n++;

	    /* empty page for page 4. This is to test the PAGE_EMPTY
	       return value. */
	    if (page_no != 4)
	      {
		Page[i] = XmCreatePushButton (Notebook1,buf, args, n);
		XtManageChild (Page[i]);
	      }
	  }  

	/* duplicate page for page 5. This is to test PAGE_DUPLICATED. */
	
	page_no = 5;
	n=0;
	XtSetArg(args[n], XmNnotebookChildType, XmPAGE); n++;
	XtSetArg(args[n], XmNpageNumber, page_no); n++;
	DupPage = XmCreatePushButton (Notebook1, "DupPage", args, n);
	XtManageChild (DupPage);



        for (i=0; i< NUM_STATUS_AREA; i++)
	  {
	    page_no = i+1;
	    sprintf (buf, "StatusArea%d", page_no);
	    n=0;
	    XtSetArg(args[n], XmNnotebookChildType, XmSTATUS_AREA); n++;
	    XtSetArg(args[n], XmNpageNumber, page_no); n++;
	    StatusArea[i] = XmCreateLabel (Notebook1,buf, args, n);
	    XtManageChild (StatusArea[i]);
	  }  


	for (i=0; i<NUM_MAJOR_TAB; i++)
	  {

	    page_no = (i+1) * 6;
	    sprintf (buf, "MajorTab%d", i+1);
	    n=0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNpageNumber, page_no); n++;
	    MajorTab[i] = XmCreatePushButton (Notebook1,buf, args, n);
	    XtManageChild (MajorTab[i]);
	  }



	for (i=0; i< NUM_MINOR_TAB; i++)
	  {

	    page_no =  i + 4;
	    sprintf (buf, "MinorTab%d", i+1);

	    n=0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMINOR_TAB); n++;
	    XtSetArg(args[n], XmNpageNumber, page_no); n++;
	    MinorTab[i] = XmCreatePushButton (Notebook1,buf, args, n);
	    XtManageChild (MinorTab[i]);

	  } 

	XtRealizeWidget (Shell1);			   



	CommonPause();

	/* Pass in Page1. Result should be XmPAGE_FOUND. */



	page_info1.page_number = 1;
	page_info1.page_widget = Page[0];
	page_info1.status_area_widget = StatusArea[0];
	page_info1.major_tab_widget = NULL;
	page_info1.minor_tab_widget = NULL; 

	result1 = XmNotebookGetPageInfo(Notebook1,1,&page_info1);
	if (result1 == XmPAGE_FOUND)
	  printf ("Expected : PAGE_FOUND, Actual : PAGE_FOUND\n");
	else if (result1 == XmPAGE_INVALID)
	  printf ("Expected : PAGE_FOUND, Actual : PAGE_INVALID\n");
	else if (result1 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_FOUND, Actual : PAGE_EMPTY\n");
	else if (result1 == XmPAGE_DUPLICATED)
	  printf ("Expected : PAGE_FOUND, Actual : PAGE_DUPLICATED\n");
	else
	  printf ("Expected : PAGE_FOUND, Actual : Some random value\n");
	
	CommonPause();




	/* Pass in Page6. Result should be XmPAGE_INVALID. */

	page_info2.page_number = 6;
	page_info2.page_widget = Page[5];
	page_info2.status_area_widget = StatusArea[5];
	page_info2.major_tab_widget = MajorTab[0];
	page_info2.minor_tab_widget = NULL;

	result2 = XmNotebookGetPageInfo (Notebook1,6,&page_info2);
	if (result2 == XmPAGE_FOUND)
	  printf ("Expected : PAGE_INVALID, Actual : PAGE_FOUND\n");
	else if (result2 == XmPAGE_INVALID)
	  printf ("Expected : PAGE_INVALID, Actual : PAGE_INVALID\n");
	else if (result2 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_INVALID, Actual : PAGE_EMPTY\n");
	else if (result2 == XmPAGE_DUPLICATED)
	  printf ("Expected : PAGE_INVALID, Actual : PAGE_DUPLICATED\n");
	else
	  printf ("Expected : PAGE_INVALID, Actual : Some random value\n");

	CommonPause();

	/* Pass in Page4. Result should be XmPAGE_EMPTY. */

	page_info3.page_number = 4;
	page_info3.page_widget = Page[3];
	page_info3.status_area_widget = StatusArea[3];
	page_info3.major_tab_widget = NULL;
	page_info3.minor_tab_widget = MinorTab[0];

	result3 = XmNotebookGetPageInfo (Notebook1,4,&page_info3);
	if (result3 == XmPAGE_FOUND)
	  printf ("Expected : PAGE_EMPTY, Actual : PAGE_FOUND\n");
	else if (result3 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_EMPTY, Actual : PAGE_EMPTY\n");
	else if (result3 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_EMPTY, Actual : PAGE_EMPTY\n");
	else if (result3 == XmPAGE_DUPLICATED)
	  printf ("Expected : PAGE_EMPTY, Actual : PAGE_DUPLICATE\n");
	else
	  printf ("Expected : PAGE_EMPTY, Actual : Some random value\n");
	CommonPause();

	/* pass in page associated with page number 5. Result should
	   be PAGE_DUPLICATED */

	page_info4.page_number = 5;
	page_info4.page_widget = DupPage;
	page_info4.status_area_widget = NULL;
	page_info4.major_tab_widget = NULL;
	page_info4.minor_tab_widget = NULL;

	result4 = XmNotebookGetPageInfo (Notebook1,5,&page_info4);
	if (result4 == XmPAGE_FOUND)
	  printf ("Expected : PAGE_DUPLICATED, Actual : PAGE_FOUND\n");
	else if (result4 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_DUPLICATED, Actual : PAGE_EMPTY\n");
	else if (result4 == XmPAGE_EMPTY)
	  printf ("Expected : PAGE_DUPLICATED, Actual : PAGE_EMPTY\n");
	else if (result4 == XmPAGE_DUPLICATED)
	  printf ("Expected : PAGE_DUPLICATED, Actual : PAGE_DUPLICATED\n");
	else
	  printf ("Expected : PAGE_DUPLICATED, Actual : Some random value\n");


	CommonPause();
        CommonPause();


	XtAppMainLoop (app_context);
      }



