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
static char rcsid[] = "$XConsortium: mvsGetRsnNam.c /main/7 1995/07/14 11:19:08 drk $"
#endif
#endif
/***********************************************************************
	@(#)mvsGetRsnNam.c	1.6.1.1 ver 1/22/91 rel 1 
  	Author: PSN
	History:
            05/24/90 PSN add to SCCS
            05/30/90 PSN For time being fix it, even if it is susceptible to 
			 motif reason define changes.
            08/14/90 SJS fill in "missing" reasons
	Calls:

	Summary:
            Returns a string pointer to the reason name. This file is
	dependent on the values of the reasons in Xm.h !!!!!!!!!!!!!

            INPUTS
                reason - the reason name, defined in Xm.h

            OUTPUTS
                none
  
            RETURNS
                pointer to a statically declared string which names the
            reason
************************************************************************/

#include "mvslib.h"

/****************************************************************************
 *  Callback reasons  from <Xm/Xm.h>
 ****************************************************************************/

static char *mvsReasonName[] = { 
	/* XmCR_NONE           		0 */	"XmCR_NONE",
	/* XmCR_HELP           		1 */	"XmCR_HELP",
	/* XmCR_VALUE_CHANGED		2 */	"XmCR_VALUE_CHANGED",
	/* XmCR_INCREMENT		3 */	"XmCR_INCREMENT",
	/* XmCR_DECREMENT		4 */	"XmCR_DECREMENT",
	/* XmCR_PAGE_INCREMENT		5 */	"XmCR_PAGE_INCREMENT",
	/* XmCR_PAGE_DECREMENT		6 */	"XmCR_PAGE_DECREMENT",
	/* XmCR_TO_TOP			7 */	"XmCR_TO_TOP",
	/* XmCR_TO_BOTTOM		8 */	"XmCR_TO_BOTTOM",
	/* XmCR_DRAG			9 */	"XmCR_DRAG",
	/* XmCR_ACTIVATE		10 */	"XmCR_ACTIVATE",
	/* XmCR_ARM			11 */	"XmCR_ARM",
	/* XmCR_DISARM			12 */	"XmCR_DISARM",
	/*            	 		13 */	"UnknownReason",
	/*             			14 */	"UnknownReason",
	/*             			15 */	"UnknownReason",
	/* XmCR_MAP       		16 */	"XmCR_MAP",
	/* XmCR_UNMAP  			17 */	"XmCR_UNMAP",
	/* XmCR_FOCUS			18 */	"XmCR_FOCUS",
	/* XmCR_LOSING_FOCUS		19 */	"XmCR_LOSING_FOCUS",
	/* XmCR_MODIFYING_TEXT_VALUE	20 */	"XmCR_MODIFYING_TEXT_VALUE",
	/* XmCR_MOVING_INSERT_CURSOR	21 */	"XmCR_MOVING_INSERT_CURSOR",
	/* XmCR_EXECUTE			22 */	"XmCR_EXECUTE",
	/* XmCR_SINGLE_SELECT		23 */	"XmCR_SINGLE_SELECT",
	/* XmCR_MULTIPLE_SELECT		24 */	"XmCR_MULTIPLE_SELECT",
	/* XmCR_EXTENDED_SELECT		25 */	"XmCR_EXTENDED_SELECT",
	/* XmCR_BROWSE_SELECT		26 */	"XmCR_BROWSE_SELECT",
	/* XmCR_DEFAULT_ACTION		27 */	"XmCR_DEFAULT_ACTION",
	/* XmCR_CLIPBOARD_DATA_REQUEST	28 */	"XmCR_CLIPBOARD_DATA_REQUEST",
	/* XmCR_CLIPBOARD_DATA_DELETE	29 */	"XmCR_CLIPBOARD_DATA_DELETE",
	/* XmCR_CASCADING		30 */	"XmCR_CASCADING",
	/* XmCR_OK                      31 */	"XmCR_OK",
	/* XmCR_CANCEL                  32 */	"XmCR_CANCEL",
	/*             			33 */	"UnknownReason",
	/* XmCR_APPLY                   34 */	"XmCR_APPLY",
	/* XmCR_NO_MATCH                35 */	"XmCR_NO_MATCH",
	/* XmCR_COMMAND_ENTERED         36 */	"XmCR_COMMAND_ENTERED",
	/* XmCR_COMMAND_CHANGED         37 */	"XmCR_COMMAND_CHANGED",
	/* XmCR_EXPOSE			38 */	"XmCR_EXPOSE",
	/* XmCR_RESIZE			39 */	"XmCR_RESIZE",
	/* XmCR_INPUT			40 */	"XmCR_INPUT",
	/* XmCR_GAIN_PRIMARY		41 */	"XmCR_GAIN_PRIMARY",
	/* XmCR_LOSE_PRIMARY		42 */	"XmCR_LOSE_PRIMARY",
	/* Unknown			43 */	"UnknownReason"
  };

#define UnknownReason			43

char *mvsGetReasonName(reason)
int	reason;
{
    return( mvsReasonName[reason < UnknownReason ? reason : UnknownReason] );

} /* End mvsGetReasonName() */
