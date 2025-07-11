/* $TOG: XmeTargets.c /main/6 1997/11/14 14:25:34 csn $ */
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

/*  Standard C headers  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* QATS headers */
#include <testlib.h>

/*  X headers  */
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

/*  Xm headers  */
#include <Xm/Xm.h>
#include <Xm/DragDrop.h>
#include <Xm/Transfer.h>

/*  Exm headers  */
#include <Exm/CommandB.h>
#include <Exm/Grid.h>
#include "Targets.h"	/* Because this header isn't installed in the current */
			/* implementation, it is included from the local directory		*/

#define	MAX_DROPSITES	10

Display *display;
Window window;

static void DestinationCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data);
static void DoneProcCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data);
static void SetForegroundCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data);
static void StandardTestCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data);
static void TargetConvertCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data);

Widget		targ1, targ2, reset;
Widget		xmDisplay;
XmString	default_str1, default_str2, reset_str;

unsigned char	initiatorProtocolStyle;
unsigned char	receiverProtocolStyle;
unsigned char	animationStyle;
unsigned char	dropActivity;
unsigned char	dropOperations;
unsigned char	dragOperations;
Boolean done_proc_called = False;

static int ProtoError(dpy, ev)
Display		*dpy;
XErrorEvent	*ev;
{
	fprintf(stderr, "Proto Error:");
	switch (ev->error_code) {
	case BadAccess:
	  fprintf(stderr,"\tBadAccess\n"); break;
	case BadAlloc:
	  fprintf(stderr,"\tBadAlloc\n"); break;
	case BadAtom:
	  fprintf(stderr,"\tBadAtom\n"); break;
	case BadColor:
	  fprintf(stderr,"\tBadColor\n"); break;
	case BadCursor:
	  fprintf(stderr,"\tBadCursor\n"); break;
	case BadDrawable:
	  fprintf(stderr,"\tBadDrawable\n"); break;
	case BadFont:
	  fprintf(stderr,"\tBadFont\n"); break;
	case BadGC:
	  fprintf(stderr,"\tBadGC\n"); break;
	case BadIDChoice:
	  fprintf(stderr,"\tBadIDChoice\n"); break;
	case BadImplementation:
	  fprintf(stderr,"\tBadImplementation\n"); break;
	case BadLength:
	  fprintf(stderr,"\tBadLength\n"); break;
	case BadMatch:
	  fprintf(stderr,"\tBadMatch\n"); break;
	case BadName:
	  fprintf(stderr,"\tBadName\n"); break;
	case BadPixmap:
	  fprintf(stderr,"\tBadPixmap\n"); break;
	case BadRequest:
	  fprintf(stderr,"\tBadRequest\n"); break;
        case BadValue:
	  fprintf(stderr,"\tBadValue\n"); break;
	case BadWindow:
	  fprintf(stderr,"\tBadWindow\n"); break;
	default:
	  fprintf(stderr,"\tUnknown error_code\n"); break;
	}
#ifdef DEBUG
	fflush(outfp);
	fclose(outfp);
#endif
	exit(0);
}

/*** define callbacks for setting up test cases ***/

/*ARGSUSED*/
static void 
TargetConvertCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data)
{
  int n = (int) client_data;
  Atom file = XInternAtom(XtDisplay(w), "FILE", False);
  Atom targets = XInternAtom(XtDisplay(w), "TARGETS", False);
  XmConvertCallbackStruct *cs = (XmConvertCallbackStruct *) call_data;

  if (cs->target == targets) {
    Atom *targs = (Atom *) XtMalloc(sizeof(Atom));
    if (targs == NULL) {
      printf("ERROR: couldn't malloc for targets in TargetConvertCB\n");
      cs->status = XmCONVERT_REFUSE;
      return;
    }
    targs[0] = file;
    cs->value = targs;
    cs->length = 1;
    cs->type = XA_ATOM;
    cs->format = 32;
    cs->status = XmCONVERT_MERGE;
  }
  return;
}

/*ARGSUSED*/
static void 
DestinationCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data)
{
  int n = (int) client_data;
  XmDestinationCallbackStruct *dc = (XmDestinationCallbackStruct *) call_data;

  XmeTransferAddDoneProc(dc->transfer_id, (XmSelectionFinishedProc)DoneProcCB);

  return;
}

/*ARGSUSED*/
static void 
StandardTestCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data)
{
  int n = (int) client_data;
  Atom FOREGROUND = XInternAtom(XtDisplay(w), "FOREGROUND", False);
  XmDestinationCallbackStruct *dc = (XmDestinationCallbackStruct *) call_data;

  XmTransferValue(dc->transfer_id,
		  FOREGROUND, /* target */
		  (XtCallbackProc) SetForegroundCB, /* proc */
		  (XtPointer) NULL, /* client_data */
		  NULL); /* parm */ 
  return;
}

/*ARGSUSED*/
static void 
SetForegroundCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data)
{
  XmSelectionCallbackStruct *ss = (XmSelectionCallbackStruct *) call_data;
  Atom FOREGROUND = XInternAtom(XtDisplay(w), "FOREGROUND", False);

/*printf("SetForegroundCB has been called\n");*/

/*  printf ("widget: %d\n",(int)w);
  printf ("ss->value: %d\n",(int) *(ss->value));*/
}

/*ARGSUSED*/
static void 
DoneProcCB(
	Widget w,
	XtPointer client_data, 
	XtPointer call_data)
{
  int n = (int) client_data;
  XmTransferDoneCallbackStruct *ts = 
    (XmTransferDoneCallbackStruct *) call_data;

  done_proc_called = True;
  return;
}

/*ARGSUSED*/
static void ResetCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
  int	i, n, status;
  unsigned long item_id;
  XmString clip_data;
  char buf[32];
  static int cnt;
  Arg	args[MAX_ARGS];

  /* reset strings in test widgets */
  n = 0;
  XtSetArg(args[n], ExmNcompoundString, default_str1); n++;
  XtSetValues(targ1, args, n);

  n = 0;
  XtSetArg(args[n], ExmNcompoundString, default_str2); n++;
  XtSetValues(targ2, args, n);
}

int 
main (int argc, char **argv)
{
  register int       n;
  Arg                args[MAX_ARGS], dropSiteArgs[3];
  Widget       	     grid;
  char               name[80];
  XmString           tcs;
  unsigned char      alignment, dragOperations, dropSiteActivity;

  /* set default values */
  initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;
  receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;
  animationStyle = XmDRAG_UNDER_HIGHLIGHT;
  dropActivity = XmDROP_SITE_ACTIVE;
  dropOperations = XmDROP_MOVE | XmDROP_COPY;
  dragOperations = XmDROP_MOVE | XmDROP_COPY;

  sprintf(name, "%s.out", argv[0]);

#ifdef DEBUG
  if ((outfp = fopen(name, "w+")) == NULL) {
    fprintf(stderr, "Can not open output file %s.out. Trying in /tmp\n",
	    argv[0]);
    sprintf(name, "/tmp/%s.out", argv[0]);
    if ((outfp = fopen(name, "w+")) == NULL) {
      fprintf(stderr, "Can not open output file /tmp/%s.out. Exiting.\n",
	      argv[0]);
      exit(0);
    }
  }    
#endif

  XSetErrorHandler(ProtoError);

  CommonTestInit(argc, argv);

  /* Set the initiatorProtocolStyle and recieverProtocolStyle resources */
  n = 0;
  xmDisplay = XmGetXmDisplay(XtDisplay(Shell1));
  XtSetArg(args[n], XmNdragInitiatorProtocolStyle, 
	   initiatorProtocolStyle); n++;
  XtSetArg(args[n], XmNdragReceiverProtocolStyle, 
	   receiverProtocolStyle); n++;
  XtSetValues(xmDisplay, args, n);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);
  window = XtWindow(Shell1);
  display = XtDisplay(Shell1);

  /* Give the user basic instructions for the test */
  CommonPause();

  n = 0;
  grid = XtVaCreateManagedWidget("Grid",
				 exmGridWidgetClass, Shell1, 
				 XmNrows, 1,
				 XmNcolumns, 3,
				 NULL); 

  default_str1 = XmStringCreateLocalized("Widget ONE");
  targ1 = XtVaCreateManagedWidget("targ1",
				  exmTargetsWidgetClass, grid, 
				  XmNwidth, 100,
				  XmNheight, 10,
/*				  XtVaTypedArg, XmNforeground, XmRString,
				  	"red", strlen("red") + 1,*/
				  XmNalignment, XmALIGNMENT_CENTER,
				  ExmNcompoundString, default_str1,
				  NULL);
  XtAddCallback(targ1, XmNconvertCallback, TargetConvertCB, (XtPointer)1);
  XtAddCallback(targ1, XmNdestinationCallback, DestinationCB, (XtPointer)1);

  default_str2 = XmStringCreateLocalized("Widget TWO");
  targ2 = XtVaCreateManagedWidget("targ2",
				   exmTargetsWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 10,
				   ExmNcompoundString, default_str2,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);
  XtAddCallback(targ2, XmNconvertCallback, TargetConvertCB, (XtPointer)2);
  XtAddCallback(targ2, XmNdestinationCallback, DestinationCB, (XtPointer)2);

  reset_str = XmStringCreateLocalized("RESET");
  reset = XtVaCreateManagedWidget("reset",
				   exmCommandButtonWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   XmNalignment, XmALIGNMENT_CENTER,
				   ExmNcompoundString, reset_str,
				   NULL);
  XtAddCallback(reset, XmNactivateCallback, ResetCB, (XtPointer)1);

  /* have the user perform a compound string data transfer */
  CommonPause();

  /* validate the XmeTransferAddDoneProc call from the previous test. */
  if (!done_proc_called)
    printf("ERROR: XmeTransferAddDoneProc call has failed\n");

  /* remove the callback for the next test case */
  XtRemoveCallback(targ1, XmNdestinationCallback, DestinationCB, (XtPointer)1);
  XtRemoveCallback(targ2, XmNdestinationCallback, DestinationCB, (XtPointer)2);
  XtAddCallback(targ1, XmNdestinationCallback, StandardTestCB, (XtPointer)1);
  XtAddCallback(targ2, XmNdestinationCallback, StandardTestCB, (XtPointer)2);

  /* test standard convert by requesting a FOREGROUND target in the 
     StandardTestCB. */
  CommonPause();

  /* clean up */
  XmStringFree(default_str1);
  XmStringFree(default_str2);
  XmStringFree(reset_str);

  CommonPause(); /* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
