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
static char rcsid[] = "$XConsortium: TextField2.c /main/9 1995/07/13 19:34:38 drk $"
#endif
#endif
#include <testlib.h>


#define NORMAL_MODE False
#define ADD_MODE    True


Widget BBoard1;
Widget Text1, Text2;
Widget Label1, Label2;
Arg args[MAX_ARGS];
int n, ndx;
Boolean AddMode1 = NORMAL_MODE, AddMode2 = NORMAL_MODE;

char new_string1[] = "\
As I was going to St. Ives\n\
I met a man with seven wives\n\
Each wife had seven sacks,\n\
Each sack had seven cats,\n\
Each cat had seven kits\n\
Kits, cats, sacks, and wives,\n\
How many were going to St. Ives?";

char new_string2[] = "\
Jack Sprat could eat no fat\n\
His \twife could eat no lean\n\
And 	so between them both\n\
They licked the platter clean";

/* test for PIR2806 */
char new_string3[] = "new_string";

typedef	struct	_FlagWidgetPair
	{
	int		flag;
	Widget	w;
	}	FlagWidgetPair;

static void PrimaryCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	Widget client_widget;
	XmAnyCallbackStruct *cb_data;

	cb_data = (XmAnyCallbackStruct *)call_data;
	client_widget = (Widget)client_data;

	switch(cb_data->reason)
	{
	case XmCR_GAIN_PRIMARY:
		{
		printf("Gain Primary Selection ownership for ");
		if (client_widget == Text1)
			{
			printf("MultiLine Text widget\n");
			}
		else
			{
			printf("TextField widget\n");
			}
		break;
		}
	case XmCR_LOSE_PRIMARY:
		{
		printf("Lose Primary Selection ownership for ");
		if (client_widget == Text1)
			{
			printf("MultiLine Text widget\n");
			}
		else
			{
			printf("TextField widget\n");
			}
		break;
		}
	default:
		{
		printf("PrimaryCB: bad reason %d\n", cb_data->reason);
		break;
		}
	}

}

#define	TW_CUT		2
#define TW_COPY		3
#define TW_PASTE	4
#define	TW_REMOVE	5
#define	TW_HILINORM	6
#define	TW_HILISELECT	7
#define	TW_HILISECOND	8
#define	TW_CHGADDMODE	9
#define	TW_SETNULL	10
#define	TW_GETSTRING	11
#define	TW_SETSTRING	12
/* test for PIR2806 */
#define TW_SETVALUE	13
#define	TW_SHOWPOS	14

/* callback stuff for Text widget */
static void PopupCB1(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	FlagWidgetPair	*client_fwp;
	XmTextPosition	newpos, lastpos, newbeg, leftpos, rightpos;
	XButtonEvent *bev;

	client_fwp = (FlagWidgetPair *)client_data;
	bev = (XButtonEvent *)((XmPushButtonCallbackStruct *)call_data)->event;
	printf("MultiLine Text widget: ");
	if (XmTextGetSelectionPosition(client_fwp->w, &leftpos, &rightpos))
		{
		printf("Selection Position left: %d, right: %d\n",
			leftpos, rightpos);
		}
	else
		{
		printf("Don't own primary selection ...\n");
		}

	switch (client_fwp->flag)
	{
	case TW_CUT: 
		{
		if	(!XmTextCut(client_fwp->w, bev->time))
			printf("Cut failed!\n");
		break;
		}
	case TW_COPY:
		{
		if	(!XmTextCopy(client_fwp->w, bev->time))
			printf("Copy failed!\n");
		break;
		}
	case TW_PASTE:
		{
		if	(!XmTextPaste(client_fwp->w))
			printf("Paste failed!\n");
		break;
		}
	case TW_REMOVE:
		{
		if	(!XmTextRemove(client_fwp->w))
			printf("Remove failed!\n");
		break;
		}
	case TW_HILINORM:
		{
		newpos = XmTextGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_NORMAL);
		break;
		}
	case TW_HILISELECT:
		{
		newpos = XmTextGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_SELECTED);
		break;
		}
	case TW_HILISECOND:
		{
		newpos = XmTextGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_SECONDARY_SELECTED);
		break;
		}
	case TW_CHGADDMODE:
		{
                AddMode1 = (AddMode1 == NORMAL_MODE) ? ADD_MODE : NORMAL_MODE;
		XmTextSetAddMode(client_fwp->w, AddMode1 );
		break;
		}
	case TW_SETNULL:
		{
		XmTextSetString(client_fwp->w, "");
		break;
		}
	case TW_GETSTRING:
		{
		printf("GetString (between ##'s): ##%s##\n",
			XmTextGetString(client_fwp->w));
		break;
		}
	case TW_SETSTRING:
		{
		XmTextSetString(client_fwp->w, new_string1);
		break;
		}
	/* test for PIR2806 */
	case TW_SETVALUE:
		{
		n = 0;
		XtSetArg(args[n], XmNvalue, new_string3); n++;
		XtSetValues(client_fwp->w, args, n);
		break;
		}
	default:
		{
		printf("Bad Button number!!!\n"); break;
		}
	}
}

/* callback stuff for TextField widget */
static void PopupCB2(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	FlagWidgetPair	*client_fwp;
	XmTextPosition	newpos, lastpos, newbeg, leftpos, rightpos;
	XButtonEvent *bev;

	client_fwp = (FlagWidgetPair *)client_data;
	bev = (XButtonEvent *)((XmPushButtonCallbackStruct *)call_data)->event;
	printf("TextField widget: ");

	if (XmTextFieldGetSelectionPosition(client_fwp->w,
		&leftpos, &rightpos))
		{
		printf("Selection Position left: %d, right: %d\n",
			leftpos, rightpos);
		}
	else
		{
		printf("Don't own primary selection ...\n");
		}

	switch (client_fwp->flag)
	{
	case TW_CUT: 
		{
		if	(!XmTextFieldCut(client_fwp->w, bev->time))
			printf("Cut failed!\n");
		break;
		}
	case TW_COPY:
		{
		if	(!XmTextFieldCopy(client_fwp->w, bev->time))
			printf("Copy failed!\n");
		break;
		}
	case TW_PASTE:
		{
		if	(!XmTextFieldPaste(client_fwp->w))
			printf("Paste failed!\n");
		break;
		}
	case TW_REMOVE:
		{
		if	(!XmTextFieldRemove(client_fwp->w))
			printf("Remove failed!\n");
		break;
		}
	case TW_HILINORM:
		{
		newpos = XmTextFieldGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextFieldSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_NORMAL);
		break;
		}
	case TW_HILISELECT:
		{
		newpos = XmTextFieldGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextFieldSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_SELECTED);
		break;
		}
	case TW_HILISECOND:
		{
		newpos = XmTextFieldGetInsertionPosition(client_fwp->w);
		newbeg = (newpos >= 5) ? (newpos - 5) : 0;
		XmTextFieldSetHighlight(client_fwp->w, newbeg, newpos,
			XmHIGHLIGHT_SECONDARY_SELECTED);
		break;
		}
	case TW_CHGADDMODE:
		{
                AddMode2 = (AddMode2 == NORMAL_MODE) ? ADD_MODE :
                           NORMAL_MODE;
		XmTextFieldSetAddMode(client_fwp->w, AddMode2 );
		break;
		}
	case TW_SETNULL:
		{
		XmTextFieldSetString(client_fwp->w, "");
		break;
		}
	case TW_GETSTRING:
		{
		printf("GetString (between ##'s): ##%s##\n",
			XmTextFieldGetString(client_fwp->w));
		break;
		}
	case TW_SETSTRING:
		{
		XmTextFieldSetString(client_fwp->w, new_string2);
		break;
		}
	/* test for PIR2806 */
	case TW_SETVALUE:
		{
		n = 0;
		XtSetArg(args[n], XmNvalue, new_string3); n++;
		XtSetValues(client_fwp->w, args, n);
		break;
		}
	case TW_SHOWPOS:
		{
		XmTextFieldShowPosition(client_fwp->w, (XmTextPosition)3);
		break;
		}
	default:
		{
		printf("Bad Button number!!!\n"); break;
		}
	}
}

static void PostIt(Widget w, XtPointer client_data,
                                XEvent *call_data, Boolean *c)
{
	if (((XButtonPressedEvent *)call_data)->button != Button3)
		return;
	XmMenuPosition((Widget)client_data, (XButtonPressedEvent *)call_data);
	XtManageChild((Widget)client_data);
}


#define	BTN_COUNT	15
int main(argc, argv)
int argc;
char **argv;
{
	Widget SWin;
	Widget	Popup1Btn[BTN_COUNT], Popup1;
	Widget	Popup2Btn[BTN_COUNT], Popup2;
	FlagWidgetPair	FWP1[BTN_COUNT];
	FlagWidgetPair	FWP2[BTN_COUNT];
	Position	tx, ty;
        XmString tmp_string;



	/* initialize toolkit */
	CommonTestInit(argc, argv);
    
	n = 0;
	BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
	XtManageChild(BBoard1);

	/* create multiline text widget */
	n = 0;
	tmp_string = XmStringCreate("MultiLine Text widget",
		     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	Label1 = XmCreateLabel(BBoard1, "Label1", args, n);
	XtManageChild(Label1);
        XmStringFree (tmp_string);


	n = 0;
	XtSetArg(args[n], XmNy, 30); n++;
	XtSetArg(args[n], XmNrows, 5); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13")); n++;
	Text1 = XmCreateText(BBoard1, "Text1", args, n);

	XtAddCallback(Text1, XmNgainPrimaryCallback, PrimaryCB,
				Text1);
	XtAddCallback(Text1, XmNlosePrimaryCallback, PrimaryCB,
				Text1);
	XtManageChild(Text1);

	printf("TextPosition for MultiLine Text widget at x=21, y=21 is: %d\n",
		XmTextXYToPos(Text1, 21, 21));
	printf("TextPosition for MultiLine Text widget at x=31, y=21 is: %d\n",
		XmTextXYToPos(Text1, 31, 21));
	if (XmTextPosToXY(Text1, XmTextGetLastPosition(Text1), &tx, &ty))
		{
		printf(
		"XY of last character in MultiLine Text widget is: %d, %d\n",
			tx, ty);
		}

	Popup1 = XmCreatePopupMenu(Text1, "Popup1", NULL, 0);
	XtAddEventHandler(Text1, ButtonPressMask, False, PostIt, 
			  (XtPointer) Popup1);

	ndx = 0;

	n = 0;
        tmp_string=XmStringCreate("Text Functions", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreateLabelGadget(Popup1, "Title1",
		args, n);
        XmStringFree (tmp_string);		



	n = 0;
	Popup1Btn[ndx++] = XmCreateSeparatorGadget(Popup1, "separator1",
		args, n);
	n = 0;
        tmp_string = XmStringCreate("Cut", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Cut1",
		args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("Copy", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string);

	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Copy1",
		args, n);
	XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("Paste", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Paste1",
		  args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("Remove", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Remove1",
		           args, n);
        XmStringFree (tmp_string);
	
        n = 0;
        tmp_string = XmStringCreate("HiLi Normal", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Normal1",
	  	  args, n);
        XmStringFree (tmp_string); 

	n = 0;
        tmp_string = XmStringCreate("HiLi Selected", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Selected1",
	 	  args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string=XmStringCreate("HiLi Secondary", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Secondary1",
		args, n);
        XmStringFree (tmp_string);
	
        n = 0;
        tmp_string=XmStringCreate("Toggle AddMode", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Toggle_AddMode1",
		  args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("Set String NULL", 
                                     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Set_String_NULL1",
		args, n);
        XmStringFree (tmp_string);



	n = 0;
        tmp_string = XmStringCreate("Get String", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Get_String1",
		args, n);
        XmStringFree (tmp_string);



	n = 0;
        tmp_string=XmStringCreate("Set String Orig", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Set_String_Orig1",
		args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string=XmStringCreate("SetValue String", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	/* test for PIR2806 */
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "SetValue_String1",
		args, n);
        XmStringFree (tmp_string);

	XtManageChildren(Popup1Btn, ndx);

	ndx = 2;
	FWP1[ndx-2].flag = TW_CUT; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_COPY; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_PASTE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_REMOVE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILINORM; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILISELECT; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILISECOND; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_CHGADDMODE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_SETNULL; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_GETSTRING; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_SETSTRING; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;
	/* test for PIR2806 */
	FWP1[ndx-2].flag = TW_SETVALUE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback, PopupCB1,
		&FWP1[ndx-2]); ndx++;

	/* create text field widget */
	n = 0;
        tmp_string = XmStringCreate("TextField widget", 
                                     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	XtSetArg(args[n], XmNy, 120); n++;
	Label2 = XmCreateLabel(BBoard1, "Label2", args, n);
	XtManageChild(Label2);
	XmStringFree (tmp_string);


        n = 0;
	XtSetArg(args[n], XmNy, 140); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	XtSetArg(args[n], XmNvalue, new_string2); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13")); n++;
	Text2 = XmCreateTextField(BBoard1, "Text2", args, n);
	XtAddCallback(Text2, XmNgainPrimaryCallback, PrimaryCB,
				Text2);
	XtAddCallback(Text2, XmNlosePrimaryCallback, PrimaryCB,
				Text2);
	XtManageChild(Text2);

	XmTextFieldInsert(Text2, 3, "**THIS TEXT INSERTED**");
	printf("TextPosition for TextField widget at x=21, y=21 is: %d\n",
		XmTextFieldXYToPos(Text2, 21, 21));
	printf("TextPosition for TextField widget at x=31, y=21 is: %d\n",
		XmTextFieldXYToPos(Text2, 31, 21));
	if (XmTextFieldPosToXY(Text2, XmTextFieldGetLastPosition(Text2),
		&tx, &ty))
		{
		printf(
		"XY of last character in TextField widget is: %d, %d\n",
			tx, ty);
		}
	printf("Text Baseline for TextField widget = %d\n",
		XmTextFieldGetBaseline(Text2));

	Popup2 = XmCreatePopupMenu(Text2, "Popup2", NULL, 0);
	XtAddEventHandler(Text2, ButtonPressMask, False, PostIt, 
			  (XtPointer) Popup2);

	ndx = 0;

	n = 0;
        tmp_string=XmStringCreate("TextF Functions", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreateLabelGadget(Popup2, "Title2",
		args, n);
        XmStringFree (tmp_string);

	n = 0;
	Popup2Btn[ndx++] = XmCreateSeparatorGadget(Popup2, "separator2",
		args, n);

	n = 0;
        tmp_string = XmStringCreate("Cut", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Cut2",
                                                    args, n);
        XmStringFree( tmp_string );

	n = 0;
        tmp_string = XmStringCreate("Copy", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		

	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Copy2",
		args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string = XmStringCreate("Paste", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string);
        Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Paste2",
		args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string = XmStringCreate("Remove", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Remove2",
		args, n);
        XmStringFree (tmp_string);
	
        n = 0;
        tmp_string = XmStringCreate("HiLi Normal", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Normal2",
		  args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string = XmStringCreate("HiLi Selected", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Selected2",
		  args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string=XmStringCreate("HiLi Secondary", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string);
        Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Secondary2",
		 args, n);
        XmStringFree (tmp_string);
        

	n = 0;
        tmp_string = XmStringCreate("Toggle AddMode", 
                                     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Toggle_AddMode2",
		args, n);
        XmStringFree (tmp_string);




	n = 0;
        tmp_string = XmStringCreate("Set String NULL", 
                                     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Set_String_NULL2",
		args, n);
        XmStringFree (tmp_string);


	n = 0;
        tmp_string = XmStringCreate("Get String", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Get_String2",
		args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreate("Set String Orig", 
                                     XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Set_String_Orig2",
		                                    args, n);
        XmStringFree (tmp_string);



	n = 0;
        tmp_string=XmStringCreate("SetValue String", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		

	/* test for PIR2806 */
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "SetValue_String2",
		args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string=XmStringCreate("Show Pos 3", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Show_Pos_32",
		args, n);
        XmStringFree (tmp_string);

	XtManageChildren(Popup2Btn, ndx);

	ndx = 2;
	FWP2[ndx-2].flag = TW_CUT; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_COPY; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_PASTE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_REMOVE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILINORM; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILISELECT; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILISECOND; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_CHGADDMODE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_SETNULL; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_GETSTRING; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_SETSTRING; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	/* test for PIR2806 */
	FWP2[ndx-2].flag = TW_SETVALUE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_SHOWPOS; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback, PopupCB2,
		&FWP2[ndx-2]); ndx++;

	XtRealizeWidget(Shell1);

	CommonPause();

	CommonPause();

	/* test case for PIR 2895, see .dat */
	CommonPause();
	/* end of test case for PIR 2895 */

	XtAppMainLoop(app_context);
}
