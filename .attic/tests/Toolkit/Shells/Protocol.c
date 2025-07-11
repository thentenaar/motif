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
static char rcsid[] = "$XConsortium: Protocol.c /main/8 1995/07/13 19:20:41 drk $"
#endif
#endif


#include <testlib.h>

Widget  	RowCol;
Widget 		ShortMenu, LongMenu, NoMenu;
Atom		mwm_messages;

XmString 	text;

XFontStruct *fnt;
XmFontList 	font;

#define ENTRY_1		1
#define ENTRY_2		2
#define ENTRY_3		3

static void addShortMenu();
static void addLongMenu();
static void addNoMenu();


static Widget ShellAncestor(widget)
Widget widget;
{

    Widget topWidget;

   topWidget = widget;
   while (widget != NULL && !XtIsShell(topWidget))
    	topWidget = XtParent(topWidget);
   return(topWidget);

}


static void EntrySelected(w, client_data, call_data)
Widget	    w;
XtPointer	client_data, call_data;
{
    printf("entry %d selected\n", (int)(long)client_data);
}


static void addShortMenu(w, client_data, call_data)
Widget		w;
XtPointer	client_data;
XtPointer	call_data;
{

	Widget	shell;
	Arg		args[1];
	int			n;
	static Atom	atoms[1];
	static int 	status = 0;
	static String	menu = "entry_1	f.send_msg 1";

    atoms[0] = (Atom)ENTRY_1;

	shell = ShellAncestor(w);
	
	n = 0;
	XtSetArg(args[n], XmNmwmMenu, menu); n++;
	XtSetValues(Shell1, args, n);

	switch(status) {

  		case 0:
			XmAddProtocols(Shell1, mwm_messages, atoms, XtNumber(atoms));

			XmAddProtocolCallback(Shell1, mwm_messages, ENTRY_1, 
								  EntrySelected, (XtPointer) ENTRY_1);

			if (XtIsRealized(Shell1)) {
				XtUnmapWidget(Shell1);
				XtMapWidget(Shell1);
			}
			status = 1;
			break;
	
		case 1:
			XmRemoveProtocolCallback(Shell1, mwm_messages, ENTRY_1, 
									 EntrySelected, (XtPointer) ENTRY_1);
			status = 2;
			break;
	
		case 2:
			XmAddProtocolCallback(Shell1, mwm_messages, ENTRY_1, 
								  EntrySelected, (XtPointer) ENTRY_1);
			status = 1;
			break;

		default:

			break;

	}

}


static void addLongMenu(w, client_data, call_data)
Widget      w;
XtPointer   client_data;
XtPointer   call_data;
{

    Widget		shell;
    Arg			arg;
    static String	menu = 
      "entry_1	f.send_msg  1 \n\
       entry_2	f.send_msg  2 \n\
       entry_3	f.send_msg  3";
    static Atom		atoms[] = { (Atom)ENTRY_1, (Atom)ENTRY_2, (Atom)ENTRY_3};
    static int status = 0;

    shell = ShellAncestor(w);

    XtSetArg(arg, XmNmwmMenu, menu);
    XtSetValues(Shell1, &arg, 1);

    switch(status) {

    	case 0:
    		XmAddProtocols(Shell1, mwm_messages, atoms, XtNumber(atoms));

    		XmAddProtocolCallback(Shell1, mwm_messages, ENTRY_1, 
								  EntrySelected, (XtPointer) ENTRY_1);
    		XmAddProtocolCallback(Shell1, mwm_messages, ENTRY_2, 
								  EntrySelected, (XtPointer) ENTRY_2);
    		XmAddProtocolCallback(Shell1, mwm_messages, ENTRY_3, 
								  EntrySelected, (XtPointer) ENTRY_3);
        
    		status = 1;
			break;
	
    	case 1:
			XmDeactivateWMProtocol(Shell1, mwm_messages);

			status = 2;
			break;

    	case 2:
			XmActivateWMProtocol(Shell1, mwm_messages);

			status = 1;
			break;

    	default:

			break;

    }

    if (XtIsRealized(Shell1)) {
		XtUnmapWidget(Shell1);
		XtMapWidget(Shell1);
    }

}


static void addNoMenu(w, client_data, call_data)
Widget      w;
XtPointer     client_data;
XtPointer     call_data;
{

    Widget	shell;
    Arg		args[1];
	int		n;

    shell = ShellAncestor(w);
	 
	n = 0;
    XtSetArg(args[n], XmNmwmMenu, ""); n++;
    XtSetValues(Shell1, args, n);

    if (XtIsRealized(Shell1)) {
		XtUnmapWidget(Shell1);
		XtMapWidget(Shell1);
    }

}


void main(argc, argv)
unsigned int argc;
char **argv;
{

    Cardinal	n;
    Arg			args[MAX_ARGS];


    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
	XtSetValues(Shell1, args, n);

    /*
     * intern the atoms and add mwm_messages to wm_protocols so that
     * mwm will look for mwm_messages property changes.
     */
    mwm_messages = XmInternAtom(XtDisplay(Shell1), _XA_MWM_MESSAGES, False);
    XmAddWMProtocols(Shell1, &mwm_messages, 1);

	font = CommonGetFontList("6x13");

    n = 0;
    XtSetArg(args[n], XmNborderWidth, 2); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    RowCol = XmCreateRowColumn(Shell1, "RowCol", args, n);
    XtManageChild(RowCol);

    n = 0;    
#ifdef MOTIF1_1
    text = XmStringCreateLtoR("ShortMenu", XmSTRING_DEFAULT_CHARSET);
#else
    text = XmStringCreateLtoR("ShortMenu", XmFONTLIST_DEFAULT_TAG);
#endif
    XtSetArg(args[n], XmNfontList, font); n++;
    XtSetArg(args[n], XmNlabelString, text); n++;
    XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    ShortMenu = XmCreatePushButton(RowCol, "ShortMenu", args, n);
    XtManageChild(ShortMenu);
	XtAddCallback (ShortMenu, XmNactivateCallback, addShortMenu, NULL);

    n = 0;    
#ifdef MOTIF1_1
    text = XmStringCreateLtoR("LongMenu", XmSTRING_DEFAULT_CHARSET);
#else
    text = XmStringCreateLtoR("LongMenu", XmFONTLIST_DEFAULT_TAG);
#endif
    XtSetArg(args[n], XmNfontList, font); n++;
    XtSetArg(args[n], XmNlabelString, text); n++;
    XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    LongMenu = XmCreatePushButton(RowCol, "LongMenu", args, n);
    XtManageChild(LongMenu);
	XtAddCallback (LongMenu, XmNactivateCallback, addLongMenu, NULL);
    
    n = 0;    
#ifdef MOTIF1_1
    text = XmStringCreateLtoR("NoMenu", XmSTRING_DEFAULT_CHARSET);
#else
    text = XmStringCreateLtoR("NoMenu", XmFONTLIST_DEFAULT_TAG);
#endif
    XtSetArg(args[n], XmNfontList, font); n++;
    XtSetArg(args[n], XmNlabelString, text); n++;
    XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    NoMenu = XmCreatePushButton(RowCol, "NoMenu", args, n);
    XtManageChild(NoMenu);
	XtAddCallback (NoMenu, XmNactivateCallback, addNoMenu, NULL);

	XmStringFree(text);

    n = 0;
    XtSetArg(args[n], XmNdeleteResponse, XmUNMAP); n++;
    XtSetValues(Shell1, args, n);

    XtRealizeWidget(Shell1);

    CommonPause();

    CommonPause();

    CommonPause();

    CommonPause();

    CommonPause();

    XtPopup(Shell1, XtGrabNone);

    CommonPause();

    XtPopup(Shell1, XtGrabNone);

    CommonPause();

    CommonPause();

    XtAppMainLoop(app_context);

}
