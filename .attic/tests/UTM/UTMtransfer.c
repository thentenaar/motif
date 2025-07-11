/* $TOG: UTMtransfer.c /main/6 1999/11/17 12:49:22 jff $ */
/*
 *  @OSF_COPRIGHT@
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
#include <stdio.h>
#include <Xm/XmP.h>           /* Motif Toolkit */
#include "UTMtransfer.h"

/* Internal functions */
static void SetModeCB(Widget widget, XtPointer client_data, 
		      XtPointer call_data);
static void CutCB(Widget widget, XtPointer client_data, XtPointer call_data);
static void CopyCB(Widget widget, XtPointer client_data, XtPointer call_data);
static void PasteCB(Widget widget, XtPointer client_data,XtPointer call_data); 
static void PasteMoveCB(Widget widget, XtPointer client_data, 
			XtPointer call_data);
static void InputCB(Widget w, XtPointer client_data, XtPointer call_data);
static int ErrorHandler(Display *display, XErrorEvent *event);

/* String to use for Clipboard tagging */
XmString generic_label;

/* This tracks which transfer selection we're using.  Drag and drop isn't
   listed since it is dynamic,  we don't take ownership of DnD */
TransferMode Tmode = Mprimary;

char *TargetType[12] = { "ATOM", "DRAWABLE", "WINDOW", "COLORMAP", "INTEGER",
			 "PIXEL", "COMPOUND_TEXT", "STRING", "SPAN", "",
			 "_MOTIF_COMPOUND_STRING", "NONE" };

/*************************************************************************
 * A list of widgets we need to be able to find outside the main
 * program.
 *
 * MainW        is the child of the toplevel Shell1,  an XmMainWindow
 * target       is the drawing area used for drag and drop
 * list         is the list of targets displayed
 * labelV       contains the value of the selected target
 * labelN       contains the name of the selected or new target
 * typeOM       is the option menu which displays type information for the
 *              selected or new target
 * OwnPrimLG    is the label displayed when the drawing area owns the
 *              primary selection
 * OwnSecLG     is the label displayed when the drawing area owns the
 *              secondary selection
 *************************************************************************/

Widget MainW, list, target, labelV, labelN, typeOM, OwnPrimLG, OwnSecLG;

/*
 *  Main program
 */
void
main(int argc, char **argv)
{
    Arg args[15];
    int n;
    Widget menubar, pulldown, menu_btn;
    Widget CutPB, CopyPB, SeparatorG, PastePB, PasteMovePB;
    Widget form, optionMenu, SelectionMenu;
    Widget PrimaryPB, SecondaryPB, ClipboardPB;
    Widget frame, SelOwnLG;
    Widget frame1, frameLabel, workArea, field1, label1, field2, label2;
    Widget buttons, TypeMenu;
    Widget ATOMbutton, DRAWABLEbutton, WINDOWbutton, COLORMAPbutton;
    Widget INTEGERbutton, PIXELbutton, COMPOUND_TEXTbutton, STRINGbutton;
    Widget SPANbutton, LOCALE_TEXTbutton, _MOTIF_COMPOUND_STRINGbutton;
    Widget unknownbutton;
    XmString Label;
 
    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 2); n++;
    XtSetArg(args[n], XmNwidth, 400); n++;
    XtSetArg(args[n], XmNheight, 550); n++;
    MainW = XmCreateMainWindow(Shell1, "MainW", args, n);
    XtManageChild(MainW);

    /* 
     * Create a menu bar
     */
    n = 0;
    menubar = XmCreateMenuBar(MainW, "menubar", args, n);
    XtManageChild(menubar);

    n = 0;
    pulldown = XmCreatePulldownMenu(menubar, "pulldown", args, n);
    
    Label = XmStringCreate("Edit", XmSTRING_DEFAULT_CHARSET);
    n = 0;
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNsubMenuId, pulldown);  n++;
    menu_btn = XmCreateCascadeButton(menubar, "menu_btn", args, n);
    XtManageChild(menu_btn);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Cut", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CutPB = XmCreatePushButtonGadget(pulldown, "CutPB", args, n);
    XtManageChild(CutPB);
    XmStringFree(Label);
    XtAddCallback(CutPB, XmNactivateCallback, CutCB, NULL);

    n = 0;
    Label = XmStringCreate("Copy", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    CopyPB = XmCreatePushButtonGadget(pulldown, "CopyPB", args, n);
    XtManageChild(CopyPB);
    XmStringFree(Label);
    XtAddCallback(CopyPB, XmNactivateCallback, CopyCB, NULL);

    n = 0;
    SeparatorG = XmCreateSeparatorGadget(pulldown, "SeparatorG", args, n);
    XtManageChild(SeparatorG);
    
    n = 0;
    Label = XmStringCreate("Paste (copy)", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PastePB = XmCreatePushButtonGadget(pulldown, "PastePB", args, n);
    XtManageChild(PastePB);
    XmStringFree(Label);
    XtAddCallback(PastePB, XmNactivateCallback, PasteCB, NULL);

    n = 0;
    Label = XmStringCreate("Paste (move)", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    PasteMovePB = XmCreatePushButtonGadget(pulldown, "PasteMovePB", args, n);
    XtManageChild(PasteMovePB);
    XmStringFree(Label);
    XtAddCallback(PasteMovePB, XmNactivateCallback, PasteMoveCB, NULL);

    /* 
     * Create a form which contains:
     * - an option menu for choosing the type of selection,
     * - a drawing area for dragging the target,
     * - a label displaying the selection ownership value,
     * - the value of the selected target,
     * - the name of the selected target,
     * - an option menu with the possible target types,
     * - a list of targets.
     */

    n = 0;
    form = XmCreateForm(MainW, "form", args, n);
    XtManageChild(form);

    n = 0;
    SelectionMenu = XmCreatePulldownMenu(form, "SelectionMenu", args, n);

    n = 0;
    Label = XmStringCreate("Primary", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    PrimaryPB = XmCreatePushButtonGadget(SelectionMenu, "PrimaryPB", args, n);
    XtManageChild(PrimaryPB);
    XtAddCallback(PrimaryPB, XmNactivateCallback, SetModeCB, 
		  (XtPointer) Mprimary);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Secondary", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    SecondaryPB = XmCreatePushButtonGadget(SelectionMenu, "SecondaryPB", 
					   args, n);
    XtManageChild(SecondaryPB);
    XtAddCallback(SecondaryPB, XmNactivateCallback, SetModeCB, 
		  (XtPointer) Msecondary);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Clipboard", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    ClipboardPB = XmCreatePushButtonGadget(SelectionMenu, "ClipboardPB", 
					   args, n);
    XtManageChild(ClipboardPB);
    XtAddCallback(ClipboardPB, XmNactivateCallback, SetModeCB, 
		  (XtPointer) Mclipboard);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Selection", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNresizable, True); n++;
    XtSetArg(args[n], XmNsubMenuId, SelectionMenu); n++;
    optionMenu = XmCreateOptionMenu(form, "optionMenu", args, n);
    XtManageChild(optionMenu);
    XmStringFree(Label);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNresizable, True); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNleftWidget, optionMenu); n++;
    frame = XmCreateFrame(form, "frame", args, n);
    XtManageChild(frame);

    n =0;
    XtSetArg(args[n], XmNwidth, 70); n++;
    XtSetArg(args[n], XmNheight, 70); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor ("red")); n++;
    target = XmCreateDrawingArea(frame, "target", args, n);
    XtManageChild(target);

    /* Add the UTM callbacks on the drawing area */

    XtAddCallback(target, XmNdestinationCallback, TargetDestinationCB, NULL);
    XtAddCallback(target, XmNconvertCallback, TargetConvertCB, NULL);

    /* Add other callbacks on the drawing area */
    XtAddCallback(target, XmNinputCallback, InputCB, NULL);
    

    n = 0;
    Label = XmStringCreate("Selection Ownership: ", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, frame); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    SelOwnLG = XmCreateLabelGadget(form, "SelOwnLG", args, n);
    XtManageChild(SelOwnLG);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Primary", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, SelOwnLG);
    XtSetArg(args[n], XmNtopOffset, 30); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, frame); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    OwnPrimLG = XmCreateLabelGadget(form, "OwnPrimLG", args, n);
    XtManageChild(OwnPrimLG);
    XtUnmanageChild(OwnPrimLG);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Secondary", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, SelOwnLG);
    XtSetArg(args[n], XmNtopOffset, 50); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, frame); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    OwnSecLG = XmCreateLabelGadget(form, "OwnSecLG", args, n);
    XtManageChild(OwnSecLG);
    XtUnmanageChild(OwnSecLG);
    XmStringFree(Label);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 10); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame); n++;
    XtSetArg(args[n], XmNtopOffset, 6); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNresizable, True); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    frame1 = XmCreateFrame(form, "frame1", args, n);
    XtManageChild(frame1);
    
    n = 0;
    Label = XmStringCreate("Target Values", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD); n++;
    frameLabel = XmCreateLabelGadget(frame1, "frameLabel", args, n);
    XtManageChild(frameLabel);
    XmStringFree(Label);

    n = 0;
    workArea = XmCreateWorkArea(frame1, "workArea", args, n);
    XtManageChild(workArea);

    n = 0;
    XtSetArg(args[n], XmNrubberPositioning, True); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
    field2 = XmCreateForm(workArea, "field2", args, n);
    XtManageChild(field2);

    n = 0;
    Label = XmStringCreate("Target", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNwidth, 72); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNresizable, False); n++;
    label2 = XmCreateLabelGadget(field2, "label2", args, n);
    XtManageChild(label2);
    XmStringFree(Label);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNresizable, True); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNleftWidget, label2); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    labelN = XmCreateLabel(field2, "labelN", args, n);
    XtManageChild(labelN);

    n = 0;
    XtSetArg(args[n], XmNrubberPositioning, True); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
    field1 = XmCreateForm(workArea, "field1", args, n);
    XtManageChild(field1);

    n = 0;
    Label = XmStringCreate("Target Value", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 10); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 10); n++;
    XtSetArg(args[n], XmNresizable, False); n++;
    label1 = XmCreateLabelGadget(field1, "label1", args, n);
    XtManageChild(label1);
    XmStringFree(Label);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNresizable, True); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNleftWidget, label1); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    labelV = XmCreateLabel(field1, "labelV", args, n);
    XtManageChild(labelV);

    n = 0;
    TypeMenu = XmCreatePulldownMenu(workArea, "TypeMenu", args, n);

    n = 0;
    Label = XmStringCreate("ATOM", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    ATOMbutton = XmCreatePushButtonGadget(TypeMenu, "ATOMbutton", args, n);
    XtManageChild(ATOMbutton);
    XtAddCallback(ATOMbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[0]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("DRAWABLE", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    DRAWABLEbutton = XmCreatePushButtonGadget(TypeMenu, "DRAWABLEbutton", 
					      args, n);
    XtManageChild(DRAWABLEbutton);
    XtAddCallback(DRAWABLEbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[1]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("WINDOW", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    WINDOWbutton = XmCreatePushButtonGadget(TypeMenu, "WINDOWbutton", args, n);
    XtManageChild(WINDOWbutton);
    XtAddCallback(WINDOWbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[2]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("COLORMAP", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    COLORMAPbutton = XmCreatePushButtonGadget(TypeMenu, "COLORMAPbutton", 
					      args, n);
    XtManageChild(COLORMAPbutton);
    XtAddCallback(COLORMAPbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[3]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("INTEGER", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    INTEGERbutton = XmCreatePushButtonGadget(TypeMenu, "INTEGERbutton", 
					      args, n);
    XtManageChild(INTEGERbutton);
    XtAddCallback(INTEGERbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[4]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("PIXEL", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    PIXELbutton = XmCreatePushButtonGadget(TypeMenu, "PIXELbutton", args, n);
    XtManageChild(PIXELbutton);
    XtAddCallback(PIXELbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[5]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("COMPOUND_TEXT", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    COMPOUND_TEXTbutton = XmCreatePushButtonGadget(TypeMenu, 
						   "COMPOUND_TEXTbutton", 
						   args, n);
    XtManageChild(COMPOUND_TEXTbutton);
    XtAddCallback(COMPOUND_TEXTbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[6]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("STRING", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    STRINGbutton = XmCreatePushButtonGadget(TypeMenu, "STRINGbutton", args, n);
    XtManageChild(STRINGbutton);
    XtAddCallback(STRINGbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[7]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("SPAN", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    SPANbutton = XmCreatePushButtonGadget(TypeMenu, "SPANbutton", args, n);
    XtManageChild(SPANbutton);
    XtAddCallback(SPANbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[8]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("LOCALE_TEXT", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    LOCALE_TEXTbutton = XmCreatePushButtonGadget(TypeMenu, 
						 "LOCALE_TEXTbutton",
						 args, n);
    XtManageChild(LOCALE_TEXTbutton);
    XtAddCallback(LOCALE_TEXTbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[9]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("MOTIF COMPOUND STRING", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    _MOTIF_COMPOUND_STRINGbutton = XmCreatePushButtonGadget(TypeMenu, 
			"_MOTIF_COMPOUND_STRINGbutton", args, n);
    XtManageChild(_MOTIF_COMPOUND_STRINGbutton);
    XtAddCallback(_MOTIF_COMPOUND_STRINGbutton, XmNactivateCallback, 
		  SetTypeCB, &TargetType[10]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("unknown", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    unknownbutton = XmCreatePushButtonGadget(TypeMenu, "unknownbutton", args, 
					     n);
    XtManageChild(unknownbutton);
    XtAddCallback(unknownbutton, XmNactivateCallback, SetTypeCB, 
		  &TargetType[11]);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("Type", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label);  n++;
    XtSetArg(args[n], XmNsubMenuId, TypeMenu); n++;
    typeOM = XmCreateOptionMenu(workArea, "typeOM", args, n);
    XtManageChild(typeOM);
    XmStringFree(Label);

    n = 0;
    XtSetArg(args[n], XmNwidth, 250); n++;
    XtSetArg(args[n], XmNheight, 240); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, frame1); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    XtSetArg(args[n], XmNresizable, False); n++;
    XtSetArg(args[n], XmNleftWidget, frame1); n++;
    XtSetArg(args[n], XmNleftOffset, 0); n++;
    XtSetArg(args[n], XmNrightWidget, frame1); n++;
    XtSetArg(args[n], XmNrightOffset, 0); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT); n++;
    list = XmCreateScrolledList(form, "list", args, n);
    XtManageChild(list);
    XtAddCallback(list, XmNsingleSelectionCallback, ListSelectItemCB, NULL);

    XmMainWindowSetAreas(MainW, menubar, NULL, NULL, NULL, form);

   /* Create a default label string for the clipboard */

    generic_label = XmStringCreateLocalized("Generic Label");

    /* Register the drawing area as a drop site */

    XmeDropSink(target, NULL, 0);

    /* Install our special error handler */
    XSetErrorHandler((XErrorHandler) ErrorHandler);

    XtRealizeWidget(Shell1);
    
    for (n = 0; n < 17; n++)
      CommonPause();

    XtAppMainLoop(app_context);
}

/***************************************************
 * function setMode                               
 * Set the mode to PRIMARY, SECONDARY or CLIPBOARD
 ***************************************************/
static void 
SetModeCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
  Tmode = (int) client_data;
}

/******************************************************************
 * cut, copy and paste perform the apropriate actions
 * depending on which mode we're in.  They use the widget writer's
 * API for this.  Normally application developers won't need to
 * call these,  but this is a pseudo widget writing case as we're
 * adding interchange behavior to drawing area.  This is an example
 * of where the widget writing API should be used by app. developers
 ******************************************************************/

static void 
CutCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySource(target, 0);
    XtManageChild(OwnPrimLG);
  } else if (Tmode == Msecondary) {
    XmeSecondarySource(target, 0);
    XmeSecondaryTransfer(target, 
                         XInternAtom(XtDisplay(widget), 
                                     XmSCOMPOUND_TEXT, False),
                         XmMOVE, 0);
  } else if (Tmode == Mclipboard)
    XmeClipboardSource(target, XmMOVE, 0);
}


static void 
CopyCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySource(target, 0);
    XtManageChild(OwnPrimLG);
  } else if (Tmode == Msecondary) {
    XmeSecondarySource(target, 0);
    XmeSecondaryTransfer(target, 
                         XInternAtom(XtDisplay(widget), 
                                     XmSCOMPOUND_TEXT, False),
                         XmCOPY, 0);
  } else if (Tmode == Mclipboard)
    XmeClipboardSource(target, XmCOPY, 0);
}

static void 
PasteCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
  if (Tmode == Mprimary)
    XmePrimarySink(target, XmCOPY, 0, 0);
  else if (Tmode == Msecondary) {
    XmeSecondarySink(target, 0);
    XtManageChild(OwnSecLG);
  }
  else if (Tmode == Mclipboard)
    XmeClipboardSink(target, XmCOPY, 0);
}

static void 
PasteMoveCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
  if (Tmode == Mprimary)
    XmePrimarySink(target, XmMOVE, 0, 0);
  else if (Tmode == Msecondary) {
    XmeSecondarySink(target, 0);
    XtManageChild(OwnSecLG);
  }
  else if (Tmode == Mclipboard)
    XmeClipboardSink(target, XmMOVE, 0);
}

/* Error handler for X protocol errors.  Display error text in labelN,V */

static int 
ErrorHandler(Display *display, XErrorEvent *event)
{
  char buffer[80];
  char errortext[100];
  XmString tmp;
  int n;
  Arg args[2];

  XGetErrorText(display, event -> error_code, (char *) errortext, 100);

  tmp = XmStringCreateLocalized(errortext);

  n = 0;
  XtSetArg(args[n], XmNlabelString, tmp); n++;
  XtSetValues(labelN, args, n);

  XmStringFree(tmp);

  sprintf(buffer, "XID %d serial %d major %d minor %d",
          event -> resourceid,
	  event -> serial,
          event -> request_code,
          event -> minor_code);

  tmp = XmStringCreateLocalized(buffer);

  n = 0;
  XtSetArg(args[n], XmNlabelString, tmp); n++;
  XtSetValues(labelV, args, n);

  XmStringFree(tmp);

  return 0;
}

static void 
InputCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmDrawingAreaCallbackStruct *cbstruct;
  XEvent *event;

  cbstruct = (XmDrawingAreaCallbackStruct *) call_data;
  event = cbstruct -> event;

  if (event -> type == ButtonPress &&
      event -> xbutton.button == 2) {
    XmeDragSource(w, NULL, event, NULL, 0);
  }
}

