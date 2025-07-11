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
static char rcsid[] = "$XConsortium: Navigation1.c /main/9 1995/07/13 19:40:16 drk $"
#endif
#endif

#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Vendor.h>

#include <Xm/XmP.h>
#include <Xm/RowColumnP.h>

#include <testlib.h>

/*************Some Globals***********************/

Widget mainWindow;
Widget manager1, rc2;
Widget button[6];
Widget pb1, pb2;

/*  Private Functions  */
static XtCallbackProc  SimpleMessage();

void CreateButtons (Widget w);

Quit()
{
    fprintf(stdout,"Begin exiting from SIGINT ... please standby ... \n");
    fflush(stdout);
    exit(0);
}


void PrintName(w)
     Widget w;
{
    fprintf(stdout, "Widget %s was activated\n",
	    XrmQuarkToString(w->core.xrm_name));
}

void PrintName_2(rc, client_data, data)
     Widget		rc;
     XtPointer		client_data;
     XtPointer		data;

{
     XmRowColumnCallbackStruct	*rc_data=(XmRowColumnCallbackStruct*)data;
    fprintf(stdout, "Widget %s was activated\n", 
	    XrmQuarkToString(rc_data->widget->core.xrm_name));
}

static XtCallbackRec PrintName_CB[] =
{
    {PrintName_2, NULL},
    {NULL, NULL}
};



void SetInsensitive(w,d1,d2)
     Widget w;
     XtPointer d1 ;
     XtPointer d2 ;
{
    Arg args[1];
    int	n;

    XtSetSensitive(w, False);
}

void ChangeLabel (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
  char buf[16];
  static int counter = 1;
  Arg args[1];
  
  sprintf (buf, "victim%d", counter);
  counter++;

  XtSetArg (args[0], XmNlabelString,
	    XmStringCreateSimple (buf));
  XtSetValues (w, args, 1);

}

Widget CreateManagedKid (Widget parent, 
		       char *name, 
		       XmNavigationType navMode,
		       int numKids)

{
  Arg args[4];
  int n, i;
  static int counter = 0;
  char buf[32];
  Widget kid;
  Widget *subkids;

  /* Create the kid */

  n = 0;
  XtSetArg (args[n], XmNnumColumns, 2); n++;
  XtSetArg (args[n], XmNnavigationType, (XtArgVal) navMode); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  kid = XmCreateRowColumn (parent, name, args, n);

  /* alloc space to create subkids */  

  subkids = (Widget *) XtMalloc (sizeof (Widget) * numKids);

  for (i = 0; i <= numKids - 1; i++) {
    n = 0;
    sprintf (buf, "Imagine your name here %d", ++counter%10);
    subkids[i] = XmCreatePushButton (kid, buf, args, n);
  }

  XtManageChildren (subkids, numKids);
  XtManageChild (kid);
  XtFree ((char *) subkids);

  return (kid);

}


void main (argc, argv)
     unsigned int	argc;
     char		*argv[];

{
    Widget	child[7];
    XEvent	event;
    Arg		args[15];
    int		n, i;
    Widget      rc1, rc2;


    CommonTestInit(argc, argv);
    
/*
 * case 1: 1.1 Paned Window parent, child of shell
 */

    n = 0;
    manager1 = XmCreatePanedWindow(Shell1, "manager1", (ArgList) args, n);
    XtManageChild(manager1);

    CreateButtons(manager1);
    
    XtRealizeWidget (Shell1);

    CommonPause();


/*
 *  case 2:         shell
 *             mainWindow (RC)
 *    manager1 (1.0 PW) -- rc2 (1.0 RC)
 */

    XtDestroyWidget(manager1);

    n = 0;
    XtSetArg (args[n], XmNwidth, 300); n++;
    mainWindow = XmCreateRowColumn(Shell1, "mainWindow", args, n);

    n = 0;
    manager1 = XmCreatePanedWindow(mainWindow, "manager1", (ArgList) args, n);
   
    XmAddTabGroup (manager1);

    CreateButtons(manager1);

    n = 0;
    rc2 = XmCreateRowColumn(mainWindow, "rc2", (ArgList) args, n);
    
    n = 0;
    pb1 = XmCreatePushButton(rc2, "pb1", args, n);
    pb2 = XmCreatePushButton(rc2, "pb2", args, n);

    XmAddTabGroup(rc2);

    XtManageChild(pb1);
    XtManageChild(pb2);
    XtManageChild(rc2);
    XtManageChild(manager1);
    XtManageChild(mainWindow);

    XtRealizeWidget (Shell1);

    CommonPause();

/*
 *  case 2a:         shell
 *               manager1 (1.1 PW) [tabbable sash ]
 *        rc kid EXCLUSIVE    rc kid STICKY
 */
    XmRemoveTabGroup (manager1);
    XtDestroyWidget(manager1);
    XmRemoveTabGroup (rc2);
    XtDestroyWidget(rc2);
    XmRemoveTabGroup (mainWindow);
    XtDestroyWidget (mainWindow);

    n = 0;
    manager1 = XmCreatePanedWindow(Shell1, "manager1", (ArgList) args, n);

/*  begin test for PIR 4826 */
/*  if the following is a STICKY tab group the Paned Window in the
    next panel has the right traversal behavior. When it's exclusive,
    even though we XmRemove all tab groups, the behavior still shows
    the presence of an EXCLUSIVE tab group */
    rc1 = CreateManagedKid (manager1, "rc1", XmEXCLUSIVE_TAB_GROUP, 4);
/*  end test  for PIR  4826 */
    rc2 = CreateManagedKid (manager1, "rc2", XmSTICKY_TAB_GROUP, 6);

    XtManageChild(manager1);

    CommonPause();

/*
 *  case 3:      shell
 *             mainWindow (RC)
 *      manager1 (1.1 PW)    rc2 (1.1 RC)
 */
    
    XmRemoveTabGroup (rc1);
    XtDestroyWidget (rc1);
    XmRemoveTabGroup (rc2);
    XtDestroyWidget (rc2);
    XtDestroyWidget (manager1);

    n = 0;
    XtSetArg (args[n], XmNwidth, 300); n++;
    mainWindow = XmCreateRowColumn(Shell1, "mainWindow", args, n);

    n = 0;
    manager1 = XmCreatePanedWindow(mainWindow, "manager1", (ArgList) args, n);
    XtManageChild(manager1);
   
    CreateButtons(manager1);

    n = 0;
    rc2 = XmCreateRowColumn(mainWindow, "rc2", (ArgList) args, n);
    
    n = 0;
    pb1 = XmCreatePushButton(rc2, "pb1", args, n);
    pb2 = XmCreatePushButton(rc2, "pb2", args, n);
    XtManageChild(pb1);
    XtManageChild(pb2);
    XtManageChild(rc2);
    XtManageChild(mainWindow);

    XtRealizeWidget (Shell1);

    CommonPause();

/*
 *  case 4:      shell
 *           mainWindow (Form)
 *     manager1 (1.0 PW) rc2 (RC)
 */

    XtDestroyWidget(mainWindow);

    n = 0;
    mainWindow = XmCreateForm(Shell1, "mainWindow", args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    manager1 = XmCreatePanedWindow(mainWindow, "manager1", (ArgList) args, n);
    XtManageChild(manager1);
   
    XmAddTabGroup (manager1);

    CreateButtons(manager1);

    n = 0;
    XtSetArg(args[n], XmNtopWidget, manager1); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    rc2 = XmCreateRowColumn(mainWindow, "rc2", (ArgList) args, n);
    
    n = 0;
    pb1 = XmCreatePushButton(rc2, "pb1", args, n);
    pb2 = XmCreatePushButton(rc2, "pb2", args, n);
    XtManageChild(pb1);
    XtManageChild(pb2);
    XtManageChild(rc2);
    XmAddTabGroup(rc2);
    XtManageChild(mainWindow);

    XtRealizeWidget (Shell1);

    CommonPause();

/*  
 *  case 5:  Note, we are keeping mainWindow this time.
 *           shell
 *         mainWindow (Form)
 *           manager1 (RC)
 */

    XmRemoveTabGroup(manager1);
    XtDestroyWidget(manager1);
    XmRemoveTabGroup(rc2);
    XtDestroyWidget(rc2);

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++;
    manager1 = XmCreateRowColumn(mainWindow, "manager1", (ArgList) args, n);
    XtManageChild(manager1);

    CreateButtons(manager1);
    
    CommonPause();

/*
 *  case 6: Destroy all children of row column and recreate them.
 *          Check for initial focus. PIR 3601.
 */

    for (i = 0; i <= 5; i++)
      XtDestroyWidget (button[i]);

    CreateButtons (manager1);
    
    CommonPause();

/* 
 *  case 7: same thing, except make callback for buttons
 *          "traversal-indifferent"
 */

    for (i = 0; i <= 5; i++)
      XtDestroyWidget (button[i]);

    CreateButtons (manager1);

    for (i = 0; i <= 5; i++) {
      XtRemoveCallback (button[i], XmNactivateCallback, SetInsensitive, NULL);
      XtAddCallback (button[i], XmNactivateCallback, ChangeLabel, NULL);
    }

    CommonPause();
    CommonPause(); /* exit */
    XtAppMainLoop(app_context);
}


void CreateButtons (Widget parent)
{
    int i,n;
    Arg args[1];

    n = 0;
    i = 0;
    XtSetArg (args[n], XmNwidth, 150); n++;
    button[i] = XmCreatePushButtonGadget(parent, "button1", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    button[i] = XmCreatePushButtonGadget(parent, "button2", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    button[i] = XmCreatePushButtonGadget(parent, "button3", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    button[i] = XmCreatePushButtonGadget(parent, "button4", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    button[i] = XmCreatePushButtonGadget(parent, "button5", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    button[i] = XmCreatePushButtonGadget(parent, "button6", args, n);
    XtAddCallback(button[i], XmNactivateCallback, SetInsensitive, NULL);
    i++;

    XtManageChildren (button, i);
}

static XtCallbackProc  SimpleMessage (w, client_data, call_data)
    Widget   w;
    caddr_t  client_data;
    caddr_t  call_data;

{
    printf ("Activate callback from %s\n", (char *) client_data);
    fflush (stdout);
}
