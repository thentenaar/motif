/* $XConsortium: Notebook1.c /main/5 1995/07/15 21:07:16 drk $ */
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


/* This test takes all its resources from the resource file Notebook.defaults
   in the ./tests/Toolkit/Notebook directory. The test should be passed
   a flag (a number from 1-5), which will determine the XT name of the
   NoteBook widget created. This name then determines what resources
   are set for the Notebook. The resource file contains 5 set of resources
   for this test */

#include <Xm/NotebookP.h>
#include <testlib.h>



#define match(a,b) strcmp(a,b)==0

#define NUM_PAGE 6
#define NUM_MAJOR_TAB 2
#define NUM_MINOR_TAB 3



void
CreateGetValuesPanel();


void
GetBackPageNumber(Widget,XtPointer,XtPointer);


void
GetBackPageSize (Widget,XtPointer,XtPointer);

void
GetBindingWidth (Widget,XtPointer,XtPointer);

void
GetFrameShadowThickness(Widget,XtPointer,XtPointer);

void
GetInnerMarginWidth(Widget,XtPointer,XtPointer);


void
GetInnerMarginHeight(Widget,XtPointer,XtPointer);

void
GetMajorTabSpacing(Widget,XtPointer,XtPointer);

void
GetMinorTabSpacing(Widget,XtPointer,XtPointer);

void
PageChanged (Widget, XtPointer, XtPointer);



Widget Notebook;


void
main (int argc, char **argv)
{
	Widget Parent;
	Widget Page[NUM_PAGE];
	Widget MajorTab[NUM_MINOR_TAB];
	Widget MinorTab[NUM_MINOR_TAB];
	Widget PageScr;
	Widget StatusArea;
	char buf[16];
	int i,a;
	Arg args[10];

	CommonTestInit(argc,argv);

	/* User specifies an integer from 1-5  which determines the Xt name of
	   the Notebook widget. */

	if (UserData == NULL)
          a = 1;
	else

	if (match (UserData,"1")) 
	  a=1;
	else if (match (UserData,"2")) 
	  a=2;
	else if (match (UserData,"3")) 
	  a=3;
	else if (match (UserData,"4")) 
	  a=4;
	else if (match (UserData,"5")) 
	  a=5;
        else if (match (UserData, "6"))
          a=6;

	
	/* determine Xt Name of Parent and create Parent */
	sprintf (buf, "Parent%d", a);
	Parent = XmCreateFrame(Shell1, buf, NULL, 0);
	XtManageChild (Parent);


	/* determine Xt Name of Notebook and create Notebook */

	sprintf (buf, "Notebook%d", a);
	Notebook  = XmCreateNotebook (Parent, buf, NULL, 0);
	XtManageChild (Notebook);
	XtAddCallback (Notebook, XmNpageChangedCallback, PageChanged, 
		       (XtPointer)NULL);



	/* Create children of Notebook : Pages, Minor Tabs, Major Tabs,
	   a Page Scroller and a Status Area. */

        for (i=0; i< NUM_PAGE; i++)
	  {
	    sprintf (buf, "Page%d", i+1);
	    Page[i] = XmCreateLabel (Notebook,buf,NULL,0);
	  }
	    XtManageChildren (Page,NUM_PAGE);


	for (i=0; i< NUM_MAJOR_TAB; i++)
	  {
	    sprintf (buf, "MajorTab%d", i+1);
	    MajorTab[i] = XmCreatePushButton (Notebook,buf,NULL,0);
	  }
	    XtManageChildren (MajorTab, NUM_MAJOR_TAB);


	for (i=0; i< NUM_MINOR_TAB; i++)
	  {
	    sprintf (buf, "MinorTab%d", i+1);
	    MinorTab[i] = XmCreatePushButton (Notebook,buf,NULL,0);
	  }

	XtManageChildren (MinorTab,NUM_MINOR_TAB);

	PageScr = XmCreateScrollBar (Notebook, "PageScr", NULL, 0);
	XtManageChild (PageScr);

	StatusArea = XmCreateLabel (Notebook, "StatusArea", NULL, 0);
	XtManageChild (StatusArea);

	CreateGetValuesPanel();
	XtRealizeWidget (Shell1);

	CommonPause();
	CommonPause();


	XtAppMainLoop (app_context);
      }




void
CreateGetValuesPanel()
{

  Widget BBDialog1;
  Widget RowCol1;
  Widget GetValuesPB[8], Label1;
  int i,n;
  Arg args[10];
  static char *get_values[] = { "backPageNumber", 
                       "backPageSize",
		       "bindingWidth",
		       "frameShadowThickness",
		       "innerMarginHeight",
		       "innerMarginWidth",
		       "minorTabSpacing",
		       "majorTabSpacing"};
  XmString GetValuesLabel;


  n=0;
  BBDialog1 = XmCreateBulletinBoardDialog (Shell1, "BBDialog1", args, n);
  XtManageChild (BBDialog1);

  n=0;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  RowCol1 = XmCreateRowColumn (BBDialog1, "RowCol1", args, n);
  XtManageChild (RowCol1);

  n=0;
  GetValuesLabel = XmStringCreate ("Get Resource Values", XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, GetValuesLabel); n++;
  Label1 = XmCreateLabel (RowCol1, "Label1", args, n);
  XtManageChild (Label1);

  for (i=0; i < 8; i++)
    {
      n=0;
      GetValuesPB[i] = XmCreatePushButton (RowCol1, get_values[i], args, n);
    }
   XtManageChildren (GetValuesPB,8);


  XtAddCallback (GetValuesPB[0], XmNactivateCallback, GetBackPageNumber, 
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[1], XmNactivateCallback, GetBackPageSize, 
		 (XtPointer)NULL); 

  XtAddCallback (GetValuesPB[2], XmNactivateCallback, GetBindingWidth,
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[3], XmNactivateCallback, GetFrameShadowThickness,
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[4], XmNactivateCallback, GetInnerMarginHeight,
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[5], XmNactivateCallback, GetInnerMarginWidth,
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[6], XmNactivateCallback, GetMinorTabSpacing,
		 (XtPointer)NULL);

  XtAddCallback (GetValuesPB[7], XmNactivateCallback, GetMajorTabSpacing,
		 (XtPointer)NULL);

}



void
GetBackPageNumber (Widget w, XtPointer client_data, XtPointer call_data)
{
  Cardinal back_page_number;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNbackPageNumber, &back_page_number); n++;
  XtGetValues (Notebook,args,n);

  printf ("backPageNumber is %d\n", back_page_number);

}


void
GetBackPageSize (Widget w, XtPointer client_data, XtPointer call_data)
{
  Dimension back_page_size;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNbackPageSize, &back_page_size); n++;
  XtGetValues (Notebook,args,n);

  printf ("backPageSize is %d\n", back_page_size);

}


void
GetBindingWidth (Widget w, XtPointer client_data, XtPointer call_data)
{
  Dimension binding_width;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNbindingWidth, &binding_width); n++;
  XtGetValues (Notebook,args,n);

  printf ("binding width is %d\n", binding_width);

}

void
GetFrameShadowThickness (Widget w, XtPointer client_data, 
			 XtPointer call_data)
{
  Dimension frame_shadow_thickness;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNframeShadowThickness, &frame_shadow_thickness); n++;
  XtGetValues (Notebook,args,n);

  printf ("frame shadow thickness is %d\n", frame_shadow_thickness);

}


void
GetInnerMarginWidth (Widget w, XtPointer client_data, 
			 XtPointer call_data)
{
  Dimension inner_margin_width;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNinnerMarginWidth, &inner_margin_width); n++;
  XtGetValues (Notebook,args,n);

  printf ("inner margin width is %d\n", inner_margin_width);

}

void
GetInnerMarginHeight (Widget w, XtPointer client_data, 
			 XtPointer call_data)
{
  Dimension inner_margin_height;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNinnerMarginHeight, &inner_margin_height); n++;
  XtGetValues (Notebook,args,n);

  printf ("inner margin height is %d\n", inner_margin_height);

}


void
GetMinorTabSpacing (Widget w, XtPointer client_data, 
			 XtPointer call_data)
{
  Dimension minor_tab_spacing;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNminorTabSpacing, &minor_tab_spacing); n++;
  XtGetValues (Notebook,args,n);

  printf ("minor tab spacing is %d\n", minor_tab_spacing);

}

void
GetMajorTabSpacing (Widget w, XtPointer client_data, 
			 XtPointer call_data)
{
  Dimension major_tab_spacing;
  Arg args[10];
  int n;


  n=0;
  XtSetArg (args[n], XmNmajorTabSpacing, &major_tab_spacing); n++;
  XtGetValues (Notebook,args,n);

  printf ("major tab spacing is %d\n", major_tab_spacing);

}






void
PageChanged (Widget w, XtPointer client_data, XtPointer call_data)
{

   int want_page_no,current_page_no,reason;

  if (call_data == NULL)
    printf ("Current page has been changed\n");
  else
    {
      want_page_no = ((XmNotebookCallbackStruct *)call_data)->page_number;
      current_page_no = ((XmNotebookCallbackStruct *)call_data)->prev_page_number;
      reason = ((XmNotebookCallbackStruct *)call_data)->reason;
      printf ("Page has been changed\n");

   switch (reason)
        {
        case XmCR_MAJOR_TAB:
          printf ("Reason is XmCR_MAJOR_TAB\n");
          break;
        case XmCR_MINOR_TAB:
          printf ("Reason is XmCR_MINOR_TAB\n");
          break;
        case XmCR_PAGE_SCROLLER_DECREMENT:
          printf ("Reason is XmCR_PAGE_SCROLLER_DECREMENT\n");
          break;
        case XmCR_PAGE_SCROLLER_INCREMENT:
          printf ("Reason is XmCR_PAGE_SCROLLER_INCREMENT\n");
          break;
        case XmCR_NONE:
          printf ("Reason is XmCR_NONE\n");
          break;
        default:
          printf ("Invalid reason\n");
          break;
        }
      printf ("Current page is %d\n",current_page_no);
      printf ("Desired page is %d\n\n\n",want_page_no);
    }

}



