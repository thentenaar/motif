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
static char rcsid[] = "$XConsortium: SimpleM3.c /main/6 1995/07/13 18:40:41 drk $"
#endif
#endif


#include <testlib.h>

/*  functions defined in this program  */

void main();
void activateCB(); /* Callback for the PushButton */

void PostIt(Widget w, XtPointer p, XEvent *event, Boolean *cont)
{
  Arg args[1];
  unsigned int whichButton;
  Widget popup = (Widget) p;
 
  XtSetArg (args[0], XmNwhichButton, &whichButton);
  XtGetValues (popup, args, 1);

  if (event->xbutton.button != whichButton)
    return;

  XmMenuPosition(popup, (XButtonPressedEvent *)event);
  XtManageChild(popup);
}

#define cs(string)  XmStringLtoRCreate(string, XmSTRING_DEFAULT_CHARSET)

static XmString XmNULL;

static void SimpleCallbackProc(Widget w,
					 XtPointer closure, XtPointer call_data)
{
  printf("selected=%d\n", (long) closure);
  switch ((long)closure) {
  case 4: exit(1);
    break;
  }
}

void main (argc,argv)
unsigned int argc;
char **argv;
{
  Widget form;
  Widget bar, option, popup, radio_box, check_box;
  Widget pd0, pd1, pd2;
  Arg args[10]; 
  register int n;

  XmNULL = cs("");

  CommonTestInit(argc, argv);

  n = 0;
  form = XmCreateForm(Shell1, "form", args, n);
  XtManageChild(form);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++; 
  bar = XmVaCreateSimpleMenuBar(form, "bar", 
				XmVaCASCADEBUTTON, cs("File"), (int) 'F',
				XmVaCASCADEBUTTON, cs("Edit"), (int) 'E',
				XmVaCASCADEBUTTON, cs("Help"), (int) 'H',
				NULL);
  XtSetValues(bar, args, n);
  XtManageChild(bar);

  pd0 = 
    XmVaCreateSimplePulldownMenu(bar, "pd0", 0, SimpleCallbackProc,
	      XmVaTITLE, cs("This is the Title"),
	      XmVaDOUBLE_SEPARATOR,
	      XmVaPUSHBUTTON, cs("New"), (int) 'N', "", XmNULL,
	      XmVaPUSHBUTTON, cs("Open..."), (int) 'O', "", XmNULL,
	      XmVaPUSHBUTTON, cs("Save"), (int) 'S', "", XmNULL,
	      XmVaPUSHBUTTON, cs("Save As..."), (int) 'A', "", XmNULL,
	      XmVaSEPARATOR,
	      XmVaPUSHBUTTON, cs("Exit"), (int) 'E', "", XmNULL, NULL);

  pd1 = XmVaCreateSimplePulldownMenu(bar, "pd1", 1, SimpleCallbackProc,
				     XmVaPUSHBUTTON, cs("Cut"), 
				     (int) 'C', "", XmNULL,
				     XmVaPUSHBUTTON, cs("Copy"), 	
				     (int) 'p', "", XmNULL,
				     XmVaPUSHBUTTON, cs("Paste"), 
				     (int) 'e', "", XmNULL,
				     NULL);

  pd2 = XmVaCreateSimplePulldownMenu(bar, "pd2", 2, (XtPointer) NULL,
				     XmVaPUSHBUTTON, cs("On Context..."), 
				     (int) ' ', "", XmNULL,
				     XmVaCHECKBUTTON, cs("On Help..."), 
				     (int) ' ', "", XmNULL,
				     XmVaRADIOBUTTON, cs("On Keys"), 
				     (int) ' ', "", XmNULL,
				     NULL);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, bar); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
  option = XmVaCreateSimpleOptionMenu(form, "option", cs("MyLabel"), 
				      (KeySym) 'M', 1, SimpleCallbackProc,
				      XmVaPUSHBUTTON, cs("Red"), (int) 'R', 
				      "", XmNULL,
				      XmVaCASCADEBUTTON, cs("Help"), (int) 'H',
				      XmVaSEPARATOR,
				      XmVaPUSHBUTTON, cs("Green"), (int) 'G', 
				      "", XmNULL,
				      XmVaSEPARATOR,
				      XmVaPUSHBUTTON, cs("Blue"), (int) 'B', 
				      "", XmNULL,
				      NULL);
  XtSetValues(option, args, n);
  XtManageChild(option);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, option); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
  radio_box = XmVaCreateSimpleRadioBox(form, "radio_box", 2, 
				       SimpleCallbackProc,
				       XmVaTOGGLEBUTTON, cs("Variable"), 
				       (int) 'V', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Fixed"), 
				       (int) 'F', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Times"), 
				       (int) 'T', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Courier"), 
				       (int) 'C', "", XmNULL,
				       NULL);

  XtSetValues(radio_box, args, n);
  XtManageChild(radio_box);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, radio_box); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  check_box = XmVaCreateSimpleCheckBox(form, "check_box", SimpleCallbackProc,
				       XmVaTOGGLEBUTTON, cs("Burgers"), 
				       (int) 'B', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Fries"), 
				       (int) 'F', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Shake"), 
				       (int) 'S', "", XmNULL,
				       XmVaTOGGLEBUTTON, cs("Coke"), 
				       (int) 'C', "", XmNULL,
				       NULL);

  XtSetValues(check_box, args, n);
  XtManageChild(check_box);

  n = 0;
  popup = XmVaCreateSimplePopupMenu(form, "popup", SimpleCallbackProc,
				    XmVaTITLE, cs("This is the Title"),
				    XmVaDOUBLE_SEPARATOR,
				    XmVaPUSHBUTTON, cs("push1"), (int) 'B', 
				    "", XmNULL,
				    XmVaPUSHBUTTON, cs("push2"), (int) 'F', 
				    "", XmNULL,
				    XmVaCHECKBUTTON, cs("check1"), (int) 'S', 
				    "", XmNULL,
				    XmVaRADIOBUTTON, cs("radio1"), (int) 'C', 
				    "", XmNULL,
				    NULL);

  XtAddEventHandler(form, ButtonPressMask, False, PostIt, (XtPointer)popup);

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();  /* Exit */

  XtAppMainLoop(app_context);
}

