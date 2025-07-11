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
static char rcsid[] = "$XConsortium: Command1.c /main/7 1995/07/13 19:16:34 drk $"
#endif
#endif

#include <testlib.h>

#define NITEMS 14
#define VITEMS 10

char *Istrings[] = {
    	"btest1a",
    	"decor00",
    	"decor06",
    	"func00",
    	"func05",
    	"func10",
    	"iplac00",
    	"iplac05",
    	"menu00",
    	"menu04",
    	"mtest1",
    	"parse00",
    	"prop00",
    	"rstest1"
};

Widget    Command1;

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs, ItemList[NITEMS];
    Widget	  ChildW, Frame;

    CommonTestInit(argc, argv);

    n = 0;
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);

    /* Convert the items to Xm strings */

    for (n = 0; n < NITEMS; n++)
    	ItemList[n] = XmStringLtoRCreate(Istrings[n], XmSTRING_DEFAULT_CHARSET);

    tcs = XmStringLtoRCreate("Pick a Command",XmSTRING_DEFAULT_CHARSET);

    /* add them to the widget's list */
    n = 0;
    XtSetArg(args[n], XmNhistoryItems, ItemList); 			n++;
    XtSetArg(args[n], XmNhistoryItemCount, NITEMS); 			n++;
    XtSetArg(args[n], XmNhistoryVisibleItemCount, VITEMS); 		n++;
    XtSetArg(args[n], XmNlistLabelString, tcs); 			n++;
    XtSetArg(args[n], XmNforeground, CommonGetColor("white"));		n++;
    Command1 = XmCreateCommand(Frame, "Command1", args, n);
    XtManageChild(Command1);

    XmStringFree(tcs);

    /* Set up the initial command */

    tcs = XmStringLtoRCreate("ls ",XmSTRING_DEFAULT_CHARSET);
    XmCommandSetValue(Command1, tcs);
    XmStringFree(tcs);

    tcs = XmStringLtoRCreate("-l",XmSTRING_DEFAULT_CHARSET);
    XmCommandAppendValue(Command1, tcs);
    XmStringFree(tcs);


    /* use XtSetValues to change prompt */

    tcs = XmStringLtoRCreate(">>",XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNpromptString, tcs); 			n++;
    XtSetValues(Command1,args,n);
    XmStringFree(tcs);

    /* See about the children */

    ChildW = XmCommandGetChild(Command1,XmDIALOG_PROMPT_LABEL);
    printf("Prompt Label Child Name - %s\n", XtName(ChildW));

    ChildW = XmCommandGetChild(Command1,XmDIALOG_COMMAND_TEXT); 
    printf("Command Text Child Name - %s\n", XtName(ChildW));

    ChildW = XmCommandGetChild(Command1,XmDIALOG_HISTORY_LIST);
    printf("History List Child Name - %s\n", XtName(ChildW));

    fflush(stdout);

    /* add an error message to the list */

    tcs = XmStringLtoRCreate("erroror",XmSTRING_DEFAULT_CHARSET);
    XmCommandError(Command1,tcs);
    XmStringFree(tcs);

    /* realize widgets */

    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    
    XtAppMainLoop(app_context);
}
