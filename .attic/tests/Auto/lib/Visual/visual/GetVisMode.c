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
static char rcsid[] = "$XConsortium: GetVisMode.c /main/7 1995/07/14 11:50:31 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetVisMode.c	1.4.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to SCCS
	Calls:

	Summary:
            Gets the visual mode.

        INPUTS:
            none
    
        OUTPUTS:
            none

        RETURNS:
            Returns one of: VisualBatchCompare, VisualInteractiveCompare,
                            or VisualGeneratePixmaps.
************************************************************************/

#include "vislib.h"

int mvsGetVisualMode()
{
    return(mvsVisualMode);

} /* End mvsGetVisualMode() */
