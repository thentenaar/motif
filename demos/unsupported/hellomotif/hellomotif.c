/* $XConsortium: hellomotif.c /main/5 1995/07/15 20:47:36 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <stdlib.h>
#include <Xm/XmAll.h>        

#define APP_CLASS "XmdHelloMotif"

/* Although it's really bad to hardcode Positions, keep this for
   a close hellomotif_in_c equivalence to the uil version */
#define NEW_BUTTON_X 11
/* not mentioning hardcoding labels... */
#define NEW_BUTTON_LABEL  "Goodbye\nWorld!"


static void Activate(
        Widget	widget,
	XtPointer tag,
	XtPointer data);


/*
 * Fallbacks: font, label and position go together in this demo 
 */
static String fallbacks[] = {
"hellomotif_in_c.allowShellResize: True",
"hellomotif_in_c*fontList: fixed",
"*helloworld_label.labelString:Press button once\\nto change label;\\ntwice to exit.",
"*helloworld_button.x: 15",
"*helloworld_button.y: 60",
"*helloworld_button.labelString:Hello\nWorld!",
NULL
};


/*
 *  Main program: create a shell, a pure bb with a label and a button 
 *                underneath it. Attach a callback to the button.
 */
int main(int argc, char *argv[])
{
    XtAppContext        app_context;
    Widget              top_level, helloworld_main,
                        helloworld_label, helloworld_button ;

    /* 
     * Initialize Xt and create a resizable shell 
     */

    top_level = XtVaAppInitialize(&app_context, APP_CLASS, 
				  NULL, 0, &argc, argv, fallbacks, NULL); 

    /*
     *  Create the widget tree: first the bb parent.
     */

    helloworld_main = XtVaCreateManagedWidget ("helloworld_main",
					       xmBulletinBoardWidgetClass,
					       top_level, NULL);

    /* the label */
    helloworld_label = XtVaCreateManagedWidget ("helloworld_label",
						xmLabelWidgetClass,
						helloworld_main, 
						NULL);

    /* the button underneath the label */
    helloworld_button = XtVaCreateManagedWidget ("helloworld_button",
						 xmPushButtonWidgetClass,
						 helloworld_main,
						 NULL);
    /* add the callback to change the button label */
    XtAddCallback(helloworld_button, XmNactivateCallback, Activate, NULL);

    /*
     *  Realize the toplevel widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(top_level);

    /*
     *  Loop and process events
     */

    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}


static void 
Activate(
        Widget	widget,
	XtPointer tag,
	XtPointer data)

{
    static Boolean first_time = True ;

    /* First click: change the pushbutton label and recenter */
    if (first_time) {
	XmString new_button_label = XmStringGenerate(NEW_BUTTON_LABEL,
						     NULL, 
						     XmCHARSET_TEXT, NULL);
	XtVaSetValues(widget, 
		      XmNx, NEW_BUTTON_X,
		      XmNlabelString, new_button_label,
		      NULL);

	XmStringFree(new_button_label);
	first_time = False ;
    } else {
	exit(0);
    }
}
