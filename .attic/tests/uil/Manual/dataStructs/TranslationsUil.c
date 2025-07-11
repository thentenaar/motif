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
static char rcsid[] = "$XConsortium: TranslationsUil.c /main/4 1995/07/13 20:35:10 drk $"
#endif
#endif

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/TextFP.h>
#include <Mrm/MrmPublic.h>


static void actSetTextCb();
static void actquit();

static void SetTextCb();
static void quit();
static MrmHierarchy mrmId;
static char *mrmVec[]={"tst.uid"};
static MrmCode mrmClass;
static MRMRegisterArg mrmNames[] = {
  {"SetTextCb", (caddr_t)SetTextCb },
  {"quit", (caddr_t)quit }
};	

static  XtActionsRec  actions[] = {
  { "actSetTextCb", actSetTextCb },
  { "actquit",  actquit }, 
};

static XtAppContext  appContext;

static Widget appMain;
  
main(argc, argv)
     int argc;
     char *argv[];
{

  Widget shell;
  Display *display;
  char *uid;


  MrmInitialize ();


  XtToolkitInitialize();
  appContext = XtCreateApplicationContext();
  display = XtOpenDisplay(appContext, NULL, "test", "Test",
			  NULL, 0, &argc, argv);
  if (display == NULL) {
    fprintf(stderr, "%s:  Can't open display\n", argv[0]);
    exit(1);
  }

  shell = XtAppCreateShell("Uil Version", NULL, applicationShellWidgetClass,
			   display, NULL, 0);

  uid = XtMalloc (strlen (argv[0]) + 5);
  sprintf (uid, "%s.uid", argv[0]); 
  if (MrmOpenHierarchy (1, &uid, NULL, &mrmId) != MrmSUCCESS) exit(0);
  MrmRegisterNames(mrmNames, XtNumber(mrmNames));
  MrmFetchWidget (mrmId, "appMain", shell, &appMain, &mrmClass);
  XtManageChild(appMain);

  XtAppAddActions(appContext, actions, XtNumber(actions));
  
  XtRealizeWidget(shell);

  XtInstallAccelerators(XtNameToWidget(appMain, "text"),
                        XtNameToWidget(appMain, "squeeze"));
  
  XtAppMainLoop(appContext);


}

static void SetTextCb(w, id, cb)
     Widget w;
     char *id;
     XtPointer cb;
{
  Widget text = XtNameToWidget(appMain, id);
  char *value = XmTextFieldGetString (text);
  XmString label = XmStringCreate (value, XmSTRING_DEFAULT_CHARSET); 
  Arg args[1];
  
  XtSetArg (args[0], XmNlabelString, label);
  XtSetValues (w, args, 1);
  XmStringFree (label);
}

static void quit(w, client_data, call_data )
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Widget tw, fw;
  char *cp, *value;
  XmString label;
  Arg args[10];
  int n;

  cp = (char *)client_data;
  tw = XtNameToWidget(appMain, cp);
  value =  XmTextFieldGetString (tw);

  n = 0;
  XtSetArg (args[n], XmNlabelString, &label); n++;
  XtGetValues (XtNameToWidget(appMain, "squeeze"), args, n);
  XmStringGetLtoR(label, XmSTRING_DEFAULT_CHARSET, &cp);

  fw = XtParent(w);
  XtDestroyWidget(XtParent(fw));


  printf("\nThe current text field string : %s \n\n", value);

  printf("\nThe current Shift button label : %s \n\n", cp);

  exit(0);
}

static void actSetTextCb(w, event, pars, num_pars)
     Widget w;
     XEvent *event;
     String *pars;
     Cardinal *num_pars;
{
  SetTextCb(w, "*text", NULL);

}

static void actquit(w, event, pars, num_pars)
     Widget w;
     XEvent *event;
     String *pars;
     Cardinal *num_pars;
{

  quit(w, "*text", NULL);

}




  
