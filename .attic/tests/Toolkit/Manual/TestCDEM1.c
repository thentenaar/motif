/* $XConsortium: TestCDEM1.c /main/5 1995/07/15 21:06:28 drk $ */
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

void main(int argc, char **argv)
{
    register int  	n;
    Arg         	args[MAX_ARGS];
    XmString    	tcs, tcs1, tcs2, tcs3;
    Widget              Form, TextF1, RC1, Toggle1, Label1;
    Widget              Separator, RC2, ToggleG2, LabelG2;
    Widget    	        FileSelectionBox1, FileSelectionBox2;
    Widget              xmDisplay;

    CommonTestInit(argc, argv);

    /* Setting Display resources to be CDE compliant */

    xmDisplay = XmGetXmDisplay(display);
    n = 0;
    XtSetArg(args[n], XmNenableBtn1Transfer, True);             n++;
    XtSetArg(args[n], XmNenableButtonTab, True);                n++;
    XtSetArg(args[n], XmNenableToggleColor, True);              n++;
    XtSetArg(args[n], XmNenableToggleVisual, True);             n++;
    XtSetArg(args[n], XmNdefaultButtonEmphasis, XmINTERNAL_HIGHLIGHT); n++;
    XtSetArg(args[n], XmNenableDragIcon, True);                 n++;
    XtSetValues(xmDisplay, args, n);

    
    n = 0;
    XtSetArg(args[n], XmNwidth,  580);  			n++;
    XtSetArg(args[n], XmNheight, 450);  			n++; 
    XtSetValues(Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, 5);                 n++;
    XtSetArg(args[n], XmNverticalSpacing, 5);                   n++;
    Form = XmCreateForm(Shell1, "Form", args, n);
    XtManageChild(Form);

    n = 0;
    XtSetArg(args[n], XmNvalue, "This is testing FileSelectionBox"); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                  n++;
    TextF1 = XmCreateTextField(Form, "TextF1", args, n);
    XtManageChild(TextF1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);      n++;
    XtSetArg(args[n], XmNtopWidget, TextF1);                   n++;
    XtSetArg(args[n], XmNnumColumns, 1);                       n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);             n++;
    RC1 = XmCreateRowColumn(Form, "RC1", args, n);
    XtManageChild(RC1);
    
    n = 0;
    tcs = XmStringGenerate("Click and try ",
                          XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
    XtSetArg(args[n], XmNlabelString, tcs);                     n++;
    XtSetArg(args[n], XmNhighlightColor, CommonGetColor("red"));n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                   n++;
    XtSetArg(args[n], XmNselectColor, XmHIGHLIGHT_COLOR);       n++;
    XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY);           n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                   n++;
    Toggle1 = XmCreateToggleButton(RC1, "Toggle1", args, n);
    XtManageChild(Toggle1);

    XmStringFree(tcs);

    n = 0;
    tcs = XmStringGenerate("with PATH_MODE_FULL ",
			   XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
    XtSetArg(args[n], XmNlabelString, tcs);                    n++;
    Label1 = XmCreateLabel(RC1, "Label1", args, n);
    XtManageChild(Label1);

    n = 0;
    tcs = XmStringCreateSimple(
      "/users/qauser/FS/fourteencharac/");
    tcs1 = XmStringCreateSimple("Look for:");
    tcs2 = XmStringCreateSimple("b*");

    XtSetArg(args[n], XmNdirMask, tcs);  		       n++;
    XtSetArg(args[n], XmNdirSpec, NULL); 		       n++;
    XtSetArg(args[n], XmNfilterLabelString, tcs1);	       n++;
    XtSetArg(args[n], XmNpattern, tcs2);		       n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                  n++;
    XtSetArg(args[n], XmNpathMode, XmPATH_MODE_FULL);          n++;
    XtSetArg(args[n], XmNfileFilterStyle, XmFILTER_NONE);      n++;
    FileSelectionBox1 = XmCreateFileSelectionBox(RC1,
				    "FileSelectionBox1", args, n);
    XtManageChild(FileSelectionBox1);

    XmStringFree(tcs);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);      n++;
    XtSetArg(args[n], XmNtopWidget, TextF1);                   n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget, RC1);                     n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);             n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);     n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                  n++;
    Separator = XmCreateSeparator(Form, "Separator", args, n);
    XtManageChild(Separator);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);      n++;
    XtSetArg(args[n], XmNtopWidget, TextF1);                   n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget, Separator);               n++;
    XtSetArg(args[n], XmNnumColumns, 1);                       n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL);             n++;
    RC2 = XmCreateRowColumn(Form, "RC2", args, n);
    XtManageChild(RC2);
    
    n = 0;
    tcs = XmStringGenerate("Click and try again ",
			   XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
    XtSetArg(args[n], XmNlabelString, tcs);                     n++;
    XtSetArg(args[n], XmNhighlightColor, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNselectColor, XmHIGHLIGHT_COLOR);       n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY_ROUND);   n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                  n++;
    ToggleG2 = XmCreateToggleButtonGadget(RC2, "ToggleG2", args, n);
    XtManageChild(ToggleG2);

    XmStringFree(tcs);
    
    n = 0;
    tcs = XmStringGenerate("with PATH_MODE_RELATIVE ",
			   XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
    XtSetArg(args[n], XmNlabelString, tcs);                    n++;
    LabelG2 = XmCreateLabel(RC2, "LabelG2", args, n);
    XtManageChild(LabelG2);

    XmStringFree(tcs);
   
    n = 0;
    tcs = XmStringCreateSimple(
      "/users/qauser/FS/fourteencharac/");
    tcs1 = XmStringCreateSimple("In this directory: ");
    tcs2 = XmStringCreateSimple("Look for:");
    tcs3 = XmStringCreateSimple("b*");

    XtSetArg(args[n], XmNdirMask, tcs);  		       n++;
    XtSetArg(args[n], XmNdirTextLabelString, tcs1);	       n++;
    XtSetArg(args[n], XmNdirSpec, NULL); 		       n++;
    XtSetArg(args[n], XmNfilterLabelString, tcs2);	       n++;
    XtSetArg(args[n], XmNpattern, tcs3);		       n++;
    XtSetArg(args[n], XmNpathMode, XmPATH_MODE_RELATIVE);      n++;
    XtSetArg(args[n], XmNfileFilterStyle, XmFILTER_HIDDEN_FILES); n++;
    XtSetArg(args[n], XmNshadowThickness, 1);                  n++;
    FileSelectionBox2 = XmCreateFileSelectionBox(RC2,
				    "FileSelectionBox2", args, n);
    XtManageChild(FileSelectionBox2);

    XmStringFree(tcs);
    XmStringFree(tcs1);
    XmStringFree(tcs2);
    XmStringFree(tcs3);

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}


