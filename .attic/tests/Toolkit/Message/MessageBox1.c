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
static char rcsid[] = "$XConsortium: MessageBox1.c /main/8 1995/07/13 18:58:39 drk $"
#endif
#endif


#include <testlib.h>
#include "warning"
#ifndef PROGNAME
#define PROGNAME  "MsgBox2"
#endif

/*  Global Variables  */
Widget    RowColumn1;
Widget    MessageBox1;
Widget    MessageBox2;
Widget    MessageBox3;
Widget    MessageBox4;
Widget    MessageBox5;
Widget    MessageBox6;

Pixmap warnmap;
void HelpCB();
void FocusCB();

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    Boolean       trace = False;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs1;
    XmString      tcs2;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetValues(Shell1, args, n);

    XtRealizeWidget(Shell1);

    n = 0;
    XtSetArg(args[n], XmNentryBorder, 1);  n++;
    RowColumn1 = XmCreateRowColumn(Shell1, "RowColumn1", args, n);
/*  XtManageChild(RowColumn1); */

    n = 0;
    MessageBox1 = XmCreateMessageBox(RowColumn1, "MessageBox", args, n);
    XtManageChild(MessageBox1);
    XtAddCallback (MessageBox1, XmNfocusCallback, FocusCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR);  n++;
    MessageBox2 = XmCreateMessageBox(RowColumn1, "ErrorDialog", args, n);
    XtManageChild(MessageBox2);
    XtAddCallback (MessageBox2, XmNfocusCallback, FocusCB, NULL);


    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_QUESTION);  n++;
    MessageBox3 = XmCreateMessageBox(RowColumn1, "QuestionDialog", args, n);
    XtManageChild(MessageBox3);
    XtAddCallback (MessageBox3, XmNfocusCallback, FocusCB, NULL);


    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION);  n++;
    MessageBox4 = XmCreateMessageBox(RowColumn1, "InformationDialog", args, n);
    XtManageChild(MessageBox4);
    XtAddCallback (MessageBox4, XmNfocusCallback, FocusCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_WARNING);  n++;
    MessageBox5 = XmCreateMessageBox(RowColumn1, "WarningDialog", args, n);
    XtManageChild(MessageBox5);
    XtAddCallback (MessageBox5, XmNfocusCallback, FocusCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_WORKING);  n++;
    MessageBox6 = XmCreateMessageBox(RowColumn1, "WorkingDialog", args, n);
    XtManageChild(MessageBox6);
    XtAddCallback (MessageBox6, XmNfocusCallback, FocusCB, NULL);


    /* Reset values for the warning messagebox. 1st build the pixmap */
    warnmap = XCreatePixmapFromBitmapData(display,
    		DefaultRootWindow(display),
	   	warning_bits,warning_width,warning_height,
		BlackPixel(display,DefaultScreen(display)),
		WhitePixel(display,DefaultScreen(display)),
		DefaultDepth(display,DefaultScreen(display)));

    n = 0;
    tcs1 = XmStringLtoRCreate("Warning!! File Exists!\nOK to OverWrite?",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1);  n++;
    XtSetArg(args[n], XmNsymbolPixmap, warnmap);  n++;
    XtSetArg(args[n], XmNmessageAlignment, XmALIGNMENT_END);  n++;
    tcs2 = XmStringLtoRCreate("Not OK", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNcancelLabelString, tcs2); n++;
    XtSetValues(MessageBox5, args, n);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    /* Reset messages for each other message boxes */
    n = 0;
    tcs1 = XmStringLtoRCreate("For your information",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1); n++;
    tcs2 = XmStringLtoRCreate("Boring", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNcancelLabelString, tcs2); n++;
    XtSetValues(MessageBox4,args,n);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    n = 0;
    tcs1 = XmStringLtoRCreate("This is a Question?",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1); n++;
    tcs2 = XmStringLtoRCreate("No Answer", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNcancelLabelString, tcs2); n++;
    XtSetValues(MessageBox3,args,n);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    n = 0;
    tcs1 = XmStringLtoRCreate("This is a Message.",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1); n++;
    XtSetValues(MessageBox1,args,n);
    XmStringFree(tcs1);

    n = 0;
    tcs1 = XmStringLtoRCreate("You made an error!",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1); n++;
    tcs2 = XmStringLtoRCreate("Re-do", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNcancelLabelString, tcs2); n++;
    XtSetValues(MessageBox2,args,n);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    n = 0;
    tcs1 = XmStringLtoRCreate("Working Dialog.",
	  		      XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, tcs1); n++;
    tcs2 = XmStringLtoRCreate("Go Home", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNcancelLabelString, tcs2); n++;
    XtSetValues(MessageBox6,args,n);
    XmStringFree(tcs1);
    XmStringFree(tcs2);

    /* add the help callback to each one */
    XtAddCallback(MessageBox1,XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(MessageBox2,XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(MessageBox3,XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(MessageBox4,XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(MessageBox5,XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(MessageBox6,XmNhelpCallback, HelpCB, NULL);

    XtManageChild(RowColumn1);
        
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    
    XtAppMainLoop(app_context);
}

void FocusCB (widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;

{
  printf ("Focus has arrived in %s\n", XtName(widget));
  fflush (stdout);
}

void HelpCB(widget, client_data, call_data)
    Widget      widget;
    XtPointer     client_data;
    XtPointer     call_data;
{

  printf ("Help callback called from %s\n", XtName(widget));
  fflush (stdout);

}












