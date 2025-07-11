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
static char rcsid[] = "$XConsortium: BeginVisual.c /main/7 1995/07/14 11:46:32 drk $"
#endif
#endif
/***********************************************************************
  	@(#)BeginVisual.c	1.2.1.1	Date:1/22/91
  	Author: SJS
	History:
            09/14/90 SJS create, part of visual/mvslib split
	Calls:

	Summary:
            Initializes the visual library local copies of visual
	related structures. Set up other visual related items

        INPUTS:
            display_in     -  the display that is connected to
            trace_msg_func - a function which prints out a trace message
            warning_msg_func - a function which prints out a warning message
            error_msg_func - a function which prints out an error message
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#define DECLARE_VISLIB_GLOBALS_HERE

#include <AutoMessages.h>
#include "vislib.h"

void  visBeginVisual(display_in, trace_msg_func, warning_msg_func,
                     error_msg_func)
Display *display_in;
void (*trace_msg_func)();      /* These three funcs have params like printf */
void (*warning_msg_func)();
void (*error_msg_func)();
{
    static char routine_name[] = "visBeginVisual():";
    char msg_string[125];
    /* Validate incoming parameters */

    visDisplay = display_in;
    visTraceMsg = trace_msg_func;
    visWarningMsg = warning_msg_func;
    visErrorMsg = error_msg_func;

    if (error_msg_func == 0) {
      
        fprintf(stderr, "%s called with NULL error_msg_func\n", routine_name);
        exit(-1);
    }

    if (display_in == 0) {
        sprintf (msg_string, _AutoMessages[VISMSG3], routine_name);
	AutoError(msg_string);
        
    }
    if (trace_msg_func == 0) {
        sprintf (msg_string, _AutoMessages[VISMSG4], routine_name);
	AutoError(msg_string);

    }
    if (warning_msg_func == 0) {
        sprintf (msg_string, _AutoMessages[VISMSG5], routine_name);
	AutoError(msg_string);
    }

    mvsOpenVisual();

} /* end visBeginVisual() */
