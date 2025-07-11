/* $XConsortium: Notebook5.c /main/5 1995/07/15 21:07:44 drk $ */
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


void CreateGeoPanel();
void SetWidgetValues();
void WidthHasChanged();
void HeightHasChanged();
void XHasChanged();
void YHasChanged();
void BWHasChanged();
void ResourceHasChanged();
void ValueHasChanged();

#define APPLY 1

/* global declarations 
 *
 */

/* 
 * These widgets are used by several routines. 
 */


Widget Parent;
Widget Notebook;


Widget GeoScale[5];
Widget GeoPB;
Widget ResourceText, ValueText;
Widget ScrollList1;


Widget Frame1,Form1,PW1,RC1,BB1;
Widget MajorTab[2];
Widget MinorTab[2];
Widget StatusArea[5];


Widget FrameKid1;
Widget FormKid[4];
Widget RCKid[4];
Widget BBKid[4];
Widget PWKid[4];

/* 
 * for indicating if values have changed
 */

Boolean WidthChanged = False;
Boolean HeightChanged = False;
Boolean XChanged = False;
Boolean YChanged = False;
Boolean BWChanged = False;
Boolean ResourceChanged = False;
Boolean ValueChanged = False;



enum          {
                 Width = 0,
		 Height,
		 X,
		 Y,
		 BorderWidth
		 };



void
main (int argc, char **argv)

{
  Arg args[10];
  int n,i;
  char buf[16];
  Widget BBDialog1;
  XmString PBLabel;



  CommonTestInit (argc, argv);

  /* 
   * Create the parent. 
   */

  if (UserData != NULL)  
  if (strcmp (UserData,"early") == 0)
    XtRealizeWidget (Shell1);


  n=0;
  Parent = XmCreateFrame (Shell1, "Parent", args, n); n++;
  XtManageChild (Parent);

  n=0;
  Notebook = XmCreateNotebook (Parent, "Notebook", args, n);
  XtManageChild (Notebook);

    
  for (i=0; i< 2; i++)
    {
      n=0;
      sprintf (buf, "MajorTab%d", i+1);
      MajorTab[i] = XmCreatePushButton (Notebook,buf,args,n);
      XtManageChild(MajorTab[i]);
    }
            


  for (i=0; i< 2; i++)
    {
       sprintf (buf, "MinorTab%d", i+1);
       MinorTab[i] = XmCreatePushButton (Notebook,buf,args,n);
       XtManageChild(MinorTab[i]);
    }
        

  for (i=0; i< 5; i++)
     {
       n=0;
       sprintf (buf,"Page%d", i+1);
       PBLabel = XmStringCreate (buf, XmFONTLIST_DEFAULT_TAG); 
       XtSetArg (args[n], XmNlabelString, PBLabel); n++;
       StatusArea[i] = XmCreateLabel (Notebook, buf, args,n);
       XmStringFree (PBLabel);
     }


  /*
   * Create Pages of Notebook
   */

  n=0;
  Frame1 = XmCreateFrame (Notebook, "Frame1", args, n);
  XtManageChild (Frame1);

  n=0;
  Form1 = XmCreateForm (Notebook, "Form1", args, n);
  XtManageChild (Form1);

  n=0;
  RC1 = XmCreateRowColumn (Notebook, "RC1", args, n);
  XtManageChild (RC1);


  n=0;
  PW1 = XmCreatePanedWindow (Notebook, "PW1", args, n);
  XtManageChild (PW1);

  n=0;
  BB1 = XmCreateBulletinBoard (Notebook, "BB1", args, n);
  XtManageChild (BB1);

  /*
   *   Create kids of pages
   */

   n=0;
   FrameKid1 = XmCreatePushButton (Frame1,"FrameKid1",args,n);
   XtManageChild (FrameKid1);


  for (i=0; i < 4; i++)
    {
      n=0;
      sprintf (buf, "FormKid%d", i+1);
      FormKid[i] = XmCreatePushButton (Form1,buf,args,n);
      XtManageChild (FormKid[i]);
    }

  for (i=0; i < 4; i++)
    {
      n=0;
      sprintf (buf, "RCKid%d", i+1);
      RCKid[i] = XmCreatePushButton (RC1,buf,args,n);
      XtManageChild (RCKid[i]);
    }


  for (i=0; i < 4; i++)
    {
      n=0;
      sprintf (buf, "PWKid%d", i+1);
      PWKid[i] = XmCreatePushButton (PW1,buf,args,n);
      XtManageChild (PWKid[i]);
    }

  for (i=0; i < 4; i++)
    {
      n=0;
      sprintf (buf, "BBKid%d", i+1);
      BBKid[i] = XmCreatePushButton (BB1,buf,args,n);
      XtManageChild (BBKid[i]);
    }

  /*
   * Create scrolled list with names of widgets
   */

  n=0;
  BBDialog1 = XmCreateBulletinBoardDialog (Shell1, "BBDialog1", args, n);
  XtManageChild (BBDialog1);

  n=0;
  XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
  ScrollList1 = XmCreateScrolledList(BBDialog1, "ScrollList1", args, n);
  XtManageChild (ScrollList1);

  XtAddCallback (ScrollList1, XmNbrowseSelectionCallback, SetWidgetValues, 
		 (XtPointer)APPLY);

  if ((UserData == NULL) || (strcmp (UserData,"late") == 0))
    XtRealizeWidget (Shell1);

  CreateGeoPanel();

  




  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  XtAppMainLoop (app_context);
    
}

  




void
CreateGeoPanel()
{
  Widget GeoShell1;
  Widget GeoForm1;
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


  int i,n;
  Arg args[10];


  n=0;
  GeoShell1 = XtCreatePopupShell("GeoShell1",
				 topLevelShellWidgetClass,
				 Shell1, args, n);


  

  n=0;
  GeoForm1 = XmCreateFormDialog (GeoShell1, "GeoForm1", args, n);
  XtManageChild (GeoForm1);


  n=0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 2); n++;
  GeoRC1 = XmCreateRowColumn (GeoForm1, "GeoRC1", args, n);
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
      XtSetArg (args[n], XmNmaximum, 1000); n++;
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
   LabelString = XmStringCreate ("Resource Value", XmFONTLIST_DEFAULT_TAG); 
   XtSetArg (args[n],XmNlabelString,LabelString); n++;
   ValueLabel = XmCreateLabel (TextRC1, "ValueLabel", args, n);
   XtManageChild (ValueLabel);
   XmStringFree (LabelString);

   n=0;
   ValueText = XmCreateTextField (TextRC1, "ValueText", args, n);
   XtManageChild (ValueText);
   XtAddCallback (ValueText, XmNactivateCallback, ValueHasChanged, 
                  (XtPointer)NULL);

   XtPopup (GeoShell1, XtGrabNone);

}









void 
SetWidgetValues(Widget w,XtPointer client_data, XtPointer call_data)
{
  int value;
  Arg args[10];
  Arg nargs[1];
  int n;

  Boolean UserEntry;
  char *Resource = NULL;
  char *Value = NULL;
  XmStringTable ListItem;
  char *NameOfWidget = NULL;
  char name[32];
  Widget wid;
  

  int cdata = (int) client_data;

  UserEntry = False;




  n=0;
     
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

  if (cdata==APPLY)
    {
      XtSetArg(nargs[0], XmNselectedItems, &ListItem);
      XtGetValues (ScrollList1, nargs, 1);
      if (ListItem != NULL) 
      XmStringGetLtoR (ListItem[0], XmFONTLIST_DEFAULT_TAG,&NameOfWidget);
    }
  


   if (NameOfWidget != NULL)
      {
	if (strcmp ("Shell1", NameOfWidget) == 0)
	  wid = Shell1;
	else
	  {
	    sprintf (name,"*%s", NameOfWidget);
	    wid = XtNameToWidget (Shell1,name);
	  }
	if (UserEntry == True)
              XtVaSetValues (wid, XtVaTypedArg,
			     (String)Resource,XtRString,
			     Value,strlen(Value)+1,NULL);

          else
              XtSetValues (wid, args, n);
      }

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








