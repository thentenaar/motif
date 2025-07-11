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
static char rcsid[] = "$XConsortium: Form22.c /main/4 1995/07/13 18:07:07 drk $"
#endif
#endif


#include <testlib.h>
#define MAXTEXT 4

Widget Form;
Widget ScrolledList;
Widget PushButton1;
Widget PushButton2;

static void die();
static void RunTest();
static void CreateForm();
static void DestroyForm();

void  main(argc, argv)
int argc;
char **argv;
{
  register int  n;
  Arg args[MAX_ARGS];
  
  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg (args[n], XmNwidth,  50);                               n++;
  XtSetArg (args[n], XmNheight, 25);                               n++;
  XtSetValues(Shell1, args, n);
    
  XtRealizeWidget(Shell1);    

  RunTest(Shell1);

  XtAppMainLoop(app_context);
}

static void RunTest(Shell)
Widget Shell;
{
  register int  i;
  register int  n;
  Arg args[MAX_ARGS];
  
  CreateForm(Shell);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE);                 n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
  XtSetValues(PushButton1, args, n);
  XtSetValues(PushButton2, args, n);
  CommonPause();

  DestroyForm();
  CreateForm(Shell);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE);                 n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);   		n++;
  XtSetValues(PushButton1, args, n);
  XtSetValues(PushButton2, args, n);
  CommonPause();

  DestroyForm();
}

static void CreateForm(Shell)
Widget Shell;
{
  register int  i;
  register int  n;
  Arg           args[MAX_ARGS];
  char          label[25];
  XmString      items[20];
  int           nitems = 20;

  for (i = 0; i < nitems; i++) {
    sprintf(label, "Item: %d", i);
    items[i] = XmStringCreate(label, XmSTRING_DEFAULT_CHARSET);
  }
  
  n = 0;
  XtSetArg(args[n], XmNautoUnmanage, False);                           n++;
  XtSetArg(args[n], XmNallowShellResize, True);                        n++;
  Form = XmCreateFormDialog(Shell, "Form", args, n);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);                 n++;
  PushButton1 = XmCreatePushButton(Form, "PushButton1", args, n);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);               n++;
  XtSetArg(args[n], XmNleftWidget, PushButton1);                       n++;
  XtSetArg(args[n], XmNitemCount, nitems);                             n++;
  XtSetArg(args[n], XmNitems, items);                                  n++;
  XtSetArg(args[n], XmNvisibleItemCount, 10);                          n++;
  ScrolledList = XmCreateScrolledList(Form, "ScrolledList", args, n);

  n = 0;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);                n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);               n++;
  XtSetArg(args[n], XmNleftWidget, XtParent(ScrolledList));            n++;
  PushButton2 = XmCreatePushButton(Form, "PushButton2", args, n);

  XtManageChild(PushButton1);
  XtManageChild(PushButton2);
  XtManageChild(ScrolledList);
  XtManageChild(Form);
}

static void DestroyForm()
{
  XtDestroyWidget(Form);
}
