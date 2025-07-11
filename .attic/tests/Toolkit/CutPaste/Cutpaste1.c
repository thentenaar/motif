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
static char rcsid[] = "$XConsortium: Cutpaste1.c /main/8 1995/07/13 17:54:26 drk $"
#endif
#endif


#include <testlib.h>
#include "X11/Xatom.h"
#include <X11/StringDefs.h>


/*  Global Variables  */
Widget    Form1;
Widget    Text1;
Widget    RowCol1;
Widget    PushButton1;
Widget    PushButton2;
Widget    PushButton3;
Widget    PushButton4;
Widget    PushButton5;

void CopyFileToClipboard();
void DeletePrimarySelection();
void PasteItemFromClipboard();
static char *StatusToString();

void ButtonCB(w, client_data, call_data)
	Widget w;
     XtPointer client_data, call_data;
{
    XmAnyCallbackStruct		*cb = (XmAnyCallbackStruct *) call_data;
    char			format_name[256];
    unsigned long		length;
    unsigned long		pend_count;
    int				count, status = 0,
    				i;
    XmClipboardPendingList	item_list;
    Arg				args[MAX_ARGS];

    switch((int) client_data)
    {
      case 1:
	fprintf(stdout, "CUTTING...\n"); 
	CopyFileToClipboard(Text1, cb->event->xbutton.time); 
	break;

      case 2:
	fprintf(stdout, "PASTING...\n"); 
	PasteItemFromClipboard(Text1); 
	break;

      case 3:
	fprintf(stdout, "DELETING PRIMARY SELECTION...\n"); 
	DeletePrimarySelection(Text1, cb->event->xbutton.time); 
	break;

      case 4:
	fprintf(stdout, "DELETING FROM CLIPBOARD...\n"); 
	XmClipboardUndoCopy(XtDisplay(Text1), XtWindow(Text1));
	break;

      case 5:
	fprintf(stdout, "STATUS...\n");
	length = 256;
    
	status = XmClipboardInquireCount(XtDisplay(Text1), XtWindow(Text1), 
					 &count, &length); 
	if (status == ClipboardSuccess)
	    fprintf(stdout, "XmClipboardInquireCount: %d\n", count);
	else
	    fprintf(stdout, "XmClipboardInquireCount status: %s\n", 
		    StatusToString(status));
	
	status = XmClipboardInquireFormat(XtDisplay(Text1), XtWindow(Text1), 
					  count, format_name, 
					  sizeof(format_name)-1, &length);
	
	if (status == ClipboardSuccess)
	{
	    format_name[length] = '\0';
	    fprintf(stdout, "XmClipboardInquireFormat: '%s' %d\n", format_name,
		    length);
	}
	else
	    fprintf(stdout, "XmClipboardInquireFormat status: %s\n", 
		    StatusToString(status));
	
	status = XmClipboardInquireLength(XtDisplay(Text1), XtWindow(Text1),
					  format_name, &length);

	if (status == ClipboardSuccess)
	    fprintf(stdout, "XmClipboardInquireLength: %d\n", (int)length);
	else
	    fprintf(stdout, "XmClipboardInquireLength status: %s\n", 
		    StatusToString(status));

	
	item_list = NULL;

	status = XmClipboardInquirePendingItems(XtDisplay(Text1), 
						XtWindow(Text1),
						format_name, &item_list, 
						&pend_count);
	if (status == ClipboardSuccess)
	{
	    fprintf(stdout, "XmClipboardInquirePending:\n");
	
	    for(i=0; i<(int) pend_count; i++)
	    {
		fprintf(stdout, "item %d: DataId=%d PrivateId=%d\n", 
			item_list[i].DataId, item_list[i].PrivateId);
	    }
	}
	else
	    fprintf(stdout, "XmClipboardInquirePending status: %s\n", 
		    StatusToString(status));

	if(item_list)
	{
	    free(item_list);
	}
	break;

      default:
	fprintf(stdout, "CB Error\n");
	break;
    }
    fprintf(stdout, "*\n");
}

void  main(argc, argv)
     int	argc;
     char	*argv[];
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;

    CommonTestInit(argc, argv);
    
/*
 * Form
 */

    Form1 = XmCreateForm(Shell1, "Form1", NULL, 0);
    XtManageChild(Form1);

/*
 * Text
 */

    n = 0;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrows, 8);  n++;
    XtSetArg(args[n], XmNcolumns, 40);  n++;
    
    Text1 = XmCreateText(Form1, "Text1", args, n);
    XtManageChild(Text1);
    
/*
 * RowColumn
 */
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg(args[n], XmNtopWidget, Text1);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);  n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL );  n++;
    XtSetArg(args[n], XmNadjustLast, False );  n++;
    XtSetArg(args[n], XmNisAligned, False );  n++;

    RowCol1 = XmCreateRowColumn(Form1, "RowCol1", args, n);
    XtManageChild(RowCol1);
    
/*
 * CUT PushButton
 */

    tcs = XmStringLtoRCreate( "CUT", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;

    PushButton1 = XmCreatePushButton(RowCol1, "PushButton1", args, n);
    XtManageChild(PushButton1);

/*
 * PASTE PushButton
 */

    tcs = XmStringLtoRCreate( "PASTE", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;

    PushButton2 = XmCreatePushButton(RowCol1, "PushButton2", args, n);
    XtManageChild(PushButton2);

/*
 * DELETE PRIMARY SELECTION PushButton
 */

    tcs = XmStringLtoRCreate( "DELETE\nPRIMARY\nSELECTION", 
			     XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;

    PushButton3 = XmCreatePushButton(RowCol1, "PushButton3", args, n);
    XtManageChild(PushButton3);

/*
 * DELETE FROM CLIPBOARD PushButton
 */

    tcs = XmStringLtoRCreate( "DELETE\nFROM\nCLIPBOARD", 
	XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;

    PushButton4 = XmCreatePushButton(RowCol1, "PushButton4", args, n);
    XtManageChild(PushButton4);

/*
 * STATUS PushButton
 */

    tcs = XmStringLtoRCreate( "STATUS", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;

    PushButton5 = XmCreatePushButton(RowCol1, "PushButton5", args, n);
    XtManageChild(PushButton5);

/*
 * Realize top level shell widget
 */

    XtRealizeWidget(Shell1);

/*
 * Add callbacks for buttons
 */

    XtAddCallback(PushButton1, XmNarmCallback, ButtonCB, (XtPointer)1);
    XtAddCallback(PushButton2, XmNarmCallback, ButtonCB, (XtPointer)2);
    XtAddCallback(PushButton3, XmNarmCallback, ButtonCB, (XtPointer)3);
    XtAddCallback(PushButton4, XmNarmCallback, ButtonCB, (XtPointer)4);
    XtAddCallback(PushButton5, XmNarmCallback, ButtonCB, (XtPointer)5);

    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop(app_context);
}




/*
 * CopyFileToClipboard - Copy the present file to the clipboard.
 */

void CopyFileToClipboard(textw, time)
     Widget	textw;
     Time	time;
{
   char			*selected_string = XmTextGetSelection (textw);
					/* text selection    */
   long		        item_id = 0;	/* clipboard item id */
#ifndef MOTIF1_1
   long			data_id = 0;	/* clipboard data id */
#else
   int              	data_id = 0;    /* clipboard data id */
#endif /* MOTIF1_1 */
   XmString		tcs;
   int                  status = 0;

/*
 * using the clipboard facilities, copy the selected text to the clipboard
 */

   if (selected_string != NULL) 
   {
	
/*
 * start copy to clipboard
 */

        tcs = XmStringLtoRCreate( "XM_EDITOR", XmSTRING_DEFAULT_CHARSET);

	status = XmClipboardStartCopy(XtDisplay(textw), XtWindow(textw), tcs, 
				      time, textw, NULL, &item_id);

	if (status != ClipboardSuccess)
	{
	    fprintf(stdout, "Start copy failed, status: %s\n",
		    StatusToString(status));
	    return;
	}

/*
 * move the data to the clipboard
 */

	status = XmClipboardCopy(XtDisplay(textw), XtWindow(textw), item_id,
				 "STRING", selected_string,
				 (long)strlen(selected_string)+1, 0, &data_id);

	if (status != ClipboardSuccess)
	{
	    fprintf(stdout, "Copy failed, status: %s\n",
		    StatusToString(status));
	    return;
	}

/*
 * end the copy to the clipboard
 */

	status = XmClipboardEndCopy(XtDisplay(textw),  XtWindow(textw), 
				    item_id);

	if (status != ClipboardSuccess)
	{
	    fprintf(stdout, "End copy failed, status: %s\n",
		    StatusToString(status));
	    return;
	}
    }
}


/*
 * DeletePrimarySelection - Deletes the primary selection.
 */

void DeletePrimarySelection(textw, etime)
     Widget textw;
	 Time	etime;
{

#ifdef	DONT
    XClientMessageEvent cm;

/*
 * send a client message to the text widget to delete the current selection
 */

    cm.type = ClientMessage;
    cm.display = XtDisplay(textw);
    cm.message_type = XmInternAtom(XtDisplay(textw), "KILL_SELECTION", FALSE);
    cm.window = XtWindow(textw);
    cm.format = 32;
    cm.data.l[0] = XA_PRIMARY;
    XSendEvent(XtDisplay(textw), cm.window, TRUE, NoEventMask, &cm);
#endif /* DONT */

	XConvertSelection(XtDisplay(textw), XA_PRIMARY,
		XInternAtom(XtDisplay(textw), "DELETE", False),
		None, XtWindow(textw), etime);
}


/*
 * PasteItemFromClipboard - paste item from the clipboard
 *                          to the current cursor location
 */

void PasteItemFromClipboard(textw)
     Widget textw;
{
    char 		*selected_string = XmTextGetSelection (textw);
					/* containts of selection	*/
    char		*buffer;	/* temporary text buffer	*/
    unsigned long	length;		/* length of buffer		*/
    unsigned long	outlength = 0;	/* length of bytes copied	*/
#ifndef MOTIF1_1
    long		private_id = 0;	/* id of item on clipboard	*/
#else
    int 		private_id = 0; /* id of item on clipboard      */
#endif /* MOTIF1_1 */
    XmTextPosition	cursorPos;	/* text cursor position		*/
    register int	ac;		/* arg count			*/
    Arg			al[10];		/* arg list			*/
    int                 status = 0;

/*
 * retrieve the current data from the clipboard
 * and paste it at the current cursor position
 */

/* 
 * find the length of the paste item
 */

    status = XmClipboardInquireLength(XtDisplay(textw), XtWindow(textw), 
				      "STRING", &length);

    if (status != ClipboardSuccess)
    {
	fprintf(stdout, "Paste inquire length failed, status: %s\n",
		StatusToString(status));
	return;
    }
			
/*
 * malloc necessary space
 */

    buffer = XtMalloc((int) length);

/*
 * Retrieve paste item
 */

    status = XmClipboardRetrieve(XtDisplay(textw), XtWindow(textw), "STRING", 
				 buffer, length, &outlength, &private_id);

    if (status != ClipboardSuccess)
    {
	fprintf(stdout, "Paste retrieve failed, status: %s\n",
		StatusToString(status));
	return;
    }

/*
 * get cursor position for pasting
 */

    XtSetArg(al[0], XmNcursorPosition, &cursorPos);
    XtGetValues(textw, al, 1);
			
/*
 * add new text
 */

    XmTextReplace(textw, cursorPos, cursorPos, buffer);
}

static char *StatusToString(status)
    int status;
{
  char *returnvalue;

  switch(status) {
        case ClipboardFail:
           returnvalue = "Clipboard Fail";
           break;
        case ClipboardTruncate: 
           returnvalue = "Clipboard Truncate";
           break;
        case ClipboardLocked:
           returnvalue = "Clipboard Locked";
           break;
        case ClipboardBadFormat:
           returnvalue = "Clipboard Bad Format";
           break;
       case ClipboardNoData:
           returnvalue = "Clipboard No Data";
           break;
        default:
           returnvalue = "Unknown status";
        }

  return(returnvalue);

}

