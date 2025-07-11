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
static char rcsid[] = "$XConsortium: Cutpaste4b.c /main/7 1995/07/13 17:57:00 drk $"
#endif
#endif

#include <X11/Xatom.h>
#include <testlib.h>

Arg	args[20];
Atom	target;

static void GetCBSelection();

#define XA_TIMESTAMP ( XmInternAtom( display, "TIMESTAMP", False ))
#define XA_TARGETS ( XmInternAtom( display, "TARGETS", False ))
#define XA_CLIPBOARD ( XInternAtom( display, "CLIPBOARD", False ))

/*
 * This function is called when the selection value is returned by the
 * Toolkit.  It will try to use the cut buffer if no X selection is found.
 */

static void PasteCallBack(w, closure, selection, type, value, length, format)
     Widget	w;
     XtPointer	closure;
     Atom	*selection,
     		*type;
     XtPointer	value;
     unsigned long *length;
     int	*format;
{
    Widget	text = (Widget) closure;
    char	string[20];

    if (*type == XA_STRING)
    {
	char *text_string;
	char *text_value = (char *)value;

	text_string = (char *) XtMalloc (strlen(text_value) + 12);
	sprintf(text_string, "%s", text_value);
	XmTextSetString(text, text_string);
    }
    else   if (*type == None)
    {
	fprintf(stdout, "Retrieve Failed, No Clipboard selection.\n");
    }
    else
    {
	fprintf(stdout, "Incorrect selection type\n");
    }
} 


static void GetCBSelection (button, client_data, call_data)
     Widget	button;
     XtPointer	client_data,
     		call_data;
{
    Widget 		text = (Widget) client_data;
    XmAnyCallbackStruct	*cb = (XmAnyCallbackStruct *)call_data;

    XtGetSelectionValue(text, XA_CLIPBOARD, XA_STRING, PasteCallBack,
			text, cb->event->xbutton.time);
}


void main (argc, argv)
int argc;
char **argv;
{
    Widget	PanedWindow,
    		PushButton1,
    		Label,
    		Text;
    int		n;

    CommonTestInit(argc, argv);

/*
 * Paned window to hold pushbutton, labels and text widgets
 */

    n = 0;
    PanedWindow = XmCreatePanedWindow(Shell1, "PanedWindow", args, n);
    XtManageChild(PanedWindow);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
	     XmStringCreateLtoR("Get Values", XmSTRING_DEFAULT_CHARSET)); n++;
    PushButton1 = XmCreatePushButton(PanedWindow, "PushButton1", args, n);
    XtManageChild(PushButton1);

    n = 0;
    XtSetArg(args[n], XmNlabelString,
	     XmStringCreateLtoR("String", XmSTRING_DEFAULT_CHARSET)); n++;
    Label = XmCreateLabel(PanedWindow, "Label", args, n);
    XtManageChild(Label);

    n = 0;
    XtSetArg(args[n], XmNresizeWidth, False);  n++;
    XtSetArg(args[n], XmNresizeHeight, False);  n++;
    XtSetArg(args[n], XmNrows, 1);  n++;
    XtSetArg(args[n], XmNcolumns, 15);  n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("6x10"));  n++;

    Text = XmCreateText(PanedWindow, "Text", args, n);
    XtManageChild(Text);

    XtAddCallback (PushButton1, XmNactivateCallback, GetCBSelection, 
		   (XtPointer) Text);

    XtRealizeWidget(Shell1);

    XtSetSelectionTimeout(20000); 

    XtAppMainLoop(app_context);
}
