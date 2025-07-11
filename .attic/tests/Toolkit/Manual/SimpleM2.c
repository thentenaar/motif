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
static char rcsid[] = "$XConsortium: SimpleM2.c /main/8 1995/07/13 18:39:29 drk $"
#endif
#endif


#include <testlib.h>

Widget MenuBar, FileMenu, PullDownMB, PullDownOM, OptionMenu;
Widget RadioBox, Label, Form, PopupMenu;

Arg args[10];
Cardinal n;
static void CreateControlPanel();
static void StringTableCB();
static void Error1CB();
static void CoreDump2CB();
static void CoreDump3CB();
static void CoreDump4CB();
static void destroyWidgetsCB();
void buttonCB();
void PostMenu();

static XtCallbackRec simple_cb[] = { buttonCB, NULL, NULL, NULL };

main(argc, argv)
int argc;
char **argv;
{

  CommonTestInit(argc, argv);

  /*
   *  Initialize Xrm and save the current database to a file so we
   *    can reset our changes.
   */
  
  XrmInitialize();
#ifdef MOTIF1_1
  XrmPutFileDatabase(display->db, "SimpleM2.savdb");
#else
  XrmPutFileDatabase(XtDatabase(display), "SimpleM2.savdb");
#endif

  CreateControlPanel();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();

  XtRealizeWidget(Shell1);
  XtAppMainLoop(app_context);
}
static void 
CreateControlPanel()
{
  Widget controlpanel, mainParent, coredump4;
  Widget error1, coredump2, coredump3, noconvert, destroy;
  XmString tcs;
  register int n;

  n = 0;
  XtSetArg(args[n], XmNx, 414); n++;
  XtSetArg(args[n], XmNy, 261); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  controlpanel = XmCreateBulletinBoardDialog(Shell1, "Control Panel", args, n);

  n = 0;
  mainParent = XmCreateRowColumn(controlpanel, "mainParent", args, n);
 
  n = 0;
  tcs = XmStringCreateLtoR("Show Missing String to StringTable Converter Bug",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  noconvert = XmCreatePushButton(mainParent, "noconvert", args, n);
  XtAddCallback(noconvert, XmNactivateCallback, StringTableCB, NULL);
  XmStringFree(tcs);
  XtManageChild(noconvert);

  n = 0;
  tcs = XmStringCreateLtoR("Show ButtonType converter errors (white space)",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  error1 = XmCreatePushButton(mainParent, "Error1", args, n);
  XtAddCallback(error1, XmNactivateCallback, Error1CB, NULL);
  XmStringFree(tcs);
  XtManageChild(error1);

  n = 0;
  tcs = XmStringCreateLtoR("Show keysym (mnemonic) Converter seg violation",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  coredump2 = XmCreatePushButton(mainParent, "coredump2", args, n);
  XtAddCallback(coredump2, XmNactivateCallback, CoreDump2CB, NULL);
  XmStringFree(tcs);
  XtManageChild(coredump2);

  n = 0;
  tcs = XmStringCreateLtoR("Show CharSetTable (mnemonic char set) seg violation",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  coredump3 = XmCreatePushButton(mainParent, "coredump3", args, n);
  XtAddCallback(coredump3, XmNactivateCallback, CoreDump3CB, NULL);
  XmStringFree(tcs);
  XtManageChild(coredump3);

  n = 0;
  tcs = XmStringCreateLtoR("Show button count syntax core dump",
			   XmSTRING_DEFAULT_CHARSET);
  
/* This case is only testing the syntax for buttonCount, */
/* if buttonCount > number of buttons, it's OK to crash! */
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  coredump4 = XmCreatePushButton(mainParent, "coredump4", args, n);
  XtAddCallback(coredump4, XmNactivateCallback, CoreDump4CB, NULL);
  XmStringFree(tcs);
  XtManageChild(coredump4);

  n = 0;
  tcs = XmStringCreateLtoR("Destroy Current Widget Hierarchy",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  destroy = XmCreatePushButton(mainParent, "destroy", args, n);
  XtAddCallback(destroy, XmNactivateCallback, destroyWidgetsCB, NULL);
  XmStringFree(tcs);
  XtManageChild(destroy);
  
  XtManageChild(mainParent);
  XtManageChild(controlpanel);

}

/* ARGSUSED */

static void
destroyWidgetsCB(Widget w, XtPointer client, XtPointer call)
{
  if (Form != NULL)
    {
      XtDestroyWidget(Form);
      Form = NULL;
    }
}

/* ARGSUSED */

static void 
StringTableCB(Widget w, XtPointer client, XtPointer call)
{
  /* 
   * This callback demonstrates the missing string-to-string table converter
   */

  XrmDatabase new_db, sav_db;

  new_db = XrmGetFileDatabase("SimpleM2.db1");

#ifdef MOTIF1_1
  XrmMergeDatabases(new_db, &(display->db));
#else
  sav_db = XtDatabase(display);
  XrmCombineDatabase(new_db, &sav_db, True);
#endif

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 100);	n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  MenuBar = XmCreateSimpleMenuBar(Form, "MenuBar", args, n);
  XtManageChild(MenuBar);

  n = 0;
  XtSetArg(args[n], XmNactivateCallback, simple_cb); n++;
  PullDownMB = XmCreateSimplePulldownMenu(MenuBar, "PullDownMB", args, n);

  /*
   *  Reset the database; end of test
   */

#ifdef MOTIF1_1
  XrmDestroyDatabase(display->db);
  display->db = XrmGetFileDatabase("SimpleM2.savdb");
#else
  XrmDestroyDatabase(sav_db);
  new_db = XrmGetFileDatabase("SimpleM2.savdb");
  XrmSetDatabase(display,new_db);
#endif
}

/* ARGSUSED */

static void 
Error1CB(Widget w, XtPointer client, XtPointer call)
{

  XrmDatabase new_db, sav_db;

  /*  Read in specific database */

  new_db = XrmGetFileDatabase("SimpleM2.db2");

#ifdef MOTIF1_1
  XrmMergeDatabases(new_db, &(display->db));
#else
  sav_db = XtDatabase(display);
  XrmCombineDatabase(new_db, &sav_db, True);
#endif

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 100);	n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  MenuBar = XmCreateSimpleMenuBar(Form, "MenuBar", args, n);
  XtManageChild(MenuBar);

  n = 0;
  XtSetArg(args[n], XmNactivateCallback, simple_cb); n++;
  PullDownMB = XmCreateSimplePulldownMenu(MenuBar, "PullDownMB", args, n);

  /*
   *  Reset the database; end of test
   */

#ifdef MOTIF1_1
  XrmDestroyDatabase(display->db);
  display->db = XrmGetFileDatabase("SimpleM2.savdb");
#else
  XrmDestroyDatabase(sav_db);
  new_db = XrmGetFileDatabase("SimpleM2.savdb");
  XrmSetDatabase(display,new_db);
#endif
}

/* ARGSUSED */

static void
CoreDump2CB(Widget w, XtPointer client, XtPointer call)
{
  XrmDatabase new_db, sav_db;
  XmString tcs;

  /*  Read in specific database */

  new_db = XrmGetFileDatabase("SimpleM2.db3");

#ifdef MOTIF1_1
  XrmMergeDatabases(new_db, &(display->db));
#else
  sav_db = XtDatabase(display);
  XrmCombineDatabase(new_db, &sav_db, True);
#endif

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  PopupMenu = XmCreateSimplePopupMenu(Shell1, "PopupMenu", args, n);
  XtAddEventHandler(Shell1, ButtonPressMask, False, PostMenu, PopupMenu);

  n = 0;
  tcs = XmStringCreateSimple("Push Button 3 for Popup");
  XtSetArg(args[n], XmNlabelString, tcs);			n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
  Label = XmCreateLabel(Form, "Label", args, n);
  XtManageChild(Label);
  XmStringFree(tcs);

  /*
   *  Reset the database; end of test
   */

#ifdef MOTIF1_1
  XrmDestroyDatabase(display->db);
  display->db = XrmGetFileDatabase("SimpleM2.savdb");
#else
  XrmDestroyDatabase(sav_db);
  new_db = XrmGetFileDatabase("SimpleM2.savdb");
  XrmSetDatabase(display,new_db);
#endif
}

/* ARGSUSED */

static void 
CoreDump3CB(Widget w, XtPointer client, XtPointer call)
{
  XrmDatabase new_db, sav_db;

  /*  Read in specific database */

  new_db = XrmGetFileDatabase("SimpleM2.db4");

#ifdef MOTIF1_1
  XrmMergeDatabases(new_db, &(display->db));
#else
  sav_db = XtDatabase(display);
  XrmCombineDatabase(new_db, &sav_db, True);
#endif

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
  OptionMenu = XmCreateSimpleOptionMenu(Form, "OptionMenu", args, n);
  XtManageChild(OptionMenu);

  /*
   *  Reset the database; end of test
   */

#ifdef MOTIF1_1
  XrmDestroyDatabase(display->db);
  display->db = XrmGetFileDatabase("SimpleM2.savdb");
#else
  XrmDestroyDatabase(sav_db);
  new_db = XrmGetFileDatabase("SimpleM2.savdb");
  XrmSetDatabase(display,new_db);
#endif
}

/* ARGSUSED */

static void 
CoreDump4CB(Widget w, XtPointer client, XtPointer call)
{
  XrmDatabase new_db, sav_db;
  XmString tcs;

  /*  Read in specific database */

  new_db = XrmGetFileDatabase("SimpleM2.db5");

#ifdef MOTIF1_1
  XrmMergeDatabases(new_db, &(display->db));
#else
  sav_db = XtDatabase(display);
  XrmCombineDatabase(new_db, &sav_db, True);
#endif

  n = 0;
  XtSetArg(args[n], XmNwidth, 300); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  PopupMenu = XmCreateSimplePopupMenu(Shell1, "PopupMenu", args, n);
  XtAddEventHandler(Shell1, ButtonPressMask, False, PostMenu, PopupMenu);

  n = 0;
  tcs = XmStringCreateSimple("Push Button 3 for Popup");
  XtSetArg(args[n], XmNlabelString, tcs);			n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
  Label = XmCreateLabel(Form, "Label", args, n);
  XtManageChild(Label);
  XmStringFree(tcs);

  /*
   *  Reset the database; end of test
   */

#ifdef MOTIF1_1
  XrmDestroyDatabase(display->db);
  display->db = XrmGetFileDatabase("SimpleM2.savdb");
#else
  XrmDestroyDatabase(sav_db);
  new_db = XrmGetFileDatabase("SimpleM2.savdb");
  XrmSetDatabase(display,new_db);
#endif
}

/* ARGSUSED */

void buttonCB(Widget w, XtPointer client, XtPointer call)
{
  XmString label;
  String s;
  Arg arg;

  XtSetArg(arg, XmNlabelString, &label);
  XtGetValues(w, &arg, 1);

  XmStringGetLtoR(label, XmSTRING_DEFAULT_CHARSET, &s);
  printf("%s is pressed\n", s);
  
  return;
}

/* ARGSUSED */

void 
PostMenu(Widget w, Widget popup, XButtonEvent *event)
{
  if (event->button != Button3)
      return;

  XmMenuPosition(popup, event);
  XtManageChild(popup);
}

