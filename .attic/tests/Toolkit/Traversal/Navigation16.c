/* $XConsortium: Navigation16.c /main/5 1995/07/15 21:11:54 drk $ */
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

#include <testlib.h>
#include <Xm/XmP.h>

typedef struct wstruct
{
    Widget w;
    Widget (*createFunc)(Widget, String, ArgList, Cardinal);
    char* name;
} WStruct;

#define NUM_GADGETS 5

static Widget rowcol1;

static WStruct Widgets[NUM_GADGETS] =
{
    {NULL, XmCreateLabelGadget, "labelG1"},
    {NULL, XmCreateSeparatorGadget, "separatorG1"},
    {NULL, XmCreatePushButtonGadget, "pushBG1"},
    {NULL, XmCreateToggleButtonGadget, "toggleBG1"},
    {NULL, XmCreateArrowButtonGadget, "arrowBG1"}
};

void printWidgetName(Widget w)
{
    int i;
    char* name;

    name = "Unknown widget";
    if (w == rowcol1)
       name = "rowcol1";
    else if (w == Shell1)
	name = "Shell1";
    else
	for (i = 0; i < NUM_GADGETS; i++)
	    if (w == Widgets[i].w)
		{
		    name = Widgets[i].name;
		    break;
		}
    printf("%s", name);
}

void focusCB(Widget w, XtPointer client_data, XtPointer call_data )
{
    XmFocusMovedCallbackStruct* cbs;

    cbs = (XmFocusMovedCallbackStruct*)call_data;

    printf("Focus moved from ");

    printWidgetName(cbs->old_focus);

    printf(" to ");

    printWidgetName(cbs->new_focus);

    printf("\n");
}

main (int argc, char ** argv)
{
   Arg args[10];
   int i;

   /* initialize Toolkit */
   CommonTestInit(argc, argv);

   rowcol1 = XmCreateRowColumn(Shell1, "rowcol1", args, 0);

   for (i = 0; i < NUM_GADGETS; i++) {
       Widgets[i].w
	   = Widgets[i].createFunc(rowcol1, Widgets[i].name, args, 0);
       XtManageChild(Widgets[i].w);
   }
   XtManageChild(rowcol1);

   XtRealizeWidget(Shell1);

   CommonPause();

   XmeAddFocusChangeCallback(Shell1, focusCB, NULL);

   CommonPause();

   XtAppMainLoop(app_context);
}
