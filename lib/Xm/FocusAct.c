/* $XConsortium: FocusAct.c /main/5 1995/07/15 20:50:59 drk $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"
#include "PrimitiveI.h"
#include "UniqueEvnI.h"

/**********************************************************************
 *
 * _XmMenuButtonTakeFocus
 *
 *********************************************************************/
void _XmMenuButtonTakeFocus(Widget wid, XEvent *event, String *params,
                            Cardinal *num_params)
{
	/* Support menu replay, free server input queue until next button event */
	(void)params;
	(void)num_params;
	XAllowEvents(XtDisplay(wid), SyncPointer, CurrentTime);
	XmProcessTraversal(wid, XmTRAVERSE_CURRENT);
	_XmRecordEvent(event);
}

/**********************************************************************
 *
 * _XmMenuButtonTakeFocusUp
 *
 *********************************************************************/
void _XmMenuButtonTakeFocusUp(Widget wid, XEvent *event, String *params,
                              Cardinal *num_params)
{
	/* Support menu replay, free server input queue until next button event */
	(void)params;
	(void)num_params;
	XAllowEvents(XtDisplay(wid), SyncPointer, CurrentTime);
	_XmRecordEvent(event);
}

