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
static char rcsid[] = "$XConsortium: ArrowBtn1.c /main/7 1995/07/13 17:28:12 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.2              *
*       Tue Apr  4 12:26:53 1989        *
****************************************/

#include <testlib.h>

Widget ArrowButton1;

static void do_activate(w, client, call)
Widget w;
XtPointer client, call;
{
  XmAnyCallbackStruct *cb;

  cb = (XmAnyCallbackStruct *) call;
  if (cb->event == NULL)
    printf ("ACTIVATED. Error! Event is NULL!\n");
  else
    printf ("ACTIVATED.\n");
}

static void do_arm(w, client, call)
Widget w;
XtPointer client, call;
{
  printf("armed\n");
}

static void do_disarm(w, client, call)
Widget w;
XtPointer client, call;
{
  printf("disarmed\n");
}

static XtCallbackRec activate_selected[] = { do_activate, NULL, NULL, NULL };
static XtCallbackRec arm_selected[]      = { do_arm,      NULL, NULL, NULL };
static XtCallbackRec disarm_selected[]   = { do_disarm,   NULL, NULL, NULL };

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           tcs;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  n = 0;
  ArrowButton1 = XmCreateArrowButton(Shell1, "ArrowButton1", args, n);
  XtManageChild(ArrowButton1);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_LEFT);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, 0);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  printf("trying illegal arrow direction\n");
  n = 0;
  XtSetArg(args[n], XmNarrowDirection, 100);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  printf("trying illegal arrow direction\n");
  n = 0;
  XtSetArg(args[n], XmNarrowDirection, 5);  n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  printf("trying to set up activate callback\n");
  n = 0;
  XtSetArg(args[n], XmNactivateCallback, activate_selected); n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  printf("trying to set up arm callback\n");
  n = 0;
  XtSetArg(args[n], XmNarmCallback, arm_selected); n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  printf("trying to set up disarm callback\n");
  n = 0;
  XtSetArg(args[n], XmNdisarmCallback, disarm_selected); n++;
  XtSetValues(ArrowButton1, args, n);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(ArrowButton1);
}






































