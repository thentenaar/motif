/* $TOG: CommScrTxt2.c /main/7 1997/10/02 15:40:55 csn $ */
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


typedef	struct	_FlagWidgetPair
	{
	int		flag;
	Widget	w;
	}	FlagWidgetPair;

static char new_string1[] = "\
As I was going to St. Ives\n\
I met a man with seven wives\n\
Each wife had seven sacks,\n\
Each sack had seven cats,\n\
Each cat had seven kits\n\
Kits, cats, sacks, and wives,\n\
How many were going to St. Ives?";

static char new_string2[] = "\
Jack Sprat could eat no fat\n\
His wife could eat no lean\n\
And so between them both\n\
They licked the platter clean";

static char new_string3[] = "\
Just one line of text.";

static char font_name1[] = "8x13";
static char render_tag[] = "render1";

#define NORMAL_MODE False
#define ADD_MODE    True

Widget BBoard1;
Widget Text1, Text2, HiddenText;
Widget Label1, Label2;
void		*Source1, *Source2, *saveSource2;
Arg args[MAX_ARGS];
int n, ndx;
Boolean AddMode = NORMAL_MODE;


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
			printf("Scrolled Text widget\n");
			}
		else
			{
			printf("SingleLine Text widget\n");
			}
		break;
		}
	case XmCR_LOSE_PRIMARY:
		{
		printf("Lose Primary Selection ownership for ");
		if (client_widget == Text1)
			{
			printf("Scrolled Text widget\n");
			}
		else
			{
			printf("SingleLine Text widget\n");
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
#define TW_COPYLINK     5
#define TW_PASTELINK    6
#define	TW_REMOVE	7
#define	TW_CURSPOS	8
#define	TW_HILINORM	9
#define	TW_HILISELECT   10	
#define	TW_HILISECOND   11	
#define	TW_SCROLLUP	12
#define	TW_SCROLLDN	13
#define	TW_CHGSOURCE	14
#define	TW_CHGADDMODE	15
#define TW_SETSOURCE    16

static void PopupCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
	FlagWidgetPair  *client_fwp;
	XmTextPosition  newpos, lastpos, newbeg, leftpos, rightpos;
	XButtonEvent *bev;

	client_fwp = (FlagWidgetPair *)client_data;
	bev = (XButtonEvent *)((XmPushButtonCallbackStruct *)call_data)->event;

	if (client_fwp->w == Text1)
		{
		printf("Scrolled Text widget: ");
		}
	else
		{
		printf("SingleLine Text widget: ");
		}

	if	(XmTextGetSelectionPosition(client_fwp->w, &leftpos, 
					    &rightpos))
		{
		printf("Selection Position left: %d, right: %d\n", leftpos, 
		       rightpos);
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
                case TW_COPYLINK:
                        {
                        if      (!XmTextCopyLink(client_fwp->w, bev->time))
                                printf("CopyLink failed!\n");
                        break;
                        }
                case TW_PASTELINK:
                        {
                        if      (!XmTextPasteLink(client_fwp->w))
                                printf("PasteLink failed!\n");
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
		case TW_SCROLLUP:
			{
			XmTextScroll(client_fwp->w, 2);
			break;
			}
		case TW_SCROLLDN:
			{
			XmTextScroll(client_fwp->w, -3);
			break;
			}
		case TW_CHGSOURCE:
			{
			if (XmTextGetSource(Text2) == Source1)
				/* set Text2 source to Source2 */
                                XmTextSetSource( Text2, saveSource2,
					(XmTextPosition)0, (XmTextPosition)0);
			else
				/* set Text2 source to Source1 */
				XmTextSetSource(Text2, Source1,
					(XmTextPosition)0, (XmTextPosition)0);
			break;
			}
		case TW_CHGADDMODE:
			{
                        AddMode = (AddMode == NORMAL_MODE) ? ADD_MODE :
                                  NORMAL_MODE;
			XmTextSetAddMode(client_fwp->w, AddMode );
			break;
			}
                case TW_SETSOURCE:
                        /*
                         *  Set Text2 source to Text1 while setting
                         *  the cursor position beyond the bounds of
                         *  the text.
                         */
                         XmTextSetSource(Text1, XmTextGetSource(Text2),
                                         0, strlen(new_string3) + 50 );
                         break;

		default:
			{
			printf("Bad Button number!!!\n"); break;
			}
	}
}

static void PostIt(Widget w, XtPointer client_data,
                                XEvent *call_data, Boolean *c)
{
	if (((XButtonEvent *)call_data)->button != Button3)
		return;
	XmMenuPosition((Widget)client_data, (XButtonPressedEvent *)call_data);
	XtManageChild((Widget)client_data);
}


#define	BTN_COUNT	16
void main_scrtext2(argc, argv)
     int argc;
     char **argv;
{
        Widget SWin;
        Widget			Popup1Btn[BTN_COUNT], Popup1, SetSourcePB;
        Widget			Popup2Btn[BTN_COUNT], Popup2, SwitchSourcePB;
        FlagWidgetPair	FWP1[BTN_COUNT];
        FlagWidgetPair	FWP2[BTN_COUNT];
        Position	tx, ty;
        XmString tmp_string = NULL;
        int      set_source_index;
        XmRendition  rendition;
        XmRenderTable renderTab = NULL;
        static char inserted[] = "**THIS TEXT INSERTED**";

        /* initialize toolkit */
        CommonTestInit(argc, argv);
    
        /* create unmanaged text widget to store Source in */
        HiddenText = XmCreateText(Shell1, "HiddenText", NULL, 0);

        n = 0;
        BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);
        XtManageChild(BBoard1);


        /* create scrolled text widget */
        n = 0;
        tmp_string = XmStringCreateLtoR("Scrolled Text widget", 
				XmSTRING_DEFAULT_CHARSET); 
        XtSetArg(args[n], XmNlabelString,tmp_string); n++;
		 
        Label1 = XmCreateLabel(BBoard1, "Label1", args, n);
        XtManageChild(Label1);
        XmStringFree (tmp_string);

	n = 0;
	XtSetArg(args[n], XmNwidth, 350); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	XtSetArg(args[n], XmNscrollVertical, True); n++;
	XtSetArg(args[n], XmNscrollHorizontal, True); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, new_string2); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1)); n++;
	Text1 = XmCreateScrolledText(BBoard1, "Text1", args, n);
	n = 0;
	XtSetArg(args[n], XmNy, 30); n++;
	XtSetValues(XtParent(Text1), args, n);

	XtAddCallback(Text1, XmNgainPrimaryCallback, PrimaryCB, Text1);
	XtAddCallback(Text1, XmNlosePrimaryCallback, PrimaryCB, Text1);
	XtManageChild(Text1);

	printf("TextPosition for Scrolled Text widget at x=21, y=21 is: %d\n",
		XmTextXYToPos(Text1, 21, 21));
	printf("TextPosition for Scrolled Text widget at x=31, y=21 is: %d\n",
		XmTextXYToPos(Text1, 31, 21));
	if (XmTextPosToXY(Text1, XmTextGetLastPosition(Text1), &tx, &ty))
	  {
	  printf("XY of last character in Scrolled Text widget is: %d, %d\n",
		   tx, ty);
	  }
	Source1 = XmTextGetSource(Text1);

	Popup1 = XmCreatePopupMenu(Text1, "Popup1", NULL, 0);
	XtAddEventHandler(Text1, ButtonPressMask, False, PostIt, Popup1);

	ndx = 0;

	n = 0;
        tmp_string = XmStringCreateLtoR("Text Functions", 
				    XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreateLabelGadget(Popup1, "Title1",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
	Popup1Btn[ndx++] = XmCreateSeparatorGadget(Popup1, "separator1",
						args, n); 


	n = 0;
        tmp_string = XmStringCreateLtoR("Cut", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Cut1",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("Copy", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Copy1",
						args, n);
        XmStringFree (tmp_string);
	
        n = 0;
        tmp_string = XmStringCreateLtoR("Paste", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Paste1",
						args, n);
        XmStringFree (tmp_string);

        n = 0;
        tmp_string = XmStringCreateLtoR("CopyLink", XmSTRING_DEFAULT_CHARSET);
        XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "CopyLink1",
                                                args, n);
        XmStringFree (tmp_string);

        n = 0;
        tmp_string = XmStringCreateLtoR("PasteLink", XmSTRING_DEFAULT_CHARSET);
        XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "PasteLink1",
                                                args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string =  XmStringCreateLtoR("Remove", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Remove1",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("HiLi Normal", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString,tmp_string); n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Normal1",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string=XmStringCreateLtoR("HiLi Selected", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Selected1", 
						args, n);

        XmStringFree(tmp_string);



	n = 0;
        tmp_string = XmStringCreateLtoR("HiLi Secondary", 
				    XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string);	n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "HiLi_Secondary1",
						args, n);
        XmStringFree(tmp_string);


	n = 0;
        tmp_string = XmStringCreateLtoR("Scroll Up 2", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); 	n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Scroll_Up_21",
						args, n);
        XmStringFree (tmp_string);


        /*
         *  Save the index for the SetSourcePB which will be
         *  added to the menu later.
         */
        set_source_index = ndx;

	n = 0;
        tmp_string= XmStringCreateLtoR("Scroll Down 3", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Scroll_Down_31", 
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("Toggle AddMode", 
				    XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string);n++;
	Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "Toggle_AddMode1", 
						args, n);
        XmStringFree (tmp_string);

	XtManageChildren( Popup1Btn, ndx);

	ndx = 2;
	FWP1[ndx-2].flag = TW_CUT; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_COPY; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_PASTE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
        FWP1[ndx-2].flag = TW_COPYLINK; FWP1[ndx-2].w = Text1;
        XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
                                        PopupCB, &FWP1[ndx-2]); ndx++;
        FWP1[ndx-2].flag = TW_PASTELINK; FWP1[ndx-2].w = Text1;
        XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
                                        PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_REMOVE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILINORM; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILISELECT; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_HILISECOND; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_SCROLLUP; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_SCROLLDN; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;
	FWP1[ndx-2].flag = TW_CHGADDMODE; FWP1[ndx-2].w = Text1;
	XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP1[ndx-2]); ndx++;

	/* create singleline text widget */
	n = 0;
        tmp_string = XmStringCreateLtoR("SingleLine Text widget", 	
				XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNy, 160); n++;
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Label2 = XmCreateLabel(BBoard1, "Label2", args, n);
	XtManageChild(Label2);
        XmStringFree (tmp_string);
	
        n = 0;
	XtSetArg(args[n], XmNy, 180); n++;
	XtSetArg(args[n], XmNwidth, 350); n++;
	XtSetArg(args[n], XmNselectionArrayCount, 2); n++;
	XtSetArg(args[n], XmNvalue, new_string1); n++;
	XtSetArg(args[n], XmNfontList, CommonGetFontList(font_name1)); n++;
	Text2 = XmCreateText(BBoard1, "Text2", args, n);

	XtAddCallback(Text2, XmNgainPrimaryCallback, PrimaryCB, Text2);
	XtAddCallback(Text2, XmNlosePrimaryCallback, PrimaryCB, Text2);
	XtManageChild(Text2);

	XmTextInsert( Text2, 3, inserted );

	printf("TextPosition for SingleLine Text widget at x=21, y=21 is: %d\n",
	       XmTextXYToPos(Text2, 21, 21));
	printf("TextPosition for SingleLine Text widget at x=31, y=21 is: %d\n",
	       XmTextXYToPos(Text2, 31, 21));
	if (XmTextPosToXY(Text2, XmTextGetLastPosition(Text2), &tx, &ty))
	  {
	  printf("XY of last character in SingleLine Text widget is: %d, %d\n",
		   tx, ty);
	  }
	Source2 = XmTextGetSource(Text2);
	/*
	 * save the current source of Text 2
	 * so it doesn't get freed 
	 */
	saveSource2 = Source2;
	XmTextSetSource(HiddenText, saveSource2, 0, 0);

	Popup2 = XmCreatePopupMenu(Text2, "Popup2", NULL, 0);
	XtAddEventHandler(Text2, ButtonPressMask, False, PostIt, Popup2);

	ndx = 0;

	n = 0;
        tmp_string = XmStringCreateLtoR("Text Functions", 
				    XmSTRING_DEFAULT_CHARSET);        
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreateLabelGadget(Popup2, "Title2",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
	Popup2Btn[ndx++] = XmCreateSeparatorGadget(Popup2, "separator2",
						args, n); 
	n = 0;
        tmp_string = XmStringCreateLtoR("Cut", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Cut2",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("Copy", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Copy2",
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("Paste", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Paste2",
						args, n);
        XmStringFree (tmp_string);

        n = 0;
        tmp_string = XmStringCreateLtoR("CopyLink", XmSTRING_DEFAULT_CHARSET); n++;
        XtSetArg(args[n], XmNlabelString, tmp_string); n++;
        Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "CopyLink2",
                                                args, n);
        XmStringFree (tmp_string);

        n = 0;
        tmp_string = XmStringCreateLtoR("PasteLink", XmSTRING_DEFAULT_CHARSET);
        XtSetArg(args[n], XmNlabelString, tmp_string); n++;

        Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "PasteLink2",
                                                args, n);
        XmStringFree (tmp_string);

	
        n = 0;
        tmp_string = XmStringCreateLtoR("Remove", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Remove2",
						args, n); 
        XmStringFree (tmp_string);
       
	n = 0;
        tmp_string = XmStringCreateLtoR("HiLi Normal", XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
		
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Normal2", 
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string =XmStringCreateLtoR("HiLi Selected", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Selected2", 
						args, n);
        XmStringFree (tmp_string);
        
	n = 0;
        tmp_string=XmStringCreateLtoR("HiLi Secondary", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "HiLi_Secondary2", 
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string = XmStringCreateLtoR("Scroll Up 2", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Scroll_Up_22", 
						args, n);
        XmStringFree (tmp_string);

	n = 0;
        tmp_string=XmStringCreateLtoR("Scroll Down 3", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Scroll_Down_32", 
						args, n);
        XmStringFree (tmp_string);
        
	n = 0;
        tmp_string=XmStringCreateLtoR("Toggle AddMode", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "Toggle_AddMode2", 
						args, n);
        XmStringFree (tmp_string);
	
        n = 0;
        tmp_string=XmStringCreateLtoR("Switch Source", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNlabelString, tmp_string); n++;
	SwitchSourcePB = Popup2Btn[ndx++] =
              XmCreatePushButtonGadget(Popup2, "Switch_Source2", 
						args, n);
        XmStringFree (tmp_string);

	ndx = 2;
	FWP2[ndx-2].flag = TW_CUT; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_COPY; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_PASTE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
        FWP2[ndx-2].flag = TW_COPYLINK; FWP2[ndx-2].w = Text2;
        XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
                                        PopupCB, &FWP2[ndx-2]); ndx++;
        FWP2[ndx-2].flag = TW_PASTELINK; FWP2[ndx-2].w = Text2;
        XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
                                         PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_REMOVE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILINORM; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILISELECT; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_HILISECOND; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_SCROLLUP; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_SCROLLDN; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_CHGADDMODE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	FWP2[ndx-2].flag = TW_CHGSOURCE; FWP2[ndx-2].w = Text2;
	XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
					PopupCB, &FWP2[ndx-2]); ndx++;
	XtManageChildren( Popup2Btn, ndx);

	XtRealizeWidget(Shell1);

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

	/* test case for PIR 3096, see .dat */
	CommonPause();
	/* end of test case for PIR 3096 */

	/* test case for PIR 2975, see .dat  */
	CommonPause();
	/* end of test case for PIR 2975 */

      /*  Assign new strings to the text widgets. */
        tmp_string = NULL;

        n = 0;
	XtSetArg(args[n], XmNvalue, ""); n++;
        XtSetValues( Text1, args, n );

        n = 0;
	XtSetArg(args[n], XmNvalue, new_string3 ); n++;
        XtSetValues( Text2, args, n );


        /*  Create new pushbutton for Scroll Text menu. */
        n = 0;
        tmp_string = XmStringCreateLtoR("Set Source at Invalid Position",
                                    XmSTRING_DEFAULT_CHARSET);
        XtSetArg(args[n], XmNlabelString, tmp_string);n++;
        SetSourcePB = XmCreatePushButtonGadget(Popup1, "Set_Source",
                                                args, n);

        XmStringFree (tmp_string);


        FWP1[set_source_index - 2].flag = TW_SETSOURCE;
        FWP1[set_source_index - 2].w = Text1;
        XtAddCallback(SetSourcePB, XmNactivateCallback,
                                PopupCB, &FWP1[set_source_index-2]);

        XtManageChild(SetSourcePB);

        /*
         *  Unmanage the Switch Source button from Single Line
         *  text widget - since the sources for both widgets
         *  are now shared.
         *  Don't want users mucking with the sources anymore.
         */
        XtUnmanageChild(SwitchSourcePB);

        /* test case for CR 3669 */
        CommonPause();

        CommonPause();
	XtAppMainLoop(app_context);
}




