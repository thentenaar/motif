/* $XConsortium: XmeDrawing1.c /main/5 1995/07/15 21:04:26 drk $ */
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


void CreateControlPanel();
void DirectionPulldownCB();
void IndicatorPulldownCB(Widget,XtPointer,XtPointer);
void OrientationPulldownCB(Widget,XtPointer,XtPointer);
void SeparatorTypePulldownCB(Widget,XtPointer,XtPointer);
void DrawingPulldownCB(Widget,XtPointer,XtPointer);
void ClearImageCB(Widget,XtPointer,XtPointer);
void SetValuesCB(Widget,XtPointer,XtPointer);

#define DRAWING_AREA_HEIGHT 400
#define DRAWING_AREA_WIDTH 400

Widget DrawingArea;

void
main (int argc, char **argv)
{
   Arg args[10];
   int n, i;

   /* initialize toolkit */
   CommonTestInit(argc, argv);
    
   /* Create the drawing area. */
   n=0;
   XtSetArg (args[n], XmNheight, DRAWING_AREA_HEIGHT); n++;
   XtSetArg (args[n], XmNwidth, DRAWING_AREA_WIDTH); n++;
   DrawingArea = XmCreateDrawingArea (Shell1, "DrawingArea", args, n);

   XtManageChild (DrawingArea);

   CreateControlPanel();

   XtRealizeWidget(Shell1);

   CommonPause();

   XtAppMainLoop(app_context);
}


void
CreateControlPanel()
{
   Widget FormD1;
   Widget RowC1, RowC2;
   Widget RCScale[8];
   Widget Separator1, Separator2;
   Widget ClearImagePB, SetValuesPB;
   Widget directionPDM, indicatorPDM, orientationPDM; 
   Widget separator_typePDM, drawingPDM; 
   Widget directionPB[4];
   Widget indicatorPB[2];
   Widget orientationPB[2];
   Widget separator_typePB[9];
   Widget drawing_typePB[4];
   Widget DirectionM, IndicatorM, OrientationM, SeparatorTypeM, DrawingM;
   Arg    args[20];
   int    n, i, value;

   static char          *scale_labels[] = 
                         { "X Bounding",
			   "Y Bounding",
			   "Widgth",
			   "Height",
			   "Shadow Thickness",
			   "Fill",
			   "Image Thickness",
			   "Margin"};

   static char          *option_menu_labels[] = 
                         { "Direction",
			   "Indicator Type",
			   "Orientation",
			   "Separator Type",
			   "Drawing"};

   static char          *direction_pulldown_options[] = 
                         { "XmARROW_RIGHT",
                           "XmARROW_LEFT",
                           "XmARROW_UP",
			   "XmARROW_DOWN"};

   static char          *indicator_pulldown_options[] = 
                         { "XmINDICATOR_CHECK",
			   "XmINDICATOR_CROSS"};

   static char          *orientation_pulldown_options[] = 
                         { "XmHORIZONTAL",
			   "XmVERTICAL"};

   static char          *separator_type_pulldown_options[] = 
                         { "XmSINGLE_LINE",
                           "XmDOUBLE_LINE",
                           "XmSINGLE_DASHED_LINE",
                           "XmDOUBLE_DASHED_LINE",
                           "XmNO_LINE",
                           "XmSHADOW_ETCHED_IN",
                           "XmSHADOW_ETCHED_OUT",
                           "XmSHADOW_ETCHED_IN_DASHED",
                           "XmSHADOW_ETCHED_OUT_DASHED",
			   "XmVERTICAL"};

   static char          *drawing_pulldown_options[] = 
                         { "xmeDrawArrow",
                           "xmeDrawDiamond",
                           "xmeDrawIndicator",
			   "xmeDrawSeparator"};


  
   XmString LabelString;
   char     name[20];
   char     PBGadgetName[20];
   char     CBname[20];

   /* Create main parent */ 
   
   n=0;
   XtSetArg(args[n], XmNdefaultPosition, False); n++;
   XtSetArg(args[n], XmNautoUnmanage, False); n++;
   XtSetArg(args[n], XmNx, 10); n++;
   XtSetArg(args[n], XmNy, 250); n++;
   FormD1 = XmCreateFormDialog (Shell1, "FormD1", args, n);
   XtManageChild (FormD1);

   /* Create Row Column to contain the Scale widgets */
   n=0;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNnumColumns, 2); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, FormD1); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNrightWidget, FormD1); n++;
   XtSetArg(args[n], XmNleftOffset, 5); n++;
   XtSetArg(args[n], XmNrightOffset, 5); n++;
   RowC1 = XmCreateRowColumn (FormD1, "RowC1", args, n);
   XtManageChild (RowC1);

   /* Create the different scale widgets */

   for (i=0; i<8; i++)
     {
	LabelString = XmStringCreate(scale_labels[i],XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "RCScale%d", i);
	n=0;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 200); n++;
	XtSetArg(args[n], XmNmaximum, 500); n++;
	XtSetArg(args[n], XmNminimum, 0); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, LabelString); n++;

	RCScale[i] = XmCreateScale(RowC1, name, args, n);

	XtManageChild(RCScale[i]);

	XmStringFree(LabelString);

     }


   /* Create the first separator */

   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, RowC1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   Separator1 = XmCreateSeparatorGadget(FormD1, "Separator1", args, n);

   XtManageChild(Separator1);


   /* Create Row Column to contain the Option Menu widgets */

   n=0;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   XtSetArg(args[n], XmNnumColumns, 2); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, Separator1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, FormD1); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNrightWidget, FormD1); n++;
   RowC2 = XmCreateRowColumn (FormD1, "RowC2", args, n);
   XtManageChild (RowC2);


   /* Create the Direction Option Menu and its buttons */

   n = 0;
   directionPDM = XmCreatePulldownMenu(RowC2, "directionPDM", args, n);

   for (i=0; i<4; i++)
     {
	sprintf (PBGadgetName, "%s", direction_pulldown_options[i]);
	LabelString = XmStringCreate(PBGadgetName, XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "directionPB%d", i);

	n = 0;
	XtSetArg(args[n], XmNlabelString, LabelString); n++;
	directionPB[i] = XmCreatePushButtonGadget(directionPDM, name, args, n);
	XtAddCallback (directionPB[i], XmNactivateCallback, 
		       DirectionPulldownCB, 
		       (XtPointer) direction_pulldown_options[i]);
	XtManageChild(directionPB[i]);
	XmStringFree(LabelString);

     }

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, directionPDM); n++;
     LabelString = XmStringCreate("Arrow Direction",
				  XmSTRING_DEFAULT_CHARSET);
     XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
     DirectionM = XmCreateOptionMenu(RowC2, "DirectionM", args, n);
     XtManageChild (DirectionM);

     XmStringFree(LabelString);


   /* Create the Indicator Option Menu and its buttons */

   n = 0;
   indicatorPDM = XmCreatePulldownMenu(RowC2, "indicatorPDM", args, n);

   for (i=0; i<2; i++)
     {
	sprintf (PBGadgetName, "%s", indicator_pulldown_options[i]);
	LabelString = XmStringCreate(PBGadgetName, XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "indicatorPB%d", i);

	n = 0;
	XtSetArg(args[n], XmNlabelString, LabelString); n++;
	indicatorPB[i] = XmCreatePushButtonGadget(indicatorPDM, name, args, n);
	XtAddCallback (indicatorPB[i], XmNactivateCallback, 
		       IndicatorPulldownCB, 
		       (XtPointer) indicator_pulldown_options[i]);
	XtManageChild(indicatorPB[i]);
	XmStringFree(LabelString);

     }

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, indicatorPDM); n++;
   LabelString = XmStringCreate("Indicator Type",
				XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   IndicatorM = XmCreateOptionMenu(RowC2, "IndicatorM", args, n);
   XtManageChild (IndicatorM);

   XmStringFree(LabelString);


   /* Create the Separator Orientation Option Menu and its buttons */

   n = 0;
   orientationPDM = XmCreatePulldownMenu(RowC2, "orientationPDM", args, n);

   for (i=0; i<2; i++)
     {
	sprintf (PBGadgetName, "%s", orientation_pulldown_options[i]);
	LabelString = XmStringCreate(PBGadgetName, XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "orientationPB%d", i);

	n = 0;
	XtSetArg(args[n], XmNlabelString, LabelString); n++;
	orientationPB[i] = XmCreatePushButtonGadget(orientationPDM, name, 
						    args, n);
	XtAddCallback (orientationPB[i], XmNactivateCallback, 
		       OrientationPulldownCB, 
		       (XtPointer) orientation_pulldown_options[i]);
	XtManageChild(orientationPB[i]);
	XmStringFree(LabelString);

     }

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, orientationPDM); n++;
   LabelString = XmStringCreate("Separator Orientation",
				XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   OrientationM = XmCreateOptionMenu(RowC2, "OrientationM", args, n);
   XtManageChild (OrientationM);

   XmStringFree(LabelString);


   /* Create the Separator Type Option Menu and its buttons */

   n = 0;
   separator_typePDM = XmCreatePulldownMenu(RowC2, "separator_typePDM", 
					    args, n);

   for (i=0; i<9; i++)
     {
	sprintf (PBGadgetName, "%s", separator_type_pulldown_options[i]);
	LabelString = XmStringCreate(PBGadgetName, XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "separator_typePB%d", i);

	n = 0;
	XtSetArg(args[n], XmNlabelString, LabelString); n++;
	separator_typePB[i] = XmCreatePushButtonGadget(separator_typePDM, name, 
						       args, n);
	XtAddCallback (separator_typePB[i], XmNactivateCallback, 
		       SeparatorTypePulldownCB, 
		       (XtPointer) separator_type_pulldown_options[i]);
	XtManageChild(separator_typePB[i]);
	XmStringFree(LabelString);

     }

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, separator_typePDM); n++;
   LabelString = XmStringCreate("Separator Type",
				XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   SeparatorTypeM = XmCreateOptionMenu(RowC2, "SeparatorTypeM", args, n);
   XtManageChild (SeparatorTypeM);

   XmStringFree(LabelString);


   /* Create the Drawing Type Option Menu and its buttons */

   n = 0;
   drawingPDM = XmCreatePulldownMenu(RowC2, "drawingPDM", args, n);

   for (i=0; i<4; i++)
     {
	sprintf (PBGadgetName, "%s", drawing_pulldown_options[i]);
	LabelString = XmStringCreate(PBGadgetName, XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "drawing_typePB%d", i);

	n = 0;
	XtSetArg(args[n], XmNlabelString, LabelString); n++;
	drawing_typePB[i] = XmCreatePushButtonGadget(drawingPDM, name, 
						       args, n);
	XtAddCallback (drawing_typePB[i], XmNactivateCallback, 
		       DrawingPulldownCB, 
		       (XtPointer) drawing_pulldown_options[i]);
	XtManageChild(drawing_typePB[i]);
	XmStringFree(LabelString);

     }

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, drawingPDM); n++;
   LabelString = XmStringCreate("Draw Function",
				XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   DrawingM = XmCreateOptionMenu(RowC2, "DrawingM", args, n);
   XtManageChild (DrawingM);

   XmStringFree(LabelString);


   /* Create the second separator */

   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, RowC2); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   Separator2 = XmCreateSeparatorGadget(FormD1, "Separator2", args, n);

   XtManageChild(Separator2);

   /* Create the Set Values PushButton */

   LabelString = XmStringCreate("Set Values",XmSTRING_DEFAULT_CHARSET);

   n = 0;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, Separator2); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget, FormD1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, FormD1); n++;
   XtSetArg(args[n], XmNleftOffset, 20); n++;
   XtSetArg(args[n], XmNwidth, 100); n++;
   SetValuesPB = XmCreatePushButton(FormD1, "SetValuesPB", args, n);
   XtAddCallback (SetValuesPB, XmNactivateCallback, SetValuesCB, NULL);
   XtManageChild(SetValuesPB);

   XmStringFree(LabelString);


   /* Create the Clear Image PB */

   LabelString = XmStringCreate("Clear Image", XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, Separator2); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget, FormD1); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNrightWidget, FormD1); n++;
   XtSetArg(args[n], XmNrightOffset, 20); n++;
   XtSetArg(args[n], XmNwidth, 100); n++;
   ClearImagePB = XmCreatePushButtonGadget(FormD1, "ClearImagePB", args, n);
   XtAddCallback (ClearImagePB, XmNactivateCallback, ClearImageCB, NULL);
   XtManageChild(ClearImagePB);

   XmStringFree(LabelString);


   XtRealizeWidget(FormD1);

}


void 
DirectionPulldownCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
/* empty stub */
}


void 
IndicatorPulldownCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;

/* empty stub */

   /* XmToggleButtonCallbackStruct * scb = 
	(XmToggleButtonCallbackStruct *) call_data ;

   n=0;
   XtSetArg(args[n], XmNallowShellResize, scb->set); n++;
   XtSetValues(Shell1, args, n); */
}


void 
OrientationPulldownCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
/* empty stub */
}


void 
SeparatorTypePulldownCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
/* empty stub */
}


void 
DrawingPulldownCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
   GC        TopGC, BottomGC, CenterGC;

/* empty stub */
}


void 
ClearImageCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
/* empty stub */
}


void 
SetValuesCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[2];
   int       n, value;
/* empty stub */
}

