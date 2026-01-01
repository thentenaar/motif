/**
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: xmfonts.c /main/5 1995/07/14 10:08:26 drk $"
#endif
#endif

/**---------------------------------------------------------------------
***
***	file:		xmfonts.c
***
***
Xmfonts.ad:

  xmfonts*XmScrolledWindow.height:	432
  xmfonts*XmScrolledWindow.width:	390
  xmfonts*row_column.XmNnumColumns:	5
  xmfonts*row_column.XmNpacking:        XmPACK_COLUMN

***-------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Frame.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>


/*-------------------------------------------------------------
**	Forward Declarations
*/

static Widget CreateApplication(Widget); /* create main window */
static Widget CreateFontSample(Widget);  /* create font display window */
static Widget CreateHelp(Widget);        /* create help window */

static void SelectFontCB(Widget, XtPointer, XtPointer); /* callback for font buttons */
static void CloseCB(Widget, XtPointer, XtPointer); /*  callback for close button */
static void HelpCB(Widget, XtPointer, XtPointer); /*  callback for help button */
static void QuitCB(Widget, XtPointer, XtPointer); /*  callback for quit button */

/*-------------------------------------------------------------
**	Global Variables
*/

#define MAX_ARGS 20

static XmStringCharSet	charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;


typedef struct {
  char *fontpattern;
  int   maxfont ;    /* number max of font to be displayed */
  int   maxlen ;     /* max length of font name to be displayed */
  Boolean   usegadget ;
} ApplicationData, *ApplicationDataPtr;

ApplicationData AppData;

#define XtNfontPattern "fontPattern"
#define XtCFontPattern "FontPattern"
#define XtNmaxFont     "maxFont"
#define XtCMaxFont     "MaxFont"
#define XtNmaxLen      "maxLen"
#define XtCMaxLen      "MaxLen"
#define XtNuseGadget   "useGadget"
#define XtCUseGadget   "UseGadget"

static XtResource resources[] = {
  { XtNfontPattern, XtCFontPattern, XmRString, sizeof(String),
    XtOffset(ApplicationDataPtr, fontpattern), XmRString, "*" },
  { XtNmaxFont, XtCMaxFont, XmRInt, sizeof(int),
    XtOffset(ApplicationDataPtr, maxfont), XmRImmediate, (caddr_t) 1000 },
  { XtNmaxLen, XtCMaxLen, XmRInt, sizeof(int),
    XtOffset(ApplicationDataPtr, maxlen), XmRImmediate, (caddr_t) 10 },
  { XtNuseGadget, XtCUseGadget, XmRBoolean, sizeof (Boolean),
    XtOffset(ApplicationDataPtr, usegadget), XmRImmediate, (caddr_t) TRUE }
};

static XtAppContext app_context;

/*-------------------------------------------------------------
**	main		- main logic for application
*/
int main(int argc, char *argv[])
{
	Display		*display;	/*  Display		*/
	Widget		app_shell;	/*  ApplicationShell	*/
	Widget		main_window;	/*  MainWindow		*/

	/*	Initialize toolkit and open the display.
	*/
	app_shell = XtAppInitialize(&app_context, "XMdemos", NULL, 0,
			    &argc, argv, NULL, NULL, 0);

	XtGetApplicationResources(app_shell,
			      (XtPointer)&AppData,
			      resources,
			      XtNumber(resources),
			      NULL,
			      0);

	main_window = CreateApplication (app_shell);
	XtRealizeWidget (app_shell);

	/*	Get and dispatch events.
	*/
	XtAppMainLoop (app_context);
	return 0;    /* make compiler happy */
}

/*-------------------------------------------------------------
**	CreateApplication	- create main window
*/
static Widget CreateApplication(Widget parent)
{
	Widget main_window;	/*  MainWindow		*/
	Widget menu_bar;	/*  MenuBar		*/
	Widget menu_pane;	/*  MenuPane		*/
	Widget cascade;	/*  CascadeButton	*/
	Widget frame;		/*  Frame		*/
	Widget swindow;	/*  ScrolledWindow	*/
	Widget row_column;	/*  RowColumn		*/
	Widget button;		/*  PushButtonGadget	*/
	Widget hsb, vsb;	/*  ScrollBars		*/

	Arg args[MAX_ARGS];	/*  arg list		*/
	int n;		/*  arg count		*/

	char *name;
	int count, i;
	char **fontnamelist;
	XmString label_string;

	/*	Create MainWindow.
	*/
	n = 0;
	main_window = XmCreateMainWindow (parent, "main_window", args, n);
	XtManageChild (main_window);

	/*	Create MenuBar in MainWindow.
	*/
	n = 0;
	menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n);
	XtManageChild (menu_bar);

	/*	Create "Exit" PulldownMenu.
	*/
	n = 0;
	menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

	n = 0;
	button = XmCreatePushButton (menu_pane, "Quit", args, n);
	XtManageChild (button);
	XtAddCallback (button, XmNactivateCallback, QuitCB, NULL);

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
	cascade = XmCreateCascadeButton (menu_bar, "Exit", args, n);
	XtManageChild (cascade);


	/*	Create "Help" button.
	*/
	n = 0;
	cascade = XmCreateCascadeButton (menu_bar, "Help", args, n);
	XtManageChild (cascade);
	XtAddCallback (cascade, XmNactivateCallback, HelpCB, NULL);

	n = 0;
	XtSetArg (args[n], XmNmenuHelpWidget, cascade);  n++;
	XtSetValues (menu_bar, args, n);


	/*	Create Frame MainWindow and ScrolledWindow in Frame.
	*/
	n = 0;
	frame = XmCreateFrame (main_window, "frame", args, n);
	XtManageChild (frame);

	n = 0;
	XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++;
	swindow = XmCreateScrolledWindow (frame, "swindow", args, n);
	XtManageChild (swindow);


	/*	Create RowColumn in ScrolledWindow to manage buttons.
	*/
	n = 0;

	row_column = XmCreateRowColumn (swindow, "row_column", args, n);
	XtManageChild (row_column);


	/*	Set MainWindow areas and add tab groups
	*/
	XmMainWindowSetAreas (main_window, menu_bar, NULL, NULL, NULL,
			frame);

	/*	Create a PushButton widget for each font.
		Get the font list from the server and keep
		only the font name whose length is less than
		the given maxLen resource.
	*/

	fontnamelist = XListFonts(XtDisplay(main_window),
				  AppData.fontpattern,
				  AppData.maxfont, &count);
	if (!fontnamelist) {
	    fprintf(stderr,"No font on your server, exit\n");
	    exit(0);
	}

	/*  read one entry each time through the loop  */
	for (i = 0;  i < count;  i++ )
	{
		name = fontnamelist[i] ;
		if (strlen(name) > AppData.maxlen) continue ;

		/*  create PushButton in RowCol  */
		n = 0;
		label_string = XmStringGenerate(name, charset, XmCHARSET_TEXT, NULL);
		XtSetArg (args[n], XmNlabelString, label_string);  n++;
		if (AppData.usegadget)
		    button = XmCreatePushButtonGadget (row_column, name,
						       args, n);
		else
		    button = XmCreatePushButton(row_column, name,
						args, n);
		XtManageChild (button);
		XtAddCallback (button, XmNarmCallback, SelectFontCB, NULL);
		XmStringFree (label_string);
	}

	/*	Return MainWindow.
	*/

	XFreeFontNames(fontnamelist);
	return (main_window);
}

/*-------------------------------------------------------------
**	CreateFontSample	- create font display window
*/
static Widget CreateFontSample(Widget parent)
{
	Widget		message_box;		/*  MessageBox Dialog	*/
	Widget		button;
	Arg		args[MAX_ARGS];		/*  arg list		*/
	int	n;			/*  arg count		*/
	char		*name = NULL;		/*  font name		*/
	XFontStruct	*font = NULL;		/*  font pointer	*/
	XmFontListEntry e;
	XmFontList	fontlist = NULL;	/*  fontlist pointer	*/
	static char	message[BUFSIZ];	/*  text sample		*/
	XmString	name_string = NULL;
	XmString	message_string = NULL;
	XmString	button_string = NULL;

	/* Get font name. */
	n = 0;
	XtSetArg(args[n], XmNlabelString, &name_string); n++;
	XtGetValues(parent, args, n);
	name = XmStringUnparse(name_string, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

	/*	Load font and generate message to display. */
	if (name)
		font = XLoadQueryFont (XtDisplay (XtParent (parent)), name);
	if  (!font)
		sprintf (message, "Unable to load font: %s", name);
	else
	{
		fontlist = XmFontListAppendEntry(
			NULL,
			XmFontListEntryCreate(charset, XmFONT_IS_FONT, font)
		);
		sprintf (message, "\
This is font %s.\n\
The quick brown fox jumps over the lazy dog.", name);
	}
	message_string = XmStringGenerate(message, charset, XmCHARSET_TEXT, NULL);
	button_string  = XmStringGenerate("Close", charset, XmCHARSET_TEXT, NULL);

	/*	Create MessageBox dialog.
	*/
	n = 0;
	XtSetArg (args[n], XmNdialogTitle, name_string);  n++;
	XtSetArg (args[n], XmNokLabelString, button_string);  n++;
	XtSetArg (args[n], XmNmessageString, message_string);  n++;
	message_box = XmCreateMessageDialog (XtParent (XtParent(parent)),
					     "fontbox",
					     args, n);

	button = XtNameToWidget(message_box, "Message");
	if (fontlist)
	{
		n = 0;
		XtSetArg (args[n], XmNfontList, fontlist);  n++;
		XtSetValues(button, args, n);
	}

	XtUnmanageChild(XtNameToWidget(message_box, "Cancel"));
	XtUnmanageChild(XtNameToWidget(message_box, "Help"));

	/*	Free strings and return MessageBox.
	*/
	if (fontlist) XmFontListFree (fontlist);
	if (name_string) XmStringFree (name_string);
	if (message_string) XmStringFree (message_string);
	if (button_string) XmStringFree (button_string);
	return (message_box);
}

/*-------------------------------------------------------------
**	CreateHelp		- create help window
*/
Widget CreateHelp(Widget parent)
{
	Widget		button;
	Widget		message_box;	/*  Message Dialog 	*/
	Arg		args[MAX_ARGS];	/*  arg list		*/
	int	n;		/*  arg count		*/

	static char	message[BUFSIZ];	/*  help text	*/
	XmString	title_string = NULL;
	XmString	message_string = NULL;
	XmString	button_string = NULL;



	/*	Generate message to display.
	*/
	sprintf (message, "\
These are buttons for the fonts in your X11 server whose  \n\
name lengths are below the given maxLen application resource.\n\
The button label is the name of the font.  When you select \n\
a button, a small window will display a sample of the font.  \n\n\
Press the 'close' button to close a font window.  \n\
Select 'quit' from the 'exit' menu to exit this application.");
	message_string = XmStringGenerate(message, charset, XmCHARSET_TEXT, NULL);
	button_string  = XmStringGenerate("Close", charset, XmCHARSET_TEXT, NULL);
	title_string   = XmStringGenerate("xmfonts help", charset, XmCHARSET_TEXT, NULL);

	/*	Create MessageBox dialog.
	*/
	n = 0;
	XtSetArg (args[n], XmNdialogTitle, title_string);  n++;
	XtSetArg (args[n], XmNokLabelString, button_string);  n++;
	XtSetArg (args[n], XmNmessageString, message_string);  n++;
	message_box = XmCreateMessageDialog (parent, "helpbox", args, n);
	XtUnmanageChild(XtNameToWidget(message_box, "Cancel"));
	XtUnmanageChild(XtNameToWidget(message_box, "Help"));

	/*	Free strings and return MessageBox.
	*/
	if (title_string) XmStringFree (title_string);
	if (message_string) XmStringFree (message_string);
	if (button_string) XmStringFree (button_string);
	return (message_box);
}

/*-------------------------------------------------------------
**	SelectFontCB		- callback for font buttons
*/
static void SelectFontCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget		message_box;


	/*	Create font sample window.
	*/
	message_box = CreateFontSample (w);


	/*	Display font sample window.
	*/
	XtManageChild (message_box);
}

/*-------------------------------------------------------------
**	CloseCB			- callback for close button
*/
static void CloseCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	XFontStruct	*font  = (XFontStruct *)client_data;
	Widget message_box = XtParent(w);
	Widget shell       = XtParent(message_box);

	(void)call_data;

	/*	Free font. */
	if (font) XFreeFont (XtDisplay(w), font);

	/*	Unmanage and destroy widgets. */
	XtUnmanageChild(message_box);
	XtDestroyWidget(shell);
}

/*-------------------------------------------------------------
**	HelpCB			- callback for help button
*/
static void HelpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget message_box; /* MessageBox */

	(void)client_data;
	(void)call_data;

	/*	Create help window. */
	message_box = CreateHelp (w);
	XtManageChild(message_box);
}

/*-------------------------------------------------------------
**	QuitCB			- callback for quit button
*/
static void QuitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	/*	Terminate the application. */
	(void)w;
	(void)client_data;
	(void)call_data;
	XtAppSetExitFlag(app_context);
}

