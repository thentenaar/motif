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
static char rcsid[] = "$XConsortium: Form10.c /main/4 1995/07/13 18:00:48 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.4              *
*       Tue May  9 15:22:45 1989        *
****************************************/

#include <testlib.h>

Widget Form1, ArrowBtn1, ArrowBtn2, ArrowBtn3;
void  main(argc, argv)
int     argc;
char  **argv;
{
    Boolean       trace = False;
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  400);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
    XtSetValues(Shell1, args, n);

    XtRealizeWidget(Shell1);

    n = 0;
    Form1 = XmCreateForm(Shell1, "Form1", args, n);
    XtManageChild(Form1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;

    ArrowBtn1 = XmCreateArrowButton(Form1, "ArrowBtn1", args, n);
    XtManageChild(ArrowBtn1);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNtopOffset, 10);  n++;
    XtSetArg(args[n], XmNbottomOffset, 30);  n++;
    XtSetArg(args[n], XmNleftOffset, 20);  n++;
    XtSetArg(args[n], XmNrightOffset, 40);  n++;
    XtSetValues(ArrowBtn1, args, n);

    CommonPause();

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION);  n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION);  n++;
    XtSetArg(args[n], XmNleftPosition, 10);  n++;
    XtSetArg(args[n], XmNrightPosition, 90);  n++;
    XtSetArg(args[n], XmNtopPosition, 25);  n++;
    XtSetArg(args[n], XmNbottomPosition, 75);  n++;
    XtSetValues(ArrowBtn1, args, n);

    CommonPause();

   /* 
      Note: either unmanaging and remanaging the Form, or unrealizing
      and realizing the Shell, will fix the ugly Shell sizing problem
      shown herein, PIR 1581.
   */

    n = 0;
    XtSetArg(args[n], XmNtopPosition, 3);  n++;
    XtSetArg(args[n], XmNbottomPosition, 8);  n++;
    XtSetArg(args[n], XmNleftPosition, 1);  n++;
    XtSetArg(args[n], XmNrightPosition, 9);  n++;
    XtSetValues(ArrowBtn1, args, n);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, 10);  n++;
    XtSetValues(Form1, args, n);

    CommonPause();
  
    XtDestroyWidget (ArrowBtn1);

    n = 0;
    XtSetArg(args[n], XmNfractionBase, 100);  n++;
    XtSetValues(Form1, args, n);
   
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 10);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 30);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopOffset, 20);			n++;
   XtSetArg (args[n], XmNwidth, 100);		n++;
   XtSetArg (args[n], XmNheight, 100);		n++;
   ArrowBtn1 = XmCreateArrowButton(Form1, "ArrowBtn1", args, n);
   XtManageChild(ArrowBtn1);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 40);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 60);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopOffset, 0);				n++;
   XtSetArg (args[n], XmNwidth, 100);		n++;
   XtSetArg (args[n], XmNheight, 100);		n++;
   ArrowBtn2 = XmCreateArrowButton(Form1, "ArrowBtn2", args, n);
   XtManageChild(ArrowBtn2);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 70);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 90);			n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomOffset, 5);			n++;
   XtSetArg (args[n], XmNwidth, 100);		n++;
   XtSetArg (args[n], XmNheight, 100);		n++;
   ArrowBtn3 = XmCreateArrowButton(Form1, "ArrowBtn3", args, n);
   XtManageChild(ArrowBtn3);

   XtRealizeWidget (Shell1);

   CommonPause();

   XtAppMainLoop(app_context);
}













