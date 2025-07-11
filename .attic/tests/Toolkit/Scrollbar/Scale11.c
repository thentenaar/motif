/* $XConsortium: Scale11.c /main/5 1995/07/15 21:09:20 drk $ */
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

enum {
       big_every = 0,
       medium_num,
       small_num,
       big_size,
       medium_size,
       small_size
      };

void ScaleValueChangedCB(Widget,XtPointer,XtPointer);
void CreateControlPanel();
void SetScaleTicsCB(Widget,XtPointer,XtPointer);
void EditableCB(Widget,XtPointer,XtPointer);
void ResizeCB(Widget,XtPointer,XtPointer);
void SetValueCB(Widget,XtPointer,XtPointer);
void ShowValueCB(Widget,XtPointer,XtPointer);


Widget Scale1;
Widget RCScale[6];
Widget ValSC;

void
main (int argc, char **argv)
{
   Arg args[10];
   int n, i;

   /* initialize toolkit */
   CommonTestInit(argc, argv);
    
   n = 0;

   /* Create a thermometer scale */

   XtSetArg(args[n], XmNx, 10); n++;
   XtSetArg(args[n], XmNy, 10);  n++;
   XtSetArg(args[n], XmNslidingMode, XmTHERMOMETER); n++;
   XtSetArg(args[n], XmNsliderVisual, XmFOREGROUND_COLOR); n++;
   Scale1 = XmCreateScale(Shell1, "Scale1", args, n); 

   XtAddCallback (Scale1, XmNvalueChangedCallback, ScaleValueChangedCB, NULL);

   XtManageChild(Scale1);

  /* Put the red color in the Scale's ScrollBar */
   n = 0;
   XtSetArg(args[n], XmNforeground, CommonGetColor("red")); n++;
   XtSetValues (XtNameToWidget(Scale1, "Scrollbar"), args, n);

   CreateControlPanel();

   XtRealizeWidget(Shell1);

   for (i=0; i<7; i++)
     CommonPause();

   XtAppMainLoop(app_context);
}

void 
ScaleValueChangedCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   int       value;

   /* Get the values from the scale */

   XmScaleGetValue(Scale1, &value);

   /* Reflect the change in the Control Panel */
   
   XmScaleSetValue(ValSC, value);

}

void
CreateControlPanel()
{
   Widget FormD1;
   Widget RowC1;
   Widget SetTicsPB;
   Widget Separator;
   Widget EditableTB;
   Widget ResizeTB;
   Widget SetValuePB;
   Widget pulldown, OptionM, NonePB, NearSliderPB, NearBorderPB;
   Arg    args[20];
   int    n, i, value;

   static char          *scale_labels[] = 
                         { "Num of scale values between big tics",
			   "Num of medium tics between big values",
			   "Num of small tics between medium values",
			   "Big tics size",
			   "Medium tics size",
			   "Small tics size"};

  
   XmString LabelString;
   char     name[20];

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
   RowC1 = XmCreateRowColumn (FormD1, "RowC1", args, n);
   XtManageChild (RowC1);

   /* Create the different scale widgets */

   for (i=0; i<6; i++)
     {
	LabelString = XmStringCreate(scale_labels[i],XmSTRING_DEFAULT_CHARSET);
	sprintf (name, "RCScale%d", i);
	n=0;
	XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNscaleHeight, 20); n++;
	XtSetArg(args[n], XmNscaleWidth, 180); n++;
	XtSetArg(args[n], XmNmaximum, 50); n++;
	XtSetArg(args[n], XmNshowValue, True); n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
	XtSetArg(args[n], XmNtitleString, LabelString); n++;

	RCScale[i] = XmCreateScale(RowC1, name, args, n);

	XtManageChild(RCScale[i]);

	XmStringFree(LabelString);

     }

   /* Create the Set Tics PB */

   LabelString = XmStringCreate("Set Tic Marks", XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, RowC1); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 180); n++;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   SetTicsPB = XmCreatePushButtonGadget(FormD1, "SetTicsPB", args, n);

   XtAddCallback (SetTicsPB, XmNactivateCallback, SetScaleTicsCB, NULL);

   XtManageChild(SetTicsPB);
   XmStringFree(LabelString);

   /* Create a separator */

   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, SetTicsPB); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   Separator = XmCreateSeparatorGadget(FormD1, "Separator", args, n);

   XtManageChild(Separator);
  

   /* Create Toggle Buttons */

   LabelString = XmStringCreate("Editable", XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, Separator); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   XtSetArg(args[n], XmNleftOffset, 50); n++;
   EditableTB = XmCreateToggleButtonGadget(FormD1, "EditableTB", args, n);

   XtAddCallback (EditableTB, XmNvalueChangedCallback, EditableCB, NULL);

   XtManageChild(EditableTB);
   XmStringFree(LabelString);

   LabelString = XmStringCreate("Allow Shell Resize",XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, EditableTB); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 50); n++;
   XtSetArg(args[n], XmNset, TRUE); n++;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   ResizeTB = XmCreateToggleButtonGadget(FormD1, "ResizeTB",args, n);

   XtAddCallback (ResizeTB, XmNvalueChangedCallback, ResizeCB, NULL);
   
   XtManageChild(ResizeTB);
   XmStringFree(LabelString);

   /* Create the Show Value option menu and its buttons */
   
   n = 0;
   pulldown = XmCreatePulldownMenu(FormD1, "show_value", args, n);
    
   n = 0;
   LabelString = XmStringCreate("none",XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   NonePB = XmCreatePushButtonGadget(pulldown, "NonePB", args, n);
   XtAddCallback (NonePB, XmNactivateCallback, ShowValueCB, 
		   (XtPointer) XmNONE);
   XtManageChild(NonePB);
   XmStringFree(LabelString);

   n = 0;
   LabelString = XmStringCreate("near_slider",XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   NearSliderPB = XmCreatePushButtonGadget(pulldown, "NearSliderPB", args, n);
   XtAddCallback (NearSliderPB, XmNactivateCallback, ShowValueCB, 
		  (XtPointer) XmNEAR_SLIDER);
   XtManageChild(NearSliderPB);
   XmStringFree(LabelString);

   n = 0;
   LabelString = XmStringCreate("near_border",XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   NearBorderPB = XmCreatePushButtonGadget(pulldown, "NearBorderPB", args, n);
   XtAddCallback (NearBorderPB, XmNactivateCallback, ShowValueCB, 
		  (XtPointer) XmNEAR_BORDER);
   XtManageChild(NearBorderPB);
   XmStringFree(LabelString);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pulldown); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, Separator); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, ResizeTB); n++;
   XtSetArg(args[n], XmNleftOffset, 30); n++;
   LabelString = XmStringCreate("Show Value",XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   OptionM = XmCreateOptionMenu(FormD1, "OptionM", args, n);
   XtManageChild (OptionM);

   XmStringFree(LabelString);

   /* Create the Value Scale and the Push Button */

   LabelString = XmStringCreate("Thermometer Value",XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNscaleHeight, 20); n++;
   XtSetArg(args[n], XmNscaleWidth, 180); n++;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XmScaleGetValue(Scale1, &value);
   XtSetArg(args[n], XmNvalue, value); n++;
   XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg(args[n], XmNtitleString, LabelString); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, ResizeTB); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 50); n++;

   ValSC = XmCreateScale(FormD1, "ValSC", args, n);

   XtRealizeWidget(ValSC);
   XtManageChild(ValSC);

   XmStringFree(LabelString);

   LabelString = XmStringCreate("Set Value",XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, ResizeTB); n++;
   XtSetArg(args[n], XmNtopOffset, 12); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNleftWidget, ValSC); n++;
   XtSetArg(args[n], XmNleftOffset, 20); n++;
   XtSetArg(args[n], XmNlabelString, LabelString); n++;
   
   SetValuePB = XmCreatePushButtonGadget(FormD1, "SetValuePB", args, n);

   XtAddCallback (SetValuePB, XmNactivateCallback, SetValueCB, NULL);

   XtManageChild(SetValuePB);
   XmStringFree(LabelString);
   
}

void 
SetScaleTicsCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
   Widget    *children;
   Cardinal  num_children, i;
   int       num_big, value;
   Cardinal  num_medium;
   Cardinal  num_small;
   Dimension size_big, size_medium, size_small;

   /* First remove the current tics */
   
   n = 0;
   XtSetArg (args[n], XmNnumChildren, &num_children);  n++;
   XtSetArg (args[n], XmNchildren, &children);  n++;
   XtGetValues (Scale1, args, n);

   XtUnmanageChildren(children+2, num_children-2);

   for (i=2; i < num_children; i++) {
	XtDestroyWidget(children[i]);
   }

   /* Get the values from the scales */

   XmScaleGetValue(RCScale[big_every], &num_big);
   XmScaleGetValue(RCScale[medium_num], &value);
   num_medium = (Cardinal) value;
   XmScaleGetValue(RCScale[small_num], &value);
   num_small = (Cardinal) value;
   XmScaleGetValue(RCScale[big_size], &value);
   size_big = (Dimension) value;
   XmScaleGetValue(RCScale[medium_size], &value);
   size_medium = (Dimension) value;
   XmScaleGetValue(RCScale[small_size], &value);
   size_small = (Dimension) value;

   /* Create the new tics */

   XmScaleSetTicks(Scale1, num_big, num_medium, num_small, size_big, 
		   size_medium, size_small);
  
}


void 
EditableCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
   XmToggleButtonCallbackStruct * scb = 
	(XmToggleButtonCallbackStruct *) call_data ;

   n=0;
   XtSetArg(args[n], XmNeditable, scb->set); n++;
   XtSetValues(Scale1, args, n);

}


void 
ResizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[20];
   int       n;
   XmToggleButtonCallbackStruct * scb = 
	(XmToggleButtonCallbackStruct *) call_data ;

   n=0;
   XtSetArg(args[n], XmNallowShellResize, scb->set); n++;
   XtSetValues(Shell1, args, n);
}

void 
SetValueCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[2];
   int       n, value;

   XmScaleGetValue(ValSC, &value);
   n=0;
   XtSetArg(args[n], XmNvalue, value); n++;
   XtSetValues(Scale1, args, n);
}

void 
ShowValueCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   Arg       args[2];
   int       n;

   n=0;
   XtSetArg(args[n], XmNshowValue, client_data); n++;
   XtSetValues(Scale1, args, n);
}
