/* $TOG: XmeTransfer.c /main/6 1997/11/14 15:05:30 csn $ */
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

/*  Exm headers  */
#include <Exm/StringTrans.h>
#include <Exm/Grid.h>

#define	MAX_DROPSITES	10

Display *display;
Window window;

static Widget		CreatePushButton();
static void	 	Quit();
static void	 	StartDrag();

Widget stwid1, stwid2;
Widget xmDisplay;
Widget Form;

Widget Reset;

Widget  TestCase_Pulldown;
Widget  TestCase_Option;
Widget  TC_DragAndDrop, TC_Primary, TC_Secondary, TC_Clipboard;

unsigned char	initiatorProtocolStyle;
unsigned char	receiverProtocolStyle;
unsigned char	animationStyle;
unsigned char	dropActivity;
unsigned char	dropOperations;
unsigned char	dragOperations;

XmString	default_str1, default_str2;
XtTranslations  dnd_trans, prim_trans, sec_trans, clip_trans;

static char dndTranslations[] = 
	"<Btn2Down>:		ProcessDrag()\n\
	<EnterWindow>:		PrimitiveEnter()\n\
	<LeaveWindow>:		PrimitiveLeave()\n\
	~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
	<Key>osfActivate:	PrimitiveParentActivate()\n\
	<Key>osfCancel:		PrimitiveParentCancel()\n\
	<Key>osfHelp:		PrimitiveHelp()";

static char primaryTranslations[] = 
	"<Btn1Down>:		StartHighlight()\n\
	<Btn1Motion>:		ExtendHighlight()\n\
	<Btn1Up>:		MakePrimSelection()\n\
	s <Btn2Down>:		PrimMove()\n\
	<Btn2Down>:		PrimCopy()\n\
	<EnterWindow>:		PrimitiveEnter()\n\
	<LeaveWindow>:		PrimitiveLeave()\n\
	~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
	<Key>osfActivate:	PrimitiveParentActivate()\n\
	<Key>osfCancel:		PrimitiveParentCancel()\n\
	<Key>osfHelp:		PrimitiveHelp()";


static char secondaryTranslations[] = 
	"<Btn1Down>:		StartUnderline()\n\
	<Btn1Motion>:		ExtendUnderline()\n\
	<Btn1Up>:		MakeSecSelection()\n\
	<Btn2Down>:		SecSink()\n\
	s <Btn3Down>:		SecMove()\n\
	<Btn3Down>:		SecCopy()\n\
	<EnterWindow>:		PrimitiveEnter()\n\
	<LeaveWindow>:		PrimitiveLeave()\n\
	~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
	<Key>osfActivate:	PrimitiveParentActivate()\n\
	<Key>osfCancel:		PrimitiveParentCancel()\n\
	<Key>osfHelp:		PrimitiveHelp()";

static char clipboardTranslations[] = 
	"<Btn1Down>:		StartClipSelect()\n\
	<Btn1Motion>:		ExtendClipSelect()\n\
	s <Btn1Up>:		ClipMoveSrc()\n\
	<Btn1Up>:		MakeClipSelection()\n\
	<Btn2Down>:		ClipCopySink()\n\
	<EnterWindow>:		PrimitiveEnter()\n\
	<LeaveWindow>:		PrimitiveLeave()\n\
	~s ~m ~a <Key>Return:	PrimitiveParentActivate()\n\
	<Key>osfActivate:	PrimitiveParentActivate()\n\
	<Key>osfCancel:		PrimitiveParentCancel()\n\
	<Key>osfHelp:		PrimitiveHelp()";

/*** define callbacks for setting up test cases ***/

/*ARGSUSED*/
static void DragAndDropCB(Widget w, XtPointer client_data, 
			  XtPointer call_data)
{
	int	n;
	Arg	args[MAX_ARGS];

	/* reset translations in the test widgets */
	n = 0;
	XtSetArg(args[n], XmNtranslations, dnd_trans); n++;
	XtSetValues(stwid1, args, n);
	XtSetValues(stwid2, args, n);

	return;
}

/*ARGSUSED*/
static void PrimaryCB(Widget w, XtPointer client_data, 
		      XtPointer call_data)
{
	int	n;
	Arg	args[MAX_ARGS];

	/* reset translations in the test widgets */
	n = 0;
	XtSetArg(args[n], XmNtranslations, prim_trans); n++;
	XtSetValues(stwid1, args, n);
	XtSetValues(stwid2, args, n);

	return;
}

/*ARGSUSED*/
static void SecondaryCB(Widget w, XtPointer client_data, 
			XtPointer call_data)
{
	int	n;
	Arg	args[MAX_ARGS];

	/* reset translations in the test widgets */
	n = 0;
	XtSetArg(args[n], XmNtranslations, sec_trans); n++;
	XtSetValues(stwid1, args, n);
	XtSetValues(stwid2, args, n);

	return;
}

/*ARGSUSED*/
static void ClipboardCB(Widget w, XtPointer client_data, 
			XtPointer call_data)
{
	int	n;
	Arg	args[MAX_ARGS];

	/* reset translations in the test widgets */
	n = 0;
	XtSetArg(args[n], XmNtranslations, clip_trans); n++;
	XtSetValues(stwid1, args, n);
	XtSetValues(stwid2, args, n);

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
  XtSetValues(stwid1, args, n);

  n = 0;
  XtSetArg(args[n], ExmNcompoundString, default_str2); n++;
  XtSetValues(stwid2, args, n);

}

/*** end of Test Case callbacks ***/

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


int 
main (int argc, char **argv)
{
  register int       n;
  Arg                args[MAX_ARGS], dropSiteArgs[6];
  Widget       	     topLevel, grid;
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

  dnd_trans = XtParseTranslationTable(dndTranslations);
  prim_trans = XtParseTranslationTable(primaryTranslations);
  sec_trans = XtParseTranslationTable(secondaryTranslations);
  clip_trans = XtParseTranslationTable(clipboardTranslations);

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

  /* give the user general instructions */
  CommonPause();

  XtRealizeWidget(Shell1);
  window = XtWindow(Shell1);
  display = XtDisplay(Shell1);

  /* Create Outer Form */
  n = 0;
  XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
  TestCase_Pulldown = XmCreatePulldownMenu(Shell1, "TestCase_Pulldown", 
					    args, n);

  TC_DragAndDrop = CreatePushButton(TestCase_Pulldown, "DragAndDrop");
  TC_Primary = CreatePushButton(TestCase_Pulldown, "Primary");
  TC_Secondary = CreatePushButton(TestCase_Pulldown, "Secondary");
  TC_Clipboard = CreatePushButton(TestCase_Pulldown, "Clipboard");

  XtAddCallback(TC_DragAndDrop, XmNactivateCallback, 
		DragAndDropCB, NULL);
  XtAddCallback(TC_Primary, XmNactivateCallback, 
		PrimaryCB, NULL);
  XtAddCallback(TC_Secondary, XmNactivateCallback, 
		SecondaryCB, NULL);
  XtAddCallback(TC_Clipboard, XmNactivateCallback, 
		ClipboardCB, NULL);

  tcs = XmStringCreateLocalized("Test Case       ");

  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNmenuHistory, TC_DragAndDrop); n++;
  XtSetArg(args[n], XmNsubMenuId, TestCase_Pulldown); n++;
  XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 5); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 5); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 5); n++;
  TestCase_Option = XmCreateOptionMenu(Form, "TestCase_Option", args, n);
  XtManageChild(TestCase_Option);

  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopOffset, 5); n++;
  XtSetArg(args[n], XmNtopWidget, TestCase_Option); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 5); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 5); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomOffset, 5); n++;
  Reset = XmCreatePushButton(Form, "Reset", args, n);
  XtManageChild(Reset);

  XtAddCallback(Reset, XmNactivateCallback, ResetCB, NULL);

  /* Create test widgets under a separate top level shell */
  n = 0;
  topLevel = XtCreatePopupShell("topLevel", topLevelShellWidgetClass,
				Shell1, args, n);
  n = 0;
  grid = XtVaCreateManagedWidget("Grid",
				 exmGridWidgetClass, topLevel, 
				 XmNheight, 100,
				 XmNwidth, 200,
				 XmNrows, 1,
				 XmNcolumns, 2,
				 NULL); 

  default_str1 = XmStringCreateLocalized("Widget ONE");
  stwid1 = XtVaCreateManagedWidget("STwid1",
				   exmStringTransferWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   XmNalignment, XmALIGNMENT_CENTER,
				   XmNstringDirection,
				   	XmSTRING_DIRECTION_L_TO_R,
				   ExmNcompoundString, default_str1,
				   NULL);
  XtVaGetValues(stwid1,
		XmNalignment, &alignment,
		NULL);
  if (alignment != XmALIGNMENT_CENTER)
    printf("ERROR: default alignment is incorrect for stwid1\n");

  /* validate dropSite data */
  n = 0;
  XtSetArg(dropSiteArgs[n], XmNdragOperations, &dragOperations); n++;
  XtSetArg(dropSiteArgs[n], XmNdropSiteActivity, &dropSiteActivity); n++;
  XmDropSiteRetrieve(stwid1, dropSiteArgs, n);
  if (dragOperations !=  (XmDROP_COPY | XmDROP_MOVE))
    printf("ERROR: incorrect dragOperation for stringTransfer widget #1: %d\n",
	   dragOperations);
  if (dropSiteActivity != XmDROP_SITE_ACTIVE)
    printf("ERROR: wrong dropSiteActivity for stringTransfer widget #1: %d\n",
	   dropSiteActivity);

  default_str2 = XmStringCreateLocalized("Widget TWO");
  stwid2 = XtVaCreateManagedWidget("STwid2",
				   exmStringTransferWidgetClass, grid, 
				   XmNwidth, 100,
				   XmNheight, 100,
				   XmNalignment, XmALIGNMENT_CENTER,
				   XmNstringDirection,
				   	XmSTRING_DIRECTION_L_TO_R,
				   ExmNcompoundString, default_str2,
				   NULL);

  /* validate dropSite data */
  n = 0;
  XtSetArg(dropSiteArgs[n], XmNdragOperations, &dragOperations); n++;
  XtSetArg(dropSiteArgs[n], XmNdropSiteActivity, &dropSiteActivity); n++;
  XmDropSiteRetrieve(stwid2, dropSiteArgs, n);
  if (dragOperations !=  (XmDROP_COPY | XmDROP_MOVE))
    printf("ERROR: incorrect dragOperation for stringTransfer widget #2: %d\n",
	   dragOperations);
  if (dropSiteActivity != XmDROP_SITE_ACTIVE)
    printf("ERROR: wrong dropSiteActivity for stringTransfer widget #2: %d\n",
	   dropSiteActivity);

  XtPopup(topLevel, XtGrabNone);

  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();

  /* clean up */
  XmStringFree(default_str1);
  XmStringFree(default_str2);

  CommonPause(); /* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}

static Widget  CreatePushButton(Widget parent, char *label)
{
	Widget          widget;
	register int	n;
	Arg		args[3];
	XmString	tcs;

	tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	widget = XmCreatePushButton(parent, label, args, n);
	XtManageChild(widget);

	XmStringFree(tcs);
	return(widget);
}
