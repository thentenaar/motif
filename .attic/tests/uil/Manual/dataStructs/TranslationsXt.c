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
static char rcsid[] = "$XConsortium: TranslationsXt.c /main/4 1995/07/13 20:35:20 drk $"
#endif
#endif
  
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>
  
static void actSetTextCb();
static void actquit();

static void enter();
static void leave();

String button_trans = "<Btn1Down>, <Btn1Up>: actSetTextCb() \n\
                       Ctrl<Key>q: actquit()";
     
String appMain_trans = "Ctrl<Key>q: actquit()";
     
String text_trans = "Ctrl<Key>q: actquit() \n\
                     Ctrl<Key>b: beginning-of-line() \n\
                     Ctrl<Key>e: end-of-line() ";
     
String done_trans = "<Btn1Down>: actquit() \n\
                     Ctrl<Key>q: actquit()";
     
static  XtActionsRec  actions[] = {
  { "actSetTextCb", actSetTextCb },
  { "actquit",  actquit },
};
     
     
static XtAppContext  appContext;
     
static Widget appMain, shell, textfd, button, done; 
     
     
main(argc, argv)
     int argc;
     char *argv[];
{
  
  Display *display;
  Arg args[10];
  Boolean size;
  int n;
  XtAccelerators accels;
  
  
  XtToolkitInitialize();
  appContext = XtCreateApplicationContext();
  display = XtOpenDisplay(appContext, NULL, "test", "Test",
			  NULL, 0, &argc, argv);
  if (display == NULL) {
    fprintf(stderr, "%s:  Can't open display\n", argv[0]);
    exit(1);
  }
  
  
  shell = XtAppCreateShell("Xt Version", NULL, applicationShellWidgetClass,
			   display, NULL, 0);
  
  appMain = XmCreateForm(shell, "form", NULL, 0);
  XtOverrideTranslations(appMain, XtParseTranslationTable(appMain_trans)); 
  XtManageChild(appMain);
 
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopPosition, 0); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomPosition, 100); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 10);n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 40); n++;
  
  textfd = XmCreateTextField(appMain, "text", args, n);
  XtOverrideTranslations(textfd, XtParseTranslationTable(text_trans)); 
  XtManageChild(textfd);
  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopPosition, 0); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomPosition, 100); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50);n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 80); n++;
  
  button = XmCreatePushButton(appMain, "Change My Label", args, n);
  n = 0;
  XtSetArg(args[n], XmNdefaultButton, button); n++;
  XtSetValues(appMain, args, n);
  XtOverrideTranslations(button, XtParseTranslationTable(button_trans)); 
  XtManageChild(button);
  
  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopPosition, 0); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomPosition, 100); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 85);n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 95); n++;
  
  
  done = XmCreatePushButton(appMain, "Done", args, n);
  XtOverrideTranslations(done, XtParseTranslationTable(done_trans)); 
  XtManageChild(done);
  
  XtAppAddActions(appContext, actions, XtNumber(actions));
  
  XtRealizeWidget(shell);

  XtInstallAccelerators(textfd, button);
  
  XtAppMainLoop(appContext);
  
  
}

static void SetTextCb(w, id, cb)
     Widget w;
     char *id;
     XtPointer cb;
{
  Widget txt = XtNameToWidget(appMain, id); 
  char *value = XmTextFieldGetString (txt);
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
  XtGetValues (button, args, n);
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




