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
static char rcsid[] = "$XConsortium: PromptDia1.c /main/8 1995/07/13 19:18:56 drk $"
#endif
#endif

#include <testlib.h>

static void  OkCB();
static void  CancelCB();
static void  HelpCB();
static void  ApplyCB();

Widget    PromptDia;
char      *dia_type;
Widget    force_focus;

/* forward declarations */
void ResetFocus();

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString	  cs1, cs2;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 10);		   n++;
    XtSetArg(args[n], XmNheight, 10);		   n++;
    XtSetValues(Shell1, args, n);
    

    XtRealizeWidget(Shell1);

    cs1 = XmStringLtoRCreate("MessageDialog", XmSTRING_DEFAULT_CHARSET);
    cs2 = XmStringLtoRCreate("Select a Dialog:", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNx,0);				n++;
    XtSetArg(args[n], XmNy,0);				n++;
    XtSetArg(args[n], XmNwidth, 200);			n++;
    XtSetArg(args[n], XmNheight, 200);			n++;
    XtSetArg(args[n], XmNautoUnmanage, False);		n++;
    XtSetArg(args[n], XmNtextString, cs1);		n++;
    XtSetArg(args[n], XmNselectionLabelString, cs2);	n++;
    XtSetArg(args[n], XmNshadowThickness, 5);		n++;
    XtSetArg(args[n], XmNhighlightThickness, 5);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True);	n++;
    PromptDia = XmCreatePromptDialog(Shell1, "PromptDia", args, n);
    XtManageChild(PromptDia);

    CommonPause();

    XtAddCallback(PromptDia, XmNokCallback, OkCB, NULL);
    XtAddCallback(PromptDia, XmNhelpCallback, HelpCB, NULL);
    XtAddCallback(PromptDia, XmNcancelCallback, CancelCB, NULL);
    XtAddCallback(PromptDia, XmNapplyCallback, ApplyCB, NULL);

    CommonPause();
    CommonPause();
    CommonPause();

    /* test to reset focus on OK button every time */

    XtDestroyWidget (PromptDia);

    cs2 = XmStringCreateSimple ("Press Apply or Cancel button");

    n = 0;
    XtSetArg(args[n], XmNtextString, cs2);		n++;
    XtSetArg(args[n], XmNshadowThickness, 5);		n++;
    XtSetArg(args[n], XmNhighlightThickness, 5);	n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True);	n++;
    PromptDia = XmCreatePromptDialog(Shell1, "PromptDia", args, n);



    /* we'll try to always force focus here */

    force_focus = XmSelectionBoxGetChild (PromptDia, XmDIALOG_OK_BUTTON);

#ifndef MOTIF1_1

    XtAddCallback (PromptDia, XmNmapCallback, ResetFocus, NULL);

#endif /* MOTIF1_1 */

    /* and we'll do it when dialog's mapped */

/*map here */


    XtManageChild(PromptDia);
    XmStringFree (cs2);

    CommonPause();

#ifndef MOTIF1_1

    XtRemoveCallback(PromptDia, XmNmapCallback, ResetFocus, NULL);

#endif /* MOTIF1_1 */

    if (!XtIsManaged (PromptDia))
	     XtManageChild (PromptDia);
    else 
	     printf ("Test error: Dialog was managed at wrong time\n");

    CommonPause();

    XtAddCallback (PromptDia, XmNmapCallback, ResetFocus, NULL);

    if (!XtIsManaged (PromptDia))
	     XtManageChild (PromptDia);
    else 
	     printf ("Test error: Dialog was managed at wrong time");


    CommonPause();

    XtAppMainLoop(app_context);
}

void ResetFocus (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
  if ( ((XmSelectionBoxCallbackStruct *)call_data)->reason == XmCR_MAP)
     XmProcessTraversal (force_focus, XmTRAVERSE_CURRENT);
}      


static void  OkCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    Widget Apply;


    XmStringGetLtoR(((XmSelectionBoxCallbackStruct *)call_data)->value,
                    XmSTRING_DEFAULT_CHARSET, &dia_type);
    printf("Text string: %s\n", dia_type);

    Apply = XmSelectionBoxGetChild(w, XmDIALOG_APPLY_BUTTON);
    XtManageChild(Apply);


    
}



static void  CancelCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    int n;
    Arg args[5];
    XmString tcs;
    Widget Apply;

    tcs = XmStringLtoRCreate("", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtextString, tcs);	n++;
    XtSetValues(w, args, n);

    Apply = XmSelectionBoxGetChild(w, XmDIALOG_APPLY_BUTTON);
    XtUnmanageChild(Apply);

    XmStringFree (tcs);
}


static void  HelpCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    Widget HelpW;
    int n;
    Arg args[10];
    XmString tcs;

    tcs = XmStringLtoRCreate("Help!!!! - Has now arrived", 
				XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNx, 50); 			n++;
    XtSetArg(args[n], XmNy, 50);			n++;
    XtSetArg(args[n], XmNshadowThickness, 5);		n++;
    XtSetArg(args[n], XmNdefaultPosition, False);	n++;
    XtSetArg(args[n], XmNmessageString, tcs);		n++;
    HelpW = XmCreateMessageDialog(Shell1, "Help", args, n);
    XtManageChild (HelpW);

   XmStringFree (tcs);
}


static void  ApplyCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    Widget ApplyW;
    int n;
    Arg args[10];
    XmString tcs;

    tcs = XmStringLtoRCreate("Pick a Dialog, Any Dialog", 
				XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNx, 100); 			n++;
    XtSetArg(args[n], XmNy, 100);			n++;
    XtSetArg(args[n], XmNshadowThickness, 5);		n++;
    XtSetArg(args[n], XmNdefaultPosition, False);	n++;
    XtSetArg(args[n], XmNmessageString, tcs);		n++;
    if (strcmp(dia_type,"QuestionDialog") == 0)
    	ApplyW = XmCreateQuestionDialog(Shell1, "ApplyQuestion", args, n);
    else  
    if (strcmp(dia_type,"WorkingDialog") == 0)
	ApplyW = XmCreateWorkingDialog(Shell1, "ApplyWorking", args, n);
    else
    if (strcmp(dia_type,"WarningDialog") == 0)
	ApplyW = XmCreateWarningDialog(Shell1, "ApplyWarning", args, n);
    else
    if (strcmp(dia_type,"MessageDialog") == 0)
	ApplyW = XmCreateMessageDialog(Shell1, "ApplyMessage", args, n);
    else
    if (strcmp(dia_type,"ErrorDialog") == 0)
	ApplyW = XmCreateErrorDialog(Shell1, "ApplyError", args, n);
    else
    if (strcmp(dia_type,"InformationDialog") == 0)
	ApplyW = XmCreateInformationDialog(Shell1, "ApplyInfo", args, n);
    else {
	printf("This Form of Dialog is not supported\n");

	XtFree ((char *)dia_type);

	return;
    }
	
    XtManageChild (ApplyW);
    XmStringFree (tcs);
}
