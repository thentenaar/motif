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
static char rcsid[] = "$XConsortium: Form21.c /main/4 1995/07/13 18:06:43 drk $"
#endif
#endif


#include <testlib.h>
#define MAXTEXT 4

Widget DialogShell;
Widget ScrollWin;
Widget Form;
Widget Text[MAXTEXT];

static void            die();
static void            RunTest();
static void            CreateForm();
static void            DestroyForm();
static void            PrintValues();

void  main(argc, argv)
int     argc;
char    **argv;
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
    
  for (i = 0; i < MAXTEXT; i++) XtUnmapWidget(Text[i]);
  CommonPause();
    
  for (i = 0; i < MAXTEXT; i++) XtMapWidget(Text[i]);
  CommonPause();
    
  DestroyForm();
  CommonPause();

  CreateForm(Shell);
  CommonPause();

  XtUnmanageChild(Text[0]);
  CommonPause();

  XtUnmanageChild(Text[1]);
  CommonPause();
    
  XtUnmanageChild(Text[2]);
  CommonPause();

  XtUnmanageChild(Text[3]);
  CommonPause();
    
  for (i = 0; i < MAXTEXT; i++) XtManageChild(Text[i]);
  CommonPause();
    
  DestroyForm();
}

static void PrintValues(w)
Widget w;
{
  unsigned int n;
  Arg          args[MAX_ARGS];
  Position     x, y;
  Dimension    width, height;
  
  n = 0;
  XtSetArg(args[n], XmNx,      &x);            n++;
  XtSetArg(args[n], XmNy,      &y);            n++;
  XtSetArg(args[n], XmNwidth,  &width);        n++;
  XtSetArg(args[n], XmNheight, &height);       n++;
  XtGetValues(w, args, n);

  printf("\n\tX: %d Y: %d W: %d H: %d\n", x, y, width, height);

  if ( XtIsManaged(w) ) 
    printf("The Widget is Managed.\n");
  else
    printf("The Widget is NOT Managed.\n");
}

static void CreateForm(Shell)
Widget Shell;
{
  register int  i;
  register int  n;
  Arg args[MAX_ARGS];

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True);                  n++;
  DialogShell = XmCreateDialogShell(Shell, "Form21", args, n);

  n = 0;
  XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);            n++;
  XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC);        n++;
  ScrollWin = XmCreateScrolledWindow(DialogShell, "ScrollWin", args, n);
  
  n = 0;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW);             n++;
  Form = XmCreateForm(ScrollWin, "Form", args, n);
  
  for (i = 0; i < MAXTEXT; i++) {
    n = 0;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);                 n++;
    XtSetArg(args[n], XmNcolumns, 40);                                 n++;
    XtSetArg(args[n], XmNrows,    40);                                 n++;
    switch (i) {
    case 0:
      XtSetArg(args[n], XmNvalue, "This is Text0");                    n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);             n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);              n++;
      break;
    case 1:
      n = 0;
      XtSetArg(args[n], XmNvalue, "This is Text1");                     n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);             n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);            n++;
      XtSetArg(args[n], XmNleftWidget, Text[0]);                        n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);               n++;
      XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
      XtSetArg(args[n], XmNbottomWidget, Text[0]);                      n++;
      break;
    case 2:
      n = 0;
      XtSetArg(args[n], XmNvalue, "This is Text2");                    n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);             n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);            n++;
      XtSetArg(args[n], XmNtopWidget, Text[0]);                        n++;
      XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);           n++;
      break;
    case 3:
      n = 0;
      XtSetArg(args[n], XmNvalue, "This is Text3");                     n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);             n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);            n++;
      XtSetArg(args[n], XmNleftWidget, Text[2]);                        n++;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);             n++;
      XtSetArg(args[n], XmNtopWidget, Text[1]);                         n++;
      XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
      XtSetArg(args[n], XmNbottomWidget, Text[2]);                      n++;
      break;
    }
    Text[i] = XmCreateText(Form, "Text", args, n);
  }

  XtManageChildren(Text, MAXTEXT);
  XtManageChild(Form);
  XtManageChild(ScrollWin);
}

static void DestroyForm()
{
  XtDestroyWidget(DialogShell);
}
