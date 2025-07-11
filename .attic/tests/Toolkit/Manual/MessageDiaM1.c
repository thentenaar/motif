/* $TOG: MessageDiaM1.c /main/5 1999/07/01 13:22:03 vipin $ */
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

static 	void  OkCB();
static  void  FocusCB();
static  void  PrintMessage();
Widget  shell;
Widget  mbox; 

void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[10];           /*  arg list            */
    register int    n;                  /*  arg count           */
    XmString	    tcs, tcs2;
    XFontStruct     *newfont;
    XmFontList      newfontlist;
    Widget db, cancelbutton;



    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg (args[n], XmNwidth, 400);                     n++;
    XtSetArg (args[n], XmNheight, 400);                    n++;
    XtSetValues(Shell1, args, n);
    

    tcs = XmStringCreateLtoR("DialogStyle Unmanaged Test", 
			     XmSTRING_DEFAULT_CHARSET);
    tcs2 = XmStringCreateLtoR("UnManage", XmSTRING_DEFAULT_CHARSET);

    newfont = XLoadQueryFont(display, "variable");
    newfontlist = XmFontListCreate(newfont, XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS);	  n++; 
    XtSetArg (args[n], XmNmessageString, tcs);			  n++;
    XtSetArg (args[n], XmNokLabelString, tcs2);			  n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                   n++;
    XtSetArg (args[n], XmNminimizeButtons, False);                n++;
    XtSetArg (args[n], XmNlabelFontList, newfontlist);		  n++;
    XtSetArg (args[n], XmNbuttonFontList, newfontlist);		  n++;
    mbox = XmCreateMessageDialog (Shell1, "mbox", args, n);
    XmStringFree(tcs);

    XtAddCallback(mbox, XmNokCallback, OkCB, NULL);

    XtAddCallback(mbox, XmNfocusCallback, FocusCB, NULL);

    cancelbutton = XmMessageBoxGetChild (mbox, XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (cancelbutton, XmNactivateCallback, PrintMessage, NULL);

    /* begin test for PIR 4147 */
    XtRealizeWidget (Shell1);
    XtManageChild (mbox);
    /* end test for PIR 4147 */

    CommonPause();

    XtRemoveCallback(mbox, XmNfocusCallback, FocusCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL);	  n++; 
    XtSetValues (mbox, args, n);

    XtManageChild(mbox);

    CommonPause();

    tcs = XmStringCreateLtoR(" DialogType Unmanaged Test ", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);  n++; 
    XtSetArg (args[n], XmNmessageString, tcs);			n++;
    XtSetArg (args[n], XmNdialogType, XmDIALOG_QUESTION);	n++;
    XtSetValues (mbox, args, n);

    XtManageChild(mbox);

    XmStringFree(tcs);

    CommonPause();

    /* begin test for PIR 3306 */
    /* 1) make sure we *have* a default button */

    db = XmMessageBoxGetChild (mbox, XmDIALOG_DEFAULT_BUTTON);

    if (db != NULL) 
      printf ("The default button is %s\n", XtName (db));
    else {
      printf ("There is no default button! I am setting one.\n");
      db = XmMessageBoxGetChild (mbox, XmDIALOG_OK_BUTTON);
      n = 0;
      XtSetArg (args[n], XmNdefaultButton, db);
      n++;
      XtSetValues (mbox, args, n);
      printf ("The default button is %s\n", XtName (db));
    }

    /* 2) set the "unmanage" button to another button and switch callbacks. */

    n = 0;
    XtSetArg (args[n], XmNlabelString, tcs2); n++;
    XtSetValues (XmMessageBoxGetChild (mbox, XmDIALOG_CANCEL_BUTTON), args, n);

    n = 0;
    XtSetArg (args[n], XmNlabelString, 
	      XmStringCreateSimple ("does nothing")); n++;
    XtSetValues (XmMessageBoxGetChild (mbox, XmDIALOG_OK_BUTTON), args, n);

    XtRemoveCallback (mbox, XmNokCallback, OkCB, NULL);
    XtAddCallback (mbox, XmNcancelCallback, OkCB, NULL);

    XmStringFree(tcs2);

    n = 0;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS);	  n++; 
    XtSetValues (mbox, args, n);
    XtManageChild (mbox);

    CommonPause();    /* pop up, user unmanages */

    XtManageChild (mbox);
    /* check the default button again */

    db = XmMessageBoxGetChild (mbox, XmDIALOG_DEFAULT_BUTTON);

    if (db != NULL) 
      printf ("The default button is %s\n", XtName (db));
    else {
      printf ("There is no default button! I am setting one.\n");
      db = XmMessageBoxGetChild (mbox, XmDIALOG_OK_BUTTON);
      n = 0;
      XtSetArg (args[n], XmNdefaultButton, db);
      n++;
      XtSetValues (mbox, args, n);
      printf ("The default button is %s\n", XtName (db));
    }

    CommonPause();    /* where is the focus? */

    /*  process events  */
    CommonPause (); /* exit */
    XtAppMainLoop(app_context);
}

static void  OkCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{

    XtUnmanageChild (w);
}


static void FocusCB (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

	XtCallCallbacks (XmMessageBoxGetChild (w, XmDIALOG_CANCEL_BUTTON),
			 XmNactivateCallback, NULL);

}

static void PrintMessage (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
	printf ("This message is expected on Cancel or FocusIn\n");
	fflush (stdout);
}
