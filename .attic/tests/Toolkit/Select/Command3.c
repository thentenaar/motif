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
static char rcsid[] = "$XConsortium: Command3.c /main/8 1995/07/13 19:17:02 drk $"
#endif
#endif

#include <testlib.h>

static XmString	stringList[15];
static int	enter_count = 0,
		change_count = 0;

static void EnterCB();
static XtCallbackRec enter_cb[] = {
    {  EnterCB, NULL},
    { NULL, NULL},
};

static void EnterCB (w, client_data, call_data)
     Widget	w;
     caddr_t	client_data;
     caddr_t	call_data;
{
    static XmString	str = NULL,
    			str2 = NULL;

    if(str == NULL)
    {
	str = XmStringLtoRCreate("setValue", XmSTRING_DEFAULT_CHARSET);
    }
    
    if(str2 == NULL)
    {
	str2 = XmStringLtoRCreate("errorValue", XmSTRING_DEFAULT_CHARSET);
    }

    switch(enter_count)
    {
      case 1:

	XmCommandSetValue(w, str);
	break;

      case 3:
	
        XmCommandError (w, str2); 
	break;

      case 4:

	change_count = 1;
	break;
    }

    enter_count++;
}


static void ChangeCB();
static XtCallbackRec change_cb[] = {
    {  ChangeCB, NULL},
    { NULL, NULL},
};

static void ChangeCB (w, client_data, call_data)
     Widget	w;
     caddr_t	client_data;
     caddr_t	call_data;
{
    static XmString	str = NULL;

    if(str == NULL) 
        str = XmStringLtoRCreate("addValue", XmSTRING_DEFAULT_CHARSET);

    if(change_count)
    {
	if(change_count == 4)
	{
	    change_count = 0;

	    XmCommandAppendValue (w, str);
	}

	change_count++;
    }
}


void main (argc,argv)
unsigned int argc;
char **argv;
{
    Widget        command, Frame;
    Arg           args[10];
    register int  n;      
    Widget        unManageList[5];
    XmString      stringList[15];
    XmString      commandString;
    XmString      testString;

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);

    n = 0;
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);

/*
 * set up XmStrings for list
 */

    stringList[0] = XmStringLtoRCreate ("one", XmSTRING_DEFAULT_CHARSET);
    stringList[1] = XmStringLtoRCreate ("two", XmSTRING_DEFAULT_CHARSET);
    stringList[2] = XmStringLtoRCreate ("three", XmSTRING_DEFAULT_CHARSET);
    stringList[3] = XmStringLtoRCreate ("four", XmSTRING_DEFAULT_CHARSET);
    stringList[4] = XmStringLtoRCreate ("five", XmSTRING_DEFAULT_CHARSET);
    stringList[5] = XmStringLtoRCreate ("six", XmSTRING_DEFAULT_CHARSET);
    stringList[6] = NULL;

    commandString = XmStringLtoRCreate ("next", XmSTRING_DEFAULT_CHARSET);

/*
 * create Command
 */

    n = 0;
    XtSetArg (args[n], XmNcommand, commandString);  		n++;
    XtSetArg (args[n], XmNhistoryItems, stringList);		n++;
    XtSetArg (args[n], XmNhistoryMaxItems, 4);  		n++;
    XtSetArg (args[n], XmNhistoryItemCount, 6); 		n++;
    XtSetArg (args[n], XmNcommandEnteredCallback, enter_cb);  	n++;
    XtSetArg (args[n], XmNcommandChangedCallback, change_cb);	n++;
    XtSetArg (args[n], XmNwidth, 250); 				n++;

    command = XmCreateCommand(Frame, "command", args, n);
    XtManageChild (command);

/*
 * realize widgets
 */

    XtRealizeWidget (Shell1);

/*
 * CommonPauses for message boxes
 */

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

/*
 * Code to test CR 5739
 */

    testString = XmStringCreateSimple("testString");
    n = 0;
    XtSetArg (args[n], XmNcommand, testString);                 n++;
    XtSetValues(command, args, n);
    CommonPause();
    
/*
 * process events
 */

    XtAppMainLoop(app_context);
}
