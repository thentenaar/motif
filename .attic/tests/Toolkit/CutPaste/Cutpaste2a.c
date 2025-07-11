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
static char rcsid[] = "$XConsortium: Cutpaste2a.c /main/7 1995/07/13 17:54:58 drk $"
#endif
#endif


#include <testlib.h>
#include "X11/Xatom.h"

Widget    Form1;
Widget    Text1;
Widget    RowCol1;
Widget    PushButton1;
Widget    PushButton2;
Widget    ToggleButton1;

void CopyFileToClipboard();
void PasteItemFromClipboard();

void ButtonCB(w, client_data, call_data)
     Widget	w;
     XtPointer	client_data,
     		call_data;
{
    XmAnyCallbackStruct		*cb = (XmAnyCallbackStruct *) call_data;
    Arg				args[MAX_ARGS];
    Boolean			state = False;

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
	XtSetArg(args[0], XmNset, &state);
	XtGetValues(ToggleButton1, args, 1);
	
	if(!state)
	{
	    fprintf(stdout, "UNLOCKING CLIPBOARD...\n");
	    if(XmClipboardUnlock(XtDisplay(Text1), XtWindow(Text1), TRUE) != 
	       ClipboardSuccess)
	    {
		fprintf(stdout, "unlock failed\n");	
		XtSetArg(args[0], XmNset, True);  
		XtSetValues(ToggleButton1, args, 1); 
	    }
	}
	else
	{
	    fprintf(stdout, "LOCKING CLIPBOARD...\n");
		
	    if(XmClipboardLock(XtDisplay(Text1), XtWindow(Text1)) !=
	       ClipboardSuccess)
	    {
		fprintf(stdout, "lock failed\n");	
		XtSetArg(args[0], XmNset, False);  
		XtSetValues(ToggleButton1, args, 1); 
	    }
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
    register int	n;
    Arg			args[MAX_ARGS];
    XmString		tcs;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 160);  n++;
    XtSetValues(Shell1, args, n);
    

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
 * LOCK Toggle Button
 */

    tcs = XmStringLtoRCreate( "LOCK", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs );  n++;
    XtSetArg(args[n], XmNshadowThickness, 2 );  n++;

    ToggleButton1 = XmCreateToggleButton(RowCol1, "ToggleButton1", args, n);
    XtManageChild(ToggleButton1);

/*
 * Realize top level shell widget
 */

    XtRealizeWidget(Shell1);
    
/*
 * Add callbacks for buttons
 */

    XtAddCallback(PushButton1, XmNarmCallback, ButtonCB, (XtPointer)1);
    XtAddCallback(PushButton2, XmNarmCallback, ButtonCB, (XtPointer)2);

    XtAddCallback(ToggleButton1, XmNvalueChangedCallback, ButtonCB,
		  (XtPointer) 3);

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
   long			item_id = 0;	/* clipboard item id */
   long			data_id = 0;	/* clipboard data id */
   XmString		tcs;

/*
 * using the clipboard facilities, copy the selected text to the clipboard
 */

   if (selected_string != NULL) 
   {
	
/*
 * start copy to clipboard
 */

        tcs = XmStringLtoRCreate( "XM_EDITOR", XmSTRING_DEFAULT_CHARSET);

	if(XmClipboardStartCopy(XtDisplay(textw), XtWindow(textw), tcs, time,
				textw, NULL, &item_id) != ClipboardSuccess)
	{
	    fprintf(stdout, "start copy failed\n");
	    return;
	}

/*
 * move the data to the clipboard
 */

	if(XmClipboardCopy(XtDisplay(textw), XtWindow(textw), item_id,
			   "STRING", selected_string,
			   (long)strlen(selected_string)+1, 0, &data_id)
	   != ClipboardSuccess)
	{
	    fprintf(stdout, "copy failed\n");
	    return;
	}

/*
 * end the copy to the clipboard
 */

	if(XmClipboardEndCopy(XtDisplay(textw),  XtWindow(textw), item_id)
	   != ClipboardSuccess)
	{
	    fprintf(stdout, "end copy failed\n");
	    return;
	}
    }
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
   char			*buffer;	/* temporary text buffer	*/
   unsigned long	length;		/* length of buffer		*/
   unsigned long	outlength = 0;	/* length of bytes copied	*/
   long			private_id = 0;	/* id of item on clipboard	*/
   XmTextPosition	cursorPos;	/* text cursor position		*/
   register int		ac;		/* arg count			*/
   Arg			al[10];		/* arg list			*/

/*
 * retrieve the current data from the clipboard
 * and paste it at the current cursor position
 */

/* 
 * find the length of the paste item
 */

    if(XmClipboardInquireLength(XtDisplay(textw), XtWindow(textw), "STRING",
				&length) != ClipboardSuccess)
    {
	fprintf(stdout, "paste inquire length failed\n");
	return;
    }
			
/*
 * malloc necessary space
 */

    buffer = XtMalloc((int) length);

/*
 * Retrieve paste item
 */

    if(XmClipboardRetrieve(XtDisplay(textw), XtWindow(textw), "STRING", buffer,
			   length, &outlength, &private_id)
       != ClipboardSuccess)
    {
	fprintf(stdout, "paste retrieve failed\n");	
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
