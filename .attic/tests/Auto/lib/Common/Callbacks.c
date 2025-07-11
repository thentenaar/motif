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
static char rcsid[] = "$XConsortium: Callbacks.c /main/9 1995/07/14 10:54:50 drk $"
#endif
#endif
#include "testlib.h"

char *REASONS[50] = {
	"XmCR_NONE",
	"XmCR_HELP",
	"XmCR_VALUE_CHANGED",
	"XmCR_INCREMENT",
	"XmCR_DECREMENT",
	"XmCR_PAGE_INCREMENT",
	"XmCR_PAGE_DECREMENT",
	"XmCR_TO_TOP",
	"XmCR_TO_BOTTOM",
	"XmCR_DRAG",
	"XmCR_ACTIVATE",
	"XmCR_ARM",
	"XmCR_DISARM",
	"NO_REASON",
	"NO_REASON",
	"NO_REASON",
	"XmCR_MAP",
	"XmCR_UNMAP",
	"XmCR_FOCUS",
	"XmCR_LOSING_FOCUS",
	"XmCR_MODIFYING_TEXT_VALUE",
	"XmCR_MOVING_INSERT_CURSOR",
	"XmCR_EXECUTE",
	"XmCR_SINGLE_SELECT",
	"XmCR_MULTIPLE_SELECT",
	"XmCR_EXTENDED_SELECT",
	"XmCR_BROWSE_SELECT",
	"XmCR_DEFAULT_ACTION",
	"XmCR_CLIPBOARD_DATA_REQUEST",
	"XmCR_CLIPBOARD_DATA_DELETE",
	"XmCR_CASCADING",
	"XmCR_OK",
	"XmCR_CANCEL",
	"NO_REASON",
	"XmCR_APPLY",
	"XmCR_NO_MATCH",
	"XmCR_COMMAND_ENTERED ",
	"XmCR_COMMAND_CHANGED",
	"XmCR_EXPOSE",
	"XmCR_RESIZE",
	"XmCR_INPUT",
	"XmCR_GAIN_PRIMARY",
	"XmCR_LOSE_PRIMARY",
};

/*************************************************************************
*									 *
*	Name:		CommonGenericCB()				 *
*									 *
*	Purpose:	This is a generic callback routine that will     *
*			simply fprintf out a message that a callback has *
*		    	been called and what the reason was for the      *
*			callback. 					 *
*									 *
*	Returns:	Nothing						 *
*									 *
*************************************************************************/

void CommonGenericCB(Widget w, XtPointer client_data,
		     XtPointer call_data)

{
    char *reason;
    int  index;

    index = ((XmAnyCallbackStruct *)call_data)->reason;
    reason = REASONS[index];
    fprintf(stdout, "Generic Callback called:  Reason - %s\n", reason);
    if (client_data)
	fprintf(stdout, "    Client Data:  %s\n", (char *)client_data);
}
