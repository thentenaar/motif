/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
static char rcsid[] = "$XConsortium: transfer.c /main/6 1995/07/14 09:47:23 drk $"
#endif
#endif

#include <stdio.h>
#include <Xm/Xm.h>           /* Motif Toolkit */
#include "transfer.h"

void quitCB(Widget widget, char *tag, XmAnyCallbackStruct *callback_data);
void setMode(Widget widget, int param, XmAnyCallbackStruct *callback_data);
void cut(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data);
void copy(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data);
void paste(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data);
void pasteMove(Widget widget, char *ignore, 
	       XmAnyCallbackStruct *callback_data);

static void NOP(Widget widget, XEvent *event, 
		String *params, Cardinal *num_params);
static int ErrorHandler(Display *display, XErrorEvent *event);

/* String to use for Clipboard tagging */
XmString generic_label;
Display  	*display;
XtAppContext  	app_context;

/* This tracks which transfer selection we're using.  Drag and drop isn't
   listed since it is dynamic,  we don't take ownership of DnD */
TransferMode Tmode = Mprimary;

/* We need a NOP action */
static XtActionsRec actions[] = {
  { "NOP",		NOP},
};

/*************************************************************************
 * A list of widgets we need to be able to find outside the main
 * program.
 *
 * toplevel	is the ApplicationShell
 * mainw	is the child of toplevel,  an XmMainWindow
 * target	is the drawing area used for drag and drop
 * list		is the list of targets displayed
 * text1	contains the value of the selected target
 * text2	contains the name of the selected or new target
 * typeOM	is the option menu which displays type information for the
 *  		selected or new target
 * format_om    Format option menu
 * format_s	String format button
 * format_l	Locale format button
 * format_t	CompoundText format button
 * format_cs	CompoundString format button
 * format_id	Integer format button (decimal)
 * format_hd	Integer format button (hex)
 *************************************************************************/

Widget toplevel, mainW, editBox;

/*
 *  Main program
 */

int 
main(int argc, String *argv)
{
    Arg	args[10];
    int n;

    toplevel = XtVaAppInitialize(&app_context, "XmdTransfer", NULL, 0, 
				 &argc, argv, NULL, 0);

    /* Add the NOP actions */
    XtAppAddActions(app_context, actions, XtNumber(actions));

    /* Create edit dialog */
    editBox = create_editbox_widget("edit_box", toplevel);

    /* Initialize the interface */
    mainW = create_mainWindow_widget("main", toplevel);

    /* Create a default label string for the clipboard */
    generic_label = XmStringCreateLocalized("Generic Label");

    /* Register the drawing area as a drop site */
    XmeDropSink(target, NULL, 0);

    XtManageChild(mainW);

    /* Install our special error handler */
    XSetErrorHandler((XErrorHandler) ErrorHandler);

    /* Add the UTM callbacks on the drawing area */

    XtAddCallback(target, XmNdestinationCallback, 
		  (XtCallbackProc) targetDestinationCallback, NULL);
    XtAddCallback(target, XmNconvertCallback, 
		  (XtCallbackProc) targetConvertCallback, NULL);

    /* Add other callbacks on the drawing area */
    XtAddCallback(target, XmNinputCallback,
		  (XtCallbackProc) inputCallback, NULL);
    
    XtRealizeWidget(toplevel);

    XtAppMainLoop(app_context);
}

/***************************************************
 * function setMode				  
 * Set the mode to PRIMARY, SECONDARY or CLIPBOARD
 ***************************************************/
void 
setMode(Widget widget, int param, XmAnyCallbackStruct *callback_data)
{
  Tmode = param;
}

void 
quitCB(Widget widget, char *tag, XmAnyCallbackStruct *callback_data)
{
  exit(0);
}

/******************************************************************
 * cut, copy and paste perform the apropriate actions
 * depending on which mode we're in.  They use the widget writer's
 * API for this.  Normally application developers won't need to
 * call these,  but this is a pseudo widget writing case as we're
 * adding interchange behavior to drawing area.  This is an example
 * of where the widget writing API should be used by app. developers
 ******************************************************************/

void 
cut(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySource(target, 0);
    XmToggleButtonSetState(op, True, False);
  } else if (Tmode == Mnamed) {
    XmeNamedSource(target, XInternAtom(XtDisplay(target), _NAMED, False), 0);
    XmToggleButtonSetState(onamed, True, False);
  } else if (Tmode == Msecondary) {
    XmeSecondarySource(target, 0);
    XmeSecondaryTransfer(target, 
			 XInternAtom(XtDisplay(target), 
				     "COMPOUND_TEXT", False),
			 XmMOVE, 0);
  } else if (Tmode == Mclipboard)
    XmeClipboardSource(target, XmMOVE, 0, generic_label);
}


void 
copy(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySource(target, 0);
    XmToggleButtonSetState(op, True, False); 
  } else if (Tmode == Mnamed) {
    XmeNamedSource(target, XInternAtom(XtDisplay(target), _NAMED, False), 0);
    XmToggleButtonSetState(onamed, True, False);
  } else if (Tmode == Msecondary) {
    XmeSecondarySource(target, 0);
    XmeSecondaryTransfer(target, 
			 XInternAtom(XtDisplay(target), 
				     "COMPOUND_TEXT", False),
			 XmCOPY, 0);
  } else if (Tmode == Mclipboard)
    XmeClipboardSource(target, XmCOPY, 0, generic_label);
}

void 
paste(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySink(target, XmCOPY, NULL, 0);
  } else if (Tmode == Mnamed) {
    XmeNamedSink(target, XInternAtom(XtDisplay(target), _NAMED, False),
		 XmCOPY, NULL, 0);
  } else if (Tmode == Msecondary) {
    XmeSecondarySink(target, 0);
    XmToggleButtonSetState(omd, True, False);
  } else if (Tmode == Mclipboard)
    XmeClipboardSink(target, XmCOPY, 0);
}

void 
pasteMove(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  if (Tmode == Mprimary) {
    XmePrimarySink(target, XmMOVE, 0, 0);
  } else if (Tmode == Mnamed) {
    XmeNamedSink(target, XInternAtom(XtDisplay(target), _NAMED, False),
		 XmMOVE, NULL, 0);
  } else if (Tmode == Msecondary) {
    XmeSecondarySink(target, 0);
    XmToggleButtonSetState(omd, True, False);
  }
  else if (Tmode == Mclipboard)
    XmeClipboardSink(target, XmMOVE, 0);
}

/* Error handler for X protocol errors.  Display error text in text1,2 */

static int 
ErrorHandler(Display *display, XErrorEvent *event)
{
  char buffer[80];
  char errortext[100];
  XmString tmp;

  XGetErrorText(display, event -> error_code, errortext, 100);

  tmp = XmStringCreateLocalized(errortext);
  XmCSTextSetString(text2, tmp);
  XmStringFree(tmp);

  sprintf(buffer, "XID %d serial %d major %d minor %d",
	  event -> resourceid,
	  event -> request_code,
	  event -> minor_code);

  tmp = XmStringCreateLocalized(buffer);
  XmCSTextSetString(text1, tmp);
  XmStringFree(tmp);

  return 0;
}

void 
inputCallback(Widget w, XtPointer ignore, XmDrawingAreaCallbackStruct *cbstruct)
{
  XEvent *event = cbstruct -> event;

  if (event -> type == ButtonPress &&
      event -> xbutton.button == 2) {
    XmeDragSource(w, NULL, event, NULL, 0);
  }
}

static void 
NOP(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  /* We do nothing in the action. */
}

