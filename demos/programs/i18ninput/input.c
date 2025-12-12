/* $XConsortium: input.c /main/5 1995/07/15 20:45:41 drk $ */
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
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <X11/Xlocale.h>
#include <Xm/Protocols.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/MwmUtil.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>
#include <Xmd/Help.h>

#define WI_INPUT 0
#define WI_MAINWINDOW 1
#define WI_MENUBAR 2
#define WI_FILEMENU 3
#define WI_EXIT 4
#define WI_FILE 5
#define WI_MAIN 6
#define WI_SCROLLEDTEXT 7
#define WI_STFONT 8
#define WI_SCROLLEDTEXT1 20
#define WI_SEPARATOR 21
#define WI_TEXT 22
#define WI_TEXT1 35
#define WI_SEPARATOR1 36
#define WI_TEXTFIELD 37
#define WI_TEXTFIELD1 50
#define WI_HELPMENU 51
#define WI_OVERVIEW 52
#define WI_INPUTINFO 53
#define WI_HELP 54

#define WI_FORM 0
#define WI_FONT 1
#define WI_RADIOBOX 2
#define WI_BUTTON1 3
#define WI_BUTTON2 4
#define WI_BUTTON3 5
#define WI_LABEL 6
#define WI_COLOR 7
#define WI_RADIOBOX1 8
#define WI_BUTTON4 9
#define WI_BUTTON5 10
#define WI_BUTTON6 11
#define WI_LABEL1 12

/* Global data */

typedef struct {
    String locale_string;
    String preedit_string;
    String status_string;
    String im_info_string;
    String no_im_string;
    String no_ic_string;
} ApplicationData, *ApplicationDataPtr;

static ApplicationData app_resources;

static XtResource resources[] = {
  {
    "localeString", "LocaleString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, locale_string),
    XtRImmediate, "Locale:"
  },
  {
    "preeditString", "PreeditString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, preedit_string),
    XtRImmediate, "Preedit style:"
  },
  {
    "statusString", "StatusString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, status_string),
    XtRImmediate, "Status style:"
  },
  {
    "noImString", "NoImString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, no_im_string),
    XtRImmediate, "No input method has been opened"
  },
  {
    "noIcString", "NoIcString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, no_ic_string),
    XtRImmediate, "No input context(s) created"
  },
  {
    "imInfoString", "ImInfoString",
    XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, im_info_string),
    XtRImmediate, "\n\
See VendorShell documentation for more information\n\
on preedit and status styles. For information on\n\
available input methods and invocation of input\n\
methods, refer to your vendor documentation."
  }
};

/* callback functions */
static void ShowIMStatus(Widget widget,
			 XtPointer args,
			 XtPointer calldata);
static void SelectFont(Widget widget,
		       XtPointer args,
		       XtPointer calldata);
static void SelectColor(Widget widget,
			XtPointer args,
			XtPointer calldata);
static void Exit(Widget widget,
		 XtPointer args,
		 XtPointer calldata);
static void Overview(Widget widget,
		     XtPointer args,
		     XtPointer calldata);

/****************************************************************
 *
 *  Interface
 *
 ****************************************************************/

/****************************************************************
 * get_constraint_widget:
 *   Get the right widget for setting form constraints.
 *   Used for the scrolled XmText.
*
 ****************************************************************/
static Widget
get_constraint_widget(Widget child,
		      Widget parent)
{
  Widget w;

  w = child;
  while (XtParent(w) != parent)
    w = XtParent(w);
  return (w);
}



/****************************************************************
 *
 *  Main C code for presentation component
 *
 ****************************************************************/

/**************************************************************
 * form_widget:
 *   Create a form widget with two radio boxes inside a frame,
 *   one for setting fonts and one for setting color.
 *
 **************************************************************/
static void
form_widget(char    * name,
	    Widget    parent,
	    Widget  * warr_ret)
{
  Arg args[16];
  Widget widget_array[13];
  XtCallbackList cbl;
  XtCallbackRec cbs1[2];
  XtCallbackRec cbs2[2];
  int inc;
  int n;

  /***************** object of type : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNfractionBase, 8); n++;
  widget_array[WI_FORM] =
    XmCreateForm(parent, name, args, n);

  /***************** font : XmFrame *****************/
  widget_array[WI_FONT] =
    XmCreateFrame(widget_array[WI_FORM], "font", NULL, 0);

  /***************** radiobox : XmRadioBox *****************/
  n = 0;
  cbl = cbs1;
  cbl->closure = (XtPointer) NULL;
  cbl->callback = SelectFont;
  cbl++;
  if (cbl != cbs1) {
    cbl->callback = NULL;
    XtSetArg(args[n], XmNentryCallback, cbs1); n++;
  }

  widget_array[WI_RADIOBOX] =
    XmCreateRadioBox(widget_array[WI_FONT], "radiobox", args, n);

  /***************** button1 : XmToggleButton *****************/
  widget_array[WI_BUTTON1] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX], "button1", NULL, 0);

  /***************** button2 : XmToggleButton *****************/
  widget_array[WI_BUTTON2] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX], "button2", NULL, 0);

  /***************** button3 : XmToggleButton *****************/
  widget_array[WI_BUTTON3] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX], "button3", NULL, 0);

  /***************** label : XmLabel *****************/
  n = 0;
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
  widget_array[WI_LABEL] =
    XmCreateLabel(widget_array[WI_FONT], "label", args, n);

  /***************** color : XmFrame *****************/
  widget_array[WI_COLOR] =
    XmCreateFrame(widget_array[WI_FORM], "color", NULL, 0);

  /***************** radiobox : XmRadioBox *****************/
  n = 0;
  cbl = cbs2;
  cbl->closure = (XtPointer) NULL;
  cbl->callback = SelectColor;
  cbl++;
  if (cbl != cbs2) {
    cbl->callback = NULL;
    XtSetArg(args[n], XmNentryCallback, cbs2); n++;
  }

  widget_array[WI_RADIOBOX1] =
    XmCreateRadioBox(widget_array[WI_COLOR], "radiobox", args, n);

  /***************** button1 : XmToggleButton *****************/
  n = 0;
  widget_array[WI_BUTTON4] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX1], "button1", args, n);

  /***************** button2 : XmToggleButton *****************/
  n = 0;
  widget_array[WI_BUTTON5] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX1], "button2", args, n);

  /***************** button3 : XmToggleButton *****************/
  n = 0;
  widget_array[WI_BUTTON6] =
    XmCreateToggleButton(widget_array[WI_RADIOBOX1], "button3", args, n);

  /***************** label : XmLabel *****************/
  n = 0;
  XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
  XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_CENTER); n++;
  widget_array[WI_LABEL1] =
    XmCreateLabel(widget_array[WI_COLOR], "label", args, n);


  /***************** font : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightPosition, 4); n++;
  XtSetArg(args[n], XmNtopPosition, 5); n++;
  XtSetValues(widget_array[WI_FONT], args, n);

  XtManageChild(widget_array[WI_BUTTON1]);
  XtManageChild(widget_array[WI_BUTTON2]);
  XtManageChild(widget_array[WI_BUTTON3]);
  XtManageChild(widget_array[WI_RADIOBOX]);
  XtManageChild(widget_array[WI_LABEL]);
  XtManageChild(widget_array[WI_FONT]);

  /***************** color : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, widget_array[WI_FONT]); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopPosition, 5); n++;
  XtSetValues(widget_array[WI_COLOR], args, n);

  XtManageChild(widget_array[WI_BUTTON4]);
  XtManageChild(widget_array[WI_BUTTON5]);
  XtManageChild(widget_array[WI_BUTTON6]);
  XtManageChild(widget_array[WI_RADIOBOX1]);
  XtManageChild(widget_array[WI_LABEL1]);
  XtManageChild(widget_array[WI_COLOR]);
  XtManageChild(widget_array[WI_FORM]);

  if (warr_ret != NULL) {
    (void) memcpy((char *)warr_ret,
                  (char *)widget_array,
           sizeof(Widget)*13);
  }

}

/**************************************************************
 * create_input_widget:
 *   Creates the application shell and all children except the
 *   two dialogs.
 **************************************************************/
static Widget
create_input_widget(char    * name,
		    Display * display,
		    int       argc,
		    char   ** argv)
{
  Arg args[18];
  Arg pargs[5];
  Widget tmpw;
  Widget widget_array[60];
  int inc;
  int n;
  int pn;

  /***************** ApplicationShell *****************/
  n = 0;
  /* Setup argc & argv attribute */
  XtSetArg(args[n], XtNargc, argc); n++;
  XtSetArg(args[n], XtNargv, argv); n++;
  XtSetArg(args[n], XtNallowShellResize, True); n++; /* to allow widgets to
							change size when font
							changes */

  widget_array[WI_INPUT] =
    XtAppCreateShell(name, "I18ninput", applicationShellWidgetClass,
		     display, args, n);

  /***************** mainWindow : XmMainWindow *****************/
  widget_array[WI_MAINWINDOW] =
    XmCreateMainWindow(widget_array[WI_INPUT], "mainWindow", NULL, 0);

  /***************** menuBar : XmMenuBar *****************/
  widget_array[WI_MENUBAR] =
    XmCreateMenuBar(widget_array[WI_MAINWINDOW], "menuBar", NULL, 0);

  /***************** filemenu : XmPulldownMenu *****************/
  widget_array[WI_FILEMENU] =
    XmCreatePulldownMenu(widget_array[WI_MENUBAR], "filemenu", NULL, 0);

  /***************** exit : XmPushButton *****************/
  widget_array[WI_EXIT] =
    XmCreatePushButton(widget_array[WI_FILEMENU], "Exit", NULL, 0);
  XtAddCallback(widget_array[WI_EXIT],
		XmNactivateCallback, (XtCallbackProc)Exit, NULL);

  /***************** file : XmCascadeButton *****************/
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, widget_array[WI_FILEMENU]); n++;
  widget_array[WI_FILE] =
    XmCreateCascadeButton(widget_array[WI_MENUBAR], "File", args, n);

  /***************** helpmenu : XmPulldownMenu *****************/
  widget_array[WI_HELPMENU] =
    XmCreatePulldownMenu(widget_array[WI_MENUBAR], "helpmenu", NULL, 0);

  /***************** overview : XmPushButton *****************/
  widget_array[WI_OVERVIEW] =
    XmCreatePushButton(widget_array[WI_HELPMENU], "Overview", NULL, 0);
  XtAddCallback(widget_array[WI_OVERVIEW],
		XmNactivateCallback, (XtCallbackProc)Overview, NULL);

  /***************** input : XmPushButton *****************/
  widget_array[WI_INPUTINFO] =
    XmCreatePushButton(widget_array[WI_HELPMENU], "InputInfo", NULL, 0);
  XtAddCallback(widget_array[WI_INPUTINFO],
		XmNactivateCallback, (XtCallbackProc)ShowIMStatus, NULL);

  /***************** help : XmCascadeButton *****************/
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, widget_array[WI_HELPMENU]); n++;
  widget_array[WI_HELP] =
    XmCreateCascadeButton(widget_array[WI_MENUBAR], "Help", args, n);

  n = 0;
  XtSetArg(args[n], XmNmenuHelpWidget, widget_array[WI_HELP]); n++;
  XtSetValues(widget_array[WI_MENUBAR], args, n);

  /***************** main : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNfractionBase, 3); n++;
  widget_array[WI_MAIN] =
    XmCreateForm(widget_array[WI_MAINWINDOW], "main", args, n);

  /***************** scrolledtext : XmForm *****************/

  form_widget("scrolledtext", widget_array[WI_MAIN],
	      &(widget_array[WI_SCROLLEDTEXT]));

  /***************** scrolledText : XmScrolledText *****************/
  n = 0;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  XtSetArg(args[n], XmNrows, 8); n++;
  XtSetArg(args[n], XmNcolumns, 12); n++;
  widget_array[WI_SCROLLEDTEXT1] =
    XmCreateScrolledText(widget_array[WI_SCROLLEDTEXT], "scrolledText",
			 args, n);

  /***************** separator : XmSeparator *****************/
  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg(args[n], XmNshadowThickness, 4); n++;
  widget_array[WI_SEPARATOR] =
    XmCreateSeparator(widget_array[WI_MAIN], "separator", args, n);

  /***************** text : XmForm *****************/

  form_widget("text", widget_array[WI_MAIN], &(widget_array[WI_TEXT]));

  /***************** text : XmText *****************/
  n = 0;
  XtSetArg(args[n], XmNcolumns, 12); n++;
  widget_array[WI_TEXT1] =
    XmCreateText(widget_array[WI_TEXT], "Text", args, n);

  /***************** separator1 : XmSeparator *****************/
  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg(args[n], XmNshadowThickness, 4); n++;
  widget_array[WI_SEPARATOR1] =
    XmCreateSeparator(widget_array[WI_MAIN], "separator1", args, n);

  /***************** textfield : XmForm *****************/

  form_widget("textfield", widget_array[WI_MAIN],
	      &(widget_array[WI_TEXTFIELD]));

  /***************** textField : XmTextField *****************/
  n = 0;
  XtSetArg(args[n], XmNcolumns, 12); n++;
  widget_array[WI_TEXTFIELD1] =
    XmCreateTextField(widget_array[WI_TEXTFIELD], "textField", args, n);

  XtManageChild(widget_array[WI_EXIT]);
  XtManageChild(widget_array[WI_FILE]);
  XtManageChild(widget_array[WI_OVERVIEW]);
  XtManageChild(widget_array[WI_INPUTINFO]);
  XtManageChild(widget_array[WI_HELP]);
  XtManageChild(widget_array[WI_MENUBAR]);

  /***************** scrolledtext : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightPosition, 1); n++;
  XtSetValues(widget_array[WI_SCROLLEDTEXT], args, n);

  /***************** scrolledText : XmScrolledText *****************/
  pn = 0;
  XtSetArg(pargs[pn], XmNleftAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNrightAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNtopAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNbottomAttachment, XmATTACH_WIDGET); pn++;
  XtSetArg(pargs[pn], XmNbottomWidget, widget_array[WI_STFONT]); pn++;
  tmpw = get_constraint_widget(widget_array[WI_SCROLLEDTEXT1],
			       widget_array[WI_SCROLLEDTEXT]);
  if (tmpw)
    XtSetValues(tmpw, pargs, pn);

  XtManageChild(widget_array[WI_SCROLLEDTEXT1]);
  XtManageChild(widget_array[WI_SCROLLEDTEXT]);

  /***************** separator : XmSeparator *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftWidget, widget_array[WI_SCROLLEDTEXT]); n++;
  XtSetValues(widget_array[WI_SEPARATOR], args, n);

  XtManageChild(widget_array[WI_SEPARATOR]);

  /***************** text : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, widget_array[WI_SEPARATOR]); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 2); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetValues(widget_array[WI_TEXT], args, n);

  /***************** text : XmText *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
  XtSetValues(widget_array[WI_TEXT1], args, n);

  XtManageChild(widget_array[WI_TEXT1]);
  XtManageChild(widget_array[WI_TEXT]);

  /***************** separator1 : XmSeparator *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftWidget, widget_array[WI_TEXT]); n++;
  XtSetValues(widget_array[WI_SEPARATOR1], args, n);

  XtManageChild(widget_array[WI_SEPARATOR1]);

  /***************** textfield : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, widget_array[WI_SEPARATOR1]); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetValues(widget_array[WI_TEXTFIELD], args, n);

  /***************** textField : XmTextField *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
  XtSetValues(widget_array[WI_TEXTFIELD1], args, n);

  XtManageChild(widget_array[WI_TEXTFIELD1]);
  XtManageChild(widget_array[WI_TEXTFIELD]);
  XtManageChild(widget_array[WI_MAIN]);
  XtManageChild(widget_array[WI_MAINWINDOW]);

  /*
   *   Return the first created widget.
   */
  return widget_array[WI_INPUT];
}

/****************************************************************
 *
 *  Dialog
 *
 ****************************************************************/

/****************************************************************
 * SelectFont:
 *   Set the font of the Text child of our parent form to the
 *
 ****************************************************************/

/*ARGSUSED*/
static void
SelectFont(Widget widget,
	   XtPointer args,
	   XtPointer calldata)
{
  XmRowColumnCallbackStruct *cbs = (XmRowColumnCallbackStruct*)calldata;
  XmFontList fontlist;
  Widget *children, *swchildren;
  Cardinal num_children, num_swchildren;
  Widget form = widget;
  Boolean set = False;
  int i, j;

  XtVaGetValues(cbs->widget, XmNfontList, &fontlist, XmNset, &set, NULL);
  if (!set)
    return; /* do not change font when toggle is unset */

  /* Find XmForm parent */
  while (form && !XmIsForm(form))
    form = XtParent(form);

  if (form) {
    /* Find an XmText[Field] child and apply XmNfontList */
    XtVaGetValues(form, XmNchildren, &children,
		  XmNnumChildren, &num_children, NULL);
    for (i = 0; i < num_children; i++) {
      if (XmIsText(children[i]) || XmIsTextField(children[i])) {
	XtVaSetValues(children[i], XmNfontList, fontlist, NULL);
	break;
      }
      if (XmIsScrolledWindow(children[i])) {
	/* This may be a scrolled text. Look for an XmText child */
	XtVaGetValues(children[i], XmNchildren, &swchildren,
		      XmNnumChildren, &num_swchildren, NULL);
	for (j = 0; j < num_swchildren; j++)
	  if (XmIsText(swchildren[j])) {
	    XtVaSetValues(swchildren[j], XmNfontList, fontlist, NULL);
	    break;
	  }
	if (j != num_swchildren)
	  break;
      }
    }
  }
}

/*ARGSUSED*/
static void
SelectColor(Widget widget,
	   XtPointer args,
	   XtPointer calldata)
{
  XmRowColumnCallbackStruct *cbs = (XmRowColumnCallbackStruct*)calldata;
  Pixel color;
  Widget *children, *swchildren;
  Cardinal num_children, num_swchildren;
  Widget form = widget;
  Boolean set;
  int i, j;

  XtVaGetValues(cbs->widget, XmNforeground, &color, XmNset, &set, NULL);
  if (!set)
    return; /* do not change color when toggle is unset */

  /* Find XmForm parent */
  while (form && !XmIsForm(form))
    form = XtParent(form);

  if (form) {
    /* Find an XmText[Field] child and apply toggle's
       XmNforeground to text's XmNbackground */
    XtVaGetValues(form, XmNchildren, &children,
		  XmNnumChildren, &num_children, NULL);
    for (i = 0; i < num_children; i++) {
      if (XmIsText(children[i]) || XmIsTextField(children[i]))
	XtVaSetValues(children[i], XmNbackground, color, NULL);

      if (XmIsScrolledWindow(children[i])) {
	/* This may be a scrolled text. Look for an XmText child */
	XtVaGetValues(children[i], XmNchildren, &swchildren,
		      XmNnumChildren, &num_swchildren, NULL);
	for (j = 0; j < num_swchildren; j++)
	  if (XmIsText(swchildren[j])) {
	    XtVaSetValues(swchildren[j], XmNbackground, color, NULL);
	    break;
	  }
	if (j != num_swchildren)
	  break;
      }
    }
  }
}

/*ARGSUSED*/
static void
ShowIMStatus(Widget widget,
	     XtPointer client_data,
	     XtPointer calldata)
{
  static Widget message_dialog = NULL;
  Widget toplevel;
  Widget text_widget;
  XIM xim;
  XIC xic = NULL;
  XIMStyle style;
  char *locale;
  char *preedit_style = "", *status_style = "";
  char *buf;
  XmString message_str;
  Arg args[10];
  int n;

  /* Get input method used */
  xim = XmImGetXIM(widget);

  if (!xim) {
    locale = setlocale(LC_CTYPE, NULL);
  } else {
    locale = XLocaleOfIM(xim);
  }

  /* Find ApplicationShell */
  toplevel = widget;
  while (toplevel && !XtIsApplicationShell(toplevel))
    toplevel = XtParent(toplevel);

  if (!toplevel)
    return;

  /* Get input context info for text widget */
  text_widget = XtNameToWidget(toplevel, "*Text");

  if (text_widget)
    xic = XmImSetXIC(text_widget, NULL);

  if (xic && XGetICValues(xic, XNInputStyle, &style, NULL) == NULL) {
    if (style & XIMPreeditArea)
      preedit_style = "OffTheSpot \t(XIMPreeditArea)";
    else if (style & XIMPreeditPosition)
      preedit_style = "OverTheSpot \t(XIMPreeditPosition)";
    else if (style & XIMPreeditNothing)
      preedit_style = "Root \t(XIMPreeditNothing)";
    else if (style & XIMPreeditNone)
      preedit_style = "None \t(XIMPreeditNone)";

    if (style & XIMStatusArea)
      status_style = "OffTheSpot \t(XIMStatusArea)";
    else if (style & XIMStatusNothing)
      status_style = "Root \t(XIMStatusNothing)";
    else if (style & XIMStatusNone)
      status_style = "None \t(XIMStatusNone)";
  }

  /* Create help text */
  buf = (char *)XtMalloc(strlen(app_resources.locale_string) + strlen(locale) +
			 strlen(app_resources.preedit_string) +
			 strlen(preedit_style) +
			 strlen(app_resources.status_string) +
			 strlen(status_style) +
			 strlen(app_resources.im_info_string) + 100);
  if (xim) {
    if (xic) {
      sprintf(buf,
	    "%s \t%s\n%s \t%s\n%s \t%s\n%s",
	    app_resources.locale_string, locale,
	    app_resources.preedit_string, preedit_style,
	    app_resources.status_string, status_style,
	    app_resources.im_info_string);
    } else {
      sprintf(buf,
	    "%s \t%s\n%s\n%s",
	    app_resources.locale_string, locale,
	    app_resources.no_ic_string,
	    app_resources.im_info_string);
    }
  } else {
    sprintf(buf,
	    "%s \t%s\n%s",
	    app_resources.locale_string, locale,
	    app_resources.no_im_string);
  }
  message_str = XmStringGenerate(buf, NULL, XmCHARSET_TEXT, NULL);
  XtFree((char *)buf);

  /* put help text in a message dialog */
  n = 0;
  XtSetArg(args[n], XmNmessageString, message_str); n++;
  if (message_dialog == NULL) {
    message_dialog = XmCreateMessageDialog(toplevel, "im_info", args, n);
    /* Only keep OK button */
    XtUnmanageChild(XtNameToWidget(message_dialog, "Cancel"));
    XtUnmanageChild(XtNameToWidget(message_dialog, "Help"));
  } else {
    XtSetValues(message_dialog, args, n);
  }
  XtManageChild(message_dialog);
  XmStringFree(message_str);
}


/*ARGSUSED*/
static void
Overview(Widget widget,
	 XtPointer client_data,
	 XtPointer calldata)
{
  Widget toplevel;
  static Widget help_window = NULL;
  Arg args[10];
  int n;
  Position x, y;
  Dimension width, height;

  /* Find ApplicationShell */
  toplevel = widget;
  while (toplevel && !XtIsApplicationShell(toplevel))
    toplevel = XtParent(toplevel);

  if (!toplevel)
    return;

  /* Position dialog */
  n = 0;
  XtSetArg(args[n], XmNx, &x); n++;
  XtSetArg(args[n], XmNy, &y); n++;
  XtSetArg(args[n], XmNwidth, &width); n++;
  XtSetArg(args[n], XmNheight, &height); n++;
  XtGetValues(toplevel, args, n);

  n = 0;
  XtSetArg(args[n], XmNx, x+width/2); n++;
  XtSetArg(args[n], XmNy, y+height/2); n++;

  if (help_window == NULL) {
    XtSetArg(args[n], XmdNhelpFile, "i18ninput"); n++;
    XtSetArg(args[n], XmNrenderTable, NULL); n++; /* to get default */
    help_window = XmdCreateHelpDialog(toplevel, "help", args, n);
  } else {
    XtSetValues(XtParent(help_window), args, n);
  }

  XtManageChild(help_window);
}

/*ARGSUSED*/
static void
Exit(Widget widget,
     XtPointer args,
     XtPointer calldata)
{
   exit(0);
}

int
dialog_init(int *argc, char **argv, Display *dpy)
{
  Widget appl_w;

  appl_w = create_input_widget("input", dpy, *argc, argv);
  XtGetApplicationResources(appl_w,
			    &app_resources,
			    resources,
			    XtNumber(resources),
			    NULL, 0);
  XtRealizeWidget(appl_w);
  XtPopup(appl_w, XtGrabNone);

  return 0;
}

