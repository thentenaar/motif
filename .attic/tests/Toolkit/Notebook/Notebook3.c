/* $XConsortium: Notebook3.c /main/5 1995/07/15 21:07:25 drk $ */
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

#include <testlib.h>
#include <Xm/Notebook.h>
#include "Notebook3.h"





void CreateMainParent();
void CreateNotebookPanel();
void CreateNotebookApplyPanel();
void CreateChildPanel();
void CreateChildApplyPanel();
void CreateAndSetNotebookValues(Widget,XtPointer,XtPointer);
void CreateAndSetChildValues();
void CreateWidgetSelectionBox();
void CreatePopupSelectionPanel();
void CreateAppWidgetList();
void DisplayWidgetList();
void ModifyList (XmString, char *);
void DestroyW();
void PageChangedCallback(Widget,XtPointer,XtPointer);





/* These widgets are used by several routines */

  Widget        Parent;
  Widget        Notebook;


  Widget        Page[NUM_PAGE];
  Widget        MajorTab[NUM_MAJOR_TAB];
  Widget        MinorTab[NUM_MINOR_TAB];
  Widget        PageScroller[NUM_PAGE_SCROLLER];
  Widget        StatusArea[NUM_STATUS_AREA];
  Widget	MainFrame1, MainFrame2, MainFrame3, MainRC1, MainRC2;
  Widget        NotebookScale[11];
  Widget        ToggleOrientation[2];
  Widget        ToggleBinding[5];
  Widget        TogglePlacement[4];
  Widget        NotebookText[4][2];
  Widget        PageNumberScale;
  Widget        ToggleResizable;
  Widget        TogglenotebookChildType[5];

  Widget        ChildWidgetSelectionDialog;
  Widget        ListOfAppWidgets;

  
void
main(int argc, char **argv)
{
 int n,i;

 CommonTestInit(argc,argv);



 if (UserData == NULL)
   {
     printf ("Usage : Notebook3 -u <XmParentWidget>\n");
     exit(0);
   }



 if (strcmp (UserData, "XmBulletinBoard") == 0)
   Parent = XmCreateBulletinBoard (Shell1, "Parent", NULL, 0);
 else
 if (strcmp (UserData, "XmDrawingArea") == 0)
   Parent = XmCreateDrawingArea (Shell1, "Parent", NULL, 0);
 else
 if (strcmp (UserData, "XmForm") == 0)
     Parent = XmCreateForm (Shell1, "Parent", NULL, 0);
 else
 if (strcmp (UserData, "XmFrame") == 0)
   Parent = XmCreateFrame (Shell1, "Parent", NULL, 0);
 else
 if (strcmp (UserData, "XmRowColumn") == 0)
   Parent = XmCreateRowColumn(Shell1, "Parent", NULL, 0);
 else
 if (strcmp (UserData, "XmPanedWindow") == 0)
   Parent = XmCreatePanedWindow(Shell1, "Parent", NULL, 0);
 else
   {
     printf ("Not a valid Manager!\n");
     exit(0);
   }

 Notebook = XmCreateNotebook (Parent, "Notebook", NULL,0);
 XtAddCallback (Notebook, XmNpageChangedCallback,PageChangedCallback,(XtPointer)NULL);
 

 XtManageChild(Notebook); 
 XtManageChild(Parent);




 CreateMainParent();
 CreateNotebookPanel();
 CreateChildPanel();
 CreateNotebookApplyPanel(); 
 CreateChildApplyPanel();
 CreateWidgetSelectionBox();
 CreatePopupSelectionPanel();
 CreateAppWidgetList();
 XtRealizeWidget (Shell1);





   if (instruct_file_name != NULL) {
     if (strncmp (instruct_file_name, "Notebook3a", 10) == 0)
       for (i = 0; i < 9; i++)
	 CommonPause();
     else
     if (strncmp (instruct_file_name, "Notebook3b", 10) == 0)
       for (i = 0; i < 10; i++)
	 CommonPause();
     else
     if (strncmp (instruct_file_name, "Notebook3c", 10) == 0)
       for (i = 0; i < 9; i++)
	 CommonPause();
     else
     if (strncmp (instruct_file_name, "Notebook3d", 10) == 0)
       for (i = 0; i < 12; i++)
	 CommonPause();

   }


 XtAppMainLoop(app_context);

}	



void
CreateMainParent()
{


  Widget        ControlPanel;
  Widget        MainParent1;
  Arg args[10];
  int n;


  /* Create Top Layer */

  n = 0;
  XtSetArg (args[n], XmNautoUnmanage, False); n++;
  XtSetArg (args[n], XmNdefaultPosition, False); n++;
  XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
  XtSetArg (args[n], XmNx, 10); n++;
  XtSetArg (args[n], XmNy, 250); n++;
  ControlPanel = XmCreateBulletinBoardDialog(Shell1, "ControlPanel", args, n);
  XtManageChild(ControlPanel);

  n = 0;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);  n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  MainParent1 = XmCreateRowColumn(ControlPanel, "MainParent1", args, n);
  XtManageChild(MainParent1);

  n=0;
  XtSetArg (args[n], XmNborderWidth, 3); n++;
  XtSetArg (args[n], XmNborderColor, CommonGetColor("darkgreen")); n++;
  MainFrame1 = XmCreateFrame (MainParent1, "MainFrame1", args, n);
  XtManageChild(MainFrame1);


  n=0;
  MainFrame2 = XmCreateFrame (MainParent1, "MainFrame2", args, n);
  XtManageChild(MainFrame2);



  n = 0;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);  n++;
  XtSetArg (args[n], XmNnumColumns, 2);  n++;
  MainRC1 = XmCreateRowColumn(MainFrame1, "MainRC1", args, n);
  XtManageChild(MainRC1);


  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
  MainRC2 = XmCreateRowColumn(MainFrame2, "MainRC2", args, n);
  XtManageChild(MainRC2);








}


void
CreateNotebookPanel()

{

  Widget        ScaleRC1;
  Widget        NotebookFrame1,NotebookFrame2;
  Widget        ResourceRC1, ResourceRC2;
  Widget        ResourceFrame[4];
  Widget        TextRC1;
  Widget        ApplyRC1;
  Widget	ConstraintRadioBox[3];
  
  Widget        NotebookText[3];
  Widget        NotebookLabel[3];



  Arg           args[32];
  XmString      TitleString;
  char          name[32];
  int           n,i;


  


  static char *scale_labels[] = { 
                           "Current Page Number", 
			   "First Page Number", 
			   "Last Page Number",
			   "Back Page Number", 
			   "Back Page Size", 
			   "Binding Width", 
			   "Inner Margin Width", 
			   "Inner Margin Height", 
			   "Major Tab Spacing", 
			   "Minor Tab Spacing",
			   "Frame Shadow Thickness"};
			   


  

  static char          *color_labels[]  = { 
                           "Back Page Foreground", 
			   "Back Page Background", 
			   "Frame Background" }; 

  static  char          *orientation_names[] = { 
                           "HORIZONTAL", 
			   "VERTICAL" };

  static  char          *place_names[] = { 
                          "BOTTOM_RIGHT",
			  "BOTTOM_LEFT",
			  "TOP_RIGHT",
			  "TOP_LEFT" };
  
  static  char          *binding_names[] = { 
                          "NONE",
			  "SOLID",
			  "SPIRAL",
			  "PIXMAP",
			  "PIXMAP_OVERLAP_ONLY" };
  

/* Create panel for setting Notebook resources */


  
  /* Create 2 frames */


  n=0;
  NotebookFrame1 = XmCreateFrame (MainRC1, "NotebookFrame1", args, n);
  XtManageChild(NotebookFrame1);

  n=0;
  NotebookFrame2 = XmCreateFrame (MainRC1, "NotebookFrame2", args, n);
  XtManageChild(NotebookFrame2);

/* Create RowColumn for each frame */

  n = 0;
  XtSetArg (args[n],XmNorientation, XmVERTICAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1);  n++;
  ResourceRC1 = XmCreateRowColumn(NotebookFrame1, "ResourceRC1", args, n);
  XtManageChild(ResourceRC1);

  n = 0;
  XtSetArg (args[n],XmNorientation, XmVERTICAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1);  n++;
  ResourceRC2 = XmCreateRowColumn(NotebookFrame2, "ResourceRC2", args, n);
  XtManageChild(ResourceRC2);




 /* Create Scales */


  for (i=0; i < 11; i++)
    {

      TitleString = XmStringCreate(scale_labels[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "NotebookScale%d", i);
      n = 0;
      XtSetArg (args[n], XmNmaximum, 50); n++;
      XtSetArg (args[n], XmNminimum, -50); n++;
      XtSetArg (args[n], XmNorientation,XmHORIZONTAL); n++;
      XtSetArg (args[n], XmNscaleMultiple, 1); n++;
      XtSetArg (args[n], XmNshowValue, True); n++;
      XtSetArg (args[n], XmNtitleString, TitleString); n++;
      NotebookScale[i] = XmCreateScale(ResourceRC1,name,args,n);
      XtManageChild (NotebookScale[i]);
      XmStringFree (TitleString);
    }

  /* Create Frames and RowColumn that will hold RadioBox and Text */




  /* Create Frames which will hold RadioBoxes */
  
  for (i=0; i < 4; i++)
    {
      sprintf (name, "ResourceFrame%d", i);
      n = 0;
      ResourceFrame[i] = XmCreateFrame (ResourceRC2, name, args, n);
      XtManageChild (ResourceFrame[i]);
    }




  


  /* Create RadioBoxes */

  for (i=0; i < 3; i++)
    {
      sprintf (name, "ConstraintRadioBox%d", i);

      n = 0;
      XtSetArg (args[n], XmNisHomogeneous, True); n++;
      XtSetArg (args[n], XmNradioAlwaysOne, True); n++;
      ConstraintRadioBox[i]=XmCreateRadioBox(ResourceFrame[i], name, args, n);
      XtManageChild (ConstraintRadioBox[i]);

    }



  /* Create ToggleButtonGadget children of RadioBoxes */

  /* Create toggles for back page placement */

  for (i=0; i<4 ; i++)
    {
      TitleString = XmStringCreate(place_names[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "TogglePlacement%d", i);

      n=0;
      XtSetArg (args[n], XmNlabelString, TitleString); n++;
      TogglePlacement[i] = XmCreateToggleButtonGadget(ConstraintRadioBox[0],
						      name,
						      args, n);
      XtManageChild(TogglePlacement[i]);
      XmStringFree(TitleString);
    };

  /* Set BOTTOM_RIGHT on by default */
   XmToggleButtonGadgetSetState(TogglePlacement[BOTTOM_RIGHT], True, False);


  /* Create toggles for binding type */
  for (i=0; i<5 ; i++)
    {
      TitleString = XmStringCreate(binding_names[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "ToggleBinding%d", i);

      n=0;
      XtSetArg (args[n], XmNlabelString, TitleString); n++;
      ToggleBinding[i] = XmCreateToggleButtonGadget(ConstraintRadioBox[1],
						      name,
						      args, n);
      XtManageChild (ToggleBinding[i]);
      XmStringFree (TitleString);
    };

   /* set NONE on by default */
   XmToggleButtonGadgetSetState(ToggleBinding[NONE], True, False);


  /* Create toggles for orientation */
  for (i=0; i<2 ; i++)
    {
      TitleString = XmStringCreate (orientation_names[i],
				    XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "ToggleOrientation%d", i);

      n=0;
      XtSetArg (args[n], XmNlabelString, TitleString); n++;
      ToggleOrientation[i] = XmCreateToggleButtonGadget(ConstraintRadioBox[2],
						      name,
						      args, n);
      XtManageChild(ToggleOrientation[i]);
      XmStringFree(TitleString);
    }
   /* set HORIZONTAL on by defaults */
   XmToggleButtonGadgetSetState (ToggleOrientation[HORIZONTAL], True, False);




/* Create Frame and RowColumn for TextWidgets */

  n=0;
  XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  TextRC1 = XmCreateRowColumn (ResourceFrame[3], "TextRC1", args, n);
  XtManageChild (TextRC1);

  for (i=0; i < 3; i++)
    {

      TitleString = XmStringCreate (color_labels[i], XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "NotebookLabel%d", i);
      n=0;
      XtSetArg(args[n], XmNlabelString, TitleString); n++;
      NotebookLabel[i] = XmCreateLabel (TextRC1, name, args, n);
      XtManageChild (NotebookLabel[i]); 

      sprintf (name, "NotebookText%d", i);
      n=0;
      NotebookText[i] = XmCreateTextField (TextRC1, name, args, n);
      XtManageChild (NotebookText[i]);


    }

  /* Add Callbacks for Text Widgets */

  XtAddCallback (NotebookText[0], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)BACK_PAGE_FOREGROUND);

  XtAddCallback (NotebookText[1], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)BACK_PAGE_BACKGROUND);

  XtAddCallback (NotebookText[2], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)FRAME_BACKGROUND);

}




void
CreateNotebookApplyPanel()

{


    static char          *scale_set_labels[] = 
                         { "Set Current Page Number", 
			   "Set First Page Number", 
			   "Set Last Page Number",
			   "Set Back Page Number", 
			   "Set Back Page Size",
			   "Set Binding Width", 
			   "Set Inner Margin Width", 
			   "Set Inner Margin Height", 
			   "Set Major Tab Spacing", 
			   "Set Minor Tab Spacing" ,
			   "Set Frame Shadow Thickness" }; 

  Widget ApplyFrame1;
  Widget        NotebookPulldown1;
  Widget        NotebookMenubar1;
  Widget        NotebookCascade1;
  Widget        ApplyScalePB[11];
  Widget        ApplyPB[3];
  Arg args[32];
  int n,i;
  XmString TitleString;
  char name[32];

    /* Create Frame for "Apply" panel */

    n=0;
    XtSetArg(args[n], XmNborderWidth,3); n++;
    XtSetArg(args[n], XmNborderColor, CommonGetColor("darkgreen")); n++;
    ApplyFrame1 = XmCreateFrame (MainRC2, "ApplyFrame1", args, n);
    XtManageChild(ApplyFrame1);



  /* Create Pulldown Menu for applying Notebook Values */

  n=0;
  NotebookMenubar1 = XmCreateMenuBar (ApplyFrame1, "NotebookMenubar1", 
				      args, n); 
  XtManageChild (NotebookMenubar1);

  n=0;
  XtSetArg (args[n],XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
  NotebookPulldown1 =XmCreatePulldownMenu(NotebookMenubar1,
					  "NotebookPulldown1",args, n);

  n=0;
  TitleString = XmStringCreate ("Set Notebook Values", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, NotebookPulldown1); n++;
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  NotebookCascade1 =XmCreateCascadeButton(NotebookMenubar1,
					  "NotebookCascade1", args, n);
  XtManageChild (NotebookCascade1); 

  /* Create PushButtons for menus */

    for (i=0; i < 11; i++)
    {
      sprintf (name, "ApplyScalePB%d", i);
      TitleString = XmStringCreateSimple (scale_set_labels[i]);
  
      n=0;
      XtSetArg (args[n], XmNlabelString, TitleString); n++;
      ApplyScalePB[i] = XmCreatePushButton(NotebookPulldown1, name, args, n);
      XtManageChild(ApplyScalePB[i]);
      XmStringFree (TitleString);
    }
  

 /* Add Callbacks for PushButtons */

  XtAddCallback (ApplyScalePB[CurrentPageNumber], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)CURRENT_PAGE_NUMBER);


  XtAddCallback (ApplyScalePB[FirstPageNumber], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)FIRST_PAGE_NUMBER);


  XtAddCallback (ApplyScalePB[LastPageNumber], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)LAST_PAGE_NUMBER);


  XtAddCallback (ApplyScalePB[BackPageNumber], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)BACK_PAGE_NUMBER);



  XtAddCallback (ApplyScalePB[BackPageSize], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)BACK_PAGE_SIZE);



  XtAddCallback (ApplyScalePB[BindingWidth], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)BINDING_WIDTH);



  XtAddCallback (ApplyScalePB[InnerMarginWidth], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)INNER_MARGIN_WIDTH);



  XtAddCallback (ApplyScalePB[InnerMarginHeight], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)INNER_MARGIN_HEIGHT);



  XtAddCallback (ApplyScalePB[MajorTabSpacing], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)MAJOR_TAB_SPACING);



  XtAddCallback (ApplyScalePB[MinorTabSpacing], XmNactivateCallback, 
		 CreateAndSetNotebookValues, (XtPointer)MINOR_TAB_SPACING);

  XtAddCallback (ApplyScalePB[FrameShadowThickness], XmNactivateCallback, 
		CreateAndSetNotebookValues, (XtPointer)FRAME_SHADOW_THICKNESS);







   /* Create "Apply" PushButtons that are not scale values */


   n=0;
   TitleString = XmStringCreate("Set Orientation",XmFONTLIST_DEFAULT_TAG);
   XtSetArg (args[n], XmNlabelString, TitleString); n++; 
   ApplyPB[0] = XmCreatePushButton(NotebookPulldown1, "OrientationPB1", args, n);
   XtManageChild (ApplyPB[0]);
   XtAddCallback (ApplyPB[0], XmNactivateCallback, CreateAndSetNotebookValues, 
		  (XtPointer)ORIENTATION);
   XmStringFree (TitleString); 



   n=0;
   TitleString = XmStringCreate("Set Back Page Placement",
				XmFONTLIST_DEFAULT_TAG);
   XtSetArg (args[n], XmNlabelString, TitleString); n++;
   ApplyPB[1] = XmCreatePushButton(NotebookPulldown1, "PlacementPB1", args, n);
   XtManageChild (ApplyPB[1]);
   XtAddCallback (ApplyPB[1], XmNactivateCallback, CreateAndSetNotebookValues, 
		  (XtPointer)BACK_PAGE_PLACEMENT);
   XmStringFree (TitleString);


   n=0;
   TitleString = XmStringCreate("Set Binding Type",XmFONTLIST_DEFAULT_TAG);
   XtSetArg (args[n], XmNlabelString, TitleString); n++;
   ApplyPB[2] = XmCreatePushButton(NotebookPulldown1, "BindingPB1", args, n);
   XtManageChild (ApplyPB[2]);
   XtAddCallback (ApplyPB[2], XmNactivateCallback, CreateAndSetNotebookValues, 
		  (XtPointer)BINDING_TYPE);
   XmStringFree (TitleString); 

  }



void
CreateChildPanel()

{
  static char          *child_type_names[] = {
                                    "PAGE",
				    "MAJOR_TAB",
				    "MINOR_TAB",
				    "STATUS_AREA",
				    "PAGE_SCROLLER"};


  Widget        ChildFrame1;
  Widget        ChildRC1;
  Widget        ChildRboxFrame1, ChildToggleFrame1, ChildScaleFrame1;
  Widget        ChildRbox1;
  
  Arg           args[MAX_ARGS];
  int n,i;
  XmString TitleString;
  char name[32];

  n=0;
  XtSetArg(args[n], XmNborderWidth,3); n++;
  XtSetArg(args[n], XmNborderColor, CommonGetColor("goldenrod")); n++;
  ChildFrame1 = XmCreateFrame (MainRC2, "ChildFrame1", args,n);
  XtManageChild (ChildFrame1);

  n=0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  ChildRC1 = XmCreateRowColumn (ChildFrame1, "ChildRC1", args, n);
  XtManageChild (ChildRC1);

  /* Create frame for "notebookChildType" RadioBox. */

  n=0;
  ChildRboxFrame1 = XmCreateFrame (ChildRC1, "ChildRboxFrame1", args,n);
  XtManageChild (ChildRboxFrame1);

  /* Create frame for "pageNumber" Scale */
  
  n=0;
  ChildScaleFrame1 = XmCreateFrame (ChildRC1, "ChildScaleFrame1", args,n);
  XtManageChild (ChildScaleFrame1);


  /* Create frame for "resizable" Toggle */
  
  n=0;
  ChildToggleFrame1 = XmCreateFrame (ChildRC1, "ChildToggleFrame1", args,n);
  XtManageChild (ChildToggleFrame1);

  /* Create RadioBox */

  n=0;
  ChildRbox1 = XmCreateRadioBox (ChildRboxFrame1, "ChildRbox1", args, n);
  XtManageChild (ChildRbox1);

  /* Create ToggleButton Children of RadioBoxes */


  for (i=0; i<5 ; i++)
    {
      TitleString = XmStringCreate(child_type_names[i],
				   XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "TogglenotebookChildType%d", i);

      n=0;
      XtSetArg (args[n], XmNlabelString, TitleString); n++;
      TogglenotebookChildType[i] = XmCreateToggleButtonGadget(ChildRbox1,
						      name,
						      args, n);
      XtManageChild (TogglenotebookChildType[i]);
      XmStringFree (TitleString);
    };

   /* set PAGE on by default */
   XmToggleButtonGadgetSetState(TogglenotebookChildType[PAGE], True, False);


  /* Create "pageNumber" Scale */

   TitleString = XmStringCreate("Page Number",XmFONTLIST_DEFAULT_TAG);
   n = 0;
   XtSetArg (args[n], XmNorientation,XmHORIZONTAL); n++;
   XtSetArg (args[n], XmNscaleMultiple, 1); n++;
   XtSetArg (args[n], XmNmaximum, 20); n++;
   XtSetArg (args[n], XmNshowValue, True); n++;
   XtSetArg (args[n], XmNtitleString, TitleString); n++;
   PageNumberScale = XmCreateScale(ChildScaleFrame1,"PageNumberScale",args,n);
   XtManageChild (PageNumberScale);
   XmStringFree (TitleString);

  
  /* Create "resizable" Toggle */

     TitleString = XmStringCreate("Resizable",XmFONTLIST_DEFAULT_TAG);
   n = 0;
   XtSetArg (args[n], XmNlabelString, TitleString); n++;
   ToggleResizable = XmCreateToggleButton(ChildToggleFrame1, 
						"ToggleResizable", args,n);
						
   XtManageChild (ToggleResizable);
   XmStringFree (TitleString);

}

void
CreateChildApplyPanel()
{


  Widget ApplyFrame2;
  Widget ChildMenubar1;
  Widget ChildPulldown1;
  Widget ChildCascade1;
  Widget ChildApplyPB[3];
  XmString TitleString;
  Arg args[MAX_ARGS];
  int n, i;


  /* Create frame for holding "Apply" PushButtons for child */

    n=0;
    XtSetArg(args[n], XmNborderWidth,3); n++;
    XtSetArg(args[n], XmNborderColor, CommonGetColor("goldenrod")); n++;
    ApplyFrame2 = XmCreateFrame (MainRC2, "ApplyFrame2", args, n);
    XtManageChild(ApplyFrame2);

    /* Create Pulldown Menu for applying Notebook Values */

  n=0;
  ChildMenubar1 = XmCreateMenuBar (ApplyFrame2, "ChildMenubar1", args, n); 
  XtManageChild (ChildMenubar1);

  n=0;
  ChildPulldown1 =XmCreatePulldownMenu(ChildMenubar1,
					  "ChildPulldown1",args, n);

  n=0;
  TitleString = XmStringCreate ("Set Child Values", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, ChildPulldown1); n++;
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildCascade1 =XmCreateCascadeButton(ChildMenubar1,
					  "ChildCascade1", args, n);
  XtManageChild (ChildCascade1); 


  n=0;
  TitleString = XmStringCreate("Set Child Type",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildApplyPB[0] = XmCreatePushButton(ChildPulldown1,"notebookChildTypePB1", args, n);
  XtManageChild (ChildApplyPB[0]);
  XtAddCallback (ChildApplyPB[0], XmNactivateCallback, CreateAndSetChildValues,
		  (XtPointer)CHILD_TYPE);
  XmStringFree (TitleString); 


  n=0;
  TitleString = XmStringCreate("Set Page Number",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildApplyPB[1] = XmCreatePushButton(ChildPulldown1,"pageNumberPB1", args, n);
  XtManageChild (ChildApplyPB[1]);
  XtAddCallback (ChildApplyPB[1], XmNactivateCallback, CreateAndSetChildValues,
		  (XtPointer)PAGE_NUMBER);
  XmStringFree (TitleString); 


  n=0;
  TitleString = XmStringCreate("Set Resizable",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildApplyPB[2] = XmCreatePushButton(ChildPulldown1,"resizablePB1", args, n);
  XtManageChild (ChildApplyPB[2]);
  XtAddCallback (ChildApplyPB[2], XmNactivateCallback, CreateAndSetChildValues,
		  (XtPointer)RESIZABLE);
  XmStringFree (TitleString); 

}




void
CreateWidgetSelectionBox()

{


  Arg args[MAX_ARGS];
  int n,i;
  XmString label;
  XmStringTable ChildWidgetType;

  ChildWidgetType = (XmString *) XtMalloc(num_widgets * sizeof(XmString));
  label = XmStringCreate ("Widgets", XmFONTLIST_DEFAULT_TAG);

  for (i=0; i < num_widgets; i++)
   ChildWidgetType[i] = XmStringCreate (WidgetNames[i], XmFONTLIST_DEFAULT_TAG);


  n=0;
  XtSetArg (args[n],XmNlistLabelString, label); n++;
  XtSetArg (args[n],XmNlistItemCount, num_widgets); n++;
  XtSetArg (args[n],XmNlistVisibleItemCount, num_widgets); n++;
  XtSetArg (args[n],XmNlistItems, ChildWidgetType); n++;
  ChildWidgetSelectionDialog = XmCreateSelectionDialog (Shell1, 
			       "ChildWidgetSelectionDialog",args, n);

  XtAddCallback (ChildWidgetSelectionDialog, XmNokCallback,
		 CreateAndSetChildValues, (XtPointer)CREATE_CHILD);
  XmStringFree (label);

    while (--i >= 0)
    XmStringFree(ChildWidgetType[i]); 


}


void
CreateAppWidgetList()

{
  Widget BBParent;
  Arg args[10];
  int n;

  n=0;
  XtSetArg (args[n], XmNautoUnmanage, False); n++;
  XtSetArg (args[n], XmNdefaultPosition, False); n++;
  XtSetArg (args[n], XmNx, 10); n++;
  XtSetArg (args[n], XmNy, 10); n++;
  BBParent = XmCreateBulletinBoardDialog(Shell1, "BBParent", args, n);
  XtManageChild (BBParent);

  n=0;
  XtSetArg(args[n], XmNvisibleItemCount, 10); n++;
  XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); n++;
  ListOfAppWidgets = XmCreateScrolledList(BBParent,"ListOfAppWidgets", args, n);
  XtManageChild (ListOfAppWidgets);

}



void
DisplayWidgetList(Widget w,XtPointer client_data,XtPointer call_data)
{

  if (client_data!=NULL)
    {
      XtManageChild ((Widget) client_data);
    }
}


void 
CreatePopupSelectionPanel()

{
  Widget SetFrame1;
  Widget SetRC1;
  Widget Menubar1, Menubar2;
  Widget MenuPulldown1, MenuPulldown2;
  Widget MenuCascade1, MenuCascade2;
  Widget NotebookPB[2], ChildPB[2], DestroyPB1;
  XmString TitleString;
  Arg args[MAX_ARGS];
  int n, i;

  n=0;
  XtSetArg(args[n], XmNborderWidth, 3); n++;
  XtSetArg(args[n], XmNborderColor, CommonGetColor("red")); n++;
  SetFrame1 = XmCreateFrame (MainRC2, "SetRC1", args, n);
  XtManageChild (SetFrame1);

  n=0;
  XtSetArg(args[n], XmNorientation,XmVERTICAL); n++;
  SetRC1 = XmCreateRowColumn (SetFrame1, "SetRC1", args, n);
  XtManageChild (SetRC1);


  /* Create Pulldown Menu for Creating/Setting Values on Notebook */

  n=0;
  Menubar1 = XmCreateMenuBar (SetRC1, "Menubar1", args, n); 
  XtManageChild (Menubar1);

  n=0;
  MenuPulldown1 =XmCreatePulldownMenu(Menubar1,
					  "MenuPulldown1",args, n);

  n=0;
  TitleString = XmStringCreate ("Get Notebook Values", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, MenuPulldown1); n++;
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  MenuCascade1 =XmCreateCascadeButton(Menubar1,
					  "MenuCascade1", args, n);
  XtManageChild (MenuCascade1); 
  XmStringFree (TitleString);


  n=0;
  TitleString = XmStringCreate("NYI",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  NotebookPB[1] = XmCreatePushButton(MenuPulldown1,"NotebookPB1", args, n);
  XtManageChild (NotebookPB[1]);
  XmStringFree (TitleString); 



  /* Create Pulldown Menu for Creating/Setting Values on Notebook */

  n=0;
  Menubar2 = XmCreateMenuBar (SetRC1, "Menubar2", args, n); 
  XtManageChild (Menubar2);

  n=0;
  MenuPulldown2=XmCreatePulldownMenu(Menubar2, "MenuPulldown2",args, n);

  n=0;
  TitleString = XmStringCreate ("Child", XmFONTLIST_DEFAULT_TAG);
  XtSetArg(args[n], XmNsubMenuId, MenuPulldown2); n++;
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  MenuCascade2 =XmCreateCascadeButton(Menubar2,"MenuCascade2", args, n);
  XtManageChild (MenuCascade2); 
  XmStringFree (TitleString); 

  n=0;
  TitleString = XmStringCreate("Create Child",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildPB[0] = XmCreatePushButton(MenuPulldown2,"ChildPB1", args, n);
  XtManageChild (ChildPB[0]);
  XtAddCallback (ChildPB[0], XmNactivateCallback, DisplayWidgetList,
		  (XtPointer)ChildWidgetSelectionDialog);
  XmStringFree (TitleString); 


  n=0;
  TitleString = XmStringCreate("Set Values Child",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  ChildPB[1] = XmCreatePushButton(MenuPulldown2,"ChildPB2", args, n);
  XtManageChild (ChildPB[1]);
  XtAddCallback (ChildPB[1], XmNactivateCallback, CreateAndSetChildValues,
		  (XtPointer)SET_VALUES_CHILD);
  XmStringFree (TitleString); 


  n=0;
  TitleString = XmStringCreate("Destroy Widget",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, TitleString); n++;
  DestroyPB1 = XmCreatePushButton(SetRC1,"DestroyPB1", args, n);
  XtManageChild (DestroyPB1);
  XtAddCallback (DestroyPB1,XmNactivateCallback,DestroyW,(XtPointer)NULL);
  XmStringFree (TitleString); 

}


void
ModifyList (XmString item, char *action)

{


  static int pos=1;

  Arg args[10];
  int n;


  
      if (strcmp (action,"add")==0)
	{
	  XmListAddItem (ListOfAppWidgets,item,pos);
	  pos++;
	}

  else
      if (strcmp (action,"delete")==0)
	
	  XmListDeleteItem (ListOfAppWidgets,item);
  else
    printf ("invalid action!\n");

}



  

void 
CreateAndSetNotebookValues (Widget w, XtPointer client_data, XtPointer call_data)
{

  static Arg args[MAX_ARGS];
  int n;
  int scaleValue;
  char * textValue;
  int i;
  static int SetCurrentPageNumber = FALSE;
  
  n=0;
    switch ((int) client_data)
	{
	    case CURRENT_PAGE_NUMBER:
                XmScaleGetValue(NotebookScale[CurrentPageNumber], &scaleValue);
                n=0;
	        XtSetArg (args[n], XmNcurrentPageNumber, scaleValue); n++;
                XtSetValues (Notebook,args,n);
		break;		
	
	    case FIRST_PAGE_NUMBER:
	      	XmScaleGetValue(NotebookScale[FirstPageNumber], &scaleValue);
	        XtSetArg (args[n], XmNfirstPageNumber, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;
	    
	    case  LAST_PAGE_NUMBER:
	      	XmScaleGetValue(NotebookScale[LastPageNumber], &scaleValue);
	        XtSetArg (args[n], XmNlastPageNumber, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;
	    
	    case  BACK_PAGE_NUMBER:
	      	XmScaleGetValue(NotebookScale[BackPageNumber], &scaleValue);
	        XtSetArg (args[n], XmNbackPageNumber, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;

	      case  BACK_PAGE_SIZE:
	      	XmScaleGetValue(NotebookScale[BackPageSize], &scaleValue);
	        XtSetArg (args[n], XmNbackPageSize, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;
	    
	    case  BINDING_WIDTH:
	        XmScaleGetValue(NotebookScale[BindingWidth], &scaleValue);
	        XtSetArg (args[n], XmNbindingWidth, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;
	    
	      case  INNER_MARGIN_WIDTH:
	        XmScaleGetValue(NotebookScale[InnerMarginWidth], &scaleValue);
	        XtSetArg (args[n], XmNinnerMarginWidth, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;
	    
	      case  INNER_MARGIN_HEIGHT:
	        XmScaleGetValue(NotebookScale[InnerMarginHeight], &scaleValue);
	        XtSetArg (args[n], XmNinnerMarginHeight, scaleValue); n++; 
                XtSetValues (Notebook,args,n);
                break;

	    case  MAJOR_TAB_SPACING:
	        XmScaleGetValue(NotebookScale[MajorTabSpacing], &scaleValue);
	        XtSetArg (args[n], XmNmajorTabSpacing, scaleValue); n++;
                XtSetValues (Notebook,args,n);
		break;

	    case  MINOR_TAB_SPACING:
	        XmScaleGetValue(NotebookScale[MinorTabSpacing], &scaleValue);
	        XtSetArg (args[n], XmNminorTabSpacing, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;

	    case  FRAME_SHADOW_THICKNESS:
	        XmScaleGetValue(NotebookScale[FrameShadowThickness],
				&scaleValue);
	        XtSetArg (args[n], XmNframeShadowThickness, scaleValue); n++;
                XtSetValues (Notebook,args,n);
	        break;

	    case  ORIENTATION:
	        i=0;
	        while (!(XmToggleButtonGadgetGetState (ToggleOrientation[i])))
		  i++;

		  
		switch (i)
		  {
		  case HORIZONTAL:
		      XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
                      break;
	          case VERTICAL:
		      XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
                      break;
		    } 
            XtSetValues (Notebook,args,n);
            break;
	       

	    case BACK_PAGE_PLACEMENT:
		i=0;
	        while (!(XmToggleButtonGadgetGetState (TogglePlacement[i]))) 
		  i++;
		switch (i)
		  {
		    case BOTTOM_RIGHT:
		    XtSetArg (args[n], XmNbackPagePlacement, XmBOTTOM_RIGHT);n++;               
                   break;
      
		  case BOTTOM_LEFT:
		    XtSetArg (args[n], XmNbackPagePlacement, XmBOTTOM_LEFT);n++;                  
		    break;
		  case TOP_RIGHT:
		    XtSetArg (args[n], XmNbackPagePlacement, XmTOP_RIGHT);n++;
                    
		    break;
		  case TOP_LEFT:
		    XtSetArg (args[n], XmNbackPagePlacement, XmTOP_LEFT);n++;
                    break;
		  }
                XtSetValues (Notebook,args,n);
		break;

	    
	    case BINDING_TYPE:
		i=0;
		while (!(XmToggleButtonGadgetGetState (ToggleBinding[i]))) i++;
		switch(i)
		  {
		  case NONE:
		    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
		    break;
		  case SOLID:
		    XtSetArg(args[n], XmNbindingType, XmSOLID); n++;
		    break;
		  case SPIRAL:
		    XtSetArg(args[n], XmNbindingType, XmSPIRAL); n++;
		    break;
		  case PIXMAP:
		    XtSetArg(args[n], XmNbindingType, XmPIXMAP); n++;
		    break;
		  case PIXMAP_OVERLAP_ONLY:
		    XtSetArg(args[n],XmNbindingType,XmPIXMAP_OVERLAP_ONLY);n++;
                    break;
		  default:
		    break;
		  }
                XtSetValues (Notebook,args,n);
		break;
		
	      case BACK_PAGE_FOREGROUND:
		textValue = XmTextFieldGetString(w);
		XtSetArg(args[n], XmNbackPageForeground, 
			 CommonGetColor(textValue)); n++;
                XtSetValues (Notebook,args,n);
		break;

	      case BACK_PAGE_BACKGROUND:
		textValue = XmTextFieldGetString(w);
		XtSetArg(args[n], XmNbackPageBackground,
			 CommonGetColor(textValue)); n++;
                XtSetValues (Notebook,args,n);
		break;

	      case FRAME_BACKGROUND:
		textValue = XmTextFieldGetString(w);
		XtSetArg(args[n], XmNframeBackground,
			 CommonGetColor(textValue)); n++;
                XtSetValues (Notebook,args,n);
		break;



	      default:

		break;

	      }


}



void 
CreateAndSetChildValues (Widget w, XtPointer client_data, XtPointer call_data)

{


  static  char name[16];
  
  static Arg args[MAX_ARGS];
  static int n=0;
  int i;




  int scaleValue;
  char *ChildWanted;
  static int num_page=0, num_minor_tab=0, num_major_tab=0, 
             num_page_scroller=0, num_status_area = 0;
  XmString ChildJustCreated;
  XmStringTable ListItem;
  char *NameOfWidget;
  Arg nargs[5];

  static char *notebookChildType = "PAGE";

  
  switch ((int)client_data)
    {
    case CHILD_TYPE:
         i=0;
         while (!(XmToggleButtonGadgetGetState(TogglenotebookChildType[i]))) i++;
	 switch (i)
	   {
	   case PAGE:
	     XtSetArg (args[n], XmNnotebookChildType, XmPAGE); n++;
	     notebookChildType = "PAGE";
	     break;
	   
	   case MAJOR_TAB:
	     XtSetArg (args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	     notebookChildType = "MAJOR_TAB";
	     break;

	   case MINOR_TAB:
	     XtSetArg (args[n], XmNnotebookChildType, XmMINOR_TAB); n++;
	     notebookChildType = "MINOR_TAB";
	     break;

	   case PAGE_SCROLLER:
	     XtSetArg (args[n], XmNnotebookChildType, XmPAGE_SCROLLER); n++;
	     notebookChildType = "PAGE_SCROLLER";
	     break;

	   case STATUS_AREA:
	     XtSetArg (args[n], XmNnotebookChildType, XmSTATUS_AREA); n++;
	     notebookChildType = "STATUS_AREA";
	     break;

	   }
       case PAGE_NUMBER:
	 XmScaleGetValue (PageNumberScale, &scaleValue);
	 XtSetArg (args[n], XmNpageNumber, scaleValue); n++;
	 break;

       case RESIZABLE:
	 if (!(XmToggleButtonGetState(ToggleResizable)))
	   {
	     XtSetArg (args[n], XmNresizable, False); n++;
	   }
	 else
	   {
	     XtSetArg (args[n], XmNresizable, True); n++;
	   }
        break;
	      

       case CREATE_CHILD:

	 /* Get the widget you want to create. */

	 XmStringGetLtoR (((XmSelectionBoxCallbackStruct *)call_data)->value,
					XmFONTLIST_DEFAULT_TAG,&ChildWanted);


	 i=0;
	 while (((strcmp (ChildWanted, WidgetStruct[i].name) != 0) && (i < num_widgets)) && (name != NULL)) i++;
	 if ((i != num_widgets) && (Notebook != NULL))
	   { 
	     if (strcmp (notebookChildType, "PAGE") == 0)

	    
	       {
		 sprintf (name, "Page%d", num_page+1);
		 Page[num_page] = 
		    (*WidgetStruct[i].CreateFunction)(Notebook, name, args, n);
		 XtManageChild (Page[num_page]);
		 num_page++;
	       }

	     else
	       
		 if (strcmp (notebookChildType, "MAJOR_TAB") == 0)
		   {
		     sprintf (name, "MajorTab%d", num_major_tab+1);
		     MajorTab[num_major_tab] = 
		    (*WidgetStruct[i].CreateFunction)(Notebook, name, args, n);
		     XtManageChild (MajorTab[num_major_tab]);
		     num_major_tab++;
		   }

	     else
	       
		 if (strcmp (notebookChildType, "MINOR_TAB") == 0)
		   {
		   sprintf (name, "MinorTab%d", num_minor_tab+1);
		   MinorTab[num_minor_tab] = 
		    (*WidgetStruct[i].CreateFunction)(Notebook, name, args, n);
		   XtManageChild (MinorTab[num_minor_tab]);
		   num_minor_tab++;
		 }

	     else

	     if (strcmp (notebookChildType, "STATUS_AREA") == 0)
	       {
		 sprintf (name, "StatusArea%d", num_status_area+1);
		 StatusArea[num_status_area] = 
		   (*WidgetStruct[i].CreateFunction)(Notebook, name, args, n);
		 XtManageChild (StatusArea[num_status_area]);
		 num_status_area++;
	       }


	     else
	       
	     if (strcmp (notebookChildType, "PAGE_SCROLLER") == 0)
	       {
		 sprintf (name, "PageScroller%d", num_page_scroller+1);
		 PageScroller[num_page_scroller] = 
		 (*WidgetStruct[i].CreateFunction)(Notebook, name, args, n);
		 XtManageChild (PageScroller[num_page_scroller]);
		 num_page_scroller++;
	       }
	     



	    ChildJustCreated =  XmStringCreate(name, XmFONTLIST_DEFAULT_TAG);
	    ModifyList (ChildJustCreated,"add");
	    XmStringFree (ChildJustCreated);

	   
	    

}		   
		   
	 break;

       case SET_VALUES_CHILD:
	   XtSetArg (nargs[0], XmNselectedItems, &ListItem); 
	   XtGetValues (ListOfAppWidgets,nargs,1);
	   XmStringGetLtoR (*ListItem,XmFONTLIST_DEFAULT_TAG,&NameOfWidget);
	 
        if ((NameOfWidget != NULL) && (n!=0))
	  {
	    XtSetValues(XtNameToWidget(Notebook, (String)NameOfWidget),args,n);
	    n=0;
	  }
	break;		     

       default:
	 break;

       }
}


void 
DestroyW (Widget w, XtPointer client_data, XtPointer call_data)

{

    Arg args[5];
    int n;
    char *NameOfWidget;
    XmStringTable ListItem;

  
    n = 0;
    XtSetArg (args[n], XmNselectedItems, &ListItem); n++;
    XtGetValues (ListOfAppWidgets,args,n);
    if (ListItem != NULL)
      XmStringGetLtoR (*ListItem,XmFONTLIST_DEFAULT_TAG,&NameOfWidget);
        if (NameOfWidget != NULL)
	  {
	    XtDestroyWidget(XtNameToWidget(Notebook, (String)NameOfWidget));
	    ModifyList (*ListItem,"delete");
	  }
   
  }






void 
PageChangedCallback(Widget w, XtPointer client_data, XtPointer call_data)
{

  int want_page_no,current_page_no,reason;

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

