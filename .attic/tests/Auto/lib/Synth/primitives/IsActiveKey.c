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
static char rcsid[] = "$XConsortium: IsActiveKey.c /main/7 1995/07/14 11:38:55 drk $"
#endif
#endif
/***********************************************************************
  	@(#)IsActiveKey.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/30/90 SJS add to sccs
            06/11/90 PSN fixed modifier key mask checking bug
	Calls:

	Summary:
            Returns True if the key is the active key.

        INPUTS:
            key_code - the key in question (KeySpace, KeyReturn, etc.)
        
        OUTPUTS:
            none

        RETURNS:
            True = specified key is the active key
            False = otherwise
************************************************************************/

#include <AutoMessages.h>
#include "xislib.h"

int xisIsActiveKey(modifier_keys,key_code)
unsigned int modifier_keys;
int key_code;
{
    if (!xisInform.is_valid)
        AutoMessage(_AutoMessages[WARNMSG49]);

    return ( (xisInform.key_code == key_code) && 
	     (xisInform.modifier_key_status == modifier_keys) );

} /* End xisIsActiveKey() */
