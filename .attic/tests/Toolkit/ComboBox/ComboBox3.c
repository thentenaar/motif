/* $XConsortium: ComboBox3.c /main/5 1995/07/15 21:03:01 drk $ */
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
#include "ComboBox3.h"

void CreateControlPanel();
void CreateGeoPanel();
void SetAndGetHighlightThickness (Widget,XtPointer,XtPointer);
void SetAndGetArrowSize(Widget,XtPointer,XtPointer);
void SetAndGetArrowSpacing(Widget,XtPointer,XtPointer);
void SetAndGetMarginWidth(Widget,XtPointer,XtPointer);
void SetAndGetMarginHeight(Widget,XtPointer,XtPointer);
void SetAndGetSelectedPosition(Widget,XtPointer,XtPointer);
void SetAndGetMatchBehavior(Widget,XtPointer,XtPointer);
void SetAndGetStrDirection(Widget,XtPointer,XtPointer);
void SetFont(Widget,XtPointer,XtPointer);
void SelectionCallback(Widget,XtPointer,XtPointer);
void SetWidgetValues();
void WidthHasChanged();
void HeightHasChanged();
void XHasChanged();
void YHasChanged();
void BWHasChanged();
void ResourceHasChanged();
void ValueHasChanged();
void ManageGeoBB();

#define NUM_LIST_ITEM 10

/* global declarations */
/* The widgets are used by several routines. */


Widget Parent;
Widget ComboBox;                /* This is the ComboBox under test. */


Widget ToggleDir[2];
Widget ToggleMatch[2];
Widget ComboScale[6];
Widget ToggleHighlight;
Widget GeoScale[5];
Widget GeoPB;
Widget ResourceText, ValueText;

/* for client_data parameters */

int get = GET;
int set = SET;

/* for geo testing */

Boolean WidthChanged = False;
Boolean HeightChanged = False;
Boolean XChanged = False;
Boolean YChanged = False;
Boolean BWChanged = False;
Boolean ResourceChanged = False;
Boolean ValueChanged = False;
Widget GeoBB1;


void
main (int argc, char **argv)

{
  Arg args[10];
  int n,i;
  char *Manager, *ComboBoxType;

  XmString TempLabel, ListItem[NUM_LIST_ITEM];

  static char *ListString[] = {
                           "Apple",
			   "Armadillo",
			   "Falafel",
			   "Ferret",
			   "Kebab",
			   "Kangaroo",
			   "Ramen Noodles",
			   "Racoon",
			   "Watermelon",
			   "Wallaby"
			   };

  CommonTestInit (argc, argv);

  if (UserData == NULL)
    {
      printf ("Usage: ComboBox3 -u <XmParent,comboBoxType,[geo]>\n");
      exit(0);
    }

  /* parse the command line. */


  Manager = strtok (UserData,",");
  ComboBoxType = strtok (NULL,",");
  

  /* Create the parent. */
  n=0;
  if (strcmp (Manager,"XmFrame") == 0)
    Parent = XmCreateFrame (Shell1, "Parent", args, n);
  else
    if (strcmp (Manager, "XmForm") == 0)
    Parent = XmCreateForm(Shell1, "Parent", args, n);
  else
    if (strcmp (Manager, "XmBulletinBoard") == 0)
    Parent = XmCreateBulletinBoard(Shell1, "Parent", args, n);
  else
    if (strcmp (Manager, "XmRowColumn") == 0)
    Parent = XmCreateRowColumn(Shell1, "Parent", args, n);
  else
    if (strcmp (Manager, "XmPanedWindow") == 0)
    Parent = XmCreateDrawingArea(Shell1, "Parent", args, n);
  else
    {
      printf ("Not a valid Manager!\n");
      exit(0);
    }

  XtManageChild (Parent);

/* Create a list for the ComboBox List child */

  for (i=0; i < NUM_LIST_ITEM; i++)
    {
      ListItem[i] = XmStringCreate (ListString[i], XmFONTLIST_DEFAULT_TAG);
    };


  n=0;
    /* set the comboBoxType. */
  if (strcmp (ComboBoxType,"COMBO_BOX") == 0)
    {
      ComboBox = XmCreateComboBox (Parent, "ComboBox", args, n);
    }
  else
    if (strcmp (ComboBoxType,"DROP_DOWN_COMBO_BOX") == 0)
    {
      ComboBox = XmCreateDropDownComboBox(Parent, "ComboBox", args, n);
    }
  else
      if (strcmp (ComboBoxType,"DROP_DOWN_LIST") == 0)
    {
      ComboBox = XmCreateDropDownList(Parent, "ComboBox", args, n);
    }
  else
    {
      printf ("Not a valid type!\n");
      exit(0);
    }

  XtManageChild (ComboBox);
  XtAddCallback (ComboBox, XmNselectionCallback, SelectionCallback, 
		 (XtPointer)NULL);


  


  n=0;
  XtSetArg (args[n], XmNitemCount, NUM_LIST_ITEM); n++;
  XtSetArg (args[n], XmNvisibleItemCount, 6); n++;
  XtSetArg (args[n], XmNitems, ListItem); n++;
  XtSetValues (XtNameToWidget (ComboBox,"*List"), args, n);  
  
  printf ("This is the widget hierarchy, starting from the ComboBox\n");
  CommonDumpHierarchy (ComboBox,stdout);

  CreateControlPanel();

  CreateGeoPanel();

  XtRealizeWidget (Shell1);
 if (instruct_file_name != NULL) {
     if (strncmp (instruct_file_name, "ComboBox3a", 10) == 0)
       for (i = 0; i < 6; i++)
         CommonPause();
     else
     if (strncmp (instruct_file_name, "ComboBox3b", 10) == 0)
       for (i = 0; i < 7; i++)
         CommonPause();
     else
     if (strncmp (instruct_file_name, "ComboBox3c", 10) == 0)
       for (i = 0; i < 8; i++)
         CommonPause();
 }
     
  XtAppMainLoop (app_context);
    
}

  
void
CreateControlPanel()

{

  Widget        MainBB1;
  Widget        MainRC1;
  Widget        ComboFrame[3];
  Widget        ComboRC[3];
  Widget        RboxFrame[2];
  Widget        ToggleFrame1;
  Widget        FontMenubar1;
  Widget        FontPulldown1;
  Widget        FontCascade1;
  Widget        FontPB[4];
  Widget        ComboRadioBox[2];
  Widget        ComboMenubar[2];
  Widget        ComboPulldown[2];
  Widget        ComboCascade[2];
  Widget        ComboSetPB[8];
  Widget        ComboGetPB[8];
  Widget        Separator1;
  

  

  static char          *scale_labels[] = 
                         { "Highlight Thickness",
			   "Arrow Size",
			   "Arrow Spacing",
			   "Margin Width",
			   "Margin Height",
			   "Selected Position"};


  static char          *match_names[] =
                         { "NONE",
			   "QUICK_NAVIGATE"};

  static char          *strdir_names[] =
                        { "STRING_DIRECTION_L_TO_R",
			  "STRING_DIRECTION_R_TO_L"};

  
  static char          *set_labels[] = 
                         { "Set Highlight Thickness",
			   "Set Arrow Size",
			   "Set Arrow Spacing",
			   "Set Margin Width",
			   "Set Margin Height",
			   "Set Match Behavior",
			   "Set Selected Position"
			   };


  static char          *get_labels[] = 
                         { "Get Highlight Thickness",
			   "Get Arrow Size",
			   "Get Arrow Spacing",
			   "Get Margin Width",
			   "Get Margin Height",
			   "Get Match Behavior",
			   "Get Selected Position"
			   };


  static char          *font_labels[] = 
                         { "Fixed",
			   "Variable",
			   "9x15",
			   "8x13bold"
			 };


  
  Arg           args[32];
  XmString      LabelString;
  char          name[32];
  int           n,i;
  Dimension     temp_y;


  /* Create main parent */

  n=0;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 400); n++;
  MainBB1 = XmCreateBulletinBoardDialog (Shell1, "MainBB1", args, n);
  XtManageChild (MainBB1);


  n=0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 3); n++;
  MainRC1 = XmCreateRowColumn (MainBB1, "MainRC1", args, n);
  XtManageChild (MainRC1);






  
  /* Create 2 frames */


  for (i=0; i < 2; i++)
    {
      sprintf (name, "ComboFrame%d", i);
      n=0;
      ComboFrame[i] = XmCreateFrame (MainRC1, name, args, n);
      XtManageChild(ComboFrame[i]);
    }


/* Create RowColumn for each frame */

  for (i=0; i<2; i++)
    {
      sprintf (name, "ComboRC%d", i);

      n = 0;
      XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
      ComboRC[i] = XmCreateRowColumn(ComboFrame[i], name, args, n);
      XtManageChild(ComboRC[i]);
    }

  /* Create Scales */


  for (i=0; i < 6; i++)
    {

      LabelString = XmStringCreate(scale_labels[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "ComboScale%d", i);
      n = 0;
      XtSetArg (args[n], XmNmaximum, 100); n++;
      XtSetArg (args[n], XmNorientation,XmHORIZONTAL); n++;
      XtSetArg (args[n], XmNscaleMultiple, 1); n++;
      XtSetArg (args[n], XmNshowValue, True); n++;
      XtSetArg (args[n], XmNtitleString, LabelString); n++;
      ComboScale[i] = XmCreateScale(ComboRC[0],name,args,n);
      XtManageChild (ComboScale[i]);
      XmStringFree (LabelString);
    }


  /* Create Frames which will hold RadioBoxes */
  
  for (i=0; i < 1; i++)
    {
      sprintf (name, "RboxFrame%d", i);
      n = 0;
      RboxFrame[i] = XmCreateFrame (ComboRC[1], name, args, n);
      XtManageChild (RboxFrame[i]);
    }


  /* Create RadioBoxes */

  for (i=0; i < 1; i++)
    {
     sprintf (name, "ComboRadioBox%d", i);
      n = 0;
      XtSetArg (args[n], XmNisHomogeneous, True); n++;
      XtSetArg (args[n], XmNradioAlwaysOne, True); n++;
      ComboRadioBox[i]=XmCreateRadioBox(RboxFrame[i], name, args, n);
      XtManageChild (ComboRadioBox[i]);

    }


  /* Create ToggleButton children of RadioBoxes */

  /* Create toggles for match. */

  for (i=0; i<2 ; i++)
    {
      LabelString = XmStringCreate(match_names[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "ToggleMatch%d", i);
      n=0;
      XtSetArg (args[n], XmNlabelString, LabelString); n++;
      ToggleMatch[i] = XmCreateToggleButton(ComboRadioBox[0],
						      name,
						      args, n);
      XtManageChild(ToggleMatch[i]);
      XmStringFree(LabelString);
    };

  /* Set NONE on by default */
   XmToggleButtonSetState(ToggleMatch[NONE], True, False); 







  /* Create pulldown for fonts. */

  n=0;
  FontMenubar1 = XmCreateMenuBar (ComboRC[1], "FontMenubar1",args,n);
  XtManageChild (FontMenubar1);

  n=0;
  FontPulldown1 =XmCreatePulldownMenu(ComboRC[1],
					  "FontPulldown1",args, n);



  n=0;
  LabelString = XmStringCreate ("Set Font", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, FontPulldown1); n++;
  XtSetArg (args[n], XmNlabelString, LabelString); n++;
  FontCascade1 =XmCreateCascadeButton(FontMenubar1,
					  "FontCascade1", args, n);
  XtManageChild (FontCascade1);

  XmStringFree(LabelString);



  /* Create pulldown for fonts */

  for (i=0; i < 4; i++)
    {

      sprintf (name, "FontPB%d", i);
      LabelString = XmStringCreate (font_labels[i], XmFONTLIST_DEFAULT_TAG);
      n=0;
      XtSetArg (args[n], XmNlabelString, LabelString); n++;
      FontPB[i] = XmCreatePushButton (FontPulldown1, name, args, n);
      XtManageChild (FontPB[i]);
    }

  XtAddCallback (FontPB[0], XmNactivateCallback, SetFont, "fixed");
  XtAddCallback (FontPB[1], XmNactivateCallback, SetFont, "variable");
  XtAddCallback (FontPB[2], XmNactivateCallback, SetFont, "9x15");
  XtAddCallback (FontPB[3], XmNactivateCallback, SetFont, "8x13bold");

  /* Create Separator */

  n=0;
  XtSetArg (args[n], XmNheight, 10); n++;
  Separator1 = XmCreateSeparator (ComboRC[1], "Separator1", args, n);
  XtManageChild (Separator1);



  /* Create Pulldown Menu for applying ComboBox values. */

  n=0;
  ComboMenubar[0] = XmCreateMenuBar (ComboRC[1], "ComboMenubar1",args,n);
  XtManageChild (ComboMenubar[0]);

  n=0;
  ComboPulldown[0] =XmCreatePulldownMenu(ComboRC[1],
					  "ComboPulldown1",args, n);


  LabelString = XmStringCreate ("Set ComboBox Values", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, ComboPulldown[0]); n++;
  XtSetArg (args[n], XmNlabelString, LabelString); n++;
  ComboCascade[0] =XmCreateCascadeButton(ComboMenubar[0],
					  "ComboCascade1", args, n);
  XtManageChild (ComboCascade[0]); 

  XmStringFree(LabelString);

  /* Create PushButtons for menus */

    for (i=0; i < 7; i++)
    {
      sprintf (name, "ComboSetPB%d", i);
      LabelString = XmStringCreate(set_labels[i], 
					 XmFONTLIST_DEFAULT_TAG);
  
      n=0;
      XtSetArg (args[n], XmNlabelString, LabelString); n++;
      ComboSetPB[i] = XmCreatePushButton(ComboPulldown[0], name, args, n);
      XtManageChild(ComboSetPB[i]);
      XmStringFree (LabelString);
    }
  

 /* Add Callbacks for PushButtons */

  XtAddCallback (ComboSetPB[SetHighlightThickness], XmNactivateCallback, 
		 SetAndGetHighlightThickness, (XtPointer)&set);

  XtAddCallback (ComboSetPB[SetArrowSize], XmNactivateCallback, 
		 SetAndGetArrowSize, (XtPointer)&set);

  XtAddCallback (ComboSetPB[SetArrowSpacing], XmNactivateCallback, 
		 SetAndGetArrowSpacing, (XtPointer)&set);

  XtAddCallback (ComboSetPB[SetMarginWidth], XmNactivateCallback, 
		 SetAndGetMarginWidth, (XtPointer)&set);

  XtAddCallback (ComboSetPB[SetMarginHeight], XmNactivateCallback, 
		 SetAndGetMarginHeight, (XtPointer)&set);

  XtAddCallback (ComboSetPB[SetMatchBehavior], XmNactivateCallback, 
		 SetAndGetMatchBehavior, (XtPointer)&set);


  XtAddCallback (ComboSetPB[SetSelectedPosition], XmNactivateCallback, 
		 SetAndGetSelectedPosition, (XtPointer)&set);




  /* Create menu for getting resources. */

  /* Create Pulldown Menu for getting ComboBox values. */

  n=0;
  ComboMenubar[1] = XmCreateMenuBar (ComboRC[1], "ComboMenubar2",args,n);
  XtManageChild (ComboMenubar[1]);

  n=0;
  ComboPulldown[1] =XmCreatePulldownMenu(ComboRC[1],
					  "ComboPulldown2",args, n);



  /* Get cascade to line up with cascade that sets values */



  LabelString = XmStringCreate ("Get ComboBox Values", 
				XmFONTLIST_DEFAULT_TAG);
 
  XtSetArg(args[n], XmNsubMenuId, ComboPulldown[1]); n++;
  XtSetArg (args[n], XmNlabelString, LabelString); n++;
  ComboCascade[1] =XmCreateCascadeButton(ComboMenubar[1],
					  "ComboCascade2", args, n);
  XtManageChild (ComboCascade[1]); 

  XmStringFree(LabelString);

  /* Create PushButtons for menus */

    for (i=0; i < 7; i++)
    {
      sprintf (name, "ComboGetPB%d", i);
      LabelString = XmStringCreate(get_labels[i], 
					 XmFONTLIST_DEFAULT_TAG);
  
      n=0;
      XtSetArg (args[n], XmNlabelString, LabelString); n++;
      ComboGetPB[i] = XmCreatePushButton(ComboPulldown[1], name, args, n);
      XtManageChild(ComboGetPB[i]);
      XmStringFree (LabelString);
    }
  

 /* Add Callbacks for PushButtons */

  XtAddCallback (ComboGetPB[GetHighlightThickness], XmNactivateCallback, 
		 SetAndGetHighlightThickness, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetArrowSize], XmNactivateCallback, 
		 SetAndGetArrowSize, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetArrowSpacing], XmNactivateCallback, 
		 SetAndGetArrowSpacing, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetMarginWidth], XmNactivateCallback, 
		 SetAndGetMarginWidth, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetMarginHeight], XmNactivateCallback, 
		 SetAndGetMarginHeight, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetMatchBehavior], XmNactivateCallback, 
		 SetAndGetMatchBehavior, (XtPointer)&get);

  XtAddCallback (ComboGetPB[GetSelectedPosition], XmNactivateCallback, 
		 SetAndGetSelectedPosition, (XtPointer)&get);


  /* create a PushButton to enable geo testing */

  n=0;
  LabelString = XmStringCreate ("Geometry",XmFONTLIST_DEFAULT_TAG);
  XtSetArg (args[n], XmNlabelString, LabelString); n++;
  XtSetArg (args[n], XmNbackground, CommonGetColor("red"));
  GeoPB = XmCreatePushButton (ComboRC[1], "GeoPB", args,n);
  XtManageChild (GeoPB);

  XtAddCallback (GeoPB, XmNactivateCallback, ManageGeoBB, (XtPointer)NULL);



  
}




void
CreateGeoPanel()
{
  Widget GeoPB[5];  
  Widget GeoRC1;
  Widget GeoFrame[2];
  Widget GeoRC[2];
  char name[32];
  Widget TextRC1;
  XmString LabelString;
  Widget ResourceLabel, ValueLabel;

  static char *geo_labels[] = 
                         { "Set Width",
			   "Set Height",
			   "Set x",
			   "Set y",
			   "Set Border Width"};


  static char *geo_apply[] = 
                         { "Apply to Shell",
			   "Apply to Parent",
			   "Apply to ComboBox",
			   "Apply to List",
			   "Apply to Text" };
			  

  int i,n;
  Arg args[10];


  n=0;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 250); n++;
  GeoBB1 = XmCreateBulletinBoardDialog (Shell1, "GeoBB1", args, n);



  n=0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 2); n++;
  GeoRC1 = XmCreateRowColumn (GeoBB1, "GeoRC1", args, n);
  XtManageChild (GeoRC1);


  /* Create 2 frames */


  for (i=0; i < 2; i++)
    {
      sprintf (name, "GeoFrame%d", i);
      n=0;
      GeoFrame[i] = XmCreateFrame (GeoRC1, name, args, n);
      XtManageChild(GeoFrame[i]);
    }


/* Create RowColumn for each frame */

  for (i=0; i < 2; i++)
    {
      sprintf (name, "GeoRC%d", i);

      n = 0;
      XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
      GeoRC[i] = XmCreateRowColumn(GeoFrame[i], name, args, n);
      XtManageChild(GeoRC[i]);
    }



  for (i=0; i < 5; i++)
    {

      LabelString = XmStringCreate(geo_labels[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "GeoScale%d", i);
      n = 0;
      XtSetArg (args[n], XmNmaximum, 500); n++;
      XtSetArg (args[n], XmNorientation,XmHORIZONTAL); n++;
      XtSetArg (args[n], XmNscaleMultiple, 1); n++;
      XtSetArg (args[n], XmNshowValue, True); n++;
      XtSetArg (args[n], XmNtitleString, LabelString); n++;
      GeoScale[i] = XmCreateScale(GeoRC[0],name,args,n);
      XtManageChild (GeoScale[i]);
      XmStringFree (LabelString);
    }



    XtAddCallback (GeoScale[0], XmNvalueChangedCallback,WidthHasChanged, (XtPointer)NULL);
    XtAddCallback (GeoScale[1], XmNvalueChangedCallback,HeightHasChanged, (XtPointer)NULL);
    XtAddCallback (GeoScale[2], XmNvalueChangedCallback,XHasChanged, (XtPointer)NULL);
    XtAddCallback (GeoScale[3], XmNvalueChangedCallback,YHasChanged, (XtPointer)NULL);
    XtAddCallback (GeoScale[4], XmNvalueChangedCallback,BWHasChanged, (XtPointer)NULL);

/* add Text */

   n=0;
   XtSetArg (args[n],XmNnumColumns, 1); n++;
   XtSetArg (args[n],XmNpacking, XmPACK_COLUMN); n++;
   TextRC1 = XmCreateRowColumn (GeoRC[1], "TextRC1", args, n);
   XtManageChild (TextRC1);

   n=0;
   LabelString = XmStringCreate ("Resource Name", XmFONTLIST_DEFAULT_TAG); n++;
   XtSetArg (args[n],XmNlabelString,LabelString); n++;
   ResourceLabel = XmCreateLabel (TextRC1, "ResourceLabel", args, n);
   XtManageChild (ResourceLabel);
   XmStringFree (LabelString);


   n=0;
   ResourceText = XmCreateTextField (TextRC1, "ResourceText", args, n);
   XtManageChild (ResourceText);
   XtAddCallback (ResourceText, XmNactivateCallback, ResourceHasChanged,
   (XtPointer)NULL);

   n=0;
   LabelString = XmStringCreate ("Resource Value", XmFONTLIST_DEFAULT_TAG); n++;
   XtSetArg (args[n],XmNlabelString,LabelString); n++;
   ValueLabel = XmCreateLabel (TextRC1, "ValueLabel", args, n);
   XtManageChild (ValueLabel);





   n=0;
   ValueText = XmCreateTextField (TextRC1, "ValueText", args, n);
   XtManageChild (ValueText);
   XtAddCallback (ValueText, XmNactivateCallback, ValueHasChanged, 
                  (XtPointer)NULL);


  for (i=0; i < 5; i++)
    {

      LabelString = XmStringCreate(geo_apply[i],XmFONTLIST_DEFAULT_TAG);
      sprintf (name, "GeoPB%d", i);
      n = 0;
      XtSetArg (args[n], XmNlabelString, LabelString); n++;
      GeoPB[i] = XmCreatePushButton(GeoRC[1],name,args,n);
      XtManageChild (GeoPB[i]);
      XmStringFree (LabelString);
    }


/* add callbacks */
    XtAddCallback (GeoPB[0], XmNactivateCallback, SetWidgetValues, (XtPointer)"Shell1");
    XtAddCallback (GeoPB[1], XmNactivateCallback, SetWidgetValues, (XtPointer)"Parent");
    XtAddCallback (GeoPB[2], XmNactivateCallback, SetWidgetValues, (XtPointer)"ComboBox"); 
    XtAddCallback (GeoPB[3], XmNactivateCallback, SetWidgetValues, (XtPointer)"List");
    XtAddCallback (GeoPB[4], XmNactivateCallback, SetWidgetValues, (XtPointer)"Text");






}


/* Routines for setting and getting values */

void 
SetAndGetHighlightThickness(Widget w,XtPointer client_data, XtPointer call_data)
{

  int value;
  Dimension get;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;
  

  switch (*cdata)
    {
    case SET:
      XmScaleGetValue(ComboScale[HighlightThickness],&value);
      n=0;
      XtSetArg (args[n], XmNhighlightThickness, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNhighlightThickness, &get);n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Highlight Thickness is %d\n", get);
      break;

    default:
    break;
      
    }
      
}





void 
SetAndGetSelectedPosition(Widget w,XtPointer client_data, XtPointer call_data)
{

  int value;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;




    switch (*cdata)
    {
    case SET:
      XmScaleGetValue(ComboScale[SelectedPosition],&value);
      n=0;
      XtSetArg (args[n], XmNselectedPosition, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNselectedPosition, &value);n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Selected Position is %d\n", value);
      break;

    default:
    break;
      
    }

      
}





void 
SetAndGetArrowSize(Widget w,XtPointer client_data, XtPointer call_data)
{

  int value;
  Dimension get;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;

  switch (*cdata)
    {
    case SET:
      XmScaleGetValue(ComboScale[ArrowSize],&value);
      n=0;
      XtSetArg (args[n], XmNarrowSize, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNarrowSize, &get); n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Arrow Size is %d\n", get);
      break;

    default:

      break;
    }
  
}



void 
SetAndGetArrowSpacing(Widget w,XtPointer client_data, XtPointer call_data)
{

  int value;
  Dimension get;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;



   switch (*cdata)
    {
    case SET:
      XmScaleGetValue(ComboScale[ArrowSpacing],&value);
      n=0;
      XtSetArg (args[n], XmNarrowSpacing, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNarrowSpacing, &get); n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Arrow Spacing is %d\n", get);
      break;


    default:

      break;
    }

}




void 
SetAndGetMarginWidth(Widget w,XtPointer client_data, XtPointer call_data)
{

  Dimension get;
  int value;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;


  switch (*cdata)
    {
    case SET:
      n=0;
      XmScaleGetValue(ComboScale[MarginWidth],&value);
      XtSetArg (args[n], XmNmarginWidth, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNmarginWidth, &get); n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Margin Width is %d\n", get);
      break;

    default:
      break;
    }

      
}


void 
SetAndGetMarginHeight(Widget w,XtPointer client_data, XtPointer call_data)
{
  Dimension get;
  int value;
  Arg args[2];
  int n;
  int *cdata = (int *)client_data;


  switch (*cdata)
    {
    case SET:
      n=0;
      XmScaleGetValue(ComboScale[MarginHeight],&value);
      XtSetArg (args[n], XmNmarginHeight, value); n++;
      XtSetValues (ComboBox, args, n);
      break;

    case GET:
      n=0;
      XtSetArg (args[n], XmNmarginHeight, &get); n++;
      XtGetValues (ComboBox,args,n);
      fprintf (stdout, "Margin Height  is %d\n", get);
      break;

    default:

      break;
    }
      
}




void 
SetAndGetMatchBehavior(Widget w,XtPointer client_data, XtPointer call_data)
{

  Arg args[2];
  int n,i;
  unsigned char match;
  int *cdata = (int *)client_data;

   switch (*cdata)
    {
    case SET:
        i=0;
	while (!(XmToggleButtonGetState(ToggleMatch[i]))) i++;
  
        switch (i)
	  {
	   case NONE:
	   n=0;
	   XtSetArg (args[n], XmNmatchBehavior, XmNONE); n++;
	   XtSetValues (ComboBox, args, n);
           break;

	   case QUICK_NAVIGATE:
	   n=0;
	   XtSetArg (args[n], XmNmatchBehavior, XmQUICK_NAVIGATE); n++;
	   XtSetValues (ComboBox, args, n);  
	   break;

	  default:
	   break;
	 }
	break;
      case GET:
	   n=0;
	   XtSetArg (args[n], XmNmatchBehavior, &match); n++;
	   XtGetValues (ComboBox, args, n);  
	   switch (match)
	     {
	     case XmNONE:
	       printf ("matchBehavior is XmNONE\n");
	       break;
	     case XmQUICK_NAVIGATE:
	       printf ("matchBehavior is XmQUICK_NAVIGATE\n");
	       break;
	     }
	break;

      default:
	break;
}


}






void 
SetFont(Widget w,XtPointer client_data, XtPointer call_data)
{

  Arg args[2];
  int n;
  


  XmFontList font_list = CommonGetFontList ((char *) client_data);
  
  n=0;
  XtSetArg (args[n], XmNrenderTable, font_list); n++;
  XtSetValues (ComboBox,args,n); 
}


/* Callback routines */



void 
SelectionCallback(Widget w,XtPointer client_data, XtPointer call_data)
{

  XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *)call_data;
  XmStringCharSet charset;
  XmStringDirection direction;
  XmStringContext context;
  Boolean separator;
  char *item;
  int reason = cb->reason;
  int position = cb->item_position;

   /* Print item that was just selected */

   /* Retrieve text string from XmString. */
        
   XmStringInitContext (&context, cb->item_or_text); 
   XmStringGetNextSegment (context, &item, &charset, &direction,
                                                        &separator);
   printf ("ComboBox SelectionCallback invoked\n");
   printf ("Item is %s\n", item);
   printf ("Position is %d\n", position);

    if (reason == XmCR_SELECT)
    {
      printf ("Reason is XmCR_SELECT\n");
    }
  else printf ("invalid reason\n"); 
  printf ("\n\n");
}






void 
ManageGeoBB(Widget w,XtPointer client_data, XtPointer call_data)
{
    XtManageChild (GeoBB1);
}







  

void 
SetWidgetValues(Widget w,XtPointer client_data, XtPointer call_data)
{
  int value;
  Arg args[2];
  int n;

  Boolean UserEntry;
  char *Resource = NULL;
  char *Value = NULL;

  Widget wid;
  char name[32];
  char *wname = (char *) client_data;
  

  UserEntry = False;



  n=0;
     
      printf ("%s\n", wname);
      if (WidthChanged)
	{
	  XmScaleGetValue(GeoScale[Width],&value);
	  XtSetArg (args[n], XmNwidth, value); n++;
	  WidthChanged = False;
	}


      if (HeightChanged)
	{
	  XmScaleGetValue(GeoScale[Height],&value);
	  XtSetArg (args[n], XmNheight, value); n++;
	  HeightChanged = False;
	}

      if (XChanged)
	{
	  XmScaleGetValue(GeoScale[X],&value);
	  XtSetArg (args[n], XmNx, value); n++;
	  XChanged = False;
	}

      if (YChanged)
	{
	  XmScaleGetValue(GeoScale[Y],&value);
	  XtSetArg (args[n], XmNy, value); n++;
	  YChanged = False;
	}

      if (BWChanged)
	{
	  XmScaleGetValue(GeoScale[BorderWidth],&value);
	  XtSetArg (args[n], XmNborderWidth, value); n++;
	  BWChanged = False;
	}

      if (ResourceChanged || ValueChanged)
	{
	  UserEntry = True;
	  Resource = XmTextFieldGetString(ResourceText);
          Value = XmTextFieldGetString(ValueText);
	  printf ("setting %s to %s\n",Resource,Value);
	  if (ResourceChanged)
	    ResourceChanged = False;
	  if (ValueChanged)
	    ValueChanged = False;
	}


      if (strcmp (wname, "Shell1") == 0)
	wid = Shell1;
      else
	{
	  sprintf (name,"*%s",wname);
  	  wid = XtNameToWidget (Shell1,name);
	}
      if (UserEntry == True)
	XtVaSetValues (wid,XtVaTypedArg,(String)Resource,XtRString,
		       Value,strlen(Value)+1,NULL);

      else
        XtSetValues (wid, args, n);
}







/* set of functions which resets flags */
 
void
WidthHasChanged()
{
  
  WidthChanged = True;

}


void 
HeightHasChanged()
{

  HeightChanged = True;

}


void 
XHasChanged()

{

  XChanged = True;

}


void 
YHasChanged()

{

  YChanged = True;

}


void 
BWHasChanged()

{

  BWChanged = True;

}


void 
ResourceHasChanged()

{

  ResourceChanged = True;

}




void 
ValueHasChanged()

{

  ValueChanged = True;

}








